use crate::ahc_library::utils::{
    index_set::IndexSet,
    random::{DiscreteSampler, RandPcg64Mcg, Random, RandomSampler},
    time,
    v2::V2,
};
use proconio::input;

const TL: f64 = 1.98;
const N: usize = 20;
const M: usize = N * N / 2;

const PRUNE_THRESHOLD: i64 = N as i64 / 2;

struct Input {
    n: usize,
    a: [[usize; N]; N],
}
impl Input {
    fn read_input() -> Self {
        input! {
            n: usize,
            _a: [[usize; N]; N],
        }
        let mut a = [[0; N]; N];
        for i in 0..N {
            for j in 0..N {
                a[i][j] = _a[i][j];
            }
        }
        Self { n, a }
    }
}

type VMap = [[V2<i64>; 2]; M];
type DistTable = [[[[i64; 2]; M]; 2]; M];

fn get_vmap(input: &Input) -> VMap {
    let mut vmap = [[V2::new(0, 0); 2]; M];
    let mut count = vec![0; M];
    for i in 0..input.n {
        for j in 0..input.n {
            let v = input.a[i][j];
            if count[v] == 0 {
                vmap[v][0] = V2::new(i as i64, j as i64);
            } else {
                vmap[v][1] = V2::new(i as i64, j as i64);
            }
            count[v] += 1;
        }
    }
    vmap
}
fn calc_dist_table(vmap: &VMap) -> DistTable {
    let mut table = [[[[0; 2]; M]; 2]; M];
    for i in 0..M {
        for j in 0..M {
            for a in 0..2 {
                for b in 0..2 {
                    let d = vmap[i][a] - vmap[j][b];
                    table[i][a][j][b] = d.x.abs() + d.y.abs();
                }
            }
        }
    }
    table
}

fn pertubate_e(e: i64, rnd: &mut impl Random) -> i64 {
    const A: i64 = 1000;
    e * A as i64 + rnd.gen_index(A as usize) as i64
}

fn find_best_insertion(
    dtable: &DistTable,
    order: &Vec<(usize, usize)>,
    vmap: &VMap,
    i: usize,
    before_set: &mut IndexSet,
    seen_set: &mut IndexSet,
    rnd: &mut impl Random,
) -> (i64, (usize, usize, usize, usize, usize)) {
    let mut best_e = 1_000_000_000;
    let mut best = (0, 0, 0, 0, 1); // (i, l, r, la, ra)

    before_set.clear();

    for l in 0..=order.len() {
        if l > 0 {
            let (v, _) = order[l - 1];
            if !before_set.contains(v) {
                before_set.add(v);
            } else {
                before_set.remove(v);
            }
        }

        // 隣接して挿入する
        // 前のコスト: d(l-1, l)
        let cur_cost = d(&dtable, &order, &vmap, l);

        // 枝刈り
        // min(d(l-1,a), d(l-1,1-a)) >= PRUNE_THRESHOLD ならスキップ
        let first_dist = (0..2)
            .map(|a| {
                if l == 0 {
                    let v1 = vmap[i][a];
                    v1.x + v1.y
                } else {
                    dtable[order[l - 1].0][order[l - 1].1][i][a]
                }
            })
            .min()
            .unwrap();
        if first_dist >= PRUNE_THRESHOLD {
            continue;
        }

        for a in 0..2 {
            let mut new_cost = 0;
            // 後のコスト: d(l-1, a) + d(a, 1-a) + d(1-a, l)
            if l == 0 {
                let v1 = vmap[i][a];
                new_cost += v1.x + v1.y;
            } else {
                new_cost += dtable[order[l - 1].0][order[l - 1].1][i][a];
            }
            new_cost += dtable[i][a][i][1 - a];
            if l < order.len() {
                new_cost += dtable[i][1 - a][order[l].0][order[l].1];
            }

            let e = new_cost - cur_cost;
            let e = pertubate_e(e, rnd);
            let v = (i, l, l, a, 1 - a);
            if e < best_e {
                best_e = e;
                best = v;
            }
        }

        seen_set.clear();

        // 挿入先として選べる条件
        // - 間にあるものが全てペアとして消されている
        for r in l + 1..=order.len() {
            let (v, _) = order[r - 1];
            // before_setに含まれているものが見つかったら、それ以降は挿入できない
            if before_set.contains(v) {
                break;
            }

            if seen_set.contains(v) {
                seen_set.remove(v);
            } else {
                seen_set.add(v);
            }

            if seen_set.size() > 0 {
                continue;
            }

            // 挿入可能
            // 前のコスト: d(l-1,l) + d(r-1,r)
            let cur_cost = d(&dtable, &order, &vmap, l) + d(&dtable, &order, &vmap, r);

            // 挿入時のコスト
            // 順番は近い方の距離を採用する
            for a in 0..2 {
                let mut new_cost = 0;

                if l == 0 {
                    let v1 = vmap[i][a];
                    new_cost += v1.x + v1.y;
                } else {
                    new_cost += dtable[order[l - 1].0][order[l - 1].1][i][a];
                }
                if l < order.len() {
                    new_cost += dtable[i][a][order[l].0][order[l].1];
                }
                if r == 0 {
                    let v1 = vmap[i][1 - a];
                    new_cost += v1.x + v1.y;
                } else {
                    new_cost += dtable[order[r - 1].0][order[r - 1].1][i][1 - a];
                }
                if r < order.len() {
                    new_cost += dtable[i][1 - a][order[r].0][order[r].1];
                }

                let e = new_cost - cur_cost;
                let e = pertubate_e(e, rnd);
                let v = (i, l, r, a, 1 - a);
                if e < best_e {
                    best_e = e;
                    best = v;
                }
            }
        }
    }

    (best_e, best)
}

fn main() {
    time::start_clock(1.);
    let input = Input::read_input();

    let vmap = get_vmap(&input);
    let dtable = calc_dist_table(&vmap);

    let mut rnd = RandPcg64Mcg::new(0);

    // 初期解
    // 全てのiの全ての挿入先を試して、最も良いものを採用する
    let mut remaining = IndexSet::full(M);

    // (i, idx)
    let mut order: Vec<(usize, usize)> = Vec::with_capacity(M * 2);
    let mut set1 = IndexSet::empty(M);
    let mut set2 = IndexSet::empty(M);
    while remaining.size() > 0 {
        let mut best_e = 1_000_000_000;
        let mut best = (0, 0, 0, 0, 0); // (i, l, r, la, ra)

        for &i in remaining.iter() {
            let (e, v) =
                find_best_insertion(&dtable, &order, &vmap, i, &mut set1, &mut set2, &mut rnd);
            if e < best_e {
                best_e = e;
                best = v;
            }
        }

        let (i, l, r, la, ra) = best;
        remaining.remove(i);
        order.insert(l, (i, la));
        order.insert(r + 1, (i, ra));
    }

    let initial_score = calc_score(&order, &vmap);
    eprintln!("elapsed: {:.3}", time::elapsed_seconds());
    eprintln!("start score: {:5}", initial_score);

    // 局所改善
    // ランダムに一つのペアを削除して、最適な挿入位置を探す
    let mut sampler: DiscreteSampler<usize, _> = DiscreteSampler::new(
        &vec![(1, 1), (2, 2), (2, 3), (4, 4), (3, 8), (3, 10), (3, 16)],
        RandPcg64Mcg::new(1),
    );

    let mut it = 0;
    let mut is = IndexSet::empty(M);
    let mut best_score = initial_score;
    let mut best_order = order.clone();

    while time::elapsed_seconds() < TL {
        is.clear();

        let r = sampler.sample();
        while is.size() < r {
            let i = rnd.gen_index(M);
            if is.contains(i) {
                continue;
            }
            is.add(i);
        }

        for &i in is.iter() {
            let mut idx1 = None;
            let mut idx2 = None;
            for (idx, &(v, a)) in order.iter().enumerate() {
                if v == i {
                    if idx1.is_none() {
                        idx1 = Some((idx, a));
                    } else {
                        idx2 = Some((idx, a));
                    }
                }
            }
            let idx1 = idx1.unwrap();
            let idx2 = idx2.unwrap();

            // 削除
            order.remove(idx2.0);
            order.remove(idx1.0);
        }

        for &i in is.iter() {
            let (_, best) =
                find_best_insertion(&dtable, &order, &vmap, i, &mut set1, &mut set2, &mut rnd);
            let (i, l, r, la, ra) = best;

            order.insert(l, (i, la));
            order.insert(r + 1, (i, ra));
        }

        let new_score = calc_score(&order, &vmap);
        if new_score < best_score {
            best_score = new_score;
            best_order = order.clone();
        }

        it += 1;
    }

    let order = best_order;
    eprintln!("iterations: {:5}", it);

    let final_score = calc_score(&order, &vmap);
    eprintln!(
        "finalscore: {}, {}",
        final_score,
        (N * N + 2 * N * N * N) as i64 - final_score
    );
    output(&order, &vmap);
    eprintln!("elapsed: {:.3}", time::elapsed_seconds());
}

#[allow(dead_code)]
fn calc_score(order: &Vec<(usize, usize)>, vmap: &VMap) -> i64 {
    let mut score = 0;
    let mut cur = V2::new(0, 0);
    for &(v, a) in order.iter() {
        let next = vmap[v][a];
        let d = next - cur;
        score += d.x.abs() + d.y.abs();
        cur = next;
    }
    score
}

fn output(order: &Vec<(usize, usize)>, vmap: &VMap) {
    let mut cur = V2::new(0, 0);
    for &(v, a) in order.iter() {
        let next = vmap[v][a];
        let d = next - cur;
        for _ in 0..d.x.abs() {
            if d.x > 0 {
                println!("D");
            } else {
                println!("U");
            }
        }
        for _ in 0..d.y.abs() {
            if d.y > 0 {
                println!("R");
            } else {
                println!("L");
            }
        }
        println!("Z");
        cur = next;
    }
}

#[inline]
fn d(dtable: &DistTable, order: &Vec<(usize, usize)>, vmap: &VMap, i: usize) -> i64 {
    if i < order.len() {
        if i == 0 {
            let v = vmap[order[i].0][order[i].1];
            v.x + v.y
        } else {
            dist_between(&dtable, &order, i - 1, i)
        }
    } else {
        0
    }
}

#[inline]
fn dist_between(dtable: &DistTable, order: &Vec<(usize, usize)>, i: usize, j: usize) -> i64 {
    let (vi, ai) = order[i];
    let (vj, aj) = order[j];
    dtable[vi][ai][vj][aj]
}

pub mod ahc_library {
    pub mod annealer {
        pub mod components {
            pub mod neighbor_generator {
                use crate::ahc_library::{
                    annealer::types::{NeighborGenerator, NeighborType},
                    utils::random::Random,
                };

