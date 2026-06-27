#include <bits/stdc++.h>
#include <chrono>
using namespace std;

// ------------------------------------------------------------
// Fast-core Chokudai/beam version for Takahashi Railway sorting problem
// R is always 10.
//
// Main changes from the vector-based baseline:
//   - Fixed-size line representation: no heap allocations inside Node.
//   - Turn moves stored in fixed arrays, not vector<Move> per node.
//   - Candidate generation keeps per-edge best + small sorted top list.
//     No full candidate sort, no repeated scan in select_rect_best().
//   - Layered beam pruning uses nth_element instead of full sort.
//   - Child state is copied only after cheap validity checks.
//   - The previous experimental ready-matching evaluation is NOT used here.
// ------------------------------------------------------------

struct Move {
    unsigned char type, i, j, k;
};

struct MoveCandidate {
    unsigned char type, i, j, k;
    long long score;
};

#ifndef TIME_LIMIT_SEC
#define TIME_LIMIT_SEC 1.90
#endif
#ifndef BEAM_WIDTH
#define BEAM_WIDTH 100
#endif
#ifndef FORCE_BRANCH
#define FORCE_BRANCH 24
#endif
#ifndef MAX_DEPTH
#define MAX_DEPTH 4000
#endif
#ifndef LINEAR_LINK_BONUS
#define LINEAR_LINK_BONUS 1200
#endif

// Macro exposure bonuses:
// When a suffix move S->T makes the globally next-needed car (10*r+next_req[r])
// become the front of a siding, keep that transition even if destination S[r]
// is not clean yet. This directly models moves like "cut at 0/10/20/...",
// then after 0 is fixed, cut at 1, and so on.
#ifndef EXPOSE_NEXT_BASE_BONUS
#define EXPOSE_NEXT_BASE_BONUS 185000LL
#endif
#ifndef EXPOSE_NEXT_CLEAN_EXTRA
#define EXPOSE_NEXT_CLEAN_EXTRA 90000LL
#endif
#ifndef EXPOSE_NEXT_BLOCK_BONUS
#define EXPOSE_NEXT_BLOCK_BONUS 36000LL
#endif
#ifndef EXPOSE_NEXT_SRC_LINE_BONUS
#define EXPOSE_NEXT_SRC_LINE_BONUS 8000LL
#endif
#ifndef STRICT_BLOCK_MOVE
#define STRICT_BLOCK_MOVE 0
#endif

#ifndef COARSE_HASH_ENABLE
#define COARSE_HASH_ENABLE 0
#endif
#ifndef COARSE_EVAL_SLACK
#define COARSE_EVAL_SLACK 120000LL
#endif
#ifndef COARSE_DEPTH_BUCKET
#define COARSE_DEPTH_BUCKET 2
#endif

#ifndef TOP_CANDIDATES
#define TOP_CANDIDATES 96
#endif
#ifndef CHOKUDAI_POP_PER_DEPTH
#define CHOKUDAI_POP_PER_DEPTH 5
#endif
#ifndef CHOKUDAI_QUEUE_SOFT_LIMIT
#define CHOKUDAI_QUEUE_SOFT_LIMIT 14000
#endif
#ifndef CHOKUDAI_BRANCH_LIMIT
#define CHOKUDAI_BRANCH_LIMIT 28
#endif
#ifndef GREEDY_WARMUP_SEC
#define GREEDY_WARMUP_SEC 0.07
#endif

#ifndef SEARCH_GUARD_SEC
#define SEARCH_GUARD_SEC 0.012
#endif
#ifndef COMPACT_GUARD_SEC
#define COMPACT_GUARD_SEC 0.004
#endif
#ifndef CHOKUDAI_POOL_LIMIT
#define CHOKUDAI_POOL_LIMIT 850000
#endif
#ifndef RESTART_MIN_SEC
#define RESTART_MIN_SEC 0.055
#endif

// Simple, low-risk knobs added in v8:
// Search both the classic evacuated start and the real root start.
#ifndef CLASSIC_BEAM_RATIO
#define CLASSIC_BEAM_RATIO 0.20
#endif
#ifndef ROOT_BEAM_RATIO
#define ROOT_BEAM_RATIO 0.24
#endif
#ifndef CLASSIC_CHOKUDAI_RATIO
#define CLASSIC_CHOKUDAI_RATIO 0.20
#endif
#ifndef READY_DP_EVAL_ENABLE
#define READY_DP_EVAL_ENABLE 1
#endif
#ifndef READY_DP_PER_CAR
#define READY_DP_PER_CAR 380000LL
#endif
#ifndef READY_DP_PER_SQ
#define READY_DP_PER_SQ 36000LL
#endif

static constexpr int R = 10;
static constexpr int S_CAP = 15;
static constexpr int T_CAP = 20;
static constexpr long long NEG_INF = -(1LL << 60);

struct Timer {
    chrono::steady_clock::time_point st;
    Timer() : st(chrono::steady_clock::now()) {}
    inline double elapsed() const {
        return chrono::duration<double>(chrono::steady_clock::now() - st).count();
    }
};

static inline bool consecutive_cars(int a, int b) {
    return a / 10 == b / 10 && b == a + 1;
}

static inline uint64_t splitmix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

struct Line {
    unsigned char n = 0;
    unsigned char a[20]{};
};

struct TurnMoves {
    unsigned char cnt = 0;
    Move m[R]{};

    inline void clear() { cnt = 0; }
    inline void push(Move x) { m[cnt++] = x; }
    inline bool empty() const { return cnt == 0; }
};

struct Node {
    Line S[R], T[R];
    int parent = -1;
    unsigned short turn = 0;
    uint64_t h = 0;
    long long eval = 0;

    unsigned char fixed[R]{};
    unsigned char next_req[R]{};
    unsigned char fixed_sum = 0;

    unsigned char last_cnt = 0;
    Move last[R]{};
};

