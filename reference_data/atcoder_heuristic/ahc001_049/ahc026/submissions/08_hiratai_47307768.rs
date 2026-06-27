#[allow(unused_imports)]
use std::{cmp::*, collections::*, f64, i64, io, iter::*, mem::*, str::*, usize};

#[allow(unused_imports)]
use bitset_fixed::BitSet;
#[allow(unused_imports)]
use itertools::{chain, iproduct, iterate, izip, repeat_n, unfold, Itertools};
#[allow(unused_imports)]
use itertools_num::ItertoolsNum;
#[allow(unused_imports)]
use num::FromPrimitive;
#[allow(unused_imports)]
use num_derive::FromPrimitive;
#[allow(unused_imports)]
use rand::{
    distributions::{Distribution, Uniform},
    rngs::SmallRng,
    seq::{IteratorRandom, SliceRandom},
    Rng, SeedableRng,
};
#[allow(unused_imports)]
use rustc_hash::{FxHashMap, FxHashSet};

#[allow(unused_imports)]
use proconio::{
    input,
    marker::{Bytes, Chars, Isize1, Usize1},
    source::{Readable, Source},
};

// vec with some initial value
#[allow(unused_macros)]
macro_rules! vvec {
    ($($x:expr),+; $y:expr; $n:expr) => {{
        let mut v = vec![$y; $n];

        let mut it = v.iter_mut();
        $(
            *it.next().unwrap() = $x;
        )+

        v
    }}
}

#[allow(unused_macros)]
macro_rules! bitset {
    ($n:expr, $x:expr) => {{
        let mut bs = BitSet::new($n);
        if $n > 0 {
            bs.buffer_mut()[0] = $x as u64;
        }
        bs
    }};
}

#[allow(dead_code)]
fn println_opt<T: std::fmt::Display>(ans: Option<T>) {
    if let Some(ans) = ans {
        println!("{}", ans);
    } else {
        println!("-1");
    }
}

use easy_ext::ext;

#[ext(IterCopyExt)]
impl<'a, I, T> I
where
    Self: IntoIterator<Item = &'a T>,
    T: 'a + Copy,
{
    fn citer(self) -> std::iter::Copied<Self::IntoIter> {
        self.into_iter().copied()
    }
}

enum Digits {}

impl Readable for Digits {
    type Output = Vec<usize>;
    fn read<R: std::io::BufRead, S: Source<R>>(source: &mut S) -> Vec<usize> {
        source
            .next_token_unwrap()
            .chars()
            .map(|c| c.to_digit(10).unwrap() as usize)
            .collect()
    }
}

#[derive(Debug, Clone, Copy)]
struct Operation {
    target: usize,
    destination: Option<usize>,
}

impl Operation {
    fn move_to_box(target: usize, destination: usize) -> Operation {
        Operation {
            target,
            destination: Some(destination),
        }
    }

    fn move_to_outside(target: usize) -> Operation {
        Operation {
            target,
            destination: None,
        }
    }
}

impl std::fmt::Display for Operation {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "{} {}",
            self.target + 1,
            self.destination.map_or(0, |idx| idx + 1)
        )
    }
}

// pos_in_boxを含めたposよりも上のものを移動
fn move_above(
    n: usize,
    m: usize,
    box_idx: usize,
    pos_in_box: usize,
    b: &mut [Vec<usize>],
    idxs: &mut [(usize, usize)],
    mins: &mut [Vec<usize>],
    ops: &mut Vec<Operation>,
    perturb: &[Option<usize>],
) -> usize {
    assert!(pos_in_box < b[box_idx].len());

    let min_pos_in_above = pos_in_box + b[box_idx][pos_in_box..].citer().position_min().unwrap();
    let mut energy = 0;
    if min_pos_in_above != b[box_idx].len() - 1 {
        energy += move_above(
            n,
            m,
            box_idx,
            min_pos_in_above + 1,
            b,
            idxs,
            mins,
            ops,
            if perturb.is_empty() {
                perturb
            } else {
                &perturb[1..]
            },
        );
    }

    let item = b[box_idx][pos_in_box];

    let destination = if let Some(dest) =
        perturb
            .get(0)
            .copied()
            .flatten()
            .map(|dest| if dest < box_idx { dest } else { dest + 1 })
    {
        dest
    } else {
        (0..m)
            .filter(|&j| j != box_idx)
            .max_by_key(|&j| *mins[j].last().unwrap())
            .unwrap()
    };

    ops.push(Operation::move_to_box(item, destination));
    energy += b[box_idx].len() - pos_in_box + 1;

    let mut bb = take(&mut b[box_idx]);
    b[destination].reserve(bb.len() - pos_in_box);
    mins[destination].reserve(bb.len() - pos_in_box);
    for ii in bb[pos_in_box..].citer() {
        idxs[ii] = (destination, b[destination].len());
        b[destination].push(ii);
        let ml = *mins[destination].last().unwrap();
        mins[destination].push(min(ml, ii));
    }

    bb.truncate(pos_in_box);
    b[box_idx] = bb;
    mins[box_idx].truncate(pos_in_box + 1);

    energy
}

