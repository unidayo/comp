fn main() {
    let line1 = read_string();
    let line1: Vec<String> = line1.split_whitespace().map(|x| x.to_string()).collect();
    let n = line1[0].parse::<usize>().expect("not an integer");
    let m = line1[1].parse::<usize>().expect("not an integer");
    let e = line1[2].parse::<f64>().expect("not f64");

    let mut interactor = Interactor::new(n);

    let mut polys: Vec<Vec<(usize, usize)>> = vec![vec![]; m];
    let mut sum_d = 0;
    for i in 0..m {
        let line = read_ints::<usize>();
        let d = line[0];
        sum_d += d;
        for j in 0..d {
            polys[i].push((line[2 * j + 1], line[2 * j + 2]));
        }
    }
    let sum_d = sum_d;
    let max_time = 2500 * 1000; // micro second
    use std::time::Instant;
    let start_t = Instant::now();

    let mut ques = vec![];
    let mut place_vec: Vec<(f64, Vec<(usize, usize)>)> = vec![];

    let mut qs = vec![];

    let mut intersection_gain = [[0; 800]; 400 * 20];

    let max_trial = if (e * 100.0).round() as i32 == 1 {
        n * n / 2 - 10
    } else {
        n * n - 10
    };
    let mut places = vec![(0, 0); m];
    let mut prev_failed_try = None;

    //let coef = 119.0 / 152.0 -125.0 * e / 38.0;
    //let mut query_size = ((n * n) as f64 * coef).round() as usize;
    let coef_arr = [(1, 0.75), (3, 0.55), (5, 0.42), (12, 0.3), (20, 0.18)];
    let mut coef = 0.0;
    for i in 0..coef_arr.len() {
        let ei = (e * 100.0).round() as i32;
        if ei == coef_arr[i].0 {
            coef = coef_arr[i].1;
            break;
        }
        if ei < coef_arr[i].0 {
            // linealy
            let left = ei - coef_arr[i - 1].0;
            let right = coef_arr[i].0 - ei;
            let left = left as f64;
            let right = right as f64;
            coef = (right * coef_arr[i - 1].1 + left * coef_arr[i].1) / (left + right);
            break;
        }
    }
    let query_size = ((n * n) as f64 * coef).round() as usize;
    {
        // first query is random
        let qu = get_distinguish_query(n, &[(0.0, places.clone())], &polys, query_size, 2024);
        let v = interactor.query2(&qu);
        qs.push((v as usize, qu));
    }
    for trial in 0..max_trial {
        if start_t.elapsed().as_micros() > 2900 * 1000 {
            println!("# truncated due to timeout");
            break;
        }
        // update intersection gain
        {
            let coef2 = ((1.0 - 2.0 * e) * 100.0).round() as i32;
            let mut query_board = [[0; 20]; 20];
            for &(qx, qy) in qs.last().unwrap().1.iter() {
                query_board[qx][qy] += 1;
            }
            let j = qs.len() - 1;
            for i in 0..m {
                for x in 0..n {
                    for y in 0..n {
                        let key = 400 * i + 20 * x + y;
                        let mut sum = 0;
                        let mut boundary = true;
                        for &(cx, cy) in polys[i].iter() {
                            if x + cx >= n || y + cy >= n {
                                boundary = false;
                                break;
                            }
                            sum += query_board[x + cx][y + cy];
                        }
                        if !boundary {
                            continue;
                        }
                        intersection_gain[key][j] = sum * coef2;
                    }
                }
            }
        }
        // update weights
        {
            let coef1 = (e * 100.0).round() as i32;
            for i in 0..place_vec.len() {
                let mut sum = 0;
                for j in 0..m {
                    let (x, y) = place_vec[i].1[j];
                    let key = 400 * j + 20 * x + y;
                    sum += intersection_gain[key][qs.len() - 1];
                }
                let (v, points) = qs.last().unwrap();
                let k = points.len() as i32;
                let mu = k * coef1 + sum;
                let v = *v as i32;
                let diff = mu - v * 100;
                place_vec[i].0 += (-diff * diff) as f64;
            }
            place_vec.reverse();
            place_vec.sort_by(|a, b| {
                let lp1 = a.0;
                let lp2 = b.0;
                lp1.partial_cmp(&lp2).unwrap()
            });
            place_vec.reverse();
        }

        //
        let mut res = vec![vec![false; n]; n];

        let mut signif = true;
        let mut tl = max_time / max_trial as u128;
        if trial <= 4 {
            tl += 60 * 1000;
        }
        if start_t.elapsed().as_micros() > (max_time + 60 * 1000 * 5) {
            // do nothing
        } else {
            place_vec = hill_climbing(
                place_vec,
                n,
                e,
                &polys,
                &intersection_gain,
                &qs,
                2024 * trial as u64,
                tl,
            );
        }

        place_vec = remove_duplication(&place_vec, &polys);

        let mut likelihoods = vec![];
        let st2 = Instant::now();
        let mut most_probable = None;
        let mut second_probable = None;
        for i in 0..place_vec.len() {
            /* if prev_failed_try.is_some() && prev_failed_try.as_ref().unwrap() == &place_vec[i] {
                // ignore
                continue;
            }*/
            if most_probable.is_none() {
                most_probable = Some(place_vec[i].1.clone());
            } else if second_probable.is_none() {
                second_probable = Some(place_vec[i].1.clone());
            }
            let vs = get_probabilities_given_solution(&place_vec[i].1, &qs, &polys, n, e);
            /* eprintln!(
                "# estimation {i} : {:?}",
                vs
            );*/
            let mut likelihood = 0.0;
            for &w in vs.iter() {
                likelihood += w.ln();
            }
            println!("# likelihood {i} : {likelihood}");
            likelihoods.push(likelihood);
            if likelihoods.len() >= 2 {
                break;
            }
        }
        println!("# time to calc likelihoods : {}", st2.elapsed().as_micros());
        if likelihoods[0] < likelihoods[1] + 3.0 {
            signif = false;
        }
        let most_probable = most_probable.unwrap();
        let second_probable = second_probable.unwrap();
        // ask in order to distinguish first and second
        places = most_probable.clone();
        if !signif {
            let up = 28.min(place_vec.len());
            let qu = get_distinguish_query(n, &place_vec[0..up], &polys, query_size, trial as u64);
            let v = interactor.query2(&qu);
            qs.push((v as usize, qu));
        }
        if
        /*start_t.elapsed().as_micros() < (max_time + 60 * 5 * 1000)
        &&*/
        trial < max_trial - 1 && !signif {
            println!("# trial {trial} not significant to answer");
            continue;
        }

        let mut can_answer = true;
        if prev_failed_try.is_some() && prev_failed_try.unwrap() == places {
            if start_t.elapsed().as_micros() >= (max_time + 60 * 5 * 1000) && trial == max_trial - 1
            {
                places = second_probable.clone();
            } else {
                can_answer = false;
            }
        }
        for i in 0..m {
            for &(x, y) in polys[i].iter() {
                let cx = x + places[i].0;
                let cy = y + places[i].1;
                res[cx][cy] = true;
            }
        }
        let mut res2 = vec![];
        for x in 0..n {
            for y in 0..n {
                if res[x][y] {
                    res2.push((x, y));
                }
            }
        }
        ques.push(qs.clone());
        if can_answer && interactor.query3(&res2) == 1 {
            std::process::exit(0);
        } else {
            let up = 28.min(place_vec.len());
            let qu = get_distinguish_query(n, &place_vec[0..up], &polys, query_size, trial as u64);
            let v = interactor.query2(&qu);
            qs.push((v as usize, qu));

            prev_failed_try = Some(places.clone());
        }
    }
    /* {
        // in order to finished interactor
        for _ in 0..2 * n * n {
            println!("q 1 0 0");
        }
    }*/
    // escape
    let mut ask11 = vec![vec![false; n]; n];
    let mut yuden = vec![vec![false; n]; n];
    use std::collections::*;
    let mut que = VecDeque::new();
    for i in 0..m {
        for &(cx, cy) in polys[i].iter() {
            let x = cx + places[i].0;
            let y = cy + places[i].1;
            que.push_back((x, y));
        }
    }
    let mut sum = 0;
    while que.len() > 0 {
        let (x, y) = que.pop_front().unwrap();
        if ask11[x][y] {
            continue;
        }
        let v = interactor.query1((x, y));
        ask11[x][y] = true;
        if v == 0 {
            continue;
        }
        yuden[x][y] = true;
        sum += v;
        if sum == sum_d {
            break;
        }
        for &(dx, dy) in [(-1, 0), (1, 0), (0, -1), (0, 1)].iter() {
            let nx = x as i32 + dx;
            let ny = y as i32 + dy;
            if nx < 0 || ny < 0 {
                continue;
            }
            let nx = nx as usize;
            let ny = ny as usize;
            if nx >= n || ny >= n {
                continue;
            }
            if ask11[nx][ny] {
                continue;
            }
            que.push_back((nx, ny));
        }
    }
    if sum == sum_d {
        // finished
        let mut res2 = vec![];
        for x in 0..n {
            for y in 0..n {
                if yuden[x][y] {
                    res2.push((x, y));
                }
            }
        }
        if interactor.query3(&res2) == 1 {
            std::process::exit(0);
        } else {
            panic!();
        }
    }

    // last
    'outer2: for x in 0..n {
        for y in 0..n {
            if ask11[x][y] {
                continue;
            }
            let v = interactor.query1((x, y));
            ask11[x][y] = true;
            if v == 0 {
                continue;
            }
            yuden[x][y] = true;
            sum += 1;
            if sum == sum_d {
                break 'outer2;
            }
        }
    }
    let mut res2 = vec![];
    for x in 0..n {
        for y in 0..n {
            if yuden[x][y] {
                res2.push((x, y));
            }
        }
    }
    if interactor.query3(&res2) == 1 {
        std::process::exit(0);
    }
    panic!();
}
fn remove_duplication(
    place_vec: &Vec<(f64, Vec<(usize, usize)>)>,
    polys: &Vec<Vec<(usize, usize)>>,
) -> Vec<(f64, Vec<(usize, usize)>)> {
    let mut res = vec![];

    res.push(place_vec[0].clone());
    for i in 1..place_vec.len() {
        if place_vec[i].0 != place_vec[i - 1].0 {
            res.push(place_vec[i].clone());
            continue;
        }
        if is_same_board(&place_vec[i - 1].1, &place_vec[i].1, polys) {
            continue;
        }
        res.push(place_vec[i].clone());
    }
    res
}
fn get_distinguish_query(
    n: usize,
    res: &[(f64, Vec<(usize, usize)>)],
    polys: &Vec<Vec<(usize, usize)>>,
    cnt: usize,
    seed: u64,
) -> Vec<(usize, usize)> {
    let m = polys.len();
    let mut table = [[0i128; 20]; 20];
    let mut table2 = [[0i128; 20]; 20];

    let mut coef = 1i128;
    for sample in res.iter().rev() {
        for i in 0..m {
            let (dx, dy) = sample.1[i];
            for &(cx, cy) in polys[i].iter() {
                let x = cx + dx;
                let y = cy + dy;
                table[x][y] += coef;
            }
        }
        for i in 0..m {
            let (dx, dy) = res[0].1[i];
            for &(cx, cy) in polys[i].iter() {
                let x = cx + dx;
                let y = cy + dy;
                table2[x][y] += coef;
            }
        }
        coef *= 21;
        // eprintln!("coef {coef}");
    }

    use rand::prelude::SliceRandom;
    use rand::rngs::StdRng;
    use rand::SeedableRng;

    let mut rr = StdRng::seed_from_u64(seed);

    let mut ord = vec![];
    for x in 0..n {
        for y in 0..n {
            ord.push((x, y));
        }
    }
    ord.shuffle(&mut rr);
    ord.sort_by(|p1, p2| {
        let p1 = *p1;
        let p2 = *p2;
        let v1 = table2[p1.0][p1.1] - table[p1.0][p1.1];
        let v2 = table2[p2.0][p2.1] - table[p2.0][p2.1];
        v1.cmp(&v2)
    });
    ord.reverse();
    let mut res = vec![];
    for &(x, y) in ord.iter() {
        res.push((x, y));
        if res.len() == cnt {
            return res;
        }
    }
    res
}
fn is_same_board(
    res1: &Vec<(usize, usize)>,
    res2: &Vec<(usize, usize)>,
    polys: &Vec<Vec<(usize, usize)>>,
) -> bool {
    let mut board1 = [[0; 20]; 20];
    let mut board2 = [[0; 20]; 20];
    let m = polys.len();
    for i in 0..m {
        let (cx, cy) = res1[i];
        for &(px, py) in polys[i].iter() {
            board1[cx + px][cy + py] += 1;
        }
        let (cx, cy) = res2[i];
        for &(px, py) in polys[i].iter() {
            board2[cx + px][cy + py] += 1;
        }
    }
    board1 == board2
}
fn get_probabilities_given_solution(
    est: &Vec<(usize, usize)>,
    query: &Vec<(usize, Vec<(usize, usize)>)>,
    polys: &Vec<Vec<(usize, usize)>>,
    n: usize,
    e: f64,
) -> Vec<f64> {
    let mut table = vec![vec![0; n]; n];
    let m = est.len();
    for i in 0..m {
        for &(cx, cy) in polys[i].iter() {
            let x = cx + est[i].0;
            let y = cy + est[i].1;
            table[x][y] += 1;
        }
    }

    let mut res = vec![];
    for (v, points) in query.iter() {
        let v = *v;
        let mut sum = 0;
        for &(cx, cy) in points.iter() {
            sum += table[cx][cy];
        }
        let k = points.len() as f64;
        let sum = sum as f64;
        let sigma = k * e * (1.0 - e);
        let mu = (k - sum) * e + sum * (1.0 - e);
        let value = calc_prob(v, sigma.sqrt(), mu);
        res.push(value);
    }
    res
}
fn hill_climbing(
    prev_que: Vec<(f64, Vec<(usize, usize)>)>,
    n: usize,
    e: f64,
    polys: &Vec<Vec<(usize, usize)>>,
    intersection_gain: &[[i32; 800]; 400 * 20],
    qs: &Vec<(usize, Vec<(usize, usize)>)>,
    seed: u64,
    tl: u128,
) -> Vec<(f64, Vec<(usize, usize)>)> {
    use std::time::Instant;
    let start_t = Instant::now();
    // use rand::prelude::SliceRandom;
    use rand::rngs::StdRng;
    use rand::Rng;
    use rand::SeedableRng;

    let mut rr = StdRng::seed_from_u64(seed);
    let m = polys.len();
    let mut loopc = 0;

    // calc dp
    let mut within_boundary: Vec<bool> = vec![true; 400 * m];
    let coef1 = (e * 100.0).round() as i32;
    {
        for i in 0..m {
            for x in 0..n {
                for y in 0..n {
                    let key = 400 * i + 20 * x + y;
                    let mut boundary = true;
                    for &(cx, cy) in polys[i].iter() {
                        if x + cx >= n || y + cy >= n {
                            boundary = false;
                            break;
                        }
                    }
                    if !boundary {
                        within_boundary[key] = false;
                        continue;
                    }
                }
            }
        }
    }

    let num_query = qs.len();

    // que storing best results
    let mut best_que: Vec<(f64, Vec<(usize, usize)>)> = prev_que;
    let que_size = 200;
    /* for (_, ve) in prev_que.iter() {
        // calc score
        let mut log_prob = 0;
        for (j, (v, points)) in qs.iter().enumerate() {
            let mut sum = 0;
            for i in 0..m {
                let (x, y) = ve[i];
                let key = 400 * i + 20 * x + y;
                sum += intersection_gain[key][j];
            }
            let k = points.len() as i32;
            let mu = k * coef1 + sum;
            let v = *v as i32;
            let diff = mu - v * 100;
            log_prob += -diff * diff;
        }
        let log_prob = log_prob as f64;
        best_que.push((log_prob, ve.clone()));
    }
    // eprintln!("{}", &best_que == prev_que);
    best_que.sort_by(|a, b| {
        let lp1 = a.0;
        let lp2 = b.0;
        lp1.partial_cmp(&lp2).unwrap()
    });
    best_que.reverse();
    assert_eq!(best_que.iter().map(|x| x.0).collect::<Vec<_>>(), prev_que.iter().map(|x| x.0).collect::<Vec<_>>());*/
    use std::collections::VecDeque;
    let mut best_stack = best_que
        .iter() /* .rev()*/
        .map(|x| x.1.clone())
        .collect::<VecDeque<_>>();
    /* while best_stack.len() > 1 {
        best_stack.pop_front();
    }*/
    // best_stack.reverse();

    // println!("# time to construct : {} (in {tl})", start_t.elapsed().as_micros());
    let mut ord: Vec<usize> = (0..m).collect();
    ord.sort_by(|x, y| {
        let v1 = polys[*x].len();
        let v2 = polys[*y].len();
        v1.cmp(&v2)
    });
    ord.reverse();
    loop {
        if start_t.elapsed().as_micros() > tl {
            break;
        }
        loopc += 1;
        let mut cur_sol: Vec<(usize, usize)> = vec![(0, 0); m];
        let mut sums = vec![0; num_query];
        for i in 0..m {
            if best_stack.len() > 0 {
                let (x, y) = best_stack.back().unwrap()[i];
                cur_sol[i] = (x, y);
                // add sums
                let key = 400 * i + 20 * x + y;
                for j in 0..qs.len() {
                    sums[j] += intersection_gain[key][j];
                }
                continue;
            }

            loop {
                let x = rr.gen_range(0..n);
                let y = rr.gen_range(0..n);
                let mut border_check = true;
                for &(cx, cy) in polys[i].iter() {
                    if x + cx >= n || y + cy >= n {
                        border_check = false;
                        break;
                    }
                }
                if border_check {
                    cur_sol[i] = (x, y);
                    // add sums
                    let key = 400 * i + 20 * x + y;
                    for j in 0..qs.len() {
                        sums[j] += intersection_gain[key][j];
                    }
                    break;
                }
            }
        }
        if best_stack.len() > 0 {
            best_stack.pop_back();
        }

        // calculate current score
        let mut score = 0;
        for j in 0..qs.len() {
            let v = qs[j].0 as i32;
            let k = qs[j].1.len() as i32;
            let diff = (k * coef1 + sums[j]) - v as i32 * 100;
            score -= diff * diff;
        }
        // eprintln!("score : {score}");

        let mut vec1 = [0; 800];

        let beam_width = 5;
        let mut beam_stack = vec![];
        beam_stack.push((score, cur_sol.clone(), sums.clone()));
        for &i in ord.iter() {
            // eprintln!("{i} bs {}", beam_stack.len());
            let mut next_beam_stack: Vec<(i32, Vec<(usize, usize)>, Vec<i32>)> = vec![];
            for (_, cur_sol, mut sums) in beam_stack {
                // check for debug
                /* let mut score2 = 0;
                for j in 0..qs.len() {
                    let v = qs[j].0 as i32;
                    let k = qs[j].1.len() as i32;
                    let diff = (k * coef1 + sums[j]) - v as i32 * 100;
                    score2 -= diff * diff;
                }
                assert_eq!(score, score2);*/
                // subtract sums
                let (px, py) = cur_sol[i];
                let key = 400 * i + 20 * px + py;
                for j in 0..qs.len() {
                    sums[j] -= intersection_gain[key][j];
                }
                // place greedy

                for j in 0..qs.len() {
                    vec1[j] = qs[j].1.len() as i32 * coef1 + sums[j] as i32 - qs[j].0 as i32 * 100;
                }

                for x in 0..n {
                    for y in 0..n {
                        let key = 400 * i + 20 * x + y;
                        if !within_boundary[key] {
                            break;
                        }

                        // calc score
                        let mut log_prob = 0;
                        let vec2 = &intersection_gain[key];
                        for j in 0..qs.len() {
                            /* let sum = sums[j] + intersection_gain[key][j];

                            let k = points.len() as f64;
                            let sum = sum as f64;
                            // let sigma = k * e * (1.0 - e);
                            let mu = (k - sum) * e + sum * (1.0 - e);
                            let value = calc_prob3(v, 0.0, mu);*/

                            let c = vec1[j] + vec2[j];
                            let value = c * c;
                            log_prob += value;
                        }
                        log_prob = -log_prob;
                        let log_prob = log_prob as f64;

                        if best_que.len() == que_size && best_que.last().unwrap().0 >= log_prob {
                            // do nothing
                        } else {
                            // add the solution to the que
                            let mut index = best_que.len();
                            let mut sol = cur_sol.clone();
                            sol[i] = (x, y);
                            let mut flag = true;
                            for i in 0..best_que.len() {
                                if best_que[i].1 == sol {
                                    flag = false;
                                    break;
                                }
                                if best_que[i].0 < log_prob {
                                    index = i;
                                    break;
                                }
                            }
                            if !flag {
                                // already in the que
                            } else {
                                best_stack.push_back(sol.clone());
                                best_que.insert(index, (log_prob, sol));
                                if best_que.len() > que_size {
                                    best_que.pop();
                                }
                            }
                        }

                        let log_prob = log_prob.round() as i32;
                        // add to next beam stack
                        if next_beam_stack.len() == beam_width
                            && next_beam_stack.last().unwrap().0 >= log_prob
                        {
                            // do nothing
                        } else {
                            // insert this state
                            let mut index = next_beam_stack.len();
                            let mut sol = cur_sol.clone();
                            sol[i] = (x, y);
                            let mut flag = true;
                            for i in 0..next_beam_stack.len() {
                                if next_beam_stack[i].1 == sol {
                                    flag = false;
                                    break;
                                }
                                if next_beam_stack[i].0 < log_prob {
                                    index = i;
                                    break;
                                }
                            }
                            if !flag {
                                // already in stack
                            } else {
                                // update sums
                                let mut next_sums = sums.clone();
                                let key = 400 * i + 20 * x + y;
                                for j in 0..qs.len() {
                                    next_sums[j] += intersection_gain[key][j];
                                }
                                next_beam_stack.insert(index, (log_prob, sol, next_sums));
                                if next_beam_stack.len() > beam_width {
                                    next_beam_stack.pop();
                                }
                            }
                        }
                    }
                }
            }
            beam_stack = next_beam_stack;
        }
    }
    println!("# hill count : {loopc}");
    println!("# que : {:?}", &best_que[0..5]);
    best_que
}
fn calc_prob(v: usize, sigma: f64, mu: f64) -> f64 {
    let f = |x: f64| -> f64 {
        let x = x - mu;
        let num = (-x * x / (2.0 * sigma * sigma)).exp();
        let den = sigma;
        num / den
    };
    let integral2 = |x: f64, y: f64| -> f64 {
        let mx = 100;
        let mut res = 0.0;
        for i in 0..mx {
            let left = x + (y - x) * i as f64 / mx as f64;
            let right = x + (y - x) * (i + 1) as f64 / mx as f64;
            let mid = (left + right) / 2.0;
            let v = (f(left) + 4.0 * f(mid) + f(right)) * (right - left) / 6.0;
            res += v;
        }
        res / (2.0 * std::f64::consts::PI).sqrt()
    };
    if v == 0 {
        return integral2(-6.0, 0.5);
    }
    let left = v as f64 - 0.5;
    let right = v as f64 + 0.5;
    integral2(left, right)
}
/*
fn calc_prob3(v: usize, _sigma: f64, mu: f64) -> f64 {
    let x = v as f64 - mu;
    -x * x
}*/
struct Interactor {
    rem: usize,
}
impl Interactor {
    fn new(n: usize) -> Self {
        Self { rem: 2 * n * n }
    }
    fn query1(&mut self, (x, y): (usize, usize)) -> usize {
        self.rem -= 1;
        use std::io::Write;
        println!("q 1 {x} {y}");
        std::io::stdout().flush().unwrap();
        let res = read_int::<usize>();
        if self.rem == 0 {
            eprintln!("consumed all queries");
            std::process::exit(0);
        }
        res
    }
    fn query2(&mut self, ps: &Vec<(usize, usize)>) -> usize {
        self.rem -= 1;
        use std::io::Write;
        print!("q {} ", ps.len());
        for &(x, y) in ps.iter() {
            print!("{x} {y} ");
        }
        println!();
        std::io::stdout().flush().unwrap();
        let res = read_int::<usize>();
        if self.rem == 0 {
            eprintln!("consumed all queries");
            std::process::exit(0);
        }
        res
    }
    fn query3(&mut self, ps: &Vec<(usize, usize)>) -> usize {
        self.rem -= 1;
        use std::io::Write;
        print!("a {} ", ps.len());
        for &(x, y) in ps.iter() {
            print!("{x} {y} ");
        }
        println!();
        std::io::stdout().flush().unwrap();
        let res = read_int::<usize>();
        res
    }
}
fn read_ints<T: std::str::FromStr>() -> Vec<T>
where
    T::Err: std::fmt::Debug,
{
    let x = read_string()
        .split_whitespace()
        .map(|y| y.parse().expect("not an integer"))
        .collect();
    x
}
fn read_int<T: std::str::FromStr>() -> T
where
    T::Err: std::fmt::Debug,
{
    let x = read_string().parse().expect("not an integer");
    x
}
fn read_string() -> String {
    let mut x = String::new();
    std::io::stdin().read_line(&mut x).unwrap();
    x.trim().to_string()
}
