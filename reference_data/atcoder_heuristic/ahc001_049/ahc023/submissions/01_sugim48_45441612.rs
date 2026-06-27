use std::{io, time::Instant};

use itertools::Itertools;
use proconio::{input, marker::Usize1, source::once::OnceSource};

const T: usize = 100;
const H: usize = 20;
const W: usize = 20;
const N: usize = H * W;

fn main() {
    let s = io::read_to_string(io::stdin()).unwrap();
    let task = parse_task(&s);
    let plants = solve(&task);

    println!("{}", plants.len());
    for Plant { c, y, x, t } in plants {
        println!("{} {} {} {}", c + 1, y, x, t + 1);
    }
}

struct Task {
    y0: usize,
    ah: Vec<Vec<bool>>,
    av: Vec<Vec<bool>>,
    crops: Vec<(usize, usize)>,
}
struct Plant {
    c: usize,
    y: usize,
    x: usize,
    t: usize,
}

fn parse_task(s: &str) -> Task {
    let source = OnceSource::from(s);

    input! {
        from source,
        _t: usize,
        _h: usize,
        _w: usize,
        y0: usize,
        ah: [String; H - 1],
        av: [String; H],
        c: usize,
        crops: [(Usize1, usize); c],
    }

    Task {
        y0,
        ah: ah
            .iter()
            .map(|row| row.chars().map(|x| x == '1').collect())
            .collect(),
        av: av
            .iter()
            .map(|row| row.chars().map(|x| x == '1').collect())
            .collect(),
        crops,
    }
}

fn dfs(i: usize, g: &Vec<Vec<usize>>, i_to_is_black: &mut Vec<bool>) {
    i_to_is_black[i] = false;
    for &ni in &g[i] {
        if i_to_is_black[ni] {
            dfs(ni, g, i_to_is_black);
        }
    }
}

fn calc_cc_num(g: &Vec<Vec<usize>>, pat: usize) -> usize {
    let mut i_to_is_black = vec![false; 9];
    let mapping = vec![0, 1, 2, 3, 5, 6, 7, 8, 4];
    for b in 0..9 {
        if (pat & (1 << b)) != 0 {
            i_to_is_black[mapping[b]] = true;
        }
    }

    let mut cc_num = 0;
    for i in 0..9 {
        if i_to_is_black[i] {
            cc_num += 1;
            dfs(i, g, &mut i_to_is_black);
        }
    }

    cc_num
}

#[inline(never)]
fn calc_score(dnms: &Vec<(usize, usize, usize)>, dl: usize, dr: usize, dm: usize) -> usize {
    let mut score = 0;
    let mut k = 0;
    for &(d, n, m) in dnms.iter().rev() {
        let m = if dl <= d && d <= dr { m + dm } else { m };
        let dk = n.min(m - k);
        k += dk;
        score += dk * d;
    }
    score
}

#[derive(Debug)]
#[allow(dead_code)]
pub struct Xorshift {
    seed: u64,
}
impl Xorshift {
    #[allow(dead_code)]
    pub fn new() -> Xorshift {
        Xorshift {
            seed: 0xf0fb588ca2196dac,
        }
    }
    #[allow(dead_code)]
    pub fn with_seed(seed: u64) -> Xorshift {
        Xorshift { seed: seed }
    }
    #[inline]
    #[allow(dead_code)]
    pub fn next(&mut self) -> u64 {
        self.seed = self.seed ^ (self.seed << 13);
        self.seed = self.seed ^ (self.seed >> 7);
        self.seed = self.seed ^ (self.seed << 17);
        self.seed
    }
    #[inline]
    #[allow(dead_code)]
    pub fn rand(&mut self, m: u64) -> u64 {
        self.next() % m
    }
    #[inline]
    #[allow(dead_code)]
    // 0.0 ~ 1.0
    pub fn randf(&mut self) -> f64 {
        use std::mem;
        const UPPER_MASK: u64 = 0x3FF0000000000000;
        const LOWER_MASK: u64 = 0xFFFFFFFFFFFFF;
        let tmp = UPPER_MASK | (self.next() & LOWER_MASK);
        let result: f64 = unsafe { mem::transmute(tmp) };
        result - 1.0
    }
}

