use rand::rngs::StdRng;
use rand::Rng;
use rand::SeedableRng;
use rand_distr::num_traits::Pow;
use rand_distr::{Distribution, Normal};
use rustc_hash::FxHashMap;
use std::collections::{HashMap, HashSet};
use std::io::{self, Write};
use std::ops::Mul;
use std::time::{Duration, Instant};

#[derive(Debug, Clone)]
struct Config {
    sumq_type: usize,     // SumqType
    query_num: usize,     // StartBit
    sum_w_range: f64,     // SumWRange
    pq_size: usize,       // LIMIT (時間制限など)
    time_limit: Duration, // TIME LIMIT
    bonus: i64,           // 重複ボーナス
    line_bonus: i64,
    marge_size: i32,
    sigma_hi: f64,
    sigma_low: f64,
}

impl Default for Config {
    fn default() -> Self {
        Config {
            sumq_type: 4,
            query_num: 5,
            sum_w_range: 0.15,
            pq_size: 10000000,
            time_limit: std::time::Duration::from_millis(2500),
            bonus: 50000000000,
            line_bonus: 60000,
            marge_size: 100000,
            sigma_hi: 2.2,
            sigma_low: 0.8,
        }
    }
}

// 初期設定を保持する構造体
#[derive(Debug)]
struct InitialInput {
    pub n: usize,
    pub t: usize,
    pub sigma: i32,
    pub rectangles: Vec<(i32, i32)>, // 長方形の観測値
}

// 初期入力を受け取る関数
fn read_initial_input() -> InitialInput {
    let mut input = String::new();

    // 1行目を読み込み (N, T, σ)
    io::stdin().read_line(&mut input).unwrap();
    let mut parts = input.trim().split_whitespace();
    let n: usize = parts.next().unwrap().parse().unwrap();
    let t: usize = parts.next().unwrap().parse().unwrap();
    let sigma: i32 = parts.next().unwrap().parse().unwrap();

    // 各長方形の観測値を読み込み
    let mut rectangles = Vec::new();
    for _ in 0..n {
        input.clear();
        io::stdin().read_line(&mut input).unwrap();
        let mut parts = input.trim().split_whitespace();
        let w: i32 = parts.next().unwrap().parse().unwrap();
        let h: i32 = parts.next().unwrap().parse().unwrap();
        rectangles.push((w, h));
    }

    InitialInput {
        n,
        t,
        sigma,
        rectangles,
    }
}

// 配置方法を出力し、計測値を受け取る関数
fn query_output(commands: Vec<(usize, i32, char, i32)>) -> (i32, i32) {
    // 配置方法の出力
    println!("{}", commands.len());
    for (p, r, d, b) in commands {
        println!("{} {} {} {}", p, r, d, b);
    }
    io::stdout().flush().unwrap(); // 標準出力をフラッシュ

    // 計測値を受け取る
    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    let mut parts = input.trim().split_whitespace();
    let w_prime: i32 = parts.next().unwrap().parse().unwrap();
    let h_prime: i32 = parts.next().unwrap().parse().unwrap();

    (w_prime, h_prime)
}

fn gradient_descent(
    queries: &[Vec<usize>],
    results: &[f64],
    initial_estimates: &[f64],
    learning_rate: f64,
    max_iterations: usize,
    tolerance: f64,
) -> (Vec<f64>, f64) {
    let size = initial_estimates.len();
    let mut estimates = initial_estimates.to_vec();
    let mut initial_error = 0.0;
    let mut final_error = 0.0;

    for (query, &result) in queries.iter().zip(results.iter()) {
        let prediction: f64 = query.iter().map(|&idx| estimates[idx]).sum();
        let error = prediction - result;
        initial_error += error.powi(2);
    }

    for _ in 0..max_iterations {
        let mut gradients = vec![0.0; size];

        // Compute gradients
        for (query, &result) in queries.iter().zip(results.iter()) {
            let prediction: f64 = query.iter().map(|&idx| estimates[idx]).sum();
            let error = prediction - result;

            for &idx in query {
                gradients[idx] += 2.0 * error;
            }
        }

        // Update estimates and check for convergence
        let mut max_change: f64 = 0.0;
        for j in 0..size {
            let change = learning_rate * gradients[j] / queries.len() as f64;
            estimates[j] -= change;
            max_change = max_change.max(change.abs());
        }

        if max_change < tolerance {
            break;
        }
    }

    for (query, &result) in queries.iter().zip(results.iter()) {
        let prediction: f64 = query.iter().map(|&idx| estimates[idx]).sum();
        let error = prediction - result;
        final_error += error.powi(2);
    }

    eprintln!("first error: {}", initial_error);
    eprintln!("final error: {}", final_error);

    (estimates, initial_error - final_error)
}

