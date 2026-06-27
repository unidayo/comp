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

    /// 除算を高速に行う構造体
    ///
    /// https://nu50218.dev/posts/integer-division-by-multiplication/
    #[derive(Debug, Clone, Copy)]
    pub struct Divisor {
        l: u64,
        m: u64,
    }

    impl Divisor {
        pub const fn new(div: u32) -> Self {
            let div = div as u64;
            let l = ((div << 1) - 1).ilog2() as u64;
            let m = ((1u64 << (32 + l)) + div - 1) / div;

            Self { l, m }
        }

        pub const fn div(&self, x: u32) -> u32 {
            let mut t = x as u64;
            t *= self.m;
            t >>= 32 + self.l;
            t as u32
        }
    }

    pub struct DivisorSet<const N: usize> {
        dividers: [Divisor; N],
    }

    impl<const N: usize> DivisorSet<N> {
        pub const fn new() -> Self {
            let mut dividers = [Divisor { l: 0, m: 0 }; N];
            let mut i = 0;

            while i < dividers.len() {
                dividers[i] = Divisor::new(i as u32 + 1);
                i += 1;
            }

            Self { dividers }
        }

        pub const fn div(&self, x: u32, div: u32) -> u32 {
            self.dividers[div as usize - 1].div(x)
        }
    }

    pub const DIVISOR: DivisorSet<1000> = DivisorSet::new();

    #[cfg(test)]
    mod test {
        use rand::Rng as _;

        use super::*;

        #[test]
        fn test_divider() {
            let mut rng = rand::thread_rng();

            for _ in 0..1000 {
                let d = rng.gen_range(1..=1000);
                let div = Divisor::new(d);
                let x = rng.gen_range(0..=10000);

                assert_eq!(div.div(x), x / d as u32);
            }
        }
    }
}
mod problem {
    use std::{fmt::Display, time::Instant};

    use proconio::input;

    use crate::params::ParamSuggester;

    #[derive(Debug, Clone)]
    pub struct Input {
        pub days: usize,
        pub n: usize,
        pub requests: Vec<Vec<i32>>,
        pub packing_ratio: f64,
        pub since: Instant,
        pub first_fit_config: FirstFitConfig,
    }

    impl Input {
        pub const W: i32 = 1000;

        pub fn read() -> Self {
            input! {
                _w: i32,
                days: usize,
                n: usize,
                requests: [[i32; n]; days],
            }

            let mut packing_ratio = requests
                .iter()
                .map(|reqs| reqs.iter().sum::<i32>() as f64)
                .sum::<f64>();
            packing_ratio /= days as f64 * Self::W as f64 * Self::W as f64;

            let e = (1.0 - packing_ratio) * (Input::W * Input::W) as f64;

            let first_fit_config = FirstFitConfig::new(days, n, e);

            let since = Instant::now();

            Self {
                days,
                n,
                requests,
                packing_ratio,
                first_fit_config,
                since,
            }
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
    pub struct Rect {
        pub x0: i32,
        pub y0: i32,
        pub x1: i32,
        pub y1: i32,
    }

    impl Rect {
        pub fn new(x0: i32, y0: i32, x1: i32, y1: i32) -> Self {
            Self { x0, y0, x1, y1 }
        }

        pub fn is_valid(&self) -> bool {
            self.x0 < self.x1 && self.y0 < self.y1
        }

        pub fn area(&self) -> i32 {
            (self.x1 - self.x0) * (self.y1 - self.y0)
        }
    }

    impl Display for Rect {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "{} {} {} {}", self.y0, self.x0, self.y1, self.x1)
        }
    }

    #[derive(Debug, Clone, Copy)]
    pub struct FirstFitConfig {
        pub step1_ratio: f64,
        pub step1_temp0: f64,
        pub step1_temp1: f64,
        pub step2_temp0: f64,
        pub step2_temp1: f64,
    }

    impl FirstFitConfig {
        fn new(d: usize, n: usize, e: f64) -> Self {
            //let args = std::env::args().collect_vec();
            //let step1_ratio = args.get(1).and_then(|s| s.parse().ok()).unwrap_or(0.1);
            //let step1_temp0 = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(1e7);
            //let step1_temp1 = args.get(3).and_then(|s| s.parse().ok()).unwrap_or(1e0);
            //let step2_temp0 = args.get(4).and_then(|s| s.parse().ok()).unwrap_or(2e2);
            //let step2_temp1 = args.get(5).and_then(|s| s.parse().ok()).unwrap_or(3e0);

            let step1_ratio = ParamSuggester::gen_ratio_pred().suggest(d, n, e);
            let step1_temp0 = ParamSuggester::gen_t00_pred().suggest(d, n, e);
            let step1_temp1 = ParamSuggester::gen_t01_pred().suggest(d, n, e);
            let step2_temp0 = ParamSuggester::gen_t10_pred().suggest(d, n, e);
            let step2_temp1 = ParamSuggester::gen_t11_pred().suggest(d, n, e);

            Self {
                step1_ratio,
                step1_temp0,
                step1_temp1,
                step2_temp0,
                step2_temp1,
            }
        }
    }
}
mod solver {
    pub mod annealier2d {
        use itertools::Itertools as _;
        use rand::Rng as _;

        use crate::{common::ChangeMinMax as _, solver::Solver};

        use super::{Input, Rect};

        pub struct Annealer2d {
            duration: f64,
        }

        impl Annealer2d {
            pub fn new(duration: f64) -> Self {
                Self { duration }
            }
        }

        impl Solver for Annealer2d {
            fn solve(&self, input: &Input) -> (Vec<Vec<Rect>>, i64) {
                let sizes = dp(&input);
                let (coords, indices) = build_squares(&sizes);
                let env = Env::new(input.clone(), indices.clone());
                let state = State::new(&env, vec![coords; input.days]);
                let duration = self.duration;
                let state = annealing(&env, state, duration);
                let score = state.calc_score(&env).unwrap();

                let mut result = vec![];

                for coords in &state.coords {
                    let mut rects = indices
                        .rects
                        .iter()
                        .map(|r| coords.get_rect(r))
                        .collect_vec();
                    rects.sort_by_key(|r| r.area());

                    result.push(rects);
                }

                (result, score)
            }
        }

        static mut AREA_BUF: Vec<i32> = vec![];

        #[derive(Debug, Clone)]
        struct Env {
            input: Input,
            coord_indices: CoordIndices,
        }

        impl Env {
            fn new(input: Input, coord_indices: CoordIndices) -> Self {
                Self {
                    input,
                    coord_indices,
                }
            }
        }

        #[derive(Debug, Clone)]
        struct State {
            coords: Vec<LineCoords>,
            area_scores: Vec<i64>,
            line_scores: Vec<i64>,
        }

        impl State {
            fn new(env: &Env, coords: Vec<LineCoords>) -> Self {
                let mut state = Self {
                    coords,
                    area_scores: vec![],
                    line_scores: vec![],
                };

                for day in 0..env.input.days {
                    state
                        .area_scores
                        .push(state.calc_area_score(&env, day).unwrap());
                    state.line_scores.push(state.calc_line_score(&env, day));
                }

                state
                    .line_scores
                    .push(state.calc_line_score(&env, env.input.days));

                state
            }

            fn calc_score(&self, env: &Env) -> Result<i64, ()> {
                let mut score = 1;
                let areas = unsafe {
                    AREA_BUF.clear();
                    &mut AREA_BUF
                };

                for (reqs, coord) in env.input.requests.iter().zip(self.coords.iter()) {
                    areas.clear();

                    for r in env.coord_indices.rects.iter() {
                        let rect = coord.get_rect(r);
                        if !rect.is_valid() {
                            return Err(());
                        }

                        let area = rect.area();
                        areas.push(area);
                    }

                    areas.sort_unstable();

                    for (req, area) in reqs.iter().zip(areas.iter()) {
                        score += 100 * (req - area).max(0) as i64;
                    }
                }

                // 線分が偶然重なることを考慮していないので、厳密には正しくない
                for l in env.coord_indices.lines.iter() {
                    let mut l0 = self.coords[0].get_line(l);

                    for coord in self.coords.iter().skip(1) {
                        let l1 = coord.get_line(l);
                        score += l0.diff(&l1) as i64;
                        l0 = l1;
                    }
                }

                Ok(score)
            }

            fn calc_area_score(&self, env: &Env, day: usize) -> Result<i64, ()> {
                let mut score = 0;
                let areas = unsafe {
                    AREA_BUF.clear();
                    &mut AREA_BUF
                };
                let coord = &self.coords[day];

                for r in env.coord_indices.rects.iter() {
                    let rect = coord.get_rect(r);
                    if !rect.is_valid() {
                        return Err(());
                    }

                    let area = rect.area();
                    areas.push(area);
                }

                areas.sort_unstable();

                for (req, area) in env.input.requests[day].iter().zip(areas.iter()) {
                    score += (req - area).max(0) as i64;
                }

                Ok(score * 100)
            }

