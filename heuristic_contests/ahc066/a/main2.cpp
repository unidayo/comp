#include <bits/stdc++.h>
using namespace std;

struct Pos {
    int r;
    int c;
};

static const int DR[4] = {-1, 0, 1, 0};
static const int DC[4] = {0, 1, 0, -1};

struct Solver {
    int N, M, T;
    vector<string> v, h;
    vector<Pos> ball, basket;
    Pos cur{0, 0};
    int dir = 1;

    bool can_move(int r, int c, int d) const {
        int nr = r + DR[d], nc = c + DC[d];
        if (nr < 0 || nr >= N || nc < 0 || nc >= N) return false;
        if (d == 0) return h[r - 1][c] == '0';
        if (d == 2) return h[r][c] == '0';
        if (d == 1) return v[r][c] == '0';
        return v[r][c - 1] == '0';
    }

    int cell_id(Pos p) const { return p.r * N + p.c; }

    array<pair<int, int>, 4> directed_cost(Pos from, Pos to) const {
        const int SZ = N * N * 4;
        auto id = [&](int r, int c, int d) { return ((r * N + c) * 4 + d); };

        array<pair<int, int>, 4> res;
        for (int sd = 0; sd < 4; ++sd) {
            vector<int> dist(SZ, -1);
            queue<int> q;
            int s = id(from.r, from.c, sd);
            dist[s] = 0;
            q.push(s);
            while (!q.empty()) {
                int x = q.front();
                q.pop();
                int d = x % 4;
                int cell = x / 4;
                int r = cell / N, c = cell % N;

                int nd = (d + 1) % 4;
                int y = id(r, c, nd);
                if (dist[y] == -1) {
                    dist[y] = dist[x] + 1;
                    q.push(y);
                }

                nd = (d + 3) % 4;
                y = id(r, c, nd);
                if (dist[y] == -1) {
                    dist[y] = dist[x] + 1;
                    q.push(y);
                }

                if (can_move(r, c, d)) {
                    int nr = r + DR[d], nc = c + DC[d];
                    y = id(nr, nc, d);
                    if (dist[y] == -1) {
                        dist[y] = dist[x] + 1;
                        q.push(y);
                    }
                }
            }

            int best = INT_MAX, best_dir = 0;
            for (int ed = 0; ed < 4; ++ed) {
                int t = id(to.r, to.c, ed);
                if (dist[t] < best) {
                    best = dist[t];
                    best_dir = ed;
                }
            }
            res[sd] = {best, best_dir};
        }
        return res;
    }

    vector<int> improve_order(vector<int> ord, const function<int(const vector<int> &)> &cost) const {
        int best = cost(ord);
        bool updated = true;
        while (updated) {
            updated = false;
            int best_type = -1, best_i = -1, best_j = -1, best_cost = best;
            for (int i = 0; i < M; ++i) {
                for (int j = i + 1; j < M; ++j) {
                    swap(ord[i], ord[j]);
                    int cur_cost = cost(ord);
                    if (cur_cost < best_cost) {
                        best_cost = cur_cost;
                        best_type = 0;
                        best_i = i;
                        best_j = j;
                    }
                    swap(ord[i], ord[j]);

                    reverse(ord.begin() + i, ord.begin() + j + 1);
                    cur_cost = cost(ord);
                    if (cur_cost < best_cost) {
                        best_cost = cur_cost;
                        best_type = 1;
                        best_i = i;
                        best_j = j;
                    }
                    reverse(ord.begin() + i, ord.begin() + j + 1);
                }
            }

            for (int i = 0; i < M; ++i) {
                int x = ord[i];
                ord.erase(ord.begin() + i);
                for (int j = 0; j <= M - 1; ++j) {
                    if (j == i) continue;
                    ord.insert(ord.begin() + j, x);
                    int cur_cost = cost(ord);
                    if (cur_cost < best_cost) {
                        best_cost = cur_cost;
                        best_type = 2;
                        best_i = i;
                        best_j = j;
                    }
                    ord.erase(ord.begin() + j);
                }
                ord.insert(ord.begin() + i, x);
            }

            if (best_type == 0) {
                swap(ord[best_i], ord[best_j]);
            } else if (best_type == 1) {
                reverse(ord.begin() + best_i, ord.begin() + best_j + 1);
            } else if (best_type == 2) {
                int x = ord[best_i];
                ord.erase(ord.begin() + best_i);
                ord.insert(ord.begin() + best_j, x);
            }
            if (best_type != -1) {
                best = best_cost;
                updated = true;
            }
        }
        return ord;
    }

