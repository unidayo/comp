//*
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
//*/

#include <bits/stdc++.h>

using namespace std;

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

struct GaussianSampler {
    vector<double> buf;
    size_t idx = 0;

    GaussianSampler(size_t table_size = 1 << 10) { init(table_size); }

    void init(size_t table_size) {
        if (table_size % 2 == 1)
            table_size++; // 2個ずつ埋めるので偶数に
        buf.resize(table_size);
        idx = 0;
        const double two_pi = 2.0 * (double)pi;
        for (size_t i = 0; i < table_size; i += 2) {
            // Box-Muller で2個ずつ生成（初期化時のみコストを払う）
            double u1 = max(1e-12, randdouble());
            double u2 = randdouble();
            double r = sqrt(-2.0 * log(u1));
            double th = two_pi * u2;
            double z0 = r * cos(th);
            double z1 = r * sin(th);
            buf[i] = z0;
            buf[i + 1] = z1;
        }
    }

    inline double sample(double mu = 0.0, double sigma = 1.0) {
        if (idx >= buf.size())
            idx = 0; // wrap
        return mu + sigma * buf[idx++];
    }
};

GaussianSampler gaussian_sampler;

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

// -------------------- これより上は修正してはいけない ------------------

constexpr int N = 20;
constexpr int M = N * N / 2;
constexpr int CELLS = N * N;
static constexpr pii ORIGIN = {0, 0};
static constexpr pii INVALID = {-1, -1};

static array<array<uint8_t, CELLS>, CELLS> dist_tbl;
static bool dist_tbl_inited = false;
inline int pos_id(const pii &p) { return p.first * N + p.second; }
inline void init_dist_tbl() {
    if (dist_tbl_inited)
        return;
    for (int y1 = 0; y1 < N; y1++) {
        for (int x1 = 0; x1 < N; x1++) {
            int id1 = y1 * N + x1;
            for (int y2 = 0; y2 < N; y2++) {
                for (int x2 = 0; x2 < N; x2++) {
                    int id2 = y2 * N + x2;
                    dist_tbl[id1][id2] = (uint8_t)(abs(y1 - y2) + abs(x1 - x2));
                }
            }
        }
    }
    dist_tbl_inited = true;
}

array<array<int, N>, N> a;
vector<vector<pii>> card_positions(M);

void input() {
    int dummy;
    cin >> dummy;
    rep(i, N) {
        rep(j, N) {
            cin >> a[i][j];
            card_positions[a[i][j]].push_back({i, j});
        }
    }
}

string get_path(pii from, pii to) {
    string res;
    int dy_val = to.first - from.first;
    int dx_val = to.second - from.second;
    res.reserve(abs(dy_val) + abs(dx_val));
    rep(_, abs(dy_val)) res += (dy_val > 0 ? 'D' : 'U');
    rep(_, abs(dx_val)) res += (dx_val > 0 ? 'R' : 'L');
    return res;
}

inline int dist(const pii &p1, const pii &p2) {
    return dist_tbl[pos_id(p1)][pos_id(p2)];
}

struct Node {
    int id;
    pii from, to;
    vector<Node> inner;
    int cost;

    Node() : id(-1), from({0, 0}), to({0, 0}), cost(-1) {}
    Node(int id, pii from, pii to, int cost = -1, vector<Node> inner = {})
        : id(id), from(from), to(to), inner(inner), cost(cost) {}

    int calc_cost() {
        if (inner.size() == 0) {
            cost = dist(from, to);
            return cost;
        }
        cost = dist(from, inner[0].from);
        for (int i = 0; i < (int)inner.size(); i++) {
            cost += inner[i].calc_cost();
            if (i + 1 < (int)inner.size()) {
                cost += dist(inner[i].to, inner[i + 1].from);
            } else {
                cost += dist(inner[i].to, to);
            }
        }
        return cost;
    }
};

vector<Node> build_initial() {
    vector<Node> ret;
    vector<bool> is_used(M, false);
    rep(_, M) {
        int best_cost1 = 2e9;
        int best_id1 = -1;
        pii b_from, b_to;
        rep(i, M) {
            if (is_used[i])
                continue;
            int d = dist(card_positions[i][0], card_positions[i][1]);
            if (d < best_cost1) {
                best_cost1 = d;
                best_id1 = i;
                b_from = card_positions[i][0];
                b_to = card_positions[i][1];
            }
        }

        int best_cost2 = 2e9;
        int best_id2 = -1;
        pii b_from2, b_to2;
        int best_child = -1;
        rep(i, M) {
            if (is_used[i])
                continue;
            rep(j, ret.size()) {
                const pii p0 = card_positions[i][0];
                const pii p1 = card_positions[i][1];
                {
                    int d = dist(p0, ret[j].from) + dist(p1, ret[j].to);
                    if (d < best_cost2) {
                        best_cost2 = d;
                        best_id2 = i;
                        b_from2 = p0;
                        b_to2 = p1;
                        best_child = j;
                    }
                }
                {
                    int d = dist(p1, ret[j].from) + dist(p0, ret[j].to);
                    if (d < best_cost2) {
                        best_cost2 = d;
                        best_id2 = i;
                        b_from2 = p1;
                        b_to2 = p0;
                        best_child = j;
                    }
                }
            }
        }

        if (best_cost1 < best_cost2 || best_child == -1) {
            ret.push_back(Node(best_id1, b_from, b_to, best_cost1));
            is_used[best_id1] = true;
        } else {
            int new_cost = best_cost2 + ret[best_child].cost;
            ret[best_child] =
                Node(best_id2, b_from2, b_to2, new_cost, {ret[best_child]});
            is_used[best_id2] = true;
        }
    }

    vector<Node> node2;
    pii now_pos = ORIGIN;
    while (!ret.empty()) {
        int best_dist = 2e9;
        int best_idx = -1;
        rep(i, ret.size()) {
            int d = dist(now_pos, ret[i].from);
            if (d < best_dist) {
                best_dist = d;
                best_idx = i;
            }
        }
        node2.push_back(ret[best_idx]);
        now_pos = ret[best_idx].to;
        ret.erase(ret.begin() + best_idx);
    }
    return node2;
}