            fn calc_line_score(&self, env: &Env, day: usize) -> i64 {
                let Some(prev_coord) = self.coords.get(day - 1) else {
                    return 0;
                };

                let Some(next_coord) = self.coords.get(day) else {
                    return 0;
                };

                let mut score = 0;

                for l in env.coord_indices.lines.iter() {
                    let l0 = prev_coord.get_line(l);
                    let l1 = next_coord.get_line(l);
                    score += l0.diff(&l1) as i64;
                }

                score
            }
        }

        #[derive(Debug, Clone)]
        struct LineCoords {
            coords: Vec<i32>,
        }

        impl LineCoords {
            fn new() -> Self {
                Self { coords: vec![] }
            }

            fn get_line(&self, line: &LineIndex) -> Line {
                Line::new(
                    self.coords[line.mu0],
                    self.coords[line.mu1],
                    self.coords[line.xi],
                )
            }

            fn get_rect(&self, rect: &RectIndex) -> Rect {
                Rect::new(
                    self.coords[rect.x0],
                    self.coords[rect.y0],
                    self.coords[rect.x1],
                    self.coords[rect.y1],
                )
            }
        }

        #[derive(Debug, Clone)]
        struct CoordIndices {
            lines: Vec<LineIndex>,
            rects: Vec<RectIndex>,
        }

