mod solver {
    use std::{
        cmp::Reverse, collections::{HashMap, HashSet, VecDeque}, env, error, fmt, io::{self, BufReader}, mem::{self, swap}, path, time::Instant
    };
    
    use alga::general::ComplexField;
    use bitvec::vec;
    use itertools::Itertools;
    use multiversion::target;
    use proconio::{input, source::line::LineSource};
    use rand::{prelude::*, Rng};
    use rand_core::SeedableRng;
    use rand_pcg::Pcg64Mcg;

    pub struct HyperParameter {
        pub sa_start_target_num: i32,
        pub sa_prob_clamp_min: f64,
        pub sa_prob_clamp_max: f64,
        pub sa_prob_prob_pow_factor: f64,
        pub sa_prob_score_pow_factor: f64,
        pub sa_base_temperature: f64,
        pub sa_target_temperature: f64,
        pub sa2_base_temperature: f64,
        pub sa2_target_temperature: f64,
    }
    impl HyperParameter {
        pub fn new() -> Self {
            let sa_start_target_num = match(env::var("AHC_SA_START_TARGET_NUM")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 15,
            };
            let sa_prob_clamp_min = match(env::var("AHC_SA_PROB_CLAMP_MIN")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 4.9486188806486244e-20,
            };
            let sa_prob_clamp_max = match(env::var("AHC_SA_PROB_CLAMP_MAX")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.9959018854118757,
            };
            let sa_prob_prob_pow_factor = match(env::var("AHC_SA_PROB_PROB_POW_FACTOR")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.8,
            };
            let sa_prob_score_pow_factor = match(env::var("AHC_SA_PROB_SCORE_POW_FACTOR")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 1.891366434963121,
            };
            let sa_base_temperature = match(env::var("AHC_SA_BASE_TEMPERATURE")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 3.2093021063658447,
            };
            let sa_target_temperature = match(env::var("AHC_SA_TARGET_TEMPERATURE")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.07323213558311645,
            };
            let sa2_base_temperature = match(env::var("AHC_SA2_BASE_TEMPERATURE")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.36529266649867653,
            };
            let sa2_target_temperature = match(env::var("AHC_SA2_TARGET_TEMPERATURE")) {
                Ok(val) => val.parse().unwrap(),
                Err(_) => 0.049339540684413416,
            };
            Self {
                sa_start_target_num,
                sa_prob_clamp_min,
                sa_prob_clamp_max,
                sa_prob_prob_pow_factor,
                sa_prob_score_pow_factor,
                sa_base_temperature,
                sa_target_temperature,
                sa2_base_temperature,
                sa2_target_temperature,
            }
        }
    }

    pub struct Input {
        pub target_num: i32,
        pub state_num: i32,
        pub str_len: i32,
        pub targets: Vec<(String, i32)>,
    }

    impl Input {
        pub fn from_stdin() -> Self {
            // magic
            let mut stdin = LineSource::new(BufReader::new(io::stdin()));
            macro_rules! input(($($tt:tt)*) => (proconio::input!(from &mut stdin, $($tt)*)));

            input! {
                target_num: i32,
                state_num: i32,
                str_len: i32,
            }

            let mut targets = Vec::new();
            for _ in 0..target_num {
                input! {
                    s: String,
                    t: i32,
                }
                targets.push((s, t));
            }

            Self {
                target_num,
                state_num,
                str_len,
                targets,
            }
        }
    }

    struct Output {
        pub chars: Vec<char>,
        pub probs: Vec<Vec<i32>>,
    }

    impl Output {
        pub fn new() -> Self {
            Self {
                chars: vec![],
                probs: vec![],
            }
        }

        pub fn output(&self) {
            for i in 0..self.chars.len() {
                print!("{}", self.chars[i]);
                for j in 0..self.probs[i].len() {
                    print!(" {}", self.probs[i][j]);
                }
                println!();
            }
        }
    }

    pub struct Env {
        pub time_begin: Instant,

        pub target_num: i32,
        pub state_num: i32,
        pub str_len: i32,
        pub targets: Vec<(String, i32)>,

        pub target_vec_chars: Vec<Vec<char>>,
    }

