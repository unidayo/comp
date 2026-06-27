#[cfg(target_arch = "x86_64")]
use std::arch::x86_64::*;

use std::{
    collections::{HashMap, HashSet},
    f32::consts::PI,
    process::exit,
};

use fixedbitset::FixedBitSet;
use itertools::Itertools;
use proconio::input_interactive;
use rand::{seq::SliceRandom, Rng, SeedableRng};
use rand_distr::{Distribution, WeightedAliasIndex};
use rand_pcg::{Mcg128Xsl64, Pcg64Mcg};

const INTERACTION: bool = true;
const PRINT_LOG: bool = false;

const TIME_LIMIT_SEC: f64 = 1.9;
const NUM_VERTICES: usize = 800;
const NV_BW: usize = 10;
const NUM_QUERIES: usize = 400;

const TIME_LIMIT_E: f64 = 0.7;
const INTERACT_TIME_SEC: f64 = 1.5;
const INIT_SAMPLE_R: f32 = 1.0;
const UPDATE_BATCHES: usize = 32;
const EPS_UPDATE: f32 = 1.0;

const NUM_MSTS: usize = 2;
const SCHEDULER_TYPE: SchedulerType = SchedulerType::Exp;
const GW: i64 = 1000;
const T0: f64 = 200.0;
const T1: f64 = 100.0;
const SA_MERGE: usize = 32;
const SA_SPLIT: usize = 32 + SA_MERGE;
const SA_PATIENCE: i32 = 10;

const NUM_SHUFFLES: usize = 20;
const T2: f64 = 50.0;
const T3: f64 = 10.0;

fn main() {
    get_time_sec();

    let input = read_input();
    let interact_time_sec = if input.num_groups == 1 {
        TIME_LIMIT_SEC - 0.05
    } else {
        INTERACT_TIME_SEC
    };

    let finalize_queries = if input.max_vertices_in_query == 3 {
        5
    } else {
        0
    };
    let estimation_queries = NUM_QUERIES - finalize_queries;

    let mut estimator = Estimator::new(&input);
    let mut query_time = 0.0;
    for turn in 0..estimation_queries {
        // query
        query_time -= get_time_sec();
        let vertices = estimator.choose_vertices();
        estimator.interact(vertices);
        query_time += get_time_sec();

        // sample
        let now = get_time_sec();
        let rem_time = interact_time_sec - now;
        let update_time_limit = now + TIME_LIMIT_E * rem_time / (estimation_queries - turn) as f64;
        estimator.update_sample(update_time_limit);
    }
    let distances = estimator.calc_average_distances();

    // TODO: explicitly fix isolated vertices
    let mut group_optimizer = GroupOptimizer::new(&input, distances);
    let output = if input.num_groups == 1 {
        // already valid
        group_optimizer.make_output()
    } else {
        while group_optimizer.group_cost > 0 {
            for _ in 0..SA_TIME_COUNTS {
                group_optimizer.update();
            }
        }
        let mut min_raw_cost = group_optimizer.raw_cost;
        let mut output = group_optimizer.make_output();

        group_optimizer.scheduler = AnnealingScheduler::new(SCHEDULER_TYPE, T2, T3, TIME_LIMIT_SEC);
        let sa_time_limit = TIME_LIMIT_SEC - 1e-3 * input.max_vertices_in_query as f64;
        while get_time_sec() < sa_time_limit {
            for _ in 0..SA_TIME_COUNTS {
                if !group_optimizer.update2() {
                    continue;
                }
                if !min_raw_cost.chmin(group_optimizer.raw_cost) {
                    continue;
                }
                let mut noutput = group_optimizer.make_output();
                if noutput.cost > output.cost {
                    continue;
                }
                std::mem::swap(&mut output, &mut noutput);
            }
        }

        estimator.replace_to_shorter_edge(output, &group_optimizer.distances)
    };

    let output = estimator.improve_output(output, finalize_queries);

    output.print();

    if PRINT_LOG {
        eprintln!("query time: {}", query_time);
        estimator.print_log();
        group_optimizer.scheduler.print_log();
        if !INTERACTION {
            estimator.print_true_score(&output);
        }
    }

    exit(0);
}

#[derive(Debug, Clone)]
struct Query {
    vertices: Vec<usize>,
    edges: Vec<(usize, usize)>,
}

#[derive(Debug, Clone, Copy)]
struct EdgeConstraint {
    stuv: usize, // st < uv
}

impl EdgeConstraint {
    fn new(mut s: usize, mut t: usize, mut u: usize, mut v: usize) -> Self {
        debug_assert!(s < NUM_VERTICES);
        debug_assert!(t < NUM_VERTICES);
        debug_assert!(u < NUM_VERTICES);
        debug_assert!(v < NUM_VERTICES);
        debug_assert_ne!(s, t);
        debug_assert_ne!(u, v);

        if s > t {
            std::mem::swap(&mut s, &mut t);
        }
        if u > v {
            std::mem::swap(&mut u, &mut v);
        }

        if t == u || t == v {
            std::mem::swap(&mut s, &mut t);
        }
        if v == s {
            std::mem::swap(&mut u, &mut v);
        }

        Self {
            stuv: encode4(s, t, u, v, NV_BW, NV_BW, NV_BW),
        }
    }

    fn encode(&self) -> usize {
        self.stuv
    }

    fn decode(&self) -> (usize, usize, usize, usize) {
        decode4(self.stuv, NV_BW, NV_BW, NV_BW)
    }

    fn get_geometric_constraints(&self) -> Vec<(usize, GeoConstraint)> {
        let (s, t, u, v) = self.decode();
        let mut constraints = vec![];
        if s == u {
            constraints.push((s, GeoConstraint::Line(t, v)));
        } else {
            constraints.push((s, GeoConstraint::InCircle(u, v, t)));
            constraints.push((u, GeoConstraint::OutCircle(s, t, v)));
        }
        constraints.push((t, GeoConstraint::InCircle(u, v, s)));
        constraints.push((v, GeoConstraint::OutCircle(s, t, u)));
        constraints
    }
}

#[derive(Debug, Clone)]
enum GeoConstraint {
    Line(usize, usize),             // short, long
    InCircle(usize, usize, usize),  // long, long, short
    OutCircle(usize, usize, usize), // short, short, long
}

#[inline(always)]
fn encode_geo_constraint(c: GeoConstraint) -> usize {
    match c {
        GeoConstraint::Line(s, t) => encode4(0, 0, s, t, NV_BW, NV_BW, NV_BW),
        GeoConstraint::InCircle(s, t, u) => encode4(1, s, t, u, NV_BW, NV_BW, NV_BW),
        GeoConstraint::OutCircle(s, t, u) => encode4(2, s, t, u, NV_BW, NV_BW, NV_BW),
    }
}

#[inline(always)]
fn decode_geo_constraint(c: usize) -> GeoConstraint {
    let (t, s, u, v) = decode4(c, NV_BW, NV_BW, NV_BW);
    match t {
        0 => GeoConstraint::Line(u, v),
        1 => GeoConstraint::InCircle(s, u, v),
        2 => GeoConstraint::OutCircle(s, u, v),
        _ => unreachable!(),
    }
}

#[derive(Debug, Clone)]
struct Estimator {
    rng: Mcg128Xsl64,
    my_rng: XorShift32,
    rects: [Rectangle; NUM_VERTICES],
    max_vertices_in_query: usize,
    num_search_vertices: usize,
    update_constraints: usize,
    grad_step: f32,
    kick_step: f32,
    mst_cost_exp: f32,
    buffers: Buffers,
    distribution: WeightedAliasIndex<f32>,
    num_samples: usize,
    samples: Vec<Sample8>,
    queries: Vec<Query>,
    constraint_set: HashSet<usize>,
    constraints_by_vertex: Vec<Vec<usize>>,
    centered: [i32; NUM_VERTICES],
    isolated: [bool; NUM_VERTICES],
    update_counts: i32,
    true_coordinates: [(i32, i32); NUM_VERTICES],
}

impl Estimator {
    fn new(input: &Input) -> Self {
        let samples = Self::init_samples(input);
        let num_samples = 8 * samples.len();
        let isolated = Self::init_isolated_vertices(input);
        let (grad_step, kick_step) = Self::get_grad_kick_step(input);
        let num_search_vertices = Self::get_num_search_vertices(input);
        let update_constraints = Self::get_update_constraints(input);
        let mst_cost_exp = Self::get_mst_cost_exp(input);

        let mut weights = input.rects.iter().map(|r| r.area()).collect_vec();
        for v in 0..NUM_VERTICES {
            if isolated[v] {
                weights[v] = 0.0;
            }
        }
        let distribution = WeightedAliasIndex::new(weights).unwrap();

        let buffers = Buffers::new();

        let true_coordinates = if INTERACTION {
            [(0, 0); NUM_VERTICES]
        } else {
            input_interactive! {
                true_coordinates: [(i32, i32); NUM_VERTICES],
            }
            true_coordinates.try_into().unwrap()
        };

        Self {
            rng: Pcg64Mcg::seed_from_u64(0),
            my_rng: XorShift32::new(1),
            rects: input.rects.clone(),
            max_vertices_in_query: input.max_vertices_in_query,
            num_search_vertices,
            update_constraints,
            grad_step,
            kick_step,
            mst_cost_exp,
            buffers,
            distribution,
            num_samples,
            samples,
            queries: Vec::with_capacity(NUM_QUERIES),
            constraint_set: HashSet::new(),
            constraints_by_vertex: vec![vec![]; NUM_VERTICES],
            centered: [0; NUM_VERTICES],
            isolated,
            update_counts: 0,
            true_coordinates,
        }
    }

