pub mod def {
    pub const MAX_INVEST_LEVEL: usize = 20;
    pub const INF: f64 = 1e18;

    use crate::interactor::*;

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum Card {
        WorkSingle(i64),
        WorkAll(i64),
        CancelSingle,
        CancelAll,
        Invest,
        None,
    }

    #[derive(Debug, Clone, Copy)]
    pub struct Project {
        pub h: i64,
        pub v: i64,
    }

    impl Card {
        pub fn from_tw(t: usize, w: i64) -> Card {
            match t {
                0 => Card::WorkSingle(w),
                1 => Card::WorkAll(w),
                2 => Card::CancelSingle,
                3 => Card::CancelAll,
                4 => Card::Invest,
                5 => Card::None,
                _ => panic!("invalid card type: {t}"),
            }
        }

        pub fn to_t(&self) -> usize {
            match self {
                Card::WorkSingle(_) => 0,
                Card::WorkAll(_) => 1,
                Card::CancelSingle => 2,
                Card::CancelAll => 3,
                Card::Invest => 4,
                Card::None => panic!(),
            }
        }
    }

    pub struct Input {
        pub n: usize,
        pub m: usize,
        pub k: usize,
        pub t: usize,
    }

    #[derive(Clone, Copy)]
    pub struct Param {
        pub overflow_alpha_refill: f64,
        pub overflow_alpha: f64,
        pub overflow_alpha_all_refill: f64,
        pub overflow_alpha_all: f64,
        pub cancel_alpha_refill: f64,
        pub cancel_alpha: f64,
        pub cancel_alpha_all_refill: f64,
        pub cancel_alpha_all: f64,
        pub invest_limit: usize,
        pub cancel_limit: usize,
        pub invest_cost: i64,
        pub work_single_beta: f64,
    }

    #[derive(Debug, Clone)]
    pub struct State {
        pub last_invest_round: usize,
        pub invest_level: usize,
        pub score: i64,
        pub cards: Vec<Card>,
        pub projects: Vec<Project>,
    }

    impl State {
        pub fn read_status<I: Interactor>(
            &mut self,
            input: &Input,
            interactor: &mut I,
        ) -> Vec<(Card, i64)> {
            let (projects, score, new_cards) = interactor.read_status(input);
            self.projects = projects;
            self.score = score;
            new_cards
        }

        pub fn use_card<I: Interactor>(&mut self, use_card: usize, m: usize, interactor: &mut I) {
            interactor.output_c(use_card, m);
            if let Card::Invest = self.cards[use_card] {
                self.invest_level += 1;
            }
            self.cards[use_card] = Card::None;
        }

        pub fn refill_card<I: Interactor>(
            &mut self,
            selected_card: usize,
            new_cards: &Vec<(Card, i64)>,
            interactor: &mut I,
        ) {
            interactor.output_r(selected_card);
            let i = self.empty_card_index().unwrap();
            self.cards[i] = new_cards[selected_card].0;
        }

        pub fn empty_card_index(&self) -> Option<usize> {
            for i in 0..self.cards.len() {
                if let Card::None = self.cards[i] {
                    return Some(i);
                }
            }
            None
        }
    }

    pub struct Recorder {
        pub scores: Vec<i64>,
        pub invest_rounds: Vec<usize>,
        pub x: Vec<i64>,
    }

    impl Recorder {
        pub fn new() -> Recorder {
            Recorder {
                scores: vec![0],
                invest_rounds: vec![],
                x: vec![0; 5],
            }
        }
    }
}
pub mod interactor {
    use crate::def::*;
    use std::io::{Stdin, Write};

    use proconio::*;

    pub trait Interactor {
        fn output_c(&mut self, c: usize, m: usize);
        fn output_r(&mut self, r: usize);
        fn read_status(&mut self, input: &Input) -> (Vec<Project>, i64, Vec<(Card, i64)>);
    }

    pub struct IOInteractor {
        source: proconio::source::line::LineSource<std::io::BufReader<Stdin>>,
    }

    impl IOInteractor {
        pub fn new() -> IOInteractor {
            IOInteractor {
                source: proconio::source::line::LineSource::new(std::io::BufReader::new(
                    std::io::stdin(),
                )),
            }
        }

