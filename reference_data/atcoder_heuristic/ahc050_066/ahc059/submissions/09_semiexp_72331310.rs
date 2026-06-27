use proconio::input;
use rand::prelude::*;

struct Task {
    n: usize,
    a: Vec<Vec<usize>>,
}

fn load_task() -> Task {
    input! {
        n: usize,
        a: [[usize; n]; n],
    }

    Task { n, a }
}

type Seq = Vec<(usize, usize)>;

fn initial_seq(task: &Task, side: usize) -> Seq {
    let mut pos = vec![vec![]; task.n * task.n / 2];
    if side == 0 {
        for y in 0..task.n {
            for x in 0..task.n {
                let v = task.a[y][x];
                pos[v].push((y, x));
            }
        }
    } else {
        for x in 0..task.n {
            for y in 0..task.n {
                let v = task.a[y][x];
                pos[v].push((y, x));
            }
        }
    }

    let mut ords = vec![];
    for i in 0..task.n * task.n / 2 {
        ords.push((pos[i][0], i));
    }
    ords.sort();
    let mut idx = vec![];
    for &(_, i) in &ords {
        idx.push(i);
    }

    let mut ret = vec![];
    for &v in &idx {
        ret.push(pos[v][0]);
        ret.push(pos[v][1]);
    }

    ret
}

fn show_seq(_task: &Task, seq: &Seq) {
    let mut y = 0;
    let mut x = 0;

    for i in 0..seq.len() {
        let y2 = seq[i].0;
        let x2 = seq[i].1;

        if y < y2 {
            for _ in 0..(y2 - y) {
                println!("D");
            }
        } else if y > y2 {
            for _ in 0..(y - y2) {
                println!("U");
            }
        }
        y = y2;

        if x < x2 {
            for _ in 0..(x2 - x) {
                println!("R");
            }
        } else if x > x2 {
            for _ in 0..(x - x2) {
                println!("L");
            }
        }
        x = x2;

        println!("Z");
    }
}

fn compute_score(seq: &Seq) -> usize {
    let mut ret = seq[0].0 + seq[0].1;
    for i in 1..seq.len() {
        ret += seq[i - 1].0.abs_diff(seq[i].0);
        ret += seq[i - 1].1.abs_diff(seq[i].1);
    }
    ret
}

fn dist(p1: (usize, usize), p2: (usize, usize)) -> usize {
    p1.0.abs_diff(p2.0) + p1.1.abs_diff(p2.1)
}

