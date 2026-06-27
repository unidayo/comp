/**
 *  date : 2021-12-12 19:44:19
 */

#define NDEBUG
using namespace std;

// intrinstic
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
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// utility
namespace Nyaan {
using ll = long long;
using i64 = long long;
using u64 = unsigned long long;
using i128 = __int128_t;
using u128 = __uint128_t;

template <typename T>
using V = vector<T>;
template <typename T>
using VV = vector<vector<T>>;
using vi = vector<int>;
using vl = vector<long long>;
using vd = V<double>;
using vs = V<string>;
using vvi = vector<vector<int>>;
using vvl = vector<vector<long long>>;

template <typename T, typename U>
struct P : pair<T, U> {
  template <typename... Args>
  P(Args... args) : pair<T, U>(args...) {}

  using pair<T, U>::first;
  using pair<T, U>::second;

  T &x() { return first; }
  const T &x() const { return first; }
  U &y() { return second; }
  const U &y() const { return second; }

  P &operator+=(const P &r) {
    first += r.first;
    second += r.second;
    return *this;
  }
  P &operator-=(const P &r) {
    first -= r.first;
    second -= r.second;
    return *this;
  }
  P &operator*=(const P &r) {
    first *= r.first;
    second *= r.second;
    return *this;
  }
  P operator+(const P &r) const { return P(*this) += r; }
  P operator-(const P &r) const { return P(*this) -= r; }

  P operator*(const P &r) const { return P(*this) *= r; }

  P operator*(int r) const { return {first * r, second * r}; }

  P operator-() const { return P{-first, -second}; }
};

using pl = P<ll, ll>;
using pi = P<int, int>;
using vp = V<pl>;

constexpr int inf = 1001001001;
constexpr long long infLL = 4004004004004004004LL;

template <typename T>
int sz(const T &t) {
  return t.size();
}

template <typename T, typename U>
inline bool amin(T &x, U y) {
  return (y < x) ? (x = y, true) : false;
}
template <typename T, typename U>
inline bool amax(T &x, U y) {
  return (x < y) ? (x = y, true) : false;
}

template <typename T>
inline T Max(const vector<T> &v) {
  return *max_element(begin(v), end(v));
}
template <typename T>
inline T Min(const vector<T> &v) {
  return *min_element(begin(v), end(v));
}
template <typename T>
inline long long Sum(const vector<T> &v) {
  return accumulate(begin(v), end(v), 0LL);
}

template <typename T>
int lb(const vector<T> &v, const T &a) {
  return lower_bound(begin(v), end(v), a) - begin(v);
}
template <typename T>
int ub(const vector<T> &v, const T &a) {
  return upper_bound(begin(v), end(v), a) - begin(v);
}

constexpr long long TEN(int n) {
  long long ret = 1, x = 10;
  for (; n; x *= x, n >>= 1) ret *= (n & 1 ? x : 1);
  return ret;
}

template <typename T, typename U>
pair<T, U> mkp(const T &t, const U &u) {
  return make_pair(t, u);
}

template <typename T>
vector<T> mkrui(const vector<T> &v, bool rev = false) {
  vector<T> ret(v.size() + 1);
  if (rev) {
    for (int i = int(v.size()) - 1; i >= 0; i--) ret[i] = v[i] + ret[i + 1];
  } else {
    for (int i = 0; i < int(v.size()); i++) ret[i + 1] = ret[i] + v[i];
  }
  return ret;
};

template <typename T>
vector<T> mkuni(const vector<T> &v) {
  vector<T> ret(v);
  sort(ret.begin(), ret.end());
  ret.erase(unique(ret.begin(), ret.end()), ret.end());
  return ret;
}

template <typename F>
vector<int> mkord(int N, F f) {
  vector<int> ord(N);
  iota(begin(ord), end(ord), 0);
  sort(begin(ord), end(ord), f);
  return ord;
}

template <typename T>
vector<int> mkinv(vector<T> &v) {
  int max_val = *max_element(begin(v), end(v));
  vector<int> inv(max_val + 1, -1);
  for (int i = 0; i < (int)v.size(); i++) inv[v[i]] = i;
  return inv;
}

}  // namespace Nyaan

