use proconio::input;

const N: usize = 20;
const L: usize = 200;
const EXIT: (usize, usize) = (0, 10);

fn build_main_cycle() -> Vec<(usize, usize)> {
    let mut edges = vec![];
    let mut add = |a: (usize, usize), b: (usize, usize)| edges.push((a, b));

    for top in [0, 4, 8, 12, 16] {
        let bot = top + 3;
        for c in 1..18 {
            add((top, c), (top, c + 1));
            add((bot, c), (bot, c + 1));
        }
        for r in top..bot {
            add((r, 1), (r + 1, 1));
            add((r, 18), (r + 1, 18));
        }
    }

    let mut keep = vec![];
    for (a, b) in edges {
        let mut remove = false;
        for (bot, c) in [(3, 5), (7, 8), (11, 11), (15, 14)] {
            let top = bot + 1;
            if (a == (bot, c) && b == (bot, c + 1))
                || (b == (bot, c) && a == (bot, c + 1))
                || (a == (top, c) && b == (top, c + 1))
                || (b == (top, c) && a == (top, c + 1))
            {
                remove = true;
            }
        }
        if !remove {
            keep.push((a, b));
        }
    }
    for (bot, c) in [(3, 5), (7, 8), (11, 11), (15, 14)] {
        let top = bot + 1;
        keep.push(((bot, c), (top, c)));
        keep.push(((bot, c + 1), (top, c + 1)));
    }

    let mut adj = vec![vec![]; N * N];
    for (a, b) in keep {
        let ai = a.0 * N + a.1;
        let bi = b.0 * N + b.1;
        adj[ai].push(b);
        adj[bi].push(a);
    }

    let mut path = vec![EXIT];
    let mut prev = EXIT;
    let mut cur = adj[EXIT.0 * N + EXIT.1][0];
    while cur != EXIT {
        path.push(cur);
        let ns = &adj[cur.0 * N + cur.1];
        let next = if ns[0] == prev { ns[1] } else { ns[0] };
        prev = cur;
        cur = next;
    }
    path
}

fn build_matching(main: &[(usize, usize)]) -> Vec<(usize, usize)> {
    let mut main_id = vec![vec![usize::MAX; N]; N];
    for (k, &(i, j)) in main.iter().enumerate() {
        main_id[i][j] = k;
    }
    let mut buffer = vec![(usize::MAX, usize::MAX); L];
    let mut set_pair = |m: (usize, usize), b: (usize, usize)| {
        let id = main_id[m.0][m.1];
        assert_ne!(id, usize::MAX);
        buffer[id] = b;
    };

    for block in 0..5 {
        let top = block * 4;
        let bot = top + 3;
        set_pair((top, 1), (top, 0));
        set_pair((top, 18), (top, 19));
        set_pair((bot, 1), (bot, 0));
        set_pair((bot, 18), (bot, 19));
        for r in top + 1..=top + 2 {
            set_pair((r, 1), (r, 0));
            set_pair((r, 18), (r, 19));
        }
        for c in 2..=17 {
            set_pair((top, c), (top + 1, c));
            set_pair((bot, c), (bot - 1, c));
        }
    }

    for &b in &buffer {
        assert_ne!(b, (usize::MAX, usize::MAX));
    }
    buffer
}

struct Solver {
    main: Vec<(usize, usize)>,
    buffer: Vec<(usize, usize)>,
    a: Vec<Vec<i32>>,
    target: i32,
    ops: Vec<(usize, i32)>,
}

impl Solver {
    fn new(a: Vec<Vec<i32>>, main: Vec<(usize, usize)>, buffer: Vec<(usize, usize)>) -> Self {
        let mut solver = Self {
            main,
            buffer,
            a,
            target: 0,
            ops: vec![],
        };
        solver.try_remove();
        solver
    }

    fn is_range(x: i32, base: i32) -> bool {
        base <= x && x < base + L as i32
    }

    fn is_stage_item(x: i32, base: i32) -> bool {
        x == -1 || Self::is_range(x, base)
    }

    fn try_remove(&mut self) {
        let (i, j) = EXIT;
        while self.a[i][j] == self.target {
            self.a[i][j] = -1;
            self.target += 1;
        }
    }

    fn swap_at(&mut self, idx: usize) {
        let (mi, mj) = self.main[idx];
        let (bi, bj) = self.buffer[idx];
        let tmp = self.a[mi][mj];
        self.a[mi][mj] = self.a[bi][bj];
        self.a[bi][bj] = tmp;
        self.ops.push((idx + 1, 1));
        self.try_remove();
    }

    fn rotate_main(&mut self) {
        let first = self.a[self.main[0].0][self.main[0].1];
        for k in 0..L - 1 {
            let (i, j) = self.main[k];
            let (ni, nj) = self.main[k + 1];
            self.a[i][j] = self.a[ni][nj];
        }
        let (i, j) = self.main[L - 1];
        self.a[i][j] = first;
        self.ops.push((0, -1));
        self.try_remove();
    }