struct FlatNode {
    int id;
    pii from, to;
    int internal_cost;
    FlatNode() : id(-1), from(ORIGIN), to(ORIGIN), internal_cost(0) {}
    FlatNode(int id, pii from, pii to)
        : id(id), from(from), to(to), internal_cost(0) {}
};

struct AnnealingState {
    using Cost = int;
    Cost score;
    Cost best_score;
    vector<FlatNode> nodes;
    vector<vector<int>> children;
    vector<int> parent;
    vector<int> child_index; // 親のchildren内でのインデックス（高速化用）
    vector<vector<int>> best_children;
    vector<int> best_parent;
    vector<int> best_child_index;
    vector<bool> is_reverse;
    vector<bool> best_is_reverse;
    vector<bool> eff_rev;
    vector<bool> best_eff_rev;

    double start_sec, end_sec, temperature;

    AnnealingState(double end_sec)
        : start_sec(calc_seconds()), end_sec(end_sec) {
        nodes.resize(M + 1);
        children.resize(M + 1);
        parent.assign(M + 1, -1);
        child_index.assign(M + 1, -1);
        is_reverse.assign(M + 1, false);
        eff_rev.assign(M + 1, false);
        init();
    }

    void init() {
        auto init_nodes = build_initial();
        rep(i, M) nodes[i] =
            FlatNode(i, card_positions[i][0], card_positions[i][1]);
        nodes[M] = FlatNode(-1, ORIGIN, ORIGIN);

        for (const auto &n : init_nodes) {
            int root_child = convert_tree(n);
            child_index[root_child] = children[M].size();
            children[M].push_back(root_child);
            parent[root_child] = M;
        }
        update_all_eff_rev();
        recalculate_all_costs();
        score = nodes[M].internal_cost;
        best_score = score;
        best_children = children;
        best_is_reverse = is_reverse;
        best_eff_rev = eff_rev;
        best_parent = parent;
        best_child_index = child_index;
    }

    int convert_tree(const Node &old_node) {
        int u = old_node.id;
        nodes[u] = FlatNode(old_node.id, old_node.from, old_node.to);
        for (const auto &child : old_node.inner) {
            int c_idx = convert_tree(child);
            child_index[c_idx] = children[u].size();
            children[u].push_back(c_idx);
            parent[c_idx] = u;
        }
        return u;
    }

    void update_all_eff_rev() {
        // iterative (faster than recursive lambda)
        static vector<int> st;
        static vector<uint8_t> st_rev;
        st.clear();
        st_rev.clear();
        st.reserve(M + 1);
        st_rev.reserve(M + 1);
        st.push_back(M);
        st_rev.push_back(0);
        while (!st.empty()) {
            int u = st.back();
            bool rev = (bool)st_rev.back();
            st.pop_back();
            st_rev.pop_back();
            eff_rev[u] = rev;
            const auto &ch = children[u];
            // push reverse order so traversal order matches recursion (not
            // required, but keeps it stable)
            for (int i = (int)ch.size() - 1; i >= 0; i--) {
                int c = ch[i];
                st.push_back(c);
                st_rev.push_back((uint8_t)(rev ^ is_reverse[c]));
            }
        }
    }

    void update_subtree_eff_rev(int u) {
        bool p_rev = (parent[u] == M ? false : eff_rev[parent[u]]);
        bool start_rev = p_rev ^ is_reverse[u];
        static vector<int> st;
        static vector<uint8_t> st_rev;
        st.clear();
        st_rev.clear();
        st.reserve(M + 1);
        st_rev.reserve(M + 1);
        st.push_back(u);
        st_rev.push_back((uint8_t)start_rev);
        while (!st.empty()) {
            int v = st.back();
            bool rev = (bool)st_rev.back();
            st.pop_back();
            st_rev.pop_back();
            eff_rev[v] = rev;
            const auto &ch = children[v];
            for (int i = (int)ch.size() - 1; i >= 0; i--) {
                int c = ch[i];
                st.push_back(c);
                st_rev.push_back((uint8_t)(rev ^ is_reverse[c]));
            }
        }
    }

    pii get_eff_from(int u) const {
        return eff_rev[u] ? nodes[u].to : nodes[u].from;
    }
    pii get_eff_to(int u) const {
        return eff_rev[u] ? nodes[u].from : nodes[u].to;
    }
    pii get_eff_from_with_rev(int u, bool rev) const {
        return rev ? nodes[u].to : nodes[u].from;
    }
    pii get_eff_to_with_rev(int u, bool rev) const {
        return rev ? nodes[u].from : nodes[u].to;
    }

