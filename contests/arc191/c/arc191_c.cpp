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
// 最終更新: 2026-07-12 14:27:44 JST
/*ミラーラビン素数判定法 O(logn)で素数判定を行う*/
bool miller_rabin(ull n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    vector<ull> A = {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL};

    ull d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        s++;
        d >>= 1;
    }

    for (ull a : A) {
        a %= n;
        if (a == 0) continue;
        ull x = pow_mod64(a, d, n);
        if (x == 1) continue;
        for (int i = 1; x != n - 1; ++i) {
            if (i >= s) return false;
            x = (uvll)x * x % n;
        }
    }
    return true;
}
// 最終更新: 2026-07-12 15:22:19 JST
// Brent型Pollard's Rho法による unsigned 64-bit整数の素因数分解。
// 返す素因数は重複を含む昇順。
class PollardsRho {
   public:
    static vector<ull> factorization(ull n) {
        vector<ull> factors;
        if (n <= 1) return factors;

        // Rho法へ渡す前に、頻出する小さい因子を低い固定費で取り除く。
        constexpr ull small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
        for (ull prime : small_primes) {
            while (n % prime == 0) {
                factors.push_back(prime);
                n /= prime;
            }
        }

        ull random_state = splitmix64(n ^ 0x243f6a8885a308d3ULL);
        factor_rec(n, factors, random_state);
        ranges::sort(factors);
        return factors;
    }

   private:
    static ull multiply_mod(ull a, ull b, ull mod) { return (ull)((__uint128_t)a * b % mod); }

    static ull power_mod(ull base, ull exponent, ull mod) {
        ull result = 1;
        while (exponent > 0) {
            if (exponent & 1) result = multiply_mod(result, base, mod);
            base = multiply_mod(base, base, mod);
            exponent >>= 1;
        }
        return result;
    }

    static bool miller_rabin(ull n) {
        if (n < 2) return false;
        constexpr ull small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
        for (ull prime : small_primes) {
            if (n == prime) return true;
            if (n % prime == 0) return false;
        }

        ull d = n - 1;
        int s = countr_zero(d);
        d >>= s;
        constexpr ull bases[] = {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL};
        for (ull base : bases) {
            base %= n;
            if (base == 0) continue;
            ull x = power_mod(base, d, n);
            if (x == 1 || x == n - 1) continue;

            bool composite = true;
            for (int r = 1; r < s; ++r) {
                x = multiply_mod(x, x, n);
                if (x == n - 1) {
                    composite = false;
                    break;
                }
            }
            if (composite) return false;
        }
        return true;
    }

    static ull splitmix64(ull x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static ull next_random(ull &state) {
        state = splitmix64(state);
        return state;
    }

    static ull iterate(ull x, ull c, ull mod) { return (ull)(((__uint128_t)x * x + c) % mod); }

    static ull pollards_rho(ull n, ull &random_state) {
        if (n % 2 == 0) return 2;
        if (n % 3 == 0) return 3;
        constexpr ull batch_size = 128;

        while (true) {
            ull y = next_random(random_state) % (n - 2) + 2;
            ull c = next_random(random_state) % (n - 1) + 1;
            ull x = 0, saved_y = 0;
            ull radius = 1;
            ull divisor = 1;

            while (divisor == 1) {
                x = y;
                for (ull i = 0; i < radius; ++i) y = iterate(y, c, n);

                for (ull offset = 0; offset < radius && divisor == 1; offset += batch_size) {
                    saved_y = y;
                    ull product = 1;
                    ull count = min(batch_size, radius - offset);
                    for (ull i = 0; i < count; ++i) {
                        y = iterate(y, c, n);
                        ull difference = x > y ? x - y : y - x;
                        product = multiply_mod(product, difference, n);
                    }
                    divisor = gcd(product, n);
                }
                radius <<= 1;
            }

            if (divisor == n) {
                do {
                    saved_y = iterate(saved_y, c, n);
                    ull difference = x > saved_y ? x - saved_y : saved_y - x;
                    divisor = gcd(difference, n);
                } while (divisor == 1);
            }
            if (divisor != n) return divisor;
        }
    }

    static void factor_rec(ull n, vector<ull> &factors, ull &random_state) {
        if (n == 1) return;
        if (miller_rabin(n)) {
            factors.push_back(n);
            return;
        }
        ull divisor = pollards_rho(n, random_state);
        factor_rec(divisor, factors, random_state);
        factor_rec(n / divisor, factors, random_state);
    }
};

// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/algebra/number_theory/pollards_rho_factorization.cpp
// mod を法とする最小の正の原始根を返す。存在しない場合は -1 を返す。
// 原始根が存在するのは mod = 2, 4, p^k, 2*p^k（p は奇素数）の場合に限る。
//
// mod は 2 以上の signed 64-bit 整数とする。
// 素因数分解には Miller-Rabin 素数判定と Brent 型 Pollard's rho 法を使う。
// 候補 g 1個の判定は、φ(mod) の異なる素因数の個数を k として
// O(k log(mod))。最小原始根を r とすると、候補探索全体は O(r k log(mod))。
ll primitive_root(ll mod) {
    assert(mod >= 2);
    if (mod == 2) return 1;
    if (mod == 4) return 3;

    auto distinct_prime_factors = [](ll value) {
        vector<ull> factors = PollardsRho::factorization((ull)value);
        factors.erase(unique(factors.begin(), factors.end()), factors.end());
        return factors;
    };

    // 存在条件を確認し、mod の唯一の奇素因数 p を得る。
    ll odd_part = mod;
    if (odd_part % 2 == 0) odd_part /= 2;
    if (odd_part % 2 == 0) return -1;

    vector<ull> modulus_factors = distinct_prime_factors(odd_part);
    if (modulus_factors.size() != 1) return -1;
    ll odd_prime = (ll)modulus_factors[0];

    ll phi = odd_part / odd_prime * (odd_prime - 1);
    vector<ull> phi_factors = distinct_prime_factors(phi);

    auto power_mod = [mod](ll base, ll exponent) {
        ll result = 1;
        while (exponent > 0) {
            if (exponent & 1) result = (ll)((__int128_t)result * base % mod);
            base = (ll)((__int128_t)base * base % mod);
            exponent >>= 1;
        }
        return result;
    };

    // 乗法群が巡回群なので、全ての q | φ(mod) に対して
    // g^(φ(mod)/q) != 1 なら g の位数は φ(mod)。
    for (ll candidate = 2; candidate < mod; ++candidate) {
        if (gcd(candidate, mod) != 1) continue;
        bool is_primitive = true;
        for (ull prime : phi_factors) {
            if (power_mod(candidate, phi / (ll)prime) == 1) {
                is_primitive = false;
                break;
            }
        }
        if (is_primitive) return candidate;
    }
    return -1;
}

void solve(){
    ll N;
    input(N);
    Xoshiro256 rnd;
    ll res = -1;
    while(1){
        ll K = rnd.rand_range(1,1e18/N) * N + 1;
        if (miller_rabin(K)){
            res = K;
            break;
        }
    }

    ll g = primitive_root(res);
    ll ans1 = pow_mod64(g,(res-1)/N,res);
    print(ans1,res);
}

int main() {
    int T;
    input(T);
    while (T--) {
        solve();
    }
}
