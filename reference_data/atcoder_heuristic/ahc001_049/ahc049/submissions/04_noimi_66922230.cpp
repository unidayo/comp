#pragma region Macros
#ifdef noimi
#pragma comment(linker, "/stack:256000000")
#else
#pragma GCC target("avx2")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")

#include <immintrin.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cfenv>
#include <cfloat>
#include <chrono>
#include <cinttypes>
#include <climits>
#include <cmath>
#include <complex>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <immintrin.h>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <streambuf>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#define oj_local(a, b) (a)

#define LOCAL if(oj_local(0, 1))
#define OJ if(oj_local(1, 0))

using namespace std;
using ll = long long;
using ull = unsigned long long int;
using i128 = __int128_t;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using ld = long double;
template <typename T> using vc = vector<T>;
template <typename T> using vvc = vector<vc<T>>;
template <typename T> using vvvc = vector<vvc<T>>;
using vi = vc<int>;
using vl = vc<ll>;
using vpi = vc<pii>;
using vpl = vc<pll>;
template <class T> using pq = priority_queue<T>;
template <class T> using pqg = priority_queue<T, vector<T>, greater<T>>;
template <typename T> int si(const T &x) { return x.size(); }
template <class T, class S> inline bool chmax(T &a, const S &b) { return (a < b ? a = b, 1 : 0); }
template <class T, class S> inline bool chmin(T &a, const S &b) { return (a > b ? a = b, 1 : 0); }
vi iota(int n) {
    vi a(n);
    return iota(a.begin(), a.end(), 0), a;
}
template <typename T> vi iota(const vector<T> &a, bool greater = false) {
    vi res(a.size());
    iota(res.begin(), res.end(), 0);
    sort(res.begin(), res.end(), [&](int i, int j) {
        if(greater) return a[i] > a[j];
        return a[i] < a[j];
    });
    return res;
}

// macros
#define overload5(a, b, c, d, e, name, ...) name
#define overload4(a, b, c, d, name, ...) name
#define endl '\n'
#define REP0(n) for(ll jidlsjf = 0; jidlsjf < n; ++jidlsjf)
#define REP1(i, n) for(ll i = 0; i < (n); ++i)
#define REP2(i, a, b) for(ll i = (a); i < (b); ++i)
#define REP3(i, a, b, c) for(ll i = (a); i < (b); i += (c))
#define rep(...) overload4(__VA_ARGS__, REP3, REP2, REP1, REP0)(__VA_ARGS__)
#define per0(n) for(int jidlsjf = 0; jidlsjf < (n); ++jidlsjf)
#define per1(i, n) for(ll i = (n) - 1; i >= 0; --i)
#define per2(i, a, b) for(ll i = (a) - 1; i >= b; --i)
#define per3(i, a, b, c) for(ll i = (a) - 1; i >= (b); i -= (c))
#define per(...) overload4(__VA_ARGS__, per3, per2, per1, per0)(__VA_ARGS__)
#define fore0(a) rep(a.size())
#define fore1(i, a) for(auto &&i : a)
#define fore2(a, b, v) for(auto &&[a, b] : v)
#define fore3(a, b, c, v) for(auto &&[a, b, c] : v)
#define fore4(a, b, c, d, v) for(auto &&[a, b, c, d] : v)
#define fore(...) overload5(__VA_ARGS__, fore4, fore3, fore2, fore1, fore0)(__VA_ARGS__)
#define setbits(j, n) for(ll iiiii = (n), j = lowbit(iiiii); iiiii; iiiii ^= 1 << j, j = lowbit(iiiii))
#define perm(v) for(bool flag = true; (flag ? exchange(flag, false) : next_permutation(all(v)));)
#define fi first
#define se second
#define pb push_back
#define ppb pop_back
#define ppf pop_front
#define eb emplace_back
#define drop(s) cout << #s << endl, exit(0)
#define si(c) (int)(c).size()
#define lb(c, x) distance((c).begin(), lower_bound(all(c), (x)))
#define lbg(c, x) distance((c).begin(), lower_bound(all(c), (x), greater{}))
#define ub(c, x) distance((c).begin(), upper_bound(all(c), (x)))
#define ubg(c, x) distance((c).begin(), upper_bound(all(c), (x), greater{}))
#define rng(v, l, r) v.begin() + (l), v.begin() + (r)
#define all(c) begin(c), end(c)
#define rall(c) rbegin(c), rend(c)
#define SORT(v) sort(all(v))
#define REV(v) reverse(all(v))
#define UNIQUE(x) SORT(x), x.erase(unique(all(x)), x.end())
template <typename T = ll, typename S> T SUM(const S &v) { return accumulate(all(v), T(0)); }
#define MIN(v) *min_element(all(v))
#define MAX(v) *max_element(all(v))
#define overload2(_1, _2, name, ...) name
#define vec(type, name, ...) vector<type> name(__VA_ARGS__)
#define vv(type, name, h, ...) vector<vector<type>> name(h, vector<type>(__VA_ARGS__))
#define vvv(type, name, h, w, ...) vector<vector<vector<type>>> name(h, vector<vector<type>>(w, vector<type>(__VA_ARGS__)))
#define vvvv(type, name, a, b, c, ...)                                                                                                                         \
    vector<vector<vector<vector<type>>>> name(a, vector<vector<vector<type>>>(b, vector<vector<type>>(c, vector<type>(__VA_ARGS__))))
constexpr pii dx4[4] = {pii{1, 0}, pii{0, 1}, pii{-1, 0}, pii{0, -1}};
constexpr pii dx8[8] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};

