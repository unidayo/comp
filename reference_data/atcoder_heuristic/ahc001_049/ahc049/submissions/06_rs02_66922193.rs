#![allow(unstable_name_collisions)]

use std::time::Duration;

use rand::{Rng, SeedableRng};
use util_common::*;

pub mod util_common {
    pub use itertools::Itertools;
    pub use proconio::input_interactive as input;
    pub use proconio::marker::*;
    pub use proconio::source::{line::LineSource, once::OnceSource};
    pub use std::io::Write;
    use std::ops::Range;
    use std::sync::atomic::AtomicBool;
    use std::time::Duration;

    #[cfg(feature = "local_test")]
    pub use log::{debug, info};
    #[cfg(not(feature = "local_test"))]
    pub mod log {
        #[macro_export]
        macro_rules! debug {
        ($($_:tt)*) => {
            if false{
                let _=format_args!($($_)*);
            }
        };
    }
        #[macro_export]
        macro_rules! info {
        ($($_:tt)*) => {
            if false{
                let _=format_args!($($_)*);
            }
        };
    }
    }
    #[cfg(feature = "local_test")]
    pub fn init_local_test() {
        env_logger::init();
    }

    #[macro_export]
    macro_rules! d {
        () => {
            if crate::util_common::d_enabled() {
                // #[cfg(debug_assertions)]
                #[cfg(feature="local_test")]
                ::std::eprintln!(
                    "{line:>3}|{_empty: <indent$}    {escape_start}[{file}:{line}]{escape_end}",
                    file = ::std::file!(),
                    line = ::std::line!(),

                    _empty = "",
                    indent = ::std::column!() as usize / 4,

                    escape_start = "\x1b[30m",
                    escape_end = "\x1b[0m",
                );
            }
        };
        ($($x:expr),+ $(,)?) => {
            if cfg!(feature="local_test")&& crate::util_common::d_enabled() {
                // #[cfg(debug_assertions)]
                // #[cfg(feature="local_test")]
                ::std::eprintln!(
                    "{line:>3}|{_empty: <indent$}{name} = {value:?}    {escape_start}[{file}:{line}]{escape_end}",
                    name = ::std::stringify!($($x),+),
                    value = ($(&($x)),+),

                    file = ::std::file!(),
                    line = ::std::line!(),
                    _empty = "",
                    indent = ::std::column!() as usize / 4,
                    escape_start = "\x1b[30m",
                    escape_end = "\x1b[0m",
                );
            }
        };
    }
    #[doc(hidden)]
    pub static ENABLE_D: AtomicBool = AtomicBool::new(true);
    pub struct Guard<const ENABLE: bool> {}
    impl<const ENABLE: bool> Drop for Guard<ENABLE> {
        fn drop(&mut self) {
            if ENABLE {
                disable_d();
            } else {
                enable_d();
            }
        }
    }
    pub fn enable_d_scoped() -> Guard<true> {
        enable_d();
        Guard {}
    }
    pub fn disable_d_scoped() -> Guard<false> {
        disable_d();
        Guard {}
    }
    pub fn enable_d() {
        ENABLE_D.store(true, std::sync::atomic::Ordering::Relaxed)
    }
    pub fn disable_d() {
        ENABLE_D.store(false, std::sync::atomic::Ordering::Relaxed)
    }
    pub fn d_enabled() -> bool {
        ENABLE_D.load(std::sync::atomic::Ordering::Relaxed)
    }

    pub fn stdio() -> (impl std::io::BufRead, impl std::io::Write) {
        lazy_static::lazy_static! {
            static ref STDIN:std::io::Stdin=std::io::stdin();
            static ref STDOUT:std::io::Stdout=std::io::stdout();
        }

        let stdin = std::io::BufReader::new(STDIN.lock());
        let stdout = std::io::BufWriter::new(STDOUT.lock());
        (stdin, stdout)
    }
    pub static START: once_cell::sync::Lazy<std::time::Instant> =
        once_cell::sync::Lazy::new(|| std::time::Instant::now());
    pub const TL: Duration = Duration::from_millis(2900);

    pub fn bsearch(this: Range<i64>, f: impl Fn(&i64) -> bool) -> i64 {
        let (mut l, mut r) = (this.start, this.end + 1);
        while l < r - 1 {
            let m = l / 2 + r / 2 + (r & l & 1);
            if f(&(m - 1)) {
                r = m;
            } else {
                l = m;
            }
        }
        l
    }
}

pub mod lib {}
#[allow(non_upper_case_globals)]
const n: usize = 20;

