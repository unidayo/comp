#![allow(dead_code, unused_macros, unused_imports, non_camel_case_types, non_upper_case_globals, non_snake_case)]

use proconio::source::auto::AutoSource;
use proconio::{input, marker::Chars, source::line::LineSource};
use num::Integer;
use core::panic;
use std::borrow::Borrow;
use std::char::MAX;
use std::collections::btree_set::Union;
use std::collections::{BinaryHeap, BTreeSet, VecDeque, HashMap, vec_deque, HashSet};
use std::f64::consts::PI;
use std::fs::File;
use std::fs;
use std::io::{self, stdin, stdout, Read, Write, BufReader, BufWriter};
use std::sync::Arc;
use std::{char, cmp, vec, mem};
use rand::Rng;
use std::cmp::{Reverse, Ordering};
use itertools::Itertools;
use std::time::{Duration, Instant};
use std::ops::Bound::{Included, Unbounded, Excluded};

use rand_pcg::{Pcg64Mcg, Mcg128Xsl64};
use rand::prelude::*;

const MODULO: i128 = 1e9 as i128 + 7;
const INF_i32: i32 = 1111111111;                    
const INF_i64: i64 = 1000000000000000001;
const INF_USIZE: usize = 2000000001;
const DX: [i32; 9] = [0,  0, 1, -1, 1,  1, -1, -1, 0 ];
const DY: [i32; 9] = [1, -1, 0,  0, 1, -1,  1, -1, 0 ];
const DXY: [(usize, usize); 4] = [(0, 1), (1, 0), (0, !0), (!0, 0)];
const DXY9: [(usize, usize); 9] = [(0, 1), (1, 0), (0, !0), (!0, 0), (1, 1), (1, !0), (!0, !0), (!0, 1), (0, 0)];


macro_rules! mat {($($e:expr),*) => { Vec::from(vec![$($e),*]) };($($e:expr,)*) => { Vec::from(vec![$($e),*]) };($e:expr; $d:expr) => { Vec::from(vec![$e; $d]) };($e:expr; $d:expr $(; $ds:expr)+) => { Vec::from(vec![mat![$e $(; $ds)*]; $d]) };}
macro_rules! chmin {($base:expr, $($cmps:expr),+ $(,)*) => {{let min = min!($($cmps),+);if $base > min {$base = min;true} else {false}}};}
macro_rules! chmax {($base:expr, $($cmps:expr),+ $(,)*) => {{let max = max!($($cmps),+);if $base < max {$base = max;true} else {false}}};}
macro_rules! min {($a:expr $(,)*) => {{$a}};($a:expr, $b:expr $(,)*) => {{std::cmp::min($a, $b)}};($a:expr, $($rest:expr),+ $(,)*) => {{std::cmp::min($a, min!($($rest),+))}};}
macro_rules! max {($a:expr $(,)*) => {{$a}};($a:expr, $b:expr $(,)*) => {{std::cmp::max($a, $b)}};($a:expr, $($rest:expr),+ $(,)*) => {{std::cmp::max($a, max!($($rest),+))}};}


const IS_LOCAL: bool = cfg!(feature = "local");
static mut SEED: u128 = 42;

const B_BOARD_SIZE: usize = 6;
const BOARD_SIZE: usize = 1 << B_BOARD_SIZE;
const TIME_LIMIT: f64 = 4.95;
static mut TL_COEFF: f64 = 1.0;

static mut c_T0: f64 = 32000.;
static mut c_T1: f64 = 7000.;

static mut c_MUTATE_PROB_0: usize = 6;
static mut c_MUTATE_PROB_1: usize = 26;
static mut c_MUTATE_PROB_2: usize = 88;

static mut c_REMOVE: f64 = 0.926109091815829;

static mut c_METRIC_PROB: usize = 46;
static mut c_MAX_FAIL_COUNT: usize = 4635;
static mut c_CIRCLE_RAD: usize = 7;
static mut c_LEN: usize = 13;

static mut c_REJECT: f64 = 1.;

fn main() {
    // optick::start_capture();

    if IS_LOCAL {
        if std::env::args().len() == 1 {
            // デバッグモード
            unsafe {TL_COEFF = 1.0;}

            let start_seed = 1;
            let n_seeds = 1;
            for seed in start_seed..start_seed+n_seeds {
                unsafe { SEED = seed; }

                eprintln!("===========================");
                eprintln!("== debug mode (seed={}) == ", seed);
                eprintln!("===========================");
                let file = format!("../ahc014-tools/in/{:04}.txt", seed);
                let input = &fs::read_to_string(file).expect("read failed")[..];
                solve(input);
            }
        }
        else if std::env::args().len() == 3 {
            eprintln!("===========================");
            eprintln!("== parallel mode ==");
            eprintln!("===========================");
            let file: String = std::env::args().nth(1).unwrap().parse().unwrap();
            unsafe {TL_COEFF = std::env::args().nth(2).unwrap().parse().unwrap();}
            let input = &fs::read_to_string(file).expect("read failed")[..];
            solve(input);
        }
        else if std::env::args().len() == 14 {
            eprintln!("===========================");
            eprintln!("== tuning mode ==");
            eprintln!("===========================");
            let file: String = std::env::args().nth(1).unwrap().parse().unwrap();
            unsafe {TL_COEFF = std::env::args().nth(2).unwrap().parse().unwrap();}
            unsafe {c_T0 = std::env::args().nth(3).unwrap().parse().unwrap()};
            unsafe {c_T1 = std::env::args().nth(4).unwrap().parse().unwrap()};
            unsafe {c_MUTATE_PROB_0 = std::env::args().nth(5).unwrap().parse().unwrap()};
            unsafe {c_MUTATE_PROB_1 = std::env::args().nth(6).unwrap().parse().unwrap()};
            unsafe {c_MUTATE_PROB_2 = std::env::args().nth(7).unwrap().parse().unwrap()};
            unsafe {c_REMOVE = std::env::args().nth(8).unwrap().parse().unwrap()};
            unsafe {c_METRIC_PROB = std::env::args().nth(9).unwrap().parse().unwrap()};
            unsafe {c_MAX_FAIL_COUNT = std::env::args().nth(10).unwrap().parse().unwrap()};
            unsafe {c_CIRCLE_RAD = std::env::args().nth(11).unwrap().parse().unwrap()};
            unsafe {c_LEN = std::env::args().nth(12).unwrap().parse().unwrap()};
            unsafe {c_REJECT = std::env::args().nth(13).unwrap().parse().unwrap()};
            let input = &fs::read_to_string(file).expect("read failed")[..];
            solve(input);
        }
    } else {
        eprintln!("== submission mode == ");
        solve("Submission");
    }
    
    // optick::stop_capture("capture_name");
}





