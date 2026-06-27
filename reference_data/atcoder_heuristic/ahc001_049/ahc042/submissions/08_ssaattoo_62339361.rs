#![allow(dead_code)]
#![allow(non_upper_case_globals)]
#![allow(unused_imports)]
use crate::{
    data_structures::{
        grid::{Coord, Map2d},
        *,
    },
    problem::*,
    utils::*,
};
use itertools::{iproduct, Itertools};
use proconio::{marker::*, *};
use std::{cmp::*, collections::*};

const TIME_LIMIT: f64 = 1.97 * 1.0;

pub fn main() {
    get_time();
    let input = read_input();
    let output = solve(&input);
    //let output = greedy(&input);
    output.print_ans();
    let time = get_time();
    crate::printdata!(time);
    eprintln!("Time = {}", get_time());
}

fn solve(input: &Input) -> Output {
    for _ in 0..100 {
        eprintln!("{}", rnd::get(100000));
    }
    let beam_width = 210;
    let mut beam = vec![State::new(input)];
    let mut best_moves = vec![];
    let mut best_score = usize::MAX;
    let n = input.n;

    // 鬼が残っている限り繰り返す
    let mut iter = 0;
    while !beam[0].pos_oni.is_empty() {
        iter += 1;
        if iter == 200 {
            //break;
        }
        if best_moves.len() == input.n * input.n * 4 || get_time() > TIME_LIMIT {
            //break;
            return greedy(input);
        }

        let mut next_beam = vec![];

        // 現在のビームの各状態に対して
        for state in beam.iter() {
            // 各方向と位置の組み合わせを試す
            for dir in 0..4 {
                for pos in 0..n {
                    // 空の行/列はスキップ
                    //if state.is_empty_line(input, dir, pos) {
                    if state.is_remove_fuku(input, dir, pos) {
                        continue;
                    }
                    if !state.is_exist_oni_line(input, dir, pos) {
                        continue;
                    }
                    // 福を消す操作はスキップ

                    // 操作を試す
                    let mut next_state = state.clone();
                    next_state.move_line(input, dir, pos);

                    // 操作後の評価値を計算
                    let score: usize = next_state
                        .pos_oni
                        .iter()
                        .enumerate()
                        .map(|(i, _)| {
                            let (eval, _) = next_state.get_nearest_edge_3(input, i);
                            eval
                        })
                        .sum();

                    // 移動履歴を更新
                    let mut moves = state.moves.clone();
                    let dir_char = match dir {
                        0 => 'L',
                        1 => 'U',
                        2 => 'R',
                        3 => 'D',
                        _ => unreachable!(),
                    };
                    moves.push((dir_char, pos));
                    next_state.moves = moves;

                    next_beam.push((next_state, score));
                }
            }
        }

        // スコアでソートし、上位beam_width個を選択
        next_beam.sort_by_key(|(_, score)| *score);
        // 最良の解を更新
        if let Some((state, score)) = next_beam.first() {
            if score < &best_score && state.pos_oni.is_empty() {
                best_score = *score;
                best_moves = state.moves.clone();
            }
        }
        let mut set = HashSet::new();
        //next_beam.retain(|s| set.insert(s.0.calc_oni_hash(input)));
        //next_beam.truncate(beam_width);
        beam = next_beam
            .into_iter()
            .filter(|s| set.insert(s.0.calc_oni_hash(input)))
            .take(beam_width)
            .map(|t| t.0)
            .collect_vec();

        // 次のビームを更新
        //beam = next_beam.into_iter().map(|(state, _)| state).collect_vec();

        // ビームが空になった場合は終了
        if beam.is_empty() {
            break;
        }
    }

    let score = best_moves.len();
    Output {
        score,
        moves: best_moves,
    }
}

#[derive(Clone)]
struct State {
    map: Map2d<Cell>,
    pos_oni: Vec<Coord>,
    pos_fuku: Vec<Coord>,
    moves: Vec<(char, usize)>, // 移動履歴を追加
}
impl State {
    pub fn new(input: &Input) -> Self {
        Self {
            map: input.map.clone(),
            pos_oni: input.pos_oni.clone(),
            pos_fuku: input.pos_fuku.clone(),
            moves: vec![], // 移動履歴を初期化
        }
    }