namespace yesno_impl {
const string YESNO[2] = {"NO", "YES"};
const string YesNo[2] = {"No", "Yes"};
const string yesno[2] = {"no", "yes"};
const string firstsecond[2] = {"second", "first"};
const string FirstSecond[2] = {"Second", "First"};
const string possiblestr[2] = {"impossible", "possible"};
const string Possiblestr[2] = {"Impossible", "Possible"};
void YES(bool t = 1) { cout << YESNO[t] << endl; }
void NO(bool t = 1) { YES(!t); }
void Yes(bool t = 1) { cout << YesNo[t] << endl; }
void No(bool t = 1) { Yes(!t); }
void yes(bool t = 1) { cout << yesno[t] << endl; }
void no(bool t = 1) { yes(!t); }
void first(bool t = 1) { cout << firstsecond[t] << endl; }
void First(bool t = 1) { cout << FirstSecond[t] << endl; }
void possible(bool t = 1) { cout << possiblestr[t] << endl; }
void Possible(bool t = 1) { cout << Possiblestr[t] << endl; }
}; // namespace yesno_impl
using namespace yesno_impl;
#define INT(...)                                                                                                                                               \
    int __VA_ARGS__;                                                                                                                                           \
    IN(__VA_ARGS__)
#define INTd(...)                                                                                                                                              \
    int __VA_ARGS__;                                                                                                                                           \
    IN2(__VA_ARGS__)
#define LL(...)                                                                                                                                                \
    ll __VA_ARGS__;                                                                                                                                            \
    IN(__VA_ARGS__)
#define LLd(...)                                                                                                                                               \
    ll __VA_ARGS__;                                                                                                                                            \
    IN2(__VA_ARGS__)
#define STR(...)                                                                                                                                               \
    string __VA_ARGS__;                                                                                                                                        \
    IN(__VA_ARGS__)
#define CHR(...)                                                                                                                                               \
    char __VA_ARGS__;                                                                                                                                          \
    IN(__VA_ARGS__)
#define DBL(...)                                                                                                                                               \
    double __VA_ARGS__;                                                                                                                                        \
    IN(__VA_ARGS__)
#define VEC(type, name, size)                                                                                                                                  \
    vector<type> name(size);                                                                                                                                   \
    IN(name)
#define VECd(type, name, size)                                                                                                                                 \
    vector<type> name(size);                                                                                                                                   \
    IN2(name)
#define VEC2(type, name1, name2, size)                                                                                                                         \
    vector<type> name1(size), name2(size);                                                                                                                     \
    for(int i = 0; i < size; i++) IN(name1[i], name2[i])
#define VEC2d(type, name1, name2, size)                                                                                                                        \
    vector<type> name1(size), name2(size);                                                                                                                     \
    for(int i = 0; i < size; i++) IN2(name1[i], name2[i])
#define VEC3(type, name1, name2, name3, size)                                                                                                                  \
    vector<type> name1(size), name2(size), name3(size);                                                                                                        \
    for(int i = 0; i < size; i++) IN(name1[i], name2[i], name3[i])
#define VEC3d(type, name1, name2, name3, size)                                                                                                                 \
    vector<type> name1(size), name2(size), name3(size);                                                                                                        \
    for(int i = 0; i < size; i++) IN2(name1[i], name2[i], name3[i])
#define VEC4(type, name1, name2, name3, name4, size)                                                                                                           \
    vector<type> name1(size), name2(size), name3(size), name4(size);                                                                                           \
    for(int i = 0; i < size; i++) IN(name1[i], name2[i], name3[i], name4[i]);
#define VEC4d(type, name1, name2, name3, name4, size)                                                                                                          \
    vector<type> name1(size), name2(size), name3(size), name4(size);                                                                                           \
    for(int i = 0; i < size; i++) IN2(name1[i], name2[i], name3[i], name4[i]);
#define VV(type, name, h, w)                                                                                                                                   \
    vector<vector<type>> name(h, vector<type>(w));                                                                                                             \
    IN(name)
#define VVd(type, name, h, w)                                                                                                                                  \
    vector<vector<type>> name(h, vector<type>(w));                                                                                                             \
    IN2(name)
int scan() { return getchar(); }
void scan(int &a) { cin >> a; }
void scan(long long &a) { cin >> a; }
void scan(char &a) { cin >> a; }
void scan(double &a) { cin >> a; }
void scan(string &a) { cin >> a; }
template <class T, class S> void scan(pair<T, S> &p) { scan(p.first), scan(p.second); }
template <class T> void scan(vector<T> &);
template <class T> void scan(vector<T> &a) {
    for(auto &i : a) scan(i);
}
template <class T> void scan(T &a) { cin >> a; }
void IN() {}
void IN2() {}
template <class Head, class... Tail> void IN(Head &head, Tail &...tail) {
    scan(head);
    IN(tail...);
}
template <class Head, class... Tail> void IN2(Head &head, Tail &...tail) {
    scan(head);
    --head;
    IN2(tail...);
}

template <int p = -1> void pat() {}
template <int p = -1, class Head, class... Tail> void pat(Head &h, Tail &...tail) {
    h += p;
    pat<p>(tail...);
}

template <typename T, typename S> T ceil(T x, S y) {
    assert(y);
    return (y < 0 ? ceil(-x, -y) : (x > 0 ? (x + y - 1) / y : x / y));
}

template <typename T, typename S> T floor(T x, S y) {
    assert(y);
    return (y < 0 ? floor(-x, -y) : (x > 0 ? x / y : x / y - (x % y == 0 ? 0 : 1)));
}

template <typename T, typename S, typename U> U bigmul(const T &x, const S &y, const U &lim) { // clamp(x * y, -lim, lim)
    if(x < 0 and y < 0) return bigmul(-x, -y, lim);
    if(x < 0) return -bigmul(-x, y, lim);
    if(y < 0) return -bigmul(x, -y, lim);
    return y == 0 or x <= lim / y ? x * y : lim;
}