    void get_neighbors(int u, pii &prev_to, pii &next_from) const {
        int p = parent[u];
        int u_idx = child_index[u];
        bool p_rev = (p == M ? false : eff_rev[p]);
        int n = children[p].size();
        if (!p_rev) {
            pii p_from = (p == M ? ORIGIN : nodes[p].from);
            prev_to =
                (u_idx == 0 ? p_from : get_eff_to(children[p][u_idx - 1]));
            if (u_idx == n - 1)
                next_from = (p == M ? INVALID : nodes[p].to);
            else
                next_from = get_eff_from(children[p][u_idx + 1]);
        } else {
            pii p_to = nodes[p].to;
            prev_to =
                (u_idx == n - 1 ? p_to : get_eff_to(children[p][u_idx + 1]));
            if (u_idx == 0)
                next_from = nodes[p].from;
            else
                next_from = get_eff_from(children[p][u_idx - 1]);
        }
    }

    int recalculate_all_costs() {
        auto rec = [&](auto self, int u) -> int {
            if (children[u].empty()) {
                nodes[u].internal_cost =
                    (u == M ? 0 : dist(nodes[u].from, nodes[u].to));
                return nodes[u].internal_cost;
            }
            bool rev = eff_rev[u];
            int c = 0;
            pii curr_pos =
                (u == M ? ORIGIN : (rev ? nodes[u].to : nodes[u].from));
            if (!rev) {
                rep(i, children[u].size()) {
                    int child = children[u][i];
                    c += dist(curr_pos, get_eff_from(child));
                    c += self(self, child);
                    curr_pos = get_eff_to(child);
                }
                if (u != M)
                    c += dist(curr_pos, nodes[u].to);
            } else {
                for (int i = (int)children[u].size() - 1; i >= 0; i--) {
                    int child = children[u][i];
                    c += dist(curr_pos, get_eff_from(child));
                    c += self(self, child);
                    curr_pos = get_eff_to(child);
                }
                c += dist(curr_pos, nodes[u].from);
            }
            nodes[u].internal_cost = c;
            return c;
        };
        return rec(rec, M);
    }

    bool is_ancestor(int u, int v) {
        if (u == v)
            return true;
        int curr = v;
        while (curr != -1) {
            if (curr == u)
                return true;
            curr = parent[curr];
        }
        return false;
    }

    void step() {
        int op = rnd(52);
        if (op < 10) {
            op_move_parent();
        } else if (op < 20) {
            op_swap_nodes();
        } else if (op < 30) {
            op_move_segments();
        } else if (op < 40) {
            op_swap_segments();
        } else if (op < 50) {
            op_reverse_segment();
        } else if (op < 51) {
            op_promote();
        } else if (op < 52) {
            op_demote();
        } else {
            assert(false);
        }
    }

    void op_swap_segments() {
        int p1 = rnd(M + 1);
        int p2 = rnd(M + 1);
        if (p1 == p2 || children[p1].empty() || children[p2].empty())
            return;

        int i1 = rnd(children[p1].size()), j1 = rnd(children[p1].size());
        if (i1 > j1)
            swap(i1, j1);
        int i2 = rnd(children[p2].size()), j2 = rnd(children[p2].size());
        if (i2 > j2)
            swap(i2, j2);

        // Ancestor check
        for (int k = i1; k <= j1; ++k)
            if (is_ancestor(children[p1][k], p2))
                return;
        for (int k = i2; k <= j2; ++k)
            if (is_ancestor(children[p2][k], p1))
                return;

        auto get_endpoints = [&](int p, int i, int j) {
            bool p_rev = (p == M ? false : eff_rev[p]);
            int n = children[p].size();
            pii p_start =
                (p == M ? ORIGIN : (p_rev ? nodes[p].to : nodes[p].from));
            pii p_end =
                (p == M ? INVALID : (p_rev ? nodes[p].from : nodes[p].to));
            pii prev_t, next_f;
            if (!p_rev) {
                prev_t = (i == 0 ? p_start : get_eff_to(children[p][i - 1]));
                next_f =
                    (j == n - 1 ? p_end : get_eff_from(children[p][j + 1]));
            } else {
                prev_t =
                    (j == n - 1 ? p_start : get_eff_to(children[p][j + 1]));
                next_f = (i == 0 ? p_end : get_eff_from(children[p][i - 1]));
            }
            return make_pair(prev_t, next_f);
        };

        auto info1 = get_endpoints(p1, i1, j1);
        auto info2 = get_endpoints(p2, i2, j2);

        auto get_seg_ends = [&](int p, int i, int j, bool flip, bool to_p_rev) {
            int v_entry =
                (!to_p_rev ? (!flip ? children[p][i] : children[p][j])
                           : (!flip ? children[p][j] : children[p][i]));
            int v_exit =
                (!to_p_rev ? (!flip ? children[p][j] : children[p][i])
                           : (!flip ? children[p][i] : children[p][j]));
            bool eff_rev_entry = is_reverse[v_entry] ^ flip ^ to_p_rev;
            bool eff_rev_exit = is_reverse[v_exit] ^ flip ^ to_p_rev;
            pii entry_pos =
                (eff_rev_entry ? nodes[v_entry].to : nodes[v_entry].from);
            pii exit_pos =
                (eff_rev_exit ? nodes[v_exit].from : nodes[v_exit].to);
            return make_pair(entry_pos, exit_pos);
        };

        Cost best_diff = 2e9;
        bool bf1 = false, bf2 = false;

        bool p1_rev = (p1 == M ? false : eff_rev[p1]);
        bool p2_rev = (p2 == M ? false : eff_rev[p2]);

        // Original connections to remove
        auto s1_orig = get_seg_ends(p1, i1, j1, false, p1_rev);
        auto s2_orig = get_seg_ends(p2, i2, j2, false, p2_rev);
        Cost base_rem = -dist(info1.first, s1_orig.first) -
                        dist(info2.first, s2_orig.first);
        if (info1.second != INVALID)
            base_rem -= dist(s1_orig.second, info1.second);
        if (info2.second != INVALID)
            base_rem -= dist(s2_orig.second, info2.second);

        for (bool f1 : {false, true}) {
            auto s1_new = get_seg_ends(p1, i1, j1, f1, p2_rev);
            for (bool f2 : {false, true}) {
                auto s2_new = get_seg_ends(p2, i2, j2, f2, p1_rev);

                Cost d = dist(info1.first, s2_new.first) +
                         dist(info2.first, s1_new.first);
                if (info1.second != INVALID)
                    d += dist(s2_new.second, info1.second);
                if (info2.second != INVALID)
                    d += dist(s1_new.second, info2.second);

                if (d + base_rem < best_diff) {
                    best_diff = d + base_rem;
                    bf1 = f1;
                    bf2 = f2;
                }
            }
        }

        if (should_accept(best_diff)) {
            score += best_diff;
            vector<int> s1, s2;
            s1.reserve(j1 - i1 + 1);
            s2.reserve(j2 - i2 + 1);
            for (int k = i1; k <= j1; ++k)
                s1.push_back(children[p1][k]);
            for (int k = i2; k <= j2; ++k)
                s2.push_back(children[p2][k]);

            if (bf1) {
                reverse(all(s1));
                for (int node : s1)
                    is_reverse[node] = !is_reverse[node];
            }
            if (bf2) {
                reverse(all(s2));
                for (int node : s2)
                    is_reverse[node] = !is_reverse[node];
            }

            children[p1].erase(children[p1].begin() + i1,
                               children[p1].begin() + j1 + 1);
            children[p2].erase(children[p2].begin() + i2,
                               children[p2].begin() + j2 + 1);

            children[p1].insert(children[p1].begin() + i1, all(s2));
            children[p2].insert(children[p2].begin() + i2, all(s1));

            for (int node : s1) {
                parent[node] = p2;
                update_subtree_eff_rev(node);
            }
            for (int node : s2) {
                parent[node] = p1;
                update_subtree_eff_rev(node);
            }
            // child_index を更新
            for (int i = i1; i < (int)children[p1].size(); i++)
                child_index[children[p1][i]] = i;
            for (int i = i2; i < (int)children[p2].size(); i++)
                child_index[children[p2][i]] = i;
            update_best();
        }
    }