fn solve(input: &str) {   
    // optick::event!();

    // 入力を貰う
    let input = get_input(input);
    let timelimit = TIME_LIMIT * unsafe {TL_COEFF};
    let mut wall_timer  = Timer::new(timelimit, 1);
    let mut rng = rand_pcg::Pcg64Mcg::new(unsafe{SEED});

    let mut initial_board = Board {
        N: input.N,
        used_nodes: [0; BOARD_SIZE],
        nodes: input.XY.clone(),
        ans: Vec::new(),
        n_used: [0; 1 << (B_BOARD_SIZE * 2)],
        is_edge: [0; 1 << (B_BOARD_SIZE * 2)],
    };
    for &(i, j) in &input.XY {
        initial_board.used_nodes[i] |= 1 << j;
    }

    // 焼きなまし
    let best_board = apply_simulated_annealing(&initial_board, timelimit, &mut rng, &mut wall_timer, &input);
    let best_score = compute_score(&best_board.ans, &input);


    // 出力
    print_ans(&best_board.ans);
    eprintln!("score : {}", best_score);

}



fn apply_simulated_annealing(initial_board: &Board, timelimit: f64, 
    rng: &mut Mcg128Xsl64, wall_timer: &mut Timer, input: &Input) -> Board {

    // optick::event!();

    let mut best_board = initial_board.clone();
    let mut best_score = 0;

    let mut now_board = best_board.clone();
    let mut now_score = 0;

    let mut sa = SimulatedAnnealing::new(
        timelimit, 10, unsafe{c_T0}, unsafe{c_T1}, rng.clone());
    let mut fail_count = 0;

    let mutations = [
        random_subset, remove_outside_of_line, remove_circle, remove_seq];
    let mut n_success = vec![0; mutations.len()];
    let mut n_failure = vec![0; mutations.len()];

    while !sa.timer.timeover() && !wall_timer.timeover() {
        // 盤面を初期化       
        
        let m_type = match rng.gen_range(0, 100) {
            x if x < unsafe{c_MUTATE_PROB_0} => 0,
            x if x < unsafe{c_MUTATE_PROB_1} => 1,
            x if x < unsafe{c_MUTATE_PROB_2} => 2,
            _ => 3,  
        };
        
        let p_remove = (1. - sa.timer.elapsed_rate) * unsafe{c_REMOVE};
        let mut board = match mutations[m_type](&now_board, &initial_board, &input, rng, p_remove) {
            Some(b) => b,
            None => continue,
        };

        // 行き止まりまで進む
        let metric = match rng.gen_range(0, 100) {
            x if x < unsafe{c_METRIC_PROB} => 0,
            _ => 1,  
        };
        straight_search(&mut board, &input, rng, metric);
        
        // スコアを更新
        let score = compute_score(&board.ans, &input);
        let d_worse = now_score - score;
        fail_count += 1;
        if sa.accept(d_worse as f64) {
            if score > best_score {
                best_score = score;
                best_board = board.clone();
                fail_count = 0;
            }

            now_board = board;
            now_score = score;
            n_success[m_type] += 1;
        }
        else {
            n_failure[m_type] += 1;
        }

        // 一定回数改善されなければ、ベストを復元
        if fail_count == unsafe{c_MAX_FAIL_COUNT} {
            now_score = best_score;
            now_board = best_board.clone();
            fail_count = 0;
        }
    }

    eprintln!("====== sa detail ========");
    eprintln!("n_iter: {}", sa.n_iter);
    let mut p_success = vec![0.; mutations.len()];
    for i in 0..mutations.len() {
        p_success[i] = n_success[i] as f64 / (n_success[i] + n_failure[i]) as f64;
    }
    eprintln!("p_success: {:?}", p_success);
    eprintln!("=========================");

    best_board
}





#[derive(Clone)]
struct Board {
    N: usize,
    used_nodes: [usize; BOARD_SIZE],
    nodes: Vec<(usize, usize)>,
    n_used: [usize; 1 << (B_BOARD_SIZE * 2)],
    ans: Vec<Rectangle>,
    is_edge: [usize; 1 << (B_BOARD_SIZE * 2)], // (i, j) の dir が使用可能か = is_edge[i][j] & (1 << dir)
}


fn compute_manhattan(pos1: (usize, usize), pos2: (usize, usize)) -> usize {
    pos1.0.abs_diff(pos2.0) + pos1.1.abs_diff(pos2.1)
}

