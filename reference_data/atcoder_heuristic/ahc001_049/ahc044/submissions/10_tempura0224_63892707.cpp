// #include <atcoder/all>
#include <bits/stdc++.h>
using ll = long long;
using ull = unsigned long long;
#define rep(i, n) for(int i = 0; i < (int)(n); i++)
#define REP(i, m, n) for(int i = (int)(m); i < (int)(n); i++)
using namespace std;
// using namespace atcoder;
// using mint = modint998244353;
const int inf = 1000000007;
const ll longinf = 1ll << 60;

struct Timer {
    chrono::system_clock::time_point start, last_updated;

    Timer() {
        start = chrono::system_clock::now();
        last_updated = chrono::system_clock::now();
    }

    void reset() {
        start = chrono::system_clock::now();
    }

    void update() {
        last_updated = chrono::system_clock::now();
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

const int N = 100;
const int L = 500000;

int A[N];

void input() {
    int _;
    cin >> _ >> _;
    rep(i, N) cin >> A[i];
}

int used[N], ord[N], low[N];
int dfs(vector<array<int, 2>> &g, int idx, int k, int par, int itr) {
    used[idx] = itr;
    ord[idx] = k++;
    low[idx] = ord[idx];
    int cnt = 0;
    for(auto &to : g[idx]) {
        if(used[to] != itr) {
            ++cnt;
            k = dfs(g, to, k, idx, itr);
            if(k == -1) {
                return -1;
            }
            low[idx] = min(low[idx], low[to]);
            if(ord[idx] < low[to]) {
                return -1;
            }
        } else if(to != par) {
            low[idx] = min(low[idx], ord[to]);
        }
    }
    return k;
}

int check(vector<array<int, 2>> &g, int itr) {
    int k = 0;
    int ret = dfs(g, 0, k, -1, itr);
    if(ret == -1) {
        return false;
    }
    for(int i = 0; i < g.size(); i++) {
        if(used[i] != itr) {
            return false;
        }
    }
    return true;
}

int eval(vector<array<int, 2>> &a) {
    int res = 0;
    int c = 0;
    vector<int> cnt(N);
    rep(i, L) {
        cnt[c]++;
        c = a[c][(cnt[c] + 1) % 2];
    }
    rep(i, N) {
        res += abs(cnt[i] - A[i]);
    }
    return res;
}

inline int f(int x) {
    return x * x;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    input();
    vector<int> target(N);
    rep(i, N) {
        target[i] = A[i] * 2;
    }
    vector<array<int, 2>> g(N);
    vector<int> weight(N, 0);
    rep(i, N) {
        g[i][0] = (i + 1) % N;
        weight[(i + 1) % N] += A[i];
        g[i][1] = rnd.nextInt(0, N);
        weight[g[i][1]] += A[i];
    }
    ll score = 0;
    rep(i, N) {
        weight[i] -= target[i];
        score += f(weight[i]);
    }
    double tl = 1.9;
    double t0 = 500000, t1 = 200;
    double t = t0;
    int itr = 0;
    vector<array<int, 2>> best_g = g;
    vector<int> best_weight = weight;
    int best_score = eval(g);
    int update = 0;
    while(1) {
        if(itr % 512 == 0) {
            double time = timer.getTime() / tl;
            if(time > 1.0)
                break;
            t = pow(t0, 1 - time) * pow(t1, time);
            // t = t0 * (1 - time) + t1 * time;
        }
        int mode = rnd.nextInt(0, 100);
        if(mode < 25) {
            int i = rnd.nextInt(0, N);
            int j = rnd.nextInt(0, 2);
            int x = rnd.nextInt(0, N);
            int prv = g[i][j];
            if(prv == x)
                continue;
            ++itr;
            ll diff = 0;
            diff -= f(weight[prv]);
            diff -= f(weight[x]);
            // weight[prv] -= A[i];
            // weight[x] += A[i];
            diff += f(weight[prv] - A[i]);
            diff += f(weight[x] + A[i]);
            if(diff <= 0 || exp(-diff / t) > rnd.nextDouble(0, 1)) {
                g[i][j] = x;
                auto valid = check(g, itr);
                // LowLink lowlink(g);
                // auto valid = lowlink.build();
                if(valid) {
                    score += diff;
                    weight[prv] -= A[i];
                    weight[x] += A[i];
                    // ++update;
                } else {
                    g[i][j] = prv;
                }
            }

        } else {
            int i1 = rnd.nextInt(0, N);
            int i2 = rnd.nextInt(0, N);
            if(i1 == i2)
                continue;
            ++itr;
            int j1 = rnd.nextInt(0, 2);
            int j2 = rnd.nextInt(0, 2);
            ll diff = 0;
            diff -= f(weight[g[i1][j1]]);
            diff -= f(weight[g[i2][j2]]);
            int d = A[i1] - A[i2];
            // weight[g[i1][j1]] += A[i1] - A[i2];
            // weight[g[i2][j2]] += A[i2] - A[i1];
            diff += f(weight[g[i1][j1]] - d);
            diff += f(weight[g[i2][j2]] + d);
            if(diff <= 0 || exp(-diff / t) > rnd.nextDouble(0, 1)) {
                swap(g[i1][j1], g[i2][j2]);
                auto valid = check(g, itr);
                // LowLink lowlink(g);
                // auto valid = lowlink.build();
                if(valid) {
                    score += diff;
                    weight[g[i1][j1]] += d;
                    weight[g[i2][j2]] -= d;
                    // ++update;
                } else {
                    swap(g[i1][j1], g[i2][j2]);
                }
            }
        }
        if(itr % 220000 == 0) {
            int exact_score = eval(g);
            if(exact_score < best_score) {
                best_score = exact_score;
                best_weight = weight;
                best_g = g;
            } else {
                g = best_g;
                weight = best_weight;
                score = 0;
                rep(i, N) {
                    score += f(weight[i]);
                }
            }
        }
    }
    {
        int exact_score = eval(g);
        if(exact_score < best_score) {
            best_score = exact_score;
            best_weight = weight;
            best_g = g;
        }
    }

    std::cerr << "itr: " << itr << std::endl;
    std::cerr << "update: " << update << std::endl;
    std::cerr << "score: " << score << std::endl;
    int exact_score = eval(best_g);
    std::cerr << "exact_score: " << exact_score << std::endl;
    {
        double tl = 1.95;
        double t0 = 5, t1 = 0.1;
        double t = t0;
        int itr = 0;
        while(1) {
            ++itr;
            if(itr % 2) {
                double time = timer.getTime() / tl;
                if(time > 1.0)
                    break;
                t = pow(t0, 1 - time) * pow(t1, time);
            }
            int i = rnd.nextInt(0, N);
            swap(best_g[i][0], best_g[i][1]);
            int nxt = eval(best_g);
            // cerr << nxt << " ";
            int diff = nxt - exact_score;
            if(diff <= 0 || exp(-diff / t) > rnd.nextDouble(0, 1)) {
                exact_score = nxt;
            } else {
                swap(best_g[i][0], best_g[i][1]);
            }
        }
        std::cerr << "itr: " << itr << std::endl;
        std::cerr << "exact_best_score: " << exact_score << std::endl;
    }
    rep(i, N) {
        cout << best_g[i][0] << " " << best_g[i][1] << endl;
    }
    return 0;
}