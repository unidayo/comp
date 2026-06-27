use std::{
    collections::{BinaryHeap, HashSet, VecDeque},
    hash::{BuildHasherDefault, Hasher},
    process::exit,
    rc::Rc,
};

use itertools::Itertools;
use proconio::input;
use rand::{Rng, SeedableRng};
use rand_pcg::Pcg64Mcg;
use rustc_hash::FxHashSet;

type Real = f64;
type Color = [Real; 3];

const TIME_LIMIT_SEC: f64 = 2.95;
const N: usize = 20;
const N2: usize = N * N;
const NUM_TARGETS: usize = 1000;
const C_ERROR: Real = 1e4;

const EPS: Real = 1e-7;
const DIV_EPS: Real = 1e-9;
const BEAM_END_TIME: f64 = 0.8 * TIME_LIMIT_SEC;

fn main() {
    get_time_sec();

    let input = read_input();

    if input.num_turns < 10000 {
        one_dim_beam(&input);
    } else {
        let success = double_comb_beam(&input);
        if !success {
            comb_beam(&input);
        }
    }

    exit(0);
}

fn one_dim_beam(input: &Input) {
    let one_dim_env = OneDimEnvironment::new(&input);

    let mut selector = OneDimSelector::new(&one_dim_env);
    let mut states = vec![OneDimState::new(&one_dim_env)];
    for _ in 0..input.targets.len() {
        for state in &states {
            state.expand(&one_dim_env, &mut selector);
        }
        states = selector.select(&one_dim_env);
    }

    let state = states
        .into_iter()
        .min_by(|a, b| a.total_error.partial_cmp(&b.total_error).unwrap())
        .unwrap();

    one_dim_env.print_walls();
    for action in state.history() {
        print_action(action);
    }
}

#[derive(Clone, Debug)]
struct OneDimSelector {
    beam_width: usize,
    candidates: Vec<Vec<OneDimState>>,
    target: usize,
    low_extra_turns: usize,
    high_extra_turns: usize,
}

impl OneDimSelector {
    fn new(env: &OneDimEnvironment) -> Self {
        let beam_width = 40 / env.num_tubes;

        let target = 0;
        let high_extra_turns = (target + 1) * env.max_extra_turns / NUM_TARGETS;
        let low_extra_turns = high_extra_turns.saturating_sub(beam_width - 1);

        Self {
            beam_width,
            candidates: vec![vec![]; env.max_extra_turns + 1],
            target,
            low_extra_turns,
            high_extra_turns,
        }
    }

    fn add(&mut self, state: OneDimState) {
        let extra_turns = state.extra_turns().max(self.low_extra_turns);
        if extra_turns > self.high_extra_turns {
            return;
        }
        self.candidates[extra_turns].push(state);
    }

    fn select(&mut self, env: &OneDimEnvironment) -> Vec<OneDimState> {
        let mut que = BinaryHeap::new();
        let mut ret = Vec::with_capacity(self.beam_width);
        for extra_turn in self.low_extra_turns..=self.high_extra_turns {
            for (i, state) in self.candidates[extra_turn].iter().enumerate() {
                let cost = state.evaluate(env);
                let cost = (-1e9 * cost) as i64;
                que.push((cost, extra_turn, i));
            }
            if let Some((_, extra_turn, i)) = que.pop() {
                ret.push(self.candidates[extra_turn][i].clone());
            }
        }
        for extra_turn in self.low_extra_turns..=self.high_extra_turns {
            self.candidates[extra_turn].clear();
        }
        self.update(env);
        ret
    }

    fn update(&mut self, env: &OneDimEnvironment) {
        const TLE_AVOID_TIME: f64 = 0.7 * TIME_LIMIT_SEC;
        if get_time_sec() > TLE_AVOID_TIME {
            self.beam_width = 1;
        }
        self.target += 1;
        self.high_extra_turns = (self.target + 1) * env.max_extra_turns / NUM_TARGETS;
        if self.high_extra_turns % 2 == 1 {
            self.high_extra_turns -= 1;
        }
        self.low_extra_turns = self.high_extra_turns.saturating_sub(self.beam_width - 1);
    }
}

#[derive(Clone, Debug)]
struct OneDimState {
    num_delivers: usize,
    num_adds: usize,
    num_toggles: usize,
    total_error: Real,
    wells: Vec<Well>,
    partition0: usize,
    partitions: Vec<usize>,
    history: Option<Rc<HistoryNode>>,
}

impl OneDimState {
    fn new(env: &OneDimEnvironment) -> Self {
        let n = env.well_sizes_first.len();

        let mut wells = Vec::with_capacity(n);
        for i in 0..n {
            let well = Well {
                p: [0.0; 3],
                v: 0.0,
                s: env.well_sizes_first[i],
            };
            wells.push(well);
        }
        let mut partitions = env.well_sizes_first.clone();
        partitions.insert(0, 0);
        for v in 0..n {
            partitions[v + 1] += partitions[v];
        }
        // initial additions
        let num_adds = n;
        let mut actions = Vec::with_capacity(num_adds);
        let mut tube_que = (0..env.num_tubes).collect_vec();
        let mut adj_counts = vec![vec![0; env.num_tubes]; env.num_tubes];
        let mut last_tube = env.num_tubes - 1;
        for i in 0..num_adds {
            if tube_que.is_empty() {
                tube_que = (0..env.num_tubes).collect_vec();
            }
            let mut best_tube = 0;
            let mut min_adj_cnt = num_adds;
            for &tube in &tube_que {
                if tube == last_tube {
                    continue; // skip the last tube
                }
                let adj_cnt = adj_counts[last_tube][tube];
                if min_adj_cnt.chmin(adj_cnt) {
                    best_tube = tube;
                }
            }
            let tube = best_tube;
            tube_que.retain(|&t| t != tube);
            adj_counts[last_tube][tube] += 1;
            adj_counts[tube][last_tube] += 1;
            last_tube = tube;

            actions.push(env.make_add_action(partitions[i], tube));
            wells[i].add_paint(env.tubes[tube]);
        }
        let history = HistoryNode::new(None, actions);
        Self {
            num_delivers: 0,
            num_adds,
            num_toggles: 0,
            total_error: 0.0,
            wells,
            partition0: 0,
            partitions,
            history,
        }
    }

    fn evaluate(&self, env: &OneDimEnvironment) -> Real {
        self.total_error + self.num_adds.saturating_sub(NUM_TARGETS) as Real * env.cost_per_gram
    }

    fn extra_turns(&self) -> usize {
        self.num_toggles + self.num_adds.saturating_sub(NUM_TARGETS)
    }

    fn init_partitions_from_wells(&mut self) {
        let n = self.wells.len();
        self.partitions.clear();
        self.partitions.push(self.partition0);
        for i in 0..n {
            self.partitions.push(self.partitions[i] + self.wells[i].s);
        }
        debug_assert_eq!(self.partitions[n] - self.partitions[0], N2);
    }

    fn expand_no_add(&self, env: &OneDimEnvironment, selector: &mut OneDimSelector) {
        if self.num_adds < NUM_TARGETS {
            return;
        }
        let target = env.targets[self.num_delivers];

        let mut min_error = Real::INFINITY;
        let mut best_l = 0;
        let mut best_merge_wells = 0;
        for left in 0..self.wells.len() {
            let mut well = self.wells[left].clone();
            let mut merge_wells = 1;
            while well.v < 1.0 - EPS {
                if merge_wells >= self.wells.len() {
                    return; // cannot merge anymore
                }
                let right = (left + merge_wells) % self.wells.len();
                well += self.wells[right].clone();
                merge_wells += 1;
            }
            let color = well.color();
            let error = color_distance(color, target);
            if min_error.chmin(error) {
                best_l = left;
                best_merge_wells = merge_wells;
            }
        }
        let left = best_l;
        let right = left + best_merge_wells;

        let mut actions = vec![];
        let mut well = self.wells[left].clone();
        for i in left + 1..right {
            actions.push(env.make_toggle_action(self.partitions[i % self.wells.len()]));
            well += self.wells[i % self.wells.len()].clone();
        }
        actions.push(env.make_deliver_action(self.partitions[left]));
        debug_assert!(well.v >= 1.0 - EPS);
        well.discard_paint();

        let mut state = self.clone();
        state.num_delivers += 1;
        state.num_toggles += best_merge_wells - 1;
        state.total_error += min_error;
        state.history = HistoryNode::new(state.history.clone(), actions);

        state.wells[left] = well;
        if right > self.wells.len() {
            state.wells.drain(left + 1..self.wells.len());
            for well in state.wells.drain(0..right - self.wells.len()) {
                state.partition0 += well.s;
            }
        } else {
            state.wells.drain(left + 1..right);
        }
        state.init_partitions_from_wells();
        selector.add(state);
    }

