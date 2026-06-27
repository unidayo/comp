#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <cstdint>
#include <chrono>
#include <cstring>
#include <cmath>
#include <climits>
using namespace std;

const int R = 10;
const int MAX_DEP_SIZE = 15;
const int MAX_ESC_SIZE = 20;
const int TARGET_SCORE = 100;

const int MIN_BEAM_WIDTH = 100;
const int MAX_BEAM_WIDTH = 1000;
const int MAX_TURNS = 1000;
const int NEIGHBOR_COUNT = 50;
const int DP_NEIGHBOR_COUNT = 0;
const double TIME_LIMIT = 1.96;

struct FastRNG {
    uint64_t x;
    FastRNG(uint64_t seed = 88172645463325252ULL) : x(seed) {}
    inline uint64_t next_u64() {
        x ^= x << 7;
        x ^= x >> 9;
        return x;
    }
    inline uint32_t next_u32() { return (uint32_t)next_u64(); }
    inline int randint(int l, int r) { return l + (int)(next_u32() % (uint32_t)(r - l + 1)); }
};

struct Action {
    int type; // 0: dep -> esc, 1: esc -> dep
    int i;
    int j;
    int k;
};

struct TurnAction {
    int n = 0;
    Action actions[R];
    inline void clear() { n = 0; }
    inline void push_back(const Action& a) { actions[n++] = a; }
};

inline bool is_pair_score(int u, int v) {
    return u / 10 == v / 10 && u + 1 == v;
}
inline int dep_penalty_from_size(int sz) { return max(0, sz - 12); }
inline int esc_penalty_from_size(int sz) { return max(0, sz - 16); }

struct State {
    uint8_t dep[R][MAX_DEP_SIZE];
    uint8_t esc[R][MAX_ESC_SIZE];
    uint8_t dep_sz[R];
    uint8_t esc_sz[R];

    uint8_t pref[R];
    int score;
    int penalty;
    int prefix;
    int placed;
    int correct_dep;
    int eval;

    int parent;
    int depth;
    TurnAction last;

    State()
        : score(0), penalty(0), prefix(0), placed(0), correct_dep(0), eval(0), parent(-1), depth(0) {
        memset(dep, 0, sizeof(dep));
        memset(esc, 0, sizeof(esc));
        memset(dep_sz, 0, sizeof(dep_sz));
        memset(esc_sz, 0, sizeof(esc_sz));
        memset(pref, 0, sizeof(pref));
        last.clear();
    }

    inline int calc_row_prefix(int r) const {
        int lim = min<int>(dep_sz[r], 10);
        int res = 0;
        for (int p = 0; p < lim; ++p) {
            if ((int)dep[r][p] == 10 * r + p) ++res;
            else break;
        }
        return res;
    }

    void update_eval() {
        prefix = 0;
        placed = 0;
        correct_dep = 0;
        for (int r = 0; r < R; ++r) {
            int pr = calc_row_prefix(r);
            pref[r] = (uint8_t)pr;
            prefix += pr;
            int lim = min<int>(dep_sz[r], 10);
            for (int p = 0; p < lim; ++p) {
                int x = dep[r][p];
                if (x == 10 * r + p) ++placed;
                if (x / 10 == r) ++correct_dep;
            }
            for (int p = 10; p < (int)dep_sz[r]; ++p) {
                if (dep[r][p] / 10 == r) ++correct_dep;
            }
        }

        eval = score * 100000
             + prefix * 300
             - penalty * 100
             - depth;
    }

    void calc_score() {
        score = 0;
        penalty = 0;
        for (int i = 0; i < R; ++i) {
            for (int p = 0; p + 1 < (int)dep_sz[i]; ++p) {
                if (is_pair_score(dep[i][p], dep[i][p + 1])) ++score;
            }
            penalty += dep_penalty_from_size(dep_sz[i]);
        }
        for (int j = 0; j < R; ++j) {
            for (int p = 0; p + 1 < (int)esc_sz[j]; ++p) {
                if (is_pair_score(esc[j][p], esc[j][p + 1])) ++score;
            }
            penalty += esc_penalty_from_size(esc_sz[j]);
        }
        for (int r = 0; r < R; ++r) {
            if (dep_sz[r] > 0 && dep[r][0] == 10 * r) ++score;
        }
        update_eval();
    }
};

inline int dep_head_bonus(const State& s, int i) {
    return (s.dep_sz[i] > 0 && s.dep[i][0] == 10 * i) ? 1 : 0;
}