        impl CoordIndices {
            fn new() -> Self {
                Self {
                    lines: vec![],
                    rects: vec![],
                }
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        struct Line {
            mu0: i32,
            mu1: i32,
            xi: i32,
        }

        impl Line {
            fn new(mu0: i32, mu1: i32, xi: i32) -> Self {
                Self { mu0, mu1, xi }
            }

            fn len(&self) -> i32 {
                self.mu1 - self.mu0
            }

            fn overlap(&self, other: &Self) -> i32 {
                if self.xi != other.xi {
                    return 0;
                }

                let mu0 = self.mu0.max(other.mu0);
                let mu1 = self.mu1.min(other.mu1);
                (mu1 - mu0).max(0)
            }

            fn diff(&self, other: &Self) -> i32 {
                self.len() + other.len() - 2 * self.overlap(other)
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        struct LineIndex {
            mu0: usize,
            mu1: usize,
            xi: usize,
        }

        impl LineIndex {
            fn new(mu0: usize, mu1: usize, xi: usize) -> Self {
                Self { mu0, mu1, xi }
            }
        }

        #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
        struct RectIndex {
            x0: usize,
            y0: usize,
            x1: usize,
            y1: usize,
        }

        impl RectIndex {
            fn new(x0: usize, y0: usize, x1: usize, y1: usize) -> Self {
                Self { x0, y0, x1, y1 }
            }
        }

        fn dp(input: &Input) -> Vec<i32> {
            const INF: i32 = i32::MAX / 2;
            const MAX_POW: usize = 10;
            const SUM: usize = 1 << MAX_POW;
            let mut dp = vec![vec![INF; SUM + 1]; input.n + 1];
            let mut from = vec![vec![!0; SUM + 1]; input.n + 1];
            let sizes = (0..=MAX_POW)
                .map(|i| (1 << i) * Input::W * Input::W / (1 << MAX_POW))
                .collect_vec();
            dp[0][0] = 0;

            for i in 0..input.n {
                let target = input.requests.iter().map(|r| r[i]).max().unwrap();
                //let target = input.requests.iter().map(|r| r[i]).sum::<i64>() / input.days as i64;

                for j in 0..=SUM {
                    for pow in 0..=MAX_POW {
                        let next = j + (1 << pow);

                        if next > SUM {
                            break;
                        }

                        let next_score = dp[i][j] + (target - sizes[pow]).max(0);

                        if dp[i + 1][next].change_min(next_score) {
                            from[i + 1][next] = j;
                        }
                    }
                }
            }

            let mut sizes = vec![];
            let mut i = input.n;
            let mut j = SUM;

            while i > 0 {
                let prev = from[i][j];
                sizes.push((j - prev) as i32);
                j = prev;
                i -= 1;
            }

            sizes.reverse();

            sizes
        }

        fn build_squares(sizes: &[i32]) -> (LineCoords, CoordIndices) {
            let mut index = 0;
            let mut line_coords = LineCoords::new();
            let mut coord_indices = CoordIndices::new();
            line_coords.coords.push(0);
            line_coords.coords.push(0);
            line_coords.coords.push(Input::W);
            line_coords.coords.push(Input::W);
            coord_indices.lines.push(LineIndex::new(0, 2, 1));
            coord_indices.lines.push(LineIndex::new(0, 2, 3));
            coord_indices.lines.push(LineIndex::new(1, 3, 0));
            coord_indices.lines.push(LineIndex::new(1, 3, 2));

            sq_dfs(
                0,
                1,
                2,
                3,
                &mut index,
                1024,
                sizes,
                true,
                &mut line_coords,
                &mut coord_indices,
            );

            (line_coords, coord_indices)
        }

        fn sq_dfs(
            x0: usize,
            y0: usize,
            x1: usize,
            y1: usize,
            index: &mut usize,
            size: i32,
            sizes: &[i32],
            vertical: bool,
            cd: &mut LineCoords,
            idx: &mut CoordIndices,
        ) {
            if sizes[*index] == size {
                idx.rects.push(RectIndex::new(x0, y0, x1, y1));
                *index += 1;
                return;
            }

            let size = size / 2;

            if vertical {
                let xx0 = cd.coords[x0];
                let xx1 = cd.coords[x1];
                let mid = (xx0 + xx1) / 2;
                let i = cd.coords.len();
                cd.coords.push(mid as i32);
                idx.lines.push(LineIndex::new(y0, y1, i));

                sq_dfs(x0, y0, i, y1, index, size, sizes, !vertical, cd, idx);
                sq_dfs(i, y0, x1, y1, index, size, sizes, !vertical, cd, idx);
            } else {
                let yy0 = cd.coords[y0];
                let yy1 = cd.coords[y1];
                let mid = (yy0 + yy1) / 2;
                let i = cd.coords.len();
                cd.coords.push(mid as i32);
                idx.lines.push(LineIndex::new(x0, x1, i));

                sq_dfs(x0, y0, x1, i, index, size, sizes, !vertical, cd, idx);
                sq_dfs(x0, i, x1, y1, index, size, sizes, !vertical, cd, idx);
            }
        }

        fn annealing(env: &Env, mut state: State, duration: f64) -> State {
            let mut best_solution = state.clone();
            let mut current_score = state.calc_score(&env).unwrap();
            let mut best_score = current_score;

            let mut all_iter = 0;
            let mut rng = rand_pcg::Pcg64Mcg::new(42);

            let duration_inv = 1.0 / duration;
            let since = std::time::Instant::now();

            let temp0 = 1e6;
            let temp1 = 1e0;
            let mut temp = temp0;

            loop {
                all_iter += 1;
                if (all_iter & ((1 << 4) - 1)) == 0 {
                    let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                    temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);

                    if time >= 1.0 {
                        break;
                    }
                }

                // 変形
                let (day, index, x) = if rng.gen_bool(0.5) {
                    // copy
                    let pre = rng.gen_bool(0.5);
                    let mut day = rng.gen_range(0..env.input.days - 1);
                    if pre {
                        day += 1;
                    }

                    let index = rng.gen_range(4..state.coords[day].coords.len());

                    let x0 = if pre {
                        state.coords[day - 1].coords[index]
                    } else {
                        state.coords[day + 1].coords[index]
                    };

                    let x1 = state.coords[day].coords[index];

                    if x0 == x1 {
                        continue;
                    }

                    let ratio = rng.gen_range(0.1f64..1.5).min(1.0);
                    let new_x = (x0 as f64 * ratio + x1 as f64 * (1.0 - ratio)).round() as i32;
                    (day, index, new_x)
                } else {
                    // shift
                    let day = rng.gen_range(0..env.input.days);
                    let index = rng.gen_range(4..state.coords[day].coords.len());
                    let sign = if rng.gen_bool(0.5) { 1 } else { -1 };
                    let dx = sign * 10f64.powf(rng.gen_range(0.0..2.0)).round() as i32;
                    let new_x = state.coords[day].coords[index] + dx;

                    if new_x <= 0 || new_x >= Input::W {
                        continue;
                    }

                    (day, index, new_x)
                };

                // スコア計算
                // 先に閾値を求めることで評価を高速化する
                let prev_score_day =
                    state.area_scores[day] + state.line_scores[day] + state.line_scores[day + 1];

                let score_threshold =
                    prev_score_day as f64 - temp * rng.gen_range(0.0f64..1.0).ln();

                let prev_x = state.coords[day].coords[index];
                state.coords[day].coords[index] = x;

                let prev_line_score = state.calc_line_score(env, day);

                if prev_line_score as f64 > score_threshold {
                    state.coords[day].coords[index] = prev_x;
                    continue;
                }

                let next_line_score = state.calc_line_score(env, day + 1);

                if (prev_line_score + next_line_score) as f64 > score_threshold {
                    state.coords[day].coords[index] = prev_x;
                    continue;
                }

                let Ok(area_score) = state.calc_area_score(&env, day) else {
                    state.coords[day].coords[index] = prev_x;
                    continue;
                };

                let new_score_day = area_score + prev_line_score + next_line_score;
                let score_diff = new_score_day - prev_score_day;

                if new_score_day as f64 <= score_threshold {
                    // 解の更新
                    current_score += score_diff;
                    state.area_scores[day] = area_score;
                    state.line_scores[day] = prev_line_score;
                    state.line_scores[day + 1] = next_line_score;

                    if best_score.change_min(current_score) {
                        best_solution = state.clone();
                    }
                } else {
                    state.coords[day].coords[index] = prev_x;
                }
            }

            eprintln!("all iter: {}", all_iter);

            best_solution
        }
    }
    pub mod bin_packing {
        pub mod step1 {
            use std::time::Instant;

            use itertools::Itertools as _;
            use rand::{Rng as _, SeedableRng as _};

            use crate::{common::ChangeMinMax as _, problem::Input};

            pub fn get_best_width(input: &Input, duration: f64) -> (Vec<Vec<i32>>, usize) {
                let since = Instant::now();
                let mut dividers = vec![0, Input::W];
                let mut all_divs = vec![vec![]; input.n + 1];
                all_divs[1] = dividers.clone();

                let mut ok = 1;
                let mut ng = input.n;

                while ng - ok > 1 {
                    let mid = (ok + ng) / 2;
                    let state = gen_dividers(input, mid);
                    let score = state.calc_score(input).unwrap();

                    if score <= 0 {
                        ok = mid;
                    } else {
                        ng = mid;
                    }

                    let mut divs = vec![0];

                    for &w in state.widths.iter() {
                        let w = w + divs.last().unwrap();
                        divs.push(w);
                    }

                    all_divs[mid] = divs;

                    if since.elapsed().as_secs_f64() > duration {
                        break;
                    }
                }

                let div_size = ok;

                dividers = all_divs[ok].clone();
                let mut divider_set = vec![dividers];

                if div_size == 1 {
                    return (divider_set, div_size);
                }

                for _ in 0..3 {
                    if since.elapsed().as_secs_f64() > duration {
                        break;
                    }

                    let state = gen_dividers(input, div_size);
                    let score = state.calc_score(input).unwrap();

                    if score > 0 {
                        continue;
                    }

                    let mut divs = vec![0];

                    for &w in state.widths.iter() {
                        let w = w + divs.last().unwrap();
                        divs.push(w);
                    }

                    divider_set.push(divs);
                }

                (divider_set, div_size)
            }

            fn gen_dividers(input: &Input, div: usize) -> State {
                let lines = (0..=div)
                    .map(|i| (Input::W as usize * i / div) as i32)
                    .collect_vec();
                let widths = lines
                    .iter()
                    .tuple_windows()
                    .map(|(&a, &b)| b - a)
                    .collect_vec();
                let mut state = State::new(widths);
                state = annealing(input, state, 0.01);

                state.widths.sort_unstable();
                state
            }

            #[derive(Debug, Clone)]
            struct State {
                widths: Vec<i32>,
            }

            impl State {
                fn new(widths: Vec<i32>) -> Self {
                    Self { widths }
                }

                fn calc_score(&self, input: &Input) -> Result<i64, ()> {
                    if self.widths.iter().any(|&w| w <= 0) {
                        return Err(());
                    }

                    let mut score = 0;

                    let mut used = vec![false; self.widths.len()];
                    let bins = self.widths.iter().map(|&w| w * Input::W).collect_vec();

                    for reqs in input.requests.iter() {
                        let mut bins = bins.clone();
                        used.fill(false);

                        // best-fit algoithm
                        for &req in reqs.iter().rev() {
                            let mut best_i = !0;
                            let mut best_score = i64::MAX;

                            for (i, &bin) in bins.iter().enumerate() {
                                let score = (bin - req) as i64;
                                let score = if score < 0 {
                                    score * -1000000000
                                } else {
                                    score
                                };

                                if best_score.change_min(score) {
                                    best_i = i;
                                }
                            }

                            bins[best_i] -= req;
                            used[best_i] = true;
                        }

                        for &b in bins.iter() {
                            score += (-b).max(0) as i64;
                        }

                        // 未使用の仕切りがあればペナルティ
                        score += used.iter().filter(|&&b| !b).count() as i64 * 100;
                    }

                    // 最大幅が小さいほどボーナス
                    score -= (Input::W / 2 - *self.widths.iter().max().unwrap()) as i64 / 10;

                    Ok(score)
                }
            }

            fn annealing(input: &Input, initial_solution: State, duration: f64) -> State {
                let mut solution = initial_solution;
                let mut best_solution = solution.clone();
                let mut current_score = solution.calc_score(input).unwrap();
                let mut best_score = current_score;

                let mut all_iter = 0;
                let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = 1e6 * input.days as f64;
                let temp1 = 1e0;
                let mut inv_temp = 1.0 / temp0;

                loop {
                    all_iter += 1;
                    if (all_iter & ((1 << 4) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
                        inv_temp = 1.0 / temp;

                        if time >= 1.0 {
                            break;
                        }
                    }

                    // 変形
                    let i = rng.gen_range(0..solution.widths.len());
                    let mut j = rng.gen_range(0..solution.widths.len() - 1);

                    if j >= i {
                        j += 1;
                    }

                    let sign = if rng.gen_bool(0.5) { 1 } else { -1 };
                    let dx = sign * 10f64.powf(rng.gen_range(0.0..2.0)).round() as i32;
                    let mut new_state = solution.clone();
                    new_state.widths[i] -= dx;
                    new_state.widths[j] += dx;

                    // スコア計算
                    let Ok(new_score) = new_state.calc_score(input) else {
                        continue;
                    };
                    let score_diff = new_score - current_score;

                    if score_diff <= 0 || rng.gen_bool(f64::exp(-score_diff as f64 * inv_temp)) {
                        // 解の更新
                        current_score = new_score;
                        solution = new_state;

                        if best_score.change_min(current_score) {
                            best_solution = solution.clone();
                        }
                    }
                }

                best_solution
            }
        }
        mod step2 {
            use std::time::Instant;

            use itertools::Itertools;
            use rand::prelude::*;

            use crate::{
                common::ChangeMinMax as _,
                problem::{Input, Rect},
            };

            pub fn divide(
                input: &Input,
                dividers: &[Vec<i32>],
                duration: f64,
            ) -> (Vec<Vec<Rect>>, i64) {
                let since = Instant::now();
                let trial_count = (3000 / (input.days * input.n)).max(3);
                let max_beam_width = trial_count / 2;

                let separator_count = input.n - (dividers[0].len() - 1);
                let lines = (1..=separator_count)
                    .map(|i| Separator::new(0, i as i32))
                    .collect_vec();
                let init_state = State::new(lines);

                let mut beam = vec![vec![]; input.days + 1];

                for (i, dividers) in dividers.iter().enumerate() {
                    let env = Env::new(&input, dividers, 0, None);
                    beam[0].push(BeamState::new(&env, None, 1, i, !0));
                }

                for day in 0..input.days {
                    let each_duration = ((duration - since.elapsed().as_secs_f64())
                        / (input.days - day) as f64)
                        .max(0.0);

                    for i in 0..trial_count {
                        let beam_index = i % beam[day].len();
                        let beam_state = &beam[day][beam_index];
                        let prev_state = beam_state.state.clone();
                        let prev_score = beam_state.score;
                        let divider_index = beam_state.divider_index;
                        let state = prev_state.clone().unwrap_or_else(|| init_state.clone());
                        let env =
                            Env::new(&input, &dividers[beam_state.divider_index], day, prev_state);

                        let duration = each_duration / trial_count as f64;
                        let state = annealing(&env, state.clone(), duration);

                        beam[day + 1].push(BeamState::new(
                            &env,
                            Some(state),
                            prev_score,
                            divider_index,
                            beam_index,
                        ));
                    }

                    beam[day + 1].sort_unstable_by_key(|s| s.score);

                    if beam[day + 1].len() > max_beam_width {
                        beam[day + 1].truncate(max_beam_width);
                    }
                }

                let best_state = &beam[input.days][0];
                let mut rects = vec![];
                let mut day = input.days;
                let mut index = 0;

                while day > 0 {
                    let state = &beam[day][index];
                    let prev_day = day - 1;
                    let prev_index = state.prev_state_index;
                    let prev_state = beam[prev_day][prev_index].state.clone();
                    let env = Env::new(&input, &dividers[state.divider_index], day - 1, prev_state);
                    rects.push(state.state.as_ref().unwrap().to_rects(&env));
                    day = prev_day;
                    index = prev_index;
                }

                rects.reverse();

                (rects, best_state.score)
            }

            #[derive(Debug, Clone)]
            struct BeamState {
                state: Option<State>,
                divider_index: usize,
                score: i64,
                prev_state_index: usize,
            }

            impl BeamState {
                fn new(
                    env: &Env,
                    mut state: Option<State>,
                    prev_score: i64,
                    divider_index: usize,
                    prev_state_index: usize,
                ) -> Self {
                    let score = state
                        .as_mut()
                        .map(|s| s.calc_score(env).unwrap())
                        .unwrap_or(0)
                        + prev_score;
                    Self {
                        state,
                        score,
                        divider_index,
                        prev_state_index,
                    }
                }
            }

            impl PartialEq for BeamState {
                fn eq(&self, other: &Self) -> bool {
                    self.score == other.score
                }
            }

            impl Eq for BeamState {}

            impl PartialOrd for BeamState {
                fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                    self.score.partial_cmp(&other.score)
                }
            }

            impl Ord for BeamState {
                fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                    self.score.cmp(&other.score)
                }
            }

            #[derive(Debug, Clone)]
            struct Env<'a> {
                input: &'a Input,
                dividers: &'a [i32],
                widths: Vec<i32>,
                day: usize,
                prev_state: Option<State>,
            }

            impl<'a> Env<'a> {
                fn new(
                    input: &'a Input,
                    dividers: &'a [i32],
                    day: usize,
                    prev_state: Option<State>,
                ) -> Self {
                    let widths = dividers
                        .iter()
                        .tuple_windows()
                        .map(|(&a, &b)| b - a)
                        .collect_vec();

                    Self {
                        input,
                        dividers,
                        widths,
                        day,
                        prev_state,
                    }
                }
            }

            #[derive(Debug, Clone)]
            struct State {
                lines: Vec<Separator>,
            }

            impl State {
                fn new(mut lines: Vec<Separator>) -> Self {
                    lines.sort_unstable();
                    Self { lines }
                }

                fn calc_score(&mut self, env: &Env) -> Result<i64, ()> {
                    let mut score = self.calc_area_score(env)?;

                    if let Some(prev_state) = &env.prev_state {
                        score += self.calc_line_score(env, prev_state);
                    }

                    Ok(score)
                }

                fn calc_area_score(&self, env: &Env<'_>) -> Result<i64, ()> {
                    let mut areas = Vec::with_capacity(env.input.n);
                    let mut pointer = 0;

                    for (i, &width) in env.widths.iter().enumerate() {
                        let mut y = 0;

                        while pointer < self.lines.len() && self.lines[pointer].index == i {
                            let area = (self.lines[pointer].y - y) * width;
                            y = self.lines[pointer].y;

                            if area <= 0 {
                                return Err(());
                            }

                            areas.push(area);
                            pointer += 1;
                        }

                        let area = (Input::W - y) * width;

                        if area <= 0 {
                            return Err(());
                        }

                        areas.push(area);
                    }

                    areas.sort_unstable();

                    let mut score = 0;

                    for (&req, &area) in env.input.requests[env.day].iter().zip(areas.iter()) {
                        let diff = req - area;
                        score += diff.max(0);
                    }

                    Ok(score as i64 * 100)
                }

                fn calc_line_score(&self, env: &Env, prev_state: &State) -> i64 {
                    let mut score = 0;
                    let mut ptr0 = 0;
                    let mut ptr1 = 0;

                    let lines0 = &prev_state.lines;
                    let lines1 = &self.lines;
                    let inf = Separator::new(usize::MAX, i32::MAX);

                    while ptr0 < lines0.len() || ptr1 < lines1.len() {
                        let line0 = lines0.get(ptr0).copied().unwrap_or(inf);
                        let line1 = lines1.get(ptr1).copied().unwrap_or(inf);

                        if line0 == line1 {
                            ptr0 += 1;
                            ptr1 += 1;
                        } else if line0 < line1 {
                            score += env.widths[line0.index];
                            ptr0 += 1;
                        } else {
                            score += env.widths[line1.index];
                            ptr1 += 1;
                        }
                    }

                    score as i64
                }

                fn to_rects(&self, env: &Env) -> Vec<Rect> {
                    let mut rects = vec![];
                    let mut pointer = 0;

                    for (i, (&x0, &x1)) in env.dividers.iter().tuple_windows().enumerate() {
                        let mut y = 0;

                        while pointer < self.lines.len() && self.lines[pointer].index == i {
                            let rect = Rect::new(x0, y, x1, self.lines[pointer].y);
                            y = self.lines[pointer].y;

                            assert!(rect.is_valid());

                            rects.push(rect);
                            pointer += 1;
                        }

                        let rect = Rect::new(x0, y, x1, Input::W);

                        assert!(rect.is_valid());

                        rects.push(rect);
                    }

                    glidesort::sort_by_key(&mut rects, |r| r.area());

                    rects
                }
            }

            #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
            struct Separator {
                index: usize,
                y: i32,
            }

            impl Separator {
                fn new(index: usize, y: i32) -> Self {
                    Self { index, y }
                }
            }

            fn annealing(env: &Env, mut state: State, duration: f64) -> State {
                if duration <= 0.0 {
                    return state;
                }

                let mut current_score = state.calc_score(&env).unwrap();
                let mut best_solution = state.clone();
                let mut best_score = current_score;

                let mut all_iter = 0;
                let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = 1e6;
                let temp1 = 1e0;
                let mut inv_temp = 1.0 / temp0;

                loop {
                    all_iter += 1;
                    if (all_iter & ((1 << 4) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
                        inv_temp = 1.0 / temp;

                        if time >= 1.0 {
                            break;
                        }
                    }

                    // 変形
                    let neigh_type = rng.gen_range(0..10);

                    let mut new_state = if neigh_type < 5 {
                        let mut index = rng.gen_range(0..state.lines.len());
                        let sign = if rng.gen_bool(0.5) { 1 } else { -1 };
                        let dy = sign * 10f64.powf(rng.gen_range(0.0..3.0)).round() as i32;
                        let mut new_state = state.clone();
                        new_state.lines[index].y += dy;

                        while index > 0 && new_state.lines[index] < new_state.lines[index - 1] {
                            new_state.lines.swap(index, index - 1);
                            index -= 1;
                        }

                        while index + 1 < new_state.lines.len()
                            && new_state.lines[index] > new_state.lines[index + 1]
                        {
                            new_state.lines.swap(index, index + 1);
                            index += 1;
                        }

                        new_state
                    } else if neigh_type < 7 {
                        let mut index = rng.gen_range(0..state.lines.len());
                        let new_index = rng.gen_range(0..env.widths.len());
                        let new_y = rng.gen_range(1..Input::W);
                        let mut new_state = state.clone();
                        new_state.lines[index] = Separator::new(new_index, new_y);

                        while index > 0 && new_state.lines[index] < new_state.lines[index - 1] {
                            new_state.lines.swap(index, index - 1);
                            index -= 1;
                        }

                        while index + 1 < new_state.lines.len()
                            && new_state.lines[index] > new_state.lines[index + 1]
                        {
                            new_state.lines.swap(index, index + 1);
                            index += 1;
                        }

                        new_state
                    } else if neigh_type == 7 {
                        let Some(prev_state) = &env.prev_state else {
                            continue;
                        };

                        let i0 = rng.gen_range(0..prev_state.lines.len());
                        let i1 = rng.gen_range(0..state.lines.len());
                        let mut new_state = state.clone();
                        new_state.lines[i1] = prev_state.lines[i0];

                        let mut index = i1;

                        while index > 0 && new_state.lines[index] < new_state.lines[index - 1] {
                            new_state.lines.swap(index, index - 1);
                            index -= 1;
                        }

                        while index + 1 < new_state.lines.len()
                            && new_state.lines[index] > new_state.lines[index + 1]
                        {
                            new_state.lines.swap(index, index + 1);
                            index += 1;
                        }

                        new_state
                    } else if neigh_type == 8 {
                        let index0 = rng.gen_range(0..env.widths.len());
                        let diff = rng.gen_range(1..=3);

                        let index1 = if rng.gen_bool(0.5) {
                            index0 + diff
                        } else {
                            index0.wrapping_sub(diff)
                        };

                        if index1 >= env.widths.len() {
                            continue;
                        }

                        let mut new_state = state.clone();

                        for sep in new_state.lines.iter_mut() {
                            let index = sep.index;

                            let new_sep = if index == index0 {
                                Separator::new(index1, sep.y)
                            } else if index == index1 {
                                Separator::new(index0, sep.y)
                            } else {
                                Separator::new(index, sep.y)
                            };

                            *sep = new_sep;
                        }
                        new_state.lines.sort_unstable();

                        new_state
                    } else {
                        let i = rng.gen_range(0..env.widths.len());

                        let mut targets = vec![];

                        for (j, l) in state.lines.iter().enumerate() {
                            if l.index == i {
                                targets.push(j);
                            }
                        }

                        if targets.len() == 0 {
                            continue;
                        }

                        glidesort::sort_by_key(&mut targets, |j| state.lines[*j].y);

                        let mut y = 0;
                        let mut dy = vec![];

                        for &j in targets.iter() {
                            dy.push(state.lines[j].y - y);
                            y = state.lines[j].y;
                        }

                        dy.push(Input::W - y);

                        dy.shuffle(&mut rng);

                        let mut y = 0;

                        let mut new_state = state.clone();

                        for (&j, &dy) in targets.iter().zip(dy.iter()) {
                            y += dy;
                            new_state.lines[j].y = y;
                        }

                        new_state.lines.sort_unstable();

                        new_state
                    };

                    // スコア計算
                    let Ok(new_score) = new_state.calc_score(&env) else {
                        continue;
                    };
                    let score_diff = new_score - current_score;

                    if score_diff <= 0 || rng.gen_bool(f64::exp(-score_diff as f64 * inv_temp)) {
                        // 解の更新
                        current_score = new_score;
                        state = new_state;

                        if best_score.change_min(current_score) {
                            best_solution = state.clone();
                        }
                    }
                }

                best_solution
            }
        }

        use std::time::Instant;

        use super::{Input, Rect};
        use crate::solver::Solver;

        pub struct BinPacking1d {
            duration: f64,
        }

        impl BinPacking1d {
            pub fn new(duration: f64) -> Self {
                Self { duration }
            }
        }

        impl Solver for BinPacking1d {
            fn solve(&self, input: &Input) -> (Vec<Vec<Rect>>, i64) {
                let since = Instant::now();
                let (dividers, div_size) = step1::get_best_width(input, self.duration);

                if div_size >= 3 {
                    let duration = (self.duration - since.elapsed().as_secs_f64()).max(0.01);
                    step2::divide(input, &dividers, duration)
                } else {
                    (vec![], i64::MAX)
                }
            }
        }
    }
    pub mod first_fit {
        mod step1 {
            use crate::{
                common::{ChangeMinMax as _, DIVISOR},
                problem::Input,
            };
            use rand::prelude::*;