fn beam_search(global: &GlobalState, t: usize) {
    // 初期状態をキューに追加

    //let start_time = Instant::now();

    for ttt in 0..global.config.sumq_type {
        let mut states = Vec::new();
        let mut pq = vec![];

        let start_time = Instant::now();

        let initial_state = State::new_initial(ttt, global);
        states.push(initial_state);
        pq.push(ScoreState {
            parent_index: 0,
            pos: 0,
            d: 0,
            pos_c: 0,
            pos_w: 0,
            pos_h: 0,
            h: 0,
            w: 0,
            final_pos: 0,
            hash: 0,
            used_space: 0,
            score: 0,
        });

        // 探索ループ
        for turn in 0..global.n {
            let mut next_pq = vec![];
            let mut pq_count = 0;

            let elapsed = start_time.elapsed();

            let loop_size;
            if elapsed
                >= global
                    .config
                    .time_limit
                    .mul((turn + 1) as u32)
                    .div_f32((global.config.sumq_type * global.n) as f32)
                    + std::time::Duration::from_millis(50)
            {
                loop_size = global.config.pq_size / global.n / global.config.sumq_type / 2;
            } else {
                loop_size = global.config.pq_size / global.n / global.config.sumq_type;
            }

            while pq.len() > 0 {
                if pq_count > loop_size {
                    break;
                }
                pq_count += 1;

                let state_index;
                let new_state = pq.pop();

                if turn == 0 {
                    state_index = 0;
                } else {
                    state_index = states.len();
                    states.push(State::new(&new_state.unwrap(), &states, global));
                }

                let current_state = &states[state_index];

                for pos in current_state.final_pos..=current_state.lw.len() {
                    if pos != current_state.lw.len() && current_state.lh[pos] == 0 {
                        continue;
                    }
                    for d in 0..2 {
                        // 新しい状態を生成
                        let new_score = get_score_state(d, pos, state_index, &states, global);
                        // 状態を格納
                        pq_count += 1;
                        // 優先度付きキューに追加
                        next_pq.push(new_score);
                    }
                }
            }

            // 次のキューに置き換え
            pq.clear();

            next_pq.sort_by_key(|s| s.score);

            let mut pq3 = vec![];

            let mut maxhash = 0;
            let mut bonus_map: FxHashMap<u64, i64> = FxHashMap::default();
            for jj in next_pq {
                let score = jj.score;
                let hash = jj.hash;

                if !bonus_map.contains_key(&hash) {
                    bonus_map.insert(hash, 1);
                } else {
                    bonus_map.insert(hash, bonus_map[&hash] + 1);
                }
                let num = bonus_map[&hash];
                maxhash = maxhash.max(num);

                pq3.push(PrioritizedState {
                    score: -(score - global.config.bonus / num),
                    ss: jj,
                });
            }

            //eprintln!("{} {}", maxhash, pq.len());
            pq3.sort_by_key(|s| s.score);

            for ps in pq3 {
                pq.push(ps.ss);
            }
        }

        let start_time2 = Instant::now();

        let mt = if ttt < t % global.config.sumq_type {
            t / global.config.sumq_type + 1
        } else {
            t / global.config.sumq_type
        };

        let mut result_states = Vec::new();
        let mut result_cnt = 0;

        //pq.sort_by_key(|s| s.h + s.w);

        let mut map: HashMap<u64, usize> = HashMap::default();
        let hash_limit = 3;

        let seed: [u8; 32] = [1; 32];
        let mut rng = StdRng::from_seed(seed);
        let normal = Normal::new(0.0, 1.0).unwrap();

        let loop_num = 51;
        let mut rand_set = vec![vec![vec![0.0; 2]; global.n]; loop_num];

        for i in 0..loop_num {
            for j in 0..global.n {
                for k in 0..2 {
                    rand_set[i][j][k] = normal.sample(&mut rng);
                }
            }
        }

        while pq.len() != 0 && result_cnt <= 300 {
            let ss = pq.pop().unwrap();
            let hash = ss.hash;
            let count = map.entry(hash).and_modify(|c| *c += 1).or_insert(1);
            if *count > hash_limit {
                continue;
            }
            result_cnt += 1;

            let state_index = states.len();
            states.push(State::new(&ss, &states, global));
            let final_state = states[state_index].clone();
            let score = final_state.calc_real_score(&states, global, &rand_set);

            result_states.push((final_state, score));
        }

        result_states.sort_by_key(|s| s.1); // h + w の小さい順にソート
        let selected_states: Vec<_> = result_states.into_iter().collect();

        eprintln!("Pattern {} {}", ttt, selected_states.len());
        let mut cnt = 0;
        let mut set: HashSet<u64> = HashSet::default();
        let mut set2: HashSet<i32> = HashSet::default();

        // 各状態に対してコマンド列を出力
        for state2 in &selected_states {
            if cnt >= mt {
                break;
            }
            let state = &state2.0;
            let calcscore = state2.1;

            let pattern = state.hash;
            if set.contains(&pattern) {
                continue;
            }
            set.insert(pattern);
            let pattern = state.h * 717 + state.w * 173;
            if set2.contains(&pattern) {
                continue;
            }
            set2.insert(pattern);

            cnt += 1;
            let commands = state.generate_commands(&states);

            let ret = query_output(commands);
            eprintln!(
                "score:{}->{}({}) W:{}->{} H:{}->{} ) used:{}",
                state.h + state.w,
                ret.0 + ret.1,
                calcscore,
                state.w,
                ret.0,
                state.h,
                ret.1,
                state.used_space
            );
        }

        while cnt < mt {
            println!("0");
            cnt += 1;
        }

        let elapsed2 = start_time2.elapsed();
        eprintln!("{}", elapsed2.as_millis());
    }
}

