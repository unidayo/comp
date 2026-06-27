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
constexpr double TL = 1.98;

double calc_seconds() {
    return static_cast<double>(chrono::duration_cast<chrono::microseconds>(
                                   chrono::system_clock::now() - start)
                                   .count() /
                               1000000.0);
}

class HRandom {
  public:
    HRandom() {
        random_mw = 1985;
        random_mz = 2012;
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
    // assert(0 <= n);

    if (n == 1) {
        return 0;
    }
    return ((hrandom.uintRandom_65535()) * n) >> 16;
}

int rnd(const int &l, const int &r) {
    // assert(l <= r);
    return rnd(r - l) + l;
}

constexpr float START_TEMP = 0.001;
constexpr float END_TEMP = 0.00001;
constexpr int TWICE_DP_THRESHOLD = 300;
constexpr float ANNEAL_GOAL_RATE_THRESHOLD = 0.05;
constexpr float OP1_PROB = 0.416666666;
constexpr float OP2_PROB = 0.416666666;
constexpr float OP3_PROB = 0.083333333;
constexpr int ANG_RESET_INTERVAL = 10;

class Parameter {
  public:
    float start_temp;
    float end_temp;
    int twice_dp_threshold;
    float anneal_goal_rate_threshold;
    float op1_prob;
    float op2_prob;
    float op3_prob;
    int ang_reset_interval;

    Parameter() {}

    Parameter(float start_temp, float end_temp, int twice_dp_threshold,
              float anneal_goal_rate_threshold, float op1_prob, float op2_prob,
              float op3_prob, int ang_reset_interval)
        : start_temp(start_temp), end_temp(end_temp),
          twice_dp_threshold(twice_dp_threshold),
          anneal_goal_rate_threshold(anneal_goal_rate_threshold),
          op1_prob(op1_prob), op2_prob(op2_prob), op3_prob(op3_prob),
          ang_reset_interval(ang_reset_interval) {}

    static Parameter from_env() {
        return Parameter(
            getenv("START_TEMP") ? stof(getenv("START_TEMP")) : START_TEMP,
            getenv("END_TEMP") ? stof(getenv("END_TEMP")) : END_TEMP,
            getenv("TWICE_DP_THRESHOLD") ? stoi(getenv("TWICE_DP_THRESHOLD"))
                                         : TWICE_DP_THRESHOLD,
            getenv("ANNEAL_GOAL_RATE_THRESHOLD")
                ? stof(getenv("ANNEAL_GOAL_RATE_THRESHOLD"))
                : ANNEAL_GOAL_RATE_THRESHOLD,
            getenv("OP1_PROB") ? stof(getenv("OP1_PROB")) : OP1_PROB,
            getenv("OP2_PROB") ? stof(getenv("OP2_PROB")) : OP2_PROB,
            getenv("OP3_PROB") ? stof(getenv("OP3_PROB")) : OP3_PROB,
            getenv("ANG_RESET_INTERVAL") ? stoi(getenv("ANG_RESET_INTERVAL"))
                                         : ANG_RESET_INTERVAL);
    }
};

Parameter param;
std::default_random_engine engine(42);

namespace segment {
// 線分交差判定
int sign(long long x) { return x > 0 ? 1 : (x < 0 ? -1 : 0); }

int orientation(pii a, pii b, pii c) {
    long long cross = (long long)(b.first - a.first) * (c.second - a.second) -
                      (long long)(b.second - a.second) * (c.first - a.first);
    return sign(cross);
}

inline bool on_segment(const pii &p, const pii &q, const pii &r) {
    // q が線分 pr 上にあるか（境界含む）
    if (orientation(p, r, q) != 0)
        return false;
    return min(p.first, r.first) <= q.first &&
           q.first <= max(p.first, r.first) &&
           min(p.second, r.second) <= q.second &&
           q.second <= max(p.second, r.second);
}

bool segments_intersect(pii p1, pii p2, pii q1, pii q2) {
    if (p1 == q1 || p1 == q2 || p2 == q1 || p2 == q2) {
        return false;
    }

    if (max(p1.first, p2.first) < min(q1.first, q2.first) ||
        max(q1.first, q2.first) < min(p1.first, p2.first) ||
        max(p1.second, p2.second) < min(q1.second, q2.second) ||
        max(q1.second, q2.second) < min(p1.second, p2.second)) {
        return false;
    }
    int o1 = orientation(p1, p2, q1);
    int o2 = orientation(p1, p2, q2);
    if (o1 * o2 > 0) {
        return false;
    }
    int o3 = orientation(q1, q2, p1);
    int o4 = orientation(q1, q2, p2);
    return (o3 * o4 <= 0);
}

// 点 x が三角形 abc の内部にあるか
bool point_in_triangle(pii a, pii b, pii c, pii x) {
    int oabx = orientation(a, b, x);
    int obcx = orientation(b, c, x);
    int ocax = orientation(c, a, x);

    int area_sign = orientation(a, b, c);
    if (area_sign == 0) {
        // 計算速度向上のため、退化した三角形は考慮しない
        return false;
    }

    bool pos = (oabx > 0 && obcx > 0 && ocax > 0);
    bool neg = (oabx < 0 && obcx < 0 && ocax < 0);
    return pos || neg;
}
}; // namespace segment

using namespace segment;

struct Sorter {
    int idx;
    vector<float> p;

    Sorter() : idx(-1) {}
    Sorter(int idx, vector<float> p) : idx(idx), p(p) {}

    pair<vector<float>, vector<float>> split(vector<float> prob) {
        vector<float> p1(prob.size()), p2(prob.size());
        for (int i = 0; i < prob.size(); i++) {
            p1[i] = prob[i] * p[i];
            p2[i] = prob[i] * (1 - p[i]);
        }
        return {p1, p2};
    }
};

int TRASH_KIND, SORTER_PLACE, SORTER_KIND;
vector<pair<int, int>> goal_pos;   // 処理装置設置場所の座標
vector<pair<int, int>> sorter_pos; // 分別器設置場所の座標
vector<Sorter> sorters;
const pii start_pos = {0, 5000};
// ノード番号 -> 座標 の前計算
vector<pii> node_pos;

// 角度ソート用の事前計算構造
struct AngleCand {
    int node;
    float ang;
    float dist2;
};
vector<vector<AngleCand>> g_angle_cands_by_loc; // index: sorter_loc

void precompute_angle_lists() {
    g_angle_cands_by_loc.assign(SORTER_PLACE, {});
    const float TWO_PI = 2.0 * (float)pi;
    for (int loc = 0; loc < SORTER_PLACE; loc++) {
        vector<AngleCand> &vec = g_angle_cands_by_loc[loc];
        vec.clear();
        const pii &cp = sorter_pos[loc];
        auto ang_of = [&](const pii &p) -> float {
            float a =
                atan2((float)p.second - cp.second, (float)p.first - cp.first);
            if (a < 0)
                a += TWO_PI;
            return a;
        };
        // start
        vec.emplace_back(TRASH_KIND + SORTER_PLACE, ang_of(start_pos),
                         (float)(start_pos.first - cp.first) *
                                 (start_pos.first - cp.first) +
                             (float)(start_pos.second - cp.second) *
                                 (start_pos.second - cp.second));
        // goals
        for (int g = 0; g < TRASH_KIND; g++) {
            const pii &p = goal_pos[g];
            float dx = (float)p.first - cp.first,
                  dy = (float)p.second - cp.second;
            vec.emplace_back(g, ang_of(p), dx * dx + dy * dy);
        }
        // sorters
        for (int s = 0; s < SORTER_PLACE; s++) {
            if (s == loc)
                continue;
            const pii &p = sorter_pos[s];
            float dx = (float)p.first - cp.first,
                  dy = (float)p.second - cp.second;
            vec.emplace_back(TRASH_KIND + s, ang_of(p), dx * dx + dy * dy);
        }
        sort(vec.begin(), vec.end(),
             [](const AngleCand &a, const AngleCand &b) {
                 if (a.ang != b.ang)
                     return a.ang < b.ang;
                 return a.dist2 < b.dist2;
             });
    }
}

// 入力を受け取る関数
void input() {
    cin >> TRASH_KIND >> SORTER_PLACE >> SORTER_KIND;

    // 処理装置設置場所の座標
    goal_pos.resize(TRASH_KIND);
    for (int i = 0; i < TRASH_KIND; i++) {
        cin >> goal_pos[i].first >> goal_pos[i].second;
    }

    // 分別器設置場所の座標
    sorter_pos.resize(SORTER_PLACE);
    for (int i = 0; i < SORTER_PLACE; i++) {
        cin >> sorter_pos[i].first >> sorter_pos[i].second;
    }

    // 分別器の確率
    sorters.resize(SORTER_KIND);
    for (int i = 0; i < SORTER_KIND; i++) {
        vector<float> p(TRASH_KIND);
        for (int j = 0; j < TRASH_KIND; j++) {
            cin >> p[j];
        }
        sorters[i] = Sorter(i, p);
    }

    // ノード座標前計算
    node_pos.assign(TRASH_KIND + SORTER_PLACE + 1, {0, 0});
    for (int i = 0; i < TRASH_KIND; i++)
        node_pos[i] = goal_pos[i];
    for (int i = 0; i < SORTER_PLACE; i++)
        node_pos[TRASH_KIND + i] = sorter_pos[i];
    node_pos[TRASH_KIND + SORTER_PLACE] = start_pos;
}

inline int loc_to_node(int loc) { return TRASH_KIND + loc; }
inline int node_to_loc(int node) { return node - TRASH_KIND; }
inline bool is_sorter_node(int node) {
    return node >= TRASH_KIND && node < TRASH_KIND + SORTER_PLACE;
}
inline bool is_goal_node(int node) { return node < TRASH_KIND; }
inline bool is_start_node(int node) {
    return node == TRASH_KIND + SORTER_PLACE;
}

struct Solution {
    vector<int> goal_assignment; // goal_assignment[i] =
                                 // i番目の処理装置に配置するゴミ種類
    int start_destination;       // ごみ搬入口からの行き先
    vector<int>
        placed_sorters; // placed_sorters[i] =
                        // i番目の分別器設置場所に配置された分別器種類(-1=未配置)