    impl Env {
        pub fn new(input: Input) -> Self {
            let time_begin = Instant::now();

            let target_num = input.target_num;
            let state_num = input.state_num;
            let str_len = input.str_len;
            let targets = input.targets.clone();

            let mut target_vec_chars = Vec::new();
            for i in 0..target_num {
                let target = &targets[i as usize].0;
                let mut vec_chars = Vec::new();
                for c in target.chars() {
                    vec_chars.push(c);
                }
                target_vec_chars.push(vec_chars);
            }

            Self {
                time_begin,
                target_num,
                state_num,
                str_len,
                targets,
                target_vec_chars,
            }
        }
    }

    fn mul(a: &Vec<f64>, b: &Vec<f64>, rows_a: usize, cols_a: usize, cols_b: usize) -> Vec<f64> {
        let mut c = vec![0.0; rows_a * cols_b];
        for i in 0..rows_a {
            for k in 0..cols_a {
                for j in 0..cols_b {
                    c[i * cols_b + j] += a[i * cols_a + k] * b[k * cols_b + j];
                }
            }
        }
        c
    }

    fn solve_trivial(env: &Env, rng: &mut Pcg64Mcg) -> Output {
        let chars = vec!['a', 'b', 'c', 'd', 'e', 'f'];

        let mut output = Output::new();
        output.chars = vec!['a'; env.state_num as usize];
        for i in 0..env.state_num as usize {
            output.chars[i] = chars[i % chars.len()];
        }
        output.probs = vec![vec![0; env.state_num as usize]; env.state_num as usize];
        for i in 0..env.state_num as usize {
            for j in 0..100 as usize {
                output.probs[i][j % env.state_num as usize] += 1;
            }
        }
        output
    }

    fn solve_greedy(env: &Env, rng: &mut Pcg64Mcg) -> Output {
        let mut best_target_idx: usize = 0;
        let mut best_target_score = 0;
        for i in 0..env.target_num {
            let target = &env.targets[i as usize];
            let target_score = target.1;
            if target_score > best_target_score {
                best_target_idx = i as usize;
                best_target_score = target_score;
            }
        }

        let best_target_str = &env.targets[best_target_idx].0;

        let mut output = Output::new();
        output.chars = vec!['a'; env.state_num as usize];
        output.probs = vec![vec![5; env.state_num as usize]; env.state_num as usize];
        for i in 0..best_target_str.len() {
            let c = best_target_str.chars().nth(i).unwrap();
            output.chars[i] = c;
            output.probs[i][(i + 1) % best_target_str.len() as usize] = 100 - 5 * (env.state_num - 1);
        }
        for i in best_target_str.len()..env.state_num as usize {
            output.probs[i][0] = 100 - 5 * (env.state_num - 1);
        }
        
        output
    }

