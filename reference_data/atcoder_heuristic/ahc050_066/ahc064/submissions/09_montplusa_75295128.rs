use std::collections::{HashMap, HashSet};
use std::io::{self, Read};

const CARS_PER_ROW: usize = 10;
const DEP_CAP: usize = 15;
const SIDING_CAP: usize = 20;
const GROUP_BEAM_WIDTH: usize = 600;
const GROUP_BEAM_MAX_DEPTH: usize = 90;
const ROUTE_CAND_LIMIT: usize = 8;
const GROUP_TURN_CAND_LIMIT: usize = 96;
const GROUP_FIXED_PROGRESS_WEIGHT: i64 = 1_000_000;
const PHASE1_BEAM_WIDTH: usize = 450;
const PHASE1_BEAM_MAX_DEPTH: usize = 80;
const PHASE1_TURN_CAND_LIMIT: usize = 18;

#[derive(Clone, Copy, PartialEq, Eq, Hash)]
struct Op {
    typ: usize,
    i: usize,
    j: usize,
    k: usize,
}

const EMPTY_OP: Op = Op {
    typ: 0,
    i: 0,
    j: 0,
    k: 0,
};

#[derive(Clone, Copy)]
struct BeamAction {
    ops: [Op; 2],
    len: usize,
}

impl BeamAction {
    fn empty() -> Self {
        Self {
            ops: [EMPTY_OP; 2],
            len: 0,
        }
    }

    fn single(op: Op) -> Self {
        Self {
            ops: [op, EMPTY_OP],
            len: 1,
        }
    }

    fn pair(a: Op, b: Op) -> Self {
        Self {
            ops: [a, b],
            len: 2,
        }
    }

    fn as_slice(&self) -> &[Op] {
        &self.ops[..self.len]
    }

    fn to_vec(&self) -> Vec<Op> {
        self.as_slice().to_vec()
    }
}

#[derive(Clone, Copy)]
struct GroupRun {
    i: usize,
    group: usize,
    k: usize,
}

#[derive(Clone, Copy, PartialEq, Eq, Hash)]
struct Phase1State {
    len: [u8; 10],
}

#[derive(Clone, Copy, PartialEq, Eq, Hash)]
struct Phase1EvacuatedKey {
    bits: [u64; 2],
}

#[derive(Clone)]
struct Phase1TurnCandidate {
    ops: Vec<Op>,
    evacuated_key: Phase1EvacuatedKey,
    score: (i64, i64, i64, i64),
}

struct Phase1Node {
    state: Phase1State,
    evacuated_key: Phase1EvacuatedKey,
    score: i64,
    parent: Option<usize>,
    turn: Vec<Op>,
}

struct Phase1DepMetrics {
    remaining_sum: i64,
    max_remaining: i64,
    nonempty_count: i64,
    balance_variance: i64,
    lower_bound: i64,
}

struct Solver {
    r: usize,
    dep: Vec<Vec<usize>>,
    siding: Vec<Vec<usize>>,
    turns: Vec<Vec<Op>>,
}

impl Solver {
    fn new(dep: Vec<Vec<usize>>) -> Self {
        let r = dep.len();
        Self {
            r,
            dep,
            siding: vec![Vec::new(); r],
            turns: Vec::new(),
        }
    }

    fn solve(&mut self) {
        let phase1_initial_dep = self.dep.clone();
        self.collect_by_goal_pair();
        let phase1_turns = self.turns.clone();
        self.sort_each_two_track_group(&phase1_initial_dep, &phase1_turns);
        self.assert_solved();
    }

    fn collect_by_goal_pair(&mut self) {
        if let Some(turns) = phase1_beam_solve(&self.dep, &self.siding) {
            for turn in turns {
                assert!(!turn.is_empty());
                assert!(is_valid_turn(&turn, self.r));
                for &op in &turn {
                    self.move_dep_to_siding(op.i, op.j, op.k);
                }
                self.turns.push(turn);
            }
            return;
        }

        while self.dep.iter().any(|line| !line.is_empty()) {
            let state = phase1_initial_state(&self.dep);
            let runs = phase1_collect_runs(&self.dep, &state);
            let turn = choose_best_group_turn(&runs, &self.dep);
            assert!(!turn.is_empty());
            assert!(is_valid_turn(&turn, self.r));
            for &op in &turn {
                self.move_dep_to_siding(op.i, op.j, op.k);
            }
            self.turns.push(turn);
        }
    }

    fn sort_each_two_track_group(
        &mut self,
        phase1_initial_dep: &[Vec<usize>],
        phase1_turns: &[Vec<Op>],
    ) {
        let mut group_turns = Vec::new();
        let mut group_phase1_turns = Vec::new();
        for base in (0..self.r).step_by(2) {
            let initial_candidates = GroupSorter::initial_candidates_from_phase1(
                base,
                phase1_initial_dep,
                phase1_turns,
                GROUP_BEAM_WIDTH,
            );
            let mut sorter = GroupSorter::new(base, initial_candidates);
            sorter.sort();
            group_phase1_turns.push(sorter.phase1_turns);
            group_turns.push(sorter.turns);
        }

        self.dep = phase1_initial_dep.to_vec();
        self.siding = vec![Vec::new(); self.r];
        self.turns.clear();
        for step in 0..phase1_turns.len() {
            let mut turn = Vec::new();
            for turns in &group_phase1_turns {
                if step < turns.len() {
                    turn.extend(turns[step].iter().copied());
                }
            }
            if !turn.is_empty() {
                assert!(is_valid_turn(&turn, self.r));
                for &op in &turn {
                    self.move_dep_to_siding(op.i, op.j, op.k);
                }
                self.turns.push(turn);
            }
        }

        let max_len = group_turns.iter().map(|turns| turns.len()).max().unwrap_or(0);
        for step in 0..max_len {
            let mut turn = Vec::new();
            for turns in &group_turns {
                if step < turns.len() {
                    turn.extend(turns[step].iter().copied());
                }
            }
            if !turn.is_empty() {
                assert!(is_valid_turn(&turn, self.r));
                for &op in &turn {
                    match op.typ {
                        0 => self.move_dep_to_siding(op.i, op.j, op.k),
                        1 => self.move_siding_to_dep(op.j, op.i, op.k),
                        _ => unreachable!(),
                    }
                }
                self.turns.push(turn);
            }
        }
    }

    fn move_dep_to_siding(&mut self, i: usize, j: usize, k: usize) {
        assert!(k > 0);
        assert!(self.dep[i].len() >= k);
        assert!(self.siding[j].len() + k <= SIDING_CAP);

        let start = self.dep[i].len() - k;
        let mut moved = self.dep[i].split_off(start);
        moved.extend(self.siding[j].drain(..));
        self.siding[j] = moved;
    }

    fn move_siding_to_dep(&mut self, j: usize, i: usize, k: usize) {
        assert!(k > 0);
        assert!(self.siding[j].len() >= k);
        assert!(self.dep[i].len() + k <= DEP_CAP);

        let dep = &mut self.dep;
        let siding = &mut self.siding;
        dep[i].extend(siding[j].drain(0..k));
    }

    fn assert_solved(&self) {
        for row in 0..self.r {
            assert_eq!(self.dep[row].len(), CARS_PER_ROW);
            for col in 0..CARS_PER_ROW {
                assert_eq!(self.dep[row][col], row * CARS_PER_ROW + col);
            }
        }
        assert!(self.siding.iter().all(|line| line.is_empty()));
    }
}

struct GroupSorter {
    base: usize,
    dep: Vec<Vec<usize>>,
    siding: Vec<Vec<usize>>,
    turns: Vec<Vec<Op>>,
    phase1_turns: Vec<Vec<Op>>,
    initial_candidates: Vec<GroupInitialCandidate>,
}

#[derive(Clone, PartialEq, Eq, Hash)]
struct GroupState {
    dep: [Vec<usize>; 2],
    siding: [Vec<usize>; 2],
}

#[derive(Clone)]
struct GroupInitialCandidate {
    siding: [Vec<usize>; 2],
    phase1_turns: Vec<Vec<Op>>,
}

#[derive(Clone)]
struct GroupPhase1Move {
    source: usize,
    actual_dst: usize,
    cars: Vec<usize>,
}

#[derive(Clone, Copy, PartialEq, Eq, Hash)]
struct GroupStateKey {
    data: [u8; 24],
}

