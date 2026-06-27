use std::{collections::BinaryHeap, process::exit};

use itertools::Itertools;
use proconio::input;

const TIME_LIMIT_SEC: f64 = 1.9;
const N: usize = 200;

const BEAM_TURN: usize = 40;
const TURN_W: i32 = 3;

const GREEDY_TIME: f64 = 0.1 * TIME_LIMIT_SEC;

const A_PRUNE: usize = 10;

const SCHED_TYPE: SchedulerType = SchedulerType::Exp;
const SA_T0: f64 = 10.0;
const SA_T1: f64 = 0.75;

fn main() {
    get_time_sec();

    let input = read_input();

    let config = Config {
        max_turn: 500 * N,
        beam_width: 50,
        tour_capacity: 0,
    };
    let actions = beam_search(&config, State::new(&input)).unwrap();

    let mut best_state = GreedyState::new(&input, &actions, 1);
    best_state.solve();
    while get_time_sec() < GREEDY_TIME {
        let mut state = GreedyState::new(&input, &actions, best_state.rng.gen());
        state.solve();
        if state.attacks.len() < best_state.attacks.len() {
            best_state = state;
        }
    }

    let (mut best_state, mut env) = SAState::new(&input, &best_state);
    let mut state = best_state.clone();
    while get_time_sec() < TIME_LIMIT_SEC {
        for _ in 0..SA_TIME_COUNTS {
            state.update(&mut env);
            if state.cost < best_state.cost {
                best_state.clone_from(&state);
            }
        }
    }
    best_state.print(&input);

    exit(0);
}

#[derive(Debug, Clone)]
struct Environment {
    a: [[i32; N]; N + 1],
    ra: [Vec<usize>; N],
    rra: [Vec<usize>; N],
    scheduler: AnnealingScheduler,
}

#[derive(Debug, Clone)]
struct SAState {
    rng: XorShift32,
    graph: Graph,
    hardness: [i32; N],
    durability: [i32; N + 1],
    cost: i32,
    positive_hard: IndexSet,
}

impl SAState {
    fn new(input: &Input, state: &GreedyState) -> (Self, Environment) {
        let mut graph = Graph::new();
        let mut hardness = input.h.clone();
        let mut durability = input.c.clone();
        for &(u, v) in &state.attacks {
            if u == N {
                continue;
            }
            graph.add_edge(u, v);
            hardness[v] -= input.a[u][v];
            durability[u] -= 1;
        }
        let mut ra_sorted = vec![vec![]; N];
        for u in 0..N {
            for v in 0..N {
                if input.a[u][v] > 0 {
                    ra_sorted[v].push((input.a[u][v], u));
                }
            }
        }
        for v in 0..N {
            ra_sorted[v].sort_by_key(|&(a, _)| -a);
        }
        let mut ra = vec![vec![]; N];
        for v in 0..N {
            ra[v] = ra_sorted[v][..N / A_PRUNE].iter().map(|&(_, u)| u).collect();
        }
        let mut rra = vec![vec![]; N];
        for v in 0..N {
            for &u in &ra[v] {
                rra[u].push(v);
            }
        }
        let mut positive_hard = IndexSet::new(N);
        for v in 0..N {
            if hardness[v] > 0 {
                positive_hard.push(v);
            }
        }

        let state = Self {
            rng: state.rng.clone(),
            graph,
            hardness,
            durability,
            cost: hardness.iter().filter(|&&h| h > 0).sum(),
            positive_hard,
        };
        let env = Environment {
            a: input.a.clone(),
            ra: ra.try_into().unwrap(),
            rra: rra.try_into().unwrap(),
            scheduler: AnnealingScheduler::new(SCHED_TYPE, SA_T0, SA_T1, TIME_LIMIT_SEC),
        };
        (state, env)
    }

    fn update(&mut self, env: &mut Environment) {
        if self.rng.gen_range(0, 2) == 0 {
            self.change_edge(env);
        } else {
            self.swap_two_edges(env);
        }
    }

