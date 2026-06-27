pub mod annealing {
    //! 焼きなましライブラリ
    #![macro_use]
    use itertools::{izip, Itertools};
    use rand::Rng as _;
    use rand_distr::{weighted::WeightedAliasIndex, Distribution as _};
    use rand_pcg::Pcg64Mcg;
    use std::{
        cell::RefCell,
        fmt::{Debug, Display},
        rc::Rc,
        time::{Duration, Instant},
    };

    pub type AnnealingRng = Pcg64Mcg;

    /// 焼きなましの状態
    pub trait State: Clone {
        type Env;
        type Score: Score;

        /// 生スコア（大きいほど良い）
        fn score(&self, env: &Self::Env) -> Self::Score;
    }

    pub trait Score {
        /// 焼きなまし用スコア（大きいほど良い）
        /// デフォルトでは生スコアをそのまま返す
        #[allow(unused_variables)]
        fn annealing_score(&self, progress: f64) -> f64 {
            self.raw_score()
        }

        /// 生スコア
        fn raw_score(&self) -> f64;
    }

    /// 単一の値からなるスコア
    #[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
    pub struct SingleScore(pub f64);

    impl Score for SingleScore {
        fn raw_score(&self) -> f64 {
            self.0
        }
    }

    /// 焼きなましの近傍
    ///
    /// * 受理パターンの流れ: `preprocess()` -> `eval()` -> `postprocess()`
    /// * 却下パターンの流れ: `preprocess()` -> `eval()` -> `rollback()`
    pub trait Neighbor: Sized {
        type Env;
        type State: State<Env = Self::Env>;

        fn generate(
            env: &Self::Env,
            state: &Self::State,
            rng: &mut AnnealingRng,
            progress: f64,
        ) -> Option<Self>;

        /// `eval()` 前の変形操作を行う
        fn preprocess(&mut self, env: &Self::Env, state: &mut Self::State);

        /// 変形後の状態の評価を行う
        ///
        /// # Arguments
        ///
        /// * `env` - 環境
        /// * `state` - 状態
        /// * `progress` - 焼きなましの進捗（[0, 1]の範囲をとる）
        /// * `threshold` - 近傍採用の閾値。新しいスコアがこの値を下回る場合はrejectされる
        ///
        /// # Returns
        ///
        /// 現在の状態のスコア。スコアが `threshold` を下回ることが明らかな場合は `None` を返すことで評価の打ち切りを行うことができる。
        ///
        /// 評価の打ち切りについては[焼きなまし法での評価関数の打ち切り](https://qiita.com/not522/items/cd20b87157d15850d31c)を参照。
        #[allow(unused_variables)]
        fn eval(
            &mut self,
            env: &Self::Env,
            state: &Self::State,
            progress: f64,
            threshold: f64,
        ) -> Option<<Self::State as State>::Score> {
            Some(state.score(env))
        }

        /// `eval()` 後の変形操作を行う（2-optの区間reverse処理など）
        fn postprocess(self, env: &Self::Env, state: &mut Self::State);

        /// `preprocess()` で変形した `state` をロールバックする
        fn rollback(self, env: &Self::Env, state: &mut Self::State);
    }

    pub trait NeighborDelegator: Sized {
        type Env;
        type State: State<Env = Self::Env>;

        fn get_neighbor_weights() -> Vec<f64>;

        fn get_neighbor_names() -> &'static [&'static str];

        fn step(context: &mut AnnealingContext<Self::Env, Self::State>, neighbor_index: usize);
    }

    /// 近傍の委譲をするstructを生成するマクロ
    ///
    /// `delegator_name` は生成するstructの名前、`env` は環境の型、`state` は状態の型を指定する。
    /// 各近傍は `variant => weight` の形式で指定する。
    /// `weight` はその近傍が選ばれる確率の重みを表す。焼きなまし実行時に遅延評価されるため、`weight` は定数である必要はない。
    ///
    /// # Usage
    ///
    /// ```ignore
    /// neighbors! {
    ///     Neighbors, Env, State, [
    ///         TwoOpt => 1.0,
    ///         Swap => 2.0,
    ///         Insert => 0.5,
    ///     ]
    /// }
    ///
    /// let (state, stats) = run_annealing::<Neighbors, SimdSelector, 128>(&input, state, 1e1, 1e-1, Duration::from_millis(1000), 42);
    /// ```
    #[macro_export]
    macro_rules! neighbors {
    (
        $delegator_name:ident, $env:ty, $state:ty, [
            $( $variant:ident => $weight:expr ),+ $(,)?
        ]
    ) => {
        use $crate::annealing::Neighbor as _;

        // generate()内のmatch式で使用するNEIGHBOR_INDEXを定義する
        neighbors! {
            @index 0; $($variant),+
        }

        // 本体
        neighbors! {
            @body
            $delegator_name, $env, $state, [
                $( $variant => $weight ),+
            ]
        }
    };

    (
        @body
        $delegator_name:ident, $env:ty, $state:ty, [
            $( $variant:ident => $weight:expr ),+ $(,)?
        ]
    ) => {
        struct $delegator_name;

        impl $delegator_name {
            thread_local!(static WEIGHTS: std::cell::RefCell<Vec<Box<dyn FnMut() -> f64>>> = std::cell::RefCell::new(vec![$(Box::new(|| $weight), )+]));
            const NAMES: &'static [&'static str] = &[$(stringify!($variant), )+];
        }

        impl $crate::annealing::NeighborDelegator for $delegator_name
        {
            type Env = $env;
            type State = $state;

            fn get_neighbor_weights() -> Vec<f64> {
                // 重みの設定を遅延させる
                Self::WEIGHTS.with(|weights| {
                    weights.borrow_mut().iter_mut().map(|f| f()).collect()
                })
            }

            fn get_neighbor_names() -> &'static [&'static str] {
                Self::NAMES
            }

            fn step(context: &mut $crate::annealing::AnnealingContext<Self::Env, Self::State>, neighbor_index: usize) {
                match neighbor_index {
                    $($variant::NEIGHBOR_INDEX => {
                        let Some(neighbor) = $variant::generate(context.env, &context.state, &mut context.rng, context.progress) else {
                            return;
                        };
                        context.step(neighbor, neighbor_index)
                    }),*,
                    _ => panic!("Invalid neighbor index: {}", neighbor_index),
                }
            }
        }
    };

    // 近傍にNEIGHBOR_INDEXを定義する補助マクロ
    // バリアントが複数残っているとき
    (@index $idx:expr; $head:ident, $($tail:ident),+) => {
        impl $head {
            const NEIGHBOR_INDEX: usize = $idx;
        }

        neighbors! {
            @index ($idx + 1); $($tail),+
        }
    };

    // 最後のバリアント
    (@index $idx:expr; $last:ident) => {
        impl $last {
            const NEIGHBOR_INDEX: usize = $idx;
        }
    };
}

    /// 近傍選択のためのトレイト
    pub trait NeighborSelector {
        fn new(weights: Vec<f64>) -> Self;
        fn select(&self, rng: &mut AnnealingRng) -> usize;
    }

    /// WeightedAliasを使用した近傍選択
    pub struct WeightedAliasSelector {
        alias: WeightedAliasIndex<f64>,
    }

    impl NeighborSelector for WeightedAliasSelector {
        fn new(weights: Vec<f64>) -> Self {
            let alias = WeightedAliasIndex::new(weights)
                .expect("weights must be non-negative and not all zero");
            Self { alias }
        }

        fn select(&self, rng: &mut AnnealingRng) -> usize {
            self.alias.sample(rng)
        }
    }

    /// SIMD命令を使用した近傍選択
    ///
    /// 候補となる近傍は8個までという制限があるが高速
    pub struct SimdSelector {
        prefix_sum: std::arch::x86_64::__m256,
    }

    impl SimdSelector {
        #[target_feature(enable = "bmi1,avx")]
        unsafe fn select_simd(&self, rng: &mut AnnealingRng) -> usize {
            // SIMD命令を使用して、重みの中からランダムな値以上の最初のインデックスを選択する
            // 8要素同時に比較してからtrue/falseのビットを取得し、tzcntで最初のtrueのインデックスを返す
            let x = rng.random::<f32>();
            let x = std::arch::x86_64::_mm256_set1_ps(x);
            let cmp =
                std::arch::x86_64::_mm256_cmp_ps(self.prefix_sum, x, std::arch::x86_64::_CMP_GE_OQ);
            let flag = std::arch::x86_64::_mm256_movemask_ps(cmp);

            std::arch::x86_64::_tzcnt_u32(flag as u32) as usize
        }
    }

    impl NeighborSelector for SimdSelector {
        fn new(weights: Vec<f64>) -> Self {
            assert!(!weights.is_empty(), "Weights must not be empty");
            assert!(
                weights.len() <= 8,
                "SimdSelector requires weights of length 8 or less"
            );
            assert!(
                weights.iter().all(|&w| w >= 0.0),
                "Weights must be non-negative"
            );

            let mut prefix_sum = [0.0; 8];
            for i in 0..weights.len() {
                prefix_sum[i] = weights[i] as f32;
            }

            for i in 1..prefix_sum.len() {
                prefix_sum[i] += prefix_sum[i - 1];
            }

            let sum = prefix_sum[7];
            assert!(sum > 0.0, "Weights must not all be zero");

            for v in prefix_sum.iter_mut() {
                *v /= sum; // 正規化
            }

            let prefix_sum = unsafe { std::arch::x86_64::_mm256_loadu_ps(prefix_sum.as_ptr()) };
            Self { prefix_sum }
        }

        fn select(&self, rng: &mut AnnealingRng) -> usize {
            unsafe { self.select_simd(rng) }
        }
    }

    /// 焼きなましの統計データ
    #[derive(Debug, Clone)]
    pub struct AnnealingStatistics {
        all_iter: usize,
        valid_iter: usize,
        accepted_count: usize,
        updated_count: usize,
        init_score: f64,
        final_score: f64,
        selected: Vec<u64>,
        accepted: Vec<u64>,
        neighbor_names: &'static [&'static str],
    }

    impl AnnealingStatistics {
        fn new(init_score: f64, neighbor_names: &'static [&'static str]) -> Self {
            Self {
                all_iter: 0,
                valid_iter: 0,
                accepted_count: 0,
                updated_count: 0,
                init_score,
                final_score: init_score,
                selected: vec![0; neighbor_names.len()],
                accepted: vec![0; neighbor_names.len()],
                neighbor_names,
            }
        }

        fn select(&mut self, neighbor_index: usize) {
            self.valid_iter += 1;
            self.selected[neighbor_index] += 1;
        }

        fn accept(&mut self, neighbor_index: usize) {
            self.accepted_count += 1;
            self.accepted[neighbor_index] += 1;
        }
    }

    impl Display for AnnealingStatistics {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            writeln!(f, "===== annealing =====")?;
            writeln!(f, "init score : {}", self.init_score)?;
            writeln!(f, "score      : {}", self.final_score)?;
            writeln!(f, "all iter   : {}", self.all_iter)?;
            writeln!(f, "valid iter : {}", self.valid_iter)?;
            writeln!(f, "accepted   : {}", self.accepted_count)?;
            writeln!(f, "updated    : {}", self.updated_count)?;

            for (name, &selected, &accepted) in
                izip!(self.neighbor_names, &self.selected, &self.accepted)
            {
                let percent = accepted as f64 / selected as f64 * 100.0;

                writeln!(f, "{name:11}: {accepted} / {selected} ({percent:.2}%)")?;
            }

            Ok(())
        }
    }

    /// 焼きなましを実行する関数
    ///
    /// `N` は近傍のセットを表すenumで、`NeighborEnum` トレイトを実装している必要がある。
    /// `I` は焼きなましの進捗を更新する間隔を指定する。例えば `I = 1024` とすると、1024回に1回の頻度で進捗を更新する。
    ///
    /// # Usage
    ///
    /// ```ignore
    /// neighbors! {
    ///     Neighbors, Env, State, [
    ///         TwoOpt => 1.0,
    ///         Swap => 2.0,
    ///         Insert => 0.5,
    ///     ]
    /// }
    ///
    /// let (state, stats) = run_annealing::<Neighbors, SimdSelector, 128>(&input, state, 1e1, 1e-1, Duration::from_millis(1000), 42);
    /// ```
    pub fn run_annealing<N: NeighborDelegator, S: NeighborSelector, const I: usize>(
        env: &N::Env,
        state: N::State,
        start_temp: f64,
        end_temp: f64,
        duration: Duration,
        seed: u128,
    ) -> (N::State, AnnealingStatistics) {
        let threshold_generator = ThresholdGenerator::get_singleton();
        let mut threshold_generator = threshold_generator.borrow_mut();
        let mut context =
            AnnealingContext::new::<N>(env, state, &mut threshold_generator, start_temp, seed);
        context
            .threshold_generator
            .set_pos(context.rng.random_range(0..ThresholdGenerator::LEN));
        let selector = S::new(N::get_neighbor_weights());

        let since = Instant::now();
        let duration_inv = 1.0 / duration.as_secs_f64();

        loop {
            if context.stats.all_iter % I == 0 {
                context.progress = since.elapsed().as_secs_f64() * duration_inv;
                context.temperature =
                    start_temp.powf(1.0 - context.progress) * end_temp.powf(context.progress);

                if context.progress >= 1.0 {
                    break;
                }
            }

            context.stats.all_iter += 1;
            let neighbor_index = selector.select(&mut context.rng);
            N::step(&mut context, neighbor_index);
        }

        context.stats.final_score = context.best_state.score(env).raw_score();

        (context.best_state, context.stats)
    }

    pub struct AnnealingContext<'a, E, S: State<Env = E>> {
        pub env: &'a E,
        pub state: S,
        pub best_state: S,
        pub current_score: S::Score,
        pub best_score: f64,
        pub stats: AnnealingStatistics,
        pub progress: f64,
        pub temperature: f64,
        pub rng: AnnealingRng,
        threshold_generator: &'a mut ThresholdGenerator,
    }

    impl<'a, E, S: State<Env = E>> AnnealingContext<'a, E, S> {
        fn new<N: NeighborDelegator>(
            env: &'a E,
            state: S,
            threshold_generator: &'a mut ThresholdGenerator,
            temperature: f64,
            seed: u128,
        ) -> Self {
            let best_state = state.clone();
            let current_score = state.score(env);
            let best_score = current_score.annealing_score(1.0);
            let stats =
                AnnealingStatistics::new(current_score.raw_score(), N::get_neighbor_names());
            let progress = 0.0;
            let rng = AnnealingRng::new(seed);

            Self {
                env,
                state,
                best_state,
                current_score,
                best_score,
                stats,
                progress,
                temperature,
                rng,
                threshold_generator,
            }
        }

        pub fn step<N: Neighbor<Env = E, State = S>>(
            &mut self,
            mut neighbor: N,
            neighbor_index: usize,
        ) {
            self.stats.select(neighbor_index);
            neighbor.preprocess(self.env, &mut self.state);

            // スコア計算
            let threshold = self.threshold_generator.next(
                self.current_score.annealing_score(self.progress),
                self.temperature,
            );
            let Some(new_score) = neighbor.eval(self.env, &self.state, self.progress, threshold)
            else {
                // 明らかに閾値に届かない場合はreject
                neighbor.rollback(self.env, &mut self.state);
                return;
            };

            if new_score.annealing_score(self.progress) >= threshold {
                self.stats.accept(neighbor_index);
                self.current_score = new_score;
                neighbor.postprocess(self.env, &mut self.state);

                let new_score = self.current_score.annealing_score(1.0);

                if self.best_score < new_score {
                    self.best_score = new_score;
                    self.best_state = self.state.clone();
                    self.stats.updated_count += 1;
                }
            } else {
                neighbor.rollback(self.env, &mut self.state);
            }
        }
    }

    /// 焼きなましにおける評価関数の打ち切り基準となる次の閾値を返す構造体
    ///
    /// 参考: [焼きなまし法での評価関数の打ち切り](https://qiita.com/not522/items/cd20b87157d15850d31c)
    struct ThresholdGenerator {
        iter: usize,
        log_randoms: Vec<f64>,
    }

    impl ThresholdGenerator {
        const LEN: usize = 1 << 16;
        thread_local! {
            static THRESHOLD_GENERATOR: Rc<RefCell<ThresholdGenerator>> = Rc::new(RefCell::new(ThresholdGenerator::new(42)));
        }

        fn new(seed: u128) -> Self {
            let mut rng = Pcg64Mcg::new(seed);
            let log_randoms = (0..Self::LEN)
                .map(|_| rng.random_range(0.0f64..1.0).ln())
                .collect_vec();

            Self {
                iter: 0,
                log_randoms,
            }
        }

        /// 評価関数の打ち切り基準となる次の閾値を返す
        fn next(&mut self, prev_score: f64, temperature: f64) -> f64 {
            let threshold = prev_score + temperature * self.log_randoms[self.iter % Self::LEN];
            self.iter += 1;
            threshold
        }

        fn set_pos(&mut self, pos: usize) {
            self.iter = pos % Self::LEN;
        }

        fn get_singleton() -> Rc<RefCell<Self>> {
            Self::THRESHOLD_GENERATOR.with(|cell| cell.clone())
        }
    }

    #[cfg(test)]
    mod test {
        use crate::{
            annealing::{run_annealing, SimdSelector},
            random::RandExtension,
        };
        use itertools::Itertools;
        use std::time::Duration;

        #[derive(Debug, Clone)]
        struct Input {
            n: usize,
            distances: Vec<Vec<i32>>,
        }

        impl Input {
            fn gen_testcase() -> Self {
                let n = 4;
                let distances = vec![
                    vec![0, 2, 3, 10],
                    vec![2, 0, 1, 3],
                    vec![3, 1, 0, 2],
                    vec![10, 3, 2, 0],
                ];

                Self { n, distances }
            }
        }

        #[derive(Debug, Clone)]
        struct State {
            order: Vec<usize>,
            dist: i32,
        }

        impl State {
            fn new(input: &Input) -> Self {
                let mut order = (0..input.n).collect_vec();
                order.push(0);
                let dist = order
                    .iter()
                    .tuple_windows()
                    .map(|(&prev, &next)| input.distances[prev][next])
                    .sum();

                Self { order, dist }
            }
        }

        impl super::State for State {
            type Env = Input;
            type Score = Dist;

            fn score(&self, _env: &Self::Env) -> Self::Score {
                Dist(self.dist)
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        struct Dist(i32);

        impl super::Score for Dist {
            fn annealing_score(&self, _progress: f64) -> f64 {
                // 大きい方が良いとするため符号を反転
                -self.0 as f64
            }

            fn raw_score(&self) -> f64 {
                self.0 as f64
            }
        }

        struct NoOp;

        impl super::Neighbor for NoOp {
            type Env = Input;
            type State = State;

            fn generate(
                _env: &Self::Env,
                _state: &Self::State,
                _rng: &mut super::AnnealingRng,
                _progress: f64,
            ) -> Option<Self> {
                Some(NoOp)
            }

            fn preprocess(&mut self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn postprocess(self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn rollback(self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }
        }

        struct TwoOpt {
            begin: usize,
            end: usize,
            new_dist: Option<i32>,
        }

        impl TwoOpt {
            fn new(begin: usize, end: usize) -> Self {
                Self {
                    begin,
                    end,
                    new_dist: None,
                }
            }
        }

        impl super::Neighbor for TwoOpt {
            type Env = Input;
            type State = State;

            fn generate(
                _env: &Self::Env,
                state: &Self::State,
                rng: &mut super::AnnealingRng,
                _progress: f64,
            ) -> Option<Self> {
                loop {
                    let (i, j) =
                        rng.fast_gen_range_u16x2(1..state.order.len(), 1..state.order.len());
                    let begin = i.min(j);
                    let end = i.max(j);

                    if begin + 2 <= end {
                        return Some(TwoOpt::new(begin, end));
                    }
                }
            }

            fn preprocess(&mut self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn eval(
                &mut self,
                env: &Self::Env,
                state: &Self::State,
                _progress: f64,
                _threshold: f64,
            ) -> Option<<Self::State as super::State>::Score> {
                let v0 = state.order[self.begin - 1];
                let v1 = state.order[self.begin];
                let v2 = state.order[self.end - 1];
                let v3 = state.order[self.end];

                let d00 = env.distances[v0][v1];
                let d01 = env.distances[v0][v2];
                let d10 = env.distances[v2][v3];
                let d11 = env.distances[v1][v3];

                let new_dist = state.dist - d00 - d10 + d01 + d11;
                self.new_dist = Some(new_dist);

                Some(Dist(new_dist))
            }

            fn postprocess(self, _env: &Self::Env, state: &mut Self::State) {
                state.order[self.begin..self.end].reverse();
                state.dist = self
                    .new_dist
                    .expect("postprocess()を呼ぶ前にeval()を呼んでください。");
            }

            fn rollback(self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }
        }

        // 近傍セットを表すenumの生成
        crate::neighbors! {
            Neighbors, Input, State, [
                NoOp => 1.0,
                TwoOpt => 2.0,
            ]
        }

        #[test]
        fn annealing_tsp_test() {
            let input = Input::gen_testcase();
            let state = State::new(&input);

            let (state, diagnostics) = run_annealing::<Neighbors, SimdSelector, 1024>(
                &input,
                state,
                1e1,
                1e-1,
                Duration::from_millis(100),
                42,
            );

            eprintln!("{diagnostics}");

            eprintln!("score: {}", state.dist);
            eprintln!("state.dist: {:?}", state.order);
            assert_eq!(state.dist, 10);
            assert!(state.order == vec![0, 1, 3, 2, 0] || state.order == vec![0, 2, 3, 1, 0]);
        }
    }
}
pub mod diagnostics {
    use itertools::Itertools;
    use rustc_hash::FxHashMap;
    use std::{
        borrow::Cow,
        cell::RefCell,
        fmt::Display,
        io::{self, IsTerminal as _},
        rc::Rc,
        time::{Duration, Instant},
    };

    /// 実行時間を計測・集計する構造体
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::diagnostics::Perf;
    /// use cp_lib_rs::perf;
    ///
    /// // 計測グループを作成する
    /// // dropされるときに計測結果を出力する
    /// let mut perf = Perf::new("Group");
    /// let mut _sum = 0u64;
    ///
    /// for i in 0..100000 {
    ///     // start-stop間の処理時間を計測する
    ///     let sw = perf.start("sum");
    ///     _sum += i;
    ///     sw.stop();
    ///
    ///     // 名前は&'strでもStringでもOK
    ///     let sw = perf.start(format!("no-op"));
    ///     sw.stop();
    /// }
    ///
    /// let mut _sum_sqrt = 0f64;
    ///
    /// for i in 0..100000 {
    ///     // 明示的にstop()を呼ばなくても、スコープを抜ける際に自動でstopする
    ///     let _sw = perf.start("sum sqrt");
    ///     _sum_sqrt += (i as f64).sqrt();
    /// }
    ///
    /// let mut _sum_sq = 0u64;
    ///
    /// for i in 0..100000 {
    ///     // perfのインスタンス化が面倒な場合はシングルトンを使う
    ///     let sw = Perf::start_singleton("sum sq");
    ///     _sum_sq += i * i;
    ///     sw.stop();
    /// }
    ///
    /// let mut _sum_pow = 0f64;
    ///
    /// for i in 0..100000 {
    ///     // マクロでショートハンド化もできる
    ///     _sum_pow += perf!("sum pow", (i as f64).powf(3.1415926));
    /// }
    /// ```
    pub struct Perf {
        name: Option<Cow<'static, str>>,
        measures: FxHashMap<Cow<'static, str>, Measure>,
    }

    impl Perf {
        thread_local!(static SINGLETON: Rc<RefCell<Perf>> = Rc::new(RefCell::new(Perf::new("Singleton"))));

        /// 新しい Perf インスタンスを作成する
        pub fn new(name: impl Into<Cow<'static, str>>) -> Self {
            Self {
                name: Some(name.into()),
                measures: FxHashMap::default(),
            }
        }

        /// 匿名の Perf インスタンスを作成する
        pub fn new_anonymous() -> Self {
            Self {
                name: None,
                measures: FxHashMap::default(),
            }
        }

        /// 計測を開始する
        pub fn start<'a>(
            &'a mut self,
            name: impl Into<Cow<'static, str>>,
        ) -> StopWatch<&'a mut Perf> {
            let name = name.into();

            StopWatch {
                start: Instant::now(),
                perf: self,
                name,
            }
        }

        /// 計測を開始する（シングルトン）
        pub fn start_singleton(name: impl Into<Cow<'static, str>>) -> StopWatch<Rc<RefCell<Perf>>> {
            Self::SINGLETON.with(|perf| StopWatch {
                start: Instant::now(),
                perf: perf.clone(),
                name: name.into(),
            })
        }
    }

    impl Drop for Perf {
        fn drop(&mut self) {
            if self.measures.is_empty() {
                return;
            }

            // ターミナルかどうかで色を付けるかどうか分岐
            let is_tty = io::stderr().is_terminal();
            let name = self.name.as_deref().unwrap_or("Anonymous");

            if is_tty {
                // コンソール → 色付き
                eprintln!("\x1b[35m[{}] Performance measures\x1b[0m", name);
            } else {
                // リダイレクト → 色なし
                eprintln!("[{}] Performance measures", name);
            }

            for (name, measure) in self
                .measures
                .iter()
                .sorted_unstable_by(|(a, _), (b, _)| a.as_ref().cmp(b.as_ref()))
            {
                eprintln!("{}: {}", name, measure);
            }
        }
    }

    #[macro_export]
    macro_rules! perf {
        ($name: expr, $body: expr) => {{
            let sw = Perf::start_singleton($name);
            let result = $body;
            sw.stop();
            result
        }};
    }

    pub struct StopWatch<M: WithMut<Perf>> {
        start: Instant,
        perf: M,
        name: Cow<'static, str>,
    }

    impl<M: WithMut<Perf>> StopWatch<M> {
        pub fn stop(self) {
            std::mem::drop(self);
        }
    }

    impl<M: WithMut<Perf>> Drop for StopWatch<M> {
        fn drop(&mut self) {
            let duration = self.start.elapsed();
            let name = std::mem::take(&mut self.name);

            self.perf.with_mut(|perf| {
                perf.measures
                    .entry(name)
                    .or_insert_with(Measure::new)
                    .add_measure(&duration)
            })
        }
    }

    #[derive(Debug, Clone, Copy, Default)]
    struct Measure {
        sum: f64,
        sum_sq: f64,
        cnt: usize,
    }

    impl Measure {
        fn new() -> Self {
            Self::default()
        }

        fn add_measure(&mut self, duration: &Duration) {
            let sec = duration.as_secs_f64();
            self.sum += sec;
            self.sum_sq += sec * sec;
            self.cnt += 1;
        }

        fn mean(&self) -> Duration {
            assert_ne!(self.cnt, 0);
            Duration::from_secs_f64(self.sum / self.cnt as f64)
        }

        fn std_dev(&self) -> Duration {
            assert_ne!(self.cnt, 0);

            let mean = self.mean().as_secs_f64();
            let variance = (self.sum_sq / self.cnt as f64) - (mean * mean);
            Duration::from_secs_f64(variance.sqrt())
        }
    }

    impl Display for Measure {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(
                f,
                "{:?} ± {:?} ({} samples)",
                self.mean(),
                self.std_dev(),
                self.cnt
            )
        }
    }

    /// &mut T と Rc<RefCell<T>> を統一的に扱って処理を行うためのトレイト
    pub trait WithMut<T> {
        fn with_mut<R>(&mut self, f: impl FnOnce(&mut T) -> R) -> R;
    }

    impl<T> WithMut<T> for &mut T {
        fn with_mut<R>(&mut self, f: impl FnOnce(&mut T) -> R) -> R {
            f(*self)
        }
    }

    impl<T> WithMut<T> for Rc<RefCell<T>> {
        fn with_mut<R>(&mut self, f: impl FnOnce(&mut T) -> R) -> R {
            let mut guard = self.borrow_mut();
            f(&mut *guard)
        }
    }
}
pub mod grid {
    use std::{
        fmt::Display,
        ops::{Add, AddAssign, Index, IndexMut},
    };

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Default)]
    pub struct Coord {
        row: u8,
        col: u8,
    }

    impl Coord {
        pub const fn new(row: usize, col: usize) -> Self {
            Self {
                row: row as u8,
                col: col as u8,
            }
        }

        pub const fn row(&self) -> usize {
            self.row as usize
        }

        pub const fn col(&self) -> usize {
            self.col as usize
        }

        pub fn in_map(&self, size: usize) -> bool {
            self.row < size as u8 && self.col < size as u8
        }

        pub const fn to_index(&self, size: usize) -> CoordIndex {
            CoordIndex(self.row as usize * size + self.col as usize)
        }

        pub const fn dist(&self, other: &Self) -> usize {
            Self::dist_1d(self.row, other.row) + Self::dist_1d(self.col, other.col)
        }

        const fn dist_1d(x0: u8, x1: u8) -> usize {
            x0.abs_diff(x1) as usize
        }
    }

    impl Display for Coord {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({}, {})", self.row, self.col)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Default)]
    pub struct CoordIndex(pub usize);

    impl CoordIndex {
        pub const fn to_coord(&self, size: usize) -> Coord {
            Coord::new(self.0 / size, self.0 % size)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Default)]
    pub struct CoordDiff {
        dr: i8,
        dc: i8,
    }

    impl CoordDiff {
        pub const fn new(dr: i32, dc: i32) -> Self {
            Self {
                dr: dr as i8,
                dc: dc as i8,
            }
        }

        pub const fn invert(&self) -> Self {
            Self {
                dr: -self.dr,
                dc: -self.dc,
            }
        }

        pub const fn dr(&self) -> i32 {
            self.dr as i32
        }

        pub const fn dc(&self) -> i32 {
            self.dc as i32
        }
    }

    impl Display for CoordDiff {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({}, {})", self.dr, self.dc)
        }
    }

    impl Add<CoordDiff> for Coord {
        type Output = Coord;

        fn add(self, rhs: CoordDiff) -> Self::Output {
            Coord {
                row: self.row.wrapping_add(rhs.dr as u8),
                col: self.col.wrapping_add(rhs.dc as u8),
            }
        }
    }

    impl AddAssign<CoordDiff> for Coord {
        fn add_assign(&mut self, rhs: CoordDiff) {
            self.row = self.row.wrapping_add(rhs.dr as u8);
            self.col = self.col.wrapping_add(rhs.dc as u8);
        }
    }

    pub const ADJACENTS: [CoordDiff; 4] = [
        CoordDiff::new(-1, 0),
        CoordDiff::new(0, 1),
        CoordDiff::new(1, 0),
        CoordDiff::new(0, -1),
    ];

    pub const U: usize = 0;
    pub const R: usize = 1;
    pub const D: usize = 2;
    pub const L: usize = 3;
    pub const DIRECTIONS: [char; 4] = ['U', 'R', 'D', 'L'];

    #[derive(Debug, Clone)]
    pub struct Map2d<T> {
        size: usize,
        map: Vec<T>,
    }

    impl<T> Map2d<T> {
        pub fn new(map: Vec<T>, size: usize) -> Self {
            debug_assert!(size * size == map.len());
            Self { size, map }
        }

        pub fn from_fn(mut f: impl FnMut(Coord) -> T, size: usize) -> Self {
            let mut map = Vec::with_capacity(size * size);

            for row in 0..size {
                for col in 0..size {
                    map.push(f(Coord::new(row, col)));
                }
            }

            Self { size, map }
        }

        pub fn iter(&self) -> impl Iterator<Item = &T> {
            self.map.iter()
        }
    }

    impl<T: Default + Clone> Map2d<T> {
        pub fn with_default(size: usize) -> Self {
            let map = vec![T::default(); size * size];
            Self { size, map }
        }
    }

    impl<T> Index<Coord> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: Coord) -> &Self::Output {
            &self.map[coordinate.to_index(self.size).0]
        }
    }

    impl<T> IndexMut<Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(self.size).0]
        }
    }

    impl<T> Index<&Coord> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &Coord) -> &Self::Output {
            &self.map[coordinate.to_index(self.size).0]
        }
    }

    impl<T> IndexMut<&Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: &Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(self.size).0]
        }
    }

    impl<T> Index<usize> for Map2d<T> {
        type Output = [T];

        #[inline]
        fn index(&self, row: usize) -> &Self::Output {
            let begin = row * self.size;
            let end = begin + self.size;
            &self.map[begin..end]
        }
    }

    impl<T> IndexMut<usize> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, row: usize) -> &mut Self::Output {
            let begin = row * self.size;
            let end = begin + self.size;
            &mut self.map[begin..end]
        }
    }

    impl<T> Index<CoordIndex> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, index: CoordIndex) -> &Self::Output {
            &self.map[index.0]
        }
    }

    impl<T> IndexMut<CoordIndex> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, index: CoordIndex) -> &mut Self::Output {
            &mut self.map[index.0]
        }
    }

    #[derive(Debug, Clone)]
    pub struct ConstMap2d<T, const N: usize> {
        map: Vec<T>,
    }

    impl<T, const N: usize> ConstMap2d<T, N> {
        pub fn new(map: Vec<T>) -> Self {
            assert_eq!(map.len(), N * N);
            Self { map }
        }

        pub fn from_fn(mut f: impl FnMut(Coord) -> T) -> Self {
            let mut map = Vec::with_capacity(N * N);

            for row in 0..N {
                for col in 0..N {
                    map.push(f(Coord::new(row, col)));
                }
            }

            Self { map }
        }
    }

    impl<T: Default + Clone, const N: usize> ConstMap2d<T, N> {
        pub fn with_default() -> Self {
            let map = vec![T::default(); N * N];
            Self { map }
        }
    }

    impl<T, const N: usize> Index<Coord> for ConstMap2d<T, N> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: Coord) -> &Self::Output {
            &self.map[coordinate.to_index(N).0]
        }
    }

    impl<T, const N: usize> IndexMut<Coord> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(N).0]
        }
    }

    impl<T, const N: usize> Index<&Coord> for ConstMap2d<T, N> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &Coord) -> &Self::Output {
            &self.map[coordinate.to_index(N).0]
        }
    }

    impl<T, const N: usize> IndexMut<&Coord> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, coordinate: &Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(N).0]
        }
    }

    impl<T, const N: usize> Index<usize> for ConstMap2d<T, N> {
        type Output = [T];

        #[inline]
        fn index(&self, row: usize) -> &Self::Output {
            let begin = row * N;
            let end = begin + N;
            &self.map[begin..end]
        }
    }

    impl<T, const N: usize> IndexMut<usize> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, row: usize) -> &mut Self::Output {
            let begin = row * N;
            let end = begin + N;
            &mut self.map[begin..end]
        }
    }

    impl<T, const N: usize> Index<CoordIndex> for ConstMap2d<T, N> {
        type Output = T;

        #[inline]
        fn index(&self, index: CoordIndex) -> &Self::Output {
            &self.map[index.0]
        }
    }

    impl<T, const N: usize> IndexMut<CoordIndex> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, index: CoordIndex) -> &mut Self::Output {
            &mut self.map[index.0]
        }
    }

    #[cfg(test)]
    mod test {
        use super::{ConstMap2d, Coord, CoordDiff, Map2d};

        #[test]
        fn coord_add() {
            let c = Coord::new(2, 4);
            let d = CoordDiff::new(-3, 5);
            let actual = c + d;

            let expected = Coord::new(!0, 9);
            assert_eq!(expected, actual);
        }

        #[test]
        fn coord_add_assign() {
            let mut c = Coord::new(2, 4);
            let d = CoordDiff::new(-3, 5);
            c += d;

            let expected = Coord::new(!0, 9);
            assert_eq!(expected, c);
        }

        #[test]
        fn map_new() {
            let map = Map2d::new(vec![0, 1, 2, 3], 2);
            let actual = map[Coord::new(1, 0)];
            let expected = 2;
            assert_eq!(expected, actual);
        }

        #[test]
        fn map_default() {
            let map = Map2d::with_default(2);
            let actual = map[Coord::new(1, 0)];
            let expected = 0;
            assert_eq!(expected, actual);
        }

        #[test]
        fn const_map_new() {
            let map = ConstMap2d::<_, 2>::new(vec![0, 1, 2, 3]);
            let actual = map[Coord::new(1, 0)];
            let expected = 2;
            assert_eq!(expected, actual);
        }

        #[test]
        fn const_map_default() {
            let map = ConstMap2d::<_, 2>::with_default();
            let actual = map[Coord::new(1, 0)];
            let expected = 0;
            assert_eq!(expected, actual);
        }
    }
}
pub mod problem {
    use proconio::input;
    use rand::seq::SliceRandom;
    use rand_pcg::Pcg64Mcg;
    use std::fmt;

