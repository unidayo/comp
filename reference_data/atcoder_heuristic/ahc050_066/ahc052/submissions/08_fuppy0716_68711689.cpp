//*
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
//*/

#include <bits/stdc++.h>
// #include <atcoder/all>

using namespace std;
// using namespace atcoder;

// #define _GLIBCXX_DEBUG
#ifdef LOCAL
#define DEBUG(x) cerr << #x << ": " << x << endl;
#define DEBUG_VEC(v)                                                           \
    cerr << #v << ":";                                                         \
    for (int iiiiiiii = 0; iiiiiiii < v.size(); iiiiiiii++)                    \
        cerr << " " << v[iiiiiiii];                                            \
    cerr << endl;
#define DEBUG_MAT(v)                                                           \
    cerr << #v << endl;                                                        \
    for (int iv = 0; iv < v.size(); iv++) {                                    \
        for (int jv = 0; jv < v[iv].size(); jv++) {                            \
            cerr << v[iv][jv] << " ";                                          \
        }                                                                      \
        cerr << endl;                                                          \
    }
#else
#define DEBUG(x) ;
#define DEBUG_VEC(v) ;
#define DEBUG_MAT(v) ;
#endif
typedef long long ll;
// #define int ll

#define vi vector<int>
#define vl vector<ll>
#define vii vector<vector<int>>
#define vll vector<vector<ll>>
#define pii pair<int, int>
#define pis pair<int, string>
#define psi pair<string, int>
#define pll pair<ll, ll>
template <class S, class T>
pair<S, T> operator+(const pair<S, T> &s, const pair<S, T> &t) {
    return pair<S, T>(s.first + t.first, s.second + t.second);
}
template <class S, class T>
pair<S, T> operator-(const pair<S, T> &s, const pair<S, T> &t) {
    return pair<S, T>(s.first - t.first, s.second - t.second);
}
template <class S, class T> ostream &operator<<(ostream &os, pair<S, T> p) {
    os << "(" << p.first << ", " << p.second << ")";
    return os;
}
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define rep1(i, n) for (int i = 1; i <= (int)(n); i++)
#define rrep(i, n) for (int i = (int)(n) - 1; i >= 0; i--)
#define rrep1(i, n) for (int i = (int)(n); i > 0; i--)
#define REP(i, a, b) for (int i = a; i < b; i++)
#define in(x, a, b) (a <= x && x < b)
#define all(c) c.begin(), c.end()
void YES(bool t = true) { cout << (t ? "YES" : "NO") << endl; }
void Yes(bool t = true) { cout << (t ? "Yes" : "No") << endl; }
void yes(bool t = true) { cout << (t ? "yes" : "no") << endl; }
void NO(bool t = true) { cout << (t ? "NO" : "YES") << endl; }
void No(bool t = true) { cout << (t ? "No" : "Yes") << endl; }
void no(bool t = true) { cout << (t ? "no" : "yes") << endl; }
template <class T> bool chmax(T &a, const T &b) {
    if (a < b) {
        a = b;
        return 1;
    }
    return 0;
}
template <class T> bool chmin(T &a, const T &b) {
    if (a > b) {
        a = b;
        return 1;
    }
    return 0;
}
template <class T, class U> T ceil_div(T a, U b) { return (a + b - 1) / b; }
template <class T> T safe_mul(T a, T b) {
    if (a == 0 || b == 0)
        return 0;
    if (numeric_limits<T>::max() / a < b)
        return numeric_limits<T>::max();
    return a * b;
}
#define UNIQUE(v) v.erase(std::unique(v.begin(), v.end()), v.end());
const ll inf = 1000000001;
const ll INF = (ll)1e18 + 1;
const long double pi = 3.1415926535897932384626433832795028841971L;
int popcount(ll t) { return __builtin_popcountll(t); }
vector<int> gen_perm(int n) {
    vector<int> ret(n);
    iota(all(ret), 0);
    return ret;
}
// int dx[4] = {1, 0, -1, 0}, dy[4] = {0, 1, 0, -1};
// int dx2[8] = { 1,1,0,-1,-1,-1,0,1 }, dy2[8] = { 0,1,1,1,0,-1,-1,-1 };
vi dx = {0, 0, -1, 1}, dy = {-1, 1, 0, 0};
vi dx2 = {1, 1, 0, -1, -1, -1, 0, 1}, dy2 = {0, 1, 1, 1, 0, -1, -1, -1};
string dirs = "UDLR";
struct Setup_io {
    Setup_io() {
        ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);
        cout << fixed << setprecision(25);
        cerr << fixed << setprecision(25);
    }
} setup_io;
// constexpr ll MOD = 1000000007;
constexpr ll MOD = 998244353;
// #define mp make_pair

