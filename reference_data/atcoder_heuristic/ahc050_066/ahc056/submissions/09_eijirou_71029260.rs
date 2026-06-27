use std::{
    collections::{BinaryHeap, VecDeque},
    process::exit,
};

use itertools::Itertools;
use ordered_float::OrderedFloat;
use proconio::input;

const TIME_LIMIT_SEC: f64 = 1.95;
const UDLR: [char; 4] = ['U', 'D', 'L', 'R'];
const DXDY: [(usize, usize); 4] = [(!0, 0), (1, 0), (0, !0), (0, 1)];

const EXTEND_ROUTE_ID_MIN: usize = 10;

fn main() {
    get_time_sec();

    let input = read_input();
    let grid = Grid::new(&input);
    let mut rng = XorShift32::new(1);
    let mut rngs = RandomGenerators::new(input.targets.len());

    let mut best_output: Option<Output> = None;
    while get_time_sec() < TIME_LIMIT_SEC {
        let mut nrng = rngs.clone();
        nrng.change_seeds(&mut rng, input.targets.len() / 10);
        nrng.reset();
        let route = grid.calc_shortest_route(&mut nrng);
        if get_time_sec() >= TIME_LIMIT_SEC {
            break;
        }
        let mut route_optimizer = RouteOptimizer::new(&input, route);
        route_optimizer.optimize_route();
        if get_time_sec() >= TIME_LIMIT_SEC {
            break;
        }
        let transition_builder = TransitionBuilder::new(input.n, &route_optimizer.route);
        let output = if let Some(output) = &best_output {
            transition_builder.optimize_num_colors_from(output.c)
        } else {
            transition_builder.optimize_num_colors()
        };
        if best_output.is_none() || output.cost() <= best_output.as_ref().unwrap().cost() {
            std::mem::swap(&mut rngs, &mut nrng);
        }
        if best_output.is_none() || output.cost() < best_output.as_ref().unwrap().cost() {
            best_output = Some(output);
        }
    }
    best_output.unwrap().print();

    exit(0);
}

/// same transition group
#[derive(Debug, Clone)]
struct Group {
    state: usize,        // internal state / 4
    dir_fr: usize,       // direction from
    dir_to: usize,       // direction to
    next_group: usize,   // next group id
    decrement: bool,     // whether to decrement state
    color_fr: usize,     // assigned color (!0 if not assigned)
    members: Vec<usize>, // 4 * state + dir_fr of same color groups
}

impl Group {
    fn straight(num_colors: usize) -> Self {
        Self {
            state: !0,
            dir_fr: !0,
            dir_to: !0,
            next_group: !0,
            decrement: false,
            color_fr: num_colors, // straight color
            members: vec![!0],
        }
    }

    fn internal_state_from(&self) -> usize {
        if self.state == !0 {
            return !0;
        }
        self.state * 4 + self.dir_fr
    }

    fn internal_state_to(&self) -> usize {
        if self.state == !0 {
            return !0;
        }
        self.state * 4 + self.dir_to - if self.decrement { 4 } else { 0 }
    }

    fn transition_id(&self, num_colors: usize, color: usize) -> usize {
        self.state * (4 * num_colors) + color * 4 + self.dir_fr
    }
}

#[derive(Debug, Clone)]
struct ColorAssigner {
    n: usize,
    waypoints: Vec<(usize, usize)>,
    actions: Vec<(usize, usize, usize)>, // (waypoint id, direction from, direction to)
    first_pos: (usize, usize),
    first_dir_fr: usize,
    first_dir_to: usize,
    first_state: usize,
    num_colors: usize,
    groups: Vec<Group>,
    dsu: DisjointSetUnion,          // merged groups
    next_group: Vec<usize>,         // [waypoint id] = next group id
    used_colors: Vec<bool>,         // [border_id * 4 * num_colors + 4 * color + dir_fr] = is used
    state: usize,                   // internal state / 4
    dir_cnt: [usize; 4],            // number of directions in current state
    dirdir_groups: Vec<Vec<usize>>, // [d_fr * 4 + d_to] = group ids
    action_to_group: Vec<usize>,    // [action id] = group id
    remain_trans: Vec<usize>,       // unused internal states
    colors: Vec<usize>,             // [x * n + y] = color
    additional_colors: usize,
    rules: Vec<TransitionRule>,
    members_buf: Vec<usize>,
}

impl ColorAssigner {
    fn new(
        n: usize,
        waypoints: Vec<(usize, usize)>,
        last_is_free: Vec<bool>,
        actions: Vec<(usize, usize, usize)>,
        first_pos: (usize, usize),
        first_dir_to: usize,
        num_colors: usize,
    ) -> Self {
        debug_assert!(num_colors >= 1);
        let first_dir_fr = first_dir_to;
        let first_state = !0;
        let groups = vec![Group::straight(num_colors)];
        let dsu = DisjointSetUnion::new(1);
        let mut next_group = vec![0; waypoints.len()];
        for i in 0..waypoints.len() {
            if last_is_free[i] {
                next_group[i] = !0;
            }
        }
        let used_colors = vec![];
        let state = 0;
        let dir_cnt = [0; 4];
        let dirdir_groups = vec![vec![]; 16];
        let action_to_group = vec![!0; actions.len()];
        let remain_trans = vec![];
        let colors = vec![num_colors; n * n];
        let rules = vec![];
        Self {
            n,
            waypoints,
            actions,
            first_pos,
            first_dir_fr,
            first_dir_to,
            first_state,
            num_colors,
            groups,
            dsu,
            next_group,
            used_colors,
            state,
            dir_cnt,
            dirdir_groups,
            action_to_group,
            remain_trans,
            colors,
            additional_colors: 0,
            rules,
            members_buf: vec![],
        }
    }

    fn assign_action_to_group(&mut self, action_id: usize) -> bool {
        let (u, mut dir_fr, dir_to) = self.actions[action_id];
        if dir_fr == !0 {
            // start position
            // try to add to existing group
            for d in 0..4 {
                for i in 0..self.dirdir_groups[d * 4 + dir_to].len() {
                    let group_id = self.dirdir_groups[d * 4 + dir_to][i];
                    if self.try_to_add_to_group(action_id, group_id) {
                        // dir_fr = d;
                        self.first_dir_fr = d;
                        return false;
                    }
                }
            }
            dir_fr = self.first_dir_fr;
        } else {
            // try to add to existing group
            for i in 0..self.dirdir_groups[dir_fr * 4 + dir_to].len() {
                let group_id = self.dirdir_groups[dir_fr * 4 + dir_to][i];
                if self.try_to_add_to_group(action_id, group_id) {
                    return false;
                }
            }
        }
        // create new group
        let decrement = self.dir_cnt[dir_fr] + 1 > self.num_colors;
        if decrement {
            // update state
            for d in 0..4 {
                for _ in self.dir_cnt[d]..self.num_colors {
                    self.remain_trans.push(self.state * 4 + d);
                }
            }
            if self.try_to_complete(action_id) {
                return true;
            }
            self.state += 1;
            self.dir_cnt.fill(0);
            self.dir_cnt[dir_fr] = 1;
            self.dirdir_groups.iter_mut().for_each(|v| v.clear());
            self.used_colors.extend(vec![false; self.num_colors * 4]);
        } else {
            self.dir_cnt[dir_fr] += 1;
        }
        let group_id = self.groups.len();
        let next_group = self.next_group[u];
        self.groups.push(Group {
            state: self.state,
            dir_fr,
            dir_to,
            next_group,
            decrement,
            color_fr: !0,
            members: vec![self.state * 4 + dir_fr],
        });
        self.action_to_group[action_id] = group_id;
        self.dsu.extend(1);
        self.next_group[u] = group_id;
        if !decrement {
            self.dirdir_groups[dir_fr * 4 + dir_to].push(group_id);
        }
        false
    }

    fn assign(&mut self) -> Output {
        self.used_colors.resize(self.num_colors * 4, false);
        for action_id in (0..self.actions.len()).rev() {
            if self.assign_action_to_group(action_id) {
                return self.create_output();
            }
        }
        self.assign_colors_to_all_groups();
        self.update_colors();
        self.create_output()
    }

    fn assign_colors_to_all_groups(&mut self) {
        // assign colors
        for gu in 1..self.groups.len() {
            let gv = self.leader(gu);
            if self.groups[gv].color_fr != !0 {
                self.groups[gu].color_fr = self.groups[gv].color_fr;
                continue;
            }
            for color in 0..self.num_colors {
                let transition_id = self.get_transition_id(gv, color);
                if self.used_colors[transition_id] {
                    continue;
                }
                self.used_colors[transition_id] = true;
                self.groups[gv].color_fr = color;
                break;
            }
            debug_assert!(self.groups[gv].color_fr != !0);
            self.groups[gu].color_fr = self.groups[gv].color_fr;
        }
    }