    vector<int> placed_indices;

    vector<int> sorter_out1, sorter_out2; // 各分別器設置場所の出力先

    Solution() {
        goal_assignment.resize(TRASH_KIND);
        placed_sorters.resize(SORTER_PLACE, -1);
        sorter_out1.resize(SORTER_PLACE, -1);
        sorter_out2.resize(SORTER_PLACE, -1);
        start_destination = -1;

        // デフォルトの処理装置配置（各ゴミ種類を対応する処理装置に）
        for (int i = 0; i < TRASH_KIND; i++) {
            goal_assignment[i] = i;
        }
    }

    // 高速: 動的な vector 生成を避ける隣接取得
    inline int out_deg(int node) const {
        if (is_start_node(node))
            return (start_destination == -1 ? 0 : 1);
        if (is_goal_node(node))
            return 0;
        int loc = node_to_loc(node);
        if (sorter_out1[loc] == -1)
            return 0;
        // 出力先は常に2本（仕様上同一先でも可）
        return 2;
    }
    inline void get_adjs_fast(int node, int &a, int &b) const {
        if (is_start_node(node)) {
            a = start_destination;
            b = -1;
            return;
        }
        if (is_goal_node(node)) {
            a = b = -1;
            return;
        }
        int loc = node_to_loc(node);
        if (sorter_out1[loc] == -1) {
            a = b = -1;
            return;
        }
        a = sorter_out1[loc];
        b = sorter_out2[loc];
    }

    vector<pii> edges() const {
        vector<pii> ret;
        for (int i : placed_indices) {
            int sorter_node = loc_to_node(i);
            ret.emplace_back(sorter_node, sorter_out1[i]);
            ret.emplace_back(sorter_node, sorter_out2[i]);
        }
        ret.emplace_back(TRASH_KIND + SORTER_PLACE, start_destination);
        return ret;
    }

    void output() const {
        // 処理装置の配置を出力
        for (int i = 0; i < TRASH_KIND; i++) {
            cout << goal_assignment[i];
            if (i < TRASH_KIND - 1)
                cout << " ";
        }
        cout << endl;

        // ごみ搬入口からの行き先を出力
        cout << start_destination << endl;

        // 各分別器設置場所の状態を出力
        for (int i = 0; i < SORTER_PLACE; i++) {
            if (placed_sorters[i] == -1) {
                cout << -1 << endl;
            } else {
                cout << placed_sorters[i] << " " << sorter_out1[i] << " "
                     << sorter_out2[i] << endl;
            }
        }
    }
};

// 前計算された距離情報
vector<vector<pair<float, int>>>
    precomputed_distances; // [from_location][sorted_by_distance]

struct ConnectionDecider {
    pii start_pos = {0, 5000}; // ごみ搬入口
    Solution solution;         // 解の情報

    vector<int> nearest_goal;

    ConnectionDecider() {
        precompute_distances();

        nearest_goal.resize(SORTER_PLACE);
        for (int i = 0; i < SORTER_PLACE; i++) {
            nearest_goal[i] = -1;
        }
    }

    // 距離情報を前計算
    void precompute_distances() {
        if (precomputed_distances.size() != 0) {
            return;
        }
        precomputed_distances.resize(SORTER_PLACE);

        for (int from = 0; from < SORTER_PLACE; from++) {
            pii from_pos = sorter_pos[from];
            vector<pair<float, int>> distances;

            // 分別器設置場所への距離
            for (int i = 0; i < SORTER_PLACE; i++) {
                if (from_pos == sorter_pos[i]) {
                    continue; // 自分自身は除外
                }
                distances.emplace_back(
                    squared_distance(from_pos, sorter_pos[i]), TRASH_KIND + i);
            }

            sort(distances.begin(), distances.end());
            precomputed_distances[from] = distances;
        }
    }

    // 距離計算
    float squared_distance(pii a, pii b) {
        float dx = a.first - b.first;
        float dy = a.second - b.second;
        return dx * dx + dy * dy;
    }

    // 座標取得 (node番号から座標を取得)
    pii get_position(int node) {
        if (is_start_node(node)) {
            return start_pos; // 特別なstart node
        } else if (is_goal_node(node)) {
            return goal_pos[node]; // 処理装置
        } else {
            return sorter_pos[node_to_loc(node)]; // 分別器設置場所
        }
    }

    // 近い場所を選択 (上位candidate_count個からnum_select個をランダム選択)
    vector<int> get_nearby_nodes(int from_location, int candidate_count,
                                 int num_select) {
        const auto &distances = precomputed_distances[from_location];

        // 上位candidate_count個から選択
        static vector<int> candidates;
        candidates.clear();
        for (int i = 0; i < min(candidate_count, (int)distances.size()); i++) {
            candidates.emplace_back(distances[i].second);
        }

        // ランダムに num_select 個選択
        vector<int> result;
        for (int i = 0; i < min(num_select, (int)candidates.size()); i++) {
            int idx = rnd(candidates.size());
            result.emplace_back(candidates[idx]);
            swap(candidates[idx], candidates.back());
            candidates.pop_back();
        }

        return result;
    }

    // 閉路検出 (新しい辺 from -> to を追加した時に閉路ができるか)
    bool would_create_cycle(int from, int to) {
        // toからDFSしてfromに到達できるかチェック
        static vector<int> visited(TRASH_KIND + SORTER_PLACE + 1, -1);
        static int seen_iter = 0;
        seen_iter++;
        return dfs_reaches(to, from, visited, seen_iter);
    }

    bool dfs_reaches(int current, int target, vector<int> &visited, int iter) {
        if (current == target)
            return true;
        if (visited[current] == iter)
            return false;
        visited[current] = iter;

        int a, b;
        solution.get_adjs_fast(current, a, b);
        if (a != -1 && dfs_reaches(a, target, visited, iter))
            return true;
        if (b != -1 && dfs_reaches(b, target, visited, iter))
            return true;
        return false;
    }

