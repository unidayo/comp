#![allow(dead_code)]
#![allow(non_upper_case_globals)]
#![allow(unused_imports)]
use grid::{Coord, Map2d};
use itertools::{iproduct, Itertools};
use proconio::{marker::*, *};
use std::cmp::*;
use std::collections::*;
use std::process::id;

const TIME_LIMIT: f64 = 1.989 * 1.0;
const IS_LOCAL: bool = false;
const UNVISIT_PENALTY: f64 = 1000000000.0;

static mut visit_for_dfs: [usize; 1600] = [!0; 1600];
static mut iter_for_sabun: usize = 0;
static mut cnt_greater: usize = 0;
static mut cnt_equal: usize = 0;
static mut cnt_less: usize = 0;
static mut ac_eq: usize = 0;
static mut ac_lt: usize = 0;
static mut ac_gt: usize = 0;
static mut gstack: [(Coord, usize, u8); 1000] = [(Coord::origin(), 0, 0); 1000];

pub fn main() {
    get_time();
    let mut input = read_input();
    eprintln!("sumd {}", input.sumd);
    let output = solve(&mut input);
    output.print_ans(&input);
    eprintln!("Time = {}", get_time());
    /*unsafe {
        crate::print_data!(cnt_greater);
        crate::print_data!(ac_gt);
        crate::print_data!(cnt_equal);
        crate::print_data!(ac_eq);
        crate::print_data!(cnt_less);
        crate::print_data!(ac_lt);
    }*/
}

fn solve(input: &mut Input) -> Output {
    /*let mut pool = vec![];
    let mut cnt = 4;
    for _i in 0..cnt {
        pool.push(SASolver::new(State::new(input)));
    }
    let mut _first_temp = 3000;
    while cnt > 1 {
        for i in 0..cnt {
            pool[i].run(input, 0.02);
        }
        pool.sort_by(|a, b| (a.state.score as usize).cmp(&(b.state.score as usize)));
        for _i in 0..cnt / 2 {
            pool.pop();
        }
        cnt = pool.len();
    }
    let mut solver = pool.pop().unwrap();
    solver.run(input, TIME_LIMIT - get_time());
    */
    let state = State::new(input);
    let mut solver = SASolver::new(state);
    solver.run(input, TIME_LIMIT - get_time());

    if IS_LOCAL {
        let real_score = solver.state.calc_real_score(input);
        Output {
            score: real_score,
            actions: solver.state.pos,
        }
    } else {
        Output {
            score: solver.state.score,
            actions: solver.state.pos,
        }
    }
}

#[derive(Clone)]
pub struct State {
    score: f64,
    penalty: f64, // 0.0
    pos: Vec<Coord>,
    visit: Map2d<usize>,
    pos_cell: Map2d<Vec<usize>>, // 各マスの訪問する位置を昇順に
    amount_width: Vec<usize>,
    nanbanme: Vec<usize>, // [i]: pos[i]はcoord(pos[i])のなかで何番目か
    need_update: bool,
}

impl State {
    fn new(input: &Input) -> Self {
        let mut best_state = Self {
            score: f64::MAX,
            penalty: 0.0,
            pos: vec![],
            visit: Map2d::new(vec![0], 1),
            pos_cell: Map2d::new(vec![vec![]], 1),
            amount_width: vec![],
            nanbanme: vec![],
            need_update: true,
        };
        for _i in 0..100 {
            let mut state = Self {
                score: 0.0,
                penalty: 0.0,
                pos: vec![],
                visit: Map2d::new(vec![0; input.n * input.n], input.n),
                pos_cell: Map2d::new(vec![vec![]], 1),
                amount_width: vec![],
                nanbanme: vec![],
                need_update: true,
            };
            let mut visit = Map2d::new(vec![false; input.n * input.n], input.n);
            make_valid_pos(input, &mut state.pos, &mut visit, Coord::origin());
            for &p in state.pos.iter() {
                state.visit[p] += 1;
            }
            state.score = state.calc_score(input);
            if state.score < best_state.score {
                best_state = state;
            }
        }
        best_state.pos_cell = Map2d::new(vec![vec![]; input.n * input.n], input.n);
        best_state
    }

    fn calc_score(&self, input: &Input) -> f64 {
        if self.pos.len() > 100000 {
            return UNVISIT_PENALTY;
        }
        let mut sum = 0;
        // (最初の訪問、最後の訪問, 出現回数)
        let mut mae_visit = Map2d::new(vec![(!0, !0, 0); input.n * input.n], input.n);
        for (i, &p) in self.pos.iter().enumerate() {
            if mae_visit[p].0 == !0 {
                mae_visit[p] = (i, i, 0);
            } else {
                sum += (i - mae_visit[p].1) * (i - mae_visit[p].1 - 1) / 2 * input.d[p];
                mae_visit[p].1 = i;
            }
        }
        for (x, y) in iproduct!(0..input.n, 0..input.n) {
            let p = Coord::new(x, y);
            let dur = (self.pos.len() - mae_visit[p].1) + mae_visit[p].0;
            sum += dur * (dur - 1) / 2 * input.d[p];
        }
        sum as f64 / self.pos.len() as f64
    }

    fn calc_real_score(&self, input: &Input) -> f64 {
        if self.pos.len() > 100000 {
            return UNVISIT_PENALTY;
        }
        let mut sum = 0;
        // (最初の訪問、最後の訪問, 出現回数)
        let mut mae_visit = Map2d::new(vec![(!0, !0, 0); input.n * input.n], input.n);
        for (i, &p) in self.pos.iter().enumerate() {
            if mae_visit[p].0 == !0 {
                mae_visit[p] = (i, i, 0);
            } else {
                sum += (i - mae_visit[p].1) * (i - mae_visit[p].1 - 1) / 2 * input.real_d[p];
                mae_visit[p].1 = i;
            }
        }
        for (x, y) in iproduct!(0..input.n, 0..input.n) {
            let p = Coord::new(x, y);
            let dur = (self.pos.len() - mae_visit[p].1) + mae_visit[p].0;
            sum += dur * (dur - 1) / 2 * input.real_d[p];
        }
        sum as f64 / self.pos.len() as f64
    }

    fn calc_sabun_for_inverse(&mut self, input: &Input, start_idx: usize, end_idx: usize) -> f64 {
        let mut prev = 0;
        let mut next = 0;
        if self.need_update {
            self.need_update = false;
            self.update_from_pos(input);
        }
        let mut used = Map2d::new(vec![false; input.n * input.n], input.n);
        for (i, &p) in self.pos[start_idx..end_idx].iter().enumerate() {
            if used[p] {
                continue;
            }
            used[p] = true;
            // 範囲にある反転前に最も左のPのidx
            let in_left = start_idx + i;
            let new_in_right = end_idx - (in_left - start_idx);
            // 範囲にある反転前に最も右のPのidx
            let in_right = self.get_lt(p, end_idx + 1).unwrap();
            let new_in_left = end_idx - (in_right - start_idx);

            // start_idxより左にある最も右のPのidx
            let out_left = self.get_lt_fast(p, in_left).unwrap();
            if start_idx <= out_left && out_left <= end_idx {
                // 範囲内にしかないのでスコアは変わらない
                continue;
            }
            // end_idxより右にある最も左のPのidx
            let out_right = self.get_gt_fast(p, in_right).unwrap();

            prev += self.calc_cost(input, p, out_left, in_left);
            prev += self.calc_cost(input, p, in_right, out_right);
            next += self.calc_cost(input, p, out_left, new_in_left);
            next += self.calc_cost(input, p, new_in_right, out_right);
        }

        (next as f64 - prev as f64) / self.pos.len() as f64
    }