    use crate::diagnostics::Perf;

    #[derive(Debug, Clone)]
    pub struct Input {
        pub graph: Vec<Vec<usize>>,
        pub path_graph: Vec<Vec<Path>>,
        pub nearest_path: Vec<Vec<Vec<Option<Vec<usize>>>>>,
    }

    impl Input {
        /// 固定パラメータ: 頂点数。
        pub const VERTEX_COUNT: usize = 100;
        /// 固定パラメータ: アイスクリームショップ数。
        pub const SHOP_COUNT: usize = 10;
        pub const TREE_COUNT: usize = Self::VERTEX_COUNT - Self::SHOP_COUNT;
        /// 固定パラメータ: 行動の最大回数。
        pub const MAX_STEPS: usize = 10000;
        pub const MAX_PATH_LEN: usize = 14;

        pub fn read() -> Self {
            input! {
                n: usize,
                m: usize,
                k: usize,
                t: usize,
            }

            assert_eq!(n, Self::VERTEX_COUNT);
            assert_eq!(k, Self::SHOP_COUNT);
            assert_eq!(t, Self::MAX_STEPS);

            let mut graph = vec![vec![]; n];

            for _ in 0..m {
                input! {
                    u: usize,
                    v: usize,
                }

                graph[u].push(v);
                graph[v].push(u);
            }

            let perf = Perf::start_singleton("Input::gen_path_graph");
            let path_graph = Self::gen_path_graph(&graph);
            perf.stop();

            let mut nearest_path = vec![];

            for s in 0..Self::SHOP_COUNT {
                let mut paths_from_s = vec![];

                for d in 0..Self::SHOP_COUNT {
                    let mut paths_to_d = vec![];

                    for prohibited in 0..Self::VERTEX_COUNT {
                        let path = Self::bfs(&graph, s, d, prohibited);
                        paths_to_d.push(path);
                    }

                    paths_from_s.push(paths_to_d);
                }

                nearest_path.push(paths_from_s);
            }

            /*

            for v in 0..Input::SHOP_COUNT {
                for p in path_graph[v].iter() {
                    eprintln!(
                        "{} -> {} via {:?} (len={})",
                        v, p.destination, p.trees, p.len
                    );
                }
            }

            for v in 0..Input::SHOP_COUNT {
                for u in 0..Input::SHOP_COUNT {
                    eprintln!(
                        "{} -> {}: {} paths",
                        v,
                        u,
                        path_graph[v].iter().filter(|p| p.destination == u).count()
                    );
                }
            }

             */

            Self {
                graph,
                path_graph,
                nearest_path,
            }
        }

