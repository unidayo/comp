pub use __cargo_equip::prelude::*;

use common::*;

fn main() {
    let is_local = !std::env::var("ATCODER").is_ok();
    solve(&mut Atcoder { is_local });
}

// The following code was expanded by `cargo-equip`.

///  # Bundled libraries
///
///  - `common 0.1.0 (path+████████████████████████████████████████████████████████████)` published in **missing** licensed under **missing** as `crate::__cargo_equip::crates::common`
#[allow(unused)]
mod __cargo_equip {
    pub(crate) mod crates {
        pub mod common {
            use std::collections::HashMap;

            use proconio::input;
            use rand::prelude::*;

            pub struct ProblemInput {
                pub v: Vec<(i64, i64)>,
            }

            pub struct ProblemOutput {
                pub result: Vec<((i64, i64), (i64, i64))>,
            }

            // Common interface for atcoder and evaluator
            pub trait Environment {
                fn receive_input(&mut self) -> ProblemInput;
                fn send_output(&mut self, output: ProblemOutput);
            }

            pub struct Atcoder {
                pub is_local: bool,
            }

            impl Environment for Atcoder {
                fn receive_input(&mut self) -> ProblemInput {
                    input! {
                        n: usize,
                        v: [(i64, i64); n],
                    }

                    ProblemInput { v }
                }

                fn send_output(&mut self, output: ProblemOutput) {
                    println!("{}", output.result.len());
                    for (a, b) in output.result {
                        println!("{} {} {} {}", a.0, a.1, b.0, b.1);
                    }
                }
            }

            fn compute_base((x0, y0): (i64, i64), (x1, y1): (i64, i64)) -> (i64, i64) {
                let xb = x0.min(x1);
                let yb = y0.min(y1);
                (xb, yb)
            }

            fn calc_cost((x0, y0): (i64, i64), (x1, y1): (i64, i64)) -> i64 {
                let (xb, yb) = compute_base((x0, y0), (x1, y1));
                -(xb + yb)
            }

            pub fn iterate(
                v: &[(i64, i64)],
                x_map: &HashMap<i64, usize>,
                y_map: &HashMap<i64, usize>,
                ng: &[bool],
            ) -> Vec<((i64, i64), (i64, i64))> {
                let mut output = Vec::new();
                let mut groups = Vec::new();
                for &(x, y) in v {
                    groups.push((x as i64, y as i64, x_map[&x], y_map[&y]));
                }
                groups.sort_by_cached_key(|&(x, y, _, _)| -(x + y));
                while groups.len() > 1 {
                    let mut best = (0, 1);
                    let mut best_eval = i64::MIN;
                    for i in 0..groups.len() {
                        let (x0, y0, xi0, yi0) = groups[i];
                        if x0 + y0 <= best_eval {
                            break;
                        }
                        for j in i + 1..groups.len() {
                            let (x1, y1, xi1, yi1) = groups[j];
                            if x1 + y1 <= best_eval {
                                break;
                            }

                            let (xb, yb) = (x0.min(x1), y0.min(y1));
                            let eval = xb + yb;
                            if eval > best_eval {
                                let xbi = xi0.min(xi1);
                                let ybi = yi0.min(yi1);
                                if ng[xbi * 1000 + ybi] {
                                    continue;
                                }

                                best = (i, j);
                                best_eval = eval;
                            }
                        }
                    }
                    let (x0, y0, xi0, yi0) = groups[best.0];
                    let (x1, y1, xi1, yi1) = groups[best.1];
                    let (xb, yb, xbi, ybi) = (x0.min(x1), y0.min(y1), xi0.min(xi1), yi0.min(yi1));
                    let base = (xb, yb);
                    let best0 = (x0, y0);
                    let best1 = (x1, y1);
                    if base != best0 {
                        output.push((base, best0));
                    }
                    if base != best1 {
                        output.push((base, best1));
                    }
                    groups.remove(best.1);
                    groups.remove(best.0);
                    let idx = groups.partition_point(|&(x, y, _, _)| -(x + y) > best_eval);
                    groups.insert(idx, (xb, yb, xbi, ybi));
                }
                output.reverse();
                output
            }

            pub fn solve<E: Environment>(environment: &mut E) {
                let input = environment.receive_input();
                let start = std::time::Instant::now();

                let xs = {
                    let mut r = input.v.iter().map(|&(x, _)| x).collect::<Vec<_>>();
                    r.sort();
                    r
                };
                let ys = {
                    let mut r = input.v.iter().map(|&(_, y)| y).collect::<Vec<_>>();
                    r.sort();
                    r
                };

                let x_map = {
                    let mut map = std::collections::HashMap::new();
                    for (i, &x) in xs.iter().enumerate() {
                        map.insert(x, i);
                    }
                    map
                };
                let y_map = {
                    let mut map = std::collections::HashMap::new();
                    for (i, &y) in ys.iter().enumerate() {
                        map.insert(y, i);
                    }
                    map
                };
                let n = xs.len();

                let v_set = input
                    .v
                    .iter()
                    .cloned()
                    .collect::<std::collections::HashSet<_>>();

                let mut ng = [false; 1000 * 1000];
                let mut best_output = iterate(&input.v, &x_map, &y_map, &ng);
                let mut best_cost = 0;
                for &(a, b) in &best_output {
                    best_cost += (b.0 - a.0) + (b.1 - a.1);
                }

                let tl = std::time::Duration::from_millis(1900);
                let mut rng = SmallRng::seed_from_u64(9834982394);
                let mut iter = 0;
                let mut ngs = Vec::new();

                while start.elapsed() < tl {
                    iter += 1;
                    if rng.gen_bool(ngs.len() as f64 / 1000.0 / 4.0) {
                        let idx = rng.gen_range(0..ngs.len());
                        let (xi, yi) = ngs[idx];
                        ng[xi * 1000 + yi] = false;

                        let output = iterate(&input.v, &x_map, &y_map, &ng);
                        let mut cost = 0;
                        for &(a, b) in &output {
                            cost += (b.0 - a.0) + (b.1 - a.1);
                        }
                        if cost <= best_cost {
                            best_output = output;
                            best_cost = cost;
                            ngs.remove(idx);
                        } else {
                            ng[xi * 1000 + yi] = true;
                        }
                    } else {
                        let toggle = loop {
                            let (_, b) = best_output.choose(&mut rng).unwrap();
                            if v_set.contains(&b) {
                                continue;
                            }
                            break b;
                        };
                        let (xi, yi) = (x_map[&toggle.0], y_map[&toggle.1]);
                        ng[xi * 1000 + yi] = true;
                        let output = iterate(&input.v, &x_map, &y_map, &ng);
                        let mut cost = 0;
                        for &(a, b) in &output {
                            cost += (b.0 - a.0) + (b.1 - a.1);
                        }
                        if cost <= best_cost {
                            best_output = output;
                            best_cost = cost;
                            ngs.push((xi, yi));
                        } else {
                            ng[xi * 1000 + yi] = false;
                        }
                    }
                }

                environment.send_output(ProblemOutput {
                    result: best_output,
                });
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