    fn expand_4_turn(&self, env: &OneDimEnvironment, selector: &mut OneDimSelector) {
        if self.wells.len() < 3 {
            return;
        }
        let target = env.targets[self.num_delivers];
        let most_distant_tube = self.most_distant_tube(env);

        let mut min_error = Real::INFINITY;
        let mut best_actions = vec![];
        let mut best_changes = vec![];
        let mut best_partition0 = 0;

        // add + merge + merge + deliver
        for left in 0..self.wells.len() {
            let mid = (left + 1) % self.wells.len();
            let right = (left + 2) % self.wells.len();
            for add_well in 0..4 {
                if add_well == 0 {
                    if self.wells[left].s as Real - self.wells[left].v < 1.0 - EPS {
                        continue; // overflow
                    }
                } else if add_well == 1 {
                    if (self.wells[left].s + self.wells[mid].s + self.wells[right].s) as Real
                        - (self.wells[left].v + self.wells[mid].v + self.wells[right].v)
                        < 1.0 - EPS
                    {
                        continue; // overflow
                    }
                } else if add_well == 2 {
                    if self.wells[right].s as Real - self.wells[right].v < 1.0 - EPS {
                        continue; // overflow
                    }
                } else {
                    if self.wells[left].v + self.wells[mid].v + self.wells[right].v < 1.0 - EPS {
                        continue; // cannot deliver
                    }
                }
                for tube in 0..env.num_tubes {
                    let mut well_l = self.wells[left].clone();
                    let mut well_m = self.wells[mid].clone();
                    let mut well_r = self.wells[right].clone();
                    if add_well == 0 {
                        well_l.add_paint(env.tubes[tube]);
                    } else if add_well == 1 {
                        well_m += Well {
                            p: env.tubes[tube],
                            v: 1.0,
                            s: 1,
                        }
                    } else if add_well == 2 {
                        well_r.add_paint(env.tubes[tube]);
                    }
                    let (add_size_l, add_size_r, error) = self.merge_3_wells(
                        &well_l,
                        &well_m,
                        &well_r,
                        min_error,
                        target,
                        add_well == 1,
                    );
                    if add_well == 1 {
                        // revert
                        well_m = self.wells[mid].clone();
                    }
                    if add_size_l == 0 || add_size_r == 0 || error >= min_error {
                        continue;
                    }
                    min_error = error;
                    best_actions.clear();

                    if add_well == 0 {
                        best_actions.push(env.make_add_action(self.partitions[left], tube));
                    } else if add_well == 2 {
                        best_actions.push(env.make_add_action(self.partitions[right], tube));
                    }
                    let whole_merge_l = add_size_l == well_l.s;
                    let whole_merge_r = add_size_r == well_r.s;
                    if !whole_merge_l {
                        best_actions.push(
                            env.make_toggle_action(
                                self.partitions[mid] + env.num_cells - add_size_l,
                            ),
                        );
                    }
                    if !whole_merge_r {
                        best_actions
                            .push(env.make_toggle_action(self.partitions[right] + add_size_r));
                    }
                    let dispatched_well_l = well_l.divide(add_size_l);
                    let dispatched_well_r = well_r.divide(add_size_r);
                    well_l -= dispatched_well_l.clone();
                    well_r -= dispatched_well_r.clone();

                    best_actions.push(env.make_toggle_action(self.partitions[mid]));
                    best_actions.push(env.make_toggle_action(self.partitions[right]));
                    well_m += dispatched_well_l.clone();
                    well_m += dispatched_well_r.clone();

                    if add_well == 1 {
                        best_actions.push(env.make_add_action(self.partitions[mid], tube));
                        well_m.add_paint(env.tubes[tube]);
                    }
                    best_actions.push(env.make_deliver_action(self.partitions[mid]));
                    debug_assert!(well_m.v >= 1.0 - EPS);
                    well_m.discard_paint();

                    if add_well == 3 {
                        best_actions
                            .push(env.make_add_action(self.partitions[mid], most_distant_tube));
                        well_m.add_paint(env.tubes[most_distant_tube]);
                    }
                    if whole_merge_l {
                        best_actions
                            .push(env.make_toggle_action(self.partitions[left] + well_m.s / 2));
                        let dispatched_well = well_m.divide(well_m.s / 2);
                        well_m -= dispatched_well.clone();
                        well_l = dispatched_well.clone();
                    }
                    if whole_merge_r {
                        best_actions.push(env.make_toggle_action(
                            self.partitions[right + 1] + env.num_cells - well_m.s / 2,
                        ));
                        let dispatched_well = well_m.divide(well_m.s / 2);
                        well_m -= dispatched_well.clone();
                        well_r = dispatched_well.clone();
                    }
                    if mid == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + well_l.s - self.wells[left].s;
                    } else if right == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + self.wells[right].s - well_r.s;
                    } else {
                        best_partition0 = self.partition0;
                    }
                    best_changes.clear();
                    best_changes.push((left, well_l));
                    best_changes.push((mid, well_m));
                    best_changes.push((right, well_r));

                    if add_well == 3 {
                        break;
                    }
                }
            }
        }
        // add + center_merge + deliver
        for left in 0..self.wells.len() {
            let right = (left + 1) % self.wells.len();
            for add_well in 0..4 {
                if add_well == 0 {
                    if self.wells[left].s as Real - self.wells[left].v < 1.0 - EPS {
                        continue; // overflow
                    }
                } else if add_well == 1 {
                    if self.wells[right].s as Real - self.wells[right].v < 1.0 - EPS {
                        continue; // overflow
                    }
                } else if add_well == 2 {
                    if (self.wells[left].s + self.wells[right].s) as Real
                        - (self.wells[left].v + self.wells[right].v)
                        < 1.0 - EPS
                    {
                        continue; // overflow
                    }
                } else {
                    if self.wells[left].v + self.wells[right].v < 1.0 - EPS {
                        continue; // cannot deliver
                    }
                }
                for tube in 0..env.num_tubes {
                    let mut well_l = self.wells[left].clone();
                    let mut well_r = self.wells[right].clone();
                    if add_well == 0 {
                        well_l.add_paint(env.tubes[tube]);
                    } else if add_well == 1 {
                        well_r.add_paint(env.tubes[tube]);
                    }
                    let well_m = if add_well == 2 {
                        Well {
                            p: env.tubes[tube],
                            v: 1.0,
                            s: 1,
                        }
                    } else {
                        Well::zero()
                    };
                    let (add_size_l, add_size_r, error) = self.merge_3_wells(
                        &well_l,
                        &well_m,
                        &well_r,
                        min_error,
                        target,
                        add_well == 2,
                    );
                    if error >= min_error
                        || add_size_l == 0
                        || add_size_r == 0
                        || add_size_l == well_l.s
                        || add_size_r == well_r.s
                    {
                        continue;
                    }
                    min_error = error;
                    best_actions.clear();

                    if add_well == 0 {
                        best_actions.push(env.make_add_action(self.partitions[left], tube));
                    } else if add_well == 1 {
                        best_actions.push(env.make_add_action(self.partitions[right], tube));
                    }
                    best_actions.push(
                        env.make_toggle_action(self.partitions[right] + env.num_cells - add_size_l),
                    );
                    best_actions.push(env.make_toggle_action(self.partitions[right] + add_size_r));
                    let dispatched_well_l = well_l.divide(add_size_l);
                    let dispatched_well_r = well_r.divide(add_size_r);
                    well_l -= dispatched_well_l.clone();
                    well_r -= dispatched_well_r.clone();

                    best_actions.push(env.make_toggle_action(self.partitions[right]));
                    let mut well_m = dispatched_well_l + dispatched_well_r;

                    if add_well == 2 {
                        best_actions.push(env.make_add_action(self.partitions[right], tube));
                        well_m.add_paint(env.tubes[tube]);
                    }
                    best_actions.push(env.make_deliver_action(self.partitions[right]));
                    debug_assert!(well_m.v >= 1.0 - EPS);
                    well_m.discard_paint();

                    if well_l.s < well_r.s {
                        // merge well_m into well_l
                        best_actions.push(env.make_toggle_action(
                            self.partitions[right] + env.num_cells - add_size_l,
                        ));
                        well_l += well_m;
                        if add_well == 3 {
                            best_actions.push(
                                env.make_add_action(self.partitions[left], most_distant_tube),
                            );
                            well_l.add_paint(env.tubes[most_distant_tube]);
                        }
                    } else {
                        // merge well_m into well_r
                        best_actions
                            .push(env.make_toggle_action(self.partitions[right] + add_size_r));
                        well_r += well_m;
                        if add_well == 3 {
                            best_actions.push(
                                env.make_add_action(self.partitions[right], most_distant_tube),
                            );
                            well_r.add_paint(env.tubes[most_distant_tube]);
                        }
                    }
                    if right == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + self.wells[right].s - well_r.s;
                    } else {
                        best_partition0 = self.partition0;
                    }
                    best_changes.clear();
                    best_changes.push((left, well_l));
                    best_changes.push((right, well_r));
                }
            }
        }
        let mut state = self.clone();
        for (i, well) in best_changes {
            state.wells[i] = well;
        }
        state.partition0 = best_partition0;
        state.init_partitions_from_wells();
        state.num_delivers += 1;
        state.num_adds += 1;
        state.num_toggles += 4;
        state.total_error += min_error;
        state.history = HistoryNode::new(state.history.clone(), best_actions);
        selector.add(state);
    }

    // merge well_l and well_r into well_m
    fn merge_3_wells(
        &self,
        well_l: &Well,
        well_m: &Well,
        well_r: &Well,
        prune_error: Real,
        target: Color,
        add_after_merge: bool,
    ) -> (usize, usize, Real) {
        if well_l.v + well_m.v + well_r.v < 1.0 - EPS {
            return (!0, !0, Real::INFINITY); // cannot deliver
        }
        if add_after_merge
            && (well_l.s + well_m.s + well_r.s) as Real - (well_l.v + well_m.v + well_r.v)
                < 1.0 - EPS
        {
            return (!0, !0, Real::INFINITY); // cannot add after merge
        }
        let unit_l = well_l.divide(1);
        let unit_r = well_r.divide(1);

        if error_lower_bound3(well_m.color(), unit_l.color(), unit_r.color(), target) >= prune_error
        {
            return (!0, !0, Real::INFINITY); // prune
        }
        let mut min_error = Real::INFINITY;
        let mut best_cnt_l = 0;
        let mut best_cnt_r = 0;

        let room_l = 1.0 - unit_l.v;
        let room_r = 1.0 - unit_r.v;
        let free_cap_m = well_m.s as Real - well_m.v;
        let enough_free_cap = |cnt_l: usize, cnt_r: usize| {
            !add_after_merge
                || free_cap_m + cnt_l as Real * room_l + cnt_r as Real * room_r >= 1.0 - EPS
        };

        let mut cnt_l = 0;
        let mut cnt_r = 0;
        while cnt_l < well_l.s && cnt_r < well_r.s {
            // increment left
            let mut well_inc_l = well_m.clone();
            well_inc_l += unit_l.clone() * (cnt_l + 1);
            well_inc_l += unit_r.clone() * cnt_r;
            let color = well_inc_l.color();
            let error_l = color_distance(color, target);

            // increment right
            let mut well_inc_r = well_m.clone();
            well_inc_r += unit_l.clone() * cnt_l;
            well_inc_r += unit_r.clone() * (cnt_r + 1);
            let color = well_inc_r.color();
            let error_r = color_distance(color, target);

            if error_l < error_r {
                // increment left
                cnt_l += 1;
                if well_inc_l.v >= 1.0 - EPS
                    && min_error.chmin(error_l)
                    && enough_free_cap(cnt_l, cnt_r)
                {
                    best_cnt_l = cnt_l;
                    best_cnt_r = cnt_r;
                }
            } else {
                // increment right
                cnt_r += 1;
                if well_inc_r.v >= 1.0 - EPS
                    && min_error.chmin(error_r)
                    && enough_free_cap(cnt_l, cnt_r)
                {
                    best_cnt_l = cnt_l;
                    best_cnt_r = cnt_r;
                }
            }
        }
        while cnt_l < well_l.s {
            let mut well_inc_l = well_m.clone();
            well_inc_l += unit_l.clone() * (cnt_l + 1);
            well_inc_l += unit_r.clone() * cnt_r;
            let color = well_inc_l.color();
            let error = color_distance(color, target);
            cnt_l += 1;
            if well_inc_l.v >= 1.0 - EPS && min_error.chmin(error) && enough_free_cap(cnt_l, cnt_r)
            {
                best_cnt_l = cnt_l;
                best_cnt_r = cnt_r;
            }
        }
        while cnt_r < well_r.s {
            let mut well_inc_r = well_m.clone();
            well_inc_r += unit_l.clone() * cnt_l;
            well_inc_r += unit_r.clone() * (cnt_r + 1);
            let color = well_inc_r.color();
            let error = color_distance(color, target);
            cnt_r += 1;
            if well_inc_r.v >= 1.0 - EPS && min_error.chmin(error) && enough_free_cap(cnt_l, cnt_r)
            {
                best_cnt_l = cnt_l;
                best_cnt_r = cnt_r;
            }
        }
        (best_cnt_l, best_cnt_r, min_error)
    }

    fn expand_2_turn(&self, env: &OneDimEnvironment, selector: &mut OneDimSelector) {
        if self.wells.len() < 2 {
            return;
        }
        let target = env.targets[self.num_delivers];

        let mut min_error = Real::INFINITY;
        let mut best_actions = vec![];
        let mut best_changes = vec![];
        let mut best_partition0 = 0;

        for i in 0..self.wells.len() {
            if self.wells[i].s as Real - self.wells[i].v < 1.0 - EPS {
                continue; // overflow
            }
            // add + divide + merge + deliver
            let j = (i + self.wells.len() - 1) % self.wells.len();
            let well_j = &self.wells[j];
            for tube in 0..env.num_tubes {
                let mut well_i = self.wells[i].clone();
                well_i.add_paint(env.tubes[tube]);
                let well_i = well_i;

                // merge well_j into well_i
                let (add_size, error) = self.merge_2_wells(&well_i, &well_j, min_error, target, 0);
                if add_size > 0 && min_error.chmin(error) {
                    let mut well_i = well_i.clone();
                    let mut well_j = well_j.clone();
                    best_actions.clear();

                    best_actions.push(env.make_add_action(self.partitions[i], tube));

                    let whole_merge = add_size == well_j.s;
                    if !whole_merge {
                        best_actions.push(
                            env.make_toggle_action(self.partitions[i] + env.num_cells - add_size),
                        );
                    }
                    let dispatched_well = well_j.divide(add_size);
                    well_j -= dispatched_well.clone();

                    best_actions.push(env.make_toggle_action(self.partitions[i]));
                    well_i += dispatched_well.clone();

                    best_actions.push(env.make_deliver_action(self.partitions[i]));
                    debug_assert!(well_i.v >= 1.0 - EPS);
                    well_i.discard_paint();

                    if whole_merge {
                        best_actions
                            .push(env.make_toggle_action(self.partitions[j] + well_i.s / 2));
                        let dispatched_well = well_i.divide(well_i.s / 2);
                        well_i -= dispatched_well.clone();
                        well_j = dispatched_well.clone();
                    }
                    if i == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + self.wells[i].s - well_i.s;
                    } else {
                        best_partition0 = self.partition0;
                    }
                    best_changes.clear();
                    best_changes.push((j, well_j));
                    best_changes.push((i, well_i));
                }
                // merge well_i into well_j
                let (add_size, error) = self.merge_2_wells(&well_j, &well_i, min_error, target, 0);
                if add_size > 0 && min_error.chmin(error) {
                    let mut well_i = well_i.clone();
                    let mut well_j = well_j.clone();
                    best_actions.clear();

                    best_actions.push(env.make_add_action(self.partitions[i], tube));

                    let whole_merge = add_size == well_i.s;
                    if !whole_merge {
                        best_actions.push(env.make_toggle_action(self.partitions[i] + add_size));
                    }
                    let dispatched_well = well_i.divide(add_size);
                    well_i -= dispatched_well.clone();

                    best_actions.push(env.make_toggle_action(self.partitions[i]));
                    well_j += dispatched_well.clone();

                    best_actions.push(env.make_deliver_action(self.partitions[j]));
                    debug_assert!(well_j.v >= 1.0 - EPS);
                    well_j.discard_paint();

                    if whole_merge {
                        best_actions.push(env.make_toggle_action(
                            self.partitions[i + 1] + env.num_cells - well_j.s / 2,
                        ));
                        let dispatched_well = well_j.divide(well_j.s / 2);
                        well_j -= dispatched_well.clone();
                        well_i = dispatched_well.clone();
                    }
                    if i == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + self.wells[i].s - well_i.s;
                    } else {
                        best_partition0 = self.partition0;
                    }
                    best_changes.clear();
                    best_changes.push((j, well_j));
                    best_changes.push((i, well_i));
                }
            }
            let j = (i + 1) % self.wells.len();
            let well_j = &self.wells[j];
            for tube in 0..env.num_tubes {
                let mut well_i = self.wells[i].clone();
                well_i.add_paint(env.tubes[tube]);

                // merge well_j into well_i
                let (add_size, error) = self.merge_2_wells(&well_i, &well_j, min_error, target, 0);
                if add_size > 0 && min_error.chmin(error) {
                    let mut well_i = well_i.clone();
                    let mut well_j = well_j.clone();
                    best_actions.clear();

                    best_actions.push(env.make_add_action(self.partitions[i], tube));

                    let whole_merge = add_size == well_j.s;
                    if !whole_merge {
                        best_actions.push(env.make_toggle_action(self.partitions[j] + add_size));
                    }
                    let dispatched_well = well_j.divide(add_size);
                    well_j -= dispatched_well.clone();

                    best_actions.push(env.make_toggle_action(self.partitions[j]));
                    well_i += dispatched_well.clone();

                    best_actions.push(env.make_deliver_action(self.partitions[i]));
                    debug_assert!(well_i.v >= 1.0 - EPS);
                    well_i.discard_paint();

                    if whole_merge {
                        best_actions.push(env.make_toggle_action(
                            self.partitions[j + 1] + env.num_cells - well_i.s / 2,
                        ));
                        let dispatched_well = well_i.divide(well_i.s / 2);
                        well_i -= dispatched_well.clone();
                        well_j = dispatched_well.clone();
                    }
                    if j == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + self.wells[j].s - well_j.s;
                    } else {
                        best_partition0 = self.partition0;
                    }
                    best_changes.clear();
                    best_changes.push((i, well_i));
                    best_changes.push((j, well_j));
                }
                // merge well_i into well_j
                let (add_size, error) = self.merge_2_wells(&well_j, &well_i, min_error, target, 0);
                if add_size > 0 && min_error.chmin(error) {
                    let mut well_i = well_i.clone();
                    let mut well_j = well_j.clone();
                    best_actions.clear();

                    best_actions.push(env.make_add_action(self.partitions[i], tube));

                    let whole_merge = add_size == well_i.s;
                    if !whole_merge {
                        best_actions.push(
                            env.make_toggle_action(self.partitions[j] + env.num_cells - add_size),
                        );
                    }
                    let dispatched_well = well_i.divide(add_size);
                    well_i -= dispatched_well.clone();

                    best_actions.push(env.make_toggle_action(self.partitions[j]));
                    well_j += dispatched_well.clone();

                    best_actions.push(env.make_deliver_action(self.partitions[j]));
                    debug_assert!(well_j.v >= 1.0 - EPS);
                    well_j.discard_paint();

                    if whole_merge {
                        best_actions
                            .push(env.make_toggle_action(self.partitions[i] + well_j.s / 2));
                        let dispatched_well = well_j.divide(well_j.s / 2);
                        well_j -= dispatched_well.clone();
                        well_i = dispatched_well.clone();
                    }
                    if j == 0 {
                        best_partition0 =
                            self.partition0 + env.num_cells + self.wells[j].s - well_j.s;
                    } else {
                        best_partition0 = self.partition0;
                    }
                    best_changes.clear();
                    best_changes.push((i, well_i));
                    best_changes.push((j, well_j));
                }
            }
            // divide + add + deliver + merge
            for tube in 0..env.num_tubes {
                let well = self.wells[i].clone();
                let base_well = Well {
                    p: env.tubes[tube],
                    v: 1.0,
                    s: 1,
                }; // virtual well
                let min_add_size = (1.0 / (1.0 - well.v / well.s as Real)).ceil() as usize;
                if min_add_size > well.s {
                    continue; // cannot add
                }
                let (add_size, error) =
                    self.merge_2_wells(&base_well, &well, min_error, target, min_add_size);
                if error >= min_error || add_size == well.s {
                    continue; // prune
                }
                let dispatched_well = well.divide(add_size);
                debug_assert!(dispatched_well.s as Real - dispatched_well.v >= 1.0 - EPS);
                min_error = error;
                let mut well = well.clone();
                let mut dispatched_well = dispatched_well.clone();
                best_actions.clear();

                best_actions.push(env.make_toggle_action(self.partitions[i] + add_size));
                well -= dispatched_well.clone();

                best_actions.push(env.make_add_action(self.partitions[i], tube));
                dispatched_well.add_paint(env.tubes[tube]);

                best_actions.push(env.make_deliver_action(self.partitions[i]));
                debug_assert!(dispatched_well.v >= 1.0 - EPS);
                dispatched_well.discard_paint();

                best_actions.push(env.make_toggle_action(self.partitions[i] + add_size));
                well += dispatched_well.clone();

                best_partition0 = self.partition0;

                best_changes.clear();
                best_changes.push((i, well));
            }
        }

        let mut state = self.clone();
        for (i, well) in best_changes {
            state.wells[i] = well;
        }
        state.partition0 = best_partition0;
        state.init_partitions_from_wells();
        state.num_delivers += 1;
        state.num_adds += 1;
        state.num_toggles += 2;
        state.total_error += min_error;
        state.history = HistoryNode::new(state.history.clone(), best_actions);
        selector.add(state);
    }

    fn merge_2_wells(
        &self,
        base_well: &Well,
        add_well: &Well,
        prune_error: Real,
        target: Color,
        min_add_size: usize,
    ) -> (usize, Real) {
        if base_well.v + add_well.v < 1.0 - EPS {
            return (!0, Real::INFINITY); // cannot deliver
        }
        let unit = add_well.divide(1);

        let min_add_size = if base_well.v >= 1.0 - EPS {
            0.max(min_add_size)
        } else {
            (((1.0 - base_well.v) / unit.v).ceil() as usize).max(min_add_size)
        };
        let mut add_size = min_add_size;
        let color_first = (base_well.clone() + unit.clone() * add_size).color();

        if error_lower_bound2(color_first, unit.color(), target) >= prune_error {
            return (!0, Real::INFINITY); // prune
        }
        let mut min_error = color_distance(color_first, target);

        let sqrt_add_well_size = (add_well.s as Real).sqrt() as usize;
        if sqrt_add_well_size >= 3 {
            while add_size + sqrt_add_well_size <= add_well.s {
                let mut well = base_well.clone();
                well += unit.clone() * (add_size + sqrt_add_well_size);
                let color = well.color();
                let error = color_distance(color, target);
                if min_error.chmin(error) {
                    add_size += sqrt_add_well_size;
                } else {
                    break; // no more improvement
                }
            }
        }
        let mut updated = false;
        while add_size + 1 <= add_well.s {
            let mut well = base_well.clone();
            well += unit.clone() * (add_size + 1);
            let color = well.color();
            let error = color_distance(color, target);
            if min_error.chmin(error) {
                add_size += 1;
                updated = true;
            } else {
                break; // no more improvement
            }
        }
        if updated {
            return (add_size, min_error);
        }
        while add_size > min_add_size {
            let mut well = base_well.clone();
            well += unit.clone() * (add_size - 1);
            let color = well.color();
            let error = color_distance(color, target);
            if min_error.chmin(error) {
                add_size -= 1;
            } else {
                break; // no more improvement
            }
        }
        (add_size, min_error)
    }

    fn expand_0_turn(&self, env: &OneDimEnvironment, selector: &mut OneDimSelector) {
        let target = env.targets[self.num_delivers];
        let most_distant_tube = self.most_distant_tube(env);

        let mut min_error = Real::INFINITY;
        let mut best_actions = vec![];
        let mut best_changes = vec![];

        for i in 0..self.wells.len() {
            let well = self.wells[i].clone();

            // add + deliver
            for tube in 0..env.num_tubes {
                if well.s as Real - well.v < 1.0 - EPS {
                    continue; // overflow
                }
                let mut well = well.clone();
                well.add_paint(env.tubes[tube]);
                let color = well.color();
                let error = color_distance(color, target);
                if min_error.chmin(error) {
                    best_actions.clear();

                    best_actions.push(env.make_add_action(self.partitions[i], tube));

                    best_actions.push(env.make_deliver_action(self.partitions[i]));
                    debug_assert!(well.v >= 1.0 - EPS);
                    well.discard_paint();

                    best_changes.clear();
                    best_changes.push((i, well));
                }
            }
            if well.v >= 1.0 - EPS {
                let color = well.color();
                let error = color_distance(color, target);
                if min_error.chmin(error) {
                    let mut well = well.clone();
                    best_actions.clear();

                    best_actions.push(env.make_deliver_action(self.partitions[i]));
                    debug_assert!(well.v >= 1.0 - EPS);
                    well.discard_paint();

                    best_actions.push(env.make_add_action(self.partitions[i], most_distant_tube));
                    well.add_paint(env.tubes[most_distant_tube]);

                    best_changes.clear();
                    best_changes.push((i, well));
                }
            }
        }
        let mut state = self.clone();
        for (i, well) in best_changes {
            state.wells[i] = well;
        }
        state.num_delivers += 1;
        state.num_adds += 1;
        // state.num_toggles += 0;
        state.total_error += min_error;
        state.history = HistoryNode::new(state.history.clone(), best_actions);
        selector.add(state);
    }

    fn most_distant_tube(&self, env: &OneDimEnvironment) -> usize {
        let well_colors = self.wells.iter().map(|w| w.color()).collect_vec();

        let mut max_distance = -1.0;
        let mut most_distant_tube = !0;
        for tube in 0..env.num_tubes {
            let color = env.tubes[tube];
            let mut min_distance = Real::INFINITY;
            for well_color in &well_colors {
                min_distance.chmin(color_distance(*well_color, color));
            }
            if max_distance.chmax(min_distance) {
                most_distant_tube = tube;
            }
        }
        most_distant_tube
    }

    fn expand(&self, env: &OneDimEnvironment, selector: &mut OneDimSelector) {
        self.expand_0_turn(env, selector);
        self.expand_2_turn(env, selector);
        self.expand_no_add(env, selector);
        if get_time_sec() < TIME_LIMIT_SEC {
            self.expand_4_turn(env, selector);
        }
    }

    fn history(&self) -> Vec<Action> {
        self.history.clone().unwrap().history()
    }
}