    fn solve_sa1(env: &Env, init_output: &Output, hyp_param: &HyperParameter, rng: &mut Pcg64Mcg, time_limit: f64) -> Output {
        let mut chars = init_output.chars.clone();
        let mut probs = init_output.probs.clone();

        let mut is_target = vec![false; env.target_num as usize];

        for _ in 0..hyp_param.sa_start_target_num as usize {
            let mut best_target_idx = 0;
            let mut best_target_score = 0;
            for i in 0..env.target_num as usize {
                if is_target[i] {
                    continue;
                }
                let target = &env.targets[i];
                let target_score = target.1;
                if target_score > best_target_score {
                    best_target_idx = i;
                    best_target_score = target_score;
                }
            }
            is_target[best_target_idx] = true;
        }
            
        fn get_score(env: &Env, chars: &Vec<char>, probs: &Vec<Vec<i32>>, is_target: &Vec<bool>, hyp_param: &HyperParameter) -> f64 {
            let mut exist_prob = vec![0.0; env.state_num as usize];
            {
                exist_prob[0] = 1.0;
                let mut next_mat = vec![0.0; env.state_num as usize * env.state_num as usize];
                for i in 0..env.state_num as usize {
                    for j in 0..env.state_num as usize {
                        next_mat[i * env.state_num as usize + j] = probs[i][j] as f64 / 100.0;
                    }
                }
                let mut power = env.str_len - 1;
                while power > 0 {
                    if power % 2 == 1 {
                        exist_prob = mul(&exist_prob, &next_mat, 1, env.state_num as usize, env.state_num as usize);
                    }
                    next_mat = mul(&next_mat, &next_mat, env.state_num as usize, env.state_num as usize, env.state_num as usize);
                    power /= 2;
                }
            }

            let mut score = 0.0;
            for i in 0..env.target_num as usize {
                if !is_target[i] {
                    continue;
                }
                let target_str = &env.target_vec_chars[i];
                let target_score = &env.targets[i].1;
                let mut prob = 0.0;
                for j in 0..env.state_num as usize {
                    if chars[j] != target_str[0] {
                        continue;
                    }

                    let mut dp = vec![0.0; env.state_num as usize];
                    dp[j] = 1.0;
                    for k in 1..target_str.len() {
                        let mut next_dp = vec![0.0; env.state_num as usize];
                        for l in 0..env.state_num as usize {
                            if chars[l] == target_str[k] {
                                for m in 0..env.state_num as usize {
                                    next_dp[l] += dp[m] * (probs[m][l] as f64 / 100.0);
                                }
                            }
                        }
                        dp = next_dp;
                    }

                    let mut prob_sum = 0.0;
                    for l in 0..env.state_num as usize {
                        prob_sum += dp[l];
                    }

                    prob += (1.0 - (1.0 - prob_sum).powf(env.str_len as f64 * exist_prob[j])) * (1.0 - prob);
                }
                score += -prob.clamp(hyp_param.sa_prob_clamp_min, hyp_param.sa_prob_clamp_max).log2().abs().powf(hyp_param.sa_prob_prob_pow_factor) * (*target_score as f64).powf(hyp_param.sa_prob_score_pow_factor);
            }
            score
        }

        let mut best_score = get_score(env, &chars, &probs, &is_target, hyp_param);
        let mut last_score = best_score;
        
        let base_temperature: f64 = hyp_param.sa_base_temperature;
        let target_temperature: f64 = hyp_param.sa_target_temperature;
        let mut temperature = base_temperature;

        let mut iter_count = 0;
        let time_start = Instant::now();

        let mut progress;

        loop {
            if iter_count % 1 == 0 {
                let time_now = Instant::now();
                let time_since_program_start = time_now.duration_since(env.time_begin);
                if time_since_program_start.as_secs_f64() > time_limit {
                    break;
                }
                let current_time = time_now.duration_since(time_start);
                progress = (current_time.as_secs_f64() / (time_limit - time_start.duration_since(env.time_begin).as_secs_f64())).min(1.0);
                temperature = base_temperature.powf(1.0 - progress) * target_temperature.powf(progress);
            }

            let roll = rng.gen::<f64>();
            if roll < 0.50 {
                let i = rng.gen_range(0..env.state_num);
                let j1 = rng.gen_range(0..env.state_num);
                let j2 = rng.gen_range(0..env.state_num);
                if j1 == j2 || probs[i as usize][j1 as usize] == 0 {
                    continue;
                }
                
                probs[i as usize][j1 as usize] -= 1;
                probs[i as usize][j2 as usize] += 1;

                let score = get_score(env, &chars, &probs, &is_target, hyp_param);

                if score >= last_score {
                    last_score = score;
                    if score > best_score {
                        best_score = score;
                    }
                }
                else {
                    let delta = (score - last_score) as f64;
                    let prob = (delta / temperature).exp();
                    if rng.gen::<f64>() < prob { // accept
                        last_score = score;
                    }
                    else { // rollback
                        probs[i as usize][j1 as usize] += 1;
                        probs[i as usize][j2 as usize] -= 1;
                    }
                }
            }
            else if roll < 1.00 {
                let i = rng.gen_range(0..env.state_num);
                let j1 = rng.gen_range(0..env.state_num);
                let j2 = rng.gen_range(0..env.state_num);
                if j1 == j2 || probs[i as usize][j1 as usize] == 0 {
                    continue;
                }
                let d = rng.gen_range(1..=probs[i as usize][j1 as usize]);
                
                probs[i as usize][j1 as usize] -= d;
                probs[i as usize][j2 as usize] += d;

                let score = get_score(env, &chars, &probs, &is_target, hyp_param);

                if score >= last_score {
                    last_score = score;
                    if score > best_score {
                        best_score = score;
                    }
                }
                else {
                    let delta = (score - last_score) as f64;
                    let prob = (delta / temperature).exp();
                    if rng.gen::<f64>() < prob { // accept
                        last_score = score;
                    }
                    else { // rollback
                        probs[i as usize][j1 as usize] += d;
                        probs[i as usize][j2 as usize] -= d;
                    }
                }
            }

            if iter_count % 1000 == 0 {
                eprintln!("iter: {:>8}, last_score: {:>12.4}, best_score: {:>12.4}, temperature: {:12.4}, time: {}", iter_count, last_score, best_score, temperature, time_start.elapsed().as_secs_f64());
            }

            iter_count += 1;
        }

        eprintln!("iter_count  = {}", iter_count);
        eprintln!("last_score  = {}", last_score);
        eprintln!("best_score  = {}", best_score);
        eprintln!("temperature = {}", temperature);
        eprintln!("time        = {}", time_start.elapsed().as_secs_f64());

        let mut output = Output::new();
        output.chars = chars;
        output.probs = probs;
        output
    }

