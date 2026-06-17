// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>

#ifdef ATCODER
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
using namespace atcoder;
#endif

#ifdef ATCODER
template <typename K, typename V>
using hash_map = ankerl::unordered_dense::map<K, V>;
template <typename K>
using hash_set = ankerl::unordered_dense::set<K>;
#else
template <typename K, typename V>
using hash_map = std::unordered_map<K, V>;
template <typename K>
using hash_set = std::unordered_set<K>;
#endif

namespace cp_internal {
char char_buff[64];
};

/* namespace */
using namespace std;

/* type */
using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;   // 128bit符号付き整数
using uvll = __uint128_t; // 128bit符号なし整数
template <typename T>
concept RAR = ranges::random_access_range<T>;
template <ranges::range Range>
using rvt = ranges::range_value_t<Range>;

/* ios*/
struct fast_ios {
    fast_ios() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);
        cout << fixed << setprecision(15);
    }
} fast_ios;

#ifdef ATCODER
template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val;
    is >> val;
    x = val;
    return is;
}
template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) {
    os << x.val();
    return os;
}
#endif
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) {
    os << pr.first << ' ' << pr.second;
    return os;
}
template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) {
    is >> pr.first >> pr.second;
    return is;
}

template <class Tuple, size_t... I>
void print_tuple_impl(ostream &os, const Tuple &t, index_sequence<I...>) {
    ((os << (I == 0 ? "" : " ") << get<I>(t)), ...);
}
template <class... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &t) {
    print_tuple_impl(os, t, index_sequence_for<Ts...>{});
    return os;
}
template <class Tuple, size_t... I>
void read_tuple_impl(istream &is, Tuple &t, index_sequence<I...>) {
    ((is >> get<I>(t)), ...);
}
template <class... Ts>
istream &operator>>(istream &is, tuple<Ts...> &t) {
    read_tuple_impl(is, t, index_sequence_for<Ts...>{});
    return is;
}
template <typename T>
typename enable_if<is_same<T, __int128>::value || is_same<T, __uint128_t>::value, ostream &>::type operator<<(ostream &os, T x) {
    if (x == 0) return os << '0';
    string s;
    if constexpr (is_signed<T>::value) {  // 符号付き
        bool neg = x < 0;
        if (neg) x = -x;
        while (x > 0) {
            s += '0' + x % 10;
            x /= 10;
        }
        if (neg) s += '-';
    } else {  // 符号なし
        while (x > 0) {
            s += '0' + x % 10;
            x /= 10;
        }
    }
    ranges::reverse(s);
    os << s;
    return os;
}
template <typename T>
typename enable_if<is_same<T, __int128>::value || is_same<T, __uint128_t>::value, istream &>::type operator>>(istream &is, T &x) {
    string s;
    is >> s;
    x = 0;
    size_t i = 0;
    if constexpr (is_signed<T>::value) {
        bool neg = false;
        if (s[0] == '-') {
            neg = true;
            i = 1;
        }
        for (; i < s.size(); i++) x = x * 10 + (s[i] - '0');
        if (neg) x = -x;
    } else {
        for (; i < s.size(); i++) x = x * 10 + (s[i] - '0');
    }
    return is;
}
template <class T, template <class...> class Container>
istream &operator>>(istream &is, Container<T> &v)
    requires requires(Container<T> c) {
        c.begin();
        c.end();
        c.push_back(T{});
    } && (!is_same_v<Container<T>, string>)
{
    for (auto &x : v) is >> x;
    return is;
}

template <class... T>
void input(T &...a) {
    (cin >> ... >> a);
}

template <class T>
concept Iterable = requires(T a) {
    begin(a);
    end(a);
} && !is_same_v<T, string>;
template <Iterable T>
void print(const T &v) {
    bool first = true;
    for (auto const &x : v) {
        if (!first) cout << " ";
        first = false;
        cout << x;
    }
    cout << '\n';
}

template <class T, class... Ts>
void print(const T &a, const Ts &...b) {
    ((cout << a), ..., (cout << ' ' << b));
    cout << '\n';
}

template <class T>
concept RandomAccessContainer = requires(T a) {
    typename T::value_type;
    { a.begin() } -> random_access_iterator;
    { a.end() } -> random_access_iterator;
};
template <RandomAccessContainer C>
void print_range(const C &v, int l, int r, string sep = " ") {
    int n = v.size();
    l = max(0, l);
    r = min(n, r);
    if (l >= r) {
        cout << '\n';
        return;
    }
    cout << v[l];
    for (int i = l + 1; i < r; i++) {
        cout << sep << v[i];
    }
    cout << '\n';
}