    fn change_edge(&mut self, env: &mut Environment) {
        let nv = self.positive_hard.data[self.rng.gen_range(0, self.positive_hard.len())];
        let u = env.ra[nv][self.rng.gen_range(0, env.ra[nv].len())];

        if self.durability[u] > 0 {
            let profit = self.hardness[nv].max(0) - (self.hardness[nv] - env.a[u][nv]).max(0);
            if !env.scheduler.accept(profit as f64) {
                return;
            }
            if !self.graph.can_add_edge(u, nv) {
                return;
            }
            self.graph.add_edge(u, nv);
            if self.hardness[nv] > 0 && self.hardness[nv] - env.a[u][nv] <= 0 {
                self.positive_hard.pop(nv);
            }
            self.hardness[nv] -= env.a[u][nv];
            self.durability[u] -= 1;
            self.cost -= profit;
            return;
        }

        if self.graph.edges[u].is_empty() {
            return;
        }
        let &(ov, _) = self.graph.edges[u]
            .get(self.rng.gen_range(0, self.graph.edges[u].len()))
            .unwrap();
        if ov == nv {
            return;
        }
        let mut profit = self.hardness[nv].max(0) - (self.hardness[nv] - env.a[u][nv]).max(0);
        profit += self.hardness[ov].max(0) - (self.hardness[ov] + env.a[u][ov]).max(0);
        if !env.scheduler.accept(profit as f64) {
            return;
        }
        if !self.graph.can_add_edge(u, nv) {
            return;
        }
        self.graph.remove_edge(u, ov);
        self.graph.add_edge(u, nv);
        if self.hardness[nv] > 0 && self.hardness[nv] - env.a[u][nv] <= 0 {
            self.positive_hard.pop(nv);
        }
        self.hardness[nv] -= env.a[u][nv];
        if self.hardness[ov] <= 0 && self.hardness[ov] + env.a[u][ov] > 0 {
            self.positive_hard.push(ov);
        }
        self.hardness[ov] += env.a[u][ov];
        self.cost -= profit;
    }

    fn swap_two_edges(&mut self, env: &mut Environment) {
        let u0 = self.rng.gen_range(0, N);
        let v0 = if self.graph.edges[u0].is_empty() {
            return;
        } else {
            let &(v, _) = self.graph.edges[u0]
                .get(self.rng.gen_range(0, self.graph.edges[u0].len()))
                .unwrap();
            v
        };
        let u1 = env.ra[v0][self.rng.gen_range(0, env.ra[v0].len())];
        if u0 == u1 {
            return;
        }
        let v1 = if self.graph.edges[u1].is_empty() {
            return;
        } else {
            let &(v, _) = self.graph.edges[u1]
                .get(self.rng.gen_range(0, self.graph.edges[u1].len()))
                .unwrap();
            v
        };
        if v0 == v1 {
            return;
        }
        let mut profit = self.hardness[v0].max(0) - (self.hardness[v0] - env.a[u1][v0] + env.a[u0][v0]).max(0);
        profit += self.hardness[v1].max(0) - (self.hardness[v1] - env.a[u0][v1] + env.a[u1][v1]).max(0);
        if !env.scheduler.accept(profit as f64) {
            return;
        }
        if !self.graph.can_add_edge(u1, v0) {
            return;
        }
        if !self.graph.can_add_edge(u0, v1) {
            return;
        }
        self.graph.remove_edge(u0, v0);
        self.graph.remove_edge(u1, v1);
        self.graph.add_edge(u1, v0);
        self.graph.add_edge(u0, v1);
        if self.hardness[v0] > 0 && self.hardness[v0] - env.a[u1][v0] + env.a[u0][v0] <= 0 {
            self.positive_hard.pop(v0);
        }
        if self.hardness[v0] <= 0 && self.hardness[v0] - env.a[u1][v0] + env.a[u0][v0] > 0 {
            self.positive_hard.push(v0);
        }
        self.hardness[v0] -= env.a[u1][v0];
        self.hardness[v0] += env.a[u0][v0];
        if self.hardness[v1] > 0 && self.hardness[v1] - env.a[u0][v1] + env.a[u1][v1] <= 0 {
            self.positive_hard.pop(v1);
        }
        if self.hardness[v1] <= 0 && self.hardness[v1] - env.a[u0][v1] + env.a[u1][v1] > 0 {
            self.positive_hard.push(v1);
        }
        self.hardness[v1] -= env.a[u0][v1];
        self.hardness[v1] += env.a[u1][v1];
        self.cost -= profit;
        return;
    }