fn solve(
    n: usize,
    m: usize,
    b: &[Vec<usize>],
    idxs: &[(usize, usize)],
    perturbations: &[Vec<Option<usize>>],
) -> (Vec<Operation>, usize, Vec<usize>) {
    let mut ops = vec![];
    let mut energy = 0;

    let mut b = b.to_vec();
    let mut idxs = idxs.to_vec();
    let mut mins = b
        .iter()
        .map(|c| {
            once(usize::MAX)
                .chain(c.citer().scan(usize::MAX, |mi, x| {
                    *mi = min(*mi, x);
                    Some(*mi)
                }))
                .collect::<Vec<_>>()
        })
        .collect::<Vec<_>>();

    let mut num_moves = vec![];
    for i in 0..n {
        let (current_idx, pos_in_box) = idxs[i];
        assert!(b[current_idx][pos_in_box] == i);
        assert!(mins[current_idx][pos_in_box + 1] <= i);

        let num_move = if pos_in_box != b[current_idx].len() - 1 {
            let prev_num_ops = ops.len();
            energy += move_above(
                n,
                m,
                current_idx,
                pos_in_box + 1,
                &mut b,
                &mut idxs,
                &mut mins,
                &mut ops,
                &perturbations[i],
            );
            ops.len() - prev_num_ops
        } else {
            0
        };
        num_moves.push(num_move);

        assert!(b[current_idx].last() == Some(&i));

        ops.push(Operation::move_to_outside(i));
        b[current_idx].pop().unwrap();
        mins[current_idx].truncate(pos_in_box + 1);
    }

    (ops, energy, num_moves)
}

fn main() {
    let start = std::time::Instant::now();

    input! {
        n: usize, m: usize,
        b: [[Usize1; n / m]; m],
    };

    let idxs = b
        .iter()
        .enumerate()
        .fold(vec![(0, 0); n], |mut idxs, (box_idx, c)| {
            c.citer().enumerate().for_each(|(pos, idx)| {
                idxs[idx] = (box_idx, pos);
            });
            idxs
        });

    const TIME_LIMIT_MS: u128 = 1950;
    let temperature = || {
        let progress = start.elapsed().as_millis() as f64 / TIME_LIMIT_MS as f64;

        const START_TEMP: f64 = 5.0;
        const END_TEMP: f64 = 0.1;

        START_TEMP * (END_TEMP / START_TEMP).powf(progress)
    };

    let accept = |improve: i64, rng: &mut SmallRng| {
        if improve >= 0 {
            true
        } else {
            rng.gen_bool(f64::exp(improve as f64 / temperature()))
        }
    };

    let mut rng = SmallRng::seed_from_u64(42);

    let mut perturbations: Vec<Vec<Option<usize>>> = vec![vec![]; n];
    let mut ans = vec![];
    let mut min_energy = 1 << 40;
    let mut num_iters = 0;
    while start.elapsed().as_millis() < TIME_LIMIT_MS {
        num_iters += 1;

        #[derive(Debug)]
        enum Restore {
            One(usize, usize, Option<usize>),
            All(usize, Vec<Option<usize>>),
        }

        let mut restore: Option<Restore> = None;
        if let Some(i) = (0..n)
            .filter(|&i| !perturbations[i].is_empty())
            .choose(&mut rng)
        {
            let p = &mut perturbations[i];
            if rng.gen_bool(0.01) {
                restore = Some(Restore::All(i, take(p)));
            } else {
                let l = p.len();
                let idx = rng.gen_range(0..l);

                restore = Some(Restore::One(i, idx, p[idx]));
                if let Some(old) = p[idx] {
                    if rng.gen() {
                        let j = rng.gen_range(0..m - 2);
                        p[idx] = if j < old { Some(j) } else { Some(j + 1) };
                    } else {
                        p[idx] = None;
                    }
                } else {
                    p[idx] = Some(rng.gen_range(0..m - 1));
                }
            }
        }

        let (ops, energy, num_moves) = solve(n, m, &b, &idxs, &perturbations);
        assert!(num_moves.len() == n);

        let diff = min_energy as i64 - energy as i64;
        if accept(diff, &mut rng) {
            ans = ops;
            min_energy = energy;

            for (p, k) in izip!(perturbations.iter_mut(), num_moves) {
                p.resize(k, None);
            }
        } else if let Some(restore) = restore {
            match restore {
                Restore::One(i, idx, old) => {
                    perturbations[i][idx] = old;
                }
                Restore::All(i, old) => {
                    perturbations[i] = old;
                }
            }
        }
    }

    eprintln!("score = {}", max(1, 10000usize.saturating_sub(min_energy)));
    eprintln!("#iter={num_iters}");

    for op in ans {
        println!("{}", op);
    }
}
