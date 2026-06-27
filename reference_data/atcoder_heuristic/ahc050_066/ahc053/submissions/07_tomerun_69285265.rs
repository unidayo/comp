#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(non_snake_case)]
use std::collections::HashSet;
use std::env;
use std::io::{self, BufRead, BufReader, Read, Stdin};
use std::iter;
use std::mem;
use std::str::FromStr;
use std::string::String;
use std::time;

const TIMELIMIT: u64 = 1950;
const INF: usize = 1 << 28;
const N: usize = 500;
const M: usize = 50;
const L: i64 = 1_000_000_000_000_000 - 2_000_000_000_000;
const U: i64 = 1_000_000_000_000_000 + 2_000_000_000_000;

#[derive(Debug)]
pub struct Scanner<R> {
    reader: BufReader<R>,
}

fn scanner() -> Scanner<Stdin> {
    Scanner::new(io::stdin())
}

impl<R: io::Read> Scanner<R> {
    pub fn new(read: R) -> Scanner<R> {
        Scanner {
            reader: BufReader::new(read),
        }
    }

    pub fn next_str(&mut self) -> Option<String> {
        let mut buf = [0; 1];
        let size = self.reader.read(&mut buf).unwrap();
        if size == 0 {
            None
        } else {
            self.skip_whitespace(&mut buf)?;
            let mut v = vec![buf[0]];
            loop {
                let size = self.reader.read(&mut buf).unwrap();
                if size == 0 || buf[0].is_ascii_whitespace() {
                    break;
                }
                v.push(buf[0]);
            }
            Some(String::from_utf8(v).unwrap())
        }
    }

    pub fn next_line(&mut self) -> String {
        let mut line = String::new();
        self.reader.read_line(&mut line).unwrap();
        if let Some(s) = line.strip_suffix("\n") {
            s.to_string()
        } else {
            line
        }
    }

    pub fn vec<S: FromStr>(&mut self, size: i32) -> Vec<S> {
        let mut v: Vec<S> = vec![];
        for _ in 0..size {
            let token = self.next_str().unwrap();
            v.push(S::from_str(&token).ok().unwrap());
        }
        v
    }

    pub fn next_as<S: FromStr>(&mut self) -> Option<S> {
        let s = self.next_str()?;
        S::from_str(&s).ok()
    }

    pub fn str(&mut self) -> String {
        self.next_str().unwrap()
    }

    pub fn i32(&mut self) -> i32 {
        self.next_as::<i32>().unwrap()
    }

    pub fn u32(&mut self) -> u32 {
        self.next_as::<u32>().unwrap()
    }

    pub fn i64(&mut self) -> i64 {
        self.next_as::<i64>().unwrap()
    }

    pub fn u64(&mut self) -> u64 {
        self.next_as::<u64>().unwrap()
    }

    pub fn usize(&mut self) -> usize {
        self.next_as::<usize>().unwrap()
    }

    fn skip_whitespace(&mut self, mut buf: &mut [u8]) -> Option<u8> {
        loop {
            if !buf[0].is_ascii_whitespace() {
                return Some(buf[0]);
            }
            let size = self.reader.read(&mut buf).unwrap();
            if size == 0 {
                return None;
            }
        }
    }
}

struct XorShift32 {
    state: u32,
}

impl XorShift32 {
    fn from_seed(seed: u32) -> Self {
        let initial_state = if seed == 0 { 1 } else { seed };
        XorShift32 { state: initial_state }
    }

    fn next_u32(&mut self) -> u32 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.state = x;
        x
    }

    fn next_u64(&mut self) -> u64 {
        ((self.next_u32() as u64) << 32) | (self.next_u32() as u64)
    }

    fn next_f64(&mut self) -> f64 {
        self.next_u32() as f64 / (u32::MAX as f64 + 1.0)
    }
}

#[derive(Debug)]
struct Timer {
    start_time: time::Instant,
    limit: time::Instant,
    before_time: Option<time::Instant>,
    worst_time: time::Duration,
}

impl Timer {
    fn new(tl: u64) -> Timer {
        let start_time = time::Instant::now();
        Timer {
            start_time,
            limit: start_time + time::Duration::from_millis(tl),
            before_time: None,
            worst_time: time::Duration::from_millis(0),
        }
    }

    fn elapsed(&self) -> u64 {
        self.start_time.elapsed().as_millis() as u64
    }