    fn init_samples(input: &Input) -> Vec<Sample8> {
        let num_samples = Self::get_num_samples(input);
        debug_assert_eq!(num_samples % 8, 0);
        let sample_bytes = num_samples * std::mem::size_of::<Sample8>() / 8;
        let mut samples = aligned_alloc(sample_bytes, 32);
        for _ in 0..num_samples / 8 {
            let sample = Sample8::new();
            samples.push(sample);
        }

        let mut rng = Pcg64Mcg::seed_from_u64(0);
        let mut perm = (0..num_samples).collect_vec();
        let theta_step = 2.0 * PI / num_samples as f32;
        for v in 0..NUM_VERTICES {
            perm.shuffle(&mut rng);
            let theta_offset = rng.gen_range(0.0..theta_step);
            for i in 0..num_samples {
                let theta = theta_offset + i as f32 * theta_step;
                let (x, y) = input.rects[v].get_by_theta(theta);
                samples[perm[i] / 8].set_point(perm[i] % 8, v, x, y);
            }
        }

        samples
    }

    fn init_isolated_vertices(input: &Input) -> [bool; NUM_VERTICES] {
        let mut isolated = [false; NUM_VERTICES];

        if input.max_vertices_in_query > 4 {
            return isolated;
        }

        let mut num_isolated_vertices = input
            .num_vertices_in_group
            .iter()
            .filter(|&&x| x == 1)
            .count() as i32;

        if input.max_vertices_in_query == 3 {
            num_isolated_vertices -= 5;
        } else {
            num_isolated_vertices -= 55;
        }

        if num_isolated_vertices <= 0 {
            return isolated;
        }

        let mut vertices = (0..NUM_VERTICES)
            .map(|v| (input.rects[v].area(), v))
            .collect_vec();
        vertices.sort_unstable_by(|a, b| b.0.partial_cmp(&a.0).unwrap());

        for i in 0..num_isolated_vertices as usize {
            let v = vertices[i].1;
            isolated[v] = true;
        }

        isolated
    }

    fn get_num_samples(input: &Input) -> usize {
        if input.max_vertices_in_query <= 3 {
            64
        } else if input.max_vertices_in_query <= 4 {
            48
        } else if input.max_vertices_in_query <= 5 {
            40
        } else if input.max_vertices_in_query <= 6 {
            32
        } else if input.max_vertices_in_query <= 8 {
            24
        } else if input.max_vertices_in_query <= 11 {
            16
        } else {
            8
        }
    }

    fn get_grad_kick_step(input: &Input) -> (f32, f32) {
        let rate = 1.0 - (input.max_vertices_in_query - 3) as f32 / 12.0;
        let rate2 = rate * rate;
        let grad_step = 30.0 + (70.0 * rate2);
        let mut kick_step = (0.3 + 0.2 * rate) * grad_step;
        if input.max_vertices_in_query <= 5 {
            kick_step += 5.0;
        }
        kick_step *= 5.0 - 2.0 * rate;
        (grad_step, kick_step)
    }

    fn get_num_search_vertices(input: &Input) -> usize {
        30
    }

    fn get_update_constraints(input: &Input) -> usize {
        let rate = 1.0 - (input.max_vertices_in_query - 3) as f32 / 12.0;
        let rate2 = rate * rate;
        ((1.5 - 0.7 * rate) * (5.0 - 2.0 * rate) * (25.0 - 10.0 * rate2)) as usize
    }

    fn get_mst_cost_exp(input: &Input) -> f32 {
        let rate = (input.max_vertices_in_query - 3) as f32 / 12.0;
        1.5 * (1.0 - rate).max(0.0).powf(1.5)
    }

    fn interact(&mut self, vertices: Vec<usize>) -> Query {
        println!(
            "? {} {}",
            vertices.len(),
            vertices
                .iter()
                .map(|x| x.to_string())
                .collect_vec()
                .join(" ")
        );
        let edges = if INTERACTION {
            input_interactive! {
                edges: [(usize, usize); vertices.len() - 1],
            }
            edges
        } else {
            // calculate MST
            let mut all_edges = vec![];
            for i in 0..vertices.len() {
                let u = vertices[i];
                let (ux, uy) = self.true_coordinates[u];
                for j in i + 1..vertices.len() {
                    let v = vertices[j];
                    let (vx, vy) = self.true_coordinates[v];
                    let dx = ux - vx;
                    let dy = uy - vy;
                    let dist = ((dx * dx + dy * dy) as f32).sqrt() as usize;
                    all_edges.push((dist, u.min(v), u.max(v)));
                }
            }
            all_edges.sort_unstable();
            let mut edges = vec![];
            let mut dsu = DisjointSetUnion::new(NUM_VERTICES);
            for &(_, u, v) in &all_edges {
                if !dsu.merge(u, v) {
                    continue;
                }
                edges.push((u, v));
            }
            edges.sort_unstable();
            edges
        };
        let query = Query { vertices, edges };
        self.add_constraints(&query);
        self.queries.push(query.clone());
        query
    }

    fn add_constraints(&mut self, query: &Query) {
        for &v in &query.vertices {
            self.buffers.adjacency[v].clear();
        }
        for &(u, v) in &query.edges {
            self.buffers.adjacency[u].push(v);
            self.buffers.adjacency[v].push(u);
        }
        for &s in &query.vertices {
            for i in (0..self.buffers.adjacency[s].len()).rev() {
                let t = self.buffers.adjacency[s][i];
                if t > s {
                    continue;
                }
                self.buffers.adjacency[s].swap_remove(i);
                self.buffers.adjacency[t].retain(|&x| x != s);
                let s_group = self.buffers.bfs(s);
                let t_group = self.buffers.bfs(t);
                for &u in &s_group {
                    for &v in &t_group {
                        if u == s && v == t {
                            continue;
                        }
                        self.push_constraint(EdgeConstraint::new(s, t, u, v));
                    }
                }
                self.buffers.adjacency[s].push(t);
                self.buffers.adjacency[t].push(s);
            }
        }
    }

    fn push_constraint(&mut self, constraint: EdgeConstraint) -> bool {
        let hash = constraint.encode();
        if !self.constraint_set.insert(hash) {
            return false;
        }
        let (s, t, u, v) = constraint.decode();
        if self
            .samples
            .iter()
            .all(|sample| unsafe { sample.all_sat_edge_constraint(s, t, u, v) })
        {
            return false;
        }
        for (v, c) in constraint.get_geometric_constraints() {
            self.constraints_by_vertex[v].push(encode_geo_constraint(c));
        }
        return true;
    }