    // 線分交差チェック
    bool would_intersect(int from, int to) {
        const pii &from_pos = node_pos[from];
        const pii &to_pos = node_pos[to];

        for (int sorter_loc : solution.placed_indices) {
            int sorter_node = loc_to_node(sorter_loc);
            const pii &edge_from = node_pos[sorter_node];
            rep(i, 2) {
                int out_node = (i == 0 ? solution.sorter_out1[sorter_loc]
                                       : solution.sorter_out2[sorter_loc]);
                if (out_node == -1) {
                    continue;
                }
                const pii &edge_to = node_pos[out_node];
                if (segments_intersect(from_pos, to_pos, edge_from, edge_to)) {
                    return true;
                }
            }
        }

        if (solution.start_destination != -1) {
            const pii &edge_to = node_pos[solution.start_destination];
            if (segments_intersect(from_pos, to_pos, start_pos, edge_to)) {
                return true;
            }
        }
        return false;
    }

    // 未配置の分別器場所からランダム選択 (入力辺があるが出力辞がないもの)
    int select_unplaced_sorter_location() {
        vector<int> unplaced;
        for (int i = 0; i < SORTER_PLACE; i++) {
            if (solution.placed_sorters[i] != -1 &&
                solution.sorter_out1[i] == -1) {
                // 分別器は配置されているが、出力先が未決定
                unplaced.emplace_back(i);
            }
        }
        if (unplaced.empty())
            return -1;
        return unplaced[rnd(unplaced.size())];
    }

    // 全ての未配置分別器がゴールに接続可能かチェック
    bool can_connect_all_to_goals() {
        static vector<int> to_rollback;
        to_rollback.clear();

        for (int i = 0; i < SORTER_PLACE; i++) {
            if (solution.placed_sorters[i] != -1 &&
                solution.sorter_out1[i] == -1) {
                // この分別器は配置されているが出力先が未決定
                int nearest_goal = find_nearest_goal(i);
                int sorter_node = loc_to_node(i);

                if (would_intersect(sorter_node, nearest_goal)) {
                    for (int i : to_rollback) {
                        solution.sorter_out1[i] = -1;
                        solution.sorter_out2[i] = -1;
                    }
                    return false;
                }
                solution.sorter_out1[i] = nearest_goal;
                solution.sorter_out2[i] = nearest_goal;
                to_rollback.emplace_back(i);
            }
        }
        for (int i : to_rollback) {
            solution.sorter_out1[i] = -1;
            solution.sorter_out2[i] = -1;
        }
        return true;
    }

    int find_nearest_goal(int sorter_loc) {
        if (nearest_goal[sorter_loc] != -1) {
            return nearest_goal[sorter_loc];
        }

        pii sorter_pos_cur = sorter_pos[sorter_loc];
        int nearest_goal_idx = 0;
        float min_dist = squared_distance(sorter_pos_cur, goal_pos[0]);
        for (int i = 1; i < TRASH_KIND; i++) {
            float dist = squared_distance(sorter_pos_cur, goal_pos[i]);
            if (dist < min_dist) {
                min_dist = dist;
                nearest_goal_idx = i;
            }
        }
        nearest_goal[sorter_loc] = nearest_goal_idx;
        return nearest_goal_idx;
    }

    // メインアルゴリズム
    void solve() {
        // 1. スタート地点から最も近い分別器設置場所に接続
        int first_sorter_loc = 0;
        float min_dist = squared_distance(start_pos, sorter_pos[0]);
        for (int i = 1; i < SORTER_PLACE; i++) {
            float dist = squared_distance(start_pos, sorter_pos[i]);
            if (dist < min_dist) {
                min_dist = dist;
                first_sorter_loc = i;
            }
        }

        int first_sorter_node = loc_to_node(first_sorter_loc);
        solution.placed_sorters[first_sorter_loc] =
            0; // とりあえず0番の分別器を配置
        solution.start_destination = first_sorter_node;
        solution.placed_indices.emplace_back(first_sorter_loc);

        // 2. 拡張フェーズ - より積極的に分別器を配置
        int iteration = 0;

        int consecutive_failures = 0;
        int placed_sorters_count = 0;

        while (calc_seconds() < TL - 0.2) {
            if (consecutive_failures > 5 * placed_sorters_count) {
                DEBUG("iteration: " + to_string(iteration));
                break;
            }

            iteration++;

            // 既に配置された分別器で出力先が未決定のものから選択
            int sorter_loc = select_unplaced_sorter_location();
            if (sorter_loc == -1) {
                break;
            }

            int sorter_node = loc_to_node(sorter_loc);

            // 近い場所2つを選択
            vector<int> targets =
                get_nearby_nodes(sorter_loc, SORTER_PLACE * 0.3, 2);

            if (targets.size() < 2) {
                consecutive_failures++;
                continue;
            }
            // assert(targets.size() == 2);

            // 閉路チェック
            if (would_create_cycle(sorter_node, targets[0]) ||
                would_create_cycle(sorter_node, targets[1])) {
                consecutive_failures++;
                continue;
            }

            // 交差チェック
            if (would_intersect(sorter_node, targets[0]) ||
                would_intersect(sorter_node, targets[1])) {
                consecutive_failures++;
                continue;
            }

            // 辞を追加
            solution.sorter_out1[sorter_loc] = targets[0];
            solution.sorter_out2[sorter_loc] = targets[1];

            // 対象が分別器設置場所なら分別器を配置
            static vector<int> placed_sorters_for_rollback;
            placed_sorters_for_rollback.clear();
            for (int target : targets) {
                int target_loc = target - TRASH_KIND;
                if (solution.placed_sorters[target_loc] == -1) {
                    placed_sorters_for_rollback.emplace_back(target_loc);
                    solution.placed_sorters[target_loc] = 0;
                    solution.placed_indices.emplace_back(target_loc);
                }
            }

            // ゴールに接続可能かチェック
            if (!can_connect_all_to_goals()) {
                solution.sorter_out1[sorter_loc] = -1;
                solution.sorter_out2[sorter_loc] = -1;
                for (int placed_sorter : placed_sorters_for_rollback) {
                    solution.placed_sorters[placed_sorter] = -1;
                    solution.placed_indices.pop_back();
                }

                consecutive_failures++;
                continue;
            }

            consecutive_failures = 0;
            placed_sorters_count++;
        }

        // assert(can_connect_all_to_goals());

        // 3. 最終接続フェーズ
        connect_remaining_to_goals();
    }

    void connect_remaining_to_goals() {
        for (int i = 0; i < SORTER_PLACE; i++) {
            if (solution.placed_sorters[i] != -1 &&
                solution.sorter_out1[i] == -1) {
                // この分別器は配置されているが出力先が未決定
                int nearest_goal = find_nearest_goal(i);
                int sorter_node = loc_to_node(i);
                // assert(!would_intersect(sorter_node, nearest_goal));
                solution.sorter_out1[i] = nearest_goal;
                solution.sorter_out2[i] = nearest_goal;
            }
        }
    }
};

struct SorterOptimizer {
    vector<vector<float>>
        node_score; // node_score[node][trash_type] =
                    // そのノードにtrash_typeが来たら何点もらえるか
    vector<vector<float>> in_probs;

    vector<int> last_changed_iter;
    int iter;

    Solution solution;

    SorterOptimizer(const Solution &sol) : solution(sol) {
        node_score.resize(TRASH_KIND + SORTER_PLACE + 1,
                          vector<float>(TRASH_KIND, 0.0));
        in_probs.resize(TRASH_KIND + SORTER_PLACE + 1,
                        vector<float>(TRASH_KIND, 0.0));
        last_changed_iter.resize(TRASH_KIND + SORTER_PLACE + 1, -1);
        iter = 0;
    }