            pub fn generate_init_solution(
                input: &Input,
                duration: f64,
            ) -> (Vec<i32>, Vec<Vec<(usize, i32)>>) {
                let state = State::new(vec![Input::W], vec![false; input.n]);
                let state = annealing(input, state, duration);
                let score = state.calc_score(input, i64::MAX);
                eprintln!("first_fit - step1 score: {}", score);

                state.restore(input)
            }

            #[derive(Debug, Clone)]
            struct State {
                widths: Vec<i32>,
                directions: Vec<bool>,
            }

            impl State {
                fn new(widths: Vec<i32>, directions: Vec<bool>) -> Self {
                    Self { widths, directions }
                }

                fn len(&self) -> usize {
                    self.widths.len()
                }

                fn calc_score(&self, input: &Input, threshold: i64) -> i64 {
                    let mut widths = self.widths.clone();
                    widths.sort_unstable();

                    let mut y_lanes = vec![0; widths.len()];

                    let mut score = 1;

                    for (day, requests) in input.requests.iter().enumerate() {
                        let line_mul = if day == 0 || day == input.days - 1 {
                            1
                        } else {
                            2
                        };

                        y_lanes.fill(0);

                        for (rect_j, &request) in requests.iter().enumerate().rev() {
                            let mut next_i = !0;
                            let mut next_y = i32::MAX;

                            // first-fitをする
                            if self.directions[rect_j] {
                                for (i, (&width, &y)) in
                                    widths.iter().zip(y_lanes.iter()).enumerate().rev()
                                {
                                    let dy = DIVISOR.div((request + width - 1) as u32, width as u32)
                                        as i32;
                                    let y = y + dy;

                                    if next_y.change_min(y) {
                                        next_i = i;
                                    }

                                    if next_y <= Input::W {
                                        break;
                                    }
                                }
                            } else {
                                for (i, (&width, &y)) in
                                    widths.iter().zip(y_lanes.iter()).enumerate()
                                {
                                    let dy = DIVISOR.div((request + width - 1) as u32, width as u32)
                                        as i32;
                                    let y = y + dy;

                                    if next_y.change_min(y) {
                                        next_i = i;
                                    }

                                    if next_y <= Input::W {
                                        break;
                                    }
                                }
                            }

                            // 各レーン2個目以降はコストがかかる
                            if y_lanes[next_i] > 0 {
                                score += widths[next_i] as i64 * line_mul;
                            }

                            // 溢れるケース
                            score += 100
                                * ((next_y - Input::W.max(y_lanes[next_i])).max(0) * widths[next_i])
                                    as i64;

                            y_lanes[next_i] = next_y;
                        }

                        // 1つ以上入れないとダメ
                        score += y_lanes.iter().filter(|&&y| y == 0).count() as i64 * 1000;

                        // 閾値以下にならないことが確定したら打ち切り
                        if score > threshold {
                            return 1 << 50;
                        }
                    }

                    score
                }

