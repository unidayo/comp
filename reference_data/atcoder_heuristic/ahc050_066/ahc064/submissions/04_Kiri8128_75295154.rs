use std::collections::HashMap;
use std::io::{self, Read};
use std::time::{Duration, Instant};

const R: usize = 10;
const CARS_PER_LINE: usize = 10;
const N: usize = R * CARS_PER_LINE;
const MAIN_CAP: usize = 15;
const SIDING_CAP: usize = 20;
const TMAX: usize = 4000;

// 提出時は false 推奨
const DEBUG: bool = false;
const PROFILE: bool = false;

// ビーム幅の設定
const BEAM_WIDTH: usize = 24;

// 高速化優先なら greedy を切る。
// 両方比較したい場合は true に戻す。
const USE_GREEDY_SOLVER: bool = false;
const USE_BEAM_SOLVER: bool = true;

macro_rules! timed {
    ($prof:expr, $name:expr, $body:block) => {{
        if PROFILE {
            let st = Instant::now();
            let ret = $body;
            $prof.add_time($name, st.elapsed());
            ret
        } else {
            $body
        }
    }};
}

// -----------------------------------------------------
// Simple Profiler
// -----------------------------------------------------
#[derive(Clone, Copy, Default)]
struct ProfItem {
    calls: u64,
    total: Duration,
}

struct Profiler {
    title: &'static str,
    start: Instant,
    items: HashMap<&'static str, ProfItem>,
    counters: HashMap<&'static str, u64>,
}

impl Profiler {
    fn new(title: &'static str) -> Self {
        Self {
            title,
            start: Instant::now(),
            items: HashMap::new(),
            counters: HashMap::new(),
        }
    }

    #[inline]
    fn add_time(&mut self, name: &'static str, elapsed: Duration) {
        if !PROFILE {
            return;
        }

        let e = self.items.entry(name).or_default();
        e.calls += 1;
        e.total += elapsed;
    }

    #[inline]
    fn inc(&mut self, name: &'static str, v: u64) {
        if !PROFILE {
            return;
        }

        *self.counters.entry(name).or_insert(0) += v;
    }

    fn report(&self) {
        if !PROFILE {
            return;
        }

        let total = self.start.elapsed();

        eprintln!("=== Profile: {} ===", self.title);
        eprintln!(
            "{:<36} {:>10.3} ms",
            "total",
            total.as_secs_f64() * 1000.0
        );

        let mut items: Vec<_> = self.items.iter().collect();
        items.sort_by(|a, b| b.1.total.cmp(&a.1.total));

        eprintln!("--- time ---");
        eprintln!(
            "{:<36} {:>10} {:>12} {:>12}",
            "name", "calls", "total_ms", "avg_us"
        );

        for (name, item) in items {
            let total_ms = item.total.as_secs_f64() * 1000.0;
            let avg_us = if item.calls == 0 {
                0.0
            } else {
                item.total.as_secs_f64() * 1_000_000.0 / item.calls as f64
            };

            eprintln!(
                "{:<36} {:>10} {:>12.3} {:>12.3}",
                name, item.calls, total_ms, avg_us
            );
        }

        let mut counters: Vec<_> = self.counters.iter().collect();
        counters.sort_by_key(|(name, _)| **name);

        eprintln!("--- counters ---");
        for (name, value) in counters {
            eprintln!("{:<36} {:>12}", name, value);
        }
    }
}

// -----------------------------------------------------
// Zobrist Hashing
// -----------------------------------------------------
struct XorShift(u64);

impl XorShift {
    fn new(seed: u64) -> Self {
        Self(seed)
    }

    fn next(&mut self) -> u64 {
        self.0 ^= self.0 << 13;
        self.0 ^= self.0 >> 7;
        self.0 ^= self.0 << 17;
        self.0
    }
}

fn compute_zobrist() -> [u64; N] {
    let mut rng = XorShift::new(123456789);
    let mut z = [0; N];

    for i in 0..N {
        z[i] = rng.next();
    }

    z
}

// -----------------------------------------------------
// Data
// -----------------------------------------------------
#[derive(Clone, Copy, Debug, Default)]
struct Move {
    typ: u8,
    i: usize,
    j: usize,
    k: usize,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Loc {
    Main(usize, usize),
    Siding(usize, usize),
}

#[derive(Clone, Copy)]
struct Turn {
    len: usize,
    mv: [Move; R],
}

impl Default for Turn {
    fn default() -> Self {
        Self {
            len: 0,
            mv: [Move::default(); R],
        }
    }
}

#[inline]
fn enc_main(i: usize, p: usize) -> u16 {
    ((i as u16) << 5) | (p as u16)
}

#[inline]
fn enc_siding(j: usize, p: usize) -> u16 {
    (1u16 << 9) | ((j as u16) << 5) | (p as u16)
}

#[inline]
fn decode_loc(x: u16) -> Loc {
    let side = x >> 9;
    let line = ((x >> 5) & 15) as usize;
    let pos = (x & 31) as usize;

    if side == 0 {
        Loc::Main(line, pos)
    } else {
        Loc::Siding(line, pos)
    }
}

#[derive(Clone)]
struct State {
    main: [[u8; MAIN_CAP]; R],
    main_len: [usize; R],
    siding: [[u8; SIDING_CAP]; R],
    siding_len: [usize; R],
    loc: [u16; N],
}

impl State {
    fn new(src: Vec<Vec<usize>>) -> Self {
        let mut st = Self {
            main: [[0; MAIN_CAP]; R],
            main_len: [0; R],
            siding: [[0; SIDING_CAP]; R],
            siding_len: [0; R],
            loc: [0; N],
        };

        for i in 0..R {
            st.main_len[i] = src[i].len();

            for p in 0..src[i].len() {
                st.main[i][p] = src[i][p] as u8;
            }

            st.rebuild_main_loc(i);
        }

        st
    }

