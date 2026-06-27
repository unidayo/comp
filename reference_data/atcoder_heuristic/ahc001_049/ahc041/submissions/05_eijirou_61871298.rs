use std::collections::VecDeque;

use itertools::Itertools;
use proconio::input;
use rand::{seq::SliceRandom, Rng, SeedableRng};
use rand_pcg::{Mcg128Xsl64, Pcg64Mcg};

const TIME_LIMIT_SEC: f64 = 1.95;
const N: usize = 1000;
const H: usize = 10;
const IS_ROOT: usize = N;
const IS_FREE: usize = N + 1;
const DIAMITER: usize = 1000;

const SCHEDULER_TYPE: SchedulerType = SchedulerType::Linear;
const T0: f64 = 1e2;
const T1: f64 = 5.0;

fn main() {
    get_time_sec();

    let input = read_input();

    let mut state = State::new(&input, 0);
    let mut ans = state.parents.clone();
    let mut best_score = state.score;
    while get_time_sec() < TIME_LIMIT_SEC {
        if state.update() && best_score.chmax(state.score) {
            ans = state.parents;
        }
    }
    State::print(&ans);

    // state.scheduler.print_log();

    std::process::exit(0);
}

#[derive(Debug, Clone)]
struct State {
    rng: Mcg128Xsl64,
    scheduler: SimulatedAnnealingScheduler,
    a: Vec<usize>,
    edges: Vec<Vec<usize>>,
    parents: [usize; N],
    heights: [usize; N],
    children: Vec<Vec<usize>>,
    roots: Vec<usize>,
    score: usize,
    pop_history: Vec<usize>,
    history: Vec<(usize, usize)>,
}

impl State {
    fn new(input: &Input, seed: u64) -> Self {
        let mut state = Self {
            rng: Pcg64Mcg::seed_from_u64(seed),
            scheduler: SimulatedAnnealingScheduler::new(SCHEDULER_TYPE, T0, T1, TIME_LIMIT_SEC),
            a: input.a.clone(),
            edges: input.edges.clone(),
            parents: [IS_FREE; N],
            heights: [0; N],
            children: vec![vec![]; N],
            roots: vec![],
            score: 0,
            pop_history: vec![],
            history: vec![],
        };
        state.make_forest((0..N).collect_vec());
        state
    }

    fn print(parents: &[usize]) {
        for v in 0..N {
            if parents[v] == IS_ROOT {
                print!("{}", -1);
            } else {
                print!("{}", parents[v]);
            }
            if v + 1 == N {
                print!("\n");
            } else {
                print!(" ");
            }
        }
    }

    fn make_forest(&mut self, que: Vec<usize>) {
        // que.shuffle(&mut self.rng);
        for &root in &que {
            if self.parents[root] != IS_FREE {
                continue;
            }
            self.roots.push(root);
            self.parents[root] = IS_ROOT;
            self.heights[root] = 0;
            self.dfs(root);
        }
    }

    // fn bfs(&mut self, u: usize) {
    //     let mut que = VecDeque::new();
    //     que.push_back(u);
    //     while let Some(u) = que.pop_front() {
    //         for &v in &self.edges[u] {
    //             if self.parents[v] != IS_FREE {
    //                 continue;
    //             }
    //             self.parents[v] = u;
    //             self.heights[v] = self.heights[u] + 1;
    //             self.children[u].push(v);
    //             self.score += self.heights[v] * self.a[v];
    //             if self.heights[v] < H {
    //                 que.push_back(v);
    //             }
    //         }
    //     }
    // }

    fn dfs(&mut self, u: usize) {
        let eul = self.edges[u].len();
        // self.edges[u].swap(0, self.rng.gen_range(1..eul));
        // self.edges[u].shuffle(&mut self.rng);
        self.children[u].clear();
        for i in 0..eul {
            let v = self.edges[u][i];
            if self.parents[v] != IS_FREE {
                continue;
            }
            self.parents[v] = u;
            self.heights[v] = self.heights[u] + 1;
            self.children[u].push(v);
            self.score += self.heights[v] * self.a[v];
            if self.heights[v] < H {
                self.dfs(v);
            }
        }
    }

    fn update(&mut self) -> bool {
        if self.rng.gen_range(0..10) == 0 {
            self.change_roots()
        } else {
            self.change_subtree()
        }
    }

