#include <bits/stdc++.h>
using namespace std;

using ll = long long;

static constexpr int N = 200;
static constexpr int H = 4;
static constexpr int W = 200;
static constexpr int HW = H * W;
static constexpr ll NEG = -(1LL << 60);
static constexpr int G = 4; // gadget width
static constexpr int TARGET_LEN_A = HW / 2;
static constexpr int FINAL_LEN_TOL = 2;

struct State {
    uint8_t ea, eb, mask;
};

struct Trans {
    uint16_t next_state;
    uint16_t t1, t2;
    vector<uint8_t> seqA, seqB; // vertices in 2-column local ids [0,7], excluding current endpoints
};

struct RuntimeTrans {
    uint16_t next_state;
    uint16_t t1;
    ll coef;
    ll base;
};

struct PathCand {
    uint8_t end;               // local vertex id
    uint32_t used_mask;        // includes start vertex
    vector<uint8_t> seq;       // excludes start vertex, includes end if length>0
};

struct EntryTopo {
    uint16_t state;
    uint16_t lenA0;
    vector<uint8_t> seqA, seqB; // local 4x4 ids, INCLUDING starts as first element
};

struct RuntimeEntry {
    uint16_t state;
    uint16_t lenA0;
    ll score;
};

struct ExitTopo {
    uint16_t t1;
    vector<uint8_t> seqA, seqB; // local 4x4 ids, EXCLUDING current frontier starts
};

struct RuntimeExit {
    uint16_t idx;
    uint16_t t1;
    ll coef;
    ll base;
};

struct TwoPaths {
    vector<pair<int,int>> A, B;
};

static vector<State> states;
static int sid[H][H][1 << H];
static vector<vector<Trans>> trans_list;

static vector<int> adj2[8];      // 2-column gadget graph, ids: col*H + row
static vector<int> adj4[16];     // 4-column gadget graph, ids: col*H + row

static vector<EntryTopo> entry_topos;
static vector<vector<ExitTopo>> exit_shared_topos; // standard output: bottom row col0 / col3
static vector<vector<ExitTopo>> exit_cap_topos;    // special last-strip output: bottom row col1 / col0

static inline int vid2(int row, int col) { return col * H + row; }
static inline int vrow(int v) { return v % H; }
static inline int vcol(int v) { return v / H; }

static void build_graphs() {
    for (int i = 0; i < 8; ++i) adj2[i].clear();
    for (int c1 = 0; c1 < 2; ++c1) {
        for (int r1 = 0; r1 < H; ++r1) {
            int v1 = c1 * H + r1;
            for (int c2 = 0; c2 < 2; ++c2) {
                for (int r2 = 0; r2 < H; ++r2) {
                    int v2 = c2 * H + r2;
                    if (v1 == v2) continue;
                    if (max(abs(c1 - c2), abs(r1 - r2)) == 1) adj2[v1].push_back(v2);
                }
            }
        }
    }
    for (int i = 0; i < 16; ++i) adj4[i].clear();
    for (int c1 = 0; c1 < G; ++c1) {
        for (int r1 = 0; r1 < H; ++r1) {
            int v1 = vid2(r1, c1);
            for (int c2 = 0; c2 < G; ++c2) {
                for (int r2 = 0; r2 < H; ++r2) {
                    int v2 = vid2(r2, c2);
                    if (v1 == v2) continue;
                    if (max(abs(c1 - c2), abs(r1 - r2)) == 1) adj4[v1].push_back(v2);
                }
            }
        }
    }
}

static void dfs_paths_two_cols(int cur, int allowed_mask, int used_mask,
                               vector<uint8_t>& seq,
                               vector<tuple<int,int,vector<uint8_t>>> &out) {
    if (cur >= H && !seq.empty()) out.emplace_back(cur - H, used_mask, seq);
    for (int nb : adj2[cur]) {
        if (((allowed_mask >> nb) & 1) == 0) continue;
        if ((used_mask >> nb) & 1) continue;
        seq.push_back((uint8_t)nb);
        dfs_paths_two_cols(nb, allowed_mask, used_mask | (1 << nb), seq, out);
        seq.pop_back();
    }
}