    fn checkpoint(&mut self) {
        if let Some(before_time) = self.before_time {
            let elapsed = before_time.elapsed();
            self.worst_time = self.worst_time.max(elapsed);
        }
        self.before_time = Some(time::Instant::now());
    }

    fn tl(&self) -> u64 {
        (self.limit - self.start_time).as_millis() as u64
    }

    fn tl_exceeded(&self) -> bool {
        self.limit < time::Instant::now()
    }

    fn should_stop(&self) -> bool {
        self.limit < time::Instant::now() + self.worst_time
    }

    fn should_stop_with_margin(&self, margin: u64) -> bool {
        self.limit < time::Instant::now() + self.worst_time + time::Duration::from_millis(margin)
    }
}

#[derive(Debug, Clone)]
struct Result {
    X: [usize; N],
    score: i64,
}

impl Result {
    fn new(X: [usize; N], score: i64) -> Result {
        Result { X, score }
    }

    fn new_empty() -> Result {
        Result { X: [0; N], score: 1 << 60 }
    }

    fn output(&self) {
        println!(
            "{}",
            self.X
                .iter()
                .map(|v| (if *v == M { 0 } else { v + 1 }).to_string())
                .collect::<Vec<String>>()
                .join(" ")
        );
    }
}

struct State {
    visited: [usize; N],
    pos: [usize; M],
    unvisit_cnt: usize,
    hash: u64,
}

struct Solver {
    A: [i64; N],
    B: [i64; M],
    timer: Timer,
    rng: XorShift32,
    best_score: i64,
}

impl Solver {
    fn new() -> Solver {
        let tl = env::var("TL").map_or(TIMELIMIT, |v| v.parse().unwrap());
        let timer = Timer::new(tl);
        let rng = XorShift32::from_seed(2);
        let mut sc = scanner();
        sc.next_line();
        let mut A = [0; N];
        for i in 0..M {
            A[i] = L + 40_000_000_000 * (if i <= 7 { 0 } else { i - 7 }) as i64;
        }
        A[M] = (U - L) * 55 / 100;
        for i in M + 1..N {
            A[i] = (A[i - 1] as f64 * if i - M < 65 { 0.979 } else { 0.947 }).floor().max(1.0) as i64;
        }
        println!("{}", A.iter().map(|v| v.to_string()).collect::<Vec<String>>().join(" "));
        let mut B = [0; M];
        for i in 0..M {
            B[i] = sc.i64();
        }
        Solver {
            A,
            B,
            timer,
            rng,
            best_score: 1 << 60,
        }
    }

    fn solve(&mut self, _tl: u64) -> Result {
        let mut best_res = self.solve_one().unwrap();
        eprintln!("score:{:?} turn:{:?}", best_res.score, 0);
        let mut turn = 0;
        while !self.timer.should_stop_with_margin(100) {
            self.timer.checkpoint();
            let res = self.solve_one();
            if let Some(res) = res {
                if res.score < best_res.score {
                    eprintln!("score:{:?} turn:{:?}", res.score, turn);
                    self.best_score = res.score;
                    best_res = res;
                }
            }
            turn += 1;
        }
        eprintln!("solve_turn:{}", turn);
        let improve_res = self.improve(&best_res.X);
        eprintln!("improve_score:{}", improve_res.score);
        if improve_res.score < best_res.score {
            best_res = improve_res;
        }
        best_res
    }

    fn solve_one(&mut self) -> Option<Result> {
        let mut left = self.B.clone();
        let mut X = [M; N];
        for i in 0..M {
            X[i] = i;
            left[i] -= self.A[i];
        }
        let mut cand = vec![];
        for i in M..N {
            for j in 0..M {
                if (left[j] - self.A[i]).abs() < 150 {
                    cand.clear();
                    cand.push(j);
                    break;
                }
                if left[j] >= self.A[i] {
                    cand.push(j);
                }
            }
            if cand.len() > 0 {
                let x = cand[self.rng.next_u32() as usize % cand.len()];
                X[i] = x;
                left[x] -= self.A[i];
                cand.clear();
            }
        }
        let score: i64 = left.iter().map(|v| v.abs()).sum();
        Some(Result::new(X, score))
    }