    // 後ろから各ノードの期待スコアを計算
    void calculate_node_scores() {
        // 1. ゴールでのスコアを初期化
        for (int i = 0; i < TRASH_KIND; i++) {
            for (int trash = 0; trash < TRASH_KIND; trash++) {
                if (solution.goal_assignment[i] == trash) {
                    node_score[i][trash] = 1.0; // 正しいゴミが来たら1点
                } else {
                    node_score[i][trash] = 0.0; // 間違ったゴミが来たら0点
                }
            }
        }

        // 2. 後ろ向きに各ノードのスコアを計算
        static vector<int> visited(TRASH_KIND + SORTER_PLACE + 1, -1);
        static int visited_iter = 0;
        visited_iter++;

        auto dfs = [&](auto &&dfs, int node) {
            if (visited[node] == visited_iter)
                return;
            visited[node] = visited_iter;

            // このノードから出る辺を探す
            bool need_update = last_changed_iter[node] == iter ||
                               last_changed_iter[node] == -1;
            int a, b;
            solution.get_adjs_fast(node, a, b);
            if (a != -1) {
                dfs(dfs, a);
                if (last_changed_iter[a] == iter)
                    need_update = true;
            }
            if (b != -1) {
                dfs(dfs, b);
                if (last_changed_iter[b] == iter)
                    need_update = true;
            }

            if (need_update) {
                last_changed_iter[node] = iter;
            }

            // 分別器ノードの場合、最適な分別器を決定
            if (need_update && is_sorter_node(node)) {
                optimize_sorter_at_node(node);
            }
        };

        // スタートノードから開始
        dfs(dfs, TRASH_KIND + SORTER_PLACE);
        for (int sorter_loc : solution.placed_indices) {
            int sorter_node = loc_to_node(sorter_loc);
            dfs(dfs, sorter_node);
        }
    }

    // 特定のノードで最適な分別器を決定
    void optimize_sorter_at_node(int node) {
        int sorter_loc = node_to_loc(node);
        int out1 = solution.sorter_out1[sorter_loc];
        int out2 = solution.sorter_out2[sorter_loc];

        float best_score = -1.0;
        int best_type = 0;
        bool best_flip = false;

        // 事前計算: sum_out1, sum_out2 と weighted_diff = in_probs[node] *
        // (score[out1]-score[out2])
        static vector<float> weighted_diff; // 再利用してアロケーションを避ける
        if ((int)weighted_diff.size() != TRASH_KIND)
            weighted_diff.resize(TRASH_KIND);

        float sum_out1 = 0.0, sum_out2 = 0.0;
        for (int trash = 0; trash < TRASH_KIND; trash++) {
            float in_p = in_probs[node][trash];
            float s1 = node_score[out1][trash];
            float s2 = node_score[out2][trash];
            sum_out1 += in_p * s1;
            sum_out2 += in_p * s2;
            weighted_diff[trash] = in_p * (s1 - s2);
        }

        // 各分別器について、一回の内積で flip=0/1 両方を評価
        for (int k = 0; k < SORTER_KIND; k++) {
            const vector<float> &p1v = sorters[k].p;
            float dot_w = 0.0;
            for (int trash = 0; trash < TRASH_KIND; trash++) {
                dot_w += p1v[trash] * weighted_diff[trash];
            }
            float score_flip0 = sum_out2 + dot_w; // out1=out1, out2=out2
            float score_flip1 = sum_out1 - dot_w; // out1=out2, out2=out1

            if (score_flip0 > best_score) {
                best_score = score_flip0;
                best_type = k;
                best_flip = false;
            }
            if (score_flip1 > best_score) {
                best_score = score_flip1;
                best_type = k;
                best_flip = true;
            }

            if (out1 == out2) {
                break;
            }
        }

        // 最適な分別器を設定
        solution.placed_sorters[sorter_loc] = best_type;
        if (best_flip) {
            swap(solution.sorter_out1[sorter_loc],
                 solution.sorter_out2[sorter_loc]);
        }

        // このノードのスコアを更新
        int final_out1 = solution.sorter_out1[sorter_loc];
        int final_out2 = solution.sorter_out2[sorter_loc];
        for (int trash = 0; trash < TRASH_KIND; trash++) {
            float p1 = sorters[best_type].p[trash];
            node_score[node][trash] =
                p1 * node_score[final_out1][trash] +
                (1.0 - p1) * node_score[final_out2][trash];
        }
    }

    // 前向きに各ノードへの到達確率を計算
    void calculate_in_probs() {
        // 到達可能ノードの抽出（start から）
        int start_dest = solution.start_destination;

        static vector<char> reachable;
        reachable.assign(TRASH_KIND + SORTER_PLACE + 1, 0);

        queue<int> rq;
        rq.push(start_dest);
        reachable[start_dest] = 1;

        vector<int> reachable_nodes;
        reachable_nodes.reserve(TRASH_KIND + SORTER_PLACE + 1);
        reachable_nodes.emplace_back(start_dest);

        while (!rq.empty()) {
            int u = rq.front();
            rq.pop();
            int a, b;
            solution.get_adjs_fast(u, a, b);
            if (a != -1 && !reachable[a]) {
                reachable[a] = 1;
                rq.push(a);
                reachable_nodes.emplace_back(a);
            }
            if (b != -1 && !reachable[b]) {
                reachable[b] = 1;
                rq.push(b);
                reachable_nodes.emplace_back(b);
            }
        }

        // 到達可能ノードのみ初期化
        for (int node : reachable_nodes) {
            for (int trash = 0; trash < TRASH_KIND; trash++)
                in_probs[node][trash] = 0.0;
        }
        for (int trash = 0; trash < TRASH_KIND; trash++)
            in_probs[start_dest][trash] = 1.0;

        // 到達可能部分グラフのトポロジカル順序
        static vector<int> indeg(TRASH_KIND + SORTER_PLACE + 1, 0);
        for (int node : reachable_nodes)
            indeg[node] = 0;
        for (int node : reachable_nodes) {
            int a, b;
            solution.get_adjs_fast(node, a, b);
            if (a != -1 && reachable[a])
                indeg[a]++;
            if (b != -1 && reachable[b])
                indeg[b]++;
        }
        queue<int> q;
        for (int node : reachable_nodes)
            if (indeg[node] == 0)
                q.push(node);
        vector<int> order;
        order.reserve(reachable_nodes.size());
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            order.emplace_back(v);
            int a, b;
            solution.get_adjs_fast(v, a, b);
            if (a != -1 && reachable[a]) {
                if (--indeg[a] == 0)
                    q.push(a);
            }
            if (b != -1 && reachable[b]) {
                if (--indeg[b] == 0)
                    q.push(b);
            }
        }

        // 確率伝播
        for (int node : order) {
            if (is_goal_node(node) || is_start_node(node))
                continue;
            int sorter_loc = node_to_loc(node);
            if (solution.placed_sorters[sorter_loc] == -1)
                continue;
            int sorter_type = solution.placed_sorters[sorter_loc];
            int out1 = solution.sorter_out1[sorter_loc];
            int out2 = solution.sorter_out2[sorter_loc];

            const vector<float> &p1v = sorters[sorter_type].p;
            for (int trash = 0; trash < TRASH_KIND; trash++) {
                float prob = in_probs[node][trash];
                if (prob == 0.0)
                    continue;
                float p1 = p1v[trash];
                in_probs[out1][trash] += prob * p1;
                in_probs[out2][trash] += prob * (1.0 - p1);
            }
        }
    }

    // メイン処理
    void optimize(vi changed_nodes = {}) {
        // 後ろ向きDPでスコア計算と分別器最適化を行う
        iter++;
        if (changed_nodes.empty()) {
            rep(i, SORTER_PLACE) { last_changed_iter[loc_to_node(i)] = iter; }
            last_changed_iter[TRASH_KIND + SORTER_PLACE] = iter;
        } else {
            for (int node : changed_nodes) {
                last_changed_iter[node] = iter;
            }
        }

        calculate_node_scores();

        if (SORTER_PLACE < param.twice_dp_threshold) {
            calculate_in_probs();

            rep(i, SORTER_PLACE) { last_changed_iter[loc_to_node(i)] = iter; }
            last_changed_iter[TRASH_KIND + SORTER_PLACE] = iter;
            calculate_node_scores();
        }
    }