    void op_move_segments() {
        int p1 = rnd(M + 1);
        if (children[p1].empty())
            return;
        int i1 = rnd(children[p1].size()), j1 = rnd(children[p1].size());
        if (i1 > j1)
            swap(i1, j1);

        int p2 = rnd(M + 1);
        if (p1 == p2)
            return; // 簡略化のため

        for (int k = i1; k <= j1; ++k)
            if (is_ancestor(children[p1][k], p2))
                return;

        int n2 = children[p2].size();
        int pos2 = rnd(n2 + 1);

        auto get_seg_endpoints = [&](int p, int i, int j) {
            bool p_rev = (p == M ? false : eff_rev[p]);
            int n = children[p].size();
            pii p_start =
                (p == M ? ORIGIN : (p_rev ? nodes[p].to : nodes[p].from));
            pii p_end =
                (p == M ? INVALID : (p_rev ? nodes[p].from : nodes[p].to));
            pii prev_t, next_f;
            if (!p_rev) {
                prev_t = (i == 0 ? p_start : get_eff_to(children[p][i - 1]));
                next_f =
                    (j == n - 1 ? p_end : get_eff_from(children[p][j + 1]));
            } else {
                prev_t =
                    (j == n - 1 ? p_start : get_eff_to(children[p][j + 1]));
                next_f = (i == 0 ? p_end : get_eff_from(children[p][i - 1]));
            }
            int start_node = (!p_rev ? children[p][i] : children[p][j]);
            int end_node = (!p_rev ? children[p][j] : children[p][i]);
            return make_tuple(prev_t, next_f, start_node, end_node);
        };

        auto info1 = get_seg_endpoints(p1, i1, j1);
        pii p1_prev_t = get<0>(info1);
        pii p1_next_f = get<1>(info1);
        int v_start = get<2>(info1);
        int v_end = get<3>(info1);

        Cost diff_remove = -dist(p1_prev_t, get_eff_from(v_start));
        if (p1_next_f != INVALID) {
            diff_remove -= dist(get_eff_to(v_end), p1_next_f);
            diff_remove += dist(p1_prev_t, p1_next_f);
        }

        auto get_ins_endpoints = [&](int p, int pos) {
            bool p_rev = (p == M ? false : eff_rev[p]);
            int n = children[p].size();
            pii p_start =
                (p == M ? ORIGIN : (p_rev ? nodes[p].to : nodes[p].from));
            pii p_end =
                (p == M ? INVALID : (p_rev ? nodes[p].from : nodes[p].to));
            pii prev_t, next_f;
            if (!p_rev) {
                prev_t =
                    (pos == 0 ? p_start : get_eff_to(children[p][pos - 1]));
                next_f = (pos == n ? p_end : get_eff_from(children[p][pos]));
            } else {
                prev_t = (pos == n ? p_start : get_eff_to(children[p][pos]));
                next_f =
                    (pos == 0 ? p_end : get_eff_from(children[p][pos - 1]));
            }
            return make_pair(prev_t, next_f);
        };

        auto info2 = get_ins_endpoints(p2, pos2);
        pii p2_prev_t = info2.first;
        pii p2_next_f = info2.second;

        auto calc_ins_diff = [&](bool flip) {
            bool p2_rev = (p2 == M ? false : eff_rev[p2]);
            int v_entry =
                (!p2_rev ? (!flip ? children[p1][i1] : children[p1][j1])
                         : (!flip ? children[p1][j1] : children[p1][i1]));
            int v_exit =
                (!p2_rev ? (!flip ? children[p1][j1] : children[p1][i1])
                         : (!flip ? children[p1][i1] : children[p1][j1]));
            bool eff_rev_entry = is_reverse[v_entry] ^ flip ^ p2_rev;
            bool eff_rev_exit = is_reverse[v_exit] ^ flip ^ p2_rev;
            pii entry_pos =
                (eff_rev_entry ? nodes[v_entry].to : nodes[v_entry].from);
            pii exit_pos =
                (eff_rev_exit ? nodes[v_exit].from : nodes[v_exit].to);
            Cost d = dist(p2_prev_t, entry_pos);
            if (p2_next_f != INVALID) {
                d -= dist(p2_prev_t, p2_next_f);
                d += dist(exit_pos, p2_next_f);
            }
            return d;
        };

        Cost d_orig = calc_ins_diff(false);
        Cost d_flip = calc_ins_diff(true);
        bool do_flip = (d_flip < d_orig);
        Cost best_diff = diff_remove + (do_flip ? d_flip : d_orig);

        if (should_accept(best_diff)) {
            score += best_diff;
            vector<int> seg;
            seg.reserve(j1 - i1 + 1);
            for (int k = i1; k <= j1; ++k)
                seg.push_back(children[p1][k]);
            children[p1].erase(children[p1].begin() + i1,
                               children[p1].begin() + j1 + 1);
            if (do_flip) {
                reverse(all(seg));
                for (int node : seg)
                    is_reverse[node] = !is_reverse[node];
            }
            children[p2].insert(children[p2].begin() + pos2, all(seg));
            for (int node : seg) {
                parent[node] = p2;
                update_subtree_eff_rev(node);
            }
            // child_index を更新
            for (int i = i1; i < (int)children[p1].size(); i++)
                child_index[children[p1][i]] = i;
            for (int i = pos2; i < (int)children[p2].size(); i++)
                child_index[children[p2][i]] = i;
            update_best();
        }
    }