                pub struct WeightedNeighborGenerator<N>
                where
                    N: NeighborType,
                {
                    neighbors: Vec<(N, f64)>,
                }

                impl<N> WeightedNeighborGenerator<N>
                where
                    N: NeighborType,
                {
                    pub fn new(mut neighbors: Vec<(N, f64)>) -> WeightedNeighborGenerator<N> {
                        let total_weight: f64 = neighbors.iter().map(|(_, weight)| *weight).sum();
                        let mut cum_weight = 0.;
                        for (_, weight) in neighbors.iter_mut() {
                            assert!(*weight >= 0.0);
                            cum_weight += *weight / total_weight;
                            *weight = cum_weight;
                        }

                        WeightedNeighborGenerator { neighbors }
                    }
                }

                impl<N> NeighborGenerator<N> for WeightedNeighborGenerator<N>
                where
                    N: NeighborType,
                {
                    fn generate(&self, _: f64, rnd: &mut impl Random) -> N::H {
                        let p = rnd.nextf();
                        for (n, cum_weight) in self.neighbors.iter() {
                            if p < *cum_weight {
                                return n.setup();
                            }
                        }
                        unreachable!()
                    }
                }
            }

            pub mod criterion {
                use crate::ahc_library::{annealer::types::Criterion, utils::random::Random};

                pub struct HillClimbingCriterion {
                    is_maximize: bool,
                }

                impl HillClimbingCriterion {
                    pub fn new(is_maximize: bool) -> Self {
                        HillClimbingCriterion { is_maximize }
                    }
                }

                impl Criterion for HillClimbingCriterion {
                    fn adopt(
                        &self,
                        cur_score: f64,
                        new_score: f64,
                        _: f64,
                        _: f64,
                        _: &mut impl Random,
                    ) -> bool {
                        if self.is_maximize {
                            new_score >= cur_score
                        } else {
                            new_score <= cur_score
                        }
                    }
                }

                pub struct AnnealingCriterion {
                    is_maximize: bool,
                }

                impl AnnealingCriterion {
                    pub fn new(is_maximize: bool) -> Self {
                        AnnealingCriterion { is_maximize }
                    }
                }

                impl Criterion for AnnealingCriterion {
                    fn adopt(
                        &self,
                        cur_score: f64,
                        new_score: f64,
                        cur_temp: f64,
                        _: f64,
                        rnd: &mut impl Random,
                    ) -> bool {
                        let sign = self.is_maximize as i32 * 2 - 1;
                        let score_diff = sign as f64 * (new_score - cur_score);
                        if score_diff > 0. {
                            return true;
                        }
                        let prob = (score_diff / cur_temp).exp();
                        rnd.nextf() < prob
                    }
                }
            }

            pub mod temperature_scheduler {
                use crate::ahc_library::annealer::types::TemperatureScheduler;

                pub struct ExpTemperatureScheduler {
                    start_temp: f64,
                    end_temp: f64,
                }

                impl ExpTemperatureScheduler {
                    pub fn new(start_temp: f64, end_temp: f64) -> Self {
                        ExpTemperatureScheduler {
                            start_temp,
                            end_temp,
                        }
                    }
                }

                impl TemperatureScheduler for ExpTemperatureScheduler {
                    fn get_temp(&self, progress: f64) -> f64 {
                        self.start_temp.powf(1. - progress) * self.end_temp.powf(progress)
                    }
                }
            }

            pub mod progress_scheduler {
                use crate::ahc_library::{annealer::types::ProgressScheduler, utils::time};

                pub struct IterationProgressScheduler {
                    iteration: usize,
                    cur_step: usize,
                }

                impl IterationProgressScheduler {
                    pub fn new(iteration: usize) -> Self {
                        IterationProgressScheduler {
                            iteration,
                            cur_step: 0,
                        }
                    }
                }

                impl ProgressScheduler for IterationProgressScheduler {
                    fn step(&mut self) {
                        self.cur_step += 1;
                    }

                    fn get_progress(&self) -> f64 {
                        self.cur_step as f64 / self.iteration as f64
                    }
                }

                pub struct SecondProgressScheduler {
                    start_time: f64,
                    seconds: f64,
                }

                impl SecondProgressScheduler {
                    pub fn new(seconds: f64) -> Self {
                        SecondProgressScheduler {
                            start_time: 0.0,
                            seconds,
                        }
                    }
                }

                impl ProgressScheduler for SecondProgressScheduler {
                    fn start(&mut self) {
                        self.start_time = time::elapsed_seconds();
                    }

                    fn get_progress(&self) -> f64 {
                        (time::elapsed_seconds() - self.start_time) / self.seconds
                    }
                }
            }

            pub mod callback {
                use crate::ahc_library::annealer::types::{Callback, Env, State};
                use std::marker::PhantomData;

                pub struct RestoreBestStateCallback<S, E>
                where
                    S: State<E>,
                    E: Env,
                {
                    is_maximize: bool,
                    patience: usize,
                    last_update_step: usize,
                    best_state: Option<S>,
                    _phantom: PhantomData<E>,
                }

                impl<S, E> RestoreBestStateCallback<S, E>
                where
                    S: State<E>,
                    E: Env,
                {
                    pub fn new(patience: usize, is_maximize: bool) -> Self {
                        RestoreBestStateCallback {
                            last_update_step: 0,
                            patience,
                            best_state: None,
                            is_maximize,
                            _phantom: PhantomData,
                        }
                    }
                }

                impl<S, E> Callback<S, E> for RestoreBestStateCallback<S, E>
                where
                    S: State<E>,
                    E: Env,
                {
                    fn on_after_step(
                        &mut self,
                        step: usize,
                        progress: f64,
                        state: &mut S,
                        env: &E,
                    ) {
                        let should_update = match &mut self.best_state {
                            None => true,
                            Some(s) => {
                                let cur_score = s.get_score(env, progress);
                                let new_score = state.get_score(env, progress);
                                self.is_maximize == (new_score > cur_score)
                            }
                        };
                        if should_update {
                            self.best_state = Some(state.clone());
                            self.last_update_step = step;
                            return;
                        }

                        if step < self.last_update_step + self.patience {
                            return;
                        }

                        *state = self.best_state.clone().unwrap();
                        self.last_update_step = step;
                    }
                }

                pub struct ReturnBestStateCallback<S, E>
                where
                    S: State<E>,
                    E: Env,
                {
                    is_maximize: bool,
                    best_state: Option<S>,
                    _phantom: PhantomData<E>,
                }

                impl<S, E> ReturnBestStateCallback<S, E>
                where
                    S: State<E>,
                    E: Env,
                {
                    pub fn new(is_maximize: bool) -> Self {
                        ReturnBestStateCallback {
                            best_state: None,
                            is_maximize,
                            _phantom: PhantomData,
                        }
                    }
                }

                impl<S, E> Callback<S, E> for ReturnBestStateCallback<S, E>
                where
                    S: State<E>,
                    E: Env,
                {
                    fn on_after_step(&mut self, _: usize, progress: f64, state: &mut S, env: &E) {
                        let should_update = match &mut self.best_state {
                            None => true,
                            Some(s) => {
                                let cur_score = s.get_score(env, progress);
                                let new_score = state.get_score(env, progress);
                                self.is_maximize == (new_score > cur_score)
                            }
                        };
                        if should_update {
                            self.best_state = Some(state.clone());
                        }
                    }

                    fn on_finish(&mut self, state: &mut S, _env: &E) {
                        if let Some(best_state) = self.best_state.as_mut() {
                            *state = best_state.clone();
                        }
                    }
                }
            }
        }

        pub mod runner {
            use std::collections::BTreeSet;

            use crate::ahc_library::annealer::scheduler::AnnealerScheduler;
            use crate::ahc_library::annealer::types::{
                Callback, Criterion, NeighborGenerator, NeighborHandler, NeighborType,
                ProgressScheduler, State, TemperatureScheduler,
            };
            use crate::ahc_library::utils::random::Random;

            #[derive(PartialEq, Eq, Clone, Copy, Debug)]
            pub enum AnnealerMode {
                Debug,
                Release,
            }

            pub struct AnnealerConfig {
                pub mode: AnnealerMode,
            }

            #[allow(clippy::type_complexity)]
            pub struct Annealer<G, N, C, T, P, R>
            where
                G: NeighborGenerator<N>,
                N: NeighborType,
                C: Criterion,
                T: TemperatureScheduler,
                P: ProgressScheduler,
                R: Random,
            {
                pub state: <N::H as NeighborHandler>::State,
                pub env: <N::H as NeighborHandler>::Env,
                pub logger: AnnealingLogger,
                mutator: Mutator<G, N>,
                scheduler: AnnealerScheduler<C, T, P, R>,
                rnd: R,
                callbacks: Vec<
                    Box<
                        dyn Callback<
                            <N::H as NeighborHandler>::State,
                            <N::H as NeighborHandler>::Env,
                        >,
                    >,
                >,
                config: AnnealerConfig,
            }

            impl<G, N, C, T, P, R> Annealer<G, N, C, T, P, R>
            where
                G: NeighborGenerator<N>,
                N: NeighborType,
                C: Criterion,
                T: TemperatureScheduler,
                P: ProgressScheduler,
                R: Random,
            {
                #[allow(clippy::type_complexity)]
                pub fn new(
                    state: <N::H as NeighborHandler>::State,
                    env: <N::H as NeighborHandler>::Env,
                    mutator: Mutator<G, N>,
                    scheduler: AnnealerScheduler<C, T, P, R>,
                    callbacks: Vec<
                        Box<
                            dyn Callback<
                                <N::H as NeighborHandler>::State,
                                <N::H as NeighborHandler>::Env,
                            >,
                        >,
                    >,
                    config: AnnealerConfig,
                    rnd: R,
                ) -> Annealer<G, N, C, T, P, R> {
                    Annealer {
                        state,
                        env,
                        logger: AnnealingLogger::new(),
                        mutator,
                        scheduler,
                        rnd,
                        callbacks,
                        config,
                    }
                }

                pub fn run(&mut self) {
                    for callback in &mut self.callbacks {
                        callback.on_start(&mut self.state, &self.env);
                    }

                    let mut cur_step = 0;
                    while self.scheduler.to_next_iter() {
                        let progress = self.scheduler.get_progress();
                        for callback in &mut self.callbacks {
                            callback.on_before_step(cur_step, progress, &mut self.state, &self.env);
                        }

                        let step_log = self.step(progress);

                        if self.config.mode != AnnealerMode::Release {
                            self.logger.send(step_log);
                        }

                        for callback in &mut self.callbacks {
                            callback.on_after_step(cur_step, progress, &mut self.state, &self.env);
                        }

                        cur_step += 1;
                    }

                    for callback in &mut self.callbacks {
                        callback.on_finish(&mut self.state, &self.env);
                    }
                }

                fn step(&mut self, progress: f64) -> StepLog {
                    let cur_score = self.state.get_score(&self.env, progress);
                    let (successed, tag) =
                        self.mutator
                            .mutate(&mut self.state, &self.env, progress, &mut self.rnd);

                    if !successed {
                        return StepLog {
                            score: cur_score,
                            adopt: false,
                            valid: false,
                            tag,
                            score_delta: 0.,
                        };
                    }

                    let new_score = self.state.get_score(&self.env, progress);
                    let score_delta = new_score - cur_score;
                    let adopt = self.scheduler.adopt(cur_score, new_score);
                    if !adopt {
                        self.mutator
                            .revert(&mut self.state, &self.env, &mut self.rnd);
                    }

                    StepLog {
                        score: cur_score,
                        adopt,
                        valid: true,
                        tag,
                        score_delta,
                    }
                }
            }

