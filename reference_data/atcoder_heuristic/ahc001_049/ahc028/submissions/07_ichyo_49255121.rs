pub use __cargo_equip::prelude::*;

use common::*;

fn main() {
    solve(&mut Atcoder {});
}

// The following code was expanded by `cargo-equip`.

///  # Bundled libraries
///
///  - `common 0.1.0 (path+████████████████████████████████████████████████████████████)` published in **missing** licensed under **missing** as `crate::__cargo_equip::crates::common`
#[allow(unused)]
mod __cargo_equip {
    pub(crate) mod crates {
        pub mod common {
            use std::{collections::HashMap, os::linux::raw, process::id};

            use proconio::{input, marker::Chars};
            use rand::prelude::*;

            const N: usize = 15;
            const M: usize = 200;
            const L: usize = 5;
            const C: usize = 26;

            pub struct ProblemInput {
                pub start: (usize, usize),
                pub keys: Vec<Vec<char>>,
                pub strs: Vec<String>,
            }

            pub struct ProblemOutput {
                pub moves: Vec<(usize, usize)>,
            }

            pub trait Environment {
                fn receive_input(&mut self) -> ProblemInput;
                fn send_output(&mut self, output: ProblemOutput);
            }

            pub struct Atcoder {}

            impl Environment for Atcoder {
                fn receive_input(&mut self) -> ProblemInput {
                    input! {
                        n: usize,
                        m: usize,
                        start: (usize, usize),
                        keys: [Chars; n],
                        strs: [String; m],
                    }
                    assert!(n == N);
                    assert!(m == M);
                    ProblemInput { start, keys, strs }
                }

                fn send_output(&mut self, output: ProblemOutput) {
                    for (i, j) in output.moves {
                        println!("{} {}", i, j);
                    }
                }
            }

            pub fn calc_best_cost(
                str: &[u8],
                places: &Vec<Vec<(usize, usize)>>,
                start: Option<(usize, usize)>,
            ) -> i64 {
                let mut dp = vec![[[i64::MAX; N]; N]; str.len()];
                for next_p in &places[str[0] as usize] {
                    if let Some(start) = start {
                        let cost = (next_p.0 as i64 - start.0 as i64).abs()
                            + (next_p.1 as i64 - start.1 as i64).abs()
                            + 1;
                        dp[0][next_p.0][next_p.1] = cost;
                    } else {
                        dp[0][next_p.0][next_p.1] = 0;
                    }
                }

                for i in 1..str.len() {
                    for p in &places[str[i - 1] as usize] {
                        for next_p in &places[str[i] as usize] {
                            let cost = (next_p.0 as i64 - p.0 as i64).abs()
                                + (next_p.1 as i64 - p.1 as i64).abs()
                                + 1;
                            if dp[i][next_p.0][next_p.1] > dp[i - 1][p.0][p.1] + cost {
                                dp[i][next_p.0][next_p.1] = dp[i - 1][p.0][p.1] + cost;
                            }
                        }
                    }
                }

                let mut best_p = (0, 0);
                for p in &places[str[str.len() - 1] as usize] {
                    if dp[str.len() - 1][p.0][p.1] < dp[str.len() - 1][best_p.0][best_p.1] {
                        best_p = *p;
                    }
                }

                let cost = dp[str.len() - 1][best_p.0][best_p.1];
                cost
            }

            pub fn calc_best_moves(
                str: &[u8],
                places: &Vec<Vec<(usize, usize)>>,
                start: Option<(usize, usize)>,
            ) -> (Vec<(usize, usize)>, i64) {
                let mut dp = vec![[[i64::MAX; N]; N]; str.len() + 1];
                let mut prev = vec![[[(0, 0); N]; N]; str.len() + 1];
                for next_p in &places[str[0] as usize] {
                    if let Some(start) = start {
                        let cost = (next_p.0 as i64 - start.0 as i64).abs()
                            + (next_p.1 as i64 - start.1 as i64).abs()
                            + 1;
                        dp[0][next_p.0][next_p.1] = cost;
                    } else {
                        dp[0][next_p.0][next_p.1] = 0;
                    }
                }

                for i in 1..str.len() {
                    for p in &places[str[i - 1] as usize] {
                        for next_p in &places[str[i] as usize] {
                            let cost = (next_p.0 as i64 - p.0 as i64).abs()
                                + (next_p.1 as i64 - p.1 as i64).abs()
                                + 1;
                            if dp[i][next_p.0][next_p.1] > dp[i - 1][p.0][p.1] + cost {
                                dp[i][next_p.0][next_p.1] = dp[i - 1][p.0][p.1] + cost;
                                prev[i][next_p.0][next_p.1] = *p;
                            }
                        }
                    }
                }

                let mut best_p = (0, 0);
                for p in &places[str[str.len() - 1] as usize] {
                    if dp[str.len() - 1][p.0][p.1] < dp[str.len() - 1][best_p.0][best_p.1] {
                        best_p = *p;
                    }
                }

                let cost = dp[str.len() - 1][best_p.0][best_p.1];

                let mut moves = Vec::new();
                let mut cur_p = best_p;
                for i in (0..str.len()).rev() {
                    moves.push(cur_p);
                    cur_p = prev[i][cur_p.0][cur_p.1];
                }
                moves.reverse();
                (moves, cost)
            }