    fn update_sample(&mut self, update_time_limit: f64) {
        while get_time_sec() < update_time_limit {
            for _ in 0..UPDATE_BATCHES {
                let v = self.distribution.sample(&mut self.rng);
                let num_constraints = self.constraints_by_vertex[v].len();
                if num_constraints == 0 {
                    continue;
                }
                // random walk
                let rect = &self.rects[v];
                for sample in &mut self.samples {
                    for sample_id in 0..8 {
                        let (x, y) = sample.get_point(sample_id, v);
                        let (nx, ny) = rect.sample_around(&mut self.my_rng, x, y, self.kick_step);
                        sample.set_point(sample_id, v, nx, ny);
                    }
                }
                // most gradient descent
                let update_constraints = if num_constraints < self.update_constraints {
                    num_constraints
                } else {
                    // ramdomly choose
                    for i in 0..self.update_constraints {
                        let j = i + self.my_rng.gen_range((num_constraints - i) as u64) as usize;
                        self.constraints_by_vertex[v].swap(i, j);
                    }
                    self.update_constraints
                };
                unsafe {
                    self.update_vertex(v, update_constraints);
                }
                self.update_counts += self.num_samples as i32 * update_constraints as i32;
            }
        }
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn update_vertex(&mut self, v: usize, update_constraints: usize) {
        let grad_step = _mm256_set1_ps(self.grad_step);
        let rect = &self.rects[v];
        let lx = _mm256_set1_ps(rect.lx);
        let rx = _mm256_set1_ps(rect.rx);
        let ly = _mm256_set1_ps(rect.ly);
        let ry = _mm256_set1_ps(rect.ry);
        let eps = _mm256_set1_ps(EPS_UPDATE);
        for &c in &self.constraints_by_vertex[v][..update_constraints] {
            let c = decode_geo_constraint(c);
            for sample in &mut self.samples {
                sample.gradient(v, &c, grad_step, lx, rx, ly, ry, eps);
            }
        }
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn update_vertex(&mut self, v: usize, update_constraints: usize) {
        let rect = &self.rects[v];
        let grad_step = self.grad_step;
        for &c in &self.constraints_by_vertex[v][..update_constraints] {
            let c = decode_geo_constraint(c);
            for sample in &mut self.samples {
                sample.gradient(v, &c, rect, grad_step);
            }
        }
    }

    fn choose_vertices(&mut self) -> Vec<usize> {
        let center = self.calc_most_uncertain_vertex();
        self.centered[center] += 1;

        let mut vertices_sorted = Vec::with_capacity(NUM_VERTICES);
        for v in 0..NUM_VERTICES {
            if self.isolated[v] || v == center {
                continue;
            }
            let mut sum_dist = 0.0;
            for sample in &self.samples {
                sum_dist += sample.calc_distance(center, v).iter().sum::<f32>();
            }
            vertices_sorted.push(encode2(sum_dist as usize, v, NV_BW));
        }
        vertices_sorted.sort_unstable();
        vertices_sorted.truncate(self.num_search_vertices);
        for v in &mut vertices_sorted {
            *v = decode2(*v, NV_BW).1;
        }

        // greedy
        let mut ret = vec![center];
        ret.push(vertices_sorted.swap_remove(0));

        let ret0 = ret[0];
        let ret1 = ret[1];
        let mut msts = vec![vec![]; self.num_samples];
        for i in 0..self.num_samples {
            let dist = self.samples[i / 8].get_distance(i % 8, ret0, ret1) as usize;
            msts[i].push(encode3(dist, ret0, ret1, NV_BW, NV_BW));
        }
        let mut nmsts = vec![vec![]; self.num_samples];
        let mut omsts = vec![vec![]; self.num_samples];

        for _ in 2..self.max_vertices_in_query {
            let mut max_profit = -1e9;
            let mut best_index = 0;
            for i in 0..vertices_sorted.len() {
                let v = vertices_sorted[i];
                ret.push(v);
                for i in 0..self.num_samples {
                    for &u in &ret {
                        let dist = self.samples[i / 8].get_distance(i % 8, u, v) as usize;
                        msts[i].push(encode3(dist, u, v, NV_BW, NV_BW));
                    }
                }
                let profit = self.calc_query_profit(&ret, &mut msts, &mut omsts);
                for i in 0..self.num_samples {
                    msts[i].retain(|&duv| decode3(duv, NV_BW, NV_BW).2 != v);
                }
                if max_profit.chmax(profit) {
                    best_index = i;
                    std::mem::swap(&mut nmsts, &mut omsts);
                }
                ret.pop();
            }
            ret.push(vertices_sorted.swap_remove(best_index));
            std::mem::swap(&mut nmsts, &mut msts);
        }
        ret
    }

    fn calc_query_profit(
        &mut self,
        vertices: &[usize],
        msts: &mut Vec<Vec<usize>>,
        nmsts: &mut Vec<Vec<usize>>,
    ) -> f32 {
        self.buffers.set_vertex_to_index(vertices);
        let mut sum_mst_cost = 0.0;
        let mut mst_costs = Vec::with_capacity(self.num_samples);
        let mut counts = vec![0; vertices.len() * vertices.len()];
        for sample_id in 0..self.num_samples {
            let cost = self.samples[sample_id / 8].kruskal(
                sample_id % 8,
                vertices,
                &mut self.buffers,
                &mut msts[sample_id],
                &mut nmsts[sample_id],
            );
            sum_mst_cost += cost;
            mst_costs.push(cost);
            for &duv in &nmsts[sample_id] {
                let (_, u, v) = decode3(duv, NV_BW, NV_BW);
                let ui = self.buffers.vertex_to_index[u];
                let vi = self.buffers.vertex_to_index[v];
                counts[ui * vertices.len() + vi] += 1;
            }
        }

        let mut profit = -(sum_mst_cost * self.num_samples as f32);
        for i in 0..vertices.len() {
            for j in 0..vertices.len() {
                let u = vertices[i];
                let v = vertices[j];
                if u == v {
                    continue;
                }
                let ui = self.buffers.vertex_to_index[u];
                let vi = self.buffers.vertex_to_index[v];
                let cnt = counts[ui * vertices.len() + vi];
                if cnt == 0 {
                    continue;
                }
                let mut p = 0.0;
                for sample in &self.samples {
                    for sample_id in 0..8 {
                        p += sample.get_distance(sample_id, u, v)
                    }
                }
                profit += cnt as f32 * p;
            }
        }

        profit /= sum_mst_cost.powf(self.mst_cost_exp) + 1e-9;
        profit
    }

    fn calc_most_uncertain_vertex(&self) -> usize {
        let max_centered = if self.max_vertices_in_query <= 3 {
            3
        } else {
            1
        };
        let mut ret = NUM_VERTICES;
        let mut max_var = 0.0;
        for v in 0..NUM_VERTICES {
            if self.centered[v] >= max_centered || self.isolated[v] {
                continue;
            }
            let mut x_sum = 0.0;
            let mut x2_sum = 0.0;
            let mut y_sum = 0.0;
            let mut y2_sum = 0.0;
            for sample in &self.samples {
                for sample_id in 0..8 {
                    let (x, y) = sample.get_point(sample_id, v);
                    x_sum += x;
                    x2_sum += x * x;
                    y_sum += y;
                    y2_sum += y * y;
                }
            }
            let n = self.num_samples as f32;
            let x_var = x2_sum / n - (x_sum * x_sum) / (n * n);
            let y_var = y2_sum / n - (y_sum * y_sum) / (n * n);
            let var = x_var + y_var;
            if max_var.chmax(var) {
                ret = v;
            }
        }
        ret
    }

    fn calc_average_distances(&self) -> Vec<[u32; NUM_VERTICES]> {
        let mut distances = vec![[0; NUM_VERTICES]; NUM_VERTICES];
        for u in 0..NUM_VERTICES {
            for v in 0..NUM_VERTICES {
                let mut s = 0.0;
                for sample in &self.samples {
                    s += sample.calc_distance(u, v).iter().sum::<f32>();
                }
                let avg = (s / self.num_samples as f32) as u32;
                distances[u][v] = avg;
                distances[v][u] = avg;
            }
        }
        distances
    }

    fn replace_to_shorter_edge(
        &mut self,
        output: Output,
        distances: &Vec<[u32; NUM_VERTICES]>,
    ) -> Output {
        let mut replaceable_edges: HashMap<(usize, usize), Vec<(usize, usize)>> = HashMap::new();
        for &tuvw in &self.constraint_set {
            let (t, u, v, w) = decode4(tuvw, NV_BW, NV_BW, NV_BW);
            match replaceable_edges.get_mut(&(v, w)) {
                Some(edges) => {
                    edges.push((t, u));
                }
                None => {
                    replaceable_edges.insert((v, w), vec![(t, u)]);
                }
            }
        }

        let mut ret = Output {
            groups: vec![],
            edges: vec![],
            cost: 0,
        };
        let mut in_u_group = FixedBitSet::with_capacity(NUM_VERTICES);
        let mut in_t_group = FixedBitSet::with_capacity(NUM_VERTICES);

        for (group, edge) in output.groups.iter().zip(&output.edges) {
            let group = group.clone();
            for &v in &group {
                self.buffers.adjacency[v].clear();
            }
            for &(u, v) in edge {
                self.buffers.adjacency[u].push(v);
                self.buffers.adjacency[v].push(u);
            }
            for &t in &group {
                for i in (0..self.buffers.adjacency[t].len()).rev() {
                    let u = self.buffers.adjacency[t][i];
                    if u > t {
                        continue;
                    }
                    if !replaceable_edges.contains_key(&(u, t)) {
                        continue;
                    }
                    self.buffers.adjacency[t].swap_remove(i);
                    self.buffers.adjacency[u].retain(|&x| x != t);
                    let u_group = self.buffers.bfs(u);
                    let t_group = self.buffers.bfs(t);

                    in_u_group.clear();
                    in_t_group.clear();
                    for &v in &u_group {
                        in_u_group.insert(v);
                    }
                    for &v in &t_group {
                        in_t_group.insert(v);
                    }
                    let mut replaced = false;
                    for &(v, w) in replaceable_edges.get(&(u, t)).unwrap() {
                        if (in_u_group[v] && in_t_group[w]) || (in_t_group[v] && in_u_group[w]) {
                            // replace
                            self.buffers.adjacency[v].push(w);
                            self.buffers.adjacency[w].push(v);
                            replaced = true;
                            break;
                        }
                    }
                    if !replaced {
                        self.buffers.adjacency[t].push(u);
                        self.buffers.adjacency[u].push(t);
                    }
                }
            }
            let mut edge = vec![];
            for &u in &group {
                for &v in &self.buffers.adjacency[u] {
                    if u > v {
                        continue;
                    }
                    edge.push((u, v));
                    ret.cost += distances[u][v] as i64;
                }
            }
            ret.groups.push(group);
            ret.edges.push(edge);
        }
        ret
    }

    fn improve_output(&mut self, mut output: Output, mut remained_queries: usize) -> Output {
        if remained_queries == 0 {
            return output;
        }

        let mut all_edges = vec![vec![]; NUM_VERTICES];
        for v in 0..NUM_VERTICES {
            all_edges[v].clear();
        }
        for edge in &output.edges {
            for &(u, v) in edge {
                all_edges[u].push(v);
                all_edges[v].push(u);
            }
        }

        self.mst_cost_exp = 0.0;

        let mut triangles = vec![];
        for u in 0..NUM_VERTICES {
            for i in 0..all_edges[u].len() {
                let v = all_edges[u][i];
                for j in (i + 1)..all_edges[u].len() {
                    let w = all_edges[u][j];
                    debug_assert_ne!(v, w);
                    let mut msts = vec![vec![]; self.num_samples];
                    let mut nmsts = vec![vec![]; self.num_samples];
                    let profit = self.calc_query_profit(&[u, v, w], &mut msts, &mut nmsts);
                    if profit > 0.0 {
                        triangles.push((profit, u, v, w));
                    }
                }
            }
        }
        triangles.sort_unstable_by(|a, b| a.0.partial_cmp(&b.0).unwrap());

        while remained_queries > 0 && !triangles.is_empty() {
            let (_, u, v, w) = triangles.pop().unwrap();

            if !all_edges[u].contains(&v) {
                continue;
            };
            if !all_edges[u].contains(&w) {
                continue;
            }

            let query = self.interact(vec![u, v, w]);
            remained_queries -= 1;

            all_edges[u].retain(|&x| x != v && x != w);
            all_edges[v].retain(|&x| x != u);
            all_edges[w].retain(|&x| x != u);
            for &(a, b) in &query.edges {
                all_edges[a].push(b);
                all_edges[b].push(a);
            }
        }

        let mut vertex_to_group = [0; NUM_VERTICES];
        for (i, group) in output.groups.iter().enumerate() {
            for &v in group {
                vertex_to_group[v] = i;
            }
            output.edges[i].clear();
        }

        for u in 0..NUM_VERTICES {
            for &v in &all_edges[u] {
                if u > v {
                    continue;
                }
                debug_assert_eq!(vertex_to_group[u], vertex_to_group[v]);
                output.edges[vertex_to_group[u]].push((u, v));
            }
        }

        output
    }

    fn print_log(&self) {
        eprintln!("update_counts: {}", self.update_counts);
    }

    fn print_true_score(&self, output: &Output) {
        if INTERACTION {
            return;
        }
        let mut score = 0;
        for edge in &output.edges {
            for &(u, v) in edge {
                let (ux, uy) = self.true_coordinates[u];
                let (vx, vy) = self.true_coordinates[v];
                let dx = ux - vx;
                let dy = uy - vy;
                let dist = ((dx * dx + dy * dy) as f64).sqrt() as usize;
                score += dist;
            }
        }
        eprintln!("score: {}", score);
    }
}

#[derive(Debug, Clone)]
struct Buffers {
    dsu: DisjointSetUnion,
    adjacency: Vec<Vec<usize>>,
    visited: FixedBitSet,
    vertex_to_index: [usize; NUM_VERTICES],
}

impl Buffers {
    fn new() -> Self {
        Buffers {
            dsu: DisjointSetUnion::new(NUM_VERTICES),
            adjacency: vec![vec![]; NUM_VERTICES],
            visited: FixedBitSet::with_capacity(NUM_VERTICES),
            vertex_to_index: [0; NUM_VERTICES],
        }
    }

    fn bfs(&mut self, root: usize) -> Vec<usize> {
        self.visited.clear();
        let mut ret = vec![root];
        self.visited.insert(root);
        let mut i = 0;
        while i < ret.len() {
            let u = ret[i];
            i += 1;
            for &v in &self.adjacency[u] {
                if self.visited.contains(v) {
                    continue;
                }
                self.visited.insert(v);
                ret.push(v);
            }
        }
        ret
    }

    fn set_vertex_to_index(&mut self, vertices: &[usize]) {
        for i in 0..vertices.len() {
            self.vertex_to_index[vertices[i]] = i;
        }
    }
}

#[derive(Debug, Clone)]
struct Sample8 {
    points: [[f32; 8]; 2 * NUM_VERTICES],
}

impl Sample8 {
    fn new() -> Self {
        Sample8 {
            points: [[0.0; 8]; 2 * NUM_VERTICES],
        }
    }

    fn kruskal(
        &self,
        sample_id: usize,
        vertices: &[usize],
        buffers: &mut Buffers,
        all_edges: &mut Vec<usize>,
        mst: &mut Vec<usize>,
    ) -> f32 {
        if all_edges.is_empty() {
            all_edges.reserve(vertices.len() * (vertices.len() - 1) / 2);
            for i in 0..vertices.len() {
                let u = vertices[i];
                let (ux, uy) = self.get_point(sample_id, u);
                for j in i + 1..vertices.len() {
                    let v = vertices[j];
                    let (vx, vy) = self.get_point(sample_id, v);
                    let dist = euclid(ux, uy, vx, vy) as usize;
                    let duw = encode3(dist, u, v, NV_BW, NV_BW);
                    all_edges.push(duw);
                }
            }
        }

        all_edges.sort_unstable();

        buffers.dsu.clear(vertices);
        let mut cost = 0;
        mst.clear();
        for &duv in all_edges.iter() {
            let (dist, u, v) = decode3(duv, NV_BW, NV_BW);
            if !buffers.dsu.merge(u, v) {
                continue;
            }
            cost += dist;
            mst.push(duv);
            if mst.len() == vertices.len() - 1 {
                break;
            }
        }
        // edges.sort_unstable();
        cost as f32
    }

    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn all_sat_edge_constraint(&self, s: usize, t: usize, u: usize, v: usize) -> bool {
        let sx = _mm256_loadu_ps(self.points[2 * s].as_ptr());
        let sy = _mm256_loadu_ps(self.points[2 * s + 1].as_ptr());
        let tx = _mm256_loadu_ps(self.points[2 * t].as_ptr());
        let ty = _mm256_loadu_ps(self.points[2 * t + 1].as_ptr());
        let ux = _mm256_loadu_ps(self.points[2 * u].as_ptr());
        let uy = _mm256_loadu_ps(self.points[2 * u + 1].as_ptr());
        let vx = _mm256_loadu_ps(self.points[2 * v].as_ptr());
        let vy = _mm256_loadu_ps(self.points[2 * v + 1].as_ptr());
        let st = euclid2_avx(sx, sy, tx, ty);
        let uv = euclid2_avx(ux, uy, vx, vy);
        let mask = _mm256_cmp_ps(st, uv, _CMP_LE_OQ);
        let mut ret = [0; 8];
        _mm256_storeu_ps(ret.as_mut_ptr() as *mut f32, mask);
        ret.iter().all(|&x| x != 0)
    }

    #[cfg(not(target_arch = "x86_64"))]
    fn all_sat_edge_constraint(&self, s: usize, t: usize, u: usize, v: usize) -> bool {
        for i in 0..8 {
            let sx = self.points[2 * s][i];
            let sy = self.points[2 * s + 1][i];
            let tx = self.points[2 * t][i];
            let ty = self.points[2 * t + 1][i];
            let ux = self.points[2 * u][i];
            let uy = self.points[2 * u + 1][i];
            let vx = self.points[2 * v][i];
            let vy = self.points[2 * v + 1][i];
            let st = euclid2(sx, sy, tx, ty);
            let uv = euclid2(ux, uy, vx, vy);
            if st > uv {
                return false;
            }
        }
        true
    }

    /// most gradient descent
    #[cfg(target_arch = "x86_64")]
    #[target_feature(enable = "avx2")]
    unsafe fn gradient(
        &mut self,
        v: usize,
        c: &GeoConstraint,
        grad_step: __m256,
        lx: __m256,
        rx: __m256,
        ly: __m256,
        ry: __m256,
        eps: __m256,
    ) {
        let vx = _mm256_loadu_ps(self.points.get_unchecked(2 * v).as_ptr());
        let vy = _mm256_loadu_ps(self.points.get_unchecked(2 * v + 1).as_ptr());
        match c {
            GeoConstraint::Line(s, t) => {
                let sx = _mm256_loadu_ps(self.points.get_unchecked(2 * *s).as_ptr());
                let sy = _mm256_loadu_ps(self.points.get_unchecked(2 * *s + 1).as_ptr());
                let tx = _mm256_loadu_ps(self.points.get_unchecked(2 * *t).as_ptr());
                let ty = _mm256_loadu_ps(self.points.get_unchecked(2 * *t + 1).as_ptr());

                let vs2 = euclid2_avx(sx, sy, vx, vy);
                let vt2 = euclid2_avx(tx, ty, vx, vy);
                let mask = _mm256_cmp_ps(vs2, vt2, _CMP_LE_OQ);

                let tsx = _mm256_sub_ps(sx, tx);
                let tsy = _mm256_sub_ps(sy, ty);
                let tsx2 = _mm256_mul_ps(tsx, tsx);
                let tsy2 = _mm256_mul_ps(tsy, tsy);
                let ts2 = _mm256_add_ps(tsx2, tsy2);
                let ts = _mm256_sqrt_ps(ts2);

                let ts = _mm256_add_ps(ts, eps);
                let dx = _mm256_mul_ps(tsx, grad_step);
                let dy = _mm256_mul_ps(tsy, grad_step);
                let dx = _mm256_div_ps(dx, ts);
                let dy = _mm256_div_ps(dy, ts);
                let x = _mm256_add_ps(vx, dx);
                let y = _mm256_add_ps(vy, dy);

                let x = _mm256_max_ps(lx, _mm256_min_ps(x, rx));
                let y = _mm256_max_ps(ly, _mm256_min_ps(y, ry));
                let x = _mm256_blendv_ps(x, vx, mask);
                let y = _mm256_blendv_ps(y, vy, mask);
                _mm256_storeu_ps(self.points.get_unchecked_mut(2 * v).as_mut_ptr(), x);
                _mm256_storeu_ps(self.points.get_unchecked_mut(2 * v + 1).as_mut_ptr(), y);
            }
            GeoConstraint::InCircle(s, t, u) => {
                let sx = _mm256_loadu_ps(self.points.get_unchecked(2 * *s).as_ptr());
                let sy = _mm256_loadu_ps(self.points.get_unchecked(2 * *s + 1).as_ptr());
                let tx = _mm256_loadu_ps(self.points.get_unchecked(2 * *t).as_ptr());
                let ty = _mm256_loadu_ps(self.points.get_unchecked(2 * *t + 1).as_ptr());
                let ux = _mm256_loadu_ps(self.points.get_unchecked(2 * *u).as_ptr());
                let uy = _mm256_loadu_ps(self.points.get_unchecked(2 * *u + 1).as_ptr());

                let ts2 = euclid2_avx(sx, sy, tx, ty);

                let vux = _mm256_sub_ps(ux, vx);
                let vuy = _mm256_sub_ps(uy, vy);
                let vux2 = _mm256_mul_ps(vux, vux);
                let vuy2 = _mm256_mul_ps(vuy, vuy);
                let vu2 = _mm256_add_ps(vux2, vuy2);

                let mask = _mm256_cmp_ps(ts2, vu2, _CMP_GE_OQ);

                let vu = _mm256_sqrt_ps(vu2);
                let vu = _mm256_add_ps(vu, eps);
                let grad_step = _mm256_min_ps(grad_step, vu);
                let dx = _mm256_mul_ps(vux, grad_step);
                let dy = _mm256_mul_ps(vuy, grad_step);
                let dx = _mm256_div_ps(dx, vu);
                let dy = _mm256_div_ps(dy, vu);
                let x = _mm256_add_ps(vx, dx);
                let y = _mm256_add_ps(vy, dy);

                let x = _mm256_max_ps(lx, _mm256_min_ps(x, rx));
                let y = _mm256_max_ps(ly, _mm256_min_ps(y, ry));
                let x = _mm256_blendv_ps(x, vx, mask);
                let y = _mm256_blendv_ps(y, vy, mask);
                _mm256_storeu_ps(self.points.get_unchecked_mut(2 * v).as_mut_ptr(), x);
                _mm256_storeu_ps(self.points.get_unchecked_mut(2 * v + 1).as_mut_ptr(), y);
            }
            GeoConstraint::OutCircle(s, t, u) => {
                let sx = _mm256_loadu_ps(self.points.get_unchecked(2 * *s).as_ptr());
                let sy = _mm256_loadu_ps(self.points.get_unchecked(2 * *s + 1).as_ptr());
                let tx = _mm256_loadu_ps(self.points.get_unchecked(2 * *t).as_ptr());
                let ty = _mm256_loadu_ps(self.points.get_unchecked(2 * *t + 1).as_ptr());
                let ux = _mm256_loadu_ps(self.points.get_unchecked(2 * *u).as_ptr());
                let uy = _mm256_loadu_ps(self.points.get_unchecked(2 * *u + 1).as_ptr());

                let ts2 = euclid2_avx(sx, sy, tx, ty);

                let uvx = _mm256_sub_ps(vx, ux);
                let uvy = _mm256_sub_ps(vy, uy);
                let uvx2 = _mm256_mul_ps(uvx, uvx);
                let uvy2 = _mm256_mul_ps(uvy, uvy);
                let uv2 = _mm256_add_ps(uvx2, uvy2);

                let mask = _mm256_cmp_ps(ts2, uv2, _CMP_LE_OQ);

                let uv = _mm256_sqrt_ps(uv2);
                let uv = _mm256_add_ps(uv, eps);
                let dx = _mm256_mul_ps(uvx, grad_step);
                let dy = _mm256_mul_ps(uvy, grad_step);
                let dx = _mm256_div_ps(dx, uv);
                let dy = _mm256_div_ps(dy, uv);
                let x = _mm256_add_ps(vx, dx);
                let y = _mm256_add_ps(vy, dy);

                let x = _mm256_max_ps(lx, _mm256_min_ps(x, rx));
                let y = _mm256_max_ps(ly, _mm256_min_ps(y, ry));
                let x = _mm256_blendv_ps(x, vx, mask);
                let y = _mm256_blendv_ps(y, vy, mask);
                _mm256_storeu_ps(self.points.get_unchecked_mut(2 * v).as_mut_ptr(), x);
                _mm256_storeu_ps(self.points.get_unchecked_mut(2 * v + 1).as_mut_ptr(), y);
            }
        }
    }

    /// most gradient descent
    #[cfg(not(target_arch = "x86_64"))]
    fn gradient(&mut self, v: usize, c: &GeoConstraint, rect: &Rectangle, grad_step: f32) {
        for sample_id in 0..8 {
            let (vx, vy) = self.get_point(sample_id, v);
            match c {
                GeoConstraint::Line(s, t) => {
                    let (sx, sy) = self.get_point(sample_id, *s);
                    let (tx, ty) = self.get_point(sample_id, *t);
                    let sv2 = euclid2(sx, sy, vx, vy);
                    let tv2 = euclid2(tx, ty, vx, vy);
                    if sv2 <= tv2 {
                        // OK
                        continue;
                    }
                    let tsx = sx - tx;
                    let tsy = sy - ty;
                    let ts = (tsx * tsx + tsy * tsy).sqrt() + EPS_UPDATE;
                    let dx = tsx * grad_step / ts;
                    let dy = tsy * grad_step / ts;
                    let (x, y) = rect.clamp(vx + dx, vy + dy);
                    self.set_point(sample_id, v, x, y);
                }
                GeoConstraint::InCircle(s, t, u) => {
                    let (sx, sy) = self.get_point(sample_id, *s);
                    let (tx, ty) = self.get_point(sample_id, *t);
                    let (ux, uy) = self.get_point(sample_id, *u);
                    let st2 = euclid2(sx, sy, tx, ty);
                    let uv2 = euclid2(ux, uy, vx, vy);
                    if st2 >= uv2 {
                        // OK
                        continue;
                    }
                    let vux = ux - vx;
                    let vuy = uy - vy;
                    let vu = (vux * vux + vuy * vuy).sqrt() + EPS_UPDATE;
                    let grad_step = grad_step.min(vu);
                    let dx = vux * grad_step / vu;
                    let dy = vuy * grad_step / vu;
                    let (x, y) = rect.clamp(vx + dx, vy + dy);
                    self.set_point(sample_id, v, x, y);
                }
                GeoConstraint::OutCircle(s, t, u) => {
                    let (sx, sy) = self.get_point(sample_id, *s);
                    let (tx, ty) = self.get_point(sample_id, *t);
                    let (ux, uy) = self.get_point(sample_id, *u);
                    let st2 = euclid2(sx, sy, tx, ty);
                    let uv2 = euclid2(vx, vy, ux, uy);
                    if st2 <= uv2 {
                        // OK
                        continue;
                    }
                    let uvx = vx - ux;
                    let uvy = vy - uy;
                    let uv = (uvx * uvx + uvy * uvy).sqrt() + EPS_UPDATE;
                    let dx = uvx * grad_step / uv;
                    let dy = uvy * grad_step / uv;
                    let (x, y) = rect.clamp(vx + dx, vy + dy);
                    self.set_point(sample_id, v, x, y);
                }
            }
        }
    }

    #[inline(always)]
    fn get_point(&self, sample_id: usize, v: usize) -> (f32, f32) {
        let x = self.points[2 * v][sample_id];
        let y = self.points[2 * v + 1][sample_id];
        (x, y)
    }

    #[inline(always)]
    fn set_point(&mut self, sample_id: usize, v: usize, x: f32, y: f32) {
        self.points[2 * v][sample_id] = x;
        self.points[2 * v + 1][sample_id] = y;
    }

    #[inline(always)]
    fn calc_distance(&self, u: usize, v: usize) -> [f32; 8] {
        let mut ret = [0.0; 8];
        for i in 0..8 {
            let (ux, uy) = self.get_point(i, u);
            let (vx, vy) = self.get_point(i, v);
            ret[i] = euclid(ux, uy, vx, vy);
        }
        ret
    }

    #[inline(always)]
    fn get_distance(&self, sample_id: usize, u: usize, v: usize) -> f32 {
        let (ux, uy) = self.get_point(sample_id, u);
        let (vx, vy) = self.get_point(sample_id, v);
        euclid(ux, uy, vx, vy)
    }
}

#[derive(Debug, Clone)]
struct Output {
    groups: Vec<Vec<usize>>,
    edges: Vec<Vec<(usize, usize)>>,
    cost: i64,
}

impl Output {
    fn print(&self) {
        println!("!");
        for (group, edge) in self.groups.iter().zip(&self.edges) {
            debug_assert_eq!(group.len(), edge.len() + 1);
            println!(
                "{}",
                group.iter().map(|x| x.to_string()).collect_vec().join(" ")
            );
            for &(u, v) in edge {
                println!("{} {}", u, v);
            }
        }
    }
}

#[derive(Debug, Clone)]
struct GroupOptimizer {
    rng: Mcg128Xsl64,
    scheduler: AnnealingScheduler,
    num_vertices_in_group: Vec<usize>,
    distances: Vec<[u32; NUM_VERTICES]>,
    sorted_edges: Vec<usize>,
    short_edges: Vec<(u32, u32)>,
    edges: [Vec<usize>; NUM_VERTICES],
    roots: [usize; NUM_VERTICES],
    parents: [usize; NUM_VERTICES],
    sizes: [u32; NUM_VERTICES],
    group_counts: [i32; NUM_VERTICES + 1],
    permutation: [u32; NUM_VERTICES],
    raw_cost: i64,
    group_cost: i64,
}

impl GroupOptimizer {
    fn new(input: &Input, distances: Vec<[u32; NUM_VERTICES]>) -> Self {
        // make edges_sorted
        let mut sorted_edges = Vec::with_capacity(NUM_VERTICES * (NUM_VERTICES - 1) / 2);
        for u in 0..NUM_VERTICES {
            for v in u + 1..NUM_VERTICES {
                let dist = distances[u][v] as usize;
                let duv = encode3(dist, u, v, NV_BW, NV_BW);
                sorted_edges.push(duv);
            }
        }
        sorted_edges.sort_unstable();

        // enumerate short_edges
        let mut is_used = vec![false; sorted_edges.len()];
        let mut short_edges = vec![];
        let mut edges = vec![vec![]; NUM_VERTICES];
        for mst_id in 0..NUM_MSTS {
            // Kruskal's algorithm
            let mut dsu = DisjointSetUnion::new(NUM_VERTICES);
            let mut num_groups = NUM_VERTICES;
            for i in 0..sorted_edges.len() {
                if is_used[i] {
                    continue;
                }
                let (_, u, v) = decode3(sorted_edges[i], NV_BW, NV_BW);
                if !dsu.merge(u, v) {
                    continue;
                }
                short_edges.push((u as u32, v as u32));
                is_used[i] = true;
                num_groups -= 1;
                if num_groups == 1 {
                    break;
                }
            }
            if mst_id == 0 {
                for &(u, v) in &short_edges {
                    edges[u as usize].push(v as usize);
                    edges[v as usize].push(u as usize);
                }
            }
        }

        let mut ret = Self {
            rng: Pcg64Mcg::seed_from_u64(0),
            scheduler: AnnealingScheduler::new(SCHEDULER_TYPE, T0, T1, TIME_LIMIT_SEC),
            num_vertices_in_group: input.num_vertices_in_group.clone(),
            distances: distances,
            sorted_edges,
            short_edges,
            edges: edges.try_into().unwrap(),
            roots: [0; NUM_VERTICES],
            parents: [0; NUM_VERTICES],
            sizes: [0; NUM_VERTICES],
            group_counts: [0; NUM_VERTICES + 1],
            permutation: (0..NUM_VERTICES as u32).collect_vec().try_into().unwrap(),
            raw_cost: 0,
            group_cost: 0,
        };

        ret.evaluate_full();

        ret
    }

    fn evaluate_full(&mut self) {
        self.raw_cost = 0;
        for u in 0..NUM_VERTICES {
            for &v in &self.edges[u] {
                if u < v {
                    self.raw_cost += self.get_dist(u, v);
                }
            }
        }

        self.group_counts.fill(0);
        for &c in &self.num_vertices_in_group {
            self.group_counts[c] -= 1;
        }

        self.roots.fill(!0);

        for root in 0..NUM_VERTICES {
            if self.roots[root] != !0 {
                continue;
            }
            self.update_subtree(root, root);
            self.group_counts[self.sizes[root] as usize] += 1;
        }

        self.group_cost = 0;
        for &d in &self.group_counts {
            let d = d as i64;
            self.group_cost += d * d;
        }
    }

    fn update(&mut self) -> bool {
        let r = self.rng.gen_range(0..SA_SPLIT);
        if r < SA_MERGE {
            return self.try_to_merge(SA_PATIENCE);
        } else {
            return self.try_to_split(SA_PATIENCE);
        }
    }

    fn update2(&mut self) -> bool {
        let (u, v) = self.choose_short_edge();
        if self.roots[u] != self.roots[v] {
            return self.merge_then_split(SA_PATIENCE, u, v);
        } else {
            return self.replace_edge_in_tree(SA_PATIENCE, u, v);
        }
    }

    fn try_to_merge(&mut self, patience: i32) -> bool {
        let (u_merge, v_merge) = self.choose_short_edge();
        let u_root = self.roots[u_merge];
        let v_root = self.roots[v_merge];
        if u_root == v_root {
            if patience == 0 {
                return false;
            }
            return self.try_to_merge(patience - 1);
        }
        let u_size = self.sizes[u_root] as usize;
        let v_size = self.sizes[v_root] as usize;
        let d_group_cost = self.calc_merge_d_group_cost(u_size, v_size);
        let d_raw_cost = self.get_dist(u_merge, v_merge);
        let d_cost = GW * d_group_cost + d_raw_cost;

        if self.scheduler.accept(-(d_cost as f64)) {
            // accept
            self.merge(u_merge, v_merge);
            self.group_cost += d_group_cost;
            self.group_counts[u_size] -= 1;
            self.group_counts[v_size] -= 1;
            self.group_counts[u_size + v_size] += 1;
            return true;
        } else {
            // reject
            return false;
        }
    }

    fn try_to_split(&mut self, patience: i32) -> bool {
        let (mut u_split, mut v_split) = self.choose_short_edge();
        if !self.edges[u_split].contains(&v_split) {
            if patience == 0 {
                return false;
            }
            return self.try_to_split(patience - 1);
        }
        if self.parents[u_split] == v_split {
            std::mem::swap(&mut u_split, &mut v_split);
        }
        let root = self.roots[u_split];
        let v_size = self.sizes[v_split] as usize;
        let u_size = self.sizes[root] as usize - v_size;
        let d_group_cost = self.calc_split_d_group_cost(u_size, v_size);
        let d_raw_cost = -self.get_dist(u_split, v_split);

        let d_cost = GW * d_group_cost + d_raw_cost;
        if self.scheduler.accept(-(d_cost as f64)) {
            // accept
            self.split(u_split, v_split);
            self.group_cost += d_group_cost;
            self.group_counts[u_size] += 1;
            self.group_counts[v_size] += 1;
            self.group_counts[u_size + v_size] -= 1;
            return true;
        } else {
            // reject
            return false;
        }
    }

    fn merge_then_split(&mut self, patience: i32, u_merge: usize, v_merge: usize) -> bool {
        let u_root = self.roots[u_merge];
        let v_root = self.roots[v_merge];
        if u_root == v_root {
            if patience == 0 {
                return false;
            }
            let (u_merge, v_merge) = self.choose_short_edge();
            return self.merge_then_split(patience - 1, u_merge, v_merge);
        }

        let u_size = self.sizes[u_root];
        let v_size = self.sizes[v_root];

        self.merge(u_merge, v_merge);
        let cost = self.get_dist(u_merge, v_merge);

        // if remove edge cost >= threshold then accept
        let threshold = cost + self.scheduler.get_threshold() as i64;

        for i in 0..NUM_SHUFFLES {
            let j = self.rng.gen_range(0..NUM_VERTICES);
            self.permutation.swap(i, j);
        }

        for i in 0..NUM_VERTICES {
            let w = self.permutation[i] as usize;
            let w_size = self.sizes[w];
            if w_size != u_size && w_size != v_size {
                continue;
            }
            if self.sizes[self.roots[w]] != u_size + v_size {
                continue;
            }
            if w == u_merge || w == v_merge {
                continue;
            }
            let cost = self.get_dist(w, self.parents[w]);
            if cost < threshold {
                continue;
            }
            // accept
            self.scheduler.accepted();
            self.split(w, self.parents[w]);
            return true;
        }

        // reject
        self.scheduler.rejected();
        self.split(u_merge, v_merge);
        return false;
    }

    fn replace_edge_in_tree(&mut self, patience: i32, u_merge: usize, v_merge: usize) -> bool {
        let u_root = self.roots[u_merge];
        let v_root = self.roots[v_merge];
        if u_root != v_root || self.edges[u_merge].contains(&v_merge) {
            if patience == 0 {
                return false;
            }
            let (u_merge, v_merge) = self.choose_short_edge();
            return self.replace_edge_in_tree(patience - 1, u_merge, v_merge);
        }

        let (mut profit, s, t) = self.calc_cycle_max_edge(u_merge, v_merge);
        profit -= self.get_dist(u_merge, v_merge);

        if !self.scheduler.accept(profit as f64) {
            // reject
            return false;
        }

        // accept
        self.split(s, t);
        self.merge(u_merge, v_merge);
        return true;
    }

    fn merge(&mut self, mut u: usize, mut v: usize) {
        debug_assert!(u < NUM_VERTICES);
        debug_assert!(v < NUM_VERTICES);
        debug_assert_ne!(u, v);
        let mut u_root = self.roots[u];
        let mut v_root = self.roots[v];
        debug_assert_ne!(u_root, v_root);
        let mut u_size = self.sizes[u_root];
        let mut v_size = self.sizes[v_root];
        if u_size < v_size {
            std::mem::swap(&mut u, &mut v);
            std::mem::swap(&mut u_root, &mut v_root);
            std::mem::swap(&mut u_size, &mut v_size);
        }
        // merge v to u

        self.update_subtree(v, u_root);
        self.parents[v] = u;

        let mut x = u;
        while x != !0 {
            self.sizes[x] += v_size;
            x = self.parents[x];
        }

        self.edges[u].push(v);
        self.edges[v].push(u);
        self.raw_cost += self.get_dist(u, v);
    }

    fn split(&mut self, mut u: usize, mut v: usize) {
        if self.parents[u] == v {
            std::mem::swap(&mut u, &mut v);
        }
        debug_assert_eq!(self.parents[v], u);

        // split v from u

        debug_assert!(self.edges[u].contains(&v));
        debug_assert!(self.edges[v].contains(&u));
        self.edges[u].retain(|&x| x != v);
        self.edges[v].retain(|&x| x != u);

        let root = self.roots[u];
        debug_assert_eq!(self.roots[v], root);

        let v_size = self.sizes[v];

        self.update_subtree(v, v);

        let mut x = u;
        while x != !0 {
            self.sizes[x] -= v_size;
            x = self.parents[x];
        }

        self.raw_cost -= self.get_dist(u, v);
    }

    fn calc_merge_d_group_cost(&mut self, u_size: usize, v_size: usize) -> i64 {
        debug_assert!(u_size > 0);
        debug_assert!(v_size > 0);

        if u_size == v_size {
            let uc = self.group_counts[u_size] as i64;
            let mut d_group_cost = 4 - 4 * uc;
            let wc = self.group_counts[u_size + v_size] as i64;
            d_group_cost += 2 * wc + 1;
            d_group_cost
        } else {
            let uc = self.group_counts[u_size] as i64;
            let vc = self.group_counts[v_size] as i64;
            let mut d_group_cost = 2 - 2 * uc - 2 * vc;
            let wc = self.group_counts[u_size + v_size] as i64;
            d_group_cost += 2 * wc + 1;
            d_group_cost
        }
    }

    fn calc_split_d_group_cost(&mut self, u_size: usize, v_size: usize) -> i64 {
        debug_assert!(u_size > 0);
        debug_assert!(v_size > 0);

        if u_size == v_size {
            let uc = self.group_counts[u_size] as i64;
            let mut d_group_cost = 4 * uc + 4;
            let wc = self.group_counts[u_size + v_size] as i64;
            d_group_cost += 1 - 2 * wc;
            d_group_cost
        } else {
            let uc = self.group_counts[u_size] as i64;
            let vc = self.group_counts[v_size] as i64;
            let mut d_group_cost = 2 * uc + 2 * vc + 2;
            let wc = self.group_counts[u_size + v_size] as i64;
            d_group_cost += 1 - 2 * wc;
            d_group_cost
        }
    }

    /// update roots, parents, sizes
    fn update_subtree(&mut self, u: usize, root: usize) {
        // DFS
        self.roots[u] = root;
        self.parents[u] = !0;
        let mut que = vec![u];
        let mut i = 0;
        while i < que.len() {
            let u = que[i];
            i += 1;
            let parent = self.parents[u];
            self.sizes[u] = 1;
            for &v in &self.edges[u] {
                if v == parent {
                    continue;
                }
                self.roots[v] = root;
                self.parents[v] = u;
                que.push(v);
            }
        }
        for &v in que[1..].iter().rev() {
            self.sizes[self.parents[v]] += self.sizes[v];
        }
    }

    fn choose_short_edge(&mut self) -> (usize, usize) {
        let &(u, v) = self.short_edges.choose(&mut self.rng).unwrap();
        (u as usize, v as usize)
    }

    fn calc_cycle_max_edge(&self, mut u: usize, mut v: usize) -> (i64, usize, usize) {
        debug_assert_eq!(self.roots[u], self.roots[v]);
        let mut ret = (i64::MIN, 0, 0);
        let mut u_size = self.sizes[u];
        let mut v_size = self.sizes[v];
        while u != v {
            if u_size < v_size {
                let p = self.parents[u];
                let dist = self.get_dist(u, p);
                ret.chmax((dist, u, p));
                u = p;
                u_size = self.sizes[u];
            } else {
                let p = self.parents[v];
                let dist = self.get_dist(v, p);
                ret.chmax((dist, v, p));
                v = p;
                v_size = self.sizes[v];
            }
        }
        ret
    }

    fn get_dist(&self, u: usize, v: usize) -> i64 {
        debug_assert!(u < NUM_VERTICES);
        debug_assert!(v < NUM_VERTICES);
        self.distances[u][v] as i64
    }

    fn make_output(&self) -> Output {
        debug_assert_eq!(self.group_cost, 0);
        debug_assert_eq!(self.group_counts, [0; NUM_VERTICES + 1]);

        let mut group_ids = vec![vec![]; NUM_VERTICES + 1];
        for i in 0..self.num_vertices_in_group.len() {
            group_ids[self.num_vertices_in_group[i]].push(i);
        }
        let mut root_to_group = [!0; NUM_VERTICES];
        for root in 0..NUM_VERTICES {
            if self.roots[root] != root {
                continue;
            }
            root_to_group[root] = group_ids[self.sizes[root] as usize].pop().unwrap();
        }

        let mut groups = vec![vec![]; self.num_vertices_in_group.len()];
        for v in 0..NUM_VERTICES {
            groups[root_to_group[self.roots[v]]].push(v);
        }

        let mut edges = vec![vec![]; self.num_vertices_in_group.len()];
        let mut cost = 0;
        let mut dsu = DisjointSetUnion::new(NUM_VERTICES);
        let mut num_groups = NUM_VERTICES;
        for &duv in &self.sorted_edges {
            let (dist, u, v) = decode3(duv, NV_BW, NV_BW);
            let root = self.roots[u];
            if root != self.roots[v] {
                continue;
            }
            if !dsu.merge(u, v) {
                continue;
            }
            edges[root_to_group[root]].push((u, v));
            cost += dist as i64;
            num_groups -= 1;
            if num_groups == self.num_vertices_in_group.len() {
                break;
            }
        }

        Output {
            groups,
            edges,
            cost,
        }
    }
}

#[derive(Debug, Clone)]
struct Rectangle {
    lx: f32,
    rx: f32,
    ly: f32,
    ry: f32,
}

impl Rectangle {
    #[inline(always)]
    fn sample_around(&self, rng: &mut XorShift32, x: f32, y: f32, r: f32) -> (f32, f32) {
        let x_min = (x - r).max(self.lx);
        let x_max = (x + r).min(self.rx);
        let y_min = (y - r).max(self.ly);
        let y_max = (y + r).min(self.ry);

        let x = x_min + rng.gen_range_f32(x_max - x_min);
        let y = y_min + rng.gen_range_f32(y_max - y_min);
        (x, y)
    }

    fn get_by_theta(&self, theta: f32) -> (f32, f32) {
        let cx = 0.5 * (self.lx + self.rx);
        let cy = 0.5 * (self.ly + self.ry);
        let dx = self.rx - cx;
        let dy = self.ry - cy;
        let x = cx + INIT_SAMPLE_R * dx * theta.cos();
        let y = cy + INIT_SAMPLE_R * dy * theta.sin();
        (x, y)
    }

    fn area(&self) -> f32 {
        (self.rx - self.lx) * (self.ry - self.ly)
    }

    fn clamp(&self, x: f32, y: f32) -> (f32, f32) {
        (x.clamp(self.lx, self.rx), y.clamp(self.ly, self.ry))
    }
}

#[derive(Debug, Clone)]
struct Input {
    num_groups: usize,
    max_vertices_in_query: usize,
    max_rect_width: usize,
    num_vertices_in_group: Vec<usize>,
    rects: [Rectangle; NUM_VERTICES],
}

fn read_input() -> Input {
    input_interactive! {
        num_vertices: usize,
        num_groups: usize,
        num_queries: usize,
        max_vertices_in_query: usize,
        max_rect_width: usize,
        num_vertices_in_group: [usize; num_groups],
        rects: [(f32, f32, f32, f32); num_vertices],
    }
    debug_assert_eq!(num_vertices, NUM_VERTICES);
    debug_assert_eq!(num_queries, NUM_QUERIES);
    let rects = rects
        .into_iter()
        .map(|(lx, rx, ly, ry)| Rectangle { lx, rx, ly, ry })
        .collect_vec();
    Input {
        num_groups,
        max_vertices_in_query,
        max_rect_width,
        num_vertices_in_group,
        rects: rects.try_into().unwrap(),
    }
}

#[cfg(target_arch = "x86_64")]
#[target_feature(enable = "avx2")]
unsafe fn euclid2_avx(ux: __m256, uy: __m256, vx: __m256, vy: __m256) -> __m256 {
    let dx = _mm256_sub_ps(ux, vx);
    let dy = _mm256_sub_ps(uy, vy);
    let dx2 = _mm256_mul_ps(dx, dx);
    let dy2 = _mm256_mul_ps(dy, dy);
    _mm256_add_ps(dx2, dy2)
}

#[inline(always)]
fn euclid2(ux: f32, uy: f32, vx: f32, vy: f32) -> f32 {
    let dx = ux - vx;
    let dy = uy - vy;
    dx * dx + dy * dy
}

#[inline(always)]
fn euclid(ux: f32, uy: f32, vx: f32, vy: f32) -> f32 {
    euclid2(ux, uy, vx, vy).sqrt()
}

#[inline(always)]
fn encode2(a: usize, b: usize, bw: usize) -> usize {
    debug_assert!(b < (1 << bw));
    (a << bw) | b
}

#[inline(always)]
fn decode2(ab: usize, bw: usize) -> (usize, usize) {
    let a = ab >> bw;
    let b = ab & ((1 << bw) - 1);
    (a, b)
}

#[inline(always)]
fn encode3(a: usize, b: usize, c: usize, bw: usize, cw: usize) -> usize {
    debug_assert!(b < (1 << bw));
    debug_assert!(c < (1 << cw));
    (a << (bw + cw)) | (b << cw) | c
}

#[inline(always)]
fn decode3(abc: usize, bw: usize, cw: usize) -> (usize, usize, usize) {
    let a = abc >> (bw + cw);
    let b = (abc >> cw) & ((1 << bw) - 1);
    let c = abc & ((1 << cw) - 1);
    (a, b, c)
}

#[inline(always)]
fn encode4(a: usize, b: usize, c: usize, d: usize, bw: usize, cw: usize, dw: usize) -> usize {
    debug_assert!(b < (1 << bw));
    debug_assert!(c < (1 << cw));
    debug_assert!(d < (1 << dw));
    (a << (bw + cw + dw)) | (b << (cw + dw)) | (c << dw) | d
}

#[inline(always)]
fn decode4(abcd: usize, bw: usize, cw: usize, dw: usize) -> (usize, usize, usize, usize) {
    let a = abcd >> (bw + cw + dw);
    let b = (abcd >> (cw + dw)) & ((1 << bw) - 1);
    let c = (abcd >> dw) & ((1 << cw) - 1);
    let d = abcd & ((1 << dw) - 1);
    (a, b, c, d)
}

// size: bytes
// align: bytes
// return empty vector
pub fn aligned_alloc<T>(size: usize, align: usize) -> Vec<T> {
    let capacity = size / std::mem::size_of::<T>();
    unsafe {
        let layout = std::alloc::Layout::from_size_align_unchecked(size, align);
        let ptr = std::alloc::alloc(layout);
        Vec::from_raw_parts(ptr as *mut T, 0, capacity)
    }
}

#[derive(Debug, Clone)]
pub struct XorShift32 {
    state: u32,
}

impl XorShift32 {
    pub fn new(seed: u32) -> Self {
        debug_assert!(seed != 0, "Seed must be non-zero");
        Self { state: seed }
    }

    #[inline(always)]
    pub fn next_u32(&mut self) -> u32 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.state = x;
        x
    }

    #[inline(always)]
    pub fn gen_range(&mut self, n: u64) -> u64 {
        (n * self.next_u32() as u64) >> 32
    }

    #[inline(always)]
    pub fn gen_f32(&mut self) -> f32 {
        self.next_u32() as f32 / ((1u64 << 32) as f32)
    }

    #[inline(always)]
    pub fn gen_range_f32(&mut self, r: f32) -> f32 {
        self.next_u32() as f32 * (r / ((1u64 << 32) as f32))
    }
}

#[derive(Debug, Clone)]
pub struct DisjointSetUnion {
    n: usize,
    parent_or_size: Vec<i32>,
}

impl DisjointSetUnion {
    pub fn new(n: usize) -> Self {
        DisjointSetUnion {
            n,
            parent_or_size: vec![-1; n],
        }
    }