    void op_swap_nodes() {
        int u = rnd(M);
        int v = rnd(M);
        if (u == v || is_ancestor(u, v) || is_ancestor(v, u))
            return;

        int p_u = parent[u];
        int p_v = parent[v];

        int u_idx = child_index[u];
        int v_idx = child_index[v];

        // 隣接している場合などは一旦除外して安全に計算
        if (p_u == p_v && abs(u_idx - v_idx) <= 1)
            return;

        auto get_neighbors_for_replace = [&](int p, int idx) {
            bool p_rev = (p == M ? false : eff_rev[p]);
            int n = children[p].size();
            pii prev_t, next_f;
            if (!p_rev) {
                pii p_start = (p == M ? pii(0, 0) : nodes[p].from);
                pii p_end = (p == M ? pii(-1, -1) : nodes[p].to);
                prev_t =
                    (idx == 0 ? p_start : get_eff_to(children[p][idx - 1]));
                next_f =
                    (idx == n - 1 ? p_end : get_eff_from(children[p][idx + 1]));
            } else {
                pii p_start = nodes[p].to;
                pii p_end = nodes[p].from;
                prev_t =
                    (idx == n - 1 ? p_start : get_eff_to(children[p][idx + 1]));
                next_f =
                    (idx == 0 ? p_end : get_eff_from(children[p][idx - 1]));
            }
            return make_pair(prev_t, next_f);
        };

        auto neighbors_u = get_neighbors_for_replace(p_u, u_idx);
        auto neighbors_v = get_neighbors_for_replace(p_v, v_idx);

        Cost base_diff = 0;
        base_diff -= dist(neighbors_u.first, get_eff_from(u));
        if (neighbors_u.second != INVALID)
            base_diff -= dist(get_eff_to(u), neighbors_u.second);
        base_diff -= dist(neighbors_v.first, get_eff_from(v));
        if (neighbors_v.second != INVALID)
            base_diff -= dist(get_eff_to(v), neighbors_v.second);

        auto calc_ins_diff = [&](int node, bool node_is_rev, pii prev_t,
                                 pii next_f, int p) {
            bool p_rev = (p == M ? false : eff_rev[p]);
            bool eff_rev_node = node_is_rev ^ p_rev;
            pii eff_f = get_eff_from_with_rev(node, eff_rev_node);
            pii eff_t = get_eff_to_with_rev(node, eff_rev_node);
            Cost d = dist(prev_t, eff_f);
            if (next_f != INVALID)
                d += dist(eff_t, next_f);
            return d;
        };

        Cost best_total_ins_diff = 2e9;
        bool best_u_flip = false;
        bool best_v_flip = false;

        for (bool u_flip : {false, true}) {
            for (bool v_flip : {false, true}) {
                Cost d =
                    calc_ins_diff(u, is_reverse[u] ^ u_flip, neighbors_v.first,
                                  neighbors_v.second, p_v) +
                    calc_ins_diff(v, is_reverse[v] ^ v_flip, neighbors_u.first,
                                  neighbors_u.second, p_u);
                if (d < best_total_ins_diff) {
                    best_total_ins_diff = d;
                    best_u_flip = u_flip;
                    best_v_flip = v_flip;
                }
            }
        }

        Cost total_diff = base_diff + best_total_ins_diff;

        if (should_accept(total_diff)) {
            score += total_diff;
            if (best_u_flip)
                is_reverse[u] = !is_reverse[u];
            if (best_v_flip)
                is_reverse[v] = !is_reverse[v];

            children[p_u][u_idx] = v;
            children[p_v][v_idx] = u;
            parent[u] = p_v;
            parent[v] = p_u;
            child_index[u] = v_idx;
            child_index[v] = u_idx;

            update_subtree_eff_rev(u);
            update_subtree_eff_rev(v);
            update_best();
        }
    }