/// ランダムに p_remove の確率で長方形を削除する
fn random_subset(best_board: &Board, initial_board: &Board, input: &Input, 
    rng: &mut Mcg128Xsl64, p_remove: f64) -> Option<Board> {
    // optick::event!();

    let mut board = initial_board.clone();
    let w_max = input.weight[0][0];

    let mut safety = true;
    let mut n_removed = 0;
    for rect in &best_board.ans {
        let (i, j) = rect.pos[0];
        let p = input.weight[i][j] as f64 / w_max as f64;

        if rng.gen_bool(p_remove * p) {
            safety = false;
            n_removed += 1;
            continue;
        }

        if safety {
            add_rectangle(&mut board, rect);
        }
        else if let Some(r) = check_empty_count(&board, &rect.pos) {
            add_rectangle(&mut board, &r);
        }
        else {
            n_removed += 1;
        }
    }

    let p_removed = n_removed as f64 / best_board.ans.len().max(1) as f64;
    if rng.gen_bool(p_removed.powf(unsafe{c_REJECT})) {
        return None;
    }
    Some(board)
}


fn pos_usize2int(pos: (usize, usize)) -> (i32, i32) {
    // optick::start_capture();

    (pos.0 as i32, pos.1 as i32)
}

/// 直線の上側ならtrueを返す
fn over_f(pos: (usize, usize), pos1: (usize, usize), pos2: (usize, usize)) -> bool {
    // optick::start_capture();

    let (x, y) = pos_usize2int(pos);
    let (x1, y1) = pos_usize2int(pos1);
    let (x2, y2) = pos_usize2int(pos2);

    y * (x2 - x1) > (y2 - y1) * (x - x1) + y1 * (x2 - x1)
}


/// ランダムな直線の外側を削除する
fn remove_outside_of_line(best_board: &Board, initial_board: &Board, input: &Input, 
        rng: &mut Mcg128Xsl64, _: f64) -> Option<Board> {
    // optick::event!();

    let mut board = initial_board.clone();

    let x0 = rng.gen_range(0, input.N);
    let x1 = rng.gen_range(0, input.N);
    let y0 = rng.gen_range(0, input.N);
    let y1 = rng.gen_range(0, input.N);
    if x0 == x1 || y0 == y1 {
        return None;
    }
    let pos1 = (x0, y0);
    let pos2 = (x1, y1);

    let origin_is_over = over_f((input.N / 2, input.N / 2), pos1, pos2);
    
    let mut n_removed = 0;
    let mut safety = true;
    for rect in &best_board.ans {
        if origin_is_over !=  over_f(rect.pos[0], pos1, pos2){
            safety = false;
            n_removed += 1;
            continue;
        }
        
        if safety {
            add_rectangle(&mut board, rect);
        }
        else if let Some(r) = check_empty_count(&board, &rect.pos) {
            add_rectangle(&mut board, &r);
        }
        else {
            n_removed += 1;
        }
    }

    let p_removed = n_removed as f64 / best_board.ans.len().max(1) as f64;
    if rng.gen_bool(p_removed.powf(unsafe{c_REJECT})) {
        return None;
    }
    Some(board)
}

/// ランダムな四角の中を削除する
fn remove_circle(best_board: &Board, initial_board: &Board, input: &Input, 
        rng: &mut Mcg128Xsl64, _: f64) -> Option<Board> {
    // optick::event!();

    let mut board = initial_board.clone();

    let cx = rng.gen_range(input.N / 4, input.N * 3 / 4);
    let cy = rng.gen_range(input.N / 4, input.N * 3 / 4);
    let r = rng.gen_range(1, unsafe{c_CIRCLE_RAD});
    
    let mut n_removed = 0;
    let mut safety = true;
    for rect in &best_board.ans {
        if compute_manhattan(rect.pos[0], (cx, cy)) < r {
            safety = false;
            n_removed += 1;
            continue;
        }
        
        if safety {
            add_rectangle(&mut board, rect);
        }
        else if let Some(r) = check_empty_count(&board, &rect.pos) {
            add_rectangle(&mut board, &r);
        }
        else {
            n_removed += 1;
        }
    }

    let p_removed = n_removed as f64 / best_board.ans.len().max(1) as f64;
    if rng.gen_bool(p_removed.powf(unsafe{c_REJECT})) {
        return None;
    }
    Some(board)
}


/// ランダムに連続した一部を削除する
fn remove_seq(best_board: &Board, initial_board: &Board, _: &Input, rng: &mut Mcg128Xsl64, _: f64) -> Option<Board> {
    // optick::start_capture();

    if best_board.ans.len() == 0 {
        return None;
    }

    let mut board = initial_board.clone();

    let mut n_removed = 0;
    let left = rng.gen_range(0, best_board.ans.len());
    let length = rng.gen_range(0, unsafe{c_LEN});
    let right = min!(left + length, best_board.ans.len());

    let mut safety = true;
    for (i, rect) in best_board.ans.iter().enumerate() {
        if i.isin(left, right) {
            safety = false;
            n_removed += 1;
            continue;
        }
        
        if safety {
            add_rectangle(&mut board, rect);
        }
        else if let Some(r) = check_empty_count(&board, &rect.pos) {
            add_rectangle(&mut board, &r);
        }
        else {
            n_removed += 1;
        }

    }

    let p_removed = n_removed as f64 / best_board.ans.len().max(1) as f64;
    if rng.gen_bool(p_removed.powf(unsafe{c_REJECT})) {
        return None;
    }
    Some(board)
}


fn shuffle_rects(rects: &mut Vec<Rectangle>, rng: &mut Mcg128Xsl64) -> () {
    // optick::event!();

    rects.shuffle(rng);
}