    fn update_colors(&mut self) {
        for waypoint_id in 0..self.waypoints.len() {
            let (x, y) = self.waypoints[waypoint_id];
            let mut g = self.next_group[waypoint_id];
            if g == !0 {
                g = 0;
            }
            self.colors[x * self.n + y] = self.groups[g].color_fr;
        }
    }

    fn create_output(&self) -> Output {
        // create output
        let mut output = Output {
            n: self.n,
            c: self.num_colors + 1,
            q: 4 * (self.state + 1) + self.additional_colors,
            s: self.colors.clone(),
            rules: self.rules.clone(),
        };
        // straight color
        for q in 0..output.q {
            output.rules.push(TransitionRule {
                c: self.num_colors,
                q,
                a: self.num_colors,
                s: q,
                d: q % 4,
            });
        }
        for group in &self.groups[1..] {
            let next_group = if group.next_group == !0 {
                0
            } else {
                group.next_group
            };
            output.rules.push(TransitionRule {
                c: group.color_fr,
                q: group.internal_state_from(),
                a: self.groups[next_group].color_fr,
                s: group.internal_state_to(),
                d: group.dir_to,
            });
        }
        let first_state = if self.first_state == !0 {
            self.state * 4 + self.first_dir_fr
        } else {
            self.first_state
        };
        output.swap_state(0, first_state);
        output
    }

    /// try to complete actions by using remaining transitions
    fn try_to_complete(&mut self, action_id_end: usize) -> bool {
        let (mut sx, mut sy) = self.first_pos;
        let mut num_cells = 1;
        for action_id in 0..=action_id_end {
            let (t, _, _) = self.actions[action_id];
            let (tx, ty) = self.waypoints[t];
            num_cells += manhattan(sx, sy, tx, ty);
            if num_cells > self.remain_trans.len() + 3 * self.num_colors {
                return false;
            }
            (sx, sy) = (tx, ty);
        }
        // can complete
        if num_cells > self.remain_trans.len() {
            self.used_colors.extend(vec![false; self.num_colors * 4]);
        }
        while num_cells > self.remain_trans.len() {
            self.remain_trans.extend(vec![4 * (self.state + 1) + self.additional_colors; self.num_colors]);
            self.additional_colors += 1;
        }
        self.assign_colors_to_all_groups();
        self.update_colors();

        let gt = self.leader(self.next_group[self.actions[action_id_end].0]);
        let color_to = if gt == !0 {
            self.groups[0].color_fr
        } else {
            self.groups[gt].color_fr
        };
        let (t, _, dir_to) = self.actions[action_id_end];
        let (mut x, mut y) = self.waypoints[t];
        let mut state_to = self.state * 4 + dir_to;
        let state_fr = self.remain_trans.pop().unwrap();
        let color_fr = self.find_unused_color(state_fr / 4, state_fr % 4);
        debug_assert_ne!(color_fr, !0);
        let transition_id = state_fr / 4 * (4 * self.num_colors) + color_fr * 4 + state_fr % 4;
        debug_assert!(!self.used_colors[transition_id]);
        self.used_colors[transition_id] = true;
        self.rules.push(TransitionRule {
            c: color_fr,
            q: state_fr,
            a: color_to,
            s: state_to,
            d: dir_to,
        });
        self.colors[x * self.n + y] = color_fr;
        state_to = state_fr;

        let mut route = Vec::with_capacity(action_id_end + 1);
        let (first_x, first_y) = self.first_pos;
        route.push((first_x, first_y, self.first_dir_to));
        for action_id in 0..action_id_end {
            let (u, _, dir_to) = self.actions[action_id];
            let (ux, uy) = self.waypoints[u];
            route.push((ux, uy, dir_to));
        }
        for &(sx, sy, dir_to) in route.iter().rev() {
            let (dx, dy) = DXDY[dir_to];
            while (x, y) != (sx, sy) {
                x = x.wrapping_sub(dx);
                y = y.wrapping_sub(dy);
                let state_fr = self.remain_trans.pop().unwrap();
                let color_fr = self.find_unused_color(state_fr / 4, state_fr % 4);
                debug_assert_ne!(color_fr, !0);
                let transition_id =
                    state_fr / 4 * (4 * self.num_colors) + color_fr * 4 + state_fr % 4;
                debug_assert!(!self.used_colors[transition_id]);
                self.used_colors[transition_id] = true;
                let color_to = self.colors[x * self.n + y];
                self.rules.push(TransitionRule {
                    c: color_fr,
                    q: state_fr,
                    a: color_to,
                    s: state_to,
                    d: dir_to,
                });
                self.colors[x * self.n + y] = color_fr;
                state_to = state_fr;
            }
        }
        self.first_state = state_to;
        true
    }

    fn try_to_add_to_group(&mut self, action_id: usize, group_id: usize) -> bool {
        let (u, dir_fr, dir_to) = self.actions[action_id];
        debug_assert!(self.groups[group_id].dir_fr == dir_fr || dir_fr == !0);
        debug_assert_eq!(self.groups[group_id].dir_to, dir_to);
        debug_assert!(!self.groups[group_id].decrement);
        let gu = self.leader(self.next_group[u]);
        let gv = self.leader(self.groups[group_id].next_group);
        if gu == gv || gu == !0 {
            // add simply
            self.next_group[u] = group_id;
            self.action_to_group[action_id] = group_id;
            return true;
        }
        if gv == !0 {
            // add simply
            self.next_group[u] = group_id;
            self.action_to_group[action_id] = group_id;
            self.groups[group_id].next_group = gu;
            return true;
        }
        if self.try_to_merge_groups(gu, gv) {
            // merged
            self.next_group[u] = group_id;
            self.action_to_group[action_id] = group_id;
            return true;
        }
        false
    }

    fn try_to_merge_groups(&mut self, gu: usize, gv: usize) -> bool {
        debug_assert_eq!(self.leader(gu), gu);
        debug_assert_eq!(self.leader(gv), gv);
        if gu == gv {
            return true;
        }
        if gu == 0 || gv == 0 {
            return false;
        }
        // merge next states
        self.members_buf.clear();
        let mut i = 0;
        let mut j = 0;
        if !self.groups[gu].members.is_empty() && !self.groups[gv].members.is_empty() {
            let mut qu = self.groups[gu].members[0];
            let mut qv = self.groups[gv].members[0];
            loop {
                if qu == qv {
                    return false;
                }
                if qu < qv {
                    self.members_buf.push(qu);
                    i += 1;
                    if i == self.groups[gu].members.len() {
                        break;
                    }
                    qu = self.groups[gu].members[i];
                } else {
                    self.members_buf.push(qv);
                    j += 1;
                    if j == self.groups[gv].members.len() {
                        break;
                    }
                    qv = self.groups[gv].members[j];
                }
            }
        }
        self.members_buf
            .extend(self.groups[gu].members[i..].iter().cloned());
        self.members_buf
            .extend(self.groups[gv].members[j..].iter().cloned());
        let u_color = self.groups[gu].color_fr;
        let v_color = self.groups[gv].color_fr;
        if u_color == v_color && u_color != !0 {
            // merge directly
            let leader = self.dsu.merge(gu, gv);
            std::mem::swap(&mut self.groups[leader].members, &mut self.members_buf);
            return true;
        }
        // try to merge gv to gu
        if u_color != !0
            && self.groups[gv].members.iter().all(|&qv| {
                let state = qv / 4;
                let dir_fr = qv % 4;
                let transition_id = state * (4 * self.num_colors) + u_color * 4 + dir_fr;
                !self.used_colors[transition_id]
            })
        {
            // merge gv to gu
            self.set_color(gv, u_color);
            let leader = self.dsu.merge(gu, gv);
            std::mem::swap(&mut self.groups[leader].members, &mut self.members_buf);
            return true;
        }
        // try to merge gu to gv
        if v_color != !0
            && self.groups[gu].members.iter().all(|&qu| {
                let state = qu / 4;
                let dir_fr = qu % 4;
                let transition_id = state * (4 * self.num_colors) + v_color * 4 + dir_fr;
                !self.used_colors[transition_id]
            })
        {
            // merge gu to gv
            self.set_color(gu, v_color);
            let leader = self.dsu.merge(gu, gv);
            std::mem::swap(&mut self.groups[leader].members, &mut self.members_buf);
            return true;
        }
        // try to assign new color
        for color in 0..self.num_colors {
            if color == u_color || color == v_color {
                continue;
            }
            if self.members_buf.iter().all(|&q| {
                let state = q / 4;
                let dir_fr = q % 4;
                let transition_id = state * (4 * self.num_colors) + color * 4 + dir_fr;
                !self.used_colors[transition_id]
            }) {
                // assign new color
                self.set_color(gu, color);
                self.set_color(gv, color);
                let leader = self.dsu.merge(gu, gv);
                std::mem::swap(&mut self.groups[leader].members, &mut self.members_buf);
                return true;
            }
        }
        false
    }