        fn gen_path_graph(graph: &[Vec<usize>]) -> Vec<Vec<Path>> {
            let mut result = vec![];
            let mut rng = Pcg64Mcg::new(42);

            for s in 0..Self::SHOP_COUNT {
                let mut path_list = vec![vec![]; Self::SHOP_COUNT];
                let mut path = vec![];
                Self::path_dfs(graph, s, s, &mut path, &mut path_list);

                // 短い順にPATH_LIMIT個までに制限
                const PATH_LIMIT: usize = 50;
                let mut paths = vec![];

                for dest in 0..Self::SHOP_COUNT {
                    path_list[dest].shuffle(&mut rng);
                    path_list[dest].sort_unstable_by_key(|p| p.len);
                    path_list[dest].truncate(PATH_LIMIT);
                    paths.extend(path_list[dest].clone());
                }

                paths.sort_unstable_by_key(|p| p.len);
                result.push(paths);
            }

            result
        }

        fn path_dfs(
            graph: &[Vec<usize>],
            prev: usize,
            v: usize,
            path: &mut Vec<usize>,
            path_list: &mut Vec<Vec<Path>>,
        ) {
            for &next in &graph[v] {
                if next == prev {
                    continue;
                }

                if next < Input::SHOP_COUNT {
                    let p = Path::new(next, path.clone());
                    path_list[next].push(p);
                } else if path.len() < Input::MAX_PATH_LEN {
                    path.push(next);
                    Self::path_dfs(graph, v, next, path, path_list);
                    path.pop();
                }
            }
        }

