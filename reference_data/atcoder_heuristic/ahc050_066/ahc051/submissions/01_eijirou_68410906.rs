#[cfg(target_arch = "x86_64")]
use std::arch::x86_64::*;

use std::process::exit;

use itertools::Itertools;
use proconio::input;

#[cfg(feature = "local")]
const TIME_LIMIT_SEC: f64 = 3.47;

#[cfg(not(feature = "local"))]
const TIME_LIMIT_SEC: f64 = 1.97;

const GRID_SIZE: i32 = 10000;
const INLET: Point = Point {
    x: 0,
    y: GRID_SIZE / 2,
};

type Real = f32;
const REAL_BYTES: usize = std::mem::size_of::<Real>();
const LANES_REAL: usize = 32 / REAL_BYTES;

const TSP_TIME_SEC: f64 = 0.02 * TIME_LIMIT_SEC;
const TSP_SCHED_T: SchedulerType = SchedulerType::Exp;
const TSP_T0: f64 = 100.0;
const TSP_T1: f64 = 1.0;

const EPS_PROB: Real = Real::EPSILON;
#[allow(unused)]
const LOG_EPS: Real = -23.0; // log2_fast(EPS_PROB);

const LOG_TIME_SEC: f64 = 0.1 * TIME_LIMIT_SEC;
const OUT_END_TIME: f64 = 0.9 * TIME_LIMIT_SEC;

const SCHED_T: SchedulerType = SchedulerType::Linear;
// const LOG_T0: f64 = 1e-4;
const LOG_T1: f64 = 0.0;
// const RAW_T0: f64 = 1e-3;
const RAW_T1: f64 = 0.0;

const SA_SORTER_R: usize = 1;
const SA_SORTER_CHILD_R: usize = 16 + SA_SORTER_R;

const SORTER_PATIENCE: usize = 8;

fn get_log_t0(n: usize, m: usize) -> f64 {
    let r = (m as f64 / n as f64 - 10.0) / 40.0;
    let t = get_raw_t0(n, m);
    let tl = 0.075 * t;
    let tr = 0.15 * t;
    (1.0 - r) * tl + r * tr
}

fn get_raw_t0(n: usize, m: usize) -> f64 {
    let r = (m as f64 / n as f64 - 10.0) / 40.0;
    let tl = 2.5 * 1e-3f64;
    let tr = 0.25 * 1e-3f64;
    tl.powf(1.0 - r) * tr.powf(r)
}

fn main() {
    get_time_sec();

    let input = read_input();

    let graph_builder = GraphBuilder::new(&input);
    let edges = graph_builder.build();

    let mut state = State::new(&input, &edges);

    #[allow(unused_unsafe)]
    unsafe {
        solve(&mut state);
    }
    exit(0);
}

#[cfg(not(target_arch = "x86_64"))]
fn solve(state: &mut State) {
    let end_time = get_time_sec() + LOG_TIME_SEC;
    let log_t0 = get_log_t0(state.n, state.m);
    let mut scheduler = AnnealingScheduler::new(SCHED_T, log_t0, LOG_T1, end_time);
    while get_time_sec() < end_time {
        state.update(&mut scheduler, true);
    }
    // scheduler.print_log();
    let raw_t0 = get_raw_t0(state.n, state.m);
    let mut scheduler = AnnealingScheduler::new(SCHED_T, raw_t0, RAW_T1, TIME_LIMIT_SEC);
    while get_time_sec() < OUT_END_TIME {
        state.update(&mut scheduler, false);
    }
    let mut output = state.make_output();
    while get_time_sec() < TIME_LIMIT_SEC {
        state.update(&mut scheduler, false);
        if state.score > output.score {
            output = state.make_output();
        }
    }
    // scheduler.print_log();

    output.print();
}

#[cfg(target_arch = "x86_64")]
#[target_feature(enable = "avx2")]
unsafe fn solve(state: &mut State) {
    let end_time = get_time_sec() + LOG_TIME_SEC;
    let log_t0 = get_log_t0(state.n, state.m);
    let mut scheduler = AnnealingScheduler::new(SCHED_T, log_t0, LOG_T1, end_time);
    while get_time_sec() < end_time {
        state.update(&mut scheduler, true);
    }
    // scheduler.print_log();
    let raw_t0 = get_raw_t0(state.n, state.m);
    let mut scheduler = AnnealingScheduler::new(SCHED_T, raw_t0, RAW_T1, TIME_LIMIT_SEC);
    while get_time_sec() < OUT_END_TIME {
        state.update(&mut scheduler, false);
    }
    let mut output = state.make_output();
    while get_time_sec() < TIME_LIMIT_SEC {
        state.update(&mut scheduler, false);
        if state.score > output.score {
            output = state.make_output();
        }
    }
    // scheduler.print_log();

    output.print();
}

#[derive(Debug, Clone)]
struct State {
    rng: XorShift32,
    n: usize,
    m: usize,
    k: usize,
    n_ceil: usize, // 8 * ((n + 7) / 8)
    points: Vec<Point>,
    prob: Vec<Real>,
    nodes: Vec<Node>,
    fwd: Vec<Real>, // forward probabilities
    bwd: Vec<Real>, // backward probabilities
    order: Vec<usize>,
    res: Vec<Real>,
    nres: Vec<Real>,
    nnres: Vec<Real>,
    nnnres: Vec<Real>,
    score: Real,
    visited: Vec<bool>,
    locks: Vec<i32>,
    stack: Vec<usize>,
    candidates: Vec<usize>,
}

impl State {
    fn new(input: &Input, edges: &[(usize, usize)]) -> Self {
        let mut nodes = vec![Node::new(); input.n + input.m + 1];
        for &(u, v) in edges {
            if nodes[u].child0 == v {
                continue;
            }
            if nodes[u].child0 == !0 {
                nodes[u].child0 = v;
            } else {
                debug_assert_eq!(nodes[u].child1, !0);
                nodes[u].child1 = v;
            }
            nodes[v].parents.push(u);
        }
        // processors
        for v in 0..input.n {
            debug_assert_eq!(nodes[v].child0, !0);
            debug_assert_eq!(nodes[v].child1, !0);
            nodes[v].sorter = v;
        }
        // sorters
        for v in input.n..input.n + input.m {
            debug_assert_ne!(nodes[v].child0, !0);
            if nodes[v].child1 == !0 {
                nodes[v].child1 = nodes[v].child0;
            }
            nodes[v].sorter = input.k;
        }
        // inlet
        debug_assert_ne!(nodes[input.n + input.m].child0, !0);
        debug_assert_eq!(nodes[input.n + input.m].child1, !0);
        nodes[input.n + input.m].child1 = nodes[input.n + input.m].child0;

        // rotation
        let mut points = input.points.clone();
        points.push(INLET);
        for u in 0..input.n + input.m + 1 {
            let node = &mut nodes[u];
            node.rotation.clear();
            node.rotation.extend(node.parents.iter().map(|&v| (v, 0)));
            if node.child0 != !0 {
                node.rotation.push((node.child0, 0));
            }
            if node.child1 != !0 && node.child1 != node.child0 {
                node.rotation.push((node.child1, 0));
            }
            let pu = points[u];
            node.rotation.sort_unstable_by(|&(v, _), &(w, _)| {
                let pv = points[v];
                let pw = points[w];
                let ps = Point {
                    x: pu.x - 1,
                    y: pu.y,
                };
                if is_ccw(pu, ps, pv, pw) {
                    return std::cmp::Ordering::Less;
                } else if is_ccw(pu, ps, pw, pv) {
                    return std::cmp::Ordering::Greater;
                } else {
                    return std::cmp::Ordering::Less;
                }
            });
        }
        // rotation index
        for u in 0..input.n + input.m + 1 {
            for i in 0..nodes[u].rotation.len() {
                let v = nodes[u].rotation[i].0;
                nodes[u].rotation[i].1 =
                    nodes[v].rotation.iter().position(|&(w, _)| w == u).unwrap();
            }
        }
        // add non-sorter
        let k = input.k + 1;
        let mut prob = input.prob.clone();
        prob.push(vec![1.0; k]);

        // flatten probabilities
        let n_ceil = LANES_REAL * ((input.n + LANES_REAL - 1) / LANES_REAL);
        let mut prob_flatten = aligned_alloc::<Real>(k * n_ceil * std::mem::size_of::<Real>(), 64);
        prob_flatten.resize(k * n_ceil, 0.0);
        for i in 0..k {
            for j in 0..input.n {
                prob_flatten[i * n_ceil + j] = prob[i][j];
            }
        }
        // fwd, bwd, res, nres, nnres, nnnres
        let mut fwd = aligned_alloc(nodes.len() * n_ceil * REAL_BYTES, 64);
        fwd.resize(nodes.len() * n_ceil, 0.0);
        let mut bwd = aligned_alloc(nodes.len() * n_ceil * REAL_BYTES, 64);
        bwd.resize(nodes.len() * n_ceil, 0.0);
        let mut res = aligned_alloc(n_ceil * REAL_BYTES, 64);
        res.resize(n_ceil, 0.0);
        let mut nres = aligned_alloc(n_ceil * REAL_BYTES, 64);
        nres.resize(n_ceil, 0.0);
        let mut nnres = aligned_alloc(n_ceil * REAL_BYTES, 64);
        nnres.resize(n_ceil, 0.0);
        let mut nnnres = aligned_alloc(n_ceil * REAL_BYTES, 64);
        nnnres.resize(n_ceil, 0.0);

        let mut ret = Self {
            rng: XorShift32::new(1),
            n: input.n,
            m: input.m,
            k,
            n_ceil,
            points,
            prob: prob_flatten,
            nodes,
            fwd,
            bwd,
            order: Vec::with_capacity(input.n + input.m + 1),
            res,
            nres,
            nnres,
            nnnres,
            score: 0.0,
            visited: vec![false; input.n + input.m + 1],
            locks: vec![0; input.n + input.m + 1],
            stack: vec![],
            candidates: (0..k).collect_vec(),
        };
        ret.set_processor();
        ret
    }

