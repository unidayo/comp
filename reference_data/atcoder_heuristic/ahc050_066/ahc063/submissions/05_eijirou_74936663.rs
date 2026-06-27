#[cfg(target_arch = "x86_64")]
use std::arch::x86_64::*;

use std::{
    collections::{HashMap, HashSet, VecDeque},
    hash::{BuildHasherDefault, Hasher},
    process::exit,
};

use fixedbitset::FixedBitSet;
use proconio::input;

const TIME_LIMIT_SEC: f64 = 1.95;
const DXDY: [(usize, usize); 4] = [(!0, 0), (1, 0), (0, !0), (0, 1)];
const UDLR: [char; 4] = ['U', 'D', 'L', 'R'];
const UP: usize = 0;
const DOWN: usize = 1;
const LEFT: usize = 2;
const RIGHT: usize = 3;

type BitSet256 = StaticBitSet<256, 4>;

const TIME_BUF: f64 = 1.25;
const WIDTH_UP: f64 = 1.5;
const MAX_BEAM_TIME_SEC: f64 = 0.4;
const TSP_THRESHOLD: usize = 8;

fn main() {
    get_time_sec();

    let input = read_input();

    let beam_width = first_beam_width(&input);
    let mut config = Config {
        max_turn: 2000,
        beam_width,
        nodes_capacity: 131072,
        seed: 123456789,
    };

    let start_time = get_time_sec();
    let mut best = solve_by_beam_search(&config, &input).unwrap();
    let mut elapsed = get_time_sec() - start_time;

    while get_time_sec() < TIME_LIMIT_SEC {
        config.max_turn.chmin(best.len());
        config.seed += 1;
        let start_time = get_time_sec();
        if start_time + TIME_BUF * elapsed >= TIME_LIMIT_SEC {
            let r = (TIME_LIMIT_SEC - start_time) / (TIME_BUF * elapsed);
            config.beam_width = ((config.beam_width as f64) * r.max(0.25)) as usize;
        } else if elapsed < MAX_BEAM_TIME_SEC
            && start_time + TIME_BUF * elapsed * WIDTH_UP < TIME_LIMIT_SEC
        {
            config.beam_width = ((config.beam_width as f64) * WIDTH_UP) as usize;
        }
        let directions = solve_by_beam_search(&config, &input);
        elapsed = get_time_sec() - start_time;
        if directions.is_none() {
            continue;
        }
        let directions = directions.unwrap();
        if directions.len() < best.len() {
            best = directions;
        }
    }

    println!(
        "{}",
        best.into_iter()
            .map(|dir| UDLR[dir].to_string())
            .collect::<Vec<_>>()
            .join("\n")
    );

    exit(0);
}

fn solve_by_beam_search(config: &Config, input: &Input) -> Option<Vec<usize>> {
    let state = State::new(&input);

    let actions = beam_search(&config, state)?;

    let mut state = State::new(&input);
    let mut directions = Vec::with_capacity(actions.len());
    for action in actions {
        state.move_forward_internal(&action, Some(&mut directions));
    }
    state.push_last_picks(&mut directions);

    Some(directions)
}

fn first_beam_width(input: &Input) -> usize {
    let n = input.n;
    let m = input.num_balls;
    1000000 / (n * m)
}

type Cost = i32;
type Hash = u32;
type NodeIndex = u32;
type TurnIndex = u16;
type TspCost = u16;

// beam search setting
// capacity = 0 is OK.
#[derive(Debug, Clone)]
struct Config {
    max_turn: usize,
    beam_width: usize,
    nodes_capacity: usize,
    seed: u32,
}

// data for a state transition
// Try to minimize memory usage.
#[derive(Debug, Clone, Default)]
struct Action {
    dir: u8, // direction of movement, 0: up, 1: down, 2: left, 3: right
    num_rotates: u8,
    sep_len: u8, // the number of balls to be separated by biting off (excluding rotating)
}

impl Action {
    fn new(dir: usize) -> Self {
        Self {
            dir: dir as u8,
            num_rotates: 0,
            sep_len: 0,
        }
    }

    fn get_dir(&self) -> usize {
        self.dir as usize
    }

    fn get_num_rotates(&self) -> usize {
        self.num_rotates as usize
    }

    fn set_num_rotates(&mut self, num_rotates: usize) {
        self.num_rotates = num_rotates as u8;
    }

    fn set_sep_len(&mut self, sep_len: usize) {
        self.sep_len = sep_len as u8;
    }
}

// data for evaluation
// Try to minimize memory usage.
#[derive(Debug, Clone, Default)]
struct Evaluator {
    goal_id: u8,      // the index of goals
    progress: u8,     // the number of balls in the correct position
    aligns: i8,       // the number of cells that the head pass through the correct edge
    lower_bound: u16, // the lower bound of the number of turns to finish
    is_clean: bool,
}

impl Evaluator {
    // the lower, the better
    fn evaluate(&self, tsp_coeff: Cost) -> Cost {
        let mut cost = -128 * 128 * self.goal_id as Cost;
        cost -= 128 * self.progress as Cost;
        if !self.is_clean {
            cost += 128;
        }
        cost -= self.aligns as Cost;
        cost += tsp_coeff
            * if self.lower_bound == 0 {
                8 * TSP_THRESHOLD as Cost
            } else {
                self.lower_bound as Cost
            };
        cost
    }

    fn get_goal_id(&self) -> usize {
        self.goal_id as usize
    }

    fn get_progress(&self) -> usize {
        self.progress as usize
    }
}

// data for an expansion node
#[derive(Debug, Clone)]
struct Candidate {
    action: Action,
    evaluator: Evaluator,
    cost: Cost,
    hash: Hash,
    parent: NodeIndex,
}

// select beam width candidates from better evaluation
// It remains only the best one for same hash candidates.
#[derive(Debug, Clone)]
struct Selector {
    n: usize,
    beam_width: usize,
    candidates: Vec<Candidate>,
    hash_to_index: NoHashMap<Hash, u32>,
    finished_candidate: Option<Candidate>,
}

impl Selector {
    fn new(beam_width: usize, n: usize) -> Self {
        Self {
            n,
            beam_width,
            candidates: Vec::with_capacity(3 * beam_width),
            hash_to_index: NoHashMap::default(),
            finished_candidate: None,
        }
    }

    // add a candidate
    // finished = true iff it can get feasible solution through the candidate in the turn minimizing problem.
    // It returns true iff it provisionally accept the candidate.
    fn push(&mut self, candidate: Candidate, finished: bool) -> bool {
        if finished {
            self.finished_candidate = Some(candidate);
            return true;
        }
        if let Some(&i) = self.hash_to_index.get(&candidate.hash) {
            let i = i as usize;
            if self.candidates[i].cost <= candidate.cost {
                return false;
            }
            self.candidates[i] = candidate;
            return true;
        }
        self.hash_to_index
            .insert(candidate.hash, self.candidates.len() as u32);
        self.candidates.push(candidate);
        true
    }