        fn bfs(
            graph: &[Vec<usize>],
            start: usize,
            goal: usize,
            prohibited: usize,
        ) -> Option<Vec<usize>> {
            let mut queue = std::collections::VecDeque::new();
            let mut visited = vec![false; graph.len()];
            let mut parent = vec![!0; graph.len()];

            visited[start] = true;

            for &v in graph[start].iter() {
                if v == prohibited {
                    continue;
                }

                if v == goal {
                    return Some(vec![goal]);
                }

                visited[v] = true;
                parent[v] = start;
                queue.push_back(v);
            }

            while let Some(v) = queue.pop_front() {
                if v == goal {
                    let mut path = vec![];
                    let mut cur = v;

                    while cur != start {
                        path.push(cur);
                        cur = parent[cur];
                    }

                    path.reverse();
                    return Some(path);
                }

                for &next in &graph[v] {
                    if !visited[next] {
                        visited[next] = true;
                        parent[next] = v;
                        queue.push_back(next);
                    }
                }
            }

            None
        }
    }

    #[derive(Debug, Clone)]
    pub struct Path {
        pub destination: usize,
        pub trees: Vec<usize>,
        pub len: usize,
        pub turns: usize,
        pub strawberries: Vec<IceCream>,
    }

    impl Path {
        pub fn new(destination: usize, trees: Vec<usize>) -> Self {
            let len = trees.len();
            let turns = len + 1;
            let mut strawberries = vec![];

            for i in 0..len {
                let v = trees[i];
                let mut ice_cream = IceCream::new(i as u16, 0);

                for j in i..len {
                    ice_cream = ice_cream.push(v == trees[j]);
                }

                strawberries.push(ice_cream);
            }

            Self {
                destination,
                trees,
                len,
                turns,
                strawberries,
            }
        }