    fn calc_sabun_for_change(
        &mut self,
        input: &mut Input,
        start_idx: usize,
        end_idx: usize,
        newpos: &Vec<Coord>,
    ) -> (usize, usize) {
        if self.need_update {
            self.need_update = false;
            self.update_from_pos(input);
        }
        unsafe {
            iter_for_sabun += 1;
        }
        let baseprev = 0;
        let basenext = 1;

        let mut prev = self.calc_uti(input, &self.pos[start_idx..=end_idx]);
        prev += self.calc_soto(
            input,
            start_idx,
            end_idx,
            &self.pos[start_idx..=end_idx],
            baseprev,
            true,
        );
        // !0,!0
        for &p in self.pos[start_idx..=end_idx].iter() {
            input.memo[p].1 = !0;
        }
        let mut next = self.calc_uti(input, &newpos[..]);
        next += self.calc_soto(input, start_idx, end_idx, &newpos[..], basenext, false);

        prev += self.calc_not_contain(
            input,
            start_idx,
            end_idx,
            &self.pos[start_idx..=end_idx],
            &newpos[..],
            baseprev,
        );
        next += self.calc_not_contain(
            input,
            start_idx,
            end_idx,
            &newpos[..],
            &self.pos[start_idx..=end_idx],
            basenext,
        );
        if end_idx - start_idx + 1 < newpos.len() {
            next += self.calc_not_use(
                input,
                start_idx,
                end_idx,
                &self.pos[start_idx..=end_idx],
                &newpos[..],
            );
        } else if end_idx - start_idx + 1 > newpos.len() {
            prev += self.calc_not_use(
                input,
                start_idx,
                end_idx,
                &self.pos[start_idx..=end_idx],
                &newpos[..],
            );
        }

        (prev, next)
    }

    fn calc_uti(&self, input: &mut Input, pos: &[Coord]) -> usize {
        // 内側だけ計算
        let mut ret = 0;
        for (idx, &p) in pos.iter().enumerate() {
            unsafe {
                if input.memo[p].4 != iter_for_sabun {
                    input.memo[p] = (0, !0, 0, false, iter_for_sabun);
                }
            }
            if input.memo[p].1 == !0 {
                input.memo[p].1 = idx;
                continue;
            }
            let width = idx - input.memo[p].1;
            ret += width * (width - 1) / 2 * input.d[p];
            input.memo[p].1 = idx;
        }
        ret
    }

    fn calc_soto(
        &self,
        input: &mut Input,
        start_idx: usize,
        end_idx: usize,
        pos: &[Coord],
        base: u8,
        is_prev: bool,
    ) -> usize {
        // self.posのstart_idx..=end_idxをposで置き換えたときの外側を計算
        let mut ret = 0;
        if is_prev {
            // fastを使える
            if pos.len() >= end_idx - start_idx + 1 {
                // 伸びる or 同じ
                let henkawidth = pos.len() - (end_idx - start_idx + 1);
                for (idx_, &p) in pos.iter().enumerate() {
                    if (input.memo[p].0 >> base) & 1 == 1 {
                        continue;
                    }
                    input.memo[p].0 |= 1 << base;
                    let inleft = idx_ + start_idx;
                    let inright = input.memo[p].1 + start_idx;
                    let mut outleft = self.get_lt_fast(p, inleft).unwrap();
                    if start_idx <= outleft && outleft <= end_idx {
                        outleft = inright;
                    } else if end_idx < outleft {
                        outleft += henkawidth;
                    };
                    ret += self.calc_cost_nobiru(input, p, outleft, inleft, henkawidth);
                    if outleft == inright {
                        // 左と右がつながっている
                        continue;
                    }

                    let mut outright = self.get_gt_fast(p, inright).unwrap();
                    if start_idx <= outright && outright <= end_idx {
                        unreachable!();
                    } else if end_idx < outright {
                        outright += henkawidth;
                    };
                    ret += self.calc_cost_nobiru(input, p, inright, outright, henkawidth);
                }
            } else {
                let henkawidth = (end_idx - start_idx + 1) - pos.len();
                for (idx_, &p) in pos.iter().enumerate() {
                    if (input.memo[p].0 >> base) & 1 == 1 {
                        continue;
                    }
                    input.memo[p].0 |= 1 << base;
                    let inleft = idx_ + start_idx;
                    let inright = input.memo[p].1 + start_idx;
                    let mut outleft = self.get_lt(p, start_idx).unwrap();
                    if start_idx <= outleft && outleft <= end_idx {
                        outleft = inright;
                    } else if end_idx < outleft {
                        outleft -= henkawidth;
                    };
                    ret += self.calc_cost_tidimu(input, p, outleft, inleft, henkawidth);
                    if outleft == inright {
                        // 左と右がつながっている
                        continue;
                    }

                    let mut outright = self.get_gt(p, end_idx).unwrap();
                    if start_idx <= outright && outright <= end_idx {
                        unreachable!();
                    } else if end_idx < outright {
                        outright -= henkawidth;
                    };
                    ret += self.calc_cost_tidimu(input, p, inright, outright, henkawidth);
                }
            }
        } else {
            if pos.len() >= end_idx - start_idx + 1 {
                // 伸びる or 同じ
                let henkawidth = pos.len() - (end_idx - start_idx + 1);
                for (idx_, &p) in pos.iter().enumerate() {
                    if (input.memo[p].0 >> base) & 1 == 1 {
                        continue;
                    }
                    input.memo[p].0 |= 1 << base;
                    let inleft = idx_ + start_idx;
                    let inright = input.memo[p].1 + start_idx;
                    let mut outleft = self.get_lt(p, start_idx).unwrap();
                    if start_idx <= outleft && outleft <= end_idx {
                        outleft = inright;
                    } else if end_idx < outleft {
                        outleft += henkawidth;
                    };
                    ret += self.calc_cost_nobiru(input, p, outleft, inleft, henkawidth);
                    if outleft == inright {
                        // 左と右がつながっている
                        continue;
                    }

                    let mut outright = self.get_gt(p, end_idx).unwrap();
                    if start_idx <= outright && outright <= end_idx {
                        unreachable!();
                    } else if end_idx < outright {
                        outright += henkawidth;
                    };
                    ret += self.calc_cost_nobiru(input, p, inright, outright, henkawidth);
                }
            } else {
                let henkawidth = (end_idx - start_idx + 1) - pos.len();
                for (idx_, &p) in pos.iter().enumerate() {
                    if (input.memo[p].0 >> base) & 1 == 1 {
                        continue;
                    }
                    input.memo[p].0 |= 1 << base;
                    let inleft = idx_ + start_idx;
                    let inright = input.memo[p].1 + start_idx;
                    let mut outleft = self.get_lt(p, start_idx).unwrap();
                    if start_idx <= outleft && outleft <= end_idx {
                        outleft = inright;
                    } else if end_idx < outleft {
                        outleft -= henkawidth;
                    };
                    ret += self.calc_cost_tidimu(input, p, outleft, inleft, henkawidth);
                    if outleft == inright {
                        // 左と右がつながっている
                        continue;
                    }

                    let mut outright = self.get_gt(p, end_idx).unwrap();
                    if start_idx <= outright && outright <= end_idx {
                        unreachable!();
                    } else if end_idx < outright {
                        outright -= henkawidth;
                    };
                    ret += self.calc_cost_tidimu(input, p, inright, outright, henkawidth);
                }
            }
        }
        ret
    }