    fn print(&mut self, input: &Input) {
        for v in 0..N {
            if self.hardness[v] <= 0 {
                continue;
            }
            for _ in 0..self.hardness[v] {
                self.graph.add_edge(N, v);
            }
        }
        let order = self.graph.get_topological_order();
        let mut hardness = input.h.clone();
        for &u in &order {
            for &(v, cnt) in &self.graph.edges[u] {
                for _ in 0..cnt {
                    if hardness[v] <= 0 {
                        continue;
                    }
                    hardness[v] -= input.a[u][v];
                    let u_print = if u == N { -1 } else { u as i32 };
                    println!("{} {}", u_print, v);
                }
            }
        }
    }
}

#[derive(Debug, Clone)]
struct GreedyState {
    rng: XorShift32,
    attacks: Vec<(usize, usize)>, // (weapon, target)
    graph: Graph,
    hardness: [i32; N],
    durability: [i32; N + 1],
    a: [[i32; N]; N + 1],
}

impl GreedyState {
    fn new(input: &Input, actions: &Vec<Action>, seed: u32) -> Self {
        let mut attacks = vec![];
        let mut graph = Graph::new();
        let mut hardness = input.h.clone();
        let mut durability = input.c.clone();
        for action in actions {
            let v = action.target;
            for &u in &action.weapons {
                attacks.push((u, v));
                graph.add_edge(u, v);
                hardness[v] -= input.a[u][v];
                durability[u] -= 1;
            }
        }
        Self {
            rng: XorShift32::new(seed),
            attacks,
            graph,
            hardness,
            durability,
            a: input.a.clone(),
        }
    }

    fn solve(&mut self) {
        let mut priorities = Vec::with_capacity(N * (N + 1));
        for u in 0..=N {
            for v in 0..N {
                priorities.push((self.priority(u, v), self.rng.gen(), u, v));
            }
        }
        let mut heap = BinaryHeap::from(priorities);
        while let Some((pr, _, u, v)) = heap.pop() {
            let npr = self.priority(u, v);
            if pr != npr {
                heap.push((npr, self.rng.gen(), u, v));
                continue;
            }
            if self.hardness[v] <= 0 || self.durability[u] <= 0 {
                continue;
            }
            if !self.graph.can_add_edge(u, v) {
                continue;
            }
            // perform attack
            self.attacks.push((u, v));
            self.graph.add_edge(u, v);
            self.hardness[v] -= self.a[u][v];
            self.durability[u] -= 1;
            if self.hardness[v] > 0 && self.durability[u] > 0 {
                let npr = self.priority(u, v);
                heap.push((npr, self.rng.gen(), u, v));
            }
        }
    }

    fn priority(&self, u: usize, v: usize) -> i32 {
        let reduc = self.a[u][v].min(self.hardness[v]);
        reduc
    }

    fn print(&self, input: &Input) {
        let mut edges = vec![vec![]; N + 1];
        for &(u, v) in &self.attacks {
            edges[u].push(v);
        }
        let order = self.graph.get_topological_order();
        let mut hardness = input.h.clone();
        for &u in &order {
            debug_assert!(u == N || hardness[u] <= 0);
            for &v in &edges[u] {
                if hardness[v] <= 0 {
                    continue;
                }
                hardness[v] -= self.a[u][v];
                let u = if u == N { -1 } else { u as i32 };
                println!("{} {}", u, v);
            }
        }
    }
}

#[derive(Debug, Clone)]
struct Graph {
    edges: [Vec<(usize, usize)>; N + 1],
    in_degree: [usize; N + 1],
    visited: [bool; N],
}