                fn restore(&self, input: &Input) -> (Vec<i32>, Vec<Vec<(usize, i32)>>) {
                    let dividers = self.restore_dividers();
                    let separatators = self.restore_separators(input);
                    (dividers, separatators)
                }

                fn restore_dividers(&self) -> Vec<i32> {
                    let mut x = 0;
                    let mut dividers = vec![0];

                    let mut widths = self.widths.clone();
                    widths.sort_unstable();

                    for w in widths.iter() {
                        x += *w;
                        dividers.push(x);
                    }

                    dividers
                }

                fn restore_separators(&self, input: &Input) -> Vec<Vec<(usize, i32)>> {
                    let mut all_separators = vec![];
                    let mut widths = self.widths.clone();
                    widths.sort();

                    let mut y_lanes = vec![0; widths.len()];

                    for requests in input.requests.iter() {
                        let mut lane_rects = vec![vec![]; widths.len()];
                        y_lanes.fill(0);

                        for (rect_j, &request) in requests.iter().enumerate().rev() {
                            let mut next_i = !0;
                            let mut next_y = i32::MAX;

                            // first-fitをする
                            if self.directions[rect_j] {
                                for (i, (&width, &y)) in
                                    widths.iter().zip(y_lanes.iter()).enumerate().rev()
                                {
                                    let dy = DIVISOR.div((request + width - 1) as u32, width as u32)
                                        as i32;
                                    let y = y + dy;

                                    if next_y.change_min(y) {
                                        next_i = i;
                                    }

                                    if next_y <= Input::W {
                                        break;
                                    }
                                }
                            } else {
                                for (i, (&width, &y)) in
                                    widths.iter().zip(y_lanes.iter()).enumerate()
                                {
                                    let dy = DIVISOR.div((request + width - 1) as u32, width as u32)
                                        as i32;
                                    let y = y + dy;

                                    if next_y.change_min(y) {
                                        next_i = i;
                                    }

                                    if next_y <= Input::W {
                                        break;
                                    }
                                }
                            }

                            lane_rects[next_i].push(next_y - y_lanes[next_i]);
                            y_lanes[next_i] = next_y;
                        }

                        let mut separators = vec![];

                        for i in 0..widths.len() {
                            let mut y = 0;
                            let mut local_separators = vec![];

                            for &dy in lane_rects[i].iter() {
                                y += dy;
                                local_separators.push(y);
                            }

                            if let Some(&last) = local_separators.last() {
                                for y in local_separators.iter_mut() {
                                    *y *= Input::W;
                                    *y /= last;
                                }

                                local_separators.pop();

                                for &y in local_separators.iter() {
                                    separators.push((i, y));
                                }
                            }
                        }

                        let separator_count = input.n - self.widths.len();

                        if separators.len() > separator_count {
                            separators.truncate(separator_count);
                        }

                        all_separators.push(separators);
                    }

                    all_separators
                }
            }

            fn annealing(input: &Input, initial_solution: State, duration: f64) -> State {
                let mut state = initial_solution;
                let mut best_state = state.clone();
                let mut current_score = state.calc_score(input, i64::MAX);
                let mut best_score = current_score;

                let mut all_iter = 0;
                let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = input.first_fit_config.step1_temp0;
                let temp1 = input.first_fit_config.step1_temp1;
                let mut temp = temp0;

                loop {
                    all_iter += 1;
                    if (all_iter & ((1 << 4) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);

                        if time >= 1.0 {
                            break;
                        }
                    }

                    // 変形
                    let neigh_type = rng.gen_range(0..15);

                    let mut new_state = if neigh_type < 8 {
                        if state.len() <= 1 {
                            continue;
                        }

                        let i = rng.gen_range(0..state.len());
                        let mut j = rng.gen_range(0..state.len() - 1);

                        if j >= i {
                            j += 1;
                        }

                        if state.widths[i] <= 1 {
                            continue;
                        }

                        let mut new_state = state.clone();
                        let dw = rng.gen_range(1..state.widths[i]);

                        new_state.widths[i] -= dw;
                        new_state.widths[j] += dw;
                        new_state
                    } else if neigh_type == 8 {
                        if state.len() >= input.n - 1 {
                            continue;
                        }

                        let i = rng.gen_range(0..state.len());

                        if state.widths[i] <= 1 {
                            continue;
                        }

                        let mut new_state = state.clone();
                        let dw = rng.gen_range(1..state.widths[i]);
                        new_state.widths[i] -= dw;
                        new_state.widths.push(dw);
                        new_state
                    } else if neigh_type == 9 {
                        if state.len() <= 1 {
                            continue;
                        }

                        let i = rng.gen_range(0..state.len());
                        let mut j = rng.gen_range(0..state.len() - 1);

                        if j >= i {
                            j += 1;
                        }

                        let mut new_state = state.clone();
                        new_state.widths[i] += new_state.widths[j];
                        new_state.widths.remove(j);
                        new_state
                    } else {
                        let i = rng.gen_range(0..state.directions.len());
                        let mut new_state = state.clone();
                        new_state.directions[i] ^= true;
                        new_state
                    };

                    new_state.widths.sort_unstable();

                    // スコア計算
                    // 先に閾値を求めることで評価を高速化する
                    let score_threshold =
                        current_score - (temp * rng.gen_range(0.0f64..1.0).ln()).round() as i64;

                    let new_score = new_state.calc_score(input, score_threshold);

                    if new_score <= score_threshold {
                        // 解の更新
                        current_score = new_score;
                        state = new_state;

                        if best_score.change_min(current_score) {
                            best_state = state.clone();
                        }
                    }
                }

                eprintln!("all iter: {}", all_iter);

                best_state
            }
        }
        mod step2 {
            use itertools::Itertools;
            use rand::prelude::*;

