mod bayesian {
    use rand::Rng;
    use rand_distr::{Distribution, Gamma, Normal};

    /// 正規-ガンマ分布を表す構造体。
    ///
    /// 正規-逆ガンマ分布  NG(mu, lambda, alpha, beta) = N(mu, (lambda * precision)^-1) * G(alpha, beta) を表す構造体。
    /// ベイズ推定により、正規分布の平均と精度の事後分布を更新することができる。
    #[derive(Debug, Clone, Copy)]
    pub struct GaussInverseGamma {
        mu: f64,
        lambda: f64,
        alpha: f64,
        beta: f64,
    }

    #[allow(dead_code)]
    impl GaussInverseGamma {
        /// 正規-ガンマ分布 NG(mu, lambda, alpha, beta) を生成する。
        ///
        /// # Arguments
        ///
        /// * `mu` - 正規分布の平均の事前分布の平均
        /// * `lambda` - サンプリングされた精度と正規分布の平均の精度との比
        /// * `alpha` - **精度 (分散の逆数)** を表すガンマ分布の形状パラメータ
        /// * `beta` - **精度 (分散の逆数)** を表すガンマ分布の尺度パラメータ
        ///
        /// # Note
        ///
        /// * `lambda` は正規分布の平均の事前分布の疑似観測回数と解釈することができる。
        /// * `beta` は正規分布の精度の事前分布の疑似観測回数の2倍と解釈することができる。
        pub fn new(mu: f64, lambda: f64, alpha: f64, beta: f64) -> Self {
            assert!(!mu.is_nan(), "mu is NaN");
            assert!(lambda > 0.0, "lambda is not positive");
            assert!(alpha > 0.0, "alpha is not positive");
            assert!(beta > 0.0, "beta is not positive");

            Self {
                mu,
                lambda,
                alpha,
                beta,
            }
        }

        /// 対象とする正規分布からの疑似観測値から正規-ガンマ分布 NG(mu, lambda, alpha, beta) を生成する。
        ///
        /// # Arguments
        ///
        /// - `mean` - 疑似観測値の期待値
        /// - `std_dev` - 疑似観測値の標準偏差
        /// - `pseudo_observation_count` - 疑似観測回数
        pub fn from_psuedo_observation(
            mean: f64,
            std_dev: f64,
            pseudo_observation_count: usize,
        ) -> Self {
            assert!(std_dev > 0.0, "expected_std_dev is not positive");
            assert!(
                pseudo_observation_count > 0,
                "pseudo_observation_count is not positive"
            );

            let expected_variance = std_dev * std_dev;
            let expected_precision = 1.0 / expected_variance;

            let mu = mean;
            let lambda = pseudo_observation_count as f64;
            let alpha = (pseudo_observation_count * 2) as f64;

            // 精度の期待値E[p] = alpha / beta より、 beta = alpha / E[p]
            let beta = alpha / expected_precision;

            Self::new(mu, lambda, alpha, beta)
        }

        /// 観測値xを元にしてベイズ更新を行う。
        pub fn update(&mut self, x: f64) {
            let mu = (x + self.lambda * self.mu) / (self.lambda + 1.0);
            let lambda = self.lambda + 1.0;
            let alpha = self.alpha + 0.5;
            let dev2 = (x - self.mu) * (x - self.mu);
            let beta = self.beta + 0.5 * (self.lambda * dev2) / (self.lambda + 1.0);

            self.mu = mu;
            self.lambda = lambda;
            self.alpha = alpha;
            self.beta = beta;
        }

        /// (平均, 標準偏差) の期待値を取得する。
        pub fn expected(&self) -> (f64, f64) {
            let expected_precision = self.alpha / self.beta;
            let expected_variance = 1.0 / expected_precision;
            let expected_std_dev = expected_variance.sqrt();
            (self.mu, expected_std_dev)
        }

        pub fn get_pseudo_observation_count(&self) -> f64 {
            self.lambda
        }

        pub fn set_pseudo_observation_count(&mut self, pseudo_observation_count: f64) {
            self.lambda = pseudo_observation_count;
            self.alpha = pseudo_observation_count * 0.5;
        }
    }

    impl Distribution<(f64, f64)> for GaussInverseGamma {
        fn sample<R: Rng + ?Sized>(&self, rng: &mut R) -> (f64, f64) {
            // ガンマ分布から精度をサンプリング
            let precision = rng.sample(Gamma::new(self.alpha, 1.0 / self.beta).unwrap());
            let std_dev = 1.0 / precision.sqrt();

            // 正規分布から平均をサンプリング
            let std_dev_mean = 1.0 / (precision * self.lambda).sqrt();
            let mean = rng.sample(Normal::new(self.mu, std_dev_mean).unwrap());

            (mean, std_dev)
        }
    }
}
mod beam {
    use std::time::Instant;

    use crate::bayesian::GaussInverseGamma;

    /// ビーム幅を提案するトレイト
    pub trait BeamWidthSuggester {
        // 現在のターン数を受け取り、ビーム幅を提案する
        fn suggest(&mut self) -> usize;
    }

    /// 常に固定のビーム幅を返すBeamWidthSuggester
    pub struct FixedBeamWidthSuggester {
        width: usize,
    }

    #[allow(dead_code)]
    impl FixedBeamWidthSuggester {
        pub fn new(width: usize) -> Self {
            Self { width }
        }
    }

    impl BeamWidthSuggester for FixedBeamWidthSuggester {
        fn suggest(&mut self) -> usize {
            self.width
        }
    }

    /// ベイズ推定により適切なビーム幅を計算するBeamWidthSuggester。
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
        /// ターンごとの所要時間が従う正規分布の(平均, 標準偏差)の事前分布
        prior_dist: GaussInverseGamma,
        /// 問題の実行時間制限T
        time_limit_sec: f64,
        /// 現在のターン数i
        current_turn: usize,
        /// 最大ターン数M
        max_turn: usize,
        /// ウォームアップターン数（最初のXターン分の情報は採用せずに捨てる）
        warmup_turn: usize,
        /// 所要時間を記憶するターン数の目安
        max_memory_turn: usize,
        /// 最小ビーム幅W_min
        min_beam_width: usize,
        /// 最大ビーム幅W_max
        max_beam_width: usize,
        /// 現在のビーム幅W_i
        current_beam_width: usize,
        /// ログの出力インターバル（0にするとログを出力しなくなる）
        verbose_interval: usize,
        /// ビーム開始時刻
        start_time: Instant,
        /// 前回の計測時刻
        last_time: Instant,
    }

    #[allow(dead_code)]
    impl BayesianBeamWidthSuggester {
        pub fn new(
            max_turn: usize,
            warmup_turn: usize,
            time_limit_sec: f64,
            standard_beam_width: usize,
            min_beam_width: usize,
            max_beam_width: usize,
            verbose_interval: usize,
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

            // 雑にσ=20%ズレると仮定
            let stddev_sec = 0.2 * mean_sec;
            let prior_dist = GaussInverseGamma::from_psuedo_observation(mean_sec, stddev_sec, 10);

            // 直近10%程度のターン数の移動平均的な所要時間を参考にする
            let max_memory_turn = max_turn / 10;

            eprintln!(
                "standard beam width: {}, time limit: {:.3}s",
                standard_beam_width, time_limit_sec
            );

            Self {
                prior_dist,
                time_limit_sec,
                current_turn: 0,
                min_beam_width,
                max_beam_width,
                verbose_interval,
                max_turn,
                max_memory_turn,
                warmup_turn,
                current_beam_width: 0,
                start_time: Instant::now(),
                last_time: Instant::now(),
            }
        }

        fn update_distribution(&mut self, duration_sec: f64) {
            self.prior_dist.update(duration_sec);

            // ベイズ推定の疑似観測数にリミットをかける
            // （序盤と終盤で実行時間が異なるケースで、序盤の観測値に引きずられないようにするため）
            if self.prior_dist.get_pseudo_observation_count() >= self.max_memory_turn as f64 {
                self.prior_dist
                    .set_pseudo_observation_count(self.max_memory_turn as f64);
            }
        }

        fn calc_safe_beam_width(&self) -> usize {
            let remaining_turn = (self.max_turn - self.current_turn) as f64;
            let elapsed_time = (Instant::now() - self.start_time).as_secs_f64();
            let remaining_time = self.time_limit_sec - elapsed_time;

            let (mean, std_dev) = self.prior_dist.expected();
            let variance = std_dev * std_dev;

            let mean_remaining = remaining_turn * mean;
            let variance_remaining = remaining_turn * variance;
            let std_dev_remaining = variance_remaining.sqrt();

            // 3σの余裕を持たせる
            const SIGMA_COEF: f64 = 3.0;
            let needed_time_per_width = mean_remaining + SIGMA_COEF * std_dev_remaining;
            let beam_width = ((remaining_time / needed_time_per_width) as usize)
                .clamp(self.min_beam_width, self.max_beam_width);

            if self.verbose_interval != 0 && self.current_turn % self.verbose_interval == 0 {
                let stddev_per_run = (self.max_turn as f64 * variance).sqrt();
                let stddev_per_turn = variance.sqrt();

                eprintln!(
                    "turn:{:5}, beam width:{:5}, pase:{:7.1} ±{:6.2}ms/run,{:6.3} ±{:6.3}ms/turn",
                    self.current_turn,
                    beam_width,
                    mean * (beam_width * self.max_turn) as f64 * 1e3,
                    stddev_per_run * beam_width as f64 * 1e3,
                    mean * beam_width as f64 * 1e3,
                    stddev_per_turn * beam_width as f64 * 1e3
                );
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
                self.update_distribution(elapsed_per_beam);
            }

            self.last_time = Instant::now();
            let beam_width = self.calc_safe_beam_width();
            self.current_beam_width = beam_width;
            self.current_turn += 1;
            beam_width
        }
    }
}
mod common {
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
}
mod data_structures {
    #[derive(Debug, Clone, Copy)]
    pub struct ConstVec<T: Default + Copy, const N: usize> {
        stack: [T; N],
        top: usize,
    }

    #[allow(dead_code)]
    impl<T: Default + Copy, const N: usize> ConstVec<T, N> {
        pub fn new() -> Self {
            Self {
                stack: [T::default(); N],
                top: 0,
            }
        }

        pub fn push(&mut self, value: T) {
            self.stack[self.top] = value;
            self.top += 1;
        }

        pub fn pop(&mut self) -> Option<T> {
            if self.is_empty() {
                return None;
            }

            self.top -= 1;
            Some(self.stack[self.top])
        }

        pub fn last(&self) -> Option<T> {
            if self.is_empty() {
                return None;
            }

            Some(self.stack[self.top - 1])
        }

        pub fn is_empty(&self) -> bool {
            self.top == 0
        }

        pub fn len(&self) -> usize {
            self.top
        }

