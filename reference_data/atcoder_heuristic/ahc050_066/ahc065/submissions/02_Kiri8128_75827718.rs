use std::io::{self, BufRead};

const N: usize = 20;
const EMPTY: usize = 400;

type Pos = (usize, usize);

#[derive(Clone)]
struct Group {
    cells: Vec<Pos>,
    loop_k: usize,   // cells[loop_k] が大ループ上のマス
    main_idx: usize, // 大ループ上での index
}

fn is_ready_box(x: usize, l_len: usize, next_to_remove: usize) -> bool {
    if x == EMPTY {
        return false;
    }
    x < l_len || x - l_len < next_to_remove
}

fn target_slot(x: usize, l_len: usize) -> usize {
    x % l_len
}

fn rotate_group(grid: &mut Vec<Vec<usize>>, g: &Group, d: i32) {
    let len = g.cells.len();
    let old: Vec<usize> = g.cells.iter().map(|&(r, c)| grid[r][c]).collect();

    for k in 0..len {
        let nk = if d == 1 {
            (k + 1) % len
        } else {
            (k + len - 1) % len
        };
        let (nr, nc) = g.cells[nk];
        grid[nr][nc] = old[k];
    }
}

fn dir_to_bring_to_loop(g: &Group, from_k: usize) -> i32 {
    let len = g.cells.len();
    let cw = (g.loop_k + len - from_k) % len; // d = +1 を cw 回
    let ccw = (from_k + len - g.loop_k) % len; // d = -1 を ccw 回

    if cw <= ccw {
        1
    } else {
        -1
    }
}