impl GroupStateKey {
    fn from_state(state: &GroupState) -> Self {
        let mut data = [255_u8; 24];
        let mut pos = 0;
        for line in state.dep.iter().chain(state.siding.iter()) {
            data[pos] = line.len() as u8;
            pos += 1;
            for &car in line {
                data[pos] = car as u8;
                pos += 1;
            }
        }
        Self { data }
    }
}

impl GroupState {
    fn empty() -> Self {
        Self {
            dep: [Vec::new(), Vec::new()],
            siding: [Vec::new(), Vec::new()],
        }
    }
}

struct BeamNode {
    state: GroupState,
    key: GroupStateKey,
    score: i64,
    parent: Option<usize>,
    turn: BeamAction,
    initial_idx: usize,
}

struct BeamQueue {
    items: Vec<BeamNode>,
    width: usize,
    border: i64,
}

impl BeamQueue {
    fn new(width: usize) -> Self {
        Self {
            items: Vec::with_capacity(width * 2),
            width,
            border: i64::MIN,
        }
    }

    fn promising(&self, score: i64) -> bool {
        score >= self.border
    }

    fn push(&mut self, item: BeamNode) {
        if !self.promising(item.score) {
            return;
        }
        self.items.push(item);
        if self.items.len() >= self.width * 2 {
            self.prune();
        }
    }

    fn prune(&mut self) {
        self.items.sort_unstable_by(|a, b| b.score.cmp(&a.score));
        let mut seen = HashSet::with_capacity(self.items.len());
        let mut kept = Vec::with_capacity(self.width);
        for item in self.items.drain(..) {
            if seen.insert(item.key) {
                kept.push(item);
                if kept.len() == self.width {
                    break;
                }
            }
        }
        self.border = if kept.len() == self.width {
            kept.last().unwrap().score
        } else {
            i64::MIN
        };
        self.items = kept;
    }

    fn into_sorted(mut self) -> Vec<BeamNode> {
        self.prune();
        self.items
    }
}

impl GroupSorter {
    fn new(base: usize, mut initial_candidates: Vec<GroupInitialCandidate>) -> Self {
        if initial_candidates.is_empty() {
            initial_candidates.push(GroupInitialCandidate {
                siding: [Vec::new(), Vec::new()],
                phase1_turns: Vec::new(),
            });
        }
        let siding = initial_candidates[0].siding.clone();
        let phase1_turns = initial_candidates[0].phase1_turns.clone();
        Self {
            base,
            dep: vec![Vec::new(), Vec::new()],
            siding: vec![siding[0].clone(), siding[1].clone()],
            turns: Vec::new(),
            phase1_turns,
            initial_candidates,
        }
    }

    fn sort(&mut self) {
        if let Some((initial_idx, turns)) = self.beam_sort() {
            self.use_initial_candidate(initial_idx);
            for turn in turns {
                self.apply_turn(&turn);
            }
        } else {
            self.use_initial_candidate(0);
            self.greedy_sort();
        }
        self.assert_local_solved();
    }

    fn use_initial_candidate(&mut self, idx: usize) {
        let candidate = &self.initial_candidates[idx];
        self.dep = vec![Vec::new(), Vec::new()];
        self.siding = vec![candidate.siding[0].clone(), candidate.siding[1].clone()];
        self.phase1_turns = candidate.phase1_turns.clone();
        self.turns.clear();
    }

    fn initial_candidates_from_phase1(
        base: usize,
        phase1_initial_dep: &[Vec<usize>],
        phase1_turns: &[Vec<Op>],
        width: usize,
    ) -> Vec<GroupInitialCandidate> {
        let events = Self::phase1_events_for_group(base, phase1_initial_dep, phase1_turns);
        let fallback =
            Self::actual_initial_candidate_from_events(base, phase1_turns.len(), &events);
        let scorer = Self {
            base,
            dep: vec![Vec::new(), Vec::new()],
            siding: vec![Vec::new(), Vec::new()],
            turns: Vec::new(),
            phase1_turns: Vec::new(),
            initial_candidates: Vec::new(),
        };
        let mut candidates = vec![GroupInitialCandidate {
            siding: [Vec::new(), Vec::new()],
            phase1_turns: vec![Vec::new(); phase1_turns.len()],
        }];

        for (turn_idx, turn_events) in events.iter().enumerate() {
            if turn_events.is_empty() {
                continue;
            }
            let mut next_candidates = Vec::new();
            match turn_events.len() {
                1 => {
                    for candidate in &candidates {
                        for local_dst in 0..2 {
                            if let Some(next) = Self::apply_initial_event(
                                base,
                                candidate,
                                turn_idx,
                                &turn_events[0],
                                local_dst,
                            ) {
                                next_candidates.push(next);
                            }
                        }
                    }
                }
                2 => {
                    let mut ordered = [turn_events[0].clone(), turn_events[1].clone()];
                    if ordered[1].source < ordered[0].source {
                        ordered.swap(0, 1);
                    }
                    for candidate in &candidates {
                        if let Some(next) =
                            Self::apply_forced_initial_events(base, candidate, turn_idx, &ordered)
                        {
                            next_candidates.push(next);
                        }
                    }
                }
                _ => unreachable!(),
            }

            if next_candidates.is_empty() {
                return fallback.into_iter().collect();
            }
            candidates = next_candidates;
            scorer.prune_initial_candidates(&mut candidates, width);
        }

        scorer.prune_initial_candidates(&mut candidates, width);
        candidates
    }

    fn actual_initial_candidate_from_events(
        base: usize,
        turn_count: usize,
        events: &[Vec<GroupPhase1Move>],
    ) -> Option<GroupInitialCandidate> {
        let mut candidate = GroupInitialCandidate {
            siding: [Vec::new(), Vec::new()],
            phase1_turns: vec![Vec::new(); turn_count],
        };
        for (turn_idx, turn_events) in events.iter().enumerate() {
            let mut ordered = turn_events.clone();
            ordered.sort_by_key(|event| event.source);
            for event in &ordered {
                candidate =
                    Self::apply_initial_event(base, &candidate, turn_idx, event, event.actual_dst)?;
            }
        }
        Some(candidate)
    }

    fn phase1_events_for_group(
        base: usize,
        phase1_initial_dep: &[Vec<usize>],
        phase1_turns: &[Vec<Op>],
    ) -> Vec<Vec<GroupPhase1Move>> {
        let mut dep = phase1_initial_dep.to_vec();
        let mut events = vec![Vec::new(); phase1_turns.len()];
        for (turn_idx, turn) in phase1_turns.iter().enumerate() {
            for &op in turn {
                if op.typ != 0 || op.i >= dep.len() || op.k == 0 || dep[op.i].len() < op.k {
                    continue;
                }
                let start = dep[op.i].len() - op.k;
                let cars = dep[op.i][start..].to_vec();
                if cars
                    .first()
                    .map_or(false, |&car| ((car / CARS_PER_ROW) / 2) * 2 == base)
                    && base <= op.j
                    && op.j < base + 2
                {
                    events[turn_idx].push(GroupPhase1Move {
                        source: op.i,
                        actual_dst: op.j - base,
                        cars,
                    });
                }
            }
            for &op in turn {
                if op.typ == 0 && op.i < dep.len() && dep[op.i].len() >= op.k {
                    let new_len = dep[op.i].len() - op.k;
                    dep[op.i].truncate(new_len);
                }
            }
        }
        events
    }

    fn apply_initial_event(
        base: usize,
        candidate: &GroupInitialCandidate,
        turn_idx: usize,
        event: &GroupPhase1Move,
        local_dst: usize,
    ) -> Option<GroupInitialCandidate> {
        if candidate.siding[local_dst].len() + event.cars.len() > SIDING_CAP {
            return None;
        }
        let mut next = candidate.clone();
        let mut new_siding = event.cars.clone();
        new_siding.extend(next.siding[local_dst].drain(..));
        next.siding[local_dst] = new_siding;
        next.phase1_turns[turn_idx].push(Op {
            typ: 0,
            i: event.source,
            j: base + local_dst,
            k: event.cars.len(),
        });
        Some(next)
    }

    fn apply_forced_initial_events(
        base: usize,
        candidate: &GroupInitialCandidate,
        turn_idx: usize,
        ordered: &[GroupPhase1Move; 2],
    ) -> Option<GroupInitialCandidate> {
        let mut next = candidate.clone();
        for (local_dst, event) in ordered.iter().enumerate() {
            if next.siding[local_dst].len() + event.cars.len() > SIDING_CAP {
                return None;
            }
            let mut new_siding = event.cars.clone();
            new_siding.extend(next.siding[local_dst].drain(..));
            next.siding[local_dst] = new_siding;
            next.phase1_turns[turn_idx].push(Op {
                typ: 0,
                i: event.source,
                j: base + local_dst,
                k: event.cars.len(),
            });
        }
        Some(next)
    }