auto start = chrono::system_clock::now();
constexpr double TL = 1.99;

double calc_seconds() {
    return static_cast<double>(chrono::duration_cast<chrono::microseconds>(
                                   chrono::system_clock::now() - start)
                                   .count() /
                               1000000.0);
}

class HRandom {
  public:
    HRandom() {
        random_mw = 1995;
        random_mz = 2022;
    }
    int intRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result >> 1;
    }
    unsigned int uintRandom_65535() {
        // random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        // random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        // return random_mw & 65535;
        const unsigned tmp = random_mw & 65535;
        random_mw = 18000 * tmp + (random_mw >> 16);
        return tmp;
    }
    unsigned int uintRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result;
    }
    double dblRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return (result + 1.0) * 2.328306435454494e-10;
    }

  private:
    unsigned random_mw;
    unsigned random_mz;
};

HRandom hrandom;

static uint32_t randxor() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
void randxor_init() {}

double randdouble() { return hrandom.dblRandom(); }
bool randbool(const float &p = 0.5) {
    return hrandom.uintRandom_65535() < (p * 65535);
}

inline int rnd(const int n) {
    assert(0 <= n);

    if (n == 1) {
        return 0;
    }
    return ((hrandom.uintRandom_65535()) * n) >> 16;
}

int rnd(const int &l, const int &r) {
    assert(l <= r);
    return rnd(r - l) + l;
}

class Parameter {
  public:
    Parameter() {}

    static Parameter from_env() {
        return Parameter();
        // return Parameter(getenv("START_TEMP") ? stof(getenv("START_TEMP"))
        //                                       : START_TEMP);
    }
};

Parameter param;
std::default_random_engine engine(42);

constexpr int N = 30;
constexpr int M = 10;
constexpr int K = 10;
constexpr int MAX_OPS = 2 * N * N;

struct Robot {
    int y, x;
    Robot(int y = 0, int x = 0) : y(y), x(x) {}
};

struct Button {
    string s;

    Button(string s = string(M, 'S')) : s(s) {}
};

struct Space {
    int u, d, l, r;
    Space(int u = 0, int d = 0, int l = 0, int r = 0)
        : u(u), d(d), l(l), r(r) {}

    bool can_merge_horizontal(const Space &other) const {
        // 水平方向にマージ可能か（左右に隣接）
        return (u == other.u && d == other.d && (r == other.l || l == other.r));
    }

    bool can_merge_vertical(const Space &other) const {
        // 垂直方向にマージ可能か（上下に隣接）
        return (l == other.l && r == other.r && (d == other.u || u == other.d));
    }

    Space merge_horizontal(const Space &other) const {
        return Space(u, d, min(l, other.l), max(r, other.r));
    }

    Space merge_vertical(const Space &other) const {
        return Space(min(u, other.u), max(d, other.d), l, r);
    }

    bool has_wall_between_horizontal(const Space &other,
                                     const vii &v_walls_ref) const {
        // 水平方向の隣接スペース間に壁があるかチェック
        if (r == other.l) {
            // thisの右端とotherの左端が隣接
            for (int i = u; i < d; i++) {
                if (r - 1 < N - 1 && v_walls_ref[i][r - 1] == 1) {
                    return true;
                }
            }
        } else if (l == other.r) {
            // thisの左端とotherの右端が隣接
            for (int i = u; i < d; i++) {
                if (other.r - 1 < N - 1 && v_walls_ref[i][other.r - 1] == 1) {
                    return true;
                }
            }
        }
        return false;
    }

    bool has_wall_between_vertical(const Space &other,
                                   const vii &h_walls_ref) const {
        // 垂直方向の隣接スペース間に壁があるかチェック
        if (d == other.u) {
            // thisの下端とotherの上端が隣接
            for (int j = l; j < r; j++) {
                if (d - 1 < N - 1 && h_walls_ref[d - 1][j] == 1) {
                    return true;
                }
            }
        } else if (u == other.d) {
            // thisの上端とotherの下端が隣接
            for (int j = l; j < r; j++) {
                if (other.d - 1 < N - 1 && h_walls_ref[other.d - 1][j] == 1) {
                    return true;
                }
            }
        }
        return false;
    }
};