            use crate::{
                common::ChangeMinMax as _,
                problem::{Input, Rect},
            };

            const INF: i64 = 1 << 50;

            pub fn devide(
                input: &Input,
                dividers: &[i32],
                separators: Vec<Vec<(usize, i32)>>,
                duration: f64,
            ) -> (Vec<Vec<Rect>>, i64) {
                let env = Env::new(input, dividers);

                let separators = separators
                    .into_iter()
                    .map(|seps| {
                        seps.into_iter()
                            .map(|(i, y)| Separator::new(i, y))
                            .collect_vec()
                    })
                    .collect_vec();

                let state = State::new(separators, &env);

                let mut state = annealing(&env, state, duration);
                let score = state.calc_score(&env).unwrap_or(INF);

                (state.to_rects(&env), score)
            }

            #[derive(Debug, Clone)]
            struct Env<'a> {
                input: &'a Input,
                dividers: &'a [i32],
                widths: Vec<i32>,
            }

            impl<'a> Env<'a> {
                fn new(input: &'a Input, dividers: &'a [i32]) -> Self {
                    let widths = dividers
                        .iter()
                        .tuple_windows()
                        .map(|(&a, &b)| b - a)
                        .collect_vec();

                    Self {
                        input,
                        dividers,
                        widths,
                    }
                }
            }

            static mut AREA_BUF: Vec<i32> = vec![];

            #[derive(Debug, Clone)]
            struct State {
                lines: Vec<Vec<Separator>>,
                area_scores: Vec<i64>,
                line_scores: Vec<i64>,
            }

            impl State {
                fn new(mut lines: Vec<Vec<Separator>>, env: &Env) -> Self {
                    for lines in lines.iter_mut() {
                        glidesort::sort(lines);
                    }

                    let mut state = Self {
                        lines,
                        area_scores: vec![],
                        line_scores: vec![],
                    };

                    for day in 0..env.input.days {
                        let s = state.calc_area_score(env, day).unwrap_or(INF);
                        state.area_scores.push(s);
                    }

                    for day in 0..env.input.days + 1 {
                        let s = state.calc_line_score(env, day);
                        state.line_scores.push(s);
                    }

                    state
                }

                fn calc_score(&mut self, env: &Env) -> Result<i64, ()> {
                    let mut score = 1;

                    for lines in self.lines.iter_mut() {
                        glidesort::sort(lines);
                    }

                    for day in 0..env.input.days {
                        score += self.calc_area_score(env, day)?;
                        score += self.calc_line_score(env, day);
                    }

                    Ok(score)
                }

                fn calc_area_score(&self, env: &Env, day: usize) -> Result<i64, ()> {
                    let areas = unsafe {
                        AREA_BUF.clear();
                        &mut AREA_BUF
                    };
                    let lines = &self.lines[day];

                    let mut pointer = 0;

                    for (i, &width) in env.widths.iter().enumerate() {
                        let mut y = 0;

                        while pointer < lines.len() {
                            let line = &lines[pointer];

                            if line.index() != i {
                                break;
                            }

                            let area = (line.y - y) * width;
                            y = line.y;

                            if area <= 0 {
                                return Err(());
                            }

                            areas.push(area);
                            pointer += 1;
                        }

                        let area = (Input::W - y) * width;

                        if area <= 0 {
                            return Err(());
                        }

                        areas.push(area);
                    }

                    areas.sort_unstable();

                    let mut score = 0;

                    for (&req, &area) in env.input.requests[day].iter().zip(areas.iter()) {
                        let diff = (req - area) as i64;
                        score += diff.max(0);
                    }

                    Ok(score * 100)
                }

                fn calc_line_score(&self, env: &Env, day: usize) -> i64 {
                    let Some(lines0) = self.lines.get(day.wrapping_sub(1)) else {
                        return 0;
                    };
                    let Some(lines1) = self.lines.get(day) else {
                        return 0;
                    };

                    let mut score = 0;
                    let mut ptr0 = 0;
                    let mut ptr1 = 0;

                    let inf = Separator::new(usize::MAX, i32::MAX);
                    let mut line0 = lines0[ptr0];
                    let mut line1 = lines1[ptr1];
                    let n = lines0.len();

                    while ptr0 < n || ptr1 < n {
                        if line0 == line1 {
                            ptr0 += 1;
                            ptr1 += 1;
                            line0 = if ptr0 < lines0.len() {
                                lines0[ptr0]
                            } else {
                                inf
                            };
                            line1 = if ptr1 < lines1.len() {
                                lines1[ptr1]
                            } else {
                                inf
                            };
                        } else if line0 < line1 {
                            score += env.widths[line0.index()];
                            ptr0 += 1;
                            line0 = if ptr0 < lines0.len() {
                                lines0[ptr0]
                            } else {
                                inf
                            };
                        } else {
                            score += env.widths[line1.index()];
                            ptr1 += 1;
                            line1 = if ptr1 < lines1.len() {
                                lines1[ptr1]
                            } else {
                                inf
                            };
                        }
                    }

                    score as i64
                }

                fn to_rects(&self, env: &Env) -> Vec<Vec<Rect>> {
                    let mut all_rects = vec![];

                    for day in 0..env.input.days {
                        let mut rects = vec![];
                        let mut pointer = 0;
                        let lines = &self.lines[day];

                        for (i, (&x0, &x1)) in env.dividers.iter().tuple_windows().enumerate() {
                            let mut y = 0;

                            while pointer < lines.len() && lines[pointer].index() == i {
                                let rect = Rect::new(x0, y, x1, lines[pointer].y);
                                y = lines[pointer].y;

                                assert!(rect.is_valid());

                                rects.push(rect);
                                pointer += 1;
                            }

                            let rect = Rect::new(x0, y, x1, Input::W);

                            rects.push(rect);
                        }

                        glidesort::sort_by_key(&mut rects, |r| r.area());

                        all_rects.push(rects);
                    }

                    all_rects
                }
            }

            #[derive(Debug, Clone, Copy, Hash)]
            struct Separator {
                index: u32,
                y: i32,
                v: u64,
            }

            impl Separator {
                fn new(index: usize, y: i32) -> Self {
                    let v = (index as u64) << 32 | (y + (1 << 16)) as u64;
                    Self {
                        index: index as u32,
                        y,
                        v,
                    }
                }

                fn index(&self) -> usize {
                    self.index as usize
                }
            }

            impl PartialEq for Separator {
                fn eq(&self, other: &Self) -> bool {
                    self.v == other.v
                }
            }

            impl Eq for Separator {}

            impl PartialOrd for Separator {
                fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                    self.v.partial_cmp(&other.v)
                }
            }

            impl Ord for Separator {
                fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                    self.v.cmp(&other.v)
                }
            }