    fn calc_not_contain(
        &self,
        input: &mut Input,
        start_idx: usize,
        end_idx: usize,
        pos: &[Coord],
        kouho: &[Coord],
        base: u8,
    ) -> usize {
        // self.pos[start_idx..=end_idx]をposにしたときkouhoの中でposに含まれない要素の外を計算
        let mut ret = 0;
        if pos.len() >= end_idx - start_idx + 1 {
            // 伸びる or 同じ
            let henkawidth = pos.len() - (end_idx - start_idx + 1);
            for &p in kouho.iter() {
                if (input.memo[p].2 >> base) & 1 == 1 || (input.memo[p].0 >> base) & 1 == 1 {
                    continue;
                }
                input.memo[p].2 |= 1 << base;
                let mut left = self.get_lt(p, start_idx).unwrap();
                let mut right = self.get_gt(p, end_idx).unwrap();
                if left > end_idx {
                    left += henkawidth;
                }
                if right > end_idx {
                    right += henkawidth;
                }
                ret += self.calc_cost_nobiru(input, p, left, right, henkawidth);
            }
        } else {
            let henkawidth = (end_idx - start_idx + 1) - pos.len();
            for &p in kouho.iter() {
                if (input.memo[p].2 >> base) & 1 == 1 || (input.memo[p].0 >> base) & 1 == 1 {
                    continue;
                }
                input.memo[p].2 |= 1 << base;
                let mut left = self.get_lt(p, start_idx).unwrap();
                let mut right = self.get_gt(p, end_idx).unwrap();
                if left > end_idx {
                    left -= henkawidth;
                }
                if right > end_idx {
                    right -= henkawidth;
                }
                ret += self.calc_cost_tidimu(input, p, left, right, henkawidth);
            }
        }
        ret
    }

    fn calc_not_use(
        &self,
        input: &mut Input,
        start_idx: usize,
        end_idx: usize,
        prev: &[Coord],
        next: &[Coord],
    ) -> usize {
        // prevにもnextにも含まれないものの長さを変更したときのコストを計算
        let mut ret = 0;

        if next.len() >= end_idx - start_idx + 1 {
            // 伸びる or 同じ
            let henkawidth = next.len() - (end_idx - start_idx + 1);

            ret += self.amount_width[end_idx] * henkawidth
                + henkawidth * (henkawidth - 1) / 2 * input.sumd;
            for &p in prev.iter().chain(next.iter()) {
                if input.memo[p].3 {
                    continue;
                }
                input.memo[p].3 = true;
                let left = self.get_lt(p, end_idx + 1).unwrap();
                let right = self.get_gt(p, end_idx).unwrap();
                let width = if left < right {
                    right - left
                } else {
                    self.pos.len() - left + right
                };
                let dt = width * input.d[p];
                let aaa = dt * henkawidth + henkawidth * (henkawidth - 1) / 2 * input.d[p];
                ret -= aaa;
            }
        } else {
            let henkawidth = (end_idx - start_idx + 1) - next.len();

            ret += self.amount_width[end_idx] * henkawidth
                - henkawidth * (henkawidth + 1) / 2 * input.sumd;
            for &p in prev.iter().chain(next.iter()) {
                if input.memo[p].3 {
                    continue;
                }
                input.memo[p].3 = true;
                let left = self.get_lt(p, end_idx + 1).unwrap();
                let right = self.get_gt(p, end_idx).unwrap();
                let width = if left < right {
                    right - left
                } else {
                    self.pos.len() - left + right
                };
                let dt = width * input.d[p];
                ret += henkawidth * (henkawidth + 1) / 2 * input.d[p];
                ret -= dt * henkawidth;
            }
        }
        ret
    }

    fn calc_cost(&self, input: &Input, p: Coord, left: usize, right: usize) -> usize {
        // これpos.lenに依存することに注意
        if left < right {
            (right - left) * (right - left - 1) / 2 * input.d[p]
        } else {
            (self.pos.len() - left + right) * (self.pos.len() - left + right - 1) / 2 * input.d[p]
        }
    }

    fn calc_cost_nobiru(
        &self,
        input: &Input,
        p: Coord,
        left: usize,
        right: usize,
        henkawidth: usize,
    ) -> usize {
        // これpos.lenに依存することに注意
        if left < right {
            (right - left) * (right - left - 1) / 2 * input.d[p]
        } else {
            (self.pos.len() + henkawidth - left + right)
                * (self.pos.len() + henkawidth - left + right - 1)
                / 2
                * input.d[p]
        }
    }

    fn calc_cost_tidimu(
        &self,
        input: &Input,
        p: Coord,
        left: usize,
        right: usize,
        henkawidth: usize,
    ) -> usize {
        // これpos.lenに依存することに注意
        if left < right {
            (right - left) * (right - left - 1) / 2 * input.d[p]
        } else {
            (self.pos.len() - henkawidth - left + right)
                * (self.pos.len() - henkawidth - left + right - 1)
                / 2
                * input.d[p]
        }
    }

    fn update_from_pos(&mut self, input: &Input) {
        for p in iproduct!(0..input.n, 0..input.n).map(|t| Coord::new(t.0, t.1)) {
            self.pos_cell[p].clear();
        }
        self.nanbanme.resize(self.pos.len(), 0);
        for (i, &p) in self.pos.iter().enumerate() {
            self.nanbanme[i] = self.pos_cell[p].len();
            self.pos_cell[p].push(i);
        }

        self.amount_width.resize(self.pos.len(), 0);
        let mut amount = 0;
        for p in iproduct!(0..input.n, 0..input.n).map(|t| Coord::new(t.0, t.1)) {
            let left = if p == self.pos[0] {
                0
            } else {
                self.pos_cell[p][self.pos_cell[p].len() - 1]
            };
            let right = if p == self.pos[0] {
                self.get_gt_fast(p, 0).unwrap()
            } else {
                self.pos_cell[p][0]
            };
            let width = if left < right {
                right - left
            } else {
                self.pos.len() - left + right
            };
            amount += width * input.d[p];
        }
        for (i, &p) in self.pos.iter().enumerate() {
            if i == 0 {
                self.amount_width[i] = amount;
            } else {
                let prevleft = self.get_lt_fast(p, i).unwrap();
                let prevright = i;
                let prevwidth = if prevleft < prevright {
                    prevright - prevleft
                } else {
                    self.pos.len() - prevleft + prevright
                };
                amount -= prevwidth * input.d[p];

                let left = i;
                let right = self.get_gt_fast(p, i).unwrap();
                let width = if left < right {
                    right - left
                } else {
                    self.pos.len() - left + right
                };
                amount += width * input.d[p];
                self.amount_width[i] = amount;
            }
        }
    }