    #[inline]
    fn main_car(&self, i: usize, p: usize) -> usize {
        self.main[i][p] as usize
    }

    #[inline]
    fn siding_car(&self, j: usize, p: usize) -> usize {
        self.siding[j][p] as usize
    }

    #[inline]
    fn rebuild_main_loc(&mut self, i: usize) {
        for p in 0..self.main_len[i] {
            let car = self.main[i][p] as usize;
            self.loc[car] = enc_main(i, p);
        }
    }

    #[inline]
    fn rebuild_siding_loc(&mut self, j: usize) {
        for p in 0..self.siding_len[j] {
            let car = self.siding[j][p] as usize;
            self.loc[car] = enc_siding(j, p);
        }
    }
}

#[derive(Clone)]
struct Solution {
    name: String,
    turns: Vec<Vec<Move>>,
    score: i64,
    complete: bool,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
struct LastOp {
    typ: u8,
    i: usize,
    j: usize,
    turn: usize,
    idx: usize,
}

#[derive(Clone, Copy)]
struct Touch {
    i: [usize; 64],
    j: [usize; 64],
    n: usize,
    last_t: usize,
}

impl Touch {
    #[inline]
    fn new() -> Self {
        Self {
            i: [0; 64],
            j: [0; 64],
            n: 0,
            last_t: 0,
        }
    }

    #[inline]
    fn push(&mut self, i: usize, j: usize, t: usize) {
        if self.n < 64 {
            self.i[self.n] = i;
            self.j[self.n] = j;
            self.n += 1;
        }

        if self.last_t < t {
            self.last_t = t;
        }
    }

    #[inline]
    fn key_part(&self) -> (usize, usize, usize) {
        let mut max_i = 0usize;
        let mut max_j = 0usize;

        for p in 0..self.n {
            if max_i < self.i[p] {
                max_i = self.i[p];
            }
            if max_j < self.j[p] {
                max_j = self.j[p];
            }
        }

        let max_v = max_i.max(max_j);
        let min_v = max_i.min(max_j);

        (self.last_t, max_v, min_v)
    }
}

// -----------------------------------------------------
// Concurrent Builder
// -----------------------------------------------------
#[derive(Clone)]
struct ConcurrentBuilder {
    st: State,
    turns: Vec<Turn>,
    turn_count: usize,
    time_main: [usize; R],
    time_siding: [usize; R],

    last_main: [Option<LastOp>; R],
    last_siding: [Option<LastOp>; R],
}

impl ConcurrentBuilder {
    fn new(main: Vec<Vec<usize>>) -> Self {
        Self {
            st: State::new(main),
            turns: vec![],
            turn_count: 0,
            time_main: [0; R],
            time_siding: [0; R],
            last_main: [None; R],
            last_siding: [None; R],
        }
    }

    #[inline]
    fn compact_turn_count(&self) -> usize {
        self.turn_count
    }

    #[inline]
    fn can_schedule(&self, turn_idx: usize, i: usize, j: usize) -> bool {
        if turn_idx >= self.turns.len() {
            return true;
        }

        let tr = &self.turns[turn_idx];

        for idx in 0..tr.len {
            let mv = tr.mv[idx];

            if mv.i == i || mv.j == j {
                return false;
            }

            if (mv.i < i && mv.j >= j) || (mv.i > i && mv.j <= j) {
                return false;
            }
        }

        true
    }

    #[inline]
    fn eval_schedule(&self, i: usize, j: usize) -> usize {
        let mut t = self.time_main[i].max(self.time_siding[j]);

        while !self.can_schedule(t, i, j) {
            t += 1;
        }

        t
    }

    #[inline]
    fn can_merge_with_last(&self, typ: u8, i: usize, j: usize) -> Option<LastOp> {
        let lm = self.last_main[i]?;
        let ls = self.last_siding[j]?;

        if lm == ls && lm.typ == typ && lm.i == i && lm.j == j {
            Some(lm)
        } else {
            None
        }
    }

    fn apply_state_only(&mut self, typ: u8, i: usize, j: usize, k: usize) -> Option<()> {
        if k == 0 {
            return Some(());
        }

        if typ == 0 {
            let mlen = self.st.main_len[i];
            let slen = self.st.siding_len[j];

            if mlen < k {
                return None;
            }

            if slen + k > SIDING_CAP {
                return None;
            }

            for p in (0..slen).rev() {
                self.st.siding[j][p + k] = self.st.siding[j][p];
            }

            for p in 0..k {
                self.st.siding[j][p] = self.st.main[i][mlen - k + p];
            }

            self.st.main_len[i] -= k;
            self.st.siding_len[j] += k;

            self.st.rebuild_main_loc(i);
            self.st.rebuild_siding_loc(j);
        } else {
            let mlen = self.st.main_len[i];
            let slen = self.st.siding_len[j];

            if slen < k {
                return None;
            }

            if mlen + k > MAIN_CAP {
                return None;
            }

            for p in 0..k {
                self.st.main[i][mlen + p] = self.st.siding[j][p];
            }

            for p in k..slen {
                self.st.siding[j][p - k] = self.st.siding[j][p];
            }

            self.st.main_len[i] += k;
            self.st.siding_len[j] -= k;

            self.st.rebuild_main_loc(i);
            self.st.rebuild_siding_loc(j);
        }

        Some(())
    }

