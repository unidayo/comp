#include <bits/stdc++.h>
using namespace std;

// =============================================================
// A - Ice Cream Collection
// Beam search with "lazy action2 insertion" + time-dependent weights.
//
// Lazy action2 insertion (the user's interpretation):
// - The first time you arrive at a tree, you must harvest W.
// - If you arrive at the same tree again while it's still W, you may
//   interpret that you had done action2 (-1) right after the previous
//   visit, so that now you harvest R.
// - This increases the total number of actions by +1, but does NOT
//   change the previously harvested ice.
//
// We implement this by:
// - When a state chooses "conversion" on arrival to tree v, we just
//   mark v as R and add +1 to steps_used.
// - In the final output, we emit "-1" right after the previous move
//   that arrived at v.
//
// Input constraints (from statement.md):
//   N=100, K=10, T=10000
// =============================================================

// -------------------- Beam parameters --------------------
static constexpr int BEAM_WIDTH = 14 * 50;    // keep top states per depth
static constexpr int SEG_MOVES = 400;     // plan this many action1 per segment
static constexpr int MAX_SEGMENTS = 20000;  // safety cap

// -------------------- Time-dependent weights --------------------
// ここだけいじれば「ターンごとのペナルティ調節」ができるようにしてある。
struct WeightSchedule {
    // main objective
    double w_score = 331.533847;

    // early -> late (mixed by phase)
    double w_cone_len_early = 2.911387;
    double w_cone_len_late = 3.552239;

    double w_visited_early = 8.212242;
    double w_visited_late = 2.164051;

    // "flip-to-R" penalty should vanish late
    // 前半: 強く抑制 / 後半: ほぼ無視 にしたいなら late を 0 に寄せる
    double w_r_cnt_early = -11.753740;
    double w_r_cnt_late = -2.771546;

    // prefer fewer used steps; relax late
    double w_steps_early = -1.085573;
    double w_steps_late = -0.476367;

    // extra: be close to a shop late (delivery/revisit phase)
    double w_near_shop_early = 0.045907;
    double w_near_shop_late = 0.023645;

    // extra: if cone is long, being near a shop is even better (late)
    double w_near_shop_x_cone_early = 0.009956;
    double w_near_shop_x_cone_late = 0.017383;

    // extra: weakly balance shops (increase min |S_i|)
    double w_min_shop = 0.910180;

    // phase mixing window
    // phase = steps_used / T. t=0 (early) -> t=1 (late)
    // 例: center=0.72, halfwidth=0.15 なら 0.57〜0.87
    // あたりで滑らかに切り替える。
    double phase_center = 0.887931;
    double phase_halfwidth = 0.136045;

    // expansion control for conversion branch
    // Always allow conversion when remaining steps is small, or in late phase.
    int conv_rem_on = 14;        // 残り手数 <= これ なら常に色変え分岐OK
    double conv_phase_on = 0.597320;  // phase >= これ なら常に色変え分岐OK

    // In early phase, allow conversion only when it is "locally plausible" to
    // pay off.
    int conv_early_dist = 3;  // 近い店がある（距離<=1）なら前半でも出す
    int conv_early_cone = 6;  // かつ cone がそこそこ長い時だけ（分岐爆発抑制）
};
static const WeightSchedule S;

static int N, M, K, Tlim;
static vector<vector<int>> g;
static vector<int> dist_nearest_shop;

static inline double clamp01(double x) { return x < 0 ? 0 : (x > 1 ? 1 : x); }
static inline double smoothstep(double t) {
    t = clamp01(t);
    return t * t * (3.0 - 2.0 * t);
}
static inline double lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

static inline double phase_t(int steps_used) {
    double phase = (double)steps_used / (double)Tlim;  // 0..1
    double a = S.phase_center - S.phase_halfwidth;
    double b = S.phase_center + S.phase_halfwidth;
    double u = (phase - a) / (b - a);
    return smoothstep(u);
}

