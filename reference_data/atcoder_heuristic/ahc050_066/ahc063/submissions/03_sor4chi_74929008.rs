use rand::SeedableRng;
use rand_pcg::Pcg64Mcg;
use solver::{
    io::IO,
    solver::{Solver, beam::BeamSolver},
};
pub mod solver {
pub mod io {
use std::fmt::Display;
use std::time::Instant;
use proconio::input;
pub const TL: usize = 1950;
pub struct Input {
    pub n: usize,
    pub m: usize,
    pub c: usize,
    pub d: Vec<usize>,
    pub f: Vec<Vec<usize>>,
    pub start_time: Instant,
}
#[derive(Clone, Copy)]
pub enum Direction {
    Up,
    Down,
    Left,
    Right,
}
pub struct Output {
    pub moves: Vec<Direction>,
}
impl Direction {
    pub fn to_char(self) -> char {
        match self {
            Direction::Up => 'U',
            Direction::Down => 'D',
            Direction::Left => 'L',
            Direction::Right => 'R',
        }
    }
}
impl Display for Direction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.to_char())
    }
}
impl From<char> for Direction {
    fn from(c: char) -> Self {
        match c {
            'U' => Direction::Up,
            'D' => Direction::Down,
            'L' => Direction::Left,
            'R' => Direction::Right,
            _ => panic!("Invalid direction: {}", c),
        }
    }
}
pub const DIRECTIONS: [Direction; 4] = [
    Direction::Up,
    Direction::Down,
    Direction::Left,
    Direction::Right,
];
impl Display for Output {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for &m in &self.moves {
            writeln!(f, "{}", m)?;
        }
        Ok(())
    }
}
#[derive(Default)]
pub struct IO {}
impl IO {
    pub fn read(&mut self) -> Input {
        input! {
            n: usize, m: usize, c: usize,
            d: [usize; m],
            f: [[usize; n]; n],
        }
        Input {
            n,
            m,
            c,
            d,
            f,
            start_time: Instant::now(),
        }
    }
    pub fn write(&self, output: &Output) {
        print!("{}", output);
    }
}
}
pub mod model {
use super::io::{Direction, Input};
pub const MAX_N: usize = 16;
pub const MAX_CELLS: usize = MAX_N * MAX_N;
pub const MAX_M: usize = 192;
pub const POS_CAP: usize = 256;
const POS_MASK: usize = POS_CAP - 1;
pub const DR: [i32; 4] = [-1, 1, 0, 0];
pub const DC: [i32; 4] = [0, 0, -1, 1];
pub const DIRS: [Direction; 4] = [
    Direction::Up,
    Direction::Down,
    Direction::Left,
    Direction::Right,
];
#[inline]
pub fn dir_index(d: Direction) -> usize {
    match d {
        Direction::Up => 0,
        Direction::Down => 1,
        Direction::Left => 2,
        Direction::Right => 3,
    }
}
pub enum MoveResult {
    Move,
    Eat(usize), 
    Bite(usize),
}
pub struct UndoInfo {
    pub old_head_idx: u16,
    pub old_k: u16,
    pub result: MoveResult,
    pub eat_color: u8,       
    pub bite_cells: [u8; 64],
    pub bite_count: u8,
}
#[inline(always)]
fn find_byte_u64(w: u64, target: u8) -> Option<usize> {
    let pattern = (target as u64) * 0x0101010101010101u64;
    let diff = w ^ pattern;
    let marker = diff.wrapping_sub(0x0101010101010101u64) & !diff & 0x8080808080808080u64;
    if marker == 0 {
        None
    } else {
        Some((marker.trailing_zeros() / 8) as usize)
    }
}
#[inline]
fn find_byte_in_slice(buf: &[u8], start: usize, len: usize, target: u8) -> Option<usize> {
    let slice = &buf[start..start + len];
    let mut i = 0;
    while i + 8 <= len {
        let w = unsafe { std::ptr::read_unaligned(slice.as_ptr().add(i) as *const u64) };
        if let Some(off) = find_byte_u64(w, target) {
            return Some(i + off);
        }
        i += 8;
    }
    while i < len {
        if slice[i] == target {
            return Some(i);
        }
        i += 1;
    }
    None
}
#[derive(Clone)]
pub struct GameState {
    pub n: u8,
    pub board: [u64; 16],  
    pub pos: [u8; POS_CAP],
    pub col: [u8; MAX_M],  
    pub head_idx: u16,     
    pub k: u16,            
}
impl GameState {
    #[inline(always)]
    pub fn board_get(&self, cell: usize) -> u8 {
        let w = unsafe { *self.board.get_unchecked(cell >> 4) };
        ((w >> ((cell & 15) << 2)) & 0xF) as u8
    }
    #[inline(always)]
    pub fn board_set(&mut self, cell: usize, val: u8) {
        let shift = (cell & 15) << 2;
        unsafe {
            let w = self.board.get_unchecked_mut(cell >> 4);
            *w = (*w & !(0xF << shift)) | ((val as u64) << shift);
        }
    }
    pub fn new(input: &Input) -> Self {
        let n = input.n;
        let mut board = [0u64; 16];
        for r in 0..n {
            for c in 0..n {
                let cell = r * n + c;
                let val = input.f[r][c] as u64;
                let shift = (cell & 15) << 2;
                board[cell >> 4] |= val << shift;
            }
        }
        let mut pos = [0u8; POS_CAP];
        for i in 0..5 {
            pos[i] = ((4 - i) * n) as u8;
        }
        let mut col = [0u8; MAX_M];
        for i in 0..5 {
            col[i] = 1;
        }
        GameState {
            n: n as u8,
            board,
            pos,
            col,
            head_idx: 0,
            k: 5,
        }
    }
    #[inline(always)]
    fn body_slot(&self, i: usize) -> usize {
        (self.head_idx as usize + i) & POS_MASK
    }
    #[inline(always)]
    pub fn body(&self, i: usize) -> u8 {
        unsafe { *self.pos.get_unchecked(self.body_slot(i)) }
    }
    #[inline(always)]
    pub fn k(&self) -> usize {
        self.k as usize
    }
    #[inline(always)]
    pub fn n(&self) -> usize {
        self.n as usize
    }
    #[inline]
    pub fn is_body(&self, cell: usize) -> bool {
        let c = cell as u8;
        for i in 0..self.k as usize {
            if self.body(i) == c {
                return true;
            }
        }
        false
    }
    #[inline]
    pub fn head(&self) -> (usize, usize) {
        let cell = self.head_cell();
        let n = self.n();
        (cell / n, cell % n)
    }
    #[inline(always)]
    pub fn head_cell(&self) -> usize {
        unsafe { *self.pos.get_unchecked(self.head_idx as usize) as usize }
    }
    #[inline(always)]
    pub fn tail_cell(&self) -> usize {
        self.pos[((self.head_idx + self.k - 1) as usize) & POS_MASK] as usize
    }
    #[inline(always)]
    fn dir_delta(&self, di: usize) -> i32 {
        match di {
            0 => -(self.n as i32),
            1 => self.n as i32,
            2 => -1,
            _ => 1,
        }
    }
    pub fn forbidden_dir(&self) -> Option<Direction> {
        if self.k() < 2 {
            return None;
        }
        let h = self.body(0) as i32;
        let b = self.body(1) as i32;
        let delta = b - h;
        let n = self.n as i32;
        Some(if delta == -n {
            Direction::Up
        } else if delta == n {
            Direction::Down
        } else if delta == -1 {
            Direction::Left
        } else {
            Direction::Right
        })
    }
    #[inline(always)]
    pub fn apply_raw(&mut self, d: Direction, bite_cells: &mut [u8; 64]) -> (MoveResult, u8, u8) {
        let di = dir_index(d);
        let k = self.k();
        let head_cell = unsafe { *self.pos.get_unchecked(self.head_idx as usize) } as i32;
        let new_cell = (head_cell + self.dir_delta(di)) as u8;
        let new_head_idx = self.head_idx.wrapping_sub(1) & POS_MASK as u16;
        self.head_idx = new_head_idx;
        unsafe { *self.pos.get_unchecked_mut(new_head_idx as usize) = new_cell; }
        let nc_idx = new_cell as usize;
        let food_color = self.board_get(nc_idx);
        if food_color > 0 {
            self.board_set(nc_idx, 0);
            self.col[k] = food_color;
            self.k += 1;
            return (MoveResult::Eat(k), food_color, 0);
        }
        if k >= 4 {
            let scan_start = (new_head_idx as usize + 2) & POS_MASK;
            let scan_len = k - 3;
            let part1_len = (POS_CAP - scan_start).min(scan_len);
            let part2_len = scan_len - part1_len;
            let found = find_byte_in_slice(&self.pos, scan_start, part1_len, new_cell)
                .or_else(|| {
                    find_byte_in_slice(&self.pos, 0, part2_len, new_cell)
                        .map(|o| part1_len + o)
                });
            if let Some(offset) = found {
                let h = 2 + offset;
                let cnt = (k - h - 1).min(64);
                for i in 0..cnt {
                    let slot = (new_head_idx as usize + h + 1 + i) & POS_MASK;
                    let pc = unsafe { *self.pos.get_unchecked(slot) };
                    bite_cells[i] = pc;
                    let color = unsafe { *self.col.get_unchecked(h + 1 + i) };
                    self.board_set(pc as usize, color);
                }
                self.k = (h + 1) as u16;
                return (MoveResult::Bite(h), 0, cnt as u8);
            }
        }
        (MoveResult::Move, 0, 0)
    }
    pub fn apply_move(&mut self, d: Direction) {
        let mut buf = [0u8; 64];
        self.apply_raw(d, &mut buf);
    }
    pub fn apply_move_undo(&mut self, d: Direction) -> UndoInfo {
        let old_k = self.k;
        let old_head_idx = self.head_idx;
        let mut bite_cells = [0u8; 64];
        let (result, eat_color, bite_count) = self.apply_raw(d, &mut bite_cells);
        UndoInfo {
            old_head_idx,
            old_k,
            result,
            eat_color,
            bite_cells,
            bite_count,
        }
    }
    pub fn undo_move(&mut self, undo: &UndoInfo) {
        match undo.result {
            MoveResult::Eat(_) => {
                let new_cell = self.pos[self.head_idx as usize] as usize;
                self.board_set(new_cell, undo.eat_color);
            }
            MoveResult::Bite(_) => {
                for i in 0..undo.bite_count as usize {
                    self.board_set(undo.bite_cells[i] as usize, 0);
                }
            }
            MoveResult::Move => {}
        }
        self.head_idx = undo.old_head_idx;
        self.k = undo.old_k;
    }
    #[inline(always)]
    pub fn revert_apply(
        &mut self,
        result: &MoveResult,
        eat_color: u8,
        bite_count: u8,
        bite_cells: &[u8; 64],
        old_head_idx: u16,
        old_k: u16,
    ) {
        match *result {
            MoveResult::Eat(_) => {
                let new_cell = self.pos[self.head_idx as usize] as usize;
                self.board_set(new_cell, eat_color);
            }
            MoveResult::Bite(_) => {
                for i in 0..bite_count as usize {
                    self.board_set(bite_cells[i] as usize, 0);
                }
            }
            MoveResult::Move => {}
        }
        self.head_idx = old_head_idx;
        self.k = old_k;
    }
    pub fn matched_prefix_len(&self, target: &[usize]) -> usize {
        let k = self.k();
        let mut l = 0;
        for i in 0..k.min(target.len()) {
            if self.col[i] as usize == target[i] {
                l += 1;
            } else {
                break;
            }
        }
        l
    }
}
}
pub mod solver {
use super::io::Output;
use rand::Rng;
pub mod beam {
use super::Solver;
use super::inspect::{BeamInspector, NoopInspector, SnapshotNode};
use super::super::io::{Direction, Input, Output};
use super::super::model::{DC, DIRS, DR, GameState, MAX_CELLS, MoveResult, dir_index};
use rand::Rng;
use rand::seq::SliceRandom;
use rustc_hash::FxHashSet;
use std::time::Duration;
pub struct BeamSolver<'a> {
    input: &'a Input,
}
impl BeamSolver<'_> {
    pub fn new(input: &Input) -> BeamSolver<'_> {
        BeamSolver { input }
    }
}
#[derive(Clone)]
struct Node {
    state: GameState,
    prefix: u16,
    board_hash: u64,
    potential: u16,
}
struct ZobristTables {
    board: [[u64; 8]; MAX_CELLS],
    head: [u64; MAX_CELLS],
    prefix: [u64; MAX_CELLS],
}
fn gen_zobrist<R: Rng + ?Sized>(rng: &mut R) -> ZobristTables {
    let mut board = [[0u64; 8]; MAX_CELLS];
    for i in 0..MAX_CELLS {
        for c in 0..8 {
            board[i][c] = rng.gen_range(0..u64::MAX);
        }
    }
    let mut head = [0u64; MAX_CELLS];
    for i in 0..MAX_CELLS {
        head[i] = rng.gen_range(0..u64::MAX);
    }
    let mut prefix = [0u64; MAX_CELLS];
    for i in 0..MAX_CELLS {
        prefix[i] = rng.gen_range(0..u64::MAX);
    }
    ZobristTables {
        board,
        head,
        prefix,
    }
}
static ZOBRIST: std::sync::OnceLock<ZobristTables> = std::sync::OnceLock::new();
fn zob<R: Rng + ?Sized>(rng: &mut R) -> &'static ZobristTables {
    ZOBRIST.get_or_init(|| gen_zobrist(rng))
}
fn eval_val(prefix: u16, state: &GameState, d: &[usize], n: usize, prefix_w: i64) -> i64 {
    let base = (prefix as i64) * prefix_w;
    let p = prefix as usize;
    if p >= d.len() {
        return base;
    }
    let slack = (state.k() as i64 - p as i64).clamp(0, 3);
    base + slack
}
struct TabuList {
    tenure: usize,
    buf: Vec<u64>,
    idx: usize,
    filled: bool,
    set: FxHashSet<u64>,
}
impl TabuList {
    fn new(tenure: usize) -> Self {
        let tenure = tenure.max(1);
        let mut set = FxHashSet::default();
        set.reserve(tenure * 2 + 8);
        Self {
            tenure,
            buf: vec![0u64; tenure],
            idx: 0,
            filled: false,
            set,
        }
    }
    #[inline]
    fn is_tabu(&self, sig: u64) -> bool {
        self.set.contains(&sig)
    }
    #[inline]
    fn push(&mut self, sig: u64) {
        if self.filled {
            let old = self.buf[self.idx];
            self.set.remove(&old);
        }
        self.buf[self.idx] = sig;
        self.set.insert(sig);
        self.idx += 1;
        if self.idx >= self.tenure {
            self.idx = 0;
            self.filled = true;
        }
    }
}
pub fn beam_search<R: Rng + ?Sized, I: BeamInspector>(
    input: &Input,
    rng: &mut R,
    beam_w: usize,
    tl: Duration,
    inspector: &mut I,
    tabu_tenure_override: Option<usize>,
    _extra: usize,
) -> (Vec<Direction>, usize, usize, usize) {
    let tabu_tenure = tabu_tenure_override
        .unwrap_or_else(|| beam_w.saturating_mul(4).max(64));
    let n = input.n;
    let m = input.m;
    let d = &input.d;
    let start = input.start_time;
    let z = zob(rng);
    let initial = GameState::new(input);
    let p0 = initial.matched_prefix_len(d) as u16;
    let mut init_hash = 0u64;
    for cell in 0..(n * n) {
        let c = initial.board_get(cell) as usize;
        if c > 0 {
            init_hash ^= z.board[cell][c];
        }
    }
    let mut cur = vec![Node {
        state: initial,
        prefix: p0,
        board_hash: init_hash,
        potential: 0,
    }];
    let mut history: Vec<Vec<(u32, u8)>> = Vec::with_capacity(8192);
    let mut best_depth = 0usize;
    let mut best_idx = 0usize;
    let mut best_prefix = p0 as usize;
    let mut tabu = TabuList::new(tabu_tenure);
    let mut cand_meta: Vec<(i64, u32, u8, u64, bool, u16, u64, u16)> =
        Vec::with_capacity(beam_w * 4);
    let mut cand_idx: Vec<u32> = Vec::with_capacity(beam_w * 4);
    let mut seen: FxHashSet<u64> = {
        let mut s = FxHashSet::default();
        s.reserve(beam_w * 4);
        s
    };
    let prefix_w: i64 = std::env::var("EV_PW")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or(16);
    let pot_w: i64 = std::env::var("EV_POTW")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or(8);
    let la2_w: i64 = std::env::var("EV_LA2W")
        .ok()
        .and_then(|v| v.parse().ok())
        .unwrap_or(7);
    for t in 0.. {
        if t > 5000 {
            break;
        }
        if start.elapsed() > tl {
            break;
        }
        let hd = history.len();
        for (bi, nd) in cur.iter().enumerate() {
            let p = nd.prefix as usize;
            if p >= m && nd.state.k() >= m {
                let moves = reconstruct(&history, hd, bi);
                return (moves, p, hd, hd);
            }
            if p > best_prefix || (p == best_prefix && hd < best_depth) {
                best_prefix = p;
                best_depth = hd;
                best_idx = bi;
            }
        }
        cand_meta.clear();
        cand_idx.clear();
        seen.clear();
        let mut bite_buf = [0u8; 64];
        for (bi, nd) in cur.iter_mut().enumerate() {
            let pp = nd.prefix as usize;
            let parent_potential = nd.potential;
            let forbidden = nd.state.forbidden_dir();
            let (hr, hc) = nd.state.head();
            let parent_head_idx = nd.state.head_idx;
            let parent_k = nd.state.k;
            for di in 0..4usize {
                if let Some(f) = forbidden {
                    if di == dir_index(f) {
                        continue;
                    }
                }
                let nr = hr as i32 + DR[di];
                let nc = hc as i32 + DC[di];
                if nr < 0 || nr >= n as i32 || nc < 0 || nc >= n as i32 {
                    continue;
                }
                let (result, eat_color, bite_count) =
                    nd.state.apply_raw(DIRS[di], &mut bite_buf);
                let mut child_potential = parent_potential.saturating_sub(1);
                let prefix = match result {
                    MoveResult::Move => pp,
                    MoveResult::Eat(old_k) => {
                        if pp == old_k
                            && old_k < d.len()
                            && nd.state.col[old_k] as usize == d[old_k]
                        {
                            pp + 1
                        } else {
                            child_potential = 0;
                            pp
                        }
                    }
                    MoveResult::Bite(h) => {
                        let new_prefix = pp.min(h + 1);
                        let lost = (pp - new_prefix) as u16;
                        child_potential = child_potential.saturating_add(lost);
                        new_prefix
                    }
                };
                let mut child_hash = nd.board_hash;
                match result {
                    MoveResult::Eat(_) => {
                        let hc = nd.state.head_cell();
                        child_hash ^= z.board[hc][eat_color as usize];
                    }
                    MoveResult::Bite(h) => {
                        for i in 0..bite_count as usize {
                            let cell = bite_buf[i];
                            child_hash ^=
                                z.board[cell as usize][nd.state.col[h + 1 + i] as usize];
                        }
                    }
                    MoveResult::Move => {}
                }
                let child_sig = child_hash
                    ^ z.head[nd.state.head_cell()]
                    ^ z.prefix[prefix];
                let child_k = nd.state.k();
                let solved = prefix >= m && child_k >= m;
                let in_recovery = child_potential > 0;
                let advanced = prefix > pp;
                let aspiration = solved || prefix > best_prefix || in_recovery || advanced;
                if tabu.is_tabu(child_sig) && !aspiration {
                    nd.state.revert_apply(
                        &result,
                        eat_color,
                        bite_count,
                        &bite_buf,
                        parent_head_idx,
                        parent_k,
                    );
                    continue;
                }
                if !aspiration && !seen.insert(child_sig) {
                    nd.state.revert_apply(
                        &result,
                        eat_color,
                        bite_count,
                        &bite_buf,
                        parent_head_idx,
                        parent_k,
                    );
                    continue;
                }
                let mut ev = eval_val(prefix as u16, &nd.state, d, n, prefix_w);
                ev += (child_potential as i64) * pot_w;
                let child_k = nd.state.k();
                if child_k == prefix && prefix < d.len() {
                    let tgt = d[prefix] as u8;
                    let hc = nd.state.head_cell();
                    let ni = n as i32;
                    let hr = (hc / n) as i32;
                    let hcc = (hc % n) as i32;
                    let mut found_1 = false;
                    for &(dr, dc) in &[(-1i32,0i32),(1,0),(0,-1),(0,1)] {
                        let nr = hr + dr;
                        let nc = hcc + dc;
                        if nr >= 0 && nr < ni && nc >= 0 && nc < ni {
                            let cell = (nr as usize) * n + nc as usize;
                            if nd.state.board_get(cell) == tgt {
                                ev += 10;
                                found_1 = true;
                                if prefix + 1 < d.len() {
                                    let tgt2 = d[prefix + 1] as u8;
                                    for &(dr2, dc2) in &[(-1i32,0i32),(1,0),(0,-1),(0,1)] {
                                        let nr2 = nr + dr2;
                                        let nc2 = nc + dc2;
                                        if nr2 >= 0 && nr2 < ni && nc2 >= 0 && nc2 < ni {
                                            let c2 = (nr2 as usize) * n + nc2 as usize;
                                            if c2 != hc && nd.state.board_get(c2) == tgt2 {
                                                ev += la2_w;
                                                break;
                                            }
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                    let _ = found_1;
                }
                let ready = child_k == prefix;
                cand_meta.push((
                    ev,
                    bi as u32,
                    di as u8,
                    child_sig,
                    ready,
                    prefix as u16,
                    child_hash,
                    child_potential,
                ));
                nd.state.revert_apply(
                    &result,
                    eat_color,
                    bite_count,
                    &bite_buf,
                    parent_head_idx,
                    parent_k,
                );
            }
        }
        if cand_meta.is_empty() {
            break;
        }
        cand_idx.extend(0..cand_meta.len() as u32);
        cand_idx.sort_unstable_by(|&a, &b| cand_meta[b as usize].0.cmp(&cand_meta[a as usize].0));
        let eval_slots = (beam_w * 6 / 10).min(cand_idx.len());
        let total = beam_w.min(cand_idx.len());
        if total > eval_slots {
            cand_idx[eval_slots..].sort_unstable_by(|&a, &b| {
                let ar = cand_meta[a as usize].4 as u8;
                let br = cand_meta[b as usize].4 as u8;
                br.cmp(&ar).then(cand_meta[b as usize].0.cmp(&cand_meta[a as usize].0))
            });
        }
        for i in 0..total {
            tabu.push(cand_meta[cand_idx[i] as usize].3);
        }
        cand_idx[..total].shuffle(rng);
        let mut decisions: Vec<(u32, u8)> = Vec::with_capacity(total);
        let mut new_cur: Vec<Node> = Vec::with_capacity(total);
        for i in 0..total {
            let m_i = cand_idx[i] as usize;
            let (_, parent_bi, di, _, _, prefix, child_hash, potential) = cand_meta[m_i];
            decisions.push((parent_bi, di));
            let mut state = cur[parent_bi as usize].state.clone();
            state.apply_move(DIRS[di as usize]);
            new_cur.push(Node {
                state,
                prefix,
                board_hash: child_hash,
                potential,
            });
        }
        history.push(decisions);
        if I::ACTIVE {
            let snap: Vec<SnapshotNode> = new_cur
                .iter()
                .enumerate()
                .map(|(i, nd)| {
                    let m_i = cand_idx[i] as usize;
                    let (ev, parent_bi, di, _, _, prefix, _, potential) = cand_meta[m_i];
                    SnapshotNode {
                        prefix,
                        potential,
                        eval: ev as i32,
                        parent_bi,
                        di,
                    }
                })
                .collect();
            inspector.on_layer_selected(history.len(), &snap);
        }
        cur = new_cur;
    }
    let moves = reconstruct(&history, best_depth, best_idx);
    let depth = moves.len();
    (moves, best_prefix, depth, best_depth)
}
impl Solver for BeamSolver<'_> {
    fn solve<R: Rng + ?Sized>(&mut self, rng: &mut R) -> Output {
        let m = self.input.m;
        let tl_ms: u64 = std::env::var("BEAM_TL")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(1950);
        let tl = Duration::from_millis(tl_ms);
        let fixed_w: usize = std::env::var("BEAM_W")
            .ok()
            .and_then(|v| v.parse().ok())
            .unwrap_or(0);
        let mut best_moves = Vec::new();
        let mut best_prefix = 0usize;
        let mut best_score = usize::MAX;
        let mut best_w: usize = 0;
        let mut w = if fixed_w > 0 { fixed_w } else { 100 };
        loop {
            let iter_start = self.input.start_time.elapsed();
            if iter_start > tl {
                break;
            }
            let tabu_tenure: Option<usize> = std::env::var("TABU_TENURE")
                .ok()
                .and_then(|v| v.parse().ok());
            let mut noop = NoopInspector;
            let (moves, prefix, _, _) =
                beam_search(self.input, rng, w, tl, &mut noop, tabu_tenure, 0);
            #[cfg(feature = "debug")]
            eprintln!(
                "W={} prefix={} moves={} elapsed={}ms",
                w,
                prefix,
                moves.len(),
                self.input.start_time.elapsed().as_millis()
            );
            let better = prefix > best_prefix
                || (prefix == best_prefix && prefix >= m && moves.len() < best_score);
            if better {
                best_prefix = prefix;
                best_score = moves.len();
                best_moves = moves;
                best_w = w;
            }
            if fixed_w > 0 {
                break;
            }
            let iter_elapsed = self.input.start_time.elapsed() - iter_start;
            let remaining = tl.saturating_sub(self.input.start_time.elapsed());
            let next_w = w * 3 / 2;
            let est_next_ms = iter_elapsed.as_millis() as u64 * (next_w as u64).pow(2)
                / (w as u64).pow(2).max(1);
            if est_next_ms > remaining.as_millis() as u64 && remaining.as_millis() > 100 {
                w = 100;
            } else {
                w = next_w;
            }
        }
        Output { moves: best_moves }
    }
}
fn reconstruct(history: &[Vec<(u32, u8)>], depth: usize, idx: usize) -> Vec<Direction> {
    let depth = depth.min(history.len());
    let mut moves = Vec::with_capacity(depth);
    let mut cur_idx = idx;
    for d in (0..depth).rev() {
        if cur_idx >= history[d].len() {
            break;
        }
        let (parent, dir) = history[d][cur_idx];
        moves.push(DIRS[dir as usize]);
        cur_idx = parent as usize;
    }
    moves.reverse();
    moves
}
}
pub mod inspect {
use super::super::io::{Direction, Input};
use super::super::model::{DIRS, GameState};
use std::io::{self, BufRead, IsTerminal, Write};
#[derive(Clone, Copy, Debug)]
pub struct SnapshotNode {
    pub prefix: u16,
    pub potential: u16,
    pub eval: i32,
    pub parent_bi: u32,
    pub di: u8,
}
pub trait BeamInspector {
    const ACTIVE: bool;
    fn on_layer_selected(&mut self, layer: usize, selected: &[SnapshotNode]);
}
pub struct NoopInspector;
impl BeamInspector for NoopInspector {
    const ACTIVE: bool = false;
    #[inline(always)]
    fn on_layer_selected(&mut self, _layer: usize, _selected: &[SnapshotNode]) {}
}
#[derive(Default)]
pub struct RecordingInspector {
    pub snapshots: Vec<Vec<SnapshotNode>>,
}
impl BeamInspector for RecordingInspector {
    const ACTIVE: bool = true;
    fn on_layer_selected(&mut self, _layer: usize, selected: &[SnapshotNode]) {
        self.snapshots.push(selected.to_vec());
    }
}
#[derive(Default)]
pub struct ProfileInspector {
    pub max_prefix_per_layer: Vec<u16>,
    pub max_pot_per_layer: Vec<u16>,
    pub eval_range_per_layer: Vec<i32>,
    pub max_potential_seen: u16,
}
impl BeamInspector for ProfileInspector {
    const ACTIVE: bool = true;
    fn on_layer_selected(&mut self, _layer: usize, selected: &[SnapshotNode]) {
        if selected.is_empty() {
            return;
        }
        let max_pref = selected.iter().map(|n| n.prefix).max().unwrap();
        let max_pot = selected.iter().map(|n| n.potential).max().unwrap();
        let max_eval = selected.iter().map(|n| n.eval).max().unwrap();
        let min_eval = selected.iter().map(|n| n.eval).min().unwrap();
        self.max_prefix_per_layer.push(max_pref);
        self.max_pot_per_layer.push(max_pot);
        self.eval_range_per_layer.push(max_eval - min_eval);
        if max_pot > self.max_potential_seen {
            self.max_potential_seen = max_pot;
        }
    }
}
impl ProfileInspector {
    pub fn max_stall(&self) -> usize {
        if self.max_prefix_per_layer.len() < 2 {
            return 0;
        }
        let mut max_stall = 0usize;
        let mut cur_stall = 0usize;
        let mut prev = self.max_prefix_per_layer[0];
        for &p in self.max_prefix_per_layer.iter().skip(1) {
            if p > prev {
                prev = p;
                cur_stall = 0;
            } else {
                cur_stall += 1;
                if cur_stall > max_stall {
                    max_stall = cur_stall;
                }
            }
        }
        max_stall
    }
    pub fn avg_eval_range(&self) -> f64 {
        if self.eval_range_per_layer.is_empty() {
            return 0.0;
        }
        let sum: i64 = self.eval_range_per_layer.iter().map(|&v| v as i64).sum();
        sum as f64 / self.eval_range_per_layer.len() as f64
    }
}
pub struct BeamTrace {
    pub snapshots: Vec<Vec<SnapshotNode>>,
    pub initial: GameState,
    pub label: String,
}
pub fn path_moves(trace: &BeamTrace, layer: usize, idx: usize) -> Vec<Direction> {
    let mut path = Vec::new();
    let mut l = layer;
    let mut i = idx;
    while l > 0 {
        let node = &trace.snapshots[l][i];
        path.push(DIRS[node.di as usize]);
        i = node.parent_bi as usize;
        l -= 1;
    }
    path.reverse();
    path
}
pub fn reconstruct_state(trace: &BeamTrace, layer: usize, idx: usize) -> GameState {
    let moves = path_moves(trace, layer, idx);
    let mut state = trace.initial.clone();
    for m in moves {
        state.apply_move(m);
    }
    state
}
fn show_board(state: &GameState, input: &Input, focus_idx: usize, layer: usize) {
    let n = state.n();
    let mut body_by_cell: std::collections::HashMap<usize, usize> =
        std::collections::HashMap::new();
    let k = state.k();
    for i in (0..k).rev() {
        body_by_cell.insert(state.body(i) as usize, i);
    }
    let head_cell = state.head_cell();
    println!(
        "  layer={} idx={} prefix_matched={} k={} head=({},{})",
        layer,
        focus_idx,
        state.matched_prefix_len(&input.d),
        state.k(),
        head_cell / n,
        head_cell % n,
    );
    print!("   ");
    for c in 0..n {
        print!("{:>2} ", c);
    }
    println!();
    for r in 0..n {
        print!("{:>2} ", r);
        for c in 0..n {
            let cell = r * n + c;
            let food = state.board_get(cell);
            if let Some(&body_i) = body_by_cell.get(&cell) {
                if body_i == 0 {
                    print!(" @ ");
                } else if body_i == state.k() - 1 {
                    print!(" * ");
                } else {
                    print!(" # ");
                }
            } else if food > 0 {
                print!("{:>2} ", food);
            } else {
                print!(" . ");
            }
        }
        println!();
    }
}
fn list_layer(trace: &BeamTrace, layer: usize, n: usize) {
    let layer_nodes = &trace.snapshots[layer];
    let mut sorted: Vec<(usize, &SnapshotNode)> = layer_nodes.iter().enumerate().collect();
    sorted.sort_unstable_by(|(_, a), (_, b)| b.eval.cmp(&a.eval));
    println!(
        "=== layer {} (total {} nodes, sorted by eval desc, showing top {}) ===",
        layer,
        layer_nodes.len(),
        n.min(layer_nodes.len())
    );
    for (rank, (idx, nd)) in sorted.iter().take(n).enumerate() {
        println!(
            "  #{:>3} [idx {:>4}] eval={:>6} prefix={:>3} pot={:>3} parent={:>4} dir={}",
            rank,
            idx,
            nd.eval,
            nd.prefix,
            nd.potential,
            nd.parent_bi,
            DIRS[nd.di as usize].to_char()
        );
    }
}
fn stats_layer(trace: &BeamTrace, layer: usize) {
    let nodes = &trace.snapshots[layer];
    if nodes.is_empty() {
        println!("layer {} is empty", layer);
        return;
    }
    let n = nodes.len();
    let min_eval = nodes.iter().map(|n| n.eval).min().unwrap();
    let max_eval = nodes.iter().map(|n| n.eval).max().unwrap();
    let avg_eval: i64 = nodes.iter().map(|n| n.eval as i64).sum::<i64>() / n as i64;
    let min_pref = nodes.iter().map(|n| n.prefix).min().unwrap();
    let max_pref = nodes.iter().map(|n| n.prefix).max().unwrap();
    let min_pot = nodes.iter().map(|n| n.potential).min().unwrap();
    let max_pot = nodes.iter().map(|n| n.potential).max().unwrap();
    let n_ready = nodes.iter().filter(|_n| false).count();
    let _ = n_ready;
    println!("=== layer {} stats ({} nodes) ===", layer, n);
    println!(
        "  eval:     min={:>6} max={:>6} avg={:>6}",
        min_eval, max_eval, avg_eval
    );
    println!("  prefix:   min={:>3} max={:>3}", min_pref, max_pref);
    println!("  potential:min={:>3} max={:>3}", min_pot, max_pot);
}
fn hist_layer(trace: &BeamTrace, layer: usize) {
    let nodes = &trace.snapshots[layer];
    if nodes.is_empty() {
        println!("layer {} is empty", layer);
        return;
    }
    let mut counts: std::collections::BTreeMap<u16, usize> = Default::default();
    for nd in nodes.iter() {
        *counts.entry(nd.prefix).or_insert(0) += 1;
    }
    let max_count = *counts.values().max().unwrap();
    println!(
        "=== layer {} prefix histogram ({} nodes, {} distinct prefix) ===",
        layer,
        nodes.len(),
        counts.len()
    );
    for (pref, cnt) in counts.iter() {
        let bar_len = (*cnt * 40) / max_count.max(1);
        let bar: String = "█".repeat(bar_len);
        println!("  prefix {:>3}: {:>4} {}", pref, cnt, bar);
    }
}
fn show_chain(trace: &BeamTrace, layer: usize, idx: usize) {
    let mut chain = Vec::new();
    let mut l = layer;
    let mut i = idx;
    loop {
        let nd = &trace.snapshots[l][i];
        chain.push((l, i, *nd));
        if l == 0 {
            break;
        }
        i = nd.parent_bi as usize;
        l -= 1;
    }
    chain.reverse();
    println!("=== chain from root to (L={} I={}) ===", layer, idx);
    let mut prev_prefix: i32 = -1;
    for (l, i, nd) in chain.iter() {
        let advance = if *l > 0 && nd.prefix as i32 > prev_prefix {
            "+"
        } else {
            " "
        };
        println!(
            "  L{:>3} I{:>4} eval={:>5} prefix={:>3}{} pot={:>3} dir={}",
            l,
            i,
            nd.eval,
            nd.prefix,
            advance,
            nd.potential,
            DIRS[nd.di as usize].to_char()
        );
        prev_prefix = nd.prefix as i32;
    }
}
fn filter_prefix_layer(trace: &BeamTrace, layer: usize, target: u16, n: usize) {
    let nodes = &trace.snapshots[layer];
    let matches: Vec<(usize, &SnapshotNode)> = nodes
        .iter()
        .enumerate()
        .filter(|(_, nd)| nd.prefix == target)
        .collect();
    println!(
        "=== layer {} prefix={} ({} / {}) top {} ===",
        layer,
        target,
        matches.len(),
        nodes.len(),
        n.min(matches.len())
    );
    for (i, nd) in matches.iter().take(n) {
        println!(
            "  [{:>4}] eval={:>5} pot={:>3} parent={:>4} dir={}",
            i,
            nd.eval,
            nd.potential,
            nd.parent_bi,
            DIRS[nd.di as usize].to_char()
        );
    }
}
fn show_node(trace: &BeamTrace, layer: usize, idx: usize, input: &Input) {
    if layer >= trace.snapshots.len() || idx >= trace.snapshots[layer].len() {
        println!("out of range");
        return;
    }
    let nd = &trace.snapshots[layer][idx];
    println!(
        "[layer={} idx={}] eval={} prefix={} pot={} parent={} dir={}",
        layer,
        idx,
        nd.eval,
        nd.prefix,
        nd.potential,
        nd.parent_bi,
        DIRS[nd.di as usize].to_char()
    );
    let state = reconstruct_state(trace, layer, idx);
    show_board(&state, input, idx, layer);
}
pub fn run_repl(trace: BeamTrace, input: Input) {
    let max_layer = trace.snapshots.len().saturating_sub(1);
    let mut cur_layer = max_layer;
    let mut cur_idx = 0usize;
    let is_tty = io::stdin().is_terminal();
    if is_tty {
        println!(
            "[{}] beam loaded: {} layers. 'help' for commands.",
            trace.label,
            trace.snapshots.len()
        );
        if !trace.snapshots.is_empty() {
            list_layer(&trace, cur_layer, 10);
        }
    } else {
        println!(
            "# [{}] beam loaded: {} layers",
            trace.label,
            trace.snapshots.len()
        );
    }
    let stdin = io::stdin();
    let mut input_line = String::new();
    loop {
        if is_tty {
            print!("[L={} I={}] > ", cur_layer, cur_idx);
            io::stdout().flush().ok();
        }
        input_line.clear();
        if stdin.lock().read_line(&mut input_line).is_err() {
            break;
        }
        let parts: Vec<&str> = input_line.trim().split_whitespace().collect();
        if parts.is_empty() {
            continue;
        }
        match parts[0] {
            "q" | "quit" | "exit" => break,
            "help" | "?" => {
                println!(
                    "commands: layer <T> | list [N] | sel <I> | parent | children [N] | board | path | info"
                );
                println!("          stats | hist | chain | filter prefix <N> [top] | q");
            }
            "stats" => stats_layer(&trace, cur_layer),
            "hist" => hist_layer(&trace, cur_layer),
            "chain" => show_chain(&trace, cur_layer, cur_idx),
            "trace" => {
                println!("layer,max_prefix,max_pot,eval_range,stall_len");
                let mut stall = 0i32;
                let mut prev_prefix: i32 = -1;
                for (l, layer_nodes) in trace.snapshots.iter().enumerate() {
                    if layer_nodes.is_empty() {
                        continue;
                    }
                    let max_pref = layer_nodes.iter().map(|n| n.prefix).max().unwrap();
                    let max_pot = layer_nodes.iter().map(|n| n.potential).max().unwrap();
                    let max_eval = layer_nodes.iter().map(|n| n.eval).max().unwrap();
                    let min_eval = layer_nodes.iter().map(|n| n.eval).min().unwrap();
                    if prev_prefix >= 0 && (max_pref as i32) > prev_prefix {
                        stall = 0;
                    } else if prev_prefix >= 0 {
                        stall += 1;
                    }
                    println!("{},{},{},{},{}", l, max_pref, max_pot, max_eval - min_eval, stall);
                    prev_prefix = max_pref as i32;
                }
            }
            "filter" => {
                if parts.get(1) == Some(&"prefix") {
                    if let Some(p) = parts.get(2).and_then(|s| s.parse::<u16>().ok()) {
                        let n = parts.get(3).and_then(|s| s.parse::<usize>().ok()).unwrap_or(20);
                        filter_prefix_layer(&trace, cur_layer, p, n);
                    } else {
                        println!("usage: filter prefix <N> [top]");
                    }
                } else {
                    println!("usage: filter prefix <N> [top]");
                }
            }
            "layer" => {
                if let Some(n) = parts.get(1).and_then(|s| s.parse::<usize>().ok()) {
                    if n < trace.snapshots.len() {
                        cur_layer = n;
                        cur_idx = 0;
                        list_layer(&trace, cur_layer, 10);
                    } else {
                        println!("layer out of range (max {})", max_layer);
                    }
                } else {
                    println!("usage: layer <T>");
                }
            }
            "list" | "ls" => {
                let n = parts.get(1).and_then(|s| s.parse::<usize>().ok()).unwrap_or(10);
                list_layer(&trace, cur_layer, n);
            }
            "sel" | "node" => {
                if let Some(i) = parts.get(1).and_then(|s| s.parse::<usize>().ok()) {
                    if i < trace.snapshots[cur_layer].len() {
                        cur_idx = i;
                        show_node(&trace, cur_layer, cur_idx, &input);
                    } else {
                        println!("idx out of range");
                    }
                } else {
                    println!("usage: sel <I>");
                }
            }
            "parent" | "up" => {
                if cur_layer == 0 {
                    println!("already at root");
                } else {
                    cur_idx = trace.snapshots[cur_layer][cur_idx].parent_bi as usize;
                    cur_layer -= 1;
                    show_node(&trace, cur_layer, cur_idx, &input);
                }
            }
            "children" | "ch" => {
                let n = parts.get(1).and_then(|s| s.parse::<usize>().ok()).unwrap_or(20);
                if cur_layer + 1 >= trace.snapshots.len() {
                    println!("no children (at max layer)");
                } else {
                    let next = &trace.snapshots[cur_layer + 1];
                    let mut children: Vec<(usize, &SnapshotNode)> = next
                        .iter()
                        .enumerate()
                        .filter(|(_, nd)| nd.parent_bi as usize == cur_idx)
                        .collect();
                    children.sort_unstable_by(|(_, a), (_, b)| b.eval.cmp(&a.eval));
                    println!(
                        "=== children of (L={} I={}): {} total, showing top {} ===",
                        cur_layer,
                        cur_idx,
                        children.len(),
                        n.min(children.len())
                    );
                    for (rank, (global_i, nd)) in children.iter().take(n).enumerate() {
                        println!(
                            "  [{:>3}] global_idx={:>4} eval={:>5} prefix={:>3} pot={:>3} dir={}",
                            rank,
                            global_i,
                            nd.eval,
                            nd.prefix,
                            nd.potential,
                            DIRS[nd.di as usize].to_char()
                        );
                    }
                }
            }
            "board" | "b" => {
                let state = reconstruct_state(&trace, cur_layer, cur_idx);
                show_board(&state, &input, cur_idx, cur_layer);
            }
            "path" => {
                let moves = path_moves(&trace, cur_layer, cur_idx);
                for (i, m) in moves.iter().enumerate() {
                    print!("{}", m.to_char());
                    if (i + 1) % 30 == 0 {
                        println!();
                    }
                }
                if !moves.is_empty() && moves.len() % 30 != 0 {
                    println!();
                }
                println!("(total {} moves)", moves.len());
            }
            "info" => {
                show_node(&trace, cur_layer, cur_idx, &input);
            }
            _ => println!("unknown. 'help' for commands"),
        }
    }
}
}
pub mod local {
use super::super::io::{Direction, Input};
use super::super::model::{GameState, MoveResult};
pub fn extract_eating_plan(input: &Input, moves: &[Direction]) -> Vec<EatEvent> {
    let mut state = GameState::new(input);
    let mut events = Vec::new();
    let mut bite_buf = [0u8; 64];
    let mut prefix = state.matched_prefix_len(&input.d);
    for (step, &dir) in moves.iter().enumerate() {
        let old_k = state.k;
        let old_hi = state.head_idx;
        let (result, eat_color, bite_count) = state.apply_raw(dir, &mut bite_buf);
        match result {
            MoveResult::Eat(_) => {
                let new_prefix = state.matched_prefix_len(&input.d);
                let is_correct = new_prefix > prefix;
                events.push(EatEvent {
                    step,
                    cell: state.head_cell(),
                    color: eat_color,
                    is_correct,
                    prefix_after: new_prefix,
                });
                prefix = new_prefix;
            }
            MoveResult::Bite(_) => {
                prefix = state.matched_prefix_len(&input.d);
                events.push(EatEvent {
                    step,
                    cell: state.head_cell(),
                    color: 0,
                    is_correct: false,
                    prefix_after: prefix,
                });
            }
            MoveResult::Move => {}
        }
        let _ = (old_k, old_hi, bite_count);
    }
    events
}
#[derive(Debug, Clone)]
pub struct EatEvent {
    pub step: usize,
    pub cell: usize,
    pub color: u8,
    pub is_correct: bool,
    pub prefix_after: usize,
}
}
pub trait Solver {
    fn solve<R: Rng + ?Sized>(&mut self, rng: &mut R) -> Output;
}
}
}
fn main() {
    let mut io = IO::default();
    let input = io.read();
    let mut rng = Pcg64Mcg::seed_from_u64(42);
    let engine = std::env::var("SOLVER_ENGINE").unwrap_or_else(|_| "beam".to_string());
    let output = match engine.as_str() {
        "beam" => BeamSolver::new(&input).solve(&mut rng),
        _ => panic!("unknown SOLVER_ENGINE: {}", engine),
    };
    io.write(&output);
    std::process::exit(0);
}