static inline bool better_cand(const MoveCandidate& a, const MoveCandidate& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.type != b.type) return a.type > b.type;
    if (a.i != b.i) return a.i < b.i;
    if (a.j != b.j) return a.j < b.j;
    return a.k > b.k;
}

struct CandidateSet {
    MoveCandidate best[R][R];
    bool has[R][R];
    vector<MoveCandidate> top;

    CandidateSet() {
        memset(has, 0, sizeof(has));
        for (int i = 0; i < R; ++i) for (int j = 0; j < R; ++j) {
            best[i][j] = MoveCandidate{0, (unsigned char)i, (unsigned char)j, 0, NEG_INF};
        }
        top.reserve(TOP_CANDIDATES);
    }

    inline void add(MoveCandidate c) {
        int i = c.i, j = c.j;
        if (!has[i][j] || better_cand(c, best[i][j])) {
            has[i][j] = true;
            best[i][j] = c;
        }

        if ((int)top.size() < TOP_CANDIDATES) {
            top.push_back(c);
            int p = (int)top.size() - 1;
            while (p > 0 && better_cand(top[p], top[p - 1])) {
                swap(top[p], top[p - 1]);
                --p;
            }
        } else if (better_cand(c, top.back())) {
            top.back() = c;
            int p = (int)top.size() - 1;
            while (p > 0 && better_cand(top[p], top[p - 1])) {
                swap(top[p], top[p - 1]);
                --p;
            }
        }
    }

    inline bool empty() const { return top.empty(); }
};

static inline void calc_fixed_next(Node& nd) {
    int fs = 0;
    for (int i = 0; i < R; ++i) {
        int c = 0;
        int lim = min<int>(10, nd.S[i].n);
        while (c < lim && (int)nd.S[i].a[c] == 10 * i + c) ++c;
        nd.fixed[i] = (unsigned char)c;
        nd.next_req[i] = (unsigned char)c;
        fs += c;
    }
    nd.fixed_sum = (unsigned char)fs;
}

static inline bool is_done(const Node& nd) {
    for (int i = 0; i < R; ++i) {
        if (nd.S[i].n != 10) return false;
        for (int c = 0; c < 10; ++c) {
            if ((int)nd.S[i].a[c] != 10 * i + c) return false;
        }
    }
    return true;
}

static inline int evaluate_order(int bottom, int top, const unsigned char next_req[R]) {
    if (bottom / 10 == top / 10) {
        return (top % 10 < bottom % 10) ? 600 : -600;
    } else {
        int r_bot = bottom % 10 - (int)next_req[bottom / 10];
        int r_top = top % 10 - (int)next_req[top / 10];
        if (r_top < r_bot) return 200;
        if (r_top > r_bot) return -200;
        return 0;
    }
}

static uint64_t state_hash(const Node& nd) {
    uint64_t h = 0x123456789abcdef0ULL;
    for (int side = 0; side < 2; ++side) {
        const Line* A = (side == 0 ? nd.S : nd.T);
        for (int i = 0; i < R; ++i) {
            h ^= splitmix64((uint64_t)(side * 1000 + i * 97 + A[i].n) + (h << 1));
            for (int p = 0; p < (int)A[i].n; ++p) {
                uint64_t x = (uint64_t)(A[i].a[p] + 1) + 131ULL * p + 10007ULL * i + 1000003ULL * side;
                h ^= splitmix64(x + (h << 1));
            }
        }
    }
    return h;
}

static inline int rel_code_for_coarse(int v, const unsigned char next_req[R]) {
    int r = v / 10;
    int p = v % 10;
    int d = p - (int)next_req[r];
    if (d < -2) d = -2;
    if (d > 6) d = 6;
    return r * 16 + (d + 2);
}

static uint64_t coarse_hash(const Node& nd, int level) {
    uint64_t h = 0x6a09e667f3bcc909ULL ^ splitmix64((uint64_t)nd.turn);
    h ^= splitmix64(1000003ULL + (uint64_t)nd.fixed_sum * 257ULL);

    for (int i = 0; i < R; ++i) {
        int sz = nd.S[i].n;
        int mask = 0;
        int own = 0;
        for (int p = 0; p < sz; ++p) {
            int v = nd.S[i].a[p];
            mask |= 1 << (v / 10);
            own += (v / 10 == i);
        }
        int size_bucket = (level == 0 ? min(15, sz) : sz);
        h ^= splitmix64(0x100000ULL + i * 1009ULL + (int)nd.fixed[i] * 37ULL +
                        size_bucket * 131ULL + mask * 10007ULL + own * 17ULL);

        int take = (level == 0 ? 3 : 5);
        int st = max<int>(nd.fixed[i], sz - take);
        for (int p = st; p < sz; ++p) {
            int v = nd.S[i].a[p];
            int from_tail = sz - 1 - p;
            int code = (level == 0 ? rel_code_for_coarse(v, nd.next_req) : v);
            h ^= splitmix64(0x200000ULL + i * 4099ULL + from_tail * 193ULL + code * 769ULL);
        }
    }

    for (int j = 0; j < R; ++j) {
        int sz = nd.T[j].n;
        int size_bucket = (level == 0 ? (sz <= 2 ? sz : 2 + (sz - 2) / 2) : sz);
        int mask = 0;
        for (int p = 0; p < sz; ++p) mask |= 1 << ((int)nd.T[j].a[p] / 10);
        h ^= splitmix64(0x300000ULL + j * 6151ULL + size_bucket * 197ULL + mask * 3001ULL);

        int take = min(sz, level == 0 ? 4 : 6);
        for (int p = 0; p < take; ++p) {
            int v = nd.T[j].a[p];
            int code = (level == 0 ? rel_code_for_coarse(v, nd.next_req) : v);
            h ^= splitmix64(0x400000ULL + j * 8191ULL + p * 277ULL + code * 997ULL);
        }

        if (level >= 1 && sz > 0) {
            for (int q = 0; q < min(2, sz); ++q) {
                int v = nd.T[j].a[sz - 1 - q];
                h ^= splitmix64(0x500000ULL + j * 12289ULL + q * 389ULL + v * 1543ULL);
            }
        }
    }
    return h;
}

