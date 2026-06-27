#[allow(dead_code)]
mod annealing {
    //! 焼きなましライブラリ

    use itertools::Itertools;
    use rand::Rng;
    use rand_pcg::Pcg64Mcg;
    use std::{
        cell::RefCell,
        fmt::{Debug, Display},
        rc::Rc,
        time::Instant,
    };

    /// 焼きなましの状態
    pub trait State {
        type Env;
        type StateValue: Clone;
        type Score: Score + Clone + PartialEq + Debug;

        /// 生スコア（大きいほど良い）
        fn score(&self, env: &Self::Env) -> Self::Score;

        fn state_value(&self) -> &Self::StateValue;
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
    pub trait Neighbor {
        type Env;
        type State: State<Env = Self::Env>;

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
            rng: &mut impl Rng,
        ) -> Box<dyn Neighbor<Env = Self::Env, State = Self::State>>;
    }

    /// 焼きなましの統計データ
    #[derive(Debug, Clone, Copy)]
    pub struct AnnealingStatistics {
        all_iter: usize,
        accepted_count: usize,
        updated_count: usize,
        init_score: f64,
        final_score: f64,
    }

    impl AnnealingStatistics {
        fn new(init_score: f64) -> Self {
            Self {
                all_iter: 0,
                accepted_count: 0,
                updated_count: 0,
                init_score,
                final_score: init_score,
            }
        }
    }

    impl Display for AnnealingStatistics {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            writeln!(f, "===== annealing =====")?;
            writeln!(f, "init score : {}", self.init_score)?;
            writeln!(f, "score      : {}", self.final_score)?;
            writeln!(f, "all iter   : {}", self.all_iter)?;
            writeln!(f, "accepted   : {}", self.accepted_count)?;
            writeln!(f, "updated    : {}", self.updated_count)?;

            Ok(())
        }
    }

    #[derive(Debug, Clone)]
    pub struct Annealer {
        /// 開始温度
        start_temp: f64,
        /// 終了温度
        end_temp: f64,
        /// 乱数シード
        seed: u128,
        /// 時間計測を行うインターバル
        clock_interval: usize,
    }

    impl Annealer {
        pub fn new(start_temp: f64, end_temp: f64, seed: u128, clock_interval: usize) -> Self {
            Self {
                start_temp,
                end_temp,
                seed,
                clock_interval,
            }
        }

        pub fn run<E, S: State<Env = E>, G: NeighborGenerator<Env = E, State = S>>(
            &self,
            env: &E,
            mut state: S,
            neighbor_generator: &G,
            duration_sec: f64,
        ) -> (S::StateValue, AnnealingStatistics) {
            let mut best_state = state.state_value().clone();
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
                diagnostics.all_iter += 1;

                if diagnostics.all_iter % self.clock_interval == 0 {
                    progress = (Instant::now() - since).as_secs_f64() * duration_inv;
                    temperature = f64::powf(self.start_temp, 1.0 - progress)
                        * f64::powf(self.end_temp, progress);

                    if progress >= 1.0 {
                        break;
                    }
                }

                // 変形
                let mut neighbor = neighbor_generator.generate(env, &state, &mut rng);
                neighbor.preprocess(env, &mut state);

                // スコア計算
                let threshold =
                    threshold_generator.next(current_score.annealing_score(progress), temperature);
                let Some(new_score) = neighbor.eval(env, &state, progress, threshold) else {
                    // 明らかに閾値に届かない場合はreject
                    neighbor.rollback(env, &mut state);
                    debug_assert_eq!(state.score(&env), current_score);
                    continue;
                };

                if new_score.annealing_score(progress) >= threshold {
                    // 解の更新
                    neighbor.postprocess(env, &mut state);
                    debug_assert_eq!(state.score(&env), new_score);

                    current_score = new_score;
                    diagnostics.accepted_count += 1;

                    let new_score = current_score.annealing_score(1.0);

                    if best_score < new_score {
                        best_score = new_score;
                        best_state = state.state_value().clone();
                        diagnostics.updated_count += 1;
                    }
                } else {
                    neighbor.rollback(env, &mut state);
                    debug_assert_eq!(state.score(&env), current_score);
                }
            }

            diagnostics.final_score = best_score;

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
}
#[allow(dead_code)]
mod arithmetic {
    /// u16 向けマジックナンバー高速剰余構造体
    #[derive(Debug, Clone, Copy)]
    pub struct FastMod16 {
        d: u16,
        mul: u32,
        shift: u8,
    }

    impl FastMod16 {
        /// d > 0 の前提でマジックナンバーを計算し構造体を構築
        pub fn new(d: u16) -> Self {
            assert!(d > 0, "除数 d must be non-zero");
            let (mul, shift) = Self::compute_magic(d as u32);

            FastMod16 { d, mul, shift }
        }

        fn compute_magic(d: u32) -> (u32, u8) {
            let bits = 16;
            let two_to_bits = 1u64 << bits;
            let anc = two_to_bits - 1 - (two_to_bits - 1) % d as u64;

            let mut p = bits;
            let mut q1 = two_to_bits / anc;
            let mut r1 = two_to_bits - q1 * anc;
            let mut q2 = two_to_bits / d as u64;
            let mut r2 = two_to_bits - q2 * d as u64;

            loop {
                p += 1;
                q1 <<= 1;
                r1 <<= 1;
                if r1 >= anc {
                    q1 += 1;
                    r1 -= anc;
                }
                q2 <<= 1;
                r2 <<= 1;
                if r2 >= d as u64 {
                    q2 += 1;
                    r2 -= d as u64;
                }

                let delta = d as u64 - r2;
                if !(q1 < delta || (q1 == delta && r1 == 0)) {
                    break;
                }
            }

            let m = (q2 + 1) as u32;
            let s = p as u8 - bits as u8;
            (m, s)
        }

        /// n / d を高速に計算。
        pub fn div(&self, n: u16) -> u16 {
            // threshold を超えると商がオーバーフローしてしまい、正しい値が計算できない
            // そのため、微妙な速度差には目を瞑り u64 で計算する
            let prod = (n as u64) * (self.mul as u64);
            let q = ((prod >> 16) >> self.shift) as u32;

            q as u16
        }

        /// n % d を高速に計算。
        pub fn modulo(&self, n: u16) -> u16 {
            // 剰余 r = n - q * d
            n - self.div(n) * self.d
        }
    }

    #[cfg(test)]
    mod tests {
        use super::*;

        #[test]
        fn test_fast_div16() {
            for div in 1..=1024 {
                let fast_mod = FastMod16::new(div);

                for i in 0..=u16::MAX {
                    let expected = i / div;
                    let actual = fast_mod.div(i);
                    assert_eq!(expected, actual, "Failed for div: {}, i: {}", div, i);
                }
            }
        }

        #[test]
        fn test_fast_mod16() {
            for div in 1..=1024 {
                let fast_mod = FastMod16::new(div);

                for i in 0..=u16::MAX {
                    let expected = i % div;
                    let actual = fast_mod.modulo(i);
                    assert_eq!(expected, actual, "Failed for div: {}, i: {}", div, i);
                }
            }
        }
    }
}
#[allow(dead_code)]
mod beam {
    //! ビームサーチライブラリ
    //!
    //! eijirouさんのライブラリをベースにRust化
    //! https://eijirou-kyopro.hatenablog.com/entry/2024/02/01/115639
    //!
    //! # Traits
    //!
    //! 実装すべきトレイトは以下の通り
    //!
    //! - `State`: 状態を表すトレイト
    //!   - `make_initial_node`: 初期の `Evaluator` と `BeamHash` を生成する関数
    //!   - `expand`: 状態を展開し、 `candidate_set` に候補を追加する関数
    //! - `Evaluator`: 評価値を表すトレイト
    //!   - `evaluate`: コストを計算する関数（コストは小さい方が良い）
    //! - `Action`: 行動を表すトレイト
    //!   - `apply`: 行動を適用する関数
    //!   - `rollback`: 行動を元に戻す関数
    //! - `BeamHash`: ビームサーチのハッシュ値を表すトレイト
    //!
    //! # Usage
    //!
    //! ```rust
    //! let state = State::new();
    //! let beam_width_suggester = beam::common::FixedBeamWidthSuggester::new(4500);
    //! // 71119は内部のハッシュテーブルのサイズで、ビーム幅の16倍程度の素数を指定する
    //! let beam_search =
    //!     beam::single_turn::BeamSearch::<_, 71999>::new(beam_width_suggester, 10000);
    //! let result = beam_search.run_default(state).unwrap();
    //! ```
    //!
    //! # Crates
    //!
    //! - `common`: ビームサーチの共通部分
    //! - `single_turn`: 1ターン先にしか遷移できない代わりに高速なビームサーチ
    //!   - 内部実装としてはオイラーツアーの辺を配列として保持
    //! - `multi_turn`: 複数ターン先に遷移できるビームサーチ
    //!   - 内部実装としては二重連鎖木のノードを保持
    pub mod common {
        use ac_library::Monoid;
        use num_traits::bounds::LowerBounded;
        use std::{
            convert::Infallible, error::Error, fmt::Display, marker::PhantomData,
            num::NonZeroUsize, time::Instant,
        };

        pub trait BeamWidthSuggester {
            fn suggest(&mut self) -> usize;
            fn max_width(&self) -> usize;
        }

        pub struct FixedBeamWidthSuggester {
            beam_width: usize,
        }

        impl FixedBeamWidthSuggester {
            pub fn new(beam_width: usize) -> Self {
                assert!(beam_width > 0, "Beam width must be greater than 0.");
                Self { beam_width }
            }
        }

        impl BeamWidthSuggester for FixedBeamWidthSuggester {
            fn suggest(&mut self) -> usize {
                self.beam_width
            }

            fn max_width(&self) -> usize {
                self.beam_width
            }
        }

        /// ベイズ推定+カルマンフィルタにより適切なビーム幅を計算するBeamWidthSuggester。
        /// 1ターンあたりの実行時間が正規分布に従うと仮定し、+3σ分の余裕を持ってビーム幅を決める。
        ///
        /// ## モデル
        ///
        /// カルマンフィルタを適用するにあたって、以下のモデルを考える。
        ///
        /// - `i` ターン目のビーム幅1あたりの所要時間の平均値 `t_i` が正規分布 `N(μ_i, σ_i^2)` に従うと仮定する。
        ///   - 各ターンに観測される所要時間が `N(μ_i, σ_i^2)` に従うのではなく、所要時間の**平均値**が `N(μ_i, σ_i^2)` に従うとしている点に注意。
        ///     - すなわち `μ_i` は所要時間の平均値の平均値であり、所要時間の平均値が `μ_i` を中心とした確率分布を形成しているものとしている。ややこしい。
        ///   - この `μ_i` , `σ_i^2` をベイズ推定によって求めたい。
        /// - 所要時間 `t_i` は `t_{i+1}=t_i+N(0, α^2)` により更新されるものとする。
        ///   - `N(0, α^2)` は標準偏差 `α` のノイズを意味する。お気持ちとしては「実行時間がターン経過に伴ってちょっとずつ変わっていくことがあるよ」という感じ。
        ///   - `α` は既知の定数とし、適当に決める。
        ///   - 本来は問題に合わせたちゃんとした更新式にすべき（ターン経過に伴って線形に増加するなど）なのだが、事前情報がないため大胆に仮定する。
        /// - 所要時間の観測値 `τ_i` は `τ_i=t_i+N(0, β^2)` により得られるものとする。
        ///   - `β` は既知の定数とし、適当に決める。
        ///   - 本来この `β` も推定できると嬉しいのだが、取扱いが煩雑になるためこちらも大胆に仮定する。
        ///
        /// ## モデルの初期化
        ///
        /// - `μ_0` は実行時間制限を `T` 、標準ビーム幅を `W` 、実行ターン数を `M` として、 `μ_0=T/WM` などとすればよい。
        /// - `σ_0` は適当に `σ_0=0.1μ_0` とする。ここは標準ビーム幅にどのくらい自信があるかによる。
        /// - `α` は適当に `α=0.01μ_0` とする。定数は本当に勘。多分問題に合わせてちゃんと考えた方が良い。
        /// - `β` は `σ_0=0.05μ_0` とする。適当なベンチマーク問題で標準偏差を取ったらそのくらいだったため。
        ///
        /// ## モデルの更新
        ///
        /// 以下のように更新をかけていく。
        ///
        /// 1. `t_0=N(μ_0, σ_0^2)` と初期化する。
        /// 2. `t_1=t_0+N(0, α^2)` とし、事前分布 `t_1=N(μ_1, σ_1^2)=N(μ_0, σ_0^2+α^2)` を得る。ここはベイズ更新ではなく単純な正規分布の合成でよい。
        /// 3. `τ_1` が観測されるので、ベイズ更新して事後分布 `N(μ_1', σ_1^2')` を得る。
        /// 4. 同様に `t_2=N(μ_2, σ_2^2)` を得る。
        /// 5. `τ_2` を用いてベイズ更新。以下同様。
        ///
        /// ## 適切なビーム幅の推定
        ///
        /// - 余裕を持って、99.8%程度の確率（+3σ）で実行時間制限に収まるようなビーム幅にしたい。
        /// - ここで、 `t_i=t_{i+1}=･･･=t_M=N(μ_i, σ_i^2)` と大胆仮定する。
        ///   - `α` によって `t_i` がどんどん変わってしまうと考えるのは保守的すぎるため。
        /// - すると残りターン数 `M_i=M-i` として、 `Στ_i=N(M_i*μ_i, M_i*σ_i^2)` となる。
        /// - したがって、残り時間を `T_i` として `W(M_i*μ_i+3(σ_i√M_i))≦T_i` となる最大の `W` を求めればよく、 `W=floor(T_i/(M_i*μ_i+3(σ_i√M_i)))` となる。
        /// - 最後に、念のため適当な `W_min` , `W_max` でclampしておく。
        pub struct BayesianBeamWidthSuggester {
            /// ビーム幅1あたりの所要時間の平均値の平均値μ_i（逐次更新される）
            mean_sec: f64,
            /// ビーム幅1あたりの所要時間の平均値の分散σ_i^2（逐次更新される）
            variance_sec: f64,
            /// 1ターンごとに状態に作用するノイズの大きさを表す分散α^2（定数）
            variance_state_sec: f64,
            /// 観測時に乗るノイズの大きさを表す分散β^2（定数）
            variance_observe_sec: f64,
            /// 問題の実行時間制限T
            time_limit_sec: f64,
            /// 現在のターン数i
            current_turn: usize,
            /// 最大ターン数M
            max_turn: usize,
            /// ウォームアップターン数（最初のXターン分の情報は採用せずに捨てる）
            warmup_turn: usize,
            /// 最小ビーム幅W_min
            min_beam_width: usize,
            /// 最大ビーム幅W_max
            max_beam_width: usize,
            /// 現在のビーム幅W_i
            current_beam_width: usize,
            /// ログの出力インターバル
            verbose_interval: Option<NonZeroUsize>,
            /// ビーム開始時刻
            start_time: Instant,
            /// 前回の計測時刻
            last_time: Instant,
        }

        impl BayesianBeamWidthSuggester {
            pub fn new(
                max_turn: usize,
                warmup_turn: usize,
                time_limit_sec: f64,
                standard_beam_width: usize,
                min_beam_width: usize,
                max_beam_width: usize,
                verbose_interval: Option<NonZeroUsize>,
            ) -> Self {
                assert!(
                    max_turn * standard_beam_width > 0,
                    "ターン数とビーム幅設定が不正です。"
                );
                assert!(
                    min_beam_width > 0,
                    "最小のビーム幅は正の値でなければなりません。"
                );
                assert!(
                    min_beam_width <= max_beam_width,
                    "最大のビーム幅は最小のビーム幅以上でなければなりません。"
                );

                let mean_sec = time_limit_sec / (max_turn * standard_beam_width) as f64;

                // 雑にσ=10%ズレると仮定
                let stddev_sec = 0.1 * mean_sec;
                let variance_sec = stddev_sec * stddev_sec;
                let stddev_state_sec = 0.01 * mean_sec;
                let variance_state_sec = stddev_state_sec * stddev_state_sec;
                let stddev_observe_sec = 0.05 * mean_sec;
                let variance_observe_sec = stddev_observe_sec * stddev_observe_sec;

                Self {
                    mean_sec,
                    variance_sec,
                    time_limit_sec,
                    variance_state_sec,
                    variance_observe_sec,
                    current_turn: 0,
                    min_beam_width,
                    max_beam_width,
                    verbose_interval,
                    max_turn,
                    warmup_turn,
                    current_beam_width: 0,
                    start_time: Instant::now(),
                    last_time: Instant::now(),
                }
            }

            fn update_state(&mut self) {
                // N(0, α^2)のノイズが乗る
                self.variance_sec += self.variance_state_sec;
            }

            fn update_distribution(&mut self, duration_sec: f64) {
                let old_mean = self.mean_sec;
                let old_variance = self.variance_sec;
                let noise_variance = self.variance_observe_sec;

                self.mean_sec = (old_mean * noise_variance + old_variance * duration_sec)
                    / (noise_variance + old_variance);
                self.variance_sec = old_variance * noise_variance / (old_variance + noise_variance);
            }

            fn calc_safe_beam_width(&self) -> usize {
                let remaining_turn = (self.max_turn - self.current_turn) as f64;
                let elapsed_time = (Instant::now() - self.start_time).as_secs_f64();
                let remaining_time = self.time_limit_sec - elapsed_time;

                // 平均値の分散σ^2と観測ノイズβ^2が乗ってくると考える
                let variance_total = self.variance_sec + self.variance_observe_sec;

                // N(ξ, η^2)からのサンプリングをK回繰り返すとN(Kξ, Kη^2)となる（はず）
                let mean = remaining_turn * self.mean_sec;
                let variance = remaining_turn * variance_total;
                let stddev = variance.sqrt();

                // 3σの余裕を持たせる
                const SIGMA_COEF: f64 = 3.0;
                let needed_time_per_width = mean + SIGMA_COEF * stddev;
                let beam_width = ((remaining_time / needed_time_per_width) as usize)
                    .max(self.min_beam_width)
                    .min(self.max_beam_width);

                if let Some(verbose_interval) = self.verbose_interval {
                    if self.current_turn % verbose_interval.get() == 0 {
                        let stddev_per_run = (self.max_turn as f64 * variance_total).sqrt();
                        let stddev_per_turn = variance_total.sqrt();

                        eprintln!(
                    "turn: {:4}, beam width: {:4}, pase: {:.3}±{:.3}ms/run, iter time: {:.3}±{:.3}ms",
                    self.current_turn,
                    beam_width,
                    self.mean_sec * (beam_width * self.max_turn) as f64 * 1e3,
                    stddev_per_run * beam_width as f64 * 1e3,
                    self.mean_sec * beam_width as f64 * 1e3,
                    stddev_per_turn * beam_width as f64 * 1e3
                );
                    }
                }

                beam_width
            }
        }

        impl BeamWidthSuggester for BayesianBeamWidthSuggester {
            fn suggest(&mut self) -> usize {
                assert!(
                    self.current_turn < self.max_turn,
                    "規定ターン終了後にsuggest()が呼び出されました。"
                );

                if self.current_turn >= self.warmup_turn {
                    let elapsed = (Instant::now() - self.last_time).as_secs_f64();
                    let elapsed_per_beam = elapsed / self.current_beam_width as f64;
                    self.update_state();
                    self.update_distribution(elapsed_per_beam);
                }

                self.last_time = Instant::now();
                let beam_width = self.calc_safe_beam_width();
                self.current_beam_width = beam_width;
                self.current_turn += 1;
                beam_width
            }

            fn max_width(&self) -> usize {
                self.max_beam_width
            }
        }

        /// Open Addressing / Linear Probing型の連想配列
        ///
        /// # Type Parameters
        /// - `K`: キーの型
        /// - `V`: 値の型
        /// - `N`: 内部的に使う連想配列のサイズ。格納する要素数（>ビーム幅）の16倍程度の素数を推奨
        pub(super) struct OpenHashMap<K, V, const N: usize> {
            data: Vec<Option<(K, V)>>,
        }

        impl<K: Copy + Eq + Into<u64>, V: Clone, const N: usize> OpenHashMap<K, V, N> {
            pub(super) fn new() -> Self {
                Self {
                    data: vec![None; N],
                }
            }

            pub(super) fn get_index(&self, key: K) -> Entry {
                let i: u64 = key.into();
                let mut i = (i as u32 % N as u32) as usize;

                unsafe {
                    while let Some((k, _)) = self.data.get_unchecked(i) {
                        if *k == key {
                            return Entry::Occupied(HashMapIndex(i));
                        }

                        i += 1;

                        if i == N {
                            i = 0;
                        }
                    }
                }

                return Entry::Vacant(HashMapIndex(i));
            }

            pub(super) fn set(&mut self, index: HashMapIndex, key: K, value: V) {
                self.data[index.0] = Some((key, value));
            }

            pub(super) fn get(&self, index: HashMapIndex) -> &V {
                self.data[index.0]
                    .as_ref()
                    .map(|(_, v)| v)
                    .expect("There is no value.")
            }

            pub(super) fn clear(&mut self) {
                self.data.fill(None);
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        pub(super) struct HashMapIndex(pub(super) usize);

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        pub(super) enum Entry {
            Occupied(HashMapIndex),
            Vacant(HashMapIndex),
        }

        impl Entry {
            pub(super) fn index(&self) -> HashMapIndex {
                match self {
                    Entry::Occupied(index) => *index,
                    Entry::Vacant(index) => *index,
                }
            }
        }

        pub(super) struct MaxCostIndex<T>(Infallible, PhantomData<fn() -> T>);

        impl<T> Monoid for MaxCostIndex<T>
        where
            T: Copy + Ord + LowerBounded + Default,
        {
            type S = (T, usize);

            fn identity() -> Self::S {
                (T::min_value(), !0)
            }

            fn binary_operation(a: &Self::S, b: &Self::S) -> Self::S {
                if a.0 >= b.0 {
                    *a
                } else {
                    *b
                }
            }
        }

        #[derive(Debug)]
        pub enum BeamError {
            NoCandidates(NoCandidatesError),
        }

        #[derive(Debug)]
        pub struct NoCandidatesError {
            turn: usize,
        }

        impl NoCandidatesError {
            pub(super) fn new(turn: usize) -> Self {
                Self { turn }
            }
        }

        impl Display for BeamError {
            fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
                match self {
                    BeamError::NoCandidates(err) => {
                        write!(f, "[turn {}] No candidates found.", err.turn)
                    }
                }
            }
        }

        impl Error for BeamError {}
    }
    pub mod single_turn {
        //! オイラーツアーの辺を保持するビームサーチ
        //!
        //! 1ターン先にしか遷移できない代わりに高速
        use super::common::{
            BeamError, BeamWidthSuggester, Entry, MaxCostIndex, NoCandidatesError, OpenHashMap,
        };
        use ac_library::Segtree;
        use num_traits::bounds::LowerBounded;
        use std::{
            fmt::{Debug, Display},
            marker::PhantomData,
            time::Instant,
            usize,
        };

        /// 状態遷移を行うために必要な情報
        /// メモリ使用量をできるだけ小さくしてください
        pub trait Action: Clone + Eq {
            type State: State<Action = Self>;

            fn apply(&self, state: &mut Self::State);

            fn rollback(&self, state: &mut Self::State);
        }

        /// 状態のコストを評価するための構造体
        /// メモリ使用量をできるだけ小さくしてください
        pub trait Evaluator: Clone {
            type Cost: Copy + Ord + LowerBounded + Default + Display;

            fn evaluate(&self) -> Self::Cost;
        }

        pub trait BeamHash: Debug + Copy + Eq + Into<u64> {}
        impl BeamHash for u8 {}
        impl BeamHash for u16 {}
        impl BeamHash for u32 {}
        impl BeamHash for u64 {}

        pub trait State {
            type Evaluator: Evaluator;
            type Hash: BeamHash;
            type Action: Action<State = Self>;

            fn make_initial_node(&self) -> (Self::Evaluator, Self::Hash);

            fn expand(
                &mut self,
                evaluator: &Self::Evaluator,
                hash: Self::Hash,
                candidate_set: &mut impl CandidateSet<
                    Action = Self::Action,
                    Evaluator = Self::Evaluator,
                    Hash = Self::Hash,
                >,
            );
        }

        #[derive(Clone)]
        pub struct Candidate<A: Action, E: Evaluator, H: BeamHash> {
            action: A,
            evaluator: E,
            hash: H,
            parent_id: ParentId,
        }

        impl<A: Action, E: Evaluator, H: BeamHash> Candidate<A, E, H> {
            #[inline]
            fn new(action: A, evaluator: E, hash: H, parent_id: ParentId) -> Self {
                Self {
                    action,
                    evaluator,
                    hash,
                    parent_id,
                }
            }

            pub fn action(&self) -> &A {
                &self.action
            }

            pub fn evaluator(&self) -> &E {
                &self.evaluator
            }

            pub fn hash(&self) -> &H {
                &self.hash
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        struct ParentId(u32);

        impl ParentId {
            fn index(&self) -> usize {
                self.0 as usize
            }
        }

        pub trait CandidateSet {
            type Action: Action;
            type Evaluator: Evaluator;
            type Hash: BeamHash;

            fn push(
                &mut self,
                action: Self::Action,
                evaluator: Self::Evaluator,
                hash: Self::Hash,
                is_finished: bool,
            );
        }

        struct CandidateSetImpl<'a, A: Action, E: Evaluator, H: BeamHash, const N: usize> {
            selector: &'a mut NodeSelector<A, E, H, N>,
            parent_id: ParentId,
        }

        impl<'a, A: Action, E: Evaluator, H: BeamHash, const N: usize> CandidateSetImpl<'a, A, E, H, N> {
            fn new(selector: &'a mut NodeSelector<A, E, H, N>, parent_id: ParentId) -> Self {
                Self {
                    selector,
                    parent_id,
                }
            }
        }

        impl<'a, A: Action, E: Evaluator, H: BeamHash, const N: usize> CandidateSet
            for CandidateSetImpl<'a, A, E, H, N>
        {
            type Action = A;
            type Evaluator = E;
            type Hash = H;

            #[inline]
            fn push(
                &mut self,
                action: Self::Action,
                evaluator: Self::Evaluator,
                hash: Self::Hash,
                is_finished: bool,
            ) {
                self.selector.push(
                    Candidate::new(action, evaluator, hash, self.parent_id),
                    is_finished,
                );
            }
        }

        /// ノードの候補から実際に追加するものを選ぶ構造体
        /// ビーム幅の個数だけ、評価がよいものを選ぶ
        /// ハッシュ値が一致したものについては、評価がよいほうのみを残す
        struct NodeSelector<A: Action, E: Evaluator, H: BeamHash, const N: usize> {
            beam_width: usize,
            candidates: Vec<Candidate<A, E, H>>,
            hash_to_index: OpenHashMap<H, usize, N>,
            costs: Vec<(E::Cost, usize)>,
            /// セグメント木を削除可能な優先度付きキューとして使う
            cost_segtree: Option<Segtree<MaxCostIndex<E::Cost>>>,
            finished_candidates: Vec<Candidate<A, E, H>>,
        }

        impl<A: Action, E: Evaluator, H: BeamHash, const N: usize> NodeSelector<A, E, H, N> {
            fn new(max_beam_width: usize) -> Self {
                let candidates = Vec::with_capacity(max_beam_width);
                let costs = Vec::with_capacity(max_beam_width);

                Self {
                    beam_width: max_beam_width,
                    candidates,
                    hash_to_index: OpenHashMap::new(),
                    costs,
                    cost_segtree: None,
                    finished_candidates: Vec::new(),
                }
            }

            fn push(&mut self, candidate: Candidate<A, E, H>, is_finished: bool) {
                if is_finished {
                    self.finished_candidates.push(candidate);
                    return;
                }

                let cost = candidate.evaluator.evaluate();

                // 保持しているどの候補よりもコストが小さくないとき
                if let Some(segtree) = &self.cost_segtree {
                    if cost >= segtree.all_prod().0 {
                        return;
                    }
                }

                // ハッシュ値が等しいものが存在している場合
                let hash_entry = self.hash_to_index.get_index(candidate.hash);

                if let Entry::Occupied(index_h) = hash_entry {
                    let index_c = *self.hash_to_index.get(index_h);
                    let old_cand = &self.candidates[index_c];

                    // 同じハッシュの状態は1つしか保持しないため、その場合は上書き処理となる
                    if candidate.hash == old_cand.hash {
                        // セグ木が構築されているかどうかで場合分け
                        match &mut self.cost_segtree {
                            Some(segtree) => {
                                if cost < segtree.get(index_c).0 {
                                    self.candidates[index_c] = candidate;
                                    segtree.set(index_c, (cost, index_c));
                                }
                            }
                            None => {
                                if cost < self.costs[index_c].0 {
                                    self.candidates[index_c] = candidate;
                                    self.costs[index_c] = (cost, index_c);
                                }
                            }
                        }

                        return;
                    }
                }

                // ハッシュ値が等しいものが存在しない場合
                let index_h = hash_entry.index();

                // セグ木が構築されているかで場合分け
                match &mut self.cost_segtree {
                    Some(segtree) => {
                        let index_c = segtree.all_prod().1;
                        self.hash_to_index.set(index_h, candidate.hash, index_c);
                        self.candidates[index_c] = candidate;
                        segtree.set(index_c, (cost, index_c));
                    }
                    None => {
                        let index_c = self.candidates.len();
                        self.hash_to_index.set(index_h, candidate.hash, index_c);
                        self.candidates.push(candidate);
                        self.costs.push((cost, index_c));

                        // 保持している候補がビーム幅分になったときにセグ木を構築する
                        if self.candidates.len() >= self.beam_width {
                            // ビーム幅の変動に備え、少し多めに確保
                            let mut costs = Vec::with_capacity(self.beam_width * 12 / 10);
                            std::mem::swap(&mut self.costs, &mut costs);
                            let segtree = Segtree::from(costs);
                            self.cost_segtree = Some(segtree);
                        }
                    }
                }
            }

            fn iter_candidates_and_clear<'a>(
                &'a mut self,
            ) -> impl Iterator<Item = Candidate<A, E, H>> + 'a {
                self.costs.clear();
                self.hash_to_index.clear();
                self.cost_segtree = None;

                self.candidates.drain(..)
            }

            fn calculate_best_candidate(&self) -> Option<&Candidate<A, E, H>> {
                match &self.cost_segtree {
                    Some(segtree) => {
                        let mut best_index = 0;
                        let (mut best_cost, _) = segtree.get(0);

                        for i in 1..self.beam_width {
                            let cost = segtree.get(i).0;

                            if cost < best_cost {
                                best_cost = cost;
                                best_index = i;
                            }
                        }

                        Some(&self.candidates[best_index])
                    }
                    None => {
                        if self.candidates.len() == 0 {
                            return None;
                        }

                        let mut best_index = 0;
                        let (mut best_cost, _) = self.costs[0];

                        for i in 1..self.costs.len() {
                            let cost = self.costs[i].0;

                            if cost < best_cost {
                                best_cost = cost;
                                best_index = i;
                            }
                        }

                        Some(&self.candidates[best_index])
                    }
                }
            }

            pub(super) fn set_beam_width(&mut self, beam_width: usize) {
                self.beam_width = beam_width;
            }
        }

        struct BeamTree<E: Evaluator, H: BeamHash, S: State, A: Action, const N: usize> {
            state: S,
            current_tour: Vec<(TourStep, A)>,
            next_tour: Vec<(TourStep, A)>,
            leaves: Vec<(E, H)>,
            buckets: Vec<Vec<(A, E, H)>>,
            direct_road: Vec<A>,
        }