#[derive(Clone, Debug)]
struct OneDimEnvironment {
    num_tubes: usize,
    // num_turns: usize,
    cost_per_gram: Real,
    tubes: Vec<Color>,
    targets: Vec<Color>,
    max_extra_turns: usize,
    num_cells: usize,
    cells: Vec<usize>, // one-dimensional cells
    v_walls: [[bool; N - 1]; N],
    h_walls: [[bool; N]; N - 1],
    well_sizes_first: Vec<usize>,
}

impl OneDimEnvironment {
    fn new(input: &Input) -> Self {
        let max_extra_turns = input.num_turns - 2 * NUM_TARGETS;

        // snake
        let num_cells = N2;
        let mut cells = Vec::with_capacity(num_cells);
        for x in 0..N {
            if x % 2 == 0 {
                for y in 1..N {
                    cells.push(encode(x, y));
                }
            } else {
                for y in (1..N).rev() {
                    cells.push(encode(x, y));
                }
            }
        }
        for x in (0..N).rev() {
            cells.push(encode(x, 0));
        }
        debug_assert_eq!(cells.len(), num_cells);

        let mut v_walls = [[false; N - 1]; N];
        let mut h_walls = [[false; N]; N - 1];
        for x in 0..N - 1 {
            if x % 2 == 0 {
                for y in 1..N - 1 {
                    h_walls[x][y] = true;
                }
            } else {
                for y in 2..N {
                    h_walls[x][y] = true;
                }
            }
        }
        for x in 0..N - 1 {
            v_walls[x][0] = true;
        }
        let well_sizes_first = Self::init_well_sizes();
        let mut i = well_sizes_first[0];
        for &s in &well_sizes_first[1..] {
            let u = cells[i - 1];
            let v = cells[i];
            if u + 1 == v {
                let (ux, uy) = decode(u);
                v_walls[ux][uy] = true;
            } else if u == v + 1 {
                let (vx, vy) = decode(v);
                v_walls[vx][vy] = true;
            } else if u + N == v {
                let (ux, uy) = decode(u);
                h_walls[ux][uy] = true;
            } else if u == v + N {
                let (vx, vy) = decode(v);
                h_walls[vx][vy] = true;
            } else {
                unreachable!();
            }
            i += s;
        }
        Self {
            num_tubes: input.num_tubes,
            // num_turns: input.num_turns,
            cost_per_gram: input.cost_per_gram,
            tubes: input.tubes.clone(),
            targets: input.targets.clone(),
            max_extra_turns,
            num_cells,
            cells,
            v_walls,
            h_walls,
            well_sizes_first,
        }
    }

    fn init_well_sizes() -> Vec<usize> {
        let num_wells = 40;

        let mut well_sizes = vec![0; num_wells];
        for i in 0..num_wells {
            well_sizes[i] = (i + 1) * N2 / num_wells - i * N2 / num_wells;
        }
        debug_assert_eq!(well_sizes.iter().sum::<usize>(), N2);
        well_sizes
    }

    fn make_add_action(&self, i: usize, tube: usize) -> Action {
        Action::Add {
            v: self.cells[i % self.num_cells],
            tube,
        }
    }

    fn make_deliver_action(&self, i: usize) -> Action {
        Action::Deliver {
            v: self.cells[i % self.num_cells],
        }
    }

    // toggle the wall between cells[i - 1] and cells[i]
    fn make_toggle_action(&self, i: usize) -> Action {
        Action::Toggle {
            v: self.cells[(i + self.num_cells - 1) % self.num_cells],
            u: self.cells[i % self.num_cells],
        }
    }

    fn print_walls(&self) {
        for x in 0..N {
            for y in 0..N - 1 {
                if self.v_walls[x][y] {
                    print!("1");
                } else {
                    print!("0");
                }
                if y < N - 2 {
                    print!(" ");
                } else {
                    println!();
                }
            }
        }
        for x in 0..N - 1 {
            for y in 0..N {
                if self.h_walls[x][y] {
                    print!("1");
                } else {
                    print!("0");
                }
                if y < N - 1 {
                    print!(" ");
                } else {
                    println!();
                }
            }
        }
    }
}

fn comb_beam(input: &Input) {
    let comb_env = CombEnvironment::new(&input);

    let mut selector = CombSelector::new(&comb_env);
    let mut states = vec![CombState::new(&comb_env)];
    for _ in 0..input.targets.len() {
        for state in &states {
            state.expand(&comb_env, &mut selector);
        }
        states = selector.select(&comb_env);
    }

    let state = states
        .into_iter()
        .min_by(|a, b| {
            a.evaluate(&comb_env)
                .partial_cmp(&b.evaluate(&comb_env))
                .unwrap()
        })
        .unwrap();

    comb_env.print_walls();
    let history = state.history();
    for action in history {
        print_action(action);
    }
}

#[derive(Clone, Debug)]
struct CombSelector {
    beam_width: usize,
    candidates: Vec<Vec<CombState>>,
    target: usize,
    low_extra_turns: usize,
    high_extra_turns: usize,
    num_states: usize,
    branch_beam_width: usize,
    expand_start_time: f64,
    avg_time: f64, // average time per branch_beam_width
}

impl CombSelector {
    fn new(env: &CombEnvironment) -> Self {
        const BEAM_WIDTH: usize = 1;
        const DEFAULT_BRANCH_BEAM_WIDTH: usize = 5;
        const DEFAULT_AVG_TIME: f64 =
            TIME_LIMIT_SEC / (NUM_TARGETS * BEAM_WIDTH * DEFAULT_BRANCH_BEAM_WIDTH) as f64;

        let target = 0;
        let high_extra_turns = (target + 1) * env.max_extra_turns / NUM_TARGETS;
        let low_extra_turns = high_extra_turns.saturating_sub(BEAM_WIDTH - 1);

        Self {
            beam_width: BEAM_WIDTH,
            candidates: vec![vec![]; env.max_extra_turns + 1],
            target,
            low_extra_turns,
            high_extra_turns,
            num_states: 1,
            branch_beam_width: DEFAULT_BRANCH_BEAM_WIDTH,
            expand_start_time: get_time_sec(),
            avg_time: DEFAULT_AVG_TIME,
        }
    }

    fn add(&mut self, state: CombState) {
        let extra_turns = state.extra_turns().max(self.low_extra_turns);
        if extra_turns > self.high_extra_turns {
            return;
        }
        self.candidates[extra_turns].push(state);
    }

    fn select(&mut self, env: &CombEnvironment) -> Vec<CombState> {
        let mut que = BinaryHeap::new();
        let mut ret = Vec::with_capacity(self.beam_width);
        for extra_turn in self.low_extra_turns..=self.high_extra_turns {
            for (i, state) in self.candidates[extra_turn].iter().enumerate() {
                let cost = state.evaluate(env);
                let cost = (-1e9 * cost) as i64;
                que.push((cost, extra_turn, i));
            }
            if let Some((_, extra_turn, i)) = que.pop() {
                ret.push(self.candidates[extra_turn][i].clone());
            }
        }
        for extra_turn in self.low_extra_turns..=self.high_extra_turns {
            self.candidates[extra_turn].clear();
        }
        self.update(env, ret.len());
        ret
    }