static inline uint64_t coarse_key(const Node& nd) {
#if COARSE_HASH_ENABLE
    int level = (nd.turn < 34 ? 0 : 1);
    uint64_t db = (uint64_t)(nd.turn / COARSE_DEPTH_BUCKET);
    return coarse_hash(nd, level) ^ splitmix64(0x9e3779b97f4a7c15ULL + db * 1000003ULL + level * 97ULL);
#else
    return nd.h ^ splitmix64((uint64_t)nd.turn);
#endif
}

static inline bool accept_by_coarse_hash(unordered_map<uint64_t, long long>& best_eval, const Node& nd) {
#if COARSE_HASH_ENABLE
    uint64_t key = coarse_key(nd);
    auto it = best_eval.find(key);
    if (it != best_eval.end() && it->second >= nd.eval - COARSE_EVAL_SLACK) return false;
    if (it == best_eval.end() || nd.eval > it->second) best_eval[key] = nd.eval;
#endif
    return true;
}

static inline int ready_front_block_len(const Node& nd, int j, int r, int need) {
    int len = 0;
    const Line& t = nd.T[j];
    while (need + len < 10 && len < (int)t.n) {
        if ((int)t.a[len] != 10 * r + need + len) break;
        ++len;
    }
    return len;
}

static long long ready_matching_dp_value(const Node& nd) {
#if READY_DP_EVAL_ENABLE
    long long w[R][R] = {};
    for (int r = 0; r < R; ++r) {
        if ((int)nd.S[r].n != (int)nd.fixed[r]) continue;
        int need = nd.next_req[r];
        if (need >= 10) continue;
        for (int j = 0; j < R; ++j) {
            int len = ready_front_block_len(nd, j, r, need);
            if (len > 0) w[r][j] = READY_DP_PER_CAR * len + READY_DP_PER_SQ * len * len;
        }
    }
    long long dp[R + 1][R + 1] = {};
    for (int r = R - 1; r >= 0; --r) {
        for (int j = R - 1; j >= 0; --j) {
            long long v = dp[r + 1][j];
            if (dp[r][j + 1] > v) v = dp[r][j + 1];
            if (w[r][j] > 0) {
                long long use = w[r][j] + dp[r + 1][j + 1];
                if (use > v) v = use;
            }
            dp[r][j] = v;
        }
    }
    return dp[0][0];
#else
    (void)nd;
    return 0;
#endif
}

static long long evaluate_state(Node& nd) {
    calc_fixed_next(nd);

    long long sc = 0;
    int fixed_sum = nd.fixed_sum;
    sc += 1000000LL * fixed_sum;
    sc -= 45000LL * nd.turn;

    for (int i = 0; i < R; ++i) {
        int dirty = (int)nd.S[i].n - (int)nd.fixed[i];
        if (dirty == 0) sc += 25000;
        else sc -= 9000LL * dirty;

        for (int x = 0; x < (int)nd.S[i].n; ++x) {
            int v = nd.S[i].a[x];
            if (v / 10 == i) sc += 1200;
        }
    }

    for (int side = 0; side < 2; ++side) {
        const Line* A = (side == 0 ? nd.S : nd.T);
        for (int i = 0; i < R; ++i) {
            for (int p = 0; p + 1 < (int)A[i].n; ++p) {
                int a = A[i].a[p], b = A[i].a[p + 1];
                if (consecutive_cars(a, b)) sc += (side == 0 ? 3500 : 4500);
                else if (a / 10 == b / 10) sc += 600;
            }
        }
    }

    // Original robust ready-block evaluation. Do not penalize non-parallel ready blocks here;
    // that experimental matching penalty tended to delete useful future branches.
    for (int r = 0; r < R; ++r) {
        if ((int)nd.S[r].n != (int)nd.fixed[r]) continue;
        int need = nd.next_req[r];
        if (need >= 10) continue;
        for (int j = 0; j < R; ++j) {
            int len = ready_front_block_len(nd, j, r, need);
            if (len > 0) sc += 150000LL * len + 12000LL * len * len;
        }
    }

    // Additional small DP bonus: among currently ready T-front blocks, prefer
    // sets that are simultaneously placeable under the non-crossing constraint.
    sc += ready_matching_dp_value(nd);

    for (int j = 0; j < R; ++j) {
        int sz = nd.T[j].n;
        if (sz >= 18) sc -= 30000LL * (sz - 17);
    }

    nd.eval = sc;
    nd.h = state_hash(nd);
    return sc;
}