vector<Robot> initial_robots;
vii h_walls(N - 1, vi(N, 0)), v_walls(N, vi(N - 1, 0));

// 事前計算したBFS結果（グローバル）
vector<vector<vector<int>>> dist;   // dist[start_y][start_x][y*N+x]
vector<vector<vector<pii>>> parent; // parent[start_y][start_x][y*N+x]

// 全始点からのBFS結果を事前計算
void precompute_bfs() {
    dist.assign(N, vector<vector<int>>(N, vector<int>(N * N, -1)));
    parent.assign(N, vector<vector<pii>>(N, vector<pii>(N * N, {-1, -1})));

    rep(start_y, N) {
        rep(start_x, N) {
            queue<pii> q;
            q.push({start_y, start_x});
            dist[start_y][start_x][start_y * N + start_x] = 0;

            while (!q.empty()) {
                auto [y, x] = q.front();
                q.pop();

                for (int d = 0; d < 4; d++) {
                    int ny = y + dy[d];
                    int nx = x + dx[d];

                    if (!in(ny, 0, N) || !in(nx, 0, N))
                        continue;
                    if (dist[start_y][start_x][ny * N + nx] != -1)
                        continue;

                    bool can_move = true;
                    if (d == 0 || d == 1) { // U or D
                        if (h_walls[min(y, ny)][x] == 1) {
                            can_move = false;
                        }
                    } else { // L or R
                        if (v_walls[y][min(x, nx)] == 1) {
                            can_move = false;
                        }
                    }

                    if (can_move) {
                        dist[start_y][start_x][ny * N + nx] =
                            dist[start_y][start_x][y * N + x] + 1;
                        parent[start_y][start_x][ny * N + nx] = {y, x};
                        q.push({ny, nx});
                    }
                }
            }
        }
    }
}

void input() {
    int dummy;
    cin >> dummy >> dummy >> dummy;

    initial_robots.resize(M);
    for (int i = 0; i < M; i++) {
        int y, x;
        cin >> y >> x;
        initial_robots[i] = Robot(y, x);
    }

    rep(i, N) {
        rep(j, N - 1) {
            char c;
            cin >> c;
            v_walls[i][j] = c - '0';
        }
    }

    rep(i, N - 1) {
        rep(j, N) {
            char c;
            cin >> c;
            h_walls[i][j] = c - '0';
        }
    }
}

vector<Space> divide_space() {
    set<int> v_walls_set, h_walls_set;
    rep(i, N) {
        rep(j, N - 1) {
            if (v_walls[i][j] == 1) {
                v_walls_set.insert(j + 1);
            }
        }
    }
    rep(i, N - 1) {
        rep(j, N) {
            if (h_walls[i][j] == 1) {
                h_walls_set.insert(i + 1);
            }
        }
    }

    v_walls_set.insert(0);
    v_walls_set.insert(N);
    h_walls_set.insert(0);
    h_walls_set.insert(N);

    vector<Space> spaces;
    for (auto it = v_walls_set.begin(); next(it) != v_walls_set.end(); it++) {
        int l = *it;
        int r = *next(it);
        for (auto it2 = h_walls_set.begin(); next(it2) != h_walls_set.end();
             it2++) {
            int u = *it2;
            int d = *next(it2);
            spaces.push_back(Space(u, d, l, r));
        }
    }

    return spaces;
}

vector<Space> merge_spaces(vector<Space> spaces) {
    bool merged = true;

    while (merged) {
        if (spaces.size() <= M) {
            break;
        }

        merged = false;

        for (int i = 0; i < spaces.size() && !merged; i++) {
            for (int j = i + 1; j < spaces.size() && !merged; j++) {
                // 水平方向のマージを試行
                if (spaces[i].can_merge_horizontal(spaces[j]) &&
                    !spaces[i].has_wall_between_horizontal(spaces[j],
                                                           v_walls)) {
                    Space new_space = spaces[i].merge_horizontal(spaces[j]);
                    spaces.erase(spaces.begin() + j);
                    spaces.erase(spaces.begin() + i);
                    spaces.push_back(new_space);
                    merged = true;
                    break;
                }

                // 垂直方向のマージを試行
                if (spaces[i].can_merge_vertical(spaces[j]) &&
                    !spaces[i].has_wall_between_vertical(spaces[j], h_walls)) {
                    Space new_space = spaces[i].merge_vertical(spaces[j]);
                    spaces.erase(spaces.begin() + j);
                    spaces.erase(spaces.begin() + i);
                    spaces.push_back(new_space);
                    merged = true;
                    break;
                }
            }
        }
    }

    return spaces;
}