    fn update(&mut self, env: &CombEnvironment, new_num_states: usize) {
        self.target += 1;
        self.high_extra_turns = (self.target + 1) * env.max_extra_turns / NUM_TARGETS;
        self.low_extra_turns = self.high_extra_turns.saturating_sub(self.beam_width - 1);

        const TARGET_MARGIN: usize = 10;
        const AVG_DECAY: f64 = 0.75;
        let expand_end_time = get_time_sec();
        let avg_time = (expand_end_time - self.expand_start_time)
            / (self.branch_beam_width * self.num_states) as f64;
        self.avg_time = AVG_DECAY * self.avg_time + (1.0 - AVG_DECAY) * avg_time;
        let remaining_time = BEAM_END_TIME - expand_end_time;
        let remaining_time_per_width =
            remaining_time / ((NUM_TARGETS + TARGET_MARGIN - self.target) * self.beam_width) as f64;
        self.branch_beam_width = (remaining_time_per_width / self.avg_time).floor() as usize;
        self.branch_beam_width.chmax(1);
        self.num_states = new_num_states;
        self.expand_start_time = expand_end_time;
    }
}

#[derive(Clone, Debug)]
struct Mixer {
    stem: Well,
    branches: Vec<usize>,
    max_pieces: Vec<usize>,
    volumes: Vec<Real>,
    pigments: Vec<Color>,
}

impl Mixer {
    fn new(stem: Well) -> Self {
        Self {
            stem,
            branches: vec![],
            max_pieces: vec![],
            volumes: vec![],
            pigments: vec![],
        }
    }

    fn add_well(&mut self, branch: usize, well: &Well) {
        debug_assert!(well.s > 0);
        debug_assert!(well.v > 0.0);
        let max_piece = well.s;
        let volume = well.v / max_piece as Real;
        let pigment = [
            well.p[0] / max_piece as Real,
            well.p[1] / max_piece as Real,
            well.p[2] / max_piece as Real,
        ];
        self.branches.push(branch);
        self.max_pieces.push(max_piece);
        self.volumes.push(volume);
        self.pigments.push(pigment);
    }

    fn sub_stem(&mut self, stem: Well) {
        self.stem -= stem;
    }

    fn total_volume(&self, pieces: &[usize]) -> Real {
        let mut total_volume = self.stem.v;
        for (i, &piece) in pieces.iter().enumerate() {
            if piece > 0 {
                total_volume += self.volumes[i] * piece as Real;
            }
        }
        total_volume
    }

    fn total_pigment(&self, pieces: &[usize]) -> Color {
        let mut total_pigment = self.stem.p;
        for (i, &piece) in pieces.iter().enumerate() {
            if piece > 0 {
                total_pigment[0] += self.pigments[i][0] * piece as Real;
                total_pigment[1] += self.pigments[i][1] * piece as Real;
                total_pigment[2] += self.pigments[i][2] * piece as Real;
            }
        }
        total_pigment
    }

    fn greedy_add(&self, pieces: &mut [usize], target: Color) -> usize {
        let n = pieces.len();
        let mut total_volume = self.total_volume(pieces);
        let mut total_pigment = self.total_pigment(pieces);

        while total_volume < 1.0 - EPS {
            let mut add_branch = !0;
            let mut min_error = Real::INFINITY;
            for i in 0..n {
                let nvolume = total_volume + self.volumes[i];
                let npigment = [
                    total_pigment[0] + self.pigments[i][0],
                    total_pigment[1] + self.pigments[i][1],
                    total_pigment[2] + self.pigments[i][2],
                ];
                let ncolor = pigment_to_color(npigment, nvolume);
                let error = color_distance(ncolor, target);
                if min_error.chmin(error) {
                    add_branch = i;
                }
            }
            if pieces[add_branch] >= self.max_pieces[add_branch] {
                return add_branch; // cannot add more pieces
            }
            pieces[add_branch] += 1;
            total_volume += self.volumes[add_branch];
            total_pigment[0] += self.pigments[add_branch][0];
            total_pigment[1] += self.pigments[add_branch][1];
            total_pigment[2] += self.pigments[add_branch][2];
        }
        !0
    }

    fn greedy_remove(&self, pieces: &mut [usize], target: Color) {
        let n = pieces.len();
        let mut total_volume = self.total_volume(pieces);
        let mut total_pigment = self.total_pigment(pieces);

        // remove greedily
        if total_volume < 1.0 {
            return; // cannot remove
        }
        loop {
            let mut remove_branch = !0;
            let mut min_error = Real::INFINITY;
            for i in 0..n {
                if pieces[i] == 0 {
                    continue;
                }
                let nvolume = total_volume - self.volumes[i];
                if nvolume < 1.0 - EPS {
                    continue;
                }
                let npigment = [
                    total_pigment[0] - self.pigments[i][0],
                    total_pigment[1] - self.pigments[i][1],
                    total_pigment[2] - self.pigments[i][2],
                ];
                let ncolor = pigment_to_color(npigment, nvolume);
                let error = color_distance(ncolor, target);
                if min_error.chmin(error) {
                    remove_branch = i;
                }
            }
            if remove_branch == !0 {
                break; // no more removable branches
            }
            pieces[remove_branch] -= 1;
            total_volume -= self.volumes[remove_branch];
            total_pigment[0] -= self.pigments[remove_branch][0];
            total_pigment[1] -= self.pigments[remove_branch][1];
            total_pigment[2] -= self.pigments[remove_branch][2];
            debug_assert!(total_volume >= 1.0 - EPS);
        }
    }

    fn greedy_replace(&self, pieces: &mut [usize], target: Color) {
        let n = pieces.len();
        let mut total_volume = self.total_volume(pieces);
        debug_assert!(total_volume >= 1.0 - EPS);
        let mut total_pigment = self.total_pigment(pieces);
        let mut error = color_distance(pigment_to_color(total_pigment, total_volume), target);
        let mut patience = n * n;
        loop {
            for i in 0..n {
                if pieces[i] == 0 {
                    patience = patience.saturating_sub(n);
                    if patience == 0 {
                        return; // no more replacements
                    }
                    continue; // cannot remove
                }
                let mut j = 0;
                while j < n && patience > 0 && pieces[i] > 0 {
                    if i == j || pieces[j] >= self.max_pieces[j] {
                        j += 1;
                        patience -= 1;
                        continue; // cannot add
                    }
                    let (ci, cj, nvolume) = if self.volumes[i] < self.volumes[j] {
                        let mut nvolume = total_volume + self.volumes[j];
                        let mut ci = 0;
                        while nvolume - self.volumes[i] >= 1.0 - EPS && pieces[i] - ci > 0 {
                            ci += 1;
                            nvolume -= self.volumes[i];
                        }
                        (ci, 1, nvolume)
                    } else {
                        let mut nvolume = total_volume - self.volumes[i];
                        let mut cj = 0;
                        while nvolume < 1.0 - EPS && pieces[j] + cj < self.max_pieces[j] {
                            cj += 1;
                            nvolume += self.volumes[j];
                        }
                        (1, cj, nvolume)
                    };
                    if nvolume < 1.0 - EPS {
                        j += 1;
                        patience -= 1;
                        if patience == 0 {
                            return; // no more replacements
                        }
                        continue;
                    }
                    let npigment = [
                        total_pigment[0] - self.pigments[i][0] * ci as Real
                            + self.pigments[j][0] * cj as Real,
                        total_pigment[1] - self.pigments[i][1] * ci as Real
                            + self.pigments[j][1] * cj as Real,
                        total_pigment[2] - self.pigments[i][2] * ci as Real
                            + self.pigments[j][2] * cj as Real,
                    ];
                    let ncolor = pigment_to_color(npigment, nvolume);
                    let nerror = color_distance(ncolor, target);
                    if nerror < error - EPS {
                        // replace
                        pieces[i] -= ci;
                        pieces[j] += cj;
                        total_volume = nvolume;
                        total_pigment = npigment;
                        error = nerror;
                        patience = n * n; // reset patience
                    } else {
                        j += 1;
                        patience -= 1;
                        if patience == 0 {
                            return; // no more replacements
                        }
                    }
                }
                if patience == 0 {
                    return; // no more replacements
                }
            }
        }
    }

    fn beam_search(&self, target: Color, env: &CombEnvironment) -> Vec<usize> {
        #[derive(Clone, Debug)]
        struct Pieces {
            volume: Real,
            pigment: Color,
            pieces: Vec<usize>,
            hash: u64,
        }
        let beam_width = (env.max_extra_turns.saturating_sub(8000) / 300).clamp(5, 40);

        let mut states = vec![Pieces {
            volume: self.stem.v,
            pigment: self.stem.p,
            pieces: vec![0; self.branches.len()],
            hash: 0,
        }];
        let mut nstates = vec![];
        let mut hashes = NoHashSet::default();

        let mut best_pieces = vec![];
        let mut min_error = Real::INFINITY;

        while !states.is_empty() {
            let mut candidates = vec![];
            hashes.clear();
            for state_id in 0..states.len() {
                let state = &states[state_id];
                for branch in 0..self.branches.len() {
                    if state.pieces[branch] >= self.max_pieces[branch] {
                        continue; // cannot add more pieces
                    }
                    let nhash = state.hash.wrapping_add(env.rands[branch]);
                    if !hashes.insert(nhash) {
                        continue; // already visited
                    }
                    let nvolume = state.volume + self.volumes[branch];
                    let npigment = [
                        state.pigment[0] + self.pigments[branch][0],
                        state.pigment[1] + self.pigments[branch][1],
                        state.pigment[2] + self.pigments[branch][2],
                    ];
                    let ncolor = pigment_to_color(npigment, nvolume);
                    let nerror = color_distance(ncolor, target);
                    if nvolume < 1.0 - EPS {
                        candidates.push((nerror, state_id as u32, branch as u32));
                        continue; // cannot deliver
                    }
                    if min_error.chmin(nerror) {
                        best_pieces = state.pieces.clone();
                        best_pieces[branch] += 1;
                    }
                }
            }
            candidates.sort_unstable_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
            if candidates.len() > beam_width {
                candidates.truncate(beam_width);
            }
            nstates.clear();
            for (_, state_id, branch) in candidates {
                let mut state = states[state_id as usize].clone();
                let branch = branch as usize;
                state.pieces[branch] += 1;
                state.volume += self.volumes[branch];
                state.pigment[0] += self.pigments[branch][0];
                state.pigment[1] += self.pigments[branch][1];
                state.pigment[2] += self.pigments[branch][2];
                state.hash = state.hash.wrapping_add(env.rands[branch]);
                nstates.push(state);
            }
            std::mem::swap(&mut states, &mut nstates);
        }
        debug_assert!(!best_pieces.is_empty());
        best_pieces
    }

    fn verify_pieces(&self, pieces: &[usize]) -> bool {
        if self.branches.len() != pieces.len() {
            return false;
        }
        for (i, &piece) in pieces.iter().enumerate() {
            if piece > self.max_pieces[i] {
                return false; // too many pieces
            }
        }
        true
    }
}

#[derive(Clone, Debug)]
struct BranchSelector {
    beam_width: usize,
    selected_states: Vec<Option<BranchState>>,
    candidates: Vec<BranchState>,
}

impl BranchSelector {
    fn new(beam_width: usize) -> Self {
        Self {
            beam_width,
            selected_states: vec![],
            candidates: vec![],
        }
    }

    fn add(&mut self, state: BranchState) {
        if state.valid {
            let i = state.d_extra_turns;
            while self.selected_states.len() <= i {
                self.selected_states.push(None);
            }
            if self.selected_states[i].is_none()
                || self.selected_states[i].as_ref().unwrap().cost > state.cost
            {
                self.selected_states[i] = Some(state.clone());
            }
        }
        self.candidates.push(state);
    }

    fn select(&mut self) -> Vec<BranchState> {
        self.candidates
            .sort_unstable_by(|a, b| b.cost.partial_cmp(&a.cost).unwrap());
        let mut last_d_extra_turns = 0;
        let mut ret = vec![];
        let mut rejected_states = VecDeque::new();
        let mut hashes = FxHashSet::default();
        while let Some(state) = self.candidates.pop() {
            if ret.is_empty() || last_d_extra_turns > state.d_extra_turns {
                last_d_extra_turns = state.d_extra_turns;
                hashes.insert(state.branch_bitset);
                ret.push(state);
                if ret.len() >= self.beam_width {
                    break;
                }
            } else {
                rejected_states.push_back(state);
            }
        }
        let mut new_rejected_states = VecDeque::new();
        while ret.len() < self.beam_width && !rejected_states.is_empty() {
            let state = rejected_states.pop_front().unwrap();
            if hashes.insert(state.branch_bitset) {
                ret.push(state);
            } else {
                new_rejected_states.push_back(state);
            }
        }
        rejected_states = new_rejected_states;
        while ret.len() < self.beam_width && !rejected_states.is_empty() {
            let state = rejected_states.pop_front().unwrap();
            ret.push(state);
        }
        self.candidates.clear();

        ret
    }
}

#[derive(Clone, Debug)]
struct BranchState {
    branch_bitset: u32,
    refill_bitset: u32,
    branches: Vec<usize>,
    pieces: Vec<usize>,
    valid: bool,
    cost: Real,
    d_extra_turns: usize,
}

impl BranchState {
    fn new() -> Self {
        Self {
            branch_bitset: 0,
            refill_bitset: 0,
            branches: vec![],
            pieces: vec![],
            valid: false,
            cost: Real::INFINITY,
            d_extra_turns: 0,
        }
    }

    fn is_used(&self, branch: usize) -> bool {
        self.branch_bitset & (1 << branch) != 0
    }

    fn is_refill(&self, branch: usize) -> bool {
        self.refill_bitset & (1 << branch) != 0
    }

    fn num_refills(&self) -> usize {
        self.refill_bitset.count_ones() as usize
    }

    fn push_branch(&mut self, branch: usize, refill: bool) {
        debug_assert!(!self.is_used(branch));
        self.branch_bitset |= 1 << branch;
        if refill {
            self.refill_bitset |= 1 << branch;
        }
        self.branches.push(branch);
        self.pieces.push(0); // initially 0 pieces

        // reset state
        self.valid = false;
        self.cost = Real::INFINITY;
        self.d_extra_turns = 0;
    }

