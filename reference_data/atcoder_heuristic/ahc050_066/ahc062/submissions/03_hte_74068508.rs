use std::collections::HashMap;
use std::convert::TryInto;
use std::io::{self, Read, Write};
use std::time::Instant;

const INF_NEG: i64 = -(1_i64 << 60);
const SOURCE_TOP_PERCENT: usize = 13;
const FULL_DISTRIBUTION_TAIL: usize = 7;
const TOP_LEFT: [[u8; 5]; 4] = [
    [1, 1, 1, 1, 1],
    [1, 1, 2, 2, 2],
    [1, 1, 2, 3, 3],
    [1, 1, 2, 3, 4],
];
const TOP_RIGHT: [[u8; 4]; 4] = [
    [1, 1, 1, 1],
    [2, 2, 2, 1],
    [3, 3, 2, 1],
    [4, 3, 2, 1],
];
const MID_LEFT: [[u8; 4]; 8] = [
    [1, 2, 3, 4],
    [1, 2, 3, 3],
    [1, 2, 2, 2],
    [1, 1, 1, 1],
    [5, 5, 5, 5],
    [5, 6, 6, 6],
    [5, 6, 7, 7],
    [5, 6, 7, 8],
];
const MID_RIGHT: [[u8; 4]; 8] = [
    [8, 7, 6, 5],
    [7, 7, 6, 5],
    [6, 6, 6, 5],
    [5, 5, 5, 5],
    [1, 1, 1, 1],
    [2, 2, 2, 1],
    [3, 3, 2, 1],
    [4, 3, 2, 1],
];
const BOT_LEFT: [[u8; 4]; 4] = [
    [1, 2, 3, 4],
    [1, 2, 3, 3],
    [1, 2, 2, 2],
    [1, 1, 1, 1],
];
const BOT_RIGHT: [[u8; 4]; 4] = [
    [4, 3, 2, 1],
    [3, 3, 2, 1],
    [2, 2, 2, 1],
    [1, 1, 1, 1],
];

#[derive(Clone)]
struct StateTable<const K: usize> {
    inv_states: Vec<[u8; K]>,
    masks: Vec<u16>,
    next_state: Vec<u16>,
}

#[derive(Clone, Copy)]
enum BandKind {
    Top,
    MidEven,
    MidOdd,
    Bottom,
}

#[derive(Clone)]
struct Band {
    kind: BandKind,
    top_row: usize,
    left_w: usize,
    right_w: usize,
    labels: Vec<Vec<u8>>,
}

fn log_progress(start: &Instant, message: &str) {
    eprintln!("[{:.3}s] {}", start.elapsed().as_secs_f64(), message);
}

fn factorial(n: usize) -> usize {
    (1..=n).product()
}

fn perm_to_index<const K: usize>(perm: &[u8; K]) -> usize {
    let mut used = [false; K];
    let mut idx = 0_usize;
    for i in 0..K {
        let v = perm[i] as usize;
        let smaller_unused = (0..v).filter(|&x| !used[x]).count();
        idx += smaller_unused * factorial(K - 1 - i);
        used[v] = true;
    }
    idx
}

fn valid_masks<const K: usize>() -> Vec<u16> {
    let edge_count = K - 1;
    let mut masks = Vec::new();
    for mask in 0..(1_usize << edge_count) {
        if (mask & (mask << 1)) == 0 {
            masks.push(mask as u16);
        }
    }
    masks
}

fn apply_mask<const K: usize>(state: &[u8; K], mask: u16) -> [u8; K] {
    let mut next = *state;
    let mut i = 0;
    while i + 1 < K {
        if ((mask >> i) & 1) != 0 {
            next.swap(i, i + 1);
            i += 2;
        } else {
            i += 1;
        }
    }
    next
}