    float calc_score() {
        float total_score = 0.0;

        // スタートから各ゴミが等確率で入ってくると仮定
        int start_dest = solution.start_destination;
        for (int trash = 0; trash < TRASH_KIND; trash++) {
            float prob = 1.0;
            total_score += prob * node_score[start_dest][trash];
        }

        return total_score / TRASH_KIND;
    }
};

int valid_op_num = 0;

struct AnnealingSorterConnection {
    using Cost = float;

    Cost score;
    Cost best_score;

    float start_sec, end_sec;

    float temperature;

    Solution solution;
    Solution best_solution;

    SorterOptimizer optimizer;
    // 既存エッジの線分キャッシュ（from node, to node）
    vector<pair<int, int>> edge_segments_cache;

    // 空間グリッドによる交差候補の加速構造
    int grid_side = 10;             // グリッド分割数（64x64）
    int grid_cell_size = 1000;      // おおよそ 10000 / 64 ~= 156.25
    vector<vector<int>> grid_cells; // 各セルに属するエッジindexの配列
    vector<int> edge_seen_stamp;    // エッジ重複排除用スタンプ
    int edge_seen_iter = 1;

    AnnealingSorterConnection(float end_sec, Solution solution)
        : start_sec(calc_seconds()), end_sec(end_sec), solution(solution),
          optimizer(solution) {
        init();
    }

    void init() {
        build_initial();

        best_score = -inf;
        update_best();
        rebuild_edge_cache();
    }

    void build_initial() {
        // TODO: 問題に合わせて初期解を実装
        optimizer.solution = solution;
        optimizer.optimize();
        solution = optimizer.solution;
        score = optimizer.calc_score();
        DEBUG(score);
    }

    // エッジキャッシュを構築
    void rebuild_edge_cache() {
        edge_segments_cache.clear();
        edge_segments_cache.reserve(1 + 2 * solution.placed_indices.size());
        if (solution.start_destination != -1) {
            edge_segments_cache.emplace_back(TRASH_KIND + SORTER_PLACE,
                                             solution.start_destination);
        }
        for (int sorter_loc : solution.placed_indices) {
            int sorter_node = loc_to_node(sorter_loc);
            int out1 = solution.sorter_out1[sorter_loc];
            int out2 = solution.sorter_out2[sorter_loc];
            if (out1 != -1)
                edge_segments_cache.emplace_back(sorter_node, out1);
            if (out2 != -1)
                edge_segments_cache.emplace_back(sorter_node, out2);
        }

        // 空間グリッドを再構築
        build_spatial_index();
    }

    void solve() {
        float start_temp = param.start_temp, end_temp = param.end_temp;
        float rate = 0.0;

        rep(try_num, inf) {
            if (try_num % 16 == 0) {
                float sec = calc_seconds();
                if (should_stop(try_num, sec)) {
                    DEBUG(try_num);
                    cerr << try_num << endl;
                    break;
                }

                rate = decide_rate(try_num, sec);
                set_temperature(rate, start_temp, end_temp);
            }

            step(rate);

            if (try_num % 8192 == 0) {
                int ori_size = solution.placed_indices.size();
                double ori_score = score;
                prune_less_effective();
                if (solution.placed_indices.size() < ori_size) {
                    DEBUG(pii(ori_size, solution.placed_indices.size()));
                    DEBUG(make_pair(ori_score, score));
                    update_best();
                }
            }
        }
    }

    inline bool should_stop(int try_num, float sec) const {
#ifdef FREEZE
        return try_num > 10000;
#else
        return sec > end_sec;
#endif
    }

    float decide_rate(int try_num, float sec) const {
#ifdef FREEZE
        // フリーズしている場合は、10000回に1回の確率で温度を下げる
        return try_num / 10000.0;
#else
        return (sec - start_sec) / (end_sec - start_sec);
#endif
    }

    void set_temperature(float rate, float start_temp, float end_temp) {
        // temperature = start_temp + (end_temp - start_temp) * rate;
        temperature = start_temp * pow(end_temp / start_temp, rate);
    }

    void step(float rate) {
        bool accepted = false;
        constexpr int MAX_R = 1 << 10;

        static int op1_thre = MAX_R * param.op1_prob;
        static int op2_thre = MAX_R * (param.op1_prob + param.op2_prob);
        static int op3_thre =
            MAX_R * (param.op1_prob + param.op2_prob + param.op3_prob);
        static bool change_mode = false;

        if (!change_mode && rate > param.anneal_goal_rate_threshold) {
            change_mode = true;
            float p1 = param.op1_prob /
                       (param.op1_prob + param.op2_prob + param.op3_prob);
            float p2 = (param.op1_prob + param.op2_prob) /
                       (param.op1_prob + param.op2_prob + param.op3_prob);
            op1_thre = MAX_R * p1;
            op2_thre = MAX_R * p2;
            op3_thre = MAX_R;
            DEBUG(op1_thre);
            DEBUG(op2_thre);
            DEBUG(op3_thre);
        }

        int op_type = rnd(MAX_R);
        if (op_type < op1_thre) {
            accepted = op_change_sorter_out();
        } else if (op_type < op2_thre) {
            accepted = op_change_sorter_twice();
        } else if (op_type < op3_thre) {
            accepted = op_change_sorter_out_to_new();
        } else {
            if (rnd(2)) {
                accepted = op_swap_goal_assignment();
            } else {
                accepted = op_swap_3_goal_assignment();
            }
        }

        if (accepted) {
            update_best();
        }
    }

    bool op_change_sorter_out() {
        int sorter_loc =
            solution.placed_indices[rnd(solution.placed_indices.size())];
        int sorter_node = loc_to_node(sorter_loc);
        // 角度スキャンで行き先選択（事前計算を使用）
        int new_node =
            pick_non_intersecting_precomputed(sorter_loc, sorter_node, false);
        if (new_node == -1) {
            // // フォールバック: 従来の単発試行
            // int sum_num = solution.placed_indices.size() + TRASH_KIND;
            // if (rnd(sum_num) < TRASH_KIND)
            //     new_node = rnd(TRASH_KIND);
            // else
            //     new_node = loc_to_node(
            //         solution
            //             .placed_indices[rnd(solution.placed_indices.size())]);
            return false;
        }

        int old_out_node1 = solution.sorter_out1[sorter_loc];
        int old_out_node2 = solution.sorter_out2[sorter_loc];
        bool is_new_out1 = rnd(2);

        if (is_new_out1) {
            solution.sorter_out1[sorter_loc] = new_node;
        } else {
            solution.sorter_out2[sorter_loc] = new_node;
        }

        // 閉路チェック
        if (is_new_out1) {
            // // assert(!would_intersect(sorter_node, new_node));
            if (would_create_cycle(sorter_node, new_node)) {
                solution.sorter_out1[sorter_loc] = old_out_node1;
                return false;
            }
        } else {
            // // assert(!would_intersect(sorter_node, new_node));
            if (would_create_cycle(sorter_node, new_node)) {
                solution.sorter_out2[sorter_loc] = old_out_node2;
                return false;
            }
        }

        valid_op_num++;

        static vector<vector<float>> ori_node_score;
        static vector<vector<float>> ori_in_probs;

        ori_node_score = optimizer.node_score;
        if (SORTER_PLACE < param.twice_dp_threshold) {
            ori_in_probs = optimizer.in_probs;
        }

        if (is_new_out1) {
            optimizer.solution.sorter_out1[sorter_loc] = new_node;
        } else {
            optimizer.solution.sorter_out2[sorter_loc] = new_node;
        }

        optimizer.optimize({sorter_node});
        float new_score = optimizer.calc_score();

        if (should_accept(new_score - score)) {
            score = new_score;
            optimizer.calculate_in_probs();

            solution.sorter_out1 = optimizer.solution.sorter_out1;
            solution.sorter_out2 = optimizer.solution.sorter_out2;
            solution.placed_sorters = optimizer.solution.placed_sorters;

            rebuild_edge_cache();
            return true;
        } else {
            if (is_new_out1) {
                solution.sorter_out1[sorter_loc] = old_out_node1;
            } else {
                solution.sorter_out2[sorter_loc] = old_out_node2;
            }

            optimizer.solution = solution;
            optimizer.node_score = ori_node_score;
            if (SORTER_PLACE < param.twice_dp_threshold) {
                optimizer.in_probs = ori_in_probs;
            }
            return false;
        }
    }