    fn remove_unused_branches(&mut self) -> bool {
        let mut l = 0;
        for r in 0..self.branches.len() {
            if self.pieces[r] > 0 {
                self.branches[l] = self.branches[r];
                self.pieces[l] = self.pieces[r];
                l += 1;
            } else {
                self.branch_bitset &= !(1 << self.branches[r]);
                self.refill_bitset &= !(1 << self.branches[r]);
            }
        }
        let ret = l < self.branches.len();
        self.branches.truncate(l);
        self.pieces.truncate(l);
        ret
    }
}

#[derive(Clone, Debug)]
struct CombState {
    num_delivers: usize,
    num_adds: usize,
    num_toggles: usize,
    total_error: Real,
    wells: Vec<Well>, // for each branch
    stem: Well,
    history: Option<Rc<HistoryNode>>,
}

impl CombState {
    fn new(env: &CombEnvironment) -> Self {
        let wells = (0..env.num_tubes)
            .map(|tube| Well {
                p: env.tubes[tube],
                v: 1.0,
                s: env.branch_size(tube),
            })
            .collect_vec();

        let stem = Well {
            p: [0.0; 3],
            v: 0.0,
            s: env.stem_size,
        };

        let mut actions = Vec::with_capacity(env.num_tubes);
        for tube in 0..env.num_tubes {
            actions.push(Action::Add {
                v: env.entry(tube),
                tube,
            });
        }
        Self {
            num_delivers: 0,
            num_adds: env.num_tubes,
            num_toggles: 0,
            total_error: 0.0,
            wells,
            stem,
            history: HistoryNode::new(None, actions),
        }
    }

    fn evaluate(&self, env: &CombEnvironment) -> Real {
        self.total_error + self.num_adds.saturating_sub(NUM_TARGETS) as Real * env.cost_per_gram
    }

    fn extra_turns(&self) -> usize {
        self.num_toggles + self.num_adds.saturating_sub(NUM_TARGETS)
    }

    fn evaluate_next(&self, error: Real, d_num_adds: usize, env: &CombEnvironment) -> Real {
        self.total_error
            + error
            + (self.num_adds + d_num_adds).saturating_sub(NUM_TARGETS) as Real * env.cost_per_gram
    }

    fn enumerate_next_branches(
        &self,
        state: &BranchState,
        env: &CombEnvironment,
    ) -> Vec<BranchState> {
        let mut next_branches = vec![];
        for branch in 0..env.num_tubes {
            if state.is_used(branch) {
                continue;
            }
            let mut nstate = state.clone();
            nstate.push_branch(branch, self.wells[branch].s == 0);
            next_branches.push(nstate);
        }
        next_branches
    }

    fn build_mixer(&self, state: &BranchState, env: &CombEnvironment) -> Mixer {
        let mut mixer = Mixer::new(self.stem.clone());
        for &branch in &state.branches {
            let mut well = self.wells[branch].clone();
            if state.is_refill(branch) {
                let detached_well = self.stem.divide(env.branch_size(branch) - well.s);
                mixer.sub_stem(detached_well.clone());
                well += detached_well;
                for _ in 0..env.add_unit {
                    well.add_paint(env.tubes[branch]);
                }
            }
            mixer.add_well(branch, &well);
        }
        mixer
    }

    fn set_branch_cost(&self, state: &mut BranchState, mixer: &Mixer, env: &CombEnvironment) {
        let total_volume = mixer.total_volume(&state.pieces);
        let valid = total_volume >= 1.0 - EPS;
        let total_pigment = mixer.total_pigment(&state.pieces);
        let target = env.targets[self.num_delivers];
        let error = if valid {
            let color = pigment_to_color(total_pigment, total_volume);
            color_distance(color, target)
        } else {
            let mut error = 0.0;
            for i in 0..3 {
                let diff = (target[i] - total_pigment[i])
                    .max(total_pigment[i] + 1.0 - total_volume - target[i]);
                error += diff * diff;
            }
            error.sqrt()
        };
        let cost = self.evaluate_next(error, state.num_refills(), env);

        state.valid = valid;
        state.cost = cost;
    }

    fn set_branch_d_extra_turns(&self, state: &mut BranchState, mixer: &Mixer) {
        let mut d_extra_turns = 0;
        for i in 0..state.branches.len() {
            let branch = state.branches[i];
            // debug_assert!(state.pieces[i] > 0);
            if state.is_refill(branch) {
                if self.wells[branch].s == 0 {
                    d_extra_turns += 1; // close (+ add)
                } else {
                    d_extra_turns += 2; // refill (+ add)
                }
            }
            if state.pieces[i] == mixer.max_pieces[i] {
                d_extra_turns += 1; // open
            } else if state.pieces[i] > 0 {
                d_extra_turns += 2; // use
            }
        }
        let num_adds = state.num_refills();
        if self.num_adds >= NUM_TARGETS {
            d_extra_turns += num_adds;
        } else if self.num_adds + num_adds >= NUM_TARGETS {
            d_extra_turns += self.num_adds + num_adds - NUM_TARGETS;
        }

        state.d_extra_turns = d_extra_turns;
    }

    fn make_comb_actions(
        &self,
        state: &BranchState,
        mixer: &Mixer,
        env: &CombEnvironment,
    ) -> Vec<CombAction> {
        let mut comb_actions = vec![];
        for i in 0..state.branches.len() {
            let branch = state.branches[i];
            if state.is_refill(branch) {
                if self.wells[branch].s == 0 {
                    comb_actions.push(CombAction::Close { branch });
                } else {
                    comb_actions.push(CombAction::Refill { branch });
                }
                for _ in 0..env.add_unit {
                    comb_actions.push(CombAction::Add {
                        branch,
                        tube: branch,
                    });
                }
            }
            if state.pieces[i] == mixer.max_pieces[i] {
                comb_actions.push(CombAction::Open { branch });
            } else if state.pieces[i] > 0 {
                comb_actions.push(CombAction::Use {
                    branch,
                    s: state.pieces[i],
                });
            }
        }
        comb_actions
    }

    fn hill_climb_branches(
        &self,
        mut state: BranchState,
        acceptable_cost: Real,
        selector: &mut BranchSelector,
        env: &CombEnvironment,
    ) {
        const NUM_ADD_PIECES: usize = 2;

        let mut i = state.pieces.len() - 1;
        let target = env.targets[self.num_delivers];
        let mut mixer = self.build_mixer(&state, env);
        debug_assert!(mixer.verify_pieces(&state.pieces));

        let volume = mixer.total_volume(&state.pieces);
        if volume < 1.0 - EPS {
            state.pieces[i] += ((1.0 - volume) / mixer.volumes[i]).ceil() as usize;
            state.pieces[i].chmin(mixer.max_pieces[i]);
        }
        debug_assert!(mixer.verify_pieces(&state.pieces));

        // hill climbing
        while state.pieces[i] < mixer.max_pieces[i] {
            // try to add new branch pieces
            let opieces = state.pieces.clone();
            state.pieces[i] = (state.pieces[i] + NUM_ADD_PIECES).min(mixer.max_pieces[i]);

            // greedy
            mixer.greedy_remove(&mut state.pieces, target);

            if state.pieces[i] == 0 {
                return;
            }
            if state.pieces == opieces {
                break; // not updated
            }

            if state.remove_unused_branches() {
                mixer = self.build_mixer(&state, env);
                i = state.branches.len() - 1;
            }

            // remove useless refill
            for j in 0..state.branches.len() - 1 {
                let branch = state.branches[j];
                if !state.is_refill(branch) {
                    continue;
                }
                let volume = state.pieces[j] as Real * mixer.volumes[j];
                if volume > self.wells[branch].v + EPS {
                    continue;
                }
                state.refill_bitset &= !(1 << branch);
                state.pieces[j] = self.wells[branch].s;
                self.set_branch_cost(&mut state, &mixer, env);
                if state.cost > acceptable_cost {
                    return;
                }
                self.set_branch_d_extra_turns(&mut state, &mixer);
                selector.add(state.clone());
                return self.hill_climb_branches(state, acceptable_cost, selector, env);
            }
        }

        if state.remove_unused_branches() {
            mixer = self.build_mixer(&state, env);
            i = state.branches.len() - 1;
        }

        self.set_branch_cost(&mut state, &mixer, env);
        if state.cost > acceptable_cost {
            return;
        }
        self.set_branch_d_extra_turns(&mut state, &mixer);
        selector.add(state.clone());

        if mixer.max_pieces[i] - state.pieces[i] >= NUM_ADD_PIECES
            || state.is_refill(state.branches[i])
        {
            return;
        }
        // refill new branch
        state.refill_bitset |= 1 << state.branches[i];
        state.pieces[i] = 0;
        return self.hill_climb_branches(state, acceptable_cost, selector, env);
    }

    fn expand(&self, env: &CombEnvironment, selector: &mut CombSelector) {
        self.search_direct_deliver(env, selector);
        self.search_all_usable(env, selector);

        let mut branch_selector = BranchSelector::new(selector.branch_beam_width);
        let mut states = vec![BranchState::new()];
        while !states.is_empty() {
            while let Some(state) = states.pop() {
                let acceptable_cost = state.cost - EPS;
                for nstate in self.enumerate_next_branches(&state, env) {
                    self.hill_climb_branches(nstate, acceptable_cost, &mut branch_selector, env);
                }
            }
            states = branch_selector.select();
        }
        for state in branch_selector.selected_states {
            if state.is_none() {
                continue;
            }
            let state = state.unwrap();
            debug_assert!(state.valid);
            let mixer = self.build_mixer(&state, env);
            let comb_actions = self.make_comb_actions(&state, &mixer, env);
            let mut nstate = self.clone();
            nstate.apply(comb_actions, env);
            selector.add(nstate);
        }
    }

    fn search_all_usable(&self, env: &CombEnvironment, selector: &mut CombSelector) {
        let target = env.targets[self.num_delivers];
        let mut state = BranchState::new();
        for branch in 0..env.num_tubes {
            state.push_branch(branch, self.wells[branch].s == 0);
        }
        let mut mixer = self.build_mixer(&state, env);
        loop {
            let i = mixer.greedy_add(&mut state.pieces, target);
            if i == !0 {
                break;
            }
            debug_assert!(!state.is_refill(state.branches[i]));
            let ovolume = mixer.volumes[i] * state.pieces[i] as Real;
            state.refill_bitset |= 1 << state.branches[i];
            mixer = self.build_mixer(&state, env);
            state.pieces[i] = (ovolume / mixer.volumes[i]).floor() as usize;
        }
        debug_assert!(mixer.total_volume(&state.pieces) >= 1.0 - EPS);
        debug_assert!(mixer.verify_pieces(&state.pieces));
        mixer.greedy_replace(&mut state.pieces, target);
        debug_assert!(mixer.total_volume(&state.pieces) >= 1.0 - EPS);
        debug_assert!(mixer.verify_pieces(&state.pieces));
        let comb_actions = self.make_comb_actions(&state, &mixer, env);
        let mut nstate = self.clone();
        nstate.apply(comb_actions, env);
        selector.add(nstate);

        state.pieces = mixer.beam_search(target, env);
        let comb_actions = self.make_comb_actions(&state, &mixer, env);
        let mut nstate = self.clone();
        nstate.apply(comb_actions, env);
        selector.add(nstate);
    }

    fn search_direct_deliver(&self, env: &CombEnvironment, selector: &mut CombSelector) {
        let mut min_cost = Real::INFINITY;
        let mut best_branch = !0;
        let mut best_tube = !0;

        // branch = tube
        for tube in 0..env.num_tubes {
            let mut well = self.wells[tube].clone();
            if well.s == 0 {
                continue;
            }
            // if well.s as Real <= well.v + 1.0 {
            //     // overflow
            //     continue;
            // }
            well.add_paint(env.tubes[tube]);
            let error = color_distance(well.color(), env.targets[self.num_delivers]);
            let cost = self.evaluate_next(error, 1, env);
            if min_cost.chmin(cost) {
                best_branch = tube;
                best_tube = tube;
            }
        }

        // branch = stem
        for tube in 0..env.num_tubes {
            let mut well = self.stem.clone();
            well.add_paint(env.tubes[tube]);
            let error = color_distance(well.color(), env.targets[self.num_delivers]);
            let cost = self.evaluate_next(error, 1, env);
            if min_cost.chmin(cost) {
                best_branch = !0; // stem
                best_tube = tube;
            }
        }

        let branch = best_branch;
        let tube = best_tube;
        let actions = vec![
            CombAction::Add { branch, tube },
            CombAction::DirectDeliver { branch },
        ];
        let mut state = self.clone();
        state.apply(actions, env);
        selector.add(state);
    }

    fn apply(&mut self, mut comb_actions: Vec<CombAction>, env: &CombEnvironment) {
        sort_comb_actions(&mut comb_actions);

        let mut actions = vec![];
        let mut deliver_branch = !0;

        for &comb_action in comb_actions.iter() {
            match comb_action {
                CombAction::Close { branch } => {
                    let well = &mut self.wells[branch];
                    debug_assert_eq!(well.s, 0);

                    actions.push(env.make_lid_toggle_action(branch));
                    let detached_well = self.stem.divide(env.branch_size(branch));
                    self.stem -= detached_well.clone();
                    *well = detached_well;
                }
                CombAction::Refill { branch } => {
                    let well = &mut self.wells[branch];
                    debug_assert!(0 < well.s && well.s < env.branch_size(branch));

                    actions.push(env.make_lid_toggle_action(branch));
                    let ds = env.branch_size(branch) - well.s;
                    let detached_well = self.stem.divide(ds);
                    self.stem -= detached_well.clone();

                    actions.push(env.make_toggle_action(branch, well.s));
                    *well += detached_well;
                }
                CombAction::Add { branch, tube } => {
                    let well = if branch < self.wells.len() {
                        &mut self.wells[branch]
                    } else {
                        &mut self.stem
                    };
                    actions.push(env.make_add_action(branch, tube));
                    well.add_paint(env.tubes[tube]);
                    self.num_adds += 1;
                }
                CombAction::Use { branch, s } => {
                    let well = &mut self.wells[branch];
                    debug_assert!(s < well.s);

                    actions.push(env.make_toggle_action(branch, well.s - s));
                    let detached_well = well.divide(s);
                    *well -= detached_well.clone();

                    actions.push(env.make_toggle_action(branch, well.s + s));
                    self.stem += detached_well;
                }
                CombAction::Open { branch } => {
                    let well = &mut self.wells[branch];
                    debug_assert!(well.s > 0);

                    actions.push(env.make_toggle_action(branch, well.s));
                    self.stem += well.clone();
                    *well = Well::zero();
                }
                CombAction::DirectDeliver { branch } => {
                    debug_assert_eq!(deliver_branch, !0);
                    deliver_branch = branch;
                }
            }
        }
        self.num_toggles += count_toggles(&actions);

        // deliver
        let well = if deliver_branch == !0 {
            actions.push(Action::Deliver { v: env.stem_entry });
            &mut self.stem
        } else {
            actions.push(Action::Deliver {
                v: env.entry(deliver_branch),
            });
            &mut self.wells[deliver_branch]
        };
        debug_assert!(well.v >= 1.0 - EPS);
        let c = well.color();
        self.total_error += color_distance(c, env.targets[self.num_delivers]);
        well.discard_paint();
        self.num_delivers += 1;

        // update history
        self.history = HistoryNode::new(self.history.clone(), actions);
    }