/* debug */
// ローカルのみ変数名付きで標準エラーに出力、提出時は無効
#ifdef LOCAL
template <Iterable T>
void debug_impl(const T &v) {
    bool first = true;
    for (auto const &x : v) {
        if (!first) cerr << " ";
        first = false;
        cerr << x;
    }
    cerr << '\n';
}
template <class T, class... Ts>
void debug_impl(const T &a, const Ts &...b) {
    ((cerr << a), ..., (cerr << ' ' << b));
    cerr << '\n';
}
#define debug(...) do { cerr << "\033[33m[" << #__VA_ARGS__ << "]\033[0m "; debug_impl(__VA_ARGS__); } while(0)
#else
#define debug(...)
#endif

/* math*/
// x^n を整数で返す (オーバーフロー注意、mod不要なとき用)
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
// x/y の切り上げ (負の数も正しく動作)
template <class T>
T divceil(T x, T y) {
    auto d = x / y, r = x % y;
    return d + (r != 0 && ((r ^ y) > 0));
}
// x/y の切り捨て (負の数も正しく動作)
template <class T>
T divfloor(T x, T y) {
    auto d = x / y, r = x % y;
    return d - (r != 0 && ((r ^ y) < 0));
}
// x % m を常に非負で返す
template <class T1, class T2>
T1 smod(T1 x, T2 m) {
    if (x < 0) return (x % m + m) % m;
    return x % m;
}
// x^n mod m (m が int範囲、中間積がオーバーフローしない場合)
ll pow_mod32(ll x, ll n, int m) {
    if (m == 1) return 0;
    ll ret = 1;
    x = smod(x, m);
    while (n) {
        if (n & 1) ret = smod(x * ret, m);
        x = smod(x * x, m);
        n >>= 1;
    }
    return ret;
}
// x^n mod m (m が ll範囲、__int128で中間積を計算)
ll pow_mod64(vll x, vll n, ll m) {
    if (m == 1) return 0;
    vll ret = 1;
    x = smod(x, m);
    while (n) {
        if (n & 1) ret = smod(x * ret, m);
        x = smod(x * x, m);
        n >>= 1;
    }
    return ret;
}
// m以下の最大の整数x で x ≡ q (mod p) を返す
ll nearest_px_plus_q_le(ll m, ll p, ll q) {
    return m - smod(m - q, p);
}
// m以上の最小の整数x で x ≡ q (mod p) を返す
ll nearest_px_plus_q_ge(ll m, ll p, ll q) {
    return m + (p - smod(m - q, p)) % p;
}
// 2点を通る直線 ax+by+c=0 を既約整数の {a,b,c} で返す
tuple<ll, ll, ll> line_from_two_points(ll x1, ll y1, ll x2, ll y2) {
    ll a = y1 - y2;
    ll b = x2 - x1;
    ll c = x1 * y2 - y1 * x2;
    ll g = gcd(gcd(a, b), c);
    a /= g;
    b /= g;
    c /= g;
    if (a < 0 || (a == 0 && b < 0)) {
        a *= -1;
        b *= -1;
        c *= -1;
    }
    return {a, b, c};
}
// 3点が同一直線上にあるか判定
bool collinear(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3) {
    vll cross = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    return cross == 0;
}

