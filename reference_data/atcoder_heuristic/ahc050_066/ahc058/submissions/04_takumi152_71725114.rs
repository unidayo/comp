mod lib {}

mod solver {
    use std::{
        io::{self, BufReader},
        time::Instant,
    };

    use proconio::{input, source::line::LineSource};
    use rand_core::SeedableRng;
    use rand_pcg::Pcg64Mcg;

    use crate::lib::*;

    pub struct HyperParameter {
        pub sa1_time_limit: f64,
        pub sa1_base_temperature: f64,
        pub sa1_target_temperature: f64,
        pub sa2_base_temperature: f64,
        pub sa2_target_temperature: f64,
    }
    impl HyperParameter {
        pub fn new() -> Self {
            let sa1_time_limit = match std::env::var("AHC_SA1_TIME_LIMIT") {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.8,
            };
            let sa1_base_temperature = match std::env::var("AHC_SA1_BASE_TEMPERATURE") {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.027345264988184705,
            };
            let sa1_target_temperature = match std::env::var("AHC_SA1_TARGET_TEMPERATURE") {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.006588196860347772,
            };
            let sa2_base_temperature = match std::env::var("AHC_SA2_BASE_TEMPERATURE") {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.008239073303456818,
            };
            let sa2_target_temperature = match std::env::var("AHC_SA2_TARGET_TEMPERATURE") {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 2.4505556884336865e-07,
            };
            Self {
                sa1_time_limit,
                sa1_base_temperature,
                sa1_target_temperature,
                sa2_base_temperature,
                sa2_target_temperature,
            }
        }
    }

    pub struct Input {
        pub machine_type_num: usize,
        pub machine_level_max: usize,
        pub turn_limit: usize,
        pub initial_money: i64,
        pub machine_base_output: Vec<i64>,
        pub machine_base_cost: Vec<Vec<i64>>,
    }

    impl Input {
        pub fn from_stdin() -> Self {
            // magic
            let mut stdin = LineSource::new(BufReader::new(io::stdin()));
            macro_rules! input(($($tt:tt)*) => (proconio::input!(from &mut stdin, $($tt)*)));

            input! {
                machine_type_num: usize,
                machine_level_max: usize,
                turn_limit: usize,
                initial_money: i64,
                machine_base_output: [i64; machine_type_num as usize],
                machine_base_cost: [[i64; machine_type_num as usize]; machine_level_max as usize],
            }

            Self {
                machine_type_num,
                machine_level_max,
                turn_limit,
                initial_money,
                machine_base_output,
                machine_base_cost,
            }
        }
    }

    #[derive(Clone)]
    pub struct Env {
        pub time_begin: Instant,

        pub machine_type_num: usize,
        pub machine_level_max: usize,
        pub turn_limit: usize,
        pub initial_money: i64,
        pub machine_base_output: Vec<i64>,
        pub machine_base_cost: Vec<Vec<i64>>,
    }

    impl Env {
        pub fn new(input: &Input) -> Self {
            let time_begin = Instant::now();

            Self {
                time_begin,
                machine_type_num: input.machine_type_num,
                machine_level_max: input.machine_level_max,
                turn_limit: input.turn_limit,
                initial_money: input.initial_money,
                machine_base_output: input.machine_base_output.clone(),
                machine_base_cost: input.machine_base_cost.clone(),
            }
        }
    }

    #[derive(Clone)]
    pub struct Action {
        pub upgrade_target_level: i32,
        pub upgrade_target_id: i32,
    }

    impl Action {
        pub fn new(upgrade_target_level: i32, upgrade_target_id: i32) -> Self {
            Self {
                upgrade_target_level,
                upgrade_target_id,
            }
        }
    }

    #[derive(Clone)]
    pub struct Output {
        pub actions: Vec<Action>,
    }

    impl Output {
        pub fn new() -> Self {
            Self {
                actions: Vec::new(),
            }
        }

        pub fn output(&self) {
            for action in &self.actions {
                if action.upgrade_target_level == -1 {
                    println!("-1");
                } else {
                    println!(
                        "{} {}",
                        action.upgrade_target_level, action.upgrade_target_id
                    );
                }
            }
        }
    }

    mod sa_solver1 {
        use core::time;

        use rand::Rng;

        use super::*;

        struct SimBuffer {
            counts: Vec<i64>,
            powers: Vec<i64>,
            active_by_level: Vec<Vec<usize>>,
        }