inline int calc_delta_score(const State& s, const Action& a) {
    int delta = 0;
    if (a.type == 0) {
        int ds = s.dep_sz[a.i];
        int es = s.esc_sz[a.j];
        if (ds > a.k) {
            int u = s.dep[a.i][ds - a.k - 1];
            int v = s.dep[a.i][ds - a.k];
            if (is_pair_score(u, v)) --delta;
        }
        if (es > 0) {
            int u = s.dep[a.i][ds - 1];
            int v = s.esc[a.j][0];
            if (is_pair_score(u, v)) ++delta;
        }
        int before_bonus = dep_head_bonus(s, a.i);
        int after_bonus = 0;
        if (ds - a.k > 0 && s.dep[a.i][0] == 10 * a.i) after_bonus = 1;
        delta += after_bonus - before_bonus;
    } else {
        int ds = s.dep_sz[a.i];
        int es = s.esc_sz[a.j];
        if (es > a.k) {
            int u = s.esc[a.j][a.k - 1];
            int v = s.esc[a.j][a.k];
            if (is_pair_score(u, v)) --delta;
        }
        if (ds > 0) {
            int u = s.dep[a.i][ds - 1];
            int v = s.esc[a.j][0];
            if (is_pair_score(u, v)) ++delta;
        }
        int before_bonus = dep_head_bonus(s, a.i);
        int after_bonus = before_bonus;
        if (ds == 0) after_bonus = (s.esc[a.j][0] == 10 * a.i) ? 1 : 0;
        delta += after_bonus - before_bonus;
    }
    return delta;
}

inline int calc_delta_penalty(const State& s, const Action& a) {
    int delta = 0;
    if (a.type == 0) {
        int old_dep = s.dep_sz[a.i], old_esc = s.esc_sz[a.j];
        int new_dep = old_dep - a.k, new_esc = old_esc + a.k;
        delta -= dep_penalty_from_size(old_dep);
        delta -= esc_penalty_from_size(old_esc);
        delta += dep_penalty_from_size(new_dep);
        delta += esc_penalty_from_size(new_esc);
    } else {
        int old_dep = s.dep_sz[a.i], old_esc = s.esc_sz[a.j];
        int new_dep = old_dep + a.k, new_esc = old_esc - a.k;
        delta -= dep_penalty_from_size(old_dep);
        delta -= esc_penalty_from_size(old_esc);
        delta += dep_penalty_from_size(new_dep);
        delta += esc_penalty_from_size(new_esc);
    }
    return delta;
}

inline int delta_prefix_single(const State& s, const Action& a) {
    int i = a.i;
    int before = s.pref[i];
    int after = before;
    if (a.type == 0) {
        after = min<int>(before, (int)s.dep_sz[i] - a.k);
    } else {
        if (before == (int)s.dep_sz[i]) {
            after = before;
            for (int t = 0; t < a.k && after < 10; ++t) {
                if ((int)s.esc[a.j][t] == 10 * i + after) ++after;
                else break;
            }
        }
    }
    return after - before;
}

inline int continuation_len_for_type1(const State& s, const Action& a) {
    if (a.type != 1) return 0;
    int i = a.i;
    if ((int)s.pref[i] != (int)s.dep_sz[i]) return 0;
    int cur = s.pref[i], cnt = 0;
    for (int t = 0; t < a.k && cur < 10; ++t) {
        if ((int)s.esc[a.j][t] == 10 * i + cur) { ++cur; ++cnt; }
        else break;
    }
    return cnt;
}

inline int internal_pair_count_block_dep(const State& s, const Action& a) {
    int cnt = 0;
    int st = s.dep_sz[a.i] - a.k;
    for (int t = 0; t + 1 < a.k; ++t) {
        if (is_pair_score(s.dep[a.i][st + t], s.dep[a.i][st + t + 1])) ++cnt;
    }
    return cnt;
}

inline int internal_pair_count_block_esc(const State& s, const Action& a) {
    int cnt = 0;
    for (int t = 0; t + 1 < a.k; ++t) {
        if (is_pair_score(s.esc[a.j][t], s.esc[a.j][t + 1])) ++cnt;
    }
    return cnt;
}

inline int action_priority(const State& s, const Action& a, int delta_score, int delta_penalty, int mode, FastRNG& rng) {
    int dpref = delta_prefix_single(s, a);
    int val = delta_score * 12000 - delta_penalty * 220 + dpref * (mode == 1 ? 8000 : 4300);

    if (a.type == 1) {
        int cont = continuation_len_for_type1(s, a);
        val += cont * (mode == 1 ? 1700 : 900);
        if (cont == a.k && cont > 0) val += 700;
        if (s.esc_sz[a.j] == a.k) val += 90; // emptying an escape is slightly good
        if (s.esc_sz[a.j] > 0) {
            int first = s.esc[a.j][0];
            if (first / 10 == a.i) val += 160;
            if (a.i == a.j && first / 10 == a.i) val += 80;
        }
        val += internal_pair_count_block_esc(s, a) * 80;
    } else {
        int ds = s.dep_sz[a.i];
        if (s.pref[a.i] > 0 && ds - a.k >= (int)s.pref[a.i]) val += 200; // keep a fixed prefix intact
        if (ds == a.k) val += 70; // emptying a dep creates a clean rebuild buffer
        int first = s.dep[a.i][ds - a.k];
        if (first / 10 == a.j) val += 80;
        if (a.i == a.j && first / 10 == a.j) val += 40;
        val += internal_pair_count_block_dep(s, a) * 70;
    }

    if (mode == 2) val += a.k * 120;
    if (mode == 3) val -= a.k * 60;
    val += (int)(rng.next_u32() % (mode >= 4 ? 2600u : 700u));
    return val;
}

