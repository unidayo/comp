#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("inline")
// #pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,mmx,tune=native")

#ifdef LOCAL
//
#else
#define NDEBUG
#endif

#include <bits/stdc++.h>
using namespace std;
// #include <atcoder/all>
// using namespace atcoder;

typedef long long ll;
// typedef long double ld;
typedef double ld;

template<class T> using V = vector<T>;
template<class T> using VV = V<V<T>>;
template<class T> using VVV = V<VV<T>>;

template<class T> using US = unordered_set<T>;
template<class T, class U> using UM = unordered_map<T, U>;

template<class T> using PQMAX = priority_queue<T, V<T>, less<T>>; // 降順
template<class T> using PQMIN = priority_queue<T, V<T>, greater<T>>; // 昇順

#define overload4(_1,_2,_3,_4,name,...) name
#define REP1(n) for(ll i=0;i<n;i++)
#define REP2(i,n) for(ll i=0;i<n;i++)
#define REP3(i,a,b) for(ll i=a;i<b;i++)
#define REP4(i,a,b,c) for(ll i=a;i<b;i+=c)
// 1: 0->n-1, 2: i,0->n-1, 3: i,a->b-1, 4: i,a->b-1,c
#define REP(...) overload4(__VA_ARGS__, REP4, REP3, REP2, REP1)(__VA_ARGS__)
#define RREP1(n) for(ll i=n;i>0;i--)
#define RREP2(i,n) for(ll i=n;i>0;i--)
#define RREP3(i,a,b) for(ll i=a;i>b;i--)
#define RREP4(i,a,b,c) for(ll i=a;i>b;i-=c)
// 1: n->1, 2: i,n->1, 3: i,a->b+1, 4: i,a->b+1,c
#define RREP(...) overload4(__VA_ARGS__, RREP4, RREP3, RREP2, RREP1)(__VA_ARGS__)
#define REPA(i, a) for(auto&&i:a)
#define ALL(x) x.begin(), x.end()
#define SZ(x) (int)x.size()
#define MIN(x) *min_element(ALL(x))
#define MAX(x) *max_element(ALL(x))
#define ARGMIN(x) distance(x.begin(),min_element(ALL(x)))
#define ARGMAX(x) distance(x.begin(),max_element(ALL(x)))

