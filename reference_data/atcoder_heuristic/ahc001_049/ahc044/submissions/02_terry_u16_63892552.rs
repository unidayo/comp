use itertools::Itertools;
#[allow(unused_imports)]
use proconio::*;
#[allow(unused_imports)]
use rand::prelude::*;
use rand_distr::WeightedIndex;
use std::{cell::RefCell, env, fmt::Display, rc::Rc, str::FromStr};

pub trait ChangeMinMax {
    fn change_min(&mut self, v: Self) -> bool;
    fn change_max(&mut self, v: Self) -> bool;
}

impl<T: PartialOrd> ChangeMinMax for T {
    fn change_min(&mut self, v: T) -> bool {
        *self > v && {
            *self = v;
            true
        }
    }

    fn change_max(&mut self, v: T) -> bool {
        *self < v && {
            *self = v;
            true
        }
    }
}

fn main() {
    let input = Input::read_input();
    let params = Params::get();
    let params = params.borrow();
    const WEEKS: [usize; 8] = [3906, 7812, 15625, 31250, 62500, 125000, 250000, 500000];

    let temps = [
        params.temp0,
        params.temp1,
        params.temp2,
        params.temp3,
        params.temp4,
        params.temp5,
        params.temp6,
        params.temp7,
        params.temp8,
    ];
    let duration_ratios = [
        params.duration0,
        params.duration1,
        params.duration2,
        params.duration3,
        params.duration4,
        params.duration5,
        params.duration6,
        params.duration7,
    ];
    let duration_ratio_sum: f64 = duration_ratios.iter().sum();
    let mul = 1.99 / duration_ratio_sum;
    let durations = duration_ratios.iter().map(|&x| x * mul).collect::<Vec<_>>();

    let mut state = State::init(&Env::new(&input, WEEKS[0]));

    for ((&weeks, (&temp0, &temp1)), &duration) in WEEKS
        .iter()
        .zip(temps.iter().tuple_windows())
        .zip(durations.iter())
    {
        let env = Env::new(&input, weeks);
        let s = State::new(&env, state.next);
        state = annealing(&env, s, duration, &params, temp0, temp1);
    }

    println!("{}", state);
}

#[derive(Debug, Clone)]
struct Input {
    targets: Vec<u32>,
}

impl Input {
    const PEOPLE_CNT: usize = 100;
    const WEEKS: usize = 500_000;

    fn read_input() -> Self {
        input! {
            _n: usize,
            _l: usize,
            targets: [u32; Self::PEOPLE_CNT],
        }

        Self { targets }
    }
}

struct Env {
    weeks: usize,
    targets: Vec<f64>,
}

impl Env {
    fn new(input: &Input, weeks: usize) -> Self {
        let targets = input
            .targets
            .iter()
            .map(|&x| x as f64 * weeks as f64 / Input::WEEKS as f64)
            .collect();

        Self { weeks, targets }
    }
}

#[derive(Debug, Clone)]
struct State {
    next: [[usize; 2]; Input::PEOPLE_CNT],
    counts: [usize; Input::PEOPLE_CNT],
    score: f64,
}

impl State {
    fn new(env: &Env, next: [[usize; 2]; Input::PEOPLE_CNT]) -> Self {
        let mut state = Self {
            next,
            counts: [0; Input::PEOPLE_CNT],
            score: 0.0,
        };

        state.update_score(env);
        state
    }

    fn init(env: &Env) -> Self {
        let mut next = [[0; 2]; Input::PEOPLE_CNT];

        for i in 0..Input::PEOPLE_CNT {
            next[i][0] = i;
            next[i][1] = i;
        }

        Self::new(env, next)
    }

