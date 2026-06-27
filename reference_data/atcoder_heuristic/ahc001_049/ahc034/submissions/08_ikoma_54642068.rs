#![allow(unused_imports, dead_code, unused_macros, unused_variables, non_snake_case, unused_parens)]
use proconio::{input,fastout,marker::{Bytes, Chars, Usize1,Isize1},source::auto::AutoSource};
use itertools::{Itertools,iproduct};
use rand_chacha::ChaCha20Rng;
use std::cmp::{min,max,Ordering,Reverse};
use std::mem::{swap,replace};
use std::collections::{VecDeque,LinkedList,HashMap,BTreeMap,HashSet,BTreeSet,BinaryHeap, hash_map::RandomState};
use rand::prelude::*;
use std::cell::Cell;
use std::time::Instant;
use std::time::Duration;
use num::integer::*;
use superslice::*;
use ac_library::*;
use std::hash::{BuildHasherDefault, BuildHasher, Hash, Hasher};
use rustc_hash::FxHasher;
type DefaultHasher = BuildHasherDefault<FxHasher>;
macro_rules! get {($vec:expr, $index:expr) => {unsafe{$vec.get_unchecked($index)}};($vec:expr, $index:expr, $($rest:expr),+) => {get!($vec.get_unchecked($index), $($rest),+)};}
macro_rules! get_mut {($vec:expr, $index:expr) => {unsafe{$vec.get_unchecked_mut($index)}};($vec:expr, $index:expr, $($rest:expr),+) => {get_mut!($vec.get_unchecked_mut($index), $($rest),+)};}
macro_rules! mulvec {($x:expr; $s:expr) => {vec![$x; $s]};($x:expr; $s0:expr; $( $s:expr );+) => {mulvec![vec![$x; $s0]; $( $s );+ ]};}
macro_rules! dbgdispln {() => {};($exp: expr) =>{};($fmt: expr, $($arg : tt )*) =>{}}
macro_rules! displn {() => { let _ = println!(); };($exp: expr) => { let _ = println!("{}", $exp); };($fmt: expr, $($arg : tt )*) => { let _ = println!($fmt, $($arg)*); }}
macro_rules! disp {($exp: expr) => { let _ = print!("{}", $exp); };($fmt: expr, $($arg : tt )*) => { let _ = print!($fmt, $(arg)*); }}
macro_rules! dbg {($($x:tt)*) => {{#[cfg(debug_assertions)]{std::dbg!($($x)*)}#[cfg(not(debug_assertions))]{($($x)*)}}}}
macro_rules! mat {($($e:expr),*) => { Vec::from(vec![$($e),*]) };($($e:expr,)*) => { Vec::from(vec![$($e),*]) };($e:expr; $d:expr) => { Vec::from(vec![$e; $d]) };($e:expr; $d:expr $(; $ds:expr)+) => { Vec::from(vec![mat![$e $(; $ds)*]; $d]) };}
trait ChMaxMin<T> {
	fn chmax(&mut self, v: T) -> bool;
	fn chmin(&mut self, v: T) -> bool;
}
impl<T: PartialOrd> ChMaxMin<T> for Option<T> {
	fn chmax(&mut self, v: T) -> bool { if self.is_none() || v > *self.as_ref().unwrap() { *self = Some(v); true } else { false } }
	fn chmin(&mut self, v: T) -> bool { if self.is_none() || v < *self.as_ref().unwrap() { *self = Some(v); true } else { false } }
}
impl<T: PartialOrd> ChMaxMin<T> for T {
	fn chmax(&mut self, v: T) -> bool { if v > *self { *self = v; true } else { false } }
	fn chmin(&mut self, v: T) -> bool { if v < *self { *self = v; true } else { false } }
}
const CONTEST_TIME_LIMIT:f64 = 2.0;
const TIME_LIMIT:f64 = CONTEST_TIME_LIMIT * 0.95;
mod ahc_time {
	use std::time::Instant;
	static mut START_TIME:Option<Instant> = None;
	static mut LIMIT_TIME_US:u128 = 0;
	static mut LIMIT_TIME_F64:f64 = 1e60;
	const LOCAL_MARGIN:f64 = 1.1;
	pub fn init() {
		unsafe {
			START_TIME = Some(Instant::now());
		}
	}
	#[inline]
	pub fn get_time() -> f64 {
		unsafe {
			let elapse = START_TIME.unwrap().elapsed().as_secs_f64();
			elapse
		}
	}
	#[inline]
	pub fn set_limit_time(limit:f64) {
		unsafe {
			{
				LIMIT_TIME_F64 = limit;
			}
			LIMIT_TIME_US = (LIMIT_TIME_F64 * 1e6) as u128;
		}
	}
	#[inline]
	pub fn is_limit_over() -> bool {
		unsafe {
			START_TIME.unwrap().elapsed().as_micros() > LIMIT_TIME_US
		}
	}
	#[inline]
	pub fn time_limit_ratio() -> f64 {
		unsafe {
			START_TIME.unwrap().elapsed().as_secs_f64() / LIMIT_TIME_F64
		}
	}
}
mod rnd {
	use super::*;
	use rand_chacha::ChaCha20Rng;
	use rand::prelude::*;
	use rand_distr::{Normal, uniform::{SampleUniform, SampleRange}};
	static mut RNG:Option<ChaCha20Rng> = None;
	static mut DIST:Option<Normal<f64>> = None;
	pub fn init(seed:u64, s:usize) {
		unsafe {
			RNG = Some(rand_chacha::ChaCha20Rng::seed_from_u64(seed));
			DIST = Some(Normal::<f64>::new(0f64, s as f64).unwrap());
		}
	}
	#[inline]
	pub fn get_noise() -> f64 {
		unsafe {
			DIST.unwrap().sample(&mut RNG.as_mut().unwrap()).round()
		}
	}
	#[inline]
	pub fn gen_range<T, R>(range: R) -> T
	where
		T: SampleUniform,
		R: SampleRange<T>,
	{
		unsafe {
			RNG.as_mut().unwrap().gen_range(range)
		}
	}
	#[inline]
	pub fn gen_bool(th:f64) -> bool
	{
		unsafe {
			RNG.as_mut().unwrap().gen_bool(th)
		}
	}
	#[inline]
	pub fn gen_uniform() -> f64
	{
		unsafe {
			RNG.as_mut().unwrap().gen_range(0..100_000) as f64 * 1e-5
		}
	}
	#[inline]
	pub fn get_rng() -> ChaCha20Rng {
		rand_chacha::ChaCha20Rng::seed_from_u64(gen_range(0..=!0))
	}
}
pub struct Input {
    pub N:usize,
	pub h:Vec<Vec<i64>>,
}
impl std::fmt::Display for Input {
	fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
		writeln!(f, "{}", self.N)?;
		for i in 0..self.N {
			writeln!(f, "{}", self.h[i].iter().join(" "))?;
		}
		Ok(())
	}
}
pub fn parse_input(f: &str) -> Input {
	let f = proconio::source::once::OnceSource::from(f);
	input! {
		from f,
		N: usize,
		h: [[i64; N]; N],
	}
	Input { N, h }
}
impl Input {
	pub fn T(&self) -> Self {
		let mut h = self.h.clone();
		for i in 0..self.N {
			for j in 0..self.N {
				h[i][j] = self.h[j][i];
			}
		}
		Input { N: self.N, h }
	}
}
fn input_from_stdio() -> Input {
	input! {
		N: usize,
		h: [[i64;N]; N],
	}
	Input{N, h}
}
const DIJ: [(usize, usize); 4] = [(0, 1), (1, 0), (0, !0), (!0, 0)];
const DIR: [char; 4] = ['R', 'D', 'L', 'U'];
const DIR_T: [char; 4] = ['D', 'R', 'U', 'L'];
#[derive(Clone, Debug, Copy)]
pub enum Action {
	Move(usize),
	Load(i64),
}
#[derive(Clone, Debug)]
pub struct Output {
	pub out: Vec<Action>,
}
impl Output {
	fn new() -> Self {
		Self {out: vec![]}
	}
	fn output(&self) {
		for &act in self.out.iter() {
			match act {
				Action::Move(i) => println!("{}", DIR[i]),
				Action::Load(i) => println!("{:+}", i),
			}
		}
	}
	fn output_t(&self) {
		for &act in self.out.iter() {
			match act {
				Action::Move(i) => println!("{}", DIR_T[i]),
				Action::Load(i) => println!("{:+}", i),
			}
		}
	}
	fn push(&mut self, act:Action) {
		self.out.push(act);
	}
}
#[derive(Clone, Debug, Copy)]
struct Pos {
	x:usize,
	y:usize,
}
impl Pos {
	fn new(x:usize, y:usize) -> Self {
		Self {x, y}
	}
	fn origin() -> Self {
		Self {x:0, y:0}
	}
	fn go_left(&mut self) -> bool {
		if self.x > 0 {
			self.x -= 1;
			true
		}else {
			false
		}
	}
	fn go_right(&mut self) -> bool {
		if self.x < 19 {
			self.x += 1;
			true
		}else {
			false
		}
	}
	fn go_up(&mut self) -> bool {
		if self.y > 0 {
			self.y -= 1;
			true
		}else {
			false
		}
	}
	fn go_down(&mut self) -> bool {
		if self.y < 19 {
			self.y += 1;
			true
		}else{
			false
		}
	}
}
#[derive(Clone, Debug)]
pub struct State {
	h:Vec<Vec<i64>>,
	pos: Pos,
	out: Output,
	dump:i64,
	cost: i64,
	diff: i64,
	not_h0: i64,
	base:i64,
}
impl State {
	fn new(input:&Input) -> Self {
		let mut not_h0 = 0;
		let mut diff = 0;
		for i in 0..20 {
			for j in 0..20 {
				if input.h[i][j] != 0 {
					not_h0 += 1;
					diff += input.h[i][j].abs();
				}
			}
		}
		Self {
			h: input.h.clone(),
			pos:Pos::origin(),
			out: Output::new(),
			dump: 0,
			cost: 0,
			diff,
			not_h0,
			base: diff,
		}
	}
	fn compute_score(&self) -> i64 {
		(1e9 * self.base as f64 / (self.cost + self.diff * 100 + 10000 * self.not_h0) as f64).round() as i64
	}
	fn get_h(&self) -> i64 {
		self.h[self.pos.y][self.pos.x]
	}
	fn load(&mut self, d:i64) {
		if d == 0 {return;}
		assert!(d > 0 || -d <= self.dump);
		let h = &mut self.h[self.pos.y][self.pos.x];
		if *h == 0 {
			self.not_h0 += 1;
		}else{
			self.diff -= h.abs();
		}
		*h -= d;
		if *h == 0 {
			self.not_h0 -= 1;
		}else{
			self.diff += h.abs();
		}
		self.out.push(Action::Load(d));
		self.cost += d.abs();
		self.dump += d;
	}
	fn go_left(&mut self) -> bool {
		let ret = self.pos.go_left();
		if ret {
			self.out.push(Action::Move(2));
			self.cost += 100 + self.dump;
		}
		ret
	}
	fn go_right(&mut self) -> bool {
		let ret = self.pos.go_right();
		if ret {
			self.out.push(Action::Move(0));
			self.cost += 100 + self.dump;
		}
		ret
	}
	fn go_up(&mut self) -> bool {
		let ret = self.pos.go_up();
		if ret {
			self.out.push(Action::Move(3));
			self.cost += 100 + self.dump;
		}
		ret
	}
	fn go_down(&mut self) -> bool {
		let ret = self.pos.go_down();
		if ret {
			self.out.push(Action::Move(1));
			self.cost += 100 + self.dump;
		}
		ret
	}
}
pub fn solve(input:&Input, center:usize) -> (i64, State) {
	let mut state = State::new(input);
	fn load(state:&mut State, ub:i64) {
		let h = state.get_h();
		if h > 0 {
			state.load(h.min(ub));
		}else if h < 0 && state.dump > 0 {
			state.load(-(-h).min(state.dump));
		}
	}
	for _ in 0..center {
		load(&mut state, 1000);
		state.go_right();
	}
	let mut hv = (0..center).map(|i| state.h[0][i]).sum::<i64>();
	if hv < 0 {
		state.load(-hv - state.dump);
		for _ in 0..center {
			state.go_left();
			let h = state.get_h();
			hv -= h;
			load(&mut state, 1000);
			if hv == 0 {break;}
		}
		while state.pos.x < center {
			state.go_right();
		}
	}
	for i in 0..19 {
		let right = (i&1)==0;
		let mut hv = vec![];
		if right {
			for j in center+1..20 {
				hv.push(state.h[i][j]);
			}
			for j in (center+1..20).rev() {
				hv.push(state.h[i+1][j]);
			}
		}else{
			for j in (0..center).rev() {
				hv.push(state.h[i][j]);
			}
			for j in 0..center {
				hv.push(state.h[i+1][j]);
			}
		}
		let mut h_min = 0;
		let mut hs = 0;
		for &h in &hv {
			hs += h;
			h_min.chmin(hs);
		}
		let h_tgt = -h_min;
		state.load(h_tgt - state.dump);
		if right {
			for j in center+1..20 {
				state.go_right();
				load(&mut state, 1000);
			}
			state.go_down();
			for j in center+1..20 {
				load(&mut state, 1000);
				state.go_left();
			}
		}else{
			for j in 0..center {
				state.go_left();
				load(&mut state, 1000);
			}
			state.go_down();
			for j in 0..center {
				load(&mut state, 1000);
				state.go_right();
			}
		}
	}
	{
		let hv = (0..center).map(|i| state.h[19][i]).rev().collect_vec();
		let mut h_min = 0;
		let mut hs = 0;
		for &h in &hv {
			hs += h;
			h_min.chmin(hs);
		}
		let h_tgt = -h_min;
		state.load(h_tgt - state.dump);
		for j in 0..center {
			state.go_left();
			load(&mut state, 1000);
		}
	}
	for _ in 0..center {
		state.go_right();
		load(&mut state, 1000);
	}
	let hv = (0..center).map(|i| state.h[i][center]).rev().collect_vec();
	let mut n_not_h0 = 0;
	for j in (0..19).rev() {
		state.go_up();
		let h = state.get_h();
		if h >= 0 && j>0 {
			let h_minus = (0..j).fold(0, |acc, i| {
				let h = state.h[i][center];
				if h < 0 {
					acc - h
				}else{
					acc
				}
			});
			let load_tgt = h_minus - state.dump;
			if load_tgt > 0 {
				load(&mut state, load_tgt);
			}
		}else{
			load(&mut state, 1000);
		}
		if state.get_h() != 0 {
			n_not_h0 += 1;
		}
		if state.not_h0 == n_not_h0 {
			break;
		}
	}
	for j in 0..19 {
		if state.not_h0 == 0 {break;}
		state.go_down();
		load(&mut state, 1000);
	}
	let score = state.compute_score();
	(score, state)
}
fn main() {
	ahc_time::init();
	let input;
	let limit_time;
	{
		input = input_from_stdio();
		limit_time = TIME_LIMIT;
	}
	rnd::init(0, 1);
	let mut best_state = State::new(&input);
	for center in 0..20 {
		let (score, state) = solve(&input, center);
		if score > best_state.compute_score() {
			best_state = state;
		}
	}
	let input_t = input.T();
	let mut best_state_t = State::new(&input_t);
	for center in 0..20 {
		let (score, state) = solve(&input_t, center);
		if score > best_state_t.compute_score() {
			best_state_t = state;
		}
	}
	if best_state_t.compute_score() > best_state.compute_score() {
		best_state_t.out.output_t();
	}else{
		best_state.out.output();
	}
	std::process::exit(0);
}