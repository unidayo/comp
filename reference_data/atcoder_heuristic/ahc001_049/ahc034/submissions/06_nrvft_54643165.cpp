#pragma region MACRO
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using vl = vector<ll>;
template <class T>
using vc = vector<T>;
template <class T>
using vvc = vector<vector<T>>;

#define eb emplace_back
#define all(x) (x).begin(), (x).end()
#define rep(i, n) for (int i = 0; i < (n); i++)
#define repr(i, n) for (int i = (n)-1; i >= 0; i--)
#define repe(i, l, r) for (int i = (l); i < (r); i++)
#define reper(i, l, r) for (int i = (r)-1; i >= (l); i--)
#define repa(i, n) for (auto &i : n)

template <class T1, class T2>
inline bool chmax(T1 &a, const T2 &b) {
    if (a < b) {
        a = b;
        return 1;
    }
    return 0;
}
template <class T1, class T2>
inline bool chmin(T1 &a, const T2 &b) {
    if (b < a) {
        a = b;
        return 1;
    }
    return 0;
}

struct init {
    init() {
        cin.tie(0);
        ios::sync_with_stdio(false);
        cout << fixed << setprecision(15);
        cerr << fixed << setprecision(15);
    }
} init_;

template <typename T, typename U>
ostream &operator<<(ostream &out, const pair<T, U> &a) { return out << a.first << ' ' << a.second; }
template <typename T>
ostream &operator<<(ostream &out, const vector<T> &a) {
    for (auto it = a.begin(); it != a.end();) {
        out << *it;
        if (++it != a.end()) out << ' ';
    }
    return out;
}
template <typename T, size_t N>
ostream &operator<<(ostream &out, const array<T, N> &a) {
    for (auto it = a.begin(); it != a.end();) {
        out << *it;
        if (++it != a.end()) out << ' ';
    }
    return out;
}
template <typename T>
ostream &operator<<(ostream &out, const set<T> &a) {
    for (auto it = a.begin(); it != a.end();) {
        out << *it;
        if (++it != a.end()) out << ' ';
    }
    return out;
}
template <typename T, typename U>
ostream &operator<<(ostream &out, const map<T, U> &a) {
    for (auto it = a.begin(); it != a.end();) {
        out << *it;
        if (++it != a.end()) out << '\n';
    }
    return out;
}

#ifdef DEBUG
template <class T, class N>
void verr(const vector<T> &a, const N &n) {
    rep(i, n) cerr << a[i] << " ";
    cerr << endl;
}
template <class T, class N, size_t AN>
void verr(const array<T, AN> &a, const N &n) {
    rep(i, n) cerr << a[i] << " ";
    cerr << endl;
}
ll dbgt = 1;
void err() { cerr << "passed " << dbgt++ << endl; }
template <class H, class... T>
void err(H &&h, T &&...t) {
    cerr << h << (sizeof...(t) ? " " : "\n") << flush;
    if (sizeof...(t) > 0) err(forward<T>(t)...);
}
#else
void err() {}
template <class H, class... T>
void err(H &&h, T &&...t) {}
template <class H, class... T>
void verr(H &&h, T &&...t) {}
#endif

const ll INF = 4e18;
const ld EPS = 1e-11;
const ld PI = acos(-1.0L);
// const ll MOD = 1e9 + 7;
const int MOD = 998244353;
//--------------------------------------------------------------------------------//

inline uint32_t pcg32() {
    static uint64_t x = 0x0123456789012345u;
    unsigned count = (unsigned)(x >> 61);
    x *= 3;
    x ^= x >> 22;
    return (uint32_t)(x >> (22 + count));
}
#pragma endregion

// 時間を出力
chrono::system_clock::time_point startTime, endTime;

// 経過時間(ms) を取得
int get_diff_time() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - startTime).count();
}



// [0, a)の整数乱数
inline int randint(int a) {
    return (long long)pcg32() * a >> 32;
}

