use proconio::input;
use std::time::Instant;

// --- 構造体定義 ---

#[derive(Clone, Copy, Debug)]
struct Point {
    x: f64,
    y: f64,
}

#[derive(Clone, Copy, Debug)]
struct Velocity {
    vx: f64,
    vy: f64,
}

#[derive(Clone, Copy, Debug)]
struct Operation {
    t: usize,
    u: usize,
    v: usize,
}

#[derive(Clone, Debug)]
struct ClusterResult {
    cost: i64,
    operations: Vec<Operation>,
    member_indices: Vec<usize>,
}

struct XorShift {
    state: u64,
}

impl XorShift {
    fn new(seed: u64) -> Self {
        let mut state = seed;
        if state == 0 {
            state = 123456789;
        }
        Self { state }
    }

    #[inline]
    fn next_u64(&mut self) -> u64 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        self.state = x;
        x
    }

    #[inline]
    fn gen_range(&mut self, n: usize) -> usize {
        (self.next_u64() % (n as u64)) as usize
    }

    #[inline]
    fn next_f64(&mut self) -> f64 {
        (self.next_u64() >> 11) as f64 * (1.0 / 9007199254740992.0)
    }
}

// --- 計算用関数 (インライン化・最適化) ---

#[inline]
fn normalize(val: f64, l: f64) -> f64 {
    // 多くの場合は範囲内か少し外れる程度なので、%演算の前にチェックを入れる
    if val >= 0.0 && val < l {
        return val;
    }
    let mut v = val % l;
    if v < 0.0 {
        v += l;
    }
    v
}

#[inline]
fn get_dist_sq(p1: Point, p2: Point, l: f64) -> f64 {
    let mut dx = (p1.x - p2.x).abs();
    let mut dy = (p1.y - p2.y).abs();
    // 条件分岐最適化: Lの半分より大きい場合のみ補正
    let half_l = l * 0.5;
    if dx > half_l {
        dx = l - dx;
    }
    if dy > half_l {
        dy = l - dy;
    }
    dx * dx + dy * dy
}

#[inline]
fn calc_bond_cost(p1: Point, p2: Point, l: f64) -> i64 {
    get_dist_sq(p1, p2, l).sqrt().round() as i64
}

#[inline]
fn predict_pos(p: Point, v: Velocity, l: f64, dt: i64) -> Point {
    let dt_f = dt as f64;
    Point {
        x: normalize(p.x + v.vx * dt_f, l),
        y: normalize(p.y + v.vy * dt_f, l),
    }
}

// --- メモリ再利用のためのコンテキスト ---

struct ClusterGenContext {
    member_pos: Vec<Point>,
    temp_cluster_pos: Vec<Point>,
    is_in_cluster: Vec<bool>,
    operations: Vec<Operation>,
    cluster_members: Vec<usize>,
}

impl ClusterGenContext {
    fn new(n: usize, k: usize) -> Self {
        Self {
            member_pos: Vec::with_capacity(k),
            temp_cluster_pos: Vec::with_capacity(k),
            is_in_cluster: vec![false; n],
            operations: Vec::with_capacity(k),
            cluster_members: Vec::with_capacity(k),
        }
    }
}

// --- コアロジック: 単一クラスタの生成（初期解用） ---