fn anneal<R: Rng>(task: &Task, seq: &mut Seq, rng: &mut R) {
    let start = std::time::Instant::now();

    let ki = task.n * task.n / 2;
    let mut locs = vec![(!0, !0); ki];
    let mut cur_score = compute_score(seq);

    let mut temperature = 2.0;
    let should_update = |cur_score: usize, new_score: usize, temp: f64, rng: &mut R| {
        if new_score <= cur_score {
            true
        } else {
            let prob = ((cur_score as f64 - new_score as f64) / temp).exp();
            rng.random_range(0.0..1.0) < prob
        }
    };

    let mut indices = vec![];
    let mut n_steps = 0;
    let mut updated = true;

    loop {
        n_steps += 1;
        let elapsed = start.elapsed().as_secs_f64();
        if elapsed > 0.49f64 {
            break;
        }

        // fill locs
        if updated {
            for i in 0..ki {
                locs[i] = (!0, !0);
            }
            for i in 0..seq.len() {
                let v = task.a[seq[i].0][seq[i].1];
                if locs[v].0 == !0 {
                    locs[v].0 = i;
                } else {
                    locs[v].1 = i;
                }
            }
            updated = false;
        }

        let mode = rng.random_range(0..4);

        if mode == 0 {
            // swap
            let u = rng.random_range(0..ki);
            let v = rng.random_range(0..ki);
            if u == v {
                continue;
            }

            seq.swap(locs[u].0, locs[v].0);
            seq.swap(locs[u].1, locs[v].1);

            // TODO: compute new_score quickly
            // == start ==
            let i1 = locs[u].0;
            let i2 = locs[v].0;
            let i3 = locs[u].1;
            let i4 = locs[v].1;

            indices.clear();
            indices.push(i1);
            indices.push(i2);
            indices.push(i3);
            indices.push(i4);
            indices.sort_unstable();
            indices.dedup();

            // swap を元に戻してold_localを計算
            seq.swap(locs[u].0, locs[v].0);
            seq.swap(locs[u].1, locs[v].1);

            let mut old_local = 0;
            for &i in &indices {
                if i == 0 {
                    old_local += seq[0].0 + seq[0].1;
                } else {
                    old_local += dist(seq[i - 1], seq[i]);
                }
                if i + 1 < seq.len() {
                    old_local += dist(seq[i], seq[i + 1]);
                }
            }
            for w in 0..indices.len() - 1 {
                if indices[w] + 1 == indices[w + 1] {
                    old_local -= dist(seq[indices[w]], seq[indices[w + 1]]);
                }
            }

            // swap を再度実行
            seq.swap(locs[u].0, locs[v].0);
            seq.swap(locs[u].1, locs[v].1);

            let mut new_local = 0;
            for &i in &indices {
                if i == 0 {
                    new_local += seq[0].0 + seq[0].1;
                } else {
                    new_local += dist(seq[i - 1], seq[i]);
                }
                if i + 1 < seq.len() {
                    new_local += dist(seq[i], seq[i + 1]);
                }
            }
            for w in 0..indices.len() - 1 {
                if indices[w] + 1 == indices[w + 1] {
                    new_local -= dist(seq[indices[w]], seq[indices[w + 1]]);
                }
            }

            let new_score_quick = cur_score - old_local + new_local;
            // == end ==

            if should_update(cur_score, new_score_quick, temperature, rng) {
                cur_score = new_score_quick;
                updated = true;
            } else {
                // revert
                seq.swap(locs[u].0, locs[v].0);
                seq.swap(locs[u].1, locs[v].1);
            }
        } else if mode == 1 {
            continue;
            /*
            let u = rng.random_range(0..ki);
            seq.swap(locs[u].0, locs[u].1);

            // TODO: compute new_score quickly
            // == start ==
            let i1 = locs[u].0;
            let i2 = locs[u].1;

            indices.clear();
            indices.push(i1);
            indices.push(i2);
            indices.sort_unstable();

            // swap を元に戻してold_localを計算
            seq.swap(locs[u].0, locs[u].1);

            let mut old_local = 0;
            for &i in &indices {
                if i == 0 {
                    old_local += seq[0].0 + seq[0].1;
                } else {
                    old_local += dist(seq[i - 1], seq[i]);
                }
                if i + 1 < seq.len() {
                    old_local += dist(seq[i], seq[i + 1]);
                }
            }
            if indices.len() == 2 && indices[0] + 1 == indices[1] {
                old_local -= dist(seq[indices[0]], seq[indices[1]]);
            }

            // swap を再度実行
            seq.swap(locs[u].0, locs[u].1);

            let mut new_local = 0;
            for &i in &indices {
                if i == 0 {
                    new_local += seq[0].0 + seq[0].1;
                } else {
                    new_local += dist(seq[i - 1], seq[i]);
                }
                if i + 1 < seq.len() {
                    new_local += dist(seq[i], seq[i + 1]);
                }
            }
            if indices.len() == 2 && indices[0] + 1 == indices[1] {
                new_local -= dist(seq[indices[0]], seq[indices[1]]);
            }

            let new_score_quick = cur_score - old_local + new_local;
            // == end ==

            if should_update(cur_score, new_score_quick, temperature, rng) {
                cur_score = new_score_quick;
                updated = true;
            } else {
                // revert
                seq.swap(locs[u].0, locs[u].1);
            }
            */
        } else if mode == 2 {
            let u = rng.random_range(0..ki);
            let v = rng.random_range(0..ki);
            if u == v {
                continue;
            }

            if locs[u].0 < locs[v].0 && locs[v].1 < locs[u].1 {
                continue;
            }

            let bef = rng.random_range(0..2) == 0;
            let tgt = if rng.random_range(0..2) == 0 {
                locs[v].0
            } else {
                locs[v].1
            };

            // TODO: compute new_score quickly
            // == start ==
            let move_start = locs[u].0;
            let move_end = locs[u].1;
            let insert_pos_pre = if bef { tgt as isize - 1 } else { tgt as isize };
            let insert_pos_post = insert_pos_pre + 1;

            if bef && move_end as isize == insert_pos_pre {
                continue;
            }
            if !bef && move_start as isize == insert_pos_post {
                continue;
            }

            let mut cur_edge_cost = 0;
            if insert_pos_pre >= 0 && insert_pos_pre + 1 < seq.len() as isize {
                cur_edge_cost += dist(
                    seq[insert_pos_pre as usize],
                    seq[insert_pos_post as usize],
                );
            } else if insert_pos_post < seq.len() as isize {
                cur_edge_cost += seq[insert_pos_post as usize].0 + seq[insert_pos_post as usize].1;
            }
            if move_start > 0 {
                cur_edge_cost += dist(seq[move_start - 1], seq[move_start]);
            } else {
                cur_edge_cost += seq[0].0 + seq[0].1;
            }
            if move_end + 1 < seq.len() {
                cur_edge_cost += dist(seq[move_end], seq[move_end + 1]);
            }

            let mut new_edge_cost = 0;
            if insert_pos_pre >= 0 {
                new_edge_cost += dist(seq[insert_pos_pre as usize], seq[move_start]);
            } else {
                new_edge_cost += seq[move_start].0 + seq[move_start].1;
            }
            if insert_pos_post < seq.len() as isize {
                new_edge_cost += dist(seq[move_end], seq[insert_pos_post as usize]);
            }
            if move_start > 0 && move_end + 1 < seq.len() {
                new_edge_cost += dist(seq[move_start - 1], seq[move_end + 1]);
            } else if move_end + 1 < seq.len() {
                new_edge_cost += seq[move_end + 1].0 + seq[move_end + 1].1;
            }

            let new_score_quick = cur_score - cur_edge_cost + new_edge_cost;
            // == end ==

            // let new_score = compute_score(&new_seq);
            // eprintln!("{} {} {}, {} {} {} {}", cur_score, new_score, new_score_quick, move_start, move_end, bef, tgt);
            // assert_eq!(new_score, new_score_quick);

            if should_update(cur_score, new_score_quick, temperature, rng) {
                updated = true;
                let mut new_seq = vec![];
                for i in 0..seq.len() {
                    if locs[u].0 <= i && i <= locs[u].1 {
                        continue;
                    }
                    if bef && i == tgt {
                        for j in locs[u].0..=locs[u].1 {
                            new_seq.push(seq[j]);
                        }
                    }
                    new_seq.push(seq[i]);
                    if !bef && i == tgt {
                        for j in locs[u].0..=locs[u].1 {
                            new_seq.push(seq[j]);
                        }
                    }
                }
                assert_eq!(new_seq.len(), seq.len());

                *seq = new_seq;
                cur_score = new_score_quick;
            }
        } else if mode == 3 {
            let u = rng.random_range(0..ki);
            let v = rng.random_range(0..ki);
            if u == v {
                continue;
            }

            let (ua, ub) = locs[u];
            let (va, vb) = locs[v];

            if ub + 1 == va || vb + 1 == ua {
                // super mendoi
                continue;
            }
            if !(ub < va || vb < ua) {
                continue;
            }

            let mut old_edge_cost = 0;
            if ua > 0 {
                old_edge_cost += dist(seq[ua - 1], seq[ua]);
            } else {
                old_edge_cost += seq[0].0 + seq[0].1;
            }
            if ub + 1 < seq.len() {
                old_edge_cost += dist(seq[ub], seq[ub + 1]);
            }
            if va > 0 {
                old_edge_cost += dist(seq[va - 1], seq[va]);
            } else {
                old_edge_cost += seq[0].0 + seq[0].1;
            }
            if vb + 1 < seq.len() {
                old_edge_cost += dist(seq[vb], seq[vb + 1]);
            }

            let mut new_edge_cost = 0;
            if ua > 0 {
                new_edge_cost += dist(seq[ua - 1], seq[va]);
            } else {
                new_edge_cost += seq[va].0 + seq[va].1;
            }
            if vb + 1 < seq.len() {
                new_edge_cost += dist(seq[ub], seq[vb + 1]);
            }
            if va > 0 {
                new_edge_cost += dist(seq[va - 1], seq[ua]);
            } else {
                new_edge_cost += seq[ua].0 + seq[ua].1;
            }
            if ub + 1 < seq.len() {
                new_edge_cost += dist(seq[vb], seq[ub + 1]);
            }

            let new_score_quick = cur_score - old_edge_cost + new_edge_cost;
            if should_update(cur_score, new_score_quick, temperature, rng) {
                updated = true;
                let mut new_seq = vec![];
                for i in 0..seq.len() {
                    if i == ua {
                        for j in va..=vb {
                            new_seq.push(seq[j]);
                        }
                    }
                    if i == va {
                        for j in ua..=ub {
                            new_seq.push(seq[j]);
                        }
                    }
                    if (ua..=ub).contains(&i) || (va..=vb).contains(&i) {
                        continue;
                    }
                    new_seq.push(seq[i]);
                }
                assert_eq!(new_seq.len(), seq.len());

                *seq = new_seq;
                cur_score = new_score_quick;
            }
        }

        temperature *= 0.99999935;
    }

    eprintln!("final temp: {}", temperature);
    eprintln!("! n_steps {}", n_steps);
}