    fn update_score(&mut self, env: &Env) {
        let mut counts = [0; Input::PEOPLE_CNT];
        let mut person = 0;

        for _ in 0..env.weeks {
            counts[person] += 1;
            person = self.next[person][counts[person] % 2];
        }

        let mut scores = 0.0;

        for i in 0..Input::PEOPLE_CNT {
            scores += (counts[i] as f64 - env.targets[i] as f64).abs();
        }

        self.counts = counts;
        self.score = 1e6 - scores * Input::WEEKS as f64 / env.weeks as f64
    }
}

impl Display for State {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for i in 0..Input::PEOPLE_CNT {
            writeln!(f, "{} {}", self.next[i][0], self.next[i][1])?;
        }

        Ok(())
    }
}

fn annealing(
    env: &Env,
    initial_solution: State,
    duration: f64,
    params: &Params,
    temp0: f64,
    temp1: f64,
) -> State {
    let mut state = initial_solution;
    let mut best_solution = state.clone();
    let mut current_score = state.score;
    let mut best_score = current_score;
    let init_score = current_score;

    let mut all_iter = 0;
    let mut valid_iter = 0;
    let mut accepted_count = 0;
    let mut update_count = 0;
    let mut rng = rand_pcg::Pcg64Mcg::from_entropy();

    let duration_inv = 1.0 / duration;
    let since = std::time::Instant::now();

    let mut probs = [
        params.neigh_prob_change,
        params.neigh_prob_change_suit,
        params.neigh_prob_swap,
        params.neigh_prob_swap_suit,
        params.neigh_prob_shift,
    ];

    if env.weeks < Input::WEEKS / 2 {
        probs[4] = 0.0;
    }

    let dist = WeightedIndex::new(probs.iter()).unwrap();

    loop {
        all_iter += 1;

        let time = (std::time::Instant::now() - since).as_secs_f64() * duration_inv;
        let temp = f64::powf(temp0, 1.0 - time) * f64::powf(temp1, time);
        let inv_temp = 1.0 / temp;

        if time >= 1.0 {
            break;
        }

        // 変形
        let neigh_type = dist.sample(&mut rng);

        let mut neigh = match neigh_type {
            0 => ChangeNeigh::gen(env, &state, &mut rng),
            1 => match ChangeNeigh::gen_suitable(env, &state, &mut rng) {
                Some(neigh) => neigh,
                None => continue,
            },
            2 => SwapNeigh::gen(env, &state, &mut rng),
            3 => match SwapNeigh::gen_suitable(env, &state, &mut rng) {
                Some(neigh) => neigh,
                None => continue,
            },
            4 => SwapShiftNeigh::gen(env, &state, &mut rng),
            _ => unreachable!(),
        };

        neigh.apply(env, &mut state);

        // スコア計算
        state.update_score(env);
        let new_score = state.score;
        let score_diff = new_score - current_score;

        if score_diff >= 0.0 || rng.gen_bool(f64::exp(score_diff as f64 * inv_temp)) {
            // 解の更新
            current_score = new_score;
            accepted_count += 1;
            state = state;

            if best_score.change_max(current_score) {
                best_solution = state.clone();
                update_count += 1;
            }
        } else {
            neigh.rollback(env, &mut state);
        }

        valid_iter += 1;
    }

    eprintln!("===== annealing =====");
    eprintln!("init score : {}", init_score);
    eprintln!("score      : {}", best_score);
    eprintln!("all iter   : {}", all_iter);
    eprintln!("valid iter : {}", valid_iter);
    eprintln!("accepted   : {}", accepted_count);
    eprintln!("updated    : {}", update_count);
    eprintln!("");

    best_solution
}

trait Neigh {
    fn apply(&mut self, env: &Env, state: &mut State);
    fn rollback(&mut self, env: &Env, state: &mut State);
}

struct ChangeNeigh {
    person: usize,
    shift: usize,
    to: usize,
    state: Option<State>,
}