fn straight_search(board: &mut Board, input: &Input, rng: &mut Mcg128Xsl64, metric: usize) -> () {
    // optick::event!();

    // 長方形を列挙
    let mut rects = enumerate_rectangles(&board, &input);

    let metrics = [compute_rect_value, compute_rect_value_old];
    let mut remove_idxs = Vec::with_capacity(300);

    while !rects.is_empty() {
        shuffle_rects(&mut rects, rng);

        // 最も評価値が高いものを使用する
        let mut best_idx = !0;
        let mut best_value = (-INF_i32, -INF_i32, -INF_i32);
        remove_idxs.clear();
        for (i, rect) in rects.iter().enumerate() {

            let value = metrics[metric](board, rect, input);
            if value > best_value {
                if !check_valid_rectangle(board, rect) {
                    remove_idxs.push(i);
                    continue;
                }
                
                best_value = value;
                best_idx = i;
            }
        }
        if best_idx != !0 {
            add_rectangle(board, &rects[best_idx]);
            enumerate_rectangles_single(board, input, rects[best_idx].pos[0], &mut rects);
        }

        while let Some(idx) = remove_idxs.pop() {
            rects.remove(idx);
        }
    }
}


fn get_first_node(board: &Board, input: &Input, pos: (usize, usize), dir: usize) -> (usize, usize) {
    // optick::event!();

    let (i, j) = pos;
    let (di, dj) = DXY9[dir];

    let mut ni = i + di;
    let mut nj = j + dj;
    while input.contains(ni, nj) {
        if board.used_nodes[ni].hasbit(nj) {
            return (ni, nj);
        }
        ni += di;
        nj += dj;
    }

    (!0, !0)
}


fn get_index(i: usize, j: usize) -> usize {
    (i << B_BOARD_SIZE) | j
}


/// 全ての作成可能な長方形を列挙する
fn enumerate_rectangles(board: &Board, input: &Input) -> Vec<Rectangle> {
    // optick::event!();

    let mut rects = Vec::new();
    let mut first_node = [(!0, !0); 8];

    for &(i, j) in &board.nodes {
        // 各方向で最初に見つかる頂点
        first_node = [(!0, !0); 8];
        for dir in 0..8 {
            if board.is_edge[get_index(i, j)].hasbit(dir) {
                continue;
            }
            first_node[dir] = get_first_node(board, input, (i, j), dir);
        }

        // 長方形を列挙
        for &d_base in &[0, 4] {
            for d1 in d_base..d_base + 4 {
                let d2 = d_base + (d1 + 1) % 4;
                let pos1 = first_node[d1];
                let pos2 = first_node[d2];
                if pos1 == (!0, !0) || pos2 == (!0, !0) {
                    continue;
                }

                let vec1 = (pos1.0 - i, pos1.1 - j);
                let vec2 = (pos2.0 - i, pos2.1 - j);

                let pos3 = (i + vec1.0 + vec2.0, j + vec1.1 + vec2.1);
                if !input.contains(pos3.0, pos3.1) {
                    continue;
                }

                // 制約を満たすか確認
                let rect = Rectangle::new(pos3, pos2, (i, j), pos1);
                if check_valid_rectangle_dir_fixed(board, &rect, input) {
                    rects.push(rect);
                }
            }
        }
    }

    rects
}



/// (i, j)を使う長方形を列挙する
fn enumerate_rectangles_single(board: &Board, input: &Input, 
    pos: (usize, usize), rects: &mut Vec<Rectangle>) -> () {
    // optick::event!();

    let (i, j) = pos;
    
    // 各方向で最初に見つかる頂点
    let mut first_node = [(!0, !0); 8];
    for dir in 0..8 {
        if board.is_edge[get_index(i, j)].hasbit(dir) {
            continue;
        }
        first_node[dir] = get_first_node(board, input, (i, j), dir);
    }

    // 長方形を列挙(2方向でぶつかるパターン)
    for &d_base in &[0, 4] {
        for d1 in d_base..d_base + 4 {
            let d2 = d_base + (d1 + 1) % 4;
            let pos1 = first_node[d1];
            let pos2 = first_node[d2];
            if pos1 == (!0, !0) || pos2 == (!0, !0) {
                continue;
            }

            let vec1 = (pos1.0 - i, pos1.1 - j);
            let vec2 = (pos2.0 - i, pos2.1 - j);
            let pos3 = (i + vec1.0 + vec2.0, j + vec1.1 + vec2.1);
            if !input.contains(pos3.0, pos3.1) {
                continue;
            }

            // 制約を満たすか確認
            let rect = Rectangle::new(pos3, pos2, (i, j), pos1);
            if check_valid_rectangle_dir_fixed(board, &rect, input) {
                rects.push(rect);
            }
        }
    }

    // dir1でだけぶつかるパターン    
    for &d_base in &[0, 4] {
        for &bias in &[1, 3] {
            for d1 in d_base..d_base + 4 {
                let d2 = d_base + (d1 + bias) % 4;
                let pos1 = first_node[d1];
                if pos1 == (!0, !0) {
                    continue;
                }

                // pos1 から d2 の方向に探索する
                let pos3 = get_first_node(board, input, pos1, d2);
                if pos3 == (!0, !0) {
                    continue;
                }

                let vec13 = (pos3.0 - pos1.0, pos3.1 - pos1.1);

                let pos2 = (i + vec13.0, j + vec13.1);
                if !input.contains(pos2.0, pos2.1) {
                    continue;
                }

                // 制約を満たすか確認
                let rect = Rectangle::new(pos2, (i, j), pos1, pos3);
                if check_valid_rectangle_dir_fixed(board, &rect, input) {
                    rects.push(rect);
                }
            }
        }
    }



}


fn compute_rect_value(board: &mut Board, rect: &Rectangle, input: &Input) -> (i32, i32, i32) {
    // optick::event!();

    // 次数は使い切る
    let mut sum_degree = 0;
    for &(pi, pj) in &rect.pos {
        sum_degree += board.n_used[get_index(pi, pj)] as i32;
    }

    let score = input.weight[rect.pos[0].0][rect.pos[0].1];

    let value = (sum_degree, score, score);
    value
}