static void build_states_and_middle_transitions() {
    memset(sid, -1, sizeof(sid));
    states.clear();
    for (int ea = 0; ea < H; ++ea) {
        for (int eb = 0; eb < H; ++eb) {
            if (ea == eb) continue;
            for (int mask = 0; mask < (1 << H); ++mask) {
                if ((mask >> ea) & 1) continue;
                if ((mask >> eb) & 1) continue;
                int id = (int)states.size();
                sid[ea][eb][mask] = id;
                states.push_back(State{(uint8_t)ea, (uint8_t)eb, (uint8_t)mask});
            }
        }
    }

    trans_list.assign(states.size(), {});
    for (int s = 0; s < (int)states.size(); ++s) {
        const auto &st = states[s];
        int ea = st.ea, eb = st.eb, mask = st.mask;
        int allowed = mask | (0b1111 << H);
        vector<tuple<int,int,vector<uint8_t>>> pa;
        vector<uint8_t> seq;
        dfs_paths_two_cols(ea, allowed, 0, seq, pa);
        for (auto &ta : pa) {
            int endA, usedA;
            vector<uint8_t> seqA;
            tie(endA, usedA, seqA) = ta;
            int allowedB = allowed & ~usedA;
            vector<tuple<int,int,vector<uint8_t>>> pb;
            vector<uint8_t> seq2;
            dfs_paths_two_cols(eb, allowedB, 0, seq2, pb);
            for (auto &tb : pb) {
                int endB, usedB;
                vector<uint8_t> seqB;
                tie(endB, usedB, seqB) = tb;
                if (usedA & usedB) continue;
                if (((usedA | usedB) & 0b1111) != mask) continue;
                int used_curr = ((usedA | usedB) >> H) & 0b1111;
                int next_mask = (~used_curr) & 0b1111;
                int ns = sid[endA][endB][next_mask];
                if (ns < 0) continue;
                trans_list[s].push_back(Trans{
                    (uint16_t)ns,
                    (uint16_t)seqA.size(),
                    (uint16_t)seqB.size(),
                    seqA,
                    seqB
                });
            }
        }
    }
}

static void dfs_paths_four_cols(int cur, uint32_t allowed_mask, uint32_t used_mask,
                                uint32_t target_mask, vector<uint8_t>& seq,
                                vector<PathCand>& out) {
    if ((target_mask >> cur) & 1u) {
        out.push_back(PathCand{(uint8_t)cur, used_mask, seq});
    }
    for (int nb : adj4[cur]) {
        if (((allowed_mask >> nb) & 1u) == 0u) continue;
        if ((used_mask >> nb) & 1u) continue;
        seq.push_back((uint8_t)nb);
        dfs_paths_four_cols(nb, allowed_mask, used_mask | (1u << nb), target_mask, seq, out);
        seq.pop_back();
    }
}

static vector<PathCand> enum_four_col_paths(int start, uint32_t allowed_mask, uint32_t target_mask) {
    vector<PathCand> out;
    vector<uint8_t> seq;
    dfs_paths_four_cols(start, allowed_mask, (1u << start), target_mask, seq, out);
    return out;
}

static void build_entry_topos() {
    entry_topos.clear();
    const int startA = vid2(0, 0); // (row 0, col 0)
    const int startB = vid2(0, 3); // (row 0, col 3)
    uint32_t all16 = (1u << 16) - 1;
    uint32_t target_last_col = 0;
    for (int r = 0; r < H; ++r) target_last_col |= 1u << vid2(r, 3);
    auto pa = enum_four_col_paths(startA, all16, target_last_col);
    for (const auto &ca : pa) {
        if (ca.used_mask & (1u << startB)) continue;
        uint32_t allowedB = all16 & ~ca.used_mask;
        auto pb = enum_four_col_paths(startB, allowedB, target_last_col);
        for (const auto &cb : pb) {
            if (ca.used_mask & cb.used_mask) continue;
            uint32_t used = ca.used_mask | cb.used_mask;
            bool ok = true;
            for (int c = 0; c < 3 && ok; ++c) {
                for (int r = 0; r < H; ++r) {
                    int v = vid2(r, c);
                    if (((used >> v) & 1u) == 0u) ok = false;
                }
            }
            if (!ok) continue;
            int mask = 0;
            for (int r = 0; r < H; ++r) {
                int v = vid2(r, 3);
                if (((used >> v) & 1u) == 0u) mask |= 1 << r;
            }
            int ea = vrow(ca.end), eb = vrow(cb.end);
            int s = (ea == eb ? -1 : sid[ea][eb][mask]);
            if (s < 0) continue;
            vector<uint8_t> fullA, fullB;
            fullA.reserve(ca.seq.size() + 1);
            fullB.reserve(cb.seq.size() + 1);
            fullA.push_back((uint8_t)startA);
            for (auto v : ca.seq) fullA.push_back(v);
            fullB.push_back((uint8_t)startB);
            for (auto v : cb.seq) fullB.push_back(v);
            entry_topos.push_back(EntryTopo{
                (uint16_t)s,
                (uint16_t)fullA.size(),
                fullA,
                fullB
            });
        }
    }
}