    fn set_color(&mut self, group_id: usize, color: usize) {
        let group_id = self.leader(group_id);
        if self.groups[group_id].color_fr != !0 {
            self.reset_color(group_id);
        }
        for &q in &self.groups[group_id].members {
            let state = q / 4;
            let dir_fr = q % 4;
            let transition_id = state * (4 * self.num_colors) + color * 4 + dir_fr;
            debug_assert!(!self.used_colors[transition_id]);
            self.used_colors[transition_id] = true;
        }
        self.groups[group_id].color_fr = color;
    }

    fn reset_color(&mut self, group_id: usize) {
        let group_id = self.leader(group_id);
        let color = self.groups[group_id].color_fr;
        if color == !0 {
            return;
        }
        for &q in &self.groups[group_id].members {
            let state = q / 4;
            let dir_fr = q % 4;
            let transition_id = state * (4 * self.num_colors) + color * 4 + dir_fr;
            debug_assert!(self.used_colors[transition_id]);
            self.used_colors[transition_id] = false;
        }
        self.groups[group_id].color_fr = !0;
    }

    fn get_transition_id(&self, group_id: usize, color: usize) -> usize {
        debug_assert!(color < self.num_colors);
        self.groups[group_id].transition_id(self.num_colors, color)
    }

    fn find_unused_color(&self, state: usize, dir_fr: usize) -> usize {
        for color in 0..self.num_colors {
            let transition_id = state * (4 * self.num_colors) + color * 4 + dir_fr;
            if !self.used_colors[transition_id] {
                return color;
            }
        }
        !0
    }

    fn leader(&mut self, group_id: usize) -> usize {
        if group_id == !0 {
            return !0;
        }
        self.dsu.leader(group_id)
    }
}

#[derive(Debug, Clone)]
struct TransitionBuilder {
    n: usize,
    waypoints: Vec<(usize, usize)>,
    last_is_free: Vec<bool>, // [waypoint id] = whether the last color is free to decide
    actions: Vec<(usize, usize, usize)>, // (waypoint id, direction from, direction to)
    first_pos: (usize, usize),
    first_dir_to: usize,
}

impl TransitionBuilder {
    fn new(n: usize, route_simplified: &[(usize, usize)]) -> Self {
        // insert intermediate waypoints
        let mut waypoints = Vec::with_capacity(route_simplified.len());
        let mut waypoints_rows = vec![vec![]; n];
        let mut waypoints_cols = vec![vec![]; n];
        for &(x, y) in &route_simplified[1..route_simplified.len() - 1] {
            waypoints.push((x, y));
            waypoints_rows[x].push(y);
            waypoints_cols[y].push(x);
        }
        for z in 0..n {
            waypoints_rows[z].sort_unstable();
            waypoints_rows[z].dedup();
            waypoints_cols[z].sort_unstable();
            waypoints_cols[z].dedup();
        }
        let mut route = vec![route_simplified[0]];
        let (mut ux, mut uy) = route_simplified[0];
        for &(vx, vy) in &route_simplified[1..] {
            match get_direction(ux, uy, vx, vy) {
                0 => {
                    // up
                    let y = uy;
                    route.extend(
                        waypoints_cols[y]
                            .iter()
                            .rev()
                            .filter(|&&x| vx < x && x < ux)
                            .map(|&x| (x, y)),
                    );
                }
                1 => {
                    // down
                    let y = uy;
                    route.extend(
                        waypoints_cols[y]
                            .iter()
                            .filter(|&&x| ux < x && x < vx)
                            .map(|&x| (x, y)),
                    );
                }
                2 => {
                    // left
                    let x = ux;
                    route.extend(
                        waypoints_rows[x]
                            .iter()
                            .rev()
                            .filter(|&&y| y < uy && y > vy)
                            .map(|&y| (x, y)),
                    );
                }
                3 => {
                    // right
                    let x = ux;
                    route.extend(
                        waypoints_rows[x]
                            .iter()
                            .filter(|&&y| uy < y && y < vy)
                            .map(|&y| (x, y)),
                    );
                }
                _ => unreachable!(),
            }
            route.push((vx, vy));
            ux = vx;
            uy = vy;
        }
        let mut waypoint_to_id = vec![!0; n * n];
        for (i, &(x, y)) in waypoints.iter().enumerate() {
            waypoint_to_id[x * n + y] = i;
        }
        let mut used = vec![false; waypoints.len()];
        let mut actions = Vec::with_capacity(route.len() - 1);
        let mut last_is_free = vec![true; waypoints.len()];
        for i in (1..route.len() - 1).rev() {
            let (ux, uy) = route[i - 1];
            let (vx, vy) = route[i];
            let (wx, wy) = route[i + 1];
            let iv = waypoint_to_id[vx * n + vy];
            let dir_fr = get_direction(ux, uy, vx, vy);
            let dir_to = get_direction(vx, vy, wx, wy);
            if dir_fr == dir_to && !used[iv] {
                last_is_free[iv] = false;
                continue;
            }
            used[iv] = true;
            actions.push((iv, dir_fr, dir_to));
        }
        let first_pos = route[0];
        let (first_x, first_y) = first_pos;
        let (second_x, second_y) = route[1];
        let first_dir_to = get_direction(first_x, first_y, second_x, second_y);
        if waypoints.contains(&first_pos) {
            let iv = waypoint_to_id[first_x * n + first_y];
            actions.push((iv, !0, first_dir_to));
        }
        actions.reverse();

        Self {
            n,
            waypoints,
            last_is_free,
            actions,
            first_pos,
            first_dir_to,
        }
    }

    fn get_num_colors_max(&self) -> usize {
        (self.waypoints.len() / 4).max(10)
    }

    fn optimize_num_colors_from(&self, num_colors_prev: usize) -> Output {
        const COLORS_MIN_DELTA: usize = 4;
        const COLORS_MAX_DELTA: usize = 2;
        let mut best_output: Option<Output> = None;
        let colors_min = num_colors_prev.saturating_sub(COLORS_MIN_DELTA).max(2);
        let colors_max = num_colors_prev + COLORS_MAX_DELTA;
        for num_colors in colors_min..=colors_max {
            let mut color_assigner = ColorAssigner::new(
                self.n,
                self.waypoints.clone(),
                self.last_is_free.clone(),
                self.actions.clone(),
                self.first_pos,
                self.first_dir_to,
                num_colors,
            );
            let output = color_assigner.assign();
            if best_output.is_none() || output.cost() < best_output.as_ref().unwrap().cost() {
                best_output = Some(output);
            }
        }
        best_output.unwrap()
    }

    fn optimize_num_colors(&self) -> Output {
        let num_colors_max = self.get_num_colors_max();
        let mut outputs = vec![None; num_colors_max + 1];
        let get_cost = |num_colors: usize, outputs: &mut Vec<Option<Output>>| -> usize {
            if let Some(output) = &outputs[num_colors] {
                return output.cost();
            }
            let mut color_assigner = ColorAssigner::new(
                self.n,
                self.waypoints.clone(),
                self.last_is_free.clone(),
                self.actions.clone(),
                self.first_pos,
                self.first_dir_to,
                num_colors,
            );
            outputs[num_colors] = Some(color_assigner.assign());
            outputs[num_colors].as_ref().unwrap().cost()
        };
        // ternary search
        let mut left = 1;
        let mut right = num_colors_max;
        while right - left >= 3 {
            let mid1 = left + (right - left) / 3;
            let mid2 = right - (right - left) / 3;
            let mid1_cost = get_cost(mid1, &mut outputs);
            let mid2_cost = get_cost(mid2, &mut outputs);
            if mid1_cost < mid2_cost {
                right = mid2;
            } else {
                left = mid1;
            }
        }
        for num_colors in left..=right {
            get_cost(num_colors, &mut outputs);
        }
        let mut best_num_colors = left;
        let mut min_cost = get_cost(left, &mut outputs);
        for num_colors in 1..=num_colors_max {
            if let Some(output) = &outputs[num_colors] {
                if output.cost() < min_cost {
                    min_cost = output.cost();
                    best_num_colors = num_colors;
                }
            }
        }
        outputs[best_num_colors].clone().unwrap()
    }
}

#[derive(Debug, Clone)]
struct Grid {
    n: usize,
    edges: Vec<[usize; 4]>,
    targets: Vec<(usize, usize)>,
    distances: Vec<Vec<i32>>,
}

impl Grid {
    fn new(input: &Input) -> Self {
        let edges = Self::build_edges(input.n, &input.v, &input.h);
        let mut ret = Self {
            n: input.n,
            edges,
            targets: input.targets.clone(),
            distances: vec![vec![]; input.n * input.n],
        };
        for x in 0..input.n {
            for y in 0..input.n {
                ret.distances[x * input.n + y] = ret.bfs(x, y);
            }
        }
        ret
    }