    void op_move_parent() {
        int u = rnd(M);
        int old_p = parent[u];
        int new_p = rnd(M + 1);
        if (is_ancestor(u, new_p))
            return;
        int u_idx = child_index[u];
        int new_pos = rnd(children[new_p].size() + 1);
        if (old_p == new_p && (new_pos == u_idx || new_pos == u_idx + 1))
            return;

        Cost diff_remove = 0;
        pii eff_f = get_eff_from(u);
        pii eff_t = get_eff_to(u);
        {
            pii prev_t, next_f;
            get_neighbors(u, prev_t, next_f);
            diff_remove -= dist(prev_t, eff_f);
            if (next_f != INVALID) {
                diff_remove -= dist(eff_t, next_f);
                diff_remove += dist(prev_t, next_f);
            }
        }

        bool new_p_rev = (new_p == M ? false : eff_rev[new_p]);

        auto calc_insert_diff = [&](bool u_is_rev) {
            bool new_eff_rev_u = u_is_rev ^ new_p_rev;
            pii new_eff_f = get_eff_from_with_rev(u, new_eff_rev_u);
            pii new_eff_t = get_eff_to_with_rev(u, new_eff_rev_u);
            Cost d = 0;
            pii p_start = (new_p == M ? ORIGIN
                                      : (new_p_rev ? nodes[new_p].to
                                                   : nodes[new_p].from));
            pii p_end = (new_p == M ? INVALID
                                    : (new_p_rev ? nodes[new_p].from
                                                 : nodes[new_p].to));
            pii prev_t, next_f;
            if (!new_p_rev) {
                prev_t =
                    (new_pos == 0 ? p_start
                                  : get_eff_to(children[new_p][new_pos - 1]));
                next_f = (new_pos == (int)children[new_p].size()
                              ? p_end
                              : get_eff_from(children[new_p][new_pos]));
            } else {
                prev_t = (new_pos == (int)children[new_p].size()
                              ? p_start
                              : get_eff_to(children[new_p][new_pos]));
                next_f =
                    (new_pos == 0 ? p_end
                                  : get_eff_from(children[new_p][new_pos - 1]));
            }
            if (next_f != INVALID)
                d -= dist(prev_t, next_f);
            d += dist(prev_t, new_eff_f);
            if (next_f != INVALID)
                d += dist(new_eff_t, next_f);
            return d;
        };

        Cost diff_orig = diff_remove + calc_insert_diff(is_reverse[u]);
        Cost diff_flip = diff_remove + calc_insert_diff(!is_reverse[u]);

        bool do_flip = false;
        Cost best_diff = diff_orig;
        if (diff_flip < diff_orig) {
            best_diff = diff_flip;
            do_flip = true;
        }

        if (should_accept(best_diff)) {
            score += best_diff;
            if (do_flip)
                is_reverse[u] = !is_reverse[u];
            children[old_p].erase(children[old_p].begin() + u_idx);
            // old_p の u_idx 以降のノードの child_index を更新
            for (int i = u_idx; i < (int)children[old_p].size(); i++)
                child_index[children[old_p][i]] = i;
            int actual_new_pos = new_pos;
            if (old_p == new_p && new_pos > u_idx)
                actual_new_pos--;
            children[new_p].insert(children[new_p].begin() + actual_new_pos, u);
            // new_p の actual_new_pos 以降のノードの child_index を更新
            for (int i = actual_new_pos; i < (int)children[new_p].size(); i++)
                child_index[children[new_p][i]] = i;
            parent[u] = new_p;
            update_subtree_eff_rev(u);
            update_best();
        }
    }

    void op_reverse(bool must_accept = false) {
        int u = rnd(M);
        pii prev_t, next_f;
        get_neighbors(u, prev_t, next_f);
        Cost diff = 0;
        diff -= dist(prev_t, get_eff_from(u));
        if (next_f != INVALID)
            diff -= dist(get_eff_to(u), next_f);
        is_reverse[u] = !is_reverse[u];
        bool new_eff_rev_u = !eff_rev[u];
        diff += dist(prev_t, get_eff_from_with_rev(u, new_eff_rev_u));
        if (next_f != INVALID)
            diff += dist(get_eff_to_with_rev(u, new_eff_rev_u), next_f);

        if (must_accept || should_accept(diff)) {
            score += diff;
            update_subtree_eff_rev(u);
            update_best();
        } else
            is_reverse[u] = !is_reverse[u];
    }