// bit operation
namespace Nyaan {
__attribute__((target("popcnt"))) inline int popcnt(const u64 &a) {
  return _mm_popcnt_u64(a);
}
inline int lsb(const u64 &a) { return a ? __builtin_ctzll(a) : 64; }
inline int ctz(const u64 &a) { return a ? __builtin_ctzll(a) : 64; }
inline int msb(const u64 &a) { return a ? 63 - __builtin_clzll(a) : -1; }
template <typename T>
inline int gbit(const T &a, int i) {
  return (a >> i) & 1;
}
template <typename T>
inline void sbit(T &a, int i, bool b) {
  if (gbit(a, i) != b) a ^= T(1) << i;
}
constexpr long long PW(int n) { return 1LL << n; }
constexpr long long MSK(int n) { return (1LL << n) - 1; }
}  // namespace Nyaan

// inout
namespace Nyaan {

template <typename T, typename U>
ostream &operator<<(ostream &os, const pair<T, U> &p) {
  os << p.first << " " << p.second;
  return os;
}
template <typename T, typename U>
istream &operator>>(istream &is, pair<T, U> &p) {
  is >> p.first >> p.second;
  return is;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<T> &v) {
  int s = (int)v.size();
  for (int i = 0; i < s; i++) os << (i ? " " : "") << v[i];
  return os;
}
template <typename T>
istream &operator>>(istream &is, vector<T> &v) {
  for (auto &x : v) is >> x;
  return is;
}

void in() {}
template <typename T, class... U>
void in(T &t, U &... u) {
  cin >> t;
  in(u...);
}

void out() { cout << "\n"; }
template <typename T, class... U, char sep = ' '>
void out(const T &t, const U &... u) {
  cout << t;
  if (sizeof...(u)) cout << sep;
  out(u...);
}

void outr() {}
template <typename T, class... U, char sep = ' '>
void outr(const T &t, const U &... u) {
  cout << t;
  outr(u...);
}

struct IoSetupNya {
  IoSetupNya() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);
    cout << fixed << setprecision(15);
    cerr << fixed << setprecision(7);
  }
} iosetupnya;

}  // namespace Nyaan

// debug
namespace DebugImpl {

template <typename U, typename = void>
struct is_specialize : false_type {};
template <typename U>
struct is_specialize<
    U, typename conditional<false, typename U::iterator, void>::type>
    : true_type {};
template <typename U>
struct is_specialize<
    U, typename conditional<false, decltype(U::first), void>::type>
    : true_type {};
template <typename U>
struct is_specialize<U, enable_if_t<is_integral<U>::value, void>> : true_type {
};

void dump(const char& t) { cerr << t; }

void dump(const string& t) { cerr << t; }

void dump(const bool& t) { cerr << (t ? "true" : "false"); }

template <typename U,
          enable_if_t<!is_specialize<U>::value, nullptr_t> = nullptr>
void dump(const U& t) {
  cerr << t;
}

template <typename T>
void dump(const T& t, enable_if_t<is_integral<T>::value>* = nullptr) {
  string res;
  if (t == Nyaan::inf) res = "inf";
  if constexpr (is_signed<T>::value) {
    if (t == -Nyaan::inf) res = "-inf";
  }
  if constexpr (sizeof(T) == 8) {
    if (t == Nyaan::infLL) res = "inf";
    if constexpr (is_signed<T>::value) {
      if (t == -Nyaan::infLL) res = "-inf";
    }
  }
  if (res.empty()) res = to_string(t);
  cerr << res;
}

template <typename T, typename U>
void dump(const pair<T, U>&);
template <typename T>
void dump(const pair<T*, int>&);

template <typename T>
void dump(const T& t,
          enable_if_t<!is_void<typename T::iterator>::value>* = nullptr) {
  cerr << "[ ";
  for (auto it = t.begin(); it != t.end();) {
    dump(*it);
    cerr << (++it == t.end() ? "" : ", ");
  }
  cerr << " ]";
}

template <typename T, typename U>
void dump(const pair<T, U>& t) {
  cerr << "( ";
  dump(t.first);
  cerr << ", ";
  dump(t.second);
  cerr << " )";
}

template <typename T>
void dump(const pair<T*, int>& t) {
  cerr << "[ ";
  for (int i = 0; i < t.second; i++) {
    dump(t.first[i]);
    cerr << (i == t.second - 1 ? "" : ", ");
  }
  cerr << " ]";
}

void trace() { cerr << endl; }
template <typename Head, typename... Tail>
void trace(Head&& head, Tail&&... tail) {
  cerr << " ";
  dump(head);
  if (sizeof...(tail) != 0) cerr << ",";
  trace(forward<Tail>(tail)...);
}

}  // namespace DebugImpl