// [0, 1]の乱数
inline double randdouble() {
    return pcg32() / (double)numeric_limits<uint32_t>::max();
}

// [0, n) の値の追加・削除・存在確認を時間計算量O(1)で行う簡易set
struct IndexSet {
  private:
    vector<int> que;
    vector<int> pos;

  public:
    IndexSet() = default;
    IndexSet(int n) : pos(n, -1) {}

    void add(int v) noexcept {
        pos[v] = que.size();
        que.push_back(v);
    }

    void remove(int v) noexcept {
        int p = pos[v];
        int b = que.back();
        que[p] = b;
        que.pop_back();
        pos[b] = p;
        pos[v] = -1;
    }

    bool contains(int v) const noexcept {
        return pos[v] != -1;
    }

    int size() const noexcept {
        return que.size();
    }

    int select(int k) const noexcept {
        assert(k < que.size());
        return que[k];
    }

    vector<int> get_all() const noexcept {
        return que;
    }
};


using Score = ll;
// 固定パラメータ
constexpr int TIME_LIMIT = 1900;

constexpr int LOG_NUM = 10000;
array<double, LOG_NUM> log_table;
void init_log_table() {
    rep(i, LOG_NUM) log_table[i] = log(randdouble());
}

double get_time_progress() {
    return get_diff_time() / (double)TIME_LIMIT;
}

// グローバルパラメータ
constexpr int N = 20;
constexpr int K = 100;
vc<int> DI = {0, 1, 0, -1, 0};
vc<char> DC = {'R', 'D', 'L', 'U'};
array<int, N * N> H;

// ハイパーパラメータ ---------------------------------
int ZERO = 0;
constexpr int inf = 1e9;

constexpr double START_TEMP = 100, END_TEMP = 0.1;

int get_dist(const int id1, const int id2) {
    const int x1 = id1 / N, y1 = id1 % N;
    const int x2 = id2 / N, y2 = id2 % N;
    return abs(x1 - x2) + abs(y1 - y2);
}

struct State {
    vc<int> order;
    IndexSet is1, is2;
    array<int, 10 * N * N> cum;
    array<pair<int, int>, 10 * N * N> give;

    Score score;
    vc<string> ans;

    // 構築用

    State(){
        order = vc<int>(N * N);
        iota(all(order), 0);
        // TODO: 0のマスが不要だったら消す
        rep(i, N) {
            if (i % 2 == 0) continue;
            reverse(order.begin() + i * N, order.begin() + (i + 1) * N);
        }

        is1 = IndexSet(N * N);
        is2 = IndexSet(N * N);
        rep(i, N * N) is1.add(i);

        EvaluateAll();
    }

    State(vc<int> r){
        order = r;

        EvaluateAll();
    }

    void Insert(int id, int pos){
        order.insert(order.begin() + pos, id);
    }

    void Move(int id1, int id2) {
        int y1 = id1 / N, x1 = id1 % N;
        int y2 = id2 / N, x2 = id2 % N;
        rep(i, x2 - x1) ans.eb("R");
        rep(i, y2 - y1) ans.eb("D");
        rep(i, x1 - x2) ans.eb("L");
        rep(i, y1 - y2) ans.eb("U");
    }

    void Load(int d) {
        assert(d > 0);
        ans.eb("+" + to_string(d));
    }

    void Unload(int d) {
        assert(d > 0);
        ans.eb("-" + to_string(d));
    }