        pub fn read_input(&mut self) -> (Input, State) {
            input! {
                from &mut self.source,
                n: usize,
                m: usize,
                k: usize,
                t: usize,
                cards: [(usize, i64); n],
                hv: [(i64, i64); m],
            }
            let projects = hv.iter().copied().map(|(h, v)| Project { h, v }).collect();
            let cards = cards
                .iter()
                .copied()
                .map(|(t, w)| Card::from_tw(t, w))
                .collect();
            (
                Input { n, m, k, t },
                State {
                    last_invest_round: 0,
                    invest_level: 0,
                    score: 0,
                    cards,
                    projects,
                },
            )
        }

        fn flush(&self) {
            std::io::stdout().flush().unwrap();
        }
    }

    impl Interactor for IOInteractor {
        fn output_c(&mut self, c: usize, m: usize) {
            println!("{} {}", c, m);
            self.flush();
        }

        fn output_r(&mut self, r: usize) {
            println!("{}", r);
            self.flush();
        }

        fn read_status(&mut self, input: &Input) -> (Vec<Project>, i64, Vec<(Card, i64)>) {
            input! {
                from &mut self.source,
                hv: [(i64, i64); input.m],
                money: i64,
                twp: [(usize, i64, i64); input.k],
            }
            let projects = hv.iter().copied().map(|(h, v)| Project { h, v }).collect();
            let cards: Vec<(Card, i64)> = twp
                .iter()
                .copied()
                .map(|(t, w, p)| (Card::from_tw(t, w), p))
                .collect();
            (projects, money, cards)
        }
    }
}
pub mod simulator {
    use rand::{prelude::*, Rng};
    use rand_chacha::{rand_core::SeedableRng, ChaCha20Rng};
    use rand_distr::{Normal, WeightedIndex};

    use crate::{def::*, interactor::Interactor, solver::*, util::rnd};

    fn generate_project(rng: &mut ChaCha20Rng) -> Project {
        let b = rng.gen_range(2.0f64..=8.0);
        let h = 2.0f64.powf(b).round() as i64;
        let normal_dist = Normal::<f64>::new(b, 0.5).unwrap();
        let v = 2.0f64
            .powf(normal_dist.sample(rng).clamp(0.0, 10.0))
            .round() as i64;
        Project { h, v }
    }

    fn generate_card(rng: &mut ChaCha20Rng, m: usize, x: &Vec<i64>) -> (Card, i64) {
        let weighted_index = WeightedIndex::new(x).unwrap();
        let t = weighted_index.sample(rng);
        let mut w = 0;
        let mut p;
        match t {
            0 => {
                w = rng.gen_range(1i64..=50);
                let mu = w as f64;
                let normal_dist = Normal::<f64>::new(mu, mu / 3.0).unwrap();
                p = normal_dist.sample(rng).round() as i64;
                p = p.clamp(1, 10000);
            }
            1 => {
                w = rng.gen_range(1i64..=50);
                let mu = w as f64 * m as f64;
                let normal_dist = Normal::<f64>::new(mu, mu / 3.0).unwrap();
                p = normal_dist.sample(rng).round() as i64;
                p = p.clamp(1, 10000);
            }
            2 => p = rng.gen_range(0i64..=10),
            3 => p = rng.gen_range(0i64..=10),
            4 => p = rng.gen_range(200i64..=1000),
            _ => panic!(),
        }

        (Card::from_tw(t, w), p)
    }

    pub struct MockInteractor {
        t: usize,
        score: i64,
        invest_level: usize,
        cards: Vec<Card>,
        projects: Vec<Project>,
        used_card: usize,
        candidate_cards: Vec<(Card, i64)>,
        new_projects: Vec<Project>,
        new_cards: Vec<Vec<(Card, i64)>>,
    }