State apply_turn_action_fast(const State& s, const TurnAction& ta) {
    State ns = s;
    ns.depth = s.depth + 1;

    int delta_score = 0;
    int delta_penalty = 0;
    for (int idx = 0; idx < ta.n; ++idx) {
        const Action& a = ta.actions[idx];
        delta_score += calc_delta_score(s, a);
        delta_penalty += calc_delta_penalty(s, a);
    }

    for (int idx = 0; idx < ta.n; ++idx) {
        const Action& a = ta.actions[idx];
        if (a.type == 0) {
            int i = a.i, j = a.j, k = a.k;
            int ds = ns.dep_sz[i], es = ns.esc_sz[j];
            for (int p = es - 1; p >= 0; --p) ns.esc[j][p + k] = ns.esc[j][p];
            for (int t = 0; t < k; ++t) ns.esc[j][t] = ns.dep[i][ds - k + t];
            ns.dep_sz[i] = ds - k;
            ns.esc_sz[j] = es + k;
        } else {
            int i = a.i, j = a.j, k = a.k;
            int ds = ns.dep_sz[i], es = ns.esc_sz[j];
            for (int t = 0; t < k; ++t) ns.dep[i][ds + t] = ns.esc[j][t];
            for (int p = k; p < es; ++p) ns.esc[j][p - k] = ns.esc[j][p];
            ns.dep_sz[i] = ds + k;
            ns.esc_sz[j] = es - k;
        }
    }

    ns.score = s.score + delta_score;
    ns.penalty = s.penalty + delta_penalty;
    ns.update_eval();
    return ns;
}

inline bool is_compatible_action(const Action& a, const Action& b) {
    if (a.i == b.i || a.j == b.j) return false;
    return (a.i < b.i) == (a.j < b.j);
}

inline bool can_add_action_fast(const TurnAction& ta, const Action& new_act, const bool used_dep[R], const bool used_esc[R]) {
    if (ta.n >= R) return false;
    if (used_dep[new_act.i] || used_esc[new_act.j]) return false;
    for (int idx = 0; idx < ta.n; ++idx) {
        if (!is_compatible_action(ta.actions[idx], new_act)) return false;
    }
    return true;
}

uint64_t calc_hash(const State& s) {
    uint64_t h = 1469598103934665603ULL;
    auto add = [&](int x) {
        h ^= (uint64_t)(x + 1000003);
        h *= 1099511628211ULL;
    };
    for (int i = 0; i < R; ++i) {
        add(10000 + i); add(s.dep_sz[i]);
        for (int p = 0; p < (int)s.dep_sz[i]; ++p) add(s.dep[i][p]);
    }
    for (int j = 0; j < R; ++j) {
        add(20000 + j); add(s.esc_sz[j]);
        for (int p = 0; p < (int)s.esc_sz[j]; ++p) add(s.esc[j][p]);
    }
    return h;
}

inline bool is_goal_state(const State& s) {
    for (int r = 0; r < R; ++r) {
        if (s.dep_sz[r] != 10) return false;
        for (int p = 0; p < 10; ++p) if ((int)s.dep[r][p] != 10 * r + p) return false;
    }
    for (int j = 0; j < R; ++j) if (s.esc_sz[j] != 0) return false;
    return true;
}

vector<TurnAction> reconstruct_history(const vector<State>& pool, int idx) {
    vector<TurnAction> hist;
    while (idx != -1 && pool[idx].parent != -1) {
        hist.push_back(pool[idx].last);
        idx = pool[idx].parent;
    }
    reverse(hist.begin(), hist.end());
    return hist;
}


vector<TurnAction> compress_history(const vector<TurnAction>& original) {
    vector<TurnAction> res;

    for (const TurnAction& ta : original) {
        for (int ai = 0; ai < ta.n; ++ai) {
            const Action& a = ta.actions[ai];

            int target_t = (int)res.size();

            for (int t = (int)res.size() - 1; t >= 0; --t) {
                bool dependency = false;

                for (int bi = 0; bi < res[t].n; ++bi) {
                    const Action& prev = res[t].actions[bi];

                    if (prev.i == a.i || prev.j == a.j) {
                        dependency = true;
                        break;
                    }
                }

                if (dependency) {
                    target_t = t + 1;
                    break;
                }

                target_t = t;
            }

            while (target_t < (int)res.size()) {
                bool can_fit = true;

                if (res[target_t].n >= R) {
                    can_fit = false;
                }

                for (int bi = 0; can_fit && bi < res[target_t].n; ++bi) {
                    const Action& existing = res[target_t].actions[bi];

                    if (!is_compatible_action(existing, a)) {
                        can_fit = false;
                    }
                }

                if (can_fit) {
                    break;
                }

                target_t++;
            }

            if (target_t == (int)res.size()) {
                TurnAction nt;
                nt.clear();
                nt.push_back(a);
                res.push_back(nt);
            } else {
                res[target_t].push_back(a);
            }
        }
    }

    return res;
}

