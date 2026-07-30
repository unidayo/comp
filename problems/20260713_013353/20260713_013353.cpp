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
// 最終更新: 2026-07-13 01:33:07 JST
// 最終更新: 2026-07-12 14:27:44 JST
// ACL: convolution
// https://github.com/atcoder/ac-library
// clang-format off
#ifndef ATCODER
#include <algorithm>
#include <array>
#include <cassert>
#include <numeric>
#include <type_traits>
#include <utility>
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
namespace internal{
constexpr long long safe_mod(long long x,long long m){
x %= m;
if(x < 0)x += m;
return x;
}
struct barrett{
unsigned int _m;
unsigned long long im;
explicit barrett(unsigned int m): _m(m),im((unsigned long long)(-1)/ m + 1){}
unsigned int umod()const{return _m;}
unsigned int mul(unsigned int a,unsigned int b)const{
unsigned long long z = a;
z *= b;
#ifdef _MSC_VER
unsigned long long x;
_umul128(z,im,&x);
#else
unsigned long long x =
(unsigned long long)(((unsigned __int128)(z)*im)>> 64);
#endif
unsigned long long y = x * _m;
return(unsigned int)(z - y +(z < y ? _m : 0));
}
};
constexpr long long pow_mod_constexpr(long long x,long long n,int m){
if(m == 1)return 0;
unsigned int _m =(unsigned int)(m);
unsigned long long r = 1;
unsigned long long y = safe_mod(x,m);
while(n){
if(n & 1)r =(r * y)% _m;
y =(y * y)% _m;
n >>= 1;
}
return r;
}
constexpr bool is_prime_constexpr(int n){
if(n <= 1)return false;
if(n == 2 || n == 7 || n == 61)return true;
if(n % 2 == 0)return false;
long long d = n - 1;
while(d % 2 == 0)d /= 2;
constexpr long long bases[3]={2,7,61};
for(long long a : bases){
long long t = d;
long long y = pow_mod_constexpr(a,t,n);
while(t != n - 1 && y != 1 && y != n - 1){
y = y * y % n;
t <<= 1;
}
if(y != n - 1 && t % 2 == 0){
return false;
}
}
return true;
}
template <int n> constexpr bool is_prime = is_prime_constexpr(n);
constexpr std::pair<long long,long long> inv_gcd(long long a,long long b){
a = safe_mod(a,b);
if(a == 0)return{b,0};
long long s = b,t = a;
long long m0 = 0,m1 = 1;
while(t){
long long u = s / t;
s -= t * u;
m0 -= m1 * u;// |m1 * u| <= |m1| * s <= b
auto tmp = s;
s = t;
t = tmp;
tmp = m0;
m0 = m1;
m1 = tmp;
}
if(m0 < 0)m0 += b / s;
return{s,m0};
}
constexpr int primitive_root_constexpr(int m){
if(m == 2)return 1;
if(m == 167772161)return 3;
if(m == 469762049)return 3;
if(m == 754974721)return 11;
if(m == 998244353)return 3;
int divs[20]={};
divs[0]= 2;
int cnt = 1;
int x =(m - 1)/ 2;
while(x % 2 == 0)x /= 2;
for(int i = 3;(long long)(i)*i <= x;i += 2){
if(x % i == 0){
divs[cnt++]= i;
while(x % i == 0){
x /= i;
}
}
}
if(x > 1){
divs[cnt++]= x;
}
for(int g = 2;;g++){
bool ok = true;
for(int i = 0;i < cnt;i++){
if(pow_mod_constexpr(g,(m - 1)/ divs[i],m)== 1){
ok = false;
break;
}
}
if(ok)return g;
}
}
template <int m> constexpr int primitive_root = primitive_root_constexpr(m);
unsigned long long floor_sum_unsigned(unsigned long long n,
unsigned long long m,
unsigned long long a,
unsigned long long b){
unsigned long long ans = 0;
while(true){
if(a >= m){
ans += n *(n - 1)/ 2 *(a / m);
a %= m;
}
if(b >= m){
ans += n *(b / m);
b %= m;
}
unsigned long long y_max = a * n + b;
if(y_max < m)break;
n =(unsigned long long)(y_max / m);
b =(unsigned long long)(y_max % m);
std::swap(m,a);
}
return ans;
}
}// namespace internal
}// namespace atcoder
namespace atcoder{
namespace internal{
#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
typename std::conditional<std::is_same<T,__int128_t>::value ||
std::is_same<T,__int128>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_unsigned_int128 =
typename std::conditional<std::is_same<T,__uint128_t>::value ||
std::is_same<T,unsigned __int128>::value,
std::true_type,
std::false_type>::type;
template <class T>
using make_unsigned_int128 =
typename std::conditional<std::is_same<T,__int128_t>::value,
__uint128_t,
unsigned __int128>;
template <class T>
using is_integral = typename std::conditional<std::is_integral<T>::value ||
is_signed_int128<T>::value ||
is_unsigned_int128<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
std::is_signed<T>::value)||
is_signed_int128<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_unsigned_int =
typename std::conditional<(is_integral<T>::value &&
std::is_unsigned<T>::value)||
is_unsigned_int128<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using to_unsigned = typename std::conditional<
is_signed_int128<T>::value,
make_unsigned_int128<T>,
typename std::conditional<std::is_signed<T>::value,
std::make_unsigned<T>,
std::common_type<T>>::type>::type;
#else
template <class T> using is_integral = typename std::is_integral<T>;
template <class T>
using is_signed_int =
typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_unsigned_int =
typename std::conditional<is_integral<T>::value &&
std::is_unsigned<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using to_unsigned = typename std::conditional<is_signed_int<T>::value,
std::make_unsigned<T>,
std::common_type<T>>::type;
#endif
template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;
template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;
template <class T> using to_unsigned_t = typename to_unsigned<T>::type;
}// namespace internal
}// namespace atcoder
namespace atcoder{
namespace internal{
struct modint_base{};
struct static_modint_base : modint_base{};
template <class T> using is_modint = std::is_base_of<modint_base,T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;
}// namespace internal
template <int m,std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base{
using mint = static_modint;
public:
static constexpr int mod(){return m;}
static mint raw(int v){
mint x;
x._v = v;
return x;
}
static_modint(): _v(0){}
template <class T,internal::is_signed_int_t<T>* = nullptr>
static_modint(T v){
long long x =(long long)(v %(long long)(umod()));
if(x < 0)x += umod();
_v =(unsigned int)(x);
}
template <class T,internal::is_unsigned_int_t<T>* = nullptr>
static_modint(T v){
_v =(unsigned int)(v % umod());
}
unsigned int val()const{return _v;}
mint& operator++(){
_v++;
if(_v == umod())_v = 0;
return *this;
}
mint& operator--(){
if(_v == 0)_v = umod();
_v--;
return *this;
}
mint operator++(int){
mint result = *this;
++*this;
return result;
}
mint operator--(int){
mint result = *this;
--*this;
return result;
}
mint& operator+=(const mint& rhs){
_v += rhs._v;
if(_v >= umod())_v -= umod();
return *this;
}
mint& operator-=(const mint& rhs){
_v -= rhs._v;
if(_v >= umod())_v += umod();
return *this;
}
mint& operator*=(const mint& rhs){
unsigned long long z = _v;
z *= rhs._v;
_v =(unsigned int)(z % umod());
return *this;
}
mint& operator/=(const mint& rhs){return *this = *this * rhs.inv();}
mint operator+()const{return *this;}
mint operator-()const{return mint()- *this;}
mint pow(long long n)const{
assert(0 <= n);
mint x = *this,r = 1;
while(n){
if(n & 1)r *= x;
x *= x;
n >>= 1;
}
return r;
}
mint inv()const{
if(prime){
assert(_v);
return pow(umod()- 2);
}else{
auto eg = internal::inv_gcd(_v,m);
assert(eg.first == 1);
return eg.second;
}
}
friend mint operator+(const mint& lhs,const mint& rhs){
return mint(lhs)+= rhs;
}
friend mint operator-(const mint& lhs,const mint& rhs){
return mint(lhs)-= rhs;
}
friend mint operator*(const mint& lhs,const mint& rhs){
return mint(lhs)*= rhs;
}
friend mint operator/(const mint& lhs,const mint& rhs){
return mint(lhs)/= rhs;
}
friend bool operator==(const mint& lhs,const mint& rhs){
return lhs._v == rhs._v;
}
friend bool operator!=(const mint& lhs,const mint& rhs){
return lhs._v != rhs._v;
}
private:
unsigned int _v;
static constexpr unsigned int umod(){return m;}
static constexpr bool prime = internal::is_prime<m>;
};
template <int id> struct dynamic_modint : internal::modint_base{
using mint = dynamic_modint;
public:
static int mod(){return(int)(bt.umod());}
static void set_mod(int m){
assert(1 <= m);
bt = internal::barrett(m);
}
static mint raw(int v){
mint x;
x._v = v;
return x;
}
dynamic_modint(): _v(0){}
template <class T,internal::is_signed_int_t<T>* = nullptr>
dynamic_modint(T v){
long long x =(long long)(v %(long long)(mod()));
if(x < 0)x += mod();
_v =(unsigned int)(x);
}
template <class T,internal::is_unsigned_int_t<T>* = nullptr>
dynamic_modint(T v){
_v =(unsigned int)(v % mod());
}
unsigned int val()const{return _v;}
mint& operator++(){
_v++;
if(_v == umod())_v = 0;
return *this;
}
mint& operator--(){
if(_v == 0)_v = umod();
_v--;
return *this;
}
mint operator++(int){
mint result = *this;
++*this;
return result;
}
mint operator--(int){
mint result = *this;
--*this;
return result;
}
mint& operator+=(const mint& rhs){
_v += rhs._v;
if(_v >= umod())_v -= umod();
return *this;
}
mint& operator-=(const mint& rhs){
_v += mod()- rhs._v;
if(_v >= umod())_v -= umod();
return *this;
}
mint& operator*=(const mint& rhs){
_v = bt.mul(_v,rhs._v);
return *this;
}
mint& operator/=(const mint& rhs){return *this = *this * rhs.inv();}
mint operator+()const{return *this;}
mint operator-()const{return mint()- *this;}
mint pow(long long n)const{
assert(0 <= n);
mint x = *this,r = 1;
while(n){
if(n & 1)r *= x;
x *= x;
n >>= 1;
}
return r;
}
mint inv()const{
auto eg = internal::inv_gcd(_v,mod());
assert(eg.first == 1);
return eg.second;
}
friend mint operator+(const mint& lhs,const mint& rhs){
return mint(lhs)+= rhs;
}
friend mint operator-(const mint& lhs,const mint& rhs){
return mint(lhs)-= rhs;
}
friend mint operator*(const mint& lhs,const mint& rhs){
return mint(lhs)*= rhs;
}
friend mint operator/(const mint& lhs,const mint& rhs){
return mint(lhs)/= rhs;
}
friend bool operator==(const mint& lhs,const mint& rhs){
return lhs._v == rhs._v;
}
friend bool operator!=(const mint& lhs,const mint& rhs){
return lhs._v != rhs._v;
}
private:
unsigned int _v;
static internal::barrett bt;
static unsigned int umod(){return bt.umod();}
};
template <int id> internal::barrett dynamic_modint<id>::bt(998244353);
using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;
namespace internal{
template <class T>
using is_static_modint = std::is_base_of<internal::static_modint_base,T>;
template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;
template <class> struct is_dynamic_modint : public std::false_type{};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type{};
template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;
}// namespace internal
}// namespace atcoder
namespace atcoder{
namespace internal{
template <class mint,
int g = internal::primitive_root<mint::mod()>,
internal::is_static_modint_t<mint>* = nullptr>
struct fft_info{
static constexpr int rank2 = countr_zero_constexpr(mint::mod()- 1);
std::array<mint,rank2 + 1> root;// root[i]^(2^i)== 1
std::array<mint,rank2 + 1> iroot;// root[i]* iroot[i]== 1
std::array<mint,std::max(0,rank2 - 2 + 1)> rate2;
std::array<mint,std::max(0,rank2 - 2 + 1)> irate2;
std::array<mint,std::max(0,rank2 - 3 + 1)> rate3;
std::array<mint,std::max(0,rank2 - 3 + 1)> irate3;
fft_info(){
root[rank2]= mint(g).pow((mint::mod()- 1)>> rank2);
iroot[rank2]= root[rank2].inv();
for(int i = rank2 - 1;i >= 0;i--){
root[i]= root[i + 1]* root[i + 1];
iroot[i]= iroot[i + 1]* iroot[i + 1];
}
{
mint prod = 1,iprod = 1;
for(int i = 0;i <= rank2 - 2;i++){
rate2[i]= root[i + 2]* prod;
irate2[i]= iroot[i + 2]* iprod;
prod *= iroot[i + 2];
iprod *= root[i + 2];
}
}
{
mint prod = 1,iprod = 1;
for(int i = 0;i <= rank2 - 3;i++){
rate3[i]= root[i + 3]* prod;
irate3[i]= iroot[i + 3]* iprod;
prod *= iroot[i + 3];
iprod *= root[i + 3];
}
}
}
};
template <class mint,internal::is_static_modint_t<mint>* = nullptr>
void butterfly(std::vector<mint>& a){
int n = int(a.size());
int h = internal::countr_zero((unsigned int)n);
static const fft_info<mint> info;
int len = 0;// a[i,i+(n>>len),i+2*(n>>len),..]is transformed
while(len < h){
if(h - len == 1){
int p = 1 <<(h - len - 1);
mint rot = 1;
for(int s = 0;s <(1 << len);s++){
int offset = s <<(h - len);
for(int i = 0;i < p;i++){
auto l = a[i + offset];
auto r = a[i + offset + p]* rot;
a[i + offset]= l + r;
a[i + offset + p]= l - r;
}
if(s + 1 !=(1 << len))
rot *= info.rate2[countr_zero(~(unsigned int)(s))];
}
len++;
}else{
int p = 1 <<(h - len - 2);
mint rot = 1,imag = info.root[2];
for(int s = 0;s <(1 << len);s++){
mint rot2 = rot * rot;
mint rot3 = rot2 * rot;
int offset = s <<(h - len);
for(int i = 0;i < p;i++){
auto mod2 = 1ULL * mint::mod()* mint::mod();
auto a0 = 1ULL * a[i + offset].val();
auto a1 = 1ULL * a[i + offset + p].val()* rot.val();
auto a2 = 1ULL * a[i + offset + 2 * p].val()* rot2.val();
auto a3 = 1ULL * a[i + offset + 3 * p].val()* rot3.val();
auto a1na3imag =
1ULL * mint(a1 + mod2 - a3).val()* imag.val();
auto na2 = mod2 - a2;
a[i + offset]= a0 + a2 + a1 + a3;
a[i + offset + 1 * p]= a0 + a2 +(2 * mod2 -(a1 + a3));
a[i + offset + 2 * p]= a0 + na2 + a1na3imag;
a[i + offset + 3 * p]= a0 + na2 +(mod2 - a1na3imag);
}
if(s + 1 !=(1 << len))
rot *= info.rate3[countr_zero(~(unsigned int)(s))];
}
len += 2;
}
}
}
template <class mint,internal::is_static_modint_t<mint>* = nullptr>
void butterfly_inv(std::vector<mint>& a){
int n = int(a.size());
int h = internal::countr_zero((unsigned int)n);
static const fft_info<mint> info;
int len = h;// a[i,i+(n>>len),i+2*(n>>len),..]is transformed
while(len){
if(len == 1){
int p = 1 <<(h - len);
mint irot = 1;
for(int s = 0;s <(1 <<(len - 1));s++){
int offset = s <<(h - len + 1);
for(int i = 0;i < p;i++){
auto l = a[i + offset];
auto r = a[i + offset + p];
a[i + offset]= l + r;
a[i + offset + p]=
(unsigned long long)(mint::mod()+ l.val()- r.val())*
irot.val();
;
}
if(s + 1 !=(1 <<(len - 1)))
irot *= info.irate2[countr_zero(~(unsigned int)(s))];
}
len--;
}else{
int p = 1 <<(h - len);
mint irot = 1,iimag = info.iroot[2];
for(int s = 0;s <(1 <<(len - 2));s++){
mint irot2 = irot * irot;
mint irot3 = irot2 * irot;
int offset = s <<(h - len + 2);
for(int i = 0;i < p;i++){
auto a0 = 1ULL * a[i + offset + 0 * p].val();
auto a1 = 1ULL * a[i + offset + 1 * p].val();
auto a2 = 1ULL * a[i + offset + 2 * p].val();
auto a3 = 1ULL * a[i + offset + 3 * p].val();
auto a2na3iimag =
1ULL *
mint((mint::mod()+ a2 - a3)* iimag.val()).val();
a[i + offset]= a0 + a1 + a2 + a3;
a[i + offset + 1 * p]=
(a0 +(mint::mod()- a1)+ a2na3iimag)* irot.val();
a[i + offset + 2 * p]=
(a0 + a1 +(mint::mod()- a2)+(mint::mod()- a3))*
irot2.val();
a[i + offset + 3 * p]=
(a0 +(mint::mod()- a1)+(mint::mod()- a2na3iimag))*
irot3.val();
}
if(s + 1 !=(1 <<(len - 2)))
irot *= info.irate3[countr_zero(~(unsigned int)(s))];
}
len -= 2;
}
}
}
template <class mint,internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution_naive(const std::vector<mint>& a,
const std::vector<mint>& b){
int n = int(a.size()),m = int(b.size());
std::vector<mint> ans(n + m - 1);
if(n < m){
for(int j = 0;j < m;j++){
for(int i = 0;i < n;i++){
ans[i + j]+= a[i]* b[j];
}
}
}else{
for(int i = 0;i < n;i++){
for(int j = 0;j < m;j++){
ans[i + j]+= a[i]* b[j];
}
}
}
return ans;
}
template <class mint,internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution_fft(std::vector<mint> a,std::vector<mint> b){
int n = int(a.size()),m = int(b.size());
int z =(int)internal::bit_ceil((unsigned int)(n + m - 1));
a.resize(z);
internal::butterfly(a);
b.resize(z);
internal::butterfly(b);
for(int i = 0;i < z;i++){
a[i]*= b[i];
}
internal::butterfly_inv(a);
a.resize(n + m - 1);
mint iz = mint(z).inv();
for(int i = 0;i < n + m - 1;i++)a[i]*= iz;
return a;
}
}// namespace internal
template <class mint,internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution(std::vector<mint>&& a,std::vector<mint>&& b){
int n = int(a.size()),m = int(b.size());
if(!n || !m)return{};
int z =(int)internal::bit_ceil((unsigned int)(n + m - 1));
assert((mint::mod()- 1)% z == 0);
if(std::min(n,m)<= 60)return convolution_naive(a,b);
return internal::convolution_fft(a,b);
}
template <class mint,internal::is_static_modint_t<mint>* = nullptr>
std::vector<mint> convolution(const std::vector<mint>& a,
const std::vector<mint>& b){
int n = int(a.size()),m = int(b.size());
if(!n || !m)return{};
int z =(int)internal::bit_ceil((unsigned int)(n + m - 1));
assert((mint::mod()- 1)% z == 0);
if(std::min(n,m)<= 60)return convolution_naive(a,b);
return internal::convolution_fft(a,b);
}
template <unsigned int mod = 998244353,
class T,
std::enable_if_t<internal::is_integral<T>::value>* = nullptr>
std::vector<T> convolution(const std::vector<T>& a,const std::vector<T>& b){
int n = int(a.size()),m = int(b.size());
if(!n || !m)return{};
using mint = static_modint<mod>;
int z =(int)internal::bit_ceil((unsigned int)(n + m - 1));
assert((mint::mod()- 1)% z == 0);
std::vector<mint> a2(n),b2(m);
for(int i = 0;i < n;i++){
a2[i]= mint(a[i]);
}
for(int i = 0;i < m;i++){
b2[i]= mint(b[i]);
}
auto c2 = convolution(std::move(a2),std::move(b2));
std::vector<T> c(n + m - 1);
for(int i = 0;i < n + m - 1;i++){
c[i]= c2[i].val();
}
return c;
}
std::vector<long long> convolution_ll(const std::vector<long long>& a,
const std::vector<long long>& b){
int n = int(a.size()),m = int(b.size());
if(!n || !m)return{};
static constexpr unsigned long long MOD1 = 754974721;// 2^24
static constexpr unsigned long long MOD2 = 167772161;// 2^25
static constexpr unsigned long long MOD3 = 469762049;// 2^26
static constexpr unsigned long long M2M3 = MOD2 * MOD3;
static constexpr unsigned long long M1M3 = MOD1 * MOD3;
static constexpr unsigned long long M1M2 = MOD1 * MOD2;
static constexpr unsigned long long M1M2M3 = MOD1 * MOD2 * MOD3;
static constexpr unsigned long long i1 =
internal::inv_gcd(MOD2 * MOD3,MOD1).second;
static constexpr unsigned long long i2 =
internal::inv_gcd(MOD1 * MOD3,MOD2).second;
static constexpr unsigned long long i3 =
internal::inv_gcd(MOD1 * MOD2,MOD3).second;
static constexpr int MAX_AB_BIT = 24;
static_assert(MOD1 %(1ull << MAX_AB_BIT)== 1,"MOD1 isn't enough to support an array length of 2^24.");
static_assert(MOD2 %(1ull << MAX_AB_BIT)== 1,"MOD2 isn't enough to support an array length of 2^24.");
static_assert(MOD3 %(1ull << MAX_AB_BIT)== 1,"MOD3 isn't enough to support an array length of 2^24.");
assert(n + m - 1 <=(1 << MAX_AB_BIT));
auto c1 = convolution<MOD1>(a,b);
auto c2 = convolution<MOD2>(a,b);
auto c3 = convolution<MOD3>(a,b);
std::vector<long long> c(n + m - 1);
for(int i = 0;i < n + m - 1;i++){
unsigned long long x = 0;
x +=(c1[i]* i1)% MOD1 * M2M3;
x +=(c2[i]* i2)% MOD2 * M1M3;
x +=(c3[i]* i3)% MOD3 * M1M2;
long long diff =
c1[i]- internal::safe_mod((long long)(x),(long long)(MOD1));
if(diff < 0)diff += MOD1;
static constexpr unsigned long long offset[5]={
0,0,M1M2M3,2 * M1M2M3,3 * M1M2M3};
x -= offset[diff % 5];
c[i]= x;
}
return c;
}
}// namespace atcoder
// clang-format on
using namespace atcoder;
#endif  // ATCODER

