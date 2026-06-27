use std::{cmp::min, io::BufReader};

use once_cell::sync::Lazy;
use proconio::{input, source::once::OnceSource};

use crate::{common_traits::ToAnswer, input::read_input, my_clap::Parse};

fn main() {
    let result = solve();
    println!("{}", result.to_answer());
}

fn solve() -> Output {
    // Build initial DAG and optimize with SA; fall back to baseline if needed
    let mut state = DAGState::new();
    let sa = simulated_annealing::SimulatedAnnealing::<simulated_annealing::Verbose>::new(
        A.max_temprature, // max T (from CLI)
        0.0,              // min T
        0.95,             // record best from start
        std::time::Duration::from_millis(1990),
    );
    let mut stats = simulated_annealing::BasicStatistics::default();
    let result = sa.optimize(&mut state, &mut stats);
    if result.ops.is_empty() {
        // Fallback to baseline
        return solve2();
    }
    eprintln!("{stats:#?}");
    eprintln!(
        "Param:Hsum = {}",
        P.h.iter().map(|&x| x as i64).sum::<i64>()
    );
    eprintln!("Param:turn = {}", result.ops.len());
    // eprintln!(
    // "A[*][142] = {:?}",
    // P.a.iter().map(|x| x[142]).collect::<Vec<_>>()
    // );
    result
}

fn solve2() -> Output {
    // Strategy:
    // - Always pick the unopened treasure with the lowest remaining HP and open it by hand.
    // - When a chest opens, immediately use its weapon once on the unopened chest that
    //   receives the maximum damage. If that attack opens another chest, chain the same
    //   procedure with the newly opened chest. Repeat until no new chest opens.
    let n = P.n;
    let mut ops = Vec::new();
    let mut hp = P.h.clone();
    let mut opened = vec![false; n];
    let mut remain_c = P.c.clone();

    // Helper to pick unopened with minimal hp
    let mut unopened_cnt = n;
    while unopened_cnt > 0 {
        // pick min-hp unopened
        let mut best_j: Option<usize> = None;
        let mut best_hp = i32::MAX;
        for j in 0..n {
            if !opened[j] && hp[j] < best_hp {
                best_hp = hp[j];
                best_j = Some(j);
            }
        }
        let j = best_j.expect("there should be unopened chests");

        // Open j by hands
        while hp[j] > 0 {
            ops.push((-1, j));
            hp[j] -= 1;
        }
        if !opened[j] {
            opened[j] = true;
            unopened_cnt -= 1;
        }

        // Process chain: queue of newly opened chests
        let mut stack = vec![j];
        while let Some(i) = stack.pop() {
            // Use weapon i as much as possible, chaining on newly opened chests immediately
            while remain_c[i] > 0 {
                // choose best target among unopened
                let mut target: Option<usize> = None;
                let mut best_dmg = -1;
                for b in 0..n {
                    if !opened[b] {
                        let dmg = min(P.a[i][b], hp[b]);
                        if dmg > best_dmg {
                            best_dmg = dmg;
                            target = Some(b);
                        }
                    }
                }
                let Some(b) = target else { break };
                // attack
                ops.push((i as i32, b));
                remain_c[i] -= 1;
                hp[b] -= P.a[i][b];
                if hp[b] <= 0 && !opened[b] {
                    opened[b] = true;
                    unopened_cnt -= 1;
                    // Depth-first chaining: process newly opened chest next
                    stack.push(b);
                }
            }
        }
    }

    Output { ops }
}

#[derive(Debug)]
pub struct Input {
    pub n: usize,
    pub h: Vec<i32>,
    pub c: Vec<i32>,
    pub a: Vec<Vec<i32>>, // damage matrix
    // Precomputed for speed: for each i, targets sorted by A[i][b] desc
    pub a_sorted_idx: Vec<Vec<usize>>, // indices b sorted by descending A[i][b]
    pub a_sorted_val: Vec<Vec<i32>>,   // corresponding sorted values
}

#[derive(Debug)]
pub struct Output {
    pub ops: Vec<(i32, usize)>, // (W, B) where W=-1 means hand
}

// A minimal DAG representation for tasks in docs/tasks.md
// - `topo`: a topological order of vertex indices (0..N)
// - `edges[i]`: list of (target_b, times_w) from weapon/node i to treasure/node b
#[derive(Debug, Clone)]
struct Dag {
    topo: Vec<usize>,
    edges: Vec<Vec<(usize, usize)>>,
}

// Build an initial DAG by simulating `solve2` and aggregating weapon attacks
fn build_initial_dag() -> Dag {
    let n = P.n;
    let mut hp = P.h.clone();
    let mut opened = vec![false; n];
    let mut remain_c = P.c.clone();
    let mut topo = Vec::with_capacity(n);
    let mut cnt = vec![vec![0usize; n]; n];

    let mut unopened_cnt = n;
    while unopened_cnt > 0 {
        // pick min-hp unopened
        let mut best_j: Option<usize> = None;
        let mut best_hp = i32::MAX;
        for j in 0..n {
            if !opened[j] && hp[j] < best_hp {
                best_hp = hp[j];
                best_j = Some(j);
            }
        }
        let j = best_j.expect("there should be unopened chests");

        // Open j by hands
        while hp[j] > 0 {
            hp[j] -= 1;
        }
        if !opened[j] {
            opened[j] = true;
            unopened_cnt -= 1;
            topo.push(j);
        }

        // Process chain: queue of newly opened chests
        let mut stack = vec![j];
        while let Some(i) = stack.pop() {
            while remain_c[i] > 0 {
                // choose best target among unopened
                let mut target: Option<usize> = None;
                let mut best_dmg = -1;
                for b in 0..n {
                    if !opened[b] {
                        let dmg = min(P.a[i][b], hp[b]);
                        if dmg > best_dmg {
                            best_dmg = dmg;
                            target = Some(b);
                        }
                    }
                }
                let Some(b) = target else { break };
                // attack
                remain_c[i] -= 1;
                cnt[i][b] += 1;
                hp[b] -= P.a[i][b];
                if hp[b] <= 0 && !opened[b] {
                    opened[b] = true;
                    unopened_cnt -= 1;
                    topo.push(b);
                    stack.push(b);
                }
            }
        }
    }

    // Build adjacency list from counts
    let mut edges = vec![Vec::<(usize, usize)>::new(); n];
    for i in 0..n {
        for b in 0..n {
            let w = cnt[i][b];
            if w > 0 {
                edges[i].push((b, w));
            }
        }
    }

    Dag { topo, edges }
}