        pub fn make_ice_cream(&self, strawberries: &[bool]) -> IceCream {
            let mut ice_cream = IceCream::new(0, 0);

            for &v in &self.trees {
                ice_cream = ice_cream.push(strawberries[v]);
            }

            ice_cream
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default, Hash)]
    pub struct IceCream {
        pub size: u16,
        pub stack: u16,
    }

    impl IceCream {
        fn new(size: u16, stack: u16) -> Self {
            Self { size, stack }
        }

        pub fn push(&self, flavor: bool) -> Self {
            let stack = self.stack | ((flavor as u16) << self.size);

            Self {
                size: self.size + 1,
                stack,
            }
        }
    }

    impl std::ops::BitOr for IceCream {
        type Output = Self;

        fn bitor(self, rhs: Self) -> Self::Output {
            assert_eq!(self.size, rhs.size);

            Self {
                size: self.size,
                stack: self.stack | rhs.stack,
            }
        }
    }

    impl std::ops::BitOrAssign for IceCream {
        fn bitor_assign(&mut self, rhs: Self) {
            assert_eq!(self.size, rhs.size);
            self.stack |= rhs.stack;
        }
    }

    #[derive(Debug, Clone)]
    pub struct Output {
        /// 出力する行動列。
        pub actions: Vec<Action>,
    }