impl Graph {
    fn new() -> Self {
        Graph {
            edges: (0..N + 1)
                .map(|_| Vec::new())
                .collect_vec()
                .try_into()
                .unwrap(),
            in_degree: [0; N + 1],
            visited: [false; N],
        }
    }

    fn add_edge(&mut self, u: usize, v: usize) {
        debug_assert!(u <= N);
        debug_assert!(v < N);
        debug_assert_ne!(u, v);
        self.in_degree[v] += 1;
        if let Some((_, cnt)) = self.edges[u].iter_mut().find(|(x, _)| *x == v) {
            *cnt += 1;
            return;
        }
        self.edges[u].push((v, 1));
    }

    fn remove_edge(&mut self, u: usize, v: usize) {
        debug_assert!(u <= N);
        debug_assert!(v < N);
        debug_assert_ne!(u, v);
        self.in_degree[v] -= 1;
        let (_, cnt) = self.edges[u].iter_mut().find(|(x, _)| *x == v).unwrap();
        *cnt -= 1;
        if *cnt == 0 {
            self.edges[u].retain(|(x, _)| *x != v);
        }
    }

    // after adding edge u->v, check if any cycles are formed
    fn can_add_edge(&mut self, u: usize, v: usize) -> bool {
        if u == v {
            return false;
        }
        if self.in_degree[u] == 0 {
            return true;
        }
        if self.edges[u].iter().find(|(x, _)| *x == v).is_some() {
            return true;
        }
        !self.can_reach(v, u)
    }

    fn can_reach(&mut self, s: usize, t: usize) -> bool {
        // DFS
        self.visited.fill(false);
        self.visited[s] = true;
        let mut stack = vec![s];
        while let Some(u) = stack.pop() {
            for &(v, _) in &self.edges[u] {
                if v == t {
                    return true;
                }
                if !self.visited[v] {
                    self.visited[v] = true;
                    stack.push(v);
                }
            }
        }
        false
    }

    fn get_topological_order(&self) -> Vec<usize> {
        let mut locks = self.in_degree.clone();
        let mut order = vec![N];
        let mut i = 0;
        while i < order.len() {
            let u = order[i];
            for &(v, cnt) in &self.edges[u] {
                locks[v] -= cnt;
                if locks[v] == 0 {
                    order.push(v);
                }
            }
            i += 1;
        }
        debug_assert_eq!(order.len(), N + 1);
        order
    }
}

#[derive(Debug, Clone)]
struct Input {
    h: [i32; N],
    c: [i32; N + 1],
    a: [[i32; N]; N + 1],
}

fn read_input() -> Input {
    input! {
        _n: usize,
        h: [i32; N],
        mut c: [i32; N],
        mut a: [[i32; N]; N],
    }
    debug_assert_eq!(_n, N);
    c.push(500 * N as i32);
    a.push(vec![1; N]);
    Input {
        h: h.try_into().unwrap(),
        c: c.try_into().unwrap(),
        a: a.into_iter()
            .map(|row| row.try_into().unwrap())
            .collect::<Vec<[i32; N]>>()
            .try_into()
            .unwrap(),
    }
}

type Cost = i32;
type Hash = u64;
type CandidateIndex = u32;

// beam search setting
// capacity = 0 is OK.
#[derive(Debug, Clone)]
struct Config {
    max_turn: usize,
    beam_width: usize,
    tour_capacity: usize,
}

// data for a state transition
// Try to minimize memory usage.
#[derive(Debug, Clone, PartialEq, Eq)]
struct Action {
    target: usize,
    weapons: Vec<usize>,
}

// data for evaluation
// Try to minimize memory usage.
#[derive(Debug, Clone)]
struct Evaluator {
    turn: i32,
    weapons: i32,
}

impl Evaluator {
    // the lower, the better
    fn evaluate(&self) -> Cost {
        TURN_W * self.turn - self.weapons
    }
}

// data for an expansion node
#[derive(Debug, Clone)]
struct Candidate {
    action: Action,
    evaluator: Evaluator,
    cost: Cost,
    hash: Hash,
    parent: usize,
}

