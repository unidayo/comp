use proconio::input;

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
    let since = std::time::Instant::now();
    let state = annealing1::greedy(&input);
    let state = annealing2::State::new(state.C, state.A);
    println!("{}", state);

    let elapsed = (std::time::Instant::now() - since).as_secs_f64();
    let duration = 1.98 - elapsed;

    let state1 = annealing2::annealing(&input, state.clone(), duration * 0.5, 5e1, 1e0);
    let state2 = annealing2::annealing(&input, state, duration * 0.5, 5e1, 1e0);

    let state = if state1.evaluate_approx(&input) > state2.evaluate_approx(&input) {
        state1
    } else {
        state2
    };

    println!("{}", state);
}

/// 入力データ
#[derive(Clone)]
pub struct Input {
    pub S: Vec<Vec<usize>>,
    pub P: Vec<i64>,
}

impl Input {
    const N: usize = 36;
    const M: usize = 12;
    const L: usize = 1000000;

    fn read_input() -> Self {
        input! {
            _N: usize,
            _M: usize,
            _L: usize,
        }

        let mut S = vec![];
        let mut P = vec![];

        for _ in 0..Self::N {
            input! {
                s: String,
                p: i64,
            }

            S.push(s);
            P.push(p);
        }

        let S = S
            .into_iter()
            .map(|s| s.chars().map(|c| c as usize - 'a' as usize).collect())
            .collect();

        Self { S, P }
    }
}

mod annealing1 {
    use std::{cmp::Reverse, collections::BTreeSet};

    use crate::{annealing2, ChangeMinMax as _, Input};
    use itertools::Itertools;
    use ordered_float::OrderedFloat;
    use rand::Rng;

    pub fn greedy(input: &Input) -> State {
        let mut env = Env::new(input.clone(), 0.6);
        let mut matrix = vec![vec![0; Input::N]; Input::N];

        for i in 0..Input::N {
            let mut set = BTreeSet::new();

            for j in 0..input.S[i].len() - 1 {
                set.insert((input.S[i][j], input.S[i][j + 1]));
            }

            for &(u, v) in set.iter() {
                matrix[u][v] += input.P[i] * input.P[i];
            }
        }

        let C = vec![0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5];
        let mut A = vec![vec![0; Input::M]; Input::M];

        for i in 0..Input::M {
            let c = C[i];
            let mut B = vec![0; Input::M];

            for j in 0..Input::M {
                let c2 = C[j];
                B[j] += matrix[c][c2];
            }

            let sum = B.iter().sum::<i64>();
            let B_std = B
                .iter()
                .map(|&x| x as f64 * 100.0 / sum as f64)
                .collect::<Vec<_>>();
            let mut added = 0;
            let mut retain = vec![];

            for j in 0..Input::M {
                let v = B_std[j] as i32;
                A[i][j] = v;
                retain.push((v as f64 - A[i][j] as f64, j));
                added += v;
            }

            retain.sort_by_key(|(x, _)| Reverse(OrderedFloat(*x)));

            for j in 0..Input::M {
                if added >= 100 {
                    break;
                }

                A[i][retain[j].1] += 1;
                added += 1;
            }
        }

        let mut state = State::new(C, A);

        let mut word_indices = (0..Input::N).collect_vec();
        word_indices.sort_by_key(|&i| Reverse(env.input.P[i]));
        const WORD_CNT: usize = 15;

        for &i in word_indices.iter().take(WORD_CNT) {
            eprintln!("target: {}", env.input.P[i]);
        }

        for (i, &word_i) in word_indices.iter().enumerate().take(WORD_CNT) {
            env.word_indices.push((word_i, env.threshold, 0.1));
            let new_state = annealing(&env, state.clone(), 0.1, 1e-1, 1e-3);
            let score = new_state.evaluate_approx(&env);

            let P_mat = state.build_probability_matrix();
            let pi = State::compute_stationary(&P_mat, 10); // 10ステップ繰り返す
            let mut ok = true;

            for &(j, threshold, mul) in env.word_indices.iter() {
                ok &= mul != 1.0 || state.evaluate_approx_single(&env, &pi, j) - threshold >= 0.0;
            }

            if !ok {
                eprintln!("NG: {}", env.input.P[word_i]);
                env.word_indices.pop();
                continue;
            }

            state = new_state;
            println!(
                "{}",
                annealing2::State::new(state.C.clone(), state.A.clone())
            );

            if score >= 0.0 {
                env.word_indices.last_mut().unwrap().2 = 1.0;
                eprintln!("OK: {}", env.input.P[word_i]);
            } else {
                let prob_ln = state.evaluate_approx_single(&env, &pi, word_i);
                env.word_indices.last_mut().unwrap().1 = prob_ln;
                eprintln!("NG: {}", env.input.P[word_i]);
            }
        }

        state
    }