    fn solve_sa2(env: &Env, init_output: &Output, hyp_param: &HyperParameter, rng: &mut Pcg64Mcg, time_limit: f64) -> (Output, f64) {
        let mut chars = init_output.chars.clone();
        let mut probs = init_output.probs.clone();
            
        fn get_score(env: &Env, chars: &Vec<char>, probs: &Vec<Vec<i32>>) -> f64 {
            let mut exist_prob = vec![0.0; env.state_num as usize];
            {
                exist_prob[0] = 1.0;
                let mut next_mat = vec![0.0; env.state_num as usize * env.state_num as usize];
                for i in 0..env.state_num as usize {
                    for j in 0..env.state_num as usize {
                        next_mat[i * env.state_num as usize + j] = probs[i][j] as f64 / 100.0;
                    }
                }
                let mut power = env.str_len - 1;
                while power > 0 {
                    if power % 2 == 1 {
                        exist_prob = mul(&exist_prob, &next_mat, 1, env.state_num as usize, env.state_num as usize);
                    }
                    next_mat = mul(&next_mat, &next_mat, env.state_num as usize, env.state_num as usize, env.state_num as usize);
                    power /= 2;
                }
            }

            let mut score = 0.0;
            for i in 0..env.target_num as usize {
                let target_str = &env.target_vec_chars[i];
                let target_score = &env.targets[i].1;
                let mut prob = 0.0;
                for j in 0..env.state_num as usize {
                    if chars[j] != target_str[0] {
                        continue;
                    }

                    let mut dp = vec![0.0; env.state_num as usize];
                    dp[j] = 1.0;
                    for k in 1..target_str.len() {
                        let mut next_dp = vec![0.0; env.state_num as usize];
                        for l in 0..env.state_num as usize {
                            if chars[l] == target_str[k] {
                                for m in 0..env.state_num as usize {
                                    next_dp[l] += dp[m] * (probs[m][l] as f64 / 100.0);
                                }
                            }
                        }
                        dp = next_dp;
                    }

                    let mut prob_sum = 0.0;
                    for l in 0..env.state_num as usize {
                        prob_sum += dp[l];
                    }

                    prob += (1.0 - (1.0 - prob_sum).powf(env.str_len as f64 * exist_prob[j])) * (1.0 - prob);
                }
                score += prob * (*target_score as f64);
            }
            score
        }

        let mut best_score = get_score(env, &chars, &probs);
        let mut last_score = best_score;
        
        let base_temperature: f64 = hyp_param.sa2_base_temperature;
        let target_temperature: f64 = hyp_param.sa2_target_temperature;
        let mut temperature = base_temperature;

        let mut iter_count = 0;
        let time_start = Instant::now();

        let mut progress;

        loop {
            if iter_count % 1 == 0 {
                let time_now = Instant::now();
                let time_since_program_start = time_now.duration_since(env.time_begin);
                if time_since_program_start.as_secs_f64() > time_limit {
                    break;
                }
                let current_time = time_now.duration_since(time_start);
                progress = (current_time.as_secs_f64() / (time_limit - time_start.duration_since(env.time_begin).as_secs_f64())).min(1.0);
                temperature = base_temperature.powf(1.0 - progress) * target_temperature.powf(progress);
            }

            let roll = rng.gen::<f64>();
            if roll < 0.50 {
                let i = rng.gen_range(0..env.state_num);
                let j1 = rng.gen_range(0..env.state_num);
                let j2 = rng.gen_range(0..env.state_num);
                if j1 == j2 || probs[i as usize][j1 as usize] == 0 {
                    continue;
                }
                
                probs[i as usize][j1 as usize] -= 1;
                probs[i as usize][j2 as usize] += 1;

                let score = get_score(env, &chars, &probs);

                if score >= last_score {
                    last_score = score;
                    if score > best_score {
                        best_score = score;
                    }
                }
                else {
                    let delta = (score - last_score) as f64;
                    let prob = (delta / temperature).exp();
                    if rng.gen::<f64>() < prob { // accept
                        last_score = score;
                    }
                    else { // rollback
                        probs[i as usize][j1 as usize] += 1;
                        probs[i as usize][j2 as usize] -= 1;
                    }
                }
            }
            else if roll < 1.00 {
                let i = rng.gen_range(0..env.state_num);
                let j1 = rng.gen_range(0..env.state_num);
                let j2 = rng.gen_range(0..env.state_num);
                if j1 == j2 || probs[i as usize][j1 as usize] == 0 {
                    continue;
                }
                let d = rng.gen_range(1..=probs[i as usize][j1 as usize]);
                
                probs[i as usize][j1 as usize] -= d;
                probs[i as usize][j2 as usize] += d;

                let score = get_score(env, &chars, &probs);

                if score >= last_score {
                    last_score = score;
                    if score > best_score {
                        best_score = score;
                    }
                }
                else {
                    let delta = (score - last_score) as f64;
                    let prob = (delta / temperature).exp();
                    if rng.gen::<f64>() < prob { // accept
                        last_score = score;
                    }
                    else { // rollback
                        probs[i as usize][j1 as usize] += d;
                        probs[i as usize][j2 as usize] -= d;
                    }
                }
            }

            if iter_count % 1000 == 0 {
                eprintln!("iter: {:>8}, last_score: {:>12.4}, best_score: {:>12.4}, temperature: {:12.4}, time: {}", iter_count, last_score, best_score, temperature, time_start.elapsed().as_secs_f64());
            }

            iter_count += 1;
        }

        eprintln!("iter_count  = {}", iter_count);
        eprintln!("last_score  = {}", last_score);
        eprintln!("best_score  = {}", best_score);
        eprintln!("temperature = {}", temperature);
        eprintln!("time        = {}", time_start.elapsed().as_secs_f64());

        let mut output = Output::new();
        output.chars = chars;
        output.probs = probs;
        (output, last_score)
    }