    fn prune_initial_candidates(
        &self,
        candidates: &mut Vec<GroupInitialCandidate>,
        width: usize,
    ) {
        if candidates.len() <= 1 || width == 0 {
            return;
        }
        let mut scored = Vec::with_capacity(candidates.len());
        for candidate in candidates.drain(..) {
            scored.push((
                self.initial_candidate_score(&candidate),
                self.initial_candidate_key(&candidate),
                candidate,
            ));
        }
        scored.sort_unstable_by(|a, b| b.0.cmp(&a.0));
        let mut seen = HashSet::new();
        let mut kept = Vec::with_capacity(width.min(scored.len()));
        for (_, key, candidate) in scored {
            if seen.insert(key) {
                kept.push(candidate);
                if kept.len() == width {
                    break;
                }
            }
        }
        *candidates = kept;
    }

    fn initial_candidate_score(&self, candidate: &GroupInitialCandidate) -> i64 {
        let state = GroupState {
            dep: [Vec::new(), Vec::new()],
            siding: [candidate.siding[0].clone(), candidate.siding[1].clone()],
        };
        self.evaluate_state(&state, 0)
    }

    fn initial_candidate_key(&self, candidate: &GroupInitialCandidate) -> GroupStateKey {
        let mut data = [255_u8; 24];
        let mut pos = 0;
        data[pos] = 0;
        pos += 1;
        data[pos] = 0;
        pos += 1;
        for line in &candidate.siding {
            data[pos] = line.len() as u8;
            pos += 1;
            for &car in line {
                data[pos] = car as u8;
                pos += 1;
            }
        }
        GroupStateKey { data }
    }

    fn greedy_sort(&mut self) {
        while !self.is_solved() {
            if self.place_direct_pair_or_single() {
                continue;
            }
            if self.clear_best_buffer() {
                continue;
            }
            self.expose_best_target();
        }
    }

    fn assert_local_solved(&self) {
        for row in 0..2 {
            assert_eq!(self.dep[row].len(), CARS_PER_ROW);
            for col in 0..CARS_PER_ROW {
                assert_eq!(self.dep[row][col], (self.base + row) * CARS_PER_ROW + col);
            }
        }
        assert!(self.siding.iter().all(|line| line.is_empty()));
    }

    fn beam_sort(&self) -> Option<(usize, Vec<Vec<Op>>)> {
        let mut initial_layer = Vec::with_capacity(self.initial_candidates.len());
        for (initial_idx, candidate) in self.initial_candidates.iter().enumerate() {
            let state = GroupState {
                dep: [Vec::new(), Vec::new()],
                siding: [candidate.siding[0].clone(), candidate.siding[1].clone()],
            };
            if self.state_is_solved(&state) {
                return Some((initial_idx, Vec::new()));
            }
            let key = GroupStateKey::from_state(&state);
            initial_layer.push(BeamNode {
                score: self.evaluate_state(&state, 0),
                key,
                state,
                parent: None,
                turn: BeamAction::empty(),
                initial_idx,
            });
        }
        initial_layer.sort_unstable_by(|a, b| b.score.cmp(&a.score));
        if initial_layer.len() > GROUP_BEAM_WIDTH {
            initial_layer.truncate(GROUP_BEAM_WIDTH);
        }
        if initial_layer.is_empty() {
            return None;
        }

        let mut layers = vec![initial_layer];
        let mut candidate_routes: [Vec<Op>; 4] =
            std::array::from_fn(|_| Vec::with_capacity(16));
        let mut candidate_turns = Vec::with_capacity(96);
        let mut scored_ops = Vec::with_capacity(32);
        let mut scored_turns = Vec::with_capacity(96);

        for depth in 0..GROUP_BEAM_MAX_DEPTH {
            let current_layer = layers.len() - 1;
            let mut queue = BeamQueue::new(GROUP_BEAM_WIDTH);
            let mut next_state = GroupState::empty();

            for parent_idx in 0..layers[current_layer].len() {
                let state = &layers[current_layer][parent_idx].state;
                self.generate_candidate_turns(
                    state,
                    &mut candidate_routes,
                    &mut candidate_turns,
                    &mut scored_ops,
                    &mut scored_turns,
                );
                for &turn in &candidate_turns {
                    if !self.apply_turn_into_state(state, turn.as_slice(), &mut next_state) {
                        continue;
                    }
                    let score = self.evaluate_state(&next_state, depth + 1);
                    if queue.promising(score) {
                        let key = GroupStateKey::from_state(&next_state);
                        queue.push(BeamNode {
                            state: next_state.clone(),
                            key,
                            score,
                            parent: Some(parent_idx),
                            turn,
                            initial_idx: layers[current_layer][parent_idx].initial_idx,
                        });
                    }
                }
            }

            let next_nodes = queue.into_sorted();
            if next_nodes.is_empty() {
                return None;
            }

            layers.push(next_nodes);
            let solved_layer = layers.len() - 1;
            if let Some(pos) = layers[solved_layer]
                .iter()
                .position(|node| self.state_is_solved(&node.state))
            {
                return Some(Self::reconstruct_path(&layers, solved_layer, pos));
            }
        }

        None
    }

    fn reconstruct_path(
        layers: &[Vec<BeamNode>],
        mut layer_idx: usize,
        mut node_idx: usize,
    ) -> (usize, Vec<Vec<Op>>) {
        let mut path = Vec::new();
        while layer_idx > 0 {
            let node = &layers[layer_idx][node_idx];
            path.push(node.turn.to_vec());
            node_idx = node.parent.unwrap();
            layer_idx -= 1;
        }
        path.reverse();
        (layers[0][node_idx].initial_idx, path)
    }

    fn generate_candidate_turns(
        &self,
        state: &GroupState,
        routes: &mut [Vec<Op>; 4],
        turns: &mut Vec<BeamAction>,
        scored_ops: &mut Vec<(i64, Op)>,
        scored_turns: &mut Vec<(i64, BeamAction)>,
    ) {
        let fixed = [
            self.state_fixed_prefix_len(state, 0),
            self.state_fixed_prefix_len(state, 1),
        ];
        for route in routes.iter_mut() {
            route.clear();
        }
        self.collect_type1_candidates(state, &fixed, routes);
        self.collect_type0_candidates(state, &fixed, routes);

        for route in routes.iter_mut() {
            scored_ops.clear();
            scored_ops.extend(
                route
                    .iter()
                    .map(|&op| (self.rough_op_priority(state, &fixed, &op), op)),
            );
            scored_ops.sort_unstable_by(|a, b| b.0.cmp(&a.0));
            route.clear();
            for &(_, op) in scored_ops.iter().take(ROUTE_CAND_LIMIT) {
                Self::push_unique_route_op(route, op);
            }
            for &(_, op) in scored_ops.iter() {
                if self.op_is_reserved_candidate(state, &fixed, &op) {
                    Self::push_unique_route_op(route, op);
                }
            }
        }

        turns.clear();
        for route in routes.iter() {
            for &op in route {
                turns.push(BeamAction::single(op));
            }
        }
        for &op0 in routes[0].iter() {
            for &op1 in routes[3].iter() {
                let turn = BeamAction::pair(op0, op1);
                if is_valid_turn(turn.as_slice(), 10) {
                    turns.push(turn);
                }
            }
        }

        scored_turns.clear();
        scored_turns.extend(
            turns
                .iter()
                .map(|&turn| (self.rough_turn_priority(state, &fixed, &turn), turn)),
        );
        scored_turns.sort_unstable_by(|a, b| b.0.cmp(&a.0));
        turns.clear();
        turns.extend(
            scored_turns
                .iter()
                .take(GROUP_TURN_CAND_LIMIT)
                .map(|&(_, turn)| turn),
        );
    }