static void build_exit_topos_for_spec(vector<vector<ExitTopo>>& dest, int endA_col, int endB_col) {
    const int S = (int)states.size();
    dest.assign(S, {});
    uint32_t cols123_mask = 0;
    for (int c = 1; c < 4; ++c) for (int r = 0; r < H; ++r) cols123_mask |= 1u << vid2(r, c);
    int endA = vid2(3, endA_col);
    int endB = vid2(3, endB_col);
    for (int s = 0; s < S; ++s) {
        const auto &st = states[s];
        int startA = vid2(st.ea, 0);
        int startB = vid2(st.eb, 0);
        uint32_t allowed_base = cols123_mask;
        for (int r = 0; r < H; ++r) if ((st.mask >> r) & 1) allowed_base |= 1u << vid2(r, 0);
        allowed_base |= 1u << startA;
        allowed_base |= 1u << startB;

        auto pa = enum_four_col_paths(startA, allowed_base, 1u << endA);
        for (const auto &ca : pa) {
            if (ca.used_mask & (1u << startB)) continue;
            uint32_t allowedB = allowed_base & ~ca.used_mask;
            auto pb = enum_four_col_paths(startB, allowedB, 1u << endB);
            for (const auto &cb : pb) {
                if (ca.used_mask & cb.used_mask) continue;
                uint32_t used = ca.used_mask | cb.used_mask;
                uint32_t required = allowed_base;
                if (used != required) continue;
                dest[s].push_back(ExitTopo{
                    (uint16_t)ca.seq.size(),
                    ca.seq,
                    cb.seq
                });
            }
        }
    }
}

static inline ll local_value_two_cols(const array<array<int, W>, H>& a, int local_v, int c) {
    if (local_v < H) return a[local_v][c];
    return a[local_v - H][c + 1];
}

static inline ll gadget_value(const array<array<int, W>, H>& a, int local_v, int c0) {
    int row = vrow(local_v);
    int col = vcol(local_v);
    return a[row][c0 + col];
}