    fn rotate_main_rev(&mut self) {
        let last = self.a[self.main[L - 1].0][self.main[L - 1].1];
        for k in (1..L).rev() {
            let (i, j) = self.main[k];
            let (pi, pj) = self.main[k - 1];
            self.a[i][j] = self.a[pi][pj];
        }
        let (i, j) = self.main[0];
        self.a[i][j] = last;
        self.ops.push((0, 1));
        self.try_remove();
    }

    fn min_bottleneck_assignment(
        sources: &[usize],
        sinks: &[usize],
        dir: i32,
    ) -> (usize, Vec<(usize, usize)>) {
        if sources.is_empty() {
            return (0, vec![]);
        }
        let cost = |p: usize, q: usize| -> usize {
            if dir == -1 {
                (p + L - q) % L
            } else {
                (q + L - p) % L
            }
        };
        fn lower_bound(a: &[usize], x: usize) -> usize {
            let mut ok = a.len();
            let mut ng = 0;
            while ng < ok {
                let mid = (ng + ok) / 2;
                if a[mid] >= x {
                    ok = mid;
                } else {
                    ng = mid + 1;
                }
            }
            ok
        }

        fn upper_bound(a: &[usize], x: usize) -> usize {
            let mut ok = a.len();
            let mut ng = 0;
            while ng < ok {
                let mid = (ng + ok) / 2;
                if a[mid] > x {
                    ok = mid;
                } else {
                    ng = mid + 1;
                }
            }
            ok
        }

        fn add_shift_range(diff: &mut [i32], n: usize, k: usize, l: usize, r: usize) {
            let a = (l + n - k % n) % n;
            let b = (r + n - k % n) % n;
            if a <= b {
                diff[a] += 1;
                diff[b + 1] -= 1;
            } else {
                diff[0] += 1;
                diff[b + 1] -= 1;
                diff[a] += 1;
                diff[n] -= 1;
            }
        }

        fn add_sink_pos_range(
            diff: &mut [i32],
            sinks: &[usize],
            k: usize,
            pos_l: usize,
            pos_r: usize,
        ) {
            let l = lower_bound(sinks, pos_l);
            let r = upper_bound(sinks, pos_r);
            if l < r {
                add_shift_range(diff, sinks.len(), k, l, r - 1);
            }
        }

        fn feasible_shift(
            sources: &[usize],
            sinks: &[usize],
            dir: i32,
            lim: usize,
        ) -> Option<usize> {
            let n = sources.len();
            let mut diff = vec![0_i32; n + 1];
            for (k, &p) in sources.iter().enumerate() {
                if lim + 1 >= L {
                    add_shift_range(&mut diff, n, k, 0, n - 1);
                    continue;
                }
                if dir == -1 {
                    let l = (p + L - lim) % L;
                    let r = p;
                    if l <= r {
                        add_sink_pos_range(&mut diff, sinks, k, l, r);
                    } else {
                        add_sink_pos_range(&mut diff, sinks, k, 0, r);
                        add_sink_pos_range(&mut diff, sinks, k, l, L - 1);
                    }
                } else {
                    let l = p;
                    let r = (p + lim) % L;
                    if l <= r {
                        add_sink_pos_range(&mut diff, sinks, k, l, r);
                    } else {
                        add_sink_pos_range(&mut diff, sinks, k, 0, r);
                        add_sink_pos_range(&mut diff, sinks, k, l, L - 1);
                    }
                }
            }

            let mut cur = 0;
            for (s, &d) in diff.iter().take(n).enumerate() {
                cur += d;
                if cur == n as i32 {
                    return Some(s);
                }
            }
            None
        }

        let mut lo = 0;
        let mut hi = L - 1;
        while lo < hi {
            let mid = (lo + hi) / 2;
            if feasible_shift(sources, sinks, dir, mid).is_some() {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }
        let best = lo;
        let best_shift = feasible_shift(sources, sinks, dir, best).unwrap();
        let n = sources.len();
        let pairs = (0..n)
            .map(|k| {
                let q = sinks[(k + best_shift) % n];
                (cost(sources[k], q), q)
            })
            .collect();
        (best, pairs)
    }

    fn collect_range_to_buffer(&mut self, base: i32) {
        let mut sources = vec![];
        let mut sinks = vec![];
        for idx in 0..L {
            let (mi, mj) = self.main[idx];
            let (bi, bj) = self.buffer[idx];
            if Self::is_stage_item(self.a[mi][mj], base) {
                sources.push(idx);
            }
            if !Self::is_stage_item(self.a[bi][bj], base) {
                sinks.push(idx);
            }
        }
        assert_eq!(sources.len(), sinks.len());

        let (cost_neg, pairs_neg) = Self::min_bottleneck_assignment(&sources, &sinks, -1);
        let (cost_pos, pairs_pos) = Self::min_bottleneck_assignment(&sources, &sinks, 1);
        let (dir, max_t, pairs) = if cost_neg <= cost_pos {
            (-1, cost_neg, pairs_neg)
        } else {
            (1, cost_pos, pairs_pos)
        };

        let mut schedule = vec![vec![]; max_t + 1];
        for (t, q) in pairs {
            schedule[t].push(q);
        }
        for (t, qs) in schedule.iter().enumerate() {
            if t > 0 {
                if dir == -1 {
                    self.rotate_main();
                } else {
                    self.rotate_main_rev();
                }
            }
            for &q in qs {
                self.swap_at(q);
            }
        }
    }

    fn load_range_sorted_from_buffer(&mut self, base: i32) -> (i32, usize) {
        let mut pos = vec![usize::MAX; L];
        let mut holes = vec![];
        for idx in 0..L {
            let (bi, bj) = self.buffer[idx];
            let x = self.a[bi][bj];
            if Self::is_range(x, base) {
                pos[(x - base) as usize] = idx;
            } else if x == -1 {
                holes.push(idx);
            }
        }

        for k in 0..L {
            let q = if pos[k] != usize::MAX {
                pos[k]
            } else if base + (k as i32) < self.target {
                holes.pop().unwrap()
            } else {
                panic!("box {} is not staged in buffer", base + k as i32);
            };
            pos[k] = q;
        }

        let mut best_score = usize::MAX;
        let mut best_load_dir = -1;
        let mut best_discharge_dir = -1;
        let mut best_offset = 0;
        let mut best_r = L;
        let mut best_post = 0;
        for load_dir in [-1, 1] {
            for discharge_dir in [-1, 1] {
                for offset in 0..L {
                    let post = if offset == 0 {
                        L - 1
                    } else if discharge_dir == -1 {
                        offset + L - 1
                    } else {
                        2 * L - offset - 1
                    };
                    for r in 0..L {
                        let mut ok = true;
                        for k in 0..L {
                            let desired = if discharge_dir == -1 {
                                (offset + k) % L
                            } else {
                                (offset + L - k % L) % L
                            };
                            let q = pos[k];
                            let t = if load_dir == -1 {
                                (desired + L - q + r) % L
                            } else {
                                (q + r + L - desired) % L
                            };
                            if t > r {
                                ok = false;
                                break;
                            }
                        }
                        if !ok {
                            continue;
                        }
                        let score = r + post;
                        if score < best_score {
                            best_score = score;
                            best_load_dir = load_dir;
                            best_discharge_dir = discharge_dir;
                            best_offset = offset;
                            best_r = r;
                            best_post = post;
                        }
                        break;
                    }
                }
            }
        }

        let mut schedule = vec![vec![]; best_r + 1];
        for k in 0..L {
            let desired = if best_discharge_dir == -1 {
                (best_offset + k) % L
            } else {
                (best_offset + L - k % L) % L
            };
            let q = pos[k];
            let t = if best_load_dir == -1 {
                (desired + L - q + best_r) % L
            } else {
                (q + best_r + L - desired) % L
            };
            schedule[t].push(q);
        }

        for (t, qs) in schedule.iter().enumerate() {
            for &q in qs {
                self.swap_at(q);
            }
            if t < best_r {
                if best_load_dir == -1 {
                    self.rotate_main();
                } else {
                    self.rotate_main_rev();
                }
            }
        }
        (best_discharge_dir, best_post)
    }

    fn discharge_one_cycle(&mut self, dir: i32, post: usize) {
        for _ in 0..post {
            if dir == -1 {
                self.rotate_main();
            } else {
                self.rotate_main_rev();
            }
        }
    }
}

fn main() {
    input! {
        n: usize,
        a: [[i32; n]; n],
    }
    assert_eq!(n, N);

    let main = build_main_cycle();
    let buffer = build_matching(&main);
    let mut solver = Solver::new(a, main.clone(), buffer.clone());

    solver.collect_range_to_buffer(0);
    let (dir0, pre0) = solver.load_range_sorted_from_buffer(0);
    solver.discharge_one_cycle(dir0, pre0);
    let (dir1, pre1) = solver.load_range_sorted_from_buffer(200);
    solver.discharge_one_cycle(dir1, pre1);

    println!("{}", 1 + L);
    print!("{}", L);
    for &(i, j) in &main {
        print!(" {} {}", i, j);
    }
    println!();
    for idx in 0..L {
        let (mi, mj) = main[idx];
        let (bi, bj) = buffer[idx];
        println!("2 {} {} {} {}", mi, mj, bi, bj);
    }
    println!("{}", solver.ops.len());
    for (m, d) in solver.ops {
        println!("{} {}", m, d);
    }
}