        impl SimBuffer {
            fn new(env: &Env) -> Self {
                let size = env.machine_level_max * env.machine_type_num;
                Self {
                    counts: vec![1; size],
                    powers: vec![0; size],
                    active_by_level: vec![Vec::new(); env.machine_level_max],
                }
            }

            #[inline]
            fn reset(&mut self) {
                self.counts.fill(1);
                self.powers.fill(0);
                for active in &mut self.active_by_level {
                    active.clear();
                }
            }
        }

        struct State {
            pub upgrade_order: Vec<i32>,
        }

        impl State {
            pub fn new(env: &Env) -> Self {
                let mut upgrade_order = Vec::with_capacity(env.turn_limit);
                for _ in 0..env.turn_limit {
                    upgrade_order.push(-1);
                }
                Self { upgrade_order }
            }

            pub fn simulate_full(&self, env: &Env, sim_buf: &mut SimBuffer) -> f64 {
                sim_buf.reset();

                let mut apples: i64 = env.initial_money as i64;
                let counts = &mut sim_buf.counts;
                let powers = &mut sim_buf.powers;
                let active_by_level = &mut sim_buf.active_by_level;
                let level_max = env.machine_level_max;
                let type_num = env.machine_type_num;
                let base_output = &env.machine_base_output;
                let base_cost = &env.machine_base_cost;

                let mut upgrade_idx = 0;
                for _ in 0..env.turn_limit {
                    if upgrade_idx < self.upgrade_order.len() {
                        let target_id = self.upgrade_order[upgrade_idx];
                        if target_id >= 0 && target_id < type_num as i32 {
                            let j = target_id as usize;
                            let mut zero_power_candidate = None;
                            let mut highest_affordable = None;
                            for i in 0..level_max {
                                let idx = i * type_num + j;
                                let cost = base_cost[i][j] as i64 * (powers[idx] + 1);
                                if apples >= cost {
                                    if powers[idx] == 0 && zero_power_candidate.is_none() {
                                        zero_power_candidate = Some((i, cost));
                                    }
                                    highest_affordable = Some((i, cost));
                                }
                            }
                            if let Some((i, cost)) = zero_power_candidate.or(highest_affordable) {
                                apples -= cost;
                                let idx = i * type_num + j;
                                if powers[idx] == 0 {
                                    active_by_level[i].push(idx);
                                }
                                powers[idx] += 1;
                                upgrade_idx += 1;
                            }
                        }
                    }

                    // 生産フェーズ。Level 0 から順に処理する。
                    for i in 0..level_max {
                        for &idx in &active_by_level[i] {
                            let power = powers[idx];
                            if i == 0 {
                                let j = idx - i * type_num;
                                apples += base_output[j] as i64 * counts[idx] * power;
                            } else {
                                let prev_idx = idx - type_num;
                                counts[prev_idx] += counts[idx] * power;
                            }
                        }
                    }
                }

                f64::log2(apples as f64)
            }

            pub fn get_output(&self, env: &Env, sim_buf: &mut SimBuffer) -> Output {
                sim_buf.reset();

                let mut apples: i64 = env.initial_money as i64;
                let counts = &mut sim_buf.counts;
                let powers = &mut sim_buf.powers;
                let active_by_level = &mut sim_buf.active_by_level;
                let level_max = env.machine_level_max;
                let type_num = env.machine_type_num;
                let base_output = &env.machine_base_output;
                let base_cost = &env.machine_base_cost;

                let mut upgrade_idx = 0;
                let mut output = Output::new();

                for _ in 0..env.turn_limit {
                    let mut action = Action::new(-1, -1);
                    if upgrade_idx < self.upgrade_order.len() {
                        let target_id = self.upgrade_order[upgrade_idx];
                        if target_id >= 0 && target_id < type_num as i32 {
                            let j = target_id as usize;
                            let mut zero_power_candidate = None;
                            let mut highest_affordable = None;
                            for i in 0..level_max {
                                let idx = i * type_num + j;
                                let cost = base_cost[i][j] as i64 * (powers[idx] + 1);
                                if apples >= cost {
                                    if powers[idx] == 0 && zero_power_candidate.is_none() {
                                        zero_power_candidate = Some((i, cost));
                                    }
                                    highest_affordable = Some((i, cost));
                                }
                            }
                            if let Some((i, cost)) = zero_power_candidate.or(highest_affordable) {
                                apples -= cost;
                                let idx = i * type_num + j;
                                if powers[idx] == 0 {
                                    active_by_level[i].push(idx);
                                }
                                powers[idx] += 1;
                                upgrade_idx += 1;
                                action = Action::new(i as i32, j as i32);
                            }
                        }
                    }
                    output.actions.push(action);

                    // 生産フェーズ
                    for i in 0..level_max {
                        for &idx in &active_by_level[i] {
                            let power = powers[idx];
                            if i == 0 {
                                let j = idx - i * type_num;
                                apples += base_output[j] as i64 * counts[idx] * power;
                            } else {
                                let prev_idx = idx - type_num;
                                counts[prev_idx] += counts[idx] * power;
                            }
                        }
                    }
                }

                output
            }
        }