// Compute a topological order from current edges; returns None if cyclic
fn compute_topo_from_edges(dag: &Dag) -> Option<Vec<usize>> {
    let n = P.n;
    if dag.edges.len() != n {
        return None;
    }
    let mut indeg = vec![0usize; n];
    for i in 0..n {
        for &(b, w) in &dag.edges[i] {
            if w == 0 {
                continue;
            }
            indeg[b] += 1;
        }
    }
    let mut q: std::collections::VecDeque<usize> = (0..n).filter(|&v| indeg[v] == 0).collect();
    let mut order = Vec::with_capacity(n);
    while let Some(v) = q.pop_front() {
        order.push(v);
        for &(to, _) in &dag.edges[v] {
            indeg[to] -= 1;
            if indeg[to] == 0 {
                q.push_back(to);
            }
        }
    }
    if order.len() == n {
        Some(order)
    } else {
        None
    }
}

// Restore a valid attack sequence from a DAG using current edges
fn dag_to_output(dag: &Dag) -> Output {
    let n = P.n;
    let mut hp = P.h.clone();
    let mut opened = vec![false; n];
    let mut remain_c = P.c.clone();
    let mut ops: Vec<(i32, usize)> = Vec::new();

    let order = match compute_topo_from_edges(dag) {
        Some(o) => o,
        None => return Output { ops },
    };

    for &i in &order {
        // Ensure chest i is opened (by hands as needed)
        while hp[i] > 0 {
            ops.push((-1, i));
            hp[i] -= 1;
        }
        opened[i] = true;

        // Use weapon i according to edges
        if i < dag.edges.len() {
            for &(b, w) in &dag.edges[i] {
                for _ in 0..w {
                    if remain_c[i] == 0 {
                        break;
                    }
                    // Do not attack an already-opened chest
                    if opened[b] {
                        break;
                    }
                    ops.push((i as i32, b));
                    remain_c[i] -= 1;
                    hp[b] -= P.a[i][b];
                    if hp[b] <= 0 && !opened[b] {
                        opened[b] = true;
                    }
                }
            }
        }
    }

    Output { ops }
}

// O(N+M) evaluation for a DAG: returns contest score (higher is better)
fn eval_dag_score(dag: &Dag) -> f64 {
    let n = P.n;
    if dag.edges.len() != n {
        return f64::NEG_INFINITY;
    }
    let mut s1_attacks: i64 = 0;
    let mut incoming_damage = vec![0i64; n];
    let mut used_per_weapon = vec![0i64; n];

    for i in 0..n {
        if i >= dag.edges.len() {
            return f64::NEG_INFINITY;
        }
        for &(b, w) in &dag.edges[i] {
            if b >= n {
                return f64::NEG_INFINITY;
            }
            let w_i64 = w as i64;
            s1_attacks += w_i64;
            used_per_weapon[i] += w_i64;
            incoming_damage[b] += w_i64 * P.a[i][b] as i64;
        }
        if used_per_weapon[i] > P.c[i] as i64 {
            return f64::NEG_INFINITY;
        }
    }

    let mut s2_hands: i64 = 0;
    for b in 0..n {
        let need = P.h[b] as i64 - incoming_damage[b];
        if need > 0 {
            s2_hands += need;
        }
    }
    let t_total = s1_attacks + s2_hands;
    let sum_h: i64 = P.h.iter().map(|&x| x as i64).sum();
    (sum_h - t_total + 1) as f64
}

// Helper: mark ancestors (nodes that can reach `target`) using prebuilt reverse edges
fn mark_ancestors_with_rev(
    rev: &Vec<Vec<usize>>,
    target: usize,
    vis: &mut cleanable_vec::CleanableVecBool,
    stack_buf: &mut [usize],
) {
    vis.clear();
    // Manual stack using preallocated buffer to avoid push/pop realloc
    let mut top: usize = 0;
    debug_assert!(top < stack_buf.len());
    stack_buf[top] = target;
    top += 1;
    while top > 0 {
        top -= 1;
        let v = stack_buf[top];
        for &p in &rev[v] {
            if !vis.is_visited(p) {
                vis.visit(p);
                debug_assert!(top < stack_buf.len());
                stack_buf[top] = p;
                top += 1;
            }
        }
    }
}

// Move: pick i and (i->b1) then decrement 1, and increment 1 to (i->b2)
// where b2 is chosen from top-10 A[i][b] with condition: adding i->b2 does not create a cycle.
fn random_move_preserve_weapon_sum(
    dag: &mut Dag,
    rng: &mut xorshift::XorShift,
    anc_vis: &mut cleanable_vec::CleanableVecBool,
    rev: &mut Vec<Vec<usize>>,
    stack_buf: &mut [usize],
) -> Option<(usize, usize, usize)> {
    let n = P.n;
    if dag.edges.len() != n {
        return None;
    }

    // collect candidates i with at least one outgoing edge
    let mut i = usize::MAX;
    loop {
        i = rng.gen_usize(n);
        if !dag.edges[i].is_empty() {
            break;
        }
    }

    // pick an existing edge to decrement
    let e_len = dag.edges[i].len();
    let e_idx = rng.gen_usize(e_len);
    let (b1, _) = dag.edges[i][e_idx];

    // decrement 1
    if dag.edges[i][e_idx].1 > 1 {
        dag.edges[i][e_idx].1 -= 1;
    } else {
        dag.edges[i].swap_remove(e_idx);
        // update rev: remove parent i from b1
        if let Some(pos) = rev[b1].iter().position(|&p| p == i) {
            rev[b1].swap_remove(pos);
        }
    }

    // build candidate b2 set using rev and precomputed sorted order; take top-K feasible
    mark_ancestors_with_rev(rev, i, anc_vis, stack_buf);
    let top_k: usize = A.TOP_K;
    let mut top_candidates: Vec<usize> = Vec::with_capacity(top_k);
    for &b in &P.a_sorted_idx[i] {
        if b != i && b != b1 && !anc_vis.is_visited(b) {
            top_candidates.push(b);
            if top_candidates.len() == top_k {
                break;
            }
        }
    }
    if top_candidates.is_empty() {
        // revert decrement
        // add back to b1
        if let Some(edge) = dag.edges[i].iter_mut().find(|e| e.0 == b1) {
            edge.1 += 1;
        } else {
            dag.edges[i].push((b1, 1));
            rev[b1].push(i);
        }
        return None;
    }
    let b2 = top_candidates[rng.gen_usize(top_candidates.len())];

    // increment to b2
    if let Some(edge) = dag.edges[i].iter_mut().find(|e| e.0 == b2) {
        edge.1 += 1;
    } else {
        dag.edges[i].push((b2, 1));
        rev[b2].push(i);
    }

    Some((i, b1, b2))
}

