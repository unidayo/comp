#[allow(dead_code)]
mod annealing {
    #![macro_use]
    //! 焼きなましライブラリ
    //!
    use itertools::Itertools;
    use rand::{distributions::Distribution, Rng as _};
    use rand_distr::WeightedAliasIndex;
    use rand_pcg::Pcg64Mcg;
    use std::{
        cell::RefCell,
        collections::BTreeMap,
        fmt::{Debug, Display},
        rc::Rc,
        time::Instant,
    };

    pub type AnnealingRng = Pcg64Mcg;

    /// 焼きなましの状態
    pub trait State {
        type Env;
        type Score: Score;

        /// 生スコア（大きいほど良い）
        fn score(&self, env: &Self::Env) -> Self::Score;
    }

    pub trait Score {
        /// 焼きなまし用スコア（大きいほど良い）
        /// デフォルトでは生スコアをそのまま返す
        fn annealing_score(&self, _progress: f64) -> f64 {
            self.raw_score() as f64
        }

        /// 生スコア
        fn raw_score(&self) -> f64;
    }

    /// 単一の値からなるスコア
    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct SingleScore(pub i64);

    impl Score for SingleScore {
        fn raw_score(&self) -> f64 {
            self.0 as f64
        }
    }

    /// 焼きなましの近傍
    ///
    /// * 受理パターンの流れ: `preprocess()` -> `eval()` -> `postprocess()`
    /// * 却下パターンの流れ: `preprocess()` -> `eval()` -> `rollback()`
    pub trait Neighbor {
        type Env;
        type State: State<Env = Self::Env>;

        fn generate(
            env: &Self::Env,
            state: &Self::State,
            rng: &mut AnnealingRng,
            progress: f64,
        ) -> Option<Box<dyn Neighbor<Env = Self::Env, State = Self::State>>>
        where
            Self: Sized;

        /// `eval()` 前の変形操作を行う
        fn preprocess(&mut self, _env: &Self::Env, _state: &mut Self::State);

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
        fn eval(
            &mut self,
            env: &Self::Env,
            state: &Self::State,
            _progress: f64,
            _threshold: f64,
        ) -> Option<<Self::State as State>::Score> {
            Some(state.score(env))
        }

        /// `eval()` 後の変形操作を行う（2-optの区間reverse処理など）
        fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State);

        /// `preprocess()` で変形した `state` をロールバックする
        fn rollback(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State);

