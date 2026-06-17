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
// ACL: modint
// https://github.com/atcoder/ac-library
// clang-format off
#ifndef ATCODER
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
using namespace atcoder;
template <int m>
std::istream &operator>>(std::istream &is, static_modint<m> &x) {
    long long val;
    is >> val;
    x = val;
    return is;
}
template <int m>
std::ostream &operator<<(std::ostream &os, const static_modint<m> &x) {
    return os << x.val();
}
template <int id>
std::istream &operator>>(std::istream &is, dynamic_modint<id> &x) {
    long long val;
    is >> val;
    x = val;
    return is;
}
template <int id>
std::ostream &operator<<(std::ostream &os, const dynamic_modint<id> &x) {
    return os << x.val();
}
#endif  // ATCODER

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
[[maybe_unused]] constexpr ll MOD = 1e9 + 7;
//[[maybe_unused]] constexpr ll MOD = 998244353;
[[maybe_unused]] constexpr int INF32 = (1 << 30) - 1;
[[maybe_unused]] constexpr ll INF64 = (1LL << 62) - 1;
[[maybe_unused]] const double PI = numbers::pi;

// clang-format on
using mint = modint1000000007;
int main() {
    int N;
    input(N);
    vector<int> A(N);
    input(A);

    vector<int> pre(N+1);
    vector<int> B(N+1);
    rep(i,N){
        pre[i+1] = B[A[i]];
        B[A[i]] = i+1;
    }

    auto dp = make_vec<mint>(N+1,N+1,mint(0));
    dp[0][0] = 1;

    rep(i,N){
        rep(j,N){
            int p = pre[i+1];
            dp[i+1][j+1] += dp[i][j];
            if (p > 0){
                dp[i+1][j+1] -= dp[p-1][j];
            }
        }
        rep(j,N+1){
            dp[i+1][j] += dp[i][j];
        }
    }

    rep(i,1,N+1){
        print(dp[N][i]);
    }
}