// New transition: pick two vertices i!=j, pick one existing edge from each: (i->b1), (j->b2).
// Swap one unit of weight between them: decrement those, and increment (i->b2) and (j->b1),
// ensuring DAG acyclicity via ancestor checks using rev. Returns (i,j,b1,b2) on success.
fn random_move_swap_two_edges(
    dag: &mut Dag,
    rng: &mut xorshift::XorShift,
    anc_vis: &mut cleanable_vec::CleanableVecBool,
    rev: &mut Vec<Vec<usize>>,
    stack_buf: &mut [usize],
) -> Option<(usize, usize, usize, usize)> {
    let n = P.n;
    if dag.edges.len() != n {
        return None;
    }
    // collect candidates with at least one outgoing edge
    let mut cands: Vec<usize> = (0..n).filter(|&u| !dag.edges[u].is_empty()).collect();
    if cands.len() < 2 {
        return None;
    }
    // choose distinct i, j
    let idx_i = rng.gen_usize(cands.len());
    let i = cands[idx_i];
    cands.swap_remove(idx_i);
    let j = cands[rng.gen_usize(cands.len())];

    // pick one edge from each
    let e_i = rng.gen_usize(dag.edges[i].len());
    let b1 = dag.edges[i][e_i].0;
    let e_j = rng.gen_usize(dag.edges[j].len());
    let b2 = dag.edges[j][e_j].0;

    // decrement (i->b1)
    if dag.edges[i][e_i].1 > 1 {
        dag.edges[i][e_i].1 -= 1;
    } else {
        dag.edges[i].swap_remove(e_i);
        if let Some(pos) = rev[b1].iter().position(|&p| p == i) {
            rev[b1].swap_remove(pos);
        }
    }
    // decrement (j->b2)
    // Note: if i==j (should not happen), we already ensured distinct
    let e_j_len = dag.edges[j].len();
    let mut removed_parent_jb2 = false;
    let mut dec_j_b2_done = false;
    for idx in 0..e_j_len {
        if dag.edges[j][idx].0 == b2 {
            if dag.edges[j][idx].1 > 1 {
                dag.edges[j][idx].1 -= 1;
            } else {
                dag.edges[j].swap_remove(idx);
                if let Some(pos) = rev[b2].iter().position(|&p| p == j) {
                    rev[b2].swap_remove(pos);
                }
                removed_parent_jb2 = true;
            }
            dec_j_b2_done = true;
            break;
        }
    }
    if !dec_j_b2_done {
        // revert i->b1 and abort
        if let Some(edge) = dag.edges[i].iter_mut().find(|e| e.0 == b1) {
            edge.1 += 1;
        } else {
            dag.edges[i].push((b1, 1));
            rev[b1].push(i);
        }
        return None;
    }

    // Check cycles for adding (i->b2) and (j->b1)
    mark_ancestors_with_rev(rev, i, anc_vis, stack_buf);
    if anc_vis.is_visited(b2) || b2 == i {
        // revert decrements
        if let Some(edge) = dag.edges[i].iter_mut().find(|e| e.0 == b1) {
            edge.1 += 1;
        } else {
            dag.edges[i].push((b1, 1));
            rev[b1].push(i);
        }
        if let Some(edge) = dag.edges[j].iter_mut().find(|e| e.0 == b2) {
            edge.1 += 1;
        } else {
            dag.edges[j].push((b2, 1));
            if removed_parent_jb2 {
                rev[b2].push(j);
            }
        }
        return None;
    }
    mark_ancestors_with_rev(rev, j, anc_vis, stack_buf);
    if anc_vis.is_visited(b1) || b1 == j {
        // revert decrements
        if let Some(edge) = dag.edges[i].iter_mut().find(|e| e.0 == b1) {
            edge.1 += 1;
        } else {
            dag.edges[i].push((b1, 1));
            rev[b1].push(i);
        }
        if let Some(edge) = dag.edges[j].iter_mut().find(|e| e.0 == b2) {
            edge.1 += 1;
        } else {
            dag.edges[j].push((b2, 1));
            if removed_parent_jb2 {
                rev[b2].push(j);
            }
        }
        return None;
    }

    // increment (i->b2)
    if let Some(edge) = dag.edges[i].iter_mut().find(|e| e.0 == b2) {
        edge.1 += 1;
    } else {
        dag.edges[i].push((b2, 1));
        rev[b2].push(i);
    }
    // increment (j->b1)
    if let Some(edge) = dag.edges[j].iter_mut().find(|e| e.0 == b1) {
        edge.1 += 1;
    } else {
        dag.edges[j].push((b1, 1));
        rev[b1].push(j);
    }

    Some((i, j, b1, b2))
}

// SA state over DAG
struct DAGState {
    dag: Dag,
    rng: xorshift::XorShift,
    current_score: f64,
    anc_vis: cleanable_vec::CleanableVecBool,
    rev: Vec<Vec<usize>>,
    incoming_damage: Vec<i64>,
    stack_buf: Vec<usize>,
}