fn get_score_state(
    d: i32,
    pos: usize,
    parent_index: usize,
    states: &Vec<State>,
    global: &GlobalState,
) -> ScoreState {
    let parent = &states[parent_index];
    let mut used_space = parent.used_space;
    let mut h = parent.h;
    let mut w = parent.w;

    let mut pos_w = if pos == parent.lw.len() {
        0
    } else {
        parent.lw[pos]
    };
    let mut pos_h = if pos == parent.lh.len() {
        0
    } else {
        parent.lh[pos]
    };
    let mut hash = parent.hash;

    let mut final_pos = parent.final_pos;

    let now_h = if d == 0 {
        global.fs[parent.id].1
    } else {
        global.fs[parent.id].0
    };
    let now_w = if d == 0 {
        global.fs[parent.id].0
    } else {
        global.fs[parent.id].1
    };

    used_space += 0.max(global.config.line_bonus - 0.max(w.max(parent.sumw as i32) - pos_w) as i64)
        * pos_h as i64
        / 8;

    let add_h = (now_h - pos_h).max(0);
    used_space += (add_h as i64) * (pos_w as i64);
    pos_h = pos_h.max(now_h);
    used_space += global.config.line_bonus * (add_h as i64) / 8;

    let sub_h = (pos_h - now_h).max(0);
    used_space += (sub_h as i64) * (now_w as i64);

    h += add_h;
    pos_w += now_w;
    w = w.max(pos_w);

    used_space -= 0.max(global.config.line_bonus - 0.max(w.max(parent.sumw as i32) - pos_w) as i64)
        * pos_h as i64
        / 8;

    if pos_w + global.nokori_w[parent.id] >= w.max(parent.sumw as i32) {
        let add = w.max(parent.sumw as i32) - pos_w;
        pos_w += add;
        used_space += add as i64 * pos_h as i64;
    }

    let pos_c;
    if pos >= parent.lw.len() {
        pos_c = global.sigmas[parent.id];
        //hash *= global.n as u64; //1231236515;
        hash <<= 8;
        hash += (parent.id * 2 + d as usize) as u64;
        hash &= 0xFFFF;
    } else {
        pos_c = parent.lc[pos] + global.sigmas[parent.id];
    }

    if pos > 0 {
        let mul_num = if global.config.sumq_type == 1 {
            (global.config.sigma_hi - global.config.sigma_low) / 2.0
        } else {
            global.config.sigma_low
                + parent.base_id as f64 * (global.config.sigma_hi - global.config.sigma_low)
                    / (global.config.sumq_type as f64 - 1.0)
        };

        let sigma_target = pos_w + (mul_num * ((pos_c + parent.lc[pos - 1]) as f64).sqrt()) as i32;

        let sigma_dec_target =
            pos_w - (mul_num * ((pos_c + parent.lc[pos - 1]) as f64).sqrt()) as i32;

        if true {
            let mut ng_flag = false;
            let mut p = pos;
            let mut size = pos_h;
            while p > 0 {
                if parent.lh[p - 1] == 0 {
                    p -= 1;
                    continue;
                }
                if !ng_flag && sigma_target < parent.lw[p - 1] {
                    break;
                }

                if sigma_target >= parent.lw[p - 1] && sigma_dec_target <= parent.lw[p - 1] {
                    ng_flag = true;
                }
                size += parent.lh[p - 1];
                if size > global.config.marge_size && parent.sumw - pos_w as i64 >= 50000 {
                    ng_flag = true;
                }

                if ng_flag {
                    let diff = (w.max(parent.sumw as i32) as i64 - parent.lw[p - 1] as i64) as i32;
                    if diff <= 0 {
                        break;
                    }
                    final_pos = final_pos.max(p);
                    used_space += diff as i64 * (parent.lh[p - 1] as i64);
                } else {
                    let diff = pos_w - parent.lw[p - 1];
                    used_space += diff as i64 * (parent.lh[p - 1] as i64);
                }
                p -= 1;
            }
        }

        /*
        while pos - 1 > final_pos
            && parent.lw[final_pos] + global.nokori_w[parent.id] as i32 > w.max(parent.sumw as i32)
        {
            let diff = (w.max(parent.sumw as i32) as i64 - parent.lw[final_pos] as i64) as i32;
            used_space += diff as i64 * (parent.lh[final_pos] as i64);
            final_pos += 1;
        }
        */
    }

    let score = State::calculate_score(parent.id + 1, h, w, used_space, parent.sumh, global);

    ScoreState {
        parent_index,
        pos,
        d,
        pos_c,
        pos_w,
        pos_h,
        h,
        w,
        final_pos,
        hash,
        used_space,
        score,
    }
}