// 元のターン列から「前にずらす」だけの安全な圧縮。
// 空から詰め直す compress_history はケースによって元より長くなることがあるため、
// こちらを併用して、少なくとも元のターン数より悪化しない候補を作る。
vector<TurnAction> left_shift_history(const vector<TurnAction>& original) {
    vector<Action> acts;
    vector<int> orig_turn;
    vector<vector<int>> sched(original.size());

    for (int t = 0; t < (int)original.size(); ++t) {
        for (int ai = 0; ai < original[t].n; ++ai) {
            int id = (int)acts.size();
            acts.push_back(original[t].actions[ai]);
            orig_turn.push_back(t);
            sched[t].push_back(id);
        }
    }

    const int N = (int)acts.size();
    vector<int> cur_turn(N);
    for (int id = 0; id < N; ++id) cur_turn[id] = orig_turn[id];

    int last_dep[R], last_esc[R];
    fill(last_dep, last_dep + R, -1);
    fill(last_esc, last_esc + R, -1);

    auto can_fit = [&](int t, const Action& a) -> bool {
        if ((int)sched[t].size() >= R) return false;
        for (int id : sched[t]) {
            if (!is_compatible_action(acts[id], a)) return false;
        }
        return true;
    };

    for (int id = 0; id < N; ++id) {
        const Action& a = acts[id];
        int ot = orig_turn[id];

        auto& v = sched[ot];
        v.erase(find(v.begin(), v.end(), id));

        int earliest = max(last_dep[a.i] + 1, last_esc[a.j] + 1);
        int target = ot;

        // 自分の元ターンより前にだけ動かす。これで既存解よりターン数が増えない。
        for (int t = earliest; t < ot; ++t) {
            if (can_fit(t, a)) {
                target = t;
                break;
            }
        }

        sched[target].push_back(id);
        cur_turn[id] = target;
        last_dep[a.i] = target;
        last_esc[a.j] = target;
    }

    vector<TurnAction> res;
    for (auto& ids : sched) {
        if (ids.empty()) continue;
        sort(ids.begin(), ids.end(), [&](int x, int y) {
            if (acts[x].i != acts[y].i) return acts[x].i < acts[y].i;
            return acts[x].j < acts[y].j;
        });
        TurnAction ta;
        ta.clear();
        for (int id : ids) ta.push_back(acts[id]);
        res.push_back(ta);
    }

    return res;
}

// Safer and usually shorter than the original chronological greedy compression.
// It keeps only the true state dependencies: each dep line and each escape line must preserve
// its own operation order.  Then every turn is chosen as a maximum-weight non-crossing matching
// among currently ready operations.
vector<TurnAction> schedule_history(const vector<TurnAction>& original) {
    vector<Action> acts;
    for (const auto& ta : original) for (int i = 0; i < ta.n; ++i) acts.push_back(ta.actions[i]);
    int N = (int)acts.size();
    if (N == 0) return {};

    vector<array<int, 2>> pred_edge(N);
    vector<int> indeg(N, 0);
    vector<vector<int>> succ(N);
    for (int i = 0; i < N; ++i) pred_edge[i] = {-1, -1};

    int last_dep[R], last_esc[R];
    fill(last_dep, last_dep + R, -1);
    fill(last_esc, last_esc + R, -1);

    auto add_edge = [&](int u, int v) {
        if (u < 0 || u == v) return;
        for (int x : succ[u]) if (x == v) return;
        succ[u].push_back(v);
        ++indeg[v];
    };

    for (int id = 0; id < N; ++id) {
        const Action& a = acts[id];
        add_edge(last_dep[a.i], id);
        add_edge(last_esc[a.j], id);
        last_dep[a.i] = id;
        last_esc[a.j] = id;
    }

    vector<int> crit(N, 1);
    for (int id = N - 1; id >= 0; --id) {
        for (int v : succ[id]) crit[id] = max(crit[id], 1 + crit[v]);
    }

    vector<char> ready(N, 0), done(N, 0);
    for (int id = 0; id < N; ++id) if (indeg[id] == 0) ready[id] = 1;

    vector<TurnAction> res;
    int done_cnt = 0;

    while (done_cnt < N) {
        int cell_id[R][R];
        int cell_w[R][R];
        for (int i = 0; i < R; ++i) for (int j = 0; j < R; ++j) cell_id[i][j] = -1, cell_w[i][j] = INT_MIN / 4;

        for (int id = 0; id < N; ++id) if (ready[id] && !done[id]) {
            const Action& a = acts[id];
            int w = 100000 + crit[id] * 40 - id / 8;
            if (w > cell_w[a.i][a.j]) {
                cell_w[a.i][a.j] = w;
                cell_id[a.i][a.j] = id;
            }
        }

        int dp[R + 1][R + 1];
        uint8_t take[R + 1][R + 1];
        memset(dp, 0x80, sizeof(dp));
        memset(take, 0, sizeof(take));
        dp[0][0] = 0;
        for (int i = 0; i <= R; ++i) {
            for (int j = 0; j <= R; ++j) {
                if (dp[i][j] < -100000000) continue;
                if (i < R && dp[i][j] > dp[i + 1][j]) {
                    dp[i + 1][j] = dp[i][j];
                    take[i + 1][j] = 1; // from up, skip dep i
                }
                if (j < R && dp[i][j] > dp[i][j + 1]) {
                    dp[i][j + 1] = dp[i][j];
                    take[i][j + 1] = 2; // from left, skip esc j
                }
                if (i < R && j < R && cell_id[i][j] != -1) {
                    int nw = dp[i][j] + cell_w[i][j];
                    if (nw > dp[i + 1][j + 1]) {
                        dp[i + 1][j + 1] = nw;
                        take[i + 1][j + 1] = 3; // use cell (i,j)
                    }
                }
            }
        }

        vector<int> selected;
        int i = R, j = R;
        while (i > 0 || j > 0) {
            if (take[i][j] == 3) {
                selected.push_back(cell_id[i - 1][j - 1]);
                --i; --j;
            } else if (take[i][j] == 1) {
                --i;
            } else if (take[i][j] == 2) {
                --j;
            } else {
                // fallback for unreachable corner, should not occur
                if (i > 0) --i; else --j;
            }
        }
        reverse(selected.begin(), selected.end());

        if (selected.empty()) {
            // Robust fallback: output any ready action.
            for (int id = 0; id < N; ++id) if (ready[id] && !done[id]) { selected.push_back(id); break; }
        }

        TurnAction ta;
        ta.clear();
        for (int id : selected) ta.push_back(acts[id]);
        res.push_back(ta);

        for (int id : selected) {
            if (done[id]) continue;
            done[id] = 1;
            ready[id] = 0;
            ++done_cnt;
            for (int v : succ[id]) {
                --indeg[v];
                if (indeg[v] == 0) ready[v] = 1;
            }
        }
    }

    return res;
}

