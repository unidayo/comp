#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <unordered_set>
#include <iostream>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <vector>

using namespace std;

constexpr int R = 10;
constexpr int N = 100;
constexpr int DEPARTURE_CAPACITY = 15;
constexpr int SIDING_CAPACITY = 20;
constexpr int MAX_TURNS = 4000;
constexpr int MAX_BLOCK = 10;
constexpr int UNCONNECTED_PENALTY = 1500;
constexpr int ONE_SIDE_EXPOSED_PENALTY = 1400;
constexpr int REACH_PENALTY = 800;
constexpr int HORIZONTAL_WEIGHT = 3;
constexpr long long TIME_BUDGET_MS = 2000;

struct Loc {
    int kind; // 0: dep, 1: sid
    int line;
    int idx;
};

struct Move {
    int type;
    int i;
    int j;
    int k;
};

struct Candidate {
    Move move;
    int gain;
};

vector<int> dep[R], sid[R];
Loc pos[N];
int pair_penalties[N];
int top_penalties[R];
int total_penalty;
vector<vector<Move>> turns;
int initial_dep[R][10];

constexpr int MAX_LINE_LEN = 25;
constexpr int POS_INDEX_RANGE = 2 * R * MAX_LINE_LEN;
uint64_t zobrist[N][POS_INDEX_RANGE];
uint64_t state_zobrist;

inline int pos_index(const Loc& l) {
    return l.kind * (R * MAX_LINE_LEN) + l.line * MAX_LINE_LEN + l.idx;
}

void init_zobrist() {
    mt19937_64 zr(987654321ULL);
    for (int c = 0; c < N; c++)
        for (int p = 0; p < POS_INDEX_RANGE; p++)
            zobrist[c][p] = zr();
}

mt19937 rng(12345);

void input_data() {
    int r;
    cin >> r;
    for (int i = 0; i < R; i++) {
        dep[i].clear();
        for (int c = 0; c < 10; c++) {
            int v;
            cin >> v;
            dep[i].push_back(v);
            initial_dep[i][c] = v;
        }
    }
    for (int i = 0; i < R; i++) sid[i].clear();
}

void reset_state() {
    for (int i = 0; i < R; i++) {
        dep[i].assign(initial_dep[i], initial_dep[i] + 10);
        sid[i].clear();
    }
}

int top_penalty(int r) {
    int car = r * 10;
    Loc b = pos[car];
    if (b.kind == 0 && b.line == r && b.idx == 0) return 0;
    bool slot_ready = dep[r].empty();
    bool car_exposed = b.kind == 1 && b.idx == 0;
    int base_penalty;
    if (slot_ready && car_exposed) base_penalty = REACH_PENALTY;
    else if (slot_ready || car_exposed) base_penalty = ONE_SIDE_EXPOSED_PENALTY;
    else base_penalty = UNCONNECTED_PENALTY;
    int vertical = (b.kind == 0) ? 30 : 50;
    int horizontal = abs(r - b.line);
    return base_penalty + vertical + HORIZONTAL_WEIGHT * horizontal;
}

int pair_penalty(int a_car, int b_car) {
    Loc a = pos[a_car];
    Loc b = pos[b_car];
    if (a.kind == b.kind && a.line == b.line && a.idx + 1 == b.idx) return 0;
    bool a_exposed = a.kind == 0 && a.idx == (int)dep[a.line].size() - 1;
    bool b_exposed = b.kind == 1 && b.idx == 0;
    int base_penalty;
    if (a_exposed && b_exposed) base_penalty = REACH_PENALTY;
    else if (a_exposed || b_exposed) base_penalty = ONE_SIDE_EXPOSED_PENALTY;
    else base_penalty = UNCONNECTED_PENALTY;
    int vertical = 30;
    if (a.kind == b.kind && a.line == b.line) {
        vertical = a.idx < b.idx ? 60 : 800;
    }
    int horizontal = abs(a.line - b.line);
    return base_penalty + vertical + HORIZONTAL_WEIGHT * horizontal;
}