        pub fn as_slice(&self) -> &[T] {
            &self.stack[..self.top]
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct ConstQueue<T: Default + Copy, const N: usize> {
        queue: [T; N],
        front: usize,
    }

    #[allow(dead_code)]
    impl<T: Default + Copy, const N: usize> ConstQueue<T, N> {
        pub fn new(values: [T; N]) -> Self {
            Self {
                queue: values,
                front: 0,
            }
        }

        pub fn pop_front(&mut self) -> Option<T> {
            if self.is_empty() {
                return None;
            }

            let value = self.queue[self.front];
            self.front += 1;
            Some(value)
        }

        pub fn front(&self) -> Option<T> {
            if self.is_empty() {
                return None;
            }

            Some(self.queue[self.front])
        }

        pub fn is_empty(&self) -> bool {
            self.len() == 0
        }

        pub fn len(&self) -> usize {
            N - self.front
        }

        pub fn as_slice(&self) -> &[T] {
            &self.queue[self.front..]
        }
    }

    #[derive(Debug, Clone)]
    pub struct History<T: Clone> {
        history: Vec<(T, HistoryIndex)>,
    }

    impl<T: Clone> History<T> {
        pub fn new() -> Self {
            History { history: vec![] }
        }

        pub fn push(&mut self, action: T, parent: HistoryIndex) -> HistoryIndex {
            let index = HistoryIndex(self.history.len());
            self.history.push((action, parent));
            index
        }

        pub fn collect(&self, mut index: HistoryIndex) -> Vec<T> {
            let mut actions = vec![];

            while index != HistoryIndex::ROOT {
                let (action, i) = self.history[index.0].clone();
                actions.push(action);
                index = i;
            }

            actions.reverse();
            actions
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
    pub struct HistoryIndex(usize);

    impl HistoryIndex {
        pub const ROOT: Self = Self(!0);
    }
}
mod grid {
    use std::{
        fmt::{Debug, Display},
        ops::{Add, AddAssign, Index, IndexMut},
    };

    #[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
    pub struct Coord {
        row: u8,
        col: u8,
    }

    #[allow(dead_code)]
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

        pub const fn to_index(&self, size: usize) -> usize {
            self.row as usize * size + self.col as usize
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
            write!(f, "({}, {})", self.row(), self.col())
        }
    }

    impl Debug for Coord {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "({}, {})", self.row(), self.col())
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct CoordDiff {
        dr: i8,
        dc: i8,
    }

    #[allow(dead_code)]
    impl CoordDiff {
        pub const fn new(dr: isize, dc: isize) -> Self {
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

        pub const fn dr(&self) -> isize {
            self.dr as isize
        }

        pub const fn dc(&self) -> isize {
            self.dc as isize
        }
    }

    impl Add<CoordDiff> for Coord {
        type Output = Coord;

        fn add(self, rhs: CoordDiff) -> Self::Output {
            Coord {
                row: self.row.wrapping_add_signed(rhs.dr),
                col: self.col.wrapping_add_signed(rhs.dc),
            }
        }
    }

    impl AddAssign<CoordDiff> for Coord {
        fn add_assign(&mut self, rhs: CoordDiff) {
            self.row = self.row.wrapping_add_signed(rhs.dr);
            self.col = self.col.wrapping_add_signed(rhs.dc);
        }
    }

    #[allow(dead_code)]
    pub const ADJACENTS: [CoordDiff; 4] = [
        CoordDiff::new(-1, 0),
        CoordDiff::new(0, 1),
        CoordDiff::new(1, 0),
        CoordDiff::new(0, -1),
    ];

    #[allow(dead_code)]
    pub const DIRECTIONS: [char; 4] = ['U', 'R', 'D', 'L'];

    #[derive(Debug, Clone)]
    pub struct Map2d<T> {
        size: usize,
        map: Vec<T>,
    }

    #[allow(dead_code)]
    impl<T> Map2d<T> {
        pub fn new(map: Vec<T>, size: usize) -> Self {
            debug_assert!(size * size == map.len());
            Self { size, map }
        }
    }

    #[allow(dead_code)]
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
            &self.map[coordinate.to_index(self.size)]
        }
    }

    impl<T> IndexMut<Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(self.size)]
        }
    }

    impl<T> Index<&Coord> for Map2d<T> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &Coord) -> &Self::Output {
            &self.map[coordinate.to_index(self.size)]
        }
    }

    impl<T> IndexMut<&Coord> for Map2d<T> {
        #[inline]
        fn index_mut(&mut self, coordinate: &Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(self.size)]
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

    #[derive(Debug, Clone, Copy)]
    pub struct ConstMap2d<T, const N: usize, const N2: usize> {
        map: [T; N2],
    }

    #[allow(dead_code)]
    impl<T, const N: usize, const N2: usize> ConstMap2d<T, N, N2> {
        pub fn new(map: [T; N2]) -> Self {
            Self { map }
        }
    }

    #[allow(dead_code)]
    impl<T: Copy, const N: usize, const N2: usize> ConstMap2d<T, N, N2> {
        pub fn map<U>(&self, f: impl FnMut(T) -> U) -> ConstMap2d<U, N, N2> {
            let map = self.map.map(f);
            ConstMap2d::new(map)
        }
    }

    #[allow(dead_code)]
    impl<T: Default + Clone + Copy, const N: usize, const N2: usize> ConstMap2d<T, N, N2> {
        pub fn with_default() -> Self {
            let map = [T::default(); N2];
            Self { map }
        }
    }

    impl<T, const N: usize, const N2: usize> Index<Coord> for ConstMap2d<T, N, N2> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: Coord) -> &Self::Output {
            &self.map[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize, const N2: usize> IndexMut<Coord> for ConstMap2d<T, N, N2> {
        #[inline]
        fn index_mut(&mut self, coordinate: Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize, const N2: usize> Index<&Coord> for ConstMap2d<T, N, N2> {
        type Output = T;

        #[inline]
        fn index(&self, coordinate: &Coord) -> &Self::Output {
            &self.map[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize, const N2: usize> IndexMut<&Coord> for ConstMap2d<T, N, N2> {
        #[inline]
        fn index_mut(&mut self, coordinate: &Coord) -> &mut Self::Output {
            &mut self.map[coordinate.to_index(N)]
        }
    }

    impl<T, const N: usize, const N2: usize> Index<usize> for ConstMap2d<T, N, N2> {
        type Output = [T];

        #[inline]
        fn index(&self, row: usize) -> &Self::Output {
            let begin = row * N;
            let end = begin + N;
            &self.map[begin..end]
        }
    }

    impl<T, const N: usize, const N2: usize> IndexMut<usize> for ConstMap2d<T, N, N2> {
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
        data_structures::{ConstQueue, ConstVec},
        grid::{ConstMap2d, Coord, CoordDiff},
    };
    use proconio::input;
    use std::fmt::Display;

    pub type Grid<T> = ConstMap2d<T, { Input::N }, { Input::N * Input::N }>;

    #[derive(Debug, Clone)]
    pub struct Input {
        containers: [[Container; Input::N]; Input::N],
        params: Params,
    }

    impl Input {
        pub const N: usize = 5;
        pub const CONTAINER_COUNT: usize = Self::N * Self::N;

        fn new(containers: [[Container; Input::N]; Input::N]) -> Self {
            let params = Params::new();
            Self { containers, params }
        }

        pub fn read_input() -> Self {
            input! {
                n: usize,
                c: [[usize; n]; n],
            }

            let mut containers = [[Container::new(0); Self::N]; Self::N];

            for i in 0..Self::N {
                for j in 0..Self::N {
                    containers[i][j] = Container::new(c[i][j]);
                }
            }

            Self::new(containers)
        }

        pub const fn containers(&self) -> &[[Container; Input::N]; Input::N] {
            &self.containers
        }

        pub const fn is_large_crane(crane: usize) -> bool {
            crane == 0
        }

        pub const fn get_goal(container: Container) -> Coord {
            Coord::new(container.index() / Input::N, Input::N - 1)
        }

        pub const fn params(&self) -> &Params {
            &self.params
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Default)]
    pub struct Container(u8);

    impl Container {
        pub const fn new(index: usize) -> Self {
            Self(index as u8)
        }

        pub const fn index(&self) -> usize {
            self.0 as usize
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum Operation {
        Up,
        Right,
        Down,
        Left,
        Pick,
        Drop,
        Destroy,
        None,
    }

    impl Operation {
        pub fn dir(&self) -> CoordDiff {
            match self {
                Self::Up => CoordDiff::new(-1, 0),
                Self::Right => CoordDiff::new(0, 1),
                Self::Down => CoordDiff::new(1, 0),
                Self::Left => CoordDiff::new(0, -1),
                _ => CoordDiff::new(0, 0),
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum CraneState {
        Empty(Coord),
        Holding(Container, Coord),
        Destroyed,
    }

    impl CraneState {
        pub const fn coord(&self) -> Option<Coord> {
            match self {
                Self::Empty(coord) => Some(*coord),
                Self::Holding(_, coord) => Some(*coord),
                Self::Destroyed => None,
            }
        }

        pub const fn is_holding(&self) -> bool {
            matches!(self, Self::Holding(_, _))
        }
    }

    #[derive(Debug, Clone)]
    pub struct Yard {
        grid: Grid<Option<Container>>,
        cranes: [CraneState; Input::N],
        waiting: [ConstQueue<Container, { Input::N }>; Input::N],
        shipped: [ConstVec<Container, { Input::N }>; Input::N],
        shipped_flags: [u8; Input::N],
        shipped_count: u8,
        inversions: u16,
    }

    impl Yard {
        pub fn new(input: &Input) -> Self {
            let mut grid = Grid::with_default();
            let cranes = [0, 1, 2, 3, 4].map(|i| CraneState::Empty(Coord::new(i, 0)));
            let mut waiting = input.containers.map(|row| ConstQueue::new(row));

            for row in 0..Input::N {
                grid[Coord::new(row, 0)] = Some(waiting[row].pop_front().unwrap());
            }

            let shipped = [ConstVec::new(); Input::N];
            let shipped_flags = [0; Input::N];
            let shipped_count = 0;
            let inversions = 0;

            Self {
                grid,
                cranes,
                waiting,
                shipped,
                shipped_count,
                shipped_flags,
                inversions,
            }
        }

        pub const fn grid(&self) -> &Grid<Option<Container>> {
            &self.grid
        }

        pub const fn cranes(&self) -> &[CraneState; Input::N] {
            &self.cranes
        }

        pub const fn waiting(&self) -> &[ConstQueue<Container, { Input::N }>; Input::N] {
            &self.waiting
        }

        pub const fn inversions(&self) -> u32 {
            self.inversions as u32
        }

        pub fn apply(&mut self, operations: &[Operation; Input::N]) -> Result<(), &'static str> {
            let prev_coords = self.cranes.map(|c| c.coord());

            for crane in 0..Input::N {
                match operations[crane] {
                    Operation::Up => self.move_crane(crane, -1, 0),
                    Operation::Down => self.move_crane(crane, 1, 0),
                    Operation::Left => self.move_crane(crane, 0, -1),
                    Operation::Right => self.move_crane(crane, 0, 1),
                    Operation::Pick => self.pick(crane),
                    Operation::Drop => self.drop(crane),
                    Operation::Destroy => self.destroy(crane),
                    Operation::None => Ok(()),
                }?;
            }

            let current_coords = self.cranes.map(|c| c.coord());

            // クレーン交叉判定
            for (i, (p0, c0)) in prev_coords.iter().zip(current_coords.iter()).enumerate() {
                let (Some(p0), Some(c0)) = (p0, c0) else {
                    continue;
                };

                let p1 = &prev_coords[i + 1..];
                let c1 = &current_coords[i + 1..];

                for (p1, c1) in p1.iter().zip(c1.iter()) {
                    let (Some(p1), Some(c1)) = (p1, c1) else {
                        continue;
                    };

                    if c0 == c1 || (c0 == p1 && c1 == p0) {
                        return Err("Two cranes are in the same cell");
                    }
                }
            }

            Ok(())
        }

        fn move_crane(&mut self, crane: usize, dr: isize, dc: isize) -> Result<(), &'static str> {
            let new_state = match self.cranes[crane] {
                CraneState::Empty(coord) => CraneState::Empty(coord + CoordDiff::new(dr, dc)),
                CraneState::Holding(container, coord) => {
                    let new_coord = coord + CoordDiff::new(dr, dc);

                    if !Input::is_large_crane(crane) && self.grid[new_coord].is_some() {
                        return Err("Cannot move to a cell with a container");
                    }

                    CraneState::Holding(container, coord + CoordDiff::new(dr, dc))
                }
                CraneState::Destroyed => return Err("Cannot move a destroyed crane"),
            };

            self.cranes[crane] = new_state;
            Ok(())
        }

        fn pick(&mut self, crane: usize) -> Result<(), &'static str> {
            let CraneState::Empty(coord) = self.cranes[crane] else {
                return Err("Tried to pick up a container while holding one");
            };

            let container = self.grid[coord]
                .take()
                .ok_or("Tried to pick up a container but the cell is empty")?;
            self.cranes[crane] = CraneState::Holding(container, coord);

            Ok(())
        }

        fn drop(&mut self, crane: usize) -> Result<(), &'static str> {
            let CraneState::Holding(container, coord) = self.cranes[crane] else {
                return Err("Tried to drop a container while not holding one");
            };

            self.cranes[crane] = CraneState::Empty(coord);
            let place = self.grid[coord].as_mut();

            if place.is_some() {
                return Err("Tried to drop a container but the cell is not empty");
            }

            self.grid[coord] = Some(container);
            Ok(())
        }

        fn destroy(&mut self, crane: usize) -> Result<(), &'static str> {
            match self.cranes[crane] {
                CraneState::Empty(_) => {
                    self.cranes[crane] = CraneState::Destroyed;
                    Ok(())
                }
                CraneState::Holding(_, _) => Err("Tried to destroy a crane holding a container"),
                CraneState::Destroyed => Err("Tried to destroy a destroyed crane"),
            }
        }

        pub fn carry_in_and_ship(&mut self) {
            // 搬入
            for row in 0..Input::N {
                let cell = &mut self.grid[Coord::new(row, 0)];

                if cell.is_none() {
                    if let Some(container) = self.waiting[row].pop_front() {
                        *cell = Some(container);
                    }
                }
            }

            // 搬出
            for row in 0..Input::N {
                let cell = &mut self.grid[Coord::new(row, Input::N - 1)];

                if let Some(container) = cell.take() {
                    let min = Input::N * row;
                    let max = min + Input::N;
                    assert!(min <= container.index() && container.index() < max);

                    for &c in self.shipped[row].as_slice().iter() {
                        if container < c {
                            self.inversions += 1;
                        }
                    }

                    self.shipped_flags[row] |= 1 << (container.index() - min);
                    self.shipped[row].push(container);
                    self.shipped_count += 1;
                }
            }
        }

        pub const fn is_end(&self) -> bool {
            self.shipped_count == Input::CONTAINER_COUNT as u8
        }

        pub fn is_next_ship(&self, container: Container) -> bool {
            let row = container.index() / Input::N;
            self.next_ship_of(row) == Some(container)
        }

        pub fn next_ship_of(&self, row: usize) -> Option<Container> {
            let flag = self.shipped_flags[row];
            let min = Input::N * row;

            if flag < (1 << 5) {
                let i = (!flag).trailing_zeros() as usize;
                Some(Container::new(min + i))
            } else {
                None
            }
        }
    }

    #[derive(Debug, Clone)]
    pub struct Output {
        operations: [Vec<Operation>; Input::N],
    }

    impl Output {
        pub fn new() -> Self {
            Self {
                operations: [vec![], vec![], vec![], vec![], vec![]],
            }
        }

        pub fn push(&mut self, operations: &[Operation; Input::N]) {
            for (ops, &operation) in self.operations.iter_mut().zip(operations.iter()) {
                ops.push(operation);
            }
        }

        pub fn len(&self) -> usize {
            self.operations.iter().map(|ops| ops.len()).max().unwrap()
        }
    }

    impl Display for Output {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            for operations in self.operations.iter() {
                for operation in operations.iter() {
                    match operation {
                        Operation::Up => write!(f, "U")?,
                        Operation::Down => write!(f, "D")?,
                        Operation::Left => write!(f, "L")?,
                        Operation::Right => write!(f, "R")?,
                        Operation::Pick => write!(f, "P")?,
                        Operation::Drop => write!(f, "Q")?,
                        Operation::Destroy => write!(f, "B")?,
                        Operation::None => write!(f, ".")?,
                    }
                }

                writeln!(f)?;
            }

            Ok(())
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct Params {
        kappa_step02: f64,
        kappa_step02_best: f64,
        kappa_step03: f64,
        temp0: f64,
        temp1: f64,
        neigh_weight: [f64; 7],
    }

    impl Params {
        pub fn new() -> Self {
            let args = std::env::args().collect::<Vec<_>>();
            let kappa_step02 = args
                .get(1)
                .and_then(|s| s.parse().ok())
                .unwrap_or(0.36695712599458363);
            let kappa_step02_best = args
                .get(2)
                .and_then(|s| s.parse().ok())
                .unwrap_or(0.33853359719806914);
            let kappa_step03 = args
                .get(3)
                .and_then(|s| s.parse().ok())
                .unwrap_or(1.0943382134137645);
            let temp0 = args
                .get(4)
                .and_then(|s| s.parse().ok())
                .unwrap_or(0.26427465765573904);
            let temp1 = args
                .get(5)
                .and_then(|s| s.parse().ok())
                .unwrap_or(0.2348013030504484);
            let neigh_weight = [
                args.get(6)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.8001974732395251),
                args.get(7)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.0021234186564319402),
                args.get(8)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.07794107305552037),
                args.get(9)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.061893395349745486),
                args.get(10)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.03785676941674077),
                args.get(11)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.02113742827777307),
                args.get(12)
                    .and_then(|s| s.parse().ok())
                    .unwrap_or(0.005224236584165685),
            ];

            Self {
                kappa_step02,
                kappa_step02_best,
                kappa_step03,
                temp0,
                temp1,
                neigh_weight,
            }
        }

        pub fn kappa_step02(&self) -> f64 {
            self.kappa_step02
        }

        pub fn kappa_step02_best(&self) -> f64 {
            self.kappa_step02_best
        }

        pub fn kappa_step03(&self) -> f64 {
            self.kappa_step03
        }

        pub fn temp0(&self) -> f64 {
            self.temp0
        }

        pub fn temp1(&self) -> f64 {
            self.temp1
        }

        pub fn neigh_weight(&self) -> &[f64; 7] {
            &self.neigh_weight
        }
    }
}
mod solver {
    pub mod beam {
        use std::{array, collections::VecDeque};

        use rand::SeedableRng;
        use rand_pcg::Pcg64Mcg;

        use crate::{
            common::ChangeMinMax as _,
            grid::{Coord, ADJACENTS},
            problem::{Grid, Input, Output, Yard},
        };

        use super::{Solver, SolverResult};

        mod step01a_gen_dp {
            mod step01_01_dp {
                use itertools::iproduct;

                use crate::{common::ChangeMinMax as _, problem::Input};

                use super::{N, N2, NP1};

                /// 滞留コンテナ数の最大値を最小にするDPを行い、搬入順（何番目にどの行から搬入するか）を返す
                pub(super) fn dp(input: &Input) -> (u128, Vec<usize>) {
                    let mut counts = vec![vec![vec![vec![vec![0; NP1]; NP1]; NP1]; NP1]; NP1];

                    for i0 in 0..NP1 {
                        for i1 in 0..NP1 {
                            for i2 in 0..NP1 {
                                for i3 in 0..NP1 {
                                    for i4 in 0..NP1 {
                                        let indices = [i0, i1, i2, i3, i4];
                                        counts[i0][i1][i2][i3][i4] = count(input, indices);
                                    }
                                }
                            }
                        }
                    }

                    let mut dp =
                        vec![vec![vec![vec![vec![u128::MAX / 2; NP1]; NP1]; NP1]; NP1]; NP1];
                    dp[0][0][0][0][0] = 0;

                    let mut from =
                        vec![vec![vec![vec![vec![([0; N], 0); NP1]; NP1]; NP1]; NP1]; NP1];

                    for (i0, i1, i2, i3, i4) in iproduct!(0..NP1, 0..NP1, 0..NP1, 0..NP1, 0..NP1) {
                        let old_indices = [i0, i1, i2, i3, i4];
                        let current_dp = dp[i0][i1][i2][i3][i4];
                        let current_cnt = counts[i0][i1][i2][i3][i4];

                        for idx in 0..N {
                            let mut indices = old_indices;
                            indices[idx] += 1;

                            if indices[idx] > N {
                                continue;
                            }

                            let cnt =
                                counts[indices[0]][indices[1]][indices[2]][indices[3]][indices[4]];
                            let new_cnt = current_cnt.max(cnt);
                            let cost = current_dp + (1 << (new_cnt * 4));

                            if dp[indices[0]][indices[1]][indices[2]][indices[3]][indices[4]]
                                .change_min(cost)
                            {
                                from[indices[0]][indices[1]][indices[2]][indices[3]][indices[4]] =
                                    (old_indices, idx);
                            }
                        }
                    }

                    let mut current = [N; N];
                    let mut history = vec![];

                    while current != [0; N] {
                        let (prev, index) =
                            from[current[0]][current[1]][current[2]][current[3]][current[4]];
                        history.push(index);
                        current = prev;
                    }

                    history.reverse();

                    (dp[N][N][N][N][N], history)
                }

                fn count(input: &Input, indices: [usize; N]) -> u32 {
                    let mut contains = [false; N2];

                    for (i, &c) in indices.iter().enumerate() {
                        for &c in input.containers()[i][..c].iter() {
                            contains[c.index()] = true;
                        }
                    }

                    let mut count = indices.iter().map(|&i| i as u32).sum::<u32>();

                    for i in 0..N {
                        let slice = &contains[i * N..(i + 1) * N];
                        count -= slice.iter().take_while(|&&b| b).count() as u32;
                    }

                    count
                }
            }

            use crate::{
                common::ChangeMinMax as _,
                grid::Coord,
                problem::{Container, Grid, Input},
                solver::beam::step01a_gen_dp::step01_01_dp::dp,
            };
            use itertools::Itertools;
            use rand::prelude::*;
            use rand::Rng;

            const N: usize = Input::N;
            const NP1: usize = N + 1;
            const N2: usize = N * N;

            #[derive(Debug, Clone)]
            pub struct Task {
                crane: Option<u8>,
                container: Container,
                from: Coord,
                to: Coord,
            }

            impl Task {
                pub fn new(
                    crane: Option<u8>,
                    container: Container,
                    from: Coord,
                    to: Coord,
                ) -> Self {
                    Self {
                        crane,
                        container,
                        from,
                        to,
                    }
                }

                pub fn crane(&self) -> Option<usize> {
                    self.crane.map(|c| c as usize)
                }

                pub fn container(&self) -> Container {
                    self.container
                }

                pub fn from(&self) -> Coord {
                    self.from
                }

                pub fn to(&self) -> Coord {
                    self.to
                }
            }

            pub(super) fn generate_tasks(
                input: &Input,
                rng: &mut impl Rng,
            ) -> Result<Vec<Task>, &'static str> {
                let (_, history) = dp(input);

                let mut tasks = vec![];
                let mut containers = input
                    .containers()
                    .map(|c| c.iter().copied().rev().collect_vec());

                let mut storages = [
                    Coord::new(0, 3),
                    Coord::new(2, 3),
                    Coord::new(4, 3),
                    Coord::new(0, 2),
                    Coord::new(2, 2),
                    Coord::new(4, 2),
                ];

                let mut board = Grid::new([false; Input::CONTAINER_COUNT]);
                board[Coord::new(0, 0)] = true;
                board[Coord::new(1, 0)] = true;
                board[Coord::new(2, 0)] = true;
                board[Coord::new(3, 0)] = true;
                board[Coord::new(4, 0)] = true;

                let mut positions = [None; Input::CONTAINER_COUNT];
                let mut next_shippings = [0, 5, 10, 15, 20];

                for &row in history.iter() {
                    let container = containers[row].pop().unwrap();
                    let from = Coord::new(row, 0);
                    let to = Input::get_goal(container);

                    // 搬入口クリア
                    if containers[row].is_empty() {
                        board[from] = false;
                    }

                    if container.index() == next_shippings[to.row()] {
                        next_shippings[to.row()] += 1;
                        let task = Task::new(None, container, from, to);
                        tasks.push(task);
                    } else {
                        // ベストな場所を探す
                        let mut best_pos = None;
                        let mut best_cost = usize::MAX;
                        storages.shuffle(rng);

                        for &cand in storages.iter() {
                            if board[cand] {
                                continue;
                            }

                            let cost = from.dist(&cand) + cand.dist(&to);

                            if best_cost.change_min(cost) {
                                best_pos = Some(cand);
                            }
                        }

                        match best_pos {
                            Some(best_pos) => {
                                positions[container.index()] = Some(best_pos);
                                let task = Task::new(None, container, from, best_pos);
                                tasks.push(task);
                                board[best_pos] = true;
                            }
                            None => {
                                return Err("storage positions are occupied");
                            }
                        }
                    }

                    loop {
                        let mut next = None;
                        const ORDER: [usize; 25] = [
                            0, 5, 10, 15, 20, 1, 6, 11, 16, 21, 2, 7, 12, 17, 22, 3, 8, 13, 18, 23,
                            4, 9, 14, 19, 24,
                        ];

                        for &container in ORDER.iter() {
                            let Some(pos) = positions[container] else {
                                continue;
                            };

                            if next_shippings[Input::get_goal(Container::new(container)).row()]
                                != container
                            {
                                continue;
                            }

                            next = Some((container, pos));
                            break;
                        }

                        let Some((container, pos)) = next else {
                            break;
                        };

                        board[pos] = false;
                        positions[container] = None;
                        next_shippings[Input::get_goal(Container::new(container)).row()] += 1;
                        let task = Task::new(
                            None,
                            Container::new(container),
                            pos,
                            Input::get_goal(Container::new(container)),
                        );
                        tasks.push(task);
                    }
                }

                Ok(tasks)
            }
        }
        mod step01b_gen_beam {
            use super::{step01a_gen_dp::Task, DistDict, Precalc};
            use crate::{
                beam::{BayesianBeamWidthSuggester, BeamWidthSuggester as _},
                common::ChangeMinMax as _,
                data_structures::{History, HistoryIndex},
                grid::Coord,
                problem::{Container, Grid, Input},
            };
            use itertools::Itertools;
            use rand::prelude::*;
            use rand::Rng;
            use rand_pcg::Pcg64Mcg;
            use std::{array, collections::HashSet};

            const MAX_TURN: usize = 80;

            pub(super) fn generate_tasks(
                input: &Input,
                precalc: &Precalc,
            ) -> Result<Vec<Task>, &'static str> {
                let env = Env::new(&input, &precalc.dist_dict);
                let mut beam = vec![vec![vec![]; State::STORAGE_COUNT + 1]; u8::MAX as usize];
                beam[0][0].push(State::init(&env));
                let mut history = History::new();
                let mut rng = Pcg64Mcg::from_entropy();
                let mut completed_list: Vec<Option<State>> = vec![None; u8::MAX as usize];
                let mut beam_width_suggester =
                    BayesianBeamWidthSuggester::new(MAX_TURN, 5, 0.5, 3000, 300, 10000, 1);
                let mut hashset = HashSet::new();

                for turn in 0..MAX_TURN {
                    let beam_width = beam_width_suggester.suggest();

                    if let Some(completed) = completed_list[turn as usize] {
                        let tasks = history.collect(completed.history);
                        eprintln!("{}", completed.score);
                        eprintln!("{:?}", completed.crane_avail_turns);
                        eprintln!("1st beam turn: {}", turn);
                        return Ok(tasks);
                    }

                    for storage in 0..=State::STORAGE_COUNT {
                        let mut b = vec![];
                        std::mem::swap(&mut b, &mut beam[turn as usize][storage as usize]);

                        for state in b.iter() {
                            if state.is_completed() {
                                let t = *state
                                    .crane_avail_turns
                                    .iter()
                                    .filter(|&&t| t < u8::MAX)
                                    .max()
                                    .unwrap() as usize;

                                if completed_list[t].is_none() {
                                    completed_list[t] = Some(*state);
                                }
                            }
                        }

                        b.sort_unstable();
                        hashset.clear();
                        let mut count = 0;

                        for state in b.iter_mut() {
                            if !hashset.insert(state.hash) {
                                continue;
                            }

                            count += 1;

                            if count >= beam_width {
                                break;
                            }

                            state.gen_next(&env, &mut beam, &mut history, &mut rng);
                        }
                    }
                }

                Err("Failed to find a solution")
            }

            struct Env<'a> {
                input: &'a Input,
                dist_dict: &'a DistDict,
                index_grid: Grid<usize>,
                crane_hash: [Grid<u64>; Input::N],
                container_hash: [Grid<u64>; Input::CONTAINER_COUNT],
            }

            impl<'a> Env<'a> {
                fn new(input: &'a Input, dist_dict: &'a DistDict) -> Self {
                    let mut index_grid = Grid::new([!0; Input::N * Input::N]);

                    for (i, c) in State::POS.iter().enumerate() {
                        index_grid[c] = i;
                    }

                    let mut rng = Pcg64Mcg::from_entropy();
                    let mut crane_hash = [Grid::new([0; Input::N * Input::N]); Input::N];
                    let mut container_hash =
                        [Grid::new([0; Input::N * Input::N]); Input::CONTAINER_COUNT];

                    for row in 0..Input::N {
                        for col in 0..Input::N {
                            let c = Coord::new(row, col);

                            for i in 0..Input::N {
                                crane_hash[i][c] = rng.gen();
                            }

                            for i in 0..Input::CONTAINER_COUNT {
                                container_hash[i][c] = rng.gen();
                            }
                        }
                    }

                    Self {
                        input,
                        dist_dict,
                        index_grid,
                        crane_hash,
                        container_hash,
                    }
                }
            }

            #[derive(Debug, Clone, Copy)]
            struct State {
                in_ptr: [u8; Input::N],
                container_avail_turns: [u8; Input::N + Self::STORAGE_COUNT],
                containers: [Option<Container>; Input::N + Self::STORAGE_COUNT],
                cranes: [Coord; Input::N],
                out_next: [u8; Input::N],
                out_avail_turns: [u8; Input::N],
                crane_avail_turns: [u8; Input::N],
                crane_score_per_turn: [f32; Input::N],
                temp_count: u8,
                finished_container_count: u8,
                score: f32,
                hash: u64,
                history: HistoryIndex,
            }

            impl State {
                const STORAGE_COUNT: usize = 6;
                const STORAGES: [Coord; Self::STORAGE_COUNT] = [
                    Coord::new(0, 2),
                    Coord::new(2, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(2, 3),
                    Coord::new(4, 3),
                ];
                const POS: [Coord; Input::N * 2 + Self::STORAGE_COUNT] = [
                    // 搬入口
                    Coord::new(0, 0),
                    Coord::new(1, 0),
                    Coord::new(2, 0),
                    Coord::new(3, 0),
                    Coord::new(4, 0),
                    // 一時保管
                    Coord::new(0, 2),
                    Coord::new(2, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(2, 3),
                    Coord::new(4, 3),
                    // 搬出口
                    Coord::new(0, 4),
                    Coord::new(1, 4),
                    Coord::new(2, 4),
                    Coord::new(3, 4),
                    Coord::new(4, 4),
                ];
                const CRANE_AVAIL: u8 = u8::MAX;

                fn init(env: &Env) -> Self {
                    let mut containers = [None; Input::N + Self::STORAGE_COUNT];
                    let mut hash = 0;

                    for i in 0..Input::N {
                        let container = env.input.containers()[i][0];
                        containers[i] = Some(container);
                        hash ^= env.crane_hash[i][Self::POS[i]];
                        hash ^= env.container_hash[container.index()][Self::POS[i]];
                    }

                    Self {
                        in_ptr: [0; Input::N],
                        container_avail_turns: [0; Input::N + Self::STORAGE_COUNT],
                        containers,
                        temp_count: 0,
                        finished_container_count: 0,
                        cranes: array::from_fn(|i| Coord::new(i, 0)),
                        out_next: [0, 5, 10, 15, 20],
                        out_avail_turns: [0; Input::N],
                        crane_avail_turns: [0; Input::N],
                        crane_score_per_turn: [0.0; Input::N],
                        score: 0.0,
                        hash,
                        history: HistoryIndex::ROOT,
                    }
                }

                fn is_completed(&self) -> bool {
                    self.finished_container_count == Input::CONTAINER_COUNT as u8
                }

                fn gen_next(
                    &mut self,
                    env: &Env,
                    beam: &mut Vec<Vec<Vec<Self>>>,
                    history: &mut History<Task>,
                    rng: &mut impl Rng,
                ) {
                    let mut container_counts = [0; Input::N + Self::STORAGE_COUNT];
                    let mut container_suffix_sum = [0; Input::N + Self::STORAGE_COUNT];

                    for i in 0..Input::N {
                        container_counts[i] = Input::N - self.in_ptr[i] as usize;
                    }

                    for i in Input::N..Input::N + Self::STORAGE_COUNT {
                        container_counts[i] = self.containers[i].is_some() as usize;
                    }

                    container_suffix_sum[Input::N + Self::STORAGE_COUNT - 1] =
                        container_counts[Input::N + Self::STORAGE_COUNT - 1];

                    for i in (0..Input::N + Self::STORAGE_COUNT - 1).rev() {
                        container_suffix_sum[i] = container_suffix_sum[i + 1] + container_counts[i];
                    }

                    let turn = self.crane_avail_turns.iter().copied().min().unwrap();
                    let mut available_crane_count = 0;

                    for (c, t) in self
                        .crane_avail_turns
                        .iter_mut()
                        .zip(self.crane_score_per_turn.iter_mut())
                    {
                        if *c == turn {
                            *c = Self::CRANE_AVAIL;
                            *t = 0.0;
                            available_crane_count += 1;
                        }
                    }

                    // コンテナ数とクレーン数の小さい方の数だけ操作を割り付ける必要がある
                    let remaining = container_suffix_sum[0].min(available_crane_count);

                    let mut state = *self;
                    state.dfs(
                        env,
                        beam,
                        history,
                        &container_suffix_sum,
                        turn,
                        0,
                        remaining,
                        rng,
                    );
                }

                fn dfs(
                    &mut self,
                    env: &Env,
                    beam: &mut Vec<Vec<Vec<Self>>>,
                    history: &mut History<Task>,
                    container_suffix_sum: &[usize; Input::N + Self::STORAGE_COUNT],
                    turn: u8,
                    search_i: usize,
                    remaining: usize,
                    rng: &mut impl Rng,
                ) {
                    if remaining == 0 {
                        let next_turn = self.crane_avail_turns.iter().copied().min().unwrap();

                        if next_turn > MAX_TURN as u8 {
                            return;
                        }

                        let score_per_turn = self.crane_score_per_turn.iter().copied().sum::<f32>();
                        assert!(next_turn > turn);

                        for t in self.crane_avail_turns.iter_mut() {
                            if *t == Self::CRANE_AVAIL {
                                *t = next_turn;
                            }
                        }

                        self.score += score_per_turn * (next_turn - turn) as f32;
                        beam[next_turn as usize][self.temp_count as usize].push(*self);
                        return;
                    }

                    for container_i in search_i..Input::N + Self::STORAGE_COUNT {
                        if container_suffix_sum[container_i] < remaining {
                            break;
                        }

                        let Some(container) = self.containers[container_i] else {
                            continue;
                        };

                        let from = Self::POS[container_i];
                        let goal = Input::get_goal(container);

                        let to_out = self.out_next[goal.row()] == container.index() as u8;
                        let to_temp =
                            container_i < Input::N && self.temp_count < Self::STORAGE_COUNT as u8;

                        if !to_out && !to_temp {
                            continue;
                        }

                        let mut state = *self;
                        state.hash ^= env.container_hash[container.index()][from];

                        // craneのアサイン
                        let mut best_crane = !0;
                        let mut best_dist = usize::MAX;

                        for (crane, (&t, &c)) in self
                            .crane_avail_turns
                            .iter()
                            .zip(self.cranes.iter())
                            .enumerate()
                        {
                            if t == Self::CRANE_AVAIL && best_dist.change_min(c.dist(&from)) {
                                best_crane = crane;
                            }
                        }

                        let crane_pos = self.cranes[best_crane];
                        let consider_container = !Input::is_large_crane(best_crane);
                        state.hash ^= env.crane_hash[best_crane][crane_pos];

                        // in側の更新
                        let storage_flag = env
                            .dist_dict
                            .get_flag(|c| state.containers[env.index_grid[c]].is_some());

                        // pickを開始するターン
                        let move_len =
                            env.dist_dict.dist(storage_flag, crane_pos, from, false) as u8;
                        let pick_turn =
                            (turn + move_len).max(state.container_avail_turns[container_i]);
                        state.container_avail_turns[container_i] = pick_turn + 2;

                        // pick_work: Pickの有効作業量。一時保管場所からのPickは無駄な作業なので0
                        let pick_work = if container_i < Input::N {
                            let in_ptr = &mut state.in_ptr[container_i];
                            *in_ptr += 1;
                            state.containers[container_i] = env.input.containers()[container_i]
                                .get(*in_ptr as usize)
                                .copied();
                            1
                        } else {
                            state.containers[container_i] = None;
                            state.temp_count -= 1;
                            0
                        };

                        // クレーンの戻り距離に応じた仕事量
                        // 左方向の移動は作業に貢献しており、それ以外の移動は貢献していないと見なす
                        let crane_back_work = crane_pos.col().saturating_sub(from.col()) as i32;

                        // out側の更新
                        if to_out {
                            // そのまま搬出
                            state.out_next[goal.row()] += 1;
                            state.finished_container_count += 1;
                            state.hash ^= env.container_hash[container.index()][goal];

                            let dist = from.dist(&goal);
                            let prev_potential = dist as i32;

                            // クレーンの左移動 + Pick + ゴールまでの移動 + Dropが仕事量
                            let total_work = crane_back_work + pick_work + prev_potential + 1;

                            // dropが開始するターン
                            let move_len =
                                env.dist_dict
                                    .dist(storage_flag, from, goal, consider_container)
                                    as u8;
                            let drop_turn =
                                (pick_turn + 1 + move_len).max(state.out_avail_turns[goal.row()]);
                            state.out_avail_turns[goal.row()] = drop_turn + 2;

                            // クレーンが使用可能になるターン
                            let crane_avail_turn = drop_turn + 1;
                            let total_turn = crane_avail_turn - turn;
                            let score_per_turn = total_work as f32 / total_turn as f32;
                            state.crane_score_per_turn[best_crane] = score_per_turn;
                            state.crane_avail_turns[best_crane] = crane_avail_turn;
                            state.cranes[best_crane] = goal;
                            state.hash ^= env.crane_hash[best_crane][goal];

                            // historyの追加
                            let task = Task::new(Some(best_crane as u8), container, from, goal);
                            state.history = history.push(task, state.history);
                            state.dfs(
                                env,
                                beam,
                                history,
                                container_suffix_sum,
                                turn,
                                container_i,
                                remaining - 1,
                                rng,
                            );
                        } else {
                            // 一時保管場所に運ぶ
                            let mut candidates = (Input::N..Self::STORAGE_COUNT + Input::N)
                                .filter(|&i| state.containers[i].is_none())
                                .collect_vec();
                            let dists = Self::STORAGES.map(|c| {
                                env.dist_dict
                                    .dist(storage_flag, from, c, consider_container)
                                    + env.dist_dict.dist(storage_flag, c, goal, true)
                            });
                            candidates.shuffle(rng);
                            candidates.sort_by_key(|&i| dists[i - Input::N]);
                            const TAKE_COUNT: usize = 3;
                            state.temp_count += 1;

                            for &temp_i in candidates.iter().take(TAKE_COUNT) {
                                let mut state = state;
                                state.containers[temp_i] = Some(container);
                                let to = Self::POS[temp_i];
                                state.hash ^= env.container_hash[container.index()][to];
                                let prev_potential = from.dist(&goal) as i32;
                                let new_potential = to.dist(&goal) as i32;
                                let potential_diff = prev_potential - new_potential;

                                // クレーンの左移動 + Pick + ゴールまでの移動 + Dropが仕事量
                                // potential_diffが負になること、一時保管場所にDropする操作は進捗を生まないことに注意
                                let total_work = crane_back_work + pick_work + potential_diff;
                                let move_len =
                                    env.dist_dict
                                        .dist(storage_flag, from, to, consider_container)
                                        as u8;
                                let drop_turn = (pick_turn + 1 + move_len)
                                    .max(state.container_avail_turns[temp_i]);
                                state.container_avail_turns[temp_i] = drop_turn + 2;
                                state.cranes[best_crane] = to;

                                let crane_avail_turn = drop_turn + 1;
                                let total_turn = crane_avail_turn - turn;
                                let score_per_turn = total_work as f32 / total_turn as f32;
                                state.crane_score_per_turn[best_crane] = score_per_turn;
                                state.crane_avail_turns[best_crane] = crane_avail_turn;
                                state.hash ^= env.crane_hash[best_crane][to];

                                // historyの追加
                                let task = Task::new(Some(best_crane as u8), container, from, to);
                                state.history = history.push(task, state.history);
                                state.dfs(
                                    env,
                                    beam,
                                    history,
                                    container_suffix_sum,
                                    turn,
                                    container_i,
                                    remaining - 1,
                                    rng,
                                );
                            }
                        }
                    }
                }
            }

            impl PartialEq for State {
                fn eq(&self, other: &Self) -> bool {
                    self.score == other.score
                }
            }

            impl Eq for State {}

            impl PartialOrd for State {
                fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                    self.score.partial_cmp(&other.score).map(|c| c.reverse())
                }
            }

            impl Ord for State {
                fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                    self.score.partial_cmp(&other.score).unwrap().reverse()
                }
            }
        }
        mod step02_order {
            mod step02_01_annealing {
                use super::{Env, State, TaskType};
                use crate::{
                    common::ChangeMinMax as _,
                    grid::Coord,
                    problem::{Container, Input},
                    solver::beam::step02_order::Turns,
                };
                use itertools::Itertools;
                use rand::prelude::*;
                use rand_distr::WeightedAliasIndex;

                const STORAGES: [Coord; 6] = [
                    Coord::new(0, 2),
                    Coord::new(2, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(2, 3),
                    Coord::new(4, 3),
                ];

                pub(super) fn annealing(
                    env: &Env,
                    initial_solution: State,
                    duration: f64,
                ) -> State {
                    let mut state = initial_solution;
                    let mut best_state = state.clone();
                    let mut current_score = state.calc_score(env, usize::MAX).unwrap();
                    let mut best_score = state.calc_score_best_state(env, usize::MAX).unwrap();
                    let init_score = best_score;

                    let mut all_iter = 0;
                    let mut valid_iter = 0;
                    let mut accepted_count = 0;
                    let mut update_count = 0;
                    let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

                    let duration_inv = 1.0 / duration;
                    let since = std::time::Instant::now();

                    let temp0 = env.input.params().temp0();
                    let temp1 = env.input.params().temp1();
                    let mut temp = temp0;
                    let neigh_weights =
                        WeightedAliasIndex::new(env.input.params().neigh_weight().to_vec())
                            .unwrap();

                    loop {
                        all_iter += 1;
                        if (all_iter & ((1 << 4) - 1)) == 0 {
                            let time =
                                (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                            temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);

                            if time >= 1.0 {
                                break;
                            }
                        }

                        // 変形
                        let neigh_type = neigh_weights.sample(&mut rng);
                        let neigh = match neigh_type {
                            0 => SwapAfter::gen(&state, &mut rng),
                            1 => Move::gen(&state, &mut rng),
                            2 => SwapSingle::gen(&state, &mut rng),
                            3 => SwapInCrane::gen(&state, &mut rng),
                            4 => ChangeStoragePos::gen(&state, &mut rng),
                            5 => SwapStoragePos::gen(&state, &mut rng),
                            6 => SwapStorageAll::gen(&state, &mut rng),
                            _ => unreachable!(),
                        };
                        let Some(neigh) = neigh else {
                            continue;
                        };
                        let new_state = neigh.neigh(env, state.clone());

                        // スコア計算
                        let threshold = current_score - temp * rng.gen_range(0.0f64..1.0).ln();
                        let Ok(turns) = new_state.simulate::<Turns>(env, threshold as usize) else {
                            continue;
                        };
                        let new_score = turns.calc_score(env);

                        if new_score <= threshold {
                            // 解の更新
                            current_score = new_score;
                            accepted_count += 1;
                            state = new_state;

                            if best_score.change_min(turns.calc_score_best_state(env)) {
                                best_state = state.clone();
                                update_count += 1;
                            }
                        }

                        valid_iter += 1;
                    }

                    eprintln!("===== annealing =====");
                    eprintln!("init score : {}", init_score);
                    eprintln!("score      : {}", best_score);
                    eprintln!("all iter   : {}", all_iter);
                    eprintln!("valid iter : {}", valid_iter);
                    eprintln!("accepted   : {}", accepted_count);
                    eprintln!("updated    : {}", update_count);
                    eprintln!("");

                    best_state
                }

                trait Neigh {
                    fn neigh(&self, env: &Env, state: State) -> State;
                }

                struct SwapAfter {
                    crane0: usize,
                    crane1: usize,
                    index0: usize,
                    index1: usize,
                }

                impl SwapAfter {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let crane0 = rng.gen_range(0..Input::N);
                        let crane1 = (crane0 + rng.gen_range(1..Input::N)) % Input::N;
                        let index0 = rng.gen_range(0..=state.tasks[crane0].len());
                        let index1 = rng.gen_range(0..=state.tasks[crane1].len());

                        Some(Box::new(Self {
                            crane0,
                            crane1,
                            index0,
                            index1,
                        }))
                    }
                }

                impl Neigh for SwapAfter {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        let temp0 = state.tasks[self.crane0].drain(self.index0..).collect_vec();
                        let temp1 = state.tasks[self.crane1].drain(self.index1..).collect_vec();
                        state.tasks[self.crane0].extend(temp1);
                        state.tasks[self.crane1].extend(temp0);

                        state
                    }
                }

                struct Move {
                    crane0: usize,
                    index0: usize,
                    crane1: usize,
                    index1: usize,
                }

                impl Move {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let candidates = (0..Input::N)
                            .filter(|&i| state.tasks[i].len() >= 1)
                            .collect_vec();

                        let crane0 = *candidates.choose(rng)?;
                        let crane1 = (crane0 + rng.gen_range(1..Input::N)) % Input::N;
                        let index0 = rng.gen_range(0..state.tasks[crane0].len());
                        let index1 = rng.gen_range(0..=state.tasks[crane1].len());

                        Some(Box::new(Self {
                            crane0,
                            index0,
                            crane1,
                            index1,
                        }))
                    }
                }

                impl Neigh for Move {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        let task = state.tasks[self.crane0].remove(self.index0);
                        state.tasks[self.crane1].insert(self.index1, task);

                        state
                    }
                }

                struct SwapSingle {
                    crane0: usize,
                    crane1: usize,
                    index0: usize,
                    index1: usize,
                }

                impl SwapSingle {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let candidates = (0..Input::N)
                            .filter(|&i| state.tasks[i].len() >= 1)
                            .collect_vec();

                        if candidates.len() < 2 {
                            return None;
                        }

                        let mut cranes = candidates.choose_multiple(rng, 2);
                        let crane0 = *cranes.next().unwrap();
                        let crane1 = *cranes.next().unwrap();

                        let index0 = rng.gen_range(0..state.tasks[crane0].len());
                        let index1 = rng.gen_range(0..state.tasks[crane1].len());
                        Some(Box::new(Self {
                            crane0,
                            crane1,
                            index0,
                            index1,
                        }))
                    }
                }

                impl Neigh for SwapSingle {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        let temp = state.tasks[self.crane0][self.index0];
                        state.tasks[self.crane0][self.index0] =
                            state.tasks[self.crane1][self.index1];
                        state.tasks[self.crane1][self.index1] = temp;

                        state
                    }
                }

                struct SwapInCrane {
                    crane: usize,
                    index: usize,
                }

                impl SwapInCrane {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let candidates = (0..Input::N)
                            .filter(|&i| state.tasks[i].len() >= 2)
                            .collect_vec();
                        let crane = *candidates.choose(rng)?;
                        let index = rng.gen_range(0..state.tasks[crane].len() - 1);

                        Some(Box::new(Self { crane, index }))
                    }
                }

                impl Neigh for SwapInCrane {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        state.tasks[self.crane].swap(self.index, self.index + 1);
                        state
                    }
                }

                struct ChangeStoragePos {
                    container: Container,
                    pos: Coord,
                }

                impl ChangeStoragePos {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let mut candidates = vec![];

                        for tasks in state.tasks.iter() {
                            for &task in tasks.iter() {
                                if let TaskType::ToTemporary(container, _, to) = task {
                                    candidates.push((container, to));
                                }
                            }
                        }

                        let &(container, _) = candidates.choose(rng)?;
                        let &pos = STORAGES.choose(rng).unwrap();

                        Some(Box::new(Self { container, pos }))
                    }
                }

                impl Neigh for ChangeStoragePos {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        for tasks in state.tasks.iter_mut() {
                            for task in tasks.iter_mut() {
                                match task {
                                    TaskType::ToTemporary(container, _, to) => {
                                        if *container == self.container {
                                            *to = self.pos;
                                        }
                                    }
                                    TaskType::FromTemporary(container, from, _) => {
                                        if *container == self.container {
                                            *from = self.pos;
                                        }
                                    }
                                    _ => {}
                                }
                            }
                        }

                        state
                    }
                }

                struct SwapStoragePos {
                    container0: Container,
                    container1: Container,
                    pos0: Coord,
                    pos1: Coord,
                }

                impl SwapStoragePos {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let mut candidates = vec![];

                        for tasks in state.tasks.iter() {
                            for &task in tasks.iter() {
                                if let TaskType::ToTemporary(container, _, to) = task {
                                    candidates.push((container, to));
                                }
                            }
                        }

                        if candidates.len() < 2 {
                            return None;
                        }

                        let mut chosen = candidates.choose_multiple(rng, 2);

                        let &(container0, pos0) = chosen.next().unwrap();
                        let &(container1, pos1) = chosen.next().unwrap();

                        Some(Box::new(Self {
                            container0,
                            container1,
                            pos0,
                            pos1,
                        }))
                    }
                }

                impl Neigh for SwapStoragePos {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        for tasks in state.tasks.iter_mut() {
                            for task in tasks.iter_mut() {
                                match task {
                                    TaskType::ToTemporary(container, _, to) => {
                                        if *container == self.container0 {
                                            *to = self.pos1;
                                        } else if *container == self.container1 {
                                            *to = self.pos0;
                                        }
                                    }
                                    TaskType::FromTemporary(container, from, _) => {
                                        if *container == self.container0 {
                                            *from = self.pos1;
                                        } else if *container == self.container1 {
                                            *from = self.pos0;
                                        }
                                    }
                                    _ => {}
                                }
                            }
                        }

                        state
                    }
                }

                struct SwapStorageAll {
                    pos0: Coord,
                    pos1: Coord,
                }

                impl SwapStorageAll {
                    fn gen(_state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let mut pos = STORAGES.choose_multiple(rng, 2);
                        let pos0 = *pos.next().unwrap();
                        let pos1 = *pos.next().unwrap();
                        Some(Box::new(Self { pos0, pos1 }))
                    }
                }

                impl Neigh for SwapStorageAll {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        for tasks in state.tasks.iter_mut() {
                            for task in tasks.iter_mut() {
                                match task {
                                    TaskType::ToTemporary(_, _, to) => {
                                        if *to == self.pos0 {
                                            *to = self.pos1;
                                        } else if *to == self.pos1 {
                                            *to = self.pos0;
                                        }
                                    }
                                    TaskType::FromTemporary(_, from, _) => {
                                        if *from == self.pos0 {
                                            *from = self.pos1;
                                        } else if *from == self.pos1 {
                                            *from = self.pos0;
                                        }
                                    }
                                    _ => {}
                                }
                            }
                        }

                        state
                    }
                }
            }
            mod step02_02_breakdown {
                use crate::problem::{Grid, Input};

                use super::{Env, Recorder, State, SubTask};

                pub(super) fn breakdown(
                    env: &Env,
                    state: &State,
                ) -> Result<[Vec<SubTask>; Input::N], &'static str> {
                    let subtasks: SubTaskRecorder = state.simulate(env, usize::MAX)?;
                    Ok(subtasks.tasks)
                }

                struct SubTaskRecorder {
                    tasks: [Vec<SubTask>; 5],
                    grid: Grid<usize>,
                }

                impl Recorder for SubTaskRecorder {
                    fn new() -> Self {
                        Self {
                            tasks: [vec![], vec![], vec![], vec![], vec![]],
                            grid: Grid::with_default(),
                        }
                    }

                    fn record_pick(&mut self, crane: usize, coord: crate::grid::Coord) {
                        let task = SubTask::Pick(coord, self.grid[coord]);
                        self.tasks[crane].push(task);
                        self.grid[coord] += 1;
                    }

                    fn record_drop(&mut self, crane: usize, coord: crate::grid::Coord) {
                        let task = SubTask::Drop(coord, self.grid[coord]);
                        self.tasks[crane].push(task);
                        self.grid[coord] += 1;
                    }

                    fn record_turn(&mut self, _turns: [usize; Input::N]) {
                        // シミュレーションが終わったので、EOOを追加
                        for tasks in self.tasks.iter_mut() {
                            tasks.push(SubTask::EndOfOrder);
                        }
                    }
                }
            }

            use super::{step01a_gen_dp::Task, DistDict, Precalc};
            use crate::{
                common::ChangeMinMax,
                grid::Coord,
                problem::{Container, CraneState, Grid, Input},
            };
            use rand::prelude::*;
            use rand::SeedableRng;
            use rand_pcg::Pcg64Mcg;
            use std::array;

            pub(super) fn order_tasks(
                input: &Input,
                precalc: &Precalc,
                tasks: Vec<Vec<Task>>,
            ) -> Result<[Vec<SubTask>; Input::N], &'static str> {
                let env = Env::new(&input, &precalc.dist_dict);
                let mut rng = Pcg64Mcg::from_entropy();

                let mut best_state = None;
                let mut best_score = f64::INFINITY;

                if tasks.len() == 0 {
                    return Err("no tasks");
                }

                let step1_duration = 0.5 / tasks.len() as f64;
                let step2_duration = 1.0;

                'main: for tasks in tasks {
                    let mut state = State::new(&tasks, |c, i| c.unwrap_or(i % Input::N));
                    let mut trial_count = 0;

                    loop {
                        if state.calc_score(&env, 1000).is_ok() {
                            break;
                        }

                        trial_count += 1;

                        if trial_count >= 10000 {
                            continue 'main;
                        }

                        eprintln!("retrying...");
                        state = State::new(&tasks, |_, _| rng.gen_range(0..Input::N));
                    }

                    let state = step02_01_annealing::annealing(&env, state, step1_duration);

                    if best_score.change_min(
                        state
                            .calc_score_best_state(&env, usize::MAX)
                            .unwrap_or(f64::INFINITY),
                    ) {
                        best_state = Some(state);
                    }
                }

                let Some(state) = best_state else {
                    return Err("no valid state found");
                };

                let state = step02_01_annealing::annealing(&env, state, step2_duration);

                step02_02_breakdown::breakdown(&env, &state)
            }

            #[derive(Debug, Clone, Copy)]
            enum TaskType {
                Direct(Container, Coord, Coord),
                ToTemporary(Container, Coord, Coord),
                FromTemporary(Container, Coord, Coord),
            }

            #[derive(Debug, Clone)]
            struct Env<'a> {
                input: &'a Input,
                dist_dict: &'a DistDict,
            }

            impl<'a> Env<'a> {
                fn new(input: &'a Input, dist_dict: &'a DistDict) -> Self {
                    Self { input, dist_dict }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                tasks: [Vec<TaskType>; Input::N],
            }

            impl State {
                fn new(
                    tasks: &[Task],
                    mut assign_fn: impl FnMut(Option<usize>, usize) -> usize,
                ) -> Self {
                    let mut assigned_tasks = [vec![], vec![], vec![], vec![], vec![]];

                    for (i, task) in tasks.iter().enumerate() {
                        let t = if task.from().col() == 0 {
                            if task.to().col() == Input::N - 1 {
                                TaskType::Direct(task.container(), task.from(), task.to())
                            } else {
                                TaskType::ToTemporary(task.container(), task.from(), task.to())
                            }
                        } else {
                            TaskType::FromTemporary(task.container(), task.from(), task.to())
                        };

                        assigned_tasks[assign_fn(task.crane(), i)].push(t);
                    }

                    Self {
                        tasks: assigned_tasks,
                    }
                }

                fn calc_score(&self, env: &Env, max_turn: usize) -> Result<f64, &'static str> {
                    let turns: Turns = self.simulate(env, max_turn)?;
                    Ok(turns.calc_score(env))
                }

                fn calc_score_best_state(
                    &self,
                    env: &Env,
                    max_turn: usize,
                ) -> Result<f64, &'static str> {
                    let turns: Turns = self.simulate(env, max_turn)?;
                    Ok(turns.calc_score_best_state(env))
                }

                fn simulate<T: Recorder>(
                    &self,
                    env: &Env,
                    max_turn: usize,
                ) -> Result<T, &'static str> {
                    let mut recorder = T::new();
                    let mut in_ptr = [0; Input::N];
                    let mut out_next: [_; Input::N] = array::from_fn(|i| i * Input::N);
                    let mut task_ptr = [0; Input::N];
                    let mut cranes: [_; Input::N] =
                        array::from_fn(|i| CraneState::Empty(Coord::new(i, 0)));
                    let mut yard = Grid::new([None; Input::N * Input::N]);
                    let mut avail_turns = Grid::new([0; Input::N * Input::N]);
                    let mut last_turns = [0; Input::N];
                    let mut no_progress = 0;

                    for row in 0..Input::N {
                        yard[Coord::new(row, 0)] = Some(env.input.containers()[row][0]);
                    }

                    for turn in 1.. {
                        if turn > max_turn {
                            return Err("turn limit exceeded");
                        }

                        let prev_state = cranes.clone();
                        let flag = env.dist_dict.get_flag(|c| yard[c].is_some());

                        for crane_i in 0..Input::N {
                            let crane = &mut cranes[crane_i];
                            let Some(task) = self.tasks[crane_i].get(task_ptr[crane_i]).copied()
                            else {
                                continue;
                            };

                            match *crane {
                                CraneState::Empty(coord) => {
                                    let (container, from) = match task {
                                        TaskType::Direct(container, from, _) => (container, from),
                                        TaskType::ToTemporary(container, from, _) => {
                                            (container, from)
                                        }
                                        TaskType::FromTemporary(container, from, _) => {
                                            (container, from)
                                        }
                                    };

                                    let can_pick =
                                        yard[from] == Some(container) && avail_turns[from] <= turn;

                                    if coord == from && can_pick {
                                        // コンテナをPickする
                                        *crane = CraneState::Holding(container, from);
                                        avail_turns[from] = turn + 2;
                                        recorder.record_pick(crane_i, coord);

                                        if let TaskType::FromTemporary(_, _, _) = task {
                                            yard[from] = None;
                                        } else {
                                            let row = from.row();
                                            let in_ptr = &mut in_ptr[row];
                                            *in_ptr += 1;
                                            yard[from] =
                                                env.input.containers()[row].get(*in_ptr).copied();
                                        }
                                    } else {
                                        // コンテナに向けて移動
                                        let next = env.dist_dict.next(flag, coord, from, false);
                                        *crane = CraneState::Empty(next);
                                    }
                                }
                                CraneState::Holding(container_t, coord) => {
                                    let (container, to) = match task {
                                        TaskType::Direct(container, _, to) => (container, to),
                                        TaskType::ToTemporary(container, _, to) => (container, to),
                                        TaskType::FromTemporary(container, _, to) => {
                                            (container, to)
                                        }
                                    };
                                    assert_eq!(container, container_t);

                                    let can_drop = if let TaskType::ToTemporary(_, _, _) = task {
                                        yard[to] == None && avail_turns[to] <= turn
                                    } else {
                                        out_next[to.row()] == container.index()
                                            && avail_turns[to] <= turn
                                    };

                                    if coord == to && can_drop {
                                        // コンテナをDropする
                                        *crane = CraneState::Empty(coord);
                                        avail_turns[to] = turn + 2;
                                        task_ptr[crane_i] += 1;
                                        last_turns[crane_i] = turn;
                                        recorder.record_drop(crane_i, coord);

                                        if let TaskType::ToTemporary(_, _, _) = task {
                                            yard[to] = Some(container);
                                        } else {
                                            out_next[to.row()] += 1;
                                        }
                                    } else {
                                        // コンテナに向けて移動
                                        let consider_container = !Input::is_large_crane(crane_i);
                                        let next =
                                            env.dist_dict.next(flag, coord, to, consider_container);
                                        *crane = CraneState::Holding(container, next);
                                    }
                                }
                                CraneState::Destroyed => continue,
                            }
                        }

                        if task_ptr
                            .iter()
                            .zip(&self.tasks)
                            .all(|(&ptr, tasks)| ptr == tasks.len())
                        {
                            recorder.record_turn(last_turns);
                            return Ok(recorder);
                        }

                        if prev_state == cranes {
                            no_progress += 1;

                            // available_turn制約により1ターンは状況が変化しない可能性がある
                            // 2ターン以上状況が変化しない場合は終了
                            if no_progress >= 2 {
                                return Err("no progress");
                            }
                        } else {
                            no_progress = 0;
                        }
                    }

                    unreachable!();
                }
            }

            #[derive(Debug, Clone, Copy, PartialEq, Eq)]
            pub enum SubTask {
                Pick(Coord, usize),
                Drop(Coord, usize),
                EndOfOrder,
            }

            impl SubTask {
                pub fn coord(&self) -> Option<Coord> {
                    match self {
                        SubTask::Pick(coord, _) => Some(*coord),
                        SubTask::Drop(coord, _) => Some(*coord),
                        SubTask::EndOfOrder => None,
                    }
                }

                pub fn index(&self) -> Option<usize> {
                    match self {
                        SubTask::Pick(_, index) => Some(*index),
                        SubTask::Drop(_, index) => Some(*index),
                        SubTask::EndOfOrder => None,
                    }
                }
            }

            trait Recorder {
                fn new() -> Self;
                fn record_pick(&mut self, crane: usize, coord: Coord);
                fn record_drop(&mut self, crane: usize, coord: Coord);
                fn record_turn(&mut self, turns: [usize; Input::N]);
            }

            #[derive(Debug, Clone, Copy)]
            struct Turns {
                turns: [usize; Input::N],
            }

            impl Turns {
                fn new(turns: [usize; Input::N]) -> Self {
                    Self { turns }
                }

                fn calc_score(&self, env: &Env) -> f64 {
                    // logsumexp
                    let kappa = env.input.params().kappa_step02();
                    let logsumexp = self
                        .turns
                        .iter()
                        .map(|&t| (t as f64 / kappa).exp())
                        .sum::<f64>()
                        .ln()
                        * kappa;
                    logsumexp
                }

                fn calc_score_best_state(&self, env: &Env) -> f64 {
                    // logsumexp
                    let kappa = env.input.params().kappa_step02_best();
                    let logsumexp = self
                        .turns
                        .iter()
                        .map(|&t| (t as f64 / kappa).exp())
                        .sum::<f64>()
                        .ln()
                        * kappa;
                    logsumexp
                }
            }

            impl Recorder for Turns {
                fn new() -> Self {
                    Self::new([0; Input::N])
                }

                fn record_pick(&mut self, _crane: usize, _coord: Coord) {
                    // do nothing
                }

                fn record_drop(&mut self, _crane: usize, _coord: Coord) {
                    // do nothing
                }

                fn record_turn(&mut self, turns: [usize; Input::N]) {
                    self.turns = turns;
                }
            }
        }
        mod step03_execute {
            mod step03_01_dag {
                use crate::{
                    grid::Coord,
                    problem::Input,
                    solver::beam::{step02_order::SubTask, Precalc},
                };
                use itertools::Itertools;
                use std::{collections::HashMap, iter::repeat};

                const DEPOTS: [Coord; 16] = [
                    Coord::new(0, 0),
                    Coord::new(1, 0),
                    Coord::new(2, 0),
                    Coord::new(3, 0),
                    Coord::new(4, 0),
                    Coord::new(0, 2),
                    Coord::new(2, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(2, 3),
                    Coord::new(4, 3),
                    Coord::new(0, 4),
                    Coord::new(1, 4),
                    Coord::new(2, 4),
                    Coord::new(3, 4),
                    Coord::new(4, 4),
                ];

                pub(super) fn critical_path_analysis(
                    tasks: &[Vec<SubTask>; Input::N],
                    precalc: &Precalc,
                ) -> TaskSet {
                    let all_tasks = tasks.iter().flatten().copied().collect_vec();
                    let cranes = (0..Input::N)
                        .flat_map(|i| repeat(i).take(tasks[i].len()).collect_vec())
                        .collect_vec();
                    let mut init_ptr = [0; Input::N];
                    let mut end_ptr = [0; Input::N];

                    for (i, t) in tasks.iter().take(Input::N - 1).enumerate() {
                        init_ptr[i + 1] = init_ptr[i] + t.len() as u8;
                        end_ptr[i] = init_ptr[i + 1] as u8;
                    }

                    end_ptr[Input::N - 1] = all_tasks.len() as u8;

                    let mut grid = HashMap::new();

                    for &d in DEPOTS.iter() {
                        grid.insert(d, vec![]);
                    }

                    for (i, t) in all_tasks.iter().enumerate() {
                        let (Some(coord), Some(index)) = (t.coord(), t.index()) else {
                            continue;
                        };

                        let v = grid.get_mut(&coord).unwrap();

                        if v.len() <= index {
                            v.resize(index + 1, !0);
                        }
                        grid.get_mut(&coord).unwrap()[index] = i;
                    }

                    // DAGを構築
                    let mut graph = vec![vec![]; all_tasks.len()];

                    // 同クレーン内の依存関係
                    for i in 0..all_tasks.len() - 1 {
                        if let (Some(c0), Some(c1)) =
                            (all_tasks[i].coord(), all_tasks[i + 1].coord())
                        {
                            graph[i + 1].push((i, c0.dist(&c1) + 1));
                        } else if all_tasks[i + 1] == SubTask::EndOfOrder {
                            graph[i + 1].push((i, 0));
                        }
                    }

                    // 位置別の依存関係
                    for (i, t) in all_tasks.iter().enumerate() {
                        let (Some(coord), Some(index)) = (t.coord(), t.index()) else {
                            continue;
                        };

                        let v = grid.get(&coord).unwrap();
                        let Some(prev) = v.get(index.wrapping_sub(1)).copied() else {
                            continue;
                        };

                        if cranes[prev] != cranes[i] {
                            graph[i].push((prev, 2));
                        }
                    }

                    let mut dp = vec![0.0; all_tasks.len()];
                    let mut indegrees = vec![0; all_tasks.len()];
                    let mut stack = vec![];

                    for edges in graph.iter() {
                        for &(v, _) in edges.iter() {
                            indegrees[v] += 1;
                        }
                    }

                    for v in 0..all_tasks.len() {
                        if indegrees[v] == 0 {
                            stack.push(v);
                            dp[v] = precalc.exp_table[0];
                        }
                    }

                    while let Some(v) = stack.pop() {
                        for &(u, cost) in graph[v].iter() {
                            dp[u] += dp[v] * precalc.exp_table[cost];
                            indegrees[u] -= 1;

                            if indegrees[u] == 0 {
                                stack.push(u);
                            }
                        }
                    }

                    assert!(indegrees.iter().all(|&x| x == 0));

                    TaskSet::new(all_tasks, dp, init_ptr, end_ptr)
                }

                #[derive(Debug, Clone)]
                pub struct TaskSet {
                    pub tasks: Vec<SubTask>,
                    pub dp: Vec<f64>,
                    pub init_ptr: [u8; Input::N],
                    pub end_ptr: [u8; Input::N],
                }

                impl TaskSet {
                    pub fn new(
                        tasks: Vec<SubTask>,
                        dp: Vec<f64>,
                        init_ptr: [u8; Input::N],
                        end_ptr: [u8; Input::N],
                    ) -> Self {
                        Self {
                            tasks,
                            dp,
                            init_ptr,
                            end_ptr,
                        }
                    }
                }
            }
            mod step03_02_beam {
                use super::{
                    super::step02_order::SubTask, step03_01_dag::TaskSet, Precalc, StorageFlag,
                };
                use crate::{
                    data_structures::{History, HistoryIndex},
                    grid::Coord,
                    problem::{Container, CraneState, Grid, Input, Operation},
                };
                use rand::prelude::*;
                use rand_pcg::Pcg64Mcg;
                use std::array;

                const HASH_LEN: usize = 11;
                const BEAM_SIZE: usize = 1 << HASH_LEN;

                pub(super) fn beam(
                    input: &Input,
                    precalc: &Precalc,
                    tasks: TaskSet,
                    max_turn: usize,
                ) -> Result<Vec<[Operation; Input::N]>, &'static str> {
                    let env = Env::new(input, precalc, tasks);
                    let mut history = History::new();
                    let mut beam = vec![None; BEAM_SIZE];
                    beam[0] = Some(State::init(&env));
                    let mut turn = 0;

                    while !beam
                        .iter()
                        .any(|s| s.map_or(false, |s| s.is_completed(&env)))
                    {
                        turn += 1;

                        if turn > max_turn {
                            return Err("turn limit exceeded");
                        }

                        let mut next_beam = vec![None; BEAM_SIZE];
                        for state in beam {
                            let Some(state) = state else {
                                continue;
                            };

                            state.gen_next(&env, &mut next_beam, &mut history);
                        }

                        beam = next_beam;
                    }

                    let best = beam
                        .iter()
                        .filter(|s| s.is_some_and(|s| s.is_completed(&env)))
                        .next()
                        .unwrap()
                        .unwrap();

                    Ok(history.collect(best.history))
                }

                struct Env<'a> {
                    _input: &'a Input,
                    precalc: &'a Precalc,
                    tasks: TaskSet,
                    hashes: [Grid<usize>; Input::N],
                }

                impl<'a> Env<'a> {
                    fn new(input: &'a Input, precalc: &'a Precalc, tasks: TaskSet) -> Self {
                        const HASH_MASK: usize = (1 << HASH_LEN) - 1;
                        let mut hashes = [Grid::new([0; Input::N * Input::N]); Input::N];
                        let mut rng = Pcg64Mcg::new(42);

                        for i in 0..Input::N {
                            for row in 0..Input::N {
                                for col in 0..Input::N {
                                    let c = Coord::new(row, col);
                                    hashes[i][c] = rng.gen::<usize>() & HASH_MASK;
                                }
                            }
                        }

                        Self {
                            _input: input,
                            precalc,
                            tasks,
                            hashes,
                        }
                    }
                }

                #[derive(Debug, Clone, Copy)]
                struct State {
                    task_ptr: [u8; Input::N],
                    grid_ptr: Grid<u8>,
                    cranes: [CraneState; Input::N],
                    board: Grid<i8>,
                    history: HistoryIndex,
                    score: f64,
                }

                impl State {
                    fn init(env: &Env) -> Self {
                        let task_ptr = env.tasks.init_ptr;
                        let cranes = array::from_fn(|i| CraneState::Empty(Coord::new(i, 0)));
                        let history = HistoryIndex::ROOT;
                        let mut board = Grid::with_default();
                        board[Coord::new(0, 0)] = 5;
                        board[Coord::new(1, 0)] = 5;
                        board[Coord::new(2, 0)] = 5;
                        board[Coord::new(3, 0)] = 5;
                        board[Coord::new(4, 0)] = 5;
                        board[Coord::new(0, 4)] = -5;
                        board[Coord::new(1, 4)] = -5;
                        board[Coord::new(2, 4)] = -5;
                        board[Coord::new(3, 4)] = -5;
                        board[Coord::new(4, 4)] = -5;

                        let storage_flag = env.precalc.dist_dict.get_flag(|c| board[c] > 0);
                        let grid_ptr = Grid::with_default();

                        let mut scores = [0.0; Input::N];

                        for (i, &task_ptr) in task_ptr.iter().enumerate() {
                            let crane = cranes[i];
                            let task = &env.tasks.tasks[task_ptr as usize];
                            let edge_cost = if let (Some(c0), Some(c1)) =
                                (cranes[i].coord(), task.coord())
                            {
                                let consider_container =
                                    !Input::is_large_crane(i) && crane.is_holding();
                                env.precalc
                                    .dist_dict
                                    .dist(storage_flag, c0, c1, consider_container)
                                    + 1
                            } else {
                                0
                            };

                            scores[i] =
                                env.tasks.dp[task_ptr as usize] * env.precalc.exp_table[edge_cost];
                        }

                        let score = scores.iter().sum::<f64>();

                        Self::new(task_ptr, grid_ptr, cranes, board, score, history)
                    }

                    fn new(
                        task_ptr: [u8; Input::N],
                        grid_ptr: Grid<u8>,
                        cranes: [CraneState; Input::N],
                        board: Grid<i8>,
                        score: f64,
                        history: HistoryIndex,
                    ) -> Self {
                        Self {
                            task_ptr,
                            grid_ptr,
                            cranes,
                            board,
                            history,
                            score,
                        }
                    }

                    fn gen_next(
                        &self,
                        env: &Env,
                        beam: &mut Vec<Option<State>>,
                        history: &mut History<[Operation; Input::N]>,
                    ) {
                        // 操作の候補を列挙
                        let mut candidates = [vec![], vec![], vec![], vec![], vec![]];
                        const MOVES: [Operation; 5] = [
                            Operation::None,
                            Operation::Up,
                            Operation::Right,
                            Operation::Down,
                            Operation::Left,
                        ];

                        let storage_flag = env.precalc.dist_dict.get_flag(|c| self.board[c] > 0);

                        for crane_i in 0..Input::N {
                            let crane = self.cranes[crane_i];
                            let candidates = &mut candidates[crane_i];
                            let task = env.tasks.tasks[self.task_ptr[crane_i] as usize];

                            if crane == CraneState::Destroyed {
                                candidates.push(Operation::None);
                            } else if task == SubTask::EndOfOrder {
                                candidates.push(Operation::Destroy);
                            } else {
                                let coord = crane.coord().unwrap();
                                let destination = task.coord().unwrap();

                                if coord == destination
                                    && self.grid_ptr[coord] == task.index().unwrap() as u8
                                {
                                    // pick / drop
                                    let op = match task {
                                        SubTask::Pick(_, _) => Operation::Pick,
                                        SubTask::Drop(_, _) => Operation::Drop,
                                        SubTask::EndOfOrder => unreachable!(),
                                    };
                                    candidates.push(op);
                                } else {
                                    let nearest_crane_dist = self
                                        .cranes
                                        .iter()
                                        .filter_map(|c| c.coord())
                                        .map(|c| c.dist(&coord))
                                        .filter(|&d| d > 0)
                                        .min()
                                        .unwrap_or(usize::MAX);
                                    let consider_container =
                                        !Input::is_large_crane(crane_i) && crane.is_holding();
                                    let current_dist = env.precalc.dist_dict.dist(
                                        storage_flag,
                                        coord,
                                        destination,
                                        consider_container,
                                    );

                                    if crane.is_holding() && !Input::is_large_crane(crane_i) {
                                        // コンテナを考慮して移動
                                        for &op in MOVES.iter() {
                                            let next = coord + op.dir();
                                            if next.in_map(Input::N) && self.board[next] <= 0 {
                                                let next_dist = env.precalc.dist_dict.dist(
                                                    storage_flag,
                                                    next,
                                                    destination,
                                                    consider_container,
                                                );

                                                // 他のクレーンが遠いときは損しない移動方法だけを考慮
                                                if nearest_crane_dist <= 1
                                                    || next_dist <= current_dist
                                                {
                                                    candidates.push(op);
                                                }
                                            }
                                        }
                                    } else {
                                        // コンテナを無視して移動
                                        for &op in MOVES.iter() {
                                            let next = coord + op.dir();
                                            if next.in_map(Input::N) {
                                                let next_dist = env.precalc.dist_dict.dist(
                                                    storage_flag,
                                                    next,
                                                    destination,
                                                    consider_container,
                                                );

                                                // 他のクレーンが遠いときは損しない移動方法だけを考慮
                                                if nearest_crane_dist <= 1
                                                    || next_dist <= current_dist
                                                {
                                                    candidates.push(op);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        let mut cant_in = Grid::new([false; Input::N * Input::N]);
                        let mut cant_move = Grid::new([[false; 8]; Input::N * Input::N]);
                        let mut state = self.clone();

                        state.dfs(
                            env,
                            &candidates,
                            &mut cant_in,
                            &mut cant_move,
                            history,
                            &mut [Operation::None; Input::N],
                            beam,
                            storage_flag,
                            0.0,
                            0,
                            0,
                        );
                    }

                    fn dfs(
                        &mut self,
                        env: &Env,
                        candidates: &[Vec<Operation>],
                        cant_in: &mut Grid<bool>,
                        cant_move: &mut Grid<[bool; 8]>,
                        history: &mut History<[Operation; Input::N]>,
                        operations: &mut [Operation; Input::N],
                        beam: &mut Vec<Option<State>>,
                        storage_flag: StorageFlag,
                        score: f64,
                        hash: usize,
                        depth: usize,
                    ) {
                        if depth == Input::N {
                            let old_score = beam[hash].map_or(f64::MAX, |s| s.score);

                            if score < old_score {
                                let mut new_state = Self::new(
                                    self.task_ptr,
                                    self.grid_ptr,
                                    self.cranes,
                                    self.board,
                                    score,
                                    self.history,
                                );

                                let hist_index = history.push(operations.clone(), self.history);
                                new_state.history = hist_index;
                                beam[hash] = Some(new_state);
                            }

                            return;
                        }

                        let crane_i = depth;
                        let crane = self.cranes[crane_i];
                        let task = env.tasks.tasks[self.task_ptr[crane_i] as usize];

                        for &op in candidates[depth].iter() {
                            if crane == CraneState::Destroyed {
                                operations[crane_i] = Operation::None;

                                self.dfs(
                                    env,
                                    candidates,
                                    cant_in,
                                    cant_move,
                                    history,
                                    operations,
                                    beam,
                                    storage_flag,
                                    score,
                                    hash,
                                    depth + 1,
                                );

                                continue;
                            }

                            let coord = crane.coord().unwrap();
                            let next = coord + op.dir();
                            let op_usize = op as usize;

                            if (cant_in[next] && op != Operation::Destroy)
                                || cant_move[coord][op_usize]
                            {
                                continue;
                            }

                            let board_change = match op {
                                Operation::Pick => -1,
                                Operation::Drop => 1,
                                _ => 0,
                            };
                            let current_crane = self.cranes[crane_i];
                            let task_ptr_diff = match op {
                                Operation::Pick => 1,
                                Operation::Drop => 1,
                                _ => 0,
                            };
                            self.board[coord] += board_change;
                            self.grid_ptr[coord] =
                                self.grid_ptr[coord].wrapping_add_signed(task_ptr_diff);
                            self.task_ptr[crane_i] =
                                self.task_ptr[crane_i].wrapping_add_signed(task_ptr_diff);
                            self.cranes[crane_i] = if task == SubTask::EndOfOrder {
                                CraneState::Destroyed
                            } else {
                                match op {
                                    Operation::Pick => {
                                        CraneState::Holding(Container::new(!0), coord)
                                    }
                                    Operation::Drop => CraneState::Empty(coord),
                                    _ => match current_crane {
                                        CraneState::Empty(_) => CraneState::Empty(next),
                                        CraneState::Holding(container, _) => {
                                            CraneState::Holding(container, next)
                                        }
                                        CraneState::Destroyed => CraneState::Destroyed,
                                    },
                                }
                            };

                            operations[crane_i] = op;

                            if op != Operation::Destroy {
                                cant_in[next] = true;
                            }

                            // クロスする移動もNG
                            if op_usize < 4 {
                                cant_move[next][op_usize ^ 2] = true;
                            }

                            // ハッシュ値を更新
                            let new_hash = hash ^ env.hashes[crane_i][next];

                            // スコア
                            let new_crane = self.cranes[crane_i].coord();
                            let new_task_coord =
                                env.tasks.tasks[self.task_ptr[crane_i] as usize].coord();
                            let edge_cost = if let (Some(c0), Some(c1)) =
                                (new_crane, new_task_coord)
                            {
                                let consider_container = !Input::is_large_crane(crane_i)
                                    && self.cranes[crane_i].is_holding();
                                env.precalc
                                    .dist_dict
                                    .dist(storage_flag, c0, c1, consider_container)
                                    + 1
                            } else {
                                0
                            };

                            let task_ptr = self.task_ptr[crane_i];
                            let score_diff =
                                env.tasks.dp[task_ptr as usize] * env.precalc.exp_table[edge_cost];

                            self.dfs(
                                env,
                                candidates,
                                cant_in,
                                cant_move,
                                history,
                                operations,
                                beam,
                                storage_flag,
                                score + score_diff,
                                new_hash,
                                depth + 1,
                            );

                            self.board[coord] -= board_change;
                            self.grid_ptr[coord] =
                                self.grid_ptr[coord].wrapping_add_signed(-task_ptr_diff);
                            self.task_ptr[crane_i] =
                                self.task_ptr[crane_i].wrapping_add_signed(-task_ptr_diff);
                            self.cranes[crane_i] = current_crane;

                            if op != Operation::Destroy {
                                cant_in[next] = false;
                            }

                            if op_usize < 4 {
                                cant_move[next][op_usize ^ 2] = false;
                            }
                        }
                    }

                    fn is_completed(&self, env: &Env) -> bool {
                        self.task_ptr
                            .iter()
                            .all(|&ptr| env.tasks.tasks[ptr as usize] == SubTask::EndOfOrder)
                    }
                }

                impl PartialEq for State {
                    fn eq(&self, other: &Self) -> bool {
                        self.score == other.score
                    }
                }

                impl Eq for State {}

                impl PartialOrd for State {
                    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                        self.score.partial_cmp(&other.score)
                    }
                }

                impl Ord for State {
                    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                        self.partial_cmp(&other).unwrap()
                    }
                }
            }

            use super::{step02_order::SubTask, Precalc, StorageFlag};
            use crate::problem::{Input, Operation};

            pub(super) fn execute(
                input: &Input,
                precalc: &Precalc,
                tasks: &[Vec<SubTask>; Input::N],
                max_turn: usize,
            ) -> Result<Vec<[Operation; Input::N]>, &'static str> {
                let tasks = step03_01_dag::critical_path_analysis(tasks, precalc);
                step03_02_beam::beam(input, precalc, tasks, max_turn)
            }
        }

        pub struct BeamSolver {
            seed: u64,
            max_turn: usize,
        }

        impl BeamSolver {
            pub fn new(seed: u64, max_turn: usize) -> Self {
                Self { seed, max_turn }
            }
        }

        impl Solver for BeamSolver {
            fn solve(
                &self,
                input: &crate::problem::Input,
            ) -> Result<super::SolverResult, &'static str> {
                let mut rng = Pcg64Mcg::seed_from_u64(self.seed);

                // バグを直す（precalc_no_inf）とスコアが下がるので両方用意……
                let precalc_inf = Precalc::new(input, false);
                let precalc_no_inf = Precalc::new(input, true);

                let mut all_tasks = vec![];
                let task = step01a_gen_dp::generate_tasks(input, &mut rng)?;
                all_tasks.push(task);

                let since = std::time::Instant::now();
                if let Ok(tasks) = step01b_gen_beam::generate_tasks(input, &precalc_no_inf) {
                    all_tasks.push(tasks);
                };
                eprintln!("step01b elapsed: {:?}", since.elapsed());

                let subtasks = step02_order::order_tasks(input, &precalc_inf, all_tasks)?;

                let since = std::time::Instant::now();
                let operations =
                    step03_execute::execute(input, &precalc_inf, &subtasks, self.max_turn)?;
                eprintln!("elapsed: {:?}", since.elapsed());
                let mut yard = Yard::new(&input);
                let mut output = Output::new();

                for op in operations.iter() {
                    yard.apply(op)?;
                    yard.carry_in_and_ship();
                    output.push(op);
                }

                Ok(SolverResult::new(output, &yard))
            }
        }

        #[derive(Debug, Clone)]
        struct Precalc {
            dist_dict: DistDict,
            exp_table: [f64; 1000],
        }

        impl Precalc {
            fn new(input: &Input, avoid_inf: bool) -> Self {
                let exp_table =
                    array::from_fn(|i| (i as f64 / input.params().kappa_step03()).exp());

                Self {
                    dist_dict: DistDict::new(avoid_inf),
                    exp_table,
                }
            }
        }

        #[derive(Debug, Clone)]
        struct DistDict {
            dists: [Vec<Grid<Grid<usize>>>; 2],
            next: [Vec<Grid<Grid<Coord>>>; 2],
        }

        impl DistDict {
            const STORAGES: [Coord; 11] = [
                Coord::new(0, 0),
                Coord::new(1, 0),
                Coord::new(2, 0),
                Coord::new(3, 0),
                Coord::new(4, 0),
                Coord::new(0, 2),
                Coord::new(2, 2),
                Coord::new(4, 2),
                Coord::new(0, 3),
                Coord::new(2, 3),
                Coord::new(4, 3),
            ];

            fn new(avoid_inf: bool) -> Self {
                let mut dists_with_container = vec![];
                let mut next_with_container = vec![];

                for flag in 0..1 << Self::STORAGES.len() {
                    let mut board = Grid::new([false; Input::N * Input::N]);

                    for (i, &storage) in Self::STORAGES.iter().enumerate() {
                        if (flag & (1 << i)) > 0 {
                            board[storage] = true;
                        }
                    }

                    let mut d = Grid::new(
                        [Grid::new([usize::MAX; Input::N * Input::N]); Input::N * Input::N],
                    );
                    let mut next = Grid::new(
                        [Grid::new([Coord::new(0, 0); Input::N * Input::N]); Input::N * Input::N],
                    );

                    for row in 0..Input::N {
                        for col in 0..Input::N {
                            let c = Coord::new(row, col);
                            Self::bfs(&board, &mut d[c], &mut next[c], c, avoid_inf);
                        }
                    }

                    dists_with_container.push(d);
                    next_with_container.push(next);
                }

                // コンテナなし = flagが0
                let dists_without_container =
                    vec![dists_with_container[0].clone(); dists_with_container.len()];
                let next_without_container =
                    vec![next_with_container[0].clone(); next_with_container.len()];

                Self {
                    dists: [dists_without_container, dists_with_container],
                    next: [next_without_container, next_with_container],
                }
            }

            fn bfs(
                board: &Grid<bool>,
                dists: &mut Grid<usize>,
                next: &mut Grid<Coord>,
                from: Coord,
                avoid_inf: bool,
            ) {
                dists[from] = 0;
                let mut queue = VecDeque::new();
                queue.push_back(from);

                while let Some(coord) = queue.pop_front() {
                    let next_dist = dists[coord] + 1;

                    for &adj in ADJACENTS.iter() {
                        let next = coord + adj;

                        if next.in_map(Input::N)
                            && !board[next]
                            && dists[next].change_min(next_dist)
                        {
                            queue.push_back(next);
                        }
                    }
                }

                // INFになるのを防ぐ

                if avoid_inf {
                    let mut temp = dists.clone();

                    for row in 0..Input::N {
                        for col in 0..Input::N {
                            let c = Coord::new(row, col);

                            for &adj in ADJACENTS.iter() {
                                let next = c + adj;

                                if next.in_map(Input::N) {
                                    temp[c].change_min(dists[next].saturating_add(1));
                                }
                            }
                        }
                    }

                    *dists = temp;
                }

                for row in 0..Input::N {
                    for col in 0..Input::N {
                        let c = Coord::new(row, col);
                        let mut best = c;
                        let mut best_dist = dists[best];

                        for &adj in ADJACENTS.iter() {
                            let next = c + adj;

                            if next.in_map(Input::N) && best_dist.change_min(dists[next]) {
                                best = next;
                            }
                        }

                        next[c] = best;
                    }
                }
            }

            fn get_flag(&self, f: impl Fn(Coord) -> bool) -> StorageFlag {
                let mut flag = 0;

                for (i, &storage) in Self::STORAGES.iter().enumerate() {
                    if f(storage) {
                        flag |= 1 << i;
                    }
                }

                StorageFlag(flag)
            }

            fn dist(
                &self,
                flag: StorageFlag,
                from: Coord,
                to: Coord,
                consider_container: bool,
            ) -> usize {
                // [to][from]の順になることに注意
                self.dists[consider_container as usize][flag.0][to][from]
            }

            fn next(
                &self,
                flag: StorageFlag,
                from: Coord,
                to: Coord,
                consider_container: bool,
            ) -> Coord {
                self.next[consider_container as usize][flag.0][to][from]
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        struct StorageFlag(usize);
    }
    pub mod beam_storage8 {
        use std::{array, collections::VecDeque};

        use rand::SeedableRng;
        use rand_pcg::Pcg64Mcg;

        use crate::{
            common::ChangeMinMax as _,
            grid::{Coord, ADJACENTS},
            problem::{Grid, Input, Output, Yard},
        };

        use super::{Solver, SolverResult};

        mod step01a_gen_dp {
            mod step01_01_dp {
                use itertools::iproduct;

                use crate::{common::ChangeMinMax as _, problem::Input};

                use super::{N, N2, NP1};

                /// 滞留コンテナ数の最大値を最小にするDPを行い、搬入順（何番目にどの行から搬入するか）を返す
                pub(super) fn dp(input: &Input) -> (u128, Vec<usize>) {
                    let mut counts = vec![vec![vec![vec![vec![0; NP1]; NP1]; NP1]; NP1]; NP1];

                    for i0 in 0..NP1 {
                        for i1 in 0..NP1 {
                            for i2 in 0..NP1 {
                                for i3 in 0..NP1 {
                                    for i4 in 0..NP1 {
                                        let indices = [i0, i1, i2, i3, i4];
                                        counts[i0][i1][i2][i3][i4] = count(input, indices);
                                    }
                                }
                            }
                        }
                    }

                    let mut dp =
                        vec![vec![vec![vec![vec![u128::MAX / 2; NP1]; NP1]; NP1]; NP1]; NP1];
                    dp[0][0][0][0][0] = 0;

                    let mut from =
                        vec![vec![vec![vec![vec![([0; N], 0); NP1]; NP1]; NP1]; NP1]; NP1];

                    for (i0, i1, i2, i3, i4) in iproduct!(0..NP1, 0..NP1, 0..NP1, 0..NP1, 0..NP1) {
                        let old_indices = [i0, i1, i2, i3, i4];
                        let current_dp = dp[i0][i1][i2][i3][i4];
                        let current_cnt = counts[i0][i1][i2][i3][i4];

                        for idx in 0..N {
                            let mut indices = old_indices;
                            indices[idx] += 1;

                            if indices[idx] > N {
                                continue;
                            }

                            let cnt =
                                counts[indices[0]][indices[1]][indices[2]][indices[3]][indices[4]];
                            let new_cnt = current_cnt.max(cnt);
                            let cost = current_dp + (1 << (new_cnt * 4));

                            if dp[indices[0]][indices[1]][indices[2]][indices[3]][indices[4]]
                                .change_min(cost)
                            {
                                from[indices[0]][indices[1]][indices[2]][indices[3]][indices[4]] =
                                    (old_indices, idx);
                            }
                        }
                    }

                    let mut current = [N; N];
                    let mut history = vec![];

                    while current != [0; N] {
                        let (prev, index) =
                            from[current[0]][current[1]][current[2]][current[3]][current[4]];
                        history.push(index);
                        current = prev;
                    }

                    history.reverse();

                    (dp[N][N][N][N][N], history)
                }

                fn count(input: &Input, indices: [usize; N]) -> u32 {
                    let mut contains = [false; N2];

                    for (i, &c) in indices.iter().enumerate() {
                        for &c in input.containers()[i][..c].iter() {
                            contains[c.index()] = true;
                        }
                    }

                    let mut count = indices.iter().map(|&i| i as u32).sum::<u32>();

                    for i in 0..N {
                        let slice = &contains[i * N..(i + 1) * N];
                        count -= slice.iter().take_while(|&&b| b).count() as u32;
                    }

                    count
                }
            }

            use crate::{
                common::ChangeMinMax as _,
                grid::Coord,
                problem::{Container, Grid, Input},
                solver::beam_storage8::step01a_gen_dp::step01_01_dp::dp,
            };
            use itertools::Itertools;
            use rand::prelude::*;
            use rand::Rng;

            const N: usize = Input::N;
            const NP1: usize = N + 1;
            const N2: usize = N * N;

            #[derive(Debug, Clone)]
            pub struct Task {
                crane: Option<u8>,
                container: Container,
                from: Coord,
                to: Coord,
            }

            impl Task {
                pub fn new(
                    crane: Option<u8>,
                    container: Container,
                    from: Coord,
                    to: Coord,
                ) -> Self {
                    Self {
                        crane,
                        container,
                        from,
                        to,
                    }
                }

                pub fn crane(&self) -> Option<usize> {
                    self.crane.map(|c| c as usize)
                }

                pub fn container(&self) -> Container {
                    self.container
                }

                pub fn from(&self) -> Coord {
                    self.from
                }

                pub fn to(&self) -> Coord {
                    self.to
                }
            }

            pub(super) fn generate_tasks(
                input: &Input,
                rng: &mut impl Rng,
            ) -> Result<Vec<Task>, &'static str> {
                let (_, history) = dp(input);

                let mut tasks = vec![];
                let mut containers = input
                    .containers()
                    .map(|c| c.iter().copied().rev().collect_vec());

                let mut storages = [
                    Coord::new(0, 3),
                    Coord::new(1, 3),
                    Coord::new(3, 3),
                    Coord::new(4, 3),
                    Coord::new(0, 2),
                    Coord::new(1, 2),
                    Coord::new(3, 2),
                    Coord::new(4, 2),
                ];

                let mut board = Grid::new([false; Input::CONTAINER_COUNT]);
                board[Coord::new(0, 0)] = true;
                board[Coord::new(1, 0)] = true;
                board[Coord::new(2, 0)] = true;
                board[Coord::new(3, 0)] = true;
                board[Coord::new(4, 0)] = true;

                let mut positions = [None; Input::CONTAINER_COUNT];
                let mut next_shippings = [0, 5, 10, 15, 20];

                for &row in history.iter() {
                    let container = containers[row].pop().unwrap();
                    let from = Coord::new(row, 0);
                    let to = Input::get_goal(container);

                    // 搬入口クリア
                    if containers[row].is_empty() {
                        board[from] = false;
                    }

                    if container.index() == next_shippings[to.row()] {
                        next_shippings[to.row()] += 1;
                        let task = Task::new(None, container, from, to);
                        tasks.push(task);
                    } else {
                        // ベストな場所を探す
                        let mut best_pos = None;
                        let mut best_cost = usize::MAX;
                        storages.shuffle(rng);

                        for &cand in storages.iter() {
                            if board[cand] {
                                continue;
                            }

                            let cost = from.dist(&cand) + cand.dist(&to);

                            if best_cost.change_min(cost) {
                                best_pos = Some(cand);
                            }
                        }

                        match best_pos {
                            Some(best_pos) => {
                                positions[container.index()] = Some(best_pos);
                                let task = Task::new(None, container, from, best_pos);
                                tasks.push(task);
                                board[best_pos] = true;
                            }
                            None => {
                                return Err("storage positions are occupied");
                            }
                        }
                    }

                    loop {
                        let mut next = None;
                        const ORDER: [usize; 25] = [
                            0, 5, 10, 15, 20, 1, 6, 11, 16, 21, 2, 7, 12, 17, 22, 3, 8, 13, 18, 23,
                            4, 9, 14, 19, 24,
                        ];

                        for &container in ORDER.iter() {
                            let Some(pos) = positions[container] else {
                                continue;
                            };

                            if next_shippings[Input::get_goal(Container::new(container)).row()]
                                != container
                            {
                                continue;
                            }

                            next = Some((container, pos));
                            break;
                        }

                        let Some((container, pos)) = next else {
                            break;
                        };

                        board[pos] = false;
                        positions[container] = None;
                        next_shippings[Input::get_goal(Container::new(container)).row()] += 1;
                        let task = Task::new(
                            None,
                            Container::new(container),
                            pos,
                            Input::get_goal(Container::new(container)),
                        );
                        tasks.push(task);
                    }
                }

                Ok(tasks)
            }
        }
        mod step01b_gen_beam {
            use super::{step01a_gen_dp::Task, DistDict, Precalc};
            use crate::{
                beam::{BayesianBeamWidthSuggester, BeamWidthSuggester as _},
                common::ChangeMinMax as _,
                data_structures::{History, HistoryIndex},
                grid::Coord,
                problem::{Container, Grid, Input},
            };
            use itertools::Itertools;
            use rand::prelude::*;
            use rand::Rng;
            use rand_pcg::Pcg64Mcg;
            use std::{array, collections::HashSet};

            const MAX_TURN: usize = 80;

            pub(super) fn generate_tasks(
                input: &Input,
                precalc: &Precalc,
            ) -> Result<Vec<Task>, &'static str> {
                let env = Env::new(&input, &precalc.dist_dict);
                let mut beam = vec![vec![vec![]; State::STORAGE_COUNT + 1]; u8::MAX as usize];
                beam[0][0].push(State::init(&env));
                let mut history = History::new();
                let mut rng = Pcg64Mcg::from_entropy();
                let mut completed_list: Vec<Option<State>> = vec![None; u8::MAX as usize];
                let mut beam_width_suggester =
                    BayesianBeamWidthSuggester::new(MAX_TURN, 5, 0.5, 3000, 300, 10000, 1);
                let mut hashset = HashSet::new();

                for turn in 0..MAX_TURN {
                    let beam_width = beam_width_suggester.suggest();

                    if let Some(completed) = completed_list[turn as usize] {
                        let tasks = history.collect(completed.history);
                        eprintln!("{}", completed.score);
                        eprintln!("{:?}", completed.crane_avail_turns);
                        eprintln!("1st beam turn: {}", turn);
                        return Ok(tasks);
                    }

                    for storage in 0..=State::STORAGE_COUNT {
                        let mut b = vec![];
                        std::mem::swap(&mut b, &mut beam[turn as usize][storage as usize]);

                        for state in b.iter() {
                            if state.is_completed() {
                                let t = *state
                                    .crane_avail_turns
                                    .iter()
                                    .filter(|&&t| t < u8::MAX)
                                    .max()
                                    .unwrap() as usize;

                                if completed_list[t].is_none() {
                                    completed_list[t] = Some(*state);
                                }
                            }
                        }

                        b.sort_unstable();
                        hashset.clear();
                        let mut count = 0;

                        for state in b.iter_mut() {
                            if !hashset.insert(state.hash) {
                                continue;
                            }

                            count += 1;

                            if count >= beam_width {
                                break;
                            }

                            state.gen_next(&env, &mut beam, &mut history, &mut rng);
                        }
                    }
                }

                Err("Failed to find a solution")
            }

            struct Env<'a> {
                input: &'a Input,
                dist_dict: &'a DistDict,
                index_grid: Grid<usize>,
                crane_hash: [Grid<u64>; Input::N],
                container_hash: [Grid<u64>; Input::CONTAINER_COUNT],
            }

            impl<'a> Env<'a> {
                fn new(input: &'a Input, dist_dict: &'a DistDict) -> Self {
                    let mut index_grid = Grid::new([!0; Input::N * Input::N]);

                    for (i, c) in State::POS.iter().enumerate() {
                        index_grid[c] = i;
                    }

                    let mut rng = Pcg64Mcg::from_entropy();
                    let mut crane_hash = [Grid::new([0; Input::N * Input::N]); Input::N];
                    let mut container_hash =
                        [Grid::new([0; Input::N * Input::N]); Input::CONTAINER_COUNT];

                    for row in 0..Input::N {
                        for col in 0..Input::N {
                            let c = Coord::new(row, col);

                            for i in 0..Input::N {
                                crane_hash[i][c] = rng.gen();
                            }

                            for i in 0..Input::CONTAINER_COUNT {
                                container_hash[i][c] = rng.gen();
                            }
                        }
                    }

                    Self {
                        input,
                        dist_dict,
                        index_grid,
                        crane_hash,
                        container_hash,
                    }
                }
            }

            #[derive(Debug, Clone, Copy)]
            struct State {
                in_ptr: [u8; Input::N],
                container_avail_turns: [u8; Input::N + Self::STORAGE_COUNT],
                containers: [Option<Container>; Input::N + Self::STORAGE_COUNT],
                cranes: [Coord; Input::N],
                out_next: [u8; Input::N],
                out_avail_turns: [u8; Input::N],
                crane_avail_turns: [u8; Input::N],
                crane_score_per_turn: [f32; Input::N],
                temp_count: u8,
                finished_container_count: u8,
                score: f32,
                hash: u64,
                history: HistoryIndex,
            }

            impl State {
                const STORAGE_COUNT: usize = 8;
                const STORAGES: [Coord; Self::STORAGE_COUNT] = [
                    Coord::new(0, 2),
                    Coord::new(1, 2),
                    Coord::new(3, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(1, 3),
                    Coord::new(3, 3),
                    Coord::new(4, 3),
                ];
                const POS: [Coord; Input::N * 2 + Self::STORAGE_COUNT] = [
                    // 搬入口
                    Coord::new(0, 0),
                    Coord::new(1, 0),
                    Coord::new(2, 0),
                    Coord::new(3, 0),
                    Coord::new(4, 0),
                    // 一時保管
                    Coord::new(0, 2),
                    Coord::new(1, 2),
                    Coord::new(3, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(1, 3),
                    Coord::new(3, 3),
                    Coord::new(4, 3),
                    // 搬出口
                    Coord::new(0, 4),
                    Coord::new(1, 4),
                    Coord::new(2, 4),
                    Coord::new(3, 4),
                    Coord::new(4, 4),
                ];
                const CRANE_AVAIL: u8 = u8::MAX;

                fn init(env: &Env) -> Self {
                    let mut containers = [None; Input::N + Self::STORAGE_COUNT];
                    let mut hash = 0;

                    for i in 0..Input::N {
                        let container = env.input.containers()[i][0];
                        containers[i] = Some(container);
                        hash ^= env.crane_hash[i][Self::POS[i]];
                        hash ^= env.container_hash[container.index()][Self::POS[i]];
                    }

                    Self {
                        in_ptr: [0; Input::N],
                        container_avail_turns: [0; Input::N + Self::STORAGE_COUNT],
                        containers,
                        temp_count: 0,
                        finished_container_count: 0,
                        cranes: array::from_fn(|i| Coord::new(i, 0)),
                        out_next: [0, 5, 10, 15, 20],
                        out_avail_turns: [0; Input::N],
                        crane_avail_turns: [0; Input::N],
                        crane_score_per_turn: [0.0; Input::N],
                        score: 0.0,
                        hash,
                        history: HistoryIndex::ROOT,
                    }
                }

                fn is_completed(&self) -> bool {
                    self.finished_container_count == Input::CONTAINER_COUNT as u8
                }

                fn gen_next(
                    &mut self,
                    env: &Env,
                    beam: &mut Vec<Vec<Vec<Self>>>,
                    history: &mut History<Task>,
                    rng: &mut impl Rng,
                ) {
                    let mut container_counts = [0; Input::N + Self::STORAGE_COUNT];
                    let mut container_suffix_sum = [0; Input::N + Self::STORAGE_COUNT];

                    for i in 0..Input::N {
                        container_counts[i] = Input::N - self.in_ptr[i] as usize;
                    }

                    for i in Input::N..Input::N + Self::STORAGE_COUNT {
                        container_counts[i] = self.containers[i].is_some() as usize;
                    }

                    container_suffix_sum[Input::N + Self::STORAGE_COUNT - 1] =
                        container_counts[Input::N + Self::STORAGE_COUNT - 1];

                    for i in (0..Input::N + Self::STORAGE_COUNT - 1).rev() {
                        container_suffix_sum[i] = container_suffix_sum[i + 1] + container_counts[i];
                    }

                    let turn = self.crane_avail_turns.iter().copied().min().unwrap();
                    let mut available_crane_count = 0;

                    for (c, t) in self
                        .crane_avail_turns
                        .iter_mut()
                        .zip(self.crane_score_per_turn.iter_mut())
                    {
                        if *c == turn {
                            *c = Self::CRANE_AVAIL;
                            *t = 0.0;
                            available_crane_count += 1;
                        }
                    }

                    // コンテナ数とクレーン数の小さい方の数だけ操作を割り付ける必要がある
                    let remaining = container_suffix_sum[0].min(available_crane_count);

                    let mut state = *self;
                    state.dfs(
                        env,
                        beam,
                        history,
                        &container_suffix_sum,
                        turn,
                        0,
                        remaining,
                        rng,
                    );
                }

                fn dfs(
                    &mut self,
                    env: &Env,
                    beam: &mut Vec<Vec<Vec<Self>>>,
                    history: &mut History<Task>,
                    container_suffix_sum: &[usize; Input::N + Self::STORAGE_COUNT],
                    turn: u8,
                    search_i: usize,
                    remaining: usize,
                    rng: &mut impl Rng,
                ) {
                    if remaining == 0 {
                        let next_turn = self.crane_avail_turns.iter().copied().min().unwrap();

                        if next_turn > MAX_TURN as u8 {
                            return;
                        }

                        let score_per_turn = self.crane_score_per_turn.iter().copied().sum::<f32>();
                        assert!(next_turn > turn);

                        for t in self.crane_avail_turns.iter_mut() {
                            if *t == Self::CRANE_AVAIL {
                                *t = next_turn;
                            }
                        }

                        self.score += score_per_turn * (next_turn - turn) as f32;
                        beam[next_turn as usize][self.temp_count as usize].push(*self);
                        return;
                    }

                    for container_i in search_i..Input::N + Self::STORAGE_COUNT {
                        if container_suffix_sum[container_i] < remaining {
                            break;
                        }

                        let Some(container) = self.containers[container_i] else {
                            continue;
                        };

                        let from = Self::POS[container_i];
                        let goal = Input::get_goal(container);

                        let to_out = self.out_next[goal.row()] == container.index() as u8;
                        let to_temp =
                            container_i < Input::N && self.temp_count < Self::STORAGE_COUNT as u8;

                        if !to_out && !to_temp {
                            continue;
                        }

                        let mut state = *self;
                        state.hash ^= env.container_hash[container.index()][from];

                        // craneのアサイン
                        let mut best_crane = !0;
                        let mut best_dist = usize::MAX;

                        for (crane, (&t, &c)) in self
                            .crane_avail_turns
                            .iter()
                            .zip(self.cranes.iter())
                            .enumerate()
                        {
                            if t == Self::CRANE_AVAIL && best_dist.change_min(c.dist(&from)) {
                                best_crane = crane;
                            }
                        }

                        let crane_pos = self.cranes[best_crane];
                        let consider_container = !Input::is_large_crane(best_crane);
                        state.hash ^= env.crane_hash[best_crane][crane_pos];

                        // in側の更新
                        let storage_flag = env
                            .dist_dict
                            .get_flag(|c| state.containers[env.index_grid[c]].is_some());

                        // pickを開始するターン
                        let move_len =
                            env.dist_dict.dist(storage_flag, crane_pos, from, false) as u8;
                        let pick_turn =
                            (turn + move_len).max(state.container_avail_turns[container_i]);
                        state.container_avail_turns[container_i] = pick_turn + 2;

                        // pick_work: Pickの有効作業量。一時保管場所からのPickは無駄な作業なので0
                        let pick_work = if container_i < Input::N {
                            let in_ptr = &mut state.in_ptr[container_i];
                            *in_ptr += 1;
                            state.containers[container_i] = env.input.containers()[container_i]
                                .get(*in_ptr as usize)
                                .copied();
                            1
                        } else {
                            state.containers[container_i] = None;
                            state.temp_count -= 1;
                            0
                        };

                        // クレーンの戻り距離に応じた仕事量
                        // 左方向の移動は作業に貢献しており、それ以外の移動は貢献していないと見なす
                        let crane_back_work = crane_pos.col().saturating_sub(from.col()) as i32;

                        // out側の更新
                        if to_out {
                            // そのまま搬出
                            state.out_next[goal.row()] += 1;
                            state.finished_container_count += 1;
                            state.hash ^= env.container_hash[container.index()][goal];

                            let dist = from.dist(&goal);
                            let prev_potential = dist as i32;

                            // クレーンの左移動 + Pick + ゴールまでの移動 + Dropが仕事量
                            let total_work = crane_back_work + pick_work + prev_potential + 1;

                            // dropが開始するターン
                            let move_len =
                                env.dist_dict
                                    .dist(storage_flag, from, goal, consider_container)
                                    as u8;
                            let drop_turn =
                                (pick_turn + 1 + move_len).max(state.out_avail_turns[goal.row()]);
                            state.out_avail_turns[goal.row()] = drop_turn + 2;

                            // クレーンが使用可能になるターン
                            let crane_avail_turn = drop_turn + 1;
                            let total_turn = crane_avail_turn - turn;
                            let score_per_turn = total_work as f32 / total_turn as f32;
                            state.crane_score_per_turn[best_crane] = score_per_turn;
                            state.crane_avail_turns[best_crane] = crane_avail_turn;
                            state.cranes[best_crane] = goal;
                            state.hash ^= env.crane_hash[best_crane][goal];

                            // historyの追加
                            let task = Task::new(Some(best_crane as u8), container, from, goal);
                            state.history = history.push(task, state.history);
                            state.dfs(
                                env,
                                beam,
                                history,
                                container_suffix_sum,
                                turn,
                                container_i,
                                remaining - 1,
                                rng,
                            );
                        } else {
                            // 一時保管場所に運ぶ
                            let mut candidates = (Input::N..Self::STORAGE_COUNT + Input::N)
                                .filter(|&i| state.containers[i].is_none())
                                .collect_vec();
                            let dists = Self::STORAGES.map(|c| {
                                env.dist_dict
                                    .dist(storage_flag, from, c, consider_container)
                                    + env.dist_dict.dist(storage_flag, c, goal, true)
                            });
                            candidates.shuffle(rng);
                            candidates.sort_by_key(|&i| dists[i - Input::N]);
                            const TAKE_COUNT: usize = 3;
                            state.temp_count += 1;

                            for &temp_i in candidates.iter().take(TAKE_COUNT) {
                                let mut state = state;
                                state.containers[temp_i] = Some(container);
                                let to = Self::POS[temp_i];
                                state.hash ^= env.container_hash[container.index()][to];
                                let prev_potential = from.dist(&goal) as i32;
                                let new_potential = to.dist(&goal) as i32;
                                let potential_diff = prev_potential - new_potential;

                                // クレーンの左移動 + Pick + ゴールまでの移動 + Dropが仕事量
                                // potential_diffが負になること、一時保管場所にDropする操作は進捗を生まないことに注意
                                let total_work = crane_back_work + pick_work + potential_diff;
                                let move_len =
                                    env.dist_dict
                                        .dist(storage_flag, from, to, consider_container)
                                        as u8;
                                let drop_turn = (pick_turn + 1 + move_len)
                                    .max(state.container_avail_turns[temp_i]);
                                state.container_avail_turns[temp_i] = drop_turn + 2;
                                state.cranes[best_crane] = to;

                                let crane_avail_turn = drop_turn + 1;
                                let total_turn = crane_avail_turn - turn;
                                let score_per_turn = total_work as f32 / total_turn as f32;
                                state.crane_score_per_turn[best_crane] = score_per_turn;
                                state.crane_avail_turns[best_crane] = crane_avail_turn;
                                state.hash ^= env.crane_hash[best_crane][to];

                                // historyの追加
                                let task = Task::new(Some(best_crane as u8), container, from, to);
                                state.history = history.push(task, state.history);
                                state.dfs(
                                    env,
                                    beam,
                                    history,
                                    container_suffix_sum,
                                    turn,
                                    container_i,
                                    remaining - 1,
                                    rng,
                                );
                            }
                        }
                    }
                }
            }

            impl PartialEq for State {
                fn eq(&self, other: &Self) -> bool {
                    self.score == other.score
                }
            }

            impl Eq for State {}

            impl PartialOrd for State {
                fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                    self.score.partial_cmp(&other.score).map(|c| c.reverse())
                }
            }

            impl Ord for State {
                fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                    self.score.partial_cmp(&other.score).unwrap().reverse()
                }
            }
        }
        mod step02_order {
            mod step02_01_annealing {
                use super::{Env, State, TaskType};
                use crate::{
                    common::ChangeMinMax as _,
                    grid::Coord,
                    problem::{Container, Input},
                    solver::beam_storage8::step02_order::Turns,
                };
                use itertools::Itertools;
                use rand::prelude::*;
                use rand_distr::WeightedAliasIndex;

                const STORAGES: [Coord; 8] = [
                    Coord::new(0, 2),
                    Coord::new(1, 2),
                    Coord::new(3, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(1, 3),
                    Coord::new(3, 3),
                    Coord::new(4, 3),
                ];

                pub(super) fn annealing(
                    env: &Env,
                    initial_solution: State,
                    duration: f64,
                ) -> State {
                    let mut state = initial_solution;
                    let mut best_state = state.clone();
                    let mut current_score = state.calc_score(env, usize::MAX).unwrap();
                    let mut best_score = state.calc_score_best_state(env, usize::MAX).unwrap();
                    let init_score = best_score;

                    let mut all_iter = 0;
                    let mut valid_iter = 0;
                    let mut accepted_count = 0;
                    let mut update_count = 0;
                    let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

                    let duration_inv = 1.0 / duration;
                    let since = std::time::Instant::now();

                    let temp0 = env.input.params().temp0();
                    let temp1 = env.input.params().temp1();
                    let mut temp = temp0;
                    let neigh_weights =
                        WeightedAliasIndex::new(env.input.params().neigh_weight().to_vec())
                            .unwrap();

                    loop {
                        all_iter += 1;
                        if (all_iter & ((1 << 4) - 1)) == 0 {
                            let time =
                                (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                            temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);

                            if time >= 1.0 {
                                break;
                            }
                        }

                        // 変形
                        let neigh_type = neigh_weights.sample(&mut rng);
                        let neigh = match neigh_type {
                            0 => SwapAfter::gen(&state, &mut rng),
                            1 => Move::gen(&state, &mut rng),
                            2 => SwapSingle::gen(&state, &mut rng),
                            3 => SwapInCrane::gen(&state, &mut rng),
                            4 => ChangeStoragePos::gen(&state, &mut rng),
                            5 => SwapStoragePos::gen(&state, &mut rng),
                            6 => SwapStorageAll::gen(&state, &mut rng),
                            _ => unreachable!(),
                        };
                        let Some(neigh) = neigh else {
                            continue;
                        };
                        let new_state = neigh.neigh(env, state.clone());

                        // スコア計算
                        let threshold = current_score - temp * rng.gen_range(0.0f64..1.0).ln();
                        let Ok(turns) = new_state.simulate::<Turns>(env, threshold as usize) else {
                            continue;
                        };
                        let new_score = turns.calc_score(env);

                        if new_score <= threshold {
                            // 解の更新
                            current_score = new_score;
                            accepted_count += 1;
                            state = new_state;

                            if best_score.change_min(turns.calc_score_best_state(env)) {
                                best_state = state.clone();
                                update_count += 1;
                            }
                        }

                        valid_iter += 1;
                    }

                    eprintln!("===== annealing =====");
                    eprintln!("init score : {}", init_score);
                    eprintln!("score      : {}", best_score);
                    eprintln!("all iter   : {}", all_iter);
                    eprintln!("valid iter : {}", valid_iter);
                    eprintln!("accepted   : {}", accepted_count);
                    eprintln!("updated    : {}", update_count);
                    eprintln!("");

                    best_state
                }

                trait Neigh {
                    fn neigh(&self, env: &Env, state: State) -> State;
                }

                struct SwapAfter {
                    crane0: usize,
                    crane1: usize,
                    index0: usize,
                    index1: usize,
                }

                impl SwapAfter {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let crane0 = rng.gen_range(0..Input::N);
                        let crane1 = (crane0 + rng.gen_range(1..Input::N)) % Input::N;
                        let index0 = rng.gen_range(0..=state.tasks[crane0].len());
                        let index1 = rng.gen_range(0..=state.tasks[crane1].len());

                        Some(Box::new(Self {
                            crane0,
                            crane1,
                            index0,
                            index1,
                        }))
                    }
                }

                impl Neigh for SwapAfter {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        let temp0 = state.tasks[self.crane0].drain(self.index0..).collect_vec();
                        let temp1 = state.tasks[self.crane1].drain(self.index1..).collect_vec();
                        state.tasks[self.crane0].extend(temp1);
                        state.tasks[self.crane1].extend(temp0);

                        state
                    }
                }

                struct Move {
                    crane0: usize,
                    index0: usize,
                    crane1: usize,
                    index1: usize,
                }

                impl Move {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let candidates = (0..Input::N)
                            .filter(|&i| state.tasks[i].len() >= 1)
                            .collect_vec();

                        let crane0 = *candidates.choose(rng)?;
                        let crane1 = (crane0 + rng.gen_range(1..Input::N)) % Input::N;
                        let index0 = rng.gen_range(0..state.tasks[crane0].len());
                        let index1 = rng.gen_range(0..=state.tasks[crane1].len());

                        Some(Box::new(Self {
                            crane0,
                            index0,
                            crane1,
                            index1,
                        }))
                    }
                }

                impl Neigh for Move {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        let task = state.tasks[self.crane0].remove(self.index0);
                        state.tasks[self.crane1].insert(self.index1, task);

                        state
                    }
                }

                struct SwapSingle {
                    crane0: usize,
                    crane1: usize,
                    index0: usize,
                    index1: usize,
                }

                impl SwapSingle {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let candidates = (0..Input::N)
                            .filter(|&i| state.tasks[i].len() >= 1)
                            .collect_vec();

                        if candidates.len() < 2 {
                            return None;
                        }

                        let mut cranes = candidates.choose_multiple(rng, 2);
                        let crane0 = *cranes.next().unwrap();
                        let crane1 = *cranes.next().unwrap();

                        let index0 = rng.gen_range(0..state.tasks[crane0].len());
                        let index1 = rng.gen_range(0..state.tasks[crane1].len());
                        Some(Box::new(Self {
                            crane0,
                            crane1,
                            index0,
                            index1,
                        }))
                    }
                }

                impl Neigh for SwapSingle {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        let temp = state.tasks[self.crane0][self.index0];
                        state.tasks[self.crane0][self.index0] =
                            state.tasks[self.crane1][self.index1];
                        state.tasks[self.crane1][self.index1] = temp;

                        state
                    }
                }

                struct SwapInCrane {
                    crane: usize,
                    index: usize,
                }

                impl SwapInCrane {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let candidates = (0..Input::N)
                            .filter(|&i| state.tasks[i].len() >= 2)
                            .collect_vec();
                        let crane = *candidates.choose(rng)?;
                        let index = rng.gen_range(0..state.tasks[crane].len() - 1);

                        Some(Box::new(Self { crane, index }))
                    }
                }

                impl Neigh for SwapInCrane {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        state.tasks[self.crane].swap(self.index, self.index + 1);
                        state
                    }
                }

                struct ChangeStoragePos {
                    container: Container,
                    pos: Coord,
                }

                impl ChangeStoragePos {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let mut candidates = vec![];

                        for tasks in state.tasks.iter() {
                            for &task in tasks.iter() {
                                if let TaskType::ToTemporary(container, _, to) = task {
                                    candidates.push((container, to));
                                }
                            }
                        }

                        let &(container, _) = candidates.choose(rng)?;
                        let &pos = STORAGES.choose(rng).unwrap();

                        Some(Box::new(Self { container, pos }))
                    }
                }

                impl Neigh for ChangeStoragePos {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        for tasks in state.tasks.iter_mut() {
                            for task in tasks.iter_mut() {
                                match task {
                                    TaskType::ToTemporary(container, _, to) => {
                                        if *container == self.container {
                                            *to = self.pos;
                                        }
                                    }
                                    TaskType::FromTemporary(container, from, _) => {
                                        if *container == self.container {
                                            *from = self.pos;
                                        }
                                    }
                                    _ => {}
                                }
                            }
                        }

                        state
                    }
                }

                struct SwapStoragePos {
                    container0: Container,
                    container1: Container,
                    pos0: Coord,
                    pos1: Coord,
                }

                impl SwapStoragePos {
                    fn gen(state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let mut candidates = vec![];

                        for tasks in state.tasks.iter() {
                            for &task in tasks.iter() {
                                if let TaskType::ToTemporary(container, _, to) = task {
                                    candidates.push((container, to));
                                }
                            }
                        }

                        if candidates.len() < 2 {
                            return None;
                        }

                        let mut chosen = candidates.choose_multiple(rng, 2);

                        let &(container0, pos0) = chosen.next().unwrap();
                        let &(container1, pos1) = chosen.next().unwrap();

                        Some(Box::new(Self {
                            container0,
                            container1,
                            pos0,
                            pos1,
                        }))
                    }
                }

                impl Neigh for SwapStoragePos {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        for tasks in state.tasks.iter_mut() {
                            for task in tasks.iter_mut() {
                                match task {
                                    TaskType::ToTemporary(container, _, to) => {
                                        if *container == self.container0 {
                                            *to = self.pos1;
                                        } else if *container == self.container1 {
                                            *to = self.pos0;
                                        }
                                    }
                                    TaskType::FromTemporary(container, from, _) => {
                                        if *container == self.container0 {
                                            *from = self.pos1;
                                        } else if *container == self.container1 {
                                            *from = self.pos0;
                                        }
                                    }
                                    _ => {}
                                }
                            }
                        }

                        state
                    }
                }

                struct SwapStorageAll {
                    pos0: Coord,
                    pos1: Coord,
                }

                impl SwapStorageAll {
                    fn gen(_state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
                        let mut pos = STORAGES.choose_multiple(rng, 2);
                        let pos0 = *pos.next().unwrap();
                        let pos1 = *pos.next().unwrap();
                        Some(Box::new(Self { pos0, pos1 }))
                    }
                }

                impl Neigh for SwapStorageAll {
                    fn neigh(&self, _env: &Env, mut state: State) -> State {
                        for tasks in state.tasks.iter_mut() {
                            for task in tasks.iter_mut() {
                                match task {
                                    TaskType::ToTemporary(_, _, to) => {
                                        if *to == self.pos0 {
                                            *to = self.pos1;
                                        } else if *to == self.pos1 {
                                            *to = self.pos0;
                                        }
                                    }
                                    TaskType::FromTemporary(_, from, _) => {
                                        if *from == self.pos0 {
                                            *from = self.pos1;
                                        } else if *from == self.pos1 {
                                            *from = self.pos0;
                                        }
                                    }
                                    _ => {}
                                }
                            }
                        }

                        state
                    }
                }
            }
            mod step02_02_breakdown {
                use crate::problem::{Grid, Input};

                use super::{Env, Recorder, State, SubTask};

                pub(super) fn breakdown(
                    env: &Env,
                    state: &State,
                ) -> Result<[Vec<SubTask>; Input::N], &'static str> {
                    let subtasks: SubTaskRecorder = state.simulate(env, usize::MAX)?;
                    Ok(subtasks.tasks)
                }

                struct SubTaskRecorder {
                    tasks: [Vec<SubTask>; 5],
                    grid: Grid<usize>,
                }

                impl Recorder for SubTaskRecorder {
                    fn new() -> Self {
                        Self {
                            tasks: [vec![], vec![], vec![], vec![], vec![]],
                            grid: Grid::with_default(),
                        }
                    }

                    fn record_pick(&mut self, crane: usize, coord: crate::grid::Coord) {
                        let task = SubTask::Pick(coord, self.grid[coord]);
                        self.tasks[crane].push(task);
                        self.grid[coord] += 1;
                    }

                    fn record_drop(&mut self, crane: usize, coord: crate::grid::Coord) {
                        let task = SubTask::Drop(coord, self.grid[coord]);
                        self.tasks[crane].push(task);
                        self.grid[coord] += 1;
                    }

                    fn record_turn(&mut self, _turns: [usize; Input::N]) {
                        // シミュレーションが終わったので、EOOを追加
                        for tasks in self.tasks.iter_mut() {
                            tasks.push(SubTask::EndOfOrder);
                        }
                    }
                }
            }

            use super::{step01a_gen_dp::Task, DistDict, Precalc};
            use crate::{
                common::ChangeMinMax,
                grid::Coord,
                problem::{Container, CraneState, Grid, Input},
            };
            use rand::prelude::*;
            use rand::SeedableRng;
            use rand_pcg::Pcg64Mcg;
            use std::array;

            pub(super) fn order_tasks(
                input: &Input,
                precalc: &Precalc,
                tasks: Vec<Vec<Task>>,
            ) -> Result<[Vec<SubTask>; Input::N], &'static str> {
                let env = Env::new(&input, &precalc.dist_dict);
                let mut rng = Pcg64Mcg::from_entropy();

                let mut best_state = None;
                let mut best_score = f64::INFINITY;

                if tasks.len() == 0 {
                    return Err("no tasks");
                }

                let step1_duration = 0.5 / tasks.len() as f64;
                let step2_duration = 0.8;

                'main: for tasks in tasks {
                    let mut state = State::new(&tasks, |c, i| c.unwrap_or(i % Input::N));
                    let mut trial_count = 0;

                    loop {
                        if state.calc_score(&env, 1000).is_ok() {
                            break;
                        }

                        trial_count += 1;

                        if trial_count >= 10000 {
                            continue 'main;
                        }

                        eprintln!("retrying...");
                        state = State::new(&tasks, |_, _| rng.gen_range(0..Input::N));
                    }

                    let state = step02_01_annealing::annealing(&env, state, step1_duration);

                    if best_score.change_min(
                        state
                            .calc_score_best_state(&env, usize::MAX)
                            .unwrap_or(f64::INFINITY),
                    ) {
                        best_state = Some(state);
                    }
                }

                let Some(state) = best_state else {
                    return Err("no valid state found");
                };

                let state = step02_01_annealing::annealing(&env, state, step2_duration);

                step02_02_breakdown::breakdown(&env, &state)
            }

            #[derive(Debug, Clone, Copy)]
            enum TaskType {
                Direct(Container, Coord, Coord),
                ToTemporary(Container, Coord, Coord),
                FromTemporary(Container, Coord, Coord),
            }

            #[derive(Debug, Clone)]
            struct Env<'a> {
                input: &'a Input,
                dist_dict: &'a DistDict,
            }

            impl<'a> Env<'a> {
                fn new(input: &'a Input, dist_dict: &'a DistDict) -> Self {
                    Self { input, dist_dict }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                tasks: [Vec<TaskType>; Input::N],
            }

            impl State {
                fn new(
                    tasks: &[Task],
                    mut assign_fn: impl FnMut(Option<usize>, usize) -> usize,
                ) -> Self {
                    let mut assigned_tasks = [vec![], vec![], vec![], vec![], vec![]];

                    for (i, task) in tasks.iter().enumerate() {
                        let t = if task.from().col() == 0 {
                            if task.to().col() == Input::N - 1 {
                                TaskType::Direct(task.container(), task.from(), task.to())
                            } else {
                                TaskType::ToTemporary(task.container(), task.from(), task.to())
                            }
                        } else {
                            TaskType::FromTemporary(task.container(), task.from(), task.to())
                        };

                        assigned_tasks[assign_fn(task.crane(), i)].push(t);
                    }

                    Self {
                        tasks: assigned_tasks,
                    }
                }

                fn calc_score(&self, env: &Env, max_turn: usize) -> Result<f64, &'static str> {
                    let turns: Turns = self.simulate(env, max_turn)?;
                    Ok(turns.calc_score(env))
                }

                fn calc_score_best_state(
                    &self,
                    env: &Env,
                    max_turn: usize,
                ) -> Result<f64, &'static str> {
                    let turns: Turns = self.simulate(env, max_turn)?;
                    Ok(turns.calc_score_best_state(env))
                }

                fn simulate<T: Recorder>(
                    &self,
                    env: &Env,
                    max_turn: usize,
                ) -> Result<T, &'static str> {
                    let mut recorder = T::new();
                    let mut in_ptr = [0; Input::N];
                    let mut out_next: [_; Input::N] = array::from_fn(|i| i * Input::N);
                    let mut task_ptr = [0; Input::N];
                    let mut cranes: [_; Input::N] =
                        array::from_fn(|i| CraneState::Empty(Coord::new(i, 0)));
                    let mut yard = Grid::new([None; Input::N * Input::N]);
                    let mut avail_turns = Grid::new([0; Input::N * Input::N]);
                    let mut last_turns = [0; Input::N];
                    let mut no_progress = 0;

                    for row in 0..Input::N {
                        yard[Coord::new(row, 0)] = Some(env.input.containers()[row][0]);
                    }

                    for turn in 1.. {
                        if turn > max_turn {
                            return Err("turn limit exceeded");
                        }

                        let prev_state = cranes.clone();
                        let flag = env.dist_dict.get_flag(|c| yard[c].is_some());

                        for crane_i in 0..Input::N {
                            let crane = &mut cranes[crane_i];
                            let Some(task) = self.tasks[crane_i].get(task_ptr[crane_i]).copied()
                            else {
                                continue;
                            };

                            match *crane {
                                CraneState::Empty(coord) => {
                                    let (container, from) = match task {
                                        TaskType::Direct(container, from, _) => (container, from),
                                        TaskType::ToTemporary(container, from, _) => {
                                            (container, from)
                                        }
                                        TaskType::FromTemporary(container, from, _) => {
                                            (container, from)
                                        }
                                    };

                                    let can_pick =
                                        yard[from] == Some(container) && avail_turns[from] <= turn;

                                    if coord == from && can_pick {
                                        // コンテナをPickする
                                        *crane = CraneState::Holding(container, from);
                                        avail_turns[from] = turn + 2;
                                        recorder.record_pick(crane_i, coord);

                                        if let TaskType::FromTemporary(_, _, _) = task {
                                            yard[from] = None;
                                        } else {
                                            let row = from.row();
                                            let in_ptr = &mut in_ptr[row];
                                            *in_ptr += 1;
                                            yard[from] =
                                                env.input.containers()[row].get(*in_ptr).copied();
                                        }
                                    } else {
                                        // コンテナに向けて移動
                                        let next = env.dist_dict.next(flag, coord, from, false);
                                        *crane = CraneState::Empty(next);
                                    }
                                }
                                CraneState::Holding(container_t, coord) => {
                                    let (container, to) = match task {
                                        TaskType::Direct(container, _, to) => (container, to),
                                        TaskType::ToTemporary(container, _, to) => (container, to),
                                        TaskType::FromTemporary(container, _, to) => {
                                            (container, to)
                                        }
                                    };
                                    assert_eq!(container, container_t);

                                    let can_drop = if let TaskType::ToTemporary(_, _, _) = task {
                                        yard[to] == None && avail_turns[to] <= turn
                                    } else {
                                        out_next[to.row()] == container.index()
                                            && avail_turns[to] <= turn
                                    };

                                    if coord == to && can_drop {
                                        // コンテナをDropする
                                        *crane = CraneState::Empty(coord);
                                        avail_turns[to] = turn + 2;
                                        task_ptr[crane_i] += 1;
                                        last_turns[crane_i] = turn;
                                        recorder.record_drop(crane_i, coord);

                                        if let TaskType::ToTemporary(_, _, _) = task {
                                            yard[to] = Some(container);
                                        } else {
                                            out_next[to.row()] += 1;
                                        }
                                    } else {
                                        // コンテナに向けて移動
                                        let consider_container = !Input::is_large_crane(crane_i);
                                        let next =
                                            env.dist_dict.next(flag, coord, to, consider_container);
                                        *crane = CraneState::Holding(container, next);
                                    }
                                }
                                CraneState::Destroyed => continue,
                            }
                        }

                        if task_ptr
                            .iter()
                            .zip(&self.tasks)
                            .all(|(&ptr, tasks)| ptr == tasks.len())
                        {
                            recorder.record_turn(last_turns);
                            return Ok(recorder);
                        }

                        if prev_state == cranes {
                            no_progress += 1;

                            // available_turn制約により1ターンは状況が変化しない可能性がある
                            // 2ターン以上状況が変化しない場合は終了
                            if no_progress >= 2 {
                                return Err("no progress");
                            }
                        } else {
                            no_progress = 0;
                        }
                    }

                    unreachable!();
                }
            }

            #[derive(Debug, Clone, Copy, PartialEq, Eq)]
            pub enum SubTask {
                Pick(Coord, usize),
                Drop(Coord, usize),
                EndOfOrder,
            }

            impl SubTask {
                pub fn coord(&self) -> Option<Coord> {
                    match self {
                        SubTask::Pick(coord, _) => Some(*coord),
                        SubTask::Drop(coord, _) => Some(*coord),
                        SubTask::EndOfOrder => None,
                    }
                }

                pub fn index(&self) -> Option<usize> {
                    match self {
                        SubTask::Pick(_, index) => Some(*index),
                        SubTask::Drop(_, index) => Some(*index),
                        SubTask::EndOfOrder => None,
                    }
                }
            }

            trait Recorder {
                fn new() -> Self;
                fn record_pick(&mut self, crane: usize, coord: Coord);
                fn record_drop(&mut self, crane: usize, coord: Coord);
                fn record_turn(&mut self, turns: [usize; Input::N]);
            }

            #[derive(Debug, Clone, Copy)]
            struct Turns {
                turns: [usize; Input::N],
            }

            impl Turns {
                fn new(turns: [usize; Input::N]) -> Self {
                    Self { turns }
                }

                fn calc_score(&self, env: &Env) -> f64 {
                    // logsumexp
                    let kappa = env.input.params().kappa_step02();
                    let logsumexp = self
                        .turns
                        .iter()
                        .map(|&t| (t as f64 / kappa).exp())
                        .sum::<f64>()
                        .ln()
                        * kappa;
                    logsumexp
                }

                fn calc_score_best_state(&self, env: &Env) -> f64 {
                    // logsumexp
                    let kappa = env.input.params().kappa_step02_best();
                    let logsumexp = self
                        .turns
                        .iter()
                        .map(|&t| (t as f64 / kappa).exp())
                        .sum::<f64>()
                        .ln()
                        * kappa;
                    logsumexp
                }
            }

            impl Recorder for Turns {
                fn new() -> Self {
                    Self::new([0; Input::N])
                }

                fn record_pick(&mut self, _crane: usize, _coord: Coord) {
                    // do nothing
                }

                fn record_drop(&mut self, _crane: usize, _coord: Coord) {
                    // do nothing
                }

                fn record_turn(&mut self, turns: [usize; Input::N]) {
                    self.turns = turns;
                }
            }
        }
        mod step03_execute {
            mod step03_01_dag {
                use crate::{
                    grid::Coord,
                    problem::Input,
                    solver::beam_storage8::{step02_order::SubTask, Precalc},
                };
                use itertools::Itertools;
                use std::{collections::HashMap, iter::repeat};

                const DEPOTS: [Coord; 18] = [
                    Coord::new(0, 0),
                    Coord::new(1, 0),
                    Coord::new(2, 0),
                    Coord::new(3, 0),
                    Coord::new(4, 0),
                    Coord::new(0, 2),
                    Coord::new(1, 2),
                    Coord::new(3, 2),
                    Coord::new(4, 2),
                    Coord::new(0, 3),
                    Coord::new(1, 3),
                    Coord::new(3, 3),
                    Coord::new(4, 3),
                    Coord::new(0, 4),
                    Coord::new(1, 4),
                    Coord::new(2, 4),
                    Coord::new(3, 4),
                    Coord::new(4, 4),
                ];

                pub(super) fn critical_path_analysis(
                    tasks: &[Vec<SubTask>; Input::N],
                    precalc: &Precalc,
                ) -> TaskSet {
                    let all_tasks = tasks.iter().flatten().copied().collect_vec();
                    let cranes = (0..Input::N)
                        .flat_map(|i| repeat(i).take(tasks[i].len()).collect_vec())
                        .collect_vec();
                    let mut init_ptr = [0; Input::N];
                    let mut end_ptr = [0; Input::N];

                    for (i, t) in tasks.iter().take(Input::N - 1).enumerate() {
                        init_ptr[i + 1] = init_ptr[i] + t.len() as u8;
                        end_ptr[i] = init_ptr[i + 1] as u8;
                    }

                    end_ptr[Input::N - 1] = all_tasks.len() as u8;

                    let mut grid = HashMap::new();

                    for &d in DEPOTS.iter() {
                        grid.insert(d, vec![]);
                    }

                    for (i, t) in all_tasks.iter().enumerate() {
                        let (Some(coord), Some(index)) = (t.coord(), t.index()) else {
                            continue;
                        };

                        let v = grid.get_mut(&coord).unwrap();

                        if v.len() <= index {
                            v.resize(index + 1, !0);
                        }
                        grid.get_mut(&coord).unwrap()[index] = i;
                    }

                    // DAGを構築
                    let mut graph = vec![vec![]; all_tasks.len()];

                    // 同クレーン内の依存関係
                    for i in 0..all_tasks.len() - 1 {
                        if let (Some(c0), Some(c1)) =
                            (all_tasks[i].coord(), all_tasks[i + 1].coord())
                        {
                            graph[i + 1].push((i, c0.dist(&c1) + 1));
                        } else if all_tasks[i + 1] == SubTask::EndOfOrder {
                            graph[i + 1].push((i, 0));
                        }
                    }

                    // 位置別の依存関係
                    for (i, t) in all_tasks.iter().enumerate() {
                        let (Some(coord), Some(index)) = (t.coord(), t.index()) else {
                            continue;
                        };

                        let v = grid.get(&coord).unwrap();
                        let Some(prev) = v.get(index.wrapping_sub(1)).copied() else {
                            continue;
                        };

                        if cranes[prev] != cranes[i] {
                            graph[i].push((prev, 2));
                        }
                    }

                    let mut dp = vec![0.0; all_tasks.len()];
                    let mut indegrees = vec![0; all_tasks.len()];
                    let mut stack = vec![];

                    for edges in graph.iter() {
                        for &(v, _) in edges.iter() {
                            indegrees[v] += 1;
                        }
                    }

                    for v in 0..all_tasks.len() {
                        if indegrees[v] == 0 {
                            stack.push(v);
                            dp[v] = precalc.exp_table[0];
                        }
                    }

                    while let Some(v) = stack.pop() {
                        for &(u, cost) in graph[v].iter() {
                            dp[u] += dp[v] * precalc.exp_table[cost];
                            indegrees[u] -= 1;

                            if indegrees[u] == 0 {
                                stack.push(u);
                            }
                        }
                    }

                    assert!(indegrees.iter().all(|&x| x == 0));

                    TaskSet::new(all_tasks, dp, init_ptr, end_ptr)
                }

                #[derive(Debug, Clone)]
                pub struct TaskSet {
                    pub tasks: Vec<SubTask>,
                    pub dp: Vec<f64>,
                    pub init_ptr: [u8; Input::N],
                    pub end_ptr: [u8; Input::N],
                }

                impl TaskSet {
                    pub fn new(
                        tasks: Vec<SubTask>,
                        dp: Vec<f64>,
                        init_ptr: [u8; Input::N],
                        end_ptr: [u8; Input::N],
                    ) -> Self {
                        Self {
                            tasks,
                            dp,
                            init_ptr,
                            end_ptr,
                        }
                    }
                }
            }
            mod step03_02_beam {
                use super::{
                    super::step02_order::SubTask, step03_01_dag::TaskSet, Precalc, StorageFlag,
                };
                use crate::{
                    data_structures::{History, HistoryIndex},
                    grid::Coord,
                    problem::{Container, CraneState, Grid, Input, Operation},
                };
                use rand::prelude::*;
                use rand_pcg::Pcg64Mcg;
                use std::array;

                const HASH_LEN: usize = 11;
                const BEAM_SIZE: usize = 1 << HASH_LEN;

                pub(super) fn beam(
                    input: &Input,
                    precalc: &Precalc,
                    tasks: TaskSet,
                    max_turn: usize,
                ) -> Result<Vec<[Operation; Input::N]>, &'static str> {
                    let env = Env::new(input, precalc, tasks);
                    let mut history = History::new();
                    let mut beam = vec![None; BEAM_SIZE];
                    beam[0] = Some(State::init(&env));
                    let mut turn = 0;

                    while !beam
                        .iter()
                        .any(|s| s.map_or(false, |s| s.is_completed(&env)))
                    {
                        turn += 1;

                        if turn > max_turn {
                            return Err("turn limit exceeded");
                        }

                        let mut next_beam = vec![None; BEAM_SIZE];
                        for state in beam {
                            let Some(state) = state else {
                                continue;
                            };

                            state.gen_next(&env, &mut next_beam, &mut history);
                        }

                        beam = next_beam;
                    }

                    let best = beam
                        .iter()
                        .filter(|s| s.is_some_and(|s| s.is_completed(&env)))
                        .next()
                        .unwrap()
                        .unwrap();

                    Ok(history.collect(best.history))
                }

                struct Env<'a> {
                    _input: &'a Input,
                    precalc: &'a Precalc,
                    tasks: TaskSet,
                    hashes: [Grid<usize>; Input::N],
                }

                impl<'a> Env<'a> {
                    fn new(input: &'a Input, precalc: &'a Precalc, tasks: TaskSet) -> Self {
                        const HASH_MASK: usize = (1 << HASH_LEN) - 1;
                        let mut hashes = [Grid::new([0; Input::N * Input::N]); Input::N];
                        let mut rng = Pcg64Mcg::new(42);

                        for i in 0..Input::N {
                            for row in 0..Input::N {
                                for col in 0..Input::N {
                                    let c = Coord::new(row, col);
                                    hashes[i][c] = rng.gen::<usize>() & HASH_MASK;
                                }
                            }
                        }

                        Self {
                            _input: input,
                            precalc,
                            tasks,
                            hashes,
                        }
                    }
                }

                #[derive(Debug, Clone, Copy)]
                struct State {
                    task_ptr: [u8; Input::N],
                    grid_ptr: Grid<u8>,
                    cranes: [CraneState; Input::N],
                    board: Grid<i8>,
                    history: HistoryIndex,
                    score: f64,
                }

                impl State {
                    fn init(env: &Env) -> Self {
                        let task_ptr = env.tasks.init_ptr;
                        let cranes = array::from_fn(|i| CraneState::Empty(Coord::new(i, 0)));
                        let history = HistoryIndex::ROOT;
                        let mut board = Grid::with_default();
                        board[Coord::new(0, 0)] = 5;
                        board[Coord::new(1, 0)] = 5;
                        board[Coord::new(2, 0)] = 5;
                        board[Coord::new(3, 0)] = 5;
                        board[Coord::new(4, 0)] = 5;
                        board[Coord::new(0, 4)] = -5;
                        board[Coord::new(1, 4)] = -5;
                        board[Coord::new(2, 4)] = -5;
                        board[Coord::new(3, 4)] = -5;
                        board[Coord::new(4, 4)] = -5;

                        let storage_flag = env.precalc.dist_dict.get_flag(|c| board[c] > 0);
                        let grid_ptr = Grid::with_default();

                        let mut scores = [0.0; Input::N];

                        for (i, &task_ptr) in task_ptr.iter().enumerate() {
                            let crane = cranes[i];
                            let task = &env.tasks.tasks[task_ptr as usize];
                            let edge_cost = if let (Some(c0), Some(c1)) =
                                (cranes[i].coord(), task.coord())
                            {
                                let consider_container =
                                    !Input::is_large_crane(i) && crane.is_holding();
                                env.precalc
                                    .dist_dict
                                    .dist(storage_flag, c0, c1, consider_container)
                                    + 1
                            } else {
                                0
                            };

                            scores[i] =
                                env.tasks.dp[task_ptr as usize] * env.precalc.exp_table[edge_cost];
                        }

                        let score = scores.iter().sum::<f64>();

                        Self::new(task_ptr, grid_ptr, cranes, board, score, history)
                    }

                    fn new(
                        task_ptr: [u8; Input::N],
                        grid_ptr: Grid<u8>,
                        cranes: [CraneState; Input::N],
                        board: Grid<i8>,
                        score: f64,
                        history: HistoryIndex,
                    ) -> Self {
                        Self {
                            task_ptr,
                            grid_ptr,
                            cranes,
                            board,
                            history,
                            score,
                        }
                    }

                    fn gen_next(
                        &self,
                        env: &Env,
                        beam: &mut Vec<Option<State>>,
                        history: &mut History<[Operation; Input::N]>,
                    ) {
                        // 操作の候補を列挙
                        let mut candidates = [vec![], vec![], vec![], vec![], vec![]];
                        const MOVES: [Operation; 5] = [
                            Operation::None,
                            Operation::Up,
                            Operation::Right,
                            Operation::Down,
                            Operation::Left,
                        ];

                        let storage_flag = env.precalc.dist_dict.get_flag(|c| self.board[c] > 0);

                        for crane_i in 0..Input::N {
                            let crane = self.cranes[crane_i];
                            let candidates = &mut candidates[crane_i];
                            let task = env.tasks.tasks[self.task_ptr[crane_i] as usize];

                            if crane == CraneState::Destroyed {
                                candidates.push(Operation::None);
                            } else if task == SubTask::EndOfOrder {
                                candidates.push(Operation::Destroy);
                            } else {
                                let coord = crane.coord().unwrap();
                                let destination = task.coord().unwrap();

                                if coord == destination
                                    && self.grid_ptr[coord] == task.index().unwrap() as u8
                                {
                                    // pick / drop
                                    let op = match task {
                                        SubTask::Pick(_, _) => Operation::Pick,
                                        SubTask::Drop(_, _) => Operation::Drop,
                                        SubTask::EndOfOrder => unreachable!(),
                                    };
                                    candidates.push(op);
                                } else {
                                    let nearest_crane_dist = self
                                        .cranes
                                        .iter()
                                        .filter_map(|c| c.coord())
                                        .map(|c| c.dist(&coord))
                                        .filter(|&d| d > 0)
                                        .min()
                                        .unwrap_or(usize::MAX);
                                    let consider_container =
                                        !Input::is_large_crane(crane_i) && crane.is_holding();
                                    let current_dist = env.precalc.dist_dict.dist(
                                        storage_flag,
                                        coord,
                                        destination,
                                        consider_container,
                                    );

                                    if crane.is_holding() && !Input::is_large_crane(crane_i) {
                                        // コンテナを考慮して移動
                                        for &op in MOVES.iter() {
                                            let next = coord + op.dir();
                                            if next.in_map(Input::N) && self.board[next] <= 0 {
                                                let next_dist = env.precalc.dist_dict.dist(
                                                    storage_flag,
                                                    next,
                                                    destination,
                                                    consider_container,
                                                );

                                                // 他のクレーンが遠いときは損しない移動方法だけを考慮
                                                if nearest_crane_dist <= 1
                                                    || next_dist <= current_dist
                                                {
                                                    candidates.push(op);
                                                }
                                            }
                                        }
                                    } else {
                                        // コンテナを無視して移動
                                        for &op in MOVES.iter() {
                                            let next = coord + op.dir();
                                            if next.in_map(Input::N) {
                                                let next_dist = env.precalc.dist_dict.dist(
                                                    storage_flag,
                                                    next,
                                                    destination,
                                                    consider_container,
                                                );

                                                // 他のクレーンが遠いときは損しない移動方法だけを考慮
                                                if nearest_crane_dist <= 1
                                                    || next_dist <= current_dist
                                                {
                                                    candidates.push(op);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        let mut cant_in = Grid::new([false; Input::N * Input::N]);
                        let mut cant_move = Grid::new([[false; 8]; Input::N * Input::N]);
                        let mut state = self.clone();

                        state.dfs(
                            env,
                            &candidates,
                            &mut cant_in,
                            &mut cant_move,
                            history,
                            &mut [Operation::None; Input::N],
                            beam,
                            storage_flag,
                            0.0,
                            0,
                            0,
                        );
                    }

                    fn dfs(
                        &mut self,
                        env: &Env,
                        candidates: &[Vec<Operation>],
                        cant_in: &mut Grid<bool>,
                        cant_move: &mut Grid<[bool; 8]>,
                        history: &mut History<[Operation; Input::N]>,
                        operations: &mut [Operation; Input::N],
                        beam: &mut Vec<Option<State>>,
                        storage_flag: StorageFlag,
                        score: f64,
                        hash: usize,
                        depth: usize,
                    ) {
                        if depth == Input::N {
                            let old_score = beam[hash].map_or(f64::MAX, |s| s.score);

                            if score < old_score {
                                let mut new_state = Self::new(
                                    self.task_ptr,
                                    self.grid_ptr,
                                    self.cranes,
                                    self.board,
                                    score,
                                    self.history,
                                );

                                let hist_index = history.push(operations.clone(), self.history);
                                new_state.history = hist_index;
                                beam[hash] = Some(new_state);
                            }

                            return;
                        }

                        let crane_i = depth;
                        let crane = self.cranes[crane_i];
                        let task = env.tasks.tasks[self.task_ptr[crane_i] as usize];

                        for &op in candidates[depth].iter() {
                            if crane == CraneState::Destroyed {
                                operations[crane_i] = Operation::None;

                                self.dfs(
                                    env,
                                    candidates,
                                    cant_in,
                                    cant_move,
                                    history,
                                    operations,
                                    beam,
                                    storage_flag,
                                    score,
                                    hash,
                                    depth + 1,
                                );

                                continue;
                            }

                            let coord = crane.coord().unwrap();
                            let next = coord + op.dir();
                            let op_usize = op as usize;

                            if (cant_in[next] && op != Operation::Destroy)
                                || cant_move[coord][op_usize]
                            {
                                continue;
                            }

                            let board_change = match op {
                                Operation::Pick => -1,
                                Operation::Drop => 1,
                                _ => 0,
                            };
                            let current_crane = self.cranes[crane_i];
                            let task_ptr_diff = match op {
                                Operation::Pick => 1,
                                Operation::Drop => 1,
                                _ => 0,
                            };
                            self.board[coord] += board_change;
                            self.grid_ptr[coord] =
                                self.grid_ptr[coord].wrapping_add_signed(task_ptr_diff);
                            self.task_ptr[crane_i] =
                                self.task_ptr[crane_i].wrapping_add_signed(task_ptr_diff);
                            self.cranes[crane_i] = if task == SubTask::EndOfOrder {
                                CraneState::Destroyed
                            } else {
                                match op {
                                    Operation::Pick => {
                                        CraneState::Holding(Container::new(!0), coord)
                                    }
                                    Operation::Drop => CraneState::Empty(coord),
                                    _ => match current_crane {
                                        CraneState::Empty(_) => CraneState::Empty(next),
                                        CraneState::Holding(container, _) => {
                                            CraneState::Holding(container, next)
                                        }
                                        CraneState::Destroyed => CraneState::Destroyed,
                                    },
                                }
                            };

                            operations[crane_i] = op;

                            if op != Operation::Destroy {
                                cant_in[next] = true;
                            }

                            // クロスする移動もNG
                            if op_usize < 4 {
                                cant_move[next][op_usize ^ 2] = true;
                            }

                            // ハッシュ値を更新
                            let new_hash = hash ^ env.hashes[crane_i][next];

                            // スコア
                            let new_crane = self.cranes[crane_i].coord();
                            let new_task_coord =
                                env.tasks.tasks[self.task_ptr[crane_i] as usize].coord();
                            let edge_cost = if let (Some(c0), Some(c1)) =
                                (new_crane, new_task_coord)
                            {
                                let consider_container = !Input::is_large_crane(crane_i)
                                    && self.cranes[crane_i].is_holding();
                                env.precalc
                                    .dist_dict
                                    .dist(storage_flag, c0, c1, consider_container)
                                    + 1
                            } else {
                                0
                            };

                            let task_ptr = self.task_ptr[crane_i];
                            let score_diff =
                                env.tasks.dp[task_ptr as usize] * env.precalc.exp_table[edge_cost];

                            self.dfs(
                                env,
                                candidates,
                                cant_in,
                                cant_move,
                                history,
                                operations,
                                beam,
                                storage_flag,
                                score + score_diff,
                                new_hash,
                                depth + 1,
                            );

                            self.board[coord] -= board_change;
                            self.grid_ptr[coord] =
                                self.grid_ptr[coord].wrapping_add_signed(-task_ptr_diff);
                            self.task_ptr[crane_i] =
                                self.task_ptr[crane_i].wrapping_add_signed(-task_ptr_diff);
                            self.cranes[crane_i] = current_crane;

                            if op != Operation::Destroy {
                                cant_in[next] = false;
                            }

                            if op_usize < 4 {
                                cant_move[next][op_usize ^ 2] = false;
                            }
                        }
                    }

                    fn is_completed(&self, env: &Env) -> bool {
                        self.task_ptr
                            .iter()
                            .all(|&ptr| env.tasks.tasks[ptr as usize] == SubTask::EndOfOrder)
                    }
                }

                impl PartialEq for State {
                    fn eq(&self, other: &Self) -> bool {
                        self.score == other.score
                    }
                }

                impl Eq for State {}

                impl PartialOrd for State {
                    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                        self.score.partial_cmp(&other.score)
                    }
                }

                impl Ord for State {
                    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                        self.partial_cmp(&other).unwrap()
                    }
                }
            }

            use super::{step02_order::SubTask, Precalc, StorageFlag};
            use crate::problem::{Input, Operation};

            pub(super) fn execute(
                input: &Input,
                precalc: &Precalc,
                tasks: &[Vec<SubTask>; Input::N],
                max_turn: usize,
            ) -> Result<Vec<[Operation; Input::N]>, &'static str> {
                let tasks = step03_01_dag::critical_path_analysis(tasks, precalc);
                step03_02_beam::beam(input, precalc, tasks, max_turn)
            }
        }

        pub struct BeamSolver8 {
            seed: u64,
            max_turn: usize,
        }

        impl BeamSolver8 {
            pub fn new(seed: u64, max_turn: usize) -> Self {
                Self { seed, max_turn }
            }
        }

        impl Solver for BeamSolver8 {
            fn solve(
                &self,
                input: &crate::problem::Input,
            ) -> Result<super::SolverResult, &'static str> {
                let mut rng = Pcg64Mcg::seed_from_u64(self.seed);

                // バグを直す（precalc_no_inf）とスコアが下がるので両方用意……
                let precalc_inf = Precalc::new(input, false);
                let precalc_no_inf = Precalc::new(input, true);

                let mut all_tasks = vec![];
                if let Ok(tasks) = step01a_gen_dp::generate_tasks(input, &mut rng) {
                    all_tasks.push(tasks);
                };

                let since = std::time::Instant::now();
                if let Ok(tasks) = step01b_gen_beam::generate_tasks(input, &precalc_no_inf) {
                    all_tasks.push(tasks);
                };
                eprintln!("step01b elapsed: {:?}", since.elapsed());

                let subtasks = step02_order::order_tasks(input, &precalc_inf, all_tasks)?;

                let since = std::time::Instant::now();
                let operations =
                    step03_execute::execute(input, &precalc_inf, &subtasks, self.max_turn)?;
                eprintln!("elapsed: {:?}", since.elapsed());
                let mut yard = Yard::new(&input);
                let mut output = Output::new();

                for op in operations.iter() {
                    yard.apply(op)?;
                    yard.carry_in_and_ship();
                    output.push(op);
                }

                Ok(SolverResult::new(output, &yard))
            }
        }

        #[derive(Debug, Clone)]
        struct Precalc {
            dist_dict: DistDict,
            exp_table: [f64; 1000],
        }

        impl Precalc {
            fn new(input: &Input, avoid_inf: bool) -> Self {
                let exp_table =
                    array::from_fn(|i| (i as f64 / input.params().kappa_step03()).exp());

                Self {
                    dist_dict: DistDict::new(avoid_inf),
                    exp_table,
                }
            }
        }

        #[derive(Debug, Clone)]
        struct DistDict {
            dists: [Vec<Grid<Grid<usize>>>; 2],
            next: [Vec<Grid<Grid<Coord>>>; 2],
        }

        impl DistDict {
            const STORAGES: [Coord; 13] = [
                Coord::new(0, 0),
                Coord::new(1, 0),
                Coord::new(2, 0),
                Coord::new(3, 0),
                Coord::new(4, 0),
                Coord::new(0, 2),
                Coord::new(1, 2),
                Coord::new(3, 2),
                Coord::new(4, 2),
                Coord::new(0, 3),
                Coord::new(1, 3),
                Coord::new(3, 3),
                Coord::new(4, 3),
            ];

            fn new(avoid_inf: bool) -> Self {
                let mut dists_with_container = vec![];
                let mut next_with_container = vec![];

                for flag in 0..1 << Self::STORAGES.len() {
                    let mut board = Grid::new([false; Input::N * Input::N]);

                    for (i, &storage) in Self::STORAGES.iter().enumerate() {
                        if (flag & (1 << i)) > 0 {
                            board[storage] = true;
                        }
                    }

                    let mut d = Grid::new(
                        [Grid::new([usize::MAX; Input::N * Input::N]); Input::N * Input::N],
                    );
                    let mut next = Grid::new(
                        [Grid::new([Coord::new(0, 0); Input::N * Input::N]); Input::N * Input::N],
                    );

                    for row in 0..Input::N {
                        for col in 0..Input::N {
                            let c = Coord::new(row, col);
                            Self::bfs(&board, &mut d[c], &mut next[c], c, avoid_inf);
                        }
                    }

                    dists_with_container.push(d);
                    next_with_container.push(next);
                }

                // コンテナなし = flagが0
                let dists_without_container =
                    vec![dists_with_container[0].clone(); dists_with_container.len()];
                let next_without_container =
                    vec![next_with_container[0].clone(); next_with_container.len()];

                Self {
                    dists: [dists_without_container, dists_with_container],
                    next: [next_without_container, next_with_container],
                }
            }

            fn bfs(
                board: &Grid<bool>,
                dists: &mut Grid<usize>,
                next: &mut Grid<Coord>,
                from: Coord,
                avoid_inf: bool,
            ) {
                dists[from] = 0;
                let mut queue = VecDeque::new();
                queue.push_back(from);

                while let Some(coord) = queue.pop_front() {
                    let next_dist = dists[coord] + 1;

                    for &adj in ADJACENTS.iter() {
                        let next = coord + adj;

                        if next.in_map(Input::N)
                            && !board[next]
                            && dists[next].change_min(next_dist)
                        {
                            queue.push_back(next);
                        }
                    }
                }

                // INFになるのを防ぐ

                if avoid_inf {
                    let mut temp = dists.clone();

                    for row in 0..Input::N {
                        for col in 0..Input::N {
                            let c = Coord::new(row, col);

                            for &adj in ADJACENTS.iter() {
                                let next = c + adj;

                                if next.in_map(Input::N) {
                                    temp[c].change_min(dists[next].saturating_add(1));
                                }
                            }
                        }
                    }

                    *dists = temp;
                }

                for row in 0..Input::N {
                    for col in 0..Input::N {
                        let c = Coord::new(row, col);
                        let mut best = c;
                        let mut best_dist = dists[best];

                        for &adj in ADJACENTS.iter() {
                            let next = c + adj;

                            if next.in_map(Input::N) && best_dist.change_min(dists[next]) {
                                best = next;
                            }
                        }

                        next[c] = best;
                    }
                }
            }

            fn get_flag(&self, f: impl Fn(Coord) -> bool) -> StorageFlag {
                let mut flag = 0;

                for (i, &storage) in Self::STORAGES.iter().enumerate() {
                    if f(storage) {
                        flag |= 1 << i;
                    }
                }

                StorageFlag(flag)
            }

            fn dist(
                &self,
                flag: StorageFlag,
                from: Coord,
                to: Coord,
                consider_container: bool,
            ) -> usize {
                // [to][from]の順になることに注意
                self.dists[consider_container as usize][flag.0][to][from]
            }

            fn next(
                &self,
                flag: StorageFlag,
                from: Coord,
                to: Coord,
                consider_container: bool,
            ) -> Coord {
                self.next[consider_container as usize][flag.0][to][from]
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        struct StorageFlag(usize);
    }
    pub mod single_crane {
        use crate::{
            common::ChangeMinMax as _,
            grid::Coord,
            problem::{Input, Operation, Output, Yard},
        };

        use super::{Solver, SolverResult};

        pub struct SingleCraneSolver;

        impl Solver for SingleCraneSolver {
            fn solve(&self, input: &Input) -> Result<SolverResult, &'static str> {
                let mut yard = Yard::new(&input);
                let mut output = Output::new();

                // 取り出す
                let rule = vec![
                    Operation::Pick,
                    Operation::Right,
                    Operation::Right,
                    Operation::Right,
                    Operation::Drop,
                    Operation::Left,
                    Operation::Left,
                    Operation::Left,
                    Operation::Pick,
                    Operation::Right,
                    Operation::Right,
                    Operation::Drop,
                    Operation::Left,
                    Operation::Left,
                    Operation::Pick,
                    Operation::Right,
                    Operation::Drop,
                ];

                for &operation in rule.iter() {
                    let operations = [operation; Input::N];
                    yard.apply(&operations)?;
                    yard.carry_in_and_ship();
                    output.push(&operations);
                }

                // 爆破
                let operation = [
                    Operation::None,
                    Operation::Destroy,
                    Operation::Destroy,
                    Operation::Destroy,
                    Operation::Destroy,
                ];
                yard.apply(&operation)?;
                output.push(&operation);

                // 運ぶ
                while !yard.is_end() {
                    if let Some(next_c) = find_next_pick(&yard) {
                        let container = yard.grid()[next_c].unwrap();
                        move_to(&mut yard, &mut output, next_c)?;
                        apply_single(&mut yard, &mut output, Operation::Pick)?;
                        move_to(&mut yard, &mut output, Input::get_goal(container))?;
                        apply_single(&mut yard, &mut output, Operation::Drop)?;
                    } else if let Some((start, goal)) = find_next_stock(&yard) {
                        move_to(&mut yard, &mut output, start)?;
                        apply_single(&mut yard, &mut output, Operation::Pick)?;
                        move_to(&mut yard, &mut output, goal)?;
                        apply_single(&mut yard, &mut output, Operation::Drop)?;
                    } else {
                        let c = yard.cranes()[0].coord().unwrap();
                        let container = yard.grid()[c].unwrap();
                        apply_single(&mut yard, &mut output, Operation::Pick)?;
                        move_to(&mut yard, &mut output, Input::get_goal(container))?;
                        apply_single(&mut yard, &mut output, Operation::Drop)?;
                    }
                }

                Ok(SolverResult::new(output, &yard))
            }
        }

        fn move_to(
            yard: &mut Yard,
            output: &mut Output,
            destination: Coord,
        ) -> Result<(), &'static str> {
            while yard.cranes()[0].coord().unwrap().row() < destination.row() {
                apply_single(yard, output, Operation::Down)?;
            }

            while yard.cranes()[0].coord().unwrap().row() > destination.row() {
                apply_single(yard, output, Operation::Up)?;
            }

            while yard.cranes()[0].coord().unwrap().col() < destination.col() {
                apply_single(yard, output, Operation::Right)?;
            }

            while yard.cranes()[0].coord().unwrap().col() > destination.col() {
                apply_single(yard, output, Operation::Left)?;
            }

            Ok(())
        }

        fn apply_single(
            yard: &mut Yard,
            output: &mut Output,
            operation: Operation,
        ) -> Result<(), &'static str> {
            let mut operations = [Operation::None; Input::N];

            operations[0] = operation;
            yard.apply(&operations)?;
            output.push(&operations);
            yard.carry_in_and_ship();

            Ok(())
        }

        fn find_next_pick(yard: &Yard) -> Option<Coord> {
            for row in 0..Input::N {
                for col in 0..Input::N - 1 {
                    let c = Coord::new(row, col);
                    let Some(container) = yard.grid()[c] else {
                        continue;
                    };

                    if yard.is_next_ship(container) {
                        return Some(c);
                    }
                }
            }

            None
        }

        fn find_next_stock(yard: &Yard) -> Option<(Coord, Coord)> {
            let mut best_pair = None;
            let mut best_cost = usize::MAX;

            for start_row in 0..Input::N {
                let start = Coord::new(start_row, 0);

                if yard.grid()[start].is_none() || yard.waiting()[start_row].is_empty() {
                    continue;
                }

                for row in 0..Input::N {
                    for col in 0..Input::N - 1 {
                        let goal = Coord::new(row, col);

                        if yard.grid()[goal].is_none() {
                            let cost =
                                yard.cranes()[0].coord().unwrap().dist(&start) + start.dist(&goal);

                            if best_cost.change_min(cost) {
                                best_pair = Some((start, goal));
                            }
                        }
                    }
                }
            }

            best_pair
        }
    }

    use crate::problem::{Input, Output, Yard};

    pub trait Solver {
        fn solve(&self, input: &Input) -> Result<SolverResult, &'static str>;
    }

    #[derive(Debug, Clone)]
    pub struct SolverResult {
        output: Output,
        score: u32,
    }

    impl SolverResult {
        fn new(output: Output, yard: &Yard) -> Self {
            let score = output.len() as u32 + yard.inversions() * 100;
            Self { output, score }
        }

        pub fn output(&self) -> &Output {
            &self.output
        }

        pub fn score(&self) -> u32 {
            self.score
        }
    }
}