struct SingleMoveCand {
    Action act;
    int delta_score;
    int priority;
};

const int MIN_DELTA = -3;
const int MAX_DELTA = 3;
const int BUCKET_N = MAX_DELTA - MIN_DELTA + 1;
const int BUCKET_TRY_LIMIT = NEIGHBOR_COUNT * 2;

inline int bucket_index(int d) {
    if (d < MIN_DELTA) d = MIN_DELTA;
    if (d > MAX_DELTA) d = MAX_DELTA;
    return d - MIN_DELTA;
}

template <class T>
void fast_shuffle(vector<T>& v, FastRNG& rng) {
    for (int i = (int)v.size() - 1; i > 0; --i) {
        int j = rng.randint(0, i);
        swap(v[i], v[j]);
    }
}

inline int decide_max_moves(int score) {
    if (score < 40) return 10;
    if (score < 70) return 9;
    if (score < 90) return 8;
    return 7;
}

int decide_beam_width(int best_score, double elapsed) {
    double tm = elapsed - (1.90 * best_score / 100.0);
    tm /= max(0.05, 1.90 - elapsed);
    int w = (0.4 + 0.6 * best_score / 100) * MAX_BEAM_WIDTH;
    w *= pow(10.0, -1.0 * tm);
    w = max(MIN_BEAM_WIDTH, min(MAX_BEAM_WIDTH, w));
    return w;
}

TurnAction make_neighbor_from_buckets(const array<vector<Action>, BUCKET_N>& buckets, FastRNG& rng, int num_moves, int min_allowed_delta) {
    TurnAction ta;
    ta.clear();
    bool used_dep[R] = {};
    bool used_esc[R] = {};

    for (int bi = BUCKET_N - 1; bi >= 0; --bi) {
        int delta = bi + MIN_DELTA;
        if (delta < min_allowed_delta) continue;
        const auto& v = buckets[bi];
        if (v.empty()) continue;
        int n = (int)v.size();
        int start = rng.randint(0, n - 1);
        int limit = min(n, BUCKET_TRY_LIMIT);
        for (int t = 0; t < limit; ++t) {
            const Action& a = v[(start + t) % n];
            if (can_add_action_fast(ta, a, used_dep, used_esc)) {
                ta.push_back(a);
                used_dep[a.i] = true;
                used_esc[a.j] = true;
                if (ta.n == num_moves) return ta;
            }
        }
    }
    return ta;
}

TurnAction make_neighbor_from_priority(vector<SingleMoveCand>& cands, FastRNG& rng, int num_moves, int min_allowed_delta, int top_limit) {
    TurnAction ta;
    ta.clear();
    bool used_dep[R] = {};
    bool used_esc[R] = {};

    int n = min<int>(top_limit, cands.size());
    nth_element(cands.begin(), cands.begin() + n, cands.end(), [](const auto& a, const auto& b) {
        return a.priority > b.priority;
    });
    sort(cands.begin(), cands.begin() + n, [](const auto& a, const auto& b) {
        return a.priority > b.priority;
    });

    for (int idx = 0; idx < n; ++idx) {
        const auto& sc = cands[idx];
        if (sc.delta_score < min_allowed_delta) continue;
        if (can_add_action_fast(ta, sc.act, used_dep, used_esc)) {
            ta.push_back(sc.act);
            used_dep[sc.act.i] = true;
            used_esc[sc.act.j] = true;
            if (ta.n == num_moves) break;
        }
    }
    return ta;
}