fn build_inv_states<const K: usize>() -> Vec<[u8; K]> {
    fn dfs<const K: usize>(idx: usize, used: &mut [bool; K], cur: &mut [u8; K], out: &mut Vec<[u8; K]>) {
        if idx == K {
            out.push(invert_perm(*cur));
            return;
        }
        for v in 0..K {
            if !used[v] {
                used[v] = true;
                cur[idx] = v as u8;
                dfs(idx + 1, used, cur, out);
                used[v] = false;
            }
        }
    }

    let mut out = Vec::with_capacity(factorial(K));
    let mut used = [false; K];
    let mut cur = [0_u8; K];
    dfs(0, &mut used, &mut cur, &mut out);
    out
}

fn invert_perm<const K: usize>(p: [u8; K]) -> [u8; K] {
    let mut inv = [0_u8; K];
    for (lane, &row) in p.iter().enumerate() {
        inv[row as usize] = lane as u8;
    }
    inv
}

fn build_state_table<const K: usize>() -> StateTable<K> {
    let inv_states = build_inv_states::<K>();
    let masks = valid_masks::<K>();
    let mut next_state = vec![0_u16; inv_states.len() * masks.len()];
    for (sid, inv_state) in inv_states.iter().enumerate() {
        let base = sid * masks.len();
        for (mi, &mask) in masks.iter().enumerate() {
            let next_inv = apply_mask(inv_state, mask);
            let next_state_perm = invert_perm(next_inv);
            next_state[base + mi] = perm_to_index(&next_state_perm) as u16;
        }
    }
    StateTable {
        inv_states,
        masks,
        next_state,
    }
}

fn boundary_state_left<const H: usize, const W: usize>(pat: &[[u8; W]; H]) -> Vec<u8> {
    let mut pos = vec![0_u8; H];
    for (r, row) in pat.iter().enumerate() {
        pos[(row[W - 1] - 1) as usize] = r as u8;
    }
    pos
}

fn boundary_state_right<const H: usize, const W: usize>(pat: &[[u8; W]; H]) -> Vec<u8> {
    let mut pos = vec![0_u8; H];
    for (r, row) in pat.iter().enumerate() {
        pos[(row[0] - 1) as usize] = r as u8;
    }
    pos
}

fn fill_fixed<const H: usize, const W: usize>(dst: &mut [Vec<u8>], left: &[[u8; W]; H], col0: usize) {
    for r in 0..H {
        for c in 0..W {
            dst[r][col0 + c] = left[r][c];
        }
    }
}

