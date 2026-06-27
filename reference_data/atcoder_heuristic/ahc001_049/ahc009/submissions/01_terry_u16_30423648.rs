use std::{cmp::Reverse, collections::BinaryHeap, time::Instant};

use grid::{Coordinate, CoordinateDiff};
#[allow(unused_imports)]
use proconio::*;
#[allow(unused_imports)]
use rand::prelude::*;
use rand_pcg::Pcg64Mcg;

use crate::grid::{ADJACENTS, DIRECTIONS};

#[allow(unused_macros)]
macro_rules! chmin {
    ($base:expr, $($cmps:expr),+ $(,)*) => {{
        let cmp_min = min!($($cmps),+);
        if $base > cmp_min {
            $base = cmp_min;
            true
        } else {
            false
        }
    }};
}

#[allow(unused_macros)]
macro_rules! chmax {
    ($base:expr, $($cmps:expr),+ $(,)*) => {{
        let cmp_max = max!($($cmps),+);
        if $base < cmp_max {
            $base = cmp_max;
            true
        } else {
            false
        }
    }};
}

#[allow(unused_macros)]
macro_rules! min {
    ($a:expr $(,)*) => {{
        $a
    }};
    ($a:expr, $b:expr $(,)*) => {{
        std::cmp::min($a, $b)
    }};
    ($a:expr, $($rest:expr),+ $(,)*) => {{
        std::cmp::min($a, min!($($rest),+))
    }};
}

#[allow(unused_macros)]
macro_rules! max {
    ($a:expr $(,)*) => {{
        $a
    }};
    ($a:expr, $b:expr $(,)*) => {{
        std::cmp::max($a, $b)
    }};
    ($a:expr, $($rest:expr),+ $(,)*) => {{
        std::cmp::max($a, max!($($rest),+))
    }};
}

#[allow(unused_macros)]
macro_rules! mat {
    ($e:expr; $d:expr) => { vec![$e; $d] };
    ($e:expr; $d:expr $(; $ds:expr)+) => { vec![mat![$e $(; $ds)*]; $d] };
}

#[derive(Debug, Clone)]
struct Input {
    start: Coordinate,
    goal: Coordinate,
    probability: f64,
    map: Vec<Vec<[Option<Coordinate>; 4]>>,
}

const MAP_SIZE: usize = 20;
const MAX_COMMAND_LEN: usize = 200;
const SEED: u128 = 42;

fn main() {
    let input = input();
    let result = to_output(&solve(&input));
    println!("{}", result);
}

fn input() -> Input {
    input! {
        sr: usize,
        sc: usize,
        gr: usize,
        gc: usize,
        probability: f64
    }

    let start = Coordinate::new(sr, sc);
    let goal = Coordinate::new(gr, gc);
    let mut h = vec![];
    let mut v = vec![];

    for _ in 0..MAP_SIZE {
        input! {
            hi: proconio::marker::Chars
        }

        h.push(hi);
    }

    for _ in 0..(MAP_SIZE - 1) {
        input! {
            vi: proconio::marker::Chars
        }

        v.push(vi);
    }

    let mut map = mat![[None; 4]; MAP_SIZE; MAP_SIZE];

    for row in 0..MAP_SIZE {
        for col in 0..MAP_SIZE {
            let current = Coordinate::new(row, col);

            for (i, (&adj, &dir)) in ADJACENTS.iter().zip(DIRECTIONS.iter()).enumerate() {
                let next = current + adj;

                if !next.in_map(MAP_SIZE) {
                    continue;
                }

                let wall = match dir {
                    'U' => v[row - 1][col],
                    'D' => v[row][col],
                    'L' => h[row][col - 1],
                    'R' => h[row][col],
                    _ => unreachable!(),
                } == '1';

                if !wall {
                    map[row][col][i] = Some(next);
                }
            }
        }
    }

    Input {
        start,
        goal,
        probability,
        map,
    }
}

fn solve(input: &Input) -> Vec<CoordinateDiff> {
    let solution = random_dup(input, 0.05);
    let solution = annealing(input, solution, 1.92);

    let score = calc_score(input, &solution);
    eprintln!("final score = {}", score);

    solution
}

fn dijkstra(input: &Input, seed: u128) -> Vec<CoordinateDiff> {
    let mut distances = mat![std::i32::MAX / 2; MAP_SIZE; MAP_SIZE];
    let mut from = mat![None; MAP_SIZE; MAP_SIZE];
    let mut queue = BinaryHeap::new();
    let start = input.start;
    let goal = input.goal;
    distances[start.row][start.col] = 0;
    queue.push(Reverse((0, start)));
    let mut rng = Pcg64Mcg::new(seed);

    while let Some(Reverse((dist, current))) = queue.pop() {
        if distances[current.row][current.col] < dist {
            continue;
        }

        for i in 0..4 {
            if let Some(next) = input.map[current.row][current.col][i] {
                let next_dist = distances[current.row][current.col] + rng.gen_range(10, 31);

                if chmin!(distances[next.row][next.col], next_dist) {
                    queue.push(Reverse((next_dist, next)));
                    from[next.row][next.col] = Some((current, ADJACENTS[i]));
                }
            }
        }
    }

    let mut history = vec![];
    let mut current = goal;

    while let Some((prev, dir)) = from[current.row][current.col] {
        history.push(dir);
        current = prev;
    }

    history.reverse();
    history
}

