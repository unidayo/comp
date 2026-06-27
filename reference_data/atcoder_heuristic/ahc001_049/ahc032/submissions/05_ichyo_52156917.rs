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
            use std::{cmp::Reverse, collections::BinaryHeap};

            use itertools::Itertools;
            use proconio::input;
            use rand::prelude::*;

            pub struct ProblemInput {
                pub n: usize,
                pub m: usize,
                pub k: usize,
                pub init: Vec<Vec<u64>>,
                pub stamps: Vec<Vec<Vec<u64>>>,
            }

            pub struct ProblemOutput {
                pub result: Vec<(usize, usize, usize)>,
            }

            // Common interface for atcoder and evaluator
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
                        k: usize,
                        init: [[u64; n]; n],
                        stamps: [[[u64; 3]; 3]; m],
                    }

                    ProblemInput {
                        n,
                        m,
                        k,
                        init,
                        stamps,
                    }
                }

                fn send_output(&mut self, output: ProblemOutput) {
                    println!("{}", output.result.len());
                    for (m, p, q) in output.result {
                        println!("{} {} {}", m, p, q);
                    }
                }
            }

            const MOD: u64 = 998244353;

            struct StampSet {
                add: [u64; 9],
                idx: Vec<usize>,
            }

            fn gen(
                stamps: &Vec<Vec<Vec<u64>>>,
                depth: usize,
                idxes: &mut Vec<usize>,
                sets: &mut Vec<StampSet>,
            ) {
                let mut add = [0; 9];
                for &i in idxes.iter() {
                    for x in 0..3 {
                        for y in 0..3 {
                            add[x * 3 + y] = (add[x * 3 + y] + stamps[i][x][y]) % MOD;
                        }
                    }
                }
                sets.push(StampSet {
                    add,
                    idx: idxes.clone(),
                });
                if depth != 0 {
                    let first_idx = if idxes.len() == 0 {
                        0
                    } else {
                        idxes[idxes.len() - 1]
                    };

                    for i in first_idx..stamps.len() {
                        idxes.push(i);
                        gen(stamps, depth - 1, idxes, sets);
                        idxes.pop();
                    }
                }
            }

            #[derive(Clone)]
            struct State {
                grid: [u64; 81],
                score: i64,
                output: Vec<(usize, usize, usize)>,
            }

            pub fn solve<E: Environment>(environment: &mut E) {
                // Write your solution here

                let input = environment.receive_input();

                let stamps = input.stamps.clone();

                const n: usize = 9;
                const m: usize = 20;

                let start = std::time::Instant::now();
                let mut sets = Vec::new();
                gen(&stamps, 6, &mut Vec::new(), &mut sets);

                sets.sort_by_key(|set| set.idx.len() as i64);

                let sets = sets;
                // eprintln!("gen = {:?}", start.elapsed());

                let b = [0, 1, 2, 3, 4, 5, 6, 9];
                let segments: Vec<(usize, usize, usize, usize)> = b
                    .windows(2)
                    .map(|r_| (r_[0], r_[1]))
                    .cartesian_product(b.windows(2).map(|c_| (c_[0], c_[1])))
                    .map(|((sr, er), (sc, ec))| (sr, er, sc, ec))
                    .collect();

                let start = std::time::Instant::now();
                let mut th = 0;

                let mut init_grid = [0; 81];
                for r in 0..n {
                    for c in 0..n {
                        init_grid[r * n + c] = input.init[r][c];
                    }
                }

                let mut states = vec![State {
                    grid: init_grid,
                    score: 0,
                    output: Vec::new(),
                }];

                let beam_width = 450;
                let branches = 30;

                for (sr, er, sc, ec) in segments {
                    th += (er - sr) * (ec - sc);

                    let mut next_states = Vec::new();
                    for state in states {
                        let mut candidates: BinaryHeap<(Reverse<u64>, usize)> = BinaryHeap::new();
                        'search: for (set_idx, set) in sets.iter().enumerate() {
                            if state.output.len() + set.idx.len() > th {
                                break;
                            }
                            let mut score = 0;
                            for r in sr..er {
                                for c in sc..ec {
                                    let mut tmp =
                                        state.grid[r * n + c] + set.add[(r - sr) * 3 + c - sc];
                                    if tmp >= MOD {
                                        tmp -= MOD;
                                    }

                                    if tmp < MOD * 1 / 2 {
                                        continue 'search;
                                    }

                                    score += tmp;

                                    /*
                                    score += (state.grid[r * n + c] + set.add[r - sr][c - sc]) % MOD;
                                    */
                                }
                            }
                            if candidates.len() == branches {
                                let Reverse(_score) = (*candidates.peek().unwrap()).0;
                                if _score > score {
                                    continue;
                                }
                            }
                            candidates.push((Reverse(score), set_idx));
                            if candidates.len() > branches {
                                candidates.pop();
                            }
                        }

                        while let Some((Reverse(score), set_idx)) = candidates.pop() {
                            let mut next_grid = state.grid.clone();
                            for r in sr..sr + 3 {
                                for c in sc..sc + 3 {
                                    /*
                                    next_grid[r * n + c] =
                                        (next_grid[r * n + c] + sets[set_idx].add[r - sr][c - sc]) % MOD;
                                        */

                                    next_grid[r * n + c] +=
                                        sets[set_idx].add[(r - sr) * 3 + c - sc];
                                    if next_grid[r * n + c] >= MOD {
                                        next_grid[r * n + c] -= MOD;
                                    }
                                }
                            }
                            let mut next_output = state.output.clone();
                            for i in sets[set_idx].idx.iter() {
                                next_output.push((*i, sr, sc));
                            }
                            next_states.push(State {
                                grid: next_grid,
                                score: state.score + score as i64,
                                output: next_output,
                            });
                        }
                    }
                    next_states.sort_by_key(|state| -state.score);
                    next_states.truncate(beam_width);
                    states = next_states;
                }
                // eprintln!("compute = {:?}", start.elapsed());
                let state = states[0].clone();
                let mut grid = state.grid;
                let mut result = state.output;

                while result.len() < input.k {
                    let best_push = (0..n - 2)
                        .cartesian_product(0..n - 2)
                        .cartesian_product(
                            sets.iter()
                                .filter(|set| result.len() + set.idx.len() <= input.k),
                        )
                        .map(|((sr, sc), set)| {
                            let mut score = 0i64;
                            for dr in 0..3 {
                                for dc in 0..3 {
                                    score += ((grid[(sr + dr) * n + sc + dc]
                                        + set.add[dr * 3 + dc])
                                        % MOD) as i64;
                                    score -= grid[(sr + dr) * n + sc + dc] as i64
                                }
                            }
                            ((sr, sc, set), score)
                        })
                        .filter(|&(_, score)| score > 0)
                        .max_by_key(|&(_, score)| score);

                    if let Some(best_push) = best_push {
                        let (sr, sc, set) = best_push.0;
                        for dr in 0..3 {
                            for dc in 0..3 {
                                grid[(sr + dr) * n + sc + dc] =
                                    (grid[(sr + dr) * n + sc + dc] + set.add[dr * 3 + dc]) % MOD;
                            }
                        }
                        for i in set.idx.iter() {
                            result.push((*i, sr, sc));
                        }
                    } else {
                        break;
                    }
                }

                let output = ProblemOutput { result };
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