    pub struct Env {
        input: Input,
        // (index, threshold, mul)
        word_indices: Vec<(usize, f64, f64)>,
        ln: Vec<f64>,
        threshold: f64,
    }

    impl Env {
        const MIN_PROB: f64 = 1e-10;

        pub fn new(input: Input, target_expected_val: f64) -> Self {
            let word_indices = vec![];
            let mut ln = vec![Self::MIN_PROB.ln()];

            for i in 1..=100 {
                ln.push((i as f64).ln());
            }

            let threshold = target_expected_val.ln() - (Input::L as f64).ln();
            eprintln!("threshold: {}", threshold);

            Self {
                input,
                word_indices,
                ln,
                threshold,
            }
        }
    }

    /// 解の型：文字割り当てと遷移確率行列
    #[derive(Clone)]
    pub struct State {
        pub C: Vec<usize>,    // 長さ M の文字割り当て ('a'～'f')
        pub A: Vec<Vec<i32>>, // M×M の遷移確率行列（各行の和＝100）
    }

    impl State {
        pub fn new(C: Vec<usize>, A: Vec<Vec<i32>>) -> Self {
            Self { C, A }
        }

        /// 遷移確率行列 P: A[i][j]/100.0 の f64 行列に変換
        pub fn build_probability_matrix(&self) -> Vec<Vec<f64>> {
            let M = self.A.len();

            let mut P = vec![vec![0.0; M]; M];
            for i in 0..M {
                for j in 0..M {
                    P[i][j] = (self.A[i][j] as f64) / 100.0;
                }
            }

            P
        }

        /// べき乗法で定常分布 π を求める
        pub fn compute_stationary(P: &Vec<Vec<f64>>, iters: usize) -> Vec<f64> {
            let M = P.len();
            let mut pi = vec![1.0 / M as f64; M];

            for _ in 0..iters {
                let mut next = vec![0.0; M];

                for (pi, p) in pi.iter().zip(P.iter()) {
                    for (next, p) in next.iter_mut().zip(p.iter()) {
                        *next += pi * p;
                    }
                }

                pi = next;
            }

            pi
        }

        pub fn evaluate_approx(&self, env: &Env) -> f64 {
            let P_mat = self.build_probability_matrix();
            let pi = Self::compute_stationary(&P_mat, 10); // 10ステップ繰り返す

            let mut total = 0.0;

            for &(word_i, threshold, mul) in env.word_indices.iter() {
                let prob_ln = self.evaluate_approx_single(env, &pi, word_i);
                total += (prob_ln - threshold).min(0.0) * mul;
            }

            total
        }

        fn evaluate_approx_single(&self, env: &Env, pi: &Vec<f64>, word_i: usize) -> f64 {
            let word_idxs = &env.input.S[word_i];
            let mut prob = [pi[word_idxs[0] * 2], pi[word_idxs[0] * 2 + 1]];
            let mut prev_c = word_idxs[0];

            for &c in &word_idxs[1..] {
                let mut next_prob = [0.0; 2];

                for i in 0..2 {
                    for j in 0..2 {
                        next_prob[j] += prob[i] * self.A[prev_c * 2 + i][c * 2 + j] as f64 / 100.0;
                    }
                }

                prob = next_prob;
                prev_c = c;
            }

            let prob = prob[0] + prob[1];
            let prob_ln = prob.max(1e-30).ln();
            prob_ln
        }