#ifdef NyaanDebug
#define trc(...)                            \
  do {                                      \
    cerr << "## " << #__VA_ARGS__ << " = "; \
    DebugImpl::trace(__VA_ARGS__);          \
  } while (0)
#else
#define trc(...) (void(0))
#endif

// macro
#define each(x, v) for (auto&& x : v)
#define each2(x, y, v) for (auto&& [x, y] : v)
#define all(v) (v).begin(), (v).end()
#define rep(i, N) for (long long i = 0; i < (long long)(N); i++)
#define repr(i, N) for (long long i = (long long)(N)-1; i >= 0; i--)
#define rep1(i, N) for (long long i = 1; i <= (long long)(N); i++)
#define repr1(i, N) for (long long i = (N); (long long)(i) > 0; i--)
#define reg(i, a, b) for (long long i = (a); i < (b); i++)
#define regr(i, a, b) for (long long i = (b)-1; i >= (a); i--)
#define fi first
#define se second
#define ini(...)   \
  int __VA_ARGS__; \
  in(__VA_ARGS__)
#define inl(...)         \
  long long __VA_ARGS__; \
  in(__VA_ARGS__)
#define ins(...)      \
  string __VA_ARGS__; \
  in(__VA_ARGS__)
#define in2(s, t)                           \
  for (int i = 0; i < (int)s.size(); i++) { \
    in(s[i], t[i]);                         \
  }
#define in3(s, t, u)                        \
  for (int i = 0; i < (int)s.size(); i++) { \
    in(s[i], t[i], u[i]);                   \
  }
#define in4(s, t, u, v)                     \
  for (int i = 0; i < (int)s.size(); i++) { \
    in(s[i], t[i], u[i], v[i]);             \
  }
#define die(...)             \
  do {                       \
    Nyaan::out(__VA_ARGS__); \
    return;                  \
  } while (0)

namespace Nyaan {
void solve();
}
int main() { Nyaan::solve(); }

//

struct UnionFind {
  vector<int> data;
  UnionFind(int N) : data(N, -1) {}

  int find(int k) { return data[k] < 0 ? k : data[k] = find(data[k]); }

  int unite(int x, int y) {
    if ((x = find(x)) == (y = find(y))) return false;
    if (data[x] > data[y]) swap(x, y);
    data[x] += data[y];
    data[y] = x;
    return true;
  }

  // f ... merge function
  template<typename F>
  int unite(int x, int y,const F &f) {
    if ((x = find(x)) == (y = find(y))) return false;
    if (data[x] > data[y]) swap(x, y);
    data[x] += data[y];
    data[y] = x;
    f(x, y);
    return true;
  }

  int size(int k) { return -data[find(k)]; }

  int same(int x, int y) { return find(x) == find(y); }
};

/**
 * @brief Union Find(Disjoint Set Union)
 * @docs docs/data-structure/union-find.md
 */


template <typename T>
struct edge {
  int src, to;
  T cost;

  edge(int _to, T _cost) : src(-1), to(_to), cost(_cost) {}
  edge(int _src, int _to, T _cost) : src(_src), to(_to), cost(_cost) {}

  edge &operator=(const int &x) {
    to = x;
    return *this;
  }

  operator int() const { return to; }
};
template <typename T>
using Edges = vector<edge<T>>;
template <typename T>
using WeightedGraph = vector<Edges<T>>;
using UnweightedGraph = vector<vector<int>>;

// Input of (Unweighted) Graph
UnweightedGraph graph(int N, int M = -1, bool is_directed = false,
                      bool is_1origin = true) {
  UnweightedGraph g(N);
  if (M == -1) M = N - 1;
  for (int _ = 0; _ < M; _++) {
    int x, y;
    cin >> x >> y;
    if (is_1origin) x--, y--;
    g[x].push_back(y);
    if (!is_directed) g[y].push_back(x);
  }
  return g;
}