struct Solver {
    vector<Space> spaces;
    vector<Button> buttons;
    vector<Robot> current_robots;
    vector<int> is_visited; // 各行をintのビットで表現

    Solver(vector<Space> spaces, const vector<vector<int>> &buttons_0_3,
           const vector<vector<int>> &buttons_4_9)
        : spaces(spaces) {
        current_robots = initial_robots;
        init_buttons(buttons_0_3, buttons_4_9);
        is_visited.resize(N, 0);

        for (auto r : current_robots) {
            set_visited(r.y, r.x);
        }
    }

    // ビット演算用のプライベートメンバ関数
    void set_visited(int y, int x) { is_visited[y] |= (1 << x); }

    bool is_cell_visited(int y, int x) const {
        return (is_visited[y] >> x) & 1;
    }

    // spaceに未訪問セルがあるかを高速チェック
    bool has_unvisited_in_space(const Space &space) const {
        // 対象範囲のビットマスクを作成
        int mask = 0;
        for (int x = space.l; x < space.r; x++) {
            mask |= (1 << x);
        }
        for (int y = space.u; y < space.d; y++) {
            // 未訪問ビット（~is_visited[y]）と範囲マスクのANDが0でなければ未訪問あり
            if ((~is_visited[y]) & mask) {
                return true;
            }
        }
        return false;
    }

    // マスクを受け取るバージョン（高速化用）
    bool has_unvisited_in_space_with_mask(const Space &space, int mask) const {
        for (int y = space.u; y < space.d; y++) {
            if ((~is_visited[y]) & mask) {
                return true;
            }
        }
        return false;
    }

    // spaceの未訪問セル数をカウント
    int count_unvisited_in_space(const Space &space) const {
        int count = 0;
        for (int y = space.u; y < space.d; y++) {
            for (int x = space.l; x < space.r; x++) {
                if (!is_cell_visited(y, x)) {
                    count++;
                }
            }
        }
        return count;
    }

    void init_buttons(const vector<vector<int>> &buttons_0_3,
                      const vector<vector<int>> &buttons_4_9) {
        buttons.resize(K);

        // ボタン0~3の設定（直接指定）
        rep(i, 4) {
            string s;
            rep(j, M) { s += dirs[buttons_0_3[i][j]]; }
            buttons[i] = Button(s);
        }

        // ボタン4~9の設定（直接指定）
        rep(i, 6) {
            string s;
            rep(j, M) { s += dirs[buttons_4_9[i][j]]; }
            buttons[i + 4] = Button(s);
        }
    }

    void apply_button(int idx) {
        rep(i, M) {
            int y = current_robots[i].y;
            int x = current_robots[i].x;

            char c = buttons[idx].s[i];
            int ny = y, nx = x;
            if (c == 'U') {
                ny--;
            } else if (c == 'D') {
                ny++;
            } else if (c == 'L') {
                nx--;
            } else if (c == 'R') {
                nx++;
            }

            bool can_move = true;
            if (!in(ny, 0, N) || !in(nx, 0, N)) {
                can_move = false;
            }

            if (can_move) {
                if (c == 'U' || c == 'D') {
                    if (h_walls[min(y, ny)][x] == 1) {
                        can_move = false;
                    }
                } else if (c == 'L' || c == 'R') {
                    if (v_walls[y][min(x, nx)] == 1) {
                        can_move = false;
                    }
                }
            }

            if (can_move) {
                current_robots[i] = Robot(ny, nx);
                set_visited(ny, nx);
            } else {
                current_robots[i] = Robot(y, x);
                set_visited(y, x);
            }
        }
    }

