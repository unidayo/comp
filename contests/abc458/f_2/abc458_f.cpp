// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <charconv>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numbers>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using namespace std;

#ifdef ATCODER
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
using namespace atcoder;
#endif

#ifdef ATCODER
template <typename K, typename V> using hash_map = ankerl::unordered_dense::map<K, V>;
template <typename K> using hash_set = ankerl::unordered_dense::set<K>;
#else
template <typename K, typename V> using hash_map = unordered_map<K, V>;
template <typename K> using hash_set = unordered_set<K>;
#endif

namespace cp_internal {
char char_buff[64];
template <ranges::range Range, class T>
auto sorted_lower_bound(const Range &r, const T &x) {
    if constexpr (requires { r.lower_bound(x); }) return r.lower_bound(x);
    else return ranges::lower_bound(r, x);
}
template <ranges::range Range, class T>
auto sorted_upper_bound(const Range &r, const T &x) {
    if constexpr (requires { r.upper_bound(x); }) return r.upper_bound(x);
    else return ranges::upper_bound(r, x);
}
};

/* type */
using uint = unsigned int;
using ll = long long;
using ull = unsigned long long;
using vll = __int128_t;
using uvll = __uint128_t;
template <typename T> concept RAR = ranges::random_access_range<T>;
template <ranges::range Range> using rvt = ranges::range_value_t<Range>;
/* ios*/
struct fast_ios {
    fast_ios() { ios::sync_with_stdio(false); cin.tie(nullptr); cout << fixed << setprecision(15); }
} fast_ios;