    fn history(&self) -> Vec<Action> {
        self.history.clone().unwrap().history()
    }
}

#[derive(Clone, Debug)]
struct CombEnvironment {
    num_tubes: usize,
    // num_turns: usize,
    cost_per_gram: Real,
    tubes: Vec<Color>,
    targets: Vec<Color>,
    max_extra_turns: usize,
    branch_cells: Vec<Vec<usize>>, // for each tube, from leaf to stem
    stem_entry: usize,
    stem_size: usize,
    v_walls: [[bool; N - 1]; N],
    h_walls: [[bool; N]; N - 1],
    add_unit: usize, // number of additions per refill
    rands: Vec<u64>,
}

impl CombEnvironment {
    fn new(input: &Input) -> Self {
        let max_extra_turns = input.num_turns - 2 * NUM_TARGETS;

        let mut v_walls = [[false; N - 1]; N];
        for x in 0..N {
            v_walls[x][N - 2] = true;
        }
        let mut h_walls = [[false; N]; N - 1];
        for x in 0..N - 1 {
            for y in 0..N - 1 {
                h_walls[x][y] = true;
            }
        }
        let rows = Self::assgin_rows(input);
        let mut branch_cells = vec![vec![]; input.num_tubes];
        let mut sx = 0;
        for branch in 0..input.num_tubes {
            if branch == input.num_tubes - 1 {
                debug_assert_eq!(sx + rows[branch], N);
                for x in (sx..N).rev() {
                    if x % 2 == 0 {
                        if x < N - 1 {
                            h_walls[x][0] = false;
                        }
                        for y in 0..N - 1 {
                            branch_cells[branch].push(encode(x, y));
                        }
                    } else {
                        if x < N - 1 {
                            h_walls[x][N - 2] = false;
                        }
                        for y in (0..N - 1).rev() {
                            branch_cells[branch].push(encode(x, y));
                        }
                    }
                }
                branch_cells[branch].reverse();
                branch_cells[branch].push(encode(N - 1, N - 1)); // stem cell
                sx += rows[branch];
            } else {
                for dx in 0..rows[branch] {
                    if dx % 2 == 0 {
                        if dx > 0 {
                            h_walls[sx + dx - 1][N - 2] = false;
                        }
                        for y in (0..N - 1).rev() {
                            branch_cells[branch].push(encode(sx + dx, y));
                        }
                    } else {
                        if dx > 0 {
                            h_walls[sx + dx - 1][0] = false;
                        }
                        for y in 0..N - 1 {
                            branch_cells[branch].push(encode(sx + dx, y));
                        }
                    }
                }
                branch_cells[branch].reverse();
                branch_cells[branch].push(encode(sx, N - 1)); // stem cell
                sx += rows[branch];
            }
        }
        let stem_entry = encode(N / 2, N - 1);
        let mut stem_size = N;

        debug_assert!(v_walls[0][N - 2]);
        v_walls[0][N - 2] = false;
        for x in 1..=rows[0] {
            branch_cells[0].push(encode(x, N - 1));
        }
        h_walls[rows[0] - 1][N - 1] = true;
        stem_size -= rows[0];

        debug_assert!(v_walls[N - 1][N - 2]);
        v_walls[N - 1][N - 2] = false;
        for x in (N - rows[input.num_tubes - 1] - 1..N - 1).rev() {
            branch_cells[input.num_tubes - 1].push(encode(x, N - 1));
        }
        h_walls[N - rows[input.num_tubes - 1] - 1][N - 1] = true;
        stem_size -= rows[input.num_tubes - 1];

        let mut rng = Pcg64Mcg::seed_from_u64(0);
        let rands = (0..input.num_tubes).map(|_| rng.gen()).collect_vec();

        Self {
            num_tubes: input.num_tubes,
            // num_turns: input.num_turns,
            cost_per_gram: input.cost_per_gram,
            tubes: input.tubes.clone(),
            targets: input.targets.clone(),
            max_extra_turns,
            branch_cells,
            stem_entry,
            stem_size,
            v_walls,
            h_walls,
            add_unit: Self::decide_add_unit(input),
            rands,
        }
    }

    fn decide_add_unit(input: &Input) -> usize {
        if input.num_turns >= 10000 {
            1
        } else if input.num_turns >= 7500 {
            2
        } else if input.num_turns >= 5500 {
            3
        } else if input.num_turns >= 5000 {
            4
        } else {
            5
        }
    }

    fn assgin_rows(input: &Input) -> Vec<usize> {
        let mut priorities = vec![0; input.num_tubes];
        for tube in 0..input.num_tubes {
            let color = input.tubes[tube];
            let mut min_dist = Real::INFINITY;
            for otube in 0..input.num_tubes {
                if otube == tube {
                    continue;
                }
                let ocolor = input.tubes[otube];
                let dist = color_distance(color, ocolor);
                min_dist.chmin(dist);
            }
            priorities[tube] = (1e9 * min_dist) as i64;
        }
        let tube_order = (0..input.num_tubes)
            .sorted_by_key(|&tube| priorities[tube])
            .collect_vec();

        let mut assignment = vec![0; input.num_tubes];
        for i in 0..N {
            assignment[tube_order[i % input.num_tubes]] += 1;
        }
        assignment
    }

    fn branch_size(&self, branch: usize) -> usize {
        self.branch_cells[branch].len() - 1
    }

    fn entry(&self, branch: usize) -> usize {
        if branch < self.num_tubes {
            self.branch_cells[branch][0]
        } else {
            self.stem_entry
        }
    }

    fn make_add_action(&self, branch: usize, tube: usize) -> Action {
        Action::Add {
            v: self.entry(branch),
            tube,
        }
    }

    fn make_toggle_action(&self, branch: usize, s: usize) -> Action {
        debug_assert!(s > 0);
        Action::Toggle {
            u: self.branch_cells[branch][s - 1],
            v: self.branch_cells[branch][s],
        }
    }

    fn make_lid_toggle_action(&self, branch: usize) -> Action {
        self.make_toggle_action(branch, self.branch_size(branch))
    }

    fn print_walls(&self) {
        for x in 0..N {
            for y in 0..N - 1 {
                if self.v_walls[x][y] {
                    print!("1");
                } else {
                    print!("0");
                }
                if y < N - 2 {
                    print!(" ");
                } else {
                    println!();
                }
            }
        }
        for x in 0..N - 1 {
            for y in 0..N {
                if self.h_walls[x][y] {
                    print!("1");
                } else {
                    print!("0");
                }
                if y < N - 1 {
                    print!(" ");
                } else {
                    println!();
                }
            }
        }
    }
}

/*
CombAction patterns
- DirectDeliver (0 turn)
- Use (2 turn)
- Refill + Add + Use (4 turn)
- Open (1 turn)
- (Open +) Close + Add + Use (3 turn)
*/

#[derive(Clone, Copy, Debug)]
enum CombAction {
    Close { branch: usize },            // 1 turn
    Refill { branch: usize },           // 2 turn
    Add { branch: usize, tube: usize }, // 0 turn
    Use { branch: usize, s: usize },    // 2 turn
    Open { branch: usize },             // 1 turn
    DirectDeliver { branch: usize },    // 0 turn
}

fn sort_comb_actions(actions: &mut [CombAction]) {
    actions.sort_unstable_by_key(|action| comb_action_priotiry(*action));
}

fn comb_action_priotiry(action: CombAction) -> usize {
    match action {
        CombAction::Close { .. } => 0,
        CombAction::Refill { .. } => 1,
        CombAction::Add { .. } => 2,
        CombAction::Use { .. } => 3,
        CombAction::Open { .. } => 4,
        CombAction::DirectDeliver { .. } => 5,
    }
}

fn double_comb_beam(input: &Input) -> bool {
    let double_comb_env = DoubleCombEnvironment::new(&input);

    let mut state = DoubleCombState::new(&double_comb_env);
    let mut selector = DoubleCombSelector::new(&double_comb_env, &state);
    for _ in 0..input.targets.len() {
        state.expand(&double_comb_env, &mut selector);
        let nstate = selector.select(&double_comb_env);
        if nstate.is_none() {
            return false;
        }
        state = nstate.unwrap();
    }

    double_comb_env.print_walls();
    for action in state.history() {
        print_action(action);
    }
    true
}

#[derive(Clone, Debug)]
struct DoubleCombSelector {
    best_state: Option<DoubleCombState>,
    best_cost: Real,
    target: usize,
    first_extra_turns: usize,
    max_extra_turns_tmp: usize,
    branch_beam_width: usize,
    expand_start_time: f64,
    avg_time: f64,
}

impl DoubleCombSelector {
    fn new(env: &DoubleCombEnvironment, state: &DoubleCombState) -> Self {
        const DEFAULT_BRANCH_BEAM_WIDTH: usize = 20;
        const DEFAULT_AVG_TIME: f64 =
            TIME_LIMIT_SEC / (NUM_TARGETS * DEFAULT_BRANCH_BEAM_WIDTH) as f64;

        let target = 0;
        let first_extra_turns = state.extra_turns();
        let max_extra_turns_tmp = (target + 1) * (env.max_extra_turns - first_extra_turns)
            / NUM_TARGETS
            + first_extra_turns;
        Self {
            best_state: None,
            best_cost: Real::INFINITY,
            target,
            first_extra_turns,
            max_extra_turns_tmp,
            branch_beam_width: DEFAULT_BRANCH_BEAM_WIDTH,
            expand_start_time: get_time_sec(),
            avg_time: DEFAULT_AVG_TIME,
        }
    }

    fn add(&mut self, state: DoubleCombState, env: &DoubleCombEnvironment) {
        const TURN_PENALTY: Real = 1e9;
        const PRUNE_THRESHOLD: Real = 4e4;
        let mut cost = state.evaluate(env);
        if self.target >= 10
            && cost / (self.target + 1) as f64 * NUM_TARGETS as f64 * C_ERROR > PRUNE_THRESHOLD
        {
            return; // prune
        }
        let over_turns = state.extra_turns().saturating_sub(self.max_extra_turns_tmp);
        if over_turns >= 20 || (self.target >= 100 && over_turns > 0) {
            return; // prune
        }
        cost += TURN_PENALTY * over_turns as Real;
        if self.best_cost.chmin(cost) {
            self.best_state = Some(state);
        }
    }

    fn select(&mut self, env: &DoubleCombEnvironment) -> Option<DoubleCombState> {
        let mut ret = None;
        std::mem::swap(&mut self.best_state, &mut ret);
        self.update(env);
        ret
    }

    fn update(&mut self, env: &DoubleCombEnvironment) {
        self.best_cost = Real::INFINITY;
        self.target += 1;
        self.max_extra_turns_tmp =
            (self.target + 1) * (env.max_extra_turns - self.first_extra_turns) / NUM_TARGETS
                + self.first_extra_turns;

        const TARGET_MARGIN: usize = 10;
        const AVG_DECAY: f64 = 0.75;
        let expand_end_time = get_time_sec();
        let avg_time = (expand_end_time - self.expand_start_time) / self.branch_beam_width as f64;
        self.avg_time = AVG_DECAY * self.avg_time + (1.0 - AVG_DECAY) * avg_time;
        let remaining_time = BEAM_END_TIME - expand_end_time;
        let remaining_time_per_target =
            remaining_time / (NUM_TARGETS + TARGET_MARGIN - self.target) as f64;
        self.branch_beam_width = (remaining_time_per_target / self.avg_time).floor() as usize;
        self.branch_beam_width.chmax(1);
        self.expand_start_time = expand_end_time;
    }
}

#[derive(Clone, Debug)]
struct DoubleBranchState {
    front_refills: u32,
    back_refills: u32,
    front_wells: Vec<Well>,
    back_wells: Vec<Well>,
    stem: Well,
    front_pieces: Vec<usize>,
    back_pieces: Vec<usize>,
    res: Well, // result well
}