    // 綺麗にされていない面積が一番大きいSpaceを探す
    int find_largest_unvisited_space() {
        int max_unvisited = 0;
        int best_space_idx = 0;

        rep(i, spaces.size()) {
            int unvisited_count = count_unvisited_in_space(spaces[i]);
            if (unvisited_count > max_unvisited) {
                max_unvisited = unvisited_count;
                best_space_idx = i;
            }
        }
        return best_space_idx;
    }

    // そのSpaceの四隅に一番近いロボットを探す（ロボットと最寄り四隅を返す）
    pair<int, pii> find_nearest_robot_to_space(const Space &space) {
        vector<pii> corners = {
            {space.u, space.l},        // 左上
            {space.u, space.r - 1},    // 右上
            {space.d - 1, space.l},    // 左下
            {space.d - 1, space.r - 1} // 右下
        };

        int best_robot = 0;
        pii best_corner = corners[0];
        int min_dist = inf;

        rep(i, M) {
            int robot_y = current_robots[i].y;
            int robot_x = current_robots[i].x;

            for (auto corner : corners) {
                int dist =
                    abs(robot_y - corner.first) + abs(robot_x - corner.second);
                if (dist < min_dist) {
                    min_dist = dist;
                    best_robot = i;
                    best_corner = corner;
                }
            }
        }
        return {best_robot, best_corner};
    }

    // 事前計算されたBFS結果から経路を復元
    vector<pii> find_path_bfs(int start_y, int start_x, int target_y,
                              int target_x) {
        vector<pii> path;

        // 経路が存在しない場合
        if (dist[start_y][start_x][target_y * N + target_x] == -1) {
            return path;
        }

        // 経路復元
        int cy = target_y, cx = target_x;
        while (cy != start_y || cx != start_x) {
            path.push_back({cy, cx});
            auto [py, px] = parent[start_y][start_x][cy * N + cx];
            cy = py;
            cx = px;
        }
        path.push_back({start_y, start_x});

        reverse(path.begin(), path.end());
        return path;
    }

    // ボタン4~9の中から指定されたロボットが指定された方向に移動できるボタンを探す
    // 見つからない場合は0~3を使用
    int find_4_to_9_button(int robot_idx, int desired_dir) {
        // まずボタン4~9を検索
        for (int btn = 4; btn < K; btn++) {
            if (buttons[btn].s[robot_idx] == dirs[desired_dir]) {
                return btn;
            }
        }
        // 見つからない場合はボタン0~3を検索
        for (int btn = 0; btn < 4; btn++) {
            if (buttons[btn].s[robot_idx] == dirs[desired_dir]) {
                return btn;
            }
        }
        assert(false);
    }

    // ボタン0~3の中から指定されたロボットが指定された方向に移動できるボタンを探す
    int find_0_to_3_button(int robot_idx, int desired_dir) {
        for (int btn = 0; btn < 4; btn++) {
            if (buttons[btn].s[robot_idx] == dirs[desired_dir]) {
                return btn;
            }
        }
        // デバッグ情報を出力
        cerr << "Error: No button found for robot " << robot_idx
             << " direction " << desired_dir << endl;
        cerr << "Available directions: ";
        for (int btn = 0; btn < 4; btn++) {
            cerr << (int)(buttons[btn].s[robot_idx] - 'U') << " ";
        }
        cerr << endl;
        assert(false);
    }

    // ロボットを指定された四隅に移動させる（ボタン4-7を使用、BFS経路探索）
    vector<int> move_robot_to_corner(int robot_idx, pii target_corner) {
        vector<int> operations;
        int start_y = current_robots[robot_idx].y;
        int start_x = current_robots[robot_idx].x;
        int target_y = target_corner.first;
        int target_x = target_corner.second;

        // BFSで最短経路を求める
        vector<pii> path = find_path_bfs(start_y, start_x, target_y, target_x);

        if (path.size() <= 1) {
            return operations; // 既に目標位置にいるか、経路が見つからない
        }

        // 経路に従って移動
        for (int i = 0; i < path.size() - 1; i++) {
            int cy = path[i].first, cx = path[i].second;
            int ny = path[i + 1].first, nx = path[i + 1].second;

            // 移動方向を決定
            int move_dir = -1;
            if (ny == cy - 1)
                move_dir = 0; // U
            else if (ny == cy + 1)
                move_dir = 1; // D
            else if (nx == cx - 1)
                move_dir = 2; // L
            else if (nx == cx + 1)
                move_dir = 3; // R

            assert(move_dir != -1);

            int button_to_use = find_4_to_9_button(robot_idx, move_dir);
            apply_button(button_to_use);
            operations.push_back(button_to_use);

            if (operations.size() > MAX_OPS)
                break;
        }

        return operations;
    }