        pub fn neigh1(mut self, rng: &mut impl Rng) -> Self {
            let i = rng.gen_range(0..Input::M);
            let from = loop {
                let j = rng.gen_range(0..Input::M);

                if self.A[i][j] > 0 {
                    break j;
                }
            };

            let to = loop {
                let j = rng.gen_range(0..Input::M);

                if j != from {
                    break j;
                }
            };

            self.A[i][from] -= 1;
            self.A[i][to] += 1;
            self
        }

        pub fn neigh2(mut self, rng: &mut impl Rng) -> Self {
            let (i, j, k, l) = loop {
                let mut cands = (0..Input::M).collect_vec();
                let index = rng.gen_range(0..cands.len());
                let i = cands.swap_remove(index);
                let index = rng.gen_range(0..cands.len());
                let j = cands.swap_remove(index);

                let mut cands = (0..Input::M).collect_vec();
                let index = rng.gen_range(0..cands.len());
                let k = cands.swap_remove(index);
                let index = rng.gen_range(0..cands.len());
                let l = cands.swap_remove(index);

                if self.A[i][k] > 0 && self.A[j][l] > 0 {
                    break (i, j, k, l);
                }
            };

            self.A[i][k] -= 1;
            self.A[j][k] += 1;
            self.A[i][l] += 1;
            self.A[j][l] -= 1;
            self
        }
    }

