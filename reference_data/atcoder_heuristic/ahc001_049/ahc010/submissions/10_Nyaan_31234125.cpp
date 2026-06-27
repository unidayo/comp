/**
 *  date : 2022-04-24 18:54:38
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
vector<int> mkord(int N,F f) {
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
bool nxp(vector<T> &v) {
  return next_permutation(begin(v), end(v));
}



#define inV(T, v, n) \
  vector<T> v(n);    \
  in(v)

#define inVV(T, v, h, w)                \
  vector<vector<T>> v(h, vector<T>(w)); \
  in(v);

template <typename T>
using minpq = priority_queue<T, vector<T>, greater<T>>;

// 区間：半開区間 (ng, ok] または [ok, ng)
template <typename T, typename F>
T binary_search(T ng, T ok, const F& f) {
  if constexpr (is_integral<T>::value == true) {
    while (abs(ok - ng) > 1) {
      T x = (ok + ng) / 2;
      (f(x) ? ok : ng) = x;
    }
    return ok;
  } else {
    for (int iter = 0; iter < 60; iter++) {
      T x = (ok + ng) / 2;
      (f(x) ? ok : ng) = x;
    }
    return ok;
  }
}

// 解区間 (l, r)
template <typename T, typename F>
void ternary_search(T l, T r, const F& f, bool greater = false) {
  if constexpr (is_integral<T>::value == true) {
    while (abs(l - r) > 2) {
      T llr = (l * 2 + r * 1) / 3;
      T lrr = (l * 1 + r * 2) / 3;
      bool flag = f(llr) < f(lrr);
      if (flag != greater) {
        r = lrr;
      } else {
        l = llr;
      }
    }
    return (l + r) / 2;
  } else {
    for (int iter = 0; iter < 80; iter++) {
      T llr = (l * 2 + r * 1) / 3;
      T lrr = (l * 1 + r * 2) / 3;
      bool flag = f(llr) < f(lrr);
      if (flag != greater) {
        r = lrr;
      } else {
        l = llr;
      }
    }
    return (l + r) / 2;
  }
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

void outr() {}
template <typename T, class... U, char sep = ' '>
void outr(const T &t, const U &...u) {
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

void dump(__int128_t t) {
  if (t == 0) cerr << 0;
  if (t < 0) cerr << '-', t = -t;
  string S;
  while (t) S.push_back('0' + t % 10), t /= 10;
  reverse(begin(S), end(S));
  cerr << S;
}

void dump(__uint128_t t) {
  if (t == 0) cerr << 0;
  string S;
  while (t) S.push_back('0' + t % 10), t /= 10;
  reverse(begin(S), end(S));
  cerr << S;
}

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
vector<long long> randset(int64_t l, int64_t r, int64_t n) {
  assert(l <= r && n <= r - l);
  unordered_set<int64_t> s;
  for (int64_t i = n; i; --i) {
    int64_t m = randint(l, r + 1 - i);
    if (s.find(m) != s.end()) m = r - i;
    s.insert(m);
  }
  vector<long long> ret;
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

constexpr int N = 30;
int tiles[32][32];
int ok(int i, int j) { return 0 <= i and i < N and 0 <= j and j < N; }
int di[4] = {0, -1, 0, 1};
int dj[4] = {-1, 0, 1, 0};
int to[8][4] = {
    {1, 0, -1, -1}, {3, -1, -1, 0}, {-1, -1, 3, 2}, {-1, 2, 1, -1},
    {1, 0, 3, 2},   {3, 2, 1, 0},   {2, -1, 0, -1}, {-1, 3, -1, 1},
};
int ans[N][N];
int get(int i, int j) {
  if (tiles[i][j] < 4) {
    return (tiles[i][j] + ans[i][j]) & 3;
  } else {
    return tiles[i][j] ^ (ans[i][j] & 1);
  }
}
void output_answer() {
  rep(i, N) rep(j, N) cout << (ans[i][j] % 4);
  cout << "\n";
  cout.flush();
};

int consists_loops[N][N];
ll calc_loops2(int si, int sj, int sd) {
  int i = si;
  int j = sj;
  int d = sd;
  int length = 0;
  memset(consists_loops, 0, sizeof(consists_loops));
  while (true) {
    consists_loops[i][j] = 1;
    // trc(i,j,d,tiles[i][j]);
    int d2 = to[get(i, j)][d];
    // trc(d2);
    if (d2 == -1) return 0;
    i += di[d2];
    j += dj[d2];
    d = (d2 + 2) % 4;
    length += 1;
    // trc(i,j,d,length);
    if (!ok(i, j)) return 0;
    if (i == si && j == sj && d == sd) return length;
  }
}

bool calced[32][32][4];
int calc_score() {
  rep(i, N) rep(j, N) rep(k, 4) calced[i][j][k] = false;
  auto calc_loops = [&](int si, int sj, int sd) {
    int i = si;
    int j = sj;
    int d = sd;
    int length = 0;
    while (true) {
      // trc(i,j,d,tiles[i][j]);
      int d2 = to[get(i, j)][d];
      // trc(d2);
      if (d2 == -1) return 0;
      calced[i][j][d] = calced[i][j][d2] = true;
      i += di[d2];
      j += dj[d2];
      d = (d2 + 2) % 4;
      length += 1;
      // trc(i,j,d,length);
      if (!ok(i, j)) return 0;
      if (i == si && j == sj && d == sd) return length;
    }
  };
  priority_queue<ll, vl, greater<>> Q;
  rep(i, 30) rep(j, 30) rep(k, 4) {
    if (calced[i][j][k]) continue;
    ll l = calc_loops(i, j, k);
    if (l != 0) Q.push(l);
    if (sz(Q) > 2) Q.pop();
  }
  if (sz(Q) < 2) return 0;
  ll sc = 1;
  rep(i, 2) {
    sc *= Q.top();
    Q.pop();
  }
  return sc;
}

// 半面ずつ最適化する

// ある地点からスタートして、できるだけ巨大なパスを作る
// [L,R) [U,D) の点のみ使える

void make_large_path(int U, int L, int D, int R) {
  int vis[N][N];
  int best[N][N];
  memcpy(best, ans, sizeof(ans));
  ll best_score = 0;
  int best_si = 0, best_sj = 0, best_sd = 0;
  Timer timer2;
  constexpr int PAD = 1;
  // int kinkyuukaihi = 0;
  while (timer2.elapsed() < 965 * PAD) {
    int time_upper = timer2.elapsed() < 700 * PAD ? 3 * PAD : 10 * PAD;
    // 下から入ってくる
    // int uso = 0;

    int si = randint(U + 1, D - 1);
    int sj = randint(L + 1, R - 1);
    if (time_upper == 3) {
      ans[si][sj] += randint(0, 4);
    } 
    /*
    else {
      int UD = (U + D) / 2;
      int LR = (L + R) / 2;
      si = randint(UD - 4, UD + 4);
      sj = randint(LR - 4, LR + 4);
    }
    */
    int sd = 0;
    while (to[get(si, sj)][sd] == -1) sd++;
    Timer timer;

    auto upd = [&](ll length) {
      if (amax(best_score, length)) {
        memcpy(best, ans, sizeof(ans));
        best_si = si;
        best_sj = sj;
        best_sd = sd;
      }
      /*
      if (best_score < length) {
        if (length < 1e9) {
          amax(best_score, length);
        } else {
          ll loops = calc_loops2(si, sj, sd);
          if (loops + 1e9 == length) {
            amax(best_score, length);
            memcpy(best, ans, sizeof(ans));
          } else {
            uso++;
            kinkyuukaihi++;
          }
        }
      }
      */
    };
    int loop = 0;
    auto dfs = [&](auto rc, int i, int j, int d, ll length) -> void {
      if ((loop & 1023) == 1023 and timer.elapsed() > time_upper) return;
      // if (uso > 10) return;
      ++loop;
      int d2 = to[get(i, j)][d];
      if (d2 == -1) return;
      int ni = i + di[d2];
      int nj = j + dj[d2];
      int nd = (d2 + 2) & 3;
      length++;
      if (ni == si and nj == sj and nd == sd) {
        upd(length + 1e9);
        return;
      } else {
        upd(length);
      }
      bool ok = U <= ni and ni < D and L <= nj and nj < R;
      if (!ok) return;
      if (vis[ni][nj] and to[get(ni, nj)][nd] == -1) return;
      int dans = rng() % 2 ? 1 : 3;
      rep(rot, 4) {
        if (to[get(ni, nj)][nd] != -1) {
          vis[ni][nj] += 1;
          rc(rc, ni, nj, nd, length);
          vis[ni][nj] -= 1;
        }
        if (ni == si and nj == sj) break;
        if (vis[ni][nj]) break;
        ans[ni][nj] += dans;
      }
    };
    timer.reset();
    rep(i, N) rep(j, N) vis[i][j] = 0;
    dfs(dfs, si, sj, sd, 0);
    // cerr << get(si, sj) << endl;
    // cerr << "start : " << si << " " << sj << " " << sd << endl;
    // cerr << "debug : " << best_score << " " << loop << endl;

    timer.reset();
    rep(i, N) rep(j, N) vis[i][j] = 0;
    sd = to[get(si, sj)][sd];
    dfs(dfs, si, sj, sd, 0);
    memcpy(ans, best, sizeof(ans));

    // cerr << "start : " << si << " " << sj << " " << sd << endl;
    // cerr << "debug : " << best_score << " " << loop << endl;
  }
  cerr << "debug : " << best_score << endl;
  memcpy(ans, best, sizeof(ans));

  calc_loops2(best_si, best_sj, best_sd);
}