    fn change_subtree(&mut self) -> bool {
        let u = self.rng.gen_range(0..N);
        let pu = self.parents[u];
        if pu == IS_ROOT {
            return false;
        }
        for i in 0..self.children[pu].len() {
            if self.children[pu][i] == u {
                self.children[pu].swap_remove(i);
                break;
            }
        }
        let old_score = self.score;
        let num_fixed_roots = self.roots.len();
        let mut que = vec![];
        self.history.clear();
        self.pop_history.clear();
        self.destroy(u, &mut que);
        self.expand(&mut que);
        self.make_forest(que);
        if self.scheduler.accept(self.score as f64 - old_score as f64) {
            true
        } else {
            self.score = old_score;
            self.restore();
            self.roots.truncate(num_fixed_roots);
            false
        }
    }

    fn change_roots(&mut self) -> bool {
        let old_score = self.score;
        let mut old_roots = self.pop_roots();
        let num_fixed_roots = self.roots.len();
        let mut que = vec![];
        self.history.clear();
        self.pop_history.clear();
        for &root in &old_roots {
            self.destroy(root, &mut que);
        }
        self.expand(&mut que);
        self.make_forest(que);
        if self.scheduler.accept(self.score as f64 - old_score as f64) {
            true
        } else {
            self.score = old_score;
            self.restore();
            self.roots.truncate(num_fixed_roots);
            self.roots.append(&mut old_roots);
            false
        }
    }

    fn expand(&mut self, que: &mut [usize]) {
        que.shuffle(&mut self.rng);
        for &u in que.iter() {
            if self.parents[u] != IS_FREE {
                continue;
            }
            let eul = self.edges[u].len();
            self.edges[u].swap(0, self.rng.gen_range(1..eul));
            for i in 0..eul {
                let v = self.edges[u][i];
                if self.parents[v] != IS_FREE && self.heights[v] < H {
                    self.parents[u] = v;
                    self.heights[u] = self.heights[v] + 1;
                    self.children[v].push(u);
                    self.pop_history.push(v);
                    self.score += self.heights[u] * self.a[u];
                    if self.heights[u] < H {
                        self.dfs(u);
                    }
                    break;
                }
            }
        }
    }

    fn pop_roots(&mut self) -> Vec<usize> {
        let i = self.rng.gen_range(0..self.roots.len());
        let ret = vec![self.roots[i]];
        self.roots.swap_remove(i);
        ret
    }

    // destroy whole tree (not support subtree)
    fn destroy(&mut self, u: usize, que: &mut Vec<usize>) {
        let mut i = que.len();
        que.push(u);
        while i < que.len() {
            let u = que[i];
            i += 1;
            self.history.push((u, self.parents[u]));
            self.score -= self.heights[u] * self.a[u];
            debug_assert_ne!(self.parents[u], IS_FREE);
            self.parents[u] = IS_FREE;
            // self.heights[u] = 0;
            while let Some(v) = self.children[u].pop() {
                debug_assert_eq!(self.parents[v], u);
                que.push(v);
            }
        }
    }

    fn restore(&mut self) {
        for &v in &self.pop_history {
            self.children[v].pop();
        }
        for &(u, _) in &self.history {
            self.children[u].clear();
        }
        for &(u, v) in &self.history {
            self.parents[u] = v;
            if v == IS_ROOT {
                self.heights[u] = 0;
            } else {
                self.children[v].push(u);
                self.heights[u] = self.heights[v] + 1;
            }
        }
    }

    fn verify(&self) {
        for u in 0..N {
            for &v in &self.children[u] {
                debug_assert_eq!(u, self.parents[v]);
            }
        }
        for u in 0..N {
            let v = self.parents[u];
            if v != IS_ROOT {
                debug_assert!(self.children[v].contains(&u));
            }
        }
        for &root in &self.roots {
            debug_assert_eq!(self.parents[root], IS_ROOT);
        }
    }
}

#[derive(Debug, Clone)]
struct Input {
    m: usize,
    a: Vec<usize>,
    uv: Vec<(usize, usize)>,
    xy: Vec<(usize, usize)>,
    edges: Vec<Vec<usize>>,
}

fn read_input() -> Input {
    input! {
        _n: usize,
        m: usize,
        _h: usize,
        a: [usize; N],
        uv: [(usize, usize); m],
        xy: [(usize, usize); N],
    }
    let mut edges = vec![vec![]; N];
    for &(u, v) in &uv {
        edges[u].push(v);
        edges[v].push(u);
    }
    Input {
        m,
        a,
        uv,
        xy,
        edges,
    }
}

pub trait ChangeMinMax {
    fn chmin(&mut self, x: Self) -> bool;
    fn chmax(&mut self, x: Self) -> bool;
}

