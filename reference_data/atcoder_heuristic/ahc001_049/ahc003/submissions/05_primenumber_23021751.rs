use lazy_static::lazy_static;
use std::cmp::Ordering;
use std::collections::BinaryHeap;
use std::fmt;
use std::io::{stdin, stdout, BufWriter, Write};

const SIZE: usize = 30;
const V: usize = SIZE * SIZE;
const E: usize = (SIZE - 1) * SIZE + SIZE * (SIZE - 1);
const QUERIES: usize = 1000;
//const COST_LO: f64 = 1000.0;
//const COST_HI: f64 = 9000.0;

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
enum LogLevel {
    Critical,
    Error,
    Warning,
    Info,
    Debug,
    Debug2,
}

lazy_static! {
    static ref ARGV: Vec<String> = std::env::args().collect();
    static ref LOG_LEVEL: LogLevel = {
        match ARGV.iter().nth(1) {
            Some(s) => {
                if s == "-v" {
                    LogLevel::Info
                } else if s == "-d" {
                    LogLevel::Debug
                } else if s == "-D" {
                    LogLevel::Debug2
                } else if s == "-w" {
                    LogLevel::Warning
                } else if s == "-q" {
                    LogLevel::Critical
                } else {
                    panic!()
                }
            }
            None => LogLevel::Error,
        }
    };
    static ref L2_WEIGHT: f64 = {
        match ARGV.iter().nth(2) {
            Some(v) => v.parse().unwrap(),
            None => 0.0003847572516344378,
        }
    };
    static ref NEIGHBOR_WEIGHT: f64 = {
        match ARGV.iter().nth(3) {
            Some(v) => v.parse().unwrap(),
            None => 0.5114617766767865,
        }
    };
    static ref CUT_SIDE_BY_SIDE_TIME: usize = {
        match ARGV.iter().nth(4) {
            Some(v) => v.parse().unwrap(),
            None => 72,
        }
    };
    static ref CUT_M_THRESHOLD: f64 = {
        match ARGV.iter().nth(5) {
            Some(v) => v.parse().unwrap(),
            None => 793319.0598056563,
        }
    };
    static ref SCALE_NEIGHBOR_TIME: usize = {
        match ARGV.iter().nth(6) {
            Some(v) => v.parse().unwrap(),
            None => 260,
        }
    };
    static ref NEIGHBOR_WEIGHT_SHIFTER: f64 = {
        match ARGV.iter().nth(7) {
            Some(v) => v.parse().unwrap(),
            None => -13.99721273046934,
        }
    };
    static ref NEIGHBOR_WEIGHT_SCALER: f64 = {
        match ARGV.iter().nth(8) {
            Some(v) => v.parse().unwrap(),
            None => 0.9399884685563552,
        }
    };
    static ref DEFAULT_WEIGHT_MAX: f64 = {
        match ARGV.iter().nth(9) {
            Some(v) => v.parse().unwrap(),
            None => 3193.08,
        }
    };
    static ref DEFAULT_WEIGHT_SCALER: f64 = {
        match ARGV.iter().nth(10) {
            Some(v) => v.parse().unwrap(),
            None => 0.021377,
        }
    };
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Pos {
    r: isize,
    c: isize,
}

fn diff(p0: Pos, p1: Pos) -> (isize, isize) {
    (p1.r - p0.r, p1.c - p0.c)
}

impl Pos {
    fn to_id(&self) -> usize {
        (self.r * (SIZE as isize) + self.c) as usize
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum Dir {
    Right,
    Down,
    Left,
    Up,
}

impl fmt::Display for Dir {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let s = match self {
            Dir::Right => "R",
            Dir::Down => "D",
            Dir::Left => "L",
            Dir::Up => "U",
        };
        write!(f, "{}", s)
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
struct Edge {
    src: Pos,
    dest: Pos,
}

impl Edge {
    fn to_dir(&self) -> Dir {
        let (dr, dc) = diff(self.src, self.dest);
        if dr == 0 {
            if dc == 1 {
                Dir::Right
            } else if dc == -1 {
                Dir::Left
            } else {
                panic!();
            }
        } else {
            if dc != 0 {
                panic!();
            } else {
                if dr == 1 {
                    Dir::Down
                } else if dr == -1 {
                    Dir::Up
                } else {
                    panic!();
                }
            }
        }
    }

    fn to_id_impl(p: Pos, dir: Dir) -> usize {
        (match dir {
            Dir::Right => p.r * (SIZE as isize - 1) + p.c,
            Dir::Down => p.r + p.c * (SIZE as isize - 1) + ((SIZE * (SIZE - 1)) as isize),
            _ => panic!(),
        }) as usize
    }

    fn to_id(&self) -> usize {
        match self.to_dir() {
            Dir::Left => Self::to_id_impl(self.dest, Dir::Right),
            Dir::Up => Self::to_id_impl(self.dest, Dir::Down),
            Dir::Right => Self::to_id_impl(self.src, Dir::Right),
            Dir::Down => Self::to_id_impl(self.src, Dir::Down),
        }
    }
}

#[derive(Debug, Clone, PartialEq)]
struct Equation {
    edges: Vec<Edge>,
    cost: f64,
}

#[derive(Debug, Copy, Clone, PartialEq)]
struct EdgeWithCost {
    src: Pos,
    dest: Pos,
    cost: f64,
}

#[derive(Debug, Clone)]
struct Graph {
    edge_list: Vec<Vec<EdgeWithCost>>,
}

#[derive(Debug, Clone)]
struct SpM {
    weight: Vec<f64>,
    col_size: usize,
    row_starts: Vec<usize>,
    cols: Vec<usize>,
}

impl SpM {
    fn row_size(&self) -> usize {
        self.row_starts.len() - 1
    }
}

#[target_feature(enable = "avx2")]
unsafe fn spmv(spm: &SpM, a: &[f64], b: &mut [f64]) {
    for row in 0..spm.row_size() {
        let row_start = spm.row_starts[row];
        let row_end = spm.row_starts[row + 1];
        let mut ans = 0.;
        for (col, w) in spm.cols[row_start..row_end]
            .iter()
            .zip(&spm.weight[row_start..row_end])
        {
            ans += w * a.get_unchecked(*col);
        }
        b[row] = ans;
    }
}

#[target_feature(enable = "avx2")]
unsafe fn spmv_trans(spm: &SpM, b: &[f64], a: &mut [f64]) {
    for e in a.iter_mut() {
        *e = 0.;
    }
    for row in 0..spm.row_size() {
        let row_start = spm.row_starts[row];
        let row_end = spm.row_starts[row + 1];
        let val = b[row];
        for (col, w) in spm.cols[row_start..row_end]
            .iter()
            .zip(&spm.weight[row_start..row_end])
        {
            *a.get_unchecked_mut(*col) += w * val;
        }
    }
}

#[allow(dead_code)]
fn transpose(spm: &SpM) -> SpM {
    let expected = spm.cols.len() / spm.col_size;
    let mut w_transposed = vec![Vec::with_capacity(expected); spm.col_size];
    let mut col_transposed = vec![Vec::with_capacity(expected); spm.col_size];
    for row in 0..spm.row_size() {
        let row_start = spm.row_starts[row];
        let row_end = spm.row_starts[row + 1];
        for (col, w) in spm.cols[row_start..row_end]
            .iter()
            .zip(&spm.weight[row_start..row_end])
        {
            w_transposed[*col].push(*w);
            col_transposed[*col].push(row);
        }
    }
    let weight = w_transposed.concat();
    let cols = col_transposed.concat();
    let mut row_starts = Vec::new();
    let mut offset = 0;
    for col_row in col_transposed {
        row_starts.push(offset);
        offset += col_row.len();
    }
    SpM {
        weight,
        col_size: spm.row_size(),
        row_starts,
        cols,
    }
}

fn sub(x: &[f64], y: &[f64], z: &mut [f64]) {
    if x.len() != y.len() {
        panic!();
    }
    for idx in 0..x.len() {
        z[idx] = x[idx] - y[idx];
    }
}

#[allow(dead_code)]
fn scale(a: f64, x: &[f64], y: &mut [f64]) {
    for idx in 0..x.len() {
        y[idx] = a * x[idx];
    }
}

#[allow(dead_code)]
fn dot(x: &[f64], y: &[f64]) -> f64 {
    let mut ans = 0.;
    for (a, b) in x.iter().zip(y) {
        ans += a * b;
    }
    ans
}

fn norm(x: &[f64]) -> f64 {
    let mut ans = 0.;
    for a in x {
        ans += a * a;
    }
    ans
}

fn clamp_vec(x: &mut [f64], min: f64, max: f64) {
    for a in x {
        *a = a.max(min).min(max);
    }
}

fn calc_default_weight(temp: f64) -> f64 {
    (*DEFAULT_WEIGHT_MAX / (1.0 + temp.ln() * *DEFAULT_WEIGHT_SCALER)).max(1000.)
}

#[allow(dead_code)]
fn gradient_descent(spm: &SpM, a: &mut [f64], b: &[f64], iter_num: usize) {
    let mut ta = vec![0.; spm.col_size];
    let mut c = vec![0.; spm.row_size()];
    let mut d = vec![0.; spm.row_size()];
    let mut sd = vec![0.; spm.row_size()];
    let alpha = 0.0005;
    for _i in 0..iter_num {
        unsafe {
            spmv(spm, a, &mut c);
        }
        sub(&b, &c, &mut d);
        if *LOG_LEVEL >= LogLevel::Debug {
            eprintln!("GD Diff: {}", norm(&d));
        }
        scale(alpha, &d, &mut sd);
        unsafe {
            spmv_trans(spm, &sd, &mut ta);
        }
        for idx in 0..spm.col_size {
            a[idx] += ta[idx];
        }
        clamp_vec(a, 1000., 9000.);
    }
}

fn cgls(spm: &SpM, a: &mut [f64], b: &[f64], iter_num: usize) {
    let mut pa = vec![0.; spm.row_size()];
    unsafe {
        spmv(&spm, &a, &mut pa);
    }
    let mut r = vec![0.; spm.row_size()];
    sub(&b, &pa, &mut r);
    let mut p = vec![0.; spm.col_size];
    unsafe {
        spmv_trans(&spm, &r, &mut p);
    }
    let mut s = p.clone();
    let mut old_s_norm = norm(&s);
    let mut q = vec![0.; spm.row_size()];
    for _i in 0..iter_num {
        unsafe {
            spmv(&spm, &p, &mut q);
        }
        let alpha = old_s_norm / norm(&q);
        for idx in 0..spm.col_size {
            a[idx] += alpha * p[idx];
        }
        for idx in 0..spm.row_size() {
            r[idx] -= alpha * q[idx];
        }
        unsafe {
            spmv_trans(&spm, &r, &mut s);
        }
        let new_s_norm = norm(&s);
        if *LOG_LEVEL >= LogLevel::Debug {
            let p_norm = norm(&p);
            let q_norm = norm(&q);
            eprintln!(
                "CGLS S norm: {}, alpha: {}, |p|: {}, |q|: {}",
                new_s_norm, alpha, p_norm, q_norm
            );
        }
        if new_s_norm < 1.0 {
            break;
        }
        let beta = new_s_norm / old_s_norm;
        for idx in 0..spm.col_size {
            p[idx] = s[idx] + beta * p[idx];
        }
        old_s_norm = new_s_norm;
    }
}

fn estimate_cost(qid: usize, spm: &SpM, a: &mut [f64], b: &[f64], temp: f64) -> Graph {
    if qid > 0 {
        let iter_num = if temp > 5.0 {
            10
        } else if temp > 2.0 {
            18
        } else {
            25
        };
        cgls(spm, a, b, iter_num)
    } else {
        let w = calc_default_weight(temp);
        for e in a.iter_mut() {
            *e = w;
        }
    };
    clamp_vec(a, 1000.0, 9000.0);
    let mut edge_list = Vec::with_capacity(V);
    for row in 0..SIZE as isize {
        for col in 0..SIZE as isize {
            let v0 = Pos { r: row, c: col };
            let mut edges = Vec::with_capacity(4);
            let d = [0, 1, 0, -1, 0];
            for k in 0..4 {
                let nr = row + d[k];
                let nc = col + d[k + 1];
                if nr < 0 || nc < 0 || nr >= SIZE as isize || nc >= SIZE as isize {
                    continue;
                }
                let v1 = Pos { r: nr, c: nc };
                edges.push(EdgeWithCost {
                    src: v0,
                    dest: v1,
                    cost: a[Edge { src: v0, dest: v1 }.to_id()],
                });
            }
            edge_list.push(edges);
        }
    }
    Graph { edge_list }
}

#[derive(Debug, Copy, Clone, PartialEq)]
struct State {
    cost: f64,
    pos: Pos,
}

impl Eq for State {}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        self.partial_cmp(other).unwrap()
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        other.cost.partial_cmp(&self.cost)
    }
}

fn dijkstra(s: Pos, t: Pos, g: &Graph) -> Vec<EdgeWithCost> {
    let mut dist: Vec<_> = (0..V).map(|_| 1e18).collect();
    let mut parent: Vec<Option<EdgeWithCost>> = (0..V).map(|_| None).collect();
    let mut heap = BinaryHeap::new();
    dist[s.to_id()] = 0.;
    heap.push(State { cost: 0., pos: s });
    while let Some(State { cost, pos }) = heap.pop() {
        if pos == t {
            break;
        }
        let i = pos.to_id();
        if cost > dist[i] {
            continue;
        }
        for e in &g.edge_list[i] {
            let next = State {
                cost: cost + e.cost,
                pos: e.dest,
            };
            let j = next.pos.to_id();
            if next.cost < dist[j] {
                dist[j] = next.cost;
                parent[j] = Some(*e);
                heap.push(next);
            }
        }
    }
    let mut edges = Vec::new();
    let mut pos = t;
    while let Some(e) = parent[pos.to_id()] {
        edges.push(e);
        pos = e.src;
    }
    edges.reverse();
    edges
}

fn solve(qid: usize, s: Pos, t: Pos, spm: &SpM, a: &mut [f64], b: &[f64], temp: f64) -> Vec<Edge> {
    let g = estimate_cost(qid, spm, a, b, temp);
    let edges = dijkstra(s, t, &g);
    edges
        .iter()
        .map(|e| Edge {
            src: e.src,
            dest: e.dest,
        })
        .collect()
}

fn respond(edges: &[Edge]) -> f64 {
    let out = stdout();
    {
        let mut out = BufWriter::new(out.lock());
        for e in edges {
            write!(out, "{}", e.to_dir()).unwrap();
        }
        writeln!(out, "").unwrap();
    }
    let mut buf = String::new();
    stdin().read_line(&mut buf).unwrap();
    buf.split_whitespace().next().unwrap().parse().unwrap()
}

fn get_query() -> (Pos, Pos) {
    let mut buf = String::new();
    stdin().read_line(&mut buf).unwrap();
    let mut nums = buf.split_whitespace();
    let rs: isize = nums.next().unwrap().parse().unwrap();
    let cs: isize = nums.next().unwrap().parse().unwrap();
    let rt: isize = nums.next().unwrap().parse().unwrap();
    let ct: isize = nums.next().unwrap().parse().unwrap();
    let s = Pos { r: rs, c: cs };
    let t = Pos { r: rt, c: ct };
    (s, t)
}

#[derive(Debug, Clone)]
struct NEProblem {
    spm: SpM,
    b: Vec<f64>,
    avg_norm_rows: usize,
    neighbor_edge_norm_h_rows: usize,
    neighbor_edge_norm_v_rows: usize,
}

fn construct_spm() -> NEProblem {
    let mut w = Vec::new();
    let mut cols = Vec::new();
    let mut row_starts = Vec::new();
    let mut b = Vec::new();
    row_starts.push(cols.len());
    // average normalization
    b.push(E as f64 * 5000. * *L2_WEIGHT);
    for i in 0..E {
        w.push(*L2_WEIGHT);
        cols.push(i);
    }
    row_starts.push(cols.len());
    // neighbor edge normalization
    // horizontal
    for r in 0..SIZE {
        for c in 0..(SIZE - 1) {
            b.push(0.);
            let id1 = r * (SIZE - 1) + c;
            let id2 = r * (SIZE - 1) + (c + 1) % (SIZE - 1);
            w.push(*NEIGHBOR_WEIGHT);
            cols.push(id1);
            w.push(-*NEIGHBOR_WEIGHT);
            cols.push(id2);
            row_starts.push(cols.len());
        }
    }
    // vertical
    for c in 0..SIZE {
        for r in 0..(SIZE - 1) {
            b.push(0.);
            let id1 = r + c * (SIZE - 1) + (SIZE - 1) * SIZE;
            let id2 = (r + 1) % (SIZE - 1) + c * (SIZE - 1) + (SIZE - 1) * SIZE;
            w.push(*NEIGHBOR_WEIGHT);
            cols.push(id1);
            w.push(-*NEIGHBOR_WEIGHT);
            cols.push(id2);
            row_starts.push(cols.len());
        }
    }
    NEProblem {
        spm: SpM {
            weight: w,
            col_size: E,
            row_starts,
            cols,
        },
        b,
        avg_norm_rows: 1,
        neighbor_edge_norm_h_rows: SIZE * (SIZE - 1),
        neighbor_edge_norm_v_rows: (SIZE - 1) * SIZE,
    }
}

fn update_spm(spm: &mut SpM, edges: &[Edge]) {
    let mut ids = Vec::new();
    for e in edges {
        ids.push(e.to_id());
    }
    ids.sort_unstable();
    for id in ids {
        spm.weight.push(1.0 / edges.len() as f64);
        spm.cols.push(id);
    }
    spm.row_starts.push(spm.cols.len());
}

fn scale_row(spm: &mut SpM, row: usize, coeff: f64) {
    let from = spm.row_starts[row];
    let to = spm.row_starts[row + 1];
    for w in &mut spm.weight[from..to] {
        *w *= coeff;
    }
}

// cut side-by-side penalty when m is probably 2
fn cut_when_m_is_2(ne_prob: &mut NEProblem, a: &[f64]) {
    let spm = &mut ne_prob.spm;
    let b = &ne_prob.b;
    let mut ma = vec![0.; b.len()];
    unsafe {
        spmv(spm, a, &mut ma);
    }
    let mut d = vec![0.; b.len()];
    sub(b, &ma, &mut d);
    let mut sum = 0.;
    let from = ne_prob.avg_norm_rows;
    let to = from + ne_prob.neighbor_edge_norm_h_rows + ne_prob.neighbor_edge_norm_v_rows;
    for idx in from..to {
        sum += d[idx].abs().powi(2);
    }
    if *LOG_LEVEL >= LogLevel::Warning {
        eprintln!("whole: {}, neighbor: {}", norm(&d), sum);
    }
    if sum > *CUT_M_THRESHOLD * *NEIGHBOR_WEIGHT {
        for r in 0..SIZE {
            let c = SIZE - 2;
            let row_idx = ne_prob.avg_norm_rows + r * (SIZE - 1) + c;
            scale_row(spm, row_idx, 0.0);
        }
        for r in 0..SIZE {
            for c in 13..17 {
                let row_idx = ne_prob.avg_norm_rows + r * (SIZE - 1) + c;
                scale_row(spm, row_idx, 0.7);
            }
        }
        for c in 0..SIZE {
            let r = SIZE - 2;
            let row_idx =
                ne_prob.avg_norm_rows + ne_prob.neighbor_edge_norm_h_rows + r + c * (SIZE - 1);
            scale_row(spm, row_idx, 0.0);
        }
        for c in 0..SIZE {
            for r in 13..17 {
                let row_idx =
                    ne_prob.avg_norm_rows + ne_prob.neighbor_edge_norm_h_rows + r + c * (SIZE - 1);
                scale_row(spm, row_idx, 0.7);
            }
        }
    }
}

fn scale_neighbor_weight(ne_prob: &mut NEProblem, a: &[f64]) {
    let spm = &mut ne_prob.spm;
    let b = &ne_prob.b;
    let mut ma = vec![0.; b.len()];
    unsafe {
        spmv(spm, a, &mut ma);
    }
    let mut d = vec![0.; b.len()];
    sub(b, &ma, &mut d);
    let mut sum = 0.;
    let from = ne_prob.avg_norm_rows;
    let to = from + ne_prob.neighbor_edge_norm_h_rows + ne_prob.neighbor_edge_norm_v_rows;
    for idx in from..to {
        sum += d[idx].abs().powi(2);
    }
    if *LOG_LEVEL >= LogLevel::Warning {
        eprintln!("SUM:{}", sum);
    }
    let normed = (sum.ln() + *NEIGHBOR_WEIGHT_SHIFTER) * *NEIGHBOR_WEIGHT_SCALER;
    let coeff = 2.0f64.powf(-normed);
    {
        let elem_from = spm.row_starts[from];
        let elem_to = spm.row_starts[to];
        for idx in elem_from..elem_to {
            spm.weight[idx] *= coeff;
        }
    }
}

fn optimize(ne_prob: &NEProblem, a: &[f64]) -> f64 {
    let mut new_a: Vec<_> = a.iter().cloned().collect();
    cgls(&ne_prob.spm, &mut new_a, &ne_prob.b, 1);
    let mut ma = vec![0.; ne_prob.b.len()];
    unsafe {
        spmv(&ne_prob.spm, &new_a, &mut ma);
    }
    let mut d = vec![0.; ne_prob.b.len()];
    sub(&ne_prob.b, &ma, &mut d);
    norm(&d)
}

fn estimate_cut_pos(ne_prob: &mut NEProblem, a: &[f64]) {
    let mut ma = vec![0.; ne_prob.b.len()];
    unsafe {
        spmv(&ne_prob.spm, a, &mut ma);
    }
    let mut d = vec![0.; ne_prob.b.len()];
    sub(&ne_prob.b, &ma, &mut d);
    let mut sum = 0.;
    let from = ne_prob.avg_norm_rows;
    let to = from + ne_prob.neighbor_edge_norm_h_rows + ne_prob.neighbor_edge_norm_v_rows;
    for idx in from..to {
        sum += d[idx].abs().powi(2);
    }
    if *LOG_LEVEL >= LogLevel::Warning {
        eprintln!("SUM:{}", sum);
    }
    if sum < 2e6 {
        return;
    }
    let current_error = norm(&d);
    let mut cuts_h = Vec::new();
    for r in 0..SIZE {
        let offset = ne_prob.avg_norm_rows;
        for c in 0..(SIZE - 2) {
            let row_idx = offset + r * (SIZE - 1) + c;
            scale_row(&mut ne_prob.spm, row_idx, 1.2);
        }
        let mut min_error = current_error;
        let mut min_col = None;
        for c in 0..(SIZE - 2) {
            let row_idx = offset + r * (SIZE - 1) + c;
            scale_row(&mut ne_prob.spm, row_idx, 0.4);
            let new_error = optimize(ne_prob, a);
            scale_row(&mut ne_prob.spm, row_idx, 2.5);
            if new_error < min_error {
                min_error = new_error;
                min_col = Some(c);
            }
        }
        for c in 0..(SIZE - 2) {
            let row_idx = offset + r * (SIZE - 1) + c;
            scale_row(&mut ne_prob.spm, row_idx, 1. / 1.2);
        }
        match min_col {
            Some(c) => cuts_h.push((r, c)),
            None => (),
        }
    }
    let mut cuts_v = Vec::new();
    for c in 0..SIZE {
        let offset = ne_prob.avg_norm_rows + ne_prob.neighbor_edge_norm_h_rows;
        for r in 0..(SIZE - 2) {
            let row_idx = offset + c * (SIZE - 1) + r;
            scale_row(&mut ne_prob.spm, row_idx, 1.2);
        }
        let mut min_error = current_error;
        let mut min_row = None;
        for r in 0..(SIZE - 2) {
            let row_idx = offset + c * (SIZE - 1) + r;
            scale_row(&mut ne_prob.spm, row_idx, 0.4);
            let new_error = optimize(ne_prob, a);
            scale_row(&mut ne_prob.spm, row_idx, 2.5);
            if new_error < min_error {
                min_error = new_error;
                min_row = Some(r);
            }
        }
        for r in 0..(SIZE - 2) {
            let row_idx = offset + c * (SIZE - 1) + r;
            scale_row(&mut ne_prob.spm, row_idx, 1. / 1.2);
        }
        match min_row {
            Some(r) => cuts_v.push((r, c)),
            None => (),
        }
    }
    for (r, c) in cuts_h {
        let offset = ne_prob.avg_norm_rows;
        if *LOG_LEVEL >= LogLevel::Info {
            eprintln!("row {}, col: {}", r, c);
        }
        for col in 0..(SIZE - 2) {
            let row_idx = offset + r * (SIZE - 1) + col;
            scale_row(&mut ne_prob.spm, row_idx, 1.2);
        }
        let row_idx = offset + r * (SIZE - 1) + c;
        scale_row(&mut ne_prob.spm, row_idx, 0.4);
    }
    for (r, c) in cuts_v {
        let offset = ne_prob.avg_norm_rows + ne_prob.neighbor_edge_norm_h_rows;
        if *LOG_LEVEL >= LogLevel::Info {
            eprintln!("row {}, col: {}", r, c);
        }
        for row in 0..(SIZE - 2) {
            let row_idx = offset + c * (SIZE - 1) + row;
            scale_row(&mut ne_prob.spm, row_idx, 1.2);
        }
        let row_idx = offset + c * (SIZE - 1) + r;
        scale_row(&mut ne_prob.spm, row_idx, 0.4);
    }
}

fn main() {
    let mut ne_prob = construct_spm();
    let mut a = vec![0.; ne_prob.spm.col_size];
    for i in 0..QUERIES {
        if i == *CUT_SIDE_BY_SIDE_TIME {
            cut_when_m_is_2(&mut ne_prob, &a);
        }
        if i == *SCALE_NEIGHBOR_TIME {
            scale_neighbor_weight(&mut ne_prob, &a);
        }
        if i == 500 {
            estimate_cut_pos(&mut ne_prob, &a);
        }
        let temp = 0.998f64.powi(i as i32 - 1000);
        if *LOG_LEVEL >= LogLevel::Debug {
            eprintln!("Query {}, temp: {}", i, temp);
        }
        let (s, t) = get_query();
        ne_prob.b[0] = calc_default_weight(temp) * *L2_WEIGHT * E as f64;
        let path = solve(i, s, t, &ne_prob.spm, &mut a, &ne_prob.b, temp);
        let cost = respond(&path);
        ne_prob.b.push(cost / path.len() as f64);
        update_spm(&mut ne_prob.spm, &path);
    }
}
