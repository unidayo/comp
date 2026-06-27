pub mod def {
    use itertools::iproduct;
    use proconio::input;
    use std::collections::VecDeque;

    use crate::util::*;

    pub const INF: i64 = 1_000_000_000;
    pub const COST_FOR_STATION: i64 = 5_000;
    pub const COST_FOR_RAIL: i64 = 100;
    pub const D: [Coor; 4] = [
        Coor { i: 0, j: 1 },
        Coor { i: 1, j: 0 },
        Coor { i: 0, j: !0 },
        Coor { i: !0, j: 0 },
    ];
    pub const D_STATION: [Coor; 13] = [
        Coor { i: !1, j: 0 },
        Coor { i: !0, j: !0 },
        Coor { i: !0, j: 0 },
        Coor { i: !0, j: 1 },
        Coor { i: 0, j: !1 },
        Coor { i: 0, j: !0 },
        Coor { i: 0, j: 0 },
        Coor { i: 0, j: 1 },
        Coor { i: 0, j: 2 },
        Coor { i: 1, j: !0 },
        Coor { i: 1, j: 0 },
        Coor { i: 1, j: 1 },
        Coor { i: 2, j: 0 },
    ];

    /// 繋がっている方向を表すbit列をもとに、問題の線路の番号を返す関数
    /// 方向はDと対応する
    pub fn rail_bit_to_rail_index(bit: usize) -> usize {
        match bit {
            0b0011 => 6,
            0b0101 => 1,
            0b1001 => 5,
            0b0110 => 3,
            0b1010 => 2,
            0b1100 => 4,
            _ => panic!("invalid bit: {}", bit),
        }
    }

    #[repr(C)]
    #[derive(PartialEq, Eq, Copy, Clone, Debug)]
    pub struct Coor {
        pub i: usize,
        pub j: usize,
    }

    impl Coor {
        pub fn is_valid(&self, n: usize) -> bool {
            self.i < n && self.j < n
        }

        pub fn add(&self, d: &Coor) -> Coor {
            Coor {
                i: self.i + d.i,
                j: self.j + d.j,
            }
        }

        pub fn sub(&self, d: &Coor) -> Coor {
            Coor {
                i: self.i - d.i,
                j: self.j - d.j,
            }
        }
    }

    #[inline]
    pub fn dist(a: &Coor, b: &Coor) -> i64 {
        (a.i.abs_diff(b.i) + a.j.abs_diff(b.j)) as i64
    }

    #[inline]
    pub fn region_size(s: &Coor, t: &Coor) -> i64 {
        let (min_c, max_c) = get_region(s, t);
        ((max_c.i - min_c.i) * (max_c.j - min_c.j)) as i64
    }

    #[inline]
    pub fn get_region(c1: &Coor, c2: &Coor) -> (Coor, Coor) {
        let min_c = Coor {
            i: c1.i.min(c2.i),
            j: c1.j.min(c2.j),
        };
        let max_c = Coor {
            i: c1.i.max(c2.i),
            j: c1.j.max(c2.j),
        };
        (min_c, max_c)
    }

    pub fn is_in_region(c: &Coor, s: &Coor, t: &Coor, include_c: bool) -> bool {
        if include_c && (c == s || c == t) {
            return false;
        }
        let (min_c, max_c) = get_region(s, t);
        min_c.i <= c.i && c.i <= max_c.i && min_c.j <= c.j && c.j <= max_c.j
    }

    pub struct Param {
        pub alpha_low: f64,
        pub alpha: f64,
        pub required: i64,
        pub greedy_rail: i64,
        pub s_range: f64,
        pub start_temp: f64,
        pub end_temp: f64,
        pub beam_width: usize,
        pub beam_size: usize,
    }

    pub struct Input {
        pub n: usize,
        pub m: usize,
        pub k: i64,
        pub t: i64,
        pub coors: Vec<[Coor; 2]>,
        pub scores: Vec<i64>,
        // points[i][j] := (i, j) に駅を作ることで到達可能な点（idx, is_s）の添字
        pub points: Vec<Vec<Vec<(usize, usize)>>>,
        pub hashes: Vec<u32>,
        pub p: Param,
    }

    impl Input {
        pub fn read_input() -> Input {
            input! {
                n: usize,
                m: usize,
                k: i64,
                t: i64,
                ps: [(usize, usize, usize, usize); m]
            }
            let coors: Vec<[Coor; 2]> = ps
                .into_iter()
                .map(|x| [Coor { i: x.0, j: x.1 }, Coor { i: x.2, j: x.3 }])
                .collect();
            let scores = coors.iter().map(|a| dist(&a[0], &a[1])).collect();
            let mut points = vec![vec![vec![]; n]; n];
            for (idx, [s, t]) in coors.iter().enumerate() {
                for d in D_STATION {
                    let sd = s.add(&d);
                    let td = t.add(&d);
                    if sd.is_valid(n) {
                        points[sd.i][sd.j].push((idx, 0));
                    }
                    if td.is_valid(n) {
                        points[td.i][td.j].push((idx, 1));
                    }
                }
            }
            let hashes: Vec<u32> = (0..5_000).map(|_| rnd::next() as u32).collect();
            let params = Input::load_params();
            Input {
                n,
                m,
                k,
                t,
                coors,
                scores,
                points,
                hashes,
                p: params,
            }
        }

        fn load_params() -> Param {
            let load_from_cmd_args = false;
            if load_from_cmd_args {
                use std::env;
                let args: Vec<String> = env::args().collect();
                Param {
                    alpha_low: args[1].parse::<f64>().unwrap(),
                    alpha: args[2].parse::<f64>().unwrap(),
                    required: args[3].parse::<i64>().unwrap(),
                    greedy_rail: args[4].parse::<i64>().unwrap(),
                    s_range: args[5].parse::<f64>().unwrap(),
                    start_temp: args[6].parse::<f64>().unwrap(),
                    end_temp: args[7].parse::<f64>().unwrap(),
                    beam_width: args[8].parse::<usize>().unwrap(),
                    beam_size: args[9].parse::<usize>().unwrap(),
                }
            } else {
                Param {
                    alpha_low: 0.01,
                    alpha: 0.2,
                    required: 100,
                    greedy_rail: 3,
                    s_range: 0.4,
                    start_temp: 5e2,
                    end_temp: 5e4,
                    beam_width: 100_000,
                    beam_size: 100,
                }
            }
        }
    }

    #[derive(Clone, Copy, PartialEq, Eq, Debug)]
    pub enum Tile {
        None,
        Rail,
        Station,
    }

    pub struct Field {
        _field_id: usize,
        _tiles: Vec<Vec<(usize, Tile)>>,
    }

    impl Field {
        pub fn new(n: usize) -> Field {
            Field {
                _field_id: 0,
                _tiles: vec![vec![(0, Tile::None); n]; n],
            }
        }

        pub fn reset(&mut self) {
            self._field_id += 1;
        }

        pub fn add_rail_path(&mut self, path: &Vec<Coor>) {
            assert!(path.len() >= 2);

            // 両端は含まない
            for c in path[1..path.len() - 1].iter() {
                self.add_rail(c);
            }
        }

        pub fn remove_rail_path(&mut self, path: &Vec<Coor>) {
            assert!(path.len() >= 2);

            // 両端は含まない
            for c in path[1..path.len() - 1].iter() {
                self.remove_rail(c);
            }
        }

        #[inline]
        pub fn add_station(&mut self, c: &Coor) {
            assert!(!matches!(self.get_tile(c), Tile::Station));
            self.set_tile(c, Tile::Station);
        }

        #[inline]
        pub fn add_rail(&mut self, c: &Coor) {
            assert!(
                matches!(self.get_tile(c), Tile::None),
                "{:?} {:?}",
                c,
                self._tiles[c.i][c.j]
            );
            self.set_tile(c, Tile::Rail);
        }

        #[inline]
        pub fn remove_rail(&mut self, c: &Coor) {
            assert!(matches!(self.get_tile(c), Tile::Rail), "{:?}", c);
            self.set_tile(c, Tile::None);
        }

        #[inline]
        pub fn get_tile(&mut self, c: &Coor) -> Tile {
            self.reset_tile_if_needed(c);
            self._tiles[c.i][c.j].1
        }

        #[inline]
        pub fn set_tile(&mut self, c: &Coor, tile: Tile) {
            self.reset_tile_if_needed(c);
            self._tiles[c.i][c.j].1 = tile;
        }

        #[inline]
        fn reset_tile_if_needed(&mut self, c: &Coor) {
            if self._tiles[c.i][c.j].0 != self._field_id {
                self._tiles[c.i][c.j].0 = self._field_id;
                self._tiles[c.i][c.j].1 = Tile::None;
            }
        }

        #[allow(unused)]
        pub fn print(&mut self) {
            for i in 0..self._tiles.len() {
                for j in 0..self._tiles.len() {
                    match self.get_tile(&Coor { i, j }) {
                        Tile::None => eprint!("."),
                        Tile::Rail => eprint!("R"),
                        Tile::Station => eprint!("S"),
                    }
                }
                eprintln!()
            }
        }
    }

    pub struct PathFinder {
        search_id: usize,
        dist: Vec<Vec<(usize, i64)>>,
        q: VecDeque<Coor>,
        ds: Vec<Coor>,
    }

    impl PathFinder {
        pub fn new(n: usize) -> PathFinder {
            PathFinder {
                search_id: 0,
                dist: vec![vec![(0, INF); n]; n],
                q: VecDeque::new(),
                ds: vec![],
            }
        }

        pub fn find_path(&mut self, s: &Coor, t: &Coor, field: &mut Field) -> Option<(Vec<Coor>, i64)> {
            self.search_id += 1;

            self.q.clear();
            self.dist[s.i][s.j] = (self.search_id, 0);
            self.q.push_front(s.clone());

            // sからtへの方向のみ使える
            self.get_priority_ds(s, t);

            'a: while let Some(c) = self.q.pop_front() {
                for d in self.ds.iter() {
                    let nc = c.add(&d);
                    if !nc.is_valid(self.dist.len()) {
                        continue;
                    }
                    if self.search_id == self.dist[nc.i][nc.j].0
                        && self.dist[nc.i][nc.j].1 <= self.dist[c.i][c.j].1 + 1
                    {
                        continue;
                    }

                    if field.get_tile(&nc) != Tile::None && &nc != t {
                        continue;
                    }
                    self.dist[nc.i][nc.j] = (self.search_id, self.dist[c.i][c.j].1 + 1);
                    if &nc == t {
                        break 'a;
                    }
                    self.q.push_back(nc);
                }
            }

            if self.dist[t.i][t.j].0 != self.search_id {
                return None;
            }

            let path = self.restore_path(s, &t);
            Some((path, self.dist[t.i][t.j].1))
        }

        pub fn find_path_to_closest_station(
            &mut self,
            s: &Coor,
            stations: &Vec<Coor>,
            field: &mut Field,
        ) -> Option<(Vec<Coor>, i64)> {
            self.search_id += 1;

            // 駅を置こうとしている地点が線路になっているなら、そのまま駅をおいて良い
            if field.get_tile(s) == Tile::Rail {
                return None;
            }

            let mut q = VecDeque::new();
            self.dist[s.i][s.j] = (self.search_id, 0);
            q.push_front(s.clone());
            'a: while let Some(c) = q.pop_front() {
                for d in D {
                    let nc = c.add(&d);
                    if !nc.is_valid(self.dist.len()) {
                        continue;
                    }
                    if self.search_id == self.dist[nc.i][nc.j].0
                        && self.dist[nc.i][nc.j].1 <= self.dist[c.i][c.j].1 + 1
                    {
                        continue;
                    }

                    // 最短経路になっていないなら探索を進めない
                    // NOTE: 最後の方は許容した方が良いかも
                    if dist(s, &nc) != self.dist[c.i][c.j].1 + 1 {
                        continue;
                    }
                    if field.get_tile(&nc) == Tile::Rail {
                        continue;
                    }
                    self.dist[nc.i][nc.j] = (self.search_id, self.dist[c.i][c.j].1 + 1);
                    if field.get_tile(&nc) == Tile::Station {
                        break 'a;
                    }
                    q.push_back(nc);
                }
            }

            let Some((dist_t, closest_t_idx)) = stations
                .iter()
                .enumerate()
                .filter(|(_, s)| self.dist[s.i][s.j].0 == self.search_id)
                .map(|(i, t)| (self.dist[t.i][t.j].1, i))
                .min()
            else {
                // 到達不可能
                return None;
            };

            let path = self.restore_path(s, &stations[closest_t_idx]);
            Some((path, dist_t))
        }

        fn restore_path(&mut self, s: &Coor, t: &Coor) -> Vec<Coor> {
            assert!(self.dist[t.i][t.j].1 < INF);
            assert!(self.dist[t.i][t.j].0 == self.search_id);
            let mut cur = t.clone();
            let mut path = vec![cur.clone()];
            while &cur != s {
                self.get_priority_ds(&cur, &s);
                for d in self.ds.iter() {
                    let next = cur.add(&d);
                    if !next.is_valid(self.dist.len()) {
                        continue;
                    }
                    if self.search_id == self.dist[next.i][next.j].0
                        && self.dist[next.i][next.j].1 + 1 == self.dist[cur.i][cur.j].1
                    {
                        path.push(next);
                        cur = next;
                    }
                }
            }
            path.reverse();
            path
        }

        fn get_priority_ds(&mut self, s: &Coor, t: &Coor) {
            // TODO: refactor
            self.ds.clear();
            let mut a = None;
            for d in D {
                let ns = s.add(&d);
                if !ns.is_valid(self.dist.len()) {
                    continue;
                }
                if dist(&ns, &t) >= dist(&s, &t) {
                    continue;
                }
                let e = ns.i.abs_diff(t.i).max(ns.j.abs_diff(t.j));
                if let Some(a) = a {
                    if e < a {
                        self.ds.insert(0, d);
                        break;
                    }
                }
                a = Some(e);
                self.ds.push(d);
            }
            assert!(self.ds.len() <= 2, "{:?} {:?} {:?}", s, t, self.ds);
        }
    }

    pub fn path_to_commands(path: &Vec<Coor>) -> Vec<(usize, Coor)> {
        let mut cmds = vec![];
        if path.len() <= 1 {
            return cmds;
        }
        for i in 1..path.len() - 1 {
            let d1 = path[i - 1].sub(&path[i]);
            let d2 = path[i + 1].sub(&path[i]);
            let i1 = D.iter().position(|x| x == &d1).unwrap();
            let i2 = D.iter().position(|x| x == &d2).unwrap();
            let p = rail_bit_to_rail_index((1 << i1) | (1 << i2));
            cmds.push((p, path[i]));
        }
        cmds
    }

    #[derive(Clone, Debug)]
    pub struct Status {
        status_id: usize,
        /// addedを更新するかどうか
        /// 局所探索など削除が発生する場合は速度面で更新しない方が良い
        track_added: bool,
        /// dsを更新するかどうか
        track_ds: bool,
        pub s: i64,
        pub potential_s: i64,
        pub added: Vec<(usize, usize)>,
        // (status_id, ds, potential_ds)
        ds: Vec<Vec<(usize, i64, i64)>>,
        base_ds: Vec<Vec<i64>>,
        is_station: Vec<Vec<(usize, bool)>>,
        // (status_id, count)
        count: Vec<[(usize, usize); 2]>,
    }

    impl Status {
        pub fn new(input: &Input, track_added: bool) -> Status {
            let mut base_ds = vec![vec![0; input.n]; input.n];
            for (i, j) in iproduct!(0..input.n, 0..input.n) {
                for &(idx, _) in input.points[i][j].iter() {
                    base_ds[i][j] += input.scores[idx];
                }
            }

            Status {
                status_id: 0,
                track_added,
                track_ds: false,
                s: 0,
                potential_s: 0,
                added: Vec::with_capacity(2_500),
                ds: vec![],
                base_ds,
                is_station: vec![vec![(0, false); input.n]; input.n],
                count: vec![[(0, 0), (0, 0)]; input.m],
            }
        }

        pub fn reset(&mut self) {
            self.status_id += 1;
            self.s = 0;
            self.potential_s = 0;
            self.added.clear();
        }

        pub fn to_track_ds_status(mut self, input: &Input) -> Status {
            self.track_ds = true;
            self.ds = vec![vec![(0, 0, 0); input.n]; input.n];
            for (i, j) in iproduct!(0..input.n, 0..input.n) {
                let c = Coor { i, j };
                let (ds, potential_ds) = self.calc_ds(&c, input);
                self.ds[i][j] = (self.status_id, ds, potential_ds);
            }
            self
        }

        /// NOTE: 最後に駅を繋ぐことを前提としている
        pub fn register_station(&mut self, c: Coor, input: &Input) {
            self.set_is_station(&c, true);
            for &(idx, is_s) in input.points[c.i][c.j].iter() {
                let ds = input.scores[idx];
                if !self.is_added(idx, is_s) {
                    if self.track_added {
                        self.added.push((idx, is_s));
                    }
                    if self.is_added(idx, 1 - is_s) {
                        self.s += ds;
                        self.potential_s -= ds;
                        self.update_ds(&input.coors[idx][is_s], -ds, 0, input);
                    } else {
                        self.potential_s += ds;
                        self.update_ds(&input.coors[idx][is_s], 0, -ds, input);
                        self.update_ds(&input.coors[idx][1 - is_s], ds, -ds, input);
                    }
                }
                self.add_count(idx, is_s);
            }
        }

        /// IMPORTANT:
        /// self.addedの更新はしていないので、`self.track_added = true`の場合は外側でする必要がある
        pub fn unregister_station(&mut self, c: Coor, input: &Input) {
            self.set_is_station(&c, false);
            for &(idx, is_s) in input.points[c.i][c.j].iter() {
                let ds = input.scores[idx];
                if self.get_count(idx, is_s) == 1 {
                    if self.is_added(idx, 1 - is_s) {
                        self.s -= ds;
                        self.potential_s += ds;
                        self.update_ds(&input.coors[idx][is_s], ds, 0, input);
                    } else {
                        self.potential_s -= ds;
                        self.update_ds(&input.coors[idx][is_s], 0, ds, input);
                        self.update_ds(&input.coors[idx][1 - is_s], -ds, ds, input);
                    }
                }
                self.sub_count(idx, is_s);
            }
        }

        pub fn calc_ds(&mut self, c: &Coor, input: &Input) -> (i64, i64) {
            let mut ds = 0;
            let mut potential_ds = 0;
            for &(idx, is_s) in input.points[c.i][c.j].iter() {
                if !self.is_added(idx, is_s) {
                    if self.is_added(idx, 1 - is_s) {
                        ds += input.scores[idx];
                    } else {
                        potential_ds += input.scores[idx];
                    }
                }
            }
            (ds, potential_ds)
        }

        #[inline]
        pub fn is_added(&mut self, idx: usize, is_s: usize) -> bool {
            self.get_count(idx, is_s) > 0
        }

        #[inline]
        pub fn get_count(&mut self, idx: usize, is_s: usize) -> usize {
            self.reset_count_if_needed(idx, is_s);
            self.count[idx][is_s].1
        }

        #[inline]
        pub fn add_count(&mut self, idx: usize, is_s: usize) {
            self.reset_count_if_needed(idx, is_s);
            self.count[idx][is_s].1 += 1;
        }

        #[inline]
        pub fn sub_count(&mut self, idx: usize, is_s: usize) {
            self.reset_count_if_needed(idx, is_s);
            self.count[idx][is_s].1 -= 1;
        }

        #[inline]
        fn reset_count_if_needed(&mut self, idx: usize, is_s: usize) {
            if self.count[idx][is_s].0 != self.status_id {
                self.count[idx][is_s].0 = self.status_id;
                self.count[idx][is_s].1 = 0;
            }
        }

        #[inline]
        pub fn get_is_station(&mut self, c: &Coor) -> bool {
            self.reset_is_station_if_needed(c);
            self.is_station[c.i][c.j].1
        }

        #[inline]
        pub fn set_is_station(&mut self, c: &Coor, value: bool) {
            self.is_station[c.i][c.j] = (self.status_id, value);
        }

        #[inline]
        fn reset_is_station_if_needed(&mut self, c: &Coor) {
            if self.is_station[c.i][c.j].0 != self.status_id {
                self.is_station[c.i][c.j] = (self.status_id, false);
            }
        }

        #[inline]
        pub fn update_ds(&mut self, center_c: &Coor, ds: i64, potential_ds: i64, input: &Input) {
            if !self.track_ds {
                return;
            }
            for d in D_STATION {
                let c = center_c.add(&d);
                if !c.is_valid(input.n) {
                    continue;
                }
                self.add_ds(&c, ds, potential_ds);
            }
        }

        /// returns: (ds, potential_ds)
        #[inline]
        pub fn get_ds(&mut self, c: &Coor) -> (i64, i64) {
            self.reset_ds_if_needed(c);
            (self.ds[c.i][c.j].1, self.ds[c.i][c.j].2)
        }

        #[inline]
        pub fn add_ds(&mut self, c: &Coor, ds: i64, potential_ds: i64) {
            self.reset_ds_if_needed(c);
            self.ds[c.i][c.j].1 += ds;
            self.ds[c.i][c.j].2 += potential_ds;
        }

        #[inline]
        fn reset_ds_if_needed(&mut self, c: &Coor) {
            if self.ds[c.i][c.j].0 != self.status_id {
                self.ds[c.i][c.j] = (self.status_id, 0, self.base_ds[c.i][c.j]);
            }
        }
    }

    #[allow(unused)]
    pub fn assert_status_eq(status: &mut Status, expected: &mut Status, input: &Input) {
        for (i, j) in iproduct!(0..input.n, 0..input.n) {
            let c = Coor { i, j };
            assert_eq!(status.get_ds(&c), expected.get_ds(&c), "{:?}", c,);
            assert_eq!(
                status.get_is_station(&c),
                expected.get_is_station(&c),
                "{:?}",
                c,
            );
        }
        for (idx, is_s) in iproduct!(0..input.coors.len(), 0..2) {
            assert_eq!(
                status.get_count(idx, is_s),
                expected.get_count(idx, is_s),
                "{:?}",
                input.coors[idx],
            );
        }
    }
}
pub mod itertools_clone {
    use core::cmp::Ordering;

