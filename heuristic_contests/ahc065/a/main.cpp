#include <bits/stdc++.h>
#include <ankerl/unordered_dense.h>
using namespace std;

using ll = long long;

#ifndef EVAL_LOOK
#define EVAL_LOOK 80
#endif

#ifndef EVAL_WEIGHT
#define EVAL_WEIGHT 30
#endif

#ifndef EVAL_MODE
#define EVAL_MODE 0
#endif

#ifndef BEAM_WIDTH
#define BEAM_WIDTH 300
#endif

#ifndef FLAT_GAIN
#define FLAT_GAIN 250
#endif

#ifndef FLAT_PENALTY
#define FLAT_PENALTY 1
#endif

#ifndef SETUP_FUTURE
#define SETUP_FUTURE 0
#endif

struct Conveyor {
    vector<pair<int, int>> cells;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    vector<vector<int>> a(N, vector<int>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) cin >> a[i][j];
    }

    vector<Conveyor> conveyors;
    vector<vector<int>> board = a;
    vector<pair<int, int>> pos(N * N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) pos[board[i][j]] = {i, j};
    }

    // Each cell belongs to exactly two conveyors: one horizontal ring and one
    // vertical ring. This uses the per-cell limit fully while keeping moves easy
    // to reason about.

    // 0..9: 2x20 horizontal rings over row pairs (0,1), (2,3), ...
    for (int r = 0; r < N; r += 2) {
        Conveyor c;
        for (int j = 0; j < N; ++j) c.cells.push_back({r, j});
        for (int j = N - 1; j >= 0; --j) c.cells.push_back({r + 1, j});
        conveyors.push_back(c);
    }

    // 10..19: 20x2 vertical rings over column pairs (0,1), (2,3), ...
    for (int col = 0; col < N; col += 2) {
        Conveyor c;
        for (int i = 0; i < N; ++i) c.cells.push_back({i, col});
        for (int i = N - 1; i >= 0; --i) c.cells.push_back({i, col + 1});
        conveyors.push_back(c);
    }

    // Flatten cells to id = i * N + j. conveyor_index[m][cell] gives the index
    // of a cell on conveyor m, so we can compute where the current box moves
    // before actually rotating a whole state.
    vector<vector<int>> conveyor_cells;
    for (const auto &c : conveyors) {
        vector<int> cells;
        for (auto [i, j] : c.cells) cells.push_back(i * N + j);
        conveyor_cells.push_back(cells);
    }
    vector<vector<int>> conveyor_index(conveyor_cells.size(), vector<int>(N * N, -1));
    for (int m = 0; m < (int)conveyor_cells.size(); ++m) {
        for (int x = 0; x < (int)conveyor_cells[m].size(); ++x) {
            conveyor_index[m][conveyor_cells[m][x]] = x;
        }
    }

    struct State {
        array<int, 400> board; // board[cell] = box number, or -1 for empty.
        array<int, 400> pos;   // pos[box] = cell id, or -1 if already removed.
        int next_box = 0;      // Smallest box number still in the warehouse.
        int node = 0;          // Path node id for reconstructing operations.
        int depth = 0;         // Number of operations used to reach this state.
        int flat_streak = 0;   // Consecutive moves that did not move current box closer.
        long long eval = 0;    // Smaller is better for beam pruning.
        uint64_t hash = 0;     // Zobrist hash of board[] and next_box.
    };

    struct PathNode {
        int parent;
        int m;
        int d;
    };

    const int SZ = N * N;
    const int EXIT = N / 2;

    auto splitmix64 = [](uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    };

    vector<array<uint64_t, 401>> zob(SZ);
    vector<uint64_t> zob_next(SZ + 1);
    for (int cell = 0; cell < SZ; ++cell) {
        for (int v = 0; v <= SZ; ++v) {
            zob[cell][v] = splitmix64(1000003ULL * cell + v);
        }
    }
    for (int v = 0; v <= SZ; ++v) zob_next[v] = splitmix64(998244353ULL + v);

    auto value_id = [&](int box) {
        return box == -1 ? SZ : box;
    };

    vector<PathNode> path_nodes;
    path_nodes.reserve(2000000);
    path_nodes.push_back({-1, -1, 0});

    auto remove_if_ready = [&](State &st) {
        if (st.next_box < SZ && st.board[EXIT] == st.next_box) {
            st.hash ^= zob_next[st.next_box];
            st.hash ^= zob[EXIT][value_id(st.board[EXIT])];
            st.pos[st.next_box] = -1;
            st.board[EXIT] = -1;
            st.hash ^= zob[EXIT][value_id(st.board[EXIT])];
            ++st.next_box;
            st.hash ^= zob_next[st.next_box];
        }
    };

    // Apply one conveyor operation and update both board[] and pos[].
    auto rotate = [&](State &st, int m, int d) {
        path_nodes.push_back({st.node, m, d});
        st.node = (int)path_nodes.size() - 1;
        ++st.depth;
        const auto &cells = conveyor_cells[m];
        const int L = (int)cells.size();
        array<int, 40> old;
        for (int x = 0; x < L; ++x) old[x] = st.board[cells[x]];
        for (int x = 0; x < L; ++x) st.hash ^= zob[cells[x]][value_id(st.board[cells[x]])];
        for (int x = 0; x < L; ++x) {
            int y = (x + d + L) % L;
            int v = old[x];
            st.board[cells[y]] = v;
            if (v != -1) st.pos[v] = cells[y];
        }
        for (int x = 0; x < L; ++x) st.hash ^= zob[cells[x]][value_id(st.board[cells[x]])];
        remove_if_ready(st);
    };

    // Manhattan distance to the exit (0, 10). A candidate move is accepted only
    // if this distance for the current box decreases by exactly one.
    auto dist_to_exit = [&](const State &st, int box) {
        int p = st.pos[box];
        if (p == -1) return 0;
        int i = p / N;
        int j = p % N;
        return abs(j - 10) + i;
    };

    // Beam score: prioritize shorter operation sequences, then states where
    // near-future boxes are already closer to the exit.
    auto evaluate = [&](const State &st) {
        long long value = 1LL * st.depth * 1000;
        const int LOOK = EVAL_LOOK;
        for (int t = 0; t < LOOK && st.next_box + t < SZ; ++t) {
            long long w = LOOK - t;
            if constexpr (EVAL_MODE == 1) w *= w;
            if constexpr (EVAL_MODE == 2) w = 1LL << min(20, LOOK - 1 - t);
            value += w * EVAL_WEIGHT * dist_to_exit(st, st.next_box + t);
        }
        return value;
    };

    auto future_score = [&](const State &st) {
        long long value = 0;
        const int LOOK = EVAL_LOOK;
        for (int t = 1; t < LOOK && st.next_box + t < SZ; ++t) {
            value += 1LL * (LOOK - t) * dist_to_exit(st, st.next_box + t);
        }
        return value;
    };

    // Expand one beam state. Normal candidates move the current box one step
    // closer to the exit. We also allow a small number of flat moves if they
    // keep the current box at the same distance and improve near-future boxes.
    auto add_step_candidates = [&](vector<State> &to, const State &from) {
        int p = from.pos[from.next_box];
        if (p == -1) {
            State ns = from;
            remove_if_ready(ns);
            ns.eval = evaluate(ns);
            to.push_back(move(ns));
            return;
        }

        int i = p / N;
        int j = p % N;
        int before = dist_to_exit(from, from.next_box);
        // Horizontal ring id and vertical ring id containing (i, j).
        array<int, 2> ms = {i / 2, 10 + j / 2};

        for (int m : ms) {
            int x = conveyor_index[m][p];
            if (x == -1) continue;
            int L = (int)conveyor_cells[m].size();
            for (int d : {-1, 1}) {
                int np = conveyor_cells[m][(x + d + L) % L];
                int ni = np / N;
                int nj = np % N;
                int after = abs(nj - 10) + ni;
                if (after != before - 1) continue;

                State ns = from;
                rotate(ns, m, d);
                ns.flat_streak = 0;
                ns.eval = evaluate(ns);
                to.push_back(move(ns));
            }
        }

        if constexpr (SETUP_FUTURE <= 0) return;
        if (from.flat_streak >= 1) return;

        long long before_future = future_score(from);
        const int current_cell = from.pos[from.next_box];
        const int limit = min(SZ, from.next_box + 1 + SETUP_FUTURE);
        for (int box = from.next_box + 1; box < limit; ++box) {
            int fp = from.pos[box];
            if (fp == -1) continue;
            int fi = fp / N;
            int fj = fp % N;
            array<int, 2> setup_ms = {fi / 2, 10 + fj / 2};

            for (int m : setup_ms) {
                if (conveyor_index[m][current_cell] != -1) continue;
                for (int d : {-1, 1}) {
                    State ns = from;
                    rotate(ns, m, d);
                    if (ns.next_box != from.next_box) continue;
                    long long gain = before_future - future_score(ns);
                    if (gain * FLAT_GAIN <= FLAT_PENALTY * 1000) continue;
                    ns.flat_streak = from.flat_streak + 1;
                    ns.eval = evaluate(ns);
                    to.push_back(move(ns));
                }
            }
        }
    };

    auto better = [](const State &lhs, const State &rhs) {
        if (lhs.next_box != rhs.next_box) return lhs.next_box > rhs.next_box;
        if (lhs.eval != rhs.eval) return lhs.eval < rhs.eval;
        return lhs.depth < rhs.depth;
    };

    State init;
    init.board.fill(-1);
    init.pos.fill(-1);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int v = a[i][j];
            init.board[i * N + j] = v;
            init.pos[v] = i * N + j;
            init.hash ^= zob[i * N + j][value_id(v)];
        }
    }
    init.hash ^= zob_next[init.next_box];
    remove_if_ready(init);
    init.eval = evaluate(init);

    // Fixed-width step-by-step beam search. One iteration corresponds to one
    // conveyor operation, not to one delivered box.
    vector<State> beam = {init};
    while (!beam.empty() && beam.front().next_box < SZ) {
        vector<State> nxt;
        nxt.reserve(beam.size() * 4);
        for (const auto &st : beam) {
            if (st.depth >= 100000 || st.next_box >= SZ) {
                nxt.push_back(st);
                continue;
            }
            add_step_candidates(nxt, st);
        }
        if (nxt.empty()) break;
        sort(nxt.begin(), nxt.end(), [&](const State &lhs, const State &rhs) {
            return better(lhs, rhs);
        });
        vector<State> deduped;
        deduped.reserve(nxt.size());
        ankerl::unordered_dense::set<uint64_t> seen;
        seen.reserve(nxt.size() * 2);
        for (auto &st : nxt) {
            if (seen.insert(st.hash).second) deduped.push_back(move(st));
        }
        nxt = move(deduped);
        if ((int)nxt.size() > BEAM_WIDTH) nxt.resize(BEAM_WIDTH);
        beam = move(nxt);
    }

    auto best_it = max_element(beam.begin(), beam.end(), [&](const State &lhs, const State &rhs) {
        return better(rhs, lhs);
    });
    vector<pair<int, int>> ops;
    for (int node = best_it->node; path_nodes[node].parent != -1; node = path_nodes[node].parent) {
        ops.push_back({path_nodes[node].m, path_nodes[node].d});
    }
    reverse(ops.begin(), ops.end());

    cout << conveyors.size() << '\n';
    for (const auto &c : conveyors) {
        cout << c.cells.size();
        for (auto [i, j] : c.cells) cout << ' ' << i << ' ' << j;
        cout << '\n';
    }

    cout << ops.size() << '\n';
    for (auto [m, d] : ops) cout << m << ' ' << d << '\n';
}