    fn collect_type1_candidates(
        &self,
        state: &GroupState,
        fixed: &[usize; 2],
        routes: &mut [Vec<Op>; 4],
    ) {
        for dep_row in 0..2 {
            let free = DEP_CAP - state.dep[dep_row].len();
            if free == 0 {
                continue;
            }
            for siding_row in 0..2 {
                let avail = state.siding[siding_row].len();
                if avail == 0 {
                    continue;
                }
                let max_k = free.min(avail);
                let mut lens = Vec::new();
                Self::push_len(&mut lens, 1, max_k);
                Self::push_len(&mut lens, 2, max_k);
                Self::push_len(&mut lens, 3, max_k);
                Self::push_len(&mut lens, max_k, max_k);

                let dep_fixed = fixed[dep_row];
                if dep_fixed < CARS_PER_ROW && state.dep[dep_row].len() == dep_fixed {
                    let target = (self.base + dep_row) * CARS_PER_ROW + dep_fixed;
                    if state.siding[siding_row].first().copied() == Some(target) {
                        Self::push_len(
                            &mut lens,
                            self.state_direct_run_len(state, dep_row, siding_row, dep_fixed),
                            max_k,
                        );
                    }
                }

                let mut same_goal_run = 0;
                while same_goal_run < max_k
                    && state.siding[siding_row][same_goal_run] / CARS_PER_ROW
                        == self.base + dep_row
                {
                    same_goal_run += 1;
                }
                Self::push_len(&mut lens, same_goal_run, max_k);

                for pos in 0..state.siding[siding_row].len().saturating_sub(1) {
                    if self.is_adjacent_progress_pair(
                        state.siding[siding_row][pos],
                        state.siding[siding_row][pos + 1],
                    ) {
                        Self::push_len(&mut lens, pos, max_k);
                        Self::push_len(&mut lens, pos + 2, max_k);
                    }
                }

                for target_row in 0..2 {
                    let target_fixed = fixed[target_row];
                    if target_fixed >= CARS_PER_ROW
                        || state.dep[target_row].len() != target_fixed
                        || dep_row != 1 - target_row
                    {
                        continue;
                    }
                    let target = (self.base + target_row) * CARS_PER_ROW + target_fixed;
                    if let Some((located_siding, pos)) = self.state_locate_in_sidings(state, target)
                    {
                        if located_siding == siding_row && pos > 0 {
                            Self::push_len(&mut lens, pos.min(max_k), max_k);
                        }
                    }
                }

                lens.sort_unstable();
                lens.dedup();
                for k in lens {
                    self.push_candidate_op(
                        state,
                        routes,
                        Op {
                            typ: 1,
                            i: self.base + dep_row,
                            j: self.base + siding_row,
                            k,
                        },
                    );
                }
            }
        }
    }

    fn collect_type0_candidates(
        &self,
        state: &GroupState,
        fixed: &[usize; 2],
        routes: &mut [Vec<Op>; 4],
    ) {
        for dep_row in 0..2 {
            let dep_fixed = fixed[dep_row];
            if state.dep[dep_row].len() <= dep_fixed {
                continue;
            }
            let dirty_len = state.dep[dep_row].len() - dep_fixed;

            for siding_row in 0..2 {
                let free = SIDING_CAP - state.siding[siding_row].len();
                if free == 0 {
                    continue;
                }
                let max_k = dirty_len.min(free);
                let mut lens = Vec::new();
                Self::push_len(&mut lens, 1, max_k);
                Self::push_len(&mut lens, 2, max_k);
                Self::push_len(&mut lens, 3, max_k);
                Self::push_len(&mut lens, max_k, max_k);

                for pos in dep_fixed..state.dep[dep_row].len() {
                    let car = state.dep[dep_row][pos];
                    let k = state.dep[dep_row].len() - pos;
                    if car / CARS_PER_ROW == self.base + siding_row {
                        Self::push_len(&mut lens, k, max_k);
                    }
                    for target_row in 0..2 {
                        let target_fixed = fixed[target_row];
                        if target_fixed >= CARS_PER_ROW {
                            continue;
                        }
                        let target = (self.base + target_row) * CARS_PER_ROW + target_fixed;
                        if car == target {
                            Self::push_len(&mut lens, k, max_k);
                        }
                    }
                }

                for pos in dep_fixed..state.dep[dep_row].len().saturating_sub(1) {
                    if self.is_adjacent_progress_pair(
                        state.dep[dep_row][pos],
                        state.dep[dep_row][pos + 1],
                    ) {
                        Self::push_len(&mut lens, state.dep[dep_row].len() - pos, max_k);
                    }
                }

                lens.sort_unstable();
                lens.dedup();
                for k in lens {
                    self.push_candidate_op(
                        state,
                        routes,
                        Op {
                            typ: 0,
                            i: self.base + dep_row,
                            j: self.base + siding_row,
                            k,
                        },
                    );
                }
            }
        }
    }

    fn push_len(lens: &mut Vec<usize>, k: usize, max_k: usize) {
        if k > 0 && k <= max_k {
            lens.push(k);
        }
    }

    fn push_unique_route_op(route: &mut Vec<Op>, op: Op) {
        if !route.contains(&op) {
            route.push(op);
        }
    }

    fn push_candidate_op(
        &self,
        state: &GroupState,
        routes: &mut [Vec<Op>; 4],
        op: Op,
    ) {
        let route_idx = (op.i - self.base) * 2 + (op.j - self.base);
        if !routes[route_idx].contains(&op) && self.op_is_applicable(state, op) {
            routes[route_idx].push(op);
        }
    }

    fn op_is_applicable(&self, state: &GroupState, op: Op) -> bool {
        if op.i < self.base || op.i >= self.base + 2 || op.j < self.base || op.j >= self.base + 2 {
            return false;
        }
        let i = op.i - self.base;
        let j = op.j - self.base;
        match op.typ {
            0 => state.dep[i].len() >= op.k && state.siding[j].len() + op.k <= SIDING_CAP,
            1 => state.siding[j].len() >= op.k && state.dep[i].len() + op.k <= DEP_CAP,
            _ => false,
        }
    }

    fn op_is_reserved_candidate(&self, state: &GroupState, fixed: &[usize; 2], op: &Op) -> bool {
        let i = op.i - self.base;
        let j = op.j - self.base;
        match op.typ {
            0 => {
                let dirty_len = state.dep[i].len() - fixed[i];
                let max_k = dirty_len.min(SIDING_CAP - state.siding[j].len());
                let start = state.dep[i].len() - op.k;
                let moved = &state.dep[i][start..];
                let exposes_target = (0..2).any(|row| {
                    fixed[row] < CARS_PER_ROW
                        && moved.first().copied()
                            == Some((self.base + row) * CARS_PER_ROW + fixed[row])
                });
                op.k == max_k
                    || op.k == dirty_len
                    || self.adjacent_progress_in_line(moved) > 0
                    || exposes_target
            }
            1 => {
                let max_k =
                    (DEP_CAP - state.dep[i].len()).min(state.siding[j].len());
                let moved = &state.siding[j][..op.k];
                let clean = state.dep[i].len() == fixed[i];
                let own_target = (self.base + i) * CARS_PER_ROW + fixed[i];
                let direct = clean
                    && fixed[i] < CARS_PER_ROW
                    && state.siding[j].first().copied() == Some(own_target);
                let exposes_target = (0..2).any(|row| {
                    fixed[row] < CARS_PER_ROW
                        && state.dep[row].len() == fixed[row]
                        && state.siding[j].get(op.k).copied()
                            == Some((self.base + row) * CARS_PER_ROW + fixed[row])
                });
                op.k == max_k
                    || direct
                    || exposes_target
                    || self.adjacent_progress_in_line(moved) > 0
            }
            _ => false,
        }
    }