void initialize_caches() {
    for (int i = 0; i < R; i++) {
        for (int p = 0; p < (int)dep[i].size(); p++) pos[dep[i][p]] = {0, i, p};
        for (int p = 0; p < (int)sid[i].size(); p++) pos[sid[i][p]] = {1, i, p};
    }
    total_penalty = 0;
    for (int r = 0; r < R; r++) {
        top_penalties[r] = top_penalty(r);
        total_penalty += top_penalties[r];
    }
    for (int a = 0; a < N; a++) {
        pair_penalties[a] = (a % 10 != 9) ? pair_penalty(a, a + 1) : 0;
        total_penalty += pair_penalties[a];
    }
    state_zobrist = 0;
    for (int c = 0; c < N; c++) state_zobrist ^= zobrist[c][pos_index(pos[c])];
}

void rebuild_pos_for_line(int kind, int line) {
    auto& arr = (kind == 0) ? dep[line] : sid[line];
    for (int idx = 0; idx < (int)arr.size(); idx++) {
        pos[arr[idx]] = {kind, line, idx};
    }
}

vector<int> affected_cars_for_move(const Move& m) {
    vector<int> result;
    result.reserve(m.k + 2);
    if (m.type == 0) {
        int count = (int)dep[m.i].size();
        for (int idx = count - m.k; idx < count; idx++) result.push_back(dep[m.i][idx]);
        int new_tail = count - m.k - 1;
        if (new_tail >= 0) result.push_back(dep[m.i][new_tail]);
        if (!sid[m.j].empty()) result.push_back(sid[m.j][0]);
    } else {
        for (int idx = 0; idx < m.k; idx++) result.push_back(sid[m.j][idx]);
        if (!dep[m.i].empty()) result.push_back(dep[m.i].back());
        if ((int)sid[m.j].size() > m.k) result.push_back(sid[m.j][m.k]);
    }
    return result;
}

void apply_move_and_update(const Move& m) {
    auto cars = affected_cars_for_move(m);
    set<int> affected_pairs, affected_tops;
    affected_tops.insert(m.i);
    for (int c : cars) {
        if (c % 10 != 9) affected_pairs.insert(c);
        if (c % 10 != 0) affected_pairs.insert(c - 1);
        affected_tops.insert(c / 10);
    }
    for (int a : affected_pairs) total_penalty -= pair_penalties[a];
    for (int r : affected_tops) total_penalty -= top_penalties[r];

    if (m.type == 0) {
        int count = (int)dep[m.i].size();
        for (int idx = count - m.k; idx < count; idx++) {
            int c = dep[m.i][idx];
            state_zobrist ^= zobrist[c][pos_index(pos[c])];
        }
        for (int x : sid[m.j]) {
            state_zobrist ^= zobrist[x][pos_index(pos[x])];
        }
        int start = count - m.k;
        vector<int> block(dep[m.i].begin() + start, dep[m.i].end());
        dep[m.i].erase(dep[m.i].begin() + start, dep[m.i].end());
        sid[m.j].insert(sid[m.j].begin(), block.begin(), block.end());
    } else {
        for (int idx = 0; idx < m.k; idx++) {
            int c = sid[m.j][idx];
            state_zobrist ^= zobrist[c][pos_index(pos[c])];
        }
        for (int x = m.k; x < (int)sid[m.j].size(); x++) {
            int c = sid[m.j][x];
            state_zobrist ^= zobrist[c][pos_index(pos[c])];
        }
        vector<int> block(sid[m.j].begin(), sid[m.j].begin() + m.k);
        sid[m.j].erase(sid[m.j].begin(), sid[m.j].begin() + m.k);
        dep[m.i].insert(dep[m.i].end(), block.begin(), block.end());
    }
    rebuild_pos_for_line(0, m.i);
    rebuild_pos_for_line(1, m.j);

    if (m.type == 0) {
        for (int x : sid[m.j]) {
            state_zobrist ^= zobrist[x][pos_index(pos[x])];
        }
    } else {
        int dep_count = (int)dep[m.i].size();
        for (int idx = dep_count - m.k; idx < dep_count; idx++) {
            int c = dep[m.i][idx];
            state_zobrist ^= zobrist[c][pos_index(pos[c])];
        }
        for (int x : sid[m.j]) {
            state_zobrist ^= zobrist[x][pos_index(pos[x])];
        }
    }

    for (int a : affected_pairs) {
        pair_penalties[a] = (a % 10 != 9) ? pair_penalty(a, a + 1) : 0;
        total_penalty += pair_penalties[a];
    }
    for (int r : affected_tops) {
        top_penalties[r] = top_penalty(r);
        total_penalty += top_penalties[r];
    }
}