// 最終更新: 2026-07-12 14:27:44 JST
// ACL: modint
// https://github.com/atcoder/ac-library
// clang-format off
// convolutionの展開済みコードにmodintと内部数学実装が含まれているため、
// 重複する2個目のmodint本体はコンパイルしない。
#if 0
#include <cassert>
#include <istream>
#include <numeric>
#include <ostream>
#include <type_traits>
#include <utility>
#ifdef _MSC_VER
#include <intrin.h>
#endif
namespace atcoder{
namespace internal{
constexpr long long safe_mod(long long x,long long m){
x %= m;
if(x < 0)x += m;
return x;
}
struct barrett{
unsigned int _m;
unsigned long long im;
explicit barrett(unsigned int m): _m(m),im((unsigned long long)(-1)/ m + 1){}
unsigned int umod()const{return _m;}
unsigned int mul(unsigned int a,unsigned int b)const{
unsigned long long z = a;
z *= b;
#ifdef _MSC_VER
unsigned long long x;
_umul128(z,im,&x);
#else
unsigned long long x =
(unsigned long long)(((unsigned __int128)(z)*im)>> 64);
#endif
unsigned long long y = x * _m;
return(unsigned int)(z - y +(z < y ? _m : 0));
}
};
constexpr long long pow_mod_constexpr(long long x,long long n,int m){
if(m == 1)return 0;
unsigned int _m =(unsigned int)(m);
unsigned long long r = 1;
unsigned long long y = safe_mod(x,m);
while(n){
if(n & 1)r =(r * y)% _m;
y =(y * y)% _m;
n >>= 1;
}
return r;
}
constexpr bool is_prime_constexpr(int n){
if(n <= 1)return false;
if(n == 2 || n == 7 || n == 61)return true;
if(n % 2 == 0)return false;
long long d = n - 1;
while(d % 2 == 0)d /= 2;
constexpr long long bases[3]={2,7,61};
for(long long a : bases){
long long t = d;
long long y = pow_mod_constexpr(a,t,n);
while(t != n - 1 && y != 1 && y != n - 1){
y = y * y % n;
t <<= 1;
}
if(y != n - 1 && t % 2 == 0){
return false;
}
}
return true;
}
template <int n> constexpr bool is_prime = is_prime_constexpr(n);
constexpr std::pair<long long,long long> inv_gcd(long long a,long long b){
a = safe_mod(a,b);
if(a == 0)return{b,0};
long long s = b,t = a;
long long m0 = 0,m1 = 1;
while(t){
long long u = s / t;
s -= t * u;
m0 -= m1 * u;// |m1 * u| <= |m1| * s <= b
auto tmp = s;
s = t;
t = tmp;
tmp = m0;
m0 = m1;
m1 = tmp;
}
if(m0 < 0)m0 += b / s;
return{s,m0};
}
constexpr int primitive_root_constexpr(int m){
if(m == 2)return 1;
if(m == 167772161)return 3;
if(m == 469762049)return 3;
if(m == 754974721)return 11;
if(m == 998244353)return 3;
int divs[20]={};
divs[0]= 2;
int cnt = 1;
int x =(m - 1)/ 2;
while(x % 2 == 0)x /= 2;
for(int i = 3;(long long)(i)*i <= x;i += 2){
if(x % i == 0){
divs[cnt++]= i;
while(x % i == 0){
x /= i;
}
}
}
if(x > 1){
divs[cnt++]= x;
}
for(int g = 2;;g++){
bool ok = true;
for(int i = 0;i < cnt;i++){
if(pow_mod_constexpr(g,(m - 1)/ divs[i],m)== 1){
ok = false;
break;
}
}
if(ok)return g;
}
}
template <int m> constexpr int primitive_root = primitive_root_constexpr(m);
unsigned long long floor_sum_unsigned(unsigned long long n,
unsigned long long m,
unsigned long long a,
unsigned long long b){
unsigned long long ans = 0;
while(true){
if(a >= m){
ans += n *(n - 1)/ 2 *(a / m);
a %= m;
}
if(b >= m){
ans += n *(b / m);
b %= m;
}
unsigned long long y_max = a * n + b;
if(y_max < m)break;
n =(unsigned long long)(y_max / m);
b =(unsigned long long)(y_max % m);
std::swap(m,a);
}
return ans;
}
}// namespace internal
}// namespace atcoder
namespace atcoder{
namespace internal{
#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
typename std::conditional<std::is_same<T,__int128_t>::value ||
std::is_same<T,__int128>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_unsigned_int128 =
typename std::conditional<std::is_same<T,__uint128_t>::value ||
std::is_same<T,unsigned __int128>::value,
std::true_type,
std::false_type>::type;
template <class T>
using make_unsigned_int128 =
typename std::conditional<std::is_same<T,__int128_t>::value,
__uint128_t,
unsigned __int128>;
template <class T>
using is_integral = typename std::conditional<std::is_integral<T>::value ||
is_signed_int128<T>::value ||
is_unsigned_int128<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_signed_int = typename std::conditional<(is_integral<T>::value &&
std::is_signed<T>::value)||
is_signed_int128<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_unsigned_int =
typename std::conditional<(is_integral<T>::value &&
std::is_unsigned<T>::value)||
is_unsigned_int128<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using to_unsigned = typename std::conditional<
is_signed_int128<T>::value,
make_unsigned_int128<T>,
typename std::conditional<std::is_signed<T>::value,
std::make_unsigned<T>,
std::common_type<T>>::type>::type;
#else
template <class T> using is_integral = typename std::is_integral<T>;
template <class T>
using is_signed_int =
typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using is_unsigned_int =
typename std::conditional<is_integral<T>::value &&
std::is_unsigned<T>::value,
std::true_type,
std::false_type>::type;
template <class T>
using to_unsigned = typename std::conditional<is_signed_int<T>::value,
std::make_unsigned<T>,
std::common_type<T>>::type;
#endif
template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;
template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;
template <class T> using to_unsigned_t = typename to_unsigned<T>::type;
}// namespace internal
}// namespace atcoder
namespace atcoder{
namespace internal{
struct modint_base{};
struct static_modint_base : modint_base{};
template <class T> using is_modint = std::is_base_of<modint_base,T>;
template <class T> using is_modint_t = std::enable_if_t<is_modint<T>::value>;
}// namespace internal
template <int m,std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base{
using mint = static_modint;
public:
static constexpr int mod(){return m;}
static mint raw(int v){
mint x;
x._v = v;
return x;
}
static_modint(): _v(0){}
template <class T,internal::is_signed_int_t<T>* = nullptr>
static_modint(T v){
long long x =(long long)(v %(long long)(umod()));
if(x < 0)x += umod();
_v =(unsigned int)(x);
}
template <class T,internal::is_unsigned_int_t<T>* = nullptr>
static_modint(T v){
_v =(unsigned int)(v % umod());
}
unsigned int val()const{return _v;}
mint& operator++(){
_v++;
if(_v == umod())_v = 0;
return *this;
}
mint& operator--(){
if(_v == 0)_v = umod();
_v--;
return *this;
}
mint operator++(int){
mint result = *this;
++*this;
return result;
}
mint operator--(int){
mint result = *this;
--*this;
return result;
}
mint& operator+=(const mint& rhs){
_v += rhs._v;
if(_v >= umod())_v -= umod();
return *this;
}
mint& operator-=(const mint& rhs){
_v -= rhs._v;
if(_v >= umod())_v += umod();
return *this;
}
mint& operator*=(const mint& rhs){
unsigned long long z = _v;
z *= rhs._v;
_v =(unsigned int)(z % umod());
return *this;
}
mint& operator/=(const mint& rhs){return *this = *this * rhs.inv();}
mint operator+()const{return *this;}
mint operator-()const{return mint()- *this;}
mint pow(long long n)const{
assert(0 <= n);
mint x = *this,r = 1;
while(n){
if(n & 1)r *= x;
x *= x;
n >>= 1;
}
return r;
}
mint inv()const{
if(prime){
assert(_v);
return pow(umod()- 2);
}else{
auto eg = internal::inv_gcd(_v,m);
assert(eg.first == 1);
return eg.second;
}
}
friend mint operator+(const mint& lhs,const mint& rhs){
return mint(lhs)+= rhs;
}
friend mint operator-(const mint& lhs,const mint& rhs){
return mint(lhs)-= rhs;
}
friend mint operator*(const mint& lhs,const mint& rhs){
return mint(lhs)*= rhs;
}
friend mint operator/(const mint& lhs,const mint& rhs){
return mint(lhs)/= rhs;
}
friend bool operator==(const mint& lhs,const mint& rhs){
return lhs._v == rhs._v;
}
friend bool operator!=(const mint& lhs,const mint& rhs){
return lhs._v != rhs._v;
}
private:
unsigned int _v;
static constexpr unsigned int umod(){return m;}
static constexpr bool prime = internal::is_prime<m>;
};
template <int id> struct dynamic_modint : internal::modint_base{
using mint = dynamic_modint;
public:
static int mod(){return(int)(bt.umod());}
static void set_mod(int m){
assert(1 <= m);
bt = internal::barrett(m);
}
static mint raw(int v){
mint x;
x._v = v;
return x;
}
dynamic_modint(): _v(0){}
template <class T,internal::is_signed_int_t<T>* = nullptr>
dynamic_modint(T v){
long long x =(long long)(v %(long long)(mod()));
if(x < 0)x += mod();
_v =(unsigned int)(x);
}
template <class T,internal::is_unsigned_int_t<T>* = nullptr>
dynamic_modint(T v){
_v =(unsigned int)(v % mod());
}
unsigned int val()const{return _v;}
mint& operator++(){
_v++;
if(_v == umod())_v = 0;
return *this;
}
mint& operator--(){
if(_v == 0)_v = umod();
_v--;
return *this;
}
mint operator++(int){
mint result = *this;
++*this;
return result;
}
mint operator--(int){
mint result = *this;
--*this;
return result;
}
mint& operator+=(const mint& rhs){
_v += rhs._v;
if(_v >= umod())_v -= umod();
return *this;
}
mint& operator-=(const mint& rhs){
_v += mod()- rhs._v;
if(_v >= umod())_v -= umod();
return *this;
}
mint& operator*=(const mint& rhs){
_v = bt.mul(_v,rhs._v);
return *this;
}
mint& operator/=(const mint& rhs){return *this = *this * rhs.inv();}
mint operator+()const{return *this;}
mint operator-()const{return mint()- *this;}
mint pow(long long n)const{
assert(0 <= n);
mint x = *this,r = 1;
while(n){
if(n & 1)r *= x;
x *= x;
n >>= 1;
}
return r;
}
mint inv()const{
auto eg = internal::inv_gcd(_v,mod());
assert(eg.first == 1);
return eg.second;
}
friend mint operator+(const mint& lhs,const mint& rhs){
return mint(lhs)+= rhs;
}
friend mint operator-(const mint& lhs,const mint& rhs){
return mint(lhs)-= rhs;
}
friend mint operator*(const mint& lhs,const mint& rhs){
return mint(lhs)*= rhs;
}
friend mint operator/(const mint& lhs,const mint& rhs){
return mint(lhs)/= rhs;
}
friend bool operator==(const mint& lhs,const mint& rhs){
return lhs._v == rhs._v;
}
friend bool operator!=(const mint& lhs,const mint& rhs){
return lhs._v != rhs._v;
}
private:
unsigned int _v;
static internal::barrett bt;
static unsigned int umod(){return bt.umod();}
};
template <int id> internal::barrett dynamic_modint<id>::bt(998244353);
using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;
namespace internal{
template <class T>
using is_static_modint = std::is_base_of<internal::static_modint_base,T>;
template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;
template <class> struct is_dynamic_modint : public std::false_type{};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type{};
template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;
}// namespace internal
}// namespace atcoder
// clang-format on
using namespace atcoder;
template <int m>
std::istream& operator>>(std::istream& is, static_modint<m>& x) {
    long long val;
    is >> val;
    x = val;
    return is;
}
template <int m>
std::ostream& operator<<(std::ostream& os, const static_modint<m>& x) {
    return os << x.val();
}
template <int id>
std::istream& operator>>(std::istream& is, dynamic_modint<id>& x) {
    long long val;
    is >> val;
    x = val;
    return is;
}
template <int id>
std::ostream& operator<<(std::ostream& os, const dynamic_modint<id>& x) {
    return os << x.val();
}
#endif  // ATCODER

