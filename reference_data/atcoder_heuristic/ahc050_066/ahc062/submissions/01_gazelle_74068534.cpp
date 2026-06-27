#pragma GCC optimize("Ofast,unroll-loops")
#include <bits/stdc++.h>
using namespace std;

#define FOR(i, n, m) for (int i = n; i < (int)m; ++i)
#define REP(i, n) FOR (i, 0, n)
#define REP_1(i, n) for (int i = 1; i <= (int)n; ++i)
#define RFOR(i, n, m) for (int i = (int)n - 1; i >= (int)m; --i)
#define RREP(i, n) RFOR(i, n, 0)
#define RREP_1(i, n) for (int i = (int)n; i >= 1; --i)

#define ALL(v) v.begin(), v.end()
#define RALL(v) v.rbegin(), v.rend()
#define SIZE(v) (int)v.size()
#define EMPTY(v) v.empty()
#define SORT(v) sort(ALL(v))
#define RSORT(v) sort(RALL(v))
#define REVERSE(v) reverse(ALL(v))
#define UNIQUE(v) (SORT(v), v.erase(unique(ALL(v)), v.end()))

#define PB push_back
#define EB emplace_back
#define MP make_pair

#define YES() cout << "YES\n"
#define NO() cout << "NO\n"
#define Yes() cout << "Yes\n"
#define No() cout << "No\n"
#define YESNO(cond) cout << ((cond) ? "YES" : "NO") << '\n'
#define YesNo(cond) cout << ((cond) ? "Yes" : "No") << '\n'

#define IN(x, a, b) ((a) <= (x) && (x) < (b))
#define BETWEEN(x, a, b) ((a) <= (x) && (x) <= (b))

#define FASTIO() \
    ios::sync_with_stdio(false), cin.tie(nullptr), cout.tie(nullptr)

static int xorshift(int n) {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8))) % n;
}

static bool is_accept(double diff, double temp) {
    return diff >= 0 ||
           (exp((double)diff / temp) * 10000000.0 > xorshift(10000000));
}

constexpr int dx8[] = {-1, -1, -1, 0, 0, 1, 1, 1};
constexpr int dy8[] = {-1, 0, 1, -1, 1, -1, 0, 1};

constexpr int N = 200, NN = N * N;
int A[N * N];
vector<int> adj[N * N];
int route[N * N];
int pos[N * N];
constexpr int B = 256;
long long S1_base[N * N + 1];
long long S1_lazy[(N * N + 1) / B + 2];
long long S2_base[N * N + 1];

inline long long get_S1(int i) { return S1_base[i] + S1_lazy[i >> 8]; }

inline void add_S1_suffix(int L, long long diff) {
    if (diff == 0) return;
    for (int i = L; i <= N * N;) {
        if (i % B == 0 && i + B - 1 <= N * N) {
            S1_lazy[i >> 8] += diff;
            i += B;
        } else {
            S1_base[i] += diff;
            ++i;
        }
    }
}
long long S2_lazy[(N * N + 1) / B + 2];

inline long long get_S2(int i) { return S2_base[i] + S2_lazy[i >> 8]; }

inline void add_S2_suffix(int L, long long diff) {
    if (diff == 0) return;
    for (int i = L; i <= N * N;) {
        if (i % B == 0 && i + B - 1 <= N * N) {
            S2_lazy[i >> 8] += diff;
            i += B;
        } else {
            S2_base[i] += diff;
            ++i;
        }
    }
}

inline bool is_adj(int p1, int p2) {
    int dx = abs((p1 / N) - (p2 / N));
    int dy = abs((p1 % N) - (p2 % N));
    return dx <= 1 && dy <= 1;
}

bool visited[N * N];

int get_degree(int u) {
    int deg = 0;
    for (int v : adj[u]) {
        if (!visited[v]) deg++;
    }
    return deg;
}