/* algorithms */
// ソート済みrにおけるx未満の要素数
template <RAR Range>
ll lower_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::lower_bound(r, x));
}
// ソート済みrにおけるx以下の要素数
template <RAR Range>
ll upper_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::upper_bound(r, x));
}
// a = min(a, b)。更新したらtrue
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a > b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmin(A &a, const B &b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}
// a = max(a, b)。更新したらtrue
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a < b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmax(A &a, const B &b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
// 座標圧縮。{元の値 -> 0始まりの番号} のhash_mapを返す
template <RAR Range>
hash_map<rvt<Range>, ll> compression(Range r) {
    ranges::sort(r);
    auto res = ranges::unique(r);
    r.erase(ranges::begin(res), ranges::end(res));
    hash_map<rvt<Range>, ll> ret;
    for (ll i = 0; auto v : r) {
        ret[v] = i++;
    }
    return ret;
}
// 累積和。res[i] = r[0]+...+r[i] (長さはrと同じ)
// res[0]=init, res[i+1]=res[i]+r[i] (長さはr+1)
template <ranges::range Range>
vector<rvt<Range>> accumulate_sum(const Range &r) {
    vector<rvt<Range>> res(ssize(r));
    partial_sum(ranges::begin(r), ranges::end(r), ranges::begin(res));
    return res;
}
template <ranges::range Range>
vector<rvt<Range>> accumulate_sum(const Range &r, const rvt<Range> &init) {
    vector<rvt<Range>> res(ssize(r) + 1);
    res[0] = init;
    inclusive_scan(ranges::begin(r), ranges::end(r), ranges::begin(res) + 1, plus<>(), init);
    return res;
}
// rをw個ずつに分割したvector<vector<T>>を返す
template <ranges::range Range>
vector<vector<rvt<Range>>> lpartition(const Range &r, int w) {
    int n = ssize(r);
    int h = (n + w - 1) / w;
    vector<vector<rvt<Range>>> ret(h, vector<rvt<Range>>(w));
    for (int i = 0; i < n; ++i) {
        ret[i / w][i % w] = r[i];
    }
    return ret;
}

// RLE。{値, 連続個数} のvectorを返す。例: "aaabbc" -> {('a',3),('b',2),('c',1)}
template <RAR Range>
auto runlength(const Range &R) {
    vector<pair<rvt<Range>, ll>> ret;
    for (ll l = 0; l < ssize(R);) {
        ll r = l + 1;
        for (; r < ssize(R) && R[l] == R[r]; ++r) {
        }
        ret.emplace_back(R[l], r - l);
        l = r;
    }
    return ret;
}
// vをソートして重複を除去 (in-place)
template <RAR Range>
void remove_duplicates(Range &v) {
    ranges::sort(v);
    auto res = ranges::unique(v);
    v.erase(ranges::begin(res), ranges::end(res));
}
// Rの総和を返す
// 型Tで総和を返す (オーバーフロー対策などに)
template <ranges::range Range>
auto sum_of(const Range &R) {
    return reduce(ranges::begin(R), ranges::end(R));
}
template <ranges::range Range, class T>
T sum_of(const Range &R) {
    return reduce(ranges::begin(R), ranges::end(R), T());
}

/* data_structure */
// 最小値優先ヒープ (pq.top()が最小)
template <class T>
using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
// 最大値優先ヒープ (pq.top()が最大)
template <class T>
using priority_queue_max = priority_queue<T, vector<T>>;
// vector<T>(n, val)
// vector<vector<T>>(n, m, val)  (多次元も可)
template <class T>
auto make_vec(size_t n, const T &val) {
    return vector<T>(n, val);
}
template <class T, class... Args>
auto make_vec(size_t n, Args... args) {
    return vector<decltype(make_vec<T>(args...))>(n, make_vec<T>(args...));
}

/* string */
// 非負整数xをbase進数文字列に変換 (base=2〜36)
string dec_to_base(ull x, int base) {
    auto b = begin(cp_internal::char_buff);
    auto [ptr, ec] = to_chars(b, end(cp_internal::char_buff), x, base);
    return string(b, ptr);
}
// base進数文字列xを10進数に変換
ull base_to_dec(const string &x, int base) { return stoull(x, 0, base); }
// 文字列sをwidthの幅になるよう先頭を'0'で埋める
void zfill(string &s, size_t width) { s = string(width > s.size() ? width - s.size() : 0, '0') + s; }

/* grid */
// d=0:右, 1:下, 2:左, 3:上 の移動量
[[maybe_unused]] constexpr int dx[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dy[] = {1, 0, -1, 0};
// グリッド(h行w列)の範囲内かチェック
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

/* bit */
// xの立っているビット数
int popcnt(ull x) { return popcount(x); }
// xの最上位ビット位置 (0-indexed, x>0)
int msb(ull x) { return 63 - countl_zero(x); }
// xの最下位ビット位置 (0-indexed, x>0)
int lsb(ull x) { return countr_zero(x); }

/* random */
// 高速な疑似乱数生成器
// 32bit乱数
// 64bit乱数
// [l,r]の32bit一様乱数
// [l,r]の64bit一様乱数
struct Xorshift128 {
    uint x = 123456789;
    uint y = 362436069;
    uint z = 521288629;
    uint w;
    Xorshift128() {
        random_device rd;
        w = rd();
        if (w == 0) w = 88675123;
    }
    uint operator()() {
        uint t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
    ull rand64() {
        ull high = ull((*this)()) << 32;
        ull low = ull((*this)());
        return high | low;
    }
    uint rand_range(uint l, uint r) {
        uint range = r - l + 1;
        uint limit = ~uint(0) - (~uint(0) % range);
        while (true) {
            uint x = (*this)();
            if (x <= limit) {
                return l + (x % range);
            }
        }
    }
    ull rand_range64(ull l, ull r) {
        ull range = r - l + 1;
        ull limit = ~ull(0) - (~ull(0) % range);
        while (true) {
            ull x = rand64();
            if (x <= limit) {
                return l + (x % range);
            }
        }
    };
};

/* util */
#define OL_REP(_1, _2, _3, _4, name, ...) name
#define REP2(i, n) for (ll i = 0; i < ll(n); ++(i))
#define REP3(i, a, b) for (ll i = ll(a); i < ll(b); ++(i))
#define REP4(i, a, b, c) for (ll i = ll(a); i < ll(b); i += c)
#define rep(...) OL_REP(__VA_ARGS__, REP4, REP3, REP2)(__VA_ARGS__)
#define RREP2(i, n) for (ll i = ll(n); i >= 0; --i)
#define RREP3(i, a, b) for (ll i = ll(a); i > ll(b); --(i))
#define RREP4(i, a, b, c) for (ll i = ll(a); i > ll(b); i -= c)
#define rrep(...) OL_REP(__VA_ARGS__, RREP4, RREP3, RREP2)(__VA_ARGS__)

/* constants */
//[[maybe_unused]] constexpr ll MOD = 1e9 + 7;
[[maybe_unused]] constexpr ll MOD = 998244353;
[[maybe_unused]] constexpr int INF32 = (1 << 30) - 1;
[[maybe_unused]] constexpr ll INF64 = (1LL << 62) - 1;
[[maybe_unused]] const double PI = numbers::pi;

// clang-format on
template <typename T = int>
struct Edge {
    int from, to;
    T cost;
    int idx;

    Edge() = default;

    Edge(int from, int to, T cost = 1, int idx = -1) : from(from), to(to), cost(cost), idx(idx) {}

    operator int() const { return to; }
};

template <typename T = int>
struct Graph {
    vector<vector<Edge<T>>> g;
    int es;

    Graph() = default;

    explicit Graph(int n) : g(n), es(0) {}

    size_t size() const { return g.size(); }

    void add_directed_edge(int from, int to, T cost = 1) { g[from].emplace_back(from, to, cost, es++); }

    void add_edge(int from, int to, T cost = 1) {
        g[from].emplace_back(from, to, cost, es);
        g[to].emplace_back(to, from, cost, es++);
    }

    void read(int M, int padding = -1, bool weighted = false, bool directed = false) {
        for (int i = 0; i < M; i++) {
            int a, b;
            cin >> a >> b;
            a += padding;
            b += padding;
            T c = T(1);
            if (weighted) cin >> c;
            if (directed)
                add_directed_edge(a, b, c);
            else
                add_edge(a, b, c);
        }
    }

    inline vector<Edge<T>> &operator[](const int &k) { return g[k]; }

    inline const vector<Edge<T>> &operator[](const int &k) const { return g[k]; }
};

template <typename T = int>
using Edges = vector<Edge<T>>;

#line 1 "graph/tree/static-top-tree.hpp"
template <typename G>
struct StaticTopTree {
    enum OpType { Vertex, AddVertex, AddEdge, Rake, Compress };

    struct Node {
        OpType op;

        int l, r, p;

        int e_id;

        Node(OpType op, int l, int r) : op{op}, l{l}, r{r}, p{-1}, e_id{-1} {}
    };

    vector<Node> vs;

    vector<int> edge_to_vs;

    int root;

    explicit StaticTopTree(G &g, int r = 0) : g(g), edge_to_vs(g.size() - 1) {
        int e_sz = 0;
        for (int i = 0; i < g.size(); i++) e_sz += g[i].size();
        if (e_sz + 1 != g.size()) {
            throw std::runtime_error("`g` must be a directed tree.");
        }
        dfs(r);

        vs.assign(g.size(), {Vertex, -1, -1});
        vs.reserve(g.size() * 4);
        root = compress(r).first;
        vs.shrink_to_fit();
    }

    const Node &operator[](int k) const { return vs[k]; }

    size_t size() const { return vs.size(); }

   private:
    G &g;

    using P = pair<int, int>;

    int dfs(int u) {
        int size = 1, heavy = 0;
        for (auto &v : g[u]) {
            int subtree_size = dfs(v);
            size += subtree_size;
            if (heavy < subtree_size) {
                heavy = subtree_size;
                swap(v, g[u][0]);
            }
        }
        return size;
    }

    int make_node(OpType t, int l, int r, int k = -1) {
        if (k == -1) {
            k = (int)vs.size();
            vs.emplace_back(t, l, r);
        } else {
            vs[k] = {t, l, r};
        }
        if (l != -1) vs[l].p = k;
        if (r != -1) vs[r].p = k;
        return k;
    }

    P merge_forRake(const vector<P> &a) {
        if (a.size() == 1) return a[0];
        int size_sum = 0;
        for (auto &[_, size] : a) {
            size_sum += size;
        }
        vector<P> b, c;
        for (auto &[it, size] : a) {
            (size_sum > size ? b : c).emplace_back(it, size);
            size_sum -= size * 2;
        }
        auto [l, l_size] = merge_forRake(b);
        auto [r, r_size] = merge_forRake(c);
        return {make_node(Rake, l, r), l_size + r_size};
    }

    P merge_forCompress(const vector<pair<P, int>> &a) {
        if (a.size() == 1) return a[0].first;
        int size_sum = 0;
        for (auto &[it, _] : a) {
            size_sum += it.second;
        }
        vector<pair<P, int>> b, c;
        for (auto &[it, _] : a) {
            (size_sum > it.second ? b : c).emplace_back(it, _);
            size_sum -= it.second * 2;
        }
        auto [l, l_size] = merge_forCompress(b);
        auto [r, r_size] = merge_forCompress(c);
        int idx = make_node(Compress, l, r);
        edge_to_vs[c[0].second] = idx;
        vs[idx].e_id = c[0].second;
        return {idx, l_size + r_size};
    }

    P add_edge(int u, int e_idx) {
        auto [it, size] = compress(u);
        int idx = make_node(AddEdge, it, -1);
        edge_to_vs[e_idx] = idx;
        vs[idx].e_id = e_idx;
        return {idx, size};
    }

    P rake(int u) {
        vector<P> chs;
        for (int j = 1; j < (int)g[u].size(); j++) {
            chs.emplace_back(add_edge(g[u][j].to, g[u][j].idx));
        }
        return merge_forRake(chs);
    }

    P add_vertex(int u) {
        if (g[u].size() < 2) {
            return {make_node(OpType::Vertex, -1, -1, u), 1};
        } else {
            auto [it, size] = rake(u);
            return {make_node(OpType::AddVertex, it, -1, u), size + 1};
        }
    }

    P compress(int u) {
        vector<pair<P, int>> chs{{add_vertex(u), -1}};
        vector<int> ids{-1};
        while (not g[u].empty()) {
            int e_idx = g[u][0].idx;
            u = g[u][0];
            chs.emplace_back(add_vertex(u), e_idx);
        }
        return merge_forCompress(chs);
    }
};
#line 2 "graph/tree/static-top-tree-dp.hpp"

template <typename TreeDPInfo, typename G>
struct StaticTopTreeDP {
    using Path = typename TreeDPInfo::Path;
    using Point = typename TreeDPInfo::Point;

    using STT = StaticTopTree<G>;

    const STT &g;

    const TreeDPInfo &info;

    vector<int> par;

    explicit StaticTopTreeDP(const STT &g, const TreeDPInfo &info, vector<int> &P) : g(g), info(info) {
        dp.resize(g.size());
        dfs(g.root);
        par = P;
    }

    ll update_vertex(int u) {
        while (u != -1) {
            modify(u);
            u = g[u].p;
        }
        Path res = get<Path>(dp[u]);
        ll ans = res.val1;
        if (par[u] != -1){
            auto pp = get<Path>(dp[par[u]]);
            if (res.c1 == pp.c2){
                ans += pp.val2;
            }
        }
        return ans;
    }

    Path update_edge(int e) { return update_vertex(g.edge_to_vs[e]); }

   private:
    vector<variant<Point, Path>> dp;

    void modify(int k) {
        switch (g[k].op) {
            case STT::Vertex:
                dp[k] = info.vertex(k);
                return;
            case STT::Compress:
                dp[k] = info.compress(get<Path>(dp[g[k].l]), get<Path>(dp[g[k].r]), g[k].e_id);
                return;
            case STT::Rake:
                dp[k] = info.rake(get<Point>(dp[g[k].l]), get<Point>(dp[g[k].r]));
                return;
            case STT::AddEdge:
                dp[k] = info.add_edge(get<Path>(dp[g[k].l]), g[k].e_id);
                return;
            case STT::AddVertex:
                dp[k] = info.add_vertex(get<Point>(dp[g[k].l]), k);
                return;
        }
    }

    void dfs(int u) {
        if (u == -1) return;
        dfs(g[u].l);
        dfs(g[u].r);
        modify(u);
    }
};

struct TreeDPInfo {
    struct Point {
        ll valb,valw, ans;
    };
    struct Path {
        ll val1,val2,c1,c2,ans;
        bool connect;
    };

    vector<ll> W,C;

    TreeDPInfo(int n) {}

    Path vertex(int u) const {
        if (C[u] == 1) {
            return {W[u], W[u], 1, 1, true};
        } else {
            return {W[u], W[u], 0, 0,true};
        }
    };
    Path add_vertex(Point d, int u) const {
        if (C[u] == 1) {
            return {W[u] + d.valb, W[u] + d.valb, 1, 1,max(d.ans,W[u]+d.valb),true};
        } else {
            return {W[u] + d.valw, W[u] + d.valw, 0, 0,max(d.ans,W[u]+d.valw),true};
        }
    }
    Point add_edge(Path d, int e) const { 
        if (d.c1 == 1){
            return {d.val1,0, d.ans};
        } else{
            return {0, d.val1, d.ans};
        }
    }
    Point rake(Point l, Point r) const { return {l.valb+r.valb,l.valw+r.valw,max(l.ans,r.ans)}; }
    Path compress(Path p, Path c, int e) const {
        Path res;
        res.c1 = p.c1;
        res.c2 = c.c2;
        if (p.c2 == c.c1){
            res.connect= p.connect && c.connect;
            if (p.connect){
                res.val1 = p.val1+c.val1;
            } else {
                res.val1 = p.val1;
            }
            if (c.connect){
                res.val2 = c.val2 + p.val2;
            } else {
                res.val2 = c.val2;
            }
        } else {
            res.connect = false;
            res.val1 = p.val1;
            res.val2 = c.val2;
        }
        res.ans = max({res.val1,res.val2,p.ans,c.ans});
        return res;
    }
};
template <typename T>
Graph<T> convert_rooted_tree(const Graph<T> &g, int r = 0) {
    int N = (int)g.size();
    Graph<T> rg(N);
    vector<int> v(N);
    v[r] = 1;
    queue<int> que;
    que.emplace(r);
    while (!que.empty()) {
        auto p = que.front();
        que.pop();
        for (auto &to : g[p]) {
            if (v[to] == 0) {
                v[to] = 1;
                que.emplace(to);
                rg.g[p].emplace_back(to);
            }
        }
    }
    return rg;
}
int main() {
    int N,Q;
    input(N,Q);
    TreeDPInfo info(N);
    info.W.resize(N);
    info.C.resize(N);
    input(info.W);
    input(info.C);
    Graph G(N);

    rep(i, N - 1) {
        int u, v;
        input(u, v);
        u--, v--;
        G.add_edge(u, v);
    }

    auto G2 = convert_rooted_tree(G);

    vector<int> par(N,-1);
    auto dfs = [&](this auto self, int u) -> void {
        for(auto v:G2.g[u]){
            par[v] = u;
            self(v);
        }
    };
    dfs(0);
    StaticTopTree tree(G2);
    StaticTopTreeDP stt(tree, info, par);

    ll q,v,x;
    rep(_, Q) {
        input(q);
        if(q==1){
            input(v);
            v--;
            info.C[v] ^= 1;
            stt.update_vertex(v);
        } else if (q==2){
            input(v,x);
            v--;
            info.W[v] += x;
            stt.update_vertex(v);
        } else {
            input(v);
            v--;
            print(stt.update_vertex(v));
        }
    }
}