    fn select(&mut self, rng: &mut XorShift32) -> Vec<Candidate> {
        rng.shuffle(&mut self.candidates);
        self.candidates.sort_unstable_by_key(|candidate| candidate.cost);

        const CANDS_PER_CELL_DEN: usize = 2;
        let cands_per_cell = (self.beam_width / (self.n * self.n / 2) / CANDS_PER_CELL_DEN).max(1);

        let mut res = Vec::with_capacity(self.beam_width);
        let mut count_by_cell = vec![0; self.n * self.n];
        for candidate in &self.candidates {
            let cell = (candidate.hash >> 24) as usize;
            if count_by_cell[cell] < cands_per_cell || res.len() < self.beam_width {
                res.push(candidate.clone());
                count_by_cell[cell] += 1;
            }
        }
        if res.len() > self.beam_width {
            for i in (0..res.len()).rev() {
                let cell = (res[i].hash >> 24) as usize;
                if count_by_cell[cell] > cands_per_cell {
                    res.swap_remove(i);
                    count_by_cell[cell] -= 1;
                    if res.len() <= self.beam_width {
                        break;
                    }
                }
            }
        }
        res
    }

    fn clear(&mut self) {
        self.candidates.clear();
        self.hash_to_index.clear();
        self.finished_candidate = None;
    }

    // fn get_best_candidate(&self) -> Candidate {
    //     self.candidates
    //         .iter()
    //         .min_by_key(|candidate| candidate.cost)
    //         .unwrap()
    //         .clone()
    // }
}

#[derive(Debug, Clone)]
struct HashBitSet<const N: u32> {
    bits: FixedBitSet,
    que: VecDeque<u32>,
}

impl<const N: u32> HashBitSet<N> {
    fn new() -> Self {
        Self {
            bits: FixedBitSet::with_capacity(N as usize),
            que: VecDeque::with_capacity((N >> 5) as usize),
        }
    }

    fn insert(&mut self, hash: Hash) -> bool {
        let i = splitmix32(hash) % N;
        let exists = self.bits.contains(i as usize);
        if !exists {
            self.bits.insert(i as usize);
            self.que.push_back(i);
            if self.que.len() >= (N >> 5) as usize {
                let j = self.que.pop_front().unwrap();
                self.bits.set(j as usize, false);
            }
        }
        !exists
    }
}

const HASH_BITSET_SIZE: u32 = 1 << 24;
const VISITED_HASH_PENALTY: Cost = 1;

#[derive(Debug, Clone)]
struct MultiSelectors {
    beam_width: usize,
    n: usize,
    selectors: std::collections::VecDeque<Selector>,
    step_max: usize,
    hashes: HashBitSet<HASH_BITSET_SIZE>,
    nhashes: HashBitSet<HASH_BITSET_SIZE>,
}

impl MultiSelectors {
    fn new(beam_width: usize, n: usize) -> Self {
        Self {
            beam_width,
            n,
            selectors: std::collections::VecDeque::new(),
            step_max: 0,
            hashes: HashBitSet::new(),
            nhashes: HashBitSet::new(),
        }
    }

    #[inline(always)]
    fn push_candidate(&mut self, candidate: Candidate, finished: bool, step: usize) -> bool {
        debug_assert!(step >= 1);
        while self.selectors.len() < step {
            self.selectors
                .push_back(Selector::new(self.beam_width, self.n));
        }
        if self.selectors[step - 1].push(candidate, finished) {
            self.step_max.chmax(step);
            true
        } else {
            false
        }
    }

    fn reset_step_max(&mut self) {
        self.step_max = 1;
    }

    fn pop_front(&mut self) -> Selector {
        debug_assert!(!self.selectors.is_empty());
        let mut selector = self.selectors.pop_front().unwrap();
        for candidate in &mut selector.candidates {
            if !self.hashes.insert(candidate.hash) {
                candidate.cost += VISITED_HASH_PENALTY;
            }
        }
        std::mem::swap(&mut self.hashes, &mut self.nhashes);
        selector
    }

    fn push_back(&mut self, mut selector: Selector) {
        selector.clear();
        self.selectors.push_back(selector);
    }
}

const MASK24: Hash = (1 << 24) - 1;

#[derive(Debug, Clone)]
struct State {
    n: usize,               // grid size
    goals: Vec<Vec<usize>>, // color sequence
    cell_hashes: Vec<u32>,  // random hashes for cells
    cell_mask: BitSet256,
    tsp_solver: TSPSolver,
    tsp_coeff: Cost,
    cells: Vec<usize>,                   // ball color in each cell, !0 if empty
    in_cell: BitSet256,                  // whether each cell has a ball
    snake_color: Vec<usize>,             // colors of balls in the snake
    snake_pos: VecDeque<(usize, usize)>, // positions of balls in the snake
    in_snake: BitSet256,                 // whether each cell is in the snake except the tail
    tail_popped_pos: Vec<(u8, u8)>,      // for move backward
    sep_popped_pos: Vec<(u8, u8)>,       // for move backward
}

// data updated in depth first search
impl State {
    fn new(input: &Input) -> Self {
        let n = input.n;
        let num_balls = input.num_balls;
        let num_colors = input.num_colors;
        let goals = Self::split_goal(n, num_colors, &input.goal);

        let mut rng = XorShift32::new(123456789);
        let mut cell_hashes = vec![0; n * n];
        for i in 0..n * n {
            cell_hashes[i] = rng.gen_u32() & MASK24;
        }
        let mut cell_mask = BitSet256::new();
        for x in 0..n - (goals.len() - 1) {
            for y in 0..n - (goals.len() - 1) {
                cell_mask.set_by_xy(x, y, true);
            }
        }
        let tsp_coeff = Self::tsp_coeff(num_colors);
        let cells = input.cells.clone();
        let mut in_cell = BitSet256::new();
        for x in 0..n {
            for y in 0..n {
                if cells[x * n + y] != !0 {
                    in_cell.set_by_xy(x, y, true);
                }
            }
        }
        let mut snake_color = vec![0; 5];
        snake_color.reserve(num_balls - 5);
        let mut snake_pos = VecDeque::with_capacity(num_balls);
        for x in 0..5 {
            snake_pos.push_front((x, 0));
        }
        let mut in_snake = BitSet256::new();
        for i in 0..4 {
            let (x, y) = snake_pos[i];
            in_snake.set_by_xy(x, y, true);
        }
        let mut state = Self {
            n,
            goals,
            cell_hashes,
            cell_mask,
            tsp_solver: TSPSolver::new(!0, !0),
            tsp_coeff,
            cells,
            in_cell,
            snake_color,
            snake_pos,
            in_snake,
            tail_popped_pos: vec![],
            sep_popped_pos: vec![],
        };
        let (tx, ty) = state.end_pos();
        state.tsp_solver.tx = tx;
        state.tsp_solver.ty = ty;
        state
    }

    fn split_goal(mut n: usize, num_colors: usize, goal: &[usize]) -> Vec<Vec<usize>> {
        let mut goals = vec![];
        let mut m = goal.len();
        while !Self::pick_from_head(n, m, num_colors) {
            let l = m - (2 * n - 1);
            let mut colors = vec![0; 5];
            if goals.len() % 2 == 0 {
                colors.extend(goal[l..m].iter().cloned());
            } else {
                colors.extend(goal[l..m].iter().rev().cloned());
            }
            goals.push(colors);
            m = l;
            n -= 1;
        }
        goals.push(goal[..m].to_vec());
        goals
    }