impl ChangeNeigh {
    fn gen(_env: &Env, state: &State, rng: &mut impl Rng) -> Box<dyn Neigh> {
        let person = rng.gen_range(0..Input::PEOPLE_CNT);
        let shift = rng.gen_range(0..2);
        let mut to = rng.gen_range(0..Input::PEOPLE_CNT);

        while to == state.next[person][shift] {
            to = rng.gen_range(0..Input::PEOPLE_CNT);
        }

        Box::new(Self {
            person,
            shift,
            to,
            state: None,
        })
    }

    fn gen_suitable(env: &Env, state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
        thread_local! {
            static IN_EDGES: RefCell<Vec<(usize, usize)>> = RefCell::new(vec![]);
            static CANDIDATES: RefCell<Vec<(usize, usize, usize, f64)>> = RefCell::new(vec![]);
        }

        IN_EDGES.with(|in_edges| {
            let mut in_edges = in_edges.borrow_mut();
            in_edges.clear();

            CANDIDATES.with(|candidates| {
                let mut candidates = candidates.borrow_mut();
                candidates.clear();

                let mut excess = [0.0; Input::PEOPLE_CNT];
                let mut diffs = [0.0; Input::PEOPLE_CNT];

                for ((&tgt, &cnt), (ex, diff)) in env
                    .targets
                    .iter()
                    .zip(state.counts.iter())
                    .zip(excess.iter_mut().zip(diffs.iter_mut()))
                {
                    let d = cnt as f64 - tgt;
                    *ex = d.max(0.0);
                    *diff = d.abs();
                }

                let dist = WeightedIndex::new(excess.iter().map(|x| pow_4(*x))).unwrap();
                let person = dist.sample(rng);

                let mut in_edges = vec![];

                for i in 0..Input::PEOPLE_CNT {
                    for shift in 0..2 {
                        if state.next[i][shift] == person {
                            in_edges.push((i, shift));
                        }
                    }
                }

                let mut candidates = vec![];

                for &(i, shift) in in_edges.iter() {
                    let cnt_half = state.counts[i] as f64 * 0.5;

                    for to in 0..Input::PEOPLE_CNT {
                        if i == to {
                            continue;
                        }

                        let current_diff = diffs[person] + diffs[to];
                        let next_diff =
                            (state.counts[person] as f64 - cnt_half - env.targets[person]).abs()
                                + (state.counts[to] as f64 + cnt_half - env.targets[to]).abs();

                        if current_diff > next_diff {
                            candidates.push((i, shift, to, current_diff - next_diff));
                        }
                    }
                }

                if candidates.len() == 0 {
                    return None;
                }

                let weight = candidates.iter().map(|&(_, _, _, diff)| pow_4(diff));
                let dist = WeightedIndex::new(weight).unwrap();
                let (person, shift, to, _) = candidates[dist.sample(rng)];

                let result: Option<Box<dyn Neigh>> = Some(Box::new(Self {
                    person,
                    shift,
                    to,
                    state: None,
                }));

                result
            })
        })
    }
}

impl Neigh for ChangeNeigh {
    fn apply(&mut self, _env: &Env, state: &mut State) {
        self.state = Some(state.clone());
        state.next[self.person][self.shift] = self.to;
    }

    fn rollback(&mut self, _env: &Env, state: &mut State) {
        *state = self.state.take().unwrap();
    }
}

struct SwapNeigh {
    person0: usize,
    shift0: usize,
    person1: usize,
    shift1: usize,
    state: Option<State>,
}

impl SwapNeigh {
    fn gen(_env: &Env, _state: &State, rng: &mut impl Rng) -> Box<dyn Neigh> {
        let person0 = rng.gen_range(0..Input::PEOPLE_CNT);
        let shift0 = rng.gen_range(0..2);
        let mut person1 = rng.gen_range(0..Input::PEOPLE_CNT);
        let mut shift1 = rng.gen_range(0..2);

        while person0 == person1 && shift0 == shift1 {
            person1 = rng.gen_range(0..Input::PEOPLE_CNT);
            shift1 = rng.gen_range(0..2);
        }

        Box::new(Self {
            person0,
            shift0,
            person1,
            shift1,
            state: None,
        })
    }

