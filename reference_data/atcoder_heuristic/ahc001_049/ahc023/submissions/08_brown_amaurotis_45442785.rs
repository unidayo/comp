use rand::distributions::Uniform;
use rand::prelude::*;
use rand::seq::SliceRandom;
use std::collections::VecDeque;
use std::time::Instant;

fn main() {
    let [_, _, _, i0] = read_ints::<usize>()[..] else {
        panic!()
    };
    let n = 20;
    let start_time = Instant::now();
    let mut down_walls: Vec<Vec<bool>> = vec![vec![false; n]; n - 1];
    let mut right_walls: Vec<Vec<bool>> = vec![vec![false; n - 1]; n];
    for i in 0..(n - 1) {
        let bits: Vec<char> = read_string().chars().collect();
        for j in 0..n {
            down_walls[i][j] = bits[j] != '0';
        }
    }
    for i in 0..n {
        let bits: Vec<char> = read_string().chars().collect();
        for j in 0..(n - 1) {
            right_walls[i][j] = bits[j] != '0';
        }
    }
    let k: usize = read_string().parse().expect("not an integer");
    let mut crops: Vec<(usize, usize)> = vec![];
    for _ in 0..k {
        let [s, d] = read_ints()[..] else { panic!() };
        crops.push((s, d));
    }

    let mut rr: rand::rngs::StdRng = rand::rngs::StdRng::seed_from_u64(2023);
    let mut max_score = 0;
    let mut max_res = vec![];
    for num in 0..2 {
        let is_first = if num == 0 { true } else { false };
        let (res, mut score) = solve(i0, k, &crops, &down_walls, &right_walls, &mut rr, &start_time, is_first);
        if score > max_score {
            max_score = score;
            max_res = res;
        }
        eprintln!("{num} : {}", score);
    }
    let res = max_res;
    let mut score = max_score;
    let feasibility = check_feasiability((i0, 0), &down_walls, &right_walls, &crops, &res);
    if !feasibility {
        score = 0;
    }
    println!("{}", res.len());
    for &(i, x, y, s) in res.iter() {
        println!("{} {} {} {}", i + 1, x, y, s);
    }
    eprintln!("feasibility = {}", feasibility);
    eprintln!("Score = {}", score);
}
fn get_good_tree(
    i0: usize,
    down_walls: &Vec<Vec<bool>>,
    right_walls: &Vec<Vec<bool>>,
    seed: u64,
) -> Vec<Vec<(usize, usize)>> {
    let start_time = Instant::now();
    let mut rr: rand::rngs::StdRng = rand::rngs::StdRng::seed_from_u64(seed);
    let n = 20;
    let root: (usize, usize) = (i0, 0);

    let directions = [(-1, 0), (1, 0), (0, -1), (0, 1)];

    let is_movable = |pos: (usize, usize), dir: (i32, i32)| -> bool {
        let (x, y) = pos;
        let (dx, dy) = dir;
        let nx = x as i32 + dx;
        let ny = y as i32 + dy;
        if nx < 0 || nx >= n as i32 || ny < 0 || ny >= n as i32 {
            return false;
        }
        let nx = nx as usize;
        let ny = ny as usize;
        if dx == 1 {
            if down_walls[x][y] {
                return false;
            }
            return true;
        }
        if dy == 1 {
            if right_walls[x][y] {
                return false;
            }
            return true;
        }
        if dx == -1 {
            if down_walls[nx][ny] {
                return false;
            }
            return true;
        }
        if dy == -1 {
            if right_walls[nx][ny] {
                return false;
            }
            return true;
        }
        panic!();
    };
    let mut dep: Vec<Vec<usize>> = vec![vec![0; n]; n];
    {
        // calc depth
        let mut pars: Vec<Vec<(usize, usize)>> = vec![vec![(100, 100); n]; n];
        let mut st: VecDeque<(usize, usize)> = VecDeque::new();
        st.push_back((i0, 0));
        pars[i0][0] = (i0, 0);
        while let Some(&(x, y)) = st.front() {
            st.pop_front();
            for &(dx, dy) in directions.iter() {
                if !is_movable((x, y), (dx, dy)) {
                    continue;
                }
                let nx = (x as i32 + dx) as usize;
                let ny = (y as i32 + dy) as usize;
                if pars[nx][ny] == (100, 100) {
                    pars[nx][ny] = (x, y);
                    st.push_back((nx, ny));
                    dep[nx][ny] = dep[x][y] + 1;
                }
            }
        }
    }

    let mut distant_ord_xy: Vec<(usize, usize)> = vec![];
    for i in (0..n).rev() {
        for j in (0..n).rev() {
            distant_ord_xy.push((i, j));
        }
    }
    distant_ord_xy.sort_by(|a, b| dep[a.0][a.1].cmp(&dep[b.0][b.1]));
    distant_ord_xy.reverse();

    let mut log_table: Vec<f64> = vec![0.0; 500];
    for i in 0..500 {
        log_table[i] = ((i + 1) as f64).ln();
    }

    let mut max_score: f64 = 0.0;
    let mut res: Vec<Vec<(usize, usize)>> = vec![vec![(100, 100); n]; n];
    // let mut hill_cnt = 0;
    for lambda in 0..10 {
        // get bfs tree
        let mut pars: Vec<Vec<(usize, usize)>> = vec![vec![(100, 100); n]; n];
        let mut st: VecDeque<(usize, usize)> = VecDeque::new();
        st.push_back((i0, 0));
        pars[i0][0] = (i0, 0);
        while let Some(&(x, y)) = st.front() {
            st.pop_front();
            let mut ord = (0..4).collect::<Vec<usize>>();
            ord.shuffle(&mut rr);
            for &i in ord.iter() {
                let (dx, dy) = directions[i];
                if !is_movable((x, y), (dx, dy)) {
                    continue;
                }
                let nx = (x as i32 + dx) as usize;
                let ny = (y as i32 + dy) as usize;
                if pars[nx][ny] == (100, 100) {
                    pars[nx][ny] = (x, y);
                    st.push_back((nx, ny));
                } else if dep[nx][ny] == dep[x][y] + 1 {
                    pars[nx][ny] = (x, y);
                }
            }
        }
        let mut num_subs: Vec<Vec<usize>> = vec![vec![1; n]; n];
        for &(x, y) in distant_ord_xy.iter() {
            if (x, y) == (i0, 0) {
                continue;
            }
            let (px, py) = pars[x][y];
            num_subs[px][py] += num_subs[x][y];
        }
        let mut sum2: f64 = 0.0;
        for x in 0..n {
            for y in 0..n {
                sum2 += (num_subs[x][y] as f64) * log_table[num_subs[x][y]];
            }
        }
        let mut max_sum2 = sum2;

        fn change_par(
            (x, y): (usize, usize),
            (nx, ny): (usize, usize),
            num_subs: &mut Vec<Vec<usize>>,
            pars: &mut Vec<Vec<(usize, usize)>>,
            log_table: &Vec<f64>,
        ) -> f64 {
            // link (x, y) -> (px, py) (this fn does not change pars)
            // return change of sum2
            let gain = num_subs[x][y] as i32;
            let mut res = 0.0;
            let (mut px1, mut py1) = pars[x][y];
            let (mut px2, mut py2) = (nx, ny);
            while (px1, py1) != (px2, py2) {
                // cut
                res -= (num_subs[px1][py1] as f64) * log_table[num_subs[px1][py1]];
                num_subs[px1][py1] = (num_subs[px1][py1] as i32 - gain) as usize;
                res += (num_subs[px1][py1] as f64) * log_table[num_subs[px1][py1]];
                // link
                res -= (num_subs[px2][py2] as f64) * log_table[num_subs[px2][py2]];
                num_subs[px2][py2] = (num_subs[px2][py2] as i32 + gain) as usize;
                res += (num_subs[px2][py2] as f64) * log_table[num_subs[px2][py2]];

                (px1, py1) = pars[px1][py1];
                (px2, py2) = pars[px2][py2];
            }
            pars[x][y] = (nx, ny);
            res
        }

        // hill climbing
        let uni = Uniform::new(0, 20);
        loop {
            // hill_cnt += 1;
            if start_time.elapsed().as_micros() > 8000 * (lambda + 1) {
                if max_sum2 > max_score {
                    max_score = max_sum2;
                    res = pars;
                }
                // eprintln!("{}", max_sum2);
                break;
            }
            let x = uni.sample(&mut rr);
            let y = uni.sample(&mut rr);
            if (x, y) == root {
                continue;
            }
            let (px, py) = pars[x][y];
            let mut npar: Option<(usize, usize)> = None;
            for &(dx, dy) in directions.iter() {
                if !is_movable((x, y), (dx, dy)) {
                    continue;
                }
                let nx = (x as i32 + dx) as usize;
                let ny = (y as i32 + dy) as usize;
                if dep[x][y] != (dep[nx][ny] + 1) {
                    continue;
                }
                if (nx, ny) == (px, py) {
                    continue;
                }
                npar = Some((nx, ny));
            }
            if npar.is_none() {
                continue;
            }
            let (nx, ny) = npar.unwrap();
            // change (x, y) -> (px, py) to (x, y) -> (nx, ny)
            sum2 += change_par((x, y), (nx, ny), &mut num_subs, &mut pars, &log_table);

            if sum2 > max_sum2 {
                // eprintln!("gained {} {}", max_sum2, sum2);
                max_sum2 = sum2;
            } else {
                // revert
                sum2 += change_par((x, y), (px, py), &mut num_subs, &mut pars, &log_table);
            }
        }
        // dbg!(sum2);
    }
    // eprintln!("{}", hill_cnt);
    // eprintln!("{}", max_score);
    res
}
fn solve(
    i0: usize,
    _k: usize,
    crops: &[(usize, usize)],
    down_walls: &Vec<Vec<bool>>,
    right_walls: &Vec<Vec<bool>>,
    rr: &mut rand::rngs::StdRng,
    start_time: &Instant,
    is_first: bool,
) -> (Vec<(usize, usize, usize, usize)>, i64) {
    let root: (usize, usize) = (i0, 0);
    let n = 20;
    let t = 101;

    let seed: u64 = start_time.elapsed().as_millis() as u64;
    let pars = get_good_tree(i0, down_walls, right_walls, seed);

    let mut forbid_left: [[u128; 20]; 20] = [[0; 20]; 20];
    let mut forbid_right: [[u128; 20]; 20] = [[0; 20]; 20];

    let mut score: i64 = 0;

    let mut crop_cnt = [[0; 102]; 102];
    let mut crop_memo: Vec<Vec<Vec<(usize, usize)>>> = vec![vec![vec![]; n]; n];
    for &(s, d) in crops.iter() {
        crop_cnt[s][d] += 1;
    }

    // chs
    let mut chs: Vec<Vec<Vec<(usize, usize)>>> = vec![vec![vec![]; n]; n];
    for x in 0..n {
        for y in 0..n {
            if (x, y) == (i0, 0) {
                continue;
            }
            let (px, py) = pars[x][y];
            chs[px][py].push((x, y));
        }
    }
    let mut hill_cnt = 0;
    let mut last_used_time: [[usize; 20]; 20] = [[0; 20]; 20];
    let mut selected_cnt: [[usize; 20]; 20] = [[0; 20]; 20];

    for num in 0..1 {
        let mut living_child: Vec<Vec<usize>> = vec![vec![0; 20]; 20];
        let mut leaves: Vec<(usize, (usize, usize))> = vec![];
        for x in 0..n {
            for y in 0..n {
                if chs[x][y].len() > 0 {
                    continue;
                }
                let (mut cx, mut cy) = (x, y);
                let mut dep = 0;
                loop {
                    living_child[cx][cy] += 1;
                    dep += 1;
                    if (cx, cy) == root {
                        break;
                    }
                    (cx, cy) = pars[cx][cy];
                }
                leaves.push((dep, (x, y)));
            }
        }
        leaves.sort();
        leaves.reverse();
        for &(dep, (rx, ry)) in leaves.iter() {
            let mut candidates: Vec<(usize, usize)> = vec![];
            if start_time.elapsed().as_millis() > 1950 {
                break;
            }

            {
                if chs[rx][ry].len() > 0 {
                    continue;
                }
                if num > 0 && dep > 20 {
                    continue;
                }

                let (mut cx, mut cy) = (rx, ry);
                let mut t_cnt = 0;
                loop {
                    if living_child[cx][cy] > 2 {
                        t_cnt += 1;
                        if num == 0 && t_cnt > 20 {
                            break;
                        }
                    }
                    candidates.push((cx, cy));
                    if (cx, cy) == root {
                        break;
                    }
                    (cx, cy) = pars[cx][cy];
                }
            }
            // clear
            for &(x, y) in candidates.iter() {
                for &(s, d) in crop_memo[x][y].iter() {
                    score -= (d - s + 1) as i64;
                    crop_cnt[s][d] += 1;
                }
                crop_memo[x][y].clear();

                forbid_left[x][y] = 0;
                forbid_right[x][y] = 0;
            }
            // recalc bit masks
            for &(x, y) in candidates.iter() {
                for &(nx, ny) in chs[x][y].iter() {
                    forbid_left[x][y] |= forbid_left[nx][ny];
                    forbid_right[x][y] |= forbid_right[nx][ny];
                }
            }
            // reconstruct
            candidates.reverse();
            let mut dp1: [[[usize; 102]; 102]; 100] = [[[0; 102]; 102]; 100];
            let mut dp2: [[[usize; 102]; 102]; 100] = [[[0; 102]; 102]; 100];
            let mut left_bound_table: [usize; 102] = [102; 102];
            let mut cum_left = 102;
            for i in (0..=100).rev() {
                for j in 0..i {
                    if crop_cnt[j][i] > 0 {
                        cum_left = cum_left.min(j);
                        break;
                    }
                }
                left_bound_table[i] = cum_left;
            }
            for cur in 0..candidates.len() {
                let (x, y) = candidates[cur];

                let mut left1 = 0;
                let mut left2 = 0;
                for i in 0..=t {
                    // (left1, i]
                    // [left2, i)
                    if (forbid_left[x][y] >> i) & 1 > 0 {
                        left1 = i;
                    }
                    if i > 0 && (forbid_right[x][y] >> (i - 1)) & 1 > 0 {
                        left2 = i;
                    }
                    let mut down = 1;
                    down = down.max(left1);
                    down = down.max(left2);

                    for j in (0..i).rev() {
                        // split [0, j - 1], [j, i]
                        let s = j;
                        let d = i;

                        if cur < (candidates.len() - 1) && j < left_bound_table[i] {
                            break;
                        }

                        if j >= down && crop_cnt[s][d] > 0 {
                            for k in 0..crop_cnt[s][d] {
                                dp1[cur + 1 + k][j][i] = dp1[cur + 1 + k][j][i]
                                    .max((k + 1) * (i - j + 1) + dp2[cur][j][i]);
                            }
                        } else {
                            dp1[cur + 1][j][i] = 0;
                        }
                        for k in j..i {
                            // [j, k], [k + 1, i]
                            /* dp2[cur + 1][j][i] =
                                dp2[cur + 1][j][i].max(dp2[cur + 1][j][k] + dp2[cur + 1][k + 1][i]);*/
                            /* dp2[cur + 1][j][i] =
                                dp2[cur + 1][j][i].max(dp1[cur + 1][j][k] + dp2[cur + 1][k + 1][i]);*/
                            dp2[cur + 1][j][i] =
                                dp2[cur + 1][j][i].max(dp2[cur + 1][j][k] + dp1[cur + 1][k + 1][i]);
                            dp2[cur + 1][j][i] =
                                dp2[cur + 1][j][i].max(dp1[cur + 1][j][k] + dp1[cur + 1][k + 1][i]);
                        }
                        dp2[cur + 1][j][i] = dp2[cur + 1][j][i].max(dp2[cur + 1][j + 1][i]);
                        dp2[cur + 1][j][i] = dp2[cur + 1][j][i].max(dp2[cur + 1][j][i - 1]);
                        dp2[cur + 1][j][i] = dp2[cur + 1][j][i].max(dp1[cur + 1][j][i - 1]);
                    }
                }
            }
            fn reconstruct_from_table(
                cur: usize,
                (left, right): (usize, usize),
                v: usize,
                dp1: &[[[usize; 102]; 102]; 100],
                dp2: &[[[usize; 102]; 102]; 100],
                added_crop: &mut Vec<(usize, (usize, usize))>,
            ) {
                if v == 0 {
                    return;
                }
                if dp1[cur][left][right] == 0 && dp2[cur][left][right] == 0 {
                    return;
                }
                if v != dp2[cur][left][right] {
                    added_crop.push((cur, (left, right)));
                    reconstruct_from_table(
                        cur - 1,
                        (left, right),
                        v - (right - left + 1),
                        dp1,
                        dp2,
                        added_crop,
                    );
                    return;
                }
                let (mut left, mut right) = (left, right);
                while dp2[cur][left][right] == dp2[cur][left + 1][right] {
                    left += 1;
                }
                while dp2[cur][left][right] == dp2[cur][left][right - 1] {
                    right -= 1
                }
                for k in (left..right).rev() {
                    // [left, k], [k + 1, right]
                    if dp2[cur][left][right] == dp2[cur][left][k] + dp1[cur][k + 1][right] {
                        reconstruct_from_table(
                            cur,
                            (left, k),
                            dp2[cur][left][k],
                            dp1,
                            dp2,
                            added_crop,
                        );
                        if dp1[cur][k + 1][right] > 0 {
                            added_crop.push((cur, (k + 1, right)));
                            reconstruct_from_table(
                                cur - 1,
                                (k + 1, right),
                                dp1[cur][k + 1][right] - (right - k),
                                dp1,
                                dp2,
                                added_crop,
                            );
                        }
                        return;
                    }
                    if dp2[cur][left][right] == dp1[cur][left][k] + dp1[cur][k + 1][right] {
                        if dp1[cur][left][k] > 0 {
                            added_crop.push((cur, (left, k)));
                            reconstruct_from_table(
                                cur - 1,
                                (left, k),
                                dp1[cur][left][k] - (k - left + 1),
                                dp1,
                                dp2,
                                added_crop,
                            );
                        }
                        if dp1[cur][k + 1][right] > 0 {
                            added_crop.push((cur, (k + 1, right)));
                            reconstruct_from_table(
                                cur - 1,
                                (k + 1, right),
                                dp1[cur][k + 1][right] - (right - k),
                                dp1,
                                dp2,
                                added_crop,
                            );
                        }
                        return;
                    }
                }
                panic!();
            }
            let mut added_crop: Vec<(usize, (usize, usize))> = vec![];
            let sz = candidates.len();
            reconstruct_from_table(
                sz,
                (1, t),
                dp2[sz][1][t].max(dp1[sz][1][t]),
                &dp1,
                &dp2,
                &mut added_crop,
            );
            for &(cur, (s, d)) in added_crop.iter() {
                let (x, y) = candidates[cur - 1];
                crop_memo[x][y].push((s, d));
                crop_cnt[s][d] -= 1;
                forbid_left[x][y] |= 1u128 << s;
                forbid_right[x][y] |= 1u128 << d;
            }

            // recalc bit masks
            for &(x, y) in candidates.iter().rev() {
                for &(cx, cy) in chs[x][y].iter() {
                    forbid_left[x][y] |= forbid_left[cx][cy];
                    forbid_right[x][y] |= forbid_right[cx][cy];
                }
            }

            {
                // kill node
                let (mut cx, mut cy) = (rx, ry);
                loop {
                    living_child[cx][cy] -= 1;
                    if (cx, cy) == root {
                        break;
                    }
                    let (px, py) = pars[cx][cy];
                    (cx, cy) = (px, py);
                }
            }
            let n_score = score + dp1[sz][1][t].max(dp2[sz][1][t]) as i64;
            // eprintln!("{num} new sc {} \t({} secs)", n_score * 25, start_time.elapsed().as_millis());
            score = n_score;
        }
    }

    let mut max_score = score;
    let max_time = 1900;
    let mut true_maximum = score;
    let mut true_crop_memo = crop_memo.to_vec();
    loop {
        if is_first {
            break;
        }
        hill_cnt += 1;
        let cur_time = start_time.elapsed().as_millis();
        if cur_time >= max_time {
            break;
        }
        let ur = Uniform::from(0..20);
        let num_nodes = 3;
        let mut candidates: Vec<(usize, usize)> = vec![];

        let mut buff: Vec<(usize, usize)> = vec![];
        for _ in 0..num_nodes {
            let mut rx = ur.sample(rr);
            let mut ry = ur.sample(rr);
            loop {
                let mut ok = true;
                for &(cx, cy) in chs[rx][ry].iter() {
                    if selected_cnt[cx][cy] <= selected_cnt[rx][ry] {
                        (rx, ry) = (cx, cy);
                        ok = false;
                        break;
                    }
                }
                if ok {
                    break;
                }
            }
            while last_used_time[rx][ry] == hill_cnt {
                rx = ur.sample(rr);
                ry = ur.sample(rr);
                loop {
                    let mut ok = true;
                    for &(cx, cy) in chs[rx][ry].iter() {
                        if selected_cnt[cx][cy] <= selected_cnt[rx][ry] {
                            (rx, ry) = (cx, cy);
                            ok = false;
                            break;
                        }
                    }
                    if ok {
                        break;
                    }
                }
            }
            let (mut cx, mut cy) = (rx, ry);
            selected_cnt[rx][ry] += 1;
            while last_used_time[cx][cy] != hill_cnt {
                last_used_time[cx][cy] = hill_cnt;
                buff.push((cx, cy));
                if (cx, cy) == (i0, 0) {
                    break;
                }
                (cx, cy) = pars[cx][cy];
            }
            while let Some(&p) = buff.last() {
                candidates.push(p);
                buff.pop();
            }
        }
        candidates.reverse();

        let mut poped_crop: Vec<(usize, usize, usize, usize)> = vec![];
        // clear
        for &(x, y) in candidates.iter() {
            for &(s, d) in crop_memo[x][y].iter() {
                score -= (d - s + 1) as i64;
                poped_crop.push((x, y, s, d));
                crop_cnt[s][d] += 1;
            }
            crop_memo[x][y].clear();

            forbid_left[x][y] = 0;
            forbid_right[x][y] = 0;
        }
        // recalc bit masks
        for &(x, y) in candidates.iter() {
            for &(nx, ny) in chs[x][y].iter() {
                forbid_left[x][y] |= forbid_left[nx][ny];
                forbid_right[x][y] |= forbid_right[nx][ny];
            }
        }
        // reconstruct
        {
            for &(x, y) in candidates.iter() {
                let mut dp: [usize; 102] = [0; 102];
                let mut dp2: [usize; 102] = [0; 102];
                let mut downs: [usize; 102] = [0; 102];

                let mut left1 = 0;
                let mut left2 = 0;
                for i in 0..=t {
                    // (left1, i]
                    // [left2, i)
                    if (forbid_left[x][y] >> i) & 1 > 0 {
                        left1 = i;
                    }
                    if i > 0 && (forbid_right[x][y] >> (i - 1)) & 1 > 0 {
                        left2 = i;
                    }
                    let mut down = 1;
                    down = down.max(left1);
                    down = down.max(left2);
                    downs[i] = down;

                    for j in down..i {
                        // split [0, j - 1], [j, i]
                        let s = j;
                        let d = i;

                        if crop_cnt[s][d] == 0 {
                            continue;
                        }
                        dp[i] = dp[i].max(dp2[j - 1] + (i - j + 1) * 100 - 199);
                    }
                    dp2[i] = dp[i];
                    if i > 0 {
                        dp2[i] = dp2[i].max(dp2[i - 1]);
                    }
                }
                // reconstruct from dp table
                let mut right = t;
                let mut crop_here: Vec<(usize, usize)> = vec![];
                loop {
                    if dp2[right] == 0 {
                        break;
                    }
                    while right > 0 && dp2[right] == dp2[right - 1] {
                        right -= 1;
                    }
                    let down = downs[right];
                    for left in down..right {
                        // [0, left), [left, right]
                        let s = left;
                        let d = right;

                        if crop_cnt[s][d] == 0 {
                            continue;
                        }
                        if dp[right] == (dp2[left - 1] + (right - left + 1) * 100 - 199) {
                            score += (d - s + 1) as i64;
                            crop_here.push((s, d));
                            crop_memo[x][y].push((s, d));
                            right = left - 1;
                            break;
                        }
                    }
                }
                // eprintln!("fill {} {} {:?}", x, y, crop_here);
                for &(s, d) in crop_here.iter() {
                    crop_cnt[s][d] -= 1;
                    forbid_left[x][y] |= 1u128 << s;
                    forbid_right[x][y] |= 1u128 << d;
                }
                let (px, py) = pars[x][y];
                forbid_left[px][py] |= forbid_left[x][y];
                forbid_right[px][py] |= forbid_right[x][y];
            }
        }
        let diff = score - max_score;
        let max_temp = 6;
        let cur_temp = (max_time - cur_time) as f64 / max_time as f64 * max_temp as f64;
        let prob = (diff as f64 / cur_temp).exp();
        if score > max_score {
            max_score = score;

            /* eprintln!(
                "hill climbing score :  {}\t({} msecs)",
                1000000 * score / (400 * 100),
                start_time.elapsed().as_millis()
            );*/
            if score > true_maximum {
                true_maximum = score;
                true_crop_memo = crop_memo.to_vec();
            }
        } else {
            // revert
            score = max_score;
            // clear
            for &(x, y) in candidates.iter() {
                for &(s, d) in crop_memo[x][y].iter() {
                    crop_cnt[s][d] += 1;
                }
                crop_memo[x][y].clear();

                forbid_left[x][y] = 0;
                forbid_right[x][y] = 0;
            }
            // add crops
            {
                for &(x, y, s, d) in poped_crop.iter() {
                    crop_memo[x][y].push((s, d));
                    crop_cnt[s][d] -= 1;

                    forbid_left[x][y] |= 1u128 << s;
                    forbid_right[x][y] |= 1u128 << d;
                }
            }
            // recalc bit masks
            for &(x, y) in candidates.iter() {
                for &(nx, ny) in chs[x][y].iter() {
                    forbid_left[x][y] |= forbid_left[nx][ny];
                    forbid_right[x][y] |= forbid_right[nx][ny];
                }
            }
        }
    }
    score = true_maximum;
    crop_memo = true_crop_memo;
    eprintln!("hill iterations: {}", hill_cnt);

    // construct res
    let mut large_crop_table: Vec<Vec<Vec<usize>>> = vec![vec![vec![]; t + 1]; t + 1];
    for (i, &(s, d)) in crops.iter().enumerate() {
        large_crop_table[s][d].push(i);
    }
    let mut res: Vec<(usize, usize, usize, usize)> = vec![];
    score = 0;
    for x in 0..n {
        for y in 0..n {
            for &(s, d) in crop_memo[x][y].iter() {
                let index = *large_crop_table[s][d].last().unwrap();
                large_crop_table[s][d].pop();
                res.push((index, x, y, s));
                score += (d - s + 1) as i64;
            }
        }
    }

    score *= 1000000;
    score /= (n * n * 100) as i64;
    (res, score)
}
fn check_feasiability(
    root: (usize, usize),
    down_walls: &Vec<Vec<bool>>,
    right_walls: &Vec<Vec<bool>>,
    crops: &[(usize, usize)],
    res: &Vec<(usize, usize, usize, usize)>,
) -> bool {
    let n = 20;
    let mut switch_on: Vec<Vec<(usize, usize)>> = vec![vec![]; 101];
    let mut switch_off: Vec<Vec<(usize, usize)>> = vec![vec![]; 101];
    for &(i, x, y, s) in res.iter() {
        let d = crops[i].1;
        switch_on[s].push((x, y));
        switch_off[d].push((x, y));
        /*
        if (x, y) == root {
            return false;
        }*/
    }
    let directions = [(-1, 0), (1, 0), (0, -1), (0, 1)];

    let is_movable = |pos: (usize, usize), dir: (i32, i32)| -> bool {
        let (x, y) = pos;
        let (dx, dy) = dir;
        let nx = x as i32 + dx;
        let ny = y as i32 + dy;
        if nx < 0 || nx >= n as i32 || ny < 0 || ny >= n as i32 {
            return false;
        }
        let nx = nx as usize;
        let ny = ny as usize;
        if dx == 1 {
            if down_walls[x][y] {
                return false;
            }
            return true;
        }
        if dy == 1 {
            if right_walls[x][y] {
                return false;
            }
            return true;
        }
        if dx == -1 {
            if down_walls[nx][ny] {
                return false;
            }
            return true;
        }
        if dy == -1 {
            if right_walls[nx][ny] {
                return false;
            }
            return true;
        }
        panic!();
    };
    let mut currently_used: Vec<Vec<bool>> = vec![vec![false; n]; n];
    for i in 0..=100 {
        // switch on
        let mut reacheable: Vec<Vec<bool>> = vec![vec![false; n]; n];
        let mut st: Vec<(usize, usize)> = vec![];
        reacheable[root.0][root.1] = true;
        st.push(root);
        while let Some(&(x, y)) = st.last() {
            st.pop();
            for &d in directions.iter() {
                let nx = x as i32 + d.0;
                let ny = y as i32 + d.1;
                if nx < 0 || ny < 0 {
                    continue;
                }
                let nx = nx as usize;
                let ny = ny as usize;
                if nx >= n || ny >= n {
                    continue;
                }
                if reacheable[nx][ny] {
                    continue;
                }
                if currently_used[nx][ny] {
                    continue;
                }
                if !is_movable((x, y), d) {
                    continue;
                }
                reacheable[nx][ny] = true;
                st.push((nx, ny));
            }
        }
        for &(x, y) in switch_on[i].iter() {
            if !reacheable[x][y] {
                return false;
            }
            if currently_used[x][y] {
                return false;
            }
            currently_used[x][y] = true;
        }
        for &(x, y) in switch_off[i].iter() {
            currently_used[x][y] = false;
        }
    }
    for i in (0..=100).rev() {
        // switch off
        let mut reacheable: Vec<Vec<bool>> = vec![vec![false; n]; n];
        let mut st: Vec<(usize, usize)> = vec![];
        reacheable[root.0][root.1] = true;
        st.push(root);
        while let Some(&(x, y)) = st.last() {
            st.pop();
            for &d in directions.iter() {
                let nx = x as i32 + d.0;
                let ny = y as i32 + d.1;
                if nx < 0 || ny < 0 {
                    continue;
                }
                let nx = nx as usize;
                let ny = ny as usize;
                if nx >= n || ny >= n {
                    continue;
                }
                if reacheable[nx][ny] {
                    continue;
                }
                if currently_used[nx][ny] {
                    continue;
                }
                if !is_movable((x, y), d) {
                    continue;
                }
                reacheable[nx][ny] = true;
                st.push((nx, ny));
            }
        }
        for &(x, y) in switch_off[i].iter() {
            if !reacheable[x][y] {
                return false;
            }
            if currently_used[x][y] {
                return false;
            }
            currently_used[x][y] = true;
        }
        for &(x, y) in switch_on[i].iter() {
            currently_used[x][y] = false;
        }
    }
    true
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
fn read_string() -> String {
    let mut x = String::new();
    std::io::stdin()
        .read_line(&mut x)
        .expect("failed to read a line");
    x = x.trim().to_string();
    x
}