        impl<
                E: Evaluator,
                H: BeamHash,
                S: State<Evaluator = E, Hash = H, Action = A>,
                A: Action<State = S>,
                const N: usize,
            > BeamTree<E, H, S, A, N>
        {
            fn new(state: S, max_beam_width: usize) -> Self {
                Self {
                    state,
                    current_tour: vec![],
                    next_tour: vec![],
                    leaves: vec![],
                    buckets: vec![vec![]; max_beam_width],
                    direct_road: vec![],
                }
            }

            /// 状態を更新しながら深さ優先探索を行い、次のノードの候補を全てselectorに追加する
            fn dfs(&mut self, selector: &mut NodeSelector<A, E, H, N>) {
                if self.current_tour.is_empty() {
                    // 最初のターン
                    let (evaluator, hash) = self.state.make_initial_node();
                    let mut cand_set = CandidateSetImpl::new(selector, ParentId(0));
                    self.state.expand(&evaluator, hash, &mut cand_set);
                    return;
                }

                for (tour_step, action) in self.current_tour.iter() {
                    match *tour_step {
                        TourStep::FORWARD_EDGE => {
                            action.apply(&mut self.state);
                        }
                        TourStep::BACKWARD_EDGE => {
                            action.rollback(&mut self.state);
                        }
                        TourStep(leaf_index) => {
                            let leaf_index = leaf_index as usize;
                            action.apply(&mut self.state);
                            let (evaluator, hash) = &self.leaves[leaf_index];
                            let mut cand_set =
                                CandidateSetImpl::new(selector, ParentId(leaf_index as u32));
                            self.state.expand(evaluator, *hash, &mut cand_set);
                            action.rollback(&mut self.state);
                        }
                    }
                }
            }

            fn update(&mut self, candidates: impl Iterator<Item = Candidate<A, E, H>>) {
                self.leaves.clear();

                if self.current_tour.is_empty() {
                    // 最初のターン
                    for Candidate {
                        action,
                        evaluator,
                        hash,
                        ..
                    } in candidates
                    {
                        let step = TourStep::new_leaf(self.leaves.len() as u32);
                        self.current_tour.push((step, action));
                        self.leaves.push((evaluator, hash));
                    }

                    return;
                }

                for Candidate {
                    action,
                    evaluator,
                    hash,
                    parent_id,
                } in candidates
                {
                    self.buckets[parent_id.index()].push((action, evaluator, hash));
                }

                let mut tour_index = 0;
                let tour = &mut self.current_tour;

                // 一本道を反復しないようにする
                while tour[tour_index].0 == TourStep::FORWARD_EDGE
                    && tour[tour_index].1 == tour.last().unwrap().1
                {
                    let action = tour[tour_index].1.clone();
                    tour_index += 1;
                    action.apply(&mut self.state);
                    self.direct_road.push(action);
                    tour.pop();
                }

                // 葉の追加や不要な辺の削除をする
                for (tour_step, action) in tour.drain(tour_index..) {
                    match tour_step {
                        TourStep::FORWARD_EDGE => {
                            self.next_tour.push((TourStep::FORWARD_EDGE, action));
                        }
                        TourStep::BACKWARD_EDGE => {
                            let (prev_step, _) = self.next_tour.last().unwrap();

                            match *prev_step {
                                TourStep::FORWARD_EDGE => {
                                    // 行ってすぐ戻る場合は削除
                                    self.next_tour.pop();
                                }
                                _ => {
                                    self.next_tour.push((TourStep::BACKWARD_EDGE, action));
                                }
                            }
                        }
                        TourStep(leaf_index) => {
                            let leaf_index = leaf_index as usize;
                            let bucket = &mut self.buckets[leaf_index];

                            if bucket.is_empty() {
                                continue;
                            }

                            self.next_tour
                                .push((TourStep::FORWARD_EDGE, action.clone()));

                            for (action, evaluator, hash) in bucket.drain(..) {
                                let leaf_index = self.leaves.len() as u32;
                                self.next_tour
                                    .push((TourStep::new_leaf(leaf_index), action));
                                self.leaves.push((evaluator, hash));
                            }

                            self.next_tour.push((TourStep::BACKWARD_EDGE, action));
                        }
                    }
                }

                std::mem::swap(&mut self.current_tour, &mut self.next_tour);
                self.next_tour.clear();
            }

            fn restore_path(&self, parent_id: ParentId) -> Vec<A> {
                let mut result = self.direct_road.clone();

                for (tour_step, action) in self.current_tour.iter() {
                    match *tour_step {
                        TourStep::FORWARD_EDGE => {
                            result.push(action.clone());
                        }
                        TourStep::BACKWARD_EDGE => {
                            result.pop();
                        }
                        TourStep(leaf_index) => {
                            if leaf_index == parent_id.0 {
                                result.push(action.clone());
                                return result;
                            }
                        }
                    }
                }

                unreachable!("There is no path to the parent node.");
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        struct TourStep(u32);

        impl TourStep {
            const FORWARD_EDGE: Self = Self(u32::MAX);
            const BACKWARD_EDGE: Self = Self(u32::MAX - 1);

            const fn new_leaf(leaf_index: u32) -> Self {
                Self(leaf_index)
            }
        }

        /// ビームサーチを行う構造体
        ///
        /// 使用には以下の実装が必要になる。
        ///
        /// - `State`: 状態遷移を行うための構造体
        /// - `Evaluator`: 状態のコストを評価するための構造体
        /// - `BeamHash`: ビームサーチのためのハッシュ値を計算するための構造体
        /// - `Action`: 状態遷移を行うためのアクションを表す構造体
        /// - `BeamWidthSuggester`: ビーム幅を提案するための構造体（Optional）
        ///
        /// # Type Parameters
        ///
        /// - `N`: 内部的に使う連想配列のサイズ。格納する要素数（>ビーム幅）の16倍程度の素数を推奨
        pub struct BeamSearch<W: BeamWidthSuggester, const N: usize> {
            beam_width_suggester: W,
            max_turn: usize,
        }

        impl<W: BeamWidthSuggester, const N: usize> BeamSearch<W, N> {
            pub fn new(beam_width_suggester: W, max_turn: usize) -> Self {
                assert!(max_turn > 0, "Max turn must be greater than 0.");

                Self {
                    beam_width_suggester,
                    max_turn,
                }
            }

            /// デフォルトの標準エラー出力用コールバックを使ってビームサーチを実行する
            pub fn run_default<S: State>(
                self,
                state: S,
            ) -> Result<(Vec<S::Action>, S::Evaluator), BeamError> {
                let callbacks: Vec<Box<dyn BeamCallback<State = S>>> =
                    vec![Box::new(DefaultBeamCallback::<S>::new())];
                self.run(state, callbacks)
            }

            /// 任意のコールバックを使ってビームサーチを実行する
            ///
            /// コールバックをカスタムする必要がないときは `run_default` を使うことを推奨
            pub fn run<'a, S: State>(
                mut self,
                state: S,
                mut callbacks: Vec<Box<dyn BeamCallback<State = S> + 'a>>,
            ) -> Result<(Vec<S::Action>, S::Evaluator), BeamError> {
                let mut tree =
                    BeamTree::<_, _, _, _, N>::new(state, self.beam_width_suggester.max_width());
                let mut selector = NodeSelector::new(self.beam_width_suggester.max_width());

                for turn in 0..self.max_turn {
                    let beam_width = self.beam_width_suggester.suggest();

                    for callback in &mut callbacks {
                        callback.on_turn_start(turn, beam_width);
                    }

                    selector.set_beam_width(beam_width);

                    // Euler Tourでselectorに候補を追加する
                    tree.dfs(&mut selector);

                    let best_candidate = selector.calculate_best_candidate();

                    for callback in &mut callbacks {
                        callback.on_expanded(turn, selector.candidates.as_slice(), best_candidate);
                    }

                    // ターン数最小化型の問題で実行可能解が見つかったとき
                    if let Some(cand) = selector
                        .finished_candidates
                        .iter()
                        .min_by_key(|c| c.evaluator.evaluate())
                    {
                        let mut actions = tree.restore_path(cand.parent_id);
                        actions.push(cand.action.clone());
                        return Ok((actions, cand.evaluator.clone()));
                    }

                    if selector.candidates.is_empty() {
                        return Err(BeamError::NoCandidates(NoCandidatesError::new(turn)));
                    }

                    // ターン数固定型の問題で全ターンが終了したとき
                    if turn == self.max_turn - 1 {
                        break;
                    }

                    // 木を更新する
                    tree.update(selector.iter_candidates_and_clear());

                    for callback in &mut callbacks {
                        callback.on_turn_end(turn);
                    }
                }

                let Some(best_candidate) = selector.calculate_best_candidate() else {
                    return Err(BeamError::NoCandidates(NoCandidatesError::new(
                        self.max_turn - 1,
                    )));
                };
                let mut actions = tree.restore_path(best_candidate.parent_id);
                actions.push(best_candidate.action.clone());
                Ok((actions, best_candidate.evaluator.clone()))
            }
        }

        pub trait BeamCallback {
            type State: State;

            fn on_turn_start(&mut self, turn: usize, beam_width: usize);

            fn on_expanded(
                &mut self,
                turn: usize,
                canidates: &[Candidate<
                    <Self::State as State>::Action,
                    <Self::State as State>::Evaluator,
                    <Self::State as State>::Hash,
                >],
                best_candidate: Option<
                    &Candidate<
                        <Self::State as State>::Action,
                        <Self::State as State>::Evaluator,
                        <Self::State as State>::Hash,
                    >,
                >,
            );

            fn on_turn_end(&mut self, turn: usize);
        }

        /// デフォルトの標準エラー出力用コールバック
        ///
        /// ターン数・ビーム幅・最良候補の評価値・実行時間を出力する
        pub struct DefaultBeamCallback<S: State> {
            since: Instant,
            since_turn: Instant,
            phantom: PhantomData<S>,
        }

        impl<S: State> DefaultBeamCallback<S> {
            fn new() -> Self {
                Self {
                    since: Instant::now(),
                    since_turn: Instant::now(),
                    phantom: PhantomData,
                }
            }
        }

        impl<S: State> BeamCallback for DefaultBeamCallback<S> {
            type State = S;

            fn on_turn_start(&mut self, turn: usize, beam_width: usize) {
                self.since_turn = Instant::now();
                eprintln!("[Turn {}]", turn);
                eprintln!("beam width = {}", beam_width);
            }

            fn on_expanded(
                &mut self,
                _turn: usize,
                _canidates: &[Candidate<
                    <Self::State as State>::Action,
                    <Self::State as State>::Evaluator,
                    <Self::State as State>::Hash,
                >],
                best_candidate: Option<
                    &Candidate<
                        <Self::State as State>::Action,
                        <Self::State as State>::Evaluator,
                        <Self::State as State>::Hash,
                    >,
                >,
            ) {
                let Some(candidate) = best_candidate else {
                    return;
                };

                let best_score = candidate.evaluator.evaluate();
                eprintln!("best score = {}", best_score);
            }

            fn on_turn_end(&mut self, _turn: usize) {
                eprintln!("elapsed = {:?}", self.since_turn.elapsed());
                eprintln!("total elapsed = {:?}", self.since.elapsed());
                eprintln!();
            }
        }
    }
}
mod geo {
    use nalgebra::Vector3;
    use std::arch::x86_64::{
        _mm256_andnot_pd, _mm256_fmadd_pd, _mm256_fmsub_pd, _mm256_mul_pd, _mm256_set1_pd,
        _mm256_setr_pd, _mm256_storeu_pd, _mm256_sub_pd,
    };
    use std::cmp::Ordering;
    use std::num::NonZeroU16;

    /// 四面体の体積 = (1/6) * |(AB × AC) ⋅ AD|
    pub fn tetrahedron_volume(
        a: &Vector3<f64>,
        b: &Vector3<f64>,
        c: &Vector3<f64>,
        d: &Vector3<f64>,
    ) -> f64 {
        ((b - a).cross(&(c - a))).dot(&(d - a)).abs() / 6.0
    }

    pub fn sub_tetrahedron_volume(
        e: &Vector3<f64>,
        f: &Vector3<f64>,
        g: &Vector3<f64>,
        h: &Vector3<f64>,
        p: &Vector3<f64>,
    ) -> [f64; 4] {
        unsafe { sub_tetrahedron_volume_inner(e, f, g, h, p) }
    }

    #[target_feature(enable = "avx,avx2,fma")]
    unsafe fn sub_tetrahedron_volume_inner(
        e: &Vector3<f64>,
        f: &Vector3<f64>,
        g: &Vector3<f64>,
        h: &Vector3<f64>,
        p: &Vector3<f64>,
    ) -> [f64; 4] {
        // 四面体PBCD, APCD, ABPD, ABCPの体積をまとめて計算する

        unsafe {
            // b - a
            let ax = _mm256_setr_pd(p[0], e[0], e[0], e[0]);
            let ay = _mm256_setr_pd(p[1], e[1], e[1], e[1]);
            let az = _mm256_setr_pd(p[2], e[2], e[2], e[2]);
            let bx = _mm256_setr_pd(f[0], p[0], f[0], f[0]);
            let by = _mm256_setr_pd(f[1], p[1], f[1], f[1]);
            let bz = _mm256_setr_pd(f[2], p[2], f[2], f[2]);
            let cx = _mm256_setr_pd(g[0], g[0], p[0], g[0]);
            let cy = _mm256_setr_pd(g[1], g[1], p[1], g[1]);
            let cz = _mm256_setr_pd(g[2], g[2], p[2], g[2]);
            let dx = _mm256_setr_pd(h[0], h[0], h[0], p[0]);
            let dy = _mm256_setr_pd(h[1], h[1], h[1], p[1]);
            let dz = _mm256_setr_pd(h[2], h[2], h[2], p[2]);

            // AB, AC, AD
            let abx = _mm256_sub_pd(bx, ax);
            let aby = _mm256_sub_pd(by, ay);
            let abz = _mm256_sub_pd(bz, az);
            let acx = _mm256_sub_pd(cx, ax);
            let acy = _mm256_sub_pd(cy, ay);
            let acz = _mm256_sub_pd(cz, az);
            let adx = _mm256_sub_pd(dx, ax);
            let ady = _mm256_sub_pd(dy, ay);
            let adz = _mm256_sub_pd(dz, az);

            // AB x AC
            let ab_cross_ac_x = _mm256_fmsub_pd(aby, acz, _mm256_mul_pd(abz, acy));
            let ab_cross_ac_y = _mm256_fmsub_pd(abz, acx, _mm256_mul_pd(abx, acz));
            let ab_cross_ac_z = _mm256_fmsub_pd(abx, acy, _mm256_mul_pd(aby, acx));

            // (AB x AC) ⋅ AD
            let dot = _mm256_fmadd_pd(
                ab_cross_ac_x,
                adx,
                _mm256_fmadd_pd(ab_cross_ac_y, ady, _mm256_mul_pd(ab_cross_ac_z, adz)),
            );

            // 体積
            let abs = _mm256_andnot_pd(
                _mm256_set1_pd(-0.0),
                dot, // 符号ビットを除去して絶対値を取得
            );
            let div = _mm256_set1_pd(1.0 / 6.0);
            let volume = _mm256_mul_pd(abs, div);

            let mut out: [f64; 4] = [0.0; 4];
            _mm256_storeu_pd(out.as_mut_ptr(), volume);

            out
        }
    }

    #[derive(Clone, Debug)]
    struct Face {
        a: usize,
        b: usize,
        c: usize,
        q: Vector3<f64>,
    }

    /// 3D 凸包を構築する関数。O(N^2) で動作。
    /// see: https://codeforces.com/blog/entry/81768
    ///
    /// - `points`：3D 空間の点のベクター（Vec<Vector3<f64>>）
    /// - 戻り値：凸包の面のベクター（Vec<Face>）
    pub fn convex_hull(points: &[Vector3<f64>]) -> Vec<usize> {
        let n = points.len();
        assert!(n >= 3, "points.len() must be at least 3");
        const EPS: f64 = 1e-9;

        let mut faces: Vec<Face> = Vec::new();
        let mut dead = vec![vec![true; n]; n];

        // 面を追加するクロージャ
        let add_face =
            |a: usize, b: usize, c: usize, faces: &mut Vec<Face>, dead: &mut Vec<Vec<bool>>| {
                let ab = points[b] - points[a];
                let ac = points[c] - points[a];
                let q = ab.cross(&ac);
                faces.push(Face { a, b, c, q });
                dead[a][b] = false;
                dead[b][c] = false;
                dead[c][a] = false;
            };

        // 最初の三角板（二面）を張る
        add_face(0, 1, 2, &mut faces, &mut dead);
        add_face(0, 2, 1, &mut faces, &mut dead);

        for i in 3..n {
            // 新点から見えない面だけを残す
            let mut new_faces = Vec::new();
            for face in &faces {
                let pa = &points[face.a];
                let vi = points[i] - pa;
                if vi.dot(&face.q) <= EPS {
                    new_faces.push(face.clone());
                } else {
                    dead[face.a][face.b] = true;
                    dead[face.b][face.c] = true;
                    dead[face.c][face.a] = true;
                }
            }

            // exposed edge に新しい面を貼る
            faces.clear();
            for face in &new_faces {
                let verts = [face.a, face.b, face.c];
                for j in 0..3 {
                    let a = verts[j];
                    let b = verts[(j + 1) % 3];
                    if dead[b][a] {
                        add_face(b, a, i, &mut faces, &mut dead);
                    }
                }
            }

            // 不可視面を戻す
            faces.extend(new_faces);
        }

        // 頂点を返す
        let mut indices = vec![];

        for face in &faces {
            indices.push(face.a);
            indices.push(face.b);
            indices.push(face.c);
        }

        indices.sort_unstable();
        indices.dedup();

        indices
    }

    /// 3D KD-tree node（メモリ節約版）  
    /// - `point_idx`：点集合 Vec<Vector3<f64>> 中のインデックス (0～9999 を想定)。  
    /// - `axis`：分割する軸 (0=x, 1=y, 2=z)。  
    /// - `left` / `right`：子ノードの「0ベースインデックス＋1」を NonZeroU16 で保持。None は子なし。  
    ///
    /// レイアウト（バイトオフセット）例:  
    /// ```text
    /// offset 0: point_idx: u16         (2 bytes)
    /// offset 2: axis: u8               (1 byte)
    /// offset 3: (パディング 1 byte)
    /// offset 4: left: Option<NonZeroU16>  (2 bytes)
    /// offset 6: right: Option<NonZeroU16> (2 bytes)
    /// -------- total: 8 bytes --------
    /// ```
    ///
    #[derive(Clone, Copy)]
    struct Node {
        point_idx: u16,            // Vec<Vector3> 内のインデックス (0-based)
        axis: u8,                  // 0=x,1=y,2=z
        left: Option<NonZeroU16>,  // 子ノードの (index + 1)。None は子なし
        right: Option<NonZeroU16>, // 同上
    }

    /// KDTree 全体を保持する構造体
    /// - `points`：検索対象の点群（Vec<Vector3<f64>>、最大 10000 個までを想定）。  
    /// - `nodes`：`Node` 配列。プリオーダー（pre-order）で詰める。  
    /// - `root`：根ノードの 0ベースインデックス（ツリーが空なら None）。
    ///
    /// 構築: O(N · (log N)²)  (N ≲ 10000 なら十分高速)  
    /// 単一点最近傍クエリ: 平均 O(log N)
    /// 上位 K 件取得クエリ: 平均 O(log N · log K)
    #[derive(Clone)]
    pub struct KDTree {
        points: Vec<Vector3<f64>>,
        nodes: Vec<Node>,
    }

    impl KDTree {
        /// Vec<Vector3<f64>> を受け取り、KDTree を構築する。
        /// - 入力: `points`（長さ N）、N ≤ 10000 と仮定  
        /// - 戻り値: KDTree { points, nodes, root }  
        ///
        /// 内部で indices = [0,1,2,…,N-1] を作り、再帰的に median-of-slice によって pre-order で Node を詰めていく。
        /// 各 Node は 8 バイトに圧縮されているため、高速・省メモリ。
        pub fn new(points: Vec<Vector3<f64>>) -> Self {
            assert!(points.len() > 0, "KDTree requires at least one point");

            let n = points.len();
            // nodes はプリオーダーで Node を push するため、最終的に nodes.len() == n になる
            let mut nodes = Vec::with_capacity(n);

            // 0..N-1 のインデックス配列を生成し、再帰でこのスライスをソートしつつ分割する
            let mut indices: Vec<usize> = (0..n).collect();

            /// 再帰的に KD-tree を構築する関数  
            /// - `points`: 元の点集合への参照  
            /// - `nodes`: Node 配列への可変参照  
            /// - `indices`: 現在処理すべき点の「0-based インデックス」を並べたスライス  
            /// - `depth`: 再帰深度 (軸 = depth % 3)  
            ///
            /// 戻り値: この部分木の「自ノード 0-based インデックス」を u16 で返す  
            fn build_recursive(
                points: &Vec<Vector3<f64>>,
                nodes: &mut Vec<Node>,
                indices: &mut [usize],
                depth: usize,
            ) -> Option<NonZeroU16> {
                if indices.is_empty() {
                    return None;
                }

                // 分割軸を決める (0=x,1=y,2=z)
                let axis = (depth % 3) as u8;
                let len = indices.len();
                let mid = len / 2;

                // スライス indices 全体を、points[indices[*]].axis 成分でソートする
                match axis {
                    0 => indices.sort_unstable_by(|&a, &b| {
                        points[a]
                            .x
                            .partial_cmp(&points[b].x)
                            .unwrap_or(Ordering::Equal)
                    }),
                    1 => indices.sort_unstable_by(|&a, &b| {
                        points[a]
                            .y
                            .partial_cmp(&points[b].y)
                            .unwrap_or(Ordering::Equal)
                    }),
                    _ => indices.sort_unstable_by(|&a, &b| {
                        points[a]
                            .z
                            .partial_cmp(&points[b].z)
                            .unwrap_or(Ordering::Equal)
                    }),
                }

                // 「中央値インデックス」を取り出す
                let median_point_idx = indices[mid];

                // このノードの 0-based インデックス = nodes.len() (u16 にキャスト可能と仮定)
                let my_idx = nodes.len() as u16;

                // ダミーを先に push しておき、子ノードを構築したあとで上書きする
                nodes.push(Node {
                    point_idx: 0,
                    axis: 0,
                    left: None,
                    right: None,
                });

                // 左部分木: indices[0..mid]
                let left_child = {
                    let (left_slice, _) = indices.split_at_mut(mid);
                    build_recursive(points, nodes, left_slice, depth + 1)
                };

                // 右部分木: indices[mid+1..]
                let right_child = {
                    let (_, right_slice) = indices.split_at_mut(mid + 1);
                    build_recursive(points, nodes, right_slice, depth + 1)
                };

                // ダミーを上書きして、このノードに正しい値をセット
                nodes[my_idx as usize] = Node {
                    point_idx: median_point_idx as u16,
                    axis,
                    left: left_child,
                    right: right_child,
                };

                NonZeroU16::new(my_idx)
            }

            // 木を再帰的に構築
            build_recursive(&points, &mut nodes, &mut indices[..], 0);

            KDTree { points, nodes }
        }

        /// 単一点最近傍 (1-NN) を返す  
        /// - `query`：探索したい点  
        /// - 戻り値：`points` 内のインデックス (usize)。
        pub fn nearest(&self, query: &Vector3<f64>) -> usize {
            let root_idx = 0;

            // best = (best_dist_sq, best_point_idx_u16)
            let mut best: (f64, u16) = (f64::INFINITY, 0);

            /// 再帰的探索関数  
            fn search_rec(
                tree: &KDTree,
                node_idx: usize,
                query: &Vector3<f64>,
                best: &mut (f64, u16),
            ) {
                let node = &tree.nodes[node_idx];
                let pt_idx_u16 = node.point_idx;
                let pt_idx = pt_idx_u16 as usize;
                let pt = &tree.points[pt_idx];

                // このノードの点との距離²を計算し、best を更新
                let diff = pt - query;
                let dist_sq = diff.norm_squared();
                if dist_sq < best.0 {
                    best.0 = dist_sq;
                    best.1 = pt_idx_u16;
                }

                // 分割軸に沿った差分
                let diff_axis = match node.axis {
                    0 => query.x - pt.x,
                    1 => query.y - pt.y,
                    _ => query.z - pt.z,
                };

                // 近い側を先に探索
                let (first_opt, second_opt) = if diff_axis <= 0.0 {
                    (node.left, node.right)
                } else {
                    (node.right, node.left)
                };

                // 1) 近い側
                if let Some(nz) = first_opt {
                    let child_idx = nz.get() as usize;
                    search_rec(tree, child_idx, query, best);
                }

                // 2) 分割平面を越えて探索が必要か判定
                if diff_axis * diff_axis < best.0 {
                    if let Some(nz) = second_opt {
                        let child_idx = nz.get() as usize;
                        search_rec(tree, child_idx, query, best);
                    }
                }
            }

            // 探索開始
            search_rec(self, root_idx, query, &mut best);
            best.1 as usize
        }
    }

    /// ─────────────────────────────────────────────────────────
    /// テストコード例
    #[cfg(test)]
    mod tests {
        use super::KDTree;
        use nalgebra::Vector3;
        use rand::{thread_rng, Rng};

        #[test]
        fn test_kdtree_nearest() {
            let pts = vec![
                Vector3::new(0.0, 0.0, 0.0), // idx=0
                Vector3::new(1.0, 0.0, 0.0), // idx=1
                Vector3::new(0.0, 2.0, 0.0), // idx=2
                Vector3::new(0.0, 0.0, 3.0), // idx=3
                Vector3::new(5.0, 5.0, 5.0), // idx=4
            ];
            let tree = KDTree::new(pts.clone());

            // クエリ (0.1, 0.1, 0.1) → idx=0 が最寄り
            let q = Vector3::new(0.1, 0.1, 0.1);
            assert_eq!(tree.nearest(&q), 0);

            // クエリ (4.9, 4.9, 4.9) → idx=4 が最寄り
            let q2 = Vector3::new(4.9, 4.9, 4.9);
            assert_eq!(tree.nearest(&q2), 4);
        }

        /// ランダムな点群（N=1000）とランダムなクエリ（Q=1000）を生成し、
        /// KDTree::nearest の結果と愚直探索の結果を比較するテスト
        #[test]
        fn random_nearest_correctness() {
            let mut rng = thread_rng();
            const N: usize = 1000;
            const Q: usize = 1000;

            // 1. ランダムな点群を生成 (座標範囲は [0.0, 1.0) とする)
            let mut points: Vec<Vector3<f64>> = Vec::with_capacity(N);
            for _ in 0..N {
                let x = rng.gen_range(0.0..1.0);
                let y = rng.gen_range(0.0..1.0);
                let z = rng.gen_range(0.0..1.0);
                points.push(Vector3::new(x, y, z));
            }

            // 2. KDTree を構築
            let tree = KDTree::new(points.clone());

            // 3. Q 回ランダムクエリを生成し、KDTree の結果と愚直探索を比較
            for _ in 0..Q {
                let qx = rng.gen_range(0.0..1.0);
                let qy = rng.gen_range(0.0..1.0);
                let qz = rng.gen_range(0.0..1.0);
                let query = Vector3::new(qx, qy, qz);

                // ────── 愚直探索（Brute-force） ──────
                let mut brute_idx = 0;
                let mut brute_dist_sq = f64::INFINITY;
                for (i, p) in points.iter().enumerate() {
                    let d_sq = (p - query).norm_squared();
                    if d_sq < brute_dist_sq {
                        brute_dist_sq = d_sq;
                        brute_idx = i;
                    }
                }

                // ────── KDTree を用いた最近傍探索 ──────
                let kd_idx = tree.nearest(&query);

                // 結果を比較。インデックスが一致するはず。
                assert_eq!(
                    kd_idx, brute_idx,
                    "Mismatch: KDTree returned {}, but brute-force returned {} for query {:?}",
                    kd_idx, brute_idx, query
                );
            }
        }

        #[test]
        fn convex_hull_test() {
            let points = [
                [0.34747579, 0.7618358, 0.57805279],
                [0.2333486, 0.90357391, 0.71899575],
                [0.95755049, 0.5114862, 0.25290919],
                [0.01103717, 0.65554822, 0.26206766],
                [0.4812179, 0.07392082, 0.19045505],
                [0.12991306, 0.54038176, 0.91987081],
                [0.31445755, 0.26937057, 0.20215778],
                [0.77693399, 0.64106001, 0.49111278],
                [0.14971229, 0.57600757, 0.87259677],
                [0.02109585, 0.35413073, 0.2889333],
                [0.4763587, 0.97607602, 0.21661759],
                [0.50739955, 0.01370998, 0.79162781],
                [0.82909583, 0.71350626, 0.419372],
                [0.73096612, 0.13036025, 0.15084491],
                [0.93928697, 0.63285776, 0.84302798],
            ];
            let points = points.map(|p| Vector3::new(p[0], p[1], p[2]));

            let hull = super::convex_hull(&points);

            let expected = vec![1, 2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 14];
            assert_eq!(hull, expected, "Convex hull did not match expected output");
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

        pub const fn to_index(&self, size: usize) -> CoordIndex {
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

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default)]
    pub struct CoordIndex(pub usize);

    impl CoordIndex {
        pub const fn new(index: usize) -> Self {
            Self(index)
        }

        pub const fn to_coord(&self, n: usize) -> Coord {
            Coord::new(self.0 / n, self.0 % n)
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default)]
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

    pub const DIRECTIONS: [char; 4] = ['U', 'R', 'D', 'L'];
    pub const U: usize = 0;
    pub const R: usize = 1;
    pub const D: usize = 2;
    pub const L: usize = 3;

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
    }

    impl<T: Default + Clone> Map2d<T> {
        pub fn with_default(size: usize) -> Self {
            let map = vec![T::default(); size * size];
            Self { size, map }
        }

        pub fn iter(&self) -> impl Iterator<Item = &T> {
            self.map.iter()
        }
    }

    impl<T: Default + Clone + Copy> Map2d<T> {
        pub fn fill(&mut self, value: T) {
            self.map.fill(value);
        }
    }