fn random_dup(input: &Input, duration: f64) -> Vec<CoordinateDiff> {
    let mut best_solution = vec![];
    let mut best_score = 0;

    let since = Instant::now();
    let mut time = 0.0;
    let duration_inv = 1.0 / duration;
    let mut all_iter = 0;
    let mut rng = rand_pcg::Pcg64Mcg::new(SEED);

    while time < 1.0 {
        all_iter += 1;
        if (all_iter & ((1 << 4) - 1)) == 0 {
            time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
        }

        let mut targets = vec![];
        let solution = dijkstra(input, all_iter);
        let target_len = (solution.len() as f64 / (1.0 - input.probability)).ceil() as usize;

        for _ in 0..(target_len - solution.len()) {
            let index = rng.gen_range(0, solution.len());
            targets.push(index);
        }

        targets.sort_unstable();

        let mut new_solution = vec![];
        let mut ptr = 0;

        for i in 0..solution.len() {
            let c = solution[i];
            new_solution.push(c);

            while ptr < targets.len() && targets[ptr] == i {
                new_solution.push(c);
                ptr += 1;
            }
        }

        let score = calc_score(input, &new_solution);

        if chmax!(best_score, score) {
            best_solution = new_solution;
        }
    }

    eprintln!("[pre] all iter: {}", all_iter);

    best_solution
}

fn annealing(
    input: &Input,
    initial_solution: Vec<CoordinateDiff>,
    duration: f64,
) -> Vec<CoordinateDiff> {
    let mut solution = initial_solution;
    let mut best_solution = solution.clone();
    let mut current_score = calc_score(input, &solution);
    let mut best_score = current_score;

    let mut all_iter = 0;
    let mut valid_iter = 0;
    let mut accepted_count = 0;
    let mut update_count = 0;
    let mut rng = rand_pcg::Pcg64Mcg::new(SEED);

    let duration_inv = 1.0 / duration;
    let since = std::time::Instant::now();
    let mut time = 0.0;

    let temp0 = 1e5;
    let temp1 = 1e2;
    let mut inv_temp = 1.0 / temp0;

    const NEIGH_COUNT: usize = 5;
    let mut selected = vec![0; NEIGH_COUNT];
    let mut accepted = vec![0; NEIGH_COUNT];

    while time < 1.0 {
        all_iter += 1;
        if (all_iter & ((1 << 4) - 1)) == 0 {
            time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
            let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
            inv_temp = 1.0 / temp;
        }

        // 変形
        let neigh_type = rng.gen_range(0, NEIGH_COUNT);
        let mut new_solution = solution.clone();

        match neigh_type {
            0 => {
                // 重複
                if new_solution.len() == MAX_COMMAND_LEN {
                    continue;
                }

                let index = rng.gen_range(0, new_solution.len());

                new_solution.insert(index + 1, new_solution[index]);
            }
            1 => {
                // 追加
                if new_solution.len() == MAX_COMMAND_LEN {
                    continue;
                }

                let index = rng.gen_range(0, new_solution.len() + 1);
                let dir = rng.gen_range(0, 4);

                new_solution.insert(index, ADJACENTS[dir]);
            }
            2 => {
                // 削除
                if new_solution.len() < MAX_COMMAND_LEN / 2 {
                    continue;
                }

                let index = rng.gen_range(0, new_solution.len());
                new_solution.remove(index);
            }
            3 => {
                // 変更
                if new_solution.len() < 2 {
                    continue;
                }

                let index = rng.gen_range(0, new_solution.len());
                let mut adj = new_solution[index];

                while adj == new_solution[index] {
                    adj = ADJACENTS[rng.gen_range(0, 4)];
                }

                new_solution[index] = adj;
            }
            4 => {
                // 挿入
                let index0 = rng.gen_range(0, new_solution.len());
                let index1 = rng.gen_range(0, new_solution.len());
                let adj = new_solution.remove(index0);
                new_solution.insert(index1, adj);
            }
            _ => unreachable!(),
        }

        // スコア計算
        let new_score = calc_score(input, &new_solution);
        let score_diff = new_score - current_score;

        selected[neigh_type] += 1;

        if score_diff >= 0 || rng.gen_bool(f64::exp(score_diff as f64 * inv_temp)) {
            // 解の更新
            current_score = new_score;
            solution = new_solution;
            accepted_count += 1;
            accepted[neigh_type] += 1;

            if chmax!(best_score, current_score) {
                best_solution = solution.clone();
                update_count += 1;
            }
        }

        valid_iter += 1;
    }

    let prob = selected
        .iter()
        .zip(accepted.iter())
        .map(|(&s, &a)| a as f64 / s as f64)
        .collect::<Vec<_>>();

    eprintln!("===== annealing =====");
    eprintln!("score      : {}", best_score);
    eprintln!("all iter   : {}", all_iter);
    eprintln!("valid iter : {}", valid_iter);
    eprintln!("accepted   : {}", accepted_count);
    eprintln!("updated    : {}", update_count);
    eprintln!("");
    eprintln!("selected   : {:?}", &selected);
    eprintln!("accepted   : {:?}", &accepted);
    eprintln!("probability: {:?}", &prob);

    best_solution
}