// erasable max priority queue
#[derive(Debug, Clone)]
struct SegmentTree {
    n: usize,
    log: usize,
    size: usize,
    d: Vec<(Cost, CandidateIndex)>,
}

impl SegmentTree {
    fn new(n: usize) -> Self {
        let mut ret = Self {
            n: 0,
            log: 0,
            size: 0,
            d: Vec::new(),
        };
        ret.init(n);
        ret
    }

    fn init(&mut self, n: usize) {
        self.n = n;
        self.size = n.next_power_of_two();
        self.log = self.size.trailing_zeros() as usize;
        self.d.clear();
        self.d
            .resize(2 * self.size, (Cost::MIN, CandidateIndex::MAX));
    }

    fn build(&mut self, candidates: &Vec<Candidate>) {
        self.init(candidates.len());
        for i in 0..self.n {
            self.d[self.size + i] = (candidates[i].cost, i as CandidateIndex);
        }
        for i in (1..self.size).rev() {
            self.update(i);
        }
    }

    fn set(&mut self, p: usize, cost: Cost) {
        debug_assert!(p < self.n);
        let q = p + self.size;
        self.d[q] = (cost, p as CandidateIndex);
        for i in 1..=self.log {
            self.update(q >> i);
        }
    }

    fn top(&self) -> (Cost, CandidateIndex) {
        self.d[1]
    }

    fn update(&mut self, k: usize) {
        let (c0, p0) = self.d[2 * k];
        let (c1, p1) = self.d[2 * k + 1];
        self.d[k] = if c0 >= c1 { (c0, p0) } else { (c1, p1) };
    }
}

// select beam width candidates from better evaluation
// It remains only the best one for same hash candidates.
#[derive(Debug, Clone)]
struct Selector {
    beam_width: usize,
    candidates: Vec<Candidate>,
    hash_to_index: rustc_hash::FxHashMap<Hash, CandidateIndex>,
    segtree: SegmentTree,
    finished_candidate: Option<Candidate>,
}

impl Selector {
    fn new(beam_width: usize) -> Self {
        Self {
            beam_width,
            candidates: Vec::with_capacity(beam_width),
            hash_to_index: rustc_hash::FxHashMap::default(),
            segtree: SegmentTree::new(beam_width),
            finished_candidate: None,
        }
    }

    // add a candidate
    // finished = true iff it can get feasible solution through the candidate in the turn minimizing problem.
    // It returns true iff it provisionally accept the candidate.
    #[inline(always)]
    fn push(&mut self, candidate: Candidate, finished: bool) -> bool {
        let cost = candidate.cost;
        if finished {
            match &self.finished_candidate {
                Some(best_candidate) => {
                    if cost < best_candidate.cost {
                        self.finished_candidate = Some(candidate);
                        return true;
                    } else {
                        return false;
                    }
                }
                None => {
                    self.finished_candidate = Some(candidate);
                    return true;
                }
            }
        }
        if self.built_segtree() && cost >= self.segtree.top().0 {
            return false;
        }
        let hash = candidate.hash;
        match self.hash_to_index.get(&hash) {
            Some(&p) => {
                // contains a candidate with same hash
                let p = p as usize;
                debug_assert_eq!(self.candidates[p].hash, hash);
                if cost < self.candidates[p].cost {
                    self.candidates[p] = candidate;
                    if self.built_segtree() {
                        self.segtree.set(p, cost);
                    }
                    true
                } else {
                    false
                }
            }
            None => {
                // does not contain a candidate with same hash
                if self.built_segtree() {
                    let p = self.segtree.top().1 as usize;
                    self.hash_to_index.remove(&self.candidates[p].hash);
                    self.hash_to_index.insert(hash, p as CandidateIndex);
                    self.candidates[p] = candidate;
                    self.segtree.set(p, cost);
                } else {
                    let p = self.candidates.len();
                    self.hash_to_index.insert(hash, p as CandidateIndex);
                    self.candidates.push(candidate);
                    if self.built_segtree() {
                        // candidates become full
                        self.segtree.build(&self.candidates);
                    }
                }
                true
            }
        }
    }

