mod common {
    use std::{collections::VecDeque, fmt::Display, time::Instant};

    use itertools::Itertools;
    use proconio::{input, marker::Chars};
    use rand::Rng;

    use crate::{common::grid::DIRECTIONS, ChangeMinMax};

    use self::grid::{Coord, CoordIndex, Map2d};

    #[macro_export]
    macro_rules! stopwatch {
        ($ex:expr, $s:expr) => {{
            let since = std::time::Instant::now();
            let res = $ex;
            let until = std::time::Instant::now();
            let duration = (until - since).as_secs_f64();
            eprintln!("{}: {} sec", $s, duration);
            res
        }};
    }

    #[allow(dead_code)]
    pub mod grid {
        use std::{
            fmt::Display,
            ops::{Add, AddAssign, Index, IndexMut},
            slice::{Iter, IterMut},
        };

        #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Default, Hash)]
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

            pub const fn to_index(&self, size: usize) -> usize {
                self.row as usize * size + self.col as usize
            }

            pub const fn dist(&self, other: &Self) -> usize {
                Self::dist_1d(self.row, other.row) + Self::dist_1d(self.col, other.col)
            }

            const fn dist_1d(x0: u8, x1: u8) -> usize {
                (x0 as i64 - x1 as i64).abs() as usize
            }

            pub const fn to_coord_index(&self, size: usize) -> CoordIndex {
                CoordIndex((self.row() * size + self.col()) as u16)
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
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

        impl Display for Coord {
            fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
                write!(f, "({}, {})", self.row(), self.col())
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Default)]
        pub struct CoordIndex(pub u16);

        impl CoordIndex {
            pub const fn to_coord(&self, size: usize) -> Coord {
                let row = self.0 as usize / size;
                let col = self.0 as usize - row * size;
                Coord::new(row, col)
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
            pub map: Vec<T>,
        }

        impl<T> Map2d<T> {
            pub fn new(map: Vec<T>, size: usize) -> Self {
                debug_assert!(size * size == map.len());
                Self { size, map }
            }

            pub fn iter(&self) -> Iter<T> {
                self.map.iter()
            }

            pub fn iter_mut(&mut self) -> IterMut<T> {
                self.map.iter_mut()
            }

            pub unsafe fn get_unchecked(&self, index: CoordIndex) -> &T {
                self.map.get_unchecked(index.0 as usize)
            }

            pub unsafe fn get_unchecked_mut(&mut self, index: CoordIndex) -> &mut T {
                self.map.get_unchecked_mut(index.0 as usize)
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

        impl<T> Index<CoordIndex> for Map2d<T> {
            type Output = T;

            #[inline]
            fn index(&self, index: CoordIndex) -> &Self::Output {
                unsafe { self.map.get_unchecked(index.0 as usize) }
            }
        }

        impl<T> IndexMut<CoordIndex> for Map2d<T> {
            fn index_mut(&mut self, index: CoordIndex) -> &mut Self::Output {
                unsafe { self.map.get_unchecked_mut(index.0 as usize) }
            }
        }
    }

    #[derive(Debug, Clone)]
    pub struct Input {
        pub map_size: usize,
        pub map: Map2d<[Option<Coord>; 4]>,
        pub map_index: Map2d<[Option<CoordIndex>; 4]>,
        pub dirty: Map2d<i64>,
        pub dists: Map2d<Map2d<i64>>,
        pub config: Config,
        pub since: Instant,
    }

    impl Input {
        pub const START: Coord = Coord::new(0, 0);

        pub fn read_input() -> Self {
            input! {
                map_size: usize,
                h: [Chars; map_size - 1],
                v: [Chars; map_size],
                d: [i64; map_size * map_size]
            }

            let since = Instant::now();
            let mut map = Map2d::new(vec![[None; 4]; map_size * map_size], map_size);
            let mut map_index = Map2d::new(vec![[None; 4]; map_size * map_size], map_size);

            // bulid map
            for row in 0..(map_size - 1) {
                for col in 0..map_size {
                    let c0 = Coord::new(row, col);
                    let c1 = Coord::new(row + 1, col);
                    let c0_index = c0.to_coord_index(map_size);
                    let c1_index = c1.to_coord_index(map_size);

                    if h[row][col] == '0' {
                        map[c0][grid::D] = Some(c1);
                        map[c1][grid::U] = Some(c0);
                        map_index[c0_index][grid::D] = Some(c1_index);
                        map_index[c1_index][grid::U] = Some(c0_index);
                    }
                }
            }

            for row in 0..map_size {
                for col in 0..(map_size - 1) {
                    let c0 = Coord::new(row, col);
                    let c1 = Coord::new(row, col + 1);
                    let c0_index = c0.to_coord_index(map_size);
                    let c1_index = c1.to_coord_index(map_size);

                    if v[row][col] == '0' {
                        map[c0][grid::R] = Some(c1);
                        map[c1][grid::L] = Some(c0);
                        map_index[c0_index][grid::R] = Some(c1_index);
                        map_index[c1_index][grid::L] = Some(c0_index);
                    }
                }
            }

            let dirty = Map2d::new(d, map_size);
            let dists = Self::get_dist_matrix(&map, map_size);
            let config = Config::read_config(map_size);

            Self {
                map_size,
                map,
                map_index,
                dirty,
                dists,
                since,
                config,
            }
        }

        fn get_dist_matrix(map: &Map2d<[Option<Coord>; 4]>, map_size: usize) -> Map2d<Map2d<i64>> {
            let mut queue = VecDeque::new();
            let mut all_dists = vec![];

            for sr in 0..map_size {
                for sc in 0..map_size {
                    let start = Coord::new(sr, sc);
                    let mut dists = Map2d::new(vec![i64::MAX; map_size * map_size], map_size);
                    queue.push_back(start);
                    dists[start] = 0;

                    while let Some(c) = queue.pop_front() {
                        let next_dist = dists[c] + 1;

                        for next in map[c] {
                            let Some(next) = next else {
                                continue;
                            };

                            if dists[next].change_min(next_dist) {
                                queue.push_back(next);
                            }
                        }
                    }

                    all_dists.push(dists);
                }
            }

            Map2d::new(all_dists, map_size)
        }
    }

    pub fn calc_tentative_score(input: &Input, path: &[CoordIndex]) -> i64 {
        unsafe { calc_tentative_score_inner(input, path) }
    }

    #[target_feature(
        enable = "aes,avx,avx2,bmi1,bmi2,fma,fxsr,pclmulqdq,popcnt,rdrand,rdseed,sse,sse2,sse4.1,sse4.2,ssse3,xsave,xsavec,xsaveopt,xsaves"
    )]
    unsafe fn calc_tentative_score_inner(input: &Input, path: &[CoordIndex]) -> i64 {
        static mut VIS_BUFFER: Vec<usize> = vec![];
        static mut DIST_BUFFER: Vec<usize> = vec![];
        static mut TRIANGLE_TABLE: Vec<u32> = vec![];

        assert!(path.len() >= 1);

        unsafe {
            if VIS_BUFFER.len() == 0 {
                VIS_BUFFER = vec![0; input.map_size * input.map_size];
            }

            if DIST_BUFFER.len() == 0 {
                DIST_BUFFER = vec![0; 100001];
            }

            // かけ算が重いので前計算する
            if TRIANGLE_TABLE.len() == 0 {
                for i in 0..=100000 {
                    TRIANGLE_TABLE.push(i * (i - 1) / 2);
                }
            }

            let mut score = 0;
            let mut turn = 0;
            let mut current = path[0];

            // 最終ターンを求める
            for (&c, d) in path.iter().zip(DIST_BUFFER.iter_mut()) {
                *d = *input.dists.get_unchecked(current).get_unchecked(c) as usize;
                turn += *d;
                *VIS_BUFFER.get_unchecked_mut(c.0 as usize) = turn;
                current = c;
            }

            // スコア計算
            for (&c, d) in path.iter().zip(DIST_BUFFER.iter()) {
                turn += d;
                let prev = VIS_BUFFER.get_unchecked_mut(c.0 as usize);
                let dt = turn - *prev;
                *prev = turn;
                score += (*input.dirty.get_unchecked(c) as u32 * TRIANGLE_TABLE.get_unchecked(dt))
                    as u64;
            }

            // score /= (turn / 2)
            score *= 2;
            score /= turn as u64;

            score as i64
        }
    }