    void EvaluateAll(bool out = false){
        score = 0;
        const int M = order.size();

        cum[0] = 0;
        rep(i, M) cum[i + 1] = cum[i] + H[order[i]];
        // int minId = -1;
        // {
        //     int minSum = inf;
        //     rep(i, M) {
        //         if(chmin(minSum, cum[i + 1])) {
        //             minId = i;
        //         }
        //     }
        // }
        // assert(minId >= 0);

        // if(minId < M - 1) {
        //     rotate(order.begin(), order.begin() + minId + 1, order.end());
        // }
        if(out) ans.clear();

        vc<int> C(N * N);
        assert(M == N * N);
        rep(i, M) {
            C[i] = H[i];
        }

        int fromId = 0;
        rep(i, M) {
            int id = order[i];
            int dist = get_dist(fromId, id);

            if(H[id] > 0) {
                if(cum[i + 1] > 0) {
                    // ここまで運んできた量
                    int kd = max(0, cum[i + 1] - H[id]);
                    // 積む量
                    int d = min(cum[i + 1], H[id]);
                    score += dist * (K + kd);
                    if(out) {
                        Move(fromId, id);
                        Load(d);
                    }
                    fromId = id;
                }
            }else if(H[id] < 0) {
                if(cum[i + 1] > H[id]) {
                    // ここまで運んできた量
                    int kd = cum[i + 1] - H[id];
                    // 降ろす量
                    int d = min(abs(H[id]), cum[i + 1] - H[id]);
                    score += dist * (K + kd);
                    if(out) {
                        Move(fromId, id);
                        Unload(d);
                    }
                    fromId = id;
                }
            }
        }

        if(fromId != order[M - 1]) {
            score += get_dist(fromId, order[M - 1]) * K;
            if(out) {
                Move(fromId, order[M - 1]);
                if(H[order[M - 1]] > 0) {
                    Load(H[order[M - 1]]);
                }else if(H[order[M - 1]] < 0) {
                    Unload(abs(H[order[M - 1]]));
                }
            }
            fromId = order[M - 1];
        }

        repr(i, M - 1) {
            int id = order[i];
            int dist = get_dist(fromId, id);

            if(H[id] > 0) {
                if(cum[i + 1] < H[id]) {
                    int kd = max(0, -cum[i + 1]);
                    int d = min(H[id], H[id] - cum[i + 1]);
                    score += dist * (K + kd);
                    if(out) {
                        Move(fromId, id);
                        Load(d);
                    }
                    fromId = id;
                }
            }else if(H[id] < 0) {
                if(cum[i + 1] < 0) {
                    int kd = abs(cum[i + 1]);
                    int d = min(abs(cum[i + 1]), -H[id]);
                    score += dist * (K + kd);
                    if(out) {
                        Move(fromId, id);
                        Unload(d);
                    }
                    fromId = id;
                }
            }
        }

        // err("score:", score);
    }

    vc<string> Output(){
        return ans;
    };
};

struct Action2opt {
    Action2opt(){}

    Score preScore;

    void Run(State &s, const double threshold){
        preScore = s.score;

        int l = randint(s.order.size());
        int r = randint(s.order.size());
        while(l == r) r = randint(s.order.size());
        if(l > r) swap(l, r);
        r++;

        reverse(s.order.begin() + l, s.order.begin() + r);
        s.EvaluateAll();

        int scoreDiff = s.score - preScore;
        err(scoreDiff);

        if(s.score - preScore <= threshold) {

        }else{
            // ロールバック
            reverse(s.order.begin() + l, s.order.begin() + r);
            s.EvaluateAll();
        }
    }
};

struct ActionSwap {
    ActionSwap(){}

    Score preScore;

    void Run(State &s, const double threshold){
        preScore = s.score;

        int l = randint(s.order.size() - 1);

        swap(s.order[l], s.order[l + 1]);

        s.EvaluateAll();

        if(s.score - preScore <= threshold) {

        }else{
            // ロールバック
            swap(s.order[l], s.order[l + 1]);
            s.EvaluateAll();
        }
    }
};