fn main() {
    // 入力を読み込む
    let initial_input = read_initial_input();
    let mut global_state = GlobalState::new(&initial_input);

    while initial_input.t < global_state.config.query_num * global_state.config.sumq_type
        && global_state.config.query_num >= 2
    {
        global_state.config.query_num -= 1;
    }

    while initial_input.t < global_state.config.query_num * global_state.config.sumq_type
        && global_state.config.sumq_type >= 4
    {
        global_state.config.sumq_type -= 1;
    }

    let mut t = initial_input.t;
    eprintln!("{}", t);

    // fsを必要に応じて更新
    global_state.update_fs(&mut t);

    eprintln!("{}", t);

    // ビームサーチを実行
    beam_search(&global_state, t);
}

#[derive(Debug, Clone)]
struct GlobalState {
    n: usize,
    fs: Vec<(i32, i32)>,
    sum: i64,
    nokori: Vec<i64>,
    sigma: i64,
    sigmas: Vec<i64>,
    config: Config, // Config を追加
    nokori_w: Vec<i32>,
    min_w: i32,
}

impl GlobalState {
    fn new(initial_input: &InitialInput) -> Self {
        let config = Config::default(); // デフォルト設定値を使用

        let fs = initial_input.rectangles.clone();

        let mut nokori = vec![0; initial_input.n + 1];
        for i in (0..initial_input.n).rev() {
            nokori[i] = nokori[i + 1]
                + (initial_input.rectangles[i].0 as i64) * (initial_input.rectangles[i].1 as i64);
        }

        GlobalState {
            n: initial_input.n,
            fs,
            sum: initial_input
                .rectangles
                .iter()
                .map(|&(w, h)| (w as i64) * (h as i64))
                .sum(),
            nokori,
            sigma: initial_input.sigma as i64,
            sigmas: vec![],
            config,
            nokori_w: vec![],
            min_w: 100000,
        }
    }

