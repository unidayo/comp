use std::process::exit;

use proconio::input;

const TIME_LIMIT_SEC: f64 = 1.95;
const N: usize = 20;

const SCHED_TYPE: SchedulerType = SchedulerType::Exp;
const T0: f64 = 2.0;
const T1: f64 = 0.1;
const PENALTY0: f64 = 2.0;
const PENALTY1: f64 = 10.0;
const NUM_GROUPS: usize = 100;

fn main() {
    get_time_sec();

    let input = read_input();

    let mut state = State::new(&input, 1);
    let mut best_state = state.clone();
    let mut sched = AnnealingScheduler::new(SCHED_TYPE.clone(), T0, T1, TIME_LIMIT_SEC);

    while get_time_sec() < TIME_LIMIT_SEC {
        for _ in 0..SA_TIME_COUNTS {
            if !state.update(&input, &mut sched) {
                continue; // No change
            }
            if state.total_penalties > 0 {
                continue; // Skip if there are penalties
            }
            if state.total_turns < best_state.total_turns {
                best_state.clone_from(&state);
                // dbg!(best_state.total_turns, sched.progress);
            }
        }
    }

    // dbg!(best_state.total_turns, best_state.total_penalties);
    best_state.print();

    // sched.print_log();

    exit(0);
}

#[derive(Debug, Clone)]
struct State {
    rng: XorShift32,
    groups: Vec<Group>,
    turns: Vec<i32>,
    penalties: Vec<i32>,
    total_turns: i32,
    total_penalties: i32,
}

impl State {
    fn new(input: &Input, seed: u32) -> Self {
        let mut argsorted_points = Vec::new();
        for x in 0..N {
            let sy = if x == 0 { 1 } else { 0 };
            for y in sy..N {
                argsorted_points.push((-input[x][y].w, x, y));
            }
        }
        argsorted_points.sort_unstable();

        let mut groups = vec![Group::new(); NUM_GROUPS];
        let mut cnt = 0;
        let mut group_size = 0;
        for &(_, x, y) in &argsorted_points {
            let mut best_g = cnt;
            let mut min_dist = usize::MAX;
            if group_size > 0 {
                for g in 0..NUM_GROUPS {
                    if groups[g].points.len() > group_size {
                        continue; // Skip full groups
                    }
                    let &(px, py) = groups[g].points.last().unwrap();
                    let dist = px.abs_diff(x) + py.abs_diff(y);
                    if min_dist.chmin(dist) {
                        best_g = g;
                    }
                }
            }
            groups[best_g].points.push((x, y));

            cnt += 1;
            if cnt >= NUM_GROUPS {
                cnt = 0; // wrap around
                group_size += 1;
            }
        }

        let mut turns = vec![];
        let mut penalties = vec![];
        for group in &groups {
            let (t, p) = group.evaluate(&input);
            turns.push(t);
            penalties.push(p);
        }
        let total_turns = turns.iter().sum();
        let total_penalties = penalties.iter().sum();

        Self {
            rng: XorShift32::new(seed),
            groups,
            turns,
            penalties,
            total_turns,
            total_penalties,
        }
    }

    fn accept(dt: i32, dp: i32, sched: &mut AnnealingScheduler) -> bool {
        let prog = sched.progress;
        let penalty_tmp = PENALTY0 * (1.0 - prog) + PENALTY1 * prog;
        let profit = -(dt as f64 + dp as f64 * penalty_tmp);
        sched.accept(profit)
    }

    fn update(&mut self, input: &Input, sched: &mut AnnealingScheduler) -> bool {
        const SWAP_IN_GROUP: usize = 50;
        const SWAP_PART_OF_TWO_GROUPS: usize = 400 + SWAP_IN_GROUP;
        const MOVE_BOX_TO_OTHER_GROUP: usize = 30 + SWAP_PART_OF_TWO_GROUPS;
        let r = self.rng.gen_range(0, MOVE_BOX_TO_OTHER_GROUP);
        let ret = if r < SWAP_IN_GROUP {
            self.swap_in_group(input, sched)
        } else if r < SWAP_PART_OF_TWO_GROUPS {
            self.swap_part_of_two_groups(input, sched)
        } else {
            self.move_box_to_other_group(input, sched)
        };
        if ret && self.total_penalties == 0 && self.groups.last().unwrap().points.is_empty() {
            // Remove empty group
            self.groups.pop();
            self.turns.pop();
            self.penalties.pop();
        }
        ret
    }