    fn get_best_candidate(&self) -> Candidate {
        self.candidates
            .iter()
            .min_by_key(|candidate| candidate.cost)
            .unwrap()
            .clone()
    }

    fn clear(&mut self) {
        self.candidates.clear();
        self.hash_to_index.clear();
        self.finished_candidate = None;
    }

    fn built_segtree(&self) -> bool {
        self.candidates.len() == self.beam_width
    }
}

#[derive(Debug, Clone)]
struct State {
    rng: XorShift32,
    c_hashes: [Hash; 7],
    id_hashes: [Hash; N],
    turn: usize,
    h: [i32; N],
    c: [i32; N + 1],
    a: [[i32; N]; N + 1],
}

// data updated in depth first search
impl State {
    fn new(input: &Input) -> Self {
        let mut rng = XorShift32::new(1);
        let mut c_hashes = [0; 7];
        for i in 0..7 {
            c_hashes[i] = (rng.gen() as u64) << 32 | rng.gen() as u64;
        }
        let mut id_hashes = [0; N];
        for v in 0..N {
            id_hashes[v] = (rng.gen() as u64) << 32 | rng.gen() as u64;
        }
        Self {
            rng,
            c_hashes,
            id_hashes,
            turn: 0,
            h: input.h,
            c: input.c,
            a: input.a,
        }
    }

    // return the initial value of Evaluator and Hash
    fn get_initial_data(&self) -> (Action, Evaluator, Hash) {
        let action = Action {
            target: !0,
            weapons: vec![],
        };
        let evaluator = Evaluator {
            turn: 0,
            weapons: 0,
        };
        let hash = 0;
        (action, evaluator, hash)
    }

    // add candidates to selector
    // argument
    // - evaluator: current evaluator
    // - hash: current hash
    // - parent: current node ID (parent node ID for next state)
    fn expand(
        &mut self,
        action: &Action,
        evaluator: &Evaluator,
        mut hash: Hash,
        parent: usize,
        selector: &mut Selector,
    ) {
        let mut us = vec![];
        let mut vs = vec![];
        for v in 0..N {
            if self.h[v] <= 0 {
                if self.c[v] > 0 {
                    us.push(v);
                }
            } else {
                vs.push(v);
            }
        }
        for &v in &vs {
            let mut heap = Vec::with_capacity(us.len());
            for &u in &us {
                heap.push((self.a[u][v], self.c[u], u));
            }
            let mut heap = BinaryHeap::from(heap);
            let mut hv = self.h[v];
            let mut weapons = vec![];
            let mut nhash = hash;
            while hv > 0 && !heap.is_empty() {
                let &(_, _, u) = heap.peek().unwrap();
                weapons.push(u);
                hv -= self.a[u][v];
                nhash ^= self.weapon_hash(u, self.c[u] as usize);
                self.c[u] -= 1;
                nhash ^= self.weapon_hash(u, self.c[u] as usize);
                if self.c[u] == 0 {
                    heap.pop();
                    continue;
                }
                *heap.peek_mut().unwrap() = (self.a[u][v], self.c[u], u);
            }
            for &u in &weapons {
                self.c[u] += 1;
            }
            let weapons_len = weapons.len();
            if hv > 0 {
                for _ in 0..hv {
                    weapons.push(N);
                }
            }
            let nevaluator = Evaluator {
                turn: evaluator.turn + weapons.len() as i32,
                weapons: evaluator.weapons + self.c[v] - weapons_len as i32,
            };
            let naction = Action { target: v, weapons };
            let ncost = nevaluator.evaluate();
            let candidate = Candidate {
                action: naction,
                evaluator: nevaluator,
                cost: ncost,
                hash: nhash,
                parent,
            };
            selector.push(candidate, self.turn == BEAM_TURN - 1);
        }
    }

    fn weapon_hash(&self, v: usize, cnt: usize) -> Hash {
        self.c_hashes[cnt].wrapping_mul(self.id_hashes[v])
    }

    fn move_forward(&mut self, action: &Action) {
        self.turn += 1;
        let v = action.target;
        for &u in &action.weapons {
            self.h[v] -= self.a[u][v];
            self.c[u] -= 1;
        }
    }