    #[derive(Debug, Clone)]
    pub struct Output {
        dir: Vec<usize>,
    }

    impl Output {
        pub fn from_path(input: &Input, path: &[Coord]) -> Self {
            if path.len() == 0 {
                panic!("pathの長さが0です");
            }

            let first = *path.first().unwrap();
            let last = *path.last().unwrap();
            assert_eq!(
                first,
                Coord::new(0, 0),
                "pathは(0, 0)から始めなければなりませんが、{}が与えられました。",
                first
            );
            assert_eq!(
                last,
                Coord::new(0, 0),
                "pathは(0, 0)で終わらなければなりませんが、{}が与えられました。",
                last
            );

            let mut dir = vec![];

            for (&c0, &c1) in path.iter().tuple_windows() {
                if c0 == c1 {
                    continue;
                }

                let mut d = None;

                for dd in 0..4 {
                    if input.map[c0][dd] == Some(c1) {
                        d = Some(dd);
                    }
                }

                let d = d.expect(&format!("{}と{}は隣接マスではありません。", c0, c1));
                dir.push(d);
            }

            Self { dir }
        }
    }

    impl Display for Output {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            let s = self.dir.iter().map(|&d| DIRECTIONS[d]).collect::<String>();
            write!(f, "{s}")
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct Config {
        pub time_ratio: f64,
        rough_sa_neigh_ratio: [f64; Self::ROUGH_SA_NEIGH_LEN],
        detail_sa_neigh_ratio: [f64; Self::DETAIL_SA_NEIGH_LEN],
        pub thresholds: [i64; 5],
    }

    impl Config {
        const ROUGH_SA_NEIGH_LEN: usize = 3;
        const DETAIL_SA_NEIGH_LEN: usize = 5;

        pub fn read_config(map_size: usize) -> Self {
            let args = std::env::args().collect::<Vec<_>>();

            let time_ratio = Self::get_time_ratio(&args);
            let rough_sa_neigh_ratio = Self::get_rough_sa_neigh_ratio(&args);
            let detail_sa_neigh_ratio = Self::get_detail_sa_neigh_ratio(&args);
            let thresholds = Self::get_thresholds(map_size);
            let config = Self {
                time_ratio,
                rough_sa_neigh_ratio,
                detail_sa_neigh_ratio,
                thresholds,
            };

            eprintln!("{:?}", config);
            config
        }

        fn get_time_ratio(args: &[String]) -> f64 {
            let time_ratio = args.get(1).map(|s| s.parse().unwrap()).unwrap_or(1.0);
            time_ratio
        }

        fn get_rough_sa_neigh_ratio(args: &[String]) -> [f64; Self::ROUGH_SA_NEIGH_LEN] {
            let mut result = [0.0; Self::ROUGH_SA_NEIGH_LEN];

            result[0] = Self::parse(args, 2).unwrap_or(0.5);
            result[1] = Self::parse(args, 3).unwrap_or(0.5);
            result[2] = Self::parse(args, 4).unwrap_or(1.0);

            result
        }

        fn get_detail_sa_neigh_ratio(args: &[String]) -> [f64; Self::DETAIL_SA_NEIGH_LEN] {
            let mut result = [0.0; Self::DETAIL_SA_NEIGH_LEN];

            result[0] = Self::parse(args, 2 + Self::DETAIL_SA_NEIGH_LEN).unwrap_or(0.0);
            result[1] = Self::parse(args, 3 + Self::DETAIL_SA_NEIGH_LEN).unwrap_or(0.0);
            result[2] = Self::parse(args, 4 + Self::DETAIL_SA_NEIGH_LEN).unwrap_or(0.5);
            result[3] = Self::parse(args, 5 + Self::DETAIL_SA_NEIGH_LEN).unwrap_or(1.0);
            result[4] = Self::parse(args, 6 + Self::DETAIL_SA_NEIGH_LEN).unwrap_or(0.05);

            result
        }

        fn get_thresholds(map_size: usize) -> [i64; 5] {
            const THRESHOLD_N20: [i64; 5] = [600, 96, 24, 6, 0];
            const THRESHOLD_N30: [i64; 5] = [1001, 160, 36, 8, 0];
            const THRESHOLD_N40: [i64; 5] = [1001, 200, 45, 10, 0];

            let mut threshold = [0; 5];

            for i in 0..5 {
                let v = if map_size <= 30 {
                    let v0 = THRESHOLD_N20[i];
                    let v1 = THRESHOLD_N30[i];
                    let v = ((v0 as f64 * (30 - map_size) as f64
                        + v1 as f64 * (map_size - 20) as f64)
                        / 10.0)
                        .round() as i64;
                    v
                } else {
                    let v0 = THRESHOLD_N30[i];
                    let v1 = THRESHOLD_N40[i];
                    let v = ((v0 as f64 * (40 - map_size) as f64
                        + v1 as f64 * (map_size - 30) as f64)
                        / 10.0)
                        .round() as i64;
                    v
                };
                threshold[i] = v;
            }

            threshold
        }

        fn parse(args: &[String], index: usize) -> Option<f64> {
            args.get(index)?.parse().ok()
        }

        pub fn choose_rough_sa_neigh(&self, rng: &mut impl Rng) -> usize {
            Self::choose(&self.rough_sa_neigh_ratio, rng)
        }

        pub fn choose_detail_sa_neigh(&self, rng: &mut impl Rng) -> usize {
            Self::choose(&self.detail_sa_neigh_ratio, rng)
        }

        fn choose(probs: &[f64], rng: &mut impl Rng) -> usize {
            let mut prefix_sum = probs.iter().copied().collect_vec();

            for i in 1..prefix_sum.len() {
                prefix_sum[i] += prefix_sum[i - 1];
            }

            let max = *prefix_sum.last().unwrap();
            let v = rng.gen_range(0.0..max);

            for i in 0..prefix_sum.len() {
                if v <= prefix_sum[i] {
                    return i;
                }
            }

            return prefix_sum.len() - 1;
        }
    }
}
mod solvers {
    pub mod loop_insertion {
        mod annealing_detail {
            use std::time::Instant;

            use im_rc::HashMap;
            use itertools::Itertools;
            use rand::{seq::SliceRandom, Rng};

            use crate::{
                common::calc_tentative_score,
                common::grid::{Coord, CoordIndex, Map2d},
                common::Input,
                ChangeMinMax,
            };

            use super::path::to_path_grouped;

            pub fn to_path(input: &Input, path: Vec<CoordIndex>, duration: f64) -> Vec<Coord> {
                let since = Instant::now();
                let groups = to_path_grouped(input, &path);
                let state = State::new(input, path, groups);
                let until = Instant::now();
                let duration = duration - (until - since).as_secs_f64();
                let groups = annealing(&input, state, duration);
                groups
                    .path
                    .iter()
                    .flatten()
                    .map(|c| c.to_coord(input.map_size))
                    .collect_vec()
            }

            #[derive(Debug, Clone)]
            struct State {
                checkpoints: Vec<CoordIndex>,
                path: Vec<Vec<CoordIndex>>,
                visit_count: Map2d<i32>,
            }

            impl State {
                fn new(
                    input: &Input,
                    checkpoints: Vec<CoordIndex>,
                    path: Vec<Vec<CoordIndex>>,
                ) -> Self {
                    let mut visit_count = Map2d::with_default(input.map_size);

                    for &c in checkpoints[1..].iter() {
                        visit_count[c] += 1;
                    }

                    Self {
                        checkpoints,
                        path,
                        visit_count,
                    }
                }

                fn calc_score(&self, input: &Input) -> i64 {
                    static mut PATH: Vec<CoordIndex> = vec![];

                    unsafe {
                        PATH.clear();

                        for p in &self.path {
                            PATH.extend_from_slice(p);
                        }

                        calc_tentative_score(input, &PATH)
                    }
                }
            }