    // ジグザグにスペースを掃除する（開始位置に応じて方向を決定）
    vector<int> clean_space_zigzag(const Space &space, pii start_corner,
                                   int robot_idx) {
        vector<int> operations;
        operations.reserve(128); // メモリ再確保を削減

        // 事前計算: スペース範囲
        const int u = space.u, d = space.d, l = space.l, r = space.r;
        const int width = r - l;
        const int height = d - u;

        // 開始位置と方向を決定
        const bool is_top_start = (start_corner.first == u);
        const bool is_left_start = (start_corner.second == l);

        // spaceの範囲マスク（事前計算）
        const int space_mask = ((1 << r) - 1) - ((1 << l) - 1);

        // 現在位置
        int y = start_corner.first;
        int x = start_corner.second;

        // 方向状態（ローカル変数で高速化）
        bool going_down = is_top_start;
        bool going_right = is_left_start;

        // ボタンキャッシュ（頻繁な検索を避ける）
        int button_cache[4] = {-1, -1, -1, -1};

        // メイン処理ループ
        while (true) {
            // 早期終了チェック（ビット演算で高速化）
            bool has_unvisited = false;
            for (int check_y = u; check_y < d; check_y++) {
                if ((~is_visited[check_y]) & space_mask) {
                    has_unvisited = true;
                    break;
                }
            }
            if (!has_unvisited)
                break;

            // 水平移動の一括処理
            const int target_x = going_right ? r - 1 : l;
            const int dx = going_right ? 1 : -1;
            const int dir = going_right ? 3 : 2; // R : L

            // ボタンキャッシュの活用
            if (button_cache[dir] == -1) {
                button_cache[dir] = find_0_to_3_button(robot_idx, dir);
            }
            const int h_button = button_cache[dir];

            // 水平移動をバッチで実行
            while (x != target_x) {
                // 進行方向の空きチェック（最適化）
                int remaining_mask = going_right
                                         ? (space_mask & ~((1 << (x + 1)) - 1))
                                         : (space_mask & ((1 << x) - 1));

                bool has_unvisited_ahead = false;

                // 現在行チェック
                if ((~is_visited[y]) & remaining_mask) {
                    has_unvisited_ahead = true;
                } else {
                    // 次行チェック（存在する場合のみ）
                    int next_y = going_down ? y + 1 : y - 1;
                    if (next_y >= u && next_y < d) {
                        if ((~is_visited[next_y]) & remaining_mask) {
                            has_unvisited_ahead = true;
                        }
                    }
                }

                if (!has_unvisited_ahead)
                    break;

                x += dx;
                apply_button(h_button);
                operations.push_back(h_button);

                // 操作数制限チェック
                if (operations.size() >= 1000)
                    return operations;
            }

            for (int check_y = u; check_y < d; check_y++) {
                if ((~is_visited[check_y]) & space_mask) {
                    has_unvisited = true;
                    break;
                }
            }
            if (!has_unvisited)
                break;

            // 垂直移動
            const int target_y = going_down ? d - 1 : u;
            if (y == target_y)
                break;

            const int v_dir = going_down ? 1 : 0; // D : U
            if (button_cache[v_dir] == -1) {
                button_cache[v_dir] = find_0_to_3_button(robot_idx, v_dir);
            }

            y += going_down ? 1 : -1;
            apply_button(button_cache[v_dir]);
            operations.push_back(button_cache[v_dir]);

            // 方向反転
            going_right = !going_right;
        }

        return operations;
    }