fn solve(z: &Task) -> Vec<Plant> {
    let start_time = Instant::now();
    let mut xorshift = Xorshift::new();

    let mut g = vec![vec![]; N];
    for y in 0..H - 1 {
        for x in 0..W {
            if !z.ah[y][x] {
                g[y * W + x].push((y + 1) * W + x);
                g[(y + 1) * W + x].push(y * W + x);
            }
        }
    }
    for y in 0..H {
        for x in 0..W - 1 {
            if !z.av[y][x] {
                g[y * W + x].push(y * W + (x + 1));
                g[y * W + (x + 1)].push(y * W + x);
            }
        }
    }

    let get_ah = |y: isize, x: isize| -> bool {
        if (0..(H - 1) as isize).contains(&y) && (0..W as isize).contains(&x) {
            z.ah[y as usize][x as usize]
        } else {
            true
        }
    };
    let get_av = |y: isize, x: isize| -> bool {
        if (0..H as isize).contains(&y) && (0..(W - 1) as isize).contains(&x) {
            z.av[y as usize][x as usize]
        } else {
            true
        }
    };

    let mut i_pat_to_is_flippable = vec![vec![]; N];
    for y in 0..H {
        for x in 0..W {
            let mut g = vec![vec![]; 9];
            for dy in 0..2 {
                for dx in 0..3 {
                    if !get_ah(y as isize + dy as isize - 1, x as isize + dx as isize - 1) {
                        g[dy * 3 + dx].push((dy + 1) * 3 + dx);
                        g[(dy + 1) * 3 + dx].push(dy * 3 + dx);
                    }
                }
            }
            for dy in 0..3 {
                for dx in 0..2 {
                    if !get_av(y as isize + dy as isize - 1, x as isize + dx as isize - 1) {
                        g[dy * 3 + dx].push(dy * 3 + (dx + 1));
                        g[dy * 3 + (dx + 1)].push(dy * 3 + dx);
                    }
                }
            }

            i_pat_to_is_flippable[y * W + x] = (0..(1 << 8))
                .map(|pat| calc_cc_num(&g, pat) == calc_cc_num(&g, pat | (1 << 8)))
                .collect_vec();
        }
    }

    let mut t_to_dcs = vec![vec![]; T + 1];
    for c in 0..z.crops.len() {
        let (tl, tr) = z.crops[c];
        t_to_dcs[tr].push((tr - tl, c));
    }
    for dcs in &mut t_to_dcs {
        dcs.sort();
        dcs.reverse();
    }

    let t_to_ds = t_to_dcs
        .iter()
        .map(|dis| dis.iter().map(|&(d, _)| d).sorted().collect_vec())
        .collect_vec();

    let mut t_to_dnms = t_to_ds
        .iter()
        .map(|ds| {
            ds.iter()
                .counts()
                .iter()
                .map(|(&&d, &n)| (d, n, 0))
                .sorted()
                .collect_vec()
        })
        .collect_vec();

    let t_to_min_d = t_to_ds
        .iter()
        .map(|ds| *ds.first().unwrap_or(&(T + 1)))
        .collect_vec();

    let mut ti_to_is_reachable = vec![vec![false; N]; T + 1];
    for t in 0..=T {
        for i in 0..N {
            ti_to_is_reachable[t][i] = (t == 0) || (t == T) || (i == z.y0 * W);
        }
    }

    let mut ti_to_pat = vec![vec![0usize; N]; T + 1];
    let mut ti_to_is_flippable = vec![vec![false; N]; T + 1];

    let mut ti_to_prev_t = vec![vec![]; T + 1];
    let mut ti_to_next_t = vec![vec![]; T + 1];
    let mut t_to_score = vec![0usize; T + 1];

    let mut calc_layer = |t: usize| {
        for i in 0..N {
            if ti_to_is_reachable[t][i] {
                let y = i / W;
                let x = i % W;

                if y > 0 && x > 0 {
                    let ni = i - W - 1;
                    ti_to_pat[t][ni] ^= 1 << 7;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if y > 0 {
                    let ni = i - W;
                    ti_to_pat[t][ni] ^= 1 << 6;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if y > 0 && x < W - 1 {
                    let ni = i - W + 1;
                    ti_to_pat[t][ni] ^= 1 << 5;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if x > 0 {
                    let ni = i - 1;
                    ti_to_pat[t][ni] ^= 1 << 4;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if x < W - 1 {
                    let ni = i + 1;
                    ti_to_pat[t][ni] ^= 1 << 3;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if y < H - 1 && x > 0 {
                    let ni = i + W - 1;
                    ti_to_pat[t][ni] ^= 1 << 2;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if y < H - 1 {
                    let ni = i + W;
                    ti_to_pat[t][ni] ^= 1 << 1;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
                if y < H - 1 && x < W - 1 {
                    let ni = i + W + 1;
                    ti_to_pat[t][ni] ^= 1 << 0;
                    ti_to_is_flippable[t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[t][ni]];
                }
            }
        }

        ti_to_prev_t[t] = vec![0; N];
        ti_to_next_t[t] = vec![T; N];

        if t > 0 {
            for i in 0..N {
                let prev_t = &mut ti_to_prev_t[t][i];
                *prev_t = t - 1;
                while !ti_to_is_reachable[*prev_t][i] {
                    *prev_t -= 1;
                }
            }
        }
        if t < T {
            for i in 0..N {
                let next_t = &mut ti_to_next_t[t][i];
                *next_t = t + 1;
                while !ti_to_is_reachable[*next_t][i] {
                    *next_t += 1;
                }
            }
        }

        if t > 0 {
            for i in 0..N {
                if ti_to_is_reachable[t][i] {
                    for (d, _, m) in &mut t_to_dnms[t] {
                        if *d <= t - ti_to_prev_t[t][i] {
                            *m += 1;
                        }
                    }
                }
            }
        }

        t_to_score[t] = calc_score(&t_to_dnms[t], 0, 0, 0);
    };

    for t in 0..=T {
        calc_layer(t);
    }

    let mut score: usize = t_to_score.iter().sum();

    const TIME_LIMIT_MILLIS: u128 = 1950;
    let start_temp = 1.0f64;
    let end_temp = 0f64;
    let mut temp = 0f64;

    let mut round = 0;
    let mut flip_cnt = 0;

    loop {
        if round % 10000 == 0 {
            let elapsed_millis = (Instant::now() - start_time).as_millis();

            if elapsed_millis > TIME_LIMIT_MILLIS {
                eprintln!("{} {} {} {}", round, flip_cnt, score, temp);
                break;
            }

            let ratio = (elapsed_millis as f64) / (TIME_LIMIT_MILLIS as f64);
            temp = (start_temp * (1.0 - ratio) + end_temp * ratio).powf(0.6);
        }
        if round % 100000 == 0 {
            eprintln!("{} {} {} {}", round, flip_cnt, score, temp);
        }
        round += 1;

        let cur_t = 1 + xorshift.rand((T - 1) as u64) as usize;
        let i = loop {
            let i = xorshift.rand(N as u64) as usize;
            if ti_to_is_flippable[cur_t][i] && i != z.y0 * W {
                break i;
            }
        };

        let is_add = !ti_to_is_reachable[cur_t][i];
        let prev_t = ti_to_prev_t[cur_t][i];
        let next_t = ti_to_next_t[cur_t][i];

        let delta_cur_score = if cur_t - prev_t < t_to_min_d[cur_t] {
            0
        } else {
            calc_score(
                &t_to_dnms[cur_t],
                0,
                cur_t - prev_t,
                if is_add { 1 } else { usize::MAX },
            ) as isize
                - t_to_score[cur_t] as isize
        };

        let delta_next_score = if next_t - prev_t < t_to_min_d[next_t] {
            0
        } else {
            calc_score(
                &t_to_dnms[next_t],
                next_t - cur_t + 1,
                next_t - prev_t,
                if is_add { usize::MAX } else { 1 },
            ) as isize
                - t_to_score[next_t] as isize
        };

        let delta_score = delta_cur_score + delta_next_score;

        if delta_score >= 0 || xorshift.randf() < (delta_score as f64 / temp).exp() {
            flip_cnt += 1;
            score += delta_score as usize;
            t_to_score[cur_t] += delta_cur_score as usize;
            t_to_score[next_t] += delta_next_score as usize;

            ti_to_is_reachable[cur_t][i] = is_add;

            for t in prev_t..=cur_t - 1 {
                ti_to_next_t[t][i] = if is_add { cur_t } else { next_t };
            }
            for t in cur_t + 1..=next_t {
                ti_to_prev_t[t][i] = if is_add { cur_t } else { prev_t };
            }

            let y = i / W;
            let x = i % W;

            if y > 0 && x > 0 {
                let ni = i - W - 1;
                ti_to_pat[cur_t][ni] ^= 1 << 7;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if y > 0 {
                let ni = i - W;
                ti_to_pat[cur_t][ni] ^= 1 << 6;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if y > 0 && x < W - 1 {
                let ni = i - W + 1;
                ti_to_pat[cur_t][ni] ^= 1 << 5;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if x > 0 {
                let ni = i - 1;
                ti_to_pat[cur_t][ni] ^= 1 << 4;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if x < W - 1 {
                let ni = i + 1;
                ti_to_pat[cur_t][ni] ^= 1 << 3;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if y < H - 1 && x > 0 {
                let ni = i + W - 1;
                ti_to_pat[cur_t][ni] ^= 1 << 2;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if y < H - 1 {
                let ni = i + W;
                ti_to_pat[cur_t][ni] ^= 1 << 1;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }
            if y < H - 1 && x < W - 1 {
                let ni = i + W + 1;
                ti_to_pat[cur_t][ni] ^= 1 << 0;
                ti_to_is_flippable[cur_t][ni] = i_pat_to_is_flippable[ni][ti_to_pat[cur_t][ni]];
            }

            if is_add {
                for (d, _, m) in t_to_dnms[cur_t].iter_mut() {
                    if *d <= cur_t - prev_t {
                        *m += 1;
                    } else {
                        break;
                    }
                }
                for (d, _, m) in &mut t_to_dnms[next_t].iter_mut() {
                    if next_t - cur_t + 1 <= *d {
                        if *d <= next_t - prev_t {
                            *m -= 1;
                        } else {
                            break;
                        }
                    }
                }
            } else {
                for (d, _, m) in t_to_dnms[cur_t].iter_mut() {
                    if *d <= cur_t - prev_t {
                        *m -= 1;
                    } else {
                        break;
                    }
                }
                for (d, _, m) in &mut t_to_dnms[next_t].iter_mut() {
                    if next_t - cur_t + 1 <= *d {
                        if *d <= next_t - prev_t {
                            *m += 1;
                        } else {
                            break;
                        }
                    }
                }
            }
        }
    }

    let mut plants = vec![];

    for t in 1..=T {
        let mut eis = vec![];

        for i in 0..N {
            if ti_to_is_reachable[t][i] {
                eis.push((t - ti_to_prev_t[t][i], i));
            }
        }

        eis.sort();
        eis.reverse();

        let mut k = 0;
        for (e, i) in eis {
            while let Some(&(d, c)) = t_to_dcs[t].get(k) {
                k += 1;
                if d <= e {
                    plants.push(Plant {
                        c,
                        y: i / W,
                        x: i % W,
                        t: t - e,
                    });
                    break;
                }
            }
        }
    }

    plants
}