static CandidateSet generate_candidates(const Node& nd, mt19937& rng) {
    CandidateSet cs;

    for (int i = 0; i < R; ++i) {
        int n = nd.S[i].n;
        int unfixed = n - (int)nd.fixed[i];
        if (unfixed <= 0) continue;
        for (int j = 0; j < R; ++j) {
            int max_k = min(unfixed, T_CAP - (int)nd.T[j].n);
            for (int k = 1; k <= max_k; ++k) {
                int v_bot = nd.S[i].a[n - k];
                int v_top = nd.S[i].a[n - 1];
                int link_delta = 0;
                long long score = 1000 + 25LL * k - 7LL * (int)nd.T[j].n;

                bool staged_good_block = false;
                int staged_len = 0;
                int br = v_bot / 10;
                int bp = v_bot % 10;

                // Macro transition: cut the S suffix exactly at the next globally needed
                // car for its destination line, so that car becomes T[j].front().
                // This is valuable even before S[br] is clean; later moves can clean S[br]
                // and place this ready front immediately.
                if (bp == (int)nd.next_req[br]) {
                    while (staged_len < k && bp + staged_len < 10 &&
                           (int)nd.S[i].a[n - k + staged_len] == 10 * br + bp + staged_len) {
                        ++staged_len;
                    }
                    staged_good_block = staged_len > 0;
                    if (staged_good_block) {
                        bool dest_clean = ((int)nd.S[br].n == (int)nd.fixed[br]);
                        score += EXPOSE_NEXT_BASE_BONUS;
                        score += EXPOSE_NEXT_BLOCK_BONUS * staged_len + 9000LL * staged_len * staged_len;
                        if (dest_clean) score += EXPOSE_NEXT_CLEAN_EXTRA + 35000LL * staged_len;
                        if (br == i) score += EXPOSE_NEXT_SRC_LINE_BONUS;
                        if (k == staged_len) score += 12000;

                        // Prefer not to bury a macro-exposed next car under a huge irrelevant suffix,
                        // but keep the move available; sometimes the large suffix is exactly what cleans a line.
                        int garbage_after_block = k - staged_len;
                        if (garbage_after_block > 0) score -= 1800LL * garbage_after_block;
                    }
                }

                if (nd.T[j].n == 0) {
                    score += 350;
                } else {
                    int u = nd.T[j].a[0];
                    score += evaluate_order(u, v_bot, nd.next_req);
                    if (consecutive_cars(v_top, u)) ++link_delta;

                    int ur = u / 10;
                    int up = u % 10;
                    if (up == (int)nd.next_req[ur] && (int)nd.S[ur].n == (int)nd.fixed[ur]) {
                        score -= staged_good_block ? 7000 : 90000;
                    }
                }

                if (n > k && consecutive_cars((int)nd.S[i].a[n - k - 1], v_bot)) {
#if STRICT_BLOCK_MOVE
                    continue;
#else
                    --link_delta;
                    score -= 350;
#endif
                }

                if (k == unfixed) score += 2600;
                if (n - k == (int)nd.fixed[i]) score += 1200;
                if (n - k > (int)nd.fixed[i]) {
                    int e = nd.S[i].a[n - k - 1];
                    int er = e / 10, ep = e % 10;
                    if (ep == (int)nd.next_req[er] && (int)nd.S[er].n == (int)nd.fixed[er]) {
                        score += 26000;
                    }
                }

                score += LINEAR_LINK_BONUS * link_delta;
                score += (int)(rng() % 41);
                cs.add(MoveCandidate{0, (unsigned char)i, (unsigned char)j, (unsigned char)k, score});
            }
        }
    }

    for (int j = 0; j < R; ++j) {
        int tsz = nd.T[j].n;
        if (tsz == 0) continue;
        for (int i = 0; i < R; ++i) {
            int max_k = min(tsz, S_CAP - (int)nd.S[i].n);
            for (int k = 1; k <= max_k; ++k) {
                long long score = 100 + 25LL * k - 5LL * (int)nd.S[i].n;
                int link_delta = 0;

                if (k < tsz && consecutive_cars((int)nd.T[j].a[k - 1], (int)nd.T[j].a[k])) {
#if STRICT_BLOCK_MOVE
                    continue;
#else
                    --link_delta;
                    score -= 350;
#endif
                }

                bool perfect = false;
                if ((int)nd.S[i].n == (int)nd.fixed[i]) {
                    bool ok = true;
                    for (int x = 0; x < k; ++x) {
                        if ((int)nd.T[j].a[x] != 10 * i + (int)nd.next_req[i] + x) {
                            ok = false;
                            break;
                        }
                    }
                    perfect = ok;
                }

                if (perfect) {
                    score = 1200000 + 70000LL * k;
                } else {
                    if ((int)nd.S[i].n == (int)nd.fixed[i]) {
                        score -= 14000;
                    }

                    if (k < tsz) {
                        int u = nd.T[j].a[k];
                        int rr = u / 10;
                        int cc = u % 10;
                        if (cc == (int)nd.next_req[rr]) {
                            int len = 0;
                            while (k + len < tsz && cc + len < 10 &&
                                   (int)nd.T[j].a[k + len] == 10 * rr + cc + len) {
                                ++len;
                            }
                            bool dest_clean = ((int)nd.S[rr].n == (int)nd.fixed[rr]);
                            score += 115000LL + 30000LL * len + 6000LL * len * len;
                            if (dest_clean) score += 65000LL + 18000LL * len;
                            if ((int)nd.S[i].n > (int)nd.fixed[i]) score += 26000;
                            else score -= 3500;
                        }
                    }

                    if ((int)nd.S[i].n > (int)nd.fixed[i]) {
                        int u = nd.S[i].a[nd.S[i].n - 1];
                        int v = nd.T[j].a[0];
                        if (u / 10 == v / 10) {
                            score += (u % 10 < v % 10) ? 700 : -700;
                        } else {
                            score += evaluate_order(u, v, nd.next_req);
                        }
                        if (consecutive_cars(u, v)) ++link_delta;
                    } else if (nd.S[i].n == 0 && (int)nd.T[j].a[0] == 10 * i) {
                        ++link_delta;
                    }

                    score += LINEAR_LINK_BONUS * link_delta;
                }

                score += (int)(rng() % 41);
                cs.add(MoveCandidate{1, (unsigned char)i, (unsigned char)j, (unsigned char)k, score});
            }
        }
    }

    return cs;
}

static inline void sort_turn(TurnMoves& mv) {
    for (int a = 1; a < (int)mv.cnt; ++a) {
        Move x = mv.m[a];
        int b = a - 1;
        while (b >= 0) {
            const Move& y = mv.m[b];
            bool gt = (y.i > x.i) || (y.i == x.i && (y.j > x.j || (y.j == x.j && (y.type > x.type || (y.type == x.type && y.k > x.k)))));
            if (!gt) break;
            mv.m[b + 1] = mv.m[b];
            --b;
        }
        mv.m[b + 1] = x;
    }
}

static uint64_t turn_key(const TurnMoves& mv) {
    uint64_t h = 0x243f6a8885a308d3ULL ^ (uint64_t)mv.cnt;
    for (int p = 0; p < (int)mv.cnt; ++p) {
        const Move& m = mv.m[p];
        uint64_t code = (uint64_t)m.type;
        code = code * 16 + m.i;
        code = code * 16 + m.j;
        code = code * 32 + m.k;
        h ^= splitmix64(code + 0x9e3779b97f4a7c15ULL + (h << 1));
    }
    return h;
}