    fn pick_from_head(n: usize, m: usize, c: usize) -> bool {
        const COEFFS: [f64; 5] = [12.0, 9.0, 8.5, 8.0, 7.5];
        (m as f64) < COEFFS[c - 3] * (n as f64)
    }

    fn tsp_coeff(c: usize) -> Cost {
        const COEFFS: [Cost; 5] = [64, 48, 32, 28, 24];
        COEFFS[c - 3]
    }

    fn end_pos(&self) -> (usize, usize) {
        if self.goals.len() == 1 {
            (!0, !0)
        } else if self.goals.len() % 2 == 0 {
            (0, self.n - self.goals.len())
        } else {
            (self.n - self.goals.len(), 0)
        }
    }

    fn push_last_picks(&self, directions: &mut Vec<usize>) {
        for i in (1..self.goals.len()).rev() {
            if i % 2 == 0 {
                directions.push(DOWN);
                for _ in 0..self.n - i {
                    directions.push(RIGHT);
                }
                for _ in 0..self.n - i {
                    directions.push(UP);
                }
            } else {
                directions.push(RIGHT);
                for _ in 0..self.n - i {
                    directions.push(DOWN);
                }
                for _ in 0..self.n - i {
                    directions.push(LEFT);
                }
            }
        }
    }

    // return the initial value of Action, Evaluator and Hash
    fn get_initial_data(&self) -> (Action, Evaluator, Hash) {
        let action = Action::new(1); // down
        let evaluator = Evaluator {
            goal_id: 0,
            progress: 5,
            aligns: 0,
            lower_bound: 0,
            is_clean: true,
        };
        let hash = self.calc_hash();
        (action, evaluator, hash)
    }

    // for debug
    fn calc_hash(&self) -> Hash {
        let (hx, hy) = self.snake_pos[0];
        let mut hash = ((hx * self.n + hy) as Hash) << 24;
        for p in 0..self.n * self.n {
            if self.cells[p] != !0 {
                hash ^= self.cell_hashes[p];
            }
        }
        hash
    }

    // add candidates to selector
    // argument
    // - evaluator: current evaluator
    // - hash: current hash
    // - parent: current node ID (parent node ID for next state)
    fn expand(
        &mut self,
        _action: &Action,
        evaluator: &Evaluator,
        hash: Hash,
        parent: NodeIndex,
        selector: &mut MultiSelectors,
    ) {
        let (hx, hy) = self.snake_pos[0];
        let all_collected = evaluator.get_progress() >= self.goals[evaluator.get_goal_id()].len();
        let target_color = if !all_collected && evaluator.is_clean {
            self.goals[evaluator.get_goal_id()][evaluator.get_progress()]
        } else {
            !0
        };
        for dir in 0..4 {
            let (dx, dy) = DXDY[dir];
            let nhx = hx.wrapping_add(dx);
            let nhy = hy.wrapping_add(dy);
            if nhx >= self.n - evaluator.get_goal_id()
                || nhy >= self.n - evaluator.get_goal_id()
                || self.snake_pos[1] == (nhx, nhy)
            {
                continue;
            }
            let mut nhash = ((nhx * self.n + nhy) as Hash) << 24 | (hash & MASK24);
            let mut naction = Action::new(dir);
            let mut nevaluator = evaluator.clone();
            self.update_aligns(&mut nevaluator, hx, hy, dir);
            let color = self.cells[nhx * self.n + nhy];

            if self.in_snake.get_by_xy(nhx, nhy) {
                // bite off
                let i = self
                    .snake_pos
                    .iter()
                    .position(|&p| p == (nhx, nhy))
                    .unwrap()
                    + 2;
                let j = if i <= nevaluator.get_progress() {
                    // eat the ball in the correct position
                    if i < evaluator.get_progress() {
                        naction.set_num_rotates(nevaluator.get_progress() - i);
                        nevaluator.aligns = 0;
                        let (nhx, nhy) = self.snake_pos[nevaluator.get_progress() - 2];
                        nhash = ((nhx * self.n + nhy) as Hash) << 24 | (nhash & MASK24);
                    }
                    nevaluator.is_clean = true;
                    nevaluator.get_progress()
                } else {
                    i
                };
                naction.set_sep_len(self.snake_color.len() - j);
                let step = 1 + naction.get_num_rotates();
                for k in j - 1..self.snake_pos.len() - 1 {
                    let (x, y) = self.snake_pos[k];
                    nhash ^= self.cell_hashes[x * self.n + y];
                }
                if self.whether_update_lower_bound(&nevaluator) {
                    let mut nin_cell = self.in_cell.clone();
                    for k in j - 1..self.snake_pos.len() - 1 {
                        let (x, y) = self.snake_pos[k];
                        nin_cell.set_by_xy(x, y, false);
                    }
                    let (nhx, nhy) = self.snake_pos[nevaluator.get_progress() - 2];
                    self.update_lower_bound(&mut nevaluator, nin_cell, nhx, nhy);
                }
                let cost = nevaluator.evaluate(self.tsp_coeff);
                selector.push_candidate(
                    Candidate {
                        action: naction,
                        evaluator: nevaluator,
                        cost: cost,
                        hash: nhash,
                        parent,
                    },
                    false,
                    step,
                );
            } else if color == !0 {
                // move without eating
                let finished = nevaluator.get_goal_id() == self.goals.len() - 1
                    && nevaluator.get_progress() == self.goals[nevaluator.get_goal_id()].len()
                    && (self.goals.len() == 1 || (nhx, nhy) == self.end_pos());
                if self.whether_update_lower_bound(&nevaluator) {
                    self.update_lower_bound(&mut nevaluator, self.in_cell.clone(), nhx, nhy);
                }
                let cost = nevaluator.evaluate(self.tsp_coeff);
                selector.push_candidate(
                    Candidate {
                        action: naction,
                        evaluator: nevaluator,
                        cost: cost,
                        hash: nhash,
                        parent,
                    },
                    finished,
                    1,
                );
            } else {
                // eat the ball
                nhash ^= self.cell_hashes[nhx * self.n + nhy];
                if !all_collected && nevaluator.is_clean {
                    if color == target_color {
                        nevaluator.progress += 1;
                    } else {
                        nevaluator.is_clean = false;
                    }
                }
                let finished = nevaluator.get_goal_id() == self.goals.len() - 1
                    && nevaluator.get_progress() == self.goals[nevaluator.get_goal_id()].len()
                    && (self.goals.len() == 1 || (nhx, nhy) == self.end_pos());
                if self.whether_update_lower_bound(&nevaluator) {
                    let mut nin_cell = self.in_cell.clone();
                    nin_cell.set_by_xy(nhx, nhy, false);
                    self.update_lower_bound(&mut nevaluator, nin_cell, nhx, nhy);
                }
                let cost = nevaluator.evaluate(self.tsp_coeff);
                selector.push_candidate(
                    Candidate {
                        action: naction,
                        evaluator: nevaluator,
                        cost: cost,
                        hash: nhash,
                        parent,
                    },
                    finished,
                    1,
                );
            }
        }
    }