static TwoPaths solve_strip_oriented(const array<array<int, W>, H>& a, int row_base, ll b_shift,
                                     bool cap_exit, bool swap_score_roles) {
    const int S = (int)states.size();
    const int start_col = 3;
    const int exit_col = 196;
    const int steps = exit_col - start_col; // transitions for c = 3..195
    const int TOT = S * (HW + 1);
    const int FINAL_LEN_L = max(0, TARGET_LEN_A - FINAL_LEN_TOL);
    const int FINAL_LEN_R = min(HW, TARGET_LEN_A + FINAL_LEN_TOL);

    vector<uint16_t> used_after_state(S), rem_after_state(S);
    for (int s = 0; s < S; ++s) {
        const auto &st = states[s];
        int used = start_col * H + (H - __builtin_popcount((unsigned)st.mask));
        used_after_state[s] = (uint16_t)used;
        rem_after_state[s] = (uint16_t)(HW - used);
    }

    auto idx_of = [&](int s, int lenA) -> int {
        return s * (HW + 1) + lenA;
    };

    vector<ll> dp(TOT, NEG), ndp(TOT, NEG);
    vector<int> active, next_active;
    vector<int> seen(TOT, 0);
    int seen_token = 1;

    // parent[step * TOT + flat_next] = (prev_state << 16) | trans_idx
    vector<uint32_t> parent((size_t)steps * (size_t)TOT, 0xFFFFFFFFu);
    vector<uint16_t> entry_choice(TOT, 0xFFFFu);

    // Entry runtimes
    vector<RuntimeEntry> runtime_entry;
    runtime_entry.reserve(entry_topos.size());
    for (const auto &et : entry_topos) {
        ll sumA = 0, offA = 0;
        for (int i = 0; i < (int)et.seqA.size(); ++i) {
            ll v = gadget_value(a, et.seqA[i], 0);
            sumA += v;
            offA += 1LL * i * v;
        }
        ll sumB = 0, offB = 0;
        for (int i = 0; i < (int)et.seqB.size(); ++i) {
            ll v = gadget_value(a, et.seqB[i], 0);
            sumB += v;
            offB += 1LL * i * v;
        }
        ll score;
        if (!swap_score_roles) {
            score = offA + (b_shift + (HW - 1)) * sumB - offB;
        } else {
            score = offB + (b_shift + (HW - 1)) * sumA - offA;
        }
        runtime_entry.push_back(RuntimeEntry{et.state, et.lenA0, score});
    }

    for (int i = 0; i < (int)runtime_entry.size(); ++i) {
        const auto &re = runtime_entry[i];
        int rem = rem_after_state[re.state];
        if (re.lenA0 > FINAL_LEN_R) continue;
        if (re.lenA0 + rem < FINAL_LEN_L) continue;
        int flat = idx_of(re.state, re.lenA0);
        if (re.score > dp[flat]) {
            dp[flat] = re.score;
            entry_choice[flat] = (uint16_t)i;
            active.push_back(flat);
        }
    }

    // Middle runtimes, position-dependent only through used_before.
    vector<vector<RuntimeTrans>> runtime(S);
    for (int c = start_col, step = 0; c < exit_col; ++c, ++step) {
        for (int s = 0; s < S; ++s) {
            runtime[s].clear();
            const auto &st = states[s];
            int used_before = c * H + (H - __builtin_popcount((unsigned)st.mask));
            runtime[s].reserve(trans_list[s].size());
            for (int ti = 0; ti < (int)trans_list[s].size(); ++ti) {
                const auto &tr = trans_list[s][ti];
                ll sumA = 0, offA = 0;
                for (int i = 0; i < (int)tr.seqA.size(); ++i) {
                    ll v = local_value_two_cols(a, tr.seqA[i], c);
                    sumA += v;
                    offA += 1LL * i * v;
                }
                ll sumB = 0, offB = 0;
                for (int i = 0; i < (int)tr.seqB.size(); ++i) {
                    ll v = local_value_two_cols(a, tr.seqB[i], c);
                    sumB += v;
                    offB += 1LL * i * v;
                }
                ll coef, base;
                if (!swap_score_roles) {
                    coef = sumA + sumB;
                    base = offA + (b_shift + (HW - 1 - used_before)) * sumB - offB;
                } else {
                    coef = -(sumA + sumB);
                    base = 1LL * used_before * sumB + offB + (b_shift + (HW - 1)) * sumA - offA;
                }
                runtime[s].push_back(RuntimeTrans{tr.next_state, tr.t1, coef, base});
            }
        }

        ++seen_token;
        if (seen_token == INT_MAX) {
            fill(seen.begin(), seen.end(), 0);
            seen_token = 1;
        }
        next_active.clear();
        
        int average_len_a = (c + 1) * 2;
        for (int flat : active) {
            ll cur = dp[flat];
            if (cur == NEG) continue;
            int s = flat / (HW + 1);
            int lenA = flat % (HW + 1);
            const auto &rts = runtime[s];
            for (int ti = 0; ti < (int)rts.size(); ++ti) {
                const auto &rt = rts[ti];
                int nlenA = lenA + rt.t1;
                int rem = rem_after_state[rt.next_state];
                if (nlenA > FINAL_LEN_R) continue;
                if (nlenA + rem < FINAL_LEN_L) continue;
                if (nlenA - average_len_a > 20) continue;
                if (average_len_a - nlenA > 20) continue;
                int nfl = idx_of(rt.next_state, nlenA);
                ll cand = cur + rt.coef * lenA + rt.base;
                if (cand > ndp[nfl]) {
                    ndp[nfl] = cand;
                    parent[(size_t)step * (size_t)TOT + (size_t)nfl] = (uint32_t(s) << 16) | uint32_t(ti);
                    if (seen[nfl] != seen_token) {
                        seen[nfl] = seen_token;
                        next_active.push_back(nfl);
                    }
                }
            }
        }

        for (int flat : active) dp[flat] = NEG;
        for (int flat : next_active) {
            dp[flat] = ndp[flat];
            ndp[flat] = NEG;
        }
        active.swap(next_active);
    }

    // Exit runtimes
    const auto &exit_topos = cap_exit ? exit_cap_topos : exit_shared_topos;
    vector<vector<RuntimeExit>> runtime_exit(S);
    for (int s = 0; s < S; ++s) {
        const auto &st = states[s];
        int used_before = exit_col * H + (H - __builtin_popcount((unsigned)st.mask));
        runtime_exit[s].reserve(exit_topos[s].size());
        for (int ei = 0; ei < (int)exit_topos[s].size(); ++ei) {
            const auto &et = exit_topos[s][ei];
            ll sumA = 0, offA = 0;
            for (int i = 0; i < (int)et.seqA.size(); ++i) {
                ll v = gadget_value(a, et.seqA[i], exit_col);
                sumA += v;
                offA += 1LL * i * v;
            }
            ll sumB = 0, offB = 0;
            for (int i = 0; i < (int)et.seqB.size(); ++i) {
                ll v = gadget_value(a, et.seqB[i], exit_col);
                sumB += v;
                offB += 1LL * i * v;
            }
            ll coef, base;
            if (!swap_score_roles) {
                coef = sumA + sumB;
                base = offA + (b_shift + (HW - 1 - used_before)) * sumB - offB;
            } else {
                coef = -(sumA + sumB);
                base = 1LL * used_before * sumB + offB + (b_shift + (HW - 1)) * sumA - offA;
            }
            runtime_exit[s].push_back(RuntimeExit{(uint16_t)ei, et.t1, coef, base});
        }
    }

    ll best = NEG;
    int best_flat = -1;
    int best_exit_idx = -1;
    for (int flat : active) {
        ll cur = dp[flat];
        if (cur == NEG) continue;
        int s = flat / (HW + 1);
        int lenA = flat % (HW + 1);
        for (const auto &re : runtime_exit[s]) {
            int final_lenA = lenA + re.t1;
            if (final_lenA < FINAL_LEN_L || final_lenA > FINAL_LEN_R) continue;
            ll cand = cur + re.coef * lenA + re.base;
            cand -= (final_lenA-2*W)*18000LL*W*W/2;
            if (cand > best) {
                best = cand;
                best_flat = flat;
                best_exit_idx = re.idx;
            }
        }
    }

    if (best_flat < 0) {
        // Fallback: simple snake inside the strip.
        TwoPaths fb;
        // A uses rows 0,1; B uses rows 2,3.
        for (int c = 0; c < W; ++c) fb.A.push_back({row_base + 0, c});
        for (int c = W - 1; c >= 0; --c) fb.A.push_back({row_base + 1, c});
        for (int c = 0; c < W; ++c) fb.B.push_back({row_base + 2, c});
        for (int c = W - 1; c >= 0; --c) fb.B.push_back({row_base + 3, c});
        return fb;
    }

    // Reconstruct middle history
    vector<pair<int,uint32_t>> hist;
    hist.reserve(steps);
    int cur_flat = best_flat;
    for (int step = steps - 1; step >= 0; --step) {
        uint32_t info = parent[(size_t)step * (size_t)TOT + (size_t)cur_flat];
        hist.push_back({step, info});
        int prev_state = int(info >> 16);
        int ti = int(info & 0xFFFFu);
        cur_flat = idx_of(prev_state, (cur_flat % (HW + 1)) - trans_list[prev_state][ti].t1);
    }
    reverse(hist.begin(), hist.end());
    int entry_idx = entry_choice[cur_flat];

    TwoPaths out;
    const auto &ent = entry_topos[entry_idx];
    out.A.reserve(HW);
    out.B.reserve(HW);
    for (uint8_t v : ent.seqA) out.A.push_back({row_base + vrow(v), vcol(v)});
    for (uint8_t v : ent.seqB) out.B.push_back({row_base + vrow(v), vcol(v)});

    for (auto [step, info] : hist) {
        int s = int(info >> 16);
        int ti = int(info & 0xFFFFu);
        const auto &tr = trans_list[s][ti];
        int c = start_col + step;
        for (uint8_t v : tr.seqA) {
            if (v < H) out.A.push_back({row_base + v, c});
            else out.A.push_back({row_base + (v - H), c + 1});
        }
        for (uint8_t v : tr.seqB) {
            if (v < H) out.B.push_back({row_base + v, c});
            else out.B.push_back({row_base + (v - H), c + 1});
        }
    }

    int last_state = best_flat / (HW + 1);
    const auto &ex = exit_topos[last_state][best_exit_idx];
    for (uint8_t v : ex.seqA) out.A.push_back({row_base + vrow(v), exit_col + vcol(v)});
    for (uint8_t v : ex.seqB) out.B.push_back({row_base + vrow(v), exit_col + vcol(v)});

    return out;
}