            fn annealing(input: &Input, initial_solution: State, duration: f64) -> State {
                if duration <= 0.0 {
                    return initial_solution;
                }

                let mut solution = initial_solution;
                let mut best_solution = solution.clone();
                let mut current_score = solution.calc_score(input);
                let mut best_score = current_score;
                let init_score = current_score;

                let mut all_iter = 0;
                let mut valid_iter = 0;
                let mut accepted_count = 0;
                let mut update_count = 0;
                let mut rng = rand_pcg::Pcg64Mcg::new(42);

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = current_score as f64 * 3e-5;
                let temp1 = temp0 * 1e-1;
                let mut inv_temp = 1.0 / temp0;

                let mut trials = HashMap::new();
                let mut accepted = HashMap::new();

                loop {
                    if (all_iter & ((1 << 4) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
                        inv_temp = 1.0 / temp;

                        if time >= 1.0 {
                            break;
                        }
                    }
                    all_iter += 1;

                    if all_iter % (1 << 8) == 0 {
                        change_start_point(&mut solution, &mut rng);
                    }

                    // 変形
                    let neigh_type = input.config.choose_detail_sa_neigh(&mut rng);

                    let neigh: Option<Box<dyn Neigh>> = match neigh_type {
                        0 => SinglePath::try_generate(input, &solution, &mut rng),
                        1 => DoublePath::try_generate(input, &solution, &mut rng),
                        2 => RemoveCheckpoint::try_generate(input, &solution, &mut rng),
                        3 => AddCheckpoint::try_generate(input, &solution, &mut rng),
                        4 => AddCheckpoint2::try_generate(input, &solution, &mut rng),
                        _ => unreachable!(),
                    };

                    let Some(mut neigh) = neigh else {
                        continue;
                    };
                    neigh.apply(input, &mut solution);

                    // スコア計算
                    let new_score = neigh.calc_score(input, &solution);
                    let score_diff = new_score - current_score;
                    *trials.entry(neigh.name()).or_insert(0) += 1;

                    if score_diff <= 0 || rng.gen_bool(f64::exp(-score_diff as f64 * inv_temp)) {
                        // 解の更新
                        current_score = new_score;
                        accepted_count += 1;
                        *accepted.entry(neigh.name()).or_insert(0) += 1;
                        neigh.postprocess(input, &mut solution);

                        if best_score.change_min(current_score) {
                            best_solution = solution.clone();
                            update_count += 1;
                        }
                    } else {
                        neigh.rollback(input, &mut solution);
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

                for key in trials.keys().sorted() {
                    let trials = trials.get(key).unwrap_or(&0);
                    let accepted = accepted.get(key).unwrap_or(&0);
                    eprintln!("[neigh] {:17} : {:6} / {:6}", key, accepted, trials);
                }

                best_solution
            }

            fn change_start_point(solution: &mut State, rng: &mut rand_pcg::Mcg128Xsl64) {
                solution.checkpoints.remove(0);
                let index = rng.gen_range(1..solution.checkpoints.len());
                solution.checkpoints[..index].reverse();
                solution.checkpoints[index..].reverse();
                solution.checkpoints.reverse();
                solution
                    .checkpoints
                    .insert(0, *solution.checkpoints.last().unwrap());

                solution.path.remove(0);
                solution.path[..index].reverse();
                solution.path[index..].reverse();
                solution.path.reverse();
                solution.path.insert(0, vec![solution.checkpoints[0]]);
            }

            fn dijkstra(
                input: &Input,
                start: CoordIndex,
                goal: CoordIndex,
                rng: &mut impl Rng,
            ) -> Vec<CoordIndex> {
                const INF: i32 = i32::MAX / 2;
                static mut BUFFER_DIST: Vec<i32> = vec![];
                static mut BUFFER_FROM: Vec<CoordIndex> = vec![];
                static mut QUEUE: Vec<(i32, CoordIndex)> = vec![];
                static mut STACK: Vec<CoordIndex> = vec![];
                static mut RANDS: Vec<i32> = vec![];
                static mut RAND_I: usize = 0;
                static mut DIST_MUL: Vec<i32> = vec![];

                unsafe {
                    if BUFFER_DIST.len() == 0 {
                        BUFFER_DIST = vec![INF; input.map_size * input.map_size];
                    }

                    if BUFFER_FROM.len() == 0 {
                        BUFFER_FROM = vec![CoordIndex::default(); input.map_size * input.map_size];
                    }

                    if RANDS.len() == 0 {
                        for _ in 0..10000 {
                            RANDS.push(rng.gen_range(1..=64) * rng.gen_range(1..=64));
                        }
                    }

                    let mut dist_mul = if DIST_MUL.len() == 0 {
                        let mut dist_mul: Map2d<i32> = Map2d::with_default(input.map_size);

                        for (&d, m) in input.dirty.iter().zip(dist_mul.iter_mut()) {
                            let d = (d as f64).log2() * 7.5;
                            *m = 100 - d.round() as i32;
                        }

                        dist_mul
                    } else {
                        let mut v = vec![];
                        std::mem::swap(&mut DIST_MUL, &mut v);
                        Map2d::new(v, input.map_size)
                    };

                    let mut temp_dist = vec![];
                    let mut temp_from = vec![];
                    std::mem::swap(&mut BUFFER_DIST, &mut temp_dist);
                    std::mem::swap(&mut BUFFER_FROM, &mut temp_from);
                    temp_dist.fill(INF);
                    let mut dists = Map2d::new(temp_dist, input.map_size);
                    let mut from = Map2d::new(temp_from, input.map_size);

                    // ヒープ使うより愚直にVecを使った方が速い
                    QUEUE.clear();
                    dists[start] = 0;
                    QUEUE.push((0, start));
                    STACK.push(start);

                    loop {
                        let best_i = QUEUE.iter().position_min_by_key(|&(d, _)| d).unwrap();

                        let (dist, c) = QUEUE.swap_remove(best_i);

                        if c == goal {
                            break;
                        }

                        if *dists.get_unchecked(c) < dist {
                            continue;
                        }

                        for &next in input.map_index.get_unchecked(c) {
                            let Some(next) = next else {
                                continue;
                            };
                            let next_cost =
                                dist + RANDS.get_unchecked(RAND_I) * dist_mul.get_unchecked(next);
                            RAND_I += 1;

                            if RAND_I >= RANDS.len() {
                                RAND_I -= RANDS.len();
                            }

                            if dists.get_unchecked_mut(next).change_min(next_cost) {
                                *from.get_unchecked_mut(next) = c;
                                STACK.push(next);
                                QUEUE.push((next_cost, next));
                            }
                        }
                    }

                    let mut path = vec![];
                    let mut current = goal;

                    while current != start {
                        path.push(current);
                        current = from[current];
                    }

                    while let Some(c) = STACK.pop() {
                        dists[c] = INF;
                    }

                    std::mem::swap(&mut BUFFER_DIST, &mut dists.map);
                    std::mem::swap(&mut BUFFER_FROM, &mut from.map);
                    std::mem::swap(&mut DIST_MUL, &mut dist_mul.map);

                    path.reverse();
                    path
                }
            }

            trait Neigh {
                fn apply(&mut self, input: &Input, state: &mut State);

                fn calc_score(&self, input: &Input, state: &State) -> i64;

                fn postprocess(&mut self, input: &Input, state: &mut State);

                fn rollback(&mut self, input: &Input, state: &mut State);

                fn name(&self) -> &'static str;
            }

            struct SinglePath {
                index: usize,
                new_path: Vec<CoordIndex>,
            }

            impl SinglePath {
                fn try_generate(
                    input: &Input,
                    state: &State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    let index = rng.gen_range(1..state.path.len());
                    let prev_c = state.checkpoints[index - 1];
                    let next_c = state.checkpoints[index];

                    if input.dists[prev_c][next_c] <= 1 {
                        return None;
                    }

                    let new_path = dijkstra(input, prev_c, next_c, rng);

                    Some(Box::new(SinglePath { index, new_path }))
                }
            }

            impl Neigh for SinglePath {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path, &mut state.path[self.index]);
                }

                fn calc_score(&self, input: &Input, state: &State) -> i64 {
                    state.calc_score(input)
                }

                fn postprocess(&mut self, _input: &Input, _state: &mut State) {
                    // do nothing
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path, &mut state.path[self.index]);
                }

                fn name(&self) -> &'static str {
                    "single path"
                }
            }