    void op_reverse_segment(bool must_accept = false) {
        int p = rnd(M + 1);
        int n = children[p].size();
        if (n < 2)
            return;
        int i = rnd(n);
        int j = rnd(n);
        if (i == j)
            return;
        if (i > j)
            swap(i, j);

        bool p_rev = (p == M ? false : eff_rev[p]);
        int S_idx = (!p_rev ? i : j);
        int E_idx = (!p_rev ? j : i);
        int S = children[p][S_idx];
        int E = children[p][E_idx];

        pii prev_t, next_f, dummy;
        get_neighbors(S, prev_t, dummy);
        get_neighbors(E, dummy, next_f);

        Cost diff = 0;
        diff -= dist(prev_t, get_eff_from(S));
        if (next_f != INVALID)
            diff -= dist(get_eff_to(E), next_f);

        // After reversal, the segment traversal is reversed.
        // Start node becomes old end node (flipped), end node becomes old start
        // node (flipped).
        diff += dist(prev_t, get_eff_to(E));
        if (next_f != INVALID)
            diff += dist(get_eff_from(S), next_f);

        if (must_accept || should_accept(diff)) {
            score += diff;
            reverse(children[p].begin() + i, children[p].begin() + j + 1);
            for (int k = i; k <= j; k++) {
                int child = children[p][k];
                child_index[child] = k; // child_index を更新
                is_reverse[child] = !is_reverse[child];
                update_subtree_eff_rev(child);
            }
            update_best();
        }
    }

    // 昇格: 子ノードを親の兄弟に昇格させる
    void op_promote() {
        int p = rnd(M); // 親ノード (ルートMは除く)
        if (children[p].empty())
            return;
        int gp = parent[p]; // 祖父ノード
        if (gp == -1)
            return;

        int c_idx = rnd(children[p].size());
        int c = children[p][c_idx];

        // 現在のコスト計算用の位置
        pii prev_c, next_c;
        get_neighbors(c, prev_c, next_c);
        pii prev_p, next_p;
        get_neighbors(p, prev_p, next_p);

        // 削除コスト
        Cost diff = 0;
        diff -= dist(prev_c, get_eff_from(c));
        if (next_c != INVALID)
            diff -= dist(get_eff_to(c), next_c);

        // cを削除した後のpの内部でのつなぎ直し
        if (next_c != INVALID)
            diff += dist(prev_c, next_c);

        // cをpの直後(または直前)に挿入
        // pの後ろに挿入するケースを試す
        bool gp_rev = (gp == M ? false : eff_rev[gp]);
        int p_idx = child_index[p];
        int n_gp = children[gp].size();

        // pの直後に挿入する位置を計算
        int insert_pos = (!gp_rev ? p_idx + 1 : p_idx);

        pii ins_prev_t, ins_next_f;
        {
            pii gp_start =
                (gp == M ? ORIGIN : (gp_rev ? nodes[gp].to : nodes[gp].from));
            pii gp_end =
                (gp == M ? INVALID : (gp_rev ? nodes[gp].from : nodes[gp].to));
            if (!gp_rev) {
                ins_prev_t = (insert_pos == 0
                                  ? gp_start
                                  : get_eff_to(children[gp][insert_pos - 1]));
                ins_next_f = (insert_pos == n_gp
                                  ? gp_end
                                  : get_eff_from(children[gp][insert_pos]));
            } else {
                ins_prev_t =
                    (insert_pos == n_gp ? gp_start
                                        : get_eff_to(children[gp][insert_pos]));
                ins_next_f = (insert_pos == 0
                                  ? gp_end
                                  : get_eff_from(children[gp][insert_pos - 1]));
            }
        }

        // 挿入コストを計算（反転も考慮）
        auto calc_promote_cost = [&](bool flip) {
            bool new_eff_rev_c = is_reverse[c] ^ flip ^ gp_rev;
            pii new_from = get_eff_from_with_rev(c, new_eff_rev_c);
            pii new_to = get_eff_to_with_rev(c, new_eff_rev_c);
            Cost d = 0;
            if (ins_next_f != INVALID)
                d -= dist(ins_prev_t, ins_next_f);
            d += dist(ins_prev_t, new_from);
            if (ins_next_f != INVALID)
                d += dist(new_to, ins_next_f);
            return d;
        };

        Cost d_orig = calc_promote_cost(false);
        Cost d_flip = calc_promote_cost(true);
        bool do_flip = (d_flip < d_orig);
        Cost best_diff = diff + (do_flip ? d_flip : d_orig);

        if (should_accept(best_diff)) {
            score += best_diff;
            if (do_flip)
                is_reverse[c] = !is_reverse[c];

            // pからcを削除
            children[p].erase(children[p].begin() + c_idx);
            for (int i = c_idx; i < (int)children[p].size(); i++)
                child_index[children[p][i]] = i;

            // gpにcを挿入
            children[gp].insert(children[gp].begin() + insert_pos, c);
            for (int i = insert_pos; i < (int)children[gp].size(); i++)
                child_index[children[gp][i]] = i;

            parent[c] = gp;
            update_subtree_eff_rev(c);
            update_best();
        }
    }