    fn add_concurrent_move(&mut self, typ: u8, i: usize, j: usize, k: usize) -> Option<usize> {
        if k == 0 {
            return Some(0);
        }

        if let Some(last) = self.can_merge_with_last(typ, i, j) {
            self.apply_state_only(typ, i, j, k)?;
            self.turns[last.turn].mv[last.idx].k += k;
            return Some(last.turn);
        }

        if typ == 0 {
            if self.st.main_len[i] < k {
                return None;
            }

            if self.st.siding_len[j] + k > SIDING_CAP {
                return None;
            }
        } else {
            if self.st.siding_len[j] < k {
                return None;
            }

            if self.st.main_len[i] + k > MAIN_CAP {
                return None;
            }
        }

        let t = self.eval_schedule(i, j);

        if t >= self.turns.len() || self.turns[t].len == 0 {
            if self.turn_count >= TMAX {
                return None;
            }
        }

        while self.turns.len() <= t {
            self.turns.push(Turn::default());
        }

        if self.turns[t].len == 0 {
            self.turn_count += 1;
        }

        let idx = self.turns[t].len;
        if idx >= R {
            return None;
        }

        let mv = Move { typ, i, j, k };
        self.turns[t].mv[idx] = mv;
        self.turns[t].len += 1;

        let last = LastOp {
            typ,
            i,
            j,
            turn: t,
            idx,
        };

        self.time_main[i] = t + 1;
        self.time_siding[j] = t + 1;
        self.last_main[i] = Some(last);
        self.last_siding[j] = Some(last);

        self.apply_state_only(typ, i, j, k)?;

        Some(t)
    }

    #[inline]
    fn move_main_to_siding(&mut self, i: usize, j: usize, k: usize) -> Option<usize> {
        self.add_concurrent_move(0, i, j, k)
    }

    #[inline]
    fn move_siding_to_main(&mut self, i: usize, j: usize, k: usize) -> Option<usize> {
        self.add_concurrent_move(1, i, j, k)
    }

    #[inline]
    fn find_car(&self, car: usize) -> Loc {
        decode_loc(self.st.loc[car])
    }

    #[inline]
    fn are_adjacent(&self, a: usize, b: usize) -> bool {
        let ea = self.st.loc[a];
        let eb = self.st.loc[b];

        // 上位側：side + line、下位5bit：pos
        (ea >> 5) == (eb >> 5) && ((ea & 31) + 1 == (eb & 31))
    }

    #[inline]
    fn is_head_anchored(&self, block: usize) -> bool {
        self.st.main_len[block] > 0
            && self.st.main[block][0] as usize == block * CARS_PER_LINE
    }

    fn front_connected_run_len(&self, j: usize) -> usize {
        if self.st.siding_len[j] == 0 {
            return 0;
        }

        let first = self.st.siding_car(j, 0);
        let block = first / CARS_PER_LINE;
        let block_end = block * CARS_PER_LINE + CARS_PER_LINE;

        let mut len = 1usize;

        while len < self.st.siding_len[j] {
            let prev = self.st.siding_car(j, len - 1);
            let cur = self.st.siding_car(j, len);

            if cur == prev + 1 && cur < block_end {
                len += 1;
            } else {
                break;
            }
        }

        len
    }

    fn pick_main_with_space(
        &self,
        need: usize,
        forbidden: &[usize],
        prefer_j: usize,
    ) -> Option<usize> {
        let mut best = None;
        let mut best_key = (usize::MAX, usize::MAX, usize::MAX, usize::MAX);

        for i in 0..R {
            if forbidden.contains(&i) {
                continue;
            }

            if self.st.main_len[i] + need <= MAIN_CAP {
                let t = self.eval_schedule(i, prefer_j);
                let key = (t, i.max(prefer_j), i.min(prefer_j), self.st.main_len[i]);

                if key < best_key {
                    best_key = key;
                    best = Some(i);
                }
            }
        }

        best
    }