fn calc_score(input: &Input, solution: &[CoordinateDiff]) -> i32 {
    let mut dp = [0.0; MAP_SIZE * MAP_SIZE * (MAX_COMMAND_LEN + 1)];
    dp[to_index(&input.start)] = 1.0;

    let mut score = 0.0;

    for (turn, &adj) in solution.iter().enumerate() {
        let (left, right) = dp.split_at_mut(MAP_SIZE * MAP_SIZE * (turn + 1));
        let current_dp = &mut left[(MAP_SIZE * MAP_SIZE * turn)..];
        let next_dp = &mut right[..(MAP_SIZE * MAP_SIZE)];

        let mut dir = 0;

        for (d, &a) in ADJACENTS.iter().enumerate() {
            if adj == a {
                dir = d;
                break;
            }
        }

        unsafe {
            for row in 0..MAP_SIZE {
                let map = input.map.get_unchecked(row);

                for col in 0..MAP_SIZE {
                    let current = Coordinate::new(row, col);
                    let index = to_index(&current);
                    let current_dp = *current_dp.get_unchecked(index);

                    if let Some(next_coordinate) = map.get_unchecked(current.col).get_unchecked(dir)
                    {
                        let next_index = to_index(&next_coordinate);
                        *next_dp.get_unchecked_mut(next_index) +=
                            current_dp * (1.0 - input.probability);
                        *next_dp.get_unchecked_mut(index) += current_dp * input.probability;
                    } else {
                        *next_dp.get_unchecked_mut(index) += current_dp;
                    }
                }
            }
        }

        let goal_index = to_index(&input.goal);
        score += next_dp[goal_index] * (401.0 - (turn + 1) as f64);
        next_dp[goal_index] = 0.0;
    }

    return (score * 250000.0f64).round() as i32;

    fn to_index(coordinate: &Coordinate) -> usize {
        coordinate.row * MAP_SIZE + coordinate.col
    }
}

fn to_output(output: &[CoordinateDiff]) -> String {
    let mut string = String::new();

    for &diff in output.iter() {
        for (&adj, &dir) in ADJACENTS.iter().zip(DIRECTIONS.iter()) {
            if diff == adj {
                string.push(dir);
                break;
            }
        }
    }

    string
}

#[allow(dead_code)]
mod grid {
    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct Coordinate {
        pub row: usize,
        pub col: usize,
    }

    impl Coordinate {
        pub const fn new(row: usize, col: usize) -> Self {
            Self { row, col }
        }

        pub fn in_map(&self, size: usize) -> bool {
            self.row < size && self.col < size
        }

        pub const fn to_index(&self, size: usize) -> usize {
            self.row * size + self.col
        }
    }

    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
    pub struct CoordinateDiff {
        pub dr: usize,
        pub dc: usize,
    }

    impl CoordinateDiff {
        pub const fn new(dr: usize, dc: usize) -> Self {
            Self { dr, dc }
        }

        pub const fn invert(&self) -> Self {
            Self::new(0usize.wrapping_sub(self.dr), 0usize.wrapping_sub(self.dc))
        }
    }

    impl std::ops::Add<CoordinateDiff> for Coordinate {
        type Output = Coordinate;

        fn add(self, rhs: CoordinateDiff) -> Self::Output {
            Coordinate::new(self.row.wrapping_add(rhs.dr), self.col.wrapping_add(rhs.dc))
        }
    }

    pub const ADJACENTS: [CoordinateDiff; 4] = [
        CoordinateDiff::new(!0, 0),
        CoordinateDiff::new(0, 1),
        CoordinateDiff::new(1, 0),
        CoordinateDiff::new(0, !0),
    ];

    pub const DIRECTIONS: [char; 4] = ['U', 'R', 'D', 'L'];
}