            fn annealing(env: &Env, mut state: State, duration: f64) -> State {
                let mut current_score = state.calc_score(&env).unwrap_or(INF);
                let mut best_solution = state.clone();
                let mut best_score = current_score;

                let mut all_iter = 0;
                let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

                let duration_inv = 1.0 / duration;
                let since = std::time::Instant::now();

                let temp0 = env.input.first_fit_config.step2_temp0;
                let temp1 = env.input.first_fit_config.step2_temp1;
                let mut temp = temp0;
                let mut new_line_buffer = state.lines[0].clone();

                loop {
                    all_iter += 1;
                    if (all_iter & ((1 << 4) - 1)) == 0 {
                        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
                        temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);

                        if time >= 1.0 {
                            break;
                        }
                    }

                    // 変形
                    let neigh_type = rng.gen_range(0..4);
                    let day = rng.gen_range(0..env.input.days);
                    let index = rng.gen_range(0..state.lines[day].len());

                    let mut new_lines = if neigh_type == 0 {
                        let new_y = loop {
                            let sign = if rng.gen_bool(0.5) { 1 } else { -1 };
                            let dy = sign * 10f64.powf(rng.gen_range(0.0..3.0)).round() as i32;
                            let new_y = state.lines[day][index].y + dy;

                            if 0 < new_y && new_y < Input::W {
                                break new_y;
                            }
                        };

                        let new_lines = &mut new_line_buffer;
                        new_lines.copy_from_slice(&state.lines[day]);
                        new_lines[index] = Separator::new(state.lines[day][index].index(), new_y);
                        new_lines
                    } else if neigh_type == 1 {
                        let new_index = rng.gen_range(0..env.widths.len());
                        let new_y = rng.gen_range(1..Input::W);
                        let new_lines = &mut new_line_buffer;
                        new_lines.copy_from_slice(&state.lines[day]);
                        new_lines[index] = Separator::new(new_index, new_y);
                        new_lines
                    } else {
                        let day_diff = if rng.gen_bool(0.5) { 1 } else { -1 };

                        let Some(target_lines) = state.lines.get(day.wrapping_add_signed(day_diff))
                        else {
                            continue;
                        };

                        let i0 = rng.gen_range(0..target_lines.len());
                        let new_lines = &mut new_line_buffer;
                        new_lines.copy_from_slice(&state.lines[day]);
                        new_lines[index] = target_lines[i0];
                        new_lines
                    };

                    let line = new_lines.remove(index);
                    let index = new_lines.binary_search(&line).unwrap_or_else(|i| i);
                    new_lines.insert(index, line);

                    // スコア計算
                    // 先に閾値を求めることで評価を高速化する
                    let old_score = state.area_scores[day]
                        + state.line_scores[day]
                        + state.line_scores[day + 1];
                    let score_threshold = old_score as f64 - temp * rng.gen_range(0.0f64..1.0).ln();

                    std::mem::swap(&mut state.lines[day], &mut new_lines);

                    let prev_line_score = state.calc_line_score(env, day);

                    if prev_line_score as f64 > score_threshold {
                        std::mem::swap(&mut state.lines[day], &mut new_lines);
                        continue;
                    }

                    let next_line_score = state.calc_line_score(env, day + 1);

                    if (prev_line_score + next_line_score) as f64 > score_threshold {
                        std::mem::swap(&mut state.lines[day], &mut new_lines);
                        continue;
                    }

                    let Ok(area_score) = state.calc_area_score(&env, day) else {
                        std::mem::swap(&mut state.lines[day], &mut new_lines);
                        continue;
                    };

                    let new_score = area_score + prev_line_score + next_line_score;
                    let score_diff = new_score - old_score;

                    if new_score as f64 <= score_threshold {
                        // 解の更新
                        current_score += score_diff;
                        state.area_scores[day] = area_score;
                        state.line_scores[day] = prev_line_score;
                        state.line_scores[day + 1] = next_line_score;

                        if best_score.change_min(current_score) {
                            best_solution = state.clone();
                        }
                    } else {
                        std::mem::swap(&mut state.lines[day], &mut new_lines);
                    }
                }

                best_solution
            }
        }

        use super::{Input, Rect};
        use crate::solver::Solver;

        pub struct FirstFitPacking {
            duration1: f64,
            duration2: f64,
        }

        impl FirstFitPacking {
            pub fn new(duration1: f64, duration2: f64) -> Self {
                Self {
                    duration1,
                    duration2,
                }
            }
        }

        impl Solver for FirstFitPacking {
            fn solve(&self, input: &Input) -> (Vec<Vec<Rect>>, i64) {
                let (dividers, separators) = step1::generate_init_solution(input, self.duration1);
                step2::devide(input, &dividers, separators, self.duration2)
            }
        }
    }

    use crate::problem::{Input, Rect};

    pub trait Solver {
        fn solve(&self, input: &Input) -> (Vec<Vec<Rect>>, i64);
    }
}
mod params {
    use nalgebra::{DMatrix, DVector, DVectorView};

    // 初回パラメータ調整分
    const D1: &[u8] = b"mpmZmZmZ6T9cj8L1KFzvP7gehetRuO4/mpmZmZmZyT+kcD0K16PgP7gehetRuM4/uB6F61G4zj9mZmZmZmbmP3E9CtejcN0/cT0K16Nw3T/NzMzMzMzsP4XrUbgehes/9ihcj8L16D+kcD0K16PgPylcj8L1KNw/AAAAAAAA4D9xPQrXo3DtPxSuR+F6FO4/mpmZmZmZuT8pXI/C9SjcP5qZmZmZmck/MzMzMzMz4z+F61G4HoXrP+xRuB6F6+E/AAAAAAAA4D8fhetRuB7lP2ZmZmZmZuY/FK5H4XoU7j+4HoXrUbjuP+xRuB6F68E/uB6F61G47j/sUbgehevhP5qZmZmZmbk/KVyPwvUozD+4HoXrUbi+Pw==";
    const N1: &[u8] = b"j8L1KFyP4j9xPQrXo3DdPwAAAAAAAPA/KVyPwvUo7D/D9Shcj8LVPwAAAAAAAPA/rkfhehSu5z9xPQrXo3DdP1K4HoXrUeg/mpmZmZmZ2T9mZmZmZmbmP/YoXI/C9eg/j8L1KFyP4j+kcD0K16PgP5qZmZmZmck/mpmZmZmZyT9mZmZmZmbmPylcj8L1KNw/KVyPwvUozD8pXI/C9SjMP65H4XoUruc/pHA9Ctej4D/sUbgehevhP3sUrkfhetQ/pHA9Ctej4D+F61G4HoXrP/YoXI/C9eg/AAAAAAAA8D97FK5H4XrUP+F6FK5H4eo/uB6F61G43j/sUbgehevRP9ejcD0K1+M/rkfhehSu5z/sUbgehevhPw==";
    const E1: &[u8] = b"eCisVdsk4j9Ke2rbUWviPwJkIvc0PO0/CKwcWmQ73z85UilnmTPiP6224FT6wug/Mwc3QH0uvz+vkiOmDybbPx9gFxi8PLs/fyC6PKsT5D8Gvc5YlJDrP5huEoPAyuk/b9UVzFDK0j+zNpwhRki/Pw7qk9lFSss/EPegqd/SzT+ereKYgcDmPy6SuAFnROk/bzFCuBAxyD82noegDXruP24KuIzIB+U/YsNyjLLR2D/d7I4rMVPFP/xR4k67ytk/Kghh7/k/xD/p85IJm/bePy12ef7qXcs/zqIra7aWwD/s03VE5gTcP96V0DSBL+g/F9nO91Pj1T/IlTsKMonZP3B5TBma6NA/SyzgD1uP7j9/Jb7Eey/UPw==";
    const R: &[u8] = b"WgBLlj4Czj/k2gg3Zq/RP95Z2GksVtE/4mfNlHMZ0j/wFRQQ0RbGP28mFg2tl9I/hT3PIt4e3T+/TH80nbzWPyzoobNCVck/NYeJKt+w2z+Y40CGRTnRPznJT1IbuuI/NWdCiUPA0z8BLtk6D+/UP5CpR5vXrd4/QZJowglm1T+pJ7jHeIrLP5qZmZmZmbk/yO9DiEPI4D8cGvAbA2/QP4CBs4Vwk9M/mpmZmZmZuT/qTZRYn2PSPweHK+ZTJsI/zGJo8LYn2j+rOL8NsazFP2884peLBN4/ccu0z/7K1z9W01YwVtKpP2VlgERN4uE/7KHOM7x01T+amZmZmZm5P8PD0ca5Nt4/l53A1u/2sj9FoSyPvUjfPw==";
    const T00: &[u8] = b"1JC8bIu1HEBzELOkV9gZQDGBFlEPQxhA7D0Zwcg0FkCd1gewBf0YQND84gN2yRZALG6fibIoFkD+0FBfhK0UQEJD7h6HnBVAYbQZWr5wFkBcy5iy+1oUQMWNVSLnqRRASYijnAzpGEB3Un5/dNMYQMSQRmGt9R9A259AoQZYG0A7LQYVFsAZQAAAAAAAABxAKxwnuLlWFED7St+Nl8gYQE+6PqbLrxdAAAAAAAAAHEBcw/oNDhQXQEx78pzBax9AUDEyyuknF0Ac0zC1IqsXQGnihu1uUBdAGqYv6EHmFkDSIRtoV6kdQAHALi6f5htAYY+73W5EF0AAAAAAAAAcQHdFwZQE4RhA4UUCZwLdHECf1VCfwSsXQA==";
    const T01: &[u8] = b"Rf+OzENPxL8+NkeEb9Kevw8OMp9iAtU/cL3lecJo0b/kWUQJ8vDpPwbaai+R2Ok/2tOKRqQ89z9U34IoUKnXv1qApgbTA90/+vhP3Aki7z8fGXuNx4DfP1pLuo4T7+Y/VT7lMVzZ9j/ykdRXAerhPzfotz+s6eM/BtgPa4Lj2T9cLKNaHAXRvwAAAAAAAAAAf1WOF+sU8j8o315xEhzUP4zLB5lLvek/AAAAAAAAAADIXiIcekzzP3KzOChhCJi/Pk7GNixX7D82V6rVkZzrP7i9KuJXROs/fxELUVLM17//tKevbFfvPyQn/N7NsvI/N9VXXc/Xrr8AAAAAAAAAAFK3w/qn+PY/zs7/umUVyj+M767K2u7hPw==";
    const T10: &[u8] = b"YNuZfawBBEBtNaVZfdIHQCoaYI9qXP4/7SUiW8WwA0A3aXxb3BoEQIM28hymAv0/j+uZbymSC0DGW0872z8BQJIOx+a7FQtA1XraNTwTAkD1T6M3Gg8GQPalddexvQFAhpkOJ8H0/z81X7XmEloLQDN4EDfUfAtA0mDJUB+mB0A1jSa8pOsEQEDvE2qCaAJAu1kaLFioCUCwYtWjpH0CQCbG85POZ/0/QO8TaoJoAkDgQqCCSLD4P+to/kBc+QVA2ZgZYvoOB0CjpCK5Nlb/P/gWLlrUmP0/aFiVO5Tc/z8HwSxaWnsGQBX1HJAPswZAkjg7Mq0L+D9A7xNqgmgCQPV2bDxDmgJA/V9Qxf2vAEBObVOiowkAQA==";
    const T11: &[u8] = b"u7RpwyNQ17+byHERSFidvzyQ4F2he+C/yZzbbprX0j8UBoBZOcPvP8JDU2fmVeC/mkjba0U29z923GhJk5PpP2H9fYhM6+8/IrRi8+BB8T8EU9gbXTqwPx4KbdR66dw/YSBQ1y5Z5j9s2Ireocf0P7NGj1QjIPY/kPMNUgUq5D9Yl5LEOvbfP/3VT5Ynid4/Zv0L4HZT1j/MWu3yG+n2P1SP+98tNps//dVPlieJ3j/S33hYO2fsP42wkx7bZb4/lv47xGjq8D8S3WSu76m4P+M1C3+urqK/sXNz8knG6T+KCTjZ9LDsP91LHSG9/p0/UreVb6EY7j/91U+WJ4neP7HIHfySOeU/eRAlcgqcwr/uvYoN4GntPw==";
    const PARAM_R: &[u8] = b"FfCuGCAR/D4ukq2Qo9+SP07YczbDp3s/9hohCeTtUj8=";
    const PARAM_T00: &[u8] = b"orNE4KgkOD+FubF9d5/cP9sPHw9VRXo/DkUyGhhYvD8=";
    const PARAM_T01: &[u8] = b"ImZNmTVEyD79XIYnl/nTP5kfssGNppc/YxDybVBigD8=";
    const PARAM_T10: &[u8] = b"lUWOw/LiDD+D2YFRpnTMP1GvfepS+78/yvwDCx7IuT8=";
    const PARAM_T11: &[u8] = b"8rBnEActyD6bhnL/bmPKP7Hkggq2s9w/bqihl2x+wz8=";