    fn update_aligns(&self, evaluator: &mut Evaluator, hx: usize, hy: usize, dir: usize) {
        if evaluator.get_goal_id() >= self.goals.len() - 1 {
            // pick from the head
            return;
        }
        let n = self.n - evaluator.get_goal_id();
        let (dx, dy) = DXDY[dir];
        let nhx = hx.wrapping_add(dx);
        let nhy = hy.wrapping_add(dy);
        if nhx == n - 2 && nhy == 0 && dir != UP {
            // entry point
            evaluator.aligns += 1;
            return;
        }
        if evaluator.aligns <= 0 {
            // not start yet
            evaluator.aligns += 1;
            evaluator.aligns.chmin(0);
            if hx == n - 2 && hy == 0 {
                evaluator.aligns.chmin(-((2 * n - 3).max(0) as i8));
            } else if hx == n - 1 {
                evaluator.aligns.chmin(-((2 * n - 4 - hy).max(0) as i8));
            }
            return;
        }
        if hx == n - 2 && hy == 0 {
            if dir == DOWN {
                evaluator.aligns += 1;
            } else {
                evaluator.aligns = -((2 * n - 3).max(0) as i8);
            }
        } else if hx == n - 1 {
            // bottom row
            if hy < n - 1 {
                if dir == RIGHT {
                    evaluator.aligns += 1;
                } else {
                    evaluator.aligns = -((2 * n - 4 - hy).max(0) as i8);
                }
            } else {
                debug_assert_eq!(hy, n - 1);
                if dir == UP {
                    evaluator.aligns += 1;
                } else {
                    evaluator.aligns = 0;
                }
            }
        } else if hy == n - 1 {
            // right column
            if hx > 0 {
                if dir == UP {
                    evaluator.aligns += 1;
                } else {
                    evaluator.aligns = 0;
                }
            } else {
                debug_assert_eq!(hx, 0);
                if dir == LEFT {
                    evaluator.aligns += 1;
                } else {
                    evaluator.aligns = 0;
                }
            }
        } else if hx == 0 && hy == n - 2 {
            if dir == LEFT || dir == DOWN {
                evaluator.aligns += 1;
            } else {
                evaluator.aligns = 0;
            }
        } else if hx == 0 && hy == n - 3 {
            if dir == DOWN {
                evaluator.aligns += 1;
            } else if dir == RIGHT {
                if evaluator.get_progress() == self.goals[evaluator.get_goal_id()].len() {
                    evaluator.goal_id += 1;
                    evaluator.progress = 5;
                    evaluator.aligns = 0;
                } else {
                    evaluator.aligns = 0;
                }
            } else {
                evaluator.aligns = 0;
            }
        } else if hx == 1 && hy == n - 3 {
            if dir == RIGHT || dir == UP {
                evaluator.aligns += 1;
            } else {
                evaluator.aligns = 0;
            }
        } else if hx == 1 && hy == n - 2 {
            if dir == UP {
                if evaluator.get_progress() == self.goals[evaluator.get_goal_id()].len() {
                    evaluator.goal_id += 1;
                    evaluator.progress = 5;
                    evaluator.aligns = 0;
                } else {
                    evaluator.aligns = 0;
                }
            } else if dir == LEFT {
                evaluator.aligns += 1;
            } else {
                evaluator.aligns = 0;
            }
        }
    }

    fn whether_update_lower_bound(&self, evaluator: &Evaluator) -> bool {
        if evaluator.get_goal_id() < self.goals.len() - 1 {
            debug_assert_eq!(evaluator.lower_bound, 0);
            return false;
        } else if evaluator.get_goal_id() < self.goals.len()
            && self.goals[evaluator.get_goal_id()].len() - evaluator.get_progress() > TSP_THRESHOLD
        {
            return false;
        }
        true
    }

    fn update_lower_bound(
        &mut self,
        evaluator: &mut Evaluator,
        mut nin_cell: BitSet256,
        nhx: usize,
        nhy: usize,
    ) {
        nin_cell &= self.cell_mask.clone();
        evaluator.lower_bound = self.tsp_solver.solve(nhx, nhy, nin_cell) as u16;
    }

    fn move_forward(&mut self, action: &Action) {
        self.move_forward_internal(action, None);
    }

    #[inline(always)]
    fn move_forward_internal(&mut self, action: &Action, mut directions: Option<&mut Vec<usize>>) {
        let (hx, hy) = self.snake_pos[0];
        let (dx, dy) = DXDY[action.get_dir()];
        let nhx = hx.wrapping_add(dx);
        let nhy = hy.wrapping_add(dy);
        debug_assert!(nhx < self.n && nhy < self.n);
        self.snake_pos.push_front((nhx, nhy));
        if let Some(dirs) = directions.as_deref_mut() {
            dirs.push(action.get_dir());
        }
        // shift the snake and update cells
        let color = self.cells[nhx * self.n + nhy];
        if color != !0 {
            // eat the ball
            self.cells[nhx * self.n + nhy] = !0;
            self.in_cell.set_by_xy(nhx, nhy, false);
            self.snake_color.push(color);
            self.tail_popped_pos.push((!0, !0));
        } else {
            // move without eating
            let (x, y) = self.snake_pos.pop_back().unwrap();
            self.tail_popped_pos.push((x as u8, y as u8));
            let (tx, ty) = *self.snake_pos.back().unwrap();
            self.in_snake.set_by_xy(tx, ty, false);
        }
        // update cells for biting off
        if action.sep_len > 0 {
            let (mut x, mut y) = *self.snake_pos.back().unwrap();
            for _ in 0..action.sep_len {
                self.snake_pos.pop_back();
                let color = self.snake_color.pop().unwrap();
                let pos = x * self.n + y;
                self.cells[pos] = color;
                self.in_cell.set_by_xy(x, y, true);
                self.sep_popped_pos.push((x as u8, y as u8));
                (x, y) = *self.snake_pos.back().unwrap();
                self.in_snake.set_by_xy(x, y, false);
            }
        }
        // rotate the snake if needed
        if action.num_rotates != 0 {
            let (tx, ty) = *self.snake_pos.back().unwrap();
            self.in_snake.set_by_xy(tx, ty, true);
            let num_rotates = action.get_num_rotates();
            let snake_len = self.snake_pos.len();
            for i in (snake_len - num_rotates..snake_len + num_rotates).step_by(2) {
                if let Some(dirs) = directions.as_deref_mut() {
                    dirs.push(get_direction(self.snake_pos[0], self.snake_pos[i]));
                }
                self.snake_pos.push_front(self.snake_pos[i]);
            }
            self.snake_pos.drain(self.snake_pos.len() - num_rotates..);
            let (tx, ty) = *self.snake_pos.back().unwrap();
            self.in_snake.set_by_xy(tx, ty, false);
        }
        self.in_snake.set_by_xy(nhx, nhy, true);
    }