template<class T, class U>bool chmax(T &a, const U &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T, class U>bool chmin(T &a, const U &b) { if (b<a) { a=b; return 1; } return 0; }
template<class... T>constexpr auto min(T... a){ return min(initializer_list<common_type_t<T...>>{a...}); }
template<class... T>constexpr auto max(T... a){ return max(initializer_list<common_type_t<T...>>{a...}); }

template<class... T>void input(T&... a){ (cin >> ... >> a); }
template<class T>void inputv(int n, V<T> &A){ A.resize(n); REP(i,n) cin >> A[i]; };
template<class T>void inputvv(int n, int m, V<V<T>> &A){ A.resize(n, V<T>(m)); REP(i,n) REP(j,m) cin >> A[i][j]; };

void print(){ cout << endl; }
template<class T>void print(const T &a) { cout << a << endl; }
template<class T, class... Ts>void print(const T &a, const Ts &...b){ cout << a; (cout << ... << (cout << ' ', b)); cout << endl; }
void debug(){ cerr << endl; }
template<class T>void debug(const T &a) { cerr << a << endl; }
template<class T, class... Ts>void debug(const T &a, const Ts &...b){ cerr << a; (cerr << ... << (cerr << ' ', b)); cerr << endl; }

template<class T>void print(const V<T> &a) { REP(i,SZ(a)) {cout << a[i]; if (i < SZ(a)-1) cout << ' '; } cout << endl; }
template<class T>void debug(const V<T> &a) { REP(i,SZ(a)) {cerr << a[i]; if (i < SZ(a)-1) cerr << ' '; } cerr << endl; }

template <class T> V<T> make_vector(const size_t &a, const T &b) { return V<T>(a, b); }
template <class... Ts> auto make_vector(const size_t &a, const Ts &...b) { return V<decltype(make_vector(b...))>(a, make_vector(b...)); }

constexpr int inf = 1001001001;
constexpr ll INF = 1001001001001001001;

struct Rand{

    private:
    static uint32_t y;
    void xorshift() {
        this->y ^= (this->y << 13);
        this->y ^= (this->y >> 17);
        this->y ^= (this->y << 5);
    }

    public:
    Rand() { if (this->y == 0) this->y = 2463534242; }

    int randint(int n){
        assert(0 < n);
        this->xorshift();
        return this->y % n;
    }
    int randint(int l, int r){
        assert(l < r);
        this->xorshift();
        return l + this->y % (r - l);
    }
    ld random(){
        this->xorshift();
        return (ld) this->y / 4294967295;
    }
    ld random(int n){
        assert(0 < n);
        this->xorshift();
        return (ld) this->y / 4294967295 * n;
    }
    ld random(int l, int r){
        assert(l < r);
        this->xorshift();
        return (ld) this->y / 4294967295 * (r - l) + l;
    }
    ld rand_normal(){
        ld u1 = random(), u2 = random();
        return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    }
    ld rand_normal(ld mean, ld sigma){
        return mean + sigma * rand_normal();
    }
    // 確率 p で true, 1-p で false
    bool rand_bool(ld p) {
        return (random() <= p);
    }
    // 0 以上 n 未満の相異なる k 個の整数
    V<int> rand_distinct(int n, int k) {
        assert(n >= k);
        unordered_set<int> done;
        REP(i, n - k, n) {
            int t = this->randint(i + 1);
            if (done.count(t) > 0) {
                done.emplace(i);
            } else {
                done.emplace(t);
            }
        }
        V<int> res;
        res.reserve(k);
        REPA(x, done) {
            res.emplace_back(x);
        }
        shuffle(res);
        return res;
    }
    // l 以上 r 未満の相異なる k 個の整数
    V<int> rand_distinct(int l, int r, int k) {
        int n = r - l;
        V<int> res0 = rand_distinct(n, k);
        V<int> res;
        res.reserve(k);
        REPA(x, res0) {
            res.emplace_back(l + x);
        }
        return res;
    }
    
    template<typename T>
    void shuffle(V<T> &a){
        int n = SZ(a);
        REP(i, n){
            swap(a[i], a[this->randint(i, n)]);
        }
    }

    // ratio に従う比率で整数を生成
    int randint_ratio(const V<ld>& ratio) {
        assert(!ratio.empty());
        ld s = accumulate(ALL(ratio), 0.0);
        assert(s > 0);
        ld r = this->random() * s;
        ld cs = 0.0;
        REP(i, SZ(ratio)) {
            cs += ratio[i];
            if (r < cs) return i;
        }
        return SZ(ratio) - 1;
    }
};

uint32_t Rand::y = 0;

struct Timer{

    private:
    chrono::high_resolution_clock::time_point start_time;
    chrono::high_resolution_clock::time_point end_time;

    public:
    Timer() : 
    start_time(chrono::high_resolution_clock::now()),
    end_time(chrono::high_resolution_clock::now()) {}

    void start(int limit){
        this->start_time = chrono::high_resolution_clock::now();
        this->end_time = this->start_time + chrono::milliseconds(limit);
    }
    bool is_timeover(){
        return chrono::high_resolution_clock::now() >= this->end_time;
    }
    int elapsed_time(){
        return (int)chrono::duration_cast<chrono::milliseconds> (chrono::high_resolution_clock::now() - this->start_time).count();
    }
    ld elapsed_rate(){
        return (chrono::high_resolution_clock::now() - this->start_time).count() / (ld)(this->end_time - this->start_time).count();
    }
};

int dy[4] = {1, 0, -1, 0}, dx[4] = {0, 1, 0, -1};


int N = 40, M;
VV<bool> S0(40, V<bool>(40, 0));
void input_data() {
    input(N, M);
    REP(i, N) {
        string s;
        input(s);
        REP(j, N) {
            if (s[j] == '#') {
                S0[i][j] = 1;
            }
        }
    }
}

void output(V<pair<int, int>> &ans) {
    REP(i, SZ(ans)) {
        print(ans[i].first, ans[i].second);
    }
}


// 確率行列
VV<ld> probs_0(40, V<ld>(40, 0)); // 初期値, 固定

// 確率行列初期化
void init_probs() {
    REP(y, N) REP(x, N) {
        if (S0[y][x]) continue;
        probs_0[y][x] = 1.0 / (N * N - M);
    }
    return;
}


// 停止位置
VVV<pair<int, int>> stop_pos_0(40, VV<pair<int, int>>(40, V<pair<int, int>>(4))); // 停止位置

// 停止位置初期化
void init_stop_pos() {
    stop_pos_0.assign(40, VV<pair<int, int>>(40, V<pair<int, int>>(4)));
    REP(y, N) REP(x, N) {
        REP(d, 4) {
            int ny = y, nx = x;
            while (0 <= ny + dy[d] && ny + dy[d] < N && 0 <= nx + dx[d] && nx + dx[d] < N && !S0[ny + dy[d]][nx + dx[d]]) {
                ny += dy[d];
                nx += dx[d];
            }
            stop_pos_0[y][x][d] = {ny, nx};
        }
    }
}

// 確率の更新
// あらかじめ stop_pos を更新しておく
VV<ld> update_probs(VV<ld> &probs, VVV<pair<int, int>> &stop_pos, VV<bool> &S) {
    VV<ld> nprobs(N, V<ld>(N, 0));
    REP(y, N) REP(x, N) {
        if (S[y][x]) continue;
        if (probs[y][x] < 1e-10) continue;
        REP(d, 4) {
            auto [stop_y, stop_x] = stop_pos[y][x][d];
            nprobs[stop_y][stop_x] += probs[y][x] / 4.0;
        }
    }
    return nprobs;
}

// stop_pos の更新
void update_stop_pos(VVV<pair<int, int>> &stop_pos, int py, int px, const VV<bool>& S) {
    
    for (int y = py + 1; y < N && !S[y][px]; ++y) {
        if (stop_pos[y][px][2].first <= py) {
            stop_pos[y][px][2] = {py + 1, px};
        }
    }

    for (int x = px + 1; x < N && !S[py][x]; ++x) {
        if (stop_pos[py][x][3].second <= px) {
            stop_pos[py][x][3] = {py, px + 1};
        }
    }

    for (int y = py - 1; y >= 0 && !S[y][px]; --y) {
        if (stop_pos[y][px][0].first >= py) {
            stop_pos[y][px][0] = {py - 1, px};
        }
    }

    for (int x = px - 1; x >= 0 && !S[py][x]; --x) {
        if (stop_pos[py][x][1].second >= px) {
            stop_pos[py][x][1] = {py, px - 1};
        }
    }
}


// 初期状態から何回かシミュレーション
VV<ld> update_probs_mul(V<pair<int, int>> pos, int turn) {
    VV<ld> probs = probs_0;
    VVV<pair<int, int>> stop_pos = stop_pos_0;
    VV<bool> S = S0;
    REP(i, min(turn, SZ(pos))) {
        probs = update_probs(probs, stop_pos, S);
        S[pos[i].first][pos[i].second] = 1;
        update_stop_pos(stop_pos, pos[i].first, pos[i].second, S);
    }
    return probs;
}

// 通過確率
VV<ld> get_prob_pass(VV<ld> &probs, VVV<pair<int, int>> &stop_pos, VV<bool> &S) {

    VV<ld> probs_pass(N, V<ld>(N, 0));
    REP(y, N) REP(x, N) {
        if (S[y][x]) continue;
        REP(d, 4) {
            int ny = y, nx = x;
            if (probs[y][x] < 1e-10) continue;
            while (0 <= ny && ny < N && 0 <= nx && nx < N && !S[ny][nx]) {
                probs_pass[ny][nx] += probs[y][x] / 4.0;
                ny += dy[d];
                nx += dx[d];
            }
        }
    }
    
    return probs_pass;
}

int turn0 = 100;

V<pair<int, int>> solve0() {
    Rand rand;
    V<pair<int, int>> ans;
    REP(y, N) REP(x, N) {
        if (!S0[y][x]) {
            ans.emplace_back(y, x);
        }
    }
    rand.shuffle(ans);

    auto cost_probs = [](VV<ld> &probs) {
        ld cost = 0.0, s1 = 0.0, s2 = 0.0;
        REP(y, N) REP(x, N) {
            s1 += probs[y][x];
            s2 += probs[y][x] * probs[y][x] * probs[y][x];
        }
        cost = s1 * (-1000.0) + s2 * (-1.0);
        return cost;
    };

    V<pair<int, int>> best_ans = ans;
    ld best_cost = inf;

    Timer timer; timer.start(1900);
    int tr = 0, ac = 0; 
    while (!timer.is_timeover()) {
        tr++;
        int r0 = rand.randint(turn0);
        int r1 = rand.randint(N * N - M);
        while (r1 == r0) {
            r1 = rand.randint(N * N - M);
        }
        swap(ans[r0], ans[r1]);
        VV<ld> probs = update_probs_mul(ans, turn0);
        ld cost = cost_probs(probs);
        if (best_cost >= cost) {
            ac++;
            best_cost = cost;
            best_ans = ans;
        } else {
            swap(ans[r0], ans[r1]);
        }
    }

    debug(tr, ac);

    debug(best_cost);

    V<pair<int, int>> fans;
    REP(i, turn0) {
        fans.emplace_back(best_ans[i].first, best_ans[i].second);
    }
    return fans;
}

void solve() {

    init_probs();
    init_stop_pos();

    // はじめの turn0 ターンを解いておく
    V<pair<int, int>> ans = solve0();

    VV<ld> probs = probs_0;
    VVV<pair<int, int>> stop_pos = stop_pos_0;
    VV<bool> S = S0;
    VV<ld> probs_pass = get_prob_pass(probs, stop_pos, S);

    auto cost = [&](int y, int x) -> ld {
        return probs[y][x] * 10000.0 + probs_pass[y][x];
    };

    REP(t, N * N - M) {

        probs_pass = get_prob_pass(probs, stop_pos, S);
        probs = update_probs(probs, stop_pos, S);

        if (t < turn0) {
            auto [y, x] = ans[t];
            S[y][x] = 1;
            update_stop_pos(stop_pos, y, x, S);
        } else {
            ld best_c = inf;
            int best_y = -1, best_x = -1;
            REP(y, N) REP(x, N) {
                if (!S[y][x]) {
                    if (best_c > cost(y, x)) {
                        best_c = cost(y, x);
                        best_y = y;
                        best_x = x;
                    }
                }
            }
            ans.emplace_back(best_y, best_x);
            S[best_y][best_x] = 1;
            update_stop_pos(stop_pos, best_y, best_x, S);
        }
    }

    output(ans);
}


int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    // cout << fixed << setprecision(20);
    // local での計測用
    Timer local_timer; local_timer.start(0);

    // solver
    input_data();
    solve();

    // local での計測用
    cerr << "time=" << local_timer.elapsed_time() << endl;
    return 0;
}