// Input of Weighted Graph
template <typename T>
WeightedGraph<T> wgraph(int N, int M = -1, bool is_directed = false,
                        bool is_1origin = true) {
  WeightedGraph<T> g(N);
  if (M == -1) M = N - 1;
  for (int _ = 0; _ < M; _++) {
    int x, y;
    cin >> x >> y;
    T c;
    cin >> c;
    if (is_1origin) x--, y--;
    g[x].emplace_back(x, y, c);
    if (!is_directed) g[y].emplace_back(y, x, c);
  }
  return g;
}

// Input of Edges
template <typename T>
Edges<T> esgraph(int N, int M, int is_weighted = true, bool is_1origin = true) {
  Edges<T> es;
  for (int _ = 0; _ < M; _++) {
    int x, y;
    cin >> x >> y;
    T c;
    if (is_weighted)
      cin >> c;
    else
      c = 1;
    if (is_1origin) x--, y--;
    es.emplace_back(x, y, c);
  }
  return es;
}

// Input of Adjacency Matrix
template <typename T>
vector<vector<T>> adjgraph(int N, int M, T INF, int is_weighted = true,
                           bool is_directed = false, bool is_1origin = true) {
  vector<vector<T>> d(N, vector<T>(N, INF));
  for (int _ = 0; _ < M; _++) {
    int x, y;
    cin >> x >> y;
    T c;
    if (is_weighted)
      cin >> c;
    else
      c = 1;
    if (is_1origin) x--, y--;
    d[x][y] = c;
    if (!is_directed) d[y][x] = c;
  }
  return d;
}

/**
 * @brief グラフテンプレート
 * @docs docs/graph/graph-template.md
 */



// 一般のグラフのstからの距離！！！！
// unvisited nodes : d = -1
vector<int> Depth(const UnweightedGraph &g, int start = 0) {
  int n = g.size();
  vector<int> ds(n, -1);
  ds[start] = 0;
  queue<int> q;
  q.push(start);
  while (!q.empty()) {
    int c = q.front();
    q.pop();
    int dc = ds[c];
    for (auto &d : g[c]) {
      if (ds[d] == -1) {
        ds[d] = dc + 1;
        q.push(d);
      }
    }
  }
  return ds;
}

// Depth of Rooted Weighted Tree
// unvisited nodes : d = -1
template <typename T>
vector<T> Depth(const WeightedGraph<T> &g, int start = 0) {
  vector<T> d(g.size(), -1);
  auto dfs = [&](auto rec, int cur, T val, int par = -1) -> void {
    d[cur] = val;
    for (auto &dst : g[cur]) {
      if (dst == par) continue;
      rec(rec, dst, val + dst.cost, cur);
    }
  };
  dfs(dfs, start, 0);
  return d;
}

// Diameter of Tree
// return value : { {u, v}, length }
pair<pair<int, int>, int> Diameter(const UnweightedGraph &g) {
  auto d = Depth(g, 0);
  int u = max_element(begin(d), end(d)) - begin(d);
  d = Depth(g, u);
  int v = max_element(begin(d), end(d)) - begin(d);
  return make_pair(make_pair(u, v), d[v]);
}

// Diameter of Weighted Tree
// return value : { {u, v}, length }
template <typename T>
pair<pair<int, int>, T> Diameter(const WeightedGraph<T> &g) {
  auto d = Depth(g, 0);
  int u = max_element(begin(d), end(d)) - begin(d);
  d = Depth(g, u);
  int v = max_element(begin(d), end(d)) - begin(d);
  return make_pair(make_pair(u, v), d[v]);
}

// nodes on the path u-v ( O(N) )
template <typename G>
vector<int> Path(G &g, int u, int v) {
  vector<int> ret;
  int end = 0;
  auto dfs = [&](auto rec, int cur, int par = -1) -> void {
    ret.push_back(cur);
    if (cur == v) {
      end = 1;
      return;
    }
    for (int dst : g[cur]) {
      if (dst == par) continue;
      rec(rec, dst, cur);
      if (end) return;
    }
    if (end) return;
    ret.pop_back();
  };
  dfs(dfs, u);
  return ret;
}