        pub fn solve(
            env: &Env,
            hyp_param: &HyperParameter,
            rng: &mut Pcg64Mcg,
            time_limit: f64,
        ) -> Output {
            let mut sim_buf = SimBuffer::new(env);
            let mut state = State::new(env);

            let mut last_score = state.simulate_full(env, &mut sim_buf);
            let mut best_score = last_score;

            let base_temperature: f64 = hyp_param.sa1_base_temperature;
            let target_temperature: f64 = hyp_param.sa1_target_temperature;
            let mut temperature: f64 = base_temperature;

            let mut iter_count = 0;
            let time_start = Instant::now();

            let mut progress = 0.0;

            loop {
                if iter_count % 1 == 0 {
                    let time_now = Instant::now();
                    let time_since_program_start = time_now.duration_since(env.time_begin);
                    if time_since_program_start.as_secs_f64() > time_limit {
                        break;
                    }
                    let current_time = time_now.duration_since(time_start);
                    progress = (current_time.as_secs_f64()
                        / (time_limit - time_start.duration_since(env.time_begin).as_secs_f64()))
                    .min(1.0);
                    temperature =
                        base_temperature.powf(1.0 - progress) * target_temperature.powf(progress);
                }

                let roll = rng.random::<f64>();
                if roll < 0.50 {
                    let upgrade_idx = rng.random_range(0..state.upgrade_order.len());
                    let new_id = rng.random_range(0..(env.machine_type_num as i32));
                    if new_id == state.upgrade_order[upgrade_idx] {
                        continue;
                    }

                    let old_upgrade_info = state.upgrade_order[upgrade_idx];
                    state.upgrade_order[upgrade_idx] = new_id;

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order[upgrade_idx] = old_upgrade_info;
                        }
                    }
                } else if roll < 0.75 {
                    let upgrade_idx1 = rng.random_range(0..state.upgrade_order.len());
                    let upgrade_idx2 = rng.random_range(0..state.upgrade_order.len());
                    if upgrade_idx1 == upgrade_idx2 {
                        continue;
                    }

                    let upgrade_info_to_move = state.upgrade_order[upgrade_idx1];
                    state.upgrade_order.remove(upgrade_idx1);
                    state
                        .upgrade_order
                        .insert(upgrade_idx2, upgrade_info_to_move);

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order.remove(upgrade_idx2);
                            state
                                .upgrade_order
                                .insert(upgrade_idx1, upgrade_info_to_move);
                        }
                    }
                } else if roll < 0.95 {
                    let upgrade_idx1 = rng.random_range(0..state.upgrade_order.len());
                    let upgrade_idx2 = rng.random_range(0..state.upgrade_order.len());
                    if state.upgrade_order[upgrade_idx1] == state.upgrade_order[upgrade_idx2] {
                        continue;
                    }

                    state.upgrade_order.swap(upgrade_idx1, upgrade_idx2);

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order.swap(upgrade_idx1, upgrade_idx2);
                        }
                    }
                } else if roll < 1.00 {
                    let upgrade_idx = rng.random_range(0..state.upgrade_order.len());
                    let new_id = rng.random_range(0..(env.machine_type_num as i32));

                    let old_upgrade_order = state.upgrade_order.clone();

                    for i in upgrade_idx..state.upgrade_order.len() {
                        state.upgrade_order[i] = new_id;
                    }

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order = old_upgrade_order;
                        }
                    }
                }

                if iter_count % 1000 == 0 {
                    eprintln!(
                        "iter: {:>8}, last_score: {:>16.4}, best_score: {:>16.4}, temperature: {:16.4}, time: {}",
                        iter_count,
                        last_score,
                        best_score,
                        temperature,
                        time_start.elapsed().as_secs_f64()
                    );
                }

                iter_count += 1;
            }

            eprintln!("iter_count  = {}", iter_count);
            eprintln!("last_score  = {}", last_score);
            eprintln!("best_score  = {}", best_score);
            eprintln!("temperature = {}", temperature);
            eprintln!("time        = {}", time_start.elapsed().as_secs_f64());

            state.get_output(env, &mut sim_buf)
        }
    }

    mod sa_solver2 {
        use core::time;

        use rand::Rng;

        use super::*;

        struct SimBuffer {
            counts: Vec<i64>,
            powers: Vec<i64>,
            active_by_level: Vec<Vec<usize>>,
        }

        impl SimBuffer {
            fn new(env: &Env) -> Self {
                let size = env.machine_level_max * env.machine_type_num;
                Self {
                    counts: vec![1; size],
                    powers: vec![0; size],
                    active_by_level: vec![Vec::new(); env.machine_level_max],
                }
            }

            #[inline]
            fn reset(&mut self) {
                self.counts.fill(1);
                self.powers.fill(0);
                for active in &mut self.active_by_level {
                    active.clear();
                }
            }
        }

        #[derive(Clone)]
        struct UpgradeInfo {
            pub level: i32,
            pub id: i32,
        }

        impl UpgradeInfo {
            pub fn new(level: i32, id: i32) -> Self {
                Self { level, id }
            }
        }

        struct State {
            pub upgrade_order: Vec<UpgradeInfo>,
        }

        impl State {
            pub fn new(prev_output: &Output, env: &Env) -> Self {
                let mut upgrade_order = Vec::with_capacity(env.turn_limit);
                for action in &prev_output.actions {
                    if action.upgrade_target_level != -1 {
                        upgrade_order.push(UpgradeInfo::new(
                            action.upgrade_target_level,
                            action.upgrade_target_id,
                        ));
                    }
                }
                // 最大ターンに満たない分の強化は、最も多く強化した機械IDのレベル0で埋める
                let mut id_count = vec![0; env.machine_type_num];
                for info in &upgrade_order {
                    if info.level == 0 {
                        id_count[info.id as usize] += 1;
                    }
                }
                let max_id = id_count
                    .iter()
                    .enumerate()
                    .max_by_key(|&(_, count)| count)
                    .map(|(id, _)| id)
                    .unwrap_or(0) as i32;
                while upgrade_order.len() < env.turn_limit {
                    upgrade_order.push(UpgradeInfo::new(0, max_id));
                }
                Self { upgrade_order }
            }

            pub fn simulate_full(&self, env: &Env, sim_buf: &mut SimBuffer) -> f64 {
                sim_buf.reset();

                let mut apples: i64 = env.initial_money as i64;
                let counts = &mut sim_buf.counts;
                let powers = &mut sim_buf.powers;
                let active_by_level = &mut sim_buf.active_by_level;
                let level_max = env.machine_level_max;
                let type_num = env.machine_type_num;
                let base_output = &env.machine_base_output;
                let base_cost = &env.machine_base_cost;

                let mut upgrade_idx = 0;
                for _ in 0..env.turn_limit {
                    if upgrade_idx < self.upgrade_order.len() {
                        let target = &self.upgrade_order[upgrade_idx];
                        if target.level >= 0
                            && target.level < env.machine_level_max as i32
                            && target.id >= 0
                            && target.id < type_num as i32
                        {
                            let i = target.level as usize;
                            let j = target.id as usize;
                            let idx = i * type_num + j;
                            let cost = base_cost[i][j] as i64 * (powers[idx] + 1);
                            if apples >= cost {
                                apples -= cost;
                                if powers[idx] == 0 {
                                    active_by_level[i].push(idx);
                                }
                                powers[idx] += 1;
                                upgrade_idx += 1;
                            }
                        }
                    }

                    // 生産フェーズ。Level 0 から順に処理する。
                    for i in 0..level_max {
                        for &idx in &active_by_level[i] {
                            let power = powers[idx];
                            if i == 0 {
                                let j = idx - i * type_num;
                                apples += base_output[j] as i64 * counts[idx] * power;
                            } else {
                                let prev_idx = idx - type_num;
                                counts[prev_idx] += counts[idx] * power;
                            }
                        }
                    }
                }

                f64::log2(apples as f64)
            }

            pub fn get_output(&self, env: &Env, sim_buf: &mut SimBuffer) -> Output {
                sim_buf.reset();

                let mut apples: i64 = env.initial_money as i64;
                let counts = &mut sim_buf.counts;
                let powers = &mut sim_buf.powers;
                let active_by_level = &mut sim_buf.active_by_level;
                let level_max = env.machine_level_max;
                let type_num = env.machine_type_num;
                let base_output = &env.machine_base_output;
                let base_cost = &env.machine_base_cost;

                let mut upgrade_idx = 0;
                let mut output = Output::new();

                for _ in 0..env.turn_limit {
                    let mut action = Action::new(-1, -1);
                    if upgrade_idx < self.upgrade_order.len() {
                        let target = &self.upgrade_order[upgrade_idx];
                        if target.level >= 0
                            && target.level < env.machine_level_max as i32
                            && target.id >= 0
                            && target.id < type_num as i32
                        {
                            let i = target.level as usize;
                            let j = target.id as usize;
                            let idx = i * type_num + j;
                            let cost = base_cost[i][j] as i64 * (powers[idx] + 1);
                            if apples >= cost {
                                apples -= cost;
                                if powers[idx] == 0 {
                                    active_by_level[i].push(idx);
                                }
                                powers[idx] += 1;
                                upgrade_idx += 1;
                                action = Action::new(i as i32, j as i32);
                            }
                        }
                    }
                    output.actions.push(action);

                    // 生産フェーズ
                    for i in 0..level_max {
                        for &idx in &active_by_level[i] {
                            let power = powers[idx];
                            if i == 0 {
                                let j = idx - i * type_num;
                                apples += base_output[j] as i64 * counts[idx] * power;
                            } else {
                                let prev_idx = idx - type_num;
                                counts[prev_idx] += counts[idx] * power;
                            }
                        }
                    }
                }

                output
            }
        }

        pub fn solve(
            prev_output: &Output,
            env: &Env,
            hyp_param: &HyperParameter,
            rng: &mut Pcg64Mcg,
            time_limit: f64,
        ) -> Output {
            let mut sim_buf = SimBuffer::new(env);
            let mut state = State::new(prev_output, env);

            let mut last_score = state.simulate_full(env, &mut sim_buf);
            let mut best_score = last_score;

            let base_temperature: f64 = hyp_param.sa2_base_temperature;
            let target_temperature: f64 = hyp_param.sa2_target_temperature;
            let mut temperature: f64 = base_temperature;

            let mut iter_count = 0;
            let time_start = Instant::now();

            let mut progress = 0.0;

            loop {
                if iter_count % 1 == 0 {
                    let time_now = Instant::now();
                    let time_since_program_start = time_now.duration_since(env.time_begin);
                    if time_since_program_start.as_secs_f64() > time_limit {
                        break;
                    }
                    let current_time = time_now.duration_since(time_start);
                    progress = (current_time.as_secs_f64()
                        / (time_limit - time_start.duration_since(env.time_begin).as_secs_f64()))
                    .min(1.0);
                    temperature =
                        base_temperature.powf(1.0 - progress) * target_temperature.powf(progress);
                }

                let roll = rng.random::<f64>();
                if roll < 0.40 {
                    let upgrade_idx = rng.random_range(0..state.upgrade_order.len());
                    let new_id = rng.random_range(0..(env.machine_type_num as i32));
                    let new_level = rng.random_range(0..(env.machine_level_max as i32));
                    if new_id == state.upgrade_order[upgrade_idx].id
                        && new_level == state.upgrade_order[upgrade_idx].level
                    {
                        continue;
                    }

                    let old_upgrade_info = state.upgrade_order[upgrade_idx].clone();
                    state.upgrade_order[upgrade_idx] = UpgradeInfo::new(new_level, new_id);

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order[upgrade_idx] = old_upgrade_info;
                        }
                    }
                } else if roll < 0.65 {
                    let upgrade_idx1 = rng.random_range(0..state.upgrade_order.len());
                    let upgrade_idx2 = {
                        if rng.random::<f64>() < 0.5 {
                            (upgrade_idx1 as i32 + 1 + rng.random_range(-20..=20))
                                .clamp(0, state.upgrade_order.len() as i32 - 1)
                                as usize
                        } else {
                            rng.random_range(0..state.upgrade_order.len())
                        }
                    };
                    if upgrade_idx1 == upgrade_idx2 {
                        continue;
                    }

                    let upgrade_info_to_move = state.upgrade_order[upgrade_idx1].clone();
                    state.upgrade_order.remove(upgrade_idx1);
                    state
                        .upgrade_order
                        .insert(upgrade_idx2, upgrade_info_to_move.clone());

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order.remove(upgrade_idx2);
                            state
                                .upgrade_order
                                .insert(upgrade_idx1, upgrade_info_to_move);
                        }
                    }
                } else if roll < 0.90 {
                    let upgrade_idx1 = rng.random_range(0..state.upgrade_order.len());
                    let upgrade_idx2 = {
                        if rng.random::<f64>() < 0.5 {
                            (upgrade_idx1 as i32 + 1 + rng.random_range(-20..=20))
                                .clamp(0, state.upgrade_order.len() as i32 - 1)
                                as usize
                        } else {
                            rng.random_range(0..state.upgrade_order.len())
                        }
                    };
                    if state.upgrade_order[upgrade_idx1].id == state.upgrade_order[upgrade_idx2].id
                        && state.upgrade_order[upgrade_idx1].level
                            == state.upgrade_order[upgrade_idx2].level
                    {
                        continue;
                    }

                    state.upgrade_order.swap(upgrade_idx1, upgrade_idx2);

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order.swap(upgrade_idx1, upgrade_idx2);
                        }
                    }
                } else if roll < 0.95 {
                    let upgrade_idx = rng.random_range(0..state.upgrade_order.len());

                    let upgrade_info_to_move = state.upgrade_order[upgrade_idx].clone();
                    state.upgrade_order.remove(upgrade_idx);
                    state.upgrade_order.push(upgrade_info_to_move.clone());

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order.pop();
                            state
                                .upgrade_order
                                .insert(upgrade_idx, upgrade_info_to_move);
                        }
                    }
                } else if roll < 1.00 {
                    let upgrade_idx = rng.random_range(0..state.upgrade_order.len());
                    let new_id = {
                        if rng.random::<f64>() < 0.5 {
                            state.upgrade_order[upgrade_idx].id
                        } else {
                            rng.random_range(0..(env.machine_type_num as i32))
                        }
                    };
                    let new_level = rng.random_range(0..(env.machine_level_max as i32));

                    let upgrade_info_to_pop = state.upgrade_order[upgrade_idx].clone();
                    state.upgrade_order.pop();
                    state
                        .upgrade_order
                        .insert(upgrade_idx, UpgradeInfo::new(new_level, new_id));

                    let new_score = state.simulate_full(env, &mut sim_buf);

                    if new_score >= last_score {
                        last_score = new_score;
                        if new_score > best_score {
                            best_score = new_score
                        }
                    } else {
                        let delta = -(last_score - new_score).abs() as f64;
                        let prob = (delta / temperature).exp();
                        if rng.random::<f64>() < prob {
                            // accept
                            last_score = new_score
                        } else {
                            // rollback
                            state.upgrade_order.remove(upgrade_idx);
                            state.upgrade_order.push(upgrade_info_to_pop);
                        }
                    }
                }

                if iter_count % 1000 == 0 {
                    eprintln!(
                        "iter: {:>8}, last_score: {:>16.4}, best_score: {:>16.4}, temperature: {:16.4}, time: {}",
                        iter_count,
                        last_score,
                        best_score,
                        temperature,
                        time_start.elapsed().as_secs_f64()
                    );
                }

                iter_count += 1;
            }

            eprintln!("iter_count  = {}", iter_count);
            eprintln!("last_score  = {}", last_score);
            eprintln!("best_score  = {}", best_score);
            eprintln!("temperature = {}", temperature);
            eprintln!("time        = {}", time_start.elapsed().as_secs_f64());

            state.get_output(env, &mut sim_buf)
        }
    }

    fn solve(env: &Env, hyp_param: &HyperParameter, rng: &mut Pcg64Mcg) -> Output {
        let output_sa1 = sa_solver1::solve(env, hyp_param, rng, hyp_param.sa1_time_limit);
        let output_sa2 = sa_solver2::solve(&output_sa1, env, hyp_param, rng, 1.99);

        output_sa2
    }

    pub fn run() {
        let hyp_param = HyperParameter::new();
        let mut rng = Pcg64Mcg::seed_from_u64(42);

        let input = Input::from_stdin();
        let env = Env::new(&input);
        let output = solve(&env, &hyp_param, &mut rng);
        output.output();
    }
}

fn main() {
    unsafe {
        std::env::set_var("RUST_BACKTRACE", "1");
    }
    solver::run();
}