#ifndef ATCODER
namespace atcoder {
template <int m>
std::istream &operator>>(std::istream &is, static_modint<m> &x) {
    long long value;
    is >> value;
    x = value;
    return is;
}
template <int m>
std::ostream &operator<<(std::ostream &os, const static_modint<m> &x) {
    return os << x.val();
}
template <int id>
std::istream &operator>>(std::istream &is, dynamic_modint<id> &x) {
    long long value;
    is >> value;
    x = value;
    return is;
}
template <int id>
std::ostream &operator<<(std::ostream &os, const dynamic_modint<id> &x) {
    return os << x.val();
}
}  // namespace atcoder
#endif

// 形式的冪級数ライブラリ (mod 998244353)
// ACL の convolution を使用
using mint = modint998244353;

struct FPS : vector<mint> {
    using vector<mint>::vector;

    FPS(const vector<mint> &v) : vector<mint>(v) {}

   private:
    static const vector<mint> &inverse_table(int n) {
        static vector<mint> inverse = {mint(0), mint(1)};
        assert(n < mint::mod());
        if ((int)inverse.size() <= n) {
            int old_size = inverse.size();
            inverse.resize(n + 1);
            for (int i = max(2, old_size); i <= n; ++i) {
                inverse[i] = -mint(mint::mod() / i) * inverse[mint::mod() % i];
            }
        }
        return inverse;
    }