            pub struct Mutator<G, N>
            where
                G: NeighborGenerator<N>,
                N: NeighborType,
            {
                generator: G,
                last_neighbor: Option<N::H>,
            }

            impl<G, N> Mutator<G, N>
            where
                G: NeighborGenerator<N>,
                N: NeighborType,
            {
                pub fn new(generator: G) -> Mutator<G, N> {
                    Mutator {
                        generator,
                        last_neighbor: None,
                    }
                }

                pub fn mutate(
                    &mut self,
                    state: &mut <N::H as NeighborHandler>::State,
                    env: &<N::H as NeighborHandler>::Env,
                    progress: f64,
                    rnd: &mut impl Random,
                ) -> (bool, &'static str) {
                    let mut n = self.generator.generate(progress, rnd);
                    let successed = n.apply(state, env, rnd);
                    let tag = n.tag();
                    self.last_neighbor = Some(n);
                    if !successed {
                        return (false, tag);
                    }
                    (true, tag)
                }

                pub fn revert(
                    &mut self,
                    state: &mut <N::H as NeighborHandler>::State,
                    env: &<N::H as NeighborHandler>::Env,
                    rnd: &mut impl Random,
                ) {
                    let mut last_neighbor = self
                        .last_neighbor
                        .take()
                        .expect("expect last neighbor being set before revert");
                    last_neighbor.revert(state, env, rnd);
                }
            }

            struct StepLog {
                score: f64,
                adopt: bool,
                valid: bool,
                tag: &'static str,
                score_delta: f64,
            }

            pub struct AnnealingLogger {
                logs: Vec<StepLog>,
            }

            impl AnnealingLogger {
                fn new() -> Self {
                    AnnealingLogger { logs: Vec::new() }
                }

                fn send(&mut self, step_log: StepLog) {
                    self.logs.push(step_log);
                }

                pub fn print(&self) {
                    let total_steps = self.logs.len();
                    let valid_logs = self.logs.iter().filter(|log| log.valid).collect::<Vec<_>>();
                    let valid_steps = valid_logs.len();
                    let initial_score = self.logs.first().map_or(0.0, |log| log.score);
                    let final_score = self.logs.last().map_or(0.0, |log| log.score);
                    let neighbor_tags =
                        self.logs.iter().map(|log| log.tag).collect::<BTreeSet<_>>();

                    eprintln!();
                    eprintln!(
                        "======================== annealing results ========================"
                    );
                    eprintln!("total steps:   {:8}", total_steps);
                    eprintln!(
                        "valid steps:   {:8} ({:5.2}%)",
                        valid_steps,
                        valid_steps as f64 / total_steps as f64 * 100.0
                    );
                    eprintln!("initial score: {:8}", initial_score);
                    eprintln!("final score:   {:8}", final_score);
                    eprintln!("neighbors:");
                    for tag in neighbor_tags {
                        let tag_steps = self.logs.iter().filter(|log| log.tag == tag).count();
                        let valid_steps = valid_logs.iter().filter(|log| log.tag == tag).count();
                        let adopted_steps = valid_logs
                            .iter()
                            .filter(|log| log.tag == tag && log.adopt)
                            .count();
                        let delta_mean = valid_logs
                            .iter()
                            .filter(|log| log.tag == tag && log.adopt)
                            .map(|log| log.score_delta)
                            .sum::<f64>()
                            / adopted_steps.max(1) as f64;
                        let valid_ratio = valid_steps as f64 / tag_steps.max(1) as f64;
                        eprintln!(
                            "  {:<8}: {:5}/{:<5} (adopt={:6.2}%, valid={:6.2}%, Δ={:8.2})",
                            tag,
                            adopted_steps,
                            tag_steps,
                            adopted_steps as f64 / tag_steps as f64 * 100.0,
                            valid_ratio,
                            delta_mean,
                        );
                    }
                    eprintln!(
                        "==================================================================="
                    );
                }
            }
        }

        pub mod scheduler {
            use crate::ahc_library::{
                annealer::{
                    prelude::{
                        AnnealingCriterion, ExpTemperatureScheduler, IterationProgressScheduler,
                        SecondProgressScheduler,
                    },
                    types::{Criterion, ProgressScheduler, TemperatureScheduler},
                },
                utils::random::Random,
            };

            const NUM_PHASE: usize = 3;

            enum AnnealerSchedulerStatus {
                NotStarted,
                InProgress,
                Finished,
            }

            /// Scheduler used for annealing process
            ///
            /// Usage:
            /// ```ignore
            /// let mut scheduler = AnnealerScheduler::with_seconds(1e0, 1e-4, 1.0, true);
            /// while scheduler.to_next_iter() {
            ///     let cur_score = state.get_score();
            ///
            ///     // do something
            ///
            ///     let new_score = state.get_score();
            ///
            ///     if scheduler.adopt(cur_score, new_score) {
            ///         // adopt
            ///     } else {
            ///         // revert
            ///     }
            /// }
            /// ```
            pub struct AnnealerScheduler<C, T, P, R>
            where
                C: Criterion,
                T: TemperatureScheduler,
                P: ProgressScheduler,
                R: Random,
            {
                status: AnnealerSchedulerStatus,
                criterion: C,
                temperature_scheduler: T,
                progress_scheduler: P,
                rnd: R,
            }

            impl<C, T, P, R> AnnealerScheduler<C, T, P, R>
            where
                C: Criterion,
                T: TemperatureScheduler,
                P: ProgressScheduler,
                R: Random,
            {
                pub fn new(
                    criterion: C,
                    temperature_scheduler: T,
                    progress_scheduler: P,
                    rnd: R,
                ) -> Self {
                    AnnealerScheduler {
                        status: AnnealerSchedulerStatus::NotStarted,
                        criterion,
                        temperature_scheduler,
                        progress_scheduler,
                        rnd,
                    }
                }

                pub fn adopt(&mut self, cur_score: f64, new_score: f64) -> bool {
                    let progress = self.get_progress();
                    let cur_temp = self.temperature_scheduler.get_temp(progress);
                    self.criterion
                        .adopt(cur_score, new_score, cur_temp, progress, &mut self.rnd)
                }

                pub fn get_progress(&self) -> f64 {
                    match self.status {
                        AnnealerSchedulerStatus::NotStarted => {
                            panic!("Scheduler has not been started yet.")
                        }
                        AnnealerSchedulerStatus::InProgress => {
                            self.progress_scheduler.get_progress()
                        }
                        AnnealerSchedulerStatus::Finished => 1.,
                    }
                }

                pub fn to_next_iter(&mut self) -> bool {
                    self.status = match self.status {
                        AnnealerSchedulerStatus::NotStarted => {
                            self.progress_scheduler.start();
                            AnnealerSchedulerStatus::InProgress
                        }
                        AnnealerSchedulerStatus::InProgress => {
                            self.progress_scheduler.step();
                            if self.progress_scheduler.get_progress() >= 1. {
                                AnnealerSchedulerStatus::Finished
                            } else {
                                AnnealerSchedulerStatus::InProgress
                            }
                        }
                        AnnealerSchedulerStatus::Finished => AnnealerSchedulerStatus::Finished,
                    };

                    matches!(self.status, AnnealerSchedulerStatus::InProgress)
                }
            }

            impl<R: Random>
                AnnealerScheduler<
                    AnnealingCriterion,
                    ExpTemperatureScheduler,
                    SecondProgressScheduler,
                    R,
                >
            {
                pub fn with_seconds(
                    start_temp: f64,
                    end_temp: f64,
                    seconds: f64,
                    is_maximize: bool,
                    rnd: R,
                ) -> AnnealerScheduler<
                    AnnealingCriterion,
                    ExpTemperatureScheduler,
                    SecondProgressScheduler,
                    R,
                > {
                    AnnealerScheduler::new(
                        AnnealingCriterion::new(is_maximize),
                        ExpTemperatureScheduler::new(start_temp, end_temp),
                        SecondProgressScheduler::new(seconds),
                        rnd,
                    )
                }
            }

            impl<R: Random>
                AnnealerScheduler<
                    AnnealingCriterion,
                    ExpTemperatureScheduler,
                    IterationProgressScheduler,
                    R,
                >
            {
                pub fn with_iterations(
                    start_temp: f64,
                    end_temp: f64,
                    iteration: usize,
                    is_maximize: bool,
                    rnd: R,
                ) -> AnnealerScheduler<
                    AnnealingCriterion,
                    ExpTemperatureScheduler,
                    IterationProgressScheduler,
                    R,
                > {
                    AnnealerScheduler::new(
                        AnnealingCriterion::new(is_maximize),
                        ExpTemperatureScheduler::new(start_temp, end_temp),
                        IterationProgressScheduler::new(iteration),
                        rnd,
                    )
                }
            }

            pub struct AnnealerSchedulerWithStatistics<C, T, P, R>
            where
                C: Criterion,
                T: TemperatureScheduler,
                P: ProgressScheduler,
                R: Random,
            {
                inner: AnnealerScheduler<C, T, P, R>,
                iteration: Vec<[usize; NUM_PHASE]>,
                adopted: Vec<[usize; NUM_PHASE]>,
            }