fn generate_single_cluster(
    k: usize,
    l: f64,
    initial_pos: &Vec<Point>,
    initial_vel: &Vec<Velocity>,
    candidates: &Vec<usize>,
    rng: &mut XorShift,
    weight_params: (f64, f64),
    ctx: &mut ClusterGenContext, // コンテキストを受け取る
) -> ClusterResult {
    let (time_weight, time_power) = weight_params;

    if candidates.is_empty() {
        return ClusterResult {
            cost: 0,
            operations: vec![],
            member_indices: vec![],
        };
    }

    // コンテキストのリセット
    ctx.member_pos.clear();
    ctx.operations.clear();
    ctx.cluster_members.clear();
    // is_in_cluster は呼び出し元または末尾でリセットする運用だが、
    // ここでは初期化済みの前提で、使用した分だけfalseに戻す

    // 1. Seed選択
    let seed_idx_in_candidates = rng.gen_range(candidates.len());
    let seed_node = candidates[seed_idx_in_candidates];

    ctx.cluster_members.push(seed_node);
    let mut cluster_v = initial_vel[seed_node];
    ctx.member_pos.push(initial_pos[seed_node]);

    ctx.is_in_cluster[seed_node] = true;

    let mut total_cost = 0;
    let mut current_time = 0;

    // 2. サイズKになるまで追加
    while ctx.cluster_members.len() < k {
        let mut best_eval_score = f64::MAX;
        let mut best_real_cost = 0;
        let mut best_target_node = 0;
        let mut best_source_node = 0;
        let mut best_t = 0;
        let mut found_any = false;

        let step = 10;

        // 時間探索
        for t in (current_time..1000).step_by(step) {
            let dt = (t - current_time) as i64;
            let dt_f = dt as f64;

            let penalty = time_weight * dt_f.powf(time_power);
            if penalty >= best_eval_score {
                break;
            }

            // バッファ再利用
            ctx.temp_cluster_pos.clear();
            for &p in &ctx.member_pos {
                ctx.temp_cluster_pos.push(predict_pos(p, cluster_v, l, dt));
            }

            // 候補点探索
            for &cand_idx in candidates {
                if ctx.is_in_cluster[cand_idx] {
                    continue;
                }

                let p_cand = predict_pos(initial_pos[cand_idx], initial_vel[cand_idx], l, t as i64);

                for (i, &p_mem) in ctx.temp_cluster_pos.iter().enumerate() {
                    // 高速化: まずsqrtなしで比較判定する
                    // real_cost = sqrt(dist_sq).round()
                    // score = real_cost + penalty
                    // もし sqrt(dist_sq) + penalty >= best_eval_score なら更新の余地なし
                    // ただし round() の挙動があるため、厳密には安全マージンが必要だが、
                    // ここでは元のロジックを尊重しつつ、dist_sq計算後に即座にsqrtして比較する。

                    let dist_sq = get_dist_sq(p_mem, p_cand, l);
                    let real_cost_f = dist_sq.sqrt();
                    let real_cost = real_cost_f.round() as i64;
                    let eval_score = (real_cost as f64) + penalty;

                    if eval_score < best_eval_score {
                        best_eval_score = eval_score;
                        best_real_cost = real_cost;
                        best_target_node = cand_idx;
                        best_source_node = ctx.cluster_members[i];
                        best_t = t;
                        found_any = true;
                    }
                }
            }
        }

        // フォールバック
        if !found_any {
            best_t = current_time;
            let mut min_dist = i64::MAX;

            for &cand_idx in candidates {
                if ctx.is_in_cluster[cand_idx] {
                    continue;
                }

                let p_cand = predict_pos(
                    initial_pos[cand_idx],
                    initial_vel[cand_idx],
                    l,
                    best_t as i64,
                );

                for (i, &p_mem) in ctx.member_pos.iter().enumerate() {
                    let cost = calc_bond_cost(p_mem, p_cand, l);
                    if cost < min_dist {
                        min_dist = cost;
                        best_target_node = cand_idx;
                        best_source_node = ctx.cluster_members[i];
                        best_real_cost = cost;
                    }
                }
            }
        }

        // 更新処理
        let dt = (best_t - current_time) as i64;

        for i in 0..ctx.cluster_members.len() {
            ctx.member_pos[i] = predict_pos(ctx.member_pos[i], cluster_v, l, dt);
        }
        current_time = best_t;

        let target_vel = initial_vel[best_target_node];
        let target_pos_at_t =
            predict_pos(initial_pos[best_target_node], target_vel, l, best_t as i64);

        ctx.operations.push(Operation {
            t: best_t,
            u: best_source_node,
            v: best_target_node,
        });
        total_cost += best_real_cost;

        ctx.is_in_cluster[best_target_node] = true;
        ctx.cluster_members.push(best_target_node);
        ctx.member_pos.push(target_pos_at_t);

        let old_size = (ctx.cluster_members.len() - 1) as f64;
        let new_size = ctx.cluster_members.len() as f64;
        let new_vx = (old_size * cluster_v.vx + target_vel.vx) / new_size;
        let new_vy = (old_size * cluster_v.vy + target_vel.vy) / new_size;
        cluster_v = Velocity {
            vx: new_vx,
            vy: new_vy,
        };
    }

    // クリーンアップ: is_in_cluster を false に戻す
    for &idx in &ctx.cluster_members {
        ctx.is_in_cluster[idx] = false;
    }

    ClusterResult {
        cost: total_cost,
        operations: ctx.operations.clone(),
        member_indices: ctx.cluster_members.clone(),
    }
}