void init() {
    int _;
    cin >> _;
    for (int i = 0; i < N * N; ++i) cin >> A[i];

    for (int x = 0; x < N; ++x) {
        for (int y = 0; y < N; ++y) {
            int u = x * N + y;
            for (int d = 0; d < 8; ++d) {
                int nx = x + dx8[d];
                int ny = y + dy8[d];
                if (nx >= 0 && nx < N && ny >= 0 && ny < N) {
                    adj[u].push_back(nx * N + ny);
                }
            }
        }
    }
}

void construct_greedy() {
    bool greedy_success = false;
    for (int t = 0; t < 100; ++t) {
        for (int i = 0; i < N * N; ++i) visited[i] = false;
        int start_node = xorshift(N * N);
        route[0] = start_node;
        visited[start_node] = true;

        bool ok = true;
        for (int step = 1; step < N * N; ++step) {
            int u = route[step - 1];
            int best_v = -1;
            int min_deg = 1e9;
            int min_a = 1e9;

            for (int v : adj[u]) {
                if (!visited[v]) {
                    int deg = get_degree(v);
                    if (deg < min_deg) {
                        min_deg = deg;
                        min_a = A[v];
                        best_v = v;
                    } else if (deg == min_deg) {
                        if (A[v] < min_a ||
                            (A[v] == min_a && xorshift(2) == 0)) {
                            min_a = A[v];
                            best_v = v;
                        }
                    }
                }
            }
            if (best_v == -1) {
                ok = false;
                break;
            }
            route[step] = best_v;
            visited[best_v] = true;
        }
        if (ok) {
            greedy_success = true;
            break;
        }
    }

    if (!greedy_success) {
        int idx = 0;
        for (int i = 0; i < N; ++i) {
            if (i % 2 == 0) {
                for (int j = 0; j < N; ++j) route[idx++] = i * N + j;
            } else {
                for (int j = N - 1; j >= 0; --j) route[idx++] = i * N + j;
            }
        }
    }
}

bool find_2opt(int& best_l, int& best_r, long long& best_diff) {
    int l = xorshift(N * N - 1);
    int cands[10];
    int cands_cnt = 0;

    if (l == 0) {
        cands[cands_cnt++] = N * N - 1;
        for (int v : adj[route[0]]) {
            int p = pos[v];
            if (p > 0) cands[cands_cnt++] = p - 1;
        }
    } else {
        int u = route[l - 1];
        for (int v : adj[u]) {
            int p = pos[v];
            if (p > l) {
                if (p == N * N - 1) {
                    cands[cands_cnt++] = p;
                } else if (is_adj(route[l], route[p + 1])) {
                    cands[cands_cnt++] = p;
                }
            }
        }
    }

    if (cands_cnt == 0) return false;

    long long max_diff = 0;
    bool found = false;

    for (int i = 0; i < cands_cnt; ++i) {
        int r = cands[i];
        long long diff = (long long)(r + l) * (get_S1(r + 1) - get_S1(l)) -
                         2LL * (get_S2(r + 1) - get_S2(l));
        if (!found || diff > max_diff) {
            max_diff = diff;
            best_l = l;
            best_r = r;
            best_diff = diff;
            found = true;
        }
    }

    return found;
}