    // メインのアルゴリズム実装
    vector<int> solve() {
        vector<int> all_operations;

        while (true) {
            // 1. 綺麗にされていない面積が一番大きいSpaceを探す
            int space_idx = find_largest_unvisited_space();

            // 未訪問セルがない場合は終了
            if (!has_unvisited_in_space(spaces[space_idx])) {
                break;
            }

            // 2. そのSpaceの四隅に一番近いロボットと最寄り四隅を探す
            auto [robot_idx, nearest_corner] =
                find_nearest_robot_to_space(spaces[space_idx]);

            // 3. そのロボットをボタン4~7を使って最寄り四隅に配置する
            vector<int> move_ops =
                move_robot_to_corner(robot_idx, nearest_corner);
            all_operations.insert(all_operations.end(), move_ops.begin(),
                                  move_ops.end());

            // 4.
            // ボタン0~3を使って開始位置に応じたジグザグでそのスペースを掃除する
            vector<int> clean_ops = clean_space_zigzag(
                spaces[space_idx], nearest_corner, robot_idx);
            all_operations.insert(all_operations.end(), clean_ops.begin(),
                                  clean_ops.end());

            // 操作回数制限チェック
            if (all_operations.size() >= MAX_OPS)
                break;
        }

        return all_operations;
    }
};

struct AnnealingState {
    using Cost = int;

    double start_sec, end_sec;
    double temperature;

    Cost score;
    Cost best_score;
    // ボタン0,2の方向を指定（1,3は自動で逆方向になる）
    vector<int> button_0_dirs, button_2_dirs; // 各ロボットに対する方向
    vector<vector<int>> buttons_4_9;          // 6ボタン x 10ロボット
    vector<int> best_button_0_dirs, best_button_2_dirs;
    vector<vector<int>> best_buttons_4_9;
    vector<Space> spaces;

    AnnealingState(double end_sec, vector<Space> spaces)
        : start_sec(calc_seconds()), end_sec(end_sec), spaces(spaces) {
        init();
    }

    // ボタン0,2の方向からボタン0-3の完全な配列を構築
    vector<vector<int>> build_buttons_0_3() const {
        vector<vector<int>> buttons_0_3(4, vector<int>(M));
        rep(j, M) {
            buttons_0_3[0][j] = button_0_dirs[j];
            buttons_0_3[1][j] = button_0_dirs[j] ^ 1; // 逆方向
            buttons_0_3[2][j] = button_2_dirs[j];
            buttons_0_3[3][j] = button_2_dirs[j] ^ 1; // 逆方向
        }
        return buttons_0_3;
    }

    void init() {
        build_initial();
        best_score = inf; // 最小化問題
        update_best();
    }

    void build_initial() {
        // 初期解をランダムに生成
        button_0_dirs.resize(M);
        button_2_dirs.resize(M);
        buttons_4_9.assign(6, vector<int>(M));

        // ボタン0,2をランダムに初期化（0,1が逆方向、2,3が逆方向になるように）
        rep(i, M) {
            // button_0はU/D (0,1)またはL/R (2,3)から選択
            if (rnd(2) == 0) {
                button_0_dirs[i] = 0; // U（1はD）
                button_2_dirs[i] = 2; // L（3はR）
            } else {
                button_0_dirs[i] = 2; // L（1はR）
                button_2_dirs[i] = 0; // U（3はD）
            }
        }

        rep(i, 6) {
            rep(j, M) { buttons_4_9[i][j] = rnd(4); }
        }
        evaluate();
    }

    void evaluate() {
        vector<vector<int>> buttons_0_3 = build_buttons_0_3();
        Solver solver(spaces, buttons_0_3, buttons_4_9);
        vector<int> operations = solver.solve();
        score = operations.size();
    }

    void solve() {
        double start_temp = 20.0, end_temp = 0.5;

        rep(try_num, inf) {
            if (try_num % 16 == 0) {
                double sec = calc_seconds();
                if (should_stop(try_num, sec)) {
                    cerr << "try_num: " << try_num << endl;
                    DEBUG(try_num);
                    break;
                }

                double rate = decide_rate(try_num, sec);
                set_temperature(rate, start_temp, end_temp);
            }

            step();
        }
    }

    inline bool should_stop(int try_num, double sec) const {
#ifdef FREEZE
        return try_num > 10000;
#else
        return sec > end_sec;
#endif
    }

    double decide_rate(int try_num, double sec) const {
#ifdef FREEZE
        // フリーズしている場合は、10000回に1回の確率で温度を下げる
        return try_num / 10000.0;
#else
        return (sec - start_sec) / (end_sec - start_sec);
#endif
    }

    void set_temperature(double rate, double start_temp, double end_temp) {
        temperature = start_temp * pow(end_temp / start_temp, rate);
    }