template <class T> T POW(T x, int n) {
    T res = 1;
    for(; n; n >>= 1, x *= x)
        if(n & 1) res *= x;
    return res;
}
template <class T, class S> T POW(T x, S n, const ll &mod) {
    T res = 1;
    x %= mod;
    for(; n; n >>= 1, x = x * x % mod)
        if(n & 1) res = res * x % mod;
    return res;
}
vector<pll> factor(ll x) {
    vector<pll> ans;
    for(ll i = 2; i * i <= x; i++)
        if(x % i == 0) {
            ans.push_back({i, 1});
            while((x /= i) % i == 0) ans.back().second++;
        }
    if(x != 1) ans.push_back({x, 1});
    return ans;
}
template <class T> vector<T> divisor(T x) {
    vector<T> ans;
    for(T i = 1; i * i <= x; i++)
        if(x % i == 0) {
            ans.pb(i);
            if(i * i != x) ans.pb(x / i);
        }
    return ans;
}
template <typename T> void zip(vector<T> &x) {
    vector<T> y = x;
    UNIQUE(y);
    for(int i = 0; i < x.size(); ++i) { x[i] = lb(y, x[i]); }
}
template <class S> void fold_in(vector<S> &v) {}
template <typename Head, typename... Tail, class S> void fold_in(vector<S> &v, Head &&a, Tail &&...tail) {
    for(auto e : a) v.emplace_back(e);
    fold_in(v, tail...);
}
template <class S> void renumber(vector<S> &v) {}
template <typename Head, typename... Tail, class S> void renumber(vector<S> &v, Head &&a, Tail &&...tail) {
    for(auto &&e : a) e = lb(v, e);
    renumber(v, tail...);
}
template <class S, class... Args> vector<S> zip(vector<S> &head, Args &&...args) {
    vector<S> v;
    fold_in(v, head, args...);
    sort(all(v)), v.erase(unique(all(v)), v.end());
    renumber(v, head, args...);
    return v;
}

template <typename S> void rearrange(const vector<S> &id) {}
template <typename S, typename T> void rearrange_exec(const vector<S> &id, vector<T> &v) {
    vector<T> w(v.size());
    rep(i, si(id)) w[i] = v[id[i]];
    v.swap(w);
}
template <typename S, typename Head, typename... Tail> void rearrange(const vector<S> &id, Head &a, Tail &...tail) {
    rearrange_exec(id, a);
    rearrange(id, tail...);
}

template <typename T> vector<T> RUI(const vector<T> &v) {
    vector<T> res(v.size() + 1);
    for(int i = 0; i < v.size(); i++) res[i + 1] = res[i] + v[i];
    return res;
}
template <typename T> void zeta_supersetsum(vector<T> &f) {
    int n = f.size();
    for(int i = 1; i < n; i <<= 1) rep(b, n) if(!(i & b)) f[b] += f[b | i];
}

template <typename T> void zeta_subsetsum(vector<T> &f) {
    int n = f.size();
    for(int i = 1; i < n; i <<= 1) rep(b, n) if(!(i & b)) f[b | i] += f[b];
}
template <typename T> void mobius_subset(vector<T> &f) {
    int n = f.size();
    for(int i = 1; i < n; i <<= 1) rep(b, n) if(!(i & b)) f[b] -= f[b | i];
}
template <typename T> void mobius_superset(vector<T> &f) {
    int n = f.size();
    for(int i = 1; i < n; i <<= 1) rep(b, n) if(!(i & b)) f[b | i] -= f[b];
}
// 反時計周りに 90 度回転
template <typename T> void rot(vector<vector<T>> &v) {
    if(empty(v)) return;
    int n = v.size(), m = v[0].size();
    vector<vector<T>> res(m, vector<T>(n));
    rep(i, n) rep(j, m) res[m - 1 - j][i] = v[i][j];
    v.swap(res);
}

template <typename T> void flip(vector<vector<T>> &v) {
    if(empty(v)) return;
    int n = v.size(), m = si(v[0]);
    vector<vector<T>> nv(m, vector<T>(n));
    rep(i, n) rep(j, m) nv[j][i] = v[i][j];
    swap(v, nv);
}
vector<int> counter(const vector<int> &v, int max_num = -1) {
    if(max_num == -1) max_num = MAX(v);
    vector<int> res(max_num + 1);
    fore(e, v) res[e]++;
    return res;
}

// x in [l, r)
template <class T, class S> bool inc(const T &x, const S &l, const S &r) { return l <= x and x < r; }
template <class T, class S> bool inc(const T &x, const pair<S, S> &p) { return p.first <= x and x < p.second; }

// 便利関数
constexpr ll ten(int n) { return n == 0 ? 1 : ten(n - 1) * 10; }
constexpr ll tri(ll n) { return n * (n + 1) / 2; }
// l + ... + r
constexpr ll tri(ll l, ll r) { return (l + r) * (r - l + 1) / 2; }
ll max(int x, ll y) { return max((ll)x, y); }
ll max(ll x, int y) { return max(x, (ll)y); }
int min(int x, ll y) { return min((ll)x, y); }
int min(ll x, int y) { return min(x, (ll)y); }
// bit 演算系
#define bit(i) (1LL << i)       // (1 << i)
#define test(b, i) (b >> i & 1) // b の i bit 目が立っているか
ll pow2(int i) { return 1LL << i; }
int topbit(signed t) { return t == 0 ? -1 : 31 - __builtin_clz(t); }
int topbit(ll t) { return t == 0 ? -1 : 63 - __builtin_clzll(t); }
int lowbit(signed a) { return a == 0 ? 32 : __builtin_ctz(a); }
int lowbit(ll a) { return a == 0 ? 64 : __builtin_ctzll(a); }
// int allbit(int n) { return (1 << n) - 1; }
constexpr ll mask(int n) { return (1LL << n) - 1; }
// int popcount(signed t) { return __builtin_popcount(t); }
// int popcount(ll t) { return __builtin_popcountll(t); }
int popcount(uint64_t t) { return __builtin_popcountll(t); }
static inline uint64_t popcount64(uint64_t x) {
    uint64_t m1 = 0x5555555555555555ll;
    uint64_t m2 = 0x3333333333333333ll;
    uint64_t m4 = 0x0F0F0F0F0F0F0F0Fll;
    uint64_t h01 = 0x0101010101010101ll;

    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;

    return (x * h01) >> 56;
}
bool ispow2(int i) { return i && (i & -i) == i; }