    fn rough_op_priority(&self, state: &GroupState, fixed: &[usize; 2], op: &Op) -> i64 {
        let i = op.i - self.base;
        let j = op.j - self.base;
        let mut score = op.k as i64 * 1_000;

        match op.typ {
            0 => {
                let dep_fixed = fixed[i];
                let start = state.dep[i].len() - op.k;
                let moved = &state.dep[i][start..];
                let same_goal = moved
                    .iter()
                    .filter(|&&car| car / CARS_PER_ROW == self.base + j)
                    .count();
                score += same_goal as i64 * 5_000;
                score +=
                    self.adjacent_progress_in_line(moved) as i64 * GROUP_FIXED_PROGRESS_WEIGHT;
                score -= self.state_next_target_bury_penalty(state, fixed, j) * 2_000;
                if start == dep_fixed {
                    score += 6_000;
                }
            }
            1 => {
                let dep_fixed = fixed[i];
                let clean = state.dep[i].len() == dep_fixed;
                if clean && dep_fixed < CARS_PER_ROW {
                    let target = (self.base + i) * CARS_PER_ROW + dep_fixed;
                    if state.siding[j].first().copied() == Some(target) {
                        let run = self.state_direct_run_len(state, i, j, dep_fixed).min(op.k);
                        score += run as i64 * 80_000;
                    }
                } else {
                    score -= 8_000;
                }

                let moved = &state.siding[j][..op.k];
                let same_goal = moved
                    .iter()
                    .filter(|&&car| car / CARS_PER_ROW == self.base + i)
                    .count();
                score += same_goal as i64 * 4_000;
                score +=
                    self.adjacent_progress_in_line(moved) as i64 * GROUP_FIXED_PROGRESS_WEIGHT;

                for target_row in 0..2 {
                    if target_row == i {
                        continue;
                    }
                    let target_fixed = fixed[target_row];
                    if target_fixed >= CARS_PER_ROW || state.dep[target_row].len() != target_fixed {
                        continue;
                    }
                    let target = (self.base + target_row) * CARS_PER_ROW + target_fixed;
                    if state.siding[j].get(op.k).copied() == Some(target) {
                        score += 50_000;
                    }
                }
            }
            _ => return i64::MIN,
        }

        score
    }

    fn rough_turn_priority(&self, state: &GroupState, fixed: &[usize; 2], turn: &BeamAction) -> i64 {
        let moved: usize = turn.as_slice().iter().map(|op| op.k).sum();
        turn.as_slice()
            .iter()
            .map(|op| self.rough_op_priority(state, fixed, op))
            .sum::<i64>()
            + moved as i64 * 500
            + turn.len as i64 * 5_000
    }

    fn evaluate_state(&self, state: &GroupState, depth: usize) -> i64 {
        if self.state_is_solved(state) {
            return 1_000_000_000 - depth as i64;
        }

        let fixed = [
            self.state_fixed_prefix_len(state, 0),
            self.state_fixed_prefix_len(state, 1),
        ];
        let fixed_sum = fixed[0] + fixed[1];
        let adjacent_progress = self.state_adjacent_progress(state, &fixed);
        let next_turn_progress = self.state_next_turn_progress(state, &fixed);
        let mut score = fixed_sum as i64 * GROUP_FIXED_PROGRESS_WEIGHT
            + adjacent_progress as i64 * GROUP_FIXED_PROGRESS_WEIGHT
            + next_turn_progress * (GROUP_FIXED_PROGRESS_WEIGHT / 2)
            + fixed[0].min(fixed[1]) as i64 * 80_000
            - depth as i64 * 100;

        for row in 0..2 {
            let clean = state.dep[row].len() == fixed[row];
            if clean {
                score += 25_000;
            } else {
                let dirty_len = state.dep[row].len() - fixed[row];
                score -= 25_000 + dirty_len as i64 * 8_000;
            }

            if fixed[row] >= CARS_PER_ROW {
                continue;
            }

            let target = (self.base + row) * CARS_PER_ROW + fixed[row];
            if clean {
                if let Some((siding_row, pos)) = self.state_locate_in_sidings(state, target) {
                    score -= pos as i64 * 18_000;
                    if pos == 0 {
                        let run = self.state_direct_run_len(state, row, siding_row, fixed[row]);
                        score += run as i64 * 100_000;
                        if siding_row == row {
                            score += 10_000;
                        }
                    }
                } else {
                    score -= 180_000;
                    for dep_row in 0..2 {
                        if let Some(pos) = state.dep[dep_row].iter().position(|&car| car == target)
                        {
                            score -= pos as i64 * 4_000;
                            if dep_row == row {
                                score += 8_000;
                            }
                        }
                    }
                }
            } else if let Some(pos) = state.dep[row].iter().position(|&car| car == target) {
                score -= (pos - fixed[row]) as i64 * 7_000;
            }
        }

        for siding_row in 0..2 {
            if let Some(&front) = state.siding[siding_row].first() {
                let goal_row = front / CARS_PER_ROW;
                if self.base <= goal_row && goal_row < self.base + 2 {
                    let local_goal = goal_row - self.base;
                    if fixed[local_goal] < CARS_PER_ROW
                        && state.dep[local_goal].len() == fixed[local_goal]
                        && front == (self.base + local_goal) * CARS_PER_ROW + fixed[local_goal]
                    {
                        score += 60_000;
                    } else if local_goal == siding_row {
                        score += 5_000;
                    } else {
                        score -= 2_000;
                    }
                }
            }
        }

        score
    }

    fn state_next_target_bury_penalty(
        &self,
        state: &GroupState,
        fixed: &[usize; 2],
        siding_idx: usize,
    ) -> i64 {
        let mut penalty = 0;
        for row in 0..2 {
            let row_fixed = fixed[row];
            if row_fixed >= CARS_PER_ROW {
                continue;
            }
            let target = (self.base + row) * CARS_PER_ROW + row_fixed;
            if let Some(pos) = state.siding[siding_idx]
                .iter()
                .position(|&car| car == target)
            {
                penalty += 30 - pos.min(20) as i64;
            }
        }
        penalty
    }

    fn state_adjacent_progress(&self, state: &GroupState, fixed: &[usize; 2]) -> usize {
        let mut progress = 0;
        for row in 0..2 {
            progress += self.adjacent_progress_in_line(&state.dep[row][fixed[row]..]);
            progress += self.adjacent_progress_in_line(&state.siding[row]);
        }
        progress
    }

    fn state_next_turn_progress(&self, state: &GroupState, fixed: &[usize; 2]) -> i64 {
        let mut candidates = [EMPTY_OP; 12];
        let mut candidate_count = 0;
        for dep_row in 0..2 {
            let free = DEP_CAP - state.dep[dep_row].len();
            if free == 0 {
                continue;
            }
            for siding_row in 0..2 {
                let Some(&front) = state.siding[siding_row].first() else {
                    continue;
                };

                let op = self.absolute_op(1, dep_row, siding_row, 1);
                if state.dep[dep_row].len() == fixed[dep_row] && fixed[dep_row] < CARS_PER_ROW {
                    let target = (self.base + dep_row) * CARS_PER_ROW + fixed[dep_row];
                    if front == target {
                        Self::push_next_progress_candidate(
                            &mut candidates,
                            &mut candidate_count,
                            op,
                        );
                    }
                }

                if let Some(&tail) = state.dep[dep_row].last() {
                    if self.is_adjacent_progress_pair(tail, front) {
                        Self::push_next_progress_candidate(
                            &mut candidates,
                            &mut candidate_count,
                            op,
                        );
                    }
                }
            }
        }

        for dep_row in 0..2 {
            if state.dep[dep_row].len() <= fixed[dep_row] {
                continue;
            }
            for siding_row in 0..2 {
                let free = SIDING_CAP - state.siding[siding_row].len();
                if free == 0 {
                    continue;
                }
                let Some(&front) = state.siding[siding_row].first() else {
                    continue;
                };
                let tail = state.dep[dep_row][state.dep[dep_row].len() - 1];
                if self.is_adjacent_progress_pair(tail, front) {
                    Self::push_next_progress_candidate(
                        &mut candidates,
                        &mut candidate_count,
                        self.absolute_op(0, dep_row, siding_row, 1),
                    );
                }
            }
        }

        if candidate_count == 0 {
            return 0;
        }
        for a in 0..candidate_count {
            for b in a + 1..candidate_count {
                if Self::next_progress_ops_compatible(candidates[a], candidates[b]) {
                    return 2;
                }
            }
        }
        1
    }

    fn push_next_progress_candidate(candidates: &mut [Op; 12], len: &mut usize, op: Op) {
        if !candidates[..*len].contains(&op) {
            candidates[*len] = op;
            *len += 1;
        }
    }

    fn next_progress_ops_compatible(a: Op, b: Op) -> bool {
        a.i != b.i
            && a.j != b.j
            && ((a.i < b.i && a.j < b.j) || (a.i > b.i && a.j > b.j))
    }

    fn adjacent_progress_in_line(&self, line: &[usize]) -> usize {
        line.windows(2)
            .filter(|pair| self.is_adjacent_progress_pair(pair[0], pair[1]))
            .count()
    }

    fn is_adjacent_progress_pair(&self, front_car: usize, back_car: usize) -> bool {
        let row = front_car / CARS_PER_ROW;
        back_car == front_car + 1
            && row == back_car / CARS_PER_ROW
            && self.base <= row
            && row < self.base + 2
    }