    impl MockInteractor {
        pub fn new(
            x: &Vec<i64>,
            t: usize,
            state: &State,
            used_card: usize,
            candidate_cards: Vec<(Card, i64)>,
        ) -> MockInteractor {
            let mut rng = ChaCha20Rng::seed_from_u64(rnd::gen_range(0, 10000000000) as u64);
            let mut new_projects = vec![];
            let remain_t = 1005 - t;
            for _ in 0..state.projects.len() * remain_t {
                new_projects.push(generate_project(&mut rng));
            }
            let mut new_cards = vec![vec![]; remain_t];
            for i in 0..remain_t {
                new_cards[i].push((Card::WorkSingle(1), 0));
                for _ in 1..state.cards.len() {
                    new_cards[i].push(generate_card(&mut rng, state.projects.len(), &x));
                }
            }

            MockInteractor {
                t: 0,
                score: state.score,
                invest_level: state.invest_level,
                cards: state.cards.clone(),
                projects: state.projects.clone(),
                used_card,
                candidate_cards,
                new_projects,
                new_cards,
            }
        }

        fn get_card_candidate(&mut self) -> Vec<(Card, i64)> {
            let mut cards = self.new_cards[self.t].clone();
            for card in cards.iter_mut() {
                match &mut card.0 {
                    Card::WorkSingle(w) => *w *= 1 << self.invest_level,
                    Card::WorkAll(w) => *w *= 1 << self.invest_level,
                    _ => {}
                }
                card.1 *= 1 << self.invest_level;
            }
            self.t += 1;
            cards
        }
        fn update_project(&mut self, m: usize) {
            let project = self.new_projects.pop().unwrap();
            self.projects[m] = Project {
                h: project.h * (1 << self.invest_level),
                v: project.v * (1 << self.invest_level),
            };
        }

        fn work_project(&mut self, m: usize, w: i64) {
            self.projects[m].h -= w;
            if self.projects[m].h <= 0 {
                self.score += self.projects[m].v;
                self.update_project(m);
            }
        }
    }

    impl Interactor for MockInteractor {
        fn output_c(&mut self, c: usize, m: usize) {
            match self.cards[c] {
                Card::WorkSingle(w) => {
                    self.work_project(m, w);
                }
                Card::WorkAll(w) => {
                    for i in 0..self.projects.len() {
                        self.work_project(i, w);
                    }
                }
                Card::CancelSingle => self.update_project(m),
                Card::CancelAll => {
                    for i in 0..self.projects.len() {
                        self.update_project(i);
                    }
                }
                Card::Invest => self.invest_level += 1,
                _ => panic!(),
            }
            self.used_card = c;
        }

        fn output_r(&mut self, r: usize) {
            self.score -= self.candidate_cards[r].1;
            self.cards[self.used_card] = self.candidate_cards[r].0;
        }

        #[allow(unused)]
        fn read_status(
            &mut self,
            input: &Input,
        ) -> (Vec<crate::def::Project>, i64, Vec<(crate::def::Card, i64)>) {
            self.candidate_cards = self.get_card_candidate();
            (
                self.projects.clone(),
                self.score,
                self.candidate_cards.clone(),
            )
        }
    }

    pub fn montecarlo(
        rounds: usize,
        cur_state: &State,
        param: &Param,
        input: &Input,
        cur_t: usize,
        x: &Vec<i64>,
        refill_first: bool,
        new_select_card: usize,
        new_cards: &Vec<(Card, i64)>,
    ) -> i64 {
        let mut score_sum = 0;

        for _ in 0..rounds {
            let mut solver = Solver {
                param: param.clone(),
                state: cur_state.clone(),
            };
            let mut mock_interactor = MockInteractor::new(
                x,
                cur_t,
                &solver.state,
                solver.state.empty_card_index().unwrap_or(0),
                new_cards.clone(),
            );

            // 最初のrefillを固定する場合
            if refill_first {
                solver
                    .state
                    .refill_card(new_select_card, new_cards, &mut mock_interactor);
            }
            let start_t = if refill_first { cur_t + 1 } else { cur_t };
            for t in start_t..input.t {
                // 今持っているカードを見て、使うカードを決める
                let (select_card, m) = solver.select_use_card(t);

                if solver.state.cards[select_card] == Card::Invest {
                    solver.state.last_invest_round = t;
                }
                solver.state.use_card(select_card, m, &mut mock_interactor);
                let new_cards = solver.state.read_status(input, &mut mock_interactor);

                // 新しいカードを見て、補充するカードを決める
                let new_card = if t < input.t - 1 {
                    solver.select_new_card(&new_cards, t)
                } else {
                    0
                };
                solver
                    .state
                    .refill_card(new_card, &new_cards, &mut mock_interactor);
            }
            score_sum += solver.state.score;
        }
        score_sum / rounds as i64
    }
}
pub mod solver {
    use crate::def::*;
    use crate::interactor::*;
    use crate::simulator::*;