ll rnd(ll l, ll r) { //[l, r)
#ifdef noimi
    static mt19937_64 gen;
#else
    static mt19937_64 gen(chrono::steady_clock::now().time_since_epoch().count());
#endif
    return uniform_int_distribution<ll>(l, r - 1)(gen);
}
ll rnd(ll n) { return rnd(0, n); }

template <class t> void random_shuffle(vc<t> &a) { rep(i, si(a)) swap(a[i], a[rnd(0, i + 1)]); }

int in() {
    int x;
    cin >> x;
    return x;
}
ll lin() {
    unsigned long long x;
    cin >> x;
    return x;
}

template <class T, class S> pair<T, S> operator-(const pair<T, S> &x) { return pair<T, S>(-x.first, -x.second); }
template <class T, class S> pair<T, S> operator-(const pair<T, S> &x, const pair<T, S> &y) { return pair<T, S>(x.fi - y.fi, x.se - y.se); }
template <class T, class S> pair<T, S> operator+(const pair<T, S> &x, const pair<T, S> &y) { return pair<T, S>(x.fi + y.fi, x.se + y.se); }
template <class T> pair<T, T> operator&(const pair<T, T> &l, const pair<T, T> &r) { return pair<T, T>(max(l.fi, r.fi), min(l.se, r.se)); }
template <class T, class S> pair<T, S> operator+=(pair<T, S> &l, const pair<T, S> &r) { return l = l + r; }
template <class T, class S> pair<T, S> operator-=(pair<T, S> &l, const pair<T, S> &r) { return l = l - r; }
template <class T> bool intersect(const pair<T, T> &l, const pair<T, T> &r) { return (l.se < r.se ? r.fi < l.se : l.fi < r.se); }

template <class T> vector<T> &operator++(vector<T> &v) {
    fore(e, v) e++;
    return v;
}
template <class T> vector<T> operator++(vector<T> &v, int) {
    auto res = v;
    fore(e, v) e++;
    return res;
}
template <class T> vector<T> &operator--(vector<T> &v) {
    fore(e, v) e--;
    return v;
}
template <class T> vector<T> operator--(vector<T> &v, int) {
    auto res = v;
    fore(e, v) e--;
    return res;
}
template <class T> void connect(vector<T> &l, const vector<T> &r) { fore(e, r) l.eb(e); }
template <class T> vector<T> operator+(const vector<T> &l, const vector<T> &r) {
    vector<T> res(max(si(l), si(r)));
    rep(i, si(l)) res[i] += l[i];
    rep(i, si(r)) res[i] += r[i];
    return res;
}
template <class T> vector<T> operator-(const vector<T> &l, const vector<T> &r) {
    vector<T> res(max(si(l), si(r)));
    rep(i, si(l)) res[i] += l[i];
    rep(i, si(r)) res[i] -= r[i];
    return res;
}
template <class T> vector<T> &operator+=(vector<T> &l, const vector<T> &r) {
    if(si(l) < si(r)) l.resize(si(r));
    rep(i, si(r)) l[i] += r[i];
    return l;
}
template <class T> vector<T> &operator-=(vector<T> &l, const vector<T> &r) {
    if(si(l) < si(r)) l.resize(si(r));
    rep(i, si(r)) l[i] -= r[i];
    return l;
}
template <class T> vector<T> &operator+=(vector<T> &v, const T &x) {
    fore(e, v) e += x;
    return v;
}
template <class T> vector<T> &operator-=(vector<T> &v, const T &x) {
    fore(e, v) e -= x;
    return v;
}

template <typename T> struct edge {
    int from, to;
    T cost;
    int id;
    edge(int to, T cost) : from(-1), to(to), cost(cost) {}
    edge(int from, int to, T cost) : from(from), to(to), cost(cost) {}
    edge(int from, int to, T cost, int id) : from(from), to(to), cost(cost), id(id) {}
    constexpr bool operator<(const edge<T> &rhs) const noexcept { return cost < rhs.cost; }
    edge &operator=(const int &x) {
        to = x;
        return *this;
    }
    operator int() const { return to; }
    friend ostream operator<<(ostream &os, const edge &e) { return os << e.to; }
};
template <typename T> using Edges = vector<edge<T>>;

template <typename T = int> Edges<T> read_edges(int m, bool weighted = false) {
    Edges<T> res;
    res.reserve(m);
    for(int i = 0; i < m; i++) {
        int u, v, c = 0;
        scan(u), scan(v), u--, v--;
        if(weighted) scan(c);
        res.eb(u, v, c, i);
    }
    return res;
}