    extern crate alloc;
    use alloc::vec::Vec;

    type VecIntoIter<T> = std::vec::IntoIter<T>;

    impl<T: Iterator> Itertools for T {}

    pub trait Itertools: Iterator {
        fn k_largest_by_key<F, K>(self, k: usize, key: F) -> VecIntoIter<Self::Item>
        where
            Self: Sized,
            F: FnMut(&Self::Item) -> K,
            K: Ord,
        {
            self.k_largest_by(k, key_to_cmp(key))
        }

        fn k_largest_by<F>(self, k: usize, mut cmp: F) -> VecIntoIter<Self::Item>
        where
            Self: Sized,
            F: FnMut(&Self::Item, &Self::Item) -> Ordering,
        {
            self.k_smallest_by(k, move |a, b| cmp(b, a))
        }

        fn k_smallest_by<F>(self, k: usize, cmp: F) -> VecIntoIter<Self::Item>
        where
            Self: Sized,
            F: FnMut(&Self::Item, &Self::Item) -> Ordering,
        {
            k_smallest_general(self, k, cmp).into_iter()
        }
    }

    /// Consumes a given iterator, returning the minimum elements in **ascending** order.
    pub fn k_smallest_general<I, F>(iter: I, k: usize, mut comparator: F) -> Vec<I::Item>
    where
        I: Iterator,
        F: FnMut(&I::Item, &I::Item) -> Ordering,
    {
        /// Sift the element currently at `origin` away from the root until it is properly ordered.
        ///
        /// This will leave **larger** elements closer to the root of the heap.
        fn sift_down<T, F>(heap: &mut [T], is_less_than: &mut F, mut origin: usize)
        where
            F: FnMut(&T, &T) -> bool,
        {
            #[inline]
            fn children_of(n: usize) -> (usize, usize) {
                (2 * n + 1, 2 * n + 2)
            }

            while origin < heap.len() {
                let (left_idx, right_idx) = children_of(origin);
                if left_idx >= heap.len() {
                    return;
                }

                let replacement_idx =
                    if right_idx < heap.len() && is_less_than(&heap[left_idx], &heap[right_idx]) {
                        right_idx
                    } else {
                        left_idx
                    };

                if is_less_than(&heap[origin], &heap[replacement_idx]) {
                    heap.swap(origin, replacement_idx);
                    origin = replacement_idx;
                } else {
                    return;
                }
            }
        }

        if k == 0 {
            iter.last();
            return Vec::new();
        }
        if k == 1 {
            return iter.min_by(comparator).into_iter().collect();
        }
        let mut iter = iter.fuse();
        let mut storage: Vec<I::Item> = iter.by_ref().take(k).collect();

        let mut is_less_than = move |a: &_, b: &_| comparator(a, b) == Ordering::Less;

        // Rearrange the storage into a valid heap by reordering from the second-bottom-most layer up to the root.
        // Slightly faster than ordering on each insert, but only by a factor of lg(k).
        // The resulting heap has the **largest** item on top.
        for i in (0..=(storage.len() / 2)).rev() {
            sift_down(&mut storage, &mut is_less_than, i);
        }

        iter.for_each(|val| {
            debug_assert_eq!(storage.len(), k);
            if is_less_than(&val, &storage[0]) {
                // Treating this as an push-and-pop saves having to write a sift-up implementation.
                // https://en.wikipedia.org/wiki/Binary_heap#Insert_then_extract
                storage[0] = val;
                // We retain the smallest items we've seen so far, but ordered largest first so we can drop the largest efficiently.
                sift_down(&mut storage, &mut is_less_than, 0);
            }
        });

        // Ultimately the items need to be in least-first, strict order, but the heap is currently largest-first.
        // To achieve this, repeatedly,
        // 1) "pop" the largest item off the heap into the tail slot of the underlying storage,
        // 2) shrink the logical size of the heap by 1,
        // 3) restore the heap property over the remaining items.
        let mut heap = &mut storage[..];
        while heap.len() > 1 {
            let last_idx = heap.len() - 1;
            heap.swap(0, last_idx);
            // Sifting over a truncated slice means that the sifting will not disturb already popped elements.
            heap = &mut heap[..last_idx];
            sift_down(heap, &mut is_less_than, 0);
        }

        storage
    }