    impl Output {
        pub fn new(actions: Vec<Action>) -> Self {
            Self { actions }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum Action {
        /// 操作1: 指定した頂点へ移動する。
        Move(usize),
        /// 操作2: 現在位置の木をいちご味に変える。
        MakeStrawberry,
    }

    impl fmt::Display for Output {
        fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
            for (i, action) in self.actions.iter().enumerate() {
                if i > 0 {
                    writeln!(f)?;
                }
                match action {
                    Action::Move(v) => write!(f, "{}", v)?,
                    Action::MakeStrawberry => write!(f, "-1")?,
                }
            }
            Ok(())
        }
    }
}
pub mod random {
    use num::{FromPrimitive, PrimInt};
    use rand::Rng;
    use std::ops::{Range, RangeInclusive};

    /// 高速な乱数生成の拡張トレイト
    pub trait RandExtension {
        fn fast_gen_range_u64x1<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
            &mut self,
            range: R,
        ) -> T;

        fn fast_gen_range_u32x1<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
            &mut self,
            range: R,
        ) -> T;

        fn fast_gen_range_u32x2<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
        ) -> (T0, T1);

        fn fast_gen_range_u16x1<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
            &mut self,
            range: R,
        ) -> T;

        fn fast_gen_range_u16x2<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
        ) -> (T0, T1);

        fn fast_gen_range_u16x3<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            T2: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
            R2: BoundedRange<T2>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
            range2: R2,
        ) -> (T0, T1, T2);

        fn fast_gen_range_u16x4<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            T2: PrimInt + FromPrimitive,
            T3: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
            R2: BoundedRange<T2>,
            R3: BoundedRange<T3>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
            range2: R2,
            range3: R3,
        ) -> (T0, T1, T2, T3);
    }

    impl<G: Rng> RandExtension for G {
        fn fast_gen_range_u64x1<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
            &mut self,
            range: R,
        ) -> T {
            let rand_value = self.next_u64();
            gen_range_u64(range, rand_value)
        }

        fn fast_gen_range_u32x1<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
            &mut self,
            range: R,
        ) -> T {
            let rand_value = self.next_u64();
            gen_range_u32(range, rand_value as u32)
        }

        fn fast_gen_range_u32x2<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
        ) -> (T0, T1) {
            let rand_value = self.next_u64();
            let v0 = gen_range_u32(range0, rand_value as u32);
            let v1 = gen_range_u32(range1, (rand_value >> 32) as u32);
            (v0, v1)
        }

        fn fast_gen_range_u16x1<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
            &mut self,
            range: R,
        ) -> T {
            let rand_value = self.next_u64();
            gen_range_u16(range, rand_value as u16)
        }

        fn fast_gen_range_u16x2<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
        ) -> (T0, T1) {
            let rand_value = self.next_u64();
            let v0 = gen_range_u16(range0, rand_value as u16);
            let v1 = gen_range_u16(range1, (rand_value >> 16) as u16);
            (v0, v1)
        }

        fn fast_gen_range_u16x3<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            T2: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
            R2: BoundedRange<T2>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
            range2: R2,
        ) -> (T0, T1, T2) {
            let rand_value = self.next_u64();
            let v0 = gen_range_u16(range0, rand_value as u16);
            let v1 = gen_range_u16(range1, (rand_value >> 16) as u16);
            let v2 = gen_range_u16(range2, (rand_value >> 32) as u16);
            (v0, v1, v2)
        }

        fn fast_gen_range_u16x4<
            T0: PrimInt + FromPrimitive,
            T1: PrimInt + FromPrimitive,
            T2: PrimInt + FromPrimitive,
            T3: PrimInt + FromPrimitive,
            R0: BoundedRange<T0>,
            R1: BoundedRange<T1>,
            R2: BoundedRange<T2>,
            R3: BoundedRange<T3>,
        >(
            &mut self,
            range0: R0,
            range1: R1,
            range2: R2,
            range3: R3,
        ) -> (T0, T1, T2, T3) {
            let rand_value = self.next_u64();
            let v0 = gen_range_u16(range0, rand_value as u16);
            let v1 = gen_range_u16(range1, (rand_value >> 16) as u16);
            let v2 = gen_range_u16(range2, (rand_value >> 32) as u16);
            let v3 = gen_range_u16(range3, (rand_value >> 48) as u16);
            (v0, v1, v2, v3)
        }
    }

    fn gen_range_u64<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
        range: R,
        rand_value: u64,
    ) -> T {
        assert!(!range.is_empty(), "cannot sample empty range");
        let width = range.width().to_u64().expect("width must fit in u64");
        let start = range.start();
        let value = (((width as u128) * (rand_value as u128)) >> 64) as u64;
        T::from_u64(value).expect("value must fit in T") + start
    }

    fn gen_range_u32<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
        range: R,
        rand_value: u32,
    ) -> T {
        assert!(!range.is_empty(), "cannot sample empty range");
        let width = range.width().to_u32().expect("width must fit in u32");
        let start = range.start();
        let value = (((width as u64) * (rand_value as u64)) >> 32) as u32;
        T::from_u32(value).expect("value must fit in T") + start
    }

    fn gen_range_u16<T: PrimInt + FromPrimitive, R: BoundedRange<T>>(
        range: R,
        rand_value: u16,
    ) -> T {
        assert!(!range.is_empty(), "cannot sample empty range");
        let width = range.width().to_u16().expect("width must fit in u16");
        let start = range.start();
        let value = (((width as u32) * (rand_value as u32)) >> 16) as u16;
        T::from_u16(value).expect("value must fit in T") + start
    }

    pub trait BoundedRange<T> {
        fn start(&self) -> T;
        fn width(&self) -> T;
        fn is_empty(&self) -> bool;
    }

    impl<T: PrimInt> BoundedRange<T> for Range<T> {
        fn start(&self) -> T {
            self.start
        }

        fn width(&self) -> T {
            self.end - self.start
        }

        fn is_empty(&self) -> bool {
            self.start >= self.end
        }
    }

    impl<T: PrimInt> BoundedRange<T> for RangeInclusive<T> {
        fn start(&self) -> T {
            *self.start()
        }

        fn width(&self) -> T {
            *self.end() - *self.start() + T::one()
        }

        fn is_empty(&self) -> bool {
            self.start() > self.end()
        }
    }

    #[cfg(test)]
    mod tests {
        use super::*;
        use rand::rngs::StdRng;
        use rand::SeedableRng;
        use std::collections::HashSet;

        fn check_range<R: BoundedRange<usize> + Clone, F: Fn(&mut StdRng, R) -> usize>(
            range: R,
            generator: F,
        ) {
            let mut rng = StdRng::seed_from_u64(42);
            let mut values = HashSet::new();
            let start = range.start();
            let width = range.width();
            let end = start + width;

            for _ in 0..10000 {
                let v = generator(&mut rng, range.clone());
                assert!(start <= v && v < end, "value {v:?} out of range");
                values.insert(v);
            }

            for v in start..end {
                assert!(values.contains(&v), "value {v:?} not generated");
            }
        }

        #[test]
        fn test_fast_gen_range_u64x1() {
            check_range(0usize..10usize, |rng, r| rng.fast_gen_range_u64x1(r));
        }

        #[test]
        #[should_panic(expected = "cannot sample empty range")]
        fn test_fast_gen_range_u64x1_empty_range() {
            let mut rng = StdRng::seed_from_u64(42);
            let _ = rng.fast_gen_range_u64x1(5usize..5usize);
        }

        #[test]
        fn test_fast_gen_range_u32x1() {
            check_range(0usize..10usize, |rng, r| rng.fast_gen_range_u32x1(r));
        }

        #[test]
        fn test_fast_gen_range_u32x2() {
            let mut rng = StdRng::seed_from_u64(123);
            let mut set0 = HashSet::new();
            let mut set1 = HashSet::new();

            for _ in 0..10000 {
                let (v0, v1) = rng.fast_gen_range_u32x2(0u32..5u32, 10u32..=15u32);
                assert!((0..5).contains(&v0));
                assert!((10..=15).contains(&v1));
                set0.insert(v0);
                set1.insert(v1);
            }

            for v in 0u32..5u32 {
                assert!(set0.contains(&v));
            }

            for v in 10u32..=15u32 {
                assert!(set1.contains(&v));
            }
        }

        #[test]
        fn test_fast_gen_range_u16x1() {
            check_range(0usize..10usize, |rng, r| rng.fast_gen_range_u16x1(r));
        }

        #[test]
        fn test_fast_gen_range_u16x2() {
            let mut rng = StdRng::seed_from_u64(456);
            let mut set0 = HashSet::new();
            let mut set1 = HashSet::new();

            for _ in 0..10000 {
                let (v0, v1) = rng.fast_gen_range_u16x2(0u16..5u16, 10u16..15u16);
                assert!((0..5).contains(&v0));
                assert!((10..15).contains(&v1));
                set0.insert(v0);
                set1.insert(v1);
            }

            for v in 0u16..5u16 {
                assert!(set0.contains(&v));
            }

            for v in 10u16..15u16 {
                assert!(set1.contains(&v));
            }
        }

        #[test]
        fn test_fast_gen_range_u16x3() {
            let mut rng = StdRng::seed_from_u64(789);
            let mut set0 = HashSet::new();
            let mut set1 = HashSet::new();
            let mut set2 = HashSet::new();

            for _ in 0..10000 {
                let (v0, v1, v2) =
                    rng.fast_gen_range_u16x3(0u16..3u16, 10u16..13u16, 20u16..=23u16);
                assert!((0..3).contains(&v0));
                assert!((10..13).contains(&v1));
                assert!((20..=23).contains(&v2));
                set0.insert(v0);
                set1.insert(v1);
                set2.insert(v2);
            }

            for v in 0u16..3u16 {
                assert!(set0.contains(&v));
            }

            for v in 10u16..13u16 {
                assert!(set1.contains(&v));
            }

            for v in 20u16..=23u16 {
                assert!(set2.contains(&v));
            }
        }

        #[test]
        fn test_fast_gen_range_u16x4() {
            let mut rng = StdRng::seed_from_u64(321);
            let mut set0 = HashSet::new();
            let mut set1 = HashSet::new();
            let mut set2 = HashSet::new();
            let mut set3 = HashSet::new();

            for _ in 0..10000 {
                let (v0, v1, v2, v3) =
                    rng.fast_gen_range_u16x4(0u16..2u16, 10u16..12u16, 20u16..=22u16, 30u16..32u16);
                assert!((0..2).contains(&v0));
                assert!((10..12).contains(&v1));
                assert!((20..=22).contains(&v2));
                assert!((30..32).contains(&v3));
                set0.insert(v0);
                set1.insert(v1);
                set2.insert(v2);
                set3.insert(v3);
            }

            for v in 0u16..2u16 {
                assert!(set0.contains(&v));
            }

            for v in 10u16..12u16 {
                assert!(set1.contains(&v));
            }

            for v in 20u16..=22u16 {
                assert!(set2.contains(&v));
            }

            for v in 30u16..32u16 {
                assert!(set3.contains(&v));
            }
        }
    }
}
pub mod solver {
    pub mod annealing {
        use std::time::Duration;