    fn get_lt(&self, p: Coord, x: usize) -> Option<usize> {
        let mut ret = !0;
        for &idx in self.pos_cell[p].iter() {
            if idx >= x {
                if ret == !0 {
                    return Some(self.pos_cell[p][self.pos_cell[p].len() - 1]);
                }
                return Some(ret);
            }
            ret = idx;
        }
        Some(ret)
    }

    fn get_gt(&self, p: Coord, x: usize) -> Option<usize> {
        // posでxより右側で最も最初にあるpの位置を返す
        // 無ければ一番左のpの位置を返す
        for &idx in self.pos_cell[p].iter() {
            if idx > x {
                return Some(idx);
            }
        }
        Some(self.pos_cell[p][0])
    }

    fn get_lt_fast(&self, p: Coord, x: usize) -> Option<usize> {
        // 現在のself.pos[x] == pでないといけない
        if self.nanbanme[x] == 0 {
            Some(self.pos_cell[p][self.pos_cell[p].len() - 1])
        } else {
            Some(self.pos_cell[p][self.nanbanme[x] - 1])
        }
    }

    fn get_gt_fast(&self, p: Coord, x: usize) -> Option<usize> {
        // 現在のself.pos[x] == pでないといけない
        if self.nanbanme[x] == self.pos_cell[p].len() - 1 {
            Some(self.pos_cell[p][0])
        } else {
            Some(self.pos_cell[p][self.nanbanme[x] + 1])
        }
    }

    fn doubling(&mut self, input: &Input, x: usize) {
        // x倍する
        for p in iproduct!(0..input.n, 0..input.n).map(|t| Coord::new(t.0, t.1)) {
            self.visit[p] *= x;
        }
        let tmp = self.pos.clone();
        for _ in 0..x - 1 {
            self.pos.extend(tmp.clone());
        }
        self.need_update = true;
    }
}
struct SASolver {
    start_time: f64,
    duration: f64,
    temp: f64,
    log_table: [f64; 65536],
    state: State,
    best_state: State,
    time: f64,
    mae_kousin_best: f64,
    regal_iter: usize,
}

impl SASolver {
    // 3000case 25000, 10, 1.5乗 13065182 15.9121
    // 3000case 20000, 30, 1.5乗 13010736 15.9089
    // 3000case 17500, 10, 1.5乗
    // 3000case 15000, 10, 1.5   13057341 15.9117
    // 3000case 10000, 10, 1.5   13063874 15.9123
    // 3000case 3000, 0.1 senkei 13069761 15.9122
    const GOAL: OptimizationGoal = OptimizationGoal::Min;
    const START_TEMP: f64 = 20000.0;
    const END_TEMP: f64 = 30.0;
    const DIFF_TEMP: f64 = Self::END_TEMP / Self::START_TEMP;

    fn new(state: State) -> Self {
        let mut log_table = [0.0; 65536];
        for i in 0..65536 {
            log_table[i] = ((i as f64 + 0.5) / 65536.0).ln();
        }
        rnd::shuffle(&mut log_table);

        Self {
            start_time: 0.0,
            duration: 0.0,
            temp: 0.0,
            log_table,
            state: state.clone(),
            best_state: state.clone(),
            time: 0.0,
            mae_kousin_best: 0.0,
            regal_iter: 0,
        }
    }

    fn update(&mut self) -> bool {
        self.time = get_time();
        if self.time - self.start_time > self.duration {
            return false;
        }
        // 非線形
        //self.temp =
        //  Self::START_TEMP * Self::DIFF_TEMP.powf((self.time - self.start_time) / self.duration);
        // 線形
        //self.temp = Self::START_TEMP
        //  + (Self::END_TEMP - Self::START_TEMP) * (get_time() - self.start_time) / self.duration;
        //
        self.temp = Self::START_TEMP
            * Self::DIFF_TEMP.powf(((self.time - self.start_time) / self.duration).powf(1.5));
        true
    }

    fn select_neighborhood(&mut self, input: &Input, iter: usize) -> Box<dyn Neighbor> {
        const CNT_NEIGHBORHOOD: usize = 4;
        // 和が100
        // 最初のほうしかinverseが受理されないみたいなのある？
        static neighborhood_ratio: [usize; CNT_NEIGHBORHOOD] = [0, 95, 96, 100]; // (95, 96, 100)
        let op = rnd::next() % 100;

        if op < neighborhood_ratio[0] {
            return Box::new(neighborhood::DeleteRoute::new(input, &mut self.state));
        } else if op < neighborhood_ratio[1] {
            return Box::new(neighborhood::ChangeRoute::new(input, &self.state, iter));
        } else if op < neighborhood_ratio[2] {
            return Box::new(neighborhood::ReplaceRoute::new(input, &self.state));
        } else {
            return Box::new(neighborhood::InvertRoute::new(input, &mut self.state));
        }
    }

    fn update_best_state(&mut self) -> bool {
        if Self::GOAL == OptimizationGoal::Max {
            if self.state.score > self.best_state.score {
                self.best_state = self.state.clone();
                self.mae_kousin_best = self.time;
                return true;
            }
        } else {
            if self.state.score < self.best_state.score {
                self.best_state = self.state.clone();
                self.mae_kousin_best = self.time;
                return true;
            }
        }
        false
    }

    fn is_accepted(
        &mut self,
        input: &Input,
        pre_score: (f64, f64),
        next_score: (f64, f64),
        iter_sa: usize,
    ) -> bool {
        self.regal_iter += 1;
        if next_score.0 == UNVISIT_PENALTY {
            return false;
        }
        //let waru = (input.n as f64).log2();
        let waru = (input.n as f64).log2();
        // ペナルティも正規化必要だ
        let pre_score = (pre_score.0
            + pre_score.1 * (1.0 - (self.time - self.start_time) / self.duration) as f64)
            / waru;
        let next_score = (next_score.0
            + next_score.1 * (1.0 - (self.time - self.start_time) / self.duration) as f64)
            / waru;
        if Self::GOAL == OptimizationGoal::Max {
            next_score as f64 - pre_score as f64
                >= self.temp as f64 * self.log_table[iter_sa & 65535] as f64
        } else {
            -(next_score as f64 - pre_score as f64)
                >= self.temp as f64 * self.log_table[iter_sa & 65535] as f64
        }
    }