using Tree = vector<vector<int>>;
using Graph = vector<vector<int>>;
template <class T> using Wgraph = vector<vector<edge<T>>>;
Graph getG(int n, int m = -1, bool directed = false, int margin = 1) {
    Tree res(n);
    if(m == -1) m = n - 1;
    while(m--) {
        int a, b;
        cin >> a >> b;
        a -= margin, b -= margin;
        res[a].emplace_back(b);
        if(!directed) res[b].emplace_back(a);
    }
    return res;
}
Graph getTreeFromPar(int n, int margin = 1) {
    Graph res(n);
    for(int i = 1; i < n; i++) {
        int a;
        cin >> a;
        res[a - margin].emplace_back(i);
    }
    return res;
}
template <class T> Wgraph<T> getWg(int n, int m = -1, bool directed = false, int margin = 1) {
    Wgraph<T> res(n);
    if(m == -1) m = n - 1;
    while(m--) {
        int a, b;
        T c;
        scan(a), scan(b), scan(c);
        a -= margin, b -= margin;
        res[a].emplace_back(b, c);
        if(!directed) res[b].emplace_back(a, c);
    }
    return res;
}
void add(Graph &G, int x, int y) { G[x].eb(y), G[y].eb(x); }
template <class S, class T> void add(Wgraph<S> &G, int x, int y, T c) { G[x].eb(y, c), G[y].eb(x, c); }

#define TEST                                                                                                                                                   \
    INT(testcases);                                                                                                                                            \
    while(testcases--)

i128 abs(const i128 &x) { return x > 0 ? x : -x; }
istream &operator>>(istream &is, i128 &v) {
    string s;
    is >> s;
    v = 0;
    for(int i = 0; i < (int)s.size(); i++) {
        if(isdigit(s[i])) { v = v * 10 + s[i] - '0'; }
    }
    if(s[0] == '-') { v *= -1; }
    return is;
}

ostream &operator<<(ostream &os, const i128 &v) {
    if(v == 0) { return (os << "0"); }
    i128 num = v;
    if(v < 0) {
        os << '-';
        num = -num;
    }
    string s;
    for(; num > 0; num /= 10) { s.push_back((char)(num % 10) + '0'); }
    reverse(s.begin(), s.end());
    return (os << s);
}
template <class Ch, class Tr, class Container> std::basic_ostream<Ch, Tr> &operator<<(std::basic_ostream<Ch, Tr> &os, const Container &x);
template <typename T, typename S, typename U> std::ostream &operator<<(std::ostream &os, const priority_queue<T, S, U> &_pq);
template <typename... Ts> std::ostream &operator<<(std::ostream &os, const std::tuple<Ts...> &t);

namespace aux {
template <typename T, unsigned N, unsigned L> struct tp {
    static void output(std::ostream &os, const T &v) {
        os << std::get<N>(v) << (&os == &cerr ? ", " : " ");
        tp<T, N + 1, L>::output(os, v);
    }
};
template <typename T, unsigned N> struct tp<T, N, N> {
    static void output(std::ostream &os, const T &v) { os << std::get<N>(v); }
};
} // namespace aux
template <typename... Ts> std::ostream &operator<<(std::ostream &os, const std::tuple<Ts...> &t) {
    if(&os == &cerr) { os << '('; }
    aux::tp<std::tuple<Ts...>, 0, sizeof...(Ts) - 1>::output(os, t);
    if(&os == &cerr) { os << ')'; }
    return os;
}
template <typename T, typename S, typename U> std::ostream &operator<<(std::ostream &os, const priority_queue<T, S, U> &_pq) {
    auto pq = _pq;
    vector<T> res;
    while(!empty(pq)) res.emplace_back(pq.top()), pq.pop();
    return os << res;
}
template <class T, class S> ostream &operator<<(ostream &os, const pair<T, S> &p) {
    if(&os == &cerr) { return os << "(" << p.first << ", " << p.second << ")"; }
    return os << p.first << " " << p.second;
}
template <class Ch, class Tr, class Container> std::basic_ostream<Ch, Tr> &operator<<(std::basic_ostream<Ch, Tr> &os, const Container &x) {
    bool f = true;
    if(&os == &cerr) os << "[";
    for(auto &y : x) {
        if(&os == &cerr)
            os << (f ? "" : ", ") << y;
        else
            os << (f ? "" : " ") << y;
        f = false;
    }
    if(&os == &cerr) os << "]";
    return os;
}

#define dump(...) 1;

void OUT() { cout << endl; }
template <class Head, class... Tail> void OUT(const Head &head, const Tail &...tail) {
    cout << head;
    if(sizeof...(tail)) cout << ' ';
    OUT(tail...);
}

template <typename T> static constexpr T inf = numeric_limits<T>::max() / 2;
template <class T, class S> constexpr pair<T, S> inf<pair<T, S>> = {inf<T>, inf<S>};

template <class T> void OUT2(const T &t, T INF = inf<T>, T res = -1) { OUT(t != INF ? t : res); }
template <class T> void OUT2(vector<T> &v, T INF = inf<T>, T res = -1) {
    fore(e, v) if(e == INF) e = res;
    OUT(v);
    fore(e, v) if(e == res) e = INF;
}

template <class F> struct REC {
    F f;
    REC(F &&f_) : f(std::forward<F>(f_)) {}
    template <class... Args> auto operator()(Args &&...args) const { return f(*this, std::forward<Args>(args)...); }
};

template <class S> vector<pair<S, int>> runLength(const vector<S> &v) {
    vector<pair<S, int>> res;
    for(auto &e : v) {
        if(res.empty() or res.back().fi != e)
            res.eb(e, 1);
        else
            res.back().se++;
    }
    return res;
}
vector<pair<char, int>> runLength(const string &v) {
    vector<pair<char, int>> res;
    for(auto &e : v) {
        if(res.empty() or res.back().fi != e)
            res.eb(e, 1);
        else
            res.back().se++;
    }
    return res;
}

