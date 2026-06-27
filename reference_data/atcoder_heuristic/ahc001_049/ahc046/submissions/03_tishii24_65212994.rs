pub mod util {
    pub mod rnd {
        static mut S: usize = 88172645463325252;

        #[inline]
        #[allow(unused)]
        pub fn next() -> usize {
            unsafe {
                S = S ^ S << 7;
                S = S ^ S >> 9;
                S
            }
        }

        #[inline]
        #[allow(unused)]
        pub fn nextf() -> f64 {
            (next() & 4294967295) as f64 / 4294967296.
        }

        #[inline]
        #[allow(unused)]
        pub fn gen_rangef(low: f64, high: f64) -> f64 {
            nextf() * (high - low) + low
        }

        #[inline]
        #[allow(unused)]
        pub fn gen_range(low: usize, high: usize) -> usize {
            assert!(low < high);
            (next() % (high - low)) + low
        }

        #[inline]
        #[allow(unused)]
        pub fn gen_index(len: usize) -> usize {
            next() % len
        }

        #[allow(unused)]
        pub fn shuffle<I>(vec: &mut Vec<I>) {
            for i in 0..vec.len() {
                let j = gen_range(0, vec.len());
                vec.swap(i, j);
            }
        }
    }

    pub mod time {
        static mut START: f64 = -1.;

        #[allow(unused)]
        pub fn start_clock() {
            let _ = elapsed_seconds();
        }

        #[inline]
        #[allow(unused)]
        pub fn elapsed_seconds() -> f64 {
            let t = std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap()
                .as_secs_f64();
            unsafe {
                if START < 0. {
                    START = t;
                }
                #[cfg(feature = "local")]
                {
                    return (t - START) * 1.5;
                }
                #[cfg(feature = "gce")]
                {
                    return (t - START);
                }
                t - START
            }
        }
    }
}

use std::collections::BinaryHeap;

use crate::util::*;
use proconio::{input, source::auto};

const N: usize = 20;
const M: usize = 40;
const D: [(usize, usize); 4] = [(!0, 0), (1, 0), (0, !0), (0, 1)];
const DIR: [char; 4] = ['U', 'D', 'L', 'R'];

fn get_dir_idx(from: (usize, usize), to: (usize, usize)) -> usize {
    if from.1 == to.1 {
        if from.0 > to.0 {
            0
        } else {
            1
        }
    } else {
        if from.1 > to.1 {
            2
        } else {
            3
        }
    }
}

fn get_dist(from: (usize, usize), to: (usize, usize)) -> usize {
    return from.0.abs_diff(to.0) + from.1.abs_diff(to.1);
}

fn is_between(v: (usize, usize), from: (usize, usize), to: (usize, usize)) -> bool {
    if from.0 == to.0 {
        // vertical line
        let min_y = from.1.min(to.1);
        let max_y = from.1.max(to.1);
        from.0 == v.0 && (min_y..=max_y).contains(&v.1)
    } else if from.1 == to.1 {
        // horizontal line
        let min_x = from.0.min(to.0);
        let max_x = from.0.max(to.0);
        from.1 == v.1 && (min_x..=max_x).contains(&v.0)
    } else {
        // not a straight line
        false
    }
}

#[derive(Clone, Copy)]
struct Tile {
    last_t: Option<usize>,
    is_block: bool,
}

#[derive(Clone)]
struct State {
    cur_cost: usize,
    path: Vec<(usize, usize)>,
    // is_set, t, block_p
    block_act: Vec<(bool, usize, (usize, usize))>,
    tiles: [[Tile; N]; N],
}

impl State {
    fn new(start: (usize, usize)) -> State {
        let mut state = State {
            cur_cost: 0,
            path: vec![],
            block_act: vec![],
            tiles: [[Tile {
                last_t: None,
                is_block: false,
            }; N]; N],
        };
        state.add_v(start);
        state
    }