    pub struct Solver {
        pub state: State,
        pub param: Param,
    }

    impl Solver {
        pub fn remain_w(&self, t: usize, p: i64) -> i64 {
            (999 - t as i64) * 2_i64.pow(self.state.invest_level as u32) + (self.state.score - p)
        }

        pub fn solve(&mut self, input: &Input, interactor: &mut IOInteractor) {
            let mut recorder = Recorder::new();

            for t in 0..input.t {
                // 今持っているカードを見て、使うカードを決める
                let (select_card, m) = self.select_use_card(t);

                if self.state.cards[select_card] == Card::Invest {
                    self.state.last_invest_round = t;
                }
                self.state.use_card(select_card, m, interactor);
                recorder.scores.push(self.state.score);

                let new_cards = self.state.read_status(input, interactor);
                for (card, _) in new_cards.iter() {
                    recorder.x[card.to_t()] += 1;
                }

                // 新しいカードを見て、補充するカードを決める
                const MONTE_CARLO_ROUND: usize = 100;
                let new_card = if t < 990 {
                    self.select_new_card(&new_cards, t)
                } else if t < input.t - 1 {
                    (0..new_cards.len())
                        .max_by_key(|&i| {
                            if new_cards[i].1 <= self.state.score {
                                montecarlo(
                                    MONTE_CARLO_ROUND,
                                    &self.state,
                                    &self.param,
                                    input,
                                    t,
                                    &recorder.x,
                                    true,
                                    i,
                                    &new_cards,
                                )
                            } else {
                                -1
                            }
                        })
                        .unwrap()
                } else {
                    0
                };
                // モンテカルロしない場合
                // let new_card = if t < input.t - 1 {
                //     self.select_new_card(&new_cards, t)
                // } else {
                //     0
                // };
                if new_cards[new_card].0 == Card::Invest {
                    recorder.invest_rounds.push(t);
                }
                self.state.refill_card(new_card, &new_cards, interactor);
            }

            // ビジュアライズ用
            if cfg!(feature = "local") {
                use std::io::Write;
                let mut file = std::fs::File::create("score.log").unwrap();
                writeln!(&mut file, "{:?}", recorder.scores).unwrap();
                writeln!(&mut file, "{:?}", recorder.invest_rounds).unwrap();
            }
        }

