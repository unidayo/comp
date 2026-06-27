use proconio::*;
use rand::Rng;
use std::time::Instant;
use num::pow;

//const MOD: usize = 1_000_000_007;
//const PI: f64 = std::f64::consts::PI;
const INF: f64 = 1e18;

struct Input {
    N: usize,
    w: Vec<Vec<usize>>,
    d: Vec<Vec<isize>>,
}

#[derive(Copy, Clone)]
struct Point {
    y: usize,
    x: usize,
}

const START: Point = Point { y: 0, x: 0 };

fn dist(p0: &Point, p1: &Point) -> usize {
    let y0 = p0.y;
    let x0 = p0.x;
    let y1 = p1.y;
    let x1 = p1.x;
    let dy = (y0 as isize - y1 as isize).abs() as usize;
    let dx = (x0 as isize - x1 as isize).abs() as usize;
    dy + dx
}

fn can_carry(route: &Vec<Point>, w: &Vec<Vec<usize>>, d: &Vec<Vec<isize>>) -> bool {
    let l_last = dist(&route[route.len() - 1], &START);
    for i in 0..route.len()-1 {
        let mut w_sum = 0;
        let mut d_remain = d[route[i].y][route[i].x];
        for j in i+1..route.len() {
            let p_from = &route[j-1];
            let p_to = &route[j];
            let l = dist(p_from, p_to);
            d_remain -= l as isize * w_sum as isize;
            if d_remain <= 0 {
                return false; // 運べない
            }
            w_sum += w[p_to.y][p_to.x];
        }
        // 最後の地点からスタート地点までの距離も考慮
        d_remain -= l_last as isize * w_sum as isize;
        if d_remain <= 0 {
            return false; // 運べない
        }
    }
    true
}

fn route_len(route: &Vec<Point>) -> usize {
    let mut len = 0;
    for i in 0..route.len() - 1 {
        len += dist(&route[i], &route[i + 1]);
    }
    len += dist(&START, &route[0]);
    len += dist(&route[route.len() - 1], &START);
    len
}

fn make_command(p_from: &Point, p_to: &Point) -> Vec<String> {
    let y_from = p_from.y;
    let x_from = p_from.x;
    let y_to = p_to.y;
    let x_to = p_to.x;
    let mut commands = Vec::with_capacity(
        (y_to as isize - y_from as isize).abs() as usize +
            (x_to as isize - x_from as isize).abs() as usize
    );

    if y_from < y_to {
        commands.extend(std::iter::repeat("D".to_string()).take(y_to - y_from));
    } else if y_from > y_to {
        commands.extend(std::iter::repeat("U".to_string()).take(y_from - y_to));
    }

    if x_from < x_to {
        commands.extend(std::iter::repeat("R".to_string()).take(x_to - x_from));
    } else if x_from > x_to {
        commands.extend(std::iter::repeat("L".to_string()).take(x_from - x_to));
    }

    commands
}

fn make_answer(routes: &Vec<Vec<Point>>) -> Vec<String> {
    // 必要な文字列の総数を概算して事前に容量確保
    let mut total_capacity = 0;
    for r in routes.iter() {
        // 各点間の移動コマンド + 各点での採取コマンド ("1")
        total_capacity += r.len() * 2 + 2; // +2 はスタートからの移動とスタートへの移動
    }

    let mut ans = Vec::with_capacity(total_capacity);

    for r in routes.iter() {
        // ルートの先頭と最後にSTARTを追加
        let mut commands = Vec::new();

        // STARTから最初の点への移動
        commands.extend(make_command(&START, &r[0]));

        // 点間の移動と採取
        for i in 0..r.len() - 1 {
            commands.push("1".to_string()); // 採取
            commands.extend(make_command(&r[i], &r[i + 1]));
        }

        // 最後の点で採取
        if !r.is_empty() {
            commands.push("1".to_string());
        }

        // 最後の点からSTARTへ移動
        if !r.is_empty() {
            commands.extend(make_command(&r[r.len() - 1], &START));
        }

        // 全てのコマンドを結果に追加
        ans.extend(commands);
    }

    ans
}

