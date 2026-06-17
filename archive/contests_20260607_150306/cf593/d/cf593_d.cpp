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
/**
 * @brief Heavy-Light Decomposition（HL分解）
 *
 * 木上のパスクエリ・部分木クエリをセグ木と組み合わせて O(log^2 N) で処理する。
 * スタック溢れを避けるため DFS は反復で実装している。
 *
 * 【使い方】
 *   HLD hld(n);
 *   hld.add_edge(u, v);
 *   hld.build(root);       // デフォルト root=0
 *
 *   // セグ木は hld.pos[v] の位置に頂点 v の値を置いて初期化する
 *
 *   // 頂点クエリ（可換演算）
 *   ll ans = 0;
 *   for (auto [l, r, rev] : hld.path(u, v)) ans += seg.prod(l, r);
 *
 *   // 辺クエリ: 辺の重みを子側の頂点に持たせ、vertex=false で LCA を除く
 *   //   build 後に辺 (u,v) の重み w を seg.set(hld.pos[child], w) で設定する
 *   //   child は depth が深い方: (hld.depth[u] > hld.depth[v]) ? u : v
 *   for (auto [l, r, rev] : hld.path(u, v, false)) ans += seg.prod(l, r);
 *
 *   // 非可換演算（行列積など）
 *   // rev=true のセグメントは右→左方向（逆順積が必要）
 *   Matrix ans = I;
 *   for (auto [l, r, rev] : hld.path(u, v)) {
 *       ans = ans * (rev ? seg.prod_rev(l, r) : seg.prod(l, r));
 *   }
 *
 *   // 部分木クエリ
 *   auto [l, r] = hld.subtree(v);
 *   ans = seg.prod(l, r);
 *
 * 【メンバ】
 *   pos[v]   : v の HLD 配列上の位置
 *   sz[v]    : v の部分木サイズ
 *   depth[v] : 根からの深さ
 *   parent[v]: 親頂点（根は -1）
 *   head[v]  : v が属するチェーンの先頭頂点
 */
struct HLD {
    int n;
    vector<vector<int>> G;
    vector<int> parent, depth, sz, heavy, head, pos;
    int cur_pos = 0;

    HLD(int n) : n(n), G(n), parent(n, -1), depth(n, 0), sz(n, 1), heavy(n, -1), head(n, 0), pos(n, 0) {}

    void add_edge(int u, int v) {
        G[u].push_back(v);
        G[v].push_back(u);
    }

    void build(int root = 0) {
        // 部分木サイズと heavy child を求める（BFS 順で逆順に処理）
        vector<int> order;
        order.reserve(n);
        stack<pair<int, int>> stk;
        stk.push({root, -1});
        while (!stk.empty()) {
            auto [v, p] = stk.top();
            stk.pop();
            parent[v] = p;
            order.push_back(v);
            for (int u : G[v]) {
                if (u == p) continue;
                depth[u] = depth[v] + 1;
                stk.push({u, v});
            }
        }
        for (int i = (int)order.size() - 1; i >= 0; i--) {
            int v = order[i];
            for (int u : G[v]) {
                if (u == parent[v]) continue;
                sz[v] += sz[u];
                if (heavy[v] == -1 || sz[u] > sz[heavy[v]]) heavy[v] = u;
            }
        }
        // HLD ラベリング（heavy child を優先して DFS）
        stk.push({root, root});
        while (!stk.empty()) {
            auto [v, h] = stk.top();
            stk.pop();
            head[v] = h;
            pos[v] = cur_pos++;
            // light children を先に積む（後回し）
            for (int u : G[v]) {
                if (u == parent[v] || u == heavy[v]) continue;
                stk.push({u, u});
            }
            // heavy child を後に積む（先に処理 → 連続した pos が割り当たる）
            if (heavy[v] != -1) stk.push({heavy[v], h});
        }
    }

    // パス u-v 上の区間リストを返す（u→v の順）
    // 各要素は {l, r, rev}: セグ木上の区間 [l, r)
    //   rev=false: 左→右方向（LCA→v 側）
    //   rev=true : 右→左方向（u→LCA 側、非可換演算で逆順クエリが必要）
    // vertex=false のとき辺クエリ（LCA 頂点を除く）
    // 可換演算なら rev を無視して prod(l, r) するだけでよい
    vector<tuple<int, int, bool>> path(int u, int v, bool vertex = true) {
        vector<tuple<int, int, bool>> segs_u, segs_v;
        for (; head[u] != head[v];) {
            if (depth[head[u]] >= depth[head[v]]) {
                segs_u.emplace_back(pos[head[u]], pos[u] + 1, true);
                u = parent[head[u]];
            } else {
                segs_v.emplace_back(pos[head[v]], pos[v] + 1, false);
                v = parent[head[v]];
            }
        }
        if (depth[u] > depth[v]) swap(u, v);
        segs_u.emplace_back(pos[u] + (vertex ? 0 : 1), pos[v] + 1, false);
        // u 側は u→LCA 順に並んでいる。v 側は逆順に積んでいるので reverse して連結
        reverse(segs_v.begin(), segs_v.end());
        for (auto &s : segs_v) segs_u.push_back(s);
        return segs_u;
    }

    // 部分木 v の区間 [l, r) を返す
    pair<int, int> subtree(int v) { return {pos[v], pos[v] + sz[v]}; }

    // u, v の LCA を返す
    int lca(int u, int v) {
        for (; head[u] != head[v];) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }

    // u, v 間の距離（辺の数）を返す
    int dist(int u, int v) { return depth[u] + depth[v] - 2 * depth[lca(u, v)]; }
};
// ACL: segtree
// https://github.com/atcoder/ac-library
// clang-format off
#ifndef ATCODER
#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#if __cplusplus >= 202002L
#include <bit>
#endif
namespace atcoder{
namespace internal{
#if __cplusplus >= 202002L
using std::bit_ceil;
#else
unsigned int bit_ceil(unsigned int n){
unsigned int x = 1;
while(x <(unsigned int)(n))x *= 2;
return x;
}
#endif
int countr_zero(unsigned int n){
#ifdef _MSC_VER
unsigned long index;
_BitScanForward(&index,n);
return index;
#else
return __builtin_ctz(n);
#endif
}
constexpr int countr_zero_constexpr(unsigned int n){
int x = 0;
while(!(n &(1 << x)))x++;
return x;
}
}// namespace internal
}// namespace atcoder
namespace atcoder{
#if __cplusplus >= 201703L
template <class S,auto op,auto e> struct segtree{
static_assert(std::is_convertible_v<decltype(op),std::function<S(S,S)>>,
"op must work as S(S,S)");
static_assert(std::is_convertible_v<decltype(e),std::function<S()>>,
"e must work as S()");
#else
template <class S,S(*op)(S,S),S(*e)()> struct segtree{
#endif
public:
segtree(): segtree(0){}
explicit segtree(int n): segtree(std::vector<S>(n,e())){}
explicit segtree(const std::vector<S>& v): _n(int(v.size())){
size =(int)internal::bit_ceil((unsigned int)(_n));
log = internal::countr_zero((unsigned int)size);
d = std::vector<S>(2 * size,e());
for(int i = 0;i < _n;i++)d[size + i]= v[i];
for(int i = size - 1;i >= 1;i--){
update(i);
}
}
void set(int p,S x){
assert(0 <= p && p < _n);
p += size;
d[p]= x;
for(int i = 1;i <= log;i++)update(p >> i);
}
S get(int p)const{
assert(0 <= p && p < _n);
return d[p + size];
}
S prod(int l,int r)const{
assert(0 <= l && l <= r && r <= _n);
S sml = e(),smr = e();
l += size;
r += size;
while(l < r){
if(l & 1)sml = op(sml,d[l++]);
if(r & 1)smr = op(d[--r],smr);
l >>= 1;
r >>= 1;
}
return op(sml,smr);
}
S all_prod()const{return d[1];}
template <bool(*f)(S)> int max_right(int l)const{
return max_right(l,[](S x){return f(x);});
}
template <class F> int max_right(int l,F f)const{
assert(0 <= l && l <= _n);
assert(f(e()));
if(l == _n)return _n;
l += size;
S sm = e();
do{
while(l % 2 == 0)l >>= 1;
if(!f(op(sm,d[l]))){
while(l < size){
l =(2 * l);
if(f(op(sm,d[l]))){
sm = op(sm,d[l]);
l++;
}
}
return l - size;
}
sm = op(sm,d[l]);
l++;
}while((l & -l)!= l);
return _n;
}
template <bool(*f)(S)> int min_left(int r)const{
return min_left(r,[](S x){return f(x);});
}
template <class F> int min_left(int r,F f)const{
assert(0 <= r && r <= _n);
assert(f(e()));
if(r == 0)return 0;
r += size;
S sm = e();
do{
r--;
while(r > 1 &&(r % 2))r >>= 1;
if(!f(op(d[r],sm))){
while(r < size){
r =(2 * r + 1);
if(f(op(d[r],sm))){
sm = op(d[r],sm);
r--;
}
}
return r + 1 - size;
}
sm = op(d[r],sm);
}while((r & -r)!= r);
return 0;
}
private:
int _n,size,log;
std::vector<S> d;
void update(int k){d[k]= op(d[2 * k],d[2 * k + 1]);}
};
}// namespace atcoder
// clang-format on
using namespace atcoder;
#endif  // ATCODER

ll op(ll x, ll y) {
    if (x == -1 || y == -1) {
        return -1;
    }
    if (x >= divceil((ll)1e18 + 5,y)) {
        return -1;
    }
    return x * y;
}

ll e() { return 1; }

int main() {
    int N, M;
    input(N, M);

    HLD hld(N);
    vector<tuple<ll, ll, ll>> E;
    rep(i, N - 1) {
        ll u, v, x;
        input(u, v, x);
        u--, v--;
        hld.add_edge(u, v);
        E.emplace_back(u, v, x);
    }
    hld.build();
    segtree<ll, op, e> seg(N);
    for (auto [u, v, x] : E) {
        if (hld.depth[u] > hld.depth[v]) {
            seg.set(hld.pos[u], x);
        } else {
            seg.set(hld.pos[v], x);
        }
    }

    ll q, a, b, y, p, c;
    rep(i, M) {
        input(q);
        if (q == 1) {
            input(a, b, y);
            a--, b--;
            ll d = 1;
            for (auto [u, v, rev] : hld.path(a, b, false)) {
                ll res = seg.prod(u, v);
                if (d == -1 || res == -1) {
                    d = -1;
                } else if (d >= divceil((ll)1e18 + 5, res)) {
                    d = -1;
                } else {
                    d *= res;
                }
            }
            if (d == -1) {
                print(0);
            } else {
                print(y / d);
            }
        } else {
            input(p, c);
            p--;
            auto [u, v, x] = E[p];
            if (hld.depth[u] > hld.depth[v]) {
                seg.set(hld.pos[u], c);
            } else {
                seg.set(hld.pos[v], c);
            }
        }
    }
}