    /// 指定された鬼について、最も近い端までの方向と距離を返す
    /// 福がある場合はペナルティとして距離を増加させる
    pub fn get_nearest_edge_2(&self, input: &Input, oni_id: usize) -> (usize, usize) {
        let p = self.pos_oni[oni_id];
        let n = input.n;

        // 各方向の福の数をカウント
        let fuku_counts = [
            // 左方向の福の数
            (0..p.y)
                .filter(|&y| self.map[Coord::new(p.x, y)] == Cell::Fuku)
                .count(),
            // 上方向の福の数
            (0..p.x)
                .filter(|&x| self.map[Coord::new(x, p.y)] == Cell::Fuku)
                .count(),
            // 右方向の福の数
            ((p.y + 1)..n)
                .filter(|&y| self.map[Coord::new(p.x, y)] == Cell::Fuku)
                .count(),
            // 下方向の福の数
            ((p.x + 1)..n)
                .filter(|&x| self.map[Coord::new(x, p.y)] == Cell::Fuku)
                .count(),
        ];

        // 各方向への距離を計算（福の数 * n を加算してペナルティとする）
        let distances = [
            (p.y + 1 + fuku_counts[0] * 1, 0), // 左端までの距離
            (p.x + 1 + fuku_counts[1] * 1, 1), // 上端までの距離
            (n - p.y + fuku_counts[2] * 1, 2), // 右端までの距離
            (n - p.x + fuku_counts[3] * 1, 3), // 下端までの距離
        ];

        // 最も評価値が小さい方向を選択
        distances.into_iter().min_by_key(|&(dist, _)| dist).unwrap()
    }

    pub fn get_nearest_edge_3(&self, input: &Input, oni_id: usize) -> (usize, usize) {
        let p = self.pos_oni[oni_id];
        let n = input.n;

        // 各方向の福の数をカウント
        let fuku_counts = [
            // 左方向の福の数
            (0..p.y)
                .filter(|&y| self.map[Coord::new(p.x, y)] == Cell::Fuku)
                .count(),
            // 上方向の福の数
            (0..p.x)
                .filter(|&x| self.map[Coord::new(x, p.y)] == Cell::Fuku)
                .count(),
            // 右方向の福の数
            ((p.y + 1)..n)
                .filter(|&y| self.map[Coord::new(p.x, y)] == Cell::Fuku)
                .count(),
            // 下方向の福の数
            ((p.x + 1)..n)
                .filter(|&x| self.map[Coord::new(x, p.y)] == Cell::Fuku)
                .count(),
        ];
        let oni_counts = [
            // 左方向の福の数
            (0..p.y)
                .filter(|&y| self.map[Coord::new(p.x, y)] == Cell::Oni)
                .count(),
            // 上方向の福の数
            (0..p.x)
                .filter(|&x| self.map[Coord::new(x, p.y)] == Cell::Oni)
                .count(),
            // 右方向の福の数
            ((p.y + 1)..n)
                .filter(|&y| self.map[Coord::new(p.x, y)] == Cell::Oni)
                .count(),
            // 下方向の福の数
            ((p.x + 1)..n)
                .filter(|&x| self.map[Coord::new(x, p.y)] == Cell::Oni)
                .count(),
        ];

        // 各方向への距離を計算（福の数 * n を加算してペナルティとする）
        let distances = [
            ((p.y + 1 + 2) * 1 + fuku_counts[0] * 1 - oni_counts[0], 0), // 左端までの距離
            ((p.x + 1 + 2) * 1 + fuku_counts[1] * 1 - oni_counts[1], 1), // 上端までの距離
            ((n - p.y + 2) * 1 + fuku_counts[2] * 1 - oni_counts[2], 2), // 右端までの距離
            ((n - p.x + 2) * 1 + fuku_counts[3] * 1 - oni_counts[3], 3), // 下端までの距離
        ];

        // 最も評価値が小さい方向を選択
        distances.into_iter().min_by_key(|&(dist, _)| dist).unwrap()
    }