impl DoubleBranchState {
    fn mix(
        &mut self,
        target: Color,
        env: &DoubleCombEnvironment,
        selector: &mut DoubleCombSelector,
    ) {
        // beam search
        #[derive(Clone, Debug)]
        struct Pieces {
            well: Well,
            front: Vec<usize>,
            back: Vec<usize>,
            hash: u64,
        }
        let beam_width: usize = selector.branch_beam_width;
        let back_units = self
            .back_wells
            .iter()
            .map(|well| {
                if well.s == 0 {
                    Well::zero()
                } else {
                    well.divide(1)
                }
            })
            .collect_vec();
        let front_units = self
            .front_wells
            .iter()
            .map(|well| {
                if well.s == 0 {
                    Well::zero()
                } else {
                    well.divide(1)
                }
            })
            .collect_vec();

        let mut states = vec![Pieces {
            well: self.stem.clone(),
            front: vec![0; self.front_wells.len()],
            back: vec![0; self.back_wells.len()],
            hash: 0,
        }];
        let mut nstates = vec![];
        let mut hashes = NoHashSet::default();

        let mut min_error = Real::INFINITY;

        while !states.is_empty() {
            let mut candidates = vec![];
            hashes.clear();
            for state_id in 0..states.len() {
                let state = &states[state_id];
                let mut almost_full = state.front.iter().any(|&cnt| cnt > 0);
                if !almost_full {
                    for branch in 0..self.back_wells.len() {
                        if state.back[branch] >= self.back_wells[branch].s {
                            continue; // already full
                        }
                        let nhash = state.hash.wrapping_add(env.back_rands[branch]);
                        if !hashes.insert(nhash) {
                            continue; // already visited
                        }
                        let nwell = state.well.clone() + back_units[branch].clone();
                        let ncolor = nwell.color();
                        let nerror = color_distance(ncolor, target);
                        if nwell.v < 1.0 - EPS {
                            candidates.push((nerror, state_id as u32, branch as u32));
                            continue;
                        }
                        almost_full = true;
                        if min_error.chmin(nerror) {
                            self.res = nwell;
                            self.front_pieces = state.front.clone();
                            self.back_pieces = state.back.clone();
                            self.back_pieces[branch] += 1;
                        }
                    }
                }
                if almost_full {
                    let mut best_candidate = (Real::INFINITY, !0, !0);
                    for branch in 0..self.front_wells.len() {
                        if state.front[branch] >= self.front_wells[branch].s {
                            continue; // already full
                        }
                        let nhash = state.hash.wrapping_add(env.front_rands[branch]);
                        if !hashes.insert(nhash) {
                            continue; // already visited
                        }
                        let nwell = state.well.clone() + front_units[branch].clone();
                        let ncolor = nwell.color();
                        let nerror = color_distance(ncolor, target);
                        if nwell.v < 1.0 - EPS {
                            best_candidate.chmin((
                                nerror,
                                state_id as u32,
                                (self.back_wells.len() + branch) as u32,
                            ));
                            continue;
                        }
                        if min_error.chmin(nerror) {
                            self.res = nwell;
                            self.front_pieces = state.front.clone();
                            self.back_pieces = state.back.clone();
                            self.front_pieces[branch] += 1;
                        }
                    }
                    if best_candidate.1 != !0 {
                        candidates.push(best_candidate);
                    }
                }
            }
            candidates.sort_unstable_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
            if candidates.len() > beam_width {
                candidates.truncate(beam_width);
            }
            nstates.clear();
            for &(_, state_id, branch) in &candidates {
                let mut state = states[state_id as usize].clone();
                let branch = branch as usize;
                if branch < self.back_wells.len() {
                    state.back[branch] += 1;
                    state.well += back_units[branch].clone();
                    state.hash = state.hash.wrapping_add(env.back_rands[branch]);
                } else {
                    let branch = branch - self.back_wells.len();
                    state.front[branch] += 1;
                    state.well += front_units[branch].clone();
                    state.hash = state.hash.wrapping_add(env.front_rands[branch]);
                }
                nstates.push(state);
            }
            std::mem::swap(&mut states, &mut nstates);
        }
        debug_assert!(self.res.v >= 1.0 - EPS);
    }
}

#[derive(Clone, Debug)]
struct DoubleCombState {
    num_delivers: usize,
    num_adds: usize,
    num_toggles: usize,
    total_error: Real,
    front_wells: Vec<Well>, // for each branch
    back_wells: Vec<Well>,  // for each branch
    partitions: Vec<usize>, // for each branch
    stem: Well,
    history: Option<Rc<HistoryNode>>,
}

impl DoubleCombState {
    fn new(env: &DoubleCombEnvironment) -> Self {
        let mut front_wells = Vec::with_capacity(env.num_tubes);
        let mut back_wells = Vec::with_capacity(env.num_tubes);
        let mut partitions = env.initial_partitions.clone();
        let mut actions = Vec::with_capacity(env.num_tubes);
        for tube in 0..env.num_tubes {
            let mut front_well = Well {
                p: [0.0; 3],
                v: 0.0,
                s: env.initial_partitions[tube],
            };
            let mut back_well = Well {
                p: [0.0; 3],
                v: 0.0,
                s: env.branch_size(tube) - env.initial_partitions[tube],
            };

            actions.push(Action::Add {
                v: env.entry(tube),
                tube,
            });
            back_well.add_paint(env.tubes[tube]);

            actions.push(env.make_toggle_action_from_front(tube, front_well.s + 1));
            let dispatched_well = back_well.divide(1);
            back_well -= dispatched_well.clone();

            actions.push(env.make_toggle_action_from_front(tube, front_well.s));
            front_well += dispatched_well;
            partitions[tube] += 1;

            front_wells.push(front_well);
            back_wells.push(back_well);
        }

        let stem = Well {
            p: [0.0; 3],
            v: 0.0,
            s: env.stem_size,
        };

        Self {
            num_delivers: 0,
            num_adds: env.num_tubes,
            num_toggles: count_toggles(&actions),
            total_error: 0.0,
            front_wells,
            back_wells,
            partitions,
            stem,
            history: HistoryNode::new(None, actions),
        }
    }

    fn evaluate(&self, env: &DoubleCombEnvironment) -> Real {
        self.total_error + self.num_adds.saturating_sub(NUM_TARGETS) as Real * env.cost_per_gram
    }

    fn extra_turns(&self) -> usize {
        self.num_toggles + self.num_adds.saturating_sub(NUM_TARGETS)
    }

    fn build_branch_state(
        &self,
        front_refills: u32,
        back_refills: u32,
        env: &DoubleCombEnvironment,
        selector: &mut DoubleCombSelector,
    ) -> DoubleBranchState {
        let mut stem = self.stem.clone();
        let mut front_wells = Vec::with_capacity(self.front_wells.len());
        let mut back_wells = Vec::with_capacity(self.back_wells.len());
        for branch in 0..self.front_wells.len() {
            let mut back_well = self.back_wells[branch].clone();
            if back_refills & (1 << branch) != 0 {
                let detach_size =
                    env.branch_size(branch) - self.partitions[branch] - self.back_wells[branch].s;
                let detached_well = stem.divide(detach_size);
                stem -= detached_well.clone();
                back_well += detached_well;
                back_well.add_paint(env.tubes[branch]);
            }
            let mut front_well = self.front_wells[branch].clone();
            if front_refills & (1 << branch) != 0 {
                let detach_size = self.partitions[branch] - self.front_wells[branch].s;
                let detached_well = stem.divide(detach_size);
                stem -= detached_well.clone();
                front_well += detached_well;

                let detached_well = back_well.divide(1);
                back_well -= detached_well.clone();
                front_well += detached_well;
            }
            front_wells.push(front_well);
            back_wells.push(back_well);
        }
        let mut state = DoubleBranchState {
            front_refills,
            back_refills,
            front_wells,
            back_wells,
            stem,
            front_pieces: vec![0; self.front_wells.len()],
            back_pieces: vec![0; self.back_wells.len()],
            res: Well::zero(),
        };
        state.mix(env.targets[self.num_delivers], env, selector);
        state
    }

    fn apply(&mut self, state: &DoubleBranchState, env: &DoubleCombEnvironment) {
        let mut actions = vec![];

        // refill
        for branch in 0..env.num_tubes {
            if state.back_refills & (1 << branch) != 0 {
                let detach_size =
                    env.branch_size(branch) - self.partitions[branch] - self.back_wells[branch].s;
                actions.push(env.make_lid_toggle_action(branch));
                let detached_well = self.stem.divide(detach_size);
                self.stem -= detached_well.clone();

                if self.back_wells[branch].s > 0 {
                    actions.push(env.make_toggle_action_from_front(
                        branch,
                        self.partitions[branch] + self.back_wells[branch].s,
                    ));
                }
                self.back_wells[branch] += detached_well;

                actions.push(env.make_add_action(branch, branch));
                self.back_wells[branch].add_paint(env.tubes[branch]);
                self.num_adds += 1;
            }
            if state.front_refills & (1 << branch) != 0 {
                if self.front_wells[branch].s < self.partitions[branch] {
                    actions.push(env.make_toggle_action_from_front(branch, 0));
                }
                let detach_size = self.partitions[branch] - self.front_wells[branch].s;
                let detached_well = self.stem.divide(detach_size);
                self.stem -= detached_well.clone();

                if 0 < self.front_wells[branch].s
                    && self.front_wells[branch].s < self.partitions[branch]
                {
                    actions.push(env.make_toggle_action_from_front(
                        branch,
                        self.partitions[branch] - self.front_wells[branch].s,
                    ));
                }
                self.front_wells[branch] += detached_well;

                if self.back_wells[branch].s > 1 {
                    actions.push(
                        env.make_toggle_action_from_front(branch, self.front_wells[branch].s + 1),
                    );
                }
                let detached_well = self.back_wells[branch].divide(1);
                self.back_wells[branch] -= detached_well.clone();

                actions.push(env.make_toggle_action_from_front(branch, self.front_wells[branch].s));
                self.front_wells[branch] += detached_well;
                self.partitions[branch] += 1;
            }
        }
        // use
        for branch in 0..env.num_tubes {
            if state.back_pieces[branch] == 0 {
                continue; // no pieces to use
            }
            let s = state.back_pieces[branch];
            debug_assert!(s <= self.back_wells[branch].s);

            if s < self.back_wells[branch].s {
                actions.push(env.make_toggle_action_from_front(
                    branch,
                    self.partitions[branch] + self.back_wells[branch].s - s,
                ));
            }
            let detached_well = self.back_wells[branch].divide(s);
            actions.push(env.make_toggle_action_from_front(
                branch,
                self.partitions[branch] + self.back_wells[branch].s,
            ));
            self.back_wells[branch] -= detached_well.clone();
            self.stem += detached_well;
        }
        for branch in 0..env.num_tubes {
            if state.front_pieces[branch] == 0 {
                continue; // no pieces to use
            }
            let s = state.front_pieces[branch];
            debug_assert!(s <= self.front_wells[branch].s);
            if s < self.front_wells[branch].s {
                actions.push(env.make_toggle_action_from_front(
                    branch,
                    self.partitions[branch] - self.front_wells[branch].s + s,
                ));
            }
            let detached_well = self.front_wells[branch].divide(s);

            actions.push(env.make_toggle_action_from_front(
                branch,
                self.partitions[branch] - self.front_wells[branch].s,
            ));
            self.front_wells[branch] -= detached_well.clone();
            self.stem += detached_well;
        }
        // deliver
        debug_assert!(self.stem.v >= 1.0 - EPS);
        let c = self.stem.color();
        self.total_error += color_distance(c, env.targets[self.num_delivers]);
        actions.push(Action::Deliver { v: env.stem_entry });
        self.stem.discard_paint();
        self.num_delivers += 1;

        // reset partition
        for branch in 0..env.num_tubes {
            if self.partitions[branch] <= env.branch_size(branch) / 2 + 2 {
                continue;
            }
            let npartition = env.branch_size(branch) / 2 - 2;

            if self.front_wells[branch].s <= self.partitions[branch] - npartition {
                continue;
            }
            actions.push(env.make_toggle_action_from_front(branch, npartition));
            let detached_well =
                self.front_wells[branch].divide(self.partitions[branch] - npartition);
            self.front_wells[branch] -= detached_well.clone();

            actions.push(env.make_toggle_action_from_front(branch, self.partitions[branch]));
            if self.back_wells[branch].s == 0 {
                self.stem += detached_well;
            } else {
                self.back_wells[branch] += detached_well;
            }
            self.partitions[branch] = npartition;
        }
        self.num_toggles += count_toggles(&actions);
        self.history = HistoryNode::new(self.history.clone(), actions);
    }

    fn expand(&self, env: &DoubleCombEnvironment, selector: &mut DoubleCombSelector) {
        let mut front_refills = 0;
        let mut back_refills = 0;

        let mut total_volume = 0.0;
        for branch in 0..env.num_tubes {
            total_volume += self.front_wells[branch].v + self.back_wells[branch].v;
        }
        if total_volume < 1.0 - EPS {
            let mut nearest_tube = !0;
            let mut min_dist = Real::INFINITY;
            for tube in 0..env.num_tubes {
                let well = &self.front_wells[tube];
                let dist = color_distance(well.color(), env.targets[self.num_delivers]);
                if min_dist.chmin(dist) {
                    nearest_tube = tube;
                }
            }
            back_refills |= 1 << nearest_tube; // need refill
        }

        let mut updated = true;
        while updated {
            updated = false;
            let state = self.build_branch_state(front_refills, back_refills, env, selector);

            if state.res.v >= 1.0 - EPS {
                let mut nstate = self.clone();
                nstate.apply(&state, env);
                selector.add(nstate, env);
            }
            for branch in 0..env.num_tubes {
                if back_refills & (1 << branch) == 0 {
                    if state.back_pieces[branch] >= state.back_wells[branch].s {
                        back_refills |= 1 << branch; // need refill
                        updated = true;
                    }
                }
                if front_refills & (1 << branch) == 0 {
                    if state.front_pieces[branch] >= state.front_wells[branch].s {
                        front_refills |= 1 << branch; // need refill
                        updated = true;
                        if self.back_wells[branch].s == 0 {
                            // need refill for back well
                            back_refills |= 1 << branch;
                        }
                    }
                }
            }
        }
    }

    fn history(&self) -> Vec<Action> {
        self.history.clone().unwrap().history()
    }
}

struct DoubleCombEnvironment {
    num_tubes: usize,
    // num_turns: usize,
    cost_per_gram: Real,
    tubes: Vec<Color>,
    targets: Vec<Color>,
    max_extra_turns: usize,
    branch_cells: Vec<Vec<usize>>, // for each tube, first and last are stem cells
    stem_entry: usize,
    stem_size: usize,
    v_walls: [[bool; N - 1]; N],
    h_walls: [[bool; N]; N - 1],
    initial_partitions: Vec<usize>,
    front_rands: Vec<u64>,
    back_rands: Vec<u64>,
}