    fn run(&mut self, input: &mut Input, duration: f64) {
        self.start_time = get_time();
        self.duration = duration;

        let mut iter_sa = 0;
        let mut cnt_ac = 0;
        let mut cnt_upd_best = 0;
        let mut is_doubled = false;
        let mut is_doubled2 = false;
        while iter_sa & 127 != 0 || self.update() {
            iter_sa += 1;
            if iter_sa % 1000 == 0 {
                if get_time() > TIME_LIMIT * 0.5 && !is_doubled {
                    self.state = self.best_state.clone();
                    self.state.doubling(input, 2);
                    is_doubled = true;
                    self.state.need_update = true;
                } else if get_time() > TIME_LIMIT * 0.9 && !is_doubled2 {
                    self.state = self.best_state.clone();
                    self.state.doubling(input, 2);
                    is_doubled2 = true;
                    self.state.need_update = true;
                } else if iter_sa % 10000 == 0 && self.mae_kousin_best + 0.1 < get_time() {
                    self.state = self.best_state.clone();
                    self.mae_kousin_best = get_time();
                    // kick???????????
                    //self.temp = Self::START_TEMP; // とりあえず再加熱
                }
                self.state.score = self.state.calc_score(input);
                self.best_state.score = self.best_state.calc_score(input);
            }
            let pre_score = (self.state.score, self.state.penalty);
            let mut neighbor: Box<dyn Neighbor> = self.select_neighborhood(input, iter_sa);

            if neighbor.preprocess(input, &mut self.state)
                && self.is_accepted(
                    input,
                    pre_score,
                    (self.state.score, self.state.penalty),
                    iter_sa,
                )
            {
                neighbor.postprocess(input, &mut self.state);
                if self.update_best_state() {
                    cnt_upd_best += 1;
                }
                cnt_ac += 1;
            } else {
                neighbor.rollback(input, &mut self.state, pre_score);
            }
        }
        crate::print_data!(iter_sa);
        eprintln!("regal_iter: {}", self.regal_iter);
        eprintln!("cnt_ac: {}", cnt_ac);
        eprintln!("cnt_upd_best: {}", cnt_upd_best);
        eprintln!("mae_kousin_best: {}", self.mae_kousin_best);
        self.state = self.best_state.clone();
    }
}

#[derive(PartialEq)]
enum OptimizationGoal {
    Max,
    Min,
}
trait Neighbor {
    fn preprocess(&mut self, input: &mut Input, state: &mut State) -> bool;
    fn postprocess(&self, input: &Input, state: &mut State);
    fn rollback(&self, input: &Input, state: &mut State, pre_score: (f64, f64));
}

mod neighborhood {
    use super::*;
    pub struct ChangeRoute {
        start_idx: usize, // もとのstateのposの始まり
        end_idx: usize,   // もとのstateのposの終わり
        memopos: Vec<Coord>,
        new_len: usize,
        iter: usize,
    }
    impl ChangeRoute {
        pub fn new(_input: &Input, state: &State, iter: usize) -> Self {
            let start_idx = rnd::next() % (state.pos.len() - 1);
            let end_idx = loop {
                let end_idx = rnd::range(start_idx + 1, min(start_idx + 50, state.pos.len()));
                if state.pos[start_idx].dist(&state.pos[end_idx]) > 6 {
                    continue;
                }
                break end_idx;
            };
            let memopos = vec![];
            Self {
                start_idx,
                end_idx,
                memopos,
                new_len: 0,
                iter,
            }
        }

        fn dfs(
            &self,
            input: &Input,
            state: &mut State,
            p: Coord,
            depth: usize,
            newpos: &mut Vec<Coord>,
            cnt_unvisit: &mut usize,
        ) -> bool {
            unsafe {
                if state.visit[p] == 0 {
                    *cnt_unvisit -= 1;
                }
                state.visit[p] += 1;
                if depth > 0 && p == state.pos[self.end_idx] && *cnt_unvisit == 0 {
                    return true;
                }
                // 絶対に戻れないやつは切る
                if depth >= 6
                    || p.x.abs_diff(state.pos[self.end_idx].x)
                        + p.y.abs_diff(state.pos[self.end_idx].y)
                        > 6 - depth
                    || 6 - depth < *cnt_unvisit
                {
                    if state.visit[p] == 1 {
                        *cnt_unvisit += 1;
                    }
                    state.visit[p] -= 1;
                    return false;
                }
                for &idx in input.random_idx[input.neighbor[p].len()]
                    [rnd::next() % input.random_idx[input.neighbor[p].len()].len()]
                .iter()
                {
                    let np = input.neighbor[p][idx];
                    if rnd::next() % 15 < 4
                        || (!input.is_aps[np] && visit_for_dfs[np.idx(input.n)] == self.iter)
                    {
                        continue;
                    }
                    visit_for_dfs[np.idx(input.n)] = self.iter;
                    if self.dfs(input, state, np, depth + 1, newpos, cnt_unvisit) {
                        newpos.push(np);
                        return true;
                    }
                    // これvisitを戻す必要がある？
                    visit_for_dfs[np.idx(input.n)] = !0;
                }
                if state.visit[p] == 1 {
                    *cnt_unvisit += 1;
                }
                assert!(state.visit[p] > 0);
                state.visit[p] -= 1;
                false
            }
        }