            impl<C, T, P, R> AnnealerSchedulerWithStatistics<C, T, P, R>
            where
                C: Criterion,
                T: TemperatureScheduler,
                P: ProgressScheduler,
                R: Random,
            {
                pub fn new(inner: AnnealerScheduler<C, T, P, R>, n_types: usize) -> Self {
                    assert!(n_types > 0, "n_types must be > 0");
                    Self {
                        inner,
                        iteration: vec![[0; NUM_PHASE]; n_types],
                        adopted: vec![[0; NUM_PHASE]; n_types],
                    }
                }

                pub fn to_next_iter(&mut self) -> bool {
                    self.inner.to_next_iter()
                }

                pub fn adopt(&mut self, t: usize, cur_score: f64, new_score: f64) -> bool {
                    // phase index: 0..=NUM_PHASE-1 based on current progress
                    let progress = self.inner.get_progress();
                    let phase = ((progress * NUM_PHASE as f64).floor() as usize).min(NUM_PHASE - 1);

                    self.iteration[t][phase] += 1;
                    let adopted = self.inner.adopt(cur_score, new_score);
                    if adopted {
                        self.adopted[t][phase] += 1;
                    }
                    adopted
                }

                #[inline]
                pub fn get_adopted(&self) -> &[[usize; NUM_PHASE]] {
                    &self.adopted
                }

                #[inline]
                pub fn get_iteration(&self) -> &[[usize; NUM_PHASE]] {
                    &self.iteration
                }

                #[inline]
                pub fn get_progress(&self) -> f64 {
                    self.inner.get_progress()
                }

                pub fn eprint_statistics(&self) {
                    let n = self.iteration.len();
                    let total_iter: usize = self
                        .iteration
                        .iter()
                        .map(|row| row.iter().sum::<usize>())
                        .sum();
                    let total_adopt: usize = self
                        .adopted
                        .iter()
                        .map(|row| row.iter().sum::<usize>())
                        .sum();
                    let total_rate = if total_iter == 0 {
                        0.0
                    } else {
                        (total_adopt as f64) / (total_iter as f64) * 100.0
                    };

                    eprintln!("=== Annealer statistics ===");
                    eprintln!("progress: {:.4}", self.get_progress());
                    eprintln!("types: {}", n);
                    eprintln!(
                        "{:>6} | {:>12} | {:>12} | {:>10}",
                        "type", "adopted", "iteration", "rate %"
                    );
                    eprintln!("{}", "-".repeat(6 + 3 + 12 + 3 + 12 + 3 + 10));
                    for t in 0..n {
                        let it: usize = self.iteration[t].iter().sum();
                        let ad: usize = self.adopted[t].iter().sum();
                        let rate = if it == 0 {
                            0.0
                        } else {
                            (ad as f64) / (it as f64) * 100.0
                        };
                        eprintln!("{:>6} | {:>12} | {:>12} | {:>10.2}", t, ad, it, rate);
                    }
                    eprintln!("{}", "-".repeat(6 + 3 + 12 + 3 + 12 + 3 + 10));
                    eprintln!(
                        "{:>6} | {:>12} | {:>12} | {:>10.2}",
                        "total", total_adopt, total_iter, total_rate
                    );

                    // Per-type x per-phase acceptance rate (%)
                    eprintln!("");
                    eprintln!("per-type per-phase acceptance rate (%)");
                    // header
                    {
                        use std::fmt::Write as _;
                        let mut header = String::new();
                        let _ = write!(&mut header, "{:>6} |", "type");
                        for p in 0..NUM_PHASE {
                            let _ = write!(&mut header, " {:>8}", format!("p{}", p));
                        }
                        let _ = write!(&mut header, " | {:>8}", "avg");
                        eprintln!("{}", header);
                    }
                    let sep_len = 6 + 3 + NUM_PHASE * 9 + 3 + 8;
                    eprintln!("{}", "-".repeat(sep_len));
                    for t in 0..n {
                        let mut avg_it = 0usize;
                        let mut avg_ad = 0usize;
                        let mut rates = vec![0.0f64; NUM_PHASE];
                        for p in 0..NUM_PHASE {
                            let it = self.iteration[t][p];
                            let ad = self.adopted[t][p];
                            avg_it += it;
                            avg_ad += ad;
                            rates[p] = if it == 0 {
                                0.0
                            } else {
                                (ad as f64) / (it as f64) * 100.0
                            };
                        }
                        let avg_rate = if avg_it == 0 {
                            0.0
                        } else {
                            (avg_ad as f64) / (avg_it as f64) * 100.0
                        };
                        use std::fmt::Write as _;
                        let mut line = String::new();
                        let _ = write!(&mut line, "{:>6} |", t);
                        for p in 0..NUM_PHASE {
                            let _ = write!(&mut line, " {:>8.2}", rates[p]);
                        }
                        let _ = write!(&mut line, " | {:>8.2}", avg_rate);
                        eprintln!("{}", line);
                    }

                    // Per-phase totals across all types
                    let mut phase_iter = [0usize; NUM_PHASE];
                    let mut phase_adopt = [0usize; NUM_PHASE];
                    for t in 0..n {
                        for p in 0..NUM_PHASE {
                            phase_iter[p] += self.iteration[t][p];
                            phase_adopt[p] += self.adopted[t][p];
                        }
                    }
                    eprintln!("");
                    eprintln!("per-phase totals:");
                    {
                        use std::fmt::Write as _;
                        let mut header = String::new();
                        let _ = write!(&mut header, "{:>6} |", "phase");
                        for p in 0..NUM_PHASE {
                            let _ = write!(&mut header, " {:>8}", format!("p{}", p));
                        }
                        let _ = write!(&mut header, " | {:>8}", "avg");
                        eprintln!("{}", header);
                    }
                    let sep_len = 6 + 3 + NUM_PHASE * 9 + 3 + 8;
                    eprintln!("{}", "-".repeat(sep_len));
                    let mut avg_it = 0usize;
                    let mut avg_ad = 0usize;
                    let mut rates = vec![0.0f64; NUM_PHASE];
                    for p in 0..NUM_PHASE {
                        let it = phase_iter[p];
                        let ad = phase_adopt[p];
                        avg_it += it;
                        avg_ad += ad;
                        rates[p] = if it == 0 {
                            0.0
                        } else {
                            (ad as f64) / (it as f64) * 100.0
                        };
                    }
                    let avg_rate = if avg_it == 0 {
                        0.0
                    } else {
                        (avg_ad as f64) / (avg_it as f64) * 100.0
                    };
                    use std::fmt::Write as _;
                    let mut line = String::new();
                    let _ = write!(&mut line, "{:>6} |", "total");
                    for p in 0..NUM_PHASE {
                        let _ = write!(&mut line, " {:>8.2}", rates[p]);
                    }
                    let _ = write!(&mut line, " | {:>8.2}", avg_rate);
                    eprintln!("{}", line);
                }
            }

            impl<R: Random>
                AnnealerSchedulerWithStatistics<
                    AnnealingCriterion,
                    ExpTemperatureScheduler,
                    SecondProgressScheduler,
                    R,
                >
            {
                pub fn with_seconds(
                    start_temp: f64,
                    end_temp: f64,
                    seconds: f64,
                    is_maximize: bool,
                    n_types: usize,
                    rnd: R,
                ) -> Self {
                    let inner = AnnealerScheduler::with_seconds(
                        start_temp,
                        end_temp,
                        seconds,
                        is_maximize,
                        rnd,
                    );
                    Self::new(inner, n_types)
                }
            }

            impl<R: Random>
                AnnealerSchedulerWithStatistics<
                    AnnealingCriterion,
                    ExpTemperatureScheduler,
                    IterationProgressScheduler,
                    R,
                >
            {
                pub fn with_iterations(
                    start_temp: f64,
                    end_temp: f64,
                    iteration: usize,
                    is_maximize: bool,
                    n_types: usize,
                    rnd: R,
                ) -> Self {
                    let inner = AnnealerScheduler::with_iterations(
                        start_temp,
                        end_temp,
                        iteration,
                        is_maximize,
                        rnd,
                    );
                    Self::new(inner, n_types)
                }
            }
        }

        pub mod types {
            use crate::ahc_library::utils::random::Random;

            pub trait State<E>: Clone
            where
                E: Env,
            {
                /// スコアを取得する関数
                fn get_score(&mut self, env: &E, progress: f64) -> f64;
            }

            pub trait Env {}

            pub trait NeighborHandler {
                type State: State<Self::Env>;
                type Env: Env;

                fn setup() -> Self;
                fn tag(&self) -> &'static str;
                fn apply<R: Random>(
                    &mut self,
                    state: &mut Self::State,
                    env: &Self::Env,
                    rnd: &mut R,
                ) -> bool;
                fn revert<R: Random>(
                    &mut self,
                    state: &mut Self::State,
                    env: &Self::Env,
                    rnd: &mut R,
                );
            }

            pub trait NeighborType {
                type H: NeighborHandler;
                fn setup(&self) -> Self::H;
            }

            pub trait NeighborGenerator<N>
            where
                N: NeighborType,
            {
                fn generate(&self, progress: f64, rnd: &mut impl Random) -> N::H;
            }

            pub trait Criterion {
                fn adopt(
                    &self,
                    cur_score: f64,
                    new_score: f64,
                    cur_temp: f64,
                    progress: f64,
                    rnd: &mut impl Random,
                ) -> bool;
            }

            pub trait TemperatureScheduler {
                fn get_temp(&self, progress: f64) -> f64;
            }

            pub trait ProgressScheduler {
                fn start(&mut self) {}
                fn step(&mut self) {}
                fn get_progress(&self) -> f64;
            }

            pub trait Callback<S, E>
            where
                S: State<E>,
                E: Env,
            {
                fn on_start(&mut self, _state: &mut S, _env: &E) {}
                fn on_before_step(
                    &mut self,
                    _step: usize,
                    _progress: f64,
                    _state: &mut S,
                    _env: &E,
                ) {
                }
                fn on_after_step(
                    &mut self,
                    _step: usize,
                    _progress: f64,
                    _state: &mut S,
                    _env: &E,
                ) {
                }
                fn on_finish(&mut self, _state: &mut S, _env: &E) {}
            }

            #[macro_export]
            macro_rules! neighbor_impl {
	    ($state_type:ident, $env_type:ident, $($variant:ident),+) => {
	        enum NeighborHandlerImpl {
	            $($variant($variant),)+
	        }

	        impl $crate::ahc_library::annealer::types::NeighborHandler for NeighborHandlerImpl {
	            type Env = $env_type;
	            type State = $state_type;

	            fn setup() -> Self {
	                unreachable!()
	            }

	            fn tag(&self) -> &'static str {
	                match self {
	                    $(Self::$variant(inner) => inner.tag(),)+
	                }
	            }

	            fn apply<R: Random>(&mut self, state: &mut $state_type, env: &$env_type, rnd: &mut R) -> bool {
	                match self {
	                    $(Self::$variant(inner) => inner.apply(state, env, rnd),)+
	                }
	            }

	            fn revert<R: Random>(&mut self, state: &mut $state_type, env: &$env_type, rnd: &mut R) {
	                match self {
	                    $(Self::$variant(inner) => inner.revert(state, env, rnd),)+
	                }
	            }
	        }

	        enum Neighbor {
	            $($variant,)+
	        }

	        impl $crate::ahc_library::annealer::types::NeighborType for Neighbor {
	            type H = NeighborHandlerImpl;

	            fn setup(&self) -> Self::H {
	                match self {
	                    $(Self::$variant => NeighborHandlerImpl::$variant($variant::setup()),)+
	                }
	            }
	        }
	    };
	}
        }