    fn build_edges(n: usize, v: &Vec<u32>, h: &Vec<u32>) -> Vec<[usize; 4]> {
        let mut edges = vec![[!0; 4]; n * n];
        for ux in 0..n {
            for uy in 0..n {
                let u = ux * n + uy;
                // up
                if h[ux] & (1 << uy) == 0 {
                    let vx = ux - 1;
                    let vy = uy;
                    edges[u][0] = vx * n + vy;
                }
                // down
                if h[ux + 1] & (1 << uy) == 0 {
                    let vx = ux + 1;
                    let vy = uy;
                    edges[u][1] = vx * n + vy;
                }
                // left
                if v[ux] & (1 << uy) == 0 {
                    let vx = ux;
                    let vy = uy - 1;
                    edges[u][2] = vx * n + vy;
                }
                // right
                if v[ux] & (1 << (uy + 1)) == 0 {
                    let vx = ux;
                    let vy = uy + 1;
                    edges[u][3] = vx * n + vy;
                }
            }
        }
        edges
    }

    fn bfs(&self, sx: usize, sy: usize) -> Vec<i32> {
        let mut dist = vec![!0; self.n * self.n];
        let s = sx * self.n + sy;
        dist[s] = 0;
        let mut que = VecDeque::new();
        que.push_back(s);
        while let Some(u) = que.pop_front() {
            for &v in &self.edges[u] {
                if v != !0 && dist[v] == !0 {
                    dist[v] = dist[u] + 1;
                    que.push_back(v);
                }
            }
        }
        dist
    }

    /// Calculate shortest route that minimizes the number of squares at which the robot turns.
    fn calc_shortest_route(&self, rngs: &mut RandomGenerators) -> Vec<(usize, usize)> {
        let mut is_waypoint = vec![false; self.n * self.n];
        let (mut sx, mut sy) = self.targets[self.targets.len() - 1];
        let mut ret = vec![(sx, sy)]; // only waypoints
        let mut ds = !0;
        let mut dp = vec![[(!0u32, !0); 4]; self.n * self.n]; // (turns, direction)
        for target_id in (0..self.targets.len() - 1).rev() {
            let (tx, ty) = self.targets[target_id];
            // shortest path from (sx, sy) to (tx, ty)
            let s = sx * self.n + sy;
            let t = tx * self.n + ty;
            let mut que = vec![(s, ds)];
            dp[s] = [(0, 4); 4];
            let mut i = 0;
            while i < que.len() {
                let (u, du) = que[i];
                let cost = if u == s { 0 } else { dp[u][du].0 };
                i += 1;
                for dv in 0..4 {
                    let v = self.edges[u][dv];
                    if v == !0 {
                        continue;
                    }
                    if self.distances[t][v] > self.distances[t][u] - 1 {
                        continue;
                    }
                    const WAYPOINT_STRAIGHT_COST: u32 = 0;
                    const WAYPOINT_TURN_COST: u32 = 4;
                    const MAKE_WAYPOINT_COST: u32 = 4;
                    let mut ncost = cost
                        + if is_waypoint[v] {
                            if du == dv {
                                WAYPOINT_STRAIGHT_COST
                            } else {
                                WAYPOINT_TURN_COST
                            }
                        } else {
                            if du == dv {
                                0
                            } else {
                                MAKE_WAYPOINT_COST
                            }
                        };
                    if rngs.rngs[target_id].gen_u32() % 32 == 0 {
                        ncost = cost;
                    }
                    if ncost < dp[v][dv].0
                        || (ncost == dp[v][dv].0 && rngs.rngs[target_id].gen_u32() % 2 == 0)
                    {
                        dp[v][dv].0 = ncost;
                        if dp[v][dv].1 == !0 {
                            que.push((v, dv));
                        }
                        dp[v][dv].1 = du;
                    }
                }
            }
            let mut dt = !0;
            let mut cost = (u32::MAX, u32::MAX);
            for d in 0..4 {
                if dp[t][d].0 == !0 {
                    continue;
                }
                let next_turn_cost = if target_id + 1 == self.targets.len() {
                    0
                } else if self.edges[t][d] == !0 {
                    1
                } else {
                    let u = self.edges[t][d];
                    let (vx, vy) = self.targets[target_id + 1];
                    let v = vx * self.n + vy;
                    if self.distances[u][v] <= self.distances[u][t] - 1 {
                        0
                    } else {
                        1
                    }
                };
                let ncost = (dp[t][d].0, next_turn_cost);
                if ncost < cost || (ncost == cost && rngs.rngs[target_id].gen_u32() % 2 == 0) {
                    cost = ncost;
                    dt = d;
                }
            }
            debug_assert_ne!(dt, !0);
            // reconstruct path
            let mut path = vec![];
            let mut v = t;
            let mut dv = dt;
            while v != s {
                let u = self.edges[v][dv ^ 1];
                let du = dp[v][dv].1;
                if du != dv && !(u == s && ds == !0) {
                    is_waypoint[u] = true;
                    path.push((u / self.n, u % self.n));
                }
                v = u;
                dv = du;
            }
            path.reverse();
            ret.extend(path);
            sx = tx;
            sy = ty;
            ds = dt;
            // reset dp
            for &(v, dv) in &que {
                if dv < 4 {
                    dp[v][dv] = (!0, !0);
                }
            }
            dp[s] = [(!0, !0); 4];
        }
        if ret[ret.len() - 1] != self.targets[0] {
            ret.push(self.targets[0]);
        }
        ret.reverse();
        ret
    }
}

#[derive(Debug, Clone)]
struct RandomGenerators {
    rngs: Vec<XorShift32>,
    seeds: Vec<u32>,
}

impl RandomGenerators {
    fn new(n: usize) -> Self {
        let seeds = vec![1; n];
        let mut rngs = vec![];
        for i in 0..n {
            rngs.push(XorShift32::new(seeds[i]));
        }
        Self { rngs, seeds }
    }

    fn reset(&mut self) {
        for i in 0..self.rngs.len() {
            self.rngs[i] = XorShift32::new(self.seeds[i]);
        }
    }

    fn change_seeds(&mut self, rng: &mut XorShift32, num_changes: usize) {
        for _ in 0..num_changes {
            let i = rng.gen_range(0, self.rngs.len());
            let nseed = rng.gen_u32();
            self.seeds[i] = nseed;
            self.rngs[i] = XorShift32::new(nseed);
        }
    }
}

#[derive(Debug, Clone)]
struct RouteOptimizer {
    n: usize,
    v: Vec<u32>,
    h: Vec<u32>,
    h_trans: Vec<u32>, // horizontal walls transposed
    route: Vec<(usize, usize)>,
    vertex_targets: Vec<Vec<(usize, usize)>>,
    edge_targets: Vec<Vec<(usize, usize)>>,
    visits: Vec<Vec<usize>>, // [x * n + y] = {route id that visits (x, y)}
    to_route_id: Vec<Vec<usize>>, // [x * n + y] = {route id where the robot turns left/right/back at (x, y)}
    surplus_len: usize,           // t - route_length
}

impl RouteOptimizer {
    fn new(input: &Input, route: Vec<(usize, usize)>) -> Self {
        let n = input.n;
        let t = input.t;

        let mut h_trans = vec![0; n];
        for x in 0..=n {
            for y in 0..n {
                if input.h[x] & (1 << y) != 0 {
                    h_trans[y] |= 1 << x;
                }
            }
        }

        let mut target_id = 1;
        let mut vertex_targets = vec![vec![]; route.len()];
        vertex_targets[0].push(input.targets[0]);
        let mut edge_targets = vec![vec![]; route.len() - 1];
        for route_id in 0..route.len() - 1 {
            let (ux, uy) = route[route_id];
            let (vx, vy) = route[route_id + 1];
            let (dx, dy) = DXDY[get_direction(ux, uy, vx, vy)];
            let mut x = ux;
            let mut y = uy;
            while (x, y) != (vx, vy) {
                if (x, y) == input.targets[target_id] {
                    edge_targets[route_id].push((x, y));
                    target_id += 1;
                }
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
            }
            if (vx, vy) == input.targets[target_id] {
                vertex_targets[route_id + 1].push((vx, vy));
                target_id += 1;
            }
        }
        debug_assert_eq!(target_id, input.targets.len());

        let mut visits = vec![vec![]; n * n];
        let mut to_route_id = vec![vec![]; n * n];
        let mut surplus_len = t;
        visits[route[0].0 * n + route[0].1].push(0);
        for route_id in 0..route.len() - 1 {
            let (ux, uy) = route[route_id];
            let (vx, vy) = route[route_id + 1];
            let (dx, dy) = DXDY[get_direction(ux, uy, vx, vy)];
            let mut x = ux;
            let mut y = uy;
            while (x, y) != (vx, vy) {
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
                visits[x * n + y].push(route_id);
            }
            if route_id + 1 < route.len() - 1 {
                to_route_id[vx * n + vy].push(route_id + 1);
            }
            surplus_len -= manhattan(ux, uy, vx, vy);
        }
        Self {
            n,
            v: input.v.clone(),
            h: input.h.clone(),
            h_trans,
            route,
            vertex_targets,
            edge_targets,
            visits,
            to_route_id,
            surplus_len,
        }
    }