    fn update_fs(&mut self, t: &mut usize) {
        let calc_pattern = self.config.query_num; // StartBit の値
        let sumq_type = self.config.sumq_type; // SumqType の値

        // クエリが十分余っている場合のループ処理
        let mut add_num = if *t <= calc_pattern * sumq_type {
            0
        } else {
            *t - calc_pattern * sumq_type
        };

        let mut div_num = vec![1; self.n];

        let mut sigmas = vec![self.sigma; self.n];

        if add_num > 0 {
            let mut default = vec![0; self.n * 2];
            let mut query = vec![vec![]; self.n * 2 + add_num * 2];
            let mut result = vec![0.0; query.len()];
            for i in 0..self.n {
                default[i] = self.fs[i].0;
                default[self.n + i] = self.fs[i].1;
                query[i].push(i);
                result[i] = self.fs[i].0 as f64;
                query[self.n + i].push(self.n + i);
                result[self.n + i] = self.fs[i].1 as f64;
            }

            let seed: u64 = 42; // 任意のシード値
            let mut rng = StdRng::seed_from_u64(seed);

            for tt in 0..add_num {
                let mut tate = vec![];
                let mut yoko = vec![];
                let mut state = vec![];

                let mut lx = vec![0; self.n];
                let mut rx = vec![0; self.n];
                let mut uy = vec![0; self.n];
                let mut dy = vec![0; self.n];

                let mut prex = vec![999; self.n];
                let mut prey = vec![999; self.n];

                for i in 0..self.n {
                    let pos: usize = rng.gen_range(0..=1);
                    let d: usize = rng.gen_range(0..=1);
                    state.push((pos, d));
                    if i == 0 || pos == 0 {
                        tate.push((i, d));
                    }
                    if i == 0 || pos == 1 {
                        yoko.push((i, d));
                    }

                    let now_w = if d == 0 { self.fs[i].0 } else { self.fs[i].1 };

                    let now_h = self.fs[i].0 + self.fs[i].1 - now_w;

                    //たてならべ
                    if pos == 0 {
                        lx[i] = 0;
                        rx[i] = now_w;
                        for j in 0..i {
                            if lx[i] < rx[j] && rx[i] > lx[j] && uy[i] < dy[j] {
                                uy[i] = dy[j];
                                prey[i] = j;
                            }
                        }
                        dy[i] = uy[i] + now_h;
                    } else {
                        //よこ
                        uy[i] = 0;
                        dy[i] = now_h;
                        for j in 0..i {
                            if uy[i] < dy[j] && dy[i] > uy[j] && lx[i] < rx[j] {
                                lx[i] = rx[j];
                                prex[i] = j;
                            }
                        }
                        rx[i] = lx[i] + now_w;
                    }
                }

                let mut now_tate = tate[tate.len() - 1].0;
                while now_tate != 999 {
                    query[self.n * 2 + tt].push(now_tate + (1 - state[now_tate].1) * self.n);
                    now_tate = prey[now_tate];
                }
                let mut now_yoko = yoko[yoko.len() - 1].0;
                while now_yoko != 999 {
                    query[self.n * 2 + add_num + tt].push(now_yoko + state[now_yoko].1 * self.n);
                    now_yoko = prex[now_yoko];
                }

                //eprintln!("tate query: {}", query[self.n * 2 + tt].len());
                //eprintln!("yoko query: {}", query[self.n * 2 + add_num + tt].len());
                let mut command = vec![];
                for i in 0..self.n {
                    if state[i].0 == 0 {
                        command.push((i, state[i].1 as i32, 'U', -1));
                    } else {
                        command.push((i, state[i].1 as i32, 'L', -1));
                    }
                }

                let qret = query_output(command);
                result[self.n * 2 + tt] = qret.1 as f64;
                result[self.n * 2 + add_num + tt] = qret.0 as f64;
            }

            let mut now_num = vec![0.0; self.n * 2];
            for i in 0..2 * self.n {
                now_num[i] = result[i] as f64;
            }

            //eprintln!("{:?}", query);
            //eprintln!("{:?}", result);

            let ret = gradient_descent(&query, &result, &now_num, 0.01, 1000, 1e-6);
            let ans = ret.0;
            now_num = ans;
            //eprintln!("ans: {}", ans.1);

            //let mut mul_rnd = vec![1.0; now_num.len()];
            //for i in 0..now_num.len() {
            //    mul_rnd[i] = rng.gen_range(8000..=12000) as f64 / 10000.0;
            //}

            //let mut mulnum = 1.0;

            let mut move_len = 200.0;
            for ttt in 0..200 {
                //let mut max_move = vec![0.0; now_num.len()];
                //let mut min_move = vec![0.0; now_num.len()];
                let mut add_move = vec![0.0; now_num.len()];

                let mut diff_sum = 0.0;

                for i in 0..result.len() {
                    let mut sum = 0.0;
                    for j in query[i].iter() {
                        sum += now_num[*j] as f64;
                    }
                    let diff = (result[i] as f64 - sum);
                    //eprintln!("{}", diff);
                    diff_sum += diff * diff;
                    let add = diff as f64;
                    //let add = diff / query[i].len() as f64;
                    for j in query[i].iter() {
                        add_move[*j] += add;
                    }
                }

                //eprintln!("turn {}: {}", ttt, diff_sum);

                for i in 0..now_num.len() {
                    //let mut add = add_move[i] * 0.3 * mulnum;

                    if add_move[i] > 0.0 {
                        now_num[i] += move_len; // * rng.gen_range(8000..=12000) as f64 / 10000.0;
                                                // * mul_rnd[i];
                    }
                    if add_move[i] < 0.0 {
                        now_num[i] -= move_len; // * rng.gen_range(8000..=12000) as f64 / 10000.0;
                                                // * mul_rnd[i];
                    }
                }
                move_len *= 0.99;
            }

            let ret = gradient_descent(&query, &result, &now_num, 0.01, 1000, 1e-6);

            let next_sigma = (self.sigma as f64 * 0.4.pow(add_num as f64 / self.n as f64)) as i64;
            eprintln!("take {}", add_num);
            for i in 0..self.n {
                let a = now_num[i] as i32;
                let b = now_num[self.n + i] as i32;
                //eprintln!("({} {}) -> ({} {})", self.fs[i].0, self.fs[i].1, a, b);
                self.fs[i] = (a, b);
                sigmas[i] = next_sigma;
            }
            self.sigma = next_sigma;
            eprintln!("next_sigma: {} <- {}", next_sigma, self.sigma);
        }

        *t -= add_num;
        self.min_w = 100000;

        // fs の平均値を計算
        for i in 0..self.n {
            self.min_w = self.min_w.min(self.fs[i].0);
            self.min_w = self.min_w.min(self.fs[i].1);
        }

        // 異常値になってるやつを雑に補正する
        for i in 0..self.n {
            let mut a = self.fs[i].0;
            let mut b = self.fs[i].1;

            a = a.min(100000);
            b = b.min(100000);
            a = a.max(10000);
            b = b.max(10000);

            self.fs[i] = (a, b);
        }

        for i in 0..self.n {
            sigmas[i] = (sigmas[i] * sigmas[i]) as i64;
        }

        let mut nokori = vec![0; self.n + 1];
        let mut nokori_w = vec![0; self.n + 1];
        let mut tmp = 100000;
        for i in (0..self.n).rev() {
            nokori[i] = nokori[i + 1] + (self.fs[i].0 as i64) * (self.fs[i].1 as i64);
            tmp = tmp.min(self.fs[i].0).min(self.fs[i].1);
            nokori_w[i] = tmp;
        }
        self.nokori = nokori;
        self.sum = self.nokori[0];
        self.nokori_w = nokori_w;
        self.sigmas = sigmas;
    }
}

