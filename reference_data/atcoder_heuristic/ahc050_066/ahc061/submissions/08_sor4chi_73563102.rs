use solver::{
    io::IO,
    router::{RouterConfig, SolverEngine},
    solver::{Solver, greedy::GreedySolver, search::SearchSolver, sim::SimSolver, sim2::Sim2Solver},
    strategy::particle_filter::ParticleFilter,
};
pub mod solver {
pub mod game {
use std::collections::VecDeque;
use super::io::{Input, TurnInput};
pub const DIRS: [(usize, usize); 4] = [(0, 1), (1, 0), (0, usize::MAX), (usize::MAX, 0)];
const BB_COL0: u128 = {
    let mut m = 0u128;
    let mut i = 0;
    while i < 10 {
        m |= 1u128 << (i * 10);
        i += 1;
    }
    m
};
const BB_COL9: u128 = {
    let mut m = 0u128;
    let mut i = 0;
    while i < 10 {
        m |= 1u128 << (i * 10 + 9);
        i += 1;
    }
    m
};
const BB_VALID: u128 = (1u128 << 100) - 1;
#[inline(always)]
pub fn bb_idx(x: usize, y: usize) -> u128 {
    1u128 << (x * 10 + y)
}
#[inline(always)]
pub fn bb_expand(board: u128) -> u128 {
    let right = (board << 1) & !BB_COL0;
    let down = board << 10;
    let left = (board >> 1) & !BB_COL9;
    let up = board >> 10;
    (right | down | left | up) & BB_VALID
}
#[inline]
pub fn bb_flood_fill(seed: u128, passable: u128) -> u128 {
    let mut reach = seed & passable;
    let mut frontier = reach;
    while frontier != 0 {
        let next = bb_expand(frontier) & passable & !reach;
        reach |= next;
        frontier = next;
    }
    reach
}
pub struct BitIter(pub u128);
impl Iterator for BitIter {
    type Item = usize; // bit index
    #[inline(always)]
    fn next(&mut self) -> Option<usize> {
        if self.0 == 0 {
            None
        } else {
            let idx = self.0.trailing_zeros() as usize;
            self.0 &= self.0 - 1; // clear LSB
            Some(idx)
        }
    }
}
pub struct GameState {
    pub n: usize,
    pub m: usize,
    pub u_cap: usize,
    pub v: Vec<Vec<usize>>,
    pub positions: Vec<(usize, usize)>,
    pub owner: Vec<Vec<i32>>,
    pub level: Vec<Vec<usize>>,
}
impl GameState {
    pub fn new(input: &Input) -> Self {
        let n = input.n;
        let m = input.m;
        let mut owner = vec![vec![-1i32; n]; n];
        let mut level = vec![vec![0usize; n]; n];
        for p in 0..m {
            let (sx, sy) = input.starts[p];
            owner[sx][sy] = p as i32;
            level[sx][sy] = 1;
        }
        GameState {
            n,
            m,
            u_cap: input.u,
            v: input.v.clone(),
            positions: input.starts.clone(),
            owner,
            level,
        }
    }
    pub fn update(&mut self, turn_input: &TurnInput) {
        self.positions = turn_input.positions.clone();
        self.owner = turn_input.owner.clone();
        self.level = turn_input.level.clone();
    }
    pub fn get_candidates(&self, player: usize) -> Vec<(usize, usize)> {
        let n = self.n;
        let mut visited = vec![vec![false; n]; n];
        let mut queue = VecDeque::new();
        let mut candidates = Vec::new();
        let start = self.positions[player];
        queue.push_back(start);
        visited[start.0][start.1] = true;
        while let Some((x, y)) = queue.pop_front() {
            let blocked = (0..self.m).any(|i| i != player && self.positions[i] == (x, y));
            if !blocked {
                candidates.push((x, y));
            }
            if self.owner[x][y] == player as i32 {
                for &(dx, dy) in &DIRS {
                    let nx = x.wrapping_add(dx);
                    let ny = y.wrapping_add(dy);
                    if nx < n && ny < n && !visited[nx][ny] {
                        visited[nx][ny] = true;
                        queue.push_back((nx, ny));
                    }
                }
            }
        }
        candidates
    }
    pub fn bfs_dist(&self, player: usize) -> Vec<Vec<usize>> {
        let n = self.n;
        let inf = usize::MAX;
        let mut dist = vec![vec![inf; n]; n];
        let mut queue = VecDeque::new();
        let start = self.positions[player];
        dist[start.0][start.1] = 0;
        queue.push_back(start);
        while let Some((x, y)) = queue.pop_front() {
            let d = dist[x][y];
            if self.owner[x][y] == player as i32 || (x, y) == start {
                for &(dx, dy) in &DIRS {
                    let nx = x.wrapping_add(dx);
                    let ny = y.wrapping_add(dy);
                    if nx < n && ny < n && dist[nx][ny] == inf {
                        dist[nx][ny] = d + 1;
                        queue.push_back((nx, ny));
                    }
                }
            }
        }
        dist
    }
    pub fn compute_scores(&self) -> Vec<i64> {
        let mut scores = vec![0i64; self.m];
        for i in 0..self.n {
            for j in 0..self.n {
                let o = self.owner[i][j];
                if o >= 0 {
                    scores[o as usize] += self.v[i][j] as i64 * self.level[i][j] as i64;
                }
            }
        }
        scores
    }
    pub fn count_adjacent_owned(&self, x: usize, y: usize, player: i32) -> usize {
        let mut count = 0;
        for &(dx, dy) in &DIRS {
            let nx = x.wrapping_add(dx);
            let ny = y.wrapping_add(dy);
            if nx < self.n && ny < self.n && self.owner[nx][ny] == player {
                count += 1;
            }
        }
        count
    }
    pub fn compute_ai_threat(&self, v_ratio: f64) -> Vec<Vec<usize>> {
        let n = self.n;
        let mut threat = vec![vec![0usize; n]; n];
        for ai in 1..self.m {
            let cands = self.get_candidates(ai);
            if cands.is_empty() {
                continue;
            }
            let max_v = cands.iter().map(|&(x, y)| self.v[x][y]).max().unwrap_or(0);
            if max_v == 0 {
                continue;
            }
            let threshold = (max_v as f64 * v_ratio) as usize;
            for &(x, y) in &cands {
                let is_target = self.owner[x][y] == -1
                    || (self.owner[x][y] != ai as i32 && self.level[x][y] == 1);
                if is_target && self.v[x][y] >= threshold {
                    threat[x][y] += 1;
                }
            }
        }
        threat
    }
}
pub const N: usize = 10;
pub const MAX_M: usize = 8;
pub const MAX_LEVEL: usize = 5;
#[derive(Clone)]
pub struct SimState {
    pub n: usize,
    pub m: usize,
    pub u_cap: u8,
    pub owner: [[i32; N]; N],
    pub level: [[u8; N]; N],
    pub positions: [(usize, usize); MAX_M],
    pub scores: [i64; MAX_M],
    pub owned_bb: [u128; MAX_M],
    pub level_bb: [u128; MAX_LEVEL + 1],
}
impl SimState {
    pub fn from_game_state(gs: &GameState) -> Self {
        let mut owner = [[-1i32; N]; N];
        let mut level = [[0u8; N]; N];
        let mut positions = [(0usize, 0usize); MAX_M];
        for i in 0..gs.n {
            for j in 0..gs.n {
                owner[i][j] = gs.owner[i][j];
                level[i][j] = gs.level[i][j] as u8;
            }
        }
        positions[..gs.m].copy_from_slice(&gs.positions[..gs.m]);
        let mut scores = [0i64; MAX_M];
        let mut owned_bb = [0u128; MAX_M];
        let mut level_bb = [0u128; MAX_LEVEL + 1];
        for i in 0..gs.n {
            for j in 0..gs.n {
                let bit = bb_idx(i, j);
                level_bb[level[i][j] as usize] |= bit;
                let o = owner[i][j];
                if o >= 0 {
                    scores[o as usize] += gs.v[i][j] as i64 * level[i][j] as i64;
                    owned_bb[o as usize] |= bit;
                }
            }
        }
        SimState {
            n: gs.n,
            m: gs.m,
            u_cap: gs.u_cap as u8,
            owner,
            level,
            positions,
            scores,
            owned_bb,
            level_bb,
        }
    }
    pub fn get_candidates(&self, player: usize) -> Vec<(usize, usize)> {
        let n = self.n;
        let mut visited = [[false; N]; N];
        let mut queue = [(0usize, 0usize); N * N];
        let (mut qh, mut qt) = (0, 0);
        let mut candidates = Vec::new();
        let mut occupied = [[false; N]; N];
        for i in 0..self.m {
            if i != player {
                let (px, py) = self.positions[i];
                occupied[px][py] = true;
            }
        }
        let start = self.positions[player];
        visited[start.0][start.1] = true;
        queue[qt] = start;
        qt += 1;
        while qh < qt {
            let (x, y) = queue[qh];
            qh += 1;
            if !occupied[x][y] {
                candidates.push((x, y));
            }
            if self.owner[x][y] == player as i32 {
                for &(dx, dy) in &DIRS {
                    let nx = x.wrapping_add(dx);
                    let ny = y.wrapping_add(dy);
                    if nx < n && ny < n && !visited[nx][ny] {
                        visited[nx][ny] = true;
                        queue[qt] = (nx, ny);
                        qt += 1;
                    }
                }
            }
        }
        candidates
    }
    pub fn simulate_turn(&mut self, moves: &[(usize, usize)], v: &[[usize; N]; N]) {
        let original_positions = self.positions;
        let mut counts = [[0u8; N]; N];
        for &(x, y) in &moves[..self.m] {
            counts[x][y] += 1;
        }
        let mut collected = [false; MAX_M];
        for p in 0..self.m {
            let (x, y) = moves[p];
            if counts[x][y] >= 2 && p as i32 != self.owner[x][y] {
                collected[p] = true;
            }
        }
        for p in 0..self.m {
            if collected[p] {
                continue;
            }
            let (x, y) = moves[p];
            let cell_owner = self.owner[x][y];
            let cv = v[x][y] as i64;
            let bit = bb_idx(x, y);
            if cell_owner == -1 {
                self.owner[x][y] = p as i32;
                let old_level = self.level[x][y] as usize; // 0
                let new_level = 1usize;
                self.level[x][y] = new_level as u8;
                self.level_bb[old_level] &= !bit;
                self.level_bb[new_level] |= bit;
                self.scores[p] += cv;
                self.owned_bb[p] |= bit;
            } else if cell_owner == p as i32 {
                if self.level[x][y] < self.u_cap {
                    let old_level = self.level[x][y] as usize;
                    self.level[x][y] += 1;
                    let new_level = self.level[x][y] as usize;
                    self.level_bb[old_level] &= !bit;
                    self.level_bb[new_level] |= bit;
                    self.scores[p] += cv;
                }
            } else {
                let old_owner = cell_owner as usize;
                self.scores[old_owner] -= cv;
                let old_level = self.level[x][y] as usize;
                self.level[x][y] -= 1;
                let dec_level = self.level[x][y] as usize;
                self.level_bb[old_level] &= !bit;
                self.level_bb[dec_level] |= bit;
                if self.level[x][y] == 0 {
                    self.owner[x][y] = p as i32;
                    self.level[x][y] = 1;
                    self.scores[p] += cv;
                    self.owned_bb[old_owner] &= !bit;
                    self.owned_bb[p] |= bit;
                    self.level_bb[0] &= !bit;
                    self.level_bb[1] |= bit;
                } else {
                    collected[p] = true;
                }
            }
        }
        for p in 0..self.m {
            if collected[p] {
                self.positions[p] = original_positions[p];
            } else {
                self.positions[p] = moves[p];
            }
        }
    }
    pub fn compute_scores(&self, v: &[[usize; N]; N]) -> [i64; MAX_M] {
        let mut scores = [0i64; MAX_M];
        for (i, row) in self.owner.iter().enumerate().take(self.n) {
            for (j, &o) in row.iter().enumerate().take(self.n) {
                if o >= 0 {
                    scores[o as usize] += v[i][j] as i64 * self.level[i][j] as i64;
                }
            }
        }
        scores
    }
}
}
pub mod io {
use std::fmt::Display;
use proconio::input_interactive;
pub struct Input {
    pub n: usize,
    pub m: usize,
    pub t: usize,
    pub u: usize,
    pub v: Vec<Vec<usize>>,
    pub starts: Vec<(usize, usize)>,
    pub oracle_ai_params: Option<Vec<(f64, f64, f64, f64, f64)>>,
}
impl Display for Input {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "{} {} {} {}", self.n, self.m, self.t, self.u)?;
        for i in 0..self.n {
            let row: Vec<String> = self.v[i].iter().map(|v| v.to_string()).collect();
            writeln!(f, "{}", row.join(" "))?;
        }
        for &(sx, sy) in &self.starts {
            writeln!(f, "{} {}", sx, sy)?;
        }
        Ok(())
    }
}
pub struct TurnInput {
    pub targets: Vec<(usize, usize)>,
    pub positions: Vec<(usize, usize)>,
    pub owner: Vec<Vec<i32>>,
    pub level: Vec<Vec<usize>>,
}
pub struct Output {
    pub tx: usize,
    pub ty: usize,
}
impl Display for Output {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} {}", self.tx, self.ty)
    }
}
#[derive(Default)]
pub struct IO {}
impl IO {
    pub fn read(&mut self) -> Input {
        input_interactive! {
            n: usize, m: usize, t: usize, u: usize,
            v: [[usize; n]; n],
            starts: [(usize, usize); m],
        }
        let oracle_ai_params =
            if std::env::var("DANGEROUSLY_CORRECT_ESTIMATE").is_ok() && m > 1 {
                input_interactive! {
                    params: [(f64, f64, f64, f64, f64); m - 1],
                }
                Some(params)
            } else {
                None
            };
        Input {
            n,
            m,
            t,
            u,
            v,
            starts,
            oracle_ai_params,
        }
    }
    pub fn read_turn(&mut self, m: usize, n: usize) -> TurnInput {
        input_interactive! {
            targets: [(usize, usize); m],
            positions: [(usize, usize); m],
            owner: [[i32; n]; n],
            level: [[usize; n]; n],
        }
        TurnInput {
            targets,
            positions,
            owner,
            level,
        }
    }
    pub fn write(&self, output: &Output) {
        println!("{}", output);
    }
}
}
pub mod router {
use std::env;
use super::io::Input;
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum SolverEngine {
    Sim,
    Sim2,
    Search,
    Greedy,
}
impl SolverEngine {
    pub fn parse(s: &str) -> Option<Self> {
        match s {
            "sim" => Some(Self::Sim),
            "sim2" => Some(Self::Sim2),
            "search" => Some(Self::Search),
            "greedy" => Some(Self::Greedy),
            _ => None,
        }
    }
    pub fn as_str(self) -> &'static str {
        match self {
            Self::Sim => "sim",
            Self::Sim2 => "sim2",
            Self::Search => "search",
            Self::Greedy => "greedy",
        }
    }
}
pub struct RouterConfig {}
impl Default for RouterConfig {
    fn default() -> Self {
        Self {}
    }
}
impl RouterConfig {
    pub fn from_env() -> Self {
        Self::default()
    }
    pub fn decide(&self, _input: &Input) -> SolverEngine {
        SolverEngine::Sim2
    }
}
}
pub mod solver {
use super::io::Output;
pub mod greedy {
use std::env;
use super::super::game::GameState;
use super::super::io::{IO, Input, Output};
use super::Solver;
pub struct GreedyConfig {
    pub capture_ratio_cap: f64,
    pub connectivity_weight: f64,
    pub collision_sensitivity: f64,
    pub ai_threat_v_ratio: f64,
}
impl Default for GreedyConfig {
    fn default() -> Self {
        Self {
            capture_ratio_cap: 4.9,
            connectivity_weight: 0.16,
            collision_sensitivity: 1.0,
            ai_threat_v_ratio: 0.46,
        }
    }
}
impl GreedyConfig {
    pub fn from_env() -> Self {
        let mut config = Self::default();
        if let Ok(v) = env::var("GREEDY_CAPTURE_RATIO_CAP") {
            config.capture_ratio_cap = v.parse().unwrap_or(config.capture_ratio_cap);
        }
        if let Ok(v) = env::var("GREEDY_CONNECTIVITY_WEIGHT") {
            config.connectivity_weight = v.parse().unwrap_or(config.connectivity_weight);
        }
        if let Ok(v) = env::var("GREEDY_COLLISION_SENSITIVITY") {
            config.collision_sensitivity = v.parse().unwrap_or(config.collision_sensitivity);
        }
        if let Ok(v) = env::var("GREEDY_AI_THREAT_V_RATIO") {
            config.ai_threat_v_ratio = v.parse().unwrap_or(config.ai_threat_v_ratio);
        }
        config
    }
}
pub struct GreedySolver<'a> {
    input: &'a Input,
    io: &'a mut IO,
    config: GreedyConfig,
}
struct TurnContext {
    dist: Vec<Vec<usize>>,
    ai_threat: Vec<Vec<usize>>,
    score_ratio: f64,
}
impl TurnContext {
    fn new(state: &GameState, config: &GreedyConfig) -> Self {
        let dist = state.bfs_dist(0);
        let ai_threat = state.compute_ai_threat(config.ai_threat_v_ratio);
        let scores = state.compute_scores();
        let s0 = scores[0] as f64;
        let sa = scores[1..].iter().copied().max().unwrap_or(0) as f64;
        let score_ratio = if sa > 0.0 { s0 / sa } else { 1.0 };
        TurnContext {
            dist,
            ai_threat,
            score_ratio,
        }
    }
}
impl GreedySolver<'_> {
    pub fn new<'a>(input: &'a Input, io: &'a mut IO) -> GreedySolver<'a> {
        GreedySolver {
            input,
            io,
            config: GreedyConfig::from_env(),
        }
    }
    fn evaluate_cell(&self, x: usize, y: usize, state: &GameState, ctx: &TurnContext) -> f64 {
        let v = state.v[x][y] as f64;
        let owner = state.owner[x][y];
        let level = state.level[x][y];
        let cfg = &self.config;
        let d = ctx.dist[x][y];
        if d == usize::MAX {
            return f64::NEG_INFINITY;
        }
        let gain = if owner == -1 {
            v
        } else if owner == 0 {
            if level < state.u_cap {
                v
            } else {
                return 0.0;
            }
        } else if level == 1 {
            v * (1.0 + ctx.score_ratio.min(cfg.capture_ratio_cap))
        } else {
            v * (1.0 + ctx.score_ratio.min(cfg.capture_ratio_cap)) / level as f64
        };
        let transit = if d <= 1 { 0.0 } else { (d - 1) as f64 };
        let efficiency = gain / (1.0 + transit);
        let connectivity = if owner == -1 {
            let adj = state.count_adjacent_owned(x, y, 0) as f64;
            v * cfg.connectivity_weight * adj
        } else {
            0.0
        };
        let k = ctx.ai_threat[x][y] as f64;
        let collision_factor = 1.0 / (1.0 + cfg.collision_sensitivity * k);
        (efficiency + connectivity) * collision_factor
    }
}
impl Solver for GreedySolver<'_> {
    fn solve(&mut self) -> Vec<Output> {
        let mut state = GameState::new(self.input);
        let mut outputs = Vec::with_capacity(self.input.t);
        for _turn in 0..self.input.t {
            let candidates = state.get_candidates(0);
            let ctx = TurnContext::new(&state, &self.config);
            let mut best_cell = candidates[0];
            let mut best_score = f64::NEG_INFINITY;
            for &(x, y) in &candidates {
                let score = self.evaluate_cell(x, y, &state, &ctx);
                if score > best_score {
                    best_score = score;
                    best_cell = (x, y);
                }
            }
            let output = Output {
                tx: best_cell.0,
                ty: best_cell.1,
            };
            self.io.write(&output);
            outputs.push(output);
            let turn_input = self.io.read_turn(self.input.m, self.input.n);
            state.update(&turn_input);
        }
        outputs
    }
}
}
pub mod search {
use std::cmp::Reverse;
use std::env;
use std::time::{Duration, Instant};
use rand::Rng;
use rand::SeedableRng;
use rand_pcg::Pcg64Mcg;
use super::super::game::GameState;
use super::super::game::{BitIter, MAX_M, N, SimState, bb_expand, bb_idx};
use super::super::io::{IO, Input, Output};
use super::super::strategy::Estimator;
use super::super::strategy::cut_target::find_cut_targets;
use super::super::strategy::move_predict::{    IDX_X, IDX_Y, compute_connected_bb, predict_leader_greedy_target, predict_move_ai_bb,    predict_move_p0_bb, random_move_bb, update_connected_after_owned_change,};
use super::Solver;
pub struct SearchConfig {
    pub total_budget: f64,
    pub max_depth: usize,
    pub root_top_k: usize,
    pub node_top_k: usize,
    pub mc_budget_factor: f64,
    pub mcts_fixed_iters: usize,
    pub mcts_min_iters: usize,
    pub mcts_c_puct: f64,
    pub mcts_pw_k: f64,
    pub mcts_pw_alpha: f64,
    pub mcts_prior_beta: f64,
    pub use_exact_dist: bool,
    pub n_cut_targets: usize,
    pub n_leader_targets: usize,
    pub n_rival_targets: usize,
    pub n_pressure_players: usize,
    pub leader_chase_w: f64,
    pub anti_escape_w: f64,
    pub pressure_trigger_gap: i64,
    pub dist_branch_players: usize,
    pub dist_top_moves: usize,
    pub dist_min_prob: f64,
    pub dist_max_scenarios: usize,
    pub phi_upgrade_w: f64,
    pub phi_frontier_w: f64,
    pub phi_territory_w: f64,
    pub phi_opp_softmax_beta: f64,
}
impl Default for SearchConfig {
    fn default() -> Self {
        Self {
            total_budget: 1.85,
            max_depth: 12,
            root_top_k: 10,
            node_top_k: 6,
            mc_budget_factor: 0.0,
            mcts_fixed_iters: 1500,
            mcts_min_iters: 160,
            mcts_c_puct: 1.3,
            mcts_pw_k: 1.8,
            mcts_pw_alpha: 0.7,
            mcts_prior_beta: 0.001,
            use_exact_dist: false,
            n_cut_targets: 3,
            n_leader_targets: 3,
            n_rival_targets: 0,
            n_pressure_players: 2,
            leader_chase_w: 0.0,
            anti_escape_w: 0.0,
            pressure_trigger_gap: 2_000,
            dist_branch_players: 2,
            dist_top_moves: 3,
            dist_min_prob: 0.01,
            dist_max_scenarios: 16,
            phi_upgrade_w: 0.0,
            phi_frontier_w: 0.15,
            phi_territory_w: 50.0,
            phi_opp_softmax_beta: 5.0,
        }
    }
}
impl SearchConfig {
    pub fn from_env() -> Self {
        let mut config = Self::default();
        if let Ok(v) = env::var("SEARCH_BUDGET") {
            config.total_budget = v.parse().unwrap_or(config.total_budget);
        }
        if let Ok(v) = env::var("SEARCH_MAX_DEPTH") {
            config.max_depth = v.parse().unwrap_or(config.max_depth);
        }
        if let Ok(v) = env::var("SEARCH_TOP_K") {
            config.root_top_k = v.parse().unwrap_or(config.root_top_k);
        }
        if let Ok(v) = env::var("SEARCH_NODE_TOP_K") {
            config.node_top_k = v.parse().unwrap_or(config.node_top_k);
        }
        if let Ok(v) = env::var("SEARCH_MC_BUDGET_FACTOR") {
            config.mc_budget_factor = v.parse().unwrap_or(config.mc_budget_factor);
        }
        if let Ok(v) = env::var("SEARCH_MCTS_FIXED_ITERS") {
            config.mcts_fixed_iters = v.parse().unwrap_or(config.mcts_fixed_iters);
        }
        if let Ok(v) = env::var("SEARCH_MCTS_MIN_ITERS") {
            config.mcts_min_iters = v.parse().unwrap_or(config.mcts_min_iters);
        }
        if let Ok(v) = env::var("SEARCH_MCTS_C_PUCT") {
            config.mcts_c_puct = v.parse().unwrap_or(config.mcts_c_puct);
        }
        if let Ok(v) = env::var("SEARCH_MCTS_PW_K") {
            config.mcts_pw_k = v.parse().unwrap_or(config.mcts_pw_k);
        }
        if let Ok(v) = env::var("SEARCH_MCTS_PW_ALPHA") {
            config.mcts_pw_alpha = v.parse().unwrap_or(config.mcts_pw_alpha);
        }
        if let Ok(v) = env::var("SEARCH_MCTS_PRIOR_BETA") {
            config.mcts_prior_beta = v.parse().unwrap_or(config.mcts_prior_beta);
        }
        if let Ok(v) = env::var("SEARCH_USE_EXACT_DIST") {
            let l = v.to_ascii_lowercase();
            config.use_exact_dist = l == "1" || l == "true" || l == "yes" || l == "on";
        }
        if let Ok(v) = env::var("SEARCH_N_CUT_TARGETS") {
            config.n_cut_targets = v.parse().unwrap_or(config.n_cut_targets);
        }
        if let Ok(v) = env::var("SEARCH_N_LEADER_TARGETS") {
            config.n_leader_targets = v.parse().unwrap_or(config.n_leader_targets);
        }
        if let Ok(v) = env::var("SEARCH_N_RIVAL_TARGETS") {
            config.n_rival_targets = v.parse().unwrap_or(config.n_rival_targets);
        }
        if let Ok(v) = env::var("SEARCH_N_PRESSURE_PLAYERS") {
            config.n_pressure_players = v.parse().unwrap_or(config.n_pressure_players);
        }
        if let Ok(v) = env::var("SEARCH_LEADER_CHASE_W") {
            config.leader_chase_w = v.parse().unwrap_or(config.leader_chase_w);
        }
        if let Ok(v) = env::var("SEARCH_ANTI_ESCAPE_W") {
            config.anti_escape_w = v.parse().unwrap_or(config.anti_escape_w);
        }
        if let Ok(v) = env::var("SEARCH_PRESSURE_TRIGGER_GAP") {
            config.pressure_trigger_gap = v.parse().unwrap_or(config.pressure_trigger_gap);
        }
        if let Ok(v) = env::var("SEARCH_DIST_BRANCH_PLAYERS") {
            config.dist_branch_players = v.parse().unwrap_or(config.dist_branch_players);
        }
        if let Ok(v) = env::var("SEARCH_DIST_TOP_MOVES") {
            config.dist_top_moves = v.parse().unwrap_or(config.dist_top_moves);
        }
        if let Ok(v) = env::var("SEARCH_DIST_MIN_PROB") {
            config.dist_min_prob = v.parse().unwrap_or(config.dist_min_prob);
        }
        if let Ok(v) = env::var("SEARCH_DIST_MAX_SCENARIOS") {
            config.dist_max_scenarios = v.parse().unwrap_or(config.dist_max_scenarios);
        }
        if let Ok(v) = env::var("SEARCH_PHI_UPGRADE_W") {
            config.phi_upgrade_w = v.parse().unwrap_or(config.phi_upgrade_w);
        }
        if let Ok(v) = env::var("SEARCH_PHI_FRONTIER_W") {
            config.phi_frontier_w = v.parse().unwrap_or(config.phi_frontier_w);
        }
        if let Ok(v) = env::var("SEARCH_PHI_OPP_SOFTMAX_BETA") {
            config.phi_opp_softmax_beta = v.parse().unwrap_or(config.phi_opp_softmax_beta);
        }
        config
    }
}
pub struct SearchSolver<'a, E: Estimator> {
    input: &'a Input,
    io: &'a mut IO,
    config: SearchConfig,
    v: [[usize; N]; N],
    use_oracle: bool,
    _phantom: std::marker::PhantomData<E>,
}
#[allow(dead_code)]
#[derive(Clone)]
struct Scenario {
    moves: [(usize, usize); MAX_M],
    prob: f64,
}
#[allow(dead_code)]
#[derive(Clone)]
struct MctsEdge {
    action: (usize, usize),
    prior: f64,
    child: usize,
}
#[allow(dead_code)]
#[derive(Default)]
struct MctsNode {
    visits: u32,
    value_sum: f64,
    actions: Vec<((usize, usize), f64)>,
    next_expand: usize,
    edges: Vec<MctsEdge>,
    expanded: bool,
}
impl<E: Estimator> SearchSolver<'_, E> {
    pub fn new<'a>(input: &'a Input, io: &'a mut IO) -> SearchSolver<'a, E> {
        let mut v = [[0usize; N]; N];
        for (i, row) in v.iter_mut().enumerate().take(input.n) {
            row[..input.n].copy_from_slice(&input.v[i][..input.n]);
        }
        let use_oracle = std::env::var("DANGEROUSLY_CORRECT_ESTIMATE").is_ok()
            && input.oracle_ai_params.is_some();
        SearchSolver {
            input,
            io,
            config: SearchConfig::from_env(),
            v,
            use_oracle,
            _phantom: std::marker::PhantomData,
        }
    }
    fn oracle_params_or_estimated(
        &self,
        estimator: &E,
        m: usize,
    ) -> [(f64, f64, f64, f64, f64); MAX_M] {
        let mut out = [(1.0, 1.0, 1.0, 1.0, 0.3); MAX_M];
        if self.use_oracle {
            if let Some(ref oracle) = self.input.oracle_ai_params {
                for p in 1..m {
                    if let Some(&(wa, wb, wc, wd, eps)) = oracle.get(p - 1) {
                        out[p] = (wa, wb, wc, wd, eps);
                    }
                }
            }
        } else {
            for (p, slot) in out.iter_mut().enumerate().take(m).skip(1) {
                *slot = estimator.get_params(p);
            }
        }
        out
    }
    fn param_distribution(
        &self,
        state: &SimState,
        player: usize,
        params: (f64, f64, f64, f64, f64),
    ) -> Vec<((usize, usize), f64)> {
        let (wa, wb, wc, wd, eps) = params;
        let cands = state.get_candidates(player);
        let n = cands.len();
        if n == 0 {
            return vec![(state.positions[player], 1.0)];
        }
        if n == 1 {
            return vec![(cands[0], 1.0)];
        }
        let p_i32 = player as i32;
        let mut max_a = f64::NEG_INFINITY;
        let mut actions = vec![0.0f64; n];
        for (i, &(x, y)) in cands.iter().enumerate() {
            let cv = self.v[x][y] as f64;
            let o = state.owner[x][y];
            let l = state.level[x][y];
            let a = if o == -1 {
                cv * wa
            } else if o == p_i32 {
                if l < state.u_cap { cv * wb } else { 0.0 }
            } else if l == 1 {
                cv * wc
            } else {
                cv * wd
            };
            actions[i] = a;
            if a > max_a {
                max_a = a;
            }
        }
        let tol = 1e-9 * max_a.abs().max(1.0);
        let n_ties = actions.iter().filter(|&&a| max_a - a <= tol).count().max(1);
        let uniform = eps / n as f64;
        let mut out = Vec::with_capacity(n);
        for (i, &mv) in cands.iter().enumerate() {
            let greedy_p = if max_a - actions[i] <= tol {
                (1.0 - eps) / n_ties as f64
            } else {
                0.0
            };
            out.push((mv, greedy_p + uniform));
        }
        out.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        out
    }
    fn param_greedy_move(
        &self,
        state: &SimState,
        player: usize,
        params: (f64, f64, f64, f64, f64),
    ) -> (usize, usize) {
        let (wa, wb, wc, wd, _eps) = params;
        let cands = state.get_candidates(player);
        if cands.is_empty() {
            return state.positions[player];
        }
        if cands.len() == 1 {
            return cands[0];
        }
        let p_i32 = player as i32;
        let mut best = cands[0];
        let mut best_a = f64::NEG_INFINITY;
        for &(x, y) in &cands {
            let cv = self.v[x][y] as f64;
            let o = state.owner[x][y];
            let l = state.level[x][y];
            let a = if o == -1 {
                cv * wa
            } else if o == p_i32 {
                if l < state.u_cap { cv * wb } else { 0.0 }
            } else if l == 1 {
                cv * wc
            } else {
                cv * wd
            };
            if a > best_a {
                best_a = a;
                best = (x, y);
            }
        }
        best
    }
    fn player_distribution(
        &self,
        state: &SimState,
        estimator: &E,
        player: usize,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
    ) -> Vec<((usize, usize), f64)> {
        if self.use_oracle || !self.config.use_exact_dist {
            return self.param_distribution(state, player, params_all[player]);
        }
        let mut d = estimator.get_move_distribution(state, &self.v, player);
        if d.is_empty() {
            d.push((state.positions[player], 1.0));
        }
        d.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        d
    }
    fn compress_distribution(
        &self,
        dist: &[((usize, usize), f64)],
        base_move: (usize, usize),
    ) -> Vec<((usize, usize), f64)> {
        if dist.is_empty() {
            return vec![(base_move, 1.0)];
        }
        let mut sorted = dist.to_vec();
        sorted.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        let mut out: Vec<((usize, usize), f64)> = Vec::with_capacity(self.config.dist_top_moves + 1);
        for &(mv, p) in &sorted {
            if p <= 0.0 {
                continue;
            }
            if out.len() >= self.config.dist_top_moves.max(1) {
                break;
            }
            if p < self.config.dist_min_prob && !out.is_empty() {
                continue;
            }
            out.push((mv, p));
        }
        if out.is_empty() {
            out.push(sorted[0]);
        }
        let mut kept_sum: f64 = out.iter().map(|&(_, p)| p).sum();
        if kept_sum < 1.0 - 1e-12 {
            let residual = (1.0 - kept_sum).max(0.0);
            if let Some(item) = out.iter_mut().find(|x| x.0 == base_move) {
                item.1 += residual;
            } else {
                out.push((base_move, residual));
            }
            kept_sum += residual;
        }
        if kept_sum <= 0.0 {
            return vec![(base_move, 1.0)];
        }
        for item in &mut out {
            item.1 /= kept_sum;
        }
        out.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        out
    }
    fn select_branch_players(
        &self,
        state: &SimState,
        dists: &[Vec<((usize, usize), f64)>; MAX_M],
    ) -> Vec<usize> {
        let (x0, y0) = state.positions[0];
        let mut players: Vec<usize> = (1..state.m)
            .filter(|&p| dists[p].len() > 1)
            .collect();
        players.sort_unstable_by_key(|&p| {
            let (x, y) = state.positions[p];
            let dist = x.abs_diff(x0) + y.abs_diff(y0);
            (Reverse(state.scores[p]), dist)
        });
        players.truncate(self.config.dist_branch_players.min(players.len()));
        players
    }
    fn build_scenarios(
        &self,
        state: &SimState,
        estimator: &E,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
    ) -> Vec<Scenario> {
        let m = state.m;
        let mut base_moves = [(0usize, 0usize); MAX_M];
        let mut dists: [Vec<((usize, usize), f64)>; MAX_M] = std::array::from_fn(|_| Vec::new());
        for p in 1..m {
            let mut d = self.player_distribution(state, estimator, p, params_all);
            if d.is_empty() {
                d.push((state.positions[p], 1.0));
            }
            d.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
            base_moves[p] = d[0].0;
            dists[p] = d;
        }
        let players = self.select_branch_players(state, &dists);
        if players.is_empty() {
            return vec![Scenario {
                moves: base_moves,
                prob: 1.0,
            }];
        }
        let mut branch_dists: Vec<(usize, Vec<((usize, usize), f64)>)> = Vec::new();
        for p in players {
            let c = self.compress_distribution(&dists[p], base_moves[p]);
            if !c.is_empty() {
                branch_dists.push((p, c));
            }
        }
        if branch_dists.is_empty() {
            return vec![Scenario {
                moves: base_moves,
                prob: 1.0,
            }];
        }
        let mut out: Vec<Scenario> = Vec::new();
        let mut cur_moves = base_moves;
        fn dfs(
            idx: usize,
            branch_dists: &[(usize, Vec<((usize, usize), f64)>)],
            cur_moves: &mut [(usize, usize); MAX_M],
            cur_prob: f64,
            out: &mut Vec<Scenario>,
        ) {
            if idx == branch_dists.len() {
                out.push(Scenario {
                    moves: *cur_moves,
                    prob: cur_prob,
                });
                return;
            }
            let (p, dist) = &branch_dists[idx];
            for &(mv, pr) in dist {
                cur_moves[*p] = mv;
                dfs(idx + 1, branch_dists, cur_moves, cur_prob * pr, out);
            }
        }
        dfs(0, &branch_dists, &mut cur_moves, 1.0, &mut out);
        if out.is_empty() {
            return vec![Scenario {
                moves: base_moves,
                prob: 1.0,
            }];
        }
        out.sort_unstable_by(|a, b| b.prob.partial_cmp(&a.prob).unwrap_or(std::cmp::Ordering::Equal));
        out.truncate(self.config.dist_max_scenarios.max(1).min(out.len()));
        let sum_prob: f64 = out.iter().map(|s| s.prob).sum();
        if sum_prob > 0.0 {
            for s in &mut out {
                s.prob /= sum_prob;
            }
        } else {
            out.clear();
            out.push(Scenario {
                moves: base_moves,
                prob: 1.0,
            });
        }
        out
    }
    fn move_gain_p0(&self, state: &SimState, mv: (usize, usize)) -> f64 {
        let (x, y) = mv;
        let cv = self.v[x][y] as f64;
        let owner = state.owner[x][y];
        let level = state.level[x][y];
        if owner == -1 {
            cv * 1.25
        } else if owner == 0 {
            if level < state.u_cap {
                cv * 0.95
            } else {
                cv * 0.05
            }
        } else if level == 1 {
            cv * 1.35
        } else {
            cv * 0.25
        }
    }
    fn leader_pressure_gain(
        &self,
        state: &SimState,
        mv: (usize, usize),
        leader: usize,
        leader_pred: (usize, usize),
    ) -> f64 {
        let (x, y) = mv;
        let cv = self.v[x][y] as f64;
        let owner = state.owner[x][y];
        let level = state.level[x][y];
        if owner == leader as i32 {
            if level == 1 {
                cv * 1.4
            } else {
                cv * 0.45
            }
        } else if mv == leader_pred {
            cv * 0.8
        } else {
            0.0
        }
    }
    fn rival_pressure_gain(
        &self,
        state: &SimState,
        mv: (usize, usize),
        rival: usize,
        rival_pred: (usize, usize),
    ) -> f64 {
        let (x, y) = mv;
        let cv = self.v[x][y] as f64;
        let owner = state.owner[x][y];
        let level = state.level[x][y];
        if owner == rival as i32 {
            if level == 1 {
                cv * 0.95
            } else {
                cv * 0.30
            }
        } else if mv == rival_pred {
            cv * 0.45
        } else {
            0.0
        }
    }
    fn leader_direction_gain(&self, state: &SimState, mv: (usize, usize), leader: usize) -> f64 {
        let (x0, y0) = state.positions[0];
        let (lx, ly) = state.positions[leader];
        let d_old = x0.abs_diff(lx) + y0.abs_diff(ly);
        let d_new = mv.0.abs_diff(lx) + mv.1.abs_diff(ly);
        let delta = d_old as f64 - d_new as f64;
        let s0 = state.scores[0] as f64;
        let sl = state.scores[leader] as f64;
        let gap_scale = if sl > s0 {
            ((sl - s0) / 20_000.0).clamp(0.5, 2.5)
        } else {
            0.35
        };
        delta * self.config.leader_chase_w * gap_scale
    }
    fn anti_escape_gain(&self, state: &SimState, mv: (usize, usize)) -> f64 {
        if state.m < 2 || self.config.anti_escape_w <= 0.0 {
            return 0.0;
        }
        let mut opp_rank: Vec<usize> = (1..state.m).collect();
        opp_rank.sort_unstable_by_key(|&p| std::cmp::Reverse(state.scores[p]));
        opp_rank.truncate(2.min(opp_rank.len()));
        if opp_rank.is_empty() {
            return 0.0;
        }
        let leader = opp_rank[0];
        let s0 = state.scores[0] as f64;
        let sl = state.scores[leader] as f64;
        let gap = sl - s0;
        if gap <= 10_000.0 {
            return 0.0;
        }
        let (x0, y0) = state.positions[0];
        let mut old_min = usize::MAX;
        let mut new_min = usize::MAX;
        for &p in &opp_rank {
            let (x, y) = state.positions[p];
            old_min = old_min.min(x0.abs_diff(x) + y0.abs_diff(y));
            new_min = new_min.min(mv.0.abs_diff(x) + mv.1.abs_diff(y));
        }
        let delta = old_min as f64 - new_min as f64;
        let gap_scale = (gap / 20_000.0).clamp(0.5, 2.0);
        delta * self.config.anti_escape_w * gap_scale
    }
    fn is_pressure_move(&self, state: &SimState, mv: (usize, usize), opp_rank: &[usize]) -> bool {
        if opp_rank.is_empty() {
            return false;
        }
        let owner = state.owner[mv.0][mv.1];
        if owner >= 1 {
            return true;
        }
        let (x0, y0) = state.positions[0];
        let mut old_min = usize::MAX;
        let mut new_min = usize::MAX;
        for &p in opp_rank {
            let (x, y) = state.positions[p];
            old_min = old_min.min(x0.abs_diff(x) + y0.abs_diff(y));
            new_min = new_min.min(mv.0.abs_diff(x) + mv.1.abs_diff(y));
        }
        new_min <= old_min
    }
    fn select_candidates(
        &self,
        state: &SimState,
        top_k: usize,
        use_cut_targets: bool,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
    ) -> Vec<(usize, usize)> {
        let connected_bb = compute_connected_bb(state);
        let mut occupied_bb = 0u128;
        for i in 0..state.m {
            if i != 0 {
                let (px, py) = state.positions[i];
                occupied_bb |= bb_idx(px, py);
            }
        }
        let pos0 = state.positions[0];
        let cand_bb = (connected_bb[0] | bb_expand(connected_bb[0])) & !occupied_bb;
        let my_candidates: Vec<(usize, usize)> = BitIter(cand_bb)
            .map(|idx| (IDX_X[idx], IDX_Y[idx]))
            .collect();
        let pos0_in = my_candidates.contains(&pos0);
        let my_candidates = if pos0_in {
            my_candidates
        } else {
            let mut c = my_candidates;
            c.push(pos0);
            c
        };
        if my_candidates.is_empty() {
            return Vec::new();
        }
        if my_candidates.len() <= top_k.max(1)
            && (!use_cut_targets
                || (self.config.n_cut_targets == 0
                    && self.config.n_leader_targets == 0
                    && self.config.n_rival_targets == 0))
        {
            return my_candidates;
        }
        let mut cut_bonus = [[0.0f64; N]; N];
        let mut cut_targets: Vec<(usize, usize)> = Vec::new();
        let mut leader_bonus = [[0.0f64; N]; N];
        let mut leader_targets: Vec<(usize, usize)> = Vec::new();
        let mut rival_bonus = [[0.0f64; N]; N];
        let mut rival_targets: Vec<(usize, usize)> = Vec::new();
        let mut opp_rank: Vec<usize> = (1..state.m).collect();
        opp_rank.sort_unstable_by_key(|&p| std::cmp::Reverse(state.scores[p]));
        opp_rank.truncate(self.config.n_pressure_players.max(1).min(opp_rank.len()));
        let leader = opp_rank.first().copied().unwrap_or(0);
        if state.m >= 2 && self.config.n_leader_targets > 0 {
            let leader_pred = self.param_greedy_move(state, leader, params_all[leader]);
            let mut leader_scored: Vec<((usize, usize), f64)> = my_candidates
                .iter()
                .copied()
                .map(|mv| (mv, self.leader_pressure_gain(state, mv, leader, leader_pred)))
                .filter(|&(_, s)| s > 0.0)
                .collect();
            leader_scored.sort_unstable_by(|a, b| {
                b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal)
            });
            for &((x, y), s) in leader_scored.iter().take(self.config.n_leader_targets) {
                leader_bonus[x][y] = s;
                leader_targets.push((x, y));
            }
        }
        if opp_rank.len() >= 2 && self.config.n_rival_targets > 0 {
            let mut rival_scored: Vec<((usize, usize), f64)> = my_candidates
                .iter()
                .copied()
                .map(|mv| {
                    let mut s = 0.0;
                    for &rival in opp_rank.iter().skip(1) {
                        let rival_pred = self.param_greedy_move(state, rival, params_all[rival]);
                        s += self.rival_pressure_gain(state, mv, rival, rival_pred);
                    }
                    (mv, s)
                })
                .filter(|&(_, s)| s > 0.0)
                .collect();
            rival_scored.sort_unstable_by(|a, b| {
                b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal)
            });
            for &((x, y), s) in rival_scored.iter().take(self.config.n_rival_targets) {
                rival_bonus[x][y] = s;
                rival_targets.push((x, y));
            }
        }
        if use_cut_targets && self.config.n_cut_targets > 0 && state.m >= 2 {
            let leader = (1..state.m).max_by_key(|&p| state.scores[p]).unwrap_or(1);
            let cuts = find_cut_targets(state, &self.v, leader, &my_candidates);
            for &((x, y), cut_val) in cuts.iter().take(self.config.n_cut_targets) {
                cut_bonus[x][y] = cut_val.sqrt() * 2.0;
                cut_targets.push((x, y));
            }
        }
        let mut scored: Vec<((usize, usize), f64)> = my_candidates
            .iter()
            .copied()
            .map(|mv| {
                let base = self.move_gain_p0(state, mv);
                let pressure =
                    cut_bonus[mv.0][mv.1] + leader_bonus[mv.0][mv.1] + rival_bonus[mv.0][mv.1];
                let chase = if state.m >= 2 {
                    self.leader_direction_gain(state, mv, leader)
                } else {
                    0.0
                };
                let anti_escape = self.anti_escape_gain(state, mv);
                (mv, base + pressure + chase + anti_escape)
            })
            .collect();
        scored.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        let mut out: Vec<(usize, usize)> = scored
            .iter()
            .take(top_k.max(1).min(scored.len()))
            .map(|&(mv, _)| mv)
            .collect();
        if use_cut_targets && !opp_rank.is_empty() {
            let gap = state.scores[opp_rank[0]] - state.scores[0];
            if gap >= self.config.pressure_trigger_gap {
                let need_pressure = (top_k.max(1) / 2).max(1);
                let mut pressure_pref: Vec<(usize, usize)> = scored
                    .iter()
                    .filter_map(|&(mv, _)| {
                        if self.is_pressure_move(state, mv, &opp_rank) {
                            Some(mv)
                        } else {
                            None
                        }
                    })
                    .collect();
                pressure_pref.truncate(need_pressure.min(pressure_pref.len()));
                let mut merged = pressure_pref;
                for &(mv, _) in &scored {
                    if merged.len() >= top_k.max(1) {
                        break;
                    }
                    if !merged.contains(&mv) {
                        merged.push(mv);
                    }
                }
                if !merged.is_empty() {
                    out = merged;
                }
            }
        }
        let extra_cap = top_k.max(1)
            + if use_cut_targets { self.config.n_cut_targets } else { 0 }
            + self.config.n_leader_targets
            + self.config.n_rival_targets;
        if use_cut_targets {
            for &(x, y) in &cut_targets {
                if out.len() >= extra_cap {
                    break;
                }
                if !out.contains(&(x, y)) {
                    out.push((x, y));
                }
            }
        }
        for &(x, y) in &leader_targets {
            if out.len() >= extra_cap {
                break;
            }
            if !out.contains(&(x, y)) {
                out.push((x, y));
            }
        }
        for &(x, y) in &rival_targets {
            if out.len() >= extra_cap {
                break;
            }
            if !out.contains(&(x, y)) {
                out.push((x, y));
            }
        }
        out
    }
    fn potential_phi(&self, state: &SimState) -> f64 {
        let m = state.m;
        let alpha = self.config.phi_frontier_w;
        let beta = self.config.phi_territory_w;
        let gamma = self.config.phi_upgrade_w;
        const BOARD_MASK: u128 = (1u128 << 100) - 1;
        let s0 = state.scores[0].max(0) as f64;
        let sa = state.scores[1..m].iter().copied().max().unwrap_or(0).max(0) as f64;
        let mut owned_union = 0u128;
        let mut territory = [0usize; MAX_M];
        for p in 0..m {
            let own = state.owned_bb[p];
            owned_union |= own;
            territory[p] = own.count_ones() as usize;
        }
        let unowned = BOARD_MASK & !owned_union;
        let mut frontier = [0.0f64; MAX_M];
        for p in 0..m {
            let mask = bb_expand(state.owned_bb[p]) & unowned;
            for idx in BitIter(mask) {
                frontier[p] += self.v[IDX_X[idx]][IDX_Y[idx]] as f64;
            }
        }
        let f0 = frontier[0];
        let fa = frontier[1..m].iter().copied().fold(0.0f64, f64::max);
        let t0 = territory[0] as f64;
        let ta = *territory[1..m].iter().max().unwrap_or(&1) as f64;
        let mut upgrade_pot = [0.0f64; MAX_M];
        if gamma > 0.0 {
            let u_cap = state.u_cap as usize;
            for p in 0..m {
                for idx in BitIter(state.owned_bb[p]) {
                    let rem = u_cap.saturating_sub(state.level[IDX_X[idx]][IDX_Y[idx]] as usize);
                    if rem > 0 {
                        upgrade_pot[p] += self.v[IDX_X[idx]][IDX_Y[idx]] as f64 * rem as f64;
                    }
                }
            }
        }
        let u0 = upgrade_pot[0];
        let ua = upgrade_pot[1..m].iter().copied().fold(0.0f64, f64::max);
        let adj_s0 = s0 + alpha * f0 + beta * t0 + gamma * u0;
        let adj_sa = sa + alpha * fa + beta * ta + gamma * ua;
        if adj_sa > 0.0 {
            (1.0 + adj_s0 / adj_sa).ln()
        } else {
            (1.0 + adj_s0).ln()
        }
    }
    fn prior_score_for_move(
        &self,
        state: &SimState,
        mv: (usize, usize),
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
    ) -> f64 {
        let mut score = self.move_gain_p0(state, mv);
        if state.m < 2 {
            return score;
        }
        let mut opp_rank: Vec<usize> = (1..state.m).collect();
        opp_rank.sort_unstable_by_key(|&p| Reverse(state.scores[p]));
        opp_rank.truncate(self.config.n_pressure_players.max(1).min(opp_rank.len()));
        if opp_rank.is_empty() {
            return score;
        }
        let leader = opp_rank[0];
        let leader_pred = self.param_greedy_move(state, leader, params_all[leader]);
        score += self.leader_pressure_gain(state, mv, leader, leader_pred);
        score += self.leader_direction_gain(state, mv, leader);
        score += self.anti_escape_gain(state, mv);
        for &rival in opp_rank.iter().skip(1) {
            let rival_pred = self.param_greedy_move(state, rival, params_all[rival]);
            score += 0.5 * self.rival_pressure_gain(state, mv, rival, rival_pred);
        }
        score
    }
    fn action_priors(
        &self,
        state: &SimState,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
        is_root: bool,
    ) -> Vec<((usize, usize), f64)> {
        let top_k = if is_root {
            self.config.root_top_k
        } else {
            self.config.node_top_k
        };
        let cands = self.select_candidates(state, top_k, is_root, params_all);
        if cands.is_empty() {
            return Vec::new();
        }
        if cands.len() == 1 {
            return vec![(cands[0], 1.0)];
        }
        let mut scored = Vec::with_capacity(cands.len());
        let mut max_s = f64::NEG_INFINITY;
        for mv in cands {
            let s = self.prior_score_for_move(state, mv, params_all);
            max_s = max_s.max(s);
            scored.push((mv, s));
        }
        let beta = self.config.mcts_prior_beta.max(0.0);
        let mut sum_w = 0.0f64;
        let mut priors = Vec::with_capacity(scored.len());
        for (i, (mv, s)) in scored.into_iter().enumerate() {
            let w = if beta > 0.0 {
                let z = ((s - max_s) * beta).clamp(-60.0, 0.0);
                z.exp()
            } else {
                1.0 / (i + 1) as f64
            };
            priors.push((mv, w));
            sum_w += w;
        }
        if sum_w <= 1e-12 {
            let u = 1.0 / priors.len() as f64;
            for item in &mut priors {
                item.1 = u;
            }
            return priors;
        }
        for item in &mut priors {
            item.1 /= sum_w;
        }
        priors
    }
    fn sample_scenario(
        &self,
        state: &SimState,
        estimator: &E,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
        rng: &mut Pcg64Mcg,
    ) -> [(usize, usize); MAX_M] {
        let scenarios = self.build_scenarios(state, estimator, params_all);
        if scenarios.is_empty() {
            let mut moves = [(0usize, 0usize); MAX_M];
            for (p, slot) in moves.iter_mut().enumerate().take(state.m).skip(1) {
                *slot = self.param_greedy_move(state, p, params_all[p]);
            }
            return moves;
        }
        let r = rng.gen_range(0.0..1.0);
        let mut acc = 0.0;
        for sc in &scenarios {
            acc += sc.prob;
            if r <= acc {
                return sc.moves;
            }
        }
        scenarios.last().map(|s| s.moves).unwrap_or([(0usize, 0usize); MAX_M])
    }
    fn apply_transition(
        &self,
        state: &SimState,
        action: (usize, usize),
        _estimator: &E,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
        rng: &mut Pcg64Mcg,
    ) -> SimState {
        let m = state.m;
        let mut moves = [(0usize, 0usize); MAX_M];
        let mut occupied_bb = 0u128;
        for i in 0..m {
            let (px, py) = state.positions[i];
            occupied_bb |= bb_idx(px, py);
        }
        let mut owned_union = 0u128;
        for i in 0..m {
            owned_union |= state.owned_bb[i];
        }
        let connected_bb = compute_connected_bb(state);
        let pos0 = state.positions[0];
        let pos0_bit = bb_idx(pos0.0, pos0.1);
        let p0_cands = (connected_bb[0] | bb_expand(connected_bb[0]))
            & (!occupied_bb | pos0_bit);
        let action_bit = bb_idx(action.0, action.1);
        moves[0] = if (p0_cands & action_bit) != 0 { action } else { pos0 };
        for p in 1..m {
            let (wa, wb, wc, wd, eps) = params_all[p];
            if rng.gen_range(0.0..1.0) < eps {
                moves[p] = random_move_bb(state, p, occupied_bb, connected_bb[p], rng);
            } else {
                moves[p] = predict_move_ai_bb(
                    state, &self.v, p, wa, wb, wc, wd,
                    occupied_bb, owned_union, connected_bb[p], rng,
                );
            }
        }
        let mut next = state.clone();
        next.simulate_turn(&moves[..m], &self.v);
        next
    }
    fn rollout_with_connected(
        &self,
        state: &SimState,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
        rng: &mut Pcg64Mcg,
        depth: usize,
        init_connected_bb: [u128; MAX_M],
    ) -> f64 {
        let mut sim = state.clone();
        let mut moves = [(0usize, 0usize); MAX_M];
        let m = sim.m;
        let mut connected_bb = init_connected_bb;
        for _ in 0..depth {
            let mut occupied_bb = 0u128;
            for i in 0..m {
                let (px, py) = sim.positions[i];
                occupied_bb |= bb_idx(px, py);
            }
            let mut owned_union = 0u128;
            for i in 0..m {
                owned_union |= sim.owned_bb[i];
            }
            if rng.gen_range(0.0..1.0) < 0.2 {
                moves[0] = random_move_bb(&sim, 0, occupied_bb, connected_bb[0], rng);
            } else {
                let leader = (1..m).max_by_key(|&p| sim.scores[p]).unwrap_or(1);
                let (lwa, lwb, lwc, lwd, _) = params_all[leader];
                let leader_target = predict_leader_greedy_target(
                    &sim, &self.v, leader, lwa, lwb, lwc, lwd,
                    occupied_bb, owned_union, connected_bb[leader],
                );
                moves[0] = predict_move_p0_bb(
                    &sim, &self.v, occupied_bb, owned_union,
                    connected_bb[0], Some(leader_target),
                );
            }
            for p in 1..m {
                let (wa, wb, wc, wd, eps) = params_all[p];
                if rng.gen_range(0.0..1.0) < eps {
                    moves[p] = random_move_bb(&sim, p, occupied_bb, connected_bb[p], rng);
                } else {
                    moves[p] = predict_move_ai_bb(
                        &sim, &self.v, p, wa, wb, wc, wd,
                        occupied_bb, owned_union, connected_bb[p], rng,
                    );
                }
            }
            let old_owned = sim.owned_bb;
            sim.simulate_turn(&moves[..m], &self.v);
            for p in 0..m {
                if sim.owned_bb[p] != old_owned[p] {
                    connected_bb[p] = update_connected_after_owned_change(
                        connected_bb[p], old_owned[p], sim.owned_bb[p], sim.positions[p],
                    );
                }
            }
        }
        self.potential_phi(&sim)
    }
    fn mcts_search(
        &self,
        root_state: &SimState,
        estimator: &E,
        params_all: &[(f64, f64, f64, f64, f64); MAX_M],
        max_depth: usize,
        deadline: Instant,
        target_iters: usize,
    ) -> ((usize, usize), usize) {
        let mut seed = 1469598103934665603u64;
        for p in 0..root_state.m {
            let (x, y) = root_state.positions[p];
            seed ^= (x as u64) << 32 ^ y as u64 ^ (root_state.scores[p] as u64).wrapping_mul(109951);
            seed = seed.wrapping_mul(1099511628211);
        }
        let mut rng = Pcg64Mcg::seed_from_u64(seed);
        let cands = self.select_candidates(root_state, self.config.root_top_k, true, params_all);
        if cands.is_empty() {
            return (root_state.positions[0], 0);
        }
        if cands.len() == 1 {
            return (cands[0], 0);
        }
        let fixed = if self.config.mcts_fixed_iters > 0 {
            self.config.mcts_fixed_iters
        } else {
            target_iters
        };
        let mut best_action = cands[0];
        let mut total_iters = 0usize;
        let root_connected_bb = compute_connected_bb(root_state);
        let m = root_state.m;
        let mut root_occupied_bb = 0u128;
        for i in 0..m {
            let (px, py) = root_state.positions[i];
            root_occupied_bb |= bb_idx(px, py);
        }
        let mut root_owned_union = 0u128;
        for i in 0..m {
            root_owned_union |= root_state.owned_bb[i];
        }
        let n_cands = cands.len();
        let screen_budget = (fixed / 4).max(n_cands); // 25%をscreenに
        let screen_per = (screen_budget / n_cands).max(1);
        let mut screen_scores: Vec<(f64, usize)> = Vec::with_capacity(n_cands);
        for (ci, &action) in cands.iter().enumerate() {
            let mut sum = 0.0f64;
            for _ in 0..screen_per {
                let mut sampled_params = *params_all;
                if !self.use_oracle {
                    for p in 1..m {
                        sampled_params[p] = estimator.sample_params(p, &mut rng);
                    }
                }
                let mut moves = [(0usize, 0usize); MAX_M];
                moves[0] = action;
                for p in 1..m {
                    let (wa, wb, wc, wd, eps) = sampled_params[p];
                    if rng.gen_range(0.0..1.0) < eps {
                        moves[p] = random_move_bb(root_state, p, root_occupied_bb, root_connected_bb[p], &mut rng);
                    } else {
                        moves[p] = predict_move_ai_bb(
                            root_state, &self.v, p, wa, wb, wc, wd,
                            root_occupied_bb, root_owned_union, root_connected_bb[p], &mut rng,
                        );
                    }
                }
                let mut sim = root_state.clone();
                let old_owned = sim.owned_bb;
                sim.simulate_turn(&moves[..m], &self.v);
                let mut connected_bb = root_connected_bb;
                for p in 0..m {
                    if sim.owned_bb[p] != old_owned[p] {
                        connected_bb[p] = update_connected_after_owned_change(
                            connected_bb[p], old_owned[p], sim.owned_bb[p], sim.positions[p],
                        );
                    }
                }
                let value = if max_depth > 1 {
                    self.rollout_with_connected(&sim, &sampled_params, &mut rng, max_depth - 1, connected_bb)
                } else {
                    self.potential_phi(&sim)
                };
                sum += value;
                total_iters += 1;
            }
            screen_scores.push((sum / screen_per as f64, ci));
        }
        screen_scores.sort_unstable_by(|a, b| b.0.partial_cmp(&a.0).unwrap_or(std::cmp::Ordering::Equal));
        let keep = (n_cands / 2).max(2).min(n_cands);
        let top_cands: Vec<(usize, usize)> = screen_scores.iter()
            .take(keep)
            .map(|&(_, ci)| cands[ci])
            .collect();
        let mc_budget = fixed.saturating_sub(screen_per * n_cands);
        let mc_per = (mc_budget / top_cands.len()).max(1);
        let mut best_avg = f64::NEG_INFINITY;
        for &action in &top_cands {
            let mut sum = 0.0f64;
            for _ in 0..mc_per {
                let mut sampled_params = *params_all;
                if !self.use_oracle {
                    for p in 1..m {
                        sampled_params[p] = estimator.sample_params(p, &mut rng);
                    }
                }
                let mut moves = [(0usize, 0usize); MAX_M];
                moves[0] = action;
                for p in 1..m {
                    let (wa, wb, wc, wd, eps) = sampled_params[p];
                    if rng.gen_range(0.0..1.0) < eps {
                        moves[p] = random_move_bb(root_state, p, root_occupied_bb, root_connected_bb[p], &mut rng);
                    } else {
                        moves[p] = predict_move_ai_bb(
                            root_state, &self.v, p, wa, wb, wc, wd,
                            root_occupied_bb, root_owned_union, root_connected_bb[p], &mut rng,
                        );
                    }
                }
                let mut sim = root_state.clone();
                let old_owned = sim.owned_bb;
                sim.simulate_turn(&moves[..m], &self.v);
                let mut connected_bb = root_connected_bb;
                for p in 0..m {
                    if sim.owned_bb[p] != old_owned[p] {
                        connected_bb[p] = update_connected_after_owned_change(
                            connected_bb[p], old_owned[p], sim.owned_bb[p], sim.positions[p],
                        );
                    }
                }
                let value = if max_depth > 1 {
                    self.rollout_with_connected(&sim, &sampled_params, &mut rng, max_depth - 1, connected_bb)
                } else {
                    self.potential_phi(&sim)
                };
                sum += value;
                total_iters += 1;
            }
            let avg = sum / mc_per as f64;
            if avg > best_avg {
                best_avg = avg;
                best_action = action;
            }
        }
        let _ = deadline;
        (best_action, total_iters)
    }
}
impl<E: Estimator> Solver for SearchSolver<'_, E> {
    fn solve(&mut self) -> Vec<Output> {
        let game_start = Instant::now();
        let total_budget = self.config.total_budget;
        let deterministic_mode = self.config.mc_budget_factor > 0.0;
        let m = self.input.m;
        let t = self.input.t;
        let mut state = GameState::new(self.input);
        let mut estimator = E::new(m);
        let mut outputs = Vec::with_capacity(t);
        for turn in 0..t {
            let turn_start = Instant::now();
            let remaining_turns = (t - turn).max(1);
            let remaining_budget = if deterministic_mode {
                total_budget
            } else {
                (total_budget - game_start.elapsed().as_secs_f64()).max(0.001)
            };
            let per_turn_budget = if deterministic_mode {
                (total_budget / t as f64).max(0.001)
            } else {
                (remaining_budget / remaining_turns as f64).max(0.001)
            };
            let deadline = turn_start + Duration::from_secs_f64(per_turn_budget * 0.92);
            let sim_base = SimState::from_game_state(&state);
            let remaining_steps = (t - turn).max(1);
            let total_depth = if self.config.max_depth == 0 {
                remaining_steps
            } else {
                remaining_steps.min(self.config.max_depth)
            };
            let target_iters = if self.config.mcts_fixed_iters > 0 {
                self.config.mcts_fixed_iters
            } else if deterministic_mode {
                let ops_budget = (per_turn_budget * self.config.mc_budget_factor) as usize;
                let ops_per_iter = (total_depth.max(1) * m.max(2) * 3).max(1);
                (ops_budget / ops_per_iter).max(self.config.mcts_min_iters)
            } else {
                0
            };
            let params_all = self.oracle_params_or_estimated(&estimator, m);
            let ((tx, ty), eval_nodes) = self.mcts_search(
                &sim_base,
                &estimator,
                &params_all,
                total_depth,
                deadline,
                target_iters,
            );
            #[cfg(not(feature = "debug"))]
            let _ = eval_nodes;
            let output = Output { tx, ty };
            self.io.write(&output);
            outputs.push(output);
            let turn_input = self.io.read_turn(m, self.input.n);
            for p in 1..m {
                estimator.observe(&sim_base, &self.v, p, turn_input.targets[p]);
            }
            #[cfg(feature = "debug")]
            eprintln!(
                "SEARCH turn={} depth={} nodes={} target_iters={} elapsed_ms={:.2}",
                turn,
                total_depth,
                eval_nodes,
                target_iters,
                turn_start.elapsed().as_secs_f64() * 1000.0
            );
            state.update(&turn_input);
        }
        outputs
    }
}
}
pub mod sim {
use std::env;
use std::time::Instant;
use rand::Rng;
use rand::SeedableRng;
use rand_pcg::Pcg64Mcg;
use super::super::game::GameState;
#[cfg(test)]
use super::super::game::{BitIter, bb_expand, bb_flood_fill, bb_idx};
use super::super::game::{MAX_M, N, SimState};
use super::super::io::{IO, Input, Output};
use super::super::strategy::Estimator;
use super::super::strategy::cut_target::find_cut_targets;
use super::super::strategy::move_predict::{compute_connected_bb, predict_move_ai};
use super::Solver;
pub mod rollout {
use rand::Rng;
use rand_pcg::Pcg64Mcg;
#[cfg(feature = "debug")]
use std::time::Instant;
use super::super::super::game::{BitIter, MAX_M, N, SimState, bb_expand, bb_idx};
use super::super::super::strategy::move_predict::{    IDX_X, IDX_Y, compute_connected_bb, predict_leader_greedy_target, predict_move_ai_bb,    predict_move_p0_bb, random_move_bb, update_connected_after_owned_change,};
#[cfg(feature = "debug")]
use super::PROF;
pub fn rollout_det(
    state: &SimState,
    v: &[[usize; N]; N],
    depth: usize,
    ai_params: &[(f64, f64, f64, f64); MAX_M],
    rng: &mut Pcg64Mcg,
    eval_alpha: f64,
    eval_beta: f64,
    eval_gamma: f64,
) -> f64 {
    let mut sim = state.clone();
    let mut moves = [(0usize, 0usize); MAX_M];
    #[cfg(feature = "debug")]
    let tf0 = Instant::now();
    let mut connected_bb = compute_connected_bb(&sim);
    #[cfg(feature = "debug")]
    PROF.with(|p| {
        let mut p = p.borrow_mut();
        p.flood_ns += tf0.elapsed().as_nanos() as u64;
        p.flood_count += sim.m as u64;
    });
    for _ in 0..depth {
        let mut occupied_bb = 0u128;
        for i in 0..sim.m {
            let (px, py) = sim.positions[i];
            occupied_bb |= bb_idx(px, py);
        }
        let mut owned_union = 0u128;
        for i in 0..sim.m {
            owned_union |= sim.owned_bb[i];
        }
        let leader = (1..sim.m).max_by_key(|&p| sim.scores[p]).unwrap_or(1);
        let (lwa, lwb, lwc, lwd) = ai_params[leader];
        let leader_target = predict_leader_greedy_target(
            &sim,
            v,
            leader,
            lwa,
            lwb,
            lwc,
            lwd,
            occupied_bb,
            owned_union,
            connected_bb[leader],
        );
        moves[0] = predict_move_p0_bb(
            &sim,
            v,
            occupied_bb,
            owned_union,
            connected_bb[0],
            Some(leader_target),
        );
        for p in 1..sim.m {
            let (wa, wb, wc, wd) = ai_params[p];
            moves[p] = predict_move_ai_bb(
                &sim,
                v,
                p,
                wa,
                wb,
                wc,
                wd,
                occupied_bb,
                owned_union,
                connected_bb[p],
                rng,
            );
        }
        let old_owned = sim.owned_bb;
        sim.simulate_turn(&moves[..sim.m], v);
        #[cfg(feature = "debug")]
        let tf = Instant::now();
        #[cfg(feature = "debug")]
        let mut flood_recalc = 0u64;
        for p in 0..sim.m {
            if sim.owned_bb[p] != old_owned[p] {
                connected_bb[p] = update_connected_after_owned_change(
                    connected_bb[p],
                    old_owned[p],
                    sim.owned_bb[p],
                    sim.positions[p],
                );
                #[cfg(feature = "debug")]
                {
                    flood_recalc += 1;
                }
            }
        }
        #[cfg(feature = "debug")]
        PROF.with(|p| {
            let mut p = p.borrow_mut();
            p.flood_ns += tf.elapsed().as_nanos() as u64;
            p.flood_count += flood_recalc;
        });
    }
    eval_score(&sim, v, eval_alpha, eval_beta, eval_gamma)
}
pub fn rollout_noisy_with_params(
    state: &SimState,
    init_connected_bb: &[u128; MAX_M],
    v: &[[usize; N]; N],
    depth: usize,
    sampled: &[(f64, f64, f64, f64, f64); MAX_M],
    rng: &mut Pcg64Mcg,
    p0_eps: f64,
    eval_alpha: f64,
    eval_beta: f64,
    eval_gamma: f64,
) -> f64 {
    #[cfg(feature = "debug")]
    let t0 = Instant::now();
    let mut sim = state.clone();
    #[cfg(feature = "debug")]
    PROF.with(|p| {
        let mut p = p.borrow_mut();
        p.clone_ns += t0.elapsed().as_nanos() as u64;
        p.clone_count += 1;
    });
    let mut moves = [(0usize, 0usize); MAX_M];
    let mut connected_bb = *init_connected_bb;
    for _ in 0..depth {
        let mut occupied_bb = 0u128;
        for i in 0..sim.m {
            let (px, py) = sim.positions[i];
            occupied_bb |= bb_idx(px, py);
        }
        let mut owned_union = 0u128;
        for i in 0..sim.m {
            owned_union |= sim.owned_bb[i];
        }
        #[cfg(feature = "debug")]
        let tb = Instant::now();
        let leader = (1..sim.m).max_by_key(|&p| sim.scores[p]).unwrap_or(1);
        let (lwa, lwb, lwc, lwd, _leps) = sampled[leader];
        let leader_target = predict_leader_greedy_target(
            &sim,
            v,
            leader,
            lwa,
            lwb,
            lwc,
            lwd,
            occupied_bb,
            owned_union,
            connected_bb[leader],
        );
        if rng.gen_range(0.0..1.0) < p0_eps {
            #[cfg(feature = "debug")]
            let tr = Instant::now();
            moves[0] = random_move_bb(&sim, 0, occupied_bb, connected_bb[0], rng);
            #[cfg(feature = "debug")]
            PROF.with(|p| {
                let mut p = p.borrow_mut();
                p.move_rand_ns += tr.elapsed().as_nanos() as u64;
                p.move_rand_count += 1;
            });
        } else {
            #[cfg(feature = "debug")]
            let td = Instant::now();
            moves[0] = predict_move_p0_bb(
                &sim,
                v,
                occupied_bb,
                owned_union,
                connected_bb[0],
                Some(leader_target),
            );
            #[cfg(feature = "debug")]
            PROF.with(|p| {
                let mut p = p.borrow_mut();
                p.move_det_ns += td.elapsed().as_nanos() as u64;
                p.move_det_count += 1;
            });
        }
        for (p, slot) in moves.iter_mut().enumerate().take(sim.m).skip(1) {
            let (wa, wb, wc, wd, eps) = sampled[p];
            if rng.gen_range(0.0..1.0) < eps {
                #[cfg(feature = "debug")]
                let tr = Instant::now();
                *slot = random_move_bb(&sim, p, occupied_bb, connected_bb[p], rng);
                #[cfg(feature = "debug")]
                PROF.with(|pp| {
                    let mut pp = pp.borrow_mut();
                    pp.move_rand_ns += tr.elapsed().as_nanos() as u64;
                    pp.move_rand_count += 1;
                });
            } else {
                #[cfg(feature = "debug")]
                let td = Instant::now();
                *slot = predict_move_ai_bb(
                    &sim,
                    v,
                    p,
                    wa,
                    wb,
                    wc,
                    wd,
                    occupied_bb,
                    owned_union,
                    connected_bb[p],
                    rng,
                );
                #[cfg(feature = "debug")]
                PROF.with(|pp| {
                    let mut pp = pp.borrow_mut();
                    pp.move_det_ns += td.elapsed().as_nanos() as u64;
                    pp.move_det_count += 1;
                });
            }
        }
        #[cfg(feature = "debug")]
        PROF.with(|p| {
            let mut p = p.borrow_mut();
            p.bfs_ns += tb.elapsed().as_nanos() as u64;
            p.bfs_count += sim.m as u64;
        });
        let old_owned = sim.owned_bb;
        #[cfg(feature = "debug")]
        let ts = Instant::now();
        sim.simulate_turn(&moves[..sim.m], v);
        #[cfg(feature = "debug")]
        PROF.with(|p| {
            let mut p = p.borrow_mut();
            p.sim_turn_ns += ts.elapsed().as_nanos() as u64;
            p.sim_turn_count += 1;
        });
        #[cfg(feature = "debug")]
        let tf = Instant::now();
        #[cfg(feature = "debug")]
        let mut flood_recalc = 0u64;
        for p in 0..sim.m {
            if sim.owned_bb[p] != old_owned[p] {
                connected_bb[p] = update_connected_after_owned_change(
                    connected_bb[p],
                    old_owned[p],
                    sim.owned_bb[p],
                    sim.positions[p],
                );
                #[cfg(feature = "debug")]
                {
                    flood_recalc += 1;
                }
            }
        }
        #[cfg(feature = "debug")]
        PROF.with(|p| {
            let mut p = p.borrow_mut();
            p.flood_ns += tf.elapsed().as_nanos() as u64;
            p.flood_count += flood_recalc;
        });
    }
    #[cfg(feature = "debug")]
    let te = Instant::now();
    let result = eval_score(&sim, v, eval_alpha, eval_beta, eval_gamma);
    #[cfg(feature = "debug")]
    PROF.with(|p| {
        let mut p = p.borrow_mut();
        p.eval_ns += te.elapsed().as_nanos() as u64;
        p.eval_count += 1;
    });
    result
}
pub fn eval_score(state: &SimState, v: &[[usize; N]; N], alpha: f64, beta: f64, gamma: f64) -> f64 {
    let m = state.m;
    const BOARD_MASK: u128 = (1u128 << 100) - 1;
    let s0 = state.scores[0] as f64;
    let sa = state.scores[1..m].iter().copied().max().unwrap_or(0) as f64;
    let mut frontier = [0.0f64; MAX_M];
    let mut territory = [0usize; MAX_M];
    let mut owned_union = 0u128;
    for p in 0..m {
        let own = state.owned_bb[p];
        owned_union |= own;
        territory[p] = own.count_ones() as usize;
    }
    let unowned = BOARD_MASK & !owned_union;
    for p in 0..m {
        let mask = bb_expand(state.owned_bb[p]) & unowned;
        let mut sum = 0.0f64;
        for idx in BitIter(mask) {
            sum += v[IDX_X[idx]][IDX_Y[idx]] as f64;
        }
        frontier[p] = sum;
    }
    let f0 = frontier[0];
    let fa = frontier[1..m].iter().copied().fold(0.0f64, f64::max);
    let t0 = territory[0] as f64;
    let ta = *territory[1..m].iter().max().unwrap_or(&1) as f64;
    let mut upgrade_pot = [0.0f64; MAX_M];
    if gamma > 0.0 {
        let u_cap = state.u_cap as usize;
        for p in 0..m {
            for idx in BitIter(state.owned_bb[p]) {
                let remaining = u_cap - state.level[IDX_X[idx]][IDX_Y[idx]] as usize;
                if remaining > 0 {
                    upgrade_pot[p] += v[IDX_X[idx]][IDX_Y[idx]] as f64 * remaining as f64;
                }
            }
        }
    }
    let u0 = upgrade_pot[0];
    let ua = upgrade_pot[1..m].iter().copied().fold(0.0f64, f64::max);
    let adj_s0 = s0 + alpha * f0 + beta * t0 + gamma * u0;
    let adj_sa = sa + alpha * fa + beta * ta + gamma * ua;
    if adj_sa > 0.0 {
        adj_s0 / adj_sa
    } else {
        adj_s0 + 1.0
    }
}
}
use rollout::{rollout_det, rollout_noisy_with_params};
#[cfg(feature = "debug")]
use std::cell::RefCell;
#[cfg(feature = "debug")]
thread_local! {
    pub static PROF: RefCell<ProfCounters> = RefCell::new(ProfCounters::default());
}
#[cfg(feature = "debug")]
#[derive(Default)]
pub struct ProfCounters {
    pub bfs_ns: u64,
    pub bfs_count: u64,
    pub flood_ns: u64,
    pub flood_count: u64,
    pub move_det_ns: u64,
    pub move_det_count: u64,
    pub move_rand_ns: u64,
    pub move_rand_count: u64,
    pub sim_turn_ns: u64,
    pub sim_turn_count: u64,
    pub eval_ns: u64,
    pub eval_count: u64,
    pub clone_ns: u64,
    pub clone_count: u64,
    pub sample_ns: u64,
    pub sample_count: u64,
}
#[cfg(feature = "debug")]
impl ProfCounters {
    pub fn reset(&mut self) {
        *self = Self::default();
    }
    pub fn report(&self) {
        let total = self.bfs_ns
            + self.flood_ns
            + self.sim_turn_ns
            + self.eval_ns
            + self.clone_ns
            + self.sample_ns;
        eprintln!("=== PROFILE (total {:.1}ms) ===", total as f64 / 1e6);
        eprintln!(
            "  BFS:       {:.1}ms ({} calls, {:.0}ns/call)",
            self.bfs_ns as f64 / 1e6,
            self.bfs_count,
            self.bfs_ns as f64 / self.bfs_count.max(1) as f64
        );
        eprintln!(
            "    det:     {:.1}ms ({} calls, {:.0}ns/call)",
            self.move_det_ns as f64 / 1e6,
            self.move_det_count,
            self.move_det_ns as f64 / self.move_det_count.max(1) as f64
        );
        eprintln!(
            "    rand:    {:.1}ms ({} calls, {:.0}ns/call)",
            self.move_rand_ns as f64 / 1e6,
            self.move_rand_count,
            self.move_rand_ns as f64 / self.move_rand_count.max(1) as f64
        );
        eprintln!(
            "  flood:     {:.1}ms ({} calls, {:.0}ns/call)",
            self.flood_ns as f64 / 1e6,
            self.flood_count,
            self.flood_ns as f64 / self.flood_count.max(1) as f64
        );
        eprintln!(
            "  sim_turn:  {:.1}ms ({} calls, {:.0}ns/call)",
            self.sim_turn_ns as f64 / 1e6,
            self.sim_turn_count,
            self.sim_turn_ns as f64 / self.sim_turn_count.max(1) as f64
        );
        eprintln!(
            "  eval:      {:.1}ms ({} calls, {:.0}ns/call)",
            self.eval_ns as f64 / 1e6,
            self.eval_count,
            self.eval_ns as f64 / self.eval_count.max(1) as f64
        );
        eprintln!(
            "  clone:     {:.1}ms ({} calls, {:.0}ns/call)",
            self.clone_ns as f64 / 1e6,
            self.clone_count,
            self.clone_ns as f64 / self.clone_count.max(1) as f64
        );
        eprintln!(
            "  sample:    {:.1}ms ({} calls, {:.0}ns/call)",
            self.sample_ns as f64 / 1e6,
            self.sample_count,
            self.sample_ns as f64 / self.sample_count.max(1) as f64
        );
    }
}
pub struct SimConfig {
    pub max_depth: usize,
    pub total_budget: f64,
    pub n_cut_targets: usize,
    pub screen_top_k: usize,
    pub screen_scenarios: usize,
    pub mc_rounds: usize,
    pub mc_budget_factor: f64,
    pub mc_time_ratio: f64,
    pub n_det_samples: usize,
    pub prob_scenarios: usize,
    pub eval_alpha: f64,
    pub eval_beta: f64,
    pub p0_eps: f64,
    pub eval_gamma: f64,
}
impl Default for SimConfig {
    fn default() -> Self {
        Self {
            max_depth: 12,
            total_budget: 1.85,
            n_cut_targets: 3,
            screen_top_k: 10,
            screen_scenarios: 2,
            mc_rounds: 50,
            mc_budget_factor: 0.0,
            mc_time_ratio: 0.85,
            n_det_samples: 1,
            prob_scenarios: 0,
            eval_alpha: 0.15,
            eval_beta: 100.0,
            p0_eps: 0.2,
            eval_gamma: 0.0,
        }
    }
}
impl SimConfig {
    pub fn from_env() -> Self {
        let mut config = Self::default();
        if let Ok(v) = env::var("SIM_MAX_DEPTH") {
            config.max_depth = v.parse().unwrap_or(config.max_depth);
        }
        if let Ok(v) = env::var("SIM_BUDGET") {
            config.total_budget = v.parse().unwrap_or(config.total_budget);
        }
        if let Ok(v) = env::var("SIM_N_CUT_TARGETS") {
            config.n_cut_targets = v.parse().unwrap_or(config.n_cut_targets);
        }
        if let Ok(v) = env::var("SIM_TOP_K") {
            config.screen_top_k = v.parse().unwrap_or(config.screen_top_k);
        }
        if let Ok(v) = env::var("SIM_SCREEN_SCENARIOS") {
            config.screen_scenarios = v.parse().unwrap_or(config.screen_scenarios);
        }
        if let Ok(v) = env::var("SIM_MC_ROUNDS") {
            config.mc_rounds = v.parse().unwrap_or(config.mc_rounds);
        }
        if let Ok(v) = env::var("SIM_MC_BUDGET_FACTOR") {
            config.mc_budget_factor = v.parse().unwrap_or(config.mc_budget_factor);
        }
        if let Ok(v) = env::var("SIM_N_DET_SAMPLES") {
            config.n_det_samples = v.parse().unwrap_or(config.n_det_samples);
        }
        if let Ok(v) = env::var("SIM_PROB_SCENARIOS") {
            config.prob_scenarios = v.parse().unwrap_or(config.prob_scenarios);
        }
        if let Ok(v) = env::var("SIM_EVAL_ALPHA") {
            config.eval_alpha = v.parse().unwrap_or(config.eval_alpha);
        }
        if let Ok(v) = env::var("SIM_EVAL_BETA") {
            config.eval_beta = v.parse().unwrap_or(config.eval_beta);
        }
        if let Ok(v) = env::var("SIM_P0_EPS") {
            config.p0_eps = v.parse().unwrap_or(config.p0_eps);
        }
        if let Ok(v) = env::var("SIM_EVAL_GAMMA") {
            config.eval_gamma = v.parse().unwrap_or(config.eval_gamma);
        }
        config
    }
}
pub struct SimSolver<'a, E: Estimator> {
    input: &'a Input,
    io: &'a mut IO,
    config: SimConfig,
    v: [[usize; N]; N],
    _phantom: std::marker::PhantomData<E>,
}
impl<E: Estimator> SimSolver<'_, E> {
    pub fn new<'a>(input: &'a Input, io: &'a mut IO) -> SimSolver<'a, E> {
        let mut v = [[0usize; N]; N];
        for (i, row) in v.iter_mut().enumerate().take(input.n) {
            row[..input.n].copy_from_slice(&input.v[i][..input.n]);
        }
        SimSolver {
            input,
            io,
            config: SimConfig::from_env(),
            v,
            _phantom: std::marker::PhantomData,
        }
    }
}
impl<E: Estimator> Solver for SimSolver<'_, E> {
    fn solve(&mut self) -> Vec<Output> {
        let game_start = Instant::now();
        let total_budget = self.config.total_budget;
        let deterministic = self.config.mc_budget_factor > 0.0;
        let m = self.input.m;
        let t = self.input.t;
        let mut state = GameState::new(self.input);
        let mut estimator = E::new(m);
        let mut rng = Pcg64Mcg::seed_from_u64(42);
        let mut outputs = Vec::with_capacity(t);
        for turn in 0..t {
            let turn_start = Instant::now();
            let turn_budget = if deterministic {
                total_budget / t as f64
            } else {
                let elapsed = game_start.elapsed().as_secs_f64();
                let remaining = (total_budget - elapsed).max(0.001);
                remaining / (t - turn) as f64
            };
            let sim_base = SimState::from_game_state(&state);
            let max_depth = if self.config.max_depth == 0 {
                (t - turn).saturating_sub(1)
            } else {
                (t - turn).saturating_sub(1).min(self.config.max_depth)
            };
            let mut ai_params = [(1.0f64, 1.0f64, 1.0f64, 1.0f64); MAX_M];
            for (p, params) in ai_params.iter_mut().enumerate().take(m).skip(1) {
                let (wa, wb, wc, wd, _eps) = estimator.get_params(p);
                *params = (wa, wb, wc, wd);
            }
            let ((tx, ty), _mc_count) = self.mc_search(
                &sim_base,
                &ai_params,
                &estimator,
                &mut rng,
                max_depth,
                turn_budget,
                turn_start,
            );
            let output = Output { tx, ty };
            self.io.write(&output);
            outputs.push(output);
            let turn_input = self.io.read_turn(m, self.input.n);
            for p in 1..m {
                estimator.observe(&sim_base, &self.v, p, turn_input.targets[p]);
            }
            #[cfg(feature = "debug")]
            eprintln!(
                "DEBUG turn={} depth={} mc={} time={:.2}ms budget={:.2}ms",
                turn,
                max_depth,
                _mc_count,
                turn_start.elapsed().as_secs_f64() * 1000.0,
                turn_budget * 1000.0,
            );
            state.update(&turn_input);
        }
        outputs
    }
}
impl<E: Estimator> SimSolver<'_, E> {
    fn mc_search(
        &self,
        sim_base: &SimState,
        ai_params: &[(f64, f64, f64, f64); MAX_M],
        estimator: &E,
        rng: &mut Pcg64Mcg,
        max_depth: usize,
        turn_budget: f64,
        turn_start: Instant,
    ) -> ((usize, usize), usize) {
        let m = sim_base.m;
        #[cfg(feature = "debug")]
        let phase_t0 = Instant::now();
        let my_candidates = sim_base.get_candidates(0);
        let n_cands = my_candidates.len();
        let screen_depth = max_depth.min(4);
        let n_scenarios = self.config.screen_scenarios;
        let mut screen_eval = vec![0.0f64; n_cands];
        let mut predicted_moves = [(0usize, 0usize); MAX_M];
        let mut base_occupied = [[false; N]; N];
        for i in 0..m {
            let (px, py) = sim_base.positions[i];
            base_occupied[px][py] = true;
        }
        for scenario in 0..n_scenarios {
            for (p, slot) in predicted_moves.iter_mut().enumerate().take(m).skip(1) {
                if scenario == 0 {
                    *slot = estimator.predict_move_posterior(sim_base, &self.v, p, rng);
                } else {
                    let (wa, wb, wc, wd, _eps) = estimator.sample_params(p, rng);
                    *slot =
                        predict_move_ai(sim_base, &self.v, p, wa, wb, wc, wd, &base_occupied, rng);
                }
            }
            for (i, &(cx, cy)) in my_candidates.iter().enumerate() {
                predicted_moves[0] = (cx, cy);
                let mut sim = sim_base.clone();
                sim.simulate_turn(&predicted_moves[..m], &self.v);
                screen_eval[i] += rollout_det(
                    &sim,
                    &self.v,
                    screen_depth,
                    ai_params,
                    rng,
                    self.config.eval_alpha,
                    self.config.eval_beta,
                    self.config.eval_gamma,
                );
            }
        }
        for e in screen_eval.iter_mut() {
            *e /= n_scenarios as f64;
        }
        #[cfg(feature = "debug")]
        let phase1_ms = phase_t0.elapsed().as_secs_f64() * 1000.0;
        #[cfg(feature = "debug")]
        let phase_t1 = Instant::now();
        let top_k = self.config.screen_top_k.min(n_cands);
        let mut top_indices: Vec<usize> = (0..n_cands).collect();
        top_indices
            .sort_unstable_by(|&a, &b| screen_eval[b].partial_cmp(&screen_eval[a]).unwrap());
        top_indices.truncate(top_k);
        if self.config.n_cut_targets > 0 {
            let leader = (1..m).max_by_key(|&p| sim_base.scores[p]).unwrap_or(1);
            let cuts = find_cut_targets(sim_base, &self.v, leader, &my_candidates);
            for &((cx, cy), _cut_val) in cuts.iter().take(self.config.n_cut_targets) {
                if let Some(idx) = my_candidates.iter().position(|&c| c == (cx, cy))
                    && !top_indices.contains(&idx)
                {
                    top_indices.push(idx);
                }
            }
        }
        let top_k = top_indices.len();
        if max_depth == 0 {
            let mut best_idx = top_indices[0];
            let mut best_score = screen_eval[best_idx];
            for &ci in top_indices.iter().skip(1) {
                if screen_eval[ci] > best_score {
                    best_score = screen_eval[ci];
                    best_idx = ci;
                }
            }
            return (my_candidates[best_idx], 0);
        }
        let mut predicted_moves = [(0usize, 0usize); MAX_M];
        for (p, slot) in predicted_moves.iter_mut().enumerate().take(m).skip(1) {
            *slot = estimator.predict_move_posterior(sim_base, &self.v, p, rng);
        }
        let prob_n = self.config.prob_scenarios;
        let leader_dist = if prob_n > 0 && m >= 2 {
            let leader = (1..m).max_by_key(|&p| sim_base.scores[p]).unwrap_or(1);
            Some((
                leader,
                estimator.get_move_distribution(sim_base, &self.v, leader),
            ))
        } else {
            None
        };
        let mut eval_sum = vec![0.0f64; top_k];
        let mut eval_count = vec![0usize; top_k];
        let mut post_states: Vec<SimState> = Vec::with_capacity(top_k);
        let mut post_connected: Vec<[u128; MAX_M]> = Vec::with_capacity(top_k);
        for (ki, &ci) in top_indices.iter().enumerate() {
            eval_sum[ki] = screen_eval[ci];
            eval_count[ki] = 1;
            let (cx, cy) = my_candidates[ci];
            predicted_moves[0] = (cx, cy);
            let mut sim = sim_base.clone();
            sim.simulate_turn(&predicted_moves[..m], &self.v);
            let e = rollout_det(
                &sim,
                &self.v,
                max_depth,
                ai_params,
                rng,
                self.config.eval_alpha,
                self.config.eval_beta,
                self.config.eval_gamma,
            );
            eval_sum[ki] += e;
            eval_count[ki] += 1;
            post_connected.push(compute_connected_bb(&sim));
            post_states.push(sim);
            if let Some((leader, ref dist)) = leader_dist {
                let n_moves = dist.len().min(prob_n);
                let prob_total: f64 = dist.iter().take(n_moves).map(|&(_, p)| p).sum();
                let mut scenario_moves = predicted_moves;
                scenario_moves[0] = (cx, cy);
                let mut prob_rng = Pcg64Mcg::seed_from_u64((ki as u64 + 1) * 7919);
                let top_weight = dist[0].1 / prob_total;
                let mut expected_eval = e * top_weight;
                for &(leader_move, prob) in dist.iter().take(n_moves).skip(1) {
                    scenario_moves[leader] = leader_move;
                    let mut sim2 = sim_base.clone();
                    sim2.simulate_turn(&scenario_moves[..m], &self.v);
                    let e2 = rollout_det(
                        &sim2,
                        &self.v,
                        max_depth,
                        ai_params,
                        &mut prob_rng,
                        self.config.eval_alpha,
                        self.config.eval_beta,
                        self.config.eval_gamma,
                    );
                    expected_eval += e2 * (prob / prob_total);
                }
                eval_sum[ki] += expected_eval - e;
            }
        }
        #[cfg(feature = "debug")]
        let phase2_ms = phase_t1.elapsed().as_secs_f64() * 1000.0;
        #[cfg(feature = "debug")]
        let phase_t2 = Instant::now();
        let mc_min_rounds = self.config.mc_rounds;
        let mut mc_count = 0usize;
        if self.config.mc_budget_factor > 0.0 {
            let mut mc_rng = Pcg64Mcg::seed_from_u64(rng.r#gen::<u64>());
            let mc_min_cycles = (mc_min_rounds + top_k - 1) / top_k;
            let phase1_ops = n_cands * self.config.screen_scenarios * screen_depth * m;
            let phase2_ops = top_k * max_depth * m;
            let ops_per_cycle = top_k * max_depth * m;
            let total_ops_budget = (turn_budget * self.config.mc_budget_factor) as usize;
            let mc_ops_budget = total_ops_budget.saturating_sub(phase1_ops + phase2_ops);
            let computed_cycles = if ops_per_cycle > 0 {
                mc_ops_budget / ops_per_cycle
            } else {
                mc_min_cycles
            };
            let total_cycles = computed_cycles.max(mc_min_cycles);
            for _ in 0..total_cycles {
                for ki in 0..top_k {
                    let mut sampled =
                        [(0.0f64, 0.0f64, 0.0f64, 0.0f64, 0.0f64); MAX_M];
                    #[cfg(feature = "debug")]
                    let tsp = Instant::now();
                    for (p, s) in sampled.iter_mut().enumerate().take(m).skip(1) {
                        *s = estimator.sample_params(p, &mut mc_rng);
                    }
                    #[cfg(feature = "debug")]
                    PROF.with(|p| {
                        let mut p = p.borrow_mut();
                        p.sample_ns += tsp.elapsed().as_nanos() as u64;
                        p.sample_count += 1;
                    });
                    let e = rollout_noisy_with_params(
                        &post_states[ki],
                        &post_connected[ki],
                        &self.v,
                        max_depth,
                        &sampled,
                        &mut mc_rng,
                        self.config.p0_eps,
                        self.config.eval_alpha,
                        self.config.eval_beta,
                        self.config.eval_gamma,
                    );
                    eval_sum[ki] += e;
                    eval_count[ki] += 1;
                    mc_count += 1;
                }
            }
        } else {
            let mc_deadline = turn_budget * self.config.mc_time_ratio;
            let mut time_ok = true;
            while mc_count < mc_min_rounds || time_ok {
                if mc_count >= mc_min_rounds && mc_count & 15 == 0 {
                    time_ok = turn_start.elapsed().as_secs_f64() < mc_deadline;
                    if !time_ok {
                        break;
                    }
                }
                let ki = mc_count % top_k;
                let mut sampled =
                    [(0.0f64, 0.0f64, 0.0f64, 0.0f64, 0.0f64); MAX_M];
                #[cfg(feature = "debug")]
                let tsp = Instant::now();
                for (p, s) in sampled.iter_mut().enumerate().take(m).skip(1) {
                    *s = estimator.sample_params(p, rng);
                }
                #[cfg(feature = "debug")]
                PROF.with(|p| {
                    let mut p = p.borrow_mut();
                    p.sample_ns += tsp.elapsed().as_nanos() as u64;
                    p.sample_count += 1;
                });
                let e = rollout_noisy_with_params(
                    &post_states[ki],
                    &post_connected[ki],
                    &self.v,
                    max_depth,
                    &sampled,
                    rng,
                    self.config.p0_eps,
                    self.config.eval_alpha,
                    self.config.eval_beta,
                    self.config.eval_gamma,
                );
                eval_sum[ki] += e;
                eval_count[ki] += 1;
                mc_count += 1;
            }
        }
        let mut best_ki = 0;
        let mut best_avg = f64::NEG_INFINITY;
        for ki in 0..top_k {
            let avg = eval_sum[ki] / eval_count[ki] as f64;
            if avg > best_avg {
                best_avg = avg;
                best_ki = ki;
            }
        }
        let best_idx = top_indices[best_ki];
        #[cfg(feature = "debug")]
        {
            let phase3_ms = phase_t2.elapsed().as_secs_f64() * 1000.0;
            PROF.with(|p| {
                let p = p.borrow();
                if mc_count > 100 {
                    eprintln!(
                        "[turn] mc_count={} | phase1={:.1}ms phase2={:.1}ms mc={:.1}ms total={:.1}ms",
                        mc_count,
                        phase1_ms,
                        phase2_ms,
                        phase3_ms,
                        turn_start.elapsed().as_secs_f64() * 1000.0
                    );
                    p.report();
                }
            });
        }
        #[cfg(feature = "debug")]
        PROF.with(|p| p.borrow_mut().reset());
        (my_candidates[best_idx], mc_count)
    }
}
#[cfg(test)]
use super::super::strategy::move_predict::{predict_move_ai_bb, predict_move_p0, predict_move_p0_bb};
#[cfg(test)]
mod tests {
    use super::*;
    fn make_test_state() -> SimState {
        let n = 10;
        let m = 3;
        let mut owner = [[-1i32; N]; N];
        let mut level = [[0u8; N]; N];
        let mut owned_bb = [0u128; MAX_M];
        let mut level_bb = [0u128; 6];
        let p0_cells = [(3, 4), (3, 5), (3, 6), (4, 4), (5, 4)];
        for &(x, y) in &p0_cells {
            owner[x][y] = 0;
            level[x][y] = 2;
            owned_bb[0] |= bb_idx(x, y);
        }
        let p1_cells = [(7, 2), (7, 3), (8, 2), (8, 3)];
        for &(x, y) in &p1_cells {
            owner[x][y] = 1;
            level[x][y] = 1;
            owned_bb[1] |= bb_idx(x, y);
        }
        owner[1][8] = 2;
        level[1][8] = 3;
        owned_bb[2] |= bb_idx(1, 8);
        for (i, row) in level.iter().enumerate().take(n) {
            for (j, &l) in row.iter().enumerate().take(n) {
                level_bb[l as usize] |= bb_idx(i, j);
            }
        }
        SimState {
            n,
            m,
            u_cap: 5,
            owner,
            level,
            positions: [
                (3, 4),
                (7, 2),
                (1, 8),
                (0, 0),
                (0, 0),
                (0, 0),
                (0, 0),
                (0, 0),
            ],
            scores: [0; MAX_M],
            owned_bb,
            level_bb,
        }
    }
    #[test]
    fn test_bb_expand_basic() {
        let center = bb_idx(5, 5);
        let expanded = bb_expand(center);
        assert!(expanded & bb_idx(5, 6) != 0);
        assert!(expanded & bb_idx(6, 5) != 0);
        assert!(expanded & bb_idx(5, 4) != 0);
        assert!(expanded & bb_idx(4, 5) != 0);
        assert_eq!(expanded.count_ones(), 4);
    }
    #[test]
    fn test_bb_expand_corner() {
        let corner = bb_idx(0, 0);
        let expanded = bb_expand(corner);
        assert!(expanded & bb_idx(0, 1) != 0);
        assert!(expanded & bb_idx(1, 0) != 0);
        assert_eq!(expanded.count_ones(), 2);
    }
    #[test]
    fn test_bb_expand_edge() {
        let edge = bb_idx(3, 9);
        let expanded = bb_expand(edge);
        assert!(expanded & bb_idx(3, 8) != 0);
        assert!(expanded & bb_idx(4, 9) != 0);
        assert!(expanded & bb_idx(2, 9) != 0);
        assert!(expanded & bb_idx(4, 0) == 0);
        assert_eq!(expanded.count_ones(), 3);
    }
    #[test]
    fn test_flood_fill_matches_bfs() {
        let state = make_test_state();
        for p in 0..state.m {
            let bfs_candidates = state.get_candidates(p);
            let mut bfs_set = [[false; N]; N];
            for &(x, y) in &bfs_candidates {
                bfs_set[x][y] = true;
            }
            let pos = bb_idx(state.positions[p].0, state.positions[p].1);
            let connected = bb_flood_fill(pos, state.owned_bb[p]);
            let frontier = bb_expand(connected) & !connected;
            let bb_reachable = connected | frontier;
            let mut occupied_bb = 0u128;
            for i in 0..state.m {
                if i != p {
                    let (px, py) = state.positions[i];
                    occupied_bb |= bb_idx(px, py);
                }
            }
            let bb_candidates = bb_reachable & !occupied_bb;
            let mut bb_set = [[false; N]; N];
            for idx in BitIter(bb_candidates) {
                let x = idx / 10;
                let y = idx % 10;
                bb_set[x][y] = true;
            }
            for i in 0..state.n {
                for j in 0..state.n {
                    assert_eq!(
                        bfs_set[i][j], bb_set[i][j],
                        "Player {} cell ({},{}) mismatch: BFS={} BB={}",
                        p, i, j, bfs_set[i][j], bb_set[i][j]
                    );
                }
            }
        }
    }
    #[test]
    fn test_owned_bb_consistency() {
        let state = make_test_state();
        for p in 0..state.m {
            for i in 0..state.n {
                for j in 0..state.n {
                    let is_owned = state.owner[i][j] == p as i32;
                    let bb_owned = state.owned_bb[p] & bb_idx(i, j) != 0;
                    assert_eq!(
                        is_owned, bb_owned,
                        "Player {} cell ({},{}) owned mismatch: array={} bb={}",
                        p, i, j, is_owned, bb_owned
                    );
                }
            }
        }
    }
    #[test]
    fn test_simulate_turn_maintains_bb() {
        let mut state = make_test_state();
        let v = [[100usize; N]; N];
        let moves = [(3, 3), (7, 1), (1, 7)];
        state.simulate_turn(&moves, &v);
        for p in 0..state.m {
            for i in 0..state.n {
                for j in 0..state.n {
                    let is_owned = state.owner[i][j] == p as i32;
                    let bb_owned = state.owned_bb[p] & bb_idx(i, j) != 0;
                    assert_eq!(
                        is_owned, bb_owned,
                        "After move: Player {} cell ({},{}) mismatch: array={} bb={}",
                        p, i, j, is_owned, bb_owned
                    );
                }
            }
        }
    }
    #[test]
    fn test_simulate_turn_attack_bb() {
        let mut state = make_test_state();
        let v = [[100usize; N]; N];
        state.owner[6][2] = 0;
        state.level[6][2] = 1;
        state.owned_bb[0] |= bb_idx(6, 2);
        state.positions[0] = (6, 2);
        let moves = [(7, 2), (8, 3), (1, 8)];
        state.simulate_turn(&moves, &v);
        assert_eq!(state.owner[7][2], 0, "Cell should be captured by P0");
        for p in 0..state.m {
            for i in 0..state.n {
                for j in 0..state.n {
                    let is_owned = state.owner[i][j] == p as i32;
                    let bb_owned = state.owned_bb[p] & bb_idx(i, j) != 0;
                    assert_eq!(
                        is_owned, bb_owned,
                        "Attack: Player {} cell ({},{}) mismatch: array={} bb={}",
                        p, i, j, is_owned, bb_owned
                    );
                }
            }
        }
    }
    #[test]
    fn test_bit_iter() {
        let board = bb_idx(0, 0) | bb_idx(3, 5) | bb_idx(9, 9);
        let indices: Vec<usize> = BitIter(board).collect();
        assert_eq!(indices, vec![0, 35, 99]);
    }
    #[test]
    fn test_predict_move_p0_bb_matches_bfs() {
        let state = make_test_state();
        let mut v = [[0usize; N]; N];
        for i in 0..10 {
            for j in 0..10 {
                v[i][j] = (i * 10 + j + 1) * 10;
            }
        }
        let mut occupied = [[false; N]; N];
        let mut occupied_bb = 0u128;
        let mut owned_union = 0u128;
        for i in 0..state.m {
            let (px, py) = state.positions[i];
            occupied[px][py] = true;
            occupied_bb |= bb_idx(px, py);
            owned_union |= state.owned_bb[i];
        }
        let bfs_result = predict_move_p0(&state, &v, &occupied);
        let (px, py) = state.positions[0];
        let connected = bb_flood_fill(bb_idx(px, py), state.owned_bb[0]);
        let bb_result =
            predict_move_p0_bb(&state, &v, occupied_bb, owned_union, connected, None);
        assert_eq!(
            bfs_result, bb_result,
            "P0 move mismatch: BFS={:?} BB={:?}",
            bfs_result, bb_result
        );
    }
    #[test]
    fn test_predict_move_ai_bb_matches_bfs() {
        let state = make_test_state();
        let mut v = [[0usize; N]; N];
        for i in 0..10 {
            for j in 0..10 {
                v[i][j] = (i * 10 + j + 1) * 10;
            }
        }
        let mut occupied = [[false; N]; N];
        let mut occupied_bb = 0u128;
        let mut owned_union = 0u128;
        for i in 0..state.m {
            let (px, py) = state.positions[i];
            occupied[px][py] = true;
            occupied_bb |= bb_idx(px, py);
            owned_union |= state.owned_bb[i];
        }
        for p in 1..state.m {
            let (wa, wb, wc, wd) = (0.7, 0.5, 0.9, 0.3);
            let mut rng_bfs = Pcg64Mcg::seed_from_u64(42 + p as u64);
            let mut rng_bb = Pcg64Mcg::seed_from_u64(42 + p as u64);
            let bfs_result =
                predict_move_ai(&state, &v, p, wa, wb, wc, wd, &occupied, &mut rng_bfs);
            let (px, py) = state.positions[p];
            let connected = bb_flood_fill(bb_idx(px, py), state.owned_bb[p]);
            let bb_result = predict_move_ai_bb(
                &state,
                &v,
                p,
                wa,
                wb,
                wc,
                wd,
                occupied_bb,
                owned_union,
                connected,
                &mut rng_bb,
            );
            assert_eq!(
                bfs_result, bb_result,
                "AI player {} move mismatch: BFS={:?} BB={:?}",
                p, bfs_result, bb_result
            );
        }
    }
}
}
pub mod sim2 {
use std::env;
use std::time::Instant;
use rand::Rng;
use rand::SeedableRng;
use rand_pcg::Pcg64Mcg;
use super::super::game::{GameState, MAX_M, N, SimState, bb_idx};
use super::super::io::{IO, Input, Output};
use super::super::strategy::Estimator;
use super::super::strategy::move_predict::{    compute_connected_bb, predict_move_ai_bb, random_move_bb, update_connected_after_owned_change,};
use super::Solver;
pub mod rollout {
use rand::Rng;
use rand_pcg::Pcg64Mcg;
use super::super::super::game::{BitIter, MAX_M, N, SimState, bb_expand, bb_idx};
use super::super::super::strategy::move_predict::{    IDX_X, IDX_Y, predict_move_ai_bb, predict_leader_greedy_target, random_move_bb,    update_connected_after_owned_change,};
const BB_VALID: u128 = (1u128 << 100) - 1;
#[inline]
fn predict_move_p0(
    state: &SimState,
    v: &[[usize; N]; N],
    occupied_bb: u128,
    owned_union: u128,
    connected: u128,
    connected_all: &[u128; MAX_M],
    remaining_turns: usize,
    leader_target: Option<(usize, usize)>,
    rng: &mut Pcg64Mcg,
) -> (usize, usize) {
    let pos = state.positions[0];
    let pos_bit = bb_idx(pos.0, pos.1);
    let cand_bb = (connected | bb_expand(connected)) & (!occupied_bb | pos_bit);
    let own = state.owned_bb[0];
    let m = state.m;
    let time_phase = 1.0 - (remaining_turns as f64 / 50.0).clamp(0.0, 1.0);
    let territory_size = own.count_ones() as f64;
    let phase = (territory_size / 25.0).clamp(0.0, 1.0);
    let leader_idx = (1..m).max_by_key(|&p| state.scores[p]).unwrap_or(1);
    let leader_bb = state.owned_bb[leader_idx];
    let leader_connected = connected_all[leader_idx];
    let enemy_bb = owned_union & !own;
    let leader_frontier = bb_expand(leader_connected) & BB_VALID & !owned_union;
    let enemy_frontier = bb_expand(enemy_bb) & BB_VALID & !owned_union;
    let stun_bb = if let Some((lx, ly)) = leader_target {
        let lt_bit = bb_idx(lx, ly);
        let own_upgradable = own & !state.level_bb[state.u_cap as usize] & connected;
        if (lt_bit & own_upgradable & cand_bb) != 0 { lt_bit } else { 0u128 }
    } else {
        0u128
    };
    let vulnerable_own = own & state.level_bb[1] & connected & cand_bb & bb_expand(enemy_bb);
    let l1_attack_mult = 1.0 + (1.0 - time_phase); // [1.0 ~ 2.0]
    let l2p_attack_w = 0.05 + 0.3 * (1.0 - time_phase); // [0.05 ~ 0.35]
    let leader_attack_bonus = 2.0; // 100% bonus for leader territory
    let mut best = pos;
    let mut best_score = f64::NEG_INFINITY;
    let mut n_tied = 0usize;
    for idx in BitIter(cand_bb) {
        let x = IDX_X[idx];
        let y = IDX_Y[idx];
        let cell_v = v[x][y] as f64;
        let cell_bb = 1u128 << idx;
        let is_unowned = (cell_bb & owned_union) == 0;
        let is_own = (cell_bb & own) != 0;
        let is_capture = is_unowned || (!is_own && (cell_bb & state.level_bb[1]) != 0);
        let is_leader_cell = (cell_bb & leader_bb) != 0;
        let base = if (cell_bb & stun_bb) != 0 {
            cell_v * 4.0
        } else if is_unowned {
            cell_v
        } else if is_own {
            if (cell_bb & state.level_bb[state.u_cap as usize]) != 0 {
                0.0
            } else if (cell_bb & vulnerable_own) != 0 {
                cell_v * 1.25 // Defend vulnerable L=1 cell adjacent to enemy
            } else {
                let upgrade_w = 0.2 + 0.8 * phase.max(time_phase);
                cell_v * upgrade_w
            }
        } else if (cell_bb & state.level_bb[1]) != 0 {
            let leader_mult = if is_leader_cell {
                leader_attack_bonus
            } else if m < 5 {
                0.5 // Focus on leader in low-M games (matches sim v1)
            } else {
                1.0
            };
            cell_v * l1_attack_mult * leader_mult
        } else {
            let leader_mult = if is_leader_cell { leader_attack_bonus } else { 1.0 };
            cell_v * l2p_attack_w * leader_mult
        };
        let u_scale = if state.u_cap > 1 { 1.0 } else { 0.0 }; // Disable at U=1 only
        let expansion = if u_scale > 0.0 && is_capture {
            let neighbors = bb_expand(cell_bb) & BB_VALID;
            let new_d1 = neighbors & !owned_union & !cand_bb;
            let mut exp_val = 0.0;
            for nidx in BitIter(new_d1) {
                exp_val += v[IDX_X[nidx]][IDX_Y[nidx]] as f64;
            }
            let new_d2 = bb_expand(new_d1) & BB_VALID & !owned_union & !cand_bb & !new_d1;
            for nidx in BitIter(new_d2) {
                exp_val += v[IDX_X[nidx]][IDX_Y[nidx]] as f64 * 0.3; // decay
            }
            exp_val * 0.1 * u_scale
        } else {
            0.0
        };
        let containment = if u_scale > 0.0 && is_unowned && (cell_bb & enemy_frontier) != 0 {
            let own_adj = (bb_expand(cell_bb) & own).count_ones() as f64;
            let early_w = 1.0 - time_phase.max(phase);
            let leader_w = if (cell_bb & leader_frontier) != 0 {
                leader_attack_bonus
            } else {
                1.0
            };
            cell_v * (0.3 + 0.15 * own_adj) * early_w * leader_w * u_scale
        } else {
            0.0
        };
        let score = base + expansion + containment;
        if score > best_score + 1e-9 {
            best_score = score;
            best = (x, y);
            n_tied = 1;
        } else if score >= best_score - 1e-9 {
            n_tied += 1;
            if rng.gen_range(0..n_tied) == 0 {
                best = (x, y);
            }
        }
    }
    best
}
pub fn rollout_crn(
    state: &SimState,
    init_connected_bb: &[u128; MAX_M],
    v: &[[usize; N]; N],
    depth: usize,
    sampled: &[(f64, f64, f64, f64, f64); MAX_M],
    round_seed: u128,
    remaining_turns: usize, // turns left in game at rollout start
    p0_eps: f64,
) -> f64 {
    let mut sim = state.clone();
    let mut moves = [(0usize, 0usize); MAX_M];
    let mut connected_bb = *init_connected_bb;
    for step in 0..depth {
        let mut occupied_bb = 0u128;
        for i in 0..sim.m {
            let (px, py) = sim.positions[i];
            occupied_bb |= bb_idx(px, py);
        }
        let mut owned_union = 0u128;
        for i in 0..sim.m {
            owned_union |= sim.owned_bb[i];
        }
        let leader_idx = (1..sim.m).max_by_key(|&p| sim.scores[p]).unwrap_or(1);
        let (lwa, lwb, lwc, lwd, _) = sampled[leader_idx];
        let leader_target = predict_leader_greedy_target(
            &sim, v, leader_idx, lwa, lwb, lwc, lwd,
            occupied_bb, owned_union, connected_bb[leader_idx],
        );
        let p0_crn_seed = round_seed ^ ((step as u128 + 1) << 32);
        let mut p0_rng = Pcg64Mcg::new(p0_crn_seed);
        let step_remaining = remaining_turns.saturating_sub(step);
        if p0_eps > 0.0 {
            let mut eps_rng = Pcg64Mcg::new(p0_crn_seed ^ 0xCAFE_BABE_u128);
            if eps_rng.gen_range(0.0..1.0) < p0_eps {
                moves[0] = random_move_bb(&sim, 0, occupied_bb, connected_bb[0], &mut eps_rng);
            } else {
                moves[0] = predict_move_p0(
                    &sim, v, occupied_bb, owned_union, connected_bb[0], &connected_bb,
                    step_remaining, Some(leader_target), &mut p0_rng,
                );
            }
        } else {
            moves[0] = predict_move_p0(
                &sim, v, occupied_bb, owned_union, connected_bb[0], &connected_bb,
                step_remaining, Some(leader_target), &mut p0_rng,
            );
        }
        for p in 1..sim.m {
            let (wa, wb, wc, wd, eps) = sampled[p];
            let crn_seed = round_seed ^ ((p as u128) << 64) ^ ((step as u128 + 1) << 32);
            let mut crn_rng = Pcg64Mcg::new(crn_seed);
            if crn_rng.gen_range(0.0..1.0) < eps {
                moves[p] = random_move_bb(&sim, p, occupied_bb, connected_bb[p], &mut crn_rng);
            } else {
                moves[p] = predict_move_ai_bb(
                    &sim, v, p, wa, wb, wc, wd,
                    occupied_bb, owned_union, connected_bb[p], &mut crn_rng,
                );
            }
        }
        let old_owned = sim.owned_bb;
        sim.simulate_turn(&moves[..sim.m], v);
        for p in 0..sim.m {
            if sim.owned_bb[p] != old_owned[p] {
                connected_bb[p] = update_connected_after_owned_change(
                    connected_bb[p], old_owned[p], sim.owned_bb[p], sim.positions[p],
                );
            }
        }
    }
    let remaining_after = remaining_turns.saturating_sub(depth);
    eval(&sim, v, &connected_bb, remaining_after)
}
#[inline]
fn eval(
    state: &SimState,
    v: &[[usize; N]; N],
    connected_bb: &[u128; MAX_M],
    remaining_turns: usize,
) -> f64 {
    let m = state.m;
    let s0 = state.scores[0] as f64;
    let sa_idx = (1..m).max_by_key(|&p| state.scores[p]).unwrap_or(1);
    let sa = state.scores[sa_idx] as f64;
    let s_second = if m > 2 {
        (1..m).filter(|&p| p != sa_idx)
            .map(|p| state.scores[p])
            .max()
            .unwrap_or(0) as f64
    } else {
        0.0
    };
    let time_ratio = (remaining_turns as f64 / 50.0).min(1.0);
    let mut conn_sa = 0.0f64;
    for l in 1..=state.u_cap as usize {
        for idx in BitIter(connected_bb[sa_idx] & state.level_bb[l]) {
            conn_sa += v[IDX_X[idx]][IDX_Y[idx]] as f64 * l as f64;
        }
    }
    let mut owned_union = 0u128;
    for p in 0..m {
        owned_union |= state.owned_bb[p];
    }
    let unowned = BB_VALID & !owned_union;
    let frontier_0 = {
        let f = bb_expand(state.owned_bb[0]) & unowned;
        let mut fv = 0.0f64;
        for idx in BitIter(f) {
            fv += v[IDX_X[idx]][IDX_Y[idx]] as f64;
        }
        fv
    };
    let frontier_a = {
        let f = bb_expand(state.owned_bb[sa_idx]) & unowned;
        let mut fv = 0.0f64;
        for idx in BitIter(f) {
            fv += v[IDX_X[idx]][IDX_Y[idx]] as f64;
        }
        fv
    };
    let cw = time_ratio * 0.15;
    let eff_sa = (1.0 - cw) * sa + cw * conn_sa;
    let t0 = state.owned_bb[0].count_ones() as f64;
    let ta = state.owned_bb[sa_idx].count_ones() as f64;
    let tw = 100.0;
    let fw = 0.15 * time_ratio;
    let adj_s0 = s0 + fw * frontier_0 + tw * t0;
    let mut adj_sa = eff_sa + fw * frontier_a + tw * ta;
    if s_second > 0.0 && sa > 0.0 {
        let threat_ratio = s_second / sa; // [0, 1]: how close 2nd is to leader
        adj_sa += 0.2 * threat_ratio * s_second; // Boost proportional to 2nd enemy strength
    }
    if adj_sa > 0.0 {
        adj_s0 / adj_sa
    } else {
        adj_s0 + 1.0
    }
}
}
use rollout::rollout_crn;
pub struct Sim2Config {
    pub rollout_depth: usize,
    pub total_budget: f64,
    pub mc_budget_factor: f64,
    pub mc_time_ratio: f64,
    pub mc_min_rounds: usize,
    pub use_crn: bool,
    pub var_penalty: f64,
    pub screen_rounds: usize,
    pub screen_top_k: usize,
    pub screen_depth: usize,
    pub p0_eps: f64,
}
impl Default for Sim2Config {
    fn default() -> Self {
        Self {
            rollout_depth: 16,
            total_budget: 1.85,
            mc_budget_factor: 0.0,
            mc_time_ratio: 0.92,
            mc_min_rounds: 30,
            use_crn: true,
            var_penalty: 0.0,
            screen_rounds: 24,
            screen_top_k: 10,
            screen_depth: 4,
            p0_eps: 0.3,
        }
    }
}
impl Sim2Config {
    pub fn from_env() -> Self {
        let mut c = Self::default();
        macro_rules! env_value {
            ($var:expr, $field:expr) => {
                if let Ok(v) = env::var($var) {
                    $field = v.parse().unwrap_or($field);
                }
            };
        }
        macro_rules! env_bool {
            ($var:expr, $field:expr) => {
                if let Ok(v) = env::var($var) {
                    $field = v != "0";
                }
            };
        }
        env_value!("SIM2_ROLLOUT_DEPTH", c.rollout_depth);
        env_value!("SIM2_BUDGET", c.total_budget);
        env_value!("SIM2_MC_BUDGET_FACTOR", c.mc_budget_factor);
        env_value!("SIM2_MC_TIME_RATIO", c.mc_time_ratio);
        env_value!("SIM2_MC_MIN_ROUNDS", c.mc_min_rounds);
        env_bool!("SIM2_CRN", c.use_crn);
        env_value!("SIM2_VAR_PENALTY", c.var_penalty);
        env_value!("SIM2_SCREEN_ROUNDS", c.screen_rounds);
        env_value!("SIM2_SCREEN_TOP_K", c.screen_top_k);
        env_value!("SIM2_SCREEN_DEPTH", c.screen_depth);
        env_value!("SIM2_P0_EPS", c.p0_eps);
        c
    }
}
pub struct Sim2Solver<'a, E: Estimator> {
    input: &'a Input,
    io: &'a mut IO,
    config: Sim2Config,
    v: [[usize; N]; N],
    oracle_params: Option<Vec<(f64, f64, f64, f64, f64)>>,
    _phantom: std::marker::PhantomData<E>,
}
impl<E: Estimator> Sim2Solver<'_, E> {
    pub fn new<'a>(input: &'a Input, io: &'a mut IO) -> Sim2Solver<'a, E> {
        let mut v = [[0usize; N]; N];
        for (i, row) in v.iter_mut().enumerate().take(input.n) {
            row[..input.n].copy_from_slice(&input.v[i][..input.n]);
        }
        let oracle_params = input.oracle_ai_params.clone();
        Sim2Solver {
            input,
            io,
            config: Sim2Config::from_env(),
            v,
            oracle_params,
            _phantom: std::marker::PhantomData,
        }
    }
}
impl<E: Estimator> Solver for Sim2Solver<'_, E> {
    fn solve(&mut self) -> Vec<Output> {
        let game_start = Instant::now();
        let total_budget = self.config.total_budget;
        let deterministic = self.config.mc_budget_factor > 0.0;
        let m = self.input.m;
        let t = self.input.t;
        let mut state = GameState::new(self.input);
        let mut estimator = E::new(m);
        let mut rng = Pcg64Mcg::seed_from_u64(42);
        let mut outputs = Vec::with_capacity(t);
        for turn in 0..t {
            let turn_start = Instant::now();
            let turn_budget = if deterministic {
                total_budget / t as f64
            } else {
                let elapsed = game_start.elapsed().as_secs_f64();
                let remaining = (total_budget - elapsed).max(0.001);
                remaining / (t - turn) as f64
            };
            let sim_base = SimState::from_game_state(&state);
            let depth = if self.config.rollout_depth == 0 {
                (t - turn).saturating_sub(1)
            } else {
                (t - turn).saturating_sub(1).min(self.config.rollout_depth)
            };
            let remaining_turns = t - turn; // [1 ~ T]
            let (tx, ty) = self.mc_search(
                &sim_base,
                &estimator,
                &mut rng,
                depth,
                turn_budget,
                turn_start,
                remaining_turns,
            );
            let output = Output { tx, ty };
            self.io.write(&output);
            outputs.push(output);
            let turn_input = self.io.read_turn(m, self.input.n);
            for p in 1..m {
                estimator.observe(&sim_base, &self.v, p, turn_input.targets[p]);
            }
            #[cfg(feature = "debug")]
            if let Some(ref oracle) = self.oracle_params {
                if turn % 10 == 0 || turn < 5 {
                    for p in 1..m {
                        let (ewa, ewb, ewc, ewd, eeps) = estimator.get_params(p);
                        let (twa, twb, twc, twd, teps) = oracle[p - 1];
                        eprintln!(
                            "PF[t={} p={}] est=({:.2},{:.2},{:.2},{:.2},{:.2}) true=({:.2},{:.2},{:.2},{:.2},{:.2}) err=({:.2},{:.2},{:.2},{:.2},{:.2})",
                            turn,
                            p,
                            ewa,
                            ewb,
                            ewc,
                            ewd,
                            eeps,
                            twa,
                            twb,
                            twc,
                            twd,
                            teps,
                            (ewa - twa).abs(),
                            (ewb - twb).abs(),
                            (ewc - twc).abs(),
                            (ewd - twd).abs(),
                            (eeps - teps).abs(),
                        );
                    }
                }
            }
            state.update(&turn_input);
        }
        outputs
    }
}
impl<E: Estimator> Sim2Solver<'_, E> {
    fn mc_search(
        &self,
        sim_base: &SimState,
        estimator: &E,
        rng: &mut Pcg64Mcg,
        depth: usize,
        turn_budget: f64,
        turn_start: Instant,
        remaining_turns: usize,
    ) -> (usize, usize) {
        let m = sim_base.m;
        let candidates = sim_base.get_candidates(0);
        let n_cands = candidates.len();
        if n_cands == 0 {
            return sim_base.positions[0];
        }
        if n_cands == 1 {
            return candidates[0];
        }
        let base_connected = compute_connected_bb(sim_base);
        let mut base_occupied_bb = 0u128;
        for i in 0..m {
            let (px, py) = sim_base.positions[i];
            base_occupied_bb |= bb_idx(px, py);
        }
        let mut base_owned_union = 0u128;
        for i in 0..m {
            base_owned_union |= sim_base.owned_bb[i];
        }
        let mean_params = {
            let mut p = [(0.0f64, 0.0f64, 0.0f64, 0.0f64, 0.0f64); MAX_M];
            if let Some(ref oracle) = self.oracle_params {
                for i in 1..m {
                    p[i] = oracle[i - 1];
                }
            } else {
                for i in 1..m {
                    p[i] = estimator.get_params(i);
                }
            }
            p
        };
        let screen_rounds = self.config.screen_rounds;
        let screen_top_k = self.config.screen_top_k.min(n_cands);
        let screen_depth = self.config.screen_depth.min(depth);
        let active_candidates = if screen_rounds > 0 && n_cands > screen_top_k {
            let mut screen_sum = vec![0.0f64; n_cands];
            let mut screen_sq = vec![0.0f64; n_cands];
            let mut screen_cnt = vec![0usize; n_cands];
            self.run_det_round(
                sim_base,
                &mean_params,
                &candidates,
                depth,
                &base_connected,
                base_occupied_bb,
                base_owned_union,
                remaining_turns,
                &mut screen_sum,
                &mut screen_sq,
                &mut screen_cnt,
            );
            let mut screen_rng = Pcg64Mcg::seed_from_u64(rng.r#gen::<u64>());
            let crn_rounds = screen_rounds / 2;
            for _ in 0..crn_rounds {
                self.run_crn_round(
                    sim_base,
                    estimator,
                    &mut screen_rng,
                    &candidates,
                    screen_depth,
                    &base_connected,
                    base_occupied_bb,
                    base_owned_union,
                    remaining_turns,
                    &mut screen_sum,
                    &mut screen_sq,
                    &mut screen_cnt,
                );
            }
            let mut ranked: Vec<(usize, f64)> = (0..n_cands)
                .map(|ki| {
                    let avg = if screen_cnt[ki] > 0 {
                        screen_sum[ki] / screen_cnt[ki] as f64
                    } else {
                        f64::NEG_INFINITY
                    };
                    (ki, avg)
                })
                .collect();
            ranked.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
            ranked.truncate(screen_top_k);
            let filtered: Vec<(usize, usize)> =
                ranked.iter().map(|&(ki, _)| candidates[ki]).collect();
            filtered
        } else {
            candidates.clone()
        };
        let n_active = active_candidates.len();
        let mut eval_sum = vec![0.0f64; n_active];
        let mut eval_sq_sum = vec![0.0f64; n_active];
        let mut eval_count = vec![0usize; n_active];
        self.run_det_round(
            sim_base,
            &mean_params,
            &active_candidates,
            depth,
            &base_connected,
            base_occupied_bb,
            base_owned_union,
            remaining_turns,
            &mut eval_sum,
            &mut eval_sq_sum,
            &mut eval_count,
        );
        if self.config.mc_budget_factor > 0.0 {
            let total_ops = (turn_budget * self.config.mc_budget_factor) as usize;
            let screen_ops = if screen_rounds > 0 && n_cands > screen_top_k {
                screen_rounds * n_cands * screen_depth * m
            } else {
                0
            };
            let remaining_ops = total_ops.saturating_sub(screen_ops);
            let ops_per_round = n_active * depth * m;
            let n_rounds = if ops_per_round > 0 {
                (remaining_ops / ops_per_round).max(self.config.mc_min_rounds)
            } else {
                self.config.mc_min_rounds
            };
            let mut mc_rng = Pcg64Mcg::seed_from_u64(rng.r#gen::<u64>());
            for _ in 0..n_rounds {
                self.run_crn_round(
                    sim_base,
                    estimator,
                    &mut mc_rng,
                    &active_candidates,
                    depth,
                    &base_connected,
                    base_occupied_bb,
                    base_owned_union,
                    remaining_turns,
                    &mut eval_sum,
                    &mut eval_sq_sum,
                    &mut eval_count,
                );
            }
        } else {
            let mc_deadline = turn_budget * self.config.mc_time_ratio;
            let mut round_count = 0usize;
            loop {
                if round_count >= self.config.mc_min_rounds && round_count & 3 == 0 {
                    if turn_start.elapsed().as_secs_f64() >= mc_deadline {
                        break;
                    }
                }
                self.run_crn_round(
                    sim_base,
                    estimator,
                    rng,
                    &active_candidates,
                    depth,
                    &base_connected,
                    base_occupied_bb,
                    base_owned_union,
                    remaining_turns,
                    &mut eval_sum,
                    &mut eval_sq_sum,
                    &mut eval_count,
                );
                round_count += 1;
            }
        }
        let vp = self.config.var_penalty;
        let mut best_ki = 0;
        let mut best_score = f64::NEG_INFINITY;
        for ki in 0..n_active {
            let n = eval_count[ki];
            if n > 0 {
                let avg = eval_sum[ki] / n as f64;
                let score = if vp > 0.0 && n > 1 {
                    let variance = (eval_sq_sum[ki] / n as f64 - avg * avg).max(0.0);
                    avg - vp * variance.sqrt()
                } else {
                    avg
                };
                if score > best_score {
                    best_score = score;
                    best_ki = ki;
                }
            }
        }
        #[cfg(feature = "debug")]
        {
            let total_sims: usize = eval_count.iter().sum();
            eprintln!(
                "DEBUG sim2: cands={} screened={} total_sims={} rounds={}",
                n_cands,
                n_active,
                total_sims,
                if n_active > 0 {
                    total_sims / n_active
                } else {
                    0
                },
            );
            let mut ranked: Vec<(usize, f64, f64)> = (0..n_active)
                .filter(|&ki| eval_count[ki] > 0)
                .map(|ki| {
                    let n = eval_count[ki];
                    let avg = eval_sum[ki] / n as f64;
                    let std = if n > 1 {
                        (eval_sq_sum[ki] / n as f64 - avg * avg).max(0.0).sqrt()
                    } else {
                        0.0
                    };
                    (ki, avg, std)
                })
                .collect();
            ranked.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
            for (rank, &(ki, avg, std)) in ranked.iter().enumerate().take(8) {
                let (cx, cy) = active_candidates[ki];
                let marker = if ki == best_ki { " <--" } else { "" };
                eprintln!(
                    "  #{}: ({},{}) avg={:.4} std={:.4} n={}{}",
                    rank + 1,
                    cx,
                    cy,
                    avg,
                    std,
                    eval_count[ki],
                    marker,
                );
            }
            if n_active > 8 {
                eprintln!("  ... ({} more)", n_active - 8);
            }
        }
        active_candidates[best_ki]
    }
    fn run_crn_round(
        &self,
        sim_base: &SimState,
        estimator: &E,
        rng: &mut Pcg64Mcg,
        candidates: &[(usize, usize)],
        depth: usize,
        base_connected: &[u128; MAX_M],
        base_occupied_bb: u128,
        base_owned_union: u128,
        remaining_turns: usize,
        eval_sum: &mut [f64],
        eval_sq_sum: &mut [f64],
        eval_count: &mut [usize],
    ) {
        let m = sim_base.m;
        let mut sampled = [(0.0f64, 0.0f64, 0.0f64, 0.0f64, 0.0f64); MAX_M];
        if let Some(ref oracle) = self.oracle_params {
            for p in 1..m {
                sampled[p] = oracle[p - 1];
            }
        } else {
            for p in 1..m {
                sampled[p] = estimator.sample_params(p, rng);
            }
        }
        let round_seed: u128 = (rng.r#gen::<u64>() as u128) | ((rng.r#gen::<u64>() as u128) << 64);
        let mut ai_step0_moves = [(0usize, 0usize); MAX_M];
        for p in 1..m {
            let (wa, wb, wc, wd, eps) = sampled[p];
            let crn_seed = round_seed ^ ((p as u128) << 64);
            let mut crn_rng = Pcg64Mcg::new(crn_seed);
            if crn_rng.gen_range(0.0..1.0) < eps {
                ai_step0_moves[p] = random_move_bb(
                    sim_base,
                    p,
                    base_occupied_bb,
                    base_connected[p],
                    &mut crn_rng,
                );
            } else {
                ai_step0_moves[p] = predict_move_ai_bb(
                    sim_base,
                    &self.v,
                    p,
                    wa,
                    wb,
                    wc,
                    wd,
                    base_occupied_bb,
                    base_owned_union,
                    base_connected[p],
                    &mut crn_rng,
                );
            }
        }
        for (ki, &(cx, cy)) in candidates.iter().enumerate() {
            let mut moves = ai_step0_moves;
            moves[0] = (cx, cy);
            let mut sim = sim_base.clone();
            sim.simulate_turn(&moves[..m], &self.v);
            let mut connected = *base_connected;
            for p in 0..m {
                if sim.owned_bb[p] != sim_base.owned_bb[p] {
                    connected[p] = update_connected_after_owned_change(
                        connected[p],
                        sim_base.owned_bb[p],
                        sim.owned_bb[p],
                        sim.positions[p],
                    );
                }
            }
            let e = rollout_crn(
                &sim,
                &connected,
                &self.v,
                depth.saturating_sub(1),
                &sampled,
                round_seed,
                remaining_turns.saturating_sub(1), // step 0 already consumed
                self.config.p0_eps,
            );
            eval_sum[ki] += e;
            eval_sq_sum[ki] += e * e;
            eval_count[ki] += 1;
        }
    }
    fn run_det_round(
        &self,
        sim_base: &SimState,
        mean_params: &[(f64, f64, f64, f64, f64); MAX_M],
        candidates: &[(usize, usize)],
        depth: usize,
        base_connected: &[u128; MAX_M],
        base_occupied_bb: u128,
        base_owned_union: u128,
        remaining_turns: usize,
        eval_sum: &mut [f64],
        eval_sq_sum: &mut [f64],
        eval_count: &mut [usize],
    ) {
        let m = sim_base.m;
        let det_seed: u128 = 0xDEAD_BEEF_CAFE_u128;
        let mut ai_step0 = [(0usize, 0usize); MAX_M];
        for p in 1..m {
            let (wa, wb, wc, wd, eps) = mean_params[p];
            let crn_seed = det_seed ^ ((p as u128) << 64);
            let mut crn_rng = Pcg64Mcg::new(crn_seed);
            if crn_rng.gen_range(0.0..1.0) < eps {
                ai_step0[p] = random_move_bb(
                    sim_base,
                    p,
                    base_occupied_bb,
                    base_connected[p],
                    &mut crn_rng,
                );
            } else {
                ai_step0[p] = predict_move_ai_bb(
                    sim_base,
                    &self.v,
                    p,
                    wa,
                    wb,
                    wc,
                    wd,
                    base_occupied_bb,
                    base_owned_union,
                    base_connected[p],
                    &mut crn_rng,
                );
            }
        }
        for (ki, &(cx, cy)) in candidates.iter().enumerate() {
            let mut moves = ai_step0;
            moves[0] = (cx, cy);
            let mut sim = sim_base.clone();
            sim.simulate_turn(&moves[..m], &self.v);
            let mut connected = *base_connected;
            for p in 0..m {
                if sim.owned_bb[p] != sim_base.owned_bb[p] {
                    connected[p] = update_connected_after_owned_change(
                        connected[p],
                        sim_base.owned_bb[p],
                        sim.owned_bb[p],
                        sim.positions[p],
                    );
                }
            }
            let e = rollout_crn(
                &sim,
                &connected,
                &self.v,
                depth.saturating_sub(1),
                mean_params,
                det_seed,
                remaining_turns.saturating_sub(1),
                0.0, // greedy P0 for det pass
            );
            eval_sum[ki] += e;
            eval_sq_sum[ki] += e * e;
            eval_count[ki] += 1;
        }
    }
}
}
pub trait Solver {
    fn solve(&mut self) -> Vec<Output>;
}
}
pub mod strategy {
use rand_pcg::Pcg64Mcg;
use super::game::{N, SimState};
pub mod cut_target {
use std::collections::VecDeque;
use super::super::game::{DIRS, N, SimState};
fn bfs_component_value(
    state: &SimState,
    v: &[[usize; N]; N],
    target_player: i32,
    start: (usize, usize),
    excluded: (usize, usize),
) -> f64 {
    let n = state.n;
    let mut visited = [[false; N]; N];
    let mut queue = VecDeque::new();
    if start == excluded {
        return 0.0;
    }
    visited[start.0][start.1] = true;
    queue.push_back(start);
    let mut total = 0.0f64;
    while let Some((x, y)) = queue.pop_front() {
        total += v[x][y] as f64 * state.level[x][y] as f64;
        for &(dx, dy) in &DIRS {
            let nx = x.wrapping_add(dx);
            let ny = y.wrapping_add(dy);
            if nx < n
                && ny < n
                && !visited[nx][ny]
                && state.owner[nx][ny] == target_player
                && (nx, ny) != excluded
            {
                visited[nx][ny] = true;
                queue.push_back((nx, ny));
            }
        }
    }
    total
}
pub fn find_cut_targets(
    state: &SimState,
    v: &[[usize; N]; N],
    target_player: usize,
    p0_candidates: &[(usize, usize)],
) -> Vec<((usize, usize), f64)> {
    let n = state.n;
    let p_i32 = target_player as i32;
    let piece_pos = state.positions[target_player];
    if state.owner[piece_pos.0][piece_pos.1] != p_i32 {
        return Vec::new();
    }
    let mut is_p0_candidate = [[false; N]; N];
    for &(x, y) in p0_candidates {
        is_p0_candidate[x][y] = true;
    }
    let baseline = bfs_component_value(state, v, p_i32, piece_pos, (usize::MAX, usize::MAX));
    let mut results = Vec::new();
    #[allow(clippy::needless_range_loop)]
    for i in 0..n {
        for j in 0..n {
            if state.owner[i][j] != p_i32 || (i, j) == piece_pos {
                continue;
            }
            if !is_p0_candidate[i][j] || state.level[i][j] > 1 {
                continue;
            }
            let after = bfs_component_value(state, v, p_i32, piece_pos, (i, j));
            let cut_value = baseline - after;
            if cut_value > 0.0 {
                results.push(((i, j), cut_value));
            }
        }
    }
    results.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
    results
}
}
pub mod move_predict {
use rand::Rng;
use rand_pcg::Pcg64Mcg;
use super::super::game::{BitIter, MAX_M, N, SimState, bb_expand, bb_flood_fill, bb_idx};
pub const IDX_X: [usize; 100] = {
    let mut a = [0usize; 100];
    let mut i = 0;
    while i < 100 {
        a[i] = i / 10;
        i += 1;
    }
    a
};
pub const IDX_Y: [usize; 100] = {
    let mut a = [0usize; 100];
    let mut i = 0;
    while i < 100 {
        a[i] = i % 10;
        i += 1;
    }
    a
};
pub const IDX_BIT: [u128; 100] = {
    let mut a = [0u128; 100];
    let mut i = 0;
    while i < 100 {
        a[i] = 1u128 << i;
        i += 1;
    }
    a
};
#[inline(always)]
pub fn bfs_expand(
    idx: usize,
    n: usize,
    visited: &mut [bool; N * N],
    queue: &mut [u8; N * N],
    qt: &mut usize,
) {
    let x = idx / n;
    let y = idx % n;
    if y + 1 < n {
        let ni = idx + 1;
        if !visited[ni] {
            visited[ni] = true;
            queue[*qt] = ni as u8;
            *qt += 1;
        }
    }
    if x + 1 < n {
        let ni = idx + n;
        if !visited[ni] {
            visited[ni] = true;
            queue[*qt] = ni as u8;
            *qt += 1;
        }
    }
    if y > 0 {
        let ni = idx - 1;
        if !visited[ni] {
            visited[ni] = true;
            queue[*qt] = ni as u8;
            *qt += 1;
        }
    }
    if x > 0 {
        let ni = idx - n;
        if !visited[ni] {
            visited[ni] = true;
            queue[*qt] = ni as u8;
            *qt += 1;
        }
    }
}
#[allow(dead_code)]
pub fn predict_move_p0(
    state: &SimState,
    v: &[[usize; N]; N],
    occupied: &[[bool; N]; N],
) -> (usize, usize) {
    let n = state.n;
    let u_cap = state.u_cap;
    let mut visited = [false; N * N];
    let mut queue = [0u8; N * N];
    let (mut qh, mut qt) = (0, 0);
    let start = state.positions[0];
    let start_idx = start.0 * n + start.1;
    visited[start_idx] = true;
    queue[qt] = start_idx as u8;
    qt += 1;
    let mut best = start;
    let mut best_v = 0usize;
    while qh < qt {
        let idx = queue[qh] as usize;
        qh += 1;
        let x = idx / n;
        let y = idx % n;
        if !occupied[x][y] || idx == start_idx {
            let owner = state.owner[x][y];
            let level = state.level[x][y];
            let cv = if (owner == 0 && level >= u_cap) || (owner > 0 && level > 1) {
                0
            } else if owner > 0 && level == 1 {
                v[x][y] * 3 / 2
            } else {
                v[x][y]
            };
            if cv > best_v {
                best_v = cv;
                best = (x, y);
            }
        }
        if state.owner[x][y] == 0 {
            bfs_expand(idx, n, &mut visited, &mut queue, &mut qt);
        }
    }
    best
}
pub fn predict_move_ai(
    state: &SimState,
    v: &[[usize; N]; N],
    player: usize,
    wa: f64,
    wb: f64,
    wc: f64,
    wd: f64,
    occupied: &[[bool; N]; N],
    rng: &mut Pcg64Mcg,
) -> (usize, usize) {
    let n = state.n;
    let p_i32 = player as i32;
    let u_cap = state.u_cap;
    let mut visited = [false; N * N];
    let mut queue = [0u8; N * N];
    let (mut qh, mut qt) = (0, 0);
    let start = state.positions[player];
    let start_idx = start.0 * n + start.1;
    visited[start_idx] = true;
    queue[qt] = start_idx as u8;
    qt += 1;
    let mut best = start;
    let mut best_a = f64::NEG_INFINITY;
    let mut n_tied = 0usize;
    while qh < qt {
        let idx = queue[qh] as usize;
        qh += 1;
        let x = idx / n;
        let y = idx % n;
        if !occupied[x][y] || idx == start_idx {
            let cv = v[x][y] as f64;
            let o = state.owner[x][y];
            let l = state.level[x][y];
            let a = if o == -1 {
                cv * wa
            } else if o == p_i32 {
                if l < u_cap { cv * wb } else { 0.0 }
            } else if l == 1 {
                cv * wc
            } else {
                cv * wd
            };
            if a > best_a + 1e-9 {
                best_a = a;
                best = (x, y);
                n_tied = 1;
            } else if a >= best_a - 1e-9 {
                n_tied += 1;
                if rng.gen_range(0..n_tied) == 0 {
                    best = (x, y);
                }
            }
        }
        if state.owner[x][y] == p_i32 {
            bfs_expand(idx, n, &mut visited, &mut queue, &mut qt);
        }
    }
    best
}
pub fn predict_move_p0_bb(
    state: &SimState,
    v: &[[usize; N]; N],
    occupied_bb: u128,
    owned_union: u128,
    connected: u128,
    leader_target: Option<(usize, usize)>,
) -> (usize, usize) {
    let pos = state.positions[0];
    let pos_bit = bb_idx(pos.0, pos.1);
    let candidates = (connected | bb_expand(connected)) & (!occupied_bb | pos_bit);
    let own = state.owned_bb[0];
    let enemy = owned_union & !own;
    let level1 = state.level_bb[1];
    let own_full = own & state.level_bb[state.u_cap as usize];
    let enemy_l1 = enemy & level1;
    let enemy_l2p = enemy & !level1;
    let leader = (1..state.m).max_by_key(|&p| state.scores[p]).unwrap_or(1);
    let leader_l1 = state.owned_bb[leader] & level1;
    let non_leader_l1_w = if state.m >= 5 { (1, 1) } else { (1, 2) }; // (num, den)
    let stun_bb = if let Some((lx, ly)) = leader_target {
        let lt_bit = bb_idx(lx, ly);
        if (lt_bit & own & !own_full & connected & candidates) != 0 {
            lt_bit
        } else {
            0u128
        }
    } else {
        0u128
    };
    let vulnerable_own = own & level1 & connected & candidates & bb_expand(enemy);
    let mut best = pos;
    let mut best_v = 0usize;
    let positive = candidates & !own_full & !enemy_l2p;
    for idx in BitIter(positive) {
        let x = IDX_X[idx];
        let y = IDX_Y[idx];
        let bit = IDX_BIT[idx];
        let cv = if (bit & stun_bb) != 0 {
            v[x][y] * 4
        } else if (bit & leader_l1) != 0 {
            v[x][y] * 7 / 2 // リーダーのL=1: 3.5x
        } else if (bit & vulnerable_own) != 0 {
            v[x][y] * 5 / 4 // 脆弱L=1セル: 1.25x 強化ボーナス
        } else if (bit & enemy_l1) != 0 {
            v[x][y] * non_leader_l1_w.0 / non_leader_l1_w.1 // M<5→0.5x, M>=5→1.0x
        } else {
            v[x][y]
        };
        if cv > best_v {
            best_v = cv;
            best = (x, y);
        }
    }
    best
}
pub fn predict_leader_greedy_target(
    state: &SimState,
    v: &[[usize; N]; N],
    player: usize,
    wa: f64,
    wb: f64,
    wc: f64,
    wd: f64,
    occupied_bb: u128,
    owned_union: u128,
    connected: u128,
) -> (usize, usize) {
    let pos = state.positions[player];
    let pos_bit = bb_idx(pos.0, pos.1);
    let candidates = (connected | bb_expand(connected)) & (!occupied_bb | pos_bit);
    let own = state.owned_bb[player];
    let enemy = owned_union & !own;
    let unowned = candidates & !owned_union;
    let own_upgradable = candidates & own & !state.level_bb[state.u_cap as usize];
    let enemy_l1 = candidates & enemy & state.level_bb[1];
    let enemy_l2p = candidates & enemy & !state.level_bb[1];
    let mut best = pos;
    let mut best_a = f64::NEG_INFINITY;
    let mut consider_mask = |mask: u128, coef: f64| {
        for idx in BitIter(mask) {
            let x = IDX_X[idx];
            let y = IDX_Y[idx];
            let a = v[x][y] as f64 * coef;
            if a > best_a + 1e-9 {
                best_a = a;
                best = (x, y);
            }
        }
    };
    consider_mask(unowned, wa);
    consider_mask(own_upgradable, wb);
    consider_mask(enemy_l1, wc);
    consider_mask(enemy_l2p, wd);
    best
}
pub fn predict_move_ai_bb(
    state: &SimState,
    v: &[[usize; N]; N],
    player: usize,
    wa: f64,
    wb: f64,
    wc: f64,
    wd: f64,
    occupied_bb: u128,
    owned_union: u128,
    connected: u128,
    rng: &mut Pcg64Mcg,
) -> (usize, usize) {
    let pos = state.positions[player];
    let pos_bit = bb_idx(pos.0, pos.1);
    let candidates = (connected | bb_expand(connected)) & (!occupied_bb | pos_bit);
    let own = state.owned_bb[player];
    let enemy = owned_union & !own;
    let unowned = candidates & !owned_union;
    let own_upgradable = candidates & own & !state.level_bb[state.u_cap as usize];
    let enemy_l1 = candidates & enemy & state.level_bb[1];
    let enemy_l2p = candidates & enemy & !state.level_bb[1];
    let mut best = pos;
    let mut best_a = f64::NEG_INFINITY;
    let mut n_tied = 0usize;
    let mut consider_mask = |mask: u128, coef: f64| {
        for idx in BitIter(mask) {
            let x = IDX_X[idx];
            let y = IDX_Y[idx];
            let a = v[x][y] as f64 * coef;
            if a > best_a + 1e-9 {
                best_a = a;
                best = (x, y);
                n_tied = 1;
            } else if a >= best_a - 1e-9 {
                n_tied += 1;
                if rng.gen_range(0..n_tied) == 0 {
                    best = (x, y);
                }
            }
        }
    };
    consider_mask(unowned, wa);
    consider_mask(own_upgradable, wb);
    consider_mask(enemy_l1, wc);
    consider_mask(enemy_l2p, wd);
    if n_tied == 0 {
        for idx in BitIter(candidates) {
            let x = IDX_X[idx];
            let y = IDX_Y[idx];
            let a = v[x][y] as f64 * wd;
            if a > best_a + 1e-9 {
                best_a = a;
                best = (x, y);
                n_tied = 1;
            } else if a >= best_a - 1e-9 {
                n_tied += 1;
                if rng.gen_range(0..n_tied) == 0 {
                    best = (x, y);
                }
            }
        }
    }
    best
}
pub fn random_move_bb(
    state: &SimState,
    player: usize,
    occupied_bb: u128,
    connected: u128,
    rng: &mut Pcg64Mcg,
) -> (usize, usize) {
    let pos = state.positions[player];
    let pos_bit = bb_idx(pos.0, pos.1);
    let candidates = (connected | bb_expand(connected)) & (!occupied_bb | pos_bit);
    let n = candidates.count_ones() as usize;
    if n == 0 {
        return pos;
    }
    let k = rng.gen_range(0..n);
    let mut bits = candidates;
    for _ in 0..k {
        bits &= bits - 1;
    }
    let idx = bits.trailing_zeros() as usize;
    (IDX_X[idx], IDX_Y[idx])
}
#[inline]
pub fn compute_connected_bb(state: &SimState) -> [u128; MAX_M] {
    let mut connected_bb = [0u128; MAX_M];
    for (p, slot) in connected_bb.iter_mut().enumerate().take(state.m) {
        let (px, py) = state.positions[p];
        *slot = bb_flood_fill(bb_idx(px, py), state.owned_bb[p]);
    }
    connected_bb
}
#[inline]
pub fn update_connected_after_owned_change(
    connected_old: u128,
    old_owned: u128,
    new_owned: u128,
    new_pos: (usize, usize),
) -> u128 {
    let removed = old_owned & !new_owned;
    if removed != 0 {
        return bb_flood_fill(bb_idx(new_pos.0, new_pos.1), new_owned);
    }
    let added = new_owned & !old_owned;
    if added == 0 {
        return connected_old;
    }
    let old_disconnected = old_owned & !connected_old;
    if (bb_expand(added) & old_disconnected) != 0 {
        bb_flood_fill(bb_idx(new_pos.0, new_pos.1), new_owned)
    } else {
        connected_old | added
    }
}
}
pub mod particle_filter {
use rand::Rng;
use rand::SeedableRng;
use rand_pcg::Pcg64Mcg;
use super::super::game::{N, SimState};
use super::Estimator;
use super::move_predict::predict_move_ai;
pub const N_PARTICLES: usize = 500;
pub type Particle = [f64; 5];
#[inline(always)]
fn classify_cell(owner: i32, level: u8, player: i32, u_cap: u8) -> u8 {
    if owner == -1 {
        0
    } else if owner == player {
        if level < u_cap { 1 } else { 2 }
    } else if level == 1 {
        3
    } else {
        4
    }
}
#[inline(always)]
fn particle_weights(particle: &Particle) -> [f64; 5] {
    [particle[0], particle[1], 0.0, particle[2], particle[3]]
}
pub struct ParticleFilter {
    particles: Vec<[Particle; N_PARTICLES]>,
    pub obs_count: Vec<usize>,
    rng: Pcg64Mcg,
}
impl Estimator for ParticleFilter {
    fn new(m: usize) -> Self {
        let mut rng = Pcg64Mcg::seed_from_u64(42);
        let mut particles = Vec::with_capacity(m);
        for _ in 0..m {
            let mut ps = [[0.0f64; 5]; N_PARTICLES];
            for p in ps.iter_mut() {
                p[0] = rng.gen_range(0.3..1.0); // wa
                p[1] = rng.gen_range(0.3..1.0); // wb
                p[2] = rng.gen_range(0.3..1.0); // wc
                p[3] = rng.gen_range(0.3..1.0); // wd
                p[4] = rng.gen_range(0.1..0.5); // eps
            }
            particles.push(ps);
        }
        ParticleFilter {
            particles,
            obs_count: vec![0; m],
            rng,
        }
    }
    fn observe(
        &mut self,
        sim_base: &SimState,
        v: &[[usize; N]; N],
        player: usize,
        observed: (usize, usize),
    ) {
        let ai = player - 1;
        let candidates = sim_base.get_candidates(player);
        let nc = candidates.len();
        if nc <= 1 {
            return;
        }
        let obs_idx = match candidates.iter().position(|&c| c == observed) {
            Some(i) => i,
            None => return,
        };
        let p_i32 = player as i32;
        let u_cap = sim_base.u_cap;
        let mut cell_v = [0.0f64; 100];
        let mut cell_type = [0u8; 100];
        for (i, &(x, y)) in candidates.iter().enumerate() {
            cell_v[i] = v[x][y] as f64;
            cell_type[i] = classify_cell(sim_base.owner[x][y], sim_base.level[x][y], p_i32, u_cap);
        }
        let mut log_weights = [0.0f64; N_PARTICLES];
        for (pi, particle) in self.particles[ai].iter().enumerate() {
            let w = particle_weights(particle);
            let eps = particle[4];
            let mut max_a = f64::NEG_INFINITY;
            for i in 0..nc {
                let a = cell_v[i] * w[cell_type[i] as usize];
                if a > max_a {
                    max_a = a;
                }
            }
            let tol = 1e-9 * max_a.abs().max(1.0);
            let mut n_ties = 0usize;
            for i in 0..nc {
                let a = cell_v[i] * w[cell_type[i] as usize];
                if max_a - a <= tol {
                    n_ties += 1;
                }
            }
            let obs_a = cell_v[obs_idx] * w[cell_type[obs_idx] as usize];
            let obs_is_max = max_a - obs_a <= tol;
            let prob = if obs_is_max {
                (1.0 - eps) / n_ties as f64 + eps / nc as f64
            } else {
                eps / nc as f64
            };
            log_weights[pi] = prob.max(1e-20).ln();
        }
        let max_lw = log_weights
            .iter()
            .cloned()
            .fold(f64::NEG_INFINITY, f64::max);
        let mut weights = [0.0f64; N_PARTICLES];
        let mut total_w = 0.0f64;
        for (i, &lw) in log_weights.iter().enumerate() {
            weights[i] = (lw - max_lw).exp();
            total_w += weights[i];
        }
        if total_w < 1e-30 {
            self.obs_count[ai] += 1;
            return;
        }
        for w in weights.iter_mut() {
            *w /= total_w;
        }
        let mut cumsum = [0.0f64; N_PARTICLES];
        let mut s = 0.0f64;
        for (i, &w) in weights.iter().enumerate() {
            s += w;
            cumsum[i] = s;
        }
        let mut new_particles = [[0.0f64; 5]; N_PARTICLES];
        let u_start = self.rng.gen_range(0.0..1.0) / N_PARTICLES as f64;
        let mut idx = 0usize;
        for (i, np) in new_particles.iter_mut().enumerate() {
            let threshold = u_start + i as f64 / N_PARTICLES as f64;
            while idx < N_PARTICLES - 1 && cumsum[idx] < threshold {
                idx += 1;
            }
            *np = self.particles[ai][idx];
        }
        let jitter = 0.02f64;
        let jitter_eps = 0.01f64;
        for p in new_particles.iter_mut() {
            for val in p.iter_mut().take(4) {
                *val = (*val + self.rng.gen_range(-jitter..jitter)).clamp(0.3, 1.0);
            }
            p[4] = (p[4] + self.rng.gen_range(-jitter_eps..jitter_eps)).clamp(0.1, 0.5);
        }
        self.particles[ai] = new_particles;
        self.obs_count[ai] += 1;
    }
    fn get_params(&self, player: usize) -> (f64, f64, f64, f64, f64) {
        let ai = player - 1;
        if self.obs_count[ai] < 3 {
            return (1.0, 1.0, 1.0, 1.0, 0.3);
        }
        let mut sums = [0.0f64; 5];
        for p in &self.particles[ai] {
            for (k, &val) in p.iter().enumerate() {
                sums[k] += val;
            }
        }
        let n = N_PARTICLES as f64;
        (
            sums[0] / n,
            sums[1] / n,
            sums[2] / n,
            sums[3] / n,
            sums[4] / n,
        )
    }
    fn sample_params(&self, player: usize, rng: &mut Pcg64Mcg) -> (f64, f64, f64, f64, f64) {
        let ai = player - 1;
        if self.obs_count[ai] < 3 {
            return (
                rng.gen_range(0.3..1.0),
                rng.gen_range(0.3..1.0),
                rng.gen_range(0.3..1.0),
                rng.gen_range(0.3..1.0),
                rng.gen_range(0.1..0.5),
            );
        }
        let idx = rng.gen_range(0..N_PARTICLES);
        let p = &self.particles[ai][idx];
        (p[0], p[1], p[2], p[3], p[4])
    }
    fn predict_move_posterior(
        &self,
        sim_base: &SimState,
        v: &[[usize; N]; N],
        player: usize,
        rng: &mut Pcg64Mcg,
    ) -> (usize, usize) {
        let ai = player - 1;
        if self.obs_count[ai] < 3 {
            let mut occ = [[false; N]; N];
            for i in 0..sim_base.m {
                if i != player {
                    let (px, py) = sim_base.positions[i];
                    occ[px][py] = true;
                }
            }
            return predict_move_ai(sim_base, v, player, 1.0, 1.0, 1.0, 1.0, &occ, rng);
        }
        let candidates = sim_base.get_candidates(player);
        let nc = candidates.len();
        if nc == 0 {
            return sim_base.positions[player];
        }
        if nc == 1 {
            return candidates[0];
        }
        let p_i32 = player as i32;
        let u_cap = sim_base.u_cap;
        let mut cell_v = [0.0f64; 100];
        let mut cell_type = [0u8; 100];
        for (i, &(x, y)) in candidates.iter().enumerate() {
            cell_v[i] = v[x][y] as f64;
            cell_type[i] = classify_cell(sim_base.owner[x][y], sim_base.level[x][y], p_i32, u_cap);
        }
        let mut cell_votes = [0.0f64; 100];
        for particle in &self.particles[ai] {
            let w = particle_weights(particle);
            let mut max_a = f64::NEG_INFINITY;
            for i in 0..nc {
                let a = cell_v[i] * w[cell_type[i] as usize];
                if a > max_a {
                    max_a = a;
                }
            }
            let tol = 1e-9 * max_a.abs().max(1.0);
            let mut n_ties = 0usize;
            for i in 0..nc {
                if max_a - cell_v[i] * w[cell_type[i] as usize] <= tol {
                    n_ties += 1;
                }
            }
            let vote = 1.0 / n_ties as f64;
            for i in 0..nc {
                if max_a - cell_v[i] * w[cell_type[i] as usize] <= tol {
                    cell_votes[i] += vote;
                }
            }
        }
        let mut best_vote = f64::NEG_INFINITY;
        for &vote in cell_votes.iter().take(nc) {
            if vote > best_vote {
                best_vote = vote;
            }
        }
        let vote_tol = 1e-9 * best_vote.abs().max(1.0);
        let mut tied_indices = Vec::new();
        for (i, &vote) in cell_votes.iter().enumerate().take(nc) {
            if best_vote - vote <= vote_tol {
                tied_indices.push(i);
            }
        }
        let chosen = tied_indices[rng.gen_range(0..tied_indices.len())];
        candidates[chosen]
    }
    fn get_move_distribution(
        &self,
        sim_base: &SimState,
        v: &[[usize; N]; N],
        player: usize,
    ) -> Vec<((usize, usize), f64)> {
        let ai = player - 1;
        let candidates = sim_base.get_candidates(player);
        let nc = candidates.len();
        if nc == 0 {
            return vec![(sim_base.positions[player], 1.0)];
        }
        if nc == 1 {
            return vec![(candidates[0], 1.0)];
        }
        if self.obs_count[ai] < 3 {
            let p = 1.0 / nc as f64;
            return candidates.iter().map(|&c| (c, p)).collect();
        }
        let p_i32 = player as i32;
        let u_cap = sim_base.u_cap;
        let mut cell_v = [0.0f64; 100];
        let mut cell_type = [0u8; 100];
        for (i, &(x, y)) in candidates.iter().enumerate() {
            cell_v[i] = v[x][y] as f64;
            cell_type[i] = classify_cell(sim_base.owner[x][y], sim_base.level[x][y], p_i32, u_cap);
        }
        let mut cell_votes = [0.0f64; 100];
        let mut eps_sum = 0.0f64;
        for particle in &self.particles[ai] {
            let w = particle_weights(particle);
            eps_sum += particle[4];
            let mut max_a = f64::NEG_INFINITY;
            for i in 0..nc {
                let a = cell_v[i] * w[cell_type[i] as usize];
                if a > max_a {
                    max_a = a;
                }
            }
            let tol = 1e-9 * max_a.abs().max(1.0);
            let mut n_ties = 0usize;
            for i in 0..nc {
                if max_a - cell_v[i] * w[cell_type[i] as usize] <= tol {
                    n_ties += 1;
                }
            }
            let vote = 1.0 / n_ties as f64;
            for i in 0..nc {
                if max_a - cell_v[i] * w[cell_type[i] as usize] <= tol {
                    cell_votes[i] += vote;
                }
            }
        }
        let eps_mean = eps_sum / N_PARTICLES as f64;
        let n_particles = N_PARTICLES as f64;
        let mut result: Vec<((usize, usize), f64)> = candidates
            .iter()
            .enumerate()
            .map(|(i, &c)| {
                let greedy_prob = cell_votes[i] / n_particles;
                let prob = (1.0 - eps_mean) * greedy_prob + eps_mean / nc as f64;
                (c, prob)
            })
            .collect();
        result.sort_unstable_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
        result
    }
}
}
pub trait Estimator {
    fn new(m: usize) -> Self
    where
        Self: Sized;
    fn observe(
        &mut self,
        state: &SimState,
        v: &[[usize; N]; N],
        player: usize,
        observed: (usize, usize),
    );
    fn get_params(&self, player: usize) -> (f64, f64, f64, f64, f64);
    fn sample_params(&self, player: usize, rng: &mut Pcg64Mcg) -> (f64, f64, f64, f64, f64);
    fn predict_move_posterior(
        &self,
        state: &SimState,
        v: &[[usize; N]; N],
        player: usize,
        rng: &mut Pcg64Mcg,
    ) -> (usize, usize);
    fn get_move_distribution(
        &self,
        state: &SimState,
        v: &[[usize; N]; N],
        player: usize,
    ) -> Vec<((usize, usize), f64)>;
}
}
}
fn main() {
    let mut io = IO::default();
    let input = io.read();
    let engine = match std::env::var("SOLVER_ENGINE") {
        Ok(v) => SolverEngine::parse(v.as_str()).unwrap_or_else(|| {
            eprintln!(
                "invalid SOLVER_ENGINE='{}' (expected 'sim', 'sim2', 'search', or 'greedy')",
                v
            );
            std::process::exit(2);
        }),
        Err(_) => {
            RouterConfig::from_env().decide(&input)
        }
    };
    match engine {
        SolverEngine::Sim => {
            let mut solver = SimSolver::<ParticleFilter>::new(&input, &mut io);
            solver.solve();
        }
        SolverEngine::Sim2 => {
            let mut solver = Sim2Solver::<ParticleFilter>::new(&input, &mut io);
            solver.solve();
        }
        SolverEngine::Search => {
            let mut solver = SearchSolver::<ParticleFilter>::new(&input, &mut io);
            solver.solve();
        }
        SolverEngine::Greedy => {
            let mut solver = GreedySolver::new(&input, &mut io);
            solver.solve();
        }
    }
}