// --- 初期解生成 ---

fn solve_initial_clusters(
    n: usize,
    m: usize,
    k: usize,
    l: f64,
    initial_pos: &Vec<Point>,
    initial_vel: &Vec<Velocity>,
    rng: &mut XorShift,
    weight_params: (f64, f64),
    ctx: &mut ClusterGenContext, // Contextを受け取る
) -> Vec<ClusterResult> {
    let mut used = vec![false; n];
    let mut clusters = Vec::with_capacity(m);

    // used 配列の管理はここで行う
    // ctx.is_in_cluster は generate_single_cluster 内でのみ一時的に使われる

    for _ in 0..m {
        // candidates のアロケーションは許容するか、あるいはこれもバッファリング可能だが
        // フィルタリングコストの方が支配的。今回はそのまま。
        let candidates: Vec<usize> = (0..n).filter(|&i| !used[i]).collect();

        let result = generate_single_cluster(
            k,
            l,
            initial_pos,
            initial_vel,
            &candidates,
            rng,
            weight_params,
            ctx,
        );

        for &member_idx in &result.member_indices {
            used[member_idx] = true;
        }
        clusters.push(result);
    }
    clusters
}

// --- SA 用の構造体・ユーティリティ ---

const INF: i64 = 1_i64 << 60;

// バッファ再利用可能な UnionFind
struct UnionFind {
    parent: Vec<usize>,
    size: Vec<usize>,
}

impl UnionFind {
    fn new(n: usize) -> Self {
        let mut parent = Vec::with_capacity(n);
        let mut size = Vec::with_capacity(n);
        for i in 0..n {
            parent.push(i);
            size.push(1);
        }
        Self { parent, size }
    }

    // 状態のリセット（再確保なし）
    fn reset(&mut self) {
        let n = self.parent.len();
        for i in 0..n {
            self.parent[i] = i;
            self.size[i] = 1;
        }
    }

    fn find(&mut self, x: usize) -> usize {
        if self.parent[x] == x {
            x
        } else {
            let p = self.find(self.parent[x]);
            self.parent[x] = p;
            p
        }
    }

    fn union(&mut self, x: usize, y: usize) -> usize {
        let mut rx = self.find(x);
        let mut ry = self.find(y);
        if rx == ry {
            return rx;
        }
        if self.size[rx] < self.size[ry] {
            std::mem::swap(&mut rx, &mut ry);
        }
        self.parent[ry] = rx;
        self.size[rx] += self.size[ry];
        rx
    }
}

// SA評価用の再利用バッファ
struct EvalBuffer {
    pos: Vec<Point>,
    vel: Vec<Velocity>,
    uf: UnionFind,
    // Vec<Vec<usize>> の代わり。要素数Kの配列で、各要素がVec<usize>
    comp_members: Vec<Vec<usize>>,
    order: Vec<usize>,
}

impl EvalBuffer {
    fn new(k: usize) -> Self {
        // comp_members の各 Vec もキャパシティ確保しておく
        let mut comp_members = Vec::with_capacity(k);
        for _ in 0..k {
            comp_members.push(Vec::with_capacity(k));
        }

        Self {
            pos: Vec::with_capacity(k),
            vel: Vec::with_capacity(k),
            uf: UnionFind::new(k),
            comp_members,
            order: Vec::with_capacity(k),
        }
    }
}

#[derive(Clone, Copy, Debug)]
struct Merge {
    t: usize,
    a: usize,
    b: usize,
}

#[derive(Clone, Debug)]
struct ClusterSAState {
    members: Vec<usize>,
    merges: Vec<Merge>,
    cost: i64,
}

fn is_valid_tree(k: usize, merges: &[Merge], uf: &mut UnionFind) -> bool {
    if k == 0 {
        return true;
    }
    if merges.len() != k - 1 {
        return false;
    }
    // バッファ内のUFをリセットして使用
    uf.reset();

    for m in merges {
        if m.a >= k || m.b >= k || m.a == m.b {
            return false;
        }
        if uf.find(m.a) == uf.find(m.b) {
            return false;
        }
        uf.union(m.a, m.b);
    }
    let root0 = uf.find(0);
    for v in 1..k {
        if uf.find(v) != root0 {
            return false;
        }
    }
    true
}