fn compute_rect_value_old(board: &mut Board, rect: &Rectangle, _: &Input) -> (i32, i32, i32) {
    // optick::event!();

    // 使う格子点の個数が小さい方が良い
    let mut sum_len = 0;
    if rect.pos[0].0 == rect.pos[1].0 || rect.pos[0].1 == rect.pos[1].1 {
        // 平行
        sum_len += rect.pos[0].0.abs_diff(rect.pos[2].0) as i32;
        sum_len += rect.pos[0].1.abs_diff(rect.pos[2].1) as i32;
    }
    else {
        // 回転
        sum_len += rect.pos[0].0.abs_diff(rect.pos[1].0) as i32;
        sum_len += rect.pos[0].0.abs_diff(rect.pos[3].0) as i32;
    }

    // 次数は使い切る
    let mut sum_degree = 0;
    for &(pi, pj) in &rect.pos {
        sum_degree += board.n_used[get_index(pi, pj)] as i32;
    }

    let value = (-sum_len, sum_degree, sum_degree);
    value
}



fn check_valid_rectangle_dir_fixed(board: &Board, rect: &Rectangle, input: &Input) -> bool {

    // pos[0]が空白でないといけない
    if board.used_nodes[rect.pos[0].0].hasbit(rect.pos[0].1) {
        return false;
    }

    // 辺が使用可能でないといけない    
    for k in 0..4 {
        // from -> to をチェックする
        let from = rect.pos[k];
        let to = rect.pos[(k + 1) % 4];
        let dir = get_dir(from, to);

        // 使用済みの辺があってはいけない
        if board.is_edge[get_index(from.0, from.1)] & (1 << dir) > 0 {
            return false;
        }
    }

    // pos[0] -> pos[1], pos[3] の方向に進んで最初に出会わないといけない
    for &to_idx in &[1, 3] {
        let from = rect.pos[0];
        let to = rect.pos[to_idx];
        let dir = get_dir(from, to);

        if get_first_node(board, input, from, dir) != to {
            return false;
        }
    }

    true
}


/// (i, j)を左下として、i方向にlen_i, j方向にlen_jの大きさの長方形が作成可能か
/// 作成可能なら、その長方形の頂点の順序も返す
fn check_valid_rectangle(board: &Board, rect: &Rectangle) -> bool {
    // optick::event!();

    // pos[0]が空白でないといけない
    if board.used_nodes[rect.pos[0].0].hasbit(rect.pos[0].1) {
        return false;
    }

    // 辺が使用可能か
    if !check_unused_nodes_and_edges(board, &rect.pos) {
        return false;
    }

    true
}


/// 長方形を追加する
fn add_rectangle(board: &mut Board, rect: &Rectangle) -> () {
    // optick::event!();

    // 辺を更新
    for &dk in &[1, 3] {
        for k in 0..4 {
            // from -> to をチェックする
            let mut from = rect.pos[k];
            let to = rect.pos[(k + dk) % 4];
            let dir = get_dir(from, to);
            let (di, dj) = DXY9[dir];
    
            while from != to {
                board.is_edge[get_index(from.0, from.1)] |= 1 << dir;    
                from = (from.0 + di, from.1 + dj);
            }
        }
    }


    // 頂点を使用済みにする        
    for &(i, j) in &rect.pos {
        board.used_nodes[i] |= 1 << j;
        board.n_used[get_index(i, j)] += 1;
    }    

    board.nodes.push(rect.pos[0]);
    board.ans.push(rect.clone());
}


/// 4点のうち、空白が1つだけかを確認する。空白が1つだけなら、作れる長方形を返す
fn check_empty_count(board: &Board, positions: &[(usize, usize); 4]) -> Option<Rectangle> {
    // optick::event!();
    
    let mut empty_idx = !0;
    for (k, &(si, sj)) in positions.iter().enumerate() {
        if !board.used_nodes[si].hasbit(sj) {
            if empty_idx != !0 {
                return None;
            }
            empty_idx = k;
        }
    }
    if empty_idx == !0 {
        return None;
    }

    let rect = Rectangle::new(
        positions[empty_idx], positions[(empty_idx + 1) % 4], 
        positions[(empty_idx + 2) % 4], positions[(empty_idx + 3) % 4]);
    return Some(rect);
}


/// 辺が使用可能かを判定する
fn check_unused_nodes_and_edges(board: &Board, positions: &[(usize, usize); 4]) -> bool {
    // optick::event!();

    for k in 0..4 {
        // from -> to をチェックする
        let mut from = positions[k];
        let to = positions[(k + 1) % 4];
        let dir = get_dir(from, to);
        let (di, dj) = DXY9[dir];

        // 使用済みの辺があってはいけない
        if board.is_edge[get_index(from.0, from.1)] & (1 << dir) > 0 {
            return false;
        }

        while from != to {
            let next = (from.0 + di, from.1 + dj);

            // 頂点があってはいけない
            if to != next && board.used_nodes[next.0].hasbit(next.1) {
                return false;
            }

            from = next;
        }
    }    
    true
}





#[derive(Clone, Debug, Hash, cmp::Eq, cmp::PartialEq)]
struct Rectangle {
    pos: [(usize, usize); 4],
}
impl Rectangle {
    fn new(p1: (usize, usize), p2: (usize, usize), p3: (usize, usize), p4: (usize, usize)) -> Self {
        Rectangle { pos: [p1, p2, p3, p4] }
    }
}


fn compute_score(output: &Vec<Rectangle>, input: &Input) -> i64 {
    // optick::event!();
    
    let mut score = 0;
    for rect in output {
        score += input.weight[rect.pos[0].0][rect.pos[0].1];
    }
    for &(i, j) in &input.XY {
        score += input.weight[i][j];
    }

    let score = (1e6 * (input.N * input.N) as f64 / input.XY.len() as f64 * score as f64 / input.den as f64).round() as i64;	
    score
}


