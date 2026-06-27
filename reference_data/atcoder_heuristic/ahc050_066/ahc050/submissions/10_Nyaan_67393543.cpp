/**
 * date   : 2025-07-06 22:23:43
 * author : Nyaan
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
#include <tr2/dynamic_bitset>
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
template <typename T>
using minpq = priority_queue<T, vector<T>, greater<T>>;

template <typename T, typename U>
struct P : pair<T, U> {
  template <typename... Args>
  constexpr P(Args... args) : pair<T, U>(args...) {}

  using pair<T, U>::first;
  using pair<T, U>::second;

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
  template <typename S>
  P &operator*=(const S &r) {
    first *= r, second *= r;
    return *this;
  }
  P operator+(const P &r) const { return P(*this) += r; }
  P operator-(const P &r) const { return P(*this) -= r; }
  P operator*(const P &r) const { return P(*this) *= r; }
  template <typename S>
  P operator*(const S &r) const {
    return P(*this) *= r;
  }
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

vector<int> mkiota(int n) {
  vector<int> ret(n);
  iota(begin(ret), end(ret), 0);
  return ret;
}

template <typename T>
T mkrev(const T &v) {
  T w{v};
  reverse(begin(w), end(w));
  return w;
}

template <typename T>
bool nxp(T &v) {
  return next_permutation(begin(v), end(v));
}

// 返り値の型は入力の T に依存
// i 要素目 : [0, a[i])
template <typename T>
vector<vector<T>> product(const vector<T> &a) {
  vector<vector<T>> ret;
  vector<T> v;
  auto dfs = [&](auto rc, int i) -> void {
    if (i == (int)a.size()) {
      ret.push_back(v);
      return;
    }
    for (int j = 0; j < a[i]; j++) v.push_back(j), rc(rc, i + 1), v.pop_back();
  };
  dfs(dfs, 0);
  return ret;
}

// F : function(void(T&)), mod を取る操作
// T : 整数型のときはオーバーフローに注意する
template <typename T>
T Power(T a, long long n, const T &I, const function<void(T &)> &f) {
  T res = I;
  for (; n; f(a = a * a), n >>= 1) {
    if (n & 1) f(res = res * a);
  }
  return res;
}
// T : 整数型のときはオーバーフローに注意する
template <typename T>
T Power(T a, long long n, const T &I = T{1}) {
  return Power(a, n, I, function<void(T &)>{[](T &) -> void {}});
}

template <typename T>
T Rev(const T &v) {
  T res = v;
  reverse(begin(res), end(res));
  return res;
}

template <typename T>
vector<T> Transpose(const vector<T> &v) {
  using U = typename T::value_type;
  if(v.empty()) return {};
  int H = v.size(), W = v[0].size();
  vector res(W, T(H, U{}));
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      res[j][i] = v[i][j];
    }
  }
  return res;
}

template <typename T>
vector<T> Rotate(const vector<T> &v, int clockwise = true) {
  using U = typename T::value_type;
  int H = v.size(), W = v[0].size();
  vector res(W, T(H, U{}));
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      if (clockwise) {
        res[W - 1 - j][i] = v[i][j];
      } else {
        res[j][H - 1 - i] = v[i][j];
      }
    }
  }
  return res;
}

}  // namespace Nyaan


// bit operation

namespace Nyaan {
__attribute__((target("popcnt"))) inline int popcnt(const u64 &a) {
  return __builtin_popcountll(a);
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

istream &operator>>(istream &is, __int128_t &x) {
  string S;
  is >> S;
  x = 0;
  int flag = 0;
  for (auto &c : S) {
    if (c == '-') {
      flag = true;
      continue;
    }
    x *= 10;
    x += c - '0';
  }
  if (flag) x = -x;
  return is;
}

istream &operator>>(istream &is, __uint128_t &x) {
  string S;
  is >> S;
  x = 0;
  for (auto &c : S) {
    x *= 10;
    x += c - '0';
  }
  return is;
}

ostream &operator<<(ostream &os, __int128_t x) {
  if (x == 0) return os << 0;
  if (x < 0) os << '-', x = -x;
  string S;
  while (x) S.push_back('0' + x % 10), x /= 10;
  reverse(begin(S), end(S));
  return os << S;
}
ostream &operator<<(ostream &os, __uint128_t x) {
  if (x == 0) return os << 0;
  string S;
  while (x) S.push_back('0' + x % 10), x /= 10;
  reverse(begin(S), end(S));
  return os << S;
}

void in() {}
template <typename T, class... U>
void in(T &t, U &...u) {
  cin >> t;
  in(u...);
}

void out() { cout << "\n"; }
template <typename T, class... U, char sep = ' '>
void out(const T &t, const U &...u) {
  cout << t;
  if (sizeof...(u)) cout << sep;
  out(u...);
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


#ifdef NyaanDebug
#define trc(...) (void(0))
#else
#define trc(...) (void(0))
#endif

#ifdef NyaanLocal
#define trc2(...) (void(0))
#else
#define trc2(...) (void(0))
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

  // f(x, y) : x に y をマージ
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







using namespace std;

namespace internal {
unsigned long long non_deterministic_seed() {
  unsigned long long m =
      chrono::duration_cast<chrono::nanoseconds>(
          chrono::high_resolution_clock::now().time_since_epoch())
          .count();
  m ^= 9845834732710364265uLL;
  m ^= m << 24, m ^= m >> 31, m ^= m << 35;
  return m;
}
unsigned long long deterministic_seed() { return 88172645463325252UL; }

// 64 bit の seed 値を生成 (手元では seed 固定)
// 連続で呼び出すと同じ値が何度も返ってくるので注意
// #define RANDOMIZED_SEED するとシードがランダムになる
unsigned long long seed() {
#if defined(DETERMINISTIC_SEED)
  return deterministic_seed();
#elif defined(NyaanLocal) && !defined(RANDOMIZED_SEED)
  return deterministic_seed();
#else
  return non_deterministic_seed();
#endif
}

}  // namespace internal


namespace my_rand {
using i64 = long long;
using u64 = unsigned long long;

// [0, 2^64 - 1)
u64 rng() {
  static u64 _x = internal::seed();
  return _x ^= _x << 7, _x ^= _x >> 9;
}

// [l, r]
i64 rng(i64 l, i64 r) {
  assert(l <= r);
  return l + rng() % u64(r - l + 1);
}

// [l, r)
i64 randint(i64 l, i64 r) {
  assert(l < r);
  return l + rng() % u64(r - l);
}

// choose n numbers from [l, r) without overlapping
vector<i64> randset(i64 l, i64 r, i64 n) {
  assert(l <= r && n <= r - l);
  unordered_set<i64> s;
  for (i64 i = n; i; --i) {
    i64 m = randint(l, r + 1 - i);
    if (s.find(m) != s.end()) m = r - i;
    s.insert(m);
  }
  vector<i64> ret;
  for (auto& x : s) ret.push_back(x);
  sort(begin(ret), end(ret));
  return ret;
}

// [0.0, 1.0)
double rnd() { return rng() * 5.42101086242752217004e-20; }
// [l, r)
double rnd(double l, double r) {
  assert(l < r);
  return l + rnd() * (r - l);
}

template <typename T>
void randshf(vector<T>& v) {
  int n = v.size();
  for (int i = 1; i < n; i++) swap(v[i], v[randint(0, i + 1)]);
}

}  // namespace my_rand

using my_rand::randint;
using my_rand::randset;
using my_rand::randshf;
using my_rand::rnd;
using my_rand::rng;





using namespace std;

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


using namespace Nyaan;

constexpr int N = 40;
int M;
char S[N][N + 1];
Timer timer_from_start;

using Real = float;

struct State {
  char stone[N][N];
  Real p[N][N];
  int stone_num = 0;

  State() {
    memset((void*)stone, 0, sizeof(char) * N * N);
    memset((void*)p, 0, sizeof(Real) * N * N);
  }

  void fill_next_state(State& s) const {
    memcpy(s.stone, stone, sizeof(char) * N * N);
    memset((void*)s.p, 0, sizeof(Real) * N * N);
    s.stone_num = stone_num;

#ifdef NyaanDebug
    rep(i, N) rep(j, N) {
      if (stone[i][j] == 1) assert(p[i][j] == 0);
    }
#endif

    for (int i = 0; i < N; i++) {
      Real x = 0;
      for (int j = 0; j < N; j++) {
        x += p[i][j];
        if (j == N - 1 or stone[i][j + 1]) {
          s.p[i][j] += x;
          x = 0;
        }
      }
      for (int j = N - 1; j >= 0; j--) {
        x += p[i][j];
        if (j == 0 or stone[i][j - 1]) {
          s.p[i][j] += x;
          x = 0;
        }
      }
    }
    for (int j = 0; j < N; j++) {
      Real x = 0;
      for (int i = 0; i < N; i++) {
        x += p[i][j];
        if (i == N - 1 or stone[i + 1][j]) {
          s.p[i][j] += x;
          x = 0;
        }
      }
      for (int i = N - 1; i >= 0; i--) {
        x += p[i][j];
        if (i == 0 or stone[i - 1][j]) {
          s.p[i][j] += x;
          x = 0;
        }
      }
    }
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        s.p[i][j] *= Real{0.25};
      }
    }
  }

  void put_stone(int i, int j) {
    stone[i][j] = 1;
    p[i][j] = 0;
    stone_num++;
  }

  Real variance() {
    int n = N * N - stone_num;
    if (n == 0) return 0;
    Real s = 0;
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        if (stone[i][j] == 0) {
          Real x = p[i][j] * p[i][j];
          s += x * x;
        }
      }
    }
    return s;
  }
};

Real _buf[2][N * N];
int n0[N * N], n1[N * N], n2[N * N], n3[N * N];
tuple<Real, int> memo[N * N];
Real get_X_after(State dp, int X, bool strict = false, int C = 100) {
  if (strict) {
    State nx = dp, buf;
    rep(x, X) {
      nx.fill_next_state(buf);
      swap(nx, buf);
    }
    return nx.variance();
  }
  // 次の状態を調べる
  for (int i = 0; i < N; i++) {
    n0[i * N] = i * N;
    for (int j = 1; j < N; j++) {
      n0[i * N + j] = dp.stone[i][j - 1] ? i * N + j : n0[i * N + j - 1];
    }
    n1[i * N + N - 1] = i * N + N - 1;
    for (int j = N - 2; j >= 0; j--) {
      n1[i * N + j] = dp.stone[i][j + 1] ? i * N + j : n1[i * N + j + 1];
    }
  }
  for (int j = 0; j < N; j++) {
    n2[0 * N + j] = j;
    for (int i = 1; i < N; i++) {
      n2[i * N + j] = dp.stone[i - 1][j] ? i * N + j : n2[(i - 1) * N + j];
    }
    n3[(N - 1) * N + j] = (N - 1) * N + j;
    for (int i = N - 2; i >= 0; i--) {
      n3[i * N + j] = dp.stone[i + 1][j] ? i * N + j : n3[(i + 1) * N + j];
    }
  }
  Real *f = _buf[0], *g = _buf[1];

  /*
  if (strict) {
    rep(i, N) rep(j, N) f[i * N + j] = dp.p[i][j];
    rep(x, X) {
      memset(g, 0, sizeof(Real) * N * N);
      rep(n, N * N) {
        g[n0[n]] += f[n];
        g[n1[n]] += f[n];
        g[n2[n]] += f[n];
        g[n3[n]] += f[n];
      }
      swap(f, g);
    }
    Real res = 0;
    rep(n, N * N) res += f[n] * f[n];
    rep(x, X) res *= 0.25 * 0.25;
    return res;
  }
  */

  rep(i, N) rep(j, N) memo[i * N + j] = make_tuple(dp.p[i][j], i * N + j);
  nth_element(memo, memo + C, memo + N * N,
              [](auto& t1, auto& t2) { return get<0>(t1) > get<0>(t2); });
  while (C >= 0 and get<0>(memo[C - 1]) <= 1e-9) C--;

  rep(z, C) f[get<1>(memo[z])] = get<0>(memo[z]);
  rep(x, X) {
    rep(z, C) g[get<1>(memo[z])] = 0;
    rep(z, C) {
      int n = get<1>(memo[z]);
      g[n0[n]] += f[n];
      g[n1[n]] += f[n];
      g[n2[n]] += f[n];
      g[n3[n]] += f[n];
    }
    swap(f, g);
  }
  Real res = 0;
  rep(z, C) res += f[get<1>(memo[z])] * f[get<1>(memo[z])];
  rep(x, X) res *= 0.25 * 0.25;
  return res;
}