bool find_shift(int& best_l, int& best_r, int& best_p, bool& best_rev,
                long long& best_diff) {
    int p = xorshift(N * N - 1);

    long long max_diff = 0;
    bool found = false;

    for (int u : adj[route[p]]) {
        int pos_u = pos[u];
        for (int v : adj[route[p + 1]]) {
            int pos_v = pos[v];

            bool rev = (pos_u > pos_v);
            int l = min(pos_u, pos_v);
            int r = max(pos_u, pos_v);

            if (p >= l - 1 && p <= r) continue;

            if (l > 0 && r < N * N - 1 && !is_adj(route[l - 1], route[r + 1]))
                continue;

            long long diff = 0;
            long long L_B = r - l + 1;

            if (p < l) {
                if (!rev) {
                    diff =
                        -(long long)(l - p - 1) * (get_S1(r + 1) - get_S1(l)) +
                        L_B * (get_S1(l) - get_S1(p + 1));
                } else {
                    diff =
                        L_B * (get_S1(l) - get_S1(p + 1)) +
                        (long long)(p + r + 1) * (get_S1(r + 1) - get_S1(l)) -
                        2LL * (get_S2(r + 1) - get_S2(l));
                }
            } else {
                if (!rev) {
                    diff = (long long)(p - r) * (get_S1(r + 1) - get_S1(l)) -
                           L_B * (get_S1(p + 1) - get_S1(r + 1));
                } else {
                    diff = -L_B * (get_S1(p + 1) - get_S1(r + 1)) +
                           (long long)(p + l) * (get_S1(r + 1) - get_S1(l)) -
                           2LL * (get_S2(r + 1) - get_S2(l));
                }
            }

            if (!found || diff > max_diff) {
                max_diff = diff;
                best_l = l;
                best_r = r;
                best_p = p;
                best_rev = rev;
                found = true;
            }
        }
    }

    if (found) {
        best_diff = max_diff;
        return true;
    }
    return false;
}

void apply_2opt(int l, int r, long long diff) {
    reverse(route + l, route + r + 1);
    for (int i = l; i <= r; ++i) pos[route[i]] = i;
    for (int i = l; i <= r; ++i) {
        S1_base[i + 1] = get_S1(i) + A[route[i]] - S1_lazy[(i + 1) >> 8];
        S2_base[i + 1] =
            get_S2(i) + (long long)i * A[route[i]] - S2_lazy[(i + 1) >> 8];
    }
    add_S2_suffix(r + 1, diff);
}

void apply_shift(int l, int r, int p, bool rev, long long diff) {
    int L_B = r - l + 1;
    vector<int> B(L_B);
    for (int i = 0; i < L_B; ++i) B[i] = route[l + i];
    if (rev) reverse(B.begin(), B.end());

    if (p < l) {
        for (int i = l - 1; i >= p + 1; --i) {
            route[i + L_B] = route[i];
            pos[route[i + L_B]] = i + L_B;
        }
        for (int i = 0; i < L_B; ++i) {
            route[p + 1 + i] = B[i];
            pos[B[i]] = p + 1 + i;
        }
    } else {
        for (int i = r + 1; i <= p; ++i) {
            route[i - L_B] = route[i];
            pos[route[i - L_B]] = i - L_B;
        }
        for (int i = 0; i < L_B; ++i) {
            route[p - L_B + 1 + i] = B[i];
            pos[B[i]] = p - L_B + 1 + i;
        }
    }

    int start_idx = min(l, p + 1);
    int end_idx = max(r, p);
    for (int i = start_idx; i <= end_idx; ++i) {
        S1_base[i + 1] = get_S1(i) + A[route[i]] - S1_lazy[(i + 1) >> 8];
        S2_base[i + 1] =
            get_S2(i) + (long long)i * A[route[i]] - S2_lazy[(i + 1) >> 8];
    }
    add_S2_suffix(end_idx + 1, diff);
}

