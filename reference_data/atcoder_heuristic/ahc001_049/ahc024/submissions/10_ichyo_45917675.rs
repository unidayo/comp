pub use __cargo_equip::prelude::*;

use common::*;

fn main() {
    solve(&mut Atcoder {});
}

// The following code was expanded by `cargo-equip`.

///  # Bundled libraries
///
///  - `common 0.1.0 (path+████████████████████████████████████████████████████████████)` published in **missing** licensed under **missing** as `crate::__cargo_equip::crates::common`
#[allow(unused)]
mod __cargo_equip {
    pub(crate) mod crates {
        pub mod common {
            use std::{
                collections::VecDeque,
                env,
                time::{Duration, Instant},
            };

            use proconio::input;
            use rand::prelude::*;

            pub struct ProblemInput {
                pub n: usize,           // 50
                pub m: usize,           // 100
                pub c: Vec<Vec<usize>>, // 1-m
            }

            pub struct ProblemOutput {
                pub d: Vec<Vec<Vec<usize>>>, // 0-m
            }

            pub trait Environment {
                fn receive_input(&mut self) -> ProblemInput;
                fn send_output(&mut self, output: ProblemOutput);
                fn is_verbose(&self) -> bool;
                fn time_limit(&self) -> Duration;
            }

            pub struct Atcoder {}

            impl Environment for Atcoder {
                fn receive_input(&mut self) -> ProblemInput {
                    input! {
                        n: usize,
                        m: usize,
                        c: [[usize; n]; n],
                    }

                    ProblemInput { n, m, c }
                }

                fn send_output(&mut self, output: ProblemOutput) {
                    if env::var("ATCODER").is_ok() {
                        let d = output.d.last().unwrap();
                        for i in 0..d.len() {
                            for j in 0..d[i].len() {
                                print!("{}", d[i][j]);
                                if j != d[i].len() - 1 {
                                    print!(" ");
                                }
                            }
                            println!();
                        }
                    } else {
                        for d in output.d {
                            for i in 0..d.len() {
                                for j in 0..d[i].len() {
                                    print!("{}", d[i][j]);
                                    if j != d[i].len() - 1 {
                                        print!(" ");
                                    }
                                }
                                println!();
                            }
                        }
                    }
                }

                fn is_verbose(&self) -> bool {
                    env::var("ATCODER").is_err()
                }

                fn time_limit(&self) -> Duration {
                    if env::var("ATCODER").is_ok() {
                        Duration::from_millis(2000)
                    } else {
                        Duration::from_millis(1500)
                    }
                }
            }

            const DI: [isize; 4] = [0, 1, 0, -1];
            const DJ: [isize; 4] = [1, 0, -1, 0];

            fn generate_matrix(
                m: usize,
                num_row: usize,
                num_col: usize,
                c: &Vec<usize>,
            ) -> Vec<bool> {
                let mut ans = vec![false; (m + 1) * (m + 1)];
                for i in 0..=m {
                    ans[i * (m + 1) + i] = true;
                }

                for i in 0..num_row {
                    for j in 0..num_col {
                        let x = c[i * num_col + j];
                        for r in 0..4 {
                            let ni = i.wrapping_add_signed(DI[r]);
                            let nj = j.wrapping_add_signed(DJ[r]);
                            let y = if ni < num_row && nj < num_col {
                                c[ni * num_col + nj]
                            } else {
                                0
                            };
                            ans[x * (m + 1) + y] = true;
                            ans[y * (m + 1) + x] = true;
                        }
                    }
                }
                ans
            }

