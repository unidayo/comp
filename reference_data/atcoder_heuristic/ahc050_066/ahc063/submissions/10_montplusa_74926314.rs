use std::io::{self, Read, Write};
use std::time::{Duration, Instant};

// Global compile-time limits taken from the problem constraints.
// The implementation is written assuming MAX_N=16 and a fixed 256-cell board,
// so many arrays are sized for that full board even when the actual N is smaller.
const MAX_N: usize = 16;
const MAX_CELLS: usize = MAX_N * MAX_N;
const MAX_COLOR: usize = 8;
const MAX_M: usize = 3 * MAX_N * MAX_N / 4;
const MAX_B: usize = 8;
const INF: i32 = 1 << 30;
const RECOVER_LIMIT: usize = 12;
const SCORE_JITTER_SCALE: u64 = 1024;
const BEAM_RUN_LIMIT: i32 = 0;
const WANTED_NEXT_LOOK: usize = 1;
const DEADLINE_MS: u64 = 1880;
const DEADLINE_EXTRA_MS: u64 = 1980;
const FIRST_BEAM_WIDTH: u16 = 1024;
const W_JITTER: f32 = 1.0;
const W_MISSING_EFF: f32 = 100000.0;
const W_RECOV: f32 = 8.0;
const W_CENTER: f32 = 0.2;
const W_CLEAN_WANTED: f32 = 8.0;
const W_DIRTY_BASE: f32 = 16.0;
const W_DIRTY_CLEAR: f32 = 3.0;
const W_DIRTY_WANTED: f32 = 8.0;
const SPARSE_FOOD_THRESHOLD: u8 = 6;

const DIR_U: u8 = 0;
const DIR_L: u8 = 1;
const DIR_D: u8 = 2;
const DIR_R: u8 = 3;
const CELL_NONE: u16 = u16::MAX;

// Very small RNG used only for score jitter between nearly-equal candidates.
#[derive(Clone, Copy)]
struct XorShift64 {
    x: u64,
}

impl XorShift64 {
    // Returns a new pseudo-random 64-bit value.
    #[inline(always)]
    fn next(&mut self) -> u64 {
        let mut x = self.x;
        x ^= x << 7;
        x ^= x >> 9;
        x ^= x << 8;
        self.x = x;
        x
    }
}

// Minimal stdin scanner. We only need signed integers.
struct Scanner {
    buf: Vec<u8>,
    idx: usize,
}

impl Scanner {
    #[inline(always)]
    fn from_bytes(buf: Vec<u8>) -> Self {
        Self { buf, idx: 0 }
    }

    // Parses the next integer token.
    #[inline(always)]
    fn next_i32(&mut self) -> i32 {
        while self.idx < self.buf.len() && self.buf[self.idx].is_ascii_whitespace() {
            self.idx += 1;
        }
        let mut sign = 1i32;
        if self.idx < self.buf.len() && self.buf[self.idx] == b'-' {
            sign = -1;
            self.idx += 1;
        }
        let mut x = 0i32;
        while self.idx < self.buf.len() && !self.buf[self.idx].is_ascii_whitespace() {
            x = x * 10 + (self.buf[self.idx] - b'0') as i32;
            self.idx += 1;
        }
        x * sign
    }
}

// Trace node for path reconstruction.
#[derive(Clone, Copy, Default)]
struct Trace {
    parent: i32,
    mv: u8,
}

// Main beam state.
//
// Important representation choices:
// - We do not store pos[].
// - The body is reconstructed from:
//   - head
//   - last_visit[cell]
//   - from_dir bits for "which direction did we enter this cell from last time"
// - tail and clear_head are cached because they are queried often in hot paths.
#[derive(Clone, Copy)]
struct BeamState {
    // Current head cell.
    head: u8,
    // Current tail cell. Maintained incrementally in apply_move.
    tail: u8,
    // Body cell immediately before tail.
    // This is cached because plain non-eat moves advance tail <- tail_prev.
    tail_prev: u8,
    // Cell at body index q-1 when q>0. This is the point we target when
    // estimating the cost to clean a dirty suffix.
    clear_head: u8,
    // Current snake length.
    plen: u8,
    // Longest exact prefix already fixed.
    q: u8,
    // Dirty suffix colors. When blen>0, buf[0] is the first mismatch after q.
    buf: [u8; MAX_B],
    blen: u8,
    // Last time each cell was visited by the snake.
    last_visit: [i16; MAX_CELLS],
    // Incoming direction for each cell.
    from_dir: [u8; MAX_CELLS],
    // Food state: O(1) point color lookup.
    food_color: [u8; MAX_CELLS],
    // Sparse-color acceleration:
    // when a color has only a few cells left, iterate its bitboard directly.
    food_bb: [[u64; 4]; MAX_COLOR],
    food_count: [u8; MAX_COLOR],
    // Sum of center distances of all remaining foods.
    food_center_sum: i32,
    // Number of moves already taken.
    steps: i32,
    // Trace index of this state.
    trace: i32,
    // Extra exact chain recoverable from this state, computed lazily.
    recov: u8,
    // End cell of the chosen recoverable exact chain. Equals head when recov==0.
    recov_end: u8,
    // Cells consumed by that recoverable chain, excluding head.
    recov_mask: [u64; 4],
}

impl Default for BeamState {
    fn default() -> Self {
        Self {
            head: 0,
            tail: 0,
            tail_prev: 0,
            clear_head: 0,
            plen: 0,
            q: 0,
            buf: [0; MAX_B],
            blen: 0,
            last_visit: [-1000; MAX_CELLS],
            from_dir: [0; MAX_CELLS],
            food_color: [0; MAX_CELLS],
            food_bb: [[0; 4]; MAX_COLOR],
            food_count: [0; MAX_COLOR],
            food_center_sum: 0,
            steps: 0,
            trace: 0,
            recov: 0,
            recov_end: 0,
            recov_mask: [0; 4],
        }
    }
}

// Materialized candidate kept after one-ply generation.
#[derive(Clone, Copy, Default)]
struct Candidate {
    score: f32,
    hash: u64,
    recov: u8,
    recov_end: u8,
    recov_mask: [u64; 4],
    parent_idx: u16,
    mv: u8,
}

// Summary of a non-bite child that can be evaluated without materializing BeamState.
#[derive(Clone, Copy, Default)]
struct FastSummary {
    score: f32,
    hash: u64,
    stop: i32,
    recov: u8,
    recov_end: u8,
    recov_mask: [u64; 4],
    q: u8,
    blen: u8,
    steps: i32,
}

// Phase parameters that actually vary at runtime.
#[derive(Clone, Copy)]
struct Phase {
    max_b: u8,
    width: u16,
}

#[derive(Clone, Copy, Default)]
struct RecoverInfo {
    len: u8,
    end: u8,
    mask: [u64; 4],
}

struct BeamScratch {
    cands: Vec<Candidate>,
    order: Vec<usize>,
    next: Vec<BeamState>,
    turn_hash_keys: Vec<u64>,
    turn_hash_stamp: Vec<u32>,
    turn_hash_epoch: u32,
}

impl BeamScratch {
    fn new() -> Self {
        Self {
            cands: Vec::new(),
            order: Vec::new(),
            next: Vec::new(),
            turn_hash_keys: vec![0; 1024],
            turn_hash_stamp: vec![0; 1024],
            turn_hash_epoch: 1,
        }
    }

    fn ensure_width(&mut self, width: usize) {
        let cand_cap = width * 3;
        if self.cands.capacity() < cand_cap {
            self.cands.reserve(cand_cap - self.cands.capacity());
        }
        if self.order.capacity() < cand_cap {
            self.order.reserve(cand_cap - self.order.capacity());
        }
        if self.next.len() < width {
            self.next.resize(width, BeamState::default());
        }
        let turn_hash_cap = (width * 4).next_power_of_two().max(1024);
        if self.turn_hash_keys.len() < turn_hash_cap {
            self.turn_hash_keys.resize(turn_hash_cap, 0);
            self.turn_hash_stamp.resize(turn_hash_cap, 0);
        }
    }
}

// Solver-wide immutable precomputation plus a few scratch buffers reused across calls.
struct Solver {
    n: usize,
    m: usize,
    c: usize,
    target: [u8; MAX_M],
    next_cell: [[u16; 4]; MAX_CELLS],
    cell_dist: [[u8; MAX_CELLS]; MAX_CELLS],
    near_cell_order: [[u8; MAX_CELLS]; MAX_CELLS],
    near_cell_count: usize,
    center_dist: [u8; MAX_CELLS],
    adj: [[u8; 4]; MAX_CELLS],
    adj_dir: [[u8; 4]; MAX_CELLS],
    adj_cnt: [u8; MAX_CELLS],
    move_dir: [[u8; 4]; MAX_CELLS],
    move_cnt: [u8; MAX_CELLS],
    beam_rand: XorShift64,
    mark_epoch: u32,
    shift_visit_marks: [u32; MAX_CELLS],
}