#[derive(Debug, Clone, Eq, PartialEq)]
struct ScoreState {
    parent_index: usize,
    pos: usize,
    d: i32,
    pos_c: i64,
    pos_w: i32,
    pos_h: i32,
    h: i32,
    w: i32,
    final_pos: usize,
    hash: u64,
    used_space: i64,
    score: i64,
}

impl Ord for ScoreState {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.score.cmp(&other.score)
    }
}

impl PartialOrd for ScoreState {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Debug, Clone)]
struct State {
    h: i32,
    w: i32,
    id: usize,
    lc: Vec<i64>,
    lw: Vec<i32>,
    lh: Vec<i32>,
    final_pos: usize,
    sumw: i64,
    sumh: i64,
    used_space: i64,
    pos: usize,
    d: i32,
    hash: u64,
    parent_index: Option<usize>, // 親のインデックスを保持
    base_id: usize,
}

impl Default for State {
    fn default() -> Self {
        State {
            h: 0,
            w: 0,
            id: 0,
            lc: Vec::new(),
            lw: Vec::new(),
            lh: Vec::new(),
            final_pos: 0,
            sumw: 0,
            sumh: 0,
            used_space: 0,
            pos: 0,
            d: 0,
            hash: 0,
            parent_index: None,
            base_id: 0,
        }
    }
}

impl State {
    // 初期状態の State を作成
    fn new_initial(index: usize, global: &GlobalState) -> Self {
        let addnum = if global.config.sumq_type == 1 {
            global.config.sum_w_range / 2.0
        } else {
            global.config.sum_w_range * index as f64 / (global.config.sumq_type - 1) as f64
        };

        let sumw =
            ((global.sum as f64).sqrt() * (1.0 - global.config.sum_w_range / 2.0 + addnum)) as i64;
        let sumh = global.sum / sumw;

        State {
            h: 0,
            w: 0,
            id: 0,
            lc: vec![],
            lw: vec![],
            lh: vec![],
            final_pos: 0,
            sumw,
            sumh,
            used_space: 0,
            pos: 0,
            d: 0,
            parent_index: None,
            hash: 0,
            base_id: index,
        }
    }