    bool op_change_sorter_twice() {
        int sorter_loc =
            solution.placed_indices[rnd(solution.placed_indices.size())];
        int sorter_node = loc_to_node(sorter_loc);
        // 角度スキャンで行き先選択（事前計算を使用）
        int new_node =
            pick_non_intersecting_precomputed(sorter_loc, sorter_node, false);
        if (new_node == -1) {
            return false;
        }
        if (!is_sorter_node(new_node)) {
            return false;
        }

        int new_loc = node_to_loc(new_node);

        int old_out_node1 = solution.sorter_out1[sorter_loc];
        int old_out_node2 = solution.sorter_out2[sorter_loc];
        bool is_new_out1 = rnd(2);
        bool is_child_out1 = rnd(2);
        int child_node, old_child_node;

        if (is_new_out1) {
            child_node = old_out_node1;
            solution.sorter_out1[sorter_loc] = new_node;

            if (is_child_out1) {
                old_child_node = solution.sorter_out1[new_loc];
                solution.sorter_out1[new_loc] = child_node;
            } else {
                old_child_node = solution.sorter_out2[new_loc];
                solution.sorter_out2[new_loc] = child_node;
            }

        } else {
            child_node = old_out_node2;
            solution.sorter_out2[sorter_loc] = new_node;

            if (is_child_out1) {
                old_child_node = solution.sorter_out1[new_loc];
                solution.sorter_out1[new_loc] = child_node;
            } else {
                old_child_node = solution.sorter_out2[new_loc];
                solution.sorter_out2[new_loc] = child_node;
            }
        }

        bool rollback = false;
        if (would_intersect(new_node, child_node)) {
            rollback = true;
        }

        if (!rollback && would_create_cycle(sorter_node, new_node)) {
            rollback = true;
        }

        if (!rollback && would_create_cycle(new_node, child_node)) {
            rollback = true;
        }

        if (rollback) {
            if (is_new_out1) {
                solution.sorter_out1[sorter_loc] = old_out_node1;
            } else {
                solution.sorter_out2[sorter_loc] = old_out_node2;
            }

            if (is_child_out1) {
                solution.sorter_out1[new_loc] = old_child_node;
            } else {
                solution.sorter_out2[new_loc] = old_child_node;
            }

            return false;
        }

        valid_op_num++;

        static vector<vector<float>> ori_node_score;
        static vector<vector<float>> ori_in_probs;

        ori_node_score = optimizer.node_score;
        if (SORTER_PLACE < param.twice_dp_threshold) {
            ori_in_probs = optimizer.in_probs;
        }
        if (is_new_out1) {
            optimizer.solution.sorter_out1[sorter_loc] = new_node;
        } else {
            optimizer.solution.sorter_out2[sorter_loc] = new_node;
        }
        if (is_child_out1) {
            optimizer.solution.sorter_out1[new_loc] = child_node;
        } else {
            optimizer.solution.sorter_out2[new_loc] = child_node;
        }

        optimizer.optimize({sorter_node, new_node});
        float new_score = optimizer.calc_score();

        if (should_accept(new_score - score)) {
            score = new_score;
            optimizer.calculate_in_probs();

            solution.sorter_out1 = optimizer.solution.sorter_out1;
            solution.sorter_out2 = optimizer.solution.sorter_out2;
            solution.placed_sorters = optimizer.solution.placed_sorters;

            rebuild_edge_cache();
            return true;
        } else {
            if (is_new_out1) {
                solution.sorter_out1[sorter_loc] = old_out_node1;
            } else {
                solution.sorter_out2[sorter_loc] = old_out_node2;
            }
            if (is_child_out1) {
                solution.sorter_out1[new_loc] = old_child_node;
            } else {
                solution.sorter_out2[new_loc] = old_child_node;
            }

            optimizer.solution = solution;
            optimizer.node_score = ori_node_score;
            if (SORTER_PLACE < param.twice_dp_threshold) {
                optimizer.in_probs = ori_in_probs;
            }
            return false;
        }
    }

    bool op_change_sorter_out_to_new() {
        if (solution.placed_indices.size() == SORTER_PLACE) {
            return false;
        }

        int sorter_loc =
            solution.placed_indices[rnd(solution.placed_indices.size())];
        if (solution.sorter_out1[sorter_loc] ==
            solution.sorter_out2[sorter_loc]) {
            return false;
        }
        int sorter_node = loc_to_node(sorter_loc);

        int new_node =
            pick_non_intersecting_precomputed(sorter_loc, sorter_node, true);

        if (new_node == -1) {
            return false;
        }
        // assert(solution.placed_sorters[node_to_loc(new_node)] == -1);

        int old_out_node1 = solution.sorter_out1[sorter_loc];
        int old_out_node2 = solution.sorter_out2[sorter_loc];
        bool is_new_out1 = rnd(2);
        bool to_both = rnd(2);

        if (is_new_out1) {
            solution.sorter_out1[sorter_loc] = new_node;
        } else {
            solution.sorter_out2[sorter_loc] = new_node;
        }

        // 新しいソーターから既存のものへの繋ぎ込みが必要
        // スコアを変えずに多様性を目的とする遷移として、old に繋ぐ
        int new_loc = node_to_loc(new_node);
        if (is_new_out1) {
            solution.sorter_out1[new_loc] = old_out_node1;
            solution.sorter_out2[new_loc] = old_out_node1;
        } else {
            solution.sorter_out1[new_loc] = old_out_node2;
            solution.sorter_out2[new_loc] = old_out_node2;
        }
        solution.placed_sorters[new_loc] = 0;
        solution.placed_indices.emplace_back(new_loc);

        bool roll_back = false;
        if (is_new_out1) {
            if (would_intersect(new_node, old_out_node1)) {
                roll_back = true;
            }
        } else {
            if (would_intersect(new_node, old_out_node2)) {
                roll_back = true;
            }
        }

        if (!roll_back && to_both) {
            int new_loc = node_to_loc(new_node);
            int ori1 = solution.sorter_out1[new_loc];
            int ori2 = solution.sorter_out2[new_loc];
            solution.sorter_out1[new_loc] = old_out_node1;
            solution.sorter_out2[new_loc] = old_out_node2;

            if (is_new_out1) {
                if (would_intersect(new_node, old_out_node2)) {
                    solution.sorter_out1[new_loc] = ori1;
                    solution.sorter_out2[new_loc] = ori2;
                }
            } else {
                if (would_intersect(new_node, old_out_node1)) {
                    solution.sorter_out1[new_loc] = ori1;
                    solution.sorter_out2[new_loc] = ori2;
                }
            }
        }

        if (roll_back) {
            solution.sorter_out1[new_loc] = -1;
            solution.sorter_out2[new_loc] = -1;
            solution.placed_sorters[new_loc] = -1;
            solution.placed_indices.pop_back();
            solution.sorter_out1[sorter_loc] = old_out_node1;
            solution.sorter_out2[sorter_loc] = old_out_node2;
            return false;
        }

        static vector<vector<float>> ori_node_score;
        static vector<vector<float>> ori_in_probs;

        ori_node_score = optimizer.node_score;
        if (SORTER_PLACE < param.twice_dp_threshold) {
            ori_in_probs = optimizer.in_probs;
        }

        optimizer.solution = solution;
        optimizer.optimize({sorter_node, new_node});
        float new_score = optimizer.calc_score();

        if (should_accept(new_score - score)) {
            score = new_score;
            optimizer.calculate_in_probs();
            solution = optimizer.solution;
            rebuild_edge_cache();
            return true;
        } else {
            solution.sorter_out1[new_loc] = -1;
            solution.sorter_out2[new_loc] = -1;
            solution.placed_sorters[new_loc] = -1;
            solution.placed_indices.pop_back();
            solution.sorter_out1[sorter_loc] = old_out_node1;
            solution.sorter_out2[sorter_loc] = old_out_node2;

            optimizer.solution = solution;
            optimizer.node_score = ori_node_score;
            if (SORTER_PLACE < param.twice_dp_threshold) {
                optimizer.in_probs = ori_in_probs;
            }
            return false;
        }
    }