    fn set_processor(&mut self) {
        self.set_order();
        let mut rorder = vec![!0; self.n + self.m + 1];
        for (i, &v) in self.order.iter().enumerate() {
            rorder[v] = i;
        }
        let mut processor_parents = vec![0; self.n];
        let mut processor_priority = vec![0; self.n];
        for v in 0..self.n {
            for &u in &self.nodes[v].parents {
                let p = rorder[u];
                if processor_priority[v].chmax(p) {
                    processor_parents[v] = u;
                }
            }
        }
        let processor_order = (0..self.n)
            .sorted_unstable_by_key(|&v| processor_priority[v])
            .collect_vec();
        let mut used_waste = vec![false; self.n];
        for turn in 0..self.n {
            let mut best_waste = !0;
            let mut best_sorter = !0;
            let mut best_switch = false;
            let mut best_profit = -Real::MAX;
            for waste in 0..self.n {
                if used_waste[waste] {
                    continue;
                }
                for sorter in 0..self.k - 1 {
                    let psw = self.prob[sorter * self.n_ceil + waste];
                    let mut profit = (self.n - turn - 1) as Real * psw;
                    for i in 0..self.n {
                        if used_waste[i] || i == waste {
                            continue;
                        }
                        profit -= self.prob[sorter * self.n_ceil + i];
                    }
                    if best_profit.chmax(profit) {
                        best_waste = waste;
                        best_sorter = sorter;
                        best_switch = true;
                    }
                    let mut profit = (self.n - turn - 1) as Real * (1.0 - psw);
                    for i in 0..self.n {
                        if used_waste[i] || i == waste {
                            continue;
                        }
                        profit -= 1.0 - self.prob[sorter * self.n_ceil + i];
                    }
                    if best_profit.chmax(profit) {
                        best_waste = waste;
                        best_sorter = sorter;
                        best_switch = false;
                    }
                }
            }
            let v = processor_order[turn];
            let u = processor_parents[v];
            self.nodes[v].sorter = best_waste;
            self.nodes[u].sorter = best_sorter;
            if best_switch {
                let child0 = self.nodes[u].child0;
                let child1 = self.nodes[u].child1;
                self.nodes[u].child0 = child1;
                self.nodes[u].child1 = child0;
            }
            used_waste[best_waste] = true;
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn set_backward(&mut self) {
        self.set_backward_prepare();
        while let Some(v) = self.stack.pop() {
            debug_assert_eq!(self.locks[v], 0);
            for i in 0..self.nodes[v].parents.len() {
                let u = self.nodes[v].parents[i];
                self.update_bwd(u, v);
                self.locks[u] -= 1;
                debug_assert!(self.locks[u] >= 0);
                if self.locks[u] == 0 {
                    self.stack.push(u);
                }
            }
        }
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn set_backward(&mut self) {
        self.set_backward_prepare();
        while let Some(v) = self.stack.pop() {
            debug_assert_eq!(self.locks[v], 0);
            for i in 0..self.nodes[v].parents.len() {
                let u = self.nodes[v].parents[i];
                self.update_bwd(u, v);
                self.locks[u] -= 1;
                debug_assert!(self.locks[u] >= 0);
                if self.locks[u] == 0 {
                    self.stack.push(u);
                }
            }
        }
    }

    fn set_backward_prepare(&mut self) {
        self.bwd.fill(0.0);
        for v in self.n..self.n + self.m + 1 {
            debug_assert_ne!(self.nodes[v].child0, !0);
            debug_assert_ne!(self.nodes[v].child1, !0);
            if self.nodes[v].child0 == self.nodes[v].child1 {
                self.locks[v] = 1;
            } else {
                self.locks[v] = 2;
            }
        }
        self.stack = vec![];
        for v in 0..self.n {
            let waste = self.nodes[v].sorter;
            self.bwd[v * self.n_ceil + waste] = 1.0;
            self.stack.push(v);
        }
    }

    /// edge from u to v
    #[cfg(not(target_arch = "x86_64"))]
    fn update_bwd(&mut self, u: usize, v: usize) {
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        if child0 == child1 {
            for i in 0..self.n {
                self.bwd[u * self.n_ceil + i] += self.bwd[v * self.n_ceil + i];
            }
        } else if child0 == v {
            for i in 0..self.n {
                self.bwd[u * self.n_ceil + i] +=
                    self.bwd[v * self.n_ceil + i] * self.prob[sorter * self.n_ceil + i];
            }
        } else {
            debug_assert_eq!(child1, v);
            for i in 0..self.n {
                self.bwd[u * self.n_ceil + i] +=
                    self.bwd[v * self.n_ceil + i] * (1.0 - self.prob[sorter * self.n_ceil + i]);
            }
        }
    }

    /// edge from u to v
    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn update_bwd(&mut self, u: usize, v: usize) {
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        if child0 == child1 {
            for i in (0..self.n).step_by(LANES_REAL) {
                let bu = _mm256_load_ps(self.bwd.as_ptr().add(u * self.n_ceil + i));
                let bv = _mm256_load_ps(self.bwd.as_ptr().add(v * self.n_ceil + i));
                let res = _mm256_add_ps(bu, bv);
                _mm256_store_ps(self.bwd.as_mut_ptr().add(u * self.n_ceil + i), res);
            }
        } else if child0 == v {
            for i in (0..self.n).step_by(LANES_REAL) {
                let bu = _mm256_load_ps(self.bwd.as_ptr().add(u * self.n_ceil + i));
                let bv = _mm256_load_ps(self.bwd.as_ptr().add(v * self.n_ceil + i));
                let sp = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil + i));
                let res = _mm256_add_ps(bu, _mm256_mul_ps(bv, sp));
                _mm256_store_ps(self.bwd.as_mut_ptr().add(u * self.n_ceil + i), res);
            }
        } else {
            debug_assert_eq!(child1, v);
            for i in (0..self.n).step_by(LANES_REAL) {
                let bu = _mm256_load_ps(self.bwd.as_ptr().add(u * self.n_ceil + i));
                let bv = _mm256_load_ps(self.bwd.as_ptr().add(v * self.n_ceil + i));
                let sp = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil + i));
                let res = _mm256_add_ps(bu, _mm256_sub_ps(bv, _mm256_mul_ps(bv, sp)));
                _mm256_store_ps(self.bwd.as_mut_ptr().add(u * self.n_ceil + i), res);
            }
        }
    }

    fn set_order(&mut self) {
        let mut que = vec![];
        let mut locks = vec![0; self.n + self.m + 1];
        for v in 0..self.n + self.m {
            locks[v] = self.nodes[v].parents.len() as i32;
            if locks[v] == 0 {
                que.push(v);
            }
        }
        self.rng.shuffle(&mut que);
        que.push(self.n + self.m); // inlet

        self.order.clear();
        while let Some(u) = que.pop() {
            debug_assert_eq!(locks[u], 0);
            self.order.push(u);

            let mut push_child = |v: usize| {
                if v == !0 {
                    return;
                }
                locks[v] -= 1;
                debug_assert!(locks[v] >= 0);
                if locks[v] == 0 {
                    que.push(v);
                }
            };
            if self.rng.gen_range(0, 2) == 0 {
                push_child(self.nodes[u].child0);
                if self.nodes[u].child1 != self.nodes[u].child0 {
                    push_child(self.nodes[u].child1);
                }
            } else {
                push_child(self.nodes[u].child1);
                if self.nodes[u].child0 != self.nodes[u].child1 {
                    push_child(self.nodes[u].child0);
                }
            }
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn update(&mut self, scheduler: &mut AnnealingScheduler, log_eval: bool) {
        self.set_backward();
        self.set_order();
        self.fwd.fill(0.0);

        // set result
        let inlet = self.n + self.m;
        self.res
            .clone_from_slice(&self.bwd[inlet * self.n_ceil..(inlet + 1) * self.n_ceil]);
        self.score = self.eval_result(&self.res, log_eval);
        self.visited.fill(false);
        debug_assert_eq!(self.order[0], inlet);

        self.fwd[inlet * self.n_ceil..inlet * self.n_ceil + self.n].fill(1.0);
        self.visited[inlet] = true;

        self.fwd[self.order[1] * self.n_ceil..self.order[1] * self.n_ceil + self.n].fill(1.0);

        for i in 1..self.order.len() {
            let u = self.order[i];
            self.visited[u] = true;
            if self.nodes[u].child0 == !0 {
                // processor
                continue;
            }
            let r = self.rng.gen_range(0, SA_SORTER_CHILD_R);
            if r < SA_SORTER_R || !self.change_sorter_and_a_child(u, scheduler, log_eval) {
                self.change_sorter(u, scheduler, log_eval);
            }
            self.update_fwd(u);
        }
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn update(&mut self, scheduler: &mut AnnealingScheduler, log_eval: bool) {
        self.set_backward();
        self.set_order();
        self.fwd.fill(0.0);

        // set result
        let inlet = self.n + self.m;
        self.res
            .clone_from_slice(&self.bwd[inlet * self.n_ceil..(inlet + 1) * self.n_ceil]);
        self.score = self.eval_result(&self.res, log_eval);
        self.visited.fill(false);
        debug_assert_eq!(self.order[0], inlet);

        self.fwd[inlet * self.n_ceil..inlet * self.n_ceil + self.n].fill(1.0);
        self.visited[inlet] = true;

        self.fwd[self.order[1] * self.n_ceil..self.order[1] * self.n_ceil + self.n].fill(1.0);

        for i in 1..self.order.len() {
            let u = self.order[i];
            self.visited[u] = true;
            if self.nodes[u].child0 == !0 {
                // processor
                continue;
            }
            let r = self.rng.gen_range(0, SA_SORTER_CHILD_R);
            if r < SA_SORTER_R || !self.change_sorter_and_a_child(u, scheduler, log_eval) {
                self.change_sorter(u, scheduler, log_eval);
            }
            self.update_fwd(u);
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn update_fwd(&mut self, u: usize) {
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        for i in 0..self.n {
            let fp = self.fwd[u * self.n_ceil + i];
            let sp = self.prob[sorter * self.n_ceil + i];
            let fs = fp * sp;
            self.fwd[child0 * self.n_ceil + i] += fs;
            self.fwd[child1 * self.n_ceil + i] += fp - fs;
        }
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn update_fwd(&mut self, u: usize) {
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        for i in (0..self.n).step_by(LANES_REAL) {
            let fp = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil + i));
            let sp = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil + i));
            let fs = _mm256_mul_ps(fp, sp);
            let cptr = self.fwd.as_mut_ptr().add(child0 * self.n_ceil + i);
            let cp = _mm256_load_ps(cptr);
            _mm256_store_ps(cptr, _mm256_add_ps(cp, fs));
            let dptr = self.fwd.as_mut_ptr().add(child1 * self.n_ceil + i);
            let dp = _mm256_load_ps(dptr);
            let fpmfs = _mm256_sub_ps(fp, fs);
            _mm256_store_ps(dptr, _mm256_add_ps(dp, fpmfs));
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn try_sorters(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
        mut nchild0: usize,
        mut nchild1: usize,
        trials: usize,
        osorter: usize, // old_sorter
    ) -> bool {
        // simulate removing edges
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        for i in 0..self.n {
            let mut p = self.res[i];
            let fp = self.fwd[u * self.n_ceil + i];
            let sp = self.prob[sorter * self.n_ceil + i];
            let bp = self.bwd[child0 * self.n_ceil + i];
            let cp = self.bwd[child1 * self.n_ceil + i];
            let sb = sp * (bp - cp);
            p -= fp * (sb + cp);
            self.nres[i] = p;
        }
        // simulate adding edges
        let threshold = self.score + scheduler.get_threshold() as f32;
        for i in 0..trials.min(self.k) {
            let j = self.rng.gen_range(i, self.k);
            self.candidates.swap(i, j);
            let nsorter = self.candidates[i];
            if nsorter == osorter {
                continue; // skip old sorter
            }
            for i in 0..self.n {
                let mut p = self.nres[i];
                let mut q = p;
                let fp = self.fwd[u * self.n_ceil + i];
                let sp = self.prob[nsorter * self.n_ceil + i];
                let bp = self.bwd[nchild0 * self.n_ceil + i];
                let cp = self.bwd[nchild1 * self.n_ceil + i];
                let sb = sp * (bp - cp);
                p += fp * (sb + cp);
                q += fp * (bp - sb);
                self.nnres[i] = p;
                self.nnnres[i] = q;
            }
            let nscore = self.eval_result(&self.nnres, log_eval);
            if nscore > threshold {
                std::mem::swap(&mut self.res, &mut self.nnres);
                self.score = nscore;
            } else {
                // scheduler.rejected();
                let nscore = self.eval_result(&self.nnnres, log_eval);
                if nscore <= threshold {
                    // scheduler.rejected();
                    continue;
                }
                std::mem::swap(&mut self.res, &mut self.nnnres);
                self.score = nscore;
                std::mem::swap(&mut nchild0, &mut nchild1);
            }
            // scheduler.accepted();
            self.nodes[u].sorter = nsorter;
            self.nodes[u].child0 = nchild0;
            self.nodes[u].child1 = nchild1;
            return true;
        }
        false
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn try_sorters1(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
        mut nchild0: usize,
        mut nchild1: usize,
        trials: usize,
        osorter: usize, // old_sorter
    ) -> bool {
        debug_assert!(self.n <= 8);
        // simulate removing edges
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;

        let p0 = _mm256_load_ps(self.res.as_ptr());
        let fp0 = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil));
        let sp0 = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil));
        let bp0 = _mm256_load_ps(self.bwd.as_ptr().add(child0 * self.n_ceil));
        let cp0 = _mm256_load_ps(self.bwd.as_ptr().add(child1 * self.n_ceil));
        let sb0 = _mm256_mul_ps(sp0, _mm256_sub_ps(bp0, cp0));
        let nres0 = _mm256_sub_ps(p0, _mm256_mul_ps(fp0, _mm256_add_ps(sb0, cp0)));

        // simulate adding edges
        let bp0 = _mm256_load_ps(self.bwd.as_ptr().add(nchild0 * self.n_ceil));
        let bp0 = _mm256_mul_ps(fp0, bp0);
        let cp0 = _mm256_load_ps(self.bwd.as_ptr().add(nchild1 * self.n_ceil));
        let cp0 = _mm256_mul_ps(fp0, cp0);
        let pb0 = _mm256_add_ps(nres0, bp0);
        let pc0 = _mm256_add_ps(nres0, cp0);
        let bc0 = _mm256_sub_ps(bp0, cp0);

        let threshold = self.score + scheduler.get_threshold() as f32;
        for i in 0..trials.min(self.k) {
            let j = self.rng.gen_range(i, self.k);
            self.candidates.swap(i, j);
            let nsorter = self.candidates[i];
            if nsorter == osorter {
                continue; // skip old sorter
            }
            let sp0 = _mm256_load_ps(self.prob.as_ptr().add(nsorter * self.n_ceil));
            let p0 = _mm256_add_ps(pc0, _mm256_mul_ps(sp0, bc0));
            if !self.eval_result1(p0, threshold, log_eval) {
                // scheduler.rejected();
                let q0 = _mm256_sub_ps(pb0, _mm256_mul_ps(sp0, bc0));
                if !self.eval_result1(q0, threshold, log_eval) {
                    // scheduler.rejected();
                    continue;
                }
                std::mem::swap(&mut nchild0, &mut nchild1);
            }
            // scheduler.accepted();
            self.nodes[u].sorter = nsorter;
            self.nodes[u].child0 = nchild0;
            self.nodes[u].child1 = nchild1;
            return true;
        }
        false
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn try_sorters2(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
        mut nchild0: usize,
        mut nchild1: usize,
        trials: usize,
        osorter: usize, // old_sorter
    ) -> bool {
        debug_assert!(self.n <= 16);
        // simulate removing edges
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;

        let p0 = _mm256_load_ps(self.res.as_ptr());
        let fp0 = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil));
        let sp0 = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil));
        let bp0 = _mm256_load_ps(self.bwd.as_ptr().add(child0 * self.n_ceil));
        let cp0 = _mm256_load_ps(self.bwd.as_ptr().add(child1 * self.n_ceil));
        let sb0 = _mm256_mul_ps(sp0, _mm256_sub_ps(bp0, cp0));
        let nres0 = _mm256_sub_ps(p0, _mm256_mul_ps(fp0, _mm256_add_ps(sb0, cp0)));

        let p1 = _mm256_load_ps(self.res.as_ptr().add(LANES_REAL));
        let fp1 = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil + LANES_REAL));
        let sp1 = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil + LANES_REAL));
        let bp1 = _mm256_load_ps(self.bwd.as_ptr().add(child0 * self.n_ceil + LANES_REAL));
        let cp1 = _mm256_load_ps(self.bwd.as_ptr().add(child1 * self.n_ceil + LANES_REAL));
        let sb1 = _mm256_mul_ps(sp1, _mm256_sub_ps(bp1, cp1));
        let nres1 = _mm256_sub_ps(p1, _mm256_mul_ps(fp1, _mm256_add_ps(sb1, cp1)));

        // simulate adding edges
        let bp0 = _mm256_load_ps(self.bwd.as_ptr().add(nchild0 * self.n_ceil));
        let bp0 = _mm256_mul_ps(fp0, bp0);
        let cp0 = _mm256_load_ps(self.bwd.as_ptr().add(nchild1 * self.n_ceil));
        let cp0 = _mm256_mul_ps(fp0, cp0);
        let pb0 = _mm256_add_ps(nres0, bp0);
        let pc0 = _mm256_add_ps(nres0, cp0);
        let bc0 = _mm256_sub_ps(bp0, cp0);

        let bp1 = _mm256_load_ps(self.bwd.as_ptr().add(nchild0 * self.n_ceil + LANES_REAL));
        let bp1 = _mm256_mul_ps(fp1, bp1);
        let cp1 = _mm256_load_ps(self.bwd.as_ptr().add(nchild1 * self.n_ceil + LANES_REAL));
        let cp1 = _mm256_mul_ps(fp1, cp1);
        let pb1 = _mm256_add_ps(nres1, bp1);
        let pc1 = _mm256_add_ps(nres1, cp1);
        let bc1 = _mm256_sub_ps(bp1, cp1);

        let threshold = self.score + scheduler.get_threshold() as f32;
        for i in 0..trials.min(self.k) {
            let j = self.rng.gen_range(i, self.k);
            self.candidates.swap(i, j);
            let nsorter = self.candidates[i];
            if nsorter == osorter {
                continue; // skip old sorter
            }
            let sp0 = _mm256_load_ps(self.prob.as_ptr().add(nsorter * self.n_ceil));
            let sp1 = _mm256_load_ps(self.prob.as_ptr().add(nsorter * self.n_ceil + LANES_REAL));
            let p0 = _mm256_add_ps(pc0, _mm256_mul_ps(sp0, bc0));
            let p1 = _mm256_add_ps(pc1, _mm256_mul_ps(sp1, bc1));
            if !self.eval_result2(p0, p1, threshold, log_eval) {
                // scheduler.rejected();
                let q0 = _mm256_sub_ps(pb0, _mm256_mul_ps(sp0, bc0));
                let q1 = _mm256_sub_ps(pb1, _mm256_mul_ps(sp1, bc1));
                if !self.eval_result2(q0, q1, threshold, log_eval) {
                    // scheduler.rejected();
                    continue;
                }
                std::mem::swap(&mut nchild0, &mut nchild1);
            }
            // scheduler.accepted();
            self.nodes[u].sorter = nsorter;
            self.nodes[u].child0 = nchild0;
            self.nodes[u].child1 = nchild1;
            return true;
        }
        false
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn try_sorters3(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
        mut nchild0: usize,
        mut nchild1: usize,
        trials: usize,
        osorter: usize, // old_sorter
    ) -> bool {
        debug_assert!(self.n <= 24);
        // simulate removing edges
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;

        let p0 = _mm256_load_ps(self.res.as_ptr());
        let fp0 = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil));
        let sp0 = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil));
        let bp0 = _mm256_load_ps(self.bwd.as_ptr().add(child0 * self.n_ceil));
        let cp0 = _mm256_load_ps(self.bwd.as_ptr().add(child1 * self.n_ceil));
        let sb0 = _mm256_mul_ps(sp0, _mm256_sub_ps(bp0, cp0));
        let nres0 = _mm256_sub_ps(p0, _mm256_mul_ps(fp0, _mm256_add_ps(sb0, cp0)));

        let p1 = _mm256_load_ps(self.res.as_ptr().add(LANES_REAL));
        let fp1 = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil + LANES_REAL));
        let sp1 = _mm256_load_ps(self.prob.as_ptr().add(sorter * self.n_ceil + LANES_REAL));
        let bp1 = _mm256_load_ps(self.bwd.as_ptr().add(child0 * self.n_ceil + LANES_REAL));
        let cp1 = _mm256_load_ps(self.bwd.as_ptr().add(child1 * self.n_ceil + LANES_REAL));
        let sb1 = _mm256_mul_ps(sp1, _mm256_sub_ps(bp1, cp1));
        let nres1 = _mm256_sub_ps(p1, _mm256_mul_ps(fp1, _mm256_add_ps(sb1, cp1)));

        let p2 = _mm256_load_ps(self.res.as_ptr().add(2 * LANES_REAL));
        let fp2 = _mm256_load_ps(self.fwd.as_ptr().add(u * self.n_ceil + 2 * LANES_REAL));
        let sp2 = _mm256_load_ps(
            self.prob
                .as_ptr()
                .add(sorter * self.n_ceil + 2 * LANES_REAL),
        );
        let bp2 = _mm256_load_ps(self.bwd.as_ptr().add(child0 * self.n_ceil + 2 * LANES_REAL));
        let cp2 = _mm256_load_ps(self.bwd.as_ptr().add(child1 * self.n_ceil + 2 * LANES_REAL));
        let sb2 = _mm256_mul_ps(sp2, _mm256_sub_ps(bp2, cp2));
        let nres2 = _mm256_sub_ps(p2, _mm256_mul_ps(fp2, _mm256_add_ps(sb2, cp2)));

        // simulate adding edges
        let bp0 = _mm256_load_ps(self.bwd.as_ptr().add(nchild0 * self.n_ceil));
        let bp0 = _mm256_mul_ps(fp0, bp0);
        let cp0 = _mm256_load_ps(self.bwd.as_ptr().add(nchild1 * self.n_ceil));
        let cp0 = _mm256_mul_ps(fp0, cp0);
        let pb0 = _mm256_add_ps(nres0, bp0);
        let pc0 = _mm256_add_ps(nres0, cp0);
        let bc0 = _mm256_sub_ps(bp0, cp0);

        let bp1 = _mm256_load_ps(self.bwd.as_ptr().add(nchild0 * self.n_ceil + LANES_REAL));
        let bp1 = _mm256_mul_ps(fp1, bp1);
        let cp1 = _mm256_load_ps(self.bwd.as_ptr().add(nchild1 * self.n_ceil + LANES_REAL));
        let cp1 = _mm256_mul_ps(fp1, cp1);
        let pb1 = _mm256_add_ps(nres1, bp1);
        let pc1 = _mm256_add_ps(nres1, cp1);
        let bc1 = _mm256_sub_ps(bp1, cp1);

        let bp2 = _mm256_load_ps(
            self.bwd
                .as_ptr()
                .add(nchild0 * self.n_ceil + 2 * LANES_REAL),
        );
        let bp2 = _mm256_mul_ps(fp2, bp2);
        let cp2 = _mm256_load_ps(
            self.bwd
                .as_ptr()
                .add(nchild1 * self.n_ceil + 2 * LANES_REAL),
        );
        let cp2 = _mm256_mul_ps(fp2, cp2);
        let pb2 = _mm256_add_ps(nres2, bp2);
        let pc2 = _mm256_add_ps(nres2, cp2);
        let bc2 = _mm256_sub_ps(bp2, cp2);

        let threshold = self.score + scheduler.get_threshold() as f32;
        for i in 0..trials.min(self.k) {
            let j = self.rng.gen_range(i, self.k);
            self.candidates.swap(i, j);
            let nsorter = self.candidates[i];
            if nsorter == osorter {
                continue; // skip old sorter
            }
            let sp0 = _mm256_load_ps(self.prob.as_ptr().add(nsorter * self.n_ceil));
            let sp1 = _mm256_load_ps(self.prob.as_ptr().add(nsorter * self.n_ceil + LANES_REAL));
            let sp2 = _mm256_load_ps(
                self.prob
                    .as_ptr()
                    .add(nsorter * self.n_ceil + 2 * LANES_REAL),
            );
            let p0 = _mm256_add_ps(pc0, _mm256_mul_ps(sp0, bc0));
            let p1 = _mm256_add_ps(pc1, _mm256_mul_ps(sp1, bc1));
            let p2 = _mm256_add_ps(pc2, _mm256_mul_ps(sp2, bc2));
            if !self.eval_result3(p0, p1, p2, threshold, log_eval) {
                // scheduler.rejected();
                let q0 = _mm256_sub_ps(pb0, _mm256_mul_ps(sp0, bc0));
                let q1 = _mm256_sub_ps(pb1, _mm256_mul_ps(sp1, bc1));
                let q2 = _mm256_sub_ps(pb2, _mm256_mul_ps(sp2, bc2));
                if !self.eval_result3(q0, q1, q2, threshold, log_eval) {
                    // scheduler.rejected();
                    continue;
                }
                std::mem::swap(&mut nchild0, &mut nchild1);
            }
            // scheduler.accepted();
            self.nodes[u].sorter = nsorter;
            self.nodes[u].child0 = nchild0;
            self.nodes[u].child1 = nchild1;
            return true;
        }
        false
    }

    /// if the new score is greater than the threshold, update self.res ans self.score
    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn eval_result1(&mut self, p0: __m256, threshold: Real, log_eval: bool) -> bool {
        let nscore = if log_eval {
            let eps = _mm256_set1_ps(EPS_PROB);
            let p0 = _mm256_max_ps(p0, eps);
            let bias = _mm256_set1_epi32(127 << 23);
            let divisor = _mm256_set1_ps((1 << 23) as f32);
            let q0 = log2_fast_avx2(p0, bias, divisor);
            let q = q0;
            _mm256_store_ps(self.nres.as_mut_ptr(), q);
            self.nres[..self.n].iter().sum::<Real>()
        } else {
            let p = p0;
            _mm256_store_ps(self.nres.as_mut_ptr(), p);
            self.nres[..self.n].iter().sum::<Real>()
        };
        if nscore > threshold {
            _mm256_store_ps(self.res.as_mut_ptr(), p0);
            self.score = nscore;
            true
        } else {
            false
        }
    }

    /// if the new score is greater than the threshold, update self.res ans self.score
    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn eval_result2(
        &mut self,
        p0: __m256,
        p1: __m256,
        threshold: Real,
        log_eval: bool,
    ) -> bool {
        let nscore = if log_eval {
            let eps = _mm256_set1_ps(EPS_PROB);
            let p0 = _mm256_max_ps(p0, eps);
            let p1 = _mm256_max_ps(p1, eps);
            let bias = _mm256_set1_epi32(127 << 23);
            let divisor = _mm256_set1_ps((1 << 23) as f32);
            let q0 = log2_fast_avx2(p0, bias, divisor);
            let q1 = log2_fast_avx2(p1, bias, divisor);
            let q = _mm256_add_ps(q0, q1);
            _mm256_store_ps(self.nres.as_mut_ptr(), q);
            let nscore = self.nres[..LANES_REAL].iter().sum::<Real>();
            nscore - (self.n_ceil - self.n) as Real * LOG_EPS
        } else {
            let p = _mm256_add_ps(p0, p1);
            _mm256_store_ps(self.nres.as_mut_ptr(), p);
            self.nres[..LANES_REAL].iter().sum::<Real>()
        };
        if nscore > threshold {
            _mm256_store_ps(self.res.as_mut_ptr(), p0);
            _mm256_store_ps(self.res.as_mut_ptr().add(LANES_REAL), p1);
            self.score = nscore;
            true
        } else {
            false
        }
    }

    /// if the new score is greater than the threshold, update self.res ans self.score
    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn eval_result3(
        &mut self,
        p0: __m256,
        p1: __m256,
        p2: __m256,
        threshold: Real,
        log_eval: bool,
    ) -> bool {
        let nscore = if log_eval {
            let eps = _mm256_set1_ps(EPS_PROB);
            let p0 = _mm256_max_ps(p0, eps);
            let p1 = _mm256_max_ps(p1, eps);
            let p2 = _mm256_max_ps(p2, eps);
            let bias = _mm256_set1_epi32(127 << 23);
            let divisor = _mm256_set1_ps((1 << 23) as f32);
            let q0 = log2_fast_avx2(p0, bias, divisor);
            let q1 = log2_fast_avx2(p1, bias, divisor);
            let q2 = log2_fast_avx2(p2, bias, divisor);
            let q = _mm256_add_ps(_mm256_add_ps(q0, q1), q2);
            _mm256_store_ps(self.nres.as_mut_ptr(), q);
            let nscore = self.nres[..LANES_REAL].iter().sum::<Real>();
            nscore - (self.n_ceil - self.n) as Real * LOG_EPS
        } else {
            let p = _mm256_add_ps(_mm256_add_ps(p0, p1), p2);
            _mm256_store_ps(self.nres.as_mut_ptr(), p);
            self.nres[..LANES_REAL].iter().sum::<Real>()
        };
        if nscore > threshold {
            _mm256_store_ps(self.res.as_mut_ptr(), p0);
            _mm256_store_ps(self.res.as_mut_ptr().add(LANES_REAL), p1);
            _mm256_store_ps(self.res.as_mut_ptr().add(2 * LANES_REAL), p2);
            self.score = nscore;
            true
        } else {
            false
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn change_sorter(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
    ) -> bool {
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        let (nchild0, nchild1) = if self.rng.gen_range(0, 2) == 0 {
            (child0, child1)
        } else {
            (child1, child0)
        };
        self.try_sorters(
            u,
            scheduler,
            log_eval,
            nchild0,
            nchild1,
            SORTER_PATIENCE,
            sorter,
        )
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn change_sorter(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
    ) -> bool {
        let sorter = self.nodes[u].sorter;
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        let (nchild0, nchild1) = if self.rng.gen_range(0, 2) == 0 {
            (child0, child1)
        } else {
            (child1, child0)
        };
        if self.n <= 8 {
            self.try_sorters1(
                u,
                scheduler,
                log_eval,
                nchild0,
                nchild1,
                SORTER_PATIENCE,
                sorter,
            )
        } else if self.n <= 16 {
            self.try_sorters2(
                u,
                scheduler,
                log_eval,
                nchild0,
                nchild1,
                SORTER_PATIENCE,
                sorter,
            )
        } else {
            self.try_sorters3(
                u,
                scheduler,
                log_eval,
                nchild0,
                nchild1,
                SORTER_PATIENCE,
                sorter,
            )
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn change_sorter_and_a_child(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
    ) -> bool {
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        let (nchild0, nchild1) = self.choose_nchildren(u, child0, child1);
        if nchild0 == !0 {
            return false;
        }
        if !self.try_sorters(u, scheduler, log_eval, nchild0, nchild1, self.k, !0) {
            return false;
        }
        self.update_graph(u, child0, child1);
        true
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn change_sorter_and_a_child(
        &mut self,
        u: usize,
        scheduler: &mut AnnealingScheduler,
        log_eval: bool,
    ) -> bool {
        let child0 = self.nodes[u].child0;
        let child1 = self.nodes[u].child1;
        let (nchild0, nchild1) = self.choose_nchildren(u, child0, child1);
        if nchild0 == !0 {
            return false;
        }
        let updated = if self.n <= 8 {
            self.try_sorters1(u, scheduler, log_eval, nchild0, nchild1, self.k, !0)
        } else if self.n <= 16 {
            self.try_sorters2(u, scheduler, log_eval, nchild0, nchild1, self.k, !0)
        } else {
            self.try_sorters3(u, scheduler, log_eval, nchild0, nchild1, self.k, !0)
        };
        if !updated {
            return false;
        }
        self.update_graph(u, child0, child1);
        true
    }

    /// if it cannot find a valid child, it will return !0
    fn choose_nchildren(&mut self, u: usize, child0: usize, child1: usize) -> (usize, usize) {
        // forbid in-degree 0
        let fix_child0 = self.nodes[child0].parents.len() == 1;
        let fix_child1 = self.nodes[child1].parents.len() == 1;
        if fix_child0 && fix_child1 {
            return (!0, !0);
        }
        let nchild0 = if fix_child0 {
            child0
        } else if fix_child1 {
            child1
        } else if self.rng.gen_range(0, 2) == 0 {
            child1
        } else {
            child0
        };
        let i = self.rng.gen_range(0, self.nodes[u].rotation.len());
        let face = self.get_face(u, i);
        let mut candidates = face
            .iter()
            .cloned()
            .filter(|&v| !self.visited[v])
            .collect_vec(); // candidates for child0
        for len in (1..=candidates.len()).rev() {
            let nchild1 = candidates.swap_remove(self.rng.gen_range(0, len));
            if nchild1 != child0 && nchild1 != child1 && self.can_add_edge(u, nchild1, &face) {
                if self.rng.gen_range(0, 2) == 0 {
                    return (nchild0, nchild1);
                } else {
                    return (nchild1, nchild0);
                }
            }
        }
        (!0, !0)
    }

    fn update_graph(&mut self, u: usize, child0: usize, child1: usize) {
        let nchild0 = self.nodes[u].child0;
        let nchild1 = self.nodes[u].child1;
        if nchild0 != child0 && nchild0 != child1 {
            self.add_edge(u, nchild0);
        }
        if nchild1 != child0 && nchild1 != child1 && nchild1 != nchild0 {
            self.add_edge(u, nchild1);
        }
        if child0 != nchild0 && child0 != nchild1 {
            self.remove_edge(u, child0);
        }
        if child1 != nchild0 && child1 != nchild1 && child1 != child0 {
            self.remove_edge(u, child1);
        }
    }

    fn eval_result(&self, res: &[Real], log_eval: bool) -> Real {
        debug_assert_eq!(res.len(), self.n_ceil);
        if log_eval {
            return res[..self.n]
                .iter()
                .map(|&x| log2_fast(x.max(EPS_PROB)))
                .sum::<Real>();
        } else {
            return res[..self.n].iter().sum::<Real>();
        }
    }

    fn get_face(&self, u0: usize, i: usize) -> Vec<usize> {
        let mut face = vec![];
        let mut u = u0;
        let (v0, mut i) = self.nodes[u0].rotation[i];
        let mut v = v0;
        loop {
            face.push(u);
            debug_assert_eq!(self.nodes[v].rotation[i].0, u);
            let j = if i + 1 < self.nodes[v].rotation.len() {
                i + 1
            } else {
                0
            };
            u = v;
            (v, i) = self.nodes[v].rotation[j];
            if u == u0 && v == v0 {
                break;
            }
        }
        face
    }

    /// call before remove_edge
    fn add_edge(&mut self, u: usize, v: usize) {
        debug_assert!(!self.nodes[v].parents.contains(&u));
        self.nodes[v].parents.push(u);
        let i = self.rotation_insert(u, v);
        let j = self.rotation_insert(v, u);
        self.nodes[u].rotation[i].1 = j;
        self.nodes[v].rotation[j].1 = i;
    }

    /// insert v into self.nodes[u].rotation
    #[inline(always)]
    fn rotation_insert(&mut self, u: usize, v: usize) -> usize {
        self.nodes[u].rotation.push((0, 0));
        for i in (0..self.nodes[u].rotation.len() - 1).rev() {
            let (w, j) = self.nodes[u].rotation[i];
            if self.lt_rotation(u, v, w) {
                self.nodes[u].rotation[i + 1] = (w, j);
                debug_assert_eq!(self.nodes[w].rotation[j].0, u);
                self.nodes[w].rotation[j].1 = i + 1;
            } else {
                self.nodes[u].rotation[i + 1] = (v, 0);
                return i + 1;
            }
        }
        self.nodes[u].rotation[0] = (v, 0);
        0
    }

    /// call after add_edge
    fn remove_edge(&mut self, u: usize, v: usize) {
        let i = self.nodes[u]
            .rotation
            .iter()
            .position(|&(w, _)| w == v)
            .unwrap();
        let j = self.nodes[u].rotation[i].1;
        debug_assert_eq!(self.nodes[v].rotation[j].0, u);
        self.rotation_remove(u, i);
        self.rotation_remove(v, j);

        let i = self.nodes[v].parents.iter().position(|&x| x == u).unwrap();
        self.nodes[v].parents.swap_remove(i);
    }

    /// remove v from self.nodes[u].rotation
    fn rotation_remove(&mut self, u: usize, i: usize) {
        for j in i + 1..self.nodes[u].rotation.len() {
            let (v, k) = self.nodes[u].rotation[j];
            self.nodes[u].rotation[j - 1] = (v, k);
            debug_assert_eq!(self.nodes[v].rotation[k].0, u);
            self.nodes[v].rotation[k].1 = j - 1;
        }
        self.nodes[u].rotation.pop();
    }

    fn segments_intersect(&self, u: usize, v: usize, p: usize, q: usize) -> bool {
        if u == p || u == q || v == p || v == q {
            let uv = self.points[v] - self.points[u];
            let pq = self.points[q] - self.points[p];
            if cross(uv, pq) == 0 {
                return true;
            }
            return false;
        }
        segments_intersect(
            self.points[u],
            self.points[v],
            self.points[p],
            self.points[q],
        )
    }

    /// pq is in face and pq does not intersect with any edge
    fn can_add_edge(&self, p: usize, q: usize, face: &[usize]) -> bool {
        debug_assert_eq!(p, face[0]);
        debug_assert!(face.contains(&q));
        let u = face[1];
        let v = face[face.len() - 1];
        if q == u || q == v {
            return true;
        }
        if !is_ccw(
            self.points[p],
            self.points[v],
            self.points[q],
            self.points[u],
        ) {
            return false;
        }
        for i in 1..face.len() - 1 {
            let u = face[i];
            let v = face[i + 1];
            if self.segments_intersect(p, q, u, v) {
                return false;
            }
        }
        true
    }

    /// uv.arg() < uw.arg()
    #[inline(always)]
    fn lt_rotation(&self, u: usize, v: usize, w: usize) -> bool {
        let pu = self.points[u];
        let pv = self.points[v];
        let pw = self.points[w];
        let ps = Point {
            x: pu.x - 1,
            y: pu.y,
        };
        is_ccw(pu, ps, pv, pw)
    }

    fn make_output(&self) -> Output {
        let d = (0..self.n).map(|i| self.nodes[i].sorter).collect_vec();
        let s = self.nodes[self.n + self.m].child0;
        let kuv = (self.n..self.n + self.m)
            .map(|i| {
                let sorter = self.nodes[i].sorter;
                let child0 = self.nodes[i].child0;
                let child1 = self.nodes[i].child1;
                if sorter == self.k - 1 {
                    (0, child0, child0)
                } else {
                    (sorter, child0, child1)
                }
            })
            .collect_vec();
        let score = self.score;
        Output { d, s, kuv, score }
    }
}

#[derive(Debug, Clone)]
struct Node {
    sorter: usize, // sorter type or waste type of processor
    parents: Vec<usize>,
    child0: usize,
    child1: usize,
    rotation: Vec<(usize, usize)>, // adjacent nodes in cyclic order
}

impl Node {
    fn new() -> Self {
        Self {
            sorter: !0,
            parents: vec![],
            child0: !0,
            child1: !0,
            rotation: vec![],
        }
    }
}

#[derive(Debug, Clone)]
struct Output {
    d: Vec<usize>,
    s: usize,
    kuv: Vec<(usize, usize, usize)>,
    score: Real,
}

impl Output {
    fn print(&self) {
        println!("{}", self.d.iter().map(|&x| x.to_string()).join(" "));
        println!("{}", self.s);
        for &(k, u, v) in &self.kuv {
            if k == !0 {
                println!("-1");
            } else {
                println!("{} {} {}", k, u, v);
            }
        }
    }
}

#[derive(Debug, Clone)]
struct GraphBuilder {
    n: usize,
    m: usize,
    points: Vec<Point>,
}

impl GraphBuilder {
    fn new(input: &Input) -> Self {
        let mut ret = Self {
            n: input.n,
            m: input.m,
            points: input.points.clone(),
        };
        ret.points.push(INLET);
        return ret;
    }

    fn build(&self) -> Vec<(usize, usize)> {
        let order = self.solve_tsp();
        debug_assert_eq!(order.len(), self.m + 2);

        let mut edges = Vec::with_capacity(2 * self.m + 1);
        for i in 0..order.len() - 1 {
            edges.push((order[i], order[i + 1]));
        }
        let mut rorder = vec![!0; self.n + self.m + 1];
        for (i, &u) in order.iter().enumerate() {
            rorder[u] = i;
        }

        let mut to = vec![!0; self.n + self.m];

        // ensure that each processor has in-degree at least one.
        let mut processor_parents = vec![!0; self.n];
        for v in 0..self.n {
            if v == order[order.len() - 1] {
                processor_parents[v] = order[order.len() - 2];
                continue;
            }
            let mut candidates = Vec::with_capacity(self.m);
            for u in self.n..self.n + self.m {
                if to[u] != !0 {
                    continue;
                }
                let dist = self.euclid2(v, u);
                candidates.push((dist, u));
            }
            candidates.sort_unstable_by_key(|&(dist, _)| dist);

            let u = candidates
                .iter()
                .find(|&&(_, u)| !self.any_intersect(v, u, &edges));
            if u.is_none() {
                eprintln!("No valid sorter found for processor {}", v);
                exit(1);
            }
            let u = u.unwrap().1;
            to[u] = v;
            edges.push((u, v));
            processor_parents[v] = u;
        }
        // replace processor parents into later sorters
        for v in 0..self.n {
            let pv = processor_parents[v];
            for &u in order[rorder[pv] + 1..order.len() - 1].iter().rev() {
                if to[u] != !0 {
                    continue;
                }
                if self.any_intersect(v, u, &edges) {
                    continue;
                }
                let i = edges.iter().position(|&(x, y)| x == pv && y == v).unwrap();
                to[pv] = !0;
                to[u] = v;
                edges[i] = (u, v);
                processor_parents[v] = u;
                break;
            }
        }
        // ensure that each sorter has out-degree two
        let mut visited = vec![false; self.n + self.m];
        for u in 0..self.n {
            visited[u] = true;
        }
        for i in (1..order.len() - 1).rev() {
            let u = order[i];
            if to[u] != !0 {
                visited[u] = true;
                continue;
            }
            let mut candidates = Vec::with_capacity(self.n);
            for v in 0..self.n + self.m {
                if !visited[v] {
                    continue;
                }
                let mut dist = self.euclid2(v, u);
                if v == order[i + 1] {
                    dist = i32::MAX;
                }
                candidates.push((dist, v));
            }
            candidates.sort_by_key(|&(dist, _)| dist);
            let v = candidates
                .iter()
                .find(|&&(_, v)| v == order[i + 1] || !self.any_intersect(v, u, &edges))
                .unwrap()
                .1;
            to[u] = v;
            edges.push((u, v));
            visited[u] = true;
        }
        debug_assert_eq!(edges.len(), 2 * self.m + 1);
        edges
    }

    fn find_nearest_processor_pair(&self) -> (usize, usize) {
        let mut min_dist = i32::MAX;
        let mut nearest_pair = (0, 0);
        for u in 0..self.n {
            for v in u + 1..self.n {
                if min_dist.chmin(euclid2(self.points[u], self.points[v])) {
                    nearest_pair = (u, v);
                }
            }
        }
        nearest_pair
    }

    fn decide_first_sorter(&self) -> usize {
        let pi = INLET;
        let pb = Point {
            x: pi.x - 1,
            y: pi.y,
        };
        let mut argmin_vertex = 0;
        for i in 0..self.n + self.m {
            let pc = self.points[argmin_vertex];
            let pd = self.points[i];
            if is_ccw(pi, pb, pc, pd) {
                continue;
            } else if is_ccw(pi, pb, pd, pc) {
                argmin_vertex = i;
            } else {
                let dist1 = euclid2(pi, pc);
                let dist2 = euclid2(pi, pd);
                if dist2 < dist1 {
                    argmin_vertex = i;
                }
            }
        }
        let mut argmax_vertex = 0;
        for i in 0..self.n + self.m {
            let pc = self.points[argmax_vertex];
            let pd = self.points[i];
            if is_ccw(pi, pb, pc, pd) {
                argmax_vertex = i;
            } else if is_ccw(pi, pb, pd, pc) {
                continue;
            } else {
                let dist1 = euclid(pi, pc);
                let dist2 = euclid(pi, pd);
                if dist2 < dist1 {
                    argmax_vertex = i;
                }
            }
        }
        let mut nearest_vertex = 0;
        let mut nearest_dist = i32::MAX;
        for i in self.n..self.n + self.m {
            let dist = self.euclid2(i, self.n + self.m);
            if dist < nearest_dist {
                nearest_dist = dist;
                nearest_vertex = i;
            }
        }
        let mut ret = nearest_vertex;
        let mut cost = 0;
        if argmin_vertex >= self.n && cost.chmax(self.nearest_processor(argmin_vertex).1) {
            ret = argmin_vertex;
        }
        if argmax_vertex >= self.n && cost.chmax(self.nearest_processor(argmax_vertex).1) {
            ret = argmax_vertex;
        }
        if cost.chmax(self.nearest_processor(nearest_vertex).1) {
            ret = nearest_vertex;
        }
        ret
    }

    fn nearest_processor(&self, u: usize) -> (usize, i32) {
        let mut nearest = (0, i32::MAX);
        for v in 0..self.n {
            let dist = self.euclid2(u, v);
            if dist < nearest.1 {
                nearest = (v, dist);
            }
        }
        nearest
    }

    fn solve_tsp(&self) -> Vec<usize> {
        let last_processor = self.find_nearest_processor_pair().0;
        let first_sorter = self.decide_first_sorter();

        let mut order = vec![self.n + self.m, first_sorter];
        order.extend(self.n..first_sorter);
        order.extend(first_sorter + 1..self.n + self.m);
        order.push(last_processor);
        debug_assert_eq!(order.len(), self.m + 2);

        // SA 2-opt
        let end_time = get_time_sec() + TSP_TIME_SEC;
        let mut scheduler = AnnealingScheduler::new(TSP_SCHED_T, TSP_T0, TSP_T1, end_time);

        let mut rng = XorShift32::new(1);

        while get_time_sec() < end_time {
            for _ in 0..SA_TIME_COUNTS {
                let mut l = rng.gen_range(2, self.m + 2);
                let mut r = rng.gen_range(2, self.m + 2);
                if l > r {
                    std::mem::swap(&mut l, &mut r);
                }
                if r - l < 2 {
                    continue;
                }
                let l0 = order[l - 1];
                let l1 = order[l];
                let r0 = order[r - 1];
                let r1 = order[r];
                let mut profit = self.euclid(l0, l1) + self.euclid(r0, r1);
                profit -= self.euclid(l0, r0) + self.euclid(l1, r1);
                if scheduler.accept(profit as f64) {
                    order[l..r].reverse();
                }
            }
        }
        let mut update = true;
        while update {
            update = false;
            for l in 1..self.m + 2 {
                for r in l + 2..self.m + 2 {
                    let l0 = order[l - 1];
                    let l1 = order[l];
                    let r0 = order[r - 1];
                    let r1 = order[r];
                    if self.segments_intersect(l0, l1, r0, r1) {
                        order[l..r].reverse();
                        update = true;
                    }
                }
            }
        }
        order
    }

    fn euclid2(&self, u: usize, v: usize) -> i32 {
        euclid2(self.points[u], self.points[v])
    }

    fn euclid(&self, u: usize, v: usize) -> f64 {
        euclid(self.points[u], self.points[v])
    }

    fn segments_intersect(&self, u: usize, v: usize, p: usize, q: usize) -> bool {
        if u == p || u == q || v == p || v == q {
            return false;
        }
        segments_intersect(
            self.points[u],
            self.points[v],
            self.points[p],
            self.points[q],
        )
    }

    fn any_intersect(&self, p: usize, q: usize, edges: &[(usize, usize)]) -> bool {
        for &(u, v) in edges {
            if self.segments_intersect(p, q, u, v) {
                return true;
            }
        }
        false
    }
}

#[derive(Debug, Clone)]
struct Input {
    n: usize,
    m: usize,
    k: usize,
    points: Vec<Point>,
    prob: Vec<Vec<Real>>,
}

fn read_input() -> Input {
    input! {
        n: usize,
        m: usize,
        k: usize,
        points: [(i32, i32); n + m],
        prob: [[Real; n]; k],
    }
    debug_assert!(5 <= n && n <= 20);
    debug_assert!(10 * n <= m && m <= 50 * n);
    debug_assert!(n <= k && k <= 4 * n);
    let points = points.into_iter().map(|(x, y)| Point { x, y }).collect();
    Input {
        n,
        m,
        k,
        points,
        prob,
    }
}

#[derive(Debug, Clone, Copy)]
struct Point {
    x: i32,
    y: i32,
}

impl std::ops::Sub for Point {
    type Output = Point;

    fn sub(self, other: Point) -> Point {
        Point {
            x: self.x - other.x,
            y: self.y - other.y,
        }
    }
}

fn orientation(a: Point, b: Point, c: Point) -> i64 {
    let cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    cross as i64
}

fn segments_intersect(p1: Point, p2: Point, q1: Point, q2: Point) -> bool {
    if p1.x.max(p2.x) < q1.x.min(q2.x)
        || q1.x.max(q2.x) < p1.x.min(p2.x)
        || p1.y.max(p2.y) < q1.y.min(q2.y)
        || q1.y.max(q2.y) < p1.y.min(p2.y)
    {
        return false;
    }
    let o1 = orientation(p1, p2, q1);
    let o2 = orientation(p1, p2, q2);
    let o3 = orientation(q1, q2, p1);
    let o4 = orientation(q1, q2, p2);
    (o1 * o2 <= 0) && (o3 * o4 <= 0)
}

fn cross(a: Point, b: Point) -> i32 {
    a.x * b.y - a.y * b.x
}

/// ab.arg() < ac.arg() < ad.arg() in counter-clockwise
fn is_ccw(a: Point, b: Point, c: Point, d: Point) -> bool {
    let ab = b - a;
    let ac = c - a;
    let ad = d - a;

    let cross_bd = cross(ab, ad);
    let cross_bc = cross(ab, ac);
    let cross_cd = cross(ac, ad);

    if cross_bd > 0 {
        cross_bc > 0 && cross_cd > 0
    } else {
        !(cross_bc <= 0 && cross_cd <= 0)
    }
}

fn euclid2(a: Point, b: Point) -> i32 {
    let dx = a.x - b.x;
    let dy = a.y - b.y;
    dx * dx + dy * dy
}

fn euclid(a: Point, b: Point) -> f64 {
    (euclid2(a, b) as f64).sqrt()
}

pub fn log2_fast(x: f32) -> f32 {
    let y = (x.to_bits() as i32) - (127 << 23);
    y as f32 / (1 << 23) as f32
}

#[cfg(target_arch = "x86_64")]
#[target_feature(enable = "avx2")]
/// let bias = _mm256_set1_epi32(127 << 23);
/// let divisor = _mm256_set1_ps((1 << 23) as f32);
unsafe fn log2_fast_avx2(x: __m256, bias: __m256i, divisor: __m256) -> __m256 {
    let x_bits = _mm256_castps_si256(x);
    let y = _mm256_sub_epi32(x_bits, bias);
    let y_float = _mm256_cvtepi32_ps(y);
    _mm256_div_ps(y_float, divisor)
}

/// size: bytes
/// align: bytes
/// return empty vector
pub fn aligned_alloc<T>(size: usize, align: usize) -> Vec<T> {
    debug_assert!(align.is_power_of_two());
    debug_assert_eq!(size % std::mem::size_of::<T>(), 0);

    let capacity = size / std::mem::size_of::<T>();
    unsafe {
        let layout = std::alloc::Layout::from_size_align_unchecked(size, align);
        let ptr = std::alloc::alloc(layout);
        Vec::from_raw_parts(ptr as *mut T, 0, capacity)
    }
}

const SA_TIME_COUNTS: usize = 1 << 10;
const SA_RANDOM_STEPS: usize = 1 << 12;

#[derive(Debug, Clone)]
pub enum SchedulerType {
    Exp,
    Linear,
}

#[derive(Debug, Clone)]
pub struct AnnealingScheduler {
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

impl AnnealingScheduler {
    pub fn new(
        schedule_type: SchedulerType,
        t_first: f64,
        t_last: f64,
        time_limit_sec: f64,
    ) -> Self {
        debug_assert!(0.0 <= t_last && t_last <= t_first);

        let mut log2_random = vec![0.0; SA_RANDOM_STEPS];
        for i in 0..SA_RANDOM_STEPS {
            log2_random[i] = ((i + 1) as f64 / SA_RANDOM_STEPS as f64).log2();
        }

        use rand::seq::SliceRandom;
        use rand::SeedableRng;

        let mut rng = rand_pcg::Pcg64Mcg::seed_from_u64(0);
        log2_random.shuffle(&mut rng);
        Self {
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
        if profit >= 0.0 || profit > self.get_threshold() {
            self.accepted();
            return true;
        } else {
            self.rejected();
            return false;
        }
    }

    pub fn accepted(&mut self) {
        self.trials += 1;
        self.acceptances += 1;
    }

    pub fn rejected(&mut self) {
        self.trials += 1;
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
            SchedulerType::Exp => self.t_first.powf(1.0 - progress) * self.t_last.powf(progress),
            SchedulerType::Linear => self.t_first * (1.0 - progress) + self.t_last * progress,
        }
    }

    pub fn print_log(&self) {
        let acceptance_rate = self.acceptances as f64 / self.trials as f64;
        eprintln!("trial : {}", self.trials);
        eprintln!("accept: {}", self.acceptances);
        eprintln!("rate  : {}", acceptance_rate);
    }
}

#[derive(Debug, Clone, Copy)]
pub struct XorShift32 {
    state: u32,
}

impl XorShift32 {
    pub fn new(mut seed: u32) -> Self {
        if seed == 0 {
            seed = u32::MAX;
        }
        Self { state: seed }
    }

    /// 1..=u32::MAX
    #[inline(always)]
    pub fn gen(&mut self) -> u32 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.state = x;
        x
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f32(&mut self) -> f32 {
        self.gen() as f32 / ((1u64 << 32) as f32)
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f64(&mut self) -> f64 {
        self.gen() as f64 / ((1u64 << 32) as f64)
    }

    /// l..r
    #[inline(always)]
    pub fn gen_range(&mut self, l: usize, r: usize) -> usize {
        debug_assert!(l < r);
        debug_assert!(r as u64 <= 1 << 32);
        l + (((r - l) as u64 * self.gen() as u64) >> 32) as usize
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f32(&mut self, l: f32, r: f32) -> f32 {
        debug_assert!(l <= r);
        l + self.gen() as f32 * ((r - l) / ((1u64 << 32) as f32))
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f64(&mut self, l: f64, r: f64) -> f64 {
        debug_assert!(l <= r);
        l + self.gen() as f64 * ((r - l) / ((1u64 << 32) as f64))
    }

    #[inline(always)]
    pub fn shuffle(&mut self, v: &mut [usize]) {
        let n = v.len();
        for i in (1..n).rev() {
            let j = self.gen_range(0, i + 1);
            v.swap(i, j);
        }
    }

    #[inline(always)]
    pub fn partial_shuffle(&mut self, v: &mut [usize], n: usize) {
        let m = v.len();
        debug_assert!(n <= m);
        for i in 0..n {
            let j = self.gen_range(i, m);
            v.swap(i, j);
        }
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