    #[inline]
    pub(crate) fn key_to_cmp<T, K, F>(mut key: F) -> impl FnMut(&T, &T) -> Ordering
    where
        F: FnMut(&T) -> K,
        K: Ord,
    {
        move |a, b| key(a).cmp(&key(b))
    }
}
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

use crate::def::*;
use crate::itertools_clone::Itertools;
use crate::util::*;
use itertools::iproduct;

const TIME_LIMIT: f64 = 2.98;
const CKPT_N: usize = 6;
const CKPT: [(i64, f64); CKPT_N] = [
    (200, 0.98),
    (300, 0.98),
    (400, 0.98),
    (500, 0.99),
    (600, 0.99),
    (700, 0.99),
];

#[macro_export]
#[cfg(not(feature = "local"))]
macro_rules! eprint {
    ($($_:tt)*) => {};
}

#[macro_export]
#[cfg(not(feature = "local"))]
macro_rules! eprintln {
    ($($_:tt)*) => {};
}

#[macro_export]
#[cfg(not(feature = "local"))]
macro_rules! assert {
    ($($_:tt)*) => {};
}

#[macro_export]
#[cfg(not(feature = "local"))]
macro_rules! assert_eq {
    ($($_:tt)*) => {};
}

#[macro_export]
#[cfg(not(feature = "local"))]
macro_rules! assert_ne {
    ($($_:tt)*) => {};
}