impl Solver {
    #[inline(always)]
    fn mask_set(mask: &mut [u64; 4], id: u8) {
        let idx = id as usize;
        mask[idx >> 6] |= 1u64 << (idx & 63);
    }

    #[inline(always)]
    fn mask_clear(mask: &mut [u64; 4], id: u8) {
        let idx = id as usize;
        mask[idx >> 6] &= !(1u64 << (idx & 63));
    }

    // Zero-initialized solver shell. Real problem parameters are filled after input.
    #[inline(always)]
    fn new() -> Self {
        Self {
            n: 0,
            m: 0,
            c: 0,
            target: [0; MAX_M],
            next_cell: [[CELL_NONE; 4]; MAX_CELLS],
            cell_dist: [[0; MAX_CELLS]; MAX_CELLS],
            near_cell_order: [[0; MAX_CELLS]; MAX_CELLS],
            near_cell_count: 0,
            center_dist: [0; MAX_CELLS],
            adj: [[0; 4]; MAX_CELLS],
            adj_dir: [[0; 4]; MAX_CELLS],
            adj_cnt: [0; MAX_CELLS],
            move_dir: [[0; 4]; MAX_CELLS],
            move_cnt: [0; MAX_CELLS],
            beam_rand: XorShift64 { x: 1 },
            mark_epoch: 1,
            shift_visit_marks: [0; MAX_CELLS],
        }
    }

    // Maps board coordinates into our fixed 16x16 packed id space.
    #[inline(always)]
    fn cell_id(&self, i: usize, j: usize) -> u8 {
        (i * MAX_N + j) as u8
    }

    // Inverse of cell_id.
    #[inline(always)]
    fn row_col(&self, id: u8) -> (usize, usize) {
        let v = id as usize;
        (v / MAX_N, v & (MAX_N - 1))
    }

    // Builds all static tables used in hot loops:
    // - next_cell: direct 4-direction transition table with CELL_NONE sentinel
    // - adj/adj_cnt: compact neighbor lists for DFS-style traversals
    // - center_dist: distance to board center proxy
    // - cell_dist: full Manhattan distance table
    // - near_cell_order: for each head cell, all valid cells sorted by distance
    fn init_precompute(&mut self) {
        let mut valid = [0u8; MAX_CELLS];
        let mut len = 0usize;
        let c0 = (self.n - 1) >> 1;
        let c1 = self.n >> 1;

        let mut i = 0usize;
        while i < self.n {
            let mut j = 0usize;
            while j < self.n {
                let id = self.cell_id(i, j);
                valid[len] = id;
                len += 1;

                let mut adjn = 0usize;
                if i > 0 {
                    let nid = self.cell_id(i - 1, j);
                    self.next_cell[id as usize][DIR_U as usize] = nid as u16;
                    self.adj[id as usize][adjn] = nid;
                    self.adj_dir[id as usize][adjn] = DIR_U;
                    adjn += 1;
                } else {
                    self.next_cell[id as usize][DIR_U as usize] = CELL_NONE;
                }
                if i + 1 < self.n {
                    let nid = self.cell_id(i + 1, j);
                    self.next_cell[id as usize][DIR_D as usize] = nid as u16;
                    self.adj[id as usize][adjn] = nid;
                    self.adj_dir[id as usize][adjn] = DIR_D;
                    adjn += 1;
                } else {
                    self.next_cell[id as usize][DIR_D as usize] = CELL_NONE;
                }
                if j > 0 {
                    let nid = self.cell_id(i, j - 1);
                    self.next_cell[id as usize][DIR_L as usize] = nid as u16;
                    self.adj[id as usize][adjn] = nid;
                    self.adj_dir[id as usize][adjn] = DIR_L;
                    adjn += 1;
                } else {
                    self.next_cell[id as usize][DIR_L as usize] = CELL_NONE;
                }
                if j + 1 < self.n {
                    let nid = self.cell_id(i, j + 1);
                    self.next_cell[id as usize][DIR_R as usize] = nid as u16;
                    self.adj[id as usize][adjn] = nid;
                    self.adj_dir[id as usize][adjn] = DIR_R;
                    adjn += 1;
                } else {
                    self.next_cell[id as usize][DIR_R as usize] = CELL_NONE;
                }
                self.adj_cnt[id as usize] = adjn as u8;

                let mut mdn = 0usize;
                let move_order = [DIR_R, DIR_U, DIR_D, DIR_L];
                let mut t = 0usize;
                while t < 4 {
                    let mv = move_order[t];
                    if self.next_cell[id as usize][mv as usize] != CELL_NONE {
                        self.move_dir[id as usize][mdn] = mv;
                        mdn += 1;
                    }
                    t += 1;
                }
                self.move_cnt[id as usize] = mdn as u8;

                let d00 = i.abs_diff(c0) + j.abs_diff(c0);
                let d01 = i.abs_diff(c0) + j.abs_diff(c1);
                let d10 = i.abs_diff(c1) + j.abs_diff(c0);
                let d11 = i.abs_diff(c1) + j.abs_diff(c1);
                let mut best = d00;
                if d01 < best {
                    best = d01;
                }
                if d10 < best {
                    best = d10;
                }
                if d11 < best {
                    best = d11;
                }
                self.center_dist[id as usize] = best as u8;
                j += 1;
            }
            i += 1;
        }

        self.near_cell_count = len;

        let mut a = 0usize;
        while a < len {
            let aid = valid[a] as usize;
            let (ai, aj) = self.row_col(valid[a]);
            let mut b = 0usize;
            while b < len {
                let bid = valid[b] as usize;
                let (bi, bj) = self.row_col(valid[b]);
                self.cell_dist[aid][bid] = (ai.abs_diff(bi) + aj.abs_diff(bj)) as u8;
                b += 1;
            }
            self.near_cell_order[aid][..len].copy_from_slice(&valid[..len]);
            self.near_cell_order[aid][..len].sort_unstable_by(|&x, &y| {
                self.cell_dist[aid][x as usize]
                    .cmp(&self.cell_dist[aid][y as usize])
                    .then(x.cmp(&y))
            });
            a += 1;
        }
    }

    // Removes one food from a cell.
    #[inline(always)]
    fn food_clear_at(&self, st: &mut BeamState, id: u8) {
        let color = st.food_color[id as usize] as usize;
        st.food_color[id as usize] = 0;
        let idx = id as usize;
        st.food_bb[color][idx >> 6] &= !(1u64 << (idx & 63));
        st.food_count[color] -= 1;
    }

    // Places one food onto a cell.
    #[inline(always)]
    fn food_set_at(&self, st: &mut BeamState, id: u8, color: u8) {
        st.food_color[id as usize] = color;
        let idx = id as usize;
        let c = color as usize;
        st.food_bb[c][idx >> 6] |= 1u64 << (idx & 63);
        st.food_count[c] += 1;
    }

    // Decodes the 2-bit incoming direction stored for a cell.
    #[inline(always)]
    fn get_from_dir(&self, st: &BeamState, id: u8) -> u8 {
        st.from_dir[id as usize]
    }

    // Writes the 2-bit incoming direction for a cell.
    #[inline(always)]
    fn set_from_dir(&self, st: &mut BeamState, id: u8, dir: u8) {
        st.from_dir[id as usize] = dir;
    }

    // Returns the previous body cell on the stored trajectory.
    // For the head this is the neck.
    #[inline(always)]
    fn prev_cell(&self, st: &BeamState, id: u8) -> u8 {
        let dir = self.get_from_dir(st, id);
        self.next_cell[id as usize][dir as usize] as u8
    }

    // Opposite direction. With the chosen encoding this is just xor by 2.
    #[inline(always)]
    fn reverse_dir(&self, mv: u8) -> u8 {
        mv ^ 2
    }

    #[inline(always)]
    fn turn_hash_insert(
        keys: &mut [u64],
        stamp: &mut [u32],
        epoch: u32,
        hash: u64,
    ) -> bool {
        let mask = keys.len() - 1;
        let mut idx = (hash as usize) & mask;
        loop {
            if stamp[idx] != epoch {
                stamp[idx] = epoch;
                keys[idx] = hash;
                return true;
            }
            if keys[idx] == hash {
                return false;
            }
            idx = (idx + 1) & mask;
        }
    }

    // Copies only the base state that apply_move actually needs to preserve.
    // This intentionally skips head/steps/trace/recov* because they are always
    // overwritten or reconstructed by the caller.
    #[inline(always)]
    fn copy_state_for_move(&self, parent: &BeamState, child: &mut BeamState) {
        child.tail = parent.tail;
        child.tail_prev = parent.tail_prev;
        child.clear_head = parent.clear_head;
        child.plen = parent.plen;
        child.q = parent.q;
        child.buf = parent.buf;
        child.blen = parent.blen;
        child.last_visit = parent.last_visit;
        child.from_dir = parent.from_dir;
        child.food_color = parent.food_color;
        child.food_bb = parent.food_bb;
        child.food_count = parent.food_count;
        child.food_center_sum = parent.food_center_sum;
    }