fn weight((x, y): (usize, usize), N: usize) -> i32 {
    // optick::event!();

	let dx = x as i32 - N as i32 / 2;
	let dy = y as i32 - N as i32 / 2;
	dx * dx + dy * dy + 1
}


/// 入力を受け取る
fn get_input(input: &str) -> Input {
    if IS_LOCAL {
        input! {
            from AutoSource::from(input),
            N: usize, M: usize,
            XY: [(usize, usize); M],
        }
        let mut w = vec![vec![0; N]; N];
        let mut den = 0;
        for i in 0..N {
            for j in 0..N {
                w[i][j] = weight((i, j), N);
                den += w[i][j];
            }
        }
        Input { N, M, XY, weight: w, den }
    } 
    else {
        input! {
            N: usize, M: usize,
            XY: [(usize, usize); M],
        }
        let mut w = vec![vec![0; N]; N];
        let mut den = 0;
        for i in 0..N {
            for j in 0..N {
                w[i][j] = weight((i, j), N);
                den += w[i][j];
            }
        }
        Input { N, M, XY, weight: w, den }
    }
}

fn print_ans(ans: &Vec<Rectangle>) -> () {
    println!("{}", ans.len());
    for rect in ans {
        for &(x, y) in &rect.pos {
            print!("{} {} ", x, y);
        }
        println!();
    }

}



#[derive(Clone, Debug)]
struct Input {
    N: usize,
    M: usize,
    XY: Vec<(usize, usize)>,
    weight: Vec<Vec<i32>>,
    den: i32,
}
impl std::fmt::Display for Input {
	fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
		writeln!(f, "{} {}", self.N, self.XY.len())?;
		for p in &self.XY {
			writeln!(f, "{} {}", p.0, p.1)?;
		}
		Ok(())
	}
}
impl Input {
    fn contains(&self, i: usize, j: usize) -> bool {
        return i < self.N && j < self.N
    }
}




// =========================================================
//   My Library (Heuristic)
// =========================================================

/// タイマー
struct Timer {
    time_limit: f64,
    n_iter: usize,
    clock_freq: usize,
    start_clock: Instant, 
    elapsed_rate: f64,
}
impl Timer {
    fn new(time_limit: f64, clock_freq: usize) -> Timer {
        Timer {
            time_limit, n_iter: 0, 
            clock_freq, 
            start_clock: Instant::now(),  
            elapsed_rate: 0.,
        }
    }

    fn update_elapsed_rate(&mut self) -> () {
        // // optick::event!();
        let now_clock = Instant::now();
        let elapsed_sec = now_clock.duration_since(self.start_clock).as_secs_f64();
        self.elapsed_rate = elapsed_sec / self.time_limit;
    }

    fn get_elapsed_rate(&mut self) -> f64 {
        // // optick::event!();
        self.n_iter += 1;
        if self.n_iter % self.clock_freq == 0 {
            self.update_elapsed_rate();
        }
        self.elapsed_rate
    }

    fn timeover(&mut self) -> bool {
        // // optick::event!();
        return self.get_elapsed_rate() > 1. 
    }
}

/// 焼きなまし
struct SimulatedAnnealing {
    T0: f64, 
    T1: f64,
    freq: usize,
    timer: Timer,
    last_temp: f64,
    n_iter: usize,
    rng: Pcg64Mcg,
}
impl SimulatedAnnealing {
    fn new(time_limit: f64, freq: usize, 
           T0: f64, T1: f64, rng: Pcg64Mcg) -> SimulatedAnnealing {
        let timer = Timer::new(time_limit, 1);
        SimulatedAnnealing {
            T0, T1, freq, timer, 
            last_temp: 1., n_iter: 0,
            rng,
        }
    }

    fn accept(&mut self, d_worse: f64) -> bool {
        self.n_iter += 1;
        if d_worse < 0. {
            true
        }
        else {
            let r = self.rng.gen::<f64>();
            let p_accept = (-d_worse / self.get_temperature()).exp();
            r < p_accept
        }
    }

    fn update_temperature(&mut self) -> () {
        let progress = self.timer.get_elapsed_rate();
        let remain = 1. - progress;
        self.last_temp = self.T0.powf(remain) * self.T1.powf(progress) + 1e-10 as f64;
    }

    fn get_temperature(&mut self) -> f64 {
        if self.n_iter % self.freq == 0 {
            self.update_temperature();
        }
        self.last_temp
    }
}




/// 指数分布
fn getrand_exp(a: usize, b: usize, rng: &mut Mcg128Xsl64) -> usize {
    let base = rng.gen_range(0, b - a + 1) as f64;
    base.powf(rng.gen()) as usize + a - 1
}


/// ビームサーチのノード
#[derive(Clone)]
struct BeamNode {
    score: i64,
    value: i64,
}
impl Ord for BeamNode {
    fn cmp(&self, other: &Self) -> Ordering {
        self.value.cmp(&other.value)
    }
}
impl PartialOrd for BeamNode {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.value.cmp(&other.value))
    }
}
impl PartialEq for BeamNode {
    fn eq(&self, other: &Self) -> bool {
        self.value == other.value
    }
}
impl Eq for BeamNode { }


/// ビームサーチの状態管理
#[derive(Clone)]
struct BeamStack {
    queu: BinaryHeap<BeamNode>,
    max_size: usize,
    dict: HashSet<String>,
}
impl BeamStack {
    fn new(max_size: usize) -> BeamStack {
        BeamStack { queu: BinaryHeap::new(), max_size: max_size, dict: HashSet::new() }
    }

    fn push(&mut self, state: BeamNode) {
        self.queu.push(state);
    }

    fn pop(&mut self) -> Option<BeamNode> {
        self.queu.pop()
    }

    fn peek(&self) -> Option<&BeamNode> {
        self.queu.peek()
    }

    fn len(&self) -> usize {
        self.queu.len()
    }

    fn is_empty(&self) -> bool {
        self.queu.is_empty()
    }
}