        pub fn eval(&self, card: &Card, p: i64, t: usize, refill: bool) -> (f64, usize) {
            if p > self.state.score {
                return (-INF, 0);
            }

            let overflow_alpha = if refill {
                self.param.overflow_alpha_refill
            } else {
                self.param.overflow_alpha
            };
            let overflow_alpha_all = if refill {
                self.param.overflow_alpha_all_refill
            } else {
                self.param.overflow_alpha_all
            };
            let cancel_alpha = if refill {
                self.param.cancel_alpha_refill
            } else {
                self.param.cancel_alpha
            };
            let cancel_alpha_all = if refill {
                self.param.cancel_alpha_all_refill
            } else {
                self.param.cancel_alpha_all
            };

            match card {
                Card::WorkSingle(w) => {
                    let m = (0..self.state.projects.len())
                        .max_by_key(|&i| {
                            if self.state.projects[i].h > w + self.remain_w(t, p) {
                                return -INF as i64 - self.state.projects[i].h;
                            }
                            (((*w as f64 / self.state.projects[i].h as f64)
                                .min(1.)
                                .powf(self.param.work_single_beta)
                                * self.state.projects[i].v as f64
                                - ((w - self.state.projects[i].h).max(0) as f64))
                                * 10000.) as i64
                        })
                        .unwrap();
                    if self.state.projects[m].h > w + self.remain_w(t, p) && p > 0 {
                        return (-INF, m);
                    }
                    let eval = *w as f64
                        - p as f64
                        - ((w - self.state.projects[m].h).max(0) as f64) * overflow_alpha;
                    (eval, m)
                }
                Card::WorkAll(w) => {
                    let mut projects = self.state.projects.clone();
                    projects.sort_by_key(|p| p.h);
                    let feasible_proj_count = {
                        let mut c = 0;
                        let mut remain_w = self.remain_w(t, p);
                        for p in projects {
                            if p.h - w <= remain_w {
                                remain_w -= p.h - w; // NOTE: max(0)を取るのが正しいが、取らない方がスコアが良い
                                c += 1;
                            }
                        }
                        c
                    };
                    let w_sum = (*w * feasible_proj_count) as f64;
                    let eval = w_sum
                        - p as f64
                        - (self
                            .state
                            .projects
                            .iter()
                            .map(|proj| (*w as f64 - proj.h as f64).max(0.) * overflow_alpha_all)
                            .sum::<f64>());
                    (eval, 0)
                }
                Card::CancelSingle => {
                    let m = (0..self.state.projects.len())
                        .max_by_key(|&i| {
                            ((self.state.projects[i].h as f64 - self.state.projects[i].v as f64)
                                / self.state.projects[i].h as f64 // TODO: 消す？
                                * 10000.)
                                .round() as i64
                        })
                        .unwrap();
                    if t >= self.param.cancel_limit {
                        return (-INF, m);
                    }
                    let eval = self.state.projects[m].h as f64 * cancel_alpha
                        - self.state.projects[m].v as f64
                        - p as f64;
                    (eval, m)
                }
                Card::CancelAll => {
                    if t >= self.param.cancel_limit {
                        return (-INF, 0);
                    }
                    let eval = self
                        .state
                        .projects
                        .iter()
                        .map(|proj| proj.h as f64 * cancel_alpha_all - proj.v as f64)
                        .sum::<f64>()
                        - p as f64;
                    (eval, 0)
                }
                Card::Invest => {
                    if self.state.invest_level >= MAX_INVEST_LEVEL {
                        return (-INF, 0);
                    }
                    let invest_card_count = self
                        .state
                        .cards
                        .iter()
                        .filter(|&&card| card == Card::Invest)
                        .count();

                    // 1. 手持ちのカードが全て増資になった場合
                    // 2. 増資の期限が来た場合
                    // 3. 前回増資した場合（増資カードを消費している場合）
                    // 4. 増資回数がMAX_INVEST_LEVELに到達する場合
                    if self.state.cards.len() == invest_card_count
                        || ((t >= self.param.invest_limit || self.state.last_invest_round + 1 == t)
                            && p == 0)
                        || invest_card_count + self.state.invest_level == MAX_INVEST_LEVEL
                    {
                        return (INF, 0);
                    }
                    (-INF, 0)
                }
                Card::None => (-INF, 0),
            }
        }

        pub fn eval_refill(&self, card: &Card, p: i64, t: usize) -> f64 {
            if p > self.state.score {
                return -INF;
            }

            match card {
                Card::Invest => {
                    if self.state.invest_level >= MAX_INVEST_LEVEL || t >= self.param.invest_limit {
                        return -INF;
                    }
                    let invest_card_count = self
                        .state
                        .cards
                        .iter()
                        .filter(|&&card| card == Card::Invest)
                        .count();
                    if (self.state.score as f64 >= p as f64 * 1.5
                        && p / 2_i64.pow(self.state.invest_level as u32) < self.param.invest_cost)
                        || invest_card_count == self.state.cards.len() - 1
                        || self.state.invest_level == MAX_INVEST_LEVEL - 1
                    {
                        INF
                    } else {
                        -INF
                    }
                }
                _ => self.eval(card, p, t, true).0,
            }
        }

        pub fn select_new_card(&self, new_cards: &Vec<(Card, i64)>, t: usize) -> usize {
            let eval_refills: Vec<f64> = new_cards
                .iter()
                .map(|(card, p)| self.eval_refill(&card, *p, t))
                .collect();
            let mut card_idx = (0..new_cards.len()).collect::<Vec<usize>>();
            card_idx.sort_by(|i, j| eval_refills[*j].partial_cmp(&eval_refills[*i]).unwrap());

            // if cfg!(feature = "local") {
            //     println!("# eval_refill, m, card_type, p");
            //     for &i in card_idx.iter() {
            //         println!(
            //             "# {} {:.3} {:?}, {}",
            //             i, eval_refills[i], new_cards[i].0, new_cards[i].1
            //         );
            //     }
            // }

            card_idx[0]
        }

