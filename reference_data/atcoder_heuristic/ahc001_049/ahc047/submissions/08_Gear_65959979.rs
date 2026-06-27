#![allow(non_snake_case)] // Lazy static の変数名が大文字なので

use std::collections::{HashSet, VecDeque};
use std::env;
use std::io::{self, Read};
use std::time::Instant;

use num_traits::Pow;
use once_cell::sync::Lazy;
use rand::distributions::Uniform;
use rand::prelude::*;

// --- Meta Parameters ---
macro_rules! define_meta_param {
    ($name:ident, $type:ty, $env_var_name:expr, $default_value:expr) => {
        static $name: Lazy<$type> = Lazy::new(|| {
            let default_val = $default_value;
            if cfg!(feature = "local_env") {
                // cargo run --features local_env
                env::var($env_var_name)
                    .ok()
                    .and_then(|s| s.parse::<$type>().ok())
                    .unwrap_or_else(|| {
                        eprintln!(
                            "[PARAM] Env var {} not found or invalid, using default: {}",
                            $env_var_name, default_val
                        );
                        default_val
                    })
            } else {
                default_val
            }
        });
    };
}

// SA Parameters
define_meta_param!(T_START, f64, "T_START", 1000.0);
define_meta_param!(T_END, f64, "T_END", 1.0); // 0.1 から変更
define_meta_param!(NUM_SA_CYCLES, usize, "NUM_SA_CYCLES", 3); // 追加: SAのサイクル数

// Move Probabilities (合計100になるように)
define_meta_param!(PROB_C_CHANGE, i32, "PROB_C_CHANGE", 10); // 0-PROB_C_CHANGE
define_meta_param!(PROB_A_TWEAK, i32, "PROB_A_TWEAK", 60); // PROB_C_CHANGE - (PROB_C_CHANGE + PROB_A_TWEAK)
define_meta_param!(PROB_EMBED_PATH, i32, "PROB_EMBED_PATH", 20); // ... - (...+PROB_EMBED_PATH)
// 残りは Randomize A Row (100 - (PROB_C_CHANGE + PROB_A_TWEAK + PROB_EMBED_PATH))

// Embed Path Parameters
define_meta_param!(Q_THRESHOLD_FOR_EMBED, f64, "Q_THRESHOLD_FOR_EMBED", 0.1); // 0.1から変更

// --- Constants ---
const PI_ITERS: usize = 50;
const MAX_S_LEN: usize = 12;
const M_CONST: usize = 12;
const N_CONST: usize = 36;

#[derive(Clone)]
struct ProblemInput {
    n: usize,
    m: usize,
    l: usize,
    s: Vec<Vec<char>>,
    p: Vec<i32>,
}

#[derive(Clone)]
struct Solution {
    c: [char; M_CONST],
    a: [[i32; M_CONST]; M_CONST],
    a_f64_rows: [[f64; M_CONST]; M_CONST],
}

impl Solution {
    fn new() -> Self {
        Solution {
            c: ['a'; M_CONST],
            a: [[0; M_CONST]; M_CONST],
            a_f64_rows: [[0.0; M_CONST]; M_CONST],
        }
    }
    fn update_a_f64_all(&mut self) {
        /* ... unchanged ... */
        for i in 0..M_CONST {
            for j in 0..M_CONST {
                self.a_f64_rows[i][j] = self.a[i][j] as f64 / 100.0;
            }
        }
    }
    fn update_a_f64_row(&mut self, row_idx: usize) {
        /* ... unchanged ... */
        for j in 0..M_CONST {
            self.a_f64_rows[row_idx][j] = self.a[row_idx][j] as f64 / 100.0;
        }
    }
}