    fn add_path(&mut self, path: Vec<(usize, usize)>) {
        let mut cur = *self.path.last().unwrap();
        for v in path {
            let dist = get_dist(cur, v);

            // cur <-> v までのlast_t、is_blockを消す
            let dir_idx = get_dir_idx(cur, v);
            let dir = D[dir_idx];
            while cur != v {
                cur = (cur.0 + dir.0, cur.1 + dir.1);

                // is_blockなら、last_tの時に消す
                if self.tiles[cur.0][cur.1].is_block {
                    let last_t = self.tiles[cur.0][cur.1].last_t.unwrap();
                    self.block_act.push((false, last_t, cur));
                }

                self.tiles[cur.0][cur.1].last_t = None;
                self.tiles[cur.0][cur.1].is_block = false;
            }

            // blockを置いて止まったなら、is_blockを更新する
            if dist > 1 {
                let nv = (cur.0 + dir.0, cur.1 + dir.1);
                if nv.0 < N && nv.1 < N && !self.tiles[nv.0][nv.1].is_block {
                    let last_t = self.tiles[nv.0][nv.1].last_t.unwrap();
                    self.tiles[nv.0][nv.1].is_block = true;
                    self.block_act.push((true, last_t, nv));
                }
            }

            self.add_v(v);
        }
    }

    fn add_v(&mut self, v: (usize, usize)) {
        for d in D {
            let u = (v.0 + d.0, v.1 + d.1);
            if u.0 >= N || u.1 >= N {
                continue;
            }
            self.tiles[u.0][u.1].last_t = Some(self.path.len());
        }
        self.path.push(v);
    }

    fn output(&mut self) {
        self.path.insert(0, self.path[0]);
        for i in 0..self.path.len() - 1 {
            let from = self.path[i];
            let to = self.path[i + 1];
            let dist = get_dist(from, to);
            let dir_char = DIR[get_dir_idx(from, to)];
            if dist == 1 {
                // move
                println!("M {}", dir_char);
            } else if dist > 1 {
                // slide
                println!("S {}", dir_char);
            }

            for &(_, t, block_p) in self.block_act.iter() {
                if t != i {
                    continue;
                }
                let dist = get_dist(to, block_p);
                assert!(dist == 1);
                let dir_char = DIR[get_dir_idx(to, block_p)];
                println!("A {}", dir_char);
            }
        }
    }
}

fn search(
    state: &State,
    from: (usize, usize),
    to: (usize, usize),
    p: f64,
) -> (usize, Vec<(usize, usize)>) {
    // TODO: A*
    let mut q = BinaryHeap::new();
    let mut dist = [[(1 << 30, None); N]; N];
    dist[from.0][from.1] = (0, None);
    q.push((
        std::cmp::Reverse(dist[from.0][from.1].0),
        std::cmp::Reverse(0),
        from,
        vec![from],
    ));

    let mut well_paths = None;
    while let Some((std::cmp::Reverse(d), std::cmp::Reverse(actual_cost), v, cur_path)) = q.pop() {
        if d > dist[v.0][v.1].0 {
            continue;
        }

        // get path
        if v == to {
            well_paths = Some((actual_cost, cur_path));
            break;
        }

        // move
        for d in D {
            let nv = (v.0 + d.0, v.1 + d.1);
            if nv.0 >= N || nv.1 >= N {
                continue;
            }
            let cost = if state.tiles[nv.0][nv.1].is_block {
                // 壊すブロックの数だけコストは+1
                2
            } else {
                1
            };
            let random_cost = if rnd::nextf() < p {
                rnd::gen_range(1, 5)
            } else {
                0
            };
            let new_dist = dist[v.0][v.1].0 + 10 * cost + random_cost;
            if dist[nv.0][nv.1].0 <= new_dist {
                continue;
            }
            dist[nv.0][nv.1] = (new_dist, Some(v));
            let mut new_path = cur_path.clone();
            new_path.push(nv);
            q.push((
                std::cmp::Reverse(dist[nv.0][nv.1].0),
                std::cmp::Reverse(actual_cost + cost),
                nv,
                new_path,
            ));
        }

        // slide
        for d in D {
            let mut nv = (v.0 + d.0, v.1 + d.1);
            let mut cost = 1;
            'a: while nv.0 < N && nv.1 < N {
                // 壊すブロックの数だけコストは+1
                if state.tiles[nv.0][nv.1].is_block {
                    cost += 1;
                }

                let next_nv = (nv.0 + d.0, nv.1 + d.1);

                // ブロックを作って止まれるかどうか
                for i in 0..cur_path.len() - 1 {
                    let from = cur_path[i];
                    let to = cur_path[i + 1];
                    if is_between(next_nv, from, to) {
                        nv = (nv.0 + d.0, nv.1 + d.1);
                        continue 'a;
                    }
                }

                // ブロックを作るコスト
                let new_block_cost = if next_nv.0 >= N
                    || next_nv.1 >= N
                    || state.tiles[next_nv.0][next_nv.1].is_block
                {
                    // 壁である・すでにブロックがある
                    0
                } else if state.tiles[next_nv.0][next_nv.1].last_t.is_some() {
                    // ブロックを置ける
                    1
                } else {
                    nv = (nv.0 + d.0, nv.1 + d.1);
                    continue;
                };

                let random_cost = if rnd::nextf() < p {
                    rnd::gen_range(1, 5)
                } else {
                    0
                };
                let new_dist = dist[v.0][v.1].0 + (cost + new_block_cost) * 10 + random_cost;
                if dist[nv.0][nv.1].0 <= new_dist {
                    nv = (nv.0 + d.0, nv.1 + d.1);
                    continue;
                }

                dist[nv.0][nv.1] = (new_dist, Some(v));
                let mut new_path = cur_path.clone();
                new_path.push(nv);
                q.push((
                    std::cmp::Reverse(dist[nv.0][nv.1].0),
                    std::cmp::Reverse(actual_cost + new_block_cost + cost),
                    nv,
                    new_path,
                ));
                nv = (nv.0 + d.0, nv.1 + d.1);
            }
        }
    }

    assert!(dist[to.0][to.1].0 < 1 << 30);

    well_paths.unwrap()
}