pair<V<int16_t>, Real> naive(State dp) {
  State nx, buf;
  V<int16_t> ans;
  Real reward = 0;

  rep(t, N * N - M) {
    int best = -1;
    Real p = inf, v = -inf;

    dp.fill_next_state(nx);
    swap(dp, nx);

    if (timer_from_start() >= 1940) {
      rep(i, N) rep(j, N) {
        if (dp.stone[i][j]) continue;
        if (amin(p, dp.p[i][j])) best = i * N + j;
      }
    } else {
      bool flag = timer_from_start() <= 1700;
      rep(i, N) rep(j, N) {
        if (dp.stone[i][j]) continue;
        if (dp.p[i][j] <= p) {
          if (t < 800 and rng(0, 9)) continue;
          nx = dp;
          nx.put_stone(i, j);
          /*
          rep(_, X) {
            nx.fill_next_state(buf);
            swap(nx, buf);
          }
          Real va = nx.variance();
          */
          Real va = get_X_after(nx, flag ? 20 : 5, flag);
          if (dp.p[i][j] < p or va > v) {
            best = i * N + j;
            p = dp.p[i][j];
            v = va;
          }
        }
      }
    }
    /*
    auto trans = get_next_state(dp);
    randshf(trans);
    for (auto& [i, j, curp, curv] : trans) {
      if (curp < p or (curp == p and curv > v)) {
        best = i * N + j;
        p = curp;
        v = curv;
      }
    }
    */
    /*
    if (timer_from_start() > 1900) {
      rep(i, N) rep(j, N) {
        if (dp.stone[i][j]) continue;
        if (amin(p, dp.p[i][j])) best = i * N + j;
      }
    } else {
      randshf(ps);
    }
    */
    int i = best / N, j = best % N;
    ans.push_back(best);
    reward += dp.p[i][j] * t;
    dp.put_stone(i, j);

    if (t % 100 == 0) trc2(t, timer_from_start());
  }
  return {ans, reward};
}

void q() {
  timer_from_start.reset();

  ini(dummy);
  in(M);
  rep(i, N) in(S[i]);
  State dp;
  rep(i, N) rep(j, N) {
    if (S[i][j] == '#') {
      dp.put_stone(i, j);
    } else {
      dp.p[i][j] = Real(1.0) / (N * N - M);
    }
  }

  V<int16_t> ans;
  Real reward = 0;
  do {
    auto [nans, nreward] = naive(dp);
    if (amax(reward, nreward)) ans = nans;
  } while (0);

  each(i, ans) out(i / N, i % N);
  cerr << "Time : " << timer_from_start() << ", ";
  cerr << "Score : " << (1e6 * reward / (N * N - M - 1)) << endl;
}

void Nyaan::solve() {
  int t = 1;
  // in(t);
  while (t--) q();
}