    fn apply_turn_into_state(
        &self,
        state: &GroupState,
        turn: &[Op],
        next: &mut GroupState,
    ) -> bool {
        if turn.is_empty() || !is_valid_turn(turn, 10) {
            return false;
        }
        next.clone_from(state);
        self.apply_turn_in_place(next, turn)
    }

    fn apply_turn_in_place(&self, next: &mut GroupState, turn: &[Op]) -> bool {
        for &op in turn {
            if op.i < self.base || op.i >= self.base + 2 || op.j < self.base || op.j >= self.base + 2
            {
                return false;
            }
            let i = op.i - self.base;
            let j = op.j - self.base;
            match op.typ {
                0 => {
                    if next.dep[i].len() < op.k || next.siding[j].len() + op.k > SIDING_CAP {
                        return false;
                    }
                    let start = next.dep[i].len() - op.k;
                    let mut moved = next.dep[i].split_off(start);
                    moved.extend(next.siding[j].drain(..));
                    next.siding[j] = moved;
                }
                1 => {
                    if next.siding[j].len() < op.k || next.dep[i].len() + op.k > DEP_CAP {
                        return false;
                    }
                    let dep = &mut next.dep;
                    let siding = &mut next.siding;
                    dep[i].extend(siding[j].drain(0..op.k));
                }
                _ => return false,
            }
        }
        true
    }

    fn state_is_solved(&self, state: &GroupState) -> bool {
        state.siding.iter().all(|line| line.is_empty())
            && (0..2).all(|row| {
                state.dep[row].len() == CARS_PER_ROW
                    && (0..CARS_PER_ROW)
                        .all(|col| state.dep[row][col] == (self.base + row) * CARS_PER_ROW + col)
            })
    }

    fn state_fixed_prefix_len(&self, state: &GroupState, row: usize) -> usize {
        let mut fixed = 0;
        while fixed < CARS_PER_ROW
            && fixed < state.dep[row].len()
            && state.dep[row][fixed] == (self.base + row) * CARS_PER_ROW + fixed
        {
            fixed += 1;
        }
        fixed
    }

    fn state_direct_run_len(
        &self,
        state: &GroupState,
        row: usize,
        src_siding: usize,
        start_col: usize,
    ) -> usize {
        let mut k = 0;
        while start_col + k < CARS_PER_ROW
            && k < state.siding[src_siding].len()
            && state.siding[src_siding][k] == (self.base + row) * CARS_PER_ROW + start_col + k
        {
            k += 1;
        }
        k
    }

    fn state_locate_in_sidings(
        &self,
        state: &GroupState,
        target: usize,
    ) -> Option<(usize, usize)> {
        if let Some(pos) = state.siding[0].iter().position(|&car| car == target) {
            Some((0, pos))
        } else {
            state.siding[1]
                .iter()
                .position(|&car| car == target)
                .map(|pos| (1, pos))
        }
    }

    fn is_solved(&self) -> bool {
        self.siding.iter().all(|line| line.is_empty())
            && (0..2).all(|row| {
                self.dep[row].len() == CARS_PER_ROW
                    && (0..CARS_PER_ROW).all(|col| {
                        self.dep[row][col] == (self.base + row) * CARS_PER_ROW + col
                    })
            })
    }

    fn fixed_prefix_len(&self, row: usize) -> usize {
        let mut fixed = 0;
        while fixed < CARS_PER_ROW
            && fixed < self.dep[row].len()
            && self.dep[row][fixed] == (self.base + row) * CARS_PER_ROW + fixed
        {
            fixed += 1;
        }
        fixed
    }

    fn place_direct_pair_or_single(&mut self) -> bool {
        let mut candidates = Vec::new();
        for row in 0..2 {
            let fixed = self.fixed_prefix_len(row);
            if fixed >= CARS_PER_ROW || self.dep[row].len() != fixed {
                continue;
            }
            let target = (self.base + row) * CARS_PER_ROW + fixed;
            if let Some((src_siding, 0)) = self.locate_in_sidings(target) {
                let k = self.direct_run_len(row, src_siding, fixed);
                candidates.push((row, src_siding, k));
            }
        }

        if candidates.is_empty() {
            return false;
        }

        if candidates.len() == 2 {
            let a = candidates[0];
            let b = candidates[1];
            let turn = vec![
                self.absolute_op(1, a.0, a.1, a.2),
                self.absolute_op(1, b.0, b.1, b.2),
            ];
            if is_valid_turn(&turn, 10) {
                self.apply_turn(&turn);
                return true;
            }
        }

        let &(row, src_siding, k) = candidates
            .iter()
            .max_by_key(|&&(row, src_siding, k)| {
                (src_siding == row, k, self.dep[row].len())
            })
            .unwrap();
        let turn = vec![self.absolute_op(1, row, src_siding, k)];
        self.apply_turn(&turn);
        true
    }

    fn direct_run_len(&self, row: usize, src_siding: usize, start_col: usize) -> usize {
        let mut k = 0;
        while start_col + k < CARS_PER_ROW
            && k < self.siding[src_siding].len()
            && self.siding[src_siding][k] == (self.base + row) * CARS_PER_ROW + start_col + k
        {
            k += 1;
        }
        k
    }

    fn clear_best_buffer(&mut self) -> bool {
        let mut best: Option<(i64, usize, usize, i64, usize, usize)> = None;
        for row in 0..2 {
            let fixed = self.fixed_prefix_len(row);
            if self.dep[row].len() <= fixed {
                continue;
            }
            let buffer_len = self.dep[row].len() - fixed;
            for dst_siding in 0..2 {
                let free = SIDING_CAP - self.siding[dst_siding].len();
                if free == 0 {
                    continue;
                }
                let q = buffer_len.min(free);
                let bury_penalty = self.next_target_bury_penalty(dst_siding);
                let moved_start = self.dep[row].len() - q;
                let moved = &self.dep[row][moved_start..];
                let same_goal_count = moved
                    .iter()
                    .filter(|&&car| car / CARS_PER_ROW == self.base + dst_siding)
                    .count();
                let front_goal_bonus = moved
                    .first()
                    .map_or(0_i64, |&car| {
                        if car / CARS_PER_ROW == self.base + dst_siding {
                            1
                        } else {
                            0
                        }
                    });
                let score = (
                    -bury_penalty,
                    q,
                    same_goal_count,
                    front_goal_bonus - self.siding[dst_siding].len() as i64,
                    dst_siding,
                    row,
                );
                if best.map_or(true, |best_score| score > best_score) {
                    best = Some(score);
                }
            }
        }
        if let Some((_, q, _, _, dst_siding, row)) = best {
            let turn = vec![self.absolute_op(0, row, dst_siding, q)];
            self.apply_turn(&turn);
            true
        } else {
            false
        }
    }

    fn next_target_bury_penalty(&self, siding_idx: usize) -> i64 {
        let mut penalty = 0;
        for row in 0..2 {
            let fixed = self.fixed_prefix_len(row);
            if fixed >= CARS_PER_ROW {
                continue;
            }
            let target = (self.base + row) * CARS_PER_ROW + fixed;
            if let Some(pos) = self.siding[siding_idx]
                .iter()
                .position(|&car| car == target)
            {
                penalty += 30 - pos.min(20) as i64;
            }
        }
        penalty
    }

    fn expose_best_target(&mut self) {
        let mut best: Option<(bool, std::cmp::Reverse<usize>, usize, usize, usize, usize)> =
            None;
        for row in 0..2 {
            let fixed = self.fixed_prefix_len(row);
            if fixed >= CARS_PER_ROW || self.dep[row].len() != fixed {
                continue;
            }
            let target = (self.base + row) * CARS_PER_ROW + fixed;
            let Some((src_siding, pos)) = self.locate_in_sidings(target) else {
                continue;
            };
            if pos == 0 {
                continue;
            }
            let tmp_dep = 1 - row;
            let free_tmp = DEP_CAP - self.dep[tmp_dep].len();
            if free_tmp == 0 {
                continue;
            }
            let q = pos.min(free_tmp);
            let full_expose = q == pos;
            let key = (full_expose, std::cmp::Reverse(pos), q, row, src_siding, tmp_dep);
            if best.map_or(true, |best_key| key > best_key) {
                best = Some(key);
            }
        }

        if let Some((_, std::cmp::Reverse(pos), q, _row, src_siding, tmp_dep)) = best {
            debug_assert!(q > 0);
            debug_assert!(q <= pos);
            let turn = vec![self.absolute_op(1, tmp_dep, src_siding, q)];
            self.apply_turn(&turn);
        } else {
            let cleared = self.clear_best_buffer();
            assert!(cleared);
        }
    }

