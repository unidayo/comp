// #pragma GCC target("avx2")
// #pragma GCC optimize("O3")

// clang-format off
#include <bits/stdc++.h>

#ifdef AATCODER
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
using namespace atcoder;
#endif

#ifdef AATCODER
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
using vll = __int128_t;
using uvll = __uint128_t;
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

#ifdef AATCODER
// modint出力
template <int m>
istream &operator>>(istream &is, static_modint<m> &x) {
    ll val;
    is >> val;
    x = val;
    return is;
}
// modint入力
template <int m>
ostream &operator<<(ostream &os, const static_modint<m> &x) {
    os << x.val();
    return os;
}
#endif
// pair出力
template <class T1, class T2>
ostream &operator<<(ostream &os, const pair<T1, T2> &pr) {
    os << pr.first << ' ' << pr.second;
    return os;
}
// pair入力
template <class T1, class T2>
istream &operator>>(istream &is, pair<T1, T2> &pr) {
    is >> pr.first >> pr.second;
    return is;
}

// tuple出力
template <class Tuple, size_t... I>
void print_tuple_impl(ostream &os, const Tuple &t, index_sequence<I...>) {
    ((os << (I == 0 ? "" : " ") << get<I>(t)), ...);
}
template <class... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &t) {
    print_tuple_impl(os, t, index_sequence_for<Ts...>{});
    return os;
}
// tuple入力
template <class Tuple, size_t... I>
void read_tuple_impl(istream &is, Tuple &t, index_sequence<I...>) {
    ((is >> get<I>(t)), ...);
}
template <class... Ts>
istream &operator>>(istream &is, tuple<Ts...> &t) {
    read_tuple_impl(is, t, index_sequence_for<Ts...>{});
    return is;
}
// 128bit整数型出力
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
// 128bit整数型入力
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
// コンテナ入力
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
// コンテナ出力
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

template <class... T>
void input(T &...a) {
    (cin >> ... >> a);
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
ll powll(ll x, ll n) {
    ll ret = 1;
    while (n) {
        if (n & 1) ret = x * ret;
        x *= x;
        n >>= 1;
    }
    return ret;
}
template <class T>
T divceil(T x, T y) {
    auto d = x / y, r = x % y;
    return d + (r != 0 && ((r ^ y) > 0));
}
template <class T>
T divfloor(T x, T y) {
    auto d = x / y, r = x % y;
    return d - (r != 0 && ((r ^ y) < 0));
}
template <class T1, class T2>
T1 smod(T1 x, T2 m) {
    if (x < 0) return (x % m + m) % m;
    return x % m;
}
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
// m以下の最大の整数xで x ≡ q (mod p) を満たすものを返す
ll nearest_px_plus_q_le(ll m, ll p, ll q) {
    return m - smod(m - q, p);
}
// m以上の最小の整数xで x ≡ q (mod p) を満たすものを返す
ll nearest_px_plus_q_ge(ll m, ll p, ll q) {
    return m + (p - smod(m - q, p)) % p;
}
// 2点(x1, y1), (x2, y2)を通る直線の方程式ax+by+c=0を求める
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
// 3点(x1, y1), (x2, y2), (x3, y3)が同一直線状にあるか判定する
bool collinear(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3) {
    vll cross = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    return cross == 0;
}

/* algorithms */
template <RAR Range>
ll lower_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::lower_bound(r, x));
}
template <RAR Range>
ll upper_count(const Range &r, const rvt<Range> &x) {
    return ranges::distance(ranges::begin(r), ranges::upper_bound(r, x));
}
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

/**
 * @brief Run-Length Encoding（RLE）を行う関数
 * @tparam Range  ランダムアクセス可能な Range（vector, string, array など）
 * @param R       入力 Range
 * @return vector<pair<rvt<Range>, long long>>
 *         (値, 連続出現回数) を格納したベクタを返す。
 * @details
 * 隣接する同値要素をまとめて (値, 個数) の形で列挙する。
 * 例: [a, a, b, b, b, c] → {(a,2), (b,3), (c,1)}
 */
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
template <RAR Range>
void remove_duplicates(Range &v) {
    ranges::sort(v);
    auto res = ranges::unique(v);
    v.erase(ranges::begin(res), ranges::end(res));
}
template <ranges::range Range>
auto sum_of(const Range &R) {
    return reduce(ranges::begin(R), ranges::end(R));
}
template <ranges::range Range, class T>
T sum_of(const Range &R) {
    return reduce(ranges::begin(R), ranges::end(R), T());
}