struct string_converter {
    char start = 0;
    char type(const char &c) const { return (islower(c) ? 'a' : isupper(c) ? 'A' : isdigit(c) ? '0' : 0); }
    int convert(const char &c) {
        if(!start) start = type(c);
        return c - start;
    }
    int convert(const char &c, const string &chars) { return chars.find(c); }
    template <typename T> auto convert(const T &v) {
        vector<decltype(convert(v[0]))> ret;
        ret.reserve(size(v));
        for(auto &&e : v) ret.emplace_back(convert(e));
        return ret;
    }
    template <typename T> auto convert(const T &v, const string &chars) {
        vector<decltype(convert(v[0], chars))> ret;
        ret.reserve(size(v));
        for(auto &&e : v) ret.emplace_back(convert(e, chars));
        return ret;
    }
    int operator()(const char &v, char s = 0) {
        start = s;
        return convert(v);
    }
    int operator()(const char &v, const string &chars) { return convert(v, chars); }
    template <typename T> auto operator()(const T &v, char s = 0) {
        start = s;
        return convert(v);
    }
    template <typename T> auto operator()(const T &v, const string &chars) { return convert(v, chars); }
} toint;

template <class T, class F> T bin_search(T ok, T ng, const F &f) {
    while(abs(ok - ng) > 1) {
        T mid = ok + ng >> 1;
        (f(mid) ? ok : ng) = mid;
    }
    return ok;
}
template <class T, class F> T bin_search_double(T ok, T ng, const F &f, int iter = 80) {
    while(iter--) {
        T mid = (ok + ng) / 2;
        (f(mid) ? ok : ng) = mid;
    }
    return ok;
}

struct Setup_io {
    Setup_io() {
        ios_base::sync_with_stdio(0), cin.tie(0), cout.tie(0);
        cout << fixed << setprecision(11);
    }
} setup_io;

#endif
#pragma endregion

struct Timer {
    chrono::high_resolution_clock::time_point st;

    Timer() { reset(); }
    void reset() { st = chrono::high_resolution_clock::now(); }

    long long elapsed() {
        auto ed = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::milliseconds>(ed - st).count();
    }
    long long operator()() { return elapsed(); }
};
struct log_table {
    static constexpr int M = 65536;
    static constexpr int mask = M - 1;
    double l[M];
    log_table() : l() {
        unsigned long long x = 88172645463325252ULL;
        double log_u64max = log(2) * 64;
        for(int i = 0; i < M; i++) {
            x = x ^ (x << 7);
            x = x ^ (x >> 9);
            l[i] = log(double(x)) - log_u64max;
        }
    }
    double operator()(int i) const { return l[i & mask]; }
};
struct Simulated_Annealing {
  private:
    log_table rand_log;
    double end_time, inv_time, cur_time;
    double start_temp, diff_temp, cur_temp;
    Timer *timer;

    void set_time(double start_time) {
        inv_time = 1.0 / (end_time - start_time);
        cur_time = start_time;
    }
    void set_temp() { cur_temp = max(0.0, start_temp - cur_time * diff_temp * inv_time); }

  public:
    Simulated_Annealing(double _end_time, double _start_temp, double _end_temp, Timer *_timer = nullptr)
        : end_time(_end_time), start_temp(_start_temp), diff_temp(_start_temp - _end_temp), timer(_timer) {
        if(timer == nullptr) timer = new Timer;
        set_time(timer->elapsed());
        set_temp();
    }
    template <typename F, typename UPDATE> void run(const F &f, const UPDATE &update) {
        for(int iter = 0;; iter++) {
            if((iter & 0xFF) == 0) {
                if((cur_time = timer->elapsed()) > end_time) break;
                set_temp();
                update(inv_time * cur_time);
            }
            f(cur_temp * rand_log.l[iter & 0xFFFF]);
        }
    }

    void reset() { timer->reset(); }
};

constexpr int n = 20;
int w[n][n], d[n][n];

struct Road {
    vector<pii> v;
    pii &operator[](int k) { return v[k]; }
    int over, cost;
    void calc() {
        if(empty(v)) {
            over = cost = 0;
            return;
        }
        int pi = 0, pj = 0;
        pi = pj = 0;
        int dam = 0, sum = 0;
        over = 0;

        per(i, si(v)) {
            int t = abs(v[i].fi - pi) + abs(v[i].se - pj);
            sum += t;
            if(dam >= d[v[i].fi][v[i].se]) over += (dam - d[v[i].fi][v[i].se] + 1);
            dam += w[v[i].fi][v[i].se] * sum;
            pi = v[i].fi, pj = v[i].se;
        }
        cost = sum + abs(v[0].fi) + abs(v[0].se);
    }
    // float score(float factor) { return cost + factor * over; }
    int dist(int i, int j) {
        assert(i < si(v) and j < si(v));
        return abs(v[i].fi - v[j].fi) + abs(v[i].se - v[j].se);
    }
};

struct State {
    float factor = 0.1;
    vector<Road> v;
    int csum = 0, osum = 0;
    float score() { return csum + osum * factor; }