    fn move_backward(&mut self, action: &Action) {
        let (nhx, nhy) = self.snake_pos[action.get_num_rotates()];
        self.in_snake.set_by_xy(nhx, nhy, false);

        // rotate the snake if needed
        if action.num_rotates != 0 {
            let (tx, ty) = *self.snake_pos.back().unwrap();
            self.in_snake.set_by_xy(tx, ty, true);
            for i in (0..action.get_num_rotates()).rev() {
                self.snake_pos.push_back(self.snake_pos[i]);
            }
            self.snake_pos.drain(0..action.get_num_rotates());
            let (tx, ty) = *self.snake_pos.back().unwrap();
            self.in_snake.set_by_xy(tx, ty, false);
        }
        self.snake_pos.pop_front().unwrap();
        let (dx, dy) = DXDY[action.get_dir()];
        let hx = nhx.wrapping_sub(dx);
        let hy = nhy.wrapping_sub(dy);
        debug_assert!(hx < self.n && hy < self.n);

        // update cells for biting off
        if action.sep_len > 0 {
            let (mut x, mut y) = *self.snake_pos.back().unwrap();
            for _ in 0..action.sep_len {
                self.in_snake.set_by_xy(x, y, true);
                let (tx, ty) = self.sep_popped_pos.pop().unwrap();
                x = tx as usize;
                y = ty as usize;
                let pos = x * self.n + y;
                let color = self.cells[pos];
                self.cells[pos] = !0;
                self.in_cell.set_by_xy(x, y, false);
                self.snake_pos.push_back((x, y));
                self.snake_color.push(color);
            }
        }
        // shift the snake and update cells
        let pop_pos = self.tail_popped_pos.pop().unwrap();
        if pop_pos == (!0, !0) {
            // eat the ball
            self.cells[nhx * self.n + nhy] = self.snake_color.pop().unwrap();
            self.in_cell.set_by_xy(nhx, nhy, true);
        } else {
            // move without eating
            let (tx, ty) = *self.snake_pos.back().unwrap();
            self.in_snake.set_by_xy(tx, ty, true);
            let (x, y) = pop_pos;
            self.snake_pos.push_back((x as usize, y as usize));
        }
    }
}

#[derive(Debug, Clone)]
struct TSPSolver {
    costs: Vec<TspCost>,
    dp: Vec<TspCost>,
    tx: usize,
    ty: usize,
    results: NoHashMap<u64, TspCost>,
}

impl TSPSolver {
    fn new(tx: usize, ty: usize) -> Self {
        Self {
            costs: vec![],
            dp: vec![],
            tx,
            ty,
            results: NoHashMap::default(),
        }
    }

    fn solve(&mut self, sx: usize, sy: usize, in_cell: BitSet256) -> TspCost {
        const INF_COST: TspCost = TspCost::MAX / 2;
        let m = in_cell.count() as usize;

        if m <= 3 {
            // Try all permutations (includes start position, so not cached).
            let mut points = in_cell.get_ones_div();
            let mut res = INF_COST;
            loop {
                let mut cost = 0;
                let mut ux = sx;
                let mut uy = sy;
                for &(vx, vy) in &points {
                    cost += (ux.abs_diff(vx) + uy.abs_diff(vy)) as TspCost;
                    ux = vx;
                    uy = vy;
                }
                if self.tx != !0 {
                    cost += (ux.abs_diff(self.tx) + uy.abs_diff(self.ty)) as TspCost;
                }
                res.chmin(cost);

                if !next_permutation(&mut points) {
                    break;
                }
            }
            return res;
        }
        let hash = in_cell.splitmix();

        // Cache hit: must be m > 3 (m <= 3 is not cached).
        // Add nearest_dist to tighten the lower bound with the start position.
        if let Some(&dp_res) = self.results.get(&hash) {
            let nearest_dist = in_cell.nearest_dist(sx, sy);
            return dp_res + nearest_dist as TspCost;
        }
        // Nearest ball distance from (sx, sy): lower bound on start contribution.
        // The cache stores dp_result without this, so it can be reused across states.
        let points = in_cell.get_ones_div();
        let nearest_dist = points
            .iter()
            .map(|&(px, py)| (px.abs_diff(sx) + py.abs_diff(sy)) as TspCost)
            .min()
            .unwrap();
        self.costs.resize(m * m, 0);
        for u in 0..m {
            self.costs[u * m + u] = 0;
            let (ux, uy) = points[u];
            for v in u + 1..m {
                let (vx, vy) = points[v];
                let cost = ux.abs_diff(vx) + uy.abs_diff(vy);
                self.costs[u * m + v] = cost as TspCost;
                self.costs[v * m + u] = cost as TspCost;
            }
        }
        self.dp.clear();
        self.dp.resize((1 << m) * m, INF_COST);
        for v in 0..m {
            self.dp[(1 << v) * m + v] = 0;
        }
        for mask in 1..(1 << m) {
            for u in 0..m {
                if mask & (1 << u) == 0 {
                    continue;
                }
                let cost = self.dp[mask * m + u];
                for v in 0..m {
                    if mask & (1 << v) != 0 {
                        continue;
                    }
                    let nmask = mask | (1 << v);
                    let ncost = cost + self.costs[u * m + v];
                    self.dp[nmask * m + v].chmin(ncost);
                }
            }
        }
        let mut res = INF_COST;
        for v in 0..m {
            let mut cost = self.dp[((1 << m) - 1) * m + v];
            if self.tx != !0 {
                let (vx, vy) = points[v];
                cost += (vx.abs_diff(self.tx) + vy.abs_diff(self.ty)) as TspCost;
            }
            res.chmin(cost);
        }
        self.results.insert(hash, res);
        res + nearest_dist
    }
}

#[derive(Debug, Clone)]
pub struct ObjectPool<T> {
    data: Vec<T>,
    garbage: Vec<usize>,
}

impl<T: Default> ObjectPool<T> {
    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            data: Vec::with_capacity(capacity),
            garbage: Vec::new(),
        }
    }

    // push `item`, then return the index
    pub fn push(&mut self, item: T) -> usize {
        if let Some(i) = self.garbage.pop() {
            self.data[i] = item;
            i
        } else {
            self.data.push(item);
            self.data.len() - 1
        }
    }

    pub fn pull(&mut self) -> usize {
        if let Some(i) = self.garbage.pop() {
            i
        } else {
            self.data.push(T::default());
            self.data.len() - 1
        }
    }

    // remove the item at position `index`
    pub fn remove(&mut self, index: usize) {
        self.garbage.push(index);
    }

    pub fn clear(&mut self) {
        self.data.clear();
        self.garbage.clear();
    }
}

impl<T> std::ops::Index<usize> for ObjectPool<T> {
    type Output = T;

    fn index(&self, index: usize) -> &Self::Output {
        &self.data[index]
    }
}

impl<T> std::ops::IndexMut<usize> for ObjectPool<T> {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        &mut self.data[index]
    }
}

// node of doubly chained tree
#[derive(Debug, Clone, Default)]
// #[repr(C)]
struct Node {
    parent: NodeIndex,
    child: NodeIndex,
    left: NodeIndex,
    right: NodeIndex,
    remove_check_turn: TurnIndex,
    active: bool,
    action: Action,
    evaluator: Evaluator,
    hash: Hash,
}

impl Node {
    fn root(action: Action, evaluator: Evaluator, hash: Hash) -> Self {
        Self {
            action,
            evaluator,
            hash,
            parent: !0,
            child: !0,
            left: !0,
            right: !0,
            active: true,
            remove_check_turn: 0,
        }
    }