        use rand::{Rng, SeedableRng};
        use rand_pcg::Pcg64Mcg;
        use rustc_hash::FxHashSet;

        use crate::{
            annealing::{self, SimdSelector, SingleScore},
            neighbors,
            problem::{Action, IceCream, Input, Output},
            util::ChangeMinMax,
        };

        pub fn solve(input: &Input) -> Output {
            let env = Env::new(input.clone());
            let mut best_state = State::generate(42);
            let mut best_score = best_state.calc_score(&env);

            for seed in 0..10 {
                let state = State::generate(seed);

                if best_score.change_max(state.calc_score(&env)) {
                    best_state = state;
                }
            }

            let (state, stats) = crate::annealing::run_annealing::<Neighbors, SimdSelector, 1>(
                &env,
                best_state,
                1e0,
                1e-1,
                Duration::from_millis(1900),
                42,
            );

            eprintln!("{}", stats);
            state.simulate(&env)
        }

        neighbors! {
            Neighbors,
            Env,
            State,
            [
                ChangeNeigh => 1.0,
            ]
        }

        struct Env {
            input: Input,
        }

        impl Env {
            fn new(input: Input) -> Self {
                Self { input }
            }
        }

        #[derive(Clone)]
        struct State {
            times: Vec<usize>,
        }

        impl State {
            fn generate(seed: u64) -> Self {
                let mut rng = Pcg64Mcg::seed_from_u64(seed);

                let mut times = vec![0; Input::TREE_COUNT];

                for i in 0..Input::TREE_COUNT {
                    let t = rng.random_range(0..=Input::MAX_STEPS);
                    times[i] = t;
                }

                Self { times }
            }

            fn calc_score(&self, env: &Env) -> usize {
                let mut pos = 0;
                let mut strawberries = [false; Input::VERTEX_COUNT];
                let mut actual_strawberries = [false; Input::VERTEX_COUNT];

                let mut set = vec![FxHashSet::default(); Input::SHOP_COUNT];
                let mut rng = Pcg64Mcg::new(42);
                let mut prev_pos = !0;

                let mut events = self
                    .times
                    .iter()
                    .enumerate()
                    .map(|(i, &t)| (t, i))
                    .collect::<Vec<_>>();
                events.sort_unstable();
                let mut events_i = 0;
                let input = &env.input;
                let mut turns = 0;

                loop {
                    while events_i < events.len() && events[events_i].0 <= turns {
                        strawberries[events[events_i].1 + Input::SHOP_COUNT] = true;
                        events_i += 1;
                    }

                    let mut best_path_i = None;
                    let mut best_path_turns = usize::MAX;

                    for (path_i, path) in input.path_graph[pos].iter().enumerate() {
                        if path.trees.get(0).unwrap_or(&path.destination) == &prev_pos {
                            continue;
                        }

                        let ice_cream = path.make_ice_cream(&strawberries);

                        if !set[path.destination].contains(&ice_cream)
                            && best_path_turns.change_min(path.turns)
                        {
                            best_path_i = Some(path_i);
                            break;
                        }
                    }

                    if let Some(path_i) = best_path_i {
                        let path = &input.path_graph[pos][path_i];

                        if turns + path.turns > Input::MAX_STEPS {
                            break;
                        }

                        let mut ice_cream = IceCream::default();
                        prev_pos = pos;

                        for &v in &path.trees {
                            turns += 1;
                            prev_pos = v;
                            ice_cream = ice_cream.push(actual_strawberries[v]);

                            if strawberries[v] != actual_strawberries[v] {
                                actual_strawberries[v] = strawberries[v];
                                turns += 1;
                            }
                        }

                        turns += 1;
                        pos = path.destination;

                        if turns > Input::MAX_STEPS {
                            break;
                        }

                        set[path.destination].insert(ice_cream);
                        continue;
                    }

                    // ランダムに移動
                    let mut trial = 0;

                    let path = loop {
                        let n = rng.random_range(0..Input::SHOP_COUNT);

                        if n != pos {
                            if let Some(path) = &input.nearest_path[pos][n][prev_pos] {
                                if path.len() < 5 || trial >= 100 {
                                    break path;
                                }
                            }
                        }

                        trial += 1;
                    };

                    if turns + path.len() > Input::MAX_STEPS {
                        break;
                    }

                    for &v in path.iter() {
                        turns += 1;
                        prev_pos = pos;
                        pos = v;

                        if strawberries[v] != actual_strawberries[v] {
                            actual_strawberries[v] = strawberries[v];
                            turns += 1;
                        }
                    }
                }

                let score = set.iter().map(|s| s.len()).sum();
                score
            }