    fn locate_in_sidings(&self, target: usize) -> Option<(usize, usize)> {
        if let Some(pos) = self.siding[0].iter().position(|&car| car == target) {
            Some((0, pos))
        } else {
            self.siding[1]
                .iter()
                .position(|&car| car == target)
                .map(|pos| (1, pos))
        }
    }

    fn absolute_op(&self, typ: usize, i: usize, j: usize, k: usize) -> Op {
        Op {
            typ,
            i: self.base + i,
            j: self.base + j,
            k,
        }
    }

    fn apply_turn(&mut self, turn: &[Op]) {
        assert!(is_valid_turn(turn, 10));
        for &op in turn {
            let local_i = op.i - self.base;
            let local_j = op.j - self.base;
            match op.typ {
                0 => self.move_dep_to_siding(local_i, local_j, op.k),
                1 => self.move_siding_to_dep(local_j, local_i, op.k),
                _ => unreachable!(),
            }
        }
        self.turns.push(turn.to_vec());
    }

    fn move_dep_to_siding(&mut self, i: usize, j: usize, k: usize) {
        assert!(k > 0);
        assert!(self.dep[i].len() >= k);
        assert!(self.siding[j].len() + k <= SIDING_CAP);

        let start = self.dep[i].len() - k;
        let mut moved = self.dep[i].split_off(start);
        moved.extend(self.siding[j].drain(..));
        self.siding[j] = moved;
    }

    fn move_siding_to_dep(&mut self, j: usize, i: usize, k: usize) {
        assert!(k > 0);
        assert!(self.siding[j].len() >= k);
        assert!(self.dep[i].len() + k <= DEP_CAP);

        let dep = &mut self.dep;
        let siding = &mut self.siding;
        dep[i].extend(siding[j].drain(0..k));
    }
}

fn choose_best_group_turn(runs: &[GroupRun], dep: &[Vec<usize>]) -> Vec<Op> {
    let state = phase1_initial_state(dep);
    phase1_generate_turn_candidates(runs, dep, &state, phase1_evacuated_key_from_dep(dep), 1)
        .into_iter()
        .next()
        .map_or_else(Vec::new, |candidate| candidate.ops)
}

fn phase1_initial_state(dep: &[Vec<usize>]) -> Phase1State {
    let mut len = [0_u8; 10];
    for (i, line) in dep.iter().enumerate() {
        len[i] = line.len() as u8;
    }
    Phase1State { len }
}

fn phase1_beam_solve(dep: &[Vec<usize>], _siding: &[Vec<usize>]) -> Option<Vec<Vec<Op>>> {
    let initial_key = phase1_evacuated_key_from_dep(dep);
    let initial = phase1_initial_state(dep);
    if phase1_is_complete(&initial) {
        return Some(Vec::new());
    }

    let mut layers = vec![vec![Phase1Node {
        score: phase1_evaluate_state(dep, &initial, 0),
        state: initial,
        evacuated_key: initial_key,
        parent: None,
        turn: Vec::new(),
    }]];

    for depth in 0..PHASE1_BEAM_MAX_DEPTH {
        let current_layer = layers.len() - 1;
        let mut next_nodes: Vec<Phase1Node> = Vec::new();
        let mut seen: HashMap<Phase1EvacuatedKey, usize> = HashMap::new();

        for parent_idx in 0..layers[current_layer].len() {
            let node = &layers[current_layer][parent_idx];
            let runs = phase1_collect_runs(dep, &node.state);
            if runs.is_empty() {
                continue;
            }
            let candidates = phase1_generate_turn_candidates(
                &runs,
                dep,
                &node.state,
                node.evacuated_key,
                PHASE1_TURN_CAND_LIMIT,
            );
            for candidate in candidates {
                let mut next_state = node.state;
                phase1_apply_turn_fast(&mut next_state, &candidate.ops);
                let score = phase1_evaluate_state(dep, &next_state, depth + 1);
                let next_key = candidate.evacuated_key;
                if let Some(idx) = seen.get(&next_key).copied() {
                    if score > next_nodes[idx].score {
                        next_nodes[idx] = Phase1Node {
                            state: next_state,
                            evacuated_key: next_key,
                            score,
                            parent: Some(parent_idx),
                            turn: candidate.ops,
                        };
                    }
                } else {
                    seen.insert(next_key, next_nodes.len());
                    next_nodes.push(Phase1Node {
                        state: next_state,
                        evacuated_key: next_key,
                        score,
                        parent: Some(parent_idx),
                        turn: candidate.ops,
                    });
                }
            }
        }

        if next_nodes.is_empty() {
            break;
        }
        let solved_idx = next_nodes
            .iter()
            .position(|node| phase1_is_complete(&node.state));
        if let Some(idx) = solved_idx {
            layers.push(next_nodes);
            let solved_layer = layers.len() - 1;
            return Some(phase1_reconstruct_path(&layers, solved_layer, idx));
        }

        next_nodes.sort_unstable_by(|a, b| b.score.cmp(&a.score));
        if next_nodes.len() > PHASE1_BEAM_WIDTH {
            next_nodes.truncate(PHASE1_BEAM_WIDTH);
        }

        layers.push(next_nodes);
    }

    None
}

fn phase1_reconstruct_path(
    layers: &[Vec<Phase1Node>],
    mut layer_idx: usize,
    mut node_idx: usize,
) -> Vec<Vec<Op>> {
    let mut path = Vec::new();
    while layer_idx > 0 {
        let node = &layers[layer_idx][node_idx];
        path.push(node.turn.clone());
        node_idx = node.parent.unwrap();
        layer_idx -= 1;
    }
    path.reverse();
    path
}

fn phase1_collect_runs(dep: &[Vec<usize>], state: &Phase1State) -> Vec<GroupRun> {
    let mut runs = Vec::new();
    for i in 0..dep.len() {
        let len = state.len[i] as usize;
        if len == 0 {
            continue;
        }
        let goal_row = dep[i][len - 1] / CARS_PER_ROW;
        let group = goal_row / 2;
        let mut k = 1;
        while k < len && (dep[i][len - 1 - k] / CARS_PER_ROW) / 2 == group {
            k += 1;
        }

        runs.push(GroupRun { i, group, k });
    }
    runs
}

fn phase1_generate_turn_candidates(
    runs: &[GroupRun],
    dep: &[Vec<usize>],
    state: &Phase1State,
    current_key: Phase1EvacuatedKey,
    limit: usize,
) -> Vec<Phase1TurnCandidate> {
    let n = runs.len();
    let mut candidates = Vec::new();

    for mask in 1usize..(1usize << n) {
        let mut counts = [0usize; 5];
        let mut last_group = None;
        let mut valid = true;

        for (idx, run) in runs.iter().enumerate() {
            if (mask >> idx) & 1 == 0 {
                continue;
            }
            if let Some(last) = last_group {
                if run.group < last {
                    valid = false;
                    break;
                }
            }
            last_group = Some(run.group);
            counts[run.group] += 1;
            if counts[run.group] > 2 {
                valid = false;
                break;
            }
        }
        if !valid {
            continue;
        }

        let mut used_in_group = [0usize; 5];
        let mut ops = Vec::new();
        for (idx, run) in runs.iter().enumerate() {
            if (mask >> idx) & 1 == 0 {
                continue;
            }

            let left = run.group * 2;
            let right = left + 1;
            let j = if counts[run.group] == 1 {
                left
            } else if used_in_group[run.group] == 0 {
                left
            } else {
                right
            };

            ops.push(Op {
                typ: 0,
                i: run.i,
                j,
                k: run.k,
            });
            used_in_group[run.group] += 1;
        }

        if valid {
            phase1_push_turn_candidate(&mut candidates, ops, dep, state, current_key);
        }
    }

    phase1_select_diverse_candidates(candidates, limit)
}

fn phase1_select_diverse_candidates(
    mut candidates: Vec<Phase1TurnCandidate>,
    limit: usize,
) -> Vec<Phase1TurnCandidate> {
    if candidates.is_empty() || limit == 0 {
        return Vec::new();
    }

    let mut selected = Vec::with_capacity(limit);
    let mut seen_states = HashSet::new();
    candidates.sort_unstable_by(|a, b| {
        b.score
            .cmp(&a.score)
            .then_with(|| b.ops.len().cmp(&a.ops.len()))
    });
    for candidate in candidates {
        if seen_states.insert(candidate.evacuated_key) {
            selected.push(candidate);
            if selected.len() == limit {
                break;
            }
        }
    }
    selected
}