    fn new(candidate: &Candidate, right: NodeIndex) -> Self {
        Self {
            action: candidate.action.clone(),
            evaluator: candidate.evaluator.clone(),
            hash: candidate.hash,
            parent: candidate.parent,
            child: !0,
            left: !0,
            right,
            active: true,
            remove_check_turn: 0,
        }
    }
}

// doubly chained tree
#[derive(Debug, Clone)]
struct Tree {
    state: State,
    nodes: ObjectPool<Node>,
    root: NodeIndex,
    remove_nodes: VecDeque<Vec<NodeIndex>>,
}

impl Tree {
    fn new(state: State, config: &Config) -> Self {
        let (action, evaluator, hash) = state.get_initial_data();
        let mut nodes = ObjectPool::with_capacity(config.nodes_capacity);
        let root = nodes.push(Node::root(action, evaluator, hash)) as NodeIndex;
        let remove_nodes = VecDeque::new();
        Self {
            state,
            nodes,
            root,
            remove_nodes,
        }
    }

    // add candidates while dfs
    fn dfs(&mut self, multi_selectors: &mut MultiSelectors, turn: TurnIndex) {
        self.remove_useless_nodes(turn);
        self.update_root(turn);

        let mut v = self.root;

        if !self.nodes[v as usize].active {
            // no active nodes
            return;
        }

        loop {
            v = self.move_to_leaf(v);

            multi_selectors.reset_step_max();
            self.state.expand(
                &self.nodes[v as usize].action,
                &self.nodes[v as usize].evaluator,
                self.nodes[v as usize].hash,
                v,
                multi_selectors,
            );
            while self.remove_nodes.len() < multi_selectors.step_max {
                self.remove_nodes.push_back(Vec::new());
            }
            self.remove_nodes[multi_selectors.step_max - 1].push(v);
            self.nodes[v as usize].remove_check_turn = turn + multi_selectors.step_max as TurnIndex;

            v = self.move_to_ancestor(v);
            if v == self.root {
                break;
            }
        }
    }

    // get path from the root to the node `v`
    fn get_path(&self, mut v: NodeIndex) -> Vec<Action> {
        // dbg!(self.nodes.data.capacity());

        let mut path = Vec::new();
        while self.nodes[v as usize].parent != !0 {
            path.push(self.nodes[v as usize].action.clone());
            v = self.nodes[v as usize].parent;
        }
        path.reverse();
        path
    }

    // add leaves and activate ancestors
    fn add_leaves(&mut self, candidates: &[Candidate]) {
        #[cfg(target_arch = "x86_64")]
        const PREFETCH_OFFSET: usize = 16;

        #[cfg(target_arch = "x86_64")]
        unsafe {
            // prefetch the parent nodes of the first few candidates
            for i in 0..candidates.len().min(PREFETCH_OFFSET) {
                let parent = candidates[i].parent;
                let ptr = self.nodes.data.as_ptr().add(parent as usize);
                _mm_prefetch(ptr as *const i8, _MM_HINT_T0);
            }
        }
        let mut activation_que = VecDeque::new();
        for (_i, candidate) in candidates.iter().enumerate() {
            #[cfg(target_arch = "x86_64")]
            unsafe {
                // prefetch the parent node
                if _i + PREFETCH_OFFSET < candidates.len() {
                    let parent = candidates[_i + PREFETCH_OFFSET].parent;
                    let ptr = self.nodes.data.as_ptr().add(parent as usize);
                    _mm_prefetch(ptr as *const i8, _MM_HINT_T0);
                }
            }
            let parent = candidate.parent;
            let sibling = self.nodes[parent as usize].child;
            let v = self.nodes.push(Node::new(candidate, sibling)) as NodeIndex;

            self.nodes[parent as usize].child = v;

            if sibling != !0 {
                self.nodes[sibling as usize].left = v;
            }
            if !self.nodes[parent as usize].active {
                self.nodes[parent as usize].active = true;
                if parent == self.root {
                    continue;
                }
                let parent = self.nodes[parent as usize].parent;
                activation_que.push_back(parent);
            }
        }
        // activate ancestors
        #[cfg(target_arch = "x86_64")]
        unsafe {
            // prefetch the parent nodes in the activation queue
            for i in 0..activation_que.len().min(PREFETCH_OFFSET) {
                let ptr = self.nodes.data.as_ptr().add(activation_que[i] as usize);
                _mm_prefetch(ptr as *const i8, _MM_HINT_T0);
            }
        }
        while let Some(u) = activation_que.pop_front() {
            #[cfg(target_arch = "x86_64")]
            unsafe {
                // prefetch the parent node
                if activation_que.len() >= PREFETCH_OFFSET {
                    let ptr = self
                        .nodes
                        .data
                        .as_ptr()
                        .add(activation_que[PREFETCH_OFFSET - 1] as usize);
                    _mm_prefetch(ptr as *const i8, _MM_HINT_T0);
                }
            }
            if self.nodes[u as usize].active {
                continue;
            }
            self.nodes[u as usize].active = true;
            if u == self.root {
                continue;
            }
            let parent = self.nodes[u as usize].parent;
            activation_que.push_back(parent);
        }
    }

    // move to the leftist node in the subtree rooted at `v`
    fn move_to_leaf(&mut self, mut v: NodeIndex) -> NodeIndex {
        debug_assert!(self.nodes[v as usize].active);
        let mut child = self.nodes[v as usize].child;
        while child != !0 {
            // move to right while the node is inactive
            while !self.nodes[child as usize].active {
                child = self.nodes[child as usize].right;
                debug_assert_ne!(child, !0);
            }
            self.nodes[v as usize].active = false;
            v = child;
            self.state.move_forward(&self.nodes[child as usize].action);

            #[cfg(target_arch = "x86_64")]
            unsafe {
                // prefetch the right sibling
                let right = self.nodes[child as usize].right;
                if right != !0 {
                    let ptr = self.nodes.data.as_ptr().add(right as usize);
                    _mm_prefetch(ptr as *const i8, _MM_HINT_T0);
                }
            }
            child = self.nodes[child as usize].child;
        }
        debug_assert!(self.nodes[v as usize].active);
        self.nodes[v as usize].active = false;
        v
    }

    // move to the ancestor of `v` which has the right child
    fn move_to_ancestor(&mut self, mut v: NodeIndex) -> NodeIndex {
        while v != self.root {
            self.state.move_backward(&self.nodes[v as usize].action);

            // move to right while the node is inactive
            let mut u = self.nodes[v as usize].right;
            while u != !0 {
                if self.nodes[u as usize].active {
                    self.state.move_forward(&self.nodes[u as usize].action);
                    return u;
                }
                u = self.nodes[u as usize].right;
            }

            v = self.nodes[v as usize].parent;
        }
        self.root
    }

    // do not round trip the direct road
    fn update_root(&mut self, turn: TurnIndex) {
        let mut child = self.nodes[self.root as usize].child;
        while child != !0
            && self.nodes[child as usize].right == !0
            && self.nodes[self.root as usize].remove_check_turn <= turn
        {
            self.root = child;
            self.state.move_forward(&self.nodes[child as usize].action);
            child = self.nodes[child as usize].child;
        }
    }