fn optimize_band_4(
    a: &[Vec<usize>],
    top_row: usize,
    kind: BandKind,
    seg_id_by_lane: &[usize],
    prefix_len_by_lane: &[usize],
    table: &StateTable<4>,
) -> Band {
    let n = a.len();
    let (left_w, right_w, height, start_state, end_state, left_pat_4, right_pat_4) = match kind {
        BandKind::Top => (
            5_usize,
            4_usize,
            4_usize,
            boundary_state_left(&TOP_LEFT),
            boundary_state_right(&TOP_RIGHT),
            TOP_LEFT.map(|x| x.to_vec()).to_vec(),
            TOP_RIGHT.map(|x| x.to_vec()).to_vec(),
        ),
        BandKind::Bottom => (
            4_usize,
            4_usize,
            4_usize,
            boundary_state_left(&BOT_LEFT),
            boundary_state_right(&BOT_RIGHT),
            BOT_LEFT.map(|x| x.to_vec()).to_vec(),
            BOT_RIGHT.map(|x| x.to_vec()).to_vec(),
        ),
        _ => unreachable!(),
    };
    let mut labels = vec![vec![0_u8; n]; height];
    for r in 0..height {
        for c in 0..left_w {
            labels[r][c] = left_pat_4[r][c];
        }
        for c in 0..right_w {
            labels[r][n - right_w + c] = right_pat_4[r][c];
        }
    }
    let mid_l = left_w;
    let mid_r = n - right_w;
    let width_mid = mid_r - mid_l;
    let start_arr: [u8; 4] = start_state.as_slice().try_into().unwrap();
    let end_arr: [u8; 4] = end_state.as_slice().try_into().unwrap();
    let start_id = perm_to_index(&start_arr);
    let end_id = perm_to_index(&end_arr);
    let m = table.inv_states.len();
    let mut dp_prev = vec![INF_NEG; m];
    dp_prev[start_id] = 0;
    let mut prev = vec![vec![u16::MAX; m]; width_mid];
    let mut gain_buf = vec![0_i64; m];
    let mut active = Vec::<usize>::with_capacity(m);
    for off in 0..width_mid {
        let c = mid_l + off;
        let mid_idx = match kind {
            BandKind::Bottom => off,
            BandKind::Top => width_mid - 1 - off,
            _ => unreachable!(),
        };
        for (sid, gain_slot) in gain_buf.iter_mut().enumerate() {
            let inv = table.inv_states[sid];
            let mut gain = 0_i64;
            for r in 0..height {
                let lane = inv[r] as usize + 1;
                let w = (seg_id_by_lane[lane] * n + prefix_len_by_lane[lane] + mid_idx) as i64;
                gain += w * a[top_row + r][c] as i64;
            }
            *gain_slot = gain;
        }
        active.clear();
        for (sid, &score) in dp_prev.iter().enumerate() {
            if score != INF_NEG {
                active.push(sid);
            }
        }
        let mut dp_cur = vec![INF_NEG; m];
        for &sid in &active {
            let cur = dp_prev[sid];
            let base = sid * table.masks.len();
            for mi in 0..table.masks.len() {
                let tid = table.next_state[base + mi] as usize;
                let cand = cur + gain_buf[tid];
                if cand > dp_cur[tid] {
                    dp_cur[tid] = cand;
                    prev[off][tid] = sid as u16;
                }
            }
        }
        dp_prev = dp_cur;
    }
    let mut cur = end_id;
    let mut chosen = vec![0_usize; width_mid];
    for off in (0..width_mid).rev() {
        chosen[off] = cur;
        cur = prev[off][cur] as usize;
    }
    assert_eq!(cur, start_id);
    for off in 0..width_mid {
        let c = mid_l + off;
        let inv = table.inv_states[chosen[off]];
        for r in 0..height {
            labels[r][c] = inv[r] + 1;
        }
    }
    Band {
        kind,
        top_row,
        left_w,
        right_w,
        labels,
    }
}