        fn dfs2(
            &self,
            input: &Input,
            state: &mut State,
            p: Coord,
            depth: usize,
            newpos: &mut Vec<Coord>,
            cnt_unvisit: &mut usize,
        ) -> bool {
            //let mut stack = vec![(p, depth, 0)];
            let mut cnt_stack = 1;
            unsafe {
                gstack[0] = (p, depth, 0);
            }
            let mut ps = [Coord::new(0, !0); 7];
            //while let Some((p, depth, ikikaeri)) = stack.pop() {
            while cnt_stack > 0 {
                unsafe {
                    let (p, depth, ikikaeri) = gstack[cnt_stack - 1];
                    cnt_stack -= 1;
                    if ikikaeri == 0 {
                        // iki
                        //stack.push((p, depth, 1));
                        gstack[cnt_stack] = (p, depth, 1);
                        cnt_stack += 1;
                        ps[depth] = p;
                        visit_for_dfs[p.idx(input.n)] = self.iter;

                        if state.visit[p] == 0 {
                            *cnt_unvisit -= 1;
                        }
                        state.visit[p] += 1;
                        if depth > 0 && p == state.pos[self.end_idx] && *cnt_unvisit == 0 {
                            for i in (1..=depth).rev() {
                                newpos.push(ps[i]);
                            }
                            return true;
                        }
                        if depth >= 6
                            || p.x.abs_diff(state.pos[self.end_idx].x)
                                + p.y.abs_diff(state.pos[self.end_idx].y)
                                > 6 - depth
                            || 6 - depth < *cnt_unvisit
                        {
                            /*if state.visit[p] == 1 {
                                *cnt_unvisit += 1;
                            }
                            state.visit[p] -= 1;
                            */
                            continue;
                        }
                        for &idx in input.random_idx[input.neighbor[p].len()]
                            [rnd::next() % input.random_idx[input.neighbor[p].len()].len()]
                        .iter()
                        {
                            let np = input.neighbor[p][idx];
                            if rnd::next() % 15 < 4
                                || (!input.is_aps[np]
                                    && visit_for_dfs[np.idx(input.n)] == self.iter)
                            {
                                continue;
                            }
                            //stack.push((np, depth + 1, 0));
                            gstack[cnt_stack] = (np, depth + 1, 0);
                            cnt_stack += 1;
                        }
                    } else {
                        // kaeir
                        if state.visit[p] == 1 {
                            *cnt_unvisit += 1;
                        }
                        assert!(state.visit[p] > 0);
                        state.visit[p] -= 1;
                        visit_for_dfs[p.idx(input.n)] = !0;
                    }
                }
            }
            false
        }
    }
    impl Neighbor for ChangeRoute {
        fn preprocess(&mut self, input: &mut Input, state: &mut State) -> bool {
            // dfsで解を見つけられたらtrue, 失敗でfalse
            let mut newpos = vec![];
            let mut cnt_unvisit = 0;
            for &p in state.pos[self.start_idx..=self.end_idx].iter() {
                assert!(state.visit[p] > 0);
                state.visit[p] -= 1;
                if state.visit[p] == 0 {
                    cnt_unvisit += 1;
                }
            }
            if self.dfs2(
                input,
                state,
                state.pos[self.start_idx],
                0,
                &mut newpos,
                &mut cnt_unvisit,
            ) {
                newpos.push(state.pos[self.start_idx]);
                for &cp in state.pos[self.start_idx..=self.end_idx].iter() {
                    if state.visit[cp] == 0 {
                        // unvisitが存在
                        for &p in state.pos[self.start_idx..=self.end_idx].iter() {
                            state.visit[p] += 1;
                        }
                        for &p in newpos.iter() {
                            assert!(state.visit[p] > 0);
                            state.visit[p] -= 1;
                        }
                        return false;
                    }
                }

                newpos.reverse();
                assert!(newpos[newpos.len() - 1] == state.pos[self.end_idx]);
                self.new_len = newpos.len();
                if self.end_idx - self.start_idx + 1 == self.new_len {
                    if state.pos[self.start_idx..=self.end_idx] == newpos {
                        for &p in state.pos[self.start_idx..=self.end_idx].iter() {
                            state.visit[p] += 1;
                        }
                        for &p in newpos.iter() {
                            state.visit[p] -= 1;
                        }
                        return false;
                    }
                    let (prev, next) =
                        state.calc_sabun_for_change(input, self.start_idx, self.end_idx, &newpos);

                    state.score =
                        state.score + (next as f64 - prev as f64) / state.pos.len() as f64;
                    self.memopos = newpos;
                } else if self.end_idx - self.start_idx + 1 < self.new_len {
                    // 伸びる
                    let henkawidth = self.new_len - (self.end_idx - self.start_idx + 1);
                    let (prev, next) =
                        state.calc_sabun_for_change(input, self.start_idx, self.end_idx, &newpos);

                    state.score = (state.score * state.pos.len() as f64 + next as f64
                        - prev as f64)
                        / (state.pos.len() + henkawidth) as f64;
                    self.memopos = newpos;
                } else {
                    // tijimu
                    let henkawidth = (self.end_idx - self.start_idx + 1) - self.new_len;
                    let (prev, next) =
                        state.calc_sabun_for_change(input, self.start_idx, self.end_idx, &newpos);

                    state.score = (state.score * state.pos.len() as f64 + next as f64
                        - prev as f64)
                        / (state.pos.len() - henkawidth) as f64;
                    self.memopos = newpos;
                }
                true
            } else {
                for &p in state.pos[self.start_idx..=self.end_idx].iter() {
                    state.visit[p] += 1;
                }

                false
            }
        }

        fn postprocess(&self, _input: &Input, state: &mut State) {
            unsafe {
                if self.end_idx - self.start_idx + 1 == self.memopos.len() {
                    cnt_equal += 1;
                    ac_eq += 1;
                } else if self.end_idx - self.start_idx + 1 < self.memopos.len() {
                    cnt_greater += 1;
                    ac_gt += 1;
                } else {
                    cnt_less += 1;
                    ac_lt += 1;
                }
            }
            state.need_update = true;
            let _ = state
                .pos
                .splice(self.start_idx..=self.end_idx, self.memopos.clone())
                .collect::<Vec<_>>();
        }

        fn rollback(&self, _input: &Input, state: &mut State, pre_score: (f64, f64)) {
            if self.memopos.len() > 0 {
                unsafe {
                    if self.end_idx - self.start_idx + 1 == self.memopos.len() {
                        cnt_equal += 1;
                    } else if self.end_idx - self.start_idx + 1 < self.memopos.len() {
                        cnt_greater += 1;
                    } else {
                        cnt_less += 1;
                    }
                }
                for &p in state.pos[self.start_idx..=self.end_idx].iter() {
                    assert!(state.visit[p] > 0);
                    state.visit[p] += 1;
                }
                for &p in self.memopos.iter() {
                    state.visit[p] -= 1;
                }

                state.score = pre_score.0;
                state.penalty = pre_score.1;
            }
        }
    }