#ifdef ATCODER
template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val; is >> val; x = val;
    return is;
}
template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) { return os << x.val(); }
#endif
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) { return os << pr.first << ' ' << pr.second; }
template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) { return is >> pr.first >> pr.second; }
template <class Tuple, size_t... I>
void print_tuple_impl(ostream &os, const Tuple &t, index_sequence<I...>) {
    ((os << (I == 0 ? "" : " ") << get<I>(t)), ...);
}
template <class... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &t) {
    print_tuple_impl(os, t, index_sequence_for<Ts...>{}); return os;
}
template <class Tuple, size_t... I>
void read_tuple_impl(istream &is, Tuple &t, index_sequence<I...>) {
    ((is >> get<I>(t)), ...);
}
template <class... Ts>
istream &operator>>(istream &is, tuple<Ts...> &t) {
    read_tuple_impl(is, t, index_sequence_for<Ts...>{}); return is;
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
    requires requires(Container<T> c) { c.begin(); c.end(); c.push_back(T{}); } && (!is_same_v<Container<T>, string>)
{
    for (auto &x : v) is >> x;
    return is;
}
template <class... T>
void input(T &...a) { (cin >> ... >> a); }
template <class T>
concept Iterable = requires(T a) { begin(a); end(a); } && !is_same_v<T, string>;
template <class T>
concept NestedIterable = Iterable<T> && Iterable<rvt<T>>;
template <class T>
void print_value(const T &v) {
    if constexpr (Iterable<T>) {
        bool first = true;
        for (auto const &x : v) {
            if (!first) cout << ' ';
            first = false;
            print_value(x);
        }
    } else {
        cout << v;
    }
}
template <NestedIterable T>
void print(const T &v) {
    bool first = true;
    for (auto const &row : v) {
        if (!first) cout << '\n';
        first = false;
        print_value(row);
    }
    cout << '\n';
}
template <class... Ts>
void print(const Ts &...a) {
    bool first = true;
    auto print_one = [&](const auto &x) {
        if (!first) cout << ' ';
        first = false;
        print_value(x);
    };
    (print_one(a), ...);
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
/* math*/
// x^n を整数で返す
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
// x/y の切り上げ
template <class T>
T divceil(T x, T y) {
    auto d = x / y, r = x % y;
    return d + (r != 0 && ((r ^ y) > 0));
}
// x/y の切り捨て
template <class T>
T divfloor(T x, T y) {
    auto d = x / y, r = x % y;
    return d - (r != 0 && ((r ^ y) < 0));
}
// x % m を常に非負で返す
template <class T1, class T2>
[[nodiscard]]
T1 smod(T1 x, T2 m) {
    return x < 0 ? (x % m + m) % m : x % m;
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
ll nearest_px_plus_q_le(ll m, ll p, ll q) { return m - smod(m - q, p); }
// m以上の最小の整数x で x ≡ q (mod p) を返す
ll nearest_px_plus_q_ge(ll m, ll p, ll q) { return m + (p - smod(m - q, p)) % p; }
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
    return (x2 - x1) * vll(y3 - y1) - (y2 - y1) * vll(x3 - x1) == 0;
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
// ソート済みrにおけるx以下の最大値。存在しなければnullopt
template <ranges::bidirectional_range Range, class T>
optional<rvt<Range>> max_le(const Range &r, const T &x) {
    auto it = cp_internal::sorted_upper_bound(r, x);
    if (it == ranges::begin(r)) return nullopt;
    --it;
    return *it;
}
// ソート済みrにおけるx以上の最小値。存在しなければnullopt
template <ranges::range Range, class T>
optional<rvt<Range>> min_ge(const Range &r, const T &x) {
    auto it = cp_internal::sorted_lower_bound(r, x);
    if (it == ranges::end(r)) return nullopt;
    return *it;
}
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a > b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmin(A &a, const B &b) {
    if (a > b) return a = b, true;
    return false;
}
template <class A, class B>
    requires requires(A &a, const B &b) {
        { a < b } -> std::convertible_to<bool>;
        { a = b };
    }
bool chmax(A &a, const B &b) {
    if (a < b) return a = b, true;
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
// RLE。{値, 連続個数} のvectorを返す。例: "aaabbc" -> {('a',3),('b',2),('c',1)}
template <RAR Range>
auto runlength(const Range &R) {
    vector<pair<rvt<Range>, ll>> ret;
    for (ll l = 0; l < ssize(R);) {
        ll r = l + 1;
        for (; r < ssize(R) && R[l] == R[r]; ++r);
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
auto sum_of(const Range &R) { return reduce(ranges::begin(R), ranges::end(R)); }
template <ranges::range Range, class T>
T sum_of(const Range &R) { return reduce(ranges::begin(R), ranges::end(R), T()); }
/* data_structure */
template <class T> using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T> using priority_queue_max = priority_queue<T, vector<T>>;
template <class T>
auto make_vec(size_t n, const T &val) { return vector<T>(n, val); }
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
[[maybe_unused]] constexpr int dr4[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dc4[] = {1, 0, -1, 0};
// d=0から時計回り: 右, 右下, 下, 左下, 左, 左上, 上, 右上
[[maybe_unused]] constexpr int dr8[] = {0, 1, 1, 1, 0, -1, -1, -1};
[[maybe_unused]] constexpr int dc8[] = {1, 1, 0, -1, -1, -1, 0, 1};
// グリッド(h行w列)の範囲内かチェック
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

// 2次元コンテナを時計回りに90度回転したものを返す。
// vector<string>, vector<vector<T>> など、Grid[row][col] でアクセスできる矩形を想定。
template <class Grid>
Grid rotate90_clockwise(const Grid &a) {
    if (a.empty() || a[0].empty()) return Grid{};
    using Row = ranges::range_value_t<Grid>;
    using Cell = ranges::range_value_t<Row>;
    const int h = int(a.size());
    const int w = int(a[0].size());
    Grid b(w, Row(h, Cell{}));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            b[j][h - 1 - i] = a[i][j];
        }
    }
    return b;
}

// 2次元コンテナを反時計回りに90度回転したものを返す。
template <class Grid>
Grid rotate90_counterclockwise(const Grid &a) {
    if (a.empty() || a[0].empty()) return Grid{};
    using Row = ranges::range_value_t<Grid>;
    using Cell = ranges::range_value_t<Row>;
    const int h = int(a.size());
    const int w = int(a[0].size());
    Grid b(w, Row(h, Cell{}));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            b[w - 1 - j][i] = a[i][j];
        }
    }
    return b;
}

template <class Grid>
Grid rotate90(const Grid &a) {
    return rotate90_clockwise(a);
}
/* bit */
// xの立っているビット数
int popcnt(ull x) { return popcount(x); }
// xの最上位ビット位置 (0-indexed, x>0)
int msb(ull x) { return 63 - countl_zero(x); }
// xの最下位ビット位置 (0-indexed, x>0)
int lsb(ull x) { return countr_zero(x); }

/* random */
struct Xoshiro256 {
    ull s[4]{};
    explicit Xoshiro256(ull seed = random_device{}()) { reseed(seed); }
    static ull splitmix64(ull x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }
    static ull rotl(ull x, int k) { return (x << k) | (x >> (64 - k)); }
    void reseed(ull seed) {
        for (int i = 0; i < 4; ++i) {
            seed = splitmix64(seed);
            s[i] = seed;
        }
        if ((s[0] | s[1] | s[2] | s[3]) == 0) s[0] = 1;
    }
    ull rand64() {
        const ull result = rotl(s[1] * 5, 7) * 9;
        const ull t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);
        return result;
    }
    uint rand32() { return uint(rand64() >> 32); }
    // [0, range)
    ull rand_range_exclusive(ull range) {
        assert(range > 0);
        ull threshold = -range % range;
        while (true) {
            ull x = rand64();
            if (x >= threshold) return x % range;
        }
    }
    // [l, r]
    uint rand_range(uint l, uint r) {
        assert(l <= r);
        return l + uint(rand_range_exclusive(ull(r) - ull(l) + 1));
    }
    // [l, r]
    ull rand_range64(ull l, ull r) {
        assert(l <= r);
        ull range = r - l + 1;
        if (range == 0) return rand64();
        return l + rand_range_exclusive(range);
    }
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
// 最終更新: 2026-07-25 02:14:56 JST
// Aho-Corasick: 複数のパターンをテキストからまとめて検出する。
//
// 使い方:
//   AhoCorasick aho;                 // デフォルトは 'a'～'z'
//   aho.add("he");
//   aho.add("she");
//   aho.build();                     // 全パターンの add() 後に1回呼ぶ
//
//   int state = 0;
//   for (char c : text) {
//       state = aho(state, c);        // c を読んだ後の状態へ O(1) で遷移
//       if (aho.cnt[state] > 0) {
//           // この位置を右端とする登録パターンが1個以上ある。
//       }
//   }
//
// add(s) の返り値は s の終端状態。
// terminal[v]: v で直接終わる登録パターン数（同じ文字列の重複登録も数える）。
// cnt[v]:      v または failure link の先で終わる登録パターン数。
//              よってテキストがいずれかのパターンを含むかは cnt[state] > 0 で判定できる。
// fail[v]:     failure link。根の fail[0] は -1。
//
// 特定パターンの終端を target = add(pattern) で保存しても、state == target だけでは
// suffix としての一致を検出できない。例えば state が "she" の状態なら、failure link
// の先の "he" にも一致している。特定パターンだけを O(1)/文字で判定したい場合は、
// build() 後に fail[v] -> v の failure tree を作り、target の部分木に印を付ける。
// 走査中は、その印が state に付いているかを調べればよい。
//
// find(s) は s が Trie 上に存在すれば状態を返す。s 自体が登録済みか調べる場合は、
// int v = aho.find(s); v != -1 && aho.terminal[v] > 0 を確認する。
//
// アルファベットは先頭文字から文字コード上で連続している必要がある:
//   AhoCorasick<> lower;          // 'a'～'z'
//   AhoCorasick<26, 'A'> upper;   // 'A'～'Z'
//   AhoCorasick<10, '0'> digits;  // '0'～'9'
// 範囲外の文字を add(), find(), operator() に渡してはならない。
//
// 状態数 V、アルファベット数 Sigma、パターン合計長 L、テキスト長 T に対して、
// add: O(L), build: O(V * Sigma), テキスト走査: O(T), メモリ: O(V * Sigma)。
template <int Sigma = 26, char First = 'a'>
struct AhoCorasick {
    vector<array<int, Sigma>> next;
    vector<int> cnt, terminal, fail;

   private:
    vector<int> parent;
    vector<int> parent_symbol;
    bool built = false;

    static int symbol(char c) {
        int res = static_cast<unsigned char>(c) - static_cast<unsigned char>(First);
        assert(0 <= res && res < Sigma);
        return res;
    }

    void add_node(int p, int c) {
        next.emplace_back();
        next.back().fill(-1);
        cnt.push_back(0);
        terminal.push_back(0);
        parent.push_back(p);
        parent_symbol.push_back(c);
    }

   public:
    AhoCorasick() { add_node(-1, -1); }

    // パターンを登録し、その終端状態を返す。build() より前に呼ぶ。
    int add(string_view s) {
        assert(!built);
        int cur = 0;
        for (char c : s) {
            int x = symbol(c);
            if (next[cur][x] == -1) {
                int v = ssize(next);
                add_node(cur, x);
                next[cur][x] = v;
            }
            cur = next[cur][x];
        }
        terminal[cur]++;
        return cur;
    }

    // failure link、一致数、全遷移を構築する。
    // パターンを追加していなければ、複数回呼んでもよい。
    void build() {
        const int n = ssize(next);

        // build() を複数回呼んでもよいように、補完済みの遷移を除去する。
        if (built) {
            for (int v = 0; v < n; v++) {
                for (int c = 0; c < Sigma; c++) {
                    int u = next[v][c];
                    if (u != -1 && (parent[u] != v || parent_symbol[u] != c)) next[v][c] = -1;
                }
            }
        }

        cnt = terminal;
        fail.assign(n, 0);
        fail[0] = -1;
        queue<int> que;

        for (int c = 0; c < Sigma; c++) {
            int u = next[0][c];
            if (u == -1) {
                next[0][c] = 0;
            } else {
                fail[u] = 0;
                que.push(u);
            }
        }

        while (!que.empty()) {
            int v = que.front();
            que.pop();
            cnt[v] += cnt[fail[v]];

            for (int c = 0; c < Sigma; c++) {
                int u = next[v][c];
                if (u != -1 && parent[u] == v && parent_symbol[u] == c) {
                    fail[u] = next[fail[v]][c];
                    que.push(u);
                } else {
                    next[v][c] = next[fail[v]][c];
                }
            }
        }
        built = true;
    }

    // failure 遷移を使わず、Trie 上で s に対応する状態を探す。
    // s が登録パターンの接頭辞としても存在しなければ -1 を返す。
    int find(string_view s) const {
        int cur = 0;
        for (char c : s) {
            int x = symbol(c);
            int v = next[cur][x];
            // build() 後の next には failure 遷移も入るため、Trie の辺か確認する。
            if (v == -1 || parent[v] != cur || parent_symbol[v] != x) return -1;
            cur = v;
        }
        return cur;
    }

    // build() 後、状態 v から文字 c を1文字読んだ次状態を返す。
    int operator()(int v, char c) const {
        assert(built);
        if (v == -1) return 0;
        return next[v][symbol(c)];
    }
};
// 最終更新: 2026-07-12 14:27:44 JST
template <class T>
struct Matrix {
    int h, w;
    std::vector<std::vector<T>> a;

    Matrix() : h(0), w(0) {}
    Matrix(int h, int w, T val = T()) : h(h), w(w), a(h, std::vector<T>(w, val)) {}

    Matrix(std::initializer_list<std::initializer_list<T>> init) {
        h = init.size();
        w = (h == 0 ? 0 : (int)init.begin()->size());
        a.reserve(h);
        for (auto &row : init) {
            assert((int)row.size() == w);
            a.emplace_back(row);
        }
    }

    std::vector<T> &operator[](int i) { return a[i]; }
    const std::vector<T> &operator[](int i) const { return a[i]; }

    static Matrix identity(int n) {
        Matrix I(n, n);
        for (int i = 0; i < n; i++) I[i][i] = T(1);
        return I;
    }

    Matrix transpose() const {
        Matrix res(w, h);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[j][i] = a[i][j];
        return res;
    }

    /* ===== 行列同士の演算 ===== */

    Matrix operator+(const Matrix &other) const {
        assert(h == other.h && w == other.w);
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] + other[i][j];
        return res;
    }

    Matrix operator-(const Matrix &other) const {
        assert(h == other.h && w == other.w);
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] - other[i][j];
        return res;
    }

    Matrix operator*(const Matrix &other) const {
        assert(w == other.h);
        Matrix res(h, other.w, T());
        for (int i = 0; i < h; i++)
            for (int k = 0; k < w; k++)
                for (int j = 0; j < other.w; j++) res[i][j] += a[i][k] * other[k][j];
        return res;
    }

    std::vector<T> operator*(const std::vector<T> &v) const {
        assert(w == (int)v.size());
        std::vector<T> res(h, T());
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i] += a[i][j] * v[j];
        return res;
    }

    Matrix &operator+=(const Matrix &other) {
        assert(h == other.h && w == other.w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] += other[i][j];
        return *this;
    }

    Matrix &operator-=(const Matrix &other) {
        assert(h == other.h && w == other.w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] -= other[i][j];
        return *this;
    }

    Matrix &operator*=(const Matrix &other) {
        assert(w == other.h);
        *this = (*this) * other;
        return *this;
    }

    /* ===== スカラー演算 ===== */

    Matrix operator+(const T &x) const {
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] + x;
        return res;
    }

    Matrix operator-(const T &x) const {
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] - x;
        return res;
    }

    Matrix operator*(const T &x) const {
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] * x;
        return res;
    }

    Matrix operator/(const T &x) const {
        assert(x != T(0));
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] / x;
        return res;
    }

    Matrix &operator+=(const T &x) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] += x;
        return *this;
    }

    Matrix &operator-=(const T &x) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] -= x;
        return *this;
    }

    Matrix &operator*=(const T &x) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] *= x;
        return *this;
    }

    Matrix &operator/=(const T &x) {
        assert(x != T(0));
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] /= x;
        return *this;
    }

    /* ===== 累乗 ===== */

    Matrix pow(long long e) const {
        assert(h == w);
        Matrix res = identity(h);
        Matrix base = *this;
        while (e > 0) {
            if (e & 1) res *= base;
            base *= base;
            e >>= 1;
        }
        return res;
    }

    /* ===== 逆行列 ===== */

    Matrix inverse() const {
        assert(h == w);
        int n = h;

        Matrix<T> res = identity(n);
        Matrix<T> tmp = *this;

        for (int i = 0; i < n; i++) {
            // ピボット探索
            int pivot = i;
            for (int j = i; j < n; j++) {
                if (tmp[j][i] != T(0)) {
                    pivot = j;
                    break;
                }
            }
            assert(tmp[pivot][i] != T(0));  // 逆行列が存在しない

            // 行入れ替え
            if (i != pivot) {
                std::swap(tmp.a[i], tmp.a[pivot]);
                std::swap(res.a[i], res.a[pivot]);
            }

            // 正規化
            T inv = T(1) / tmp[i][i];
            for (int j = 0; j < n; j++) {
                tmp[i][j] *= inv;
                res[i][j] *= inv;
            }

            // 消去
            for (int r = 0; r < n; r++) {
                if (r == i) continue;
                T factor = tmp[r][i];
                for (int c = 0; c < n; c++) {
                    tmp[r][c] -= factor * tmp[i][c];
                    res[r][c] -= factor * res[i][c];
                }
            }
        }
        return res;
    }
};

template <class T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &m) {
    for (int i = 0; i < m.h; i++) {
        for (int j = 0; j < m.w; j++) os << m[i][j] << (j + 1 == m.w ? '\n' : ' ');
    }
    return os;
}

template <class T>
std::vector<T> operator*(const std::vector<T> &v, const Matrix<T> &m) {
    assert((int)v.size() == m.h);
    std::vector<T> res(m.w, T());
    for (int i = 0; i < m.h; i++)
        for (int j = 0; j < m.w; j++) res[j] += v[i] * m[i][j];
    return res;
}

using mint = modint998244353;
int main() {
    ll N,K;
    input(N,K);
    AhoCorasick aho;
    rep(i,K){
        string s;
        input(s);
        aho.add(s);
    }
    aho.build();
    ll M = ssize(aho.cnt);
    Matrix<mint> mat(M,M,mint(0));
    rep(i,M){
        rep(j,26){
            int nxt = aho(i,'a'+j);
            if (aho.cnt[nxt] == 0){
                mat[nxt][i]++;
            }
        }
    }
    auto res = mat.pow(N);
    mint ans = 0;
    rep(i,M){
        ans += res[i][0];
    }
    print(ans);

}
