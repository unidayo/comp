// THIS CODE IS BUNDLED BY cargo-inject 0.1.0.
// commit hash = d2a157d2e917c61ca4061150dc7d5b1e5dd9fb45
use crate::procon_rust_lib::{
    data_structure::UnionFind, io::scanner::Scanner, rng::Rng, time::Timer,
};

const N: usize = 400;
const M: usize = 1995;

pub struct P {
    x: usize,
    y: usize,
}

pub struct State {
    ps: Vec<P>,
    edges: Vec<(usize, usize)>,
    base_weight: Vec<i32>,
}
impl State {
    pub fn new() -> Self {
        Self {
            ps: Vec::new(),
            edges: Vec::new(),
            base_weight: Vec::new(),
        }
    }

    #[inline(always)]
    pub fn gen_weight(&self, rng: &mut Rng, ei: usize) -> i32 {
        let base = self.base_weight[ei] * 2;
        let range = match ei {
            _ => self.base_weight[ei] * 1 / 1,
        };

        let v = rng.i64((2 * range + 1) as u64) as i32 - range;
        base + v
    }

    #[inline(always)]
    pub fn max_weight(&self, ei: usize) -> i32 {
        self.base_weight[ei] * 3 - self.base_weight[ei] / 3
    }

    #[inline(always)]
    pub fn min_weight(&self, ei: usize) -> i32 {
        self.base_weight[ei] + self.base_weight[ei] / 3
    }
}

#[inline(always)]
fn random_mst(
    rng: &mut Rng,
    s: &State,
    uf: &UnionFind,
    ei: usize,
    w: i32,
    es: &Vec<(usize, usize, usize)>,
) -> bool {
    let mut uf = uf.clone();
    for e in es {
        let ww = s.gen_weight(rng, e.2);
        if ww < w {
            uf.unite(e.0, e.1);
        }
    }

    let (u, v) = s.edges[ei];
    !uf.same(u, v)
}

#[inline(always)]
fn should_accept(
    rng: &mut Rng,
    s: &State,
    uf: &mut UnionFind,
    ei: usize,
    w: i32,
    num_vote: usize,
    total_voted: &mut usize,
    num_skipped: &mut usize,
) -> bool {
    let (u, v) = s.edges[ei];

    let mut uf = uf.clone();
    let mut uf2 = uf.clone();
    let mut es = Vec::new();
    for ei in ei + 1..M {
        let (u, v) = s.edges[ei];
        if uf.same(u, v) {
            continue;
        }
        if s.max_weight(ei) <= w {
            uf.unite(u, v);
            uf2.unite(u, v);
            continue;
        }
        if s.min_weight(ei) >= w {
            continue;
        }
        es.push((u, v, ei));
        uf2.unite(u, v);
    }

    if !uf2.same(u, v) {
        *num_skipped += 1;
        return true;
    }

    if uf.same(u, v) {
        *num_skipped += 1;
        return false;
    }

    for i in 0..N {
        uf.root(i);
    }

    let mut vote = 0;
    let mut iteration = 0;
    let threshold = num_vote / 2;
    for i in 0..num_vote {
        iteration += 1;
        if random_mst(rng, s, &uf, ei, w, &es) {
            vote += 1;
            let rest_vote = num_vote - 1 - i;
            if vote > threshold || vote + rest_vote <= threshold {
                break;
            }
            if vote > threshold || vote + rest_vote <= threshold {
                break;
            }
        }

        if iteration > 10 {
            if vote > iteration * 9 / 10 {
                *total_voted += num_vote;
                return true;
            }
            if vote < iteration * 1 / 10 {
                *total_voted += num_vote;
                return false;
            }
        }
    }
    *total_voted += num_vote;
    vote > threshold
}
fn main() {
    let mut sc = Scanner::from_stdin();

    let mut s = State::new();

    let timer = Timer::new();

    for _ in 0..N {
        let x = sc.usize();
        let y = sc.usize();
        s.ps.push(P { x, y });
    }

    for _ in 0..M {
        let x = sc.usize();
        let y = sc.usize();
        s.edges.push((x, y));
        let dx = s.ps[x].x as f64 - s.ps[y].x as f64;
        let dy = s.ps[x].y as f64 - s.ps[y].y as f64;
        s.base_weight
            .push((dx * dx + dy * dy).sqrt().round() as i32);
    }

    let mut rng = Rng::new();
    let mut uf = UnionFind::new(N);

    let mut num_vote = 1001;
    let mut total_voted = 0;
    let mut num_skipped = 0;

    for ei in 0..M {
        let w = sc.i32();
        if should_accept(
            &mut rng,
            &mut s,
            &mut uf,
            ei,
            w,
            num_vote,
            &mut total_voted,
            &mut num_skipped,
        ) {
            uf.unite(s.edges[ei].0, s.edges[ei].1);
            println!("1");
        } else {
            println!("0");
        }

        if true {
            let skip_prob = num_skipped as f64 / (ei + 1) as f64;
            let rest = (1.0 - skip_prob.max(0.8)) * (M - ei - 1) as f64;
            let vote_time = timer.elapsed() / total_voted as f64;
            if timer.elapsed() + vote_time * rest * num_vote as f64 > 1950.0 {
                if num_vote > 160 {
                    num_vote -= 10;
                }
            } else {
                num_vote += 10;
            }
        }
    }
}

