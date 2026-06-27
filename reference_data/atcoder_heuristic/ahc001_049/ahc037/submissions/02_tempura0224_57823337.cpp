#include <atcoder/all>
#include <bits/stdc++.h>
using ll = long long;
using ull = unsigned long long;
#define rep(i, n) for(int i = 0; i < (int)(n); i++)
#define REP(i, m, n) for(int i = (int)(m); i < (int)(n); i++)
using namespace std;
using namespace atcoder;
using mint = modint998244353;
const int inf = 1000000007;
const ll longinf = 1ll << 60;

struct Timer {
    chrono::system_clock::time_point start;

    Timer() {
        start = chrono::system_clock::now();
    }

    void reset() {
        start = chrono::system_clock::now();
    }

    double getTime() {
        auto now = chrono::system_clock::now();
        return chrono::duration<double>(now - start).count();
    }
};

Timer timer;

struct Xor128 {
    unsigned x, y, z, w;

    Xor128() : x(123456789), y(362436069), z(521288629), w(88675123){};

    inline unsigned xor128() {
        unsigned t;
        t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }

    int nextInt(int x, int y) {
        return xor128() % (y - x) + x;
    }

    double nextDouble(double a, double b) {
        return (double)(xor128() & 0xffff) / 0xffff * (b - a) + a;
    }
};
auto rnd = Xor128();

const int N = 1000;
int A[N], B[N];

vector<ll> dist(N);
vector<vector<int>> xs(N), ys(N);
vector used(N, vector(N, -1));

ll op(ll x, ll y) {
    return max(x, y);
}

ll e() {
    return 0;
}

segtree<ll, op, e> sg(N);

inline ll calc_dist(int fx, int fy, int tx, int ty) {
    if(fx <= tx && fy <= ty) {
        return A[tx] - A[fx] + B[ty] - B[fy];
    } else {
        return longinf;
    }
}

pair<vector<int>, vector<int>> input() {
    int _;
    cin >> _;
    vector<int> a(N), b(N);
    rep(i, N) {
        cin >> a[i] >> b[i];
        A[i] = a[i];
        B[i] = b[i];
    }
    sort(A, A + N);
    sort(B, B + N);
    rep(i, N) {
        a[i] = lower_bound(A, A + N, a[i]) - A;
        b[i] = lower_bound(B, B + N, b[i]) - B;
    }
    return {a, b};
}

ll solve(vector<int> &a, vector<int> &b) {
    int n = a.size();
    vector<int> ord(n);
    rep(i, n) ord[i] = i;
    sort(ord.begin(), ord.end(), [&](int i, int j) {
        return a[i] < a[j];
    });
    ll score = 0;
    rep(oi, n) {
        int i = ord[oi];
        ll mi = A[a[i]] + B[b[i]];
        int x = 0, y = 0;
        rep(oj, oi) {
            int j = ord[oj];
            if(a[j] <= a[i] && b[j] <= b[i] && (A[a[i]] - A[a[j]] + B[b[i]] - B[b[j]]) < mi) {
                mi = A[a[i]] - A[a[j]] + B[b[i]] - B[b[j]];
                x = a[j];
                y = b[j];
            }
        }
        score += mi;
        dist[i] = mi;
    }
    return score;
}

pair<ll, int> try_add(vector<int> &a, vector<int> &b, int x, int y, int mode) {
    int n = a.size();
    ll diff = 0;
    ll mi = calc_dist(0, 0, x, y);
    int xcnt = 0, ycnt = 0;
    for(auto e : xs[x]) {
        if(e >= y) {
            int p = used[x][e];
            ll d = calc_dist(x, y, x, e);
            if(d < dist[p]) {
                diff += dist[p] - d;
            }
            if(d < dist[p] * 1.6) {
                xcnt = 1;
            }
        }
    }
    for(auto e : ys[y]) {
        if(e >= x) {
            int p = used[e][y];
            ll d = calc_dist(x, y, e, y);
            if(d < dist[p]) {
                diff += dist[p] - d;
            }
            if(d < dist[p] * 1.6) {
                ycnt = 1;
            }
        }
    }
    ll ma = mode == 0 ? sg.prod(0, y + 1) : sg.prod(0, x + 1);
    ll d = A[x] + B[y] - ma;
    diff -= d;
    return {diff, xcnt + ycnt};
}
void do_add(vector<int> &a, vector<int> &b, int x, int y) {
    int n = a.size();
    ll diff = 0;
    ll mi = calc_dist(0, 0, x, y);
    for(auto e : xs[x]) {
        if(e >= y) {
            int p = used[x][e];
            dist[p] = min(dist[p], calc_dist(x, y, x, e));
        }
    }
    for(auto e : ys[y]) {
        if(e >= x) {
            int p = used[e][y];
            dist[p] = min(dist[p], calc_dist(x, y, e, y));
        }
    }
    rep(i, n) {
        if(a[i] <= x && b[i] <= y) {
            mi = min(mi, calc_dist(a[i], b[i], x, y));
        }
    }
    dist.push_back(mi);
    a.push_back(x);
    b.push_back(y);
    xs[x].push_back(y);
    ys[y].push_back(x);
    sort(xs[x].begin(), xs[x].end());
    sort(ys[y].begin(), ys[y].end());
}