fn eval(t: i64, s: i64, k: i64, potential_s: i64, input: &Input) -> i64 {
    if t > input.t {
        return -INF;
    }
    let s_progress = (s as f64 / 1000.).min(1.).powf(2.);
    let t_progress = (1. - t as f64 / 600.).max(1e-4).sqrt();
    let progress = s_progress.min(t_progress);
    let alpha = (input.p.alpha * progress).max(input.p.alpha_low);
    ((input.t as f64 - t as f64) * (s as f64 + potential_s as f64 * alpha) + k as f64).round()
        as i64
}

fn eval_is_early_stage(s: i64, k: i64, input: &Input) -> bool {
    let required = COST_FOR_STATION + COST_FOR_RAIL * input.p.required - k;
    required > 0 && s < COST_FOR_RAIL * input.p.greedy_rail
}

fn eval_for_greedy(
    is_early_stage: bool,
    t: i64,
    s: i64,
    k: i64,
    potential_s: i64,
    input: &Input,
) -> i64 {
    if is_early_stage {
        let required = COST_FOR_STATION + COST_FOR_RAIL * input.p.required - k;
        let e = -(t as f64
            + required.max(0) as f64 / (s as f64 + potential_s as f64 * input.p.alpha_low + 1e-8))
            .ceil() as i64;
        return e;
    }
    if t > input.t {
        return -INF;
    }
    eval(t, s, k, potential_s, input)
}

fn main() {
    time::start_clock();
    let input = Input::read_input();

    let simulator = solve(&input);
    simulator.output();

    eprintln!(
        "result: {{\"score\": {}, \"duration\": {:.4}}}",
        simulator.k,
        time::elapsed_seconds(),
    );

    std::process::exit(0);
}

fn solve(input: &Input) -> Simulator {
    let mut simulator = Simulator::new(&input);
    let mut path_finder = PathFinder::new(input.n);
    let mut stations = beam_search_first_stations(input, 0.8, &mut simulator, &mut path_finder);
    stations = greedy(&stations, input, Some(400)); // :param
    stations = optimize_roughly(stations, time::elapsed_seconds() + 0.2, &input, false); // :param
    stations = greedy(&stations, input, Some(600)); // :param
    stations = optimize_roughly(stations, time::elapsed_seconds() + 0.3, &input, false); // :param
    stations = greedy(&stations, input, None);
    stations = optimize_roughly(stations, 2.7, &input, true); // :param

    simulator = optimize_strictly(stations, TIME_LIMIT, simulator, input, &mut path_finder);
    simulator.wait(input.t - simulator.t);
    simulator
}

fn get_best_c(status: &mut Status, input: &Input) -> Option<Coor> {
    let mut best_c = None;
    let mut best_e = -1.;
    for (i, j) in iproduct!(0..input.n, 0..input.n) {
        let c = Coor { i, j };
        if status.get_is_station(&c) {
            continue;
        }
        let (ds, potential_ds) = status.get_ds(&c);
        let e = ds as f64 + potential_ds as f64 * input.p.alpha;
        let e = e * (1. - input.p.s_range + input.p.s_range * 2. * rnd::nextf());
        if e > best_e {
            best_c = Some(c);
            best_e = e;
        }
    }
    best_c
}

#[allow(unused)]
#[derive(Debug)]
enum Action {
    Move(usize, usize, usize),
    Swap(usize, usize, usize),
    Flip(usize, usize),
    Shuffle(usize, usize),
    Remove(Vec<(bool, Coor)>),
    Add(Vec<(bool, Coor)>),
    Change(Vec<(bool, Coor)>),
}

impl Action {
    #[allow(unused)]
    fn apply(&self, status: &mut Status, input: &Input) {
        match self {
            Action::Move(_, _, _) => {}
            Action::Swap(_, _, _) => {}
            Action::Flip(_, _) => {}
            Action::Shuffle(_, _) => {}
            Action::Remove(stations) => Action::update_status(status, stations, false, input),
            Action::Add(stations) => Action::update_status(status, stations, false, input),
            Action::Change(stations) => Action::update_status(status, stations, false, input),
        }
    }