/**
 * @brief グラフユーティリティ
 * @docs docs/graph/graph-utility.md
 */




template <typename Tree, typename Node, typename T, T (*f)(T, T), T (*ts)(T)>
struct ReversibleBBST : Tree {
  using Tree::merge;
  using Tree::split;
  using typename Tree::Ptr;

  ReversibleBBST() = default;

  virtual void toggle(Ptr t) {
    swap(t->l, t->r);
    t->sum = ts(t->sum);
    t->rev ^= true;
  }

  T fold(Ptr &t, int a, int b) {
    auto x = split(t, a);
    auto y = split(x.second, b - a);
    auto ret = sum(y.first);
    t = merge(x.first, merge(y.first, y.second));
    return ret;
  }

  void reverse(Ptr &t, int a, int b) {
    auto x = split(t, a);
    auto y = split(x.second, b - a);
    toggle(y.first);
    t = merge(x.first, merge(y.first, y.second));
  }

  Ptr update(Ptr t) override {
    if (!t) return t;
    t->cnt = 1;
    t->sum = t->key;
    if (t->l) t->cnt += t->l->cnt, t->sum = f(t->l->sum, t->sum);
    if (t->r) t->cnt += t->r->cnt, t->sum = f(t->sum, t->r->sum);
    return t;
  }

 protected:
  inline T sum(const Ptr t) { return t ? t->sum : T(); }

  void push(Ptr t) override {
    if (!t) return;
    if (t->rev) {
      if (t->l) toggle(t->l);
      if (t->r) toggle(t->r);
      t->rev = false;
    }
  }
};

/**
 * @brief 反転可能平衡二分木(基底クラス)
 */

template <typename Node>
struct SplayTreeBase {
  using Ptr = Node *;
  template <typename... Args>
  Ptr my_new(const Args &... args) {
    return new Node(args...);
  }
  void my_del(Ptr p) { delete p; }

  bool is_root(Ptr t) { return !(t->p) || (t->p->l != t && t->p->r != t); }

  int size(Ptr t) const { return count(t); }

  virtual void splay(Ptr t) {
    push(t);
    while (!is_root(t)) {
      Ptr q = t->p;
      if (is_root(q)) {
        push(q), push(t);
        rot(t);
      } else {
        Ptr r = q->p;
        push(r), push(q), push(t);
        if (pos(q) == pos(t))
          rot(q), rot(t);
        else
          rot(t), rot(t);
      }
    }
  }

  Ptr get_left(Ptr t) {
    while (t->l) push(t), t = t->l;
    return t;
  }

  Ptr get_right(Ptr t) {
    while (t->r) push(t), t = t->r;
    return t;
  }

  pair<Ptr, Ptr> split(Ptr t, int k) {
    if (!t) return {nullptr, nullptr};
    if (k == 0) return {nullptr, t};
    if (k == count(t)) return {t, nullptr};
    push(t);
    if (k <= count(t->l)) {
      auto x = split(t->l, k);
      t->l = x.second;
      t->p = nullptr;
      if (x.second) x.second->p = t;
      return {x.first, update(t)};
    } else {
      auto x = split(t->r, k - count(t->l) - 1);
      t->r = x.first;
      t->p = nullptr;
      if (x.first) x.first->p = t;
      return {update(t), x.second};
    }
  }

  Ptr merge(Ptr l, Ptr r) {
    if (!l && !r) return nullptr;
    if (!l) return splay(r), r;
    if (!r) return splay(l), l;
    splay(l), splay(r);
    l = get_right(l);
    splay(l);
    l->r = r;
    r->p = l;
    update(l);
    return l;
  }

  using Key = decltype(Node::key);
  Ptr build(const vector<Key> &v) { return build(0, v.size(), v); }
  Ptr build(int l, int r, const vector<Key> &v) {
    if (l + 1 >= r) return my_new(v[l]);
    return merge(build(l, (l + r) >> 1, v), build((l + r) >> 1, r, v));
  }

  template <typename... Args>
  void insert(Ptr &t, int k, const Args &... args) {
    splay(t);
    auto x = split(t, k);
    t = merge(merge(x.first, my_new(args...)), x.second);
  }