    // ゴール割り当ての2点swap
    bool op_swap_goal_assignment() {
        // ランダムに2つの処理装置を選択
        int goal1 = rnd(TRASH_KIND);
        int goal2 = rnd(TRASH_KIND);

        if (goal1 == goal2) {
            return false;
        }

        // 現在の割り当てをバックアップ
        int old_assign1 = solution.goal_assignment[goal1];
        int old_assign2 = solution.goal_assignment[goal2];

        // swap実行
        solution.goal_assignment[goal1] = old_assign2;
        solution.goal_assignment[goal2] = old_assign1;

        // スコア計算
        static vector<vector<float>> ori_node_score;
        static vector<vector<float>> ori_in_probs;

        ori_node_score = optimizer.node_score;
        if (SORTER_PLACE < param.twice_dp_threshold) {
            ori_in_probs = optimizer.in_probs;
        }

        optimizer.solution = solution;
        optimizer.optimize({goal1, goal2});
        float new_score = optimizer.calc_score();

        if (should_accept(new_score - score)) {
            score = new_score;
            optimizer.calculate_in_probs();
            solution = optimizer.solution;
            rebuild_edge_cache();
            return true;
        } else {
            // ロールバック
            solution.goal_assignment[goal1] = old_assign1;
            solution.goal_assignment[goal2] = old_assign2;

            optimizer.solution = solution;
            optimizer.node_score = ori_node_score;
            if (SORTER_PLACE < param.twice_dp_threshold) {
                optimizer.in_probs = ori_in_probs;
            }
            return false;
        }
    }

    // ゴール割り当ての2点swap
    bool op_swap_3_goal_assignment() {
        // ランダムに2つの処理装置を選択
        int goal1 = rnd(TRASH_KIND);
        int goal2 = rnd(TRASH_KIND);
        int goal3 = rnd(TRASH_KIND);

        if (goal1 == goal2) {
            return false;
        }
        if (goal1 == goal3) {
            return false;
        }
        if (goal2 == goal3) {
            return false;
        }

        // 現在の割り当てをバックアップ
        int old_assign1 = solution.goal_assignment[goal1];
        int old_assign2 = solution.goal_assignment[goal2];
        int old_assign3 = solution.goal_assignment[goal3];

        // swap実行
        solution.goal_assignment[goal1] = old_assign2;
        solution.goal_assignment[goal2] = old_assign3;
        solution.goal_assignment[goal3] = old_assign1;

        // スコア計算
        static vector<vector<float>> ori_node_score;
        static vector<vector<float>> ori_in_probs;

        ori_node_score = optimizer.node_score;
        if (SORTER_PLACE < param.twice_dp_threshold) {
            ori_in_probs = optimizer.in_probs;
        }

        optimizer.solution = solution;
        optimizer.optimize({goal1, goal2, goal3});
        float new_score = optimizer.calc_score();

        if (should_accept(new_score - score)) {
            score = new_score;
            optimizer.calculate_in_probs();
            solution = optimizer.solution;
            rebuild_edge_cache();
            return true;
        } else {
            // ロールバック
            solution.goal_assignment[goal1] = old_assign1;
            solution.goal_assignment[goal2] = old_assign2;
            solution.goal_assignment[goal3] = old_assign3;

            optimizer.solution = solution;
            optimizer.node_score = ori_node_score;
            if (SORTER_PLACE < param.twice_dp_threshold) {
                optimizer.in_probs = ori_in_probs;
            }
            return false;
        }
    }

    // 閉路検出 (新しい辺 from -> to を追加した時に閉路ができるか)
    bool would_create_cycle(int from, int to) {
        // toからDFSしてfromに到達できるかチェック
        static vector<int> visited(TRASH_KIND + SORTER_PLACE + 1, -1);
        static int seen_iter = 0;
        seen_iter++;
        return dfs_reaches(to, from, visited, seen_iter);
    }

    bool dfs_reaches(int current, int target, vector<int> &visited, int iter) {
        if (current == target)
            return true;
        if (visited[current] == iter)
            return false;
        visited[current] = iter;

        int a, b;
        solution.get_adjs_fast(current, a, b);
        if (a != -1 && dfs_reaches(a, target, visited, iter))
            return true;
        if (b != -1 && dfs_reaches(b, target, visited, iter))
            return true;
        return false;
    }

    // 線分交差チェック
    inline bool would_intersect(int from, int to) {
        return get_intersect_edge(from, to).first != -1;
    }

    inline pair<int, int> get_intersect_edge(int from, int to) {
        const pii &from_pos = node_pos[from];
        const pii &to_pos = node_pos[to];

        // 2D グリッドDDAを整数演算で実装（交差はクロス乗算で比較）
        int cs = grid_cell_size;
        int gx = max(0, min(grid_side - 1, from_pos.first / cs));
        int gy = max(0, min(grid_side - 1, from_pos.second / cs));
        int gx1 = max(0, min(grid_side - 1, to_pos.first / cs));
        int gy1 = max(0, min(grid_side - 1, to_pos.second / cs));

        int dx = to_pos.first - from_pos.first;
        int dy = to_pos.second - from_pos.second;
        int sx = (dx > 0) - (dx < 0);
        int sy = (dy > 0) - (dy < 0);
        int adx = abs(dx);
        int ady = abs(dy);

        // stamping for edge deduplication
        edge_seen_iter++;

        auto process_cell = [&](int cx, int cy) -> pair<int, int> {
            const vector<int> &bucket = grid_cells[cy * grid_side + cx];
            for (int ei : bucket) {
                if (edge_seen_stamp[ei] == edge_seen_iter)
                    continue;
                edge_seen_stamp[ei] = edge_seen_iter;
                const auto &seg = edge_segments_cache[ei];
                if (segments_intersect(from_pos, to_pos, node_pos[seg.first],
                                       node_pos[seg.second])) {
                    return seg;
                }
            }
            return {-1, -1};
        };

        int ogx = gx;
        int ogy = gy;

        bool fallback = false;

        while (true) {
            auto hit = process_cell(gx, gy);
            if (hit.first != -1)
                return hit;
            if (gx == gx1 && gy == gy1)
                break;

            if (adx == 0) {
                gy += sy;
                if (gy < 0) {
                    fallback = true;
                    break;
                } else if (gy >= grid_side) {
                    fallback = true;
                    break;
                }
                continue;
            }
            if (ady == 0) {
                gx += sx;
                if (gx < 0) {
                    fallback = true;
                    break;
                } else if (gx >= grid_side) {
                    fallback = true;
                    break;
                }
                continue;
            }

            // 次の境界までの距離（線分の向きに沿った符号なし距離）
            long long distX = (sx > 0)
                                  ? (long long)((gx + 1) * cs - from_pos.first)
                                  : (long long)(from_pos.first - gx * cs);
            long long distY = (sy > 0)
                                  ? (long long)((gy + 1) * cs - from_pos.second)
                                  : (long long)(from_pos.second - gy * cs);

            // tMaxX <= tMaxY を整数比較: distX/|dx| <= distY/|dy| <=>
            // distX*|dy| <= distY*|dx|
            if (distX * (long long)ady <= distY * (long long)adx) {
                gx += sx;
                if (gx < 0) {
                    fallback = true;
                    break;
                } else if (gx >= grid_side) {
                    fallback = true;
                    break;
                }
            } else {
                gy += sy;
                if (gy < 0) {
                    fallback = true;
                    break;
                } else if (gy >= grid_side) {
                    fallback = true;
                    break;
                }
            }
        }

        if (fallback) {
            rep(ei, edge_segments_cache.size()) {
                if (edge_seen_stamp[ei] == edge_seen_iter)
                    continue;
                edge_seen_stamp[ei] = edge_seen_iter;
                const auto &seg = edge_segments_cache[ei];
                if (segments_intersect(from_pos, to_pos, node_pos[seg.first],
                                       node_pos[seg.second])) {
                    return seg;
                }
            }
        }
        return {-1, -1};
    }