State pickBestRoute() {
    State ret;
    {
        vc<tuple<int, int, int>> starts = {{0, 0, 0}, {0, N - 1, 1}, {N - 1, N - 1, 2}, {N - 1, 0, 3}};
        for(auto[sh, sw, sd]: starts) {
            vc<int> r;
            // ぐるぐる
            int h = sh, w = sw, d = sd;
            vc<int> used(N * N);
            r.eb(h * N + w);
            used[h * N + w] = 1;
            // 衝突したら右に向かう
            while (true) {
                int th = h + DI[d], tw = w + DI[d + 1];
                if (th < 0 || th >= N || tw < 0 || tw >= N || used[th * N + tw]) {
                    d = (d + 1) % 4;
                    th = h + DI[d], tw = w + DI[d + 1];
                    if(th < 0 || th >= N || tw < 0 || tw >= N || used[th * N + tw]) break;
                    continue;
                }
                h = th, w = tw;
                used[h * N + w] = 1;
                r.eb(h * N + w);
            }
            State s(r);
            if(s.score < ret.score) ret = s;
            reverse(all(r));
            if(s.score < ret.score) ret = s;
        }
    }
    {
        // 逆ぐるぐる
        vc<tuple<int, int, int>> starts = {{0, 0, 1}, {0, N - 1, 2}, {N - 1, N - 1, 3}, {N - 1, 0, 0}};
        for(auto [sh, sw, sd]: starts) {
            vc<int> r;
            int h = sh, w = sw, d = sd;
            vc<int> used(N * N);
            used[h * N + w] = 1;
            r.eb(h * N + w);
            // 衝突したら左に向かう
            while (true) {
                int th = h + DI[d], tw = w + DI[d + 1];
                if (th < 0 || th >= N || tw < 0 || tw >= N || used[th * N + tw]) {
                    d = (d + 3) % 4;
                    th = h + DI[d], tw = w + DI[d + 1];
                    if(th < 0 || th >= N || tw < 0 || tw >= N || used[th * N + tw]) break;
                    continue;
                }
                h = th, w = tw;
                used[h * N + w] = 1;
                r.eb(h * N + w);
            }
            State s(r);
            if(s.score < ret.score) ret = s;
            reverse(all(r));
            if(s.score < ret.score) ret = s;
        }
    }
    {
        // 縦しま
        // vc<int> L = {1, 2, 4, 5, 10, 20};
        // repa(lw, L) {
        //     vc<int> r;
        //     rep(d, N / lw) {

        //     }
        //     State s(r);
        //     if(s.score > ret.score) ret = s;
        // }
        vc<int> r;
        rep(j, N) {
            rep(i, N) {
                r.eb(i * N + j);
            }
        }
        rep(i, N) {
            if (i % 2 == 0) continue;
            reverse(r.begin() + i * N, r.begin() + (i + 1) * N);
        }
        State s(r);
        if(s.score < ret.score) ret = s;
        reverse(all(r));
        if(s.score < ret.score) ret = s;
    }
    {
        // 横しま
        vc<int> r;
        rep(i, N) {
            rep(j, N) {
                r.eb(i * N + j);
            }
        }
        rep(j, N) {
            if (j % 2 == 0) continue;
            reverse(r.begin() + j * N, r.begin() + (j + 1) * N);
        }
        State s(r);
        if(s.score < ret.score) ret = s;
        reverse(all(r));
        if(s.score < ret.score) ret = s;
    }
    {
        // くの字
        vc<tuple<int, int, int>> starts = {{0, 0, 0}, {0, N - 1, 1}, {N - 1, N - 1, 2}, {N - 1, 0, 3}};
        for(auto[sh, sw, sd]: starts) {
            vc<int> r;
            int h = sh, w = sw;
            rep(i, 10) {
                int d = sd;
                r.eb(h * N + w);
                rep(k, N - 2 * i - 1) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 1) % 4;
                rep(k, N - 2 * i - 1) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 1) % 4;
                h += DI[d], w += DI[d + 1];
                r.eb(h * N + w);
                d = (d + 1) % 4;
                rep(k, N - 2 * i - 2) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 3) % 4;
                rep(k, N - 2 * i - 2) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 3) % 4;
                h += DI[d], w += DI[d + 1];
                d = (d + 3) % 4;
            }
            State s(r);
            if(s.score < ret.score) ret = s;
            reverse(all(r));
            if(s.score < ret.score) ret = s;
        }
    }
    {
        // くの字
        vc<tuple<int, int, int>> starts = {{0, 0, 1}, {0, N - 1, 2}, {N - 1, N - 1, 3}, {N - 1, 0, 0}};
        for(auto[sh, sw, sd]: starts) {
            vc<int> r;
            int h = sh, w = sw;
            rep(i, 10) {
                int d = sd;
                r.eb(h * N + w);
                rep(k, N - 2 * i - 1) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 3) % 4;
                rep(k, N - 2 * i - 1) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 3) % 4;
                h += DI[d], w += DI[d + 1];
                r.eb(h * N + w);
                d = (d + 3) % 4;
                rep(k, N - 2 * i - 2) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 1) % 4;
                rep(k, N - 2 * i - 2) {
                    h += DI[d], w += DI[d + 1];
                    r.eb(h * N + w);
                }
                d = (d + 1) % 4;
                h += DI[d], w += DI[d + 1];
                d = (d + 1) % 4;
            }
            State s(r);
            if(s.score < ret.score) ret = s;
            reverse(all(r));
            if(s.score < ret.score) ret = s;
        }
    }
    return ret;
}