void output(vector<int> &a, vector<int> &b) {
    int n = a.size();
    vector<int> ord(n);
    rep(i, n) ord[i] = i;
    sort(ord.begin(), ord.end(), [&](int i, int j) {
        return a[i] == a[j] ? b[i] < b[j] : a[i] < a[j];
    });
    ll score = 0;
    int del = 0;
    rep(i, n) {
        if(a[i] == 0 && b[i] == 0) {
            ++del;
        }
    }
    cout << n - del << endl;
    rep(oi, n) {
        int i = ord[oi];
        if(a[i] == 0 && b[i] == 0) {
            continue;
        }
        ll x = 0, y = 0;
        ll mi = A[a[i]] + B[b[i]];
        rep(oj, oi) {
            int j = ord[oj];
            if(a[j] <= a[i] && b[j] <= b[i] && (A[a[i]] - A[a[j]] + B[b[i]] - B[b[j]]) < mi) {
                mi = A[a[i]] - A[a[j]] + B[b[i]] - B[b[j]];
                x = a[j];
                y = b[j];
            }
        }
        score += mi;
        cout << A[x] << " " << B[y] << " " << A[a[i]] << " " << B[b[i]] << endl;
    }
    cerr << score << endl;
}

bool is_needed(int x, int y) {
    int xcnt = 0, ycnt = 0;
    for(auto e : xs[x]) {
        if(e > y) {
            int p = used[x][e];
            ll d = calc_dist(x, y, x, e);
            if(d == dist[p]) {
                xcnt = 1;
            }
        }
    }
    for(auto e : ys[y]) {
        if(e > x) {
            int p = used[e][y];
            ll d = calc_dist(x, y, e, y);
            if(d == dist[p]) {
                ycnt = 1;
            }
        }
    }
    return xcnt >= 1 && ycnt >= 1;
}

void remove(vector<int> &a, vector<int> &b, int idx) {
    int x = a[idx], y = b[idx];
    for(auto e : xs[x]) {
        if(e > y) {
            int p = used[x][e];
            ll d = calc_dist(x, y, x, e);
            if(d == dist[p]) {
                dist[p] = longinf;
                rep(i, a.size()) {
                    if(i != idx && i != p) {
                        dist[p] = min(dist[p], calc_dist(a[i], b[i], a[p], b[p]));
                    }
                }
            }
        }
    }
    for(auto e : ys[y]) {
        if(e > x) {
            int p = used[e][y];
            ll d = calc_dist(x, y, e, y);
            if(d == dist[p]) {
                dist[p] = longinf;
                rep(i, a.size()) {
                    if(i != idx && i != p) {
                        dist[p] = min(dist[p], calc_dist(a[i], b[i], a[p], b[p]));
                    }
                }
            }
        }
    }
    a[idx] = b[idx] = 0;
    vector<int> nxs, nys;
    for(auto e : xs[x]) {
        if(e != y) {
            nxs.push_back(e);
        }
    }
    for(auto e : ys[y]) {
        if(e != x) {
            nys.push_back(e);
        }
    }
    xs[x] = nxs;
    ys[y] = nys;
    dist[idx] = 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    auto _p = input();
    vector<int> a = _p.first, b = _p.second;

    rep(i, N) {
        used[a[i]][b[i]] = i;
        xs[a[i]].push_back(b[i]);
        ys[b[i]].push_back(a[i]);
    }
    ll ans = solve(a, b);
    int t2 = 0;
    rep(_, 5) {
        for(int s = 2 * N - 2; s >= 0; s--) {
            rep(i, N) {
                sg.set(i, 0);
            }
            rep(x, N) {
                for(auto y : xs[x]) {
                    if(sg.get(y) < A[x] + B[y]) {
                        sg.set(y, A[x] + B[y]);
                    }
                }
                int y = s - x;
                if(y < 0 || y >= N) {
                    continue;
                }
                // int x = rnd.nextInt(0, N), y = rnd.nextInt(0, N);
                if(used[x][y] >= 0) {
                    if(used[x][y] >= N && !is_needed(x, y)) {
                        remove(a, b, used[x][y]);
                        used[x][y] = -1;
                    }
                    continue;
                }
                // if(cnt[x][y] < _) {
                //     continue;
                // }
                auto [diff, cnt] = try_add(a, b, x, y, 0);
                // cout << ret << " " << ans << endl;
                if(diff > 0) {
                    used[x][y] = a.size();
                    do_add(a, b, x, y);
                    ans -= diff;
                }
            }
        }
    }

    rep(_, 12) {
        for(int s = 2 * N - 2; s >= 0; s--) {
            if(timer.getTime() > 1.95) {
                break;
            }
            rep(i, N) {
                sg.set(i, 0);
            }
            rep(x, N) {
                for(auto y : xs[x]) {
                    if(sg.get(y) < A[x] + B[y]) {
                        sg.set(y, A[x] + B[y]);
                    }
                }
                int y = s - x;
                if(y < 0 || y >= N) {
                    continue;
                }
                if(used[x][y] >= 0) {
                    if(used[x][y] >= N && !is_needed(x, y)) {
                        remove(a, b, used[x][y]);
                        used[x][y] = -1;
                    }
                    continue;
                }
                // if(cnt[x][y] < _) {
                //     continue;
                // }
                auto [diff, cnt] = try_add(a, b, x, y, 0);
                // cout << ret << " " << ans << endl;
                if(diff > 0 || (diff == 0 && cnt >= 2)) {
                    used[x][y] = a.size();
                    do_add(a, b, x, y);
                    ans -= diff;
                }
            }
        }
    }

    output(a, b);
    return 0;
}