void undo_move(const Move& m) {
    apply_move_and_update({1 - m.type, m.i, m.j, m.k});
}

Loc simulated_pos(int car, const Move& m) {
    Loc cur = pos[car];
    if (m.type == 0) {
        if (cur.kind == 0 && cur.line == m.i && cur.idx >= (int)dep[m.i].size() - m.k) {
            return {1, m.j, cur.idx - ((int)dep[m.i].size() - m.k)};
        }
        if (cur.kind == 1 && cur.line == m.j) return {1, m.j, cur.idx + m.k};
        return cur;
    } else {
        if (cur.kind == 1 && cur.line == m.j && cur.idx < m.k) {
            return {0, m.i, (int)dep[m.i].size() + cur.idx};
        }
        if (cur.kind == 1 && cur.line == m.j) return {1, m.j, cur.idx - m.k};
        return cur;
    }
}

int simulated_dep_count(int line, const Move& m) {
    int c = (int)dep[line].size();
    if (m.i == line) {
        if (m.type == 0) c -= m.k;
        else c += m.k;
    }
    return c;
}

int simulated_pair_penalty(int a_car, int b_car, const Move& m) {
    Loc a = simulated_pos(a_car, m);
    Loc b = simulated_pos(b_car, m);
    if (a.kind == b.kind && a.line == b.line && a.idx + 1 == b.idx) return 0;
    bool a_exposed = a.kind == 0 && a.idx == simulated_dep_count(a.line, m) - 1;
    bool b_exposed = b.kind == 1 && b.idx == 0;
    int base_penalty;
    if (a_exposed && b_exposed) base_penalty = REACH_PENALTY;
    else if (a_exposed || b_exposed) base_penalty = ONE_SIDE_EXPOSED_PENALTY;
    else base_penalty = UNCONNECTED_PENALTY;
    int vertical = 30;
    if (a.kind == b.kind && a.line == b.line) {
        vertical = a.idx < b.idx ? 60 : 800;
    }
    int horizontal = abs(a.line - b.line);
    return base_penalty + vertical + HORIZONTAL_WEIGHT * horizontal;
}

int simulated_top_penalty(int r, const Move& m) {
    int car = r * 10;
    Loc b = simulated_pos(car, m);
    if (b.kind == 0 && b.line == r && b.idx == 0) return 0;
    bool slot_ready = simulated_dep_count(r, m) == 0;
    bool car_exposed = b.kind == 1 && b.idx == 0;
    int base_penalty;
    if (slot_ready && car_exposed) base_penalty = REACH_PENALTY;
    else if (slot_ready || car_exposed) base_penalty = ONE_SIDE_EXPOSED_PENALTY;
    else base_penalty = UNCONNECTED_PENALTY;
    int vertical = (b.kind == 0) ? 30 : 50;
    int horizontal = abs(r - b.line);
    return base_penalty + vertical + HORIZONTAL_WEIGHT * horizontal;
}

int penalty_after(const Move& m) {
    auto cars = affected_cars_for_move(m);
    set<int> affected_pairs, affected_tops;
    affected_tops.insert(m.i);
    for (int c : cars) {
        if (c % 10 != 9) affected_pairs.insert(c);
        if (c % 10 != 0) affected_pairs.insert(c - 1);
        affected_tops.insert(c / 10);
    }
    int delta = 0;
    for (int a : affected_pairs) {
        delta -= pair_penalties[a];
        if (a % 10 != 9) delta += simulated_pair_penalty(a, a + 1, m);
    }
    for (int r : affected_tops) {
        delta -= top_penalties[r];
        delta += simulated_top_penalty(r, m);
    }
    return total_penalty + delta;
}