    // remove useless nodes
    fn remove_useless_nodes(&mut self, turn: TurnIndex) {
        #[cfg(target_arch = "x86_64")]
        const PREFETCH_OFFSET: usize = 16;

        if self.remove_nodes.is_empty() {
            return;
        }
        let mut remove_nodes_front = self.remove_nodes.pop_front().unwrap();
        for (_i, &v) in remove_nodes_front.iter().enumerate() {
            #[cfg(target_arch = "x86_64")]
            unsafe {
                // prefetch the node to be removed
                if _i + PREFETCH_OFFSET < remove_nodes_front.len() {
                    let ptr = self
                        .nodes
                        .data
                        .as_ptr()
                        .add(remove_nodes_front[_i + PREFETCH_OFFSET] as usize);
                    _mm_prefetch(ptr as *const i8, _MM_HINT_T0);
                }
            }
            if self.nodes[v as usize].child == !0 {
                self.remove_leaf(v, turn);
            }
        }
        remove_nodes_front.clear();
        self.remove_nodes.push_back(remove_nodes_front);
    }

    // remove the node `v` and its ancestors while they have no child
    fn remove_leaf(&mut self, mut v: NodeIndex, turn: TurnIndex) {
        loop {
            if self.nodes[v as usize].remove_check_turn > turn {
                // v can have a child in the future
                return;
            }
            let left = self.nodes[v as usize].left;
            let right = self.nodes[v as usize].right;
            if left == !0 {
                let parent = self.nodes[v as usize].parent;
                debug_assert_ne!(parent, !0, "root node cannot be removed");
                self.nodes.remove(v as usize);
                self.nodes[parent as usize].child = right;
                if right != !0 {
                    self.nodes[right as usize].left = !0;
                    break;
                }
                v = parent;
            } else {
                self.nodes.remove(v as usize);
                self.nodes[left as usize].right = right;
                if right != !0 {
                    self.nodes[right as usize].left = left;
                }
                break;
            }
        }
    }
}

fn beam_search(config: &Config, state: State) -> Option<Vec<Action>> {
    let n = state.n;
    let mut tree = Tree::new(state, config);
    let mut multi_selectors = MultiSelectors::new(config.beam_width, n);
    let mut rng = XorShift32::new(config.seed);

    for turn in 0..config.max_turn {
        // add candidates to selector
        tree.dfs(&mut multi_selectors, turn as TurnIndex);

        let mut selector = multi_selectors.pop_front();
        if let Some(candidate) = &selector.finished_candidate {
            // find the feasible solution in turn minimizing problem
            let mut ret = tree.get_path(candidate.parent);
            ret.push(candidate.action.clone());
            return Some(ret);
        }
        if turn == config.max_turn - 1 || get_time_sec() > TIME_LIMIT_SEC {
            // the last turn or time limit exceeded
            return None;
        }
        // add new nodes
        let candidates = selector.select(&mut rng);
        tree.add_leaves(&candidates);

        multi_selectors.push_back(selector);
    }

    unreachable!();
}

#[derive(Debug, Clone)]
struct Input {
    n: usize, // grid size
    num_balls: usize,
    num_colors: usize,
    goal: Vec<usize>,  // color sequence
    cells: Vec<usize>, // ball color in each cell, !0 if empty
}

fn read_input() -> Input {
    input! {
        n: usize,
        num_balls: usize,
        num_colors: usize,
        mut goal: [usize; num_balls],
        mut cells: [usize; n * n],
    }
    assert!(8 <= n && n <= 16);
    assert!(3 <= num_colors && num_colors <= 7);
    let goal = goal.into_iter().map(|x| x - 1).collect();
    let cells = cells
        .into_iter()
        .map(|x| if x == 0 { !0 } else { x - 1 })
        .collect();
    Input {
        n,
        num_balls,
        num_colors,
        goal: goal,
        cells: cells,
    }
}