    static optional<mint> modular_sqrt(mint a) {
        if (a == mint(0)) return mint(0);
        int mod = mint::mod();
        if (a.pow((mod - 1) / 2) != mint(1)) return nullopt;
        if (mod % 4 == 3) {
            mint result = a.pow((mod + 1) / 4);
            if (result.val() > mod - result.val()) result = -result;
            return result;
        }
        int s = 0;
        int q = mod - 1;
        while ((q & 1) == 0) q >>= 1, ++s;
        mint z = 2;
        while (z.pow((mod - 1) / 2) == mint(1)) z += 1;
        mint c = z.pow(q);
        mint x = a.pow((q + 1) / 2);
        mint t = a.pow(q);
        int m = s;
        while (t != mint(1)) {
            int i = 1;
            mint squared = t * t;
            while (squared != mint(1)) squared *= squared, ++i;
            mint b = c.pow(1LL << (m - i - 1));
            x *= b;
            c = b * b;
            t *= c;
            m = i;
        }
        if (x.val() > mod - x.val()) x = -x;
        return x;
    }

   public:
    // サイズを n に縮小/拡張
    void shrink() {
        while (!empty() && back() == mint(0)) pop_back();
    }

    FPS pre(int n) const {
        assert(n >= 0);
        FPS res(begin(), begin() + min((int)size(), n));
        res.resize(n);
        return res;
    }