            fn is_connected(m: usize, num_row: usize, num_col: usize, cs: &Vec<usize>) -> bool {
                let mut visited = vec![false; num_col * num_row];
                let mut used = vec![false; m + 1];
                for i in 0..num_row {
                    for j in 0..num_col {
                        if !visited[i * num_col + j] {
                            let x = cs[i * num_col + j];
                            if used[x] {
                                return false;
                            }
                            used[x] = true;
                            let mut queue = VecDeque::new();
                            queue.push_back((i, j));
                            visited[i * num_col + j] = true;
                            while let Some((r, c)) = queue.pop_front() {
                                for d in 0..4 {
                                    let nr = r.wrapping_add_signed(DI[d]);
                                    let nc = c.wrapping_add_signed(DJ[d]);
                                    let (nr, nc) = if x == 0 {
                                        (
                                            if nr < num_row { nr } else { num_row - 1 },
                                            if nc < num_col { nc } else { num_col - 1 },
                                        )
                                    } else {
                                        (nr, nc)
                                    };

                                    if nr < num_row
                                        && nc < num_col
                                        && cs[nr * num_col + nc] == x
                                        && !visited[nr * num_col + nc]
                                    {
                                        visited[nr * num_col + nc] = true;
                                        queue.push_back((nr, nc));
                                    }
                                }
                            }
                        }
                    }
                }
                if used[0] {
                    let mut found = false;
                    for i in 0..num_row {
                        for j in 0..num_col {
                            if cs[i * num_col + j] == 0
                                && (i == 0 || i == num_row - 1 || j == 0 || j == num_col - 1)
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if !found {
                        return false;
                    }
                }
                true
            }

            pub fn expand(
                n: usize,
                num_row: usize,
                num_col: usize,
                cs: &Vec<usize>,
            ) -> Vec<Vec<usize>> {
                let mut res = vec![vec![0; n]; n];
                for i in 0..num_row {
                    for j in 0..num_col {
                        res[i][j] = cs[i * num_col + j];
                    }
                }
                res
            }

            pub fn solve<E: Environment>(environment: &mut E) {
                let mut rng = SmallRng::seed_from_u64(38942388931243u64);

                let start = Instant::now();
                let input = environment.receive_input();

                let m = input.m;

                let mut cs: Vec<usize> = Vec::new();
                for i in 0..input.n {
                    for j in 0..input.n {
                        cs.push(input.c[i][j]);
                    }
                }

                let matrix = generate_matrix(m, input.n, input.n, &cs);

                let mut ans = vec![expand(input.n, input.n, input.n, &cs)];

                let is_verbose = environment.is_verbose();

                let mut score = 1;

                let mut num_row = input.n;
                let mut num_col = input.n;

                let tl_1 = environment.time_limit() * 110 / 200;
                let tl_2 = environment.time_limit() * 150 / 200;
                let tl_3 = environment.time_limit() * 195 / 200;

                let update_per_loop = 300;
                let update_per_loop2 = 300;
                let update_per_loop3 = 300;

                let mut iteration = 0;
                let mut score_before = score;
                loop {
                    if start.elapsed() > tl_1 {
                        break;
                    }
                    iteration += 1;

                    'main: loop {
                        if start.elapsed() > tl_1 {
                            break;
                        }

                        enum Update {
                            Row(usize),
                            Col(usize),
                        }

                        let mut updates = vec![];
                        for i in 0..num_row {
                            updates.push(Update::Row(i));
                        }
                        for i in 0..num_col {
                            updates.push(Update::Col(i));
                        }

                        // updates.shuffle(&mut rng);

                        for update in updates {
                            if let Update::Row(i) = update {
                                // if (0..n).all(|j| cs[i][j] == 0) {
                                //     assert!(false);
                                //     continue;
                                // }

                                let mut ncs = vec![0; (num_row - 1) * num_col];
                                for r in 0..num_row - 1 {
                                    for c in 0..num_col {
                                        ncs[r * num_col + c] = if r >= i {
                                            cs[(r + 1) * num_col + c]
                                        } else {
                                            cs[r * num_col + c]
                                        };
                                    }
                                }

                                if is_connected(m, num_row - 1, num_col, &ncs)
                                    && matrix == generate_matrix(m, num_row - 1, num_col, &ncs)
                                {
                                    cs = ncs;
                                    score += num_col;
                                    num_row -= 1;
                                    continue 'main;
                                }
                            }
                            if let Update::Col(j) = update {
                                // if (0..n).all(|i| cs[i][j] == 0) {
                                //     assert!(false);
                                //     continue;
                                // }

                                let mut ncs = vec![0; num_row * (num_col - 1)];
                                for r in 0..num_row {
                                    for c in 0..num_col - 1 {
                                        ncs[r * (num_col - 1) + c] = if c >= j {
                                            cs[r * num_col + c + 1]
                                        } else {
                                            cs[r * num_col + c]
                                        };
                                    }
                                }

                                if is_connected(m, num_row, num_col - 1, &ncs)
                                    && matrix == generate_matrix(m, num_row, num_col - 1, &ncs)
                                {
                                    cs = ncs;
                                    score += num_row;
                                    num_col -= 1;
                                    continue 'main;
                                }
                            }
                        }
                        break;
                    }

                    for _ in 0..update_per_loop {
                        if start.elapsed() > tl_1 {
                            break;
                        }

                        let r = rng.gen_range(0..num_row);
                        let c = rng.gen_range(0..num_col);
                        let d = rng.gen_range(0..4);
                        let nr = r.wrapping_add_signed(DI[d]);
                        let nc = c.wrapping_add_signed(DJ[d]);
                        if nr >= num_row || nc >= num_col {
                            continue;
                        }
                        if cs[r * num_col + c] == 0 || cs[nr * num_col + nc] == 0 {
                            continue;
                        }
                        let cur = cs[r * num_col + c];
                        cs[r * num_col + c] = cs[nr * num_col + nc];
                        if is_connected(m, num_row, num_col, &cs)
                            && matrix == generate_matrix(m, num_row, num_col, &cs)
                        {
                            continue;
                        }
                        cs[r * num_col + c] = cur;
                    }
                }

                if environment.is_verbose() {
                    eprintln!("{} iter", iteration);
                    eprintln!("{} ms", start.elapsed().as_millis());
                    eprintln!("score: {} -> {}", score_before, score);
                }

                iteration = 0;
                score_before = score;
                loop {
                    if start.elapsed() > tl_2 {
                        break;
                    }
                    iteration += 1;

                    'main: loop {
                        if start.elapsed() > tl_2 {
                            break;
                        }
                        for i in 0..num_row {
                            for j in 0..num_col {
                                let cur = cs[i * num_col + j];
                                if cur == 0 {
                                    continue;
                                }

                                for d1 in 0..2 {
                                    for d2 in 0..2 {
                                        let mut ncs = vec![0; num_row * num_col];
                                        for r in 0..num_row {
                                            for c in 0..num_col {
                                                ncs[r * num_col + c] = if r == i {
                                                    if d1 == 0 && c >= j {
                                                        if c + 1 < num_col {
                                                            cs[r * num_col + c + 1]
                                                        } else {
                                                            0
                                                        }
                                                    } else if d1 == 1 && c <= j {
                                                        if c >= 1 {
                                                            cs[r * num_col + c - 1]
                                                        } else {
                                                            0
                                                        }
                                                    } else {
                                                        cs[r * num_col + c]
                                                    }
                                                } else if c == j {
                                                    if d2 == 0 && r >= i {
                                                        if r + 1 < num_row {
                                                            cs[(r + 1) * num_col + c]
                                                        } else {
                                                            0
                                                        }
                                                    } else if d2 == 1 && r <= i {
                                                        if r >= 1 {
                                                            cs[(r - 1) * num_col + c]
                                                        } else {
                                                            0
                                                        }
                                                    } else {
                                                        cs[r * num_col + c]
                                                    }
                                                } else {
                                                    cs[r * num_col + c]
                                                }
                                            }
                                        }

                                        if is_connected(m, num_row, num_col, &ncs)
                                            && matrix == generate_matrix(m, num_row, num_col, &ncs)
                                        {
                                            score += 1;
                                            cs = ncs;
                                            continue 'main;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }

                    for _ in 0..update_per_loop2 {
                        if start.elapsed() > tl_2 {
                            break;
                        }

                        let r = rng.gen_range(0..num_row);
                        let c = rng.gen_range(0..num_col);
                        let d = rng.gen_range(0..4);
                        let nr = r.wrapping_add_signed(DI[d]);
                        let nc = c.wrapping_add_signed(DJ[d]);
                        if nr >= num_row || nc >= num_col {
                            continue;
                        }
                        if cs[r * num_col + c] == 0 || cs[nr * num_col + nc] == 0 {
                            continue;
                        }
                        let cur = cs[r * num_col + c];
                        cs[r * num_col + c] = cs[nr * num_col + nc];
                        if is_connected(m, num_row, num_col, &cs)
                            && matrix == generate_matrix(m, num_row, num_col, &cs)
                        {
                            continue;
                        }
                        cs[r * num_col + c] = cur;
                    }
                }

                if is_verbose {
                    eprintln!("{} iter", iteration);
                    eprintln!("{} ms", start.elapsed().as_millis());
                    eprintln!(
                        "score: {} -> {} ({})",
                        score_before,
                        score,
                        score - score_before
                    );
                }

                iteration = 0;
                score_before = score;
                loop {
                    if start.elapsed() > tl_3 {
                        break;
                    }
                    iteration += 1;

                    'main: loop {
                        if start.elapsed() > tl_3 {
                            break;
                        }
                        for i in 0..num_row {
                            for j in 0..num_col {
                                let cur = cs[i * num_col + j];
                                if cur == 0 {
                                    continue;
                                }

                                let mut zero_connected = false;
                                for r in 0..4 {
                                    let ni = i.wrapping_add_signed(DI[r]);
                                    let nj = j.wrapping_add_signed(DJ[r]);
                                    if ni >= num_row || nj >= num_col || cs[ni * num_col + nj] == 0
                                    {
                                        zero_connected = true;
                                        break;
                                    }
                                }
                                if !zero_connected {
                                    continue;
                                }

                                cs[i * num_col + j] = 0;

                                if !is_connected(m, num_row, num_col, &cs) {
                                    cs[i * num_col + j] = cur;
                                    continue;
                                }

                                let new_matrix = generate_matrix(m, num_row, num_col, &cs);
                                if matrix != new_matrix {
                                    cs[i * num_col + j] = cur;
                                    continue;
                                }

                                score += 1;
                                continue 'main;
                            }
                        }
                        break;
                    }

                    for _ in 0..update_per_loop3 {
                        if start.elapsed() > tl_3 {
                            break;
                        }

                        let r = rng.gen_range(0..num_row);
                        let c = rng.gen_range(0..num_col);
                        let d = rng.gen_range(0..4);
                        let nr = r.wrapping_add_signed(DI[d]);
                        let nc = c.wrapping_add_signed(DJ[d]);
                        if nr >= num_row || nc >= num_col {
                            continue;
                        }
                        if cs[r * num_col + c] == 0 || cs[nr * num_col + nc] == 0 {
                            continue;
                        }
                        let cur = cs[r * num_col + c];
                        cs[r * num_col + c] = cs[nr * num_col + nc];
                        if is_connected(m, num_row, num_col, &cs)
                            && matrix == generate_matrix(m, num_row, num_col, &cs)
                        {
                            continue;
                        }
                        cs[r * num_col + c] = cur;
                    }
                }

                if environment.is_verbose() {
                    eprintln!("{} iter", iteration);
                    eprintln!("{} ms", start.elapsed().as_millis());
                    eprintln!(
                        "score: {} -> {} ({})",
                        score_before,
                        score,
                        score - score_before
                    );

                    eprintln!("score: {}", score);
                    eprintln!("est: {}", score * 150);
                }

                ans.push(expand(input.n, num_row, num_col, &cs));

                let output = ProblemOutput { d: ans };
                environment.send_output(output);
            }
        }
    }

    pub(crate) mod macros {
        pub mod common {}
    }

    pub(crate) mod prelude {
        pub use crate::__cargo_equip::crates::*;
    }

    mod preludes {
        pub mod common {}
    }
}