    fn optimize_route(&mut self) {
        let mut candidates = BinaryHeap::new();
        let mut try_to_kick = false;
        loop {
            for route_id in 1..self.route.len() - 1 {
                let (profit, extend_len) = self.calc_extend1_profit(route_id, try_to_kick);
                if extend_len > 0 {
                    candidates.push((OrderedFloat(profit), route_id, 0));
                }
                let (profit, extend_len) = self.calc_extend2_profit(route_id, try_to_kick);
                if extend_len > 0 {
                    candidates.push((OrderedFloat(profit), route_id, 1));
                }
                let (profit, shift_len) = self.calc_parallel_shift_profit(route_id);
                if shift_len > 0 {
                    candidates.push((OrderedFloat(profit), route_id, 2));
                }
                let profit = self.calc_switch_turn_position_profit(route_id);
                if profit > 0.0 {
                    candidates.push((OrderedFloat(profit), route_id, 3));
                }
            }
            if candidates.is_empty() {
                if try_to_kick {
                    break;
                }
                try_to_kick = true;
                continue;
            }
            while let Some((profit, route_id, extend_type)) = candidates.pop() {
                if extend_type == 0 {
                    // check again
                    let (nprofit, new_extend_len) = self.calc_extend1_profit(route_id, try_to_kick);
                    if nprofit < profit.0 || new_extend_len == 0 {
                        if new_extend_len > 0 {
                            candidates.push((OrderedFloat(nprofit), route_id, 0));
                        }
                        continue;
                    }
                    // accept
                    self.extend1(route_id, new_extend_len);

                    let (profit, extend_len) = self.calc_extend1_profit(route_id, try_to_kick);
                    if extend_len > 0 {
                        candidates.push((OrderedFloat(profit), route_id, 0));
                    }
                } else if extend_type == 1 {
                    // check again
                    let (nprofit, new_extend_len) = self.calc_extend2_profit(route_id, try_to_kick);
                    if nprofit < profit.0 || new_extend_len == 0 {
                        if new_extend_len > 0 {
                            candidates.push((OrderedFloat(nprofit), route_id, 1));
                        }
                        continue;
                    }
                    // accept
                    self.extend2(route_id, new_extend_len);

                    let (profit, extend_len) = self.calc_extend2_profit(route_id, try_to_kick);
                    if extend_len > 0 {
                        candidates.push((OrderedFloat(profit), route_id, 1));
                    }
                } else if extend_type == 2 {
                    // check again
                    let (nprofit, new_shift_len) = self.calc_parallel_shift_profit(route_id);
                    if nprofit < profit.0 || new_shift_len == 0 {
                        if new_shift_len > 0 {
                            candidates.push((OrderedFloat(nprofit), route_id, 2));
                        }
                        continue;
                    }
                    // accept
                    self.parallel_shift(route_id, new_shift_len);

                    let (profit, shift_len) = self.calc_parallel_shift_profit(route_id);
                    if shift_len > 0 {
                        candidates.push((OrderedFloat(profit), route_id, 2));
                    }
                } else if extend_type == 3 {
                    // check again
                    let nprofit = self.calc_switch_turn_position_profit(route_id);
                    if nprofit < profit.0 {
                        if nprofit > 0.0 {
                            candidates.push((OrderedFloat(nprofit), route_id, 3));
                        }
                        continue;
                    }
                    // accept
                    self.switch_turn_position(route_id);

                    let nprofit = self.calc_switch_turn_position_profit(route_id);
                    if nprofit > 0.0 {
                        candidates.push((OrderedFloat(nprofit), route_id, 3));
                    }
                } else {
                    unreachable!();
                }
            }
        }
    }

    fn can_extend_route(&self, route_id: usize, try_to_kick: bool) -> bool {
        if self.n >= 16 {
            route_id < EXTEND_ROUTE_ID_MIN && !try_to_kick
        } else {
            route_id < EXTEND_ROUTE_ID_MIN
        }
    }

    fn extend1(&mut self, route_id: usize, extend_len: usize) {
        debug_assert!(self.can_extend1(route_id));
        let (ux, uy) = self.route[route_id - 1];
        let (vx, vy) = self.route[route_id];
        let dir = get_direction(ux, uy, vx, vy);
        let (dx, dy) = DXDY[dir];
        let mut x = vx;
        let mut y = vy;
        self.remove_turn(route_id, vx, vy);
        for _ in 0..extend_len {
            self.insert_visit(route_id - 1, x, y);
            self.insert_visit(route_id, x, y);
            debug_assert!(self.can_move(x, y, dir));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
        }
        self.insert_turn(route_id, x, y);
        self.route[route_id] = (x, y);
        debug_assert!(self.surplus_len >= 2 * extend_len);
        self.surplus_len -= 2 * extend_len;
        self.edge_targets[route_id - 1].extend(self.vertex_targets[route_id].drain(..));
    }

    /// return (profit per length, extending length)
    fn calc_extend1_profit(&self, route_id: usize, try_to_kick: bool) -> (f64, usize) {
        if route_id == 0 || self.can_extend_route(route_id, try_to_kick) || route_id >= self.route.len() - 1 {
            return (f64::MIN, 0);
        }
        let (ux, uy) = self.route[route_id - 1];
        let (vx, vy) = self.route[route_id];
        let (wx, wy) = self.route[route_id + 1];
        let dir_fr = get_direction(ux, uy, vx, vy);
        let dir_to = get_direction(vx, vy, wx, wy);
        if dir_fr ^ 1 != dir_to {
            return (f64::MIN, 0);
        }
        let mut profit = self.calc_waypoint_cost(route_id, vx, vy);
        if profit < 0 || (profit == 0 && !try_to_kick) {
            return (0.0, 0);
        }
        let (dx, dy) = DXDY[dir_fr];
        let mut x = vx;
        let mut y = vy;
        let mut ret = (if try_to_kick { -1e-9 } else { 1e-9 }, 0);
        for extend_len in 1..=self.surplus_len / 2 {
            if self.is_waypoint(route_id, x, y) {
                profit -= 2;
            }
            if profit < 0 || (profit == 0 && !try_to_kick) {
                break;
            }
            if !self.can_move(x, y, dir_fr) {
                break;
            }
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
            let nprofit = profit - self.calc_waypoint_cost(route_id, x, y);
            ret.chmax((nprofit as f64 / (2 * extend_len) as f64, extend_len));
        }
        ret
    }

    fn can_extend1(&self, route_id: usize) -> bool {
        if route_id == 0 || route_id + 1 >= self.route.len() {
            return false;
        }
        let (ux, uy) = self.route[route_id - 1];
        let (vx, vy) = self.route[route_id];
        let (wx, wy) = self.route[route_id + 1];
        let dir_fr = get_direction(ux, uy, vx, vy);
        let dir_to = get_direction(vx, vy, wx, wy);
        dir_fr ^ 1 == dir_to
    }

    fn extend2(&mut self, route_id: usize, extend_len: usize) {
        debug_assert!(self.can_extend2(route_id));
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let dir_fr = get_direction(sx, sy, tx, ty);
        let dir_to = get_direction(tx, ty, ux, uy);

        self.remove_turn(route_id, tx, ty);
        self.remove_turn(route_id + 1, ux, uy);
        let (dx, dy) = DXDY[dir_to];
        let mut x = tx.wrapping_add(dx);
        let mut y = ty.wrapping_add(dy);
        while (x, y) != (ux, uy) {
            self.remove_visit(route_id, x, y);
            debug_assert!(self.can_move(x, y, dir_to));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
        }

        let (dx, dy) = DXDY[dir_fr];
        let mut x = tx;
        let mut y = ty;
        for _ in 0..extend_len {
            self.insert_visit(route_id - 1, x, y);
            debug_assert!(self.can_move(x, y, dir_fr));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
        }
        self.insert_turn(route_id, x, y);
        self.route[route_id] = (x, y);

        let (dx, dy) = DXDY[dir_to];
        debug_assert!(self.can_move(x, y, dir_to));
        x = x.wrapping_add(dx);
        y = y.wrapping_add(dy);
        for _ in 0..manhattan(tx, ty, ux, uy) - 1 {
            self.insert_visit(route_id, x, y);
            debug_assert!(self.can_move(x, y, dir_to));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
        }
        self.insert_turn(route_id + 1, x, y);
        self.route[route_id + 1] = (x, y);

        let (dx, dy) = DXDY[dir_fr ^ 1];
        for _ in 0..extend_len {
            debug_assert!(self.can_move(x, y, dir_fr ^ 1));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
            self.insert_visit(route_id + 1, x, y);
        }

        debug_assert!(self.surplus_len >= 2 * extend_len);
        self.surplus_len -= 2 * extend_len;
        self.edge_targets[route_id - 1].extend(self.vertex_targets[route_id].drain(..));
        self.edge_targets[route_id + 1].extend(self.vertex_targets[route_id + 1].drain(..));
    }