fn optimize_band_8(
    a: &[Vec<usize>],
    top_row: usize,
    kind: BandKind,
    seg_id_by_lane: &[usize],
    prefix_len_by_lane: &[usize],
    table: &StateTable<8>,
) -> Band {
    let n = a.len();
    let height = 8;
    let left_w = 4;
    let right_w = 4;
    let start_state = boundary_state_left(&MID_LEFT);
    let end_state = boundary_state_right(&MID_RIGHT);
    let start_arr: [u8; 8] = start_state.as_slice().try_into().unwrap();
    let end_arr: [u8; 8] = end_state.as_slice().try_into().unwrap();
    let start_id = perm_to_index(&start_arr);
    let end_id = perm_to_index(&end_arr);
    let mut labels = vec![vec![0_u8; n]; height];
    fill_fixed(&mut labels, &MID_LEFT, 0);
    fill_fixed(&mut labels, &MID_RIGHT, n - right_w);
    let mid_l = left_w;
    let mid_r = n - right_w;
    let width_mid = mid_r - mid_l;
    let m = table.inv_states.len();
    let mask_len = table.masks.len();
    let lane_weight_base = prefix_len_by_lane
        .iter()
        .enumerate()
        .map(|(lane, &prefix)| (seg_id_by_lane[lane] * n + prefix) as i64)
        .collect::<Vec<_>>();
    let mut dp_prev = vec![INF_NEG; m];
    dp_prev[start_id] = 0;
    let mut dp_cur = vec![INF_NEG; m];
    let mut prev = vec![u16::MAX; width_mid * m];
    let mut gain_buf = vec![0_i64; m];
    let mut active = Vec::<usize>::with_capacity(m);
    let mut sources = Vec::<usize>::with_capacity(m);
    for off in 0..width_mid {
        let c = mid_l + off;
        let mid_idx = match kind {
            BandKind::MidEven => off,
            BandKind::MidOdd => width_mid - 1 - off,
            _ => unreachable!(),
        };
        for (sid, gain_slot) in gain_buf.iter_mut().enumerate() {
            let inv = table.inv_states[sid];
            let mut gain = 0_i64;
            for r in 0..height {
                let lane = inv[r] as usize + 1;
                let w = lane_weight_base[lane] + mid_idx as i64;
                gain += w * a[top_row + r][c] as i64;
            }
            *gain_slot = gain;
        }
        active.clear();
        for (sid, &score) in dp_prev.iter().enumerate() {
            if score != INF_NEG {
                active.push(sid);
            }
        }
        sources.clear();
        if off + FULL_DISTRIBUTION_TAIL >= width_mid {
            sources.extend(active.iter().copied());
        } else {
            sources.extend(active.iter().copied());
            let keep = ((sources.len() * SOURCE_TOP_PERCENT) + 99) / 100;
            let keep = keep.max(1);
            if keep < sources.len() {
                sources.select_nth_unstable_by(keep - 1, |&lhs, &rhs| dp_prev[rhs].cmp(&dp_prev[lhs]));
                sources.truncate(keep);
            }
        }
        dp_cur.fill(INF_NEG);
        let prev_row = &mut prev[off * m..(off + 1) * m];
        for &sid in &sources {
            let cur = dp_prev[sid];
            let base = sid * mask_len;
            for mi in 0..mask_len {
                let tid = table.next_state[base + mi] as usize;
                let cand = cur + gain_buf[tid];
                if cand > dp_cur[tid] {
                    dp_cur[tid] = cand;
                    prev_row[tid] = sid as u16;
                }
            }
        }
        std::mem::swap(&mut dp_prev, &mut dp_cur);
    }
    assert!(dp_prev[end_id] != INF_NEG, "mid-band DP could not reach end state");
    let mut cur = end_id as u16;
    let mut chosen = vec![0_u16; width_mid];
    for off in (0..width_mid).rev() {
        chosen[off] = cur;
        cur = prev[off * m + cur as usize];
    }
    assert_eq!(cur, start_id as u16);
    for off in 0..width_mid {
        let c = mid_l + off;
        let inv = table.inv_states[chosen[off] as usize];
        for r in 0..height {
            labels[r][c] = inv[r] + 1;
        }
    }
    Band {
        kind,
        top_row,
        left_w,
        right_w,
        labels,
    }
}

fn build_segment_ids() -> (Vec<Vec<usize>>, Vec<usize>, Vec<usize>) {
    let mut mids = vec![vec![usize::MAX; 9]; 24];
    let mut top = vec![usize::MAX; 5];
    let mut bottom = vec![usize::MAX; 5];
    let mut id = 0_usize;
    for g in 1..=4 {
        for i in 0..24 {
            let lane = if i % 2 == 0 { g } else { g + 4 };
            mids[i][lane] = id;
            id += 1;
        }
        bottom[g] = id;
        id += 1;
        for i in (0..24).rev() {
            let lane = if i % 2 == 0 { g + 4 } else { g };
            mids[i][lane] = id;
            id += 1;
        }
        top[g] = id;
        id += 1;
    }
    (mids, top, bottom)
}