struct WeightedStringSelector {
    /* ... unchanged ... */
    cumulative_weights: Vec<f64>,
    total_weight: f64,
    num_items: usize,
}
impl WeightedStringSelector {
    /* ... unchanged ... */
    fn new_with_f64_weights(weights: &[f64]) -> Self {
        let mut cumulative_weights = Vec::with_capacity(weights.len());
        let mut current_sum: f64 = 0.0;
        for &w_val in weights {
            current_sum += w_val;
            cumulative_weights.push(current_sum);
        }
        WeightedStringSelector {
            cumulative_weights,
            total_weight: current_sum,
            num_items: weights.len(),
        }
    }
    fn new_with_p_values(p_values: &[i32]) -> Self {
        let weights_f64: Vec<f64> = p_values.iter().map(|&p| p as f64).collect();
        Self::new_with_f64_weights(&weights_f64)
    }
    fn new_with_p_times_len_weights(p_values: &[i32], s_vec: &Vec<Vec<char>>) -> Self {
        let weights_f64: Vec<f64> = p_values
            .iter()
            .zip(s_vec.iter())
            .map(|(&p_val, s_k_chars)| (p_val as f64) * (s_k_chars.len() as f64).max(1.0))
            .collect();
        Self::new_with_f64_weights(&weights_f64)
    }
    fn select_idx(&self, rng: &mut ThreadRng) -> Option<usize> {
        if self.total_weight < 1e-9 || self.num_items == 0 {
            return None;
        }
        let r_val = rng.gen_range(0.0..self.total_weight);
        match self.cumulative_weights.binary_search_by(|&cum_w| {
            if cum_w < r_val {
                std::cmp::Ordering::Less
            } else {
                std::cmp::Ordering::Greater
            }
        }) {
            Ok(idx) => Some(idx.min(self.num_items.saturating_sub(1))), // Exact match, take this or prev if at end
            Err(idx) => Some(idx.min(self.num_items.saturating_sub(1))),
        }
    }
}

fn randomize_a_row_fixed(a_row_slice: &mut [i32; M_CONST], rng: &mut ThreadRng) {
    /* ... unchanged ... */
    let mut vals: [f64; M_CONST] = [0.0; M_CONST]; // Stack array
    let mut sum_vals: f64 = 0.0;
    for i in 0..M_CONST {
        vals[i] = rng.gen_range(0.0..1.0);
        sum_vals += vals[i];
    }

    if sum_vals < 1e-9 {
        vals[rng.gen_range(0..M_CONST)] = 1.0;
        sum_vals = 1.0;
    }

    let mut current_sum_int = 0;
    let mut fractional_parts_indexed: [(usize, f64); M_CONST] = [(0, 0.0); M_CONST];

    for j in 0..M_CONST {
        let exact_val = vals[j] * 100.0 / sum_vals;
        a_row_slice[j] = exact_val.floor() as i32;
        fractional_parts_indexed[j] = (j, exact_val - a_row_slice[j] as f64);
        current_sum_int += a_row_slice[j];
    }
    let remainder = 100 - current_sum_int;

    if remainder < 0 {
        a_row_slice.fill(0);
        a_row_slice[rng.gen_range(0..M_CONST)] = 100;
        return;
    }

    fractional_parts_indexed
        .sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));

    for k_idx in 0..(remainder as usize) {
        if k_idx < M_CONST {
            a_row_slice[fractional_parts_indexed[k_idx].0] += 1;
        } else {
            a_row_slice[rng.gen_range(0..M_CONST)] += 1;
        }
    }

    let final_sum: i32 = a_row_slice.iter().sum();
    if final_sum != 100 {
        a_row_slice.fill(0);
        a_row_slice[rng.gen_range(0..M_CONST)] = 100;
    }
}

fn calculate_pi(a_f64_rows: &[[f64; M_CONST]; M_CONST]) -> [f64; M_CONST] {
    /* ... unchanged ... */
    let mut pi_buffer1 = [0.0; M_CONST];
    let mut pi_buffer2 = [0.0; M_CONST];
    pi_buffer1[0] = 1.0;

    let mut current_pi_slice = &mut pi_buffer1[..M_CONST];
    let mut next_pi_slice = &mut pi_buffer2[..M_CONST];

    for _iter in 0..PI_ITERS {
        next_pi_slice.fill(0.0);
        for prev_s in 0..M_CONST {
            let factor = current_pi_slice[prev_s];
            if factor > 1e-18 {
                let a_row = &a_f64_rows[prev_s];
                for next_s in 0..M_CONST {
                    next_pi_slice[next_s] += factor * a_row[next_s];
                }
            }
        }
        std::mem::swap(&mut current_pi_slice, &mut next_pi_slice);
    }
    let mut result_array = [0.0; M_CONST];
    result_array.copy_from_slice(current_pi_slice);
    result_array
}