    /// 指定された方向に動かした時に福が消えるかどうかをチェック
    pub fn is_remove_fuku(&self, input: &Input, dir: usize, pos: usize) -> bool {
        match dir {
            0 => self.map[Coord::new(pos, 0)] == Cell::Fuku, // 左方向
            1 => self.map[Coord::new(0, pos)] == Cell::Fuku, // 上方向
            2 => self.map[Coord::new(pos, input.n - 1)] == Cell::Fuku, // 右方向
            3 => self.map[Coord::new(input.n - 1, pos)] == Cell::Fuku, // 下方向
            _ => unreachable!(),
        }
    }

    /// 指定された方向に行/列を移動する
    /// dir: 0=左, 1=上, 2=右, 3=下
    /// pos: 移動する行/列の番号
    pub fn move_line(&mut self, input: &Input, dir: usize, pos: usize) {
        let n = input.n;
        match dir {
            0 => {
                // 左方向
                for y in 0..n - 1 {
                    self.map[Coord::new(pos, y)] = self.map[Coord::new(pos, y + 1)];
                }
                self.map[Coord::new(pos, n - 1)] = Cell::Empty;
            }
            1 => {
                // 上方向
                for x in 0..n - 1 {
                    self.map[Coord::new(x, pos)] = self.map[Coord::new(x + 1, pos)];
                }
                self.map[Coord::new(n - 1, pos)] = Cell::Empty;
            }
            2 => {
                // 右方向
                for y in (1..n).rev() {
                    self.map[Coord::new(pos, y)] = self.map[Coord::new(pos, y - 1)];
                }
                self.map[Coord::new(pos, 0)] = Cell::Empty;
            }
            3 => {
                // 下方向
                for x in (1..n).rev() {
                    self.map[Coord::new(x, pos)] = self.map[Coord::new(x - 1, pos)];
                }
                self.map[Coord::new(0, pos)] = Cell::Empty;
            }
            _ => unreachable!(),
        }

        // 鬼と福の位置を更新
        self.update_positions();
    }

    /// 鬼と福の位置情報を更新
    fn update_positions(&mut self) {
        self.pos_oni.clear();
        self.pos_fuku.clear();

        for i in 0..self.map.height {
            for j in 0..self.map.width {
                let p = Coord::new(i, j);
                match self.map[p] {
                    Cell::Oni => self.pos_oni.push(p),
                    Cell::Fuku => self.pos_fuku.push(p),
                    Cell::Empty => {}
                }
            }
        }
    }

    /// 指定された行/列が空かどうかをチェック
    pub fn is_empty_line(&self, input: &Input, dir: usize, pos: usize) -> bool {
        let n = input.n;
        match dir {
            0 | 2 => {
                // 行方向の場合
                (0..n).all(|y| self.map[Coord::new(pos, y)] == Cell::Empty)
            }
            1 | 3 => {
                // 列方向の場合
                (0..n).all(|x| self.map[Coord::new(x, pos)] == Cell::Empty)
            }
            _ => unreachable!(),
        }
    }

    pub fn is_exist_oni_line(&self, input: &Input, dir: usize, pos: usize) -> bool {
        let n = input.n;
        match dir {
            0 | 2 => {
                // 行方向の場合
                (0..n).any(|y| self.map[Coord::new(pos, y)] == Cell::Oni)
            }
            1 | 3 => {
                // 列方向の場合
                (0..n).any(|x| self.map[Coord::new(x, pos)] == Cell::Oni)
            }
            _ => unreachable!(),
        }
    }

    /// 鬼の位置に基づくハッシュ値を計算
    pub fn calc_oni_hash(&self, input: &Input) -> u64 {
        static mut hashv: [[u64; 20]; 20] = [[0; 20]; 20];
        unsafe {
            if hashv[0][0] == 0 {
                for i in 0..input.n {
                    for j in 0..input.n {
                        hashv[i][j] = rnd::next64() as u64;
                    }
                }
            }
            let mut hash = 0u64;

            for &p in &self.pos_oni {
                // 座標を1次元のインデックスに変換
                hash ^= hashv[p.x][p.y];
            }

            hash
        }
    }

    pub fn calc_fuku_hash(&self, input: &Input) -> u64 {
        static mut hashv: [[u64; 20]; 20] = [[0; 20]; 20];
        unsafe {
            if hashv[0][0] == 0 {
                for i in 0..input.n {
                    for j in 0..input.n {
                        hashv[i][j] = rnd::next64() as u64;
                    }
                }
            }
            let mut hash = 0u64;

            for &p in &self.pos_fuku {
                // 座標を1次元のインデックスに変換
                hash ^= hashv[p.x][p.y];
            }

            hash
        }
    }
}

