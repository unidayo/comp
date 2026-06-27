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
template<class T> using VVVV = V<VVV<T>>;

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

int N = 30, M = 10, K = 10;
char dc[] = {'U', 'D', 'L', 'R', 'S'};
int dy[] = {-1, 1, 0, 0, 0};
int dx[] = {0, 0, -1, 1, 0};
int ny[30][30][5];
int nx[30][30][5];
int sy[10], sx[10];

void input_data() {
    input(N, M, K);
    REP(i, M) {
        cin >> sy[i] >> sx[i];
    }

    REP(y, N) REP(x, N) REP(d, 5) {
        ny[y][x][d] = y;
        nx[y][x][d] = x;
    }
    REP(y, N) {
        string s; input(s);
        REP(x, N - 1) {
            if (s[x] == '0') {
                nx[y][x][3] = x + 1;
                nx[y][x + 1][2] = x;
            }
        }
    }
    REP(y, N - 1) {
        string s; input(s);
        REP(x, N) {
            if (s[x] == '0') {
                ny[y][x][1] = y + 1;
                ny[y + 1][x][0] = y;
            }
        }
    }
}

VVVV<int> dist;
VVVV<int> dirs; // dirs[y0][x0][y1][x1]: (y1, x1) から (y0, x0) に向かうための dir
void calc_route() {
    dist = make_vector(N, N, N, N, inf);
    dirs = make_vector(N, N, N, N, 4);

    REP(y, N) REP(x, N) {
        dist[y][x][y][x] = 0;
        queue<pair<int, int>> q;
        q.emplace(y, x);
        while (!q.empty()) {
            auto [cy, cx] = q.front(); q.pop();
            REP(d, 4) {
                int cyy = ny[cy][cx][d], cxx = nx[cy][cx][d];
                if (dist[y][x][cyy][cxx] == inf) {
                    dist[y][x][cyy][cxx] = dist[y][x][cy][cx] + 1;
                    dirs[y][x][cyy][cxx] = d ^ 1;
                    q.emplace(cyy, cxx);
                }
            }
        }
    }
}


struct State {

    VV<int> count_board;
    VV<int> config;
    V<int> ops;
    V<int> fy, fx;

    Rand rand;

    State(): count_board(N, V<int>(N, 0)), config(K, V<int>(M, 5)), ops(), fy(M, -1), fx(M, -1) {

        ops.emplace_back(9);

        REP(__, 2) {
            REP(_, 12) ops.emplace_back(0);
            ops.emplace_back(2);
            REP(_, 12) ops.emplace_back(1);
            ops.emplace_back(2);
        }

        REP(__, 2) {
            REP(_, 12) ops.emplace_back(3);
            ops.emplace_back(5);
            REP(_, 12) ops.emplace_back(4);
            ops.emplace_back(5);
        }

        REP(__, 2) {
            REP(_, 12) ops.emplace_back(6);
            ops.emplace_back(8);
            REP(_, 12) ops.emplace_back(7);
            ops.emplace_back(8);
        }

        // REP(i, 5) ops.emplace_back(9);

        REP(i, K) REP(m, M) {
            config[i][m] = rand.randint(0, 5);
        }
        REP(m, M) {
            add_route(m);
        }
    }

    void output() {
        REP(k, K) {
            REP(m, M) {
                cout << dc[config[k][m]];
                if (m < M) cout << ' ';
            }
            cout << endl;
        }
        REPA(op, ops) {
            cout << op << endl;
        }
    }

    // r 番目のロボットの命令を count_board に追加
    void add_route(int r) {
        int y = sy[r], x = sx[r];
        count_board[y][x]++;
        REP(i, SZ(ops)) {
            int d = config[ops[i]][r];
            int yy = ny[y][x][d], xx = nx[y][x][d];
            count_board[yy][xx]++;
            y = yy;
            x = xx;
        }
        fy[r] = y;
        fx[r] = x;
    }

    // r 番目のロボットの命令を count_board から削除
    void del_route(int r) {
        int y = sy[r], x = sx[r];
        count_board[y][x]--;
        REP(i, SZ(ops)) {
            int d = config[ops[i]][r];
            int yy = ny[y][x][d], xx = nx[y][x][d];
            count_board[yy][xx]--;
            y = yy;
            x = xx;
        }
        fy[r] = -1;
        fx[r] = -1;
    }