bool find_double_swap(int& best_p1, int& best_p2, int& best_len,
                      long long& best_diff) {
    int l1 = xorshift(N * N);
    if (l1 == 0) return false;

    long long max_diff = 0;
    bool found = false;

    for (int v : adj[route[l1 - 1]]) {
        int l2 = pos[v];
        if (l1 == l2) continue;

        int p1 = min(l1, l2);
        int p2 = max(l1, l2);

        for (int len = 1; len <= 20; ++len) {
            if (p2 + len > N * N) break;
            if (p1 + len > p2) break;

            bool ok = true;
            if (p1 + len == p2) {
                if (p1 > 0 && !is_adj(route[p1 - 1], route[p2])) ok = false;
                if (ok && !is_adj(route[p2 + len - 1], route[p1])) ok = false;
                if (ok && p2 + len < N * N &&
                    !is_adj(route[p1 + len - 1], route[p2 + len]))
                    ok = false;
            } else {
                if (p1 > 0 && !is_adj(route[p1 - 1], route[p2])) ok = false;
                if (ok && !is_adj(route[p2 + len - 1], route[p1 + len]))
                    ok = false;
                if (ok && !is_adj(route[p2 - 1], route[p1])) ok = false;
                if (ok && p2 + len < N * N &&
                    !is_adj(route[p1 + len - 1], route[p2 + len]))
                    ok = false;
            }

            if (!ok) continue;

            long long sumA = get_S1(p1 + len) - get_S1(p1);
            long long sumB = get_S1(p2 + len) - get_S1(p2);
            long long diff = (long long)(p2 - p1) * (sumA - sumB);

            if (!found || diff > max_diff) {
                max_diff = diff;
                best_p1 = p1;
                best_p2 = p2;
                best_len = len;
                found = true;
            }
        }
    }

    if (found) {
        best_diff = max_diff;
    }
    return found;
}

void apply_double_swap(int p1, int p2, int len, long long diff) {
    long long sumA = get_S1(p1 + len) - get_S1(p1);
    long long sumB = get_S1(p2 + len) - get_S1(p2);
    long long diff1 = 0;
    for (int x = 0; x < len; ++x) {
        diff1 += (long long)(p1 + x) * (A[route[p2 + x]] - A[route[p1 + x]]);
    }

    vector<int> temp(len);
    for (int i = 0; i < len; ++i) temp[i] = route[p1 + i];
    for (int i = 0; i < len; ++i) {
        route[p1 + i] = route[p2 + i];
        pos[route[p1 + i]] = p1 + i;
    }
    for (int i = 0; i < len; ++i) {
        route[p2 + i] = temp[i];
        pos[route[p2 + i]] = p2 + i;
    }

    for (int i = p1; i < p1 + len; ++i) {
        S1_base[i + 1] = get_S1(i) + A[route[i]] - S1_lazy[(i + 1) >> 8];
        S2_base[i + 1] =
            get_S2(i) + (long long)i * A[route[i]] - S2_lazy[(i + 1) >> 8];
    }

    add_S1_suffix(p1 + len + 1, sumB - sumA);
    add_S2_suffix(p1 + len + 1, diff1);

    for (int i = p2; i < p2 + len; ++i) {
        S1_base[i + 1] = get_S1(i) + A[route[i]] - S1_lazy[(i + 1) >> 8];
        S2_base[i + 1] =
            get_S2(i) + (long long)i * A[route[i]] - S2_lazy[(i + 1) >> 8];
    }

    add_S1_suffix(p2 + len + 1, -(sumB - sumA));
    add_S2_suffix(p2 + len + 1, diff - diff1);
}

bool find_single_shift(int& best_p, int& best_to, long long& best_diff) {
    int p = xorshift(N * N);

    long long max_diff = 0;
    bool found = false;

    for (int v : adj[route[p]]) {
        int l = pos[v];
        for (int offset = 0; offset <= 1; ++offset) {
            int to = l + offset;
            if (to == 0 || to > N * N) continue;
            if (to == p || to == p + 1) continue;

            bool ok = true;
            if (to < p) {
                if (!is_adj(route[to - 1], route[p])) ok = false;
                if (ok && to < N * N && !is_adj(route[p], route[to]))
                    ok = false;
                if (ok && p > 0 && p < N * N - 1 &&
                    !is_adj(route[p - 1], route[p + 1]))
                    ok = false;
            } else {
                if (!is_adj(route[to - 1], route[p])) ok = false;
                if (ok && to < N * N && !is_adj(route[p], route[to]))
                    ok = false;
                if (ok && p > 0 && p < N * N - 1 &&
                    !is_adj(route[p - 1], route[p + 1]))
                    ok = false;
            }

            if (!ok) continue;

            long long diff = 0;
            if (to < p) {
                diff = -(long long)(p - to) * A[route[p]] +
                       (get_S1(p) - get_S1(to));
            } else {
                diff = (long long)(to - 1 - p) * A[route[p]] -
                       (get_S1(to) - get_S1(p + 1));
            }

            if (!found || diff > max_diff) {
                max_diff = diff;
                best_p = p;
                best_to = to;
                found = true;
            }
        }
    }

    if (found) {
        best_diff = max_diff;
    }
    return found;
}