fn phase1_evacuated_key_from_dep(dep: &[Vec<usize>]) -> Phase1EvacuatedKey {
    let total_cars = dep.len() * CARS_PER_ROW;
    let mut bits = [0_u64; 2];
    for car in 0..total_cars {
        bits[car / 64] |= 1_u64 << (car % 64);
    }
    for line in dep {
        for &car in line {
            bits[car / 64] &= !(1_u64 << (car % 64));
        }
    }
    Phase1EvacuatedKey { bits }
}

fn phase1_push_turn_candidate(
    candidates: &mut Vec<Phase1TurnCandidate>,
    ops: Vec<Op>,
    dep: &[Vec<usize>],
    state: &Phase1State,
    current_key: Phase1EvacuatedKey,
) {
    if let Some((score, evacuated_key)) =
        phase1_score_turn_ops_and_key(&ops, dep, state, current_key)
    {
        candidates.push(Phase1TurnCandidate {
            ops,
            evacuated_key,
            score,
        });
    }
}

fn phase1_score_turn_ops_and_key(
    ops: &[Op],
    dep: &[Vec<usize>],
    state: &Phase1State,
    current_key: Phase1EvacuatedKey,
) -> Option<((i64, i64, i64, i64), Phase1EvacuatedKey)> {
    if ops.is_empty() || ops.len() > dep.len() || dep.len() > 10 {
        return None;
    }

    let mut remaining = [0usize; 10];
    let mut used_dep = [false; 10];
    let mut used_siding = [false; 10];
    let mut max_before = 0_i64;
    for idx in 0..dep.len() {
        remaining[idx] = state.len[idx] as usize;
        max_before = max_before.max(state.len[idx] as i64);
    }
    let mut moved_sum = 0_i64;
    let mut completed_blocks = 0_i64;
    let mut evacuated_key = current_key;

    for op in ops {
        if op.typ != 0 || op.i >= dep.len() || op.j >= dep.len() {
            return None;
        }
        if op.k == 0 {
            return None;
        }
        if used_dep[op.i] || used_siding[op.j] {
            return None;
        }
        used_dep[op.i] = true;
        used_siding[op.j] = true;
        let line_len = state.len[op.i] as usize;
        if line_len < op.k {
            return None;
        }
        let start = line_len - op.k;
        for &car in &dep[op.i][start..line_len] {
            evacuated_key.bits[car / 64] |= 1_u64 << (car % 64);
        }
        remaining[op.i] -= op.k;
        moved_sum += op.k as i64;
        completed_blocks += 1;
    }
    for a in 0..ops.len() {
        for b in a + 1..ops.len() {
            let lhs = ops[a];
            let rhs = ops[b];
            if lhs.i < rhs.i && lhs.j >= rhs.j {
                return None;
            }
            if lhs.i > rhs.i && lhs.j <= rhs.j {
                return None;
            }
        }
    }

    let remaining = &remaining[..dep.len()];
    let max_remaining = remaining.iter().copied().max().unwrap_or(0) as i64;
    let nonempty_count = remaining.iter().filter(|&&len| len > 0).count() as i64;
    let sum_remaining: i64 = remaining.iter().map(|&len| len as i64).sum();
    let balance_variance = remaining
        .iter()
        .map(|&len| {
            let x = len as i64 * dep.len() as i64 - sum_remaining;
            x * x
        })
        .sum::<i64>();
    let balance_gain = (max_before - max_remaining) * 10
        + moved_sum
        - balance_variance / 100
        - nonempty_count * 2;

    Some((
        (
            completed_blocks,
            ops.len() as i64,
            moved_sum,
            balance_gain,
        ),
        evacuated_key,
    ))
}

fn phase1_apply_turn_fast(state: &mut Phase1State, turn: &[Op]) {
    for &op in turn {
        state.len[op.i] -= op.k as u8;
    }
}

fn phase1_is_complete(state: &Phase1State) -> bool {
    state.len.iter().all(|&len| len == 0)
}

fn phase1_evaluate_state(dep: &[Vec<usize>], state: &Phase1State, depth: usize) -> i64 {
    let metrics = phase1_dep_metrics(dep, state);
    let total_cars = dep.len() as i64 * CARS_PER_ROW as i64;
    let moved_sum = total_cars - metrics.remaining_sum;
    let estimated_total_turns = depth as i64 + metrics.lower_bound;

    -estimated_total_turns * 10_000_000
        + moved_sum * 100_000
        - metrics.max_remaining * 20_000
        - metrics.nonempty_count * 5_000
        - metrics.balance_variance
}

fn phase1_dep_metrics(dep: &[Vec<usize>], state: &Phase1State) -> Phase1DepMetrics {
    let mut remaining = [0_i64; 10];
    let mut remaining_sum = 0_i64;
    let mut max_remaining = 0_i64;
    let mut nonempty_count = 0_i64;
    let mut source_lb = 0_i64;
    let mut group_blocks = [0_i64; 5];
    let mut total_blocks = 0_i64;

    for (idx, line) in dep.iter().enumerate() {
        let len = state.len[idx] as usize;
        let len_i64 = len as i64;
        remaining[idx] = len_i64;
        remaining_sum += len_i64;
        max_remaining = max_remaining.max(len_i64);
        if len > 0 {
            nonempty_count += 1;
        }

        if len == 0 {
            continue;
        }
        let mut blocks = 1_i64;
        let mut prev_group = (line[0] / CARS_PER_ROW) / 2;
        group_blocks[prev_group] += 1;
        for &car in line.iter().take(len).skip(1) {
            let group = (car / CARS_PER_ROW) / 2;
            if group != prev_group {
                blocks += 1;
                group_blocks[group] += 1;
                prev_group = group;
            }
        }
        source_lb = source_lb.max(blocks);
        total_blocks += blocks;
    }

    let line_count = dep.len() as i64;
    let balance_variance = remaining[..dep.len()]
        .iter()
        .map(|&len| {
            let x = len * line_count - remaining_sum;
            x * x
        })
        .sum::<i64>();

    let mut group_lb = 0_i64;
    for group in 0..dep.len() / 2 {
        let blocks = group_blocks[group];
        if blocks == 0 {
            continue;
        }
        group_lb = group_lb.max((blocks + 1) / 2);
    }
    let total_lb = (total_blocks + dep.len() as i64 - 1) / dep.len() as i64;
    let lower_bound = source_lb.max(group_lb).max(total_lb);

    Phase1DepMetrics {
        remaining_sum,
        max_remaining,
        nonempty_count,
        balance_variance,
        lower_bound,
    }
}

fn is_valid_turn(turn: &[Op], r: usize) -> bool {
    if turn.len() > r || r > 10 {
        return false;
    }
    let mut used_dep = [false; 10];
    let mut used_siding = [false; 10];
    for op in turn {
        if op.i >= r || op.j >= r || op.k == 0 {
            return false;
        }
        if used_dep[op.i] || used_siding[op.j] {
            return false;
        }
        used_dep[op.i] = true;
        used_siding[op.j] = true;
    }

    for a in 0..turn.len() {
        for b in a + 1..turn.len() {
            let lhs = turn[a];
            let rhs = turn[b];
            if lhs.i < rhs.i && lhs.j >= rhs.j {
                return false;
            }
            if lhs.i > rhs.i && lhs.j <= rhs.j {
                return false;
            }
        }
    }
    true
}

fn main() {
    let mut input = String::new();
    io::stdin().read_to_string(&mut input).unwrap();
    let mut it = input.split_whitespace();

    let r: usize = it.next().unwrap().parse().unwrap();
    let mut dep = vec![vec![0; CARS_PER_ROW]; r];
    for i in 0..r {
        for j in 0..CARS_PER_ROW {
            dep[i][j] = it.next().unwrap().parse().unwrap();
        }
    }

    let already_done =
        (0..r).all(|i| (0..CARS_PER_ROW).all(|j| dep[i][j] == CARS_PER_ROW * i + j));
    if already_done {
        println!("0");
        return;
    }

    let mut solver = Solver::new(dep);
    solver.solve();

    println!("{}", solver.turns.len());
    for turn in solver.turns {
        println!("{}", turn.len());
        for op in turn {
            println!("{} {} {} {}", op.typ, op.i, op.j, op.k);
        }
    }
}