use crate::{
    common::ChangeMinMax,
    solver::{
        beam::BeamSolver, beam_storage8::BeamSolver8, single_crane::SingleCraneSolver, Solver,
    },
};
use problem::Input;
use rand::{Rng as _, SeedableRng};
use rand_pcg::Pcg64Mcg;

fn main() -> Result<(), &'static str> {
    let input = Input::read_input();
    let since = std::time::Instant::now();
    let mut best_result = SingleCraneSolver.solve(&input)?;
    let mut best_score = best_result.score();
    let mut rng = Pcg64Mcg::from_entropy();

    let solver = BeamSolver::new(rng.gen(), best_score as usize);
    match solver.solve(&input) {
        Ok(result) => {
            let score = result.score();

            if best_score.change_min(score) {
                best_result = result;
                eprintln!("score updated!: {}", score);
            }
        }
        Err(err) => {
            eprintln!("{}", err);

            // 一時保管場所は基本的に6箇所だが、6箇所で足りない場合があるため8箇所も試す
            if since.elapsed().as_secs_f64() <= 0.1 {
                eprintln!("Trying BeamSolver8...");
                let solver = BeamSolver8::new(rng.gen(), best_score as usize);

                match solver.solve(&input) {
                    Ok(result) => {
                        let score = result.score();

                        if best_score.change_min(score) {
                            best_result = result;
                            eprintln!("score updated!: {}", score);
                        }
                    }
                    Err(err) => {
                        eprintln!("{}", err);
                    }
                }
            }
        }
    };

    print!("{}", best_result.output());
    eprintln!("Score: {}", best_result.score());

    Ok(())
}