  void erase(Ptr &t, int k) {
    splay(t);
    auto x = split(t, k);
    auto y = split(x.second, 1);
    my_del(y.first);
    t = merge(x.first, y.second);
  }

  virtual Ptr update(Ptr t) = 0;

 protected:
  inline int count(Ptr t) const { return t ? t->cnt : 0; }

  virtual void push(Ptr t) = 0;

  Ptr build(const vector<Ptr> &v) { return build(0, v.size(), v); }

  Ptr build(int l, int r, const vector<Ptr> &v) {
    if (l + 1 >= r) return v[l];
    return merge(build(l, (l + r) >> 1, v), build((l + r) >> 1, r, v));
  }

  inline int pos(Ptr t) {
    if (t->p) {
      if (t->p->l == t) return -1;
      if (t->p->r == t) return 1;
    }
    return 0;
  }

  virtual void rot(Ptr t) {
    Ptr x = t->p, y = x->p;
    if (pos(t) == -1) {
      if ((x->l = t->r)) t->r->p = x;
      t->r = x, x->p = t;
    } else {
      if ((x->r = t->l)) t->l->p = x;
      t->l = x, x->p = t;
    }
    update(x), update(t);
    if ((t->p = y)) {
      if (y->l == x) y->l = t;
      if (y->r == x) y->r = t;
    }
  }
};

/**
 * @brief Splay Tree(base)
 */

template <typename T>
struct ReversibleSplayTreeNode {
  using Ptr = ReversibleSplayTreeNode *;
  Ptr l, r, p;
  T key, sum;
  int cnt;
  bool rev;

  ReversibleSplayTreeNode(const T &t = T())
      : l(), r(), p(), key(t), sum(t), cnt(1), rev(false) {}
};

template <typename T, T (*f)(T, T), T (*ts)(T)>
struct ReversibleSplayTree
    : ReversibleBBST<SplayTreeBase<ReversibleSplayTreeNode<T>>,
                     ReversibleSplayTreeNode<T>, T, f, ts> {
  using Node = ReversibleSplayTreeNode<T>;
};

/**
 * @brief 反転可能Splay Tree
 */

//

template <typename Splay>
struct LinkCutBase : Splay {
  using Node = typename Splay::Node;
  using Ptr = Node*;

  virtual Ptr expose(Ptr t) {
    Ptr rp = nullptr;
    for (Ptr cur = t; cur; cur = cur->p) {
      this->splay(cur);
      cur->r = rp;
      this->update(cur);
      rp = cur;
    }
    this->splay(t);
    return rp;
  }

  virtual void link(Ptr u, Ptr v) {
    evert(u);
    expose(v);
    u->p = v;
  }

  void cut(Ptr u, Ptr v) {
    evert(u);
    expose(v);
    assert(u->p == v);
    v->l = u->p = nullptr;
    this->update(v);
  }

  void evert(Ptr t) {
    expose(t);
    this->toggle(t);
    this->push(t);
  }

  Ptr lca(Ptr u, Ptr v) {
    if (get_root(u) != get_root(v)) return nullptr;
    expose(u);
    return expose(v);
  }

  Ptr get_kth(Ptr x, int k) {
    expose(x);
    while (x) {
      this->push(x);
      if (x->r && x->r->sz > k) {
        x = x->r;
      } else {
        if (x->r) k -= x->r->sz;
        if (k == 0) return x;
        k -= 1;
        x = x->l;
      }
    }
    return nullptr;
  }

  Ptr get_root(Ptr x) {
    expose(x);
    while (x->l) this->push(x), x = x->l;
    return x;
  }

  Ptr get_parent(Ptr x) {
    expose(x);
    Ptr p = x->l;
    if(p == nullptr) return nullptr;
    while (true) {
      this->push(p);
      if (p->r == nullptr) return p;
      p = p->r;
    }
    exit(1);
  }

  virtual void set_key(Ptr t, const decltype(Node::key)& key) {
    this->splay(t);
    t->key = key;
    this->update(t);
  }

  virtual decltype(Node::key) get_key(Ptr t) { return t->key; }

  decltype(Node::key) fold(Ptr u, Ptr v) {
    evert(u);
    expose(v);
    return v->sum;
  }
};