    fn move_backward(&mut self, action: &Action) {
        self.turn -= 1;
        let v = action.target;
        for &u in &action.weapons {
            self.h[v] += self.a[u][v];
            self.c[u] += 1;
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
enum EdgeProperty {
    Leaf,
    Forward,
    Backward,
}

#[derive(Debug, Clone)]
struct Edge {
    property: EdgeProperty,
    action: Action,
}

#[derive(Debug, Clone)]
struct Tree {
    state: State,
    curr_tour: Vec<Edge>,
    next_tour: Vec<Edge>,
    leaves: Vec<(Evaluator, Hash)>,
    buckets: Vec<Vec<(Action, Evaluator, Hash)>>,
    direct_road: Vec<Action>,
}

impl Tree {
    fn new(state: State, config: &Config) -> Self {
        Self {
            state,
            curr_tour: Vec::with_capacity(config.tour_capacity),
            next_tour: Vec::with_capacity(config.tour_capacity),
            leaves: Vec::with_capacity(config.beam_width as usize),
            buckets: vec![vec![]; config.beam_width as usize],
            direct_road: Vec::new(),
        }
    }

    // add candidates to selector while updating state
    fn dfs(&mut self, selector: &mut Selector) {
        if self.curr_tour.is_empty() {
            // the first turn
            let (action, evaluator, hash) = self.state.get_initial_data();
            self.state.expand(&action, &evaluator, hash, 0, selector);
            return;
        }

        let mut leaf_id = 0;
        for edge in &self.curr_tour {
            match edge.property {
                EdgeProperty::Leaf => {
                    let (evaluator, hash) = &self.leaves[leaf_id];
                    self.state.move_forward(&edge.action);
                    self.state
                        .expand(&edge.action, evaluator, *hash, leaf_id, selector);
                    self.state.move_backward(&edge.action);
                    leaf_id += 1;
                }
                EdgeProperty::Forward => self.state.move_forward(&edge.action),
                EdgeProperty::Backward => self.state.move_backward(&edge.action),
            }
        }
    }

    // add new nodes and remove useless nodes
    fn update(&mut self, candidates: &Vec<Candidate>) {
        self.leaves.clear();

        if self.curr_tour.is_empty() {
            // no branch
            self.curr_tour.clear();
            for candidate in candidates {
                self.curr_tour.push(Edge {
                    property: EdgeProperty::Leaf,
                    action: candidate.action.clone(),
                });
                self.leaves
                    .push((candidate.evaluator.clone(), candidate.hash));
            }
            return;
        }

        // bucket sort
        for candidate in candidates {
            self.buckets[candidate.parent as usize].push((
                candidate.action.clone(),
                candidate.evaluator.clone(),
                candidate.hash,
            ));
        }

        let mut curr_tour_id = 0;

        // do not repeat direct road
        let is_direct_road = |curr_tour_id: usize, curr_tour: &Vec<Edge>| {
            curr_tour[curr_tour_id].property == EdgeProperty::Forward
                && curr_tour.last().unwrap().action == curr_tour[curr_tour_id].action
        };
        while is_direct_road(curr_tour_id, &self.curr_tour) {
            let action = self.curr_tour[curr_tour_id].action.clone();
            self.state.move_forward(&action);
            self.direct_road.push(action);
            self.curr_tour.pop();
            curr_tour_id += 1;
        }

        let mut leaf_id = 0;
        for edge in &self.curr_tour[curr_tour_id..] {
            match edge.property {
                EdgeProperty::Leaf => {
                    if self.buckets[leaf_id].is_empty() {
                        leaf_id += 1;
                        continue;
                    }
                    self.next_tour.push(Edge {
                        property: EdgeProperty::Forward,
                        action: edge.action.clone(),
                    });
                    for (action, evaluator, hash) in &self.buckets[leaf_id] {
                        self.next_tour.push(Edge {
                            property: EdgeProperty::Leaf,
                            action: action.clone(),
                        });
                        self.leaves.push((evaluator.clone(), *hash));
                    }
                    self.next_tour.push(Edge {
                        property: EdgeProperty::Backward,
                        action: edge.action.clone(),
                    });
                    self.buckets[leaf_id].clear();
                    leaf_id += 1;
                }
                EdgeProperty::Forward => {
                    self.next_tour.push(edge.clone());
                }
                EdgeProperty::Backward => {
                    if self.next_tour.last().unwrap().property == EdgeProperty::Forward {
                        self.next_tour.pop();
                    } else {
                        self.next_tour.push(edge.clone());
                    }
                }
            }
        }
        std::mem::swap(&mut self.curr_tour, &mut self.next_tour);
        self.next_tour.clear();
    }

    // get the path from the root
    fn get_path(&self, best_leaf_id: usize, turn: usize) -> Vec<Action> {
        // eprintln!("curr_tour.len() = {}", self.curr_tour.capacity());

        let mut ret = self.direct_road.clone();
        ret.reserve(turn);
        let mut leaf_id = 0;
        for edge in &self.curr_tour {
            match edge.property {
                EdgeProperty::Leaf => {
                    if leaf_id == best_leaf_id {
                        ret.push(edge.action.clone());
                        return ret;
                    }
                    leaf_id += 1;
                }
                EdgeProperty::Forward => ret.push(edge.action.clone()),
                EdgeProperty::Backward => {
                    ret.pop();
                }
            }
        }

        panic!("invalid argument: best_leaf_id");
    }
}

fn beam_search(config: &Config, state: State) -> Option<Vec<Action>> {
    let mut tree = Tree::new(state, config);
    let mut selector = Selector::new(config.beam_width);

    for turn in 0..config.max_turn {
        tree.dfs(&mut selector);

        if let Some(candidate) = &selector.finished_candidate {
            // find the feasible solution in turn minimizing problem
            let mut ret = tree.get_path(candidate.parent as usize, turn + 1);
            ret.push(candidate.action.clone());
            return Some(ret);
        }

        if selector.candidates.is_empty() {
            // cannot find the feasible solution
            return None;
        }

        if turn == config.max_turn - 1 {
            // the last turn
            let best_candidate = selector.get_best_candidate();
            let mut ret = tree.get_path(best_candidate.parent as usize, turn + 1);
            ret.push(best_candidate.action);
            return Some(ret);
        }

        tree.update(&selector.candidates);
        selector.clear();
    }

    unreachable!();
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

#[derive(Debug, Clone)]
pub struct IndexSet {
    n: usize,
    data: Vec<usize>,
    pos: Vec<usize>,
}

impl IndexSet {
    pub fn new(n: usize) -> Self {
        IndexSet {
            n,
            data: Vec::with_capacity(n),
            pos: vec![usize::MAX; n],
        }
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn push(&mut self, x: usize) -> bool {
        debug_assert!(x < self.n);
        if self.pos[x] == usize::MAX {
            self.pos[x] = self.data.len();
            self.data.push(x);
            true
        } else {
            false
        }
    }

    pub fn pop(&mut self, x: usize) -> bool {
        debug_assert!(x < self.n);
        let i = self.pos[x];
        if i == usize::MAX {
            return false;
        }
        let y = *self.data.last().unwrap();
        self.data[i] = y;
        self.data.pop();
        self.pos[y] = i;
        self.pos[x] = usize::MAX;
        true
    }

    // pop(x) and push(y)
    pub fn replace(&mut self, x: usize, y: usize) {
        debug_assert!(x < self.n);
        debug_assert!(y < self.n);
        debug_assert!(self.contains(x));
        debug_assert!(!self.contains(y));
        if x == y {
            return;
        }
        let i = self.pos[x];
        self.data[i] = y;
        self.pos[x] = usize::MAX;
        self.pos[y] = i;
    }

    pub fn contains(&self, x: usize) -> bool {
        debug_assert!(x < self.n);
        self.pos[x] != usize::MAX
    }

    pub fn clear(&mut self) {
        for &x in &self.data {
            self.pos[x] = usize::MAX;
        }
        self.data.clear();
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