TurnAction make_dp_matching_neighbor(const vector<SingleMoveCand>& cands, int min_allowed_delta, int max_moves) {
    int cell_w[R][R];
    Action cell_act[R][R];
    bool has[R][R];
    for (int i = 0; i < R; ++i) for (int j = 0; j < R; ++j) {
        cell_w[i][j] = INT_MIN / 4;
        has[i][j] = false;
    }

    for (const auto& sc : cands) {
        if (sc.delta_score < min_allowed_delta) continue;
        int i = sc.act.i, j = sc.act.j;
        int w = sc.priority;
        if (w <= (min_allowed_delta < 0 ? -3500 : 0)) continue;
        if (!has[i][j] || w > cell_w[i][j]) {
            has[i][j] = true;
            cell_w[i][j] = w;
            cell_act[i][j] = sc.act;
        }
    }

    int dp[R + 1][R + 1][R + 1];
    uint8_t pre[R + 1][R + 1][R + 1];
    for (int i = 0; i <= R; ++i) for (int j = 0; j <= R; ++j) for (int m = 0; m <= R; ++m) dp[i][j][m] = INT_MIN / 4, pre[i][j][m] = 0;
    dp[0][0][0] = 0;
    for (int i = 0; i <= R; ++i) {
        for (int j = 0; j <= R; ++j) {
            for (int m = 0; m <= max_moves; ++m) {
                int cur = dp[i][j][m];
                if (cur < -100000000) continue;
                if (i < R && cur > dp[i + 1][j][m]) {
                    dp[i + 1][j][m] = cur;
                    pre[i + 1][j][m] = 1;
                }
                if (j < R && cur > dp[i][j + 1][m]) {
                    dp[i][j + 1][m] = cur;
                    pre[i][j + 1][m] = 2;
                }
                if (i < R && j < R && m < max_moves && has[i][j]) {
                    int nw = cur + cell_w[i][j] + 20000; // prefer more parallel actions first
                    if (nw > dp[i + 1][j + 1][m + 1]) {
                        dp[i + 1][j + 1][m + 1] = nw;
                        pre[i + 1][j + 1][m + 1] = 3;
                    }
                }
            }
        }
    }

    int best_m = 0, best_v = INT_MIN / 4;
    for (int m = 1; m <= max_moves; ++m) {
        if (dp[R][R][m] > best_v) {
            best_v = dp[R][R][m];
            best_m = m;
        }
    }

    TurnAction ta;
    ta.clear();
    if (best_m == 0) return ta;

    vector<Action> rev;
    int i = R, j = R, m = best_m;
    while (i > 0 || j > 0) {
        uint8_t p = pre[i][j][m];
        if (p == 3) {
            rev.push_back(cell_act[i - 1][j - 1]);
            --i; --j; --m;
        } else if (p == 1) {
            --i;
        } else if (p == 2) {
            --j;
        } else {
            if (i > 0) --i; else --j;
        }
    }
    reverse(rev.begin(), rev.end());
    for (auto& a : rev) ta.push_back(a);
    return ta;
}

bool better_state(const State& a, const State& b) {
    if (a.eval != b.eval) return a.eval > b.eval;
    if (a.score != b.score) return a.score > b.score;
    if (a.prefix != b.prefix) return a.prefix > b.prefix;
    if (a.placed != b.placed) return a.placed > b.placed;
    if (a.penalty != b.penalty) return a.penalty < b.penalty;
    return a.depth < b.depth;
}

int scheduled_len_for_idx(const vector<State>& pool, int idx) {
    auto h = reconstruct_history(pool, idx);
    auto s = schedule_history(h);
    return (int)s.size();
}