/**
 * @brief Link/Cut Tree(base)
 * @docs docs/lct/link-cut-tree.md
 */

template <typename T, T (*f)(T, T), T (*ts)(T)>
struct LinkCutTree : LinkCutBase<ReversibleSplayTree<T, f, ts>> {};

/**
 * @brief Link/Cut Tree
 * @docs docs/lct/link-cut-tree.md
 */


namespace my_rand {

// [0, 2^64 - 1)
uint64_t rng() {
  static uint64_t x_ =
      uint64_t(chrono::duration_cast<chrono::nanoseconds>(
                   chrono::high_resolution_clock::now().time_since_epoch())
                   .count()) *
      10150724397891781847ULL;
  x_ ^= x_ << 7;
  return x_ ^= x_ >> 9;
}

// [l, r)
int64_t randint(int64_t l, int64_t r) {
  assert(l < r);
  return l + rng() % (r - l);
}

// choose n numbers from [l, r) without overlapping
vector<int64_t> randset(int64_t l, int64_t r, int64_t n) {
  assert(l <= r && n <= r - l);
  unordered_set<int64_t> s;
  for (int64_t i = n; i; --i) {
    int64_t m = randint(l, r + 1 - i);
    if (s.find(m) != s.end()) m = r - i;
    s.insert(m);
  }
  vector<int64_t> ret;
  for (auto& x : s) ret.push_back(x);
  return ret;
}

// [0.0, 1.0)
double rnd() {
  union raw_cast {
    double t;
    uint64_t u;
  };
  constexpr uint64_t p = uint64_t(1023 - 64) << 52;
  return rng() * ((raw_cast*)(&p))->t;
}

template <typename T>
void randshf(vector<T>& v) {
  int n = v.size();
  for (int loop = 0; loop < 2; loop++)
    for (int i = 0; i < n; i++) swap(v[i], v[randint(0, n)]);
}

}  // namespace my_rand

using my_rand::randint;
using my_rand::randset;
using my_rand::randshf;
using my_rand::rnd;
using my_rand::rng;



struct Timer {
  chrono::high_resolution_clock::time_point st;

  Timer() { reset(); }

  void reset() { st = chrono::high_resolution_clock::now(); }

  chrono::milliseconds::rep elapsed() {
    auto ed = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(ed - st).count();
  }
};
using namespace Nyaan;

constexpr ll N = 400;
constexpr ll M = 1995;
vector<int> X(N), Y(N), A(M), B(M);
vector<int> lower(M);
vector<int> upper(M);

using T = pair<int, int>;
T f(T a, T b) { return max(a, b); }
T ts(T a) { return a; }
using LCT = LinkCutTree<T, f, ts>;

struct Data {
  LCT lct;
  using Node = typename LCT::Node;
  vector<int> kari;
  vector<char> isused;
  vector<Node*> ns;

  // 辺の重みが小さい方を手前側に持つ
  priority_queue<pi, V<pi>, greater<>> unused;
  V<pi> dasita;

  bool same_tree(int u, int v) {
    lct.evert(ns[u]);
    return lct.get_root(ns[v]) == ns[u];
  }

  void link(int u, int v, int e) {
    assert(0 <= e and e < M);
    assert(isused[e] == false);
    assert(A[e] == u or B[e] == u);
    assert(A[e] == v or B[e] == v);
    assert(kari[e] == ns[N + e]->key.first);

    lct.link(ns[N + e], ns[u]);
    lct.link(ns[N + e], ns[v]);
    isused[e] = true;
  }

  void cut(int u, int v, int e) {
    assert(0 <= e and e < M);
    assert(isused[e] == true);
    assert(A[e] == u or B[e] == u);
    assert(A[e] == v or B[e] == v);
    assert(kari[e] == ns[N + e]->key.first);

    lct.cut(ns[N + e], ns[u]);
    lct.cut(ns[N + e], ns[v]);
    isused[e] = false;
  }

  void update_weight(int i, int d) {
    lct.set_key(ns[N + i], make_pair(d, i));
    kari[i] = d;
  }

  void build() {
    vp es;
    rep(i, M) es.emplace_back(kari[i], i);
    sort(all(es));
    UnionFind uf(N);

    each2(d, e, es) {
      int u = A[e], v = B[e];
      if (uf.same(u, v)) {
        unused.emplace(d, e);
      } else {
        uf.unite(u, v);
        link(u, v, e);
      }
    }
  }