        /// 近傍の名前
        fn name(&self) -> &'static str {
            std::any::type_name::<Self>()
        }
    }

    /// 焼きなましの近傍を生成する構造体
    pub trait NeighborGenerator {
        type Env;
        type State: State;

        /// 近傍を生成する
        fn generate(
            &self,
            env: &Self::Env,
            state: &Self::State,
            rng: &mut AnnealingRng,
            progress: f64,
        ) -> Option<Box<dyn Neighbor<Env = Self::Env, State = Self::State>>>;
    }

    /// WeightedNeighborGenerator用の簡易マクロ
    #[macro_export]
    macro_rules! weighted_neighbor {
    ( $( $ty:ident => $weight:expr ),+ $(,)? ) => {
        $crate::annealing::WeightedNeighborGenerator::new(vec![
            $(
                (Box::new(|env, state, rng, progress| $ty::generate(env, state, rng, progress)), $weight),
            )+
        ])
    };
}

    /// 複数の近傍生成器を重み付きで選択する近傍生成器
    ///
    /// `weighted_neighbor! { NeighborA => 1.0, NeighborB => 2.0 }` のように使用する。
    pub struct WeightedNeighborGenerator<E, S: State<Env = E>> {
        weights: WeightedAliasIndex<f64>,
        generators: Vec<
            Box<
                dyn Fn(
                    &E,
                    &S,
                    &mut AnnealingRng,
                    f64,
                ) -> Option<Box<dyn Neighbor<Env = E, State = S>>>,
            >,
        >,
    }

    impl<E, S: State<Env = E>> WeightedNeighborGenerator<E, S> {
        pub fn new(
            candidates: Vec<(
                Box<
                    dyn Fn(
                        &E,
                        &S,
                        &mut AnnealingRng,
                        f64,
                    ) -> Option<Box<dyn Neighbor<Env = E, State = S>>>,
                >,
                f64,
            )>,
        ) -> Self {
            let weights: Vec<f64> = candidates.iter().map(|c| c.1).collect();
            let weights = WeightedAliasIndex::new(weights)
                .expect("weights must be non-negative and not all zero");
            let generators = candidates.into_iter().map(|(gen, _)| gen).collect_vec();
            Self {
                weights,
                generators,
            }
        }
    }

    impl<E, S> NeighborGenerator for WeightedNeighborGenerator<E, S>
    where
        S: State<Env = E>,
    {
        type Env = E;
        type State = S;

        fn generate(
            &self,
            env: &Self::Env,
            state: &Self::State,
            rng: &mut AnnealingRng,
            progress: f64,
        ) -> Option<Box<dyn Neighbor<Env = Self::Env, State = Self::State>>> {
            let idx = self.weights.sample(rng);
            (self.generators[idx])(env, state, rng, progress)
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
        selected: BTreeMap<&'static str, usize>,
        accepted: BTreeMap<&'static str, usize>,
    }

    impl AnnealingStatistics {
        fn new(init_score: f64) -> Self {
            Self {
                all_iter: 0,
                valid_iter: 0,
                accepted_count: 0,
                updated_count: 0,
                init_score,
                final_score: init_score,
                selected: BTreeMap::new(),
                accepted: BTreeMap::new(),
            }
        }

        fn select(&mut self, name: &'static str) {
            self.valid_iter += 1;
            self.selected
                .entry(name)
                .and_modify(|count| *count += 1)
                .or_insert(1);
        }

        fn accept(&mut self, name: &'static str) {
            self.accepted_count += 1;
            self.accepted
                .entry(name)
                .and_modify(|count| *count += 1)
                .or_insert(1);
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

            for (name, &selected) in self.selected.iter() {
                let accepted = self.accepted.get(name).copied().unwrap_or(0);
                let precent = accepted as f64 / selected as f64 * 100.0;
                let name = name.split("::").last().unwrap_or(name);

                writeln!(
                    f,
                    "{:11}: {} / {} ({:.2}%)",
                    name, accepted, selected, precent
                )?;
            }

            Ok(())
        }
    }

    /// 焼きなましを行う構造体
    ///
    /// `I` は焼きなましの進捗を更新する間隔を指定する。例えば `I = 1024` とすると、1024回に1回の頻度で進捗を更新する。
    #[derive(Debug, Clone)]
    pub struct Annealer<const I: usize> {
        /// 開始温度
        start_temp: f64,
        /// 終了温度
        end_temp: f64,
        /// 乱数シード
        seed: u128,
    }

    impl<const I: usize> Annealer<I> {
        pub fn new(start_temp: f64, end_temp: f64, seed: u128) -> Self {
            Self {
                start_temp,
                end_temp,
                seed,
            }
        }

        pub fn run<E, S: State<Env = E> + Clone, G: NeighborGenerator<Env = E, State = S>>(
            &self,
            env: &E,
            mut state: S,
            neighbor_generator: &G,
            duration_sec: f64,
        ) -> (S, AnnealingStatistics) {
            let mut best_state = state.clone();
            let mut current_score = state.score(&env);
            let mut best_score = current_score.annealing_score(1.0);

            let mut diagnostics = AnnealingStatistics::new(current_score.raw_score());
            let mut rng = Pcg64Mcg::new(self.seed);
            let threshold_generator = ThresholdGenerator::get_singleton();
            let mut threshold_generator = threshold_generator.borrow_mut();
            threshold_generator.set_pos(rng.gen());

            let duration_inv = 1.0 / duration_sec;
            let since = Instant::now();

            let mut progress = 0.0;
            let mut temperature = self.start_temp;

            loop {
                if diagnostics.all_iter % I == 0 {
                    progress = (Instant::now() - since).as_secs_f64() * duration_inv;
                    temperature = f64::powf(self.start_temp, 1.0 - progress)
                        * f64::powf(self.end_temp, progress);

                    if progress >= 1.0 {
                        break;
                    }
                }

                diagnostics.all_iter += 1;

                // 変形
                let Some(mut neighbor) =
                    neighbor_generator.generate(env, &state, &mut rng, progress)
                else {
                    continue;
                };

                diagnostics.select(neighbor.name());
                neighbor.preprocess(env, &mut state);

                // スコア計算
                let threshold =
                    threshold_generator.next(current_score.annealing_score(progress), temperature);
                let Some(new_score) = neighbor.eval(env, &state, progress, threshold) else {
                    // 明らかに閾値に届かない場合はreject
                    neighbor.rollback(env, &mut state);
                    continue;
                };

                if new_score.annealing_score(progress) >= threshold {
                    diagnostics.accept(neighbor.name());
                    current_score = new_score;
                    neighbor.postprocess(env, &mut state);

                    let new_score = current_score.annealing_score(1.0);

                    if best_score < new_score {
                        best_score = new_score;
                        best_state = state.clone();
                        diagnostics.updated_count += 1;
                    }
                } else {
                    neighbor.rollback(env, &mut state);
                }
            }

            diagnostics.final_score = best_state.score(&env).raw_score();

            (best_state, diagnostics)
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
                .map(|_| rng.gen_range(0.0f64..1.0).ln())
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
        use super::{Annealer, Neighbor, Score};
        use itertools::Itertools;
        use rand::Rng;

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

        impl Score for Dist {
            fn annealing_score(&self, _progress: f64) -> f64 {
                // 大きい方が良いとするため符号を反転
                -self.0 as f64
            }

            fn raw_score(&self) -> f64 {
                self.0 as f64
            }
        }

        struct NoOp;

        impl Neighbor for NoOp {
            type Env = Input;
            type State = State;

            fn generate(
                _env: &Self::Env,
                _state: &Self::State,
                _rng: &mut super::AnnealingRng,
                _progress: f64,
            ) -> Option<Box<dyn Neighbor<Env = Self::Env, State = Self::State>>>
            where
                Self: Sized,
            {
                Some(Box::new(NoOp))
            }

            fn preprocess(&mut self, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn rollback(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
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

        impl Neighbor for TwoOpt {
            type Env = Input;
            type State = State;

            fn generate(
                _env: &Self::Env,
                state: &Self::State,
                rng: &mut super::AnnealingRng,
                _progress: f64,
            ) -> Option<Box<dyn Neighbor<Env = Self::Env, State = Self::State>>>
            where
                Self: Sized,
            {
                loop {
                    let begin = rng.gen_range(1..state.order.len());
                    let end = rng.gen_range(1..state.order.len());

                    if begin + 2 <= end {
                        return Some(Box::new(TwoOpt::new(begin, end)));
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

            fn postprocess(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                state.order[self.begin..self.end].reverse();
                state.dist = self
                    .new_dist
                    .expect("postprocess()を呼ぶ前にeval()を呼んでください。");
            }

            fn rollback(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }
        }

        #[test]
        fn annealing_tsp_test() {
            let input = Input::gen_testcase();
            let state = State::new(&input);
            let annealer = Annealer::<1024>::new(1e1, 1e-1, 42);
            let neighbor_generator = crate::weighted_neighbor! {
                NoOp => 1.0,
                TwoOpt => 2.0,
            };

            let (state, diagnostics) = annealer.run(&input, state, &neighbor_generator, 0.1);

            eprintln!("{}", diagnostics);

            eprintln!("score: {}", state.dist);
            eprintln!("state.dist: {:?}", state.order);
            assert_eq!(state.dist, 10);
            assert!(state.order == vec![0, 1, 3, 2, 0] || state.order == vec![0, 2, 3, 1, 0]);
        }
    }
}
#[allow(dead_code)]
mod grid {
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

        const fn to_index(&self, size: usize) -> CoordIndex {
            CoordIndex(self.row as usize * size + self.col as usize)
        }

        pub const fn dist(&self, other: &Self) -> usize {
            Self::dist_1d(self.row, other.row) + Self::dist_1d(self.col, other.col)
        }

        const fn dist_1d(x0: u8, x1: u8) -> usize {
            (x0 as i64 - x1 as i64).abs() as usize
        }
    }

    impl Display for Coord {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({}, {})", self.row, self.col)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
    pub struct CoordIndex(pub usize);

    impl CoordIndex {
        pub const fn to_coord(&self, size: usize) -> Coord {
            Coord::new(self.0 / size, self.0 % size)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
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
}
mod problem {
    use proconio::input;

    use crate::grid::{Coord, Map2d, DIRECTIONS};
    use std::{fmt::Display, time::Instant};

    #[derive(Debug, Clone)]
    pub struct Input {
        pub weights: Map2d<u32>,
        pub durabilities: Map2d<u32>,
        pub since: Instant,
    }

    impl Input {
        pub const MAP_SIZE: usize = 20;
        pub const START: Coord = Coord::new(0, 0);

        pub fn read() -> Self {
            input! {
                _n: usize,
                weights: [u32; Self::MAP_SIZE * Self::MAP_SIZE],
                durabilities: [u32; Self::MAP_SIZE * Self::MAP_SIZE],
            }

            let since = Instant::now();
            let weights = Map2d::new(weights, Self::MAP_SIZE);
            let durabilities = Map2d::new(durabilities, Self::MAP_SIZE);

            Self {
                weights,
                durabilities,
                since,
            }
        }
    }

    pub struct Output {
        pub actions: Vec<Action>,
        pub score: u32,
    }

    impl Output {
        pub fn new(actions: Vec<Action>) -> Self {
            let score = actions
                .iter()
                .filter(|&&action| matches!(action, Action::Move(_)))
                .count() as u32;
            Self { actions, score }
        }
    }

    impl Display for Output {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            for action in self.actions.iter() {
                writeln!(f, "{}", action)?;
            }

            Ok(())
        }
    }

    #[allow(dead_code)]
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum Action {
        Pick,
        Place,
        Move(usize),
    }

    impl Display for Action {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            match self {
                Action::Pick => write!(f, "1"),
                Action::Place => write!(f, "2"),
                Action::Move(x) => write!(f, "{}", DIRECTIONS[*x]),
            }
        }
    }
}
mod solver {
    use crate::problem::{Input, Output};

    mod annealing {
        use crate::{
            annealing::{self, Annealer, Neighbor as _, SingleScore},
            grid::Coord,
            problem::Input,
        };
        use itertools::Itertools;
        use rand::Rng;

        pub(super) fn solve(input: &Input, groups: Vec<Vec<Coord>>) -> Vec<Vec<Coord>> {
            let env = Env::new(input.clone());
            let state = State::new(groups);
            let neigh_gen = weighted_neighbor! {
                SwapStackNeigh => 1.0
            };

            let annealer = Annealer::<1024>::new(3e0, 3e-1, 42);
            let (mut state, stats) = annealer.run(&env, state, &neigh_gen, 1.9);
            eprintln!("Annealing stats: {}", stats);
            state.groups.retain(|group| !group.is_empty());
            //dbg!(&state);

            state.groups
        }

        struct Env {
            input: Input,
        }

        impl Env {
            fn new(input: Input) -> Self {
                Self { input }
            }
        }

        #[derive(Debug, Clone)]
        struct State {
            groups: Vec<Vec<Coord>>,
            dists: Vec<u32>,
            dist_sum: u32,
        }

        impl State {
            fn new(groups: Vec<Vec<Coord>>) -> Self {
                let dists = groups
                    .iter()
                    .map(|group| {
                        let mut current = Input::START;
                        let mut d = 0;

                        for pos in group.iter() {
                            d += current.dist(pos);
                            current = *pos;
                        }

                        d += current.dist(&Input::START);
                        d as u32
                    })
                    .collect_vec();

                let dist_sum = dists.iter().sum();

                Self {
                    groups,
                    dists,
                    dist_sum,
                }
            }
        }

        impl annealing::State for State {
            type Env = Env;
            type Score = SingleScore;

            fn score(&self, _env: &Self::Env) -> Self::Score {
                SingleScore(-(self.dist_sum as i64))
            }
        }

        struct SwapStackNeigh {
            stack0: usize,
            stack1: usize,
            pos0: usize,
            pos1: usize,
            old_dist0: u32,
            old_dist1: u32,
            new_dist0: u32,
            new_dist1: u32,
        }

        impl annealing::Neighbor for SwapStackNeigh {
            type Env = Env;
            type State = State;

            fn generate(
                env: &Self::Env,
                state: &Self::State,
                rng: &mut annealing::AnnealingRng,
                _progress: f64,
            ) -> Option<Box<dyn annealing::Neighbor<Env = Self::Env, State = Self::State>>>
            where
                Self: Sized,
            {
                let (stack0, stack1) = loop {
                    let stack0 = rng.gen_range(0..state.groups.len());
                    let stack1 = rng.gen_range(0..state.groups.len());

                    if stack0 != stack1 {
                        break (stack0, stack1);
                    }
                };

                if state.groups[stack0].is_empty() || state.groups[stack1].is_empty() {
                    // Cannot swap with an empty stack
                    return None;
                }

                let pos0 = rng.gen_range(0..=state.groups[stack0].len());
                let pos1 = rng.gen_range(0..=state.groups[stack1].len());
                let old_dist0 = state.dists[stack0];
                let old_dist1 = state.dists[stack1];
                let mut new_dist0 = 0;
                let mut new_dist1 = 0;

                if (pos0 == 0 && pos1 == 0)
                    || (pos0 == state.groups[stack0].len() && pos1 == state.groups[stack1].len())
                {
                    // Cannot swap the start or end of a stack
                    return None;
                }

                for (s0, s1, p0, p1, dist_sum) in [
                    (stack0, stack1, pos0, pos1, &mut new_dist0),
                    (stack1, stack0, pos1, pos0, &mut new_dist1),
                ] {
                    let mut damage_sum = 0;
                    let mut current = Input::START;

                    for i in (p1..state.groups[s1].len()).rev() {
                        let pos = state.groups[s1][i];
                        let dist = pos.dist(&current) as u32;

                        if damage_sum >= env.input.durabilities[pos] {
                            // This swap is not valid, as the stack would break
                            return None;
                        }

                        let weight = env.input.weights[pos];
                        *dist_sum += dist;
                        damage_sum += weight * *dist_sum;
                        current = pos;
                    }

                    for i in (0..p0).rev() {
                        let pos = state.groups[s0][i];
                        let dist = pos.dist(&current) as u32;

                        if damage_sum >= env.input.durabilities[pos] {
                            // This swap is not valid, as the stack would break
                            return None;
                        }

                        let weight = env.input.weights[pos];
                        *dist_sum += dist;
                        damage_sum += weight * *dist_sum;
                        current = pos;
                    }

                    *dist_sum += current.dist(&Input::START) as u32;
                }

                Some(Box::new(Self {
                    stack0,
                    stack1,
                    pos0,
                    pos1,
                    old_dist0,
                    old_dist1,
                    new_dist0,
                    new_dist1,
                }))
            }

            fn preprocess(&mut self, _env: &Self::Env, state: &mut Self::State) {
                state.dist_sum += self.new_dist0 + self.new_dist1 - self.old_dist0 - self.old_dist1;
            }

            fn postprocess(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                state.dists[self.stack0] = self.new_dist0;
                state.dists[self.stack1] = self.new_dist1;

                let mut new_stack0 = vec![];
                let mut new_stack1 = vec![];

                for i in 0..self.pos0 {
                    new_stack0.push(state.groups[self.stack0][i]);
                }

                for i in self.pos1..state.groups[self.stack1].len() {
                    new_stack0.push(state.groups[self.stack1][i]);
                }

                for i in 0..self.pos1 {
                    new_stack1.push(state.groups[self.stack1][i]);
                }

                for i in self.pos0..state.groups[self.stack0].len() {
                    new_stack1.push(state.groups[self.stack0][i]);
                }

                state.groups[self.stack0] = new_stack0;
                state.groups[self.stack1] = new_stack1;
            }

            fn rollback(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                state.dist_sum += self.old_dist0 + self.old_dist1 - self.new_dist0 - self.new_dist1;
            }
        }
    }
    mod greedy {
        use crate::{
            grid::{Coord, Map2d},
            problem::Input,
            util::ChangeMinMax,
        };

        pub(super) fn solve(input: &Input) -> Vec<Vec<Coord>> {
            let mut groups = vec![];
            let mut used = Map2d::with_default(Input::MAP_SIZE);
            used[Input::START] = true;
            let mut remain = Input::MAP_SIZE * Input::MAP_SIZE - 1;
            let mut current;

            while remain > 0 {
                let mut group = vec![];

                // 1. (0,0)から最も遠い未回収箱を探す
                let mut max_dist = 0;
                let mut target = Input::START;

                for row in 0..Input::MAP_SIZE {
                    for col in 0..Input::MAP_SIZE {
                        if used[row][col] {
                            continue;
                        }

                        let c = Coord::new(row, col);
                        let dist = c.dist(&Input::START); // (0,0)からのマンハッタン距離

                        if max_dist.change_max(dist) {
                            target = c;
                        }
                    }
                }

                let mut stack: Vec<u32> = vec![];
                group.push(target);

                // Pick
                stack.push(input.durabilities[target]);
                used[target] = true;
                remain -= 1;
                current = target;

                // 2. (cur,0,0)間で積めるだけ積む
                loop {
                    // (cur→(0,0))の経路上で、壊れずに積める最遠の箱を探す
                    let mut best = None;
                    let mut best_dist = 0;

                    for row in 0..=current.row() {
                        for col in 0..=current.col() {
                            let c = Coord::new(row, col);

                            if used[c] {
                                continue;
                            }

                            // stackに新箱を積んだときの耐久力シミュレーション
                            let damage = input.weights[c] * (row as u32 + col as u32);

                            if stack.iter().any(|&d| d <= damage) {
                                continue;
                            }

                            if best_dist.change_max(c.dist(&Input::START)) {
                                best = Some(c);
                            }
                        }
                    }

                    if let Some(target) = best {
                        group.push(target);

                        let damage =
                            input.weights[target] * (target.row() as u32 + target.col() as u32);

                        for d in stack.iter_mut() {
                            *d -= damage;
                        }

                        current = target;
                        stack.push(input.durabilities[target]);
                        used[target] = true;
                        remain -= 1;
                    } else {
                        break;
                    }
                }

                groups.push(group);
            }

            groups
        }
    }
    mod refine {
        use crate::annealing::Neighbor as _;
        use rand::Rng as _;
        use std::{time::Duration, u32};

        use crate::{
            annealing::{self, Annealer, SingleScore},
            grid::{Coord, Map2d, ADJACENTS, D, L, R, U},
            problem::{Action, Input, Output},
            util::ChangeMinMax,
        };

        pub(super) fn refine(input: &Input, groups: Vec<Vec<Coord>>) -> Output {
            let tl = Duration::from_millis(1990);

            let env = Env::new(input.clone());
            let state = State::new(groups);
            let neigh_gen = weighted_neighbor! {
                InsertNeigh => 1.0,
                SwapStackNeigh => 1.0,
            };

            let duration = (tl - input.since.elapsed()).as_secs_f64();
            let annealer = Annealer::<1>::new(5e-1, 1e-1, 42);
            let (state, stats) = annealer.run(&env, state, &neigh_gen, duration);
            eprintln!("{}", stats);

            state.to_output(&env)
        }

        struct Env {
            input: Input,
        }

        impl Env {
            fn new(input: Input) -> Self {
                Self { input }
            }
        }

        #[derive(Clone, Debug)]
        struct State {
            groups: Vec<Vec<Coord>>,
        }

        impl State {
            fn new(groups: Vec<Vec<Coord>>) -> Self {
                Self { groups }
            }

            fn to_output(&self, env: &Env) -> Output {
                let mut wants: Map2d<Option<Coord>> = Map2d::with_default(Input::MAP_SIZE);
                let mut inv_wants = Map2d::new(
                    vec![vec![]; Input::MAP_SIZE * Input::MAP_SIZE],
                    Input::MAP_SIZE,
                );
                let mut group_ids = Map2d::with_default(Input::MAP_SIZE);

                for (id, group) in self.groups.iter().enumerate() {
                    for p in group.iter() {
                        group_ids[*p] = id;
                    }

                    let mut group = group.clone();
                    group.insert(0, Input::START);
                    group.push(Input::START);

                    for i in 1..group.len() - 1 {
                        let c0 = group[i - 1];
                        let c1 = group[i];
                        let c2 = group[i + 1];

                        if c0.dist(&c2) == c0.dist(&c1) + c1.dist(&c2) {
                            continue;
                        }

                        let mut best_dist = usize::MAX;
                        let mut best_pos = Coord::new(!0, !0);

                        for row in c0.row().min(c2.row())..=c0.row().max(c2.row()) {
                            for col in c0.col().min(c2.col())..=c0.col().max(c2.col()) {
                                let c = Coord::new(row, col);
                                let dist = c.dist(&c1);

                                if best_dist.change_min(dist) {
                                    best_pos = c;
                                }
                            }
                        }

                        wants[c1] = Some(best_pos);
                        inv_wants[best_pos].push(c1);
                    }
                }

                let mut actions = vec![];
                let mut current_positions = Map2d::with_default(Input::MAP_SIZE);
                let mut is_vacant: Map2d<bool> = Map2d::with_default(Input::MAP_SIZE);

                for row in 0..Input::MAP_SIZE {
                    for col in 0..Input::MAP_SIZE {
                        let c = Coord::new(row, col);
                        current_positions[c] = c;
                    }
                }

                let mut current = Input::START;

                for (group_i, group) in self.groups.iter().enumerate() {
                    let mut durabilities = group
                        .iter()
                        .map(|&c| env.input.durabilities[c])
                        .collect::<Vec<_>>();

                    let mut damage_sum = 0;
                    let mut dist_sum = 0;
                    let mut current_local = Input::START;

                    for (i, &p) in group.iter().enumerate().rev() {
                        let pos = current_positions[p];
                        dist_sum += current_local.dist(&pos) as u32;
                        durabilities[i] -= damage_sum;
                        damage_sum += env.input.weights[p] * dist_sum;
                        current_local = pos;
                    }

                    for (i, &p) in group.iter().enumerate() {
                        let target = current_positions[p];

                        let mut yorimichi = None;
                        let dist = current.dist(&target);

                        'find: for row in
                            current.row().min(target.row())..current.row().max(target.row())
                        {
                            for col in
                                current.col().min(target.col())..current.col().max(target.col())
                            {
                                let c = Coord::new(row, col);

                                if !is_vacant[c] {
                                    continue;
                                }

                                for &tgt in inv_wants[c].iter() {
                                    if wants[tgt] != Some(c) || group_i >= group_ids[tgt] {
                                        continue;
                                    }

                                    let d = current.dist(&tgt) + tgt.dist(&c) + c.dist(&target);

                                    if d > dist {
                                        continue;
                                    }

                                    let carry_d = tgt.dist(&c) as u32;
                                    let damage = carry_d * env.input.weights[tgt];

                                    if durabilities.iter().take(i).any(|&d| d <= damage) {
                                        // 寄り道すると壊れる
                                        continue;
                                    }

                                    yorimichi = Some((tgt, c));
                                    break 'find;
                                }
                            }
                        }

                        if let Some((from, to)) = yorimichi {
                            let carry_d = from.dist(&to) as u32;
                            let damage = carry_d * env.input.weights[from];

                            for i in 0..i {
                                durabilities[i] -= damage;
                            }

                            // 寄り道をする
                            wants[from] = None;
                            is_vacant[from] = true;
                            is_vacant[to] = false;
                            current_positions[from] = to;

                            add_moves(&mut current, from, &mut actions);
                            actions.push(Action::Pick);
                            add_moves(&mut current, to, &mut actions);
                            actions.push(Action::Place);
                        }

                        add_moves(&mut current, target, &mut actions);
                        actions.push(Action::Pick);
                        is_vacant[target] = true;
                    }

                    add_moves(&mut current, Input::START, &mut actions);
                }

                let output = Output::new(actions);
                output
            }
        }

        impl annealing::State for State {
            type Env = Env;
            type Score = SingleScore;

            fn score(&self, env: &Self::Env) -> Self::Score {
                let output = self.to_output(env);
                SingleScore(-(output.score as i64))
            }
        }

        struct InsertNeigh {
            from: usize,
            to: usize,
        }

        impl annealing::Neighbor for InsertNeigh {
            type Env = Env;
            type State = State;

            fn generate(
                _env: &Self::Env,
                state: &Self::State,
                rng: &mut annealing::AnnealingRng,
                _progress: f64,
            ) -> Option<Box<dyn annealing::Neighbor<Env = Self::Env, State = Self::State>>>
            where
                Self: Sized,
            {
                loop {
                    let from = rng.gen_range(0..state.groups.len());
                    let to = rng.gen_range(0..state.groups.len());

                    if from == to {
                        continue;
                    }

                    return Some(Box::new(Self { from, to }));
                }
            }

            fn preprocess(&mut self, _env: &Self::Env, state: &mut Self::State) {
                let group = state.groups.remove(self.from);
                state.groups.insert(self.to, group);
            }

            fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                // 何もしない
            }

            fn rollback(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                let group = state.groups.remove(self.to);
                state.groups.insert(self.from, group);
            }
        }

        struct SwapStackNeigh {
            stack0: usize,
            stack1: usize,
            pos0: usize,
            pos1: usize,
            old_group0: Vec<Coord>,
            old_group1: Vec<Coord>,
        }

        impl annealing::Neighbor for SwapStackNeigh {
            type Env = Env;
            type State = State;

            fn generate(
                env: &Self::Env,
                state: &Self::State,
                rng: &mut annealing::AnnealingRng,
                _progress: f64,
            ) -> Option<Box<dyn annealing::Neighbor<Env = Self::Env, State = Self::State>>>
            where
                Self: Sized,
            {
                let (stack0, stack1) = loop {
                    let stack0 = rng.gen_range(0..state.groups.len());
                    let stack1 = rng.gen_range(0..state.groups.len());

                    if stack0 != stack1 {
                        break (stack0, stack1);
                    }
                };

                let pos0 = rng.gen_range(0..=state.groups[stack0].len());
                let pos1 = rng.gen_range(0..=state.groups[stack1].len());
                let mut new_dist0 = 0;
                let mut new_dist1 = 0;

                for (s0, s1, p0, p1, dist_sum) in [
                    (stack0, stack1, pos0, pos1, &mut new_dist0),
                    (stack1, stack0, pos1, pos0, &mut new_dist1),
                ] {
                    let mut damage_sum = 0;
                    let mut current = Input::START;

                    for i in (p1..state.groups[s1].len()).rev() {
                        let pos = state.groups[s1][i];
                        let dist = pos.dist(&current) as u32;

                        if damage_sum >= env.input.durabilities[pos] {
                            // This swap is not valid, as the stack would break
                            return None;
                        }

                        let weight = env.input.weights[pos];
                        *dist_sum += dist;
                        damage_sum += weight * *dist_sum;
                        current = pos;
                    }

                    for i in (0..p0).rev() {
                        let pos = state.groups[s0][i];
                        let dist = pos.dist(&current) as u32;

                        if damage_sum >= env.input.durabilities[pos] {
                            // This swap is not valid, as the stack would break
                            return None;
                        }

                        let weight = env.input.weights[pos];
                        *dist_sum += dist;
                        damage_sum += weight * *dist_sum;
                        current = pos;
                    }

                    *dist_sum += current.dist(&Input::START) as u32;
                }

                Some(Box::new(Self {
                    stack0,
                    stack1,
                    pos0,
                    pos1,
                    old_group0: state.groups[stack0].clone(),
                    old_group1: state.groups[stack1].clone(),
                }))
            }

            fn preprocess(&mut self, _env: &Self::Env, state: &mut Self::State) {
                let mut new_stack0 = vec![];
                let mut new_stack1 = vec![];

                for i in 0..self.pos0 {
                    new_stack0.push(state.groups[self.stack0][i]);
                }

                for i in self.pos1..state.groups[self.stack1].len() {
                    new_stack0.push(state.groups[self.stack1][i]);
                }

                for i in 0..self.pos1 {
                    new_stack1.push(state.groups[self.stack1][i]);
                }

                for i in self.pos0..state.groups[self.stack0].len() {
                    new_stack1.push(state.groups[self.stack0][i]);
                }

                state.groups[self.stack0] = new_stack0;
                state.groups[self.stack1] = new_stack1;
            }

            fn postprocess(self: Box<Self>, _env: &Self::Env, _state: &mut Self::State) {
                // do nothing
            }

            fn rollback(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                state.groups[self.stack0] = self.old_group0;
                state.groups[self.stack1] = self.old_group1;
            }
        }

        fn add_moves(current: &mut Coord, target: Coord, actions: &mut Vec<Action>) {
            while current.row() < target.row() {
                actions.push(Action::Move(D)); // 下
                *current += ADJACENTS[D];
            }

            while current.col() < target.col() {
                actions.push(Action::Move(R)); // 右
                *current += ADJACENTS[R];
            }

            while current.row() > target.row() {
                actions.push(Action::Move(U)); // 上
                *current += ADJACENTS[U];
            }

            while current.col() > target.col() {
                actions.push(Action::Move(L)); // 左
                *current += ADJACENTS[L];
            }
        }
    }

    pub(super) fn solve(input: &Input) -> Output {
        let groups = greedy::solve(input);
        let groups = annealing::solve(input, groups);
        refine::refine(input, groups)
    }
}
#[allow(dead_code)]
mod util {
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
    #[allow(dead_code)]
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
    #[allow(dead_code)]
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

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
    pub struct BitSetIterU128 {
        flag: u128,
    }

    impl BitSetIterU128 {
        pub fn new(flag: u128) -> Self {
            Self { flag }
        }
    }

    impl Iterator for BitSetIterU128 {
        type Item = usize;

        fn next(&mut self) -> Option<Self::Item> {
            while self.flag != 0 {
                let lsb = self.flag.trailing_zeros() as usize;
                self.flag ^= self.flag & -(self.flag as i128) as u128;
                return Some(lsb);
            }

            None
        }
    }

    #[cfg(test)]
    mod test {
        use super::*;

        #[test]
        fn binary_search_test() {
            assert_eq!(binary_search(0, 10, |x| x * x <= 5), 2);
            assert_eq!(binary_search(10, 0, |x| x * x >= 5), 3);
        }

        #[test]
        fn bitset_iter_u128_test() {
            let mut iter = BitSetIterU128 {
                flag: 0b100001101010,
            };
            assert_eq!(iter.next(), Some(1));
            assert_eq!(iter.next(), Some(3));
            assert_eq!(iter.next(), Some(5));
            assert_eq!(iter.next(), Some(6));
            assert_eq!(iter.next(), Some(11));
            assert_eq!(iter.next(), None);
        }
    }
}

use crate::solver::solve;

fn main() {
    let input = problem::Input::read();
    let output = solve(&input);
    println!("{}", output);
}