    pub struct ReplaceRoute {
        left_start: usize,
        left_end: usize,
        right_start: usize,
        right_end: usize,
        is_rev: bool,
    }
    impl ReplaceRoute {
        pub fn new(_input: &Input, state: &State) -> Self {
            assert!(state.pos.len() > 4);
            for _ in 0..300 {
                let left_start = rnd::next() % (state.pos.len() - 3);
                // この区間でかすぎるので制限する?
                let left_end =
                    rnd::range(left_start + 1, min(left_start + 300, state.pos.len() - 2));
                for right_start in left_end + 1..state.pos.len() - 1 {
                    if state.pos[left_start] == state.pos[right_start] {
                        for right_end in right_start + 1..min(right_start + 300, state.pos.len()) {
                            if state.pos[left_end] == state.pos[right_end] {
                                return Self {
                                    left_start,
                                    left_end,
                                    right_start,
                                    right_end,
                                    is_rev: false,
                                };
                            }
                        }
                    } else if state.pos[left_end] == state.pos[right_start] {
                        for right_end in right_start + 1..min(right_start + 300, state.pos.len()) {
                            if state.pos[left_start] == state.pos[right_end] {
                                return Self {
                                    left_start,
                                    left_end,
                                    right_start,
                                    right_end,
                                    is_rev: true,
                                };
                            }
                        }
                    }
                }
            }
            Self {
                left_start: !0,
                left_end: !0,
                right_start: !0,
                right_end: !0,
                is_rev: false,
            }
        }
    }
    impl Neighbor for ReplaceRoute {
        fn preprocess(&mut self, input: &mut Input, state: &mut State) -> bool {
            if self.left_start == !0 {
                return false;
            }
            let mut left = state.pos[self.left_start..=self.left_end].to_vec();
            let mut right = state.pos[self.right_start..=self.right_end].to_vec();
            if self.is_rev == false {
                state.pos.splice(self.right_start..=self.right_end, left);
                state.pos.splice(self.left_start..=self.left_end, right);
            } else {
                left.reverse();
                right.reverse();
                state.pos.splice(self.right_start..=self.right_end, left);
                state.pos.splice(self.left_start..=self.left_end, right);
            }
            state.score = state.calc_score(input);
            true
        }
        fn postprocess(&self, _input: &Input, state: &mut State) {
            state.need_update = true;
        }
        fn rollback(&self, _input: &Input, state: &mut State, pre_score: (f64, f64)) {
            if self.left_start == !0 {
                return;
            }
            let now_right_start = self.right_start + (self.right_end - self.right_start)
                - (self.left_end - self.left_start);
            let now_right_end = now_right_start + (self.left_end - self.left_start);
            let now_left_start = self.left_start;
            let now_left_end = now_left_start + (self.right_end - self.right_start);
            let mut left = state.pos[now_left_start..=now_left_end].to_vec();
            let mut right = state.pos[now_right_start..=now_right_end].to_vec();
            if self.is_rev == false {
                state.pos.splice(now_right_start..=now_right_end, left);
                state.pos.splice(now_left_start..=now_left_end, right);
            } else {
                left.reverse();
                right.reverse();
                state.pos.splice(now_right_start..=now_right_end, left);
                state.pos.splice(now_left_start..=now_left_end, right);
            }
            state.score = pre_score.0;
            state.penalty = pre_score.1;
        }
    }

    pub struct InvertRoute {
        start: usize,
        end: usize,
    }
    impl InvertRoute {
        pub fn new(input: &Input, state: &mut State) -> Self {
            assert!(state.pos.len() > 5);
            if state.need_update {
                state.need_update = false;
                state.update_from_pos(input);
            }
            for _ in 0..2000 {
                /*let start = rnd::next() % (state.pos.len() - 5);
                for end in min(start + 5, state.pos.len() - 1)..state.pos.len() {
                    if state.pos[start] == state.pos[end] {
                        return Self { start, end };
                    }
                }*/
                let p = Coord::new(rnd::next() % input.n, rnd::next() % input.n);
                if state.pos_cell[p].len() < 2 {
                    continue;
                }
                let start = state.pos_cell[p][rnd::next() % (state.pos_cell[p].len() - 1)];
                let end = state.get_gt_fast(p, start).unwrap();
                // 余りでかすぎてもじゅりされにくいかも
                if end - start < 5 {
                    continue;
                }
                return Self { start, end };
            }
            Self { start: !0, end: !0 }
        }
    }
    impl Neighbor for InvertRoute {
        fn preprocess(&mut self, input: &mut Input, state: &mut State) -> bool {
            if self.start == !0 {
                return false;
            }
            state.score += state.calc_sabun_for_inverse(input, self.start, self.end);
            true
        }
        fn postprocess(&self, _input: &Input, state: &mut State) {
            state.pos[self.start..=self.end].reverse();
            state.need_update = true;
        }
        fn rollback(&self, _input: &Input, state: &mut State, pre_score: (f64, f64)) {
            state.score = pre_score.0;
            state.penalty = pre_score.1;
        }
    }

    pub struct DeleteRoute {
        start: usize, // copy元
        end: usize,
    }
    impl DeleteRoute {
        pub fn new(input: &Input, state: &mut State) -> Self {
            assert!(state.pos.len() > 4);
            if state.need_update {
                state.update_from_pos(input);
                state.need_update = false;
            }

            for _ in 0..300 {
                let p = Coord::new(rnd::next() % input.n, rnd::next() % input.n);
                if state.pos_cell[p].len() < 2 {
                    continue;
                }
                let start = state.pos_cell[p][rnd::next() % (state.pos_cell[p].len() - 1)];
                let end = state.get_gt_fast(p, start).unwrap();
                assert!(start < end);
                if end - start > 30 {
                    continue;
                }
                let mut zero_visit = false;
                for &p in state.pos[start + 1..=end].iter() {
                    assert!(state.visit[p] > 0);
                    state.visit[p] -= 1;
                    if state.visit[p] == 0 {
                        zero_visit = true;
                    }
                }
                for &p in state.pos[start + 1..=end].iter() {
                    state.visit[p] += 1;
                }
                if zero_visit {
                    continue;
                }
                return Self { start, end };
            }
            Self { start: !0, end: !0 }
        }
    }
    impl Neighbor for DeleteRoute {
        fn preprocess(&mut self, input: &mut Input, state: &mut State) -> bool {
            if self.start == !0 {
                return false;
            }
            // visit check

            let henkawidth = self.end - self.start;
            let newpos = vec![];
            let (prev, next) =
                state.calc_sabun_for_change(input, self.start + 1, self.end, &newpos);

            state.score = (state.score * state.pos.len() as f64 + next as f64 - prev as f64)
                / (state.pos.len() - henkawidth) as f64;

            true
        }
        fn postprocess(&self, _input: &Input, state: &mut State) {
            state.need_update = true;
            for &p in state.pos[self.start + 1..=self.end].iter() {
                state.visit[p] -= 1;
            }
            let _ = state
                .pos
                .splice(self.start + 1..=self.end, vec![])
                .collect::<Vec<_>>();
        }
        fn rollback(&self, _input: &Input, state: &mut State, pre_score: (f64, f64)) {
            state.score = pre_score.0;
            state.penalty = pre_score.1;
        }
    }
}

fn make_valid_pos(input: &Input, pos: &mut Vec<Coord>, visit: &mut Map2d<bool>, p: Coord) {
    visit[p] = true;
    for &idx in input.random_idx[input.neighbor[p].len()]
        [rnd::next() % input.random_idx[input.neighbor[p].len()].len()]
    .iter()
    {
        let np = input.neighbor[p][idx];
        if visit[np] {
            continue;
        }
        pos.push(np);
        make_valid_pos(input, pos, visit, np);
        pos.push(p);
    }
}

pub struct Input {
    n: usize,
    neighbor: Map2d<Vec<Coord>>,
    d: Map2d<usize>, // 総和が100000になるように正規化
    real_d: Map2d<usize>,
    sumd: usize,
    memo: Map2d<(u8, usize, u8, bool, usize)>,
    random_idx: Vec<Vec<Vec<usize>>>,
    is_aps: Map2d<bool>,
    w: usize, // 壁の数
}