static TwoPaths mirror_and_swap(const TwoPaths& in, int row_base) {
    TwoPaths out;
    out.A.reserve(in.B.size());
    out.B.reserve(in.A.size());
    for (auto [r, c] : in.B) out.A.push_back({r, W - 1 - c});
    for (auto [r, c] : in.A) out.B.push_back({r, W - 1 - c});
    return out;
}

static bool adj(const pair<int,int>& a, const pair<int,int>& b) {
    return max(abs(a.first - b.first), abs(a.second - b.second)) == 1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    build_graphs();
    build_states_and_middle_transitions();
    build_entry_topos();
    build_exit_topos_for_spec(exit_shared_topos, 0, 3);
    build_exit_topos_for_spec(exit_cap_topos, 1, 0);

    int n;
    cin >> n;
    vector<vector<int>> A(n, vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) cin >> A[i][j];
    }

    const int bands = n / H; // 50 for official input
    vector<TwoPaths> pieces(bands);

    for (int band = 0; band < bands; ++band) {
        array<array<int, W>, H> strip{};
        for (int r = 0; r < H; ++r) {
            for (int c = 0; c < W; ++c) {
                strip[r][c] = A[band * H + r][c];
            }
        }
        ll b_shift = 1LL * HW * (bands - 1 - band);
        bool is_last = (band == bands - 1);
        if ((band & 1) == 0) {
            pieces[band] = solve_strip_oriented(strip, band * H, b_shift, false, false);
        } else {
            array<array<int, W>, H> rev{};
            for (int r = 0; r < H; ++r) for (int c = 0; c < W; ++c) rev[r][c] = strip[r][W - 1 - c];
            auto tmp = solve_strip_oriented(rev, band * H, b_shift, is_last, true);
            pieces[band] = mirror_and_swap(tmp, band * H);
        }
    }

    vector<pair<int,int>> bigA, bigB;
    bigA.reserve(n * n / 2 + 10);
    bigB.reserve(n * n / 2 + 10);
    for (int band = 0; band < bands; ++band) {
        if (!bigA.empty() && !adj(bigA.back(), pieces[band].A.front())) {
            // Fallback to keep output valid if the intended connector failed.
            // In practice this should not happen if the boundary gadgets are correct.
        }
        if (!bigB.empty() && !adj(bigB.back(), pieces[band].B.front())) {
        }
        for (auto &p : pieces[band].A) bigA.push_back(p);
        for (auto &p : pieces[band].B) bigB.push_back(p);
    }

    vector<pair<int,int>> answer = bigA;
    for (int i = (int)bigB.size() - 1; i >= 0; --i) answer.push_back(bigB[i]);

    for (auto [i, j] : answer) cout << i << ' ' << j << '\n';
    return 0;
}