// 高速化された eval_cluster
fn eval_cluster(
    l: f64,
    t_max: usize,
    member_globals: &[usize],
    merges: &[Merge],
    points: &[Point],
    vels: &[Velocity],
    buf: &mut EvalBuffer, // バッファを受け取る
) -> i64 {
    let k = member_globals.len();
    if k == 0 {
        return 0;
    }
    if merges.len() != k - 1 {
        return INF;
    }
    if t_max == 0 {
        return INF;
    }

    // バッファのセットアップ
    buf.pos.clear();
    buf.vel.clear();
    for &gi in member_globals {
        buf.pos.push(points[gi]);
        buf.vel.push(vels[gi]);
    }

    // UFリセット
    buf.uf.reset();

    // コンポーネントメンバリセット
    // ここでは初期状態で各ノードiは自分自身のみを含む
    for i in 0..k {
        buf.comp_members[i].clear();
        buf.comp_members[i].push(i);
    }

    // Merge順序のソート
    buf.order.clear();
    buf.order.extend(0..merges.len());
    // Merge数が少ないので sort_by_key は高速だが、アロケーション回避のため既存バッファ上で
    buf.order.sort_unstable_by_key(|&idx| merges[idx].t);

    let mut total_cost: i64 = 0;
    let mut current_time: usize = 0;

    for &mid in &buf.order {
        let m = &merges[mid];
        if m.t >= t_max {
            return INF;
        }

        let dt = m.t.saturating_sub(current_time);
        if dt > 0 {
            let dti = dt as i64;
            // pos の更新
            for i in 0..k {
                buf.pos[i] = predict_pos(buf.pos[i], buf.vel[i], l, dti);
            }
            current_time = m.t;
        }

        let ra = buf.uf.find(m.a);
        let rb = buf.uf.find(m.b);
        if ra == rb {
            return INF;
        }

        // メンバリストへのアクセス (borrow checker 対策でインデックスアクセス)
        // comp_members は [0..k] のスロットがある。ルートIDにあるものが有効。
        let members_a = &buf.comp_members[ra];
        let members_b = &buf.comp_members[rb];

        if members_a.is_empty() || members_b.is_empty() {
            return INF;
        }

        // 成分間で最も近い点対のコスト
        let mut best = INF;
        // ネストループ: kが小さいので許容範囲
        for &ia in members_a {
            let pa = buf.pos[ia];
            for &ib in members_b {
                let pb = buf.pos[ib];
                // ここも sqrt 遅延可能だが、回数が少ないのでそのまま
                let cost = calc_bond_cost(pa, pb, l);
                if cost < best {
                    best = cost;
                }
            }
        }
        if best >= INF {
            return INF;
        }
        total_cost += best;

        // 成分の速度更新
        let size_a = members_a.len() as f64;
        let size_b = members_b.len() as f64;
        let va = buf.vel[members_a[0]]; // 同じ成分内の速度は同一のはず
        let vb = buf.vel[members_b[0]];
        let new_vx = (size_a * va.vx + size_b * vb.vx) / (size_a + size_b);
        let new_vy = (size_a * va.vy + size_b * vb.vy) / (size_a + size_b);
        let new_v = Velocity {
            vx: new_vx,
            vy: new_vy,
        };

        // UF統合
        let new_root = buf.uf.union(ra, rb);

        // メンバリストのマージ:
        // rust の借用規則のため、一時的に取り出して結合し、戻す
        // ra, rb のどちらかが new_root。
        // もう片方を new_root に move する。

        // メモリ移動を最小限にするため、小さい方を大きい方にマージしたいが
        // UFの実装に合わせる必要がある。UFは size でマージ方向を決めている。
        // ここでは愚直に: 両方取り出して new_root に入れる (swap remove 的なことはできないので clone せざるを得ないが回避したい)

        // workaround: インデックスアクセスで処理
        // buf.comp_members[ra] と buf.comp_members[rb] を結合して buf.comp_members[new_root] にする
        if new_root == ra {
            // rb の中身を ra に移す
            // cannot borrow `buf.comp_members` as mutable more than once
            // split_at_mut などが必要だが、添字が動的。
            // 仕方ないので一時的な Vec を使わず、mem::take を使う
            let mut taken_b = std::mem::take(&mut buf.comp_members[rb]);
            buf.comp_members[ra].append(&mut taken_b);
        } else {
            // ra の中身を rb に移す
            let mut taken_a = std::mem::take(&mut buf.comp_members[ra]);
            buf.comp_members[rb].append(&mut taken_a);
        }

        // 速度適用
        for &idx in &buf.comp_members[new_root] {
            buf.vel[idx] = new_v;
        }
    }

    total_cost
}