    vector<vector<int>> build_orders() const {
        vector<array<pair<int, int>, 4>> start_to_ball(M), ball_to_basket(M);
        vector<vector<array<pair<int, int>, 4>>> basket_to_ball(M, vector<array<pair<int, int>, 4>>(M));
        for (int k = 0; k < M; ++k) {
            start_to_ball[k] = directed_cost({0, 0}, ball[k]);
            ball_to_basket[k] = directed_cost(ball[k], basket[k]);
        }
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < M; ++j) {
                basket_to_ball[i][j] = directed_cost(basket[i], ball[j]);
            }
        }
        auto task_cost_from = [&](int prev, int k) {
            int best = INT_MAX / 4;
            if (prev == -1) {
                auto [c1, d1] = start_to_ball[k][1];
                auto [c2, _] = ball_to_basket[k][d1];
                best = c1 + c2 + 2;
            } else {
                for (int d = 0; d < 4; ++d) {
                    auto [c1, d1] = basket_to_ball[prev][k][d];
                    auto [c2, _] = ball_to_basket[k][d1];
                    best = min(best, c1 + c2 + 2);
                }
            }
            return best;
        };

        function<int(const vector<int> &)> sequence_cost = [&](const vector<int> &p) {
            array<int, 4> dp;
            dp.fill(INT_MAX / 4);
            dp[1] = 0;
            int prev = -1;
            for (int k : p) {
                array<int, 4> ndp;
                ndp.fill(INT_MAX / 4);
                for (int d = 0; d < 4; ++d) {
                    if (dp[d] >= INT_MAX / 8) continue;
                    auto [c1, d1] = (prev == -1 ? start_to_ball[k][d] : basket_to_ball[prev][k][d]);
                    auto [c2, d2] = ball_to_basket[k][d1];
                    ndp[d2] = min(ndp[d2], dp[d] + c1 + c2 + 2);
                }
                dp = ndp;
                prev = k;
            }
            return *min_element(dp.begin(), dp.end());
        };
        auto insertion_cost = [&](const vector<int> &p, int pos, int value) {
            array<int, 4> dp;
            dp.fill(INT_MAX / 4);
            dp[1] = 0;
            int prev = -1;
            for (int i = 0; i <= (int)p.size(); ++i) {
                int k = (i == pos ? value : p[i - (i > pos)]);
                array<int, 4> ndp;
                ndp.fill(INT_MAX / 4);
                for (int d = 0; d < 4; ++d) {
                    if (dp[d] >= INT_MAX / 8) continue;
                    auto [c1, d1] = (prev == -1 ? start_to_ball[k][d] : basket_to_ball[prev][k][d]);
                    auto [c2, d2] = ball_to_basket[k][d1];
                    ndp[d2] = min(ndp[d2], dp[d] + c1 + c2 + 2);
                }
                dp = ndp;
                prev = k;
            }
            return *min_element(dp.begin(), dp.end());
        };

        vector<int> best(M);
        iota(best.begin(), best.end(), 0);
        best = improve_order(best, sequence_cost);
        int best_cost = sequence_cost(best);
        vector<pair<int, vector<int>>> candidates;
        candidates.push_back({best_cost, best});

        auto try_update = [&](vector<int> cand, bool do_improve = true) {
            if (do_improve) cand = improve_order(move(cand), sequence_cost);
            int c = sequence_cost(cand);
            candidates.push_back({c, cand});
            if (c < best_cost) {
                best_cost = c;
                best = move(cand);
            }
        };

        vector<int> greedy;
        vector<char> used(M, 0);
        int prev_task = -1;
        for (int step = 0; step < M; ++step) {
            int nxt = -1, nxt_cost = INT_MAX;
            for (int k = 0; k < M; ++k) {
                if (used[k]) continue;
                int c = task_cost_from(prev_task, k);
                if (c < nxt_cost) {
                    nxt_cost = c;
                    nxt = k;
                }
            }
            used[nxt] = 1;
            greedy.push_back(nxt);
            prev_task = nxt;
        }
        try_update(greedy);

        vector<int> first_candidates(M);
        iota(first_candidates.begin(), first_candidates.end(), 0);
        sort(first_candidates.begin(), first_candidates.end(),
             [&](int a, int b) { return task_cost_from(-1, a) < task_cost_from(-1, b); });
        first_candidates.resize(min(M, 8));
        for (int first : first_candidates) {
            vector<int> cand;
            vector<char> used_insert(M, 0);
            cand.push_back(first);
            used_insert[first] = 1;
            while ((int)cand.size() < M) {
                int best_k = -1, best_pos = -1, best_delta = INT_MAX;
                for (int k = 0; k < M; ++k) {
                    if (used_insert[k]) continue;
                    for (int pos = 0; pos <= (int)cand.size(); ++pos) {
                        int delta = insertion_cost(cand, pos, k) - sequence_cost(cand);
                        if (delta < best_delta) {
                            best_delta = delta;
                            best_k = k;
                            best_pos = pos;
                        }
                    }
                }
                if (best_k == -1) break;
                cand.insert(cand.begin() + best_pos, best_k);
                used_insert[best_k] = 1;
            }
            try_update(cand);
        }

        mt19937 rng(1);
        const int trials = 500;
        vector<pair<int, vector<int>>> random_candidates;
        random_candidates.reserve(trials);
        for (int trial = 0; trial < trials; ++trial) {
            vector<int> cand;
            vector<int> rem(M);
            iota(rem.begin(), rem.end(), 0);
            int prev = -1;
            while (!rem.empty()) {
                vector<pair<int, int>> score;
                score.reserve(rem.size());
                for (int k : rem) {
                    int c = task_cost_from(prev, k);
                    score.push_back({c, k});
                }
                sort(score.begin(), score.end());
                int width = min<int>(5, score.size());
                int pick_rank = uniform_int_distribution<int>(0, width - 1)(rng);
                int pick = score[pick_rank].second;
                cand.push_back(pick);
                prev = pick;
                rem.erase(find(rem.begin(), rem.end(), pick));
            }
            random_candidates.push_back({sequence_cost(cand), move(cand)});
        }
        sort(random_candidates.begin(), random_candidates.end(),
             [](const auto &a, const auto &b) { return a.first < b.first; });
        for (int i = 0; i < min<int>(20, random_candidates.size()); ++i) {
            try_update(move(random_candidates[i].second));
        }

        candidates.push_back({best_cost, best});
        sort(candidates.begin(), candidates.end(), [](const auto &a, const auto &b) { return a.first < b.first; });

        vector<vector<int>> orders;
        set<vector<int>> seen;
        for (auto &[_, cand] : candidates) {
            if (seen.insert(cand).second) orders.push_back(move(cand));
            if ((int)orders.size() >= 20) break;
        }
        return orders;
    }

    string shortest_ops_to(Pos target) {
        const int SZ = N * N * 4;
        auto id = [&](int r, int c, int d) { return ((r * N + c) * 4 + d); };

        vector<int> dist(SZ, -1), par(SZ, -1);
        vector<char> how(SZ, 0);
        queue<int> q;
        int s = id(cur.r, cur.c, dir);
        dist[s] = 0;
        q.push(s);

        int goal = -1;
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            int d = x % 4;
            int cell = x / 4;
            int r = cell / N, c = cell % N;
            if (r == target.r && c == target.c) {
                goal = x;
                break;
            }

            {
                int nd = (d + 1) % 4;
                int y = id(r, c, nd);
                if (dist[y] == -1) {
                    dist[y] = dist[x] + 1;
                    par[y] = x;
                    how[y] = 'R';
                    q.push(y);
                }
            }
            {
                int nd = (d + 3) % 4;
                int y = id(r, c, nd);
                if (dist[y] == -1) {
                    dist[y] = dist[x] + 1;
                    par[y] = x;
                    how[y] = 'L';
                    q.push(y);
                }
            }
            if (can_move(r, c, d)) {
                int nr = r + DR[d], nc = c + DC[d];
                int y = id(nr, nc, d);
                if (dist[y] == -1) {
                    dist[y] = dist[x] + 1;
                    par[y] = x;
                    how[y] = 'F';
                    q.push(y);
                }
            }
        }

        string ops;
        for (int x = goal; x != s; x = par[x]) ops.push_back(how[x]);
        reverse(ops.begin(), ops.end());

        int x = goal;
        dir = x % 4;
        int cell = x / 4;
        cur = {cell / N, cell % N};
        return ops;
    }

    string build_plain_ops(const vector<int> &order) {
        string ops;
        cur = {0, 0};
        dir = 1;
        for (int k : order) {
            ops += shortest_ops_to(ball[k]);
            ops.push_back('S');
            ops += shortest_ops_to(basket[k]);
            ops.push_back('S');
        }
        return ops;
    }

    static bool valid_macro_string(const string &s) {
        if (s.empty() || (int)s.size() > 5) return false;
        for (char ch : s) {
            if (ch != 'F' && ch != 'L' && ch != 'R') return false;
        }
        for (int i = 1; i < (int)s.size(); ++i) {
            if ((s[i - 1] == 'L' && s[i] == 'R') || (s[i - 1] == 'R' && s[i] == 'L')) return false;
        }
        return true;
    }

    vector<string> build_macro_candidates(const string &plain_ops) const {
        vector<string> base = {"",   "F",   "FF",   "FFF",   "FFFF", "FFFFF", "RF",  "RFF", "RFFF", "RFFFF",
                               "LF", "LFF", "LFFF", "LFFFF", "FR",   "FL",    "FFR", "FFL", "FFFR", "FFFL"};

        map<string, int> freq;
        for (int i = 0; i < (int)plain_ops.size(); ++i) {
            for (int L = 1; L <= 5 && i + L <= (int)plain_ops.size(); ++L) {
                string s = plain_ops.substr(i, L);
                if (valid_macro_string(s)) freq[s]++;
            }
        }
        vector<pair<int, string>> cand;
        for (auto &[s, c] : freq) {
            int gain_like = (int)s.size() * c;
            cand.push_back({-gain_like, s});
        }
        sort(cand.begin(), cand.end());
        for (auto &[_, s] : cand) {
            if ((int)base.size() >= 48) break;
            base.push_back(s);
        }

        vector<string> res;
        set<string> seen;
        for (const string &s : base) {
            if (s.empty() || valid_macro_string(s)) {
                if (seen.insert(s).second) res.push_back(s);
            }
        }
        return res;
    }

    pair<Pos, int> apply_move_string(Pos p, int d, const string &s) const {
        for (char ch : s) {
            if (ch == 'L') {
                d = (d + 3) % 4;
            } else if (ch == 'R') {
                d = (d + 1) % 4;
            } else if (ch == 'F') {
                if (can_move(p.r, p.c, d)) {
                    p.r += DR[d];
                    p.c += DC[d];
                }
            }
        }
        return {p, d};
    }

    string shortest_macro_ops_to(Pos target, const vector<string> &macros, int &macro_id) {
        const int C = (int)macros.size();
        const int CELLS = N * N;
        const int SZ = CELLS * 4 * C;
        auto id = [&](int r, int c, int d, int m) { return (((r * N + c) * 4 + d) * C + m); };

        const int INF = INT_MAX / 4;
        vector<int> dist(SZ, INF), par(SZ, -1), act(SZ, -1);
        using Node = pair<int, int>;
        priority_queue<Node, vector<Node>, greater<Node>> pq;
        int start = id(cur.r, cur.c, dir, macro_id);
        dist[start] = 0;
        pq.push({0, start});

        int goal = -1;
        while (!pq.empty()) {
            auto [cd, x] = pq.top();
            pq.pop();
            if (cd != dist[x]) continue;

            int tmp = x;
            int m = tmp % C;
            tmp /= C;
            int d = tmp % 4;
            int cell = tmp / 4;
            int r = cell / N, c = cell % N;
            if (r == target.r && c == target.c) {
                goal = x;
                break;
            }

            auto relax = [&](int y, int w, int a) {
                if (cd + w < dist[y]) {
                    dist[y] = cd + w;
                    par[y] = x;
                    act[y] = a;
                    pq.push({dist[y], y});
                }
            };

            relax(id(r, c, (d + 1) % 4, m), 1, 1);
            relax(id(r, c, (d + 3) % 4, m), 1, 2);
            if (can_move(r, c, d)) relax(id(r + DR[d], c + DC[d], d, m), 1, 0);

            if (!macros[m].empty()) {
                auto [np, nd] = apply_move_string({r, c}, d, macros[m]);
                relax(id(np.r, np.c, nd, m), 1, 3);
            }

            for (int nm = 1; nm < C; ++nm) {
                if (nm == m) continue;
                auto [np, nd] = apply_move_string({r, c}, d, macros[nm]);
                relax(id(np.r, np.c, nd, nm), (int)macros[nm].size() + 2, 4 + nm);
            }
        }

        if (goal == -1) return "";

        vector<int> acts;
        for (int x = goal; x != start; x = par[x]) acts.push_back(act[x]);
        reverse(acts.begin(), acts.end());
        string ops;
        for (int a : acts) {
            if (a == 0)
                ops.push_back('F');
            else if (a == 1)
                ops.push_back('R');
            else if (a == 2)
                ops.push_back('L');
            else if (a == 3)
                ops.push_back('P');
            else {
                const string &s = macros[a - 4];
                ops.push_back('M');
                ops += s;
                ops.push_back('M');
            }
        }

        int tmp = goal;
        macro_id = tmp % C;
        tmp /= C;
        dir = tmp % 4;
        int cell = tmp / 4;
        cur = {cell / N, cell % N};
        return ops;
    }

    string build_macro_state_ops(const vector<int> &order) {
        string plain = build_plain_ops(order);
        vector<string> macros = build_macro_candidates(plain);
        string ops;
        cur = {0, 0};
        dir = 1;
        int macro_id = 0;
        for (int k : order) {
            ops += shortest_macro_ops_to(ball[k], macros, macro_id);
            ops.push_back('S');
            ops += shortest_macro_ops_to(basket[k], macros, macro_id);
            ops.push_back('S');
            if ((int)ops.size() > T) return ops;
        }
        return ops;
    }

    struct MacroChoice {
        string pat;
        vector<int> occ;
        int gain = 0;
    };

    static MacroChoice find_best_macro(const string &ops, int begin) {
        const int n = (int)ops.size();
        MacroChoice best;

        const uint64_t BASE = 911382323ull;
        vector<uint64_t> pow_base(n + 1, 1), pref(n + 1, 0);
        for (int i = 0; i < n; ++i) {
            pow_base[i + 1] = pow_base[i] * BASE;
            pref[i + 1] = pref[i] * BASE + (unsigned char)ops[i] + 1;
        }
        auto hash_range = [&](int l, int r) { return pref[r] - pref[l] * pow_base[r - l]; };

        const int max_l = min(80, n - begin);
        for (int L = 2; L <= max_l; ++L) {
            unordered_map<uint64_t, vector<int>> pos;
            pos.reserve(max(1, n - begin - L + 1) * 2);
            for (int i = begin; i + L <= n; ++i) {
                pos[hash_range(i, i + L)].push_back(i);
            }
            for (auto &[_, all] : pos) {
                if ((int)all.size() < 2) continue;
                vector<int> occ;
                occ.reserve(all.size());
                int last_end = -1;
                for (int p : all) {
                    if (p >= last_end) {
                        occ.push_back(p);
                        last_end = p + L;
                    }
                }
                int c = (int)occ.size();
                if (c < 2) continue;
                int gain = L * c - (L + 2 + (c - 1));
                if (gain > best.gain) {
                    best.gain = gain;
                    best.pat = ops.substr(occ[0], L);
                    best.occ = move(occ);
                }
            }
        }
        return best;
    }

    static string compress_suffix(const string &ops, int begin, int rest_macros) {
        if (begin >= (int)ops.size()) return "";
        if (rest_macros <= 0) return ops.substr(begin);

        MacroChoice choice = find_best_macro(ops, begin);
        if (choice.gain <= 0) return ops.substr(begin);

        string out;
        out.reserve((int)ops.size() - begin - choice.gain);
        vector<char> start(ops.size(), 0);
        for (int p : choice.occ) start[p] = 1;

        bool registered = false;
        int L = (int)choice.pat.size();
        int last_end = choice.occ.back() + L;
        for (int i = begin; i < last_end;) {
            if (start[i]) {
                if (!registered) {
                    out.push_back('M');
                    out += choice.pat;
                    out.push_back('M');
                    registered = true;
                } else {
                    out.push_back('P');
                }
                i += L;
            } else {
                out.push_back(ops[i]);
                ++i;
            }
        }
        out += compress_suffix(ops, last_end, rest_macros - 1);
        return out;
    }

    static string apply_macros(const string &ops, int T) {
        string out = compress_suffix(ops, 0, 3);
        if ((int)out.size() <= T) return out;
        return ops;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Solver solver;
    cin >> solver.N >> solver.M >> solver.T;
    solver.v.resize(solver.N);
    for (int i = 0; i < solver.N; ++i) cin >> solver.v[i];
    solver.h.resize(solver.N - 1);
    for (int i = 0; i + 1 < solver.N; ++i) cin >> solver.h[i];

    solver.ball.resize(solver.M);
    solver.basket.resize(solver.M);
    for (int k = 0; k < solver.M; ++k) {
        cin >> solver.ball[k].r >> solver.ball[k].c >> solver.basket[k].r >> solver.basket[k].c;
    }

    vector<vector<int>> orders = solver.build_orders();
    string out;
    for (int idx = 0; idx < (int)orders.size(); ++idx) {
        const auto &order = orders[idx];
        string ops = solver.build_plain_ops(order);
        if ((int)ops.size() > solver.T) continue;
        string cand = Solver::apply_macros(ops, solver.T);
        if (out.empty() || cand.size() < out.size()) out = move(cand);

        if (idx < 1) {
            string macro_state_ops = solver.build_macro_state_ops(order);
            if ((int)macro_state_ops.size() <= solver.T && (out.empty() || macro_state_ops.size() < out.size())) {
                out = move(macro_state_ops);
            }
        }
    }
    if (out.empty()) {
        string ops = solver.build_plain_ops(orders[0]);
        out = Solver::apply_macros(ops, solver.T);
    }
    cout << out << '\n';
    return 0;
}