// =========================================================
//   My Library (Algorigthm)
// =========================================================


/*
 整数型に付け加えるメソッド
*/
pub trait MyInteger<T> {
    /// modを取った値に上書きする
    fn chmod(&mut self) -> T;

    /// 整数を文字列にしたときの長さ
    fn len(&self) -> usize;

    /// x in [a, b)?
    fn isin(&self, a: T, b: T) -> bool;

    /// bitlistに変換<br>
    /// 計算量 O(logK), K: 元の整数 <br>
    /// <br>
    /// TODO: コンストラクタのオーバーヘッドが大きい可能性がある。vectorは使いまわせるようにする。
    fn bitlist(&self, N: usize) -> Vec<bool>; 

    /// 繰り返し二乗法 <br>
    /// 計算量 O(logP)
    fn repeat_squaring(&self, P: T, use_mod: bool) -> T;

    /// 約数列挙 <br>
    /// 計算量 O(√N)
    fn divisors(&self) -> Vec<T>;

    /// 素数判定 <br>
    /// 計算量 O(√N)
    fn is_primenumber(&self) -> bool;

    /// 素因数分解 <br>
    /// 計算量 O(√N)
    fn prime_factors(&self) -> Vec<(T, i32)>;

    /// 階乗 <br>
    /// 計算量 O(N)
    fn factorial(&self, use_mod: bool) -> T;

    /// 約分 <br>
    /// 計算量 O(logN)
    fn reduce(&self, b: T) -> (T, T);

    /// 二項係数(愚直) <br>
    /// 計算量 O(m + logMOD)
    fn comb(&self, m: T, use_mod: bool) -> T;

    /// N以下の整数について最大の素数（エラトステネスの篩）<br>
    /// vec[i] == i なら素数 <br>
    /// 計算量 O(N loglogN)
    fn eratosthenes(&self) -> Vec<usize>;

}
macro_rules! impl_MyInteger {
    ( $($T:ty),* ) => ($(

        // TODO: マイナスの値が入ってたらpanicさせる。
        impl MyInteger<$T> for $T {            
            fn chmod(&mut self) -> $T {
                // mod計算は一瞬だけ型を大きくしておいてオーバーフローを回避する
                *self = (((*self as i128) % MODULO + MODULO) % MODULO) as $T;
                return *self
            }

            fn len(&self) -> usize { 
                return self.to_string().len(); 
            }   

            fn isin(&self, a: $T, b: $T) -> bool {
                return a <= *self && *self < b;
            }

            fn bitlist(&self, N: usize) -> Vec<bool> {    
                let mut x = *self; 
                let mut bits = Vec::new();
                while x > 0 { 
                    bits.push(x % 2 == 1); 
                    x /= 2; 
                }
                if N > 0 { 
                    for _ in 0..N - bits.len() { 
                        bits.push(false); 
                    } 
                }
                return bits;
            }

            fn repeat_squaring(&self, P: $T, use_mod: bool) -> $T {
                if (P == 0) {
                    return 1;
                }
                else if (P % 2 == 0) { 
                    if use_mod {
                        let t = self.repeat_squaring(P / 2, use_mod).chmod() as i128;
                        return (t * t).chmod() as $T;
                    }
                    else {
                        let t = self.repeat_squaring(P / 2, use_mod);
                        return t * t;
                    }
                }
                else {
                    if use_mod {
                        return (*self as i128 * self.repeat_squaring(P - 1, use_mod) as i128).chmod() as $T;
                    }
                    else {
                        return *self * self.repeat_squaring(P - 1, use_mod);
                    }
                }
            }

            fn divisors(&self) -> Vec<$T> {
                let mut divs = Vec::new();
                for i in 1..=*self { 
                    if i * i > *self {
                        break;
                    }
                    if *self % i == 0 { 
                        divs.push(i);	
                        if i * i != *self {
                            divs.push(*self / i); 
                        }
                    } 
                }
                divs.sort();
                return divs;
            }

            fn is_primenumber(&self) -> bool {
                let n = *self;
                for i in 2..=n {
                    if i * i > n {
                        break;
                    }
                    if n % i == 0 {
                        return true;
                    }
                }
                return false;
            }

            fn prime_factors(&self) -> Vec<($T, i32)> {
                let mut factors = Vec::new();
                let mut n = *self;
                for i in 2..=n {
                    if i * i > n {
                        break;
                    }
                    let mut count = 0; 
                    while n % i == 0 { 
                        count += 1; 
                        n /= i; 
                    }
                    if count > 0 { 	
                        factors.push((i, count)); 
                    }
                }
                if (n != 1) {
                    factors.push((n, 1)); 
                }
                return factors;
            }

            fn factorial(&self, use_mod: bool) -> $T {
                if *self <= 1 { 
                    return 1; 
                }
                return match use_mod {
                    true  => (*self * (*self - 1).factorial(use_mod)).chmod(),
                    false => *self * (*self - 1).factorial(use_mod),
                }
            }
            
            fn reduce(&self, b: $T) -> ($T, $T) {
                let gcd = self.gcd(&b);
                return (*self / gcd, b / gcd);
            }

            fn comb(&self, m: $T, use_mod: bool) -> $T {
                let a = *self as i128;
                let b = m as i128;

                let mut bunshi = 1;	
                let mut bunbo = 1;
                for i in 1..=b { 
                    bunbo *= i;	
                    bunshi *= (a - i + 1); 
                    if use_mod {
                        bunbo.chmod(); 
                        bunshi.chmod(); 
                    }
                }
                if use_mod {
                    let mut ret = bunshi * bunbo.repeat_squaring(MODULO - 2, use_mod); 
                    return ret.chmod() as $T;
                }                
                else {
                    return (bunshi / bunbo) as $T;
                }
            }
        
            fn eratosthenes(&self) -> Vec<usize> {
                let n = *self as usize;
                let mut primes = vec![0; n+1];
                for i in 0..=n {
                    primes[i] = i;
                }
                for i in 2..=n { 
                    if primes[i] == i { 
                        for j in 2.. {
                            if i * j > n {
                                break;
                            }
                            primes[i * j] = i;
                        }
                    }
                }
                return primes;
            }
        }

    )*)
}
impl_MyInteger!(i32, i64, i128, usize);