// 出力用
fn reconstruct_operations_for_cluster(
    l: f64,
    t_max: usize,
    member_globals: &[usize],
    merges: &[Merge],
    points: &[Point],
    vels: &[Velocity],
) -> Vec<Operation> {
    let k = member_globals.len();
    if k == 0 {
        return Vec::new();
    }
    if t_max == 0 {
        return Vec::new();
    }

    let mut pos = Vec::with_capacity(k);
    let mut vel = Vec::with_capacity(k);
    for &gi in member_globals {
        pos.push(points[gi]);
        vel.push(vels[gi]);
    }

    let mut order: Vec<usize> = (0..merges.len()).collect();
    order.sort_unstable_by_key(|&idx| merges[idx].t);

    let mut uf = UnionFind::new(k);
    let mut comp_members: Vec<Vec<usize>> = (0..k).map(|i| vec![i]).collect();

    let mut current_time: usize = 0;
    let mut ops: Vec<Operation> = Vec::with_capacity(merges.len());

    for &mid in &order {
        let m = &merges[mid];
        if m.t >= t_max {
            continue;
        }

        let dt = m.t.saturating_sub(current_time);
        if dt > 0 {
            let dti = dt as i64;
            for i in 0..k {
                pos[i] = predict_pos(pos[i], vel[i], l, dti);
            }
            current_time = m.t;
        }

        let ra = uf.find(m.a);
        let rb = uf.find(m.b);
        if ra == rb {
            continue;
        }

        let members_a = &comp_members[ra];
        let members_b = &comp_members[rb];
        if members_a.is_empty() || members_b.is_empty() {
            continue;
        }

        let mut best = INF;
        let mut best_pair = (members_a[0], members_b[0]);
        for &ia in members_a {
            for &ib in members_b {
                let cost = calc_bond_cost(pos[ia], pos[ib], l);
                if cost < best {
                    best = cost;
                    best_pair = (ia, ib);
                }
            }
        }

        let size_a = members_a.len() as f64;
        let size_b = members_b.len() as f64;
        let va = vel[members_a[0]];
        let vb = vel[members_b[0]];
        let new_vx = (size_a * va.vx + size_b * vb.vx) / (size_a + size_b);
        let new_vy = (size_a * va.vy + size_b * vb.vy) / (size_a + size_b);
        let new_v = Velocity {
            vx: new_vx,
            vy: new_vy,
        };

        let new_root = uf.union(ra, rb);

        // Reconstructは一度きりなのでclone許容
        let mut merged = Vec::with_capacity(members_a.len() + members_b.len());
        merged.extend_from_slice(members_a);
        merged.extend_from_slice(members_b);

        // clear old to mimic movement
        comp_members[ra].clear();
        comp_members[rb].clear();
        comp_members[new_root] = merged.clone();

        for idx in merged {
            vel[idx] = new_v;
        }

        let g_u = member_globals[best_pair.0];
        let g_v = member_globals[best_pair.1];

        ops.push(Operation {
            t: m.t,
            u: g_u,
            v: g_v,
        });
    }

    ops
}