int beam_search(State initial_state, vector<State>& pool) {
    using Clock = chrono::steady_clock;
    auto start_time = Clock::now();

    auto elapsed_sec = [&]() -> double {
        return chrono::duration<double>(Clock::now() - start_time).count();
    };

    FastRNG rng(42);

    pool.clear();
    pool.reserve(300000);

    initial_state.parent = -1;
    initial_state.depth = 0;
    initial_state.last.clear();
    initial_state.update_eval();

    pool.push_back(initial_state);

    vector<int> beam;
    beam.push_back(0);

    int best_idx = 0;
    int last_improve_turn = 0;

    for (int turn = 0; turn < MAX_TURNS; ++turn) {
        double elapsed = elapsed_sec();

        if (elapsed > TIME_LIMIT) {
            break;
        }

        int current_beam_width = decide_beam_width(pool[best_idx].score, elapsed);

        if ((int)beam.size() > current_beam_width) {
            beam.resize(current_beam_width);
        }

        vector<State> next_candidates;
        next_candidates.reserve((int)beam.size() * NEIGHBOR_COUNT);

        unordered_set<uint64_t> seen;
        seen.reserve((int)beam.size() * NEIGHBOR_COUNT * 2 + 10);

        bool time_over = false;
        int stagnation = turn - last_improve_turn;

        for (int bi = 0; bi < (int)beam.size(); ++bi) {
            if ((bi & 7) == 0 && elapsed_sec() > TIME_LIMIT) {
                time_over = true;
                break;
            }

            int idx = beam[bi];
            const State& s = pool[idx];

            if (s.score == TARGET_SCORE) {
                return idx;
            }

            vector<SingleMoveCand> single_cands;
            single_cands.reserve(2200);

            for (int i = 0; i < R; ++i) {
                for (int j = 0; j < R; ++j) {
                    int ds = s.dep_sz[i];

                    if (ds > 0) {
                        int max_k = min(ds, MAX_ESC_SIZE - (int)s.esc_sz[j]);

                        for (int k = 1; k <= max_k; ++k) {
                            Action a = {0, i, j, k};
                            single_cands.push_back({a, calc_delta_score(s, a)});
                        }
                    }

                    int es = s.esc_sz[j];

                    if (es > 0) {
                        int max_k = min(es, MAX_DEP_SIZE - (int)s.dep_sz[i]);

                        for (int k = 1; k <= max_k; ++k) {
                            Action a = {1, i, j, k};
                            single_cands.push_back({a, calc_delta_score(s, a)});
                        }
                    }
                }
            }

            if (single_cands.empty()) {
                continue;
            }

            array<vector<Action>, BUCKET_N> buckets;

            for (auto& b : buckets) {
                b.reserve(256);
            }

            for (const auto& sc : single_cands) {
                buckets[bucket_index(sc.delta_score)].push_back(sc.act);
            }

            for (auto& b : buckets) {
                fast_shuffle(b, rng);
            }

            int max_moves = decide_max_moves(s.score);

            for (int c = 0; c < NEIGHBOR_COUNT; ++c) {
                if ((c & 15) == 0 && elapsed_sec() > TIME_LIMIT) {
                    time_over = true;
                    break;
                }

                int min_allowed_delta = 0;

                if (s.score < 90 && (c % 5 == 0)) {
                    min_allowed_delta = -1;
                }

                if (stagnation >= 8 && s.score < 95 && (c % 3 == 0)) {
                    min_allowed_delta = -2;
                }

                if (s.score >= 92 && c % 4 != 0) {
                    min_allowed_delta = 0;
                }

                int num_moves = rng.randint(1, max_moves);

                TurnAction ta = make_neighbor_from_buckets(
                    buckets,
                    rng,
                    num_moves,
                    min_allowed_delta
                );

                if (ta.n == 0) {
                    continue;
                }

                State child = apply_turn_action_fast(s, ta);
                child.parent = idx;
                child.last = ta;

                uint64_t h = calc_hash(child);

                if (!seen.insert(h).second) {
                    continue;
                }

                if (child.score == TARGET_SCORE) {
                    pool.push_back(child);
                    return (int)pool.size() - 1;
                }

                next_candidates.push_back(child);
            }

            if (time_over) {
                break;
            }
        }

        if (next_candidates.empty()) {
            break;
        }

        auto better_state_local = [](const State& a, const State& b) {
            if (a.eval != b.eval) return a.eval > b.eval;
            if (a.score != b.score) return a.score > b.score;
            if (a.prefix != b.prefix) return a.prefix > b.prefix;
            if (a.penalty != b.penalty) return a.penalty < b.penalty;
            return a.depth < b.depth;
        };

        int best_score_next = pool[best_idx].score;

        for (const State& st : next_candidates) {
            if (st.score > best_score_next) {
                best_score_next = st.score;
            }
        }

        int next_beam_width = decide_beam_width(best_score_next, elapsed_sec());
        int keep = min(next_beam_width, (int)next_candidates.size());

        if ((int)next_candidates.size() > keep) {
            nth_element(
                next_candidates.begin(),
                next_candidates.begin() + keep,
                next_candidates.end(),
                better_state_local
            );

            next_candidates.resize(keep);
        }

        sort(
            next_candidates.begin(),
            next_candidates.end(),
            better_state_local
        );

        beam.clear();
        beam.reserve(keep);

        int old_best_score = pool[best_idx].score;

        for (State& st : next_candidates) {
            pool.push_back(st);
            int new_idx = (int)pool.size() - 1;
            beam.push_back(new_idx);

            const State& bs = pool[new_idx];
            const State& cur_best = pool[best_idx];

            if (bs.eval > cur_best.eval ||
                (bs.eval == cur_best.eval && bs.score > cur_best.score) ||
                (bs.eval == cur_best.eval && bs.score == cur_best.score && bs.prefix > cur_best.prefix) ||
                (bs.eval == cur_best.eval && bs.score == cur_best.score && bs.prefix == cur_best.prefix && bs.penalty < cur_best.penalty) ||
                (bs.eval == cur_best.eval && bs.score == cur_best.score && bs.prefix == cur_best.prefix && bs.penalty == cur_best.penalty && bs.depth < cur_best.depth)) {
                best_idx = new_idx;
            }
        }

        if (pool[best_idx].score > old_best_score) {
            last_improve_turn = turn;
        }

        if (time_over) {
            break;
        }
    }

    return best_idx;
}