fn solve(_in: Input) -> Vec<String> {
    let mut rng = rand::thread_rng();
    let mut routes = vec![];
    let mut route_lens = vec![];
    for y in 0.._in.N {
        for x in 0.._in.N {
            if y == 0 && x == 0 {
                continue; // スタート地点はスキップ
            }
            routes.push(vec![Point { y, x }]);
            route_lens.push(dist(&START, &Point { y, x }));
        }
    }

    // 焼きなまし法によるルート最適化
    let T_init = 10.0;
    let T_end = 0.001;
    let K = -0.0;
    let mut T = T_init;
    let mut iter = 0;
    let mut move_count = 0;
    let start_time = Instant::now();
    let time_limit = std::time::Duration::from_millis(1990);
    while start_time.elapsed() < time_limit {
        // 近傍解を生成する方法をランダムに選択
        let method = rng.gen_range(0..12);

        match method {
            0 => {
                // 方法1: 二つのルート間で点を交換
                let i = rng.gen_range(0..routes.len());
                let j = rng.gen_range(0..routes.len());

                if i == j || (routes[i].len() <= 1 && routes[j].len() <= 1) {
                    continue;
                }

                let mut route_i = routes[i].clone();
                let mut route_j = routes[j].clone();
                let len_i = route_lens[i];
                let len_j = route_lens[j];

                let pos_i = rng.gen_range(0..route_i.len());
                let pos_j = rng.gen_range(0..route_j.len());

                // 値がCopyトレイトを実装しているため、cloneは不要
                let temp = route_i[pos_i];
                route_i[pos_i] = route_j[pos_j];
                route_j[pos_j] = temp;

                if !can_carry(&route_i, &_in.w, &_in.d) || !can_carry(&route_j, &_in.w, &_in.d) {
                    continue;
                }

                let new_len_i = route_len(&route_i);
                let new_len_j = route_len(&route_j);
                let diff = (len_i + len_j) as f64 - (new_len_i + new_len_j) as f64;
                if diff > 0.0 || rng.gen::<f64>() < (diff / T).exp() {
                    // より良い解なら採用
                    routes[i] = route_i;
                    routes[j] = route_j;
                    route_lens[i] = new_len_i;
                    route_lens[j] = new_len_j;
                    move_count += 1;
                }
            },
            1 => {
                // 方法2: ルート内の点の順序を変更
                let i = rng.gen_range(0..routes.len());
                if routes[i].len() < 2 {
                    continue;
                }

                let pos1 = rng.gen_range(0..routes[i].len());
                let pos2 = rng.gen_range(0..routes[i].len());
                if pos1 == pos2 {
                    continue;
                }

                let mut route_i = routes[i].clone();
                let len_i = route_lens[i];

                // Pointはコピー可能なのでcloneは不要
                let temp = route_i[pos1];
                route_i[pos1] = route_i[pos2];
                route_i[pos2] = temp;

                // 制約チェック
                if !can_carry(&route_i, &_in.w, &_in.d) {
                    continue;
                }

                let new_len_i = route_len(&route_i);
                let diff = len_i as f64 - new_len_i as f64;
                if diff > 0.0 || rng.gen::<f64>() < (diff / T).exp() {
                    routes[i] = route_i;
                    route_lens[i] = new_len_i;
                    move_count += 1;
                }
            },
            2 | 3 => {
                // 方法3: あるルートから別のルートへ点を移動
                let i = rng.gen_range(0..routes.len());
                let j = rng.gen_range(0..routes.len());

                if i == j || routes[i].len() < 2 {
                    continue;
                }

                let mut route_i = routes[i].clone();
                let mut route_j = routes[j].clone();
                let len_i = route_lens[i];
                let len_j = route_lens[j];

                // 移動する位置を決定
                let pos_i = rng.gen_range(0..route_i.len());
                let pos_j = rng.gen_range(0..=route_j.len()); // pos_jはjの長さ以下にする

                // Pointはコピートレイトを実装しているので直接値を扱う
                let point = route_i.remove(pos_i);
                route_j.insert(pos_j, point);

                if !can_carry(&route_i, &_in.w, &_in.d) || !can_carry(&route_j, &_in.w, &_in.d) {
                    continue;
                }

                let new_len_i = route_len(&route_i);
                let new_len_j = route_len(&route_j);
                let diff = (len_i + len_j) as f64 - (new_len_i + new_len_j) as f64;
                if diff > 0.0 || rng.gen::<f64>() < (diff / T).exp() {
                    // より良い解なら採用
                    routes[i] = route_i;
                    routes[j] = route_j;
                    route_lens[i] = new_len_i;
                    route_lens[j] = new_len_j;
                    move_count += 1;
                }
            },
            4 => {
                // 方法4: 二つのルートをマージ（結合）する
                let mut i = rng.gen_range(0..routes.len());
                let mut j = rng.gen_range(0..routes.len());

                if i == j {
                    continue;
                }
                if i > j {
                    // iがjより大きい場合は入れ替える
                    std::mem::swap(&mut i, &mut j);
                }

                // 事前に長さを計算してメモリを確保
                let route_i = &routes[i];
                let route_j = &routes[j];
                let len_i = route_lens[i];
                let len_j = route_lens[j];

                // マージの方法（route_iの後にroute_jを追加するか、route_jの後にroute_iを追加するか）をランダムに決める
                let merge_method = rng.gen_range(0..2);
                let mut merged_route = Vec::with_capacity(route_i.len() + route_j.len());

                if merge_method == 0 {
                    // route_iの後にroute_jを追加
                    merged_route.extend_from_slice(route_i);
                    merged_route.extend_from_slice(route_j);
                } else {
                    // route_jの後にroute_iを追加
                    merged_route.extend_from_slice(route_j);
                    merged_route.extend_from_slice(route_i);
                }

                // 制約チェック
                if !can_carry(&merged_route, &_in.w, &_in.d) {
                    continue;
                }

                let new_len = route_len(&merged_route);

                let diff = (len_i + len_j) as f64 - new_len as f64;
                if diff > 0.0 || rng.gen::<f64>() < (diff / T).exp() {
                    routes[i] = merged_route;
                    route_lens[i] = new_len;
                    routes.remove(j); // jはiより大きいので、削除しても問題ない
                    route_lens.remove(j);
                    move_count += 1;
                }
            },
            5 => {
                // 方法5: ルートを分割する
                let i = rng.gen_range(0..routes.len());

                if routes[i].len() < 3 {
                    continue;
                }

                let mut route_i = routes[i].clone();
                let len_i = route_lens[i];

                let pos = rng.gen_range(1..route_i.len()-1); // 分割位置をランダムに選択（ただし、最初と最後の点は分割しない）

                let new_route = route_i.split_off(pos);

                // 制約チェック
                if !can_carry(&route_i, &_in.w, &_in.d) || !can_carry(&new_route, &_in.w, &_in.d) {
                    continue;
                }

                let new_len_i = route_len(&route_i);
                let new_len_j = route_len(&new_route);
                let diff = len_i as f64 - (new_len_i + new_len_j) as f64;
                if diff > 0.0 || rng.gen::<f64>() < (diff / T).exp() {
                    // より良い解なら採用
                    routes[i] = route_i;
                    route_lens[i] = new_len_i;
                    routes.push(new_route);
                    route_lens.push(new_len_j);
                    move_count += 1;
                }
            },
            6 | 7 | 8 | 9 | 10 | 11 => {
                // 方法6: 2つのルート間で後半部分を交換する
                let i = rng.gen_range(0..routes.len());
                let j = rng.gen_range(0..routes.len());

                if i == j || routes[i].len() < 2 || routes[j].len() < 2 {
                    continue;
                }

                let mut route_i = routes[i].clone();
                let mut route_j = routes[j].clone();

                let len_i = route_lens[i];
                let len_j = route_lens[j];

                // 各ルートで交換する位置（後半部分の開始位置）をランダムに決める
                let pos_i = rng.gen_range(1..route_i.len());
                let pos_j = rng.gen_range(1..route_j.len());

                // 後半部分を交換
                let tail_i = route_i.split_off(pos_i);
                let tail_j = route_j.split_off(pos_j);

                // 容量を確保して要素を追加することで再アロケーションを減らす
                route_i.reserve(tail_j.len());
                route_j.reserve(tail_i.len());

                route_i.extend(tail_j);
                route_j.extend(tail_i);

                // 制約チェック
                if !can_carry(&route_i, &_in.w, &_in.d) || !can_carry(&route_j, &_in.w, &_in.d) {
                    continue;
                }

                let new_len_i = route_len(&route_i);
                let new_len_j = route_len(&route_j);
                let diff = (len_i + len_j) as f64 - (new_len_i + new_len_j) as f64;
                if diff > 0.0 || rng.gen::<f64>() < (diff / T).exp() {
                    // より良い解なら採用
                    routes[i] = route_i;
                    routes[j] = route_j;
                    route_lens[i] = new_len_i;
                    route_lens[j] = new_len_j;
                    move_count += 1;
                }
            },
            _ => unreachable!(),
        }
        iter += 1;

        if iter % 100 == 0 {
            // 温度を下げる
            let elapsed_time = start_time.elapsed();
            let progress = elapsed_time.as_secs_f64() / time_limit.as_secs_f64();
            //T = (T_end - T_init) * progress + T_init;
            T = (T_init - K).powf(1.0 - progress) * (T_end - K).powf(progress) + K;
        }
    }

    // 標準エラー出力
    eprintln!("iter: {}, move_count: {}", iter, move_count);

    // 空のルートを削除
    routes = routes.into_iter().filter(|route| !route.is_empty()).collect();

    make_answer(&routes)
}

fn input() -> Input {
    input! {
        N: usize,
        w: [[usize; N]; N],
        d: [[isize; N]; N],
    }

    Input {
        N,
        w,
        d,
    }
}

fn output(res: Vec<String>) {
    for r in res {
        println!("{}", r);
    }
}

fn main() {
    let _in = input();
    let res = solve(_in);
    output(res);
}