    fn evacuate_siding_prefix(
        &mut self,
        sid: usize,
        cnt: usize,
        forbidden_main: &[usize],
        touch: &mut Touch,
    ) -> Option<()> {
        let mut left = cnt;

        while left > 0 {
            let i = self.pick_main_with_space(1, forbidden_main, sid)?;
            let room = MAIN_CAP - self.st.main_len[i];
            let k = left.min(room);

            let t = self.move_siding_to_main(i, sid, k)?;
            touch.push(i, sid, t);

            left -= k;
        }

        Some(())
    }
}

// -----------------------------------------------------
// Candidate & Evaluation
// -----------------------------------------------------
#[derive(Clone)]
struct CandResult {
    b: ConcurrentBuilder,
    key: (usize, usize, usize, usize),
    label: usize,
}

fn cand_key_touch(touch: &Touch, gain: usize) -> (usize, usize, usize, usize) {
    let (last_t, max_v, min_v) = touch.key_part();
    (last_t, max_v, min_v, usize::MAX - gain)
}

fn connected_run_len_from(b: &ConcurrentBuilder, a: usize) -> usize {
    let mut len = 1;

    while a + len < N && b.are_adjacent(a + len - 1, a + len) {
        len += 1;
    }

    len
}

fn register_candidate(
    best: &mut Option<CandResult>,
    cb: ConcurrentBuilder,
    touch: &Touch,
    label: usize,
) {
    let gain = connected_run_len_from(&cb, label);
    let key = cand_key_touch(touch, gain);
    let cand = CandResult { b: cb, key, label };

    if best.as_ref().map_or(true, |cur| cand.key < cur.key) {
        *best = Some(cand);
    }
}

// -----------------------------------------------------
// Small operation helpers
// -----------------------------------------------------
fn try_move_main_suffix_to_siding(
    cb: &mut ConcurrentBuilder,
    i: usize,
    pos: usize,
    j: usize,
    touch: &mut Touch,
) -> Option<()> {
    if pos >= cb.st.main_len[i] {
        return None;
    }

    let k = cb.st.main_len[i] - pos;
    let t = cb.move_main_to_siding(i, j, k)?;
    touch.push(i, j, t);

    Some(())
}

fn try_clear_main_after_pos_to_siding(
    cb: &mut ConcurrentBuilder,
    i: usize,
    pos: usize,
    j: usize,
    touch: &mut Touch,
) -> Option<()> {
    if pos + 1 >= cb.st.main_len[i] {
        return Some(());
    }

    try_move_main_suffix_to_siding(cb, i, pos + 1, j, touch)
}

fn try_clear_main_all_to_siding(
    cb: &mut ConcurrentBuilder,
    i: usize,
    j: usize,
    touch: &mut Touch,
) -> Option<()> {
    let k = cb.st.main_len[i];

    if k == 0 {
        return Some(());
    }

    let t = cb.move_main_to_siding(i, j, k)?;
    touch.push(i, j, t);

    Some(())
}

fn try_siding_run_to_main(
    cb: &mut ConcurrentBuilder,
    i: usize,
    j: usize,
    touch: &mut Touch,
) -> Option<usize> {
    let run = cb.front_connected_run_len(j);

    if run == 0 {
        return None;
    }

    let room = MAIN_CAP - cb.st.main_len[i];

    if room == 0 {
        return None;
    }

    let k = run.min(room);
    let t = cb.move_siding_to_main(i, j, k)?;
    touch.push(i, j, t);

    Some(k)
}

// -----------------------------------------------------
// Anchor & Connect
// -----------------------------------------------------
fn try_anchor_head(base: &ConcurrentBuilder, block: usize) -> Option<CandResult> {
    let car = block * CARS_PER_LINE;

    if base.is_head_anchored(block) {
        return None;
    }

    let loc = base.find_car(car);
    let mut best: Option<CandResult> = None;

    match loc {
        Loc::Main(src, pos) => {
            for focus_j in 0..R {
                let k_focus = base.st.main_len[src] - pos;

                if base.st.siding_len[focus_j] + k_focus > SIDING_CAP {
                    continue;
                }

                for clear_j in 0..R {
                    if clear_j == focus_j {
                        continue;
                    }

                    let mut cb = base.clone();
                    let mut touch = Touch::new();

                    if try_move_main_suffix_to_siding(
                        &mut cb,
                        src,
                        pos,
                        focus_j,
                        &mut touch,
                    )
                    .is_none()
                    {
                        continue;
                    }

                    if cb.st.main_len[block] + cb.st.siding_len[clear_j] > SIDING_CAP {
                        continue;
                    }

                    if try_clear_main_all_to_siding(
                        &mut cb,
                        block,
                        clear_j,
                        &mut touch,
                    )
                    .is_none()
                    {
                        continue;
                    }

                    if try_siding_run_to_main(
                        &mut cb,
                        block,
                        focus_j,
                        &mut touch,
                    )
                    .is_none()
                    {
                        continue;
                    }

                    register_candidate(&mut best, cb, &touch, car);
                }
            }
        }

        Loc::Siding(src_j, pos) => {
            for clear_j in 0..R {
                if clear_j == src_j {
                    continue;
                }

                let mut cb = base.clone();
                let mut touch = Touch::new();

                if cb
                    .evacuate_siding_prefix(src_j, pos, &[block], &mut touch)
                    .is_none()
                {
                    continue;
                }

                if cb.st.main_len[block] + cb.st.siding_len[clear_j] > SIDING_CAP {
                    continue;
                }

                if try_clear_main_all_to_siding(
                    &mut cb,
                    block,
                    clear_j,
                    &mut touch,
                )
                .is_none()
                {
                    continue;
                }

                if try_siding_run_to_main(
                    &mut cb,
                    block,
                    src_j,
                    &mut touch,
                )
                .is_none()
                {
                    continue;
                }

                register_candidate(&mut best, cb, &touch, car);
            }
        }
    }

    best
}

fn try_connect_pair(base: &ConcurrentBuilder, a: usize) -> Option<CandResult> {
    let bcar = a + 1;

    if a / CARS_PER_LINE != bcar / CARS_PER_LINE || base.are_adjacent(a, bcar) {
        return None;
    }

    if a % CARS_PER_LINE == 0 && !base.is_head_anchored(a / CARS_PER_LINE) {
        return None;
    }

    let la = base.find_car(a);
    let lb = base.find_car(bcar);
    let mut best: Option<CandResult> = None;

    match (la, lb) {
        (Loc::Main(ia, pa), Loc::Main(ib, pb)) => {
            if ia == ib {
                if pa >= pb {
                    return None;
                }

                let offset = pb - (pa + 1);

                for j in 0..R {
                    let mut cb = base.clone();
                    let mut touch = Touch::new();

                    if try_move_main_suffix_to_siding(&mut cb, ia, pa + 1, j, &mut touch)
                        .is_none()
                    {
                        continue;
                    }

                    if cb
                        .evacuate_siding_prefix(j, offset, &[ia], &mut touch)
                        .is_none()
                    {
                        continue;
                    }

                    if try_siding_run_to_main(&mut cb, ia, j, &mut touch).is_none() {
                        continue;
                    }

                    register_candidate(&mut best, cb, &touch, a);
                }
            } else {
                let clear_a = pa + 1 < base.st.main_len[ia];

                for jb in 0..R {
                    let kb = base.st.main_len[ib] - pb;

                    if base.st.siding_len[jb] + kb > SIDING_CAP {
                        continue;
                    }

                    if clear_a {
                        for ja in 0..R {
                            if ja == jb {
                                continue;
                            }

                            let ka = base.st.main_len[ia] - (pa + 1);

                            if base.st.siding_len[ja] + ka > SIDING_CAP {
                                continue;
                            }

                            let mut cb = base.clone();
                            let mut touch = Touch::new();

                            if try_clear_main_after_pos_to_siding(
                                &mut cb,
                                ia,
                                pa,
                                ja,
                                &mut touch,
                            )
                            .is_none()
                            {
                                continue;
                            }

                            if try_move_main_suffix_to_siding(
                                &mut cb,
                                ib,
                                pb,
                                jb,
                                &mut touch,
                            )
                            .is_none()
                            {
                                continue;
                            }

                            if try_siding_run_to_main(&mut cb, ia, jb, &mut touch).is_none() {
                                continue;
                            }

                            register_candidate(&mut best, cb, &touch, a);
                        }
                    } else {
                        let mut cb = base.clone();
                        let mut touch = Touch::new();

                        if try_move_main_suffix_to_siding(
                            &mut cb,
                            ib,
                            pb,
                            jb,
                            &mut touch,
                        )
                        .is_none()
                        {
                            continue;
                        }

                        if try_siding_run_to_main(&mut cb, ia, jb, &mut touch).is_none() {
                            continue;
                        }

                        register_candidate(&mut best, cb, &touch, a);
                    }
                }
            }
        }

        (Loc::Main(ia, pa), Loc::Siding(jb, pb)) => {
            let clear_a = pa + 1 < base.st.main_len[ia];

            if clear_a {
                for ja in 0..R {
                    if ja == jb {
                        continue;
                    }

                    let ka = base.st.main_len[ia] - (pa + 1);

                    if base.st.siding_len[ja] + ka > SIDING_CAP {
                        continue;
                    }

                    let mut cb = base.clone();
                    let mut touch = Touch::new();

                    if try_clear_main_after_pos_to_siding(
                        &mut cb,
                        ia,
                        pa,
                        ja,
                        &mut touch,
                    )
                    .is_none()
                    {
                        continue;
                    }

                    if cb
                        .evacuate_siding_prefix(jb, pb, &[ia], &mut touch)
                        .is_none()
                    {
                        continue;
                    }

                    if try_siding_run_to_main(&mut cb, ia, jb, &mut touch).is_none() {
                        continue;
                    }

                    register_candidate(&mut best, cb, &touch, a);
                }
            } else {
                let mut cb = base.clone();
                let mut touch = Touch::new();

                if cb
                    .evacuate_siding_prefix(jb, pb, &[ia], &mut touch)
                    .is_some()
                    && try_siding_run_to_main(&mut cb, ia, jb, &mut touch).is_some()
                {
                    register_candidate(&mut best, cb, &touch, a);
                }
            }
        }

        (Loc::Siding(ja, pa), Loc::Main(ib, pb)) => {
            for im in 0..R {
                if im == ib {
                    continue;
                }

                if base.st.main_len[im] + 1 > MAIN_CAP {
                    continue;
                }

                for jb in 0..R {
                    if jb == ja {
                        continue;
                    }

                    let kb = base.st.main_len[ib] - pb;

                    if base.st.siding_len[jb] + kb > SIDING_CAP {
                        continue;
                    }

                    let mut cb = base.clone();
                    let mut touch = Touch::new();

                    if cb
                        .evacuate_siding_prefix(ja, pa, &[im, ib], &mut touch)
                        .is_none()
                    {
                        continue;
                    }

                    if try_siding_run_to_main(&mut cb, im, ja, &mut touch).is_none() {
                        continue;
                    }

                    if try_move_main_suffix_to_siding(
                        &mut cb,
                        ib,
                        pb,
                        jb,
                        &mut touch,
                    )
                    .is_none()
                    {
                        continue;
                    }

                    if try_siding_run_to_main(&mut cb, im, jb, &mut touch).is_none() {
                        continue;
                    }

                    register_candidate(&mut best, cb, &touch, a);
                }
            }
        }

        (Loc::Siding(ja, pa), Loc::Siding(jb, pb)) => {
            if ja == jb {
                return None;
            }

            for im in 0..R {
                if base.st.main_len[im] + 2 > MAIN_CAP {
                    continue;
                }

                let mut cb = base.clone();
                let mut touch = Touch::new();

                if cb
                    .evacuate_siding_prefix(ja, pa, &[im], &mut touch)
                    .is_none()
                {
                    continue;
                }

                if cb
                    .evacuate_siding_prefix(jb, pb, &[im], &mut touch)
                    .is_none()
                {
                    continue;
                }

                if try_siding_run_to_main(&mut cb, im, ja, &mut touch).is_none() {
                    continue;
                }

                if try_siding_run_to_main(&mut cb, im, jb, &mut touch).is_none() {
                    continue;
                }

                register_candidate(&mut best, cb, &touch, a);
            }
        }
    }

    best
}

// -----------------------------------------------------
// Beam Search Components
// -----------------------------------------------------
#[derive(Clone)]
struct BeamNode {
    b: ConcurrentBuilder,
    turns: usize,
}

fn count_connections(b: &ConcurrentBuilder) -> usize {
    let mut count = 0;

    for r in 0..R {
        if b.is_head_anchored(r) {
            count += 1;
        }
    }

    for a in 0..N - 1 {
        if a / CARS_PER_LINE != (a + 1) / CARS_PER_LINE {
            continue;
        }

        if b.are_adjacent(a, a + 1) {
            count += 1;
        }
    }

    count
}

fn compute_hash(b: &ConcurrentBuilder, zobrist: &[u64; N]) -> u64 {
    let mut h = 0;

    for i in 0..R {
        if b.st.main_len[i] > 0 {
            let car = b.st.main_car(i, b.st.main_len[i] - 1);
            h ^= zobrist[car];
        }
    }

    for j in 0..R {
        if b.st.siding_len[j] > 0 {
            let car = b.st.siding_car(j, 0);
            h ^= zobrist[car];
        }
    }

    h
}

fn push_beam_candidate(
    prof: &mut Profiler,
    beams: &mut Vec<HashMap<u64, BeamNode>>,
    zobrist: &[u64; N],
    step: usize,
    mut next_b: ConcurrentBuilder,
    best_complete: &mut Option<ConcurrentBuilder>,
) {
    let next_conns = timed!(prof, "beam.count_connections", { count_connections(&next_b) });

    if next_conns <= step {
        prof.inc("beam.skip_no_progress", 1);
        return;
    }

    let target_step = next_conns.min(100);
    let h = timed!(prof, "beam.compute_hash", { compute_hash(&next_b, zobrist) });
    let turns = next_b.compact_turn_count();

    if turns >= TMAX {
        prof.inc("beam.skip_turns_over_tmax", 1);
        return;
    }

    let mut best_complete_candidate: Option<ConcurrentBuilder> = None;

    timed!(prof, "beam.insert_or_update", {
        match beams[target_step].get_mut(&h) {
            Some(entry) => {
                if turns < entry.turns {
                    entry.turns = turns;
                    std::mem::swap(&mut entry.b, &mut next_b);

                    if target_step == 100 {
                        best_complete_candidate = Some(entry.b.clone());
                    }

                    prof.inc("beam.hash_entry_updated", 1);
                } else {
                    prof.inc("beam.hash_entry_rejected", 1);
                }
            }
            None => {
                if target_step == 100 {
                    best_complete_candidate = Some(next_b.clone());
                }

                beams[target_step].insert(
                    h,
                    BeamNode {
                        b: next_b,
                        turns,
                    },
                );

                prof.inc("beam.hash_entry_inserted", 1);
            }
        }
    });

    if let Some(cand) = best_complete_candidate {
        let best_turns = best_complete
            .as_ref()
            .map_or(usize::MAX, |cur| cur.compact_turn_count());

        if turns < best_turns {
            *best_complete = Some(cand);
            prof.inc("beam.best_complete_updated", 1);
        }
    }
}

// -----------------------------------------------------
// Solvers
// -----------------------------------------------------
fn solver_connect_rule(initial: &[Vec<usize>]) -> Solution {
    let mut prof = Profiler::new("connect_rule");
    let mut b = ConcurrentBuilder::new(initial.to_vec());
    let mut made = 0usize;

    loop {
        if b.compact_turn_count() >= TMAX {
            break;
        }

        let mut best: Option<CandResult> = None;

        for block in 0..R {
            prof.inc("greedy.anchor_attempts", 1);

            let cand_opt = timed!(prof, "greedy.try_anchor_head", {
                try_anchor_head(&b, block)
            });

            if let Some(cand) = cand_opt {
                prof.inc("greedy.anchor_successes", 1);

                if best.as_ref().map_or(true, |cur| cand.key < cur.key) {
                    best = Some(cand);
                }
            }
        }

        for a in 0..N - 1 {
            if a / CARS_PER_LINE != (a + 1) / CARS_PER_LINE {
                continue;
            }

            if b.are_adjacent(a, a + 1) {
                continue;
            }

            prof.inc("greedy.connect_attempts", 1);

            let cand_opt = timed!(prof, "greedy.try_connect_pair", {
                try_connect_pair(&b, a)
            });

            if let Some(cand) = cand_opt {
                prof.inc("greedy.connect_successes", 1);

                if best.as_ref().map_or(true, |cur| cand.key < cur.key) {
                    best = Some(cand);
                }
            }
        }

        let Some(cand) = best else {
            break;
        };

        let label = cand.label;
        b = cand.b;
        made += 1;
        prof.inc("greedy.accepted_moves", 1);

        if DEBUG && made % 10 == 0 {
            eprintln!(
                "connect_rule progress: made={} last_label={} turns={}",
                made,
                label,
                b.compact_turn_count()
            );
        }
    }

    let turns = timed!(prof, "greedy.compact_turns", { compact_turns(&b.turns) });
    let (score, complete) = timed!(prof, "greedy.compute_score", {
        compute_score(initial, &turns)
    });

    prof.report();

    Solution {
        name: "connect_rule".to_string(),
        turns,
        score,
        complete,
    }
}

fn solver_beam_search(initial: &[Vec<usize>]) -> Solution {
    let mut prof = Profiler::new("beam_search");

    let zobrist = compute_zobrist();
    let mut beams: Vec<HashMap<u64, BeamNode>> = (0..=100).map(|_| HashMap::new()).collect();

    let init_b = ConcurrentBuilder::new(initial.to_vec());

    let init_conns = timed!(prof, "beam.count_connections", {
        count_connections(&init_b)
    });
    let init_hash = timed!(prof, "beam.compute_hash", {
        compute_hash(&init_b, &zobrist)
    });

    beams[init_conns].insert(
        init_hash,
        BeamNode {
            turns: init_b.compact_turn_count(),
            b: init_b,
        },
    );

    let mut best_complete: Option<ConcurrentBuilder> = None;

    for step in 0..100 {
        if beams[step].is_empty() {
            continue;
        }

        let (nodes, selected_best_turns, raw_nodes) = timed!(prof, "beam.sort_and_clone_nodes", {
            let mut selected: Vec<&BeamNode> = beams[step].values().collect();
            selected.sort_by_key(|n| n.turns);
            selected.truncate(BEAM_WIDTH);

            let nodes: Vec<ConcurrentBuilder> = selected.iter().map(|n| n.b.clone()).collect();
            let selected_best_turns = selected.first().map(|n| n.turns).unwrap_or(0);
            let raw_nodes = beams[step].len();

            (nodes, selected_best_turns, raw_nodes)
        });

        prof.inc("beam.steps_with_nodes", 1);
        prof.inc("beam.nodes_selected", nodes.len() as u64);

        if DEBUG {
            eprintln!(
                "beam_search progress: step={:<3} raw_nodes={:<5} selected={:<3} best_turns={}",
                step,
                raw_nodes,
                nodes.len(),
                selected_best_turns
            );
        }

        for b in nodes {
            prof.inc("beam.nodes_expanded", 1);

            if b.compact_turn_count() >= TMAX {
                prof.inc("beam.skip_tmax", 1);
                continue;
            }

            for block in 0..R {
                if !b.is_head_anchored(block) {
                    prof.inc("beam.anchor_attempts", 1);

                    let cand_opt = timed!(prof, "beam.try_anchor_head", {
                        try_anchor_head(&b, block)
                    });

                    if let Some(cand) = cand_opt {
                        prof.inc("beam.anchor_successes", 1);
                        prof.inc("beam.successors_generated", 1);

                        push_beam_candidate(
                            &mut prof,
                            &mut beams,
                            &zobrist,
                            step,
                            cand.b,
                            &mut best_complete,
                        );
                    }
                }
            }

            for a in 0..N - 1 {
                if a / CARS_PER_LINE == (a + 1) / CARS_PER_LINE && !b.are_adjacent(a, a + 1) {
                    prof.inc("beam.connect_attempts", 1);

                    let cand_opt = timed!(prof, "beam.try_connect_pair", {
                        try_connect_pair(&b, a)
                    });

                    if let Some(cand) = cand_opt {
                        prof.inc("beam.connect_successes", 1);
                        prof.inc("beam.successors_generated", 1);

                        push_beam_candidate(
                            &mut prof,
                            &mut beams,
                            &zobrist,
                            step,
                            cand.b,
                            &mut best_complete,
                        );
                    }
                }
            }
        }
    }

    let final_b = timed!(prof, "beam.final_select", {
        if let Some(b) = best_complete {
            b
        } else {
            let mut best_b = None;

            for step in (0..100).rev() {
                if !beams[step].is_empty() {
                    let best_node = beams[step].values().min_by_key(|n| n.turns).unwrap();
                    best_b = Some(best_node.b.clone());
                    prof.inc("beam.final_selected_step", step as u64);
                    break;
                }
            }

            best_b.unwrap()
        }
    });

    let turns = timed!(prof, "beam.compact_turns", { compact_turns(&final_b.turns) });
    let (score, complete) = timed!(prof, "beam.compute_score", {
        compute_score(initial, &turns)
    });

    prof.report();

    Solution {
        name: "beam_search".to_string(),
        turns,
        score,
        complete,
    }
}

// -----------------------------------------------------
// Validation / Score / Output
// -----------------------------------------------------
fn compact_turns(turns: &[Turn]) -> Vec<Vec<Move>> {
    let mut res = Vec::with_capacity(turns.len());

    for tr in turns {
        if tr.len == 0 {
            continue;
        }

        let mut v = Vec::with_capacity(tr.len);
        for idx in 0..tr.len {
            v.push(tr.mv[idx]);
        }
        res.push(v);
    }

    res
}

fn compute_score(initial: &[Vec<usize>], turns: &[Vec<Move>]) -> (i64, bool) {
    let mut main = initial.to_vec();
    let mut siding = vec![vec![]; R];

    if turns.len() > TMAX {
        return (0, false);
    }

    for moves in turns {
        if moves.is_empty() || !check_crossing(moves) {
            return (0, false);
        }

        for &mv in moves {
            if mv.typ == 0 {
                if main[mv.i].len() < mv.k {
                    return (0, false);
                }

                let len = main[mv.i].len();
                let moved: Vec<usize> = main[mv.i].drain(len - mv.k..).collect();
                let mut next = moved;
                next.extend(siding[mv.j].iter().copied());
                siding[mv.j] = next;
            } else {
                if siding[mv.j].len() < mv.k {
                    return (0, false);
                }

                let moved: Vec<usize> = siding[mv.j].drain(0..mv.k).collect();
                main[mv.i].extend(moved);
            }
        }

        for i in 0..R {
            if main[i].len() > MAIN_CAP || siding[i].len() > SIDING_CAP {
                return (0, false);
            }
        }
    }

    let mut complete = true;

    for r in 0..R {
        if main[r].len() != CARS_PER_LINE {
            complete = false;
            break;
        }

        for c in 0..CARS_PER_LINE {
            if main[r][c] != CARS_PER_LINE * r + c {
                complete = false;
                break;
            }
        }
    }

    if complete {
        return ((100 * R + 4000 - turns.len()) as i64, true);
    }

    let mut score = 0i64;

    for r in 0..R {
        for (c, &car) in main[r].iter().enumerate() {
            if car / CARS_PER_LINE == r {
                score += 1;

                if car % CARS_PER_LINE == c {
                    score += 9;
                }
            }
        }
    }

    (score, false)
}

fn check_crossing(moves: &[Move]) -> bool {
    let mut used_main = [false; R];
    let mut used_siding = [false; R];

    for mv in moves {
        if used_main[mv.i] || used_siding[mv.j] {
            return false;
        }

        used_main[mv.i] = true;
        used_siding[mv.j] = true;
    }

    let mut v = moves.to_vec();
    v.sort_by_key(|mv| mv.i);

    for w in v.windows(2) {
        if w[0].j >= w[1].j {
            return false;
        }
    }

    true
}

fn output_solution(sol: &Solution) {
    println!("{}", sol.turns.len());

    for turn in &sol.turns {
        println!("{}", turn.len());

        for mv in turn {
            println!("{} {} {} {}", mv.typ, mv.i, mv.j, mv.k);
        }
    }
}

// -----------------------------------------------------
// Entry point
// -----------------------------------------------------
fn main() {
    let total_start = Instant::now();

    let mut s = String::new();
    io::stdin().read_to_string(&mut s).unwrap();

    let mut it = s.split_whitespace();

    let input_r: usize = it.next().unwrap().parse().unwrap();
    assert_eq!(input_r, R);

    let mut main_st = vec![vec![]; R];

    for r in 0..R {
        for _ in 0..CARS_PER_LINE {
            let x: usize = it.next().unwrap().parse().unwrap();
            main_st[r].push(x);
        }
    }

    let sol_greedy = if USE_GREEDY_SOLVER {
        Some(solver_connect_rule(&main_st))
    } else {
        None
    };

    let sol_beam = if USE_BEAM_SOLVER {
        Some(solver_beam_search(&main_st))
    } else {
        None
    };

    let best_sol = match (&sol_greedy, &sol_beam) {
        (Some(g), Some(b)) => {
            if b.score >= g.score {
                b
            } else {
                g
            }
        }
        (Some(g), None) => g,
        (None, Some(b)) => b,
        (None, None) => unreachable!(),
    };

    eprintln!("=== Results ===");

    if let Some(sol) = &sol_greedy {
        eprintln!(
            "greedy : score={:<6} turns={:<5} complete={}",
            sol.score,
            sol.turns.len(),
            sol.complete
        );
    } else {
        eprintln!("greedy : skipped");
    }

    if let Some(sol) = &sol_beam {
        eprintln!(
            "beam   : score={:<6} turns={:<5} complete={}",
            sol.score,
            sol.turns.len(),
            sol.complete
        );
    } else {
        eprintln!("beam   : skipped");
    }

    eprintln!("Picked: {}", best_sol.name);

    if PROFILE {
        eprintln!(
            "total elapsed: {:.3} ms",
            total_start.elapsed().as_secs_f64() * 1000.0
        );
    }

    output_solution(best_sol);
}