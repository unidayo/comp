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
[[maybe_unused]] constexpr int dx4[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dy4[] = {1, 0, -1, 0};
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
// ACL: maxflow
// https://github.com/atcoder/ac-library
// clang-format off
#ifndef ATCODER
#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <vector>
namespace atcoder{
namespace internal{
template <class T> struct simple_queue{
std::vector<T> payload;
int pos = 0;
void reserve(int n){payload.reserve(n);}
int size()const{return int(payload.size())- pos;}
bool empty()const{return pos == int(payload.size());}
void push(const T& t){payload.push_back(t);}
T& front(){return payload[pos];}
void clear(){
payload.clear();
pos = 0;
}
void pop(){pos++;}
};
}// namespace internal
}// namespace atcoder
namespace atcoder{
template <class Cap> struct mf_graph{
public:
mf_graph(): _n(0){}
explicit mf_graph(int n): _n(n),g(n){}
int add_edge(int from,int to,Cap cap){
assert(0 <= from && from < _n);
assert(0 <= to && to < _n);
assert(0 <= cap);
int m = int(pos.size());
pos.push_back({from,int(g[from].size())});
int from_id = int(g[from].size());
int to_id = int(g[to].size());
if(from == to)to_id++;
g[from].push_back(_edge{to,to_id,cap});
g[to].push_back(_edge{from,from_id,0});
return m;
}
struct edge{
int from,to;
Cap cap,flow;
};
edge get_edge(int i){
int m = int(pos.size());
assert(0 <= i && i < m);
auto _e = g[pos[i].first][pos[i].second];
auto _re = g[_e.to][_e.rev];
return edge{pos[i].first,_e.to,_e.cap + _re.cap,_re.cap};
}
std::vector<edge> edges(){
int m = int(pos.size());
std::vector<edge> result;
for(int i = 0;i < m;i++){
result.push_back(get_edge(i));
}
return result;
}
void change_edge(int i,Cap new_cap,Cap new_flow){
int m = int(pos.size());
assert(0 <= i && i < m);
assert(0 <= new_flow && new_flow <= new_cap);
auto& _e = g[pos[i].first][pos[i].second];
auto& _re = g[_e.to][_e.rev];
_e.cap = new_cap - new_flow;
_re.cap = new_flow;
}
Cap flow(int s,int t){
return flow(s,t,std::numeric_limits<Cap>::max());
}
Cap flow(int s,int t,Cap flow_limit){
assert(0 <= s && s < _n);
assert(0 <= t && t < _n);
assert(s != t);
std::vector<int> level(_n),iter(_n);
internal::simple_queue<int> que;
auto bfs =[&](){
std::fill(level.begin(),level.end(),-1);
level[s]= 0;
que.clear();
que.push(s);
while(!que.empty()){
int v = que.front();
que.pop();
for(auto e : g[v]){
if(e.cap == 0 || level[e.to]>= 0)continue;
level[e.to]= level[v]+ 1;
if(e.to == t)return;
que.push(e.to);
}
}
};
auto dfs =[&](auto self,int v,Cap up){
if(v == s)return up;
Cap res = 0;
int level_v = level[v];
for(int& i = iter[v];i < int(g[v].size());i++){
_edge& e = g[v][i];
if(level_v <= level[e.to]|| g[e.to][e.rev].cap == 0)continue;
Cap d =
self(self,e.to,std::min(up - res,g[e.to][e.rev].cap));
if(d <= 0)continue;
g[v][i].cap += d;
g[e.to][e.rev].cap -= d;
res += d;
if(res == up)return res;
}
level[v]= _n;
return res;
};
Cap flow = 0;
while(flow < flow_limit){
bfs();
if(level[t]== -1)break;
std::fill(iter.begin(),iter.end(),0);
Cap f = dfs(dfs,t,flow_limit - flow);
if(!f)break;
flow += f;
}
return flow;
}
std::vector<bool> min_cut(int s){
std::vector<bool> visited(_n);
internal::simple_queue<int> que;
que.push(s);
while(!que.empty()){
int p = que.front();
que.pop();
visited[p]= true;
for(auto e : g[p]){
if(e.cap && !visited[e.to]){
visited[e.to]= true;
que.push(e.to);
}
}
}
return visited;
}
private:
int _n;
struct _edge{
int to,rev;
Cap cap;
};
std::vector<std::pair<int,int>> pos;
std::vector<std::vector<_edge>> g;
};
}// namespace atcoder
// clang-format on
using namespace atcoder;
#endif  // ATCODER

int main() {
    ll N,M,R;
    input(N,M,R);
    mf_graph<ll> g(N+M+2);
    int s = N+M,t=N+M+1;

    rep(i,N){
        g.add_edge(s, i,1);
    }

    rep(i,M){
        g.add_edge(N+i, t,1);
    }

    auto A = make_vec<char>(N,M,char(1));
    rep(i,R){
        int d,m;
        input(d,m);
        d--,m--;
        A[d][m] = 0;
    }

    rep(i,N){
        rep(j,M){
            if (A[i][j]){
                g.add_edge(i, j+N, 1);
            }
        }
    }

    print(N+M-g.flow(s,t));
}