    fn gen_suitable(env: &Env, state: &State, rng: &mut impl Rng) -> Option<Box<dyn Neigh>> {
        thread_local! {
            static IN_EDGES: RefCell<Vec<(usize, usize)>> = RefCell::new(vec![]);
            static CANDIDATES: RefCell<Vec<(usize, usize, usize, usize, f64)>> = RefCell::new(vec![]);
        }

        IN_EDGES.with(|in_edges| {
            let mut in_edges = in_edges.borrow_mut();
            in_edges.clear();

            CANDIDATES.with(|candidates| {
                let mut candidates = candidates.borrow_mut();
                candidates.clear();

                let mut diffs = [0.0; Input::PEOPLE_CNT];

                for ((&tgt, &cnt), diff) in env
                    .targets
                    .iter()
                    .zip(state.counts.iter())
                    .zip(diffs.iter_mut())
                {
                    *diff = (cnt as f64 - tgt).abs();
                }

                let dist = WeightedIndex::new(diffs.iter().map(|x| pow_4(*x))).unwrap();
                let person0 = dist.sample(rng);

                for i in 0..Input::PEOPLE_CNT {
                    for shift in 0..2 {
                        if state.next[i][shift] == person0 {
                            in_edges.push((i, shift));
                        }
                    }
                }

                for &(i0, shift0) in in_edges.iter() {
                    let cnt0_half = state.counts[i0] as f64 * 0.5;

                    for i1 in 0..Input::PEOPLE_CNT {
                        for shift1 in 0..2 {
                            if i0 == i1 && shift0 == shift1 {
                                continue;
                            }

                            // どの程度改善するか？
                            let person1 = state.next[i1][shift1];
                            let cnt1_half = state.counts[i1] as f64 * 0.5;
                            let current_diff = diffs[person0] + diffs[person1];
                            let next_diff = (state.counts[person0] as f64 - cnt0_half + cnt1_half
                                - env.targets[person0])
                                .abs()
                                + (state.counts[person1] as f64 - cnt1_half + cnt0_half
                                    - env.targets[person1])
                                    .abs();

                            if current_diff > next_diff {
                                candidates.push((i0, shift0, i1, shift1, current_diff - next_diff));
                            }
                        }
                    }
                }

                if candidates.len() == 0 {
                    return None;
                }

                let weight = candidates.iter().map(|&(_, _, _, _, diff)| pow_4(diff));
                let dist = WeightedIndex::new(weight).unwrap();
                let (person0, shift0, person1, shift1, _) = candidates[dist.sample(rng)];

                let result: Option<Box<dyn Neigh>> = Some(Box::new(Self {
                    person0,
                    shift0,
                    person1,
                    shift1,
                    state: None,
                }));

                result
            })
        })
    }
}

fn pow_4(x: f64) -> f64 {
    let x2 = x * x;
    let x4 = x2 * x2;
    x4
}

impl Neigh for SwapNeigh {
    fn apply(&mut self, _env: &Env, state: &mut State) {
        self.state = Some(state.clone());
        let tmp = state.next[self.person0][self.shift0];
        state.next[self.person0][self.shift0] = state.next[self.person1][self.shift1];
        state.next[self.person1][self.shift1] = tmp;
    }

    fn rollback(&mut self, _env: &Env, state: &mut State) {
        *state = self.state.take().unwrap();
    }
}

struct SwapShiftNeigh {
    person0: usize,
    state: Option<State>,
}

impl SwapShiftNeigh {
    fn gen(_env: &Env, _state: &State, rng: &mut impl Rng) -> Box<dyn Neigh> {
        let person0 = rng.gen_range(0..Input::PEOPLE_CNT);

        Box::new(Self {
            person0,
            state: None,
        })
    }
}