  Data() {
    kari.resize(M);
    isused.resize(M, false);
    ns.resize(N + M);
    rep(i, N) { ns[i] = lct.my_new(make_pair(-1, i)); }
    rep(i, M) {
      if (rng() % 13 < 4) {
        kari[i] = lower[i] * 2;
      } else {
        kari[i] = randint(lower[i], upper[i] + 1);
      }
      //*/
      ns[N + i] = lct.my_new(make_pair(kari[i], i));
    }
    build();
  }

  struct Response {
    bool flag;
    bool exists;
    int edge;
    Response(bool f = true, bool ex = true, int e = -1)
        : flag(f), exists(ex), edge(e) {}
  };

  // 使うべきか？
  Response query(const int i, const int d) {
    int u = A[i], v = B[i];
    if (isused[i] == false) {
      // 辺の重み更新
      update_weight(i, 0);
      // パスクエリ
      int score, id;
      tie(score, id) = lct.fold(ns[u], ns[v]);

      if (score <= d) return Response{false, false, id};
      return Response{true, false, id};
    }
    // 使っている場合 -> うーん...
    // 一旦切ってみるか
    cut(u, v, i);
    // 辺の重み更新
    update_weight(i, 0);
    // 今は (i, d) を使う予定
    int kouho_i = -1;
    int kouho_d = inf;

    dasita.clear();
    while (unused.size() and kouho_i == -1) {
      auto [cd, ci] = unused.top();
      unused.pop();
      if (ci <= i or isused[ci]) continue;
      int cu = A[ci], cv = B[ci];
      lct.evert(ns[cu]);
      if (lct.get_root(ns[cv]) != ns[cu]) {
        if (amin(kouho_d, cd)) kouho_i = ci;
      }
      dasita.emplace_back(cd, ci);
    }
    Response res;
    if (d <= kouho_d) {
      res = Response{true, true, kouho_i};
    } else {
      assert(isused[kouho_i] == false);
      res = Response{false, true, kouho_i};
    }
    for (auto& [cd, ci] : dasita) unused.emplace(cd, ci);
    link(u, v, i);
    return res;
  }

  // res.flag : 使うべきか
  // res.exists : 存在するか
  // res.edge : swap の候補となる辺

  void update(int i, Response res, bool flag) {
    assert(isused[i] == res.exists);
    if (flag == res.exists) return;
    int e = res.edge;
    if (flag == true) {
      assert(same_tree(A[e], B[e]));
      assert(i <= e);
      if (i != e) {
        cut(A[e], B[e], e);
        link(A[i], B[i], i);
        unused.emplace(kari[e], e);
      }
    } else {
      assert(same_tree(A[i], B[i]));
      assert(i <= e);
      if (i != e) {
        cut(A[i], B[i], i);
        link(A[e], B[e], e);
      }
    }
  }
};

void Nyaan::solve() {
  Timer timer;
  in2(X, Y);
  in2(A, B);
  rep(i, M) {
    double d = 0;
    int u = A[i], v = B[i];
    d += (X[u] - X[v]) * (X[u] - X[v]);
    d += (Y[u] - Y[v]) * (Y[u] - Y[v]);
    d = sqrt(d);
    lower[i] = llround(d);
    upper[i] = llround(d) * 3;
  }

  auto receive = []() {
    ini(x);
    return x;
  };

  auto send = [](int x) {
    out(x ? 1 : 0);
    cout.flush();
  };

  const int S_START = 37;
  int S = S_START;
  V<Data*> datas;
  V<typename Data::Response> res;
  rep(i, S) datas.push_back(new Data);

  rep(i, M) {
#ifndef NyaanLocal
    ll tm = timer.elapsed();
    if (tm > 1950) S = 11;
    if (tm > 1980) S = 3;
#endif
    int d = receive();
    res.clear();
    int cnt = 0;
    rep(j, S) {
      res.push_back(datas[j]->query(i, d));
      cnt += res.back().flag;
    }
    bool ans = cnt * 2 > S;
    rep(j, S) { datas[j]->update(i, res[j], ans); }
    send(ans);
  }
}