            struct DoublePath {
                index: usize,
                new_path_1: Vec<CoordIndex>,
                new_path_2: Vec<CoordIndex>,
            }

            impl DoublePath {
                fn try_generate(
                    input: &Input,
                    state: &State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    let index = rng.gen_range(1..(state.path.len() - 1));
                    let c0 = state.checkpoints[index - 1];
                    let c1 = state.checkpoints[index];
                    let c2 = state.checkpoints[index + 1];

                    let new_path_1 = dijkstra(input, c0, c1, rng);
                    let new_path_2 = dijkstra(input, c1, c2, rng);

                    Some(Box::new(DoublePath {
                        index,
                        new_path_1,
                        new_path_2,
                    }))
                }
            }

            impl Neigh for DoublePath {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                    std::mem::swap(&mut self.new_path_2, &mut state.path[self.index + 1]);
                }

                fn calc_score(&self, input: &Input, state: &State) -> i64 {
                    state.calc_score(input)
                }

                fn postprocess(&mut self, _input: &Input, _state: &mut State) {
                    // do nothing
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                    std::mem::swap(&mut self.new_path_2, &mut state.path[self.index + 1]);
                }

                fn name(&self) -> &'static str {
                    "double path"
                }
            }

            struct AddCheckpoint {
                index: usize,
                new_path_1: Vec<CoordIndex>,
                new_path_2: Vec<CoordIndex>,
                c: CoordIndex,
            }

            impl AddCheckpoint {
                fn try_generate(
                    input: &Input,
                    state: &State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    let index = rng.gen_range(1..(state.path.len() - 1));

                    if state.path[index].len() <= 1 {
                        return None;
                    }

                    let c0 = state.checkpoints[index - 1];
                    let c1 = *state.path[index][..(state.path[index].len() - 1)]
                        .choose(rng)
                        .unwrap();
                    let c2 = state.checkpoints[index];

                    let new_path_1 = dijkstra(input, c0, c1, rng);
                    let new_path_2 = dijkstra(input, c1, c2, rng);

                    Some(Box::new(AddCheckpoint {
                        index,
                        new_path_1,
                        new_path_2,
                        c: c1,
                    }))
                }
            }

            impl Neigh for AddCheckpoint {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                    state.path.insert(self.index + 1, self.new_path_2.clone());
                }

                fn calc_score(&self, input: &Input, state: &State) -> i64 {
                    state.calc_score(input)
                }

                fn postprocess(&mut self, _input: &Input, state: &mut State) {
                    state.checkpoints.insert(self.index, self.c);
                    state.visit_count[self.c] += 1;
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    state.path.remove(self.index + 1);
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                }