fn get_direction(p: (usize, usize), q: (usize, usize)) -> usize {
    let dx = q.0.wrapping_sub(p.0);
    let dy = q.1.wrapping_sub(p.1);
    DXDY.iter()
        .position(|&(dxi, dyi)| dx == dxi && dy == dyi)
        .unwrap()
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
    pub fn gen_u32(&mut self) -> u32 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.state = x;
        x
    }

    /// 1..=u64::MAX
    /// Note: Both high and low 32-bit parts are in range 1..=u32::MAX
    #[inline(always)]
    pub fn gen_u64(&mut self) -> u64 {
        let high = self.gen_u32() as u64;
        let low = self.gen_u32() as u64;
        (high << 32) | low
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f32(&mut self) -> f32 {
        self.gen_u32() as f32 / ((1u64 << 32) as f32)
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f64(&mut self) -> f64 {
        self.gen_u32() as f64 / ((1u64 << 32) as f64)
    }

    /// l..r
    #[inline(always)]
    pub fn gen_range(&mut self, l: usize, r: usize) -> usize {
        debug_assert!(l < r);
        debug_assert!(r as u64 <= 1 << 32);
        l + (((r - l) as u64 * self.gen_u32() as u64) >> 32) as usize
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f32(&mut self, l: f32, r: f32) -> f32 {
        debug_assert!(l <= r);
        l + self.gen_u32() as f32 * ((r - l) / ((1u64 << 32) as f32))
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f64(&mut self, l: f64, r: f64) -> f64 {
        debug_assert!(l <= r);
        l + self.gen_u32() as f64 * ((r - l) / ((1u64 << 32) as f64))
    }

    #[inline(always)]
    pub fn gen_bool(&mut self, p: f64) -> bool {
        debug_assert!(p >= 0.0 && p <= 1.0);
        self.gen_f64() < p
    }

    #[inline(always)]
    pub fn shuffle<T>(&mut self, v: &mut [T]) {
        let n = v.len();
        for i in (1..n).rev() {
            let j = self.gen_range(0, i + 1);
            v.swap(i, j);
        }
    }

    #[inline(always)]
    pub fn partial_shuffle<T>(&mut self, v: &mut [T], n: usize) {
        let m = v.len();
        debug_assert!(n <= m);
        for i in 0..n {
            let j = self.gen_range(i, m);
            v.swap(i, j);
        }
    }

    #[inline(always)]
    pub fn choose<T: Copy>(&mut self, v: &[T]) -> T {
        v[self.gen_range(0, v.len())]
    }
}

#[macro_export]
macro_rules! make_bitset {
    ($n:literal) => {
        StaticBitSet<$n, { ($n + 63) / 64 }>
    };
}

#[derive(Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash)]
pub struct StaticBitSet<const N: usize, const ARRAY_SIZE: usize> {
    data: [u64; ARRAY_SIZE],
}

impl<const N: usize, const ARRAY_SIZE: usize> StaticBitSet<N, ARRAY_SIZE> {
    pub const fn new() -> Self {
        debug_assert!(N > 0);
        debug_assert!(ARRAY_SIZE == (N + 63) / 64);
        Self {
            data: [0; ARRAY_SIZE],
        }
    }

    pub fn get(&self, i: usize) -> bool {
        debug_assert!(i < N);
        (self.data[i / 64] & (1 << (i & 63))) != 0
    }

    pub fn set(&mut self, i: usize, flag: bool) {
        debug_assert!(i < N);
        if flag {
            self.data[i / 64] |= 1 << (i & 63);
        } else {
            self.data[i / 64] &= !(1 << (i & 63));
        }
    }

    pub fn any(&self) -> bool {
        self.data.iter().any(|&x| x != 0)
    }

    pub fn clear(&mut self) {
        self.data.fill(0);
    }

    pub fn count(&self) -> u32 {
        self.data.iter().map(|&x| x.count_ones()).sum()
    }

    pub fn range_set(&mut self, l: usize, r: usize, flag: bool) {
        debug_assert!(l <= r && r <= N);
        if flag {
            if l / 64 == r / 64 {
                self.data[l / 64] |= (1u64 << (r & 63)).wrapping_sub(1 << (l & 63));
                return;
            }
            self.data[l / 64] |= (0 as u64).wrapping_sub(1 << (l & 63));
            for i in (l / 64 + 1)..(r / 64) {
                self.data[i] = !0;
            }
            if r & 63 != 0 {
                self.data[r / 64] |= (1 << (r & 63)) - 1;
            }
        } else {
            if l / 64 == r / 64 {
                self.data[l / 64] &= !((1u64 << (r & 63)).wrapping_sub(1 << (l & 63)));
                return;
            }
            self.data[l / 64] &= !((0 as u64).wrapping_sub(1 << (l & 63)));
            for i in (l / 64 + 1)..(r / 64) {
                self.data[i] = 0;
            }
            if r & 63 != 0 {
                self.data[r / 64] &= !((1 << (r & 63)) - 1);
            }
        }
    }

    pub fn get_ones(&self) -> Vec<usize> {
        let mut ones = vec![];
        for i in 0..self.data.len() {
            let mut x = self.data[i];
            while x > 0 {
                let p = x.trailing_zeros() as usize;
                x ^= 1 << p;
                ones.push(64 * i + p);
            }
        }
        ones
    }

    fn get_by_xy(&self, x: usize, y: usize) -> bool {
        self.get(x * 16 + y)
    }

    fn set_by_xy(&mut self, x: usize, y: usize, flag: bool) {
        self.set(x * 16 + y, flag);
    }

    fn get_ones_div(&self) -> Vec<(usize, usize)> {
        let mut ones = vec![];
        for i in 0..self.data.len() {
            let mut x = self.data[i];
            while x > 0 {
                let p = x.trailing_zeros() as usize;
                x ^= 1 << p;
                let idx = 64 * i + p;
                ones.push((idx / 16, idx % 16));
            }
        }
        ones
    }

    fn nearest_dist(&self, sx: usize, sy: usize) -> usize {
        let mut dist = usize::MAX;
        for i in 0..self.data.len() {
            let mut x = self.data[i];
            while x > 0 {
                let p = x.trailing_zeros() as usize;
                x ^= 1 << p;
                let idx = 64 * i + p;
                let x = idx / 16;
                let y = idx % 16;
                dist.chmin(x.abs_diff(sx) + y.abs_diff(sy));
            }
        }
        dist
    }

    fn splitmix(&self) -> u64 {
        let mut hash = 0;
        for &x in &self.data {
            hash = splitmix64(hash ^ x);
        }
        hash
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::BitAndAssign
    for StaticBitSet<N, ARRAY_SIZE>
{
    fn bitand_assign(&mut self, rhs: Self) {
        for i in 0..ARRAY_SIZE {
            self.data[i] &= rhs.data[i];
        }
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::BitOrAssign
    for StaticBitSet<N, ARRAY_SIZE>
{
    fn bitor_assign(&mut self, rhs: Self) {
        for i in 0..ARRAY_SIZE {
            self.data[i] |= rhs.data[i];
        }
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::BitXorAssign
    for StaticBitSet<N, ARRAY_SIZE>
{
    fn bitxor_assign(&mut self, rhs: Self) {
        for i in 0..ARRAY_SIZE {
            self.data[i] ^= rhs.data[i];
        }
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::BitAnd for StaticBitSet<N, ARRAY_SIZE> {
    type Output = Self;
    fn bitand(self, rhs: Self) -> Self::Output {
        let mut res = self.clone();
        res &= rhs;
        res
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::BitOr for StaticBitSet<N, ARRAY_SIZE> {
    type Output = Self;
    fn bitor(self, rhs: Self) -> Self::Output {
        let mut res = self.clone();
        res |= rhs;
        res
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::BitXor for StaticBitSet<N, ARRAY_SIZE> {
    type Output = Self;
    fn bitxor(self, rhs: Self) -> Self::Output {
        let mut res = self.clone();
        res ^= rhs;
        res
    }
}

impl<const N: usize, const ARRAY_SIZE: usize> std::ops::Not for StaticBitSet<N, ARRAY_SIZE> {
    type Output = Self;
    fn not(self) -> Self::Output {
        let mut res = self.clone();
        for i in 0..ARRAY_SIZE {
            res.data[i] = !res.data[i];
        }
        if (N & 63) != 0 {
            res.data[ARRAY_SIZE - 1] &= (1u64 << (N & 63)) - 1;
        }
        res
    }
}

#[inline(always)]
pub fn next_permutation<T: Ord>(v: &mut [T]) -> bool {
    // Find the largest index i such that v[i] < v[i+1]
    let mut i = match v.len().checked_sub(2) {
        None => return false,
        Some(x) => x,
    };
    loop {
        if v[i] < v[i + 1] {
            break;
        }
        if i == 0 {
            return false;
        }
        i -= 1;
    }
    // Find the largest index j > i such that v[i] < v[j]
    let mut j = v.len() - 1;
    while v[i] >= v[j] {
        j -= 1;
    }
    // Swap v[i] and v[j]
    v.swap(i, j);

    // Reverse the suffix starting at v[i+1]
    v[i + 1..].reverse();

    true
}

#[derive(Default)]
pub struct NoHashHasher {
    value: u32,
}

impl Hasher for NoHashHasher {
    fn write(&mut self, bytes: &[u8]) {
        self.value = u32::from_ne_bytes(bytes[..4].try_into().unwrap());
    }

    fn finish(&self) -> u64 {
        self.value as u64
    }
}

pub type NoHashSet<T> = HashSet<T, BuildHasherDefault<NoHashHasher>>;
pub type NoHashMap<K, V> = HashMap<K, V, BuildHasherDefault<NoHashHasher>>;

#[inline(always)]
pub fn splitmix32(mut x: u32) -> u32 {
    x = x.wrapping_add(0x9e3779b9);
    x = (x ^ (x >> 16)).wrapping_mul(0x85ebca6b);
    x = (x ^ (x >> 13)).wrapping_mul(0xc2b2ae35);
    x ^ (x >> 16)
}

#[inline(always)]
pub fn splitmix64(mut x: u64) -> u64 {
    x = x.wrapping_add(0x9e3779b97f4a7c15);
    x = (x ^ (x >> 30)).wrapping_mul(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)).wrapping_mul(0x94d049bb133111eb);
    x ^ (x >> 31)
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