void make_large_path_2(int U, int L, int D, int R) {
  int vis[N][N];
  int best[N][N];
  memcpy(best, ans, sizeof(ans));
  ll best_score = 0;
  int best_si = 0, best_sj = 0, best_sd = 0;
  Timer timer2;
  constexpr int PAD = 1;
  // int kinkyuukaihi = 0;
  while (timer2.elapsed() < 970 * PAD) {
    int time_upper = timer2.elapsed() < 700 * PAD ? 3 * PAD : 10 * PAD;
    // 下から入ってくる
    // int uso = 0;

    int si = randint(U + 1, D - 1);
    int sj = randint(L + 1, R - 1);
    if (time_upper == 3) {
      ans[si][sj] += randint(0, 4);
    } 
    /*
    else {
      int UD = (U + D) / 2;
      int LR = (L + R) / 2;
      si = randint(UD - 4, UD + 4);
      sj = randint(LR - 4, LR + 4);
    }
    */
    
    int sd = 0;
    while (to[get(si, sj)][sd] == -1) sd++;
    Timer timer;

    auto upd = [&](ll length) {
      if (amax(best_score, length)) {
        memcpy(best, ans, sizeof(ans));
        best_si = si;
        best_sj = sj;
        best_sd = sd;
      }
      /*
      if (best_score < length) {
        if (length < 1e9) {
          amax(best_score, length);
        } else {
          ll loops = calc_loops2(si, sj, sd);
          if (loops + 1e9 == length) {
            amax(best_score, length);
            memcpy(best, ans, sizeof(ans));
          } else {
            uso++;
            kinkyuukaihi++;
          }
        }
      }
      */
    };
    int loop = 0;
    auto dfs = [&](auto rc, int i, int j, int d, ll length) -> void {
      if ((loop & 1023) == 1023 and timer.elapsed() > time_upper) return;
      // if (uso > 10) return;
      ++loop;
      int d2 = to[get(i, j)][d];
      if (d2 == -1) return;
      int ni = i + di[d2];
      int nj = j + dj[d2];
      int nd = (d2 + 2) & 3;
      length++;
      if (ni == si and nj == sj and nd == sd) {
        upd(length + 1e9);
        return;
      } else {
        upd(length);
      }
      bool okflag = ok(ni, nj);
      if (!okflag) return;
      if (consists_loops[ni][nj]) return;
      if (vis[ni][nj] and to[get(ni, nj)][nd] == -1) return;
      int dans = rng() % 2 ? 1 : 3;
      rep(rot, 4) {
        if (to[get(ni, nj)][nd] != -1) {
          vis[ni][nj] += 1;
          rc(rc, ni, nj, nd, length);
          vis[ni][nj] -= 1;
        }
        if (ni == si and nj == sj) break;
        if (vis[ni][nj]) break;
        ans[ni][nj] += dans;
      }
    };
    timer.reset();
    rep(i, N) rep(j, N) vis[i][j] = 0;
    dfs(dfs, si, sj, sd, 0);
    // cerr << get(si, sj) << endl;
    // cerr << "start : " << si << " " << sj << " " << sd << endl;
    // cerr << "debug : " << best_score << " " << loop << endl;

    timer.reset();
    rep(i, N) rep(j, N) vis[i][j] = 0;
    sd = to[get(si, sj)][sd];
    dfs(dfs, si, sj, sd, 0);
    memcpy(ans, best, sizeof(ans));

    // cerr << "start : " << si << " " << sj << " " << sd << endl;
    // cerr << "debug : " << best_score << " " << loop << endl;
  }
  cerr << "debug : " << best_score << endl;
  memcpy(ans, best, sizeof(ans));

  calc_loops2(best_si, best_sj, best_sd);
}

void q() {
  rep(i, 30) {
    ins(S);
    rep(j, 30) tiles[i][j] = S[j] - '0';
  }

  make_large_path(0, 0, 15, 30);
  make_large_path_2(15, 0, 30, 30);

  ll score = calc_score();
  cerr << "score : " << score << endl;
  output_answer();
}

void Nyaan::solve() {
  int T = 1;
  // in(T);
  while (T--) q();
}