static TurnMoves select_rect_best(const CandidateSet& cs, int il, int ir, int jl, int jr) {
    long long dp[R + 1][R + 1];
    unsigned char take[R][R] = {};
    for (int i = 0; i <= R; ++i) for (int j = 0; j <= R; ++j) dp[i][j] = 0;

    for (int i = ir - 1; i >= il; --i) {
        for (int j = jr - 1; j >= jl; --j) {
            long long v = dp[i + 1][j];
            if (dp[i][j + 1] > v) v = dp[i][j + 1];
            if (cs.has[i][j]) {
                long long use = cs.best[i][j].score + dp[i + 1][j + 1];
                if (use > v) {
                    v = use;
                    take[i][j] = 1;
                }
            }
            dp[i][j] = v;
        }
    }

    TurnMoves res;
    int i = il, j = jl;
    while (i < ir && j < jr) {
        if (take[i][j] && cs.has[i][j] && cs.best[i][j].score + dp[i + 1][j + 1] == dp[i][j]) {
            const auto& c = cs.best[i][j];
            res.push(Move{c.type, c.i, c.j, c.k});
            ++i; ++j;
        } else if (dp[i + 1][j] >= dp[i][j + 1]) {
            ++i;
        } else {
            ++j;
        }
    }
    return res;
}

static vector<TurnMoves> make_turn_variants(const CandidateSet& cs) {
    vector<TurnMoves> variants;
    variants.reserve(FORCE_BRANCH + 8);
    uint64_t used[FORCE_BRANCH + 16];
    int used_cnt = 0;

    auto add_variant = [&](TurnMoves mv) {
        if (mv.empty()) return;
        sort_turn(mv);
        uint64_t key = turn_key(mv);
        for (int i = 0; i < used_cnt; ++i) if (used[i] == key) return;
        used[used_cnt++] = key;
        variants.push_back(mv);
    };

    add_variant(select_rect_best(cs, 0, R, 0, R));

    int lim = min<int>(FORCE_BRANCH, cs.top.size());
    for (int idx = 0; idx < lim; ++idx) {
        const auto& f = cs.top[idx];
        TurnMoves left = select_rect_best(cs, 0, f.i, 0, f.j);
        TurnMoves right = select_rect_best(cs, (int)f.i + 1, R, (int)f.j + 1, R);
        TurnMoves mv;
        for (int p = 0; p < (int)left.cnt; ++p) mv.push(left.m[p]);
        mv.push(Move{f.type, f.i, f.j, f.k});
        for (int p = 0; p < (int)right.cnt; ++p) mv.push(right.m[p]);
        add_variant(mv);
    }

    for (int idx = 0; idx < min<int>(5, cs.top.size()); ++idx) {
        const auto& c = cs.top[idx];
        TurnMoves mv;
        mv.push(Move{c.type, c.i, c.j, c.k});
        add_variant(mv);
    }

    return variants;
}

static bool validate_moves_against(const Node& par, const TurnMoves& moves) {
    if (moves.cnt == 0 || moves.cnt > R) return false;
    unsigned short usedS = 0, usedT = 0;
    pair<int,int> pairs[R];
    for (int idx = 0; idx < (int)moves.cnt; ++idx) {
        const Move& m = moves.m[idx];
        if (m.type > 1 || m.i >= R || m.j >= R || m.k == 0) return false;
        if ((usedS >> m.i) & 1) return false;
        if ((usedT >> m.j) & 1) return false;
        usedS |= (1u << m.i);
        usedT |= (1u << m.j);
        pairs[idx] = {(int)m.i, (int)m.j};

        if (m.type == 0) {
            if ((int)par.S[m.i].n < (int)m.k) return false;
            if ((int)par.T[m.j].n + (int)m.k > T_CAP) return false;
        } else {
            if ((int)par.T[m.j].n < (int)m.k) return false;
            if ((int)par.S[m.i].n + (int)m.k > S_CAP) return false;
        }
    }

    for (int a = 1; a < (int)moves.cnt; ++a) {
        auto x = pairs[a];
        int b = a - 1;
        while (b >= 0 && pairs[b].first > x.first) {
            pairs[b + 1] = pairs[b];
            --b;
        }
        pairs[b + 1] = x;
    }
    for (int p = 1; p < (int)moves.cnt; ++p) {
        if (!(pairs[p - 1].first < pairs[p].first && pairs[p - 1].second < pairs[p].second)) return false;
    }
    return true;
}

static inline void do_S_to_T(Node& st, int i, int j, int k) {
    Line& s = st.S[i];
    Line& t = st.T[j];
    unsigned char tmp[20];
    memcpy(tmp, s.a + s.n - k, k);
    memmove(t.a + k, t.a, t.n);
    memcpy(t.a, tmp, k);
    s.n -= (unsigned char)k;
    t.n += (unsigned char)k;
}

static inline void do_T_to_S(Node& st, int j, int i, int k) {
    Line& t = st.T[j];
    Line& s = st.S[i];
    memcpy(s.a + s.n, t.a, k);
    memmove(t.a, t.a + k, t.n - k);
    s.n += (unsigned char)k;
    t.n -= (unsigned char)k;
}

static bool apply_moves(const Node& par, const TurnMoves& moves, Node& ch) {
    if (!validate_moves_against(par, moves)) return false;

    ch = par; // fixed-size copy only; no heap allocations
    ch.parent = -1;
    ch.turn = par.turn + 1;
    ch.last_cnt = moves.cnt;
    for (int p = 0; p < (int)moves.cnt; ++p) ch.last[p] = moves.m[p];

    for (int idx = 0; idx < (int)moves.cnt; ++idx) {
        const Move& m = moves.m[idx];
        if (m.type == 0) do_S_to_T(ch, m.i, m.j, m.k);
        else do_T_to_S(ch, m.j, m.i, m.k);
    }
    evaluate_state(ch);
    return true;
}