fn main() {
    let start_time = Instant::now();

    input! {
        n: usize,
        t_max: usize,
        m: usize,
        k: usize,
        l_int: i64,
        xyv: [(i64, i64, i64, i64); n],
    }
    let l = l_int as f64;

    let mut points = Vec::with_capacity(n);
    let mut vels = Vec::with_capacity(n);
    for (x, y, vx, vy) in xyv {
        points.push(Point {
            x: x as f64,
            y: y as f64,
        });
        vels.push(Velocity {
            vx: vx as f64,
            vy: vy as f64,
        });
    }

    let mut rng = XorShift::new(88172645463393265);

    // --- Phase 1: 初期解の選定 ---
    let mut best_clusters_cache: Option<Vec<ClusterResult>> = None;
    let mut best_cost_cache = i64::MAX;
    let mut init_iter = 0;

    // バッファ確保
    let mut gen_ctx = ClusterGenContext::new(n, k);

    while best_clusters_cache.is_none() || start_time.elapsed().as_secs_f64() < 1.0 {
        let w = 0.45 + rng.next_f64() * 0.1;
        let p = 1.9 + rng.next_f64() * 0.2;

        let clusters =
            solve_initial_clusters(n, m, k, l, &points, &vels, &mut rng, (w, p), &mut gen_ctx);
        let cost: i64 = clusters.iter().map(|c| c.cost).sum();

        if cost < best_cost_cache {
            best_cost_cache = cost;
            best_clusters_cache = Some(clusters);
            eprintln!("Init Best: {} (Iter: {})", best_cost_cache, init_iter);
        }
        init_iter += 1;
    }

    let initial_clusters = best_clusters_cache.unwrap();

    // --- Phase 2: SA 用の状態初期化 ---
    let mut sa_clusters: Vec<ClusterSAState> = Vec::with_capacity(m);

    // 評価用バッファ
    let mut eval_buf = EvalBuffer::new(k);

    for c in initial_clusters.iter() {
        let members = c.member_indices.clone();
        let k_local = members.len();
        let mut global_to_local = vec![usize::MAX; n];
        for (li, &gi) in members.iter().enumerate() {
            global_to_local[gi] = li;
        }

        let merges: Vec<Merge> = c
            .operations
            .iter()
            .map(|op| Merge {
                t: op.t,
                a: global_to_local[op.u],
                b: global_to_local[op.v],
            })
            .collect();

        // バッファを使って評価
        let cost = eval_cluster(l, t_max, &members, &merges, &points, &vels, &mut eval_buf);
        sa_clusters.push(ClusterSAState {
            members,
            merges,
            cost,
        });
    }

    let mut global_cost: i64 = sa_clusters.iter().map(|c| c.cost).sum();
    let mut best_clusters = sa_clusters.clone();
    let mut best_cost = global_cost;

    eprintln!("Initial SA cost: {}", global_cost);

    // --- Phase 2: 焼きなまし ---

    let after_init_time = start_time.elapsed().as_secs_f64();
    let total_time_limit = 1.95_f64;
    let remaining = total_time_limit - after_init_time;

    if remaining > 0.0 {
        let start_temp: f64 = 1000.0;
        let end_temp: f64 = 1.0;

        let mut iterations: i64 = 0;

        while start_time.elapsed().as_secs_f64() < total_time_limit {
            iterations += 1;

            let now = start_time.elapsed().as_secs_f64();
            let progress = ((now - after_init_time) / remaining).max(0.0).min(1.0);
            let temp = start_temp * (end_temp / start_temp).powf(progress);

            let cid = rng.gen_range(sa_clusters.len());
            let cluster = &mut sa_clusters[cid];

            if cluster.members.len() <= 1 || cluster.merges.is_empty() {
                continue;
            }

            let mut move_kind: u8 = 0;
            let mut idx1: usize = 0;
            let mut idx2: usize = 0;
            let mut old_t1: usize = 0;
            let mut old_t2: usize = 0;
            let mut old_merge: Merge = Merge { t: 0, a: 0, b: 0 };

            let r = rng.next_f64();
            if r < 0.5 {
                // 時刻微調整
                move_kind = 0;
                idx1 = rng.gen_range(cluster.merges.len());
                old_t1 = cluster.merges[idx1].t;

                if t_max <= 1 {
                    continue;
                }

                let max_delta: i32 = 50;
                let delta_span = (max_delta * 2 + 1) as usize;
                let delta = rng.gen_range(delta_span) as i32 - max_delta;
                let mut new_t = old_t1 as i32 + delta;
                if new_t < 0 {
                    new_t = 0;
                }
                if new_t >= t_max as i32 {
                    new_t = t_max as i32 - 1;
                }
                cluster.merges[idx1].t = new_t as usize;
            } else if r < 0.7 {
                // 時刻スワップ
                if cluster.merges.len() < 2 {
                    continue;
                }
                move_kind = 1;
                idx1 = rng.gen_range(cluster.merges.len());
                idx2 = rng.gen_range(cluster.merges.len());
                while idx2 == idx1 {
                    idx2 = rng.gen_range(cluster.merges.len());
                }
                old_t1 = cluster.merges[idx1].t;
                old_t2 = cluster.merges[idx2].t;
                if idx1 != idx2 {
                    let (first, second) = if idx1 < idx2 {
                        let (left, right) = cluster.merges.split_at_mut(idx2);
                        (&mut left[idx1], &mut right[0])
                    } else {
                        let (left, right) = cluster.merges.split_at_mut(idx1);
                        (&mut right[0], &mut left[idx2])
                    };
                    std::mem::swap(&mut first.t, &mut second.t);
                }
            } else {
                // 辺の片側端点変更
                move_kind = 2;
                let k_local = cluster.members.len();
                if k_local <= 1 {
                    continue;
                }

                idx1 = rng.gen_range(cluster.merges.len());
                old_merge = cluster.merges[idx1];

                let change_a = rng.next_f64() < 0.5;
                if change_a {
                    let mut new_a = old_merge.a;
                    for _ in 0..10 {
                        new_a = rng.gen_range(k_local);
                        if new_a != old_merge.b {
                            break;
                        }
                    }
                    cluster.merges[idx1].a = new_a;
                } else {
                    let mut new_b = old_merge.b;
                    for _ in 0..10 {
                        new_b = rng.gen_range(k_local);
                        if new_b != old_merge.a {
                            break;
                        }
                    }
                    cluster.merges[idx1].b = new_b;
                }

                let ma = cluster.merges[idx1].a;
                let mb = cluster.merges[idx1].b;

                if ma == mb {
                    cluster.merges[idx1] = old_merge;
                    continue;
                }

                let mut dup = false;
                for (ei, m) in cluster.merges.iter().enumerate() {
                    if ei == idx1 {
                        continue;
                    }
                    if (m.a == ma && m.b == mb) || (m.a == mb && m.b == ma) {
                        dup = true;
                        break;
                    }
                }
                // is_valid_tree もバッファ版UFを使いたいが、is_valid_treeは一瞬で終わるので
                // バッファを渡すために構造を変えるよりは、eval_bufのUFを借用して使う
                if dup || !is_valid_tree(k_local, &cluster.merges, &mut eval_buf.uf) {
                    cluster.merges[idx1] = old_merge;
                    continue;
                }
            }

            // 変更後のコストを計算 (バッファ利用)
            let new_cost = eval_cluster(
                l,
                t_max,
                &cluster.members,
                &cluster.merges,
                &points,
                &vels,
                &mut eval_buf,
            );

            if new_cost >= INF {
                // revert
                match move_kind {
                    0 => {
                        cluster.merges[idx1].t = old_t1;
                    }
                    1 => {
                        cluster.merges[idx1].t = old_t1;
                        cluster.merges[idx2].t = old_t2;
                    }
                    2 => {
                        cluster.merges[idx1] = old_merge;
                    }
                    _ => {}
                }
                continue;
            }

            let old_cluster_cost = cluster.cost;
            let delta = new_cost - old_cluster_cost;
            let accept = if delta <= 0 {
                true
            } else {
                let prob = (-(delta as f64) / temp).exp();
                rng.next_f64() < prob
            };

            if accept {
                cluster.cost = new_cost;
                global_cost += delta;

                if global_cost < best_cost {
                    best_cost = global_cost;
                    best_clusters = sa_clusters.clone();
                }
            } else {
                // revert
                cluster.cost = old_cluster_cost; // 念のため戻す(costはrevert不要だが一貫性のため)
                match move_kind {
                    0 => {
                        cluster.merges[idx1].t = old_t1;
                    }
                    1 => {
                        cluster.merges[idx1].t = old_t1;
                        cluster.merges[idx2].t = old_t2;
                    }
                    2 => {
                        cluster.merges[idx1] = old_merge;
                    }
                    _ => {}
                }
            }
        }
        eprintln!(
            "SA done. iterations = {}, best_Score = {}",
            iterations, best_cost
        );
    } else {
        eprintln!("No time left for SA");
        best_clusters = sa_clusters;
        best_cost = global_cost;
    }

    // --- 最終出力 ---
    let mut all_ops: Vec<Operation> = Vec::new();
    for c in best_clusters.iter() {
        let ops =
            reconstruct_operations_for_cluster(l, t_max, &c.members, &c.merges, &points, &vels);
        all_ops.extend(ops);
    }

    all_ops.sort_unstable_by_key(|op| op.t);

    for op in all_ops {
        println!("{} {} {}", op.t, op.u, op.v);
    }
}