int incremental_delta_type0(int i, int j, int k_new) {
    int count = (int)dep[i].size();
    int idx_new_moved = count - k_new;
    int idx_new_tail = count - k_new - 1;
    int idx_old_head = count - k_new + 1;

    int new_moved = (idx_new_moved >= 0 && idx_new_moved < count) ? dep[i][idx_new_moved] : -1;
    int new_tail = (idx_new_tail >= 0 && idx_new_tail < count) ? dep[i][idx_new_tail] : -1;
    int old_head = (idx_old_head >= 0 && idx_old_head < count) ? dep[i][idx_old_head] : -1;

    Move m_new = {0, i, j, k_new};
    Move m_old = {0, i, j, k_new - 1};

    static bool seen_p[N];
    static bool seen_t[R];
    int pairs_arr[24], pairs_n = 0;
    int tops_arr[12], tops_n = 0;

    auto add_car = [&](int c) {
        if (c < 0) return;
        if (c % 10 != 9 && !seen_p[c]) { seen_p[c] = true; pairs_arr[pairs_n++] = c; }
        if (c % 10 != 0 && !seen_p[c - 1]) { seen_p[c - 1] = true; pairs_arr[pairs_n++] = c - 1; }
        int t = c / 10;
        if (!seen_t[t]) { seen_t[t] = true; tops_arr[tops_n++] = t; }
    };
    if (!seen_t[i]) { seen_t[i] = true; tops_arr[tops_n++] = i; }
    add_car(new_moved);
    add_car(new_tail);
    add_car(old_head);

    int delta = 0;
    for (int x = 0; x < pairs_n; x++) {
        int p = pairs_arr[x];
        delta += simulated_pair_penalty(p, p + 1, m_new) - simulated_pair_penalty(p, p + 1, m_old);
    }
    for (int x = 0; x < tops_n; x++) {
        int r = tops_arr[x];
        delta += simulated_top_penalty(r, m_new) - simulated_top_penalty(r, m_old);
    }

    for (int x = 0; x < pairs_n; x++) seen_p[pairs_arr[x]] = false;
    for (int x = 0; x < tops_n; x++) seen_t[tops_arr[x]] = false;

    return delta;
}

int incremental_delta_type1(int i, int j, int k_new) {
    int sid_size = (int)sid[j].size();
    int idx_new_moved = k_new - 1;
    int idx_new_head = k_new;
    int idx_old_tail = k_new - 2;

    int new_moved = (idx_new_moved >= 0 && idx_new_moved < sid_size) ? sid[j][idx_new_moved] : -1;
    int new_head = (idx_new_head >= 0 && idx_new_head < sid_size) ? sid[j][idx_new_head] : -1;
    int old_tail = (idx_old_tail >= 0 && idx_old_tail < sid_size) ? sid[j][idx_old_tail] : -1;

    Move m_new = {1, i, j, k_new};
    Move m_old = {1, i, j, k_new - 1};

    static bool seen_p[N];
    static bool seen_t[R];
    int pairs_arr[24], pairs_n = 0;
    int tops_arr[12], tops_n = 0;

    auto add_car = [&](int c) {
        if (c < 0) return;
        if (c % 10 != 9 && !seen_p[c]) { seen_p[c] = true; pairs_arr[pairs_n++] = c; }
        if (c % 10 != 0 && !seen_p[c - 1]) { seen_p[c - 1] = true; pairs_arr[pairs_n++] = c - 1; }
        int t = c / 10;
        if (!seen_t[t]) { seen_t[t] = true; tops_arr[tops_n++] = t; }
    };
    if (!seen_t[i]) { seen_t[i] = true; tops_arr[tops_n++] = i; }
    add_car(new_moved);
    add_car(new_head);
    add_car(old_tail);

    int delta = 0;
    for (int x = 0; x < pairs_n; x++) {
        int p = pairs_arr[x];
        delta += simulated_pair_penalty(p, p + 1, m_new) - simulated_pair_penalty(p, p + 1, m_old);
    }
    for (int x = 0; x < tops_n; x++) {
        int r = tops_arr[x];
        delta += simulated_top_penalty(r, m_new) - simulated_top_penalty(r, m_old);
    }

    for (int x = 0; x < pairs_n; x++) seen_p[pairs_arr[x]] = false;
    for (int x = 0; x < tops_n; x++) seen_t[tops_arr[x]] = false;

    return delta;
}