// -------------------- Hashing for cone strings --------------------
static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
static inline uint64_t rotl64(uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static uint64_t BASE1, BASE2;

// -------------------- Persistent immutable BST set --------------------
// Store 64-bit keys; membership & insertion O(height).
// Keys are mixed so expected height ~log.
struct PNode {
    uint64_t key;
    PNode *l, *r;
    int sz;
};
static deque<PNode> PNODES;
static inline int psz(PNode* t) { return t ? t->sz : 0; }
static inline PNode* pnew(uint64_t key,
                          PNode* l = nullptr,
                          PNode* r = nullptr) {
    PNODES.push_back(PNode{key, l, r, 1 + psz(l) + psz(r)});
    return &PNODES.back();
}
static inline PNode* pcl(PNode* t) {
    PNODES.push_back(*t);
    return &PNODES.back();
}
static bool pcontains(PNode* t, uint64_t key) {
    while (t) {
        if (key == t->key) return true;
        t = (key < t->key) ? t->l : t->r;
    }
    return false;
}
static PNode* pinsert_new(PNode* t, uint64_t key) {
    if (!t) return pnew(key);
    PNode* nt = pcl(t);
    if (key < t->key)
        nt->l = pinsert_new(t->l, key);
    else
        nt->r = pinsert_new(t->r, key);
    nt->sz = 1 + psz(nt->l) + psz(nt->r);
    return nt;
}

// -------------------- Bitset helpers (N=100) --------------------
static inline bool getbit(uint64_t lo, uint64_t hi, int v) {
    if (v < 64) return (lo >> v) & 1ULL;
    return (hi >> (v - 64)) & 1ULL;
}
static inline void setbit(uint64_t& lo, uint64_t& hi, int v) {
    if (v < 64)
        lo |= 1ULL << v;
    else
        hi |= 1ULL << (v - 64);
}
static inline int popcnt2(uint64_t lo, uint64_t hi) {
    return __builtin_popcountll(lo) + __builtin_popcountll(hi);
}

// -------------------- State --------------------
struct State {
    int pos = 0;
    int prev_from = -1;  // previous action1's origin (cannot return to it)

    int steps_used = 0;  // total actions = moves + conversions
    int moves_used = 0;  // number of action1
    int score_sum = 0;   // Σ |S_i|

    // cone representation
    uint64_t h1 = 0, h2 = 0;
    int cone_len = 0;

    // visited vertices
    uint64_t vis_lo = 0, vis_hi = 0;
    int visited_cnt = 0;

    // flipped-to-R trees
    uint64_t r_lo = 0, r_hi = 0;
    int r_cnt = 0;

    // last arrival move index for each vertex (-1 if never arrived via action1)
    array<int16_t, 100> last_visit;

    // per shop inventory set (persistent)
    array<PNode*, 10> shop_root;
    array<int, 10> shop_sz;

    // trace index for segment reconstruction
    int trace_idx = -1;

    float eval = 0.0f;
};

struct TraceNode {
    int prev;
    int to;
    uint8_t conv;  // 1 if we used lazy conversion on this arrival
};

// -------------------- Helpers --------------------
static inline bool is_tree(int v) { return v >= K; }
static inline bool is_shop(int v) { return v < K; }

static inline bool is_visited(const State& st, int v) {
    return getbit(st.vis_lo, st.vis_hi, v);
}
static inline void set_visited(State& st, int v) {
    if (!is_visited(st, v)) {
        setbit(st.vis_lo, st.vis_hi, v);
        st.visited_cnt++;
    }
}
static inline bool is_r(const State& st, int v) {
    return getbit(st.r_lo, st.r_hi, v);
}
static inline void set_r(State& st, int v) {
    if (!is_r(st, v)) {
        setbit(st.r_lo, st.r_hi, v);
        st.r_cnt++;
    }
}

static inline uint64_t cone_key(uint64_t h1, uint64_t h2, int len) {
    uint64_t x = h1 ^ rotl64(h2, 32) ^ (uint64_t)len * 0x9e3779b97f4a7c15ULL;
    return splitmix64(x);
}
static inline void cone_push(State& st, char c) {
    uint64_t v = (c == 'W') ? 1ULL : 2ULL;
    st.h1 = st.h1 * BASE1 + v;
    st.h2 = st.h2 * BASE2 + v;
    st.cone_len++;
}

static float evaluate_state(const State& st) {
    const int rem = Tlim - st.steps_used;
    const int d = dist_nearest_shop[st.pos];

    // int mn_shop = INT_MAX;
    // for (int i = 0; i < K; i++) mn_shop = min(mn_shop, st.shop_sz[i]);

    const double t = phase_t(st.steps_used);

    const double w_cone = std::lerp(S.w_cone_len_early, S.w_cone_len_late, t);
    const double w_vis = std::lerp(S.w_visited_early, S.w_visited_late, t);
    const double w_r = std::lerp(S.w_r_cnt_early, S.w_r_cnt_late, t);
    const double w_steps = std::lerp(S.w_steps_early, S.w_steps_late, t);
    const double w_shop = std::lerp(S.w_near_shop_early, S.w_near_shop_late, t);
    const double w_shop_c =
        std::lerp(S.w_near_shop_x_cone_early, S.w_near_shop_x_cone_late, t);

    // さらに「残りが少ないなら色変えし放題」感を強める（任意）
    double w_r2 = w_r;
    //if (rem <= 80) w_r2 = max(w_r2, -0.05);

    double e = 0.0;
    e += S.w_score * (double)st.score_sum;
    e += w_cone * (double)st.cone_len;
    e += w_vis * (double)st.visited_cnt;
    e += w_r2 * (double)st.r_cnt;
    e += w_steps * (double)st.steps_used;

    // lateほど「店に近い」が嬉しい
    e += w_shop * (double)(-d);
    e += w_shop_c * (double)(-d * st.cone_len);

    // mild balancing
    //e += S.w_min_shop * (double)mn_shop;

    return (float)e;
}

// conversion branch gating (for branching control + late game preference)
static inline bool allow_conv_branch(const State& st, int to) {
    if (!is_tree(to)) return false;
    if (!is_visited(st, to)) return false;
    if (is_r(st, to)) return false;

    const int rem = Tlim - st.steps_used;
    const double phase = (double)st.steps_used / (double)Tlim;

    // late / small remaining => always allow
    if (rem <= S.conv_rem_on || phase >= S.conv_phase_on) return true;

    // early: only allow if likely to pay off soon (reduce branching)
    if (dist_nearest_shop[to] <= S.conv_early_dist &&
        st.cone_len >= S.conv_early_cone)
        return true;

    return false;
}

// Apply a move to a copied state.
// - to: destination vertex
// - use_conv: if true, we harvest R on arrival by lazy-inserting -1 after
// previous visit
static inline bool transition(State& child,
                              const State& par,
                              int to,
                              bool use_conv,
                              vector<TraceNode>& trace_pool,
                              int global_move_offset) {
    // movement restriction
    if (par.prev_from != -1 && to == par.prev_from) return false;

    const int cost = 1 + (use_conv ? 1 : 0);
    if (par.steps_used + cost > Tlim) return false;

    if (use_conv) {
        if (!allow_conv_branch(par, to)) return false;
    }

    child = par;

    if (use_conv) {
        // mark this tree as R (conceptually performed after previous visit)
        set_r(child, to);
    }

    // apply action1
    child.steps_used += cost;
    child.prev_from = par.pos;
    child.pos = to;

    // move index (0-based) in the whole output move list
    const int new_move_idx = global_move_offset;
    child.moves_used = par.moves_used + 1;
    child.last_visit[to] = (int16_t)new_move_idx;

    // visited
    set_visited(child, to);

    // effect by vertex type
    if (is_tree(to)) {
        const bool r_now = is_r(child, to);
        cone_push(child, r_now ? 'R' : 'W');
    } else {
        // deliver cone string to this shop
        const int shop = to;
        const uint64_t key = cone_key(child.h1, child.h2, child.cone_len);
        if (!pcontains(child.shop_root[shop], key)) {
            child.shop_root[shop] = pinsert_new(child.shop_root[shop], key);
            child.shop_sz[shop]++;
            child.score_sum++;
        }
        // reset cone
        child.h1 = child.h2 = 0;
        child.cone_len = 0;
    }

    // trace
    const int new_trace_idx = (int)trace_pool.size();
    trace_pool.push_back(
        TraceNode{par.trace_idx, to, (uint8_t)(use_conv ? 1 : 0)});
    child.trace_idx = new_trace_idx;

    // eval
    child.eval = evaluate_state(child);
    return true;
}

// Plan next segment from base state; returns (moves, conv_flags)
static pair<vector<int>, vector<uint8_t>> plan_segment(const State& base) {
    vector<TraceNode> trace_pool;
    trace_pool.reserve((size_t)BEAM_WIDTH * (size_t)SEG_MOVES * 6);

    vector<State> cur, nxt;
    cur.reserve(BEAM_WIDTH);
    nxt.reserve(BEAM_WIDTH * 8);

    State s0 = base;
    s0.trace_idx = -1;
    s0.eval = evaluate_state(s0);
    cur.push_back(s0);

    for (int depth = 0; depth < SEG_MOVES; depth++) {
        nxt.clear();

        for (const State& st : cur) {
            const int global_move_offset = st.moves_used;

            for (int to : g[st.pos]) {
                if (st.prev_from != -1 && to == st.prev_from) continue;

                // Always consider normal move
                {
                    State ch;
                    if (transition(ch, st, to, false, trace_pool,
                                   global_move_offset)) {
                        nxt.push_back(std::move(ch));
                    }
                }

                // Consider conversion move when allowed
                if (allow_conv_branch(st, to)) {
                    State ch;
                    if (transition(ch, st, to, true, trace_pool,
                                   global_move_offset)) {
                        nxt.push_back(std::move(ch));
                    }
                }
            }
        }

        if (nxt.empty()) break;

        // Keep top BEAM_WIDTH by eval
        if ((int)nxt.size() > BEAM_WIDTH) {
            nth_element(
                nxt.begin(), nxt.begin() + BEAM_WIDTH, nxt.end(),
                [](const State& a, const State& b) { return a.eval > b.eval; });
            nxt.resize(BEAM_WIDTH);
        }
        sort(nxt.begin(), nxt.end(), [](const State& a, const State& b) {
            if (a.eval != b.eval) return a.eval > b.eval;
            if (a.score_sum != b.score_sum) return a.score_sum > b.score_sum;
            return a.steps_used < b.steps_used;
        });

        cur.swap(nxt);
    }

    // pick best leaf: prioritize score_sum, then eval
    const State* best = &cur[0];
    for (const State& st : cur) {
        if (st.score_sum > best->score_sum)
            best = &st;
        else if (st.score_sum == best->score_sum && st.eval > best->eval)
            best = &st;
    }

    // reconstruct segment
    vector<int> moves;
    vector<uint8_t> conv;
    int idx = best->trace_idx;
    while (idx != -1) {
        moves.push_back(trace_pool[idx].to);
        conv.push_back(trace_pool[idx].conv);
        idx = trace_pool[idx].prev;
    }
    reverse(moves.begin(), moves.end());
    reverse(conv.begin(), conv.end());

    return {moves, conv};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> N >> M >> K >> Tlim;
    g.assign(N, {});
    for (int i = 0; i < M; i++) {
        int a, b;
        cin >> a >> b;
        g[a].push_back(b);
        g[b].push_back(a);
    }
    // coordinates (unused)
    for (int i = 0; i < N; i++) {
        int x, y;
        cin >> x >> y;
        (void)x;
        (void)y;
    }

    // precompute distance to nearest shop
    dist_nearest_shop.assign(N, INT_MAX);
    for (int s = 0; s < K; s++) {
        vector<int> dist(N, -1);
        queue<int> q;
        dist[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : g[v]) {
                if (dist[to] != -1) continue;
                dist[to] = dist[v] + 1;
                q.push(to);
            }
        }
        for (int v = 0; v < N; v++) {
            if (dist[v] != -1)
                dist_nearest_shop[v] = min(dist_nearest_shop[v], dist[v]);
        }
    }

    // Deterministic rolling hash bases
    BASE1 = splitmix64(123456789ULL) | 1ULL;
    BASE2 = splitmix64(987654321ULL) | 1ULL;

    // initial state
    State base;
    base.pos = 0;
    base.prev_from = -1;
    base.steps_used = 0;
    base.moves_used = 0;
    base.score_sum = 0;
    base.h1 = base.h2 = 0;
    base.cone_len = 0;

    base.vis_lo = base.vis_hi = 0;
    base.visited_cnt = 0;
    set_visited(base, 0);

    base.r_lo = base.r_hi = 0;
    base.r_cnt = 0;

    base.last_visit.fill(-1);
    base.shop_root.fill(nullptr);
    base.shop_sz.fill(0);

    vector<int> plan_moves;
    vector<uint8_t> plan_flip_after;  // 1 => output -1 after this move
    plan_moves.reserve(Tlim);
    plan_flip_after.reserve(Tlim);

    // Plan in segments and commit each segment fully.
    for (int seg = 0; seg < MAX_SEGMENTS; seg++) {
        if (base.steps_used >= Tlim) break;

        auto [seg_moves, seg_conv] = plan_segment(base);
        if (seg_moves.empty()) break;

        // Commit
        for (size_t i = 0; i < seg_moves.size(); i++) {
            int to = seg_moves[i];
            bool use_conv = (seg_conv[i] != 0);

            // If this move uses conversion, insert -1 after previous visit.
            if (use_conv) {
                int ins = base.last_visit[to];
                if (ins >= 0 && ins < (int)plan_flip_after.size()) {
                    plan_flip_after[ins] = 1;
                }
                // account conversion step and mark R
                if (base.steps_used + 1 > Tlim) break;
                base.steps_used += 1;
                set_r(base, to);
            }

            // Now do the move (action1)
            if (base.steps_used + 1 > Tlim) break;

            base.steps_used += 1;
            int from = base.pos;
            base.prev_from = from;
            base.pos = to;

            plan_moves.push_back(to);
            plan_flip_after.push_back(0);
            int move_idx = (int)plan_moves.size() - 1;

            base.moves_used++;
            base.last_visit[to] = (int16_t)move_idx;
            set_visited(base, to);

            if (is_tree(to)) {
                const bool r_now = is_r(base, to);
                cone_push(base, r_now ? 'R' : 'W');
            } else {
                const int shop = to;
                const uint64_t key = cone_key(base.h1, base.h2, base.cone_len);
                if (!pcontains(base.shop_root[shop], key)) {
                    base.shop_root[shop] =
                        pinsert_new(base.shop_root[shop], key);
                    base.shop_sz[shop]++;
                    base.score_sum++;
                }
                base.h1 = base.h2 = 0;
                base.cone_len = 0;
            }

            if (base.steps_used >= Tlim) break;
        }
    }

    // Output plan (moves + inserted -1)
    int lines = 0;
    for (int i = 0; i < (int)plan_moves.size(); i++) {
        if (lines >= Tlim) break;
        cout << plan_moves[i] << "\n";
        lines++;
        if (lines >= Tlim) break;
        if (plan_flip_after[i]) {
            cout << -1 << "\n";
            lines++;
        }
    }

    return 0;
}