impl DAGState {
    fn new() -> Self {
        let mut state = DAGState {
            dag: build_initial_dag(),
            rng: xorshift::XorShift::seed_from_u64((A.seed as u64) ^ 0x9E3779B97F4A7C15),
            current_score: 0.0,
            anc_vis: cleanable_vec::CleanableVecBool::create(P.n),
            rev: vec![Vec::new(); P.n],
            incoming_damage: vec![0; P.n],
            stack_buf: vec![0; P.n],
        };
        // build reverse adjacency and incoming damage; compute initial score fast
        let n = P.n;
        let mut sum_w: i64 = 0;
        for i in 0..n {
            for &(b, w) in &state.dag.edges[i] {
                if w > 0 {
                    state.rev[b].push(i);
                    sum_w += w as i64;
                    state.incoming_damage[b] += w as i64 * P.a[i][b] as i64;
                }
            }
        }
        let mut hand_sum: i64 = 0;
        for b in 0..n {
            let need = P.h[b] as i64 - state.incoming_damage[b];
            if need > 0 {
                hand_sum += need;
            }
        }
        let sum_h: i64 = P.h.iter().map(|&x| x as i64).sum();
        let t_total = sum_w + hand_sum;
        state.current_score = (sum_h - t_total + 1) as f64;
        state
    }

    fn transit0(
        &mut self,
        statistics: &mut simulated_annealing::BasicStatistics,
        temperature: f64,
        _progress: f64,
    ) {
        let Some((i, b1, b2)) = random_move_preserve_weapon_sum(
            &mut self.dag,
            &mut self.rng,
            &mut self.anc_vis,
            &mut self.rev,
            &mut self.stack_buf,
        ) else {
            return;
        };
        // Differential evaluation on b1 and b2 only
        let delta1 = -(P.a[i][b1] as i64);
        let delta2 = P.a[i][b2] as i64;
        let f_before1 = 0.max(P.h[b1] as i64 - self.incoming_damage[b1]);
        let f_after1 = 0.max(P.h[b1] as i64 - (self.incoming_damage[b1] + delta1));
        let f_before2 = 0.max(P.h[b2] as i64 - self.incoming_damage[b2]);
        let f_after2 = 0.max(P.h[b2] as i64 - (self.incoming_damage[b2] + delta2));
        let delta_t = (f_after1 - f_before1) + (f_after2 - f_before2);
        let next_score = self.current_score - delta_t as f64;
        let decision = <Self as simulated_annealing::SAState<Output>>::should_transit(
            next_score,
            self.current_score,
            temperature,
            &mut self.rng,
        );
        use simulated_annealing::TransitionType;
        match decision {
            TransitionType::AcceptWithBetter | TransitionType::AcceptWithWorse => {
                // apply incoming updates
                self.incoming_damage[b1] += delta1;
                self.incoming_damage[b2] += delta2;
                self.current_score = next_score;
                simulated_annealing::Statistics::transition_update(statistics, 0, decision);
            }
            TransitionType::Reject => {
                // revert: decrement (i,b2), increment (i,b1)
                // dec b2
                if let Some(edge) = self.dag.edges[i].iter_mut().find(|e| e.0 == b2) {
                    if edge.1 > 1 {
                        edge.1 -= 1;
                    } else {
                        // remove
                        let idx = self.dag.edges[i].iter().position(|e| e.0 == b2).unwrap();
                        self.dag.edges[i].swap_remove(idx);
                        if let Some(pos) = self.rev[b2].iter().position(|&p| p == i) {
                            self.rev[b2].swap_remove(pos);
                        }
                    }
                }
                // inc b1
                if let Some(edge) = self.dag.edges[i].iter_mut().find(|e| e.0 == b1) {
                    edge.1 += 1;
                } else {
                    self.dag.edges[i].push((b1, 1));
                    self.rev[b1].push(i);
                }
                simulated_annealing::Statistics::transition_update(statistics, 0, decision);
            }
        }
    }

    fn transit1(
        &mut self,
        statistics: &mut simulated_annealing::BasicStatistics,
        temperature: f64,
        _progress: f64,
    ) {
        let Some((i, j, b1, b2)) = random_move_swap_two_edges(
            &mut self.dag,
            &mut self.rng,
            &mut self.anc_vis,
            &mut self.rev,
            &mut self.stack_buf,
        ) else {
            return;
        };
        // Net deltas on targets b1 and b2 due to swap
        let delta_b1 = (P.a[j][b1] as i64) - (P.a[i][b1] as i64);
        let delta_b2 = (P.a[i][b2] as i64) - (P.a[j][b2] as i64);
        let f_b1_before = 0.max(P.h[b1] as i64 - self.incoming_damage[b1]);
        let f_b1_after = 0.max(P.h[b1] as i64 - (self.incoming_damage[b1] + delta_b1));
        let f_b2_before = 0.max(P.h[b2] as i64 - self.incoming_damage[b2]);
        let f_b2_after = 0.max(P.h[b2] as i64 - (self.incoming_damage[b2] + delta_b2));
        let delta_t = (f_b1_after - f_b1_before) + (f_b2_after - f_b2_before);
        let next_score = self.current_score - delta_t as f64;
        let decision = <Self as simulated_annealing::SAState<Output>>::should_transit(
            next_score,
            self.current_score,
            temperature,
            &mut self.rng,
        );
        use simulated_annealing::TransitionType;
        match decision {
            TransitionType::AcceptWithBetter | TransitionType::AcceptWithWorse => {
                self.incoming_damage[b1] += delta_b1;
                self.incoming_damage[b2] += delta_b2;
                self.current_score = next_score;
                simulated_annealing::Statistics::transition_update(statistics, 1, decision);
            }
            TransitionType::Reject => {
                // Revert increments: (i->b2) and (j->b1)
                if let Some(edge) = self.dag.edges[i].iter_mut().find(|e| e.0 == b2) {
                    if edge.1 > 1 {
                        edge.1 -= 1;
                    } else {
                        let idx = self.dag.edges[i].iter().position(|e| e.0 == b2).unwrap();
                        self.dag.edges[i].swap_remove(idx);
                        if let Some(pos) = self.rev[b2].iter().position(|&p| p == i) {
                            self.rev[b2].swap_remove(pos);
                        }
                    }
                }
                if let Some(edge) = self.dag.edges[j].iter_mut().find(|e| e.0 == b1) {
                    if edge.1 > 1 {
                        edge.1 -= 1;
                    } else {
                        let idx = self.dag.edges[j].iter().position(|e| e.0 == b1).unwrap();
                        self.dag.edges[j].swap_remove(idx);
                        if let Some(pos) = self.rev[b1].iter().position(|&p| p == j) {
                            self.rev[b1].swap_remove(pos);
                        }
                    }
                }
                // Restore decremented edges: (i->b1) and (j->b2)
                if let Some(edge) = self.dag.edges[i].iter_mut().find(|e| e.0 == b1) {
                    edge.1 += 1;
                } else {
                    self.dag.edges[i].push((b1, 1));
                    self.rev[b1].push(i);
                }
                if let Some(edge) = self.dag.edges[j].iter_mut().find(|e| e.0 == b2) {
                    edge.1 += 1;
                } else {
                    self.dag.edges[j].push((b2, 1));
                    self.rev[b2].push(j);
                }
                simulated_annealing::Statistics::transition_update(statistics, 1, decision);
            }
        }
    }
}