    // 次の状態を生成するためのコンストラクタ
    fn new(ss: &ScoreState, states: &Vec<State>, global: &GlobalState) -> Self {
        let parent = &states[ss.parent_index];
        let used_space = ss.used_space;
        let h = ss.h;
        let w = ss.w;

        let mut lc = parent.lc.clone();
        let mut lw = parent.lw.clone();
        let mut lh = parent.lh.clone();

        let pos_w = ss.pos_w;
        let pos_h = ss.pos_h;
        let pos_c = ss.pos_c;
        let hash = ss.hash;

        let final_pos = ss.final_pos;
        let pos = ss.pos;

        if pos >= lw.len() {
            lc.push(pos_c);
            lw.push(pos_w);
            lh.push(pos_h);
        } else {
            lc[pos] = pos_c;
            lw[pos] = pos_w;
            lh[pos] = pos_h;
        }

        if pos > 0 {
            let mul_num = if global.config.sumq_type == 1 {
                (global.config.sigma_hi - global.config.sigma_low) / 2.0
            } else {
                global.config.sigma_low
                    + parent.base_id as f64 * (global.config.sigma_hi - global.config.sigma_low)
                        / (global.config.sumq_type as f64 - 1.0)
            };

            let sigma_target = lw[pos] + (mul_num * ((lc[pos] + lc[pos - 1]) as f64).sqrt()) as i32;

            let sigma_dec_target =
                pos_w - (mul_num * ((lc[pos] + lc[pos - 1]) as f64).sqrt()) as i32;

            if true {
                let mut ng_flag = false;
                let mut p = pos;
                let mut size = pos_h;
                while p > 0 {
                    if parent.lh[p - 1] == 0 {
                        p -= 1;
                        continue;
                    }
                    if !ng_flag && sigma_target < parent.lw[p - 1] {
                        break;
                    }

                    if sigma_target >= parent.lw[p - 1] && sigma_dec_target <= parent.lw[p - 1] {
                        ng_flag = true;
                    }
                    size += parent.lh[p - 1];
                    if size >= global.config.marge_size && parent.sumw - pos_w as i64 >= 50000 {
                        ng_flag = true;
                    }

                    if ng_flag {
                        lw[p - 1] = w.max(parent.sumw as i32);
                    } else {
                        lw[p - 1] = lw[pos];
                        lh[pos] += lh[p - 1];
                        lh[p - 1] = 0;
                    }
                    p -= 1;
                }
            }
        }

        /*
        for i in parent.final_pos..ss.final_pos {
            lw[i] = w.max(parent.sumw as i32);
        }
        */

        State {
            h,
            w,
            id: parent.id + 1,
            lc,
            lw,
            lh,
            final_pos,
            sumw: parent.sumw,
            sumh: parent.sumh,
            used_space,
            pos,
            d: ss.d,
            parent_index: Some(ss.parent_index),
            hash,
            base_id: parent.base_id,
        }
    }

    fn calc_real_score(
        &self,
        states: &Vec<State>,
        global: &GlobalState,
        rand_set: &Vec<Vec<Vec<f64>>>,
    ) -> i32 {
        let mut current = Some(self);
        let mut reverse_path = Vec::new();

        // 親を辿りながら `d` と `pos` を取得して逆順にソートする
        while let Some(state) = current {
            reverse_path.push((state.id, state.pos, state.d));
            if let Some(parent_index) = state.parent_index {
                current = Some(&states[parent_index]);
            } else {
                current = None;
            }
        }
        reverse_path.pop();
        reverse_path.reverse();

        let mut sqrtsigmas = vec![];
        for i in 0..global.n {
            sqrtsigmas.push((global.sigmas[i] as f64).sqrt());
        }

        let mut pre_ids = vec![0; global.n];
        let mut prev_id = vec![99999999; self.lw.len()];
        for i in 0..global.n {
            let pos = reverse_path[i].1;
            pre_ids[i] = prev_id[pos];
            prev_id[pos] = i;
        }

        let loop_num = rand_set.len();
        let mut scores = vec![];

        let mut check_list = vec![vec![]; global.n];
        {
            let mut real_left = vec![0; global.n];
            let mut real_right = vec![0; global.n];
            let mut real_wc = vec![0; global.n];
            let mut real_hc = vec![0; global.n];
            let mut normal_dy = vec![0; global.n];
            let mut high_dy = vec![0; global.n];

            for i in 0..global.n {
                let d = reverse_path[i].2;
                let pre_id = pre_ids[i];

                if pre_id == 99999999 {
                    real_left[i] = 0;
                    real_wc[i] = global.sigmas[i];
                } else {
                    real_left[i] = real_right[pre_id];
                    real_wc[i] = global.sigmas[i] + real_wc[pre_id];
                }

                let addw = if d == 0 {
                    global.fs[i].0
                } else {
                    global.fs[i].1
                };
                let addh = global.fs[i].0 + global.fs[i].1 - addw;

                let addw = addw.max(10000).min(100000);
                let addh = addh.max(10000).min(100000);

                real_right[i] = real_left[i] + addw;

                let left_line = real_left[i];
                let right_line = real_right[i];

                normal_dy[i] = 0;
                high_dy[i] = 0;

                for j in (0..i).rev() {
                    if reverse_path[i].1 == reverse_path[j].1 {
                        continue;
                    }
                    if real_left[j] < right_line && real_right[j] > left_line {
                        normal_dy[i] = normal_dy[i].max(normal_dy[i]);
                        real_hc[i] = real_hc[i].max(real_hc[j] + global.sigmas[i]);
                    }
                    if ((real_left[j] - (((real_wc[i] + real_wc[j]) as f64).sqrt() * 3.0) as i32)
                        < right_line)
                        && ((real_right[j]
                            + (((real_wc[i] + real_wc[j]) as f64).sqrt() * 3.0) as i32)
                            > left_line)
                    {
                        high_dy[i] = high_dy[i].max(high_dy[j]);
                        real_hc[i] = real_hc[i].max(real_hc[j] + global.sigmas[i]);

                        if high_dy[j] + ((((real_hc[i] + real_hc[j]) as f64).sqrt() * 3.0) as i32)
                            > normal_dy[i]
                        {
                            check_list[i].push(j);
                        }
                    }
                }
                normal_dy[i] += addh;
                high_dy[i] += addh;
            }
        }

        for ttt in 0..loop_num {
            let mut real_left = vec![0; global.n];
            let mut real_right = vec![0; global.n];
            let mut realh = vec![0; global.n];
            let mut maxh = 0;
            let mut maxw = 0;

            for i in 0..global.n {
                let d = reverse_path[i].2;
                let pre_id = pre_ids[i];

                let r0 = (rand_set[ttt][i][0] * sqrtsigmas[i] * 1.0) as i32;
                let r1 = (rand_set[ttt][i][1] * sqrtsigmas[i] * 1.0) as i32;

                real_left[i] = if pre_id == 99999999 {
                    0
                } else {
                    real_right[pre_id]
                };

                let addw = if d == 0 {
                    global.fs[i].0 + r0
                } else {
                    global.fs[i].1 + r0
                };
                let addh = global.fs[i].0 + global.fs[i].1 - addw + r0 + r1;

                let addw = addw.max(10000).min(100000);
                let addh = addh.max(10000).min(100000);

                real_right[i] = real_left[i] + addw;

                maxw = maxw.max(real_right[i]);

                let left_line = real_left[i];
                let right_line = real_right[i];

                realh[i] = addh;

                for jj in check_list[i].iter() {
                    let j = *jj;
                    //for j in (0..i).rev() {
                    if j == pre_id {
                        continue;
                    }
                    if real_left[j] < right_line && real_right[j] > left_line {
                        realh[i] = realh[i].max(realh[j] + addh);
                        //break;
                    }
                }
                maxh = maxh.max(realh[i]);
            }

            scores.push(maxh + maxw);
        }
        scores.sort();
        scores[loop_num / 2]
    }