vector<Candidate> build_candidates(int current_penalty, bool only_positive) {
    vector<Candidate> result;

    for (int i = 0; i < R; i++) {
        int dep_count = (int)dep[i].size();
        int max_k = min(MAX_BLOCK, dep_count);
        if (max_k == 0) continue;
        for (int j = 0; j < R; j++) {
            int sid_size = (int)sid[j].size();
            int k_limit = min(max_k, SIDING_CAPACITY - sid_size);
            if (k_limit < 1) continue;

            Move m1 = {0, i, j, 1};
            int penalty_k = penalty_after(m1);
            int gain1 = current_penalty - penalty_k;
            if (!only_positive || gain1 > 0) result.push_back({m1, gain1});

            for (int k = 2; k <= k_limit; k++) {
                int delta = incremental_delta_type0(i, j, k);
                penalty_k += delta;
                int gain = current_penalty - penalty_k;
                if (!only_positive || gain > 0) result.push_back({{0, i, j, k}, gain});
            }
        }
    }

    for (int j = 0; j < R; j++) {
        int sid_size = (int)sid[j].size();
        int max_k = min(MAX_BLOCK, sid_size);
        if (max_k == 0) continue;
        for (int i = 0; i < R; i++) {
            int dep_size = (int)dep[i].size();
            int k_limit = min(max_k, DEPARTURE_CAPACITY - dep_size);
            if (k_limit < 1) continue;

            Move m1 = {1, i, j, 1};
            int penalty_k = penalty_after(m1);
            int gain1 = current_penalty - penalty_k;
            if (!only_positive || gain1 > 0) result.push_back({m1, gain1});

            for (int k = 2; k <= k_limit; k++) {
                int delta = incremental_delta_type1(i, j, k);
                penalty_k += delta;
                int gain = current_penalty - penalty_k;
                if (!only_positive || gain > 0) result.push_back({{1, i, j, k}, gain});
            }
        }
    }

    return result;
}

vector<Candidate> build_straight_drop_candidates(int current_penalty, bool randomize) {
    uniform_real_distribution<double> dist(0.3, 1.0);
    vector<Candidate> result;
    for (int i = 0; i < R; i++) {
        int j = i;
        if ((int)sid[j].size() >= SIDING_CAPACITY) continue;
        int max_k = min(MAX_BLOCK, (int)dep[i].size());
        for (int k = 1; k <= max_k; k++) {
            if ((int)sid[j].size() + k > SIDING_CAPACITY) continue;
            Move move = {0, i, j, k};
            int next_penalty = penalty_after(move);
            int gain = current_penalty - next_penalty;
            if (randomize) gain = (int)(gain * dist(rng));
            if (gain > 0) result.push_back({move, gain});
        }
    }
    return result;
}