    void prune_less_effective() {
        // 2. 後ろ向きに各ノードのスコアを計算
        static vector<int> is_visited(TRASH_KIND + SORTER_PLACE + 1, -1);
        static int iter = 0;
        iter++;

        auto dfs = [&](auto &&dfs, int node) {
            if (is_visited[node] == iter) {
                return;
            }

            if (node < TRASH_KIND) {
                return;
            }

            is_visited[node] = iter;

            int out_node1 = solution.sorter_out1[node - TRASH_KIND];
            int out_node2 = solution.sorter_out2[node - TRASH_KIND];
            dfs(dfs, out_node1);
            dfs(dfs, out_node2);
        };

        // スタートノードから開始
        dfs(dfs, solution.start_destination);

        vector<int> new_placed_indices;
        for (int i : solution.placed_indices) {
            if (is_visited[TRASH_KIND + i] == iter) {
                new_placed_indices.emplace_back(i);
            } else {
                solution.sorter_out1[i] = -1;
                solution.sorter_out2[i] = -1;
                solution.placed_sorters[i] = -1;
            }
        }

        solution.placed_indices = new_placed_indices;

        optimizer.solution = solution;
        optimizer.optimize();
        optimizer.calculate_in_probs();
        solution = optimizer.solution;

        score = optimizer.calc_score();
    }

    void build_spatial_index() {
        // Reconfigure grid density adaptively to keep buckets small
        const int ecnt = (int)edge_segments_cache.size();
        if (ecnt <= 0) {
            grid_side = 1;
            grid_cell_size = 10000; // whole area
            grid_cells.assign(1, {});
            edge_seen_stamp.assign(0, 0);
            return;
        }

        // target grid size ~ sqrt(E) clamped to [8, 64]
        int target = (int)(sqrt((double)ecnt) * 0.7 + 1);
        if (target < 8)
            target = 8;
        if (target > 64)
            target = 64;
        grid_side = target;
        grid_cell_size = max(1, (10000 + grid_side - 1) / grid_side);

        grid_cells.assign(grid_side * grid_side, {});
        edge_seen_stamp.assign(ecnt, 0);

        for (int i = 0; i < ecnt; i++) {
            const auto &seg = edge_segments_cache[i];
            const pii &a = node_pos[seg.first];
            const pii &b = node_pos[seg.second];
            int minx = min(a.first, b.first);
            int maxx = max(a.first, b.first);
            int miny = min(a.second, b.second);
            int maxy = max(a.second, b.second);

            int x0 = max(0, min(grid_side - 1, minx / grid_cell_size));
            int x1 = max(0, min(grid_side - 1, maxx / grid_cell_size));
            int y0 = max(0, min(grid_side - 1, miny / grid_cell_size));
            int y1 = max(0, min(grid_side - 1, maxy / grid_cell_size));

            for (int gy = y0; gy <= y1; gy++) {
                int base = gy * grid_side;
                for (int gx = x0; gx <= x1; gx++) {
                    grid_cells[base + gx].emplace_back(i);
                }
            }
        }
    }

    pii get_position(int node) { return node_pos[node]; }

    bool should_accept(Cost score_diff) const {
        if (score_diff >= 0) {
            return true;
        }
        return randdouble() < exp(score_diff / temperature);
    }

    bool update_best() {
        if (score > best_score) {
            best_score = score;
            best_solution = solution;
            DEBUG(best_score);
            return true;
        }
        return false;
    }

    // 角度順＋単一ブロックで枝刈りしながら非交差先を選ぶ
    int pick_non_intersecting_precomputed(int sorter_loc, int sorter_node,
                                          bool wants_new) {

        // fill(all(is_usable[sorter_loc]), 1)
        // を避けるため、スタンプ方式で管理
        static vector<vector<int>> blocked_stamp(
            SORTER_PLACE, vector<int>(TRASH_KIND + SORTER_PLACE + 1, 0));
        static vector<int> success_count(SORTER_PLACE, 0);
        static vector<int> blocked_epoch(SORTER_PLACE, 1);

        const auto &vec = g_angle_cands_by_loc[sorter_loc];

        static vector<tuple<int, int, int>> st;
        st.clear();

        static vector<int> last_seen_iter(TRASH_KIND + SORTER_PLACE + 1, -1);
        static int iter = 0;
        iter++;

        const pii &center_pos = node_pos[sorter_node];

        int vec_size = (int)vec.size();
        int idx = rnd(vec_size);
        int dir = rnd(2) ? 1 : -1;
        rep(_, vec_size) {
            idx += dir;
            if (idx == vec_size) {
                idx = 0;
            } else if (idx < 0) {
                idx = vec_size - 1;
            }

            const int node = vec[idx].node;
            // 以降は重い処理。必要なときのみ実行
            last_seen_iter[node] = iter;

            // まずは安価なフィルタで弾く
            if (node == sorter_node)
                continue;
            if (node == solution.sorter_out1[sorter_loc] ||
                node == solution.sorter_out2[sorter_loc])
                continue;
            if (node == solution.start_destination || is_start_node(node))
                continue;
            if (blocked_stamp[sorter_loc][node] == blocked_epoch[sorter_loc])
                continue;
            if (is_sorter_node(node)) {
                bool is_new = solution.placed_sorters[node_to_loc(node)] == -1;
                if (wants_new != is_new)
                    continue;
            } else {
                if (wants_new && is_goal_node(node))
                    continue;
            }

            while (st.size() > 0) {
                auto [node1, node2, ori_cnt] = st.back();
                int cur_cnt = (int)(last_seen_iter[node1] == iter) +
                              (int)(last_seen_iter[node2] == iter);
                if ((cur_cnt & 1) != (ori_cnt & 1)) {
                    st.pop_back();
                } else {
                    break;
                }
            }

            if (st.size() > 0) {
                auto [node1, node2, ori_cnt] = st.back();
                if (!point_in_triangle(node_pos[node1], node_pos[node2],
                                       center_pos, node_pos[node])) {
                    continue;
                }
            }

            auto colision_edge_nodes = get_intersect_edge(sorter_node, node);
            if (colision_edge_nodes.first != -1) {
                int ori_cnt =
                    (int)(last_seen_iter[colision_edge_nodes.first] == iter) +
                    (int)(last_seen_iter[colision_edge_nodes.second] == iter);
                st.emplace_back(colision_edge_nodes.first,
                                colision_edge_nodes.second, ori_cnt);
            } else {
                blocked_stamp[sorter_loc][node] = blocked_epoch[sorter_loc];
                success_count[sorter_loc]++;
                if (success_count[sorter_loc] >= param.ang_reset_interval) {
                    success_count[sorter_loc] = 0;
                    blocked_epoch[sorter_loc]++;
                }
                return node;
            }
        }
        success_count[sorter_loc] = 0;
        blocked_epoch[sorter_loc]++;
        return -1;
    }
};

signed main(int argc, char **argv) {
    param = Parameter::from_env();

    input();
    precompute_angle_lists();

    double best_score = 0.0;
    Solution best_solution;

    rep(try_num, inf) {
#ifdef FREEZE
        if (try_num == 5) {
            break;
        }
#else
        if (calc_seconds() > 0.05 && best_solution.placed_indices.size() > 0) {
            break;
        }
#endif

        ConnectionDecider decider;
        decider.solve();

        // 分別器の種類を最適化
        SorterOptimizer optimizer(decider.solution);
        optimizer.optimize();

        double score = optimizer.calc_score();

        if (score > best_score) {
            best_score = score;
            best_solution = optimizer.solution;
        }
        if (best_solution.placed_indices.size() > SORTER_PLACE * 0.1) {
            break;
        }
    }

    DEBUG("connection decider score");
    DEBUG(best_score);

    // ソーター接続の最適化
    AnnealingSorterConnection state(TL, best_solution);
    state.solve();

    best_score = state.best_score;
    best_solution = state.best_solution;

    DEBUG(best_score);
    DEBUG((1 - best_score) * 1e9);
    // 解を出力
    best_solution.output();

    DEBUG(valid_op_num);

    return 0;
}