    pub struct ParamSuggester {
        x_matrix: DMatrix<f64>,
        y_vector: DVector<f64>,
        hyper_param: DVector<f64>,
        y_inv_trans: fn(f64) -> f64,
        lower: f64,
        upper: f64,
    }

    impl ParamSuggester {
        fn new(
            hyper_param: DVector<f64>,
            x_matrix: DMatrix<f64>,
            y_vector: DVector<f64>,
            y_inv_trans: fn(f64) -> f64,
            lower: f64,
            upper: f64,
        ) -> Self {
            Self {
                hyper_param,
                x_matrix,
                y_vector,
                y_inv_trans,
                lower,
                upper,
            }
        }

        fn gen_x_matrix_1() -> DMatrix<f64> {
            let d = DVector::from_vec(decode_base64(D1)).transpose();
            let n = DVector::from_vec(decode_base64(N1)).transpose();
            let e = DVector::from_vec(decode_base64(E1)).transpose();

            let x_matrix = DMatrix::from_rows(&[d, n, e]);

            x_matrix
        }

        pub fn gen_ratio_pred() -> Self {
            let hyper_param = DVector::from_vec(decode_base64(PARAM_R));
            let y_vector = DVector::from_vec(decode_base64(R));
            Self::new(
                hyper_param,
                Self::gen_x_matrix_1(),
                y_vector,
                |x| x,
                0.05,
                0.8,
            )
        }

        pub fn gen_t00_pred() -> Self {
            let hyper_param = DVector::from_vec(decode_base64(PARAM_T00));
            let y_vector = DVector::from_vec(decode_base64(T00));
            Self::new(
                hyper_param,
                Self::gen_x_matrix_1(),
                y_vector,
                |x| 10.0f64.powf(x),
                1e5,
                1e8,
            )
        }

        pub fn gen_t01_pred() -> Self {
            let hyper_param = DVector::from_vec(decode_base64(PARAM_T01));
            let y_vector = DVector::from_vec(decode_base64(T01));
            Self::new(
                hyper_param,
                Self::gen_x_matrix_1(),
                y_vector,
                |x| 10.0f64.powf(x),
                1e-1,
                1e2,
            )
        }

        pub fn gen_t10_pred() -> Self {
            let hyper_param = DVector::from_vec(decode_base64(PARAM_T10));
            let y_vector = DVector::from_vec(decode_base64(T10));
            Self::new(
                hyper_param,
                Self::gen_x_matrix_1(),
                y_vector,
                |x| 10.0f64.powf(x),
                1e1,
                1e4,
            )
        }

        pub fn gen_t11_pred() -> Self {
            let hyper_param = DVector::from_vec(decode_base64(PARAM_T11));
            let y_vector = DVector::from_vec(decode_base64(T11));
            Self::new(
                hyper_param,
                Self::gen_x_matrix_1(),
                y_vector,
                |x| 10.0f64.powf(x),
                1e-1,
                1e2,
            )
        }

        pub fn suggest(&self, d: usize, n: usize, e: f64) -> f64 {
            let d = d as f64 / 50.0;
            let n = n as f64 / 50.0;
            let e = e.sqrt() / 500.0;
            let len = self.x_matrix.shape().1;
            let y_mean = self.y_vector.mean();
            let y_mean = DVector::from_element(self.y_vector.len(), y_mean);
            let new_x = DMatrix::from_vec(3, 1, vec![d, n, e]);
            let noise = DMatrix::from_diagonal_element(len, len, self.hyper_param[3]);

            let k = self.calc_kernel_matrix(&self.x_matrix, &self.x_matrix) + noise;
            let kk = self.calc_kernel_matrix(&self.x_matrix, &new_x);

            let kernel_lu = k.lu();
            let new_y = kk.transpose() * kernel_lu.solve(&(&self.y_vector - &y_mean)).unwrap();

            (self.y_inv_trans)(new_y[(0, 0)] + y_mean[(0, 0)]).clamp(self.lower, self.upper)
        }

        fn calc_kernel_matrix(&self, x1: &DMatrix<f64>, x2: &DMatrix<f64>) -> DMatrix<f64> {
            let n = x1.shape().1;
            let m = x2.shape().1;
            let mut kernel = DMatrix::zeros(n, m);

            for i in 0..n {
                for j in 0..m {
                    kernel[(i, j)] = self.gaussian_kernel(&x1.column(i), &x2.column(j));
                }
            }

            kernel
        }

        fn gaussian_kernel(&self, x1: &DVectorView<f64>, x2: &DVectorView<f64>) -> f64 {
            let t1 = self.hyper_param[0];
            let t2 = self.hyper_param[1];
            let t3 = self.hyper_param[2];

            let diff = x1 - x2;
            let norm_diff = diff.dot(&diff);
            let dot = x1.dot(&x2);
            t1 * dot + t2 * (-norm_diff / t3).exp()
        }
    }

    fn decode_base64(data: &[u8]) -> Vec<f64> {
        const BASE64_MAP: &[u8] =
            b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        let mut stream = vec![];

        let mut cursor = 0;

        while cursor + 4 <= data.len() {
            let mut buffer = 0u32;

            for i in 0..4 {
                let c = data[cursor + i];
                let shift = 6 * (3 - i);

                for (i, &d) in BASE64_MAP.iter().enumerate() {
                    if c == d {
                        buffer |= (i as u32) << shift;
                    }
                }
            }

            for i in 0..3 {
                let shift = 8 * (2 - i);
                let value = (buffer >> shift) as u8;
                stream.push(value);
            }

            cursor += 4;
        }

        let mut result = vec![];
        cursor = 0;

        while cursor + 8 <= stream.len() {
            let p = stream.as_ptr() as *const f64;
            let x = unsafe { *p.offset(cursor as isize / 8) };
            result.push(x);
            cursor += 8;
        }

        result
    }
}

use crate::common::ChangeMinMax;
use crate::problem::Input;
use crate::solver::annealier2d::Annealer2d;
use crate::solver::first_fit::FirstFitPacking;
use solver::bin_packing::BinPacking1d;
use solver::Solver as _;

fn main() {
    let input = Input::read();
    eprintln!("packing_ratio: {:.2}%", input.packing_ratio * 100.0);

    let t0 = 2.6 * input.first_fit_config.step1_ratio;
    let t1 = 2.6 * (1.0 - input.first_fit_config.step1_ratio);
    let first_fit = FirstFitPacking::new(t0, t1);
    let (mut best_result, mut best_score) = first_fit.solve(&input);

    eprintln!("first_fit score: {}", best_score);

    let duration = 2.8 - input.since.elapsed().as_secs_f64();
    let bin_packing = BinPacking1d::new(duration);
    let (result, score) = bin_packing.solve(&input);

    eprintln!("bin_packing score: {}", score);

    if best_score.change_min(score) {
        best_result = result;
    }

    let duration = 2.98 - input.since.elapsed().as_secs_f64();

    let annealer2d = Annealer2d::new(duration);
    let (result, score) = annealer2d.solve(&input);
    eprintln!("annealer2d score: {}", score);

    if best_score.change_min(score) {
        best_result = result;
    }

    eprintln!("score: {}", best_score);

    for rects in best_result {
        for rect in rects.iter() {
            println!("{}", rect);
        }
    }
}