impl simulated_annealing::SAState<Output> for DAGState {
    type Goal = simulated_annealing::Maximize;
    fn current_score(&mut self) -> f64 {
        self.current_score
    }
    fn naive_eval(&mut self) -> f64 {
        // Recompute score from current incoming_damage
        let n = P.n;
        let mut sum_w: i64 = 0;
        for i in 0..n {
            for &(_, w) in &self.dag.edges[i] {
                sum_w += w as i64;
            }
        }
        let mut hand_sum: i64 = 0;
        for b in 0..n {
            let need = P.h[b] as i64 - self.incoming_damage[b];
            if need > 0 {
                hand_sum += need;
            }
        }
        let sum_h: i64 = P.h.iter().map(|&x| x as i64).sum();
        let t_total = sum_w + hand_sum;
        self.current_score = (sum_h - t_total + 1) as f64;
        self.current_score
    }
    fn to_result(&self) -> Output {
        dag_to_output(&self.dag)
    }
    fn transit(
        &mut self,
        statistics: &mut simulated_annealing::BasicStatistics,
        temperature: f64,
        _progress: f64,
    ) {
        let tt = self.rng.gen_u32(100);
        if tt < 100 {
            self.transit0(statistics, temperature, _progress);
        } else {
            self.transit1(statistics, temperature, _progress);
        }
    }
}

clap_struct!(
    struct Args {
        #[clap(short = '_', long = "seed", default_value = "0")]
        seed: u32,
        #[clap(short = 'f', long = "is_file_debug", default_value = "false")]
        is_file_debug: bool,
        #[clap(short = '_', long = "max_temprature", default_value = "10.0")]
        max_temprature: f64,
        #[clap(short = '_', long = "TOP_K", default_value = "19")]
        TOP_K: usize,
    }
);

impl ToAnswer for Output {
    fn to_answer(&self) -> String {
        // Join operations as lines: "W B"
        self.ops
            .iter()
            .map(|(w, b)| format!("{w} {b}"))
            .collect::<Vec<_>>()
            .join("\n")
    }
}

pub static A: Lazy<Args> = Lazy::new(Args::parse);
pub static P: Lazy<Input> = Lazy::new(|| {
    let content = read_input();
    let source = OnceSource::new(BufReader::new(content.as_bytes()));

    // TODO: Update here
    input! {
        from source,
        n: usize,
        h: [i32; n],
        c: [i32; n],
        a: [[i32; n]; n],
    }
    let a: Vec<Vec<i32>> = a.into_iter().map(|row| row.to_vec()).collect();
    // Build sorted lists per row
    let mut a_sorted_idx: Vec<Vec<usize>> = Vec::with_capacity(n);
    let mut a_sorted_val: Vec<Vec<i32>> = Vec::with_capacity(n);
    for i in 0..n {
        let mut pairs: Vec<(i32, usize)> = (0..n).map(|b| (a[i][b], b)).collect();
        pairs.sort_by(|x, y| y.0.cmp(&x.0)); // descending by damage
        a_sorted_val.push(pairs.iter().map(|p| p.0).collect());
        a_sorted_idx.push(pairs.iter().map(|p| p.1).collect());
    }
    Input {
        n,
        h: h.to_vec(),
        c: c.to_vec(),
        a,
        a_sorted_idx,
        a_sorted_val,
    }
});

pub mod input {
    use std::{
        fs,
        io::{self, Read},
    };

    use crate::A;