void apply_single_shift(int p, int to, long long diff) {
    int target = route[p];
    if (to < p) {
        for (int i = p; i > to; --i) {
            route[i] = route[i - 1];
            pos[route[i]] = i;
        }
        route[to] = target;
        pos[target] = to;

        for (int i = to; i <= p; ++i) {
            S1_base[i + 1] = get_S1(i) + A[route[i]] - S1_lazy[(i + 1) >> 8];
            S2_base[i + 1] =
                get_S2(i) + (long long)i * A[route[i]] - S2_lazy[(i + 1) >> 8];
        }
    } else {
        to--;
        for (int i = p; i < to; ++i) {
            route[i] = route[i + 1];
            pos[route[i]] = i;
        }
        route[to] = target;
        pos[target] = to;

        for (int i = p; i <= to; ++i) {
            S1_base[i + 1] = get_S1(i) + A[route[i]] - S1_lazy[(i + 1) >> 8];
            S2_base[i + 1] =
                get_S2(i) + (long long)i * A[route[i]] - S2_lazy[(i + 1) >> 8];
        }
    }
    add_S2_suffix(max(p, to) + 1, diff);
}

void solve() {
    init();
    construct_greedy();

    REP (i, NN) pos[route[i]] = i;

    S1_base[0] = 0;
    S2_base[0] = 0;
    for (int i = 0; i <= (N * N) / B + 1; ++i) {
        S1_lazy[i] = 0;
        S2_lazy[i] = 0;
    }
    for (int i = 0; i < N * N; ++i) {
        S1_base[i + 1] = S1_base[i] + A[route[i]];
        S2_base[i + 1] = S2_base[i] + (long long)i * A[route[i]];
    }

    auto start_time = chrono::high_resolution_clock::now();
    double time_limit = 1.95;

    int iter = 0;
    int accept = 0;
    constexpr double T0 = 10000000.0, T1 = 10000.0;
    double T = T0;

    while (true) {
        if ((iter & 4095) == 0) {
            auto now = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(now - start_time).count();
            if (elapsed > time_limit) break;
            double progress = elapsed / time_limit;
            T = T0 + progress * (T1 - T0);
        }
        iter++;

        static vector<int> ops = {0, 1, 1, 2, 2, 1, 1, 3, 3, 3, 3, 3};
        int op = ops[xorshift(ops.size())];
        long long diff = 0;
        int l, r, p;
        bool rev = false;
        bool found = false;
        int p1, p2, len;
        int to;

        if (op == 0) found = find_2opt(l, r, diff);
        else if (op == 1) found = find_double_swap(p1, p2, len, diff);
        else if (op == 2) found = find_single_shift(p, to, diff);
        else if (op == 3) found = find_shift(l, r, p, rev, diff);
        else assert(false);

        if (found) {
            if (is_accept(diff, T)) {
                if (op == 0) apply_2opt(l, r, diff);
                else if (op == 1) apply_double_swap(p1, p2, len, diff);
                else if (op == 2) apply_single_shift(p, to, diff);
                else if (op == 3) apply_shift(l, r, p, rev, diff);
                else assert(false);

                accept++;
            }
        }
    }

    cerr << "Iter: " << iter << ", Accept: " << accept << endl;
    for (int i = 0; i < N * N; ++i) {
        cout << route[i] / N << " " << route[i] % N << "\n";
    }
}

int main() {
    FASTIO();

    solve();
}