    fn improve(&mut self, X: &[usize; N]) -> Result {
        let mut new_X = X.clone();
        let mut sum = [0; M + 1];
        for i in 0..N {
            sum[X[i]] += self.A[i];
        }
        let mut score = 0;
        for i in 0..M {
            score += (self.B[i] - sum[i]).abs();
        }
        let mut best_score = score;
        eprintln!("score:{:?} turn:{:?}", best_score, 0);
        let mut turn = 0;
        let initial_cooler = env::var("IC").map(|v| v.parse().unwrap()).unwrap_or(1e-2f64);
        let final_cooler = env::var("FC").map(|v| v.parse().unwrap()).unwrap_or(1e-0f64);
        let mut cooler = initial_cooler;
        let start_time = self.timer.elapsed();
        loop {
            turn += 1;
            if (turn & 0xFF) == 0 {
                let elapsed = self.timer.elapsed();
                if elapsed > self.timer.tl() {
                    eprintln!("improve_turn:{:?}", turn);
                    break;
                }
                let ratio = (elapsed - start_time) as f64 / (self.timer.tl() - start_time) as f64;
                cooler = (initial_cooler.ln() * (1.0 - ratio) + final_cooler.ln() * ratio).exp();
            }
            let pos0 = self.rng.next_u32() as usize % N;
            if (self.rng.next_u32() & 1) == 1 {
                let g0 = new_X[pos0];
                let mut g1 = self.rng.next_u32() as usize % M;
                if g1 >= g0 {
                    g1 += 1;
                }
                let mut diff = 0;
                let new_sum_g0 = sum[g0] - self.A[pos0];
                let new_sum_g1 = sum[g1] + self.A[pos0];
                if g0 != M {
                    diff -= (self.B[g0] - sum[g0]).abs();
                    diff += (self.B[g0] - new_sum_g0).abs();
                }
                if g1 != M {
                    diff -= (self.B[g1] - sum[g1]).abs();
                    diff += (self.B[g1] - new_sum_g1).abs();
                }
                if self.accept(diff, cooler) {
                    score += diff;
                    if score < best_score {
                        best_score = score;
                        eprintln!("score:{:?} turn:{:?}", best_score, turn);
                    }
                    sum[g0] = new_sum_g0;
                    sum[g1] = new_sum_g1;
                    new_X[pos0] = g1;
                }
            } else {
                let mut pos1 = self.rng.next_u32() as usize % (N - 1);
                if pos1 >= pos0 {
                    pos1 += 1;
                }
                if new_X[pos0] == new_X[pos1] {
                    continue;
                }
                let g0 = new_X[pos0];
                let g1 = new_X[pos1];
                let mut diff = 0;
                let new_sum_g0 = sum[g0] - self.A[pos0] + self.A[pos1];
                let new_sum_g1 = sum[g1] - self.A[pos1] + self.A[pos0];
                if g0 != M {
                    diff -= (self.B[g0] - sum[g0]).abs();
                    diff += (self.B[g0] - new_sum_g0).abs();
                }
                if g1 != M {
                    diff -= (self.B[g1] - sum[g1]).abs();
                    diff += (self.B[g1] - new_sum_g1).abs();
                }
                if self.accept(diff, cooler) {
                    score += diff;
                    if score < best_score {
                        best_score = score;
                        eprintln!("score:{:?} turn:{:?}", best_score, turn);
                    }
                    sum[g0] = new_sum_g0;
                    sum[g1] = new_sum_g1;
                    new_X[pos0] = g1;
                    new_X[pos1] = g0;
                }
            }
        }

        Result::new(new_X, score)
    }

    fn accept(&mut self, diff: i64, cooler: f64) -> bool {
        if diff <= 0 {
            return true;
        }
        let v = -diff as f64 * cooler;
        if v < -9.0 {
            return false;
        }
        return self.rng.next_f64() < v.exp();
    }
}

fn main() {
    let mut solver = Solver::new();
    let mut best_res = Result::new_empty();
    let part = env::var("PART").map_or(1, |v| v.parse().unwrap());
    for i in 0..part {
        let result = solver.solve(solver.timer.tl() * (i + 1) / part);
        if result.score < best_res.score {
            best_res = result;
        }
        break;
    }
    best_res.output();
    let score = ((20.0 - ((best_res.score + 1) as f64).log10()) * 5e7).round() as i64;
    eprintln!("final_score:{:?} raw_score:{:e}", score, best_res.score as f64);
}