    void add_op(int op) {
        ops.emplace_back(op);
        REP(i, M) {
            int d = config[op][i];
            int yy = ny[fy[i]][fx[i]][d], xx = nx[fy[i]][fx[i]][d];
            count_board[yy][xx]++;
            fy[i] = yy;
            fx[i] = xx;
        }
    }

    int score() {
        int res = 0;
        REP(y, N) REP(x, N) {
            if (count_board[y][x] > 0) {
                res += 1;
            }
        }
        res += 100;
        return res;
    }

    // ボタン k のロボット m への割り当てを x に変更
    int change_config(int k, int m, int x) {
        del_route(m);
        config[k][m] = x;
        add_route(m);
        return score();
    }

    // 最も近くの空白に向かわせる
    void fill_board_1(VV<int> &op_ids) {
        
        int idx = -1;
        int ty = -1, tx = -1;
        int d = inf;
        int dir = 4;
        REP(i, M) {
            if (count(ALL(op_ids[i]), -1) > 0) continue;
            REP(y, N) REP(x, N) {
                if (count_board[y][x] == 0) {
                    if (d > dist[y][x][fy[i]][fx[i]]) {
                        d = dist[y][x][fy[i]][fx[i]];
                        dir = dirs[y][x][fy[i]][fx[i]];
                        ty = y;
                        tx = x;
                        idx = i;
                    }
                }
            }
        }
        
        if (ty == -1 || dir == 4 || idx == -1) return;

        add_op(op_ids[idx][dir]);
    }

    // 空白を埋める
    void fill_board() {
        VV<int> op_ids;

        REP(i, M) {
            V<int> op_id(4, -1); 
            REP(j, K) {
                if (config[j][i] != 4) {
                    op_id[config[j][i]] = j;
                }
            }
            op_ids.emplace_back(op_id);
        }

        REP(i, 200) {
            fill_board_1(op_ids);
        }
    }

    // 不要な命令を除く
    V<int> trim_ops() {
        count_board.assign(N, V<int>(N, 0));
        V<int> y_list, x_list;
        REP(i, M) {
            y_list.emplace_back(sy[i]);
            x_list.emplace_back(sx[i]);
            count_board[sy[i]][sx[i]]++;
        }

        V<int> ops_real;
        REP(i, SZ(ops)) {
            bool is_stop = 1;
            REP(m, M) {
                int d = config[ops[i]][m];
                int yy = ny[y_list[m]][x_list[m]][d], xx = nx[y_list[m]][x_list[m]][d];
                count_board[yy][xx]++;
                if (yy != y_list[m] || xx != x_list[m]) {
                    is_stop = 0;
                }
                y_list[m] = yy;
                x_list[m] = xx;
            }

            if (!is_stop) {
                ops_real.emplace_back(ops[i]);
            }

            bool ok = 1;
            REP(y, N) REP(x, N) {
                if (count_board[y][x] == 0) {
                    ok = 0;
                    break;
                }
            }
            if (ok) break;
        }
        fy = y_list;
        fx = x_list;
        return ops_real;
    }
};


void solve() {
    Rand rand;
    State state;
    int now_score = state.score();
    Timer timer; timer.start(1950);

    ld T0 = 1e+3;
    ld T1 = 1e-1;
    auto temp = [&]() -> ld {
        return T0 * pow(T1 / T0, timer.elapsed_rate());
    };
    auto prob = [&](ld now, ld nxt) -> ld {
        return min(1, exp((nxt - now) / temp()));
    };

    while (!timer.is_timeover()) {
        int k = rand.randint(0, K);
        int m = rand.randint(0, M);
        int x_prev = state.config[k][m];
        int x = rand.randint(0, 5);
        while (x == x_prev) x = rand.randint(0, 5); 
        int nxt_score = state.change_config(k, m, x);
        if (rand.rand_bool(prob(now_score, nxt_score))) {
            now_score = nxt_score;
        } else {
            state.change_config(k, m, x_prev);
        }
    }

    state.fill_board();
    state.ops = state.trim_ops();
    state.output();
}


int main(){
    // ios::sync_with_stdio(0);
    // cin.tie(0);
    // cout << fixed << setprecision(20);
    // local での計測用
    Timer local_timer; local_timer.start(0);

    input_data();
    calc_route();
    solve();

    // local での計測用
    cerr << "time=" << local_timer.elapsed_time() << endl;
    return 0;
}