    fn rollback(&self, status: &mut Status, input: &Input) {
        match self {
            Action::Move(_, _, _) => {}
            Action::Swap(_, _, _) => {}
            Action::Flip(_, _) => {}
            Action::Shuffle(_, _) => {}
            Action::Remove(stations) => Action::update_status(status, stations, true, input),
            Action::Add(stations) => Action::update_status(status, stations, true, input),
            Action::Change(stations) => Action::update_status(status, stations, true, input),
        }
    }

    fn update_status(
        status: &mut Status,
        stations: &Vec<(bool, Coor)>,
        reverse: bool,
        input: &Input,
    ) {
        for &(is_added, c) in stations.iter() {
            if is_added ^ reverse {
                status.register_station(c, input);
            } else {
                status.unregister_station(c, input);
            }
        }
    }
}

fn apply_action(
    stations: &mut Vec<Coor>,
    status: &mut Status,
    input: &Input,
    change_length: bool,
) -> Action {
    loop {
        let p = rnd::nextf();
        if p < 0.4 {
            if change_length && p < 0.04 {
                let i = rnd::gen_index(stations.len());
                let c = stations.remove(i);
                return Action::Remove(vec![(false, c)]);
            } else if change_length && p < 0.1 {
                let mut a = vec![];
                for _ in 0..rnd::gen_range(1, 3) {
                    let i = rnd::gen_index(stations.len() + 1);
                    let Some(c) = get_best_c(status, input) else {
                        continue;
                    };
                    status.register_station(c, input);
                    a.push((true, c));
                    stations.insert(i, c);
                }

                if a.len() == 0 {
                    continue;
                }

                // 元に戻す
                let action = Action::Add(a);
                action.rollback(status, input);
                return action;
            } else {
                let mut a = vec![];
                let i = rnd::gen_index(stations.len());
                // 一時的に駅の登録を外す
                let remove_station = stations[i];
                status.unregister_station(remove_station, input);
                let Some(c) = get_best_c(status, input) else {
                    status.register_station(remove_station, input);
                    continue;
                };
                if c == stations[i] {
                    status.register_station(remove_station, input);
                    continue;
                }
                status.register_station(c, input);
                a.push((false, remove_station));
                a.push((true, c));
                stations[i] = c;

                if a.len() == 0 {
                    continue;
                }

                // 元に戻す
                let action = Action::Change(a);
                action.rollback(status, input);
                return action;
            }
        } else if p < 0. {
            let length = rnd::gen_range(4, 10);
            if stations.len() <= length + 1 {
                continue;
            }
            let i = rnd::gen_index(stations.len() - length + 1);
            for d in 0..length / 2 {
                stations.swap(i + d, i + length - d - 1);
            }
            return Action::Flip(i, length);
        } else if p < 0.75 {
            let length = rnd::gen_range(4, 10);
            if stations.len() <= length + 1 {
                continue;
            }
            let i = rnd::gen_index(stations.len() - length + 1);
            for d in 0..length {
                stations.swap(i + d, i + rnd::gen_index(length));
            }
            return Action::Shuffle(i, length);
        } else if p < 0.95 {
            if stations.len() == 0 {
                continue;
            }
            let i1 = rnd::gen_index(stations.len());
            let i2 = if p < 0.9 {
                let s_len = stations.len();
                (0..s_len)
                    .filter(|&i| i != i1)
                    .min_by_key(|&i| {
                        dist(&stations[i1], &stations[i]) + rnd::gen_range(0, 10) as i64
                    })
                    .unwrap()
            } else {
                rnd::gen_index(stations.len())
            };
            if i1 == i2 {
                continue;
            }
            stations.swap(i1, i2);
            return Action::Swap(i1, i2, 1);
        } else {
            if stations.len() == 1 {
                continue;
            }
            // NOTE: 厳密にはちょっと後ろが選ばれやすく、一様ではない
            let i1 = rnd::gen_index(stations.len());
            let i2 = rnd::gen_index(stations.len());
            if i1 == i2 {
                continue;
            }
            let c = stations.remove(i2);
            stations.insert(i1, c);
            return Action::Move(i1, i2, 1);
        }
    }
}

fn optimize_strictly(
    stations: Vec<Coor>,
    time_limit: f64,
    mut simulator: Simulator,
    input: &Input,
    path_finder: &mut PathFinder,
) -> Simulator {
    let (_, mut cur_eval_ckpt) = simulator.playout(&stations, input, path_finder, true, None);
    let mut cur_score = simulator.eval_final_score(input);
    let mut cur_stations = simulator.stations.clone();
    let mut cur_status = simulator.status.clone().to_track_ds_status(input);

    eprintln!("start-score      = {:10}", cur_score);

    let mut iteration = 0;
    let mut adopted = 0;
    let mut quit_count = 0;
    let mut new_stations = Vec::with_capacity(2_500);
    while time::elapsed_seconds() < time_limit {
        new_stations.clear();
        new_stations.extend(&cur_stations);

        // NOTE: trueにしないと、stations.len()=1の時に無限ループになる
        let _action = apply_action(&mut new_stations, &mut cur_status, input, false);
        let (success, eval_ckpt) = simulator.playout(
            &new_stations,
            input,
            path_finder,
            false,
            Some(cur_eval_ckpt),
        );
        if !success {
            quit_count += 1;
            continue;
        }
        let new_score = simulator.eval_final_score(input);
        if new_score > cur_score {
            cur_eval_ckpt = eval_ckpt;
            cur_score = new_score;
            cur_stations = simulator.stations.clone(); // TODO: .cloneを消す

            _action.apply(&mut cur_status, input);
            adopted += 1;
        }
        iteration += 1;
    }

    eprintln!("score:b          = {:10}", cur_score);
    eprintln!("iteration:b      = {:10}", iteration);
    eprintln!("adopted:b        = {:10}", adopted);
    eprintln!("quit-count:b     = {:10}", quit_count);

    let (success, _) = simulator.playout(&cur_stations, input, path_finder, true, None);
    assert!(success);
    simulator
}

fn optimize_roughly(
    stations: Vec<Coor>,
    time_limit: f64,
    input: &Input,
    change_length: bool,
) -> Vec<Coor> {
    let mut rough_simulator = RoughSimulator::new(input, false);
    let _ = rough_simulator.playout(&stations, input, None);

    let mut cur_score = rough_simulator.eval_final_score(input);
    let mut cur_stations = rough_simulator.stations.clone();
    let mut cur_status = rough_simulator.status.clone().to_track_ds_status(input);
    let mut cur_eval_ckpt = rough_simulator.eval_ckpt;
    let mut iteration = 0;
    let mut adopted = 0;

    let start = time::elapsed_seconds();
    // NOTE: cur_scoreが大きいほど山を登るのが大変なので、cur_scoreに温度を依存させない方が良いかも
    let (start_temp, end_temp): (f64, f64) = (
        cur_score as f64 / input.p.start_temp,
        cur_score as f64 / input.p.end_temp,
    );

    eprintln!("rough-start-score    = {:10}", cur_score);
    let mut quit_count = 0;

    let mut new_stations = Vec::with_capacity(2_500);
    while time::elapsed_seconds() < time_limit {
        new_stations.clear();
        new_stations.extend(&cur_stations);

        let action = apply_action(&mut new_stations, &mut cur_status, input, change_length);
        let success = rough_simulator.playout(&new_stations, input, Some(cur_eval_ckpt));
        if !success {
            quit_count += 1;
            continue;
        }
        let new_score = rough_simulator.eval_final_score(input);
        let progress = (time::elapsed_seconds() - start) / (time_limit - start);
        let cur_temp = start_temp.powf(1. - progress) * end_temp.powf(progress);
        let adopt = rnd::nextf() < ((new_score - cur_score) as f64 / cur_temp).exp();
        if adopt {
            // if new_score > cur_score {
            //     eprintln!(
            //         "a: {} -> {} ({:4}) {:?}",
            //         cur_score,
            //         new_score,
            //         new_score - cur_score,
            //         action,
            //     );
            // }
            cur_eval_ckpt = rough_simulator.eval_ckpt;
            cur_score = new_score;
            cur_stations = rough_simulator.stations.clone(); // TODO: .cloneを消す

            action.apply(&mut cur_status, input);
            adopted += 1;
        }
        iteration += 1;
    }

    eprintln!("optimized-rougly-result:");
    eprintln!(
        "duration         = {:10.4}",
        time::elapsed_seconds() - start
    );
    eprintln!("iteration        = {:10}", iteration);
    eprintln!("adopted          = {:10}", adopted);
    eprintln!("quit_count       = {:10}", quit_count);
    eprintln!("rough-end-score  = {:10}", cur_score);
    rough_simulator.playout(&cur_stations, input, None);
    assert_eq!(cur_score, rough_simulator.eval_final_score(input));

    cur_stations
}