fn main() {
    input! {
        _: usize,
        _: usize,
        points: [(usize, usize); M]
    }

    const BEAM_WIDTH: usize = 40;
    let mut states = vec![State::new(points[0])];

    for i in 0..M - 1 {
        eprintln!("{}", states.len());

        let mut next_info = vec![];
        for (state_idx, state) in states.iter().enumerate() {
            // (new_cost, state_idx, path)
            let from = points[i];
            let to = points[i + 1];

            // 始点を含まない
            for p in [0., 0.5, 0.5, 0.5, 0.5, 0.5] {
                let (add_cost, path) = search(&state, from, to, p);
                next_info.push((state.cur_cost + add_cost, state_idx, path));
            }
        }

        next_info.sort_by_key(|a| a.0);
        let mut path_set = std::collections::HashSet::new();
        let mut state_count: std::collections::HashMap<usize, usize> =
            std::collections::HashMap::new();
        let mut next_states = vec![];

        for (new_cost, state_idx, path) in next_info
            .iter()
            .filter(|(_, state_idx, path)| {
                state_count.insert(*state_idx, state_count.get(state_idx).unwrap_or(&0) + 1);
                path_set.insert(path) || state_count[state_idx] <= 1
            })
            .take(BEAM_WIDTH)
            .cloned()
        {
            eprintln!("{} {} {:?}", new_cost, state_idx, path);
            let mut new_state = states[state_idx].clone();

            // 更新する
            new_state.add_path(path);
            new_state.cur_cost = new_cost;
            next_states.push(new_state);
        }

        states = next_states;
    }

    states.sort_by_key(|a| a.cur_cost);
    states[0].output();
}

/*
通った時に、先のためにブロックを置いておく・どかしておくことができる
先に経路を決める？
始点と終点は確定している
ある程度独立に最適化できる
遅延評価
ビムサかなー、、

状態として何を持てば良いか
1. 隣接したタイミング or None
    - Noneは通ったタイミング
2. ブロックがあるかどうか

使ったタイミングで必ず外せる
どうやって状態を持つか

commands: Vec<Command>;
tile: Vec<Vec<Tile>>;

struct Command {
    from: (usize, usize),
    to: (usize, usize),
}

struct Tile {
    last_t: Option<usize>,
    is_block: bool,
}
*/