fn build_prefix_lengths() -> (Vec<Vec<usize>>, Vec<usize>, Vec<usize>) {
    let mut mids = vec![vec![0_usize; 9]; 24];
    for i in 0..24 {
        if i % 2 == 0 {
            for lane in 1_u8..=4 {
                mids[i][lane as usize] = path_mid_left(lane, true).len();
                mids[i][lane as usize + 4] = path_mid_left(lane + 4, false).len();
            }
        } else {
            for lane in 1_u8..=4 {
                mids[i][lane as usize] = path_mid_right(lane, false).len();
                mids[i][lane as usize + 4] = path_mid_right(lane + 4, true).len();
            }
        }
    }
    let mut top = vec![0_usize; 5];
    let mut bottom = vec![0_usize; 5];
    for lane in 1_u8..=4 {
        top[lane as usize] = path_top_right(lane).len();
        bottom[lane as usize] = path_bot_left(lane).len();
    }
    (mids, top, bottom)
}

fn optimize_all(a: &[Vec<usize>], timer: &Instant) -> (Band, Vec<Band>, Band) {
    log_progress(timer, "building 4-lane state table");
    let table4 = build_state_table::<4>();
    log_progress(timer, "building 8-lane state table");
    let table8 = build_state_table::<8>();
    log_progress(timer, "building segment ids");
    let (mid_seg_ids, top_seg_ids, bottom_seg_ids) = build_segment_ids();
    log_progress(timer, "building prefix lengths");
    let (mid_prefix_lens, top_prefix_lens, bottom_prefix_lens) = build_prefix_lengths();
    log_progress(timer, "optimizing top band");
    let top = optimize_band_4(a, 0, BandKind::Top, &top_seg_ids, &top_prefix_lens, &table4);
    let mut mids = Vec::new();
    for i in 0..24 {
        let kind = if i % 2 == 0 { BandKind::MidEven } else { BandKind::MidOdd };
        log_progress(timer, &format!("optimizing middle band {}/24", i + 1));
        mids.push(optimize_band_8(
            a,
            4 + 8 * i,
            kind,
            &mid_seg_ids[i],
            &mid_prefix_lens[i],
            &table8,
        ));
    }
    log_progress(timer, "optimizing bottom band");
    let bottom = optimize_band_4(
        a,
        a.len() - 4,
        BandKind::Bottom,
        &bottom_seg_ids,
        &bottom_prefix_lens,
        &table4,
    );
    log_progress(timer, "finished band optimization");
    (top, mids, bottom)
}

fn neighbors(cells: &[(usize, usize)], p: (usize, usize)) -> Vec<(usize, usize)> {
    cells
        .iter()
        .copied()
        .filter(|&q| q != p && p.0.abs_diff(q.0).max(p.1.abs_diff(q.1)) == 1)
        .collect()
}

fn small_fragment_path(mat: &[Vec<u8>], lane: u8, start: (usize, usize), end: (usize, usize)) -> Vec<(usize, usize)> {
    let cells = mat
        .iter()
        .enumerate()
        .flat_map(|(r, row)| {
            row.iter()
                .enumerate()
                .filter(move |&(_, &v)| v == lane)
                .map(move |(c, _)| (r, c))
        })
        .collect::<Vec<_>>();
    let mut id = HashMap::new();
    for (i, &p) in cells.iter().enumerate() {
        id.insert(p, i);
    }
    let adj = cells.iter().copied().map(|p| neighbors(&cells, p)).collect::<Vec<_>>();
    fn dfs(
        cur: usize,
        end: usize,
        mask: u32,
        full: u32,
        cells: &[(usize, usize)],
        id: &HashMap<(usize, usize), usize>,
        adj: &[Vec<(usize, usize)>],
        path: &mut Vec<(usize, usize)>,
    ) -> bool {
        if mask == full {
            return cur == end;
        }
        let mut nexts = adj[cur]
            .iter()
            .filter_map(|q| {
                let qi = *id.get(q).unwrap();
                if (mask >> qi) & 1 == 0 {
                    Some((adj[qi]
                        .iter()
                        .filter(|&&z| {
                            let zi = *id.get(&z).unwrap();
                            (mask >> zi) & 1 == 0
                        })
                        .count(), qi))
                } else {
                    None
                }
            })
            .collect::<Vec<_>>();
        nexts.sort_unstable();
        for (_, qi) in nexts {
            path.push(cells[qi]);
            if dfs(qi, end, mask | (1 << qi), full, cells, id, adj, path) {
                return true;
            }
            path.pop();
        }
        false
    }
    let s = *id.get(&start).unwrap();
    let t = *id.get(&end).unwrap();
    let mut path = vec![start];
    let ok = dfs(s, t, 1 << s, (1_u32 << cells.len()) - 1, &cells, &id, &adj, &mut path);
    assert!(ok, "fragment path not found for lane {}", lane);
    path
}