fn greedy(input: &Input) -> Output {
    let mut state = State::new(input);
    let mut moves = vec![];
    let n = input.n;

    // 鬼が残っている限り繰り返す
    while !state.pos_oni.is_empty() {
        if moves.len() == input.n * input.n * 4 {
            break;
        }
        // 全ての可能な操作を試す
        let mut best_score = usize::MAX;
        let mut best_move = None;

        // 各方向と位置の組み合わせを試す
        for dir in 0..4 {
            for pos in 0..n {
                // 空の行/列はスキップ
                if state.is_empty_line(input, dir, pos) {
                    continue;
                }
                // 福を消す操作はスキップ
                if state.is_remove_fuku(input, dir, pos) {
                    continue;
                }

                // 操作を試す
                let mut next_state = state.clone();
                next_state.move_line(input, dir, pos);

                // 操作後の評価値を計算
                let score: usize = next_state
                    .pos_oni
                    .iter()
                    .enumerate()
                    .map(|(i, _)| {
                        let (eval, _) = next_state.get_nearest_edge_2(input, i);
                        eval
                    })
                    .sum();

                // より良い操作が見つかった場合は更新
                if score < best_score {
                    best_score = score;
                    best_move = Some((dir, pos));
                }
            }
        }

        // 最適な操作を実行
        if let Some((dir, pos)) = best_move {
            state.move_line(input, dir, pos);
            // 操作を記録
            let dir_char = match dir {
                0 => 'L',
                1 => 'U',
                2 => 'R',
                3 => 'D',
                _ => unreachable!(),
            };
            moves.push((dir_char, pos));
        } else {
            // 有効な操作が見つからない場合
            eprintln!("No valid move found!");
            break;
        }
    }
    let score = moves.len();
    Output { score, moves }
}

mod data_structures {
    use super::*;

    pub mod grid {
        #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
        pub struct Coord {
            pub x: usize,
            pub y: usize,
        }

        impl Coord {
            pub const fn new(x: usize, y: usize) -> Self {
                Self { x, y }
            }

            pub const fn new2(p: usize, width: usize) -> Self {
                Self {
                    x: p / width,
                    y: p % width,
                }
            }

            pub fn in_map(&self, size: usize) -> bool {
                self.x < size && self.y < size
            }

            pub const fn idx(&self, size: usize) -> usize {
                self.x * size + self.y
            }

            pub fn next(self, dir: usize) -> Self {
                self + DXY[dir]
            }

            pub const fn dist(&self, other: &Self) -> usize {
                Self::dist_1d(self.x, other.x) + Self::dist_1d(self.y, other.y)
            }

            const fn dist_1d(x0: usize, x1: usize) -> usize {
                x0.abs_diff(x1)
            }
        }

        impl std::ops::Add for Coord {
            type Output = Self;

            fn add(self, other: Self) -> Self {
                Self {
                    x: self.x + other.x,
                    y: self.y + other.y,
                }
            }
        }

        pub fn coord_iter(h: usize, w: usize) -> impl Iterator<Item = Coord> {
            (0..h).flat_map(move |x| (0..w).map(move |y| Coord::new(x, y)))
        }

        pub const DXY: [Coord; 4] = [
            Coord::new(0, !0),
            Coord::new(!0, 0),
            Coord::new(0, 1),
            Coord::new(1, 0),
        ];

        pub const DC: [char; 4] = ['L', 'U', 'R', 'D'];

        #[derive(Debug, Clone)]
        pub struct Map2d<T> {
            pub height: usize,
            pub width: usize,
            map: Vec<T>,
        }

        impl<T: Clone> Map2d<T> {
            pub fn new(map: Vec<T>, width: usize) -> Self {
                let height = map.len() / width;
                debug_assert!(width * height == map.len());
                Self { height, width, map }
            }

            pub fn fill(&mut self, value: T) {
                self.map.fill(value);
            }
        }

        impl<T> std::ops::Index<Coord> for Map2d<T> {
            type Output = T;