fn greedy(initial_stations: &Vec<Coor>, input: &Input, end_t: Option<i64>) -> Vec<Coor> {
    let beam_width = 100;
    let mut rough_simulator = RoughSimulator::new(input, false);
    rough_simulator.playout(&initial_stations, input, None);
    eprintln!(
        "before-greedy-score     = {:10}",
        rough_simulator.eval_final_score(input)
    );
    let end_t = end_t.unwrap_or(input.t);

    while rough_simulator.t < end_t {
        let is_early_stage =
            eval_is_early_stage(rough_simulator.status.s, rough_simulator.k, input);
        let cur_e = rough_simulator.eval_for_greedy(is_early_stage, input);
        let mut evals = vec![];

        for (_, c) in get_next_cs(
            &mut rough_simulator.status,
            &rough_simulator.stations,
            &input,
        )
        .into_iter()
        .k_largest_by_key(beam_width, |e| e.0)
        {
            let (e, _) = rough_simulator.eval_add_station(c, is_early_stage, input);
            evals.push((e, c));
        }

        let Some((e, c)) = evals.into_iter().max_by_key(|a| a.0) else {
            break;
        };

        // 悪化するようになったらやめる
        // NOTE: 潜在的な満足度を考慮していないので、最適ではないかも
        if e < cur_e {
            break;
        }

        rough_simulator.add_station(c, input);
    }

    eprintln!(
        "greedy-score     = {:10}",
        rough_simulator.eval_final_score(input)
    );

    rough_simulator.stations.clone()
}

fn beam_search_first_stations(
    input: &Input,
    time_limit: f64,
    simulator: &mut Simulator,
    path_finder: &mut PathFinder,
) -> Vec<Coor> {
    let beam_width = input.p.beam_width / input.m;
    let beam_size = input.p.beam_size * 1600 / input.m.max(400);

    let mut rough_simulator = RoughSimulator::new(input, true);
    let mut states = vec![];
    for (_, c) in get_next_cs(
        &mut rough_simulator.status,
        &rough_simulator.stations,
        input,
    )
    .into_iter()
    .k_largest_by_key(beam_width * 5, |e| e.0)
    {
        let stations = vec![c];
        states.push(stations);
    }

    let mut set = std::collections::HashSet::new();
    for _r in 0.. {
        if time::elapsed_seconds() > time_limit {
            eprintln!(
                "stopped beam-search at: r={}, time={:.4}",
                _r,
                time::elapsed_seconds()
            );
            break;
        }
        let mut next_cs = vec![];
        let mut is_early_stage = None;
        for (state_i, stations) in states.iter().enumerate() {
            rough_simulator.playout(stations, input, None);

            // NOTE: 速度面で最初のを採用しているが、minとかで見た方が良いかも
            is_early_stage = is_early_stage.or(Some(eval_is_early_stage(
                rough_simulator.status.s,
                rough_simulator.k,
                input,
            )));
            for (_, c) in get_next_cs(
                &mut rough_simulator.status,
                &rough_simulator.stations,
                input,
            )
            .into_iter()
            .k_largest_by_key(beam_size, |e| e.0)
            {
                let (eval, hash) =
                    rough_simulator.eval_add_station(c, is_early_stage.unwrap(), input);
                if eval > -INF {
                    next_cs.push((eval, hash, state_i, c));
                }
            }
        }

        let mut next_states = vec![];
        set.clear();
        for (_, _, state_i, c) in next_cs
            .into_iter()
            .k_largest_by_key(beam_width, |e| e.0)
            .filter(|s| set.insert(s.1))
        {
            let mut stations = states[state_i].clone();
            stations.push(c);
            next_states.push(stations);
        }
        if next_states.len() == 0 {
            // 途中で追加する駅が見つからなかった場合は、前回までの結果を返す
            // NOTE: get_next_csでsが増える駅がない場合が該当する
            // TODO: 多分もっと広く探索した方が良い
            break;
        }
        states = next_states;
    }

    let stations = get_valid_stations(states, simulator, path_finder, input);
    eprintln!("stations after beam-search:  {:?}", stations);
    stations
}

#[derive(Debug)]
enum Command {
    Wait(i64),
    // [始点、現状の経路、終点]
    AddRailPath(Vec<Coor>),
    AddStation(Coor),
}

#[derive(Debug)]
enum CommandInfo {
    // command-index, path1, path2, c
    Insert(usize, Vec<Coor>, Vec<Coor>, Coor),
    // path, c
    Connect(Vec<Coor>, Coor),
}

struct Simulator {
    field: Field,
    commands: Vec<Command>,
    status: Status,
    stations: Vec<Coor>,
    k: i64,
    t: i64,
    max_t: i64,
}

impl Simulator {
    fn new(input: &Input) -> Simulator {
        Simulator {
            field: Field::new(input.n),
            commands: vec![],
            status: Status::new(input, false),
            stations: vec![],
            k: input.k,
            t: 0,
            max_t: input.t,
        }
    }

    fn reset(&mut self, input: &Input) {
        self.field.reset();
        self.commands.clear();
        self.status.reset();
        self.stations.clear();
        self.k = input.k;
        self.t = 0;
        self.max_t = input.t;
    }

    fn playout(
        &mut self,
        stations: &Vec<Coor>,
        input: &Input,
        path_finder: &mut PathFinder,
        force_continue: bool,
        best_eval_ckpt: Option<[i64; CKPT_N]>,
    ) -> (bool, [i64; CKPT_N]) {
        if stations.len() == 0 {
            return (false, [0; CKPT_N]);
        }
        self.reset(input);

        let mut success = true;
        self.add_station(stations[0], &input);

        let best_evals_ckpt = best_eval_ckpt.unwrap_or([-INF; CKPT_N]);
        let mut evals_ckpt = [-INF; CKPT_N];

        for station in stations.iter().skip(1) {
            if self.t >= input.t {
                break;
            }
            // self.field.print();

            let Some(info) = self.get_command_info(*station, path_finder) else {
                // NOTE: 最短経路がない場合は失敗する
                // NOTE: 見つかったからといって、別の駅に繋ぐことになるかもしれないので、RoughSimulatorと同じ結果が得られるわけではない
                if !force_continue {
                    return (false, [0; CKPT_N]);
                } else {
                    success = false;
                    continue;
                }
            };
            self.apply_command_info(info, &input);

            // TODO: 悪化するようになったらやめる
            // NOTE: 潜在的な満足度を考慮していないので、最適ではないかも

            // 記録する
            for (i, (ckpt_t, ratio)) in CKPT.iter().enumerate() {
                if evals_ckpt[i] == -INF && self.t >= *ckpt_t {
                    evals_ckpt[i] = self.eval_final_score(input);
                    if (evals_ckpt[i] as f64) < (best_evals_ckpt[i] as f64 * ratio) {
                        return (false, evals_ckpt);
                    }
                }
            }
        }

        // NOTE: ここで最後までwaitするとpotential_tを評価しないため、局所改善の性能が悪くなる
        // self.wait(input.t - self.t);

        (success, evals_ckpt)
    }

    fn add_station(&mut self, c: Coor, input: &Input) {
        self.wait_until_enough_k(COST_FOR_STATION);
        if !self.is_in_time() {
            return;
        }

        self.status.register_station(c, input);

        self.stations.push(c);
        self.field.add_station(&c);
        self.k -= COST_FOR_STATION;

        self.commands.push(Command::AddStation(c));

        self.process_turn(1);
    }