    fn calc_extend2_profit(&self, route_id: usize, try_to_kick: bool) -> (f64, usize) {
        if route_id == 0 || self.can_extend_route(route_id, try_to_kick) || route_id + 2 >= self.route.len() {
            return (f64::MIN, 0);
        }
        if !self.edge_targets[route_id].is_empty() {
            return (f64::MIN, 0);
        }
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let (vx, vy) = self.route[route_id + 2];
        let dir_st = get_direction(sx, sy, tx, ty);
        let dir_tu = get_direction(tx, ty, ux, uy);
        let dir_uv = get_direction(ux, uy, vx, vy);
        if !(dir_st ^ 1 == dir_uv && dir_st / 2 != dir_tu / 2) {
            return (f64::MIN, 0);
        }
        let mut profit = self.calc_waypoint_cost(route_id, tx, ty)
            + self.calc_waypoint_cost(route_id + 1, ux, uy);
        let (dx_tu, dy_tu) = DXDY[dir_tu];
        let mut x = tx.wrapping_add(dx_tu);
        let mut y = ty.wrapping_add(dy_tu);
        while (x, y) != (ux, uy) {
            if self.is_waypoint(route_id, x, y) {
                profit += 1;
            }
            x = x.wrapping_add(dx_tu);
            y = y.wrapping_add(dy_tu);
        }
        if profit < 0 || (profit == 0 && !try_to_kick) {
            return (0.0, 0);
        }
        let (dx_st, dy_st) = DXDY[dir_st];
        let dist_tu = manhattan(tx, ty, ux, uy);
        let mut ret = (if try_to_kick { -1e-9 } else { 1e-9 }, 0);
        let mut ntx = tx;
        let mut nty = ty;
        let mut nux = ux;
        let mut nuy = uy;
        for extend_len in 1..=self.surplus_len / 2 {
            if self.is_waypoint(route_id, ntx, nty) {
                profit -= 1;
            }
            if self.is_waypoint(route_id + 1, nux, nuy) {
                profit -= 1;
            }
            if profit < 0 || (profit == 0 && !try_to_kick) {
                break;
            }
            if !self.can_move(ntx, nty, dir_st) || !self.can_move(nux, nuy, dir_st) {
                break;
            }
            ntx = ntx.wrapping_add(dx_st);
            nty = nty.wrapping_add(dy_st);
            nux = nux.wrapping_add(dx_st);
            nuy = nuy.wrapping_add(dy_st);
            let mut nprofit = profit
                - self.calc_waypoint_cost(route_id, ntx, nty)
                - self.calc_waypoint_cost(route_id + 1, nux, nuy);
            if !self.can_move_straight(ntx, nty, dir_tu, dist_tu) {
                break; // continue;
            }
            let mut x = ntx.wrapping_add(dx_tu);
            let mut y = nty.wrapping_add(dy_tu);
            while (x, y) != (nux, nuy) {
                if self.is_waypoint(route_id, x, y) {
                    nprofit -= 1;
                }
                x = x.wrapping_add(dx_tu);
                y = y.wrapping_add(dy_tu);
            }
            ret.chmax((nprofit as f64 / (2 * extend_len) as f64, extend_len));
        }
        ret
    }

    fn can_extend2(&self, route_id: usize) -> bool {
        if route_id == 0 || route_id + 2 >= self.route.len() {
            return false;
        }
        if !self.edge_targets[route_id].is_empty() {
            return false;
        }
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let (vx, vy) = self.route[route_id + 2];
        let dir_st = get_direction(sx, sy, tx, ty);
        let dir_tu = get_direction(tx, ty, ux, uy);
        let dir_uv = get_direction(ux, uy, vx, vy);
        dir_st ^ 1 == dir_uv && dir_st / 2 != dir_tu / 2
    }

    fn parallel_shift(&mut self, route_id: usize, shift_len: isize) {
        debug_assert!(self.can_parallel_shift(route_id));
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let dir_fr = get_direction(sx, sy, tx, ty);
        let dir_to = get_direction(tx, ty, ux, uy);

        self.remove_turn(route_id, tx, ty);
        self.remove_turn(route_id + 1, ux, uy);
        let (dx, dy) = DXDY[dir_to];
        let mut x = tx.wrapping_add(dx);
        let mut y = ty.wrapping_add(dy);
        while (x, y) != (ux, uy) {
            self.remove_visit(route_id, x, y);
            debug_assert!(self.can_move(x, y, dir_to));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
        }

        if shift_len < 0 {
            let (dx, dy) = DXDY[dir_fr ^ 1];
            let mut x = tx;
            let mut y = ty;
            for _ in 0..(-shift_len) {
                debug_assert!(self.can_move(x, y, dir_fr ^ 1));
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
                self.remove_visit(route_id - 1, x, y);
            }
            self.insert_turn(route_id, x, y);
            self.route[route_id] = (x, y);

            if let Some(i) = self.edge_targets[route_id - 1]
                .iter()
                .position(|&p| p == (x, y))
            {
                self.edge_targets[route_id - 1].remove(i);
                self.vertex_targets[route_id].push((x, y));
            }

            let (dx, dy) = DXDY[dir_to];
            debug_assert!(self.can_move(x, y, dir_to));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
            for _ in 0..manhattan(tx, ty, ux, uy) - 1 {
                self.insert_visit(route_id, x, y);
                debug_assert!(self.can_move(x, y, dir_to));
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
            }
            self.insert_turn(route_id + 1, x, y);
            self.route[route_id + 1] = (x, y);

            let (dx, dy) = DXDY[dir_fr];
            for _ in 0..(-shift_len) {
                debug_assert!(self.can_move(x, y, dir_fr));
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
                self.insert_visit(route_id + 1, x, y);
            }
            debug_assert_eq!((x, y), (ux, uy));

            self.edge_targets[route_id + 1].extend(self.vertex_targets[route_id + 1].drain(..));
        } else {
            let (dx, dy) = DXDY[dir_fr];
            let mut x = ux;
            let mut y = uy;
            for _ in 0..shift_len {
                debug_assert!(self.can_move(x, y, dir_fr));
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
                self.remove_visit(route_id + 1, x, y);
            }

            if let Some(i) = self.edge_targets[route_id + 1]
                .iter()
                .position(|&p| p == (x, y))
            {
                self.edge_targets[route_id + 1].remove(i);
                self.vertex_targets[route_id + 1].push((x, y));
            }

            let (dx, dy) = DXDY[dir_fr];
            let mut x = tx;
            let mut y = ty;
            for _ in 0..shift_len {
                self.insert_visit(route_id - 1, x, y);
                debug_assert!(self.can_move(x, y, dir_fr));
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
            }
            self.insert_turn(route_id, x, y);
            self.route[route_id] = (x, y);

            let (dx, dy) = DXDY[dir_to];
            debug_assert!(self.can_move(x, y, dir_to));
            x = x.wrapping_add(dx);
            y = y.wrapping_add(dy);
            for _ in 0..manhattan(tx, ty, ux, uy) - 1 {
                self.insert_visit(route_id, x, y);
                debug_assert!(self.can_move(x, y, dir_to));
                x = x.wrapping_add(dx);
                y = y.wrapping_add(dy);
            }
            self.insert_turn(route_id + 1, x, y);
            self.route[route_id + 1] = (x, y);

            self.edge_targets[route_id - 1].extend(self.vertex_targets[route_id].drain(..));
        }
    }