#[inline(never)]
fn calculate_score_eval(
    input: &ProblemInput,
    sol: &Solution,
    states_for_char: &[Vec<usize>; 6],
    dp_buffer: &mut [[f64; M_CONST]; MAX_S_LEN + 1],
    p_sk_given_s_buffer: &mut [[f64; M_CONST]; N_CONST], // Added back for Qk calculation
    reuse_pi_opt: Option<&[f64; M_CONST]>,
) -> (f64, [f64; M_CONST], [f64; N_CONST]) {
    // Returns Q_k array

    let pi_to_use: [f64; M_CONST];
    if let Some(reused_pi_ref) = reuse_pi_opt {
        pi_to_use = *reused_pi_ref;
    } else {
        pi_to_use = calculate_pi(&sol.a_f64_rows);
    }

    for row_idx in 0..N_CONST {
        p_sk_given_s_buffer[row_idx].fill(0.0);
    }

    let mut total_expected_score = 0.0;
    let mut current_q_values = [0.0; N_CONST];

    for k_str_idx in 0..N_CONST {
        // Iterate up to N_CONST
        let sk = &input.s[k_str_idx];
        let len_sk = sk.len();
        if len_sk == 0 {
            continue;
        }

        for s_idx in 0..M_CONST {
            dp_buffer[len_sk][s_idx] = 1.0;
        }
        for char_idx_in_sk_suffix in (1..len_sk).rev() {
            let target_char = sk[char_idx_in_sk_suffix];
            let target_char_code_idx = (target_char as u8 - b'a') as usize;
            let relevant_next_states = &states_for_char[target_char_code_idx];
            if relevant_next_states.is_empty() {
                for prev_s in 0..M_CONST {
                    dp_buffer[char_idx_in_sk_suffix][prev_s] = 0.0;
                }
                continue;
            }
            for prev_s in 0..M_CONST {
                let mut current_sum_prob = 0.0;
                for &next_s in relevant_next_states {
                    current_sum_prob += sol.a_f64_rows[prev_s][next_s]
                        * dp_buffer[char_idx_in_sk_suffix + 1][next_s];
                }
                dp_buffer[char_idx_in_sk_suffix][prev_s] = current_sum_prob;
            }
        }
        for s_start_idx in 0..M_CONST {
            if sol.c[s_start_idx] == sk[0] {
                p_sk_given_s_buffer[k_str_idx][s_start_idx] = if len_sk == 1 {
                    1.0
                } else {
                    dp_buffer[1][s_start_idx]
                };
            } // else it's already 0.0 due to fill
        }

        let mut p_avg_start_sk_full = 0.0;
        for s_idx in 0..M_CONST {
            p_avg_start_sk_full += pi_to_use[s_idx] * p_sk_given_s_buffer[k_str_idx][s_idx];
        }

        let qk_full = if len_sk <= input.l && len_sk > 0 {
            let ek_full = (input.l - len_sk + 1) as f64 * p_avg_start_sk_full;
            if ek_full < 1e-9 {
                ek_full.max(0.0)
            } else {
                1.0 - (-ek_full.min(700.0)).exp()
            }
        } else {
            0.0
        };

        current_q_values[k_str_idx] = qk_full;
        total_expected_score += input.p[k_str_idx] as f64 * qk_full;
    }
    (total_expected_score, pi_to_use, current_q_values)
}

#[derive(Debug)]
enum MoveRecord {
    /* ... unchanged ... */
    ChangeC {
        state_idx: usize,
        old_char: char,
    },
    TweakA {
        row_idx: usize,
        col_j1: usize,
        col_j2: usize,
        delta: i32,
    },
    RandomizeARow {
        row_idx: usize,
        old_row_a: [i32; M_CONST],
    },
    EmbedPath {
        changes: Vec<(usize, usize, usize, i32)>,
    },
    NoChange,
}

fn update_states_for_char_cache_fixed(sol_c: &[char; M_CONST], cache: &mut [Vec<usize>; 6]) {
    /* ... unchanged ... */
    for vec_list in cache.iter_mut() {
        vec_list.clear();
    }
    for s_idx in 0..M_CONST {
        cache[(sol_c[s_idx] as u8 - b'a') as usize].push(s_idx);
    }
}