            pub fn solve<E: Environment>(environment: &mut E) {
                let start = std::time::Instant::now();

                let time_limit = std::time::Duration::from_millis(1900);

                let mut rng = thread_rng();
                let input = environment.receive_input();

                let keys = {
                    let mut keys = [[0u8; N]; N];
                    for i in 0..N {
                        for j in 0..N {
                            keys[i][j] = input.keys[i][j] as u8 - b'A';
                        }
                    }
                    keys
                };

                let orig_strs = {
                    let mut strs = Vec::new();
                    for i in 0..M {
                        let mut s = Vec::new();
                        for j in 0..L {
                            s.push(input.strs[i].as_bytes()[j] - b'A');
                        }
                        strs.push(s);
                    }
                    strs
                };

                let mut places = vec![vec![]; C];
                for i in 0..N {
                    for j in 0..N {
                        places[keys[i][j] as usize].push((i, j));
                    }
                }

                let mut id_gen = 0;
                let mut strs = Vec::new();
                for s in orig_strs {
                    strs.push((s, id_gen));
                    id_gen += 1;
                }
                let mut memo = HashMap::new();
                while strs.len() > 1 {
                    let overlaps = {
                        let mut overlaps = vec![vec![0; strs.len()]; strs.len()];
                        for i in 0..strs.len() {
                            for j in 0..strs.len() {
                                if i == j {
                                    continue;
                                }
                                let max_l = 4;
                                let mut overlap = 0;
                                for l in (1..=max_l).rev() {
                                    if strs[i].0[strs[i].0.len() - l..] == strs[j].0[..l] {
                                        overlap = l;
                                        break;
                                    }
                                }
                                overlaps[i][j] = overlap;
                            }
                        }
                        overlaps
                    };

                    let mut best_ij = (0, 1);
                    let mut best_reduction = (i64::MIN, usize::MIN, usize::MIN);

                    let mut raw_scores = vec![0; strs.len()];
                    for i in 0..strs.len() {
                        let score = calc_best_cost(&strs[i].0, &places, None);
                        raw_scores[i] = score;
                    }

                    for i in 0..strs.len() {
                        for j in 0..strs.len() {
                            if i == j {
                                continue;
                            }
                            let reduction = {
                                let key = (strs[i].1, strs[j].1);
                                if !memo.contains_key(&key) {
                                    let new_str = {
                                        let mut new_str = strs[i].0.clone();
                                        new_str.extend_from_slice(&strs[j].0[overlaps[i][j]..]);
                                        new_str
                                    };
                                    let joint_score = calc_best_cost(&new_str, &places, None);
                                    let reduction = (
                                        raw_scores[i] + raw_scores[j] - joint_score,
                                        strs[i].0.len() + strs[j].0.len(),
                                        overlaps[i][j],
                                    );
                                    memo.insert(key, reduction);
                                }
                                memo[&key]
                            };

                            if best_reduction < reduction {
                                best_reduction = reduction;
                                best_ij = (i, j);
                            }
                        }
                    }

                    let new_str = {
                        let mut new_str = strs[best_ij.0].0.clone();
                        new_str.extend_from_slice(
                            &strs[best_ij.1].0[overlaps[best_ij.0][best_ij.1]..],
                        );
                        new_str
                    };
                    strs.push((new_str, id_gen));
                    id_gen += 1;
                    strs.swap_remove(best_ij.0);
                    strs.swap_remove(best_ij.1);
                }

                assert!(strs.len() == 1);

                let (moves, _) = calc_best_moves(&strs[0].0, &places, Some(input.start));

                let output = ProblemOutput { moves };
                environment.send_output(output);
            }
        }
    }

    pub(crate) mod macros {
        pub mod common {}
    }

    pub(crate) mod prelude {
        pub use crate::__cargo_equip::crates::*;
    }

    mod preludes {
        pub mod common {}
    }
}