impl<T: PartialOrd> ChangeMinMax for T {
    fn chmin(&mut self, x: T) -> bool {
        *self > x && {
            *self = x;
            true
        }
    }

    fn chmax(&mut self, x: T) -> bool {
        *self < x && {
            *self = x;
            true
        }
    }
}

const SA_TIME_COUNTS: usize = 1 << 4;
const SA_RANDOM_STEPS: usize = 1 << 12;

#[derive(Debug, Clone)]
pub enum SchedulerType {
    Exponential,
    Linear,
}

#[derive(Debug, Clone)]
pub struct SimulatedAnnealingScheduler {
    schedule_type: SchedulerType,
    t_first: f64,
    t_last: f64,
    start_time_sec: f64,
    duration_sec: f64,
    time_counter: usize,
    temperature: f64,
    random_index: usize,
    log2_random: Vec<f64>,
    trials: usize,
    acceptances: usize,
}

impl SimulatedAnnealingScheduler {
    pub fn new(
        schedule_type: SchedulerType,
        t_first: f64,
        t_last: f64,
        time_limit_sec: f64,
    ) -> SimulatedAnnealingScheduler {
        debug_assert!(0.0 <= t_last && t_last <= t_first);

        let mut log2_random = vec![0.0; SA_RANDOM_STEPS];
        for i in 0..SA_RANDOM_STEPS {
            log2_random[i] = ((i + 1) as f64 / SA_RANDOM_STEPS as f64).log2();
        }

        use rand::seq::SliceRandom;
        use rand::SeedableRng;

        let mut rng = rand_pcg::Pcg64Mcg::seed_from_u64(0);
        log2_random.shuffle(&mut rng);
        SimulatedAnnealingScheduler {
            schedule_type,
            t_first,
            t_last,
            start_time_sec: get_time_sec(),
            duration_sec: time_limit_sec - get_time_sec(),
            time_counter: 0,
            temperature: t_first,
            random_index: 0,
            log2_random,
            trials: 0,
            acceptances: 0,
        }
    }

    pub fn accept(&mut self, profit: f64) -> bool {
        self.trials += 1;
        if profit >= 0.0 || profit > self.get_threshold() {
            self.acceptances += 1;
            return true;
        } else {
            return false;
        }
    }

    pub fn get_threshold(&mut self) -> f64 {
        self.update_temperature();
        if self.random_index == SA_RANDOM_STEPS - 1 {
            self.random_index = 0;
        } else {
            self.random_index += 1;
        }
        self.temperature * self.log2_random[self.random_index]
    }

    fn update_temperature(&mut self) {
        if self.time_counter > 0 {
            self.time_counter -= 1;
            return;
        }
        self.time_counter = SA_TIME_COUNTS - 1;
        let progress = (get_time_sec() - self.start_time_sec) / self.duration_sec;
        self.temperature = match self.schedule_type {
            SchedulerType::Exponential => {
                self.t_first.powf(1.0 - progress) * self.t_last.powf(progress)
            }
            SchedulerType::Linear => self.t_first * (1.0 - progress) + self.t_last * progress,
        }
    }

    pub fn print_log(&self) {
        let acceptance_rate = self.acceptances as f64 / self.trials as f64;
        dbg!(self.trials, self.acceptances, acceptance_rate);
    }
}

// separate with " ", and end with "\n"
pub fn print_slice<T: std::fmt::Display>(v: &[T]) {
    use std::io::Write;
    let mut handle = std::io::BufWriter::new(std::io::stdout().lock());
    for i in 0..v.len() {
        write!(handle, "{}{}", v[i], if i + 1 == v.len() { "\n" } else { " " }).unwrap();
    }
    handle.flush().unwrap();
}

pub fn argsort<T: Ord>(v: &[T]) -> Vec<usize> {
    (0..v.len()).sorted_by_key(|&i| &v[i]).collect()
}

pub fn rperm(p: &[usize]) -> Vec<usize> {
    let n = p.len();
    let mut ret = vec![usize::MAX; n];
    for (i, &x) in p.iter().enumerate() {
        debug_assert!(x < n);
        debug_assert!(ret[x] == usize::MAX);
        ret[x] = i;
    }
    ret
}

pub fn get_time_sec() -> f64 {
    static mut STIME: f64 = -1.0;
    let t = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap();
    let ms = t.as_secs() as f64 + t.subsec_nanos() as f64 * 1e-9;
    unsafe {
        if STIME < 0.0 {
            STIME = ms;
        }
        ms - STIME
    }
}