fn path_mid_left(lane: u8, top_entry: bool) -> Vec<(usize, usize)> {
    let mat = MID_LEFT.map(|x| x.to_vec()).to_vec();
    let start = if lane <= 4 {
        if top_entry { (0, (lane - 1) as usize) } else { (7, (lane - 5) as usize) }
    } else {
        if top_entry { unreachable!() } else { (7, (lane - 5) as usize) }
    };
    let end_row = boundary_state_left(&MID_LEFT)[(lane - 1) as usize] as usize;
    small_fragment_path(&mat, lane, start, (end_row, 3))
}

fn path_mid_left_to_ext(lane: u8, top_exit: bool) -> Vec<(usize, usize)> {
    let mat = MID_LEFT.map(|x| x.to_vec()).to_vec();
    let start_row = boundary_state_left(&MID_LEFT)[(lane - 1) as usize] as usize;
    let end = if lane <= 4 {
        if top_exit { (0, (lane - 1) as usize) } else { unreachable!() }
    } else {
        if top_exit { unreachable!() } else { (7, (lane - 5) as usize) }
    };
    small_fragment_path(&mat, lane, (start_row, 3), end)
}

fn path_mid_right(lane: u8, top_entry: bool) -> Vec<(usize, usize)> {
    let mat = MID_RIGHT.map(|x| x.to_vec()).to_vec();
    let start = if lane <= 4 {
        if top_entry { unreachable!() } else { (7, (4 - lane) as usize) }
    } else if top_entry {
        (0, (8 - lane) as usize)
    } else {
        unreachable!()
    };
    let end_row = boundary_state_right(&MID_RIGHT)[(lane - 1) as usize] as usize;
    small_fragment_path(&mat, lane, start, (end_row, 0))
}

fn path_mid_right_to_ext(lane: u8, top_exit: bool) -> Vec<(usize, usize)> {
    let mat = MID_RIGHT.map(|x| x.to_vec()).to_vec();
    let start_row = boundary_state_right(&MID_RIGHT)[(lane - 1) as usize] as usize;
    let end = if lane <= 4 {
        if top_exit { unreachable!() } else { (7, (4 - lane) as usize) }
    } else if top_exit {
        (0, (8 - lane) as usize)
    } else {
        unreachable!()
    };
    small_fragment_path(&mat, lane, (start_row, 0), end)
}

fn path_bot_left(lane: u8) -> Vec<(usize, usize)> {
    let mat = BOT_LEFT.map(|x| x.to_vec()).to_vec();
    let end_row = boundary_state_left(&BOT_LEFT)[(lane - 1) as usize] as usize;
    small_fragment_path(&mat, lane, (0, (lane - 1) as usize), (end_row, 3))
}

fn path_bot_right(lane: u8) -> Vec<(usize, usize)> {
    let mat = BOT_RIGHT.map(|x| x.to_vec()).to_vec();
    let start_row = boundary_state_right(&BOT_RIGHT)[(lane - 1) as usize] as usize;
    small_fragment_path(&mat, lane, (start_row, 0), (0, (4 - lane) as usize))
}