fn main() {
    let stdin = io::stdin();
    let mut lines = stdin.lock().lines();

    let n_line = lines.next().unwrap().unwrap();
    let n: usize = n_line.trim().parse().unwrap();
    assert_eq!(n, N);

    let mut init_grid = vec![vec![0; n]; n];
    for i in 0..n {
        let line = lines.next().unwrap().unwrap();
        let parts: Vec<usize> = line
            .split_whitespace()
            .map(|s| s.parse().unwrap())
            .collect();
        for j in 0..n {
            init_grid[i][j] = parts[j];
        }
    }

    // --- 1. 新しい大ループの構築 ---
    //
    // 解釈:
    // - 上端は (0,1) -> (0,2) -> (0,3) から開始
    // - 0,4,8,12,16 行は右向き
    // - 2,6,10,14,18 行は左向き
    // - 右端側は col=18 で 2 段下りる
    // - 左寄り側は col=3 で 2 段下りる
    // - 最後に (18,3)->(18,2)->(18,1) から col=1 を上がって閉じる
    //
    // ユーザー指定文中の reverse 側の "(i,1)->..." は、
    // 4マスグループ例や j=2 のペア規則と整合するように、
    // 大ループ本体は col=3 側を通す形にしている。
    let mut loop_cells: Vec<Pos> = Vec::new();

    loop_cells.push((0, 1));
    loop_cells.push((0, 2));

    for block in 0..5 {
        let i = block * 4;

        if loop_cells.last().copied() != Some((i, 3)) {
            loop_cells.push((i, 3));
        }

        // (i,3) -> ... -> (i,18)
        for j in 4..=18 {
            loop_cells.push((i, j));
        }

        if i + 2 < N {
            // (i,18) -> (i+1,18) -> (i+2,18)
            loop_cells.push((i + 1, 18));
            loop_cells.push((i + 2, 18));

            // (i+2,18) -> ... -> (i+2,3)
            for j in (3..=17).rev() {
                loop_cells.push((i + 2, j));
            }

            if i + 4 < N {
                // (i+2,3) -> (i+3,3) -> (i+4,3)
                loop_cells.push((i + 3, 3));
                loop_cells.push((i + 4, 3));
            }
        }
    }

    // (18,3) -> (18,2) -> (18,1)
    loop_cells.push((18, 2));
    loop_cells.push((18, 1));

    // (18,1) -> (17,1) -> ... -> (1,1) -> 隣接して (0,1) に戻る
    for i in (1..=17).rev() {
        loop_cells.push((i, 1));
    }

    let l_len = loop_cells.len();

    let mut in_loop = vec![vec![false; n]; n];
    let mut loop_pos_to_idx = vec![vec![usize::MAX; n]; n];

    for (idx, &(r, c)) in loop_cells.iter().enumerate() {
        assert!(!in_loop[r][c], "duplicated loop cell: ({}, {})", r, c);
        in_loop[r][c] = true;
        loop_pos_to_idx[r][c] = idx;
    }

    // 念のため隣接チェック
    for k in 0..l_len {
        let a = loop_cells[k];
        let b = loop_cells[(k + 1) % l_len];
        let dist = a.0.abs_diff(b.0) + a.1.abs_diff(b.1);
        assert_eq!(dist, 1, "loop is not adjacent at {}", k);
    }

    // --- 2. 小コンベアグループの構築 ---
    //
    // 基本は 2 マスペア。
    // ただし、角の 2x2 は 4 マスサイクルにする。
    let mut groups: Vec<Group> = Vec::new();
    let mut used_small = vec![vec![false; n]; n];

    let mut add_group = |cells: Vec<Pos>,
                         in_loop: &Vec<Vec<bool>>,
                         loop_pos_to_idx: &Vec<Vec<usize>>,
                         groups: &mut Vec<Group>,
                         used_small: &mut Vec<Vec<bool>>| {
        let mut loop_k_opt = None;

        for (k, &(r, c)) in cells.iter().enumerate() {
            if in_loop[r][c] {
                loop_k_opt = Some(k);
            }
        }

        let loop_k = loop_k_opt.unwrap();
        let loop_cell = cells[loop_k];
        let main_idx = loop_pos_to_idx[loop_cell.0][loop_cell.1];

        for (k, &(r, c)) in cells.iter().enumerate() {
            if k != loop_k {
                assert!(!used_small[r][c], "duplicated small cell: ({}, {})", r, c);
                used_small[r][c] = true;
            }
        }

        // 小コンベア自体の隣接チェック
        let len = cells.len();
        for k in 0..len {
            let a = cells[k];
            let b = cells[(k + 1) % len];
            let dist = a.0.abs_diff(b.0) + a.1.abs_diff(b.1);
            assert_eq!(dist, 1, "small group is not adjacent");
        }

        groups.push(Group {
            cells,
            loop_k,
            main_idx,
        });
    };

    // 右端側の 4 マスグループ:
    // (2,18),(2,19),(3,19),(3,18)
    // (6,18),(6,19),(7,19),(7,18)
    // ...
    // (18,18),(18,19),(19,19),(19,18)
    for &r in &[2usize, 6, 10, 14, 18] {
        let cells = vec![(r, 18), (r, 19), (r + 1, 19), (r + 1, 18)];
        add_group(
            cells,
            &in_loop,
            &loop_pos_to_idx,
            &mut groups,
            &mut used_small,
        );
    }

    // 左寄り側の 4 マスグループ:
    // (4,2),(4,3),(5,3),(5,2)
    // (8,2),(8,3),(9,3),(9,2)
    // ...
    for &r in &[4usize, 8, 12, 16] {
        let cells = vec![(r, 2), (r, 3), (r + 1, 3), (r + 1, 2)];
        add_group(
            cells,
            &in_loop,
            &loop_pos_to_idx,
            &mut groups,
            &mut used_small,
        );
    }

    // 左下の 4 マスグループ:
    // (18,0),(18,1),(19,1),(19,0)
    add_group(
        vec![(18, 0), (18, 1), (19, 1), (19, 0)],
        &in_loop,
        &loop_pos_to_idx,
        &mut groups,
        &mut used_small,
    );

    // 残りの非ループセルは 2 マスペアにする。
    for r in 0..n {
        for c in 0..n {
            if in_loop[r][c] || used_small[r][c] {
                continue;
            }

            let in_pos = if c == 0 {
                (r, c + 1)
            } else if c == 19 {
                (r, c - 1)
            } else if c == 2 && in_loop[r][c + 1] {
                (r, c + 1)
            } else {
                assert!(r > 0);
                (r - 1, c)
            };

            assert!(
                in_loop[in_pos.0][in_pos.1],
                "pair target is not in main loop: out=({},{}), in=({},{})",
                r,
                c,
                in_pos.0,
                in_pos.1
            );

            let cells = vec![(r, c), in_pos];
            add_group(
                cells,
                &in_loop,
                &loop_pos_to_idx,
                &mut groups,
                &mut used_small,
            );
        }
    }

    // 各マスが含まれるコンベア数は高々 2。
    // 大ループ 1 回 + 小コンベア 1 回まで。
    let mut cover = vec![vec![0usize; n]; n];
    for &(r, c) in &loop_cells {
        cover[r][c] += 1;
    }
    for g in &groups {
        for &(r, c) in &g.cells {
            cover[r][c] += 1;
        }
    }
    for r in 0..n {
        for c in 0..n {
            assert!(cover[r][c] <= 2, "cell covered too much: ({}, {})", r, c);
        }
    }

    // --- 3. 全パターンのシミュレーション探索 ---
    let mut best_score: i64 = -1;
    let mut best_moves: Vec<(usize, i32)> = Vec::new();
    let mut best_b = 0usize;

    // 大ループ上のすべての位置を slot 0 の基準点として試す
    for idx0 in 0..l_len {
        let mut grid = init_grid.clone();
        let mut next_to_remove = 0usize;
        let mut moves: Vec<(usize, i32)> = Vec::new();
        let mut offset = 0usize;

        // 初期状態での搬出チェック
        if grid[0][10] == next_to_remove {
            grid[0][10] = EMPTY;
            next_to_remove += 1;
        }

        while next_to_remove < 400 && moves.len() < 100000 {
            let mut moved = false;

            // Rule A: 乗せる処理を優先
            //
            // 小コンベア内の非ループセルに ready な箱があり、
            // その箱の target slot が現在の入口 slot と一致するなら、
            // その箱がループへ近づく方向に 1 手だけ回す。
            for gi in 0..groups.len() {
                let g = &groups[gi];

                let idx = g.main_idx;
                let current_slot = (idx0 + offset + l_len - idx) % l_len;

                let mut chosen: Option<i32> = None;

                for k in 0..g.cells.len() {
                    if k == g.loop_k {
                        continue;
                    }

                    let (r, c) = g.cells[k];
                    let b = grid[r][c];

                    if is_ready_box(b, l_len, next_to_remove)
                        && target_slot(b, l_len) == current_slot
                    {
                        chosen = Some(dir_to_bring_to_loop(g, k));
                        break;
                    }
                }

                if let Some(d) = chosen {
                    rotate_group(&mut grid, g, d);
                    moves.push((gi + 1, d));
                    moved = true;
                    break;
                }
            }

            if moved {
                if grid[0][10] == next_to_remove {
                    grid[0][10] = EMPTY;
                    next_to_remove += 1;
                }
                continue;
            }

            // Rule B: 降ろす処理
            //
            // ループ上に ready だが現在 slot と合わない箱がある場合、
            // 小コンベアの隣接側からループへ入ってくる箱が not-ready/空なら回す。
            for gi in 0..groups.len() {
                let g = &groups[gi];

                let loop_pos = g.cells[g.loop_k];
                let in_box = grid[loop_pos.0][loop_pos.1];

                let idx = g.main_idx;
                let current_slot = (idx0 + offset + l_len - idx) % l_len;

                if !is_ready_box(in_box, l_len, next_to_remove) {
                    continue;
                }
                if target_slot(in_box, l_len) == current_slot {
                    continue;
                }

                let len = g.cells.len();

                let mut chosen: Option<i32> = None;

                for &d in &[1, -1] {
                    let incoming_k = if d == 1 {
                        (g.loop_k + len - 1) % len
                    } else {
                        (g.loop_k + 1) % len
                    };

                    let incoming_pos = g.cells[incoming_k];
                    let incoming_box = grid[incoming_pos.0][incoming_pos.1];

                    if !is_ready_box(incoming_box, l_len, next_to_remove) {
                        chosen = Some(d);
                        break;
                    }
                }

                if let Some(d) = chosen {
                    rotate_group(&mut grid, g, d);
                    moves.push((gi + 1, d));
                    moved = true;
                    break;
                }
            }

            if moved {
                if grid[0][10] == next_to_remove {
                    grid[0][10] = EMPTY;
                    next_to_remove += 1;
                }
                continue;
            }

            // Rule C: どの小コンベアも動かせなければ大コンベアを前進
            let last_val = grid[loop_cells[l_len - 1].0][loop_cells[l_len - 1].1];

            for k in (1..l_len).rev() {
                let curr = loop_cells[k];
                let prev = loop_cells[k - 1];
                grid[curr.0][curr.1] = grid[prev.0][prev.1];
            }

            grid[loop_cells[0].0][loop_cells[0].1] = last_val;

            offset = (offset + 1) % l_len;
            moves.push((0, 1));

            if grid[0][10] == next_to_remove {
                grid[0][10] = EMPTY;
                next_to_remove += 1;
            }
        }

        let b = next_to_remove;
        let t = moves.len();

        let score = if b == 400 {
            1_000_000 + (1_000_000.0 * (100_000.0 / t as f64).log2()).round() as i64
        } else {
            (1_000_000.0 * (b as f64) / 400.0).round() as i64
        };

        if score > best_score {
            best_score = score;
            best_moves = moves;
            best_b = b;
        }
    }

    // --- 4. 出力 ---
    let mut out = String::new();
    let total_conveyors = groups.len() + 1;

    out.push_str(&format!("{}\n", total_conveyors));

    // メインループ ID: 0
    out.push_str(&format!("{}", l_len));
    for &(r, c) in &loop_cells {
        out.push_str(&format!(" {} {}", r, c));
    }
    out.push('\n');

    // 小コンベア ID: 1 ~
    for g in &groups {
        out.push_str(&format!("{}", g.cells.len()));
        for &(r, c) in &g.cells {
            out.push_str(&format!(" {} {}", r, c));
        }
        out.push('\n');
    }

    out.push_str(&format!("{}\n", best_moves.len()));
    for &(m, d) in &best_moves {
        out.push_str(&format!("{} {}\n", m, d));
    }

    print!("{}", out);

    eprintln!("[Info] main loop length: {}", l_len);
    eprintln!("[Info] small conveyors: {}", groups.len());
    eprintln!("[Best] 回収した箱数: {}", best_b);
    eprintln!("[Best] かかったターン数: {}", best_moves.len());
    eprintln!("[Best] スコア: {}", best_score);
}