    /// 焼きなまし本体
    pub fn annealing(
        env: &Env,
        initial_solution: State,
        duration: f64,
        temp0: f64,
        temp1: f64,
    ) -> State {
        let mut solution = initial_solution;
        let mut best_solution = solution.clone();
        let mut current_score = solution.evaluate_approx(env);
        let mut best_score = current_score;
        let init_score = current_score;

        let mut all_iter = 0;
        let mut valid_iter = 0;
        let mut accepted_count = 0;
        let mut update_count = 0;
        let mut rng = rand_pcg::Pcg64Mcg::new(42);

        let duration_inv = 1.0 / duration;
        let since = std::time::Instant::now();

        let mut inv_temp = 1.0 / temp0;

        loop {
            if current_score >= 0.0 {
                break;
            }

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
            let new_solution = if rng.gen_bool(0.5) {
                solution.clone().neigh1(&mut rng)
            } else {
                solution.clone().neigh2(&mut rng)
            };

            // スコア計算
            let new_score = new_solution.evaluate_approx(env);
            let score_diff = new_score - current_score;

            if score_diff >= 0.0 || rng.gen_bool(f64::exp(score_diff as f64 * inv_temp)) {
                // 解の更新
                current_score = new_score;
                accepted_count += 1;
                solution = new_solution;

                if best_score.change_max(current_score) {
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
}

mod annealing2 {
    use std::fmt::Display;

    use itertools::Itertools;
    use rand::Rng;

    use crate::{ChangeMinMax as _, Input};

    /// 解の型：文字割り当てと遷移確率行列
    #[derive(Clone)]
    pub struct State {
        pub C: Vec<usize>,    // 長さ M の文字割り当て ('a'～'f')
        pub A: Vec<Vec<i32>>, // M×M の遷移確率行列（各行の和＝100）
    }

    impl State {
        pub fn new(C: Vec<usize>, A: Vec<Vec<i32>>) -> Self {
            Self { C, A }
        }

        /// 遷移確率行列 P: A[i][j]/100.0 の f64 行列に変換
        pub fn build_probability_matrix(&self) -> Vec<Vec<f64>> {
            let M = self.A.len();

            let mut P = vec![vec![0.0; M]; M];
            for i in 0..M {
                for j in 0..M {
                    P[i][j] = (self.A[i][j] as f64) / 100.0;
                }
            }

            P
        }

        /// べき乗法で定常分布 π を求める
        pub fn compute_stationary(P: &Vec<Vec<f64>>, iters: usize) -> Vec<f64> {
            let M = P.len();
            let mut pi = vec![1.0 / M as f64; M];

            for _ in 0..iters {
                let mut next = vec![0.0; M];

                for (pi, p) in pi.iter().zip(P.iter()) {
                    for (next, p) in next.iter_mut().zip(p.iter()) {
                        *next += pi * p;
                    }
                }

                pi = next;
            }

            pi
        }

        pub fn evaluate_approx(&self, input: &Input) -> f64 {
            let P_mat = self.build_probability_matrix();
            let pi = Self::compute_stationary(&P_mat, 10); // 10ステップ繰り返す

            let mut total = 0.0;
            let L = Input::L as f64;
            for (word_idxs, &weight) in input.S.iter().zip(&input.P) {
                let mut prob = [pi[word_idxs[0] * 2], pi[word_idxs[0] * 2 + 1]];
                let mut prev_c = word_idxs[0];

                for &c in &word_idxs[1..] {
                    let mut next_prob = [0.0; 2];

                    for i in 0..2 {
                        for j in 0..2 {
                            next_prob[j] +=
                                prob[i] * self.A[prev_c * 2 + i][c * 2 + j] as f64 / 100.0;
                        }
                    }

                    prob = next_prob;
                    prev_c = c;
                }

                let prob = prob[0] + prob[1];
                let q = 1.0 - (1.0 - prob).powi(Input::L as i32);

                total += q * weight as f64;
            }

            total
        }

        pub fn neigh1(mut self, rng: &mut impl Rng) -> Self {
            let i = rng.gen_range(0..Input::M);
            let from = loop {
                let j = rng.gen_range(0..Input::M);

                if self.A[i][j] > 0 {
                    break j;
                }
            };

            let to = loop {
                let j = rng.gen_range(0..Input::M);

                if j != from {
                    break j;
                }
            };

            self.A[i][from] -= 1;
            self.A[i][to] += 1;
            self
        }

        pub fn neigh2(mut self, rng: &mut impl Rng) -> Self {
            let (i, j, k, l) = loop {
                let mut cands = (0..Input::M).collect_vec();
                let index = rng.gen_range(0..cands.len());
                let i = cands.swap_remove(index);
                let index = rng.gen_range(0..cands.len());
                let j = cands.swap_remove(index);

                let mut cands = (0..Input::M).collect_vec();
                let index = rng.gen_range(0..cands.len());
                let k = cands.swap_remove(index);
                let index = rng.gen_range(0..cands.len());
                let l = cands.swap_remove(index);

                if self.A[i][k] > 0 && self.A[j][l] > 0 {
                    break (i, j, k, l);
                }
            };

            self.A[i][k] -= 1;
            self.A[j][k] += 1;
            self.A[i][l] += 1;
            self.A[j][l] -= 1;
            self
        }
    }

    impl Display for State {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            for i in 0..Input::M {
                write!(f, "{}", (self.C[i] as u8 + b'a') as char)?;

                for j in 0..Input::M {
                    write!(f, " {}", self.A[i][j])?;
                }

                writeln!(f)?;
            }

            Ok(())
        }
    }

    /// 焼きなまし本体
    pub fn annealing(
        input: &Input,
        initial_solution: State,
        duration: f64,
        temp0: f64,
        temp1: f64,
    ) -> State {
        let mut solution = initial_solution;
        let mut best_solution = solution.clone();
        let mut current_score = solution.evaluate_approx(input);
        let mut best_score = current_score;
        let init_score = current_score;

        let mut all_iter = 0;
        let mut valid_iter = 0;
        let mut accepted_count = 0;
        let mut update_count = 0;
        let mut rng = rand_pcg::Pcg64Mcg::new(42);

        let duration_inv = 1.0 / duration;
        let since = std::time::Instant::now();

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
            let new_solution = if rng.gen_bool(0.5) {
                solution.clone().neigh1(&mut rng)
            } else {
                solution.clone().neigh2(&mut rng)
            };

            // スコア計算
            let new_score = new_solution.evaluate_approx(input);
            let score_diff = new_score - current_score;

            if score_diff >= 0.0 || rng.gen_bool(f64::exp(score_diff as f64 * inv_temp)) {
                // 解の更新
                current_score = new_score;
                accepted_count += 1;
                solution = new_solution;

                if best_score.change_max(current_score) {
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
}