    void step() {
        // 現在の状態を保存
        vector<int> prev_button_0_dirs = button_0_dirs;
        vector<int> prev_button_2_dirs = button_2_dirs;
        vector<vector<int>> prev_buttons_4_9 = buttons_4_9;
        Cost prev_score = score;

        // ランダムに操作を選択
        int op_type = rnd(6);

        if (op_type == 0) {
            // 01反転: ボタン0,1の軸を反転（U/D ↔ L/R）
            int robot_idx = rnd(M);
            button_0_dirs[robot_idx] ^= 1;
        } else if (op_type == 1) {
            // 23反転: ボタン2,3の軸を反転（U/D ↔ L/R）
            int robot_idx = rnd(M);
            button_2_dirs[robot_idx] ^= 1;
        } else if (op_type == 2) {
            // 01と23の入れ替え: ボタン0とボタン2の方向を交換
            int robot_idx = rnd(M);
            swap(button_0_dirs[robot_idx], button_2_dirs[robot_idx]);
        } else {
            if (rnd(10)) {
                // buttons_4_9を変更（従来通り）
                int btn_idx = rnd(6);
                int robot_idx = rnd(M);
                buttons_4_9[btn_idx][robot_idx] = rnd(4);
            } else {
                int btn_idx1 = rnd(6);
                int btn_idx2 = rnd(6);
                while (btn_idx1 == btn_idx2) {
                    btn_idx2 = rnd(6);
                }
                swap(buttons_4_9[btn_idx1], buttons_4_9[btn_idx2]);
            }
        }

        // 新しいスコアを評価
        evaluate();
        Cost score_diff = score - prev_score;

        // 受容判定
        if (!should_accept(score_diff)) {
            // 元に戻す
            button_0_dirs = prev_button_0_dirs;
            button_2_dirs = prev_button_2_dirs;
            buttons_4_9 = prev_buttons_4_9;
            score = prev_score;
        } else {
            update_best();
        }
    }

    bool should_accept(Cost score_diff) const {
        if (score_diff <= 0) {
            return true;
        }
        return randdouble() < exp(-score_diff / temperature);
    }

    bool update_best() {
        if (score < best_score) {
            best_score = score;
            best_button_0_dirs = button_0_dirs;
            best_button_2_dirs = button_2_dirs;
            best_buttons_4_9 = buttons_4_9;
            cerr << "New best operations: " << best_score << endl;
            return true;
        }
        return false;
    }
};

signed main(int argc, char **argv) {
    input();
    precompute_bfs(); // BFS結果を事前計算
    DEBUG(calc_seconds());
    vector<Space> spaces = divide_space();
    spaces = merge_spaces(spaces);

    // 焼きなまし法でボタン設定を最適化
    AnnealingState annealing(TL, spaces);
    annealing.solve();

    // 最良のボタン設定で最終実行
    // AnnealingStateから最適なボタン0-3の設定を構築
    vector<vector<int>> best_buttons_0_3(4, vector<int>(M));
    rep(j, M) {
        best_buttons_0_3[0][j] = annealing.best_button_0_dirs[j];
        best_buttons_0_3[1][j] = annealing.best_button_0_dirs[j] ^ 1; // 逆方向
        best_buttons_0_3[2][j] = annealing.best_button_2_dirs[j];
        best_buttons_0_3[3][j] = annealing.best_button_2_dirs[j] ^ 1; // 逆方向
    }

    Solver best_solver(spaces, best_buttons_0_3, annealing.best_buttons_4_9);
    vector<int> best_operations = best_solver.solve();

    // 最良の結果を出力
    rep(i, K) {
        rep(j, M) {
            cout << best_solver.buttons[i].s[j];
            if (j < M - 1)
                cout << " ";
        }
        cout << endl;
    }

    for (int op : best_operations) {
        cout << op << endl;
    }

    // スコア情報を標準エラー出力
    int unvisited_count = 0;
    rep(i, N) {
        rep(j, N) {
            if (!best_solver.is_cell_visited(i, j)) {
                unvisited_count++;
            }
        }
    }

    int final_score;
    if (unvisited_count == 0) {
        final_score = 3 * N * N - (int)best_operations.size();
    } else {
        final_score = N * N - unvisited_count;
    }

    cerr << "Final operations: " << best_operations.size() << endl;
    cerr << "Final score: " << final_score << endl;
    cerr << "Unvisited cells: " << unvisited_count << endl;

    return 0;
}