fn path_top_right(lane: u8) -> Vec<(usize, usize)> {
    let mat = TOP_RIGHT.map(|x| x.to_vec()).to_vec();
    let end_row = boundary_state_right(&TOP_RIGHT)[(lane - 1) as usize] as usize;
    small_fragment_path(&mat, lane, (3, (4 - lane) as usize), (end_row, 0))
}

fn path_top_left(lane: u8) -> Vec<(usize, usize)> {
    let mat = TOP_LEFT.map(|x| x.to_vec()).to_vec();
    let start_row = boundary_state_left(&TOP_LEFT)[(lane - 1) as usize] as usize;
    small_fragment_path(&mat, lane, (start_row, 4), (3, lane as usize))
}

fn middle_rows(labels: &[Vec<u8>], lane: u8, l: usize, r: usize) -> Vec<usize> {
    (l..r)
        .map(|c| {
            labels
                .iter()
                .enumerate()
                .find(|(_, row)| row[c] == lane)
                .map(|(r, _)| r)
                .unwrap()
        })
        .collect()
}

fn shift_cols(path: &mut [(usize, usize)], delta: usize) {
    for p in path {
        p.1 += delta;
    }
}

fn band_pass_path(band: &Band, lane: u8) -> Vec<(usize, usize)> {
    let n = band.labels[0].len();
    let mid_l = band.left_w;
    let mid_r = n - band.right_w;
    let rows = middle_rows(&band.labels, lane, mid_l, mid_r);
    let local = match band.kind {
        BandKind::MidEven => {
            if lane <= 4 {
                let mut p = path_mid_left(lane, true);
                for (i, &r) in rows.iter().enumerate() {
                    p.push((r, mid_l + i));
                }
                let mut q = path_mid_right_to_ext(lane, false);
                shift_cols(&mut q, n - band.right_w);
                if p.last() == Some(&q[0]) {
                    q.remove(0);
                }
                p.extend(q);
                p
            } else {
                let mut p = path_mid_left(lane, false);
                for (i, &r) in rows.iter().enumerate() {
                    p.push((r, mid_l + i));
                }
                let mut q = path_mid_right_to_ext(lane, true);
                shift_cols(&mut q, n - band.right_w);
                if p.last() == Some(&q[0]) {
                    q.remove(0);
                }
                p.extend(q);
                p
            }
        }
        BandKind::MidOdd => {
            if lane <= 4 {
                let mut p = path_mid_right(lane, false);
                shift_cols(&mut p, n - band.right_w);
                for (i, &r) in rows.iter().enumerate().rev() {
                    p.push((r, mid_l + i));
                }
                let mut q = path_mid_left_to_ext(lane, true);
                if p.last() == Some(&q[0]) {
                    q.remove(0);
                }
                p.extend(q);
                p
            } else {
                let mut p = path_mid_right(lane, true);
                shift_cols(&mut p, n - band.right_w);
                for (i, &r) in rows.iter().enumerate().rev() {
                    p.push((r, mid_l + i));
                }
                let mut q = path_mid_left_to_ext(lane, false);
                if p.last() == Some(&q[0]) {
                    q.remove(0);
                }
                p.extend(q);
                p
            }
        }
        BandKind::Bottom => {
            let mut p = path_bot_left(lane);
            for (i, &r) in rows.iter().enumerate() {
                p.push((r, mid_l + i));
            }
            let mut q = path_bot_right(lane);
            shift_cols(&mut q, n - band.right_w);
            if p.last() == Some(&q[0]) {
                q.remove(0);
            }
            p.extend(q);
            p
        }
        BandKind::Top => {
            let mut p = path_top_right(lane);
            shift_cols(&mut p, n - band.right_w);
            for (i, &r) in rows.iter().enumerate().rev() {
                p.push((r, mid_l + i));
            }
            let mut q = path_top_left(lane);
            if p.last() == Some(&q[0]) {
                q.remove(0);
            }
            p.extend(q);
            p
        }
    };
    local
        .into_iter()
        .map(|(r, c)| (band.top_row + r, c))
        .collect()
}