    fn add_rail_path(&mut self, path: Vec<Coor>) {
        if path.len() == 0 {
            return;
        }
        self.wait_until_enough_k(COST_FOR_RAIL);
        if !self.is_in_time() {
            return;
        }

        for c in path[1..path.len() - 1].iter() {
            if !self.is_in_time() {
                return;
            }
            self.field.add_rail(&c);
            self.k -= COST_FOR_RAIL;

            self.process_turn(1);
        }

        self.commands.push(Command::AddRailPath(path));
    }

    fn wait(&mut self, turn: i64) {
        if !self.is_in_time() {
            return;
        }
        self.commands.push(Command::Wait(turn));
        self.process_turn(turn);
    }

    fn wait_until_enough_k(&mut self, cost: i64) {
        if self.k >= cost {
            return;
        }
        if self.status.s == 0 {
            self.wait(self.max_t - self.t);
            return;
        }
        // ceil((cost + k) / s)
        let turn = (cost - self.k + self.status.s - 1) / self.status.s;
        self.wait(turn);
    }

    fn process_turn(&mut self, turn: i64) {
        self.k += self.status.s * turn;
        self.t += turn;
    }

    fn is_in_time(&self) -> bool {
        self.t < self.max_t
    }

    fn output(&self) {
        // NOTE: self.commands.len() <= input.tが保証されていないかも
        let mut path = vec![];
        for cmd in self.commands.iter() {
            match cmd {
                Command::Wait(turn) => {
                    for _ in 0..*turn {
                        println!("-1");
                    }
                }
                Command::AddRailPath(cur_path) => {
                    if path.len() > 0 {
                        path.pop();
                    }
                    path.extend(cur_path);
                }
                Command::AddStation(c) => {
                    let cmds = path_to_commands(&path);
                    for (p, c) in cmds {
                        println!("{} {} {}", p, c.i, c.j);
                    }
                    path.clear();
                    println!("0 {} {}", c.i, c.j);
                }
            }
        }
    }

    fn apply_command_info(&mut self, info: CommandInfo, input: &Input) {
        match info {
            CommandInfo::Connect(path, c) => {
                self.add_rail_path(path);
                self.add_station(c, &input);
            }
            CommandInfo::Insert(ci, path1, path2, c) => {
                let Command::AddRailPath(path) = self.commands.remove(ci) else {
                    panic!()
                };

                // 線路の更新
                assert_eq!(path.len(), path1.len() + path2.len() - 1);
                self.field.remove_rail_path(&path);
                self.field.add_rail_path(&path1);
                self.field.add_rail(&c);
                self.field.add_rail_path(&path2);
                self.add_station(c, &input);

                // コマンドの更新
                // 挿入するのでこの順番じゃないとダメ
                self.commands.insert(ci, Command::AddRailPath(path2));
                self.commands.insert(ci, Command::AddRailPath(path1));
            }
        }
    }

    fn calc_evals_for_insert(
        &mut self,
        c: Coor,
        path_finder: &mut PathFinder,
    ) -> Option<CommandInfo> {
        if self.status.get_is_station(&c) {
            return None;
        }

        let mut insert_infos = vec![];
        for (ci, command) in self.commands.iter().enumerate() {
            let Command::AddRailPath(path) = command else {
                continue;
            };

            assert!(path.len() >= 2);

            let (s, t) = (path[0], path[path.len() - 1]);
            if !is_in_region(&c, &s, &t, false) {
                continue;
            }

            // 現在の線路を取り除いておく
            self.field.remove_rail_path(path);
            if self.field.get_tile(&c) != Tile::None {
                // 元の状態に戻す
                self.field.add_rail_path(path);
                continue;
            }

            let a = path_finder.find_path(&s, &c, &mut self.field);
            let b = path_finder.find_path(&c, &t, &mut self.field);

            // 元の状態に戻す
            self.field.add_rail_path(path);

            if a.is_some() && b.is_some() {
                let e = region_size(&s, &t) - region_size(&s, &c) - region_size(&c, &t);
                insert_infos.push((e, CommandInfo::Insert(ci, a.unwrap().0, b.unwrap().0, c)));
            }
        }

        let mut ret = None;
        let mut _best_e = INF;
        for (e, info) in insert_infos {
            // NOTE: 削減される矩形領域が小さい方を採用した方が良さそうだけど、常に後ろの矩形を選択する方が良いっぽい？
            if e > _best_e {
                continue;
            }
            _best_e = e;
            ret = Some(info);
        }

        ret
    }

    fn calc_evals_for_connect(
        &mut self,
        c: Coor,
        path_finder: &mut PathFinder,
    ) -> Option<CommandInfo> {
        if self.status.get_is_station(&c) {
            return None;
        }

        let Some((path, d)) =
            path_finder.find_path_to_closest_station(&c, &self.stations, &mut self.field)
        else {
            return None;
        };

        assert!(d > 0);

        // 評価値の計算
        Some(CommandInfo::Connect(path, c))
    }

    fn get_command_info(&mut self, c: Coor, path_finder: &mut PathFinder) -> Option<CommandInfo> {
        self.calc_evals_for_insert(c, path_finder)
            .or(self.calc_evals_for_connect(c, path_finder))
    }

    #[inline]
    fn eval_final_score(&self, input: &Input) -> i64 {
        eval(
            self.t,
            self.status.s,
            self.k,
            self.status.potential_s,
            input,
        )
    }
}

#[derive(Debug)]
struct RoughSimulator {
    edges: Vec<(usize, usize)>,
    stations: Vec<Coor>,
    count: Vec<usize>,
    status: Status,
    eval_ckpt: [i64; CKPT_N],
    k: i64,
    t: i64,
}

impl RoughSimulator {
    fn new(input: &Input, track_added: bool) -> RoughSimulator {
        RoughSimulator {
            edges: Vec::with_capacity(2_500),
            stations: Vec::with_capacity(2_500),
            count: Vec::with_capacity(2_500),
            status: Status::new(input, track_added),
            eval_ckpt: [0; CKPT_N],
            k: input.k,
            t: 0,
        }
    }

    fn playout(
        &mut self,
        stations: &Vec<Coor>,
        input: &Input,
        best_eval_ckpt: Option<[i64; CKPT_N]>,
    ) -> bool {
        if stations.len() == 0 {
            return false;
        }
        self.reset(input);

        let best_eval_ckpt = best_eval_ckpt.unwrap_or([-INF; CKPT_N]);
        for &c in stations.iter() {
            if !self.add_station(c, input) {
                return false;
            }

            if !self.check_ckpt(best_eval_ckpt, input) {
                return false;
            }
        }

        true
    }

    fn check_ckpt(&mut self, best_eval_ckpt: [i64; CKPT_N], input: &Input) -> bool {
        // 記録する
        for (i, (ckpt_t, ratio)) in CKPT.iter().enumerate() {
            if self.eval_ckpt[i] == -INF && self.t >= *ckpt_t {
                self.eval_ckpt[i] = self.eval_final_score(input);
                if (self.eval_ckpt[i] as f64) < (best_eval_ckpt[i] as f64 * ratio) {
                    return false;
                }
            }
        }
        true
    }

    fn reset(&mut self, input: &Input) {
        self.status.reset();
        self.edges.clear();
        self.stations.clear();
        self.count.clear();
        self.k = input.k;
        self.t = 0;
        self.eval_ckpt = [-INF; CKPT_N];
    }

    fn add_root_station(&mut self, root: Coor, input: &Input) {
        self.status.register_station(root, input);
        self.stations.push(root);
        self.count.push(0);
        self.k -= COST_FOR_STATION;
        self.t += 1; // FIXME: あるのが正しそうだが、何故かスコアが下がる、、
    }