    pub fn merge(&mut self, a: usize, b: usize) -> bool {
        debug_assert!(a < self.n);
        debug_assert!(b < self.n);
        let mut x = self.leader(a);
        let mut y = self.leader(b);
        if x == y {
            return false;
        }
        if -self.parent_or_size[x] < -self.parent_or_size[y] {
            std::mem::swap(&mut x, &mut y);
        }
        self.parent_or_size[x] += self.parent_or_size[y];
        self.parent_or_size[y] = x as i32;
        true
    }

    pub fn same(&mut self, a: usize, b: usize) -> bool {
        debug_assert!(a < self.n);
        debug_assert!(b < self.n);
        self.leader(a) == self.leader(b)
    }

    pub fn leader(&mut self, a: usize) -> usize {
        debug_assert!(a < self.n);
        if self.parent_or_size[a] < 0 {
            return a;
        }
        let root = self.leader(self.parent_or_size[a] as usize);
        self.parent_or_size[a] = root as i32;
        root
    }

    pub fn size(&mut self, a: usize) -> usize {
        let x = self.leader(a);
        (-self.parent_or_size[x]) as usize
    }

    pub fn groups(&mut self) -> Vec<Vec<usize>> {
        let mut leader_buf = vec![0; self.n];
        let mut group_size = vec![0; self.n];
        for i in 0..self.n {
            leader_buf[i] = self.leader(i);
            group_size[leader_buf[i]] += 1;
        }
        let mut result = vec![vec![]; self.n];
        for i in 0..self.n {
            result[i].reserve(group_size[i]);
        }
        for i in 0..self.n {
            result[leader_buf[i]].push(i);
        }
        result.retain(|v| !v.is_empty());
        result
    }

    pub fn clear(&mut self, indices: &[usize]) {
        for &a in indices {
            debug_assert!(a < self.n);
            self.parent_or_size[a] = -1;
        }
    }
}

const SA_TIME_COUNTS: usize = 1 << 4;
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

        let mut rng = Pcg64Mcg::seed_from_u64(0);
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
        let mut progress = (get_time_sec() - self.start_time_sec) / self.duration_sec;
        progress = progress.min(1.0);
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
        #[cfg(feature = "local")]
        {
            ms - STIME
        }
        #[cfg(not(feature = "local"))]
        {
            ms - STIME
        }
    }
}