    fn solve(env: &Env, hyp_param: &HyperParameter, rng: &mut Pcg64Mcg) -> Output {
        let output_trivial = solve_trivial(env, rng);
        // let output_greedy = solve_greedy(env, rng);
        let max_tries = 2;
        let max_time_limit = 1.9;
        let mut best_score = 0.0;
        let mut best_output = Output::new();
        for t in 0..max_tries {
            let time_limit1 = t as f64 * max_time_limit / max_tries as f64 + (max_time_limit / max_tries as f64) * 0.5;
            let time_limit2 = t as f64 * max_time_limit / max_tries as f64 + max_time_limit / max_tries as f64;
            let output_sa1 = solve_sa1(env, &output_trivial, hyp_param, rng, time_limit1);
            let (output_sa2, score) = solve_sa2(env, &output_sa1, hyp_param, rng, time_limit2);
            if score > best_score {
                best_score = score;
                best_output = output_sa2;
            }
        }
        
        let (output_sa2_final, _) = solve_sa2(env, &best_output, hyp_param, rng, 1.98);

        output_sa2_final
    }

    pub fn run() {
        let hyp_param = HyperParameter::new();
        let mut rng = Pcg64Mcg::seed_from_u64(42);

        let input = Input::from_stdin();
        let env = Env::new(input);
        let output = solve(&env, &hyp_param, &mut rng);

        output.output();
    }
}

fn main() {
    std::env::set_var("RUST_BACKTRACE", "1");
    solver::run();
}