impl DoubleCombEnvironment {
    fn new(input: &Input) -> Self {
        let max_extra_turns = input.num_turns - 2 * NUM_TARGETS;

        let mut v_walls = [[true; N - 1]; N];
        let mut h_walls = [[true; N]; N - 1];

        let rows = Self::assgin_rows(input);
        let (branch_cells, stem) = Self::build_branch_cells(&rows);
        for branch in &branch_cells {
            for i in 1..branch.len() - 2 {
                let mut u = branch[i];
                let mut v = branch[i + 1];
                // remove wall between u and v
                if u > v {
                    std::mem::swap(&mut u, &mut v);
                }
                let (ux, uy) = decode(u);
                let (vx, vy) = decode(v);
                if ux == vx {
                    v_walls[ux][uy] = false; // vertical wall
                } else {
                    debug_assert_eq!(uy, vy);
                    h_walls[ux][uy] = false; // horizontal wall
                }
            }
        }
        for i in 0..stem.len() - 1 {
            let mut u = stem[i];
            let mut v = stem[i + 1];
            // remove wall between u and v
            if u > v {
                std::mem::swap(&mut u, &mut v);
            }
            let (ux, uy) = decode(u);
            let (vx, vy) = decode(v);
            if ux == vx {
                v_walls[ux][uy] = false; // vertical wall
            } else {
                debug_assert_eq!(uy, vy);
                h_walls[ux][uy] = false; // horizontal wall
            }
        }
        let stem_entry = stem[0];
        let stem_size = stem.len();

        let mut initial_partitions = Vec::with_capacity(input.num_tubes);
        for branch in &branch_cells {
            let partition = (branch.len() - 2) / 2 - 3;
            let mut u = branch[partition];
            let mut v = branch[partition + 1];
            // add wall between u and v
            if u > v {
                std::mem::swap(&mut u, &mut v);
            }
            let (ux, uy) = decode(u);
            let (vx, vy) = decode(v);
            if ux == vx {
                v_walls[ux][uy] = true; // vertical wall
            } else {
                debug_assert_eq!(uy, vy);
                h_walls[ux][uy] = true; // horizontal wall
            }
            initial_partitions.push(partition);
        }
        let mut rng = Pcg64Mcg::seed_from_u64(0);
        let front_rands = (0..input.num_tubes).map(|_| rng.gen()).collect_vec();
        let back_rands = (0..input.num_tubes).map(|_| rng.gen()).collect_vec();

        Self {
            num_tubes: input.num_tubes,
            // num_turns: input.num_turns,
            cost_per_gram: input.cost_per_gram,
            tubes: input.tubes.clone(),
            targets: input.targets.clone(),
            max_extra_turns,
            branch_cells,
            stem_entry,
            stem_size,
            v_walls,
            h_walls,
            initial_partitions,
            front_rands,
            back_rands,
        }
    }

    fn build_branch_cells(rows: &[usize]) -> (Vec<Vec<usize>>, Vec<usize>) {
        const STEM_Y: usize = N / 2 - 1;
        let mut sum_rows = 0;
        let mut branch_cells = Vec::with_capacity(rows.len());
        let mut use_bottom_center = false;
        for &row in rows {
            let mut row = row;
            let mut cells = vec![];
            if 2 * sum_rows < N {
                let x = 2 * sum_rows;
                for y in (0..=STEM_Y).rev() {
                    cells.push(encode(x, y));
                }
                // zig-zag
                let lx = 2 * sum_rows + 1;
                let rx = (2 * (sum_rows + row)).min(N);
                for y in 0..STEM_Y {
                    if y % 2 == 0 {
                        for x in lx..rx {
                            cells.push(encode(x, y));
                        }
                    } else {
                        for x in (lx..rx).rev() {
                            cells.push(encode(x, y));
                        }
                    }
                }
                if 2 * (sum_rows + row) <= N {
                    sum_rows += row;
                    cells.push(encode(rx - 1, STEM_Y));
                    branch_cells.push(cells);
                    continue;
                }
                row -= N / 2 - sum_rows;
                sum_rows = N / 2;
                use_bottom_center = true;
            }
            let x = N - 1 - (2 * sum_rows - N);
            for y in STEM_Y..N {
                cells.push(encode(x, y));
            }
            // zig-zag
            let rx = x;
            let lx = N - (2 * (sum_rows + row) - N);
            for y in (STEM_Y + 1..N).rev() {
                if y % 2 == 0 {
                    for x in lx..rx {
                        cells.push(encode(x, y));
                    }
                } else {
                    for x in (lx..rx).rev() {
                        cells.push(encode(x, y));
                    }
                }
            }
            cells.push(encode(rx - 1, STEM_Y));
            branch_cells.push(cells);
            sum_rows += row;
        }
        let mut stem = (0..N).map(|x| encode(x, STEM_Y)).collect_vec();
        if use_bottom_center {
            stem.pop();
        }
        (branch_cells, stem)
    }

    fn assgin_rows(input: &Input) -> Vec<usize> {
        let mut priorities = vec![0; input.num_tubes];
        for tube in 0..input.num_tubes {
            let color = input.tubes[tube];
            let mut min_dist = Real::INFINITY;
            for otube in 0..input.num_tubes {
                if otube == tube {
                    continue;
                }
                let ocolor = input.tubes[otube];
                let dist = color_distance(color, ocolor);
                min_dist.chmin(dist);
            }
            priorities[tube] = (1e9 * min_dist) as i64;
        }
        let tube_order = (0..input.num_tubes)
            .sorted_by_key(|&tube| priorities[tube])
            .collect_vec();

        let mut assignment = vec![0; input.num_tubes];
        for i in 0..N {
            assignment[tube_order[i % input.num_tubes]] += 1;
        }
        assignment
    }

    fn branch_size(&self, branch: usize) -> usize {
        self.branch_cells[branch].len() - 2 // exclude stem cells
    }

    // back entry
    fn entry(&self, branch: usize) -> usize {
        if branch < self.num_tubes {
            self.branch_cells[branch][self.branch_cells[branch].len() - 2]
        } else {
            self.stem_entry
        }
    }

    fn make_add_action(&self, branch: usize, tube: usize) -> Action {
        Action::Add {
            v: self.entry(branch),
            tube,
        }
    }

    fn make_toggle_action_from_front(&self, branch: usize, s: usize) -> Action {
        Action::Toggle {
            u: self.branch_cells[branch][s],
            v: self.branch_cells[branch][s + 1],
        }
    }

    fn make_toggle_action_from_back(&self, branch: usize, s: usize) -> Action {
        let branch_size = self.branch_cells[branch].len();
        Action::Toggle {
            u: self.branch_cells[branch][branch_size - s - 2],
            v: self.branch_cells[branch][branch_size - s - 1],
        }
    }

    fn make_lid_toggle_action(&self, branch: usize) -> Action {
        self.make_toggle_action_from_back(branch, 0)
    }

    fn print_walls(&self) {
        for x in 0..N {
            for y in 0..N - 1 {
                if self.v_walls[x][y] {
                    print!("1");
                } else {
                    print!("0");
                }
                if y < N - 2 {
                    print!(" ");
                } else {
                    println!();
                }
            }
        }
        for x in 0..N - 1 {
            for y in 0..N {
                if self.h_walls[x][y] {
                    print!("1");
                } else {
                    print!("0");
                }
                if y < N - 1 {
                    print!(" ");
                } else {
                    println!();
                }
            }
        }
    }
}

#[derive(Clone, Debug)]
struct HistoryNode {
    parent: Option<Rc<HistoryNode>>,
    actions: Vec<Action>,
}

impl HistoryNode {
    fn new(parent: Option<Rc<HistoryNode>>, actions: Vec<Action>) -> Option<Rc<Self>> {
        Some(Rc::new(Self { parent, actions }))
    }

    fn history(&self) -> Vec<Action> {
        let mut actions = vec![];
        let mut node: Option<&HistoryNode> = Some(self);
        while let Some(n) = node {
            actions.extend(n.actions.iter().rev());
            node = n.parent.as_deref();
        }
        actions.reverse();
        actions
    }
}

#[derive(Clone, Copy, Debug)]
enum Action {
    Add { v: usize, tube: usize },
    Deliver { v: usize },
    Toggle { u: usize, v: usize },
}

fn print_action(action: Action) {
    match action {
        Action::Add { v, tube } => {
            let (x, y) = decode(v);
            println!("1 {} {} {}", x, y, tube);
        }
        Action::Deliver { v } => {
            let (x, y) = decode(v);
            println!("2 {} {}", x, y);
        }
        Action::Toggle { u, v } => {
            let (ux, uy) = decode(u);
            let (vx, vy) = decode(v);
            println!("4 {} {} {} {}", ux, uy, vx, vy);
        }
    }
}

fn count_toggles(actions: &[Action]) -> usize {
    actions
        .iter()
        .filter(|&&action| matches!(action, Action::Toggle { .. }))
        .count()
}

#[derive(Clone, Debug)]
struct Input {
    num_tubes: usize,
    num_turns: usize,
    cost_per_gram: Real,
    tubes: Vec<Color>,
    targets: Vec<Color>,
}

fn read_input() -> Input {
    input! {
        n: usize,
        num_tubes: usize,
        num_targets: usize,
        num_turns: usize,
        cost_per_gram: Real,
        tubes: [[Real; 3]; num_tubes],
        targets: [[Real; 3]; NUM_TARGETS],
    }
    debug_assert_eq!(n, N);
    debug_assert_eq!(num_targets, NUM_TARGETS);
    Input {
        num_tubes,
        num_turns,
        cost_per_gram: cost_per_gram / C_ERROR, // minimize (num_adds * cost_per_gram + error)
        tubes: tubes.into_iter().map(|c| c.try_into().unwrap()).collect(),
        targets: targets.into_iter().map(|c| c.try_into().unwrap()).collect(),
    }
}

fn encode(x: usize, y: usize) -> usize {
    x * N + y
}

fn decode(v: usize) -> (usize, usize) {
    (v / N, v % N)
}

#[derive(Clone, Debug)]
struct Well {
    p: Color, // pigment
    v: Real,  // volume
    s: usize, // size
}

impl Well {
    fn zero() -> Self {
        Self {
            p: [0.0; 3],
            v: 0.0,
            s: 0,
        }
    }

    fn color(&self) -> Color {
        pigment_to_color(self.p, self.v)
    }

    // immutable
    fn divide(&self, t: usize) -> Self {
        debug_assert!(t <= self.s);
        let r = t as Real / self.s as Real;
        let p = [self.p[0] * r, self.p[1] * r, self.p[2] * r];
        let v = self.v * r;
        let s = t;
        Self { p, v, s }
    }

    // add 1 gram of paint
    fn add_paint(&mut self, c: Color) {
        let dv = self.s as Real - self.v;
        if dv >= 1.0 {
            self.p = [self.p[0] + c[0], self.p[1] + c[1], self.p[2] + c[2]];
            self.v += 1.0;
        } else {
            // overflow
            self.p = [
                self.p[0] + c[0] * dv,
                self.p[1] + c[1] * dv,
                self.p[2] + c[2] * dv,
            ];
            self.v = self.s as Real;
        }
    }

    // discard 1 gram of paint
    fn discard_paint(&mut self) {
        if self.v <= 1.0 {
            // discard all paint
            self.p = [0.0; 3];
            self.v = 0.0;
        } else {
            let r = (self.v - 1.0) / self.v;
            self.p = [self.p[0] * r, self.p[1] * r, self.p[2] * r];
            self.v -= 1.0;
        }
    }
}

impl std::ops::Add for Well {
    type Output = Self;

    fn add(self, other: Self) -> Self {
        let p = [
            self.p[0] + other.p[0],
            self.p[1] + other.p[1],
            self.p[2] + other.p[2],
        ];
        let v = self.v + other.v;
        let s = self.s + other.s;
        Self { p, v, s }
    }
}

impl std::ops::Sub for Well {
    type Output = Self;

    fn sub(self, other: Self) -> Self {
        debug_assert!(self.v >= other.v);
        debug_assert!(self.s >= other.s);
        let p = [
            self.p[0] - other.p[0],
            self.p[1] - other.p[1],
            self.p[2] - other.p[2],
        ];
        let v = self.v - other.v;
        let s = self.s - other.s;
        Self { p, v, s }
    }
}

impl std::ops::Mul<usize> for Well {
    type Output = Self;

    fn mul(self, t: usize) -> Self {
        let s = self.s * t;
        let t = t as Real;
        let p = [self.p[0] * t, self.p[1] * t, self.p[2] * t];
        let v = self.v * t;
        Self { p, v, s }
    }
}

impl std::ops::AddAssign for Well {
    fn add_assign(&mut self, other: Self) {
        self.p[0] += other.p[0];
        self.p[1] += other.p[1];
        self.p[2] += other.p[2];
        self.v += other.v;
        self.s += other.s;
    }
}

impl std::ops::SubAssign for Well {
    fn sub_assign(&mut self, other: Self) {
        debug_assert!(self.v >= other.v);
        debug_assert!(self.s >= other.s);
        self.p[0] -= other.p[0];
        self.p[1] -= other.p[1];
        self.p[2] -= other.p[2];
        self.v -= other.v;
        self.s -= other.s;
    }
}

impl std::ops::MulAssign<usize> for Well {
    fn mul_assign(&mut self, t: usize) {
        self.s *= t;
        let t = t as Real;
        self.p[0] *= t;
        self.p[1] *= t;
        self.p[2] *= t;
        self.v *= t;
    }
}

fn pigment_to_color(p: Color, v: Real) -> Color {
    if v < DIV_EPS {
        [0.0; 3]
    } else {
        [p[0] / v, p[1] / v, p[2] / v]
    }
}

fn color_distance(c: Color, d: Color) -> Real {
    let mut s = 0.0;
    for i in 0..3 {
        let diff = c[i] - d[i];
        s += diff * diff;
    }
    s.sqrt()
}

fn error_lower_bound2(c: Color, d: Color, target: Color) -> Real {
    let mut ret = 0.0;
    for i in 0..3 {
        let (low, high) = if c[i] < d[i] {
            (c[i], d[i])
        } else {
            (d[i], c[i])
        };
        if target[i] < low {
            ret += (low - target[i]) * (low - target[i]);
        } else if high < target[i] {
            ret += (target[i] - high) * (target[i] - high);
        }
    }
    ret.sqrt()
}

fn error_lower_bound3(c: Color, d: Color, e: Color, target: Color) -> Real {
    let mut ret = 0.0;
    for i in 0..3 {
        let low = c[i].min(d[i]).min(e[i]);
        let high = c[i].max(d[i]).max(e[i]);
        if target[i] < low {
            ret += (low - target[i]) * (low - target[i]);
        } else if high < target[i] {
            ret += (target[i] - high) * (target[i] - high);
        }
    }
    ret.sqrt()
}

#[derive(Default)]
pub struct NoHashHasher {
    value: u64,
}

impl Hasher for NoHashHasher {
    fn write(&mut self, bytes: &[u8]) {
        self.value = u64::from_ne_bytes(bytes[..8].try_into().unwrap());
    }

    fn finish(&self) -> u64 {
        self.value
    }
}

pub type NoHashSet = HashSet<u64, BuildHasherDefault<NoHashHasher>>;

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