    // neighbor
    fn swap_in_group(&mut self, input: &Input, sched: &mut AnnealingScheduler) -> bool {
        let g = self.rng.gen_range(0, self.groups.len());
        let group = &mut self.groups[g];
        if group.points.len() < 2 {
            return false; // No swap possible
        }
        let i = self.rng.gen_range(0, group.points.len() - 1);
        group.points.swap(i, i + 1);
        let (t, p) = group.evaluate(input);

        let dt = t - self.turns[g];
        let dp = p - self.penalties[g];

        if !Self::accept(dt, dp, sched) {
            // reject
            group.points.swap(i, i + 1); // Revert swap
            return false;
        }
        // accept
        self.turns[g] = t;
        self.penalties[g] = p;
        self.total_turns += dt;
        self.total_penalties += dp;
        true
    }

    // neighbor
    fn swap_part_of_two_groups(&mut self, input: &Input, sched: &mut AnnealingScheduler) -> bool {
        let g1 = self.rng.gen_range(0, self.groups.len());
        let g2 = self.rng.gen_range(0, self.groups.len());
        if g1 == g2 {
            return false; // No swap possible
        }
        let i1 = self.rng.gen_range(0, self.groups[g1].points.len() + 1);
        let i2 = self.rng.gen_range(0, self.groups[g2].points.len() + 1);

        // swap g1[i1..] and g2[i2..]
        let mut group1 = self.groups[g1].clone();
        let mut group2 = self.groups[g2].clone();
        let mut g1r = group1.points.split_off(i1);
        let mut g2r = group2.points.split_off(i2);
        group1.points.append(&mut g2r);
        group2.points.append(&mut g1r);

        let (t1, p1) = group1.evaluate(input);
        let (t2, p2) = group2.evaluate(input);
        let dt = t1 + t2 - self.turns[g1] - self.turns[g2];
        let dp = p1 + p2 - self.penalties[g1] - self.penalties[g2];

        if !Self::accept(dt, dp, sched) {
            // reject
            return false;
        }
        // accept
        self.groups[g1] = group1;
        self.groups[g2] = group2;
        self.turns[g1] = t1;
        self.turns[g2] = t2;
        self.penalties[g1] = p1;
        self.penalties[g2] = p2;
        self.total_turns += dt;
        self.total_penalties += dp;
        true
    }

    // neighbor
    fn move_box_to_other_group(&mut self, input: &Input, sched: &mut AnnealingScheduler) -> bool {
        let g1 = self.rng.gen_range(0, self.groups.len());
        let g2 = self.rng.gen_range(0, self.groups.len());
        if g1 == g2 || self.groups[g1].points.is_empty() {
            return false; // No move possible
        }
        let mut group1 = self.groups[g1].clone();
        let mut group2 = self.groups[g2].clone();

        let i = self.rng.gen_range(0, group1.points.len());

        let j = self.rng.gen_range(0, group2.points.len() + 1);
        // let j = if self.rng.gen_range(0, 4) == 0 {
        //     self.rng.gen_range(0, group2.points.len() + 1)
        // } else {
        //     let (ix, iy) = group1.points[i];
        //     let mut j = 0;
        //     let mut min_cost = usize::MAX;
        //     let mut prev_cost = ix + iy;
        //     for k in 0..group2.points.len() {
        //         let (mx, my) = group2.points[k];
        //         let cost = mx.abs_diff(ix) + my.abs_diff(iy);
        //         if min_cost.chmin(prev_cost + cost) {
        //             j = k;
        //         }
        //         prev_cost = cost;
        //     }
        //     if min_cost.chmin(prev_cost + ix + iy) {
        //         j = group2.points.len(); // append to the end
        //     }
        //     j
        // };

        group2.points.insert(j, group1.points.remove(i));

        let (t1, p1) = group1.evaluate(input);
        let (t2, p2) = group2.evaluate(input);
        let dt = t1 + t2 - self.turns[g1] - self.turns[g2];
        let dp = p1 + p2 - self.penalties[g1] - self.penalties[g2];
        if !Self::accept(dt, dp, sched) {
            // reject
            return false;
        }
        // accept
        self.groups[g1] = group1;
        self.groups[g2] = group2;
        self.turns[g1] = t1;
        self.turns[g2] = t2;
        self.penalties[g1] = p1;
        self.penalties[g2] = p2;
        self.total_turns += dt;
        self.total_penalties += dp;
        true
    }