static vector<TurnMoves> reconstruct(const vector<Node>& pool, int idx) {
    vector<TurnMoves> ans;
    while (idx >= 0) {
        const Node& nd = pool[idx];
        if (nd.last_cnt > 0) {
            TurnMoves mv;
            mv.cnt = nd.last_cnt;
            for (int p = 0; p < (int)nd.last_cnt; ++p) mv.m[p] = nd.last[p];
            ans.push_back(mv);
        }
        idx = nd.parent;
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

static vector<TurnMoves> greedy_fallback(const Node& initial, double sec_limit = 0.18) {
    Timer timer;
    mt19937 rng(712367);
    vector<TurnMoves> best;
    int best_turns = INT_MAX;

    while (timer.elapsed() < sec_limit) {
        Node cur = initial;
        vector<TurnMoves> ops;
        ops.reserve(256);
        bool failed = false;
        unordered_set<uint64_t> seen;
        seen.reserve(8192);
        seen.insert(cur.h);

        for (int depth = cur.turn; depth < MAX_DEPTH; ++depth) {
            if (is_done(cur)) break;
            auto cs = generate_candidates(cur, rng);
            if (cs.empty()) { failed = true; break; }

            TurnMoves mv;
            if (!ops.empty() && (rng() % 100) < 12) {
                const auto& c = cs.top[rng() % min<int>(20, cs.top.size())];
                mv.push(Move{c.type, c.i, c.j, c.k});
            } else {
                mv = select_rect_best(cs, 0, R, 0, R);
                if (mv.empty()) {
                    const auto& c = cs.top[0];
                    mv.push(Move{c.type, c.i, c.j, c.k});
                }
            }

            Node nxt;
            if (!apply_moves(cur, mv, nxt)) { failed = true; break; }
            if (seen.count(nxt.h)) {
                const auto& c = cs.top[rng() % min<int>(30, cs.top.size())];
                mv.clear();
                mv.push(Move{c.type, c.i, c.j, c.k});
                if (!apply_moves(cur, mv, nxt)) { failed = true; break; }
            }
            seen.insert(nxt.h);
            ops.push_back(mv);
            cur = nxt;
            if ((int)ops.size() >= 3950) { failed = true; break; }
        }

        if (!failed && is_done(cur) && (int)ops.size() < best_turns) {
            best_turns = (int)ops.size();
            best = ops;
        }
    }
    return best;
}

struct PQEntry {
    long long eval;
    uint64_t tie;
    int idx;
    bool operator<(const PQEntry& other) const {
        if (eval != other.eval) return eval < other.eval;
        return tie < other.tie;
    }
};

static inline int fixed_sum_of(const Node& nd) {
    return nd.fixed_sum;
}

static vector<TurnMoves> layered_beam_warm_start(const Node& initial,
                                                 const TurnMoves& init_moves,
                                                 vector<TurnMoves> best_answer,
                                                 Timer& global_timer,
                                                 mt19937& rng,
                                                 double sec_budget) {
    double end_time = min(TIME_LIMIT_SEC - SEARCH_GUARD_SEC, global_timer.elapsed() + sec_budget);

    vector<Node> pool;
    pool.reserve(220000);
    Node st = initial;
    st.parent = -1;
    st.last_cnt = 0;
    evaluate_state(st);
    pool.push_back(st);

    vector<int> beam = {0};
    unordered_map<uint64_t, unsigned short> global_seen_depth;
    global_seen_depth.reserve(320000);
    global_seen_depth[pool[0].h] = pool[0].turn;

    int best_turns = best_answer.empty() ? MAX_DEPTH + 1 : (int)best_answer.size();
    int found_idx = -1;

    for (int depth = initial.turn; depth < MAX_DEPTH; ++depth) {
        if (global_timer.elapsed() > end_time) break;
        if (depth + 1 >= best_turns) break;

        vector<Node> next_nodes;
        next_nodes.reserve(BEAM_WIDTH * (FORCE_BRANCH + 8));

        for (int bi = 0; bi < (int)beam.size(); ++bi) {
            if ((bi & 3) == 0 && global_timer.elapsed() > end_time) break;
            int idx = beam[bi];
            const Node& cur = pool[idx];
            if (is_done(cur)) { found_idx = idx; break; }

            auto cs = generate_candidates(cur, rng);
            if (cs.empty()) continue;
            auto variants = make_turn_variants(cs);
            for (const auto& mv : variants) {
                Node ch;
                if (!apply_moves(cur, mv, ch)) continue;
                if (ch.turn >= best_turns) continue;
                auto it = global_seen_depth.find(ch.h);
                if (it != global_seen_depth.end() && it->second <= ch.turn) continue;
                global_seen_depth[ch.h] = ch.turn;
                ch.parent = idx;
                next_nodes.push_back(ch);
            }
        }
        if (found_idx >= 0) break;
        if (next_nodes.empty()) break;

        unordered_map<uint64_t, int> pos;
        pos.reserve(next_nodes.size() * 2 + 1);
        vector<Node> uniq;
        uniq.reserve(next_nodes.size());
        for (auto& nd : next_nodes) {
            uint64_t key = nd.h;
            auto it = pos.find(key);
            if (it == pos.end()) {
                pos[key] = (int)uniq.size();
                uniq.push_back(nd);
            } else {
                int p = it->second;
                if (nd.eval > uniq[p].eval) uniq[p] = nd;
            }
        }

        int keep = min<int>(BEAM_WIDTH, uniq.size());
        auto cmp = [](const Node& a, const Node& b) {
            if (a.eval != b.eval) return a.eval > b.eval;
            return a.turn < b.turn;
        };
        if ((int)uniq.size() > keep) {
            nth_element(uniq.begin(), uniq.begin() + keep, uniq.end(), cmp);
            uniq.resize(keep);
        }
        sort(uniq.begin(), uniq.end(), cmp);

        beam.clear();
        beam.reserve(keep);
        for (int i = 0; i < keep; ++i) {
            int new_idx = (int)pool.size();
            pool.push_back(uniq[i]);
            beam.push_back(new_idx);
            if (is_done(pool.back())) {
                found_idx = new_idx;
                break;
            }
        }
        if (found_idx >= 0) break;
    }

    if (found_idx >= 0) {
        vector<TurnMoves> ans = reconstruct(pool, found_idx);
        if (!init_moves.empty()) ans.insert(ans.begin(), init_moves);
        if ((int)ans.size() < best_turns) best_answer = ans;
    }
    return best_answer;
}

static bool same_layout(const Node& a, const Node& b) {
    for (int i = 0; i < R; ++i) {
        if (a.S[i].n != b.S[i].n || memcmp(a.S[i].a, b.S[i].a, a.S[i].n) != 0) return false;
        if (a.T[i].n != b.T[i].n || memcmp(a.T[i].a, b.T[i].a, a.T[i].n) != 0) return false;
    }
    return true;
}

static bool simulate_sequence(const Node& root, const vector<TurnMoves>& ops, Node& out) {
    Node cur = root;
    cur.parent = -1;
    cur.last_cnt = 0;
    evaluate_state(cur);
    for (const auto& mv : ops) {
        Node nxt;
        if (!apply_moves(cur, mv, nxt)) return false;
        cur = nxt;
    }
    out = cur;
    return true;
}

static vector<TurnMoves> compact_answer(const Node& root, vector<TurnMoves> ans, Timer& global_timer) {
    bool changed = true;
    for (int pass = 0; pass < 4 && changed && global_timer.elapsed() < TIME_LIMIT_SEC - COMPACT_GUARD_SEC; ++pass) {
        changed = false;
        Node cur = root;
        evaluate_state(cur);
        for (int t = 0; t + 1 < (int)ans.size() && global_timer.elapsed() < TIME_LIMIT_SEC - COMPACT_GUARD_SEC; ++t) {
            Node after1, after2;
            if (!apply_moves(cur, ans[t], after1)) break;
            if (!apply_moves(after1, ans[t + 1], after2)) break;

            if ((int)ans[t].cnt + (int)ans[t + 1].cnt <= R) {
                TurnMoves uni;
                for (int p = 0; p < (int)ans[t].cnt; ++p) uni.push(ans[t].m[p]);
                for (int p = 0; p < (int)ans[t + 1].cnt; ++p) uni.push(ans[t + 1].m[p]);
                sort_turn(uni);
                Node merged;
                if (apply_moves(cur, uni, merged) && same_layout(merged, after2)) {
                    ans[t] = uni;
                    ans.erase(ans.begin() + t + 1);
                    changed = true;
                    break;
                }
            }
            cur = after1;
        }
    }

    for (int pass = 0; pass < 2 && global_timer.elapsed() < TIME_LIMIT_SEC - COMPACT_GUARD_SEC; ++pass) {
        bool removed = false;
        for (int t = 0; t < (int)ans.size() && global_timer.elapsed() < TIME_LIMIT_SEC - COMPACT_GUARD_SEC; ++t) {
            vector<TurnMoves> tmp;
            tmp.reserve(ans.size() - 1);
            for (int u = 0; u < (int)ans.size(); ++u) if (u != t) tmp.push_back(ans[u]);
            Node fin;
            if (simulate_sequence(root, tmp, fin) && is_done(fin)) {
                ans = tmp;
                removed = true;
                break;
            }
        }
        if (!removed) break;
    }

    return ans;
}

static vector<TurnMoves> chokudai_search(const Node& initial,
                                         const TurnMoves& init_moves,
                                         vector<TurnMoves> best_answer,
                                         Timer& global_timer,
                                         mt19937& rng,
                                         double stop_time = TIME_LIMIT_SEC - SEARCH_GUARD_SEC) {
    stop_time = min(stop_time, TIME_LIMIT_SEC - SEARCH_GUARD_SEC);
    vector<Node> pool;
    pool.reserve(min(900000, CHOKUDAI_POOL_LIMIT + 1000));

    Node start = initial;
    start.parent = -1;
    start.last_cnt = 0;
    evaluate_state(start);
    pool.push_back(start);

    int best_turns = best_answer.empty() ? MAX_DEPTH + 1 : (int)best_answer.size();

    vector<priority_queue<PQEntry>> que(MAX_DEPTH + 1);
    que[pool[0].turn].push(PQEntry{pool[0].eval, splitmix64(pool[0].h ^ 0xabcdefULL), 0});

    unordered_map<uint64_t, unsigned short> seen_depth;
    seen_depth.reserve(CHOKUDAI_POOL_LIMIT * 2);
    seen_depth[pool[0].h] = pool[0].turn;

    unordered_map<uint64_t, long long> coarse_seen_eval;
    coarse_seen_eval.reserve(CHOKUDAI_POOL_LIMIT * 2);
    coarse_seen_eval[coarse_key(pool[0])] = pool[0].eval;

    auto register_solution = [&](int idx) {
        vector<TurnMoves> ans = reconstruct(pool, idx);
        if (!init_moves.empty()) ans.insert(ans.begin(), init_moves);
        if ((int)ans.size() < best_turns) {
            best_turns = (int)ans.size();
            best_answer = ans;
        }
    };

    if (is_done(pool[0])) register_solution(0);

    while (global_timer.elapsed() < stop_time) {
        bool any_expanded_in_round = false;
        int max_d = min(best_turns - 1, MAX_DEPTH - 1);
        if (max_d < initial.turn) break;

        for (int d = initial.turn; d <= max_d; ++d) {
            if (global_timer.elapsed() > stop_time) break;
            if (que[d].empty()) continue;

            for (int pop_iter = 0; pop_iter < CHOKUDAI_POP_PER_DEPTH && !que[d].empty(); ++pop_iter) {
                if (global_timer.elapsed() > stop_time) break;
                PQEntry ent = que[d].top();
                que[d].pop();
                if (ent.idx < 0 || ent.idx >= (int)pool.size()) continue;

                const Node& cur = pool[ent.idx];
                if ((int)cur.turn != d) continue;
                if (is_done(cur)) {
                    register_solution(ent.idx);
                    max_d = min(best_turns - 1, MAX_DEPTH - 1);
                    continue;
                }
                if (d + 1 >= best_turns) continue;

                any_expanded_in_round = true;

                auto cs = generate_candidates(cur, rng);
                if (cs.empty()) continue;
                auto variants = make_turn_variants(cs);
                if ((int)variants.size() > CHOKUDAI_BRANCH_LIMIT) variants.resize(CHOKUDAI_BRANCH_LIMIT);

                for (const auto& mv : variants) {
                    if (global_timer.elapsed() > stop_time) break;
                    Node ch;
                    if (!apply_moves(cur, mv, ch)) continue;
                    if (ch.turn >= best_turns) continue;

                    auto it = seen_depth.find(ch.h);
                    if (it != seen_depth.end() && it->second <= ch.turn) continue;
                    if (!accept_by_coarse_hash(coarse_seen_eval, ch)) continue;
                    seen_depth[ch.h] = ch.turn;

                    ch.parent = ent.idx;
                    int new_idx = (int)pool.size();
                    pool.push_back(ch);

                    if (is_done(pool[new_idx])) {
                        register_solution(new_idx);
                        continue;
                    }

                    int nd = pool[new_idx].turn;
                    if (nd + (100 - fixed_sum_of(pool[new_idx])) / 10 >= best_turns + 20) continue;

                    if ((int)que[nd].size() < CHOKUDAI_QUEUE_SOFT_LIMIT ||
                        (pool[new_idx].eval > que[nd].top().eval - 1500000LL)) {
                        que[nd].push(PQEntry{pool[new_idx].eval, splitmix64(pool[new_idx].h ^ (uint64_t)new_idx), new_idx});
                    }
                }
            }
        }

        if (!any_expanded_in_round) break;
        if ((int)pool.size() > CHOKUDAI_POOL_LIMIT) break;
    }

    return best_answer;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int inputR;
    if (!(cin >> inputR)) return 0;

    Node root;
    for (int i = 0; i < R; ++i) {
        root.S[i].n = 10;
        root.T[i].n = 0;
        for (int c = 0; c < 10; ++c) {
            int x; cin >> x;
            root.S[i].a[c] = (unsigned char)x;
        }
    }
    evaluate_state(root);

    vector<TurnMoves> best_answer;
    if (is_done(root)) {
        cout << 0 << '\n';
        return 0;
    }

    // Classic all-evacuation is kept only as a cheap fallback/floor.
    // The main search starts from the real root so the first turn can choose macro cuts:
    // e.g. cut at car 0/10/20..., then after a prefix is fixed, cut at the next needed car.
    TurnMoves classic_init_moves;
    for (int i = 0; i < R; ++i) {
        int k = 10 - (int)root.fixed[i];
        if (k > 0) classic_init_moves.push(Move{0, (unsigned char)i, (unsigned char)i, (unsigned char)k});
    }

    Node classic_initial = root;
    if (!classic_init_moves.empty()) {
        Node tmp;
        if (apply_moves(root, classic_init_moves, tmp)) {
            tmp.parent = -1;
            tmp.last_cnt = 0;
            classic_initial = tmp;
        }
    }

    Timer global_timer;
    mt19937 rng(998244353);

    // Small fallback pass with the old forced evacuation. Keep it only if it
    // actually completes; do not use a partial one-turn answer as a pruning bound.
    TurnMoves emergency_partial = classic_init_moves;
    double warm = min<double>(GREEDY_WARMUP_SEC * 0.55, max(0.012, TIME_LIMIT_SEC * 0.025));
    vector<TurnMoves> greedy_ans = greedy_fallback(classic_initial, warm);
    if (!greedy_ans.empty()) {
        if (!classic_init_moves.empty()) greedy_ans.insert(greedy_ans.begin(), classic_init_moves);
        best_answer = greedy_ans;
    }

    TurnMoves no_init_moves;
    Node initial = root;
    initial.parent = -1;
    initial.last_cnt = 0;
    evaluate_state(initial);

    // Dual-start, but still simple:
    // - classic start keeps the historically strong all-evacuation route.
    // - root start can choose first-turn macro cuts.
    best_answer = layered_beam_warm_start(classic_initial, classic_init_moves, best_answer,
                                          global_timer, rng, TIME_LIMIT_SEC * CLASSIC_BEAM_RATIO);
    best_answer = layered_beam_warm_start(initial, no_init_moves, best_answer,
                                          global_timer, rng, TIME_LIMIT_SEC * ROOT_BEAM_RATIO);

    if (global_timer.elapsed() < TIME_LIMIT_SEC - SEARCH_GUARD_SEC) {
        double stop = min(TIME_LIMIT_SEC - SEARCH_GUARD_SEC,
                          global_timer.elapsed() + TIME_LIMIT_SEC * CLASSIC_CHOKUDAI_RATIO);
        best_answer = chokudai_search(classic_initial, classic_init_moves, best_answer, global_timer, rng, stop);
    }

    if (global_timer.elapsed() < TIME_LIMIT_SEC - SEARCH_GUARD_SEC) {
        best_answer = chokudai_search(initial, no_init_moves, best_answer, global_timer, rng);
    }

    if (best_answer.empty() && !emergency_partial.empty()) {
        best_answer = {emergency_partial};
    }

    best_answer = compact_answer(root, best_answer, global_timer);

    if ((int)best_answer.size() > 4000) best_answer.resize(4000);

    cout << best_answer.size() << '\n';
    for (const auto& turn : best_answer) {
        cout << (int)turn.cnt << '\n';
        for (int p = 0; p < (int)turn.cnt; ++p) {
            const Move& m = turn.m[p];
            cout << (int)m.type << ' ' << (int)m.i << ' ' << (int)m.j << ' ' << (int)m.k << '\n';
        }
    }
    return 0;
}