        pub mod prelude {
            pub use crate::ahc_library::annealer::{
                components::{
                    criterion::AnnealingCriterion, criterion::HillClimbingCriterion,
                    neighbor_generator::WeightedNeighborGenerator,
                    progress_scheduler::IterationProgressScheduler,
                    progress_scheduler::SecondProgressScheduler,
                    temperature_scheduler::ExpTemperatureScheduler,
                },
                runner::{Annealer, AnnealerConfig, AnnealerMode, Mutator},
                scheduler::AnnealerScheduler,
                types::{Env, State},
            };
            pub use crate::neighbor_impl;
        }
    }

    pub mod beamsearch {
        pub mod components {
            use crate::ahc_library::beamsearch::runner::BeamWidthPolicy;

            pub struct FixedBeamWidthPolicy {
                width: usize,
            }

            impl FixedBeamWidthPolicy {
                pub fn new(width: usize) -> Self {
                    FixedBeamWidthPolicy { width }
                }
            }

            impl BeamWidthPolicy for FixedBeamWidthPolicy {
                fn suggest_width(&self, _: f64, _: usize, _: usize) -> usize {
                    self.width
                }
            }

            pub trait BeamUnitEstimator {
                fn estimate_remain_unit(
                    &self,
                    time_progress: f64,
                    cur_beam_width: usize,
                    cur_turn: usize,
                    end_turn: usize,
                ) -> usize;
            }

            pub struct FixedBeamUnitEstimator;

            impl BeamUnitEstimator for FixedBeamUnitEstimator {
                fn estimate_remain_unit(
                    &self,
                    _: f64,
                    cur_beam_width: usize,
                    cur_turn: usize,
                    end_turn: usize,
                ) -> usize {
                    (end_turn - cur_turn.min(end_turn)) * cur_beam_width
                }
            }

            pub struct DynamicBeamWidthPolicy<E>
            where
                E: BeamUnitEstimator,
            {
                desired_time_sec: f64,
                consumed_unit: usize,
                initial_width: usize,
                min_width: usize,
                max_width: usize,
                unit_estimator: E,
            }

            impl<E> DynamicBeamWidthPolicy<E>
            where
                E: BeamUnitEstimator,
            {
                pub fn new(
                    desired_time_sec: f64,
                    initial_width: usize,
                    min_width: usize,
                    max_width: usize,
                    unit_estimator: E,
                ) -> Self {
                    DynamicBeamWidthPolicy {
                        desired_time_sec,
                        consumed_unit: 0,
                        initial_width,
                        min_width,
                        max_width,
                        unit_estimator,
                    }
                }
            }

            impl<E> DynamicBeamWidthPolicy<E>
            where
                E: BeamUnitEstimator,
            {
                pub fn estimate_remain_unit(
                    &self,
                    time_progress: f64,
                    cur_beam_width: usize,
                    cur_turn: usize,
                    end_turn: usize,
                ) -> usize {
                    self.unit_estimator.estimate_remain_unit(
                        time_progress,
                        cur_beam_width,
                        cur_turn,
                        end_turn,
                    )
                }
            }

            impl<E> BeamWidthPolicy for DynamicBeamWidthPolicy<E>
            where
                E: BeamUnitEstimator,
            {
                fn suggest_width(
                    &self,
                    time_progress: f64,
                    cur_turn: usize,
                    end_turn: usize,
                ) -> usize {
                    const E: f64 = 0.1;

                    if self.consumed_unit == 0 || time_progress < 1e-5 {
                        return self.initial_width;
                    };

                    let available_unit = (self.consumed_unit as f64 * (1. - time_progress)
                        / time_progress)
                        .floor() as usize;

                    // remain_unit * (1 + E) < available_unit を満たす最大の`beam_width`を求める
                    let width = {
                        let (mut l, mut r) = (self.min_width, self.max_width + 1);
                        while r - l > 1 {
                            let w = (r + l) / 2;
                            let remain_unit =
                                self.estimate_remain_unit(time_progress, w, cur_turn, end_turn);

                            if remain_unit as f64 * (1. + E) < available_unit as f64 {
                                l = w;
                            } else {
                                r = w;
                            }
                        }

                        l
                    };

                    width.clamp(self.min_width, self.max_width)
                }

                fn end_unit(&mut self) {
                    self.consumed_unit += 1;
                }
            }
        }

        pub mod runner {
            use std::collections::HashSet;

            use crate::ahc_library::utils::time;

            pub trait BeamWidthPolicy {
                fn suggest_width(
                    &self,
                    elapsed_sec: f64,
                    cur_turn: usize,
                    end_turn: usize,
                ) -> usize;
                fn start_unit(&mut self) {}
                fn end_unit(&mut self) {}
            }

            /// Delegate trait for beam search
            pub trait BeamSearchDelegate<S, M> {
                /// scoreとhashの評価（scoreは大きい方が良い）
                fn evaluate(&self, state: &mut S, material: &M) -> (f64, u64);
                /// 状態遷移関数
                fn transfer(&self, state: &S, material: &M) -> S;
            }

            enum BeamSearchRunnerStatus {
                NotStarted,
                InProgress(f64, usize),
                Finished,
            }

            /// Runner used for beam search process
            ///
            /// Usage:
            /// ```ignore
            /// let mut states = ...;
            ///
            /// let width_policy = FixedBeamWidthPolicy::new(10);
            /// let mut runner = BeamSearchRunner::new(1.5, 20, width_policy);
            ///
            /// while runner.to_next_turn() {
            ///     let mut transfer_materials = vec![];
            ///     for (state_i, state) in states.iter().enumerate() {
            ///         runner.start_unit();
            ///
            ///         // generate transfer materials
            ///
            ///         runner.end_unit();
            ///     }
            ///     states = runner.next_states(states, transfer_materials, &delegate);
            /// }
            /// ```
            pub struct BeamSearchRunner<W>
            where
                W: BeamWidthPolicy,
            {
                status: BeamSearchRunnerStatus,
                end_turn: usize,
                width_policy: W,
            }

            impl<W> BeamSearchRunner<W>
            where
                W: BeamWidthPolicy,
            {
                pub fn new(end_turn: usize, width_policy: W) -> Self {
                    BeamSearchRunner {
                        status: BeamSearchRunnerStatus::NotStarted,
                        end_turn,
                        width_policy,
                    }
                }

                pub fn run<S, M, G, D>(
                    &mut self,
                    states: Vec<S>,
                    gen_transfer_materials: G,
                    delegate: &D,
                ) -> Vec<S>
                where
                    G: Fn(usize, &S) -> Vec<M>,
                    D: BeamSearchDelegate<S, M>,
                {
                    let mut states = states;

                    while self.to_next_turn() {
                        let mut transfer_materials = vec![];
                        for (state_i, state) in states.iter().enumerate() {
                            self.start_unit();
                            transfer_materials.extend(
                                gen_transfer_materials(state_i, state)
                                    .into_iter()
                                    .map(|m| (state_i, m)),
                            );
                            self.end_unit();
                        }

                        states = self.next_states(states, transfer_materials, delegate);
                    }

                    states
                }

                /// 次の状態をビーム幅に基づいて選択して返す
                ///
                /// S: 状態の型
                /// M: 状態遷移に必要な素材の型
                /// D: BeamSearchDelegate
                pub fn next_states<S, M, D>(
                    &self,
                    mut states: Vec<S>,
                    transfer_materials: Vec<(usize, M)>,
                    delegate: &D,
                ) -> Vec<S>
                where
                    D: BeamSearchDelegate<S, M>,
                {
                    let (elapsed_sec, cur_turn) = self.get_progress();
                    let beam_width =
                        self.width_policy
                            .suggest_width(elapsed_sec, cur_turn, self.end_turn);

                    let mut cands = vec![];
                    for (i, (state_i, material)) in transfer_materials.iter().enumerate() {
                        let (score, hash) = delegate.evaluate(&mut states[*state_i], material);
                        cands.push((score, hash, i));
                    }

                    let mut set = HashSet::new();
                    cands.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap());
                    let cands = cands
                        .into_iter()
                        .filter_map(
                            |(_, hash, idx)| {
                                if set.insert(hash) {
                                    Some(idx)
                                } else {
                                    None
                                }
                            },
                        )
                        .take(beam_width)
                        .collect::<Vec<usize>>();

                    let mut next_states = Vec::new();
                    for idx in cands {
                        let (state_i, material) = &transfer_materials[idx];
                        let new_state = delegate.transfer(&states[*state_i], material);
                        next_states.push(new_state);
                    }

                    next_states
                }

                pub fn to_next_turn(&mut self) -> bool {
                    self.status = match self.status {
                        BeamSearchRunnerStatus::NotStarted => {
                            BeamSearchRunnerStatus::InProgress(time::elapsed_seconds(), 0)
                        }
                        BeamSearchRunnerStatus::InProgress(start_time, turn) => {
                            if turn + 1 >= self.end_turn {
                                BeamSearchRunnerStatus::Finished
                            } else {
                                BeamSearchRunnerStatus::InProgress(start_time, turn + 1)
                            }
                        }
                        BeamSearchRunnerStatus::Finished => BeamSearchRunnerStatus::Finished,
                    };
                    matches!(self.status, BeamSearchRunnerStatus::InProgress(_, _))
                }

                pub fn get_progress(&self) -> (f64, usize) {
                    let (start_time, cur_turn) = match self.status {
                        BeamSearchRunnerStatus::NotStarted => {
                            panic!("BeamSearchRunner must be started before get_progress");
                        }
                        BeamSearchRunnerStatus::InProgress(start_time, turn) => (start_time, turn),
                        BeamSearchRunnerStatus::Finished => {
                            return (1.0, self.end_turn);
                        }
                    };
                    let elapsed_sec = time::elapsed_seconds() - start_time;
                    (elapsed_sec, cur_turn)
                }

                pub fn start_unit(&mut self) {
                    self.width_policy.start_unit();
                }

                pub fn end_unit(&mut self) {
                    self.width_policy.end_unit();
                }
            }
        }
    }

    pub mod utils {
        pub mod fast_clear_array {
            use crate::ahc_library::utils::ndarray::{Array2d, Array3d};

            pub struct FastClearArray<T: Clone + Copy> {
                pub version: usize,
                pub values: Vec<(usize, T)>,
                pub init_value: T,
            }

            impl<T: Clone + Copy> FastClearArray<T> {
                pub fn new(n: usize, init_value: T) -> FastClearArray<T> {
                    FastClearArray {
                        version: 0,
                        values: vec![(!0, init_value); n],
                        init_value,
                    }
                }

                #[inline]
                pub fn get(&mut self, i: usize) -> T {
                    if self.values[i].0 != self.version {
                        self.values[i] = (self.version, self.init_value);
                    }
                    self.values[i].1
                }

                #[inline]
                pub fn set(&mut self, i: usize, new_value: T) {
                    self.values[i] = (self.version, new_value);
                }

                pub fn clear(&mut self) {
                    self.version += 1;
                }
            }

            #[derive(Clone, Debug)]
            pub struct FastClearArray2d<T: Clone + Copy> {
                pub version: usize,
                pub values: Array2d<(usize, T)>,
                pub init_value: T,
            }

            impl<T: Clone + Copy> FastClearArray2d<T> {
                pub fn new(h: usize, w: usize, init_value: T) -> FastClearArray2d<T> {
                    FastClearArray2d {
                        version: 0,
                        values: Array2d::new(vec![vec![(!0, init_value); w]; h]),
                        init_value,
                    }
                }

                #[inline]
                pub fn get(&mut self, c: &(usize, usize)) -> T {
                    if self.values[*c].0 != self.version {
                        self.values[*c] = (self.version, self.init_value);
                    }
                    self.values[*c].1
                }

                #[inline]
                pub fn set(&mut self, c: &(usize, usize), new_value: T) {
                    self.values[*c] = (self.version, new_value);
                }

                pub fn clear(&mut self) {
                    self.version += 1;
                }
            }

            #[derive(Clone, Debug)]
            pub struct FastClearArray3d<T: Clone + Copy> {
                pub version: usize,
                pub values: Array3d<(usize, T)>,
                pub init_value: T,
            }

            impl<T: Clone + Copy> FastClearArray3d<T> {
                pub fn new(d0: usize, d1: usize, d2: usize, init_value: T) -> FastClearArray3d<T> {
                    FastClearArray3d {
                        version: 0,
                        values: Array3d::init(d0, d1, d2, (0, init_value)),
                        init_value,
                    }
                }

                #[inline]
                pub fn get(&mut self, c: &(usize, usize, usize)) -> T {
                    if self.values[*c].0 != self.version {
                        self.values[*c] = (self.version, self.init_value);
                    }
                    self.values[*c].1
                }

                #[inline]
                pub fn set(&mut self, c: &(usize, usize, usize), new_value: T) {
                    self.values[*c] = (self.version, new_value);
                }

                pub fn clear(&mut self) {
                    self.version += 1;
                }
            }
        }

        pub mod index_set {
            use crate::ahc_library::utils::random::Random;

            #[derive(Debug, Clone)]
            pub struct IndexSet {
                pub que: Vec<usize>,
                pub pos: Vec<usize>,
            }

            impl IndexSet {
                pub fn empty(n: usize) -> Self {
                    IndexSet {
                        que: Vec::with_capacity(n),
                        pos: vec![!0; n],
                    }
                }

                pub fn full(n: usize) -> Self {
                    IndexSet {
                        que: (0..n).collect(),
                        pos: (0..n).collect(),
                    }
                }

                pub fn clear(&mut self) {
                    for &v in &self.que {
                        self.pos[v] = !0;
                    }
                    self.que.clear();
                }

                pub fn add(&mut self, v: usize) {
                    if self.contains(v) {
                        return;
                    }
                    self.pos[v] = self.que.len();
                    self.que.push(v);
                }

                pub fn remove(&mut self, v: usize) {
                    if !self.contains(v) {
                        return;
                    }

                    let p = self.pos[v];
                    let b = self.que[self.que.len() - 1];
                    self.que.swap_remove(p);
                    self.pos[b] = p;
                    self.pos[v] = !0;
                }

                pub fn contains(&self, v: usize) -> bool {
                    self.pos[v] != !0
                }

                pub fn size(&self) -> usize {
                    self.que.len()
                }

                pub fn get_first(&self) -> Option<usize> {
                    self.que.get(0).copied()
                }

                pub fn get_random(&self, rnd: &mut impl Random) -> Option<usize> {
                    self.que.get(rnd.gen_index(self.que.len())).copied()
                }

                pub fn iter(&self) -> impl Iterator<Item = &usize> {
                    self.que.iter()
                }
            }

            #[derive(Debug, Clone)]
            pub struct IndexMap<T> {
                set: IndexSet,
                vals: Vec<Option<T>>,
            }

            impl<T> IndexMap<T>
            where
                T: Clone + Copy + Default,
            {
                pub fn new(n: usize) -> Self {
                    IndexMap {
                        set: IndexSet::empty(n),
                        vals: vec![None; n],
                    }
                }

                pub fn add(&mut self, idx: usize, val: T) {
                    if !self.set.contains(idx) {
                        self.set.add(idx);
                    }
                    self.vals[idx] = Some(val);
                }

                pub fn remove(&mut self, idx: usize) {
                    if self.set.contains(idx) {
                        self.set.remove(idx);
                        self.vals[idx] = None;
                    }
                }

                pub fn get(&self, idx: usize) -> Option<T> {
                    self.vals[idx]
                }

                pub fn iter(&self) -> impl Iterator<Item = (usize, T)> + '_ {
                    self.set.iter().map(move |&idx| {
                        (idx, self.vals[idx].expect("Somehow IndexMap is invalid"))
                    })
                }
            }
        }

        pub mod ndarray {
            use std::ops::{Index, IndexMut};

            use super::v2::V2;

            #[derive(Clone, Debug)]
            pub struct ArrayRef2d<T>
            where
                T: Clone,
            {
                pub h: usize,
                pub w: usize,
                pub values: Vec<T>,
            }

            impl<T> ArrayRef2d<T>
            where
                T: Clone,
            {
                /// expect: values[i].len() = const.
                pub fn new(values: Vec<Vec<T>>) -> ArrayRef2d<T> {
                    let h = values.len();
                    let w = values[0].len();
                    let values = values.into_iter().flatten().collect();
                    ArrayRef2d { h, w, values }
                }

                pub fn init(h: usize, w: usize, init_value: T) -> ArrayRef2d<T> {
                    let values = vec![init_value; h * w];
                    ArrayRef2d { h, w, values }
                }

                #[inline]
                pub fn iter(&self) -> std::slice::Iter<'_, T> {
                    self.values.iter()
                }
            }

            impl<T> From<Vec<Vec<T>>> for ArrayRef2d<T>
            where
                T: Clone,
            {
                fn from(values: Vec<Vec<T>>) -> Self {
                    ArrayRef2d::new(values)
                }
            }

            impl<T> Index<V2<usize>> for ArrayRef2d<T>
            where
                T: Clone,
            {
                type Output = T;

                #[inline]
                fn index(&self, v: V2<usize>) -> &Self::Output {
                    &self.values[v.x * self.w + v.y]
                }
            }

            impl<T> IndexMut<V2<usize>> for ArrayRef2d<T>
            where
                T: Clone,
            {
                #[inline]
                fn index_mut(&mut self, v: V2<usize>) -> &mut Self::Output {
                    &mut self.values[v.x * self.w + v.y]
                }
            }

            impl<T> Index<(usize, usize)> for ArrayRef2d<T>
            where
                T: Clone,
            {
                type Output = T;

                #[inline]
                fn index(&self, (x, y): (usize, usize)) -> &Self::Output {
                    &self.values[x * self.w + y]
                }
            }

            impl<T> IndexMut<(usize, usize)> for ArrayRef2d<T>
            where
                T: Clone,
            {
                #[inline]
                fn index_mut(&mut self, (x, y): (usize, usize)) -> &mut Self::Output {
                    &mut self.values[x * self.w + y]
                }
            }

            /// A 2D array stored in a 1D Vec.
            /// The element type T must implement Clone and Copy.
            ///
            /// # Example
            /// ```
            /// use ahc_library::utils::ndarray::Array2d;
            /// let mut array = Array2d::new(vec![vec![1, 2, 3], vec![4, 5, 6]]);
            /// assert_eq!(array[(0, 0)], 1);
            /// assert_eq!(array[(1, 2)], 6);
            /// array[(0, 1)] = 10;
            /// assert_eq!(array[(0, 1)], 10);
            /// ```
            ///
            /// You can use V2<usize> as index as well:
            /// ```
            /// use ahc_library::utils::{ndarray::Array2d, v2::V2};
            /// let mut array = Array2d::new(vec![vec![1, 2, 3], vec![4, 5, 6]]);
            /// let v = V2::new(1, 0);
            /// assert_eq!(array[v], 4);
            /// array[v] = 20;
            /// assert_eq!(array[v], 20);
            /// ```
            #[derive(Clone, Debug)]
            pub struct Array2d<T>
            where
                T: Clone + Copy,
            {
                pub h: usize,
                pub w: usize,
                values: Vec<T>,
            }

            impl<T> Array2d<T>
            where
                T: Clone + Copy,
            {
                /// expect: values[i].len() = const.
                pub fn new(values: Vec<Vec<T>>) -> Array2d<T> {
                    let h = values.len();
                    let w = values[0].len();
                    let values = values.into_iter().flatten().collect();
                    Array2d { h, w, values }
                }

                pub fn init(h: usize, w: usize, init_value: T) -> Array2d<T> {
                    let values = vec![init_value; h * w];
                    Array2d { h, w, values }
                }

                #[inline]
                pub fn iter(&self) -> std::slice::Iter<'_, T> {
                    self.values.iter()
                }
            }

            impl<T> From<Vec<Vec<T>>> for Array2d<T>
            where
                T: Clone + Copy,
            {
                fn from(values: Vec<Vec<T>>) -> Self {
                    Array2d::new(values)
                }
            }

            impl<T> Index<V2<usize>> for Array2d<T>
            where
                T: Clone + Copy,
            {
                type Output = T;

                #[inline]
                fn index(&self, v: V2<usize>) -> &Self::Output {
                    &self.values[v.x * self.w + v.y]
                }
            }

            impl<T> IndexMut<V2<usize>> for Array2d<T>
            where
                T: Clone + Copy,
            {
                #[inline]
                fn index_mut(&mut self, v: V2<usize>) -> &mut Self::Output {
                    &mut self.values[v.x * self.w + v.y]
                }
            }

            impl<T> Index<(usize, usize)> for Array2d<T>
            where
                T: Clone + Copy,
            {
                type Output = T;

                #[inline]
                fn index(&self, (x, y): (usize, usize)) -> &Self::Output {
                    &self.values[x * self.w + y]
                }
            }

            impl<T> IndexMut<(usize, usize)> for Array2d<T>
            where
                T: Clone + Copy,
            {
                #[inline]
                fn index_mut(&mut self, (x, y): (usize, usize)) -> &mut Self::Output {
                    &mut self.values[x * self.w + y]
                }
            }

            #[derive(Clone, Debug)]
            pub struct Array3d<T>
            where
                T: Clone + Copy,
            {
                pub d0: usize,
                pub d1: usize,
                pub d2: usize,
                values: Vec<T>,
            }

            impl<T> Array3d<T>
            where
                T: Clone + Copy,
            {
                pub fn init(d0: usize, d1: usize, d2: usize, init_value: T) -> Array3d<T> {
                    let values = vec![init_value; d0 * d1 * d2];
                    Array3d { d0, d1, d2, values }
                }
            }

            impl<T> Index<(usize, usize, usize)> for Array3d<T>
            where
                T: Clone + Copy,
            {
                type Output = T;

                #[inline]
                fn index(&self, (x, y, z): (usize, usize, usize)) -> &Self::Output {
                    &self.values[x * self.d1 * self.d2 + y * self.d2 + z]
                }
            }

            impl<T> IndexMut<(usize, usize, usize)> for Array3d<T>
            where
                T: Clone + Copy,
            {
                #[inline]
                fn index_mut(&mut self, (x, y, z): (usize, usize, usize)) -> &mut Self::Output {
                    &mut self.values[x * self.d1 * self.d2 + y * self.d2 + z]
                }
            }
        }

        pub mod object_pool {
            use crate::ahc_library::utils::{index_set::IndexSet, random::Random};

            #[derive(Clone, Debug)]
            pub struct ObjectPool<T: Clone> {
                objects: Vec<Option<T>>,
                is_active: IndexSet,
                is_inactive: IndexSet,
            }

            impl<T: Clone> ObjectPool<T> {
                pub fn new(objects: Vec<T>, pool_size: usize) -> ObjectPool<T> {
                    let mut pool = ObjectPool {
                        is_active: IndexSet::empty(pool_size),
                        is_inactive: IndexSet::full(pool_size),
                        objects: vec![None; pool_size],
                    };
                    for obj in objects {
                        pool.add(obj);
                    }
                    pool
                }

                pub fn get_ref(&self, idx: usize) -> Option<&T> {
                    self.objects[idx].as_ref()
                }

                pub fn get_mut(&mut self, idx: usize) -> Option<&mut T> {
                    self.objects[idx].as_mut()
                }

                pub fn add(&mut self, group: T) -> Option<usize> {
                    let idx = self.is_inactive.get_first()?;
                    self.objects[idx] = Some(group);
                    self.is_inactive.remove(idx);
                    self.is_active.add(idx);
                    Some(idx)
                }

                pub fn remove(&mut self, idx: usize) -> Option<T> {
                    self.is_active.remove(idx);
                    self.is_inactive.add(idx);
                    self.objects[idx].take()
                }

                pub fn size(&self) -> usize {
                    self.is_active.size()
                }

                pub fn iter(&self) -> impl Iterator<Item = &T> {
                    self.is_active
                        .iter()
                        .map(move |&idx| self.get_ref(idx).expect("Somehow is_active is invalid"))
                }

                pub fn iter_idx(&self) -> impl Iterator<Item = &usize> {
                    self.is_active.iter()
                }

                pub fn get_random_active_index(&self, rnd: &mut impl Random) -> Option<usize> {
                    self.is_active.get_random(rnd)
                }
            }
        }

        pub mod param {
            #[macro_export]
            macro_rules! params_impl {
	    (
	        { $( $arg:ident : $arg_ty:ty ),* $(,)? },
	        { $( $pname:ident : $pty:ty ),* $(,)? },
	        [ $( $pat:pat => { $( $fname:ident : $fval:expr ),* $(,)? } ),+ $(,)? ]
	    ) => {
	        #[allow(non_snake_case, unused)]
	        #[derive(Debug, Clone)]
	        pub struct Params {
	            $( pub $pname: $pty, )*
	        }

	        impl Params {
	            pub fn load( $( $arg: $arg_ty ),* ) -> Self {
	                match ( $( $arg ),* ) {
	                    $(
	                        $pat => Self {
	                            $( $fname: $fval, )*
	                        },
	                    )*
	                }
	            }
	        }
	    };

	    (
	        $(
	            $name:ident: $type:ty = $default:expr
	        ),* $(,)?
	    ) => {
	        #[allow(non_snake_case, unused)]
	        #[derive(Debug, Clone)]
	        pub struct Params {
	            $(
	                pub $name: $type,
	            )*
	        }

	        impl Params {
	            fn load() -> Self {
	                Self {
	                    $(
	                        $name: std::env::var(stringify!($name))
	                            .ok()
	                            .map(|v| v.parse::<$type>().unwrap())
	                            .unwrap_or($default),
	                    )*
	                }
	            }
	        }
	    };
	}
        }

        pub mod path_finder {
            use std::collections::VecDeque;

            use crate::ahc_library::utils::{
                fast_clear_array::FastClearArray2d, ndarray::Array2d, random::Random, v2::V2,
            };

            pub trait PathFindState {
                fn trans(&self, u: &V2<usize>, path: &[V2<usize>]) -> Self;
            }

            #[derive(Clone, Copy)]
            pub struct DummyPathFindState;

            impl PathFindState for DummyPathFindState {
                fn trans(&self, _: &V2<usize>, _: &[V2<usize>]) -> Self {
                    *self
                }
            }

            pub struct BfsGridPathFinder<R: Random> {
                h: usize,
                w: usize,
                q: VecDeque<V2<usize>>,
                dist: FastClearArray2d<i32>,
                prev: FastClearArray2d<Option<V2<usize>>>,
                seen: Vec<V2<usize>>,
                rnd: R,
            }

            impl<R: Random> BfsGridPathFinder<R> {
                pub fn new(h: usize, w: usize, rnd: R) -> Self {
                    Self {
                        h,
                        w,
                        q: VecDeque::new(),
                        dist: FastClearArray2d::new(h, w, i32::MAX),
                        prev: FastClearArray2d::new(h, w, None),
                        seen: Vec::with_capacity(h * w),
                        rnd,
                    }
                }

                pub fn get_reachable_coors<T, D>(
                    &mut self,
                    start: &V2<usize>,
                    trans_cond: T,
                    priority_d: D,
                ) -> Vec<V2<usize>>
                where
                    T: Fn(&V2<usize>, &V2<usize>) -> bool,
                    D: Fn(usize, &V2<usize>, &mut R) -> V2<usize>,
                {
                    self._bfs(start, |_| false, trans_cond, priority_d);
                    self.seen.clone()
                }

                pub fn get_reachable_size<T, D>(
                    &mut self,
                    start: &V2<usize>,
                    trans_cond: T,
                    priority_d: D,
                ) -> usize
                where
                    T: Fn(&V2<usize>, &V2<usize>) -> bool,
                    D: Fn(usize, &V2<usize>, &mut R) -> V2<usize>,
                {
                    self._bfs(start, |_| false, trans_cond, priority_d);
                    self.seen.len()
                }

                /// 両端点を含む
                pub fn find_path<C, T, D>(
                    &mut self,
                    start: &V2<usize>,
                    complete_cond: C,
                    trans_cond: T,
                    priority_d: D,
                ) -> Option<Vec<V2<usize>>>
                where
                    C: Fn(&V2<usize>) -> bool,
                    T: Fn(&V2<usize>, &V2<usize>) -> bool,
                    D: Fn(usize, &V2<usize>, &mut R) -> V2<usize>,
                {
                    let v = self._bfs(start, complete_cond, trans_cond, priority_d)?;
                    Some(self.restore_path(start, &v))
                }

                fn _bfs<C, T, D>(
                    &mut self,
                    start: &V2<usize>,
                    complete_cond: C,
                    trans_cond: T,
                    priority_d: D,
                ) -> Option<V2<usize>>
                where
                    C: Fn(&V2<usize>) -> bool,
                    T: Fn(&V2<usize>, &V2<usize>) -> bool,
                    D: Fn(usize, &V2<usize>, &mut R) -> V2<usize>,
                {
                    self.reset();

                    self.dist.set(&(start.x, start.y), 0);
                    self.q.push_back(*start);
                    self.seen.push(*start);

                    while let Some(v) = self.q.pop_front() {
                        if complete_cond(&v) {
                            return Some(v);
                        }

                        let new_dist = self.dist.get(&(v.x, v.y)) + 1;
                        for i in 0..4 {
                            let d = priority_d(i, &v, &mut self.rnd);
                            let u = v + d;
                            if u.x < self.h
                                && u.y < self.w
                                && (trans_cond)(&u, &v)
                                && new_dist < self.dist.get(&(u.x, u.y))
                            {
                                self.q.push_back(u);

                                self.dist.set(&(u.x, u.y), new_dist);
                                self.prev.set(&(u.x, u.y), Some(v));
                                self.seen.push(u);
                            }
                        }
                    }

                    None
                }

                pub fn restore_path(
                    &mut self,
                    start: &V2<usize>,
                    end: &V2<usize>,
                ) -> Vec<V2<usize>> {
                    let mut path = vec![*end];
                    let mut cur = *end;
                    while let Some(p) = self.prev.get(&(cur.x, cur.y)) {
                        cur = p;
                        path.push(cur);
                    }
                    path.reverse();

                    assert_eq!(&cur, start);

                    path
                }

                pub fn get_dist_table(&mut self) -> Array2d<usize> {
                    let mut array2d = Array2d::init(self.h, self.w, 0);
                    for i in 0..self.h {
                        for j in 0..self.w {
                            array2d[(i, j)] = self.dist.get(&(i, j)) as usize;
                        }
                    }
                    array2d
                }

                fn reset(&mut self) {
                    self.dist.clear();
                    self.q.clear();
                    self.prev.clear();
                    self.seen.clear();
                }
            }
        }

        pub mod random {
            use rand_pcg::rand_core::{RngCore, SeedableRng};

            pub trait Random {
                fn new(seed: u32) -> Self;
                fn _next(&mut self) -> u32;

                #[inline(always)]
                fn nextf(&mut self) -> f64 {
                    self._next() as f64 / ((1u64 << 32) as f64)
                }

                #[inline(always)]
                fn choice<T: Clone + Copy>(&mut self, v: &[T]) -> T {
                    let idx = self.gen_index(v.len());
                    v[idx]
                }

                #[inline(always)]
                fn gen_index(&mut self, len: usize) -> usize {
                    debug_assert!(len as u64 <= 1 << 32);
                    ((len as u64 * self._next() as u64) >> 32) as usize
                }

                #[inline(always)]
                fn gen_range(&mut self, l: usize, r: usize) -> usize {
                    debug_assert!(l < r);
                    debug_assert!(r as u64 <= 1 << 32);
                    l + (((r - l) as u64 * self._next() as u64) >> 32) as usize
                }

                #[inline(always)]
                fn gen_rangef(&mut self, l: f64, r: f64) -> f64 {
                    debug_assert!(l <= r);
                    l + self._next() as f64 * ((r - l) / ((1u64 << 32) as f64))
                }

                #[inline(always)]
                fn shuffle<T>(&mut self, v: &mut [T]) {
                    let n = v.len();
                    for i in (1..n).rev() {
                        let j = self.gen_range(0, i + 1);
                        v.swap(i, j);
                    }
                }
            }

            #[derive(Debug, Clone, Copy)]
            pub struct XorShift32 {
                state: u32,
            }

            impl Random for XorShift32 {
                fn new(mut seed: u32) -> Self {
                    if seed == 0 {
                        seed = u32::MAX;
                    }
                    Self { state: seed }
                }

                #[inline(always)]
                fn _next(&mut self) -> u32 {
                    let mut x = self.state;
                    x ^= x << 13;
                    x ^= x >> 17;
                    x ^= x << 5;
                    self.state = x;
                    x
                }
            }

            #[derive(Debug, Clone)]
            pub struct RandPcg64Mcg {
                inner: rand_pcg::Pcg64Mcg,
            }

            impl Random for RandPcg64Mcg {
                fn new(seed: u32) -> Self {
                    Self {
                        inner: rand_pcg::Pcg64Mcg::seed_from_u64(seed as u64),
                    }
                }

                #[inline(always)]
                fn _next(&mut self) -> u32 {
                    self.inner.next_u32()
                }
            }

            pub trait RandomSampler<T> {
                fn sample(&mut self) -> T;
            }

            pub struct DiscreteSampler<T, R> {
                buf: Vec<T>,
                rnd: R,
            }

            impl<T: Copy, R: Random> DiscreteSampler<T, R> {
                pub fn new(weight_values: &Vec<(usize, T)>, rnd: R) -> Self {
                    let weight_sum = weight_values.iter().map(|(w, _)| *w).sum::<usize>();
                    assert!(0 < weight_sum);
                    assert!(weight_sum < 1_000_000);
                    let mut buf = Vec::with_capacity(weight_sum);
                    for &(w, val) in weight_values.iter() {
                        buf.extend(std::iter::repeat(val).take(w));
                    }
                    Self { buf, rnd }
                }
            }

            impl<T: Copy, R: Random> RandomSampler<T> for DiscreteSampler<T, R> {
                fn sample(&mut self) -> T {
                    self.rnd.choice(&self.buf)
                }
            }

            pub struct ContinousSampler<R: Random> {
                buf: Vec<f64>,
                rnd: R,
            }

            impl<R: Random> ContinousSampler<R> {
                pub fn new<F>(f: F, x_min: f64, x_max: f64, size: usize) -> Self
                where
                    F: Fn(f64) -> f64,
                {
                    assert!(0 < size);
                    assert!(size < 1_000_000);
                    let mut buf = Vec::with_capacity(size);
                    let step = (x_max - x_min) / (size as f64 - 1.);
                    for i in 0..size {
                        let x = x_min + step * (i as f64);
                        buf.push(f(x));
                    }
                    Self {
                        buf,
                        rnd: R::new(24),
                    }
                }
            }

            impl<R: Random> RandomSampler<f64> for ContinousSampler<R> {
                fn sample(&mut self) -> f64 {
                    self.rnd.choice(&self.buf)
                }
            }
        }

        pub mod time {
            static mut START: f64 = -1.;
            static mut R: f64 = 1.;

            #[allow(unused)]
            /// r - scaling factor for elapsed time
            pub fn start_clock(r: f64) {
                unsafe {
                    R = r;
                }
                let _ = elapsed_seconds();
            }

            #[inline]
            #[allow(unused)]
            pub fn elapsed_seconds() -> f64 {
                let t = std::time::SystemTime::now()
                    .duration_since(std::time::UNIX_EPOCH)
                    .unwrap()
                    .as_secs_f64();
                unsafe {
                    if START < 0. {
                        START = t;
                    }
                    (t - START) * R
                }
            }
        }

        pub mod topklist {
            /// https://atcoder.jp/contests/ahc052/submissions/68703195 より拝借
            use std::collections::BinaryHeap;

            #[derive(Clone, Debug)]
            struct Entry<K, V> {
                k: K,
                v: V,
            }

            impl<K: PartialOrd, V> Ord for Entry<K, V> {
                fn cmp(&self, other: &Self) -> std::cmp::Ordering {
                    self.k.partial_cmp(&other.k).unwrap()
                }
            }

            #[allow(clippy::non_canonical_partial_ord_impl)]
            impl<K: PartialOrd, V> PartialOrd for Entry<K, V> {
                fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
                    self.k.partial_cmp(&other.k)
                }
            }

            impl<K: PartialEq, V> PartialEq for Entry<K, V> {
                fn eq(&self, other: &Self) -> bool {
                    self.k.eq(&other.k)
                }
            }

            impl<K: PartialEq, V> Eq for Entry<K, V> {}

            #[derive(Clone, Debug)]
            pub struct BoundedSortedList<K: PartialOrd + Copy, V: Clone> {
                que: BinaryHeap<Entry<K, V>>,
                size: usize,
            }

            impl<K: PartialOrd + Copy, V: Clone> BoundedSortedList<K, V> {
                pub fn new(size: usize) -> Self {
                    Self {
                        que: BinaryHeap::with_capacity(size),
                        size,
                    }
                }

                pub fn can_insert(&self, k: K) -> bool {
                    self.que.len() < self.size || self.que.peek().unwrap().k > k
                }

                pub fn insert(&mut self, k: K, v: V) {
                    if self.que.len() < self.size {
                        self.que.push(Entry { k, v });
                    } else if let Some(mut top) = self.que.peek_mut() {
                        if top.k > k {
                            top.k = k;
                            top.v = v;
                        }
                    }
                }

                pub fn to_list(self) -> Vec<(K, V)> {
                    let v = self.que.into_sorted_vec();
                    v.into_iter().map(|e| (e.k, e.v)).collect()
                }

                pub fn len(&self) -> usize {
                    self.que.len()
                }

                pub fn is_empty(&self) -> bool {
                    self.que.is_empty()
                }
            }
        }

        pub mod v2 {
            use num_traits::{Num, Zero};
            use std::ops::{Add, AddAssign, Div, DivAssign, Mul, MulAssign, Sub, SubAssign};

            pub const D_DOWN: V2<usize> = V2 { x: 1, y: 0 };
            pub const D_UP: V2<usize> = V2 { x: !0, y: 0 };
            pub const D_LEFT: V2<usize> = V2 { x: 0, y: !0 };
            pub const D_RIGHT: V2<usize> = V2 { x: 0, y: 1 };

            pub const D4: [V2<usize>; 4] = [D_UP, D_DOWN, D_LEFT, D_RIGHT];

            /// 2D Vector that supports basic arithmetic operations.
            /// # Examples
            /// ```
            /// use ahc_library::utils::v2::V2;
            /// let v1 = V2::new(1, 2);
            /// let v2 = V2::new(3, 4);
            /// let v3 = v1 + v2;
            /// assert_eq!(v3.x, 4);
            /// assert_eq!(v3.y, 6);
            ///
            /// let mut v4 = V2::new(5, 6);
            /// v4 += 10;
            /// assert_eq!(v4.x, 15);
            /// assert_eq!(v4.y, 16);
            /// ```
            #[derive(Clone, Copy, Debug, PartialEq, Eq, PartialOrd, Ord)]
            pub struct V2<T>
            where
                T: Num,
            {
                pub x: T,
                pub y: T,
            }

            macro_rules! impl_bin_op {
	    ($Trait:ident, $method:ident, $op:tt) => {
	        impl<T> $Trait for V2<T>
	        where
	            T: Num + $Trait<Output = T>,
	        {
	            type Output = Self;
	            fn $method(self, other: Self) -> Self {
	                Self {
	                    x: self.x $op other.x,
	                    y: self.y $op other.y,
	                }
	            }
	        }

	        impl<T> $Trait<T> for V2<T>
	        where
	            T: Num + Copy + Add<Output = T> + Sub<Output = T> + Mul<Output = T> + Div<Output = T>,
	        {
	            type Output = Self;
	            fn $method(self, factor: T) -> Self {
	                Self {
	                    x: self.x $op factor,
	                    y: self.y $op factor,
	                }
	            }
	        }
	    };
	}

            macro_rules! impl_assign_op {
	    ($Trait:ident, $method:ident, $op:tt) => {
	        impl<T> $Trait for V2<T>
	        where
	            T: Num + $Trait,
	        {
	            fn $method(&mut self, other: Self) {
	                self.x $op other.x;
	                self.y $op other.y;
	            }
	        }

	        impl<T> $Trait<T> for V2<T>
	        where
	            T: Num + $Trait + Copy,
	        {
	            fn $method(&mut self, factor: T) {
	                self.x $op factor;
	                self.y $op factor;
	            }
	        }
	    };
	}

            impl_bin_op!(Add, add, +);
            impl_bin_op!(Sub, sub, -);
            impl_bin_op!(Mul, mul, *);
            impl_bin_op!(Div, div, /);

            impl_assign_op!(AddAssign, add_assign, +=);
            impl_assign_op!(SubAssign, sub_assign, -=);
            impl_assign_op!(MulAssign, mul_assign, *=);
            impl_assign_op!(DivAssign, div_assign, /=);

            impl<T> std::fmt::Display for V2<T>
            where
                T: Num + std::fmt::Display,
            {
                fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
                    write!(f, "({}, {})", self.x, self.y)
                }
            }

            impl<T> From<(T, T)> for V2<T>
            where
                T: Num,
            {
                fn from(tuple: (T, T)) -> Self {
                    Self {
                        x: tuple.0,
                        y: tuple.1,
                    }
                }
            }

            impl<T> From<&(T, T)> for V2<T>
            where
                T: Num + Copy,
            {
                fn from(tuple: &(T, T)) -> Self {
                    Self {
                        x: tuple.0,
                        y: tuple.1,
                    }
                }
            }

            impl<T> Into<(T, T)> for V2<T>
            where
                T: Num + Copy,
            {
                fn into(self) -> (T, T) {
                    (self.x, self.y)
                }
            }

            impl<T> Zero for V2<T>
            where
                T: Num,
            {
                fn zero() -> Self {
                    Self {
                        x: T::zero(),
                        y: T::zero(),
                    }
                }

                fn is_zero(&self) -> bool {
                    self.x.is_zero() && self.y.is_zero()
                }
            }

            impl<T> V2<T>
            where
                T: Num,
            {
                pub fn new(x: T, y: T) -> Self {
                    Self { x, y }
                }
            }
        }
    }
}