const TL: Duration = Duration::from_millis(1950);

type Id = usize;
type Pos = (usize, usize);
#[derive(Debug, Clone)]
struct State {
    runs: Vec<Vec<Id>>,
}
#[derive(Debug, Clone)]
struct B {
    w: i64,
    d: i64,
    pos: (usize, usize),
    id: Id,
}
fn insertion_dscore(ids: &[Id], idx: usize, input: &[B]) -> i64 {
    let prev = if idx == 0 {
        (0, 0)
    } else {
        input[ids[idx - 1]].pos
    };
    let next = if idx == ids.len() - 1 {
        (0, 0)
    } else {
        input[ids[idx + 1]].pos
    };
    let cur = input[ids[idx]].pos;
    (dist(cur, prev) + dist(cur, next) - dist(prev, next)) as _
}
fn swap_dscore(ids: &[Id], mut i1: usize, mut i2: usize, input: &[B]) -> i64 {
    if i1 > i2 {
        (i1, i2) = (i2, i1);
    }
    if i1.abs_diff(i2) == 0 {
        return 0;
    } else if i1.abs_diff(i2) >= 2 {
        let prev1 = if i1 == 0 {
            (0, 0)
        } else {
            input[ids[i1 - 1]].pos
        };
        let next1 = if i1 == ids.len() - 1 {
            (0, 0)
        } else {
            input[ids[i1 + 1]].pos
        };
        let prev2 = if i2 == 0 {
            (0, 0)
        } else {
            input[ids[i2 - 1]].pos
        };
        let next2 = if i2 == ids.len() - 1 {
            (0, 0)
        } else {
            input[ids[i2 + 1]].pos
        };
        let cur1 = input[ids[i1]].pos;
        let cur2 = input[ids[i2]].pos;

        (dist(prev1, cur2) + dist(cur2, next1) + dist(prev2, cur1) + dist(cur1, next2)) as i64
            - (dist(prev1, cur1) + dist(cur1, next1) + dist(prev2, cur2) + dist(cur2, next2)) as i64
    } else {
        let prev = if i1 == 0 {
            (0, 0)
        } else {
            input[ids[i1 - 1]].pos
        };
        let next = if i2 == ids.len() - 1 {
            (0, 0)
        } else {
            input[ids[i2 + 1]].pos
        };
        let cur1 = input[ids[i1]].pos;
        let cur2 = input[ids[i2]].pos;
        (dist(prev, cur2) + dist(cur1, next)) as i64 - (dist(prev, cur1) + dist(cur2, next)) as i64
    }
}
// static C: AtomicUsize = AtomicUsize::new(0);
fn main() {
    let _start = *START;
    input! {
        _n:usize,
        w:[[i64;n];n],
        d:[[i64;n];n],
    }
    let mut inputs = vec![];
    for i in 0..n {
        for j in 0..n {
            if (i, j) == (0, 0) {
                continue;
            }
            inputs.push(B {
                w: w[i][j],
                d: d[i][j],
                id: i * n + j - 1,
                pos: (i, j),
            })
        }
    }
    let mut state = State {
        runs: vec![vec![]; n * n],
    };
    for b in &inputs {
        state.runs[b.id].push(b.id);
    }

    let mut rng = rand::rngs::SmallRng::seed_from_u64(4);

    let mut turn = 15599;
    let mut _iter = 0;
    let (mut best_state, mut best_turn) = (state.clone(), turn);
    let mut el = START.elapsed();
    while el < TL {
        _iter += 1;
        if _iter % 1000 == 0 {
            el = START.elapsed();
        }
        let time = el.as_secs_f64() / TL.as_secs_f64();
        let start_temp = 1.6;
        let end_temp = 0.05;
        let temp = start_temp + (end_temp - start_temp) * time * time;

        let selection = {
            let r = rng.gen_range(0..100);
            if r < 5 {
                0
            } else if r < 10 {
                1
            } else {
                2
            }
        };

        if selection == 0 {
            let i = rng.gen_range(0..state.runs.len());
            let j = rng.gen_range(0..state.runs.len());
            if i == j {
                continue;
            }
            if state.runs[j].is_empty() {
                continue;
            }

            let k = rng.gen_range(0..state.runs[j].len());
            let pop = state.runs[j][k];

            let mut best_ins = (i64::MAX, !0);
            state.runs[i].insert(0, pop);
            for ins in 0..state.runs[i].len() {
                if !is_valid(&state.runs[i], &inputs) {
                    if ins != state.runs[i].len() - 1 {
                        state.runs[i].swap(ins, ins + 1);
                    }
                    continue;
                }
                let ins_score = insertion_dscore(&state.runs[i], ins, &inputs);
                best_ins = best_ins.min((ins_score, ins));

                if ins != state.runs[i].len() - 1 {
                    state.runs[i].swap(ins, ins + 1);
                }
            }
            state.runs[i].pop();
            if best_ins.1 == !0 {
                continue;
            }
            let d = best_ins.0 - insertion_dscore(&state.runs[j], k, &inputs);
            if d >= 0 && !rng.gen_bool(f64::exp(-(d as f64 / temp))) {
                continue;
            }
            turn += d;
            state.runs[i].insert(best_ins.1, pop);
            state.runs[j].remove(k);
            if state.runs[j].is_empty() {
                state.runs.remove(j);
            }
        } else if selection == 1 {
            let i = rng.gen_range(0..state.runs.len());
            if state.runs[i].len() <= 1 {
                continue;
            }
            let j1 = rng.gen_range(0..state.runs[i].len());
            let j2 = rng.gen_range(0..state.runs[i].len());
            if j1 == j2 {
                continue;
            }
            let d = swap_dscore(&state.runs[i], j1, j2, &inputs);
            state.runs[i].swap(j1, j2);
            if !is_valid(&state.runs[i], &inputs) {
                state.runs[i].swap(j1, j2);
                continue;
            }
            if d >= 0 && !rng.gen_bool(f64::exp(-(d as f64 / temp))) {
                state.runs[i].swap(j1, j2);
                continue;
            }
            turn += d;
        } else {
            let i1 = rng.gen_range(0..state.runs.len());
            let i2 = rng.gen_range(0..state.runs.len());
            if i1 == i2 {
                continue;
            }
            if state.runs[i1].is_empty() || state.runs[i2].is_empty() {
                continue;
            }
            if state.runs[i1].len() == 1 && state.runs[i2].len() == 1 {
                continue;
            }
            let j1 = rng.gen_range(0..state.runs[i1].len());
            let j2 = rng.gen_range(0..state.runs[i2].len());

            let prev = insertion_dscore(&state.runs[i1], j1, &inputs)
                + insertion_dscore(&state.runs[i2], j2, &inputs);
            let tmp = state.runs[i1][j1];
            state.runs[i1][j1] = state.runs[i2][j2];
            state.runs[i2][j2] = tmp;
            if !is_valid(&state.runs[i1], &inputs) || !is_valid(&state.runs[i2], &inputs) {
                let tmp = state.runs[i1][j1];
                state.runs[i1][j1] = state.runs[i2][j2];
                state.runs[i2][j2] = tmp;
                continue;
            }
            let d = insertion_dscore(&state.runs[i1], j1, &inputs)
                + insertion_dscore(&state.runs[i2], j2, &inputs)
                - prev;
            if d >= 0 && !rng.gen_bool(f64::exp(-(d as f64 / temp))) {
                let tmp = state.runs[i1][j1];
                state.runs[i1][j1] = state.runs[i2][j2];
                state.runs[i2][j2] = tmp;
                continue;
            }
            turn += d;
        }
        // d!(turn);
        if turn < best_turn {
            best_state = state.clone();
            best_turn = turn;
        }
    }
    let state = best_state;
    // d!(_iter);
    d!(turn);
    for run in &state.runs {
        let mut x = 0;
        let mut y = 0;
        for id in run {
            let (nx, ny) = inputs[*id].pos;
            for _ in x..nx {
                println!("D");
            }
            for _ in nx..x {
                println!("U");
            }
            for _ in y..ny {
                println!("R");
            }
            for _ in ny..y {
                println!("L");
            }
            println!("1");
            x = nx;
            y = ny;
        }
        for _ in 0..x {
            println!("U");
        }
        for _ in 0..y {
            println!("L");
        }
    }
    // d!(C.load(std::sync::atomic::Ordering::Relaxed));
}
fn dist(x: Pos, y: Pos) -> usize {
    x.0.abs_diff(y.0) + x.1.abs_diff(y.1)
}

fn is_valid(ids: &[Id], input: &[B]) -> bool {
    let mut acc = 0;
    let mut accd = 0;

    let mut prev = (0, 0);
    for &id in ids.iter().rev() {
        if acc >= input[id].d {
            return false;
        }
        let d = dist(prev, input[id].pos);
        accd += d as i64;
        acc += accd * input[id].w as i64;
        prev = input[id].pos;

        // C.fetch_add(1, std::sync::atomic::Ordering::Relaxed);
    }

    true
}

// src/main.rs