        pub fn select_use_card(&self, t: usize) -> (usize, usize) {
            let evals: Vec<(f64, usize)> = self
                .state
                .cards
                .iter()
                .map(|card| self.eval(&card, 0, t, false))
                .collect();

            let mut card_idx = (0..self.state.cards.len()).collect::<Vec<usize>>();
            card_idx.sort_by(|i, j| evals[*j].partial_cmp(&evals[*i]).unwrap());

            // if cfg!(feature = "local") {
            //     println!("# eval, m, card_type, p");
            //     for &i in card_idx.iter() {
            //         println!(
            //             "# {} {:.3} {} {:?}",
            //             i, evals[i].0, evals[i].1, self.state.cards[i]
            //         );
            //     }
            // }

            (card_idx[0], evals[card_idx[0]].1)
        }
    }
}
pub mod util {
    #![allow(unused)]

    use std::cmp::Ordering;

    use crate::def::*;

    pub mod rnd {
        static mut S: usize = 88172645463325252;

        #[inline]
        pub fn next() -> usize {
            unsafe {
                S = S ^ S << 7;
                S = S ^ S >> 9;
                S
            }
        }

        #[inline]
        pub fn nextf() -> f64 {
            (next() & 4294967295) as f64 / 4294967296.
        }

        #[inline]
        pub fn gen_range(low: usize, high: usize) -> usize {
            assert!(low < high);
            (next() % (high - low)) + low
        }

        pub fn shuffle<I>(vec: &mut Vec<I>) {
            for i in 0..vec.len() {
                let j = gen_range(0, vec.len());
                vec.swap(i, j);
            }
        }
    }

    pub mod time {
        static mut START: f64 = -1.;
        pub fn start_clock() {
            let _ = elapsed_seconds();
        }

        #[inline]
        pub fn elapsed_seconds() -> f64 {
            let t = std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs_f64();
            unsafe {
                if START < 0. {
                    START = t;
                }
                t - START
            }
        }
    }
}

use crate::def::*;
use crate::interactor::*;
use crate::solver::*;
use crate::util::*;

fn load_params() -> Param {
    if false {
        use std::env;
        let args: Vec<String> = env::args().collect();
        Param {
            overflow_alpha_refill: args[1].parse().unwrap(),
            overflow_alpha: args[2].parse().unwrap(),
            cancel_alpha_refill: args[3].parse().unwrap(),
            cancel_alpha: args[4].parse().unwrap(),
            invest_limit: args[5].parse().unwrap(),
            cancel_limit: args[6].parse().unwrap(),
            invest_cost: args[7].parse().unwrap(),
            overflow_alpha_all_refill: args[8].parse().unwrap(),
            overflow_alpha_all: args[9].parse().unwrap(),
            cancel_alpha_all_refill: args[10].parse().unwrap(),
            cancel_alpha_all: args[11].parse().unwrap(),
            work_single_beta: args[12].parse().unwrap(),
        }
    } else {
        Param {
            cancel_alpha: 3.9025157664873658,
            cancel_alpha_all: 5.034673414949709,
            cancel_alpha_all_refill: 1.070299470579473,
            cancel_alpha_refill: 1.0009104398160726,
            cancel_limit: 979,
            invest_cost: 510,
            invest_limit: 863,
            overflow_alpha: 2.016646721749814,
            overflow_alpha_all: 2.7694849713061416,
            overflow_alpha_all_refill: 0.5531191229327318,
            overflow_alpha_refill: 0.25378716369770815,
            work_single_beta: 2.898910615818517,
        }
    }
}

fn main() {
    time::start_clock();
    let mut interactor = IOInteractor::new();
    let (input, state) = interactor.read_input();
    let param = load_params();
    let mut solver = Solver { state, param };

    solver.solve(&input, &mut interactor);
    eprintln!(
        "result: {{\"score\": {}, \"duration\": {:.4}, \"invest_level\": {}}}",
        solver.state.score,
        time::elapsed_seconds(),
        solver.state.invest_level,
    );
}
