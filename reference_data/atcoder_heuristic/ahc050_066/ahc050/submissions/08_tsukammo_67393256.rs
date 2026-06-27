#![allow(non_snake_case, unused)]
use std::time::{Duration, Instant};
use std::time::SystemTime;
use std::cmp::min;
use std::collections::BinaryHeap;
use std::cmp::Reverse;

/**
 * AHC050
 * @author tsukammo
 */

// from https://qiita.com/hatoo@github/items/fa14ad36a1b568d14f3e
macro_rules! get {
    ($t:ty) => {
        {
            let mut line: String = String::new();
            std::io::stdin().read_line(&mut line).unwrap();
            line.trim().parse::<$t>().unwrap()
        }
    };
    ($($t:ty),*) => {
        {
            let mut line: String = String::new();
            std::io::stdin().read_line(&mut line).unwrap();
            let mut iter = line.split_whitespace();
            (
                $(iter.next().unwrap().parse::<$t>().unwrap(),)*
            )
        }
    };
    ($t:ty; $n:expr) => {
        (0..$n).map(|_|
            get!($t)
        ).collect::<Vec<_>>()
    };
    ($($t:ty),*; $n:expr) => {
        (0..$n).map(|_|
            get!($($t),*)
        ).collect::<Vec<_>>()
    };
    ($t:ty ;;) => {
        {
            let mut line: String = String::new();
            std::io::stdin().read_line(&mut line).unwrap();
            line.split_whitespace()
                .map(|t| t.parse::<$t>().unwrap())
                .collect::<Vec<_>>()
        }
    };
    ($t:ty ;; $n:expr) => {
        (0..$n).map(|_| get!($t ;;)).collect::<Vec<_>>()
    };
}

const N: usize = 40;
const NI: isize = 40;

// Util: 座標 (r, c) が盤面の外周セルに接しているかどうかを判定するヘルパー関数
fn is_outer_cell(r: usize, c: usize) -> bool {
    r == 0 || r == N - 1 || c == 0 || c == N - 1
}

// Util: 座標 (r, c) が盤面の外周から2列目のセルに接しているかどうかを判定するヘルパー関数
fn is_second_outer_cell(r: usize, c: usize) -> bool {
    // 外周セルではなく、かつRまたはC座標が2列目の条件を満たすか
    !is_outer_cell(r, c) && (r == 1 || r == N - 2 || c == 1 || c == N - 2)
}

// ヘルパー関数: 指定されたマスから指定された方向に移動した際の最終着地マスを計算
// current_rock_mapは常に最新のrock_mapを渡す必要がある
fn calculate_single_dest_cell(start_r: usize, start_c: usize, dir_idx: usize, current_rock_map: &Vec<Vec<bool>>) -> (usize, usize) {
    const DR: [isize; 4] = [-1, 1, 0, 0]; 
    const DC: [isize; 4] = [0, 0, -1, 1];
    
    let mut curr_r = start_r as isize;
    let mut curr_c = start_c as isize;
    loop {
        let next_r = curr_r + DR[dir_idx];
        let next_c = curr_c + DC[dir_idx];

        // 盤面の外に出るか、岩にぶつかる場合
        if next_r < 0 || next_r >= NI || next_c < 0 || next_c >= NI || current_rock_map[next_r as usize][next_c as usize] {
            return (curr_r as usize, curr_c as usize); // 現在のマスが着地マス
        }
        curr_r = next_r;
        curr_c = next_c;
    }
}

// ゲーム状態の初期化
fn initialize_game_state() -> (Vec<Vec<bool>>, Vec<Vec<f64>>, Vec<Vec<[(usize, usize); 4]>>, usize) {
    let (n_usize, m) = get!(usize, usize);
    assert_eq!(N, n_usize, "Nは40に固定されていますが、入力値が異なります。"); // 念のためのチェック

    let mut rock_map: Vec<Vec<bool>> = vec![vec![false; N]; N];
    let mut initial_empty_cells_count = 0;
    for r in 0..N {
        let s_row = get!(String);
        for (c, ch) in s_row.chars().enumerate() {
            if ch == '#' {
                rock_map[r][c] = true;
            } else {
                initial_empty_cells_count += 1;
            }
        }
    }
    
    let num_empty_to_place = N * N - m; 
    assert_eq!(num_empty_to_place, initial_empty_cells_count, "初期の空きマス数とN^2-Mが一致しません。"); // 念のためのチェック

    let mut robot_dist: Vec<Vec<f64>> = vec![vec![0.0; N]; N];
    let initial_prob_per_cell = 1.0 / (initial_empty_cells_count as f64);
    for r in 0..N {
        for c in 0..N {
            if !rock_map[r][c] {
                robot_dist[r][c] = initial_prob_per_cell;
            }
        }
    }

    let mut dest: Vec<Vec<[(usize, usize); 4]>> = vec![vec![[(0, 0); 4]; N]; N];
    for r in 0..N {
        for c in 0..N {
            for d_idx in 0..4 {
                dest[r][c][d_idx] = calculate_single_dest_cell(r, c, d_idx, &rock_map);
            }
        }
    }
    (rock_map, robot_dist, dest, num_empty_to_place)
}