    void change(float diff) {
        int i = rnd(si(v));
        int j = rnd(si(v));
        while(i == j) j = rnd(si(v));
        int type = rnd(100);
        if(type < 85)
            type = 0;
        else
            type = 1;

        auto upd = [&] {
            int ic = v[i].cost, io = v[i].over;
            int jc = v[j].cost, jo = v[j].over;
            v[i].calc(), v[j].calc();
            int ncs = csum - ic - jc + v[i].cost + v[j].cost;
            int nos = osum - io - jo + v[i].over + v[j].over;
            if(score() - (ncs + nos * factor) >= diff) {
                csum = ncs;
                osum = nos;
                return true;
            } else {
                v[i].cost = ic, v[i].over = io;
                v[j].cost = jc, v[j].over = jo;
                return false;
            }
        };
        if(type == 0) {
            // swap
            int x = rnd(si(v[i].v));
            int y = rnd(si(v[j].v));
            swap(v[i][x], v[j][y]);
            if(!upd()) { swap(v[i][x], v[j][y]); }
        } else if(type == 1) {
            // insert
            int x = rnd(si(v[i].v));
            int y = rnd(si(v[j].v) + 1);
            v[j].v.insert(begin(v[j].v) + y, v[i][x]);
            v[i].v.erase(begin(v[i].v) + x);
            if(upd()) {
                if(v[i].v.empty()) {
                    swap(v[i], v.back());
                    v.pop_back();
                }
            } else {
                v[i].v.insert(begin(v[i].v) + x, v[j][y]);
                v[j].v.erase(begin(v[j].v) + y);
            }
        } else if(type == 2) {
            int x = rnd(si(v[i].v));
            int y = rnd(si(v[i].v));
            while(x == y) y = rnd(si(v[i].v));
            if(x > y) swap(x, y);
            // reverse(rng(v[i].v, x, y));
            swap(v[i].v[x], v[i].v[y]);
            int ic = v[i].cost, io = v[i].over;
            v[i].calc();
            int ncs = csum - ic + v[i].cost;
            int nos = osum - io + v[i].over;
            if(score() - (ncs + nos * factor) >= diff) {
                csum = ncs;
                osum = nos;
            } else {
                // reverse(rng(v[i].v, x, y));
                swap(v[i][x], v[i][y]);
                v[i].cost = ic;
                v[i].over = io;
            }
        }
    }

    int sgn(int x) { return x < 0 ? -1 : x == 0 ? 0 : 1; }
    int dist(int x, int y, int nx, int ny) { return abs(x - nx) + abs(y - ny); }
    int dist(pii p, int x, int y) { return dist(p.fi, p.se, x, y); }
    int dist(int x, int y, pii p) { return dist(x, y, p.fi, p.se); }
    int dist(pii x, pii y) { return dist(x.fi, x.se, y.fi, y.se); }