    fn add_station(&mut self, c: Coor, input: &Input) -> bool {
        if self.stations.len() == 0 {
            self.add_root_station(c, input);
            return true;
        }
        let si = self.stations.len();

        let path_length = if let Some(ei) = (0..self.edges.len())
            .rev()
            .find(|&ei| self.can_insert(&c, ei))
        {
            // 挿入する場合
            // 順序は気にしなくてよい
            // NOTE: 挿入するペアを最適化してはいない
            let edge = self.edges.swap_remove(ei);
            self.edges.push((edge.0, si));
            self.edges.push((si, edge.1));
            self.count.push(2);

            1
        } else if let Some(other_si) = (0..self.stations.len())
            .filter(|&si| self.count[si] < 4)
            .min_by_key(|&si| dist(&c, &self.stations[si]))
        {
            // 追加する場合
            self.edges.push((other_si, si));
            self.count[other_si] += 1;
            self.count.push(1);

            dist(&c, &self.stations[other_si])
        } else {
            return false;
        };

        let (cost, dt) = calc_cost_and_dt(path_length, self.k, self.status.s);
        let dk = self.status.s * dt - cost;
        self.k += dk;
        self.t += dt;

        self.status.register_station(c, input);
        self.stations.push(c);

        self.t < input.t
    }

    fn eval_add_station(&mut self, c: Coor, is_early_stage: bool, input: &Input) -> (i64, u32) {
        let path_length = if let Some(_) = (0..self.edges.len())
            .rev()
            .find(|&ei| self.can_insert(&c, ei))
        {
            1
        } else if let Some(other_si) = (0..self.stations.len())
            .filter(|&si| self.count[si] < 4)
            .min_by_key(|&si| dist(&c, &self.stations[si]))
        {
            dist(&c, &self.stations[other_si])
        } else {
            return (-INF, 0);
        };

        // 仮更新
        let (cost, dt) = calc_cost_and_dt(path_length, self.k, self.status.s);
        let dk = self.status.s * dt - cost;
        self.k += dk;
        self.t += dt;

        let restore_added_len = self.status.added.len();
        self.status.register_station(c, input);
        self.stations.push(c);

        // 評価
        let eval = self.eval_for_greedy(is_early_stage, input);
        let hash = self.hash(input);

        // ロールバック
        self.k -= dk;
        self.t -= dt;
        self.status.unregister_station(c, input);
        self.status.added.truncate(restore_added_len);
        self.stations.pop();

        (eval, hash)
    }

    #[inline]
    fn can_insert(&mut self, c: &Coor, ei: usize) -> bool {
        let edge = &self.edges[ei];
        is_in_region(&c, &self.stations[edge.0], &self.stations[edge.1], false)
    }

    #[inline]
    fn eval_for_greedy(&self, is_early_stage: bool, input: &Input) -> i64 {
        eval_for_greedy(
            is_early_stage,
            self.t,
            self.status.s,
            self.k,
            self.status.potential_s,
            input,
        )
    }

    #[inline]
    fn eval_final_score(&self, input: &Input) -> i64 {
        eval(
            self.t,
            self.status.s,
            self.k,
            self.status.potential_s,
            input,
        )
    }

    fn hash(&self, input: &Input) -> u32 {
        let mut hash = 0;
        for &(s_idx, is_s) in self.status.added.iter() {
            let idx = s_idx * 2 + is_s;
            hash ^= input.hashes[idx];
        }
        hash
    }

    #[allow(unused)]
    fn print(&self) {
        eprintln!(
            "rough-simulator: t={} k={} s={} ps={}",
            self.t, self.k, self.status.s, self.status.potential_s
        );
    }
}

fn calc_cost_and_dt(path_length: i64, k: i64, s: i64) -> (i64, i64) {
    let cost = (path_length - 1) * COST_FOR_RAIL + COST_FOR_STATION;
    let dt = if cost >= k {
        if s == 0 {
            INF
        } else {
            // ceil((cost - k) / s);
            i64::max(path_length, (cost - k + s - 1) / s)
        }
    } else {
        path_length
    };
    (cost, dt)
}

fn get_next_cs(status: &mut Status, stations: &Vec<Coor>, input: &Input) -> Vec<(i64, Coor)> {
    let mut ret = vec![];
    if stations.len() == 0 {
        // 最初の点を選ぶときは、相方となる点で届く範囲にあるものをカウントする
        for (i, j) in iproduct!(0..input.n, 0..input.n) {
            let c = Coor { i, j };
            let mut ds = 0;
            for &(idx, is_s) in input.points[c.i][c.j].iter() {
                let other = &input.coors[idx][1 - is_s];
                let is_reachable_without_s =
                    dist(&c, other) - 2 <= (input.k - COST_FOR_STATION * 2) / COST_FOR_RAIL;
                if is_reachable_without_s {
                    ds += input.scores[idx] * 1_000_000;
                } else {
                    ds += input.scores[idx];
                }
            }
            if ds == 0 {
                continue;
            }
            ret.push((ds, c));
        }
    } else if stations.len() == 1 {
        // 2個目の駅を選ぶときは、届く範囲である必要がある
        let s = stations[0];
        for (i, j) in iproduct!(0..input.n, 0..input.n) {
            let c = Coor { i, j };
            if c.i < s.i || (c.i == s.i && c.i <= s.j) {
                continue;
            }
            let mut ds = 0;
            for &(idx, is_s) in input.points[c.i][c.j].iter() {
                let other = &input.coors[idx][1 - is_s];
                if !status.is_added(idx, is_s) {
                    let is_reachable_without_s =
                        dist(&c, other) - 2 <= (input.k - COST_FOR_STATION * 2) / COST_FOR_RAIL;
                    if status.is_added(idx, 1 - is_s) && is_reachable_without_s {
                        ds += input.scores[idx] * 1_000_000_000;
                    } else {
                        let nd = stations
                            .iter()
                            .map(|s| dist(s, &input.coors[idx][1 - is_s]))
                            .min()
                            .unwrap();
                        ds += input.scores[idx] * 1_000 / (nd + 1).pow(2);
                    }
                }
            }
            if ds == 0 {
                continue;
            }
            ret.push((ds, c));
        }
    } else {
        for (i, j) in iproduct!(0..input.n, 0..input.n) {
            let c = Coor { i, j };
            if status.get_is_station(&c) {
                continue;
            }
            let mut ds = 0;
            for &(idx, is_s) in input.points[c.i][c.j].iter() {
                if !status.is_added(idx, is_s) {
                    if status.is_added(idx, 1 - is_s) {
                        ds += input.scores[idx] * 1_000_000;
                    } else {
                        let nd = stations
                            .iter()
                            .map(|s| dist(s, &input.coors[idx][1 - is_s]))
                            .min()
                            .unwrap();
                        ds += input.scores[idx] * 1_000_000 / (nd + 1).pow(2);
                    }
                }
            }
            if ds == 0 {
                continue;
            }
            ret.push((ds, c));
        }
    }
    ret
}

fn get_valid_stations(
    stations: Vec<Vec<Coor>>,
    simulator: &mut Simulator,
    path_finder: &mut PathFinder,
    input: &Input,
) -> Vec<Coor> {
    let mut first_s = None;
    for s in stations.into_iter() {
        // TODO: スコアが高い結果を取り出す
        let (success, _) = simulator.playout(&s, input, path_finder, false, None);
        if success {
            return s;
        }
        if first_s.is_none() {
            first_s = Some(s);
        }
    }
    first_s.unwrap()
}

#[allow(unused)]
fn compare_simulators(stations: &Vec<Coor>, input: &Input) {
    let mut rough_simulator = RoughSimulator::new(input, false);
    let mut simulator = Simulator::new(input);
    let mut path_finder = PathFinder::new(input.n);
    let (success, _) = simulator.playout(&stations, input, &mut path_finder, true, None);
    rough_simulator.playout(&stations, input, None);
    eprintln!("success:     {}", success);
    eprintln!(
        "simulator-eval     = eval: {}, s: {}, k: {}, t: {}",
        simulator.eval_final_score(input),
        simulator.status.s,
        simulator.k,
        simulator.t
    );
    eprintln!(
        "rough-eval         = eval: {}, s: {}, k: {}, t: {}",
        rough_simulator.eval_final_score(input),
        rough_simulator.status.s,
        rough_simulator.k,
        rough_simulator.t
    );
}