    // For a normal non-eat move, tail <- old tail_prev and the new tail_prev is the
    // cell whose visit time matches old body index plen-3.
    #[inline(always)]
    fn shift_tail_prev(&self, parent: &BeamState) -> u8 {
        let want_visit = (parent.steps - parent.plen as i32 + 3) as i16;
        let cur = parent.tail_prev;
        let cnt = self.adj_cnt[cur as usize];
        let row = &self.adj[cur as usize];
        if cnt >= 1 {
            let nid = row[0];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        if cnt >= 2 {
            let nid = row[1];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        if cnt >= 3 {
            let nid = row[2];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        if cnt >= 4 {
            let nid = row[3];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        cur
    }

    // After biting into nid, the new tail is nid itself and the new tail_prev is
    // the cell that was immediately before that old occurrence on the old body.
    #[inline(always)]
    fn bite_tail_prev(&self, parent: &BeamState, nid: u8, delta: i32) -> u8 {
        let want_visit = (parent.steps - delta + 2) as i16;
        let cnt = self.adj_cnt[nid as usize];
        let row = &self.adj[nid as usize];
        if cnt >= 1 {
            let cid = row[0];
            if parent.last_visit[cid as usize] == want_visit {
                return cid;
            }
        }
        if cnt >= 2 {
            let cid = row[1];
            if parent.last_visit[cid as usize] == want_visit {
                return cid;
            }
        }
        if cnt >= 3 {
            let cid = row[2];
            if parent.last_visit[cid as usize] == want_visit {
                return cid;
            }
        }
        if cnt >= 4 {
            let cid = row[3];
            if parent.last_visit[cid as usize] == want_visit {
                return cid;
            }
        }
        nid
    }

    // Advances the epoch for reusable visited-mark arrays.
    // When wrapping, clears the entire mark array once.
    #[inline(always)]
    fn next_mark_epoch(&mut self) -> u32 {
        self.mark_epoch = self.mark_epoch.wrapping_add(1);
        if self.mark_epoch == 0 {
            self.shift_visit_marks.fill(0);
            self.mark_epoch = 1;
        }
        self.mark_epoch
    }

    #[inline(always)]
    fn body_contains_no_tail_at(&self, st: &BeamState, id: u8, steps_at: i32) -> bool {
        let last = st.last_visit[id as usize];
        last < steps_at as i16 && last >= (steps_at - st.plen as i32 + 2) as i16
    }

    // DFS used by compute_recoverable.
    // It follows exact-color foods only, starting from the head neighborhood.
    fn compute_recoverable_dfs(
        &mut self,
        cur: u8,
        depth: usize,
        q: usize,
        limit: usize,
        st: &BeamState,
        visit_epoch: u32,
        path_mask: &mut [u64; 4],
        best: &mut RecoverInfo,
    ) {
        if depth > best.len as usize {
            best.len = depth as u8;
            best.end = cur;
            best.mask = *path_mask;
            if depth >= limit || q + depth >= self.m {
                return;
            }
        }
        if depth >= limit || q + depth >= self.m {
            return;
        }
        let need = self.target[q + depth];
        let cnt = self.adj_cnt[cur as usize] as usize;
        let mut i = 0usize;
        while i < cnt {
            let nid = self.adj[cur as usize][i];
            let id = nid as usize;
            if self.shift_visit_marks[id] != visit_epoch && st.food_color[id] == need {
                self.shift_visit_marks[id] = visit_epoch;
                Solver::mask_set(path_mask, nid);
                self.compute_recoverable_dfs(nid, depth + 1, q, limit, st, visit_epoch, path_mask, best);
                Solver::mask_clear(path_mask, nid);
                self.shift_visit_marks[id] = 0;
                if best.len as usize >= limit || q + best.len as usize >= self.m {
                    return;
                }
            }
            i += 1;
        }
    }

    // Returns how many extra exact colors are immediately recoverable from a clean state.
    // This is an optimistic short exact-chain DFS used only as a heuristic.
    fn compute_recoverable(&mut self, st: &BeamState, limit: usize) -> RecoverInfo {
        if st.blen != 0 || st.q as usize >= self.m || limit == 0 {
            return RecoverInfo {
                len: 0,
                end: st.head,
                mask: [0; 4],
            };
        }
        let need0 = self.target[st.q as usize];
        let cnt = self.adj_cnt[st.head as usize] as usize;
        let mut i = 0usize;
        let mut has_start = false;
        while i < cnt {
            let nid = self.adj[st.head as usize][i];
            if st.food_color[nid as usize] == need0 {
                has_start = true;
                break;
            }
            i += 1;
        }
        if !has_start {
            return RecoverInfo {
                len: 0,
                end: st.head,
                mask: [0; 4],
            };
        }
        let mut best = RecoverInfo {
            len: 0,
            end: st.head,
            mask: [0; 4],
        };
        let mut path_mask = [0u64; 4];
        let visit_epoch = self.next_mark_epoch();
        self.shift_visit_marks[st.head as usize] = visit_epoch;
        self.compute_recoverable_dfs(
            st.head,
            0,
            st.q as usize,
            limit,
            st,
            visit_epoch,
            &mut path_mask,
            &mut best,
        );
        self.shift_visit_marks[st.head as usize] = 0;
        best
    }

    // Recomputes state-derived fields that are not updated inline everywhere.
    #[inline(always)]
    fn refresh_derived(&mut self, st: &mut BeamState) {
        let rec = self.compute_recoverable(st, RECOVER_LIMIT);
        st.recov = rec.len;
        st.recov_end = rec.end;
        st.recov_mask = rec.mask;
    }

    fn nearest_color_dist_with_skip(
        &self,
        st: &BeamState,
        head: u8,
        want: u8,
        skip_id: i32,
        skip_mask: &[u64; 4],
    ) -> i32 {
        let mut best = (self.n * self.n) as i32;
        let skip_empty = (skip_mask[0] | skip_mask[1] | skip_mask[2] | skip_mask[3]) == 0;
        if st.food_count[want as usize] <= SPARSE_FOOD_THRESHOLD {
            let mut w = 0usize;
            while w < 4 {
                let mut bits = st.food_bb[want as usize][w];
                while bits != 0 {
                    let tz = bits.trailing_zeros() as usize;
                    bits &= bits - 1;
                    let id = ((w << 6) + tz) as u8;
                    if id as i32 != skip_id && (skip_empty || ((skip_mask[w] >> tz) & 1) == 0) {
                        let base = self.cell_dist[head as usize][id as usize] as i32;
                        if base < best {
                            best = base;
                        }
                    }
                }
                w += 1;
            }
        } else {
            let order = &self.near_cell_order[head as usize];
            let drow = &self.cell_dist[head as usize];
            let mut i = 0usize;
            while i < self.near_cell_count {
                let id = order[i];
                let base = drow[id as usize] as i32;
                if base >= best {
                    break;
                }
                let w = (id as usize) >> 6;
                let bit = 1u64 << ((id as usize) & 63);
                if id as i32 != skip_id
                    && (skip_empty || (skip_mask[w] & bit) == 0)
                    && st.food_color[id as usize] == want
                {
                    best = base;
                    break;
                }
                i += 1;
            }
        }
        best
    }

    // Main wanted-distance heuristic on a materialized state.
    // Score is based on:
    // - d1: distance to current wanted color
    // - seal penalty for dead-end foods
    // - d2/2: distance from that chosen food to the next wanted color
    fn nearest_wanted_dist(&mut self, st: &BeamState) -> i32 {
        let q0 = st.q as usize + st.recov as usize;
        if q0 >= self.m {
            return 0;
        }
        let want = self.target[q0];
        let has_next = q0 + 1 < self.m;
        let next_want = if has_next { self.target[q0 + 1] } else { 0 };
        let use_recov = st.recov != 0;
        let head = if use_recov { st.recov_end } else { st.head };
        let empty_mask = [0u64; 4];
        let skip_mask = if use_recov { &st.recov_mask } else { &empty_mask };
        let skip_empty = !use_recov;
        let mut best = (self.n * self.n) as i32;
        let mut best_score = INF;
        let mut best_next = (self.n * self.n) as i32;
        let mut seen = 0usize;
        if st.food_count[want as usize] <= SPARSE_FOOD_THRESHOLD {
            let mut w = 0usize;
            while w < 4 {
                let mut bits = st.food_bb[want as usize][w];
                while bits != 0 {
                    let tz = bits.trailing_zeros() as usize;
                    bits &= bits - 1;
                    let id = ((w << 6) + tz) as u8;
                    if !skip_empty && ((skip_mask[w] >> tz) & 1) != 0 {
                        continue;
                    }
                    let base = self.cell_dist[head as usize][id as usize] as i32;
                    if base >= best {
                        continue;
                    }
                    let cnt = self.adj_cnt[id as usize] as usize;
                    let mut sealed = true;
                    let mut has_body = false;
                    let mut k = 0usize;
                    while k < cnt {
                        let nid = self.adj[id as usize][k];
                        if self.body_contains_no_tail_at(st, nid, st.steps) {
                            has_body = true;
                            k += 1;
                            continue;
                        }
                        let w = (nid as usize) >> 6;
                        let bit = 1u64 << ((nid as usize) & 63);
                        if (!skip_empty && (skip_mask[w] & bit) != 0) || st.food_color[nid as usize] == 0 {
                            sealed = false;
                            break;
                        }
                        k += 1;
                    }
                    let mut d = base;
                    if sealed {
                        d += if has_body { 4 } else { 8 };
                    }
                    let mut score = d;
                    let mut next_dist = 0;
                    if has_next {
                        next_dist = self.nearest_color_dist_with_skip(st, id, next_want, id as i32, skip_mask);
                        score += next_dist / 2;
                    }
                    if score < best_score || (score == best_score && (d < best || (d == best && next_dist < best_next))) {
                        best_score = score;
                        best = d;
                        best_next = next_dist;
                    }
                    seen += 1;
                }
                w += 1;
            }
        } else {
            let order = &self.near_cell_order[head as usize];
            let drow = &self.cell_dist[head as usize];
            let mut i = 0usize;
            while i < self.near_cell_count {
                let id = order[i];
                let base = drow[id as usize] as i32;
                if base >= best {
                    break;
                }
                let w = (id as usize) >> 6;
                let bit = 1u64 << ((id as usize) & 63);
                if (!skip_empty && (skip_mask[w] & bit) != 0) || st.food_color[id as usize] != want {
                    i += 1;
                    continue;
                }
                let cnt = self.adj_cnt[id as usize] as usize;
                let mut sealed = true;
                let mut has_body = false;
                let mut k = 0usize;
                    while k < cnt {
                        let nid = self.adj[id as usize][k];
                        if self.body_contains_no_tail_at(st, nid, st.steps) {
                            has_body = true;
                            k += 1;
                            continue;
                        }
                        let w = (nid as usize) >> 6;
                        let bit = 1u64 << ((nid as usize) & 63);
                        if (!skip_empty && (skip_mask[w] & bit) != 0) || st.food_color[nid as usize] == 0 {
                            sealed = false;
                            break;
                        }
                        k += 1;
                    }
                let mut d = base;
                if sealed {
                    d += if has_body { 4 } else { 8 };
                }
                let mut score = d;
                let mut next_dist = 0;
                if has_next {
                    next_dist = self.nearest_color_dist_with_skip(st, id, next_want, id as i32, skip_mask);
                    score += next_dist / 2;
                }
                if score < best_score || (score == best_score && (d < best || (d == best && next_dist < best_next))) {
                    best_score = score;
                    best = d;
                    best_next = next_dist;
                }
                seen += 1;
                if seen >= WANTED_NEXT_LOOK && d > best {
                    break;
                }
                i += 1;
            }
        }
        if best_score < INF {
            best_score
        } else {
            best
        }
    }

    // Applies one move to a fully materialized state.
    // Return value:
    // - bool: legality
    // - i32: action kind
    //   0 = plain move
    //   1 = eat exact
    //   2 = eat mismatch
    //   3 = bite
    fn apply_move(&self, parent: &BeamState, mv: u8, max_b: u8, child: &mut BeamState) -> (bool, i32) {
        let nid16 = self.next_cell[parent.head as usize][mv as usize];
        let nid = nid16 as u8;
        if parent.steps - parent.last_visit[nid as usize] as i32 == 1 {
            return (false, 0);
        }

        // Start from the base state copy, then patch the changed fields in place.
        self.copy_state_for_move(parent, child);
        child.steps = parent.steps + 1;
        child.trace = 0;
        child.recov = 0;
        child.recov_end = 0;
        child.recov_mask = [0; 4];
        child.head = nid;
        let old_nid_prev_dir = self.get_from_dir(parent, nid);
        child.last_visit[nid as usize] = child.steps as i16;
        self.set_from_dir(child, nid, self.reverse_dir(mv));

        // Food case.
        let food_color = child.food_color[nid as usize];
        if food_color != 0 {
            child.plen += 1;
            child.tail = parent.tail;
            child.tail_prev = parent.tail_prev;
            self.food_clear_at(child, nid);
            child.food_center_sum -= self.center_dist[nid as usize] as i32;

            // Dirty suffix is capped. Since exact-eat is handled separately below,
            // reaching max_b means we cannot append one more mismatch.
            if child.blen >= max_b {
                return (false, 0);
            }
            // Clean exact eat: q grows, dirty suffix stays empty.
            if child.blen == 0 && food_color == self.target[child.q as usize] {
                child.q += 1;
                child.clear_head = parent.clear_head;
                return (true, 1);
            }
            // Otherwise the eaten color is appended to the dirty suffix.
            child.buf[child.blen as usize] = food_color;
            child.blen += 1;
            child.plen = child.q + child.blen;
            child.clear_head = self.shift_clear_head(parent, child.q);
            return (true, 2);
        }

        // Empty-cell move. Use last_visit to distinguish:
        // - moving into old free space / old tail path: plain move
        // - moving into still-occupied body: bite
        let delta = child.steps - parent.last_visit[nid as usize] as i32;
        if delta > child.plen as i32 - 2 {
            child.tail = parent.tail_prev;
            child.tail_prev = self.shift_tail_prev(parent);
            child.clear_head = self.shift_clear_head(parent, child.q);
            return (true, 0);
        }
        let keep = delta as u8 + 1;
        let bitten = child.plen - keep;
        // Large bites are rejected by heuristic.
        if bitten >= (child.plen / 5).max(5) + child.blen {
            return (false, 0);
        }
        let old_plen = child.plen;
        let old_q = child.q;
        let mut cid16 = self.next_cell[nid as usize][old_nid_prev_dir as usize];
        let mut p = keep;
        // Restore the cut-off suffix as foods by walking old prev links once.
        while p < old_plen {
            let c = if p < old_q {
                self.target[p as usize]
            } else {
                child.buf[(p - old_q) as usize]
            };
            let cid = cid16 as u8;
            self.food_set_at(child, cid, c);
            child.food_center_sum += self.center_dist[cid as usize] as i32;
            cid16 = self.next_cell[cid as usize][self.get_from_dir(child, cid) as usize];
            p += 1;
        }
        child.plen = keep;
        child.tail = nid;
        child.tail_prev = self.bite_tail_prev(parent, nid, delta);
        if keep >= old_q {
            child.blen = keep - old_q;
            child.plen = child.q + child.blen;
            child.clear_head = self.shift_clear_head(parent, child.q);
        } else {
            child.q = keep;
            child.blen = 0;
            child.plen = keep;
            child.clear_head = child.tail;
        }
        (true, 3)
    }

    // Small hash-combine helper for coarse state hashing.
    #[inline(always)]
    fn mix64(&self, h: u64, x: u64) -> u64 {
        h ^ (x
            .wrapping_add(0x9e3779b97f4a7c15)
            .wrapping_add(h << 6)
            .wrapping_add(h >> 2))
    }

    // Updates clear_head for a 1-step shifted virtual child without reconstructing the whole body.
    // clear_head is the cell at body index q-1, i.e. the end of the exact prefix.
    #[inline(always)]
    fn shift_clear_head(&self, parent: &BeamState, q: u8) -> u8 {
        let want_visit = (parent.steps - q as i32 + 2) as i16;
        let cur = parent.clear_head;
        let cnt = self.adj_cnt[cur as usize];
        let row = &self.adj[cur as usize];
        if cnt >= 1 {
            let nid = row[0];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        if cnt >= 2 {
            let nid = row[1];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        if cnt >= 3 {
            let nid = row[2];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        if cnt >= 4 {
            let nid = row[3];
            if parent.last_visit[nid as usize] == want_visit {
                return nid;
            }
        }
        cur
    }

    // Body-occupancy predicates for a 1-step shifted child.
    #[inline(always)]
    fn shift_body_contains_no_tail_at(
        &self,
        parent: &BeamState,
        head: u8,
        plen: usize,
        id: u8,
        steps_at: i32,
    ) -> bool {
        if id == head || plen <= 1 {
            return false;
        }
        let last = parent.last_visit[id as usize];
        last < steps_at as i16 && last >= (steps_at - plen as i32 + 2) as i16
    }

    #[inline(always)]
    fn shift_body_contains_with_tail_at(
        &self,
        parent: &BeamState,
        head: u8,
        plen: usize,
        id: u8,
        steps_at: i32,
    ) -> bool {
        if id == head || plen <= 1 {
            return false;
        }
        let last = parent.last_visit[id as usize];
        last < steps_at as i16 && last >= (steps_at - plen as i32 + 1) as i16
    }

    // Exact completion predicate on a materialized state.
    #[inline(always)]
    fn is_complete_state(&self, st: &BeamState) -> bool {
        st.q as usize == self.m && st.blen == 0
    }

    // Exact completion predicate on raw q/blen pair.
    #[inline(always)]
    fn is_complete_raw(&self, q: u8, blen: u8) -> bool {
        q as usize == self.m && blen == 0
    }

    // Coarse hash for a materialized state.
    // Intentionally uses only q/blen/head/neck/tail.
    #[inline(always)]
    fn state_hash(&self, st: &BeamState) -> u64 {
        let mut h = st.q as u64 | ((st.blen as u64) << 8);
        h = self.mix64(h, (1u64 << 8) | st.head as u64);
        if st.plen >= 2 {
            h = self.mix64(h, (2u64 << 8) | self.prev_cell(st, st.head) as u64);
        }
        self.mix64(h, 0x800000 | st.tail as u64)
    }

    // Coarse hash for a 1-step shifted virtual child.
    #[inline(always)]
    fn state_hash_shift(&self, parent: &BeamState, head: u8, q: u8, blen: u8) -> u64 {
        let plen = q + blen;
        let mut h = q as u64 | ((blen as u64) << 8);
        h = self.mix64(h, (1u64 << 8) | head as u64);
        if plen >= 2 {
            h = self.mix64(h, (2u64 << 8) | parent.head as u64);
        }
        let tail = if plen == 1 {
            head
        } else {
            if plen > parent.plen {
                parent.tail
            } else {
                parent.tail_prev
            }
        };
        self.mix64(h, 0x800000 | tail as u64)
    }

    // Stop score of a materialized state. This is used for "best-so-far" bookkeeping,
    // not as the main beam sorting score.
    #[inline(always)]
    fn stop_score(&self, st: &BeamState) -> i32 {
        if self.is_complete_state(st) {
            return st.steps;
        }
        st.steps + 10000 * (st.blen as i32 + 2 * (self.m as i32 - st.plen as i32))
    }

    // Same as stop_score, but when q/blen are known for a virtual child.
    #[inline(always)]
    fn stop_score_raw(&self, steps: i32, q: u8, blen: u8) -> i32 {
        if self.is_complete_raw(q, blen) {
            return steps;
        }
        let plen = q as i32 + blen as i32;
        steps + 10000 * (blen as i32 + 2 * (self.m as i32 - plen))
    }

    // Total ordering for "which state is currently the best fallback answer".
    // DFS for recoverable exact-chain estimation on a shifted virtual child.
    fn compute_recoverable_shift_dfs(
        &mut self,
        cur: u8,
        depth: usize,
        head: u8,
        plen: usize,
        q: usize,
        limit: usize,
        parent: &BeamState,
        removed_cell: i32,
        visit_epoch: u32,
        path_mask: &mut [u64; 4],
        best: &mut RecoverInfo,
    ) {
        if depth > best.len as usize {
            best.len = depth as u8;
            best.end = cur;
            best.mask = *path_mask;
            if depth >= limit || q + depth >= self.m {
                return;
            }
        }
        if depth >= limit || q + depth >= self.m {
            return;
        }
        let need = self.target[q + depth];
        let cnt = self.adj_cnt[cur as usize] as usize;
        let mut i = 0usize;
        while i < cnt {
            let nid = self.adj[cur as usize][i];
            let id = nid as usize;
            if !self.shift_body_contains_with_tail_at(parent, head, plen, nid, parent.steps + 1)
                && self.shift_visit_marks[id] != visit_epoch
                && nid as i32 != removed_cell
                && parent.food_color[id] == need
            {
                self.shift_visit_marks[id] = visit_epoch;
                Solver::mask_set(path_mask, nid);
                self.compute_recoverable_shift_dfs(
                    nid,
                    depth + 1,
                    head,
                    plen,
                    q,
                    limit,
                    parent,
                    removed_cell,
                    visit_epoch,
                    path_mask,
                    best,
                );
                Solver::mask_clear(path_mask, nid);
                self.shift_visit_marks[id] = 0;
                if best.len as usize >= limit || q + best.len as usize >= self.m {
                    return;
                }
            }
            i += 1;
        }
    }

    // Shift-side recoverable estimate.
    // Only meaningful when the virtual child is clean (blen==0).
    fn compute_recoverable_shift(
        &mut self,
        parent: &BeamState,
        head: u8,
        q: u8,
        blen: u8,
        removed_cell: i32,
        limit: usize,
    ) -> RecoverInfo {
        if blen != 0 || q as usize >= self.m || limit == 0 {
            return RecoverInfo {
                len: 0,
                end: head,
                mask: [0; 4],
            };
        }
        let plen = q as usize + blen as usize;
        let need0 = self.target[q as usize];
        let cnt = self.adj_cnt[head as usize] as usize;
        let mut i = 0usize;
        while i < cnt {
            let nid = self.adj[head as usize][i];
            if !self.shift_body_contains_with_tail_at(parent, head, plen, nid, parent.steps + 1)
                && nid as i32 != removed_cell
                && parent.food_color[nid as usize] == need0
            {
                let mut best = RecoverInfo {
                    len: 0,
                    end: head,
                    mask: [0; 4],
                };
                let mut path_mask = [0u64; 4];
                let visit_epoch = self.next_mark_epoch();
                self.shift_visit_marks[head as usize] = visit_epoch;
                self.compute_recoverable_shift_dfs(
                    head,
                    0,
                    head,
                    plen,
                    q as usize,
                    limit,
                    parent,
                    removed_cell,
                    visit_epoch,
                    &mut path_mask,
                    &mut best,
                );
                self.shift_visit_marks[head as usize] = 0;
                return best;
            }
            i += 1;
        }
        RecoverInfo {
            len: 0,
            end: head,
            mask: [0; 4],
        }
    }

    // Main evaluation on a materialized state.
    // This is the scalar "cost-to-go" estimate used by the beam.
    fn finish_estimate(&mut self, st: &BeamState) -> f32 {
        if self.is_complete_state(st) {
            return st.steps as f32;
        }
        let eff = st.q as i32 + (st.recov as usize).min(RECOVER_LIMIT) as i32;
        let mut eta = st.steps as f32;
        let empty_mask = [0u64; 4];
        eta += W_MISSING_EFF * (self.m as i32 - eff) as f32;
        eta += W_RECOV * (eff - st.q as i32) as f32;
        eta += st.food_center_sum as f32 * W_CENTER;
        if st.blen == 0 {
            let d = self.nearest_wanted_dist(st) as f32;
            if d > 1.0_f32 {
                eta += W_CLEAN_WANTED * (d - 1.0_f32);
            }
        } else {
            eta += W_DIRTY_BASE;
            eta += W_DIRTY_CLEAR
                * self.cell_dist[st.head as usize][st.clear_head as usize] as f32;
            let d = self.nearest_wanted_dist_shift_fast(st, st.clear_head, st.q as usize, st.q as usize, -1, &empty_mask) as f32;
            if d > 1.0_f32 {
                eta += W_DIRTY_WANTED * (d - 1.0_f32);
            }
        }
        eta
    }

    // Shift-side version of finish_estimate for non-bite children.
    fn finish_estimate_shift(
        &mut self,
        parent: &BeamState,
        head: u8,
        clear_head: u8,
        removed_cell: i32,
        steps: i32,
        q: u8,
        blen: u8,
        rec: &RecoverInfo,
    ) -> f32 {
        if self.is_complete_raw(q, blen) {
            return steps as f32;
        }
        let plen = q as i32 + blen as i32;
        let eff = q as usize + rec.len as usize;
        let mut eta = steps as f32;
        let empty_mask = [0u64; 4];
        eta += W_MISSING_EFF * (self.m as i32 - eff as i32) as f32;
        eta += W_RECOV * (eff as i32 - q as i32) as f32;
        eta += (parent.food_center_sum
            - if removed_cell >= 0 {
                self.center_dist[removed_cell as usize] as i32
            } else {
                0
            }) as f32
            * W_CENTER;
        if blen == 0 {
            let d = self.nearest_wanted_dist_shift_fast(
                parent,
                if rec.len != 0 { rec.end } else { head },
                q as usize + rec.len as usize,
                q as usize + rec.len as usize,
                removed_cell,
                &rec.mask,
            ) as f32;
            if d > 1.0_f32 {
                eta += W_CLEAN_WANTED * (d - 1.0_f32);
            }
        } else {
            eta += W_DIRTY_BASE;
            eta += W_DIRTY_CLEAR
                * self.cell_dist[head as usize][clear_head as usize] as f32;
            let d = self.nearest_wanted_dist_shift_fast(parent, clear_head, q as usize, q as usize, removed_cell, &empty_mask) as f32;
            if d > 1.0_f32 {
                eta += W_DIRTY_WANTED * (d - 1.0_f32);
            }
        }
        let _ = plen;
        eta
    }

    // Converts a materialized child into beam score.
    #[inline(always)]
    fn action_score(&mut self, child: &BeamState) -> f32 {
        -self.finish_estimate(child)
    }

    // Tiny random tie-break so exactly-equal scores do not collapse deterministically.
    #[inline(always)]
    fn jittered_score(&mut self, base: f32) -> f32 {
        if SCORE_JITTER_SCALE == 0 || W_JITTER == 0.0 {
            return base;
        }
        base + W_JITTER * (self.beam_rand.next() & (SCORE_JITTER_SCALE - 1)) as f32
            / SCORE_JITTER_SCALE as f32
    }

    // Heuristic limit for how much suffix may be bitten off at once.
    #[inline(always)]
    fn large_bite_limit(&self, plen: u8, blen: u8) -> u8 {
        plen.max(5) + blen
    }

    // Shift-side wanted-distance heuristic.
    // This is one of the hottest functions in the solver:
    // it evaluates the best current wanted food from a virtual child,
    // plus a light lookahead toward the next wanted color.
    fn nearest_wanted_dist_shift_fast(
        &mut self,
        parent: &BeamState,
        head: u8,
        plen: usize,
        q: usize,
        removed_cell: i32,
        skip_mask: &[u64; 4],
    ) -> i32 {
        if q >= self.m {
            return 0;
        }
        let want = self.target[q];
        let has_next = q + 1 < self.m;
        let next_want = if has_next { self.target[q + 1] } else { 0 };
        let food = &parent.food_color;
        let step1 = parent.steps + 1;
        let skip_empty = (skip_mask[0] | skip_mask[1] | skip_mask[2] | skip_mask[3]) == 0;
        let mut best = (self.n * self.n) as i32;
        let mut best_score = INF;
        let mut best_next = (self.n * self.n) as i32;
        let mut seen = 0usize;
        if parent.food_count[want as usize] <= SPARSE_FOOD_THRESHOLD {
            let mut w = 0usize;
            while w < 4 {
                let mut bits = parent.food_bb[want as usize][w];
                while bits != 0 {
                    let tz = bits.trailing_zeros() as usize;
                    bits &= bits - 1;
                    let id = ((w << 6) + tz) as u8;
                    let base = self.cell_dist[head as usize][id as usize] as i32;
                    if base >= best
                        || id as i32 == removed_cell
                        || (!skip_empty && ((skip_mask[w] >> tz) & 1) != 0)
                        || food[id as usize] != want
                    {
                        continue;
                    }
                    let cnt = self.adj_cnt[id as usize] as usize;
                    let mut sealed = true;
                    let mut has_body = false;
                    let mut k = 0usize;
                    while k < cnt {
                        let nid = self.adj[id as usize][k];
                        if self.shift_body_contains_no_tail_at(parent, head, plen, nid, step1) {
                            has_body = true;
                            k += 1;
                            continue;
                        }
                        let w = (nid as usize) >> 6;
                        let bit = 1u64 << ((nid as usize) & 63);
                        if nid as i32 == removed_cell
                            || (!skip_empty && (skip_mask[w] & bit) != 0)
                            || food[nid as usize] == 0
                        {
                            sealed = false;
                            break;
                        }
                        k += 1;
                    }
                    let mut d = base;
                    if sealed {
                        d += if has_body { 4 } else { 8 };
                    }
                    let mut score = d;
                    let mut next_dist = 0;
                    if has_next {
                        if parent.food_count[next_want as usize] <= SPARSE_FOOD_THRESHOLD {
                            next_dist = (self.n * self.n) as i32;
                            let mut w2 = 0usize;
                            while w2 < 4 {
                                let mut bits2 = parent.food_bb[next_want as usize][w2];
                                while bits2 != 0 {
                                    let tz2 = bits2.trailing_zeros() as usize;
                                    bits2 &= bits2 - 1;
                                    let id2 = ((w2 << 6) + tz2) as u8;
                                    if id2 as i32 != removed_cell
                                        && id2 != id
                                        && (skip_empty || ((skip_mask[w2] >> tz2) & 1) == 0)
                                        && food[id2 as usize] == next_want
                                    {
                                        let base2 = self.cell_dist[id as usize][id2 as usize] as i32;
                                        if base2 < next_dist {
                                            next_dist = base2;
                                        }
                                    }
                                }
                                w2 += 1;
                            }
                        } else {
                            let order2 = &self.near_cell_order[id as usize];
                            let drow2 = &self.cell_dist[id as usize];
                            let mut j = 0usize;
                            while j < self.near_cell_count {
                                let id2 = order2[j];
                                let base2 = drow2[id2 as usize] as i32;
                                if base2 >= next_dist && next_dist != 0 {
                                    break;
                                }
                                let w2 = (id2 as usize) >> 6;
                                let bit2 = 1u64 << ((id2 as usize) & 63);
                                if id2 as i32 != removed_cell
                                    && id2 != id
                                    && (skip_empty || (skip_mask[w2] & bit2) == 0)
                                    && food[id2 as usize] == next_want
                                {
                                    next_dist = base2;
                                    break;
                                }
                                j += 1;
                            }
                            if next_dist == 0 {
                                next_dist = (self.n * self.n) as i32;
                            }
                        }
                        score += next_dist / 2;
                    }
                    if score < best_score || (score == best_score && (d < best || (d == best && next_dist < best_next))) {
                        best_score = score;
                        best = d;
                        best_next = next_dist;
                    }
                    seen += 1;
                }
                w += 1;
            }
        } else {
            let order = &self.near_cell_order[head as usize];
            let drow = &self.cell_dist[head as usize];
            let mut i = 0usize;
            while i < self.near_cell_count {
                let id = order[i];
                let base = drow[id as usize] as i32;
                if base >= best {
                    break;
                }
                let w = (id as usize) >> 6;
                let bit = 1u64 << ((id as usize) & 63);
                if id as i32 == removed_cell
                    || (!skip_empty && (skip_mask[w] & bit) != 0)
                    || food[id as usize] != want
                {
                    i += 1;
                    continue;
                }
                let cnt = self.adj_cnt[id as usize] as usize;
                let mut sealed = true;
                let mut has_body = false;
                let mut k = 0usize;
                while k < cnt {
                    let nid = self.adj[id as usize][k];
                    if self.shift_body_contains_no_tail_at(parent, head, plen, nid, step1) {
                        has_body = true;
                        k += 1;
                        continue;
                    }
                    let w = (nid as usize) >> 6;
                    let bit = 1u64 << ((nid as usize) & 63);
                    if nid as i32 == removed_cell
                        || (!skip_empty && (skip_mask[w] & bit) != 0)
                        || food[nid as usize] == 0
                    {
                        sealed = false;
                        break;
                    }
                    k += 1;
                }
                let mut d = base;
                if sealed {
                    d += if has_body { 4 } else { 8 };
                }
                let mut score = d;
                let mut next_dist = 0;
                if has_next {
                    let order2 = &self.near_cell_order[id as usize];
                    let drow2 = &self.cell_dist[id as usize];
                    let mut j = 0usize;
                    while j < self.near_cell_count {
                        let id2 = order2[j];
                        let base2 = drow2[id2 as usize] as i32;
                        if base2 >= next_dist && next_dist != 0 {
                            break;
                        }
                        let w2 = (id2 as usize) >> 6;
                        let bit2 = 1u64 << ((id2 as usize) & 63);
                        if id2 as i32 != removed_cell
                            && id2 != id
                            && (skip_empty || (skip_mask[w2] & bit2) == 0)
                            && food[id2 as usize] == next_want
                        {
                            next_dist = base2;
                            break;
                        }
                        j += 1;
                    }
                    if next_dist == 0 {
                        next_dist = (self.n * self.n) as i32;
                    }
                    score += next_dist / 2;
                }
                if score < best_score || (score == best_score && (d < best || (d == best && next_dist < best_next))) {
                    best_score = score;
                    best = d;
                    best_next = next_dist;
                }
                seen += 1;
                if seen >= WANTED_NEXT_LOOK && d > best {
                    break;
                }
                i += 1;
            }
        }
        if best_score < INF { best_score } else { best }
    }

    // Fast 1-step simulation.
    // Returns:
    // - 0: illegal / reject now
    // - 1: fully summarized child written into out
    // - 2: bite child; legality is known, but caller must materialize to score it
    fn fast_move_summary(
        &mut self,
        parent: &BeamState,
        mv: u8,
        max_b: u8,
        out: &mut FastSummary,
    ) -> i32 {
        let nid = self.next_cell[parent.head as usize][mv as usize] as u8;
        let steps = parent.steps + 1;
        let delta = steps - parent.last_visit[nid as usize] as i32;
        if delta == 2 {
            return 0;
        }
        let food = parent.food_color[nid as usize];

        if food == 0 {
            if delta <= parent.plen as i32 - 2 {
                let bitten = parent.plen - delta as u8 - 1;
                if bitten >= self.large_bite_limit(parent.plen, parent.blen) {
                    return 0;
                }
                return 2;
            }
            let q = parent.q;
            let blen = parent.blen;
            let rec = if blen == 0 {
                self.compute_recoverable_shift(parent, nid, q, blen, -1, RECOVER_LIMIT)
            } else {
                RecoverInfo {
                    len: 0,
                    end: nid,
                    mask: [0; 4],
                }
            };
            out.q = q;
            out.blen = blen;
            out.steps = steps;
            out.recov = rec.len;
            out.recov_end = rec.end;
            out.recov_mask = rec.mask;
            out.stop = self.stop_score_raw(steps, q, blen);
            out.hash = self.state_hash_shift(parent, nid, q, blen);
            let clear_head = self.shift_clear_head(parent, q);
            let eta = self.finish_estimate_shift(parent, nid, clear_head, -1, steps, q, blen, &rec);
            out.score = self.jittered_score(-eta);
            return 1;
        }

        if parent.blen >= max_b {
            return 0;
        }

        let q;
        let blen;
        if parent.blen == 0 && food == self.target[parent.q as usize] {
            q = parent.q + 1;
            blen = 0;
        } else {
            q = parent.q;
            blen = parent.blen + 1;
        }
        let removed_cell = nid as i32;
        let rec = if blen == 0 {
            self.compute_recoverable_shift(parent, nid, q, blen, removed_cell, RECOVER_LIMIT)
        } else {
            RecoverInfo {
                len: 0,
                end: nid,
                mask: [0; 4],
            }
        };
        out.q = q;
        out.blen = blen;
        out.steps = steps;
        out.recov = rec.len;
        out.recov_end = rec.end;
        out.recov_mask = rec.mask;
        out.stop = self.stop_score_raw(steps, q, blen);
        out.hash = self.state_hash_shift(parent, nid, q, blen);
        let clear_head = if q > parent.q {
            parent.clear_head
        } else {
            self.shift_clear_head(parent, q)
        };
        let eta = self.finish_estimate_shift(parent, nid, clear_head, removed_cell, steps, q, blen, &rec);
        out.score = self.jittered_score(-eta);
        1
    }

    fn compute_food_center_sum(&self, st: &BeamState) -> i32 {
        let mut s = 0i32;
        let mut id = 0usize;
        while id < MAX_CELLS {
            if st.food_color[id] != 0 {
                s += self.center_dist[id] as i32;
            }
            id += 1;
        }
        s
    }

    fn run_beam(
        &mut self,
        mut current: Vec<BeamState>,
        phase: Phase,
        deadline: Instant,
        best_global: &mut BeamState,
        best_trace: &mut i32,
        traces: &mut Vec<Trace>,
        scratch_buf: &mut BeamScratch,
    ) -> (Vec<BeamState>, BeamState, usize) {
        let width = phase.width as usize;
        scratch_buf.ensure_width(width);
        let best_local_exact = if self.is_complete_state(best_global) {
            best_global.steps
        } else {
            INF
        };
        if current.len() > width {
            current.truncate(width);
        }
        let mut current_len = current.len();
        current.resize(width, BeamState::default());

        let mut max_q_seen = 0u8;
        let mut max_q_step = 0usize;
        let mut i = 0usize;
        while i < current_len {
            let st = &current[i];
            if st.q > max_q_seen {
                max_q_seen = st.q;
                max_q_step = 0;
            }
            i += 1;
        }
        let cands = &mut scratch_buf.cands;
        let order = &mut scratch_buf.order;
        let next = &mut scratch_buf.next;
        let turn_hash_keys = &mut scratch_buf.turn_hash_keys;
        let turn_hash_stamp = &mut scratch_buf.turn_hash_stamp;
        let mut tmp = BeamState::default();
        let mut fast = FastSummary::default();
        let max_b = phase.max_b;

        let mut step = 0usize;
        loop {
            if (step & 7) == 0 && Instant::now() >= deadline {
                break;
            }
            if step.saturating_sub(max_q_step) >= (20*max_q_seen as usize).max(2 * self.n) {
                break;
            }
            let current_steps = current[0].steps;
            if current_steps + 1 >= best_local_exact {
                break;
            }
            cands.clear();

            let mut si = 0usize;
            while si < current_len {
                let st = &current[si];
                let head_idx = st.head as usize;
                let cnt = self.move_cnt[head_idx] as usize;
                let move_dir = self.move_dir[head_idx];
                let parent_idx = si as u16;
                let mut mi = 0usize;
                while mi < cnt {
                    let mv = move_dir[mi];
                    let mode = self.fast_move_summary(&st, mv, max_b, &mut fast);
                    if mode == 0 {
                        mi += 1;
                        continue;
                    }
                    if mode == 1 {
                        if self.is_complete_raw(fast.q, fast.blen) {
                            let (ok, _) = self.apply_move(&st, mv, max_b, &mut tmp);
                            if ok {
                                let tr_idx = traces.len() as i32;
                                traces.push(Trace {
                                    parent: st.trace,
                                    mv,
                                });
                                tmp.trace = tr_idx;
                                *best_global = tmp;
                                *best_trace = tmp.trace;
                                current.truncate(current_len);
                                return (current, tmp, step);
                            }
                        }
                        if fast.q > max_q_seen {
                            max_q_seen = fast.q;
                            max_q_step = step + 1;
                        }
                        cands.push(Candidate {
                            score: fast.score,
                            hash: fast.hash,
                            recov: fast.recov,
                            recov_end: fast.recov_end,
                            recov_mask: fast.recov_mask,
                            parent_idx,
                            mv,
                        });
                        mi += 1;
                        continue;
                    }

                    let (ok, _) = self.apply_move(&st, mv, max_b, &mut tmp);
                    if !ok {
                        mi += 1;
                        continue;
                    }
                    self.refresh_derived(&mut tmp);
                    let hash = self.state_hash(&tmp);
                    let base_score = self.action_score(&tmp);
                    let score = self.jittered_score(base_score);
                    cands.push(Candidate {
                        score,
                        hash,
                        recov: tmp.recov,
                        recov_end: tmp.recov_end,
                        recov_mask: tmp.recov_mask,
                        parent_idx,
                        mv,
                    });
                    mi += 1;
                }
                si += 1;
            }

            if cands.is_empty() {
                break;
            }
            if Instant::now() >= deadline {
                break;
            }

            order.clear();
            order.resize(cands.len(), 0);
            let mut oi = 0usize;
            while oi < order.len() {
                order[oi] = oi;
                oi += 1;
            }
            order.sort_unstable_by(|&i, &j| cands[j].score.total_cmp(&cands[i].score));

            let mut next_len = 0usize;
            scratch_buf.turn_hash_epoch = scratch_buf.turn_hash_epoch.wrapping_add(1);
            if scratch_buf.turn_hash_epoch == 0 {
                turn_hash_stamp.fill(0);
                scratch_buf.turn_hash_epoch = 1;
            }
            let mut oi2 = 0usize;
            while oi2 < order.len() {
                let idx = order[oi2];
                let cand = &cands[idx];
                let parent = &current[cand.parent_idx as usize];
                if Solver::turn_hash_insert(
                    turn_hash_keys,
                    turn_hash_stamp,
                    scratch_buf.turn_hash_epoch,
                    cand.hash,
                ) {
                    let child = &mut next[next_len];
                    let (ok, _) = self.apply_move(parent, cand.mv, max_b, child);
                    if ok {
                        child.recov = cand.recov;
                        child.recov_end = cand.recov_end;
                        child.recov_mask = cand.recov_mask;
                        let tr_idx = traces.len() as i32;
                        traces.push(Trace {
                            parent: parent.trace,
                            mv: cand.mv,
                        });
                        child.trace = tr_idx;
                        next_len += 1;
                        if next_len >= width {
                            break;
                        }
                    }
                }
                oi2 += 1;
            }

            if next_len == 0 {
                break;
            }
            std::mem::swap(&mut current, next);
            current_len = next_len;
            step += 1;
        }
        current.truncate(current_len);
        let run_state = current[0];
        (current, run_state, step)
    }
}

fn reconstruct_path(traces: &[Trace], mut idx: i32) -> Vec<u8> {
    if idx <= 0 {
        return Vec::new();
    }
    let mut path = Vec::with_capacity(256);
    while idx > 0 {
        let tr = traces[idx as usize];
        path.push(tr.mv);
        idx = tr.parent;
    }
    path.reverse();
    path
}

fn build_solver_and_init(
    n: usize,
    m: usize,
    c: usize,
    target: &[u8; MAX_M],
    food_color: &[u8; MAX_CELLS],
) -> (Solver, BeamState) {
    let mut solver = Solver::new();
    solver.n = n;
    solver.m = m;
    solver.c = c;
    solver.target[..solver.m].copy_from_slice(&target[..solver.m]);
    solver.init_precompute();

    let mut init = BeamState::default();
    init.food_color = *food_color;
    let mut id = 0usize;
    while id < MAX_CELLS {
        let color = food_color[id] as usize;
        if color != 0 {
            init.food_bb[color][id >> 6] |= 1u64 << (id & 63);
            init.food_count[color] += 1;
        }
        id += 1;
    }
    init.food_center_sum = solver.compute_food_center_sum(&init);
    init.q = 5;
    init.plen = 5;
    let body = [
        solver.cell_id(4, 0),
        solver.cell_id(3, 0),
        solver.cell_id(2, 0),
        solver.cell_id(1, 0),
        solver.cell_id(0, 0),
    ];
    init.head = body[0];
    init.tail = body[4];
    init.tail_prev = body[3];
    init.clear_head = init.tail;
    let mut i = 0usize;
    while i < init.plen as usize {
        init.last_visit[body[i] as usize] = -(i as i16);
        if i + 1 < init.plen as usize {
            let cur = body[i];
            let nxt = body[i + 1];
            let dir = if solver.next_cell[cur as usize][DIR_U as usize] == nxt as u16 {
                DIR_U
            } else if solver.next_cell[cur as usize][DIR_L as usize] == nxt as u16 {
                DIR_L
            } else if solver.next_cell[cur as usize][DIR_D as usize] == nxt as u16 {
                DIR_D
            } else {
                DIR_R
            };
            solver.set_from_dir(&mut init, cur, dir);
        }
        i += 1;
    }
    solver.refresh_derived(&mut init);
    (solver, init)
}

fn official_score(solver: &Solver, st: &BeamState) -> i64 {
    let q = st.q as usize;
    let blen = st.blen as usize;
    let mut e = 0i64;
    let mut i = 0usize;
    while i < blen {
        if st.buf[i] != solver.target[q + i] {
            e += 1;
        }
        i += 1;
    }
    st.steps as i64 + 10000 * (e + 2 * (solver.m as i64 - st.plen as i64))
}

fn better_stop(solver: &Solver, a: &BeamState, b: &BeamState) -> bool {
    let sa = solver.stop_score(a);
    let sb = solver.stop_score(b);
    if sa != sb {
        return sa < sb;
    }
    if a.q != b.q {
        return a.q > b.q;
    }
    if a.blen != b.blen {
        return a.blen < b.blen;
    }
    a.steps < b.steps
}

fn best_state_in_frontier(solver: &Solver, states: &[BeamState]) -> BeamState {
    let mut best = states[0];
    let mut i = 1usize;
    while i < states.len() {
        if better_stop(solver, &states[i], &best) {
            best = states[i];
        }
        i += 1;
    }
    best
}

fn parse_problem_from_bytes(buf: Vec<u8>) -> (usize, usize, usize, [u8; MAX_M], [u8; MAX_CELLS]) {
    let mut sc = Scanner::from_bytes(buf);
    let n = sc.next_i32() as usize;
    let m = sc.next_i32() as usize;
    let c = sc.next_i32() as usize;
    let mut target = [0u8; MAX_M];
    let mut i = 0usize;
    while i < m {
        target[i] = sc.next_i32() as u8;
        i += 1;
    }
    let mut food_color = [0u8; MAX_CELLS];
    let mut r = 0usize;
    while r < n {
        let mut col = 0usize;
        while col < n {
            let v = sc.next_i32() as usize;
            if v != 0 {
                food_color[r * MAX_N + col] = v as u8;
            }
            col += 1;
        }
        r += 1;
    }
    (n, m, c, target, food_color)
}

fn solve_case(
    n: usize,
    m: usize,
    c: usize,
    target: &[u8; MAX_M],
    food_color: &[u8; MAX_CELLS],
) -> (Vec<u8>, i64) {
    let (mut solver, init) = build_solver_and_init(n, m, c, target, food_color);
    let mut beam_scratch = BeamScratch::new();

    let mut traces = Vec::with_capacity(1 << 20);
    traces.push(Trace { parent: -1, mv: 0 });

    let mut best = init;
    let mut best_trace = 0i32;
    let mut best_fallback = init;
    let start = Instant::now();
    let deadline = start + Duration::from_millis(DEADLINE_MS);
    let deadline_extra = start + Duration::from_millis(DEADLINE_EXTRA_MS);
    let base_beam_seed = solver.state_hash(&init) ^ 0xd1b54a32d192ed03;
    let mut iter = 0u64;
    let mut first_run_ms = 0u64;
    let mut current_width = FIRST_BEAM_WIDTH;
    let mut last_frontier = vec![init];
    let mut last_run_width = current_width;
    while Instant::now() < deadline {
        iter += 1;
        solver.beam_rand = XorShift64 {
            x: (base_beam_seed ^ iter.wrapping_mul(0x94d049bb133111eb)) | 1,
        };
        let run_width = current_width;
        let phase = Phase {
            max_b: MAX_B as u8,
            width: run_width,
        };
        let run_start = Instant::now();
        let (frontier, _run_state, _depth) =
            solver.run_beam(
                vec![init],
                phase,
                deadline,
                &mut best,
                &mut best_trace,
                &mut traces,
                &mut beam_scratch,
            );
        let elapsed_ms = run_start.elapsed().as_millis() as u64;
        last_run_width = run_width;
        if iter == 1 {
            first_run_ms = elapsed_ms.max(1);
        }
        if first_run_ms != 0 {
            let now = Instant::now();
            if now < deadline {
                let a_ms = deadline.duration_since(now).as_millis() as u64;
                let est_width = (1024u64 * (a_ms / 5)).max(1) / first_run_ms;
                let blended_width = (run_width as u64 + est_width * 4 + 2) / 5;
                current_width = blended_width.clamp(1, u16::MAX as u64) as u16;
            }
        }
        if !frontier.is_empty() {
            let cand = best_state_in_frontier(&solver, &frontier);
            if !solver.is_complete_state(&best) && better_stop(&solver, &cand, &best_fallback) {
                best_fallback = cand;
            }
            last_frontier = frontier;
        }
        if BEAM_RUN_LIMIT > 0 && iter as i32 >= BEAM_RUN_LIMIT {
            break;
        }
    }

    if Instant::now() < deadline_extra {
        if solver.is_complete_state(&best) {
            while Instant::now() < deadline_extra {
                iter += 1;
                solver.beam_rand = XorShift64 {
                    x: (base_beam_seed ^ iter.wrapping_mul(0x94d049bb133111eb)) | 1,
                };
                let phase = Phase {
                    max_b: MAX_B as u8,
                    width: last_run_width,
                };
                let (frontier, _, depth) = solver.run_beam(
                    vec![init],
                    phase,
                    deadline_extra,
                    &mut best,
                    &mut best_trace,
                    &mut traces,
                    &mut beam_scratch,
                );
                if depth == 0 || frontier.is_empty() {
                    break;
                }
            }
        } else if !last_frontier.is_empty() {
            while !last_frontier.is_empty() && Instant::now() < deadline_extra {
                iter += 1;
                solver.beam_rand = XorShift64 {
                    x: (base_beam_seed ^ iter.wrapping_mul(0x94d049bb133111eb)) | 1,
                };
                let phase = Phase {
                    max_b: MAX_B as u8,
                    width: (last_run_width / 5).max(1),
                };
                let (frontier, _, depth) = solver.run_beam(
                    last_frontier,
                    phase,
                    deadline_extra,
                    &mut best,
                    &mut best_trace,
                    &mut traces,
                    &mut beam_scratch,
                );
                if depth == 0 || frontier.is_empty() {
                    break;
                }
                let cand = best_state_in_frontier(&solver, &frontier);
                if better_stop(&solver, &cand, &best_fallback) {
                    best_fallback = cand;
                }
                last_frontier = frontier;
            }
        }
    }

    if !solver.is_complete_state(&best) {
        best = best_fallback;
        best_trace = best.trace;
    }

    let score = official_score(&solver, &best);
    let ans = reconstruct_path(&traces, best_trace);
    (ans, score)
}

fn main() {
    let mut buf = Vec::new();
    io::stdin().read_to_end(&mut buf).unwrap();
    let (n, m, c, target, food_color) = parse_problem_from_bytes(buf);
    let (ans, _) = solve_case(n, m, c, &target, &food_color);
    let mut out = Vec::with_capacity(ans.len() * 2);
    let mut i = 0usize;
    while i < ans.len() {
        out.push(match ans[i] {
            DIR_U => b'U',
            DIR_L => b'L',
            DIR_D => b'D',
            _ => b'R',
        });
        out.push(b'\n');
        i += 1;
    }
    io::stdout().write_all(&out).unwrap();
}