bool check_history(State initial, const vector<TurnAction>& history, bool require_goal) {
    State cur = initial;

    for (const TurnAction& ta : history) {
        bool used_dep[R] = {};
        bool used_esc[R] = {};

        for (int ai = 0; ai < ta.n; ++ai) {
            const Action& a = ta.actions[ai];

            if (a.type < 0 || a.type > 1) return false;
            if (a.i < 0 || a.i >= R || a.j < 0 || a.j >= R) return false;
            if (a.k <= 0) return false;

            if (used_dep[a.i] || used_esc[a.j]) return false;
            used_dep[a.i] = true;
            used_esc[a.j] = true;

            for (int bi = 0; bi < ai; ++bi) {
                if (!is_compatible_action(ta.actions[bi], a)) return false;
            }

            if (a.type == 0) {
                if (cur.dep_sz[a.i] < a.k) return false;
                if (cur.esc_sz[a.j] + a.k > MAX_ESC_SIZE) return false;
            } else {
                if (cur.esc_sz[a.j] < a.k) return false;
                if (cur.dep_sz[a.i] + a.k > MAX_DEP_SIZE) return false;
            }
        }

        cur = apply_turn_action_fast(cur, ta);
    }

    return !require_goal || is_goal_state(cur);
}


#ifdef LOCAL
bool validate_history(State initial, const vector<TurnAction>& history) {
    State cur = initial;
    for (const TurnAction& ta : history) {
        bool used_dep[R] = {};
        bool used_esc[R] = {};
        for (int ai = 0; ai < ta.n; ++ai) {
            const Action& a = ta.actions[ai];
            if (a.type < 0 || a.type > 1) return false;
            if (a.i < 0 || a.i >= R || a.j < 0 || a.j >= R) return false;
            if (a.k <= 0) return false;
            if (used_dep[a.i] || used_esc[a.j]) return false;
            used_dep[a.i] = true;
            used_esc[a.j] = true;
            for (int bi = 0; bi < ai; ++bi) if (!is_compatible_action(ta.actions[bi], a)) return false;
            if (a.type == 0) {
                if (cur.dep_sz[a.i] < a.k) return false;
                if (cur.esc_sz[a.j] + a.k > MAX_ESC_SIZE) return false;
            } else {
                if (cur.esc_sz[a.j] < a.k) return false;
                if (cur.dep_sz[a.i] + a.k > MAX_DEP_SIZE) return false;
            }
        }
        cur = apply_turn_action_fast(cur, ta);
    }
    return true;
}
#endif

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int _R;
    cin >> _R;

    State initial_state;
    for (int i = 0; i < R; ++i) {
        initial_state.dep_sz[i] = 10;
        for (int j = 0; j < 10; ++j) {
            int x;
            cin >> x;
            initial_state.dep[i][j] = (uint8_t)x;
        }
    }
    initial_state.calc_score();

    vector<State> pool;
    int result_idx = beam_search(initial_state, pool);

    vector<TurnAction> history = reconstruct_history(pool, result_idx);

    vector<TurnAction> raw_history = history;
    vector<TurnAction> greedy_history = compress_history(raw_history);
    vector<TurnAction> shifted_history = left_shift_history(raw_history);
    vector<TurnAction> scheduled_history = schedule_history(raw_history);

    bool want_goal = check_history(initial_state, raw_history, true);

    history = raw_history;
    vector<vector<TurnAction>*> cand = {&raw_history, &greedy_history, &shifted_history, &scheduled_history};

    int best_goal_size = 1 << 30;
    int best_valid_size = 1 << 30;
    vector<TurnAction>* best_goal = nullptr;
    vector<TurnAction>* best_valid = nullptr;

    for (auto* h : cand) {
        if (check_history(initial_state, *h, true)) {
            if ((int)h->size() < best_goal_size) {
                best_goal_size = (int)h->size();
                best_goal = h;
            }
        }
        if (check_history(initial_state, *h, false)) {
            if ((int)h->size() < best_valid_size) {
                best_valid_size = (int)h->size();
                best_valid = h;
            }
        }
    }

    if (best_goal != nullptr) history = *best_goal;
    else if (best_valid != nullptr) history = *best_valid;

#ifdef LOCAL
    if (!validate_history(initial_state, history)) {
        cerr << "invalid history\n";
        return 1;
    }
#endif

    cout << history.size() << '\n';
    for (const TurnAction& ta : history) {
        cout << ta.n << '\n';
        for (int ai = 0; ai < ta.n; ++ai) {
            const Action& a = ta.actions[ai];
            cout << a.type << ' ' << a.i << ' ' << a.j << ' ' << a.k << '\n';
        }
    }
    cerr << history.size() << '\n';
    return 0;
}