                fn name(&self) -> &'static str {
                    "add checkpoint"
                }
            }

            struct RemoveCheckpoint {
                index: usize,
                new_path_1: Vec<CoordIndex>,
                path_2: Vec<CoordIndex>,
                c: CoordIndex,
            }

            impl RemoveCheckpoint {
                fn try_generate(
                    input: &Input,
                    state: &State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    if state.path.len() <= 3 {
                        return None;
                    }

                    let index = rng.gen_range(1..(state.path.len() - 1));

                    let c0 = state.checkpoints[index - 1];
                    let c1 = state.checkpoints[index];
                    let c2 = state.checkpoints[index + 1];

                    if state.visit_count[c1] == 1 {
                        return None;
                    }

                    let new_path_1 = dijkstra(input, c0, c2, rng);
                    let path_2 = vec![]; //state.path.remove(index + 1);

                    Some(Box::new(RemoveCheckpoint {
                        index,
                        new_path_1,
                        path_2,
                        c: c1,
                    }))
                }
            }

            impl Neigh for RemoveCheckpoint {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    self.path_2 = state.path.remove(self.index + 1);
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                }

                fn calc_score(&self, input: &Input, state: &State) -> i64 {
                    state.calc_score(input)
                }

                fn postprocess(&mut self, _input: &Input, state: &mut State) {
                    state.checkpoints.remove(self.index);
                    state.visit_count[self.c] -= 1;
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                    state.path.insert(self.index + 1, self.path_2.clone());
                }

                fn name(&self) -> &'static str {
                    "remove checkpoint"
                }
            }

            struct AddCheckpoint2 {
                index: usize,
                new_path_1: Vec<CoordIndex>,
                new_path_2: Vec<CoordIndex>,
                c: CoordIndex,
            }

            impl AddCheckpoint2 {
                fn try_generate(
                    input: &Input,
                    state: &State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    unsafe {
                        static mut CANDIDATES: Vec<CoordIndex> = vec![];

                        let index = rng.gen_range(1..(state.path.len() - 1));

                        let c0 = state.checkpoints[index - 1];
                        let Some(&c1) = state.path[index].choose(rng) else {
                            return None;
                        };
                        let c2 = state.checkpoints[index];

                        const MAX_DIFF: isize = 3;
                        CANDIDATES.clear();
                        let c_orig = c1.to_coord(input.map_size);
                        let dist0 = &input.dists[c0];
                        let dist2 = &input.dists[c2];
                        let old_dist = input.dists[c0][c2];

                        for dr in -MAX_DIFF..=MAX_DIFF {
                            let row = c_orig.row().wrapping_add_signed(dr);

                            if row >= input.map_size {
                                continue;
                            }

                            let max_d = MAX_DIFF - dr.abs();

                            for dc in -max_d..=max_d {
                                let col = c_orig.col().wrapping_add_signed(dc);

                                if col >= input.map_size {
                                    continue;
                                }

                                let c = Coord::new(row, col).to_coord_index(input.map_size);

                                if c == c0 || c == c2 {
                                    continue;
                                }

                                let dist_diff =
                                    dist0.get_unchecked(c) + dist2.get_unchecked(c) - old_dist;

                                if dist_diff >= 2 {
                                    continue;
                                }

                                CANDIDATES.push(c);
                            }
                        }

                        let Some(&new_c) = CANDIDATES.choose(rng) else {
                            return None;
                        };

                        let new_path_1 = dijkstra(input, c0, new_c, rng);
                        let new_path_2 = dijkstra(input, new_c, c2, rng);

                        Some(Box::new(AddCheckpoint2 {
                            index,
                            new_path_1,
                            new_path_2,
                            c: new_c,
                        }))
                    }
                }
            }

            impl Neigh for AddCheckpoint2 {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                    state.path.insert(self.index + 1, self.new_path_2.clone());
                }

                fn calc_score(&self, input: &Input, state: &State) -> i64 {
                    state.calc_score(input)
                }

                fn postprocess(&mut self, _input: &Input, state: &mut State) {
                    state.checkpoints.insert(self.index, self.c);
                    state.visit_count[self.c] += 1;
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    state.path.remove(self.index + 1);
                    std::mem::swap(&mut self.new_path_1, &mut state.path[self.index]);
                }

                fn name(&self) -> &'static str {
                    "add checkpoint 2"
                }
            }
        }
        mod annealing_rough {
            use std::{
                cmp::Reverse,
                collections::{HashMap, VecDeque},
            };

            use crate::{
                common::calc_tentative_score,
                common::grid::{CoordIndex, Map2d},
                common::Input,
                ChangeMinMax,
            };
            use itertools::Itertools;
            use rand::{seq::SliceRandom, Rng};

            pub fn annealing<'a>(
                env: &impl Env<'a>,
                initial_solution: State,
                duration: f64,
            ) -> State {
                if duration <= 0.0 {
                    return initial_solution;
                }

                let mut solution = initial_solution.clone();
                let mut best_solution = solution.clone();
                let mut current_score = env.calc_score(&solution);
                let mut best_score = current_score;
                let init_score = current_score;

                let mut all_iter = 0;
                let mut valid_iter = 0;
                let mut accepted_count = 0;
                let mut update_count = 0;
                let mut rng = rand_pcg::Pcg64Mcg::new(42);

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = (current_score as f64).sqrt() * 3e-1;
                let temp1 = temp0 * 3e-2;
                let mut inv_temp = 1.0 / temp0;

                let mut trials = HashMap::new();
                let mut accepted = HashMap::new();

                loop {
                    if (all_iter & ((1 << 4) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
                        inv_temp = 1.0 / temp;

                        if time >= 1.0 {
                            break;
                        }
                    }

                    all_iter += 1;

                    if all_iter % (1 << 8) == 0 {
                        change_start_point(&mut solution, &mut rng);
                    }

                    // 変形
                    let neigh_type = env.input().config.choose_rough_sa_neigh(&mut rng);

                    let neigh = match neigh_type {
                        0 => MoveSingle::try_generate(&env.input(), &mut solution, &mut rng),
                        1 => TwoOpt::try_generate(&env.input(), &mut solution, &mut rng),
                        2 => MoveMulti::try_generate(&env.input(), &mut solution, &mut rng),
                        _ => unreachable!(),
                    };

                    let Some(mut neigh) = neigh else {
                        continue;
                    };
                    neigh.apply(&env.input(), &mut solution);

                    let new_score = env.calc_score(&solution);
                    let score_diff = new_score - current_score;
                    *trials.entry(neigh.name()).or_insert(0) += 1;

                    if score_diff <= 0 || rng.gen_bool(f64::exp(-score_diff as f64 * inv_temp)) {
                        // 解の更新
                        current_score = new_score;
                        accepted_count += 1;
                        neigh.postprocess(&env.input(), &mut solution);
                        *accepted.entry(neigh.name()).or_insert(0) += 1;

                        if best_score.change_min(current_score) {
                            best_solution = solution.clone();
                            update_count += 1;
                        }
                    } else {
                        neigh.rollback(&env.input(), &mut solution);
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

                for key in trials.keys().sorted() {
                    let trials = trials.get(key).unwrap_or(&0);
                    let accepted = accepted.get(key).unwrap_or(&0);
                    eprintln!("[neigh] {:10} : {:6} / {:6}", key, accepted, trials);
                }

                eprintln!("");

                best_solution
            }

            fn change_start_point(solution: &mut State, rng: &mut rand_pcg::Mcg128Xsl64) {
                solution.path.pop();
                let index = rng.gen_range(1..solution.path.len());
                solution.path[..index].reverse();
                solution.path[index..].reverse();
                solution.path.reverse();
                solution.path.push(solution.path[0]);
            }

            pub trait Env<'a> {
                fn input(&self) -> &'a Input;

                fn calc_score(&self, state: &State) -> i64;
            }

            pub struct AreaEnv<'a> {
                input: &'a Input,
                areas: Map2d<i64>,
            }

            impl<'a> AreaEnv<'a> {
                pub fn new(input: &'a Input, targets: &[CoordIndex]) -> Self {
                    let mut counts =
                        Map2d::new(vec![1; input.map_size * input.map_size], input.map_size);

                    for &c in targets {
                        counts[c] = 0;
                    }

                    for &c in targets {
                        counts[c] += 1;
                    }

                    let mut targets = targets.iter().copied().collect_vec();
                    targets.sort_unstable();
                    targets.dedup();
                    targets.sort_unstable_by_key(|&c| Reverse(input.dirty[c]));

                    let mut queue = VecDeque::new();
                    let mut group: Map2d<Option<CoordIndex>> = Map2d::with_default(input.map_size);

                    for &c in targets.iter() {
                        queue.push_back(c);
                        group[c] = Some(c);
                    }

                    while let Some(c) = queue.pop_front() {
                        let g = group[c].unwrap();

                        for &next in &input.map_index[c] {
                            let Some(next) = next else {
                                continue;
                            };

                            if group[next].is_none() {
                                group[next] = Some(g);
                                queue.push_back(next);
                            }
                        }
                    }

                    let mut areas = Map2d::with_default(input.map_size);

                    for &g in group.iter() {
                        areas[g.unwrap()] += 1;
                    }

                    const DIV: i64 = 2;

                    for (a, &cnt) in areas.iter_mut().zip(counts.iter()) {
                        *a /= cnt * DIV;
                    }

                    Self { input, areas }
                }
            }

            pub struct DistEnv<'a>(pub &'a Input);

            impl<'a> Env<'a> for DistEnv<'a> {
                fn input(&self) -> &'a Input {
                    &self.0
                }

                fn calc_score(&self, state: &State) -> i64 {
                    calc_tentative_score(&self.0, &state.path)
                }
            }

            impl<'a> AreaEnv<'a> {
                #[target_feature(
                    enable = "aes,avx,avx2,bmi1,bmi2,fma,fxsr,pclmulqdq,popcnt,rdrand,rdseed,sse,sse2,sse4.1,sse4.2,ssse3,xsave,xsavec,xsaveopt,xsaves"
                )]
                unsafe fn calc_score_inner(&self, state: &State) -> i64 {
                    static mut VIS_BUFFER: Vec<usize> = vec![];
                    static mut DIST_BUFFER: Vec<usize> = vec![];
                    static mut TRIANGLE_TABLE: Vec<u32> = vec![];

                    assert!(state.path.len() >= 1);

                    unsafe {
                        if VIS_BUFFER.len() == 0 {
                            VIS_BUFFER = vec![0; self.input.map_size * self.input.map_size];
                        }

                        if DIST_BUFFER.len() == 0 {
                            DIST_BUFFER = vec![0; 100001];
                        }

                        // かけ算が重いので前計算する
                        if TRIANGLE_TABLE.len() == 0 {
                            for i in 0..=100000 {
                                TRIANGLE_TABLE.push(i * (i - 1) / 2);
                            }
                        }

                        let mut score = 0;
                        let mut turn = 0;
                        let mut current = state.path[0];

                        // 最終ターンを求める
                        for (&c, d) in state.path.iter().zip(DIST_BUFFER.iter_mut()) {
                            *d = *self.input.dists.get_unchecked(current).get_unchecked(c) as usize
                                + self.areas[c] as usize;
                            turn += *d;
                            *VIS_BUFFER.get_unchecked_mut(c.0 as usize) = turn;
                            current = c;
                        }

                        // スコア計算
                        for (&c, d) in state.path.iter().zip(DIST_BUFFER.iter()) {
                            turn += d;
                            let prev = VIS_BUFFER.get_unchecked_mut(c.0 as usize);
                            let dt = turn - *prev;
                            *prev = turn;
                            score += (*self.input.dirty.get_unchecked(c) as u32
                                * TRIANGLE_TABLE.get_unchecked(dt))
                                as u64;
                        }

                        // score /= (turn / 2)
                        score *= 2;
                        score /= turn as u64;

                        score as i64
                    }
                }
            }

            impl<'a> Env<'a> for AreaEnv<'a> {
                fn input(&self) -> &'a Input {
                    &self.input
                }

                fn calc_score(&self, state: &State) -> i64 {
                    unsafe { self.calc_score_inner(state) }
                }
            }

            #[derive(Debug, Clone)]
            pub struct State {
                pub path: Vec<CoordIndex>,
            }

            impl State {
                pub fn from_path(_input: &Input, path: Vec<CoordIndex>) -> Self {
                    Self { path }
                }
            }

            trait Neigh {
                fn apply(&mut self, input: &Input, state: &mut State);

                fn postprocess(&mut self, input: &Input, state: &mut State);

                fn rollback(&mut self, input: &Input, state: &mut State);

                fn name(&self) -> &'static str;
            }

            struct MoveSingle {
                remove_index: usize,
                new_index: usize,
                c: CoordIndex,
            }

            impl MoveSingle {
                fn try_generate(
                    input: &Input,
                    state: &mut State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    unsafe {
                        static mut CANDIATES: Vec<usize> = vec![];

                        let remove_index = rng.gen_range(1..(state.path.len() - 1));
                        let c = state.path[remove_index];
                        let prev_c = state.path[remove_index - 1];
                        let next_c = state.path[remove_index + 1];

                        let dist_diff_remove = input.dists[prev_c][next_c]
                            - input.dists[prev_c][c]
                            - input.dists[c][next_c];

                        state.path.remove(remove_index);
                        CANDIATES.clear();

                        let dists = &input.dists[c];
                        let mut prev_c = state.path[0];
                        const THRESHOLD: i64 = 1;
                        let threshold = THRESHOLD - dist_diff_remove;

                        for i in 1..state.path.len() {
                            let next_c = state.path[i];
                            let dist_diff =
                                dists[prev_c] + dists[next_c] - input.dists[prev_c][next_c];
                            prev_c = next_c;

                            if dist_diff <= threshold && i != remove_index {
                                CANDIATES.push(i);
                            }
                        }

                        let Some(new_index) = CANDIATES.choose(rng).copied() else {
                            state.path.insert(remove_index, c);
                            return None;
                        };

                        Some(Box::new(MoveSingle {
                            remove_index,
                            new_index,
                            c,
                        }))
                    }
                }
            }

            impl Neigh for MoveSingle {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    state.path.insert(self.new_index, self.c);
                }

                fn postprocess(&mut self, _input: &Input, _state: &mut State) {
                    // do nothing
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    state.path.remove(self.new_index);
                    state.path.insert(self.remove_index, self.c);
                }

                fn name(&self) -> &'static str {
                    "MoveSingle"
                }
            }

            struct TwoOpt {
                left: usize,
                right: usize,
            }

            impl TwoOpt {
                fn try_generate(
                    input: &Input,
                    state: &mut State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    static mut RAND_LEN: Vec<usize> = vec![];
                    static mut RAND_PATH: Vec<usize> = vec![];
                    static mut RAND_I: usize = 0;

                    unsafe {
                        if RAND_LEN.len() == 0 {
                            for _ in 0..100000 {
                                let pow = 1 << rng.gen_range(1..=10);
                                let l = pow + rng.gen_range(0..pow);
                                RAND_LEN.push(l);

                                RAND_PATH.push(rng.gen());
                            }
                        }

                        let mut trial = 0;

                        let (l, r) = loop {
                            let l = RAND_PATH.get_unchecked(RAND_I) % (state.path.len() - 2) + 1;
                            trial += 1;

                            // シフト演算で擬似的にpow演算をする
                            let r = l + RAND_LEN.get_unchecked(RAND_I);
                            RAND_I += 1;

                            if RAND_I >= RAND_LEN.len() {
                                RAND_I -= RAND_LEN.len()
                            }

                            if r < state.path.len() {
                                const THRESHOLD: i64 = 1;
                                let c00 = *state.path.get_unchecked(l - 1);
                                let c01 = *state.path.get_unchecked(l);
                                let c10 = *state.path.get_unchecked(r - 1);
                                let c11 = *state.path.get_unchecked(r);

                                let distc00 = &input.dists[c00];
                                let distc11 = &input.dists[c11];

                                let dist_diff =
                                    distc00[c10] + distc11[c01] - distc00[c01] - distc11[c10];

                                if dist_diff <= THRESHOLD {
                                    break (l, r);
                                }
                            } else if trial >= 1000 {
                                return None;
                            }
                        };

                        Some(Box::new(TwoOpt { left: l, right: r }))
                    }
                }
            }

            impl Neigh for TwoOpt {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    state.path[self.left..self.right].reverse();
                }

                fn postprocess(&mut self, _input: &Input, _state: &mut State) {
                    // do nothing
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    state.path[self.left..self.right].reverse();
                }

                fn name(&self) -> &'static str {
                    "TwoOpt"
                }
            }

            struct MoveMulti {
                left: usize,
                right: usize,
                rev: bool,
                state: State,
            }

            impl MoveMulti {
                fn try_generate(
                    input: &Input,
                    state: &mut State,
                    rng: &mut impl Rng,
                ) -> Option<Box<dyn Neigh>> {
                    unsafe {
                        static mut CANDIATES: Vec<usize> = vec![];
                        static mut RAND_START: Vec<usize> = vec![];
                        static mut RAND_LEN: Vec<usize> = vec![];
                        static mut STATE_LEN: usize = !0;
                        static mut RAND_I: usize = 0;
                        const RAND_SEQ_MAX: usize = 10000;

                        if STATE_LEN != state.path.len() {
                            RAND_START.clear();
                            RAND_I = 0;
                            STATE_LEN = state.path.len();

                            for _ in 0..RAND_SEQ_MAX {
                                RAND_START.push(rng.gen_range(1..(state.path.len() - 1)));
                            }
                        }

                        if RAND_LEN.len() == 0 {
                            for _ in 0..RAND_SEQ_MAX {
                                RAND_LEN.push(rng.gen_range(2..20));
                            }
                        }

                        let mut trial = 0;
                        let (left, right) = loop {
                            const THRESHOLD: i64 = 2;
                            trial += 1;
                            let l = *RAND_START.get_unchecked(RAND_I);
                            let r = l + *RAND_LEN.get_unchecked(RAND_I);
                            RAND_I += 1;

                            if RAND_I >= RAND_SEQ_MAX {
                                RAND_I = 0;
                            }

                            let c0 = *state.path.get_unchecked(l);
                            let c1 = *state.path.get_unchecked(r - 1);

                            if r < state.path.len() && input.dists[c0][c1] <= THRESHOLD {
                                break (l, r);
                            } else if trial >= 100 {
                                return None;
                            }
                        };

                        let rev = rng.gen_bool(0.5);
                        let c0 = state.path[left];
                        let c1 = state.path[right - 1];
                        let prev_c = state.path[left - 1];
                        let next_c = state.path[right];

                        let dist_diff_remove = input.dists[prev_c][next_c]
                            - input.dists[prev_c][c0]
                            - input.dists[c1][next_c];
                        CANDIATES.clear();

                        if rev {
                            state.path[left..right].reverse();
                        }

                        let c0 = state.path[left];
                        let c1 = state.path[right - 1];

                        let dists0 = &input.dists[c0];
                        let dists1 = &input.dists[c1];
                        let mut prev_c = state.path[0];
                        const THRESHOLD: i64 = 1;
                        let threshold = THRESHOLD - dist_diff_remove;

                        for i in 1..left {
                            let next_c = *state.path.get_unchecked(i);
                            let dist_diff =
                                dists0[prev_c] + dists1[next_c] - input.dists[prev_c][next_c];
                            prev_c = next_c;

                            if dist_diff <= threshold {
                                CANDIATES.push(i);
                            }
                        }

                        prev_c = state.path[right];

                        for i in (right + 1)..state.path.len() {
                            let next_c = *state.path.get_unchecked(i);
                            let dist_diff =
                                dists0[prev_c] + dists1[next_c] - input.dists[prev_c][next_c];
                            prev_c = next_c;

                            if dist_diff <= threshold {
                                CANDIATES.push(i);
                            }
                        }

                        match CANDIATES.choose(rng).copied() {
                            Some(new_index) => {
                                let mut new_state = state.clone();

                                if new_index < left {
                                    let mut index = new_index;
                                    let next = index + right - left;
                                    new_state.path[index..next]
                                        .copy_from_slice(&state.path[left..right]);
                                    index = next;
                                    let next = index + left - new_index;
                                    new_state.path[index..next]
                                        .copy_from_slice(&state.path[new_index..left]);
                                } else {
                                    let mut index = left;
                                    let next = index + new_index - right;
                                    new_state.path[index..next]
                                        .copy_from_slice(&state.path[right..new_index]);
                                    index = next;
                                    let next = index + right - left;
                                    new_state.path[index..next]
                                        .copy_from_slice(&state.path[left..right]);
                                }

                                Some(Box::new(MoveMulti {
                                    left,
                                    right,
                                    rev,
                                    state: new_state,
                                }))
                            }
                            None => {
                                if rev {
                                    state.path[left..right].reverse();
                                }

                                None
                            }
                        }
                    }
                }
            }

            impl Neigh for MoveMulti {
                fn apply(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.state, state);
                }

                fn postprocess(&mut self, _input: &Input, _state: &mut State) {
                    // do nothing
                }

                fn rollback(&mut self, _input: &Input, state: &mut State) {
                    std::mem::swap(&mut self.state, state);

                    if self.rev {
                        state.path[self.left..self.right].reverse();
                    }
                }

                fn name(&self) -> &'static str {
                    "MoveMulti"
                }
            }
        }
        mod gen_loop {
            use crate::{
                common::grid::{Coord, CoordIndex, Map2d},
                common::Input,
                solvers::loop_insertion::annealing_rough::{self, Env, State},
                ChangeMinMax,
            };
            use itertools::Itertools;
            use rand::{seq::SliceRandom, Rng};
            use std::{cmp::Reverse, collections::VecDeque};

            use super::annealing_rough::AreaEnv;

            pub fn generate_loop(input: &Input) -> Vec<CoordIndex> {
                let mut used: Map2d<bool> = Map2d::with_default(input.map_size);
                let mut walk = vec![vec![]];
                let mut dists = vec![0];
                let mut brushup_duration = 0.1 * input.config.time_ratio;

                for (trial, &threshold) in input.config.thresholds.iter().enumerate() {
                    let mut cut = get_dirty_coords(input, threshold)
                        .iter()
                        .filter(|&&c| !used[c])
                        .copied()
                        .collect_vec();

                    if cut.len() == 0 && walk.len() == 1 {
                        continue;
                    }

                    let current_dists = get_dists(input, &used);
                    cut.sort_unstable_by_key(|&c| Reverse(current_dists[c]));

                    for c in cut {
                        insert_coord(input, &mut used, &mut walk, &mut dists, c);
                    }

                    // 焼きなましで距離を改善
                    if walk.len() == 1 {
                        let duration = 0.01 * input.config.time_ratio;
                        walk = tsp(input, walk, &mut dists, duration);
                    } else if trial != input.config.thresholds.len() - 1 {
                        (walk, dists) = brushup_path(input, walk, brushup_duration);
                        brushup_duration *= 2.0;
                    }

                    // 複製する
                    if threshold != 0 {
                        let n = walk.len();

                        for i in 0..n {
                            walk.push(walk[i].clone());
                            dists.push(dists[i]);
                        }
                    }
                }

                let mut walk = walk.iter().flatten().copied().collect_vec();
                walk.push(walk[0]);
                walk
            }

            fn tsp(
                input: &Input,
                walk: Vec<Vec<CoordIndex>>,
                dists: &mut Vec<i64>,
                annealing_duration: f64,
            ) -> Vec<Vec<CoordIndex>> {
                let walk = annealing_tsp(input, walk, annealing_duration);

                // 距離を再計算
                for i in 0..walk.len() {
                    dists[i] = 0;

                    for j in 0..walk[i].len() {
                        let (next_i, next_j) = if j + 1 < walk[i].len() {
                            (i, j + 1)
                        } else {
                            ((i + 1) % walk.len(), 0)
                        };

                        let c0 = walk[i][j];
                        let c1 = walk[next_i][next_j];
                        dists[i] += input.dists[c0][c1];
                    }
                }

                walk
            }

            fn insert_coord(
                input: &Input,
                used: &mut Map2d<bool>,
                walk: &mut Vec<Vec<CoordIndex>>,
                dists: &mut Vec<i64>,
                c: CoordIndex,
            ) {
                used[c] = true;

                if walk.len() == 1 && walk[0].len() == 0 {
                    walk[0].push(c);
                    return;
                }

                let mut candidates = Vec::with_capacity(walk.iter().map(|w| w.len()).sum());
                let mut index = 0;

                for walk_i in 0..walk.len() {
                    for insert_i in 0..walk[walk_i].len() {
                        // insert_iとinsert_i+1の間に挿入するとする
                        index += 1;
                        let prev_c = walk[walk_i][insert_i];
                        let (next_w, next_i) = if insert_i + 1 >= walk[walk_i].len() {
                            ((walk_i + 1) % walk.len(), 0)
                        } else {
                            (walk_i, insert_i + 1)
                        };
                        let next_c = walk[next_w][next_i];

                        let dist_matrix = &input.dists;
                        let old_dist = dists[walk_i];
                        let new_dist = old_dist + dist_matrix[prev_c][c] + dist_matrix[c][next_c]
                            - dist_matrix[prev_c][next_c];

                        let d2_diff = new_dist * new_dist - old_dist * old_dist;

                        candidates.push((d2_diff, new_dist, index, walk_i, insert_i));
                    }
                }

                const MAX_CANDIDATES: usize = 10;

                if candidates.len() > MAX_CANDIDATES {
                    candidates.select_nth_unstable(MAX_CANDIDATES - 1);
                    candidates.truncate(MAX_CANDIDATES);
                }

                let mut best_i0 = !0;
                let mut best_i1 = !0;
                let mut best_d = i64::MAX;
                let mut best_score = i64::MAX;

                let mut path = walk.iter().flatten().copied().collect_vec();
                let env = AreaEnv::new(&input, &path);

                for &(_, dist, index, walk_i, insert_i) in &candidates {
                    path.insert(index, c);
                    path.push(path[0]);
                    let state = State::from_path(input, path.clone());
                    let score = env.calc_score(&state);

                    if best_score.change_min(score) {
                        best_i0 = walk_i;
                        best_i1 = insert_i;
                        best_d = dist;
                    }

                    path.pop();
                    path.remove(index);
                }

                // 場所が+1されることに注意
                walk[best_i0].insert(best_i1 + 1, c);
                dists[best_i0] = best_d;
            }

            fn get_dists(input: &Input, starts: &Map2d<bool>) -> Map2d<i64> {
                let mut queue = VecDeque::new();
                let mut dists = Map2d::new(
                    vec![i64::MAX; input.map_size * input.map_size],
                    input.map_size,
                );

                for sr in 0..input.map_size {
                    for sc in 0..input.map_size {
                        let c = Coord::new(sr, sc);

                        if starts[c] {
                            queue.push_back(c);
                            dists[c] = 0;
                        }
                    }
                }

                while let Some(c) = queue.pop_front() {
                    let next_dist = dists[c] + 1;

                    for next in input.map[c] {
                        let Some(next) = next else {
                            continue;
                        };

                        if dists[next].change_min(next_dist) {
                            queue.push_back(next);
                        }
                    }
                }

                dists
            }

            fn get_dirty_coords(input: &Input, threshold: i64) -> Vec<CoordIndex> {
                let mut result = vec![];

                for row in 0..input.map_size {
                    for col in 0..input.map_size {
                        let c = Coord::new(row, col).to_coord_index(input.map_size);

                        if input.dirty[c] >= threshold {
                            result.push(c);
                        }
                    }
                }

                result
            }

            fn annealing_tsp(
                input: &Input,
                initial_solution: Vec<Vec<CoordIndex>>,
                duration: f64,
            ) -> Vec<Vec<CoordIndex>> {
                let mut solution = initial_solution;
                let mut best_solution = solution.clone();
                let mut current_score = calc_tsp_score_all(input, &solution);
                let mut best_score = current_score;
                let init_score = current_score;

                let mut all_iter = 0;
                let mut valid_iter = 0;
                let mut accepted_count = 0;
                let mut update_count = 0;
                let mut rng = rand_pcg::Pcg64Mcg::new(42);

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = 1e1;
                let temp1 = 1e-1;
                let mut inv_temp = 1.0 / temp0;

                const MIN_LEN: usize = 2;
                let mut candidates = vec![];

                for i in 0..solution.len() {
                    let max = solution[i].len().saturating_sub(MIN_LEN);

                    for j in 0..max {
                        candidates.push((i, j));
                    }
                }

                if candidates.len() == 0 {
                    return solution;
                }

                loop {
                    all_iter += 1;
                    if (all_iter & ((1 << 10) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
                        inv_temp = 1.0 / temp;

                        if time >= 1.0 {
                            break;
                        }
                    }

                    // 変形
                    let &(group, l) = candidates.choose(&mut rng).unwrap();
                    let r = l + rng.gen_range(MIN_LEN..=solution[group].len());

                    if r > solution[group].len()
                        || (solution.len() == 1 && r - l == solution[0].len())
                    {
                        continue;
                    }

                    let c00 = if l > 0 {
                        solution[group][l - 1]
                    } else {
                        let g = (group + solution.len() - 1) % solution.len();
                        *solution[g].last().unwrap()
                    };
                    let c01 = solution[group][l];
                    let c10 = solution[group][r - 1];
                    let c11 = if r < solution[group].len() {
                        solution[group][r]
                    } else {
                        let g = (group + 1) % solution.len();
                        solution[g][0]
                    };

                    // スコア計算
                    let score_diff = input.dists[c00][c10] + input.dists[c01][c11]
                        - input.dists[c00][c01]
                        - input.dists[c10][c11];

                    if score_diff <= 0 || rng.gen_bool(f64::exp(-score_diff as f64 * inv_temp)) {
                        // 解の更新
                        current_score += score_diff;
                        accepted_count += 1;
                        solution[group][l..r].reverse();

                        if best_score.change_min(current_score) {
                            best_solution = solution.clone();
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

                best_solution
            }

            fn calc_tsp_score_all(input: &Input, coords: &[Vec<CoordIndex>]) -> i64 {
                let mut score = 0;

                for i in 0..coords.len() {
                    for j in 0..coords[i].len() {
                        let is_in_group = j + 1 < coords[i].len();
                        let next_i = if is_in_group {
                            i
                        } else {
                            (i + 1) % coords.len()
                        };
                        let next_j = if is_in_group { j + 1 } else { 0 };

                        let c0 = coords[i][j];
                        let c1 = coords[next_i][next_j];
                        score += input.dists[c0][c1];
                    }
                }

                score
            }

            fn brushup_path(
                input: &Input,
                walk: Vec<Vec<CoordIndex>>,
                duration: f64,
            ) -> (Vec<Vec<CoordIndex>>, Vec<i64>) {
                let walk_len = walk.len();
                let mut walk = walk.iter().flatten().copied().collect_vec();
                let env = AreaEnv::new(&input, &walk);
                walk.push(walk[0]);
                let state = annealing_rough::State::from_path(input, walk);
                let mut walk = annealing_rough::annealing(&env, state, duration).path;

                // 再分割する
                let mut dist_prefix_sum = vec![0; walk.len()];

                for i in 1..walk.len() {
                    dist_prefix_sum[i] +=
                        dist_prefix_sum[i - 1] + input.dists[walk[i - 1]][walk[i]];
                }

                let mut dp = vec![vec![i64::MAX / 2; walk_len + 1]; walk.len()];
                let mut from = vec![vec![!0; walk_len + 1]; walk.len()];
                dp[0][0] = 0;

                for i in 0..(dp.len() - 1) {
                    for j in (i + 1)..dp.len() {
                        for k in 0..walk_len {
                            let d = dist_prefix_sum[j] - dist_prefix_sum[i];
                            let next = dp[i][k] + d * d;

                            if dp[j][k + 1].change_min(next) {
                                from[j][k + 1] = i;
                            }
                        }
                    }
                }

                walk.pop();

                let mut stack = vec![];
                let mut i = walk.len();
                let mut k = walk_len;

                while i > 0 {
                    stack.push(i);
                    i = from[i][k];
                    k -= 1;
                }

                let mut new_walk = vec![];

                while let Some(j) = stack.pop() {
                    new_walk.push(walk[i..j].iter().copied().collect_vec());
                    i = j;
                }

                let mut dists = vec![0; walk_len];

                // 距離を再計算
                for i in 0..new_walk.len() {
                    dists[i] = 0;

                    for j in 0..new_walk[i].len() {
                        let (next_i, next_j) = if j + 1 < new_walk[i].len() {
                            (i, j + 1)
                        } else {
                            ((i + 1) % new_walk.len(), 0)
                        };

                        let c0 = new_walk[i][j];
                        let c1 = new_walk[next_i][next_j];
                        dists[i] += input.dists[c0][c1];
                    }
                }

                (new_walk, dists)
            }
        }
        mod path {
            use crate::{
                common::grid::{Coord, CoordIndex, Map2d},
                common::Input,
                ChangeMinMax,
            };
            use std::{cmp::Reverse, collections::BinaryHeap};

            pub fn to_path_grouped(
                input: &Input,
                loop_coords: &[CoordIndex],
            ) -> Vec<Vec<CoordIndex>> {
                let mut dirty = Map2d::with_default(input.map_size);
                let mut path = vec![];
                let mut current = loop_coords[0];

                for _ in 0..2 {
                    // とりあえず2回やる
                    let mut p = vec![vec![loop_coords[0]]];

                    for &target in loop_coords[1..].iter() {
                        let new_path = dijkstra(input, current, target, &dirty);

                        for &coord in &new_path {
                            for row in 0..input.map_size {
                                for col in 0..input.map_size {
                                    let c = Coord::new(row, col);

                                    dirty[c] += input.dirty[c];
                                }
                            }

                            dirty[coord] = 0;
                            current = coord;
                        }

                        p.push(new_path);
                    }

                    path = p;
                }

                path
            }

            fn dijkstra(
                input: &Input,
                start: CoordIndex,
                goal: CoordIndex,
                dirty: &Map2d<i64>,
            ) -> Vec<CoordIndex> {
                const INF: i64 = i64::MAX / 2;
                let mut dists =
                    Map2d::new(vec![INF; input.map_size * input.map_size], input.map_size);
                let mut from = Map2d::with_default(input.map_size);
                let mut queue = BinaryHeap::new();
                dists[start] = 0;
                queue.push(Reverse((0, start)));

                while let Some(Reverse((dist, c))) = queue.pop() {
                    if c == goal {
                        break;
                    }

                    if dists[c] < dist {
                        continue;
                    }

                    for &next in &input.map_index[c] {
                        let Some(next) = next else {
                            continue;
                        };
                        let next_cost = dist + 1000000000 - dirty[next];

                        if dists[next].change_min(next_cost) {
                            from[next] = c;
                            queue.push(Reverse((next_cost, next)));
                        }
                    }
                }

                let mut path = vec![];
                let mut current = goal;

                while current != start {
                    path.push(current);
                    current = from[current];
                }

                path.reverse();
                path
            }
        }

        use std::time::Instant;

        use itertools::Itertools;

        use self::{annealing_rough::DistEnv, gen_loop::generate_loop};
        use super::Solver;
        use crate::{common::grid::Coord, common::Input, common::Output};

        #[derive(Debug, Clone)]
        pub struct LoopInsertionSolver;

        impl Solver for LoopInsertionSolver {
            fn solve(&self, input: &Input) -> Output {
                let loop_coords = generate_loop(input);
                let duration =
                    1.4 * input.config.time_ratio - (Instant::now() - input.since).as_secs_f64();
                let state = annealing_rough::State::from_path(input, loop_coords);
                let loop_coords =
                    annealing_rough::annealing(&DistEnv(&input), state, duration).path;
                let var_name =
                    1.985 * input.config.time_ratio - (Instant::now() - input.since).as_secs_f64();
                let duration = var_name;
                let mut path = annealing_detail::to_path(input, loop_coords, duration);
                path.pop();
                let path = normalize(path);
                Output::from_path(input, &path)
            }
        }

        fn normalize(path: Vec<Coord>) -> Vec<Coord> {
            let (start_index, _) = path.iter().find_position(|&&c| c == Input::START).unwrap();

            let mut result_path = vec![];
            result_path.extend_from_slice(&path[start_index..]);
            result_path.extend_from_slice(&path[..start_index]);
            result_path.push(Input::START);
            result_path
        }
    }

    use crate::common::{Input, Output};

    pub trait Solver {
        fn solve(&self, input: &Input) -> Output;
    }
}

use crate::{common::Input, solvers::loop_insertion::LoopInsertionSolver};
use solvers::Solver;

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

fn main() {
    let input = Input::read_input();
    let solver = LoopInsertionSolver;
    let output = solver.solve(&input);
    println!("{output}");
}