    // 加算
    FPS operator+(const FPS &r) const {
        FPS res = *this;
        if (res.size() < r.size()) res.resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) res[i] += r[i];
        return res;
    }

    // 減算
    FPS operator-(const FPS &r) const {
        FPS res = *this;
        if (res.size() < r.size()) res.resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) res[i] -= r[i];
        return res;
    }

    // 乗算 (ACL convolution)
    FPS operator*(const FPS &r) const {
        if (empty() || r.empty()) return FPS();
        auto res = convolution(*this, r);
        return FPS(res);
    }

    FPS operator*(mint c) const {
        FPS res = *this;
        for (auto &x : res) x *= c;
        return res;
    }

    FPS &operator+=(const FPS &r) {
        if (size() < r.size()) resize(r.size());
        for (int i = 0; i < (int)r.size(); ++i) (*this)[i] += r[i];
        return *this;
    }
    FPS &operator-=(const FPS &r) {
        if (size() < r.size()) resize(r.size());
        for (int i = 0; i < (int)r.size(); ++i) (*this)[i] -= r[i];
        return *this;
    }
    FPS &operator*=(const FPS &r) { return *this = *this * r; }
    FPS &operator*=(mint c) {
        for (auto &x : *this) x *= c;
        return *this;
    }

    // 疎な多項式との乗算 O(n * |G|)
    // G は {(次数, 係数)} のリスト（次数昇順）
    FPS sparse_mul(const vector<pair<int, mint>> &g) const {
        int n = size();
        FPS res(n);
        for (auto [degree, coefficient] : g) {
            if (degree >= n) break;
            for (int i = 0; i + degree < n; ++i) {
                res[i + degree] += (*this)[i] * coefficient;
            }
        }
        return res;
    }

    // 疎な多項式での除算 O(n * |G|)
    // F(x) / G(x) mod x^n を計算
    // G は {(次数, 係数)} のリスト（次数昇順）, g[0] != 0 が必要
    // Q[i] = (F[i] - Σ_{(d,c)∈G, d≥1} c·Q[i-d]) / g0
    FPS sparse_div(const vector<pair<int, mint>> &g, int n = -1) const {
        assert(!g.empty() && g[0].first == 0 && g[0].second != mint(0));
        if (n < 0) n = size();
        mint inv0 = g[0].second.inv();
        FPS res(n);
        for (int i = 0; i < n; i++) {
            mint s = (i < (int)size()) ? (*this)[i] : mint(0);
            for (auto &[d, c] : g) {
                if (d == 0) continue;
                if (d > i) break;
                s -= c * res[i - d];
            }
            res[i] = s * inv0;
        }
        return res;
    }

    // FPS から疎表現 {(次数, 係数)} を生成するヘルパー
    static vector<pair<int, mint>> to_sparse(const FPS &f) {
        vector<pair<int, mint>> res;
        for (int i = 0; i < (int)f.size(); i++) {
            if (f[i] != mint(0)) res.emplace_back(i, f[i]);
        }
        return res;
    }

    // 微分
    FPS diff() const {
        if (size() <= 1) return FPS();
        FPS res(size() - 1);
        for (int i = 1; i < (int)size(); i++) {
            res[i - 1] = (*this)[i] * mint(i);
        }
        return res;
    }

    // 積分
    FPS integral() const {
        FPS res(size() + 1);
        const auto &inverse = inverse_table(size());
        for (int i = 0; i < (int)size(); i++) {
            res[i + 1] = (*this)[i] * inverse[i + 1];
        }
        return res;
    }

    // 逆元 f^{-1} mod x^n (Newton法)
    // f[0] != 0 が必要
    FPS inv(int n) const {
        assert(n >= 0);
        assert(!empty() && (*this)[0] != mint(0));
        FPS res = {(*this)[0].inv()};
        while ((int)res.size() < n) {
            int next_size = min(2 * (int)res.size(), n);
            FPS f = pre(next_size);
            FPS g = res;
            g.resize(next_size);
            FPS fg = f * g;
            fg.resize(next_size);
            for (auto &x : fg) x = -x;
            fg[0] += mint(2);
            res = (g * fg).pre(next_size);
        }
        res.resize(n);
        return res;
    }

    // log f(x) mod x^n
    // f[0] = 1 が必要
    FPS log(int n) const {
        assert(n >= 0);
        assert(!empty() && (*this)[0] == mint(1));
        if (n == 0) return FPS();
        FPS res = (diff() * inv(n)).pre(n - 1).integral();
        res.resize(n);
        return res;
    }

    // exp f(x) mod x^n (Newton法)
    // f[0] = 0 が必要
    FPS exp(int n) const {
        assert(n >= 0);
        assert(empty() || (*this)[0] == mint(0));
        FPS res = {mint(1)};
        while ((int)res.size() < n) {
            int next_size = min(2 * (int)res.size(), n);
            FPS f = pre(next_size);
            FPS g = res.log(next_size);
            for (int i = 0; i < next_size; i++) {
                g[i] = (i < (int)f.size() ? f[i] : mint(0)) - g[i];
            }
            g[0] += mint(1);
            res = (res * g).pre(next_size);
        }
        res.resize(n);
        return res;
    }

    // f(x) の形式的平方根 mod x^n。存在しなければ nullopt。
    optional<FPS> sqrt(int n) const {
        assert(n >= 0);
        if (n == 0) return FPS();
        int first = 0;
        while (first < (int)size() && (*this)[first] == mint(0)) ++first;
        if (first == (int)size()) return FPS(n);
        if (first & 1) return nullopt;

        int shift = first / 2;
        if (shift >= n) return FPS(n);
        auto constant = modular_sqrt((*this)[first]);
        if (!constant) return nullopt;

        int required = n - shift;
        FPS normalized(required);
        for (int i = 0; i < required && first + i < (int)size(); ++i) {
            normalized[i] = (*this)[first + i];
        }

        FPS result = {*constant};
        const mint inverse_two = mint(2).inv();
        for (int length = 1; length < required; length <<= 1) {
            int next_length = min(2 * length, required);
            FPS quotient = (normalized.pre(next_length) * result.inv(next_length)).pre(next_length);
            result.resize(next_length);
            for (int i = 0; i < next_length; ++i) result[i] = (result[i] + quotient[i]) * inverse_two;
        }

        FPS answer(n);
        for (int i = 0; i < required; ++i) answer[shift + i] = result[i];
        return answer;
    }

    // 1点評価。O(N)。
    mint eval(mint x) const {
        mint result = 0;
        for (int i = (int)size() - 1; i >= 0; --i) result = result * x + (*this)[i];
        return result;
    }

    // 形式的級数として *this / denominator mod x^n を返す。
    FPS div_series(const FPS &denominator, int n) const {
        assert(n >= 0);
        assert(!denominator.empty() && denominator[0] != mint(0));
        return (pre(n) * denominator.inv(n)).pre(n);
    }

    // 多項式f(x+c)。O(M(N))。
    FPS taylor_shift(mint c) const {
        int n = size();
        if (n == 0) return FPS();
        const auto &inverse = inverse_table(n);
        vector<mint> factorial(n, 1), inverse_factorial(n, 1);
        for (int i = 1; i < n; ++i) {
            factorial[i] = factorial[i - 1] * i;
            inverse_factorial[i] = inverse_factorial[i - 1] * inverse[i];
        }
        FPS left(n), right(n);
        for (int i = 0; i < n; ++i) left[n - 1 - i] = (*this)[i] * factorial[i];
        mint power = 1;
        for (int i = 0; i < n; ++i) {
            right[i] = power * inverse_factorial[i];
            power *= c;
        }
        FPS product = left * right;
        FPS result(n);
        for (int i = 0; i < n; ++i) result[i] = product[n - 1 - i] * inverse_factorial[i];
        return result;
    }

    // 多項式としての商と余り。O(M(N))。
    pair<FPS, FPS> div_mod(FPS denominator) const {
        FPS numerator = *this;
        numerator.shrink();
        denominator.shrink();
        assert(!denominator.empty());
        if (numerator.size() < denominator.size()) return {FPS(), numerator};

        int quotient_size = numerator.size() - denominator.size() + 1;
        FPS reversed_numerator = numerator;
        FPS reversed_denominator = denominator;
        reverse(reversed_numerator.begin(), reversed_numerator.end());
        reverse(reversed_denominator.begin(), reversed_denominator.end());
        FPS quotient =
            (reversed_numerator.pre(quotient_size) * reversed_denominator.inv(quotient_size)).pre(quotient_size);
        reverse(quotient.begin(), quotient.end());
        FPS remainder = numerator - denominator * quotient;
        remainder.resize(min<int>(remainder.size(), denominator.size() - 1));
        remainder.shrink();
        return {quotient, remainder};
    }

    // f(x)^m mod x^n
    // a_0 = 1 の場合: exp(m * log(f))
    // a_0 != 1 の場合: x^k を括り出して計算
    FPS pow(long long m, int n) const {
        assert(m >= 0 && n >= 0);
        if (m == 0) {
            FPS res(n);
            if (n > 0) res[0] = mint(1);
            return res;
        }

        // 最初の非零項を見つける
        int k = 0;
        while (k < (int)size() && (*this)[k] == mint(0)) k++;

        // 全部 0
        if (k == (int)size()) return FPS(n);

        // x^k を括り出すと x^{k*m} になる
        // k*m >= n なら結果は 0
        if (k > 0 && (m >= (n + k - 1) / k || k * m >= n)) {
            return FPS(n);
        }

        long long km = k * m;
        int new_n = n - km;

        // f(x) = x^k * g(x) where g[0] != 0
        // f(x)^m = x^{km} * g(x)^m
        // g(x)^m = g[0]^m * (g(x)/g[0])^m = g[0]^m * exp(m * log(g(x)/g[0]))

        mint a0 = (*this)[k];
        mint a0_inv = a0.inv();
        mint a0_m = a0.pow(m);

        // h(x) = g(x) / g[0]  (h[0] = 1)
        FPS h(new_n);
        for (int i = 0; i < new_n && k + i < (int)size(); i++) {
            h[i] = (*this)[k + i] * a0_inv;
        }

        // h(x)^m = exp(m * log(h))
        // m が大きいので m mod (MOD-1) ではなく、log の係数に m をかける
        FPS log_h = h.log(new_n);
        mint m_mod = mint(m);
        for (auto &x : log_h) x *= m_mod;
        FPS h_m = log_h.exp(new_n);

        // g(x)^m = a0^m * h(x)^m
        for (auto &x : h_m) x *= a0_m;

        // f(x)^m = x^{km} * g(x)^m
        FPS res(n);
        for (int i = 0; i < new_n; i++) {
            res[km + i] = h_m[i];
        }
        return res;
    }
};

int main() {
    ll N,T;
    input(N,T);
    vector<ll> S(N);
    input(S);
    FPS F(T+1);

    vector<ll> cnt(T+1);
    for(auto x:S){
        cnt[x]++;
    }

    vector<mint> inv(T+1);
    rep(i,1,T+1){
        inv[i] = mint(i).inv();
    }

    rep(i,1,T+1){
        ll sign = 1;
        for(ll j = 1;j*i<= T;j++){
            F[i*j] += cnt[i] * sign * inv[j];
            sign *= -1;
        }
    }

    auto res = F.exp(T+1);
    vector<mint> ans(T+1);
    rep(i,T+1){
        ans[i] = res[i];
    }

    print_range(ans,1,T+1);
}