vector<Move> select_best_turn(const vector<Candidate>& candidates) {
    constexpr int NEG = INT_MIN / 2;

    int best_gain[R][R];
    Move best_move[R][R];
    for (int i = 0; i < R; i++)
        for (int j = 0; j < R; j++)
            best_gain[i][j] = NEG;

    for (const auto& c : candidates) {
        int i = c.move.i;
        int j = c.move.j;
        if (c.gain > best_gain[i][j]) {
            best_gain[i][j] = c.gain;
            best_move[i][j] = c.move;
        }
    }

    int dp[R + 1][R + 1];
    int pre[R + 1][R + 1];
    bool take[R + 1][R + 1];
    Move chosen[R + 1][R + 1];

    for (int i = 0; i <= R; i++)
        for (int j = 0; j <= R; j++) {
            dp[i][j] = NEG;
            pre[i][j] = -1;
            take[i][j] = false;
        }

    dp[0][0] = 0;

    for (int i = 0; i < R; i++) {
        for (int last_idx = 0; last_idx <= R; last_idx++) {
            if (dp[i][last_idx] == NEG) continue;

            if (dp[i][last_idx] > dp[i + 1][last_idx]) {
                dp[i + 1][last_idx] = dp[i][last_idx];
                pre[i + 1][last_idx] = last_idx;
                take[i + 1][last_idx] = false;
            }

            int last_j = last_idx - 1;
            for (int j = last_j + 1; j < R; j++) {
                if (best_gain[i][j] == NEG) continue;
                int ni = j + 1;
                int val = dp[i][last_idx] + best_gain[i][j];
                if (val > dp[i + 1][ni]) {
                    dp[i + 1][ni] = val;
                    pre[i + 1][ni] = last_idx;
                    take[i + 1][ni] = true;
                    chosen[i + 1][ni] = best_move[i][j];
                }
            }
        }
    }

    int best_idx = 0;
    for (int j = 1; j <= R; j++) {
        if (dp[R][j] > dp[R][best_idx]) best_idx = j;
    }
    if (dp[R][best_idx] <= 0) return {};

    vector<Move> result;
    int cur = best_idx;
    for (int i = R; i >= 1; i--) {
        if (take[i][cur]) result.push_back(chosen[i][cur]);
        cur = pre[i][cur];
    }
    reverse(result.begin(), result.end());
    return result;
}

struct Shift {
    Move t1, t2;
};

optional<Shift> find_best_lateral_shift(int current_penalty) {
    int best_gain = 0;
    optional<Shift> best;
    for (int i = 0; i < R; i++) {
        int max_k = min(MAX_BLOCK, (int)dep[i].size());
        for (int k = 1; k <= max_k; k++) {
            for (int j = 0; j < R; j++) {
                if ((int)sid[j].size() + k > SIDING_CAPACITY) continue;
                for (int it = 0; it < R; it++) {
                    if (it == i) continue;
                    if ((int)dep[it].size() + k > DEPARTURE_CAPACITY) continue;
                    Move m1 = {0, i, j, k};
                    Move m2 = {1, it, j, k};
                    apply_move_and_update(m1);
                    int new_p = penalty_after(m2);
                    int gain = current_penalty - new_p;
                    undo_move(m1);
                    if (gain > best_gain) {
                        best_gain = gain;
                        best = Shift{m1, m2};
                    }
                }
            }
        }
    }
    return best;
}

void greedy_loop() {
    while ((int)turns.size() < MAX_TURNS) {
        if (total_penalty == 0) break;
        int prev = total_penalty;
        auto candidates = build_candidates(prev, true);
        auto moves = select_best_turn(candidates);
        bool greedy_applied = false;
        if (!moves.empty()) {
            for (const auto& m : moves) apply_move_and_update(m);
            if (total_penalty < prev) {
                turns.push_back(moves);
                greedy_applied = true;
            } else {
                for (int idx = (int)moves.size() - 1; idx >= 0; idx--) undo_move(moves[idx]);
            }
        }
        if (greedy_applied) continue;
        if ((int)turns.size() + 2 > MAX_TURNS) break;
        auto shift = find_best_lateral_shift(prev);
        if (!shift) break;
        apply_move_and_update(shift->t1);
        turns.push_back({shift->t1});
        apply_move_and_update(shift->t2);
        turns.push_back({shift->t2});
    }
}