    pub fn read_input() -> String {
        if A.is_file_debug {
            let file_path = format!(
                "{}/tools/in/{}.txt",
                "/Users/hiro116s/IdeaProjects/atcoder-rust", A.seed
            );
            fs::read_to_string(&*file_path).expect("Failed to read file")
        } else {
            let mut buf = String::new();
            io::stdin()
                .read_to_string(&mut buf)
                .expect("Failed to read stdin");
            buf
        }
    }
}
pub mod common_traits {
    pub trait ToAnswer {
        fn to_answer(&self) -> String;
    }
}
pub mod my_clap {
    use std::{collections::HashMap, env};
    pub trait Parse {
        fn parse() -> Self;
    }
    pub struct FieldInfo {
        pub short: Option<char>,
        pub long: Option<String>,
        pub default_value: Option<String>,
    }
    impl Default for FieldInfo {
        fn default() -> Self {
            Self::new()
        }
    }
    impl FieldInfo {
        pub fn new() -> Self {
            Self {
                short: None,
                long: None,
                default_value: None,
            }
        }
        pub fn short(mut self, c: char) -> Self {
            self.short = Some(c);
            self
        }
        pub fn long(mut self, s: &str) -> Self {
            self.long = Some(s.to_string());
            self
        }
        pub fn default_value(mut self, s: &str) -> Self {
            self.default_value = Some(s.to_string());
            self
        }
    }
    pub fn parse_args(fields: &HashMap<String, FieldInfo>) -> HashMap<String, String> {
        let args: Vec<String> = env::args().collect();
        let mut result = HashMap::new();
        for (field_name, info) in fields {
            if let Some(default) = &info.default_value {
                result.insert(field_name.clone(), default.clone());
            }
        }
        let mut i = 1;
        while i < args.len() {
            let arg = &args[i];
            if arg.starts_with('-') {
                let mut found = false;
                for (field_name, info) in fields {
                    let matches = if arg.starts_with("--") {
                        if let Some(long) = &info.long {
                            format!("--{long}") == *arg
                        } else {
                            false
                        }
                    } else if arg.len() == 2 {
                        if let Some(short) = info.short {
                            format!("-{short}") == *arg
                        } else {
                            false
                        }
                    } else {
                        false
                    };
                    if matches {
                        if i + 1 < args.len() {
                            result.insert(field_name.clone(), args[i + 1].clone());
                            i += 1;
                        }
                        found = true;
                        break;
                    }
                }
                if !found {
                    eprintln!("Unknown option: {arg}");
                }
            }
            i += 1;
        }
        result
    }
    #[macro_export]
    macro_rules ! clap_struct {(struct $ name : ident {$ (# [clap (short = $ short : literal , long = $ long : literal , default_value = $ default : literal ) ] $ field : ident : $ ty : ty ) ,* $ (, ) ? } ) => {pub struct $ name {$ (pub $ field : $ ty ) ,* } impl $ crate :: my_clap :: Parse for $ name {fn parse () -> Self {use std :: collections :: HashMap ; use $ crate :: my_clap :: FieldInfo ; let mut fields = HashMap :: new () ; $ (let info = FieldInfo :: new () . short ($ short ) . long ($ long ) . default_value ($ default ) ; fields . insert (stringify ! ($ field ) . to_string () , info ) ; ) * let parsed = $ crate :: my_clap :: parse_args (& fields ) ; Self {$ ($ field : clap_struct ! (@ convert_type parsed . get (stringify ! ($ field ) ) . unwrap_or (& String :: new () ) . clone () , $ ty ) ) ,* } } } } ; (@ convert_type $ value : expr , $ ty : ty ) => {$ value . parse ::<$ ty > () . unwrap_or_default () } ; }
}

pub mod timekeeper {
    use std::{
        fmt::{Display, Formatter, Result},
        time::{Duration, SystemTime, UNIX_EPOCH},
    };
    pub const SECTYPE_SEC: i32 = 0;
    pub const SECTYPE_MSEC: i32 = 1;
    pub const DEFAULT_TIME_CHECK_FREQUENCY: usize = 100;
    #[derive(Debug)]
    pub struct TimeKeeper {
        pub max_loop_size: usize,
        pub start_time: u64,
        pub pushed_time: Vec<u64>,
        pub total_loop_time: Vec<u64>,
        pub prev_pushed_time: Vec<u64>,
    }
    impl Default for TimeKeeper {
        fn default() -> Self {
            Self::new()
        }
    }
    impl TimeKeeper {
        pub fn new() -> Self {
            let max_loop_size = 100;
            let mut keeper = TimeKeeper {
                max_loop_size,
                start_time: 0,
                pushed_time: Vec::new(),
                total_loop_time: vec![0; max_loop_size],
                prev_pushed_time: vec![0; max_loop_size],
            };
            keeper.reset();
            keeper
        }
        pub fn reset(&mut self) {
            self.start_time = current_time_millis();
            for i in 0..self.max_loop_size {
                self.total_loop_time[i] = 0;
                self.prev_pushed_time[i] = 0;
            }
            self.pushed_time.clear();
            self.push();
        }
        pub fn push(&mut self) {
            self.pushed_time.push(current_time_millis());
        }
        pub fn loop_push(&mut self, id: usize) {
            assert_eq!(
                self.prev_pushed_time[id], 0,
                "loop_push called twice without loop_pop"
            );
            self.prev_pushed_time[id] = current_time_millis();
        }
        pub fn loop_pop(&mut self, id: usize) {
            assert_ne!(
                self.prev_pushed_time[id], 0,
                "loop_pop called without loop_push"
            );
            let current = current_time_millis();
            self.total_loop_time[id] += current - self.prev_pushed_time[id];
            self.prev_pushed_time[id] = 0;
        }
        pub fn get_passed_time(&self) -> u64 {
            current_time_millis() - self.start_time
        }
        pub fn get_passed_time_second(&self, type_: i32) -> String {
            match type_ {
                SECTYPE_SEC => format!("{}s", self.get_passed_time() as f64 / 1000.0),
                SECTYPE_MSEC => format!("{}ms", self.get_passed_time()),
                _ => panic!("Invalid time type"),
            }
        }
    }
    impl Display for TimeKeeper {
        fn fmt(&self, f: &mut Formatter<'_>) -> Result {
            let mut result = String::new();
            result.push_str("---------- Normal pushed time -----------------\n");
            let mut total_time = 0.0;
            for i in 0..self.pushed_time.len() - 1 {
                let time = (self.pushed_time[i + 1] - self.pushed_time[i]) as f64 / 1000.0;
                total_time += time;
                result.push_str(&format!("{i} : {time}s\n"));
            }
            result.push_str(&format!("Total Time = {total_time}s\n"));
            result.push_str("----------- Loop pushed time -----------------\n");
            let mut total_loop_time = 0.0;
            for i in 0..self.max_loop_size {
                if self.total_loop_time[i] != 0 {
                    let time = self.total_loop_time[i] as f64 / 1000.0;
                    result.push_str(&format!("Loop {i} : {time}s\n"));
                    total_loop_time += time;
                }
            }
            result.push_str(&format!("Total Loop Time = {total_loop_time}s\n"));
            write!(f, "{result}")
        }
    }
    fn current_time_millis() -> u64 {
        SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .expect("Time went backwards")
            .as_millis() as u64
    }
    #[derive(Debug)]
    pub struct TimeCondition {
        pub time_limit_ms: u64,
        pub time_check_frequency: usize,
        time_keeper: TimeKeeper,
        pub count: usize,
        pub previous_time_ms: u64,
    }
    impl TimeCondition {
        pub fn new(
            time_limit: Duration,
            time_check_frequency: usize,
            time_keeper: TimeKeeper,
        ) -> Self {
            let previous_time_ms = time_keeper.get_passed_time();
            Self {
                time_limit_ms: time_limit.as_millis() as u64,
                time_check_frequency,
                time_keeper,
                count: 0,
                previous_time_ms,
            }
        }
        pub fn with_default_frequency(time_limit: Duration, time_keeper: TimeKeeper) -> Self {
            Self::new(time_limit, DEFAULT_TIME_CHECK_FREQUENCY, time_keeper)
        }
        pub fn should_finish(&mut self) -> bool {
            if self.count == self.time_check_frequency {
                self.count = 1;
                self.previous_time_ms = self.time_keeper.get_passed_time();
            } else {
                self.count += 1;
            }
            self.previous_time_ms >= self.time_limit_ms
        }
        pub fn get_time_ms(&self) -> u64 {
            self.previous_time_ms
        }
        pub fn get_progress_percent(&self) -> i32 {
            ((100 * self.previous_time_ms) / self.time_limit_ms) as i32
        }
        pub fn get_progress(&self) -> f64 {
            self.previous_time_ms as f64 / self.time_limit_ms as f64
        }
    }
}
pub mod xorshift {
    #[derive(Debug, Clone, PartialEq, Eq)]
    pub struct XorShift {
        seed: u64,
    }
    impl XorShift {
        pub const fn seed_from_u64(seed: u64) -> Self {
            assert!(seed != 0, "Seed must be non-zero");
            Self { seed }
        }
        #[inline]
        fn next(&mut self) -> u64 {
            self.seed ^= self.seed << 13;
            self.seed ^= self.seed >> 7;
            self.seed ^= self.seed << 17;
            self.seed
        }
        #[inline]
        pub fn gen_u64(&mut self, max: u64) -> u64 {
            self.next() % max
        }
        #[inline]
        pub fn gen_range_u64(&mut self, min: u64, max: u64) -> u64 {
            assert!(min < max, "min must be less than max");
            min + self.gen_u64(max - min)
        }
        /// 0.0以上1.0未満のf64の乱数を生成します。
        #[inline]
        pub fn gen_f64(&mut self) -> f64 {
            (self.next() >> 11) as f64 * (1.0 / (1u64 << 53) as f64)
        }
        /// 0.0以上1.0未満のf32の乱数を生成します。
        #[inline]
        pub fn gen_f32(&mut self) -> f32 {
            (self.next() >> 40) as f32 * (1.0 / (1u32 << 24) as f32)
        }
        #[inline]
        pub fn gen_bool(&mut self) -> bool {
            self.gen_u64(2) == 1
        }
        #[inline]
        pub fn gen_usize(&mut self, max: usize) -> usize {
            self.gen_u64(max as u64) as usize
        }
        #[inline]
        pub fn gen_range_usize(&mut self, min: usize, max: usize) -> usize {
            self.gen_range_u64(min as u64, max as u64) as usize
        }
        #[inline]
        pub fn gen_u8(&mut self, max: u8) -> u8 {
            self.gen_u64(max as u64) as u8
        }
        #[inline]
        pub fn gen_range_u8(&mut self, min: u8, max: u8) -> u8 {
            self.gen_range_u64(min as u64, max as u64) as u8
        }
        #[inline]
        pub fn gen_u16(&mut self, max: u16) -> u16 {
            self.gen_u64(max as u64) as u16
        }
        #[inline]
        pub fn gen_range_u16(&mut self, min: u16, max: u16) -> u16 {
            self.gen_range_u64(min as u64, max as u64) as u16
        }
        #[inline]
        pub fn gen_u32(&mut self, max: u32) -> u32 {
            self.gen_u64(max as u64) as u32
        }
        #[inline]
        pub fn gen_range_u32(&mut self, min: u32, max: u32) -> u32 {
            self.gen_range_u64(min as u64, max as u64) as u32
        }
        #[inline]
        pub fn get_randomly<'a, T>(&mut self, vs: &'a [T]) -> &'a T {
            let index = self.gen_usize(vs.len());
            &vs[index]
        }
        pub fn shuffle<T>(&mut self, vs: &mut [T]) {
            let n = vs.len();
            for i in 0..n {
                let j = self.gen_range_usize(i, n);
                vs.swap(i, j);
            }
        }
    }
}
pub mod simulated_annealing {
    use std::{marker::PhantomData, time::Duration};

    use once_cell::sync::Lazy;

    use crate::{
        common_traits::ToAnswer,
        timekeeper::{TimeCondition, TimeKeeper},
        xorshift::XorShift,
    };
    const LOG_TABLE_SIZE: usize = 65535;
    static LOG_TABLE: Lazy<Vec<f64>> = Lazy::new(|| {
        let mut table = Vec::with_capacity(LOG_TABLE_SIZE);
        for i in 0..LOG_TABLE_SIZE {
            let p = (i as f64 + 0.5) / LOG_TABLE_SIZE as f64;
            table.push(p.ln());
        }
        table
    });
    pub trait Logger {
        fn best_update_log(best_score: f64, current_score: f64);
    }
    pub struct Verbose;
    impl Logger for Verbose {
        fn best_update_log(best_score: f64, current_score: f64) {
            eprintln!("Best score updated: {best_score} => {current_score}");
        }
    }
    pub struct Silent;
    impl Logger for Silent {
        fn best_update_log(_best_score: f64, _current_score: f64) {}
    }
    pub trait Objective {
        fn diff(next_score: f64, current_score: f64) -> f64;
    }
    pub struct Maximize;
    impl Objective for Maximize {
        #[inline]
        fn diff(next_score: f64, current_score: f64) -> f64 {
            next_score - current_score
        }
    }
    pub struct Minimize;
    impl Objective for Minimize {
        #[inline]
        fn diff(next_score: f64, current_score: f64) -> f64 {
            current_score - next_score
        }
    }
    pub trait Statistics {
        fn loop_count(&mut self);
        fn best_update(&mut self);
        fn transition_update(&mut self, transition_id: usize, transition_type: TransitionType);
    }
    const TRANSITION_TYPE_CNT: usize = 5;
    #[derive(Debug, Default)]
    pub struct BasicStatistics {
        pub loop_count: u64,
        pub best_update_count: u64,
        pub accept_with_worse_count: [u64; TRANSITION_TYPE_CNT],
        pub accept_with_better_count: [u64; TRANSITION_TYPE_CNT],
        pub reject_count: [u64; TRANSITION_TYPE_CNT],
    }
    impl Statistics for BasicStatistics {
        fn loop_count(&mut self) {
            self.loop_count += 1;
        }
        fn best_update(&mut self) {
            self.best_update_count += 1;
        }
        fn transition_update(&mut self, transition_id: usize, transition_type: TransitionType) {
            match transition_type {
                TransitionType::AcceptWithWorse => self.accept_with_worse_count[transition_id] += 1,
                TransitionType::AcceptWithBetter => {
                    self.accept_with_better_count[transition_id] += 1
                }
                TransitionType::Reject => self.reject_count[transition_id] += 1,
            }
        }
    }
    #[derive(Debug, Default)]
    pub struct NoStatistics;
    impl Statistics for NoStatistics {
        fn loop_count(&mut self) {}
        fn best_update(&mut self) {}
        fn transition_update(&mut self, _transition_id: usize, _transition_type: TransitionType) {}
    }
    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum TransitionType {
        AcceptWithWorse,
        AcceptWithBetter,
        Reject,
    }
    pub trait SAState<R: ToAnswer> {
        type Goal: Objective;
        fn current_score(&mut self) -> f64;
        fn naive_eval(&mut self) -> f64;
        fn to_result(&self) -> R;
        fn transit(&mut self, statistics: &mut BasicStatistics, temperature: f64, progress: f64);
        fn should_transit(
            next_score: f64,
            current_score: f64,
            temperature: f64,
            rng: &mut XorShift,
        ) -> TransitionType {
            let diff = Self::Goal::diff(next_score, current_score);
            if diff > 0.0 {
                TransitionType::AcceptWithBetter
            } else if temperature > 1e-9 {
                let log_r = LOG_TABLE[rng.gen_u64(LOG_TABLE.len() as u64) as usize];
                if temperature * log_r < diff {
                    TransitionType::AcceptWithWorse
                } else {
                    TransitionType::Reject
                }
            } else {
                TransitionType::Reject
            }
        }
    }
    #[derive(Debug, Clone)]
    pub struct SimulatedAnnealing<LOGGER: Logger> {
        pub max_temperature: f64,
        pub min_temperature: f64,
        pub best_update_progress: f64,
        pub time_limit: Duration,
        _logger: PhantomData<LOGGER>,
    }
    impl<LOGGER: Logger> SimulatedAnnealing<LOGGER> {
        pub fn new(
            max_temperature: f64,
            min_temperature: f64,
            best_update_progress: f64,
            time_limit: Duration,
        ) -> Self {
            Self {
                max_temperature,
                min_temperature,
                best_update_progress,
                time_limit,
                _logger: PhantomData,
            }
        }
        pub fn optimize<R, S>(&self, state: &mut S, statistics: &mut BasicStatistics) -> R
        where
            S: SAState<R>,
            R: ToAnswer,
            LOGGER: Logger,
        {
            let mut best_score = state.naive_eval();
            let mut best_result = state.to_result();
            let mut time_condition =
                TimeCondition::with_default_frequency(self.time_limit, TimeKeeper::new());
            while !time_condition.should_finish() {
                statistics.loop_count();
                let progress = time_condition.get_progress();
                let temperature = self.calc_temperature(progress);
                state.transit(statistics, temperature, progress);
                if progress >= self.best_update_progress
                    && S::Goal::diff(state.current_score(), best_score) > 0.0
                {
                    LOGGER::best_update_log(best_score, state.current_score());
                    statistics.best_update();
                    best_score = state.current_score();
                    best_result = state.to_result();
                }
            }
            best_result
        }
        fn calc_temperature(&self, progress: f64) -> f64 {
            self.max_temperature - progress * (self.max_temperature - self.min_temperature)
        }
    }
    pub fn to_prob_array(ratio: Vec<usize>) -> Vec<usize> {
        let total: usize = ratio.iter().sum();
        let mut result = Vec::with_capacity(total);
        for (index, &count) in ratio.iter().enumerate() {
            for _ in 0..count {
                result.push(index);
            }
        }
        result
    }
}
pub mod cleanable_vec {
    use std::clone::Clone;
    #[derive(Debug, Clone)]
    pub struct CleanableVecBool {
        data: Vec<u32>,
        next: u32,
        vis_count: u32,
    }
    impl CleanableVecBool {
        pub fn new(data: Vec<u32>) -> Self {
            Self {
                data,
                next: 1,
                vis_count: 0,
            }
        }
        pub fn create(n: usize) -> Self {
            Self::new(vec![0; n])
        }
        pub fn clear(&mut self) {
            self.next += 1;
            self.vis_count = 0;
        }
        pub fn get_vis_count(&self) -> usize {
            self.vis_count as usize
        }
        pub fn is_visited(&self, v: usize) -> bool {
            self.data[v] == self.next
        }
        pub fn visit(&mut self, v: usize) {
            self.data[v] = self.next;
        }
        pub fn unvisit(&mut self, v: usize) {
            if self.data[v] == self.next {
                self.vis_count -= 1;
            }
            self.data[v] = self.next - 1;
        }
    }
    #[derive(Debug, Clone)]
    pub struct CleanableVec<T: Clone> {
        cleanable_bool: CleanableVecBool,
        values: Vec<T>,
        default_values: Vec<T>,
    }
    impl<T: Clone> CleanableVec<T> {
        pub fn new(
            cleanable_bool: CleanableVecBool,
            values: Vec<T>,
            default_values: Vec<T>,
        ) -> Self {
            assert_eq!(values.len(), default_values.len());
            Self {
                cleanable_bool,
                values,
                default_values,
            }
        }
        pub fn create(n: usize, default_value: T) -> Self
        where
            T: Clone,
        {
            let default_values = vec![default_value; n];
            Self::new(
                CleanableVecBool::create(n),
                vec![default_values[0].clone(); n],
                default_values,
            )
        }
        pub fn create_with_defaults(n: usize, default_values: Vec<T>) -> Self
        where
            T: Clone,
        {
            assert_eq!(
                n,
                default_values.len(),
                "Size mismatch between n and default_values"
            );
            Self::new(
                CleanableVecBool::create(n),
                vec![default_values[0].clone(); n],
                default_values,
            )
        }
        pub fn clear(&mut self) {
            self.cleanable_bool.clear();
        }
        pub fn set(&mut self, i: usize, v: T) {
            self.values[i] = v;
            self.cleanable_bool.visit(i);
        }
        pub fn get(&self, i: usize) -> T {
            if self.cleanable_bool.is_visited(i) {
                self.values[i].clone()
            } else {
                self.default_values[i].clone()
            }
        }
        pub fn to_vec(&self) -> Vec<T> {
            let n = self.values.len();
            let mut result = Vec::with_capacity(n);
            for i in 0..n {
                result.push(self.get(i));
            }
            result
        }
        pub fn len(&self) -> usize {
            self.values.len()
        }
        pub fn is_empty(&self) -> bool {
            self.values.is_empty()
        }
    }
}