State simulated_annealing(){
    State s = pickBestRoute();
    State bestState = s;
    // return bestState;

    Score firstScore = s.score;

    Action2opt change2opt;
    ActionSwap changeSwap;

    constexpr int Coef2opt = 5;
    constexpr int CoefSwap = 5;

    vc<int> CoefAcc = {Coef2opt, CoefSwap};
    rep(i, CoefAcc.size() - 1) CoefAcc[i + 1] += CoefAcc[i];

    int CoefSum = CoefAcc.back();


    double temp = START_TEMP;
    int sa_count = 0;
    int diff_time = get_diff_time();

    while (true) {
        if((sa_count & 511) == 511) {
            diff_time = get_diff_time();
            if (diff_time > TIME_LIMIT) break;
            auto progress = get_time_progress();
            temp = START_TEMP * pow(END_TEMP / START_TEMP,  get_time_progress());
        }

        // double threshold = temp * log_table[randint(LOG_NUM)];
        double threshold = 0;

        int randv = randint(CoefSum);
        if (randv < CoefAcc[0]) {
            change2opt.Run(s, threshold);
        }else if(randv < CoefAcc[1]) {
            changeSwap.Run(s, threshold);
        }

        if(diff_time > TIME_LIMIT - TIME_LIMIT / 20) {
            if (s.score < bestState.score) bestState = s;
        }
        sa_count++;
    }

    err("sa loops:", sa_count);
    err("final score:", bestState.score);
    err("改善量:", firstScore - bestState.score);

    return bestState;
}


void input(){
    int n_;
    cin >> n_;
    rep(i, N) rep(j, N) {
        cin >> H[i * N + j];
    }
}

void assert_ans(vc<string> ans){
    int x = 0, y = 0;
    repa(s, ans) {
        if (s.size() >= 2) continue;
        if (s == "R") {
            x++;
        } else if (s == "D") {
            y++;
        } else if (s == "L") {
            x--;
        } else if (s == "U") {
            y--;
        }
        assert(0 <= x and x < N and 0 <= y and y < N);
    }
}

void solve() {
    input();

    auto bestState = simulated_annealing();
    // auto bestState = Greedy();
    bestState.EvaluateAll(true);
    auto ans = bestState.Output();
    assert_ans(ans);
    // cout << ans.size() << '\n';
    repa(a, ans) cout << a << '\n';
}

void run(){
    solve();
}

int main() {
    startTime = chrono::system_clock::now();
#ifdef TUNE
    // params
#endif
    run();

    return 0;
}