    impl<T> Index<Coord> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: Coord) -> &Self::Output {
            &self[coordinate.to_index(self.size)]
        }
    }

    impl<T> IndexMut<Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            let size = self.size;
            &mut self[coordinate.to_index(size)]
        }
    }

    impl<T> Index<&Coord> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &Coord) -> &Self::Output {
            &self[coordinate.to_index(self.size)]
        }
    }

    impl<T> IndexMut<&Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: &Coord) -> &mut Self::Output {
            let size = self.size;
            &mut self[coordinate.to_index(size)]
        }
    }

    impl<T> Index<CoordIndex> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: CoordIndex) -> &Self::Output {
            &self.map[coordinate.0]
        }
    }

    impl<T> IndexMut<CoordIndex> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: CoordIndex) -> &mut Self::Output {
            &mut self.map[coordinate.0]
        }
    }

    impl<T> Index<&CoordIndex> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &CoordIndex) -> &Self::Output {
            &self.map[coordinate.0]
        }
    }

    impl<T> IndexMut<&CoordIndex> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: &CoordIndex) -> &mut Self::Output {
            &mut self.map[coordinate.0]
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

    #[derive(Debug, Clone)]
    pub struct ConstMap2d<T, const N: usize> {
        map: Vec<T>,
    }

    impl<T, const N: usize> ConstMap2d<T, N> {
        pub fn new(map: Vec<T>) -> Self {
            assert_eq!(map.len(), N * N);
            Self { map }
        }

        pub fn iter(&self) -> impl Iterator<Item = &T> {
            self.map.iter()
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
            &self[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize> IndexMut<Coord> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            &mut self[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize> Index<&Coord> for ConstMap2d<T, N> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &Coord) -> &Self::Output {
            &self[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize> IndexMut<&Coord> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, coordinate: &Coord) -> &mut Self::Output {
            &mut self[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize> Index<CoordIndex> for ConstMap2d<T, N> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: CoordIndex) -> &Self::Output {
            &self.map[coordinate.0]
        }
    }

    impl<T, const N: usize> IndexMut<CoordIndex> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, coordinate: CoordIndex) -> &mut Self::Output {
            &mut self.map[coordinate.0]
        }
    }

    impl<T, const N: usize> Index<&CoordIndex> for ConstMap2d<T, N> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &CoordIndex) -> &Self::Output {
            &self.map[coordinate.0]
        }
    }

    impl<T, const N: usize> IndexMut<&CoordIndex> for ConstMap2d<T, N> {
        #[inline]
        fn index_mut(&mut self, coordinate: &CoordIndex) -> &mut Self::Output {
            &mut self.map[coordinate.0]
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
}
mod problem {
    use crate::{
        geo::{convex_hull, sub_tetrahedron_volume, tetrahedron_volume},
        grid::{ConstMap2d, Coord},
    };
    use itertools::Itertools;
    use nalgebra::{Vector3, Vector4};
    use ordered_float::OrderedFloat;
    use proconio::input;
    use rand::{seq::SliceRandom as _, SeedableRng};
    use rand_pcg::Pcg64Mcg;
    use std::{
        fmt::Display,
        ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Sub, SubAssign},
        rc::Rc,
    };

    pub type Palette<T> = ConstMap2d<T, { Input::PALLETTE_SIZE }>;

    #[derive(Debug, Clone)]
    pub struct Input {
        pub num_tubes: usize,
        pub max_turns: usize,
        pub cost_per_gram: u64,
        pub tube_colors: Vec<Color>,
        pub target_colors: Vec<Color>,
        pub mixtures: MixtureList,
        pub since: std::time::Instant,
    }

    impl Input {
        pub const PALLETTE_SIZE: usize = 20;
        pub const NUM_TARGETS: usize = 1000;
        pub const ERROR_COEF: f64 = 1e4;

        /// 安全のためジャッジより1桁厳しいEPSにしておく
        pub const EPS: f64 = 1e-7;

        pub fn read() -> Self {
            input! {
                palette_size: usize,
                num_tubes: usize,
                num_targets: usize,
                max_turns: usize,
                cost_per_gram: u64,
            }

            let since = std::time::Instant::now();
            assert_eq!(palette_size, Self::PALLETTE_SIZE);
            assert_eq!(num_targets, Self::NUM_TARGETS);

            let mut tube_colors = Vec::with_capacity(num_tubes);

            for _ in 0..num_tubes {
                input! {
                    c: f64,
                    m: f64,
                    y: f64,
                }

                tube_colors.push(Color::new(c, m, y));
            }

            let mut target_colors = Vec::with_capacity(num_targets);

            for _ in 0..num_targets {
                input! {
                    c: f64,
                    m: f64,
                    y: f64,
                }

                target_colors.push(Color::new(c, m, y));
            }

            // 四面体が小さすぎると入力の丸めの関係（？）で作れない色が出てくる
            // 作れないと死ぬので、K=4のときは妥協
            // K≧5のときも可能性としてはありうるが、可能性は十分小さいと仮定
            let allow_out_of_bounds = num_tubes == 4;
            let mixtures = MixtureList::new(&tube_colors, &target_colors, allow_out_of_bounds);

            Self {
                num_tubes,
                max_turns,
                cost_per_gram,
                tube_colors,
                target_colors,
                mixtures,
                since,
            }
        }

        /// 凸包を構築しない点を除去する
        pub fn trim_points(&self) -> (Self, ConvexHullPoints) {
            let points = self.tube_colors.iter().map(|c| c.0).collect_vec();
            let new_indices = convex_hull(&points);
            eprintln!(
                "Trimmed points: {} -> {} ({:?})",
                self.num_tubes,
                new_indices.len(),
                new_indices
            );

            let num_tubes = new_indices.len();
            let tube_colors = new_indices
                .iter()
                .map(|&i| self.tube_colors[i])
                .collect_vec();

            let mut inv_indices = vec![None; self.tube_colors.len()];

            for (new_index, &old_index) in new_indices.iter().enumerate() {
                inv_indices[old_index] = Some(new_index);
            }

            let mixtures_random_ordered = self
                .mixtures
                .mixtures_random_ordered
                .iter()
                .map(|mixtures| {
                    mixtures
                        .iter()
                        .filter_map(|mixture| {
                            let indice =
                                mixture.map(|paint_and_index| inv_indices[paint_and_index.index]);

                            match indice {
                                [Some(i0), Some(i1), Some(i2), Some(i3)] => Some([
                                    MixInfo::new(mixture[0].amount, i0),
                                    MixInfo::new(mixture[1].amount, i1),
                                    MixInfo::new(mixture[2].amount, i2),
                                    MixInfo::new(mixture[3].amount, i3),
                                ]),
                                _ => None,
                            }
                        })
                        .collect()
                })
                .collect();

            let mixtures_volume_ordered = self
                .mixtures
                .mixtures_volume_ordered
                .iter()
                .map(|mixtures| {
                    mixtures
                        .iter()
                        .filter_map(|mixture| {
                            let indice =
                                mixture.map(|paint_and_index| inv_indices[paint_and_index.index]);

                            match indice {
                                [Some(i0), Some(i1), Some(i2), Some(i3)] => Some([
                                    MixInfo::new(mixture[0].amount, i0),
                                    MixInfo::new(mixture[1].amount, i1),
                                    MixInfo::new(mixture[2].amount, i2),
                                    MixInfo::new(mixture[3].amount, i3),
                                ]),
                                _ => None,
                            }
                        })
                        .collect()
                })
                .collect();

            let input = Self {
                num_tubes,
                max_turns: self.max_turns,
                cost_per_gram: self.cost_per_gram,
                tube_colors,
                target_colors: self.target_colors.clone(),
                mixtures: MixtureList {
                    mixtures_random_ordered: Rc::new(mixtures_random_ordered),
                    mixtures_volume_ordered: Rc::new(mixtures_volume_ordered),
                },
                since: self.since,
            };

            let convex_hull = ConvexHullPoints::new(new_indices);
            (input, convex_hull)
        }
    }

    #[derive(Debug, Clone)]
    pub struct ConvexHullPoints {
        indices: Vec<usize>,
    }

    impl ConvexHullPoints {
        pub fn new(indices: Vec<usize>) -> Self {
            Self { indices }
        }

        /// operationsを、凸包のインデックスに基づいて復元する
        pub fn restore_ops(&self, operations: &mut [Op]) {
            for op in operations.iter_mut() {
                match op {
                    Op::AddPaint(_, color) => {
                        let new_color = self.indices[*color];
                        *color = new_color;
                    }
                    _ => {}
                };
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Default)]
    pub struct Color(pub Vector3<f64>);

    impl Color {
        pub fn new(c: f64, m: f64, y: f64) -> Self {
            Self(Vector3::new(c, m, y))
        }

        pub fn new_vec3(v: Vector3<f64>) -> Self {
            Self(v)
        }

        pub fn error(&self, other: &Self) -> f64 {
            let diff = self.0 - other.0;
            diff.norm()
        }

        /// 4つの色を頂点とする四面体に点Pが含まれるかどうかを判定し、
        /// 含まれる場合はその混合係数を返す。
        /// 含まれない場合はNoneを返す。
        pub fn decompose_into_mix(
            &self,
            basis: &[Color; 4],
            allow_out_of_bounds: bool,
        ) -> Option<Vector4<f64>> {
            let p = self.0;

            let a = basis[0].0;
            let b = basis[1].0;
            let c = basis[2].0;
            let d = basis[3].0;

            let v_total = tetrahedron_volume(&a, &b, &c, &d);

            if v_total < Input::EPS {
                return None; // 体積が小さすぎる = 退化
            }

            // 四面体PBCD, APCD, ABPD, ABCPの体積をまとめて計算する
            let [v0, v1, v2, v3] = sub_tetrahedron_volume(&a, &b, &c, &d, &p);
            let sum = v0 + v1 + v2 + v3;

            // 全体体積と部分体積の合計が一致するか
            // 一致しない場合は点Pは四面体の外にある
            // ここでのEPSは、計算誤差を考慮して少し大きめに設定（作れるものを作れないと判断して候補がなくなるとまずい）
            // また、相対誤差も見る
            const EPS: f64 = 1e-4;
            let diff_abs = (sum - v_total).abs();

            if !allow_out_of_bounds && diff_abs / v_total > EPS {
                return None;
            }

            // 正規化して係数を求める（合計が1になるように）
            let coeffs = Vector4::new(v0, v1, v2, v3) / sum;
            Some(coeffs)
        }

        /// 線分上でselfに最も近い点を返す
        pub fn closest_point_on_segment(&self, basis: &[Color; 2]) -> Color {
            let p = self.0;
            let a = basis[0].0;
            let b = basis[1].0;

            let ab = b - a;
            let ab_len2 = ab.dot(&ab);
            if ab_len2 < Input::EPS {
                // A と B がほぼ同じ位置なら、A を返す
                return Color::new_vec3(a);
            }

            let t = (p - a).dot(&ab) / ab_len2;
            let t_clamped = t.max(0.0).min(1.0);
            Color::new_vec3(a + ab * t_clamped)
        }

        /// 三角形 v0-v1-v2 上で self（P）に最も近い点を返す
        /// Real-Time Collision Detection (Christer Ericson) のアルゴリズムに準拠
        pub fn closest_point_on_triangle(&self, basis: &[Color; 3]) -> Color {
            let p = self.0;
            let v0 = basis[0].0;
            let v1 = basis[1].0;
            let v2 = basis[2].0;

            // 多分これをRustに書き換えてくれたやつ
            // https://stackoverflow.com/questions/2924795/fastest-way-to-compute-point-to-triangle-distance-in-3d

            // 領域分け：
            //   1) 頂点 v0 領域
            //   2) 頂点 v1 領域
            //   3) 頂点 v2 領域
            //   4) 辺 v0–v1 領域
            //   5) 辺 v0–v2 領域
            //   6) 辺 v1–v2 領域
            //   7) 面内部領域

            // ベクトル準備
            let ab = v1 - v0;
            let ac = v2 - v0;
            let ap = p - v0;

            // 頂点 v0 領域判定
            // p が v0 に最も近い ⇔ p は ab, ac の「逆方向」にある
            let d1 = ab.dot(&ap);
            let d2 = ac.dot(&ap);

            if d1 <= 0.0 && d2 <= 0.0 {
                return Color::new_vec3(v0);
            }

            // 頂点 v1 領域判定
            // p が ab の先の v1 側にある ⇔ ab に沿って正方向、ac に対しては遠ざかる方向
            let bp = p - v1;
            let d3 = ab.dot(&bp);
            let d4 = ac.dot(&bp);

            if d3 >= 0.0 && d4 <= d3 {
                return Color::new_vec3(v1);
            }

            // 頂点 v2 領域判定
            let cp = p - v2;
            let d5 = ab.dot(&cp);
            let d6 = ac.dot(&cp);

            if d6 >= 0.0 && d5 <= d6 {
                return Color::new_vec3(v2);
            }

            // 辺 v0–v1 領域判定
            let vc = d1 * d4 - d3 * d2;
            if vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0 {
                let v = d1 / (d1 - d3);
                return Color::new_vec3(v0 + ab * v);
            }

            // 辺 v0–v2 領域判定
            let vb = d5 * d2 - d1 * d6;
            if vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0 {
                let w = d2 / (d2 - d6);
                return Color::new_vec3(v0 + ac * w);
            }

            // 辺 v1–v2 領域判定
            let va = d3 * d6 - d5 * d4;
            if va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0 {
                let w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
                return Color::new_vec3(v1 + (v2 - v1) * w);
            }

            // 面内部領域：バリセントリック座標を使って射影点を計算
            let denom = 1.0 / (va + vb + vc);
            let v = vb * denom;
            let w = vc * denom;
            Color::new_vec3(v0 + ab * v + ac * w)
        }
    }

    impl Add for Color {
        type Output = Self;

        fn add(self, other: Self) -> Self::Output {
            Self(self.0 + other.0)
        }
    }

    impl AddAssign for Color {
        fn add_assign(&mut self, other: Self) {
            self.0 += other.0;
        }
    }

    impl Sub for Color {
        type Output = Self;

        fn sub(self, other: Self) -> Self::Output {
            Self(self.0 - other.0)
        }
    }

    impl SubAssign for Color {
        fn sub_assign(&mut self, other: Self) {
            self.0 -= other.0;
        }
    }

    impl Mul<f64> for Color {
        type Output = Self;

        fn mul(self, scalar: f64) -> Self::Output {
            Self(self.0 * scalar)
        }
    }

    impl MulAssign<f64> for Color {
        fn mul_assign(&mut self, scalar: f64) {
            self.0 *= scalar;
        }
    }

    impl Div<f64> for Color {
        type Output = Self;

        fn div(self, scalar: f64) -> Self::Output {
            Self(self.0 / scalar)
        }
    }

    impl DivAssign<f64> for Color {
        fn div_assign(&mut self, scalar: f64) {
            self.0 /= scalar;
        }
    }

    impl Add for &Color {
        type Output = Color;

        fn add(self, other: &Color) -> Self::Output {
            Color(self.0 + other.0)
        }
    }

    impl AddAssign<&Color> for Color {
        fn add_assign(&mut self, other: &Color) {
            self.0 += other.0;
        }
    }

    impl Sub for &Color {
        type Output = Color;

        fn sub(self, other: &Color) -> Self::Output {
            Color(self.0 - other.0)
        }
    }

    impl SubAssign<&Color> for Color {
        fn sub_assign(&mut self, other: &Color) {
            self.0 -= other.0;
        }
    }

    impl Mul<f64> for &Color {
        type Output = Color;

        fn mul(self, scalar: f64) -> Self::Output {
            Color(self.0 * scalar)
        }
    }

    impl Div<f64> for &Color {
        type Output = Color;

        fn div(self, scalar: f64) -> Self::Output {
            Color(self.0 / scalar)
        }
    }

    impl Display for Color {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({:.5}, {:.5}, {:.5})", self.0[0], self.0[1], self.0[2])
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Default)]
    pub struct Paint {
        pub color: Color,
        pub amount: f64,
    }

    impl Paint {
        pub fn new(color: Color, amount: f64) -> Self {
            Self { color, amount }
        }

        /// 容量を増やす
        pub fn add_amount(&self, amount: f64) -> Self {
            let new_amount = self.amount + amount;
            assert!(new_amount >= -Input::EPS, "Amount must be non-negative");
            Self::new(self.color, new_amount)
        }

        /// 他の Paint と混ぜる
        pub fn mix(&self, other: &Self) -> Self {
            let total_amount = self.amount + other.amount;

            if total_amount == 0.0 {
                Self::default()
            } else {
                let mixed_color = Color(
                    (self.color.0 * self.amount + other.color.0 * other.amount) / total_amount,
                );
                Self::new(mixed_color, total_amount)
            }
        }

        /// 複数の Paint を混ぜる（合計量が0の場合は無色・無量を返す）
        pub fn mix_all(paints: &[Paint]) -> Self {
            let mut total_amount = 0.0;
            let mut weighted_sum = Vector3::zeros();

            for paint in paints {
                weighted_sum += paint.color.0 * paint.amount;
                total_amount += paint.amount;
            }

            if total_amount == 0.0 {
                Self::default()
            } else {
                Self::new(Color(weighted_sum / total_amount), total_amount)
            }
        }
    }

    impl Display for Paint {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "{} ({:.3}g)", self.color, self.amount)
        }
    }

    #[derive(Debug, Clone)]
    pub struct MixtureList {
        // 気軽にCloneできるようにRcで包む
        /// ランダムな順序で並べられた混合物のリスト
        pub mixtures_random_ordered: Rc<Vec<Vec<[MixInfo; 4]>>>,
        /// 四面体の体積が小さい順に並べられた混合物のリスト
        pub mixtures_volume_ordered: Rc<Vec<Vec<[MixInfo; 4]>>>,
    }

    impl MixtureList {
        pub fn new(
            tube_colors: &[Color],
            target_colors: &[Color],
            allow_out_of_bounds: bool,
        ) -> Self {
            let mut mixtures_random_ordered = vec![];
            let mut mixtures_volume_ordered = vec![];
            let mut rng = Pcg64Mcg::from_entropy();
            let num_tubes = tube_colors.len();

            for target_color in target_colors.iter() {
                let mut mixtures = vec![];

                for i0 in 0..num_tubes {
                    let color0 = tube_colors[i0];

                    for i1 in i0 + 1..num_tubes {
                        let color1 = tube_colors[i1];

                        for i2 in i1 + 1..num_tubes {
                            let color2 = tube_colors[i2];

                            for i3 in i2 + 1..num_tubes {
                                let color3 = tube_colors[i3];
                                let colors = [color0, color1, color2, color3];

                                let Some(mix_ratio) =
                                    target_color.decompose_into_mix(&colors, allow_out_of_bounds)
                                else {
                                    continue;
                                };

                                mixtures.push([
                                    (MixInfo::new(mix_ratio[0], i0)),
                                    (MixInfo::new(mix_ratio[1], i1)),
                                    (MixInfo::new(mix_ratio[2], i2)),
                                    (MixInfo::new(mix_ratio[3], i3)),
                                ]);
                            }
                        }
                    }
                }

                mixtures.shuffle(&mut rng);
                mixtures_random_ordered.push(mixtures.clone());

                // 体積の小さい順にソート
                let volumes = mixtures
                    .iter()
                    .map(|mixture| {
                        let color0 = &tube_colors[mixture[0].index];
                        let color1 = &tube_colors[mixture[1].index];
                        let color2 = &tube_colors[mixture[2].index];
                        let color3 = &tube_colors[mixture[3].index];
                        tetrahedron_volume(&color0.0, &color1.0, &color2.0, &color3.0)
                    })
                    .collect_vec();
                let mut indices = (0..mixtures.len()).collect_vec();
                indices.sort_unstable_by_key(|&i| OrderedFloat(volumes[i]));
                let mixtures = indices.iter().map(|&i| mixtures[i]).collect_vec();
                mixtures_volume_ordered.push(mixtures);
            }

            Self {
                mixtures_random_ordered: Rc::new(mixtures_random_ordered),
                mixtures_volume_ordered: Rc::new(mixtures_volume_ordered),
            }
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct MixInfo {
        pub amount: f64,
        pub index: usize,
    }

    impl MixInfo {
        fn new(amount: f64, index: usize) -> Self {
            Self { amount, index }
        }
    }

    pub struct Output {
        pub dividers: Dividers,
        pub operations: Vec<Op>,
        pub error_penalty: u64,
        pub paint_penalty: u64,
        pub total_error: f64,
        pub score: u64,
    }

    impl Output {
        pub fn new(
            input: &Input,
            dividers: Dividers,
            operations: Vec<Op>,
            total_error: f64,
        ) -> Self {
            let total_add = operations
                .iter()
                .filter(|op| matches!(op, Op::AddPaint(_, _)))
                .count();
            let error_penalty = (total_error * Input::ERROR_COEF).round() as u64;
            let paint_penalty = (total_add - Input::NUM_TARGETS) as u64 * input.cost_per_gram;
            let score = paint_penalty + error_penalty + 1;

            Self {
                dividers,
                operations,
                error_penalty,
                paint_penalty,
                total_error,
                score,
            }
        }
    }

    impl Display for Output {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "{}", self.dividers)?;

            for op in &self.operations {
                writeln!(f, "{}", op)?;
            }

            Ok(())
        }
    }

    #[derive(Debug, Clone)]
    pub struct Dividers {
        pub vertical: Palette<bool>,
        pub horizontal: Palette<bool>,
    }

    impl Dividers {
        pub fn new(vertical: Palette<bool>, horizontal: Palette<bool>) -> Self {
            Self {
                vertical,
                horizontal,
            }
        }
    }

    impl Display for Dividers {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            for row in 0..Input::PALLETTE_SIZE {
                let s = (0..Input::PALLETTE_SIZE - 1)
                    .map(|col| {
                        if self.vertical[Coord::new(row, col)] {
                            "1"
                        } else {
                            "0"
                        }
                    })
                    .collect::<Vec<_>>();
                writeln!(f, "{}", s.join(" "))?;
            }

            for row in 0..Input::PALLETTE_SIZE - 1 {
                let s = (0..Input::PALLETTE_SIZE)
                    .map(|col| {
                        if self.horizontal[Coord::new(row, col)] {
                            "1"
                        } else {
                            "0"
                        }
                    })
                    .collect::<Vec<_>>();
                writeln!(f, "{}", s.join(" "))?;
            }

            Ok(())
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum Op {
        AddPaint(Coord, usize),
        DeliverPaint(Coord),
        DiscardPaint(Coord),
        ToggleDivider(Coord, Coord),
    }

    impl Display for Op {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            match self {
                Op::AddPaint(coord, color) => {
                    write!(f, "1 {} {} {}", coord.row(), coord.col(), color)
                }
                Op::DeliverPaint(coord) => write!(f, "2 {} {}", coord.row(), coord.col()),
                Op::DiscardPaint(coord) => write!(f, "3 {} {}", coord.row(), coord.col()),
                Op::ToggleDivider(coord1, coord2) => write!(
                    f,
                    "4 {} {} {} {}",
                    coord1.row(),
                    coord1.col(),
                    coord2.row(),
                    coord2.col()
                ),
            }
        }
    }

    #[cfg(test)]
    mod tests {
        use super::*;
        use nalgebra::{vector, Vector3};

        fn vec3(x: f64, y: f64, z: f64) -> Vector3<f64> {
            Vector3::new(x, y, z)
        }

        fn approx_eq(v1: Vector3<f64>, v2: Vector3<f64>) -> bool {
            (v1 - v2).norm() < 1e-6
        }

        #[test]
        fn test_inside_mixture() {
            let a = Color(vec3(1.0, 0.0, 0.0)); // Red
            let b = Color(vec3(0.0, 1.0, 0.0)); // Green
            let c = Color(vec3(0.0, 0.0, 1.0)); // Blue
            let d = Color(vec3(0.0, 0.0, 0.0)); // Black

            let p = Color(vec3(0.25, 0.25, 0.25)); // Grayish point inside

            let result = p.decompose_into_mix(&[a, b, c, d], false);
            assert!(result.is_some());

            let weights = result.unwrap();
            let sum: f64 = weights.iter().sum();
            assert!((sum - 1.0).abs() < 1e-8);

            for w in weights.iter() {
                assert!(*w >= 0.0);
            }

            // 再構成される色がPと近いことも確認
            let recon = weights[0] * a.0 + weights[1] * b.0 + weights[2] * c.0 + weights[3] * d.0;
            assert!((recon - p.0).norm() < 1e-8);
        }

        #[test]
        fn test_on_vertex() {
            let a = Color(vec3(1.0, 0.0, 0.0));
            let b = Color(vec3(0.0, 1.0, 0.0));
            let c = Color(vec3(0.0, 0.0, 1.0));
            let d = Color(vec3(0.0, 0.0, 0.0));

            let p = a; // Exactly one of the vertices

            let result = p.decompose_into_mix(&[a, b, c, d], false);
            assert!(result.is_some());

            let weights = result.unwrap();
            assert!((weights[0] - 1.0).abs() < 1e-8);
            assert!((weights[1] - 0.0).abs() < 1e-8);
            assert!((weights[2] - 0.0).abs() < 1e-8);
            assert!((weights[3] - 0.0).abs() < 1e-8);
        }

        #[test]
        fn test_degenerate_basis() {
            // All points lie on a line (no volume)
            let a = Color(vec3(1.0, 0.0, 0.0));
            let b = Color(vec3(2.0, 0.0, 0.0));
            let c = Color(vec3(3.0, 0.0, 0.0));
            let d = Color(vec3(4.0, 0.0, 0.0));
            let p = Color(vec3(2.5, 0.0, 0.0));

            let result = p.decompose_into_mix(&[a, b, c, d], false);
            assert!(result.is_none()); // 退化 → 混合できない
        }

        #[test]
        fn test_outside_color() {
            let a = Color(vec3(1.0, 0.0, 0.0));
            let b = Color(vec3(0.0, 1.0, 0.0));
            let c = Color(vec3(0.0, 0.0, 1.0));
            let d = Color(vec3(0.0, 0.0, 0.0));

            let p = Color(vec3(1.0, 1.0, 1.0)); // White, outside the RGB + black tetrahedron

            let result = p.decompose_into_mix(&[a, b, c, d], false);
            assert!(result.is_none());
        }

        #[test]
        fn test_closest_point_on_segment() {
            let a = Color(Vector3::new(0.0, 0.0, 0.0));
            let b = Color(Vector3::new(1.0, 0.0, 0.0));

            // 線分内部の点 → 自身を返す
            let p1 = Color(vector![0.3, 0.0, 0.0]);
            assert!(approx_eq(p1.closest_point_on_segment(&[a, b]).0, p1.0));

            // 線分から外側（右側） → 端点 B を返す
            let p2 = Color(vector![2.0, 0.5, 0.0]);
            assert!(approx_eq(p2.closest_point_on_segment(&[a, b]).0, b.0));

            // 線分から外側（左側） → 端点 A を返す
            let p3 = Color(vector![-1.0, 0.2, 0.0]);
            assert!(approx_eq(p3.closest_point_on_segment(&[a, b]).0, a.0));

            // 線分に垂線を落とした場合 → 中点を返す
            let p4 = Color(vector![0.5, 2.0, 0.0]);
            let expected = vector![0.5, 0.0, 0.0];
            assert!(approx_eq(p4.closest_point_on_segment(&[a, b]).0, expected));
        }

        #[test]
        fn test_closest_point_on_triangle() {
            let v0 = Color(Vector3::new(0.0, 0.0, 0.0));
            let v1 = Color(Vector3::new(1.0, 0.0, 0.0));
            let v2 = Color(Vector3::new(0.0, 1.0, 0.0));

            // 1. 三角形内部の点 → 自身を返す
            let p1 = Color(vector![0.2, 0.2, 0.0]);
            assert!(approx_eq(
                p1.closest_point_on_triangle(&[v0, v1, v2]).0,
                p1.0
            ));

            // 2. 頂点 v0 に最も近い場合
            let p2 = Color(vector![-0.1, -0.1, 0.0]);
            assert!(approx_eq(
                p2.closest_point_on_triangle(&[v0, v1, v2]).0,
                v0.0
            ));

            // 3. 頂点 v1 に最も近い場合
            let p3 = Color(vector![1.2, -0.1, 0.0]);
            assert!(approx_eq(
                p3.closest_point_on_triangle(&[v0, v1, v2]).0,
                v1.0
            ));

            // 4. 頂点 v2 に最も近い場合
            let p4 = Color(vector![-0.1, 1.2, 0.0]);
            assert!(approx_eq(
                p4.closest_point_on_triangle(&[v0, v1, v2]).0,
                v2.0
            ));

            // 5. 辺 v0–v1 上の最近点
            let p5 = Color(vector![0.5, -0.5, 0.0]);
            let expected5 = vector![0.5, 0.0, 0.0];
            assert!(approx_eq(
                p5.closest_point_on_triangle(&[v0, v1, v2]).0,
                expected5
            ));

            // 6. 辺 v0–v2 上の最近点
            let p6 = Color(vector![-0.5, 0.5, 0.0]);
            let expected6 = vector![0.0, 0.5, 0.0];
            assert!(approx_eq(
                p6.closest_point_on_triangle(&[v0, v1, v2]).0,
                expected6
            ));

            // 7. 辺 v1–v2 上の最近点
            let p7 = Color(vector![0.8, 0.8, 0.0]);
            let expected7 = vector![0.5, 0.5, 0.0];
            assert!(approx_eq(
                p7.closest_point_on_triangle(&[v0, v1, v2]).0,
                expected7
            ));
        }
    }
}
mod solver {
    mod annealing {
        use std::time::Duration;

        use itertools::Itertools;
        use rand::Rng;

        use crate::{
            annealing::SingleScore,
            problem::{Color, Input, MixInfo, Paint},
        };

        pub(super) fn run(
            input: &Input,
            start_turn: usize,
            init_amounts: &[f64],
            amount_factor: f64,
        ) -> Vec<[MixInfo; 4]> {
            eprintln!("Init amounts: {:?}", init_amounts);
            eprintln!("Init amounts sum: {}", init_amounts.iter().sum::<f64>());

            let env = Env::new(input, start_turn, amount_factor);
            let state = State::init(&env, start_turn, init_amounts);

            let neigh_gen = NeighGen;
            let annealer = crate::annealing::Annealer::new(1e4, 1e0, 42, 256);
            let duration = Duration::from_millis(2700)
                .saturating_sub(input.since.elapsed())
                .max(Duration::from_millis(50));
            let (state, stats) = annealer.run(&env, state, &neigh_gen, duration.as_secs_f64());

            eprintln!("{}", stats);

            let error_penalty = (state.error_sum * Input::ERROR_COEF) as u64;
            let paint_penalty = (state.used_ceil_sum as i64
                - (Input::NUM_TARGETS - start_turn) as i64)
                * env.cost_per_gram as i64;

            eprintln!("### Annealing ###");
            eprintln!("Error penalty: {}", error_penalty);
            eprintln!("Paint penalty: {}", paint_penalty);
            eprintln!("used amounts: {:?}", state.used_amounts);
            eprintln!("used ceil sum: {}", state.used_ceil_sum);
            eprintln!();

            state.mix_recipes
        }

        struct Env {
            cost_per_gram: u64,
            start_turn: usize,
            amount_factor: f64,
            tube_colors: Vec<Color>,
            target_colors: Vec<Color>,
            mix_candidates: Vec<Vec<[MixInfo; 4]>>,
        }

        impl Env {
            fn new(input: &Input, start_turn: usize, amount_factor: f64) -> Self {
                const MIX_CANDIDATE_SIZE: usize = 50;

                let cost_per_gram = input.cost_per_gram;
                let tube_colors = input.tube_colors.clone();
                let target_colors = input
                    .target_colors
                    .iter()
                    .skip(start_turn)
                    .cloned()
                    .collect_vec();
                let mix_candidates = input
                    .mixtures
                    .mixtures_volume_ordered
                    .iter()
                    .skip(start_turn)
                    .map(|mix| mix.iter().take(MIX_CANDIDATE_SIZE).cloned().collect_vec())
                    .collect_vec();

                Self {
                    amount_factor,
                    cost_per_gram,
                    start_turn,
                    tube_colors,
                    target_colors,
                    mix_candidates,
                }
            }

            fn ceil_amount(&self, amount: f64) -> u64 {
                amount.max(0.0).ceil() as u64
            }

            fn mul_amount_factor(&self, amount: f64) -> f64 {
                amount * self.amount_factor
            }
        }

        struct State {
            value: StateValue,
            errors: Vec<f64>,
        }

        impl State {
            fn init(env: &Env, start_turn: usize, init_amounts: &[f64]) -> Self {
                let base_mix_indices = vec![0; Input::NUM_TARGETS - start_turn];
                let mix_recipes = env
                    .mix_candidates
                    .iter()
                    .map(|candidates| candidates[0])
                    .collect_vec();
                let mut error_sum = 0.0;
                let mut used_ceil_sum = 0;
                let mut errors = vec![0.0; Input::NUM_TARGETS - start_turn];
                let mut used_amounts = init_amounts.iter().map(|&x| -x).collect_vec();

                for (recipe, target_color) in mix_recipes.iter().zip(env.target_colors.iter()) {
                    let paints = recipe.map(|mix_info| {
                        Paint::new(env.tube_colors[mix_info.index], mix_info.amount)
                    });
                    let mixed_color = Paint::mix_all(&paints);
                    let error = mixed_color.color.error(target_color);

                    error_sum += error;
                    errors.push(error);

                    for mix_info in recipe {
                        used_amounts[mix_info.index] += env.mul_amount_factor(mix_info.amount);
                    }
                }

                for amount in &used_amounts {
                    used_ceil_sum += amount.ceil() as u64;
                }

                let value = StateValue::new(
                    base_mix_indices,
                    mix_recipes,
                    error_sum,
                    used_ceil_sum,
                    used_amounts,
                );

                Self { value, errors }
            }
        }

        #[derive(Debug, Clone)]
        struct StateValue {
            base_mix_indices: Vec<usize>,
            mix_recipes: Vec<[MixInfo; 4]>,
            error_sum: f64,
            used_ceil_sum: u64,
            used_amounts: Vec<f64>,
        }

        impl StateValue {
            fn new(
                base_mix_indices: Vec<usize>,
                mix_recipes: Vec<[MixInfo; 4]>,
                error_sum: f64,
                used_ceil_sum: u64,
                used_amounts: Vec<f64>,
            ) -> Self {
                Self {
                    base_mix_indices,
                    mix_recipes,
                    error_sum,
                    used_ceil_sum,
                    used_amounts,
                }
            }
        }

        impl crate::annealing::State for State {
            type Env = Env;
            type StateValue = StateValue;
            type Score = SingleScore;

            fn score(&self, env: &Self::Env) -> Self::Score {
                let wasted_amounts =
                    self.value.used_ceil_sum as i64 - (Input::NUM_TARGETS - env.start_turn) as i64;
                let score = 1.0
                    + (wasted_amounts * env.cost_per_gram as i64) as f64
                    + self.value.error_sum * Input::ERROR_COEF;

                SingleScore(-score)
            }

            fn state_value(&self) -> &Self::StateValue {
                &self.value
            }
        }

        struct NeighGen;

        impl crate::annealing::NeighborGenerator for NeighGen {
            type Env = Env;
            type State = State;

            fn generate(
                &self,
                env: &Self::Env,
                state: &Self::State,
                rng: &mut impl Rng,
            ) -> Box<dyn crate::annealing::Neighbor<Env = Self::Env, State = Self::State>>
            {
                let choice = rng.gen_range(0..3);

                match choice {
                    0 => Box::new(ChangeBaseMix::gen(env, state, rng)),
                    1 => Box::new(ResetRecipe::gen(env, state, rng)),
                    2 => Box::new(ChangeMix::gen(env, state, rng)),
                    _ => unreachable!(),
                }
            }
        }

        struct ChangeBaseMix {
            target_index: usize,
            old_index: usize,
            new_index: usize,
            old_recipe: [MixInfo; 4],
            new_error: f64,
        }

        impl ChangeBaseMix {
            fn gen(env: &Env, state: &State, rng: &mut impl Rng) -> Self {
                let target_index = rng.gen_range(0..state.value.mix_recipes.len());
                let old_index = state.value.base_mix_indices[target_index];
                let new_index = rng.gen_range(0..env.mix_candidates[target_index].len());

                let old_recipe = state.value.mix_recipes[target_index];
                let new_recipe = env.mix_candidates[target_index][new_index];

                let paints =
                    new_recipe.map(|mix| Paint::new(env.tube_colors[mix.index], mix.amount));
                let new_color = Paint::mix_all(&paints);
                let new_error = new_color.color.error(&env.target_colors[target_index]);

                Self {
                    target_index,
                    old_index,
                    new_index,
                    old_recipe,
                    new_error,
                }
            }
        }

        impl crate::annealing::Neighbor for ChangeBaseMix {
            type Env = Env;
            type State = State;