    fn calc_parallel_shift_profit(&self, route_id: usize) -> (f64, isize) {
        if route_id == 0 || route_id + 2 >= self.route.len() {
            return (f64::MIN, 0);
        }
        if !self.edge_targets[route_id].is_empty() {
            return (f64::MIN, 0);
        }
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let (vx, vy) = self.route[route_id + 2];
        let dir_st = get_direction(sx, sy, tx, ty);
        let dir_tu = get_direction(tx, ty, ux, uy);
        let dir_uv = get_direction(ux, uy, vx, vy);
        if !(dir_st == dir_uv && dir_st / 2 != dir_tu / 2) {
            return (f64::MIN, 0);
        }
        let mut profit = self.calc_waypoint_cost(route_id, tx, ty)
            + self.calc_waypoint_cost(route_id + 1, ux, uy);
        let (dx_tu, dy_tu) = DXDY[dir_tu];
        let mut x = tx.wrapping_add(dx_tu);
        let mut y = ty.wrapping_add(dy_tu);
        while (x, y) != (ux, uy) {
            if self.is_waypoint(route_id, x, y) {
                profit += 1;
            }
            x = x.wrapping_add(dx_tu);
            y = y.wrapping_add(dy_tu);
        }
        let profit_first = profit;
        let mut ret = (1e-9, 0);
        let (dx_st, dy_st) = DXDY[dir_st];
        let dist_tu = manhattan(tx, ty, ux, uy);

        // backward
        let mut ntx = tx;
        let mut nty = ty;
        let mut nux = ux;
        let mut nuy = uy;
        for shift_len in 1.. {
            if self.is_waypoint(route_id + 1, nux, nuy) {
                profit -= 1;
            }
            if !self.can_move(nux, nuy, dir_st ^ 1) {
                break;
            }
            if self.vertex_targets[route_id].contains(&(ntx, nty)) {
                break;
            }
            if self.edge_targets[route_id - 1].contains(&(ntx, nty)) {
                break;
            }
            ntx = ntx.wrapping_sub(dx_st);
            nty = nty.wrapping_sub(dy_st);
            if (ntx, nty) == (sx, sy) {
                break;
            }
            nux = nux.wrapping_sub(dx_st);
            nuy = nuy.wrapping_sub(dy_st);
            if self.is_waypoint(route_id, ntx, nty) {
                profit += 1;
            }
            let mut nprofit = profit
                - self.calc_waypoint_cost(route_id, ntx, nty)
                - self.calc_waypoint_cost(route_id + 1, nux, nuy);
            if !self.can_move_straight(ntx, nty, dir_tu, dist_tu) {
                break; // continue;
            }
            let mut x = ntx.wrapping_add(dx_tu);
            let mut y = nty.wrapping_add(dy_tu);
            while (x, y) != (nux, nuy) {
                if self.is_waypoint(route_id, x, y) {
                    nprofit -= 1;
                }
                x = x.wrapping_add(dx_tu);
                y = y.wrapping_add(dy_tu);
            }
            ret.chmax((10.0 * nprofit as f64, -(shift_len as isize)));
        }

        // forward
        profit = profit_first;
        let mut ntx = tx;
        let mut nty = ty;
        let mut nux = ux;
        let mut nuy = uy;
        for shift_len in 1.. {
            if self.is_waypoint(route_id, ntx, nty) {
                profit -= 1;
            }
            if !self.can_move(ntx, nty, dir_st) {
                break;
            }
            ntx = ntx.wrapping_add(dx_st);
            nty = nty.wrapping_add(dy_st);
            if self.vertex_targets[route_id + 1].contains(&(nux, nuy)) {
                break;
            }
            if self.edge_targets[route_id + 1].contains(&(nux, nuy)) {
                break;
            }
            nux = nux.wrapping_add(dx_st);
            nuy = nuy.wrapping_add(dy_st);
            if (nux, nuy) == (vx, vy) {
                break;
            }
            if self.is_waypoint(route_id + 1, nux, nuy) {
                profit += 1;
            }
            let mut nprofit = profit
                - self.calc_waypoint_cost(route_id, ntx, nty)
                - self.calc_waypoint_cost(route_id + 1, nux, nuy);
            if !self.can_move_straight(ntx, nty, dir_tu, dist_tu) {
                break; // continue;
            }
            let mut x = ntx.wrapping_add(dx_tu);
            let mut y = nty.wrapping_add(dy_tu);
            while (x, y) != (nux, nuy) {
                if self.is_waypoint(route_id, x, y) {
                    nprofit -= 1;
                }
                x = x.wrapping_add(dx_tu);
                y = y.wrapping_add(dy_tu);
            }
            ret.chmax((10.0 * nprofit as f64, shift_len as isize));
        }

        ret
    }

    fn can_parallel_shift(&self, route_id: usize) -> bool {
        if route_id == 0 || route_id + 2 >= self.route.len() {
            return false;
        }
        if !self.edge_targets[route_id].is_empty() {
            return false;
        }
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let (vx, vy) = self.route[route_id + 2];
        let dir_st = get_direction(sx, sy, tx, ty);
        let dir_tu = get_direction(tx, ty, ux, uy);
        let dir_uv = get_direction(ux, uy, vx, vy);
        dir_st == dir_uv && dir_st / 2 != dir_tu / 2
    }

    fn switch_turn_position(&mut self, route_id: usize) {
        debug_assert!(self.can_switch_turn_position(route_id));
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let dir_st = get_direction(sx, sy, tx, ty);
        let dir_tu = get_direction(tx, ty, ux, uy);

        let (dx_st, dy_st) = DXDY[dir_st];
        let mut x = sx.wrapping_add(dx_st);
        let mut y = sy.wrapping_add(dy_st);
        while (x, y) != (tx, ty) {
            self.remove_visit(route_id - 1, x, y);
            debug_assert!(self.can_move(x, y, dir_st));
            x = x.wrapping_add(dx_st);
            y = y.wrapping_add(dy_st);
        }
        self.remove_turn(route_id, tx, ty);

        let (dx_tu, dy_tu) = DXDY[dir_tu];
        let mut x = tx.wrapping_add(dx_tu);
        let mut y = ty.wrapping_add(dy_tu);
        while (x, y) != (ux, uy) {
            self.remove_visit(route_id, x, y);
            debug_assert!(self.can_move(x, y, dir_tu));
            x = x.wrapping_add(dx_tu);
            y = y.wrapping_add(dy_tu);
        }

        let dist_tu = manhattan(tx, ty, ux, uy);
        let mut x = sx.wrapping_add(dx_tu);
        let mut y = sy.wrapping_add(dy_tu);
        for _ in 0..dist_tu - 1 {
            self.insert_visit(route_id - 1, x, y);
            debug_assert!(self.can_move(x, y, dir_tu));
            x = x.wrapping_add(dx_tu);
            y = y.wrapping_add(dy_tu);
        }
        self.insert_turn(route_id, x, y);
        self.route[route_id] = (x, y);

        let mut x = x.wrapping_add(dx_st);
        let mut y = y.wrapping_add(dy_st);
        while (x, y) != (ux, uy) {
            self.insert_visit(route_id, x, y);
            debug_assert!(self.can_move(x, y, dir_st));
            x = x.wrapping_add(dx_st);
            y = y.wrapping_add(dy_st);
        }
    }

    fn calc_switch_turn_position_profit(&self, route_id: usize) -> f64 {
        if !self.can_switch_turn_position(route_id) {
            return f64::MIN;
        }
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let dir_st = get_direction(sx, sy, tx, ty);
        let dir_tu = get_direction(tx, ty, ux, uy);
        let mut profit = 0;

        let (dx_st, dy_st) = DXDY[dir_st];
        let mut x = sx.wrapping_add(dx_st);
        let mut y = sy.wrapping_add(dy_st);
        while (x, y) != (tx, ty) {
            if self.is_waypoint(route_id - 1, x, y) {
                profit += 1;
            }
            debug_assert!(self.can_move(x, y, dir_st));
            x = x.wrapping_add(dx_st);
            y = y.wrapping_add(dy_st);
        }
        profit += self.calc_waypoint_cost(route_id, tx, ty);

        let (dx_tu, dy_tu) = DXDY[dir_tu];
        let mut x = tx.wrapping_add(dx_tu);
        let mut y = ty.wrapping_add(dy_tu);
        while (x, y) != (ux, uy) {
            if self.is_waypoint(route_id, x, y) {
                profit += 1;
            }
            debug_assert!(self.can_move(x, y, dir_tu));
            x = x.wrapping_add(dx_tu);
            y = y.wrapping_add(dy_tu);
        }

        let dist_tu = manhattan(tx, ty, ux, uy);
        let mut x = sx.wrapping_add(dx_tu);
        let mut y = sy.wrapping_add(dy_tu);
        for _ in 0..dist_tu - 1 {
            if self.is_waypoint(route_id - 1, x, y) {
                profit -= 1;
            }
            debug_assert!(self.can_move(x, y, dir_tu));
            x = x.wrapping_add(dx_tu);
            y = y.wrapping_add(dy_tu);
        }
        profit -= self.calc_waypoint_cost(route_id, x, y);

        let mut x = x.wrapping_add(dx_st);
        let mut y = y.wrapping_add(dy_st);
        while (x, y) != (ux, uy) {
            if self.is_waypoint(route_id, x, y) {
                profit -= 1;
            }
            debug_assert!(self.can_move(x, y, dir_st));
            x = x.wrapping_add(dx_st);
            y = y.wrapping_add(dy_st);
        }
        10.0 * profit as f64
    }

    fn can_switch_turn_position(&self, route_id: usize) -> bool {
        if route_id == 0 || route_id + 1 >= self.route.len() {
            return false;
        }
        if !self.vertex_targets[route_id].is_empty() {
            return false;
        }
        if !self.edge_targets[route_id - 1].is_empty() || !self.edge_targets[route_id].is_empty() {
            return false;
        }
        let (sx, sy) = self.route[route_id - 1];
        let (tx, ty) = self.route[route_id];
        let (ux, uy) = self.route[route_id + 1];
        let dir_fr = get_direction(sx, sy, tx, ty);
        let dir_to = get_direction(tx, ty, ux, uy);
        if dir_fr / 2 == dir_to / 2 {
            return false;
        }
        let dist_st = manhattan(sx, sy, tx, ty);
        let dist_tu = manhattan(tx, ty, ux, uy);
        if !self.can_move_straight(sx, sy, dir_to, dist_tu) {
            return false;
        }
        if !self.can_move_straight(ux, uy, dir_fr ^ 1, dist_st) {
            return false;
        }
        true
    }