    void adjust(Timer timer) {
        vi far(si(v));
        rep(i, si(v)) { fore(x, y, v[i].v) chmax(far[i], x + y); }
        auto id = iota(far);
        REV(id);
        random_shuffle(id);
        rearrange(id, v);

        int bests = 0;
        string sans;
        rep(i, si(v)) {
            v[i].v.insert(begin(v[i].v), pii(0, 0));
            v[i].v.eb(0, 0);
        }

        int iter = 0;

        while(timer.elapsed() < 1960) {
            iter++;
            bool flag = true;
            int xxx, yyy;
            decltype(v) memv;
            if(iter < 100) {
                memv = v;
                random_shuffle(v);
            } else {
                xxx = rnd(si(v)), yyy = rnd(si(v));
                while(xxx == yyy) yyy = rnd(si(v));
                swap(v[xxx], v[yyy]);
            }
            // random_shuffle(id);
            // rearrange(id, v);
            // rep(i, si(v)) dump(v[i].v);

            struct D {
                // int pi, pj;
                int x, y;
                int mv;
                int d, w;
            };

            vector<vector<D>> ans(si(v));

            // rep(i, si(v)) {
            //     rep(j, si(v[i].v)) ans[i].pb(D{v[i][j].fi, v[i][j].se, v[i][j].fi, v[i][j].se, (j == si(v[i].v) - 1 or !j ? 0 : 1),
            //     d[v[i][j].fi][v[i][j].se]});
            // }
            vv(int, used, n, n, si(v));
            used[0][0] = 0;

            int bs = 0;

            rep(i, si(v)) {
                int sum = 0, dam = 0;
                int px = 0, py = 0;
                ans[i].pb(D{0, 0, 0, inf<int>, 0});

                // int sum2 = 0, dam2 = 0, px2 = 0, py2 = 0;

                per(j, si(v[i].v) - 1, 1) {

                    auto [x, y] = v[i][j];
                    if(v[i].dist(j - 1, j + 1) != v[i].dist(j - 1, j) + v[i].dist(j, j + 1)) {

                        int best = 0;
                        int bi = -1, bj = -1, bnx, bny;
                        rep(ii, i) {
                            int mi = inf<int>;
                            rep(jj, si(ans[ii]) - 1) {
                                chmin(mi, ans[ii][jj].d);

                                if(ans[ii][jj + 1].mv == 2) continue;
                                if(min(ans[ii][jj].x, ans[ii][jj + 1].x) <= x and x <= max(ans[ii][jj].x, ans[ii][jj + 1].x) //
                                   and min(ans[ii][jj].y, ans[ii][jj + 1].y) <= y and y <= max(ans[ii][jj].y, ans[ii][jj + 1].y)) {

                                    int dx = sgn(v[i][j + 1].fi - v[i][j].fi);
                                    int dy = sgn(v[i][j + 1].se - v[i][j].se);

                                    int xb = (dx ? min((v[i][j + 1].fi - v[i][j].fi) / dx, (ans[ii][jj + 1].x - x) / dx) : 0);
                                    int yb = (dy ? min((v[i][j + 1].se - v[i][j].se) / dy, (ans[ii][jj + 1].y - y) / dy) : 0);

                                    rep(xx, xb + 1) {
                                        rep(yy, yb + 1) {
                                            int tx = x + xx * dx, ty = y + yy * dy;
                                            if(used[tx][ty] >= ii) continue;
                                            if(v[i].dist(j, j + 1) < dist(tx, ty, v[i][j + 1])) continue;
                                            int ch = v[i].dist(j - 1, j) + v[i].dist(j, j + 1) - dist(v[i][j - 1], tx, ty) - dist(tx, ty, v[i][j + 1]);
                                            if(ch <= best) continue;
                                            int good = dist(x, y, tx, ty);
                                            if(mi > good * w[x][y]) {
                                                chmax(best, ch);
                                                bi = ii, bj = jj;
                                                bnx = tx, bny = ty;
                                                used[x][y] = bi;
                                                used[bnx][bny] = i;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if(best) {
                            bs += best;
                            // dump(best);
                            // dump(v[i].dist(j - 1, j), v[i].dist(j, j + 1), dist(v[i][j - 1], bnx, bny), dist(v[i][j + 1], bnx, bny));
                            // dump(px, py, px2, py2);
                            // dump(sum, sum2);
                            // dump(i, j, bi, bj, x, y, bnx, bny, v[i][j - 1], v[i][j + 1]);
                            // dump(ans[bi][bj].x, ans[bi][bj].y, ans[bi][bj + 1].x, ans[bi][bj + 1].y);
                            ans[bi].insert(begin(ans[bi]) + bj + 1, D{v[i][j].fi, v[i][j].se, 1, inf<int>, 0});
                            ans[bi].insert(begin(ans[bi]) + bj + 2, D{bnx, bny, 2, inf<int>, 0});
                            int good = abs(x - bnx) + abs(y - bny);
                            rep(jj, bj + 1) {
                                ans[bi][jj].d -= good * w[x][y];
                                if(ans[bi][jj].d <= 0) flag = false;
                            }

                            // assert(dam <= dam2);
                            // sum2 += dist(px2, py2, x, y);
                            // dam2 += w[x][y] * sum2;
                            // px2 = x, py2 = y;

                            // assert(d[x][y] > dam);
                            if(dam >= d[x][y]) {
                                flag = false;
                                break;
                            }
                            ans[i].pb(D{bnx, bny, 1, d[v[i][j].fi][v[i][j].se] - dam, w[v[i][j].fi][v[i][j].se]});
                            sum += dist(px, py, bnx, bny);
                            dam += w[v[i][j].fi][v[i][j].se] * sum;
                            px = bnx, py = bny;
                            // dump(best, sum, sum2, dam, dam2);

                            continue;
                        }
                    }

                    // if(dam > dam2) { dump(i, j, px, py, px2, py2, sum, sum2, dam, dam2); }
                    // assert(dam <= dam2);
                    // sum2 += dist(px2, py2, x, y);
                    // dam2 += w[x][y] * sum2;
                    // px2 = x, py2 = y;

                    // assert(d[x][y] > dam);
                    if(d[x][y] <= dam) {
                        flag = false;
                        break;
                    }
                    ans[i].pb(D{x, y, 1, d[x][y] - dam, w[x][y]});
                    sum += dist(px, py, x, y);
                    dam += w[x][y] * sum;
                    px = x, py = y;
                    used[x][y] = i;
                }
                if(!flag) break;
                ans[i].pb(D{0, 0, 0, inf<int>, 0});
                REV(ans[i]);
            }
            // if(!flag) continue;

            if(flag and chmax(bests, bs)) {
                sans.clear();

                rep(i, si(ans)) {
                    rep(j, si(ans[i]) - 1) {
                        int dx = ans[i][j + 1].x - ans[i][j].x;
                        int dy = ans[i][j + 1].y - ans[i][j].y;
                        while(dx > 0) sans += "D\n", dx--;
                        while(dx < 0) sans += "U\n", dx++;
                        while(dy > 0) sans += "R\n", dy--;
                        while(dy < 0) sans += "L\n", dy++;
                        if(ans[i][j + 1].mv) sans += to_string(ans[i][j + 1].mv) + "\n";
                    }
                }
            } else {
                if(iter < 100)
                    swap(v, memv);
                else
                    swap(v[xxx], v[yyy]);
            }
        }
        dump(iter);
        dump(bests);
        OUT(sans);
    }

    void output() {
        fore(r, v) {
            int pi = 0, pj = 0;
            r.v.eb(0, 0);
            rep(i, si(r.v)) {
                auto [x, y] = r.v[i];
                while(pi < x) {
                    pi++;
                    OUT('D');
                }
                while(pi > x) {
                    pi--;
                    OUT('U');
                }
                while(pj < y) {
                    pj++;
                    OUT('R');
                }
                while(pj > y) {
                    pj--;
                    OUT('L');
                }
                if(i == si(r.v) - 1) break;
                OUT(1);
            }
        }
    }
};

int main() {
    {
        INT(_);
        rep(i, n) rep(j, n) cin >> w[i][j];
        rep(i, n) rep(j, n) cin >> d[i][j];
    }

    Timer timer;
    Simulated_Annealing sa(1650, 2, -1, &timer);
    // float factor = 0.1;

    State state;
    rep(i, n) rep(j, n) {
        if(!i and !j) continue;
        Road r;

        r.v.eb(i, j);
        r.calc();
        state.v.eb(r);
        state.csum += r.cost;
        state.osum += r.over;
    }

    int iter = 0;
    auto f = [&](float diff) {
        state.change(diff);
        iter++;
    };
    auto update = [&](float d) {
        // dump(state.csum, state.osum);
        state.factor = pow(30, d);
        // dump(state.factor);
    };
    sa.run(f, update);
    dump(iter);
    // rep(i, si(state.v)) {
    //     state.v[i].calc();
    //     dump(state.v[i].v, state.v[i].cost, state.v[i].over);
    // }
    state.adjust(timer);
    // state.output();
}