            fn preprocess(&mut self, env: &Self::Env, state: &mut Self::State) {
                for recipe in self.old_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] -= env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                for recipe in env.mix_candidates[self.target_index][self.new_index] {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] += env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                state.value.error_sum -= state.errors[self.target_index];
                state.value.error_sum += self.new_error;
                state.value.base_mix_indices[self.target_index] = self.new_index;
            }

            fn postprocess(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                state.errors[self.target_index] = self.new_error;
                state.value.mix_recipes[self.target_index] =
                    env.mix_candidates[self.target_index][self.new_index];
            }

            fn rollback(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                for recipe in env.mix_candidates[self.target_index][self.new_index] {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] -= env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                for recipe in self.old_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] += env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                state.value.error_sum -= self.new_error;
                state.value.error_sum += state.errors[self.target_index];
                state.value.base_mix_indices[self.target_index] = self.old_index;
            }
        }

        struct ResetRecipe {
            target_index: usize,
            old_recipe: [MixInfo; 4],
            new_error: f64,
        }

        impl ResetRecipe {
            fn gen(env: &Env, state: &State, rng: &mut impl Rng) -> Self {
                let target_index = rng.gen_range(0..state.value.mix_recipes.len());
                let old_recipe = state.value.mix_recipes[target_index];
                let new_recipe =
                    env.mix_candidates[target_index][state.value.base_mix_indices[target_index]];

                let paints =
                    new_recipe.map(|mix| Paint::new(env.tube_colors[mix.index], mix.amount));
                let new_color = Paint::mix_all(&paints);
                let new_error = new_color.color.error(&env.target_colors[target_index]);

                Self {
                    target_index,
                    old_recipe,
                    new_error,
                }
            }
        }

        impl crate::annealing::Neighbor for ResetRecipe {
            type Env = Env;
            type State = State;

            fn preprocess(&mut self, env: &Self::Env, state: &mut Self::State) {
                for recipe in self.old_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] -= env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                let base_recipe = env.mix_candidates[self.target_index]
                    [state.value.base_mix_indices[self.target_index]];

                for recipe in base_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] += env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                state.value.error_sum -= state.errors[self.target_index];
                state.value.error_sum += self.new_error;
            }

            fn postprocess(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                let base_recipe = env.mix_candidates[self.target_index]
                    [state.value.base_mix_indices[self.target_index]];

                state.errors[self.target_index] = self.new_error;
                state.value.mix_recipes[self.target_index] = base_recipe;
            }

            fn rollback(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                let base_recipe = env.mix_candidates[self.target_index]
                    [state.value.base_mix_indices[self.target_index]];

                for recipe in base_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] -= env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                for recipe in self.old_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] += env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                state.value.error_sum -= self.new_error;
                state.value.error_sum += state.errors[self.target_index];
            }
        }

        struct ChangeMix {
            target_index: usize,
            old_recipe: [MixInfo; 4],
            new_recipe: [MixInfo; 4],
            new_error: f64,
        }

        impl ChangeMix {
            fn gen(env: &Env, state: &State, rng: &mut impl Rng) -> Self {
                let target_index = rng.gen_range(0..state.value.mix_recipes.len());
                let old_recipe = state.value.mix_recipes[target_index];

                // テストケース生成と同様に生成
                let mut random_amounts = [0.0; 4];
                let mut x_sum = 0.0;

                for i in 0..4 {
                    let x = -rng.gen_range(f64::EPSILON..=1.0).ln();
                    random_amounts[i] = x;
                    x_sum += x;
                }

                for i in 0..4 {
                    random_amounts[i] /= x_sum;
                }

                // 適当な比で新旧を混ぜる
                let mut new_recipe = old_recipe;
                let ratio = 0.1f64.powf(rng.gen_range(-2.0..=0.0));

                for i in 0..4 {
                    new_recipe[i].amount =
                        old_recipe[i].amount * (1.0 - ratio) + ratio * random_amounts[i];
                }

                let paints =
                    new_recipe.map(|mix| Paint::new(env.tube_colors[mix.index], mix.amount));
                let new_color = Paint::mix_all(&paints);
                let new_error = new_color.color.error(&env.target_colors[target_index]);

                Self {
                    target_index,
                    old_recipe,
                    new_recipe,
                    new_error,
                }
            }
        }

        impl crate::annealing::Neighbor for ChangeMix {
            type Env = Env;
            type State = State;

            fn preprocess(&mut self, env: &Self::Env, state: &mut Self::State) {
                for recipe in self.old_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] -= env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                for recipe in self.new_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] += env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                state.value.error_sum -= state.errors[self.target_index];
                state.value.error_sum += self.new_error;
            }

            fn postprocess(self: Box<Self>, _env: &Self::Env, state: &mut Self::State) {
                state.errors[self.target_index] = self.new_error;
                state.value.mix_recipes[self.target_index] = self.new_recipe;
            }

            fn rollback(self: Box<Self>, env: &Self::Env, state: &mut Self::State) {
                for recipe in self.new_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] -= env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                for recipe in self.old_recipe {
                    state.value.used_ceil_sum -=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                    state.value.used_amounts[recipe.index] += env.mul_amount_factor(recipe.amount);
                    state.value.used_ceil_sum +=
                        env.ceil_amount(state.value.used_amounts[recipe.index]);
                }

                state.value.error_sum -= self.new_error;
                state.value.error_sum += state.errors[self.target_index];
            }
        }
    }
    pub mod fixed_palette {
        pub mod beam {
            use crate::{
                problem::{Input, Op, Output, Paint},
                solver::fixed_palette::Palette,
                util::ChangeMinMax as _,
            };
            use core::f64;
            use rand::{Rng as _, SeedableRng as _};
            use rand_pcg::Pcg64Mcg;
            use std::{
                num::{NonZeroU8, NonZeroUsize},
                time::Duration,
            };

            pub const ESTIMATED_OP_PER_TURN: usize = 4;

            pub fn run(input: &Input, palette: Palette) -> Option<Output> {
                let lane_cnt = palette.lanes.len();
                let mut state = State::init(input.clone(), palette.clone());
                let mut ops = vec![];

                let mut rng = Pcg64Mcg::from_entropy();

                // 最初はランダムに色を混ぜておく
                for lane_i in 0..lane_cnt {
                    let lane = &state.palette.lanes[lane_i];
                    let lane_state = &mut state.lanes[lane_i];

                    for _ in 0..3 {
                        let color_i = rng.gen_range(0..input.num_tubes);
                        let color = input.tube_colors[color_i];
                        ops.push(Op::AddPaint(lane.coord, color_i));

                        let paint = Paint::new(color, 1.0);
                        lane_state.paints[0] = lane_state.paints[0].mix(&paint);
                        lane_state.mixed_paint = lane_state.mixed_paint.mix(&paint);
                        lane_state.amount += 1.0;
                        state.excess_paint += 1;
                    }
                }

                let duration = (Duration::from_millis(2800).saturating_sub(input.since.elapsed()))
                    .as_secs_f64()
                    .max(0.05);
                let beam_width_suggester = crate::beam::common::BayesianBeamWidthSuggester::new(
                    Input::NUM_TARGETS,
                    10,
                    duration,
                    50,
                    1,
                    500,
                    None,
                );
                let beam_search = crate::beam::single_turn::BeamSearch::<_, 16001>::new(
                    beam_width_suggester,
                    Input::NUM_TARGETS,
                );
                let (actions, evaluator) = beam_search.run_default(state).unwrap();

                restore_ops(&palette, &mut ops, actions);

                let output = Output::new(input, palette.dividers, ops, evaluator.error_sum);
                Some(output)
            }

            pub fn run_resume(
                input: &Input,
                palette: Palette,
                paints: Vec<Paint>,
                error_sum: f64,
                excess_paint: usize,
                target_i: usize,
            ) -> (Vec<Op>, f64) {
                let mut lanes = vec![];

                for lane_i in 0..palette.lanes.len() {
                    let lane = &palette.lanes[lane_i];
                    let paint = paints[lane_i];
                    let paints = [paint, Paint::default()];
                    let amount = paint.amount;
                    let lane_state = LaneState::new(paints, paint, lane.capacity, amount);
                    lanes.push(lane_state);
                }

                let mut ops = vec![];

                let state = State::new(
                    input.clone(),
                    palette.clone(),
                    lanes,
                    error_sum,
                    excess_paint,
                    target_i,
                );

                let duration = (Duration::from_millis(2800).saturating_sub(input.since.elapsed()))
                    .as_secs_f64()
                    .max(0.05);
                let turn = Input::NUM_TARGETS - target_i;
                let beam_width_suggester = crate::beam::common::BayesianBeamWidthSuggester::new(
                    turn, 10, duration, 50, 1, 500, None,
                );
                let beam_search = crate::beam::single_turn::BeamSearch::<_, 16001>::new(
                    beam_width_suggester,
                    turn,
                );
                let (actions, evaluator) = beam_search.run_default(state).unwrap();

                restore_ops(&palette, &mut ops, actions);
                let error_sum = evaluator.error_sum;

                (ops, error_sum)
            }

            fn restore_ops(palette: &Palette, ops: &mut Vec<Op>, actions: Vec<Action>) {
                for action in actions {
                    let lane_i = action.lane_i as usize;

                    if let Some(prev_div) = action.prev_div {
                        let prev_div = prev_div.get() as usize;

                        if let Some(op) = palette.lanes[lane_i].divide_ops[prev_div] {
                            ops.push(op);
                        }
                    }

                    if let Some(next_div) = action.next_div {
                        let next_div = next_div.get() as usize;

                        if let Some(op) = palette.lanes[lane_i].divide_ops[next_div] {
                            ops.push(op);
                        }
                    }

                    let recipe =
                        &palette.recipes[action.add_cnt as usize][action.add_color as usize];

                    for &color in recipe.colors.iter() {
                        ops.push(Op::AddPaint(palette.lanes[lane_i].coord, color));
                    }

                    ops.push(Op::DeliverPaint(palette.lanes[lane_i].coord));
                }
            }

            struct State {
                input: Input,
                palette: Palette,
                lanes: Vec<LaneState>,
                hash: Vec<[u32; 3]>,
                error_sum: f64,
                excess_paint: usize,
                target_i: usize,
            }

            impl State {
                fn new(
                    input: Input,
                    palette: Palette,
                    lanes: Vec<LaneState>,
                    error_sum: f64,
                    excess_paint: usize,
                    target_i: usize,
                ) -> Self {
                    let mut hash = vec![];
                    let mut rng = Pcg64Mcg::from_entropy();

                    for _ in 0..=100 {
                        let h = [rng.gen::<u32>(), rng.gen::<u32>(), rng.gen::<u32>()];
                        hash.push(h);
                    }

                    Self {
                        input,
                        palette,
                        lanes,
                        hash,
                        error_sum,
                        excess_paint,
                        target_i,
                    }
                }

                const MAX_CAPACITY: usize = 4;

                fn init(input: Input, palette: Palette) -> Self {
                    let lanes = palette
                        .lanes
                        .iter()
                        .map(|lane| {
                            LaneState::new(
                                [Paint::default(); 2],
                                Paint::default(),
                                lane.capacity,
                                0.0,
                            )
                        })
                        .collect();

                    Self::new(input, palette, lanes, 0.0, 0, 0)
                }
            }

            #[derive(Default, Clone, Copy)]
            struct LaneState {
                paints: [Paint; 2],
                mixed_paint: Paint,
                div_pos: usize,
                amount: f64,
            }

            impl LaneState {
                fn new(
                    paints: [Paint; 2],
                    mixed_paint: Paint,
                    div_pos: usize,
                    amount: f64,
                ) -> Self {
                    Self {
                        paints,
                        mixed_paint,
                        div_pos,
                        amount,
                    }
                }
            }

            impl crate::beam::single_turn::State for State {
                type Evaluator = Evaluator;
                type Hash = BeamHash;
                type Action = Action;

                fn make_initial_node(&self) -> (Self::Evaluator, Self::Hash) {
                    let evaluator = Evaluator::new(
                        &self.input,
                        self.error_sum,
                        self.excess_paint,
                        self.target_i,
                    );

                    let mut hash = BeamHash::new(0);

                    for lane in &self.lanes {
                        hash = hash.update(self, Paint::default(), lane.mixed_paint);
                    }

                    (evaluator, hash)
                }

                fn expand(
                    &mut self,
                    _evaluator: &Self::Evaluator,
                    hash: Self::Hash,
                    candidate_set: &mut impl crate::beam::single_turn::CandidateSet<
                        Action = Self::Action,
                        Evaluator = Self::Evaluator,
                        Hash = Self::Hash,
                    >,
                ) {
                    let target_color = self.input.target_colors[self.target_i];

                    for lane_i in 0..self.palette.lanes.len() {
                        let lane = &self.palette.lanes[lane_i];
                        let lane_state = self.lanes[lane_i];
                        let max_add = (Self::MAX_CAPACITY + 1 - self.lanes[lane_i].amount as usize)
                            .min(self.palette.max_recipe_size);

                        // toggleする
                        for add_cnt in 0..=max_add {
                            let each_amount = lane_state.amount as f64 / lane.capacity as f64;
                            let add_amount = add_cnt as f64;
                            let searcher = &self.palette.searchers[add_cnt];

                            let mut best_div_i = !0;
                            let mut best_recipe_i = !0;
                            let mut best_error = f64::INFINITY;

                            // add_cnt + 1マス以上なら溢れることはない
                            for div_i in 0..=lane.capacity {
                                let amount0 = each_amount * div_i as f64;

                                if amount0 + add_amount < 1.0 - Input::EPS
                                    || amount0 + add_amount > div_i as f64 + Input::EPS
                                {
                                    continue;
                                }

                                let color0 = lane_state.mixed_paint.color;
                                let paint0 = Paint::new(color0, amount0);

                                let needed_color = (target_color * (add_amount + amount0)
                                    - color0 * amount0)
                                    / add_amount;
                                let recipe_i = searcher.nearest(&needed_color);
                                let recipe = &self.palette.recipes[add_cnt][recipe_i];
                                let mixed = paint0.mix(&recipe.paint);
                                let error = mixed.color.error(&target_color);

                                if best_error.change_min(error) {
                                    best_div_i = div_i;
                                    best_recipe_i = recipe_i;
                                }
                            }

                            if best_recipe_i == !0 {
                                continue;
                            }

                            let best_recipe = &self.palette.recipes[add_cnt][best_recipe_i];
                            let amount0 = each_amount * best_div_i as f64;
                            let color0 = lane_state.mixed_paint.color;
                            let paint0 = Paint::new(color0, amount0);

                            let paint1 = paint0.mix(&best_recipe.paint).add_amount(-1.0);
                            let paint2 = Paint::new(color0, lane_state.amount as f64 - amount0);
                            let mixed_paint = paint1.mix(&paint2);

                            let new_hash = hash.update(&self, lane_state.mixed_paint, mixed_paint);
                            let new_excess_paint = self.excess_paint + add_cnt - 1;
                            let new_error_sum = self.error_sum + best_error;
                            let new_paint = [paint1, paint2];

                            let action = Action::new(
                                NonZeroUsize::new(lane_state.div_pos),
                                NonZeroUsize::new(best_div_i),
                                lane_i,
                                add_cnt,
                                best_recipe_i,
                                best_error,
                                lane_state.paints,
                                lane_state.mixed_paint,
                                new_paint,
                                mixed_paint,
                            );
                            let evaluator = Evaluator::new(
                                &self.input,
                                new_error_sum,
                                new_excess_paint,
                                self.target_i,
                            );

                            candidate_set.push(action, evaluator, new_hash, false);
                        }

                        // toggleしない
                        let max_add = max_add.min(lane_state.div_pos - 1);

                        for add_cnt in 0..=max_add {
                            let add_amount = add_cnt as f64;
                            let searcher = &self.palette.searchers[add_cnt];
                            let amount0 = lane_state.paints[0].amount;

                            if amount0 + add_amount < 1.0 - Input::EPS
                                || amount0 + add_amount > lane_state.div_pos as f64 + Input::EPS
                            {
                                continue;
                            }

                            let color0 = lane_state.paints[0].color;
                            let paint0 = lane_state.paints[0];

                            let needed_color = (target_color * (add_amount + amount0)
                                - color0 * amount0)
                                / add_amount;
                            let recipe_i = searcher.nearest(&needed_color);
                            let recipe = &self.palette.recipes[add_cnt][recipe_i];
                            let mixed = paint0.mix(&recipe.paint);
                            let error = mixed.color.error(&target_color);

                            let paint1 = mixed.add_amount(-1.0);
                            let paint2 = lane_state.paints[1];
                            let mixed_paint = paint1.mix(&paint2);

                            let new_hash = hash.update(&self, lane_state.mixed_paint, mixed_paint);
                            let new_excess_paint = self.excess_paint + add_cnt - 1;
                            let new_error_sum = self.error_sum + error;
                            let new_paint = [paint1, paint2];

                            let action = Action::new(
                                None,
                                None,
                                lane_i,
                                add_cnt,
                                recipe_i,
                                error,
                                lane_state.paints,
                                lane_state.mixed_paint,
                                new_paint,
                                mixed_paint,
                            );

                            let evaluator = Evaluator::new(
                                &self.input,
                                new_error_sum,
                                new_excess_paint,
                                self.target_i,
                            );

                            candidate_set.push(action, evaluator, new_hash, false);
                        }
                    }
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Evaluator {
                score: u64,
                error_sum: f64,
            }

            impl Evaluator {
                fn new(
                    input: &Input,
                    error_sum: f64,
                    excess_paint: usize,
                    target_i: usize,
                ) -> Self {
                    let error_penalty = error_sum * Input::ERROR_COEF;
                    let final_excess =
                        excess_paint.saturating_sub(Input::NUM_TARGETS - (target_i + 1));
                    let paint_penalty = final_excess as u64 * input.cost_per_gram;

                    let score = ((error_penalty + paint_penalty as f64) * 1e6) as u64;

                    Self { score, error_sum }
                }
            }

            impl crate::beam::single_turn::Evaluator for Evaluator {
                type Cost = u64;

                fn evaluate(&self) -> Self::Cost {
                    self.score
                }
            }

            #[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
            struct BeamHash {
                hash: u32,
            }

            impl BeamHash {
                fn new(hash: u32) -> Self {
                    Self { hash }
                }

                fn update(self, state: &State, old_paint: Paint, new_paint: Paint) -> Self {
                    let mut hash = self.hash;

                    // 多重集合ハッシュ
                    let old_v3 = old_paint.color.0 * old_paint.amount as f64 * 2.0;
                    let old_h = state.hash[old_v3[0] as usize][0]
                        + state.hash[old_v3[1] as usize][1]
                        + state.hash[old_v3[2] as usize][2];
                    hash -= old_h;

                    let new_v3 = new_paint.color.0 * new_paint.amount as f64 * 2.0;
                    let new_h = state.hash[new_v3[0] as usize][0]
                        + state.hash[new_v3[1] as usize][1]
                        + state.hash[new_v3[2] as usize][2];
                    hash += new_h;

                    Self { hash }
                }
            }

            impl crate::beam::single_turn::BeamHash for BeamHash {}

            impl Into<u64> for BeamHash {
                fn into(self) -> u64 {
                    self.hash as u64
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Action {
                prev_div: Option<NonZeroU8>,
                next_div: Option<NonZeroU8>,
                lane_i: u8,
                add_cnt: u8,
                add_color: u16,
                action_id: u32,
                error: f64,
                prev_paints: [Paint; 2],
                prev_mix_paint: Paint,
                next_paints: [Paint; 2],
                next_mix_paint: Paint,
            }

            static mut ACTION_ID: u32 = 0;

            impl Action {
                fn new(
                    prev_div: Option<NonZeroUsize>,
                    next_div: Option<NonZeroUsize>,
                    lane_i: usize,
                    add_cnt: usize,
                    add_color: usize,
                    error: f64,
                    prev_paints: [Paint; 2],
                    prev_mix_paint: Paint,
                    next_paints: [Paint; 2],
                    next_mix_paint: Paint,
                ) -> Self {
                    unsafe {
                        let prev_div = prev_div.map_or(None, |v| NonZeroU8::new(v.get() as u8));
                        let next_div = next_div.map_or(None, |v| NonZeroU8::new(v.get() as u8));
                        let lane_i = lane_i as u8;
                        let add_cnt = add_cnt as u8;
                        let add_color = add_color as u16;
                        let action_id = ACTION_ID;
                        ACTION_ID += 1;

                        Self {
                            prev_div,
                            next_div,
                            lane_i,
                            add_cnt,
                            add_color,
                            action_id,
                            error,
                            prev_paints,
                            prev_mix_paint,
                            next_paints,
                            next_mix_paint,
                        }
                    }
                }
            }

            impl PartialEq for Action {
                fn eq(&self, other: &Self) -> bool {
                    self.action_id == other.action_id
                }
            }

            impl Eq for Action {}

            impl crate::beam::single_turn::Action for Action {
                type State = State;

                fn apply(&self, state: &mut Self::State) {
                    let lane_state = &mut state.lanes[self.lane_i as usize];

                    if let Some(next_div) = self.next_div {
                        lane_state.div_pos = next_div.get() as usize;
                    }

                    lane_state.paints = self.next_paints;
                    lane_state.mixed_paint = self.next_mix_paint;
                    lane_state.amount += self.add_cnt as f64 - 1.0;
                    state.excess_paint += self.add_cnt as usize - 1;
                    state.error_sum += self.error;
                    state.target_i += 1;
                }

                fn rollback(&self, state: &mut Self::State) {
                    let lane_state = &mut state.lanes[self.lane_i as usize];

                    if let Some(prev_div) = self.prev_div {
                        lane_state.div_pos = prev_div.get() as usize;
                    }

                    lane_state.paints = self.prev_paints;
                    lane_state.mixed_paint = self.prev_mix_paint;
                    lane_state.amount -= self.add_cnt as f64 - 1.0;
                    state.excess_paint = state
                        .excess_paint
                        .wrapping_add_signed(-(self.add_cnt as isize - 1));
                    state.error_sum -= self.error;
                    state.target_i -= 1;
                }
            }
        }
        pub mod palette {
            use crate::{
                geo,
                grid::{ConstMap2d, Coord},
                problem::{Color, Dividers, Input, Op, Paint},
            };
            use itertools::Itertools;
            use ordered_float::OrderedFloat;

            const MAX_RECIPE_SIZE: usize = 3;

            pub fn create_palette(input: &Input) -> Palette {
                let mut dividers =
                    Dividers::new(ConstMap2d::with_default(), ConstMap2d::with_default());

                let mut lanes = vec![];

                for lane_i in 0..Input::PALLETTE_SIZE {
                    let start_col = lane_i;
                    let mut divide_ops = vec![None];

                    for row in 0..Input::PALLETTE_SIZE - 1 {
                        let c0 = Coord::new(row, start_col);
                        let c1 = Coord::new(row + 1, start_col);
                        dividers.vertical[c0] = true;
                        divide_ops.push(Some(Op::ToggleDivider(c0, c1)));
                    }

                    dividers.vertical[Coord::new(Input::PALLETTE_SIZE - 1, start_col)] = true;
                    divide_ops.push(None);

                    let lane =
                        Lane::new(Coord::new(0, start_col), Input::PALLETTE_SIZE, divide_ops);
                    lanes.push(lane);
                }

                let mut recipes = vec![vec![]; MAX_RECIPE_SIZE + 1];
                recipe_dfs(
                    input,
                    0,
                    0,
                    Recipe::new(Paint::default(), vec![]),
                    &mut recipes,
                );

                let searchers = recipes
                    .iter()
                    .map(|recipes| {
                        if recipes.len() < 50 {
                            Searcher::Naive(NaiveRecipeSearcher::new(recipes))
                        } else {
                            Searcher::KDTree(KDTreeRecipeSearcher::new(recipes))
                        }
                    })
                    .collect_vec();

                Palette::new(dividers, lanes, recipes, searchers, MAX_RECIPE_SIZE)
            }

            pub fn create_palette_from(
                input: &Input,
                old_palette: crate::solver::single_lane_palette::Palette,
                old_paints: &[Paint],
                ops: &mut Vec<Op>,
            ) -> (Palette, Vec<Paint>) {
                let dividers = old_palette.dividers;
                let mut lanes = vec![];
                let mut paints = vec![];

                for (tank_i, tank) in old_palette.tanks.iter().enumerate() {
                    const MIN_LANE_SIZE: usize = 20;
                    let div_cnt = (tank.capacity / MIN_LANE_SIZE).max(1);
                    let paint = old_paints[tank_i];

                    for div_i in 0..div_cnt {
                        let from = tank.capacity * div_i / div_cnt;
                        let to = tank.capacity * (div_i + 1) / div_cnt;
                        let mut divide_ops = vec![None];

                        for i in from + 1..to {
                            divide_ops.push(tank.toggle_ops[i]);
                        }

                        divide_ops.push(None);

                        if div_i > 0 {
                            if let Some(op) = tank.toggle_ops[from] {
                                ops.push(op);
                            }
                        }

                        let lane = Lane::new(tank.coords[from], to - from, divide_ops);
                        lanes.push(lane);

                        let amount = paint.amount * (to - from) as f64 / tank.capacity as f64;
                        paints.push(Paint::new(paint.color, amount));
                    }
                }

                let mut recipes = vec![vec![]; MAX_RECIPE_SIZE + 1];
                recipe_dfs(
                    input,
                    0,
                    0,
                    Recipe::new(Paint::default(), vec![]),
                    &mut recipes,
                );

                let searchers = recipes
                    .iter()
                    .map(|recipes| {
                        if recipes.len() < 50 {
                            Searcher::Naive(NaiveRecipeSearcher::new(recipes))
                        } else {
                            Searcher::KDTree(KDTreeRecipeSearcher::new(recipes))
                        }
                    })
                    .collect_vec();

                let palette = Palette::new(dividers, lanes, recipes, searchers, MAX_RECIPE_SIZE);

                (palette, paints)
            }

            fn recipe_dfs(
                input: &Input,
                prev_color: usize,
                depth: usize,
                recipe: Recipe,
                recipes: &mut [Vec<Recipe>],
            ) {
                recipes[depth].push(recipe.clone());

                if depth == recipes.len() - 1 {
                    return;
                }

                for color_i in prev_color..input.num_tubes {
                    let new_paint = recipe
                        .paint
                        .mix(&Paint::new(input.tube_colors[color_i], 1.0));
                    let mut new_colors = recipe.colors.clone();
                    new_colors.push(color_i);
                    let new_recipe = Recipe::new(new_paint, new_colors);
                    recipe_dfs(input, color_i, depth + 1, new_recipe, recipes);
                }
            }

            #[derive(Clone)]
            pub struct Palette {
                pub dividers: Dividers,
                pub lanes: Vec<Lane>,
                pub recipes: Vec<Vec<Recipe>>,
                pub searchers: Vec<Searcher>,
                pub max_recipe_size: usize,
            }

            impl Palette {
                fn new(
                    dividers: Dividers,
                    cells: Vec<Lane>,
                    recipes: Vec<Vec<Recipe>>,
                    searchers: Vec<Searcher>,
                    max_recipe_size: usize,
                ) -> Self {
                    Self {
                        dividers,
                        lanes: cells,
                        recipes,
                        searchers,
                        max_recipe_size,
                    }
                }
            }

            #[derive(Debug, Clone)]
            pub struct Lane {
                pub coord: Coord,
                pub capacity: usize,
                pub divide_ops: Vec<Option<Op>>,
            }

            impl Lane {
                pub(super) fn new(
                    coord: Coord,
                    capacity: usize,
                    divide_ops: Vec<Option<Op>>,
                ) -> Self {
                    Self {
                        coord,
                        capacity,
                        divide_ops,
                    }
                }
            }

            #[derive(Clone)]
            pub enum Searcher {
                Naive(NaiveRecipeSearcher),
                KDTree(KDTreeRecipeSearcher),
            }

            impl Searcher {
                pub fn nearest(&self, color: &Color) -> usize {
                    match self {
                        Searcher::Naive(searcher) => searcher.nearest(color),
                        Searcher::KDTree(searcher) => searcher.nearest(color),
                    }
                }
            }

            #[derive(Clone)]
            pub struct NaiveRecipeSearcher {
                colors: Vec<Color>,
            }

            impl NaiveRecipeSearcher {
                fn new(recipes: &[Recipe]) -> Self {
                    let colors = recipes.iter().map(|recipe| recipe.paint.color).collect();
                    Self { colors }
                }

                fn nearest(&self, color: &Color) -> usize {
                    self.colors
                        .iter()
                        .position_min_by_key(|c| OrderedFloat((c.0 - color.0).norm_squared()))
                        .unwrap()
                }
            }
            #[derive(Clone)]
            pub struct KDTreeRecipeSearcher {
                tree: geo::KDTree,
            }

            impl KDTreeRecipeSearcher {
                fn new(recipes: &[Recipe]) -> Self {
                    let points = recipes.iter().map(|recipe| recipe.paint.color.0).collect();
                    let tree = geo::KDTree::new(points);
                    Self { tree }
                }

                fn nearest(&self, color: &Color) -> usize {
                    self.tree.nearest(&color.0)
                }
            }

            #[derive(Debug, Clone)]
            pub struct Recipe {
                pub paint: Paint,
                pub colors: Vec<usize>,
            }

            impl Recipe {
                fn new(paint: Paint, colors: Vec<usize>) -> Self {
                    Self { paint, colors }
                }
            }
        }

        use crate::solver::fixed_palette::palette::Palette;
    }
    pub mod frac_2digit {
        pub mod beam {
            use super::palette::{Palette, Tank};
            use crate::{
                beam::{self, single_turn::Action as _},
                problem::{Input, Op, Output, Paint},
            };
            use itertools::Itertools;
            use nalgebra::Vector4;
            use rand::{Rng as _, SeedableRng as _};
            use rand_pcg::Pcg64Mcg;

            const ANNEALING_START_TURN: usize = 980;
            const AMOUNT_FACTOR: f64 = 1.0 + 5e-3;

            pub fn run(input: &Input, palette: Palette, duration_sec: f64) -> BeamResult {
                let state = State::new(input, palette.clone());
                let beam_width_suggester = beam::common::BayesianBeamWidthSuggester::new(
                    Input::NUM_TARGETS,
                    10,
                    duration_sec,
                    ((10.0 * duration_sec) as usize).max(10),
                    1,
                    1000,
                    None,
                );
                let beam_search = beam::single_turn::BeamSearch::<_, 16001>::new(
                    beam_width_suggester,
                    Input::NUM_TARGETS,
                );
                let (beam_actions, _evaluator) =
                    beam_search.run_default(state.clone()).ok().unwrap();

                let result = restore_ops(input, palette.clone(), state.clone(), &beam_actions);

                let BeamResult::Completed(beam_output) = result else {
                    return result;
                };

                // 焼きなましと比較
                run_annealing(input, palette, state, beam_actions, beam_output)
            }

            fn run_annealing(
                input: &Input,
                palette: Palette,
                state: State,
                beam_actions: Vec<Action>,
                beam_output: Output,
            ) -> BeamResult {
                let mut state_for_annealing = state.clone();
                let mut annealing_actions = vec![];

                for &action in beam_actions.iter() {
                    if state_for_annealing.target_i >= ANNEALING_START_TURN {
                        break;
                    }

                    annealing_actions.push(action);
                    action.apply(&mut state_for_annealing);
                }

                let amounts = state_for_annealing
                    .amounts
                    .iter()
                    .map(|a| a[0] + a[1])
                    .collect_vec();
                let recipes = crate::solver::annealing::run(
                    input,
                    state_for_annealing.target_i,
                    &amounts,
                    AMOUNT_FACTOR,
                );

                for (i, &recipe) in recipes.iter().enumerate() {
                    let tubes = [
                        recipe[0].index,
                        recipe[1].index,
                        recipe[2].index,
                        recipe[3].index,
                    ];
                    let amounts = Vector4::new(
                        recipe[0].amount,
                        recipe[1].amount,
                        recipe[2].amount,
                        recipe[3].amount,
                    );

                    state_for_annealing.mix_candidates[state_for_annealing.target_i + i] =
                        vec![(tubes, amounts)];
                }

                let turns = Input::NUM_TARGETS - state_for_annealing.target_i;
                let beam_width_suggester =
                    beam::common::BayesianBeamWidthSuggester::new(turns, 5, 0.1, 10, 1, 1000, None);
                let beam_search =
                    beam::single_turn::BeamSearch::<_, 16001>::new(beam_width_suggester, turns);
                let (actions, _evaluator) = beam_search
                    .run_default(state_for_annealing.clone())
                    .ok()
                    .unwrap();

                annealing_actions.extend(actions);
                let annealing_result = restore_ops(input, palette, state, &annealing_actions);

                match annealing_result {
                    BeamResult::Completed(annealing_output) => {
                        eprintln!("beam only : {}", beam_output.score);
                        eprintln!("annealing : {}", annealing_output.score);

                        if annealing_output.score < beam_output.score {
                            BeamResult::Completed(annealing_output)
                        } else {
                            BeamResult::Completed(beam_output)
                        }
                    }
                    BeamResult::Incomplete(_) => BeamResult::Completed(beam_output),
                }
            }

            fn restore_ops(
                input: &Input,
                palette: Palette,
                mut state: State,
                actions: &[Action],
            ) -> BeamResult {
                let mut ops = vec![];
                let mut divs = palette.tanks.iter().map(|t| t.init_divs).collect_vec();
                let mut tank_div_counts = palette
                    .tanks
                    .iter()
                    .map(|tank| DivCounts::new(tank.clone()))
                    .collect_vec();

                for action in actions.iter() {
                    // 100ターンバッファを設ける
                    let remaining_turn = input.max_turns - ops.len();
                    let min_threshold_turn = crate::solver::single_frac::beam::ESTIMATED_OP_PER_TURN
                        * (Input::NUM_TARGETS - state.target_i)
                        + 100;

                    // 残りターンが少ない場合は中断
                    if state.target_i < 995 && remaining_turn < min_threshold_turn {
                        let amounts = state.amounts.iter().map(|a| a[0] + a[1]).collect_vec();
                        let units = state.dividers.iter().map(|d| d[2]).collect_vec();

                        // 仕切りを元に戻す
                        for tube_i in 0..input.num_tubes {
                            let divs = &divs[tube_i];
                            let tank_div_count = &mut tank_div_counts[tube_i];

                            if divs[0] != 0 {
                                tank_div_count.turn_on_div(0, &mut ops);
                                tank_div_count.turn_off_div(divs[0], &mut ops);
                            }

                            tank_div_count.turn_off_div(divs[1], &mut ops);
                        }

                        return BeamResult::Incomplete(IncompleteResult::new(
                            ops,
                            state.target_i,
                            amounts,
                            units,
                            state.error_sum,
                            state.discard_sum,
                            state.excess_paint,
                        ));
                    }

                    // 塗料の追加・繰り下がり
                    for tank_op in action.ops.iter() {
                        let tube_i = tank_op.tube();
                        let tank = &palette.tanks[tube_i];
                        let divs = &mut divs[tube_i];
                        let tank_div_count = &mut tank_div_counts[tube_i];

                        if divs[0] != tank_op.mid_divs1()[0] {
                            tank_div_count.turn_on_div(tank_op.mid_divs1()[0], &mut ops);
                            tank_div_count.turn_off_div(divs[0], &mut ops);
                            divs[0] = tank_op.mid_divs1()[0];
                        }

                        if divs[2] != tank_op.mid_divs1()[2] {
                            tank_div_count.turn_on_div(tank_op.mid_divs1()[2], &mut ops);
                            tank_div_count.turn_off_div(divs[2], &mut ops);
                            divs[2] = tank_op.mid_divs1()[2];
                        }

                        if divs[1] != tank_op.mid_divs1()[1] {
                            tank_div_count.turn_on_div(tank_op.mid_divs1()[1], &mut ops);
                            tank_div_count.turn_off_div(divs[1], &mut ops);
                            divs[1] = tank_op.mid_divs1()[1];
                        }

                        // 塗料の追加
                        if tank_op.add_paint_flag(0) {
                            ops.push(Op::AddPaint(tank.coords[0], tube_i));
                        }

                        // 塗料の繰り下がり
                        if divs[1] != tank_op.next_divs()[1] {
                            tank_div_count.turn_on_div(tank_op.next_divs()[1], &mut ops);
                            tank_div_count.turn_off_div(divs[1], &mut ops);
                            divs[1] = tank_op.next_divs()[1];
                        }

                        // 分母準備
                        if divs[2] != tank_op.mid_divs2()[2] {
                            tank_div_count.turn_on_div(tank_op.mid_divs2()[2], &mut ops);
                            tank_div_count.turn_off_div(divs[2], &mut ops);
                            divs[2] = tank_op.mid_divs2()[2];
                        }
                    }

                    // 塗料の混合
                    for tank_op in action.ops.iter() {
                        let tube_i = tank_op.tube();
                        let divs = &mut divs[tube_i];
                        let tank_div_count = &mut tank_div_counts[tube_i];

                        // 払い出し
                        if divs[0] != tank_op.next_divs()[0] {
                            tank_div_count.turn_on_div(tank_op.next_divs()[0], &mut ops);
                            tank_div_count.turn_off_div(divs[0], &mut ops);
                            divs[0] = tank_op.next_divs()[0];
                        }

                        if divs[2] != tank_op.next_divs()[2] {
                            tank_div_count.turn_on_div(tank_op.next_divs()[2], &mut ops);
                            tank_div_count.turn_off_div(divs[2], &mut ops);
                            divs[2] = tank_op.next_divs()[2];
                        }
                    }

                    ops.push(Op::DeliverPaint(palette.deliver_coord));
                    ops.push(Op::DiscardPaint(palette.deliver_coord));
                    action.apply(&mut state);
                }

                let output = Output::new(input, palette.dividers, ops, state.error_sum);
                BeamResult::Completed(output)
            }

            pub enum BeamResult {
                Completed(Output),
                Incomplete(IncompleteResult),
            }

            pub struct IncompleteResult {
                pub ops: Vec<Op>,
                pub target_i: usize,
                pub amounts: Vec<f64>,
                pub units: Vec<usize>,
                pub total_error: f64,
                pub discard_sum: f64,
                pub excess_paint: usize,
            }

            impl IncompleteResult {
                pub fn new(
                    ops: Vec<Op>,
                    target_i: usize,
                    amounts: Vec<f64>,
                    units: Vec<usize>,
                    total_error: f64,
                    discard_sum: f64,
                    excess_paint: usize,
                ) -> Self {
                    Self {
                        ops,
                        target_i,
                        amounts,
                        units,
                        total_error,
                        discard_sum,
                        excess_paint,
                    }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                input: Input,
                palette: Palette,
                amounts: Vec<[f64; 2]>,
                dividers: Vec<[usize; 3]>,
                mix_candidates: Vec<Vec<([usize; 4], Vector4<f64>)>>,
                hash: Vec<Vec<u32>>,
                error_sum: f64,
                discard_sum: f64,
                excess_paint: usize,
                target_i: usize,
            }

            impl State {
                fn new(input: &Input, palette: Palette) -> Self {
                    const MIX_CANDIDATE_CNT: usize = 200;
                    let mut mix_candidates = vec![];

                    for mixtures in input.mixtures.mixtures_volume_ordered.iter() {
                        let candidates = mixtures
                            .iter()
                            .take(MIX_CANDIDATE_CNT)
                            .map(|mixture| {
                                let tubes = [
                                    mixture[0].index,
                                    mixture[1].index,
                                    mixture[2].index,
                                    mixture[3].index,
                                ];
                                let ratio = [
                                    mixture[0].amount,
                                    mixture[1].amount,
                                    mixture[2].amount,
                                    mixture[3].amount,
                                ];

                                (tubes, Vector4::new(ratio[0], ratio[1], ratio[2], ratio[3]))
                            })
                            .collect_vec();
                        mix_candidates.push(candidates);
                    }

                    let mut hash = vec![];
                    let mut rng = Pcg64Mcg::from_entropy();

                    for _ in 0..palette.tanks.len() {
                        let mut h = vec![];

                        for _ in 0..=200 {
                            h.push(rng.gen::<u32>() << 12);
                        }

                        hash.push(h);
                    }

                    let amounts = vec![[0.0; 2]; input.num_tubes];
                    let dividers = vec![[0; 3]; input.num_tubes];

                    Self {
                        input: input.clone(),
                        palette,
                        mix_candidates,
                        hash,
                        amounts,
                        dividers,
                        error_sum: 0.0,
                        excess_paint: 0,
                        discard_sum: 0.0,
                        target_i: 0,
                    }
                }
            }

            impl crate::beam::single_turn::State for State {
                type Evaluator = Evaluator;
                type Hash = BeamHash;
                type Action = Action;

                fn make_initial_node(&self) -> (Self::Evaluator, Self::Hash) {
                    (Evaluator { score: 0 }, BeamHash { hash: 0 })
                }

                fn expand(
                    &mut self,
                    _evaluator: &Self::Evaluator,
                    hash: Self::Hash,
                    candidate_set: &mut impl crate::beam::single_turn::CandidateSet<
                        Action = Self::Action,
                        Evaluator = Self::Evaluator,
                        Hash = Self::Hash,
                    >,
                ) {
                    let target_color = self.input.target_colors[self.target_i];

                    for (tubes, mix_ratio) in self.mix_candidates[self.target_i].iter() {
                        let mut tank_ops = [[(TankOp::default(), 0.0); 2]; 4];

                        for (op_i, (&tube, &ratio)) in
                            tubes.iter().zip(mix_ratio.iter()).enumerate()
                        {
                            let tank = &self.palette.tanks[tube];
                            let prev_divs = self.dividers[tube];

                            // 0除算に注意
                            let prev_amounts = self.amounts[tube].map(|a| a.max(1e-15));

                            let amount_sum = prev_amounts[0] + prev_amounts[1];
                            let mut mid_divs_1 = prev_divs;
                            let mut amounts = prev_amounts;
                            let mut add_paint = [false; 2];

                            // 塗料が足りていないなら追加
                            if amount_sum < ratio + Input::EPS {
                                // まずは仕切り位置のリセット
                                let each_amount_v1 =
                                    prev_amounts[1] / (mid_divs_1[2] - mid_divs_1[1]).max(1) as f64;
                                let transfer = each_amount_v1
                                    * (tank.init_divs[1].min(mid_divs_1[2]) - mid_divs_1[1]) as f64;
                                mid_divs_1[0] = tank.init_divs[0];
                                mid_divs_1[1] = tank.init_divs[1];
                                mid_divs_1[2] = mid_divs_1[2].max(mid_divs_1[1]);
                                amounts[0] += transfer;
                                amounts[1] -= transfer;

                                // 塗料追加
                                amounts[0] += 1.0;
                                add_paint[0] = true;
                            }

                            // 上の桁から貪欲に取る
                            // 0桁目
                            let mut next_divs = mid_divs_1;
                            let mut needed_amount = ratio;
                            let unit0 = next_divs[1] - next_divs[0];
                            let each_amount_v0 = amounts[0] / (unit0 as f64).max(Input::EPS);
                            let used_unit0 = ((needed_amount / each_amount_v0) as usize).min(unit0);
                            let used_amount0 = each_amount_v0 * used_unit0 as f64;
                            amounts[0] -= used_amount0;
                            needed_amount -= used_amount0;
                            next_divs[0] += used_unit0;

                            // 下の桁が足りていなさそうなら繰り下がり
                            if amounts[1] < needed_amount + Input::EPS {
                                assert!(next_divs[1] - next_divs[0] > 0);
                                amounts[0] -= each_amount_v0;
                                amounts[1] += each_amount_v0;
                                next_divs[1] -= 1;
                                add_paint[1] = true;
                            }

                            // 1桁目
                            // 分数を全探索
                            let needed_frac = (needed_amount / amounts[1]).max(0.0);

                            let mut numerator_l = !0;
                            let mut denominator_l = !0;
                            let mut numerator_r = !0;
                            let mut denominator_r = !0;
                            let mut best_diff_l = f64::MAX;
                            let mut best_diff_r = f64::MAX;

                            let min_denominator = (next_divs[2] - next_divs[1]).max(1);
                            let max_denominator = tank.capacity - next_divs[1];

                            for denominator in min_denominator..=max_denominator {
                                let inv_d = 1.0 / denominator as f64;
                                let l = (denominator as f64 * needed_frac) as usize;
                                let r = l + 1;
                                let diff_l = needed_frac - l as f64 * inv_d;
                                let diff_r = r as f64 * inv_d - needed_frac;

                                if diff_l < best_diff_l {
                                    numerator_l = l;
                                    denominator_l = denominator;
                                    best_diff_l = diff_l;
                                }

                                if diff_r < best_diff_r {
                                    numerator_r = r;
                                    denominator_r = denominator;
                                    best_diff_r = diff_r;
                                }
                            }

                            let amount_l = amounts[1] * numerator_l as f64 / denominator_l as f64;
                            let amount_r = amounts[1] * numerator_r as f64 / denominator_r as f64;

                            let mut mid_divs_l = mid_divs_1;
                            let mut mid_divs_r = mid_divs_1;
                            let mut next_divs_l = next_divs;
                            let mut next_divs_r = next_divs;
                            let mut amounts_l = amounts;
                            let mut amounts_r = amounts;

                            mid_divs_l[2] = next_divs_l[1] + denominator_l;
                            mid_divs_r[2] = next_divs_r[1] + denominator_r;
                            next_divs_l[2] = mid_divs_l[2] - numerator_l;
                            next_divs_r[2] = mid_divs_r[2] - numerator_r;
                            amounts_l[1] -= amount_l;
                            amounts_r[1] -= amount_r;

                            let used_amount_l = used_amount0 + amount_l;
                            let used_amount_r = used_amount0 + amount_r;

                            tank_ops[op_i][0] = (
                                TankOp::new(
                                    tube,
                                    prev_divs,
                                    prev_amounts,
                                    mid_divs_1,
                                    mid_divs_l,
                                    next_divs_l,
                                    amounts_l,
                                    add_paint,
                                ),
                                used_amount_l,
                            );

                            tank_ops[op_i][1] = (
                                TankOp::new(
                                    tube,
                                    prev_divs,
                                    prev_amounts,
                                    mid_divs_1,
                                    mid_divs_r,
                                    next_divs_r,
                                    amounts_r,
                                    add_paint,
                                ),
                                used_amount_r,
                            );
                        }

                        // bit全探索
                        for flag in 0..1 << 4 {
                            let mut paints = [Paint::default(); 4];
                            let mut current_ops = [TankOp::default(); 4];
                            let mut total_amount = 0.0;
                            let mut total_add_cnt = 0;

                            // まず足りているかどうかをチェック
                            for i in 0..4 {
                                let amount = tank_ops[i][(flag >> i) & 1].1;
                                total_amount += amount;
                            }

                            if total_amount <= 1.0 - Input::EPS {
                                continue;
                            }

                            let mut new_hash = hash;

                            for i in 0..4 {
                                let (tank_op, amount) = tank_ops[i][(flag >> i) & 1];
                                current_ops[i] = tank_op;
                                paints[i] =
                                    Paint::new(self.input.tube_colors[tank_op.tube()], amount);

                                if tank_op.add_paint_flag(0) {
                                    total_add_cnt += 1;
                                    new_hash = new_hash.update_add(1);
                                }

                                new_hash = new_hash.update_divs(
                                    &self,
                                    tank_op.tube(),
                                    &tank_op.prev_divs(),
                                    &tank_op.next_divs(),
                                );
                            }

                            let new_paint = Paint::mix_all(&paints);
                            let error = new_paint.color.error(&target_color);
                            let new_error_sum = self.error_sum + error;
                            let new_excess_paint = self.excess_paint + total_add_cnt - 1;
                            let new_discard_sum = self.discard_sum + (new_paint.amount - 1.0);

                            let evaluator = Evaluator::new(
                                &self.input,
                                new_error_sum,
                                new_excess_paint,
                                new_discard_sum,
                                self.target_i,
                            );

                            let action = Action::new(
                                current_ops,
                                error,
                                new_paint.amount - 1.0,
                                total_add_cnt as isize - 1,
                            );

                            candidate_set.push(action, evaluator, new_hash, false);
                        }
                    }
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Evaluator {
                score: u64,
            }

            impl Evaluator {
                fn new(
                    input: &Input,
                    error_sum: f64,
                    excess_paint: usize,
                    discard_sum: f64,
                    target_i: usize,
                ) -> Self {
                    let error_penalty = error_sum * Input::ERROR_COEF;
                    let paint_penalty = (excess_paint as u64 * input.cost_per_gram) as f64;
                    let discard_penalty = discard_sum * input.cost_per_gram as f64;

                    const START_TURN: usize = 980;
                    let t = if target_i < START_TURN {
                        0.0
                    } else {
                        (target_i - START_TURN) as f64
                            / (Input::NUM_TARGETS - START_TURN - 1) as f64
                    };

                    let score =
                        ((error_penalty + paint_penalty * t + discard_penalty * (1.0 - t) + 1.0)
                            * 1e6) as u64;

                    Self { score }
                }
            }

            impl crate::beam::single_turn::Evaluator for Evaluator {
                type Cost = u64;

                fn evaluate(&self) -> Self::Cost {
                    self.score
                }
            }

            #[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
            struct BeamHash {
                hash: u32,
            }

            impl BeamHash {
                fn update_add(&self, add_cnt: usize) -> Self {
                    let hash = self.hash + add_cnt as u32;
                    Self { hash }
                }

                fn update_divs(
                    &self,
                    state: &State,
                    tank_i: usize,
                    old_dividers: &[usize; 3],
                    new_dividers: &[usize; 3],
                ) -> Self {
                    let mut hash = self.hash;
                    let hashes = &state.hash[tank_i];
                    hash ^=
                        hashes[old_dividers[0]] ^ hashes[old_dividers[1]] ^ hashes[old_dividers[2]];
                    hash ^=
                        hashes[new_dividers[0]] ^ hashes[new_dividers[1]] ^ hashes[new_dividers[2]];
                    Self { hash }
                }
            }

            impl crate::beam::single_turn::BeamHash for BeamHash {}

            impl Into<u64> for BeamHash {
                fn into(self) -> u64 {
                    self.hash as u64
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Action {
                ops: [TankOp; 4],
                error_diff: f64,
                discard_diff: f64,
                excess_diff: isize,
                action_id: u32,
            }

            static mut ACTION_ID: u32 = 0;

            impl Action {
                fn new(
                    ops: [TankOp; 4],
                    error_diff: f64,
                    discard_diff: f64,
                    excess_diff: isize,
                ) -> Self {
                    unsafe {
                        let action_id = ACTION_ID;
                        ACTION_ID += 1;

                        Self {
                            ops,
                            error_diff,
                            discard_diff,
                            excess_diff,
                            action_id,
                        }
                    }
                }
            }

            impl PartialEq for Action {
                fn eq(&self, other: &Self) -> bool {
                    self.action_id == other.action_id
                }
            }

            impl Eq for Action {}

            #[derive(Debug, Clone, Copy, Default)]
            struct TankOp {
                /// どのチューブを使うか
                tube: u8,
                /// 元々の仕切りの位置
                prev_divs: [u8; 3],
                /// 元々の量
                prev_amounts: [f64; 2],
                /// 操作途中の仕切りの位置1
                mid_divs_1: [u8; 3],
                /// 操作途中の仕切りの位置2
                mid_divs_2: [u8; 3],
                /// 次の仕切りの位置
                next_divs: [u8; 3],
                /// 使用後の量
                next_amounts: [f64; 2],
                /// 追加の有無
                add_paint_flag: u8,
            }

            impl TankOp {
                fn new(
                    tube: usize,
                    prev_divs: [usize; 3],
                    prev_amounts: [f64; 2],
                    mid_divs_1: [usize; 3],
                    mid_divs_2: [usize; 3],
                    next_divs: [usize; 3],
                    next_amounts: [f64; 2],
                    add_paint_flag: [bool; 2],
                ) -> Self {
                    let tube = tube as u8;
                    let prev_divs = prev_divs.map(|d| d as u8);
                    let mid_divs_1 = mid_divs_1.map(|d| d as u8);
                    let mid_divs_2 = mid_divs_2.map(|d| d as u8);
                    let next_divs = next_divs.map(|d| d as u8);
                    let add_paint_flag =
                        (add_paint_flag[0] as u8) | ((add_paint_flag[1] as u8) << 1);

                    Self {
                        tube,
                        prev_divs,
                        prev_amounts,
                        mid_divs_1,
                        mid_divs_2,
                        next_divs,
                        next_amounts,
                        add_paint_flag,
                    }
                }

                fn tube(&self) -> usize {
                    self.tube as usize
                }

                fn add_paint_flag(&self, cell_i: usize) -> bool {
                    (self.add_paint_flag >> cell_i) & 1 != 0
                }

                fn next_divs(&self) -> [usize; 3] {
                    self.next_divs.map(|d| d as usize)
                }

                fn mid_divs1(&self) -> [usize; 3] {
                    self.mid_divs_1.map(|d| d as usize)
                }

                fn mid_divs2(&self) -> [usize; 3] {
                    self.mid_divs_2.map(|d| d as usize)
                }

                fn prev_divs(&self) -> [usize; 3] {
                    self.prev_divs.map(|d| d as usize)
                }
            }

            impl crate::beam::single_turn::Action for Action {
                type State = State;

                fn apply(&self, state: &mut Self::State) {
                    for op in self.ops.iter() {
                        state.amounts[op.tube()] = op.next_amounts;
                        state.dividers[op.tube()] = op.next_divs();
                    }

                    state.error_sum += self.error_diff;
                    state.discard_sum += self.discard_diff;
                    state.excess_paint = state.excess_paint.wrapping_add_signed(self.excess_diff);
                    state.target_i += 1;
                }

                fn rollback(&self, state: &mut Self::State) {
                    for op in self.ops.iter() {
                        state.amounts[op.tube()] = op.prev_amounts;
                        state.dividers[op.tube()] = op.prev_divs();
                    }

                    state.error_sum -= self.error_diff;
                    state.discard_sum -= self.discard_diff;
                    state.excess_paint = state.excess_paint.wrapping_add_signed(-self.excess_diff);
                    state.target_i -= 1;
                }
            }

            struct DivCounts {
                tank: Tank,
                div_cnts: Vec<usize>,
            }

            impl DivCounts {
                fn new(tank: Tank) -> Self {
                    let mut div_cnts = vec![0; tank.capacity + 1];

                    for &div in tank.init_divs.iter() {
                        div_cnts[div] += 1;
                    }

                    Self { tank, div_cnts }
                }

                fn turn_off_div(&mut self, pos: usize, ops: &mut Vec<Op>) {
                    assert!(self.div_cnts[pos] > 0);
                    self.div_cnts[pos] -= 1;

                    if self.div_cnts[pos] == 0 {
                        ops.push(self.tank.toggle_ops[pos]);
                    }
                }

                fn turn_on_div(&mut self, pos: usize, ops: &mut Vec<Op>) {
                    self.div_cnts[pos] += 1;

                    if self.div_cnts[pos] == 1 {
                        ops.push(self.tank.toggle_ops[pos]);
                    }
                }
            }
        }
        pub mod palette {
            use crate::{
                grid::{ConstMap2d, Coord, ADJACENTS, D, L, R},
                problem::{Dividers, Input, Op},
            };

            pub fn create_palette(input: &Input) -> Option<Palette> {
                if input.num_tubes <= 10 {
                    Some(create_u10(input))
                } else if input.num_tubes <= 19 {
                    Some(create_10to19(input))
                } else {
                    // TODO: K=20のケースを作る
                    None
                }
            }

            fn create_u10(input: &Input) -> Palette {
                assert!(input.num_tubes <= 10);

                let each_size = Input::PALLETTE_SIZE / input.num_tubes;
                let mut remaining_cols = Input::PALLETTE_SIZE % input.num_tubes;
                let mut tanks = vec![];
                let mut start_col = 0;
                let mut dividers =
                    Dividers::new(ConstMap2d::with_default(), ConstMap2d::with_default());

                // 壁を一旦作る
                for col in 0..Input::PALLETTE_SIZE {
                    dividers.horizontal[Coord::new(Input::PALLETTE_SIZE - 2, col)] = true;
                }

                for _ in 0..input.num_tubes {
                    let mut width = each_size;

                    if remaining_cols > 0 {
                        width += 1;
                        remaining_cols -= 1;
                    }

                    let mut coords = vec![];
                    let mut toggles = vec![];

                    // 下から上
                    for row in (0..Input::PALLETTE_SIZE - 1).rev() {
                        let c = Coord::new(row, start_col);
                        coords.push(c);
                        toggles.push(Op::ToggleDivider(c, c + ADJACENTS[D]));

                        if row > 0 {
                            dividers.vertical[c] = true;
                        }
                    }

                    let c = Coord::new(0, start_col);
                    let right = Coord::new(0, start_col + 1);
                    toggles.push(Op::ToggleDivider(c, right));

                    // 上から下にジグザグに
                    for row in 0..Input::PALLETTE_SIZE - 1 {
                        if row % 2 == 0 {
                            for col in start_col + 1..start_col + width - 1 {
                                let c = Coord::new(row, col);
                                let r = c + ADJACENTS[R];
                                coords.push(c);
                                toggles.push(Op::ToggleDivider(c, r));
                                dividers.horizontal[c] = true;
                            }

                            let c = Coord::new(row, start_col + width - 1);
                            coords.push(c);
                            toggles.push(Op::ToggleDivider(c, c + ADJACENTS[D]));
                        } else {
                            for col in (start_col + 2..start_col + width).rev() {
                                let c = Coord::new(row, col);
                                let l = c + ADJACENTS[L];
                                coords.push(c);
                                toggles.push(Op::ToggleDivider(c, l));
                                dividers.horizontal[c] = true;
                            }

                            let c = Coord::new(row, start_col + 1);
                            coords.push(c);
                            toggles.push(Op::ToggleDivider(c, c + ADJACENTS[D]));
                        }
                    }

                    for row in 0..Input::PALLETTE_SIZE - 1 {
                        dividers.vertical[Coord::new(row, start_col + width - 1)] = true;
                    }

                    let capacity = coords.len();
                    let init_divs = [0, (capacity + 1) / 3, capacity];

                    // 中央の仕切りを追加
                    let op = toggles[init_divs[1]];

                    if let Op::ToggleDivider(c1, c2) = op {
                        let min = c1.min(c2);

                        if c1.row() == c2.row() {
                            dividers.vertical[min] = true;
                        } else {
                            dividers.horizontal[min] = true;
                        }
                    }

                    tanks.push(Tank::new(capacity, coords, toggles, init_divs));
                    start_col += width;
                }

                let deliver_point = Coord::new(Input::PALLETTE_SIZE - 1, 0);
                Palette::new(deliver_point, dividers, tanks)
            }

            fn create_10to19(input: &Input) -> Palette {
                assert!(input.num_tubes <= 19);

                let each_size = (Input::PALLETTE_SIZE - 1) / input.num_tubes;
                let mut remaining_cols = (Input::PALLETTE_SIZE - 1) % input.num_tubes;
                let mut tanks = vec![];
                let mut start_col = 1;
                let mut dividers =
                    Dividers::new(ConstMap2d::with_default(), ConstMap2d::with_default());

                // 壁を一旦作る
                for col in 1..Input::PALLETTE_SIZE {
                    dividers.horizontal[Coord::new(0, col)] = true;
                    dividers.horizontal[Coord::new(Input::PALLETTE_SIZE - 2, col)] = true;
                }

                for row in 1..Input::PALLETTE_SIZE - 1 {
                    dividers.vertical[Coord::new(row, 0)] = true;
                }

                for _ in 0..input.num_tubes {
                    let mut width = each_size;

                    if remaining_cols > 0 {
                        width += 1;
                        remaining_cols -= 1;
                    }

                    let mut coords = vec![];
                    let mut toggles = vec![];

                    if width == 1 {
                        // 壁を建築
                        for row in 1..Input::PALLETTE_SIZE - 1 {
                            dividers.vertical[Coord::new(row, start_col)] = true;
                        }

                        // 下から上
                        for row in (1..Input::PALLETTE_SIZE - 1).rev() {
                            let c = Coord::new(row, start_col);
                            coords.push(c);
                            toggles.push(Op::ToggleDivider(c, c + ADJACENTS[D]));
                        }

                        let c = Coord::new(1, start_col);
                        let up = Coord::new(0, start_col);
                        toggles.push(Op::ToggleDivider(c, up));
                    } else {
                        // 壁を建築
                        for row in 1..Input::PALLETTE_SIZE - 1 {
                            if row > 1 {
                                dividers.vertical[Coord::new(row, start_col)] = true;
                            }

                            dividers.vertical[Coord::new(row, start_col + 1)] = true;
                        }

                        // 下から上
                        for row in (1..Input::PALLETTE_SIZE - 1).rev() {
                            let c = Coord::new(row, start_col);
                            coords.push(c);
                            toggles.push(Op::ToggleDivider(c, c + ADJACENTS[D]));
                        }

                        let c = Coord::new(1, start_col + 1);
                        let left = Coord::new(1, start_col);
                        toggles.push(Op::ToggleDivider(c, left));

                        // 上から下
                        for row in 1..Input::PALLETTE_SIZE - 1 {
                            let c = Coord::new(row, start_col + 1);
                            let d = c + ADJACENTS[D];
                            coords.push(c);
                            toggles.push(Op::ToggleDivider(c, d));
                        }
                    }

                    let capacity = coords.len();
                    let init_divs = [0, (capacity + 1) / 3, capacity];

                    // 中央の仕切りを追加
                    let op = toggles[init_divs[1]];

                    if let Op::ToggleDivider(c1, c2) = op {
                        let min = c1.min(c2);

                        if c1.row() == c2.row() {
                            dividers.vertical[min] = true;
                        } else {
                            dividers.horizontal[min] = true;
                        }
                    }

                    tanks.push(Tank::new(capacity, coords, toggles, init_divs));
                    start_col += width;
                }

                let deliver_point = Coord::new(Input::PALLETTE_SIZE - 1, 0);
                Palette::new(deliver_point, dividers, tanks)
            }

            #[derive(Debug, Clone)]
            pub struct Palette {
                pub deliver_coord: Coord,
                pub dividers: Dividers,
                pub tanks: Vec<Tank>,
            }

            impl Palette {
                pub fn new(deliver_coord: Coord, dividers: Dividers, tanks: Vec<Tank>) -> Self {
                    Self {
                        deliver_coord,
                        tanks,
                        dividers,
                    }
                }
            }

            #[derive(Debug, Clone)]
            pub struct Tank {
                pub capacity: usize,
                pub coords: Vec<Coord>,
                pub toggle_ops: Vec<Op>,
                pub init_divs: [usize; 3],
            }

            impl Tank {
                pub fn new(
                    capacity: usize,
                    coords: Vec<Coord>,
                    toggle_ops: Vec<Op>,
                    init_divs: [usize; 3],
                ) -> Self {
                    Self {
                        capacity,
                        coords,
                        toggle_ops,
                        init_divs,
                    }
                }
            }
        }
    }
    pub mod single_frac {
        pub mod beam {
            use super::super::single_lane_palette::Palette;
            use crate::{
                beam::{self, single_turn::Action as _},
                problem::{Input, Op, Output, Paint},
            };
            use itertools::Itertools;
            use nalgebra::Vector4;
            use rand::{Rng as _, SeedableRng as _};
            use rand_pcg::Pcg64Mcg;

            pub const ESTIMATED_OP_PER_TURN: usize = 19;
            const ANNEALING_START_TURN: usize = 980;
            const AMOUNT_FACTOR: f64 = 1.0 + 1e-2;

            pub fn run(input: &Input, palette: Palette, duration_sec: f64) -> BeamResult {
                let state = State::init(input, palette.clone());
                let beam_width_suggester = beam::common::BayesianBeamWidthSuggester::new(
                    Input::NUM_TARGETS,
                    10,
                    duration_sec,
                    ((10.0 * duration_sec) as usize).max(10),
                    1,
                    1000,
                    None,
                );
                let beam_search = beam::single_turn::BeamSearch::<_, 16001>::new(
                    beam_width_suggester,
                    Input::NUM_TARGETS,
                );
                let (beam_actions, _evaluator) =
                    beam_search.run_default(state.clone()).ok().unwrap();

                let result = restore_ops(input, palette.clone(), state.clone(), &beam_actions);

                let BeamResult::Completed(beam_output) = result else {
                    return result;
                };

                // 焼きなましと比較
                let annealing_result = run_annealing(input, palette, state, beam_actions);

                match annealing_result {
                    BeamResult::Completed(annealing_output) => {
                        eprintln!("beam only : {}", beam_output.score);
                        eprintln!("annealing : {}", annealing_output.score);

                        if annealing_output.score < beam_output.score {
                            BeamResult::Completed(annealing_output)
                        } else {
                            BeamResult::Completed(beam_output)
                        }
                    }
                    BeamResult::Incomplete(_) => BeamResult::Completed(beam_output),
                }
            }

            pub fn run_resume(
                input: &Input,
                palette: Palette,
                amounts: Vec<f64>,
                units: Vec<usize>,
                error_sum: f64,
                discard_sum: f64,
                excess_paint: usize,
                target_i: usize,
                duration_sec: f64,
            ) -> Vec<(Vec<Op>, f64)> {
                let state = State::new(
                    input,
                    palette.clone(),
                    amounts,
                    units,
                    error_sum,
                    discard_sum,
                    excess_paint,
                    target_i,
                );

                let turn = Input::NUM_TARGETS - target_i;

                let beam_width_suggester = beam::common::BayesianBeamWidthSuggester::new(
                    turn,
                    10,
                    duration_sec,
                    ((10.0 * duration_sec) as usize).max(10),
                    1,
                    1000,
                    None,
                );
                let beam_search =
                    beam::single_turn::BeamSearch::<_, 16001>::new(beam_width_suggester, turn);
                let (beam_actions, _evaluator) =
                    beam_search.run_default(state.clone()).ok().unwrap();

                let BeamResult::Completed(result) =
                    restore_ops(input, palette.clone(), state.clone(), &beam_actions)
                else {
                    panic!("Expected completed result, got incomplete");
                };

                let ops = result.operations;
                let error_sum = result.total_error;

                let mut candidates = vec![(ops, error_sum)];

                // 焼きなましと比較
                let annealing_result = run_annealing(input, palette, state, beam_actions);

                if let BeamResult::Completed(annealing_output) = annealing_result {
                    let ops = annealing_output.operations;
                    let error_sum = annealing_output.total_error;
                    candidates.push((ops, error_sum));
                }

                candidates
            }

            fn run_annealing(
                input: &Input,
                palette: Palette,
                state: State,
                beam_actions: Vec<Action>,
            ) -> BeamResult {
                let mut state_for_annealing = state.clone();
                let mut annealing_actions = vec![];

                for &action in beam_actions.iter() {
                    if state_for_annealing.target_i >= ANNEALING_START_TURN {
                        break;
                    }

                    annealing_actions.push(action);
                    action.apply(&mut state_for_annealing);
                }

                let amounts = state_for_annealing.amounts.clone();
                let recipes = crate::solver::annealing::run(
                    input,
                    state_for_annealing.target_i,
                    &amounts,
                    AMOUNT_FACTOR,
                );

                for (i, &recipe) in recipes.iter().enumerate() {
                    let tubes = [
                        recipe[0].index,
                        recipe[1].index,
                        recipe[2].index,
                        recipe[3].index,
                    ];
                    let amounts = Vector4::new(
                        recipe[0].amount,
                        recipe[1].amount,
                        recipe[2].amount,
                        recipe[3].amount,
                    );

                    state_for_annealing.mix_candidates[state_for_annealing.target_i + i] =
                        vec![(tubes, amounts)];
                }

                let turns = Input::NUM_TARGETS - state_for_annealing.target_i;
                let beam_width_suggester =
                    beam::common::BayesianBeamWidthSuggester::new(turns, 5, 0.1, 10, 1, 1000, None);
                let beam_search =
                    beam::single_turn::BeamSearch::<_, 16001>::new(beam_width_suggester, turns);
                let (actions, _evaluator) = beam_search
                    .run_default(state_for_annealing.clone())
                    .ok()
                    .unwrap();

                annealing_actions.extend(actions);
                let annealing_result = restore_ops(input, palette, state, &annealing_actions);

                annealing_result
            }

            fn restore_ops(
                input: &Input,
                palette: Palette,
                mut state: State,
                actions: &[Action],
            ) -> BeamResult {
                let mut ops = vec![];

                for action in actions.iter() {
                    // 100ターンバッファを設ける
                    let remaining_turn = input.max_turns - ops.len();
                    let min_threshold_turn =
                        crate::solver::single_frac::half_beam::ESTIMATED_OP_PER_TURN
                            * (Input::NUM_TARGETS - state.target_i)
                            + 100;

                    // 残りターンが少ない場合は中断
                    if state.target_i < 995 && remaining_turn < min_threshold_turn {
                        return BeamResult::Incomplete(IncompleteResult::new(
                            ops,
                            state.target_i,
                            state.amounts,
                            state.units,
                            state.error_sum,
                            state.discard_sum,
                            state.excess_paint,
                        ));
                    }

                    // 前準備
                    for op in action.ops.iter() {
                        // 分母の設定
                        let tube_i = op.tube();
                        let tank = &palette.tanks[tube_i];

                        if op.prev_unit != op.mid_unit {
                            if let Some(op) = tank.toggle_ops[op.mid_unit as usize] {
                                ops.push(op);
                            }

                            if let Some(op) = tank.toggle_ops[op.prev_unit as usize] {
                                ops.push(op);
                            }
                        }

                        // 塗料の追加
                        if op.add_paint_flag {
                            ops.push(Op::AddPaint(tank.coords[0], tube_i));
                        }
                    }

                    // 塗料の混合
                    for op in action.ops.iter() {
                        // 分母の設定
                        let tube_i = op.tube();
                        let tank = &palette.tanks[tube_i];

                        if op.mid_unit != op.next_unit {
                            if let Some(op) = tank.toggle_ops[op.next_unit as usize] {
                                ops.push(op);
                            }

                            if let Some(op) = tank.toggle_ops[op.mid_unit as usize] {
                                ops.push(op);
                            }
                        }
                    }

                    ops.push(Op::DeliverPaint(palette.deliver_coord));
                    ops.push(Op::DiscardPaint(palette.deliver_coord));
                    action.apply(&mut state);
                }

                let output = Output::new(input, palette.dividers, ops, state.error_sum);
                BeamResult::Completed(output)
            }

            pub enum BeamResult {
                Completed(Output),
                Incomplete(IncompleteResult),
            }

            pub struct IncompleteResult {
                pub ops: Vec<Op>,
                pub target_i: usize,
                pub amounts: Vec<f64>,
                pub units: Vec<usize>,
                pub total_error: f64,
                pub discard_sum: f64,
                pub excess_paint: usize,
            }

            impl IncompleteResult {
                pub fn new(
                    ops: Vec<Op>,
                    target_i: usize,
                    amounts: Vec<f64>,
                    units: Vec<usize>,
                    total_error: f64,
                    discard_sum: f64,
                    excess_paint: usize,
                ) -> Self {
                    Self {
                        ops,
                        target_i,
                        amounts,
                        units,
                        total_error,
                        discard_sum,
                        excess_paint,
                    }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                input: Input,
                palette: Palette,
                amounts: Vec<f64>,
                units: Vec<usize>,
                mix_candidates: Vec<Vec<([usize; 4], Vector4<f64>)>>,
                hash: Vec<Vec<u32>>,
                error_sum: f64,
                discard_sum: f64,
                excess_paint: usize,
                target_i: usize,
            }

            impl State {
                fn new(
                    input: &Input,
                    palette: Palette,
                    amounts: Vec<f64>,
                    units: Vec<usize>,
                    error_sum: f64,
                    discard_sum: f64,
                    excess_paint: usize,
                    target_i: usize,
                ) -> Self {
                    const MIX_CANDIDATE_CNT: usize = 200;
                    let mut mix_candidates = vec![];

                    for mixtures in input.mixtures.mixtures_volume_ordered.iter() {
                        let candidates = mixtures
                            .iter()
                            .take(MIX_CANDIDATE_CNT)
                            .map(|mixture| {
                                let tubes = [
                                    mixture[0].index,
                                    mixture[1].index,
                                    mixture[2].index,
                                    mixture[3].index,
                                ];
                                let ratio = [
                                    mixture[0].amount,
                                    mixture[1].amount,
                                    mixture[2].amount,
                                    mixture[3].amount,
                                ];

                                (tubes, Vector4::new(ratio[0], ratio[1], ratio[2], ratio[3]))
                            })
                            .collect_vec();
                        mix_candidates.push(candidates);
                    }

                    let mut hash = vec![];
                    let mut rng = Pcg64Mcg::from_entropy();

                    for _ in 0..palette.tanks.len() {
                        let mut h = vec![];

                        for _ in 0..=200 {
                            h.push(rng.gen::<u32>() << 12);
                        }

                        hash.push(h);
                    }

                    Self {
                        input: input.clone(),
                        palette,
                        amounts,
                        units,
                        mix_candidates,
                        hash,
                        error_sum,
                        discard_sum,
                        excess_paint,
                        target_i,
                    }
                }

                fn init(input: &Input, palette: Palette) -> Self {
                    let amounts = vec![0.0; input.num_tubes];
                    let units = vec![0; input.num_tubes];

                    Self::new(input, palette, amounts, units, 0.0, 0.0, 0, 0)
                }
            }

            impl crate::beam::single_turn::State for State {
                type Evaluator = Evaluator;
                type Hash = BeamHash;
                type Action = Action;

                fn make_initial_node(&self) -> (Self::Evaluator, Self::Hash) {
                    (Evaluator { score: 0 }, BeamHash { hash: 0 })
                }

                fn expand(
                    &mut self,
                    _evaluator: &Self::Evaluator,
                    hash: Self::Hash,
                    candidate_set: &mut impl crate::beam::single_turn::CandidateSet<
                        Action = Self::Action,
                        Evaluator = Self::Evaluator,
                        Hash = Self::Hash,
                    >,
                ) {
                    let target_color = self.input.target_colors[self.target_i];

                    for (tubes, mix_ratio) in self.mix_candidates[self.target_i].iter() {
                        let mut tank_ops = [[(TankOp::default(), 0.0); 2]; 4];

                        for (op_i, (&tube, &ratio)) in
                            tubes.iter().zip(mix_ratio.iter()).enumerate()
                        {
                            let tank = &self.palette.tanks[tube];
                            let prev_unit = self.units[tube];

                            // 0除算に注意
                            let prev_amount = self.amounts[tube].max(1e-15);

                            let mut amount = prev_amount;
                            let mut add_paint = false;

                            // 塗料が足りていないなら追加
                            if amount < ratio + Input::EPS {
                                amount += 1.0;
                                add_paint = true;
                            }

                            let needed_frac = (ratio / amount).max(0.0);

                            // 分数を全探索
                            let mut numerator_l = !0;
                            let mut denominator_l = !0;
                            let mut numerator_r = !0;
                            let mut denominator_r = !0;
                            let mut best_diff_l = f64::MAX;
                            let mut best_diff_r = f64::MAX;

                            for denominator in prev_unit.max(1)..=tank.capacity {
                                let inv_d = 1.0 / denominator as f64;
                                let l = (denominator as f64 * needed_frac) as usize;
                                let r = (l + 1).min(denominator);
                                let diff_l = needed_frac - l as f64 * inv_d;
                                let diff_r = r as f64 * inv_d - needed_frac;

                                if diff_l < best_diff_l {
                                    numerator_l = l;
                                    denominator_l = denominator;
                                    best_diff_l = diff_l;
                                }

                                if diff_r < best_diff_r {
                                    numerator_r = r;
                                    denominator_r = denominator;
                                    best_diff_r = diff_r;
                                }
                            }

                            let amount_l = amount * numerator_l as f64 / denominator_l as f64;
                            let amount_r = amount * numerator_r as f64 / denominator_r as f64;
                            let next_amount_l = amount - amount_l;
                            let next_amount_r = amount - amount_r;

                            tank_ops[op_i][0] = (
                                TankOp::new(
                                    tube,
                                    prev_unit,
                                    prev_amount,
                                    denominator_l,
                                    denominator_l - numerator_l,
                                    next_amount_l,
                                    add_paint,
                                ),
                                amount_l,
                            );

                            tank_ops[op_i][1] = (
                                TankOp::new(
                                    tube,
                                    prev_unit,
                                    prev_amount,
                                    denominator_r,
                                    denominator_r - numerator_r,
                                    next_amount_r,
                                    add_paint,
                                ),
                                amount_r,
                            );
                        }

                        // bit全探索
                        for flag in 0..1 << 4 {
                            let mut paints = [Paint::default(); 4];
                            let mut current_ops = [TankOp::default(); 4];
                            let mut total_amount = 0.0;
                            let mut total_add_cnt = 0;

                            // まず足りているかどうかをチェック
                            for i in 0..4 {
                                let amount = tank_ops[i][(flag >> i) & 1].1;
                                total_amount += amount;
                            }

                            if total_amount <= 1.0 - Input::EPS {
                                continue;
                            }

                            let mut new_hash = hash;

                            for i in 0..4 {
                                let (tank_op, amount) = tank_ops[i][(flag >> i) & 1];
                                current_ops[i] = tank_op;
                                paints[i] =
                                    Paint::new(self.input.tube_colors[tank_op.tube()], amount);

                                if tank_op.add_paint_flag {
                                    total_add_cnt += 1;
                                    new_hash = new_hash.update_add(1);
                                }

                                new_hash = new_hash.update_tank_amount(
                                    &self,
                                    tank_op.tube(),
                                    tank_op.prev_amount,
                                    tank_op.next_amount,
                                );
                            }

                            let new_paint = Paint::mix_all(&paints);
                            let error = new_paint.color.error(&target_color);
                            let new_error_sum = self.error_sum + error;
                            let new_excess_paint = self.excess_paint + total_add_cnt - 1;
                            let new_discard_sum = self.discard_sum + (new_paint.amount - 1.0);

                            let evaluator = Evaluator::new(
                                &self.input,
                                new_error_sum,
                                new_excess_paint,
                                new_discard_sum,
                                self.target_i,
                            );

                            let action = Action::new(
                                current_ops,
                                error,
                                new_paint.amount - 1.0,
                                total_add_cnt as isize - 1,
                            );

                            candidate_set.push(action, evaluator, new_hash, false);
                        }
                    }
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Evaluator {
                score: u64,
            }

            impl Evaluator {
                fn new(
                    input: &Input,
                    error_sum: f64,
                    excess_paint: usize,
                    discard_sum: f64,
                    target_i: usize,
                ) -> Self {
                    let error_penalty = error_sum * Input::ERROR_COEF;
                    let paint_penalty = (excess_paint as u64 * input.cost_per_gram) as f64;
                    let discard_penalty = discard_sum * input.cost_per_gram as f64;

                    const START_TURN: usize = 985;
                    let t = if target_i < START_TURN {
                        0.0
                    } else {
                        (target_i - START_TURN) as f64
                            / (Input::NUM_TARGETS - START_TURN - 1) as f64
                    };

                    let score =
                        ((error_penalty + paint_penalty * t + discard_penalty * (1.0 - t) + 1.0)
                            * 1e6) as u64;

                    Self { score }
                }
            }

            impl crate::beam::single_turn::Evaluator for Evaluator {
                type Cost = u64;

                fn evaluate(&self) -> Self::Cost {
                    self.score
                }
            }

            #[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
            struct BeamHash {
                hash: u32,
            }

            impl BeamHash {
                fn update_add(&self, add_cnt: usize) -> Self {
                    let hash = self.hash + add_cnt as u32;
                    Self { hash }
                }

                fn update_tank_amount(
                    &self,
                    state: &State,
                    tank_i: usize,
                    old_amount: f64,
                    new_amount: f64,
                ) -> Self {
                    let mut hash = self.hash;
                    let hashes = &state.hash[tank_i];
                    hash ^= hashes[(old_amount * 10.0) as usize];
                    hash ^= hashes[(new_amount * 10.0) as usize];
                    Self { hash }
                }
            }

            impl crate::beam::single_turn::BeamHash for BeamHash {}

            impl Into<u64> for BeamHash {
                fn into(self) -> u64 {
                    self.hash as u64
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Action {
                ops: [TankOp; 4],
                error_diff: f64,
                discard_diff: f64,
                excess_diff: isize,
                action_id: u32,
            }

            static mut ACTION_ID: u32 = 0;

            impl Action {
                fn new(
                    ops: [TankOp; 4],
                    error_diff: f64,
                    discard_diff: f64,
                    excess_diff: isize,
                ) -> Self {
                    unsafe {
                        let action_id = ACTION_ID;
                        ACTION_ID += 1;

                        Self {
                            ops,
                            error_diff,
                            discard_diff,
                            excess_diff,
                            action_id,
                        }
                    }
                }
            }

            impl PartialEq for Action {
                fn eq(&self, other: &Self) -> bool {
                    self.action_id == other.action_id
                }
            }

            impl Eq for Action {}

            #[derive(Debug, Clone, Copy, Default)]
            struct TankOp {
                /// どのチューブを使うか
                tube: u8,
                /// 元々のユニット数
                prev_unit: u8,
                /// 元々の量
                prev_amount: f64,
                /// 操作途中のユニット数
                mid_unit: u8,
                /// 次のユニット数
                next_unit: u8,
                /// 使用後の量
                next_amount: f64,
                /// 追加の有無
                add_paint_flag: bool,
            }

            impl TankOp {
                fn new(
                    tube: usize,
                    prev_unit: usize,
                    prev_amount: f64,
                    mid_unit: usize,
                    next_unit: usize,
                    next_amount: f64,
                    add_paint_flag: bool,
                ) -> Self {
                    let tube = tube as u8;
                    let prev_unit = prev_unit as u8;
                    let mid_unit = mid_unit as u8;
                    let next_unit = next_unit as u8;

                    Self {
                        tube,
                        prev_unit,
                        prev_amount,
                        mid_unit,
                        next_unit,
                        next_amount,
                        add_paint_flag,
                    }
                }

                fn tube(&self) -> usize {
                    self.tube as usize
                }
            }

            impl crate::beam::single_turn::Action for Action {
                type State = State;

                fn apply(&self, state: &mut Self::State) {
                    for op in self.ops.iter() {
                        state.amounts[op.tube()] = op.next_amount;
                        state.units[op.tube()] = op.next_unit as usize;
                    }

                    state.error_sum += self.error_diff;
                    state.discard_sum += self.discard_diff;
                    state.excess_paint = state.excess_paint.wrapping_add_signed(self.excess_diff);
                    state.target_i += 1;
                }

                fn rollback(&self, state: &mut Self::State) {
                    for op in self.ops.iter() {
                        state.amounts[op.tube()] = op.prev_amount;
                        state.units[op.tube()] = op.prev_unit as usize;
                    }

                    state.error_sum -= self.error_diff;
                    state.discard_sum -= self.discard_diff;
                    state.excess_paint = state.excess_paint.wrapping_add_signed(-self.excess_diff);
                    state.target_i -= 1;
                }
            }
        }
        pub mod half_beam {
            use super::super::single_lane_palette::Palette;
            use crate::{
                beam::{self, single_turn::Action as _},
                problem::{Input, Op, Output, Paint},
            };
            use itertools::Itertools;
            use nalgebra::Vector4;
            use rand::{Rng as _, SeedableRng as _};
            use rand_pcg::Pcg64Mcg;

            pub const ESTIMATED_OP_PER_TURN: usize = 13;
            const ANNEALING_START_TURN: usize = 980;
            const AMOUNT_FACTOR: f64 = 1.0 + 1e-2;

            pub fn run(input: &Input, palette: Palette, duration_sec: f64) -> BeamResult {
                let state = State::init(input, palette.clone());
                let beam_width_suggester = beam::common::BayesianBeamWidthSuggester::new(
                    Input::NUM_TARGETS,
                    10,
                    duration_sec,
                    ((10.0 * duration_sec) as usize).max(10),
                    1,
                    1000,
                    None,
                );
                let beam_search = beam::single_turn::BeamSearch::<_, 16001>::new(
                    beam_width_suggester,
                    Input::NUM_TARGETS,
                );
                let (beam_actions, _evaluator) =
                    beam_search.run_default(state.clone()).ok().unwrap();

                let result = restore_ops(input, palette.clone(), state.clone(), &beam_actions);

                let BeamResult::Completed(beam_output) = result else {
                    return result;
                };

                // 焼きなましと比較
                let annealing_result = run_annealing(input, palette, state, beam_actions);

                match annealing_result {
                    BeamResult::Completed(annealing_output) => {
                        eprintln!("beam only : {}", beam_output.score);
                        eprintln!("annealing : {}", annealing_output.score);

                        if annealing_output.score < beam_output.score {
                            BeamResult::Completed(annealing_output)
                        } else {
                            BeamResult::Completed(beam_output)
                        }
                    }
                    BeamResult::Incomplete(_) => BeamResult::Completed(beam_output),
                }
            }

            pub fn run_resume(
                input: &Input,
                palette: Palette,
                amounts: Vec<f64>,
                units: Vec<usize>,
                error_sum: f64,
                discard_sum: f64,
                excess_paint: usize,
                target_i: usize,
                duration_sec: f64,
            ) -> Vec<(Vec<Op>, f64)> {
                let state = State::new(
                    input,
                    palette.clone(),
                    amounts,
                    units,
                    error_sum,
                    discard_sum,
                    excess_paint,
                    target_i,
                );

                let turn = Input::NUM_TARGETS - target_i;

                let beam_width_suggester = beam::common::BayesianBeamWidthSuggester::new(
                    turn,
                    10,
                    duration_sec,
                    ((10.0 * duration_sec) as usize).max(10),
                    1,
                    1000,
                    None,
                );
                let beam_search =
                    beam::single_turn::BeamSearch::<_, 16001>::new(beam_width_suggester, turn);
                let (beam_actions, _evaluator) =
                    beam_search.run_default(state.clone()).ok().unwrap();

                let BeamResult::Completed(result) =
                    restore_ops(input, palette.clone(), state.clone(), &beam_actions)
                else {
                    panic!("Expected completed result, got incomplete");
                };

                let ops = result.operations;
                let error_sum = result.total_error;

                let mut candidates = vec![(ops, error_sum)];

                // 焼きなましと比較
                let annealing_result = run_annealing(input, palette, state, beam_actions);

                if let BeamResult::Completed(annealing_output) = annealing_result {
                    let ops = annealing_output.operations;
                    let error_sum = annealing_output.total_error;
                    candidates.push((ops, error_sum));
                }

                candidates
            }

            fn run_annealing(
                input: &Input,
                palette: Palette,
                state: State,
                beam_actions: Vec<Action>,
            ) -> BeamResult {
                let mut state_for_annealing = state.clone();
                let mut annealing_actions = vec![];

                for &action in beam_actions.iter() {
                    if state_for_annealing.target_i >= ANNEALING_START_TURN {
                        break;
                    }

                    annealing_actions.push(action);
                    action.apply(&mut state_for_annealing);
                }

                let amounts = state_for_annealing.amounts.clone();
                let recipes = crate::solver::annealing::run(
                    input,
                    state_for_annealing.target_i,
                    &amounts,
                    AMOUNT_FACTOR,
                );

                for (i, &recipe) in recipes.iter().enumerate() {
                    let tubes = [
                        recipe[0].index,
                        recipe[1].index,
                        recipe[2].index,
                        recipe[3].index,
                    ];
                    let amounts = Vector4::new(
                        recipe[0].amount,
                        recipe[1].amount,
                        recipe[2].amount,
                        recipe[3].amount,
                    );

                    state_for_annealing.mix_candidates[state_for_annealing.target_i + i] =
                        vec![(tubes, amounts)];
                }

                let turns = Input::NUM_TARGETS - state_for_annealing.target_i;
                let beam_width_suggester =
                    beam::common::BayesianBeamWidthSuggester::new(turns, 5, 0.1, 10, 1, 1000, None);
                let beam_search =
                    beam::single_turn::BeamSearch::<_, 16001>::new(beam_width_suggester, turns);
                let (actions, _evaluator) = beam_search
                    .run_default(state_for_annealing.clone())
                    .ok()
                    .unwrap();

                annealing_actions.extend(actions);
                let annealing_result = restore_ops(input, palette, state, &annealing_actions);

                annealing_result
            }

            fn restore_ops(
                input: &Input,
                palette: Palette,
                mut state: State,
                actions: &[Action],
            ) -> BeamResult {
                let mut ops = vec![];

                for action in actions.iter() {
                    // 100ターンバッファを設ける
                    let remaining_turn = input.max_turns - ops.len();
                    let min_threshold_turn =
                        crate::solver::fixed_palette::beam::ESTIMATED_OP_PER_TURN
                            * (Input::NUM_TARGETS - state.target_i)
                            + 100;

                    // 残りターンが少ない場合は中断
                    if state.target_i < 995 && remaining_turn < min_threshold_turn {
                        let mut paints = vec![];

                        for i in 0..input.num_tubes {
                            let paint = Paint::new(input.tube_colors[i], state.amounts[i]);
                            paints.push(paint);
                        }

                        // 仕切りを元に戻す
                        for i in 0..input.num_tubes {
                            let tank = &palette.tanks[i];

                            if state.units[i] < tank.capacity {
                                if let Some(tank_op) = tank.toggle_ops[tank.capacity] {
                                    ops.push(tank_op);
                                }

                                if let Some(tank_op) = tank.toggle_ops[state.units[i]] {
                                    ops.push(tank_op);
                                }
                            }
                        }

                        return BeamResult::Incomplete(IncompleteResult::new(
                            ops,
                            state.target_i,
                            palette,
                            paints,
                            state.error_sum,
                            state.excess_paint,
                        ));
                    }

                    // 前準備
                    for op in action.ops.iter() {
                        // 分母の設定
                        let tube_i = op.tube();
                        let tank = &palette.tanks[tube_i];

                        if op.prev_unit != op.mid_unit {
                            if let Some(op) = tank.toggle_ops[op.mid_unit as usize] {
                                ops.push(op);
                            }

                            if let Some(op) = tank.toggle_ops[op.prev_unit as usize] {
                                ops.push(op);
                            }
                        }

                        // 塗料の追加
                        if op.add_paint_flag {
                            ops.push(Op::AddPaint(tank.coords[0], tube_i));
                        }
                    }

                    // 塗料の混合
                    for op in action.ops.iter() {
                        // 分母の設定
                        let tube_i = op.tube();
                        let tank = &palette.tanks[tube_i];

                        if op.mid_unit != op.next_unit {
                            if let Some(op) = tank.toggle_ops[op.next_unit as usize] {
                                ops.push(op);
                            }

                            if let Some(op) = tank.toggle_ops[op.mid_unit as usize] {
                                ops.push(op);
                            }
                        }
                    }

                    ops.push(Op::DeliverPaint(palette.deliver_coord));
                    ops.push(Op::DiscardPaint(palette.deliver_coord));
                    action.apply(&mut state);
                }

                let output = Output::new(input, palette.dividers, ops, state.error_sum);
                BeamResult::Completed(output)
            }

            pub enum BeamResult {
                Completed(Output),
                Incomplete(IncompleteResult),
            }

            pub struct IncompleteResult {
                pub ops: Vec<Op>,
                pub target_i: usize,
                pub palette: Palette,
                pub paints: Vec<Paint>,
                pub total_error: f64,
                pub excess_paint: usize,
            }

            impl IncompleteResult {
                pub fn new(
                    ops: Vec<Op>,
                    target_i: usize,
                    palette: Palette,
                    paints: Vec<Paint>,
                    total_error: f64,
                    excess_paint: usize,
                ) -> Self {
                    Self {
                        ops,
                        target_i,
                        palette,
                        paints,
                        total_error,
                        excess_paint,
                    }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                input: Input,
                palette: Palette,
                amounts: Vec<f64>,
                units: Vec<usize>,
                mix_candidates: Vec<Vec<([usize; 4], Vector4<f64>)>>,
                hash: Vec<Vec<u32>>,
                error_sum: f64,
                discard_sum: f64,
                excess_paint: usize,
                target_i: usize,
            }

            impl State {
                fn new(
                    input: &Input,
                    palette: Palette,
                    amounts: Vec<f64>,
                    units: Vec<usize>,
                    error_sum: f64,
                    discard_sum: f64,
                    excess_paint: usize,
                    target_i: usize,
                ) -> Self {
                    const MIX_CANDIDATE_CNT: usize = 200;
                    let mut mix_candidates = vec![];

                    for mixtures in input.mixtures.mixtures_volume_ordered.iter() {
                        let candidates = mixtures
                            .iter()
                            .take(MIX_CANDIDATE_CNT)
                            .map(|mixture| {
                                let tubes = [
                                    mixture[0].index,
                                    mixture[1].index,
                                    mixture[2].index,
                                    mixture[3].index,
                                ];
                                let ratio = [
                                    mixture[0].amount,
                                    mixture[1].amount,
                                    mixture[2].amount,
                                    mixture[3].amount,
                                ];

                                (tubes, Vector4::new(ratio[0], ratio[1], ratio[2], ratio[3]))
                            })
                            .collect_vec();
                        mix_candidates.push(candidates);
                    }

                    let mut hash = vec![];
                    let mut rng = Pcg64Mcg::from_entropy();

                    for _ in 0..palette.tanks.len() {
                        let mut h = vec![];

                        for _ in 0..=200 {
                            h.push(rng.gen::<u32>() << 12);
                        }

                        hash.push(h);
                    }

                    Self {
                        input: input.clone(),
                        palette,
                        amounts,
                        units,
                        mix_candidates,
                        hash,
                        error_sum,
                        discard_sum,
                        excess_paint,
                        target_i,
                    }
                }

                fn init(input: &Input, palette: Palette) -> Self {
                    let amounts = vec![0.0; input.num_tubes];
                    let units = vec![0; input.num_tubes];

                    Self::new(input, palette, amounts, units, 0.0, 0.0, 0, 0)
                }
            }

            impl crate::beam::single_turn::State for State {
                type Evaluator = Evaluator;
                type Hash = BeamHash;
                type Action = Action;

                fn make_initial_node(&self) -> (Self::Evaluator, Self::Hash) {
                    (Evaluator { score: 0 }, BeamHash { hash: 0 })
                }

                fn expand(
                    &mut self,
                    _evaluator: &Self::Evaluator,
                    hash: Self::Hash,
                    candidate_set: &mut impl crate::beam::single_turn::CandidateSet<
                        Action = Self::Action,
                        Evaluator = Self::Evaluator,
                        Hash = Self::Hash,
                    >,
                ) {
                    let target_color = self.input.target_colors[self.target_i];

                    for (tubes, mix_ratio) in self.mix_candidates[self.target_i].iter() {
                        let mut tank_ops = [[(TankOp::default(), 0.0); 2]; 4];

                        for (op_i, (&tube, &ratio)) in
                            tubes.iter().zip(mix_ratio.iter()).enumerate()
                        {
                            let tank = &self.palette.tanks[tube];
                            let prev_unit = self.units[tube];

                            // 0除算に注意
                            let prev_amount = self.amounts[tube].max(1e-15);

                            let mut amount = prev_amount;
                            let mut add_paint = false;

                            // 塗料が足りていないなら追加
                            // 追加したときは分数計算してOK
                            if amount < ratio + Input::EPS {
                                amount += 1.0;
                                add_paint = true;

                                let needed_frac = (ratio / amount).max(0.0);

                                // 分数を全探索
                                // 容量の9割以上とする
                                const MIN_CAPACITY_RATIO: f64 = 0.9;
                                let mut numerator_l = !0;
                                let mut denominator_l = !0;
                                let mut numerator_r = !0;
                                let mut denominator_r = !0;
                                let mut best_diff_l = f64::MAX;
                                let mut best_diff_r = f64::MAX;
                                let min_denominator =
                                    prev_unit
                                        .max(1)
                                        .max((tank.capacity as f64 * MIN_CAPACITY_RATIO).ceil()
                                            as usize);

                                for denominator in min_denominator..=tank.capacity {
                                    let inv_d = 1.0 / denominator as f64;
                                    let l = (denominator as f64 * needed_frac) as usize;
                                    let r = (l + 1).min(denominator);
                                    let diff_l = needed_frac - l as f64 * inv_d;
                                    let diff_r = r as f64 * inv_d - needed_frac;

                                    if diff_l < best_diff_l {
                                        numerator_l = l;
                                        denominator_l = denominator;
                                        best_diff_l = diff_l;
                                    }

                                    if diff_r < best_diff_r {
                                        numerator_r = r;
                                        denominator_r = denominator;
                                        best_diff_r = diff_r;
                                    }
                                }

                                let amount_l = amount * numerator_l as f64 / denominator_l as f64;
                                let amount_r = amount * numerator_r as f64 / denominator_r as f64;
                                let next_amount_l = amount - amount_l;
                                let next_amount_r = amount - amount_r;

                                tank_ops[op_i][0] = (
                                    TankOp::new(
                                        tube,
                                        prev_unit,
                                        prev_amount,
                                        denominator_l,
                                        denominator_l - numerator_l,
                                        next_amount_l,
                                        add_paint,
                                    ),
                                    amount_l,
                                );

                                tank_ops[op_i][1] = (
                                    TankOp::new(
                                        tube,
                                        prev_unit,
                                        prev_amount,
                                        denominator_r,
                                        denominator_r - numerator_r,
                                        next_amount_r,
                                        add_paint,
                                    ),
                                    amount_r,
                                );
                            } else {
                                // シンプルに出荷するだけ
                                let each_amount = amount / prev_unit.max(1) as f64;
                                let units_l = (ratio / each_amount) as usize;
                                let units_r = units_l + 1;
                                let amount_l = each_amount * units_l as f64;
                                let amount_r = each_amount * units_r as f64;

                                let next_amount_l = amount - amount_l;
                                let next_amount_r = amount - amount_r;

                                tank_ops[op_i][0] = (
                                    TankOp::new(
                                        tube,
                                        prev_unit,
                                        prev_amount,
                                        prev_unit,
                                        prev_unit - units_l,
                                        next_amount_l,
                                        add_paint,
                                    ),
                                    amount_l,
                                );

                                tank_ops[op_i][1] = (
                                    TankOp::new(
                                        tube,
                                        prev_unit,
                                        prev_amount,
                                        prev_unit,
                                        prev_unit - units_r,
                                        next_amount_r,
                                        add_paint,
                                    ),
                                    amount_r,
                                );
                            }
                        }

                        // bit全探索
                        for flag in 0..1 << 4 {
                            let mut paints = [Paint::default(); 4];
                            let mut current_ops = [TankOp::default(); 4];
                            let mut total_amount = 0.0;
                            let mut total_add_cnt = 0;

                            // まず足りているかどうかをチェック
                            for i in 0..4 {
                                let amount = tank_ops[i][(flag >> i) & 1].1;
                                total_amount += amount;
                            }

                            if total_amount <= 1.0 - Input::EPS {
                                continue;
                            }

                            let mut new_hash = hash;

                            for i in 0..4 {
                                let (tank_op, amount) = tank_ops[i][(flag >> i) & 1];
                                current_ops[i] = tank_op;
                                paints[i] =
                                    Paint::new(self.input.tube_colors[tank_op.tube()], amount);

                                if tank_op.add_paint_flag {
                                    total_add_cnt += 1;
                                    new_hash = new_hash.update_add(1);
                                }

                                new_hash = new_hash.update_tank_amount(
                                    &self,
                                    tank_op.tube(),
                                    tank_op.prev_amount,
                                    tank_op.next_amount,
                                );
                            }

                            let new_paint = Paint::mix_all(&paints);
                            let error = new_paint.color.error(&target_color);
                            let new_error_sum = self.error_sum + error;
                            let new_excess_paint = self.excess_paint + total_add_cnt - 1;
                            let new_discard_sum = self.discard_sum + (new_paint.amount - 1.0);

                            let evaluator = Evaluator::new(
                                &self.input,
                                new_error_sum,
                                new_excess_paint,
                                new_discard_sum,
                                self.target_i,
                            );

                            let action = Action::new(
                                current_ops,
                                error,
                                new_paint.amount - 1.0,
                                total_add_cnt as isize - 1,
                            );

                            candidate_set.push(action, evaluator, new_hash, false);
                        }
                    }
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Evaluator {
                score: u64,
            }

            impl Evaluator {
                fn new(
                    input: &Input,
                    error_sum: f64,
                    excess_paint: usize,
                    discard_sum: f64,
                    target_i: usize,
                ) -> Self {
                    let error_penalty = error_sum * Input::ERROR_COEF;
                    let paint_penalty = (excess_paint as u64 * input.cost_per_gram) as f64;
                    let discard_penalty = discard_sum * input.cost_per_gram as f64;

                    const START_TURN: usize = 985;
                    let t = if target_i < START_TURN {
                        0.0
                    } else {
                        (target_i - START_TURN) as f64
                            / (Input::NUM_TARGETS - START_TURN - 1) as f64
                    };

                    let score =
                        ((error_penalty + paint_penalty * t + discard_penalty * (1.0 - t) + 1.0)
                            * 1e6) as u64;

                    Self { score }
                }
            }

            impl crate::beam::single_turn::Evaluator for Evaluator {
                type Cost = u64;

                fn evaluate(&self) -> Self::Cost {
                    self.score
                }
            }

            #[derive(Debug, Clone, Copy, Default, PartialEq, Eq, Hash)]
            struct BeamHash {
                hash: u32,
            }

            impl BeamHash {
                fn update_add(&self, add_cnt: usize) -> Self {
                    let hash = self.hash + add_cnt as u32;
                    Self { hash }
                }

                fn update_tank_amount(
                    &self,
                    state: &State,
                    tank_i: usize,
                    old_amount: f64,
                    new_amount: f64,
                ) -> Self {
                    let mut hash = self.hash;
                    let hashes = &state.hash[tank_i];
                    hash ^= hashes[(old_amount * 20.0) as usize];
                    hash ^= hashes[(new_amount * 20.0) as usize];
                    Self { hash }
                }
            }

            impl crate::beam::single_turn::BeamHash for BeamHash {}

            impl Into<u64> for BeamHash {
                fn into(self) -> u64 {
                    self.hash as u64
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct Action {
                ops: [TankOp; 4],
                error_diff: f64,
                discard_diff: f64,
                excess_diff: isize,
                action_id: u32,
            }

            static mut ACTION_ID: u32 = 0;

            impl Action {
                fn new(
                    ops: [TankOp; 4],
                    error_diff: f64,
                    discard_diff: f64,
                    excess_diff: isize,
                ) -> Self {
                    unsafe {
                        let action_id = ACTION_ID;
                        ACTION_ID += 1;

                        Self {
                            ops,
                            error_diff,
                            discard_diff,
                            excess_diff,
                            action_id,
                        }
                    }
                }
            }

            impl PartialEq for Action {
                fn eq(&self, other: &Self) -> bool {
                    self.action_id == other.action_id
                }
            }

            impl Eq for Action {}

            #[derive(Debug, Clone, Copy, Default)]
            struct TankOp {
                /// どのチューブを使うか
                tube: u8,
                /// 元々のユニット数
                prev_unit: u8,
                /// 元々の量
                prev_amount: f64,
                /// 操作途中のユニット数
                mid_unit: u8,
                /// 次のユニット数
                next_unit: u8,
                /// 使用後の量
                next_amount: f64,
                /// 追加の有無
                add_paint_flag: bool,
            }

            impl TankOp {
                fn new(
                    tube: usize,
                    prev_unit: usize,
                    prev_amount: f64,
                    mid_unit: usize,
                    next_unit: usize,
                    next_amount: f64,
                    add_paint_flag: bool,
                ) -> Self {
                    let tube = tube as u8;
                    let prev_unit = prev_unit as u8;
                    let mid_unit = mid_unit as u8;
                    let next_unit = next_unit as u8;

                    Self {
                        tube,
                        prev_unit,
                        prev_amount,
                        mid_unit,
                        next_unit,
                        next_amount,
                        add_paint_flag,
                    }
                }

                fn tube(&self) -> usize {
                    self.tube as usize
                }
            }

            impl crate::beam::single_turn::Action for Action {
                type State = State;

                fn apply(&self, state: &mut Self::State) {
                    for op in self.ops.iter() {
                        state.amounts[op.tube()] = op.next_amount;
                        state.units[op.tube()] = op.next_unit as usize;
                    }

                    state.error_sum += self.error_diff;
                    state.discard_sum += self.discard_diff;
                    state.excess_paint = state.excess_paint.wrapping_add_signed(self.excess_diff);
                    state.target_i += 1;
                }

                fn rollback(&self, state: &mut Self::State) {
                    for op in self.ops.iter() {
                        state.amounts[op.tube()] = op.prev_amount;
                        state.units[op.tube()] = op.prev_unit as usize;
                    }

                    state.error_sum -= self.error_diff;
                    state.discard_sum -= self.discard_diff;
                    state.excess_paint = state.excess_paint.wrapping_add_signed(-self.excess_diff);
                    state.target_i -= 1;
                }
            }
        }
    }
    mod single_lane_palette {
        use itertools::Itertools;

        use crate::{
            grid::{ConstMap2d, Coord},
            problem::{Dividers, Input, Op},
            solver::tube_map::TubeMap,
        };

        pub fn crate_palette(input: &Input) -> Palette {
            let tube_map = TubeMap::new(input.num_tubes);
            let mut dividers =
                Dividers::new(ConstMap2d::with_default(), ConstMap2d::with_default());

            for row in 0..Input::PALLETTE_SIZE {
                for col in 0..Input::PALLETTE_SIZE - 1 {
                    let c = Coord::new(row, col);
                    dividers.vertical[c] = true;
                }
            }

            for row in 0..Input::PALLETTE_SIZE - 1 {
                for col in 0..Input::PALLETTE_SIZE {
                    let c = Coord::new(row, col);
                    dividers.horizontal[c] = true;
                }
            }

            let mut tubes = vec![];

            for tube_i in 0..input.num_tubes {
                let mut coords = vec![tube_map.tubes[tube_i][0]];
                let mut ops = vec![Some(tube_map.open_ops[tube_i])];

                for i in 1..tube_map.tubes[tube_i].len() {
                    let mut c = tube_map.tubes[tube_i][i];
                    let mut prev_c = tube_map.tubes[tube_i][i - 1];
                    coords.push(c);
                    ops.push(Some(Op::ToggleDivider(c, prev_c)));

                    if c > prev_c {
                        std::mem::swap(&mut c, &mut prev_c);
                    }

                    if c.row() + 1 == prev_c.row() && c.col() == prev_c.col() {
                        dividers.horizontal[c] = false;
                    } else if c.col() + 1 == prev_c.col() && c.row() == prev_c.row() {
                        dividers.vertical[c] = false;
                    } else {
                        panic!("Invalid divider toggle: {} and {}", c, prev_c);
                    }
                }

                ops.push(None);
                coords.reverse();
                ops.reverse();

                // 出口を開ける
                let Op::ToggleDivider(mut c0, mut c1) = tube_map.open_ops[tube_i] else {
                    panic!("Invalid open operation for tube {}", tube_i);
                };

                if c0 > c1 {
                    std::mem::swap(&mut c0, &mut c1);
                }

                if c0.row() + 1 == c1.row() && c0.col() == c1.col() {
                    dividers.horizontal[c0] = false;
                } else if c0.col() + 1 == c1.col() && c0.row() == c1.row() {
                    dividers.vertical[c0] = false;
                } else {
                    panic!("Invalid open operation for tube {}", tube_i);
                }

                let capacity = coords.len();
                let tank = Tank::new(capacity, coords, ops);
                tubes.push(tank);
            }

            for &c in tube_map.mix_area[..tube_map.mix_area.len() - 1].iter() {
                dividers.vertical[c] = false;
            }

            let palette = Palette::new(tube_map.mix_area[0], dividers, tubes);
            palette
        }

        pub fn create_palette_from(
            old_palette: crate::solver::frac_2digit::palette::Palette,
        ) -> Palette {
            let deliver_coord = old_palette.deliver_coord;
            let dividers = old_palette.dividers.clone();
            let tanks = old_palette
                .tanks
                .iter()
                .map(|tank| {
                    let coords = tank.coords.clone();
                    let mut toggle_ops = tank.toggle_ops.iter().map(|&op| Some(op)).collect_vec();
                    toggle_ops[0] = None; // 最初のトグルは無効化
                    Tank::new(tank.capacity, coords, toggle_ops)
                })
                .collect_vec();

            Palette::new(deliver_coord, dividers, tanks)
        }

        #[derive(Debug, Clone)]
        pub struct Palette {
            pub deliver_coord: Coord,
            pub dividers: Dividers,
            pub tanks: Vec<Tank>,
        }

        impl Palette {
            pub fn new(deliver_coord: Coord, dividers: Dividers, tanks: Vec<Tank>) -> Self {
                Self {
                    deliver_coord,
                    tanks,
                    dividers,
                }
            }
        }

        #[derive(Debug, Clone)]
        pub struct Tank {
            pub capacity: usize,
            pub coords: Vec<Coord>,
            pub toggle_ops: Vec<Option<Op>>,
        }

        impl Tank {
            pub fn new(capacity: usize, coords: Vec<Coord>, toggle_ops: Vec<Option<Op>>) -> Self {
                Self {
                    capacity,
                    coords,
                    toggle_ops,
                }
            }
        }
    }
    mod tube_map {
        use itertools::Itertools;

        use crate::{
            grid::{Coord, ADJACENTS},
            problem::{Input, Op},
        };

        pub(super) struct TubeMap {
            pub tubes: Vec<Vec<Coord>>,
            pub mix_area: Vec<Coord>,
            pub open_ops: Vec<Op>,
        }

        impl TubeMap {
            pub fn new(num_tube: usize) -> Self {
                let mut tubes = vec![vec![]; num_tube];
                let mut mix_area = vec![];

                let map = match num_tube {
                    4 => &MAP4,
                    5 => &MAP5,
                    6 => &MAP6,
                    7 => &MAP7,
                    8 => &MAP8,
                    9 => &MAP9,
                    10 => &MAP10,
                    11 => &MAP11,
                    12 => &MAP12,
                    13 => &MAP13,
                    14 => &MAP14,
                    15 => &MAP15,
                    16 => &MAP16,
                    17 => &MAP17,
                    18 => &MAP18,
                    19 => &MAP19,
                    20 => &MAP20,
                    _ => {
                        panic!("Unsupported number of tubes: {}", num_tube);
                    }
                };

                for row in 0..Input::PALLETTE_SIZE {
                    for col in 0..Input::PALLETTE_SIZE {
                        let c = Coord::new(row, col);

                        if map[row][col] >= 0 {
                            let index = (map[row][col] / 100) as usize;
                            let order = (map[row][col] % 100) as usize;
                            tubes[index].push((order, c));
                        } else {
                            mix_area.push(c);
                        }
                    }
                }

                for i in 0..num_tube {
                    tubes[i].sort_unstable();
                }

                let tubes = tubes
                    .into_iter()
                    .map(|tube| tube.into_iter().map(|(_, c)| c).collect_vec())
                    .collect_vec();

                let mut open_ops = vec![];

                for i in 0..num_tube {
                    let c = tubes[i][0];

                    for adj in ADJACENTS {
                        let next = c + adj;

                        if next.in_map(Input::PALLETTE_SIZE) && map[next.row()][next.col()] < 0 {
                            open_ops.push(Op::ToggleDivider(c, next));
                            break;
                        }
                    }
                }

                Self {
                    tubes,
                    mix_area,
                    open_ops,
                }
            }
        }

        const MAP4: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39,
            ],
            [
                17, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
            ],
            [
                16, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77,
            ],
            [
                15, 96, 97, 98, 99, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129,
                130, 131, 132,
            ],
            [
                14, 113, 114, 115, 116, 117, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136,
                135, 134, 133,
            ],
            [
                13, 112, 151, 150, 149, 148, 147, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
                168, 169, 170,
            ],
            [
                12, 111, 152, 153, 154, 155, 156, 157, 182, 181, 180, 179, 178, 177, 176, 175, 174,
                173, 172, 171,
            ],
            [
                11, 110, 189, 188, 187, 186, 185, 184, 183, 198, 218, 219, 220, 221, 222, 223, 224,
                225, 226, 227,
            ],
            [
                10, 109, 190, 191, 192, 193, 194, 195, 196, 197, 217, 236, 235, 234, 233, 232, 231,
                230, 229, 228,
            ],
            [
                9, 108, 208, 209, 210, 211, 212, 213, 214, 215, 216, 237, 238, 239, 240, 241, 242,
                243, 244, 245,
            ],
            [
                8, 107, 207, 262, 261, 260, 259, 258, 257, 256, 255, 254, 253, 252, 251, 250, 249,
                248, 247, 246,
            ],
            [
                7, 106, 206, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276,
                277, 278, 279,
            ],
            [
                6, 105, 205, 296, 295, 294, 293, 292, 291, 290, 289, 288, 287, 286, 285, 284, 283,
                282, 281, 280,
            ],
            [
                5, 104, 204, 297, 298, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318,
                319, 320, 321,
            ],
            [
                4, 103, 203, 304, 305, 306, 335, 334, 333, 332, 331, 330, 329, 328, 327, 326, 325,
                324, 323, 322,
            ],
            [
                3, 102, 202, 303, 338, 337, 336, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352,
                353, 354, 355,
            ],
            [
                2, 101, 201, 302, 339, 340, 341, 342, 367, 366, 365, 364, 363, 362, 361, 360, 359,
                358, 357, 356,
            ],
            [
                1, 100, 200, 301, 372, 371, 370, 369, 368, 379, 380, 381, 382, 383, 384, 385, 386,
                387, 388, 389,
            ],
            [
                0, -1, -1, 300, 373, 374, 375, 376, 377, 378, 399, 398, 397, 396, 395, 394, 393,
                392, 391, 390,
            ],
        ];

        const MAP5: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39,
            ],
            [
                17, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76,
            ],
            [
                16, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
                79, 78, 77,
            ],
            [
                15, 114, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 131,
                132, 133, 134,
            ],
            [
                14, 113, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 139, 138,
                137, 136, 135,
            ],
            [
                13, 112, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 167, 168, 169,
                170, 171, 172,
            ],
            [
                12, 111, 211, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 178, 177, 176,
                175, 174, 173,
            ],
            [
                11, 110, 210, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248,
                249, 250, 251,
            ],
            [
                10, 109, 209, 268, 267, 266, 265, 264, 263, 262, 261, 260, 259, 258, 257, 256, 255,
                254, 253, 252,
            ],
            [
                9, 108, 208, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 318, 319, 320, 321,
                322, 323, 324,
            ],
            [
                8, 107, 207, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 330, 329, 328,
                327, 326, 325,
            ],
            [
                7, 106, 206, 306, 353, 352, 351, 350, 349, 348, 347, 346, 345, 344, 331, 332, 333,
                334, 335, 336,
            ],
            [
                6, 105, 205, 305, 354, 355, 356, 357, 358, 359, 360, 361, 362, 343, 342, 341, 340,
                339, 338, 337,
            ],
            [
                5, 104, 204, 304, 405, 406, 407, 408, 409, 410, 411, 412, 363, 364, 365, 366, 367,
                368, 369, 370,
            ],
            [
                4, 103, 203, 303, 404, 419, 418, 417, 416, 415, 414, 413, 378, 377, 376, 375, 374,
                373, 372, 371,
            ],
            [
                3, 102, 202, 302, 403, 420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431,
                432, 433, 434,
            ],
            [
                2, 101, 201, 301, 402, 449, 448, 447, 446, 445, 444, 443, 442, 441, 440, 439, 438,
                437, 436, 435,
            ],
            [
                1, 100, 200, 300, 401, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461,
                462, 463, 464,
            ],
            [
                0, -1, -1, -1, 400, 479, 478, 477, 476, 475, 474, 473, 472, 471, 470, 469, 468,
                467, 466, 465,
            ],
        ];

        const MAP6: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39,
            ],
            [
                17, 58, 59, 60, 61, 62, 63, 64, 65, 66, 125, 126, 127, 128, 129, 130, 131, 132,
                133, 134,
            ],
            [
                16, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 143, 142, 141, 140, 139, 138,
                137, 136, 135,
            ],
            [
                15, 114, 214, 215, 216, 217, 218, 219, 165, 164, 163, 144, 145, 146, 147, 148, 149,
                150, 151, 152,
            ],
            [
                14, 113, 213, 252, 251, 250, 249, 220, 221, 222, 162, 161, 160, 159, 158, 157, 156,
                155, 154, 153,
            ],
            [
                13, 112, 212, 253, 254, 255, 248, 247, 246, 223, 224, 225, 226, 227, 228, 229, 230,
                231, 232, 233,
            ],
            [
                12, 111, 211, 311, 312, 256, 257, 258, 245, 244, 243, 242, 241, 240, 239, 238, 237,
                236, 235, 234,
            ],
            [
                11, 110, 210, 310, 313, 314, 315, 259, 260, 261, 262, 263, 264, 265, 325, 326, 327,
                328, 329, 330,
            ],
            [
                10, 109, 209, 309, 358, 357, 316, 317, 318, 319, 320, 321, 322, 323, 324, 335, 334,
                333, 332, 331,
            ],
            [
                9, 108, 208, 308, 359, 356, 355, 354, 353, 352, 351, 350, 349, 348, 347, 336, 337,
                338, 339, 340,
            ],
            [
                8, 107, 207, 307, 360, 361, 362, 363, 364, 365, 413, 414, 415, 416, 346, 345, 344,
                343, 342, 341,
            ],
            [
                7, 106, 206, 306, 406, 407, 408, 409, 410, 411, 412, 433, 432, 417, 418, 419, 420,
                421, 422, 423,
            ],
            [
                6, 105, 205, 305, 405, 440, 439, 438, 437, 436, 435, 434, 431, 430, 429, 428, 427,
                426, 425, 424,
            ],
            [
                5, 104, 204, 304, 404, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452,
                453, 454, 455,
            ],
            [
                4, 103, 203, 303, 403, 504, 505, 506, 507, 508, 509, 464, 463, 462, 461, 460, 459,
                458, 457, 456,
            ],
            [
                3, 102, 202, 302, 402, 503, 534, 533, 532, 531, 510, 511, 512, 513, 514, 515, 516,
                517, 518, 519,
            ],
            [
                2, 101, 201, 301, 401, 502, 535, 536, 537, 530, 529, 528, 527, 526, 525, 524, 523,
                522, 521, 520,
            ],
            [
                1, 100, 200, 300, 400, 501, 564, 563, 538, 539, 540, 541, 542, 543, 544, 545, 546,
                547, 548, 549,
            ],
            [
                0, -1, -1, -1, -1, 500, 565, 562, 561, 560, 559, 558, 557, 556, 555, 554, 553, 552,
                551, 550,
            ],
        ];

        const MAP7: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 117, 118, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39,
            ],
            [
                17, 116, 119, 56, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
                136, 137, 138,
            ],
            [
                16, 115, 120, 121, 122, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, 143, 142,
                141, 140, 139,
            ],
            [
                15, 114, 214, 215, 155, 154, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
                231, 232, 233,
            ],
            [
                14, 113, 213, 216, 217, 218, 219, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237,
                236, 235, 234,
            ],
            [
                13, 112, 212, 251, 250, 249, 248, 247, 317, 318, 319, 320, 321, 322, 323, 324, 325,
                326, 327, 328,
            ],
            [
                12, 111, 211, 252, 253, 254, 255, 315, 316, 339, 338, 337, 336, 335, 334, 333, 332,
                331, 330, 329,
            ],
            [
                11, 110, 210, 310, 311, 312, 313, 314, 341, 340, 345, 346, 347, 348, 349, 350, 351,
                352, 353, 354,
            ],
            [
                10, 109, 209, 309, 409, 410, 411, 412, 342, 343, 344, 418, 419, 420, 421, 422, 423,
                424, 425, 355,
            ],
            [
                9, 108, 208, 308, 408, 445, 444, 413, 414, 415, 416, 417, 436, 435, 434, 433, 432,
                431, 426, 427,
            ],
            [
                8, 107, 207, 307, 407, 446, 443, 442, 441, 440, 439, 438, 437, 517, 518, 519, 520,
                430, 429, 428,
            ],
            [
                7, 106, 206, 306, 406, 447, 448, 449, 450, 451, 452, 453, 515, 516, 531, 530, 521,
                522, 523, 524,
            ],
            [
                6, 105, 205, 305, 405, 505, 506, 507, 508, 509, 455, 454, 514, 533, 532, 529, 528,
                527, 526, 525,
            ],
            [
                5, 104, 204, 304, 404, 504, 555, 554, 553, 510, 511, 512, 513, 534, 535, 536, 537,
                538, 539, 540,
            ],
            [
                4, 103, 203, 303, 403, 503, 604, 605, 552, 551, 550, 549, 548, 547, 546, 545, 544,
                543, 542, 541,
            ],
            [
                3, 102, 202, 302, 402, 502, 603, 606, 607, 608, 609, 610, 611, 612, 613, 614, 615,
                616, 617, 618,
            ],
            [
                2, 101, 201, 301, 401, 501, 602, 631, 630, 629, 628, 627, 626, 625, 624, 623, 622,
                621, 620, 619,
            ],
            [
                1, 100, 200, 300, 400, 500, 601, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641,
                642, 643, 644,
            ],
            [
                0, -1, -1, -1, -1, -1, 600, 657, 656, 655, 654, 653, 652, 651, 650, 649, 648, 647,
                646, 645,
            ],
        ];

        const MAP8: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 117, 118, 119, 120, 121, 122, 123, 124, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40,
                39,
            ],
            [
                17, 116, 131, 130, 129, 128, 127, 126, 125, 140, 141, 142, 143, 144, 145, 146, 147,
                148, 232, 233,
            ],
            [
                16, 115, 132, 133, 134, 135, 136, 137, 138, 139, 223, 224, 225, 226, 227, 228, 229,
                230, 231, 234,
            ],
            [
                15, 114, 214, 215, 216, 217, 218, 219, 220, 221, 222, 243, 242, 241, 240, 239, 238,
                237, 236, 235,
            ],
            [
                14, 113, 213, 313, 314, 315, 316, 248, 247, 246, 245, 244, 330, 331, 332, 333, 334,
                335, 336, 337,
            ],
            [
                13, 112, 212, 312, 319, 318, 317, 324, 325, 326, 327, 328, 329, 344, 343, 342, 341,
                340, 339, 338,
            ],
            [
                12, 111, 211, 311, 320, 321, 322, 323, 415, 416, 348, 347, 346, 345, 423, 424, 425,
                426, 427, 428,
            ],
            [
                11, 110, 210, 310, 410, 411, 412, 413, 414, 417, 418, 419, 420, 421, 422, 433, 432,
                431, 430, 429,
            ],
            [
                10, 109, 209, 309, 409, 444, 443, 442, 441, 440, 439, 438, 437, 436, 435, 434, 520,
                521, 522, 523,
            ],
            [
                9, 108, 208, 308, 408, 445, 446, 447, 448, 512, 513, 514, 515, 516, 517, 518, 519,
                526, 525, 524,
            ],
            [
                8, 107, 207, 307, 407, 507, 508, 509, 510, 511, 534, 533, 532, 531, 530, 529, 528,
                527, 619, 620,
            ],
            [
                7, 106, 206, 306, 406, 506, 539, 538, 537, 536, 535, 546, 547, 548, 614, 615, 616,
                617, 618, 621,
            ],
            [
                6, 105, 205, 305, 405, 505, 540, 541, 542, 543, 544, 545, 611, 612, 613, 626, 625,
                624, 623, 622,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 605, 606, 607, 608, 609, 610, 629, 628, 627, 648,
                715, 716, 717,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 636, 635, 634, 633, 632, 631, 630, 645, 646, 647,
                714, 719, 718,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 637, 638, 639, 640, 641, 642, 643, 644, 711, 712,
                713, 720, 721,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 702, 703, 704, 705, 706, 707, 708, 709, 710, 725,
                724, 723, 722,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 701, 734, 733, 732, 731, 730, 729, 728, 727, 726,
                745, 746, 747,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, 700, 735, 736, 737, 738, 739, 740, 741, 742, 743, 744,
                749, 748,
            ],
        ];

        const MAP9: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 44, 43, 42,
                41, 40, 39,
            ],
            [
                17, 116, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 130, 131, 132, 133,
                134, 135, 136,
            ],
            [
                16, 115, 215, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 143, 142, 141, 140,
                139, 138, 137,
            ],
            [
                15, 114, 214, 237, 238, 239, 240, 241, 242, 243, 321, 322, 323, 324, 325, 326, 327,
                328, 329, 330,
            ],
            [
                14, 113, 213, 313, 314, 315, 316, 317, 318, 319, 320, 339, 338, 337, 336, 335, 334,
                333, 332, 331,
            ],
            [
                13, 112, 212, 312, 412, 413, 414, 415, 343, 342, 341, 340, 422, 423, 424, 425, 426,
                427, 428, 429,
            ],
            [
                12, 111, 211, 311, 411, 511, 512, 416, 417, 418, 419, 420, 421, 436, 435, 434, 433,
                432, 431, 430,
            ],
            [
                11, 110, 210, 310, 410, 510, 513, 516, 517, 441, 440, 439, 438, 437, 527, 528, 529,
                530, 531, 532,
            ],
            [
                10, 109, 209, 309, 409, 509, 514, 515, 518, 442, 522, 523, 524, 525, 526, 537, 536,
                535, 534, 533,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 609, 519, 520, 521, 615, 616, 617, 618, 538, 539,
                540, 541, 542,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 610, 611, 612, 613, 614, 633, 632, 619, 620, 621,
                622, 623, 624,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 639, 638, 637, 636, 635, 634, 631, 630, 629, 628,
                627, 626, 625,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 640, 641, 642, 708, 709, 710, 711, 712, 713, 714,
                715, 716, 717,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 705, 706, 707, 726, 725, 724, 723, 722, 721,
                720, 719, 718,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 804, 805, 806, 727, 728, 729, 730, 731, 732,
                733, 734, 735,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 803, 808, 807, 814, 815, 742, 741, 740, 739,
                738, 737, 736,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 802, 809, 812, 813, 816, 817, 818, 819, 820,
                821, 822, 823,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 801, 810, 811, 840, 839, 836, 835, 832, 831,
                828, 827, 824,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, 800, 843, 842, 841, 838, 837, 834, 833, 830, 829,
                826, 825,
            ],
        ];

        const MAP10: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
            ],
            [
                18, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132,
                133, 134, 39,
            ],
            [
                17, 116, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
                231, 135, 136,
            ],
            [
                16, 115, 215, 315, 316, 317, 318, 319, 320, 321, 322, 238, 237, 236, 235, 234, 233,
                232, 138, 137,
            ],
            [
                15, 114, 214, 314, 414, 415, 416, 417, 418, 419, 323, 324, 325, 326, 327, 328, 329,
                330, 331, 332,
            ],
            [
                14, 113, 213, 313, 413, 513, 514, 515, 516, 420, 421, 422, 423, 424, 338, 337, 336,
                335, 334, 333,
            ],
            [
                13, 112, 212, 312, 412, 512, 529, 528, 517, 518, 519, 520, 521, 425, 426, 427, 428,
                429, 430, 431,
            ],
            [
                12, 111, 211, 311, 411, 511, 530, 527, 526, 525, 524, 523, 522, 438, 437, 436, 435,
                434, 433, 432,
            ],
            [
                11, 110, 210, 310, 410, 510, 531, 532, 533, 534, 535, 536, 537, 538, 618, 619, 620,
                621, 622, 623,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 610, 611, 612, 613, 614, 615, 616, 617, 628, 627,
                626, 625, 624,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 709, 710, 711, 712, 713, 714, 715, 629, 630,
                631, 632, 633,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 722, 721, 720, 719, 718, 717, 716, 638, 637,
                636, 635, 634,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 723, 724, 725, 726, 727, 728, 729, 730, 731,
                732, 733, 734,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 806, 807, 808, 809, 810, 811, 812, 738,
                737, 736, 735,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 829, 828, 827, 826, 825, 824, 813, 814,
                815, 816, 817,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 830, 831, 832, 833, 834, 823, 822, 821,
                820, 819, 818,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 903, 904, 905, 906, 835, 836, 837, 838,
                913, 914, 915,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 902, 931, 930, 907, 908, 909, 910, 911,
                912, 917, 916,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 901, 932, 929, 928, 927, 926, 925, 924,
                923, 918, 919,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, 900, 933, 934, 935, 936, 937, 938, 939, 922,
                921, 920,
            ],
        ];

        const MAP11: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 135, 235,
            ],
            [
                18, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132,
                133, 134, 234,
            ],
            [
                17, 116, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
                231, 232, 233,
            ],
            [
                16, 115, 215, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328,
                329, 330, 331,
            ],
            [
                15, 114, 214, 314, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 335,
                334, 333, 332,
            ],
            [
                14, 113, 213, 313, 413, 513, 514, 434, 433, 432, 431, 430, 429, 428, 427, 426, 526,
                527, 528, 529,
            ],
            [
                13, 112, 212, 312, 412, 512, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525,
                532, 531, 530,
            ],
            [
                12, 111, 211, 311, 411, 511, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 534,
                533, 722, 723,
            ],
            [
                11, 110, 210, 310, 410, 510, 610, 629, 628, 627, 626, 625, 624, 623, 622, 621, 719,
                720, 721, 724,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 630, 631, 632, 633, 634, 714, 715, 716, 717, 718,
                727, 726, 725,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 709, 710, 711, 712, 713, 732, 731, 730, 729,
                728, 826, 827,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 807, 808, 809, 810, 734, 733, 821, 822, 823,
                824, 825, 828,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 806, 813, 812, 811, 818, 819, 820, 833, 832,
                831, 830, 829,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 814, 815, 816, 817, 909, 910, 834, 914,
                915, 916, 917,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 904, 905, 906, 907, 908, 911, 912, 913,
                920, 919, 918,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 903, 928, 927, 926, 925, 924, 923, 922,
                921, 1012, 1013,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 929, 930, 931, 932, 933, 934, 1009,
                1010, 1011, 1014,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1002, 1003, 1004, 1005, 1006, 1007,
                1008, 1017, 1016, 1015,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1001, 1024, 1023, 1022, 1021, 1020,
                1019, 1018, 1033, 1034,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1000, 1025, 1026, 1027, 1028, 1029, 1030,
                1031, 1032, 1035,
            ],
        ];

        const MAP12: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 33, 122, 121, 223, 222, 332, 325, 324, 527, 526, 626, 625, 828, 827, 929,
                928, 1130, 1129,
            ],
            [
                18, 21, 32, 123, 120, 224, 221, 331, 326, 323, 528, 525, 627, 624, 829, 826, 930,
                927, 1131, 1128,
            ],
            [
                17, 22, 31, 124, 119, 225, 220, 330, 327, 322, 529, 524, 628, 623, 830, 825, 931,
                926, 1132, 1127,
            ],
            [
                16, 23, 30, 125, 118, 226, 219, 329, 328, 321, 530, 523, 629, 622, 831, 824, 823,
                925, 924, 1126,
            ],
            [
                15, 24, 29, 126, 117, 227, 218, 318, 319, 320, 531, 522, 630, 621, 723, 722, 822,
                922, 923, 1125,
            ],
            [
                14, 25, 28, 127, 116, 228, 217, 317, 431, 432, 532, 521, 631, 620, 724, 721, 821,
                921, 1031, 1124,
            ],
            [
                13, 26, 27, 128, 115, 229, 216, 316, 430, 423, 422, 520, 519, 619, 725, 720, 820,
                920, 1030, 1123,
            ],
            [
                12, 131, 130, 129, 114, 230, 215, 315, 429, 424, 421, 420, 518, 618, 726, 719, 819,
                919, 1029, 1122,
            ],
            [
                11, 110, 111, 112, 113, 231, 214, 314, 428, 425, 418, 419, 517, 617, 727, 718, 818,
                918, 1028, 1121,
            ],
            [
                10, 109, 209, 210, 211, 212, 213, 313, 427, 426, 417, 416, 516, 616, 728, 717, 817,
                917, 1027, 1120,
            ],
            [
                9, 108, 208, 308, 309, 310, 311, 312, 412, 413, 414, 415, 515, 615, 729, 716, 816,
                916, 1026, 1119,
            ],
            [
                8, 107, 207, 307, 407, 408, 409, 410, 411, 511, 512, 513, 514, 614, 730, 715, 815,
                915, 1025, 1118,
            ],
            [
                7, 106, 206, 306, 406, 506, 507, 508, 509, 510, 610, 611, 612, 613, 731, 714, 814,
                914, 1024, 1117,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 606, 607, 608, 609, 709, 710, 711, 712, 713, 813,
                913, 1023, 1116,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 705, 706, 707, 708, 808, 809, 810, 811, 812,
                912, 1022, 1115,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 804, 805, 806, 807, 907, 908, 909, 910,
                911, 1021, 1114,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 903, 904, 905, 906, 1016, 1017,
                1018, 1019, 1020, 1113,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1002, 1003, 1004, 1015, 1014,
                1013, 1012, 1011, 1112,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1101, 1102, 1005, 1006, 1007,
                1008, 1009, 1010, 1111,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1100, 1103, 1104, 1105, 1106, 1107,
                1108, 1109, 1110,
            ],
        ];

        const MAP13: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 129, 229, 329, 429, 529, 629, 729,
                829,
            ],
            [
                18, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 228, 328, 428, 528,
                628, 728, 828,
            ],
            [
                17, 116, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 327, 427, 527,
                627, 727, 827,
            ],
            [
                16, 115, 215, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 426, 526,
                626, 726, 826,
            ],
            [
                15, 114, 214, 314, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 525,
                625, 725, 825,
            ],
            [
                14, 113, 213, 313, 413, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524,
                624, 724, 824,
            ],
            [
                13, 112, 212, 312, 412, 512, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 622,
                623, 723, 823,
            ],
            [
                12, 111, 211, 311, 411, 511, 611, 711, 712, 713, 714, 715, 716, 717, 718, 719, 720,
                721, 722, 822,
            ],
            [
                11, 110, 210, 310, 410, 510, 610, 710, 810, 811, 812, 813, 814, 815, 816, 817, 818,
                819, 820, 821,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 709, 809, 909, 910, 911, 912, 913, 914, 915, 916,
                917, 918, 919,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 808, 908, 929, 928, 927, 926, 925, 924, 923,
                922, 921, 920,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 807, 907, 1007, 1008, 1009, 1010, 1011, 1012,
                1013, 1014, 1015, 1016,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 806, 906, 1006, 1025, 1024, 1023, 1022, 1021,
                1020, 1019, 1018, 1017,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 905, 1005, 1026, 1027, 1028, 1108, 1109,
                1110, 1111, 1112, 1113,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 904, 1004, 1104, 1105, 1106, 1107, 1118,
                1117, 1116, 1115, 1114,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 903, 1003, 1103, 1204, 1205, 1206, 1119,
                1120, 1121, 1122, 1123,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1203, 1208, 1207, 1128,
                1127, 1126, 1125, 1124,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1202, 1209, 1210, 1211,
                1212, 1213, 1214, 1215,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1201, 1222, 1221, 1220,
                1219, 1218, 1217, 1216,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1200, 1223, 1224, 1225, 1226, 1227,
                1228, 1229,
            ],
        ];

        const MAP14: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 25, 26, 224, 223, 323, 322, 424, 423, 624, 623, 727, 926, 925, 1025, 1024,
                1126, 1226, 1327,
            ],
            [
                18, 21, 24, 27, 225, 222, 324, 321, 425, 422, 625, 622, 726, 927, 924, 1026, 1023,
                1125, 1225, 1326,
            ],
            [
                17, 22, 23, 227, 226, 221, 325, 320, 426, 421, 626, 621, 725, 724, 923, 1027, 1022,
                1124, 1224, 1325,
            ],
            [
                16, 127, 126, 119, 118, 220, 326, 319, 419, 420, 627, 620, 722, 723, 922, 921,
                1021, 1123, 1223, 1324,
            ],
            [
                15, 124, 125, 120, 117, 219, 218, 318, 418, 520, 519, 619, 721, 821, 820, 920,
                1020, 1122, 1222, 1323,
            ],
            [
                14, 123, 122, 121, 116, 216, 217, 317, 417, 521, 518, 618, 720, 822, 819, 919,
                1019, 1121, 1221, 1322,
            ],
            [
                13, 112, 113, 114, 115, 215, 315, 316, 416, 522, 517, 617, 719, 823, 818, 918,
                1018, 1120, 1220, 1321,
            ],
            [
                12, 111, 211, 212, 213, 214, 314, 414, 415, 523, 516, 616, 718, 824, 817, 917,
                1017, 1119, 1219, 1320,
            ],
            [
                11, 110, 210, 310, 311, 312, 313, 413, 527, 524, 515, 615, 717, 826, 816, 916,
                1016, 1118, 1218, 1319,
            ],
            [
                10, 109, 209, 309, 409, 410, 411, 412, 526, 525, 514, 614, 716, 826, 815, 915,
                1015, 1117, 1217, 1318,
            ],
            [
                9, 108, 208, 308, 408, 508, 509, 510, 511, 512, 513, 613, 715, 827, 814, 914, 1014,
                1116, 1216, 1317,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 608, 609, 610, 611, 612, 714, 713, 813, 913, 1013,
                1115, 1215, 1316,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 707, 708, 709, 710, 711, 712, 812, 912, 1012,
                1114, 1214, 1315,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 806, 807, 808, 809, 810, 811, 911, 1011,
                1113, 1213, 1314,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 904, 905, 906, 907, 908, 909, 910, 1010,
                1112, 1212, 1313,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 903, 1003, 1004, 1005, 1006, 1007, 1008,
                1009, 1111, 1211, 1312,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1103, 1104, 1105, 1106,
                1107, 1110, 1210, 1311,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1202, 1203, 1204,
                1108, 1109, 1209, 1310,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1301, 1302, 1205,
                1206, 1207, 1208, 1309,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1300, 1303, 1304, 1305, 1306,
                1307, 1308,
            ],
        ];

        const MAP15: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                21, 22, 222, 223, 321, 322, 422, 423, 624, 825, 824, 924, 923, 1025, 1224, 1223,
                1323, 1322, 1423, 1422,
            ],
            [
                20, 23, 221, 224, 320, 323, 421, 424, 623, 725, 823, 822, 922, 1024, 1225, 1222,
                1324, 1321, 1424, 1421,
            ],
            [
                19, 24, 220, 225, 319, 324, 420, 425, 622, 724, 723, 821, 921, 1023, 1022, 1221,
                1325, 1320, 1425, 1420,
            ],
            [
                18, 25, 219, 218, 318, 325, 419, 525, 621, 620, 722, 820, 920, 1020, 1021, 1220,
                1219, 1319, 1318, 1419,
            ],
            [
                17, 116, 117, 217, 317, 417, 418, 524, 523, 619, 721, 819, 919, 1019, 1119, 1118,
                1218, 1217, 1317, 1418,
            ],
            [
                16, 115, 118, 216, 316, 416, 520, 521, 522, 618, 720, 818, 918, 1018, 1120, 1117,
                1116, 1216, 1316, 1417,
            ],
            [
                15, 114, 119, 215, 315, 415, 519, 615, 616, 617, 719, 817, 917, 1017, 1121, 1122,
                1115, 1215, 1315, 1416,
            ],
            [
                14, 113, 120, 214, 314, 414, 518, 614, 613, 612, 718, 816, 916, 1016, 1015, 1123,
                1114, 1214, 1314, 1415,
            ],
            [
                13, 112, 121, 213, 313, 413, 517, 516, 610, 611, 717, 815, 915, 914, 1014, 1124,
                1113, 1213, 1313, 1414,
            ],
            [
                12, 111, 122, 212, 312, 412, 514, 515, 609, 715, 716, 814, 813, 913, 1013, 1125,
                1112, 1212, 1312, 1413,
            ],
            [
                11, 110, 123, 211, 311, 411, 513, 512, 608, 714, 711, 710, 812, 912, 1012, 1011,
                1111, 1211, 1311, 1412,
            ],
            [
                10, 109, 124, 210, 310, 410, 510, 511, 607, 713, 712, 709, 811, 911, 910, 1010,
                1110, 1210, 1310, 1411,
            ],
            [
                9, 108, 208, 209, 309, 409, 509, 508, 606, 706, 707, 708, 810, 809, 909, 1009,
                1109, 1209, 1309, 1410,
            ],
            [
                8, 107, 207, 307, 308, 408, 506, 507, 605, 705, 805, 806, 807, 808, 908, 1008,
                1108, 1208, 1308, 1409,
            ],
            [
                7, 106, 206, 306, 406, 407, 505, 504, 604, 704, 804, 904, 905, 906, 907, 1007,
                1107, 1207, 1307, 1408,
            ],
            [
                6, 105, 205, 305, 405, 404, 403, 503, 603, 703, 803, 903, 1003, 1004, 1005, 1006,
                1106, 1206, 1306, 1407,
            ],
            [
                5, 104, 204, 304, 303, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1103, 1104,
                1105, 1205, 1305, 1406,
            ],
            [
                4, 103, 203, 202, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1202,
                1203, 1204, 1304, 1405,
            ],
            [
                3, 102, 101, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300,
                1301, 1302, 1303, 1404,
            ],
            [
                2, 1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1400, 1401, 1402, 1403,
            ],
        ];

        const MAP16: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 20, 21, 22, 23, 24, 123, 223, 323, 423, 523, 623, 723, 823, 923, 1023, 1123,
                1223, 1323, 1423,
            ],
            [
                18, 117, 118, 119, 120, 121, 122, 222, 322, 422, 522, 622, 722, 822, 922, 1022,
                1122, 1222, 1322, 1422,
            ],
            [
                17, 116, 216, 217, 218, 219, 220, 221, 321, 421, 521, 621, 721, 821, 921, 1021,
                1121, 1221, 1321, 1421,
            ],
            [
                16, 115, 215, 315, 316, 317, 318, 319, 320, 420, 520, 620, 720, 820, 920, 1020,
                1120, 1220, 1320, 1420,
            ],
            [
                15, 114, 214, 314, 414, 415, 416, 417, 418, 419, 519, 619, 719, 819, 919, 1019,
                1119, 1219, 1319, 1419,
            ],
            [
                14, 113, 213, 313, 413, 513, 514, 515, 516, 517, 518, 618, 718, 818, 918, 1018,
                1118, 1218, 1318, 1418,
            ],
            [
                13, 112, 212, 312, 412, 512, 612, 613, 614, 615, 616, 617, 717, 817, 917, 1017,
                1117, 1217, 1317, 1417,
            ],
            [
                12, 111, 211, 311, 411, 511, 611, 711, 712, 713, 714, 715, 716, 816, 916, 1016,
                1116, 1216, 1316, 1416,
            ],
            [
                11, 110, 210, 310, 410, 510, 610, 710, 810, 811, 812, 813, 814, 815, 915, 1015,
                1115, 1215, 1315, 1415,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 709, 809, 909, 910, 911, 912, 913, 914, 1014,
                1114, 1214, 1314, 1414,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 808, 908, 1008, 1009, 1010, 1011, 1012, 1013,
                1113, 1213, 1313, 1413,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 807, 907, 1007, 1107, 1108, 1109, 1110, 1111,
                1112, 1212, 1312, 1412,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 806, 906, 1006, 1106, 1206, 1207, 1208, 1209,
                1210, 1211, 1311, 1411,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 905, 1005, 1105, 1205, 1305, 1306, 1307,
                1308, 1309, 1310, 1410,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 904, 1004, 1104, 1204, 1304, 1404, 1405,
                1406, 1407, 1408, 1409,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 903, 1003, 1103, 1203, 1303, 1403, 1504,
                1505, 1506, 1507, 1508,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1202, 1302, 1402, 1503,
                1512, 1511, 1510, 1509,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1301, 1401, 1502,
                1513, 1514, 1515, 1516,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1501,
                1520, 1519, 1518, 1517,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1500, 1521, 1522, 1523,
                1524,
            ],
        ];

        const MAP17: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                20, 21, 120, 121, 421, 422, 520, 521, 619, 620, 621, 1022, 1122, 1222, 1322, 1321,
                1421, 1521, 1520, 1621,
            ],
            [
                19, 22, 119, 122, 420, 419, 519, 617, 618, 922, 921, 1021, 1121, 1221, 1220, 1320,
                1420, 1522, 1519, 1620,
            ],
            [
                18, 117, 118, 222, 221, 418, 518, 616, 716, 717, 920, 1020, 1120, 1119, 1219, 1319,
                1419, 1418, 1518, 1619,
            ],
            [
                17, 116, 216, 217, 220, 417, 517, 615, 715, 718, 919, 1019, 1018, 1118, 1218, 1318,
                1317, 1417, 1517, 1618,
            ],
            [
                16, 115, 215, 218, 219, 416, 516, 614, 714, 719, 918, 917, 1017, 1117, 1217, 1216,
                1316, 1416, 1516, 1617,
            ],
            [
                15, 114, 214, 314, 315, 415, 515, 613, 713, 720, 822, 916, 1016, 1116, 1115, 1215,
                1315, 1415, 1515, 1616,
            ],
            [
                14, 113, 213, 313, 316, 414, 514, 612, 712, 721, 821, 915, 1015, 1014, 1114, 1214,
                1314, 1414, 1514, 1615,
            ],
            [
                13, 112, 212, 312, 317, 413, 513, 611, 711, 819, 820, 914, 913, 1013, 1113, 1213,
                1313, 1413, 1513, 1614,
            ],
            [
                12, 111, 211, 311, 318, 412, 512, 610, 710, 818, 813, 812, 912, 1012, 1112, 1212,
                1312, 1412, 1512, 1613,
            ],
            [
                11, 110, 210, 310, 319, 411, 511, 609, 709, 817, 814, 811, 911, 1011, 1111, 1211,
                1311, 1411, 1511, 1612,
            ],
            [
                10, 109, 209, 309, 320, 410, 510, 608, 708, 816, 815, 810, 910, 1010, 1110, 1210,
                1310, 1410, 1510, 1611,
            ],
            [
                9, 108, 208, 308, 321, 409, 509, 607, 707, 807, 808, 809, 909, 1009, 1109, 1209,
                1309, 1409, 1509, 1610,
            ],
            [
                8, 107, 207, 307, 407, 407, 508, 606, 706, 806, 906, 907, 908, 1008, 1108, 1208,
                1308, 1408, 1508, 1609,
            ],
            [
                7, 106, 206, 306, 406, 506, 507, 605, 705, 805, 905, 1005, 1006, 1007, 1107, 1207,
                1307, 1407, 1507, 1608,
            ],
            [
                6, 105, 205, 305, 405, 505, 504, 604, 704, 804, 904, 1004, 1104, 1105, 1106, 1206,
                1306, 1406, 1506, 1607,
            ],
            [
                5, 104, 204, 304, 404, 403, 503, 603, 703, 803, 903, 1003, 1103, 1203, 1204, 1205,
                1305, 1405, 1505, 1606,
            ],
            [
                4, 103, 203, 303, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1202, 1302, 1303,
                1304, 1404, 1504, 1605,
            ],
            [
                3, 102, 202, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1301, 1401,
                1402, 1403, 1503, 1604,
            ],
            [
                2, 101, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400,
                1500, 1501, 1502, 1603,
            ],
            [
                1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1600, 1601, 1602,
            ],
        ];

        const MAP18: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                20, 119, 120, 321, 421, 420, 520, 620, 720, 820, 920, 1020, 1120, 1220, 1320, 1321,
                1421, 1620, 1619, 1720,
            ],
            [
                19, 118, 121, 320, 319, 419, 519, 619, 719, 819, 919, 1019, 1119, 1219, 1319, 1419,
                1420, 1621, 1618, 1719,
            ],
            [
                18, 117, 217, 218, 318, 418, 518, 618, 718, 818, 918, 1018, 1118, 1218, 1318, 1418,
                1518, 1517, 1617, 1718,
            ],
            [
                17, 116, 216, 219, 317, 417, 517, 617, 717, 817, 917, 1017, 1117, 1217, 1317, 1417,
                1519, 1516, 1616, 1717,
            ],
            [
                16, 115, 215, 220, 316, 416, 516, 616, 716, 816, 916, 1016, 1116, 1216, 1316, 1416,
                1520, 1515, 1615, 1716,
            ],
            [
                15, 114, 214, 314, 315, 415, 515, 615, 715, 815, 915, 1015, 1115, 1215, 1315, 1415,
                1414, 1514, 1614, 1715,
            ],
            [
                14, 113, 213, 313, 413, 414, 514, 614, 714, 814, 914, 1014, 1114, 1214, 1314, 1313,
                1413, 1513, 1613, 1714,
            ],
            [
                13, 112, 212, 312, 412, 512, 513, 613, 713, 813, 913, 1013, 1113, 1213, 1212, 1312,
                1412, 1512, 1612, 1713,
            ],
            [
                12, 111, 211, 311, 411, 511, 611, 612, 712, 812, 912, 1012, 1112, 1111, 1211, 1311,
                1411, 1511, 1611, 1712,
            ],
            [
                11, 110, 210, 310, 410, 510, 610, 710, 711, 811, 911, 1011, 1010, 1110, 1210, 1310,
                1410, 1510, 1610, 1711,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 709, 809, 810, 910, 909, 1009, 1109, 1209, 1309,
                1409, 1509, 1609, 1710,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 808, 807, 907, 908, 1008, 1108, 1208, 1308,
                1408, 1508, 1608, 1709,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 706, 806, 906, 1006, 1007, 1107, 1207, 1307,
                1407, 1507, 1607, 1708,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 605, 705, 805, 905, 1005, 1105, 1106, 1206, 1306,
                1406, 1506, 1606, 1707,
            ],
            [
                6, 105, 205, 305, 405, 505, 504, 604, 704, 804, 904, 1004, 1104, 1204, 1205, 1305,
                1405, 1505, 1605, 1706,
            ],
            [
                5, 104, 204, 304, 404, 403, 503, 603, 703, 803, 903, 1003, 1103, 1203, 1303, 1304,
                1404, 1504, 1604, 1705,
            ],
            [
                4, 103, 203, 303, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1202, 1302, 1402,
                1403, 1503, 1603, 1704,
            ],
            [
                3, 102, 202, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1301, 1401,
                1501, 1502, 1602, 1703,
            ],
            [
                2, 101, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400,
                1500, 1600, 1601, 1702,
            ],
            [
                1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1700, 1701,
            ],
        ];

        const MAP19: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 118, 119, 120, 319, 419, 519, 619, 719, 819, 919, 1019, 1119, 1219, 1319, 1419,
                1519, 1619, 1719, 1820,
            ],
            [
                18, 117, 219, 220, 318, 418, 518, 618, 718, 818, 918, 1018, 1118, 1218, 1318, 1418,
                1518, 1618, 1718, 1819,
            ],
            [
                17, 116, 218, 217, 317, 417, 517, 617, 717, 817, 917, 1017, 1117, 1217, 1317, 1417,
                1517, 1617, 1717, 1818,
            ],
            [
                16, 115, 215, 216, 316, 416, 516, 616, 716, 816, 916, 1016, 1116, 1216, 1316, 1416,
                1516, 1616, 1716, 1817,
            ],
            [
                15, 114, 214, 314, 315, 415, 515, 615, 715, 815, 915, 1015, 1115, 1215, 1315, 1415,
                1515, 1615, 1715, 1816,
            ],
            [
                14, 113, 213, 313, 413, 414, 514, 614, 714, 814, 914, 1014, 1114, 1214, 1314, 1414,
                1514, 1614, 1714, 1815,
            ],
            [
                13, 112, 212, 312, 412, 512, 513, 613, 713, 813, 913, 1013, 1113, 1213, 1313, 1413,
                1513, 1613, 1713, 1814,
            ],
            [
                12, 111, 211, 311, 411, 511, 611, 612, 712, 812, 912, 1012, 1112, 1212, 1312, 1412,
                1512, 1612, 1712, 1813,
            ],
            [
                11, 110, 210, 310, 410, 510, 610, 710, 711, 811, 911, 1011, 1111, 1211, 1311, 1411,
                1511, 1611, 1711, 1812,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 709, 809, 810, 910, 1010, 1110, 1210, 1310, 1410,
                1510, 1610, 1710, 1811,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 808, 908, 909, 1009, 1109, 1209, 1309, 1409,
                1509, 1609, 1709, 1810,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 807, 907, 1007, 1008, 1108, 1208, 1308, 1408,
                1508, 1608, 1708, 1809,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 806, 906, 1006, 1106, 1107, 1207, 1307, 1407,
                1507, 1607, 1707, 1808,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 905, 1005, 1105, 1205, 1206, 1306, 1406,
                1506, 1606, 1706, 1807,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 904, 1004, 1104, 1204, 1304, 1305, 1405,
                1505, 1605, 1705, 1806,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 903, 1003, 1103, 1203, 1303, 1403, 1404,
                1504, 1604, 1704, 1805,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1202, 1302, 1402, 1502,
                1503, 1603, 1703, 1804,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1301, 1401, 1501,
                1601, 1602, 1702, 1803,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500,
                1600, 1700, 1701, 1802,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1800, 1801,
            ],
        ];

        const MAP20: [[i32; Input::PALLETTE_SIZE]; Input::PALLETTE_SIZE] = [
            [
                19, 118, 218, 318, 418, 518, 618, 718, 818, 918, 1018, 1118, 1218, 1318, 1418,
                1518, 1618, 1718, 1818, 1919,
            ],
            [
                18, 117, 217, 317, 417, 517, 617, 717, 817, 917, 1017, 1117, 1217, 1317, 1417,
                1517, 1617, 1717, 1817, 1918,
            ],
            [
                17, 116, 216, 316, 416, 516, 616, 716, 816, 916, 1016, 1116, 1216, 1316, 1416,
                1516, 1616, 1716, 1816, 1917,
            ],
            [
                16, 115, 215, 315, 415, 515, 615, 715, 815, 915, 1015, 1115, 1215, 1315, 1415,
                1515, 1615, 1715, 1815, 1916,
            ],
            [
                15, 114, 214, 314, 414, 514, 614, 714, 814, 914, 1014, 1114, 1214, 1314, 1414,
                1514, 1614, 1714, 1814, 1915,
            ],
            [
                14, 113, 213, 313, 413, 513, 613, 713, 813, 913, 1013, 1113, 1213, 1313, 1413,
                1513, 1613, 1713, 1813, 1914,
            ],
            [
                13, 112, 212, 312, 412, 512, 612, 712, 812, 912, 1012, 1112, 1212, 1312, 1412,
                1512, 1612, 1712, 1812, 1913,
            ],
            [
                12, 111, 211, 311, 411, 511, 611, 711, 811, 911, 1011, 1111, 1211, 1311, 1411,
                1511, 1611, 1711, 1811, 1912,
            ],
            [
                11, 110, 210, 310, 410, 510, 610, 710, 810, 910, 1010, 1110, 1210, 1310, 1410,
                1510, 1610, 1710, 1810, 1911,
            ],
            [
                10, 109, 209, 309, 409, 509, 609, 709, 809, 909, 1009, 1109, 1209, 1309, 1409,
                1509, 1609, 1709, 1809, 1910,
            ],
            [
                9, 108, 208, 308, 408, 508, 608, 708, 808, 908, 1008, 1108, 1208, 1308, 1408, 1508,
                1608, 1708, 1808, 1909,
            ],
            [
                8, 107, 207, 307, 407, 507, 607, 707, 807, 907, 1007, 1107, 1207, 1307, 1407, 1507,
                1607, 1707, 1807, 1908,
            ],
            [
                7, 106, 206, 306, 406, 506, 606, 706, 806, 906, 1006, 1106, 1206, 1306, 1406, 1506,
                1606, 1706, 1806, 1907,
            ],
            [
                6, 105, 205, 305, 405, 505, 605, 705, 805, 905, 1005, 1105, 1205, 1305, 1405, 1505,
                1605, 1705, 1805, 1906,
            ],
            [
                5, 104, 204, 304, 404, 504, 604, 704, 804, 904, 1004, 1104, 1204, 1304, 1404, 1504,
                1604, 1704, 1804, 1905,
            ],
            [
                4, 103, 203, 303, 403, 503, 603, 703, 803, 903, 1003, 1103, 1203, 1303, 1403, 1503,
                1603, 1703, 1803, 1904,
            ],
            [
                3, 102, 202, 302, 402, 502, 602, 702, 802, 902, 1002, 1102, 1202, 1302, 1402, 1502,
                1602, 1702, 1802, 1903,
            ],
            [
                2, 101, 201, 301, 401, 501, 601, 701, 801, 901, 1001, 1101, 1201, 1301, 1401, 1501,
                1601, 1701, 1801, 1902,
            ],
            [
                1, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500,
                1600, 1700, 1800, 1901,
            ],
            [
                0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1900,
            ],
        ];
    }

    use crate::problem::{Input, Output};

    pub fn solve(input: &Input) -> Output {
        if input.max_turns < 5000 {
            solve_4000_5000(input)
        } else if input.max_turns < 13000 {
            solve_5000_13000(input)
        } else if input.max_turns < 23000 {
            solve_13000_23000(input)
        } else {
            solve_23000_64000(input)
        }
    }

    fn solve_4000_5000(input: &Input) -> Output {
        let palette = fixed_palette::palette::create_palette(input);
        let output = fixed_palette::beam::run(input, palette).unwrap();

        eprintln!("### FixedPaletteSolver ###");
        eprintln!("Error penalty: {}", output.error_penalty);
        eprintln!("Paint penalty: {}", output.paint_penalty);
        eprintln!("Score        : {}", output.score);
        eprintln!();

        output
    }

    fn solve_5000_13000(input: &Input) -> Output {
        let (trimmed_input, convex_hull) = input.trim_points();
        let single_palette = single_lane_palette::crate_palette(&trimmed_input);
        let result = single_frac::half_beam::run(&trimmed_input, single_palette, 1.0);

        match result {
            single_frac::half_beam::BeamResult::Completed(mut output) => {
                convex_hull.restore_ops(&mut output.operations);

                eprintln!("### HalfFracSolver ###");
                eprintln!("Error penalty: {}", output.error_penalty);
                eprintln!("Paint penalty: {}", output.paint_penalty);
                eprintln!("Score        : {}", output.score);
                eprintln!();

                output
            }
            single_frac::half_beam::BeamResult::Incomplete(incomplete_result) => {
                let mut ops = incomplete_result.ops;
                convex_hull.restore_ops(&mut ops);

                let (palette, paints) = fixed_palette::palette::create_palette_from(
                    input,
                    incomplete_result.palette,
                    &incomplete_result.paints,
                    &mut ops,
                );

                dbg!(palette.lanes.len());

                let (ex_ops, error_sum) = fixed_palette::beam::run_resume(
                    input,
                    palette.clone(),
                    paints,
                    incomplete_result.total_error,
                    incomplete_result.excess_paint,
                    incomplete_result.target_i,
                );
                ops.extend(ex_ops);
                let output = Output::new(input, palette.dividers, ops, error_sum);

                eprintln!("### HalfFracSolver + FixedPaletteSolver ###");
                eprintln!("Error penalty: {}", output.error_penalty);
                eprintln!("Paint penalty: {}", output.paint_penalty);
                eprintln!("Score        : {}", output.score);

                output
            }
        }
    }

    fn solve_13000_23000(input: &Input) -> Output {
        let (trimmed_input, convex_hull) = input.trim_points();
        let single_palette = single_lane_palette::crate_palette(&trimmed_input);
        let result = single_frac::beam::run(&trimmed_input, single_palette.clone(), 1.0);

        match result {
            single_frac::beam::BeamResult::Completed(mut output) => {
                convex_hull.restore_ops(&mut output.operations);

                eprintln!("### SingleFracSolver ###");
                eprintln!("Error penalty: {}", output.error_penalty);
                eprintln!("Paint penalty: {}", output.paint_penalty);
                eprintln!("Score        : {}", output.score);
                eprintln!();

                output
            }
            single_frac::beam::BeamResult::Incomplete(incomplete_result) => {
                let ops = incomplete_result.ops;
                let results = single_frac::half_beam::run_resume(
                    &trimmed_input,
                    single_palette.clone(),
                    incomplete_result.amounts,
                    incomplete_result.units,
                    incomplete_result.total_error,
                    incomplete_result.discard_sum,
                    incomplete_result.excess_paint,
                    incomplete_result.target_i,
                    1.0,
                );

                let mut best_output = Output::new(
                    &trimmed_input,
                    single_palette.dividers.clone(),
                    vec![],
                    1e10,
                );

                for (ex_ops, error_sum) in results.into_iter() {
                    let total_op_cnt = ops.len() + ex_ops.len();

                    if total_op_cnt > input.max_turns {
                        eprintln!(
                            "Skipping candidate with too many operations: {}",
                            total_op_cnt
                        );

                        continue;
                    }

                    let mut ops = ops.clone();
                    ops.extend(ex_ops);
                    let mut output = Output::new(
                        &trimmed_input,
                        single_palette.dividers.clone(),
                        ops,
                        error_sum,
                    );
                    convex_hull.restore_ops(&mut output.operations);
                    eprintln!("candidate score: {}", output.score);

                    if output.score < best_output.score {
                        best_output = output;
                    }
                }

                eprintln!("### SingleFracSolver + HalfFracSolver ###");
                eprintln!("Error penalty: {}", best_output.error_penalty);
                eprintln!("Paint penalty: {}", best_output.paint_penalty);
                eprintln!("Score        : {}", best_output.score);
                eprintln!();

                best_output
            }
        }
    }

    fn solve_23000_64000(input: &Input) -> Output {
        let (trimmed_input, convex_hull) = input.trim_points();
        let palette = frac_2digit::palette::create_palette(&trimmed_input).unwrap();
        let result = frac_2digit::beam::run(&trimmed_input, palette.clone(), 1.0);

        match result {
            frac_2digit::beam::BeamResult::Completed(output) => {
                let mut output = output;
                convex_hull.restore_ops(&mut output.operations);

                eprintln!("### Frac2DigitSolver ###");
                eprintln!("Error penalty: {}", output.error_penalty);
                eprintln!("Paint penalty: {}", output.paint_penalty);
                eprintln!("Score        : {}", output.score);
                eprintln!();

                output
            }
            frac_2digit::beam::BeamResult::Incomplete(incomplete_result) => {
                let ops = incomplete_result.ops;
                let palette = single_lane_palette::create_palette_from(palette);
                let results = single_frac::beam::run_resume(
                    &trimmed_input,
                    palette.clone(),
                    incomplete_result.amounts,
                    incomplete_result.units,
                    incomplete_result.total_error,
                    incomplete_result.discard_sum,
                    incomplete_result.excess_paint,
                    incomplete_result.target_i,
                    1.0,
                );

                let mut best_output =
                    Output::new(&trimmed_input, palette.dividers.clone(), vec![], 1e10);

                for (ex_ops, error_sum) in results.into_iter() {
                    let total_op_cnt = ops.len() + ex_ops.len();

                    if total_op_cnt > input.max_turns {
                        eprintln!(
                            "Skipping candidate with too many operations: {}",
                            total_op_cnt
                        );

                        continue;
                    }

                    let mut ops = ops.clone();
                    ops.extend(ex_ops);
                    let mut output =
                        Output::new(&trimmed_input, palette.dividers.clone(), ops, error_sum);
                    convex_hull.restore_ops(&mut output.operations);
                    eprintln!("candidate score: {}", output.score);

                    if output.score < best_output.score {
                        best_output = output;
                    }
                }

                eprintln!("### Frac2DigitSolver + HalfFracSolver ###");
                eprintln!("Error penalty: {}", best_output.error_penalty);
                eprintln!("Paint penalty: {}", best_output.paint_penalty);
                eprintln!("Score        : {}", best_output.score);

                best_output
            }
        }
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

use proconio::fastout;

#[fastout]
fn main() {
    let input = problem::Input::read();
    eprintln!("[Init] {:?}", input.since);

    let output = solver::solve(&input);
    println!("{}", output);
}