            #[inline]
            fn index(&self, coord: Coord) -> &Self::Output {
                unsafe { self.map.get_unchecked(coord.x * self.width + coord.y) }
            }
        }

        impl<T> std::ops::IndexMut<Coord> for Map2d<T> {
            #[inline]
            fn index_mut(&mut self, coord: Coord) -> &mut Self::Output {
                unsafe { self.map.get_unchecked_mut(coord.x * self.width + coord.y) }
            }
        }

        impl<T> std::ops::Index<&Coord> for Map2d<T> {
            type Output = T;

            #[inline]
            fn index(&self, coord: &Coord) -> &Self::Output {
                &self.map[coord.x * self.width + coord.y]
            }
        }

        impl<T> std::ops::IndexMut<&Coord> for Map2d<T> {
            #[inline]
            fn index_mut(&mut self, coord: &Coord) -> &mut Self::Output {
                &mut self.map[coord.x * self.width + coord.y]
            }
        }

        impl<T> std::ops::Index<usize> for Map2d<T> {
            type Output = T;

            #[inline]
            fn index(&self, p: usize) -> &Self::Output {
                &self.map[p]
            }
        }

        impl<T> std::ops::IndexMut<usize> for Map2d<T> {
            #[inline]
            fn index_mut(&mut self, p: usize) -> &mut Self::Output {
                &mut self.map[p]
            }
        }
    }
}
mod problem {
    use super::*;

    #[derive(Debug, Clone, Copy, PartialEq, Eq)]
    pub enum Cell {
        Oni,   // 鬼
        Fuku,  // 福
        Empty, // 空マス
    }

    pub struct Input {
        pub n: usize,
        pub map: Map2d<Cell>,
        pub pos_oni: Vec<Coord>,
        pub pos_fuku: Vec<Coord>,
    }

    pub fn read_input() -> Input {
        input! {
            n: usize,
            grid: [Chars; n]
        }

        let map = Map2d::new(
            grid.iter()
                .flat_map(|row| {
                    row.iter().map(|&c| match c {
                        'x' => Cell::Oni,
                        'o' => Cell::Fuku,
                        '.' => Cell::Empty,
                        _ => unreachable!(),
                    })
                })
                .collect_vec(),
            n,
        );

        let mut pos_oni = vec![];
        let mut pos_fuku = vec![];
        for p in grid::coord_iter(n, n) {
            match map[p] {
                Cell::Oni => pos_oni.push(p),
                Cell::Fuku => pos_fuku.push(p),
                Cell::Empty => {}
            }
        }

        Input {
            n,
            map,
            pos_oni,
            pos_fuku,
        }
    }

    #[derive(Debug, Clone)]
    pub struct Output {
        pub score: usize,
        pub moves: Vec<(char, usize)>, // (方向, 行/列番号)のベクトル
    }

    impl Output {
        pub fn print_ans(&self) {
            let score = self.score;
            crate::printdata!(score);
            for &(dir, pos) in &self.moves {
                println!("{} {}", dir, pos);
            }
        }
    }
}

mod utils {
    use super::*;

    #[macro_export]
    macro_rules! printdata {
    ( $($x:expr),* ) => {{
        $(eprintln!("[DATA] {} = {:?}", stringify!($x), $x);)*
    }}
}

    pub mod rnd {
        static mut A: u64 = 1;

        pub fn next() -> u32 {
            unsafe {
                let mut x = A;
                A *= 0xcafef00dd15ea5e5;
                x ^= x >> 22;
                (x >> 22 + (x >> 61)) as u32
            }
        }

        pub fn next64() -> u64 {
            (next() as u64) << 32 | next() as u64
        }

        pub fn nextf() -> f64 {
            unsafe {
                std::mem::transmute::<u64, f64>(0x3ff0000000000000 | (next() as u64) << 20) - 1.
            }
        }

        pub fn get(n: usize) -> usize {
            assert!(n <= u32::MAX as usize);
            next() as usize * n >> 32
        }

        pub fn range(a: usize, b: usize) -> usize {
            assert!(a < b);
            get(b - a) + a
        }

        pub fn range_skip(a: usize, b: usize, skip: usize) -> usize {
            assert!(a <= skip && skip < b);
            let n = range(a, b - 1);
            n + (skip <= n) as usize
        }