string moves_key(const vector<Move>& moves) {
    vector<Move> copy = moves;
    sort(copy.begin(), copy.end(), [](const Move& a, const Move& b) {
        if (a.i != b.i) return a.i < b.i;
        return a.j < b.j;
    });
    string s;
    for (const auto& m : copy) {
        s += to_string(m.type) + "," + to_string(m.i) + "," + to_string(m.j) + "," + to_string(m.k) + ";";
    }
    return s;
}

struct Snapshot {
    int dep_size[R];
    int dep_data[R][DEPARTURE_CAPACITY + 1];
    int sid_size[R];
    int sid_data[R][SIDING_CAPACITY + 1];
    Loc pos_arr[N];
    int pair_pen[N];
    int top_pen[R];
    int total_pen;
    uint64_t zobrist;
};

void save_snapshot(Snapshot& s) {
    for (int i = 0; i < R; i++) {
        s.dep_size[i] = (int)dep[i].size();
        for (int j = 0; j < s.dep_size[i]; j++) s.dep_data[i][j] = dep[i][j];
        s.sid_size[i] = (int)sid[i].size();
        for (int j = 0; j < s.sid_size[i]; j++) s.sid_data[i][j] = sid[i][j];
    }
    memcpy(s.pos_arr, pos, sizeof(pos));
    memcpy(s.pair_pen, pair_penalties, sizeof(pair_penalties));
    memcpy(s.top_pen, top_penalties, sizeof(top_penalties));
    s.total_pen = total_penalty;
    s.zobrist = state_zobrist;
}

void restore_snapshot(const Snapshot& s) {
    for (int i = 0; i < R; i++) {
        dep[i].assign(s.dep_data[i], s.dep_data[i] + s.dep_size[i]);
        sid[i].assign(s.sid_data[i], s.sid_data[i] + s.sid_size[i]);
    }
    memcpy(pos, s.pos_arr, sizeof(pos));
    memcpy(pair_penalties, s.pair_pen, sizeof(pair_penalties));
    memcpy(top_penalties, s.top_pen, sizeof(top_penalties));
    total_penalty = s.total_pen;
    state_zobrist = s.zobrist;
}

struct CompactNode {
    int parent_idx;
    vector<vector<Move>> turns;
};

struct BeamEntry {
    int compact_idx;
    Snapshot snap;
    int total_pen;
    int t_count;
};