    // 降格: 兄弟ノードを別の兄弟の子にする
    void op_demote() {
        int p = rnd(M + 1);
        if (children[p].size() < 2)
            return;

        int u_idx = rnd(children[p].size());
        int v_idx = rnd(children[p].size());
        if (u_idx == v_idx)
            return;

        int u = children[p][u_idx]; // uをvの子にする
        int v = children[p][v_idx]; // vが新しい親

        // uの現在の隣接情報
        pii prev_u, next_u;
        get_neighbors(u, prev_u, next_u);

        // 削除コスト
        Cost diff = 0;
        diff -= dist(prev_u, get_eff_from(u));
        if (next_u != INVALID)
            diff -= dist(get_eff_to(u), next_u);
        if (next_u != INVALID)
            diff += dist(prev_u, next_u);

        // vの子リストの先頭または末尾に挿入
        bool v_rev = eff_rev[v];
        int n_v = children[v].size();
        int insert_pos = rnd(n_v + 1);

        pii ins_prev_t, ins_next_f;
        {
            pii v_start = (v_rev ? nodes[v].to : nodes[v].from);
            pii v_end = (v_rev ? nodes[v].from : nodes[v].to);
            if (!v_rev) {
                ins_prev_t =
                    (insert_pos == 0 ? v_start
                                     : get_eff_to(children[v][insert_pos - 1]));
                ins_next_f =
                    (insert_pos == n_v ? v_end
                                       : get_eff_from(children[v][insert_pos]));
            } else {
                ins_prev_t =
                    (insert_pos == n_v ? v_start
                                       : get_eff_to(children[v][insert_pos]));
                ins_next_f = (insert_pos == 0
                                  ? v_end
                                  : get_eff_from(children[v][insert_pos - 1]));
            }
        }

        auto calc_demote_cost = [&](bool flip) {
            bool new_eff_rev_u = is_reverse[u] ^ flip ^ v_rev;
            pii new_from = get_eff_from_with_rev(u, new_eff_rev_u);
            pii new_to = get_eff_to_with_rev(u, new_eff_rev_u);
            Cost d = 0;
            d -= dist(ins_prev_t, ins_next_f);
            d += dist(ins_prev_t, new_from);
            d += dist(new_to, ins_next_f);
            return d;
        };

        Cost d_orig = calc_demote_cost(false);
        Cost d_flip = calc_demote_cost(true);
        bool do_flip = (d_flip < d_orig);
        Cost best_diff = diff + (do_flip ? d_flip : d_orig);

        if (should_accept(best_diff)) {
            score += best_diff;
            if (do_flip)
                is_reverse[u] = !is_reverse[u];

            // pからuを削除
            children[p].erase(children[p].begin() + u_idx);
            for (int i = u_idx; i < (int)children[p].size(); i++)
                child_index[children[p][i]] = i;

            // v_idxの再計算（u_idxより後ろだった場合はずれる）
            int new_v_idx = child_index[v];

            // vにuを挿入
            children[v].insert(children[v].begin() + insert_pos, u);
            for (int i = insert_pos; i < (int)children[v].size(); i++)
                child_index[children[v][i]] = i;

            parent[u] = v;
            update_subtree_eff_rev(u);
            update_best();
        }
    }

    bool should_accept(Cost score_diff) const {
        if (score_diff <= 0)
            return true;
        return randdouble() < exp(-score_diff / temperature);
    }

    bool update_best() {
        if (score < best_score) {
            best_score = score;
            best_children = children;
            best_is_reverse = is_reverse;
            best_eff_rev = eff_rev;
            best_parent = parent;
            best_child_index = child_index;
            DEBUG(best_score);
            return true;
        }
        return false;
    }

    void kick() {
        return;
        children = best_children;
        is_reverse = best_is_reverse;
        eff_rev = best_eff_rev;
        parent = best_parent;
        child_index = best_child_index;

        rep(_, 1) {
            if (rnd(2)) {
                op_reverse(true);
            } else {
                op_reverse_segment(true);
            }
        }
        update_all_eff_rev();
        score = recalculate_all_costs();
    }

    void solve() {
        double start_temp = 1.0, end_temp = 0.001;
        int last_improve_iter = 0;
        Cost prev_best = best_score;
        rep(try_num, 100000000) {
            if (try_num % 1024 == 0) {
                double sec = calc_seconds();
                if (sec > end_sec) {
                    DEBUG(try_num);
                    break;
                }
                temperature =
                    start_temp + (end_temp - start_temp) * sec / end_sec;
            }
            step();
            if (best_score < prev_best) {
                prev_best = best_score;
                last_improve_iter = try_num;
            }
            if (try_num - last_improve_iter > 1000000) {
                kick();
                prev_best = best_score;
                last_improve_iter = try_num;
            }
        }
    }

    string get_final_ops() {
        children = best_children;
        is_reverse = best_is_reverse;
        eff_rev = best_eff_rev;
        child_index = best_child_index;
        string res = "";
        auto rec = [&](auto self, int u) -> void {
            if (u != M)
                res += "Z";
            if (children[u].empty()) {
                if (u != M)
                    res += get_path(get_eff_from(u), get_eff_to(u));
                if (u != M)
                    res += "Z";
                return;
            }
            pii curr_p = (u == M ? ORIGIN : get_eff_from(u));
            bool rev = eff_rev[u];
            if (!rev) {
                for (int child : children[u]) {
                    res += get_path(curr_p, get_eff_from(child));
                    self(self, child);
                    curr_p = get_eff_to(child);
                }
                if (u != M)
                    res += get_path(curr_p, nodes[u].to);
            } else {
                for (int i = (int)children[u].size() - 1; i >= 0; i--) {
                    int child = children[u][i];
                    res += get_path(curr_p, get_eff_from(child));
                    self(self, child);
                    curr_p = get_eff_to(child);
                }
                res += get_path(curr_p, nodes[u].from);
            }
            if (u != M)
                res += "Z";
        };
        rec(rec, M);
        return res;
    }
};

void solve() {
    init_dist_tbl();
    input();
    AnnealingState state(TL);
    state.solve();
    string ops = state.get_final_ops();
    for (char c : ops)
        cout << c << "\n";
    DEBUG(state.best_score);
}

signed main() {
    solve();
    return 0;
}