        pub fn rangei(a: i64, b: i64) -> i64 {
            assert!(a < b);
            get((b - a) as usize) as i64 + a
        }

        pub fn shuffle<T>(list: &mut [T]) {
            for i in (0..list.len()).rev() {
                list.swap(i, get(i + 1));
            }
        }

        pub fn shuffle_iter<T: Copy>(list: &mut [T]) -> impl Iterator<Item = T> + '_ {
            (0..list.len()).rev().map(|i| {
                list.swap(i, get(i + 1));
                list[i]
            })
        }

        // 平均 mu, 標準偏差 sigma の正規分布に従う乱数を生成する
        pub fn sample(mu: f64, sigma: f64) -> f64 {
            let u1 = nextf();
            let u2 = nextf();
            mu + (-2.0 * u1.ln() * sigma * sigma).sqrt() * (2.0 * 3.14159265 * u2).cos()
        }
    }

    pub trait RandomChoice {
        type Output;
        fn choice(&self) -> Self::Output;
        fn choice_where<F>(&self, condition: F) -> Self::Output
        where
            F: Fn(&Self::Output) -> bool;
        fn choice_idx_where<F>(&self, condition: F) -> usize
        where
            F: Fn(&Self::Output) -> bool;
    }
    impl<T: Copy> RandomChoice for [T] {
        type Output = T;
        fn choice(&self) -> T {
            self[rnd::get(self.len())]
        }

        fn choice_where<F>(&self, condition: F) -> T
        where
            F: Fn(&T) -> bool,
        {
            loop {
                let idx = rnd::get(self.len());
                if condition(&self[idx]) {
                    return self[idx];
                }
            }
        }

        fn choice_idx_where<F>(&self, condition: F) -> usize
        where
            F: Fn(&T) -> bool,
        {
            loop {
                let idx = rnd::get(self.len());
                if condition(&self[idx]) {
                    return idx;
                }
            }
        }
    }

    pub fn get_time() -> f64 {
        static mut START: f64 = -1.0;
        let end = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs_f64();
        unsafe {
            if START < 0.0 {
                START = end;
            }
            end - START
        }
    }

    pub fn normal_cdf(x: f64, mean: f64, std_dev: f64) -> f64 {
        0.5 * (1.0 + libm::erf((x - mean) / (std_dev * 1.41421356237)))
    }

    pub fn round(x: f64, k: usize) -> f64 {
        let b = 10f64.powi(k as i32);
        (x * b).round() / b
    }
}
mod visualizer {
    use super::*;

    #[derive(Debug)]
    pub enum Color {
        Red,
        Blue,
        Green,
        Yellow,
        White,
        Black,
    }

    #[derive(Debug, Clone, Copy)]
    pub struct Visualizer {
        h: usize,
        w: usize,
    }
    impl Visualizer {
        pub fn new(h: usize, w: usize) -> Self {
            eprintln!("@setting {} {}", h, w);
            Self { h, w }
        }
        pub fn init(&self) {
            if !cfg!(feature = "local") {
                return;
            }
            eprintln!("@init");
        }
        pub fn circle(&self, p: Coord, r: usize, color: Color, text: &str, title: &str) {
            if !cfg!(feature = "local") {
                return;
            }
            eprintln!(
                "@circle {} {} {} {:?} '{}' '{}'",
                p.x, p.y, r, color, text, title
            );
        }
        pub fn rect(&self, p1: Coord, p2: Coord, color: Color, text: &str, title: &str) {
            if !cfg!(feature = "local") {
                return;
            }
            eprintln!(
                "@rect {} {} {} {} {:?} '{}' '{}'",
                p1.x, p1.y, p2.x, p2.y, color, text, title
            );
        }
        pub fn line(&self, p1: Coord, p2: Coord, color: Color, text: &str, title: &str) {
            if !cfg!(feature = "local") {
                return;
            }
            eprintln!(
                "@line {} {} {} {} {:?} '{}' '{}'",
                p1.x, p1.y, p2.x, p2.y, color, text, title
            );
        }
        pub fn next_turn(&self) {
            if !cfg!(feature = "local") {
                return;
            }
            eprintln!("@next");
        }
    }
}