    // スコア計算
    fn calculate_score(
        id: usize,
        h: i32,
        w: i32,
        used_space: i64,
        sumw: i64,
        global: &GlobalState,
    ) -> i64 {
        let target_w = w.max(sumw as i32);
        let target_h = h.max(
            ((global.sum + used_space + global.nokori[id] * 7 / 60) / (target_w as i64)) as i32,
        );
        let last_score = (target_w * 21 / 20) as i64 + target_h as i64;
        //let last_score = (target_w as i64 * (1115000 - (global.min_w / 2)) as i64) / 1000000 as i64
        //    + target_h as i64;
        //last_score * last_score + used_space * (global.n - id) as i64 / (global.n / 2) as i64
        last_score * last_score + used_space
    }

    pub fn generate_commands(&self, states: &Vec<State>) -> Vec<(usize, i32, char, i32)> {
        let mut commands = Vec::new();
        let mut current = Some(self);
        let mut reverse_path = Vec::new();

        // 親を辿りながら `d` と `pos` を取得して逆順にソートする
        while let Some(state) = current {
            reverse_path.push((state.id, state.pos, state.d));
            if let Some(parent_index) = state.parent_index {
                current = Some(&states[parent_index]);
            } else {
                current = None;
            }
        }
        reverse_path.pop();
        reverse_path.reverse(); // リストを逆順にする

        // pos ごとに最後に挿入された id を補完する
        let mut last_inserted = vec![-1; self.lw.len()]; // 初期値は -1
        for (id, pos, d) in reverse_path {
            let b = last_inserted[pos]; // 直前に挿入された四角形の ID
            commands.push((id - 1, d, 'U', b));
            last_inserted[pos] = (id - 1) as i32; // 挿入 ID を更新
        }

        commands
    }
}

#[derive(Debug, Clone, Eq, PartialEq)]
struct PrioritizedState {
    score: i64,     // 小さいスコアが優先されるようにReverseを使用
    ss: ScoreState, // ScoreStateを保持
}

impl Ord for PrioritizedState {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.score.cmp(&other.score) // Reverseにより小さい値が優先される
    }
}

impl PartialOrd for PrioritizedState {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}
