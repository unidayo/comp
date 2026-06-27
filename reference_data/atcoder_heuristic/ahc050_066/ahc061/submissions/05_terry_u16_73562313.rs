use crate::problem::Input;

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
mod estimator {
    use crate::{
        problem::{Cell, GameState, Input, Territory, AI},
        simd::AlignedF64,
    };
    use rand::{seq::SliceRandom, Rng, SeedableRng};
    use rand_distr::Normal;
    use rand_pcg::Pcg64Mcg;
    use std::arch::x86_64::*;

    pub fn run_mcmc(
        history: &ActionHistory,
        player_id: usize,
        ai_cands: &[AI],
        warmup_iter: usize,
        sample_count: usize,
        sample_interval: usize,
    ) -> Vec<AI> {
        let mut rng = Pcg64Mcg::from_os_rng();
        let env = Env::new(history, player_id);
        let mut best_state = State::new(&env, AI::new_random(player_id, &mut rng));

        for ai in ai_cands.iter() {
            let state = State::new(&env, ai.clone());
            if state.likelihood > best_state.likelihood {
                best_state = state;
            }
        }

        unsafe {
            run_mcmc_inner(
                warmup_iter,
                sample_count,
                sample_interval,
                rng,
                env,
                best_state,
            )
        }
    }

    #[target_feature(enable = "avx,avx2")]
    unsafe fn run_mcmc_inner(
        warmup_iter: usize,
        sample_count: usize,
        sample_interval: usize,
        mut rng: rand_pcg::Mcg128Xsl64,
        env: Env,
        best_state: State,
    ) -> Vec<AI> {
        let mut state = best_state;

        // warmup
        for _ in 0..warmup_iter {
            let new_state = state.generate_neighbor(&env, &mut rng);

            if metropolis_test(state.likelihood, new_state.likelihood, &mut rng) {
                state = new_state;
            }
        }

        // sampling
        let mut samples = Vec::with_capacity(sample_count);
        samples.push(state.ai.clone());
        let mut accepted = 0;
        let mut iter = 0;

        for _ in 1..sample_count {
            for _ in 0..sample_interval {
                iter += 1;
                let new_state = state.generate_neighbor(&env, &mut rng);

                if metropolis_test(state.likelihood, new_state.likelihood, &mut rng) {
                    accepted += 1;
                    state = new_state;
                }
            }

            samples.push(state.ai.clone());
        }

        eprintln!(
            "MCMC: acceptance rate = {:.2}%, total iter = {}",
            accepted as f64 / iter as f64 * 100.0,
            iter
        );

        samples.shuffle(&mut rng);

        samples
    }

    fn metropolis_test(old_likelihood: f64, new_likelihood: f64, rng: &mut impl Rng) -> bool {
        if new_likelihood >= old_likelihood {
            true
        } else {
            let acceptance_prob = new_likelihood / old_likelihood;
            rng.random_bool(acceptance_prob)
        }
    }

    struct Env {
        histories: Vec<HistoryRecord>,
    }

    impl Env {
        fn new(history: &ActionHistory, player_id: usize) -> Self {
            let mut histories = vec![];

            for record_set in history.history.iter() {
                let record = &record_set.records[player_id];
                histories.push(record.clone());
            }

            Self { histories }
        }
    }

    struct State {
        ai: AI,
        likelihood: f64,
    }

    impl State {
        const WEIGHT_STD_DEV: f64 = 0.14;
        const EPS_STD_DEV: f64 = 0.08;

        fn new(env: &Env, ai: AI) -> Self {
            let likelihood = unsafe { Self::calc_likelihood(env, &ai) };
            Self { ai, likelihood }
        }

        fn generate_neighbor(&self, env: &Env, rng: &mut impl Rng) -> Self {
            let mut new_ai = self.ai.clone();

            let index = rng.random_range(0..5);

            if index < 4 {
                let dist = Normal::new(new_ai.weights[index], Self::WEIGHT_STD_DEV).unwrap();
                let new_weight = loop {
                    let sample = rng.sample(dist);
                    if (AI::MIN_WEIGHT as f64..=AI::MAX_WEIGHT as f64).contains(&sample) {
                        break sample;
                    }
                };

                new_ai.weights[index] = new_weight;
            } else {
                let dist = Normal::new(new_ai.eps, Self::EPS_STD_DEV).unwrap();
                let new_eps = loop {
                    let sample = rng.sample(dist);
                    if (AI::MIN_EPS..=AI::MAX_EPS).contains(&sample) {
                        break sample;
                    }
                };

                new_ai.eps = new_eps;
            }

            Self::new(env, new_ai)
        }

        #[target_feature(enable = "avx,avx2")]
        unsafe fn calc_likelihood(env: &Env, ai: &AI) -> f64 {
            let mut all_likelihood = 1.0;
            let ai_weights = AlignedF64(ai.weights).load();

            for record in env.histories.iter() {
                let actual_eval = record.action_value * ai.weights[record.action_type];
                let actual_evals = _mm256_set1_pd(actual_eval);
                let best_values = record.best_values.load();
                let best_evals = _mm256_mul_pd(best_values, ai_weights);

                // 全要素actual_evals>=best_evalsだったら貪欲で選ばれたと見なす
                //eprintln!("{} vs {:?}", actual_eval, best_evals);
                let cond = _mm256_cmp_pd(actual_evals, best_evals, _CMP_GE_OQ);
                let cond = _mm256_movemask_pd(cond) == 0b1111;

                let random_prob = ai.eps * record.random_prob;
                let greedy_prob = (1.0 - ai.eps) * record.greedy_prob * ((cond as u8) as f64);
                all_likelihood *= random_prob + greedy_prob;
            }

            //eprintln!("likelihood: {}", all_likelihood);

            all_likelihood
        }
    }

    #[derive(Debug, Clone)]
    pub struct ActionHistory {
        history: Vec<HistoryRecordSet>,
    }

    impl ActionHistory {
        pub fn new() -> Self {
            Self {
                history: Vec::new(),
            }
        }

        pub fn push(&mut self, input: &Input, state: &GameState, actions: &[Cell]) {
            self.history
                .push(HistoryRecordSet::new(input, state, actions));
        }
    }

    #[derive(Debug, Clone)]
    struct HistoryRecordSet {
        records: Vec<HistoryRecord>,
    }

    impl HistoryRecordSet {
        fn new(input: &Input, state: &GameState, actions: &[Cell]) -> Self {
            let mut best_values = Vec::with_capacity(input.player_count);
            let records = (0..input.player_count)
                .map(|player_index| {
                    let action = actions[player_index];
                    let record = HistoryRecord::new(input, state, player_index, action);
                    best_values.push(record.best_values);
                    record
                })
                .collect();

            Self { records }
        }
    }

    #[derive(Debug, Clone)]
    struct HistoryRecord {
        best_values: AlignedF64,
        random_prob: f64,
        greedy_prob: f64,
        action_type: usize,
        action_value: f64,
    }

    impl HistoryRecord {
        fn new(input: &Input, state: &GameState, player_index: usize, action: Cell) -> Self {
            let mut best_values = AlignedF64::ZERO;
            let mut best_count = [1; 4];
            let legal_actions = state.get_legal_moves(player_index);
            let random_prob = 1.0 / unsafe { legal_actions.popcount() } as f64;
            let mut zero_count = 0;

            for action in legal_actions.iter() {
                let territory = state.territories[action];
                let action_type = match Self::get_action_type(input, player_index, territory) {
                    Some(value) => value,
                    None => {
                        zero_count += 1;
                        continue;
                    }
                };

                if input.values_f64[action] >= best_values[action_type] {
                    if input.values_f64[action] == best_values[action_type] {
                        best_count[action_type] += 1;
                    } else {
                        best_values[action_type] = input.values_f64[action];
                        best_count[action_type] = 1;
                    }
                }
            }

            // executed_typeがNoneでもvalueを0と見なせばw*Vは破綻しない
            let action_type = Self::get_action_type(input, player_index, state.territories[action]);
            let (action_value, greedy_prob) = match action_type {
                Some(_) => (
                    input.values_f64[action],
                    1.0 / best_count[action_type.unwrap()] as f64,
                ),
                None => (0.0, 1.0 / zero_count as f64),
            };

            // 適当に0と見なして問題ない
            let action_type = action_type.unwrap_or(0);

            Self {
                best_values,
                random_prob,
                greedy_prob,
                action_type,
                action_value,
            }
        }

        fn get_action_type(
            input: &Input,
            player_index: usize,
            territory: Territory,
        ) -> Option<usize> {
            let t = match territory {
                Territory::Vacant => 0,
                Territory::Occupied(occupied) => {
                    if occupied.player() == player_index {
                        if occupied.level() < input.max_level {
                            1
                        } else {
                            return None;
                        }
                    } else {
                        if occupied.level() == 1 {
                            2
                        } else {
                            3
                        }
                    }
                }
            };

            Some(t)
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

        pub const fn in_map(&self, size: usize) -> bool {
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
mod problem {
    use crate::{grid::Coord, random::RandExtension, util::ChangeMinMax as _};
    use itertools::{izip, Itertools};
    use proconio::input_interactive;
    use rand::Rng;
    use std::{
        arch::x86_64::_pdep_u64,
        io::Write as _,
        num::NonZeroU8,
        ops::{BitAnd, BitAndAssign, BitOr, BitOrAssign, Index, IndexMut, Not},
        time::Instant,
    };

    #[derive(Debug, Clone)]
    pub struct Input {
        pub player_count: usize,
        pub max_level: usize,
        pub values: Board<u32>,
        pub values_f64: Board<f64>,
        pub init_positions: Vec<Cell>,
        pub std_dev_value: f64,
        pub since: Instant,
        values_set: Vec<BitBoard>,
        values_set_indices: Vec<u8>,
    }

    impl Input {
        pub const MAP_SIZE: usize = 10;
        pub const MAX_TURNS: usize = 100;
        pub const MAX_PLAYERS: usize = 8;
        pub const MAX_LEVEL: usize = 5;

        pub fn read() -> Self {
            input_interactive! {
                map_size: usize,
                player_count: usize,
                max_turn: usize,
                max_level: usize,
            }

            assert_eq!(map_size, Self::MAP_SIZE);
            assert_eq!(max_turn, Self::MAX_TURNS);

            let since = Instant::now();

            let mut values = Board::new(0);
            let mut values_distinct = vec![];

            for pos in 0..Input::MAP_SIZE * Input::MAP_SIZE {
                input_interactive! {
                    v: u32,
                }

                values[Cell::new(pos as u8)] = v;
                values_distinct.push(v);
            }

            let values_f64 = Board::from(values.values.map(|v| v as f64));

            // 値に対応する
            // 一応キャッシュにも気を使う
            values_distinct.sort_unstable();
            values_distinct.dedup();
            let max_value = values_distinct.last().cloned().unwrap();

            let mut values_set_indices =
                vec![values_distinct.len() as u8; (max_value + 1) as usize];
            let mut values_set = vec![BitBoard::NONE; values_distinct.len() + 1];

            for i in 0..Input::MAP_SIZE * Input::MAP_SIZE {
                let cell = Cell::new(i as u8);
                let v = values[cell];
                let index = values_distinct.binary_search(&v).unwrap() as u8;
                values_set[index as usize] |= cell.to_board();
                values_set_indices[v as usize] = index;
            }

            let mut init_positions = Vec::with_capacity(player_count);

            for _ in 0..player_count {
                input_interactive! {
                    r: usize,
                    c: usize,
                }

                init_positions.push(Cell::new((r * Input::MAP_SIZE + c) as u8));
            }

            let mut score_sum = 0.0;
            let mut score_sq_sum = 0.0;

            for &v in values.values.iter() {
                let v = v as f64;
                score_sum += v;
                score_sq_sum += v * v;
            }

            let mean = score_sum / (Input::MAP_SIZE * Input::MAP_SIZE) as f64;
            let variance = score_sq_sum / (Input::MAP_SIZE * Input::MAP_SIZE) as f64 - mean * mean;
            let std_dev_value = variance.sqrt();

            Self {
                player_count,
                max_level,
                values,
                values_f64,
                init_positions,
                std_dev_value,
                since,
                values_set,
                values_set_indices,
            }
        }

        pub fn get_init_game_state(&self) -> GameState {
            let mut territories = Board::new(Territory::Vacant);

            for (i, &pos) in self.init_positions.iter().enumerate() {
                territories[pos] =
                    Territory::Occupied(Occupied::new(i as u8, NonZeroU8::new(1).unwrap()));
            }

            GameState::new(
                self,
                territories,
                Self::to_player_array(&self.init_positions),
                0,
            )
        }

        pub fn get_value_set(&self, value: u32) -> BitBoard {
            let index = self.values_set_indices[value as usize];
            self.values_set[index as usize]
        }

        fn to_player_array(values: &[Cell]) -> [Cell; Input::MAX_PLAYERS] {
            assert!(values.len() <= Input::MAX_PLAYERS);
            let mut result = [Cell::default(); Input::MAX_PLAYERS];
            result[..values.len()].copy_from_slice(values);
            result
        }
    }

    #[derive(Debug, Clone)]
    pub struct Board<T: Copy> {
        values: [T; Input::MAP_SIZE * Input::MAP_SIZE],
    }

    impl<T: Copy> Board<T> {
        pub fn new(value: T) -> Self {
            Self {
                values: [value; Input::MAP_SIZE * Input::MAP_SIZE],
            }
        }
    }

    impl<T: Copy> Index<Cell> for Board<T> {
        type Output = T;

        fn index(&self, cell: Cell) -> &Self::Output {
            &self.values[cell.index as usize]
        }
    }

    impl<T: Copy> IndexMut<Cell> for Board<T> {
        fn index_mut(&mut self, cell: Cell) -> &mut Self::Output {
            &mut self.values[cell.index as usize]
        }
    }

    impl<T: Copy> From<[T; Input::MAP_SIZE * Input::MAP_SIZE]> for Board<T> {
        fn from(values: [T; Input::MAP_SIZE * Input::MAP_SIZE]) -> Self {
            Self { values }
        }
    }

    #[derive(Clone, Copy, Debug, PartialEq, Eq, Default, Hash)]
    pub struct BitBoard {
        bits: u128,
    }

    impl BitBoard {
        pub const NONE: Self = Self::new(0);
        const NOT_RIGHT: Self =
        Self::new(0b0111111111011111111101111111110111111111011111111101111111110111111111011111111101111111110111111111);
        const NOT_LEFT: Self =
        Self::new(0b1111111110111111111011111111101111111110111111111011111111101111111110111111111011111111101111111110);
        pub const IN_BOARD: Self =
        Self::new(0b1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111);

        pub const fn new(bits: u128) -> Self {
            Self { bits }
        }

        pub const fn get(&self, index: Cell) -> bool {
            ((self.bits >> index.index) as u8) & 1 != 0
        }

        pub const fn set(&self, index: Cell) -> Self {
            Self::new(self.bits | (1 << index.index))
        }

        #[allow(dead_code)]
        pub const fn clear(&self, index: Cell) -> Self {
            Self::new(self.bits & !(1 << index.index))
        }

        #[target_feature(enable = "popcnt")]
        pub unsafe fn popcount(&self) -> u32 {
            self.bits.count_ones()
        }

        pub fn bfs(&self, mask: &BitBoard) -> Self {
            let mut visited = *self;

            loop {
                let next = visited.bfs_step(mask);

                if next == visited {
                    return visited;
                }

                visited = next;
            }
        }

        pub fn bfs_step(&self, mask: &BitBoard) -> Self {
            let mut next = *self;
            next |= BitBoard::new(self.bits << 1) & Self::NOT_LEFT;
            next |= BitBoard::new(self.bits >> 1) & Self::NOT_RIGHT;
            next |= BitBoard::new(self.bits << Input::MAP_SIZE) & Self::IN_BOARD;
            next |= BitBoard::new(self.bits >> Input::MAP_SIZE) & Self::IN_BOARD;
            next &= *mask;
            next
        }

        pub fn bits(&self) -> u128 {
            self.bits
        }

        pub fn iter(&self) -> BitBoardIter {
            BitBoardIter::new(*self)
        }

        /// ビットボードから一様ランダムに1つのセルを選ぶ
        #[target_feature(enable = "bmi1,bmi2,popcnt")]
        pub unsafe fn choose(&self, rng: &mut impl Rng) -> Option<Cell> {
            let lower = self.bits as u64;
            let upper = (self.bits >> 64) as u64;
            let lower_popcnt = lower.count_ones();
            let upper_popcnt = upper.count_ones();
            let cnt = lower_popcnt + upper_popcnt;

            if cnt == 0 {
                return None;
            }

            let idx = rng.fast_gen_range_u16x1(0..cnt);

            // pdep命令を使って、idx番目の1ビットを立てる
            if lower_popcnt > idx {
                let flag = 1 << idx;
                let flag = _pdep_u64(flag, lower);
                let index = flag.trailing_zeros() as u8;
                return Some(Cell::new(index));
            } else {
                let flag = 1 << (idx - lower_popcnt);
                let flag = _pdep_u64(flag, upper);
                let index = flag.trailing_zeros() as u8 + 64;
                return Some(Cell::new(index));
            }
        }

        /// ビットボードからindex番目のセルを選ぶ
        #[target_feature(enable = "bmi1,bmi2,popcnt")]
        #[allow(dead_code)]
        pub unsafe fn select(&self, index: u32) -> Cell {
            let lower = self.bits as u64;
            let upper = (self.bits >> 64) as u64;
            let lower_popcnt = lower.count_ones();
            let upper_popcnt = upper.count_ones();
            let cnt = lower_popcnt + upper_popcnt;

            assert!(index < cnt, "index out of bounds");

            // pdep命令を使って、idx番目の1ビットを立てる
            if lower_popcnt > index {
                let flag = 1 << index;
                let flag = _pdep_u64(flag, lower);
                let index = flag.trailing_zeros() as u8;
                return Cell::new(index);
            } else {
                let flag = 1 << (index - lower_popcnt);
                let flag = _pdep_u64(flag, upper);
                let index = flag.trailing_zeros() as u8 + 64;
                return Cell::new(index);
            }
        }
    }

    impl BitAnd for BitBoard {
        type Output = Self;

        fn bitand(self, rhs: Self) -> Self::Output {
            Self::new(self.bits & rhs.bits)
        }
    }

    impl BitAndAssign for BitBoard {
        fn bitand_assign(&mut self, rhs: Self) {
            self.bits &= rhs.bits;
        }
    }

    impl BitOr for BitBoard {
        type Output = Self;

        fn bitor(self, rhs: Self) -> Self::Output {
            Self::new(self.bits | rhs.bits)
        }
    }

    impl BitOrAssign for BitBoard {
        fn bitor_assign(&mut self, rhs: Self) {
            self.bits |= rhs.bits;
        }
    }

    impl Not for BitBoard {
        type Output = Self;

        fn not(self) -> Self::Output {
            Self::new(!self.bits)
        }
    }

    pub struct BitBoardIter {
        lower: u64,
        upper: u64,
    }

    impl BitBoardIter {
        fn new(bitboard: BitBoard) -> Self {
            Self {
                lower: bitboard.bits as u64,
                upper: (bitboard.bits >> 64) as u64,
            }
        }
    }

    impl Iterator for BitBoardIter {
        type Item = Cell;

        fn next(&mut self) -> Option<Self::Item> {
            if self.lower != 0 {
                let pos = self.lower.trailing_zeros() as u8;
                self.lower &= self.lower - 1;
                return Some(Cell::new(pos));
            }

            if self.upper != 0 {
                let pos = self.upper.trailing_zeros() as u8;
                self.upper &= self.upper - 1;
                return Some(Cell::new(pos + 64));
            }

            None
        }
    }

    #[derive(Clone, Copy, Debug, PartialEq, Eq, Default, Hash)]
    pub struct Cell {
        index: u8,
    }

    impl Cell {
        pub fn new(index: impl Into<u64>) -> Self {
            let index = index.into() as u8;
            assert!(index < (Input::MAP_SIZE * Input::MAP_SIZE) as u8);

            Self { index }
        }

        pub fn to_board(&self) -> BitBoard {
            BitBoard::new(1 << self.index)
        }

        pub fn index(&self) -> usize {
            self.index as usize
        }
    }

    impl Into<Coord> for Cell {
        fn into(self) -> Coord {
            let r = self.index as usize / Input::MAP_SIZE;
            let c = self.index as usize % Input::MAP_SIZE;
            Coord::new(r, c)
        }
    }

    impl From<Coord> for Cell {
        fn from(coord: Coord) -> Self {
            let index = (coord.row() * Input::MAP_SIZE + coord.col()) as u8;
            Self { index }
        }
    }

    #[derive(Debug, Clone)]
    pub struct GameState {
        pub player_count: usize,
        pub territories: Board<Territory>,
        pub territory_bits: [BitBoard; Input::MAX_PLAYERS],
        pub all_territory_bits: BitBoard,
        pub all_position_bits: BitBoard,
        pub max_level_bits: BitBoard,
        pub level_one_bits: BitBoard,
        pub scores: [u32; Input::MAX_PLAYERS],
        pub positions: [Cell; Input::MAX_PLAYERS],
        pub turn: usize,
    }

    impl GameState {
        pub fn new(
            input: &Input,
            territories: Board<Territory>,
            positions: [Cell; Input::MAX_PLAYERS],
            turn: usize,
        ) -> Self {
            let mut scores = [0; Input::MAX_PLAYERS];
            let mut territory_bits = [BitBoard::NONE; Input::MAX_PLAYERS];
            let mut max_level_bits = BitBoard::NONE;
            let mut level_one_bits = BitBoard::NONE;

            for (cell, territory) in territories.values.iter().enumerate() {
                if let Territory::Occupied(occupied) = territory {
                    let cell = Cell::new(cell as u8);
                    let cell_board = cell.to_board();
                    scores[occupied.player()] += input.values[cell] * occupied.level() as u32;
                    territory_bits[occupied.player()] |= cell_board;

                    if occupied.level() == input.max_level {
                        max_level_bits |= cell_board;
                    }

                    if occupied.level() == 1 {
                        level_one_bits |= cell_board;
                    }
                }
            }

            let all_territory_bits = territory_bits[..input.player_count]
                .iter()
                .fold(BitBoard::NONE, |acc, &b| acc | b);
            let all_position_bits = positions[..input.player_count]
                .iter()
                .fold(BitBoard::NONE, |acc, &p| acc | p.to_board());

            Self {
                player_count: input.player_count,
                territories,
                territory_bits,
                all_territory_bits,
                all_position_bits,
                max_level_bits,
                level_one_bits,
                scores,
                positions,
                turn,
            }
        }

        pub fn case_score(&self) -> u32 {
            let my_score = self.scores[0] as f64;
            let opponent_best_score =
                *self.scores[1..self.player_count].iter().max().unwrap() as f64;
            (1e5 * (1.0 + (my_score / opponent_best_score)).log2()).round() as u32
        }

        pub fn get_legal_moves(&self, player: usize) -> BitBoard {
            let player_position = self.positions[player].to_board();
            let my_territory = &self.territory_bits[player];
            let other_players = self.all_position_bits & !player_position;
            let connected = player_position.bfs(my_territory);
            let legal_moves = connected.bfs_step(&!other_players);
            legal_moves
        }

        pub fn apply_actions(&mut self, input: &Input, actions: &[Cell]) {
            // 競合解決
            let mut removed = self.resolve_conflict(actions);

            // 領土の更新
            self.update_territories(input, actions, &mut removed);

            // 位置更新
            self.all_position_bits = BitBoard::NONE;
            for i in 0..self.player_count {
                if !removed[i] {
                    self.positions[i] = actions[i];
                }

                self.all_position_bits |= self.positions[i].to_board();
            }

            self.turn += 1;
        }

        fn resolve_conflict(&self, actions: &[Cell]) -> [bool; 8] {
            let mut suffix_or = [BitBoard::NONE; 9];

            for i in (1..actions.len()).rev() {
                suffix_or[i] = suffix_or[i + 1] | actions[i].to_board();
            }

            let mut prefix_or = BitBoard::NONE;
            let mut removed = [false; 8];

            for (i, (action, my_territory, removed)) in
                izip!(actions, &self.territory_bits, &mut removed).enumerate()
            {
                // 自分以外のプレイヤーの行動と被っており、目的地が自分の領土でなければキャンセル
                let others = prefix_or | suffix_or[i + 1];

                if others.get(*action) && !my_territory.get(*action) {
                    *removed = true;
                }

                prefix_or |= action.to_board();
            }

            removed
        }

        fn update_territories(&mut self, input: &Input, actions: &[Cell], removed: &mut [bool]) {
            for (i, (&cell, removed)) in izip!(actions, removed).enumerate() {
                if *removed {
                    continue;
                }

                match self.territories[cell] {
                    // 占領
                    Territory::Vacant => {
                        self.scores[i] += input.values[cell];
                        let cell_bit = cell.to_board();

                        self.territories[cell] =
                            Territory::Occupied(Occupied::new(i as u8, NonZeroU8::new(1).unwrap()));
                        self.territory_bits[i] |= cell_bit;
                        self.all_territory_bits |= cell_bit;

                        self.level_one_bits |= cell_bit;

                        if input.max_level == 1 {
                            self.max_level_bits |= cell_bit;
                        }
                    }
                    // 強化
                    Territory::Occupied(occupied) if occupied.player() == i => {
                        if occupied.level() == input.max_level {
                            continue;
                        }

                        let new_level = occupied.level() + 1;
                        let cell_bit = cell.to_board();
                        self.scores[i] += input.values[cell];
                        self.territories[cell] = Territory::Occupied(Occupied::new(
                            i as u8,
                            NonZeroU8::new(new_level as u8).unwrap(),
                        ));

                        // レベル1 -> 2 への強化時は、level_one_bits から外す
                        if occupied.level() == 1 {
                            self.level_one_bits &= !cell_bit;
                        }

                        if new_level == input.max_level {
                            self.max_level_bits |= cell_bit;
                        }
                    }
                    // 攻撃
                    Territory::Occupied(occupied) => {
                        let opponent = occupied.player();
                        assert_ne!(opponent, i);

                        self.scores[opponent] -= input.values[cell];
                        let cell_bit = cell.to_board();

                        // レベル1の領土を奪った場合、新しくレベル1の領土ができるためlevel_one_bitsを削除する必要はない
                        // 同様に、max_levelが1の場合、奪ったことでmax_levelの領土ができるため、max_level_bitsを削除する必要はない
                        if occupied.level() == input.max_level && input.max_level > 1 {
                            self.max_level_bits &= !cell_bit;
                        }

                        if occupied.level() == 1 {
                            self.scores[i] += input.values[cell];

                            self.territories[cell] = Territory::Occupied(Occupied::new(
                                i as u8,
                                NonZeroU8::new(1).unwrap(),
                            ));
                            self.territory_bits[opponent] &= !cell_bit;
                            self.territory_bits[i] |= cell_bit;
                            self.level_one_bits |= cell_bit;
                        } else {
                            if occupied.level() - 1 == 1 {
                                self.level_one_bits |= cell_bit;
                            }

                            self.territories[cell] = Territory::Occupied(Occupied::new(
                                opponent as u8,
                                NonZeroU8::new(occupied.level() as u8 - 1).unwrap(),
                            ));
                            *removed = true;
                        }
                    }
                }
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, Default)]
    pub enum Territory {
        #[default]
        Vacant,
        Occupied(Occupied),
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
    pub struct Occupied {
        pub player: u8,
        pub level: NonZeroU8,
    }

    impl Occupied {
        pub fn new(player: u8, level: NonZeroU8) -> Self {
            Self { player, level }
        }

        pub fn player(&self) -> usize {
            self.player as usize
        }

        pub fn level(&self) -> usize {
            self.level.get() as usize
        }
    }

    #[derive(Debug, Clone, Copy, Default)]
    pub struct AI {
        pub weights: [f64; 4],
        pub eps: f64,
        pub player_index: usize,
    }

    impl AI {
        pub const MIN_WEIGHT: f64 = 0.3;
        pub const MAX_WEIGHT: f64 = 1.0;
        pub const MIN_EPS: f64 = 0.1;
        pub const MAX_EPS: f64 = 0.5;
        pub const DUMMY: Self = Self {
            weights: [0.0; 4],
            eps: 0.0,
            player_index: 0,
        };

        pub fn new(player_index: usize, weights: [f64; 4], eps: f64) -> Self {
            Self {
                player_index,
                weights,
                eps,
            }
        }

        pub fn new_random(player_index: usize, rng: &mut impl Rng) -> Self {
            let weights = [
                rng.random_range(Self::MIN_WEIGHT..=Self::MAX_WEIGHT),
                rng.random_range(Self::MIN_WEIGHT..=Self::MAX_WEIGHT),
                rng.random_range(Self::MIN_WEIGHT..=Self::MAX_WEIGHT),
                rng.random_range(Self::MIN_WEIGHT..=Self::MAX_WEIGHT),
            ];
            let eps = rng.random_range(Self::MIN_EPS..=Self::MAX_EPS);
            Self::new(player_index, weights, eps)
        }

        #[target_feature(enable = "bmi1,bmi2,popcnt")]
        pub unsafe fn choose_eps_greedy(
            &self,
            input: &Input,
            state: &GameState,
            legal_moves: BitBoard,
            rng: &mut impl Rng,
        ) -> Cell {
            if rng.random_bool(self.eps) {
                unsafe { legal_moves.choose(rng).unwrap() }
            } else {
                let candidates = unsafe { self.get_greedy_candidates(input, state, &legal_moves) };
                unsafe { candidates.choose(rng) }
            }
        }

        #[target_feature(enable = "bmi1,bmi2,popcnt")]
        pub unsafe fn get_greedy_candidates(
            &self,
            input: &Input,
            state: &GameState,
            legal_moves: &BitBoard,
        ) -> ActionCandidates {
            let mut best_set = BitBoard::NONE;
            let mut best_eval = f64::NEG_INFINITY;
            let my_territory = state.territory_bits[self.player_index];
            let mut remaining = *legal_moves;

            // 無所属
            let white = remaining & !state.all_territory_bits;
            self.update_eval(
                input,
                &mut remaining,
                &white,
                self.weights[0],
                &mut best_set,
                &mut best_eval,
            );

            let mut opponent_bits = remaining & !my_territory;
            remaining &= !opponent_bits;

            // 他マスlevel1
            let other_1 = opponent_bits & state.level_one_bits;
            self.update_eval(
                input,
                &mut opponent_bits,
                &other_1,
                self.weights[2],
                &mut best_set,
                &mut best_eval,
            );

            // 他マスその他
            let other = opponent_bits;
            self.update_eval(
                input,
                &mut opponent_bits,
                &other,
                self.weights[3],
                &mut best_set,
                &mut best_eval,
            );

            // 自マスmax以外
            let my_1 = remaining & !state.max_level_bits;
            self.update_eval(
                input,
                &mut remaining,
                &my_1,
                self.weights[1],
                &mut best_set,
                &mut best_eval,
            );

            // 自マスmax
            if best_eval < 0.0 {
                best_set = remaining;
            }

            let popcnt = unsafe { best_set.popcount() };
            assert!(popcnt > 0);

            if popcnt == 1 {
                ActionCandidates::Single(Cell::new(best_set.bits().trailing_zeros() as u8))
            } else {
                ActionCandidates::Multiple(best_set)
            }
        }

        fn update_eval(
            &self,
            input: &Input,
            board: &mut BitBoard,
            targets: &BitBoard,
            coef: f64,
            best_set: &mut BitBoard,
            best_eval: &mut f64,
        ) {
            if targets != &BitBoard::NONE {
                let mut max_value = 0;

                for cell in targets.iter() {
                    let value = input.values[cell];
                    max_value.change_max(value);
                }

                if best_eval.change_max(max_value as f64 * coef) {
                    *best_set = input.get_value_set(max_value) & *targets;
                }
            }

            *board &= !*targets;
        }
    }

    #[derive(Debug, Clone)]
    pub enum ActionCandidates {
        Single(Cell),
        Multiple(BitBoard),
    }

    impl ActionCandidates {
        #[target_feature(enable = "bmi1,bmi2,popcnt")]
        pub unsafe fn choose(&self, rng: &mut impl Rng) -> Cell {
            match self {
                ActionCandidates::Single(cell) => *cell,
                ActionCandidates::Multiple(bit_board) => unsafe { bit_board.choose(rng).unwrap() },
            }
        }
    }

    #[allow(dead_code)]
    pub trait Judge {
        fn submit_action(&mut self, input: &Input, action: Cell) -> Response;
        fn try_get_ai_params(&self) -> Option<Vec<AI>>;
    }

    #[allow(dead_code)]
    pub struct ActualJudge {
        turn: usize,
    }

    #[allow(dead_code)]
    impl ActualJudge {
        pub fn new() -> Self {
            Self { turn: 0 }
        }
    }

    impl Judge for ActualJudge {
        fn submit_action(&mut self, input: &Input, action: Cell) -> Response {
            let c: Coord = action.into();
            println!("{} {}", c.row(), c.col());
            std::io::stdout().flush().unwrap();

            let mut actions = Vec::with_capacity(input.player_count);

            for _ in 0..input.player_count {
                input_interactive! {
                    r: usize,
                    c: usize,
                }

                actions.push(Cell::from(Coord::new(r, c)));
            }

            let mut positions = [Cell::default(); Input::MAX_PLAYERS];
            for position in positions.iter_mut().take(input.player_count) {
                input_interactive! {
                    r: usize,
                    c: usize,
                }

                *position = Cell::from(Coord::new(r, c));
            }

            let mut owners = Vec::with_capacity(Input::MAP_SIZE * Input::MAP_SIZE);

            for _ in 0..(Input::MAP_SIZE * Input::MAP_SIZE) {
                input_interactive! {
                    owner: isize,
                }

                if owner == -1 {
                    owners.push(None);
                } else {
                    owners.push(Some(owner as usize));
                }
            }

            let mut levels = Vec::with_capacity(Input::MAP_SIZE * Input::MAP_SIZE);

            for _ in 0..(Input::MAP_SIZE * Input::MAP_SIZE) {
                input_interactive! {
                    level: usize,
                }

                levels.push(level);
            }

            let mut territories = Board::new(Territory::Vacant);

            for index in 0..Input::MAP_SIZE * Input::MAP_SIZE {
                let cell = Cell::new(index as u8);

                let owner = owners[index];
                let level = levels[index];

                let t = match owner {
                    Some(owner) => Territory::Occupied(Occupied::new(
                        owner as u8,
                        NonZeroU8::new(level as u8).unwrap(),
                    )),
                    None => Territory::Vacant,
                };

                territories[cell] = t;
            }

            self.turn += 1;

            let game_state = GameState::new(input, territories, positions, self.turn);

            Response {
                actions,
                game_state,
            }
        }

        fn try_get_ai_params(&self) -> Option<Vec<AI>> {
            None
        }
    }

    #[allow(dead_code)]
    pub struct MockJudge {
        game_state: GameState,
        ai_params: Vec<AI>,
        random_values: Vec<Vec<(f64, f64)>>,
        print_final_score: bool,
    }

    #[allow(dead_code)]
    impl MockJudge {
        pub fn read(input: &Input, print_final_score: bool) -> Self {
            let mut ai_params = Vec::with_capacity(input.player_count);
            ai_params.push(AI::new(0, [0.0; 4], 0.0));

            for player_index in 1..input.player_count {
                input_interactive! {
                    wa: f64,
                    wb: f64,
                    wc: f64,
                    wd: f64,
                    eps: f64,
                }

                ai_params.push(AI::new(player_index, [wa, wb, wc, wd], eps));
            }

            let mut random_values = vec![vec![(0.0, 0.0); input.player_count]; Input::MAX_TURNS];

            for turn in 0..Input::MAX_TURNS {
                for player_index in 1..input.player_count {
                    input_interactive! {
                        r1: f64,
                        r2: f64,
                    }

                    random_values[turn][player_index] = (r1, r2);
                }
            }

            Self {
                game_state: input.get_init_game_state(),
                ai_params,
                random_values,
                print_final_score,
            }
        }

        fn choose_ai_action(&self, input: &Input, player_index: usize) -> Cell {
            let ai = &self.ai_params[player_index];
            let turn = self.game_state.turn;
            let (r1, r2) = self.random_values[turn][player_index];
            let candidates = self.get_candidates(player_index);
            let mut scores = Vec::with_capacity(candidates.len());

            for &cell in &candidates {
                scores.push(self.calc_eval(input, ai, player_index, cell));
            }

            if r1 < ai.eps {
                let idx =
                    ((r2 * candidates.len() as f64).floor() as usize).min(candidates.len() - 1);
                return candidates[idx];
            }

            let max_score = scores.iter().copied().fold(f64::NEG_INFINITY, f64::max);
            let tolerance = 1e-9 * max_score.abs().max(1.0);
            let best_indices = (0..scores.len())
                .filter(|&i| scores[i] >= max_score - tolerance)
                .collect_vec();
            let idx =
                ((r2 * best_indices.len() as f64).floor() as usize).min(best_indices.len() - 1);
            candidates[best_indices[idx]]
        }

        fn calc_eval(&self, input: &Input, ai: &AI, player_index: usize, cell: Cell) -> f64 {
            let value = input.values_f64[cell];

            match self.game_state.territories[cell] {
                Territory::Vacant => value * ai.weights[0],
                Territory::Occupied(occupied) if occupied.player() == player_index => {
                    if occupied.level() < input.max_level {
                        value * ai.weights[1]
                    } else {
                        0.0
                    }
                }
                Territory::Occupied(occupied) => {
                    if occupied.level() == 1 {
                        value * ai.weights[2]
                    } else {
                        value * ai.weights[3]
                    }
                }
            }
        }

        fn get_candidates(&self, player_index: usize) -> Vec<Cell> {
            let start = self.game_state.positions[player_index];
            let mut visited = BitBoard::NONE.set(start);
            let mut queue = std::collections::VecDeque::new();
            let mut reachable = Vec::new();
            queue.push_back(start);

            while let Some(cell) = queue.pop_front() {
                if !self.is_occupied_by_other_player(cell, player_index) {
                    reachable.push(cell);
                }

                if !self.is_owned_by_player(cell, player_index) {
                    continue;
                }

                for next in self.neighbors(cell) {
                    if !visited.get(next) {
                        visited = visited.set(next);
                        queue.push_back(next);
                    }
                }
            }

            reachable
        }

        fn is_owned_by_player(&self, cell: Cell, player_index: usize) -> bool {
            matches!(
                self.game_state.territories[cell],
                Territory::Occupied(occupied) if occupied.player() == player_index
            )
        }

        fn is_occupied_by_other_player(&self, cell: Cell, player_index: usize) -> bool {
            self.game_state
                .positions
                .iter()
                .take(self.game_state.player_count)
                .enumerate()
                .any(|(i, &pos)| i != player_index && pos == cell)
        }

        fn neighbors(&self, cell: Cell) -> Vec<Cell> {
            let coord: Coord = cell.into();
            let row = coord.row();
            let col = coord.col();
            let n = Input::MAP_SIZE;
            let mut neighbors = Vec::with_capacity(4);

            if col + 1 < n {
                neighbors.push(Cell::from(Coord::new(row, col + 1)));
            }
            if row + 1 < n {
                neighbors.push(Cell::from(Coord::new(row + 1, col)));
            }
            if col > 0 {
                neighbors.push(Cell::from(Coord::new(row, col - 1)));
            }
            if row > 0 {
                neighbors.push(Cell::from(Coord::new(row - 1, col)));
            }

            neighbors
        }
    }

    impl Judge for MockJudge {
        fn submit_action(&mut self, input: &Input, action: Cell) -> Response {
            let c: Coord = action.into();
            println!("{} {}", c.row(), c.col());

            let mut actions = Vec::with_capacity(input.player_count);
            actions.push(action);

            for player_index in 1..input.player_count {
                actions.push(self.choose_ai_action(input, player_index));
            }

            self.game_state.apply_actions(input, &actions);

            if self.game_state.turn == Input::MAX_TURNS && self.print_final_score {
                eprintln!("Score = {}", self.game_state.case_score());
            }

            Response {
                actions,
                game_state: self.game_state.clone(),
            }
        }

        fn try_get_ai_params(&self) -> Option<Vec<AI>> {
            Some(self.ai_params.clone())
        }
    }

    #[derive(Debug, Clone)]
    pub struct Response {
        pub actions: Vec<Cell>,
        pub game_state: GameState,
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
mod simd {
    use std::{
        arch::x86_64::*,
        ops::{Index, IndexMut},
    };

    pub(super) const AVX2_F64_W: usize = 4;
    pub(super) const AVX512_U32_W: usize = 16;
    pub(super) const AVX512_F32_W: usize = 16;

    /// 32byte境界にアライン
    #[repr(align(32))]
    #[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
    pub(super) struct AlignedF64(pub [f64; AVX2_F64_W]);

    impl AlignedF64 {
        pub const ZERO: Self = Self([0.0; AVX2_F64_W]);

        pub fn load(&self) -> __m256d {
            unsafe { _mm256_load_pd(self.0.as_ptr() as *const f64) }
        }

        pub fn store(&mut self, x: __m256d) {
            unsafe { _mm256_store_pd(self.0.as_mut_ptr() as *mut f64, x) }
        }
    }

    impl From<__m256d> for AlignedF64 {
        fn from(x: __m256d) -> Self {
            let mut ret = Self([0.0; AVX2_F64_W]);
            ret.store(x);
            ret
        }
    }

    impl Into<__m256d> for AlignedF64 {
        fn into(self) -> __m256d {
            self.load()
        }
    }

    impl Index<usize> for AlignedF64 {
        type Output = f64;

        fn index(&self, index: usize) -> &Self::Output {
            &self.0[index]
        }
    }

    impl IndexMut<usize> for AlignedF64 {
        fn index_mut(&mut self, index: usize) -> &mut Self::Output {
            &mut self.0[index]
        }
    }

    /// 64byte境界にアライン
    #[repr(align(64))]
    #[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
    pub(super) struct AlignedU32x16(pub [u32; AVX512_U32_W]);

    impl AlignedU32x16 {
        #[allow(dead_code)]
        pub const ZERO: Self = Self([0; AVX512_U32_W]);

        pub fn load(&self) -> __m512i {
            unsafe { _mm512_load_si512(self.0.as_ptr() as *const __m512i) }
        }

        pub fn store(&mut self, x: __m512i) {
            unsafe { _mm512_store_si512(self.0.as_mut_ptr() as *mut __m512i, x) }
        }
    }

    impl From<__m512i> for AlignedU32x16 {
        fn from(x: __m512i) -> Self {
            let mut ret = Self([0; AVX512_U32_W]);
            ret.store(x);
            ret
        }
    }

    impl Into<__m512i> for AlignedU32x16 {
        fn into(self) -> __m512i {
            self.load()
        }
    }

    impl Index<usize> for AlignedU32x16 {
        type Output = u32;

        fn index(&self, index: usize) -> &Self::Output {
            &self.0[index]
        }
    }

    impl IndexMut<usize> for AlignedU32x16 {
        fn index_mut(&mut self, index: usize) -> &mut Self::Output {
            &mut self.0[index]
        }
    }

    /// 64byte境界にアライン
    #[repr(align(64))]
    #[derive(Debug, Clone, Copy, Default, PartialEq, PartialOrd)]
    pub(super) struct AlignedF32x16(pub [f32; AVX512_F32_W]);

    impl AlignedF32x16 {
        pub const ZERO: Self = Self([0.0; AVX512_F32_W]);

        pub fn load(&self) -> __m512 {
            unsafe { _mm512_load_ps(self.0.as_ptr() as *const f32) }
        }

        pub fn store(&mut self, x: __m512) {
            unsafe { _mm512_store_ps(self.0.as_mut_ptr() as *mut f32, x) }
        }
    }

    impl From<__m512> for AlignedF32x16 {
        fn from(x: __m512) -> Self {
            let mut ret = Self([0.0; AVX512_F32_W]);
            ret.store(x);
            ret
        }
    }

    impl Into<__m512> for AlignedF32x16 {
        fn into(self) -> __m512 {
            self.load()
        }
    }

    impl Index<usize> for AlignedF32x16 {
        type Output = f32;

        fn index(&self, index: usize) -> &Self::Output {
            &self.0[index]
        }
    }

    impl IndexMut<usize> for AlignedF32x16 {
        fn index_mut(&mut self, index: usize) -> &mut Self::Output {
            &mut self.0[index]
        }
    }

    /// 16-lane xorshift32* (AVX-512)
    /// - state: 16×u32
    /// - next_u32x16: 16個の乱数(u32)を __m512i で返す
    /// - next_f32x16: 16個の float を [0,1) 近似で返す
    #[derive(Clone, Copy)]
    pub struct XorShift32StarAvx512 {
        state: __m512i,
    }

    impl XorShift32StarAvx512 {
        /// 16 レーン分の初期状態を splitmix64 で作る
        pub fn new(seed: u64) -> Self {
            let mut s = [0u32; 16];
            let mut x = seed;

            for i in 0..16 {
                let v = Self::splitmix64(&mut x);
                let mut u = v as u32;
                if u == 0 {
                    // 0 は吸収状態になり得るので回避
                    u = 0x6D2B79F5;
                }
                s[i] = u;
            }

            unsafe {
                // unaligned load でもコストは重くない
                let state = _mm512_loadu_si512(s.as_ptr() as *const _);
                Self { state }
            }
        }

        /// splitmix64: シード展開用（スカラで十分）
        fn splitmix64(state: &mut u64) -> u64 {
            let mut z = {
                *state = state.wrapping_add(0x9E3779B97F4A7C15);
                *state
            };
            z = (z ^ (z >> 30)).wrapping_mul(0xBF58476D1CE4E5B9);
            z = (z ^ (z >> 27)).wrapping_mul(0x94D049BB133111EB);
            z ^ (z >> 31)
        }

        /// 16×u32 の乱数を返す
        #[target_feature(enable = "avx512f")]
        pub unsafe fn next_u32x16(&mut self) -> __m512i {
            let mut x = self.state;
            x = _mm512_xor_si512(x, _mm512_slli_epi32(x, 13));
            x = _mm512_xor_si512(x, _mm512_srli_epi32(x, 17));
            x = _mm512_xor_si512(x, _mm512_slli_epi32(x, 5));
            self.state = x;

            let mul = _mm512_set1_epi32(0x9E3779BBu32 as i32);
            _mm512_mullo_epi32(x, mul)
        }

        /// 16×f32 を [0,1) 近似で返す（上位23bitを仮数へ）
        /// 正確な一様ではないが高速。正規近似などの用途向け。
        #[target_feature(enable = "avx512f")]
        pub unsafe fn next_f32x16(&mut self) -> __m512 {
            unsafe {
                let r = self.next_u32x16();

                // hi24 = r >> 8  (上位24bitを取り出す)
                let hi24 = _mm512_srli_epi32(r, 8);

                // u32 -> f32 変換（0..2^24-1なので正確に表現可能）
                let f = _mm512_cvtepi32_ps(hi24);

                // scale = 1 / 2^24
                let scale = _mm512_set1_ps(1.0f32 / 16777216.0f32);
                _mm512_mul_ps(f, scale) // [0,1)
            }
        }
    }
}
mod solver {
    pub mod main_solver {
        mod ai_estimation {
            use crate::{
                diagnostics::Perf,
                estimator::{run_mcmc, ActionHistory},
                problem::{Input, AI},
            };

            pub(super) fn should_update_estimation(turn: usize) -> bool {
                if turn < 30 {
                    true
                } else if turn < 40 {
                    turn % 2 == 0
                } else if turn < 50 {
                    turn % 5 == 0
                } else {
                    turn % 10 == 0
                }
            }

            pub(super) fn update_estimated_ais(
                input: &Input,
                history: &ActionHistory,
                estimated_ais: &mut Vec<Vec<AI>>,
            ) {
                let perf = Perf::start_singleton("estimate");
                for player_id in 1..input.player_count {
                    let new_estimated_ais =
                        run_mcmc(history, player_id, &estimated_ais[player_id], 100, 100, 20);
                    estimated_ais[player_id] = new_estimated_ais;
                }
                perf.stop();
            }
        }
        mod articulation {
            use crate::{
                grid::{Coord, CoordDiff, ADJACENTS},
                problem::{BitBoard, Board, Cell, Input},
            };
            use std::arch::x86_64::*;

            pub struct ConnectionCheckerBitBoard {
                checker: ConnectionChecker,
                board_pext: Board<BoardPext>,
            }

            impl ConnectionCheckerBitBoard {
                pub fn new() -> Self {
                    let checker = ConnectionChecker::new();
                    let mut board_pext = Board::new(BoardPext::default());

                    for cell in 0..Input::MAP_SIZE * Input::MAP_SIZE {
                        let cell = Cell::new(cell as u64);
                        board_pext[cell] = BoardPext::new(cell);
                    }

                    Self {
                        checker,
                        board_pext,
                    }
                }

                #[target_feature(enable = "bmi2")]
                pub unsafe fn can_remove(&self, board: &BitBoard, cell: Cell) -> bool {
                    unsafe {
                        let flag = self.board_pext[cell].get_flag(board);
                        self.checker.dict[flag]
                    }
                }

                #[target_feature(enable = "bmi2")]
                pub unsafe fn is_articulation_point(&self, board: &BitBoard, cell: Cell) -> bool {
                    unsafe { !self.can_remove(board, cell) }
                }
            }

            #[derive(Debug, Clone, Copy, Default)]
            struct BoardPext {
                mask_board_lower: u64,
                mask_board_upper: u64,
                mask_lower_popcnt: u32,
                pdep_mask: u64,
            }

            impl BoardPext {
                fn new(cell: Cell) -> Self {
                    let c: Coord = cell.into();
                    let mut mask_board_lower = 0;
                    let mut mask_board_upper = 0;
                    let mut mask_lower_popcnt = 0;
                    let mut pdep_mask = 0;
                    let mut i = 0u32;

                    for dr in -1..=1 {
                        for dc in -1..=1 {
                            let c = c + CoordDiff::new(dr, dc);

                            if c.in_map(Input::MAP_SIZE) {
                                let index = Cell::from(c).index();
                                pdep_mask |= 1 << i;

                                if index < 64 {
                                    mask_board_lower |= 1 << index;
                                    mask_lower_popcnt += 1;
                                } else {
                                    mask_board_upper |= 1 << (index - 64);
                                }
                            }

                            i += 1;
                        }
                    }

                    Self {
                        mask_board_lower,
                        mask_board_upper,
                        mask_lower_popcnt,
                        pdep_mask,
                    }
                }

                #[target_feature(enable = "bmi2")]
                unsafe fn get_flag(&self, board: &BitBoard) -> usize {
                    let lower = board.bits() as u64;
                    let upper = (board.bits() >> 64) as u64;

                    // 盤面外は無視し、実在マスだけを抜き取る
                    let pext_lower = _pext_u64(lower, self.mask_board_lower);
                    let pext_upper = _pext_u64(upper, self.mask_board_upper);
                    let pext = (pext_upper << self.mask_lower_popcnt) | pext_lower;

                    // 実在マスだけ配置する（盤面外はpdepにより0となる）
                    let flag = _pdep_u64(pext, self.pdep_mask);
                    flag as usize
                }
            }

            /// 3x3領域のみを見て簡易的に関節点判定を行う構造体
            ///
            /// https://speakerdeck.com/shun_pi/ahc023can-jia-ji-zan-ding-ban-shou-shu-kitu-duo-me?slide=18
            #[derive(Debug, Clone)]
            struct ConnectionChecker {
                dict: [bool; Self::DICT_SIZE],
            }

            impl ConnectionChecker {
                const DICT_SIZE: usize = 1 << 9;
                const AREA_SIZE: usize = 3;
                const CENTER_INDEX: usize = 4;

                pub const fn new() -> Self {
                    Self {
                        dict: Self::gen_dict(),
                    }
                }

                const fn gen_dict() -> [bool; Self::DICT_SIZE] {
                    let mut dict = [false; Self::DICT_SIZE];
                    let mut flag = 0;

                    while flag < Self::DICT_SIZE {
                        // 3x3マスの中央を除いた後
                        let after_remove = flag & !(1 << Self::CENTER_INDEX);
                        dict[flag] = Self::is_connected(after_remove as u32);
                        flag += 1;
                    }

                    dict
                }

                const fn is_connected(flag: u32) -> bool {
                    const fn dfs(coord: Coord, flag: u32, mut visited: u32) -> u32 {
                        visited |= 1 << coord.to_index(ConnectionChecker::AREA_SIZE).0;
                        let mut i = 0;

                        while i < ADJACENTS.len() {
                            let adj = ADJACENTS[i];
                            let next = Coord::new(
                                coord.row().wrapping_add_signed(adj.dr() as isize),
                                coord.col().wrapping_add_signed(adj.dc() as isize),
                            );

                            if next.in_map(ConnectionChecker::AREA_SIZE) {
                                let bit = 1 << next.to_index(ConnectionChecker::AREA_SIZE).0;

                                if (visited & bit) == 0 && (flag & bit) > 0 {
                                    visited = dfs(next, flag, visited);
                                }
                            }

                            i += 1;
                        }

                        visited
                    }

                    const fn coord(index: usize) -> Coord {
                        Coord::new(
                            index / ConnectionChecker::AREA_SIZE,
                            index % ConnectionChecker::AREA_SIZE,
                        )
                    }

                    if flag == 0 {
                        return false;
                    }

                    let first_index = flag.trailing_zeros() as usize;
                    let visited = dfs(coord(first_index), flag, 0);

                    visited == flag
                }
            }

            #[cfg(test)]
            mod test {
                use super::*;
                use crate::problem::{BitBoard, Cell};
                use rand::Rng;
                use rand_pcg::Pcg64Mcg;

                #[test]
                fn board_pext_test() {
                    let mut rng = Pcg64Mcg::new(42);

                    for _ in 0..1000 {
                        let mut board = BitBoard::new(0);

                        for i in 0..100 {
                            if rng.random_bool(0.5) {
                                board = board.set(Cell::new(i as u64));
                            }
                        }

                        let cell = Cell::new(rng.random_range(0..100) as u64);
                        let coord: Coord = cell.into();
                        let pext = BoardPext::new(cell);
                        let flag = unsafe { pext.get_flag(&board) };
                        let mut expected_flag = 0;
                        let mut i = 0;

                        for dr in -1..=1 {
                            for dc in -1..=1 {
                                let next = coord + CoordDiff::new(dr, dc);

                                if next.in_map(Input::MAP_SIZE) {
                                    let index = Cell::from(next);

                                    if board.get(index) {
                                        expected_flag |= 1 << i;
                                    }
                                }

                                i += 1;
                            }
                        }

                        assert_eq!(expected_flag, flag);
                    }
                }
            }
        }
        mod monte_carlo {
            use crate::{
                problem::{BitBoard, Board, Cell, GameState, Input, AI},
                solver::main_solver::{
                    articulation::ConnectionCheckerBitBoard,
                    network::{NNInput, ValueNN},
                    thompson::PlayerAICandidates,
                },
                util::ChangeMinMax,
            };
            use itertools::Itertools;
            use ordered_float::OrderedFloat;
            use rand::{Rng, SeedableRng};
            use rand_distr::{Distribution as _, LogNormal, Normal};
            use rand_pcg::Pcg64Mcg;
            use std::{
                cmp::Reverse,
                env,
                ops::RangeInclusive,
                str::FromStr,
                time::{Duration, Instant},
            };

            #[target_feature(enable = "bmi1,bmi2,popcnt")]
            pub(super) unsafe fn choose_action(
                input: &Input,
                game_state: &GameState,
                estimated_ais: &[Vec<AI>],
                value_nn: &ValueNN,
                player_ai_candidates: &mut PlayerAICandidates,
                rng: &mut Pcg64Mcg,
            ) -> Cell {
                let mut legal_actions = game_state.get_legal_moves(0).iter().collect_vec();

                if legal_actions.len() == 1 {
                    return legal_actions[0];
                }

                let round_count = ((legal_actions.len() << 1) - 1).ilog2();

                let elapsed = input.since.elapsed();
                let time_limit = Duration::from_millis(1990);

                let duration = time_limit
                    .saturating_sub(elapsed)
                    .max(Duration::from_millis(5))
                    / (Input::MAX_TURNS - game_state.turn) as u32;
                let each_duration = duration / round_count;
                let since = Instant::now();
                let mut total_iter = 0;

                let mut score_sums = vec![0.0; legal_actions.len()];

                for round in 0..round_count {
                    let seed: u64 = rng.random();

                    for iter in 0.. {
                        if since.elapsed() >= each_duration * (round as u32 + 1) {
                            break;
                        }

                        let (player_ai, player_ai_index) = unsafe { player_ai_candidates.choose() };

                        unsafe {
                            let score = run_monte_carlo(
                                input,
                                game_state,
                                &player_ai,
                                estimated_ais,
                                value_nn,
                                &legal_actions,
                                seed,
                                &mut score_sums,
                                iter,
                                &mut total_iter,
                            );

                            player_ai_candidates.update(player_ai_index, score);
                        }
                    }

                    // Successive halvingで上位半分だけを残す
                    let mut candidate_indices = (0..legal_actions.len()).collect_vec();
                    candidate_indices.sort_by_key(|&i| Reverse(OrderedFloat(score_sums[i])));
                    candidate_indices.truncate((candidate_indices.len() + 1) / 2);

                    legal_actions = candidate_indices
                        .iter()
                        .map(|&i| legal_actions[i])
                        .collect_vec();
                    score_sums = candidate_indices
                        .iter()
                        .map(|&i| score_sums[i])
                        .collect_vec();
                }

                eprintln!("Monte Carlo iters: {}", total_iter);

                assert_eq!(legal_actions.len(), 1);
                legal_actions[0]
            }

            #[target_feature(enable = "bmi1,bmi2,popcnt")]
            unsafe fn run_monte_carlo(
                input: &Input,
                game_state: &GameState,
                player_ai: &PlayerAI,
                estimated_ais: &[Vec<AI>],
                value_nn: &ValueNN,
                legal_actions: &[Cell],
                seed: u64,
                score_sums: &mut [f64],
                iter: usize,
                total_iter: &mut usize,
            ) -> f64 {
                const PLAYOUT_TURNS: usize = 15;
                let ai_i = iter % estimated_ais[1].len();
                let mut nn_input = NNInput::new(input);
                let mut ais = vec![&AI::DUMMY];
                for player_i in 1..input.player_count {
                    ais.push(&estimated_ais[player_i][ai_i]);
                }
                nn_input.set_ai_params(&ais);
                let mut max_score = f64::NEG_INFINITY;

                for (action_idx, &action) in legal_actions.iter().enumerate() {
                    *total_iter += 1;
                    let mut rng = Pcg64Mcg::seed_from_u64(seed + iter as u64);
                    let mut game_state = game_state.clone();

                    let mut actions = [Cell::default(); Input::MAX_PLAYERS];
                    actions[0] = action;

                    for player_i in 1..input.player_count {
                        let ai = ais[player_i];
                        let legal_actions = game_state.get_legal_moves(player_i);
                        let action = unsafe {
                            ai.choose_eps_greedy(input, &game_state, legal_actions, &mut rng)
                        };
                        actions[player_i] = action;
                    }

                    game_state.apply_actions(input, &actions[..input.player_count]);

                    for _ in 1..PLAYOUT_TURNS {
                        if game_state.turn == Input::MAX_TURNS {
                            break;
                        }

                        // 他のプレイヤーが踏みそうなマス
                        let mut avoid: BitBoard = BitBoard::default();

                        let mut actions = [Cell::default(); Input::MAX_PLAYERS];

                        for player_i in 1..input.player_count {
                            let ai = &estimated_ais[player_i][ai_i];
                            let legal_actions = game_state.get_legal_moves(player_i);
                            let candidates = unsafe {
                                ai.get_greedy_candidates(input, &game_state, &legal_actions)
                            };

                            // 他のプレイヤーが踏みそうなマスをavoidに追加する
                            let to_avoid = match candidates {
                                crate::problem::ActionCandidates::Single(cell) => cell.to_board(),
                                crate::problem::ActionCandidates::Multiple(bit_board) => bit_board,
                            };
                            avoid |= to_avoid;

                            let action = if rng.random_bool(ai.eps) {
                                unsafe { legal_actions.choose(&mut rng).unwrap() }
                            } else {
                                unsafe { candidates.choose(&mut rng) }
                            };
                            actions[player_i] = action;
                        }

                        // 自分は踏まないようにする
                        let legal_actions = game_state.get_legal_moves(0);
                        let legal_actions_without_dup = legal_actions & !avoid;
                        let candidates = if legal_actions_without_dup != BitBoard::default() {
                            legal_actions_without_dup
                        } else {
                            legal_actions
                        };

                        let action = unsafe {
                            player_ai.choose_eps_greedy(input, &game_state, &candidates, &mut rng)
                        };
                        actions[0] = action;

                        game_state.apply_actions(input, &actions[..input.player_count]);
                    }

                    unsafe { nn_input.set_game_state(&game_state) };
                    let pred_score = unsafe { value_nn.forward(nn_input.get_inputs()) };

                    score_sums[action_idx] += pred_score as f64;
                    max_score.change_max(pred_score as f64);
                }

                max_score
            }

            pub struct PlayerAI<'a> {
                coef: GreedyCoef,
                eps: f64,
                value_map: Board<f64>,
                articulation_checker: &'a ConnectionCheckerBitBoard,
            }

            impl<'a> PlayerAI<'a> {
                pub fn new_random(
                    input: &Input,
                    player_params: &PlayerParam,
                    articulation_checker: &'a ConnectionCheckerBitBoard,
                    rng: &mut impl Rng,
                ) -> Self {
                    let coef = player_params.sample_greedy_coef(rng);
                    let eps = PlayerParam::sample_range(&player_params.eps, rng, &(0.0..=1.0));
                    let mut value_map = input.values_f64.clone();

                    for cell in 0..Input::MAP_SIZE * Input::MAP_SIZE {
                        let cell = Cell::new(cell as u64);
                        value_map[cell] = player_params.random_map.sample(rng);
                    }

                    Self {
                        coef,
                        eps,
                        value_map,
                        articulation_checker,
                    }
                }

                #[target_feature(enable = "bmi1,bmi2,popcnt")]
                pub unsafe fn choose_eps_greedy(
                    &self,
                    input: &Input,
                    state: &GameState,
                    legal_moves: &BitBoard,
                    rng: &mut impl Rng,
                ) -> Cell {
                    if rng.random_bool(self.eps) {
                        unsafe { legal_moves.choose(rng).unwrap() }
                    } else {
                        unsafe { self.get_greedy(input, state, legal_moves) }
                    }
                }

                #[target_feature(enable = "bmi1,bmi2,popcnt")]
                unsafe fn get_greedy(
                    &self,
                    input: &Input,
                    state: &GameState,
                    legal_moves: &BitBoard,
                ) -> Cell {
                    assert_ne!(*legal_moves, BitBoard::NONE);
                    let mut best_cell = Cell::new(legal_moves.bits().trailing_zeros());
                    let mut best_eval = f64::NEG_INFINITY;
                    let my_territory = state.territory_bits[0];
                    let mut remaining = *legal_moves;

                    // 無所属
                    let white = remaining & !state.all_territory_bits;
                    self.update_eval(
                        &mut remaining,
                        &white,
                        self.coef.empty,
                        &mut best_cell,
                        &mut best_eval,
                    );

                    // 自マス
                    let mut my_territory = remaining & my_territory;
                    remaining &= !my_territory;

                    {
                        // 自マスmax（-INF点なのでスキップして良い）
                        let my_max = my_territory & state.max_level_bits;
                        my_territory &= !my_max;

                        // 自マスレベル1
                        let my_1 = my_territory & state.level_one_bits;
                        self.update_eval(
                            &mut my_territory,
                            &my_1,
                            self.coef.my_1,
                            &mut best_cell,
                            &mut best_eval,
                        );

                        // 自マスその他
                        let my_other = my_territory;
                        self.update_eval(
                            &mut my_territory,
                            &my_other,
                            self.coef.my_other,
                            &mut best_cell,
                            &mut best_eval,
                        );
                    }

                    // 1位の相手のマス
                    let target_opponent = (1..input.player_count)
                        .max_by_key(|&i| state.scores[i])
                        .unwrap();
                    let opponent_territory = state.territory_bits[target_opponent];
                    let mut opponent_territory = remaining & opponent_territory;
                    remaining &= !opponent_territory;

                    {
                        // 1位の相手のレベル1のマス
                        let opponent_1 = opponent_territory & state.level_one_bits;
                        opponent_territory &= !opponent_1;

                        if opponent_1 != BitBoard::NONE {
                            let mut max_articulation_cell = Cell::default();
                            let mut max_articulation_value = f64::NEG_INFINITY;
                            let mut max_normal_cell = Cell::default();
                            let mut max_normal_value = f64::NEG_INFINITY;

                            // 1位の相手の移動可能範囲をBFS
                            let opponent_connection = state.positions[target_opponent]
                                .to_board()
                                .bfs(&state.territory_bits[target_opponent]);

                            for cell in opponent_1.iter() {
                                let value = self.value_map[cell];
                                let is_articulation_point = opponent_connection.get(cell)
                                    && unsafe {
                                        self.articulation_checker
                                            .is_articulation_point(&opponent_connection, cell)
                                    };

                                // 関節点かどうか判定
                                if is_articulation_point {
                                    if max_articulation_value.change_max(value) {
                                        max_articulation_cell = cell;
                                    }
                                } else {
                                    if max_normal_value.change_max(value) {
                                        max_normal_cell = cell;
                                    }
                                }
                            }

                            if best_eval.change_max(
                                max_articulation_value * self.coef.top_opponent_articulation,
                            ) {
                                best_cell = max_articulation_cell;
                            }

                            if best_eval.change_max(max_normal_value * self.coef.top_opponent_1) {
                                best_cell = max_normal_cell;
                            }
                        }

                        // 1位の相手のその他のマス
                        let opponent_other = opponent_territory;

                        self.update_eval(
                            &mut opponent_territory,
                            &opponent_other,
                            self.coef.top_opponent_other,
                            &mut best_cell,
                            &mut best_eval,
                        );
                    }

                    // その他の相手のマス
                    {
                        let other_1 = remaining & state.level_one_bits;

                        self.update_eval(
                            &mut remaining,
                            &other_1,
                            self.coef.opponent_1,
                            &mut best_cell,
                            &mut best_eval,
                        );

                        let other_other = remaining;

                        self.update_eval(
                            &mut remaining,
                            &other_other,
                            self.coef.opponent_other,
                            &mut best_cell,
                            &mut best_eval,
                        );
                    }

                    assert_eq!(remaining, BitBoard::NONE);

                    // タイの場合はサボって、最初に見つけたマスを返す
                    best_cell
                }

                fn update_eval(
                    &self,
                    board: &mut BitBoard,
                    targets: &BitBoard,
                    coef: f64,
                    best_cell: &mut Cell,
                    best_eval: &mut f64,
                ) {
                    if targets != &BitBoard::NONE {
                        let mut max_cell = Cell::default();
                        let mut max_value = f64::NEG_INFINITY;

                        for cell in targets.iter() {
                            let value = self.value_map[cell];

                            if max_value.change_max(value) {
                                max_cell = cell;
                            }
                        }

                        if best_eval.change_max(max_value * coef) {
                            *best_cell = max_cell;
                        }
                    }

                    *board &= !*targets;
                }
            }

            struct GreedyCoef {
                empty: f64,
                my_1: f64,
                my_other: f64,
                top_opponent_articulation: f64,
                top_opponent_1: f64,
                top_opponent_other: f64,
                opponent_1: f64,
                opponent_other: f64,
            }

            impl GreedyCoef {
                fn new(
                    empty: f64,
                    my_1: f64,
                    my_other: f64,
                    top_opponent_articulation: f64,
                    top_opponent_1: f64,
                    top_opponent_other: f64,
                    opponent_1: f64,
                    opponent_other: f64,
                ) -> Self {
                    Self {
                        empty,
                        my_1,
                        my_other,
                        top_opponent_articulation,
                        top_opponent_1,
                        top_opponent_other,
                        opponent_1,
                        opponent_other,
                    }
                }
            }

            pub(super) struct PlayerParam {
                empty_coef: LogNormal<f64>,
                my_1_coef: LogNormal<f64>,
                my_other_coef: LogNormal<f64>,
                top_opponent_articulation_coef: LogNormal<f64>,
                top_opponent_1_coef: LogNormal<f64>,
                top_opponent_other_coef: LogNormal<f64>,
                opponent_1_coef: LogNormal<f64>,
                opponent_other_coef: LogNormal<f64>,
                eps: Normal<f64>,
                random_map: Normal<f64>,
            }

            impl PlayerParam {
                pub(super) fn from_env() -> Self {
                    fn get_env<T: FromStr>(name: &str, default: T) -> T {
                        env::var(name)
                            .ok()
                            .and_then(|s| s.parse().ok())
                            .unwrap_or(default)
                    }

                    // 係数は LogNormal(mu, sigma) で生成する。
                    // mu は対数空間の平均値で、0.0..ln(100) 程度を使うと 1..100 オーダーになりやすい。
                    let empty_coef_mean = get_env("EMPTY_COEF_MEAN", 0.6035207610642568);
                    let empty_coef_std = get_env("EMPTY_COEF_STD", 0.9594121144116171);
                    let my_1_coef_mean = get_env("MY_1_COEF_MEAN", 1.123741403863814);
                    let my_1_coef_std = get_env("MY_1_COEF_STD", 0.3481455054826505);
                    let my_other_coef_mean = get_env("MY_OTHER_COEF_MEAN", 0.5668353029017499);
                    let my_other_coef_std = get_env("MY_OTHER_COEF_STD", 0.47078604397138324);
                    let top_opponent_articulation_coef_mean =
                        get_env("TOP_OPPONENT_ARTICULATION_COEF_MEAN", 4.254816528218739);
                    let top_opponent_articulation_coef_std =
                        get_env("TOP_OPPONENT_ARTICULATION_COEF_STD", 0.9176264115734788);
                    let top_opponent_1_coef_mean =
                        get_env("TOP_OPPONENT_1_COEF_MEAN", 3.8035692257769913);
                    let top_opponent_1_coef_std =
                        get_env("TOP_OPPONENT_1_COEF_STD", 1.1393842650219308);
                    let top_opponent_other_coef_mean =
                        get_env("TOP_OPPONENT_OTHER_COEF_MEAN", 3.233843758642677);
                    let top_opponent_other_coef_std =
                        get_env("TOP_OPPONENT_OTHER_COEF_STD", 0.7585249870011287);
                    let opponent_1_coef_mean = get_env("OPPONENT_1_COEF_MEAN", 2.0199789733903617);
                    let opponent_1_coef_std = get_env("OPPONENT_1_COEF_STD", 1.3421417747732423);
                    let opponent_other_coef_mean =
                        get_env("OPPONENT_OTHER_COEF_MEAN", 0.49424304793750495);
                    let opponent_other_coef_std =
                        get_env("OPPONENT_OTHER_COEF_STD", 1.1339980206187095);
                    let eps_mean = get_env("EPS_MEAN", 0.46592072796083717);
                    let eps_std = get_env("EPS_STD", 0.17298797087705275);
                    let random_map_mean = get_env("RANDOM_MAP_MEAN", 7594.0089933813015);
                    let random_map_std = get_env("RANDOM_MAP_STD", 559.0219602649277);

                    let empty_coef = LogNormal::new(empty_coef_mean, empty_coef_std).unwrap();
                    let my_1_coef = LogNormal::new(my_1_coef_mean, my_1_coef_std).unwrap();
                    let my_other_coef =
                        LogNormal::new(my_other_coef_mean, my_other_coef_std).unwrap();
                    let top_opponent_articulation_coef = LogNormal::new(
                        top_opponent_articulation_coef_mean,
                        top_opponent_articulation_coef_std,
                    )
                    .unwrap();
                    let top_opponent_1_coef =
                        LogNormal::new(top_opponent_1_coef_mean, top_opponent_1_coef_std).unwrap();
                    let top_opponent_other_coef =
                        LogNormal::new(top_opponent_other_coef_mean, top_opponent_other_coef_std)
                            .unwrap();
                    let opponent_1_coef =
                        LogNormal::new(opponent_1_coef_mean, opponent_1_coef_std).unwrap();
                    let opponent_other_coef =
                        LogNormal::new(opponent_other_coef_mean, opponent_other_coef_std).unwrap();
                    let eps = Normal::new(eps_mean, eps_std).unwrap();
                    let random_map = Normal::new(random_map_mean, random_map_std).unwrap();

                    Self {
                        empty_coef,
                        my_1_coef,
                        my_other_coef,
                        top_opponent_articulation_coef,
                        top_opponent_1_coef,
                        top_opponent_other_coef,
                        opponent_1_coef,
                        opponent_other_coef,
                        eps,
                        random_map,
                    }
                }

                fn sample_greedy_coef(&self, rng: &mut impl Rng) -> GreedyCoef {
                    GreedyCoef::new(
                        self.empty_coef.sample(rng),
                        self.my_1_coef.sample(rng),
                        self.my_other_coef.sample(rng),
                        self.top_opponent_articulation_coef.sample(rng),
                        self.top_opponent_1_coef.sample(rng),
                        self.top_opponent_other_coef.sample(rng),
                        self.opponent_1_coef.sample(rng),
                        self.opponent_other_coef.sample(rng),
                    )
                }

                fn sample_range(
                    normal: &Normal<f64>,
                    rng: &mut impl Rng,
                    range: &RangeInclusive<f64>,
                ) -> f64 {
                    loop {
                        let sample = normal.sample(rng);

                        if range.contains(&sample) {
                            return sample;
                        }
                    }
                }
            }
        }
        mod network {
            mod network_params_by_mu {
                // Auto-generated by py/export_small_fc64_params_by_mu.py
                // (M,U) ごとの small_fc64 パラメータ定数 (f32 bytes -> Z85)

                const ENCODED_SMALL_FC64_M2_U1: &str = concat!(
    "0/>pl+Cg7V03A]bj@8+:&n/<fv8qrzGEg9uqyEghgfe52].Oc(PY-Qz47S{ZFAR)5lC/e7W9K)#CQOkcohN4HIW?9$Ncd2NfL-tO",
    "2:#F>u{D23-6i/RGKbxRVf.1]14MXhyNCEwIaDEo+K[j=n@RT<azZg7)41x{@7]ml42Aix&8(bP[wrz.u@C*]PBQd#jzzBv)8=v)",
    "08<YCL&(t=v4!G4nrn}DW3Y4I&hVjUZ=y!]/K%P+sFl!H^2B$!Z><ZbPYzMPegt>U1wY0g5wTNAkNi@AC.b+8rbz]OihUAy3iPfG",
    "rb4BSv&-#Uz6ez-E=0mIOEAU7/[%Yh0hhkio45KewCBoWaEgS(Z]>mhV}yfT6m2wDC8b$eFuBdgaOq[KFr%z4dazD:=$.q$Sqf<+",
    "z%]RF1!TU+XcY=g05ND-[z>MLk(qG.3>(^SfH?.FyxH[9kx=8079]Y/Jl]B4lQgYnkSa=/swdTpnxy!zgnB!h{[sH%(%unDJ9f<T",
    "nwBA5sKR)+V]=KCH^5pg2jzb%[u*lG-g/aLg=*/k!dxBeFkbHC+Q4}QAm2GmFc6I#0m.cidY}X[l@zM0Frc!yE9pqd0bdlL07.8(",
    "+Uw5H&aQI1A4{Dqy#?SX7zT}NG?jr?qL8ENvGyHlI.(V(pr@KDsxLL+AkdF$Xu*]MFsWK&>T49?48PtlGbbp/a$[Kj01S.NaLO-^",
    "az%?n5r@tvAhni3d-Ere:RhMOf&*YQ08?l9em40[1KeWmv6k$YV<$2qH&0Wo=@t20s]71?W*UX*p4-hke}KLE]DcSqfS3!xkjjVG",
    "^:[ibp%Pka>0(+(bOj+8GI!CR{]8I3Z.D4Dc}xgUwit91k*Sm*eog>RAhd({)6y1YC=oT=m3ko#7db%M1%?:jcl{:FkM?Lf{Zr>g",
    "FA&69O{d7}ax3y@CN-4-[g<as[xXQbPQ%Myp-%1bX{nxX1aPL$j&WsZ7dw{43ekeq!!O&Y5lj+7^cI32aCeic1Po:Z}U(2U/6i1z",
    "TouUUo43^9CQLwY)7sSc5yPZOZ9.e?2N]}g4^%0MnG<iVM(J0p1Gsdtx14d>M{g!*5pdD(*q7>A]J}QikvQ0zmJ(+m}Vc&lXa#<-",
    "swOMMCN}9[98!o00biAlOlsvPyj2V/Xcun508eoXapjY50bVvhhv#JAawJRlc<J5KfL&NdSqUZrN18xOdQ6qACO:(6s#0JIf7%(W",
    "n^IW+BJhhhq0kIP!A8&RycY5qsGo[lu%(z^uHWr{E0gmT1M9-Mi/w<d7KRaO7{a)]x%$dSHtTyyzYt/yK[5jWu&dgy/B&r&9/VvD",
    "oD?{0UNMA402N%:kUUB]j&K!u0?^3o05<qSFm5yvsuoi<gciK<i4q6]CZ2oky.gwB(ktvt)O60hm7Cs>i5?2N7@stbp?ZfF!r)IO",
    "6jnbWLfxmQb.<HP@^>l=Avk/<-n*p0gl?*LZ*#3f}Bu&m]JvM{wm^FT931{<deaVznsE@ssvxRYr6Y}oP0u:cxI!]CBoS@51yy1H",
    "sHhE1T$wIzKA281/8jZAfR&>P02tR#swEyfx.zr^X%gEM7${jFi69c:cxc##/.5sVH}nLTnfF3$UYVf)T<r1&O9#&Wd2rR<pn^P{",
    "v=/*nrC(zixMA#jXrP4!0e}D4FBy)%+V0xiOz}@fOV*!X]cak$!m=81-qgiidblWyxK/P9d3MFI>:sVG]f+I$I-SWR0fO&{cwP4y",
    "1N(KSQ{ae*-el#DoDn8^/I)}}yz^)ID@)$)I3bisrcJ=90aZ>%r5f$bXd)Vg65hRkfTe^mM{5OO/G]6<x8vL2i&X(poM2zAl%gRB",
    "PI8TN@oZWJ[@}P!!n^obaL2je>S4?un{GX=v72C^kX:E:m:e$qU=/T(5fGdWzvJFRv1EQQOrVb#penEnapFDp/D[o{us8Za00m<r",
    "Xtd?y=.77%Fv)zE!+ONBcMTA{4n!*]=&QZt^1*NyPH7rA&iNyo}Q&}N44}n%@a9(imfVEJPE=OA.I2iT5l1rCUV92l4MpZT2UyS2",
    "UQ>ZCLWCm*oE]%52Q#95ewbsMH=xJu&oB/n3s/t:a6Od@-UgLIQ3+/xg1mbPtE+wgav=%Q7$zo)@qdqzX<>jk&f.LFPD0^wb+[[-",
    "SlKnlZ:/P2M(!ZVVrjDDNb{Ay5BA65Z+P{(xQ5@*&kbjLAiiY+FlH:w(pZlLQ6DYwU.TQrRA0.K}u:O[fK&:0Am}e5J<bDB!&Jum",
    "<]U4=p=3=/<r@8k{tB))u-/{kem3sW6l6NcL(a(LEOV1/gY1U{9)%U]@8RjUr>u4eck33^bWK3PtN-}Y95<I4s!J2.wk{KYy$)]i",
    "dGXc-qQ$3BeA+LDC.LMG!48CuT2X3W1E>gWD#/oJ6MiRc@S{w7kNF@6gdwia?vHw:GGy$N(#OpM@xG%r/T8CUcy7]2m3J+CZ:qgR",
    "}J*nb&7E(GM{wB0:GMQDPM@#tt/0f#v0eUmsN$SruknQoApFVB7]%&5)TaAow{H4jat.GVxOEOwZ{4Z?k=DAG:HPNdpYX?O2>N#c",
    "2kMXg/L$y:PT@K.H>bAog!}gp5krzx}JV(I-)uL9]/]PxKj5.C9RoSL2aQ-r=>T[<m])5Z[k1W1Si!YJ]YYL)&oIQIk>7<qg&z]#",
    "0a?[rsFj6Qy%whcqEC6?y={Prt./WHAihQxWVQFJDaBl]Ia40Hau-JkFyLz$-}$uJ*vjyv2Uw)DCSyNylZVW)U??1DF%(.Ov>+Ba",
    "4/t/Xi9n!^A6BT>KqEn@u(wWKZ@CH#G8j=CC:GElLOGX3Dx%<W9g0P/):}6z6Me09A1^dM+9}z8nxp8f]O)xAC<52rQl%SED@6z#",
    "u>w$[4J06%>Q9TG[x2(q2/tEYZ=vPu=>vLmaIk>fQ]7:!!kz^HxPJ@(^WnYKH3NqNESc/3:Z=s8wTC/}<j>&){6i}0Tw/$Gk4zX$",
    "2.gR(O)EC<C+QhgqPv9Eiy=1(=ZEu*il1/UkN??FWZArLkOQaT1ON5M-TnUck]rMZtfCtQ3nNgC5DdnAk:zX*kO8)pH=MOu]C]G@",
    "x}[=]OK5b}hXtt?8oy1l/UnPwLd6C+v2J)NKE(3QvIw:OZ=YxA^KhT&&aGBet.gUyinHKCNP(]c^7<85o6V8.ETq4cygr&0KNn&a",
    "hJjs)CiW+]Y.h&GbU2Tlg/*9Vl$JhT@aMRNrlS#Tyt%/T/B++UCWFYTioRl[]ZaJH]XFGtVF$.]-7FYCYDpcpBr.sfL:?]>jERT5",
    "h?q5{:/mu=H)Bh!p&DYNo0>rQ7&w1<u}=Fu2*Qe$FdVLg8GDq4-2m{PTvXZvYX42k!CQb}tVT+nAadRps16DFRH2BcWGAu?C=4(i",
    "1waoWH{TxH&k9]aZ(S!7*Y-%aNFPljuxr2*.1kf3)O1wf0(z03u)%+(P.pc7/PQi^]HUK=AcTf2IPJZ#7*0-IFj{qjkWlK^{<px2",
    "J(ncm[dflCd8Nn)CUTQrBPdW>Jq7<=J--c>aMd5#4f5<Y3FrP^i2ziI{$Atxm=}[c1DmDoc[r1}usnyGP7R?v>9<A9t?lRLqRUG&",
    "CRZLS5W<m7IfAz[IkR=oe.68Ga6tHaTqU6KUZ-Z!k.r:.CTfRr43H&T2!Dl-v0Tllb$oD25k#onWb2JhneZ2wA51TY2XmycM[#$0",
    "YbT?-Xph4Le&yRY5sdJ69n{uIi({i({s!%#>:F59iV$v:A>k^zo1ye1-SYnN3a*zQ(z6#+dN?}4^djGQ0fkt=&StmQ&1g)Xg/k13",
    "ga7Ek2.{WstX-Yur1{xp{b7Zv+iMb(00X+4Z$0Fr=(mi)zPxvLIF81-C<$C<fQQIa@9/oib0grrMoT>4i4]M<^3*-ykPc&t{su66",
    "0rfsr+YxO[GY{Gu^H<X!>gUFLc(OC=Bu)u05%Xx<eZGXApj92N(EI=A@!Pz]g)S>V0isOZS07g7M@*yLMMMQGar8)}3@Wf&XpBs^",
    "{{<M>X^!5u?HS0@/45bgL14ZI-dd{6r}KIEG.gCv0k&JZQ=O=zZ(y:]&on!afxD7]3v*2T>6m3wID#=oO!zKCp>7I?A8P9pA4Pbz",
    "g[ebv(r.xw7?F0a0drrW/U1BJ&5}(t{m/5W@*G]K<Nj@?Oj[dufL6sVawi0N?JW^#k>FLFX>qxNS2X^H^1&gAGMuj*B$x4ATNs[c",
    "r/KBGl[B.s&XPSd2?e94zACl?[f%pdNKu+33]8>qjfEkGo2MPH7>hU4s9?r/wrgrnNLFwo:}T86<yKgYg{9*(P.(gflDbSY[i[@)",
    "C@9G6eD]j?fCy9w0oNy%S(JG1MyT%qspGR<IOmfC5:%tzlh$ShC&uMMFhWOzCcN@dU^tK^ax#lo[r>Q^?}}9(DuClgFm0Ys/C%?3",
    "5XXaOFz.}v}#%.@fMw^e9l#8TfCJfLoQ1>S=of(B+<SrW7+rY<KHCYZnptD<V{QrCu{=t82(aJ+rcC62V$GRa9W/NMc<xsS/C^@a",
    "kXX(:M>#bL0m3bafXq@gNJwv&rUR7qilpby0]T8wxLh$hxYXu?3>]y0yWCa+RQEj.q4!jrzuc/LDjC.CyI[R=hHcW-KD[h9OoGy{",
    "So3iafKh^:J.laXg%pwifvQ#R9-xFXZ=(J@VBNoOXo>r&v5/f$)14df2+=EQe(3{Uj+vrel)QbxaEo)Tu])Hl2S7nGrdW@Wg]Ko+",
    "Pl71Bv{k2dZb(V=p%K5z<GO:1{*:DtFk^jA.Rs#K{*6vLPQqjYKCQfNSlA@c!(*iW<BWDCPPF6z<$Vx?-<S?003t-j{%5K}t:>ZR",
    "&.ln-bQnpiFpFv$v=+x*:o&RA*Z*9(va6iYc$XpFjO+KhWX]-XFf4y@^74rkMEC-A9q:nf{kW88XGBKe/CEvE0CdxS[PDm]2.-F%",
    "l/RjXTtFM[axN[(fP8sicr[$.NHXL{sM%.p4UZdXvo=k:J^?8AqD@iBw^cvvM(u#c2?k==k<:fng9hArm<-NA7jQr!y}mT2i9}Rd",
    "0628id6AAVgl^qLIKGvmQ1XV[Xqj0Bdf5?K/D@Qwu!]@>NKN7a8Ql[$&f3WA&o*iH]X[%@}0l.FZ#urC^5dk-<FIc7PJ:B&1@Ca?",
    "wdaWonf*jejFu%Zaqv+![f*sX:O4Tc2ayVq44hKSM]ewpf44T3no2Rj1GzBmTq5*<D<LhldNeRIfUla6Oc#Hku<bV8mL.fX8d<ds",
    "j4*qM-GG&at>iY=3{5{NV2%bh)SI.%Dd&S&vGh4}c&7={:wIN[uv]{P3^TL>6-M6n?7H&Md0w.bso4FFi{b2bF3Ip50NJ5P2ep{r",
    "JSoRO0ga6S=<x84)3L4vrnphi]PTL-NY5cUJ52)%Z!)Xol(nh6m+G63VHJAdZpV6ZHllO([i#0ywk$X8].3*0]aEh09a6/4c)2cD",
    "Dm8R8f2pHJDqe+2LJCGKaDnSI08@)gPPbXV+:^j%i)[tGPCu#VqC[sBESX=b>k3{GgZa%+m&kq>Ml=#6Jcoh8c<=Uj-UuJU87y?m",
    "9?G&6ARJ9?0m8+4fB=KBTg!CA-4&S6nX>{3dIb]J[n>RxZ]pRMe=]X1(%CdOWe4[UniOT?bVZN3ax:38@U>@6s#[BMaxfw}7[8vb",
    "Ug#rbk>eQ6!y:%*jDv7/+5PB8qBc.+-9#C84q?j>5@NsndiH@wemzZCdHXORsfCDm-/cMM3.u3n?l@k9][3U!K.)DGb8yw93cC@Q",
    "egUZ7QP2g}+7i^hHC.lc>6/n6mn%Ja594&E&A::xa)<.vOq80%>0r7X3P](LW-(VMe<y1ke.W8n!>CA[DQ3^UIbNek5-LaJB9vKC",
    "Jo>ZKK/N4%Ek1L.wcOnYE?sd]FAB5Ihu(HXVKwoEKNa5MDF4P0Gtx035j5Q]Ue<6@eZKWny[qk5)j9]Psy#OqP&=(Wq)G(Dm+Xx^",
    "KTR6WeeL[hQYn&SbA+11YO+SgHZvPQzo6A8.N*O$v01T$xO.:h+>*2ash6LPZ!J@OM5*EfRe[1Fg8SG]tL[Jo9f}3$x0gNnG=jNp",
    "mKg1d4B#iba%>ZKwy8?<Flt@o4ba=DQtqw7L^em(KIVEXYh>T?8lg8dXlzCo>n^+&4%%p0n70EkSk^P9cvXiyP16>Dk83IXb=KB3",
    "TBnIW:sT>:sl]W}HAWnw5p^?B[o})Y!KD:Qe]quqEuo%Z-5O8!ikvw+?-+WpVyj(VFoVaGGD91^z=Dl?]!zj+QAJZM7u/>OlLQz3",
    "&{fr51%ZE/UGpj^/1q?-RbCS5F*#pnU#5[jIfK+tiIkow6tbZdTk%/8?{SvQyc]%7T5bJOTg6p/4ql.C[<W)!8b(KfJ}#:t}3&YP",
    "b#0U?-:@h+D-v0b2J[cium%EnB5>a.2+wllCs7BQIF^mAN^UkW]2.-Y(%A&bTFGEw]R1&vm^6JVmAD*??6svi<u?ziuRRw2Nz:nk",
    "SmGkamhlDJMzh)yw*d<LK5TlkEJI.bO3]^?sFFkDrOGEA1f6m60UP9ex4wm(seQ$<ZI<K{)4Q(3jtl=IW#%v$S)<s13NzJhpnc4G",
    "5T)sCrDxrEFcPV1C&h]vj++dAip@I1BKI2sxE$1l=CAU%(dZ*74Y=zsJ1Z@VKvN8F/Mei1JOLZvClFh+l%EZBkD5hcbCR?kPXJ!+",
    "iICoxudoR6YJk7$KbG!}+kA^6kcFq9::=gYtT4C=[ENxrsvi&*zsYXI!e$RK@4t[2?3wu<EbG-y6L^#*74PbMpr0az:3G$]Zj(5t",
    "VjT1F(9<tIdEy7#+/TiQL{3WJI-vX1R&!}YtXjR.kX6:O<.NS88W%f*)bgn}zA6^z0iswRH0#UTSeoctaFGHZE@#?%:LfPQPd5UY",
    "ksQq?1OYL)j<sCJAc=Su/2N)aJtDrw-x]M#A-BT7:OXutWg%9kVWU9KA!LY31xC^(z]!ndZDmlm}Xu}iDw&w0AJ]NftbK+M>MVw$",
    "kVBjmI%nUMBw/21Fh<q=n9(kI^].Etu}XB3iUDi>we4$Pi.]xiT<&!51?jN%ysR03dtR+6+<Gphtqt@^KE.s){&Hq=tf.936POtt",
    "bb5eNg]TY[UT2Odas3f:6I4NO}N!w[5ovXi^d!-$vc6qXRxyu4g?]iraJC6T&2tPUrDov#)wPUu/Ep^MX:m$-p>Iq5/T)Im@2idf",
    "lqm3+Mb[>EBKQ*+!y){pS=14+Fh-?cg#.>9@gTM.y]5WDnf.xN<NXu]KCa7Q{2n0Dx}]aIKO}73QJzTi=ojb1N#{T.9f:jj)Idcl",
    "KAzKbu!+YE=CX3)AKzHN0m5[Lin4PJTklvVU?+g0Y$NDfMHu(GY!>OW{p2dT07:PjUfIm9>+J^y.FG]H?srS<7[O-gE:oc2KEfPx",
    "c@8oQv46evT0ifW1yo($TqaZH0.j]Sl+{e{cr#f^EN3-xd@hdGFBK1hss/X2h0z.s=]GMs]3vRaHmxbB/Ly#f)e3p^wfe0sLN}xt",
    "q$f!1+X:5!L/TYP0/nkq4lTx#gOw6*>mMufQu?>yY5Pc>qmrI*Cj76$+IvrP7{r!I5h>T]/m=J7dIA&p{m478Q]!BDtj@Jwk<3-Z",
    ">-8u26J:DX&^1>AJZU[vykCr(lhq8avFGsP-c:wPTsQY>4((2JmZ)0yZbP69lBIAU<L?b)PqHQi]Ytv2><=Q8f=Af*&1rH.}<Cn:",
    "u#7ze7ST6IctP*0{$dTwXcsADrgdWsiCKKofDx2:&L)]xN8pTx575fRiX7&SX(YrNPg[Pmsy#ngXuWl(ekS*&iZo2@0CmiMCM@vQ",
    "0d7#-l41IL?$>[/Z!1$#W12T2@!MxAyXY?KG&^-RYE?:G=M=sK?U:K&0y)>?X==MR!KfTare?LP?Ds3}inYSSF)?FtxdDUtESE>u",
    "^Zfd(:1EYZD[w8Sfv7+8d7xmvb:2M1a.Y=DoX6LngC+gTeeXyJh)4=$np%%SNesfLM:BJvBGmBnyjlk#b$i+S]KLqwh}ZnRWvEsY",
    "Mlm}DJ4pZq5B:N3BD({xQuM+-7-cZD/+[D!VAUGn)N<rtZM1X@iw2bjL.50X]MaPT6c<-{LIp*Q2yhivcLWmTpfEPV9Og[qD<hI[",
    "^79k(3m+yaB}$3tSQH+tar3SYydYTkuQunbBJ+sX=[}sSS]3^&j+d9a[o$[Z/#>.RW>r(RxVUJ.+sV(&+R>:13w+H<eKnTnR&HTq",
    "tc2[i%i]{*}vO3qrL3jm^L%DPw?G9c&CsmYxt$N$ye*oTI.QQi=9=[2k:oTfrVj9nk8IfX:-amyrDWGs:CS0<Y+Afma8Y:}kmwHz",
    "v9RGV0dM<?s2VUZMHb^bw>nyP-K:O.Yd&/*>xlQm?T:jKEhfyT?)gu/]1pTfPaD<[ytPGlNHo>ya*E.7kY5tvIOD>*GJH)YGAdA6",
    "FToa]@M3-TxQRb#{cP?lkNv@:IV8J$&*bqiQuh-{em>0SCe$VqOX%$XnjO>b]ZOzlxU*Ab>6}ki=xd.j5x@uZSzfn6Q.Q9%33FRH",
    "+r7A)ZGF@FpI[co6Now=Zt8]aLa:o/:ry*LA^*o/d4tQ#!]8XBqSFa$/.=5&i1(#&D>)WnprEW)sKR60>g?7yC8W?j1y&W<kS(Fq",
    "rzTBAazacO+JWOv[sbC3{!%q?@jDM64[MAch16J&C5(=Vdb$SFS8>Yqwz7fJnow%/fpbW5MgO}cZwVf!0f7ONO#+q)PD@m7R0Cw2",
    "EU6)I?jW/Zj#Nk>g#C4v8v-o3Yj&JNwrQ6d(EwUyvcHz0gWyb=a1%F8jMNODHyQyHyp!D0rj?-=m1%hQzF/P>Iu.Fvq9F*e?*MoM",
    "0.g6>dE[E(O}c<OS<J+W&!zM&?z*3H@NmVcRtHXnh]Ir4495JoE01Qp*Ko(3PO/}={B@hLIJw3aJ0]}P1t>u!z77Php>yhY[hgUt",
    "<t8q2n8YVCag7QMhjQs7]rrXTtkfj7-CPXfi:KCMAUJ&lS^3UrL391wN6Q+BZ6]+gO)u/VGZuiic6}fp7?yN#b1$!C1n6h*9BPst",
    "@U0O>yv%8u6JHeS5l9%t{=[*xGht@tn1y8Mp?Rw[v>>CQiSYsC+!y.MgRV%O.:+n5KKel^Xk+TO7-3dpJ2ta0GVd:>AkJZgecju=",
    "Aq*G:xN#2NPF[Nr>8.${cy/d>PRjiPr{*QmH+Ee?FmL0d09n0<[Bg87kSn:deBHIvWXob69oEF&/..m7i2*L:CM^[b1p-{COuqL0",
    "(#P*6[u%k$8r#Ghl0kzD4L-:N*T+yGezL8a)J-mRfGRRn0hGF8k?Dx9wdn%Y9a$u@5BMCVg.2s9/Y7armV*@&luijy{}pl4!q@?b",
    "UdtaSJ9Oh5mP0g17[UF9DtBAZMIa.27#(e<tQ7o6Z]?AmMipm5c$yZ?&d:!n>6yEIe^3QqSULSH{-&<7&iR]o01@wB^aFAketZ}h",
    "Kot0+Z)$=}>z]E7R44]qkOINfPZQsxx2V7fJ3Z:3?eQ>bKhpUa9ejIU)G%]Fk.M%ohosVs5kWO+4aERTlIMB2pY#Rs&QQ5lN0AeP",
    "pNv+s^eNQ?(r?Q}dnyt+{dedD.9!%KB^(NGp(lTnFzkvYa?C]Z0}[aTu(YedY+6J]",
);

                const ENCODED_SMALL_FC64_M2_U2: &str = concat!(
    "yIqcU)lKVm&dn%ifVOD4UU*+2Q=}ylJ<+H(<CL!bOCh}A0?k&C@2FOD:u!K)06?TpAeL5^:x-X%cq*l}5!zGmsk8+0A5OAy}E2WN",
    "=(MdHnvt.nu#:=fdb:}+zx(>U(pJRvUc[N0Yo?5h.Ip:}M@P6TKA::qDuG1@YNTXe&3Vi3]M+2YvINiLx.oCgKH7#J6KQ:J6X}$p",
    "Z!1gh2=1&wFubUibWb1EfOyub6Q%+sA5>UCu}D)363/G1k(ui}0c]$N80@[rIQFZ(au#<sp+qHSLgmMo@}9-:)WZZFPBbLGIfTOm",
    "D<tD>1:k!!k{*@)4J@Q4dUViZ<a6fOr.F:(razT=Xgx6(FqG=dBp1{fsp?mukm@xd0LzTJi9?>k7<sg/e{q!ecBFYzRBopOPGNP-",
    "DsJHeBdB*B+=&=:bi#kwMz8ge<Gfcf9VQPYJeLD:fQeI1v4z>.JCpNtzAXd^1K<Lri8(Xp=ih(+/.$z8yt+an2X&&)cp^mj[>1OH",
    "}Dxo12R2OZ:pt$#>ZcQGFsP[bc)I*bfDk+VoT14JQtiw]fDJJ)VsG*}-8k5FNdl+g!q=*TZrIQ100[1+l#Yg&CVR1gqN%oIytcCo",
    "vSPk@d1[0KNQ}]2tjlh&]EhsPU#E*r2i73ANQ4[^2*:!Viarrf0757{t^X&uYK11t[z(O(USMTjZ{(.ARg:{[CLbla&3Kh>+:+rs",
    "]A3j1Mn4ddmHWwKD%(b2@8x7EJ7k-*PRgbMAqRsJP.>+5*ol7v5h-Ag2=Y#19}1bkc-#z>)]w+!/J!OxLsBp<u(Xswkhzsl0jC3L",
    "4V/nQq0KdP*-?}BQ[B%9b/c7=h0q+t7.RuY/L:2rPR?fvNR*iX0ag%Nk55Xk8Sw9q[j8SPH?bFTk&)<obZ}Hi&j*x%FwI%Y7/m/R",
    "&pTy?>><ORmIQY(Fc9FDerA#E*:EP:5/6j=2@!bjZX=OBf=:T+XY]H+N@a0DJl0AWneo#zVmC&om+#VV2P#1rU^tcd4aVMmkbVjW",
    "qL5F6Gt#}mQj*iyxFBo=PBAA:6OZJ?az1cTPFNO14Rt#ftL0?r=}=WnCQ:fo-b^m*F?#yH*-(Zg0gzxHs{qD2JDCSz!Y2o!-tWz[",
    "t/P/cKyb32ur2Pv2MMo^vR}jup8hM$GG}{*KIR:C.T&NDFi<aCjG(]vt+%G{aE?yMJfac7:uwAyKI1EOE.*$a0b1QgHmdc^-8J{6",
    "fWaz$d9n+Tv4h.VSl@7pw<k7+RveD#V(a}rV&-+g^0{m5<LizwJ>f>RsrKI9s&e6u!JE+k0Qz5DCT5S1rWBj$uW7bl=@Gtz&eL1O",
    "0i>7+ro%>6Jf2<e?Eqd3PR%@=TnvEXTJ#$>-cdb6J4xN]?Am/3^K8AoAiZ}acOXiaYRQiy]LP1ssDGL@k&*Cd1O#T(*+Ckg/Pp?I",
    "]]<j9!#i]dz2.L*/9b2mI8D0W2NFEky>3uo@64B15q!Pz:A1e<dc&1?99FAqNIzbKp[@+c}Uv}u}1{C(FC3mXJg$eLXtYo/@eE{z",
    "5Dy6slA!Cyx-dwk*%DLzk=WI<9mKn/bX2j=v49p>FBcWPA34@<S1UPi7{0x[692)kS<yqu21kPb!(o]cRdw8?y*fSE-bCaTfp7Rh",
    "CHLUogj/tnZ*ft&b+rmAZ]d=9hF@1Fnl1SZ&n%8:!>7xOa/ih)7#d)1SXg##&8n-3p%gVdu>7V0Tr.yv]IV5Vt7mRTD%pW93{z!E",
    "0n{Bh5pUIfwf5/QvHY-UkB:t&a%(/[yYOfjB1BaJ*v(/9uj%w6ZlO5#A47E[{+Y?2k=X.mfE?Gl0i{#opskGy6CsFg<J4}Dp:+l=",
    "aIm#)ZgiJ[}1}DZk-V:z:OrP:0qHN}@qqBn&0=CAN4S+cZ%@lA.0soHaB/Y<u{Iy/vZJUUwl??>>?7RL&!T3^SiA!Au>6-Yx!k67",
    "c{k15UY8OmhL4[5ct4?zGqVh({Lxr)Z^p(f<CN9MaGq6E-1sc9vb^Xs05Pu?9r#%oGQ{NQqQgAr:Gbh2QZ[#?WW+u2sJQxLPCW:y",
    "i$?]:yuD%b+hqf>08pc?dY!j+u%rN(H{D56^GLfmo.goUxe+QV65o+9Uj]YzKNpBVvbXD7tST@e{>3gxDI#^n[qcICE5B}DzLlga",
    ")bar@m6+xb>WDec-@ko/7#{Jpr98S7Z<4Ax0lu7>(Ao]}Q*H)I*nJcbV$o1}i[I/[Q]Z{^UTYRAII](}il!iXwi7Tx{9(-Hvcs&c",
    "J7@GGxD^O)A*I^03:7:sxq5%4sYf92Q%GT^3HHXP8r*PMl4^X2kPlHYr?cta)N2{D09kEGbYM@1bNK&vTON)?yLMaS{+&ps0jmFv",
    "k!].1k(u!Uv7}l8*=445<m>7p>>fLvWJi:UZ.qTsXw/mDS1U:=5!rIRth]y(fJ#dMPXZ}Uu}<}vd]&)/+6[7?8JQh$037X[k!k@q",
    "PP4^wTxE+IvO<)7V%%809oD4<f-JVot+&Yf[=wf(GMn7@6NQ4%RUf+#p-pp=1NhWAdQ?OPkVnN-IuZ@hsJCr(x4DmXi&mFwD@!z6",
    "2<HOo!nA8=YY!ur(HTbJBK%kIC/oIxi7uDLlUE#lY[u>V=W.(Qx177t=[K(KIJ$qbD<SlOdX:sl0bLv>L^0[CcrwLT?zare{$==H",
    "uQ{W0lHW94Z]0wWFq*toQvN[s1PvZJ-f3?dw}F2=FhHSCn?!NNekK:dre>GM!&9a.GHUyyW3n!FnhxCy/Y(wFrqgvB+3DX:}sWiu",
    "HD[r)YTV0B7<pbp]NO[-d!yIuC-=)S<u)=EBAFx.}yqfK&0}.fd0S6X&bb3#236UC0e]k^v3Y[p{.9qlC1{#fXCU!Tb+^=wS1mxu",
    "oL.kMJ!vr]+5b[D{b[y0tiSuaO)Gq$!!TE.JaKL52+x8Jp^U*Tu@bSxyIS#lXc/Padaoq5KAL7}h4[!g.KD0qsuO8%5k%BUy@(*[",
    "PBq&7rdPCg3]QQsHbuuLYMvaoMi?7=FwO]XPB=Z*)09Ci44:JkZ{iF)}I7O8{02JX:ipbswd+VKBujuNiaO:C+XIBoJeX4PxSYrq",
    "fFq/S+&R2L0OoYMzO?^OkhWA{un!$1jq}A![eVwVyoAVi3>r}l[xouA(@@pf}F#OGFyu-@ShDu5r8-+ep==q6bT$FvZ$@[vOn1/.",
    "qPlsjKoA!u=@+ObicRBpZ)6Kn4Lb34KHS/M+=a%.M$4FRFdRdZPG}N3XhWH4[>ueG1OR4xD@I904/<tS8?4[6qX[Aa2lL2(FL0Wv",
    "WLpDr6^/edazx}35wjyyzRZ+{Fg=4c]WVe*NJ?O>52d06D3^xwv22)/rbX.Gv1.rrr5%M-3F{YOGEw9eXM2ArAV(yifLQCEOc0&D",
    "-@p[fvbnHTv!FY[[j8F3bI5nDmB1Vth0fJQ8aN&zJeb.zxV2S*kS(zpkYw5j7(0S8Qp538k^ZqGaqxZCSn(do!^[*{W0elcvbwGc",
    "3A@URApq)JkU]iUCZXI.PZFC+Bo.R9HlFmyyeIA5PQUI::H6?/O}(L[7@Y9g7?RwZWdADzk?g$fmO6%J024l3Z(1mqb-F6M+DF/D",
    "M*R*QJi:3T6RU*%q?$9iYEl#ER{NK?5)rn*7:NnpFuW(>KGU#N-Unbg5xdp9mRY#SFBs.$p(rT.Z(e/o4=a[k{rnKW5wHVEng<zC",
    "+bM5^hb:Sw^dfK!uFpQRz28KyU!t$dS3+.hk^@Wnld7[eyfDcKK6fuk*1mEZ.PlsM=$F.cZ8iw6/3/}#>:FUpZ:hZ*dV%5gL=s1j",
    "GCXC!x6u>ufVM.%Aqfpy++bDVTDmXu((83I<MeRtJf3K)r&B0Js%.UhOy>6Vg/>qM(FGW@&0Y3omI0K5r.oa2mb<SFmLY-Z*]6ON",
    "6RyWO?M>x?0q9uQ(W(x9*a5HOA[U?6INs>Es^{$S1t.5ZC)=+#fTW#BUQ^HK/F9MyxE}m*H}jtm@4.Y]9WsYF-N&+PGJ2H@Y36Rb",
    "Xk@[#.1sr.MxvH%U>O8UEX60wW3TU5Xd@t-LN.ip.0inVSegz=w=.9UY4-fbjRY+oGUGNZ!*.d[[mdXDW5C0cI0Ri9!cZ&wOgv2>",
    "u)KErKMir6*&og7Xg=w}2?844k&o<GQ@KDrJjkGzk:Rgc{*Ty?u{h0jFlZqWFpkeBdLlYA4KV!uk^+]2r?FFpv5YKbZ@/Lp=D<A(",
    "Z{R#XPPl.0c@{.?-Xc>@FnrAw!uq@!NT.F6ZrE4$0LpDT18pB(xxXV]SAt!bL7BdR]7F+tUNo>.i66U{&3{p>Fm&GcZ#CFU[%ck*",
    "S.-).y^$!zvc2ksC^VRCSZtx408bQu&3+][Z@L+5GBQzZDd}QDFf:6+8w[h(^P$[Dnf-#7S9].uXvdzWs!Wf9bf@P<hrP/WG1OcF",
    "YP?trFcro>@eWL+.PYnFpb{Cgi7+vc[x@aob^)255v4+s!^6X/>8/v3ing1rJm3.1k&KVm&p1?3BN-}Q]W+YU9kp]90j%F?NTSb!",
    "FySgi1CLC<2U[19enQ(<w@qyc00e-PTImGyp)LoE@b@]u0my[YZ}{WDie/-eiZ36G>{QNnzWSeIW[VOhRv.$8(v2/#B#n8tN^d#c",
    "p-iYO($J{(k+&>w=>7ceHYE]Doy@q1euE5U{:>eZ7^0L!m6P9xu%4wgxTa*8&3xtQgT[E#H(vGVC.U(OHsK%u5vNS.jCoosfSDCi",
    "O$bO?>*^nTP5uSZc>)yV6](qbUR/T#7&s=o4e0SlU^QX%FdRN<9Z=L./1ZQ=yvf&<HviWb5ts?v!hU(tdd[Q]c)d]>v9V3&Fx5fZ",
    "+SOlPG8gJ>.02vxBN}sLp:sn#jrH4Q06}$[H<T>r{+OZdRD*q/8M75d>UI4rIDes)Xt>fz(Co.*woKGPQe3DRFe7z:5R4j41]M9F",
    "8]]+cPWDYI=<zS)YJ/zs[pSCTy$srNPNMC[>*3/MZA5?cWRo-}r*7hgv7b^xf6GP7^2=495y2}(EXT!z8}6Q9aqfI]0fU^z5cjjy",
    "IyNQGH{(>pU>^luxQWNpIHs[RmVri*adj^@Tyxt=(v=Sf+ScDL+Y2?XQ^=uqfTPB{3yw9j=)?ZLr2zyzU>rYjO&zzpQo7aM=}6/)",
    "=}p:VPRp[//X9srlr6!{LRLr-upgCIp^hC5{@Qy^&nKNCVqEBh[B0FxS4*#V^8Dd1&g<ZyH=LauaBiE?9oy>ETsjSxaI}u@Rfv8D",
    "nQyC3]^?pJF)1bGE*y=[dvsEa?TKJdLa7O$LOFd3H]c6yFI-6A+$&<NsH<?-po=W&E5w*nsvE+N{NF@Y.f$raB-eOu.gFL$l70Y%",
    "E{:ClZ)>?E9LJ&kGTq@q*m6xL.6))EWsp?k}yNB:<6MleIwX^}tV8&W<BvI0gtL7bmJk-0ip8ddwu.:<d88in]^Z2a6pSNDN?O4I",
    "0O!Ka*u$Q%C@Gz2j+8}lF3kUi{KT3)q(d7s!Jn1s8KcNLr:g9?zkFHv]wOB!D$f.2!UOo?%8#1qQ+@E[jjdX8Fa1R}LKQj*5D&w7",
    "omN2QUUt]#nMaLgKxm(mikZM=Cg.HkNx!n9E)TGaMIBGz<0K&u.v?eSFA]0OzLXmnDZDI32!.E%:>O}>7Be4OZj{Tw3mV3t.o17r",
    "^9f{0gjSE&7<#y#b=zJ[u]^mgiKR.$}Ooi%]c=HXRKP&-+qC0/g&PiNfGSk$3$+nUN5(B:E9O-QcY[IBiuCj+cknKaGsVYFCZ5q.",
    "1l2UiyvyQPp)Pi=N)Dp5^4A=<hCHqqG-#[sy[RSUZnzBOk57tqE>T5JJ(iLl&u<*M+%1Fe}Z(2oommQvv@G1z{E2SR2:?+UG2cwe",
    "!U]65x-6f&?=w8!{FW[K>QrR#>S!)SqIfhc>0Z:Zb2ZA-+tjtppo@XsX#8dl:UXJhaTWvQg[G%@esl/fusONIp@{Bmg7WIJcpCQi",
    "d$m%>44e(+0q/H8UK]xrn$EN$VyDY+TQ32.Np}ZCMjEAr8A8K#)!79Oi-oa{W6Cj}{}gr8}}q}42mtW?>@%Zk.KOSVcJZM=0D:eY",
    "HL<)2v!YQWBDXC[0FjQ>/XFhzOg6@wYHax!k=OUlKBtD*vaOgq&fXjxDwa4p>O!&ZICmG)P7-l02Wlu6S=5qf?F))ak-zOL/P{{0",
    "ucM9T*Dgk8]@pesuehCysEmvEUt<]6UwfolVToAuOi&u4[A[N^v0yJ(XX]2[<WbPMHn=fafH%tIchelvxB=sJ[wTt0^RIH?m=W97",
    "p$eZ-0oy(b/Gg4^&^0b&aHwO<1D!XS{ZS3wcu?Qpc$(C>{Y.x}&e4&QU^sbrHGjEup%A:*o}eZStyN%hPxkZSqIIf:bYqXQjtOR7",
    "Hqw$=h}CHLsCdK9Lcy/DN7$?#Fu(=0!ckcJfUPcMQx(VsX2^e79BieseBIpSbrhd8AR@OCbDjy30AW61Wj%f}v+h*shdz[oh(zi<",
    "Roy%l4yediLNWn#^9cX$J)[d]fiHw215n!U:)!^-yJJ)%<<xzU>*J{6WB1[oV{Ysax{o)T2Bxl&AeJBk=]mdL3PqCdeOZ[vmCH6&",
    "N1e3%g%lov*%{2JSWF?2UU:)MipdXGp}C@8DAYitOeYtU:G]J[@^[clJ3Rxo=H1p?7.=zLMwd#BjoViUB:.g>%lbz9rP(@!MAq*N",
    "DUJWE@%]2(R.$woifdueTIDN7Q6.*M@E+Y{p%!Qxt.Z.sgTmshf}&wD9mrAL3qt#-(zf4})+=Wh[?g!y.aN@}2xIN.Q&hL.tWf/Z",
    "UEN&JlG73<aib5f7=jN)kIeootVj/&tvx=XdZ:4Q>IWb?!XQ06vKcMtkOjpnn!.y<&@Echjy:n.D#zP<mpD@{oXgKNoU0tJk*.[X",
    "KcW5:.8L&pa}MPB9cHm7n8(-{T2gH6P?mqUIv#)KcwwXqjSfwCcNpPl?<IOIvl017PHdwC/]FXsmcy8NRI]7I{>YLCkUCrBFnNne",
    "gkx3@t!WH8Jh/H:lPrc2{eu8$E(LChV]j*C*Yu.7&HkuZdI<$RRoDY*Z]b>#@M+<lTEqvq!Dp*}*<p?v<z>NvOPoE#&Z}qbrZ)iQ",
    "LsM@.ha<u4qw-F0^^%4]<%%+4x17NI6GDieKs][kZU?2:Q5=8{FSP@DZ^H{YaK#1Pz8>(+0R3pSnsA{HOXyjI.p7-:./wU?q:Zb?",
    "KAxzyW80Rzg8{DY!(#4^NbOMr5)](EF&MK?wsyL4J8J0QM@&<(GvpF#lH[0/1vI#GKq7y1T9#SZ8Q5]e]5>rZPV*L3n3p]pIxB-t",
    "fA<^bQ:zLzX9Bym>dkYHFu/J@Zf9XlRAcxA+>]:IFX.^VdbK*lL<q4cH7nQA0p?{bOe0G!6l.75W6Fw#{-@c*>&Se:)jG<42-fXq",
    "XtAuvZfml]^5w[qv8-U&-H95Q>6nn!qrlK@8x!Mky4cqVYYC*1]jvHb9cp>/3f}D4ch>J<EFsHup=iU:)XW=ZdzRdYl>ZSQiP.5u",
    "4!}wkg}&B)iEy](]Ue:p.5Dg:ewp}]Fg(+[3H.GsX0YDAPLMhzLPI&#gtJc#=tcDzcp1T]1&%uKTJ1*ntH<YQbQ9mN?BGYwzW!G^",
    "g8*XpjMRnWE/?(0Bqbd$O-&8FKv7JZ^<RpX!cxevZYSsO5nLE-Lc[dJ{!SVT1D+gFtRLiK&1DzVTG<W%bWdp.TqB4kJ4]{$0i2MN",
    "RM@YQJkWuZu:R8>Xvew1[o<-sM}jqwb-3VU-4ANO2.oL2{}cvnY6P)t6RUv^N6$+.Q{g#PbUN?]DwSO7($+/FA75hrkWrlV[m^Y>",
    "ehu[j3c!7na&@iEL-4aS)E@oF@!2{s(^a[gj[5Y&T-C^LJqMQrxaC?yYL3orKnN?E37rfOhV}8-00XT#UU0tcHZ>)#<FpRC3w>/D",
    "1Vss-4PB@xy0@>w<mYulh:pAUd42v{vc[VyKpn82WwD[^^{%ldj&P3pk.o6K&b#p/h-vG@^%/oIj-<4OkJFw0%nJ=%m<rdvB4pjl",
    "Cls[T0cXdG}eZO>ihFvyE:2k-Xs5jr0nB/)9*wg?odvbf0RxOWeF0y@4:+er1w(8hwv5!Wbcew&z:]R#cZLoajm0D&K3iSU^vq&5",
    "CbM[8x/[Nyfg*pHA=i{3^ovUQYokuwSgDZE@x$G^G$%]RM3Q/>>]3ClkH-0ksZF%evW}i$kmADl{dcBx/RS2?Gd/USMp=]@r=tVn",
    "<6P%wGIv6iko0ncuVd?=Jp41^KfN}k%j^=xI}G#3vJb+v7Zhd$3*yN7wBrBf>/KV8Uff.S*J&z72FPB=emQv$[gfmbr1:7j{{f4}",
    "&2<bQv9gEwkYcziv8tn[kTmH%T%XlN)91.desc2[]OL<%aIsWpn!SBkLhHF%wMCbhA!iI?9Gzd7]HrQVw(tFssa@MEH^>-nbk(dr",
    "rNH(ZIYz%+qQ5^!dcVZO0kojgGPdK%uJ7i[k9s8Ll.d{P=[{r^n}Muw.H}9^MAl0d->5a.q-$=dvaP>v{?YvZqtBzfRt4I:A7wD>",
    "}1EPGJi9KM!RkkhwG93Y^v.:FH}E4To4N^(=kNqeI]?N9uVn7<<2]K?(]N*6XU7p.jPU:Iau?zw}5+tOA7WZPJwh[Dd{H&yk3u)U",
    "OdO)/+cgMM0.f%?K!k-s2smrh4*Wr(MzGtH>wKP?Mb]%*6$9RCZWIp*idykcy#4/Hq0pnf-GPTK3l2%e-ROpL1>vuKHVB02csTi[",
    "164L1NZbHGEESW*Kvp(/mO}C{K}4-gICE=%fW?NoUW9wO0VCPk9+C<Cm)%*Ik:P>Y{@QdY:wdPavU/GDdNnZ4+G9NA)=nr1LdSRn",
    "=laBE0b$rsADy22Z9aZ+]Ry#&@dC9+--A!!FcQY*?xY.B-T4jgEqGxRe&]<)Vv0fDlY7I21h.Jh&jGgR(l%6NkVDsF.%wLXb!O9e",
    "l%AhCTGzxjImv/ASJ&3TT1^si*Ap=8:3)*cr+@dc[WTT[EWkk(aCbuZnfOQe(aGW4Be5B(?)SFCmYO^rb.)[:Pxx-GDcxs6e/nVz",
    "/OI^Q<iY$M@(Owb!NV6K+9lr#&)JlrhSR-?7]zq!/ZUj<Z?PA6=(Er]&tQ9b=>(jm>kMBn2tEVK}9R+TTB3K5hC3/XOt.MlHqtBO",
    ">MvXCz1tVYZSC=PMoCU&6Hn#*^SM.C.Vp)]PU&-Kiey3!U3?HF@%}=ovces<M&-ylE-EqU2[K410}R{CoLQDrHRI#+V+?11Ztiu:",
    "RW[.I1K!<mWm$%gE:f7IFp?7rKEMr+@kHoK9aBGiI=%*Sy=WJ*8nrmpGL}q-HT<>!gnJX?yqEnv3@C5dCYfT%26nP-^A>&RUt8Bq",
    "/UavHXoQ/Z^/6D.7nGS$AuGY[Fh!04hyy)R8]ipM&z*sI+X*>7/uAr?Xv%/kKQAXOhFRbjF-A0eE<#+UsIB{JY^O[7=czrZwMS-Z",
    "+4D&2b?nIJLe[&Ynk6QWkI^V#(}m#8543Bx&AKpnqvKUX0?BkyA/zFhYK/qJ2h/[M?@owO7@<}pmh=g>CqV&[uI*]</Q2b){Id%/",
    "}SsXt*cNThl]c$&S8lTh{#?]C<Rm^eL2u}CrvI=Grd?8j4-2qj]jf+[m-i9TAs-WB",
);

                const ENCODED_SMALL_FC64_M2_U3: &str = concat!(
    "K%rpQCngcxKN9g+XiT#B&2rxzui#F4>6UQ<UV/:z0o18An-6.o(%c0)PPK]vZ&c3Z9{]^!yZ.&cxG0Z<c^!zD*0[ImHieD?Nekjz",
    "[d9D3Jm[j]Nb25iFzk#zcbUK5G)y:6tiNcz2]RwVAv}*q@)ddGFf${<Act)bb:Wk#Z:y{2-nX)4*i]ZenvtIicB0DZVl1PXUbO$B",
    "tRbxT0aoa{5oe4tH@2W}+>D>90dNII.2tSxM$<1wawEeAPX@5EkV=yE&&gCvPNXBMO>vwy?JK^00f<fqdbU1^nvDvFQ+pgXg3:GR",
    "TLPNkJ[Ut-/UU[Ed>4jd.R*}CXHO(pt+F4ePL${+{[9KeXAnBRtSxwVkNxP=@EA]<j4jV-ltSveJfy.lFy5UQEdRP}mUZ4*@kjvO",
    "4sO%M-*tgptR<qY={%[B=]152tOTlK5z8}89ag8aLgJ@4o+)1U>J7Klx15O<<0YZD:HMAa=)Z9JfLXkkpYP]C05is(+*?-!&3.^w",
    "vS4HB&k3kxfKQLo^5?Vc!x<4nH<4{]c}MP=KMm7/:GHxi82HRL82YpNv+)?yJWKCiDI+Jw&fP-)Vki9@c(Y7%62.8?VkHP]&0nUY",
    "LU3.BJh}4-c)Ly$.%^Mq5dCFIr{$QNfzGfA!8H)5jGS+ty[KLpW0DV7C1^@WiigZ}g*:(UtfJ?RkX:K!eAJu:Foe0cZ*Pw?[na5%",
    "g]RRhGY51Rr*AhrLTksj.1GH{]T0w!}0>^P:oOw35?&MlVJ59h>W/Gh2:pigr7w<W*kJbDfRF0I1oBa/Z*/snWXwBrS6aO5{)F-S",
    "01SkC^G]20r4+z*PP*>rY-eNbg=Y}DR0r[4ZYk/D032c5*)PSO&l*gsZ.>@<cz4PT>!=t(Q=H3tC5ra1Z[jrl5B2#MLTD?hFn[EG",
    "lk)W6VvFJfS{/+%KN<B7.H^JQy}$Rn1D540raV?cc>Jw<2SHCIeyr=E/Z4cLY+29B<Wg)*[cc=]MEK+O{&%tjkPRgk3IZU+MEBOK",
    "QY?5/P{&E8FAErNGW#.TgZrS^aOshCaqGsqS1<$FDb=.[u>Xjb*tG=Wy[TM^2R?mI6Uum^&}gNg5qU2k6@SAyZ7ydP1GFy:?8Z@/",
    "@%)t*D[lUUngS@rpY??6&-@fu2R8]r{6yDbSa=e)Fh?!=FziJ90(0m0u(nI$jO!$t+OqUwJYJ*@Fz(i{g>OSr!<5VqZnd1b<Gtk<",
    "kax8CVh{AcC5b5WardS0+hnz@=#Tuy^3pa-&2G)3[lBeH]M3!w<sRmZK7:beXeo/YjIw!G9Mh2yHD*p5k0xPy<3RcNLd5y]ic-DQ",
    "Bwx]<PD07bFviPZk+#Q!VX7VVoE*B-fRjurp:2i&TGC4a[lpNV5r7OvkOf0?joyH9yRbzoPE=RBFh**@[t<Tzk>U]9SnMR$:D.E)",
    "evloU)6A3xsGh0S51a>fx82s?]V!dln^<xpKqKPMPO?mh<}c%]iaBLNA3ueZPFRfXkS4AjfLDk8PXFLu.1CyNAc%2}09%w4&U*=t",
    "tZkG@H6AC%usfr/f7j[nd9#6V=vTlMFga{u>.y&Pk(dT}[cK0BOeIk/4d}j@nhw5}{pO)r3{ICt.]FH=+/O+Q@c^D+zhr{gpm}tC",
    "%l<>&L$Kzb@cdwr>>+0o2gRr?K%/}oPGpDj&p#3r:]dypb92tXv5GCPFglmpMERV(CU6u$t.h=nQ[MH9D4YA=wrLq5URA8&AUOBm",
    "^:l#pS*{dNaIn)XBte9QRv1Rj>tP5TqY20*RbSw!I2T3Vq1zukp+vgfv7=/US4XH<Wd8UW(vCXy/FI%dIBi{$aL808CNZdP5vRhy",
    "N3mnbp^q0z:qy$<[zPY%WLs}#3tb+<Fr2&8U!@=u8T2Bf^a(]fVoTje^3c*V:Gnr5Zf!Q/C?NQf]SX55+Wd!xaqze{lAP)[Qw7SJ",
    "5?ohhg}hLV:]>nI7>}.0(dBGGsshFc^+J3Vjv*WfGZz4rjA.h$um8L.R0jC-W=3${C{*3-HsKzV8M99?aEjm+Kx>ZC2Tm%{TBEj.",
    "^y+D3ahJ/E{ZjKz042}5aOR8j6LDL>xBqTuTzU!^ON(g)Xqh$aez=V9h#!0lIdHx={#Cyq08GgF/E#c}k<1O0L2HQdtW>JjLOf}d",
    "Ah<QnHdf=U6-N)v7$R.IE1E{5@4<)b0nEQ!9+yA@zw!9<tPw<spZ^DtoY*&rGCIo+KB?L^Xm[QHA5:HM^73M$aO*ZMPYTRkQ:4(.",
    ".#$I4elsU>BFbntG>(NWQ=x74f?rC2^Ro@tZSJVya#}3:ln)*^SY4*VD)>S2oGhx5-.2)qQbX-cQq%5)ZFE#}@biT<aoqv4y?rZY",
    "6G>tq6Iq<#:FGXnX&5JYO$@zIMYHL$k(K+B-4W3hU79Q/.FdBYU/Mb)k..FoDfENTjiz?j0Lk[Q0pG.D}U]>CS0yeLlveO[KFv=F",
    "F(W#DD}wltaJS$lFb{s}KM*w<)2EatH%u)Kp):2JS5zKQu):Ks}1h}CZ]5Y}X]LN0x.y/axX*.x2=2RR04>jZv5fTh<H<lXZ$NXV",
    "js4E2[k}#?8CP-+r(Tdj6S1^xFva1yKzF4jh1#melS]2Pq1Q>M<EDikCU:bVp1D{tv.At!sxpA]Fnjgc^51xq01S+QGE1<RN7X{3",
    ">8b0Vh<.}aPTQJ9ieDAOZZI%fDr6gx6WHs6g#HOY[w8X1WDB.QGYrOa01&8u7p7-?+&}0#nsFr@BvTUuTVI$lXCkNb?+2N^2{Iju",
    "r/vbB25xz=eulMI0a=CE08[}u[]KGqg-.aWXiDU0a]}6{^SI[frpksJ0+4Y:0&$$b!vq&#C9?Eb<n]j)-ccysDrvmi8H?N%[B=1U",
    "Z{!IJ>S2q19={EboTr(}{uhxq>:wJ+008gy9l(pjsocMc}wFzd.BP<6+YR#goLp7-.C1%-ja=(:JG?]bJ@H(FaGvxmSa+V-&ax*q",
    "&*/zH(>h]N@5{BX9.a:Aj50QRdB$*&PQH2o!AXCAHZrGkGB@#5fXmR:Qqb}Q9^:+DzqM3>{/?F@JeT-0kVu*UH}xJEoHtEp3od@E",
    "p]o8-9rGk6r?V1%Xj>eYz[nk0PNNFmsE^w]xLMu^^15+>!%2wZkT0PdJ6IN1&qOOBJ<MKl+*VQ91pl>E).t42Ab#*U5q:vH5@+CQ",
    "(@1Oid6b+GE^1Kf&izaKS7-C?YTpN0@SqTNS2!yvjrS6n3w^d]CT-)<ve(O9(jO-.1IeImoYZJjjqQJ2vl{nyg[sRgv^kzS-U=9D",
    "C*}X+saUrLy8%v*(%38Ba%8SPEL[2^R0xOB7$wjvE90f/x?S=+sNRA=H]YmxU>cydLV%Uv8Pm?5sESFSY^[cnrRo]=+3af}G6{v^",
    "rr0]J!kJM:Akb^zp?ny})RyN<6omYfQ&6kD.fo[lqekyf(}+KjZ}2N?=)zb[PUx[TFpUtzNd=uHy%PV9&>1RxUZxV/E79fu:JKlt",
    "Jj#>^v3]oNH.or)CTF/2xA&Mwy:9HP^KhStpaIL(:oXAJt!#&wp.3agZ=eZm>:h)])2Dn5aPx{6Ffb%#9prQo&2}}W06+3RbZ&uI",
    "i3XL?gtgf?Hr()k=ydx<!Fb*j&buDA*jCFJ>5o=M[o>v21Gk>AGIxlB9cMhj.[ct&71&n]7$)!fR23y<00&([{=fze]EXJPKwS9(",
    "X/21&UO+M%H-$M!oJsbp[Cc=XKO6!Watm]J<tKzem3mR=E&WsQ>DA#gK<I@F3.HY!y9]08@&kxHk0=cXGX2[hcpa=E{q%l@ZsG5x",
    "aG]W}&f{TTtZ5VJKw]*Pv.4]uXp}<-0lXlJjuSi#c}}opQ@I=mXovj3[]9ez6MztX0a(^8PGa${Z.jHD0oO2VAca33I0qMm095f)",
    "BN[UeZ=!HbxPgrKJR3!{Ykf7j8rmE8*4uZPDrVB(mC1fBsx}65av/T0[$?ZpMl@m/J395Lax0+MXb&i:5VRD^TE[0aU6PE#A>oBS",
    "A:$xFjo.9naqD4.=]!^1)@q1lu<]4U04DBhNJx.!0g9q<fV0d?4Lac:p#4$Z{I8X2y#Dl}ESM:E*T.+9JKC/H&0ib)&9}<jwr}E6",
    "->ps{FxNjsZ@cQykM.-:>Ngz7!nc:2lG)1lsO7Yr&eQ@6sM[XRSlkxdTzjB(!/6{?60Ro%1B3&Ux-7MqRy/[Mawb<2N6#oxehl}l",
    "fA*@fZfgg[jCZtSBMly+BO^7wxN+W$ee]G<Dyq&q1s{6cEQn0$pej:47z#9}zbh9lSLEPd3]3B0HvzW=X)TFWA/o3x5@tsa@5CXe",
    "F<8*<MG&=W5i0$fq2M(%E+oE&YWM}{&kwUU>N+0YAmbxi.Q]!<{6gi6Vr+L+FnK$)=l*@Luo1ik?NGb&.9O#6}[&/RWBM6PzOe-U",
    "l5l.#d^:&hg-B/Q>69T+:J?EZA9)csoTNv65iuz}YQ::t^eE-w&+2!=oK}OBaAN7=u>AvI>X66vW-twX&eT)4KB=*Z5{h480mm1z",
    "(rn*6FllXLXr$BL08$:oF^m@i7.zG^:E<Y&LODf>kY]r7Z%U22H?t8F+O)(LhJmvxurq&]Ym]vXhMEt6qR0D:0PCur^V9/X&jE1w",
    "UVk<eawefpFz[qPFvaQNS3i@W*:m[xW5MB9n?or)M[FxJZ(@-I!cm5e>:yODi{Yp0QI$cm7!JV0J^]3Y>UWTe8IO43<E)v5r)xBk",
    "3%d$)OqM/0BqAK)KRR)#>2&ec/Ch7.XxLF@1@%s1]KqKF*!)ZS05B*uk6b@3jsPkL}QQ+wwCn?Z^5h<^Z*m<uideSS^6lOkoJuZd",
    "}ROV[ek27*-4@BEXtFVaipV:QzD==$kTbmYN5EQePWcGFRw+l6L=]bvfX6pslH:6H&qksy/C<6qmG!/pfsd#G2uhE5w.D7P]=7+j",
    "kc*tm+22*=fLhE</I{+v0pQl66.P2ISk4^-p!Ze!k={=]Ftgo3UW<s/0aA{VD@=-I77P9jm^)m*dcx*cM}MnF7[]*1@kd{o}P4XX",
    "vau5RpZj!H}Mv[5pZR@&H[liicCNvffOA(F0c+]+U<BOo1ps$sT}x^eL:@4x.1RuJJ6cU]sq3?goTf$#7)HvIZ[fY2P.-^[O$sow",
    "7oKWK&j]Gdv11lK0cQljZ/nm+xXvC&p#r0I1oxjIu<=x=FAvLd021#^nC!]tU.oIHA(i!#fR-{bS(igC1C0QqaEHeiQ{}Wkv0bL-",
    "7$.ktFrqg(UWAy5ChXK@KOH$48mY):1%iL57XYmuSb1i6fRhfa1^hJ(3cO4p]u+opDAA^LeqL@x3UiRTtrSGW7H[vE]ChBgewT-/",
    "*k[DYnm{5ViqtQ1k@eqHQsv7zC*vi&?R/}Fy(:)@xArisxFH[#u$(HM=4DzqJ0ml=i:0aXeLyyMS5yCmmp:(XNDS2h([M/0UUlVk",
    "8KR2D3<lWDvXW}S1xH5!>{qXf}R?Z(c}wUGLkN2E>8=8{:JXcRbijzuFq([eNY}ES1iNpI>PfreYO.GT+9eL.:UA9yC]yIkCZ+mT",
    "==K}P6W5wFa.7jf{kZ1N!)X%13<hf%]st/5Qs7^g@YyF2FB-/NE.[Mr)1GC[pu?}}90?lY/N1t%*dMv#=$H&<Z)rCibN2p708bpi",
    ":DopZJbhk5KNb0vKp=B{B6upwN(wN9cJa&D[KbaC1+%$fDlE%PnT1!u1k>]FdYT#hVa=*zLMquh*o-GDoRY/}Jq}tut=J@JnjVy/",
    "fT>/fv6MrpF{!@VW4d*[C9Ec-7+88<fe4<&V86GAg%1{{S%!:pksMtJ!d[@CKyhoQQb-fnUa3]PPAt9Ppxd(rFE#.joRgHZULVTf",
    ">8jw{S%GoI}1$&vX}zg&k*2oOvoFM6RbF!S[$]v0ThxVQ0G#q6efH(Pi7dmusLp5Bez8smjFF35bBb(6{@!c2w%5irRXyX%O=M5V",
    "E?sa]9%%O-9bce6+y-i@1w!o@d2DhjwAG%Rt^T#7.<jy*&1d=XL^9-)Np87tRCNPj=HLm8qPP9RSBM^Ve{/VyN)(c@CgCrWNIE$G",
    "=QSWMm@Bf$1CjYUJ?o3fehliILnBY)vOF5X:/Mq(KNLL1gl(ce2q(fr!%IEYzHOME7dT@WvKOVHQp9oI<X^c.%i2@aTS^+VH.tVW",
    "zeVNs)/BMBPUcHKt=Ic]B19f:G?q.qbJ5?>-hoSI:.YA&bz8TL^-CXc+z[>LQ^]C*QC4x!ea6<WFq7A]XISwg)wyNY*7M(*Lx^2s",
    "sk!>E1O7}ADQ)?AXK#}q<>Q?pxfRx&w]{tNZaz)&kaSw2AhuBDKYoaW>-V#IN)zM*2?lD145@6@KY+By{yIeP=8K]fINfL3PLvVA",
    ":GqbGq]^w2qRfgd@dg}i2UqY]2KBAjonkO[LOWAnW.8:SwyhWp2:p3bEn[]--fq94GSZVQA7/gNI7>Tqzy}}M9ub3[D$O9}3Z2qu",
    "M6bYwp!Ao5P7GObym2t(+H94CiZKxwU<vfe8wX4vV:sTmxi&($U]F&?S[@}6Crc+Uz&H:fw3P2$ma:Mm3g8p/YUB/^7HXA0&ntrZ",
    "g1%aJ0i/J?[ud>^06t%1WLeNZT6XJl^<>md^5d[$}6?:@g^n08AXQ$prk/iU6E7AaJ]&X]WrGhCkRf6299JOi&nOE+(oQ$6D#ro8",
    "60$m{h!5w(k@+&cxIe:%fOD8ZBE0#ZF[.IOzf%b.TxC0Q.X!fxPry2-Fw/]2lja]2Xjdb}SmI0ZFobu+Fzvcp[%RH305(+%nk16r",
    "Fo/:e3lyCa^c0%lHr}iWec2Z+dRr-=@=ZVx[gd.RT#hh4KJd:yZ/F{@1C*l}+Se[wmbM+j/&!=$esIBfkTtgVFW2A[6TG:i9OH&:",
    "VD8n]Rv0:C=(f}!-0c/q}7YcrfgA*)0oz}EPU=k)8}O1MQBx!sl3?-Tr*z5@m$I:BDL13cf0(0IO[7..5HTknFLt:GYd1#tkj/Y8",
    "AM!c<ePJbIgkjEAtb%UE@Zb{mi#hA>FJgqaB)2><Ul5]i6nkIZwb^lb<:<^57zgV2wX6GTQ^D4+8mO>zFJ^8FVR>hBLeIVg+Y7Il",
    "&cu.m@m8GBysaa^%nF4)!PAJe^:<2:5^gY*HpGBXVcU?Kk>zL1Z}(IMFa)P?cqh]NQqY17]1}N[*o}{lquU=sTONYoQ*l2z9MTU.",
    "r$kIGhry:IggPSl4nNqAyp+iA0ad7!To9DcTc{1L*1Jt!5f3.A@{ZZ!^7UTPtb!TrZ&Vtl!rqZMvhUX3Q}%EL!R/.R>q&Sch#BaF",
    "Y&7ulWymT0-jmV/^UAPwS=zud9Nb+wmOiM(kR6PjXc-Zfj9(N)^MV2w!qP[X*gEGKyR}*t3p7%YQX!N=dL3xuct?u%ldpDJj]eLI",
    "OkTNAJ*gIDCf}W3Ogipc.]Jh0Sf0mS]ASdAz-1Db2Y#rBfJ&V+nzn^hD5jmu1*93-F^5^0H:0LF:Sc%qwNhb{3Ih@:VEsooAb6!+",
    "X1s>T46/Pud?x.mUO4tSx/}OyhyjN3g!eoA@<6<l)Vj9Pp<TJL?p)/KwEK[?yA6YCQqmB8S1nWpFxv@9o-&f}LF!DOuD6/CBA<(J",
    "(A^@-wIAtY^-q5Bu1[)Os:2rVC%!yv38pxZG(0$RfONGVPJJE3C>]a/^a/5yD>]KxfXTo8uR#G<VqU[WeF+R8L{sZu[E6VSnG&e?",
    ">WNvJo3GvWeQ]j-bL!2@RR2s&u.5x87^@+=:qVCt9kt]Bz[e9Dbph9U/Dg5Q.n%kL.t66Ol?WJzixCqDc!zwjYh(CPNF3<1rJzs$",
    "]PM4kNnj5K<c!JjRSd]}E^CMFmYSjBGzv5Z&2T5QHO+[Lo6g-tu+<oqa@)pu.a3gz3*>)CM(F4H9*C6=]qw#Vr(t.p}O5VpMs7?0",
    "n3rciNP.vbM*vmb2>w($Q(b{K4EYfr{kCiH:SAGarBYTP^fa3]O1@@%?eVWYOvV)bzBWpjFwwALBx?2zOtcw704z$KBCM{fu}yBl",
    "GFh1w7>g5HYFB]G&or6XZuF/+pk0-6DHMeyg.U!X7ZdaS9l&ljzwM(#1@*PbF8:nq*-v6vwDdI2.#<ZqKU#2VXd(+yDS+FP33Rit",
    "V>[LiU]2O^s5w7[nh<?*XzGV/>=C&s6=!??0AC{V]<%pGEU?&6erE#/A?PJ$A2%UC(#qqPo}4+tlrO$8y7.&c)=$=Px)8CHy-1ea",
    "xUln/7i<WWYv2.7D?Cltr!iYLfNqY<6M]2weCTuJi.n+Oqn!IV8O9HA+kuX4ifNZ:RX@w{C0fuHi1#Rhnz71b{?1fM^h7VNsTi8[",
    "6AKpU/*4*q:XN-cYGD9PlbxxXdvq3!aM]c8&V?4e9W0XZwBTfy)9^:PW7V]irXl5u6Jq.eFm]uMieQo<Fj[lJ&iKGq9<Q48>.*T#",
    "d6GO.ne2vzqG$mpZ-SzoA79phu@q$FOnh1^Xn:&0G.q1Kehkac<$JX%xxm2}f[JfmmA9OotPhLly-wQ5yG<9tW*ZRDKFBD>SOAYJ",
    "3$WX$OWe[(FV-B}b8um-ip7]OfnVWwOs&UvIo?6[NR(#RFq?iaKJGHD>)S6QL-&O(eOl4/Obw%yYf7ZeMC9![(I.$<-V@)Be)x^s",
    "N9dm#&$r.6Z131zs{WtQ3}J@NuQUpmG20<tJAv4i@yJtti}DdY<D6yam(X%X^6EJ4cnsR@bEOPbap8z&=G3H&X!)AvSII<zfDc9q",
    "Xqk&4EltFj3RvCl5f/393e6<yNZ{v1a)-z%W6g4O{cOI*YuJS9:fNzku{D+o5j)OH^/htwviy*$yB(zD^pkhJRDs*4^gMKBP-gGB",
    "Ch^B8:1C=Kog!J-?N+KbOy#mNRxD>(14O.x^=Cvg@2+t&.xYM77QVAqGAwbpK?df#eo5-}PUjTB:quo*K9*uglXNM+5]-^[:EDi[",
    "<:Ii:oQU0aWVp{k:FIu?bU-eu(/5jli=[gXdb*:YO.D]Nulusl/XR+7-AqH>1F*aM{aM22XXU78-/h@w}0x9p}u-DJCKqJD>>({z",
    "nlfxq)/4V0trj1zTTzpOxXG<Sz}m#NF9h?QoR%g3JIKAPJ@COkd&BhTXW=cjXm*Z-r+Wx7::tx$U(B:pg[?(Kv4uZHk1DCMIz+=8",
    "Q(6Q+A5Zn1J?d/oQ(qZ{9?%9b&c^ju6C:xRE24Ef!y?TJDupvBik[MOUN31Em4wXt9-W*9kcZ+zllYvxn>MYULTYu2a^i2xU6$(l",
    "f9VZn70f0fWnA8IWdNRpLQ)ha1lm0R6pZtZCkhUjQm&Nv5]O-4A4<L<2<s1S(m)0p&s[[$YP$uqC2[&h64GSt/0{w%xnIqPh@WK{",
    "fbh5-d{9y%[=])[hIIP65}<Saa]AU2/226.>/:KITo3?:fvvo[o}Xn}+r7OBVR.7A2oN2hrf}X<Hl%y6nr<H@F8@^RhAE!JSdk{L",
    "D!M>h3#[W0PTHQV-m^@19zh..W4>yI2^*f!<5=sBX-^kh4@>-o9A8t&<H?9GmMTnShGj}}Bod/z54[M0BnpLO&zNrLf[JioA2l2D",
    "swzWRHL@ZT+Y7LoKzR{1]rYHJI(.j*rYrrd0X[HQSc*V&X?:P8Zhp{8yybbEK[FDi",
);

                const ENCODED_SMALL_FC64_M2_U4: &str = concat!(
    "U{[.DM$}EuS0xW(8K!@my^oU!xLo^%Xhosh1tBb{RQ$#mQDvCF0>c3gszaJ{-43:f!oB9?5dLx$9oeRle1Wr-KrhLasqTp^5C+PN",
    "+Vl%wKu3vOxFc{g<zG*/e$f1dGlXd{7I0SZ{fCRJw%62PWZ}??[Bhu0II5yK@dYsbFpg]?2S!XyJ3cS-YEK^Y62f+@PXQN1kN7*-",
    "veJFSGPcbFPSnR!>Xe)pFddD0&2^CixXyqo1A^LOU>{7p!duaMKLEiaUX-bUGY@xF{-uU501Q.APJ7l&c)U$d}0m1N}U[k)@*kb$",
    "WM+/%hb%W>s50G{-gc$B8[n$ZVqNkRL8#.coW:Y?p&>.XGX{f21o(j%mZ*1z(0aLdL<np.>&arWXlu^j1y}txbjz[LoMYbOl[9t[",
    "YrU9[7&zNJ]Ad[5C?SfW@*@5YX<aiF0c!NSkhIdfj$K{(w-sB?t->I?8quBIq+f)4mI%if+S)R<C7!*qZ[2a1[CLU=k39t0!o6Uj",
    "C:M9aFtHV9&7TA?7{FO80j)8hGK1^B5xiM&d0*@!-2VfEZ*fsuXp8dPFnN6M/.okC!AdJf81lVKM$K^V>U%1A-U$X!Sam{45rycW",
    "A]34Ag$W(5xOrrDX>]flruHFp0Xp6e]6.w#1Pkh.iklK#Pv^C-z0^/S5zo*>lmQ8uFw$^9F=KPtk!DqzT!1gVr4s>g:xT[wsu8LU",
    "=mC2t5g^Ms3y^N0Z#RsPwQT$HF?z<x+QNl*Sn[ZS2&qvuk?d0?nlWf0Fvr]&4QRB:o}D2YK[KjoPi-ezC4y/&!r0o=kT{TAn*AyW",
    "N=PQzv6AUz5p@uzN!@L!1tVaMp/Ghj0bS@xFdsxEga8[i09m2#&^*$<00J9VH*ScO(#1aj2SGSkk2]ziL<I{y[]fD05fWu5>W8xA",
    "l>@V:Kq^ZM*wO/IZ?zSq4X]=iU>RA=*)+BVKXwIlOtRYqTwZDfI$$]pC5i>!*guFX4.=w=xN!O@SnYksCYE&rZoVrM+>J}R0et*i",
    "sDa^]8DgihaKOs{[}%iQ({[U2KM1!Ok0/9fie25Vstts?{PYMrFC2:3FjKDq00/jgr=^ola}Gn*)dn9c-1l+0i[1)ef9-960k?#7",
    "4f4?vZ.feIw=[eU).jDtaNy#0<NUY3E8HACw*2[)oL}ISaGNGm=)Dxrnm]}a@Vx1<0*RAK&g3<Zt&)d(.EpN13<)rOS&1Mm&hQcD",
    "2+XgXb89x!hBcMvsoEbpip@]TlC*mCaG4fhuu2?QHk%!EXdK&^m^3kNev7TbdSV7-*xSLEk>G:]uSO)A/7%.R32eecsDZ}x:JGdF",
    "5[!O+S+dLWH*[0fe&P-x:RSI<(LLgp5iL.gVzaiVi0H3VUXx)}Xzu>>0c69)w?iU**{5rw2kd^P*Z]l7[qbl1(%19=sz$(nTzWvq",
    "za=lv<1*?M=Pj]^0f^ZKf[-p^0q@@ZfO!QHW+/MFc{Rf6mKabgjN$0=02:3xjOKYCvPADw=[MF4=rmA=XyQ2WfGNg^{#V6H2NA^X",
    ":qpTmNWeE!SqzmYFDaJr03k(o>iIF0*]Kw^!rY:umU*H%Oq$+9A1CV9:J-v2xB:)Kd.sF1g>)G[5GJz:4-dl?X}6um:l/xXI3n<L",
    "[8qbA=0kq4p[4$Tgwyo3Xt/tq7ZX+Ry(:9fA61}27)xGZJ-9o#06V:YUWEsv001b#Z?=w2>&Obx@qNc/e^xAUCEQTl09y2HA7Dqc",
    "7[!)YaArTV}Q@UE0jRBdeL=^q}n1*J{2k5COf/=3-v:9-A8MtwjGVrgkWy>id9d7O@9PPtJ30hQH<4(y0nrpubPX2ue(/<6U.nHU",
    "rpx7+PZN[5t5SaTu$uG71yBLW]Xem!y/(PUD>^d8as2HIC+#}60jwE.u#=N!{&>VfLPVg6!A/&wi^ME)-16svswVN(wu))LNXgGY",
    "1aZhfONo=bqMdduC)QJnutS#Y(^tQysH$YGHmm9+05wsKjMSxL4X]-h.Nw?{.TYuzWO5OzZ{RlCL9%Fa:Mrrj5r}=f:F0=uNHbE%",
    "QUNxj:8QmOS02-a}OxCIZ:g%5/ZxfXwqLVDm5pqPaC8yk>L.o(91DQBPzASyf/63G7.XwZ>-sAgqRp]XS0PW<7+W%lZ+afKSkDzk",
    "^V:X]^Ol7w]6t<fL<WFyXzvF7z0^&TsCW8x7}JbGd6GWmbdUuHA&a&fnp4FLJl4:bOtsMC<G:I6AfpgkFsaqr?IN4JGV5FQ5>Gjt",
    ">V@ev5fSTsrX19Zbto)S&CKJpAchWPXwoEQ6:J<mxV)/{oRRN0Z@u+]@hj*65kHoUG6{@F5egZ1[yahC=6ujg0@<WZ0j)9YL^0tm",
    "vM}aYhBaKX1BtEePN93rE+$grKPY[>+b.UT[BKl7KE6W{]Mec/rjR?rvZkpkj5y#4)9IMxw0)J:sZ80wSr<]^c#lZ-z#)15<MVa@",
    "RQ9pC++oc(N/wzYc}5{9(p.b&IGKFkellY5!%#E$Z)Og:IwK=9PAqga7[[e@A1SHv(rS4947bDofWL3R0=j)I!b]Rj0oR6WBBN^-",
    "]Wr@R+YAUA^7Iz4N4umI4V/2Jd3}.jNeKz9Swoh(U9$[B/>gV8lA/y.}#NtyAG)YvKgj:z0o^?4cu^XF6W{RR&>8lOk1{[#0qtA=",
    "e%A+cRsA+i8JqRF&oEg5]P^CfTy^:h&@u[V6AMVChhNsaGYfB6aNaM)1zE$KaH*5/7w^gb5l36Kc>+G41kHk@Ug.NyB!d6@?:B?l",
    "9CO=<i0^$@D{(TJ&7[8Jil!r-D}o4aM>nZHpl$Q!2+nN<=$)bNvablwKGlhW98p&zKFtue)2HkwstkZ$H.]kq}Oh2M/ZV)y.xld#",
    "KM)Jxp$$PPFy&$}:tg#Mys?00>9gS:MjS@bmb+!(9S&&m2.9KKJ}hi[9=R}dmjjqjkJzgBVs@Ws.r^EJ0W+a.tOY9c9oR<=OCS(g",
    "0atgPfQoC!Fus7d0a+1HCa*(UWSh[)t50W[R#%3j94r5AJbZZrKrzC6FAzu]:a/C:{1lmRxNCH%*/*C=FzFZp&8b!4SnaU<S4m+T",
    "+.:?jIO+Q(+m=3Yh3&Lik0LZOW72hhPU?!-aIlyu[e#qQp#ycx<sle5[y>]BFe$V6}P!MyM(-K@Ow80^HtmCB5fI/L+eRmX*?C&.",
    "d!Gk]98pPs2:.v24dy2QZZw%Ytf$8G05nx7{]e%gdN9:ysITz?GAQ+S9jXFJ=&.-WUVdZq1xs}Pred}Ed/>?$x00TGvSNNzNm]3y",
    "H@$7T5B!z!LWBtK>R^]N>N!YC2!0I(/Mx.FBK[O$Ze$6xb4)KYp]AzrwwG-U-aG]Il@g-:PDNt#X<8(Kg*pv3ZYz(@mW83e*dp[I",
    "3[-hR9o(a?M&(uXVz<#&OhWJj0j!AbaN%$AQcUlYd8$8QH=}E}uiuC9dY/U-!d8ZDO?#P[2Y*/}k2o&{TCxiRZ-cAtk/&<+Lhi(o",
    "=@<!<Z#>Zgv0!Rft-$yoPQS*g6S[BHm3w.a}0(qnWW=S@^6DtcTK:5T[mf6[@]G?pTDr.2y^erv]D6si7-T0#0cQ-wqI<A%RR*=7",
    "Al125/Q{6{rSQc47@vW:(k+z*(iOa!3I1ef(zRkaPF=Qfy&=sl&7:l:M)tCu!>NRREuMp$S8g8*UU8w/)7&}u^4OGyA97=b5djlb",
    "Jxpi82BQaQ>j0):S8G].6Op$JL3N%#&pZ]#PZ4xC0f)1oAWGqG0kTjHORv+vqJO&h0i&&uTwl1kx-6LE02HeDPRW*+S28o)asxQ<",
    "sIAs#nnuQoTs0OMCg22b7?q.(IvNhg2WlyN(zp)K]To#E4MqV%k?M5$sw!L25*JazBuC&4PPP^F@0ufjLZ8Md>XW:>y80m5xQ3&Q",
    "KO!<!mUxKgeeSorP-7YI1?+:pfT2W:5G!9*qN91gR<b-zy$Yy-t4%kmMFx%5av?v6Z:o2)U/@DE&flKu0x:w}+Lk>!Kza.T&4iG(",
    "0fDMUKI4$:68)J%[c)2]EO]msRmV{)6Z!/h^XtLXA2af<2.d1XJano?=#e$Goge]%)42p-!mS[^qx<1wCefY-HYDbw=)G?%J44<(",
    "5sL1g*cQ(:!?@e{[k{mmDn2lgJ@K!@6Ey7G4KgNePWTy-Z$sx8OiHV1mW<4:FjBQ)<Jfu]N1%k85s[kiZ.:*Gr7rp?A9-@nauEt4",
    "{[F+vN37@xX!zqhfETuxe$[W@Q64KhFb{j]x/o#{IY^QmX-m.z%mkCsd[MLK?INf6+E#(EFAs:ke&04jDz!V]-1?5<wgIgNFpC3<",
    "uL5+>GvUB->Vn&?FrxVubb>1AH+nhhO^?Zkm+c9N-x0?d((KfWo6C3y^7/PzW:y9#giI!zg7qKybRP!y&-3EM7C(-rn[cKp)):4+",
    "7?klZaDfggkM]E%([3{PLQy/vU.IWhZa!DT&7Bts02xaSFxLX&tY88Q<IQA0/CeA&<zl$xoNU}:xP1i2Dk9i?f8@b*2./&LkjFFk",
    "QYC^8N/x3z7]8BvSe6U2U-*3t[eIeokkKw7}F+@qxXWk-@cae:u)AlA<KEH0r=T6Yf<ZUtb3($q:rsi(ZyN}tH2P)hsB).tA&^IG",
    ":Ic}47$B>-tZ(oc5pTF*ZI=t1pWYKx94iv3S9L&PQwyy[ny/(o5o&Akk+)%lU!zc>Htm%9m3X7>Jd!axBzJtr3olUJX#Kau=j4C2",
    "[N>t8n=(pZ]C2Rmd1%0hsmr(qRP#DWXgb>t&lu}/N3^Id&af)w0mH[V0g>b)3$mEBPG3]6Nc#H/4X6Vn44%jQB{WuB&8eZJL3<0S",
    "K%GfMqARC45B!:zoS/o%u$0#!sFJh+kSk2)UPU:l6Mf?ZQ?nBC]Xk3ZaH&WVqF-%d000i+X[Qa^C4d%AQ5yp&KsBN2qh6UpC^fp{",
    "fENvzvgocF6QGK$nt%2hk0L{Tu>-HJ-1#^]H@/!kbL-/M6{{pg=)W!:u%mwewAEI/:yviyew8MT8Gs+2zIn87d]4IaX/[^Jk^$t/",
    "9&=iM05PUDTtZZ7GOYPuk2421.K9i[pkwCNM34w=3U6JcijZlCC1I3-Os*50Ad)jk7cy6WKv%NQHlp#s6C6a+n$2W(UUq%FM]-8E",
    "E!gopA9TwAAhv#}0kv)l4c}aXieZS%sD8Md:zkcCQ][b?eo:v)rks}=BOsVR@4tZ%>/>IsxS=.CEd@T:BA{i^+e{u<bQRM+0u@/4",
    "^Z*TpWrc{.9km[9k&z/gER+0c+KaBOmvjMG-3QZiXf0Q)?5/@!h@l+./+[=[^y}9yX[nul7@sXmv8Xi>syGfiES>6v?oIfCWVYQ.",
    "V5y4[eNpC5q=K&4Dq.?$:%2z2454<UFF[RPR3e!vj9Z@?>QH^ul)yFJO+&Ybn<F=T6E.o</owoVN+em/(nkz5{(gR78ScAne&{zN",
    "RjaFH!76*I2I[o2KMBG{bQkliCKTd%Kt!TM90@QUu-cQZbVF/@&)9y]Igm]Sb(0ne*0V.h7ONtjdG^)TI-)rq<4EGU/?^?}kFrh[",
    "<<0}w9T@g=2TII8WQBG@?7l/mF@L]bO/r$V^X:ljAZ.6l5x1/pXook-=gVB%8XI+Qp+{teF<aW#!iEIcvbVHDe}U*>&c>F=-JwBs",
    "eeNAYxU:QgobLPfm9YnQ8)FhkT$HRRb(/LYPY58GPu<KKSpqQSSRh]Ev<RQGfZ9smv:A%@0ow#1pfdS-Q3mVMZ#u>dn7)*NI2Ng-",
    "68vbw?umA7vpOofyR+?v.mI0RGsgiZ]}d46QyY%V8x*VCzBU6#KCDB>C+f%VV+AcEus=L*m-k}NqWVeX&1g)YxKN{OsQ3Dv?u&%u",
    "^)SH+}dmvQ+i=Y5N-d-W.jSl>=A6QJn)cH8rd#vaUZhT1{BT&Ta%.g)9Gp9m@h^uu8c8ISs@j?m}IjZRz)ssD{l/5FJ7*1WM=4j+",
    "c%RIu9>w>@X>j2cxt*pNxBpcvBPA%QcGe=8)2}ccEyP}NG-Sme>thC#s&c=A:q+sD0iOF$OCkB+<31FFXyE*%Zx<^2S.0</px7[&",
    "{KVR:aGF[:VLI^**D.gudVH&1G4Oge:qX^ak]agGV#3*E>=?cL]eXLsWLJ=O5hEj#+%5Q2N4>6M9&uFf[sHPkS9?%bj]M@}f2{O}",
    "d^N<l@Ya:Ty[wKc[bGvJlR<HwBj8E2lPeHrwt*Xnjm.1p0?>&xI1eN1Ns#wtZmIh:FAT*k7$&))nXfw+.3q)k&?AlZ8vzM?8S3:)",
    "I:V1a7lntTYB=Tb}UVppKyr&aPjEEQo7rVX&6:Xg(}lLbLe$N9iTVzh[6nzR3#Lh>)r!9i0OjFs>olM.N8fO4]0r{iV5^!RjM*v<",
    "3]l9>G&5HF{/H(!w?*XRI(KST.#$!dO7rAWJ6Wt8Jrgs/C9T^BV%bZTyb^+cK&}(hYx#c4IFrwVW8WkJayinQ6?j6y*$!489F4GU",
    "Wir{Ykxkssn2Jb}1I1@QmLwglprk:^s<A0?k:i7$Q#^!7aW1X%sZs6Lq!nD.*-xiOjTA[oQ[8r=]3vMOX#8X}:V/wY[u${5QlJ<n",
    "[9B)Pr(1C)hQb#2Y[@ZL<p8$<B]hRi1b(wa-zJf?wE]V>q-%>sZ>[HxiKx+&K1M.Zs8>-ks<8]uu(JT<xu=k@D3]?fPi:&FSt=I+",
    "cH2>vr:6gJ<8.0e8%I]pYBqCo{{<tp83%544S9^Y)WrWW+KRI]RROn^Q}o[@tu$1qU^iVgiy>mbPN*4PyWH)Xut:jnrSXsC5b!sH",
    "mG!</xF=5tn.{)t[/Fi5<E9OyJ{]d{?-pbsQ6p3H78tdVCd3i<-GE<EP/y:?ML=a72FmNX4jt^LZ+uFpxsa3</>z3.AA]=Z2hU>h",
    "-Wd4!>N98eHVem%)MXm@GRRm8u6Q4aD=VmL=n0?Z4>kSmcs}.vK<zK<@X6}Vb0}kKw]!c<(&qS3F=pLvTj&7/gt#m$oJRJ::2it0",
    "[r>sY[IN./@{ycd0C-&xa:ZyR[+[!qyKf!XrUh%TIgfk7K{&A%e]{[N^l=qCiF.aTNf6-VP.T10Ii%X{-dkrTupbanL{-SEk:.<u",
    "!+lde]ELqLv6q)XSvWXQq8c6PY!3iG-0D]q{97c-B1{xMFCb3/Y7HV&9Ja&V0v-EY>J5kGlLEnH7FUYe6?>z^&)QGkCT^DLSFDQ0",
    "?VG%)g8PEz>DY+8P{X/(Mnh#laowYgqeMcxW&YfBIf1[Bses/&aRtK9{=QdvP?.%U9=AQy%1ip}8eSr!AnGOrRMh1d2EMF7p)8bG",
    "t30#6aj02j30yGEjMqgfSo=u8@gw+VTd$/(y.sPGFFQ{wp}y:j!C[@1ng*QJ/%<7cI+:Qz:pehm0?2upHrbm^efYg0q7<Ald{JK$",
    "RuETEL:]uG:x1$E]Avv>Sq-y4mIGJ107+IH-k*co&6PdXu@v3ssJ&88KsYh2jG=T^Xg5oe-8dHr&fTnMFfa3&7aFdOd@L0[2O=Wt",
    "[{[u=y.YsJ<*l2v?EMdu?)Jx}ODyBSs.J#jHLP.f1pSsm?4EN/1#VVI@!u#T]VNlFhDrjgW4#tr0bf/(Zv0hUv4m?wlK%?]MDEMq",
    "A^W{Ls}3Rf){m}o.p:x6WiaXFOK{Pf5}<vKG%{&>NS@!+p@aLzbALwUi:%OMrK.cPt^N{5gcMF/u+MPR:=m)^i6U<9TzwcQ!N=-:",
    "+Im.lYR@.hwKQP6PwNk)/]EQDN3QyUm5%t5paY>/Ss$BU]t205)*GrjX+N:tia^>dCM0aT*Loi3IJ{^$Hb4WJ[-LJ/l)IAs[J/BO",
    "ZR)AwT}Neou.66*8+BhC/qY-N0=Rsb>P>%F]V&:^Fs$vPCYU=beX^ibK([5PucLn>3lRC{pWK7OscU:e9ibU{eKxDLMV213pYJNv",
    "0s3>k^9plbnzRR87J?F${I3OtVD2nCqOx4[PQ[/<S/$N*Qlpl{K&(K)[el<w8[I?c0togZ&@X>!p@CKYDM.YtYB>:?tkTb/-ak]j",
    "R4?Ax6fWH4owjIL.tYSZW[w8ilpZ/lu^UQY>*-c^xibK9dV:>G&vMB>r1Vt$owseA/>%8t!V&]6hPK9b:z5]jJeBk>R>86}6($>s",
    ">9VVxLhbaY51wRx]pHj<nMn8wmDG-D6FW.xiMcqe^ok%581pD/4Y6R0c^Qm9S!<[yl:o$jI}PWCU}PO2D{AMM]2Co8I1<!gwcN@P",
    "xd-<vX*q8QlI!L.s}2I!Kl>EZllX4}/+wkn5fWr4Fp<jJoibmYUmzX*MYbm+pW+LK5@P#dI@])XHbNvuNJz.$BB$WI-yt*2^F<hg",
    "A)RlTgjLfB[75Ey}tg*&09Kzc5CK$]AaZi9fXwt0XCY#53]r#cl=%)x*@h{#hGoLEjLjZsxT/k1vN.(ZFiA/(FlD2pk>FOJ&5T@M",
    "6G<NLy=UuQ5rzpNghWB.RP&nT{=]rMA]GOM/Q^u4iJwla4Y-w*gZc7S-U*G[)C2Du8^Pd3F-!EDNR!-Z5oT2Dp]m775edk(DQ[yR",
    "!l/:I5K(<=(8WGvy{wWTI-lCQDkQS:D#H1qq&4}E{oZFQ]>]yC]1%VQ5DHJGc$17fqg8fO:DM=>oj[1lFHIK4P-$0I!hJZSRBU5T",
    "nfT6S)4kJpFf^v>@TGZT>/<w%+Z7ev!lro]0.scw@ke[a<vFyC[s!(9JdR)f)N8rmPZV{kL+-mZ+3O[$3lWFNfk7c@8w(X)02*>X",
    "Tq*{#!y{GT[6qOg<qmURSp5DO)V.p$)]!j9g38@X-<>4(vI=7e^sD?YGB&@6nRs/wHm{xpD}L3-&MK+OS1GAI)Z%k$6@DKB.sXeH",
    "uB{}o=KC$0)loF>T-WA4wTyrFZr.XJt><z]HO=PoQV&*-Jy]r$2:Hlgu=[8o=r?H7Bn{ULSUV<xDMiKf>Pj(XF-a?*pKBuJ3/z63",
    "xfNrGWam-h}H?>Z1/Y>Z47%)0]X[]CQ<N!T<kzBJ/QXwxFzq/(DW>m)7iAr=A0QQ1+7fQU=#As9AzqPXqc)x!>tD#D-{DZs8{OvA",
    "}QHfY8yW^iwlj2&i00{gU9dZbVcB4Db10!zh!eTEFpp0IIJ6<n.D1AoV:?f)T-dKUX?y&.^vWK7kyVA(J.pGUJaj(Zps<)cxRo7N",
    "v0BimdKwH!X#fTlWbEB^lCb0oJT)3(63JRPnetk?=>y6a!hOF.*<=7ZG33l<K?vQavysiunTQ%uA4SYQPV[W[x)*G9@r9/#SN^1<",
    "Fo3::YPmC54G:75@2l=)@u^iidKiUffse$/FlI!iD938tBdlNK1#b%rEc4Dx>tT=i9P%-vMhS.R6hJNgg[]gTy#P!9nrF0RfeVLt",
    "=XD!W+o&=y(F.lB5TVzeGWvDD4BII}O#2NM]RM^}6$D[1Pl$Ju4MjUU3r#ZTDiDb?giSAa(ofIV%5@KtIpl]pl0WUTBdH6eQ5l4D",
    ":v59N7w%ocZD4DvzELxny5rYIZUZF7077kf(QuYAe$F4=LGM]?@fXA+lPhUxN2^]NcqfW?cLXOO6CJU[}TXF^Q)v{[9#Sr!PL)Dp",
    "](Xwu06DVk?MP48<4!q(d2]W?8t?*}f(GN%x)nw%WeIhZMrhewam&asN2LJrLYKdx",
);

                const ENCODED_SMALL_FC64_M2_U5: &str = concat!(
    "+sKMPV:p#vA5V99KNQc2P.)}mc{N2X&c:$a&h36emTP.H.):8r2Tl>MoM:n%J2On1cBZE$jC}lzBMTsJR(n%{/@j$+FiYwZ0{C.e",
    "5mPJuZZ?<aNdDGNoLONXm2+cVuJ*8pD6ej)xo@-=9yBqILV!OXUOcPwfQOs(9Z2s[FeCx00.qrmjq<r}v3#nMELP8f7+0V([c:a@",
    "{=!t-&UG2psq{ql[k5Zuk!M8rL=7VEj{!j4uxQL^3oV:1*<RpE&fo(@0i1m7*a{vP?RMNwEUbH3>TGFNk.2d.D(r}!Fs#3!eNFMy",
    "[iN1ll0W3{vajFRKU0xzir0!U{mGpqV}<:^!n=ztAj$RJ2WVNPt-)]b6RXyp=P8rBM8uD6n}CTd+8UC]GH-R!Jil+Q{i#zx:xEgT",
    "IuZHKyGg%f+NFc%:yD^l.PnsZG5BRYrp.m@fU1[Yiz#59R#fAbv6ic^N8a0Y03nWnU*r}z{--x{sv*m>>:nbdbXTT1r5]&Hd+R>Q",
    "03}fH=psA*GI+:.0fqv=>Y{:MkZ4W&ia{#e=&Cl4H^:elH/uHI[lGY&sqqlc>.fX-KxZ^dD>(JwaL(.7:ogRri@=R3/DY^%^c<4V",
    "3}}f{ap3NYC/)!lvK?C)^U+ABH02W[6mZ}m@j7s)55]wGGqTHsOl1xccj4x1S8H#73j<0%OxN+wg:uD)HKnIOWm<-pnkv5+-4F14",
    "Z<S1vu(FvzZ&9k^tYT2XAbajx-.D&L{=lhN*jECW-6*1RPAUwe5gkZ^0dH$eHxAHG<EIBE&m*EW-P0%Ye$r/]Xdg76(Hqx#enZkb",
    "i5F]z![&vtVBVO&N2Ax9f2%LD&fSfffB%eQ:zz{geu)U/DmWyQj@ZpX&c7{ep[jpywztS$FBDb(H-<V0ndFJXKx^BKi?9N>k?p$e",
    "^1jtxBozk1uC:9k*!:MPYB310KuZYD!{=:AAYPZO:P]vu^@}O4nI[dB@.#DO<d9Xz9KK+rUP=.6r98@gXvc:$8yrFr=@jU(zDu0+",
    "tzsH?qZZD=-[e0?/QrL*+da6bYQ&ot&2}qEZ%2Y?qMe6T+f.L0KuVsIYM*Lw0pepHkc0.T06!E==$PKy8c/u?t^!caB}l4ouY8!m",
    "oQaUK*ei[]A{Gjd24BjrfH7TZOzwAM0hgL$u}AR@FBCk7v4id/g]m[0oGX}g76q:]tUkk:=u=4{WG#-yPTQZW90en^03/MF:Iu1Z",
    "c]xR<ZlEA^97a+WX!+faR#%oq/WI=w+Z{?FYP&EW.2a6(D)NPiu<KXkHh5n63}&YBK#df(4CYz@>3FcNOZ)i(4XJ{qZ/Z9sfUUAq",
    "t0Iz/^3=EEoQATWXq)%WAoirFSjA>WWO:&NoIXky6Vc$n&j{i[5#IXxOpSDdk5>kH=Xmj?tX]In7b<:SkNsZfq#/9BAfsFtqzWG0",
    ".5KJfsWW>PuXt<Ke$(98OoTV*y?Qn<oA>g7H)z!?5eip3Bt7{5j@8X.&ZR!r41(CX&gcBK.0WTOFsj3Z6G6mhap4wGZZkL6&p.-m",
    "ei$ptD%)KGioMKt4P}{T&gw97>ga#BoY!2(1wP<#ed5jq!uog(7eI?h)F@rYbRj<pJ^*lai@s[14![}}BfLc@?OU{Gk=ZFuhywR>",
    "Q<L=$N#1$@Kx*f}2VsnTPKYmV:Da0DsNS<+81YmuVWNr^0C<v5>?1GIC<.-F6Qgk-Wdoku9h)?&Ajt*ep8=tX)d=4y&3gDG^a!/r",
    "5u/1}C:V:*jO(y$tataD}T}M^Av-0wjF#(bBL:R)wS0rENIC!mRITO3v1Ub:Ji%2ac<0$[nm2N+0fZXKxBf+.Tw)(^[tE!GbR<6%",
    "oT0f-l}x[.vXA+mX*=TLv526Yef:yyK$K[<80yiFAok?pCVQ}d(#}0]FmuQjZ.8miE/s{Zvar9c!jd6-NMNdOA}[+fQY#3azn{Ca",
    "G&=WnbJU323F4*l+00p1JWwy5f%h535pzydKpPI$ax13cvezSv6Y*n%2+&z7o]u0=}0$%V+&2ec.1hh4jvP-xAe4BRJ8W3{sMu^3",
    "N7z4ZXgT1e<EjpQ7(iEJ!*/(D[=[P%KL$&9Ff^K}5JO]o&V2P}HBy!r]+Gi}gb5ahnn+g=*>u#^1G&0yH?4Qv{%x:r{v>HG7n]8l",
    "04R2i{[rZgYJDvuH[SY?VyOA!u(KHbU&@p6k+C{adI]w6r1fs2kVu4wbQ$Yn9qiWKG-x=tp&2+/tS]j5sA<i@sviXlgf}b$C^rEB",
    "1r(a88K]{6jByK1a8oj/9g{$foigdr7bvzPA1-TAd#C35yR=r1PF.J/RQ&=2>WNo0sEGVit4vp&5<SJY?P?(VqJz%l/Udo$MwyeR",
    "{mXMJ0gFOKkYdh$Kzmt0@w$?fD{BeZtj*=pH+Bcsapf!=I1DA:06?F+01<6gq6O$5B}xJ{Y4fS&z&x39d<^6^gnQ}.y^.b-TwlO{",
    "{?*l#FtR2kRwWv^>b:+fJqB2DJc7#hiZ7%9im{$?@4&<^Ff=}.5fgp7KJ@XCfB%NlDteZevc9iTashU4{(&Fa[o%%)]8bY8L.Uym",
    "5wtCk*^8Ppu]nMW.U]]^?TYU?^bt2sZ^-01cVT1pHIFuMS$24()lQIh[T}u%4I#MpOJ[R/S/1OP/MB??lw8zN9<*+!p{DO8y?GPT",
    "cN7ugPSK8m7:8EmxDopIVs{[:Z}X:rxCdzWg/7x&71F.=dL*q4Z.f{o45^5!TATlGH#yIg:s4P.0eJwwBC^YznP=sy0TH[L@uwhN",
    "@iuKvwnizNQ^H}v23YS>Di]!GC7WQ]Fvu/q&nQiq!imVYBJb3dXAp<}CidBvc]H9H>N(uc0/cwgc)mP-1uE%A^1=)fgbTS@t+Mj4",
    "@}Bjw<#p-4-902LfJ]^o&jD9OROy6.k:qHm@8:1-M>c[]Z+!XD&{lDOE?G/l>J^aK8%Jo]..Q)GHdus1wdGhlEMeW!p}faX81N:v",
    "08GgF5y?#r>SrAuMxEE}Fi%&gs*0cmFz58T>Rp-50gt<Wv6hNN(u6r{Fn)h6-fb2JsXTKY057p.TF6#7>Z4=(7)%Q32XJqk.zX%r",
    "ay!?A^*o*8^O0?dvFHtCYt]xl&/h>V1u[AJUWA:e0kvE9262jcp=]sMQaIzsZ@+9s}1d9uDs&[+{[QP%oS]{e2XpV.Z>F@8/QrL*",
    "va3O*@jL86@2jz8tX#z.sCjvJ2g/Um&kb{^IwJ+nZ]NBEijl7P8y%h#v1)4x5o@%[Op@]n>!Afw2Ieo9pLSapneNVaWMX@I[ior9",
    "&:UbloS8:u-b7/7+V9sW9?oBECZ3^kI0dKCsqK[0(NoxTbXU#C]PqFN^Q7}D&75ZsTA92TNP=EH[wY)&wzs%YpI@0K04GChkOH^[",
    "JW%}sFx0d3Fsd4-g(<1Ec3pH%xa^REK#?)sMf?18kqBH#@c{cdlAZ8DZ!l7sFet*X96HX)fOC9xPBRI}@=NOvpw7:5Dl/yK1J3Y)",
    "0emqZwiwm5)3+b>WS8nWpgZ{#jrGODjJQTq@Wlgd>:&i@Z&qMJA3ee[nfQZufH3BqNI2h9l<i-Sp{KB<l[]0hv34/WFqh39/.0F3",
    "{=oiOmijMOO7?hZe}TZ0r)/wUtn4gNQYV%%/P&3>:D>R{J4@z!XBW{bPBkbt/D}/Q]f1A:0au3[02qQ$(zATQYMo4Bv3!.(aNhn9",
    "93g[qrCCsxKu{op]Xerrc[tkfk&]*zOs}@aXoSS*0oR$FXCxNEHq-4kmFy{?Je]hE+(2dqoG65!XAL]+Fxr>:js/3%^HAov=]H!Z",
    "[dK.yk<v=205fo<f.Je[GR+6HU*>nqEaUhSD<[Yw0=spqiUG(O5D4qCKz0jVRzm!T&5HhrFucQIt+KqzkU=mfjG2c>wtBrgv3Y@*",
    "Xv{cIS7.I2Ofu=MVExq%}]X/Hni=r2WtYO#kFv3eIn0yY[3GHo8BqcK@U9voJ54m[Vp:n)rX6X6C-h+*V<):ia(do8r7?wEN42+x",
    "?1pIc5wMx3sDhl4+bjpldDz468ukf0/ArIlC>h&!fHONE-Wm+12^l$oMwf$7]J9&ZCp+eQI&G.V]<cAqjp6L5nzSKzZ.o)kYS.HK",
    "}xqW#vd5GRu}G(JQ!&l#}vH3E&oey#fKR8$v6}pz@e7fYxWGEoSk#Xn*/B3W[oQ0PJiDbwSjPn%>!8LX^5SOFh0ejfPC:(BPU1am",
    "+7lDNE2&x.S9?UH81I&kAnpKTZb8(OoL+$q@^Y*rYi@Mu]wglDSJ+wBL{cb>BW?h1K%-MfA7?dZdI.P?1tqdo=[{u/g)xgE9qD{8",
    "4dM$L+jIU%GI!qNeip<nNY2:u9o!rNLR0&/PB4$/2]ha7-mw0MRP>?#4d1!zH/jysVITqTScj#HXC>o@)^zt-48%6$>Jm**S7<Y:",
    "3DOjvE3nIj1z{uoc[SAKxX5wgLQ$F2wp>AaAV}A}PS<qpoV?Py!]P$psyxrmU8Xo2037)G2VY($&[z[}{Ya75)8k41v3f-/MCRus",
    "at3y5PJ33DAfD.MNU^9shz:g5Z@@ukichR[+2N%{@WVsdFy!u}t*E).&fYV!7rR1/P)E$Ew*aDz7/k1-I*i@r&L@U@b?L7tXn#G1",
    ">foX+u[mE6GP^[0d7lcr:(VBk/<c5v+>^jg0c[#m&<3!x[t9cGfZmPfPO)kw%lJp?ZSj$*u}Os1Nccos2T9:Lc(LT&2PPezFgQmM",
    "]pYLQ-qoOI{SBKuGYkuZt*Nu4va+Gw9osGusNlpaif{5Dk02A3Z9!!3GGj1+^5L0%FdB4<Fo5dY(r9q:Tz6]jsq)gic%SJgQ6!6p",
    "m!(ggQ/7qT&@nIZy7v14YY3s.i(p8zqRQj{jz[Muv4P>I8D:tVBuT&X:MhIEfUGqaXb*!pU^9c1KMdMGkh$S^>YKi{1YN+3V&#]l",
    "jb0L}t9UR(Q*l2AZ>B{lXhoQpAq/O{t.sv^M<NArn7gWb3]-)Lj{fcAS2UK{ns7X!HZ]SY!)cxY/Q)]hq9cc?7T{gikUEJTEIRAo",
    ")UHqAE2XftX#IsmBuI6o72YLm]rP2)gy=i7buWjANifHk5gac+Z-u3ft8p5eKB%drw!6>+kUPqjT#kl57*DI)^8vfwZh0eJXe3RU",
    "UN!Q:l[f[7BuBt4FB(HE>PjfCY42oz=z]kO}EDS&>T+24RU=m:b=66Qdady]Fi&8aes9AK6HLm3^2=+&=>%?^5wAn1Kw$<m9>+W&",
    "}.L:{yek6#Lmfxk<l@)gxi>N43@9UkujKn8eqmN$FuoCD0]yBI.V$=Skd=V+)2SqMPH2Xyy/z0gRb=-)0?^}7C=FhnoSaHBjbX*b",
    "v<hyEAbL5aKAcjwNUGiMg#TFc:P$K4HfoT5[72j/*1nwJaqH3MTY#gDQ?jqPWCYDDc){93+2%AY4{693Hz4ql!t?y&MFkLbu+/n$",
    "fPoB8KNtp%)P)]FaURChI0m@7uQ.T)3IORQ}qz*#-kNi&F9-aU=9)+?yLO>6b!Z=Lr75dGow86:9a0CB:iW)JM^>W(90TT3L}GZi",
    "aS7CGQR^^g)z0bz{7%Ww=yF-kq)n$Xc22SVKVj8<OX&T=trS$ue@]+v=-<Sqe(8L-]%HTgfR{o-gwO#8+9HC!e:V0vzyqr@<^erH",
    "<({?Jxd?AwoVg^i2Zr^xP:k!PhFj!VFEnD(a^=]R)ub%.Lh)isfkBO3[6+QR@HUKMr!)lWjuwUtCPy$wKTWly@sxe%Kr(UCo&zOp",
    "zA>qr!e&$IIc.8:Srx&EVy*tA&RZi$1!t&YNFBq9SvwQ@.DE7bo5>aUBt?Sj*Q@Y.9em?m?BGPurbwJ@%c!.S}cT}2O?0V4*h5f3",
    "R1nB^NQ-lUfjRvMpz*sHhm-PE0o]9H[/9C277)@s(X1@Bn.<oU8^]B8OuzX3ZaHJ)e)*MKo2MAEN#9uyoToNuk5mz^<Z4+%AbH-J",
    "b/@P>5hoLppB1WVpRJdf93TSgf)(UV2$C/D4.Wh5WYHibiO6V:)!)LSlVWv-uSKr[qjnllPVm3}.*3)G>S2>g1N!iih74GHeIg5r",
    "tF[DAx*&N$t/M9zo6D4km7ceVXfMZvQ@CCU:dnm7+t0&c+wvW4mF=}{vGhoHHjf(JoV%uDxqM}rlra*=9rQ&9OMv2H[^.PYN<p?A",
    "HQOS{)4W[3@=MM2Oa*ThNj<Pmx<3xS>d3{zAl?O0Hc1Lif8GO3Jenxn0#AYa:=7d2*E5Imt8U}+PPIgC<i*JkQUfp[jt(]Ek$kp%",
    "8aAP0&h81sd0)v96#qB:-%vmgf2X1d)7fbTrEHG4QHXCWSkq>8t.DuCsMdYwr1H(H(L6Vtf}T^@tJu$&7DaPzWMJUn9kPMS-M%=j",
    "yR#dBh:CzD-PDFH=>GIAfjvTunM)Yf5rrzDjTthm6:zy^CQp0^mJC(2aBee{EnLCSAFVIo)4fz8qUltua!.=DT&pDc-3?!5mEvPG",
    "pT>)#92(7Ay?cp7coBERRt<Viu!i4R=[1/Ny!H3SH^dGAOo7[#k.YYY6v(Y!bcuCXNikJ]Jaz7+F&JiYdJP6.wAAhxCObP)M{x[0",
    "}Vqw-3Pq.i!{yf9/ak5FZ<N9k:ckb*la$D8(#N&])?8XnIH8G&S&5&W>iS*Cv3PcM-EXN.kxaq0A$?#}+FPMcoOz9kOdIhOsc@/$",
    "Y-$G1R/o*ydEhb&>E-04zun3C1Si=z-Q*%Vmx.LhNstZW:p>5S6e=t{^0sY=aJ{m/*r@+XPS0gJZ*%u8U*V7P:CZ>F96v3hFo8k:",
    "5Bb/Gd/f<)*8%=!>1u(ig^4RFc(3ye6Vcq4NX@OreuNF^Y@Rk#h>MRN2O&Ppk>vMh[qPxu.r4c1bOA>^{)@wpG4T!JFpE^oYWo!A",
    "t*!krIy>ph2ia2[oZh{FU@dVTLO7L(M/:*Kq2S)Fz*T{^{i$o2/zyuqk2[]4nVobh+mQQm[sW-.zQSNs8&2L1Ge[@Q5)XnDsbCf+",
    "E6Ix*)]CPnQS/C@e8ohGTGZ9=Jw*=1bF3#uj%SV-0@?yEJ9JK0Z}B8s9VSl-eUYZrLMl$j:4LNe5S3!c*PcjCQ]m86.wEWmgYr9y",
    ">h2e.=*/my.26iRZO95tbZasApcnOw0j2!tX*Enp{sX1V(wum?&)lgscehx.Hq(P{2YIia/mWIzoEtFcWrO]sKQknRG=tYV!Cpu5",
    "b8n1?N1>:S}JK{)QkrLE?.PR)TJ{KMtd$.n.-h2qj?L.$xu1#9]t.q7NkY):P+$}=3KwmEWCAw$cvbH5g}f8va0q9b^oJ#<5vrcm",
    "Z*7M!xTj8t1%IwTF}Ma%H1}l)i=39a4C7+ET=R%mgh4W(o/d)AIDYBiwD!?[}(Tf/8z6D50>xsnVvKC*M1%*xt8:<([Z8[L:vTaU",
    "(e4rh9[goRXQYh]V!pWe0qA.gj>0fv@=K.{5kerM4sFWGx{+E8Btc/XZ}gz%:pRs:bc3qVQcoIW[OZ-T{]>BCdn^1Ro[FtrhM6u+",
    "UQkvxme1g7=flumFd]2$m&*Q5!#Ps{T]Q{OaN9D2Z9dj7yQcC]bHq)]7$gAc5559dN6a>3E8VvOe:Wjnd79IF)Efv8S=@)9o2u&T",
    "LjR47*l![M/$7F6A%?rEY$!RJx)>E{NSCvt99s#f1w)vSimIGMmQC5^=x]cTwb50{)]3qKeo8H>rcaY}R{iQM:Kqv%Fe*43*g*gx",
    "KETFbm4>FYRD0jKV39dVVz4){+)n8GK/nNIwAXk3bQbaWQi@(Mo:)5@bS@R3/e[TGMO{ze3uo4VPW$$-qFYY7QV9Yoz?cMQr}PfI",
    "R6)z0*T$sCB^:NtdyCg1h^Jw}6@Yo?aa>ivQ?T<kAnmIcV#0K>!6hI?jPw%acsm6ywDvPJj}=hQ4O)i>+x=%#BcY$x)2NjwHp)(b",
    "fty%W?oWPlH![k*h:).VDa{e?:I>qBPZx(+mj)EYpWmNmZ/k>F.Apo3IE){zvKh(9]C3<VT/^ms=b!?ShV-0Md7bTg7$K+YaW$1%",
    "t>1n$6<s:7{$)X!rZThPc-%eR5Tzv&OG9f]XebmY{{PR75ta({GENqxEkjzU4*<7n0awI-Wz9[Gz&r$@t+VqNyD^&]>QXnG7&JXz",
    "ie:j?[psR6}tbiD(xV6D1K}/.)g(uQcfr*[i1Kp#e??oeAkJhH.wkc{t4?JY@oIhk9ken-nl.VHILwwffML2]QhDL^XqcrZH6=J%",
    "p}vi4Ah^D.VyOu^G<&0mdGT3xOB!f)b.p.m5]OvD7spn?A/rk%drI&el:A2>?hSpnhE>4DmdQt<)pOF{b3F5i.LpOO:pQ{a]Ap(3",
    "Ef8#7QJLxQ^2nNJ3x3n}]+ah^^4XusmJ=xUs-b7<iE>+RvMk@6C.N-.A3G9X:Hnxq8atgL%19lB:z#C/DQ23NVFnu<uy5w?TF%TZ",
    "Tsreyb}]P*{$zGb)Xc!$p5{!F8lo@8:#dHdWE2n85tK?tM*TKWy1&f)1#{P^sy@2REBqliwK.@#(PMi20XqEdQLp]}E&49dnzSHz",
    "nFWKNSc5!rTJ-/u@Wbownq5q5L?%[Map025Twql#PZ#eC6OrT7nBpibdI1LN}0vaQ]BNn2u>{KvPNv>AGG?rc:AINo9fqFXUYB>G",
    "1DLMbnkk:ut5lmlN=k%lFhjq&oM7iJXsA7MF=:N*)O<#!eCnzz9k)DyGwKCT}]V=t!ab@wZ-pwcE?am$U!CMHjZaHwtD=1fZUNRS",
    "z#=uuTYl#p+n0JhO[0Nu@<#KJmLG83%g$o&a8didlUgNbbl0(<+a-yW7K?Q.:Cdpt-tO()YHh?oPwNNJ4oDPzqB2K(yP.9Wh.9y)",
    ">AEj9X((?Z+:Srg)uK0K@[9J2Re#Y3L)JI+IeKg{W4e^j!Mdw/t9m<UC[l#ub>Aa(c>{?*qW^6FLo[U<^N3%F)S{F0Ba1GX{J.+j",
    "I$^&Dag^N#PBfbXPv%ic[d$=hbYHBWRu4%Rr24d4uBpM.<8ue9{/Z3HH.xOGCtYzBo[lV=s?2zljvr]01tpO8YI78}OqQ4O<nGj%",
    ">Tn6ced<T3HJhfm/oqug<HAV45x9G&hH&C8!Gm7v&*{Dvu&XJ^P%5)UzhTN<o:Kn4w(X6e!CLDxGN<MD7cz]qlxuYiS:<^Eb=8l&",
    "Xn&}fp>u5q<zsiD<%)n)aJh!]lE}Wu7^Yi9@G9X=S&}yCcgh5KTlUqG]X*-N^1}wcbR7mRNV5A8nXA.uo>jn*+99*VoW32u6+Xc)",
    "AljMZ.zbAh?e$.w8o7sz*!n+M*/ar*=67dJ*QF}4^R{bs-qS@NxrYQ]:8F1w*LXChkn9[ct@P<rLla7+A9kYARevi:7iylO+>Q5e",
    "5TpTap+6EE}Mq5aMK<fZBDSN=)u9:D2WdOIL*s8dKCAG>dd.n?nMcem[.=NA^xRMxr<>@v@lVlyRUOVOL^%WQi@W!@1wiKBMr4AT",
    "@jeE:QzE1C2J/zS]9uH6DtHbQPUED*msXBM&zr*[AEw=&b)E^w55l4Dszp1jV[$*$RN}U@gY+fK4]?D24D-QkdLXp{zjlu}7k[Lz",
    "xZ(6!mb(4@SCGR&9bwM!gxjZBG#GltIMq&NXK)k{({fKz]mTngcf93fG-/N:QYv8L",
);

                const ENCODED_SMALL_FC64_M3_U1: &str = concat!(
    "POPLWD:T9>.o.d7H(y7Knid!PdQHSrDj(@n4QMiH=ietOb#L!#Q^zu1&o[3oaKu)$KAnEQAg>aZBpiVkF*W0H70B!kh8$d*BE2qc",
    "m1zeFk=M3w^5oY5>Zx2mDN1DZ{nT&H]EEZu!1V7??X:tDpZwmevuzx<AS?PtTdroO^^#iS/U43[Aj<1IJ4z]X1vHky^ek(p:EB-[",
    "5syZ9!b*)U&gsHyt^I4948SYvmV$^8PN]9+u(?tvU:WpelwzXS7)<tD2<^eQ}GZZRJ*u>i=]}p&0da/hu%im/qyH6U5{?j7tI0BI",
    "vzPunD2YMwqW-xmDE*ARVF{i].yg%r9Fi&Bg(^g3d:Wxx0fHbu^aht.ntqCk}kBJYtX8by@.fYju{mKL4O$cU-ioV-Ec4K{n#!-3",
    "X+7/)@U[J]c*^M?H{.:@kP^bun^8jUk2W}l@?a)bzpu[C99Z/M5FHG{ZGa(e?eR[%jx77H{837V!{DmpWrmen?Aj/L2^hB+FnH4N",
    "FovZ3J3k8ZaNDOJkX]8=y+s<l:#F1^kT2@C-4t98(s3Za0bmOd7b?ZD{?LL}p>1-]A5(/sfQ3om<r<gQ]E5k6d9(-[S1=6<0p>>9",
    "79]?wFsH{EntvDzkgDn?*}tp6P-#M4?ICTO)eQ(=[-Ga1>Y]0F^<B4!lqKTBO(sua-5<kEg(tnCJl[%}M$1gI0g*X-.5YU<&6s[o",
    "njyuTAedBQ++cpCH>pS1ecZNp=v>z]6FB:BIHi&oF]{T#U>Pb0d71s:JbPSEgdqjcn}Ac*RL)AS!f5o=tI.+i/.WP>?nO2fAF^1A",
    "@+W(Z:}6p(H)dFA09Wl5SYoKZOCl9$fNB?N6Rx=^7{SY@]3(+:(h!dMC&MF2U(aqVFdi$&?1}{K}Caxg>NRZESn3myk3+cJ9j9yy",
    "UWou40jwlbfQl0cu#7fNU>ubJfV+g}IVgJb?ZS4#xgP6!y=r4(gIc{cZW^V8FzlRTCueN]8F82[A7mu201P-a{#%a*dS>FHP53Lj",
    "s=b!:fd{nsyU#Ol^3eaO+bTsi0o8c+aM#qdv8DUvwH*?BXC3R0vEW48{]yIVKHC]^pwpf&-dIQl?EUDPFfjx}i?XV1Clb-Eep+k[",
    "1{6VDj?wJ)aGkYii]!{SB+Wd:gWR[t1{7U=KD!?sW9%K*FsXd4yo4lCaD3Dq0k)y00ftU(BNrqCS7Dc+P-c{-UUh)Dp=w6YXfd&=",
    "Ko*j%LO^<)Fi5H#@cMmyXm8i%YSX-RkXNHp2URzmKO<%Gp<.m=+N()06}S]s[EA/c?8%mKd7](2+f:h5A}:PpfRCnQC/6{6+0}x7",
    "NGJaz5ren8*k+jb7)px3:W@UFeLX(19+o{![mdi=kUgf6y=at*nh=7kioXM0>TL[wtmdk<U4fA6:sU]OZ}!{we>pMc=[Bd!!xX9c",
    "Tpcak5U:584?D@F5YLl)a[+a-jJ1Lsgbp%Q=4BXjEWZ3{CBhds[r$)<3<$7?UU2?DaN=LF2NO^tLQ)?rlp#[]u}Pv?eA[-HZZ8c$",
    ">Y&WZZ<ge!n(-hXTGGdF>i4Vo[vHl+0jW+Dp>]9m0Z.M60e^maLT6KT+Wb>lTz(SKwknsKTH=J![OI5=sfCB:eOtqLAV34mzT1vt",
    "N!HzQ:Y2o7uct5/&n]<e9g3-uAj:Vkc{QhlwnMa}tr9Om{{>4Jl?7NG:uC>KCZnmho6A#LXjE:P)R5BWPSWcm@2je0Z^jRR1GHSF",
    "2VwHLxUSdZ40kDiVEUkMC=JIYi3Kf(gl0Lw9KOnFzIX(!REFe<bw9aKRB^BxvUSh(Ae3J&()T5uA33ZBfKhgKPIBGe:w<0s!h$94",
    "u>G]Y/.99AF!sV?UX75KXnLoUsq/1tc@:&)DpDM2edQ4uL^-OmX@:6TQ*6d#Ovqqczu#=C07$?O6UF!a6Xl>V&gMM4oW.}lhR%uQ",
    "3mfWQyD8qT71&w}562H3z)x5IK*%Qq(veloX)1}[>W[r/u(VL!3okjLeeqp2KY))X//:#3X&92v*=z/9k=SeyOwf&)*/rjA5s$R&",
    ".LUI*LM7Cwiq5U4)Oq6v?03&jKDX57^7V$L>4&Qjsz!:m6{97w8@TyLy!&<13j6-%EX41Z[dY+6m+9Kjd>DQ(JSd5k[u?vwck&F&",
    "06m}SQ@Yz@]OBl{!z#+9i:{4wi1-HgWKXF[E8e3gA6PL#pg{lk^XloCBIFE3{mWbL6OKronxoF#wxygKU(Ap/W9d2-ZYpn]c{%An",
    "qLHUlQ?AdV82OFsKdvSf*65Ig7]jsFb<Req!O:cP^*{**}KK@Q-ATE#^3xc[?K-Sr[h!}eNcQLhFm+dXya8?u=?M>uD9D$Py#sUz",
    "y*1qi2(o&HsF)i6Fo1yEw2jdTEM.^nGBV!JH=J)DFxu]+CXR:>fR6dB99INuljznP]O*Y1ZP^{QQ?#77[j.g>n9&WSiM!nk*kSQ9",
    "t%[a]S3Hol>Mcu0&6C#bFCugU2Q@b7i7S+R071ca5xCCC=(BS0UWE(41z.!^Zdkm/@62OysrIg*KAKvilkk:0h0BUD<O0ZV64CNE",
    "6Gq8^vHGJQi+jvoH=>NFXA]:0+UbDeYNW#moMpSTjHcR6I[6Fl{5J:UpQx(gNuub:XzoV/tqB:xwnRK-Wb6/R5rqjMoV^Y9Z{qPO",
    "CLZ*wSh4u{GbB!QZ}@=cJmXC1P.:!-wzxI+SmEiC(ox3@+tIC7S4(&lIvML$J6.XJ[&q:]M{)ERB{@):MzAlEXD[]<hy<nu=ECy}",
    "a/cr@DxxKqncrzo!ttR%QT#CcnI9fwFgIr{<#ow8Mp7FY5<s)XO</biw!f?-{8gfHd9tsGp$o.K=oR1+Y.GQZMy1^rXnKk/AiSq6",
    "PJ@6<jO[Lg.0H*dFg^w7KNF/8KuvUF+R1rbweR%wfT8/+mY(r@@qJ?iE2tr{/s<aaB+EB&>SGztE5OnO(nB??5hjm3*J.X<aHw?{",
    "ni#e%=#Cw+soJ/XApOxss-yEPYP%D#)58UM(${7xPKy0@F>AqYS2ew{k!zCa2ZcJ/uMyx8^?Q@SOldg55o51t07{n8Frtn)eh!24",
    "tHebs^wX!j.+Zcxp]Y4e))q&zVLRxf{}cHr9)kUs0#]j>-X9s=azfVEA2+JlKzSgNaK6.@5fEyPIF*6&&a+ekFvV-l{}k+45[yBA",
    "^M&NJv5W%9NMlUD:HSFbJ5Yz4LOS8*r<eL!^XUTg1yFn]XqYmS={>VhxMR25{.@.oE08E5rd*{Uciptl@%5Os*hsyjxsh>B4q0Hs",
    ".vUXt?wYrQALiOXu#^j1TL)CFaq8:Tv8qE%Fdc0Jc-F7Hv%ha>e^a}-hvURUin}M-.18Fcb^W2705yB:D(F?ysnXC%&(#Kauo3rB",
    "v4F&gr9G+]Tvndj+gK=#9c]Jpd=@VM2O1d7&0ko8==qD/rxr</9>095A$+Vsq^mKe=IrWrFhw=r2<H9SKt5x363/vE5B0vX0>3)#",
    "4R@gfFhrn13zj4QT$4&jPNzj2?wTN20j}5J<JAs(P2nsRA8m%}*]yVyC-!vQ[o]U%=]pCS7[Ch2HsJ#J/ND4EUT<Kj5jXy^GHNQU",
    "9Z(nS)M9>Lmhz@r^6}/B8MVQ]9Te-g=Qj6ZaO8)3jOA3[0a&vCS2:?W.SF)4h^.VUsFNdPB63/6fJo[(d7l+2kR6MgkN<H3in!I:",
    "fvS#-gK{=kO})==liN9E:qvrdSj-*be}?uW4SyanVwyC42lDIvaDh:K(ev1L5=+cWd=g@M{tvz-j?siFv3Lg-tr*1*M&ZkI&+M9r",
    "+MJZmfEk5UJ-$.Y[tRTmTFKobw!}NkZ+i1cSikx#oWNJp/XX{bC>Tbf5vsg8X)Ss!eC!=$kVl-au(n]9FlmQ*USh+FC1Oh^wwmp>",
    "V(q(QaNetT2R&nvUA.Wy!K#e</lfbpV&-h23HHwHv21ekHF}bmoFl<*nB5gE>!3D0CX2<Ec<n6.OxOWW-Ax1IP(DkS)W%koZ:b}(",
    "OhtG7t>5H>CP8G5Z/zEqTda2&mzNY()RqBcA}l7<03F<nPI=:)CgM5]gkh2Sxbud.mCf^=NKUIO]wldN@Sz)S:Hto:^/mF6A<vaa",
    "Tq6{i{:P)?&df?1L5F2%/W%fp&Zq@[xH5*=PCm>$aMY37>VMj{)Y@/Px4aKUfYgM@0Uy0${(GLW()bipnwieeS82Q<KLTUk(FP^$",
    "&nzgd&lQsz)8}sJL-.]TFmVj55pD4vZ(X1F5g^m-cIg0A=-h:-JP?}aNRmaiPe?.vp}NsLsXFm%i{uvLSabkdFmfXn&6Cz}C3*a&",
    "K^3qq2!uM<n3Tv6+Q0&:kkJU&9j]m+p}JI={t%la<sX!wJhAph5VDd35v@MrZiW5$sO2GPLXQ#DvN6cv=leVvb:8y@N?W.y}1Dy8",
    "WLU=-FevAjZJKcKn1*qOrUulkWka{HxWcMt@o(HaH+uz7FqA%klm}N/^3]W25xz#bQ#V71b&Hz&wh#[rm8g(E0*Iw3Ab{>0nziw[",
    "u#^RTsu$d][iSCN:CM7({*ImSMDwWg9dF462UNDDd27LYZ##1+>&fp*}LTRkVn&03obf@Wt8FXiBZ?1d7z<bm7ce@*NY7^2&0b</",
    "M{<$q^MM<dZYoUj05ip)LFtHdez3Sf-#Q}*Xn-z1=$xNCH[cSv((uCD8AAtY[xgEqLpwu3oYUVQ.EogLl>v*hFcbOWOoWcd+![x.",
    "Jjh2nY^%TSJ<lX*]Zfc2n)V[Td{A.3OcqkyAd@T5V<M@Fl+VmeFvH!8Z&RJGcBpI3)Y.kPmJvr0:p@8DsuA9pZ:BDuGE88X6G*6x",
    "pj8?2&eqJ^aF7%lY2q[c*}Heec[$iO[oPEA>TpSFPUJj]2j4>]7^(wbFu&uvE3QP(Nco%Ky:VqEQiKHp:.]0V4lUE&9wDnibcdq1",
    "rL>=HZfUni+y=&U{%yUa1H1W=!nMu&7&v2J[c)sIS*+(V]:Q8%r+djNeo(xLYTyhvSbGnW=<Zn]05o[PEU=Ss]Z.<>:P&UDGMlj?",
    "tb80V3#1CfOn]*!r6SdJ05kuP:g?phYPnU${{qCj/W(6CwkwmHF-eWOl2%4&msaz=+rOLgo4{NlIL!--II*TY5lr{pTNfmQ@+]Ro",
    "5jb9+Q<WIKycn00*<rJ]A5mnLU-Iz?GRBL>5ti#98AuqhW3PpQzQ+V0oL3r0:De[PmMoboIC}%pge9Iq{ndsy/PrRRG.P:OfeeLM",
    "TUV+pE(!64h:0!FIG7I*6o/Z=8T?Se=H5$xGRU$sIl4)+^<QtxW!o:oq?C7:Msy$1h)8RZ<PQr3Q++OaV.At<&@67@g+?/<CPLc]",
    "x!3o$u=iw!nt2ceBc&Ghoa)M72!LV4aGXOP^@kZUvm:tg:=vYhN9IIvO^K?$i?ZTTuXf4RTlXGLRel#i^^AhWeq75!f&=mTXxR0p",
    "8<s>$Np!YDkS0QC:9A?iTa}>i?[$2VlF.]hxO]{b6-1T&nrN/7HHvZbQu6S:sP17vYZ$RQsnNRlmBWkVm^<f!ISloLF*kMOD%%oy",
    "mw^TW&)XpGgtWByCE%GY&).+TI=b!NN.pL(&pOlRD#Pq7)qXWVot4)JdD-MLBi>gR!Xw*YR&8}p++jTxKXmHz)SefqkkZIRAX(xj",
    "+@9OQM2tr&yvZEI}20osCmvG:5OztiL9uIw{d5BM/op.F5Q/H1j5?=:3Wa}LQOcBbi[ICo/5$4pphQcFz(T)x3>Y!V-y3B+L}5s/",
    "Nf&KHpqhryur<D[zs?f#k<@UQ<m5%fSWX>1a}kf$!Y=aGcj8s?UEG>hUh7Ox=0H)[=TQ[c2aC/W&7}rPmm@uGmvl#!v{#M7f8o[d",
    "y+5k]^4p/Z67)cRkNFI}mh-)7WP0QFYmN).?1t)9DQ!NWxaV(jnZYf7v])k9AA66N(:p0ii#<[SmRL)!6=w+<?JObQ)r5g/vXE0&",
    "4U#7pw^gQ^1q>mPWGzh$0J(}%yGQ$Tp@jl!yhlp5u?&3YmZc.[TwtMOZ&CpBel]ODjns:#&2qKbGvA&k+}!cs>Qf#RQY.NHM])%P",
    "P)vi.w:Y%&)[^Jc{F-g}}K(GOK/%qHB]ZeTMHIjy.(0S3--l.o?JIlVDCprheI7LGBG8aIX28CO?xFG5p21<ACUz2H>D/-N0%^+$",
    "hZ-ZXq^#dTgybr>Aa)9Le>w%Py2kY1D3{X%NjB{[E.)J%fL@zQPiG{YcN1:tOV?Q$5V&^YwyGCH&RuhiXB73AAjn+eT$}=CQvgE-",
    "@id-^H++17v3&Z.k/nTB^K6f4l@hEGu(AW<Kw98B0b@yW]MeS$Jjz0-GPs<3{s<{Tl8#cxF8yO}Tz%Yx@&THfJnR@z=oF[N&XDxO",
    "k^Z!XV0{IyJ@sU}GPfVWC@pan<94]CR4df4Bt1H}?]!H43a$^LVnUy?/ml)=.5ufm4vSS}v=.ytHpG<?+zRoy!sE)FYPYJ.A=iXE",
    "4e]#+O&7D2s5k[YElIjaA4&7&sX64ptP7KzD(#w4D%GQ-K{i8zo&0:.GZ5R53s8D3p^!$X:t6$ZhT$SF5Fc/-@2b4r:n=l8J3gkC",
    "T@m8IBLp3lkZ]H)*[r=wW8bf].3f%8GAan2OBD-@?uaXhp-H-AJNT@wvOSc4AgU6jYSz-cIG:V=k4HiFwP%uKGKZJob]=!B*hWcQ",
    "^<YM90/RVysi>>H4CsdZp*HB>FH(X>JcMp8xOYu}UMzE2.h5dS{nxo2OHIVAeMdnnc3zmi?$C8MZg108jNZ3GNXJhkUS.$E4=:X9",
    "FBCUmWOKuB:d.e0k-JPxxcGx3m@9xl04<zE6I:@R>(zpd>JmW+zbPAdQBk!1&%g]l.W6bMF>^8}JyP?W8b9GjoWWffJ@0-VKGk-J",
    "7@9F)?^E}EFqlt4lvE#Wuz{)*EwDZ6I7AQfW97u>VYpFrqS#g+RZ%jfi-XOZlhUK.JR$j/)[(]{k(B5jD[%ztzZpBS/Tp9f79oS(",
    "aY7!Bjyia]Aw=4Pxh=BQ/mpm(CGtxI{m*uQ9B.NOCuqO]s-LN$6u:^%g6FETUj*vefz:=[D$]y*eXws3E@ne#?jiy!Jc4oF-$0Be",
    "LFA.Mz?4yxH::&qr(#C$n[!q&fKbm8I>w!r-3Umxk40Cop1r-q{VR@ER89/?fT)le9ms2:BO)b)Y-bf0?#Qy3^chiblqhqY^Wh=6",
    "fUE+a8InO+V#r^]@mzQ}g}<mfr5KG+cnvntKOd6+w}pBQ[c$Hz7iHe3?[t+#pSwkPOBwqF859rtF@S)DdH8n7{CkjKE1bo.7YY4G",
    "BvwX.bjm3DY)?ovr%<mp7.Mv5r(%Wwow]]=T%Rc319cJrx=XDYE[3C1@/d#k@(v@2sNh-}WR0$4:x3<8VOE9Cx[]8u=%%=54xbjH",
    "xF?UhEY4(-w=9c7DJ/FApP<*-)w$j&3k0]+Q7%Jt}^l5gNx0XUxu7j<[lRG{.JU0ZOFWnGa3zi&JDpM3P5?lhy:[Wj[PX/.V1*Wg",
    "rl>cH3B1Rn=6e#ct4lnKQs5M[.r}GudiICI!{OTOeNGFY2Um1&d6ti!ur8>Dtho#U/D*R:<Uc0-Z#g8l^C]mhf3H35I%Avpx2X(a",
    "-Qy8}O0n7WV@6vSKO7d/!HG(Fxt{/k9EtyYJ*CO^KoDaSk+TyPm1fr[=3x)-bZpJE<XGT8=%k6.ciwrNf5lN)XobA$CfDjm+6^j5",
    "Ek&ceX2gdUe*8).mlIl%dDFQ:/e}FC7GN7<D1O>C0neqPa{=/lFon{%yueEh1tt-#i#E?cAN4isqOBl!^ftj![yrT+=/--?VZnx&",
    "7EJ[&!RHVFM8wxhz-ARrls+XX*X8Y6i4im?SSObx^xjn0xch/tKstroma%ueh]]yw&%hUH:A]qFzJZv1s7UWJ5LTieJjjwK6){AM",
    "eFDsvI/&2.4XMSYDF4=4Au[7s*Znl*4jN#m5r7!Da*<M#98D=4M%sk!{l-MTAf}]:Z*?WMR[:E&01e!uOb-M6DGS${:o8Gbda<qr",
    "!9BFaPBKr4?4UWV]2iM4!dk/#S(7e^g>k?B^0B(*e[7M*0agU$oNxP14=91a&3$6SsJ1BTPT/!v&&fV9:eU$1lo.&>FO2JUO!RHA",
    "P9IFZvAZ?%Q:^GJ9{a=.+&?X-CTqIE/op>KB=8x{l2>L&xI^<8>E@@uBlh#bUq<-?>@k]ZqE$$O@1TE&c}El1wE:ydw(T*Ozx&vH",
    "-LmwBDmwx#f{Kf2!1?:.i4.r$J?]#AYg$yI?Cpw&kypsjz4X$ByV&w[pLH%BNB(yvV+$5536XdDnYwoLY-5g2*GVhxmzT*A(+8pw",
    "-XhN2@X(<K!{!]dSYfK-dK081ei]ccRj{)K7>Ef1az+V598@(KYJXb{@II->Wd<^r8s+hIyXm#$7ZhGPw-]^KxWP.>AJiLkQ&vr/",
    "N9y:R@PD?!SD?*-YYmeEAo-&P<9VD[ve3DXXI!>1y?6Ui:P7kjA:h.XKA[sF6GHMP:w<j]0e-UF-f@r)6IG]svcj:SL64K7sr*zJ",
    "uwZLMKM%?Gp!%[JzwkBK]9dg*wen{]+sp:f-%#ur.dscuKLVi+In7k#k<y%8XVcBV}9xOBn@bQzd.v4wyE/}?+VJ)/Sm97SkXOLb",
    "TP&o0A@i[>5]mMY+DX&DahA^#mT)qf*Jn0g3]LGcF8XZp9E@HJL]l8Q]3orWYuQ4jT@iRwnSxr98BPH@hgPB9jO.1LF*$X5y7G8.",
    "9V2H6}M0Sgg#T>nNY8Q*584aHZ>gWZFAqT3onU[}Ux$:.W[:6ux<:m>!cDk(V]s)DmWS%nU[e0vxm(Km>wSR#SSV+AI%b]f35:.R",
    "&iQ+z@^-@(]h2R8G9FCvB!5xxym+X})0fmdO)xaAkBcA9YC[7r?$](b0*7t-dV^3iJ=(a2)tu>pfxDt1!v7ZS2aqWdn%T54mVQFi",
    "0t/YrNByy-Kmn#e2Wnoi)s4plqlv2-a>f2@.qhxfM#DAD=[Sv1xY9!)fl6ftSRpOIc5$43i]Me-[c7p9ugOom-hR4}Go}.kH$@9B",
    "6$NeOrv<s%)jr}8MzVQNQ#V%rxDF.qVS]NPrtLD{c.5k>>W%GJ1%yDsIzY*@Qo&jV^xeksc(Xyl5@mq!^<px{rqT5o0z=/Y@HX%g",
    "^3(-5LON:)2h8hOS746a{C%]<kX-?NtOAhZ[rCT&gQRenkM/lyF5Z55&7Z4hF&>wzuI:]v]2sx/?3P03!Y[)[^6X8iGle9Efh5gS",
    "5/WDb.W9#&/w?3rJh@nT@1+.+R/Pw.[G:ge0lYjw<oSgTv8hrE}@@U}Uon!pDq/.=@07]?hfX]g/zK>$*A+9c:wR$2Dkg9SDsrFW",
    "Lqa=9Ass-&mBc.53r!39Ops[V?I&[$FJd<qJQjzz{QhUn7huWW3/C+-m)dT5N<%dRC4wk0&:smGpB/^oIc54!(GBevL=4erC)D<M",
    ":gdH2YE%TRtscB}2f.@*U*UwwrM66{BqHxe9V8*VjXLEX+S2nVQ[HWIfG>rbgxGFGHir0$s9=aa3gZ2s!bc1acvyIht}PFFQDi-=",
    "Z?0$/SLXbqTKh+^/)2no{8u#C>H[+a2&-S.Db%m8u!)9x0B}S-cY<h1ra8y-18XGf",
);

                const ENCODED_SMALL_FC64_M3_U2: &str = concat!(
    ")!KvQ}NB.ZI6O}h2NIrXfOQWZBydx#Tn%rOPUz2Jq!K*$6ln9a.Ke}rpm79anh&[00V?50d0!iyoG>6(PidO@zgb3&DdTVhie:xv",
    "xIkFv5e(@VAkQ!J9a=d-g[K9DAWvRD1AGQqHhKid6lWS]?<H:.xF2-p0$T1%*^cMQ+kwoKFcZA.!q+-^bXZw]Z*[]R:w!M&qJY!G",
    "3nwx>&qS49kYmA20d*6w7)ph#p#zL<@U%.cA0Yi2Fjfax+UU:YZ*HxWSmRc:iT&.=fGtE-9[O/MVp<ZJ44k+YPG!e]W-4drYyt<l",
    ".<AG#Vh>3v^k)e#[9aEFg[AqZC9@-L-cZSf{(QX3^c&!NBFFVBgu[txn&3jQf:e@s0svmsZ@m-^KK*6!UTPqt7Z]T(p$2[qTyN5G",
    "R.0@[TskhOQ8]Q-DFORm.ObiE($XF(bX8Y{0baFPauh[PB[uhdQXy(Nx2i7.du]xD3q[yFC.8K3Fg<71T%RsQ:KcV]m4ucFwz0[B",
    "PPY]1Wc8.FKra}C]A>pQPAU0KHg4%k0gqX80jQ9iU?7V[7@Wi5NJ!j:2kkgj8BiTLMtLA<Z(Y3!N32&XJpwpefDJA<Z%<.[2=UH1",
    "Fwb9$4318@J1>Lu4)cZM>y79c6Yq$P!![(X2)6GVanTl{gRnl32GXU.JfAf.{d*vQH&[efzLb:#0nXs2.s0dyROUpa(E)V25l8)%",
    "kSV.zsJ*JNBv#P%zvA8HQIuO^Iyja>Dk*!oGS9x-(sN%$ifxaZ+S3C5gh+sHhUY^9bWeEax:b?CLyF].@hd6tJJ!y9Dh%$VqTE{S",
    "Jc2[2>:3[ox5Ysz95%w1VE5uV></I5e/x*n0h9gL({+wC*goS+6DUT+zOS8iUU}ZrPHxqLWZ}tV&mH7Nt(8WT)3wH3ef0Ff&^pL=",
    "@gi{LCV1e%5ABzogweL#&<)2>OCg-2vXSoNGTGC@tmhxm-cU[8bc1qJs:rVPg<z5Ae>ffYL/pB3zCp6[ax{[tFf>H.@1(oAKyyL?",
    ".>i2Ns5Y>$9yv.eb&z25BD4xQ5wdT0bX7J3f7w+KDT)gvJ?-X?jB/TVBJL&wS+)s2[heI9C9H0fv5P*j?>q2L&+>m1nSF]%j(pE$",
    "-QlqYy8LKm0>@nfOu3FtY*/JBC}1/*5uqD/1p($B1E$xkxTT3^Z<Je]!wfUGuB^5G&kGT#7)rjU<HB##bSDWP/WR]z5iVHBFr)2)",
    "FhdRE[tL!rnjnI4i(^sB[zk#+>=Q[$[ngF8V(}GzYKVmIUPp!EeDlCFDK@Ca}SqxNA.f.7ui.sZ&P-tp*L@f}sZV]@wh?urarNEy",
    "&WprXp+Ai<5nn$]YBVzM7)P=!TZRatb0LDSNW99Ilwpxi{7c]h&4]$#r5RJP5&8TlXABhXoP/=CsGwjfH@W9u8CY&^FzF767f3$<",
    "Lk]U!CRym7DhoG:BbQyrbpC^&XQdBwJYR$Tr]E4w5mSbY!M?r3[tQ4.m71LU:E+-hqO?--!&6!DXyBRyJj5*KG0?yq}0dpX&2pPq",
    "uqcUFZ!6ky<N^4EJ53IUk/yG3Iu.x&p}P=u@]Z:N]OlNjl}L?um7TbC7.ju]jw3v4m2w/AkTUxgMK]JFvelrRBSlGcm!]r:IjNv7",
    "Xf@yZF0mdJ4l%T3rS{)+Sbg=.UN-O6joN)Z0?nbY<AcW3gQUs*Y1TE]sGpI@6Mo9%*wFy)MkmcDfFH8J+:5*N(+Zj@ImnE8i5l)+",
    "FpWtN2-YY:[%mJ-5mt&ce1Q#F5f}VFl(Z=>AfUShu3}e!)J>p=Z9OB@wsiFkei{Na4P-{2Z[#v?6Y4(:d8TU01$=sTdehPhTIe})",
    "A9RhhlH9<H&0BD/XB#Pdp(@2%0b=QlH[3Pv/OCLIH!u<yGQ!X#b(XYAFzY#WH?PdrAa+d[0i]@[?TconoN=fr{#&!.&m6M}2)VOR",
    "4Y{Zmkgn^XS6reO^{V6r1FBFYN!^dIj&GDz-T7FnzB*#{^e0{V&bS.GV[mz6qC:5HF*p1JmBew76UL[T04b-G*.w}+5wfOT+7]^Q",
    "7GDB9S2hesXlJaF0j!gMYJ7dU1Kog]kRCq:0^rwj[AFrkHnj#qVfRC7@0uW[(gWuP6VAm0YDWi^U2#u3&w>R^L1oBN?2AkA>SFyF",
    "PL3FK&8psMawf)wcA2]//.AO3{(p0uPOpL4GJGlTju-hE2(PN(fMli*y>%r0)9j0xnvyloy#<#%kTVi@G7noxM&*$:fN18vPYUA2",
    "9k-gF/HqI$5qZcBJ8E0{J<sXTzXjP+yQBeRN&jjHtzGh<CM3vht(HfO&b39sES1hV5BfOl097L.Z.[:/xd/odv%@[(r&RR3(IY5x",
    "AmtJk{.OK{1sNzoN7u=<}Wem[v}(ukv8G=wd3.[qp>YW$v.Y4UaFm&f*:>y$RK97K9%g6d-J(j4V{Cl+ET{[3*8yqXIiz?<os<EN",
    "INn!=*iU!y3i)y>GK<BjuicId5mi{I9cNGd258<R5>!#.C8bMIA92(H{ed3!YOB=}v7/%X[AG!ZIECbIwlE+Lm:/dpM(g:]5BAvT",
    "1t=BU}zmuGf-jLQ1BT%%XBtS[0hv7!@mDXo3*E=b(>Y)?ibjmiVo46W.<=.4w^oc=>]cLmHl^lJsA-GkFnBhQ[pMxRV]%<vC-yy?",
    "Gch*grK}3Ie$wPF[s!BC(lJHBweEepEe4hmhBBo6=DRuZJ<g/iyDPnyu@@KN/D$ZNPOA&!)bFhYnk:0ib+Ws-[<&pmps(}/+S9p1",
    "c81g1^pJZ0B@uu7F!zYWOp9%)BH:xAqNa$tIM>*Fy?9)^Hc0t/)eG)B0dze^JeXgTaqV4X&-.zRiob-R^0>^N&e-gEM$f>[mW&IN",
    "jrkJS]H)3lA0:[8<rIZUkOf$Tc{s]H&0Nlhp&nRkp(R>B005s@C=aPQEUrRyH8FG7a0CtZS<By(.4UG^4{xDbep2n#9FG$C[mD1u",
    "24k0P07&x/ax4.>t=*AckZ:fSBxP2ndVJ:SPKyB8Ce7(b5g7:Eg.w#^01.TX?@RR=?m{On-fvHsFrb^MsIgDy&{KlrTIs[JZ%+]*",
    "dt3koFM01wZO-8of}HA&KtKnRWmNToTT*iXCvf1nKxM=&?l$3VAWc(ku$!95UW>kRPDKR70o?=&rjZ#2u}c5q2>aa))P!^nKDmQG",
    "+g$Z:ay)ar!hg%O:q.rBOoD*6xEp*l]SmmtFu{Z/:t/8uPAMHK0n-:F>X{!<l[}SNt&WcyH.xqwJTPjbW]snfzHP$@-!O.3KjGl7",
    "?W]0m[!md#cb@CrU!fXz^5#s<Fw=$3hK*ZfxUe(e!3^gT=weHkgK2VUc#1V7A7b<JUSVEeGJl5QZ=B!As*RXLE0L=Rj/C!L=ooli",
    "Ap@!ns{rUz]W^nfSU-H+{@hvdZaN+}g^OHZ.@imi^M9x5%1H*[mWSQaN/D%h*dNgk}+Qh8<zqd=bSZ1WFC{1>1N4W9r.06K097CU",
    "CalK=Z%(<RPRw[TFz]3uglN*lCOgAF@f4<Uau65daDu9br4dh#5CUPOZ-g}tMq%cZy}CfWeCe8r5B/fKbX/iIy[^xiC4Q25*:s%y",
    "+Z/mD!f6i6[rX%BE=b&xafG9qrHY)DTvbW[Jl#1CIM2WJ>XGACaL{qmd9Gd:RAWKjDle5e}##A1fEj/NeuwUhy9h!9oAHCwFiHoK",
    "+>LZi0m?dZO9zStXwb8TPN=R@1EOQu&k/SC0fiX+Q])GS[k%XA[-2V]*qizAi.(lV/<]v>%9ws(^*G0=.laLqj42w#{(Y+.KCE$>",
    "jK.#%5uZgt09C5&Hv@RJZZf$LZZl=0]XC}G04CQ[&na^MemF3hMHqsE/M=BsxSI0xfW}>>?SK8=+acnX/BsPs>Y}Ru>-0l?2(?3t",
    "&plC479wV4YE5p{OGx7tr}o3I6LwgBdMh%]FX+>YC4w6BiwBh<-Bzj%r12iV@el7*2dJXSZ(S7?&j&d4)o#zCc@8/!ow#*jv2S8/",
    "064)f82ovU+0+nM!e2FQ6qeEu/VbZg.!)NVkWvqKu>/>dn#!vzk<+*t.0Tm@=uU18P)fi]tN+yh^Rx63N+/OVqm^MbI!k<J}iD!0",
    "&a@$r-3d96PV0V*1#AD@0eyneQ#O$#ZZh%iNN%LyL*Vi/M]{8nnmGxE>M*H/<x]R:/GSw0:MFpu?NwxQAVi*#0prcozQk{TSbeB@",
    "C!k:{Sqbw=-mP@gI2ouFTy4CWv8sAShJS$.M=/=J/Sd0CF9SAkegzu0Ml%kU!lmZxJ=x0IU:Y)JAmrpHNKMltfH.9!H(FP29bebg",
    "TNd#fsVo}Qj@{*<g7*I0Xoi5f:FKUNfIDS5A61^%Me/2fRDY!c>gX3uN^FrZPXQ2tQ6mhM029{DUNw4PqXte9LJOA0c27Q!TpMkZ",
    "^eYYJ-N*c.s-NfI}4K5#YgO8DJ-3+fk2[L{qJ?2xX$moZ>R*]5k8nO&-eEkIUZbE}=)l#Jsx}4MQ$*db[AJRdQ&+JT&ab<2M)}bW",
    "5tTiRQ&@m>0boV?Z=1!ECbG]8SaPT7&7(r{P2m%0u#sQWMoC8TD#p-68?cy4e:u[h]]lh*7YG63P(TzLTt0Hn2L11(qTXqk64iI/",
    "SiI8&k*00qmG?vbsrsRdjWj55@ezjK65esdFev=ru$bY&^7mohL^u{]eCB9DIE&3:[A7]U(K1Q{S}Cji.H?c/I#<eCgcqkznkD+e",
    "Gk*lvI+GzI=nUhOWBxH^bJn^t^!kgPU/rGM}V^P{6fT8[c2{PixSwhF*{NON)7i}AKpZXtH:!1&V(0O(ek>v*fFR@TCavWcZ(i>b",
    "N=B1MaBkJIaCwMlZ&fp=p^TIi04IA*5f}[30d}vp1tq8+2.b=o[A0n96GE2zXi)><JnMYV:r#LL<J/zv(axFH-NxSaLs6EP{t.{-",
    "fa1NvP0Bnqs:IwSKEWi3Ap2l<FhyYDPV3VsCP{-.dv88ui-/d(1f@oF@oFmlKFypr}H4u!z0:?T0guu&?W[m15Jf%fZ<)*@gkzF+",
    "puzypH@>p.Lj(w#{l8@07I?d9eOZiT2Y=6vcP7@&>[H2e[e5V0zd]9D56V)]r7gB-S%%.BKwu7[b5&h#A2&G5oU6KN@m@eLZ?)z>",
    "/GnSoBE#*K1C:s%[gI:zi6)5ErZb4%r./*h4P.HJD$U(gsI6nH4KQ<iRIat1c%=+:y@$G@[^*pm3w#5J?#Gy.LNk4pKwJ<c!{/qo",
    "JgbbSxchN^iTM=&GeBAfL%0RXfU&sMB3ZN9?/y?xQdz^T@w]G[/1%>?h-?Ehlyi{2d&qf$7uG6&krjlTP^+#mt4f@+)*p70=$N[>",
    "^Zd^[8>4FPTq%}#@^^T8ehP4BBoX8C)5DeW*4Lb4p$HVtR}R>G]5J$ru2k&&D!xh#HWUtV88k!*in8bi7}FtkkqdTM-5b+5o}CfB",
    "i!N1oic0(&Xk=qrr?YWXxpK=wP3}t=KKGHUiGId[I6Z//}2}zAq}}t^OQ^GGasgsbWhyg5iPYCpdsD(->P5[K.GIkdPq8z1Ft.c+",
    "[}W05Ms+AQ0JoV}JolnnVszR4U<0TKghYQ@=0ak.[jrpo&4Q(?xylexK4ZOdlS}e/iTMY*@W*2){vz-TK-!/f7oyMII}#GsrT6<l",
    "H.EwliO%.G/V$2wWecBUKO/x#*UDE9/xVFKe{E{>wwOWq6ZyVweH^hOLs)ssl=oT4q5GH2CrL33zflD8]ZIJpk1q/>{!x}+NpNi#",
    "w?>k/4WvKpu2gN@W>Rybp}+aq>rHMi0*0(aWAj}5ctA}5P}Gg:h{7qEL:X8Qe/4nE7z<Lzv{+?wgBLe{LPQV7uF-Dh8-TL9-y5?4",
    ":huN[/%-uyl00G*hFhhpzY<=$j)X(QhG!mEw?/ib-LFs-M^[+[0uPd(6dew}1v:+50PHV61Yj)9L)Cpvnt0BQV<}8?SM.YZ.Adh0",
    "?O83my(nLjDYpk6cizTViz@bRKpDB%Cq+e?{Aa<#KwG=bVR[z0v0W[$J==+::)F1IUfzhPBoC^Y-R*fejx)g%G+QsDX&e[rxwuv#",
    "v0Tv/y%XIkXbkqKw)I{WlQztFRJvujsM/W$/dA@m?5)fw!M:3bvRDQ)GK(b/vA0&itxNLw]Du4Owy1R@c6(27e3j*}gmFRmAP>k5",
    "E6[Zx}T{drs/>fc6<91J6=&b{mkyS4Tppc61KJV5PS1nxF:D#iRFP3<)P>]rd.E^QiG/:]XffA/clEK$gYQ<@{RD.?9i.:]Tg*)l",
    "?6XWQ<$&gm!Sn#WB7?7ddu{h-zPmz{-?sA!W*4%UR9aYt]S{6.JdBHj554D@lGo*+?Wjqo*z&C(q]].pN!/mZYNDwlRS?Q8Q5t-<",
    "/G6!Z):Jsx8$mTQY80O-KD7Q2We1)V2R!]+0fXIsRvN*-PO$zn26aZ%pp!q9^ZqoL6T8C?[Dx&BU9ScL/Yi*ZDn!!!5HRAVNH5yC",
    "lTq4M9y%!R&T[b.V[SGj]Y1#Csv3=oOA}ie]yIpYd1TlsCObFt-TL5AD-{tXDSjm:7h>G.pw5A![=XA[*dg$L6UU*a+Z>%vb5s6?",
    "Dc+Ro?lRmqwQNv!Uh]sva1]/w^0hPOv$]$crA.Y61&Ce<LtEr7uKkJ4xhOxo+B(a7*sSEuY*e[/aO}.A6@lQG(V4P&rNy@-e^x0J",
    "bmHoW)pHr3eJPcf!P%/%auL]NpjiwZTU{exMzQVCG:g4g}4k3*]HPOak.#Wdfdq^)znPZBu:CUME@60^+(4%+}VC0PtxRX:Cu)ky",
    "bZg.2R-W.?NshRf0fdjDNS8y(YksPi1BQzp+F&.X?.3(LT4b:@w^?hzlMHcWa6c+5FP8ZC5kcg<-jEtf?9SOOVwmUS2uj^rUIt10",
    "jz#NXg{PoUO(3T{7/u76noaF&!H9w>5oql[lXpK^6nbbPIxPZFf!Mkn1DPAyQF%fo(GXX&5sHl@<c)ts1hIPkc+KmUFX+.TleS=$",
    "wd1NmRo@{tkOM{Q4w^4rk-96m+Ew9EJEE&&FVQ*]y7i.F96XRXh&ZOA6OR<!*W(O#!J#5PXA3TMLa}rQBg-c#&mQ)?{}oo-5g9u0",
    "5AWv-xZBV<L^HJQ*n$>0=y[*-.&mhJPQWmtS&v}<[u$5n53vPI1MQZ&nCrPTt/:p=hOJnb!k%P4+a@(roRzB0UbC:xu(gl6@Ytu*",
    "X<d4BD<+nm>&3S%aIk-UxJuQ%:IX1<p??:HXdkazihdiLRDvj&05$7[i5o2EKAeMc/Fi&GtQevzZ@}Z2?t]^NuyKH103-n[=#s&P",
    "0cVTIcFRAYr(mIyaHY+ioP43aPNF{=FePv?@wU7%0lQ$FygdPo{awc+6Sf0={4St^9#dtW*ERgOr2=ugp#g!boV[6.N7MrW6S:Hj",
    "KDH9/p6!2pJ6[t[Vq0ID<e*f.oRRW71zqwYwumBF(-)lG+!t?%RVeV&K!bSr5yMxEPFOeZB{!>}F%D?WQQ((bDojm7FAkYMFZV/%",
    "K8>ci>&#P>QX4l]o[lwgWbmroi1$5WRX)omc^16Q)9[FrmF)t6^8n2WI8Gv*A6$IR+gQt&UXW*.d=*Z}QZ6BO[hy$zX(*I%mJH.R",
    "d}Q3uFJ^!Z.PeZ<Q)A)Kz!AZ@>ioYBhUnAiYOP?a^1Woqq0y27-150ABB<}n>!zThnoDEDFqw!uAeXUFg*r}[ne<iBJnx^ZkX^Ko",
    "y64o%[p10zZ{O?TFgRu%v3&WYQ*]y$-SXB/mc$)Ra#zc<Q#+T>PJVPO&7y!$&j2F]0obSx07>T2>S8OP?PvT*435]L9laQlM}t:&",
    "Qsy?UTsk*oe}c+-fQlSv28Fc#f#d8eT9D3QZ}Qq(bzRh&aWf2ws{Y1dlO!2w[bLX^^2l:zFUtSG0.>[[uJ+*w@x^UK:Lw+/wQ&jz",
    "65pNw:z]S(.=xO%AyL%J>$1HY5)P&GDCA7=NdT9oQcfIWp]##Dt$UpltrWQ5tV}g)B0tnh8u<xWey<*m&e/1(xQ-FyQ<@b4y/}3b",
    "/OY}$/.WRwm8.a0F%H(1Y00A]P14e1lSBdJz!jToQ<OB<%e%ThaY1B*6BTs((r152ZYnn^Jdo-#t$<+31LoY*jGuQ*}dUw<]NQ2R",
    "9U*/Kc<wIw>Bn-&:eLDtZr=?[+i5aQAo8wWp<bgsb3/4HHpGjR*h5&mFCiXs.tZM2]K-$Ap&y@#?O8d!y(XY.AHS67/QL(7VlqJF",
    "o#*f}V9hOt7j(-N9<C{-]=^3)/O){n.Pci3?Xc+?c72^?HU!K+7cFrO1t(PhTz{/}C^frztSHVOmZAVuXg-d(ES9?3MwW<?Kj4ax",
    "3%8BLJZWUC=t>UNee{xeDHz&*e)2FBn$XnW=wisKfUH=9J3Yi47a2D:C?JUwGwNSY-uWMf{yBw&d+ZHTwfRw]v3+Ju@20QuzmKq[",
    "[3YTr^}S<mFmP@*0e+.Ub+g+}M<]GFJVnIyfFbe@fYoVep?uAon1rTAKl$]tJDfzfsKqucFojk(4()p@AwKc07TS]g}S7eX46ns0",
    "k-lQCFo^o<xPFrtM2XAt+bSd?tSAkaATOo#ao$!an#m3K09..2MEm[fxZOCQ>0^RgWr4[MoJi]lkai0C{{I#)3{>*}3OPmUD4)[m",
    "/931Q-J-92XHA4)]{5B/eM=]702>vtHg$r+@H+^S?&O+<ApX=EfxY^RNdC)xk5-RFR[./oA?8vcYyfF$ONA2xKiDDAQwzL!v@0y7",
    "Yj^M)=@toO}P%J46oSO-tmnRpKj8N>0Lus:6FV]bu)OSk2-HpEO)N{IaOn4nZoxc5]?Vt!vkQ#@fn/6U9uQuQ-eVgUrTPINJ4q]Y",
    "DH#A@m2NnyX{Ew&A8^sWr]a#-Ex2j&><)T}<ASF]mbGu9ZizB$L)TAMt0[#C9j(2efO&N?6C7Wp--.@EBVU]6Q9h0I5VBf(wtoju",
    "^azbSW3d^U3p#}:C[T<Pzw(u=*kfWWbnGPdaNQR>]IuI>4b*RrfU4iE}J+%i9o@9?+-=OJu<cXD^aU]<HcW(mAf(.J>}@yL0fS%n",
    "@1=L23lJnh!&kU6R6NxI}/!*G*&oOZ?KCDENaY-$+UxB$mhhVkOkA=U**M45wn0y9dm&Wo6YH^VN*ZEOR$R:RU%u=4p3fLYhUES{",
    "NV-m^XhC!Iyg]g.b6!jy6WrbY/S.2i-b/xHo15v@><6PWCrlK6UThHbzU7SRb8q5&+c}gg]@Q^0Vwx<usd]u4]4CSU->984gh[$Z",
    "j!2=&<t5}Z)J&*={])l4JULISIT%lIocq0.@[qlMJhK0B:MgmL-^{g:SoR)C^QBaUxN>VEazCS7)W=5B(}vn&H!YwjFvf?/X$Scq",
    "lEb[Jwme]YW@eYsuA1)TJ&<kkJCD:3Zky8$EGRO[4p:JA[b<At]Hedh(wb)kIgu<mGSm)k2AZD0xCjJjp!fRg056zC(9?%5&/Hr9",
    "U<9sA=lv*5BiHjtwNu6lf^}:&MAAtlwq]Rl!YjZO4}TPWqyy@$/juRHXVWRGU0OvpL&4-?jLIK6o[=If/S5V2(SJ^xd)*%ljq8*3",
    ".*Ow@3{Na]i%sUe/Q/Dl5)H*tWJV!S9*:#iS{V4Za5ea^})1>*JB}cjEvl3T17xFr",
);

                const ENCODED_SMALL_FC64_M3_U3: &str = concat!(
    "[omZxRD%2figoU8@(hsUXjV+l!&gHWvbFsN:%$H@.y?m&hk.XjVBq0<AbKpv<IF+%-b?/?(A=kU-3DG*>pEAi{OjAz4Dq9m-j#Nu",
    "@-a$w&nT:GlDB$@+M&}!]XySNQnsFCJ>f:6A079)=(5lnuAW<5!d0oV<Syhfo{^B{%3l#Rr8[{>0gX!Pv0Kim7&O9vR#-INJ1uAh",
    "}Ulw&jC.lBg[f@EykE[.oWF.YSY/-j>/*p{Am$l6t-A5MI2aJ)]Wl&8+:MBj^0[buGMw@.=}qwwXjN8>Z{ZnRZ!S/J:@dj)HPccz",
    "DX({W&07dkpp%W[&d/MwN0m^SL=9SQMt%8>yp<wR.t%kwLVCEXnet]5xEXl&R85DxkQNZrpGKFUU117c)85Y:Qf=8c7#7j^PUBQw",
    "z+[3U<Drq4}i)sb0l0qG^baBTk^Rj*g/=lx7@Jpm!Yx}/=y^zoGdXriW56[r/VBT/z%>0DRL#R&i+)ngSrM$GK/O-3FCbV$ERT+M",
    "v0z{&1z+V-d6][=L6N6<?H9ZlFj8Gh0hAKSRC:DT/YEK]XcfZZG8j:}=w4j&evBC1[xwCQ0ja4l00.pvPQqrkkXeSZ1}[IXZZ!HH",
    "HaIpD0n}$j^1wF-s<3U9COhk+i#2g*:7Bi)/M/kp(p-fW=xWUb^T5jjnqZOlAmyt?&9bh7zT0oDHqgaeLUH+.]MjObuHgiD?GU^d",
    "0gMqoJ=iaO@qHH}*-O5V*&NXK8jbzbiOxfJCRO.JHZCs?A8v1Q9f9-RoV{$WG#%n9rb}p*[)uQOSW!Q]uwvh#>wAk(x-lvx6C$@a",
    "29[)BL}r$ubgj3o1Krb)&1D5M}xC:GS*&Z}>+:MR5<(Phn/my/QohqCPF}Xs2!v<=mJPVmd=OVnH:Qwdk<&^bZ{>1sau&GiKt7=s",
    "H]Y5*fCl032StEwaGkaHN:ZZ*Q+ZdSKKV-ZFL!M]l:W^}FL}p/uQGN.82K-QqYJ])D2oH[iWt9DqE5!aa{cpT(}LR=M>YAXfHNMR",
    "^N%FJkSZhOF0+X$r3:oj5ri*8F&y3Ht/$nN06]Zf>1&cypM6zfA6N{D8HgIWJdDQAaA![<I2AWN[AF6d18hNmXd$I@^%]O9w#4BI",
    "hYT&>dR)%9g0w0d6eVgx5X{*#BfM.1E1h4bQpwiawo=J=5w<7jsCuc8&1IY][{Y-mZ<]?8kXxNHCVOGunk*u*z1bIe9ZxP^joCR$",
    "fFp^p02>7iwhf/wioVBp98:$[-!SbdnwdLSEVja^kgl&4wmMArv2hPXL5Kz+rUHMU^u@cvxBqeY>nmyq)JOR[-99mb+L2:ob&IS:",
    "M[tUTy^rFk.AMI-/L7aAF:5B.H(X#b:&B/MFyKe51Bn==nhw7BkZ%Ojibc&7[LMhDV>e#]L7=}1Z}K)6aCR.+U+.{{sKkBeFo#:U",
    "w&mDsg8G1mXYkC#a.s281A#36)fS{e%gYpjnf6(T&Jj(HL-j[w00-?yU87p+Fh!dM{frIsL.)QEYXpxs>>by#kPE$WcB7w1d+oJ>",
    "/F/!xinp#bescvI0o&G95j9&&N7H8B>kpKUxLga7x2YRtNaPOA(I].Owd9j!E6)d>ZZ/0W=u4n]Hv+>*9b1#VHoQ0SB<A]H!}]l#",
    "B(G4bg7@SXC4)zUy&RhxM@OTH={hDX+(2(45j9[u5A(8p)bTRG=YgGd5l601GH{Zc:Met]WUxC#d2(keb]azju>gQF/@:ROS0W]n",
    "gc@aSNbNf<hRiTCXdi+%rUX}6rB.96aU^@HJ9%dUE^FY1}Wn>ayZZuU@qbzouRVnqHbMq1(je!pKoIdp6M0IbYIM*n&ol6gaxL5C",
    "]ANcmiiXJdc}pqJNO?}PSSYW(:ouYHt?ZI!/M4+v{%d3}M<9#j[APWeU-{H#PTyHSZ*QuUSYYHUFdP^+UN@v#W+dDu3kSj@<{Zxm",
    "Py]@ZHMgoaR6w=0IZC)Arg@&@TGO0Np@R$rUZqICg=G2f2>oyTu(kiwpa<{SK+<mOfNRA:RxCSD)em*n3mjZ}7*=:BVBdnb:q-k-",
    "*n>Jxe7jKk5%NQN&0d<%sFWDfI0K>1*)1:2jACv5Rfl@xEu&l]*]*E=lf%^#WUIDcHPC0#--?fg*b:TBTFyV+Ijgh5Jc+cfy^BsH",
    "Hm!4IXh7!hFg]}zg)BcoHsbfEXrAk>!(7!D--{?NT@8#dwk-d+J.PF=[mxx$P4/QBJq>p?M%8)v/6!eMH<aY?Fd/b#7%V:bfZd7I",
    "8K)29fT9^OfZ-Qq@x$cTZ-o.[wru2*H(W&-HV)v6VEs^@4/cL@65Dr@Fp2l#nvI%tv4:maXz+N[Z-L.Lk$NrOR$g@[%hja:-6Q{@",
    "(s)]A7tWrIm6Urao.9eo26nlQ?rd8qdTm&7+8@!?g@M@rkW?F<&33(s:DPgT{^v](NU0&su2yiprIxJbE7Rv3/p{3[8hZ%Fb(!T]",
    "X]O9veGEqB0m@}^S2<:cTDJou5jA&!H:e1{.2jnC=wIDY8F4173[(=!wzi8cylEZxFly^xk9>.4&:xyiZ/8>$5zm(jl}^}Jt^9h3",
    "m.v0W:B)6%9oaH(Buzm<BGBsZQu?6S^b%Sa&uC9ZW*vEDHlBnPWJxcC@d)N5>]xK.JT*j}yF2nMz1j$$M[L5AOm#@QLUmjN3qeYe",
    "I6ZV:@3>x5U^+yY]UL7JOpvj}A&sS776zch.R7wvvlbyvjNkQ51WRVw@7418S+W)k2P&(QZ#N?{Kw.{*][^cvXUig<mEBbsn)Zmn",
    ")!9RzhxIJO^+![9Se$e}x:@PQiX+2aL>eL3M(-is3#d]roXElBKr30!)7&qcKo]Kje^)y%5h0*[soi]+k+S9Tv8t/7aHJESxAr!1",
    "Xg$?kJ(fpd&{ud$kN1}^kP-83Ufl^SC!k]Fv1P:*XcPo2D#YoKnz5nLO]QbWsl260EM3QT@wk.jW*arBt-#(=.@hrzg[VP!<}42C",
    "y/^vAt:570dUVzoBw>Jt-qMNCmE(wI)%G*L2g.wtt!1sT4Z8RvC2FdH[h#<%qCTtRX>j&)*WSI2[gTV3xE&bQfUoOkH%.>UZ&fVy",
    "BK2U7lH6m*I3ZWOb:J4aLg=3.cNmqc0fP/n4/oOBi[C=ziSJFIH>jr[+!IMs<%dh}5v4Ro:v<JnN3%}=2WHVFp*@cxG233L&h:#k",
    "Ch!(LAn8G$5tcx]S]O)$:t62AsyHhLBxS]Lc@dB*&7P6wegE43A2yTFcp:(Bc)#QtBBqj]{f<Scjje^DZ9PdgteD<{X?IBznm(^7",
    "}lqRqkx?E/7vruX7pGX}Z*k*fN1ik<0pf)3AmystmY@-6k9++6Q]x]#apYzO@4IP[O>(vHu@RD=H.yYukSt-aQIbAC!8@2t5p/0*",
    "1zSF2*(?ctprQU<:tnyWjQqoRP+)}%a]a#0lj(b?4*FoiHRR?h?Zr.l[Wu1(HuhS6]9.iOxOkcOE8es^].dAgTr9iYZ%1EzPYyqV",
    "Fo!BZy]#tM3<tJLr8#M6nlD?z06{?3PFPxRrmdE7oJmrvPCXt7Z<=XO1xtlhU+PgJ5lVn3Z<5sgejkHZPZSA7oIlXBba[knuzRE4",
    "9Rb*^ZEtv<R7cNIcMCPt?zuYBQxS]@2gu*v67:TpZm0lVPXgZaCZ+/rrVmsrBXy]gp0VOGI]?bB5w2r-5mVo:Q@bjaw[pJ%N1-pq",
    ":pTH@RE(vi!rJni-f5iQiZzYrPIy525yJx$U!}i*.fIhc*}X=Xb}lz?kshkhrZ+}gA*xA-cPfHsRGxU/%ce^jKB6%uf6e8aS]he=",
    "k<8E=d8sAD?G)vdXt(:CM<u)eC+3fA1M)iSar5Xw&e0x9GHCF@w)?w%KE4xeKoq:IFzCW^Vs{9#)8AN0Fn9fr*#>DCp{mvvsx+tV",
    "@m3heZ*pXp.vKas*lq1&{vWl9cVqdiR*O@!q4E508tCw/!a4a:L.Lq.&h>wt?U=+7KA6[QxT:Z00hF^tl>o.&(PG?ZPNec3v7KH]",
    "5q6nYH&!zm[td?L&d^BTh90I9[+OqdPIuN>Fsh6Lvd^wVvdAf*y{xl*Od1b=)!YvmfAVa8dR>?z.]?O:kTyI@i3wjl0:OhUGQ&y-",
    "mx)&CLWxS*W1ArXk*l4q*ld7K2SmWR:IT02-?O}+S4zC>i4g<<szEQ[(r-L+!>t}OjtDB<Xg)M/@WU-D[whT$2VOb[D@NbVt-5ww",
    "jI@vaHmjjqA.?uKicA82l%%Aoii%Zu/Dw>W4T!yx^hdwD8K8nQ1Q%>jSVTO(p$TX?E4BlIqqd!U7-Cm{y$BSVAmT>G902:%Sg<>!",
    "WSBC<.KNTtj%lPjLg44S*/eye5qp&YKNjIXAeZ5Bjkg@xor=ZO3Ps=[]OW/OC-Y2tq0q&hKx>U{pr.rMi}C%cd{^.t/bB1*eJGW(",
    "kROo-MN!gZ(Hbm$kvexzBm+kJ-Wz9SPH0V.]V:3(kY%6rYTtX>*[7TYDt55D@]x[s/A#Phw!8hYl#TOrin:{5xUO7vxDVxGd8T-J",
    "9dooHr?U!(:wAmbWS!yTzy0j+^Y0bTe}G+gFc9A%045gpZ!J0nST2v-aIeNR!e0%ab806ZDzStV<j#:X!*ijq3Pj!A6Eu^ylCigU",
    "C/#?{)Pl)h7>W1B5A1i+7pR=9HB$0*8]9JdaGV{5*}k8Hy*JI)0dVJd&6Dqli@H/@ZhBruW2#YDy/Lh.NSBp#Sp@Iti<%NL5dx:C",
    "=<pM68oU=tY5-+5!v1LOpZFjb}fXvs9(InDX%$/BN8k&p.I@6r@4<9>3kSSO7@Qg5aEn}szM0cunCWtti^W+/[iN{NTK*lhSm5ND",
    "kTv?3fG1zzX[u^YAU3TjiUeX.Xf88vSjwXp0fsDE[s?+M7)If=!%T<$k&BmQ6S^xV+QD]jjOYayLVn[>t%FVUr7x$L6<$<)T/=a$",
    "d4Nd%mx/fzLZA!Jr/rcOy%aj&v4=&0>Og!BE!gK(2W])fEj)$5>VwfPkbBgJXgh76JkO8%/OqvF^bsu94G?[TtcVeZR0sZq0gKLj",
    "&e#0<4W1Mui8kgv&[?*vVRNtW?#.N}7iup=-TO3zXA>w60ST^=lyu-C@(a0jt9#=.oOC*FFzl*fKLiWD{*qDjecouBkMC<!5}Dfe",
    "zKgbZ:s%s2aMslJ^Lvua:v{V*Z?!S=VA8zp5yj6kyb$&F@is[/Abqw:tW:A>CP^zSi6ZLd@g6DzPSnR^94OtdcISXo&8t]MQ))Dd",
    "+NBkex=Wgn*9&vLshy?1yyuxv1y[I%?s<l0z]CQrTH3r*RgO]+:Ia07b151!/W(0A55j2^Zy35^IT[CAt1NbPR*s^}6A=Py.mX}]",
    "5dWBcZyVakTc{:^K)k}f{@Gr}>!V7rAUZgtIg1XMsP]2zh1RG@8Hj}s9Fl2VRG$?F/q=M#=A$3)(]LwmX9K<A@3b349JZ3<WVx^*",
    "4{u@!v:bm]v2y[@TK^Iq<2CHJlnwl#IY!/UQPnM4OSAiMl7UUJW(ohU<t]1V2aaw:T6!jShxewOa?l8!HeA%[P7!X!/BpA^Re=<Q",
    "Qs6B.Y5Cb2RVlMR*V3ytfi)bz{IWyLGUfBZx%:3^/@NVR:Y0mAEex[Sd$<-ZkvF9pkJUwFdH5Ed%0SYpd}6L:&s1E9Y84n%Wr:8Z",
    "Zf[1QAQprIanmk0fO#5Uk?Hs}:+L+!s%:[Du9(r1df^^PLZ?QO+(p{hjKW$btactyUcIYNpU[QP+xVj-7pALy)M=flrzZ#O1ZE[q",
    "E2GZxt$OcXK1B:53?:#)(SfPsLR>Y&0R^}OjI.v*&yunHD&VKEBA)b+Y8!iXkOVn)Azn{p{tu$8vqMaXiCEo.j=*U$:<E33@(@CD",
    "KKjxHE0@p-W5(!Y-+kMu5Sh33LN.W@yrzY6kObCfTNp>Ur?P^.7BU@>R-84Gd!5XqQ&&[9cYz}cod{GlWF7iC!r%8@PZEAA8neN7",
    "H3piM[jpqSgC.0OAEWvtfWz{E<(Tux36J$7epQFWMI3z8U2EnxttR9eXzc2R(HL1<mAQ?cIGD8n.Fp}(M$5uAGw#2Rr{4aW+WL[h",
    "U^OtZK%sH2j&M8p0iP0V:^#r>]q81wE<p(9<(R&vg<l1nf^m%TU}lo@ahMh+ieR-uuwLjp2=2p2RjtgZRKm$fo-{npSE3UumrGtf",
    "^sxLaelP?NKuL3R:QjC1Iho.D80:^T^62BxKp)8Tzf2kg5Euj=sV.qIR9inuyWqf*){{@jyl{niQGXzD^M3y6R0p1N[)Hv/MNny&",
    "e9wm[Nia&h809S*<oXZEM(]OCzGtkg-Q4Oc<f[.Dx>3{4u)JrEevkk/Sc}u6Q{#mB&xvPjzlZDh^^r^xHsNqciYNCVEYv>huIKdQ",
    "C0DhAxr8#=bWyNpPaovmgVp13chWl}*W*rqsK))B/RzS:rtA8dGeA[GyBN7-]/MY$Dz?6iAjd[[m=P@JL2Wl0v2Dx}FH9ru8#g1d",
    "r<y/4d&iAz{VmmDusOw@zMHaSd)n+N7/5h!Rocbd-e5DFP<phMf.k*?tHB(-BkKw?6VDa%?s^B7H<b7BTZc}22iU8gaC>wcuwr$]",
    "RACOQbwZz{7(Ys}I.33=x^]Dkd5C/:9f{E=Wn*}PM$45F*bJ#:{Z7oupyh](DdMdZ:GCSUWo.#V[w^)WvTeu}A[!8/Q?J^(@cJVL",
    "ppH&#7%:FvqLcp?0M61yi57>a:.%:mMhZ)1hC}gLGZXcsu!I}MLwMdu(f(H5^fIr/>7wKP&OPj1*/6z15XFfeHz&7:lAkvCA<Nmd",
    "DO9vMd@L<g35K3[B0{y?:CE5-uVOox<IM14j+hDLGpSujDmKOh/-N3{==qOt0:f.h>*aljt@!]G^t<LS8.#>a9?QFps:GXN@9y*o",
    "6Ej)ql3MS#:eb}5Aqn&-%1m##f4W^2@Z^kSgAliOq*HR>PRs(?zBOUgNC#dG>+4@uP7]&W.dyD%kl27Qg<xCz4upfE<^0LzIEALG",
    "T$xiaQr:^)^y!0)Q*2v-y-^7H&aCUA8FTHXI*imPtkOVtk%&aB?=%[UQBG-!ST7@9-1*CdoD}]#bEN27Zo3cMJn=a@1gaEWhRG<#",
    "LO%28<>.A1r7qwKx59H4=!CqfHLBUG@U+4nKS.j#U@r<<p.eNH0)KfFEUQcKZkr=wp?[Z$=#>7aQ!m)PXkrF]+-%=]FxSOShHlNY",
    "zJwes=k54D5@!xPVv3%Zppz367}?vZ9XdjOoO9nUA2x-Xji$sEF5@9.wn08HdZ1g-paPd7PRU@bp{(+8ycifz[(o&%s8UaL^inO8",
    "niZrw&+SzAu%Ft$Spcxxh]$>VhZ&5$x-wo3:{#lJu8YY}LtMA+]H&#lFkJ!?KacQ.I?W>qnqx^Jp)sL+ES&lm&p%<@d&WwfH{mVd",
    "C%}P]4f[b(:mVdvk{bKd.HUDC*b3-iANx=WX-xLI3eT#h9oTMt><wad2$qr^N20<?zCI7-<&ggZ43HlCs#e92PNoRI8i[*Frsx9@",
    "y}yiu(*wayAHwz)7:koXJTQPPHj>1sy@z<SB6a7eMLt4-+wcuaL1HIy}TXw:mk.S?etmzOx^kM0u9f71Lm9C:jRrN-RWu3Wgz8aG",
    "^1FO=8ArykzL998F(o?b&]y87TYYH8sbB]f7nW+srWha*5fLWJsmMp7[WN}$ijRvs[B9+F4MZ:RI-Y0z]U(G$ImRfIMv+k.pG1U+",
    "=W%-Zoa@kSk>z[bfW1QMKvo[$BSI:vb<#QnisdHQ8k*E#Co0>I*EW@1(FtHOZ$1=n3GjomE)x*yTQpuc@1*5hAmz}w0re(V}k<4#",
    "0Vh}wt{A=w:MV${H{<PMVN@k&HQTGJ>qV<kNzdck5(sDHJVFx?(GeR(00YI*:Cpqt@Tn6RPw6Jkg$043&.<tKq2rF>Q%)2<>^R[K",
    ">9l4.Ao7g=RzqL[SlUBRspa33:IdQog&ew?PAzZq*@IDsBIE7NJm1ujtR$C6F]S(=fHiQ&1xxOdSapiqim89EKNg]syvS%<QhBtL",
    "DB3wXk^x=uqNNaIBLTe:L7Dv>Xrrz@Fky2zdW?P@OTRS5P.k4)3bv@Mf#^kbP!>oj2X3oqfPn/wc3gp[Z{9vxJ5*5:L=6)j<(PHQ",
    "(}6&kq(VlHBx<Tn6SVw<(9SAkUrVHgV&kl:Z)%8>8gMiYRQPy}M3aVwiFf-m)j!-chP@FvfCAJYeBFn9O{.$IVftr6WP&?%9fYl)",
    "<a38Lq68QFLsKxvHhIA6OuzoyOw!@3FtUK(bgJZd/2@>kJ+S{PD?3q-Mag=%!&KEc(z$*^Aa*ucPCFcMGikirQ1m:Ttnjta7#vV{",
    "a5(4IkQJeqdC9#JCPdR^nQwp*rF5@r)2wegf{WEavRtezaIJ}3v5!Jp!q8>[Lb{yLR$$:7J2DE}&7P+a(]ssAXxEo68S>Lz]Nb61",
    "L@/U]tN2O&hXnr?Nv[xhDGlD*>NJ+s:Bbo@S)c1bBM/+*nq.=cOUUCXqywMU&nrGRns[Q1kgj4bLF9}U:NO%/2<%7Os:ljDf}x)X",
    "l$*v0kiwrFu(?!JM@{aDcOOoe6+WjN2(1D+0qGKb5j-wa&8$^$FAKZfUfjQzfZ(V/og)&1td3YhX@F41@B]y#YMBQ@sx!cQu]%{)",
    "!z^n/WB2k[w=dlB<1hm:Ok#@etUE/8P.(bVqWBNg1mB=7a)<iYRI#SX>P&{pLuU7JWCDewg-rKhJ8{7{==E$bLh1x$*SU}@O(nJJ",
    "XjfgR2ZFl&VGz@^<%p9[I-ZR@9U0-+nJjb(<*.#/8Gme<s/oh31r#d82VJur1kx0LK&1DttMOrEBL<>@L&9j*aHz!AFKXcKl]KwB",
    "2qKy=pXYD6?Zg>c{Q!Nhed#2ELYgrI7-IOJJ1d*hhd.AO*#jkQz*dwU[?p>z6][5jFhE2l{*VJJH4gbn@%4I@7t9Yx2ho)bzRie@",
    "aT@+>wyRNiju/ZSIL%@EO4jXCYaK)3v@uk0F/uLWA5!7+wu.=v!c3D0*RLTd1IQS{eBhMl?@p#S/b.%%F$ks]L{?o#H*3W?CO{Ay",
    "T$S^qjAVE[:JDJPe4+?{58T4[[gEwE?eRvn184.HAw^TbX$@y&g(K/ni1VnbqOa.Gm$Wo]+WDHA9q+BMupP7JhNjEtrpO9]>Q6j(",
    "RyJDDlo+W*Xd5[YXzOZE0fX[AbQ>)<48.nwCV(aJJ[VUfvdIKO*hH1Pw{mJA9:qZWye581/UTP2bZjTI&b)8NFm+7WmUVjMJSKvq",
    "ZePG]CXpbx7}k{Dl}es-PYutvUM.>Qm6K(#PI68-:*@&-s9VHkGD1jh&uPqO6^+SG!d}TtNL&d05o7%&2RZ$DLd7?ktUoVimoUX+",
    "sQ/X<)-pN9Xl5:BMyd?sb8GBnAgchT*lJeXJ5+#y!q<)H1zgW#ly}py:)J]TBx(TC-!5fz:A-lGUuiXEd=<@23CM{ivY@P(YWaZk",
    "UqgBp/&Lt<BK1qUu{B]W%jJk<h%f?El*8?gQA:V&}+ECX<k-o=J%RL]E@R8wDGGv:Xp+d4?K^UeKjhH&Sv2u[MMELr9rVa33gvBH",
    "*[a{rs.wqW=2<Z+P62NSzqPlB*1^4HHe(eQBU+}&X@E-z<N-7dWUyP>^@XQ7)ouOO",
);

                const ENCODED_SMALL_FC64_M3_U4: &str = concat!(
    "Qp)tkbRFTI>.(M9>ZJT%5A+{-u)sC<w$t8F{!Zm?2Oy92l/+tZY$BJZ5v$X2bOkWyb.NBa>4-Egm0S{*LEB(g//RhRitDu[kOtq6",
    "{$4gjAd3+gCWYe^2O/cUf*<H7LQqnKS>1ze6sKt)A%iY2ZjLVks4Rdg{XigK*Ud-LeFeJc[y^s]Ac5/TW3kt?LbkVK/T6T3(o@MN",
    "IBNn4=IKJM&1#zP6Hf2?&5%s/)YjHzdSiV$oIcu?da?cP00x8X0UIj>fQ0?Cp)RbzsA6OtFfM!YW-i)i9lSSvBA%W}GXoCur)&Lj",
    "XoTsLH>}6ONdP%-B)LCVk0ltq4C=rycV-Xe06e:@l#9)T>!1C?L-v<?[Bs*!U(PIvP33]y2(anfsHMR<MudTvj{f-RQ=C]$2(p:q",
    ".xP{YJ5a$R52WpSLb4#?{mNIXe/]>.PT!4m:CLHh7n3z}}d<0zt!S->45{tvhMe4rqZ%VrPI6b:Wr}CSm5g)rD}XK)H>mYK1t#jM",
    "xV9a#VnHmEFxU>(oW0*RfU*au2!R9t)5DEpX(W#Rc$z*hYY+ws=>*T-Z#8Mk0h)SA&34tE>Mv0YwmF/R}M>/q/YFhRwpa3-c<3qf",
    "fTivBoM:^S3<-:]bPg%xMZx8kp.ZmH[2Q/F9r2Au?gObc-vT?FV4ec6*:m58LN&U+yd0@[ylD&P0lDlzHV&@}k<Tirvo[C$QpFVm",
    "PTW.fk-&xn&c2EuM>vO92/FWm.#zLRD=hNtQcU->guBRhk:TjqRQI]=FxxyQ=ohp[m9wy<T0f7dQrP1q=:KiyO5*P5pLwd1-Uyqz",
    "cV{cqz[3JCfS&2Ku(0fkfN+jVzy-v.^8YXema^bP<AkD9CV6L+mL7:RkXnMVy?2#G00LM#>+Ak.Og}#Deizcu(qNQz0cr4&r&$jw",
    "i3(:gAq*!tSb4ThntWv&<DiPUYDhF:(k!Y]-T#]GP@TtuamUTWrPj1]EcKwt{y<n:aG+<Y}*cpAcJp=[(<0RQ-ow11ZZek$7<U45",
    "[Up4^dFHP<-1@2e&cr%qU.NCoXn]^9^.+(AFeuY#m&T=RX)y=o0<riG+XOul0oMa3HwNaD&d>t@/B/5+5kPF)pV[H!h:%x6n[?*0",
    ")>2qsGJB5A]$SY7tvh2bJZ:ppbvsx-fJxR^5umR3y.@zJ+01&HFxuGT<HUBQM<gWCEc.?o<G^%Z0cRNcZ/T8&0WaOT.EywBaM4Mf",
    "KvB1FPL5kRQnkH^[ik1cV$hUs2<CLRZ!29HS@J5YQhuO!GXR>R[u9LufRHG<^o*MJQ&r>dGkdr#:^S5<XZ^YyKUvD.(S6Jta{uZ+",
    "fZtC%uz3pwfGn<v{$LqK/9/kOdnRb10PT>RFun-i&e=3]0mXY)<o%tFS9SAo@&h5y].Aq-c#S+h-l.z}rcP6t!y7V^Fg@Do/PHY%",
    "Vn#8K((Z%z4#JtgXBnrpYu}3JSA705RiMP}:-q{CH[PI^F-hE1AnjUXiW:)tZ6aqCWIb:QbOKJFPXB/QHtN<3PI&Fp1oR:?FyESk",
    "Sqw9U^-c(403v#Fk(hzgA5c{@k*9N$KqkKz^QF&Z5i1pnvX-?l(xZi*XfI2op/(X&/B#@JKM<J)^La//Rsm.N]WzzPij.@3.7JNs",
    "yrPTs8BT6Sau&xgpm5/gC:PEkX](lgYZhaJu@8tpKxp)k5+V5+22^luGM{IgoNQ&)Jf-$D(%xGJt!nL2aNAau{jiK-6Hxsy0l+CQ",
    "Oj=vnKJ+0[Bs%4C{a}B^Ug%t$&6KSYH:8gH7^6Ssn0T0oagSdJD*0l1]:LxxnkNz(@HEZ/u)Z=yFdqup95i7e*ml+T=@jsm{Z}RY",
    "7}^1m{{fSSe/J.j/N]LNrkVgd2Y?TZDI+g+Si%A7>/HMKr*GKAH^PF3-f)eVbe6FV?S8$@0m*G#-8ZP=7$o[Cr5eWhyZfWFLayC>",
    "J3{erkmQ%.+PBd9!!fF#p.uR*T*Id)SSn.Q7{lQ%Oh8.g0nt4CaAvp(@jg1:VeN7xIIB$>s0J%yU5A%Cp?Ki0fLQhxt*vMb[vGQK",
    "qJBQo3TWWjBMsf1tTTmU09FN1qzc!sPNf3?LkqHF=hCyuQ+J(N2&k!ImoyJ[M&zs)!wx8nhYa2dujQQh/pni(3t9p3[kYg{Z&HHe",
    ">-pUmZ$Uzy^5{9QMtVHY=u5Uw(E<0p3@LP?Q>.sJp!7P2U<imvA0.6c@8xsL.J<QN&9Rm3q2P{$Q#%P#[BT6191+&S-m9XU819zD",
    ">&u!FnkhSrNd!V=kcxvD.!6/J4n-/ZmP/)Vup0hSdZ4ghv236(oIb)XfC1QOCRv1FPL5jb7$I%9qU[3?*QS--D{74:B]0@eH{*[U",
    "=)h5[g-2t?KLcIz?H?{[il.{{*H=zTg)E7mAak@ZA5>Lz5iQVsaCE.BTLeJAy(8pT](EpHY8-LTNi8F]jJB5bU8B$}H-ifP@8DJQ",
    "Df6Q>MLBB6Ag&31ws#i3^3{rlS4n6mA3Ro[tjy^&J!<svBJoLcn/jo=0j}hNhC{J*&4sW+*+{ut[iVC6wi5qQ(AY59LYeuuoE2If",
    ">&<AS0i<WbjoxJokY-TjIyjVL<{)M/+?pr9@qTHAC&Ka:(/1W5:xP7!myqhwp4tl%[>}S:oV9:u2*jd1y<0Lb{!AUeP855)+-eTo",
    "G]N=G)h[p3Bc<IL&8PfTVlwFCdZ=a^U^=E4+XZVHCq$1kEg@f+cZfz2i7OAWrdb7^beJfIyZdF:r[BQ&]ma9VrG2}KZsC87Psz<6",
    "qlyA[9zs1cUlF%W=/(mVVVHTWE-Gz?EaV+dYLGso&9T8V@4y5[fINu=.w@Iq5fhMHD}5SH0W-Dhr5N:&rk>Dxy!COTi2uT0&pHUA",
    "i5mUICY]aW5oX:Il)ytFKq>:y*/(>NxHWedtjer4KB?bUsN(8FFqLpc)yNH8^&ZLjKw*/@{ILC]Lq&F94b[.fk2lZyOzm)zZYS/k",
    "5wo*h04K3>Am(CF@fb!%YM31%q@Psw^Xz7Kk=v2Zp@}oLb1g=XJ9.vO<}tRCsTvPwgr3YmZE{e(-bD*iv9/=OxZ-8^KCk}Z7*B%w",
    "JjV8M{Fu%=90-60kTVhEEYt[4wbz/WE<Q{O97k>4j+<GXFhGB5NTJKCPB-0ONPd{57:-AHFyb8{LZxBzKC3MB01xn%YFP<QPCWF^",
    "&8c}{fJRI)xM}n8(GK6!k>/]z<ORAVwfUc*cI17FkM=u{jv2sF05$I5--Y{!?w@c1jqAaO:LUNY>./!ZZ5sne+9!TfYOL94z78bx",
    "zly993RI?XX=Nk>PO@[mA4Hinm6kib)0opcOrN!LkSh>g5iKL*1S]eafZtt]2^c?lkWELQX*Ap$u<d:/B<&xkl0b%bETev:0{otx",
    "0a2cd07:]q(I(-8GIs!DYD[Ae1l*Mn*teeXA[dcMjg(fu5zD88e-WzPivH7O*l4uc!np{y9V->C3)3?g8G$$<PBKmI-W[?!.F=8g",
    "&TyTRZ)!YyLPEX8sO]J&!cF&kkPI>$IJ3*-[o}Vbk&twIaLRW+fS8rx7nXJ7xF(CCWRIu}U.AlyJ9fh{t7hb/rd-@tL!4!%S]It!",
    "SrMV)3Cdd*U(**=Ju&OW>k!4o4#gfHKhJPZ4Iv>!7%p#60f7+QZ<N^%{(Tw{0o^s{8l<4KN[{Da/Zta?XwT[25ll$q.N4xF]Ip</",
    "Kiuy)h#=vXSb}h=xYqm&Z+pm2&{2U&6W6y/xO03z/r:i{Q%pj0KB-SWDo4YZ9czuF>?ikOiyW:F/7Ap.4jvD9crg)D0lv@0PER(Z",
    "y&j<r]Lr3>[z3#a0{3C@}y61&{k>(V2(1?wk^%3vkd]&q:ta1lTHMzoc@^Jn($y)Z+2<Fklya975$:(>YZcB$06GNc!/y0*3GFb0",
    "g-+t:VD*7})YiUb0T7$RA%TiT5M6/qhXR6j/</.2*[Jxj3Ba3LHl4Fgl@7rR&8p)-Z(LASssCPg]Rs#ugI<&&Zx$%]fRWn:Zj?JV",
    "WU?EQqPNq1U37rUfZJ1a3Nn}tx2w#e2aV:EKt:Hey.VYU{]CHH*=1)&MGNP$uP59L9Qm%+Fn8UVYJ?<cNvHd*Ob]lAisi)yRcuFO",
    "1q[Hf/z3A4fVLt0Ja{3&<ChnGp?xkYZ+?$3]OMUZfM*<q&0NW<0iDbZ/MQ=qXjMJWc&]ywGC9oc=@m.ysz3B59WNps+Nc9*u]MSG",
    "M{QX-n(yRYy^K]gFw3=JxTN0oDp)M/[hS0Z(@YYu2{na&XR2ZLHIBigaI!NNK*D*Qp^C8]bd46SH>j:4Fb$.xtUAzR@k[$4UMX%T",
    "OA8Z]eroV>9iH[Si0yo+>YUm:0ioCtfBuw]aObSD&)ylcyPrgMlj6:9UgQg@Nch@iICz>*b^ut)Kv3Z{fYQi!5ef2$.5kH5>C^HP",
    "x-Dq<^c(-Lj/jaI^roGx&ru?QO?5s8au#kP^9F]eZ$KEP(<9o#sNA-lr2puNU>UYv/AP8N[vq7L@1<MuRO8jToU[80d3>(ot1Kod",
    "tP4Q@5nJ)Yd7xq>PFANX2<b.Zv5:ap9:NnU&4ojpC9}<l!mQ3&(h]FGiKBD67Hiy)O0jjy[eOps!(c7]!7eGK&qh6s(r:}jJ+a.z",
    "b.2hB*.$h@PTxzly[ZO^d%le338N14J7++o]X(r6vP8tK/Jvk$:BIq]&!iPg.KQ]{{o!}Nf9G555mjlSU.p7XnB*pOSa0t3fW<z(",
    "zjofwpizr9mKaG*V-F<z)Miu>9LwpjsM}3z:^$ZXc5^)Ee)jWSmHuVtH[DGo0a^ckHus)!thxj(aBDeY!$Qp1p#8ZDFAbB$i878I",
    "2T1OtLM)*Ap.-=sxNhCE^bK]nFx.N=hJvn<vcEwFSTekW5}Y^t&5&bB!e(]Gp:3sYPVAFkp@B@#4{K6rrV^X)N-2DBnNMN6}ToUl",
    "[tBn?l*JpoGh0b{M)dB2sy/y-kf7x206IbVG.Uyb4IfEvAQEH7KU91+k:ujI.0K$gZ^*ZOPPePQC>9]ewU/t*B5*S25h$Dtb+3:a",
    "5g&BDKrzlHMiC=3AcIz%Z?0kNMUD*<d{sHqY&](#HyK3)?9#]Uh7IkiZKE0j!iHi3@.>*vi:?3ZJq]IkCcFCsoTMxl*@c!qxFPL0",
    "wkF2]{.Q7}&e-7BaFz$4p)CR8YVJ{YBJ)K[J}g#sEOx5<unyj$^bS=(A0P4FIO:{cL*X?W[j^{=U>C{@TP0JeEa^X@YQzNd5V=XX",
    "p#HJIcA5dM)kOrmGNiUZ@ddkGnf3Q49u@M*4{oMf7{l[6HzQleJZ1^Mi!e3Xhkv]I/bnV[oZQy]a??2w)FSkjRYQsJ0GslC6Bmt{",
    "DH}OeSPvby8./QKk-Ca{AS)dvYhQjzbysC]b5#i%pgwMlebo@zuv7J[t[.@P364klEVb1aTbE6yFkKrH?{e(0<a]?i!!w=YFnK.*",
    "kf2VEATMnt4g&YS^7xDBvbI72]VB)iQ{{y?@$yX.O??DMmT$1]l[Ak&r3+hKVSP5#X^G1Zh#5#UQCu<Lq>V#Jl4{yyBBUM0f[1><",
    "S=75k^EgNe?Rj^s^!{*yG*pg5J4/=DR:yD#9CaYMngK0<*dA+S=g0v1Tg/SrgsAD<v9<XyyQ+O5M^$N?)Y.^pI&q5Tm6=8wDaBQ3",
    "Gc03qfopVG*(0sth9:EwZ^o}yn}4+]GNh<}aOAaO>fdXRvad4$p*8nX!tGxhbY4In0nU^V1AZPQS5XO9=]r$#Q7vR{3q7rj9gH.c",
    "34{>]Qi?)kG9-mbnH?klEli}ixuVsEs#dLtZLs5F9Y1qtm^6zbfkn&#uQfVo1EcU+]J1mxv9ckG!?zrZJlxX->K3Nc8GfWXK!5I^",
    "uU)bkI[ROyIwRmnTc[EK3FIw>wrr@TKDm7r1JRcrHHg=WpR3[DN?[&jAjyoT0TG&iSt(yi6]VOnx$v-=loc>^9=dGk0nE*[SXV7*",
    "}oiXsqev%IqU2D7Uzwc2REW/Zx1f[ncWKVl^+e/Re]q[0Kb^CT+5F6eN?o{fSNceVN1vmV}l$l8H5>Kxs<W*B0!QP&V*3O+K(0aK",
    "uBzjC?2eFis(^W*6^cTySf*]9O4aJ[qkwj%d9AX}2>rLYesI+=5R$9-qacRZdJP[I&>A-4>J8YT<EjyS9rmVPy(Ndi]pH?5ld!y=",
    "kNG])O!#97?0*6ip*?WlvdYX%0pu(3YX+iMaG=>Knu&<)RPiU&k:S4{^TMkI1p$-2etI(3SkU]5:oTrf/)ey/tan9%<x$zug$K>7",
    "@mEaA0VjA&*Ylm{eAXK$WVliT3w:B1Q[v.4M=1D&:/TTqsA.+FBVo1SuQ^#b>gkUSmF*1EtsZqeB)fBJLMXRiw[-RR2g[akQPZvO",
    "nuN9H]>6Vb<o5&8*C:w<C+km9pt/S}<0$Z9rT(ZP}ziSnkXhVjRUgJ:5AHX<7aq2NEcneQKCxKy6HT0aT{DspQ+E/(01B(GenpuZ",
    "Z%!{?Bzgba:AkCnwkM<JEsEJ@TfKj?w<N/Up*[%&gj8US?x-6lb1@?qXhR^GhI.q@otojC04LH)J}%+XF&m5KKAHJni-579r@d?u",
    "h7-8#xR#fJM!KQo.wtILA<Vp&M(YD}>3X3G23Xb<PfRl%FHi/Hu/yumvu!*CKs2:%VhSB<z.1WHrf$A.nv91]yCPtiyaF2J[TmDZ",
    "ShIF=?sDgYriH5I+&fQaM(L9Gb.jHXm6Q:CC$-K#{Z(w%Kp/{O{m3MAhyF?{Adq06k<4{QfKA[O{.[T}ct>3wU?(.0CiY<R*[1)q",
    "FrWgHY5cTO}B%L}L^fZv)3$4r2(Q8MkWIBx(IAF6Cxre+8uib?KlmDCUqgU>ndnlStXO:LxfBs1Z$/KnMUDlc9SazLDD[gTlCLj/",
    "MnM6lU>pt%e@Eq8PO.z!WU7>Gi.]Z*rAj%>(CB]iX}qryfCW?Q&VSLf&mv$y=I%g[NFbEJXtr=IX#*j>Qdcj*w<3U*lBIDVt=]-{",
    "pIr(]XxiR>kyXv1Y<OQ?!o20gzT4=@B1Xy}9yHWQkcg3ZXMG!mOcpf4n=J&(a<sudbM)XLbgmguwwSk#n[w8*CxY7WS-f{3Rf+Cw",
    "DpSa>vkmI&F@p74)8PpzTIbN^{BM25aKwxE9nQPS{fRb&YQrj)Q8bmxUEz((jw!j7tAEJ42O+YIHcutpc[VWSx&0RFnk>.^gYAO3",
    "XIJLnJ:4{^0h{%MNZ(nduf2@*m1H07nMr5XNP(q@gS4OM4g/H6(mFvosrD8czFi:#J/9eom]itLz4ja/Nvjz}uh!X9ghgTu7Sdlb",
    "d!j@J{dPIvJb][3Ryh%ttQ}!FCQ]pEwsbM{&{yc/C:(K.AcHMW6Q63vnzwj0c$+lEwlE}QPL0ZRCRx)%H>1.N44QD-ney:eW6aWq",
    "d*A&6DOv&JmgXlsfrBF6yvA@w%4Wz{=vy.:C!aXN-:94q&.yf5rjyh!a:qXM3/+UTZ8zph9w&ElKIlyZ!N-73S{2bIye>0(L{)]>",
    "Xu)OD8>R(0xPsyM><#wGo=ztv/K?PPtPzDXXhkFzI9zbbQ{[32ce-NLV]Ra0p@xF25Z+QgxKDxPNZ*E6y+tfut>vqCW9wDY/p1xS",
    "5xDQ<YLaT^iWl1oifo^}4ZWrvy8Hi?O<fD(kmiB+iSa}4PXxVmM<JBGIqs66FnT?*b]Je=@+Zb$[3lT<8H.(NQTt{MFeq!fPR]1S",
    "2Cs-7Z)>=@Oh[<dh{.cgAb!<<Q)3)7oZGITU4-*dW^aq+yZlH[0L!E{+3/$qPVi$BU#D/HtXAs+w/*DPf%7ZB:G^wg!sZCrT9BOa",
    "{>EY*octxx8/:j*nK<jO/BVK}lKLIho84kSlgO8v0w5QYW#f*)/Oh7yyS%*ZpD.9f9)n:>?%6^DX#UfY6pVW0}1oMU4sQQRD2Dhq",
    "6Jo7!a8[IH-HdHRss-P0boc%4wBEgB)Vxv<yMRPg2/JvGFC9G+SS0U%d)[)2=JCNUc$owzT[hYVg.K){&0A]bH(?l)e.XW!qb?c<",
    "ZFnsNh%pHYOdy(FKFZaiIhior&mVt!W:^QP5!t*y[Ag+8z[c7^o?yyfIcku(k/[R(J%DGl>1&4(Snp%@tc[BDu^yk^=9l$a]N#J[",
    "?!Mrcsa!D9NGKD&*LqdWz0A&v.cGWEdkfdl.MHQqX(VhL=pg.@b31lF+*#eA^qP)+M2hmtys^5Y1W&NfH(p65EU4qTfd+492K[0w",
    "nyXP(]dVSEAK{cN:D0Wxm<R$n[)qa9CEm1R3J1i{=7OsPX3m8sj:qilHa5L8u@WJE[=n<bf!<MVvNEW&H>9GdS}cRjPcjJQ^C96.",
    "^DPe]HDN4H7x7^c2>zdZ}a22gZ*N:qqbpAgyk&M+gMfal<fDG:T@7q9jxE4%7g/$HJ2BYu9[XK$m7MI=E]([t3sqmDRvvPgV>31$",
    "v6+)bdN+grHWL8L=zHJNgFna&uD0Qwh{(?k=LKqUd2=lX3y5!0H{iF=gRwjKUbh9hG11#Cd(s4=9pSg9DKJ3Jq=>FjcLd0a0MZ2g",
    "]1-pf[/>1{Ytvc6)VZ%uk?.NqP3r88tbX${bFM=l>[%*)mEF5SU&bTF5B$9g7OFr57pSIv*(JTx*Y.&nS06@ILo54mA4Kyugoa5j",
    "rhu&-tP<Iud!oZRyft[@7:PTarX2dN[m%vlQ^)5KJT*R6hZHk[Fy5bDpm:eOIF4e$F^T!b>M4c^{ZDKM}P.do-vre$b>vG?n^*el",
    "Ggf/AvF[IpD*5<uJ!zHrq&#C<W^HAl17(HoM@!QGxhrJXvGrh!(9r<)fc/7ju$<EvAohzUVIu{L1Z?btZeW@>txlZRea%YwZ?c3G",
    "gxPtBp:Q9zX-%Noe)nB)T$O$HTyAD/FzlqKX+PHz7(gO]aJ*}4c[vRFnASvNS9]]]PV/NkM}Nxv6NTYg8u?R8YB]t208P4Cf-iH=",
    "&b+qGA)[yf1K?.ilq/OM01rN89B4L!cnTh^.=)>&rU+i*&kR[k-PlB2II+uDfH5:OxflwZ]:Ua*gAr2JY>kkE&H/)devrX=sEogO",
    "UYs?G}l!Njol2FCVqxmtNuZoa-?]Z$xu[pRRG(ia?56J-!{/9ZHG]%9Sf8kqTlMv((&()>2UgsEF:!<!M)Op=Pa:h$K>y[(ADDN)",
    "f08>3{-dvr.y(qifA^?bZ+83tkpeLc+rAUNC$w<!]gFKp<xu{{OykM[Zx4SN4f2Uc5T/i6BNoD1C9PXMi1Tz0[ThhEsgwWMZq1Q2",
    "y^OorTBf@V^DLsa)]A8{5}d?-VkKrrmUS82y4z0Y]MFhpJi@h*k:1#bo2>IS{/vLY4/n&i4>Pd)TNM!mgw-)(m2>fV]4h<l*@75z",
    "h${6f!qg[5[@.ef+x!/PZmXE&lOQyhlYnYy*QJUoWC}YXO9q1TJj>YAy#EQ)s]<QD8lRJ6lPw*A:y3NlP2IwR>*mdf0Nl=>Hpbn&",
    "/mugJbgzccQ1DM.oNJW2PZ[?>8?2N!zqAdzwf#H:dwp}0C-q77wOXS^l&qo{*ekh<gj37o]$lc}JPMr=W3$R<&QAIPYt{LTDHC-n",
    "q#kH/+[c)seKlFM=>pW?tm5M/H.lmxRWAHu<.maD}]oL*LgNT=t==Zza&oQgo6/qp",
);

                const ENCODED_SMALL_FC64_M3_U5: &str = concat!(
    "^BOW*Gd]f%]OimQ5]KXZISsGBp@l}WI1V/>jM<(2+/NYk8:T=[8Yb!xQ{Y2wW=B<f1r[2.s]%stZ-f}#=$u]MOtTK[L:2rl-2XuX",
    "=r)oU3G5JW[%$xCrqsc&b#RaZhNmG?P8Gx^:]}P8Y*f7<gj.CF{+^:?aNt@s8#$vHuB2E4b-X$tp{I)<3}&d+]OBx$&ca!9l]J2$",
    "[*028rX}-UA@R$0ZY*2=oXS::U*@>sy*z#.y*x:#FhyAwAXc#GaHfJX:B1@uZt$%2Ai#Mb1w.s%Ueee(2.q21JlR5VjrMByv>FfN",
    "z<<u-i6NZk!{$rB{{4B{8Ur)t[S^&J-6]l&@&A/D81<38Fia30jI{x%g7glFgJUulL?JvPju$Higc8N4[d5*sPV0A:C>O2FedBOL",
    "Lbh>Qw>k}n>qFTt--zs!Fd/ZZJW<+oPG<Tkz26>&eN4:eySDikjj#*izyMq-0>nixb7v&#zt7F5MzK8:5A-1o0yL^b0l.dJM$TA2",
    "ay?B-jOYNLk!TlZ3A7<aO)6ENaMwzzKyX?.H=6}2UV*>oro13yVpTM0]WRTd2UduEZpApqAbgryDni7Cl$C-0gq]}JH6#{5m9+0V",
    "Fw53BKO9Rak&In@}t[1ns{)%oq+0?28Z+qyY)pwT(zx$i!}%q!ROniR=%z:C9m*uq!q9UeM@*D7*$lwPi$rt})v6b10d{R4Z${GM",
    "A>Q$Q*}bm68HmPhG8+Hbx6+mnTxl@owlK{PTODbmfBI{mex@BM:FWd[bhd/ny.XV/c6lzo+V-MgL&iW%T*k&dJ/afb21tBN.Om?s",
    "O2:YBI{dRN]Jz+*J/{(+-h7k.mJgnHngGATy#c}XBy6yeLM%*]sxqQ/VGWuiOpICsJ}Ghhg8bRVidh*XazZ*nsx$qSIFf/s7&+zB",
    "H#$qx{$MGBDp$Rdcj)8*aLIucnq4>]b7LAYIpz%E6ECfv!YK@aL}!<]7JB=ogSx8K&(1hE)}#QFFC?R[/XbjCFi+!I[eDda!o^T$",
    "NDCr#m^ckMgWSY8PBvB9/686QXq/VlDj6d8H-GOXhi$wTFpx2Yq61NSZ$:J65oMbUKJy+v:IJ%2Ad67DfJTyIA&CY}PucPpqJ0AB",
    "soBBAL5p7VLR$$AkTXZrotj-DlWxwfH!+!15w6g1/LYw9-+?#]P2).ux.:FF6Uud:BMGU*PWq@ow#C8XW8+qw<Go9dc)+3sXthKa",
    "uvn85DpirE=}(8Las2HKk-]kksFSX]43GqSu}m^&)2X2a!i/-?p(UaWH=vpQZ}^=HO^yPA^b3B&8DZcc:SbQ&^7CNQF<#AA08uIa",
    ".R%jZkS5OOCP)OW:%AVyVgy%UTSmFR{ivg.e)2J}0n-b!b.wMIuIy#[M)5Mz*55d?N:$L6!2+@1*?7/A.0<9xGR9a{>c%>zKHfH2",
    "no[gFPB+LC)RXo5RZ2Ulri(cV7os&aSobzu]4@YCKHbP-ThXBdX#jHIRP5zBs@0pnXrAL#Qc2i+g):AtQ+Badk5ZrYP5tEsE4VDZ",
    "6RFdkH]T3eA3aNiFx0X-.A$As3$6)+]Cd/$hNYM$D{2CjPCn.n/Ox5YLSmEiZ[Cy6f8ZWv>:P&gVFD@)!YgrCCs(m9OVmSj>h#q2",
    "28f&Q^O<X&5bSZ@!/S)mUY{![g>CCn*hKzZ{6=R/mQth&+&6>W+RCny[=!f?SfG82p?%5[[kC%(UOLwzttjYMRRMR.-bv=K+i$PU",
    "bVJ75*-ApPPEt@n!h7R#35k0lfp$hq[)8>(e-VMrRRu$(Km2X8uf?wJhTQYhqE^3@OJr]pl#/SFK#onMy(=X3ar(JM=v>C}[r!wY",
    "XyooDKJyLot*/MlH/xzFfIPwDx4S9RNNtGojr5mM*%#OvGE5T(3tGHMp*!gN+NJn?y%vTDRL&Pu]P#%GxBY[Z&aGbOT%.=k)l?t}",
    ">!k}r{&s?C&/:10kWjsO<s)8!n0oDb?h^-jZ@3X}H{!6ZkS7Qou<Z72MCXnp6TX7Tm/8ZrZITE]5x:x6e{==}]1{LMp?[SC2^:j/",
    "*^gCv0^au5.hB4fZ[v8U&jCyuS/r{&gr^qxj{mSzo<aEeIO(tPy3mGr&Aqd73NAu$&/XC$Ukdv%5{XvUo^%vil^Df8FqHc=Qu4VO",
    ")LxZkDyR4H.JS26xZSR#Jg/pCByvr}kVI+=[qU^T=sD-ld^K2@t/ccDjt-.ACMLSyQu*z]wg*Xw:v<ccc<5J[Z.lZW^Jc>3k?&Oc",
    "5mubkZ=zuPu$4^4bIslO6=eZv3J^GzVpbM(OrTC@1roOrHj1V/026un>Rvd>[B%(l&96:9FpNHc[o$:cvOs2ufDtwJ>P#q608K[5",
    "PF%{8U(8wK5sh:%k+V]BKH2FhvcL+BGDYP[@n*><bX@b$Qp6US7Y*Q0vccc908MiDL9$2!Q=A83z0VTm6WFUI:{){PTA3r:xPu/u",
    "FzB>JU0F}#2M:Lf05*(v4=tKA^9Qi6.C)!NU(p4NF^6Z*!wuTEujchn3shWP+:(wVPJ/NL42Mw{@e4[@nu8exH&VRgZaVI16N4U.",
    "4.o(#p:q)JioE}qBr.^s1q]1?xVi9EAfsCsMvv=9>O8uank!>rPG7{)J9RVhekf-?mmAlHCM[KsA5#(s/Q9KtfAzS}azV0WFxf62",
    "{#xPhP}{WgNU0ocg{}u!k-ztGbceR{k*@^@SglR@ayQ%9EOC:jNr$bVxBfQf)dan(M(zeJFB=k(D@>ORg-@eFT)sYP53R(]Vqnw2",
    "Tb=10X0-QC>xi%(Bh<PYSJS9X&-Y^8>&?$cU^GfEN68N:Z-@]]G.93[Q9UEQstD75=mWQfCUvx=p=UX>JerUfY0E457Z)AdS0]rh",
    "qN9vqu[aaG=AvdfawlN:uFr<>Vh?wEidc]ML9<$pM{)#pOjNcL635+u:so-]sS!*0yV{[[MOp().0^8[7+nYKhaAlYB+RFhl}T)J",
    "g+NSFN5eO2nfLr4Z>B>-/C/Jc0]yNMOphG(@gNSlln1]]y.q-wZ<:knU?q-lfLPZ.F9%.RH]dmjFsGU62+wj.J?U-2D@T/xc#D/k",
    "vsfDn*6/pwWWiauuJ?Z*r3tHgqswl-@2kn>f-v$jlKsq!%b{<8FD6C$i8NTTN4<KwSkqa?pkT>v5sFl++-pmqncZ$mS6N1qAnuad",
    ">.dCFiieobZ#vL<)PWuqg^CuW-06!!FB^-(UPLR.5uVFQv1MG-ei+#8KG1+-5nx?hJpGrG:AlBPLF>^foX8<2:J?dQfbJ83Ac!a&",
    "4MHsGJ*Hx(6zd%h!sCFW^2Y=>3kI:q7%H%KC=f&tiGz/Qrgb.Wh*c*/t.gr^cq0)z@6PnM5?AAl7/2^#:rD*h!2(JWY?zCtGE&]Z",
    "bWI4}/E-KPwrN=(ayU}}RubvkeZ.!RC$7T{T$<DI>VciF0}99LE&WjQ)nGrlMi0dil$Z!V^0Y7Xrjj[LNHaZiSewhHc<nwtvQwMq",
    "KK@B73%6{L00UwAd4kd?0{/9:7.1ky5A7po&7&G0tgel<wk]Lwg^ff#.wAbkV)FkP{qW3z}L/aXl<5Ai^JWd64enU)Kt2k0VZ=e3",
    "Bg{M@6iEsVxm?c/[LU7Q7x)drk5$}Z%5$]8VjUl)/UdCJQ^(upPIIh(PRdQ:(eS%I.EL*zG&JcaFfrJcelTbU<s0O.=}.hnwg4^G",
    "=<KnBttG#piJF::rq{t&m3q]g@4UGu2?VECu}maTCD%$b7*4D=<}2G}]>wwdotjT]&2mbekkpj5L>plNzlD2.uNq?/{=wjk!u[.U",
    "wdvzeXon}bFk:s#xSRw1Cer)=6DjA[epkdB[zy{)sEGViKABEmsw(E/w&dp^>OWHog[j0FSTCAiH[SoWspG}@SaKjnXl+jV4eH=6",
    "CQAc$h1d--.Uh-$6bE63O}xc?^8Wj>=p!}!b9d:GPx3nwU6:o-]PDO{WQ+#y)5/V*PEMODgZRe+p?b*7kf%D@{hy!XLwMvxUM.Q3",
    "cn&GIsGWN{(&X}7E4JQoOf$)hA<TEZe)]X.ljpSy0cUMU9fp^@GUn$J4cqW@b9[[mxitp0Jj1m33+neT?j:%mRF7H)ULPReXNi@H",
    "g!<1z@YmQ22MPPxL4Qe}b3gtGFdEQIxTKRGt2Kv[YMwD.Ko?8:/NVHP0m]60X(+YuaKr+B0bCAAkPSn80d2qtS7moe0TXe9[t^)0",
    "PZ$oev2@{KxYOOAynNJm&oEp7p[bnk0mk]4Ke0o1Q^b?WeNWULa8D%3^j=Xcutk59Oet3./<U5%).LIU92*iq*l*Qc@gVA?Ceyq[",
    "C*}%u{BGud:XMb7KBxX.Kw.rai0dKvD#NE@jo?T}PM5{Ndj3%!RQ3c[M$QDL2-c+?F[8%p=]G*)tNoaF@B*<D{5ar[!19{hT)1$i",
    "b+-8?ay.:>xa$AYE-%l8s?XDg%eASAA5P{x&*[i8[oiny@U9bYnyCHv>?z6en{2}Nv3y4wP.Vw^wn%9jS!72W*k-CKREXh&7#!Jd",
    "6O-IGsH3TzRJpC:5ngF{KK2{H&31TM7eIt3p(p=a&c]x6XfX5^[jih+7<ebdh{T#TA+48xO}rPFG.}c2sI/Z7Q4^xR1YlqN*rtw^",
    "Z/J3FK@Z>(G3&q=b<pNTu/MO%GtgeBKt/P(sF{Ti)4JKO+*^YD0cdu/g:=XAF!y8SbOGl5C>G#T=ITVPC+e+n=HyekfNOKoH*In/",
    "Wa4aHtL#NR:jMqlCkk(pU#aogeX0BDt?SS2VtQ*GG9wnrT(>FN+PQyToJ.iQN1W#xpog-+O{{z%(yUk)NK2FT3nOV}{l<n<@fmS5",
    "4YN4M2^g{9!m8xu]F${AUctS6@eS{5Ml7^]r^ay7P.s2K4/C9B&f}*2PHUiT66tZ$fKtwO}A&<#zdYt4c7(?N>SsHi-sByZ8RZ[0",
    "9grRo5&{jXS2MO!b(t?HBA^2-wikShn?4u+E4KJOMkb9pDsX/{]%Ka$k.K)9=Hx-8U29--lopatg%DK]YeSJ%c8ZY8O6v=)=uORq",
    "F!(A<axfb?ii.^kUQ]udP*Zj<sWuJgtj1ji+oo5jQ<qt<{O/5]mF-fg1m3-pyhb^[RLWASPD39T&9n@rglFPII1vTWu{WM11L*TM",
    "GLmo%^1!+!7bg?kcs}/)>aG#j2c<VhCYR6w>.w)@0ezu4ec>dEJnH<5M{QHfS>bsC4=#]/PWzmJ0aIwdFlQ%zPXQ.Mm3Bxeqj/Zf",
    "Me1hWw(I+[?=kYI3=OV2]%#nf8h2Z/KKd<UCk6oBca&1%gU8:]O&!jg7/t{#V2{NuW&NZ%SOAH%CKGSq)Bb@oL#&@eyn!Wa0TQkr",
    "K]+8oL8#rIrPeOAoRdhb388G6LOD8uaL%9h:ycQk[OH0Azp=29R:Znw[0lNegQk?2gzVfrkh82XDePWQ]/%Ih.W@uLd^$ns5KR*D",
    "5jRt!6P+)H&fOQHNHh?O4WzPe(-(Tqct$xF5l[[w*31tTVSA:jyyMoqvg>EZ<lp%9.*.ID(ob!ztw{u4zL%f(/))J}nppYS?Pb-i",
    "*Y6:SL!kZl?g)HAALC>{W2]=1n/eCVSI>Xrf6if#43cxiz80GNCfSQ(sDq@oGJc.&MX{[?1ZZ(ohCD!c^4OruW=nt<}mokkvvQZ?",
    "X0SpwS}}Sp:sahM.dIE:n#Uozs7Lq)AODF^/^vd>XM4R4fvkCqQaT{cZ:Rf7HbuUdKIH}gOhF$.k!r{SiiUXjU#oC(VT1hcjRSYo",
    "6FR<p[(Iew=1@9Fv0BJxNs2JeEZ*)>Iy*5neiF$R9=.R5cu%@5TeoI[J>cD0<j-oJ.YFV/]JrRaH0b6iVpyiA{%BQP5kC<h(}%K>",
    "7C}ZCEsw=v4A1mvb=2$7T#YQ$R-6C@/Am[ZHu2l%x2OdvUJR-ARfC0::1t@SYpLSTUzITf&i!xP?..vDHGX4G4g<ann*OF>ZZvoT",
    "t1#Sy}GSJl>RG!lO+d+4+g8efl:6cD1x]q0t#p*u^x7:e0kqvB)cvAXFRVUbU/Wc-V<F+ah[fO=dd!Jy?XY$tZrUy{Az*Wr/LO>@",
    "fnY8dSD-^7yiTzQS4I<GcrcZ4!jg1-KCG%K.lw:fF=!YXI4{B.c7}9GZ#S%bvf-%IH{B@.[GJiwpf+K%y>CqxZu&J%Jlv5wvca1y",
    "]]7*)}NdMYapaG3s&zyB&&]^MAw-qhpIuB=g<&]HDYEXX]kWdg>[(AAvvNOq]PMssjWj!qaI=8ZC?gqaf4.I+tscj>L}!7TOKeVb",
    "kT{ys?#k]9R$vnS}BKn)H)w=t/n<Jcj[sN@A+VPcvyRsW^t/}B3Q+K?Bf%Pj+)L%:Q>n--nzbDa)68bPINpNuAndUia8T$v^9Q{/",
    "3rEu{bqk7<FQr+FV%S]0!5GM])md4r[y%9*i/uS{-yhvwCnV^Zs09[}SR)2+61o%oFsj{1uzKQmZ)!m=Awieig^.75jo^sIpV&@^",
    "j-$zI)J#Qhn/$WV1JMtjjWaOlRs.8Tz]8Jsg9v&aqUT0J]]cWH1]Q3/kUv81-EDD5Lqml[mH=%[IAv#OC+Oh3.9l.?Q:vYU5l2pp",
    "s#shZWGMGw/j94ssRsj*gD7-oGxdUF*7)&9*[l+[9Nc2@Fjh@wNW{g^hzp4NUV!MIb9/HFb*Wsu(+LLAi<t!K]H1+Oc*K1h&V<NH",
    "ee>v#+rpXCN@IE)yLLFBfT@pu}Ga3$wAdNwHwzowQ(VDT(=^)xF</<SthCu8iIDeWPxTS:C%9H*8^i(*dpzp1fTNE=06y&SYm0Bp",
    "Ah/)3kES?<?)5$u*RjE=vK><1XxMhhbT.t&QaBnzb^[gDKHixG3TtQ3(uo.Kd(H)3-#2WTJ!I:x5hWxJX4f#3z4N)P]>=4<OV<4%",
    "66ap3Z[k!:NHI<5:/{)</lfbqU7SMj4sn#cHyl!?wG2l@rRcCUViT=2o:S2S5dA}I1LFKASUOI%9u^IU2rwIx&dg?>7=y+C5w$[t",
    "Th60AY[atu7F}JN9VX}dL0fKxn9VZgsS%>f<dqe)O]/0p(15pc)gT]Zo?8YS0e=H{Vj+0/H$!ccif(D+hD$]1SX7r^H6/6q>6AyU",
    "E}l.=M{1nfdfKrFSAaa8K!tf9%8wAgoQ%LBZhHksq/PrgfC:q]c).^9:?jAkZ@cgmg/ri{vQ*[4ei)-fBx#t*+3Up+N9*4tE5?.g",
    "?MwQ$Q[kMs9SXTxf4R!>N5(1P7=aT}&2gaVaKpfMciPq<5lc$s!7{80?dE2c(0Vke+q{L9y]aONC9(1r1t{.7W7rPTuu(9aX*hQo",
    "8KYM@qCh8B-gaRbSmVqTSGZ1rl3JNEyg7*vi&AGK^TE6q(@Mqkk#ch#LjSGMe.%dnxB}dbCh0r!gjhtJEQ>h$n!MedCf8.TPNxT1",
    "FGf22(]s7u^!QQuis<izu.[gAvQq?]NJojdR$IA1]Z/NQjP#]([ZVQj)5yNWNXG<D[a}NF>yFh#:LP]WdL=Ywtf!Cca8}R4dVb(y",
    "<2oK[@0Vlj>t9G*9$U9VKBZ!6({A>HTm?{{>^!{J6lQip*+lAa{.rG!LXlvDMuniLD8)^Q.pWTd}kgX^<gH+)apcYm{C]]J1@eZM",
    "OWGm%!uMkwamF{pzW@KakF0^StD0YGJd)h702&gcRK%LCPFrsSqhQV5engwlr/E5wGrya@6?7?XIQ(SdPt)G^I+bcTF#QjZC4ihs",
    "w./:q40ouID10pd^Whjx/0[w!H)Z*$[mNLu]Rf^E7@ESjZ}YcC.uJ0ld2[%7[[[W@2QPmeUW>2K<GG=vsOb!hfWlTXU+Kp]FkwJ{",
    "6}tT{(+Vlceg[lxH$ARffqiv062+.LL2:VUL0.avM{K(/<Ty3?666Y&{<a*ip4GR]p{1H%g5#j9bTU5nxKnehl=i@T3gO0VSiF0t",
    "Cql]%aV^F66WT3CEOH0U*.%M:n![?GC5F6RXYYgXkKH$NjpkHX6<DEd(rx#DnK5[i{k:HkMk.V{4mASGC#n&Ji32<#=znislWfAY",
    ">TM]j/7f!z0>w3to(K]jzD-n*A5j(koZ$6ROPpw!oi7z[p-bM.Z3#K}<dMrkVt<elkc$Wh^$6$L]XARktSd:eSH7391&9ad8]gw{",
    "GH4012Go=TK(7h(AfrsDAex7Ty#j0YJc[>]9GXgKnyDP.-}q$*Dp2!sRDi#o11ZmYo8%TWipr0AUW^yu98XKQ]V*2(]C$7#N9nR}",
    "ANC#zz(GFB@x=jM+w/bGnJe1W(>b2^>k)g#7K4#9^9I^a?@v3++hP+wJnR1Ow$&G[1UjQSz5z>c4gMMa^cVL2Lutl9lZ(K!.nFb)",
    "R6{b)ju2U(B$fNQ5sS?!)tRPt![m=xUttUuJ*>a7tM!p}3nkKCFwe(-ZhVDMv-JVa00{4%k*KL^A5})JfA{J1T%ncO0b0ci[df/T",
    "BCLxffM8fI7)jTc5A}Ub[pA{s6Vnkf4:GeWqI]+lMlJ-G!6+fUA18VUrEy(iSDR-k3>amx<O=zGk$C3uNXkH3Xh2qz=r5HS<vLSI",
    "uC]]e=P=uWH7h)2je<n+b44gWl.[lRvl3G=yRt#%@a/v{XBknql)WWN<@SFAbKOI{ImlT54ZFbV+-ji/PSCvWYF?AarQweDdR#1D",
    "@!^dcf^YZXmYM:dl4@-hzzxr$Si51(Y]wO@D{te8!^>?CsGL#C[5h[5/B}^V=>!x07O@xv1Y6e5lFjich91F%tb+xiOiXEo5:.qp",
    "G&r{]06Q-JL5.*i<@UQcSR^n<(qEh!VW&E%&yi-eN0JHg/Y8bd)6HT]IKMN?Z<j)4Qj3{[zC&Kfv4s.?p]vX+:xJpv05**&yuxiz",
    "[vSl{{62U7KuArJkX$OSjFe^&&p$s9S*kRdYO/9uGoO8N2n9a.bV+z?Oq^55bY1R/aK36kP=J+*T%@LD?)R-j(^FOyUTBjfpGmJY",
    "-^}!!7B$rPKr19UpJM7&itGp>G+?9WrRRG!}Wq:OBtOUm0kG[{o8ulnoT-=?fgp4G)kv!QZLHgG]@ChR&96eAgT#y!dn+]mo2MuC",
    "tBH?>JM.z!z6KCy6j@EMH[]U!**hktjg[.gYgO6@hJSLP!nb@oy@I5<c)(C!&e7{Q1OYBsSq$$(w@2uUo5RcX!N*{c:uzq)]2OqM",
    "}01wluzX8]P:9Xzxz<}Rq&OHM{od65PeNoAJ[!.uNLCt^AF+WGqd$CyU>+N*N.92)f-4.i6$I:Q}1fVZ^M[eMi9.cIhRzYRyquJa",
    "E[k8M=Y3!%!]TG$?(c(Bp!=kH0/5$NW7LecTRS1(nzqiHJ?u##N87e-:H=Mc]C&a&SZN(v8F7G:jE20wjHfTNfIM(b]N%263IN=s",
    ">?GHUBudpM7&np%R0g6b>+rq+/2YTgzqYxEF#vFstV?YMg?1!B:x]j/P!zr!o3HW8{pQgmkRb+{<IK5yt&o!tR7C#!A(Lt$uuDgX",
    ")Cp5>Nzuf)R&W%)ddpfIb#GW^O*l](&dm:*WvGDAJ&M%fv)cIG5]2xHc&iH0hRIU8P#gReE5//La2FzW4pF[zO#AKvezH^#N9Ir!",
    "U.@&L8MF(.A0&Zh9*S9QkWY!?L:$Vn>r[S2w7Nnn/eN51dJA9n5m<>nCe{e*zixfd",
);

                const ENCODED_SMALL_FC64_M4_U1: &str = concat!(
    "p1szIWgk}}d>@kiSKghslC-obkX6n{US@4Gc[v{6b$T:)Vb)u^^eW4T]=rUq06)nt07=$1&a4Zdpca+9?{@x5T3Jt&0k#c93l-ke",
    "0TbHoF#OQW>?m)S^/4!h4p>./^hS/-}elo$}V%+@)kroOp)j^vpGn=apw?jSBjoklx6cfTIAf6EnLJ!XW0<!{zOID1/USn70gjLn",
    "LSvTmFBl^RMH0=#p*(vD2PtIjUM&&BC/^Cc^aJRaHumqQioa14C+<dUxMMWaasVcWU?gY[L<$nbsHiAs:pve>kk*3}Je[YpMyiE<",
    "uGPUG)X#%A3Am7tZnb..[U)!Eu!3}}2^Txu@-1CjpsK}WPV]kP+RqWLy=?+ZZ&5H4iQp?0(jkCV<pkk7H!+E)!&Hr7kVA4(J8}8:",
    "9qq-Z1vT$cD!!//Qt:[t6HK7TKy>BOk*CANeCW&fG}FGg]Eg$E03i%X9Kw*hh:7$vY2*p3c1vAG!(y6W!J$-Ge+j=Zvv%2L7YEk{",
    "x24RmZm6meHQ>Zy<tX0&awAUnT@#-q)6HE&Z@w(ab6yaMAVyi8@d0!vX?I4nmXm-n&h}(0i5HcqU*%h{UW#p%TA!0.0-d{>U+f<!",
    "k-RnAYZxvAS]hC>%9j-gDIyb@(U?Owoq9&HT!HAZ7p:T-&Wv%xp6#qfBS4K0Xjd{XV)hb89ccblDnBzvO<kJMseFT6D:y2>QnlwN",
    "BqFBI&btKaQ#1:c08])XT$hrvC>vT(y$yhJVpNzDC6/^[GUoKZyYO^]7^IGxZZeYdo/wn0u)b3NoE1X[vyS[ZJi<@}WMI#3]b]6!",
    "d2A$%Shx:io=iMp/w=S*>*RNP)45)z>g^qS=)50xKw#YKIO2N}ghz+gsDzBNp95sSU8qWh4Il&EDFK=F=%Oci>-Ueyas1Fh)X+.3",
    "y!jMsTG)I)S+eU4WRrT)y+!h/Z!Msu@4lZq>Pl=/Lhqu+k6Xbg{[ln50c#Y13B2gE}x!!EiZQ-$S6T?8pkOi#)5s<nqL0IAE4ddH",
    "uum}=nI4nj4bSW3QDym{5i/*2]C7-D@&J/CmL*<NEJ#2ey>DVtnBrT{3cr#C<O%kkee<!!YVFmdr5QgdCAy4WjzhZFODB?:tM%]m",
    "v4pjX*exJnvpB/3G56}IYj%Hxqt(Fr+dGEw[As7sQ%aI9qEh2&t$Gi6/BCLRp6X9*ns9anA2#$YAg{)Sp>$(gciJ?LaIt>f!&7BV",
    "X<K?c06mG$-fNEqJV$^x5fjoO/DOQ6ijUG<y[p1c7<.]s07MZ])^lDatTE?<[=%[jHx9*SoJKzuxj<N@4($a.z+7{ksAC-J?MpcG",
    "mcJZBR0PzuIGwFOg(fq/4t0d])g^b]UMou?98CC8.OJJvJn-OQg>x$^01lL7W5mB$FxGu8@fAj6C{}34<#Qw)*e>F>65/R0W/sK:",
    "4AYwGJTl*(Na:tWAnI%9b7}vc+7e.tTe6q<Qu/SUth=?KV=Q}kK1:!JF(q82p{n28B-oVh{2G#5Flq]=Ode1LOtK@LFlQph/S%RL",
    "UU%q<ckI/U.xN#5@+we<+?sNg5s$eWU4Lganh55%H!tVI&hPmed00)3/A-.n4S*w/r(@4*gg#t8Qw+uWKkhuJ!q4Z2aqnd@?sg3I",
    "&=Q9>XQzud5bQdR5lrtQZ#Dcx-dmOBZY>aUXCrzyV&}})YCrK>=yS(QdO@@9n[(pdk(BtsM)5@LN5%=fFa8$>O5kH{F1(u(htq$<",
    "Xm)]=pmxenk+(mXhBV]AID!s*4q>2^QC(@2{m8QQfSC%Ne:j6nPGKU=BRO/bY&=WBn35TKfh@ct=%/%qSweAReL65d:#6q=C&Ngm",
    "c}R0kaE<{{fQ0l:UY>aJO/VN#oU1iq&6L*NKqG#v+Ph!UC69+$S.H4qL^h2!C<+{KzIjut7%my)k=h<&g@eMyncCJ$UY]@R5kU+T",
    "Xh=rb)/=MLfGQ(2bE4to+(GZ=hO#=iDg0w#Yo.(#FzuqG=>NS=G4Wj*vN4T>].qVO>UC2rSY*5r6pNAli4p=sSjzBXndQ?BLYS/^",
    "&)@W[m!9K(x/@T.JpJ7w<Fj]K!fRPqp)Mvsi<wuwdKtLqKKbBuxiYWB.1zUgWs3ok@n-Bxk#ylr])wLV58XA<^r!)cC@DQ8HA]}+",
    "F2#12?+{afVN*mk0iZNSTG)^F+VFdlZ}hV&<u}Cg=qZh{z0EI6YYw@My#KV)03N(B:Cd)JUi1#./Rh(?>W:DKPExowH9>G+H]V(P",
    "2:OD6v7iQdU=}uA)mZ>J@L*PRtuy}<K#]:e5}jpOxO[vOHAg.842DY5o5#0}>/}[}h2wvTW6#U<iS>CR}hcH.LF[#+gw2X5=O4k>",
    "*[yc0IN}cE3%k3[=ow$QYZk[^1C7:e<S1ijJMF6g@cvCrZ#39zHYM!NFcHY<V34$q?tW!}K8-vR[w}g:Yg+/g:yp5<m]b%A]19I/",
    "S*e5%+hw5?Z-T1GZ@R%ctw&B-i>VR0.^T1/Fg*WXxKUz-&]MW[nDdm7WJjqt@UB4$kQEzZ(FYDuUSRl:-8+N8-1oNA7$O]702h&K",
    "Z&PVwA5@#>[/k52J-:4Vs%Q5W&pv!#Q+e91Z$RKDXj9CYLCAgZSZf-Ise-Eu-d&?aF!%Y^Un{PRQ@!cI>>$6nJ3(rLp^Kte5rg[D",
    "T*hxE-AvjB)VD)2&daoW9rhD<GAtmsFi&/<lw?ez(CbLDd3$=lDoldh(D=ru8L>wop(l2J+*.zhkW^qGYgpw:cU?K2=ZcV9(?P5y",
    "qY$tqWxvfQvSIJkwt!GK=BM21dEDYtk3F]s^(Y[ebhWMb1rDnX}PcdWNPa]5=%l%>UM:UYu@9PiH+8=zH@gk@E460bQnTOBnnVkR",
    "xM8wjmSF4kF&ok:r<PxJ0awgb7<JhCk!AvC+?s%rVp-/-oEHVt&o=ce3PDr]4k>NTk4j(k%k?zq@Z4k.5QjptzQRX3R4}p?d=z)v",
    "Le+c$Z&{3%niaZN/Khn5<z@p6>U$!JMI?GgPGrN}&(]n5Xnj{N0kOcnL6DT.EFCSXVr%N]d+8c{<LL:9GA(n9XmzOL!!1Pt?2587",
    "/rJEXX4@x+3PqSY:AwdVEIZ+MBu9^wK*TNtUCH?N9VtK4[Iu=JQ.dBRjf]tQ6$M}1=R7G9alZzLe^H:RfS}j:gZmp1ndHY[k>CZL",
    "veT#RkjEO}Z^/6JFe<UjAb^Iz&c@<iD&P>NVCtuX($imXHmE8k/Bu&N)aGGzu#@Cju@0Ac1Ptdf2]r1]=9l>8VKLA@=ztc=g!yt8",
    "M@WC7RN#Q^e(k!pmoiRRBvw)pFm^[9WOlqa9gUl^uF/iPi@H5j-jyJl<D$IjyesZ&DEO-Nv6]7e9WyD[1w>)Q3VNM9Cv6#7[j}N@",
    "M>2d?5g2vWoJ08#&9^->7!EGo+t]j/NxVJIPe!z:Cq.m<f5[s)x1-EL>2EH9!?78*X37X58kGRZ/&>#0fx<Mv(n5Yde!fzBXC?4x",
    "PKN1B>12zl&7(l]Fuu&82>IW([kk-tZ#0ONvKPje=I&Zm-+yKF1Jzg&4T=yiSeZ^tm4@tFAoXGbmHVB.Aj?TMp(vcy-eV]x{Nt9{",
    "5d4pdXF3Td3/w82nU)>3)J#!+-ozlO}+K#u<%HLKXeqS^V$6!jteO?5^7U(gCw!cUoUZnj[2^f+ebMLmk:0zal(<y.U*8gv+>F@!",
    "ggxQ.wa#rL<w35}A@)%TRT7j{v0GvE2Uu2HYRI+{W}IQ2($WDJ1}{:Ahl}!DUZ)p$4<V9Rmt!v-A#p<*n2F><+C<18LC?l&P:rM*",
    "1v=4$lkM34FI$xUpxfQ)US[slFmcd7:{i}&r[(frxSE^*@XP<5Jn4pA/Ju](soB^I7g9uvXtv?u2YMPM5o>a!r8F=vu%*YieqW:$",
    "=I>#1Ku6CQ)daO#SIJ6F+0nrTWP<rRjHiN5KiQV/Y6o?&GTT]zV-6h6B!-+o0WT5zuIZ$v)-+gl9bORq]COPeA02TA0]Md6?)e}r",
    "7}d2?+2kjLU:]%I0)^@h:tUk72HjFPHr#*h(tmfE2P>82EIZr[Dv9j^sq3.dQ<BP9OJHI$P0*+{)G>IW4U$n4H(ol=Q7hGjNtn5X",
    "oP]Z{lEl%>+Uk::U&3heNSS5Lf(Yp?71^B/V@B7?Xy1)3!ghX9&6HXZc]F9srca9-<wiGMZ^KCIww@M4Fp2nES-CI2N7AZsWMgO9",
    "^2oM?H*3H=MB@3!edwe:xH0rCCUkGU3[SbQl$F9!t7E6}<j=PbAZ-O-q*IuPG$@2EkEjuMHi!hfR4Uyz=&LdHZ:ULZc(I#%k?/OS",
    "pn!mUrKtxicsSVhyC>?0Izi8:lxZ[5va5#aa#Um<n)3p2z-#/q@lVYLPd()2oB3u^{v:GVM[ly)efjw@R[UbM34cLM!Dt>oIOtYC",
    "vxw#ZhL6+eo5.]@UxkgRNHIFB(G0Pg7O?i#i!=#:kvhb*Ahi>9dJL+afCi55jqu%wAdbkVu@C4T>XvDqL<F9bb6AW]k<tqZrbSpg",
    "B{jE2N2:>MclIbdEbmPhzP*3.[n8&4atd+0Kqqv&Z-.4p^aY<e-5$9lhak8uM7Z]oZ6bikO/:Z<e0JGq=5qoTQ4%?:ROoZRSCS-&",
    ".R<Mtu]<AjGJg7DPIk6(!6^f!OUhigY$Kk8PX6(vo5D2H0hNy)1wu+F94NF>MrrMnSo!mZ*h&SAJBG:/9:NO+RNzz@p$&-F&?A[B",
    "lyRZN?8r+C?u0aev31hG&3MJe6fhgsA<*S+Dfm:.T4E4?Rnz%<fPvnt7@)KU(i/Tpv#KRQRn7qtJosUkc@kDdd-.fAFoX=t7$:Tb",
    "2MVIu3H%3qfWB]8bN5w8srn}PbXkJu7-J1U4MA05FzLreA553sKoMN=da-M7^4agH>TmCAJ->ZiQaDhQ[vm@3{8$>DH!@N=-CA)=",
    "kPZiAXy<!z5AVOE}s@h%>!<.&Z{K?rs@aY.nuqIGp(B?RJm1YuoNSXIjSa)D5tuV$[oBYvejA4wKv@28jPm]nB?th%/r5TF(Ehv*",
    "6PkR$A9ypB2/<J&Si)ZgiIyWYd0oF)!0Z?iPAO1MX&DixW+m(Gei/v.*%Pp*Q.[UD8t+KtTofXhwm^OXZI!9QW#jAN3@cHWaon/i",
    "u[5H%{h=P:x.@Vgy)}0fL6u&q@WrW/0r8mD6O)kZ206-q@lk{sXm1O:*qio(b?O*7xJVPzFm]4$}zM!=IUUc58IuW9]ESJ}iG=x!",
    "sIT?#oAZ>H+t-F@M*xWC@Km5n)7gC8mpjn3^FLvQ93wiA)^{N]C2}f$ANqP^aE^{C-<9fu.(:0M90y^Qe1eQ:Lib[hf?f0[A!k#f",
    "P>g&0Cn1)T(xr70LQKNnzFgpVQ-Q[$&Aw%*wV9LWDFx#l2rj&WQZ)%IfKlX6B:dlboH1j{]oR!Uhz>I1rgew5f2*V2La<3ZrekDR",
    "@)TlFXwI@yO4wd6/*[-Hz*+0!-?aY]S@@?{PB]zY1&zrzZ&NYj7e[<fi2tSfE/aQTz*cg+6K@%-A%enLn0>>({B7Qc9QO*1e0Tg1",
    "I5wQ[ieJy7aFsw4D+V#5Be*#VC]Rk@Q-]ryC-bJ+4iG=crm36b:ZTtA2[*/kP6p*u<}UqrneBMQ^d83qzP&wIE0MC&O!u:(XfY5a",
    "Q9R]=55)f:2U7t#](hlwAPGnC/IW2V^w6J4cYjUfE4bduj@{cbl/xEScF)*%NsV.G=#P%^{4^sQ{u<E@G*l/}j/GF^jdE!n30#gD",
    "[){)@]ukH]{/nrDxN--ueN4}.*^GG4g@HPLerf?D9t!uhPET&)>^jZhhc9I!o{2:@r/-e3}U94+TB4W{hzPD&5@/&70Z79V?#[J(",
    "5fx4UkPvK5Duh9XKCy(&45Wb&2YO[s45LL?Z?ETGGG[Ff5k8pKZ]O&}95I@P/^?tik[ae+CWOF6U=BkaE*8DdMkzS%oLoXQcw:9l",
    "tZz/4b2.G9cmn5)cWW:nNjxt)n2ph?4qq/RMKj3-X?YbR6soeKl?=?UJYLrB:Vt>NDPvAP:+i<i@PTzEvzWl3H{CExKeW<%Z{iNB",
    "ZgCN5--R{$Y]K}.O^4:1tmN&1&S/YLES2$n&-[yPNWzL!*sunrt%&l/cGv#!<!:GvfP(h{XzmOdl6LXucx9NuF]@+hm4%4.>KPoH",
    "zqdrtt)/<Wr86{pQVd!c58[{1L%H}Pp(yt$Y7jYw.5F}e]t)T&L>G>%-$<!46A9}O8I?#?x7QX&Bc32SyT4R#7bqyzZHD(GlnHEj",
    "6>E>st%]QCEjbPslR9?j29AEV0GlJZ8ci0p^DtWqtDiormfl)Yf%rBG.xEw([-yrysPT/2sJRNChADTSgB5s3^3E!DxQJD@d-)01",
    "Av:k^t#?0tmseVhA9vJIiHBFzq*.0EO-scw/.P9(&JOFd8[0.#01YKLE{BUvxUn#h02Mx:iv^uU>?g}Va*A@tE:dk>>k6kn9KM.F",
    "SubV:Nq8KY}s7axue&Mihcm2}4i2{z?K$GA=1e>0xdfii?T?V1&-EzsdK5T(8rOSAe/)+K4KE{=xg3Y7E.#YfMyq+ROvx#ZA-rPm",
    "av8<rp<Eb@]W.Ze3F)wGp/C/QOtgS#un9qiC*xkIk*+R8&nOcbzUl^?rj3qL0p[*P2+2wthK3D<H<n?XeV+)m6W:Ob>olrSJP0b^",
    "+$budA7F}II2GiA]{7j]y?A5$:]L%mm17H/FgLSLsy%!iYkZPW}o5+5dPQ&e4zn)SIcecrYBb@GWH5znZ7^838kGNjoML)2D@?7E",
    "Vu/+teX:>vLROE!TlU)iVoFaEl/K6s:{OFtAj*PZXUG0:=J3eyh&uq5D5MBJe=+vN03kpSMlN5b/sKsj3u:YTRJ.z3kA?f)avuNI",
    ":.VF<90&60@OVzzN8R[m0*jiXmPg^0EJNJjPc]pQZonY]&i^X>uDUyzSF]4.wsDob*Of&{c]Om>cVAoNTf{IwrZ#z?QwCu-={GR5",
    "<v@H=+IU-JH$sNHjx(a9-g!r!!)xreUYC#v[=KFaX[&@Z?w8x<Ob*sqMU(:=@WWj}J{PlF784B%xLF?S}=}T:B%E$?rW*6]kYO+D",
    ")tV=.GoPI5p]OcwN/JZSY0HAU+2.UdtNTB)4-z:sxK!59Qu-2$E[H.BTM!VR2O5sEtSf*tTpC#Uvd+>V02?JyH=YApr?hy/XnVJ6",
    "7.nhYu#ePJ.UmlFDwU56Q?Dsj=@48i{r(*LFvecoJ.YQq-nPK=aS[?tf{S$Dskat2Ef}Nu{<MpcNz6YQt)Qepwp[v9f>DcNUn*PR",
    "=UA%6eAnR4o$@G#X>SF]^t}hsh2uvFDOi@m^8Qas<j:S0gji0lA5mg1N<Cj3tltXQu@CH/!f*6=4s.LBLCrll>SD+%PIOJJn!@yg",
    "X:9cSp{I)>=2z&g*EPM4&RB&:i0GM2JgLWn7=^ZvH@GLgwhg?.gw3k.L#s8lmc@Eq1ORJ6g&I$HL7mF+sh1&4Us>6x2Yu1[xi0tx",
    "ejx+al30:OIA${&*){vqu/qKere4qm@kk0/P!v$h4yfn6=tQzghBO(Q/LB5w(ngfdu#gIfGAVOlS25-6L&0Az3H[+y[yH7wC^6SJ",
    "z0%LKfW.w.Z-]qZKxDFn>NX(JaA8kio3.[0J!w@Ee]hE&z-l6c-6R%quLOZo*Rh&l(qq9a=(>ecK4Os[TDd=)8e5xJ:cR0kSh@SP",
    "R)W!.pF/9Cg!f!}wzbc?Bms#H}y^H.xtn%X%4gT5CP{oLn0:Gzrpbl1?r.77vuJT+g2-RQGiKs0{9de.PkyvpkUI{biYcp?&8h%a",
    "aX0Y<.E=!ir&E@v3s^BbAE3$nQqYI:0czHf[sQ32c}7<kIN<-<k-=EtB@WJ}SGxk8oB8/R97aZgE+WWZcHL0Im8^A:Gf}#ty6kl+",
    "PYpWsl@k>SM@SVt).T1U4[x8M2&+c.f3ded>-ZSZx[xl%Fxip8@d8jsPK-vh]((-^.A1TeT#ABCePk.]LiEXlf*!NEAkeIrljXVi",
    "fw{[mpaODuaT}3p(s2^/M4rCUZ/P{qUr:0&FoVmIK=dI%*R]akA=F4>!!vyk8judw2WsvyC}.SIv#NAK042g?oXr=qsw]Ht+qCIF",
    "TfOfT0}laHhU6H8C%O-1Kw^TxUYAn%!wqnJde<Ald55pRaC!r9@lSkx=%6X!eloA%-ofU8pi3@X=@^cSS2AH=4U4S}r-?b>[hNTn",
    "eF/$KKum5qjg.x7i/wl%8po0V9jGSC[c7yb5wQru@5VNC9b+%z2-p2[u7Sxana.exI$mru9fHv!:rh-(e2(KYsxDiJes4}Ma>^Ny",
    ">wchzFb%vEg6ga.h5./Au!0jZ^/9ohRvk7vgl(uk&i&zzq>jZ*FqvU#psiAj]^9IIUl//[M}Qb^A@lUoa4Pkx7*RH4H3kULcl$fb",
    "wB!Rv6K0ub!nA%MgiUp-cAR[-kyo&L7pW}*jP9yo?iPj%?F5>2xL%e1wK-@qU(yN-{]pg7I0ikH4h(gVFp5s#o&Wa=fR44loqqQ6",
    "uzMF(N.xxEFR@$Xs0<6wO}HN!k=I#uoM4Zg>T8Nr*+s@9(Ar-x+hj[!p[&=UQ/q>c{wXZrAkR(yTu!ZcH(0Mzx-xM@=m@Utpj%fx",
    "}L6orrR3EgySy@/fbzz!DXxkE1JkP1rk8XMC>#nvMsN9X{9M.3Fu!yMA1//B<E]*xeM/J@*)nz.WY1KplMG(8&bp&{2pR:GfZB9I",
    "@m9>)9+eG&z0QP7v:x&]4yL)[oy9rQc}6]B^W7k6niXffoasOdoYFl0G[1$zGX^51>RK*8F]ozTDK#i25li^nZ&.0qUEk9cgnG-t",
    "t@^u$kS<MXE+c8lID5E@kocaQIt=u}zG#w#p/]!y)8&2ogsN2L5e&nCNJBM7NVxbo.yuhjTN^k(v3)uAC(b&JoR&@E4!*1r{{8D:",
    "{-[UeYC%nLTr*g%p-d-}<0wiG[[H7@dOy2W:j>NE?CmH>wyM)Tgf>:hJ$i:7sti{)x0U4up}zE}2V?$Uwmm%w9Tp9b}(a!N.k8E)",
    "W21Mv?&QmtV-&r2RpcnBkHQ4P&auc8W!c]su?m2RCQM[iq.frg-6-]P1(z$!1)HJIIg)9p3l=Z*GLjD3Fh7a*1A.gqp.NSUzM2t8",
    "Q+aKt8E(*Zb+3FK=]IE@c$yW*k/1UU*@siBCgeQ*+=]j5NZ2(b5iujxZuO50dHv-vPDbBAUWmz]%e99mpn/^e=}9Dm2nXO#&uQ3t",
    "PGZMjfiP^qxN<K9V>Zl!e2[rqI:-s.spx%FRN5O5PS@P5C47iX(aGFGGy)l$3.)}<YK.phqiIbC&[yB%f+1ZVBMG17BoT3PN#XpF",
    "NVgogzBEk.vMt)KSY@)n6G79GHb?H.=m5[g/BBW&0a$=kXd=Xw+!Zo(=)Z&^9k&)ZaJjC%hI#E/0nd(CYBsRH@]W3u2ZQUS7]l%<",
    "jCRR60h-=gZMNkjf{sgMq+k[h&?{}kRGv$+fGw%y9m?O.Z@7vUlX!=cdrvjcxE#(<%lzqJ9i(Xh60$9wfB%Qm*%B}wZh1cvJI)=f",
    "LF2tSZ@OF$s0Hc$V#upm*UT>LD(Sp5fm80UQ*JQO=E>PH>c@L@(?4!MqL4$]&:S]>",
);

                const ENCODED_SMALL_FC64_M4_U2: &str = concat!(
    "oU?vQ+(4pI71i=e!z96$YPnap9/*m-n!W%.BED:N.>Wt%zoj9q?ys]9khrOfaI%2G).b^pzQu##<I$Dq*uT:i>Y)DE)!fKvOExT7",
    ">TJ6AU(9#MXmdcPA/MA#aixs4UU4qDg}6.MO0KWHju/*UwVFql2]iSQY@Ma=rn-*2/JCHy^pw&Y?P[CIhC^2USD9:bK(qV&-j)]/",
    "O^.bL2<W)wp)(e.dakMq:s@)(5h{S61urRGP2MTJTA02Uuwst27#FAdKHw::dGVFg[s=ynFl9gxL!4jnOjThMOtNw</4o-hN^gW4",
    "^??t-HmUfNW<i&A6Xr=chgcPhL1B0kl2I#Ja>gOCW7!c8rb=ne6MASRJi^K/A]Pi]zudsjDY/wZ04h2m}Jvvi-W*k=($yM9P.x>E",
    "H/v9Xhe{atR@*Vo:=GaP2-4iF>6$H^N6>j4>:i:h1FFq#Qi?B7bo9X%YZcE#LUx/A3J>#z/pwZ1PH:-0=[?VljlT=0tfEe-Pt*(B",
    "e&Lk(XYbvEpud!Rg]V8u&/*Dbwgu+a/8APJSfdPMKFofC^33<Xg@Y8I0aUZ+!{Vmn-(A{0FuA+3rd3z-ZY^7/)72M#s%PE#t5pdJ",
    "]Aby.{n=d}Z.y9rO#PgXT(J8q/Z88vIv*yZzqD8y}&?r0va[<G2]zHjdWuQs[]H[CTqm!GkNlKnp::vExT3^nQfq(Rb?P7VF%%i.",
    "p[R)x7rJTv:Oz.%H]B(E.LrWlGfgc!SGdcz-5KhFx28bzEcIIh99::L[cN<.Tlu+254wb[OZyYQY}y7(Ru):Ce>?Cws0[Rz/oR:^",
    "yv/e3iG&Qs5bOHMm{(P=pNlgqE)WbDbXF8Wy=DWM68YITLRA&++YR=9Q8hp(>S4V^0pm?uN!s==FiNWf/Q7kMincFTaNmy/1xU7Q",
    "*]xCGFvGFV8A>z&Xrt(n[gy7>C+Ok4m3Z/=gxb--mvPl07fgKJ/H8vCt2?fbiWNzi8Q9:{vVwC>Y+SN<Sl{yk/-eDz=$(im>-8HM",
    ").]8/Ejz$Akm8G@K@BIp+<V1NU.(HB=>n%-uEwkPJ[aH@Ux(r][0=)iL]IXBTI-h]kY/SiOg%K$W9E:CA?rH#A#}IFi$>ZWD-9z%",
    "]ufx.lKY%c8Rxo$IAT8n:U(gy=R$]Go*Kd<C==cMVr9ybdE^i$e3x{SGTb^my*x1-J^QVo!#(A]v8dQY+.*j1fZ.j&BKf$Ip:YFW",
    "d4uWsKD*f=y-fMxm1az+Ujo?3[yFe.FyB$twct@D80>zsf.YR3NMcV$CZGRcpiv2Wvt0#i>yP>J}#CUuefbV(O[VxY=<en!RiV6-",
    "PY.lD:IcSCxQITVy#@guu!6qjeSM:xUnJ/(c4-fWKow>45t0sykZlZ2k-cdot*NaB]h97ZsjRPe2d7bsh2U--Ao6wHkX^z-+:<RO",
    "@).T}[7sl}Je2bh5/m94Z(kze(b=^==Q)@Cvtt=mzVZ9QH1R$VIA[Kizi%U6POyDH!!i5pG0fC.H]I=mi3kN>>T>=!LXpH?Z?4(9",
    "&p96@A(ukoR#-OP!u$G6/FZ:.(kPdOy/077pq.$5(>{l}PN&p?f42D/{[{P{0p?:8((d%#LQcq)jB+q.YDT#3-RlZLcIIKz/>O:A",
    "KQ8Sm/IV?8@CQi0rJ#$D0lC)n5iTBmhH.C+}#L9RuWCp977]lk5oA<{:X[!A(opGD(<JhzW51:9s@EdHQ-I>oDkGvt.jk[^YeDoA",
    "3)tHijrN&>:Mp#odap)NDa0Sl.cC%jMq(XNe2yfD7-UR24j^J61wK-^2QDsY2-%<9pzr/yp1FglL?S*.rn<Jl^G/@?4rIXn:t6XT",
    "&o{3Dk!80IYWzbtjHqjOZ*0ICYA[u%<t>-6t0IU)YP>=Ct7h]6hOs{Xp@PoAwC#bky]N}cg(Mj-^c5zh/S}-]U!td>gtIdT&@aV@",
    "bsWZbhdN#C1CB3yqlZCwbzhe).Ib9MB{tfkP7!R=r0=Kq8Jg-h1tA9OBxp1RwV0yef>]^ZKU@uoF1*}@[jq#XPN7s=YR-3GyZE+/",
    "UVeSQ*!sGR1mm.Ob!Y2Eu{}){FoFrhRL**+YA^&q}U2$kb7vhrXZ!..(nuanKGKhGOH.<@CHVJ5>!06xQ$>:U/^%UDjhd:UJ$zo6",
    "7dauIV/Sw[LvI%5k+Q@=xSi1n3y7oF^8rpO[nfjeAj%Abrh!58C*W0->!51J7%-WP{iCyQ{l7U6jw[>^&cX->49bcRYTm@b=nA3[",
    "<to>1pdH<rKz)cv3UnwjT&xef7i/ph1qB8lywLt32Qqpxgr#5@@AIR3E.S}pPJB?1Y2jAU=s#x6sx3/:.j}(i7IeaXmJIZ%wfa(q",
    "Qd%c2Dy<JqXui}pA8b#q!]ebYrRS2{)0$3S:mA4/J6HgLaLZu/)d.BhjK-<:Hg-wa!VC%/D7^>{?Ssz[i^(Z@3CzLl^BBO$b^AU0",
    "r4C7l<$/cl2Q^s09+]A6At.qh(3v>wi}H>mwvgbP!w>PMkZsxZv1vyM5y2172SSWl1qyam6DzS^u#8EFH#TjM/UNl]aC2/<&8HKQ",
    "GQ}i/*>rwOwdJ4X@66}&EdjHa/Scykl$}$2EH&5lK<(/-VBSSwBcj*i8wlEy1(p)}+u^GFdbO2/e]>X?&hU[Lp<%PK!hRAXr9k)d",
    "mtk>Frb8PJ@Zd*xG1UE7H{We{hM%!S:ogfWi3bE&}z6Y/TRg=HwY34jc2yVo9@13{@g$gx<o.J[S:*N)8gvGc(H+JGJ(EB0^jXcR",
    "X]%&i?tB#Iuc.ruLc51N]7CWtRF$q6@oDNhTjqWk+L5Ojcq]:=1=n}^82@v6}xke*5l)w(X?8:Lat4(Ll%0+Vnig.6pf(phJ36W+",
    "{@b#>Boa}CiT?opKq2U%v5Igh5CN/&pY^<-fZRj<GH<ALQ*RfPC7c}:w}J&x@]-#r3.XL6-NOrSvZsJGgg7E5h0d$7KyYR0trvf{",
    "iaFKy!h.#>Z9oeZOcjFTWtIWY1y]iWLte^8kr61:*+42s0?1>T&e-M6-05B&EGT^QU>{^IUy%T4C[O-v]D3)[k:oNbd5z*lJcnsp",
    "}ltMnhBFL0Ia$VDJ*yUEtw*QSOkh)yeIeo.<>$XLOVgEd0bsBabC?RcydzssDm*6#]rBMzW#/yUp?Pmg*%-)G=[DlFwvq[fN^3SY",
    "RwNB*04YcM3s-]@fTe10YZfdww^Eh9Fy}#R/Z<{j@mtstc$E$zA6de-{+!wI^1@jA-)9kJXuhI*^[7xtbJaLAn0ZD{Ss.&?J/0)5",
    ")kRgfo7T*JicJIdym&d1i0zjN5Dmu@r4ZwFCXrHeDnAvHs}quiX:BWOGDfJ}8P.88{}94U!kx48BrZW%s*SVuFm1[V&V+.v0[b(o",
    "S3[TUNW/eDatj8n!kI2-m}6rG7kv/jB$iNc?jqkx@Z2g1ERtVyinxw7ar2^z9fl^Rm^ir8u7&jJL3tDgQFx2%M2!1UkH2PV[sg-o",
    "7@eo$Z^hIz/PIJkUNfzN41-lcXjGyTil%9r-2>>&k=H.CaPzkI>Zg09>-#Su.PN^0FgnWO00?KC5tGv<&98Qi(sd@IvWUOapFl{n",
    "GKy3c6Ap?u[8xr&{3/7EGzJc((5:@sfUvF3ah9otBoL7C6P52pFyKOgAluIuyay+$PaPHk[*od6Z8f6%>UO+3aqrY%@6cEW.FN]r",
    "=B6aOeOL?W37wv!KqBMM0aRAWu[N)g9m^knFq}94).6*RH*m^eS1VLHa]fbw4K@16F3#*H-{:FLRwkialx=*YH7/rO54<Qf0=k]<",
    ":AFmXo1H]:r3TPuEa&eB9Vh/dtXn(b5z5:36F%z+5C6?Nn*f-*wxuw=UVi-$k/ems7/ipSB{s}V9i.Yw7=4&0PL?DRgpv3y.Eeo+",
    "![*Nl0e#aZ&6vsO*&$2b^kp%rzH1Q.S.{e*q:Da4}#yv)k[3-b2GWs!talbrkS1baGNXJ=oD9@Ae]ZM#>B[-+pGJYxuBTyQ6QJpx",
    "SURW05q^CJ:JdTIYDM-bek--o<G0X>VaofZQb-@KAd6dEF{rOAypqWaZ=b7L6e#Bz]^o=7Cs0Vus[@T[2cA}H]y?06G9]aK5@hoa",
    "0p*MiO+s{7:)t$sd47GbcCLZcP)8FVaj!JDy^U3ZhShXu>!^cTQ#!)BAc6jnH]s1XniA-Y-3<7$1ID/UJ!r@!:wwi=(}@x#W4/(H",
    "mGIqTNb#=e>.FliOqxIAnm0u2Fm1K6GYs(4VleD$Aq[DME7#4j&j@Hg]tdxs@XM-3?a6@DPa)K5H9WA]xXRShv1Eb%+<Bl1sM]-E",
    "W^L1}k9Q*^phtwzKEo0gmQ>+#SdJO.9T8d01KIOg+WY3-h?tZb+5oj4!V+J9ndc6zKK]7Dk.TS#QaIi+[nIC(tYxeB>z=5%08]Va",
    "pZ)ArOYg/CtMD@JfliZGwFMh#raJ%hD/5byi2n$^(C>XAW6j5O1eKtUn$2xog]vB?wAIMdi8uA+Bz5&Y{vUHID(aX6PTSPqB}-Q[",
    "0re4&D@H({D$X}Yk:)V}oWvlj)Zq)OENq@9ii-s[fZ[c^u[P2M>9y$tLBQO{^ivO.U}aA&3haD+@o$E^WaIQ@-}qQgBqBCWT{4iQ",
    "BL(V51r{YaFq.*/v9Y[cAi&t7n=1Af:XPPkaTWu9Z@^Re!BX10DD}yh:CKMx(>bvyNV2cFkZ8xPxww[rfZxp-W=t7+5!z-u0a$Vi",
    "eglDvSJjKYMKKS(@Wf/(I+Ryd*G*ac%c0(9gvy^FXLBqhtG+RY7KCEh&c5.C.%PPI..dAw(ny06F(47.)6[KUSm2JCH(eV7Fhjsv",
    "S2hG%lkxhSii5dO[wAHk)ae6$fOp{&(s]N$A(1qf[b&}V!iZu4PWo@aPSAQP0fk]0LbV2Q[hRBI4y-x4/IVcwD?=Zor]Z:8xKvNH",
    "X$aF!d0-hjrP$mgnV*:W.N^6p<zKczSopO4CfG}G}eCJfcYc!B=pw^ojusxA*:wvI-8CnJ0k^r#UY+c3xY<4(=?YaOWi5mdJd:R.",
    "Ovu+R5*8@[A9{7p*Zs+d{ss:SEhtQw/Dz4{7EhlrlnGosxo<b(FlOEaO)nM-XTymDVFx4Pbx8)gFB8nVc!4$a6CH>!?F0Y*0^iXt",
    "Y4@gQi0c0vfI7$407jB.1Hh-9aoG*3X^^EUO]T=)5i2JXYWsWhFjf1v+cE:81s#&4*pCXBva0B^0q*xum=Q45Aq25IU!#NnZ+?B)",
    "U+{x)0@hG5@F@P@Lf-saUMNXk)$?7VZng$Dg:6PEw{%3UnzT[v-c&)]L?ky/r/WUK{0v/]w<bp?DSh-j>IM!lOl6KuW*b>@G%Ahx",
    "tmBGA.6SK4IiZ$4tOdPj@k7Ofh4G^M>kXjsJ6{2$qZwY.!Wp%YGNx{^52=l!4UxqDKG*o06&ZeR0-r@3)/5x-I]DKD7>d4Hu7&<:",
    "LQNxZn0$48ws5p*9+i5JCcm7yFob5ge]V]=[)HloJ?&^vDzDxYNHI>MV*[mlfP>[sJTY+/d0Ymmu-d[TqXklT98HVtoI$yAXtaeV",
    ":Bg}sDyBGt{.]zXDiu1qCd*O-1lWqfHy>aYEO7Fw%d$&vtV^BTXdw*cCs&T+4&([cuP.yN^+CQ4rXF*lWo0apC03T9)qi(Tdc.Zm",
    ":OOB?}H0j}LV[.-pAk?o1vNZM{PPp:1OE??Fi%KPMB/C@k&mCXo>pW}FyM[GqjY:hI(k%P20$pkMT^Y<jaFS>lJ5%UBXD?&W81#v",
    ":voS56l]$V-[SnZqp}>ZJoT[pm9}vnUA)$RtOe>U1q{w=TsX80//QZ#&Pk$Yu3dnN<gG}b6wuT7L}]RS6ceI=>V=g(*M+-40OSh<",
    "Vw/13Q(((:3m[JT9d-A15}UUEWp.9QA&(u95&ugy/T6^8E3aa=^#u-e=+R!a2zM9&^U%}:^n}:g^IAVZrGHp@l)rqTyKqnTdG)q@",
    "dMwuXauSDmyuTo.6KV*iYZ&f*I/*Qny]ijR+L3U7UWORpkDbsgHa%i3P.UGHoIU7s}%GuPGk%KE=&2jQ>?1AHSaX1Kl1rB0R{8S-",
    "Kjn!CVNDr%RXObKOgptG&*rxLw*vO{0[NDpTtimexDP4ztj@Sy8Q>#P+RT-j+Wln(VWHT>ElqkSR1s%T6e@oxC!/n4yQT^q!aO?=",
    "<=qHq@mq9og&<hv*LW/2:nKEI!^UhlYa=c%x+%%!aXOorQj1:XUJUsp+VqM[yT]%Vs-0Ap5/4=p}<LO&O(bB02BM{M=iN)[^7s>y",
    "rn&ksWDBE3-2n[BC)Gx[^]I!#0q}!?44dplvUgn+2Q@tepx+gvud*+).4?Hmx]jD8a<{}oucaVZ?mISeg^lG8G)AbH?cGqeL5?Qb",
    "J]KOj>ReWzRDn[]?iHmsPh=gZlP*q>I3<W8RM?wWBFf?>[ve(&pY=3TxRRuoeA.=3PZ{*I0m)8>.0q$5fO&fhYJAJzqIla928r=7",
    "su&VCY-5+ZfZvYiA4m%50JmChh(-{(2>%+2*@#w6}8ndlfB#w<gFq2!syZKdyz2)Ctn1V+e?YVdxE/Q!l[?t0}ZGfw4pT]MGf)po",
    "X/q)cY]/cNF5}b)8gV&[pzs!.I#Wn1E}-DcpjX%0.9v@.TAtF%BR&Ds]@<usx!75+?tKd.^Spvh6YG?wJi}0CqW[WGJ[J6#Vl6T>",
    "B-OLpZnp+vMTDG2Jr=BD.a#KAD501}!:Wxq7xCWzYfNS8LS>8uhcH:d0ADx*uCJi$-uB22CM8kstCAA8HXwG@TT@i<TBoDGcOp]N",
    "lKUq9f*ZZXjV!tgykGU*J<MzZkVuD13w5%Q0Wh>NG/87UiJn%<Vsr]0/D7bUZ%^tq(8fYTly7J-:1Cm[WRi-CC^dMr(F(J>EXv@I",
    "sQQe3&oi2iNvjjv@99Btn^iRQJB3A+}D=iD@?PJQK]23&ed/VZyoOL?*->0bsJ/CY=4g44Tt%}cii]kw@@ncBCWQcR.[iJAyi2I4",
    "tGlrF:l)F4^9BZnbo.a<V.+8)Cp8GpN/xg#-BANB}?NIL>INgvQb{/qwM6HhuIYZDDjZDErr}2pjVv]8Eyo8rrB2QsTZ9Y@QpQ^%",
    "}rAWF9T6nwRdnF$Fg&N?5s9O=vC3aAfgaXZfrYMj=Tu64aI7mXk-86{!feY)0Ai2.T<)$D]2Sk)86CebD{QT[oIg77QWK15M:8fa",
    "R4SbU<ewUnR%Z>8IF<p}jHMv@Ks0T+!*cns+ny)Q=@X5@uts4n]Rk{gPWhc3*m7QD(ze!>!!]2.Z.@lDMn0D}Kwo-dwhO$L-&G$u",
    "i8a}p{)zG6]1VM=@f0}<Wh6m^P/90p{af=stk6j69ce^bfJzw>m0]3IjO>$?@X68vuhFYRt^vwAXUZoffk}jY>^%[We/n/CU6@=4",
    "BQX.)S5Yh*/H#Q=kO[TUx?FSzFOQDkAPs.}U%e60li>v<>[j)#12x.^(&$zxmSK^cU<*77^Mie%LExR!U7-f9RA92p:$Kn*1g.QH",
    "EvyIRJb]y>)2=n3SXuGF2[kI3cN2oHEDg#8g5en)cLPm*5ipE/u<JU]:kKknUcZ%qELIZ6IG/fot1LE2ax%36k=!/^MiOnv@oX)C",
    "UtE5sRUL>UKSuUIiqCUI=^k]WPEWMoJjo@[S7AO)5)hJPz)u/nZ:+Zli7FcQRz-V.AbH9q:!Z)V5A.4%(9=vhR<si].$)a)N>O8h",
    "UJ5XFoW+{:ow099i(f)<j2]ZymaW!&V2.rS2n3*C{&/g&hZHBE3F@fO^^D@{f9[glK0u6^QB:N46Ri&t>71^x{{9x5//u=fq2frr",
    "m^4PIhl@gO<-][1/O{e1U7jtK9pYEY62+IDG.$)nMU1sN{%h#8[pEM4T@j1GSSand:E^*)q6X?0P}hR9bK46HFbuaR&rp7j<tzPN",
    "jeb((8C)p{gO8}d{*4#9je9d#.}1B)IK*h11ztFks+96Rk^hN$+JaExu/<hU@GeG>:MM8Rrq3saVF=FhQDjz#EqbN(GgrKr7E1/Z",
    "Xc@H85fNc11S^@:NmM^gIWK.(r5}z2=XK+G?q&{@VhG.E9(g]wXEMqo*53aWE5ijy=)S=}4!qi=.QKJdy)IPAx)I8thL%J*<GeOE",
    ":WJi[<#6xRGsZLokD/#eZYsRLp:nGAMk/}t0mTR5Fed1Py#bSn?2hK.rqfCcj%&6HWRwBGsEB9wBD+7+xJx7o:M#L-Or@lGRQt=m",
    "mwq3vwYet$k5R7Fg!-x.s59.G0K%q!6Fec8D4R0!kUW+gg6zEddP{dyOw&65Pr!)sSmL&G73n1H>15xlaxc6s(D}:U3S4FIl(nGV",
    "jFNbmf&:aAfU:2s(1?cIhw=GWL(/z#BGeh4!7ID0JB5>L@Zx*JVJxd0b]c+I=$OSQ<pxZ4t/)Kul#rV5D91}E(2BhBXrhCc@uds*",
    ">y%d8<+.hBrh%xv[oF81nL%SAI}?&bKts78Gcyc9)@JAXr}Z/4Sx{%a&L7R(*?VZF{r32(yj63}kaRqVBpASktHrX<d8xpNxeh$w",
    "Gbn?$@ZvhUBe3{[?+(Q3j^/3TWg?)&1i/SmkE.T}g[A<zGVzGNKOe&w7#btfm:A<7wiJNGaxZ59)4S1yk=u0w7b]*ruA[Q^4$>+k",
    "/9u7P{^DR{qn&q[[9Tpwx{wh4VgdL4:yvp}V/WE=V8!c]k^DCm&n7$b*HyHNg/3oG9}rG0I<1-p/-o<wDvQ=lz[e&Yezi>l.zWdZ",
    "e^tQ}gG}%+?J8V<v!IQ(AjTT:TFWwUSb@tfr17$8-ve[)0wL9FI:iSxs0gbHI<xR>Y>$>N9D+C$Fnlju}ptWh]-4}9dK/9@MoMF8",
    "PVM(u5yloE<e-LJKp8Z=?mc0skK0ThvqyRi/W8m+6KBf>PR[?OfX<Vi^m{]gJMr6Mzi=zgwvJh+55xFPW?(r-A!P&yxw)slJ@dzw",
    ">9J+BuW1g/n]g4Z9<GGAv)%cQvs?y<Q8%JKkUiN(N1xitB&$1NZ#0.S4.Ve.Jrvo^q<Lr+LX^fV?mi)c(atuQh1@mVnh2fI(I@4<",
    "Rk)SXa*:tx73PJ#+-IGXzHgoPSADX]Sk+EylD*Sr7(Xm7FCljVo6uC)N=+^c>k6i:CJuL5@3883e:$!+*zkaH:]3g[1bWjBc0D<5",
    "m6b*?dr6NBOpQk]g<Yr!S:Y$Zu%9-EC$63)bNLeFWAEEvr8o9#-dn38Un)K8cgyxq[NVaxRQ?Xeqh1bBx!<Uw86*Py:rD{kb3>[@",
    "UYWxq*Ox6S{F9rOQ8cLro3Tcz/q<$B.c^{t(BeiajrAMDAJhdn92bIqIZ:B}K&fV4ydnT@0bQjHN+%4qfIp1ZDkBreeg0Y#h4G7s",
    "mfuA2f[@v=1FJlqoWos{KDMXe+HDzK)}npSX.()t^}LpmY5BL&QlEP2r}yJ198m3V3JV)lZ$q8&Fo.7c9@X.{tvC97K5{A^eSEe4",
    "i:-/s=ff%DHCL:u4CCEe/!}{9xSPj!sfnTtRUBdOGBJb*[F!k!p7-D07u7fn]6zb{KszIrjK+7VQ$?@.EJ{eMbU4*Ef1Om2ID3B^",
    "J^^UnD3I#}jD1N22poR/s?D5[bfDYB)BYd!XBu1LX8CGy!PI+x%mc&quiiOWRo+R{",
);

                const ENCODED_SMALL_FC64_M4_U3: &str = concat!(
    "=?H.SmDJ@0)hYN$uSpk(^5ZP/PKE^Y!(G-5C2c=7{>a}%cWvkQOM0Fu07}HHA4VV6Tvd-PCN:U+LYzL)vcGWlcOje/dKMWTmh{((",
    "zJ%$g9@>%$kYh&IQ={p(L0N?=6I?3eqSj#BteeJHRj[Hm:Ui(ZkqWF}mr(+.t)%Q]0k>FycQ[.j3T@[B=Nopp=&-q>)s=T}0lY#c",
    "Fx*f/04n>#46.B#rcTwnZ?gXIH>!meFuP!KnxG<NPYF:dFu2cJJi:?z*uNn5Z:0[Z:pUz+b*-<g]NI>kp<A??Z@29A@8iKc&SLW-",
    "hLUSkiaKUQO[R}7ZE-obcFfUdO(^5DwCr)KFubPWoXs}WPR]Ss@09knZp01m)^.O^yEh&2sc-Ly{YAp=:AL6Qp(V.Zk!WdU^8!)x",
    "Fz$95G.y2h8f&y0/Wd:N()W}cA0IDp&iR:.PI65-BC}?o>2WXV?iA<X6QHslX43p?+4Q=q{=v]Sg#NW.gnGN^mq-(7tVHqRFNld9",
    "0X1L<^J]!}25K/j1y:RV[bXrTgZbG{So^6*vbkzbXk$b){^%CT.0#VY0bU}6t&[2VBM(fa&nI$]5jM%*t=pGjbS&(RbaC(3F({{z",
    "Z=K&}sA@c7Fc4C/6O7r?H+o(n66GAY82?Q?!epedXhqz>M<e5J>gdvh[0BCA[fZ*<ngrxe0VPGHE7BTqvzm::Nx}4H6F9}fHD7pa",
    "sEi43FA!Y0IJK&2rl=C@c8&f33m?A78O8t0@}hL*>PrKju(+g*kU}%6>Z&9k}RlV=D/UZK.F]PvPQy6/g$jYMX^J%HTC3gdra:}0",
    "t^uA6u[cPrAMP/BbGv??Hu$?LP=?#IUNX1MjtbhFLPH5(Xzd}mFsLr8oDe^Jexc8KKzFvrA3v83idPBukQ?=cC^0e>u<5LN6RI[F",
    ">TPtI^c3-Ty}s^M=%IR(v71YICX)@7Cko@S0KPONI0*i:1-e!7YI]>l6Gbwz69V3i(b[ikA2$02@4AHZfBaL&(mv@U<MmmwN84-B",
    "fs0k{bovT+XSeKhed)>8@4@&nGV(5i{d9Q}000@%T(N}[HFMF.Sd5%6xdcLs=[H0-t?2yVe>L+10S.mzQL@G>MD2ySj^9Z@*M8X9",
    "U0U4Wjj&JazjT3Po5DLTK8:[RRXUi7Dd-LrQv#{DdKG6%QV)7*B$%@H2/-sCFrjGS0lTSv&fJp:xJs1N&qM&>OAK0p5Ce=hr8d)9",
    "u>u}.H}Sh*T{7jRL&2d$+TmKRe[6EEkNYUmFrO!Ip7jhp[o:E:1Dk90=eHB>A/2*<RM6)o1yH4IL#GPQ{)Lc{)ZFH[K@IxM^)raU",
    "V]IEaQ[9yR[[NWc>S}evgBDP[HqnZeQxq5&2$7NLtNB9pkRQkw{[}/s.0}$0*Lhcc:?+OJWU<mVuoa=wqwX62u{?(Ct>CA!soM^V",
    ".x-9]c$LL9Tk9V!o#r{zD=dtD792v#yspFyDvA}0c2%^mzsY+KSyEJkHCWgr?Gu6pMPu.ieUAV?o1<S9cEHFVKFs3.42C.jaAu!X",
    "ax/)IFqWgne[{lS5?Hb$cBruTKF<}F>&6-FKGA/vo4oryS5<c9u[HMN-W3}+u#pPXPXIMuk!]s>RCS4g.@?:%ASrsbtJge%e8k@}",
    "7l]?5?[t5-c0mliie#=Tg]MwD[B8jEr6YuOkX68>n^?&o[2jmt/=sXrN:*dQ/T561dI?lY&>ThfXBY:lC8GiX*@!-<uQ=cJt}jX]",
    "a{3ody**!^2/=E00k1o6OQt[1y05C1M3!L8dr)%sI^hgOs7kP1r1K^Fgt>Qkv<AIpNAbE]=T3Gjs4{MD)0.C*WY}^8oPy!<920--",
    "Sih2>2UB5t<HF)oGGf/h[j%LctZM1Oy{OBJ7/n+}rk$(Iw(t$G{5:u?Fu:[[k+*fFt:l%9=(TdsZ}0%*0kGF+r4NJ4XoL9/>3W?{",
    ")KRc%DliXZG}s7lek(%x&VBt%!*c*1h)5pScRkt(S7&Qwr0C1?H/qa545*CAqDADNx6Q^c5C&7:D#ZI%h1jm6cqfrhqT-E9d5e]r",
    "Ij7MVgAuiMRg=foefCu:TDo3!vc2c!c(+@AvW&Q}h46q}XB{p]loa<8Iz1$y.[W6K4bq&pC=[Cp@rjSl6Blw9kF^)]RXxMN=bId}",
    "GAnXFWF^ktLMbdav9aD(^RFo:<sk1fXilJFrgzYb^1q/<07fst3{$K*9kIqisLMCgy:6h00aA*TWZB<Maxi)826=-MPXEn]2Uo&^",
    "aE-.JTtFP[u%7{<Cz0Px>Whp>G#fm(!G(}lC4ITTwY#NkG*ryo-ShWAWig*EPPqpIddaHSk?}^tY3^N>Q#*S1kADn!0xr=^)9y9[",
    "oMa=hXvf5-m8VI%k7QEiQed8/83.0w06I$#Tes<*OsVW)fXsN.&3=G9-2P<Y?=}A*S^?/FNH{aC}x9BnsTbK.{GG2U(UQ4zdWnz}",
    "B8XAGJMP{qt%yCmI#IbrXsb-EVbk](6(!7OBLd.GPQ?69.Kx^N7YW5?0qPE6GHnqzXC.1w&2-=Gp)9Q%EVh)(-{8>N:wgzKxW)Zd",
    "eirTvC.vlN><[$>=D0o8D@(j?UZYfQ>R3X:Be*j]DNlLwD(Kp[v<?&2wYP9Yr#&bwxJRWQLCr%%A>x]ou%Q]q04{e)WS*^}NO*gI",
    "el^ecJ+Fy8c)bJNCn.abPE!c%GH:t3CTmfGCOG3$mc9w<F}EvxT.]8>qi}Q6:p9Ef:On]KOsZ$rxJZ?rq]4SXqRFa:Y$]y^F?w@B",
    "QB#*nRkPRxa@Q9/H2&!d4}?E%aiY:c-w1!=nN4-Go.lPz(uOHPTPm1!7:MKLc<]Sgd5s{D:wxudGZkT3Al2:REO+}=/5idk5tNN:",
    "O)c$kL-)sPFqK9nM$O!0b?U/6KyEs:VCIFZL:>:!{Z<HtCM[8WwrgVxel7H&F0X}N6sit7{!zX<6o[tq1TKk-GI6U8eR+W^Cd<V-",
    "PB-@[c{y8kXqnV#il{r)2cM3g2oCxV{E2@:vlH$YBybu/il*ty2R<Pp3[B6C9]+eJE3805B3*WaunifTe]{D{efYj1im(UxPCM9v",
    "n1Ol]&h*GMIv42gKC[1?xzunP>TRyfT85}ys/l$/X4q%XfjY-0Yyvmi/)2+Bd64.U[py%f)WvSHDqpt2VEn0JukPa]]Rw@VM<<.x",
    "fU4m#+7Ew<xZ4B*6X2icH^j5n!c)Ro=tQuU-b9%![d^=xN.%$B/M5%+YNT]-M]i>=ASB[d1JVD-!*9U$9s-W}:U@PKV/+).AL2#2",
    "}Iy1>:)ADG[X+TB:$g3ni8ZH7[s.$Ty})Y$]FlcN:g/nJ/E&BHbh{LPnTHHi>dwO}fUI&%7*#h.5CMb=G>o^pW9@^4F2LZ%lJ8Q2",
    "Qv#%Ee}fe2Tryw5C.N!ko3vx3g^MM7N@pUSV@^t)hpas%K]5iy7G@Jb7lK!y9Arh9@O)E.qiyMnlU&Tlx.a*ewpQvqvu.Lv6VvI{",
    "k-$B%cp31)n/(A]ShB1oZ@h^9A6:E=FcqQVf15OWx6[i-FnLt1EX10Zt9u$uAb3Db/Yxd@A1]aXJkg6eH:h:UmV(W[w@15.1(RZg",
    "73]D{k1M5A6kRN2(jQP>F8i4CaMN1BJ32*FfgW.fvOrdMT%7%3pmgQ=^57x!p@[?-z5L*RGCGiL[JzZ)a@u-ZtfTJtXw%)$k.7G1",
    "ue8[Kosibg/%!QS6M<*F.uY)1A)&&dYO27OYXz{sTQCJIE?ATk<rnIb!Lm}+cC(4Zxq(oL8$V>sTyh&c/cdB.B([a^m5?#Q3Vpd4",
    "+-UdMOlxTss]WZ1[mo%g4de$^DA)Gksu8XYAc{a$Cd33!Z{0&5a{S$r{{*C?}u-F3E!*Do*ZYAe0i$p&4O.:x0oet^EcHq!w*(.6",
    "PD!o={([{{20*3*?Wy[%r*!R8++1po^[p1ZOa>ZL&N}8vqL]U>e4W5&1u]h!VmR}pkc7?H&mpPmm/7ovVtC/s4MK?Vi!P:Z{%e/J",
    ">&alUb<w&5SbkD@eg?*OG>D=nxY8&Ltu2e%->?I:*tZe^oW=l&!w=2ZLW!n/O:juV[hQ26.*hE1>-^KgLwqKfKM(y20*AWHdraN=",
    "?@UJ+RzozX1O%c5fsOLR>!qTj7Zb]kQivUxQc<(5^9*pN2(B0Kg)5S*t5-08PL=<8PG=h+r]A+^vdon>u]]R@1z6nIa%T+lIw+fZ",
    "fZVRqycv-[uyw#1c]Xcs5ec3D*ZPsT-S(yPr-!!6=508$dN%$iVY<Aiaa>#R+#1s3(RZOyFrcA/Qg1!Ip?rM*h20MOL7gUr-+JyO",
    "^55#<h^J^Nd3qXo7F+BMN33an^enhM>.j{T08I{[*7P(pNhlZNs6kNrjkeA3=<YCcgrLtmijVf7&fnjz%cp:uzeSK&YPx#[-E>GW",
    "m[K=n5%7c%(c=cm/ZcdiA&76AlIUMli6JM&{)iUF&LaOuTrKe#sNuTl0bfbb/NeB!2+pd)Olq/73#->[XxLHD{!rUf@9=Y<&U7v^",
    "A(X2tJqmSWf3I}Xq0YQzPJnRoq2Ec5aH!Bm0nIiHXp[+jjHDv@C74@P2sq$myy.8)j&d*@}c%Q[@Y{isude+=HeyPbH-:atNaq2C",
    "kk&7aKt1I:09lellCRLyM*R2&[sL[c*JlpQ]sh1!UZ^+br3}^Yip+iPWPYfl]*u70Cg&X7K8f/??{m[c6LwNLfW2$GXjC:A@p$6u",
    "Qv#}@Id)[(0!b^nnD%sJm-*fzG$ciS35$@FU9!E!+W!)W!)2suGHh=NqmJNrI(Qm0-=QNX[>Jh#RU!DFp{?1JhC67B1EH:NEe*Yt",
    "<EgxTFc/5+6SWqd^T3IU>N+=$Fj:Stpc354F}o(l&2B7a6F$}n}G7D9DAuOGP.IN3o1<5]XjibNKw(p}1cX{iX>Gs>)vwL=>EuQp",
    "U$>M/{TREtnZeY$W2?PbcAjgiFm(XWW9Qnn!w(t/lX]c:GMA}MAVHxd-+tm.0hK?K9}1+CkTN7=Fh+=+:*TY415JyqghQX/joR$6",
    "NJn0kPXFV)5{pa=C4)?!AhiR2<aAPgawh/2P[JJWAlQ)pP@a8qn?{BYRKVH9KTy@e&Cz4FT0?w?pmjjd^.bK(L^q@9c2%qQecT#}",
    "k-e@hJWqGOfH>T@zIMp@ayag[sqi%ji98@<t:c3/XA-GAie:/H@)*6)aH!>]--]%^!)m=dso<prGYU?sUQzB)BJV}.g/ptqoF1n$",
    "Aj3&M=*m:DOOf-x:?28Gf!C8[D{N&%QU+2[hkJ)UNwV-wremA:cnN6:j$mgdl:Bl+H7PcNRG!m/xLG)#djZ4::^x=O6qSjM8l53E",
    "8Gqlh5YiJ<)bFC/5acQm:I{S.V:?wC1?1E@(kF%3weM]h8{LMIG{eY6^RtHuufO8$o3&LX=v1/*VVl{ERUx#48C-%?-fd{I>N2!N",
    "M4/*:?%um:J/k[YNnI)9S3xIHO4i#h8g6a.w:Q{fu{^V5UjR[hN0-vb569l:VLLyiROkeRr0MbZ=:/TY6bnnDZ=c:bP*[VqQ$S4p",
    "O5rQ^x%8Y[9?=?5fDD#Kb^=ptr*F?hDKomaMyPhkAynnts@&qWA.eqQW5F.)51i*teKdK[9VALxvMLe=hLD0(}9x[LNPE0.vl&tQ",
    "hQwbigCk){H8If#l]4rES+!6pwvBJZnASNUV>n?xA+4r%[e{7w.3ne#x=6jKVUN#LUOZRdRi5g1]eJhM7xP&nK@7#0qI(!+Q:0ui",
    "G?^*}>d5juzr5pxeAlilc.2s]DzISCZB6WV0Lr[D4K8GGe>q)6OH/(oGT!SS2gy]ks>ghGYN0t!SOJ}9UGaO{:VRyx2@?a.Eez5H",
    "e0@YrWOfobP$T7i&^<(?1sm(#*KT%<mS^MK:uHLa6}u$(]Z{G[RMy0nYD7VHx0kh#s&N5C]r9hHBpwk:h-lBPN}WC/b{+4P*({OT",
    "G^r3GkaN9+7klIqChro:/0+$14Ff}uXoTdG2]EY@s[0rz]#z:1.+lYqKGYV/!VEGsttsE%Ci-!Pfb>wm{.#N:lUQv2/kDwS8<@(L",
    "gkDWU0&OJ3fK$v2Hnx!&MBBclbVcXrbCD>OFCs).PE[wJj9Lf#cR4{^9?PQH]]JDx[O1c2tCFq-er%3}IR7&>?#!RCPZt&z^XzBU",
    "D0r8tCPqA2Mnk$-eT9=WAy[kl?NPL$0F^R!N-RV}X29&MKH*-w)M#7XZ.b(}Gs=p&3$LwZFS1na/!v!qsh{PvoP@O(iVtio@mmsJ",
    "@/1+dbl?pmY[/SKb#AL:r%4?NV*)O0+r?WZ634*L6VDDN9HV{<HC!M-[17aPm3I]dZ%#gz5p])beB2L=SAK*pHxq9Lr]YjOez1C%",
    ":3Fs<qNfB?SuK{Fmr7J9<qv[XVFC@5q3:=%RS}5xz9DmVvra!2W*oQWtNG2{0J}u{AUB8tk#[^^ME?Rsr?MrNes?a3XWCT8w*72n",
    "I)os%T49xdoGm+&XTWj{4sLAyeH+}TtYQ#Jy7ZUkqmgCU8O^xbKQw3H?&B+Ia=IByY:{7oF3Yi?Sied{{x]/>*31NjXk}d[HbHIB",
    "fvdOJC5Pr4eq[KksDe/-]+]FF@EO&Hj8VEWVshlDcc<xw9kBCCi5XiefE&H68D%Kjk>[0^m7ZELWy6>=a4xo{o.#*]<u4MGswKGi",
    "9mVmEz8Xi6L>@uYnglqT:(5%UASrZnYu>{?iTraagZGtWFjC.=@m/=n/B0>RiqWORs-wZJLhOF^o1GU?E<VKa:1HJddzVFaHQDOj",
    "..i@k<0yV*y=2J-Wfu}3D1+F^03qIhCiGayeAg9MEfcoeru)Bnvc-AU/oyf!qz/@rmfwZ!i0pJ>=%hTBUP&l)5GmJf)<]*1-?2(q",
    "[(m-:HmA[fy{%bL1Wh0hR!0Xdue1NcK+GI[oQvR20oabB[I)8N!q5s-D#x=:?Z=sM@Zmz:)64)4}<?T!-)?N.W0b?s4[F:}:(r:)",
    "N^#lE72NYU={BY}9iMASw0vbhRYE(-x]@24kN}Fu2(21D+#f$x]2H%GCLfX7+<-&&3bsi*ND79Q+mS4l=#]vKi4j5Q:jcqp^Keo-",
    ".SNEN4lvL7Vz0t[j!vIzmVBN+Di^28.ygp9E=04Ev(:m}r(0ALN=+kCkK<?0QaZJ2[Yf3v5pbP-1.>Ihym7kF!ity%mC2F>ncH>m",
    "7ews53uMu-0AEW-p%fiV<Zmb.4De%7@k0zq/%no79H<ay9Fj5H(C^^b94NKwoWDi(u!$p!dvz3/@kkwA7^<F1VT)c2/fub@iZ[=!",
    "clcTiWKvRe5YRz}+t/0n6}gr0HYS-6Yi{yT>f^i)KbD[1ZKz=*-2m%R@olCVY4egwomVB]qob$a.J3#Ei([Ot5B(5G{xpVkrj&8(",
    "C7+W+zJtY140*!{CMkj!BPLo3o}WAUzi&.^ehBVEABMjq?7mqATl>^dy70%76DnHnNA9Gmc]bQ5BfK-sp)XY9?oY]JCwEt{}u0DR",
    "2&d$:<CVh+za+?Z>M.bKhKLNH=qiH+5fY^<fhfIK=D(CM4KKcGY7DMEF:g0kO+T6=aU{:Af5HiMS)9Jq9%=jqP7yt[<&:.*ucO!H",
    "u5$.[em@q7oHtEpPXxN-cmEDTnI=1j<s?@Aw<1y5kd(>s9K%JYcTmlESST52ktF:MOrU[BH^]F1U+vt}i:XTBx*p6gLy%BWTb()o",
    "Z-J(%@$#3P>@d-/2{regnRFANe5E(wK?>.Befsk>[ktAjT9dyQXSfL2J6wWN=Do?nGE7I1BRfOB^FXa0a[]#>HtzU?zee4yde]1p",
    "7W)2vK]qmyG.XDUn.e[==u=[i/X=}FW-B0kb{SNQ)<dqmX!I>xpR4KWPk>sMjho^?cXyzy?=]S:(kauMK9kIbQ1{UKVLCaaeIG)Z",
    "72X#4&-im$2<YQDkshqiuZ4cD1e*@s)GvX%9BdC:E#4jvJz7MX{.j#qy@C9(T3NyZlQl(1PAcBN:-xbFNFu=]PT<m1iHAmFmRuXO",
    "T?!U:t?<Lqge:NP=]i%GyKz$)v{=x5/]O}x91AGyKOux2@Ef^uPYNJlTkH0w@JXk[@R.ac&]jMl6n{-(B#bWM*+w<]3A@[YbB*?V",
    "vcurdyEk{4f}w24^hE<9^5{P^sOJ]JaTm%<y8<(HHf)$BHrM8I>+/{{2SEG3}D&wG=m>9*=sVf4rpb(kYqEIJGdcaJ?(m+Y-xR41",
    "fNHNFUW&}XkhcyT/K4E*c>:Lkp]2.SjqoG-5}<gE9/Q&E^5V[7YVPAJ}v29)gwLg?1z:e-{=ANX:K/Q*6zj%Yf63?jHbPmHneu28",
    "N/abr5t5CP)s7cWZ*avZ9s#!@]tDCFp$nYjXt?XPU-XJtZ^2>3kgtiGoE$g$)(C}Z<<:w}}w6&fE!l1v?*D.ka16ck7<hGnvc9Il",
    "O8fR#sT5.!6P*H45%/7KjP+%OD{)-[/q+mL8n8L8X8nF]5W8s3i+X):mdi&l2J+S(J@/*9Mt/kP?x-Z%tx/^7B&Q]6f%Fp7(wnAM",
    "7yFo#liLxzcu2wqJP?>9ba6xmN!x3h0hfOgBuJyk+US=5L-38pxSrU$S4YNiH.Hcdi^:<&Je9e37[G6/Z(.:ju}oT$p<1@+XwQ=#",
    "Ji$ER4g$OlKTMXZDbI.TI{U#%^--1ZBstwD+/%n75uQ]9>Yh<1uoAW2c&13i>>&xkQQumGvW5jO@fXM&0sfHPh2XEU%6chHfK(ou",
    ")N31$+k0>To&]]VJZ5jWB/y({n[<>WIeuV+0?p(b/&F+IjAcYL7XAqyhd+%-U5T6Ds(iEDaEK6gmTHT?)KZW/jW1AYob*R&&hbbu",
    "uK0@4/Ws$bhpnV)I.jLlPLc[YrIP:ZskeP@cLeE9-%)42P.LoB>[f^RG1T]x=^oelg.!RbJ!K2/s<$hItdxFO7!eKfG2/u6A[z6&",
    "axz#pwkBQqs[@rp0ET@hEhdHHh988U3&SsMlA(XrS(>7GL^>Cw*t6+j!?-r0?s90WDFP8][O:Yb1JhDEi/-jl3d+xQ+DAM.1AhNG",
    "zWdmB=wjFAP/H99Mi!0+>AVv!a0]J5Kw.]sy7B-p.X]ahN$)y}G/zgU4AGCJ/}wKk&?[N%M+I:#HS26k.buT:[T(t+2wO]QV#7&p",
    "?qvDWGUtz&mqkT->&Ou#=#y{bpL[E3lyBPj{-LIU8IL^!bJOO.z@jOVe7QHWyRqR*cA8^mtpHjGCUI?:lnsbb+S9c@fMHL:}(tH^",
    "2X)8Flv).3h{:PH6pHhW-c(QsW+:]@t@%-7A4$[pP5p3>-ZoC-BRF/eCpO]>KWN41PN<Kn(G:LCGao&OdNLND+e+mgc{IT+I4D{Q",
    "T:F%rOGCQdY{}CLgr7F3axC0}t:L%:*?W$AdK]ZcZ%VQMN-FL]Muu[90mkO%2.u<bu{L>%0d!VAFD1j+d4pdG5&s.{Ol}ZF0d*@V",
    "[r}=?g#&F/BZPg5OEemUA11lXgtv2!A)I3P+J$SXkRj/R8xaR#[ftSUvwb9-%b4KMY-GW%>o)k)jPutl1py?^xEaVZgWs*1xRq1Z",
    "e2udSfVP>IjeFsV5&twc+k-[N2dP7BL3^T<4*&Xd<eX}sv2ux%JespzC?BgLs}[>p",
);

                const ENCODED_SMALL_FC64_M4_U4: &str = concat!(
    "m5u$FFQjodr)GF.DIE(X=}+0i[ot*k9:tFO=@h..4kU{=VUZh:)$d7x8yL{aEMteaH?-ksi9TM*ei3%0bL3{V&Kuwf>NR2-QA#A[",
    "60ol*nq/sre()S*Muxz{>j0]lh/g^>sxi:h!?(5y[<j=!Af4fmtAFS[D+?l%X]@S/($g2@)KPZ00iV?{N>pXNsm91J.^-E7FcDj&",
    "uvO&=BvC1Gjsn^}NaOdDG1{(&-<?{fE4(Y=Z/Z-LM)^LHi6.]9tY1QDH$D8#Fn}T3S:IkTtW41//5RMGt:[tG5#QQI!{AVBhI+0&",
    "<nIOk9m1:KqUD.BhK^ZZH^zb8:uo^x[4y8iIMksxsC&-:E5L*-)LzTw6JMIyI0AK6Jv}$e<+4+uH.>]Y[oaYxFdH<bt!crp0lNF!",
    "qAT3L6HBfgfBj?]wS]u>&dEia=(}eSsF1@]J/n}g0o{NFGmpty9WF](Lk82?Gm(m#a[$]FDRSChhZdv$=d0rq6Op0WB^6[Wz{uf.",
    "9YpNxz5+4sOLk8bV}?7yAV{o5^3G{d[]sfjFw7P^!cHuk3]@ZG2-Yh7VHJDdSknc&edVQhy8>:p:Gmu-81DVIdbqOJQ}9hjDABS?",
    "u[K(h[jyKg!/r:lE4eG@tqo3?flffrBBB82gMxL<1]+Hlr8=5{Lh%>IM+kb/i4U-QQc2OxijpiFtipQNvg{ZCp4%]oeS0cm:2CM.",
    "a][e%xCvj8[mm}HUZas5p&(K$eImh?=UG2-^m1WibR&Q/p[$cPp=AX-XkgEl7ZMG(SiOU45)8lFLpF-dY*v)@lw:U-WUcVP^^v5%",
    "/UE5vs4GWg)rNkB@.wA=nCPkFJ7$9KUe[jr{h(Cgk-.XNAf@4o)2U8Ti@k46sO<%s9ntOG^3dFxsxLKnH/&-Ks}Eh!@ety7v1}y*",
    "Ap5W#@fz(DLO?6D&!C]Bp%G&?n#*<-/X!FEuQ=q8qLhV9xAv6Qe=#%78-=Rp<g@Nzv8xT<fs#kq+5nDpZ%N9:*5N2zPQe.yzSWI6",
    "{eu&i]DlVrxw{$CHNoZdaumoJk>{tL5(Fnl[hJbndj@=Q8BM<7jQNgZW1=]u2P2keu)E=@KJ5$qvQ#EK&!}RoO#DIt!x!W?L2:?i",
    "4P7G+8K^S/}faB8nJs4teF5KUC3368^3/OISfh$JCjj(.]4RFNEQdgX[f(w>ZolUjXnz})1tJM.U.V:J5D5@Gv8vPgzUIF<k*Peq",
    "&9:4aerPG^UNpAzYL=4U3$<}2XxMFoP-n1O1qtQ+OAEL#N6%/z5rh&oH)[@uH!e7v*c[-I(nP)2=0p/AkaF**=h?7Yu/Cz9&HUnb",
    "v0@W3k!EAnPQZnt1Pc#/.bOSwt#W</d[*Hr>mGNmp+:GkwgyP)7&2N7R0a%A](AdbqyKT&-6:R&>e%$)kSDLwl@*GP7{^U:KqBML",
    "^$]ef:RxDxYOKcZ0$]VrZi-5TZ$%7AX4v13ufS1+Z}il6yzH%8oa?#Nqn/GG0+8H1NimkE8>SLjfOy</C7M0)]VJvWzVoEBUPd^F",
    "2P#]a6JMOABC$f4&ocm=(&nxCn<wt0kX}vJv0KilU*n-Inh1^xiguy2if}d?mccu<nxRgIvLN1d2?B.[fYZ!F.8t7MzO6k*df{zG",
    "d?sH3>FmM<&QCFYGWkPuFcGZIoLsC&=pmuqA5giKurQe?{HYq]4P[PK+5Y^gQ+)iS)M*DUZ]]sMerJN*Gv{z>B*Zf>f02EBcj[S?",
    "=#dZNNJ*}d{&flC/J4sKM[o^KF[Jg*FNOajokK[$6t){oG#O!V):/>16ePhx4(ooUY!*bfFAWn4<V<{318W%0ytWr1y=.8GecS/4",
    "-}u8&:Ov)gHi{%7r)GaaMz%5Xe]6i92:&]JPO+Y(f4b/]o-2QsFynu#[cfp:23YIszwgX^n<p.qFomW3dNx(zUO2hzOs-]%I5CLu",
    "he/SXfI8a5!9$ZEp+<uj6hIGs7l6!bq[7gq9wHhISoYLTb!%mu9bwS?:CFGYmt#x5K9lz]mC+q.BR{1H6R/?mcq0$}k=xmE<Ex<C",
    "aTGxoRSnQTUXJnZXKeCvnCdr)v6bQ9UTC9DCg}%tRq2E)X<mA4sth%5)tGx9[B%mId.rycVHAl8)4IoVj/*lHFf[$[YL{rM.SI>4",
    "l:X<oH&CC?Dyo)UZ-)[-[[jJb8O(i.5DsvCkZnCPqUMIvMCieTfDpAZ0q&+c5s:L0Z]-5Fp/E?n@/LEsg<RQ72.(D{^59nBEZ#ss",
    "PHB<N]O{-3GCSgL7#u8[u#g?n={:P>FN[X[(4MvRv/>EHJyhF:c$[x0eLYIkxR@g1niD<-&bL?X:r(hCsKg+Cz88<ugDTy845>L^",
    "tkN2psBVEC!AscJ>RwHqzy=ZtfwpcO)+ZHE^:Z$7b3]]n&kaIpW2vn-C6.lybsSebDTQ:8YbOvBvg0vPTz-XOjV*S}5RYX!k3>3F",
    "DhRM).e!tzis8cr+@yJ[W)@nq>!0z0.ScA=k+I8&4:H[l<wQj+LS2}.}x}j+(%N0f2VX&aBBmQZ@(HnnxY@:nib??W7hFJnUO4nQ",
    "rcmj+P5uOhfI+SXSq<VC1Fl@5PCUm5)6X(p&WjgTr[fw%!mdYb&g94@Q<!p/C9CMY2y5IFd4pyOHZU]I/L!%DtaCx3H?.M9IT5Q5",
    "km3sk/hbQlMasQ(U})v-lsNU(-SDaLGOp75d5d($-o.IbYc+waA:dFkw@9oC>NX>1J[P/jjE3QzaI&JtQI&oe:k!X:.6G7Cd#KS@",
    "lXr7!)!8h}ZA}^KJ]M(#](maD.caZ<1a?VcJk)A.b%-Fm1/f<ny$V[z5y7^00j3h$6Lc!#HZEg@lD+L9KJSx(CN&Z@aLL74maLc%",
    "GHt@awl^3=Z^FF>Mr{YbfDTjQ[wi7bp@o[eWH4NH!of+lU.aWz=t!YO<QdOySwG#a-N5qj<(8ll7izHJ!$Ce*I#KkIDj#l2b7Cwh",
    "KC7(>b?}rw^a$DVPZ/>i)Q9<[&ZzM+mAWIK9+yf(3$kHoA7Zu%<IVg7MGc&nzssTvGzJbt-*CaowRb!4Ar0155r-3=ej>l5H(M*B",
    "Fv]m6Enf#gl2l27OAW]7TT}yOqRzNz5w4UJoAghtR2Vy^r$^?.gEOyVP:!v(W2N5U!GfvRh9mV#C<s-XwCQkTauP=?zyr4VfD}TL",
    "E48K$cD#1[Tvgh]An9xo^7v^&82O/}&go2y5<JUmTsuj[H}Rk17)q!Ii[S<-k^&QlvXQ@Sv95gb&LqLerl[{=fFW]rgLhR<]bwPj",
    "Z<?}rskAL.z=2&mBBT?l*$xz8OiSmJfK{9Q]E3uCPeJu9&6&sjH%&SyyJklBMx/t22Y/C$ikarlvaEVS?Kg4HFXCFLb(od9R}[I?",
    ".2km+KGDw.Iz1XqXpTK&VS%PBzU-.k&i9IKM@5*QjGOX0<IXX]Bj!ju]#cad-!J#@E2Xs)RbJ8Youe9^O7s*SKFdv>&bl=Gk&)5G",
    "&ljkBxUEfN7k7Ba{)w>goJ3bG@hq&[&aO&EtY8yivWc7t(@k%TQY%8$PR?ow9?hyS94.oa}ujb/p}fW(Ft!F&>S4M+dUb=b.3C[g",
    "oSJ/V(&a=lsZh7)}c-Q5Z(-d&I.tEK5#Fsn80}<EqDR[>sxlUf/EiMdjKcscxFSdwwFI>&LFk.-sRe=?Sm*q}>^paF}0G}3jw[hM",
    "3HR{$x}^pA+H][dj3A^gW=^SM8P!8ODoek)arx)-wT!fj^LmHX5xK^.o2r$V?I$F%n5+/#5rvBr3nxjUAdp[K0(R#t<-OH7r[O#!",
    "81Gn)xqyRK@fk:BGL*!6E0=*BxH3:Ow>eaHsJMVq97Vma&4yS-C?v*mq0L.S/E)]K)07m#[join7b--/kXhHW5p7Gy0&2lGtaqz5",
    "p&0$Zy+FyzE6U/CnDT=jJUcp}q5FSj)cjNkR1f:lMg+>O*H-630ijWlP7^m*jvMkj@pJ/Fd-v7Pws%du@o}S*J1Jb71R3FP2HZaB",
    "9WnfUaC&O2.Mrt(p!Q<LbCqw9uC-}Wuz>g)7SIxDkTpEAy:bxj-(=PusF%0FZrAK/0LbqBHRpq.8&Iu(qs<xx}Q(&KKVokKSq]yJ",
    "s3*04)dPT9tz$TbrEpD6uWA.Q/F6-%GnzxFJZ?*q2SjcCFtl>)>OlE#d9Gd:aOXak&kk8$&1H)U1usige!bMU0kA&)}04H+GN59Y",
    "]Sg!JFl+Vb{ZN2hS93:CezLM+k!C30p/0/GyYK&a!h4H%7h!:*ok$oL4Dwf*LGm5)N[5pE=DHR$hai.wd4C1+Q#wR)1}[ydPYIfC",
    "Z?KxzC&-zF4<nJ#f6By<2=KT-NSK+b&]!a<w^uL[peYU9q6=T-}z[vk=2XU5Z.[EZPX5D>1$w1:=ki&J(3L#:(G0VkM]@+V{Hu3z",
    "qb2uI1O2hHJDpk@5.[x<C>%u.bpwmf}MQ&1NHPFmv=mzL0^s@m&0kT.027tOpY-5TJ!##kLY9rWtPl*ePG*aMN5F6sZ??Z^J:X/#",
    "lu}+dBL{DraLzVoFzji&0j=Y60rhmzKA%^q6.B-9tXXvz7i#DlFk+Et)KA7/6MIWp4{w}D3os[D@gIw2}1.59X!.V^K>tM7GY[-n",
    "hJ>Ehu>Vy2+Tvo2k+XPFTi)a]HhzYdl<-cvjxVBkxMU)&^6H$Vaz1bb@6RX>LC!NEADS6X2m@t%opVUU9kd?8p]ca-sC$fY-36xs",
    "+639n)Qn:K/$i56SC9ItO8*V.f^8@Gr:^yufuz2z4(EX9}8$&v:K}3oUiue7MbC4j2/$NWvi<n:0q#-c009%2MA5AESfXaf/4]]g",
    "H=.MGG3RuFnk-8z1vm4vYH]coio6i^<I0<(0oLljdbS^(ddA=<5fS5SKxNHNQb.l$Sh3ZZsqeUov]0s[N[Pwo*1h=UMcB0PEwMZ6",
    "qb?l)h)Gi)Y-K?tCtXTUZ^3[[6O7IyUQT.cib^Rn)erzp@T>m9s949IVCVHJFihvB3].VVMjZsly@EKgq#IJ+V0X28iM*hv:Zk@F",
    "5]UR1t1WCXg/ZD!07sAf1oxsL5clu8==Q#9Pa^kGKLf=I{p8tfZzFfd>u(+5^7to3].xT]32MC2BXeyx<?OZzYr.Y9[JxJb=COZa",
    "KBp]SvQ1HGUZWU91p/rjaB#5R*?!x3fJXZ}05U}jTC:$Vpd$w}2/HZCU/5dS0*Rm#d=XSloE%Kl@72FOkWllg&YK&^&@m*#ERDIU",
    "W&E:#?HCI*bH&0(*qz&k]oo5luv%{]&8][urQHU/xAHN+2yT@Y7i.h>Y^:=f9<z!hF9b7>g9s@THN}<pYo6M^B<32-n]zd4T%=7&",
    "1Pa9v>DQq=t&%N1&L=2nm}S2&PJ%GuNP{=CG{2!bpv5]:(NFn=:ti>h^FEikudJZM*k{l/w<xM:p=Fzq5c.^v=)Xp{Y@dt.b?2#h",
    "v!7}WnDbl{48:nIK^lP[n4TO?/mD-f(P?Bj8$<cx7KFFiLAZ+:OfSW053y8&b-<(Ez(EbdSjGNM@8[kbu<Bk8KjIp-vBe(93B$2!",
    "F>DRs[#rA*!UrgcAO!5^QeRW/}9Sut-4K}J2T/$5.XulpNL$ylXPS]}dq[]zr]{wC4TfFrnbdzADf/=taZ{^6fJifVFokJ&bIKiL",
    "Zw.R908RsZIjk3&KxK4@)Q1tP]Xr+p7IbCpHSJdKM}A2}*e0[}aXA/&H?F4$r9xn=Y@+UQUkH/6Q4f+Hx*SH%0hYg$Cw@NahR7><",
    "W!-[o+:.qt.e.PJPNztNDrU$:euw-.=AYR$aYPt(L=Q:})(Yt/)q{B-XYx(DnNTY{o*P1EqA=<1W.L.NWSaaq47Jr2P#JthfY(YQ",
    "2EQx(A#nEKHWH9+bCSY31Q[c&SC]qUmOmvdgt?gRs{@$D*Usx]?U[LeYmu=)E1E9^h1*gEg^do>=YvPWe1p&Gmj1/]Qh]Chi?%tK",
    "=/cuq:Yi(9/U62M^9DX{]5{$9yW-65*5V35W<#?&MSG+ujjKW4?Id{(/kq{0OGEbW^=v&1[AEA@:OLl^H07h}yJbX8U/o?Wg-c/*",
    ">CT7#.-t3pJ88S38paKh9L6wa}{jupyD9R78f?/p/=ov(1E+1TP]@J(zy$7-mHkl(8892zj(FigXBRsp.-l?)uG4A/uMp#qYSNd9",
    "de&LD7/!X>sDw6EH+}ibi{!7OFp(jZKy5IZ[iARUijrS^F#hkQ(<qJi81uj(U:0<7oOd<VKD7IGck4&yQE@2fV#M)BjZ!)+uw(k7",
    "g-76)IE)tWUZD}]@Z9OF@.52ga+V1vOrc^+BTspQ?Qax#/K(A[YUv=pSdlY!nlIDDB3I!1RlSAKLL-beja+8T@v1HaMN+x!TCJW=",
    "}/0*}4{3!ouw<+8Ejz$AIQ})ys9I[M(Cqk*lVGU13qU[gc+r:2<Q-xBomKffz><f[hO(sQ41B5EojzQ0pqTp#.n*JeP%-y.fSEl0",
    "WD%kxRH5DXS={LsZ2tG{F{2hG0/eV%J25JUhKY(A{QJjC9ODYu@o*1qE9JCt&[h8Zf*cdk==gmAqLj-r/Gs{M/T$cD:1Y}BQ<V66",
    "B+6$n03CSfl8)ZyYNtTJI#[NAue^3vtPqb8{sxX^d0zgBmw*y7g&uZnquc)m+5<C)N-)1&ajQ=-hWa4YLOoK#)R>SV=oWgGSfLv=",
    "qTiCU0%R^&+fB/!Fu9GI!**X.S*JT}bY!jA:k4JB=Fv{4M2FutCfk(U*N7Bz0eMK%Bw<9(p&}ZW8<FjMSpP]2i9W*{VL:W}ZvP96",
    "Vw:(d2]%WySc68]3M*OcZXn6#d<ep-G&W1[z6P!er6WcvnKS8iRRlkTb.?%Q8*?v]fH!&2*/9%XEcSM2o5lNjUuT%?g]r=b:QnD<",
    "B@0(/kyZa9m>*C%ileztmHWQ9QV(j3jfJ}zEtDfiH>l)o4<k7uQoUiwcw1SHUxx=Ry^F:@sKG7u}@E1PUH#*^ZS1d8>#sK[enqYe",
    "nJl#rp++y5F5ql0HJ/pL:-]hFh^z+ja:(dh]>%E!86f5E=w(f]Fmire!isOXwDRwm@rN8J1H{iJM*1Jmc.bAzSQs99Y!V[b+b.eF",
    "{@>xPjy^3Snl6v/2M56@7?ugKP-qNoZ8OW/Ues*meJ-Bm952:Nw?aw9!^B4y5!X0OQ[GIbx6b@4i&RtR{[V$l4GJ]IPH5=A&ipP(",
    "R=R=?X4Jr^{*@Q3:H+[B1yp]qpzd]^?K[pvJWVG)pAHPvtM(W)>jF:rpDkk+yd@GUBixlJzMlHD1G!07N!N-j=Alj(:wB1(]bZW>",
    "EJs-*=}xFk1}2H!BB0C<PAv>KKY<uJ&3?skU$S[oq$3J%3qG<1l[]9mQVA7JhB{$lAwRn>l&$#NWVYi93O3Op}m>rt9q>8@jAf]9",
    "x6I:$T92NHxkywT9zg9hU&qB<?S]o$1JQ}:4q]0a4?8gDM%Cnb&3?phf%R[nv0BlpWh5IJ)yqTE@kZI8E*g@$yxL@$@4XX{q)Obv",
    "XfiSxiF6VkQf]{]}:mbwKiAUG2:9YJ1x69J0.Z2py0=VrUsO&g}Y[x&lz=I3DdMv^d<CJ/z>a6@ss7ZBdh-@pYq5&H[wIVE!R]kD",
    "zrzZ0jPOx[2^!hbmc)Z->$%pK=b2(kQz<F50*C&hi$:r.+>Qo.{IT$lP*S(:ZI=VTU0zCs^>.7<O=[4pJ=Qgy+o8zKvNLHQsZ[zB",
    "2dT/I9F*%-6Q[nI.p7tbnbMYcTbpKNB5b9o0KNG[0>hHIE%Lu-p[!]ya^lF3@?OgeAq}e%TKRThoF26.NdJ$:h3r=*W:%YP.2hG>",
    "hk<nk?m1E&8Myqhj2c@NZ?BPGR7l^MwKhf!o(3){sn:U-4vnFbZ)>fZGWrJe9^hV&bL[o3NCed+z2335k}(3EN)^#OWUN.[hpNKE",
    "va$T]C$z?^auPqfjUsaPlQGzrM6aky{%0Orj/hQH2.)DmQh7Jd/O-Fr0oxw9UP9USQ*:qo7/Kc)gC%8m1okBpd(H<2qGa@!uR4jY",
    "H8eNT{>uF@hTkR5Kya-}kvL.HB=B3$D%hYEDrjT>M1CrPPB64fNkjO2c@vxp-5?2lj6cggTxR%y&j:MLalE@l3)b47J53FU4o0i?",
    "e4k8-zqHKwp}}YE{dsvfxFa5kehzgUpn.^t4Rx*%p(??z={tBVS6hd-KN-aVgraazrp{vcYE9?Cqw*fjkVqL.!)%a>fNGrLN6[MV",
    "n<1]yiC*Z?!ucRK^WsMd.f]g{&k??eW^3b<ui#>hV!?N(Qi@-H7oPXXLDI8L5r!!i:3##wX8MjaTbDBgpHcKOA7B6Awb/:%W?fMy",
    "75rD}Y{/o&.<]#8S?1b[H%1Ho/)O3b-waCS=X/iB1y{=h!L*C2Ylap>?]%Y=MXDqu!2i*wc0A7r/A-/*kP0M^2LX6TEI1rGwJUWX",
    "}i+!T1FEzhv(e)6LETELm4fr6MDp>@^0gCX>AEkRUm-=Ti0KaY[cL2=t!/z+pGK>=U2hU&wokn[RldCAF2]CdK-Lq4wy^bi.m:OE",
    "XV)ODh+1B{8I5Tqpy#CF<CsO+&eT}L(%gO]xxhVG&(J&qmcM6.Mgl[0gx-JDSr1OlRd+H}J4Pj^pU[NM.*oRw9HYwRn*vVPFKEjn",
    "]n*WH8xFH:FQ!@U:eyWP5uXkY^BF#B.?RTjLh[WrOu^aE?ifq4qBrETAppVSP%TE9C*x9#K?Er$mt}qYAo^-)X6$^HChrx^e:LEf",
    "OsSnnE^0T>EaFD-:YiS2xd-gc+zt]aMi0?@Z9^s4uV/[)h}Lh}fE/HP0=!/nfN&NQTxj]8A8drm-TikuKEik:epPZ{C/csur6#zN",
    ")PpKA&jE*UUP+QiPNPi&5o:Nb/.@4/gs*AUJeR!>**DsfdBKXHDc*4+i&{p:&W3GD<O(ex!Lp@+a:)Oe0mXl^/(p^>eQ<(^sP6g6",
    "i&sX{svSRhBM$u&J5R+v+PSN>@?%2mt2%Z@iD-h4riUS2W#yLPpC5uPfwVs+Z!!EMv]B17[XxdSydm8AhDJqXDb9v)<x]45*7!ej",
    "E.4%ipYWsPIL^k-*R6:NnwV%(5ckbiuMo-[5)acSAceVO3%zn%DMES0N0]f5N8PE%p{Sy3KpJ$T<e+NhN7d=hA5*4{d*NDmiG<Ug",
    "wpNB*a6o)O&%7A8cL:Cfr!fD#qh}Fp@xyp@(lB6T!yZJUdWgWZWiFzW]>g[I5diUB@iwb)uHwQ[p?Rk&8zJl@Q8i>]-)5Fpt/mWE",
    "5$<hcFnwP8C4jB:{-KrnjALLR47F7bzKJmy^5XoL((dE?qFn.qpwuGRfL*3/RQLWY{)lK{C.nWN9k)/H(<%7Ug6F(o=#2q=6I!V>",
    "^et{/-3E=mU:W48g]e^qtCYMS-j!NJXY*kI60P$JG[0]5idzgDZVF9Jl1GHG72g7F:n}pm%3UIL!<Aw0NNd?8WqM4eMdpG1Nxp$M",
    "PCRKW+Tutj5Yk0<ahrp?lmw%0kgH@yG&*h@e3Pw[rgq<XtilhPF$]M-l[XTa<W2p#2!UC%ezTjrq84k$Z>@04NF]*[M:@6*U7-lb",
    "1P.4QI6a^(v+)G^21acUy+wI$*/N@FySYggHk<bn.XpwgewLsJH$S-WWlS]*4u>8h",
);

                const ENCODED_SMALL_FC64_M4_U5: &str = concat!(
    "LCv1%v.mjPa3Dab:Yl>N3<CrE5oC!60:iSd02V-/K@W#z5@X.j]Dp4{6p.C5p)UYQm+Km]p$f)aEYGrUmktN&z42.[5PO=5ShPl#",
    "7+]2N8UQW7Q)DwuEel{T2sc5q2DXF2Q1u]<-%(^$WqN}o-Qdk1PNQ^(.jmL&SZI@Zf*L#RY^ETeH[+o&3{>E*Td+([Y*O}+PHz+>",
    "^J3p<2+d[Up}HOTA21x@tYA1(0j4r>f0jYT0fAtON!MIsc{9Zc[&bt!Z?#I4C?C:6ka!udBFGHi4:8HGg#EW-KPf5T3&}((.CAxT",
    "mm&$i9UoOYVX9?xw<{7v2#POxHCjp)Lvzbp-yA}x>=T!BFx=OaenlSFlJij.WY$MIM9#&tBn<9vp<i7bIIq6[M#qTXA80EEhOlGj",
    "Pyf>kBbju&LsfsLi$wCQYC!2/X$WiTJlm>TJ8s2%>^SVqYuGVPZ1T+{Tun<k&xd^/c1k^@1^)y{4p#@W9=.JIAI(=1s/WDzYxQ#Z",
    "/SLSCzQ-8UNSUZB&04Z}H(XS2@9OZ4@6es!5i}3{sBHv=Fh$+mBvsA/Hnr&)Zr8hAUOma3w}GbcN0Hf]hL(+N&e8(}FcTAl*]}E9",
    "pmH%<20[kiFyFN3M4H.Ndm#iRHd8Roi4OM6{JIY-MaQiXOeh=2f.7NzA]Ij8U?7CsFreA%p/a^qFg7vVNMMV{N=}NInTBrSNLhN<",
    "SpxNARVjME2*ExMpg8+?CCq923VM}B8VzXc]J#-7Cg?O.^ak+uaC@a<c]O>6AWMyJxDM!e=x{YcBA%urVzm:&m)G6$xl]d[nh{Z:",
    "E#>5K@L=ugO.-*0Ur!KtMfYH60GPf5mCCsoYCrSyre=(3<M?H.0jLP-AcFMIV(sq7=>l{N[kq59094ralwsI:/XtsDQ)^Sknq]!%",
    "r815&7%71n2Vq^dDHFj7Xb>T5eo/=gPXwu!(+.q/S0N^:hlt0nBc](<J*Zi*o((7L45Gc1y3f?aD0&tsME}Ip{ZLlRk3N/}Z+?rr",
    "DI#^nX72:^!pnpEJx6UzBNy/}6B2ai&!1zF(n3/3V($prqFnP-t2nA^1.ab!i*bT?p@fuF0oAi6kTgZIT(m+<VLzyZq%M3HV-B+M",
    "RG=iaf-M%vHM6qrcr3qC4F7sG}*fkBm8*]x2ht>n19-wfgrDr8i3f55@{P.Eu#QiJFp6X}FqIjRO)!lOJf1AgISi0@PEq?kXh<e]",
    ">>.3^0aCxe3t(}e.uXq#CRb7w&ekcf[uELTv1O3-J0{:<Xs=8I^0Pcj2#$m{erme4AuoY}}1fGVIlbyBuk!.+HD[8q{Gk.O=w2@z",
    "oYv#]*d[za-lJP<ifPW>cH&-L<75FgMgi=@7C?z(H=[.J:H@}lFBmzsW3sU8wRl18{O%cmB{g(d@F@%47($o3Q@xY7AcJS>AUj!O",
    "4=E{Zbk+lJOrQov[Hn6lY+lTB>]C(0hZ311d<URrzj6&Q2T-Ey&h[1-]bi$cD!TB>WipTB:6EMnfEr!-C*UOnv2GxA9otKhS8pL%",
    "5hFmrMpv4JoUoZQfV[^QJi>mKwcsT280)tDg7/?=]Als2*.3+NfLoud0aE/F/6?83A3!^KJkdFqjsZs[@kX(K&FMRTagVZirGFJx",
    "WN&bjj4>t(lF=urls08AXx%R9&d6y(PWM$8W:iKr[p=DRBDXF]YB1kyyNJ$>91v9P<Igx3kNR2k(n$/PAD<f2NumM+BE@PF>y/bl",
    "NdZOA3)6h$d54hm7>W(jUlMgj39q3c8=+m#Z#):w<.KxHcK=?m:%awV8fS{gkTgb/q:HNI=^[NG7IC5cd$C2<?Henm-=4uVFi#35",
    "q0glWRH+%m7Y{[NMrpYh:shi]A:=e+C^WGjc@Uu6&n3TYltSCXLU(lW9YQ7hKL?B#v9a8lF)2xxPNnl4*{@uR)H((&&f2xG}mR<5",
    "+[Nm)05]S@Vl&>>YrrXHwHX]/+LRx8Lo&4G)O#FmKHjk+ibxnwPUAizf.4Av<Lc!02{dHc<##8Bx-5{=oHFD=Ktw8[TxOrgQe[Ii",
    "D8e)3&z&DiVu?IyojwNE&5a}aN0LHtKG4ARYSqSbsH(Npc5hMn5SO-.WLd.Cqp^([Qqu@AZ0$>4A$3Wma$O<tX1E}dlNdyI&pt}(",
    "q^H-.}?{-MqmyQg45)O%Fhps<CeorOaF(Og9d?6Kz2P?=kZdv:wqYhaJm}e)=CekH+d#Yze(]zz*=s#tvI+.3!tC7n.1WH+<qfKl",
    "LhbPvYXqBf29^g(JI*?XmIUs7@EAAZv9o/S*lrKw0$^o3Z4!5fd[9vEz22x&{[go[sOMC<JnU49PKd4H7jxyIH:ix>t?rBFX}UX$",
    "en]dlJjQJ4mHC9/PNSlsjpP}aX-x:4CO$Ck?)P)P=#8W)vYDAv!>Q(YxTUgU}[&?8?:4i4zY@^Tri!CDWeyE$e.nsZ+1EpH{vO8w",
    "!dvnC9052ZiBqXFX2LSQH>Iti+v6u[TQ#1aKs&&jD@*6R@bV*uDpk@&I0d=2(@cnmGK1^BdMv1kr3RBRPB9l.e@wQKvcc670+PXu",
    "AlS8W(&&oWZiG7VdU2q#Z$C[MVqe&YmQh@Rd(<7ZfIn#v{+FQd0]7x4PwcwgLVp!%cOp&IYyFXeKY@I.Y.Z5tYHE*)j}S{<KOp&H",
    "@=^/)K0aUso/9}Pi!Is-{]zU5AUtE!b1Ep0Z@*SedScrs<Jd@}?yHYG!rSfUJ7*/CR0r(K02Y)&r4K>e9e*WSld*g>&RubgiR(1M",
    "EG!9i4?J5gQR?F[/7@ufMvZ}Q(HF:OV1^jJ}l8Os2{y80DQ+#/UYpe^L<lWW?G+qFFo3ESDDJcGd2T/Z@9s+mqEFcu5yyOy93F+7",
    "Z^2T#sJZ-U]M3}AihZ7CZ=]?3-Xb=O/OK]qxQO9FLYIR[FdJ<o<Jj{Bv#o6Pq#cp54tcJ3SktBB<bn&e(Agc376iZly(jDuC[/8E",
    "6WCw{P5/QUH%LAJ^3%z=X*mY$}sMXVnkjJCb${][{:p0[SpF(V/B1GrA4u4z-p^4><A[$Wbucnc8%$B54.gsTe#Nk.swY{GmXw$V",
    "QNm#yFgOdx<URm@]!K&RR36/ilghR6aBr1W/HsDa?dEeg}E-X?QyM?cBqDZCZ[/4kx%vozjPRd/c#O6dU=>8&2WK8^08J}X[BRgx",
    ">V(X4d5%I+<sg9(-9sMJ^3-]RKBt5g[B][{*!EBOt^}wV&(.0UGV!UOkg2UfxQ^<w2.Gixt!U^5>{I-VuBW>+:VY{yrpfj?X5aa/",
    "(vplz[/Y6*=T}KppO#e}]F**6j}Cd^/Up!bUXi4g-VKq8V@?DlGTK-wFTRJYC52vp3p}Rsp{RxfCQQo*9o=9s6&UJ5lkDmTQ<rhc",
    "Qn=}YindADQ^IaAN8RlKY34v<nI2GSL*6L0EKQ]4>azuh:Ax3}Uk^VG}10<!]}^^lk<6BR-}laux=F28v3zS[lu[N6I>[9Kc}@j8",
    "2f38B.LsJLe]HnzPQf]prWl=<k4g.fA8T/&k.rSgfMZsckf77A5j8vtFryRWSiB/n[b/8)M#L+Zjpej8?wAGgU!c*D<vgsR4]H@(",
    "TRyAv!l5wcK$##1nlY#BRLzT7^aI{xYg3tC8PO8Q>dm4}p-c%Wwhxxiipg0mTFlT@bY:2J49mzuhX1t2Jl-cIXd[!MU+lJV*g35P",
    "Qr<&wC<bh<.u-W}qnSZNm!#$}u{u55PF$ogJ0fN4i&yv*.F*yc)R9}hvWPer4!kMy+7*(#<whgck%kpYDCQ4LVWVh[w^QAe-WK>J",
    "8k2Cx[L}54XR8B}Q-Hk-xQ9%dmM#%F!AE/kv=kP@mcRF10+Xpw08}c904HTPaAm!4^Hsz:ti:zDQ#Wp]fDNEik!bxcvb}3}GG!=^",
    ">WG^&l>f7c[t>9LDW(eYW!B&akZd0c6O*FsV&oKflO.AGBGkp2R@DC[r{lnv4Uq}?MDB!xb=NC1w&Cl6I)2Sk-vbW*-=?Q1!I@A1",
    "Sj>*oeB]I#Vh{-l2P{rmquYL4M.$xSptS]J9^YfBXCaVsp-Hllau01TPFtv!PnScIg{g1Bkn*1ONrr+!ip@MI*2zoV3q(+}JEfo&",
    "oJfxMml1El((IOc{i(M%r{e>:0?d.6rcw)rk^S[<NbGBaOk:^Eda(8zcp:H/0fr$*BzWj8nkLBi1@+d/1AkZ3!nlx)t29zSnx]n?",
    "7w$B2A1}B%)6v-@A/{2TC&G^Su&S^h:}}(S7>gbJ*LtQrI{q2Vndgo/&^0av8q:KsgOg}^P]XuGybcpkH(RLHsMceutY*Dr.U:nA",
    "/JZK2NS8JB=I-sbrzs1rauEa@Z)p.cglfb.}Ck^cDdwrC<![rQ-[)@ieomM1tXMHq&3tej-)xr2A?gy!WR9B&rv#GUt?I}p()Ogk",
    "b2efM<Z9:aLnW4RrsCiR6jX.?j8wE>6.3DjVguIV3@x^l[N@j<]o&lmA7%xDk:plu63Qf+n]Z(AOhNK-kZ1x+dYg7@++>rKXfkj)",
    "2QIBy[ycO-H.E&(6Z(X#U.?c$wk7VtCXI(BG4bB^J]W::*lntC*kh4SI3jWe9+>8PJ&7vu2vJF5<nSY7{UvnY3kIpdw6k}cl3S%N",
    "($m]-&nY<gnwzZGg9Dw9fh*K/@gFCJwAQOr8b*-6UPf&e<#p]9!sq65LQ(WBvCyDqkdWh{E9&P<g.BaTM<826PWzA7PO^?N5uqA^",
    "HPOSW1GtP?UCcrRZgj/sXNgAhezI%zBzJRAE[]5/AtkxoNI{7TF1:M*U@xAXeFLUbm:r[QunIamg+TM@7{Pn!(kCq*M@V?)H)]LJ",
    "H.4]@M@@jUszNlneg>9&YYyRa*+f^k*cUr)FoP%kkY9dai<BmHLV5I:&kswj]C.Zi}A$iAi[ocKhAFxjNu}kz<&NM=5ldq@4Ek/m",
    "!FwxMFRIvYzZSZ&8l}t$TC+cj[(:LSk^-Hk)JIRzT77/^PL4osomHrZ}EcXxOpez<u(!C]0aYs}!A]@yyS.#bVNQ?{q3vd/k#]*9",
    "9mwgSoXvR6-0YCQy^7[kuBP:z>LNIcby3gJK1}}wPI=j.pZLobcGjv9btK=x?(:*UW8n1?mlh?W*tF2P8(<mU6km[tD=kFqBHaJK",
    "q1!&>}NJMrk*)DWFBn-oltqru&jmNO^ZWE-TsPV2(nFvHFuU#:PW1y!44)IZA]KC?[d<S?6I%+47-dY+>Rvcv&g$waDD7SKvkn(!",
    "u[.Ky.QNKdSZn8E/=YMAq61yMF2%RKi1ZnCT:1UBwZeQ=CE69tvzWMdwk$AGIdttNiGf4.g6QaW^!St]888SnXJM.376pi]fP--O",
    "3*SBeJ[ap{6^j74bf):ke5m6b>GL5iSEw8oZyDR{=KT*8>rA$1uaC+P-7A9PM=bf5^*>uI6hQ6<kh0859E[FXmD&C*W&*vrw7)8d",
    "PLEMwCqK6Gu@P[nW2{>vfJ6z:VeKg{?Vh(Nz02&NxRw&#Bd^G^y-NAcD?y1B?f2j3MzWqrjc-mG/gfw=[9cl63YSU10h?M=WEAT$",
    "BRUXaAh0-Oc&3rEa.<.mrWibV}?339CT6UvV>&5xXpu6xgPX:iPZU:>k+-/>lG(d@Ael]}X8@pR7^(6KO![cm6-C9@w@*6dSAyi7",
    "x&i.Ixr>h][iQi<+r$RV[2UcOMpL<RcA$I7ios8M:4)R/*t1+NhX5bqGtS*nrh3VYQh-DjXkaD=BDIl<6=)0iu%J$2cayJJ:h%l6",
    "^EBLbanZq%]3B*eWYvOnhtqa81RWlpKK?liYar=iUlv=//5d^!3bfRbDiTO!7:Zor1<QX[v%YgV.x+mb}{yf.h7Uxlopq&{q#?xY",
    "u:#JqSpVv?msKU!0x2D)+#y8]TFN14XCx(4LH>{XH66X7v354mRXUsS123xi@&UPKU2d5t!]bg+(/rKWZHm>(NyP)(i8xAmn8//F",
    "7dV24Q7Sc)ESm!uU!z*9?:mjO76YDaUk$yYQ(<rwVJ+P^HQjrI4-((d+P%Wh:I:64n>3AScBQiyv7{IxRCl)2[[fnZlDke3P.N22",
    "la8Rl:#c:[zzW+BK!m&Ky.2gkTQ#mgks={g1f&1)}Z*qYGHlwn}&25HQJv.zw#c$wio{)+ul1.:C<!o?**yG7z)i3K):XLPD$1tl",
    "-EX^/?]QeT?^I0$S:3<-vq/U&dYC=#A:RbC7$$MAu]*V*2Uo84MJF?N?i@NK*)JP8!i{7B+anO%iX9[r/zjz&AuG-{JiiP79=G1X",
    "OQ&4IhH4CI:e)-N^}k:D9RRcHK$F2]c/4Fdl[q6E^u+?nHRB}Bgs>qBhxvvZQ1hGyfh<BoT!INNyy:Ym2gc$@jSXmIfGsf*HaBdP",
    "z%fH3)p03d^C3a:STr^XlBQBrT=qqM5qU*0YD[61UUH#TXwU}>nYqvS7(G?ctM5L9Z^+$2y^]eN=u$!/T!3v[GgO3^yA9^Ei7.:=",
    "cB+A^*3ydJc!$la@5r#QR?/gNAl/H/?s2{e2a%IVpd9:H1NACep&IdKIrO?um-7[lvb6)dih01W?)rzF[)y0hmWc6G(U&:=--W=7",
    "3Yw(ol:z]&^T<u<QkL%l<[B}Ld+pud)N:Gzr#<%#g7#VjvZMh0zSNF7C]FC2GzH.B}twV7}a(TQP@fn4J@&GFp/+K#Bt%W[Hy#?=",
    "y/9^q&^co>Zeh^j:Qt?%fOe:TA!Gp.azqj#?s6!]VB:g3gR@Pzq!4TF4OfwV3[8p[:B9IvX3dK12=5rJ-uDY[c?ohWtfUEd5dY1g",
    "btmgjq<:*N>xFU]ZWarHqCGtr3<UU6WIIJH>W]Gj(vEFH(APG-?(3ApGe<dIu#oTxsT+mM/J%!9mtAfIRgKPr3YO<-G3UEJ%1pJt",
    "PQc9KKq3S!C=1(.nC2&{aojQtNcz^S09-sk3@BTJ!j42k=%4i#nePBYOcEw74exXjNU&JcYE@88CQPk#yird.rdh{5ee4M/LTFS/",
    "U*@AV04v@Zgw7{!^c(y@:rx6DJmkB5sqaYGaNv^}>TXJkVDDZ>98z2E{}x>zH}4p=esFc7gg8p<npB8)t:1=[&4E14PZ?On*j9S%",
    "vd&[H>*WqV[gl]u^6xW+FjJW5]5&]+SThtj@34a-Z&Jw^W2w]!u]]ZHm0O#2b5WGCF].Bmvxcotb8eI11+}#IFD[9Eb%/HkYbr3h",
    "2z4]Ghj}VEII.%e4<l/[7?k6Wn2utKvVs5BGGK(I3p2W%::Nm7@IbuoB.G:Ex.:}T!zmKBBgdb[s^VAQt5H2$uCX26^RCL&qJ$uG",
    "qqWOJ1^ap0J$YC{JX1H21FRK59(ppUEA8Sb{ZIO:Bns!dJBSFZF^*f[OoMtzsHd6K5FEb/YE9-]u0dVKhgQj>y}:By^=1j<4#i}B",
    "w^nVb/pVzkHgzn*8>}n+wq.%4xw50Ml6ktmjb1G+CeY!i=TxX=+6fR]s04TX&>ie(X3:b/es-v@o.djaF7.-M}Q9}Uky[c{:]TB:",
    "[BCFIX^a$ywZhBj*X8V4U+<VH.!jxZ^b!#P&A*y=aHi&rxoR(0*>/J8iYV3gvnjH1ZV^hYxI)!Kw=D{o4x<RjZ?vat&qLTGw]/Jp",
    "J=g&f&zkE$}I8t&Ysp0^RGr6Re6-Vd134I*yxFy/2aw^Xt75]P1UbD{aJP%l9(SB?OOw!KOYd&%GH:q3)?@I@@9t#ey#/0p?@[pU",
    "KV.[>wd/l:P2z%sgn+R%p>SF}ZF)zhI83XE7)(g8S63?s[idQKyW.[HrKD^frUAlZa*/rOY)6{#c!iGaO)GBIUFSPsLI#y4dxx:r",
    "yunCVUhH#%{Y0AvZ?W.#gmQ-@B(4d5^xxufL{@/D/>89+^jIsX(bVl8x$z2xqQb[qtGx]f0nZeT+A?.j-g4%+Pjo^[LW1.-v>Q={",
    "8VBOquMhqB+7C*uM3NqYsZdZ^JrZ(C:8iH[>tvm6wBZIH-c:nMo^2S-d1<9jgt^U%xIAFf[wfDm:C1X0^TCceI)VUI}n}wMuv&A}",
    "p=}Ss@e/Hyv1!qqe$=T!G.kX^Kr21G+MgE50p6l<TT@6!GN7u@[oTAk/FZWY^-b$mrjmQ%7:69%XsFU[L8-<j-tF->bIDo:tQ%8z",
    "Lqk?Cw^WoQfsDo)0i)rNMMGCDZ$mnMCNh%n7/Wfw?EQStxB6g4@Cp^.L[{>[du:&$idKvXRpnd3J62I5eI9hK39<y&Mm<yBY-f#Q",
    "T-98XrR:i&Z]G#sPFiZ=ZZ)GUo3H5z9@c17BER0#9l?zC8ap2Vt8N1a[x)N^c2RUPSqPKQMJ9Tx0<t6SAJDYmR*i[U<N<]+Ugku)",
    "^2k>aYkIw%Xt5mK27/4N^!KIt&RwRIfGo$mtH?$3wR9*]!5b@+&$ejNZ45*1UZ]55nkjp>C=9%yF>^4Ae*1E+xZ[eFbQI])dG}#H",
    "y@k8yh1gVjUSz>G0.)ZH>NE{kA4npU0mpK55dk6)]9N4.tuA7kvUO%rwe5%@]R/R&Ozlxzs3/Y%Z=]/KF+.gqM?-eCgw()vDx%S9",
    "cF[)sr<iQTy6[(xIGxSFT}2$Cr4k=0X!DzNC?AOv>drmW{CCVd*%cXF^YeFAo<-uk@Di@XhB%:Vz3/n^5d9J]fJ0P?swn7{]6hP9",
    "9%w-ffg+&KWUE<DU/7&Krc4MyET.I+CP(@Efp@569C8N>2c{VZ2qr0/c.@MkJJICdo2:ok-O=u.PEWAj4!1?W*#biC5n0f3+f..6",
    "(&{9DTd2r!1WcUnOeRC(gl:*-Tn3A)<Ssy:@i<3%GP5Nr-s:udmdZ7m0)7Ba:l:*dj$)g7[CT4::Lh89ZZ>I0HM>+41>[k=!WmL7",
    "mC1w1mWTDB>p?C0]e>jhBws0)FvqalGWuHcHo@5p{C?4]C<(Uo07WU:VyrLZLVz3M+dnYVtOIj([J0U:g1r<1&Yb=dlWdUaw!SU9",
    "A{g-XOFdqMlbhCzz2DYloGW!*+7)#2Bx^fRZ*XuF&@r{X/ZSoi3.ua^N@yv4I=dLTAtQHCdA:(*)K]nr?/o7r2Xf8Z(feZxPJI<?",
    "{xi-Btz]WGv8XJ@T@P<yZ}e%C9bdEF>CTLc{S+za8WO5V-J{KFSROed>18Zu4]h/iJgB-Fm/cNSShsKFTT?WXShTaqr[p-]:LWVA",
    "BK9xtcp>Kc<J=%LczS-rgqUt31znQ/m@M5Oi7x-dfIX:+7(@*MF+}(1-^!HZc]HxQqyFCbxJ8Sdm8QjZjqO8-BR*pQ.P(pFo/RdD",
    "qdNv6Fu}:fK.@Acfp?ZF{9F50JIY3aX#y(Sp:)nChNFDc4o3#P0p8IOmb3}tN8gC*-3>k<^Fin}qZ(7*MT(z@XdU(bHNXGhRdyDc",
    "5%eCz-uY(Chya!Qz/dUJ(#eW-&PkUO((/fsieLO!xGLv&7-8o$g(*w7Txm)8-Qa3@aq.{UWxowumjxv[t.)deauLisEu[/+1YK0=",
    "Ug9S?5#(v7xwvWyykKHPHzE9S2i7[iJ=*sBH3md4.E7u$J8Q/Y^%q@%dE{[hQ6jEV2VPIvMbvSaz}S5(INygjo(k.JASc+rauU6n",
    "[mVNJdL$G@lz[[s2K2a2Y.8@M-Rip{rL(abRv%Uc2&HI6@VejbS3$-Yng@#l-*NP9",
);

                const ENCODED_SMALL_FC64_M5_U1: &str = concat!(
    "b7ixKucpzQ{+<uX!V/]OVJ@&XNLq#Hm2D/lROq%>+c#:X8NF5fmpF9/WNy?ydj[c9OhvN=Qkj/x0bYS=3k/K1vmZHj+D*PEirS:4",
    "HYfAFO{[2VoU(m)RIgRP2yS[&Ux{U&IB%^avuT5tpcaL2R*E.<6vN^VV6B/uG}<>{*5d{lEE7x70S/I!tuzO4ms17dIvU!hALc(:",
    "S^x^*WIg.:k?]}.HY8/!-*hg?mPefT*^qQ=s@L)Nva@mQZ.GUUk-r/Fx1@]gE!.y9n!oAmYHwbHW2Gz}jLkw3MIY4xIGs>xXf!b!",
    "jOezjUmgir*F5x@vMn8q?9K/F<fH^fWt$s4<IO%I^pc9r04[[%S-miQl}JVSn$)AvNKe9IKT^j>CcI3e98Ug#:Bt$U&f8o%&ao)t",
    "n6hDctPxSOKZ.GPF?L-*hLM9)M)Er@:v$kH.1hk3}yt1FDvixr4-OK-!-9X[-0p@Tq@T@y4fq6]R.M32=/w&O]Eks7.2kj=rq+N=",
    "k=QIvjn]TK).L[^FpFSM}za{hf]Aa@]K#1MN4>$uKq]:-]P>lBNT0:N0j*N<@0P$%tT>Q7BCkE.tVHa6DCm&>aL#m)g-)5/jC]N<",
    "3rMsLUUU0R/E0/nQ(Y]k}67$+Ciu0v<N&L/(<]pxye+^cH+((Y@r*APk6Rp+h9i}m}MMc@oZJk31rAW?&]ki@I0&tFOM.ZI%2HSN",
    "R$2ZfU?yehP0[tQ]Cu:PpVRU>P27-CIg)!18ez:Px5[(-kXK*]3Dx9V98p){:uQ6Cc{[+jW&}}swM?O!V}.QO2+rk8)(#j.U&G7N",
    ")dU6P@KB*IWAVxF+He2S8!-u!sa$@-T!/C&%k]xQbkQW&ojClvR]jPT1AH.f5qR1j[dWZ([#ojlawax3N7h$8u<.]QS3krR29O1f",
    "kQHkd-3p42>Q(tr*qfWIPDliTYL*AQmIj9jatbGmrtHcH:K&ZuaLH*0IsUY4ks@q%Ij$CwPC@tzSBVJ4R.Bq#&{L78fZtP1fW%R}",
    "08.-s?Qho-Omt<sdrsgP@SV0e5[y4pHtyovKLCQNG@M/t7}RW?anTeyjTWl(AUkYw:pTU1SfM[^bgiTdH2&(WguzO2Y-]]HL7J}K",
    "X{iCMQE1fc{-?1DRRS:=].a#<zV30rdm92S51Xx+OMpt85r4?%2::8QvJIpc@!i%dM)w9jsx*G)0Av7M]Cn02R0u<J0pS]HZ(p#I",
    "[vzi7Z<EH(E5Bql94E&0aKv720p3grTM{lSZ+{eCmKvYu5i.wLcoYX*@SwAHjL^k9>A-G?ix5z3}:(=jD2Vx/bFH+5N$4@Ye?J$-",
    "98PLCS9bs%M<tgPFD273P0s}KA<BsXtx*B!l}0-Z)R-cuSYxvT-0df]=)dTm.^a@Vq&2@!yV*df)uGSzyp0a!f6F3M{?2(9y^r(>",
    "6y{aPhDu29Oh]B?SgE(<rSKRZj9!t0=NuW])70fYNWeJr7@}a?x=mjuCOHozU=!2tid0$Q6uEw&z$8mtUlXTJ9/6RLkD%YwpQcxL",
    "Z:CL-{.QXdBDU?2&mXEItet}R>8U6Rx6f38BqtrG!%J0>=$EnUFBaI(VsFOJk+aPhjBA8J2OD:x/6m=H%5-z!-@FT[Rgyeg!AUb&",
    "=kKzV2AN&SGrM4N+[qwpMfPFNPBt3!0arY9u$rI8f]V:UD%>R>p!l!-FL3Uvk1lkIKK@{ksJx.<XC)vdlEiLjZ{L)W/6}4#F8%>E",
    "XN%qNR16IVf2)?5:B5&T)NX8ama*Ol)H{eC!Yv?QU=:+mM)!xotECtiHsW23})>m*!hL%(8sd<Eq5G)d!0S/(9K01ob+1emhhd77",
    "+Y&]%E%r1$n81LrFLH*fw@qp9YQN2a/08ksW0=5&ti7*VER(3%j{HpnY5zWjM]m7uZ/0cOMvWt{04O@1i7?l7Z)ByJaJbu+wcE>7",
    "H(5D3]]HZ/AhA#SLYJzg@EK@f}QHkiPBp1}NiVsRmu5k1TB!v6bgkHnsujI^A[F:L.Z+HCDpUq7BUAeBeQ59%2SN-8]MiZs6XweO",
    "gkmICuxl.Kzv?*<<p4hk<wk(&xA%tQk4Hg%EJaeclFxTDoje2?S(@)NVW>Rf{fnj]Q2(8kdo]!njRa5C*c-u.>Umg5s2G853:p<t",
    "]*yk)}Dr!XMoIPqeg9hq]hv!o@m/<^b0FgNtv0sN9{z(Lar$8M&aiG-xLaRm)!&kh!jv&$:CuiCH/ck@/2)ICF@/dVc]V=w9<yhU",
    "l)WV5g#-+[+UyTv0G7nG-a.g{Sz}&&>hPaFrlP9)&wFXs7Jn6wUAZiz.lu[MxJ]Ty)-zAvy%2U?1OvkTDis^y1sjQysu/+GBEkrS",
    "W4uJ}i8CC{}H20IFf+&v./bcdoWmB!{-gtouMh9<-pAxg93Snk46w*d0q7PJZhI)(nRG-0zCXnmMMXBNaGYcpDlyo(@#v-C26DYO",
    "uVG4cxe4/FsL)1sk$C#UQlzMtbS{#F=lPc$Y#Bf/LkjER]<I2!qr2zPP1f3TFB*aVws76Af-imXQ?uq.iaj>Y+=)zD9cpd6kYLe:",
    ")=AutoO{M#MslM4*fj8hh189>PP}&ak-F7xBX7sKk<-s0dD$&=A]nYqCq2P7g68U:Ff}uXl6ftN+5je>dE6Nr^8@h#k:cb#k+ZOc",
    "42!/i+7LWoh$e]yDqz1j<[RlSXfPOqfFukn&[hU(xs{h^bN7CnWzqb>{n)jnw?&X=ENQC6.2AW.MjrvEsZ4vVL[1c:gJBi][v1#M",
    "mC5}Jbs1yPl6]XRzGE3+=mCqAL-GPz]U{5SUG(&8>}Dj6G!fX@<o7#qcCybaffAA3J+KwmIR6yw=H.r7Jo9HwH^E-DbT]+!KMu8E",
    "nB<)nZZanWeh!IiOgy!RzByS<D@H-(:x#G>lq$RiKM)SAXsCa.5heWG*sWSkqjUiWS!SyzVi8t^OAM3Gj{*R?D:GwaoC&^NkCi]$",
    "!!um>=#Q?MS.o#/x.f>sG@R<IXzdh2Nwn(Sy<^XBLEwlu02-hTr^HmG04x=^[}MUDA)>HX@0+FC=sJLfjBZ>OehO$xWSFAgasl]#",
    "IdP{Or@+0t^Sq#.I}WIW*-p=aOWbvYX{Z+zix.7tAHaOSgY@p(Ps2R4QO4@Tk%&x1zb0U!aw[Mu2eO2prAf5-QLMT$cvS(@mE+68",
    "L:U]wC^b*-y@yfNr8HX02+*T#hEqh5S82chtf8vWt^!4Np@9S74b6-90igi6c}cr.ig)&D-igTNewd(z/LX}}cb+:9?ya=9w>a^B",
    "IvIBa.1#nqeycC)X*X)=S#@g>3}Mh/0qFvZX>OeEcSwWh8AG=sANtmW^0{?-oxjWpaAKOBQlja(5&]u)wdJ(E[yExF]{eg.^MQ<F",
    "?):An*<5F8VwbFAoCYX4LMG5>O-{qvOI)Yn)F<^Y@h5<B+PD)H@^GI.(8EioYi]kmT9.rrj1s7t}v(aRab?%s}$7mVVr)F)v.oZ]",
    "Ts7kn<Zl$dL1Km/PBieiTA>@3P.^Uf0k37W-)hE+5mjtdc)mM.0mwnrayncZ4^}GM[g45yZ[EX*{Ye4wau4-Z+*%Rz6Dx3AtE/bB",
    "@!/8!Fop=LfLB1>n!NmKeZrB>CX0qTgNnAZOE!Vj3(?(6fNiWZd=]%e&/}!M>)*9ASlrdzKe]s{0&U#V*Uj3*c>E7Q5@rE02bn1U",
    "rVTT&9Tz}Z.5A$G1N]OS[W:*]C-}s7-ktS?W!wN>hBwrBj@%!<[^1kt3+LpWkXK&C87svP0}dwF5elcGZ]HwEim2BPgK*4LU*rOr",
    "nBeYchSxt3Vcbkz)KqtM=nxh$pk5wQa#^Oav/50@xNtR0H:Wye-0LY(oIEl8/VY?$H)0RdF)%A%cpM!8-00h9Q&gSzaH^lun(.!5",
    ":qHEfwzFls?NwRgSEp}->4]:=>f4eALuV{d<+UoGt@Cu&t7yC6QCM(4D=VpMz@=yi**.M(D)7Wp5A)&w152aDtSdv3dKhC=c^SHv",
    "{p3WdkR}PnOoWQ/kPnlo[A:muNBwb$>Jf(G09*[+rh+sd&2Ck1oP?AHuldt8clK-#U>kompfZu6kS-dh&FI-<@]%6/6=E$5]Mu/?",
    "1V}Va1Sc^zS@ZIUiBf[w++r]Rm-O3jdOWxm-8cctFY*qt+Y1H-4=a7@Si444D)ybNUPn#>>j$yT[*y[fN5!d/L.*?LCW8(mzBQ[A",
    "{=G(^oAS%h7ejwlCUWUs[sI5v0U7*os})HulzHP]qDo8E)OcY{Qbr3qM7t}fv({x$@j!c5KWuK?/6+l89c9NyHe1Ba-luT)Q]^AQ",
    "!9Wb{iok1><nl3[+FH5rINZL!Le$Zcg7A9NQ?L>5W:KJQCQy!JL$/%}cdAsoy[N%{m.b0KFknUDyf/mJL[:2bmI2scN7&:qhNR0%",
    "IhFyFU:*OkXA0C0WY(Tx{-ZUu>Ekj&V]@}5tCYfHeC=0Ly7gAZ:o]Bu+bp&A>Bmh?3Q8Cwdj0fKK0IrI[AwdW0eft[WdXaBC:q2^",
    "0lRQT{gStDE8Sh2@gEHZ/YT-%YSoHAV$02$[v?&m&9/D{mTi0qo.8ZS:cH)iQVvJIO+K0.wQaRC*01GHoHPPceRNW$dF#9q09A.[",
    "YkY1dKvaVlD%<H#A9-#otsycSYrFt0sNo0I[8Y&$+y}#vi7]5&@1S}VjMo0%<.2=18g4Jf>OrXKfikFxU82+lFiE9#CNi&LU6(*6",
    "WtH!y:9vRD4-lpIUZID[!NHH3Nt4a[9iWgwnPS3qb(P>*TODY%K=C5S^VnAihiUWo)B^://>C<#i<YtVW}g<*!NKI2rSSZ[@j:WA",
    "zHP$@!&&MpTqrcB@lShwKJ5UhXxp+-fUHHH7}A^jinAr404w4lKMrMcLbt.4{]??6Xpk2LUUN}PYf#$tC5[8b)kA50Pc]NZd>BgQ",
    "v$%J^ZX3wC)Uz)%ib8OWKs*)PntgNDBs>$PmcrZDr0tO2[+2t+Vy-s6H5*2xpUuWK1JbP1V}}=aU^tE+j(fy8UQfT/Dqbwwe2?:A",
    "}umS$LYz1icF2y:FlH[]TVqh0Se-5-E<QF]2!l]$wzWy1^Kv4oxt^q5m!?LCdMT*%pSW&z)u&I*k3U}o1ayV(B=i@C[V-!tO3iL?",
    "{JV5q<)yTi)Kzs3JEh0]}H>PR(o?8Pu<Y0d/2to+2+nW[YV:6.k&Fa]YOJdxL^NTbDzf(x0k.dqavZgXFut(-qVb%@6MoRSRI&j5",
    "v+[A8@s{UxTuZwYP9Pv2InL)$PHe4UB25!uXSH+[WAcoHxvp1$CQyl&aC=KJHfub>7krL/mqW]CxO>(=YGcP/J[w.wb%?gq-vNim",
    "/kIA?SN/V-1:x/bN.g2ZJrC[rYSK37hN(O$yPY&:4*fv8knCIcnkqT/@Aw%fNT*hUXfg(pxa+G+4:00Xsj0)gx<5qoxY8E@V=@eb",
    "8T%:?w1AcMyO[zJ<@1Z{Y4Rwx!Vatp}xX[J+Qk-)<uxe0=NiLxSA}nxX}vZX3t]/bAnr(Adwf^bSrDCrS7cl*mYUc77Q5&.xynFT",
    "E?J?^+oZ*-a3dAQp4]Txh(#77jz<7vO>^krp0:wmut.bQjR!NxYgF{l{V<j(9rOQuORTF.r:qJoj&.uLZPbKwfcv8QbSos1l.x:X",
    "4CwlQ4K=(>uwZzJha3$2)]3E9WzSV>i&3jF?VLgrxiLT9&jy)g{V[zmZW:VO=?PdRT4G(:+IXC{FJ:pMZMoUNJ=XC!NNlIT^EZIO",
    "o@U/?mLI2edVdK@F>dyP^<eFqS^Zf@HqK29)S#o8n1:/:vtL7IUpW/dUw+2!I2#bsp#@C94gEx[sS?E)8.F]>0<*@J9gIU}?cwO@",
    "z%%}{-KI%j)6otGGeC8wVn9Vsf[5jGQ5!7OyUjOfFq$^qkio8-@:2&A{EcO%vOgdA0m?1X-afE]sU&*!/3M3j4(DZq%fx6}]azLE",
    "0CMhX1MIUU0P.8fKA06tPAa4nUWDRb/lsnU>[=>7v8ctr{rTpk>x)!8wZ7rw3KI!QPJP5[=WI0sMw5LSGb8Cvx->9yg%.pF0*whI",
    "+<CJ%jqge$bY!QLREVz=8OSZzL!kLX}/{dA+1Tcx:5Cyql<R11.DzdF7K0B76v2@d>2o)BpxcXBC61zkoe%5!eF/>G<e9c$r(<2T",
    "h?hl2kb2kAX+JP%zFvFi}*POG=Xoa%hkN=CUV&bi=3[-!&]D*%YtgzWad}c0Usavl[n6Hpy]FX{Tzay)i<Uf*GBY?4@E+qq4Qjs{",
    "i?lWyt{tVHjTc:!8uL{Z}@Avy!NPpQf%{+&wk{Gi2pj5$Tydj8H^RoUd?sl@jzNLNm?E#1&)naFW3ha=Aj<xc8{miOnJn]FNLBE#",
    "H>y^M2^yu[(43/qA:.[ZLEm.W01eXoNwu{#:KyCV*@78A7)h6^:T=[{-zLTdzO/bYHGVw}t/ovJ=.dIRYA7[]dLDDsaEVO17l9S8",
    "t.Vy}w]VA:rko@%xA(EsZeWLnt8U<kbWmAnS?mhw{w75/mh5-=t()IWhLU</wm5rIEDKY@rVE7hco}w(EBwA+<v^oKHVHrR62i!E",
    "paJ23gQ)s@bR}>Mzv=w5QvTNj-::P:RTnj:W>zLYogbmlFPQ9sJ.$:0ee.^ehOWH1T^*.4lqO37@IY3A:#%P9VoDWGUgLIzp6Xty",
    "OaZnOKOL.pU4vk?SUav9F+z5/WZ<TDda1XI&wg*8wSG}Su0Yd$h!h*2?nb)ImAEnE*L8fhS1jA)=qLH)ZHwlFm]XOk6/&DY??EYn",
    "odc-5eItgf4xvxg7}xQWL0CQ5z(ypZz+(52bk-]OEkX)PaLoENL[wFUIRa]wGO*UYjZ:bX<Tqck[zYA<.H@088ldt-Csj.XrCwi(",
    "v>N@0Sndzpm^&ENx]A@]ja2#1A/z]<vRwa)cU-9mQom9K&i?7E7tsZhZkH8@@77lWc{e0NruwIhmwK.ldzZu{J4gHlODcTyw)^++",
    "t*=P=soH#OKIVZ:BLU}tmS?$IkQ?EIS7(V)&^%8504V2HjHG]b4S:i=>UwH#p:$M&&f%B6oP^7M0p-y&/SueS/CF3XkkYukvI#Hn",
    "4=3]](c]@40*3*7mS9N!kQK]wKBmZ8p#$S&lBxiBIJS}iHo-T1R.*xE<{qTlhvD!7LO+*Zr5+Ydk1-dAKMuOUZb9sk<O{[$45Td&",
    "-5]fR4R[>OV4dblu5zDx}qJ<qF6J:iW+@{PLjkoX[i8aY0.2O)t73W<PQ)QDZ?cOf=$dEof6WR]-UGSf>U8n}c}maGvGN97S1q9Q",
    "armn(Y.hYD2(afT7-#*x[JL^y?4GcuE5^:(OB3M@uR%fOU3>Z4!:dPCw:z0ix-t?iR}1E8WY(*E!nkvO^Su.F/N.kW111t?.wi>I",
    "LN<oFDpk=!)pYXqkW)LNY$V5hXide.FY0Hi:K0#4ZSAlme4NS5oMLhqAfHQxjjD8J^dtI@eCTL8[h2$^6BrB8g/}A94RywTga}PJ",
    "4<TfHwj5U}mm@U9Ov]]OuUy}6KBET4Xm)#*)Fro^OkJSRSJ*Idk1kaT4IYTvSXbFV+W<+/Z:Nq!WTSM^E.Ofj:IzS9s{/8W>?wM&",
    "lnSR{=}HVbGAX2i!cnHUxV8)zjD2gZPT4zbgk#WQBefKn.XZUkUl7=tU$CLvFfCMsUgdv6V(6PxDmRy@.p{w3>UJhj7q(&X^d-kU",
    "De?@tcsLg-bpepjopt{@4QteX{8*&VTSv[+6DZZCYE{(XAk-hG*Aj9<g!P^w1G!Z*1Y>(v7t@N:g}>@:dWPOp@d..}tdLS:IM7>+",
    "rVrqFR&ctAD<WqC=9Nvj=j>ihYl/(c.cFO8GfHwtMUk%7^fAAgm[YWx!uQmjkEw6![bJRQD}zw)KJz=il&g*^Szb3fucn>Q}G]ka",
    "WcbfqcX%N)Ju9<tJ]+2&@H2&kDhJfan/(o(52%]?kx2:Gga30=HXux:c:e]JQ!#/U+K#KZuY0QLODlw!?Iz>e/Eg4s?4)E-nu=I^",
    "ej:zqR6fkgL.&)kUNO<>Glwa@yPstD(95gxprLWZQZmb^uFo$zW&ggr!Bp(0<.#9Ks-L4!@ou$&.2Tx%4WjB!WAodDq1s}aT/arB",
    "j754LUAsV97v8gs*{>V]g7aZUa(fE*fM3u(YK+-&*ghXEHAs*Qr0XszZu!=ZZ3GP09hr{9Ff+dT]u>6iT7[v)P06=Xv:bm{CM%*8",
    "u@dfz4}bTR}0Q2M0aRk7Fm}V&VaB*n{b:$xZA%{:7Kme^gIybVXh>G<A}(T#Kvn(S1u8=kekW7{kDp?GlOW1J1yAooA8<IkYUx}0",
    "dTs]P@(.SiFwJ3+m>D12TQ:qMMFY&#M>QTRif<Rgovp8V7ajX%exL/z+G$=6+Q#vGkwMJFndE?%TN)pxC0]}a6E]>vXnL&<V#}P%",
    "OG?Yf4R)l1fF.g>Iprb4=2/jlPtEbhL:iGc(LJ./C]F(WxEN^YPCSi[7.hOtrp}oCL}9Ok0nOm2s&#GkJWS^FMzw)>xVF99V7==Z",
    "y?nEql@vg4B2KrpAR$TbTpkI2EwkvTtc*:-KD{[JrX1tpunZxG951Cch/Y3+8n.U5wfh7vSplW$v*HdtAWz>(ix0^0fORc[{xNGT",
    "pEqSQA=0%6lL4)lwsr(&V$69G9cRSKoJH8+/ojd!2rSpaf3VL)2V7vYeDpBrQrhJgZpw%i8Wt%#}wZ$G-#S>%BL4i<.fX%w6OWX[",
    "Q5EA)cfH4DKvRS1Wp*(26a6gM93r3XOd5Pn&Cf2&yWcNptdeiglij<nuEM2tao.RPDn.@<{:8H}-j^zedk$8q*x4EvRNVYvD6]X!",
    "eC0yo-zDX1XtVC>NS8d*7X9}TX.0DoI{Isn6@cQHu<N/pp8M[)Q.K@+S0?X140pdn+]@0e&dQ>*)mK6ohd{z.9ljg9nifNg9g@==",
    "SQJtu(eafUcS9pDsE5Ct!}1M1*jqVa1x%CvFdiFZd3a9=/Ve@:N2fiNZ!D8!nxA^7@]wmp!vdlFKsj+PVG%XqZ-3v*4O7-Sb^J&K",
    "mVSY@sP)booxsR=1yZjKoRw4w2j]8Z.S(IjEW+%NVynu*zZpr9OxQoio9Wam.C1D6}rqOalV=5qk<<W%XU$GK/rCE[&VSe55Cs*v",
    "r4hm?=s@YXPMh6^ikHFm+6H+mDfquzzy[-1LYvH5Eg(=F8f6%^B*1=VuFH5{P-5=x<4Z*.mw1&?aD{/VD25UTz9@83*^A[YlxdOJ",
    "n9jOJB^*<{7%{jmn2n${i&:8Z85qzHRfw?0B@1/a/JOkpFo$Un5j46{xNhN1PY?SMaOXX{arJ0{/P!Cg9:{Ljbi94A:NtJW80BTR",
    "[q0@9k!??i3)6M9*7aF6uRhC8Bvvp^N}E?%Fvn-DyTVAIKkbx4kPc@wDSup]Npr&)[((:w}vMrksT9ZQC+GyCr#lO8uipr{yv.u!",
    "]n*jvXu%k*8F#olZ5dgW8$1Ueh79TY6!2S!aQU+y?)qDd:p.rkv6C0>8JSfcF+zk<Xvi-%3rey!DI06crvYW=WZF/vXDs6u-s^O.",
    "l#1/%u5%X6MIxEMg%q<Zvz+Y3Uqx=Nx2txFnb[c7MqkZ*ye%67H*HlFx<V4#WUkU:",
);

                const ENCODED_SMALL_FC64_M5_U2: &str = concat!(
    ")/rbCJ%VBZX2Vjo?1pCa}#y$8rT>$uu$m6^Ai-APMyk?bpc=DP?:RxBF:(k]X*vOA6CgTmaJ--SnDl<@Fga+/^ncb)ukIfP5$n!Z",
    "G&VtSC:iA+<KZ>7(Gi/l7domUBfl@Pxg^[v2{*SQNkeCrbiZjkAiY%qFS3Lw2G.V.:TAxpG7^{iJfe3vog(g@ns5f%P8T!.DWXn{",
    "5{zFY{?!RC2I1WALxsvG0)N5{ghUmGateKmKzTZ7fMG?0kMJ:Pk^n2o{[>!*k/7e@sow1Psowoh&m5r3MBsE&l]=*iS}h&C]835k",
    "BEyp=0S76Pf.xAJ6^fM(nXFDzlcb6HNv?77]I@MpT%Ri5<qz@2{/p[rPS4g*QA#!JGGojH/c)^:EqoMVNWHYHFeynT&n%mqAg66R",
    "55(QLbsEbC20&*lVRpInF2aQcy+^9Cg[%DLS{VJy9WzDkD)V$Ewfqdu6GJlTtCx3GLWo]]0VX^mvFD--&J#$8UxMi]Bmu0r>6&ZY",
    "kX#uw/6FTmE(Id9USYh5g^^S[LX{&xfU(!.D[zLnif3RAuC+Nj^bZSUp%<9(PLwFW!}Z[3-bcHc9=4rfS>s1#@^6yl^d2szoS4wz",
    "Jf5PNAm?wcXfdOgqRxa77*Lm$V%?X4?3Ba@/.UY-J3+yl(Nh}Hy:PPMEtZ6FDIOjp+6n=RKu]5yNWxB8w6gDcr/x$>mz*^)}?]8I",
    "zuOfD6S5dH4OrFffNG0Dfr)e[gM9qqg)dR%6Z(Hz6QR&Z5idV)k:#<S([tF7i.j+kyB6Rc<[gO%+[Axb7OIeG=T^UK=&>PcRaZes",
    "=NSo:k3K)D{:Z}U(4^5p.D6BCTzG<UT@g6H<M[iSR0CoH8Rz<tSeVwwB=G]{uk8D:=%R%GYX?.Z:FgQQ7/6JZ-:eg00=4S@c]v<m",
    "qFK0gTAwD{<tJ{HqAi1o^az480qFGmHfBF/:(d}pq1*+H=:gf}h0*n!2!Z$*C7eWrL=P)d3>x{ksfOeSs>Asf+.9C?=}vDME7OHN",
    ")WKS$5(n?E/VEzYWg4=5UWwtikdVP:BpDKee{lhq}JWa7=9HUrzGg}8.[:?=a)Hpn+*]9zrcDf)oOr^pGBGR0!*[ew!iFL$gd*s=",
    "r7oRCHHE2x<32*]MCf/wV).q8H!Cs):>rjFE75ESKWsQ.]z$WGsmXF!XT4+fCm(:Df@bS0G}KajgHl#[RI6:*u@Bo(Z=Q)BKC7cR",
    "5z2@a:u3<]tYu(fSqs$9+^y29><tYtbPFW8Fys[t1PBuy=H^[}nqw#:waRv!L?<o>{2t6%-}{ujY>(ciYgl.Lf%9^RyWtm/ORD^I",
    "0C^Xd]XnJwsFU(c>TBZF8sH&{p-#q)0Y}R<EmFrwk&rwvZ/yg)7(a}JaLx*Uuc+&00g#0V6i8xLu&IaUv#p&hpsB[Wv7RdQsvuYk",
    "t[yZi3Xd3EEeeHXR>]x!uN/9<j(Gg1CTfD^ocUusW0/$TMcUgF)Px#.^*Y6!?9}xec4aSlJH.Ml(rC-2Lq+wkI/Vu>rn^aa<@MU1",
    ">^4PVWaRAw>:Dd$sFLk@-g.p^A7$k-cr[wJ@4G=!epUG0t?40hQ]%kJ1pLn[re<Kl-2Sv3nlRluB7q^B!/5r/X)vUlV&^o<@DAF2",
    "*#ad8ihoqkwFNY$>LDdgo/0esICoILBL7T$&2S7=au4[or8:q<rE[{GwR%$YC13%A^1S<VoB7qPFn{T.qSyqf%0V45TZ@iWt{V[&",
    "%g-&(*@tX]fMW:^5opc3:NN({^nmA^{CVRCMuiqzB7(KR?vhe3592W:lYKt]9.#^G!<pAwyR=<WLoqRA@Mc>f-ykRzvW/!Hj.)c)",
    "Ehaj}.B+d#K7zk5y9(iUg}w5#hMs5Dp.!R}@3(S#JlxIW&7RiNSo{rwAd89a5Cf^4ej5yhFrr>{d7.P8KpxTK]BcBpKq=>pcwP:S",
    "d$Sq=SBWqsr9-(}s=Jkx)oLvT0Y1(hbMC3ZTe%mMCQ3+iU6]/MM@?c![lx&[cjN<QGOF$V}FmPANsnLRi2FUV0a0Pc:BF=NOjx%1",
    "BXk7+U9{g&:G-y<HZXh-mpV}vxVnyZ0d+c.cDaE#MPP/ibYBZo.p59w4RlJ<i*Bx(y/hfm7eng2HH<8>Ffi#q]@![s6vdAcTIp<J",
    "J5I5cTIwBi^/P+pGnqz0lDwaISs4!4f.*}%4j-$[fR{>A:I:Jf0doL-y^x}(fV<R-{%@wb0bJDlV]r4?{!ws.Z<?PZ^8Vc#6N]C.",
    "tPP]U[m?bIL&hRQPX5=#SE%7>rs!7LLtwW4Oi9<!)zTQ?>>bL3BV^5]{+Yor-s96=Zg39o:wn)1l{g7OW)O[jB?]<%ruCOY5&Nbg",
    "Dw!dNQ^!6w0eQ&&m4C?>@u8P.cp!fKuzHibsi%yHzy9H?GW6#?tRx^7Htga?i!&c%TcGIAE@pN=m>Bhz+ShdQ5nAo1t0:b<6jNH}",
    "7Mui]Ii)o^d9yBgdf<s}uOqb!FMjqndhiZ:kqT#Q[3:{.PcS^pAj&Ft=W86y7(fp$06OGoV%fDw=@Vt(*g(LVi9L=l>PogQD#/q3",
    "07mFX07![aLOF[={9^5RH(f-}09bNM(r&?&Xnp(Lr@pmSF)G#><7>FOBpVFRI((M-t-1?hiJWOt1+W=bX83X?5BGk9FAu}%]Y<#u",
    "uU4LCAkY]j3*lfQpGKwQCDcqeazNSl3<mS+]IBWk2{HfG]1B4@Xee^vacZJNw@Qy=qUMJ>k<3JS:xE1Puo9tBSx*J#/$lQ:Z8KI]",
    "ML(8L61dx[/>Yk5r*RV97J)IQ3G[-U@r=*[MdT>Pd=pGAELwC#2]SrDmELxjCD9Zq&}OA{2nHjK!Fb!Zv9Vx%FD1]ED]}l?sHA]D",
    "BuZTaTM)Tn[q>&c0]p=cH$Oui>R.P/aPk!jL8(PQ{$7A^)QSpWk*^}vtH%PMh=?^$?>vG>D3V)iT^Op:AjI%YhnJoOl50.1Dk{Rg",
    "-eJ5aX])RTD>WR+9fVgwz:KVwRrD)KsgLs]t8qVgx7dihKFRAUp%d#+7u.2E%l4zn5fnG#GuazlDm/cdjKJKU0g(?AvOb@IPFR8g",
    "FoSd#Lw4$H+{(-bO?<&?xm*o14qIozcA)!t?wA64KxRT@TJA1QjTxyc<P:9}0yR*U/@0cfOQ(XV/O2IMPw(.fX<9Wp}d3[EhZUoj",
    "TsYHDkOW>y&.DymI2Qpm[x.&Z-m%s=u$WG>?ET4b/Cp{dLXv-[0iT5H([Q^*r20S>1Hny#7:7y[[O(MjSnd]@Tz6vJbh(Xrx?iY^",
    "to[OVO<MvsdUlCt8iaIRB&s?Og=r3hwn4@9:FL4Y?g6Sj8+zD>x=g:34m:QoEw$/irl^2c&kmZ<F]4B1-l/kW19%WPCNj5S6DpF]",
    "rYFTyk(V.PFx%$55w}CW}j4BGLSoI[XHTz^+Xovbgyhjq(lDW1F3aU?k8P}nD#I8e+SRJh!tgc$ZZ[)Rew.Xb2A3nV/6/v%Q!m8?",
    "J(1&hOphnrsbd!WgoKA!*^.=ol#Jn):r@QN=Cxs&aCq47ij.jnulF{OSl+i3bSSrD5v0uuKzt-DC&DI3M#5SZ+cwv/bZrlJLGJ+=",
    "AY{iZ3#w*9REXHD5iFET}wOoPLlm^E/jXUSW<N1}8W4lDZ:/e(Fh0z8Qvtw1^DUj7jEO4t>cr*Rucrt:lpIZ7tiQV5.E}=tZ(J@S",
    "gupkNh/z3@1(Aj>7o-.h.{s^+Hg*7<^4uB.g$V>jN*}IyI(n9?3oF:hk(z!raPxkw?n==:VDgVZe(a4jJLoevdK5%#5-szqaagSk",
    ">-z2)c(dJIo{D..*$4cvsMtr6?o^W=JC<oW!fB=LCTiigu]A>N0(}sOX)KR-)eFO0Kz7u3XntecQ:cBksu0#Snm7L[aD:b@PO^T2",
    ">Ml2)ex!nXFBfZ8&pLa6p:KKL(3tw7M!i}9K+0:5NQ:8]q{R<k=H2nTvY5GPRrejJU>-l2Z#uxF44*xWb>l%Vtdvbq>+-pj!a/ly",
    "u{%7?PH{ipXm0H)oF:%S(6H%6V?q>Y8]EB[Dhh3:3k1z]J[e={/Rgdmc{kCXjBpu$qy*ngjk96nvMnV$@ahke-[sfKui%]J?3c9S",
    "e(lyG8*daN1YZZGFoPeH@UIU0m5D[CutwrfuUNnrzX9U$35n5=9GB@1<cMu24cm8{[hj1PHqyU*R0p@yNIRUX^aA6CTCuQmFzu>W",
    "XrntP0k{.9<wsZD:r3m40k$G<d1M#..1KrXbDvO7*SLZyRxRhqne-47))kL<nAph[bl/%qB}^AVZI/BK?gzihF!^W^2*VtX/U9AW",
    ">H#j=+&zd8nFpkHuPL3%&L(Cxw]Ei55?-Z/H*Wezg$:@NwETtQDwY*9cRtX$ZbezzFnlR%41^R8+.E(nIlbZHq4nh)qeo5)(3^wh",
    "/[{PW]fm^?:*w7YEsFiePbfsFRoAnWj&6&P(8Tf+@G7qGU*D#Bz!0N%wnjg?D7qZ+b1[E<Y!y9-]+$JTd.s-PfDvaai6S!zV<n}g",
    "&c2pqfGWv?*-+dWap)RDS9NSW.U{TJC7tZ*ti&GcvYnF6^c%Q&vd+CKam]v[3Q{)QiAQso:}v/vk&lJCofr>K6vZUS@AWjLCmI+T",
    "i^7RsE3IDf0>2XO?U3DM(.W}1@}H&N!Y+rYZ7G$GsV1dhaDY.*m0m(WZ#G]c]Qc1G3LR.^>NJkcvBe)Now)v)SnAT#XlrTX3n4)-",
    "TGdV2&BELN]w!jX7{6-2A4>21{C(0W<A>5(uD3D*vX6X=9L962cD<=GbB0.(0o/z8JiFBc=h#20/$pC2Nqaj=nX$V}@$IQ#JGTHV",
    ")6--NfZKXf7#g(GkUc<CxX7{2^e0P5Ac9>GPF{)NJnvZJ4^%0MH>TZ0jxxwk@46S!POk(#&2j4c(Ds/z*s-?%B=A^)tWJi1!y76Q",
    "3A})>(iCqwrY?Ruj}+E*Zzjb?XcY7CFlq6G2NcLS8jUPT:NT4dSKe?w{[urMYA8@:hQ8flFdHT25nFU!DER(acl@k$Wnl=:Zyo%3",
    ".XWTfsIh16S=2vg+>T0b4(5L%rU^XUb/PO)0k!3m4Xltc)&o3MtoA*S*V}*4ZtYHk4->FfyV]a*]tP+3?#K4@4:KZW^V!Sj^Dw!q",
    "a+#pAW*N*Kl-D&)ppOqb0nf?64.H*=&n.jb<BK0/kNl?({}T^g0eQ][p{Eq*USG=^C?u9hUTD#6q0Hea.0Ljn04#p6J3rqQAxQG/",
    "RQ]y)k}^=44f>3tu-x)n*%+2((+9V*dd4)o?.W{sIX8apG5rDt@:P7]{YyYlU(j78(T-2*m4j&NPIO9wjuXlVk)j.ZhI*]/fA[Ty",
    "1GO(y){Z{.TI.52eG>uY&s$=B+gT0.I1g)pgPo7>Xb5XfF^jPQHRFv50y^mbjpRcIKc<Dws9c8WNm*e^4zad7JOW{N.<uv%Q+YV$",
    "&u7@X(u->57MvcjF@S4eR6uviJlcZ=sT6iDcU4S?*$wF!ks#hAQrn-:olE*d23cq?7v7I8S^rX/QC{(tg>##lCthg6zNfQkV/tVi",
    "l<CGYke7.{!caF*92LnhtK%a8B${V]J}sBcZ/?ym)?AsCMVC<mtx3pI4oqaBp>K[y(ZD9.)fAnCEC}a^07TSlFPmhy7KrbD2kHbt",
    "?>K%JYh$v-JJ[)mUe&Dl9%FG8]7L[[[bCCZ{GsMgZEU0hK.K%i0%@{g?/bZheltn6w*/n}D}Ik?j0/{3+o.tCe(F938?TV]D{7MB",
    "t:drl3BrytfL-NfA!^Pcx-p=ut90:set&(&2Q0o-8Bwp89m^o!KA%U+xI-lM]I-iir-{h4mMq{jKE.a?JTE0[bBVUohr+Z/hmfey",
    "ZsnK-s+JJ-k1tSp!88gVFw5a#d-a8cwr:F+q$3ktuDX8qDFUeQjL.0!D+{<*E&v67uSEoyyd}2VXLE(xFe/cme4$df=aVXi>s4+L",
    ")Hbe&J>^UrvB.LBn}.-eFVV<NgA<s)w$xTI(b$&@N#>!bvYoL}Q]}RGV!MY!xs=zMb]$P2lqTz<8YA6E?XlbQ?TIKG[DC<PGjL7$",
    "GobgV!=$+gc@Uf1orl7CuB-OuFmE6ufXo4kiYFU7Al1S:H8&s?48<KbZ)>9YoS]T7fI6{hg6xCFdcGwGJTEwK&oB%*2^i[+9Zq:2",
    "i7cUdOk/3(1@raWSP!%%9^OtYsZ0mrEvL&4Qtju.2dmn>E/*z0j76i{-jeKPa5FQ]&0)^(cJ7%0PZ7*Q88k:qNEE4oUAYWkDp.$/",
    "3)fjF^&bH9z>3O=OZOrqjC>TL^9A&GW2Q+w]dRb$dH5*ne4M#&iw(yLK^=UvVY!$m5yRJhQ+&@LSdG@w^#FjTvp1NGQWYcY0*j2b",
    "SlUWXx0#l:{Ib#cIMF8p9FU$90D<WwUA]d0ZAO3sat]]SyXZVrfN[WrJU}*XNw#ogIh.$V.69$=nsVXy&nmQ1M@]@[Kpn#&ZnK9b",
    "v#[ppSf(kK[)ELx<)-U>Z/Vc1Xoq1roOg^cOACC^5n^Pz@3k</8N0$>/IqKY>!wpO4dvy:IEE^zZ$gY.0o2fn9V+RKZ)/5LPTJJp",
    "^JEtVSEW8]qff&wrjn$]].@C!%4^q(z2xFXWw=$^H[%jD<EYdg6]msMRZOYiX/5sF99KjzeAB@q[[&OmrJ5Q8qJMByKOBQAh-x?Z",
    "@Tk+<juI-tvj@3Mp$!WUD(1p%V<^2H[^g@5&+^19goc[UzahAbFoH1!8v-f0A+wol8OgZsVA]MOOXHQS}et^!t-YjPXYuM<Q*Yg}",
    "+kcGG9km7?R1<NCn)$mOnidCGOkcD9C*5Z(ucsw8p0EYvT+cO/vqua.pejr[L!JUK2&NnD)3TRj/9wm*/0]xR/ZWGP.IBmpIM<7)",
    "IE2}?FXpqjmP0.XsJtLE}hS@/^-@N>9[3SzcTB.QrbV2akT#IKUs8Whrg:Sl7E1VUXsJ=nYAkvLzpa$t?r(x0Y@ktN]E5?ncOS#Y",
    "iJvc-Uv22UrN!us(0?*#edTf[pbWvhskKL6ca:Y4tD.tDrpg2Q3*wtsEIx4J=Pg..FDH!x{5GuK*{r.s2}b{tb*tC=jBg0>L-Mws",
    "E8nDt[%poemqhb46/I^I0>(ogy7-D?7I.+&5R[=FfZEgIq]8[RID&@>OXH:V>AzP6F(%tk!VkPg{)!czpkQnXk>{H9s21Wdg[jGU",
    "oz5]X/Wzcf@+&N!sQrtqYP72@PQQ?MPG>6vn]7W{G*>dqDw{H>{3w2c68cyW5SgA?S1rG6WOeM[U8L0E}a{coU=oVCFLe0/ihw2/",
    "bExi?>2?Y1PXh@Jup7}Y5y)ahI83%6OVxs=fb%-1-C[jdeCYN+&(x9Mt6amrakYk{Fsd&FHjJsrX{/n{pu9?q3^6y5m05KB:+#iz",
    "*E2qYjP!XG!?8zoO>[)&J+6uE]Mb:I&/lNx>xWL2bYnHND%g6{^?#dHKAAGBsNg:RP>N<#TO1hfmqI80.CwGvDA?MR0q0>p)4&D0",
    "Tkso0i0{RsEtK{!dNa@/b8Ankf4*PCC=FRAeN>/}D/w)aj@n62li.UF.L)1rS^4@oG8>m4Ysu+jAUSBOL4}zs:JiNd]r3!iiWj+(",
    "M6@@i=[>tS}(N7=[(HyRo4K6S1&%t3/MwQR3h@vpg7mf%u[2+KcqN-cxJug*8U!f-Z%s)LsFjfSAj)74FCb3!H+d/7-8ho5d0=4X",
    "wc)d/hFEfB[=?U0X^Qj9wq]&/bbE]B?%K#qL{xFCGVE07k?!dWZB6TS.)xS.461S!ns-<Djbceoj8}k-U=>:Md{jO>K%p8[jOZD<",
    "UcZ8J!H)MgDgIUi&.zA@dfAvgQ[ms]feLWMZwCBOtX0d+sFQ455zs(-km@Af+:t9q(Zoxu&kog<YreGN1FeTTUE}iQnK30lyCdn6",
    "HUFy:fDBoe}9wYevD6O!m^q]@A9Qw{9=((@9158$M:NWz!>K#mPC[4#pjkn?Y.Lh:.Ti2>sP{SCfuTzXo(s&jZbFFA7GU{$O}iMF",
    "g-<1/y?R=<zA<@g^IeB*=lVJ9D>Ndc&g01@LC3#1Y@K<>E*$6[d)KVVn-[jfD)ef%4HGm3lO7:wk=]J0D-m7WS@4Aj5FZ!H1Aws[",
    "[I#m^=WPT(CD!Cr8hCKZG+Jl8djD2%HC!uWpj[kj.4K-3nKjT!KDkt+fD$B069KJj1qpe!TbE]X0pTTl:4$s2rSJ$0P&kU*nWTQy",
    "o:}Q%SL]%]YrI:T{#X3VKNj14l4f+pOtv&4(hL25[O@9Nx}Y(qk9eEIGYsvx8J1^mR$6)KoGFm#(sQKtV)uE4j893Or:{Fvj9.^V",
    "k)0jZ[aH-+)s!oGA{ki<eevtiyop5>=[1+4FeRm#uXWn$QV+aXK$-fnmOr!B3!ZVQ7x:.5@[iw%kH5{=pnTw(@SZ[pBnm1AD9Kbd",
    "[cbk]nM=9VbnO=)0dZI2)#TY^pq.@M6hs@W+y3iKxOjb-@GM6JLT]8[>e&p<eLHJ9Zb[Oyw{@t}xP&Fi{@Ag[sc@5jIsuIskUqA$",
    "u@on82LnV[403FO@2p+ZU=+S5JXI[BbYuprj:@>:Puny)DCoOE=z&]&jTR:y3/dk.8kOooB&]+efK9W4yWg=gU@VnRz(zRTl@GWw",
    "xLX}S*#1[>6x-T+7XVv(B]CmK[b-qFObwHns]CcVl-quCXvLZ8:4IaI@bx([QK-@<52s}yhlbN$1uPgr2M!+Pwd!7til1t0y9v<^",
    "05$L4&5Hf/{?s$p!q0Lc@mBSQp*tXqavW0Rk-]J&]2Tr:6U=Mu^7G}5R0#{Fquh8dqIf!t!}wN!Pd19Yb<sqMt!l=C&JPsDBwTf{",
    ":ZA322h3pDZGrY:v2uT2F88J/@LZ-Wbl!DrLM)eW65ekQYmCt[!7:r6e@+?7t+$aT-bXO^I7$17qqGkP5LV:${+X2Bqh=RZkS9ps",
    ":jtz#lOe}+JsETaz4-ORy*]j+{9)<x^=9HaF<M$hk+xyqGa4*}]*Y6EqV}RmJEySp8ZS2ddg+pHCMrK.7tKy5ERjK4N=0.I/$6{w",
    "9FgYkpQ?]r<Y<e%H9W<8kBG}-Xy$o6}7-1oKJOObIyR=yby)9-nAF}{GUc@>l[F3}/E063I=CtxU}7X{r-6TDU!M.(i?D}%)6T?@",
    "b&K{1r{sCmxVG<g9&JtTtVFK4L-zGKQ]nsF{ssBKigs*2w?3LtLW=tW!up^A-!m$*RV[25Z(uJ2v:}CxZjW3ikcC5M3{HgCM6WB[",
    "[9-uLT8OUmJV/&o0f?:<JIae5!n6[8HMsIX**d9E)N$a7O%m>>oz2zK:LnQo)wHgMOq--EAm@/aHD><Mc&+WfMZ{&nRY!{JoU]S1",
    "tq0j}6%qR<t9EQJ}bP<D01Ria.e}9t/-I#62y4:.C0tHXdoZ)U?.Ji=d3T$H3&bDtDXP$.!Ga&Seh(m@zlFvJ**drIXRMiSsY=GO",
    "ZEba!Q%neqJBMweZ]9nTDxR<KDJ3/a7S(CnZo/o4+(bR%Hmi}gDaRIrg%h2}cqDUpJwFgeCK:z3ajJei2xL.=egRgAXkDhs><a<8",
    "=b*75G(fECfI9YS5j}jj[S}ClW=6qjd*hAT641qap(0{>v?J]F2]G?V}:DBS^N*@i",
);

                const ENCODED_SMALL_FC64_M5_U3: &str = concat!(
    ")Gtpbe#95xF{wAHS%mp&T5fen97o=?cJro[L<t-<!.H?J8nEjQeKdg^]$RxS}x7E8^4&k9p/]Rs>Z)&*?CnBeD-EigT-zN}-{kj?",
    "N]lu1[hFDK=qshEBIAn/Ey<)uFNFyt:>SV9Guj>x6#id0!B{z+@&BeN!MShv<5]RIk[/0^2XT/kYp0iypO-$k<T(iIog8Js5UUHN",
    "9!(5Qpfi@GwCqn2WLCGUvc9OlF/gY7+!ihN&Z}wcg)fA6PC-?M?1sTWHe+y<p[u=jQ<w9nyj}^l.11u=5va5NUQ/HYaBc*Dd$xB&",
    "]h0K(T<0U9X>!%j&P4wY!o]9B3T)[^Y#*z<HGm.x9R//KjCvuf/Tzd1rn]O{hTFpz7#nyVzkB6>>]/-lD:BqEAeCTG{YsFVYPDZq",
    "bZ(h}v>%ukaxzMcD>EI+2Q({?00780ka}!^&3)REF{wa]:NdP)j66cWZWbC]YaCgW@@OMJcTh$J4lpaB2l]un3]Qsjf+eMzYt^TX",
    "2UEoAXsPDVCPDj:.>tQy<NS?{rh-nIfk9a8wej*HsI9g$2Tm0a)=/Uyqw5Z/]AOO-K#03I6E&K+LR&w5(]/.Vk&zmCPV(Jhp&NZx",
    "yw0j)2:Auwaq@!AcaWRJ3F*X9QD/2D4IPdhauy:W}D$p9odFmG?j6wpx+-fZN}3R7A(LWNkMDu%LQ)]&a.D<1lH[qyWrDP6Tgr*y",
    "A5#Q.&Z6]:MnjpW6RKE0fbv2u5fv#q.#c(8Za.^:gP7jpr94NkFh(e]Tu6=MNP+n9iRE.YwuMPV]6*6J)$j}fIDX1.s-]V!HHyw1",
    "bGwg}a>urI)MzTPEx2SFDIh[NR[.EVV1Roye*w.ad0+[S4Km1E*^i5CnuR6!Mo6=l6L*91v4A>3SdJ^q+&jU@>*fb?M{{zP2W#b[",
    "DGjiL004g7&bc[8]UgEv9@8l*PTrB^hJuDO4sFaqMTHd-=n}1IGh-q]:bm&2]w$FkwK4&X{.wiu[oU<ii@g$LFdhPyAq$zIUfLk7",
    "6sM=hU*>M[vaJpW2doJ9>L5BG=F(-8r>jBc[lgx9pcMh2(OIc{Z1U>pxDdvxnh(O(njdmc7#)iY&ZO:qg5cHpaX!*%ck@7gbz8P4",
    "4&8ri77Q!^soe4UdJr3mq]PV*xnE($YJ@m:2Y&wBxW&bbVd$[-3?1/]fRXSZ/Ues?e@+MF<T=/Z(^k8->(b[wb-.f65/M$EdbnC#",
    "^.**MssN)}maspY0bd9F-7KvGioT$/05%zX:IMj=fOlI>}K@LP0cz^mY6ozZatE*%T$aAO8$uRp1^fqfxS}F0sL6B.1RYfzi5B^J",
    "4DYl8fC8.@<xK5}z#&=$Cf8i%jG*9A4>@<M<}?akzE^E{/5BrPjC<>p7{LH5RfovgNtPVVjQ/d?{UbLR37I<AQ+Nher3ntbPGeb@",
    "})7*^6*ww8S:tLnU2heY.G5w:d{eu9]8/{}GwO2&{Oh<]k{iM%Uv2^x/SUW%)>Z4?eS}XHQcr9Ic=B}alQzH3La+c7B0<yuAsZR-",
    "v7nP!05p^(Fn^#<k<LgbOBOH1KNkR4:DucT65fg{A??6@[/k.l2>?xl@V6/Uc#y$cF=}7DUhC8GoLnvU0N^n%JwlHd[53}Ce%@a)",
    "iL8<^:X*RK9]uYP(6YyJW6{pNN5xyoi2FCN0kJhbfb+o-b{v>6x36hFYMn6PkKG-e1za^I/M$v%+W*N-.h8Kk3N&Z4SDd)GxVu7C",
    "vFmHI[l$MA99J6Iawo)tRKXO/h(ifhxCmeNg%7-&JuFD*A$lIVVaDO>{STeYlygk9rf22iH2/<ehFPXl^j4gwSPqh:mmR[(&%%u$",
    "1R1<5Fcio[O.+2z+8?YFehX+qA8a$E?t$wO5kj(x99DGe{at4j6.poEGPN=FOe8pwsN?nhS8.CjSd%-Nr</L50Z<Mje#XMU}BMyO",
    "SEQezE00y)GS=(tqtH8NFyL8)nt9SdByQYKg?xJEd+)4#av.+XO#fuqPYbsLr2ePGtH%z1.8lRNv4buMh^R]^+^}gB<teB4t!$6^",
    "nBM%H&8E6CiPlk:Hs@7WquGI7KE0e:VIIIM<s*o>Bs0nqetdWgZSKUXHHsn#ds{XCIpdIFL%h{}lj)S&4&W.%>*jli*op-A0$:JS",
    "2lg47hybN(p5Q>J6V-*e]*1kBlmOo+lifhjTzj(JXdyzUFqox3xNhK0:zMn^PUURgRQ&t(i/??82di6%jJ8t85ghh9C/n-bkSP)$",
    "bam).&cj/Q[e-mQAlm94.LP:=?r{N)SDsVADU^K-ZT!gNN3q{g1?4KDNmK:0E#@II7.l{oNQ$FwA3]>ysGg{Q^TOWr8s:xk/84RA",
    "0:u2C:C2}[-8]t?&1jbACYb(govBRq&=]ER]Q-#-slqfG1#ks8O)6QRAm&pB>KJ*W4uU+CQLC:flD$fCmqqn8Xx=9T/BG8*Z&7!*",
    "HAc*/L$wl[I66C]v?$SHqiC#:=eoK7+k3MJmY5TsXdrk=sZkb]&ikvdRh0X^U:/X}lA/jUJ-<-xaH>xkaqhjExOTdhQv:CUS-3R}",
    "={CEV[n^+QW2ISfZ^@PhPBDmXfS2*LjG:k#>i/olDF-6[@8wwlLt!0=vGWJj}HC&-B##+rpzDX?])Agvc3C&)m0RFtFp1rcKA[Ya",
    "DjasFfofv5Sge]k>51hk/eMO<C?L*71sYFDO&B:6Tf$!^BJ{Bv}zYuQ^AjKlwwk&xpsnU@Z@%v6U(kg%G>}yo*>eqmARSA{QNi&3",
    "d?^@R8-!nr&U3*RyHGW@Y$@![Y=]%fA]LAS}:hEse3tya=DoVY58}Q)mYqUoL=p$4BNg$Js)qRyyDh>*U>zbhYT6f5i-y?$kS&4f",
    "kYaSRAowEVFtr1OxS#S)>X!OS]LDiznxaJu*l*%mtj{1Jx0*++<xQcDR}m1[<c3l4*)YlBn[ydH0lTciq4cX[4O#M9n-8OyUadjg",
    "fl-gm^0&I/00x#0OkW5kwH&nAIhwyGJU$c%i-:[DYlh(w<GjMTJlbA&&0&-2jb#A=HXxjXd*8Ie(s!6#19rcS[xK6SJj--n2XF&O",
    "sb&51xt/r:S(bZ/Cz-uj9Ff.zw(FE!Tz:6gE&}cy+1W(e!.yl<[4b45]Zo*+aB}UG(ssnngn5CEm50<G8gr$#5?v>jiy9$=yMy8i",
    "dS1fe8HEf3TK/kvNRFFdoDd.fBM8WKt+24ZZ*{&=fFZZi:E64>=C=bkKvjMg:B?KDfEU^v^-HBXFzHyv*+jsyS[5%<Y-7pk6i22#",
    "/OoglxVDk%Hl%!X^@S>#xA}IHnjJbiL*WcaL9SHs(TO)oUQA+&rt0/}n*n/0r):#joF<4r>SMspAjh(iFNR8fS]Q3szE@-cC:<]f",
    "KiM0kS]F)#S1>jN^cZx!HEX<q<aU:>Iu2a&WJ#W4N@N2)mHE9%igMXR&wccKqSm:wO-l21a*t#IZB@u@MIU2MLr6^FRsDSfyOWie",
    "Y-Grvtq(5^aZZXL{Kzo/.vb?Cp+E^w?#V[w0.gbwNKVp&oU/=6b?jk8{?@a8xJzWR+RL.KxNJ=w0g=Ctde9II1wrZEXp@XInW1F]",
    "e9:KP<L:l8RBcrRj+=UB{O]<VIf2WWX)?>P!:{ag[nL%40ihW4&WG.$=EeD2XKwtqm40RVKh0#[<ChwJJjMkNP.>csc@6y$H-+Y<",
    "U!GlYEiA.@QP8Z8j<Ki[^nt(^PTK.WCjalJ{.@iQh<[LDPxEBZJ<>ckCjctj(mrGg{*ntg3u=hdN-}cZ=3M9C@s68$h5[Y>kM7KB",
    "Ove)v1rZ^VPuYbO?DOe^@qXiewVRGpWW{kRY-.z15uF{C0d?+}fRI#s!#2laB${6YFhb6Qz0N@YL<aszsDOn1k2!HA0iyJdoSnB-",
    "Xg(P0L=*y!C7m[5Rz9XqG]TA>8<<7dHLdJH=lGp5</Y7QWo*d49TF:UDx%vIyuS7sGZ+^20Vx.9NI:vl&!20dB<yxdIk}#p*wTsJ",
    "d>7UoKJdeXf6wzheD7e-2$JSe]T[x>d5.FQ5So&9MPJ2Cs%x?bimC1xn(g+=I!{l[R&vy-RJl)7!*ZFVi8M3Raq[0tHGZ1QgJsc[",
    "vTRYG0I+=u@Wpb%A+vwC]t.Fd=CG3jC=}3d[%0ovc(Tnxi!&IO^rk%.irycZ:qcP}FdDwbA+7rC9g?2Xu]:&xu}P&H0)vNR)4bZ<",
    "<%Dwb[sSijUV.TL)MZPZ&8s<jy:xw2*=oh:wBkTCXwWh!)DVgm9^P}j/R3Qad</)ieBZhk9je6@35vMO]bO{^--7*e^dimgptLoI",
    "(o(^UYr@G&2HH%.u2{$0gObQH]7!qe}B5=Bp#OYy-*cUs/lv6NJfE0#)0U&@=sdXO>/7=VaC#{R>WjF9ixFB#FZy+/!m]#qr:@c[",
    "7+y$pxq4=ceCoMqso<X%ZK)Gss16!a:]>Zc{pk36?4U^X0tVxYYri[MV9PlIUD-5UG1ihw/g?L]=GrLswBLajW82H2Ql@nI{*r$.",
    "^b%WT09QQgZ-p#&Fz(XPev{c<&g(WKd1(X9[r{h!FBA=P^T]d}0TjH}sO8+V*CHm*]N<Rpz)m0<eOEZv-ag8RBNd<FP.GM<e3d8X",
    "X-}y53jUg(aq.KISbxde%llz]cx153=-dO>v!S<oi?Hf3=%8psujLn]r4>>%urz}BBOo)>Ubv@a>u:N&={jCvio(Tt1u(@Kgk.78",
    "k#2@]Mu2CUbfY5W9p@X-r&OUN^N!n*:S)$4ohM>DImCTV[)fQ8/Bvsj&+]jraE4I)J{/rF97RqrV4#^JVwmE6p@#CC]3[)Dy/$Qs",
    "1pzO2Z%j{Kk1?-=aMa3GPAv*1PZz]}i7Z($y}6CTaO91Pd6AFh{$+0[KsNm(=%[rkiel&-/S<6(T]Y}l^vN6}:]XlFrX0jB)^eYx",
    "r!zy9&}5kA9$:w[sxgT1C2#e^PB:Sb]BFNE{6PS?b!?mgG--Hj^TOZtarkdguJ.N!6X=2s0[+b#{nNF[R{*yjYC#)wBYox]{(x!.",
    "7DV+442%%zFq[AVdHgQtCM#OT2EU})2g}.AK7W-vuS8=}h[8^JgBJ%LT[gHn.H9m=w6mXgFMley+>]F)mdQ*H<XbUsGYXZ^}Ab-Y",
    "=m2v140/U3ae-<qO@FApVE277wz/8N4NLXeHZ)@U*}pIqst5:HprZdfazl-%ngq9.hEzq7bkZ1QQl>WsZ[aljO^i.cFB7?0Qw$V^",
    "VS/dGK2}A%9)UqgKl[qW.#]enTbtg=?L.[!arwV?H#&9VMkGP:GA4SdRVTH]<CTNhVrnD!DKZ$cm7C7!1Y8&@PWBQ&a.DD(YmpRX",
    ")+(B9p(Ej{Ut/^h4g:C)G.EjneG?}]wDn$#T:-8&/0M43+t*nX@Pnt/Oc#Zr))O]].Ltdmn0holSidWmhyv[a*:9N%Abk&{Lv8CC",
    "r-D0h5ZVMKPJm:I4dvxl*@bh=av=:L>n65(F5[VOwI.cnm9W:9x/S<&f<F6HS!>gGy@RP2Y1a6sQD{>6@*<b8*RhSgHFgf3QKn$6",
    "6Ko#*{RVO=pI?QDU53u!cb9kuvrq/uW$gIw9-2G{RpqMW9No%l0>GHr4-3/jwHYyGr3JQ3DeTJxy.qW>T6UG.}DqccfWv5qHr^uz",
    "RB=6D&-2FiUQ}My*hT!&vJp[3y0fspL*hAQ/o{klEx}l-KF#{c65mDtrmj8@HC#GK3DU.0e=zLg/$$Y>@dhyw2oM-9P^T@}Z)hF{",
    "Wm5za}O*4Vo7FsZ:S[I-*8fE}Qww/wAMarU/P:$25x&6Qoy<=1BcEUeroXM&Eq+ff/Brx}irx=87ONcUS#0PSh#*YSX3n9gG[OwF",
    "RZvP>JeP*h5/RuBUEma!qEU!Lb9ktUy<$b5njQ==cMkApC[?ZumA)f[B0]FGc}=]>hR%t8}r/VBY4F=3D3Tc/6F0]X>8s/7aTddN",
    "-y$0WDgAt%rbbI#SgFpGNT[%j?R?exr.:HCN{ot:0pUrF6x>v/y<{Dt{#BR5N7s(-*-Nj[+oy@t:aFDEfuD]3E+9h=b*HP@OYkV-",
    "7lZ[}+xDuqaQyoCm!BycN[?2rahn{ZIkXCc>*G%P2uz8AzGEnuiIG3TTE*g<Mc$yJ:22G:D816FJ[Iy=0iS)%?BykM2R]7r3Wmio",
    "Zf[gV6F$>mQU6&Q3T7.oHlfc-?=.<2A(0-06WD[CFzzbbIgczRGhOUXJFmnzjdfqpiyWWAIKMSu!14)KQf2n%eJbgzAXNkI?2BVz",
    "K94A^FrRm(h[tXDNu9Dzs1)Vs(pzaxt/>eMp5/8{6I<+6?<fS(bL>iIg}OBOuvu$h]mdSvkK17?^HVt>z*PC!IR?ZMmsQog:P{-!",
    "Md5TAYAAsu110d{:acN>AI}s2?-cSV{S255QWCiEy()Qt!u?I9Q7}Gdk2ka>vB9Gr<wv6:g@Xp:O*g0]T[WPkSeMM%HzfzC2mntZ",
    "*AM)ovt!y88qIHhjm4wV]pLoX&p.Dg8*c[Hfn^9HQSLg53q0$a?Dp&i6qk>0xK/7CIK)[-.C$u:*UYw3?hxa.uf>rIf!8L7l3phX",
    "i{W<28Sw0oUN$S:!z9bIf><-MHD<$n]lwl/Gj11etqFgK]prMU2!}U0YhKD#[AL]}:5x34vhvOhW<cU(@1LbO*y&g5Vp1sGu@@:V",
    "K2Zr9c<)WJ]<(7R8K/{7zrDF-=N>8tBAAy5l3=IAp?k5s)8B=zn?r.3uo8Yi6SqXV1xckM8zlDLsw(4WEcs)2.z#u{jssk{DGmZ.",
    "fLJ:m{#6Smp/5V9e/CItnkCjd05uz{aPi9tXh5DCkRFr:soixO9jW&s9*{d{>/m.SZ&h#XS2YUpwk8%nTKol}g6xN2sLm2{ec@ms",
    "}KgRL01IAd*tbT<oFZ$d{Y6zt^V+9M6TRI^w)Jko}GKWcZ8nmYY7mkZsXri*28akmgB-.}g-H8wEan/)fDTaP5%Xr/!kNZbktIr]",
    "1f.?<i[UgXgHaMR5PFyDU+zj-S7x0jw&fI3hI1pX5{ub@RbKiOgz+ZdunjjG/ZR8rb?=3Sh3##u]G>4xg.=W$CTMKnCjnCA&e@)V",
    "Q6K:.kYAZnt0rS:J1dc$HnWWc@7w!eTraf2[gpF2XqCyxk5aVy03[<hZZTd7&S9e@VMw3*LWo!xAbr.hEEc![MJX{6a2$yRAbjq)",
    "[z^P-H/{v:s=T(yrGvby)venZh10imgt9UY5DiLgnE?:-Q!B3pMg]bUT^zX]NU#Qb+bp0dP1G?x7x45[S@bk#f5&A=q$N4LJliLj",
    "C4UT8E}Lg-YlvX$tvuFNA)eBnJ3]/k-14Ek<(:AxM@99ieMEu=5Br^*mMI$Z6MNr&D:TT6y{LrHY[qfQ<Hd6k:mjw^2JRaiQIlX<",
    "[pBa)@6&Uf?O/VjLQkLb(A}!HllvHq]NTlW[c4BST@]U!L/HqEP41]E267D)]E6@dFf9TYPQnJ*u#lj:49e7PbdX:TU2o5H{$#Dx",
    "Am/PMOyXm+Fx6@t1M32?sw-$IXxt=7^6Rxq5j9.poWDkx2P5T!=wMhDd0VB*PM)]U0qC=<guh@W:q*a3ngk]IGFs:^[zFYdf.Mby",
    "0iYRt{^+d[p>c?O:O/qR7&}/528t&noQ=.WSeObEXK](o78-Vb60#pnwyXo8SHW3MwNd8Q.sE%$gwi6e7c)t.3xkXks6N]4(fRx.",
    "t.(iu*W<.)y-ghPySA?D}O-(SHxs9Yx$eTPh:^kIVtlH8x/R$KCj9193K@<]YA6JDjGyiKJ-vjoW#tKB/[Lew9$+fWU0rdOaGR/X",
    "VwsHO:!KOO8&J=uX:s]h1w=hmzzU6KY{=O$3byvv9.nySkA[5<rte9v{7?FUmV-r/]d&hpI-wvj{9+>-E4MD-04!U{>LxM&7<^(0",
    "Sh+bb}w]h>kFv/COA^WF6)AG*Nizk!F*drX[kp8/*zRxLVUKnsa8?DG:)2eap]vFX+h43J&yvruW9+)q97I2k5R[6iS6@.X6/xnX",
    "LE3){C59TjygBXoutm!IoWQC=*KPHs**tlq-^lTU>6y7x+7*Oyre%!*[Ob3/E:}/t^[.Th43R!@1xheg}AYnr>R@).9eC(O&?[-H",
    "^N?zw=gg3YPWs#F2YV!b5AEijG1}QP2gvI92F&8TN4st)-RAACl&hhA:U+]&r5jnj)2qrnJ?N<:Vn34cHX86v<^)wPUDs>spl]=Y",
    "3)@0/49W/@dRqU]k7@t!Z]*<65K(*^YGk(54cBj%q0y?v}x^7z>Yc?<7[yxn}3>#<xA#L<i<p4%/Xv&rQ#3O+Z9D1UuDK/.IKr=]",
    "juO[xjg(U1mB(4:Z95SN2>T=MkV&=MAouKIlH9{KwU&RCs*TkNO!X}4e%}RZ<r{tdYG=-^b4@O)PVRI(0bu}{aDn]PSd=GW*=-6U",
    "vcIdl]3F)IgZQO6u{dmF9hYvwM@484M]6Ady]([+rbJgf>6ER5iy=7)2/zM:i*1zC!U4vPS)9okS1)^]vUV!OzxijJU*UV%l!f(7",
    "gT-5k^E&X!S+t{bn0]G7RxD*vv4J]Khr/t0P3YX:J3+Tt-$]A5S.F+)BAenlx74K&PU2lUq#*O}KtH665T%lzr23N<&6SX&2deoZ",
    "FzQ/1*&GIV04$s+.1@*IfX6(26+Pm=WU+g46ddIeb+]MEfVXvIcG]Y*PR^QXZirh:^]XE>Hw2L#O7KG1ge@3M:M<nFVJ-VTyQFYa",
    "N7LEyf>Qf]b5eR(X@a)0iemP0sCQcA[t4Z2jHgnpEiP^$z6aUH4<5CF=!8gfvL}^ZKD{OAawc:*!W}.xvbF/kk<89eH]s/E8Bc3>",
    "]Ep&{+=Pj85CK!&@H1$C9n0i:XAN}BrATySIQUS&+^}n$Q!!OnJe4^85d:aK091$wC-06C?CF:o]TNTVp?441^9@1TFb]?=p]OoA",
    "nr)w)Frk5?k*/LxvGnTbauxedKMjRHFsfm%RUBVn1u}2[jow?M&g/9(<tvN:Z>tp(>Y@BR&83>}5mR>brdC1-CVB{XJhJ2QV$VkE",
    "2Ssp#Q%04chxw)h&l[hXL*fSIrZXk#3?GB[*]x2vYQJ[8^8rAa]7sa?m$xg8A*^SQObjc*qf*8z=r@a=2RGmy:tnBYm-)5wNHH@m",
    "sO%^midGHyvyu@^u0?@)nm8?uOvcf@p*[%*gQ@{*XT:bw6[f%.9rDNi)A+2WLZ[fCS8MRS42@g*h%wC$&9Rm42#9MDGuJ+%&I2x%",
    "@)9Wu)1f=3/BxdtqOm$4.Yf4FI/-L{sA^(YrXr-5pY&1b0bN!fzZkuEG#}1(9[r+AFRZi/da1c><KKyCFAR8rAq=m?1H}%Q3Z}:z",
    "DIz*2EY]Rj[f}*g^wTxl/Z2U5u#5#Wd1]ZQ5xyuP)1+*en#i1YfG-{XQw^c^bQsZ2(}:y&+-K)0aNq+noW@X$L.E4s<Hbi906:sz",
    "@Ow!-{>DTH+e[])J]a@)}Qyhi.8hoSZczzaeUiD+y[2gN+.XT1[v>NdkK@bA2?mFtLkQ3E9R<!vwvl-$G#gI!oc7@mDyU*n[^Wm=",
    "lVx?7jJ@z>8:c}WY7*&<WV3D:&+05wkT9dHB6:R=x?{DDjaR%@EvazaC>0n$qH885",
);

                const ENCODED_SMALL_FC64_M5_U4: &str = concat!(
    "v]f0+e)Bb]>)96Ux.r{?]1Cmu!w{^^^3%}@Z?J)i{d8c@JoF3?oz/L*h8tj@F%cqId7T<<v^<ic(r?osvml>N0+)9B:fyDk)rFil",
    "jJ[>WsC<b>)9!D/f4+$ye}OkV}vGBnrCP{]77m#xQ?6/}v#icc(NuCUOIk*Z9$Ha&*2e7bG5P6WU.[MacY<GO@v=43SArcl5^[YM",
    "45Vh3AVFQMd@qWTB7*(MJ+3FHToijJAaVNw(yUz@=>o{6cj^TIxI3#xGFA/@d4=8NSj+R.>iYs+yd:Y65p1F[p&YI#+Y!d(1]?W*",
    "m%*T/2ShMB=j<T2CBB4C45}*<WUJJ#=.DmUr[!ANdJFpD8Q3dl7sf#-jD12sq[dZ#/jG?4JjQjB%gs=k=6CQ$[gDl7Z$}U{0dsmC",
    "3jJDo3tL}&C9s7j]JkUpnI*-p8COglBtSUNzxv+1}4?=0Ojp:!{Y8f{B+-t#bbLVg.jVj&80YvfmBTJnfQU!LO^krWtv=9[I-4g>",
    "N?r7(ot5M-Q:55^(U.S)m8ZDoC&[Ccqn(5tx}{.LN6$!-kdFP}c@+{:fMJ{J5zo{]-!ifZp#&uNUXH:iuz:qSAk660BDd}oe@f3Y",
    "tQLls07IHHno{&uxu!Df=mu:]qHbl9.Aqwzw13/vWM$Ou*Q{GGQIPwhdHmrj6y.K7y/0pdcBnW{bdu]5R:%#P+qTx5DsmmC7^g:d",
    "MU.]2LY:V5M$b0^roYJdnfL).b%)Zbj?pXTTr/QlgUHl<YKH7/b0>rN&]4sfT7E2FMYGcH615xpm:w&-/!:IVwEip?r@}6?G@oO/",
    "R({lqr[v+[Q[#CJkI?5SXxMRtzCEpCw#W^M1tZ>ci&/bvZ=8xW8?/rCv8*IRFcI%=8KMatuu2!Pi+dM<mc#wnW!O7W=t{$s[k98%",
    "0a{I?kg]J>Fu+6C1{9prm+#[l&8Ks3Qw0MV:KHnNs2rW=NXNOX2]b280t}frX%VsbT<%?(+@9N8^^M3PuE^4UvT9+[0jRqSC^ng1",
    "T>uMmMBE5X{x8A#eqI=?FH}3*fELI=OBu+DeC=}v/C!5cUsSW]ZgDQ)m!>lv?cTi@(D]z>sA?c=R2B{*YqArQEGn7+Fo73uM@O8*",
    ">W?#KixAc.H!ks{YRrUlxe/d?Of{3O^Y>*DV&@+eK%gZQz$C$Lyxb8hawiXp<ELUIF2nsvC8a.-)<!#ly8<[1A0H{aFmaQ6g%tkG",
    "/DYT[.1Sh?igd/JbWG(LN3-uo*)gF]0ent6CNrm&P0MwG-75Q%Z^uzW76bvre6m/OMD**wk}gQ6c7?IddGa7@}/umg%kV8Jyw09u",
    "Nc93:E:27gxAo3+Hk0KUFD}@k]zW1ElKaX@6qxfMNzT2bIT{XU*=?2!M)}ZwOosD!Zm/)p0javxqr:rao*keZ09p}!()x&/b*Xtg",
    "WnT/KI[y^TCBX$5?ndq#@Nx@xM6PSSR7&+1<)Fu}h(-@xj7%C-g(4LZFbvv2NE)HwM0feg]X-fKMzPv0tr[%@/[3g!]oLcA<[29c",
    "jOBd=N5<]kv/rR@H^pb^+*41/H+>yhRGg.gxAOdALMRaL-fHIsUZ7&k0dax24LkGgFcX$lZ&]uk-zXfEd:l}=25r5fUXjnakHoJ$",
    "W6Kp9zrKRQ8M%vUK2DdjN<N}[V@6#t>Qq3B]Umj=Cdoiqdt)mJz3p3QM+<AGVABqwEdd(:e&IU3g!h]aqHoE$CE4:>KaUP!4sshG",
    "r$u%%YO@KE[qS*FEY@VjB:7ga^&P8+Sc%/x7)R$K:[q2+*KGWCjY1qR4IvR.Rb@7eAyw2jxt9p/0[*tY*4<smaYIPcA/4y>?:o/B",
    "p{0:iISQ}LiDAOi]2TcWA)SZSxOE=}&co(o0r8n@t!%#mw>J0mDkaDlFv&h2k&7J-+.!W0Xc4<QGBBS?ws@?lswxh^FlhXh:i&qE",
    "?R5#ln/%ba>Csq6>j>AfAfekQF}*[J7Qxano<po5F@{LSk&&FqAqrl>[CQLyTvznl0W!4k*Nl2zfme{CzJz1=[^.9K0bZ]i=&]T%",
    "BKzl[T#2dJoR%c#@wVHzkvAFjH}.Uc:N>DOb!$@]nW[61dRkY{=ML[AHzhm3szfiG-B3P@P[<(aZb-bP[x#2m=0SPDUI*!w9zwu8",
    "8geqB]8(rZ2=I4y9X=M=Y3h?w(S]9a(V@4:mogtwwosk5v5f!:3Hb&0/SG+rkURA{tO[:1nqe}mQiuKH.#=%LZ>3ahmdgL$U(i^#",
    "xVG6]2Wik25wDb@8c)[t.2PY-g9[0PzJ3.x).3oG5>]n/NYZno(gO]a?&x]-3[u.t!m)4r5lOUsfN]&Wm@-66>SS4X*Fx=wBvMqz",
    "^6PGfx-s8Qu]q91}OGwFP+.BSFr-pg^R=DQTK0xHaQ%mv5(t}#!mW#dxGQ/da6HCkDB}V?-@0>v+Jg>0&N6Z1Ny&To&W75cPE7pn",
    "oW2jPxsdPH^Gcgb9/<Hrc2t7]d4MR^?ah]RpJ@D{+^hb#B?6-J:Z-1D5Y9Fu&ok}UtbXL^ly:l54eUN*5yEQy8uV+F/WY{-.CW}T",
    "Z><6[/V5I9y!{265zND5e<B>H[eBQ804@vRi!Iums<Gq@cfaTgUQ!Lzguj3MJ*2*xR&19VRbgwVg)uUbRg4HtJTki?Fx<>3eik4t",
    "RC^HVsNlCWvj#gN+dXU4B1S!x48Q:Z>c9ZRi49.04W{G..s:8aB4eNfS#8KlAG}1$Zl>GPS2u$}KNGD!]kJZ48ul9/t)d@9Kja$g",
    "zg}h!Q2Rt1]QM=i4mZldjA)*syk[lqX-/Z1EIAd5&LlOHpDkdlk./#=c9g^[jYSfeof+byX#xg5K:3pR&3MTV^4TZRo4O!XMI{tp",
    "L!t{qd3!10wfmd3<]IZ1n>+Klr6FNup]3v?i{q)ytOlejGBfs{/WG^Z*2I#[WJ/uEepA(0?#AgesDC70x1X{yP>Gs@suSotqxAqg",
    "H0ysAp:}7h^2yr7d2sh5IJn3bu$4i&}+k3)O30souHa:SQ<f$]5Dwm-4Mpr15orN?+Qp&JePm:Ox[4?IWN@r*FAAW<ZYpM1fGzXp",
    ">VaDxbA1ou7oZfv=Scw@.O66+SC@ebEmKSd*f]YH5izp7[iOXu{dkPPut.ja]C[QVg@W[Q^{!DMwB^b>f8iY8Y^BYiaCy3l2p5fu",
    "1})3hFk4TV:AM2]aDkXH7o@[kaKIC$aE?TT!j++k)85UkoL03znzOy3WNn9Gg#Ldx[Be=TfMT#(Qw9@^/npMXbrDD3<M5Y[-mS8A",
    "JEfJ}Z%LS:bMl971zsR[ZCps5An:bDuxIDP{t.HMh!oP-bV}0d*>g>PCDC)Hl[HqXnkU.^LY)G#05fG}j?lFlDB%l>T!w4]L%<}2",
    "7en)l5C9X1Jc=hJb*D@:tpp(jKzhtv^/Mvf]pn0czNVQSupvn@^I=f2uIRVaG?u8zAc62ZrB)q!>166%r]+Nvp.9y=P/ErXGbiGf",
    "-+2Vz}/2#T*^i%lXe^&GXk>yGBMkRHm1^FiY7tRUxQ09(.1hoMJ+d@FihP&(Fves?fYPsGKzlqVy6A81}GyYdTC/21f65>x)-[hj",
    "=FemW?fs/v009wsfb#VhM*2i$Q!dQHI7IujX7)pY:@duBYTYKQsz(OIhyWuyS6^(6+W(F>&>uew2on:!q4[/2Xqm<YaK&Lp>.l#r",
    "*BHDeH5hxyUxlu#39C+<R>I6(UTY{2-8On1AmHNzw*$ya^k8MLA+zsmRajDE=A?D6b#}N>0F3rDxlypti*tgVBNteN6z---lI&TP",
    "M$Rb^djH1+]<#pUs&PbTsVjTvJ{Pj#1T^S&(!UBKp%p^V07tFIoOYgLH7I3YmUGbKKK3?qbR>(M:z@j.+M]*:08@>Wsx>pd=j($=",
    "uG3ZNNaKjdbO++>Cfl)IIllFm2uQpQz#jJq+QDb$uZROvweZB<*h=]1b>Tyn)>8C<nsb4znn%3>TuGlt-C-%.[vjj^Q5hgfOB^l6",
    "3!9&)Fkm7Dd6L&$678v91=nHcNB^S=SZN36h7aqB@-Yv^s#KQrG6SxGPBhz}>A==AkgKqV1Trs@e{G$3!T)Hs!4F}#ycIITz%dUA",
    "@^&P.dO8S-gZE?VL3P?9U(j46}?)wC1trneEJ8z4008gB{g8<&993faxdb^OXd]3cC02Gj{p)Gr0gPPxr2lcfgcszSw]@Np[ib5V",
    "y8Aj0hD[3Z8P{{I917ieaMsz7yb2fv[t*VAikv<AP.oc+FTtdQ+Se}x8B78.it?$kp1.eiFU9y:5v*gLhM6V<Xng[5xL6-E0g#Ou",
    "E#(3uotNZy)OUWJxiY)GxVncbSaI44D#dv%@+I6/AV6IvRa(I6Mm{JB(jq-k@:yL)!qH&I[pD+mkMMzENkuoOd6[PvG?Fg7l]He<",
    "m=9I}?Js6=(Q->CFA-gW-c!dt8]Ki&Z5XbBe8+57)jP{6j=$Ai=(YXV*OtGCk:V@k=2dz.HW*3X9Fv81V}E*uDo^4{9o<fgF)<x%",
    "A+A.iA>oVi{${@(7>rQtGSr?vimp<v6KwHqk>GSterx3U^3(gw@k+?I?s3&D@&#adCZo6D(TU7H^@zVTZwdpZ>Qhw5SQo=18}V^R",
    "H:.t}0ihUMy^H0xL-Hvd7.y{OFr}$7qDp-ITP}LljMd^7D@BjZ0eE8SX>vPqi@b^q+f[>Oi]2!f}okhs.Wh%G]Nr=5YBSQS040#B",
    "zHdyb:e4+eyk<P6LaiNSmtmwCM/C!ybj!%K:H&0DR1z$%Ve@Qh1[LB0cHDz$h:CN1QzU/1]N>BLG2iKe)1gtko^T7ds)%zA/B$:d",
    "fP/[UU-GwTwoT0CNa[vFtYp-E97zJ[f7t-3vdzWTl<k(&hG2h3&0N@BWQ^PB7qdZKYDRHiPN%I.Z!U}mA=zvHzyv9Jl+#14BdtcM",
    "-M${8Rrs^t>p!B1kMlK605BeU7q)mq(]URr-jee$R)SiY:kq#(MHIsCcN5(h>zl(nPGjKn&hK/gr8P[.(q7%5s)j!Q85?6/s>(Oh",
    "/SS#t}v*DN)VWEWPK>X@FfpC%vgR59M+T7}xNc7fHN(HH>Xt1HZ:x(e2:Rj0vifIOUkn*z-zxKFNiO-C{S-YJ0q:2=32-CI82*0/",
    "w*c].Or-F9.%!{nVd$FQk=>tP!o3X-p?@b9RQ<Fmvd9}Oy{?f]FA%}.suNFlU.5]rjqN3wLc%WuF*XNK7?/Tb=(O:>U!#bRE3x=b",
    "tU6NZ0If.kI6>LLMyjpdXhoddB{Cxq&@qnax]KaClj3h@)JZvE=RutYZjzkb6Fd3.]roHL[Y^&xG0(7KMT{1S=lvXIQnm&OB=?>J",
    "AR9l{[.t.g^}/dJPI1Djg#w?TA<pM^r!A#Sytc-(35m=VxGrmUgfnfN&K*5whNl^Q!@@bdOx(:JEZsR?xV^qrZATo6Y:D?-&!*&3",
    "=T*R2C81zU/xASbzs<.&mEJSd!tX?HX@Pg>E0n7BE&f!3etjo4V{bc^.Q[T#*<!I26I%=L4H1EGQw=]iML=R.&k#M[U/z$>tOxGs",
    "Nio5Mu4jopQm9KB[t:?#BJ%?Fn(uY[ApDfamG)ek:I1vYSIM5$U(pyYg=C+a{Oj!6waI)$2&W9]auNxMWrA4)YeYNGUNSlGw6j!:",
    "[flyC(sTP&3[a?h[Wq7PmsjhYQcx<pHtc):L=o+T[TpEZ}lge?.Nbj=y56hvc=YpL*r3fP-[0NuzD/cnsTwkM^=iwLI81ziLoU]F",
    "]Y)8NOfbnPL8T>6Ot.faLzOD:4IAtpdR0eaA8ov{[nsG7JJ?z8N./@Ra4Ebf-5}D(D.uXs)%Jkv?:B!lTqfbD>PihSX8+]H2w}{N",
    "jKFX[u/d%0N+RpmfNk%#MtB4zYl/P56pQ!(RU7S/bzw3tzf6OSps]If.y=qj0)*l$NzjM(1$G{w96Zk.Cl=^m>3.61]7e-)kWcii",
    "WlvzJZPJ?=q{2FiVZ0bR2gXF)n0m:N8JSYrxG3Tu8GzMp?NsaZ+^X6B>CbyEF2(LZN/tcb(>zYGkdc&[03<LroWPSGJ809XBco5d",
    "4n&/Z7v>A*KDuXg><35HYoE#.e$<ZTj6k!58#*wY+VH{VSHw2]Zkv6EC^Z>>tb^>4-OkBC9uQrQPi?JfALVLbwmMArhxF.Ud6t+F",
    ":.!mCtN}pUQ-OwR=DLJodoM+^q<&9GaWo:a1?6WV5{cD7V?ldrR1)$Nnga>YE@n4C0HsqiTS4[!iA0KOmq7dk&ZC%)zSWR9{%H9C",
    "y)Le^uVO6r+JCzeS@V8i9GftHjq-gOhXJHiGZQ.j[QF3Z5hyBN!=@[4wVkw/5#wU[{XmE$gl4kR?%p!od7y6S=L5)+n^CR=>B5iV",
    "g2q3aN1*sd]KYgetr-$sbv0jxduYw0[WI^^i2/uwp}:$mC:rcVP7[8P)ObjCN#BX/fm+lgn&zz*5Qt/*^J[3t&YeOOdjx9@*m6PS",
    "55?nSor/)f7CB-hI%gR:D#RnjQvjbRCzjOYFpZ3I3Rl2^a/..e-q&ouxp@wu[04d)<nlnErDZ}$df61Pt[>e?OSJ[o85{f2v2>:Y",
    "h{mM3AJ>Cp:e8krsY0Qiz]8cgS6NM1:zYqpOfKT&@x0572FU&7Q:<ezA5h%R524Q*uU6Qca1^Pc*5}ZD&9XaI+fOQ4g:]GtdyB@v",
    "Q{0XfOQ>Mutl[E$Ut[:*U!Pnj9vmNC1vWdv{.?ExckuFyGWqIs33l8es(Qgfn6z)=Si#Z>wnetjdjNG{Xp2}(QQlQQujfYCCuax1",
    "7r]^(d8D]!s%:$F2oozKQLSXHgtz)]IVF@8jyNcZ6MwN=jm:gHk-VglQ1>X)^i$*7@^>5isJXotF*D4kHpbTEA[&T*RW[[NW)Y(5",
    "J5E}84/P0W!)U#2d7jfg2p4)mIa[yyZ.D*Z6HnVZ@gPd:}v<7EU#zM3H?QrXE=.0LwN/O>YWSrhABzj0cEvAdVjEnO-n%oHf@2w}",
    ".Vq?.!TXn9>Ts:I>n/{Nv&exk:]r{a7V31xF%md^E?Bd]+r!xQ0U0)FzT4D>AjUFKG:#wvSu/bu{rAd([}&]3ryQHZzJYc9l9%oG",
    "?Upeosfca1V}res=02SYRQxE#+?UfuuDmAaoJo7@B-LqZzW+{AS&k6]X#sz#jwNJx[^MCI+F9ybY9N7Ld1oc0E%(EMz?0$+?pQWH",
    "aI][F<>@G&S7+M3!xbG<mp-8QF9M^v-oA-82tQ/6!IdE@g%oY0OM.mvQJhFX3J<r>S?$whQpAxIBc9StQ!39KG:NAk6[eLlqkP#2",
    "fpLFB:+BN{M!DO}UqR=.n}cv{ds-5A<v93iI4nZZsfUT!)Nc1#?4LA8LsQr?y@gfOqCk9AVj?ZI8po[Fi)d)}(BaI)7Aifm<SAx{",
    "vdC^gh1W=XtfzjQDgj=%W4l<5(y]]BsVw{+=]q&Q-FS29E-t$-@ivh6il1/Vha@28-2rQeGX]G/Gp4wbcK#PUDen6Q2s?VQYZFFb",
    "ad0.RN?2#)t7PwWzs.m3%m-}m7HyN+C=sa}H:6TFm#ODLmFIW4zHf7iK^44Nd-N97B@##j5]@6ct9SEenbR!*bzKA8SxI-z0nze2",
    "Y2>-3J2X#-V.-O]*)y:G/eKj60sjGA[=zy%Q?G#@TdZiuJ]rEwg@SwbKj41txim7aVfgZw-Gl>ddTdWKLbjm6M(>I87$N&Y}kk<V",
    "[yr</F)1TcQ#1bCv@.Gh@pWcO5=[AlayM=-3+i(k&gZop.E%&J9rJi6&WB<>w@XKVrMhTmq2]Yv[[wxAo.U}T-P}+h1Z]M*Inl%x",
    "e@yKWZc(o(Kqtq/6]$jo)k8dkPfLiDIFCq!JsAkd05#QB]Q@/Ur6Gn6r5HnYn#fV{TM22H1WcRm]j0={2=uosKlSBLlouFRscFxL",
    "wE1%zw]r+}]^9!Q*[yVdrW&76INwTh8(YX$U^FhV(+Yd9FLd90Zp:qaq&{EZOoH<ADC!mDQY$r)[k0{=hL]>aC?f!}9^wO^vsXJ{",
    "G)6xbfKM&M*a0}6?5Kr@6TSPUZKtmSsy(2bNiZYO@Sh]f<*}lJ4zdFfExCcna1l+A=aR*[Tw:vc8]nR?Y[}GTrEXjpTJazR?}66^",
    "PsUxys)5.@}nA>d&4mhSYLE^pRHjs^PoNzW>iBqb=xT?Zaa)4%/AL?GDVhS>@QT15}Xr<eay}Ak<gb)/adTy=90f+q5:gHP.P{fl",
    "fOWuPtU%rvXC=?0edzJ>7>bJ1CRSbV:E#{60i8CIn^(F0Xn$V-m-G5Oy$u0SM<>aIPTU.25DiRiva]0KHb5>CxHV2=[yPCe-%i>O",
    "vvsadd/ccTe$Z+e+2A*MTN&nKyRK=3]3Yb6Wz+%aG}%t+Q^:VlEsvUH=48DJmFMQv}+n0yw$M0nC%Kv5H2MA5]vHB+7Y/jL6.:pB",
    "cwscq/2Y<mZIGBLxY&142Ab/H5*JP8]m5a3S}(F8K2vJ-voXsEKmja6+kyznVUTbnwBW<S5vm}w[HyuFP.z{eT<8FZH!?<3nd!Og",
    "kIfOZVv*gY}!i:T5*+ub/CxAeus!7t23vkbzhg3-1zruIH9xd4a+0]+S[l(aVuO7q&?pSX7Z@X]rfa^SExCQ{G{+!5s!qq2sj36y",
    "ca$lY?D2uAP3N0(-zW)y>:?BWCbaciPr0&kI3wX#02ywMP0x3zGJ(+=h<<MSv7Y%.(o&[hjFwW!nL!lT(.RV^->b=}ux7:B>92wI",
    "wI-bPCpyG{]!:ixFt@95Q{/5vhpViJZ$HObL^G?>c%%wCimmui*gy%ZmCs6vFyGVzAqR#nYt1]&m7/iSS^Xnq+l$skhmZYtHvZwp",
    "JfMDrdM=DH3r6zTuGe4Hb13YuH@qo!dY(@t5I5%RdfZo@NsD9/Jv<$tzNf*qV(zYKKPUl&8=pT?uwI}5kMDZ8njP7iEEAYtZ.FYt",
    "@hzD^D)U1ue7+$eqkiV]8AK].gM>6*a6fS18V5%mMBR=m.Oz.Q**0-uEOTJsNWCD+=mDj:>3pkoO^SoBK5oBIDq[#Jsu@a+azEjR",
    "5x+OGG/.:/:67jjesKt/sE>:w=Rs5BC?XPHp-Z-yz#M4=ulcjk.Bk3I28>^X<}$6X^o0qiI08y:T9tz]*r]jGoFs@f[eB+Wcyj*0",
    "M?Kj*.W$x.tfI:+BbLJi}:SUgb2lcv--C)FI$E/GJu^%Qzg0ztKIwN{71{U(!^D<?ur%r&p6Do+)GeP^lH)c2JR5axfa$uIjO%^{",
    "XCvnW=O!+oC}xxqU5U)MRQY-KQHsD[OO)=bk47n4NEr3#2vV4zaD9LtPJU46Cb0H29euL*f[F2<j4+KEw>)V=I<TpMXBWUJ]s<F5",
    "Lj+25l[hh2i(bBvbeRq.W^)P+*q7l.fcJ$LNF!r[QMlqb6@f}QpN.l{C2FWYk+[]NlMvsmWnVe1@3sXzr!TmiW!1vH<?8P#u9p(O",
    "F{RD2}MRlU[Wg9+KS)7OzK[tLCOuYkC4S[J-lqTLC0#G%s=-%7h)+$pZuyOvhHDZY7B{E?{RJH=<xpObP(e:JWH:g/9s2t7kiWnP",
    "NUfteR9+8y39lRh8b^rBo.TNWi7zmVE8dH2aR.1wPVt6^B^FKXNvt5d*(2San$Oph",
);

                const ENCODED_SMALL_FC64_M5_U5: &str = concat!(
    "<hP825!3Le4hWKc&iB3f7&p$uH=(->Abj=4/BY]5?NlJHoh#Ur.Jbnv(tq%4&TXw?XA?eFzI#6sSTFjc]]hy5)J^>8iP&&xwtK{<",
    "M[[U5oQ.rY]U=OH=iRoI=<6tfYrn}jSpbqK4iaN@bU.WXSUg^+ZGuf7yV4rtYeFta&$SDCY=]l@3LHZ#)Jv[h?}<LX<5bIkQ)eCN",
    "p[%U%xYO=$>Mycll7j4LN9M)rr7ij*aKRsAMBta5Z&RK$c]cZ[S7N33)dGB5XlHw]QepP$sB/CBlkn0lQfa0I3)8ugk?a&c67JQ$",
    "[BI/9H5<uHr%piv5)KiSpI*oH3aa9N6S{i8M2<NJUrHmA.vTyzp*r6AQhE}kikV!2AwgrPP4:rgnE3a%wOq#CrmvIN/7>?0b(f$[",
    "X>b1EiiegOE[rT5@C!<I2kxKSU&k(1V]^Cl>5*R^Wl?R>r8{VDL[p>$^zzehOe7>l@g]y/8Od6aA0n9-XE1-!:-8)Dkb9Xzq!.Pj",
    "z6wsh+8ZXU0Hr&3e=Yg>DQWOvQLccv[3?ywq.QCG0hs$Qg63{VPSW4.XsXbY@e[POH/R$/{#zE?y.b{Do5m@V9fIva6P2as:wYHW",
    "W4:rU7#q74II]-))OuFt<.VLjb]u*1x-x!JW+z86<JF2}mG$82w:6#zxegBtr74XsZ>by}fMxE[![jx:)(y>vhi2miWlF:@U/4$N",
    "7brG+Ztw!3Fs5Eh.xDYT6O9FLDoPkeb6]bfZ=%wkA+Hg<Vz>z#*#Z/l1GI<xza*ziYU8JN>^]rTu#5&Sq..YA9YqOk{Z9PSHm.Tj",
    "e)+9YkLKFb5t*yf)(QOlJ+g*Bxi*22l$[Cv(!s6QM}c0%^6/x9peq+v!piVB+S%#EJ+%tgGE[%)nml:b/EAMc04B[UFrC*NLjF99",
    "(<+LVsKa@I^.Um.C^:qeATnK9b?x9hTt7>Yoyxd2cpnS:XHOudM:YpCO81H*0l^.>5{P[T8+ohb!MYV0:MhLEbTg!jio$a*>NSnd",
    "ATU2[G%HP!<)S?{QqK]jxZ*Ely:[.:)3t/c/TXh#OBPv!kasL{p{#v%0=b26&!rHTRA%{?7cY=4jrr$9Vsm(r<=VENF{{6J(7]W?",
    "2eXu)oer(!Fme@4/EzCorXoB@HZ&010>bl{ql.Yo/^P6DxNydHp!RLpTIIXl@+j?+UP&>Qqfe+JwyNXz/X0X@d5IFSip8gd*)]$e",
    "S[ln]oF4N7C<jt568Y{o/LZq]oAJWa00Z.egnsMSkP=VikX9:9hT=F=<@MxzZ>%9}Ac/q-Z*tFM7<o/hKHDV4zsAm)>fIt=(@%q1",
    "Q>!0z^cu7[UM)(+PD:7(5Eo2-FR2ynxjZi/q48RKU.?tIar34#H&6Mko.%=jy/=C72qJ>Nn6Lg1(mQyCX0J4p4YYinZgDinsvws+",
    "V><0hALbdiRX(N0+$?ReGmSCN*KgiTpd$4tOlCr)JOEECrMJ]SEgaxjkiNqRf(*BfXQ%rll?So0oqI}U-eyLbeq/rGS8FpiS%ekX",
    "fXoO[jI14@aE4QzIQm>R@>^XUZ&E::*.C<.=$vO^:}6fsC:BHPfD*PZ[i]yJS2T{j&+^8R0bRl*gGs6hVe-FW:gv:6Veo&u>43}C",
    "n8jFmr=R]/qjg>8-U%*#3Dz^0azpvgwo/vZKv9!Dk]RHv/3il*H(V?]fyPMpKhbqsPQu$p0d-xToEmD!q5t52G5=eUCo>RG=9Njd",
    ")mE{L&jD[tuual6meuah6-Oj#7D9OOmq*ae/E#eCOm0LObLVh<vVI>Gd]!^DL1-K2fG43pAEVPMBZ.sm)p/+rL$8U7&7=hA)6}>:",
    "4<8cwEZ4FnOtK?05!t<yy{-m^-WF}(Q.eD%AbT1lH[oz+k*^AXZ{KoS[sKWj0h(E4[c4ySW97RCvaXTBC!2fm4KOJ}9n&G-DEoEt",
    "/Ck%I[5]gOAIzwPfe.rAJO{0.q$S5ZYQZmW&7o/UydZs@aH=bH)5GFp2QTvJ?zSUxSa+JX1Xrh/0(mF(:#EZgDq7VU>bbsBGU9^r",
    "TY.BjlLq<KlFby6>ruKvK@O0}nz!v08vbE?5wLJ:=@Z-r[K5^dn94X5?TWB7pQ}i6uLKD(-s4>)rgN!/=?[M(u.uZGi%BcDm(iT$",
    "C=)ZL:]xF]tYRCVIv*rhQ*Y@WN9(zQNbsF$Sn+6A>NiG=L?hLvFlp?]&gEx*J*=JL*:$lzPLcr#oE(gq82F-{nsEPj:$IU=N4{Px",
    "&9YFD5B*q]6JB27!dU>DsVN@]cRCh?}*lHGpp$g7dGF>F:xORQ[1buGAdk.*rY/oNrno*NyvpYb:p#t3w^Xgz($@pR=0fc&O.w7%",
    "gx=b?iq&wgoQMTeMIvn?Icq(3C8P!vg>L]]%1i-NnztG81tIF>u<zU7/JB-Th3@Lh.UBwHcof1dSMB5hq!E!$lEcrdO4h(?UwQRU",
    "y$WHPV>pc/C3H4<}i#xpwK7gEtc:eJchc{[:c3M*JI&O:sd385AD0eo(@*:f+STuptcW[p.y$Bxr6n<EWPwWakQ!z1Q!:xw=(QQ$",
    "=#yc&aE{InZkPG-EX8?rZ^[X(6C2#j>Pe-%?7tbf{nSOP.ok/>=CiZFpF/$jSNBK7kQ?:Sa}k:fPah:L&[G0}^X<*)y(aQ]Any{+",
    "m-6I=mcLJ4!=R0@{x@vZA[bcxBGgo.eC>srV@P7Ff[y@vOr]kGSSy?!{9iIH0T24c?DbQ#fFp>/bVMMZm01K7w:.upeStPIQ0Zbc",
    "&%R-%9A88:Inq)1udHn.kpE%zffTc+5a6iQ5bpaSB^&$bmm)UCR)lE&{.YL+dVv(J:325qs#6[SYs>ZqFkh?3kPs:Sr9ep%UVg}w",
    "l[NBZ+*N9o.vrq75k]L>{8h7q2<O}jg)q(ww]=&[Nd0zQS2pvM06T-4ui.xj6YQ2]+z8JJ>%50d*{eN#*HkH[6oofb-o*ae6iec@",
    "Zf).H8Dia*OiFX<PRYT&47qiXa-!X!Q$z?ktO#+tvcmTajoM])SZb{:>S%*l8dAe1{1[9e[TKm9N=$9(}E?g?&0aA?&j4HO0q6+.",
    "3GcL17mB6]9/=EGjLZAePYj])-zO9F?Ds%kxb#&x6w1D(YKg]SpBu=r*@D+n@3t-mnZff98=XG7mA)2usbW)doMCUENpKBMDh]xT",
    ".wxw*0a]m{EJtKN5y=cw^VH1Z>>QJbKN<2D-n$QAvJ?4209*Zd+g3y7m0>0d7]H6v42kcLw)K*/=E<Ba}h[Jr/9@?oe66$tWR]x:",
    "KJGjti>w[ocpyFsj7N->D%PW.kMT&%nfZ19lE7MN1Qtk}[0LpY*g.Is&]kwG-z#B&2N#6)E0[myC8?G]D0=$5ekO1jBY%I.!#7#1",
    "m&6(x9i8y&xV:O[fPe1QLJT0%SP[Stu/mIs)8EynGeEvb9Pjj^vaX:GF$xP/c4b%fAjeF7l+lNo<q#2#nX=pUu%Ob)8*dMiFcP/4",
    "E+xoo:+8SqJ8KG6PAs&1Ja]Si[lw)PPY7U/98kRf@7VVzAfLKUU-+2f9?@IA9n0}F/Wyqyy-8}g&.ky@C=mKMsvYv75upw}J##4#",
    "b)#AtkPe#N>Fq+-x2cO]CZ&TP4l9#yX>:OmAQD*&H$5(D9kzS?Fyi!#Fxh^]FO+MOEK/7]T(/{MMWgH3x<!keFspdm[vU+-[A^^2",
    "7X9o]B=2rB9ZeBA4/@-z5g7%LQ!5i.)ROi49]JCH!!^TmhGUd@TU*H0EPxW^1L2fsOdZYxR4FM$2C-Ia2qkHdyb!Kz}e?rNS@jUn",
    "[#gGVBbFvcz86$Zu[8<O-vNxt1%Pl[o1(<&o%![6bWnbG9mZ(Gng1p0SZtjGLW?wtbQ2:)CQTJzigYm}dc?tQ9q/N}<D*0j2V-cj",
    "&9rlA=#u!ksqF3Lz%k0H*XXVEhlCg*+NDM}e{TY=x/!vs6bxDs8E]kH{bXtHX*nicJqY$zDC^Fg0aOKjp(GV.p3[E.=v0<GMD[LN",
    "t!(cdGT-[M/13*O^TY3Avv4/{Bjw^Nmz/8qJS$e1xxyK7)4o3C2-5e!GT?yL+cJJX7jJT49[8n@Gpme2WJh#[K]@hao:vAah880[",
    "t68!?lxT8/fB=k?2$jeA=@tCcS^+7l@MhmRf6Q5i1anxTHH8e/]n**Mwz!3k($#eNdNFZ1S0!?L-:EW!BB)mJ>Up(^&6+<N:qPW[",
    "de#VF>-O3v0gltv+6SV(edT+OQ5{DuSdwS}>xGoR%kj$.[@tun)Q5b(zap5e]oy#x<N/Z<f7o1T)I#2nS*BbROj{pMo0B!#ZaK}0",
    "]GCh@^I}dcklmio{HR94X<uXpZb>7k5m%OdA7}H*{W$-62jplWlg9F<{7+[p*@ieOTznP+k>je5kNR[Jw5#$Uxs}wgqUgyirC4Si",
    "O>+ndv].)cTSTaCB8)g4L1F>u-+UZ/T(hx9hp%-G.u!EJ3)C+!Jh?(Jku4%6W:+b^rw{bQ>Vl9AK+8x5u@G]+c&%rg/JObmWWA&5",
    "2*q+K-b[U=O@I}%xTv.3i)3J$k=tNljy*MqkZ^#v[lmFbZ.4A@V%:[p1T6o$7}SwMkkk]bOZ/%rDcc4Ez8}FQ1JC5!3YE[zLxRL<",
    ">YAi9bT[SAH?>^dP.GG?kc#$aXaUG!w3iqO/Iw83j$WtVu&R{]7=KdZkS:UiH.&JzPIRk)&mCoErDBD?+&EM>n#f?E[zA#Nnjl/F",
    "Rj(m0P{8}0ipZj=lyF0u^-fA)5:(He7#1WBsrimjcx<lH5)+n}rKX}<r748c2jQ9P27IW$>H@koPal(Ax7pU<^OeKYW?!3*wL]zu",
    "ipjn?7^bt(Alsb4&?}R@7Z*g6IC9SQ0cAX2Sj[[p<IU9jb.M0c(%i]gcvA/@HZ@&nXcN@7?RDmoP?I4v=WVG?1b4NR(vzU]5(Ck+",
    ".kc3M33UvwPJWL>EQgZwoqEPXW8[VFatbdR5oaEeyw*BZZp#+zq[1f*uYh!k-Q]?nj%YEcxK6aJS.G@==GRgqbAFiOIYQ>t-}eET",
    "IQEqzy[M3^KEOZ4-i+PskI2$I%i}m/mj3nzT3!i%?wTFG*[t@RTyBl6UY7O@97vXbys{?zCjW)X^yQ7aGZOT#g!?{M/LWO2=:<cv",
    "alm^.e8+#<6x&W:nF4ZXnu]kebYB.^NcAl^k^R$6v4}IK1H4FYw)*!nv9^fjk=Hx/>fwT[!&86(YDmUEihb1*7!vVuS2n)OU(OM5",
    "][ba=mYM9{hNO/lRY]@(WtxM2s@83urYD?/9:Jww80PNlvFpQ5JZl=gr9wa(ddU.F6hsJKtEjS0HwFbqPUg}3O0FN/M@.zyZ0UK%",
    "^R.&Ai}[H}V{BD2@=%eXADgRl2!O6&iD!)@zy2rDFuxev&8+H]1[mR!1p6Dv+oY>D.pUu)D+}r$w}#Nl6B5LvGQ@Sq)ZHYWd#QC>",
    "L=Sv$Ce1W83b33@ru$FQ9caUmiDLItTre#R3VV8bmktDsox>4>cXW1k{@B{F2O^F8?!4FMM7.RIDE*PV60zTaCuTJ2aA(c]M?xx4",
    "wz:dCkIY0%EVc0BK}o&uCxE=pRb8a}ZEGGZ9+f/okW[lpeJBDT7+S&)y/rbNm^{n*fM#tq}Am6TXpXlOFrp8Jl^V%?VkSB1qojIR",
    "LKj)OXn[AeWzDokYA?rECY#2f.zqP+IjE!.<lfu7g*$vq/!k=Txs=UTviYnBPgC?y30fqO*4#(9g[ZQa)N+BiGC[vmdwqR.cX.pT",
    "@wQrihrnsj^8EJF&5*0-CufX^mag}B6y@=H3>b&]mBW+?3:kjE.#4$EnVYr.ke4K>mU@?k1cHkVtI+f1A-=zSXPN70xT7@dC1{e)",
    "y3=!L{-=/Mpvc:$ou4(l&b{DYAg.ztjyV(88sFOf8Nt!}q+V7i?a/Mn}KE1rVn#zRss6fBd%Brzabb59/R}=.0pC4{^8$Ajo0J]X",
    "tMq4yiY:aK{bos>WKL&KMRU1MDz#.lmZQdGqCYJ<tLVVgUh$6@ERuHg0ItxorOZhedbMX)Tp=F!sN!mhmnnfzkHKZs^ArMA2U$B:",
    "jgj-BxBl:-kI40(oFF^@SnJg*E/^by:(r&xu@ZQGmi:g]^(AyIDiOPc.cOd]^<am[WAO#<O$ZLB7pEtUt=F2)@)}ckK21bRz-^8A",
    "mnsYkka<?$(a(8*O<8N7gJhFuW0(/7Vw=BWQ#cWo*-.qlN9BbydpG8q*XU?J(r!M}P(5<NPRMwqgCY@-CEwkIjNOv%d%nqmy7%91",
    "B7b.A:ZZpz{/3cnTFw9BDN2gEU<)=qFG2<#/.zd73B}[9*p:vD}u^*0lAIK%JQ/4(u/xi)jqB{h4e*/5s?:Eu:frU0v/VM]0B(FJ",
    "<e<1V!j6:ca]4x/dqS.ioNPZHE(:gmyOW7QzGQ#M2$r-m7O{W!K@&h*/:rqz}!V{e^zcd8}QTW<UUjk]vZIs.=Y5sD.S>!#9g8<L",
    "E&tEN]>ro%{lODpOv@k)sTu.O7ym^&b}Pwp3La@8-@P^odHZw.@s1JDTy9XAi#{jyxLf&H=Oy)3:P*AYlv2$5K]n-MgKTX==t:-p",
    "y4mO8EsAn4b%g81/2kV)udE{@pU.UTg1NBeP3glG<SJ%7s^kpqAV3=0C@A:Sh-S(Z4R>)Oye$:bPg?BjRV-UR1Wn8jfoeGm]L.pO",
    "hynO>x3zZni5r]+N^1-OptMdrr.Wr)b090xa4zg5<+-j+UQ8R1E&[3<I9[Wm3lajNmS@]qopaOIWMFd=9WX6Osl=RBSeu<7kkE^Z",
    "@Yv}W@7Sno^eJmxqvNk4YGlFm!gcFxxF-G0NenK2TjS0qN!?oxOaLaw:MQ%-<nzRm<eGP5P38F{z0QYa:eZBG/JuneByUXwxA%<o",
    "VDOQ$0cHN&pQ*Iv<xKgFapuMgS}MCa4%<j0))!qht>LcXfITpnDozf?f-bB]u*mQ7}w?*+[=r8YcNa+!OX7^-&0lN2SaMJ65&FpO",
    "@0fvq@*!+S8z!]/GWGGSZ:zGj+R^OSL[BnlnlUZIOPgZA&gguwCH$nb!ve.$z0ASpicVf44$kDZhqIYP/@O#6nk*[IR.%pA+bmUw",
    "rf!)]vbUUfSe:Uqu!nV0Nm8wnznKYkl[OXR&**Gw^@kEN!*8].K*^Ot8:lOL!mrveRgw>n}VF@zgl&hhNq=MEl6c4HCjaF86$a/u",
    "!:6DNc(UpZTmS.Xm-z&1rkRnv/VG&2zG@-R(&5j{upCajug&6G^rE)?UFgdMB0fhIxz:XLoE+]-&j[!B)(ug/0R#4AVl<fDrkBKc",
    "B^J6uKc<m/*l33XV74gClq#^PwD(kIkIps{3dCMXUzODa?cCW$c*:U)&wD-YOA1I3w7oc]nv59a}m*D7dRL0V-0f)r/]gJ%ju:xv",
    "rWOOBpfzZ6D>NNn^#@}bm-@b5k=6A6E1S9C}nsQy:n7].5=PbI<{Jlzv=2G}e(X#q1+n:0-guFk1c(tspkCaEVcq]7&8VZrJ38B&",
    "c3&!e}XLg8s:ioEBhn2z]^#W%:g0Y:oF4[Y+kD1/p(vuFaSJ+VGbzej+@^ZAEH)Wndfchdan5HBIPAK@TSxF^iJdi!PDL]2ivu*J",
    "j$9+Hg?J=2lBGMKy7[&P}Rs4[sT&@8JyLebvUygj=NBaMpE3*3UoL{s=qsUSIr4V0?VHs2j2-Xy9Scvf><JVdJ^({H[d)dXrVUa2",
    "IMQc}YF<@CcsVj5@OyvlPhNZkMU=**Tc)#*6I3zhp#]LU8@#MlC2A/MZT(gyuZ:!OZ]!!zz*[E{t^Dr4M$$S)&tN5bG}60{j1<F{",
    "}WVYvaWdLyrGXm:m3HiQ:F4JQ[*s5[0?baZ[jK3KDpa@2^6NBHSBP+TZ=lc]ecuG#Wjrxl=HxjBZ:qsX+lF=@Q9z>o{L3NHjS1Wy",
    "u5ANmGW(LP4]&?FEesU>y$oSvUFD%DP1&@wX?9]@mq[%nKD!-!gF%#i@pX4v/cEjST)zlmi<AbcDmqYPGLt%1<YWp29BLB4E?g<d",
    "t^ecg?yd>QrLkj>?-?Pme!{Pljsc<*O69O4l*P)#}@kdI4u!Sx1hLmb5h43Fbj$Z{u3a&m6Zp@FWcsT7Rf&Cg>ntCSQpdc^P6nB5",
    "urA^.sQw-P@[%S^Z>-{(*wqBBhlyb$[!5z%D1.:)D%t2ZbVCBvkCwy$xs(WcIMYAhE3N{GvTewBRW5y0[n:]f!LkID{ql*xuk^Du",
    "g/p&$YUnzI[{=53PAOsVU?%K=ie1JH3)Qr^Qo>p(N.n*q!v:ewAYk??sK}mKc$uL@Z{GB<@9lwqHZ>0fC-f)h&?mKf[q9Y1EdZHN",
    "TxsmvfFKiOA0WQHA3R<7nwtc*F@T81y@GmnYA6GCr?hm+k:P8A01G=a9m^f+zW&8E0qilMCE:LkBxjz+TIq9Pj83GmeMf*aD.DRp",
    "r#aa^*!oZ(!aNB*Ge7//S<:d=f9a*i)N7f(mzfr$+VQjA1A=^&0V{L*FXH[xJ@*E*JkQ09[OOuwcj3&&wv^::A=>%cmpqAB-dhR=",
    "2&Bt#mgxdex-2IbL=Q-CrGS52F@f(G.D5AQ:sNLelBRFdif&O=3jkO6SEqPD:k7ap%gTx7Eqld6d{>!gS0tpCM35+k{gWC?J<HQR",
    "fC=aG}4VaTDvh6]J^-p?X3b8gHJFA*9TsF=O&?Xu0U3l=OF22q*}eFc>(-!9X<i78XnLhdHJ<>7bfP%G@QmUqp-KKtaoicu&uP8J",
    "I7Ixm-qSs(75.<Xz#KsZ0B@ax9R{Qpb/?(cRlnK+@{AN}R(dzA-3AeZ9Jj-Q]<&winzLEN8ukyNux!QbEPu/&p6Ig(MUfw-]#vRb",
    "MwbyL=%Ueh1?4w[Q)QL.>:-W6BOLWBo-Egr(=8JgO]E@FXc+wZqj/)kN2dU5@X$>hakKm^p.wNDQ&J6u7/NlyA2^PB<@wfJYR[ok",
    "/Wtz^ueWX9KP3PvV:IIJ[B2K7!KM=9!iis=TiWF69e?i/HH#62rn^U/-D/bgOX#%vjiyqZD!S.SQ:dDOT(a9fhRahdDjOrm7Gm1>",
    "{(/3#UQM#[NpNqGsPV#l1Cy.SflBM:Zt11GG3wtITtYjO-XNlJ!CZn#rlOc4T&&=s}BJJTY)isodn:6Uv{b0VAxd2Z<gnqQOdD/b",
    "PnKKI]zi?#pbmQs(X:Agd=3jJ/#NkUoXH:PR!9JOeEiK:c*S7)KLxE><yQ5e81hySUW!iRau(Tn1q+4KZ+unht:gNr+Y)g5E:]2]",
    "TuSF[Q}B<-^9UVMnqRvIZ-oF&[rez^g8T<>kOd+CH1.5vZ?hs-(CYX+fymmNrp?&T?}8p2.EFtmxcSr$+$)aT<u<r?yqL/>FWsZR",
    "go+{=qj$28Rs8<D[Ozn&Rxq9/iRkH0h3NURy7kzK>XQBnd2JBpRD.AY.DA8pFSGSsx5=X3ri&C^rYK0<l7h<U<pilCO0Ha*aAN3n",
    "WG7nf8AfjYf1kr7th&@7Mqc)j]RI{UVwVsR01pf!sdXV=6OPq@?ZTguKrY#MkE{}X0:^7:[%tq0NV!QKr85iksxw}A.RX#&Xf^(Z",
    "I0HY$u)dqu@+b(TsV!<YW<:K7f3XjCzl%)sI(v@ntKfA{7(6&W/)e@GCDV(/}=e/)",
);

                const ENCODED_SMALL_FC64_M6_U1: &str = concat!(
    "bo1whIA(W9>pNgSBTvd67{ovwkz^N$WW<Cijz)K1!t/Z=Q@Vy%u%u^EGmT7!x}YW.27zd/A7tTBQ*W}1z(?bBf@GsXUI<B-we3dG",
    "xk1Xnf4k->+fbV9V)SWMRNDN&[^77m6@qZeG)])ZO$}Jksm&KQi>]-1jU1%{z<JgviiC[qI5W{bN#5:Xmx1x(0)7*k:6x=JiY[MH",
    "76OG^J?6(EG$l+qm!t7(OoSQF0@@lZNQ*}i9<8+iW4o/L&by[zZ.kfVti}P?e!K:tFmWxD[oTrirjw2NxWS$AGEyGDxG}V))>+Uv",
    "pW}EFQ2z(YGtWjyz%&?88AD3P8?EK)@zynw)0v8y*Xy#^50V}7r77H+H]tyd)Srb9[Z:$U0m%v%&>Ux6F{y9Om1yW*C^:.qUNB1l",
    "e-Ao.<G:@ZoH3h5-Su32!.(oDwea7qY2*v6tctj}5-{eU4TjzQO{.Lu3G]&0v2)YrbA4WEmg2ev2=Oy#2sU&eWnJ{oF]pX6Uf7Dw",
    "f!<PW8prunX2Q(tOcm9I]Sk189dmW$m$ui81bv/dyh6@^3.UWa0QVF(?@FjU!NZ?QXx3Csy[!{je]clQ+Z)ZC^+}b*Z(EtoJ0(F}",
    "{$J+IsNXEpSZCKOf$3R^-]#J%{Vd8)kB!AgjXy@4Cx(}Ec?iRs=C(6ZBr=EMbnYsL7^j[k5qclhPNL}moD<>Uy.ra1T/v{Qt^Orh",
    "NgZh@tU*IDmM6ktgw[&0MDWlb[7A{/<.pB3>TtbbphOYEaYTB-:C}yR=##cDL^v9%[!E(ZWA1s>!g1.qNDAnq</y?ZbDNa(QMpK2",
    "23t}fx-AZ#H0>^?-5eY3(njF-+gnElg]In8B^j?$--9Hj(W).J6eAm]92:stHGFyQC$&#QTlA5o!yg@<-@(TYQ&Yr]<z1#:5xO3q",
    "Fd]u56L.ewsM#>Hwxhkz]BmiKD[W:$xO>@^hTQ(nQ8*{gU($[aadDrBhQ)pyVDs&kGF=%%HR!6)uo$:bR9FPOfcHJ.^6%a?hMice",
    "15C%X<M7{8^JYw/Kh^!OP*Pg1t4Oa8Y7t)l93y6JwHG^R6cc6}G+z!Bq?m.bnVU=Kiz/vWxNfIsPT8u{ju&:cn#^UfB:a0Lmvj9s",
    "hzvuVXkPT}<!D9A0fxf6L@n?gRQ[:gvL^ggkFg}%&1Ibl4%rmVWSDpGO.>kDPOSfK:ZlCW[eYwd4VcJoQR?HylLWW9+]%XZ0Hhd$",
    "zMsHo/O:VhZ]}xl/L]lKr?(6CaMm#n{Yfx/ScUbI)b!Ko5vK++aL2ZtWF9[tN)]=VZ:hMm?Xl})-!D-zx@NDjuL63D10j.X!/uXk",
    "4U$/-I%&:wsAkv]7u8KidncK3Y/+%Jh^1-@gSAtbMdmC2K]xL*Ji7L)swDPy(Y%@JJ0rE%XvjdN[hEbO-{8J@KK8DskT8cV{Yavd",
    "wZoE2L3Ycu0F3y}En}8}9)V:SwJI>kc-{aP6]iQqhqxGt/<35v>0A*}]dhpJYi^9Kq=.?*7D@u)@D.2E0q==e>eu/4KnT%D.exPx",
    "g%#Z[F$6lZcWK1GE8qeYJ8)FIH!u5Tih(segwocVD[:o/r^sTcd8Fr:Z$Rq<YH}[h]A!ydv0J!Qh+0RSTC*<Rq&WIk&OiOfxSG!2",
    "z[IS7UmnfdW&z}wy5vD:c+[j}?w=.-(A=Kmy?!BjSddD4I$:{0B-S2C]DWILQQ2iWfMU7A0e#VALQ]-@DQ!NSeXzGOV}ktKS@iTB",
    "vV}P*G&isY1sL=-5B%LFPIqJ2!mX0T61n-w9n9>B>EU[3^I(SdH=3P[^D:#H61lx8oeKaw9w0mm-6kn9n=b*b-I&jrS@wXQ}p<7P",
    "DY(ld.pNs5nfv7V!P!!(hyv9uI{g$epmwNNi.ounkBmf&tab4Dp#--.M#vX(jsI<Bk5ZlWzH*C7{<g=eu&<GzZ#A)b+j.1i*lGQ?",
    "G(MSC][9#y4</&wUK[CKix]C=@Bz(<@Y?afnYYyxz4m1ruKz6wKGUZ#FkiNoZzB8!Krm3b/u*Y:VC!uc0JY}E*xVv]x3(jzE-nn0",
    "xYIxv(S2gyPY.!bA{mRS(*dY^rW4{+vd%jPM&@NmJd8c#ZIv<M)%&>J<X<k:Y**To+bMZJ]%U(6kgW?r0wr.4T0C^1sU$g.%h{+y",
    "<Pn%MmQ:7}!DE.xQKJb7gCI>waONSO]4MI{it.O9Xvp9NS>^*X.An%Ns3E?vUHQw2s{BJge/{y[H*!Q(ssKY(m9ds1p.hc<id/7h",
    "egpV+TztNlYV#qC*}n.jB+b#BCXiKgU]L^rB]<5b^2zhw8VmVtYBu5GT4=B7%aS2q6siw9fX2nFqGZN7Da3HipaX(Frdf+us1#wx",
    "{CAO}80hdsZ-/)zG7#c8d+qT5H::oaF-.8qusfc:g>E?Mmd1nykT#RNRM42q-LXPoryfbqgsJ/$&{2z:}RGf7uoWvigmw@>Xjdu1",
    "F(H5U5-A{T>C7^m&:[YA1b%/O[m4FxR@LI8}h/Eba8)(n)6wc8zzdLzy*6cr}7Cr3{8@/!tQVBiZitJ0zZSDo+7keTIBry^CN0ct",
    "&44!<Fs@ar]8FN1N2yh(01QA?2<pFnPZ{HyyY>zngS#7ETB*sj@uYj)BN*-[qdo8WmU37NQoMNs<%4(fmK[H6wW2sIw$[GLN2Al5",
    "Jzf$#AJoeMNgupe]D9kfW3+mmy^={?zIWWyx.71x&W/@0)hJ5/06?jh=C{}*c*rMJ8Fx<Gt7bl<Z)/vcz=S&2+Z*{3x(.S:eovw{",
    "WuCX@9fan!p.TNa-]su*kqn&}m+GsRq@$H3<q)?P5JE-(cK!-(>C5OLQydO]wLdG=[+6sNOF.)Iu?#9kXcJUVXSw10ikB@AKCB67",
    "cs9rdy<krX5/5%g{ZB.@0[Jk(r}tPu{?CgtKJYanuk!<qfW0tb?RBKiIsk*Q>3<fh}XY%V2I@p>Hfr@VMnW.=h4LB9f*If<tk7VN",
    "vp%dnxAcQFLjbD+KA7xm36c&9:Qj0u>B>wVZ}jc>caw7Xj0R5B2dGe%=}9s.h>I6M@QoS@&kisyM8c{.1hBVB/HGt-@o1N6t-xia",
    "odC6Ew]XW2mt>bdE:c0E.#dwm1Is:#4Jk:th3YCW))&s2KjQwz>x+9fRVbvk=k[q@th[c%6+IDIr>g/n5(>4b7lKA^}1a%bL0cj^",
    "j<Yw9Fo/:f-HVk9@MM-NMzGZb*i}abats/#aJS@.-m0IQ6XbT=:xJ7oZ!koKZg2.U0bT.S5w.gae1Rm7N]MHL+y%a.!@fppa$USp",
    "^h+G=Dp2.pSRM9Egm^!}IZ/e#N9:hyvdn#=k&>OIPF#5zS4^8w-rf75s$hervGyvfQ*jbq[z:!/fLMSZS8sc&Ypx^4RP$Nw+*9Mc",
    "/D{415Q9@Z5te3@9ijv#>M-U38>knsygq=T1w&n-7te#cTmGvPxa7fwDj2S{VEhxeZgbbBIp7/O3h1fVF3v0S*IEfUgb#E1uBe(U",
    "!+&=q={kxe!jc/dp]xk+y1yu*m.^njteIgv^vQ}jDqYd82:-K]mYJmERP#+oL-}@S5i7&DM@Q&kwo&yjJV3kcinL9c[?YZEYD6p4",
    "jDJ=g^@%iMTfw3R&keQiEJ9w(hD!6-)ui.#hXOCuX<P-o2L1xFKp@G{04E4[muMK>FAP>AX{}l7q4cQwJavA3S(Ya>rT=1w+Rw&P",
    ".q/ZK)oaf!9hTmW8DV4j.!l.E^%]R9YNDIp}yo1*zGmZ1.H0K(>]rbPiIek)Za8gT^AmGYP3ET9FP?yk>m>J>kz9sSHq<AW@662J",
    "IjO@PEBT9nT!=6j-%-.ELy?gzza]TDLy&?CegrJ)qps.)e-iV[h4<N=:XN:RG[?4&9e<B4A52bvHmmXD:rd(NvXw@G48C-MYKY9%",
    "0agTB=<MCU5[fJTLQPi*=sfg8n:/P>6}jc@X(!<Ar=XFU[/AB%.n3EwN3bXb8%n#?QgB8kv5%5:6V=fBC&B&Dz>Wj}okz5IKrJvw",
    "5=icNnJGHbLei5t!i71Htc*:.A2UDkU/2#[Pt2Fib(ZL3W!x)q!q.ve0gT#<7&vCVm!iSx11i$dyoSNU*E7}Kj^b7[u8e#lT3e{m",
    "guF)aU/2!uU^A^WKA[n$v7hy:Qg?+k4S<zrj</(+4[tUxCbq/l.}9^e:ss72Nq/Bz@$lRvdMVYP7GbJwQEwX8b^@y&C<#+pt3ZZj",
    "x.wspV[[7jC.<!X0?n<CurVNa+:WUa3F1ZX=PewCX?yn2&ePXexJSl!&K9U<y<mD]LavJzbpFjdcyzYZYV0KS2@+1g^7taEyo>Gn",
    "j>EzhZJ$e*ktnb>KzYq8pqyE8?rf*iNHbs]?1lB^cG?.G{CPVDNJ&DUYXb22m&.K>3^YW5ggOM(^7mjWx?teksfAEOwmzn{X[=&f",
    "O{=<p{(aNL1/22d{PJymYqd2wF4wg./k}M!2Z=>s=DQL@d&I%i=1lJgH?H5bDkIBIpY0jp]2^ukfTSt)dY5ux?28Df3[jk20{ZmR",
    ")!QrOVF>ZA[q(Iup>Og0Fp{uMBMW8CEXcqEPYl!ISg58%/HTJ8)6]tA:25JmABsL+iP#ymF&LA%B.A$OXYa==P7xYW!KYnAx=&kD",
    "JoKtaB({?gaK8WPaAD{jT[0w[aQDYkzYyk<FdF[GP?*GUapnd:oBi>}c}pFO>FvtkrQE/qdhhLt}2DTwf48U&*o:w./S@32Kxy2B",
    "?><:xhMjTcFzVXQ(V[P7h$ven]Q/y3n!w6^xH]D>WZ@m%eXzt3JA55611>#:yxB]9hL2N+Um4qvbl=HRv&bea0RBC}VGF}:7TQ5.",
    "Q-W@BPlhf!jfjo3h$z}?3vmH>Af0-bzx^9l]G!OL49ZQ(6Gc%{nwK)Ypv$OInv(gwFq>a6!jy6iTJiPsX5OcO-)u6BAAxcM4Q9kP",
    "dEuxkJ]f<#ZcS=4Q)JTBcg?qNsg$#O:OB-a@4^YNw2(-B4vcg>7iUJ$IsynMEMO-oXIf[)4ab5rvavxL+(c58KdRR0H=^:Zfz+c1",
    "zCC!%=EOq[NG-TM(%miopeRnf0&7E.CvM!0WZfbIOFMYp6awcUymSwSCHK%L?H%iqZjXQiCq$]4:]d)CG+&1F6jYOaE*8dKez}{s",
    ".FweAND@Y)I2<lZ>)ligL3({88Puj07?6[!{)aofZA7&/7i!SG)+097M>$onHroDU[t%U)F<NKw!AnK0(w%MgY6Vc6A43{.=np8n",
    "3KE/+TRX]s*+t->.viq8^DIEW))d&8!VBIqFYUslGjb@!95o-x2RHS>N>a>fRYS376t-!z6o<z0MW7!VxEXP%HltO37apDwM^!n-",
    "!EAS-[w1Re3b!75=^b&We#fQMtlk{glFqD7QbWXqnYiK3j@oi)rc0=c:O^)d(?X^/<<8srjr4J!]+{&ADU/AuudQ<CdLvR.OnZtH",
    "&.cw!]W}LP3N/gxDk65!l/cPXCCm[$bG0S$.eWNWo5*)On$fb2fGRFkMhNEujX9D8v6b.TSKB=I9pBC5b3VOiiWRwJpNiE]D<u4M",
    "snwVaHqsJ!7tsZhh#%vIV6f(9:/3myF[<U^eO$kB(^d4zxmZ@k4a^2zQ^h:To.{t7=8SJYM1AVMjv$&5@pjC-DavL3JqH]k43-qa",
    "pxX6?%nu$3iW<a7q9Of%xjiXZ5Obih=j{%PEQLjHAXlm-!9ZXV5&D=66qyn@k(4lto./X)4c7<i@{2w7nUXbR2)t%@PisxwnshSP",
    "jkBkSwHSZ7HOj)^^kA+]3I{12n@kX{SJ]+)AB>(Su-2OwA3pDf[?h{=YVnkU!l4mn*Ibb15uV{oV*Vg411ejP4JM@YiN<Cfc4kZ9",
    "H:XSOB{Gh6sdLag7E!-}Fno9/e>{x2..^0f->XH:.#TO^O!r?zr[RaHY{NXu4!GLgG99{zVa?!Nf]Rsg6NGmj]k4z<(=wh3Uz+}m",
    "4RntFqxrzkXw3e1<fvo2hGVz(DX{$0H&X[D+p15!FvUD!PTyr6O*#Og:FP55ieO-<5uy]9rX=!K>:cXgg)oD7OzDJBU>=UV<n*8Y",
    "l]qqm03Z<AqQftE[kw%yTkU=@4YJdoC[Do8Dm1UoG)=t5A-upc1>CGz8%R=lR%^A}<Af4!g*QZbLOHdhRLPu/AS3[u(sfC21s-#R",
    "7M(nwp-o9)HF?S@1ZoLws.*Ty]l1=YY7@UBv4mFopdE#wC&FIiu+g3!D:D$<8YYB5LPIc:I2hb8ZfnZ[Kty#w4#Akd(G?$y5pSX7",
    "aZKizU:b!-4]O7{XcbC/AnXn<MciM0!Zus*ff8Xt)YWV[?<Ybf8FUwF1E%+KLz^pwZ.PE7oIQ*nTpw<Ra4spnblpYR[pdk$VhppW",
    "V0Q:1VR<xcZbwu)OB^4IN$a}Y6}#g52^9>+Rh(Xi1.Tn/I#$6lk>9I>Wgm]8sVno6[@G81c+u(L-)Tore>-pW8/uVW[JoAc*<5+g",
    "mi1lGk#eaT^h@#=JY4oVl88E:}M:0.PCV(RsJ$P]YgZ5R!nZ^piE5e1*Jxg69E0r6x1Z7o@-j[tU9(-jl[OONK2:*o^[9]Yz(<Tn",
    "FlmB/:oJ9^VuTQc+:1bJB43*dR7XaF?dD/6OY4D<mY@]c05y{{Lc{6eg+u6#}?0ngyi>4!}(8Rvhj=D+25V=wiRZObwkhOTf/*TB",
    "EAKT+Tri2a(%GYPXN-ArQbip)nn($</lY+DuOu6aAupGh9HnouF5Uo#BF2$O4w%oua[Q:Thw%m)+s(lQ[f57JF?}Qmj8GQk01//b",
    "<OT5j9vMGN-7(SiFUYUpjdJZ)iVT{9>FMe<jXDs25!tI^^#UGZkG[$#C.6o!ODBd3ZK(XOuUQPC5JH:(1tK@h@Kq?(LiwP57HGbm",
    "C%^YTtbqS><Bk1XFdetrkZOLQ8Od69&N@(FGU-wR>0ZG9as#/(V3EUdLiC9yZ+vA71@Xmv7HQaK(0hivSH=t*oNQFoAh-j!x=[rU",
    ":{{$QVXt8o49i&XxMs6=pNjdc{.m*lbR?tw.L1F7u#3onf[PV{Cqzd>-Oj[nk2BRW60G!FgZ1KTxo5U0s>iHmJ]A^DiBf[x:kNT[",
    "Ca9x:gf3s$AVwWPg{dcH:sf3X??lwwTRbwi*4)&{.^F@%+{woi{:vdECGa18[m5^/2([RLY2CEGmDTP.2<[U9L06*HE{eXZ-M[#T",
    "5A}0A2@[6Y<rIHORZ+>kqaV8El3nm4E]Uys+)HR/VrGBPvI?.KTO#ejsRjj*JjkYFv5$().4[fb?8*}o0mcFjnu:HaPE-ZH)GMMK",
    "[%bd%*%OB3rwGK@Ra9Tj/[I]eX*s#5eCLBxx##]fPSJmFeyYXz7O8OXWXlZnJgfdEPiH6+{84XYzV/F(e5v4PfPO06!cERuRtzh@",
    "IlNu2]yBPGm@90aBz.QLzG=HM?8M3gHK:Xmrw6izIv&AzzmJe3mKRrKTo+:/6FyDua+>Xl-Sj/dwCdZ>9+VIbKDoNUJe/E6*p{fx",
    "U=p^rmc&ErxC-Xx97QNNQ^A{3r1onHk:6Te.D{CgkUx/{D(g?Qg=]-(FCPnT+[&gHuzFBKS{c$agvI(&z9Ht2s6%H((>basSqHOF",
    "1yUH#.56C<zYL#zP&pl<aD>UDEj+K+9R1.CRt}#dO7r24CK=@(%9r[y*X71#&+#SC46ZTlIKUFEA<HCkfR1MAP4V}6bk#dqtoF!^",
    "2tQn<@J4?rs9c*A^y6fw7>2h]A6]2!9(EoR7pye94.2=TqsW5/j&77h=@8eNezaNG.]8ynZ)Z#WPbiu$PqRX4/c}!9S?qsn<@C5F",
    "w1<4sz7!KC[}@+$k<GTU6SSs&q:?u:AQK&gv>CLY4I.C%pQNz<VBrQ[O24yA0fpLLV-y-5Qmno#.&w(N.4])i2>o#uYO9V9fw:]o",
    "QG^=iI]FG9KMSEMVm$EP5ql9ciWGP}F*:ydCa:@hs-bL1DwpYELKjsx7vMsUHfAH#Q[8Ou>4KxIJN4U&YSG<I7<C^tgAGdMCFhY)",
    "6Z]UFYfBXbIg#5z2<Q6RPD@$&a{w7Za66F#idWX:=%[Yxf[?Q2.K39wpj=MV0A^MfG9+I>y(WO&Zx7*ST]qzn7eXqI(+3]Blk&0J",
    "z.d0:l!WX!r=&I#x]ZL7@=U+m>^lI=6/Z3AVEogBEAD1.aL!xVAVHi66gZcU4U[f2c$ND.d8Ad56P&EgJRtuBpLy?[wCBY*Pg}pf",
    "@)y5Bl}GeGMHx)GV[rT*v[15A^O:I/{eII+aWAj}QuN-NoKhOJ{b^&u3rn:[U>LliNpS0O63}aLk3zsc/C2Eyb90ORVY#K{-9h)#",
    "V$f$n}ulr3XBjJ58&dk6[f&(L-daF]DiT%C&[+P/.A/iQj+12bf7u-&i1+U[fWO]AbR.kwPA!#v3AdWMk:9Xh@oP]^b-&Z7aN+&l",
    "dSdNp0jCiQem8TDi6c]HQZDcBi=+[EpmI[WM(+q3=#VxN&*@czt?M8ti8Rrw-44B[KHCATPXjz205iDCw&JWL(s@7rH{5FB5rXiI",
    "DxH}njKm/St(vwXyfBE#ruy9.od23I2RG&aoNqpO*-p@f}?na^p-1F(J3ACV!P-<.kd1DRPeJNY=+MLz?A1/MSr&U<teL!L&]&u^",
    "7Z>2<K.0Q!A{E$:!Imj<<*>{=n=Wj3[em+eZ).n+w.qsyD(4gA+0^mk0D>w8!tFS@H$qZx9doMQ*y@LV-vwF-h(X#7NG%[FK%%M#",
    "CHRgbWWfp{<!^Qx*5dzQ&G(q-eqQk/]FUyqxAd>x.!ofpt^(*V^X>0@[25DoKZ!?+QVs)dDJDKG2VDWCFgZmMnqV/Hiw8#0>Pcwi",
    "P<d)/fQqBCtRT4eR!9?gRnLQj4*##j+haLX^Kiy9&LquOrfR[%iKDofabQ]H:PvEo/?}c2Dxx.(LaKG[<3Q#501n@({DkZ}/=X{h",
    "QIbp[Tp#X?(u&N}A14Z&5FtNrZK@vCimvSpoB#.qxhK*Q:S:J}iSsa-veCw!:AwjX!A)).l+96eSuG8v:pM80<F-%fsm>feei$<L",
    "z#)a9:Rb7D@4(*oz!d58Y&.UPw]Pik]1fAo<#n*BO[%t0U:1jYCmI*h8t]jMN3f!HPCTm:a2j$Mi^JIBM!I)jTa+^Zl)=xT:x[y1",
    "Phk*b4q/7V{UIPbAO#$]=H5wf:o1ljTj3WQ-wg%/MidGbHmS#X4dVRD8JEfH+Zbgh.Bz^kE!d6:1vp5v:cNPanGq6KUqwQgFR/u2",
    "/Q0jm2.7e:HrE/Qtn}]M2fO+=vrFb8daf+jS>V2bZ@rGuFp!4&01wcOGJ)1<MQ6Z@uxmm-@nO<j[oL<llrlaX913dsZ%%Wyf$<LH",
    "CNPS@N3a5OEa4-4^0vaN7]EQ4sFX!R[m=jyCy{O[aECX-u{.llCZx-Xgg7p2y#&Y33(l1VbPquHSIDZX.idbG<-B$Cpr[2DEP)8(",
    ".z8cRKezqc<tX5wu}53%.Nyw@C:9+yQs.MwvnP0FE0nTQtP^*@/zqyeRdM8UEX4]I8j(ZHX+*/?x(]YP9Yx@Xqah3Y5{Z0i]Rw@V",
    "awg&WC(G=/ji)1LokW]%I4SftlYPbIH}k61WT%(gNBGXGnKn$j6P5$Qe96?@YzM!n",
);

                const ENCODED_SMALL_FC64_M6_U2: &str = concat!(
    "*6>LPUL&(jldmH2+5=^WIiS-S/=V3.UZhc*/B)47-zn6HEtyOfFdC5ZE/%KGNeV.tF%I54]JepXPP[v$j[!6I(Sbh)7PKb<sxw6S",
    "{PN=jsPBswmT%ZpFkEb}Qkluwt&MHiSA0or+^.:U8wRIIBj!gsizb2?IEh]qSk$n*c1%l<&K+)GJZ#4w<)O:5K%tJwsI3yMPhiP)",
    "HwptMTgY09&TlC:dIw{Fwx=19=&=!Jjk#WwzrmDPQ%=rTSl@5!XyY}c5}!PB:A*$4r6ld6>M&?8hHLA=4R$/4:L}+k&68a)-8sVt",
    "FJfq==xjm55iEbg5tHL+Eae2bFDHS?:1W+i.MxT${fY{An/Q?]J9u$e/I#lv[1L=L5ugk]YZ*Y*r=O2ItC]-VX<z/EZq/?eXb).+",
    "nfGDCDrDVHn@[G=a]3KHG4m}9r}q(@g/)skLZ5uOB]Ylt[HiRmq7V?4wkppXE=OnSMnD&Gp!Xviu/x9&}#UcRcc0?+&zh}Q>c]R0",
    "A^r[Z>FJlw0(SN3A&{.4kz>(xzRd:$5uq.y3L4}P0egXwi/f$3Fdo34OLt0N?{O:x7@l6.aqoO[fZCKE[p:E$Z*v$(48rxp>^[{I",
    "!k8NE7$dRg7/M%rxq-p5U6>(mYY(Jgdkj%?biolC7W3Dlm&)-8EF}{l8&94U]Np}@&kU8{W3T1&A.0t%2FVhgB!x5O4eO*y&gnHZ",
    "+AxMKIDes({m*[pXdMtom:QImXQHLv^<Z}M2NLvY{US2lF==g?C4Ye+9U2Uy{*v<Hwp8hbvx>+Oo:*<iu*Ah8:p8Z{8juz@AU^T$",
    "KfF4m0K3(kDlg5s{c7/9/8gd-uf}y2{vO6(N<M@OWNT/ued*%Szdg=6EO$*iBaEyOr[vk.6TW$PkqT=Jz}+NY9ZbU0J.meKRz4<g",
    "*g^E#Z{{kM*!%SPLYE3(=)Ji:C*p1^fS.YCE6>m-oQlzQ=2GBu]XxCV{rF:Z}VOYqM#dNvKf+/G^{[I6l9i)B3-g0?7()StFyLjy",
    "==.](leJRBjiOWdWi32BQN<PY<uz][5vzzGBp9W:LkXrwCupYPMeJ3jomh:YZxDLV2aBHkS5m:wR1jz@V+h48MD=^2U+E#deqe%A",
    "jGQq2071))ljvnozoz@zrffCVWS$UitUX+wBGL+VAE)C*/+ue8^E9vnr27!lpjF[HHinkk=((HOG7obW/I?%^dxx.!tK+6A2y9:v",
    "9][&=rfAb4Z*MZ@=[JQ8Sj$kNf.8uW]4.TaQ-n%ttOY&y&bjdV3ABct?@yQ$H27JEwy=V4!:]xDbg9p6gHM0IJU<rg+si)xl^7I5",
    "*DERXzPiv4)G8AjPZ86XcwTSBV]z)njUBSn+w:2=pnxZePGjKn:{Nb>bU1@IlbbruZ[axkh.ADt>D4=FcTJ:-!$X0Z=$/(0+?Xsy",
    ":<<+hUeshLehNF[]!(}p/bWw}dB$)/=}E+fHPrRM9kU#z9]cR8i(W(apD?T&-to^-7N*dy-Q/Ul+l/:ZXMcLXNfW@E[Ydd.TW:IE",
    "ULFZwE%09LgtN{MQ$(qu-JRXy5DnYx7!OFPO<Ro@r*on<pqcfX05R%j3ky-FkY%0r26Gt%Ud}S.tJCB&/+v(d)P>:+mjVeqd^xF/",
    "pWgziLz:fs!2flfuTH)/Q(oVl7npffoK1%sp}SbTrSMC/*U<8m7@2yJ]H7EF33aFc!P*q>-(tXPQZ:]Gu.GboZusp+M@}qulTCU-",
    "I-(?TY@>kAU.X:XC.klAU9nE<fM3P})q9/RmX1$wzD%$uE^mOe[k)s!yS?vzyZ[>b{uGKTg[FRG<9E]xF?1C@-a/-AwLqRR9dd+2",
    "pVl/p[O.Nzr).:E]aYDZ)qhNhLzWgtN32=Ve]3}uBp0)?^c>>=7:{)$P4g6Ubajd#LWo#}DB&o5J0sO>>-Oa)xIJ-:Fo)?H/4)k!",
    "fW0wcC.qQ3pg14owljmz!A3XAz*bRMeUi@{ZgZlM!p>xB(Z-X(5wi@+PZt3cH7bGy4Bk9&M==XGk(@5&)]pNB5KxeO2j/alaO(+:",
    "2V[mykb1mPtY51ObscYu4YK/*d?@o5cs:+MZY%S$?7GNcFA-lgDU]a.FSDS&l$DRq1B{J^T$kz>AO93a?iYiCU-r/*P7-*Vrr4&?",
    "/:j%$fz/d<jcIxZ7)H*UJuKB$>u!lM:guZ.{u{nuS[nVi@K^tq*d#3r=PrNs=SD#86LkMMd3hS+FshM-V[dn3<{M)R2:76dy@/t:",
    "qBx]^[mnxdaz5N84v{>rc!us2fA4oJK(^K{!YcvS:)RBagM4$&1UkD)2cUl[>)9niR#+ygsro9d-b4GF}Y9KqdbaqQdr)CUd0<i4",
    "Mq6DN?%cH&Aj]C*Z{=r=9?]KzP2Geu^AP!B:Hzt+*CBI]urN1-UO?^J]B>a=7YA>^C{:E>1m:dtv92cbSY=}pnvwKZO-Ww{I{=-i",
    ">5Jjvn<zHLG6/:8:F0^<y@8vHW>ONXJ%Ie!i&%![U[yJVrfsdx&gb6Q&1@AaN/tAi8f!LjM6fz9f@MY/5YYKqtM[&P2!$w)^11Z2",
    "5zX*07*0}7sKz@rqAzSSD[12qu$$AU7>*{j*kF[y{L]M]k>FN2@xpMMe5V-#Ab-O8OjI*9*zdSt)}J6vaMg>+Ey2xQN1iEAKt@lj",
    "Si<]mLU!tw&Djz0vQxqM7F+YcPpTrugh39&)9c57t@P}&2x(DEdF8%&O}IqI+u/80wC!c[N3mtegoN9fzcXFLa4{pT204-cGpj?+",
    "Zf3Gfp1gvJwIzcqIu7BPZlponNzOFRTrL8o/MvUqe&aLjp?LCBUBa7TZ/cdOh5hQ]0s=p@I$NVBsCI!$F=@>L47S/ceG2G>P}&C/",
    "Kp0+Su{oSm:qq!d:GWcRZ?rLT/V6]6FjM[au(Fw]H9@[jCddif&Uv-(/G$cM1^p&dgbYGMtL0AW3GV+4mwq?9sFqC4[wh]MWlZlz",
    "x)a{qNN@P7DxD3bYH&NBfkC2XGl7wj^y4:)H^Lc8NcAx*lw[]dA(%XhbSFpSM:DuE4)Z<(tt9zS)&!{FG1H3O^$O5&t?$mICPLZ8",
    "VW/LFs&gGVKXx7B]sE{+3eJb9Y{M:KuBu=DTyEhLvg3Hdjv>HD.O3#4O/)4b^k$t(JGl}2<>ierKMb@CY7g]@/@l5SeBnlTb]Vc:",
    "h9H1.eo#].r/(KU/pW:VlGj/PaIdzjj%]tNL+rSw7-=afKBM]n>g/C&0eD[4]WA>7:F5TE?#XJ)s^3G.ELPOugz(SB=y$GSV>Q}M",
    "dt=[zq<5AtjDWcp@Dyuo?:D.x.EXlXFii-[1xIkf4U()Lo3oZ/Nn%Tda)0YbYZjxoVFS:pbj%2v8LcK:3}wyMo/(<#KppcZt}U]+",
    "}/PcR2EHFy]XfWnL*qOcVikD^C6PMu5(V&:v]G=#11l{T8leUe7]CLvbjIJ]>-QIgB1VZ<)c*c3H6^y.n9tDpa8dK:p5(rFpb$}W",
    "d9cu&9{H*f:w]mNc-Vh/iGS7gsyGij:{xGf)v.+r6fqpqGK-]Mp:RI(01if$KL<JVJ4kpHrnqWXk0EyT=<EJId8kF*2QNFN2f@w]",
    "v*QjqP(1Ha2@lG.Yc}$*z5T6J?em*cNVbAM8oNa=xjxTVB?Ya-k-o=%0/M0Ir=Lra4=#-:KnDS7qq!7W3pjr$nzoov3E(7s[l5mA",
    "x{G:MI6r[J-azAIMJ[Y@r&T(==P5wCnkHG[&b1<}b/b+VLb4[qM?)k=eyc:Epb-tuyblX&nW2]>5-bHX+/*s9B#j=Yh6[LJ.Bj+]",
    "(3O&htd1yAl)mu0k8CQ*N6p/#DL$8wFf(2Byv{{SXYxApDgfYP??M(#/#Gz>^SJh=>*VYEH$z7d!l%BZPV%QJPMZb)z0*h{:N^tk",
    "7qUI:e&7j9(@mf4(V]D)VtQvsGQeV4]L}WVaPF#pqn$u7QzxaT=n@+N{tihu0xkx?3GYe*bQ>Pl)YbD-A.}.cm#h#n*<pAZh4x*N",
    "ZU0hjYG:C1*c-hf@-CJ2<os^hGoqXqYFhYnaj14L)8jW(H6C1EtVakdFdrNjlX71SeU^y3}jWpwyG=/-+Jk?shscO&I:C1*Tn:&^",
    "Kua7uVKdFYPf/zqTvs0&[3jQX2bOdWIhF@f{0tCbXHJ]Vhny>e-3fN)a/rmxjmyVZ[lpMfPZ^SEJFhO&)?QWx->/nItYOVlo.W*n",
    "&kMeqrXGfud3$)=GRzX:nmTY[e!m!xOtsj?..vVzp}I!3mu]t^Eh&Kcm<Qq.lCTB1+#g#j9ZIf^7[rs]fWZVGry[G1sy*Ka0jm3+",
    "5I+E>Ejrl0YiduYhTE=-t4oMQf1gZwE3x#hFlvv-v}W70K*ED!wP@{wow/#=mw&ddV8)1F-m39E&n00FkBG^d0ci+8jf4a1<P#>?",
    "=0%2M9UeaXsTG}c?M#xc5kPpsX]:KQErDQzARW.<I(:YTUP(a&aI-VAKM{e+H@8Cw]3i&ut)%My5Y3rq@^d[-?=4MV>5KRs8iKTa",
    ")b2*%Uf&3rv1K-UbP/=A:BmnPLMT(EaE^F=42Kq-06<]7@pd!utX91X3)9+Ut*nJ@]ji?z(wn-VNb}C+nC6:i8H)ZU&GPIrr)0}B",
    "pAv9W)Cz#$m3M7Zefx5GY7E*xLGoVn+uWuCSrD>DYiqXS>0-*]N5@ke7qeCovw=5}=l+rW11<8RfC<F@i>1<%07[-Ov2TwjZ#la(",
    "!S6!@!lt5Gh}uOA<D1{RP]0^5le4qXu8N?s)ljGjR!D(eB]g1dUa3(Nto&gwtO#0O[hI{Yh*PIU./(eBHOT1Ab^P%6[b!.ne03*C",
    "941cq/WvEDcJ7hk?a$b+Ue^GNmRG@T0dqTBF#eyUkO4>Fv4*97/EqOsQ!u#^E0pueaDGEl-nVxZ6tBf7YpM2^Rb-2V5-)=w@-3<0",
    "{uBoypkjRs{Dzi.yYSOd^%4[bksU@w[j4T=fBW=&L&I:9{O[Rn7R4yCh^<[zzBVSGgYFl7ejrHKFk59!^)i0c{2uj&:{UX(Guymd",
    "hc^t#FTk05II{Aag!vkK]Jp1:H[3:]BF$G*ROGe]6Ey/kq2[wACz5HIPg*]KQBS^:Os$IEsvA9GO6W@[.gERe)Oa6Kn.dvY{Y]nh",
    "a)xDI63)h-=+(UznV9YzQDdN33dC=:ff]#W1o/pG{mY7hPV}Kv@im84xWOQ#C2kOfaq<^:bPJ5ZxV2Y&vKW9@Ap06SFD6/a}YB6B",
    "0u#G}SfJDceeBgcivqbDYh}x:??*Heu9wdVM6LieBmntaq10kO[#Z-XD]!BzhRBP:Y.Y]qLnw8HmF=+<2TG4^krVM<CL}?j6Ay#[",
    "RMmOE@e1&By4)fw?e3Ai}2<?:dHE&wx(D9%nb3i3G:q+lBmqNYK?y-UxNx2cxLRVMY%S[:6LW0E3sE2I]WmFrM[+XP+5H>63J<l&",
    "fd:SDWnf{*jA9D%9Ue^}AX&Lo+]KR:[geH)3ze?jkZOwLL8]A3c#bRypdH*qfVp#z?rEUEt{xtkTiRM}YK7-Oaeg:d2OGM?*-$W&",
    "^a!Q.Sc[.ioELz8[uF@RZ>3jl!P.kfD1tR(JQ{%IFGhtoBFg4}So>r.K}fPqI3f[zk*@SBe^9/vvcb^16*eCcf^nG?XlmPIJ2I*Z",
    "L<&L60MkHj!cmIt5un?}KZF*oW^WRqZh:iq<p1?#@ApnPBP0&av88/S4Hsk(V(i}#C?eGGj${naC8B8G@jKWB@rKPhbLV%Rp(C)#",
    "PtVBEeAY:waxRl13IfGCE6qA>ns{FRZ}tmYgA)(85AY!4*aoD&M@d(/4x6/Kz)-9GCtrVLJ)-=#{w$Z=1HJwo}E1DW5fgLVKW]2Q",
    "C?^5G&YgX^W{q-){m%r9^@}#TP/QBX/v.Z*Ugr-^>vcLqC.26yWh67.ng{du23qqF^(o@.q6i[wVVn2?LEINC1QRE$W<!<J^c(=N",
    "1y]ee=e=(dI6E47d5HSvNA#NgMR]+9zaJ}Q@?J(lcjfc7PUrj@m-QfC?8o>#k!xR4La6fHLZk!.->Ivlq}/eMJIcFsE1iRd3@rFd",
    "TIZC&.j#?2EY]Rj]8Iy%[&OfXs^Sd4U?I^:VCA?fkif.$oo83DygN643BTLgfcMipG^DaIEcFd6s7VIq]n/CaBQhl^w}2I03H*x@",
    "L4HVb8HjDVfJ6}{C1n4nx3y-G!(>p42dk>)P}9L})cf{3WXLfiL-#(@1$R5-R6TH3zeaa8}(DqK=W])}fA=d70!0hBid{iNxNEk3",
    "zFS3hKy0gCAe!RDF<aAyde4WA0^mnK^ekr9k:v>KkMGMLfVk+hzTf*b0f>hVEoRN)+XRdgid2B8@/#YXwqU&IRNq8>(60Kfz))9*",
    ".1&apjKY6GxL{Wh-U}]xS=6*WFtMX=Kjyf)bS/3<[hC8Cowl?/WP38qab2rY{L!{ZlEWx%.<VVIx^.VH-]32zXmc2.@szW!C0RCe",
    "][D*o@Ojej5[FNoES}RJc)YD*YJLDLNPSj@.0aAN}uf$=kQ*Fh}29s*d5*jWsqMF2Z[]U@p/K..-p3*xM}]&UtYa#7&4o{K>To6$",
    "uO!oq9v-Q75*IrSKI*p}/im^Wzkkus-L2o=l@NBah.=QxBeZAG4hdzT:m#2=F!X6{]TNo4bJV<c2G6JOgl<u5fq+0$D8c@]wwP$k",
    "8.{P<Je0h6)AgQa=gdQy)DN>saAWSDxYMIkFi82&H?WhV>FT%Bd3IfOCgj&5L@BYoVwWf{2LvQKgxz2I>!ry%V8N/s(63OgXSwKf",
    "6$gqfk[^]{WJExv&o36jDfZRRmnG8X:o2^lkyUdDuKocnN[Y8gohVY[2[}qxO/{5bNu=mnI::UWkV:1gr-1Mm(H/Xvuk9BPDm0ps",
    "pd}0s-v/&SF>^b@QvP/$RIOi+GtXi.lp1Tn6ErJr@m@nPv(U%K0i8r#cYVOq2fq=/[gO4Wq9bo5BtuJNR6+lo)4V$.ip?kPqdcQ(",
    "!}@>e?kkS:h^eQkd&r>4bw89.uN=h[=&5s9rr5&d+ODkLT8S[U(Kr*cv+S0s-j+I2]68aDfTY+2-Xixp9)k2aC{T]MkQmz+5qn&K",
    "5C%kQ-8DGB):8]!w3b{F:+z1NKpj4XU<Dhs5eYP8{UDB{whTYSER0K-FP}wZ}TEAD?YLOazy(Tr<Fw9O+MhYCx5^pjN*+Gz2D![c",
    "YcF&s6C>2*Ev=zi:w)BEBE5vS4(S-pd14szY[))@R*v1gRVY*WUZzi<(iJ}zF8^:@6[+RZ2n?f?XPNQdI8i>#5f}0lQ^kv5Z^rre",
    "BSn<]m+.H/0pepG-q0F0o1fZwnxV>7]qMZ[PN68wG&8}PaUB>JE{:>xb%.{Man#Wg=r=#6J}t#U7q?BZIr(1(%78f]NTO>.x93HB",
    ">ZTnV0X9=P}G(bNDw28dFlVbsC!X^yY%z4UtWp:KMp@C=YyoNHSb#Ah[mL9K{t2}!8Fz2(F^j-UTJMmgUs-DxJ.SoY7VN5hc[/f{",
    "2je3D}n8mdM4$C:/^R#}a)MA#F0CTH79le:>OY2^k.YRf77P6j1K[CQ6]PN=&h4Kfid7-&9/1vw=Ap8fsv?F=kf::75yF(/6Vsy[",
    "5iV=2-4R>ssJ?U%}0?/RWNB]Ak?3KY*/fR6YT>17-!D9f>-EIY4f)9J)H%C3n:dAWrmk0+d?vxFshLcOm[Ki7<gbxdpt)AYaAK:0",
    "TqXMgMS9AqJSR.:npamF4:)FM2rWNKXKN2>xQ6sCDc*irY=EFz!u(5mF=t%rM$Yda6<IY(NB<Nmu{3fS>cu6)2oruG3<KX:en$wF",
    "]Dy@Z+<gYYv9a{5/0&EVx8s.OIk6TR5=4Q7GIwsb0hFddC8ZMb>o<(QoWar^qg0QMWYNiRu>1l>?{dMLHgcoeqyZtjBK-Q&o4tsN",
    ".)1RM+4EiSrkC7/cG{e$X<y}YYxeN@E++%S&6eFH/d8M?2Z+/0*^EgoGM1Vtr87rB]dBff4p1/PpS/g<NMBtfX*@uCLT8y=/=2]m",
    "*#{sMYfJzXF/{5![zdJ!S.vB%BJSjH:DJgdEIX!zM87VFTujx:p&ImP5nvQ$7:feGm^&J8WE4Fq5*F>&]<Fy}ki<TYd9$#cl.:us",
    "3Y?zMPiK@IY6T2(U9)<7XCKyY&5&qG09=Lq*]kRlav]%8k?pI0L?cNjKGj$^+XxqPi82%T@9NJdV&UJ!r^!e=XhaBOU/sKxnoX8$",
    "d.Qkzat@:MCQ+}>{@>OeDqDpVhLj^0[xL@n&^.fC&mh}Zck}p@H=kKKlFLy3w].GEra8&wgcNUh*}E{B?k9O7ckigs0^{DH[>M?V",
    "{7zQlV7@g0h6c?L*q@Uz&d4@yyi/[zFwmsh5iqNgPGTbOKT1f}PV6pibVY1JuNTNtL{}N7^VtY!+xdqU&g{=Bh$K.@1ex5l1kHG3",
    "2&vs1kM{2UPZR@{@:/L45*lFMxXpek[c*+Wf0F<1&c=.i0&mzhzkYq89D:CchF^0tOAclO<m0Qx2NM5BigiPbMrhLGS@b-d5h?wr",
    "NVI}YFVKNtDz?[0hQ+4c0ki2eoZpj)Si(:(p%Jldhw=[q@W-1KvRJ3W0aLBWUT:<to$6EMp+XlH]BS091JvPd%cNMmN6fb@):*To",
    "M>9ffwptHZ+hs!:<woi$@fN{SFw9x){Yp#uc@.RePF).4>f()=zz%ag1tCA<r7CxIGGZA<SdtKAIVklw?Er=M2-$GsE!ayu:(A0a",
    "hL#qwC]iOBDm27A-gEZSGGkKmJA-tBIuW6A?ZFJr19h@93.1Q&c>{hPmA9ImKE>C{Pj/id[%@%#b][o}4!A33tbmix.gmrO504R)",
    "9W5(]:H&xO@op*RHDcLxB1W$=r?VRe.w!iyXgA[das4]t@e-qukZ%oSZ[sB=!*taB5hH&f}1#N7!Aukj3l<YdH::&r2>sg[H!68g",
    "nywnq]XxkPg$vEDaCD)V+5<sveC{zI/&PmUD.D#]HGa.@}JhGaGI)dX/X0b=@xD01YrOd}Jqy2%hrWlrt3-)D*gM44lQy</<lJkG",
    "U?3]Vb@d*2h(%CKrcbueGz/vB0=XjO3BL:O}WF>8Y6[p+&DT[aFesOn:mi-PRT(mJemMgNsKQ9XFZG-DGfkR!b3C^DQ*LaPnwO}5",
    "LGcM+ksyyy3kw:?ka6.X1Z@rk5w^u/[cD=>9!TRVqWZNTy3bpQon)w{Z7>iyr&EMj>H#d-TKJs}[7pk@{dnk$5k}2>E42=4m%Ezm",
    "v$5%w{yj+#icQE$8+vJK-pbg?c9r}aTS1sO[3r#$21-TBH)AF6:yD$/lv:3?uE<J*Fw-$Izw-C*d4*J5Sa4es[kb<)03Cf1HY?cH",
    "--M+ncnd3^5BZaxy^=BVB7y3t5i#sse%/W5%1n<+ZGH.5QX56f=@)28>Wzy)demKvs]lYi-Ur+-jO/=a&I3Ju9kcEdfc+XO&0=nw",
    "vKrLr/TX-f.Qti(m{]aR=>aWtDo=z.sp3B&/=Lo0F3c4tqB>MpclSBO&}L]9?e(KUk3dhoZw5bJlR3=h94vq?Q[k0cFm$Su2:+g%",
    "0S$u{rf2hpfP[]t<kJr/(FtaB{+jo-/g%4z*N+/pL7{n9KA6>P*gs+T0j&g[?$6jG",
);

                const ENCODED_SMALL_FC64_M6_U3: &str = concat!(
    "}QRPdE@tz4d}20.F3e60xTu>+v8ec)PZrESxH9&Qwl]{kBLiF3H]RDdHmv.DTQKQg^fUJuFfu}^MAV:&kV[-k[{0j7k-/@#/c@>@",
    "+*E!1R-xdrZ{XW7v8Q}=99x<o<qE>UuiGVeMk6&)IcN0)R1g#TXqm:WjU57%rlW3V08X@)E8RZv)AU:B((he4pYbe{Iw=)^&:nlt",
    "8g&L=*s-*Ccp+NeFrK80XNCL4s-TQbBl0CpAVG>C[V[=IoDRM[f.(qyaG0{S@jV]e-mg5n{-5Rlig-/b!u/frxE/]w*+/{i2OWw7",
    "i*XR+MdqS[r6AxbKOQp?wg+5Ehgs=5g!L0=w+bJaw!>J.V8G#K:wRDsg&)[8X/Wk1vA0-ep]IKHz:F0FP#Wl%Wn6EXapj<R2Nn$}",
    "kzwI!snP?D19V/rNA]0F>oM=:@iv$oqIqf=r+nOI0/O[F8)@tM5]F%Uzp#D.x^]69q1y:3xj!@[WXNO2&=w6.aL/=anE8ufabj4m",
    "{*KW@eW/i7Zg6HAO3!ww0}=Pd:ljQelcVBxdRl7LW8F(P{^=q/f$Zb[J:*8yk5E[]Fq%]G{@oi{Ku{[00j*Grkdpf#5zBJPSiF3t",
    "*ZgCM0dxsf0i}Z22$NJHIYi3/hvqJd6}EYQkfjZdpb[F1LX}OQacB7Bl(5(/1qk#xwS+YUu$*0irc(?Mjex^bX/*yN{)W?/Cpx@k",
    "]Q*E]Zt3Gpk<:g=XpQXx+Bam2JX6JTXeo>h.(iTGKd=29Il2b!C>j%0LU+vy]TF8qQ*{4h^Ny&jq[t)>qrZM:iv{yT{qszOX^x@K",
    "843a4TirW&a?&T[XwBlpt/J$TTk9Lm5NOOLk88ItxT{k&]T1+o{b6d&kO<Qr<+P2iOPA&6ak:^}9ps+e^=2R!d5D2*Kz:H1@ezmJ",
    "KrN{)uzJ/Guo#K[^bE}=wdJSxPNj?]k+(K=E6qwtH-^RH(v}R*Rkcz>yq-a@[-nVw/y@<&8vk0e@o[*x0b^[iacr2ad26Q)PWCtM",
    "NZXP}YiVmmAw&/*&PdNkG4a:6&{f-hH*plGQbI5BUf.:jD@y2etePkW5PoG?.!be%v:$=G:qUM3d6F]GC7{yr7r3nqR@le&PCF84",
    ">pm&w)p6=)XwwJ=/Q@me96LAbA?Y)5p{)h-wy&m)^5UIRlsDrBEzWLFKHKyn>5dPu:w.g-7N3*wih9s+urzFlWJ6#:[7E=PPNSg/",
    "/SS#vFu3Y7Z?1d(Z:xTPFgG9X[xv]BFq{noS&Jf-1tx[<7^RYCapXoi]FA%t6>V5Ymc+7xgC0mPMrlQvA+x?:=69<]dD##ck8GiJ",
    "3-ZOE:NgN(UP$Zh=l84epZg*JUC^h?(PG%mnrB1*fE.&w4J5Fm[jXzCFeakgsB3:RE}jc]f}{nkQRhL#9^K}LnKncB0/%.4KFNrq",
    "WGKW+[cRcrqP43FGKu^Mx(6h2<VqFn@#DXO{6=z-}K2>$Kn82O?qWh5=6-pVpd?H%tzqdI*>#fDN[Hc48hnsd1&Ei21L9{]./1o[",
    "qDi+hMvXLsU9M<^&(.5eG})P&.Ql%#dHKx.{%6<x?##6MZ=aOx5Aw[=U/Ve[5v!NH>ZgohBuJvidfu}vng7)XYmae3BO2=0!uu<8",
    "&kD}OXBt&#NAouW*8&HvySt}UUbxsLGU5hpNcy(?^wf4tR3M@Yj.nH]G7V)(e{5:g?Auz5/E992UOA@%[7376uZJDf><TFx>eZaT",
    "G*7QfAHooWQmeK7@in4vmDoN{](xfd@An>Tas*wO48(si(%CvUVl^:V5.1+lDn{h-N?fi@OX=78i8ys6NMRp$N!(IpO#8R72)i@o",
    "Qcg@V}vM@CIYo7qLBcLTim}(TyTWL<iJi@Ym%!TSvvmr-5g}k47ilj:ZZUR4!l1^}Z:0B7&f%XTF%#N102$EvWcKB0PKt2/[V4g7",
    "8r0Fqbs&LkTzX=^Ttq>4(w{V.B3*zJAXULC=&Ra$U7eR%1!7jV^2!gssx7XJx1YcE[@H(a5Gr>%PLe$&QmUe[h<J[{0n8VY=$Z.p",
    "&k9b&z@UxweIKK]s05r[(HwZOrTsZGgw^C4()yayzDG0iZY3QnnW)Ux[FRS%jX3^h<x9M?qET]nBlfsl2egEema9tzocOw?s39K}",
    "*=u9iI7vjw-x<ggn[S38eC3u:(9/zh6ox*<[fd#$>ID!k4b{=<kfeq*oX1>YMhDoGl[/9l90XR&MiuS?0cviZOj:Th6BieKZ=GDF",
    "y$C(7Q*CZ!0neCRGn^l5GAX.Dii$rNL:$MkO5ZZTC3P[!Sh8TvfL}xlkPq8:{@WiZkR+>[PTH@m01q=rmce9}fNgmyb=1hGZ#Ua1",
    "80OW}Y3n%xgrcvT{bXH5oV9Wt036g].20jQP.>3pfQS+ueu%MnGE>!t]Rw-P[u*0zUTkJQ5r$!WA.kPgmVrl*L:WiqMyjO.Xy1u<",
    "]SUd8@3>(kLWLxw1C7<iv.E%*@bt=KU(eWS:#Pmg4-C>uJ5Zgqw!h$@/DvodV$frLFn%hOU2LrZUUc-mAnjRWZ-kkBEb!kuD<]XY",
    "i6KZZmbxaJ]PzLO/4O[x5De/CE6W[czE{R%Bqg%)&+(m?gde{(8H}=Iy^r+sa-JKP&!Em&+(vq$HLtT?(29dS[Ak3V9]jrsxB[Yq",
    "RL88z3*]xaAUxe2oD@*^f-5TH)m}yV^PjSag@sdze==OFI3<?T{ZE<04[t}Hjs/?.?vIw8?#)yIU9a{XmIWmj{{bfeVv)[Ac4ACr",
    "48N#!}jX0P^j!/?5+u4q3kF=reiaLh]?(C^IY!3zVp%h&jUGWCD:ahE<J&/F5WY+BPW9W5s]cVidu+64pO%7kRv^r4M%t@&EeuBE",
    "F?:C((D>qHH(.J$5g*ZwbPAn!RLgMl9jHX^&2pVtJU!d$L0sVl7>a$r:aEZjtyd%otqK4d=dKHfrkeyEj!{<%U%fr[5]mUkxZ}GN",
    "!rCr[ZKqXoC&D6y0b*e@2+E)a<qIC<<Cfhl=x2a>F[DU#OROl05s{{9p!Ep>)FE{oupSd2x4:sG{jR3XD1r11[t]FIXeKV}wnVg@",
    "1Rm-G0]:5u2f8!s]v$!eJz>Fy!Cy@k!rG<yZcP.2DM:luZJ0yuxweM.W32w(SUkMInt(W8a{twqet?Hwt](6>WY?9)cYiKt0<*:%",
    "ms?IL?EyaWfJx*uE=t#yQK9$zZ>oFoAn9Dpv0vWz4b==K[dX?:=)0zvU*KKOaE{ke-UVd#+>TMrZIgetHJ[V?Zse9al*1G+>SP+E",
    ".*:j2D]z3![iTJC3OWQO[x3A$@7B@v@cs2W.06B-jFU}ub-@N2VYJP#])P!4-]%J/3nDiSEPh(K&5Wt<Kho156)lAp)YKt*.dHr>",
    "?Q#%kW{XI+lEsP5Uk=qJSBwT^-<z9@y[<nfz15]q{gY&bUn&iUn1?9u=yr#W}SU^UXdaXo^e$h$2G$30)L{j<8&@odCW(PzzX$sB",
    "!]qLuE4G]xrKjOEGC6#>FfO$}Zmym$7!=er(vfBgMVj9g/1vx-U<7HpDuvj>^au(j5oZaFAe[M[dJU=RJaIH((>#DV*%9aq9KV8m",
    "!q@fzF/qD?eboMqYH{48K5d5-]Fp<cGHW%+a+RpcL7@u!Rnk2LBsb)dC&/@dKUJopw>=TDhhl&G1uB=v@(=s}-%tqIZ.mFDOqK+*",
    "Ecji^Uioxeki}]8==:?J-Her>V+}:mo-l[k3s8sGngEL36M&SN(F^<!l&q*8BV=iTny?@ErYPtb}V@?d/bxgMT<mwsqw-#UG1!MP",
    "t0+=]EUlomfr*>S}hoWlQc!(x^uYZ5Ae3einO=BiM6qY<?FPqQ09X?OUBiesXyS&b{+lT1czX]1:%(w:i!J6#EM=/aS7TcNFgQ%*",
    "fI6:bFymW+S(Z<C(2C*#laGj]Fs@8<iiTdjGlVB<jqx>tP:&])^CkIIl&Fq[D#shxG?)=x9}Puz:GgdW09-.)^CRC@elJB@=pGAE",
    ":}UG3[b]gz+b?*gC^?GLSjxA35LAP}R5SjygKs%xFMIoLnRSttL6(Sf=%1<XBfn[l%lmLq:W!/-?SI>>p?fdgA}b11E]VDYK47&/",
    "caR+[*r(Ue:*o?!GCK{d.-7*-N!4ue2zIMUAz!$-e-Cx@0E-fPpN5gFc3<7nN{^bAeR*5-e[QY*0Mgtr9WPjEc<d#ib/HFhie320",
    "LhznjxH^&<U-OPwKtp-pH%%Qi<vY=(nk-CJvn3U+*]Wqmj{I&!7Q5k2FFLZV2daH%}!FWm&^.1)^=urF(D#$38LoeQ=tZ0J/.f<d",
    "54#Qsayrj6PT+H]>y#ZBRps[C&B^kB0gkYcZ^DCY:?u[2j>L+bMQ50(Li^(+B>$cKt*m]lp95W:v7@ezqG}1YA@1cCz[?WMH3-3S",
    "]jFYHc%cF-0gkx6^NeM0GR%pkIe)mySnzFR+oBBf-RMb?n=Tx8B-^)Jb@hvhpUhEbW0[:.FngZ<LY-cMRsY^hp3j5>^zS=eF+Uks",
    "(8NMyvX0b8^=lYTy]dR7xL+szI2Em:0c.d1sCoNmPP2*j0d+kk07}yE4DDhbd^MH^A5umY^lSG3[[0&@.EDGS7EYPk5#uhOcQY%T",
    "EN82=qxw>3S&%WXsrE1AFh!SlE!Uq81dlzUhw^=8GJ+VAV5DpTZ=)xj01}xa!y$tzS-Cp}17{QRhc@U.1Dns6[gY+-[fm-)04Ccm",
    "?KO2/!&t1<WXRRvDMOCnL$zj]!@YqZVe.(F<*6SPWM2RF3N#O0icj@f1d{yA=agXPw:s0x%j@svmjA=28=zB69Q]EWg!@//-.R]m",
    "Y{2OgosVohPxp%y5yw=^3G4k+ij=HW?uG&u^bA2cp&yJ<WWB=><zI6joXj2x*8PYEFseX+A3+g<>@>?aqv3R[f!yyfhM2v}F@.R3",
    "J}KbHscJA(n}IYeuC{5heIYWRjp4DuDoY59f.-:M*K}@ctNGgF[T&K*yT+LYuR4AmANZz<)W7)QL>+0^Zp$*PhR[IGsrOUE0#=D#",
    "g7rh9lQxi:&bYk/sKa^@}V*:$y@d<tao2&+c.z6^1N*j>8@[PA7wzl27DmI<FN$=xdY^x#l9{$ZM$2:4rY4-TO8t{kaafnod:ysy",
    "0ZA.H?5Hq}!p>0mwq7%=yd9$y:q.fy0Y(aJ)L@STR{Eun<IAZ^hNICR2QT<X6PxGl4bB(Cc)CDIBDSJmdN9yo1q&C#FnTU:[dyk+",
    "zD!ONCQXneu*UekAhHDoi@<7W/)-9DKfVWpO(0c+A@&a12IfkB0LXa*aptV(nz<#(+Ar2(LDh5My^lQ^4DRQ]mwnl}bw8G<:l2ly",
    "bl5-G56?8^-z#lrcyAvEgcln+lL$LvoduI$aNPv}I04*Mm}l[WcR^/d/N71&Rzqmo-KT.!i(Aa7=+9sy*-z-]*qY8=1l^Lm+6O.6",
    "!P&sK))8T(S2f[CAelwnJY2O9LS%7IY@R)YPWIl42Y!VZJ<kow@U*XU6052@sa0z1?]quN?$dz)xExSu0Ysh@40!yc@mw.Fo73jN",
    "/nZrO@>zq]dAUvGEXimC8O%SYV(&acTjzj6q00%95auR:hR!ULrswMm:eEDJCg4D@u66=M%95}aoU%9fny1PR+$txcioZYh.=<Pm",
    "mDIcB/j6ly[dl0(udaf=o{I.xjuy$O7qoi31rme1Q<aYeg.jQ&cyI$rSojn@oLP)cEW75IBxaC!1K%Y<(6#>fu}0WKBQJ60Gl[#W",
    "(gAJt4=Z>W{EO%X6pW7R7Xs7U(aR<mBxUq1OnNYbL@j#yKQhGhcP]dbXucyT}M6cI.&X!1OJ>N^Tm1I3{a6:?rVTv/s=((LMZj46",
    "ChlciR}^VMps(bLvbjWx4H{ut5OWxuQY&-(6Jb/jf5]Hp-=q>WTd.EoJ<.nqbSB!$Wp3Hd1PZ!KB/0c){eh*yGFJwpS*kVV*is*Q",
    "^<8)%4-fIKEV?i8DmzVMD^45O7U(17e+2]b@*x3m[Oq^J6xX:BKG)}h14s@Uxafv9]C5-pQ59SVaKfvq&Q}oaW<^rICde9#{CP&J",
    "-$xESUJp.ST*O)b%9uW?f}gOAF%Y!bC$(%P6>Ywqq$83C?Uz-p<B+dVy+ch.a%%4$ymFhnd7T37iJ7/G:2^u+X@W6e.mxw8r%tUq",
    "RVtZtvBc%EOoWlhd$5Tb-.F>kh7%?F)tK}bBy-ReN5m*oiz6:]eVwFcU@0v.>851SJdoUD]9-V6s=ZoWC+@3=kdi+>F$+p&p<]Df",
    "m2+Utm9w:$VAZ{NHbja?qJIp19JslDtExXVtLtnkhZM[^1n}#dAS}m.(vUl.S{V!0Z<<$JRsx?+KxYvYp{r9@.bdhC@nnNdnuJrl",
    ")b6?q!Rc#q0<[HYErJPzQqN:gA!3A?=b/ooCumnF.^&j2JpAe$ol8gt*$BJ$dAp./*9}?vC@sf!N-OC>nqvO!w[#p#k&*9L(LhYG",
    "cVzEpsK/8&8$}6fBTW8HDSA3*c-1NxK%l8N20dP8F.7=6vK5-NoqOgxFn}c(BWB9ZN971AY-GAxB6B?(>o7<g}?*4&X2<%uNkk!@",
    "@iC?7myo#@Ma2.^QT)w$1D8n6p.L0p+XmOLX]wa(KJV@MgSC0W(qK4.eG+S?9QX5kuwnUF&gbfUO$IdTwT).J>(fp1dCv9LMyeV9",
    "NfHF>+h{=f:IIJ51gZz9A$7%WevRH&GB=LA}ltH+o1XB[tXvVZN-yBK]ll.sn}/%MdC+SIHFU9owM9#<O[&*tJs?L<FQ$*83:LKK",
    "MpV#$!58[:TN0BfLtbmB{WF3[dKYeF^>0<IE@XvH=zlpXGQK!-&A0p$pPt)dU)7E$f(Z>&7jTopzRueA[>j0!pSwEe/RU1G>^lt-",
    "/k5(11qu.Tc6!P@m$e?0h*ULs1+N5]zdqG/z$9-?fg}OBcpEq=2I/*5s2fGZv7vyy9S7J6V)kGidpFmJhLq[vtVKe8lI:l!Z5e.Y",
    "2.N*{-?:Ip02s.gGRE0VMX{>*p2F1BmK[XTx{kEe3k+e749[Aa0KFQ^{/}n(^3KI>+WRtJPt{N/@e}YcE[{FpWIa?rYAq#:A+THC",
    "M+1[nn:y**W:vPfAPc%.X6b1t)V52CSxA}?r]zK)O$SH7D9?]ZA=Ac:Bc.!HvkV@QJsEwI+0dL[5Jaz?l0Lwy3Q^!88y1Gh^aj{P",
    "e%6Zfpr+(mi)d-wNfv}3O)#@TO<*BT1rELb^P+u/9A[W>G*L1qT)R(D:mnGfl%Kg+?ewin[[.TE?mQ.uTsm^A0$>7pQy5x.AvUlS",
    "GUsDKOz-f^a[j%ldaIuZMjbsU[.EJ^:]qGcR%cPj](sa$Q&Gr@b{DE9xW{/W}aRHn?uR1.y1E4jAw[7:.-JDmua[0obn9+=KyO<m",
    "rm!3i1<@/6=JvSu&(U)$7R*quo5DKg@aKS{<zY)*+h:hGX-C--qfUn*ExY#&e9/bU@IQe?LZT9a/$[AqTMC5DHx&*M7OXA5()m&U",
    "?U7D[U=4%]!L!JzoK@-r%8pxuh3oeTwuTm>!EyGJ&k{Km9z&B[)idcDs9q%9znmFZT&D{B.mSr5}WinmS{b%nL1#JJyimLhcCtXU",
    "jC^NZp#yVNaV&-rsU&dMh!&:+Vc6n:H$y&N<-[?WkPfQnS9@W&k-U-O*nwt)+S}r7E/9=>PH[/sAjyDZ<HZw::oRk#Z/-cHc}$+%",
    "dcL54FofbJy(u[d>ORsMb)Da*iw90H6X6!d915WU?zdpehKp>^Vq0wzJR}.FPJxQQ5m$NG]2c6)}F13:CVx14c2=e:()r96}Ju-I",
    "FmK[SKJ*ebo2Vd>v6.F#5vm55xOcYa5l86hTM%c9oUqc8wgf4zH&(Lc08+15[cK}l2*y4D0j5gS9.mx!734J(($P&<M)WW5Se*hS",
    "h]Ds)-3>F}cOR}>Ny(hbBy1-a=m?WJfRK{$v-@/a[>9&m6-6EDIk#&N1t3^<y$$ms-ba*8Lhs+MF7Oeym=ul&pcO?AwhDrY?aAEp",
    "InyjBES(I<y/(&.y8xJaZlw)/W:<>hh#!j/v{S=YR(IZsRSInH1%{*[NzL^:8R:snZW]BS!/Z?NYD3b#M+5RH*+9b3w*:9jfRh<&",
    "qy%lfl}KO}]0v5-3!UqsHn]yf^TyKD.fouLH{O-$1zj=0^&r{5cyZ4fbqHgLAp+=%Yw]tz!S?NCl]GuPE!42k5w}tTWsFMCF2K2z",
    "in*U$6tQSYs(pU>cSBkjlgkw#lLm^hOXFF{0zxJdnTHtU7MswrxiA0.:F:o}{+%b8tWjx]Hv4gsTZ>6*MQW#sS%3QcxUXt@KI)8w",
    "AoFW-PRs>sqFC}&P{t!{Z[dz!wCrI@7:b#{5j6vgZjlINar9>:0&{/lW$}mLW:&^tuR@@1Mx5yp*we}86!U6JpXX][/&(zfzS>7-",
    "><!Ei-zA:3xDBorJp7e?*%lqvPEZ49Bg.igLFBrmFXc-Lp:jgF*(KrQC7>Z=OIGGyjkoIzpY/Vt0tm1GHo/L%Z*4+>gaqO7m9oda",
    "vVT9]Ji*O*&1u)uI8(iZ*{)CIj&EE^CGPx/U]EL[:z@j-eS}0^{x=1boV.:%w*&.NlO%=^!j@-L><y<Lu%p96kVCe7H%l{Mju62$",
    "WVwwu9$z}$v&VQN^sLVr(dA*oN&99U>2b?Q5x8mCoF6Nk&g3o3:bJ*cf6D3zu8guinnr<(vJuEM+.4=pMk(qSn>Vt2B9>+!QQOPk",
    "OHhg!n@>Kr]vQ#A*<Abo31z7vH}xB}e/tx/D):--(CEy3f0wi/6H-?LxWRF}aDw-K1!3e&G*VlYN=7RrH4Z&%l&:UU{U(m*RQ7<C",
    "B9$tU&%@N.F)<kz[fbfLTkg1[ZfIq:V&RaeKvw7<dmcehY>TBTifv)*52B@[eA!X1SjC}(7o4>VA5=ugA/8OntQ2+57/z8iu)zsc",
    "09E#rH$%t&:CQ$IY+)uO&:uiejFM@hx!hCiKJZbbSylG]3H[!zZ?@8Aw+Ml&k:Nhs=-^%AcG@vWlzdpwFB*lkATN1NQ.3D*op=Zc",
    "P:np{.v?.&8AslH08yICD(&+XW2Bch?DvfG44rT0ClrHZoh^5#Z*yAY=pQgj/j*tWlqFpVss[6$BlOAyah7Srk06m?!HfT}!hnO^",
    "m]gOFS7!8wBUfKOSMIe6[{wS}BD{EM3Rz3@m:+K9l<jU@g)vF[ox>cyVZ*evkXq4JPABz>IM:POFR]&dXO2A^8Q[)M9&0?>^4D38",
    "9...WY&0-KGtfc7XvSo=J^?T864^o1OzitxI8*3@fV^ha9d9r3vbTC^kgr0nRr2e1b[PKT?8gXo>3Q(%}bG:ztnyITVGc}TFxwD^",
    "jI21o0k*3yarNn?wU]]2G[N]rh?/^B>&gzYvAhbb{/z]d!fXzl?1.yx&b9D$gdQAsG[bo0c]ph6KG&05jd6blw}TwL&dL-94HlAa",
    "7[6u@cUwjFND#*}6!bwhMfglyWRN!mUF@#zG&F#63WHz*j^3e^3L>rTxVM#kdVNZ%l%CzVa-^j*l9:d$}3fQcu0a%bN/dK@+EfQH",
    ":oTE./hZRu{gP@fcyP@UjHG5<-1$IHabJ>epY#8d/Ctprh89?0nAuXY+HdPHP3r3O",
);

                const ENCODED_SMALL_FC64_M6_U4: &str = concat!(
    "<hY[q&FEis}MAh#TN{%7Xbio?j@!x4@8@Q7UPW[%6YgmKEU=JpemzZB0]HQMQ9(V$VB}{8H#K<=Fo^A>-:ZL.U@%XL&CO?ii]%zO",
    "?g!88:j7hMTCOh+CTk0o8aOeHpk&psQZ]q}mFC/Mq9.9z2+DKVPvvmjBC]7<d.<GDiOKr)yDVWJ(T%]}C4D(Jrcz3JIX<F%AJTyC",
    "3g3uVem46{9P$<kDTPxaBS>iSv%%ALsgVNr1t7ANhs2=Q5yX@*[<FH3]8tWM2iCM(5y6*V=#7wDvcP>!E0NMY6U%<:Sn.IOPrG[h",
    "mFhu#Q/B53Ez>>zGQ)SUUb)*I12E6(V7=/.9s}T4BW2^^G6)]}@0DYxp]WhLN0Y(q==B9?7Y.xnw<#hJc(lzSj:#rLbk{Mn0dod7",
    "!48?GuT4tewkKTolsO20-1w?e=UB1M^dKt1kUn#cApyxJEO-nyS]u</p#U!B{i6s%}Gv[j&6z.7bnE*NVdrqDbq#O+g2YBK4RXhx",
    "z.]XKAGt]s8o[wUL0Q@*=oR/K:GiX1zHfw/kzhu=xm*pIDn7h&jJ1Xxj.OE([1mqbw[m<NNHFt(oU6)Xi4<!+I1$TC^9SW)]Kn!6",
    "HfRv70b.-#CWTBZvLe6M>93MAwxA*]T(QcUl%G)VpN{Pg?M8U#C1QyH0[CVghrNp*!0K#nmV1v.7*8XVbG+bwA)a}OrxM<o5#bj=",
    "r.>qh1BIjM@3r/9ndB^@21ntJ[R^]tUbG<Zr$2)dU8.ZeA8M{7kUNt7pep&5>LLrzI?vM:{{6(-ttcFe>^r0aTdj&9d71B!e+aFa",
    "Rl!Q-x6AeR1l=<IKv2N6fQ@Uo[=R!5I=*DLK/[e=c3dr{g]5*sKS(9=*U1$Ua8pTl5V&5$oII}dtKa<z5OW>3kVkV+.0u$wUN[03",
    "}UmXo=FTL)aup@nH@bE<XheVG>RX7cti+3eFs^v^AoQ#GV$f@=Z<UiaizPE^Y<7PL6mgn6)JxJ.L.>vj@?1+PaP$=TGDp1Ou#g#*",
    "UB^jbDwGE3D?f#@2Z4KVHAu4IEL7sTatW]FxTTREcj7)fAkPKO1@06e47?uVpp+{qme^].p!VQa)93](c[{*@ayBDFYYyg#DhJgP",
    "uB>u{i*4a?frXr*pF:ww4{q4UxWWNcd3={c@^luj+A&@ceG1Y7+3346bbR?h-]EAtZPQN1v]ig*GtH^92f+r7{</-=t+.3SWWX9@",
    "gS:NH!t/T:KK?E=WP5^gVvn)*:ovjf7=6@gV(*Qb<$LpF&2)cP93)%nT^r=^esqO:Bip#a0/a=V.9eF@+zuIrjS]OASnpQ*L)b1p",
    "lHMzp8*=keS}K6r7^DIk6>i*#n8YW}Brt+un8M0.}nTF*ukb(wFo^ttl%a@LNHz0+rvV@&xmDz.&j<>aoM[Yyy^u{&Z%V&d2h(:H",
    "wI&R:KwVMMy/[iY?OK&T^XB=Agk06G)(U:bE]/la}IsV7KrU{ZITGSuDgIRiBA<$3O.<H.]]W&?txprs02m*.T*vNY{AT9e<xb^5",
    "G/)*CGx$$t{Lps-<rO>hQ(P61u#Xlv025^5uFK9{/R=qyAhrftK{wn9^2A/y6H2RZRVh]ATx#M5C43+2:Al<-4AEM[Yb=mjY-46d",
    "r@K8Lc{tHhmLq5hUk]w27&aMk}/vPS&9yy!=<&=jqAYJxEu*]%AZ1>ldOU#pkm<r$-9J:k)4K>JsHYeg7e-ys&pjE)?yW%6qRugN",
    "9jd=uzf{/B&2{@t:o#*$[C0.YvEC0@Bc$T]IT.1^vaky{t{{*S:aoQw-5&)>^KYdDfc^U-4@eF:CVTP4)ca!?uOo)xDKJ?oK]5VK",
    "5(fgQj*rl@DS:Gn}o2.IS98Z9C6dsV=1[lh66kMAYu2Sr]RW$/igUW:aIS@HNKA)6uCRQ:p+<mUl[iEB9XKuNQ&OHUv8fE*x5B%#",
    "!uQHnWJb@zf(W&u/ec7l9IbUZOpOh-KKP2#>EWXa}9t!g}QjE/aE0CJ8KFYit})hf.NLy+rCZnGlZ6*H:1}c!xHqj20h7vz^3#C@",
    "OLnxk[fF/jS:st<f*?i?-@llc-[l}EW66boL/[qmYuVE2Cs1FR?SlvNB0}j9yXQuhjboVCqY3^]&>oO2c{c&W8*w)bGzVMH^$pHX",
    "?%?gu^KcOdR!m5lJJAWN6aD}nmp8YL[HR1fi?M)BwZof@k/DtVtgwzxMlGUGg2?rcKJPR@jx$H([s<RW:]/ywEOoEHUc%VWaoK%y",
    "0248#{[e!zk*psY]9S1u:&Cq.>a-?Qhp+Ej@j]N4-I02&Tu)N-S1Na.y$!i!M:zEWaAAgG^c0kFLUF9&I{ah]YLDT)F:5gTf=aH^",
    "7pIZQczhXHFm}D^PCsrp1UyVQ<is$[&0ZR*8<o9>Pc5Z0Q)8z5{-[STXjI<%0{gGoQ]tp@AjU>UwbpKnFSyHdz(e#p)#L=c<-9F$",
    "*1.-NDsi<r*B(pman/3JM).2UPm!HO5zTj%0m/vSxu7[7Iux0OJ$]-0I&=KNQU?iEk}+z=HNP{gK}&IXM$%1uhDEmH[uQh0C+*Cl",
    "9oGZNSn^kcg?69vhBrl-g=H00MD5zSk?#HOSFddYxTY>kaRJbl(LWG4Ms3KLPN}OP*RDy[v-tVoPd4v=*zpEosOo$M{<JSm>YrPk",
    "7Fm1T?&]3x]RQ?nzVGq<I%Z{n-Cb#GLb0/C]EHP/exxFcF7AZz8)@TdyZ+$WY9<xS^/*EI*#DKuUYHt!W}LX4]xcSEX({WHJDLp=",
    "8J/[uyH/:{5%:(U+2zC7[/&]Nj9&l!l@y9I=TD63O<)6I8Ln]K=xi(B3FVYn{#9)uqejrrdue{xSDntd=gW^xkE>WC6Zd}G!nGy(",
    "8q=^NUTk#+r/h.%@bVgc&m[gx*/(nw.NN+4+RVceN50G<3I@ajKv9%1u&gUKon%=L*srZCt$X6VTD3pBWDClo8bC/D!7iItkYBJL",
    "7tC#P=)B</X}VY/OATSGo=:!U+<hNFb8iu=Qk+bBZEBB2/4w5bM%04{Dw8qY9E6t4(Cc-sS6:MCricnq^+Hn7hmTTsp/rs^kiy23",
    "]wCOuFm<XJ^LCq{Kq%$cG4>*?bW=P]p=2R{WtHbgSd-[/*E^t^^b>RFM9qw]o%Xy]V8Fy1)Kr!3700{G[br^frjjD(V9h/[}uEtv",
    "VoDe?tKr3p5spQ7wYU?5gE%L*@i$rw[hdF+3Bc9:C/^zc04TwD6Ej%q0aY?9{=4(UVi4{bnyWS4//tLnZ[N${qxoKoDhmALNG)dZ",
    "[KFo}!ku{(>OTE+m1iAz1$A6OGh/iu5y14:RUY+{1Xg19:>[?+DW*pM.$e=0K(KV#Iqz^P[jIJIQeF}yb87.j4QZwvaiWzuZqIlU",
    "5)h0xu1sNgKt:a3xUpEXPSTtsqCut*X.SmTwXSEMtfN>%z0TeB1J9ZyNZMJ=Now354g1n[qpa<CVXaTUUkv>7@zZl?y:-]ewr^fU",
    "1IK#LEvHp2OI5RVT)qv/qtcLyZApDT*iNOG>w1&%>wf?7IyWar={$2%:zAMykWyAMU^JsJb(WFIFc10fBp(5D-3fB?S!PowW5]$l",
    "3vPxDp.h<98[+@fNA5Y1jLY$GfRPC#Tz0y5Rp(0+ZwEfdbFoCT)T%PlBA0ThwYQ]n3zkqI}r9Y0OZ-9KdlM?c5ptOk0c7lnw{Z[#",
    "=I]NYM8A[er3NynQ^SRy/kmfzhY8-ZeB{/]t(HlRZ}F5S>I]vK8T{([>KiNTm&7^hsS1OfbVdKPMobyo9aY7jQFM&YlUmb#PfW.o",
    "W4imw.Xp]<1@PD=k79Y2RiWT+L0*]^P<G=y1=g.).&).R?+CN.b?/B+R.YeN%0GneN7HzLFi+Y#.L:1?8utZVM)ns/O=>d*SaPqE",
    "Ho{()H:XAGzIU[/v^m5V@7<pQSLD9UeqaLGR/-LoT!VH(-r(A6lBA#fBC^s^LJ]!yPKFA}981pmtWZKz[)g&H2fNlmL86318d)<3",
    "4TQD7k&AUCZgU0&i5yK@Ymk#b+:iK!0hHsxqrIx3G5i/DfEub05jY%r0dftVxWyGSCuc]w4F*#btb0Vkif%/.=vJZxeR[FZ}^IVJ",
    "0Q#Erzj?DO^t]cUxA?qEi?<bE()A)?{m7UoLgV&$kJr<1@FRKRQ:E+cQGW]ot-([p&fkg{l..c8PK50?(=>Zd<MJCNPIJ+yNdFiT",
    "fN(E6uqH8OZ&o{$aOtW@0f@$=>3T6f29B?S2+zq=0Dym^bUBZ[]J*v*wq#14jLe<X}q5M[&3&n1p8T$-@RcGdeP)q<fTaIsH}xj<",
    "S7v4OExXZyKpG@bNUOmkw<1m1BoKA.Oi[hT!zo2}mJd4B!{V[$-4a9^Wvfs[/KU5@38^U-PRNNX96X%pb:kt[cAGi<2m10!8yg!n",
    "I=ACl=0{/sKmgc5>d7ZX*tUK+>Z2=ZtEX**n!!emzQ:YU=/dEd71eHk[!f([(jk2KlMMU1fZ1oP:PEeTK8C[1V9<Y$7xZID]B?l?",
    "L@b.TwvzxG({-o:0edjjkNB}ZTC]k6<xIapScc@WC22uR:Jc#!W699nrRr1F=q#RZ.9C-FTroI=D!y?L6nD7?VmmH<aIid!D.2BB",
    "CM88n*xHtm0^9qjjrUcDpH/9>yveHyfCzQxr28#aK&sH^UkD>:*$372iqcYx6Y31b&Z/R5<<xkM([++9e)>FrNy9ggQ&Zfh+:akK",
    "300MWM+wktsn=%H:^ZU&8/ap[=s2V{4Ya!wAN26df$2#Nf%(J@%6xg$A4cr3zh]HFpeLjUzY>{&o%#BZLMu!dd$g*sGD{l7KAD7n",
    "b(>l-pryWym4VYTRjAHStWApKM{]9f@6CS>4U<+(CM^7&J8erzyoc2Jjr&6lTqS0uPFo$tF(Yuz{a/1HKpVrzO!KNR%fur4=[bF!",
    "qOzna=sse[x>Q<p^X8@u1.<4hqz4pGF%%qm0f&s6M=1$I<z#zbK37lwc1MLVQmbvKDL<l909E/n7s8}yBSzR?tTb>.I}2>Zw3:T$",
    "LM5wgZQo(Jy*3$P7^dx4B0e3at>Jk5r4bNALSbN6M4GmOf*.)6>vfZ?PWImK&na/8}=w3RgTNNlSc]4oin:f^D>)qROi[hTjqta:",
    "X+v?)]:4cM{^[&a]o-(FrD6[g.BGZ4:+#(S7b7Rg!]htq+:0rl05i&LJmv47z1svmEeB.)FpXiy.M&U{nqDSDC!&jHk^s6C/(@Vz",
    "2S0qY=k-}0^LEmOx>D.C!/cwcfP^5k6pq!kJ?3bjB9F+toE#:FblY>!XjKAF)/UBg]-6/Zq#ho.ENgh4/ZKzOgq$g@<?z?IyTK4-",
    "w1Wex52YE>.0!zsG*d7L34skn.EoET^P1o2H%BkUQ/5kEDZx#gf=X!ee{DV{BfGyx>=ewYv@xJ8fBjpm9#J+C&*HlqEN/jcUK)<&",
    "AeWgHzQo<%&xT@^ML)iAQmk^VsRmk*X}Td$s5m}(Rn<=zQ1j]Wc/S&yn<2m1ud=)9@]F*Dy8/2[&F:u>lsihCpmn7Akd-*sd{Q*(",
    "]Zlt9hdwUYO@ll88Q<p3>>[{O3cr(ADL!I2c$QldGyPX?-aPlp3T#cgeYG}qdt%+gHo9RNgSuxcR/[YDq-EohaGdZ)6#UZrr2#(u",
    "[V>%7IfLmGu{x]uh$[BUc}(Un>:hPr1@gjNYIAPiA.k91<3Ms2/ss2TM26F:MowP*z:kZmbtru@JK@oa8m&y6^6Wz#Uuup/Bl.(2",
    "6*?8%)s%PkqJ}<fC$qj9Rj4>d9!<y66l!4LsZ-L?OfZ=>]0i:cOfE.>*S}^>fr&2K66yk#)s@zu]c*g]b(aMa4X^dvDCFgEilAg8",
    "[=6Pz3+M.UFQD(G@W)@WM)>eiIY^ZopNG-$^T3EdIJJUaPHRjg&=%fv1s$6Ew:#RgxJ:Gh^<cPWnjYL&n<JOzdBX*v8+:tC>Ob?!",
    "jKp%VvaYwfM[]8.n--JaX]z5<<qxvfM$KNP&Hh?>@faX83}q9%</TJQ6:!fVeCJ>(oc(pa6?iTnnmMqBs]X)[S+8S4Nz3UPVHkN)",
    "(cS9!8ikSGzV!fDO/PEEOK6OCC5Am5a]5HW}[L@>kM4DwIV^.curZ&4.L0eTwXz?xBtytsKzo!tzN>+u9XU(6.]%w&[i75wrh$<t",
    "9=g{w2:kk18M5$qE-A<1a+zX/hRnLOAq@G[Ea{$JacW:el]4!b3?><WK7SNFl:4&I)F7lR:6vB:e}k0cSyX3)Yb]3Rvp0LfU45zm",
    "<KGRX&{7[Pd@odrxO!PgqE=]zDYr8fAOHZZTBvv-kZ+o0^[%*wkLh!a&k[RQo-Rlc2:N+tz-D)[[&Ngw]GPp*yekKc}BEhw2>&}+",
    "(#XNSU3#Mrk]4hHFaFvTart8Ums*Q=PpaU4ljeAgzB7!Dh*OQ<F2/OkdgeOSSahQpX[gd<x?EB0d/])*Hy]?nHaUr}F7(i.nRbHb",
    "Eh>ad[S&4CP-b(]cthNa5Oz@{1HBBUH7q+F3m)8W(M97vB0upKAL042v55@GCh&-&G!bQ9rfMvRzjMFD}^UTIsDd.)XWpP+>Xd9i",
    "QNvGkRhx*!0+([[rE?[Gi.:a$hN>iIDxvRVjh5u>3fvgh2<W$[8oOtX&xLgyjyK{EYzPViE1a^MLpSut1]scvW[GSjk>^J9j@Or4",
    "*/H?}q^?k>T6j<N&C%sW?Uy8luLm}6?FGvbSvN+Sy.g2s]dy$CP}?YLwbzoHchFN{2^1g[l13JNX!lm4x-3%QH#3m*O>%X@GFXa>",
    "hiHD)3[mYd/sxP#aUrdDMH5#.-Sk0Z+#uI.L2p6%EP.d:<s{.JJ*a+5Zk=5OV+rt1<q[B*i2@5+Wa=CiOj5QP*NlQ9Rpf#VZZ2a#",
    "W}!-4gtyu)c{UZlFYk?@OuN/kFqToseMTbXV#y%/YNO>MFQt9]r]@eKnkJCLs}XOkJ7?++A!6rm:!m0Afc2RTC/i/HB?z&Wl7N2G",
    "(aG<a8hIDWB!n&J>R/(.MfhdhIAs3^X#}X*k>DzqVZ>[}0HbA82G{<CR0ngG(JIyy^Ii!puI5+iBS<7msda[d1BjGviz]K+OoL/k",
    "YeB7Y]LIi6KosVfJM8z>ENGqZ+c$>A*[*aCt&7BNHDwi]jkRQ4Y/bZJY&a$@lFb5Dg@zq!(.dYH@.(I8u/}<i^rP7.ZJaMXfKfZM",
    "SV=*OMnH>t-+XbUtbR+wZhWt&gF-.r+:?V}:ZX?{H>d&PN1+@iFd8=F/KQ*wwfZ!hEchfSkQyzjPKG@B^b:yMv5/KaH9vkzZZuO[",
    "Ks7=a7Zqn09fdXBXq+4jxFYADyn$]g<xaZZ>>{EOFs6g%zuB&w5hw*I:DgHbUUIt-y<I8Ow)#:]1$y!gjP2S4u[0Xw>^C#P*m!!=",
    "-7<{T?$<Ap!%jj*U1W0KR^r<%D(@OR<GZ)i/8AGGx2!e&!A8-P-M5q0a(9q+5#)3pD$R0zwWEDhRIQ3>tG}aJM8gcp9YpamV/3H2",
    "*]Ut7u2%mmY=66$?J}weLBY(VH?Q]]=?f-vOT#T>@wPM@!p<5G+y5RtiGvVm@F5$rdNn3*s3ol(B8YQwUj9WI91JcoTuj+xXBB]f",
    "9x97zJmFj$[N]!O5mDM>B]tdJ]Fk>#lFl0myGJ40JDLGstT/O]0Fp&@KBFllPyyO#UH6afhc#tb?0[Sk6<E4?qUVOxtOz=OU:FOb",
    "?jM>]EJvmSA+dZ7ucNZW?(/+*}sCMrwr3)UOxO(l!(VU1a=G1bwHE^E=xPIoHP+^k%7A*{LMyFv73{:<F9P}BWLg}1ga%VZz?Oh8",
    "uVgPX6L[{RX!y4pVk/&bE]FJ]5{ajtWm4ghXmt+a*dSB0oW&w>?hu+DkIOihO</Fs.f1J%2!fm^7(][8/#rUFpVVf4M8(HJ{Y0/G",
    "ZXJQDjkm[wabJxHxq!8eh2[#q2mpZIi*IoWrzTHC:XEBKsnB>&A}1SzV2B[Jx{SnwOnVqdmnKsNK-dIroOn5iDykWnAW(@gx+1Vq",
    "tskCP-<Q4O7to]}UXv7JY7n/-1bKYPQ!P8$HIh-j7^$1gFT@q]yAya4SdurVO1Vy^Wq[]7!l.7DKCZuSAOXQE(e]Z[f=aPrLtO8s",
    "e5DV%r8elk[Kq+9A+0S.0eyoX0kk^5v4A+1bPN{&g-GxcSijHXUOj@qKEtvD6OBT[AiC8gXA>=X0ifqr07EzUZZ+a)m9ZVM[y(d1",
    "@7:95Y]m+hFPrFegY<Wt+dGe!e2YqT<s>I^CTmt5OxSfvb[9qT!!]Kzg^q})D.mKQ9tV8%j8I)G5aye{ZHuos5CIORun8n>R))s5",
    "tV{EcY:mVLW1*a{b794$9%MX#Mj6/T7FBLpO.}A1qhSxa)m(#X=}Z(g>Ok>!f)*HB:H1G3.><AFbz1vT/[7$yE-3MBr/x!/Z<eTr",
    "-+<zyYlFcMxD4L$Nd3+kN7czn^VB!#eKsCwNW1dx!PcqR=b:FgQe>$(T@d?+r188RW6ZvRXb#Art(p9PvC^tijKWuAt5km]/>?V<",
    "VxMlv27{c]fMuc!9lp4Lskv&f7wx/5s!cx]zfE*1Hv6J9U.(KC*v(gybfhaB7hV&iw9Ls6jOq/ui%c@}9Fr-xY4yaGaqOcAl@)H8",
    "c8S]uOn-L-0dZYN!LycE5upw@1YcbKt(ri!-g3-<Im4)#wtV&Hiqp0EPsIUF5Cv!w/KU(krdFvuuK69nD!>Zm)3b=c?#Im?jI%YM",
    "kM&BF?gLL!^7T.raser%Mh}:+u9X^<pbjaVzp4ubasCQJd@&>Fv30(xc)}3?KOB2ZbizDXAGX$*&43YHv=>f-QfgTjM(d$Gnd:Ap",
    "U[wp@7DV.5Hs=NwB2Eq/?{ao%eI{=$3GA:4/(Fn#XQoVa8-UU%HQ&nQLA^jN0jNLz>m2T0Z.1@caEt8KdsZD[G21!b>qedjXaVaK",
    "SASs=w{GX8xpn?kM@54s4#kJ@P{M0ED20t)JH#U6?O!]%Wrf)gU.FQW02o%}j-4yqM?42!Q+aNuab74=nhBwZ)Z!GzV!nCA=VT?5",
    "b!<t<&F4U5kOkiP.*BTiKP{w28O#}DGZEUWZ?!iSM1V(9jb!IIqoTN8zw]6f(%pHwOuZE8m@<H2U>?=([PW%Bn<[GH>YE$hI}S!E",
    "R5y1gDpRRY:EK[CE2luoeE#m721x+1Wc4JQeoq]ECT#sdXHwYmM!TkTyKWG>wZ46^HH6md(Hcb+fhUFdzhy(#up+QukQD]L(k}/C",
    "vS<H<BF65Rw=X?]F?]LNSSORKXOklXWQ}!rxm(2OsYTv(w/3N[h#LCAz/t0QOduTNruXYZ2eSTtKqLEt{qBFP+KV682(?xFHA-9H",
    "QI<wJ)PUlaB3L*Id$6JzV=ma3kV!aLQ&DY7^AvfR^:x}=CF%x(bOY4E=(E.K0p2q1O[/N+w)VAt+@R02Nf.@*VF>.]ANm#P%0W}i",
    "p@n74iei%qRn.dLcAY0=e{#.yWAR*/Nw!g?cv>>OrfiAUmO#Jp84yY37ArW[AMlCvp(9K.}mis=6m73ERwSpCFg0p<}!1bqc)B<s",
    "8}FVo:&FkhiwI{BU(d28U26^n.{TPewI{.Q9AmLMMS?2nU(/M>T:7am>C7}?b+:u-",
);

                const ENCODED_SMALL_FC64_M6_U5: &str = concat!(
    "xjStF^J+E0sU{2&o4z+hf0.(F9I[UbH-Rdh&dqL7@@E{sffbaSJK$DsSucNL{Xk6e=aPl3vRz8>=[PQP9[ogid83+o>#ORl1%mCu",
    "[URO+K&SqvJn!whipzWpgtnlXDG><K[VxmkGFpFfNX2ONi{>23Ta>=gBnA14:or0^OsP.{gbYiEIiSPO[sy649^0BQtR.XW{+kvO",
    "MJzB}Y>osrp69U[-S9=/i+6}UAa][EVCYy0*I)uVkI>bb2.yZyfQ%x3>Y6}xL!8c6fC*@RGQV>)sqWdju}bMcxW2kzC>}w4N}TYx",
    "Y1FrZc880+rT%R)GjmV<0CN@+jYHNQ>)F/IrP?#D]}98kbV5)4MAEqL>4P^/W+Mr[y=E!AS64pDE^4DRSTC65jT<7rW0[:.-7$o:",
    "X51>5nyi?*OWMZ6}.}@@Zp5:cTK.6$5dEnPyrtN2bWDww9yj0}rMd>I=O<on200jdF^eqsT7rnl*NcE6U*T(gn?[J4tb-8%*J{Az",
    "]GY(G2fmrs}W!H}+eP8$D)KV.HbQRAE6q3.s.Nurk4k0:kgX&fb^OaEn5iIn(d<}80knN#S81?PxHZ-tUOW>-+^kXWd54f:F<(PJ",
    "[np#:*matu=EIu@H[U}N^PHxG3o)gTfk5)2oF42AoVmYf]0M(UFctnMRJ[FIIB/3QfWZ@ObTKe$Z.ku:)klr9(#4={S67l%[c9ag",
    "6FR72x7u:2&8z<4ev{c<mqlK05]hH2rVh@v1l6wA0VWH?IK^UUp:.I>9[N2ErT0AO-O(RpZNe+0U2OriNy.6mgU</Irq.lWoCJsD",
    "K>j[y5}s1E8>zNzE^8!O^^BuMsJNkFX6huC+C:Qo4VUY@yN*o{9gD4M*Hg{.PNe/k1J%4](JYhUB/<Z/d3NH>&bAA]FBNu!sJb}R",
    "&Sm=Dvc)?OfKFGQS]gF4&+$Ta:NR0$tPE-CGLx)rkxb<I&QqN=WzpVY88s]Kn9%5mfcTf8Dw6g0Y%WZIF}6AaDfAgd9-dWa7%=E2",
    "Xf%H66sG8GD):!mcH}s7.G8J!FYU*x1Agb1U6lbz(*%nKr>n=86^b4<+[%bq)mwqhScEeZKz#m6b>m3XzA[H>-T>[UIS6aIULv6t",
    "80S.qRUHS-Z7J$2/K^pXsr.?1Z=it#aab(SaQ%BAAsF)g7lAe)5-K2etyOJAzt6$)=%<pyiX5MvKf9E^Ibb@&O>amwKac)!UasYr",
    "8>:M3wx}0:v0Tom{dz!S7^bvx07<r8Jcm}e[o^No2gF<J00F@PQ@<N*M=x$bsK%6bh)[o4:W9c[supjZ.NuUV9lC1?Fjsp!oW:9R",
    "z=/&H4e=%m2/CjQaNtW.]>N:Alz&:9D#Qk(S]5K]qd@M+EIBd)H>O.&0n<P-0fz%@!D2h4Q-MkxSrm=oiG.X32Nl^hytm4EPLuUM",
    ".uwgFnZ^2<JPb@ik<4TJjjpt@Yd(lc)40=ji2:28*[=HlFDNt:W#7:h2w]F:alLgrfJJ&*zLgu2&sAjRFuz>^0q4)dX/=R?OIYy<",
    "Kj^dtlre.6E!QQ>cG+K}i/(@$Y56Q6Q-QZx+7lUcUgRTBm2r=:vU%!w0jT.}Hhr)6<EU+L-?.(.-LfUXIN:LpO$I#B!+j>:!%(v%",
    "eIE@6>hgZ59A<(89>nw+WON<QB$=v#<o0v+Fz2N!7y]Q>{Ocz8.D^%vcs/z9Qm4T^qv#2O^as7pU&o**I)6ML[)tqd-[4NYbwepn",
    "VXbkuPlYFf*k}KZ)QCovYY7C88Yiz8ocu?&JQG!f5myBfTE{p(OzAA)8)fEWJsyq0>KwU*nO1eJ5HJ%Ws<]z}5$A]8l3lG%@u-pz",
    "-hr^MCuvK57y(A*%iF3+1-a@rL:@P6pf$d/vxs98P5uJVerJ^)sEN1rA7@m6>Wg*i&ae?2U0*WJb+p7hdNZy4r}9DesBp4W0uh.p",
    ")Xi-C!JJVvg.^O=+l)t}]$!s7C(CfnMT=jx)$E7vv1qltYhKx@jyxIHSfV2FCK13E?N]e^.>>2UXr$NPQZBv&&D.dXN9>/A/Ae1+",
    "iU[XT@d8HAU*4)nppN+@?lBP9{7@&OY3D!T&dwtH/fI+!?GrZ1uLxDMZsd*1+k*qfUdx}i?W*siR^/A-FGJ@o^g1z+neDJ:hMD{y",
    "F:D%Xx>3f!wK.&@V%$#aS9M<A}rW4M/[}L@@q4}Sfp0Cry4^&Pb$!6Fqde9v^MO%vweJcCpl?Rr.O%8H<$=J>KBtfZ4JRPkILum%",
    "l]Si2fC9e9*+v3!9Els3X[%)H-J$JqU^c^jx/^Ra}-fhu3TB4L]&iMQH1#*AvBaIS}?rXnUcD-{2X8gA/(+YPWUG<QW4+WP6$H+o",
    "9aVj=@7UnCFCi2asx7IFphY.!PTC9taeuwf0xkUAw3Ujf7=c!c^JF1(nseFKFs5QmA=8@imt8!%!7{4$JB8d*p.RV$yi6V{dzTU2",
    "og?cai42z6?XSyTY#GcD/+T@SV(AkXI(Gfd&Y{EaB%L+O^j@o%jqUj+G<&T$oXHJ2F1O%rPkNy^tEHj/3Bv{?[Cu.d0hlQcF/?iW",
    "A6H[CLO[Na+gso&oQ&r2d2-t.ihj6#04ZS2C[x?=>ErmWw{kcaqWp%*DbR:a2viuM+h0om4L8uSf!(kjdRVA8EKr@m2.dv/L&1eT",
    "}^(&(l<vwqomnb13mon@!Q0[3^tz3/&7IDX{@wQYA?$SQ/3U*9zM?8xMzJGKV{!]A8J:[WxY(qa[#QcI:rPxJ&f:nLx!!FSDFW2Y",
    "}s8f.hh#6d^P(KW.:c<SF<oSav/BzhO7N%dNaWXq<Y})Jf.0WSsxHFA!KhQmT-N:WQ?Hr4O%s:/V%cY%T%ZG!9=dugWngXO]0+71",
    "*}*qfp&1?0].ZLVS)PAY^V7a!NYry3dSLf}K#pfvdPM8NP.)JTaDc6ewm$)8N01#--6s1gSZY-Eku-[b6!mLxL%47s<<Zj(D]/G:",
    "!(9^9td2$C2+pJIGLUhjv^NbUH2WwvzGaC}S^24rn<w?fgW}b4*iRt-A09<qq*[f#(T.@!4X%H@BSxy4Sf27.<)}4:p+L5Ddc^P<",
    "1&?fqJi(:44@4y])vpjtEkt/PU:hJbyiky$fo+h8WR>cs}(P>gNBq>:{%roW(#r?92tLp-rzzw:xE#-*oko+R<d?%P5/N)=)d-rH",
    "JDyD09aTSlmYN(HfeLWLpP?ar+.!RF2M!K1RJ=+Ev8gX-[c/]xQf>*Kavhg5u%9!0Q*YYP^b?EzzGWhqE@c:J1-^CM&!l$/!OiNG",
    "]RYz=oQmJG8am4VT)W/7kRf)!V28#[lk4nJ0cf<bDd6#3-#]-xEvvZW9hPb!}6I&HIx^>?n?[vHb*IlffM5%mkRU<zD:um91Xz9B",
    "])cevfKyqjPL.cHk*?*d(s-yBCqcnnt*dZZJJi<V:xOCMsdwM*ZvOY]cut6TaVT0]4%$9mnHsFJt?5^q1+k{N/0M76WmzpKcu?)x",
    "nqJ!II%j53w^VTytF}#7OIe8FKw.gOU>@x6llh4G[/8)seA#{)t>J>nBrVu2hHtbh8wRH13@{N@i2#kpZ9Awn*@^S1>O-J}U}r=7",
    ".5{/.LS>+O^Kz1RRLQxdvOgw#BGj{}5wjY1&D2z<T-yFQZ2a=i5hma%/K]N{ET@&VF.R((.w0i*]SyP@d^/48o4rKE5sDlQg8niI",
    "4ar-<Wiyq^4:x!)/VDW%*eqDAbBbe?}SWDi-iuFVN(e9%tn0R[:cRW$2}}^Y1Vk*kW!p[eZ#&i(kg0MGA+t5d:wr<u)uXbyC3aM6",
    "dX5/rb26EHZ}mPH]IL9vgrDTZ/pi8W4)iAD*//>7EI?/x<@R-gik3oBR<r{/qLxTyKHE2e{?!Y%.N60IfM@k+ScxWMtU<PEp$-PC",
    "kP@uPH>@)WfE:9o4Bs=hVJop6.II}<md%9:3OibV+MFCu9dxVSgj&Aq?VKRbr08XPrK%x**b>XnFoyGBWkG<9TkFQ)F$<hDpT9T/",
    "5xc%#ZBpx<6Jp!<vHYJOLc%EoVZ+Zx[O$%03ayg6BTjQZ>%EM@D#>W*H#N+:xulSa<C}i):H$Xt:8ECf2&#Yj:8@Vv&wcxQ:wqcm",
    "zKC]od>03-Oz!SOlzEF)?l.b+9=)Pf3=>?3dK@p#LlH##xr?L4z=RSQW!7[e2/!R.W2aN>qK.!(YCOZMc[j*L00YjiVA}$x03[%l",
    "kRX(]M@zQ1PJqSor^{<TD}xTq2c<-jy/H9>XW#6fCF?oXX<lwgVmVo)%9UOCdyT<&9vdb/[ZD*q<yt%r54xf:+R48L[t49/et<:R",
    "]SR:/8QoG&Bf{S:}m:Qba$G7)U6-t}[[=&tGYP+Tl:DqDXcGJb*3<UR:/2Xj[w2z]bo!x@KsKnxt>lf6u0U1NmY<M>BY3vCv5Zj<",
    "^$Lcw.%(LCxnbrUDSv{D<E{VV}2FHGv-MbAF)z{2c?wYH/Qu2cP1bU2?)H6V.7ZtKsP4IG75YQCK@MCB6@JVBbJ*X[tDwb{[Bs(r",
    "x*vl.-ju+#Tx[35jrFgGAkR6ac]e.7sLpN7Z$=H-ZZQDg0d/s9+/ET:PtIg6?qfeoJyFZ!s9dXh}s&P96zd3)XdPm:1eZ^:DzJ*s",
    "D8Q%gD0$K508f7E=zMAg&^EfbH^J^T]B!z(21a[yqzue6bH<7-&ZxL8}JxX#BuVrW^@oN}N]i9D]GFc}s0PH-)9eJ[D%?gZU>!8T",
    ".4gK$FoO%@oK[#7^Xx1tW$)4dn^BcldrzQPMQ*2ETwb!f)wK}^0YZ-N+(-U-?vzza>Wa/Z:*0&&J*v(3NhSR1.ww+aS8*)JC(G&?",
    "gPXCREGk%6mGPCH-vx3z!<LiQ^9yW+4a<HLEM#pAg*nJ{ShOcb&efVXGQ}qt0mvdD@8NKB(%hhaOL@j@qeFXmo.T:+3.^8O8-FRH",
    "PK431k=n!siqZg#g-356k!In5g9uLfya3CMiX[dS)7R8+ssIYYFh[%?oXg@h]AyNAUVg#)9fst!hD(@<jF>Riy?en-<y7P)PRatd",
    "kX4*D[cX9!M{:51Fgen%*f%6QFrIivt<OG7Wd{sTZ(K8W]O^=MYGB-TP-r/XHwoYui.U@8=us$aU^@h{hK-eVYC.f>v435/q1eA!",
    "1qswuF$mRerhIv.[[k<6tS-XkxVY!T7:9GP663jfXl^[5[znb%7$UT$*-?M/lvjwG=<w%IPOPkNxJq5}Kx^D3Sk4#r7nvD+B{<f<",
    "/B=AxmNtx:3vHz/kUDpk(Lrdb7eCr4rYMa3flhPRkGi^*!=aR)CrDl@hmeYksHpAdDC7/xtaa/>sRD6zic!<{wz[d+}M1bup>Wuh",
    "vvXEN92Zcq}.Vg4F3lNHj-Cm4^gu/3Xv8yJZz/RQqC$]RJoVXwD{OQ.L=l5{lPY&.cgtIhUfN3Xwi+QT36#ZExuw(wTAM()!4EX7",
    ":t%I?REO7s7>m&nA4LChQBDHju:}I#z7Ua/05!#kysscnbB8m)QMJ^msM-R@>J+2&<mFIGk6of8Um/6)cY^Yf9il55k(h=?Efh=0",
    "h35=ITz!lQGD8>-ZR?earcRLf(7je7?a[*3b^A=JRUv}<46K9NT*Xl)r^PS^}y[y5m5-kXDs*9/H+mTIGS=#w++i!8&SG>b>3:0d",
    "jHdNwU-?Br&f3jp6TMTVNqQ5dLSl@K.HV<zOzDSEeV=SGviY8xyT&wE:oSquHG4G(g&21.:fcpy&A]f<DcYp2-t{l*H/2+ru%=5X",
    "*[r1S&sDQo-{CK{2Hw]7h<4gQ<9o9q7&7Ij[k>dO4>!d4CDLEuI@EdseqrPcD&=EzdgAHCdgz$mFvcPk&f)k3Zn^jbI/vlgV*-i6",
    "Q(Ngy4UPXLLrZCdo%h#dLG.v=+BnHBx1)[3vZk>{Xc^vx!Yx^m>Pl$xr7=1gotCbM(*L.8W#4%:+/fZ#Z9fC*J&[8HaOaeE*]^>A",
    "IzPN(+//aY/NDo5+LvX>fO*TY]m+#kaviYNCMowgIufP/i2<zj.>fleV1Ip)?J)y&ECIp)ZjI]?0o(nJ^>p}<cZBXAY>=4W3h{.:",
    "%d]t3v99<VVZ]Vgd5PRHIW3S6&zbO1&6cnoGG.97I4$0zQwiLcu93dKRpk=m=C+@g*:%Zl.mbT(1vrKh&GZA9sJyLbIM4dK?Svzz",
    "A3nA0LOzDTO[7a2]4q<k>Gd3[R3Ka=&&RW.:r&mFI0/(d88.-mY1]8}R^GLux+]5V3>PN9^#QDwFe?j}H.nHS)<-iry3&I@M8K@3",
    ">@y66A]FT:>jx9D^o[(DPIg+qX5cI8KMwbV4cbZF!hoLaJX3LY:5{l)><:c=CuNLK&Zk]yRZm($u<[LG(X*Ulf@AO^7){<TNcits",
    "9.!=haw!6iN{RO%0{AJC=(zvktXxD*S*YV@:{wJ>Ko[T}h@Na^syCpBl-sh5q>kbDmzDF#CDz*#W44E![=-7dX%sv2r$S!x6c{X]",
    "4Teb<F0kvBD(Uskv4f^5=WqGJEQXCNmz@K@Ywe:^w$2bKLrw1(IrRcaA#SE?xz.8e?L1t>p?%pH=p*%D.o9SV!3OtLvSf8%tU@+W",
    "]Fbh=k>Q[=mhgLw-:WjR7cdjn&vl/J/>:RKzK^og<3.$wsPxOURwyq/s[nk#=BWyDmwLEFteHxQ+P$a&Br8z=3}jnY=b6]NAEpu>",
    "!kW0(M>qbqN=(@G76h/CzwhiDLH:wgvppDy7XOGQHK}cxBBL8<KD7%b&</ET7+VZ&.5e}gnw&wfdz?oiL!+OT2>F9ZhBagNi?F*O",
    "($r@AE^n@aTY3Deq-bVisCZiBgnBXdX/R5sQVTQ2BKcvs:5!r:iky@{!Vq?oM&I7?spUOH!e9D}R>:saYO={c2y7Tg-6v8gi<Ih*",
    "*Bj%x5-=%aw5r=djD{&0NRG8>]5tIu4F$W?P:&ngIcA&>H=2mFYYK{Y{YTFS1oa]R+*)s?il!!z]/OWbv.SF*p{&Q6tS9qfW8@&>",
    "p.23uPjB6F]jl#E4YK(<4K@g8FbH=W1]mBDl^/g4iY!$e:rSjGGjVR#:0ApKJiT##t+)}7F499PVjXrA0#O%rEK@aBvcjQPM[d$}",
    "rrB%t23*o7f4<O{Jdr=HN5bzG3[+TB{BOLxa7IfiDlC?<mMuw<7%h9S.8WK&hCciDd4zUGY+[*O.38B5]JbqiXa{y@p[:sveefr)",
    "5on5<:.S:%{YZ&nS[e{?Law.*?ASs17/#KcX3ZB]+$Hk.4a+>JB^t84Ig(Wdtp=CXKm}M%Wa<uG*H:2vZe$@YX#]hIp2tIaGTZS*",
    "+7tWr+hWuM3.QJ-0amE[wptQ:df^KIaqeoH)va<8xO*^]EJ6]NC/Tg8Vn>j+{2ER!y^Fj/bX43xfT:uZo%K/k&m8eF.P3YiU!fS(",
    "6cGDLr=k<&<VOgeBu9$$m)OK=x]4CQH@ky?u>]9xRqPJXoBLO#oB7HeNGwToeRz0PEHf)Q)$NEFQe=!eoxFZRKxP7PBkk0N*kh(A",
    "w(iwdDnWd+6FyPyvuyx1zUKw#I#$Z%l1bvd9Z#11Fmhc+v2R]:oypfvvu[5XD0/^GTKkh7-b<o*<z)HEuYRw5lJl*ByJmh7ULen+",
    "(2nz)cRV3NyA$%Q&^hJRMJc?B-=KvcW6:X-7@@mjEYb#Oug/j3{g$eIfARCw2Qi}CaLAs01*rS]N7}]H53gw%aMUn&zVav+2ms.H",
    "e:iIMv77^4v=JGZm)IF=fm@&gY<6$?5hs.W+)}9zOom^[UOl)%v:!I{tvI/<@kf7e0Df!UtMz7AgNmh*jbTJX[luEqqphp8/L)ox",
    "]C8$>p=IN><RjBLWby}%fIDh]!u]MCtBY]J+>^piV)-J0-gU]0H>dVKmQV@]1kW+aP]A$O40YB#ilVuQ@!Htk.mo/:=D@D3swfgr",
    "&eK+IxsAc1.v4[SAo&#Qs>HZay%rhFriUn[Sk.DyA-($/!Zn/VI4?!X]ZO.u(zd0nVzVo!0}!B+vB$/EMPQ8q!A?K@/UsV8g)3iI",
    "NJX<%5sQ[TX7Du$Hnt8o?K!HC7KBH(JnISrH?06>(v(%+*Az2fGTne3u-Th&yI@x1pfC(Q6nmt*F*<)qrWLQBvV<tEFb[m=7+^tK",
    "kj08Hba6cewZS}dX0zXfr0>m(KD=U9@=YIHLVmg^<E}]6mSzwvm0/zc^.(<l9&gRQv6KAfl#uUL>/kl/fRC!^}vX9rZ!OjIL8J*k",
    "Z/{DBXpA8xtWqkh[jPAp*/%A:03j-{u#E<m49<:z6V@>UL.k?@r6cbN25Ep6@aoxKkY+q=L.$2PX>rq)->q!}LdCeqpWZ87)PVhB",
    "S>?j&^:UoXYY<HJl$8DC3qeBM56QxOSlBFtnu$MC>k{V9vGY3ic]-J!xB3T.-J@(myy=K>j#>L=^xI4Zr{CNRIYjH!9!RA{s/vN#",
    "rESlch9!1MG[)KVqN.B{fr+EwRZL9Gzqd/GQuU4U8xtOq74kl*)l4yj}IWo}@$]pXyb*:SPf/?E@$F}QJ%20/E8Gs8FA#79TmQG$",
    "4-K]3t7&+ovE.M8nDUG%}8$ulo/@J>{P{zOb:p+ZCL:2abyX^?41)m}A:Ddp3mJ$ToV%(U9/l]hS/rBWdrBibue?bj/&Q.S)7!g^",
    "XGE0EH*.ZCaN6I5PL10.@1V}dKUFG6.C>(}+:-T^m8#TF7/czWBx>(Wi6x*CfJQXbT./KKL?EGF6{-vz7!)aR^kIPIC$%hVm5u.z",
    "eLe2!Apd8ENihC<EW9h.J=3wYuZRtpsvF:]W=so$c.i@zAkUdd/>H8(vx#/B^R1g@PvnYZ1I}i5z&{*dhwFD<fOaF./OWj6eG>>x",
    "G+2MBei?>4rmNXR}lFF.O-!cVtD:RYyz/WS[lA&Ai[-jIqvP0R>&0^HrLDEol(!Rb}B6RZH(:UY?tFh8ro?xlR+PB<*42=<e1+/:",
    "BWYv}e{6(MU7YIjD7HqnK6ZFLHEr%.:nojcwR)@Yy)NKNazqRaICdzw@Tb)xHjgeS?dr!7uMN0CCQ=R9-a:>p2c?t}KMPt3HFA6c",
    "B>T#4Ffww-=7xLJCNSSB@QvCk4lrK:/F=U>&okhA1<2]t@)0smH3O@RooI0yP0g^JOJx]!hTI<9kUfRyA+E8t}QRGbV9JBOI{34M",
    "Ygq#/WeWo)1o2!]H%7dVs=r$Tf$ma}pXlIdy/=e0?v{ge8OX*bYMO57/c^EVlx/tKslUKO^y>VaP>1Y#w0}RPhC/^NZP%YFqquc8",
    "A+>uXzf@I3xIi/*{wU{)zADYs[5@IV!>5p].L0Zs2C6?^r:GdE0HO!WB9PmGC}[Z2C.jTkX8Z8]EWU/2dypiZ^)5BF5eH^1+j%2<",
    "SlThi&#<:Ma9XTp![^Z*A8aIrOy4nG]L1uuaMxW?aL{@$aPc^Nu#G.)?PG7^/Jn6={aW]db^A&L6MYLM=]SKp0hZBzOo}GN9g@DU",
    "puki+nl/09igHb=@TvTDRuG<Yha00XCh))h3Buu*k*2cK>@Uj{7lVG%aJ1QK)Awkq>YHx0wZ@R3rg*UkTL5frR1BY2iiW>DzZAv=",
    "b^%d9kF{L^tX1ePo&mlv0CmJV]5luc5?]LlxOq76zmk0WUo7!GI}iV$fX7uXXQs@K",
);

                const ENCODED_SMALL_FC64_M7_U1: &str = concat!(
    "F@+KElL.(1C&%g.Q(o&qz!k9BYt2^S5p?Hc5Bf}wuLJYbi?YlWhc)7x<pDY4Z<m-Ed0?*P)&*@ewjRi-lVF>-vA$<+bA)X(-Zy1[",
    "a+C}uh3qjsldf2KL-[IG/heRk<<an%k.:LFfG@A0h:fyrn%peSU+T@Q5Z0TcXxD-))$=>lQyGBI5u={m>=][byA2M6QOIiV1$98d",
    "(PN:2vfHEhDbRc{9H(%?)K6OIgOs}odK.se3Tzdz5oTtKwg^Ep*p)?Q>^%UP@y/gmw8NqF0f?&x+!sI0g]<@@@5l@9kTs!J4+:s:",
    "}w5S=uf@}4pv:xd?Uol0li!yx5e75qj2P]0ZK#XMMJ19Jf8{Kq2SX>FwCb=iZXXB6od%[<AAzzs&?{a%ToU&ufi43@{JkucS/Ko}",
    "G?$Vg-sEve{vK>qG{M}{5&.CQW1ZL6=<k$2Hu)QcT%^sju#FR0UmI^sees:sP@>1Kwa@5F!M0?SIaAJo.P4%9:U/r}9]oHJV^X6G",
    "e41oG@CXF]jj).1J{][eRO#.4jhjxITyk:88Eg9ulWoqdi8D$B6g+RR*oL.hyWSDgHM^rMe>&:!WYT^pK7R/.Z}PSSk-ZETH*Y$&",
    "g&oh8i5x*jH@dC2+8l/{M>i4RvZtU&gD!at-HF<4CUm&@wY2+bme<6$N3F]e(toG-3b2YtSIw*[XhG$)oiGoD7SewFL>wJH2NID-",
    "![G/IR1cySRVe8eX%?J&3#m7J@lUHcy<-L4oi]>agC20c+]HdMEbWc0c}YBY21[kB>qvVJPF:tz!a}xOn]-[8a)(r[n?L9D7*jHl",
    "f*=+<]Y4pj/k8[H-oj&$(:EM>GR7^#1tALkhU2-?@w^284.&Dp(C1jITc1mkP-Dl0.b&xQC=yr$H$!{)CETV^?GW77!Iew-6UcH)",
    "<>6O$uqGW@:Mb1/aL#MGih%<fM#xszBER5GhLBFvchhgsCv>FKeV6/dmg-V*I0z381P0.:JgV1(p!pC@@^]Qk&*6}^v1RW$u#hF$",
    "l%zT+5/JU((FxN}%lE9RthIe2Sdan2NeKPV<Ov:$U?gc.Y}BG1Qw#(eN4CzjX+MMB%lAxxqEMW<Z-h#f<}c%]!9Jw.L3m9k)xzhQ",
    "<B%8qnL4yB6cOQl*A$@z8d=8%>GHiA**[%GF}Dd$l)&j4]bkzd)1Ie%yo!t.ttx)=<+::n)eaaJ<*:.v8hN{#BZZcut7m}:XWnX8",
    "Rx!)H4RvYlNw#Dl(){du!J0p^hN(z]sq6KOR1rUg{/}ZID(J>:y(fEJ<FoWSFYA<o(r>I[M*Ll{R051FcVbu^15Or@y0a<!99oEf",
    "vI$6oHtgN#.7S[)fZ9n:poq%T^AaEj6*W}{AnG([OjGc.oyD%=QfB+jPDi16[5-pb-@R4g[qubp<9G?1Q]r)#(Gu[--7Bh@wc/lH",
    "c%mUaAHXmn[dll{k!3y0&!BZ3W>d}F[QNdy6*DK#Y$orqvT@mcC&eIlsn0y)j$Wvdn%$S3+$nOZJDRFrs$Q2)/f}ZkZ]@CP->dN2",
    "vT[P{3EDHIg)[&/77u!F*2ltLcf77FJ3!G+-^k?wW9auurQmrhKM&8{REY.um2#e$*:lz(t8]R@K@fYL>QHNnA>>*vg]ItbBRpvX",
    "*d)n-0gfPAXs<N06N=:&U&l9PZ)qamcE]ulbb:wjXvMB*xK^Ey?U:dZM)vqD>/g=T7Z5Ze[jjxUnxgS(^I6sc:Nl(crlZ(BaPw2}",
    "ndDR59>F.(/Rpt/KN:6#zIWe.SWD:*5qz/1tks5?-7FB<Aq*}x/Uj=bA30MyFAkAEF!27Hu(rm.p)c^JXe:trxLZ$/Fq1N%[s)Uf",
    "PT.f/E3JB0<v[TFjJigv3t]TMLXmV)U/0xLpcjrCi2!>WFBUlOE^&<<LSV4X@i1O-Z+VlUZ#uC0ik?P8[h%vS)Z5m[W98Sov50Oj",
    "Ktgib4eks^7=4.%S:>b6=qiVqtPJ@gXr(IAx.WSKftOU[NbuHU?q{:.y&Yo-!2S*sI)rD0^x.fk=$ok&*S.s+v0G5(jQB6Yd+Bg6",
    "7TWup:!OC?*FM55Y?G!m7+mCROLA[gH3*MP&f^@=2C&oG!hvH[91vDXgQPS-IT)5}2?LRezLfCgkUm6HdFh)!.ik2kB3pqI0#aJ[",
    "fb+u-DG5bvCHOjSbd}RKXHox#z8hg8k{4)BQO5L?6nc=R5B0^*{EN(/XX]TN?x}qBbc*yAOoRC6@C>W@/Q7OWiUrw}Wbr(agv+4J",
    "k/5k?vMZby8RaC)M1-Q#TVdAoW4iHCh?NveJdaQsw}Zl0=xrojT7J8eI{(=haCtAYj/%M7B!+c.0j$b4U&2U#0FoKF{2>H3M{Mwp",
    "-eCnv!VUQUX*jI[tgQOO7uOGcV7YPe-dJO6g]*U>J<U^$7yiKA/t?7d-m6aE)O%a$^pfL%BV(ea0XK<y)E}+!e5Rz1O3V8b792F2",
    "85Zr71LV)s3Lg$7MDDXHg]!EbeoIE^pN6P%>=Ovc7JCc>1%B1gHR@{ICw+19U#E:pVSw9V(@KFb}!.wbholv)!d[Z>=A-bo=T/Zi",
    "2P.8I.-2VI0btiv(%r<U{a}K*Sn$j%k-TsbB.aogikM=Ik/6!u*pi@(NuK!!]&YzHNh3*S?#TfFPLxoCVfv4S^5wq6M%ij%S5]7l",
    "hf@MuNEhpZ=urv8k3qBfPVcjWp]E.:oWg}$Fw4Nq}#WFx]*1W5+eQc*jH.#mi&f$X/:GmwYLVs-M#}sJ%4!.t(6L{ySPKQJ{k$X8",
    "vyd]9!2))TzpWA6io7Jh}[91]vvG%71[pIk:9*AuO{oxXJ#Sj7F%y!#H0(}.cME1!c>uFB^(&vBRyHfg=ShqO9:D0lZ45wts6z%]",
    "kjBAx47}rS=#N(6bl&6HKLE979K0:I9jXZ9[m{N/J!)ntaF8(Lgkr0CUmJ6B*dVSM0Q{vTYIx6MIqiM}4Dwu<2w:-f(!yEk<Dnri",
    "M[B#z}g(%MKmq.NFcmeW!tdHQV9dSJd{ECnYvceX(c.Yc1niquUO%r2jtr3-00w3th4OxM+J*^]DMP(jcjYKRcS2De0xhmoZ]Z3/",
    "HSjf)Z+K3Bm!(7d[PH&]Ng}%2t&^I1E)A>x{^v=!.xD/Wr&[tAtFsNVl*tZQ&zc{DwGEj4b1.n^j{!5GTN3nbSNEd@e{9yx)F+u[",
    "niv2r-=Z91jH30v880H5vx^QhvOhS)gYBmmuhc3nK9mY&gt&+SC<aw8k!v*}w^B>?9WDSRFBm9-O=vb4[/1C<?d(jrNz+o6&B(-p",
    "-.)8RbJYX7?c7A9Ypa<CV*rVm65d(Fgu$:JS3wKV=?/<?f)OL*El4{7{=bv?=aS!-ncu%@U(j5OS2y=gKta475G3OwnEj9m1ND/n",
    "8F@iY[jPdXt?47.05h70-/)afzC^S2:m<vj/G[::5sgUQY6=KVv#b-IJ8W9}Tlq2@dmcwnj>sU!P9Nhme[4jj9rGb4aX:5)8VRp#",
    "^ISiK)bK6*FJ*Xer??pY*E+g6I3gvLWODLd1dGd$TkDyWcp5$vtIx3rOlHk3HQltgT$0C=dWF{OA0G-kaJoCP)^k]}u]4vpHXUZH",
    "}q!Ou?<w@d0TZS@>RLy^A$^[58kwB<TJ)9Apx?MFi6kl}l!AV#qSFC7[tQ(H@3NGpH4L{t)[Q+-[)g6lDsO2e+t{nW)&[%GL+[^J",
    "RCJ4gJ.1@sCk3eyi^DK]?OL5YY/jSS1[>K3)WzrZx>P:{Cd-F>1tQlD(kKI4Eu)>%&*8+>ndpwsQv:11Zrqx@lgx4Wj@!VcqOAM5",
    "EEU&Ik%5Eqk*gSsQmX2uBpH1qcK-yaoZ[JmA7x%)-Un2d7mz%2:OupT:%#L^%0]0K@oy)ObA05BgjZYmZQzO6r0aa7f5>M$vGB3M",
    "l%V-O[jM?Bb(t6:dP/Cfa(ah6XzAfdW[w)0Y{AX6xmhu}}?g&Wd^R#9KLB9O*T>=8Wv(2id8WF$Lf8a+*Eh/s/]=w15avd{}6^%W",
    "<$lZ0jW)A7Yj&k0J(OArKZE4UMBtOY6sAO8PvRD4?cho&QV{drh]Ew-7&LkzJOtU?u#.muULgWL}8cHh+&YnBC9bwdI3wX@Abnv.",
    "NCKpx:Z1XPG>Iw/A{AxXLx88:B$(X6NLy<8rZjwYRfmQa>lyp=qyvZwF:GUO-)uuJ>OO(-az1A-bx@1S<u7V1ZmEl}c[[Ae)Tdgg",
    "Q}M/vWh)h#rTk()c]:VeVn6OpAqnoMKMIA-*wo.&EXf[flfX/>2xER]rQ9/6W4PT%{P@*x5RrXv&=.)%2+z8YMcypkbpOdbd8:7/",
    "33iNtT5e@kyuX:khSil4&ahR@RTCto5n^Av.x>5LD]X2:.f:Ox2vhddd5:z-@mB$hX5^aj>2Z}jvezAoIz8YRH9i821Kg<FR-10b",
    "gR1pm[}xd*+<FPD8]!eRefP2Csz&1sf&JOmJ$dBqgnXUC=oYaQEA)?=/)#J7}em=B/7vANpMkoogFovmUBgnh9>s0*<>$u@e.{VB",
    "t2DMUj$3$9evS*n2$JJd{[9d2dcfE5P1HjHq0H/?v4NxXFuGx[42}8DE&EYpj}6Xu@!sN&7h!Vqsi7CeGQg<=DFch0pnbV<}}:.7",
    "!F1aVwcOnY@)%oDaOJ(rsJIswq<(:X^c[Ic^h6Zm8s:H=*M6H>Fy4H:aFHeA*Z1YVM*rtrX6?VEbS68Cf2tSx&+yJ/C$4}JfBN-&",
    "=EOV3-Jl<r>AdU^F^U4hC&6)Zo8LkxIx*85a/&vv-^UoRxxpwL:>[m89%6hvU<4@CyXj}@^sK2mspRP0G=>L4S<4NF5+TEkIC.p8",
    "{$Aty(Wf45Z^rYqT*[>N)[bpEd7R-dRrSYCr%zR/>kZ2}L4>Mxk?Z4/{.S27aBvFdp(Ji60/=RhMJ1PYOpEU6MCGod.cID5C7UZ*",
    "*Xmy9ig$00dJZ4LrqMc$)E4hw<(JXVcc:[3rZJ4:O+yHB:By3G?8E>/)wQTW=ww16v3y{XBK%E7H(#KpF8d&KRiB-/+-i4zNzG]R",
    "/d2dXoliFl2FIa&fCpX7SJM]Exa-oGtrY5G]>Jf)X8pA5PM(Qia1n5u]NMS%>yRkD@08<R?Bn0*2>C8WSS2sHPc+EtmX[<5oF4s0",
    "LpPnq?Q)Ay7C0wm^%zcULQA(L?BzJECcmG23Hni=1%m&%m+gXP[k&sEH2di?MKZX(7F+gE{+gBnauST!*[&Mmgiy]bOz=cmIgIr-",
    "uF7Yg*tH8^6niy2w(6)vz8%.>A(Q:nb(SJZo+0yLZ3:K^+Q-!dZ7$U2^aL=?1$+=$LU)=hr>kI15!58gq}>hlV7[@{7u[vNMcR/Y",
    "2om3--CHh<=rb.kWng.P9le/dE.cBX{.dg44E)dPLRfTk-q5KW.x).}*>zrkxZ.-Wgd5mV-zr^6q*F5H3mK:)GF0UDD(tk.8t:+8",
    "?49S4BCiVc[a-3Wa2t#x>=ul$*u0}1j.%^OO]5hQ/pA2SSfbxt[EQpo3rH$9u%esIZ?<ot-@1r122xz9B62(+mbe$I@-6[3mt0S+",
    "Rr@Py48pxa@St!S0foHXfM{hPXg>VY6>h!VTQV%DfANa}f=-bnU(CMNn]fky[)r&WR1pVJKMiYi}yUYgv3/Y.atkN=s0?s/8?#+O",
    "I.?4<&=2^H^qMVXz]-:e5njCB)LT:LkWk5rGB/]MvQjZLHb])Wchfw.jpN#E1huDL-A3tfJRabK9W>X=RKQ@aoJM?WaL!V:cCpqg",
    "F>bWJr7$U30[Bzp=fTsJ)1M+0]KTTTSG-Pg/+h(G-=c8:3Kk0xkQ5$p1o8aX@Hwi#re4kk?)T0cYz8m!pDqfl-h0G09gdE8@AEMc",
    "NedEV3)/)Sld9[stdsO}@Sy0JJZ8EoZAo5Xo)vdiHjQ8M1hr>X&(>ApZ/}mjgKV%I[GQQ?^B5/A^hI%DP%w4t@fUTuq2x#I^4Q4]",
    "[>n^x=7@9qqovUdJY=Ntw@}RToacwokUnawZE/V-=X0x8PgSJQo#MJ:GOWVjN)v.4euMMFlGYZWsOjyj8Q0(Gbc9jT8W%LC/gO+2",
    "TU*2vE0I}WsK@3^=Ktxs76ub4X+w6D@?qFr*gS4K.h^D!N=l(L2x+?=6lJz28.%j.veJzQlA?cq:Tj44Op@-jS:^}JZ1rR8hay&+",
    "1QfGwX2}5ta(i8T5C>]X3a2dC+=Za(o@%q/SAp1J=I(iNHOkx{Pv@qtIw9-*)E[aPTdHX4ao$GK<8KPLna]S<G/A%lp)kxMIW>DP",
    "aneh**f46jAMi7lhiG5{cc<m?qRh*IviSxGE$c>0yV!d1Cq>q/>SHHYtz5543MZw5QCIwMpn$cPRc]0vG4q/[Qb8[={1}h.BiRV!",
    "OveTp>IgOJWFdf{{CToW6EThJlLQq1Ul{DaZ?c+l/Bxb&BMF6qI/Prv5XRNoZ!J+0y-&skLfy3(U/[)!bi<CqXZYfzCo&Ot9Kk-g",
    "yvp{Z[o/=1{05=o?$6dE.]5YxF59A{A6R2#rX[}ZIsC:M&qJ)z>wzt8M@kB#JR}bn+7?<*?(191=(.!<uP!SSaWf)ee>)AKfk</N",
    "&*Ra9W+EE?b>d&#6u0!.:5)jybQGrJfZ&Zu^)#{w+lRZ]e1Ebkf[F*TSmc49F<AkzmsgK*dS/R(FVKz=-?]4=Bxpv:Z.PE8B9ucF",
    "yzg<8y}8X[1LBH6&#A0J{[tF!+gXyg5L.aC0AOzE]6?e[(mClm+S#VIH(D{G^qs6v!YYj3BZa<UMDBJ/@f:uzL{FPcOfPZJY)e%d",
    "!2!Fv^N&aPVZ4%Z[4n//reR{lH?IgHhyL6dp(e:Dp&e25[l)w3M%-I1+P4oWeA$c!f5HxSOk)A?:ufQt&3k$Gn(u8iw{b#A0k3T>",
    "6.@/:5hWJM-lTYiQ(.fhY2l5jl$:}iKJ[.c?Eo3<e<QEv!H!{biDBA1V]1HwGW%RD.VWneBBnE0^==(T&CI7F45gu#OiT}O}[9z6",
    "D{d0-f:9oD1CD}uJL[G8]AP?e48+4/@dldT.u!sEyAM}E?148J+k^5R)@FH]o$mw<3.}az2qYk<WOMoNTb/!n^C>kvzYSVb=[Ud8",
    "{48rIB4Y/M7Z2jILq>Fme88$@9wHp5NIdl^o=[EvB72-{seQR-APz!!=2CN7x^Rm>r$s%/+MHzH2?kOi+gawxm6tW=s$1wN2akPT",
    "r2:BL:-Er(utIHk-z?/g-2gzCsx)h!qWMR?mTxl(&t?f@1N-}q(mKkwov24JCisSzUGc]ii^RmH&VDPk7I<Bxf#.E4+]xY@Q?@Jl",
    "X$W285X%R})/9y6)ma@RWx>j{Bds7i9^990u7o?q?S<Kx:Aw-v[^)6him+sFf/JA400GHtb%cVvk3E($-DK<pRm>*3[:0FA0!zr5",
    "=he^29a*@&&)f*5*h}o$W:Pcc}zp4x7ZCQP!t8xy77f&GtJqzu->u30WX-TWE^vb1aNC)^M:-9&4%%zJgj]>CZ$!0mb/(cjr-{q7",
    "m#Z6Ncv-A!YG^yIV2&-8N(dDmL9mT:>Ac9nkmhREe9=pY^ai[ml3XeFVVujk31sQZP[Z-PAYwn9v&(n%am$jIK+CeE85pDg%cSX#",
    "/@[QzZRhRuhUv!HQ>b.-f(-st?g?MplFcK@E$0D)9A%KFB*tBt:d[c#icXljneqVGbuV6Io!O1XJA?{+N)X%A&3A51C{MxMkYHIL",
    "at={e2.=x{D]D{CmwkEI-T(E<zXCY95R.e^{?=+s!#E{%lU{}WT{QKVzLQ2>fc&9.}Nq)a71>pP55@7(KJp@}C7Z]2B@KS0Pz.>Q",
    "t.t?^8JD8Sr7FJ6Z#QV(=%7uI}LXx}IEn9KkP8yxliJv.cmi[1!/mG1[<dofQ=p3wB@Bb&p+3}d?rudFQViaN:[wotv@rQb(#fXM",
    "W}ML<MIo<YP8hhB@:RhaVVOdtW+>&lzH#ui00Z[ntzslkp.?.FU1pcbN>+yo3Qo$unKG?$wLv?w(H4iSB.OR4?$5.t[]3.aSExDk",
    "3naLarRBi>E[YQI(McW5jG&kdFvf$>kW]}<YhrHXRV44s3@iUj]<saI^p%^*p^R&T4$cJ5gT7G+^X0}Y^.7az-E#@e7$(2Py+n2*",
    "sbUM>z*dCS&[GhIvbOGaToL-sPG<pbXcFS?Y/5*5@lMcwJdjR!=UtFOye)*sfCd$Z(5--{o.xb:i0i$X/V(H<=Bnf+MCb@8[mg4.",
    "jvOkwtMVy*={gt?rN^>V10)j@Le6(c.Y[}uI#U0m6)RmxohkTOpX5hk9ru>*foe3aqW1L.Yz.p.bYp057+2tCUV169GL}1c/R.-.",
    "J1]Wk[p%}9k+!=J(}k+<Q)$h]9kwU&7>1==XzY&?r&PZ{tcXP32!hg]5)FwIO-p*9KFM0>!MV#QkY}*}FTw5KtJ<)9bhPpXare@%",
    "N0HzH0]4xLqeI64>O2*qiW3QGT.9:eiN^@uHPNw+kknD#f.CoaE$W)r:yW?7.SVtgA=2OQTZFE#o9qmVsyk@fqHAjuzfL0N1N/v8",
    "nh2k461XvZZ=yQvLjHy(kT1:5p[v6*vIrupy6e-B57kpd2w#W/K.hS%d7r3/w&Mo[![d6etLDJe%76i-}i*naaPUlH!w8$=CJPKK",
    "h:=%}:qXCG&ve@:s.xL8SdMAXew7:w@8u#^8N4TMyoHaueLj:hEN$-%(eBiRo%sNV=[^8:Xt#h:c!#0<zjrw}@fV}o.9&yV/]U$(",
    "7!es8DaK[S)^lsO!HVV)HIPY0PvYByUwxxn}C3VCBcUd^i}6%H^mwjuEc13:KK$5SAcR&8Z$7zci&B!$cAmXx&gRg9aZC<FFB!=n",
    "lnzYThKducvZr?-2h.hIW8?=2k!lRJ+Z(3x8/9IT@0^OU0]s3[q)ENYG9/yUF+P[)DMV4}^F8RlC?T1Gv.DwqJm?4wV+NIzj=mAC",
    "u*0[wwfYmg>^$B&puJt9>#OUmQ{cpJ0s=SORmD4TQ2pNpRkD8.sVWfTkxOc:Qp%TwB>22b!([>k&T&1JdM@wE/*6ajxiP*GeT!De",
    "ctL(WFd)mww0xS#?i&*TpC3cw/:@P@dp{1MuAwK?+d6R#!A?ux(92!8Grlccqu=N63Xgu6Tc$!cMgb0(5>%mT2WruM-I=u)gyTBI",
    "FxsAC{}?x(S25b>+g02#aKoF&uF54qjmGh.H>kaW7:f)k0*doj+XJ9Hrf]IS!39z$49tChu)^r=4RsAU&$cZOk)F8/:z1z.M?14/",
    "J>O4F+>2$>n)B<1Brb8fUph?59&h?18U2q>z02!4sg&9a<}*xGqh3pf.#L]-:NqeMQobxGC-2-&pbdyomgR.#mh*hgz^w[$Z[4!{",
    "vTOD{ZowCpm%45hb:nG}sgMhYL1vF]KM)VDnb1BC}{Q>A]GgqXyo8x?QNcRE-i8=$kQjs.B8}/8)-l>w:<LNJ!8Q%4JkEOr3lMy-",
    "Hug>r&byhfUd+yWN1$-a*EU5Ls(CVgg%lN%f&+l!e<CxgZvW&bvqR-5Cmz.VRPKMQxdB.gln!6)]+UOg[}!Taeq!5MAh<)vBJ3U.",
    "($U/HM&xDq(.qzjjO2z.-3BXk^XMGHkX(3ptFXPG&q<#:tFlom:C)z/qv/eb8]yt[",
);

                const ENCODED_SMALL_FC64_M7_U2: &str = concat!(
    "s:s{HB$8&x/nna>h08cUFcxWJs})5Y:=8s[x8y5<spEX.l6qHpOH)I#Xv@kj9sNcQuCJv1.V+(WeycF)OCqRq8N&6%c8&)*rl8^p",
    "uTsIX9aPI(?v8Lg053P0KdS5TN(eB6&iOVW:WShu)zaN({l/5Dqr8u7-hxE}=SUfHF68Dr=L#q=y9)u536[pGfp:BN1b<d>uu?%L",
    "T.UBdjTKsB]ZJ>-b*W)LFq<H<suKme*-9/pu2z5OR.2c!S2h0=<zkvv4-wrfLt}ruO?$/cXgA@VIv2n9XqHp0Hn6C^5s$afrabU^",
    "sJefv<zC(0Va=jhwEBO-oQFX<VU^p!xh+V</iB@YQxR#bO$I6a6ny/@PMe5^AO:XVp9L4$5Q3r28KInMfRqc7B(]Ej*J1A1sMhJT",
    "Ilf3bj}4]:?<Bv9RWFzB^0VLtcyH4f6Y*=bc@{nWv7:YD/HE3%d)#/&ujCyDh@Sr1E2fspHg={/L07(<A.A=5^sW9mcN4eaOL#J3",
    "Iz.s)Qh26sM}-h@TQ{EP(ya+puqsA.t<VC)G}d!FqI?M?]>?dI1]4vDF&bbz)]Q.%TV4H?KXz@wNuZ@sU:N490&m0MV>sg*uw#!n",
    "KzR?D()Uj-EcWJu7WVJjpr^-wc&E!]D}d>::JbHr51.Su8GkjgG35ZeWjyr3y2C{M6gn@^zCx7M2kTN$lCVbPOw+Q{B]RGOY!]:K",
    "3/fM2K{.%!iH({e05C4%D>(clwakvs0hTG{/0[gjh6aLpO?Z)uoTulk[je$Y51(g#n@oGfSP5*#]ei*pC8-lfsW(f1uynt/Ifbo3",
    "mRA/aHPo8<+l}%@kS!f)U^us4Ps/Xr81QS?]92jWpFP1{w?fgWhOfO=d63Z/BTAM&I%uG?s<d+%UM0GzR-c9qRec/d.^:uyv)*}[",
    "}bWZj9Ao%tGUDrRC!M}^oWcFGSmC?0Fb[T[4r?21V85Zf>X%V*1dyKIhxExk7zDFPcWp]&-aPB<.mvH$.lz[gyDGT2Qm)WZm!s*p",
    "rVr}XObs*3.]p+3(5M]I!}zac3>1%S6*s0e**%5u+tpF*o85GJ?[+U)z6v:%DlK+1E)gt7kLtEF*s-gkTe#gY)>.7es:KQw21nnI",
    "xG(^p8la@B8%S8twVD82CLNAn61F7THNf2A7ncS1vZDb0pTFvsGt0[>?Vv1BhS(f@6SVjq{qLW$t1MT}CJnY:(NUDrMKM5v(ZHX-",
    "T=J$9lF@[EPRy2J-l)IO]s1DT^c*IsIHt0d4HQ/J0bE?hu)3NOaF}N&hIC6[Vnl.??AJH8iR8P4SivR:jV!27}z7mFZl0>dZ^H7z",
    "wx<hEnDes@D@GHD7({Eotyyz4BwO?$@Rb9X>4HkE)c[ZLY?{@aLtZOS4b+s5ZF>oG2*f}BRssP2x@Q%x72L[3(q2!>}n.Wg&nKTD",
    "0J>3qOjnHJJR^AMzT0>dIO5C(jzADYhF-Sj3@(G8yVQq:Nh2>s&Z]izUqLl3nvX#tKn{St5Yn#VObd/K+vQK-e1O62Tk{PI1mS=.",
    "U0SPf!FdhWU0a:bhY{?!jUuclFP4%fLh?D-b0h<s:SiPeTV}&VRUFxGr/hU}Fpf*GZ+tn>i7A<elm%PDvmw+XB#PT/dZa9g@[kQV",
    "yc3#h(}RxfXQ7zw/!5ZSe9j&mNe<(.ypfu<xRKv@I^}/hVQtJ>2IPLFfOSfhG</G{29&ES3l96W6Gy>hEI[oWj<Ro-!U%1<h6d*>",
    "k+#:&N+^A}6!^cV8zl@YD-gX(iq!Gz0^wcq.^xDcEOcL4JjRo=Wit<&%do=+.y36Y2aK??g/:YxG[7?<{q-T=XR7}^mm>iQr<iD8",
    "*{&DVVReDO00<x6U.1NxU*h1pIx5Fa?$pts<pNOsc01?A&2W6PnKOB#<u!wVLX]ervs=qdaG$5[4H8.)c[Bn&qBZ8/awM96xmHpL",
    "K2!TKW-bU^ev]92l(a(FjW@ae0)cOqWv7)BvEw1@{R34eWx>VKN:b$qA2(OOm4Wgq^*6^7A<jujJ{zwiFyxy?i2SBzUp70%CTE}j",
    "bA-c(1&V5a3<1ygVRd7biw7X3{X!a7si-/?)dhylMtvB5IR$G$GxS#YA8(i!}E6Z{.*TeHg9nzqBXs%iK3BBzf2Bnz1Jc-)@sbC.",
    "BD9An&Ou^-ML5pvCBeR08!V0gn}]F(]xuSDGc^3IH1:QgqL:Jgyyl4:]e4az)9T4&D9BUpIsKn4yVG[bj70L4?gaE$yqN:0=nJy1",
    "aD$.D1LoG-QgIiPMo#fDA86U4eCH+U{C$dA?X[)E{aH8yyD^J^-K]P>r!i6u}ry2OtB}?cuzOvFJmiksWQH%P:30A=2n4n8JDlg7",
    "A<*S:5JY2puy}S&fB/:05#ri7M}ObR}:WLDYNJjhx=Qf^YicNB?6.o@<GLI{An%{hl^1=e[pr5L6MRX^ZaVlAN7o6bK8R.AydLqo",
    "Jxk]VC+31(uKD-]9ARdiiQQ*#fNSsLjkeHMDv-m.C=-TjZd4YTT!C>Xo4O{k9n6?}.2c8NRZ!r4j1tFrO<^]/7gN&!*yu6Nt+mg[",
    "kDViy:7i{/0X.$+2/1i*t:T#[XlO)y^7JjredL(E[WGq$T4e:AC1GJ-jDf50vq>?JQ0Sc*v^o*ME#eUrx$cI[2]/<aSQ4${TvB<9",
    "axSwx2&1YcxX#$6>UYjdUt6OXJ{/JJLa4]NPT(aav(-EfDt[tBHF7Mc=CQeOKWC6?+TE/fPnOv+ynQyiG&hbn]&j-20o#8EL&gvX",
    "gW0r4N?Dgy-ksKFiRN?.CGQvTryuw>8z@c<=Q6+dVS^(3ALT}880ZX9Yw2Fi+leP{wkKmc2MkSakXI7I0y9A)zXK+ppd{Kd?uTMO",
    "?x{yUc#?+&n0v2*W/&o=7jL:oah#KsjO}MgxT<krFpS[BS=(]Qe[:*&]Z1PL9SfS=Ba9k=KLP}2SKhaT?JG-U3^Bdl!&PfXoCm=D",
    "{T(VjtL?Bkpf)YV!@fk+Pd8@/m{?s<mY-<Tbgq-t@Ez*H&+e/=Mx/w2Jf:Vgtey7$Hrp*P7vY:m>fsA1I?OJ3z*xn:82suFYMxM8",
    "Yah?y5JW.uh[KV8og4z%}v132Rr%Kh}ok:1d?]9GXOS-Uc!V:b+xt6>>(rxI-4J2)rM#:LK7^xk^)r9cwnF58?A%T]l3KeenF8}a",
    "{=GrQ<OJ/cRvObr:7CxmFalWPmFfo^(WGTjqQ$$llk-//J@)Uv)c5D56Ess70oxq66JVwt5^n3zkVh>>)cwHL&:[T)Q5C283vNg.",
    "Niv^c&8vt5u.hnJ[t=&duE3Q8N&vy3p{?)*nh#X[<YGNpa1Rg<at}$qTure?>t)2ANek5<vqQ?loBMcUV8%MWg&bQ]zTR2Eg^ZDu",
    "5?aa1cfSN^cre0$H*x./PNj*Azuf:KXHBw/zSsFP9+L2S3=XBANLv7v6?Z:7FEHOJQ&SfwAk[[uoEN$Bx(&yGPi{wpZ46@xqw^##",
    "pytocH-0ncdei2rL6XgYY8]TUDcx+Y5eiH8l82qYxvcm-[>$<*[}Syv>-hTPz8s*Ewy4!H9VgxWXt%}?&3C/]uE^/z4O%yz>u}gJ",
    "7+OfsJrtMYoWmdYlr]TmpHkiTjU++Qd}8@3^qfHXC2Pnu.tO4gCX}<&fYHA>*)ceEBr(A2ik*OnJlNA%uuzBB^W(:/TKq+:cy0yJ",
    "Dmm:+f[tDJd&XmCxQ*])(WU?Ao@d?NfJYsdC+$0]L0Fm{hng4[/RCYl?ckW#j8bc^7ZJBvT-xJpn>)I)RkVw=87SY&6MsyyRYtW#",
    "PEa]%MM[aDeBE5ZTL*0tm.>lK6s.tYvM?v!u55!KC6BAVy0r##(.x4{%9yALZJMzo6rY2iTxQJz{}mn6D)L1uE{/6nYOdcm3&[yj",
    "08sp)0k#GjA4Hj^5^y9Pi?QKToV)w#}n4u<3<$/O8}zKjQAcl}&i#9q/ywV0V(HTe[*cvXrs1Bz&g1yy<S4O<4$7W@S1}t7F@$e*",
    "Rw/9t0gZQkFiEg26GF1-Qqm>-=M0PX30naV{1^{ac>5?-1Znh]6I:p]&e^W}z=MLA:pLR&Qs@{ovP}RKIO[k./1ST%DXyZ#8L(1F",
    "JxSp:oa.=H[0C@B6)#mVZvNpDI^{3QqpcG0.+h^F3V8=Pd[b/r3dPCFAt%UCO}&H)(4%L{ymqy&}pIGa>LS&ymUk[GbKlEm{Ken?",
    "6lA+c?UGHJWM%n]4cqVC0kS0PkYwRy@)7zOA^-s0qr%#vWi%2e/4+O=mCiQj*Kv46ULGh4bi*ahOlrr::CH6.=M%C)3v%=))UO6V",
    "Fj/@b.:IUWN4$P-s6EHA2Gv@KT(1E*1UPWnsyGDqnA/0zMG=aD}FuB2y?!X/FzZ.%x.-Dd4).QbR#P*X?eL5gY6LhoSPf<sXfkVK",
    "<8*tChJDoI})f3YuhKsSy}rq4HWfWUmVQ!n7J:dJCG%bH{8j(kqpgK<m#pRpU=aY*=?O)2cKPo7@8]4zgNK}2p=KB#?[QT6{r/OE",
    "V@pT4iVD*pK$+fBHLS1za#!z>iR#Po/X-C!e}iDa0bOjq9Ww=&>0mXm8C-GgkW$OAzpPrlhBrx/HxIwbNPd$NU:pFnycW8d:{aO6",
    "bGG#0Dlx4E}{Qbew$O6C:pWC}:nNK2bSb}xu>3CQPDN&eT)1I5PhFa>Z+8fvNv7U1@vQY7:dS=5WN:*&CYx9l<Aq}m<j3sS)^Nnu",
    "=ATpWm#iIBhLhfR@*ZnS0Pu?/H#$G@!T$a1Z[a7V8s!t5r2-Fky/f4LgWG&TwsaaB2-]xSFIg@O[62-l>T0rL&S%Y&)0f2L5=R$}",
    "%f]r$BN4mpS{zVdKQjN(ngc?/3I87$MZBCVS0{bF5/SkZd}Pp9xC4ePi9O{pm:gY(kNSEZ.M{<rMHr0Xddc){*bps)!?@q#CIiUz",
    "m4Mw4(tjtIZw0)PbEP2l.Ed(bRRc<u4TwC{Fos&8UiuM.H#$UH1l}Q85-+c1xsTks5QMdz:Ays(t+)j&hC&6&PgYUSQ^BxgG^dH0",
    "ewV>HNdVYS}{b%um.D-7t&hnsCvA[L}+wYQ5nQq?Xnv%6St6[BLcp}1]s4{/hqRv%Ek8+R^qWsx9Vllqypn1yGnv=nQfGK?ln>z#",
    "/s@Dx-0TBAUUr.kTiZ&f!!bQdH#woCTm)t(T(/<H5/Zj2L@3Gyr(2M=gHQbyR]CI}7F0ZMr(1R@(j:dWy(}l3U(yoc?1Y}[S)5WI",
    "s0RfpIl4l4Lt^1CIKP%CmepKX]a9Jsmx(?RE/!^@m^OL^{^NChspH9J!d&0FH{E(gC3w9kqcG.Tot}=jks{h=N<6V6&809h9}^@z",
    "@<Y5xvs3(}p:HVRb+FkNPlmj%0WJ^?UtGj3/YoJPvc8muMjkAfh(5S2JDxUlNC1b{V@Lt:iLs){ZfD-jwh/W=TNR0dE0q2xRs(5@",
    "Hom)EKA(I[Quu}/R*YF%]QAVXE#TB<u1]/(Kgsjkk*?+Vj.f>)JR:8E{pIoS/o{r^-Z?dms}:y&E$#v-LM.dM(<k$>ojm>n3Gz&l",
    "*IRRS:z$E#An]xW+T^HLKdxH?<DP5BAw6X[ZS]IpQLf]aCWU6}EO#v[6!f0[Xn]{eC?6@1+K1%^4vF6EkQO=Wo.-l:.Rl0yt+N>8",
    "vg:nsDgD%dJB[5gv8W@o&uJ+lSZb7E&0kT.Z#X/:g!ji4Tnt#jm*.h?)?+c1nfN/xI[7r2<m[?K/K!c@gWoRb=H9CKKzXTywqm-W",
    "ImG3rZyW[rbWCWV(WFZ{c^&bWlJ7uaA&}^z*</!@}vPmiT%F/^SfI3W:pVrMTH%oL/ZUitl=b/=-]%3UqwUfNkv-teRg7Y[GQ+3m",
    "7p0V#T+Sf9Ox2RjF/#^h9KCZU}spz@Lt{Qa5jIh.kccz4JoYDq0d.b.N*C&M+2WSnfB9g[J).(ga]BO?VGcRJHe{pCkl578&(Ud.",
    "rHEWr*6!VVKB&$n.tKQ57tMZr1/zWw/MrVFOkb2b]47um*VP2e.-SH::Gbk5dpj*zC*1n)=HBuqdf})@dR^wBhB8)Sw.!cnL}SDR",
    "x+@0U20?s+Bdun{)J!X]=z^wFzE+T?-%f{OlUXLz1Jn6*7]z#2lg=#%xfRl!YeFe4!?-8}TH(LpdThj.9%]l^y?[}(UQN^Vb2!3B",
    "K!>rbG%R/j0pdwj[X!IyCT&-q9BQ.q0G5bmTd(ScMd1ONn$q0RW2-u(.T/p(P}tEJ[h!]V]6*lOzoLtX&O6)p4]mz.y)uFl5:d(Y",
    "sGy&5ZlKbX*e!A>&>bsR::Z-WnfObHsvWc?q$:qbX3}kSk8cWjUV$x}sRgqt0xbP{K-+RbswHzgy9H5HZnsB+f8ye-nQdH7sdLjh",
    "8qZ)$M*{CbDnjzuI0^moN<T*(^}Gf7?qUYMM^C<#^s0&nT8kCY]]3q)Dx<pH)YkAY1Q{qMvlFV}CFahz)k20Zc6=](7VnT-LD8sf",
    "9n!m&w^dbbBx#)H{Z&YOL%N(5{4.v@[iM=Yx=?xf1aU3#nTsV^QN[bxF@t{U]9[^oGDsO^6b$HDtx97}-H%+9M-QXhlv1Z9IM@yf",
    "s5R(d]$St@kq1VOuzY6-y*CmFjW:}p>WG=s^%RRqV$)d:AN)*$HVw!aWs<TPdJ}%ewj)JG=)wR5z1Id161:@6yM=B@NyQv2>t3Yz",
    "n:-0f)bL3ddL1aP-)/u0!f44uw:$X)WTgOdBGCWfNW/T9/Jr+&xvS$Bag9:^WgM1]?+}(^ilIE?y@&qj{y$h437&@=&2i(N6x#nP",
    "-mKJyO=w&ajB6]u3]RoMxz!sl)*UBzRkv-^fG{h+Er/f-lG&RPHO)E4LIOn.?4aJ&&=HqD4OIHt/%L]=!.g3/@Q+z1w^>E#X/%p3",
    "9rOC*1%f}XC/E9N@M[&-TbD^qfSGgT}x#k8nD<o3@K*xq}zo!p3Bc}Jr9aGirC+z[7$5]dZv&XYT:9*UGdhw:n!VqfSS?@R/fq5P",
    "@^*$gTq4G{ts+T8k2y%^4o]*A&80@EJthWYkTMNa-6sh-R:>bU1>>$2L%#MNUX3)0A+3%&6]j=Xhp2F<pj=V.[*71]PYv07OGRf2",
    "z^Pz9V*Z5<>FJN#owaT9}l%-}iyzMl1.id%vPpVp@GR6gsEH^7<<#{:[af.@NX?z}*Bi+1UoexX/v8/B)%{rWz[WU:)$(#!53MZh",
    "iZ(4){()CCm89X8oWS@95X@DKganbCBNIo7UowuU>JLG1nPQp.HFM:tg[<w3lny79q6ZcBI*}3RS}yc&l9!)4C9=QJ0i{gJAa&v/",
    "R(NM}(q/k}UC+.<aV0Ju.0iddRtFfAIFTYNxiS-}Fwcd&m{I9sL?.qjs+yPQ=3MF8e+YjTr.+@{.78SK6$gDYWDfec3.ca*V:TLX",
    "{7Mw#y^K)Xh@Tr<4UgPyKCnSP!EDKZLTF>x*K0IC[fkXip)->0LS9D[JWWPmFxzK?v?S9f=cr%=0c{N!Xnf5Ea)K!C)6)$EKjLpl",
    "s053*4AC=/zAoyl)7FxxHn25uu%QT.APx%f>k$(KIuDNoE1Q?1@2utk52!r}LQ.kEbgqH:5#fDW@p-/D45yOIB[J$>in<A<I0y<2",
    "TP<XBexH&7g!X!3:@(zL5hkfshfhH9>MtLjZ.OgV7#je+ne<4(}M7P0m0F2Y/27rFTA}Z@g*OhoxNUxz5k!m%ibZ4QA8L=g[<M%2",
    "Q=/+.FzzfS5oCuA0j#J2ROYgyAfjl=szNH>V9c71xDm&4}*z?=TXGtp8CTqCSg]zoec40gH:sca}MK:0p=l.n6!i-TkvSRn?lDgS",
    "g.*Y/s+wd=]R42MaD#vV0+Ud&-8TibxU+iyPw(VWWaBrIFoS4$!dNN2O=*NT0ze=]N2Zh&o&AjHX&UkL==rKUQNiL{h9q19={OKg",
    "DjNc>ih7(oru^![@v)?a5y!k60$=}BfplSt4K.[5y/@0%S#Q]F@3IVZ%12=^!u!Nb+:AJot20wSp$C@^y/&OUpNLw1BmJRJgZP.^",
    "dkfq=V-LZ>MFiSEaaGfXK9jz.s{23]aA@M7>/EIJ%3y9S+8jFczOx<HgrKF#JE!P5F!U5{@b&ug1adQx}#%gn1>Nw1pIoZvS6a!c",
    "1)@Ibu>%c:yIsRG1=gvnt3WKWxJ{P.SpMlq?HIdXO5PrV}?RC?Z){/Rsfy%OBk@uCm{IVIVUZk!<ucHcJ1je%:QT}a=icRJBqIH4",
    "AYf^Rc{z64Gey(^wldl]Y/D$zR40{GXCM@L{q)=T[e)j]pmhv2H8uTF4STf=x$s14<y5UmW/vF-1uMYHIzMP>hUcHM[j@adfdc50",
    "=#5ejMuz/jBR9d/ATKRo08K7!CxBGYU?8jQ+Rl*}<M+QnT$QQ8sp^v6(LW<UFo[Q!!(+s5NOb1(b{jJ#NQl<]cz=O-0yiXH]U}PU",
    "LULNG&<+ss=-O$oSgwaBMty6B=7BvoxHsyovc@DS(3?i%1@G]}al/]wWp9o6Nat[k2J+>CQwM:Ww7Ub4iqdRY@<di&82>7fXt!2(",
    "5sqg+jxi#E8ZZLcd#-&d)pp<ZEq/e0]dv>]EnbwUz4s#SJB@.:BgVcHgVfQ]:p4n%NGiG7Ci2daBuzp(wmWqPgd&MJTppHU+aRD(",
    "BKfFum9R!@2bL.wS0#zkM<Np=E9eL7t!aQ-0oSyQsp196!j6CL^Q%mTva[?#eo-p4)6o8A=uKVH0S:TZ0eLN8v7<in3)8%)XsdVR",
    "^7C8@FqM7{=><Ki5Bba=Scm:B05$.c<JKZswx!ZIlk*VO(D{UparOwh^c?/nSL#S=-5{BK-4sG[=xPM^F05^74?plRr]rZoD%k)I",
    "PY]:}uN?8I^+!@R4ac6ekXg}H/L?LrD71[2/ZxcXsvrCe/F/6U^:OAlZ>1..Y5dP[!)8a!WQA1D6OR%*d3b38/NrDRTBCi6M@}ww",
    "0go7+LW&EY]P+JKrUnI0ZBXMYm$]s6:0&1xcc4P3G64u)mmfTgSa]{c8bkH).yu]%&qFW0C!X{BXkgkWbQg=4&2CF9AfetS^>I7<",
    "dlhdSjrD^KUR?rL&KisigW/L47&K#tesWi:I(L:GYO9pF=-vbXXih(olEd9{S1-M3SdbX#ux@jqTWLW)6NQDQ0?pEFgjo!H0)vKQ",
    "a^U$*Zw5.ZKU%$*ZY/Ib[tN=#XKLZxEcivJWWo>8Q!qdXLOQKq1H}C}HfVb&IL9RRO]>NTZ=(u[/XZ0#[faR<^8H^MKzh{3nfxly",
    "aKEvaFmbAtD(Jg33A%[4/Nu=j++FjMxU3&1Sn23Hxn<X&<@(/IK1TOGcPzk>RNE^lmhBfgkRA2GBgkw/Y:t-tHWj/I{f.asUR232",
    "q1G<.H!p90rXdB*r{BCl2&N}X/KE6d8!qZTXb}G0/JpH81fT2:R=/>fd$?R2Qzv=iIkMoAQxh2>aGCBNy[h(^fP1QG&8$=FWVY)?",
    "o32jtH%IFLu7B{dU[@P/{r9@Wh]1-yCV?k-PCslnS=+..9k{stJ]:@*j3w#{DB}.u1v6laRzQ9xQ+1djXz<ON[v.ua.<?1Dinx-Z",
    "Wjhc&+)C8#Tpa}?SYIMw:gu#r<vNCap&JzF?#Ka%k-L!b[hdQp%iW]h-w[yxerfr=",
);

                const ENCODED_SMALL_FC64_M7_U3: &str = concat!(
    "+K3Vv@^>Ry%cu^1H7E$gQ/XOH9uKc3@1tTlsC2M@zP&hD]0X$tNS2n<ZI7^4pNAQ}g::+ojW4)&^ei%UP%-VsO*iF.FAez%:]Swy",
    "ScmU(l^crfxeSdvXv*!lKB&#=9tjk)WH%MzL3ekS/^VRTaVa^lD7>)lE7EkdbO^1o(tPIh^E+mq7MQvneUS!uOSDl6Q4y04*hB2P",
    "@><yLo$ieCEFeJdXCDv<}K6(r6=8&[Bsd5Lr$V=xm]PNOouI&Hx=FS^to<XxFLfuloxOh}IS$1302hv{[vj[0Xg5=rPB0IsW4Di/",
    "^hhKuRbqF3DgWum2[!BDLfBa)>lA?c!ggzWnJAh4/2396l]$Jnz3p$AR10&^UDRxE+E7[AZ[JLCMk(>!4!qqq5DjvEsO]0VX$)9O",
    "@ldNxBAqgW@O+QR1{Ub(s%UBT/J=uT{zBG9/Cvb)[p<ACTcO#lL8f<+J![{[@q^6=rw:zgi.=eehVxeUdOR50&X==W:>h0O@%t>)",
    "tHpKaD=0[r*Cb]{oESGABGER*aLR@us&Xm&?()IMd{^eb)L!ihf(>=9&XhN&DIJ*&hOga{b*wUt)P={c4rG.aif&!)6.hX@VztpH",
    "n)}rP+>zc30c&X1Y[JV7e9)}-M-&][f*CO1&t!8%2-<K10*8APKAo<51#UbF*]FyS*t8IqB$/e]JqGCmC(ouW+.d%?i=f}>m--DW",
    "esXAc-3@U*5zT+cfDZ(*BLz??XjFRx22:x)t>X&zKt$(PA4/+Om5DcX-?rb<C*->fatErr&0E3d+9N#G+b?6}+X&n25fS%B5p].L",
    "H<6rw^OD{S1D:UpBzfvvJ-*$E}1pNFXBb}K?FgrG&jmS8LX?E8>Oio]8S)^c6HStxZ+6@DfI/l)U.J/M>&[XL007h8PVCtsu[=c7",
    "kVwVnwAm*B*-Tbu-V:3GKpLwPL&vIjkYxN.)!(1R0aW6f6L[z}jDEX0pv0Czijr+ss]{6l=PJI^hoaK^oJsf^wz40%>R]L9mLy4u",
    "6[.Acs9r39ysKclaZy+PmcoW}-Fe>8sO*Vk3<V5XqcIhPty)3&!WDg7D*]SHx!)3bx7.g%t:XtH&n?d8p-d}IrUD{@?DLK]y^7YV",
    "]UZTu5&U+jrm{rCPXNvDFls.xuMYCleguGu]}{9H3cKc%H*u05089tOiQNIxR>(snK7=$OW.D:]ROwIi*-FQw(9Lm?a>^8-1$S-!",
    ")zxC#Je:k*/N$bbl@Ht8/#ITRAHY>tn@IN&&j]NW&c7hzA5e@Q.PH7=(k8K}7*4u-YpGx[9iCTx+5AuOmf0@-HimM2ffwtWda<Rz",
    "6cdJAm7$Z}nqbwN:H#i0s45fm18ujDM#gG/ZS6emJxnZN.#iH%i:!R[y%i0{2BnTU1zGZ9UJi.!iQn4RtOa$ttVGloe6WxyZ]fyV",
    "@}FdVLYE9]Am&/a<CS<$4EmK/O[z$8Uu2.T3jvUlo+GuF3SX-vd[!zQg/C*o28jx^pldm@cC?-97dG35jQcZ864STqYDX%><RWtE",
    "7}8T)/iYW-j!--R3@2knRNNHcfVr@NS6h1YQ3+AkhyzM?bC3qECQi0}:wGHY7/(m4[xNpYo4h>m(]W:3n(-X<b0v/%txBI>+Saxw",
    "jbvJkxE]PlQd62-/:!9D[7c)tJ}fo+0:W]Jred&{fmqnjCCr6&Vt&e(w:-Zib5D^hG+O^BJ!/3}p9De>PO/Ba?9D.c>4Cz(}KLEn",
    "baTcB1LZn@6unM=9hZiU}Ce?f&}nzEO-p6&]?*DaUplfUA#CxH2t0c6?Im^/x&z/ghqR&Sx:<@6A#(O<}BOVtzPtNqYmkQG3eL6-",
    "jaVJSpV8ssJJtmRv&pp?FV[sKk}3]?tA0.*PKi)Dmgf7czreHBT7lolT{AYAVpzHt}RtL[>*L5AH/#QFrV.jMgf?QTJlALgJbzMS",
    "WU8EXfO(pk8/pS#PbomD9TYM)A4@d>.p8QJsq#?73b?iOTif3R!!mshkNTH5.iY.=IXt{15tipv1xb%%lv688w$urxy[SY0&[&3B",
    "p)-@Hq&c4m4Y-^C2Vlb+EJOCHO}0GBfi@iL0d:EH*K%5#iDNW5E=7q(cE4re1wyvgg=zr@jL%FU)ULHr7pY#1Yi4x-KoRe^>g{tb",
    "V}2a0^ood9v!ch9+fJ#18%WF*@Ydv1SN$}03r>rVN!!etZW[nm0g2McwId5$1IqyY@W0Yr>Q[y1zhfx2LK1WIeC-??&kR1{PLU*+",
    "xJizxjE#X?k:]FKVrXV*<rPt>>kx=[[MqN2C5-.X5:/oPjqduIlo)uubCnBc2xVF0!CDX!}O4%5{Z4V#SwmjjrrCU3-lhvEA[cyq",
    "<%y<cj)S.y=){Idpoz=7?!Kh}3tIBYp)[QLu+4HDv[WYLe=nIWgtP8}01D3x3j+E%5Vzx^fM!v00jU$uY:x-Z.3ZVl2xR*!P+JLI",
    "lwrGyI.>t}oNb+:Jkwu6nFdgIV.KlTQ>qSfmIUqNjs.%D.D^Kk-}{/u2OsnNt)0?+dvubUn3!7o2Y}::Q{#5<md(u#iRqn[.rhVH",
    "H$&Oaa3i{{q22sHoZl)^bjaRU4Q)I[0p2#-g+%r*ntR-^3]&=3Yp-T$omoe>f5X6sPJ7f/673:MzQ>Exd{8%qC9hU-PwwhkKxtV0",
    "3XoP/6lG!T8:/N%ZVPBC08o:Y/i&Qhk?YWf0cyYy8>%w}.:e=mGQ.hqeC.j^r$B).SI?sfhl=IX[s]@@>Jb2(^>M0UojXvwf{wU3",
    "/<yX<(Ar6dY}Cel4Wz<ZSuWr:!Pnpm5glr}dl[GVg$-E49ARz!f%x/Nn@Bj[]0>Wil(D&OZBT*%0%5}C.%%G:CIQuNz)iV.vfqJM",
    "Z#FRiGf4h*V<W[/BNkm977{v9X&3$eHZ[$#PVAAYy#e!/7$w7sXs*/7N(-vp7RfTWT%rhB*JS0F1M0=Q>.)REs%j[E}q4NOPfX^R",
    "0A3Y%8LDmQck+7vN3IT6lh0&kUnyPUA(gmdu>D}Z[:N5L.am=[6+1CjCP+6<H6KhhFB:z!f90)[4#37TY:3X-t]LsTrBtnUi2w[:",
    "(4T%QnO=eQnuocXbEKZr:R&^A/%B<Be!BI=VW0PQ(%1V@cjDLg&<=Y+u-/}-G1$>Vt!^j1i^cH}&dfruzo+X=Nt6Y+1#M@Pvvhv7",
    "l9C]7=@g3fR:Ou:-c*(lgUHc*5d1jT7tqeuCJZ>EVm-EDRuQ}KeBciEEN:4A-&2MA^c^QuV[r*>T@X]M:JFM^7Tn2P5pfc}<>x(4",
    "]o+m}li&Djp7)aX(zX.=2kFP*Bu5$SS+Yph!An2sb[LeYV38f<.>Yi7<HSyB9YC65-Y[r6Ot?Z=4K[}K):m%@)?MD#G&%T6&ep#9",
    "Jp^ckqks*!([3<N/D)Z:S?V-d(}!]&Q4=1dJFm)aON$B9w@zvaR9zb}bEQ#yTPP[k7?JC8<Ys):%b]dK-r?L/>*F/XggAzW?MM(v",
    "El!=nTy&D+DD)J{eL<RO68HuCeHhG60eeOfR@a^yi}@qgVXM:/4n$rY)4lCP{wMf$4K{E3V$X8OspTc1PNO{:]RW<no3)vDCP2B5",
    "kQqG9I&[Pmmlfu<pLx-Lx^s$q5#fxU5JFr5x5%xjxy?I78qSMJ]d>T9^Ht3Dc=+>(fD.WPlK7^#egjocqK(mH8qLLhFCZ1)=[Wmp",
    "9G?Ne-}q$=ipYzGvx)ThFjm(0^W}amLSTEU<Oh$y1*tE05bnEPzXKcOlP6[p:6BG:-]8qer/O)#A$Tl>>@1db+z#G-T&6@$/U5y>",
    "ps*vEy9ezED1/YhkzI]@Bs9UzV/e$*0Q<$<0QB})WE1>@TwTpb0u2#wrg8-f5ad?eRk?A@Ut/ZfDAW&x.fK-%fF%w8T#-WF0an./",
    "GMPdn[%D+%&{2?[o6IzI6y2RjKuG+XoERXXK]Ik(>0GA*REAZ<prAvCsC)UL)JEPN2h*H+0tZ%l-1G@2cSWv.Ex$K*A5MPpUlB41",
    "GDcI}bkL+iyis$*aH[X*208[3y/X3Sqe-9<BOAX^+pDb@0=->aC6XD$6Kfn7}GO+#Hn?yusEYGc@6.8U@tz-I9Hrba8H6#H(rKC6",
    "@g}0rFVy-x5g:nl/BzmK^K0[^1cxrw&xvc4@U?SD5muzshM:*eq#1hv^P%)RvA#XL896B:9edz9&Q00@?2Mv@w/[GyCwi[!a-u5F",
    "=kkuJXJ{ke!}}IZh2Tah0>$8o@}UUs0olWk-L[<<GDo0:K&My[!s3&WNX#E+eJ89xngH&b%5x$2U/cZStdgy[22jzDiM5o*+P!BE",
    "zNre4xh6mclCgM9FYj=P+N}GJ{3z6eHdmg^oXv1>CQ?iaFYLX?>5AZJ1mXE=Ir/WP&&+s6i=/)m[qDY$9vEBuYI(RVVe-43G#KoW",
    "L^y1}JYbg@)vqhfJ6r/C5a%M#A2!eRpzi]Bo!YhN2wbA#q*^%}iTV57NpifT*qpLp.8P?8Q(Nn[F5#9i9Y{Hq-25=]kC>.Grv$F!",
    "CCXAOOX>(G0xuNZ=QU2#z)?piH/yK<8SHUc^cNvqelU&.zLCAtfDY]1l##IB=EppR+[o==Wd.:d]FR#HyK-cy@X2h6o*g{b8B0J3",
    "4R^#ooE>4{Ga/#M5kk:f?q2z3T{)+g}o?@1(9BeCq2=Os^TwO/aVahL-m<Sxg=3vteF=TB3qWXn6{$VcViWqu134F!Tr%n}:w]Db",
    "5.Xo&Pe.lZ(WHrCX>UWTjE&9d[ZrpTW2gcZJU=-G/V4kXANryL2$(m3j1Dr669u=F%j>BN!O<AW7[t#!HGg#GpiZ>y-L*1:6a1MI",
    "lD<<vZd?$FnDye4]{4c[hsuhZa4e4Jn52OF?PZNlj:c$v)D-lm{+wQc6EWh00q4zC)+#Ka^85}#fVV:F?qhN7HQPucv/AU}SG!$#",
    "W9>OWNI&uwqyk8J2crG>)yjmKi+oqAvJIHhc#x[^w)P7kq6FB(O{46Az!EoS7{:MkhH>()X=Fl.b?dQ.([D8:eg>G$1sbOiys!}r",
    ")nOUHQe*)01x^xuvc&RhJkQccuzz}.o1Q-W9O?2=14hikzKhOg)(GNzmZ#?&2rq-D?a-hT*RwMRDRdIQ>i$h>[&[M^C:xR/ro+)B",
    "0XYzzuM6wqLBocFH!oOt3]fd[A9z.zd.7Q8O)hB.GtH*9fpq&8f[=)COsq@g>WOJ]@m9%AnnUK[2/Rwc5x%VbXtUJLu}g2QMYGJT",
    "0sMUaQCs^y83UVa(d*.L[3AtkipohGiV(?:Z?1j]KM]7{T.OY=uK}$aeqF!*m[w/Qf$H^Sx0L1wL(C77.+jyJ)!5uHBTUf>HiluS",
    "(Rb!>2U&NT@:32E-)^EwNIl@ftW0aJ<%*+Mi.PYv-.#ir8Y1M1V-.wjS$VX<Vf@8:qa=UUZ6FylL?x?oa(tWKSAy31G0XurBE+/:",
    "jCW:^iAPVM}T3^pM}RY!iyVR9Nhst=[{m8{2NouNSZnZffHD*ArG{rzMCauLDU92^^{xHD0ar<Ul3bmNFMMr]-Geqc<Ap:w2]7ls",
    "IUF%ul!TMl3{U^@yb2g>ss.Lc0W?J)<yWEo+m^K.?mrN0RCGQ[Z}WE]0Yau0vBJ(x&c)b/=Ns</Js{oCn%0C]=T3Z/W#0^wjE)YX",
    "^HXEN-1a=tO*NY9TnJNy[y.wpwV&tLczaLSW2pUxI<Yz=BITb512:NKt!#H=e@pW+Pd4{%d5s7f&qiC!C%lQtF}hQ&.2]AQ2Yf{.",
    "@g[LJl%)Hri?]Ur-.mmlRzcreu#5U6rg>E1Mg{#=EMJSOpvIgCE[#X0Ya6CcIV#NkPr2cd})eS6fk>7eU>FHs@pLxIaAI}zXNXz#",
    "Xv%=k}d9)Eh*>5b]QGeIE)0($]GH45M-S=(A5=/tEbT(:NJ7fE/euZz)07iFzg^LP0mHo%bW=2lPGlh*N6hgSxZ^cdoCI(/hZXO?",
    "h^=x]]%RMEZ^]==m.SR3}K6l9cKC[#F&%WVfTFEa7-+E$Y<[{[xXl6z->%{NL)I6oZ6P0EtrCx&@Ck4?)bLiiQ}&(>9dCvVcUry]",
    "/Sk/K7use.M)cAi14yIHAde!vV)}q7B]&%NkG]rPKu/.R(^fSn&:so3<oioii+0m%UX%Rc[UDA6*Gj?Fgppe%2=-noFB29QhN#jt",
    "Cuxg9s%p!$+f-2Vyw*uhyC^$k!Bg.%bXoiPHVoI<:UQ(Hove8JZkYwgOmDI2<tzj%e(>Ibo)K#DWLxB0pa-e6s3YV%-m!#<edX>@",
    "LVnO+D>[z1VTqGJll?1AwrbuSG^cQZSZIoFuzy#ziasqIt7IR}xmM:ve3!PRQK&Lh?i0=i7)%bx}uZW@}DBAyZ#AzFNU!nZ)2z(B",
    "kuzCWLMJi]Ps-7Nj$sH:&7UrcFj<N?B*5}I@3Q7eLgrI}NF-q-Mp&n.z.J#Y1^J4J@ecH#&w=w4:h:dL:A3sO/B7ejZmX*%2O$Q$",
    ".EHZ3=3W<Kn[XO@3?4<{qW&iftR}wL[}*O+crPPE^/rMn*(TUi9jH16YIGqaYReQ{doheqQoPcX%kVp9KK-TekeP#RT=tk4XQz&r",
    ":eM*pozR8>{!wZ<i%k#a79wS3D55E:c+yXtL*R.eQUKbd]TbP3]d14VRI-d5kF822k#j@6I/NGlAJGj9dZ62*wBUF&Y!j7:)*c6V",
    "DR0efO#/7TJV&CR1/7PISM5Ejw++#k4^:rY^e1>Gq8%iw2rmP75*(.NTR[f6.q#a&1C[Rx:oU[Z=>9B{]ji5e/]t$]^DTH>pX>kU",
    "2]{d.G45+[y.0nPVP[HN[I+c}Jo-Hr!%bnVWhkV6Rammy*(v33v)wNdNPNzt<bv564!?M?:3vWdBOMOl@{)qb40zrU9jVwux.M%t",
    "alZtI>61v1!trEm*g3ZsQgYfz]&cTSb<f#%vGs6N5E1pX5G%k1*IEXtrZfr)PxbJ9+dtWdT0vy6YjKh&SHtVS}{c=RZ&9q*JKhr>",
    "*n{x1:*UIsSqJKa{o8-P2FSI!J:b=3dqhR[q4Ne19I%&v=)ittTq2hdn=:gIU0g1wKx9U>FE@<nEbG(C[xmfhU?9kBb?qYNf0t<J",
    "Z$n&NM$v:fFkD9PXkVM[p=$Tr=$@u/GE5)$>bJskDug-6*[:y4M(Rm3*ku1@]:8>SI3(Ki6!B:D4BD4Ry$HNc)hdln.]{d+cFx?h",
    "WIDo:>!g0gfVI)i{>-DzeNPyd:R$^P+?naw2nQ&lHn$OyWw>-u/EBo(99Bq@o}Ss!S{%*/BI2&hQV&4F+/{UYr2C8oQ5UAYH#>kx",
    "l%SH2:9[<?R.*$e*9swC![jLpPL5bP11s8Xqn2E-s:5**P0FO:6okz[<dWs5q8Bcy[F97UItWfj.2[X>xTh.yPdbxaX<UyrUNUY^",
    "C4p59/PD/WcWu-DxE6p+/k%%9JH7c?J}Cd>=L2lJwtJhrvlWk{B4w^m[PARHw)c4={/pwc8FU>Sr3!XfVm{)A.&d.Z.RtyeDeUX:",
    "QtW]xqVHY0ieomM}%^/MxwWt=6{jUywOuGQzHlArFtN0wSlnZnC?s)/83D6+Ux8Xq2Iz}/NW1(bfPXxgG$MfN<ce^(C9uk@K}(ta",
    "3@f!^I.ocomR297z>%UI+</<uO{*pBa95dh1ai{eoOd8{N.qIhEj(i%b#D6O&c]Sd1S*x4Afp7hPT=db!2E@j]CYew}0e<ik-ELj",
    ">S@aWy04tSvc[)%Q{{^@EV@f)LLwR3+v4A:4Kiplf&$ws2q:]zuc-)vj3DPS*):u*Y/&a7T>9nhlM}BM-kS6DtRzZkIq=:[>oBg{",
    "8*aMY7G0&tJ4=(#I0+ye/aJ:25UBUo9P}:Z*)0TU7rz7>HxSsz60/8i5:1ZfAGP/OmC5x&ssYX2FYL5V8Em5v4FDU.{nA?Y2W%C<",
    "w23+39FS#B.%O>=fJzq&:YUYeZWh4^9$c</=yra)I]OdEmJ<fzW/)[/{vO5x&O5xpi}!PFSaHQ{vLNX(WPJ1icc$2MiLvKsH>Qw)",
    "L2Nx6CU}nJEa](gL3^Ms>Z(srYd+u>v*Nuyr-C+Ry2Z<gD38$E/<CyQj]#fSlqPOO?unW0XAyxn}T-]l1mA1#VnVIZgL/)i^0T(!",
    "X]:Yg!pnBIc6Tq23oq0EkO-ow[nzz)vKcij-aYv*Qm-@0:wbjs/F40Lvc6?>}HI0$g*G36Xwwth9Y%P&G-3Xuk51E&zG48taH9Pk",
    "H:Y@1E@fLCr(UuRiy+76UW(4)0Px(*=6CN%l.MDML00{4KOKTA!i5z#V#=7b?ckVG/$S9JKu$GY%l@cm>k=>)3hGMzDXdbsFF:$x",
    "PH3:-lt7E9u?.pFc>l*4n=F}c5Rh*NQpfTa[x7t^E*l3sci/@60]-r9f=pG=RG2bu:]ChJFhmIC]$%@%:yHZLCV(EU!TAW83^{&B",
    "e!9ACe:xTOqbwD2>z:C{g-yFI:2J9wz[G@3L3O412RopCHoihzrC1lP]*./>M$bx]a^AhA=<zqqPN@uuJBJR=d$6SCI5^nzZwh6G",
    "hw5hr9wt(8D+H&v76}T/g@a{Y}BI7eVsNaKhW?WVkjnXtYmraf{g(?9bG-ItPulQ=rU<mHgP1i!Hz3z{+)>7AwI67Vo0x!VRG=gR",
    "oz6(2RDeO/5eT1X5Zjot[}^=s=+B8aPJm2+&d&I&B6GA[RzY8J0hF.?6-wObTPY0zehGQQgCQWGDBK3<9^oDPI$@2?ju&6]2.J#c",
    "u{xKZGtXqm+)WGk5#yTK{bVFygqEXt)PL=qU0sQ6vepQ2B9H.Ht%9o]IW-7/Q7a4+^(&S6t/RjPvW0m]LTMYUmd%UB?+Clb{$SOD",
    "p=XJ/p=$1b4^u3?9]*@#gW#9(lJ{h$@2g2F2YN^D/QeW4Z.6EdsgJH*Ixxe!kZ9pyIFhW}@//61k&ZXlwL6euUl0uu/uvEkd]ugc",
    "XGM<Ae6i4I0ITFAc/AE!7n<%xXsCZ{@k9x^PI^RWaAhUvkSMi-PK2V4s@/YsPX{?NWc4g$5>+bDQ84Me:BktDzK?4%0kd0$2N<!G",
    "L<?L:b4%RkJl1NMa:dY^Xh0ef7p]w!>od:S)ht-k]6k2z]Ui&dq]-ld{&}GQOT>D#/:I{:Mh8/to?}ozi9SM00Bn2Qp&3O8m0+:{",
    "ISwhfz=ik(??H:oo.PKva&yJDc+B]xHk}uVe+[/$ZseMnAg*jz!2GGkXI(VnPK9w!)*z$aSuBL>zduqOoLCxUH3.V19Hr}xj7!e#",
    "N3]bhyTZbir)j@m*f3kBR(mnw?45j65?[Bwt6#{PMlmhiV#]+u>6RcZbj/[uH?>[YxPwYF&59Ecr.9-+L>R8?fNvlyMzbwkKJQOp",
    "jyDgxlj1gpy%65%R2hNNp0GrzFYbn^)>L9Yg1.sDZ.*1-eXNDkbPes2Y1mO$e>^rHuaQGUfJWl%PGgGmS-=[%He#75)wP]}n8@2Y",
    "G.LFVrDCc5eoGf2):B/<G5R%cXW<mO3EmblsFFkEC!da[s)JvRibJDA2JVdN5VVTg+7zugxS)AeJb5FU/Hf<B(c.u3d-^03b)dL=",
    "@9p>/1Z0p?50=Q$PY(Oy2R-9?kq1v#]Eo=5ly:61bei.k}t(?]PLbAf=#]KQ^>Un]",
);

                const ENCODED_SMALL_FC64_M7_U4: &str = concat!(
    "b3ZFFZ#IKV4d)AgvNP#O:A]ILd-N&sLT?mS[o!GOfK&o?KAwyM@>T]l[]@3=sv{j?u>1x]2>M#/mGR>*WSo0[+&YQ4kY{l))aiQF",
    "S6aDI.23Qm6IkNyA&CA?U/D8)ssy[@nd+CRTFKBWXjloS3Dn1rGG8SrU&F!D^6%oyb3.dY2^v7qk-!)Rf.wtS!wh{lPM8nu[ASAO",
    "HvJE2[djelYD<WzkM({9Ag$y^L5YV#@epJ!LYZrCxW(:?DwFGju{qN()dNA(J73{-V}6BB^averJqAs.0nj$DkYRxq2*pMbi1.y*",
    "&6C8?2?kXkv13}Cc<4BL2gLj!D)6>2m[>?Nn%ux&aJ+uWy)SpbQGbz#2RUs6<Q}p1VvgQ[)c&rl1<*Boi(dM6^U$9qtzIHSxXY1s",
    "t+1%f]f{wP4JO-+qeIf4H%F<VvliwU0}}^tOntEzNE*%5t7DVrH#>RI:9haeDYS@{/X<APCP(gZmtSfK}r]^%CQciKg7>nlFlXCM",
    "bUmw!hNJ/OiRa(qh[hR@6G5MEvylIacUbMS+Py4L*(axVK8w)IRLCp$Osr8l9Q/YmpN%+:BU:op.m{:G(%LHW}-O/fZ/b&#wtYTB",
    "m6XsaP.M8XS6c9MwRs<hj4nnzGLyizo{^MDANtN^2gp7C3Z=:.qc3-gmR-gvP3&wbxWh@9^d8$aSe&8+nt85c78{HxXmIf[Xda/*",
    "eA>?KtoM)db1=YnZkw/3%a!n1*%@[@=u%K[Ds$dc^*}ZaDfaDR+Swwd&geyZ)<:X/:laj4^u5SpZL<>XI%AC!4H9(^z:+m4vtFhL",
    "&Wnq2*UX$)HUit8i-z9Nte1ucWoj$yBiW[3&.hG#6UnO5PRUC$ZC^yJQ?bJG](3P*xR)=6LUfPwzd=CODR[j=Xy8fRsz4]Ntl:Db",
    "Ua?G2ECId<p?eX3[*0V/u#ku@ZZC&TE+Z<mVJi9JkQkKaQghcRLFFe2IfqRd*U*ExOkEtu^%Ny[^uGE1{?C#9aG/d^nS80fFx8<}",
    "F)iQ^YY)9)g:jP=ZtG2Q>!rAEVQcYolqQ/FDto6nk!Adw6uz{(CQPNOv+.IRIqw3v[0RS^5wzNo&84fIVW6$YW{0*^<T@>.+3+6e",
    "61Q&KjW=Iq9$SUgsU-96bug9}q}CnncaUnk!l}QQQmXLJUHd.YeGheZ^$A0f@f8SAM?RK:[P]7p-n.j*yV6X9.0J$).YKB[B-Ap5",
    "=F#7vrb}7Z(eH8L[j&Hps2WrBJ8y9FL+Jxnecq]^TxKpuXB=&n/KrV1q={7.yJOF$W*S8D%dCxobI{)k*0G<+bupQ6uMbp]IDE8C",
    "&%J*&S<&<dZEM2JJoIuCJYmb8N)<UCF(e$&B/^eS7ix%EMyYo:>bE7D03zVYlFrXFblOon^&t}YWb<5asVZrf^{Lv5WmD#N?u}*%",
    "pwhQ?L.Q[Q4Zds}4@>%&nf$@j.Xbs1ZI5&}TZa(sAVM{D@EzanVYqgJ)K)V^m&YW=+l@WhmP}32zYV6](9?{42Mx$4g7q<H@GJa4",
    "pfrK&>!yy!7L^gF70sh5^TL7SFb[4X?$[$sjUvZr/Vkj0d9S>$E5R@=[[DjVN0j-S{8@]?n@qroA/zlR<=an]=D(}-TmclQzm9$u",
    "j$?J3E$8KMbfo{Hqf=Xn5oM>]l)jGLi<usX5tNYpko49}kTh}z6Ww}R9aL}WkigXnHzPtu?1a/mS)7sM*%E05Pa^zKI)eX-P6OT7",
    "w:&Ec?znz1PTjKd(o*rtov9V5<w#vC!TLeN<y+XWYVYL5^XmHs^:Q#PZPJ>=>PnkoxsA{+dL1sVJRu^z=T%(5ywJN}&V6<tA5gGS",
    "]DL2kSr(2a}.(5Hu=K0EM?$qQ6=}S:yW4MQDJ[^VVBrZ{pUHv.J^MQCGn#([0!ZP/k1)*$KE<MR?BsfKk:?5NauFq>c@/fsywVX%",
    "v*A*-uvR)!2o8L+r57q=F<c=UryTja/1NLnT-!CxR0}u%*rzm-rXJC]3$6yPX{?f5l@:5@2RMH%v]:jaJuQQ2IH:FBf+V4J5jd!T",
    "2OUPF?CP.7P#1Gaf<IJbr.u7H{8<vGShH[4Kr=[ISkbKGN&a:XPWY2=cOAAK9i4WQqnO?^j2@}C^0c*1Zk7&kjhY*^Gi}LvZC.=H",
    "e9OE@82nP[Hr4R5lBnEg>prU$*^Nz&G?F.+2KvnX+Oe?@x-@rqb<*mF?j+PE8{23wB65&mJ{0sO/Ma?)FiX3p7aeJk>YPOi^V&!9",
    "0rqgzJq5^O+cWnz+UXg0ygqfz&QoEO1v8lo[vDR7ZB@6&mK0M1mBQzjFobzp6FW2de*3+1ntAVcuEmd:E}BK3yem5T8:}7%b8X9q",
    "fgE-Y]s&md6i?=PsuDgr8PXWoEL}*4Luz3KpOlf2Pb0wGhbjut}0NQ-+2d{%Oid}c@l<{{fG6>lXJu@RD01g.FkRB>G>Qd){B*tC",
    "?pjf)rTP+Ui2MZ0GKFc0fqTiieV#Q%CLl>RXI)B1p$0/P.u%EgkEgUVW@<Qx?b-du4S6MRmNi1#^/)=PyZvkTPiu}i}*d}Gp{X[u",
    "Cjkn<u?BAksz+1HFArxl1qNg+41z%7?FZiyD3Ko<uY5LYh2-4?}vY2NlL<HM/W]5{8=ILPBL5K=XV*$8<QR)nqE^hHM2-qa0aMQ7",
    "IfoJB1cAKBf]tzkbymwyvO(s4i!aSKR/uVsG1OXdl.r?Y(^$mi0+N0Db<{J5)F3Z@K6b)NNfeBiSf4a]3.ZS+7e83FtVmW^5QQ$1",
    "z?yCvk7IR9G!ZrP34*jh.$zG(/3l?1j$hA5gXF:J1Ud.V*@Xu+Q8JkUeRAmHtm#]S7c}l13^ROYo:wJ#)mMq1G[E-lp3Q3p5tcRF",
    "O:55B-XQZWVniofnhG<1S/Cd-^E+TCfGr8@98f}R9b(d@eCD9R=wHWCi=J#&bT<h@H!ceYS5FXeh#6+[2Yj0jt(@S<o)rXL/10(Q",
    "Nqg}B9DBGWHB1:&o4HnXMc8AS-Gr2vlGvL$5dw9xvHSr1KV2ei2ilia{<MqTp^RczlK^6Th{dEF{j!w^[Br}HE)yLcGHn>P5]$y[",
    "G*P/x8ZjpP3KhFIlH==]td.zThMJH.p/+VJzuq4Cf]TNAA}<^<9BEX!:<eCIcRE8{2JJS-bp/z1-Y+-ZP)IKcxAIkDt>oTkM*+D$",
    "+:w)Lq+vXpw1j#hNTNZ&Ff@QPcc:K{-k6WipFpEC^lFyfobvW[3F*VO@nDes!e]Qx{ZJz1]I.3*Nz>8@-f}@n?<f}H5XJfE.jRYi",
    "L/scCJ&fd}.W%grVS+)x)FuZ)<zjfBQ@>m4v8z=LJL+(ZtKeEQp#!6d?s^QaWx3U$[<M8im>Sr8Q}!]LD/+)Mi2Q!<u]LWgLouBf",
    ")X=q.&be?mxRP/n2U<i?tQWI6atcQa}]j4DL<Bt((y#hf7grcnZrBp4}%KD%+Ut]p6QZ2Q*BX]dZBLqR4$xW7h#V=q0PDm9Oxo9<",
    "kD-0?T^+$D^SGa:dL2W85KT#f(<gzuk(:0TA3+[8woS?)Oz<{&!8Z3Y.H<!Xb2m7fz6:$I6.E3>im0NEZ!&1.LTxYerh)2-[O.5-",
    "pelLSr4OJ>@3J-4Ox/P%{#84Z3&F.>*7ABeaM2P2}jWF{Izi+$>j2id1rc+)9d&KJpXs$bY.SKfC!uj6lE<n>>VAW8m-B/e/Ra0Y",
    "QZ)WyXGu$$CO.^uz:fV5BpJ@pico5Kw^g1NNb-MS5#:9?BCMB1J>{-sLN&U+Z+Xu<Fa$$n}krdlWvS(0]caBN}Oc5[W[A)p8{]Cm",
    "5-]Az?%{DO&gtOlXy8eaHT/5h:5{?O*hI:e.A8W1jZ^P?Yugapl]MwOr#w3LqiGu<3l54&9*<BIN>ytCz4JkJcy2Vo93x*ChyMzP",
    "/PafsY7K1P4-b3QrgM@Jfl)=oD[kri[R@Ty.P5%@lvu)pd5I8G3DZgG0qGUUEp$NMy65H&^Eb1qkMYDGle=cj*V2j@*#RUJzDOy2",
    "fTVr(SX)O+%5NNX!xDA9*-AKU{mM^}m/ruKl**j5{V]^Y}@^]v{O)X]&fGXxua5?cf<?BMj}J*?QD@&Z7uR..8sj&CV&:n>4rMy1",
    "q.PMjTY9uPwI.[2]z6@KoAKlqiku+5v1{^c1%.2GM%uazg=T?+VW[WXNjc3q(c:yX1a9)ea@i8J)&h+I1CYSlsr.JzB>k8bfkc@>",
    "snzi}rD0EJ:y]YV3qN[uJl.af0c8hPE1N*1zhu(U}.2m?9Z!dn3QT:2+JJF3/pn{9[fzXXGwbg[q}0ClRp.k1(<zra0xi#<fF4Di",
    "HHz&0no.XVHzUPQot)ymdf&+:cCDN{[8B:rC:>f=9/iMNi)M?Huwq4+smb:{V]7)$>&9X$qV@EGc{m5-D}Z7]vSqJlK+fX$@}9]/",
    "LHJBxq>{&.iM&{ot{i{]}z72[8wo5k]MOIx0hVNa]?jyfG}SDG:!$rnZHD7.or4hsNANm@/bfDftuk%XawB)W.E<JBpe}XmNng1I",
    "8wZsPSIUD:}7y19L2er[dJ]qnK)mY+KAK2O1FWDWxVXQ-7>]T^<vS&zd%acxJ^E#[q8F?F!GU%Ds=Ysx2bz^z-)HP{2(D/#q4T<j",
    "D^-kjHtOufm2FdVKIQm}-A?fe&manW<9PDv5WYzq>}]9DPE7*B4b@V$ijlH->rBg6ndBDsRKQh?vk5Y+hU(M0qBCgYIT&%A01PV7",
    "co9Cq]?x3Y*EX84V2Nv!1Mvu-HHEl@svJC9kpn-X4(-i9PF.]}]4]D>/6rSaAg^DGaf<(?uZ7@h*S}&v?dM*L(xlcIgy9XrO4S8<",
    "3W2#wrIUhU[bz/t8[p><QUqGRGN-wL*h7p-vwkCs:6%UkEbU]9:n/d4dZCUVWYEXoA/lZa&3ah]&(-AcZvDe9crgjld8UR&V9Tx)",
    "v#Z5)HyZ)TIjFB}iUk<+v&JC38m36KrD)v:-)S0)Z1OMh&a5N@j5E$2V>+nR[-{&KylNn.96gdlM&/-t6Ak6@T$cuZT91SXb}XTG",
    "MBs:}cy?(+qumlvEeKn.w(to+WTXH{?(6q-K-QYt?Cysq8sYL}0Yl{<YFqaM=L3z{2p1pLm=]<%J57:MDcJF3^$0]Jw9OJPW7?CX",
    "@^j{:3V=-<FYO-(@z[9P7KV!cc[%n^J&OEd<Ek2u9KacQ2tGe2!wdA:+Ya69jG=aQ=sP+-C^p+>*^4drwe}ZmGE*s<)YkS^HEs}K",
    "+i#TH1[d[[mBGn<Cd:P-m5Lgv1w6rwT5Q02UuFAGXH(<PvS%/BSAnQL+kR<Z1uca?aBl7jf/hiVjU&3X&<Y{<<[8IoZF[M61X%?o",
    "e#qaMr3iOMH-QfxaIDQAy!.sJ&hMvY=riVKcux:jC5fZ-c{I-n[l:FOLUfDsp/h$XJ/Fs$Rva84NM$NJ!x332)^.Qp4]C5B+)!.T",
    "YJ}l{=wHk!vV=g5py/oB1{D=}uqDepo9hidP[jr0PDkyyGXQf6cz7^XA8oVJm6T}GeLjHb-Y=NI9)!Z!ORAux@m^QMEtktD0^.do",
    "ckA^GNb5Asu5o:/J?5.OOnrz-Ozb^i2Ev8/yoA<-X}}Q{Wm)[<qJD2=hO{PJ7kh@Rn%K#(2wbH1B+M{hiN1lYxiuxv<]PCmV8D?G",
    "L9/rkc&/:J<?r@yNuU=PA7zip2st]leytt0frbY1+jqZHJb1l:5pO</+D4y3O&XL*XMAf4}wgUC@=&g(Kk84{FU-uh-v5t.-*4r6",
    "-78#8)1OMPy[t7HRB8PxaJBU#u$YqF:J$!/([D8=Rz&n7aIMgnnn+g=FpVrlk*HpWA-9^?]RinqBxeAaVD411=qMIZGyUu(di8K9",
    "EeE=]^=.#uHZGOk/@-ocLS>G#YeV(PlhvDx{cidMA{2T2Wf]plit04uIKVPsUT@locC:YoN[?8s[vZ0UrmW:a:CVs$y:ZNy*UfSV",
    "veH+KCm?Q6[S(2C]z3[Ik:7!6cw8N*m6todt?$HNnm#)9&^AKBi&S@VzKw<k{{5WsE6<c)txXTZ@b$g^MjNfjFcZ?x@RQAA]meQg",
    "*I/$M}RpZb<Y]X!>u:N&Oo/rwJ{&gCamUi4g[q=Mvo(t-AmGPPu&gl{L/W/b!Au(@/0.Iv63t48Vc[p*{{}Ig2kcIg!7*GTlY#pH",
    "h2jO[3svHhyL]qV?T(og-F[MB^MoDM4(jQchf}zGzPk8U.+#2#1/2XzXy#i(a{n6kM5{X-yv^.8d:?A.(rwQ0?<1}&EjEux/w^<+",
    "LPHTOCC+B8::^:j}#hF^5)c:1Wbdy!aquMA4bt735E+IH7GVOVQqKl0@7K(sPMzhr^B6FTqh*BEAb+AZKH*){&PSx=/!S=ACJLg8",
    "xo9>wpt4i.4F:nxyvKXNTjKeX!Ol0^qwt@>utY%gPQVa%3P9J2K+A?E*S>+(aUR55a}qmG1}*ajR6Uv?^6s7]jl</0K4PZwlD!aD",
    "35Fk=rv%R4ZZdzkG5#ZtZ^OI)vCB}$}z6Lm]&bu-F&P4dq>?z$Up>7koroqJ-n55tx+-qnZix9gCZGVUEWn8s)V(}FXm?r{84d]T",
    "0ph<gW?kG3YD&izz@<0[QmaIl>XK@%@i#I=<l}mR2+Kb>q?7#l)%tg3HgTgAJF1(bzPi6#!hHw<aoL?iXyepd}lOa8=s9$u(gERi",
    "2C^8r{n!$xq:G>mN$-<=CLqPYAQ-.ot1o2=WT[p*53!!>qmE4GlQLF14GYLxb0wQ-XAF&l(!i#a-14F-qEhyF=(roN:p5zs]farY",
    "-Si3L^rZ>/<5M<U0hIqf{smqGeGY$^0%UzZsdC?}xY{?TC=>+{qTS?:].vKZuZ-Ac0cpg.O!2T0Kn6FN:Rn}./kv.6Irf{+t6]:4",
    "P?gFj^SK87yVM0&@X]G]FvS50m)A&-UQ!x<.3vD--9!x+-lprSe-fw*viR*n-sO1)Zm6U/2+dwGp!k6e-%R4z@SN74MnLs0Aa%*o",
    "X%2-IFN]--85?r{{J8nb/$w:$?y?Hnf}Bxstlt:8g47k*}gT)ztpkj&Qms$x2Q}gp}FE=CZE8}OfnZNXW^283P*/wM{5t]zuI#Gt",
    "D1QA}we/UPb6WXkE486t6LK74wJUu7j:Mk7<u#.RkA}G#y<g(M<%dKLtX764FcCbFg&Vm7i{m<69{EsKJchOx2eL/647?IjylpXz",
    "p^>&b/.yYAg%!0wN8:([0o?}ywvLNLQ:cKok:g!4KM5R>@bW&Y9.4Rz63J2ywenQ*hyxwaNjx&55!iKc]^N)A*zD)4h&Ho>:}>ZP",
    ">vUR}/87d+bY7:?tH#(*=]J[{@vT8yC4f=2UhD-KC-ngRC^in(YxX9jw68%dgV.U3g[l?BlkQC3I.6coWcCd:!YmT34>gz&}L(x=",
    "M$9D=BoCEPd2*8E90z.zv00:fd8+EnuA[#zA0q9i07X6wfTnBbW8k0&:EmimScp0WY.:p{PZuy9r8#@iZ/>r-tS#=lou3V?j!o?X",
    "Zar}Y6MYeBEN5rBDh:B1X3k-)BA=[hl$uqjNeCq]b#]JfMvoN-*Aj}VKcFqh/S6fWzj.q4o{^>6Fytq#CY@{EBI{11.(g<z?iubE",
    "hCWjm@W$g.oRdAY+lR=Bm2i53:xf!61%x}WMVEF!=)0i/<R!(F@U){YeNkqr(dkb-v.>%R46QTl/qXa2XkntLI8Qg5OFm:hh#yYW",
    "+C3gA-8}Ya+v=Nz>KSdCFm]F9oSi=YPnB*PGdc{!^-VEvQ>=ruDl4b(04Tc]BMV]ySe$J6{3u]+%5gdaK!k08e#x^QEQnWg1@xD^",
    "{$nVYw/UW3T}i+g?=v&ZTB&BBD%YSj=%OGukTEltKF4+005qLSKW36k44?Ybg#188QAkAcC<j.gLdr29b.jJgvUX^lrRqfYi!scu",
    "HDqX67-}etsM.YU}*Un4UeaU.dD%:AaQhNy>>424.$eBxAXsESA6QkjDBW]Q/Ihud4X<xBw!f0$fG}S.c%BkCk-+[bPWj-xZ)<#e",
    "k*%$(Vm/H$P1f8fav73j&ietb!gIAGsM@w<rqvs]vJ2/>aO.c.7vn9kYt>QH(MziKxT(E6NBbcJAy*XP22ejlL?%wx5i.<kIlWvI",
    ">C/uj7Y[V<?/:@vL1o8%O(EQhP1Zb$q?xS=]npdzh&h{@FrpTink(z1DtWgQSk9jj%5PH<5whymOP{BhP%)lI!PdxE{tX[icyW4W",
    "WATj=NR?Daas:}oiq:4B7a+Ri!pE]?x{Y8*+25hNgjzFLY!$y9wrgchu6LQTgk)nr]CB5XEFdf-2S#MVw+$HLXkO*f5WeHMAH&%&",
    ">hbbY0dD=r3Uy[LE@mcSOAd<Ry{5WWFk.7:MvsPMLWnEBrG}16pRE<Yvb[PaxKu>o{E=t/4APzJ0P+K&eEg@irjXX+L#[73Ac$?L",
    "LT8&zru1osoXRG&ZNrG-Bow5D/Ua7zA]}ThS-o?G0u9ZSJ/HDW5zPy.?EG[QSnq[=&]So=sBW2dQ5WqtGOik!{wPOa==h2fB4Wrl",
    "Z%O7N6Rx-^Xl0]r?S/FL4n(pLPLx[d7(oDHb!Frnhq)91.2o>rjQYlxTU}E?b!W)9(pmLY0Df^c0)S{wXPN:iWf{]!)NtR*>QKV!",
    ":uQ+e1zYXPl@C^7-f#N%2YZ/@wnBJBd4:+Tl&&pXAlP:]aC1yzS6LdhsNld7GYI<uB$maqv2ZU?4=79#?]}VQj+]=DD?t7=LoPAT",
    "ckFzJfD?2(:SZ)LtCGFa0j8%BhW>6$eCmQR]2X.ZVY%7PFYqm+qW0io+dzKPG1!DM.aLAPKG@c9j1=f0S--*}2aOaCj2?4unn>Vy",
    "xZj@$T$-Qj/^D[=fH@06JYGu^<Trj8[fyMsOtyUFPRov/ii.XY:NaHwrp)SiAH{7x/ASJ.qScs[rrEoQFljJ/hBgJXDFj1+{JT!y",
    ":te-rrW7Eaq#][hKx5qzf+kV@@NGug*4ILZzt3xi)uRDI9h9:7INy[@{.UP@u(DdgJQBY@{mdNndGtq*n+:%K)Iki7Siw(T/QMP*",
    "u/g/Cs@(wiTsV]Qx#@m%78V%jkM)rkh/y9dl/hon5A>l1QvBga5x(E+@QS/F>rbVRo<(dc+]mOmv3AiaW.j1Kb/f-0^b4HhXw2w:",
    "k(d!0($dReu&%!1[!sFQ&c-3$F)Q/cxTJ(l5kF@Z?2p[HJ=^JHA4+lQ>Q4Ps)Jy<V5kosxlSLIEC&tpd]{y[aCTPM+EFSa&s7@L^",
    "ih.?Jz.hR^?PF+Wej?6HK@nHdkAV8XdidIkMtdp3G[baCg1=Ds(/:qwgR#7lBC7q>l]z=[1KX}!MWe*00--R(^G9:??A(#Y+:mMT",
    "Y8K-Mf$)+6wO(s$)1:vV*>qDoe<*HuQ=(aCpd(6X>-F/TFvhO[Dz:?dd#Lug&e7]9sO[8>5p(<QduQe-M=Rz7vd4hZ2:()IFBAuD",
    "D]anQA3Z]o=)/k-!2y0[h[.w*FJ<@mHpq9nFP.?[OFXQ{oo3o>Xf85vxx]NYL4L-Ff>358jDT>L96bN.yiB8KJhp?-eASJ/[kjht",
    "*n6r:5EdaaKy(f*[d>MeBNUtPS=@+L($orZ/%6^9Xy8hb9E86TA6Z6U/+t@2+ykde",
);

                const ENCODED_SMALL_FC64_M7_U5: &str = concat!(
    "Lt/Vpl}DaD&!U9caHkoo6uK3itD6>ITGB$8Dy*@@q)hVNC*i$ovmrk<+Ks@l^Fx.58!PR]>K}cMI1jLe(zhCj*)e2QBBN91=a]2@",
    "{JDhv[Nh%wkl0u2GS[y*hMwZGi4f*2UNo$:0hQByk%@QO+<9#}uxe{pI-E#Ot/1s>9z6i[Ph+D}nB6UCm:N4aUP]NHY4zM%qDE!G",
    "dEE>-:#c1Uz=.lFX*37ClIf]-PL{Bnu&6o8Po2eGX=dsDSt0ij]J[Gf}:m:7r(MZND[X<tvH5R<7@bmDy-8C2k!QadQ{!&qjmB=k",
    "a7RW<DoQi0pP@edPg42sc=gtCCJ05BThQyr&9qyc>E1.Y}&PjrrXmdXmSuHkEx9=u[nB]f9)$$ur#tL.nb>b5J{iyMM@484H%hXU",
    "1HmzU(Z)A8GG:At<yvTw3T!lyQ)TtdtQmlJH+[iRCP?]bR3iE5/Kk{^9T]bk0oMZky0Q}n.YU>&z}]j)7$W?i8!C2vQM4D2co@lU",
    "+$Kwm{M2$KPD*zG)]ZakMhXh8Fe0R#{6&-uV!jL9=%tP)ysIZ+Mdms%.C?ErKxewz[lGXu9cHUY)#LNMhgtx=H]Y2*epvKZ>!9YO",
    "M]TB{p<eV#Cav6}=^w4ww7)eg(k=rPnU$^!aUA*YT)ybyMRnjf14>t.z6W[0oTdnOxUJH&9HpoH0@Y7hG98&L.c(E&Vq!L3-W@2&",
    "@E)W7LI(WeML9aSAeukg^&^%t2?M<PYogG7*0FFpeM#gq*}V]NA0?<[L*W7P&i).&G(E)x(V4}Gi2Jz)&Ob[EtRc4v%2n*$*r^=3",
    "qiGkrFKHwsBDWt30b4.l3ugCT/co3m5#OR<}e[{mBjCR39bQ.IRWjoQPTuagi*sIBdUi)Gm7Sld75j&yV6J!J{o#Urjn{(}DOR/*",
    "M/bwp<c:ExfB4/}6VehftO&TrFfCW&EXkf7{HizJ1l2PV-RpJtz{&.<ct}wGE-r9+tj[QKglPO*8QiT)D@Eq-n]ig]Hb5?x08<us",
    "<k+>QU=m67bAH9ZrSZpOI+p9mg5Z#io=G!tC?p3Hq<d<c7VA9wXs$4TD<W[SJ4/W[4NvJ!^AG9-u(tH#<gM3^J*4C{)ixXmV-zu:",
    "^>60gV)2@w<mxQ<nuYLoB8XrB!*>h)YJYc:F4u03o4NDlFhMZRK#wPs>vh]NU#K^!)=6ksDy6h@V6=4:Uq}SR0)Ve9B3G%[X$ha0",
    "*w4UX&9qj6.)v?9!:AV8n@f(r)VLn%8KVn(fI8nS9)xTck^1Fb3H@}=9:GKA<X%diZlbCee2<5)(.ok0&J7IvR-m7cr2x${7d1!g",
    "9v61zPY$pC)kk!BZ+2WKsko=rB4K$CbG=7X+:!vti)@q!8@>i!!Tjlop@KY)!lkCR}a<C):x$S6T6DZVc$Vjqi$UKSY)+b![jP9g",
    "M0R0-}?CS]4D&kOpGv.m5c0V-t>5xsAOF5^<Gbpy&PWg46tAtpX-:skfVZrdkeW6b-i^DB^Zd]Cpin6J{E=Esi0*8eAGgU{(%G$g",
    "*.ZshrG%vzeU&M:mP?=n52Ln$a{BD0Ga]os5eRPmq+C=SM-ki)C-n7N+<MdSOf^gd!gXA%=w]?aAN.xU<T9qSGm-Wc}7W1PY0$c<",
    "-8bnM5)&U(.r2yA1:g3ZKmo3W59qUgln:Ceg:[?R1<h]&[Ein$W6D*D&D[9S@3VetB)&f<KUS:}FBLUiUR-h?v8}y)t[IxCUIzGc",
    "PP&9.6NDcfKpcoCW-vt&d/rBi[gu=o5h+[1(ZqSNkU-:>yHEf901t^s@(LQj[OAzjc3?48aI:vbTLpj$&X}37Hib1h714d!qMoaF",
    "FrD[B[b%s!2[A%#hw.Q>Pr-A0o-badKvl9hpMltTxwbtUteFAByz9v72EOfR=6%LN^#EIyw@iGImU#:XEJ4W*S(0KOoCiHKMEuNj",
    "C[puEGJyM8j}yo12NxrMMtg}k5q]?=U./f*0^HrG/{v&lsj$nFQ}W?XmS9*>rS.RJt$6EWzn8^T-sLB17R=fCfqfyGOC>AeFC:5[",
    "kXyY)Y0NU.45p9<!vxPZSc/F{[Ik>ZH[WUNPR(cWw076fL0cZ@oxzo#QQ*)$C[M{=YVzcQ<#^?24smoK>//?=LsZ<0:arql8L:pA",
    "@X{pU9p+akdVU)*gE%$$TgnqCNGeBj..PF1jbhrGr9fk=JUDMFGh!B8a<1*?uSmxAEl>h4{r6+QHl[^IO0xp5Xka}zZ)KKLZ>y2@",
    "ibx$QW/9l&q29yuJMsjA}0A&0o*pzFNq/F@g6-y{tM+!PV$?zf/ljtXKQw[qYrp?slJXU#llU6@{#Dq9A?vzoIt/y@R(#J4T7vp6",
    "YdePGPMB.TCda.u2dpf*Q2wNaTy)0}1AF-IA3s2H-?0#fYIipSN{9cXi?Mt-3kyPBD>zBMIh@/z59D.L:CVm{2hBWtUB{?qVCR:p",
    "zIJQ61@e]b!}QN}d2e8xlD:*v1HzG1](EA4xpM7!fP5Z<Qov(^IvQ<]<Ep&^r>w]b@tjFb=L5D88AhK%iC@B({$nJS5k*]&*uRr[",
    "B=3h-S=N4d0:USnd6*fxa#a>^gd{DqTAJ8R4&^2Z9Gr#X{DK)a*i:mvu*Fd=(j1foc+Vm!!o3++4-TgmX>!K6&h=JiCTbX2TK1U{",
    "-U7u@J-&.L:3dy7%7G+xeFVO)a2!bKNgHr279Fw}N(W34IEykir$0r^g+I6Sy$C1o6!s8.YbB?=H-d[VY2SslKZjl=ss!9#?y4f[",
    "jHYI$DDR!<HQ)P4}XoNN45:VIc*zg7l&90f@8XoUv^&9+6BStL0m}j+O3[OBNDSUzw)Voo:7$H:koqQzg+Vv4UsKvgmD(tox$<}/",
    "QcsZMtnh4?XJ{9PNw7uTtQ+ut}Amwjv8gNhDys3hQ*bvZi1bU$Se*U=6<)!2<z}9gN#rVE^0G6i2LM80eRB2np{dWF0I4/P7M#wy",
    "i)[wG1k]=:XdgBgFvthn:tX&q7#zR.lq{H&Ys-+Up9oUM05/u<obekQb!*H@ZX&{KKRBYR6rSioGjDq(NV4qiB&<<2DsFG?b<HvL",
    ">zZ=KRj<twL/$$$gi:nxfcrS%8*]&x8E7]T+z#])>foqSeQZwTEkM?[LMFrTjV?pT5Dh*DvbtJWTp.0!88crHD$?t03tXDYr6s?S",
    "O%tkE:k<3V:b=(Rd(+44a%EyUqcf[-RKDIR(f=^exUKBdS$LPH>VBE&:N>9FM)[T<cF%M>05tuN)kXH:Bk3).eCfpZN2CN&}LbQR",
    "-c)q43T33lY5uBFQgpn/LjeAkfPTNy0LwoxtOsVZ/eYV&4gE}M[awcz@)p:f>8qF!7[GM$VAM2C&4{b26f2XH!h75!uq0KE)2qAq",
    "n}sZ<ZTan&p84RffY>mak*BEn{baA=uK-hYtYA$h8Y4J$vcefp^6?1UWG.mA)o$@q8wk&T8kOJvH$TsxakNmp}OjC>ESZmP}g/OD",
    "]NMesyA}K%L60=?4t[EPxOW/}nCokI8FP%HuIcVHlkhQEc%DwUoPvyZ6=0]nYF:^]9y#IW4Vl(wkU21>oDcnXG8::(wl<e/P7e4d",
    "xo+zts?k28K?>A?SQm7cImgbYSf@(kC/9?2Dx=dRdH&&1?8V:[bH=!dOyDgOLC.LEEx=>^p/(2QU0hIwfY?1KPo*uTp!>W/0rhV5",
    "Z)g/XJ7l1z(&/^u)aRj@!%36ypOs8/<I3zZ<Oe2OFinH#{5$-nq3}b5^IrcL<KD*+(:]0!ym2<K&DJ!+Lpl)MFJk5vxdSkUZ{O.R",
    "3^R5J2S?K&ySr>ET9>5.1?2C=rk?M{&0qq86cD:cB!6L=YnR$p4l]Rh!8X(/vwk(%xDt48[k>HXdzYQVlN2NAuMP=(6UZXj>Srs*",
    "w)zYaXA#)KC=N(cqi0oy*<v77SZ1cW]Zp}R?p4cxyz[AYHDD!zh3:jC&/mY=ievz].q[80hh2l$l@PI!yvunTW$bU=s8:f.pZLJi",
    "^e0/bz10E#FbvbCliMWsrTA]YCbC<m84z-(+?[WGa<mIX[xBhheE1qIrj+64[?{GY&^Sjp6USD!5w96G&$w4HMzq})R4Nx70d%pz",
    "du?}r]qNp6tW%-}E@n2]B{yM4Sj0:h3N*}DhX#RgovFISP==rTn{-6K5{ov2n>nzm0=)A^}JCXS6C8a]^5Ac&VL0!ZCV=EVOCY6z",
    "X.OX#(uFsMXfFk>SYcwfBruMbYFw(/L+L-L[I2?-3Rwr2A1-WAUIC)ocCbOU4DV&!mleb@Q[XTrp1m>fO#ykN]l:fzqe6^UXduel",
    "[F@GhGBQTsWUe]&>zzhZ.Z*E9tMCK5Y{lk[qDPbCJT8ZHhBtN0}]*d{jMkCn{Ck^ZmCOu+/SvlGHkSmGTzalsH2{K)F+x:v(i{MO",
    "8D[A2}Ha:A+DeN{)H@nTxK)n?720rmK]iwm&3ah}Sd*{e[3pn5Fi(Mo1hL[?cIVreV>UL:0@d)vSROx:fSq7+s+lE:3)@hxLZaAo",
    "X+#VBC#X(Fm-&cN.^yRK)MQ}sx9M)HPTiDqcy0:Tg#7a8p.8tA=C43a@e2>^E19z9dXREdlgCk}<zB(W-qt]+!tinW!Z{+j}CdNm",
    "iu!ynX=S2FXkQhR44JquVTxa.&b8*-VRRD37rh}e*iPRTNt.qkW(+0YC^bYgO((dV16GfF0-HQi=+UdA)m!DYh[1{-{]aFh>QCJ8",
    "t58+O>81&ag{65cr=(wBvE8#qs[s49ijlDiY*Mr[TC3TpHoN%D9&(uP.Eo^:lpkiCXmY8hTvHn[1Cel&yWWPLhfzhFi6->A&z#!!",
    "v.==)IWk!nBqVq!sjdsqLLg.l(3.[YT7us:iR5>cAds1-btb*oinaky:o5/-b^$w0Bw{HG80[ilog!+qjlhq.LS!?9/!8ON2h>oK",
    "zX1iV2rJRY=W<rKHqo7!jhj+cYrd)wxaf.Y-d6>Z<n3qHo(vtK7C!Gy0Z0[OSJ{]+I7taeAwI^KDJfhx-/E!}!5zQ9b1DyYBVfJr",
    "4PSLw/C&qL*N+sQ+Ul8vs[mJ+kTNGA).gu/5R(EXv/AgpE27/^)aFIO4C8f8:>zwgvVX4i0JNMX}]qHzWZ[66]NTwi]@wLoBOv&f",
    "k2qr{C])2H*[$oq3RLoG!HAR)Vx&2l?FEf]lc-a=7k5x}y<gB[j?7%nP^UEOW]VCuGj*JlPP:yU92T:H0m1lJbTvKau)Gr@J+LYf",
    "C&F6MX)3EB[/%phNSCC)UL>Vr.fMIN>rXHG[1[L$6Leh%<GEP63yq8WRX**AKaU=(AV-v=2hGLEQ1@8H<yxw%6CLsD{u8MxSU(F-",
    "&q]DiRpq=-.[lNBUOz:N+alX*v}YypAHDpfXBCzt&4=}0mrrGkCG&ciR)T5#ErN$v6Wb#f}K*htmZ)g${g>Ed{Rwn(G?fL6(zI#>",
    "n<BZ!T)%Q$X1r@v]xzjFB}?[U.onujW.u&EKZqAU6}(2HxG>v!*DIyCvYyFjsE*o)!jvhmPJZY}Ofx&8UT[EI7]k.CZ:&K2v5xUX",
    ">!*xki7liGH(ue:A53ok-3i!@(@pg4A!XCB[?}yI@Q)h7Hauy<q>/h-]Fd]g5+tt5*Ty*t=&7X]>PSd4T*PQ>6S6A]9O%{.SeBOr",
    "eND]ty9JafvY=}15M28-0WawNvMxaPEPI6kArg!ScQ8vXsXrUE^eDr}(xPb$6]-<uEZXl0U+g+PW>AMKp}pHd1fX38vimJ]z4u(=",
    "!:2&wDjamCW+<[BT%+eKcnmG{-B*sa2)g-6bh0X{sy$4obLMatX-BR>F{64ndct+rr}hbjqzPAenL(Xa+{JlG0?Q5/:j1b5^gSJz",
    "!vdlF=%Z0sRSdcW}--JyC&O[t]0rWbN.$yNq+?)c30QH{:YE{{R+EyWf1^]wJTdWfO-!2at1aB+RD<qrKw/P4LX@ouh?@$GK[g(M",
    "sL@<ooxZ)&ewXE4?RsEhKZM<9n8)VlNirV^^E*qH0IQ{Pr&Te3V-HKEno=8!j!o!WpeYrHj&I[mQD>@{Ki4<B]p&m#&y]L3+eX5b",
    "I1#@){O+^uoF)%b=0@s?7>#3OJkoi(&0a{0uX}&F^*gj.Loy&duX#>r*W/LKdsS9%K1I}Da[.i-9W8y:Fu9xEQ/)sNJnYgEw8k0=",
    "XiRvMY65Ay^.Fmmzz:Htk7u+Iw)[-O.Pz%AjOsK]Jakz)aAj<793}/4]HdixfX^[!SR0eM:):t+i-D!SKfR^1D-r2?}!9&h^.}cU",
    "G}x^c:ZVkMYoqeqFuCFb+nPPT^wJ3?>gek1@LB(mkt:)k3mu=Qc3ixQYZk>!rj:2iZ?Y=e/NFjY/Jo&wt2nFrikjMs]H:RE}2a*s",
    "@:e^w2Y<s=.2kDv1@!R$&nTgrw/BgM1@{*XFc%Yp:Yxj>Hl<5D3!beu2EDz6@<AlHMPtmkv<J/[+2nQgGdg+6PH*0kIbnEY3$bae",
    "XAm/}vu6RpoV&CXOB<=-M@WoL+P*-lmjgpk/7g<-vJ)J<tzDixdT<=Gf#ZH.6Tg}d*.D2o9FY-&k%9l5{lt(%yHYs/ka080&L60t",
    "P8LiwQ%TrG(/n.[rv(V-eA.KD=#D9I8L9wi/IAezSuassLEvqJuR)02*?E.vWKMykv[OQxgmFrTCLko8YqVETNaWP+9in5ic5OxW",
    ">^POnoTfn:4Zyat966X9M:P*Q!6}hT?V4VhtbWK@em:gy]Sff0bG:pQxF3hEb0m/#0n?rJwev%7YV32D04/AUH<AC(40:A*aK@>y",
    "p[co7/OhEKPA#HI!to7RFtb@Q3]u5tydR*Ej#dS47}V/X>z:q(k.O>]b!Ps8->xIT27F:07n90pa)*^z?^PW2=mHUj/2xOX>}Vd2",
    "J/$XKanVQkoPH[</cxSCs7%>jhDR&JjL-=DQ]<}=}Tt*}2Hdh3}y.0Bk!OOUMgYWwL%Gi0>&$V3L1?VlxRjxHw+gBngp]uN5c.Ws",
    "hI9Y#!)tX$LR>FnwPB.jt>tA*u}[LLE+C@e6@D#7&/6Y$TW1+T8Lnm*+%0h.!rIjwdDA9BF<Wyr?XPXnNq0.R{&?$oyg]xpysb%z",
    "bt.aFhLuf{=U0G@aA[Hid!J3&W<p%^oR[e<7Z7nYFukrN/VxR@+8Y]z4h7?r/0wz<M5d@zFw)qSn2dm>g=KX.ByG)oH/q}?6@?5y",
    "DgDRLs[JB(HT:*pqPKkHH&-%qN^8^{EDe2NFQ9!I<w99CXdUvCZ@dDW+aJBZ-![s>KMP.f+hNDQkPU%0E4n{7Se?AKS]MvT{:wqt",
    "oK#U^Xcsj)M}aRG5raPva5GX+HK{NiM@V[{Cea$V5hK+WGm]dDIMxv.&F{:G=#2]0PIzt$M)7C4[D<q3g:-pqs]%QC<lP7Brn=JB",
    "]MEaBxQ>s2f4@/V?O9Kl<Dht.BFU5i+7o%:1BJ.NQ[0(pq>m8PAaGG)Jj}Y]7jf<Ym6b06z[fq<c#0qb.=I#ic#rCcys-OL@gHU+",
    "xZ[LQMx*L{MJodY*/{+>NP6<k)=H^/k.hzqJhE)-5d?z@2YbKm*Ig0cWzZ(:O(4qTi/neB=<&u8FJnO2xmDmhb^3gv.aVU#UPGS6",
    "EZi.5GgWvL[xkdLxYq=J/E&@j{qt?4x7^@yvUwydSlpTyGbMBcJ/)s8XqFOBezL?v6ome#Op[1cyqV%.gPW-xcu.8R/2rDUE3=v%",
    "bNuB{@P6Lj8!F]W(X0OG)qn4JwnhjXUU*uxOPbo9C8^YVh#SoVb}QShp%s%kek}G&>=W3h7<U8N7t>fp01n>sBL6wMLn$r1F[VI]",
    "M8::Byn#V}80(jP&mY2irn893x7y1bAXn4?S{28F1Eyn}Z&+S2WG@8gi#@]pFk.Pxt1[VNdMHttP:dwS1yhN42G%G*Xnk9SVojNt",
    "mLUaW0%VXbq^-]}I<e.9a+rVTD.3IoN3p96yPg=9PElLFP*nJsURDA@Js<]euLhng:man=Bp7:R5Rju7?CtT+F62w^[BoF(FuE5c",
    "TH5E3ES[@ev9/ixuu5uC^2XZJor9!g:O77@Qi[#nUZY0NRV.(v23)lLxg>7{d#=$X{.9A^r>0p!:#^CN3Gl9vnvo}Y&+Tv.^d?My",
    "HqAsw@EEl$W:Ef1cMh!AQmB(&.x+{vzopnuqLV6hIeZN0O<=vqFyZs8zOMQ][LA8%<Xpm?Sww#!xWSkhQJevUO[.j/c}Pib<<+tm",
    "fVV^F=hmPa*qIHOc&].#].0vsKaXxmmaD3o*9lyb/J?4ZY%3?p:HyX59ES)X59FUXQv31lXX}7MV[PxXaIBwW1RrsJLa&:R5w43!",
    "?eF6hdqW9?26alrXn85ThePCcN)ds{7}7u@0oQ-6bw003J9mQy)og.#Zi%VUPPus<?DkN$ppJnESEm]k[8(sqFi-G:gqFmJRXO{t",
    "i0ba6{.c9f9(ST[p.aTi@i)z6DDdbwU3:^:7&=@DKrG0nYoEAKZ]]A9@[GH$?Qsu@k[qTj>X^)[ULW0n:Iq&gXu[mWHtAJRzCAg9",
    "/F&)]mJ=232)A@n1Ia$80c2b7U5G0b+OT*6pZkdR((Yg&oUoKLw[=F67Y.%%?Qa%f99q>$m]]yFp-hKjL3:8fls4!#0/hiqnA8tp",
    "0pg{[zuxdpK{CBeZD=A@f<qo7UMN$u2VpN:PrQ1kv6hV9aj0)1AqNsh/TL?kMl(R@Bhl=#h3[/CR8P11]3W+ahfZ*?C>nj7Qqe6s",
    "XcT-Ia}uxxrfKTKSc(s)Q-}b0me1FX&hpq5=vG^}frR$2VLvBxVV1Tji]8^fPVxNnX<b^HJWL8}j9C5CF5AXFi}HCaP0GI5KLO>V",
    "pTx89oVZ8Ai75G+I(fB+wWXnBA/*I?J&!FUJ.?dr-nV9St&Z7x&m>&R?#&$>u%l1kTtY63*m@w@PXwr!CMo#)3<?OI&piy4iefmM",
    "kX&f&ulH#oaC*N<0278Io5*h>ch*AEujIR3s?j2+-{vOb(R%K<X<4l1b>v]$jc9@pVhAPA:I$}SH3*k26W[f*lT!YZ-NfyMy}&6C",
    ")Kh3#lPG#^cj83k{BqVCG0nAxmit^FRQ=:3iMCvrM.I47VxOn3T{oqql%UIWp&wwctj6r)zra5FZipp9c]3im[*kP{s7>e*ips*I",
    "NpLks7Q/b.aHPrOs^T=OL{G0:76662mVG^F-e*p#!V!j1/[-M<k&+x#?([W@N4N0:<?}+Am>5h!jW{zP3NXiN+7?(s^3uNN(o$Om",
    "KKA^mp(x236$>o{A/S@djj^6rWAU^oSYX@%=qhwwKqB7)bdpfeZ%BEyji={3*Yok{-sreqg=*GbPc+R[IphjkkmF=1Q7H&#Ou!tx",
    "adV}aUu4kUJNZ/*@b<v2Fc{89&=QCGnl[*3ZhYjECq4%?U+sGHg?6+8&=r?*b4Fy2-%>!TU%6:4Y$x-CJ{3{oLH=D(q[K)0Avpd!",
    "YkC[NiZy}M*MM(}t?jM(^gc&7o&ls4qf#fO6ey4WtvGfFC2XQTA^t*vQ(5RB+siZuQCx?J^LO0+)w}Z1Dc(1yPGSE(zf?Kjv}WCT",
    "UCitR1#YF}DP{b%/%B}%9xcl$FxKa/.IY?Rag/@+ZLUPD?x#:BI.mpVS#uyIcS-mv",
);

                const ENCODED_SMALL_FC64_M8_U1: &str = concat!(
    "a)bBYqb.8u^fu+/bXEj[lgeqFpan}3B4nm8]DB%XFTVXOP9>PY&<w[N/oZ5jA]jWCaGDbrZ-*#!<Zi{f*5*MK]*^Q]Y/!triw}eR",
    "c?8]6Jk3NH:LnU^J&&&7<29ZE?EItNL2@75FEJ}?Tx)IxL/}T<-iZvA+=V=nu}qYVJ71]7zK%>xWpcLT2?)?xUFSPpgCtSsra=rE",
    "x=^#3aZv=99QOen4/wh$T&TUTD*6?fA!an82)#H4zZff-py<Cq)5Wk2&Nlb!7bWUSJnl$q><qG=XlH2/vu)cwOzAnqde:1RldVDd",
    "<$kQSYXn@vShvkxmpqSJwz=#w/Vo5=0SkK6-m])Sk>NpNt(j)&ub$d4}}0Wr=3qs-9riobqx5U27a28a9pKL6P$VsA0O=*346{d3",
    "In(zOmr$(zuhDPxxoQDE5%bF)+wtA:^jvKADTPDbj+)JfHvA$Xwkxf^URIMT0.HbrE^&%(&?IKD.qhyU]Pz$ZC?Oy{60#F^j.K8)",
    "/V[4<Zz#k}SMqtaYg0EF}/&BV]k7(#v{M7DuqJHxEbH!.]Y><amJT}e7^Z8v.DQP-rgpRno*jU3]EmHpV-}=/2bcDdA2G$kHlJ(z",
    "@Hs-*Z$fbW^tu7f8P]JIxGap3:u5JE5xQz9]eXhlk):qFlri+yrg*GU+?lC?/z+>n?[UyrxQaVxX&&zAhXJ448YTZFv@lAMOs!VK",
    "0(k$)^e$]jnGFHdmqI&-w72fRmq9/bNOSDo6O>H9qfT?eJbwLR*EyJ)dHC*Y=:9vdB^b!oRKJPdd^y$o>h/x50$5zG9a[/QZxRu-",
    "o^N->l4<u)*zd0bdk&Usqh$K!P){rgIAVzJdR[*)loD2E*5Wl$q.q6kNV9H{1mb8j0z0eMi!Ks)aS9OWhwsw1eFhkKZ<7e*TU@tQ",
    "sTLyQNg$cmeih=OZ<hebN2Yq395xceS?a@eqLGHv:65B9MvL}$JaBt04yZ]7jU:@t9</:%z?OP-WpaOG^tWe0C5mt}R$dtXAJ=:.",
    "L07fRBd{K=!@hh{NX*IcwERr@xN7W.vAfa%{+Du)?H9qNqGvA5p]Pml]IYfY84%hoe?U}(7B54QeyH{?IjNy78.y.DNZaJT+Gt-a",
    "avztPZV4al7/LYyO*uIjPqOZ4?t(1!eG}%z*JT7qz-^eyxcg[Lmkt3fVh)?nXo=sXwKJAeo//7k?XVqQnXx<x55U!p=y]?M7A/Op",
    "x2q>R?R&gZtq&B[6+x1Yl7&beY)+C]+f2D41DytYW%{n0.??OXCT3Zw[z+-S9nqq]=ym-}iN!2xY#sl(pu+XuOh<e8kA)Cb}*?B7",
    "3GNjgw8X$49YRRiaK#6b8iY!r=uH!LgTl!2k}VPVzbB@U{i^N/ENE!Z]*}!tG$&p7oB&!&97-&q9m2zmhxUsHK?:-9}%d4>7Spj8",
    ".j+.UIQ(WTl3%lR%mz1cLrGk+u0!Qn@8jX2Ro8p(n7k<Gf8/DD?F4*fngNs@oEHE+3g{l?B@}>>V&X))!IW:$wGI<Ot9n{1bRvDT",
    "fV}ezlZ3foKp!XeT-g2FT{HQYjk^}r9s+iBoyoWhL]a{jw(/bGbdSVBT[jJ:=?3x-eZ{61o7:WEVVJJpFm]&+B0X)<2r$}%ZWMS^",
    "rq={k1o9qMU[hJ41W/sN[PWaecnD=dS-ugnKH6XOVZqE>FFjY?FHlKAB<.+Nriw$Wy{}x$K6PW/Y:fq7t5^L7:0cB4fC=]o%k*]9",
    "=2ns#bhHWeFl^gT<:2(F5yzSiV2$)+Hf#O}m8GD%1PuOcMY$T+dG=YZnTIrBt2)7mN4RyhVV/RoF#oE02/l:c5RmqKWxfY$dv7vO",
    "&1A-ox{b5IAE0XU[8SQ[yzhukE^B8DVCH$3BxNFjDRBJ7VNjXADSf2*MD%haeTXvRETIHn6eqEe>47y5[w^QM<Vuy5Pp2w=b^xes",
    "=/XB)@^f*wU-AteveK>ax:.Np}dRrx7y^Df[EUWZ-Qt#N5=&RetW+/sO@+]]+w2]#APp=D2)^0lWv/os.WDvPA+s1rgWL:/^O0}g",
    ".!SHuV.>e]Fd4KM1S)3yPVmc$T*s0ISPZGFu/-9qXj[8fD%p8zpjb&3vcePAA9%UH3l3Gr{@.>+AuUxDm]cAN<^VoiN-M15k^R!2",
    "r^5%Q2G^ik]i<R>:^X.ZT7J}CJ[YvMiOUoi-fqAe^CI^L%cy}[L0!7/?f>ez7)b!2A+ilNZ^EcB<2D6Zl0#355mH!^J.7Fd!H*RW",
    "KBkkmuG&E]mD6p<SFIMT2^j?7Nl$sBw.9fY[DfPyl&K.X.GTnrkpmZuAB4r.WsB8$teIk(gK+]UJR8W5R{pEszH9%1BGmpkzsdOx",
    ".-KE7bT=fb=l!dRYB9PgG2TVHK{vWyFd/GaHL@BI5Axd<s?849Rb!l(GllK)-11gUzQzryM[vzWB!8h/5rX/Zr243#ZL{P61e-j-",
    "QdUOJi?O4p7qe>zszn!*U^u5Df[UuCx*dwp-zpJ!Kuf9$Y-TfPbUy.v5eYBG+fhYQ5xV*(tTuH]k&lg^N{H+guTZZZ:[dl#?uj4}",
    "-iQk)0O}DM^k2vDB]TFlUCS}:G?EDr[A%TD:ulZ<n{v[Wvld7AK/zLFD=7iCqjMGah9p[5Ko*xJF:rcBb$phl*d]-5+?KRX{K*e)",
    "qu7HES4[:[CQ4F$)rM{*B]/4+9CkZxjiKiU5Ft}]oP+(d<.E2(6tg-=2Hw{MKh.ZzsKWeWcYF+LF<PvE!l1*%pPQMVMTREd!xpYL",
    "I!U7j?Cv-C).1Pz]aXn?}jdjxVQ<1q@bPhd@u4*#Fzmy[4fHVmt{5M0}5=>85.Sme7dBPNlYm6&@1MI3&se)L-V:e36M*Gu4/qnG",
    "A8FCP}Bp]Qnw4tNjw9:VHfo)b&1KH3e-cZ{OYwl6yDc@f^U@9c)x(WL8/QUEmS6/>?UJnCvuq^1^z6DZe7A*@P/q[.p=KI0{XR#]",
    "m.MyD[pCh:LvN=T<.?&D9uk3({i!v5uGhg5IEwCbCu<yCY@&6.9axZEe.<h^/Z]lV27:PN1cd)]zA9Rs(jxM2a)vy=yYYNdLn+7D",
    "FPx[^g9)2=Fp<WYXhNs0+JvRz/DNSmM5A?#s7]MnU$SDRBR(soJEwaA9p*kB}0fQ%D%w&$MesbQnCwpXDJJPFNt[Bh[g!fhZxh5f",
    "07f@6(/cZkL@a<>PjsC9(i39fs(3qB9[BQUunu7PNKUUR*D[crp(8r/vRAklzN50gOqWb7=d@PqvEwMS}@/p/Aq)tW-:=/$U<r:J",
    "vTNHOm.=HEe&&$@<HP]RA#PV]BdZs.q0B1OurL4Rs#gQ>+aID(k!A5<hVG*wE73&8L4**S]0qE.-dX(qr^zIA*^>73vkHXul-*tK",
    "T$:s})Db3A5:Z-8w/Wy7S0N^Z2d?Dg{@%qZ(bG:orM&X4?T4)r3Krj=:{kTzAe^TRoQjT2b%Uc/hidF}G}%![toGd)-zRu4Fv#NU",
    "k2!yxy:FQlTiyBM^L<w7EBBH}3YcY7fQfZy:tl:Td.w/@E<}/?RRSSj}9apfmp?]J&EcFBY[mE>2672l:BGX(}lV0(rLZWU01*!D",
    "xmU!2[uQ3DA:[ZQwh{Q7d8vBEZ/KJ0KPar6Xhw>2UPfYb>*.QPp(ctUl@ot=[gZSI>PjuHAdt}(XpeOl1D(*0fM3i<SfVTf*7B0v",
    "yaO?!PK7IUaA+-ru>AgEzEHyBZ(]M$5B6=*9?*/>t1#xq0]riaL>5s$pexAN:]X4}d:%c=)1Oq00n]cjtWtH=P1Y#[JYW9JfKOuM",
    "J2&nrprqXk&Ss}Hws:J0/F7mRUQ1e25^1Mo[}JKB1oSK8+:cR?+Rhk].G7^LSm+)#GU30M:L*mm)bFEpO/VR0/RYozyu:P*0)69e",
    "<zIur{././{>P?KZ>Io>s@h$c>=MCHD$S7ma#tp[1r9K*jX3s4ckgz.Qn%nqp)Xaz@?>uhAEPM6F}2Eqz:%dTe-v11recw}Obc2C",
    "Vblx3j17g{=??3iNxXCfL3D%d++#{N-XV19<cxWdgHFV}j@B>{/8Srz(s#qwxxrgc&CHwkExC4.X8j:e3CF{wthLaFFzTA>y>Tl0",
    "LB.zfj(/6}=k1%zrG%WIgi^?477cA>b->IV6dWz5xA4Lmas7p9Se}lESWnGBASzoor/vFJ)}^LrK>8VT0+S7T9>9pQzvqDpw%v:b",
    "nG:ee5si!?je%q-?OR^}Cy*e+0g1P/mbCBpmsL3CkL{!Y2W:L]Q#$cgRA6S3k5.SdUSK*ca!*:8x9FP85)KY=GpMvku263{.Av].",
    "gXo.wC[Z7/QrwfL.cHe96HT^&!2YgP5mvKWAlN&mSi!a*j%q1iKGLuw:0IiTdqgg].x)5Z(+I8:d&Pi^U64JyUGavu/?h@vNq[4}",
    "b6*{^3ar?%h?ID3iNMqr-O+.#VepQ9!5?5vGyw.3Hf]<4S$hT4=vWc0JEd()zlNxXrLCg<.6V9z93kUOOnyf%FY1]V:SzD+Pv{Uo",
    "k4#NXo)J5rD.KrG*zn6$K2R*a]}W{q!*rI)2?T$}Jc902g+Ecs9^95$>/I<Fs>>C&Zhp^3pjei/jD1W7{EhXO*DNFTx>?B%VqKQ=",
    "bHd%R@nAz]j&O3$GsQLoOhg^v-l!+5y0.#9&&4dq)D:%tvB$:@^/f#}IrLC.1n2CNaDHlG&Hv6:P8{jH}QVg&c$qnI+uJ]nzmDgI",
    "@SCXNz/3u6:?SJ?SXG=3T@=uhuhyV^pl#9%}??Ntj{4W@M.3gH}0PN]7h#A1(zlLOWe1@f[?cg<Y.[Usd</UR4N)]>(sK[D@4F]2",
    "XIdtLgK=8ZJB}g.dM7ksC(G9NlSgfLxD4&5!bVFY9f*W&n%pbR3[@^pb]0%OzqJ7wGpTNahpHhvW8+Z2?F(:.3q[avG9I$5&jE-c",
    "6K!yk=%okS5.3fZb:Fx[{Mo:n^<4w%9<-TA*40/V5%PKXYwX=kJgZ5jyLq8?5<z)Jv&3ZXQqZ#-lj1ZE8Q/!wV*&Jco7i6FTc9!4",
    "TWtxqHYY1/L@6WF[(klz0V)um4}59{.<ptmT}Q]IqQ.<koihUrmOTZZu3Ih*O4[ISaz1([YN1oOAq:7PuyXbXpNMKxx1Mv2<}{6v",
    "eshZp0JDTzi3YSVV$]!c/4=a}@</OMCCpM&>3-^6Za}vQu<^gMoN1=BDPB2F8}I$<Sje=&[o.5@>#dw?pv[9ud-o?1Z1j<HueBH5",
    ")M4ddf-I:0&kh9Gg1$A$WyU#]w6-]ug*soIs6#p$p^1?yg&JgL&fVbX7/#ZebT6bW6v@+*j-[?fgsjic6(y+26(}IYhJ9eIMWBVO",
    "ywCw6T8KE>0Z6)O]-.9NR6?Fufjnt9Qj>l:Ov^p+(Fv4PSR3>q8dN?NfS.H(pqj#^ed3yiqA:tdg.osz*Va13Blg/-<6fEoB!ny)",
    "9By?vs/4cyvjuaU9}z+mP+#v{{04el!m/mN<LChD[<A*F{4Z}*zOJIwKc?1N>G%=6M5gKoHOkwB:.@H&)4ot4*02HtqM:a=(3Y9=",
    "%6(!Uk8Xge.qniQ&cpXM9r#}^0qirPO^*H&s>]l9+z3V]j6{EK0/oGjL/K6x2ot(?m#EmdRT+J!Q@5lTM8W0YOaz]U@h/GL4X#n4",
    "KFJe@(k3x-37:h(5C.FLR@&:#Q$K-&(E#z8)FGBqYVvlt)g#b7J.KyPa-v-L&NE$z1L<.P17=Qs)9ZD#@q=@l7t#8NWK4}qq4nJ0",
    "(<}l4XUh[+LxUo+{cpQJk<OOpKZw!^oc>B*<E1vUF8>aNq1{zwYZ@M{)O3!F5J6%fhTu{EWY%}/)9gK7)hV[NjVp^7Z!WD-))+R!",
    "@@NBeFiW+YO[BoKm(^td*UIOqQyNX{?D{vGVB?[QMif]C8.f*VylZ?ASrHa43LTk/KrLvMu4}ZW.?ZGrJ0a8-pVz4hhHyDE&Pp.p",
    "ba@}&MyqOMRp]6ob/+f]96RMW9A/40=%w&GTom#QDXs:IUNkG={q!F)-VuRz]c]Ed-gmZUIW*y:hV[GxnCcQU?G<Sms&/{x[dH$#",
    "&28qNmYR6N+joeUq:Z#b*^([drBox5H./x>0}$28*3Ru]Q(>Wu]Gh].y)(+o?/P#6>jS#jL(Qr!Q8s?jFuB1d@BW}2[g]<kuCdn>",
    "N7JTqPacQ<8q>t8:QB$dAxYq{6aDiI5):h7ck:b3U$:BBLKf6$rI)*IsMF%meud4gH?(g^mbSRVx!$pMTN#Xf{t%odaB:CRMH{uM",
    "lTAu#b?!kxin%9+y9y5I^xISA8j06vMzJ7{Z{49dyHs0N)Tt{m3LGqIP@2ul&l>Jq+a!<G6q&o6QYWLV3HrZoS74&yW&Wm:w@iGJ",
    "gUR@BJj8RF<^v:nE67(hcSX^iUvJOQ<*o+ac?.4l@w*fcO3X+KD7-RG/E@b@7+}k/vZFXvyBU1KjV+phHW#6wGJB{hZHn-U&7eTo",
    "?}O@g/ak)l4UuayFNzzu[l!t>l3Yfr&MLj:XxA[}>[CTiVsT?o=queS(3Ss<y#P]T(teI}fh6eA{$uv]2eF{ao{V2E59/xydCCB#",
    "w]=J/^<}Dk}h%q([gq8.o4?[hkEp!.e$xI=Sf/YY[%2YVWq}o?-}Qs-EaK@MNJRgnv6A=$^7B#}9)#x1F5qWUI2I9LPu#i6ZLxoZ",
    "I3m@:pM}>yApZO6Kx^Sa078NSw@=h<rI]1z!}eii>n^^t>aO(&KsONssM?OaeF^ka[I@3ZOtrWdZhcFbh>B!G1!Vn/l!COQ)F&be",
    "f1dGp}F6}jmM}3<nLweeKS&gqO.]1k*HhV$Y1PaEO?2yQ7^rZsP3cK+(DD!4L0Snm7.$xg4rnjd7v&?p}9GG[nD8TMt?T.wudoJL",
    "q-(kqx}y)fip7GCL@(-yytWA5mO#feW@3o4sqAUQi8}0kux>gEZpi4lFc-eqG99L^Q-%?yKmW#!)B1Frt(qmG}x%lCffraDC98U=",
    "gf(AAehnuXo[Or=WN@A>+/s6OW}pf75-W&A%gQ-YjBV3Fj!W%1s%L{*lSU*5*^)^B5#ICq0>hyF?PLjE7sN>0UK5g1{WLpF>jl?Z",
    "*<4!>JcErBO>sV1F}QA[h@>dR^MOFY6s?qUk6/^:(Ey}VDmIm}Yw*(z9H:#i0fh)#0y(flaJLFmV+j5^7]n*#]E)+*0zywAGQDeX",
    "sJ1%/Q#I0V/LdDJFczf0KwL5Ldx(31Y)A5VfT/9pp5?jShP:*/D*xL4*)x!eWNNWr7XO-XjdRJ%fd:]MHnx2R*/iHJmDwf%J9N%F",
    "]4j[}XpT9j*^MuGfE/4CU8a#0@Vvv*1d>jXD*Ncj{?yISnM=Oq&)hNcy4CioM)9](evZx%0o*pv>x@dIcVM-iB=!.#@-HzU)+l8L",
    "mexRwfU[hZ1q/VML>{$mik%jSYjEl>7f<o(AsghVmVapXsAJ<(2UZ1p]n:IF?+..j*(AR[w?6W>NZy9#(p-LrqE/}{N^+aa4dj=)",
    ")wWIbDeO24q5QMv{:]1GDh/Rj&2F(j0ub039D}uAB)6e}>yBF+[opM?k^#[uk1=Za{{iTS?z4R^OdzLYnwlffUUE3*DqCf!Odp<#",
    "{+Aq)aMLswik})XtPOwUvb0fA{<4q4:zBwWm:A*6u]y0U*H8u(sbLelJTQ.adQ=@6>Or?P{c:+K=xIqx04Wv#*{pSS??Guq>O!DN",
    "4+C2JZ^wam:lpA7xHzIR9Uvo]5Dy/PX>-<Ifwq-awuCw=^RpeyQ8aRfR1hml4vn]=mRsx*V0EKDtjg#CI%%@=z.vP{U[My=LdavN",
    "N6acp7ht]{dkAA*D+NRonXs#n{e8$%Z0ExQS]Ci+/NN6!x7F?.ye#[1CLv7Xh.FS8-BkPbP8y{/j(x:jv%wLGoo{Tk+qo[$e#yl+",
    ".dYf0T!NO:<5f9}Y$MOw&7sFyr9P%FZ0*jIv<zgw.GXqWTz-2vrm=-o<}$?D&i#B]Z(7ioy&pT:g^5^>nCa7r<vo.A]V!k^6FjEw",
    "-*gIRmHfhZGA+8-ZYw>@b+6=RZ?dM0G6Lwfewtb#d2{$+j?m{./k@r5ZCvJaLK<!$Ec>{/?xiPE]!rwV@UR}8bT{9AA9Q2/Ia*mI",
    ":4jAqNRGq{7=XMWPE=ns9XwUI1(BX<Ye)2SaFcqfV2<Y[xW{SR1xUsXC[()++?]9RL1Hi=-p*gzaP42eU6<6W2Lc86y3!a05/Tt8",
    "yjXDf1Z(EpxDEWE?Fa/fw!lQVFk8#QoxFKOQS@!L}X+nPm.J.7^7QC.@fZf>hZ^6qC$&R0y/TO3<r=E)5p(QJ2ga%o:NArBA4aX#",
    "[(32Z6Dz59^7{pb&o&RtW.s95Hkn{BaM/Z/CNJJAnm:(%05$@Wk?z[@FrxmZ?)7Y{dfUL?Ro{$Xl-zoz{BPc7{tyY?J$8YmnBV]0",
    "H-1=c4U}wOZ+9bV(sP-M-CAA4eWR:@hV)}ZmNP[-0cujz04]AS{$=?29jqw3dU{Tf!sttTL^$OzH{4l2kQyfT3j^=-7^HXR(xRPq",
    "Fde!E+*7QIm6ZO(stW&S{6:RUWZo:iy?z9Udg/T@s<&o[=>b9EKh(mOVEHbk{vfZ#SBhhVC>}=XgG6EJ)>6wWn/1enSIbXz1I#4<",
    ")BR#-2KFoJ7j%-yv/vvY]r?BVvZtjh5I#ja=kXHLD!L.*J[E5?4cYQ.w?9Tuu{*qB:Kod-*CZDzxM[CVB%>*5rst.sGyUr)4$)5e",
    "4Q$><uzG9+*iW>O@tt*OA>%a^PoN^qsEo1IapIjj>M3NRnEAWa>k)oJ5e#7(5o0W]?V{5yhc3kg756-4Zo@OSHV]jhdErG^d%.-v",
    "r5(J[bXH1sU6gz<v5EK0ww:M]a1L4scYzg)VHp*TRtM^@-HgH8?}L>e9^h.v2(4lgg^hQq6v=Xs4+%StxTKf&K7aG<0)9+)Z}utL",
    "xz=Hd:VTk#QZ:Gw3lRR#F2<[V4<$srFcCm459]7=^ZS(5qJ(QApO[%(/y6V)!/Q%b.tUuphPdO?2#?:9>A1gR8>?)EM1}?.gbp8*",
    "8QU0quFqww[PIQXKMT!IiW<U:5PkR4N3:nQxr#/%T9<z12!}AA5gVN2fV!x1^S41QoR7]wJA:aYdXXTYdYingKw?pPxPS]>9D>TJ",
    "Raf{!i@alsmiGW+:Fi)(ls^:wcMwbX)VPE>FCyMRa@0^l/9<b)lqU#>g:}0t91jnWQh:qG(%i:T927v}zm-JRC2/LCY4/D-J.>[M",
    "SIl604H*K]d5}CN4.$[8?O><CpgIQl=4Mr=[bK-E^]S864u3(x2j0f<qWK}^(MpEq@5U9D{a:SK)KT)xS=]KGgvHb:k^%@W:PhNV",
    "S%skubauA-RWV#$vcF9j:E-^MOb/9yTEj=9*]Dw%7Imu{sN<LEhc[&%m5EdIRF6US?E&PD-/&W4FxizR+6Yh.PAKL)/ba[fKAxUF",
    ")eBES}.9aR^62.#q:HZNPOvZ7Vz:GY1GP<j9bk:zp=Ew$cg(/5oQx[oe5U)Xs^x/pxbUqA<8}7Wy.<}4*wG?)TdsJ12*<}Y?r!/r",
    "yd09S3K>9uJAF>q/U5cqpk0BEhkhJ:&b*6Pd<V-hA{<^TYgKP?e%L&*[*WezmU1D4",
);

                const ENCODED_SMALL_FC64_M8_U2: &str = concat!(
    "jEbDH]sS{x)0&[o6M:O]]#)O5L)(.ofQyrN]T6Z&=c%IwP(u49L+CrzwSTdfO!wPE8LXDq]3w!#v<o*yL?ud3n9v6rKX8MnHeS]v",
    "LT/:NHBjpUl[S5.mneDGwG?#8>h{CwF@/%@!^hEye@01.^*iMG1)FPxWji]BEiAO[{IGB-k)aj4[mn&sky#l^2PT4ioNj1Uftess",
    "1w75RpkOc})$y<bN5BZZRjKD}4j)dzq6]12J<CZ$zxe.&lidGS:U1t))3HnR(gew#=<1[Zz^M<ml/aGH+=AlSQy1Q@TnkKN^w#]L",
    "4$gQ]?$NZCmuxe^/FR6&?l^IJ1?wqcj3dvyjr&4Y!sHwq^/2N})<A7/mBu(NVlaRgW(g>i0Q@p^Mq)Z[Oto4DAryH1vWPFAew}I3",
    "HRaXuJtEsiMcQhSl7OVIL+$cp26p%12Hm#:S=Gt@ta&7i>0YG/h#b^7AI}B3-662w[iXOoUOhM22W@j@SdJ5Mg(t6<!BLVkDz&j9",
    "9@Cd:pF><&Om{beT^K8<v?YRwl0xX<QSu/PRXR&/)^uvM-uz+8U{$]1T@He0H/:pkv<[d^1@k9r7RZerZGupPU&VUkqi#{Gs{h%X",
    "-777%5A<hefh.>=oS>}t!{xl/p5Q[Ku?nYWvinN={%Ltro$coG7?%%L(/>1O7OcKFEo36P8:ov@@yaK:82@[:&rZ2Uo{JVguGeBR",
    "&>VLDXH}^j}fZe{nzkYftv=>TOxpm.k{.ZSdPzhm+q<=d7j+kNbz5-OG>NRHGQ27>>o{A%jqD[<aDRt(o=U-Fn^L:ovwycwSd&{r",
    "Eo>!{Z234TME-o6kP6qUB-CI^mp[en:0zecECM9X9JpZ9N^IQ/-C=.V9H%J1}1*dpb1?ZBL<fkLuMXz[*xL}^e{6p<DtV{26uJ6]",
    "9G8mb=BfaN5S&l&G3a?w9CkQvnx@IB*[GXu<wSB$K)G3Wp@d-Ax8$-uUzkGV0K84bh6D^02)NcpZHgebV1pIWNiW%eKn{aX04(m8",
    "a:JApZZx?HG?r(e&iNmk+4e:W@tS!<uhLl}TYH0mVvd>GU#=$03Ee&sBU/C#znS9YK2cjq=^N9KJe12?Upbb*Xtk0BfR&N08z)k.",
    "r2HoJYVo(Z<?@JfAx-}R3nmQRmPE4k0k:e$:d}a%xLSO<f1NpXNb3Skuyn[#s/5q-(ph2(g&0ic1)YQg50cGDZFgGo+c[Mr:z3U%",
    "*fpz!KoIgrrzD^Z?iQamWXy8>NszF6^q3.niy9<jtvEJAooDnr89CL{p%^fAHeqJ@5ly)9]dfvA4DeOB&AI)psIDx1>l{=ww++dB",
    "6vdYi>eim[kDt?/[7Yzb2OAy^7s{Nus($)WB%yFeMcp>]izd]pS1VhATY#V9uo?r1)GJK3E^SpX5?Kp2JQCic3wks>eyFn@{O4^M",
    "RX-8<zuCs1d3[XZD]z6&?l%BGU.q*ng[bWMx(0dI.aM*MQG/0b[L4Q#EdXC)OKDNxwRp7FC}+]8+Wh[-{@Neiv?xel}#j5/Mud{D",
    "d6S[rtt}caES2GPX[GkH5Sy}DOZ)%bl@Rvydu04C.!YGuuRD{3!/=0C0w(NCckV2ig}m=[<%Dzctyy&ZG#PirdZlfu2T0vCF5%aL",
    "PLYYLj(Y:fF3B}}oCpL&M@Pqj<@0tC0W3jh7)l*viX@lGtW?uVSWFd^c@L=j>xZn[c+p=&F&/{A(Van+gt9d10i+VJSW]IX4r^cj",
    ")J#{pdo@&nwSI$!tI(:12pB-ewq[ppb8V<[+@2zZ5Chs5Nq&J%.-i1S=IIQ{sGsCC5RnDBmvyLDcQo5[L]a<hdayHBl5N[ublXQ>",
    "7WuT^x/%[UH}N1RF}$bP9r!q-s:?Zi1v5!D:8)q8Ut0JWXPj*+0GYQYgLYNaLd[:}7YeV{l2K!biVV>^QE)11v?G[F!v<?O)TcGE",
    "uq$-8mme4UC#(sRz[GgpeK2Yr{GI/6^w/mvcvwR5-oG?8>O&i?TzLhPf{6*D@}Kpw<RTaxD0)a8/zMBE3+VXS(QKl7yJaCWv$gWB",
    "pLqoJ4o9LCsRA5z}%?Xt!B$f2T}PK56CI#UFB)hi-H%p{Mq&ZO]}#p.dTjDDuR])#QWzwI:G94*l6%yug&Nj[7^=e7A{Ya7Hk7]9",
    "urb#F62lJv8om=JiK%WNfOmP-k<bL*U8*/unrfsQ6M&JK(eYVzA3gpR0-rn!l9bQIY<QlvQ^IyIPvitAl=OA7q$Vhl=tg59QTnIz",
    "3H>w+z.%bXdNtn(?z!rzryd9QI+$UgM%7/HG[HOZPI8M49z7%#-0/Kb*cge5&I(#xO9(O9U2iPU^-vha23NfNDome1SKAFNf^G<-",
    "QB?]$k$V!)u:hYa.+=f5tKh8F0@UH0[hLy2)!@D+/U0xXjm%A6VztZRvKM))[HW0&(o>rV/>pyIjflsYbm=K>azO.Po?vyW[>pC}",
    "vfZ12+jFDyQR%q@AESkE*M2w#M1sk+.%awr7ZkjG/m!dR*hE&(}7P3oBRC4]AXAA+U{}or[k)j:5JSlw1]Q?KiafiUDX&rF:Q<7/",
    "!P$))KeZ*CXYeZ76A]O)2#E/*ePhs!EWCBzxp[DLpT7TUYp:X^7%*.}n#Kd50NE(cqWe4v5[6(=vAT($/0g7BEyg-xWT>hpjO19p",
    "@?JyPl20I:ghKbaE%0wc2J@AP32nRoblmH49-+#$:/6:6eI!fQY$ypbC*REku2b28&n2A/TmjO2wy[c1GK49Xw&^#C+MjxHN]zKm",
    "48]iVdzl&!]BxJ!>LVgfChw(PYcx.S1M]%5!]?>zVm&^LAOMkXg^3#ob.zV2!@uaWVl#[t?Y}nD^Uj%tsw7}yrrUI0*8@^ED6pxO",
    "idWnR2m#OLEjf@=BaSz:%ju$Ry@iM)/Ksr-jSs:x47fGQM(WbeCmY2kh/d5d/kgZQ=yvD?BJZKA)XI3<VZkHxm<i<NDpy3g37(4E",
    "micGZt+#OQ(K7CuIwsaylcLlJttCNsk!C-jf!I#V<Zn.m*N>GdTPd.2C*@rEU4pV%g1>x}nG5.+VC8mf*=#gU2gVhQyx:NEui[#(",
    "z<[$:-^C}xQh=>&R<.=>A]-I0p0IYXaQ3(th+M}f}:f{qEh[D:rD%tj4$&JF*}Jht{2y3PnKTYmk{D.1ZI0OWp(Oml6Zcw%X<Mb6",
    "(51*<N5n(R@*%rSFle72:R1-<WF$#{w{+f7Gu?+8^!9M4AXd41Phmj82A(^l9H$.AOfj<}I[azM2GL7M}i]@1)c6IArESDh9jl%8",
    "lKnVmP#NXSOP.n2RWe6>Lsl1}muv+^qMRmUSkzU0U{1PWwRfM^DTnztC[(NRPWH$B3n(h7hVq<LhBJqkS}1fwh1mRew*sd+Avmep",
    "?8z8>NQZ0zh932@P9#sBR[l>dW@FH.Zl<Tr:G#z?@s&:8nqdhTg)9H9nOzi*PC).?w44ib8[Btd^?9Fir3.Pf4J6:Wc/6-{nSM?k",
    "-YTvpJC:ne4fJp!3)qgQ6U?-4<JW27p@nU:-q+JOqwlh}o.Xp]><OI:noW-<mnhb(X+mI^T]3Ec-saSqwaMPYel@AwhpyJ$*f?YW",
    "tWtoiDs=IdiAY}d%dzklqZn$s)MxWC[l@N+R<0EX)q&9qvocn?MvI7X-6F^:AOlO?Q>^aa4:Unu&FDTei>U$=m<=P}JrAj>Uxu?T",
    "GVq@.i>92ujKKIHvGs@(!cqm4(CQfz0uNom9(F7wlk-S-i-mYC[CnD-{CX$.0=KR)VO-c8.14/CgA*8raU%wq.oY[UYS[0ubh{US",
    "sf+*?{Zdhs.j/:G5g%V<!.O58bInCF)n^dGf0RVAoop./![*76YH69vw{t:!/s!yJq&u*LuM/!tZftXxIOy=b<>5Xdy{LAJCjWpF",
    "2np*+%7{aDLvzqt+2a=]WAjE)Ul4J-z{=IpU[P^yTs{V/4xl>H4V^>RvK#ETuN$.g/[e.8R!Y!Q@AfSz?F5VDI.yj9<TOwn!epqf",
    "bfD$0C+mso<yY#9KUJN[[iN<L1(=b+-):op1){a*)%6^M^1d3nEDOzURT^o<ptv}ATM5JUsq%umyPKc>x5eb5W&{#0&6Mx^&60jp",
    "wOa-L0EBM8kSc)10uPlzD#}8aQG^8#@C&AiUz-+0Es-=VE7FA2]hpl8+hz<6Ff*EzBv=5q)ukZS?xPx?0zr<n(5=@GC48YgXItW0",
    "rCI3oT$:[eeKcQ9+Iz:5@^-86M)i9Pc{VuBDz[!Bo/v5Ps!(8j>1EG[QE#m+vG1P!YU%GR^prtx(hkLmH#Cx$Y3SsF{QH)H{gcO8",
    "Wp1}SqWpA8G]:G>v44r:4}VwpwU2XY-r}sjhc<MJE:]#kyZ]U)nR13d0LgMX}eWomF}pG}nt4y}HCB[5/H[}4??XW6Mza5^0R}*v",
    "NMLn}FY:m<vGg2OGzLhMc=3V::0ms(.4bi.6)A/{urYk0])[?YdxCwt@]P(!u@:S#gR:ab!C>d+{v^vQFEVLg6QI4#xwO]!<tyKk",
    "3/dEs}:]3}Te6w>jC{^l^)D.>iNO?WJH>?^HC%?0R[<Gt8OEpT>oz*@q?H(f0C1.+*n[mqQ?4MVxYRru*al5-0nQO^(y2t#D-kYj",
    ":GX/b-JU&A1i))V07cd/uTzUN2e@a8gHFhoxn[%=hHgm{^(R==m@7-g+U6X7O{?+BL>&Y?+9J8>oNCm4NS-8LfZ2S5Tajl:n@a/S",
    "OthSr>^Rc^Ix13cH}wQeHfH7Vx=)Ny1.^/%VW<{DG*k=8[Iq&h305/z[lSujt6Mgy^9mV=I/vH=UH?Ny6YSf7l8[RiqHSds1%eia",
    "DUF)hu[?+$r(q^qDh[n+UAR+{%aR{=zXF.P8N>.?@W/JXbp.+pR6ejtxotc!}lCQ+?IV}Fs2J4KxS[c]Rl})s(^-41J1)[U&()@S",
    "evYr8P*hhZy-2M66)Y5SivR2xm>GIep<omVU*w>5Z*meT[JG4Fnz[l}<6Es26N2kAn5YIZQ@c(VVMLV0oQ0QY/]Z/$:F]xSG$f=r",
    "&.=kVqmH2DhBd2I+&^$Go!hC<oAT:%F@G9fxK)mq.z237:-@C2fKibvzIj?0svXMrFD}2{O@zNu5-Jk)=ZKmlC(^izpSO<nXHSPK",
    "Xxqp]}k3D<npn#KnI#.}YV@Y7:N>}mM#47iYj/Ta]@kEl]S7nvagQ<N[>fOd}y/$6sOKO-A2]JMeoe3W38C:c@8p%89A^wB1iciY",
    "ipA9B@$iX)uW4ExJ)R:ToxBHkP$9Ra^.l%tl6^P}ft+<{Q)M0.d&GRB3uATtL}P*lQOj.LML#LY7BxN8s$-=pd!OcL.%<->LXMQH",
    "9gb>L2UvoBfSr^u[wjk1d2mSi{+i-4AhoJ%J&^-z>Y/q9UP34{avC$qAr0*sEHvTt:N@AAv7iZg<DA9Ff-7%=&a+tql?JNQ6<S-i",
    "r*!.a-}s7fTNpZ8p$HjX)c3@6yNN<W1/hR>5fKxO25Ll(XT<i[K?c^&6y]+E7RUKo5gxPO!h*yuPS%>EjGPbTs0O{4}ZFm3z+r$)",
    ":hCUP(r5{d}J84Er41}k6IJR-3mG<QIXht?9G2zgLl-8vpd}g(R*5}uU+^z]wJ>pKHi%]N6Kcj6sP<HxxOSR3[j>vB6mf[%/kx6J",
    "a/5<pdd)!U9Ru2q]Usblz>L#JFd-zSM[NI-B!JUog0u92PuZO6tI?.gOHZ%ey{)Oj&wp2qqxc^[VA^!2lHnILHoK6fPJ2QshKvMg",
    "1hPY9?@El<t-g3{KIMvVnsd-rXg6^cv5GbHc^K$u<{Wl-TJLVEmQ6<@q5+=-T:l8ydu^uqxIcpTWX3[<M-k<7Ty2N5mhvO>r+s0G",
    "H)??/A98trygPm^6XipG0ZdC=MPN(5[E/+55y6b}6c)#IPQ}%)P}B0IXP^c@I9:RB65*$%Q#9dRe)&b5=r+d%06/*k9oYHG8ir7R",
    "MH[E@6b0X%9CrkKZI)t&7cw9KfbLkl&ubf:Y66%y3:cV}QR>%50FVftB[c=>[Ey)H/egf6LQFNRTLFE(jT-6HMR&@gcuYNb.X@$E",
    "m9dc%F!Yp?4fKZn)3mVo9NpE{@%(rF45U:wV&FaVUk!X*oVoE+hBx}%US)}cC5Sg1^heck]t}IbA76Z?*1V?DN!r5Xha<e#Qvx<1",
    "s$g^YZ9>}O2+k&%idJ[J@5a!]S74SqTppJfiX*&Lk!8AUISkHpS>R6r[gkD>}T(zN0:5Tu@j!Yl>OTy+D:5n*S!!B7LOwd3ra4C{",
    "8TG^OQCsHpD+jF=I?J0i9-C9-PrHywczWm$aBKkNi*Hb!:F*sD<@)<t>:C!>V.d@B-LKjv[i#rHN8HmkI>}(o5.nf<<%lwd0w/[L",
    "VhT^pEG%ZNz57X^Pn9OlN3P0XB2NTzp.c^AE*OOcRU]U(*2AyNT>C>J)2&eG)g@vedW<c%=}i@A.I[dJ=jzwm8F)OXKrD8qimtKO",
    "a-^!3+*NMBav%yjq?t]KFZ%Ca&#!{$M95oy0iSchp3I>}sZhj@<yh=oO*3oXSV^-(V8e9$YYQ6zFGmlA4Dm@jBb(x@)l.t@1B?4E",
    "-K?8yNk{qw^wQM?5aSi7NT8t5@mViQ=/2ezdqYnLCR5<cxKiOYfz=>Q-hc}S+GE=o=d[<NX9{Gjr*9FCn)TY}?vdW[>.J?mn0<v>",
    "E0iPB)9th=LDvzufJI:GSvktwx*x^77/0@x>B).@Jyuo0Vn#$?17sNh!ZUeySfUE!no?}-9[MfhYJ(F&FtN[iEjgn(gveqYt15k2",
    "G?4E(uHAnaXC}1wAKCZbwlR$@hd@Hzj:R-AwpNuqlk%8(r9G1eDuC]BAuh<c-H4@opBQ3xWDiidmW/h#g+Cs.R+c}KPG*NYDBA%&",
    "ZZSusrDeQm-8)Ek]8Fc>ZZYbjH8qH8tJ5jr)46VW5}1i7lK3J7QS}&k-!PC=Jx.kyO27P#0h{*D>PZRLCZHTFTXH)?Zm#::E&<!p",
    ">R!:ux%pWLRmJ+wb}s74vyh^)@x#/ipsPzfQuL3elbc^vQSTT>vXc<?6R@b#byFid[EWp=ac>S{urm!7>U2$X5ANxng452PMmto^",
    "Ci+:jUIU)l3!nowWibDkra/vF37#4EjN=Z(:3n-HD#rsQ@^M99G83C2a<L+PZ:O]&-4e<v{jN!bHhRgFM}OhT]E-ODy[Xm1^aG:f",
    "<A@<Hv70$!/WN8/azMl^&?lZ[PBpb105^Fjy:2*(iiCIf4R)YhjE^Rfq6fBmppTd.<IqzwxCum^LiA<#}{+XWHVV4T)5g)/!HKwd",
    "Vnr37T>ecq*$4DF2v$LiAocb]QCuECbNMSE==+K=I#.i/udL5#AOh8v&A42j>o]WWIs2V7l[[zH1]W^Iqa2N(ny[UvgzsDo.YJ:e",
    "CMI{0!<krW=B(h3><.58&qOICj&f4&(]Vr6x)Mw+@%>hQz*1(tu{7E5pnqkYAn+rvVyyW5NP$.l&1GM-?Clo#iun5Z1@qK0R)G}$",
    "@vw)K*=e[fU<]6jGI4W%cK?m.7F?0yUXNJd3F6[[y{O]g?%TMdiq/tYhY{[&7j6^XQqd)nZMis$5Vc5L5Giysv=*F2+nf#/A6uR5",
    "^Rq=B4n7C2nAg^QO/f{UK@aG?Fh7}9sX%:+ojYbcO}}N7F{u/GtK=X@)B=i^CAah*1r:wK=K{#bj9a(y}AYY%ReplpC32+G/ieLC",
    "drNR:<ZEZy]wHL1J%?4kmTN$BnAHjx2df-g[cH+Zdj[06>(vIy[E@%R<(yUIBBX6*z&EyM(>1YXb!W*7D*CT::!*t$((@ZGqE8jA",
    "f.lf}U^Bi*v37yJQl4=b5wfp50WxD:J:3#%n{<h]m34qWZ.Y!X>/A6MM#}TS/HNRT2OkF0JdYJ>b&(%d:Cd7nmVY.}{!&a}Tw!^P",
    "9Y<hiN4LxZ(J!m&KB5op&.k6u>j&)UCN(@2l^VyT3Zh:F>UDApGb>ul)SpRaU>+Mq.2XmkiZ[]?rxVs8u?ods2)}/?Dj(7F]CPnz",
    "lc++e+n0mQDlC^xf9NMNY<hEsyxQcutyDN!Zk[dnJ1i6PlH$I$)mYgD:3WOO=CukrLOt%*h@CYu/}b@w<M<in)g0BpT4SWXs.(!Q",
    "v8cIv454yHP*{W$LUk%Vyt/<<VnwWEAlRs}@4J7N&aW!SeAk%d68B4v>un@(H$rt8K}.a:AV@/y56VYtgvDT8u{G{*3s*w7k8/GO",
    "U[196k9?zDEh$NpPOusT9bIohLNIg*H6]9q]eOP>L[PADHvw249c]rjAk(oHUq7yrgcL.5[i7}T@LjQX6<c5J>JD+Blk+XAD7[Lc",
    "FqnecUcoDv5E#Ms]AR6M9JalFzQ?JC8Y5&A[b.SlMoE@9vQ<.#=c$-p.sc?Rfx<g-0cyAs0l7D(GPt#zoK?o<vN?&XV[IG)B)MJH",
    "lEn:%OSH7*+eQetxbDg-FiL^5A6ENuGsSM%8>UA*E53zGr<z?wSaB.%2+>9TPGYc:02Id*BFrrW^2!w)yZx&1-p[#0ApCc^={U92",
    "@m#If0&Y!C{8+ttt*}1Z5jym@cZ?JVvzUF$nce!99^]YQ-pRJ(Eb:k2?AJ[.5aCrq^E3UvgPE0feXJpvwMz<CAcPaffRr5xfR0nH",
    "d]nWG)W^K[07n/Vu[]QkU)>b9H!kX3y)nM@4#nxxM7A-zj86U!}RqH<d1(SPK7{BlPMLfp^li5{l]e#0SZxYGJvRoFD1HtC09OIE",
    "5m.1?&e<*067YLCKR3.u.@C@8neGPnI)LJdw+N/vro?6d*(rakk<LqV0ehySAMb4y*%%]/mz/UHH6DLDWe8C?Ab[pU:xj9V(6(oG",
    "cwnkemvnXvw1(7Gw})6APl7e#/FF]c@qYZegHacF[vpnp]cJAWaMr2RRWn?PjNd$&KFxR7v7(rE.$If+3N!JVWiM%ehf/9-5/j6L",
    "7N9)){@V{Sw%t%57R*ww^P3Sr+<o7dFzds)22!jf[k$)!aC?>!8BtjNKv&Y&rYU6hpddMn-PfMM5+spiTjHB!}M1r]r3/-3fA<Q5",
    "!h?oSI$iJ7:]rny7PeVBi+JB@THmdMu%iVCS[ECo37:%SZbLmq!}F)[]@q]y7.w<A08T!3k/=UZt+QY3A2BOD)Pg{M{*}W.@0GC!",
    "aG+yK[)aR%^O9AIx./w9Z#5[v9bF8d3x$!ffYSWaHqaP&*$UjW&5RQ=u#K(lXxr^zC@:S)k5*}zfduk3TCd&X[z(dm+%<$jfN-k#",
    "sLCpsOAK=Mc]Lx}D7WBpNzhlQU:9IE{fdOZHC$2K+[7:R?/q@nS!&yy/SKs0C0IIfF/3%vbjdOcj&-WGudCo9O^m-#gdE{2:2gj/",
    "1v4h}g+h6WoT03X06B8>j<5yvo]*Nl&?t{aNf^%#Z>ML!b-nlTPCwAVWHNq<NIzFViY/sn[{z(GT6Y)hZ6C-y:N<}{{(VAbtUMVY",
    "OW7w(7t66V7%=kC:Yf6t@j>Oi6V!}uhKf(Dp<)TLB.x2-*ICz6Y^VaUvNGluxOuw#k=+)qULLm.1*U3&who7Td+YwopXZ}MI(0:z",
    "=itr6hZpmBx*QK!!+$[1GP.(&(Sw8sIp:<JocpjFGL3Ypxt{h#kzf22AG-:NXhWkE",
);

                const ENCODED_SMALL_FC64_M8_U3: &str = concat!(
    ":9AcAH(S/A2d*zt*wBb%VoT^uo%>6^b*>:q1E6=^>2>.G/T1pnF8P>eo5RqkGc>MJR6xXqJPV5f!k#]q9aEFZi[Ucdl:TeicyN0e",
    "a%RR3Ikh-n>&Nl97B!MhR<^-JNub$kAgeapIV^@ZZzg4*<sh9j>uK1WPNh8J>u5@)X]$f*EUT7H4sx$CcFYU6aeGuea/>TP!Oik<",
    "c9ZmWlTNR(lWXL&:kK2gsUnX%{E3(4[MNGS@.+$]GoX[?pTlBjYnqPZzCU{F[t5liM:<^[Ylcz5y0@9RmFxLuw{v&I884.$QvskS",
    "uo@tsD2}PKZko=(Rshz/jqcvhE39B3b6wS6cIg#+j0I{jVZ6SnlR^>Ge5g$]xxxdRNRud)RU{=:0y.Y9&CTN^&C%Gjj#aoAut)tk",
    "4>ZKsU)L#[OE:2UU-qamcdpEfZ#M7>7#h)s5b5(lkvg6D!}xWdal)yGEUTZZ<9H%y-rI2VRNDT>^k=hf=s9>shRP#HrFWM+PxEre",
    "56AfXNcCnDU<*hRU5jt9KZ^4T4&bNqGfnAZVy0NlFnCSS%5g78Fw4[{dha?S8i+>0mgsA6w1C!BFC6{B06Qy@*5-Ci9:ssY)^Y!V",
    ":-DSdg}yN*i^+Q7peN6nmGh8QtjKsg-&F5YeX4j-=c?.pP+Qi.Ey/nqm[Q3Gu+6)-TSyGQ+h]}uLxG)quzM^EjFwT=n!D+t[F+YC",
    "bNZRmG9q?1weG+U+B0v:VMXwxwF*y?LM)![nAz8f>R4-PB#V@]M)mN4h>IxV6OAu$EG[YyI5%:KISGA4!s@G%3uTxL[%E!qtTL8B",
    "At[{xfr-a6{r]P%9B}ZP5s/#riYio/-CFbU8%gV[?vxj(deld)t%Hhee<QbYGXce=ar&bCpUDHDs[Jd^:!7D83Bl?E-8/jqUVXU4",
    "F(N)}kpJ^0M%qyfsF2jG-bp5/.jvL/mumNm3C]$gqRW0<a[=mc9T}}WawfjfvITLSf=wGNNUok7]%+vyV0S*gnQlSmB&TG[F].Hp",
    "g=f!EF&![72OKv8pTJ{<pYJw^-DSfjO6@a^F}w9T8VT[vF{e.d:eGnaTmlCcgVkD1xP)%eA#)h33JpEE-XQdH&W#xqD8ZK3R%p&R",
    "W<!dpP}S.{YJ6IDqbut!t!(xk9ym6BNH3k.n3NYVN2C#Fo]>J3XxN1DejZC/}2Av!H72AxLc#&6MWmt$08I%]8:%8aB8H54fm^W%",
    "lX}9)}D0nLx:(h#zp$su&-&9tk5NEn3GUbF8[Eia0.V0DC]TZ+d0DimByumP*%{Wlf[>rBIHKu{!r0&#p2FVdFnaeU7G?Iq{z>i!",
    "V3=!&5!f5#p55UuPBw0ptpT!UJpwO+GmL2QXKHB0/6nkTK?zz(FI/Q79myv<B6M:EqvdUjkrU{S)rd{hib&H[kz[vh/KLvMxkwXN",
    "f7[A&<]!O@3WI@?G*SnXV0%7-M%Bq!qi>]V:AZ3lG1us[*2h[y!+En1vka1Yf318T*0mn]w.?mr)be*D}YP^*7f@@m*IaU<2Xhh]",
    "d3WH&fm]xKs1w:lP(RzuNDUkA7Ir}mvFCX>c2Uv#xI2*0ZgZ0Iu]]n&RPUaobzbl{A4Ve{5q:h@r6:?NAb^^1neGgSe?4n]^G#Tj",
    "*AAVi6z!-xABBTo?h^(mDymKwBh!i[7f4rE?[ZXN8^Q-LuhdR!7Vgxsu}cICVyZ?N4Wk{n[t]tFTddN3[&PvMk8{dp*[0:)cuN6^",
    "]@W4j1i}4Zz7zFAUzc-6N2GMTP[LIu3r&=VvqBaJJuPZ.ZV%38UIs&CF%u%iTLveC1[D#N+0M(QMQSaiuINeRPDH^dO{Ytcw0TX*",
    ")Yxp0}u?)sgB)(pFu=F}n[52/*he:Z7QQnblGB?64/d9T*YnLg77>e)hB/+>e=cbyY=8Zv@r4ePv+<I/.lT!sU@m}iP/y38VS[?v",
    "aS#>g:r)E(8u[ILat]O2w+M*1OkQPzPzP(@4WUXU9bRpXDN=}D!-kC#m$I7hBI2sK=LH4NW250JnQ$qm8<oRKxcPgBgd)Ls&Cy+w",
    "iAm>0![H>>&yg*JPD>TxvR)NaYJO4y=[3?kX@n11OYpApD90yDuh{3IY@67r*R%&tM}Re9s{K@rWYd8a<(W:Ir0$ov(w[#Vuvt+^",
    "KD+<tL62?H-EjL{)2mTKn?rGD50hTY33EdHwQRwclckh3?yI{Chyw[@sKyqo0)g:itsT*?KUuKzM/*cr5&yv!Yv(X=PSE3l@:/AH",
    "f/l2C3!Za{*a]Irw@#O@(%!s8uETJ&Efo}(9iS!:=}D48Iz<PUV653FaE5]?K8EU50oj9)%6!pb7*/x/a&Hd!v^kKeJ!4.P0}9Ig",
    "NA5k?g5pb%v7JbD^cW*BjEw@cJQsRCI]Lt1bX&i048$0mcm2]UEJ@U<*3-))<$8t4:F6Lp1l23HoZCx+i*(E3lq$0H:f8RWHoO@^",
    "lM^{zSl0<VUjf7!ss0az5o>4=-#}9t3:ZkOC!azFuM701D{:Tw[S>v.@0TdJGwP$FY7U[XEN2l[5]9ooVu#wW1=nQf>Hity+.KXh",
    "E#)5ZMC6uj50g9Vr4<3t5l)1liWM)JDYRI@5<cMo*&+J/PDVfOa(rz^n8(6-DFW5.WnA6$d%Obb5eE1lA(Lk]!KM*PEqp8<0F*=&",
    "E.%&5<1)grNQ]jbgXc7f{c.3R>8yU1O)pM]h<<-XJJY/Nj2Rq@D5!JgJaA%wAEjg*hI*$qi%(zflj=8?!%LCVzQyeJG0[ik}&3d<",
    "TS0bg2^o1CgU4=6zD3u]BRAZ%]ouKxOZ*d>I:.i>4%I{8YJA-F2K}k)aF+so9PhdP9x%A+ua9/#Xc[tg[&kjjyMQ9XH!pSfXO-cC",
    "QKU5hYsCZtx-<14Qr:cf(*(O&1$<2(bn=4@/MAm?n0kF*j3]A1:cflg<ZuC0*re}H4%a8K{Gb?T64-)ycS*Tqg0MBPp{.331oYr0",
    "K]$3.:oEQp2><4=MfI7aVF&eNXv5GLU[9?C{!-cCOtzleijiec{n[uUz)f)q20qJvN[!F@q0$.N(Lhg/tNUQlO>$#</qT?y{e?^o",
    "*f$sJx0vreTTdq!Vl<&WyRZSE6ELN$O:vdP&<*}XH{(0Hp}]N+ojFI%x/D47??X^8>XCYiuOv:eufOMaFvr3KoRk[ea)Q[}eg(Yw",
    "Ljs#7n9K<7[EGp%XcKVI<RChoA&$fXJ8<#iAPzR.&Q%upyDYl(vW^oU40k-q0e7ql@W:0<RlDgEJv0w[BDdJU!HGZ}MoK+3RvI-L",
    "mkMtPR[9>Z%b@QegnZCJt!4Eg@RoWg4RI#UD}?F>FQdU!Gw}D]uIS!Dg{nI]3[x<<v4oP$1:Dy$8q}p69qW6aT2me}WZ@I5O#%1@",
    "}t4XpLl=s{5pQQ<>G7JNS1hS^c[Ag#EUX/MpH*s^s30iw!fMF7i4]u!5Q5(w^Kdn>u?7qhF[D:I-AQN02XOR0ni&?hgdK[#!1+d0",
    "mnyn7Y$/Ap}LBDX0j3YeBC=2t-1>bwQf8d)rl)Ip1n?QKthTee6/$o^>m]R.5N}[apPrqo>&s.nSgLrF:YcOHSA).h[.Rc}[Lb9e",
    "dv0T>13Pw>WXYuOvoIFLaf{X-ZIgf&{gM$f-eGjey?+T!QIgZZtd)f:jBlwbDO*Hv!iZo2^@@e.Du[Zqb.(pY5I+B(EAh7Bn@=rM",
    "!8GdCcSybV<6eSFWooMz6y{%)*iFW>{H*=]vsi8UvgYF5BC<Uw7^S13<KrGV0cah=}((Q7]oRW8qC>]PPA6!Y=-l-M8LV^=mJa?f",
    "oD%o[QW*KWmdIiTbf+%cAy(@?0.2z/fX8ZS8w:{q+ido/zDLx3lbOQzZ$wxAHJ*J{3iYcEy5kVUY(b?)vj<]h(9R6^D0vI^auBqK",
    "Ev=M:?v!3}g*j-VIwzU[<xP[(=r^keC-T#2f^U+>OvQ7iBxRi#^SkKN32$QgVdSf>V6)X^&IA8dK6pp3pL97t5MV>l5.MA.4#F&I",
    "]>.]p6O}vOs1}$lI<asSFj{Fq)u?v{u-M:U@kY49&uMc2Lu$SfWPD$K-3g74Fu[u3?1<KN@PH@bXczB=aroJb]A+vTrBz^?2)G4@",
    "mPVI1(UO^Gi>)yju5nOC=&OnJK<honL$RYL[O#4K2z92&0a%Y[R0:$?e7i/E5sWz%ZRTY$:S?&<^7U*UVE5Ch[5$q=>3T(CU8tGR",
    "M5LKJKaJYYZsF65Q:8RDNaxhr8X[=7qPm0A@f<@Ndt+S0Aoh76[VWeI9Cv]b6O=!7Cr.Idv-$BT.VL7}=TO=A!c%Kju[4>:2:)iR",
    "cS@n&{m*wao(2W2ZiyXZZXbsOj7yxoMJ05Xt0No{V11qd@}SAO-su!0{lM-h&*in2XWXU4W%<4y8AaRez8q}&tbx=.?$Qkm<35q.",
    "wYZ7Eze[z}BHOu+fQ#:*CT9=yc7d#J{++h#MvRv=!ZQfMv4!h%WM6EkM-<NQ4VmwHWJT9Z1VUTcJ8nrwUM[gwG>@uO[@EuAq(FUr",
    "4tg}04lS&qK0kpKQ3rf4M&b#e=^vk9DD&lG6UeVQbALsb.?Vk{Agq[J=v.DE7GbcmzuH59{/$zA{7S(U5P2m@*3Js.cd/B=k7&vp",
    "lc8LV5S8-nP(QvHD<5VD.@Ou<5*r1ya6io(@]8nq@iK[=-o(3=FLzcp[Cm^Ja(S8Sf*2I1RWmPh:4UFPo}{pN53$fb/P%v!>T^o/",
    "+N7:w^kL-4fU>y:cY@aaHo#1:&7<>E!^F3jb3SvaN:z}=>pu}K)42Gphq^DcPoRJ5K*>JT<@s3c.Q[$7{v1J-n5Xg+h3gLa4RA$#",
    "*vEr*/m7R2!!HfTrt{d@!qiPSQMo=m&8=BhhlKbC<oXZEC+?)-<mO<7llMPEMB)U<=g}(l+49a/UY:!B*f=u)HFA)]0MnJ.T)OS)",
    "8h[+-3G}8]x]^5y0Q(^CdL[X0+heh>{Faz{f%=}NubmZGi@4doy!9x@*h#4g77.!9PJOOlDKiRkvN4:[.GOQn1AbA@B9jiuM=3FH",
    "eF={J[>#N5Ji<.uIJ}+=.>+q+C&jrt&[jBFr0]]]Zz7VJ)Z-On(1F?+[A}<vr:DAK9e<zKC${#AkspZHKFj-#z($MM!pevHhCAqE",
    "E.iqarKc:fWzTMdn{}Es!oyTIj>H1N5%*LJYrcuwqG<1)VVt+5KSKUsGo1P$sJb}RmDoVDrtgw9}##M5wW{*m)&YGVFG83X@r9A<",
    "]5P:j:NKml.#:zZxbd)u.bP*n{<r<>F^(#?-M+bubh>yyAm/L4-Gs=AhXrse9kGcHbMOH>JjXEuDYZANo({U7SJW41GOc9:l)1AM",
    "d6(o8)8f:P?lQm#Ogfi9h&OXsPfn&hO[wICa0!IkoM)hB?JxiIe-=gW/H{!eIlFwvE/n&JVRc4!:].-fwFGc$CW!>d<@y6UVhG?G",
    "brm6:ag@gItInd3[:t[Xt./^LzB6&c?e49}oHzJr.u>VOOVHtNBNW!>xulG8lK]>P0%S6}UZigXBA9bIC[A?oXBLqpd}hBv()?A!",
    "6N)Na?/a=yys)Tv><LJmEHgRucTcbPpUyJo8ZwoAoL?zRm*eMHn?7Vw!jU(!<n/a>Cb^4btIwNeh#0hk!1{ef^[C>p<W>6roJSA3",
    "!$&KkVP:+&S-KZm}p*Uq(>mi07n}IMAkdKF)RlNLp&!4#V&M/jCWJhrFAPCnfP:w*&dAHxXz6/)PSThnZ<wX!A0gCHe{F{hDor6e",
    "(q&by=Lcb/=*^k-4e7jCy!YC)WwXq-GzTj.vxg9RxZzJTo)Kp-:PEnW^y75Vv03oIvoONL+n:{7PzBEfG&IhriFV-Wy}uKTnvSAG",
    "H.xP#>S)CbQRYL=&r$8#S})pv=*7ts)h5bM(:*6f4T<hFL$3Lya{nAu}ywnMf*}LAA.}AM)Vjqg3!smHAiJGVIPT!?59JHDE4o%}",
    "mBUeF*][@o<55%[lk<]es.Zmno6fXG-*@[*<t6T$3fi$TBrU}yGwRH0k#X:^:ydML[v.NXMP{G[?#m]nE4KOa+0GJ2oY@Gu6)r(%",
    "Q[ZU@34{WtM7Ee4BFNIJ-%Bc9NcE>*vAD+TWGAS}PGsX*QM8oq2VZxcCRbz#Rp$=f*uPG*oE#8+3BVXx[>:.E>L6i+W)q5$C<p>:",
    "9]c}jjv$C{frJTnZ3Wymr#Xh7&EF6]Tcy&wHZ.XB>&Svq&MBK39Id<Da9a{8An(pfXjEjB{hCTFCwq:]qLLtFJ3M]^(f1*9uR%MZ",
    "=EvY-848&%C^yTrbEADA&?Xy]t^@z2fY7M$LP:%dBTqe]VO%B0N!U/6Kp!YW0Wql<xRo7MUXGljaq5n0bOPu74T-9bZ}-J7/T&MB",
    "oB=FmJeMVdC?Vul:H==jn)zwgt9*2skfNp@6Q^aS=wDc@hYWGl<(6sOq[EN[i0Tj.DqxU!PmHxlE<>**s7^EQ@fibNlELu^.vR$@",
    "CEF[.ftHC#Vp<syi3dO4OC]O3iUzq6]C@@Qd-NPmTIkB.8Kk!oRo9Wu?cg]ml.Jf$B1Wp5E>>!5GCwt/-9tyrQuv[cJtDU15o9)#",
    "aK90Z0muwi8p@]x{spR9jcQ-03f-i?a!F?Iq8./BsK@{7}5)df%0Fz(W+S7uW^%zX7XgCqD(*eq>cESyaV2DGw/INH9BWj8ic2.E",
    "*{O(I)X}A{p.!I{C4]vq>}1v0.6R35nii/+9#twIRQE0d46<}qJcVWg@.U)zN:6zCz%BBqsHbPN>&Qvd=@Gc?aUhXqPpD?kHo:4T",
    "qX(NI*2q=&8x.43.YtKp=Df]!wu942++oRLZB]=8E^hW3T&j:jG/u#+j/c]EfN1!Q-ZpoJ/Eg%ScA=#6+-h&&TlDxy3lHC8@:n+<",
    "eqKUEq=F@z7J-FqAIcWM7v-9M7=OlO5w-}i&KorifHboz0])]Pr)IsvEhB/Pbdy]yowv0vRg*Xr//VYbLYzHw&^R?eP-Yl$(1/v[",
    "2nskNY0b3}9TKjdu-Mf29g*iBh36U*B)7Rz[xA*N9(}3?ty0>B9o%%}c[H3nd.X!AaS:VFKL$gynekpv.2$7TOFLD[9CcCd[#-bg",
    "&lQh(x9O&d)WgKFe6Al5oR=F*vLa(H=)0P@u$MM9>YOZAdzfgPaaN3FC7:nq&GWuPF]EqA^1zwjfZ[REH6hCey<?Egi!@kHE2jYk",
    "6mBi3cw4EA99LY[D^7>u0h%G)8mkF?%cnQdJO1X7M3W+&jhU#2r&n<fj5Lla2e:iIi*Z7m5-c)e}Tk<%1$bqwL6%atFisu9Ca@S-",
    "X{loHa<S]%Vq^hOBhYE&Q]gN.dt/<]L:CDlm!n*TPWnb3EGUem:+jE[RTq8Xl}mD}B]U^YSwvjiA=zn%fK@*1AMwT7dUi5.!ia.:",
    ">7at.Gz9ROPHd.M9PbkTPFqV]YneCWzYbcfovk<=Q>@S}7kZc@Y9GE<b8ulkM(eNYMPV9FD1a-QFV#QD}7(LCp2RWbp&hsUi{BND",
    "nzV^09=o3kZmePCGz!SX}=iE8H(uBt><yC{8c6nwsKy6YSg(2Wo3JBgT6OicS9:FS*=#gW*-Z(8{cFF!BO^ay27$n=NhZN/06(vL",
    ":F}XyAU6@?<g<fU{WJdn&dypc@r{n2Z[e?mqe>>+=V&!XCC[C7R&iI$aB*MeH[V=v0o2a+>>ZER)kcjPOCt:<!omixh%j[!-?oxC",
    "Hp4NRW*Sy6Qp#&9(z}}r-d[]rFkpwJx%pS4W@rz5A4#NJm!Nn8ZtIWG.9}$H5cCbf!}*s/szA2[E0:5HSoPLVwHH=%Tpc]003Wxm",
    "E=LgQ(WCV]p<u9Q}}9mgZ}/4]-[cbf?Mk<5b4]uq+iB:y&sIOAR2hyHhu.2-v6@Is0gs0ONhIOV.xL+*e@Xz}}=h=w8K?IQ?}xV@",
    "XvR1tp?$.F:oQkd.q&wzXw#R58skp9T!@(%0bd#*XpiONNy[UCwBV>pS%AHPI*Fw^f/lB7v#=FYhbL13d8<4^qb8K6YUj-=&/Ek9",
    "ei+/4qYX-FLyb5FjaJ[+dXfnE}EGojj<tDu!OOK85f=x-Gh7R]K:P}/{-tR.!1qf6V4A3t}VqO^B-Ruj[FdSVy-8DJhHk>E]8%/X",
    "Y9(Y)n3i7XujXwCY0!C0Iw@iLAY1y(k(.85=3*{42/}#?CE{*rtB.L7Mr$Q}cZ27QJLCk4v(cD+4%WiHZ&bvF9ki{!k7e!ZO28rj",
    "WHC*xHZrZ/U^YlE0fqYz&n2fZ5z3XvFjl}gZkm4$wok-65lxBR7+$hpn]j%Ir1Q1Z1xoDRxU{PekQvI=f@z?2/Fvwc1$&Cgem(<+",
    "av^>z.<Z]}wwNSATH*:iPjfN&W)V4:zr-c8c3YLQ3(xn:)kn$0d(g$P@+3vnr1R8NC:#udFzVy0>f1:U?IR=QKKjJKY8aAI7-C]U",
    "4brx$0oCK&PJY4w+Svgo[pKVMgajne!qUV^lm+jjfCM&ps@v(nNsns6wjJ*^G4Hk<9g&MYzYO[)ZB8MmaqD0i2@Vtd}plg)/W4-4",
    "L7mnW]z8fAe.JPSvTak6<+wWRZVzEV})vzbGh%u-9G+CcLW}3ZfYsp>Up?9k&+Tff]]H-t>?nl-xx*tFb+kSkE:@v2k<gx}brQK&",
    "]E8AaZf$5aYRuQ-moFUmd%#]Z>v2#bDs!u:[dPSKz{qv%w]b:QJ%zk?@?QI}kSWMh)c3TDY){!VSTV!T:DA2QUYaA)H*Ur/U8=k&",
    "C^{hQc$<)u@i$>5Fyd^/Z-<G9xQ(fq!w2(+2.lQ4ie:y(ssR1$welPc&e.UqYz:GFbv)aNO4:r+W30No9v/pDuivWHgVG/Cy!&G[",
    "E^LitXsaZczRGF3[IeST@1b79i<KgF@:MtHmze^!08).$t(8G7:v@c/L6}2Tnyl@>5kc3qqyKDpN66Vcp*.bN(DiT3ww16*dIR$F",
    "=}?RyL4DeSjz*g[yf&CPwh^jDEC^4FBh1V/DBnU]]]5WWPi<]F7&Kl8nl.!3!r^2A{vzGL>ufmF}o8k>H][cH.OpVo18%HrsWLhk",
    "09Y0aU=LZPXxth(c>ZWpG..l69l#ZuKJaT&C19PrKKpIIZ-lSzOjlTBaJ$UoAasHjWRd!qH[yrM6Y+io6$k!VqEQAQL68QBrIllM",
    "SC<^Wh)1!#->glA/Pk2PuOhKDvTd*+KIu%)0D(V3A4Po@[ESKIV2C36xZ)))jnt+/?&BJ@gXmn6G=E9QNCg#W]*MTeWNls)0.+2R",
    "Ea#L4M:O/!%i{{f.WQE%23!<PJk8wR1rGkf=+BiUlu04vJn}@was-eV0o^E$Z!P)4OwwKC06PITif!r[W2-wyay%p<5v#?]Vze}:",
    "5A^)[g@SuRXAlh)2>vHHI42uSQK}jklZXX6tPso*wL]l^LTlVW/=$e[A(XtDE#}mPOP0M4lo+T/2tVBaYOxC1y?$VpYotSDjFLFi",
    "kL<(.?Pxlu6CAEu(td(.!rpUiml2lIeu5S-PR8m%9USk3jAGF{fYJCMOj=w^TEXK/=sZ3s!k@Y:->M=jvIq0<=&KcV(YP&j[t*}2",
    "]byW[@L&[xn}O$l{#@:hU-c4Ost8FzcNRH1+TTMP+w>J+w77rsvMDLll73Z%Uf?!:",
);

                const ENCODED_SMALL_FC64_M8_U4: &str = concat!(
    "n=@Y&uFw:EZR2]DQl3$v/s=1l[4hockASy/?e6%viu}=(1s9$hQ6uJsv?&xmcy}OdWK6Y(]W&d^q5b6LTyPZw+w1dyp)m>vwHw+p",
    "X(sLI:eQgyoGbKa4&>a5PUfB1Wf)eL&gPy0C9SI1rNr?s=+eH9xs=9@(g}6^k[az&.QrnmFzF8Pz!)[pMo6NXd4M[)f[7H:-@3le",
    "9o$nKn2G.c!qND)-Ed$N=Dc.-GvcCS@a3)k1wrO{*F?rs(o^3+5YZ=.+7DGNdP-#uM{S>%gW3eH+zyzPOkxTbYH[UR9X2J2l:z86",
    "bKRl?u7Izr9Pf1y3Dz)aLdx5P{-)F=Orui4/YU&s@Xjk.2TRALOg[IqD)$z}jCabb&1pKNRJn{lxL=qkPGyW=M[m>(Ji9A1^J=@G",
    "[td{6&eY-UzEraJW4/:%fNl70ymS5KfDM0jimlo?b:DJ=&?CwAy[]m*F[-3lFq4z]rW${GXcFzlD{HQ)zTaNwvQYkGaB&E:U>g/9",
    "2S**9<wL&/P.jsdFwZJXQm[D$Kx&*sdV&:Psx%Z@gZe-Ht&YxTv52elsMCTUHjlzvt-ZII>Mth9yf]/hXyO$?@qDr4v39:*bOLhY",
    "GAbq>8O+z$nhTpd(xZhrd7hVf41tov6H?@ky?L=v*{R.}M&<GNY5oII)Y<ErtgG(v*qp6Slhl5Bdj1#cE.+&].jY(5=1)D}h}IPN",
    "I)!2CgTYpp}e*)Bb9Ygx1?QCvK=A>-i-Du#WrRU-tDLs%)3nK6h5qE&>mBjE^WMid:0Qh!a6A0+c9O7D&uQ0qNmeNqJn4k[!(8]s",
    "Z5mW*81f5&Xl>=ts(%hb]Z&B5j1LhgOQ7o0N6REV<zSIX@l9%g0N%d[A(mBX4cj4]qrd!(6f:t$(AN(.hhpe6YIKOJjT+bei(-Kf",
    "ygAR]&[UP/RBNS0x01A-uIQ+q})htDC!.}BnZLk=gtMXdJrk23YnNPv+YSV)k3H<%Ki/:EC&!21S>+ND>^chXn1Zs9Oj+@TlxMEN",
    "+yJN7BVV!uswH$vknn^:tQLy>wDS-gbpY=T469KA!H<oNQ-kHXfO<m5=1d$WvGn8B=bq>e(4OB={FycpZ1VEG}Bc]piDA>q-!HSV",
    "e.)S]BW*BnNiKTNz.JX88WtZ[)R51K4z:2F/[Z214arlXcb@>E<5@[97Cru-+961(Bd7Gubva?z@]7S83CbE+hM8kx=nG/eRF9w3",
    "T&LGBBJ7w}Tm8E*?njg@+qGC^IgLrj@Z-.CdAcD[</d91%d-f0%8$IcJyJ!dTQn]el)/001}7EDxKm&aoF}%CPTVhVSL^M8BHiBe",
    "b[Bs{3JY}l+!x{^[}m=uMatFV>Zi=LXc7H0Pv}{YXq(/!AZ!D&lyh%XC!Zmw*vaBx7bw7+u=&U*&*thd<R4[ZGeuW[zR1$L7T<f.",
    "VdAex=Ho=9CXqMuVFH)g:c4)+Up/)&54eHLQW$$d6xv]Zq]t0naT2V8un((qV6QfO=99.K<ng#32ns<nt?%&>52uOC7[GetBSwmY",
    "n4O*=85b=Tx&fh&xNusintW/EFa&iu!4yz#?YA[4)c)-.<!^}$))Ftj)NR4PI8A!}M.HQ{&#rxVIWj>%J-B:}Fxx2$Lwh%r?kxbx",
    "Aj9x)9sLSObTZ?eoDzkrh5AeNzt^^8z>^O4[WU2G(ew*eoNLE8r)N!TNy]EX[t3*ZFuQN/pSG7<!0u9dlHwby.Peie^{yA+5?.D)",
    "vaJ4P.#hITp>XQbsYIoh>?alzzc?YPr{KM++AN)$<[ZL(+SuYRJ9Cp8*L9d2m^$04.2pY89f&GEOp+W(5Q/LHxUobk]n/b0y7=Pu",
    "(Z32g7pbCN5]XS1-Q+Z}H)(^PrKvJBDsJ*mn>M>i!}z:ujWU@b1<W&CXJi?QOsg-!/b-g2{3OFovWaX(nX&[uv*)4-f/T>Ih59a8",
    "nw3Z(4Q{J{99i?sU)E?Mo8ycLRA6HCaDk}Pgi4m@R1ldJMm3NZs(kf3>q<^1J^d3R:JA!XIWmzq:bd5:1:)}]m@PUn0:-pFNnrB/",
    "FKWX)v(oB.FKb*ske>yJ8x<]0hR>my-Mb6[@n&gpd(}@s}^:9EcCj{)(%1wvv%!x)BU^X]fwS+zmnsMh>Ziiu)$OOsfeVu:fpIR$",
    "Ru=1>8?E$3D^s03-$:YHJA7J(<b[1r2dlI9G1&}K@S-Ltf4j=3=O=}fbC=XHXXZo*c-AK#oGFPPMkJ95/tE0?cL)DfTj1.]f6NNy",
    "^jsB]O*w=%tHFY#wqzA)Nx-W6xt+.4exjo{g.2<nx4b.Kl6%]3CY5*gmO>T01z]*9V40>epou*@P=IJzB<r6iv=&V-]:/P^FQ0^1",
    "GYKU@>2rD^sDFDOea+s7)37<t88Sx%d3shnvX&[kB+(c>/M)}s4Xb?#2}5k8)7A}zbv!+Tp?t{aU8R$+]h4[u=74S{pW0r(CP3s<",
    "i5a]bBX#Ngg%hQSlze2a9i.<{M=vVwu(ZjHKczvZTpZ#p1(6^9CaOs&0EQg%ON0+VZisoO/%RNdaf#{2TWw:AcRafGYx{=R2fyTA",
    ")%km6Uq{7at8OuM6v6&D+6pXny3!H9>N+o*gp<hxKij9dS%5{zT)DDVtvnymd[-=6<MT^#(OM5-fTy*AA$OBnk%82drFJv[Vuhi(",
    "wk}?S11*ohmFkuC7BdJ{3WVT4lEvwFw/N<mm^+Z*7FY/I-A=4#GztHfeQPonftld7)d.L:Wrc4AFJN%njd@lfm%FMdPy9dKp#gYP",
    "ZUq{P-FYC.EX7TB8GzuiOe?#3quUMlL)l?yV(c9XxKw?zh9qshKKIljeetxLbAN*Cy+Q]kIQn#Fq:SWWGjIFPsU7aFfSdr5{FScC",
    "Q{C}wp-?2F)Ea%Qp7$f3zy*o2kp{kauE8Lj5h#Bf}KFr:sK=g<5NZnOU!6S[+g@TM36kJVb]fd/?jv}pkWc6dtfPt@d6hEwPn[sY",
    "eDc0&eq.uV.$@GdE^*u2P%d.m7y}=Og^4M%Zz*:$tFpId.%{ykSSe2xW{+&tD9CZVX0$5]u*INv<s2k=}+!$d=T]>:2^Dn27B^V[",
    "=2C%il:>l522IoZ8h{w:{hcP*Q86jYd]H%.YQzKbd]4t6T)4r0eKAb][?2RXx$7SoW7B/0)uExeq=#pFv&cNS5SmI<54uL<dExqZ",
    "<EC3?zp-$}&vqD5dc2@)M#=$9y/^(8c:6)w0x3/rXY(29&iUZitr@JUAfKV)Oq9}8t5Gefi0g&#ai2A^r=ZBr4R)A8&=/x96&Ked",
    "W{zS?yNZ[U8&t+3>p>gAGCFbY2S<a(f*pET6B7c(IAby+)3ubmSH]0XH<vN-?*<[5U!A(D&ih:=sZJU)h5HCFgKbSS^{=T%3FN}X",
    "8{%FO[%z?SgpUcjU5+HzC*X@ya%5^e-5m.it{l%ClK{E^MXQbO)qtV.Gx*<$G<kCd:ci+(r+)TTgB<tb&G}i>buL#1?#a3G:VhWJ",
    "-fC{rWlADXNGYuF?lG48+wySIRkwP5!s:p+&WspWQDK]a<0Af/mqd3iDC9g&&iO&+>:C%AEtc:}c)SZ{<LrqtBpw{F=A1Dsp5<rr",
    "pR9O<9!?qwg18^VDeX6O@@--]bdV^Eb?vPhKq&Zk^z9Z+*/*^xxCg23C&$*mO@Yj3Q6Q#+8M9T2bA<=zX)oOA-3V[BbI>yz&]+:6",
    "PxV}J=V5kk!MulyN-$<%XZRJY]5F:)A1&TyO4h[tw%0D+[I#<%:k]1*oN+m0lSyd#sTaUA{q::b5Ce%=Mqu}YvkFuP3Ig}][lL3p",
    "/4EByKGG-<8me(%g-%E}jn*M<eWp:bUt-L9P=@5Kry^+o.Y/4w.t#OVNFL^MYJ#vpbCRz8NqN^y+::2yeH4?<2WktkPf.K3wNGWA",
    "/lnP9gddXE-fZ#$/Zl+(GqQv^BihS&iM0nV4q3}KJ#2HxyB]}b}GJ97/*2.NAvco2Z%XvTFvY10TE7lzw:>C/>MmIx>6swEMA++H",
    "z@1nO0@Af=U?2423S5v!S{n4{2@/ET^d]f7u#mg3?G-zt<ba2NFq-+Q?]p?((=q(l)@9RruF%UKORH/uc+@{1}kGJtSr2VaA2.zi",
    "*p=#s/4^mqmWB.3q/5zC*OUfrX3>G-}X!h6I#}Iw]DtFA=>d2cd!J8wazq=Ve$57&]QdUKBIOW9c<C<FgM3p/6ZnmOfdecYEGKHL",
    "lL!-CtMo%JZmtx&kbGZw5]#KorNY$<W[qp^DaPDSjnbaa4]U[XX]}@6VL}@t2D=cieNYQiBEbna[S(y5yp^?oCsFKARV7OH:5.lk",
    "p$#{LiT2ZJQODNb8KnMg9EDQ7^Xa6j1}8beKi4z=:J4yC?cyQQj/cVwW[[fUY[PoBui$O:@F{@(ar%d+p5[AW+l.<{HasMh[cOIC",
    ")5lX)}HA/O!74vg[$1F$[2YeBiVR5hDV$/su1=afG&/S+q/ddJ!&&=vk2h[S(02%TK1KRHH8v()PQitObQEis<wJI3EeyokXxqcv",
    "YdB8$c)H94^FP9uQQ?d6HZ/tcs8B8jmZmh3SzWrwmoCjaK^Fo{kV={TY)Lw{(l?#-4LG0uju0d3^AUcAigTnqbHp5p)Pt{>kTJ@j",
    "YNo*{2PGu#sDZtgsS7s7K1Ej)H!94Ws]eGs75aulf^nG*Or&$kO#zv@L+Bv9?/xn>@<YCG!n!}XZ<4a^xQt8HHIDYHHwZ)g/FNb2",
    "w.&[t/94>Z-R3@d!b5#9CMw(0=/j60wz*$iLEqF}/]p5ncBl=qsZiQx4W+bD7)ZWP3RH%[l65-y0(73:Z%n9al$IMBHm}s8bIfK&",
    "Dj&t73n#(/dl8H+fWcDdmP#VLeO%Dh{J9gA=!yuwJtkbJ4/]A*@@.yDCHU/?sDIb%*@C:BdUAz/mC1d{E#C*u4^so.wbv@A<S=/K",
    "S^*E2lT/Y1:JOP=+VCK>N>RrlXP^hHIGJh!nZB]TYTLjgABF@hH6m]E5/7<r*Al%rp7LQ.W81jPcPgrOb&f}hZ0EoKn8od!O47ZD",
    "*)C66YqOfiY]X{IYPfrX-Q!<F*#B5I8uCu3&hj/j)/xV9+/i!#Lp-Z@@JN9J1<NDsYTsX3-IF9#X$ig>^.$7Ssi:KN&+Vv)rA2Q8",
    "UKXkE{TMpSrr.qyv#[vp^7w*fxf)/ShPBujk^WgF1/&(ZB4su=d5qd3z5X!Oq8z$1!wlq<JGhn%pzor.L<+ea!Ko9[onyu%BNzWZ",
    "=qQ1wN-tJB^00&32]Lte868ggbk43XaaD<zU>H1s1#fz%:Vg#/Z8ycqry@PwMa/BCgbRzjdf]u9R+LY8<Md@a=T3uf!5OBJd2$2+",
    "1o5o:Z:YY8:&XUr0b@T^:BycK[Ig0QPX2*H@BIM.zJuoYGA>5Z4mJa/wHfBK{lkY]R!/:8zodO$hA6p5417BR2eh4<WX=KdDToyW",
    "tN^n/l/[((PT+>4>$D1VmQ72H8NR%%D}}4inF%$hs:/LFc&#$6Fa#%Q}tLlz+}[!3&fy30==ot9}8Q%U!s7V%Q?:eFC+w&^T<ZQ&",
    "lLZ[dp8A39hYweG.?JYQJqA-wEtK5$w%]72v9sc+EXw^nVrYMa&8Z(iSW:V8{K@ETbOcV-!:2mW^md@+-{SMlVn)3v[oeV.6N76H",
    "1*pFhfcnAKi)SL{}O*-x}VU3NV#T)l@u(o)R!lP97-sqRiG$z}HNX1Jwbn5DRUKgO-G&o[>A}k(2r2q&[u>bqhpRT*]XSxSOgT7<",
    "nvTq^Af3K4yZITQfmb#GUVF(TkQ}e{ND&wpEW.N@cg8IZm9#HUI=gq6E1kG+5YOxC^.gpE2EoMboFe7:Ai.HroST5-/6-<Te[lrz",
    "H]db:[A6bL}2n=aH*8NE+uE=Rd8^&6l31?CpVbXC=GF4/=b-]0G6k}y3UhigJ0Xr6IN0^SWDh}MRF9>h5&r#Wh}Gd=j2Cf{oX>rm",
    "z5tG3oUEKv?>7=JHe$&GeprExr4N^SnZ:1<@?^+2rx&]o<pOx8JqHScYTG1E^CNa2&kU#nIb3QQm#.pEqO:hkxXUtS[<eTbcK&Ke",
    "l7oHw-w@:^zByr+Xx>n>fTKwmLgh0C^g+rGj:qW{9Yjzv]ru4{P@b7T9zQceI1&El)c>a>d?jCIub6<.qG%$guc3$G5TmVbUSnBw",
    "O+dfu5oD.(TlqjIEGyR9Z^emuGF<o6YixeptrUnnZ!)57wv)jL]h)(-oGi?KBZ(4Fz^41(TFc/JlY}:to}lBv6-g$C:-PbFJP2J7",
    "4jT!gtYa/2iL#p&[gJOh2+df]DP^a$]oy99rk6cZK8Fn!54/nkCUhX:T[9dsKRYW+X<7s3amv6gj*HUyS2<RP19/X^*ob$uz=^rg",
    "XfD>{&nGeHowvF23BzfAO@YZfRykMXDG>TDMKX8Jb/2:g.U#v9]VqOD0zQi<%hXrRSUUPFn21bs?l)H4L#Zpp?-90#y1o5]}$o.E",
    "/@E6DX@&THPE9q}is<iBFux3!C>j2h=ZTgi-E#5>UF)I7b3mk@yz@9)Gr-9J>G=yS8gV2aJ0&=L2L-$=qvS/zo[wf=K{=[9?Ew{!",
    "Q*bYtUS-sR)<L[kjf8myj%B/bQpC56smE@T6PU5:/WKGDlt*3>P}L^R/SZ)VS0WTXE(&DI(09<yjXru2!uxj)9Ft-M{yWY{-)b$}",
    "?TU^31OO7]d6C}J*1DA?0eXfG]qKGe5w/3[}I/tOPLt{&760j8xC2+UaPno62@1@+n<!g)t@tf!0-6m?YQqHdu{ikRE9.&aKtIi@",
    "W/bORV(c0VU)f.Yfk($nFOv2a*Bkd%m{U4olf]EMQETaMva%FIgUIVs8V>1yNwc=BywXaC{uf=n4[Z.6v=.z&)A1!hC]9P@W1ar5",
    "(]lSgkG$6f@!hr8-9KoA+6bK6Ugufdk.OhWH)Unb&74v)Dx{9CjH9!Sv7=lE9^5Zw[]214{*E%LfPa^Ku%=pm(x.SqkZLBL[m==6",
    "xX(d&J?4]rj:5V}2K<JkfW.})B6x0=4<?#O&5m{P?E@YGUY4d/cqSTm7tmDQyN7FOj/m3#SW+8l:LkDg)7wT$2CBL#UPj[2i50Y.",
    "wyHPy2/xiEnfti4y:M?UZMQPuCF<FCF*T44IaZ(kj8oX!5Yj]qmwI@TkQ$2x5BCQ[>vU5-u&LHp-})M=%9[d+VHAxd=(N.KTDk}o",
    "zMW/?p)OM7mhX@^.G7S0MvgB2]U1M{/kwz/PuZI4c(*Bzs6+KkKlj*5&?k.M]oI3[*$bN<weH[11}lc#q%(]+8wS*@KEN1GK)u<D",
    "a]-@N1wn8CLkVlg?t9K7YPr/u/QA<AE(Y9]>S8Wb(B.CR-8+tIxW@Up=&%=&XX]7zu8G$KEd=7723G2gF{dshA9ts-Al:uPIeM3J",
    "t(w9BLeYb*9j(hm^.xOiP0&w$1w{425?lw+0e<*<sN=^kH[P7b7?gs{aMEmK07HGZMw4jXp}eOJSWyoIxPyj#=#n]@Q*#*aROkBi",
    "ovUVdTQ66-@ae18=[jitz^O.>3Q><}!%Xh80fI3dlt)=aCK:g}iB-rcz}G0y7fj:9xKLNrp+LZgtDry<O@WT2g5d-Y6M?wUEaI{[",
    "{thvO1^:W4m6pBwImsiZ[0n[@PlTzIhf/kohWj4XOP[0}S6qN(/H[N}?z#St7Av-+0g%>q4/v}yXPFPI4xVJ(dF$3$D$<0L.03Lp",
    "PiNgo-zVK}Rzd?V/BeVZ*(=zo)Uv<R&C5Fdo/1fh6yAH}X5Yk%8&WcEwRqf&H6?=Y:f&9l7znF=4B:x3BBtZO5L[rG:FzV@FnX4z",
    "mjx[7%e4/<:-.Bk8PLAk7KIW=icP}!UKD^1l./Y9-[k*PJ}^+g1se1KC><8cK*lq$/IIAdD?CK]{E#:AvCo+P6M{s!v/3G}J@oK6",
    "kfBeAq{vRx7pSkn0DP5vY#gQGt0(HRp%80=Nx[Oh)J[Kt!<7@N})V+)?cz}MaGmumcLnQck(k7QT-+8^xRGsRp)>I3U/q2Np&DPL",
    "j[.Yo5xEv9jbLEJavo)cZLZKO-/sB&ZMA=PUBl&L0c?s6=fCr(:GONR0DzNjy2aDR-449qT8r$Z5?(Zq=myS.wxGRuFLy<j6w&4r",
    "0Ku@lmY-yGg&s0>KP3JtX9a8Yp9{]zU+bjnIN0J4NH.XFAs#M>d6saBxX0.Uwci@rqHxMI)0:=6(-=sh?s![4Q&.CR=]1Lio$px<",
    "rHX#Y9)i.fBpueLs&sQWrJ$duAFH3%q43^Dy%UjbxH>cDVR>bA8jHaWqzb{KazvGfsivm3p0cZ8:5x)?XeNGxCTUDxsPiAQnx2:o",
    "i13Fmj$F-UU2bx!@!aMvFr.?5pd[0?bfp25zy[@7Udn!>0ivDTDw&.9k>:-WS*QxfF9eC#+R#Jmmoo&gsfKFAb^x5qKXxk#xRyl}",
    "[o9UK[wWKNpCr3/f-HSa9kJr5wW1G+ioa*r2hDvA65=[Qb>dOw^TCce2xDAHdhm&SdpZsWpZo/W-%fTGID*&V5+f>Pb$Buo(S+Ny",
    "(BoZ95[8+yO!N<hJuRK/*Lb!]xE[FyfY-K6-NO[t.Zk:QJMXm-@$..2JV.%#C^gX>Gh{%A-h92/aFjC6zuK=tek-Mk*d:9iS5%N-",
    ">FFx98z)QvNXz7wy!VAziWDq<*O=14g40NSiZwRpb0VmAtM{MW1jhIhB@dXMvlxxe]eE:OGuZ{toU?i4D4X^Na4u)S81!YU}FWW[",
    "i%${G([Ay)]8NVgE*=!1PYp?<[Cwzj^(}&^{wus53d1Exxk5LNr&nzJv7LkS*hh0%FjQ/@H%Gi!g!@wh!(sK<CT0okk(K?GChvOW",
    "kOBmjL9V+AoTLwA^1[X:A8](JU>nKvLX<<y-krfJOHVwcevF>GGOIa)I.iprx7+D43NrROyMlp$V-VFQ05RZdo#FCz^X8(s)0eDw",
    "dCH<^O?ll(GsOlJAR)TpPPUONm#Sf=q1%d=N1TV=@PMwSPub}N5zT&YNFq&Q{{zxYNH*]ifJKyKWd[Y91@Zi1rM(&8P81*qQJo)d",
    "Z<dDAb<yl30qNJ@ue{*RqP5S2FE($roNRVfHnC{7Of@eqNz2)?@*X{g<sDXj?8Os@QB4w5fTl8v-XdV.5x#yZ{w?cQ0jhqURbS4g",
    "xX<@Ekj11/95l5fZBOFgg(v12e(MQIJ-PJ1UubWbZ{+yH(:dnrDuMXW+V8)1}1MtLMvOV<nn3}}c(iSi0{Px>nHMbNDF8=w?Icx>",
    "[k!Nji%l=(>Xr%S@k/$[Lj/CIuU^[GE9mL:b^(TB[L.#)Ir*qqUMlh^[]EKtNAtVC(.43qTGP2{ys^#0VsUEFG&G!tC(%UK4@&{W",
    "+L)Wr?-%]IEoN(d21Exh[kP.b7=B0guGbJWAf[FQPHtFoVn3Honq!a{g?4Gt1M5%/A+xXZ9Uzn-PV4y8AoGT5BtDa{!wB[)ShKWH",
    "V4irCBPj2dSl+)=&//M?Sj7+I{#zN(}{7r/kyeeH1E=6#eE*ZL7O(O{a?*(s0&TBdG1-$Nj%W:4JgF?sa&-#-^se^ybXA/mzwKUl",
    "u[b3/4YCh$<UiO@8>odbY7f7tC/hi#x.wX{ND^2IH(2+dngs%hhua2%<uFbTr!a^C",
);

                const ENCODED_SMALL_FC64_M8_U5: &str = concat!(
    "cUHvXGs$3[OV:C86oZ<dLk7t9Ke/&Q2>=AP9=^WZ}ls0+s!h@.l<*)}o5$09W}hf]XQAo@77-2Vcm>4=O@AIda?wENH[{z3[EfJM",
    "sYU[[eJ/Q!WDY0I.^t2WDDH7*N{IarFf[qXp1rlci%d(mNaLn1NnU[8/{-yk*SrwTKt-xoKpddkTxL=<4?dhQ2&PqT*]wsQeK7pt",
    "*nEUc1$A[xRYO2+ry/ztn-o.m?tFAVn/GuB:s*7FF[nb#oZU8d89mgG<cqw.mv5r:D*A%Xm%0JIfh0rGqXI$>M:DMKsEY{+BOvEM",
    "zz$oNBPR=XkkSz:U=S{I2Yvprq3PMM0Kw9a}HJ[8T1)wZE=Q7kZ1L&!(a07[BJS2[&d)#s3LkoX?H9qQGMENlma^seE}!eXBOTAH",
    "]Y$FGml)oNiHH(2UqaXy-E&d[hh#PqXbd<x4*71?907<Ua+b/>nFBfF-u6}1/H?q&e2O-EJ/.e(8A&$>Ffe:zf74QW/dNluoz)^j",
    "L!&LCJ1I)G5d0[Jk^1}p=VhJqXs*h(MM=4nJ%4*jPPdac-Kage}%7aySqzGn-p/k8+O7t$:n]<n]h{7E4wV#!H=R!7IyI=xThy.{",
    "))1s}rZ3S1H?CwNe)l#xN+cc7aVI0+i?*mI*%<$t]MY4KM&IxA60esGl:i{WWWDOE+]a+?a/[{h!t9185hWoGT0WN6J2Gvx<b-{$",
    ":$b?wFY=$n:e+%#+CndJ:8wgB:-^Fyn#w#d(A]3Mp15k*=#67L%djhxrH7-<fwUi(N9MZnPeisPZlJ[8cwa@Pbs>S]4ndhA!7Z?m",
    "8DwpKiU/T$()tN[ALGdH]EC^iUlwG3qo0Gp(lrn([I@]#&$Gx{6bH*ofZ7.+Qc?[>jv?s)R&/&Gl$mi2FwK99FC$FsZWr*uF>jt5",
    "P<rEq+](Qt:f?RNDv7/rOc+JzmAdbEE31$7go8M.3}TX!a>S!Tlf27g!7Xa<md9TW8y@xZlEjJ4OuTfHi+kq8vKV(w0i>[6asuu^",
    "!sQ?C>hKH@91]9YScG2?ve4h[fVH37p#I}#eqM9}p&VJGk(4]KyvV$&VlU4nU-X(D?Kj41HuL5sFAF@9d7Hth*f?Ud!rbf)[yWix",
    "5x}t<>/PU.S+)+VSbjHSeezQa!r){j>U$ntU0xFfXv&>BbM*7<aprIf/Qgm6>YvfB0cJU2bj}+CFui9vqx#dN>/@xP+Vw#3r[Nq.",
    "Z+gQeFvcpT>X?iHp{o:V.LU%G.AJ&u5rDSH[zu)K*+/hEsAZgEEX+o5AhbJTA6FrOU^qCmfN>Aj^4SoT[Kxa-0BDf$aDr#@{G#ui",
    "V:gfaZmzR]4IgMZGxssvFvM(r+)lnvEk7xc{6<hF*@xJe1<T2eufQ=tp.B$=HSLm-Y=MYbWks4fH8Eqg+6HSZNuzZaHdO[/jzJ+p",
    "42^.ub=>onqf$<EH:V@I-J3ja39yT0uk5hVo7jY3K?z4j)ltW9]3M^mJnI/wC&+BdV+m[MdfoSoBe(*+%c3nyxkdVm&b?aBW50)?",
    "vjX3l7U&Q^[vzpQ]=Aeb[(MvmDbFKL4wneky9.SG]p>fR[b$j8NLrvb&2x^36+Y3f71=u}d[Q.[&&SH1ftU=za-}oVhv>B6Xb%dO",
    "}l4d?AqtRUmJt4k?}:*n((P+2WTV37y<ae!@enz6Dyb15)$vF#D:o23&wq=wVNBD/z=p<xDPD${dv]/!9e+>2p[]%igff2&2<.Va",
    "2aK46JfNbIKz/t95}$@#5Z=&Pn}B))70b/85WwYajLeA4@atpTp*z5NM0lz-C[2o!/{k<QDtfruDj7d}qH1Y]S4>}Fb:Q<hpXTAx",
    "9)MBLVLMz0a-nbWYWy<-V&<KB!p>MG9C/:V7LI3oyZU9[tvp=2jp/-Jo1xF=Gc:+=wPf<%{ssdAz>anHG8L3#Zu90<QWpxhE]x^*",
    "E-o(2pldoEma^F.Iy{l?ayN8<p#Iai5m@^x9CFbeICKgkn}#&Gqe]EBcPP*xHj}YM-I5qPxMOzWE$=ulDXuENPj>F)a1]if9]VrC",
    "S^#$t-IK?y3uh/Lj:B[cF--<[i*dVGj#PSd!!{$vYUIoBS.@WSriORKsDz5%es:}%9j)7L3Vzj(WvE^>yd+V)]9UQZ]/1@}5Et0(",
    "u3&XmYr?z#D^Mm=f8CPhQ6#N=mCz6g%g=/v%l*W=iwyB]AgnNC@l{=>/iIPzZwgO?Ht*&HX<}<{Nq-*60@dWqaw>c.wJCAa{hA${",
    "Bk4P%dj[aPVWv/%0.r?+Wfld+T>$3Wg(Xly6H+>-0+8l}C.yu9N7-IXlb9H+QH1i?Lq<4n-NNNzk{X]ZC}MG!LA5wIP0Bhou!HVa",
    ">Cdu9S/6qe8[f3lWx2q2YqAzaL^PWoq0?1upT&uaQ7kpbeDVf&T=iK}VENhZ.0cJ=)>bUB@6?>(zJU)Ksju0si)-U}13?vGFeF)i",
    ">IzYwkK00#9!f9YP&rc$@)=qdH{i5P*^p)0JE4.ulm{M@AJ=4G?9MR8E*yIpi2%6OW3]-j1nAIxi{Orfy^7AN9?LhJyZ6z)X]BBT",
    "md8bge1AL3J-6M7]zlD&^UHGeS*82D{/%Vd^Foiz^D>!=y{{&qa/fYL?-+X/km&c7*X}t?Q+ZdS<&Rf$)h%6P@3nbL}D?W-bpCgU",
    ":%T)oJr+-Z^[Z=yEXF71Ox(^Gl1{-&xTaA$*3*SbRCvoa}<Sg2M=C^Yrv}0f=iqrKk1S{W68aNM}*BafXZsCv<]5*Q(NfFh/1G8o",
    ">^E(Z6q2Q{46U*Sm.qBe>dZgzEneS.1x7WJHh.(h/=F0[YlYTNr{{LZED#OI9Lq>C0)K}jg*H%iz03:vPD7>Z{qFNDjT)W1R3@?R",
    ">JaoVGI/3rUXvl7.67}MCeF^x(tdObu)yH<+?5gBuM:?Yy4Nj{)1A!II.J!@iQ.Nk(Vl*PU@]0Hv2BG?.&Jhc[DMkhhKMfWQzb:J",
    "C%KhF{ItQFNppfHCSk@w=Ej{uTF!/Q?tg)1J9rf[JfcB*Se>YbiamL6d^@wgWLZJq{5%i:HYyEsO^/N2)u&NtFhJSQb]A]1IdK:<",
    "7F:fR]QXmXQ.LWHPSYs$Cq8O3Qe]BI7=Ir8hOIC<ZCxohQePNO[.+[h6&S5:a@tEpob*q.ssrb(yi+-IeO+B4AiHs@jfhvAF(}9E",
    "a?q?VSn#WBlBE6i/mS?fRg]mt:6}g{](JjdDTZK0Tld>]t{:3[aXj7BmaRvI?gj!73XnsxXw0Ted3?zbOG2qQSf#2C7fqUyj2Trc",
    "WPrps:(am6KS9KEJngD%^k*FPz]AzOXOjQExfFQB?]Kzj(o)%5MD+o0=oK3Cqm=0Pth^gTH/NFrBg1X7!%%Z()Yi]Z02K9E?#N<Y",
    "^v=T=FRJ5AT?]}dW6ItFPM*/a<$/7.:Velx2Hy8%BVzME{3ut8e&+vAzn(oiI4x{aSpF&VVmJ1pPTrMsW4etMwmoBu1ZOS2F@8!b",
    "}j}{)6bK(^D=+][L!quauqn{jC[t&%N]##eY!cL64dB-<SNqeqql)]0EU9[HXcnr-{j>SuGknW[^4X]JG&)j}E!a!@r*vpgWLS]R",
    "!+<*eMN!umOSin9F9(yk-ar5:]A>4JAs4Z&>r*2#GNS8E{FJn0X{</CT^G*o6v?K}v<rS&:Dh3qMYhhj!XP3.b68c}?Jo#=R5jG<",
    "eXyWs8a1=@C>^1RG>kV{2CM@o!IBJ{ifDo=SQ8jHUAXDr!3n3$4c^mAVx<Vpx8z@#yi6hmoCtuc<EIw}/{(Lox0fxw0)c#{b{sr[",
    "YSs{)q-=i/r5>KOBdPx{w*#H#r1x{hp)I?Wty)UPDvkAG1@tACs1>ZW7bXlsW#8y]j)zM3=!Yo#{}VC-}!3Ze1>jLRrcyeYlh4$8",
    "14j.83#!M$hqjS:z==i!I-WB(toI3ISEcXs>JyhQKj)@7kJsBW9HD997}{epy8FS!1-Fuxwk!{VQh@Z=!w{raiSU8Jep/]D)*{qG",
    "*an-a!8d4n.:?Tce(s}$5?>6kNvG*gwPc2gLfJhMZJw.L+pab/{U-!ExUqc(fpiLt[)L!qI5q>$EoA%vtnUY1ccL^v@qEeush?42",
    "j/tnyv%Ig?YjeJno[G&qPfP)0rXe<o2lDtrTDOKP{6^6B<2mK-hj/08s?bwkMxwjgKofnZXrKUsD)EBUJnw3SSMSauE7/UBn+y=G",
    "GO.d>SP=1FzNaaxQpWi-PC+UXCywC.++gy/e%(]cu1fo^GNdpBz65Bm(/ai[Fn}FIj*jB&o4siW3RzXTDTOl.uQeXC!x!H+wG3uq",
    "X{XV:?T^We6%w3f8HBFd@K:Buuq(S)a=!#OD3f.iVnngpxdkC%ms2$rJL)$z^BfFQm$C!Cfmo[m!odcw8}gMBGPZHw3wrW/d]GRo",
    "sZzC0l>=]srspSDeCyLRw^x8m6^-Pq+f%cI9Yyk=[n8k>QGgCWpMXEq&ky<xhQ2?$ZVCqP.@1lN/W*3+*O)O8iPpPl0!I4GsI]qC",
    "]g7M1I*uQiiqg}oRuW&0+G.y$c0RPRDEsDf2{*GN8}DmDYtrO(V[5{OSy7I^FpTZ-g7m9)53&e$4?K^^tgv4B?Z>])SQE%v?oa?w",
    "9DRa>d0v13{Q.Ff*ZbF]<^+r)m1CUcnLDcEzhVs}SSFK2?=4/.PZ{t&?YEOn@x@h18VLaCA#+!7Zk)(TFEKGI]CS4/1#Lc$=Q&ZI",
    "fMwhF3J:x)KBg.71>M.!(?aWH+=Ah9QP($}e2n9pNoaWExEg!-?^K>{VA82d&?ImwALh/Bq4P()Wnmaw!W1.VW)!1{-Q)a[[<IlC",
    "iPcw&{7mOz]p-4]R.B^Qz4*r<gn.W}xYp3{Ji0<S<9^7Nggqfo0JWiLp4/ml&mGc=^>fs/>?p<b2FAR<f=e#+vnCB/*CSn1f)j2v",
    "U@@Z.8Akt(0Srqqopky=fK$6[Qx^CPO!N)iJGG^BppQRzY/W2)Ds<ctT]^Ax+]I:aF#%7bsX9[MB}yvXyxPEb1fV)XyUyB8&?DH/",
    "OM!@Mw!1TINaqqL@)[A?O9Ek>NVzZac:K=9KDoJbg$5D*liNF82c5w1zYTr&iff^%ZloW5YmN&iD.oGm)iztr7+2^9pN0(M0xS[p",
    "D==x6>@V4hTF]eZnx]<Kg0a:Px$M4h-16L}tP.VZS%[{t7:C@>:D$I(JnT$7BR)}qSnF/iP3E2wutlb@+ZZRB-:#Ftol^zGiO<u<",
    "g-)atF[Q/uR]->&lkniro[Dqc4tesSx:lW.<r!xIzj.3CZ&ZKet4hCl+(6&wy+PUsjm]493tWe+MmwT)Qkaoj?hJspy3KUufGXhQ",
    "UEHvu>ct-oQB)(Ref?.?bFT]9*+wN!Y]Xz>oP*pb=Bi^q9YtvVazZ}*{s&h6(Z52yZ{BS:aCtXp}?zvrg8JJ}gxP9<{tG1nQmDc+",
    ".-yQTrJ)(oZhaa&h[/j79CUZvQnR9RUb-{l0!B6e0:.(<7w$0(R?*o}f7P=vhw]@q]^Sf?@Fe@!L>?vy(-ZKSx%n)FK8W&Rj]n<9",
    "]y.xjcDaL2DQCtcG90+>R/6{Dh{jg]rjLQ+j[rUTX>dJrJfe&TTkKb[JgCa921w^fiqXSFMbh0lhh[.45DE0**tfPVezO1N77Bwf",
    "B/hAa*k}T+ZoYmK.=&UBVQ!gYR2$qxdT3uR?Q#j0{ecpL({O?a:DV26]4Kjf-DB5Kp.E6FEvYGjd>l@I..}n6gWTG>zgMT*DE$>O",
    "wO>wWFx[iV?w626:K<3EkBqZu08tX>0BP0y)!O}b+WLC6K!>HWf9//=!9QnKUglfewmK<!1#*ClXmvY+1n}4tX(hOx-n[+DQYvp9",
    "AZCz(A&GX=wI^e}NCjD1qW%hID:PuQcVp#T69T/qyvyA27&UW5Ds08O+BzV#RGeX2Zf0OkLW*KN(m0xg+P4rWJ4%[q3DFw{^BKdr",
    "PY{44xapGE7)N@Yo54=S&]p#(JA!n5BS?me).QD/sDX=VXKC>#S:Wbnii.UXeqF79FRKlu4>6}iU=v%)Nqjp^1<>7F)80?TFW8:G",
    "!V^y.TXPA?]vd=]bO$>({{8+vxDN{Jo7O&xzLFpqQRl%%AsG1-^MHWAUKdSthe%bv{rdu/Mo87X1&k%TVqWBgG:TIpk^W-d%kC1X",
    "{oX.KXy(CD.CKe]az!0?gwBbFp.GM!w)e9D@gA&0PZApN0cboRv3j3)XXaX1:a3-AICjn*RBP=iqzH6)=<P.k)..910jN+D7.^v(",
    "Zj3i#7.N}92CD^-o)<Pqj>DTDL1(PKdplq[XQYJ2EJz[gU)&3ztN<rsrtA8dF!*wXhDFx(0Q>MMv3>Mu5+2}ps%FkPeOkwO+pp[5",
    "6(2D^hMeb&W-3(.](6if1nJnnjq:D#R=-dC]?)MVx/?}I)5.XIGbdfBi0nV-(mhOx><{NAO?$ZRM!%$&D?lEnKl3zQCu%4sCvWvg",
    "EFH<@{9r^7/v2?@<getSUNkqjurd0q0p6}OFs$Sf!0izofE(n1r3LEd:yxY-hJDF8+N5p6Dc{Gy+^pkhu&0.++VfJ:rQ1njCfLoB",
    "A-OmmC*/o#}yHL/V}KvYC-)A!v609/t$opRdK}{wDRar5U$EW5swCqG+lyy03LdetTJgWXE9XxF!#!bGS)aqM!qpltkiQRjX]GNu",
    "XS()5e@+-Kb{JEP=>0MG0*0NGlTx{W]zEKTyZqM9P0!*p6F%O?CdBec+rH6ZD=NCC5@K=AR*mEtH8(O[AcgoR/#5GPg3N90DMw8e",
    "Mg%=h2y@75v?!KFD.C:FG7h@aj}Jrf=0M#8/Wl82-?#9^n]Oxa[PKVtQEVW{@KO{<dEMDl?ISn=@l*CoLGY4I9{f#COqg<[g!N<k",
    "VgcwUpoEBaK@c8viox&$fLHIJ)fAkCJV9.!aM>1lJcRkjak-^8k11?Mgh$*TCB@nb}}ub8ntP9C4?.w@rEnL}c*F02[-fIWOl6h-",
    "X+OJN?#2C%n.5WiRu$vc3cp@or01Hj}AP9=@AkmCN8/?5X=Me38i/lSqXP*cHQtps6u7br=V<v(]f/Z{{?94fBY&Q@H3)zM02&Wp",
    "0vGIlFT}VYFB97{99fFXJaKa{6=4X3S6>.>1$f>[&)j:toz.]br=+pOJ[w2ew8IKA<%ekoN/jCx}L![S42Hul++@xx?PQ.*rH>6)",
    "z^UYww0qWh@l(1!M9E^h7]tY{2p#vS[>#9)e-0em2/ooH@pFBJ+*txk{*(a(yZMxtNO)oLPO6FYjG>%4SeL=gg]H!@!dJ(/o.1E.",
    "y>w=LOs{JU-0*a?rk/>wPPb.W)j<zWXK!3VwLJ}6ADi?GVBXn[.ZFFFjd4zf@$FA@2:S3m!rNRUWL.5GGnk5)k{pG!!{-(EX(:5r",
    "L%<=F+p12/x1(N7dp9]RHL1IGsX7#]@V[3%s{!a<r[Zhf<2&hZS=F#<z}XO.4qBGUYIo86)SCP{Q&>}o?Njfk:[f+<B$j6q%4CNt",
    "61+Ol.($HL>gO3yC*&3yctjHn7EK>e@9#[yCB0$C7H[G1p18D[:O#^-lZ5AK0f{XJfuMpsLlX0GuGWcUsNKo]652(uNTL2}Z+4NX",
    "(l!2AjJWyZC:!T}vGxN{sz{h.OwtJB(N!1bFtz8oTH=(]>&(={Y0>!QC}ctqSJQUj]=<2<ctudqCOjh]EsE!Ho*R@C7v)hyVZZhx",
    "ubh691IX3LhX}:[G[$jxW]fltjK/.gu=@ElHDvPi!Y(HPo2m1f{pBLyuXH+SEY-jP:X0+?oW0S6^N-1MST3m2mJQIKR$CzJu<1b9",
    "An4iMPCHcZ&g228Z&2vH7.5Tx&0m#G(p$G@kReqnM}FL+^1FeSpa{@Tv9t@u]K!0l93RI#dcJfA[B4Ej&1U3gJflg+-es#pzNycu",
    "FZ?g)0o<3/K]=lf?3jRc:bQJ>a.)Ad1%BWA>*4hYd0Gvr[Zb>23Nvh=7*wh$3AU)y@G9L-JzO(N]%:oJ)[1&?4fjeR=fYje?DHdE",
    "i64Woa9&Ya1(COdpOS(Y*OWDNEoAmc(af8y^t!jwCslZuiu.OpY(p}N=>}:aJ]&q:v={v$uDtvBM2a<YsuOMa3a1fQ*@Qhyefv5t",
    ">^W0yQkgnj=fG2R:oX?g<%8s?=[]q%MnQ.p+W8{o^63587+f^zGK}<g&ZV>Wn?tk3v1r${2<pIp2>pA#Aj=^%idWa6oP?AI*eIE]",
    "R8+zpnG*1a>6/Fc/Z-38D]{%-i+{6a2UmMKfXu0ga-:$Nrz{7=LNEvKxZB6USZY[2)2Lw+i74b*w=/>kC!Af2B?t1A08o%!*z%Rm",
    "PC0XQ{YHi5N2:LE<M[Ci^w^skok#h<^7:V^+>H6Va@:X0YH[bCFe5kHMW.Mu9ojrq{YZgJr4tdo5uL6dpga5=mYe192!X}QPY5d0",
    "9[s]qQ.OUFxNKbH^6aC4YVLSol<XW.k+k2s5fzv}5Q42nak#m]32zSoT+YJZZ^ooV&hEkJ7dRa->ZvlWh4.i1e)vT9[djRyAkD7#",
    "Z&chnu#!Q%qe):yy/S:lOT7Y*QidV[iG*UYxl1>NKCmHlR=TbpunJOmoT+RFssu<bgQszLg77DMDZV&Rd@PmrK}pj0ACMBP-)yzy",
    "Xen{zW{6SZfkGQZj:L*BU^.zhL1E4mPIf<FTDm/[T?dMDDv&B^H.<dad-<3K&i213b?YJqp%#g6e@JZ[LQmRsJB(zeF.4&OuY@b^",
    "hJUeRBQTi(A:z*hJT@Uk]&h.<K*)/)a4H*]1z}ybOL!Ji03#qw>t0%0=@C@!.g6o-XlEB{OI2QT.m%^12[.9*pl2W}0lNdh03FNc",
    "LaI=?em/2H{#&m4ei&Wri?T?.>f)i(erKm5Nj%04^k&Tr&&W)EST-TPN$W7<=*3[hig$gNF$jbIq4{TRG-=lSU#6k*(x?*&&{zAj",
    "Mi2EF6JiR$=BjDHe3cy%91E!rdUtD0549]K(p7[tkFw2H1WCb./41&Qo-tBl]7vt)KI2CAbkfIG^$6u&]!2z>9+LZw]Tc3WyMpf:",
    "ITAQvifGik>&KrSy%UeQwBjSQZ[wn2[jG#Ht=x{3PG+PNAptUDPW[q2m:}^VFfEK%Y1QA[bQd7?avQ-x}N5NL&Mm>ep4sDfc$PP}",
    "!Yx41lXXA3YJXTPR.*izvCi$fCQ[OkBK$JCc$1}@lg/<]Kyvunh.v56[n/a7GpU3otc^uPK)1?&q<H?9!Q=%FBA*)3L:0yRdGFl!",
    "ZTk-?Qa9N+/FP.ytP]rA}pTrhSeDc?nkJ3{BM=Yrd}wv/24PeDmBW$[)LTq[8c#5YI}yP+CNB5Qd^YqVOjKX:m7*A2LX%Kn<{dtr",
    "ztkRA{=mrD>F]Cy7wSP%PU%>]MmdzH24oNb-&cd8ck%3HZ>&#zO/{A:eNp.cUAi9PbK77HYzuGV6lLS+y}{ru4e8LxazBG.Hs!gz",
    "&ZD5B>xP=:ol7PT0g*)qToCui=m2FLZVUp6DR6w:w?L5^f)J7GI>=R4*mxL6MqT$K(p%JbN&Gyg@tgFT^1zLo+e:=JVdVE$x.kW@",
    "MO-3.h]Wab]<0}H8Kg^@Nm5{AhU}eLrG$5cBMzw[9tu1E/10xDs(UdFk&$%dbq]ec",
);

                pub(super) const MU_PARAM_TABLE: &[(usize, usize, &str)] = &[
                    (2, 1, ENCODED_SMALL_FC64_M2_U1),
                    (2, 2, ENCODED_SMALL_FC64_M2_U2),
                    (2, 3, ENCODED_SMALL_FC64_M2_U3),
                    (2, 4, ENCODED_SMALL_FC64_M2_U4),
                    (2, 5, ENCODED_SMALL_FC64_M2_U5),
                    (3, 1, ENCODED_SMALL_FC64_M3_U1),
                    (3, 2, ENCODED_SMALL_FC64_M3_U2),
                    (3, 3, ENCODED_SMALL_FC64_M3_U3),
                    (3, 4, ENCODED_SMALL_FC64_M3_U4),
                    (3, 5, ENCODED_SMALL_FC64_M3_U5),
                    (4, 1, ENCODED_SMALL_FC64_M4_U1),
                    (4, 2, ENCODED_SMALL_FC64_M4_U2),
                    (4, 3, ENCODED_SMALL_FC64_M4_U3),
                    (4, 4, ENCODED_SMALL_FC64_M4_U4),
                    (4, 5, ENCODED_SMALL_FC64_M4_U5),
                    (5, 1, ENCODED_SMALL_FC64_M5_U1),
                    (5, 2, ENCODED_SMALL_FC64_M5_U2),
                    (5, 3, ENCODED_SMALL_FC64_M5_U3),
                    (5, 4, ENCODED_SMALL_FC64_M5_U4),
                    (5, 5, ENCODED_SMALL_FC64_M5_U5),
                    (6, 1, ENCODED_SMALL_FC64_M6_U1),
                    (6, 2, ENCODED_SMALL_FC64_M6_U2),
                    (6, 3, ENCODED_SMALL_FC64_M6_U3),
                    (6, 4, ENCODED_SMALL_FC64_M6_U4),
                    (6, 5, ENCODED_SMALL_FC64_M6_U5),
                    (7, 1, ENCODED_SMALL_FC64_M7_U1),
                    (7, 2, ENCODED_SMALL_FC64_M7_U2),
                    (7, 3, ENCODED_SMALL_FC64_M7_U3),
                    (7, 4, ENCODED_SMALL_FC64_M7_U4),
                    (7, 5, ENCODED_SMALL_FC64_M7_U5),
                    (8, 1, ENCODED_SMALL_FC64_M8_U1),
                    (8, 2, ENCODED_SMALL_FC64_M8_U2),
                    (8, 3, ENCODED_SMALL_FC64_M8_U3),
                    (8, 4, ENCODED_SMALL_FC64_M8_U4),
                    (8, 5, ENCODED_SMALL_FC64_M8_U5),
                ];
            }

            use crate::{
                problem::{GameState, Input, AI},
                simd::AlignedF32x16,
            };
            use itertools::Itertools as _;
            use network_params_by_mu::MU_PARAM_TABLE;
            use std::arch::x86_64::*;

            #[derive(Debug, Clone, Copy)]
            pub struct ValueNN {
                w_in: [__m512; 64],
                w_mid: [[__m512; 16]; 3],
                w_out: [f32; 16],
                b_in: __m512,
                b_mid: [__m512; 3],
                b_out: f32,
            }

            impl ValueNN {
                const OUT_SCALE: f32 = 1e5;
                const PARAM_FLOAT_COUNT: usize = 1873;

                #[target_feature(enable = "avx512f")]
                pub unsafe fn new(m: usize, u: usize) -> Self {
                    let encoded = find_encoded_params(m, u);
                    let flat = decode_z85_f32(encoded, Self::PARAM_FLOAT_COUNT);
                    unsafe { Self::from_flat_params(&flat) }
                }

                #[target_feature(enable = "avx512f")]
                unsafe fn from_flat_params(flat: &[f32]) -> Self {
                    let mut idx = 0;
                    let mut next = |n: usize| {
                        let start = idx;
                        idx += n;
                        &flat[start..idx]
                    };

                    let w1_s = next(16 * 64);
                    let b1_s = next(16);
                    let w2_s = next(16 * 16);
                    let b2_s = next(16);
                    let w3_s = next(16 * 16);
                    let b3_s = next(16);
                    let w4_s = next(16 * 16);
                    let b4_s = next(16);
                    let w5_s = next(16);
                    let b5_s = next(1);
                    assert_eq!(idx, Self::PARAM_FLOAT_COUNT);

                    let mut w_in = [AlignedF32x16::ZERO; 64];
                    let mut w_mid = [[AlignedF32x16::ZERO; 16]; 3];
                    let mut w_out = [0.0; 16];
                    let mut b_in = AlignedF32x16::ZERO;
                    let mut b_mid = [AlignedF32x16::ZERO; 3];

                    for i in 0..64 {
                        for j in 0..16 {
                            w_in[i][j] = w1_s[j * 64 + i];
                        }
                    }

                    for i in 0..16 {
                        for j in 0..16 {
                            w_mid[0][i][j] = w2_s[j * 16 + i];
                            w_mid[1][i][j] = w3_s[j * 16 + i];
                            w_mid[2][i][j] = w4_s[j * 16 + i];
                        }
                    }

                    for i in 0..16 {
                        w_out[i] = w5_s[i];
                    }

                    for i in 0..16 {
                        b_in[i] = b1_s[i];
                        b_mid[0][i] = b2_s[i];
                        b_mid[1][i] = b3_s[i];
                        b_mid[2][i] = b4_s[i];
                    }

                    let b_out = b5_s[0];

                    Self {
                        w_in: w_in.map(|x| x.load()),
                        w_mid: w_mid.map(|layer| layer.map(|x| x.load())),
                        w_out,
                        b_in: b_in.load(),
                        b_mid: b_mid.map(|x| x.load()),
                        b_out,
                    }
                }

                #[target_feature(enable = "avx512f")]
                pub unsafe fn forward(&self, x: &[f32; 64]) -> f32 {
                    let zero = _mm512_setzero_ps();

                    let mut y = self.b_in;

                    for i in 0..64 {
                        let w = self.w_in[i];
                        let x_i = _mm512_set1_ps(x[i]);
                        y = _mm512_fmadd_ps(w, x_i, y);
                    }

                    let x = _mm512_max_ps(y, zero);
                    let mut x = AlignedF32x16::from(x);

                    for layer in 0..3 {
                        let mut y = self.b_mid[layer];
                        let layer = &self.w_mid[layer];

                        for i in 0..16 {
                            let w = layer[i];
                            let x_i = _mm512_set1_ps(x[i]);
                            y = _mm512_fmadd_ps(w, x_i, y);
                        }

                        let y = _mm512_max_ps(y, zero);
                        x = AlignedF32x16::from(y);
                    }

                    let mut y = self.b_out;

                    for i in 0..16 {
                        y += self.w_out[i] * x[i];
                    }

                    y * Self::OUT_SCALE
                }
            }

            fn find_encoded_params(m: usize, u: usize) -> &'static str {
                for &(em, eu, data) in MU_PARAM_TABLE {
                    if em == m && eu == u {
                        return data;
                    }
                }

                unreachable!("parameters not found for m={}, u={}", m, u);
            }

            fn decode_z85_f32(text: &str, expected_f32: usize) -> Vec<f32> {
                let expected_bytes = expected_f32 * 4;
                let bytes = decode_z85_bytes(text);
                assert_eq!(bytes.len(), expected_bytes);

                let mut out = Vec::with_capacity(expected_f32);
                for chunk in bytes.chunks_exact(4) {
                    out.push(f32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]));
                }
                out
            }

            fn decode_z85_bytes(text: &str) -> Vec<u8> {
                const Z85_ALPHABET: &[u8; 85] =
        b"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.-:+=^!/*?&<>()[]{}@%$#";

                let mut bytes = Vec::with_capacity(text.len() * 4 / 5);
                let mut chunk = [0u32; 5];
                let mut chunk_len = 0usize;

                for b in text.bytes() {
                    if b.is_ascii_whitespace() {
                        continue;
                    }
                    let digit = Z85_ALPHABET
                        .iter()
                        .position(|&c| c == b)
                        .unwrap_or_else(|| panic!("invalid z85 char: {}", b));
                    chunk[chunk_len] = digit as u32;
                    chunk_len += 1;
                    if chunk_len == 5 {
                        let mut value: u32 = 0;
                        for d in chunk {
                            value = value * 85 + d;
                        }
                        bytes.extend_from_slice(&value.to_be_bytes());
                        chunk_len = 0;
                    }
                }
                assert_eq!(chunk_len, 0, "z85 text length must be multiple of 5");
                bytes
            }

            pub struct NNInput {
                inputs: [f32; 64],
                player_count: usize,
            }

            impl NNInput {
                pub fn new(input: &Input) -> Self {
                    let mut inputs = [0.0; 64];

                    inputs[59] = input.max_level as f32 / Input::MAX_LEVEL as f32;
                    inputs[60] = input.player_count as f32 / Input::MAX_PLAYERS as f32;
                    inputs[63] = input.std_dev_value as f32 * 1e-4;

                    Self {
                        inputs,
                        player_count: input.player_count,
                    }
                }

                pub fn set_ai_params(&mut self, ai: &[&AI]) {
                    // AIのパラメータ
                    for player_i in 1..self.player_count {
                        let index = (player_i - 1) * 5 + 24;
                        let ai = ai[player_i];
                        self.inputs[index] = ai.weights[0] as f32;
                        self.inputs[index + 1] = ai.weights[1] as f32;
                        self.inputs[index + 2] = ai.weights[2] as f32;
                        self.inputs[index + 3] = ai.weights[3] as f32;
                        self.inputs[index + 4] = ai.eps as f32;
                    }
                }

                #[target_feature(enable = "bmi1,bmi2,popcnt")]
                pub unsafe fn set_game_state(&mut self, game_state: &GameState) {
                    // スコア
                    for player_i in 0..self.player_count {
                        self.inputs[player_i] = game_state.scores[player_i] as f32 * 1e-4;
                    }

                    // マス数
                    for player_i in 0..self.player_count {
                        let count = unsafe { game_state.territory_bits[player_i].popcount() };
                        self.inputs[player_i + 8] = count as f32 * 1e-1;
                    }

                    // 到達可能マス数
                    for player_i in 0..self.player_count {
                        let count = unsafe { game_state.get_legal_moves(player_i).popcount() };
                        self.inputs[player_i + 16] = count as f32 * 1e-1;
                    }

                    self.inputs[61] = game_state.case_score() as f32 * 1e-5;
                    self.inputs[62] = game_state.turn as f32 / Input::MAX_TURNS as f32;
                }

                pub fn get_inputs(&self) -> &[f32; 64] {
                    &self.inputs
                }

                #[allow(dead_code)]
                pub fn print(&self, game_state: &GameState) {
                    assert_eq!(game_state.turn, Input::MAX_TURNS);

                    let mut row = self.inputs.to_vec();
                    let normalized_score = game_state.case_score() as f32 * 1e-5;
                    row.push(normalized_score);
                    eprintln!("{}", row.iter().join(","));
                }
            }

            // Rust側推論手順:
            // 1) 各層 y = W * x + b
            // 2) 最終層以外は ReLU を適用
        }
        mod thompson {
            use rand::Rng;
            use std::arch::x86_64::*;

            use crate::{
                problem::Input,
                simd::{AlignedF32x16, AlignedU32x16, XorShift32StarAvx512},
                solver::main_solver::{
                    articulation::ConnectionCheckerBitBoard,
                    monte_carlo::{PlayerAI, PlayerParam},
                },
                util::ChangeMinMax,
            };

            pub struct PlayerAICandidates<'a> {
                player_ai_set: Vec<PlayerAI<'a>>,
                predicted_normal_means: Vec<NormalMeanKnownVar>,
                estimated_means: [AlignedF32x16; 32],
                // 予測分散 (σ^2 + τ^2) の平方根
                estimated_std_devs: [AlignedF32x16; 32],
                xorshift: [XorShift32StarAvx512; 4],
            }

            impl<'a> PlayerAICandidates<'a> {
                const CANDIDATE_COUNT: usize = 512;

                pub fn generate(
                    input: &Input,
                    predicted_score: f64,
                    player_params: &PlayerParam,
                    articulation_checker: &'a ConnectionCheckerBitBoard,
                    rng: &mut impl Rng,
                ) -> Self {
                    let player_ai_set = (0..Self::CANDIDATE_COUNT)
                        .map(|_| {
                            PlayerAI::new_random(input, player_params, articulation_checker, rng)
                        })
                        .collect();
                    let predicted_normal_means = vec![
                        NormalMeanKnownVar::new(
                            predicted_score,
                            predicted_score * 0.5,
                            predicted_score * 0.1
                        );
                        Self::CANDIDATE_COUNT
                    ];
                    let mut estimated_means = [AlignedF32x16::ZERO; 32];
                    let mut estimated_std_devs = [AlignedF32x16::ZERO; 32];

                    for i in 0..Self::CANDIDATE_COUNT {
                        let posterior = predicted_normal_means[i].posterior();
                        estimated_means[i / 16][i % 16] = posterior.mu as f32;
                        estimated_std_devs[i / 16][i % 16] = posterior.predictive_var.sqrt() as f32;
                    }

                    let xorshift = [
                        XorShift32StarAvx512::new(rng.random()),
                        XorShift32StarAvx512::new(rng.random()),
                        XorShift32StarAvx512::new(rng.random()),
                        XorShift32StarAvx512::new(rng.random()),
                    ];

                    Self {
                        player_ai_set,
                        predicted_normal_means,
                        estimated_means,
                        estimated_std_devs,
                        xorshift,
                    }
                }

                #[target_feature(enable = "avx512f,fma")]
                pub unsafe fn choose(&mut self) -> (&PlayerAI<'a>, usize) {
                    let mut indices = [
                        AlignedU32x16([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
                            .load(),
                        AlignedU32x16([
                            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
                        ])
                        .load(),
                        AlignedU32x16([
                            32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
                        ])
                        .load(),
                        AlignedU32x16([
                            48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
                        ])
                        .load(),
                    ];
                    let mut max_values = [_mm512_set1_ps(f32::NEG_INFINITY); 4];
                    let mut max_indices = [_mm512_set1_epi32(0); 4];
                    let sixtyfour = _mm512_set1_epi32(64);
                    let six = _mm512_set1_ps(6.0);
                    let mut index = 0;

                    while index < Self::CANDIDATE_COUNT {
                        // 4個ずつ並列に実行することでレイテンシを隠蔽する
                        let mut normals = [_mm512_setzero_ps(); 4];

                        // [0, 1) 一様分布を12回足すことで正規分布を近似
                        for _ in 0..12 {
                            for i in 0..4 {
                                let uniform = unsafe { self.xorshift[i].next_f32x16() };
                                normals[i] = _mm512_add_ps(normals[i], uniform);
                            }
                        }

                        for i in 0..4 {
                            // 平均を0にする
                            normals[i] = _mm512_sub_ps(normals[i], six);

                            // スケーリング
                            let mu = self.estimated_means[index / 16 + i].load();
                            let std_dev = self.estimated_std_devs[index / 16 + i].load();

                            normals[i] = _mm512_fmadd_ps(normals[i], std_dev, mu);

                            // 最大値とインデックスの更新
                            let mask = _mm512_cmp_ps_mask(normals[i], max_values[i], _CMP_GT_OQ);
                            max_values[i] = _mm512_mask_mov_ps(max_values[i], mask, normals[i]);
                            max_indices[i] =
                                _mm512_mask_mov_epi32(max_indices[i], mask, indices[i]);

                            // インデックスを進める
                            indices[i] = _mm512_add_epi32(indices[i], sixtyfour);
                        }

                        index += 64;
                    }

                    // 最大値を求める
                    let mut max_value = _mm512_set1_ps(f32::NEG_INFINITY);
                    let mut max_index = _mm512_set1_epi32(0);

                    for i in 0..4 {
                        let mask = _mm512_cmp_ps_mask(max_value, max_values[i], _CMP_LT_OQ);
                        max_value = _mm512_mask_mov_ps(max_value, mask, max_values[i]);
                        max_index = _mm512_mask_mov_epi32(max_index, mask, max_indices[i]);
                    }

                    let max_values = AlignedF32x16::from(max_value);
                    let max_indices = AlignedU32x16::from(max_index);

                    let mut max_value = f32::NEG_INFINITY;
                    let mut max_index = 0;

                    for i in 0..16 {
                        if max_value.change_max(max_values[i]) {
                            max_index = max_indices[i] as usize;
                        }
                    }

                    (&self.player_ai_set[max_index], max_index)
                }

                pub fn update(&mut self, index: usize, observed_score: f64) {
                    let posterior = self.predicted_normal_means[index].update_one(observed_score);
                    self.estimated_means[index / 16][index % 16] = posterior.mu as f32;
                    self.estimated_std_devs[index / 16][index % 16] =
                        posterior.predictive_var.sqrt() as f32;
                }
            }

            /// 正規分布（平均が未知、分散が既知）に対するベイズ更新。
            ///
            /// モデル:
            ///   観測: x_i | μ ~ Normal(μ, σ^2)   （σ^2 は既知）
            ///   事前: μ ~ Normal(μ0, τ0^2)
            ///
            /// 共役性により、事後も正規分布:
            ///   μ | x_{1:n} ~ Normal(μ_n, τ_n^2)
            ///
            /// 更新式（バッチ）:
            ///   τ_n^2 = 1 / (1/τ0^2 + n/σ^2)
            ///   μ_n   = τ_n^2 * ( μ0/τ0^2 + n * x̄ / σ^2 )
            ///
            /// 逐次更新（1点ずつ）も同等の結果になります。
            #[derive(Debug, Clone, Copy)]
            pub struct NormalMeanKnownVar {
                /// 観測分散 σ^2（既知、正）
                sigma2: f64,
                /// 現在の μ の事後平均
                mu: f64,
                /// 現在の μ の事後分散 τ^2
                tau2: f64,
                /// 取り込んだ観測数（任意の管理用）
                n: usize,
            }

            /// 事後の要約（必要な値だけ返す）
            #[derive(Debug, Clone, Copy)]
            pub struct Posterior {
                /// μ の事後平均
                pub mu: f64,
                #[allow(dead_code)]
                /// μ の事後分散
                pub tau2: f64,
                /// 次の観測 x の予測分散（σ^2 + τ^2）
                pub predictive_var: f64,
            }

            impl NormalMeanKnownVar {
                /// 事前分布と既知分散から初期化する。
                ///
                /// ## Params
                /// - `mu0`: 事前平均
                /// - `tau0`: 事前分散の平方根（標準偏差）
                /// - `sigma`: 観測分散の平方根（標準偏差）
                pub fn new(mu0: f64, tau0: f64, sigma: f64) -> Self {
                    let tau0_2 = tau0 * tau0;
                    let sigma2 = sigma * sigma;

                    Self {
                        sigma2,
                        mu: mu0,
                        tau2: tau0_2,
                        n: 0,
                    }
                }

                /// 観測を1点だけ逐次取り込む更新（精度形式）。
                pub fn update_one(&mut self, x: f64) -> Posterior {
                    let prior_prec = 1.0 / self.tau2;
                    let like_prec = 1.0 / self.sigma2;

                    let post_prec = prior_prec + like_prec;
                    let post_tau2 = 1.0 / post_prec;

                    let post_mu = post_tau2 * (prior_prec * self.mu + like_prec * x);

                    self.mu = post_mu;
                    self.tau2 = post_tau2;
                    self.n += 1;

                    self.posterior()
                }

                /// 観測を1点だけ逐次取り込む更新（カルマンフィルタ風の書き方）。
                ///
                /// K = τ^2 / (τ^2 + σ^2) を「観測にどれだけ寄せるか」の係数として、
                ///   μ <- μ + K (x - μ)
                ///   τ^2 <- (1 - K) τ^2
                /// と更新する。update_one と数学的に等価。
                #[allow(dead_code)]
                pub fn update_one_kalman(&mut self, x: f64) -> Posterior {
                    let k = self.tau2 / (self.tau2 + self.sigma2);
                    self.mu = self.mu + k * (x - self.mu);
                    self.tau2 = (1.0 - k) * self.tau2;
                    self.n += 1;
                    self.posterior()
                }

                /// 現在の事後を要約して返す。
                pub fn posterior(&self) -> Posterior {
                    Posterior {
                        mu: self.mu,
                        tau2: self.tau2,
                        // 予測分布: x_new | data ~ Normal(mu, σ^2 + τ^2)
                        predictive_var: self.sigma2 + self.tau2,
                    }
                }

                /// 観測回数
                #[allow(dead_code)]
                pub fn n(&self) -> usize {
                    self.n
                }
            }
        }

        use crate::{
            estimator::ActionHistory,
            problem::{Input, Judge, AI},
            solver::main_solver::{
                articulation::ConnectionCheckerBitBoard,
                network::{NNInput, ValueNN},
                thompson::PlayerAICandidates,
            },
        };
        use itertools::Itertools;
        use rand::{Rng, SeedableRng};
        use rand_pcg::Pcg64Mcg;
        use std::time::Duration;

        #[target_feature(enable = "bmi1,bmi2,popcnt")]
        pub unsafe fn solve(input: &Input, judge: &mut impl Judge) {
            let mut game_state = input.get_init_game_state();
            let mut rng = Pcg64Mcg::from_os_rng();
            let mut estimated_ais = vec![vec![]; input.player_count];
            let mut action_history = ActionHistory::new();
            let value_nn = unsafe { ValueNN::new(input.player_count, input.max_level) };
            let player_params = monte_carlo::PlayerParam::from_env();
            let connection_checker = ConnectionCheckerBitBoard::new();

            let estimated_score = estimate_score(input, &value_nn, &mut rng);
            let mut player_ai_candidates = PlayerAICandidates::generate(
                input,
                estimated_score,
                &player_params,
                &connection_checker,
                &mut rng,
            );

            for turn in 0..Input::MAX_TURNS {
                eprintln!("[Turn {}]", turn);

                if ai_estimation::should_update_estimation(game_state.turn) {
                    ai_estimation::update_estimated_ais(input, &action_history, &mut estimated_ais);
                }

                let mut nn_in = NNInput::new(input);
                let mut ais = vec![&AI::DUMMY];
                for player_i in 1..input.player_count {
                    ais.push(&estimated_ais[player_i][0]);
                }
                nn_in.set_ai_params(&ais);
                unsafe { nn_in.set_game_state(&game_state) };
                let pred_score = unsafe { value_nn.forward(nn_in.get_inputs()) };
                eprintln!("pred_score: {}", pred_score);

                let action = if input.since.elapsed() <= Duration::from_millis(1990) {
                    unsafe {
                        monte_carlo::choose_action(
                            input,
                            &game_state,
                            &estimated_ais,
                            &value_nn,
                            &mut player_ai_candidates,
                            &mut rng,
                        )
                    }
                } else {
                    let legal_actions = game_state.get_legal_moves(0);
                    let action = unsafe { legal_actions.choose(&mut rng).unwrap() };
                    action
                };

                let mut response = judge.submit_action(input, action);
                std::mem::swap(&mut game_state, &mut response.game_state);

                // 尤度計算は「行動選択前」の局面を前提にする
                action_history.push(input, &response.game_state, &response.actions);
            }
        }

        fn estimate_score(input: &Input, value_nn: &ValueNN, rng: &mut impl Rng) -> f64 {
            const SAMPLE_COUNT: usize = 100;
            let mut nn_in = NNInput::new(input);
            unsafe { nn_in.set_game_state(&input.get_init_game_state()) };

            let mut sum = 0.0;
            let mut ais = vec![AI::DUMMY; input.player_count];

            for _ in 0..SAMPLE_COUNT {
                for player_i in 1..input.player_count {
                    let ai = AI::new_random(player_i, rng);
                    ais[player_i] = ai;
                }

                let ais = ais.iter().map(|ai| ai).collect_vec();
                nn_in.set_ai_params(&ais);
                sum += unsafe { value_nn.forward(nn_in.get_inputs()) as f64 };
            }

            sum / SAMPLE_COUNT as f64
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

fn main() {
    let input = Input::read();
    let mut judge = problem::ActualJudge::new();
    //let mut judge = problem::MockJudge::read(&input, false);
    unsafe {
        solver::main_solver::solve(&input, &mut judge);
    }
}