    fn calc_waypoint_cost(&self, route_id: usize, x: usize, y: usize) -> i32 {
        if let Some(&t) = self.to_route_id[x * self.n + y].last() {
            if t > route_id {
                return 0;
            }
        }
        let mut ret = self.visits[x * self.n + y].partition_point(|&t| t < route_id) as i32;
        let i = self.to_route_id[x * self.n + y].partition_point(|&t| t < route_id);
        if i > 0 {
            let route_id_prev = self.to_route_id[x * self.n + y][i - 1];
            ret -= self.visits[x * self.n + y].partition_point(|&t| t < route_id_prev) as i32;
        }
        ret
    }

    fn is_waypoint(&self, route_id: usize, x: usize, y: usize) -> bool {
        if let Some(&t) = self.to_route_id[x * self.n + y].last() {
            if t > route_id {
                return true;
            }
        }
        false
    }

    fn insert_turn(&mut self, route_id: usize, x: usize, y: usize) {
        let i = self.to_route_id[x * self.n + y].partition_point(|&t| t < route_id);
        self.to_route_id[x * self.n + y].insert(i, route_id);
    }

    fn remove_turn(&mut self, route_id: usize, x: usize, y: usize) {
        let i = self.to_route_id[x * self.n + y].partition_point(|&t| t < route_id);
        debug_assert_eq!(self.to_route_id[x * self.n + y][i], route_id);
        self.to_route_id[x * self.n + y].remove(i);
    }

    fn insert_visit(&mut self, route_id: usize, x: usize, y: usize) {
        let i = self.visits[x * self.n + y].partition_point(|&t| t < route_id);
        self.visits[x * self.n + y].insert(i, route_id);
    }

    fn remove_visit(&mut self, route_id: usize, x: usize, y: usize) {
        let i = self.visits[x * self.n + y].partition_point(|&t| t < route_id);
        debug_assert_eq!(self.visits[x * self.n + y][i], route_id);
        self.visits[x * self.n + y].remove(i);
    }

    fn can_move(&self, x: usize, y: usize, dir: usize) -> bool {
        match dir {
            0 => {
                // up
                self.h[x] & (1 << y) == 0
            }
            1 => {
                // down
                self.h[x + 1] & (1 << y) == 0
            }
            2 => {
                // left
                self.v[x] & (1 << y) == 0
            }
            3 => {
                // right
                self.v[x] & (1 << (y + 1)) == 0
            }
            _ => unreachable!(),
        }
    }

    fn can_move_straight(&self, x: usize, y: usize, dir: usize, len: usize) -> bool {
        debug_assert!(len >= 1);
        match dir {
            0 => {
                // up
                if x < len {
                    return false;
                }
                let x = x - len;
                let mut bits = self.h_trans[y];
                bits >>= x + 1;
                bits &= (1 << len) - 1;
                bits == 0
            }
            1 => {
                // down
                let mut bits = self.h_trans[y];
                bits >>= x + 1;
                bits &= (1 << len) - 1;
                bits == 0
            }
            2 => {
                // left
                if y < len {
                    return false;
                }
                let y = y - len;
                let mut bits = self.v[x];
                bits >>= y + 1;
                bits &= (1 << len) - 1;
                bits == 0
            }
            3 => {
                // right
                let mut bits = self.v[x];
                bits >>= y + 1;
                bits &= (1 << len) - 1;
                bits == 0
            }
            _ => unreachable!(),
        }
    }
}

#[inline(always)]
fn get_direction(sx: usize, sy: usize, tx: usize, ty: usize) -> usize {
    if sy == ty {
        if sx > tx {
            0 // up
        } else {
            debug_assert!(sx < tx);
            1 // down
        }
    } else {
        debug_assert_eq!(sx, tx);
        if sy > ty {
            2 // left
        } else {
            debug_assert!(sy < ty);
            3 // right
        }
    }
}

#[inline(always)]
fn manhattan(ux: usize, uy: usize, vx: usize, vy: usize) -> usize {
    ux.abs_diff(vx) + uy.abs_diff(vy)
}

#[derive(Debug, Clone)]
struct Input {
    n: usize,
    t: usize,
    v: Vec<u32>,
    h: Vec<u32>,
    targets: Vec<(usize, usize)>,
}

fn read_input() -> Input {
    input! {
        n: usize,
        k: usize,
        t: usize,
        vs: [String; n],
        hs: [String; n - 1],
        targets: [(usize, usize); k],
    }
    assert!(10 <= n && n <= 20);
    assert!(n <= k && k <= n * n);

    let mut v = vec![0; n];
    for x in 0..n {
        v[x] |= 1 << 0;
        v[x] |= 1 << n;
        let row = vs[x].chars().collect_vec();
        debug_assert_eq!(row.len(), n - 1);
        for y in 1..n {
            if row[y - 1] == '1' {
                v[x] |= 1 << y;
            }
        }
    }
    let mut h = vec![0; n + 1];
    h[0] = (1 << n) - 1;
    h[n] = (1 << n) - 1;
    for x in 1..n {
        let row = hs[x - 1].chars().collect_vec();
        debug_assert_eq!(row.len(), n);
        for y in 0..n {
            if row[y] == '1' {
                h[x] |= 1 << y;
            }
        }
    }
    Input {
        n,
        t,
        v,
        h,
        targets,
    }
}

#[derive(Debug, Clone)]
struct TransitionRule {
    c: usize,
    q: usize,
    a: usize,
    s: usize,
    d: usize,
}

#[derive(Debug, Clone)]
struct Output {
    n: usize,
    c: usize,
    q: usize,
    s: Vec<usize>,
    rules: Vec<TransitionRule>,
}

impl Output {
    fn cost(&self) -> usize {
        self.c + self.q
    }

    fn swap_state(&mut self, p: usize, q: usize) {
        if p == q {
            return;
        }
        for rule in &mut self.rules {
            if rule.q == p {
                rule.q = q;
            } else if rule.q == q {
                rule.q = p;
            }
            if rule.s == p {
                rule.s = q;
            } else if rule.s == q {
                rule.s = p;
            }
        }
    }

    fn print(&self) {
        println!("{} {} {}", self.c, self.q, self.rules.len());
        for x in 0..self.n {
            let row = self.s[x * self.n..(x + 1) * self.n]
                .iter()
                .map(|&c| c.to_string())
                .collect_vec()
                .join(" ");
            println!("{}", row);
        }
        for rule in &self.rules {
            println!(
                "{} {} {} {} {}",
                rule.c, rule.q, rule.a, rule.s, UDLR[rule.d]
            );
        }
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

    pub fn extend(&mut self, additional: usize) {
        for _ in 0..additional {
            self.parent_or_size.push(-1);
        }
        self.n += additional;
    }

    pub fn merge(&mut self, a: usize, b: usize) -> usize {
        debug_assert!(a < self.n);
        debug_assert!(b < self.n);
        let mut x = self.leader(a);
        let mut y = self.leader(b);
        if x == y {
            return x;
        }
        if -self.parent_or_size[x] < -self.parent_or_size[y] {
            std::mem::swap(&mut x, &mut y);
        }
        self.parent_or_size[x] += self.parent_or_size[y];
        self.parent_or_size[y] = x as i32;
        x
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
}

#[derive(Debug, Clone, Copy)]
pub struct XorShift32 {
    state: u32,
}

impl XorShift32 {
    pub fn new(mut seed: u32) -> Self {
        if seed == 0 {
            seed = u32::MAX;
        }
        Self { state: seed }
    }

    /// 1..=u32::MAX
    #[inline(always)]
    pub fn gen_u32(&mut self) -> u32 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.state = x;
        x
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f32(&mut self) -> f32 {
        self.gen_u32() as f32 / ((1u64 << 32) as f32)
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f64(&mut self) -> f64 {
        self.gen_u32() as f64 / ((1u64 << 32) as f64)
    }

    /// l..r
    #[inline(always)]
    pub fn gen_range(&mut self, l: usize, r: usize) -> usize {
        debug_assert!(l < r);
        debug_assert!(r as u64 <= 1 << 32);
        l + (((r - l) as u64 * self.gen_u32() as u64) >> 32) as usize
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f32(&mut self, l: f32, r: f32) -> f32 {
        debug_assert!(l <= r);
        l + self.gen_u32() as f32 * ((r - l) / ((1u64 << 32) as f32))
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f64(&mut self, l: f64, r: f64) -> f64 {
        debug_assert!(l <= r);
        l + self.gen_u32() as f64 * ((r - l) / ((1u64 << 32) as f64))
    }

    #[inline(always)]
    pub fn shuffle(&mut self, v: &mut [usize]) {
        let n = v.len();
        for i in (1..n).rev() {
            let j = self.gen_range(0, i + 1);
            v.swap(i, j);
        }
    }

    #[inline(always)]
    pub fn partial_shuffle(&mut self, v: &mut [usize], n: usize) {
        let m = v.len();
        debug_assert!(n <= m);
        for i in 0..n {
            let j = self.gen_range(i, m);
            v.swap(i, j);
        }
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
        ms - STIME
    }
}