fn main() {
    let start_instant = Instant::now();
    let time_limit_seconds = env::var("TIME_LIMIT_SEC")
        .ok()
        .and_then(|s| s.parse().ok())
        .unwrap_or(1.95);

    let mut rng = thread_rng();

    let mut buffer = String::new();
    io::stdin().read_to_string(&mut buffer).unwrap();
    let mut iter = buffer.split_whitespace();
    let n_in: usize = iter.next().unwrap().parse().unwrap();
    let m_in: usize = iter.next().unwrap().parse().unwrap();
    let l_in: usize = iter.next().unwrap().parse().unwrap();
    assert_eq!(n_in, N_CONST, "Input N does not match N_CONST");
    assert_eq!(m_in, M_CONST, "Input M does not match M_CONST");

    let mut s_vec = Vec::with_capacity(N_CONST);
    let mut p_vec = Vec::with_capacity(N_CONST);
    for _ in 0..N_CONST {
        s_vec.push(
            iter.next()
                .unwrap()
                .parse::<String>()
                .unwrap()
                .chars()
                .collect::<Vec<char>>(),
        );
        p_vec.push(iter.next().unwrap().parse().unwrap());
    }
    let input = ProblemInput {
        n: N_CONST,
        m: M_CONST,
        l: l_in,
        s: s_vec,
        p: p_vec,
    };

    let string_selector_for_c_init = WeightedStringSelector::new_with_p_values(&input.p);

    let mut current_sol = Solution::new();
    if M_CONST >= 6 {
        for i in 0..6 {
            current_sol.c[i] = (b'a' + i as u8) as char;
        }
        for i in 6..M_CONST {
            if let Some(s_idx) = string_selector_for_c_init.select_idx(&mut rng) {
                if !input.s[s_idx].is_empty() {
                    current_sol.c[i] = input.s[s_idx][rng.gen_range(0..input.s[s_idx].len())];
                } else {
                    current_sol.c[i] = (b'a' + rng.gen_range(0..6)) as char;
                }
            } else {
                current_sol.c[i] = (b'a' + rng.gen_range(0..6)) as char;
            }
        }
    } else {
        for i in 0..M_CONST {
            current_sol.c[i] = (b'a' + rng.gen_range(0..6)) as char;
        }
    }
    for i in 0..M_CONST {
        randomize_a_row_fixed(&mut current_sol.a[i], &mut rng);
    }
    current_sol.update_a_f64_all();

    let mut dp_buffer: [[f64; M_CONST]; MAX_S_LEN + 1] = [[0.0; M_CONST]; MAX_S_LEN + 1];
    let mut p_sk_given_s_main_buffer: [[f64; M_CONST]; N_CONST] = [[0.0; M_CONST]; N_CONST];
    let mut states_for_char_cache: [Vec<usize>; 6] =
        core::array::from_fn(|_| Vec::with_capacity(M_CONST));
    update_states_for_char_cache_fixed(&current_sol.c, &mut states_for_char_cache);

    let (mut current_score, mut current_pi_array, mut current_q_values_array) =
        calculate_score_eval(
            &input,
            &current_sol,
            &states_for_char_cache,
            &mut dp_buffer,
            &mut p_sk_given_s_main_buffer,
            None,
        );

    let mut best_sol = current_sol.clone();
    let mut best_score = current_score;

    // MODIFIED: Log NUM_SA_CYCLES
    eprintln!(
        "[PARAM] T_START: {:.2}, T_END: {:.3}, NUM_SA_CYCLES: {}",
        *T_START, *T_END, *NUM_SA_CYCLES
    );
    eprintln!(
        "[PARAM] PROB_C_CHANGE: {}, PROB_A_TWEAK: {}, PROB_EMBED_PATH: {}",
        *PROB_C_CHANGE, *PROB_A_TWEAK, *PROB_EMBED_PATH
    );
    eprintln!(
        "[PARAM] Q_THRESHOLD_FOR_EMBED: {:.3}",
        *Q_THRESHOLD_FOR_EMBED
    );
    eprintln!("[INFO] Initial score: {:.2}", best_score);

    let mut iterations = 0_u64;
    let state_dist = Uniform::from(0..M_CONST);
    let mut last_log_time = Instant::now();
    const LOG_INTERVAL_MS: u128 = 500;

    // --- MODIFIED: Temperature calculation setup for cycles ---
    let effective_num_cycles = (*NUM_SA_CYCLES).max(1); // Ensure at least 1 cycle
    let cycle_duration = time_limit_seconds / (effective_num_cycles as f64);
    // --- End of temperature calculation setup ---

    let prob_boundary_c_change = *PROB_C_CHANGE;
    let prob_boundary_a_tweak = prob_boundary_c_change + *PROB_A_TWEAK;
    let prob_boundary_embed_path = prob_boundary_a_tweak + *PROB_EMBED_PATH;

    loop {
        let current_run_time = start_instant.elapsed().as_secs_f64();
        if current_run_time > time_limit_seconds {
            break;
        }
        iterations += 1;

        // --- MODIFIED: Temperature calculation based on cycles ---
        let progress_in_cycle: f64;
        let current_cycle_display_num: usize; // For logging, 1-indexed

        if effective_num_cycles == 1 {
            // Single cycle (or NUM_SA_CYCLES set to 0 or 1) behaves like original
            progress_in_cycle = (current_run_time / cycle_duration).min(1.0); // cycle_duration is time_limit_seconds
            current_cycle_display_num = 1;
        } else {
            // Multi-cycle
            // current_cycle_idx is 0-indexed
            let current_cycle_idx_float = current_run_time / cycle_duration;
            let current_cycle_idx = current_cycle_idx_float.floor() as usize;

            // Clamp current_cycle_idx to be within [0, effective_num_cycles - 1]
            let clamped_cycle_idx = current_cycle_idx.min(effective_num_cycles - 1);

            let time_elapsed_in_clamped_cycle =
                current_run_time - (clamped_cycle_idx as f64 * cycle_duration);
            progress_in_cycle = (time_elapsed_in_clamped_cycle / cycle_duration).min(1.0);
            current_cycle_display_num = clamped_cycle_idx + 1;
        }

        let t: f64 = (*T_START) * ((*T_END) / (*T_START)).powf(progress_in_cycle);
        // --- End of temperature calculation modification ---

        if iterations % 2000 == 0
            || iterations == 1
            || last_log_time.elapsed().as_millis() >= LOG_INTERVAL_MS
        {
            // MODIFIED: Logging with cycle information
            eprintln!(
                "[INFO] Iter: {:<8} | Time: {:<5.2}s ({:<3.0}%) | Cycle: {}/{} ({:<3.0}%) | Temp: {:<8.2} | Score: {:<10.2} | Best: {:<10.2}",
                iterations,
                current_run_time,
                (current_run_time / time_limit_seconds * 100.0).min(100.0), // Overall progress
                current_cycle_display_num,
                effective_num_cycles,
                progress_in_cycle * 100.0, // Progress within current cycle
                t,
                current_score,
                best_score
            );
            last_log_time = Instant::now();
        }

        let move_type_rand = rng.gen_range(0..100);
        let mut move_record = MoveRecord::NoChange;
        let mut pi_needs_recalc = false;
        let mut c_changed_for_cache_update = false;

        if move_type_rand < prob_boundary_c_change {
            /* ... Change C logic (unchanged) ... */
            let k_state_to_change = rng.sample(state_dist);
            let original_char_at_k = current_sol.c[k_state_to_change];
            let mut new_char = original_char_at_k;
            if let Some(s_idx) = string_selector_for_c_init.select_idx(&mut rng) {
                if !input.s[s_idx].is_empty() {
                    new_char = input.s[s_idx][rng.gen_range(0..input.s[s_idx].len())];
                }
            } else {
                new_char = (b'a' + rng.gen_range(0..6)) as char;
            }

            if original_char_at_k != new_char {
                current_sol.c[k_state_to_change] = new_char;
                let mut distinct_chars = HashSet::new();
                for char_val in current_sol.c.iter() {
                    distinct_chars.insert(*char_val);
                }
                if M_CONST >= 6 && distinct_chars.len() < 6 {
                    current_sol.c[k_state_to_change] = original_char_at_k;
                } else {
                    move_record = MoveRecord::ChangeC {
                        state_idx: k_state_to_change,
                        old_char: original_char_at_k,
                    };
                    c_changed_for_cache_update = true;
                }
            }
        } else if move_type_rand < prob_boundary_a_tweak {
            /* ... Tweak A logic (unchanged) ... */
            let k_state = rng.sample(state_dist);
            let j1 = rng.sample(state_dist);
            let mut j2 = rng.sample(state_dist);
            if j1 == j2 {
                j2 = (j1 + 1 + rng.gen_range(0..(M_CONST - 1))) % M_CONST;
            }
            let delta_range = Uniform::from(1..=20);
            let mut delta = rng.sample(delta_range);
            delta = delta.min(current_sol.a[k_state][j1]);
            delta = delta.min(100 - current_sol.a[k_state][j2]);
            if delta > 0 {
                current_sol.a[k_state][j1] -= delta;
                current_sol.a[k_state][j2] += delta;
                current_sol.update_a_f64_row(k_state);
                move_record = MoveRecord::TweakA {
                    row_idx: k_state,
                    col_j1: j1,
                    col_j2: j2,
                    delta,
                };
                pi_needs_recalc = true;
            }
        } else if move_type_rand < prob_boundary_embed_path {
            // Embed String Path (unchanged)
            let mut candidate_indices_for_embed: Vec<usize> = Vec::new();
            let mut candidate_weights_for_embed: Vec<f64> = Vec::new();
            for k in 0..N_CONST {
                if current_q_values_array[k] < *Q_THRESHOLD_FOR_EMBED {
                    candidate_indices_for_embed.push(k);
                    candidate_weights_for_embed
                        .push((input.p[k] as f64) * (input.s[k].len() as f64).max(1.0));
                }
            }

            if !candidate_indices_for_embed.is_empty() {
                let temp_selector =
                    WeightedStringSelector::new_with_f64_weights(&candidate_weights_for_embed);
                if let Some(selected_internal_idx) = temp_selector.select_idx(&mut rng) {
                    let target_s_idx = candidate_indices_for_embed[selected_internal_idx];
                    let s_k = &input.s[target_s_idx];
                    let mut path_changes_record: Vec<(usize, usize, usize, i32)> = Vec::new();
                    let mut actual_reinforcement_done = false;
                    if !s_k.is_empty() && s_k.len() <= M_CONST {
                        let mut path: Vec<usize> = Vec::with_capacity(s_k.len());
                        let q0_options: Vec<usize> = (0..M_CONST)
                            .filter(|&s_idx| current_sol.c[s_idx] == s_k[0])
                            .collect();
                        if !q0_options.is_empty() {
                            path.push(q0_options[rng.gen_range(0..q0_options.len())]);
                            let mut path_found_successfully = true;
                            for char_idx_in_sk in 1..s_k.len() {
                                let prev_state_in_path = *path.last().unwrap();
                                let target_char = s_k[char_idx_in_sk];
                                let candidate_next_states: Vec<usize> = (0..M_CONST)
                                    .filter(|&s_idx| current_sol.c[s_idx] == target_char)
                                    .collect();
                                if candidate_next_states.is_empty() {
                                    path_found_successfully = false;
                                    break;
                                }
                                let next_state_in_path = *candidate_next_states
                                    .iter()
                                    .max_by_key(|&&cand_s_idx| {
                                        current_sol.a[prev_state_in_path][cand_s_idx]
                                    })
                                    .unwrap_or_else(|| {
                                        &candidate_next_states
                                            [rng.gen_range(0..candidate_next_states.len())]
                                    });
                                path.push(next_state_in_path);
                            }
                            if path_found_successfully && path.len() == s_k.len() {
                                let delta_reinforce_total = 10;
                                let mut modified_rows_for_a_f64_update = HashSet::new();
                                for i in 0..(path.len() - 1) {
                                    let p_i = path[i];
                                    let p_i_plus_1 = path[i + 1];
                                    let mut amount_to_steal = delta_reinforce_total;
                                    amount_to_steal =
                                        amount_to_steal.min(100 - current_sol.a[p_i][p_i_plus_1]);
                                    if amount_to_steal == 0 {
                                        continue;
                                    }
                                    let mut victim_candidates: Vec<(usize, i32)> = (0..M_CONST)
                                        .filter(|&k_idx| {
                                            k_idx != p_i_plus_1 && current_sol.a[p_i][k_idx] > 0
                                        })
                                        .map(|k_idx| (k_idx, current_sol.a[p_i][k_idx]))
                                        .collect();
                                    victim_candidates.shuffle(&mut rng);
                                    for (victim_idx, victim_val) in victim_candidates {
                                        if amount_to_steal == 0 {
                                            break;
                                        }
                                        let steal_this = amount_to_steal.min(victim_val);
                                        if steal_this > 0 {
                                            current_sol.a[p_i][p_i_plus_1] += steal_this;
                                            current_sol.a[p_i][victim_idx] -= steal_this;
                                            path_changes_record
                                                .push((p_i, p_i_plus_1, victim_idx, steal_this));
                                            actual_reinforcement_done = true;
                                            modified_rows_for_a_f64_update.insert(p_i);
                                            amount_to_steal -= steal_this;
                                        }
                                    }
                                }
                                if actual_reinforcement_done {
                                    for &row_idx in &modified_rows_for_a_f64_update {
                                        current_sol.update_a_f64_row(row_idx);
                                    }
                                    move_record = MoveRecord::EmbedPath {
                                        changes: path_changes_record,
                                    };
                                    pi_needs_recalc = true;
                                }
                            }
                        }
                    }
                    if !actual_reinforcement_done {
                        move_record = MoveRecord::NoChange;
                    }
                } else {
                    move_record = MoveRecord::NoChange;
                }
            } else {
                move_record = MoveRecord::NoChange;
            }
        } else {
            /* ... Randomize A Row logic (unchanged) ... */
            let k_state = rng.sample(state_dist);
            let old_row_a_clone = current_sol.a[k_state];
            randomize_a_row_fixed(&mut current_sol.a[k_state], &mut rng);
            if current_sol.a[k_state] != old_row_a_clone {
                current_sol.update_a_f64_row(k_state);
                move_record = MoveRecord::RandomizeARow {
                    row_idx: k_state,
                    old_row_a: old_row_a_clone,
                };
                pi_needs_recalc = true;
            }
        }

        if c_changed_for_cache_update {
            update_states_for_char_cache_fixed(&current_sol.c, &mut states_for_char_cache);
        }

        if let MoveRecord::NoChange = move_record {
            if iterations > 1 {
                // Avoid skipping on first iteration if NoChange happened by chance
                continue;
            }
        }

        let (next_score, next_pi_cand_arr, next_q_vals_cand_arr) = calculate_score_eval(
            &input,
            &current_sol,
            &states_for_char_cache,
            &mut dp_buffer,
            &mut p_sk_given_s_main_buffer,
            if pi_needs_recalc {
                None
            } else {
                Some(&current_pi_array)
            },
        );

        if next_score > current_score
            || (t > 1e-9 && rng.gen_bool(((next_score - current_score) / t).exp()))
        {
            current_score = next_score;
            current_pi_array = next_pi_cand_arr;
            current_q_values_array = next_q_vals_cand_arr;
            if current_score > best_score {
                best_score = current_score;
                best_sol = current_sol.clone();
                eprintln!( /* ... logging ... */); // This log for best score updates is kept as is.
            }
        } else {
            /* ... Reject and revert logic (unchanged) ... */
            match move_record {
                MoveRecord::ChangeC {
                    state_idx,
                    old_char,
                } => {
                    current_sol.c[state_idx] = old_char;
                    if c_changed_for_cache_update {
                        update_states_for_char_cache_fixed(
                            &current_sol.c,
                            &mut states_for_char_cache,
                        );
                    }
                }
                MoveRecord::TweakA {
                    row_idx,
                    col_j1,
                    col_j2,
                    delta,
                } => {
                    current_sol.a[row_idx][col_j1] += delta;
                    current_sol.a[row_idx][col_j2] -= delta;
                    current_sol.update_a_f64_row(row_idx);
                }
                MoveRecord::RandomizeARow { row_idx, old_row_a } => {
                    current_sol.a[row_idx] = old_row_a;
                    current_sol.update_a_f64_row(row_idx);
                }
                MoveRecord::EmbedPath { changes } => {
                    let mut modified_rows = HashSet::new();
                    for &(row_idx, target_col, victim_col, moved_amount) in changes.iter() {
                        current_sol.a[row_idx][target_col] -= moved_amount;
                        current_sol.a[row_idx][victim_col] += moved_amount;
                        modified_rows.insert(row_idx);
                    }
                    for &row_idx in &modified_rows {
                        current_sol.update_a_f64_row(row_idx);
                    }
                }
                MoveRecord::NoChange => {}
            }
        }
    } // End SA Loop

    eprintln!(
        "----------------------------------------------------------------------------------------------------"
    );
    eprintln!("[INFO] SA finished.");
    eprintln!("[INFO] Total iterations: {}", iterations);
    eprintln!("[INFO] Final best score (raw): {:.10}", best_score);
    eprintln!("[INFO] Final rounded score: {}", best_score.round());

    for i in 0..M_CONST {
        print!("{}", best_sol.c[i]);
        for j in 0..M_CONST {
            print!(" {}", best_sol.a[i][j]);
        }
        println!();
    }
}