/* data_structure */
template <class T>
using priority_queue_min = priority_queue<T, vector<T>, greater<T>>;
template <class T>
using priority_queue_max = priority_queue<T, vector<T>>;
template <class T>
auto make_vec(size_t n, const T &val) {
    return vector<T>(n, val);
}
template <class T, class... Args>
auto make_vec(size_t n, Args... args) {
    return vector<decltype(make_vec<T>(args...))>(n, make_vec<T>(args...));
}

/* string */
string dec_to_base(ull x, int base) {
    auto b = begin(cp_internal::char_buff);
    auto [ptr, ec] = to_chars(b, end(cp_internal::char_buff), x, base);
    return string(b, ptr);
}
ull base_to_dec(const string &x, int base) { return stoull(x, 0, base); }
void zfill(string &s, size_t width) { s = string(width > s.size() ? width - s.size() : 0, '0') + s; }

/* grid */
[[maybe_unused]] constexpr int dx[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dy[] = {1, 0, -1, 0};
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

/* bit */
int popcnt(ull x) { return popcount(x); }
int msb(ull x) { return 63 - countl_zero(x); }
int lsb(ull x) { return countr_zero(x); }

/* random */
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
    // 32bit乱数を1つ生成 (RNG1回呼び出し)
    uint operator()() {
        uint t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
    // 64bit乱数を1つ生成 (RNG2回呼び出し)
    ull rand64() {
        ull high = ull((*this)()) << 32;
        ull low = ull((*this)());
        return high | low;
    }
    // 完全一様な [l, r] の32bit整数乱数 (RNG1回呼び出し)
    // modulo biasを除去するためのrejection sampling
    uint rand_range(uint l, uint r) {
        uint range = r - l + 1;
        // 2^32 を超えない最大の「range の倍数」
        uint limit = ~uint(0) - (~uint(0) % range);
        while (true) {
            uint x = (*this)();
            if (x <= limit) {
                return l + (x % range);
            }
        }
    }
    // 完全一様な [l, r] の64bit整数乱数 (RNG2回呼び出し)
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
// ACL: segtree
// https://github.com/atcoder/ac-library
// clang-format off
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
// ACL: modint
// https://github.com/atcoder/ac-library
// clang-format off
#include <cassert>
#include <numeric>
#include <type_traits>
#ifdef _MSC_VER
#include <intrin.h>
#endif
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
#include <cassert>
#include <numeric>
#include <type_traits>
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
template <class T>
class Treap {
   private:
    mt19937 rnd;
    struct Node {
        T key;
        Node *l, *r;
        uint priority;
        int cnt;
        Node(T key, uint priority) : key(key), l(nullptr), r(nullptr), priority(priority), cnt(1) {};
    } *root = nullptr;
    using Tree = Node *;

    int count(const Tree t) { return !t ? 0 : t->cnt; }
    Tree update(Tree t) {
        t->cnt = count(t->l) + count(t->r) + 1;
        return t;
    }
    bool find(const Tree &t, T key) {
        if (!t) {
            return false;
        } else if (t->key == key) {
            return true;
        } else {
            return find(key < t->key ? t->l : t->r, key);
        }
    }

    Tree merge(Tree l, Tree r) {
        if (!l || !r) {
            return l ? l : r;
        }
        if (l->priority > r->priority) {
            l->r = merge(l->r, r);
            return update(l);
        } else {
            r->l = merge(l, r->l);
            return update(r);
        }
    }
    pair<Tree, Tree> split(Tree &t, int k) {
        if (!t) {
            return {nullptr, nullptr};
        }
        if (k <= count(t->l)) {
            auto s = split(t->l, k);
            t->l = s.second;
            return {s.first, update(t)};
        } else {
            auto s = split(t->r, k - count(t->l) - 1);
            t->r = s.first;
            return {update(t), s.second};
        }
        return {nullptr, nullptr};
    }

    Tree insert(Tree &t, Tree item) {
        if (!t) {
            t = item;
            return t;
        } else {
            auto s = split(t, lower_bound(t, item->key));
            return merge(merge(s.first, item), s.second);
        }
    }

    ll lower_bound(Tree &t, T key) {
        if (!t) return 0;
        if (key <= t->key) {
            return lower_bound(t->l, key);
        } else {
            return count(t->l) + 1 + lower_bound(t->r, key);
        }
    }

    ll upper_bound(Tree &t, T key) {
        if (!t) return 0;
        if (key < t->key) {
            return upper_bound(t->l, key);
        } else {
            return count(t->l) + 1 + upper_bound(t->r, key);
        }
    }

    optional<T> prev(Tree &t, T x) {
        if (!t) return nullopt;
        if (t->key < x) {
            auto res = prev(t->r, x);
            return res ? res : t->key;
        } else {
            return prev(t->l, x);
        }
    }
    optional<T> next(Tree &t, T x) {
        if (!t) return nullopt;
        if (t->key > x) {
            auto res = next(t->l, x);
            return res ? res : t->key;
        } else {
            return next(t->r, x);
        }
    }

    T min(const Tree &t) { return t->l ? min(t->l) : t->key; }
    T max(const Tree &t) { return t->r ? max(t->r) : t->key; }

    T kth(Tree &t, int k) {
        if (count(t->l) > k) {
            return kth(t->l, k);
        } else if (count(t->l) == k) {
            return t->key;
        } else {
            return kth(t->r, k - count(t->l) - 1);
        }
    }

    void destroy(Node *t) {
        if (!t) return;
        destroy(t->l);
        destroy(t->r);
        delete t;
    }

   public:
    Treap() : rnd(random_device{}()) {};
    ~Treap() { destroy(root); }
    // key を1つ挿入する
    void insert(T key) { root = insert(root, new Node(key, rnd())); }
    // key が存在するか判定する
    bool find(T key) { return find(root, key); }
    // 最小値を返す
    T min() { return min(root); }
    // 最大値を返す
    T max() { return max(root); }
    // key 未満の最大の要素を返す（存在しない場合は未定義動作）
    T prev(T key) { return prev(root, key).value(); }
    // key より大きい最小の要素を返す（存在しない場合は未定義動作）
    T next(T key) { return next(root, key).value(); }
    // key 未満の要素の個数（= key 以上の要素が始まるインデックス）を返す
    ll lower_bound(T key) { return lower_bound(root, key); }
    // key 以下の要素の個数（= key より大きい要素が始まるインデックス）を返す
    ll upper_bound(T key) { return upper_bound(root, key); }
    // key に等しい要素の [lower_bound, upper_bound) を返す
    pair<int, int> equal_range(T key) { return {lower_bound(key), upper_bound(key)}; }
    // key 未満の要素からなる木と key 以上の要素からなる木に分割する（root は破壊される）
    pair<Tree, Tree> split(T key) { return split(root, lower_bound(key)); }
    // key を1つ削除する。存在しなければ false を返す
    bool erase(T key) {
        int li = lower_bound(key);
        int ri = upper_bound(key);
        if (li == ri) return false;
        auto s1 = split(root, li);
        auto s2 = split(s1.second, 1);
        delete s2.first;
        root = merge(s1.first, s2.second);
        return true;
    }
    // 昇順 k 番目（0-indexed）の要素を返す
    T kth(int k) { return kth(root, k); }
    // 要素数を返す
    int size() { return root ? root->cnt : 0; }
};

ll op(ll x, ll y) { return x + y; }
ll e() { return 0; }
using mint = atcoder::modint998244353;
int main() {
    int M;
    input(M);
    vector<ll> A(M);
    input(A);
    auto comp = compression(A);
    vector<ll> B(M);
    rep(i, M) { B[i] = comp[A[i]]; }
    atcoder::segtree<ll, op, e> seg(M);
    Treap<ll> st;
    rep(i, 2) {
        seg.set(B[i], A[i]);
        st.insert(B[i]);
    }

    auto calc_L = [&](int k, int tot) -> ll {
        ll pos = st.kth(k);
        ll x = seg.get(pos);
        return x * k - seg.prod(0, pos);
    };

    auto calc_R = [&](int k, int tot) -> ll {
        ll pos = st.kth(k);
        ll x = seg.get(pos);
        return seg.prod(pos+1,M) - x * (tot - k - 1);
    };

    rep(i, 2, M) {
        seg.set(B[i], A[i]);
        st.insert(B[i]);
        ll ok = 0, ng = i;
        while (ng - ok > 1) {
            ll mid = (ok + ng) / 2;
            ll sr = (mid + 1 <= i ? calc_R(mid+1, i + 1) : 0);
            ll sl = (mid - 1 >= 0 ? calc_L(mid - 1, i + 1) : 0);
            (sr - sl >= 0 ? ok : ng) = mid;
        }
        ll val1 = max((ok + 1 <= i ? calc_R(ok+1, i + 1) : 0), (ok - 1 >= 0 ? calc_L(ok - 1, i + 1) : 0));
        ll val2 = max((ok + 2 <= i ? calc_R(ok + 2, i + 1) : 0), (ok >= 0 ? calc_L(ok, i + 1) : 0));
        // print(i,ok,val1,val2);
        mint res = min(val1,val2);
        res *= mint(i - 1).inv();
        print(res.val());
    }
}