            fn simulate(&self, env: &Env) -> Output {
                let mut pos = 0;
                let mut strawberries = [false; Input::VERTEX_COUNT];
                let mut actual_strawberries = [false; Input::VERTEX_COUNT];
                let mut actions = vec![];

                let mut set = vec![FxHashSet::default(); Input::SHOP_COUNT];
                let mut rng = Pcg64Mcg::new(42);
                let mut prev_pos = !0;

                let mut events = self
                    .times
                    .iter()
                    .enumerate()
                    .map(|(i, &t)| (t, i))
                    .collect::<Vec<_>>();
                events.sort_unstable();
                let mut events_i = 0;
                let input = &env.input;

                loop {
                    while events_i < events.len() && events[events_i].0 <= actions.len() {
                        strawberries[events[events_i].1 + Input::SHOP_COUNT] = true;
                        events_i += 1;
                    }

                    let mut best_path_i = None;
                    let mut best_path_turns = usize::MAX;

                    for (path_i, path) in input.path_graph[pos].iter().enumerate() {
                        if path.trees.get(0).unwrap_or(&path.destination) == &prev_pos {
                            continue;
                        }

                        let ice_cream = path.make_ice_cream(&strawberries);

                        if !set[path.destination].contains(&ice_cream)
                            && best_path_turns.change_min(path.turns)
                        {
                            best_path_i = Some(path_i);
                            break;
                        }
                    }

                    if let Some(path_i) = best_path_i {
                        let path = &input.path_graph[pos][path_i];

                        if actions.len() + path.turns > Input::MAX_STEPS {
                            break;
                        }

                        let mut ice_cream = IceCream::default();
                        prev_pos = pos;

                        for &v in &path.trees {
                            actions.push(Action::Move(v));
                            prev_pos = v;
                            ice_cream = ice_cream.push(actual_strawberries[v]);

                            if strawberries[v] != actual_strawberries[v] {
                                actions.push(Action::MakeStrawberry);
                                actual_strawberries[v] = strawberries[v];
                            }
                        }

                        set[path.destination].insert(ice_cream);
                        actions.push(Action::Move(path.destination));
                        pos = path.destination;
                        continue;
                    }

                    // ランダムに移動
                    let mut trial = 0;

                    let path = loop {
                        let n = rng.random_range(0..Input::SHOP_COUNT);

                        if n != pos {
                            if let Some(path) = &input.nearest_path[pos][n][prev_pos] {
                                if path.len() < 5 || trial >= 100 {
                                    break path;
                                }
                            }
                        }

                        trial += 1;
                    };

                    if actions.len() + path.len() > Input::MAX_STEPS {
                        break;
                    }

                    for &v in path.iter() {
                        actions.push(Action::Move(v));
                        prev_pos = pos;
                        pos = v;

                        if strawberries[v] != actual_strawberries[v] {
                            actions.push(Action::MakeStrawberry);
                            actual_strawberries[v] = strawberries[v];
                        }
                    }
                }

                while actions.len() > Input::MAX_STEPS {
                    actions.pop();
                }

                Output::new(actions)
            }
        }

        impl annealing::State for State {
            type Env = Env;
            type Score = SingleScore;

            fn score(&self, env: &Self::Env) -> Self::Score {
                SingleScore(self.calc_score(env) as f64)
            }
        }

        struct ChangeNeigh {
            tree_i: usize,
            old_time: usize,
            new_time: usize,
        }

        impl annealing::Neighbor for ChangeNeigh {
            type Env = Env;
            type State = State;

            fn generate(
                env: &Self::Env,
                state: &Self::State,
                rng: &mut annealing::AnnealingRng,
                progress: f64,
            ) -> Option<Self> {
                let tree_i = rng.random_range(0..Input::TREE_COUNT);
                let old_time = state.times[tree_i];
                let new_time = rng.random_range(0..=Input::MAX_STEPS);

                Some(ChangeNeigh {
                    tree_i,
                    old_time,
                    new_time,
                })
            }

            fn preprocess(&mut self, _env: &Self::Env, state: &mut Self::State) {
                state.times[self.tree_i] = self.new_time;
            }

            fn postprocess(self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn rollback(self, _env: &Self::Env, state: &mut Self::State) {
                state.times[self.tree_i] = self.old_time;
            }
        }
    }
    pub mod greedy {
        use rand::Rng;
        use rand_pcg::Pcg64Mcg;
        use rustc_hash::FxHashSet;

        use crate::{
            problem::{Action, Input, Output},
            util::ChangeMinMax,
        };

        pub fn solve(input: &Input) -> Output {
            let mut pos = 0;
            let mut actions = vec![];
            let mut strawberries = vec![false; Input::VERTEX_COUNT];
            let mut set = vec![FxHashSet::default(); Input::SHOP_COUNT];
            let mut rng = Pcg64Mcg::new(42);
            let mut prev_pos = !0;

            while actions.len() < 9950 {
                let mut best_path_i = None;
                let mut best_path_turns = usize::MAX;

                for (path_i, path) in input.path_graph[pos].iter().enumerate() {
                    if path.trees.get(0).unwrap_or(&path.destination) == &prev_pos {
                        continue;
                    }

                    let ice_cream = path.make_ice_cream(&strawberries);

                    if !set[path.destination].contains(&ice_cream)
                        && best_path_turns.change_min(path.turns)
                    {
                        best_path_i = Some(path_i);
                    }
                }

                if let Some(path_i) = best_path_i {
                    let path = &input.path_graph[pos][path_i];
                    let ice_cream = path.make_ice_cream(&strawberries);
                    set[path.destination].insert(ice_cream);
                    prev_pos = pos;

                    for &v in &path.trees {
                        actions.push(Action::Move(v));
                        prev_pos = v;
                    }

                    actions.push(Action::Move(path.destination));
                    pos = path.destination;
                    continue;
                }

                let mut best_op = None;
                let mut best_path_turns = usize::MAX;

                for (path_i, path) in input.path_graph[pos].iter().enumerate() {
                    if path.trees.get(0).unwrap_or(&path.destination) == &prev_pos {
                        continue;
                    }

                    let ice_cream = path.make_ice_cream(&strawberries);

                    for j in (0..path.len).rev() {
                        let ice_cream = ice_cream | path.strawberries[j];

                        if !set[path.destination].contains(&ice_cream)
                            && best_path_turns.change_min(path.turns + 1)
                        {
                            best_op = Some((path_i, j));
                        }
                    }
                }

                if let Some((path_i, cut_idx)) = best_op {
                    let path = &input.path_graph[pos][path_i];
                    let ice_cream = path.make_ice_cream(&strawberries) | path.strawberries[cut_idx];
                    set[path.destination].insert(ice_cream);

                    prev_pos = pos;

                    for &v in &path.trees[..=cut_idx] {
                        actions.push(Action::Move(v));
                        prev_pos = v;
                    }

                    actions.push(Action::MakeStrawberry);
                    strawberries[path.trees[cut_idx]] = true;

                    for &v in &path.trees[cut_idx + 1..] {
                        actions.push(Action::Move(v));
                        prev_pos = v;
                    }

                    actions.push(Action::Move(path.destination));
                    pos = path.destination;
                    continue;
                }

                // ランダムに移動
                let path = loop {
                    let n = rng.random_range(0..Input::SHOP_COUNT);

                    if n != pos {
                        if let Some(path) = &input.nearest_path[pos][n][prev_pos] {
                            break path;
                        }
                    }
                };

                for &v in path.iter() {
                    actions.push(Action::Move(v));
                    prev_pos = pos;
                    pos = v;
                }
            }

            Output::new(actions)
        }
    }
}
pub mod util {
    //! よく使われるユーティリティ関数をまとめたモジュール

    use num::PrimInt;

    /// 最小値と最大値を更新するトレイト
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::util::ChangeMinMax;
    ///
    /// let mut x = 10;
    /// assert!(x.change_min(3));
    /// assert_eq!(x, 3);
    /// ```
    pub trait ChangeMinMax {
        fn change_min(&mut self, v: Self) -> bool;
        fn change_max(&mut self, v: Self) -> bool;
    }

    impl<T: PartialOrd> ChangeMinMax for T {
        fn change_min(&mut self, v: T) -> bool {
            *self > v && {
                *self = v;
                true
            }
        }

        fn change_max(&mut self, v: T) -> bool {
            *self < v && {
                *self = v;
                true
            }
        }
    }

    /// 多次元配列を作成する
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::mat;
    ///
    /// let a = mat![0; 4; 3];
    /// assert_eq!(a, vec![vec![0; 3]; 4]);
    /// ```
    #[macro_export]
    macro_rules! mat {
	($($e:expr),*) => { vec![$($e),*] };
	($($e:expr,)*) => { vec![$($e),*] };
	($e:expr; $d:expr) => { vec![$e; $d] };
	($e:expr; $d:expr $(; $ds:expr)+) => { vec![mat![$e $(; $ds)*]; $d] };
}

    /// 整数の二分探索を行う
    ///
    /// # Examples
    ///
    /// ```
    /// use cp_lib_rs::util::binary_search;
    ///
    /// let result = binary_search(0, 10, |x| x * x <= 5);
    /// assert_eq!(result, 2);
    /// ```
    pub fn binary_search<T: PrimInt>(ok: T, ng: T, f: impl Fn(T) -> bool) -> T {
        let mut ok = ok;
        let mut ng = ng;

        while ok.max(ng) - ok.min(ng) > T::one() {
            let mid = (ok + ng) / (T::one() << 1);

            if f(mid) {
                ok = mid;
            } else {
                ng = mid;
            }
        }

        ok
    }

    #[cfg(test)]
    mod test {
        use super::*;

        #[test]
        fn binary_search_test() {
            assert_eq!(binary_search(0, 10, |x| x * x <= 5), 2);
            assert_eq!(binary_search(10, 0, |x| x * x >= 5), 3);
        }
    }
}

use crate::problem::{Action, Input};

fn main() {
    let input = Input::read();
    let output = solver::annealing::solve(&input);
    println!("{}", output);
}