fn append_segment(path: &mut Vec<(usize, usize)>, mut seg: Vec<(usize, usize)>) {
    if path.is_empty() {
        path.extend(seg);
        return;
    }
    if path.last() == Some(&seg[0]) {
        seg.remove(0);
    }
    if seg.is_empty() {
        return;
    }
    let a = *path.last().unwrap();
    let b = seg[0];
    assert!(a.0.abs_diff(b.0).max(a.1.abs_diff(b.1)) == 1, "{:?} -> {:?}", a, b);
    path.extend(seg);
}

fn validate_segment(seg: &[(usize, usize)], name: &str) {
    let mut seen = HashMap::new();
    for (i, &p) in seg.iter().enumerate() {
        if let Some(prev) = seen.insert(p, i) {
            panic!("segment {} dup {:?} at {} prev {}", name, p, i, prev);
        }
    }
}

fn build_full_path(top: &Band, mids: &[Band], bottom: &Band) -> Vec<(usize, usize)> {
    let mut path = Vec::new();
    for g in 1_u8..=4 {
        for (i, band) in mids.iter().enumerate() {
            let lane = if i % 2 == 0 { g } else { g + 4 };
            let seg = band_pass_path(band, lane);
            validate_segment(&seg, &format!("mid-down-{}-{}", i, lane));
            append_segment(&mut path, seg);
        }
        let seg = band_pass_path(bottom, g);
        validate_segment(&seg, &format!("bottom-{}", g));
        append_segment(&mut path, seg);
        for i in (0..mids.len()).rev() {
            let lane = if i % 2 == 0 { g + 4 } else { g };
            let seg = band_pass_path(&mids[i], lane);
            validate_segment(&seg, &format!("mid-up-{}-{}", i, lane));
            append_segment(&mut path, seg);
        }
        let seg = band_pass_path(top, g);
        validate_segment(&seg, &format!("top-{}", g));
        append_segment(&mut path, seg);
    }
    path
}

fn validate(path: &[(usize, usize)], n: usize) {
    assert_eq!(path.len(), n * n, "len {} != {}", path.len(), n * n);
    let mut used = vec![usize::MAX; n * n];
    for (i, &(r, c)) in path.iter().enumerate() {
        assert!(r < n && c < n, "out {} {} {}", i, r, c);
        let idx = r * n + c;
        assert!(used[idx] == usize::MAX, "dup {} {} {} prev {}", i, r, c, used[idx]);
        used[idx] = i;
    }
    for i in 0..path.len() - 1 {
        let a = path[i];
        let b = path[i + 1];
        assert!(a.0.abs_diff(b.0).max(a.1.abs_diff(b.1)) == 1, "adj {} {:?} {:?}", i, a, b);
    }
}

fn main() {
    let timer = Instant::now();
    let mut input = String::new();
    io::stdin().read_to_string(&mut input).unwrap();
    let mut it = input.split_whitespace();
    let n = it.next().unwrap().parse::<usize>().unwrap();
    let mut a = vec![vec![0_usize; n]; n];
    for r in 0..n {
        for c in 0..n {
            a[r][c] = it.next().unwrap().parse::<usize>().unwrap();
        }
    }
    assert_eq!(n, 200);
    log_progress(&timer, "input loaded");
    let (top, mids, bottom) = optimize_all(&a, &timer);
    log_progress(&timer, "building full path");
    let path = build_full_path(&top, &mids, &bottom);
    log_progress(&timer, "validating path");
    validate(&path, n);
    log_progress(&timer, "writing output");
    let mut out = io::BufWriter::new(io::stdout().lock());
    for (r, c) in path {
        writeln!(out, "{} {}", r, c).unwrap();
    }
    log_progress(&timer, "done");
}