fn read_input() -> Input {
    input! {
        n: usize,
        h: [Chars; n-1],
        v: [Chars; n],
        d: [usize; n*n],
    }
    let mut w = 0;
    for (i, j) in iproduct!(0..n, 0..n - 1) {
        if v[i][j] == '1' {
            w += 1;
        }
    }
    for (i, j) in iproduct!(0..n - 1, 0..n) {
        if h[i][j] == '1' {
            w += 1;
        }
    }
    let mut neighbor = Map2d::new(vec![vec![]; n * n], n);
    for (x, y) in iproduct!(0..n, 0..n) {
        let p = Coord::new(x, y);
        if y > 0 && v[x][y - 1] == '0' {
            neighbor[p].push(Coord::new(x, y - 1));
        }
        if x > 0 && h[x - 1][y] == '0' {
            neighbor[p].push(Coord::new(x - 1, y));
        }
        if y < n - 1 && v[x][y] == '0' {
            neighbor[p].push(Coord::new(x, y + 1));
        }
        if x < n - 1 && h[x][y] == '0' {
            neighbor[p].push(Coord::new(x + 1, y));
        }
    }
    let sumd = d.iter().sum::<usize>();
    let seiki_sumd = 100000;
    let mut d = Map2d::new(d, n);
    let real_d = d.clone();
    let mut newsumd = 0;
    for p in iproduct!(0..n, 0..n).map(|t| Coord::new(t.0, t.1)) {
        d[p] = max(
            1,
            (d[p] as f64 * seiki_sumd as f64 / sumd as f64).round() as usize,
        );
        newsumd += d[p];
    }
    let sumd = newsumd;

    let memo = Map2d::new(vec![(0, !0, 0, false, 0); n * n], n);
    let mut random_idx = vec![vec![]; 5];
    for i in 1..=4 {
        for v in (0..i).permutations(i) {
            random_idx[i].push(v);
        }
    }
    let mut lowlink = Lowlink::new(n);
    let mut ret = Input {
        n,
        neighbor,
        d,
        real_d,
        sumd,
        memo,
        random_idx,
        is_aps: Map2d::new(vec![false], 1),
        w,
    };
    lowlink.calc_aps(&ret);
    ret.is_aps = lowlink.aps;
    crate::print_data!(n);
    crate::print_data!(w);
    ret
}

struct Output {
    score: f64,
    actions: Vec<Coord>,
}

impl Output {
    fn print_ans(&self, input: &Input) {
        if IS_LOCAL {
            let mut visit = Map2d::new(vec![false; input.n * input.n], input.n);
            let mut p = Coord::new(0, 0);
            let mut ans = "".to_string();
            for &np in self.actions.iter() {
                visit[np] = true;
                ans += if p.y == np.y + 1 {
                    assert_eq!(p.x, np.x);
                    &"L"
                } else if p.x == np.x + 1 {
                    assert_eq!(p.y, np.y);
                    &"U"
                } else if p.y + 1 == np.y {
                    assert_eq!(p.x, np.x);
                    &"R"
                } else if p.x + 1 == np.x {
                    assert_eq!(p.y, np.y);
                    &"D"
                } else {
                    unreachable!("{} {} {} {}", p.x, p.y, np.x, np.y);
                };
                p = np;
            }
            assert_eq!(p, Coord::new(0, 0));
            for (x, y) in iproduct!(0..input.n, 0..input.n) {
                assert_eq!(visit[Coord::new(x, y)], true);
            }
            println!("{}", ans);
            let score = self.score.round();
            crate::print_data!(score);
        } else {
            let mut p = Coord::new(0, 0);
            let mut ans = "".to_string();
            for &np in self.actions.iter() {
                ans += if p.y == np.y + 1 {
                    &"L"
                } else if p.x == np.x + 1 {
                    &"U"
                } else if p.y + 1 == np.y {
                    &"R"
                } else if p.x + 1 == np.x {
                    &"D"
                } else {
                    unreachable!("{} {} {} {}", p.x, p.y, np.x, np.y);
                };
                p = np;
            }
            println!("{}", ans);
            let score = self.score.round();
            crate::print_data!(score);
            std::process::exit(0);
        }
    }
}

// ここからライブラリ
#[macro_export]
macro_rules! print_data {
    ( $($x:expr),* ) => {{
        $(eprintln!("[DATA] {} = {:?}", stringify!($x), $x);)*
    }}
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

#[allow(unused)]
mod rnd {
    static mut S: usize = 88172645463325252;

    pub fn next() -> usize {
        unsafe {
            S ^= S << 7;
            S ^= S >> 9;
            S
        }
    }

    pub fn nextf() -> f64 {
        f64::from_bits((0x3ff0000000000000 | next() & 0xfffffffffffff) as u64) - 1.
    }

    pub fn range(a: usize, b: usize) -> usize {
        next() % (b - a) + a
    }

    pub fn exu(a: usize, b: usize, skip: usize) -> usize {
        assert!(a <= skip && skip < b);
        let ret = range(a, b - 1);

        ret + (skip <= ret) as usize
    }

    pub fn rangei(a: i64, b: i64) -> i64 {
        (next() % ((b - a) as usize)) as i64 + a
    }

    pub fn shuffle<T>(list: &mut [T]) {
        for i in (0..list.len()).rev() {
            list.swap(i, next() % (i + 1));
        }
    }
}

#[allow(dead_code)]
mod grid {
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

        pub const fn origin() -> Self {
            Coord::new(0, 0)
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

#[derive(Clone)]
// 関節点を列挙
struct Lowlink {
    used: Map2d<bool>,
    ord: Map2d<usize>,
    low: Map2d<usize>,
    aps: Map2d<bool>,
}
impl Lowlink {
    fn new(n: usize) -> Lowlink {
        let used = Map2d::new(vec![false; n * n], n);
        let ord = Map2d::new(vec![0; n * n], n);
        let low = Map2d::new(vec![0; n * n], n);
        let aps = Map2d::new(vec![false; n * n], n);
        Lowlink {
            used,
            ord,
            low,
            aps,
        }
    }
    fn calc_aps(&mut self, input: &Input) {
        self.used.fill(false);
        self.ord.fill(0);
        self.low.fill(0);
        self.aps.fill(false);

        let k = 0;
        let null = Coord::new(0, !0);
        self.dfs(input, Coord::origin(), k, null);
    }

    fn dfs(&mut self, input: &Input, p: Coord, mut k: usize, par: Coord) -> usize {
        self.used[p] = true;
        self.ord[p] = k;
        k += 1;
        self.low[p] = self.ord[p];
        let mut is_aps = false;
        let mut count = 0;

        for &np in input.neighbor[p].iter() {
            if !self.used[np] {
                count += 1;
                k = self.dfs(input, np, k, p);
                self.low[p] = min(self.low[p], self.low[np]);
                if par.y != !0 && self.ord[p] <= self.low[np] {
                    is_aps = true;
                }
            } else if np != par {
                self.low[p] = min(self.low[p], self.ord[np]);
            }
        }

        if par.y == !0 && count >= 2 {
            is_aps = true;
        }
        if is_aps {
            self.aps[p] = true;
        }

        k
    }
}

// x1 1000case 1279 15.916
// x1.2 1000   1279 15.915
// x2 1000case 1272 15.912
// x5 1000case 1258 15.903