    fn print(&self) {
        for group in &self.groups {
            group.print();
        }
    }
}

#[derive(Debug, Clone)]
struct Group {
    points: Vec<(usize, usize)>,
}

impl Group {
    fn new() -> Self {
        Self { points: Vec::new() }
    }

    // return turns and penalty
    fn evaluate(&self, input: &Input) -> (i32, i32) {
        let mut turns = 0;
        let mut penalty = 0;
        let mut weight_time = 0;
        let mut nx = 0;
        let mut ny = 0;
        for &(x, y) in self.points.iter().rev() {
            turns += x.abs_diff(nx) + y.abs_diff(ny);

            // penalty.chmax(turns as i32 * sw - input[x][y].d);
            penalty += (weight_time - input[x][y].d).max(0);

            weight_time += turns as i32 * input[x][y].w;
            nx = x;
            ny = y;
        }
        turns += nx + ny;
        (turns as i32, penalty)
    }

    fn print(&self) {
        let mut x = 0;
        let mut y = 0;
        for &(nx, ny) in &self.points {
            if x < nx {
                for _ in x..nx {
                    println!("D");
                }
            } else {
                for _ in nx..x {
                    println!("U");
                }
            }
            if y < ny {
                for _ in y..ny {
                    println!("R");
                }
            } else {
                for _ in ny..y {
                    println!("L");
                }
            }
            println!("1");
            x = nx;
            y = ny;
        }
        for _ in 0..x {
            println!("U");
        }
        for _ in 0..y {
            println!("L");
        }
    }
}

#[derive(Debug, Clone, Copy)]
struct Box {
    w: i32,
    d: i32,
}

type Input = [[Box; N]; N];

fn read_input() -> Input {
    input! {
        _n: usize,
        w: [[i32; N]; N],
        d: [[i32; N]; N],
    }
    let mut input = [[Box { w: 0, d: 0 }; N]; N];
    for x in 0..N {
        for y in 0..N {
            input[x][y] = Box {
                w: w[x][y],
                d: d[x][y] - 1,
            };
        }
    }
    input
}

const SA_TIME_COUNTS: usize = 1 << 10;
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
    progress: f64,
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

        use rand::seq::SliceRandom;
        use rand::SeedableRng;

        let mut rng = rand_pcg::Pcg64Mcg::seed_from_u64(0);
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
            progress: 0.0,
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
        let progress = (get_time_sec() - self.start_time_sec) / self.duration_sec;
        self.progress = progress;
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
    pub fn gen(&mut self) -> u32 {
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
        self.gen() as f32 / ((1u64 << 32) as f32)
    }

    /// 0.0..=1.0
    #[inline(always)]
    pub fn gen_f64(&mut self) -> f64 {
        self.gen() as f64 / ((1u64 << 32) as f64)
    }

    /// l..r
    #[inline(always)]
    pub fn gen_range(&mut self, l: usize, r: usize) -> usize {
        debug_assert!(l < r);
        debug_assert!(r as u64 <= 1 << 32);
        l + (((r - l) as u64 * self.gen() as u64) >> 32) as usize
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f32(&mut self, l: f32, r: f32) -> f32 {
        debug_assert!(l <= r);
        l + self.gen() as f32 * ((r - l) / ((1u64 << 32) as f32))
    }

    /// l..=r
    #[inline(always)]
    pub fn gen_range_f64(&mut self, l: f64, r: f64) -> f64 {
        debug_assert!(l <= r);
        l + self.gen() as f64 * ((r - l) / ((1u64 << 32) as f64))
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