pub trait AbsDiff {
    fn abs_diff(&self, other: usize) -> usize;
    fn hasbit(&self, idx: usize) -> bool;
}
impl AbsDiff for usize {
    fn abs_diff(&self, other: usize) -> usize {
        (self - other).min(other - self)
    }

    fn hasbit(&self, idx: usize) -> bool {
        ((self >> idx) & 1) == 1
    }
}


/*
 Vectorに付け加えるメソッド
*/
pub trait MyVector<T> {
    /// 総和・最大値・最小値
    fn vsum(&self) -> T;
    fn vmax(&self) -> T;
    fn vmin(&self) -> T;
    
    /// 累積和
    fn accumulate(&self) -> Vec<T>;

    /// 空白区切りの出力
    fn print(&self) -> ();

    /// x以上になる最初のインデックス
    fn lower_bound(&self, x: T) -> usize;

    /// xより大きくなる最初のインデックス
    fn upper_bound(&self, x: T) -> usize;

    /// argsort
    fn argsort(&self) -> Vec<usize>;
    fn argsort_rev(&self) -> Vec<usize>;
}
macro_rules! impl_MyVector {
    ( $($T:ty),* ) => ($(

        // TODO: マイナスの値が入ってたらpanicさせる。
        impl MyVector<$T> for Vec<$T>{
            fn vsum(&self) -> $T {
                return self.iter().sum();
            }
            
            fn vmax(&self) -> $T {
                return *self.iter().max().unwrap();
            }
            
            fn vmin(&self) -> $T {
                return *self.iter().min().unwrap();
            }

            fn accumulate(&self) -> Vec<$T> {
                let mut accum = self.clone();
                for i in 1..accum.len() {
                    accum[i] += accum[i - 1];
                }
                return accum;
            }

            fn print(&self) {
                for v in self {
                    print!("{} ", v);
                }
                println!();
            }

            fn lower_bound(&self, x: $T) -> usize {
                let mut ok = self.len() as i64;
                let mut ng = -1;
                while ok - ng > 1 {
                    let mid = (ok + ng) / 2;
                    if self[mid as usize] >= x {
                        ok = mid;
                    }
                    else {
                        ng = mid;
                    }
                }
                return ok as usize;
            }

            fn upper_bound(&self, x: $T) -> usize {
                let mut ok = self.len() as i64;
                let mut ng = -1;
                while ok - ng > 1 {
                    let mid = (ok + ng) / 2;
                    if self[mid as usize] > x {
                        ok = mid;
                    }
                    else {
                        ng = mid;
                    }
                }
                return ok as usize;
            }
            
            fn argsort(&self) -> Vec<usize> {
                let mut idx = (0..self.len()).collect::<Vec<_>>();
                idx.sort_by(|&i, &j| self[i].cmp(&self[j]));
                idx
            }
            
            fn argsort_rev(&self) -> Vec<usize> {
                let mut idx = (0..self.len()).collect::<Vec<_>>();
                idx.sort_by(|&i, &j| self[j].cmp(&self[i]));
                idx
            }
        }

    )*)
}
impl_MyVector!(i32, i64, i128, usize);




/// from -> to の方向のインデックス
fn get_dir(from: (usize, usize), to: (usize, usize)) -> usize {
    // // optick::event!();
    match (from.0.cmp(&to.0), from.1.cmp(&to.1)) {
        (Ordering::Equal, Ordering::Less) => 0,
        (Ordering::Less, Ordering::Equal) => 1,
        (Ordering::Equal, Ordering::Greater) => 2,
        (Ordering::Greater, Ordering::Equal) => 3,
        
        (Ordering::Less, Ordering::Less) => 4,        
        (Ordering::Less, Ordering::Greater) => 5,   
        (Ordering::Greater, Ordering::Greater) => 6,       
        (Ordering::Greater, Ordering::Less) => 7,      
        
        (Ordering::Equal, Ordering::Equal) => 8,
    }
}




/// UnionFind
#[derive(Clone, Debug)]
struct UnionFind {
    _siz: Vec<usize>, // その集合のサイズ
    n_group: usize, // 素集合の個数
    par: Vec<usize>, // 各元の親
}
impl UnionFind {
    fn new(siz: usize) -> UnionFind {
        let mut par = vec![0; siz];
        for i in 0..siz {
            par[i] = i;
        }
        UnionFind{
            _siz: vec![1_usize; siz],
            n_group: siz, 
            par: par,
        }
    }

    /// 親のインデックス
    fn find(&mut self, mut x: usize) -> usize {
        while self.par[x] != x {
            self.par[x] = self.par[self.par[x]];
            x = self.par[x];
        }
        return x;
    }

    /// Union
    fn union(&mut self, mut x: usize, mut y: usize) -> () {
        x = self.find(x);
        y = self.find(y);
        if x == y {
            return;
        }
        if self._siz[x] < self._siz[y] {
            mem::swap(&mut x, &mut y);
        }
        self._siz[x] += self._siz[y];
        self.par[y] = x;
        self.n_group -= 1;
    }

    /// 連結判定
    fn issame(&mut self, x: usize, y: usize) -> bool {
        return self.find(x) == self.find(y);
    }

    /// xを含む集合の大きさ
    fn size(&mut self, x: usize) -> usize {
        let r = self.find(x);
        return self._siz[r];
    }
}