impl Neigh for SwapShiftNeigh {
    fn apply(&mut self, _env: &Env, state: &mut State) {
        self.state = Some(state.clone());
        state.next[self.person0].swap(0, 1);
    }

    fn rollback(&mut self, _env: &Env, state: &mut State) {
        *state = self.state.take().unwrap();
    }
}

thread_local! {
    static PARAMS: Rc<RefCell<Params>> = Rc::new(RefCell::new(Params::from_env()));
}

#[derive(Debug, Clone)]
pub struct Params {
    temp0: f64,
    temp1: f64,
    temp2: f64,
    temp3: f64,
    temp4: f64,
    temp5: f64,
    temp6: f64,
    temp7: f64,
    temp8: f64,
    duration0: f64,
    duration1: f64,
    duration2: f64,
    duration3: f64,
    duration4: f64,
    duration5: f64,
    duration6: f64,
    duration7: f64,
    neigh_prob_swap: f64,
    neigh_prob_swap_suit: f64,
    neigh_prob_change: f64,
    neigh_prob_change_suit: f64,
    neigh_prob_shift: f64,
}

impl Params {
    #[allow(dead_code)]
    fn from_env() -> Self {
        let temp0 = get_env("AHC_TEMP0", 1099.120386407297);
        let temp1 = get_env("AHC_TEMP1", 735.7241651891716);
        let temp2 = get_env("AHC_TEMP2", 593.3069095856193);
        let temp3 = get_env("AHC_TEMP3", 417.0837924298739);
        let temp4 = get_env("AHC_TEMP4", 238.54015996647942);
        let temp5 = get_env("AHC_TEMP5", 136.17506118410714);
        let temp6 = get_env("AHC_TEMP6", 104.81988329319867);
        let temp7 = get_env("AHC_TEMP7", 8.492565036842366);
        let temp8 = get_env("AHC_TEMP8", 7.933257199050386);

        let duration0 = get_env("AHC_DURATION0", 0.169168589882266);
        let duration1 = get_env("AHC_DURATION1", 0.5352858765093825);
        let duration2 = get_env("AHC_DURATION2", 0.4184676566323651);
        let duration3 = get_env("AHC_DURATION3", 0.902699736777769);
        let duration4 = get_env("AHC_DURATION4", 0.5096876465670732);
        let duration5 = get_env("AHC_DURATION5", 0.4808503133205497);
        let duration6 = get_env("AHC_DURATION6", 0.4760330055347155);
        let duration7 = get_env("AHC_DURATION7", 0.08138204382783938);

        let neigh_prob_swap = get_env("AHC_NEIGH_PROB_SWAP", 0.030850096960706633);
        let neigh_prob_swap_suit = get_env("AHC_NEIGH_PROB_SWAP_SUIT", 0.8386667150361452);
        let neigh_prob_change = get_env("AHC_NEIGH_PROB_CHANGE", 0.030014167792913105);
        let neigh_prob_change_suit = get_env("AHC_NEIGH_PROB_CHANGE_SUIT", 0.816524156933654);
        let neigh_prob_shift = get_env("AHC_NEIGH_PROB_SHIFT", 0.3729115680391697);

        Self {
            temp0,
            temp1,
            temp2,
            temp3,
            temp4,
            temp5,
            temp6,
            temp7,
            temp8,
            duration0,
            duration1,
            duration2,
            duration3,
            duration4,
            duration5,
            duration6,
            duration7,
            neigh_prob_swap,
            neigh_prob_swap_suit,
            neigh_prob_change,
            neigh_prob_change_suit,
            neigh_prob_shift,
        }
    }

    pub fn get() -> Rc<RefCell<Self>> {
        PARAMS.with(|p| p.clone())
    }
}

fn get_env<T: FromStr>(name: &str, default: T) -> T {
    env::var(name)
        .ok()
        .and_then(|s| s.parse().ok())
        .unwrap_or(default)
}
