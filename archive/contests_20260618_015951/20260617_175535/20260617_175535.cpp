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
// ACL: mincostflow
// https://github.com/atcoder/ac-library
// clang-format off
#ifndef ATCODER
#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <utility>
#include <vector>
namespace atcoder{
namespace internal{
template <class E> struct csr{
std::vector<int> start;
std::vector<E> elist;
explicit csr(int n,const std::vector<std::pair<int,E>>& edges)
: start(n + 1),elist(edges.size()){
for(auto e : edges){
start[e.first + 1]++;
}
for(int i = 1;i <= n;i++){
start[i]+= start[i - 1];
}
auto counter = start;
for(auto e : edges){
elist[counter[e.first]++]= e.second;
}
}
};
}// namespace internal
}// namespace atcoder
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
template <class Cap,class Cost> struct mcf_graph{
public:
mcf_graph(){}
explicit mcf_graph(int n): _n(n){}
int add_edge(int from,int to,Cap cap,Cost cost){
assert(0 <= from && from < _n);
assert(0 <= to && to < _n);
assert(0 <= cap);
assert(0 <= cost);
int m = int(_edges.size());
_edges.push_back({from,to,cap,0,cost});
return m;
}
struct edge{
int from,to;
Cap cap,flow;
Cost cost;
};
edge get_edge(int i){
int m = int(_edges.size());
assert(0 <= i && i < m);
return _edges[i];
}
std::vector<edge> edges(){return _edges;}
std::pair<Cap,Cost> flow(int s,int t){
return flow(s,t,std::numeric_limits<Cap>::max());
}
std::pair<Cap,Cost> flow(int s,int t,Cap flow_limit){
return slope(s,t,flow_limit).back();
}
std::vector<std::pair<Cap,Cost>> slope(int s,int t){
return slope(s,t,std::numeric_limits<Cap>::max());
}
std::vector<std::pair<Cap,Cost>> slope(int s,int t,Cap flow_limit){
assert(0 <= s && s < _n);
assert(0 <= t && t < _n);
assert(s != t);
int m = int(_edges.size());
std::vector<int> edge_idx(m);
auto g =[&](){
std::vector<int> degree(_n),redge_idx(m);
std::vector<std::pair<int,_edge>> elist;
elist.reserve(2 * m);
for(int i = 0;i < m;i++){
auto e = _edges[i];
edge_idx[i]= degree[e.from]++;
redge_idx[i]= degree[e.to]++;
elist.push_back({e.from,{e.to,-1,e.cap - e.flow,e.cost}});
elist.push_back({e.to,{e.from,-1,e.flow,-e.cost}});
}
auto _g = internal::csr<_edge>(_n,elist);
for(int i = 0;i < m;i++){
auto e = _edges[i];
edge_idx[i]+= _g.start[e.from];
redge_idx[i]+= _g.start[e.to];
_g.elist[edge_idx[i]].rev = redge_idx[i];
_g.elist[redge_idx[i]].rev = edge_idx[i];
}
return _g;
}();
auto result = slope(g,s,t,flow_limit);
for(int i = 0;i < m;i++){
auto e = g.elist[edge_idx[i]];
_edges[i].flow = _edges[i].cap - e.cap;
}
return result;
}
private:
int _n;
std::vector<edge> _edges;
struct _edge{
int to,rev;
Cap cap;
Cost cost;
};
std::vector<std::pair<Cap,Cost>> slope(internal::csr<_edge>& g,
int s,
int t,
Cap flow_limit){
std::vector<std::pair<Cost,Cost>> dual_dist(_n);
std::vector<int> prev_e(_n);
std::vector<bool> vis(_n);
struct Q{
Cost key;
int to;
bool operator<(Q r)const{return key > r.key;}
};
std::vector<int> que_min;
std::vector<Q> que;
auto dual_ref =[&](){
for(int i = 0;i < _n;i++){
dual_dist[i].second = std::numeric_limits<Cost>::max();
}
std::fill(vis.begin(),vis.end(),false);
que_min.clear();
que.clear();
size_t heap_r = 0;
dual_dist[s].second = 0;
que_min.push_back(s);
while(!que_min.empty()|| !que.empty()){
int v;
if(!que_min.empty()){
v = que_min.back();
que_min.pop_back();
}else{
while(heap_r < que.size()){
heap_r++;
std::push_heap(que.begin(),que.begin()+ heap_r);
}
v = que.front().to;
std::pop_heap(que.begin(),que.end());
que.pop_back();
heap_r--;
}
if(vis[v])continue;
vis[v]= true;
if(v == t)break;
Cost dual_v = dual_dist[v].first,dist_v = dual_dist[v].second;
for(int i = g.start[v];i < g.start[v + 1];i++){
auto e = g.elist[i];
if(!e.cap)continue;
Cost cost = e.cost - dual_dist[e.to].first + dual_v;
if(dual_dist[e.to].second - dist_v > cost){
Cost dist_to = dist_v + cost;
dual_dist[e.to].second = dist_to;
prev_e[e.to]= e.rev;
if(dist_to == dist_v){
que_min.push_back(e.to);
}else{
que.push_back(Q{dist_to,e.to});
}
}
}
}
if(!vis[t]){
return false;
}
for(int v = 0;v < _n;v++){
if(!vis[v])continue;
dual_dist[v].first -= dual_dist[t].second - dual_dist[v].second;
}
return true;
};
Cap flow = 0;
Cost cost = 0,prev_cost_per_flow = -1;
std::vector<std::pair<Cap,Cost>> result ={{Cap(0),Cost(0)}};
while(flow < flow_limit){
if(!dual_ref())break;
Cap c = flow_limit - flow;
for(int v = t;v != s;v = g.elist[prev_e[v]].to){
c = std::min(c,g.elist[g.elist[prev_e[v]].rev].cap);
}
for(int v = t;v != s;v = g.elist[prev_e[v]].to){
auto& e = g.elist[prev_e[v]];
e.cap += c;
g.elist[e.rev].cap -= c;
}
Cost d = -dual_dist[s].first;
flow += c;
cost += c * d;
if(prev_cost_per_flow == d){
result.pop_back();
}
result.push_back({flow,cost});
prev_cost_per_flow = d;
}
return result;
}
};
}// namespace atcoder
// clang-format on
using namespace atcoder;
#endif  // ATCODER

int main() {
    int N,M;
    input(N,M);
    mcf_graph<ll, ll> g(2*M+2);
    int s = 2*M,t=2*M+1;
    const ll C = 1e6;

    rep(i,M){
        g.add_edge(s, i, 1, 0);
        g.add_edge(i+M, t, 1,0);
    }

    auto A = make_vec<ll>(N,M,0LL);
    input(A);

    rep(i,M){
        vector<int> cnt(M+1);
        ll S = 0;
        rep(j,N){
            cnt[A[j][i]]++;
            S += A[j][i];
        }

        ll mx = 0;

        rrep(j,M,0){
            if (cnt[j] == 1){
                mx = j;
                break;
            }
        }
        rep(j,1,M+1){
            if (j > mx && cnt[j] == 0){
                g.add_edge((j-1), M+i, 1, C - (S+j));
            } else {
                g.add_edge(j-1, M+i, 1, C);
            }
        }
    }

    ll ans = C * M - g.flow(s,t).second;
    print(ans);
}