// ロボットの次の着地確率分布を計算
#[inline(always)]
fn calculate_next_robot_distribution(
    turn: usize,
    robot_dist: &Vec<Vec<f64>>,
    dest: &Vec<Vec<[(usize, usize); 4]>>,
    rock_map: &Vec<Vec<bool>>
) -> Vec<Vec<f64>> {
    const DR: [isize; 4] = [-1, 1, 0, 0]; 
    const DC: [isize; 4] = [0, 0, -1, 1];
    let mut p_after_move: Vec<Vec<f64>> = vec![vec![0.0; N]; N];

    for r_start in 0..N {
        for c_start in 0..N {
            if robot_dist[r_start][c_start] > f64::EPSILON {
                for d_idx in 0..4 {
                    let (dr, dc) = dest[r_start][c_start][d_idx];
                    p_after_move[dr][dc] += robot_dist[r_start][c_start] / 4.0;
                }
            }
        }
    }
    p_after_move
}

// 最適な岩の配置場所を見つける (この関数は現在使われていませんが、残しておきます)
fn find_optimal_placement(
    p_after_move: &Vec<Vec<f64>>,
    rock_map: &Vec<Vec<bool>>
) -> Option<(usize, usize, f64)> { // Option<(r, c, min_prob)>
    let mut min_prob = f64::MAX;
    let mut best_r = 0;
    let mut best_c = 0;
    let mut found_best = false;

    let center_r = (N - 1) as f64 / 2.0;
    let center_c = (N - 1) as f64 / 2.0;
    let mut max_dist_sq = -1.0f64;

    const THRESHOLD_ZERO_CHECK: f64 = 1e-10; 
    const THRESHOLD_GENERAL_COMPARE: f64 = 1e-12;
    const OUTER_CELL_BONUS: f64 = 1e-5;

    for r in 0..N {
        for c in 0..N {
            if !rock_map[r][c] {
                let mut current_cell_prob = p_after_move[r][c];
                let current_cell_dist_sq = (r as f64 - center_r).powi(2) + (c as f64 - center_c).powi(2);

                if is_outer_cell(r, c) {
                    current_cell_prob += OUTER_CELL_BONUS;
                }

                let min_prob_is_zero = min_prob.abs() < THRESHOLD_ZERO_CHECK;
                let current_prob_is_zero = current_cell_prob.abs() == 0.0; // p_after_moveは負にはならないのでこれでOK

                if current_prob_is_zero {
                    if !min_prob_is_zero {
                        min_prob = current_cell_prob;
                        best_r = r;
                        best_c = c;
                        max_dist_sq = current_cell_dist_sq;
                        found_best = true;
                    } else {
                        if current_cell_dist_sq > max_dist_sq {
                            best_r = r;
                            best_c = c;
                            max_dist_sq = current_cell_dist_sq;
                        }
                    }
                } else {
                    if min_prob_is_zero {
                        // min_prob (0) の方が小さいので何もしない
                    } else {
                        if current_cell_prob < min_prob {
                            min_prob = current_cell_prob;
                            best_r = r;
                            best_c = c;
                            max_dist_sq = current_cell_dist_sq;
                            found_best = true;
                        } else if (current_cell_prob - min_prob).abs() < THRESHOLD_GENERAL_COMPARE {
                            if current_cell_dist_sq > max_dist_sq {
                                best_r = r;
                                best_c = c;
                                max_dist_sq = current_cell_dist_sq;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if found_best {
        Some((best_r, best_c, min_prob))
    } else {
        None
    }
}

// 岩の配置後のゲーム状態を更新
fn update_game_state_after_placement(
    placed_r: usize, placed_c: usize,
    colliding_prob: f64,
    rock_map: &mut Vec<Vec<bool>>,
    robot_dist: &mut Vec<Vec<f64>>,
    p_after_move: &Vec<Vec<f64>>, // p_after_moveからrobot_distへコピーするため
    dest: &mut Vec<Vec<[(usize, usize); 4]>>
) -> bool { // true: 続行, false: 強制終了
    rock_map[placed_r][placed_c] = true;

    for r in 0..N {
        for c in 0..N {
            robot_dist[r][c] = p_after_move[r][c];
        }
    }
    robot_dist[placed_r][placed_c] = 0.0;

    let normalization_factor = 1.0 - colliding_prob;
    if normalization_factor < f64::EPSILON {
        // eprintln!("Info: ロボットが潰れる確率がほぼ100%になったため、残りを適当に埋めます。");
        return false; // 強制終了
    }

    // dest テーブルの更新
    // 垂直方向の更新: placed_c を通る上下の動き
    for r_curr in 0..N {
        if r_curr < placed_r { 
            dest[r_curr][placed_c][1] = calculate_single_dest_cell(r_curr, placed_c, 1, &rock_map);
        }
        if r_curr > placed_r {
            dest[r_curr][placed_c][0] = calculate_single_dest_cell(r_curr, placed_c, 0, &rock_map);
        }
    }
    // 水平方向の更新: placed_r を通る左右の動き
    for c_curr in 0..N {
        if c_curr < placed_c {
            dest[placed_r][c_curr][3] = calculate_single_dest_cell(placed_r, c_curr, 3, &rock_map);
        }
        if c_curr > placed_c {
            dest[placed_r][c_curr][2] = calculate_single_dest_cell(placed_r, c_curr, 2, &rock_map);
        }
    }
    true // 続行
}

// Step1: 存在確率の高いマスから順に調べ、上下左右に空きマスが2つ以上ある場合のみ処理
fn step1_placement(
    rock_map: &mut Vec<Vec<bool>>,
    robot_dist: &mut Vec<Vec<f64>>,
    dest: &mut Vec<Vec<[(usize, usize); 4]>>,
    p_sequence: &mut Vec<(usize, usize)>,
    num_empty_to_place: usize, 
    current_prob_not_crushed: &mut f64, 
    expected_prize: &mut f64, 
    enable_random_in_step1: bool, // 追加: 乱数有効化フラグ
    rng: &mut XorShift, // 追加: 乱数ジェネレータ
) {
    // eprintln!("Step1開始: ターン{}", p_sequence.len()); // 試行中は抑制
    
    const DR: [isize; 4] = [-1, 1, 0, 0]; 
    const DC: [isize; 4] = [0, 0, -1, 1];
    
    let mut current_turn_step1 = p_sequence.len(); // Step1開始時のターン数を初期値とする
    
    loop {

        let p_after_move = calculate_next_robot_distribution(current_turn_step1, &robot_dist, &dest, &rock_map); // current_turn_step1 を渡す
        
        // 配置候補を格納 (score, r_b, c_b)
        let mut placement_candidates_heap: BinaryHeap<Reverse<(u64, usize, usize)>> = BinaryHeap::new();
        const N_TOP_CANDIDATES: usize = 40; // ここでブレの幅を調整

        // 存在確率の高いマスを直接ヒープに格納する
        // scoreがprob_aに依存するため、まずprob_aが高いマスをN_TOP_CANDIDATES個選ぶためのヒープ
        let mut top_prob_a_heap: BinaryHeap<Reverse<(u64, usize, usize)>> = BinaryHeap::new(); // prob_aを保存するための最小ヒープ
        const N_TOP_PROB_A_CANDIDATES: usize = 100; // prob_aの高い候補をいくつ考慮するか (調整可能)

        for r_a in 0..N {
            for c_a in 0..N {
                if !rock_map[r_a][c_a] {
                    let prob_a = p_after_move[r_a][c_a];

                    if prob_a < 0.000001 { // 低い確率のマスは考慮しない (ただし、これだと途中のマスがスキップされる)
                        continue;
                    }

                    // prob_aが大きい順に上位N_TOP_PROB_A_CANDIDATES個を保持 (Reverseで最大ヒープのように扱う)
                    if top_prob_a_heap.len() < N_TOP_PROB_A_CANDIDATES {
                        top_prob_a_heap.push(Reverse((prob_a.to_bits(), r_a, c_a))); // 修正: ReverseのReverseは不要
                    } else if prob_a.to_bits() > top_prob_a_heap.peek().unwrap().0.0 { // 修正: .0.0 を追加
                        // 現在のprob_aがヒープ内の最小値より大きければ置換
                        top_prob_a_heap.pop();
                        top_prob_a_heap.push(Reverse((prob_a.to_bits(), r_a, c_a)));
                    }
                }
            }
        }

        // top_prob_a_heapから、prob_aの高い順に候補を取り出し、step1の基準で評価してplacement_candidates_heapに格納
        let mut prob_cells_evaluated: Vec<(f64, usize, usize)> = top_prob_a_heap.into_iter()
            .map(|Reverse((prob_a_bits, r_a, c_a))| (f64::from_bits(prob_a_bits), r_a, c_a))
            .collect();
        prob_cells_evaluated.sort_by(|a, b| b.0.partial_cmp(&a.0).unwrap()); // 降順ソートし直す

        for (prob_a, r_a, c_a) in prob_cells_evaluated {
            // マスAの上下左右の空きマスを調べる
            let mut empty_neighbors: [Option<(usize, usize)>; 4] = [None; 4]; // Vecから固定長配列に変更
            let mut empty_neighbors_count = 0; // 有効な隣接マスの数をカウント

            for d in 0..4 {
                let nr = r_a as isize + DR[d];
                let nc = c_a as isize + DC[d];
                if nr >= 0 && nr < NI && nc >= 0 && nc < NI {
                    let nr = nr as usize;
                    let nc = nc as usize;
                    if !rock_map[nr][nc] {
                        empty_neighbors[empty_neighbors_count] = Some((nr, nc));
                        empty_neighbors_count += 1;
                    }
                }
            }
            
            // 空きマスが1つしかない場合はskip
            if empty_neighbors_count < 2 { // len() の代わりにカウントを使用
                continue;
            }
            
            // 空きマスの中で最も存在確率の低いマスBを見つける
            let mut min_prob_b = f64::MAX;
            let mut best_b: Option<(usize, usize)> = None;
            for i in 0..empty_neighbors_count { // 固定長配列の有効範囲のみをイテレート
                if let Some((r_b, c_b)) = empty_neighbors[i] {
                    let prob_b = p_after_move[r_b][c_b];
                    if prob_b < min_prob_b {
                        min_prob_b = prob_b;
                        best_b = Some((r_b, c_b));
                    }
                }
            }
            
            if let Some((r_b, c_b)) = best_b {
                let score = min_prob_b * 1000.0 - prob_a; // スコアは小さいほど良い
                
                // ヒープに候補を追加し、N_TOP_CANDIDATES個に制限
                if placement_candidates_heap.len() < N_TOP_CANDIDATES {
                    placement_candidates_heap.push(Reverse((score.to_bits(), r_b, c_b)));
                } else if score.to_bits() < placement_candidates_heap.peek().unwrap().0.0 {
                    // 現在のスコアがヒープ内の最も悪いスコア（最大値）より良ければ、それを置き換える
                    placement_candidates_heap.pop();
                    placement_candidates_heap.push(Reverse((score.to_bits(), r_b, c_b)));
                }
            }
        }
        
        // 最適な配置が見つからない場合、step1終了
        if placement_candidates_heap.is_empty() { // Heapが空かチェック
            // eprintln!("Step1終了 (最適な配置なし): ターン{}", p_sequence.len());
            break;
        }
        
        let (best_r, best_c);
        if enable_random_in_step1 {
            // スコアが最も良いものから、上位 N_TOP_CANDIDATES 個の中からランダムに選択
            // ヒープから全て取り出してVecに変換
            let mut top_candidates_vec: Vec<(f64, usize, usize)> = 
                placement_candidates_heap.into_iter()
                    .map(|Reverse((score_bits, r, c))| (f64::from_bits(score_bits), r, c))
                    .collect();
            
            let num_to_choose_from = top_candidates_vec.len();
            let idx = rng.next_usize(num_to_choose_from);
            best_r = top_candidates_vec[idx].1;
            best_c = top_candidates_vec[idx].2;
        } else {
            // ブレなしの場合、最もスコアが良いものを選択 (ヒープのルート)
            let Reverse((_score_bits, r, c)) = placement_candidates_heap.pop().unwrap();
            best_r = r;
            best_c = c;
        }
        
        // ブロックを置くマスの、ブロックを置く前の確率
        let colliding_prob = p_after_move[best_r][best_c]; 

        // ブロックを置くマスの存在確率が閾値より大きい場合、配置せずにstep1を終了
        if colliding_prob > 0.001 { // 閾値は調整可能です
            // eprintln!("Step1終了 (マスBの確率が高すぎる): ターン{}, 確率={:.10e}", p_sequence.len(), colliding_prob);
            break;
        }
        
        // 期待値の更新
        *expected_prize += *current_prob_not_crushed; // このターンで岩を置けたことで1円獲得できる期待値を加算
        *current_prob_not_crushed *= (1.0 - colliding_prob); // ロボットが潰れなかった場合の次のターンの確率

        p_sequence.push((best_r, best_c));
        current_turn_step1 += 1; // ここでカウンターをインクリメント
        
        if !update_game_state_after_placement(best_r, best_c, colliding_prob, rock_map, robot_dist, &p_after_move, dest) {
            // eprintln!("Step1終了 (ロボットが潰れる確率がほぼ100%): ターン{}", p_sequence.len());
            *current_prob_not_crushed = 0.0; // 強制終了なので、今後の確率は0
            break;
        }
    }
}

// Step2: 存在確率の低い順にマスを選び、そのマスをAと呼び、スコアを計算して配置
fn step2_placement(
    rock_map: &mut Vec<Vec<bool>>,
    robot_dist: &mut Vec<Vec<f64>>,
    dest: &mut Vec<Vec<[(usize, usize); 4]>>,
    p_sequence: &mut Vec<(usize, usize)>,
    num_empty_to_place: usize, 
    current_prob_not_crushed: &mut f64, 
    expected_prize: &mut f64, 
) {
    // eprintln!("Step2開始: ターン{}", p_sequence.len()); // 試行中は抑制
    
    const DR: [isize; 4] = [-1, 1, 0, 0]; 
    const DC: [isize; 4] = [0, 0, -1, 1];
    
    // Step2開始時点での、残りの配置回数を計算
    let remaining_placements = num_empty_to_place - p_sequence.len();
    let mut current_turn_step2 = p_sequence.len(); // Step2開始時のターン数を初期値とする

    // 残りの配置回数だけループを回す
    for _ in 0..remaining_placements {
        // ロボットが既に潰れている（確率がほぼ0）なら終了 (この条件は維持)
        if *current_prob_not_crushed < f64::EPSILON {
            *current_prob_not_crushed = 0.0;
            break;
        }

        // ロボットの次の着地確率分布を計算
        let p_after_move = calculate_next_robot_distribution(current_turn_step2, &robot_dist, &dest, &rock_map); // current_turn_step2 を渡す
        
        let mut best_score = f64::MAX;
        let mut best_score_prob_a = f64::MAX;
        let mut best_placement: Option<(usize, usize)> = None;
        
        // 全ての空きマスを直接走査して最適なマスを探す
        for r_a in 0..N {
            for c_a in 0..N {
                if !rock_map[r_a][c_a] { // 岩が置かれていない空きマスのみを候補とする
                    let prob_a = p_after_move[r_a][c_a]; // 対象マスの確率
                    // 枝刈りの条件を追加
                    // 現在のスコアが、これまでの最良スコアを十分に超えている場合、この候補は無視する
                    // 例えば、ベストスコアの1.1倍より大きければスキップするなど。
                    // ただし、スコアは小さいほど良いので、 best_score + THRESHOLD < current_score となる場合をスキップ
                    const PRUNING_THRESHOLD_SCORE: f64 = 1e-5; // スコアの閾値、調整してください
                    if prob_a > best_score_prob_a + PRUNING_THRESHOLD_SCORE {
                        continue; // この候補は現在の最良よりも明らかに悪いのでスキップ
                    }

                    let mut total_scanned_prob_along_paths = 0.0;
                    let mut total_immediate_neighbor_prob = 0.0; 
                    
                    // マスAから上下左右に壁にぶつかるまで走査し、各マスの存在確率を合計
                    for d in 0..4 {
                        let mut curr_r = r_a as isize + DR[d];
                        let mut curr_c = c_a as isize + DC[d];

                        // まずは隣接1マス目の処理
                        if curr_r >= 0 && curr_r < NI && curr_c >= 0 && curr_c < NI {
                            let nr = curr_r as usize;
                            let nc = curr_c as usize;
                            if !rock_map[nr][nc] {
                                total_immediate_neighbor_prob += p_after_move[nr][nc];
                            }
                        }

                        // その後、壁にぶつかるまで走査
                        loop {
                            // 盤面の外に出るか、岩にぶつかる場合
                            if curr_r < 0 || curr_r >= NI || curr_c < 0 || curr_c >= NI || rock_map[curr_r as usize][curr_c as usize] {
                                break;
                            }
                            total_scanned_prob_along_paths += p_after_move[curr_r as usize][curr_c as usize];
                            
                            curr_r += DR[d];
                            curr_c += DC[d];
                        }
                    }
                    
                    // スコアの計算: マスAの存在確率 * 1000.0 + (パス上の合計) - (隣接1マスのみの合計)
                    let current_score = prob_a * 1000.0 + total_scanned_prob_along_paths - total_immediate_neighbor_prob * 0.1;
                    

                    if current_score < best_score {
                        best_score = current_score;
                        best_placement = Some((r_a, c_a));
                        best_score_prob_a = prob_a;
                    }
                }
            }
        }
        
        // 最適な配置が見つからない場合、step2終了
        if best_placement.is_none() {
            // eprintln!("Step2終了 (最適な配置なし): ターン{}", p_sequence.len());
            break;
        }
        
        let (best_r, best_c) = best_placement.unwrap();
        // ブロックを置くマスの、ブロックを置く前の確率
        let colliding_prob = p_after_move[best_r][best_c]; 
        
        // 期待値の更新
        *expected_prize += *current_prob_not_crushed;
        *current_prob_not_crushed *= (1.0 - colliding_prob);
        
        p_sequence.push((best_r, best_c));
        current_turn_step2 += 1; // ここでカウンターをインクリメント
        
        if !update_game_state_after_placement(best_r, best_c, colliding_prob, rock_map, robot_dist, &p_after_move, dest) {
            *current_prob_not_crushed = 0.0;
            break;
        }
    }
}

// 1回の試行を実行する関数
fn solve_one_trial(
    mut rock_map: Vec<Vec<bool>>, // クローンされた初期状態を受け取る
    mut robot_dist: Vec<Vec<f64>>, // クローンされた初期状態を受け取る
    mut dest: Vec<Vec<[(usize, usize); 4]>>, // クローンされた初期状態を受け取る
    num_empty_to_place: usize,
    enable_random_in_step1: bool, 
    enable_random_in_step2: bool,
    seed: usize,
) -> (Vec<(usize, usize)>, f64) {
    let mut rng = XorShift::new(seed); 

    let mut p_sequence: Vec<(usize, usize)> = Vec::with_capacity(num_empty_to_place);
    let mut current_prob_not_crushed: f64 = 1.0;
    let mut expected_prize: f64 = 0.0;

    step1_placement(
        &mut rock_map, 
        &mut robot_dist, 
        &mut dest, 
        &mut p_sequence, 
        num_empty_to_place, 
        &mut current_prob_not_crushed, 
        &mut expected_prize,
        enable_random_in_step1, 
        &mut rng, 
    );
    
    step2_placement(
        &mut rock_map, 
        &mut robot_dist, 
        &mut dest, 
        &mut p_sequence,
        num_empty_to_place, 
        &mut current_prob_not_crushed, 
        &mut expected_prize,
    );
    
    // 残りの空きマスをすべて埋める
    if p_sequence.len() < num_empty_to_place {
        for r_fill in 0..N {
            for c_fill in 0..N {
                if !rock_map[r_fill][c_fill] {
                    rock_map[r_fill][c_fill] = true; 
                    p_sequence.push((r_fill, c_fill));
                }
            }
        }
    }

    (p_sequence, expected_prize) 
}

fn main() {
    get_time_init();

    // 1. ゲーム状態の初期化 (一度だけ実行)
    // 注意: initialize_game_stateは標準入力を読み込むため、複数回呼び出すとエラーになります。
    let (initial_rock_map, initial_robot_dist, initial_dest, num_empty_to_place) = initialize_game_state();
    
    let mut best_p_sequence: Vec<(usize, usize)> = Vec::new();
    let mut max_expected_prize: f64 = -1.0;

    let mut trial_count = 0; // 試行回数をカウントする変数
    
    loop {
        trial_count += 1;

        // タイムリミットチェック
        // 各試行の開始前に全体の実行時間を見て、残り時間が少なければ終了
        if get_time() > TIME_LIMIT * 0.97 { // 余裕を持って終了
            eprintln!("タイムリミットが近いので終了します。現在の試行数: {}", trial_count - 1);
            break;
        }
        
        let enable_random_in_step1 = trial_count != 1; // 1回目はブレなし、2回目以降はブレあり
        let enable_random_in_step2 = trial_count != 1; // 1回目はブレなし、2回目以降はブレあり
        let seed = trial_count; // シードを試行回数にする (シンプルにするため)

        // solve_one_trial関数に初期状態のクローンを渡す
        let (p_sequence, expected_prize) = solve_one_trial(
            initial_rock_map.clone(), 
            initial_robot_dist.clone(), 
            initial_dest.clone(), 
            num_empty_to_place,
            enable_random_in_step1, 
            enable_random_in_step2, 
            seed,
        );

        // 各試行ごとのスコアを出力
        let max_e_for_current_trial = (num_empty_to_place - 1) as f64; 
        let current_score = (expected_prize / max_e_for_current_trial * 1_000_000.0).round();
        // eprintln!("  試行 {} 期待値E: {:.6}, スコア: {:.0}", trial_count, expected_prize, current_score);

        if expected_prize > max_expected_prize {
            max_expected_prize = expected_prize;
            best_p_sequence = p_sequence;
            // eprintln!("  => 過去最高の期待値を更新しました！");
        }
    }

    // 最も良い結果を出力
    for (r, c) in &best_p_sequence {
        println!("{} {}", r, c);
    }

    // 最終スコアの計算と出力
    let final_e = max_expected_prize;
    // 最大獲得賞金はN^2-M-1
    let max_e = (num_empty_to_place - 1) as f64; 
    let score = (final_e / max_e * 1_000_000.0).round();
    eprintln!("--- 最終結果 ---");
    eprintln!("最終的に選ばれた期待値E: {:.6}", final_e); 
    eprintln!("最大賞金N^2-M-1: {:.0}", max_e);
    eprintln!("最終スコア: {:.0}", score);

    unsafe {
        eprintln!("合計実行時間: {:.3}秒", get_time());
    }
}

// Util
// 時間計測用のインスタンス
static mut START_TIME_INS: Option<Instant> = None;
#[inline(always)]
fn get_time() -> f64 {
    unsafe {
        START_TIME_INS.unwrap().elapsed().as_secs_f64()
    }
}
fn  get_time_init() {
    unsafe {
        START_TIME_INS = Some(Instant::now());
    }
}
// シンプルな擬似乱数生成器（XorShift）
struct XorShift {
    w: u32,
    x: u32,
    y: u32,
    z: u32,
}
impl XorShift {
    fn new(seed: usize) -> Self {
        XorShift {
            x: seed as u32,
            y: 362436069,
            z: 521288629,
            w: 88675123,
        }
    }
    #[inline(always)]
    fn next_u32(&mut self) -> u32 {
        let t = self.x ^ (self.x << 11);
        self.x = self.y;
        self.y = self.z;
        self.z = self.w;
        self.w = self.w ^ (self.w >> 19) ^ (t ^ (t >> 8));
        self.w
    }
    #[inline(always)]
    fn next_usize(&mut self, n: usize) -> usize {
        (self.next_u32() as usize) % n
    }
    #[inline(always)]
    fn next_isize(&mut self, n: isize) -> isize {
        (self.next_u32() as isize) % n
    }
    #[inline(always)]
    fn next_f64(&mut self) -> f64 {
        (self.next_u32() as f64) / (u32::MAX as f64)
    }
}
// 高速な指数関数近似関数
#[inline(always)]
fn fast_exp(x: f64) -> f64 {
    const LOG2_E: f64 = 1.4426950408889634; // 1 / ln(2)
    const EXP_MULT: f64 = (1u64 << 52) as f64; // 2^52
    const EXP_BIAS: u64 = 1023u64 << 52;        // f64用のバイアス
    let i = (x * EXP_MULT * LOG2_E) as i64 + EXP_BIAS as i64;
    f64::from_bits(i as u64)
}

// setting
const TIME_LIMIT: f64 = 1.990*1.0;