mod procon_rust_lib {

    pub mod data_structure {

        pub use self::union_find::*;

        pub mod union_find {
            #[derive(Clone)]
            pub struct UnionFind {
                data: Vec<isize>,
            }
            impl UnionFind {
                pub fn new(size: usize) -> Self {
                    Self {
                        data: vec![-1; size],
                    }
                }
                pub fn root(&mut self, i: usize) -> usize {
                    if self.data[i] < 0 {
                        i
                    } else {
                        let r = self.root(self.data[i] as usize);
                        self.data[i] = r as isize;
                        r
                    }
                }
                pub fn same(&mut self, i: usize, j: usize) -> bool {
                    self.root(i) == self.root(j)
                }
                pub fn unite(&mut self, i: usize, j: usize) {
                    let mut ri = self.root(i);
                    let mut rj = self.root(j);

                    if ri == rj {
                        return;
                    }
                    if self.data[ri] > self.data[rj] {
                        std::mem::swap(&mut ri, &mut rj);
                    }

                    self.data[ri] += self.data[rj];
                    self.data[rj] = ri as isize;
                }
            }
        }
    }

    pub mod io {

        pub mod scanner {
            use std::fmt::Debug;
            use std::io::BufRead;

            use std::str::FromStr;

            pub struct Scanner<'a> {
                buf_read: Box<dyn BufRead + 'a>,
                read_data: String,
                tokens: Vec<String>,
            }
            impl<'a> Scanner<'a> {
                #[inline]
                fn leak<'c, T>(b: Box<T>) -> &'c mut T
                where
                    T: 'c,
                {
                    unsafe { &mut *Box::into_raw(b) }
                }
                pub fn from_stdin() -> Self {
                    let stdin = Self::leak(Box::new(std::io::stdin()));
                    Self {
                        buf_read: Box::new(stdin.lock()),
                        read_data: String::new(),
                        tokens: Vec::new(),
                    }
                }
                pub fn next(&mut self) -> String {
                    while self.tokens.is_empty() {
                        let mut s = String::new();
                        match self.buf_read.read_line(&mut s) {
                            Err(_) => {
                                panic!()
                            }
                            Ok(0) => {
                                std::process::exit(0);
                            }
                            _ => (),
                        }
                        self.read_data += &s;
                        self.tokens = s
                            .split_whitespace()
                            .filter(|s| !s.is_empty())
                            .rev()
                            .map(str::to_string)
                            .collect();
                    }
                    self.tokens.pop().unwrap()
                }
                pub fn next_g<T: FromStr>(&mut self) -> T
                where
                    T::Err: Debug,
                {
                    let s = self.next();
                    match s.parse() {
                        Ok(v) => v,
                        Err(_e) => {
                            dbg!(s);
                            panic!()
                        }
                    }
                }
                pub fn usize(&mut self) -> usize {
                    self.next_g()
                }
                pub fn i32(&mut self) -> i32 {
                    self.next_g()
                }
            }
        }
    }

    pub mod rng {
        pub use self::rng::*;

        pub mod rng {
            pub struct Rng {
                x: u64,
                logs_index: usize,
                logs: Vec<f64>,
            }
            impl Rng {
                pub fn new() -> Self {
                    let rng = Self {
                        x: 88172645463325252,
                        logs_index: 0,
                        logs: Vec::new(),
                    };
                    rng
                }
                pub fn xor64(&mut self) -> u64 {
                    self.x = self.x ^ (self.x.wrapping_shl(7));
                    self.x = self.x ^ (self.x.wrapping_shr(9));
                    self.x
                }
                pub fn i64(&mut self, n: u64) -> i64 {
                    (self.xor64() % n) as i64
                }
            }
        }
    }

    pub mod time {
        pub use self::timer::*;

        pub mod timer {
            use std::time::Instant;

            #[derive(Debug, Clone)]
            pub struct Timer {
                start: Instant,
            }
            impl Timer {
                pub fn new() -> Self {
                    Self {
                        start: Instant::now(),
                    }
                }
                pub fn elapsed(&self) -> f64 {
                    self.start.elapsed().as_nanos() as f64 * 1e-6
                }
            }
        }
    }
}