vector<vector<Move>> run_beam_search(int beam_width, long long time_budget_ms) {
    auto t_start = chrono::steady_clock::now();

    vector<CompactNode> compact;
    vector<BeamEntry> beam;

    BeamEntry root;
    root.compact_idx = 0;
    save_snapshot(root.snap);
    root.total_pen = total_penalty;
    root.t_count = 0;
    compact.push_back({-1, {}});
    beam.push_back(root);

    int best_compact_idx = -1;
    int best_t_count = INT_MAX;

    int max_gen = 200;
    int gens_done = 0;

    for (int gen = 0; gen < max_gen; gen++) {
        if (chrono::steady_clock::now() - t_start > chrono::milliseconds(time_budget_ms)) break;
        if (beam.empty()) break;

        vector<BeamEntry> new_beam;
        new_beam.reserve(beam.size() * 3);

        uniform_real_distribution<double> dist(0.3, 1.0);
        int num_variants = (gen == 0) ? 50 : 15;
        unordered_set<size_t> seen_states;
        seen_states.reserve(beam_width * num_variants * 4 + 100);
        seen_states.max_load_factor(0.7);

        for (auto& parent : beam) {
            if (parent.t_count + 1 >= best_t_count) continue;

            restore_snapshot(parent.snap);
            int prev = total_penalty;
            auto cands = build_candidates(prev, true);

            int children_added = 0;

            for (int v = 0; v < num_variants && !cands.empty(); v++) {
                if (v > 0) restore_snapshot(parent.snap);

                vector<Candidate> scored;
                if (v == 0) {
                    scored = cands;
                } else {
                    scored.reserve(cands.size());
                    for (const auto& c : cands) {
                        scored.push_back({c.move, (int)(c.gain * dist(rng))});
                    }
                }

                auto moves = select_best_turn(scored);
                if (moves.empty()) continue;

                for (const auto& m : moves) apply_move_and_update(m);
                children_added++;

                size_t h = (size_t)state_zobrist;
                if (!seen_states.insert(h).second) continue;

                int new_compact_idx = (int)compact.size();
                compact.push_back({parent.compact_idx, {moves}});

                int new_t = parent.t_count + 1;
                bool finished = (total_penalty == 0);

                if (finished) {
                    if (new_t < best_t_count) {
                        best_t_count = new_t;
                        best_compact_idx = new_compact_idx;
                    }
                } else {
                    BeamEntry child;
                    child.compact_idx = new_compact_idx;
                    save_snapshot(child.snap);
                    child.total_pen = total_penalty;
                    child.t_count = new_t;
                    new_beam.push_back(child);
                }
            }

            if (children_added == 0 && parent.t_count + 1 < best_t_count) {
                restore_snapshot(parent.snap);
                auto all_cands = build_candidates(prev, false);
                sort(all_cands.begin(), all_cands.end(),
                     [](const Candidate& a, const Candidate& b) { return a.gain > b.gain; });
                int top_n = min(3, (int)all_cands.size());
                for (int t = 0; t < top_n; t++) {
                    if (t > 0) restore_snapshot(parent.snap);
                    apply_move_and_update(all_cands[t].move);

                    size_t h = (size_t)state_zobrist;
                    if (!seen_states.insert(h).second) continue;

                    int new_compact_idx = (int)compact.size();
                    compact.push_back({parent.compact_idx, {{all_cands[t].move}}});

                    int new_t = parent.t_count + 1;
                    bool finished = (total_penalty == 0);

                    if (finished) {
                        if (new_t < best_t_count) {
                            best_t_count = new_t;
                            best_compact_idx = new_compact_idx;
                        }
                    } else {
                        BeamEntry child;
                        child.compact_idx = new_compact_idx;
                        save_snapshot(child.snap);
                        child.total_pen = total_penalty;
                        child.t_count = new_t;
                        new_beam.push_back(child);
                    }
                }
            }
        }

        if (new_beam.empty()) break;

        sort(new_beam.begin(), new_beam.end(), [](const BeamEntry& a, const BeamEntry& b) {
            return a.total_pen < b.total_pen;
        });
        if ((int)new_beam.size() > beam_width) new_beam.resize(beam_width);
        beam = move(new_beam);
        gens_done = gen + 1;
    }

    cerr << "beam: gens=" << gens_done << " compact_size=" << compact.size()
         << " best_t=" << (best_compact_idx >= 0 ? best_t_count : -1) << "\n";

    int idx = -1;
    if (best_compact_idx >= 0) {
        idx = best_compact_idx;
    } else if (!beam.empty()) {
        idx = beam[0].compact_idx;
    }

    vector<vector<Move>> result;
    while (idx > 0) {
        for (auto it = compact[idx].turns.rbegin(); it != compact[idx].turns.rend(); ++it) {
            result.push_back(*it);
        }
        idx = compact[idx].parent_idx;
    }
    reverse(result.begin(), result.end());
    return result;
}

int compute_score() {
    if (total_penalty == 0) return 100 * R + 4000 - (int)turns.size();
    int partial = 0;
    for (int r = 0; r < R; r++) {
        for (int c = 0; c < (int)dep[r].size(); c++) {
            int car = dep[r][c];
            if (car / 10 == r) {
                partial += 1;
                if (car % 10 == c) partial += 9;
            }
        }
    }
    return partial;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    init_zobrist();
    input_data();
    reset_state();
    initialize_caches();

    auto best_turns = run_beam_search(200, TIME_BUDGET_MS);

    cout << best_turns.size() << "\n";
    for (const auto& turn : best_turns) {
        cout << turn.size() << "\n";
        for (const auto& m : turn) {
            cout << m.type << " " << m.i << " " << m.j << " " << m.k << "\n";
        }
    }
    return 0;
}