fn to_range(a: usize, b: usize) -> std::ops::RangeInclusive<usize> {
    if a <= b {
        a..=b
    } else {
        b..=a
    }
}

fn show_move(y: usize, x: usize, y2: usize, x2: usize, ans: &mut Vec<char>) {
    if y < y2 {
        for _ in 0..(y2 - y) {
            ans.push('D');
        }
    } else if y > y2 {
        for _ in 0..(y - y2) {
            ans.push('U');
        }
    }

    if x < x2 {
        for _ in 0..(x2 - x) {
            ans.push('R');
        }
    } else if x > x2 {
        for _ in 0..(x - x2) {
            ans.push('L');
        }
    }
}

fn pickup(ans: &mut Vec<char>) {
    ans.push('Z');
}

fn put(ans: &mut Vec<char>) {
    ans.push('X');
}

fn postproc(task: &Task, seq: &Seq) -> (usize, Vec<char>) {
    let mut ans = vec![];

    let ki = task.n * task.n / 2;
    let mut n_used = vec![0; ki];

    let mut cell_val = vec![vec![0; task.n]; task.n];
    let mut pos = vec![(0, 0); ki * 2];
    let mut idx = vec![0; ki * 2];
    let mut act_seq = vec![];
    for i in 0..seq.len() {
        let v = task.a[seq[i].0][seq[i].1];
        act_seq.push(v * 2 + n_used[v]);
        cell_val[seq[i].0][seq[i].1] = v * 2 + n_used[v];
        idx[v * 2 + n_used[v]] = i;
        pos[v * 2 + n_used[v]] = (seq[i].0, seq[i].1);
        n_used[v] += 1;
    }

    let mut y = 0;
    let mut x = 0;

    let mut ideal_cost = 0;
    for i in 0..act_seq.len() {
        let (y2, x2) = pos[act_seq[i]];

        let mut rep = 0;
        loop {
            let mut best_profit = (0, 0, 0, 0, 0);

            for y3 in to_range(y, y2) {
                for x3 in to_range(x, x2) {
                    if (y3, x3) == (y, x) || (y3, x3) == (y2, x2) {
                        continue;
                    }
                    if cell_val[y3][x3] == !0 {
                        continue;
                    }

                    let id = idx[cell_val[y3][x3]];
                    assert!(id > i);
                    for y4 in to_range(y3, y2) {
                        for x4 in to_range(x3, x2) {
                            if (y4, x4) == (y3, x3) || (y4, x4) == (y2, x2) {
                                continue;
                            }
                            if cell_val[y4][x4] != !0 {
                                continue;
                            }

                            let mut cost_before = 0;
                            let mut cost_after = 0;
                            cost_before += dist((y3, x3), pos[act_seq[id - 1]]);
                            cost_after += dist((y4, x4), pos[act_seq[id - 1]]);
                            if id + 1 < act_seq.len() {
                                cost_before += dist((y3, x3), pos[act_seq[id + 1]]);
                                cost_after += dist((y4, x4), pos[act_seq[id + 1]]);
                            }

                            if cost_before > cost_after && cost_before - cost_after > best_profit.0 {
                                best_profit = (
                                    cost_before - cost_after,
                                    y3,
                                    x3,
                                    y4,
                                    x4,
                                );
                            }
                        }
                    }
                }
            }

            {
                let (profit, y3, x3, y4, x4) = best_profit;
                if profit > 0 {
                    show_move(y, x, y3, x3, &mut ans);
                    pickup(&mut ans);
                    show_move(y3, x3, y4, x4, &mut ans);
                    put(&mut ans);
                    let id = idx[cell_val[y3][x3]];
                    assert!(id > i);

                    // perform move
                    cell_val[y4][x4] = cell_val[y3][x3];
                    cell_val[y3][x3] = !0;
                    pos[cell_val[y4][x4]] = (y4, x4);

                    ideal_cost += y.abs_diff(y4) + x.abs_diff(x4);
                    y = y4;
                    x = x4;
                    rep += 1;
                    continue;
                } else {
                    show_move(y, x, y2, x2, &mut ans);
                }
                pickup(&mut ans);
            }
            ideal_cost += y.abs_diff(y2) + x.abs_diff(x2);
            y = y2;
            x = x2;
            cell_val[y][x] = !0;
            break;
        }
        if rep > 0 {
            eprintln!("rep: {}", rep);
        }
    }

    (ideal_cost, ans)
}

fn main() {
    let task = load_task();

    let mut best_cost = usize::MAX;
    let mut best_ans = vec![];

    let offset = task.n * task.n + 2 * task.n * task.n * task.n;
    for side in [0, 1, 0, 1] {
        let mut seq = initial_seq(&task, side);
        let initial_score = compute_score(&seq);
        eprintln!("initial score: {}", offset - initial_score);
        let mut rng = rand::rng();
        anneal(&task, &mut seq, &mut rng);
        let final_score = compute_score(&seq);
        eprintln!("! intermediate_score {}", offset - final_score);
        // show_seq(&task, &seq);
        let (ideal_cost, ans) = postproc(&task, &seq);

        eprintln!("! ideal_cost {}", ideal_cost);
        if ideal_cost < best_cost {
            best_cost = ideal_cost;
            best_ans = ans;
        }
    }

    eprintln!("Score = {}", offset - best_cost);
    for &c in &best_ans {
        println!("{}", c);
    }
}
