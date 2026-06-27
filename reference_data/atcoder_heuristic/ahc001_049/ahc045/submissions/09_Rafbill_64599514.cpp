// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
 #include <ext/pb_ds/assoc_container.hpp>
using namespace std;
// Macros
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
template <class T> using min_queue = priority_queue<T, vector<T>, greater<T>>;
template <class T> using max_queue = priority_queue<T>;
struct uint64_hash {
  static inline uint64_t rotr(uint64_t x, unsigned k) {
    return (x >> k) | (x << (8U * sizeof(uint64_t) - k));
  }
  static inline uint64_t hash_int(uint64_t x) noexcept {
    auto h1 = x * (uint64_t)(0xA24BAED4963EE407);
    auto h2 = rotr(x, 32U) * (uint64_t)(0x9FB21C651E98DF25);
    auto h = rotr(h1 + h2, 32U);
    return h;
  }
  size_t operator()(uint64_t x) const {
    static const uint64_t FIXED_RANDOM = std::chrono::steady_clock::now().time_since_epoch().count();
    return hash_int(x + FIXED_RANDOM);
  }
};
template <typename K, typename V, typename Hash = uint64_hash>
using hash_map = __gnu_pbds::gp_hash_table<K, V, Hash>;
template <typename K, typename Hash = uint64_hash>
using hash_set = hash_map<K, __gnu_pbds::null_type, Hash>;
// Types
template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T>
using max_queue = priority_queue<T>;
// Printing
template<class T>
void print_collection(ostream& out, T const& x);
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>);
namespace std {
  template<class... A>
  ostream& operator<<(ostream& out, tuple<A...> const& x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
  }
  template<class... A>
  ostream& operator<<(ostream& out, pair<A...> const& x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
  }
  template<class A, size_t N>
  ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, deque<A> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, multiset<A> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, multimap<A, B> const& x) { print_collection(out, x); return out; }
  template<class A>
  ostream& operator<<(ostream& out, set<A> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, map<A, B> const& x) { print_collection(out, x); return out; }
  template<class A, class B>
  ostream& operator<<(ostream& out, unordered_set<A> const& x) { print_collection(out, x); return out; }
}
template<class T, size_t... I>
void print_tuple(ostream& out, T const& a, index_sequence<I...>){
  using swallow = int[];
  out << '(';
  (void)swallow{0, (void(out << (I == 0? "" : ", ") << get<I>(a)), 0)...};
  out << ')';
}
template<class T>
void print_collection(ostream& out, T const& x) {
  int f = 0;
  out << '[';
  for(auto const& i: x) {
    out << (f++ ? "," : "");
    out << i;
  }
  out << "]";
}
// Random
struct RNG {
  uint64_t s[2];
  RNG(u64 seed) {
    reset(seed);
  }
  RNG() {
    reset(time(0));
  }
  using result_type = u32;
  constexpr u32 min(){ return numeric_limits<u32>::min(); }
  constexpr u32 max(){ return numeric_limits<u32>::max(); }
  u32 operator()() { return randomInt32(); }
  static __attribute__((always_inline)) inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
  }
  inline void reset(u64 seed) {
    struct splitmix64_state {
      u64 s;
      u64 splitmix64() {
        u64 result = (s += 0x9E3779B97f4A7C15);
        result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
        result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
        return result ^ (result >> 31);
      }
    };
    splitmix64_state sm { seed };
    s[0] = sm.splitmix64();
    s[1] = sm.splitmix64();
  }
  uint64_t next() {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];
    const uint64_t result = rotl(s0 * 5, 7) * 9;
    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    s[1] = rotl(s1, 37); // c
    return result;
  }
  inline u32 randomInt32() {
    return next();
  }
  inline u64 randomInt64() {
    return next();
  }
  inline u32 random32(u32 r) {
    return (((u64)randomInt32())*r)>>32;
  }
  inline u64 random64(u64 r) {
    return randomInt64()%r;
  }
  inline u32 randomRange32(u32 l, u32 r) {
    return l + random32(r-l+1);
  }
  inline u64 randomRange64(u64 l, u64 r) {
    return l + random64(r-l+1);
  }
  inline double randomDouble() {
    return (double)randomInt32() / 4294967296.0;
  }
  inline float randomFloat() {
    return (float)randomInt32() / 4294967296.0;
  }
  inline double randomRangeDouble(double l, double r) {
    return l + randomDouble() * (r-l);
  }
  template<class T>
  void shuffle(vector<T>& v) {
    i32 sz = v.size();
    for(i32 i = sz; i > 1; i--) {
      i32 p = random32(i);
      swap(v[i-1],v[p]);
    }
  }
  template<class T>
  void shuffle(T* fr, T* to) {
    i32 sz = distance(fr,to);
    for(int i = sz; i > 1; i--) {
      int p = random32(i);
      swap(fr[i-1],fr[p]);
    }
  }
  template<class T>
  inline int sample_index(vector<T> const& v) {
    return random32(v.size());
  }
  template<class T>
  inline T sample(vector<T> const& v) {
    return v[sample_index(v)];
  }
} rng;
// Timer
struct timer {
  chrono::high_resolution_clock::time_point t_begin;
  timer() {
    t_begin = chrono::high_resolution_clock::now();
  }
  void reset() {
    t_begin = chrono::high_resolution_clock::now();
  }
  float elapsed() const {
    return chrono::duration<float>(chrono::high_resolution_clock::now() - t_begin).count();
  }
};
// Util
template<class T>
T& smin(T& x, T const& y) { x = min(x,y); return x; }
template<class T>
T& smax(T& x, T const& y) { x = max(x,y); return x; }
template<typename T>
int sgn(T val) {
  if(val < 0) return -1;
  if(val > 0) return 1;
  return 0;
}
static inline
string int_to_string(int val, int digits = 0) {
  string s = to_string(val);
  reverse(begin(s), end(s));
  while((int)s.size() < digits) s.push_back('0');
  reverse(begin(s), end(s));
  return s;
}
// Debug
static inline void debug_impl_seq() {
  cerr << "}";
}
template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}
// Bits
__attribute__((always_inline)) inline
u64 bit(u64 x) { return 1ull<<x; }
__attribute__((always_inline)) inline
void setbit(u64& a, u32 b, u64 value = 1) {
  a = (a&~bit(b)) | (value<<b);
}
__attribute__((always_inline)) inline
u64 getbit(u64 a, u32 b) {
  return (a>>b)&1;
}
__attribute__((always_inline)) inline
u64 lsb(u64 a) {
  return __builtin_ctzll(a);
}
__attribute__((always_inline)) inline
int msb(uint64_t bb) {
  return __builtin_clzll(bb) ^ 63;
}

namespace blossom {
  const i64 N = 1003;
  const i64 inf = (i64)1e18;
  struct Q{ i64 u, v, w; } e[N][N];
  vector<i64> p[N];
  i64 n, m = 0, id, h, t, lk[N], sl[N], st[N], f[N], b[N][N], s[N], ed[N], q[N], lab[N];
  void upd(i64 u, i64 v) { if (!sl[v] || (lab[e[u][v].u] + lab[e[u][v].v] - 2 * e[e[u][v].u][e[u][v].v].w) < (lab[e[sl[v]][v].u] + lab[e[sl[v]][v].v] - 2 * e[e[sl[v]][v].u][e[sl[v]][v].v].w)) sl[v] = u; }
  void ss(i64 v) {
    sl[v] = 0;
    for (i64 u = 1; u <= n; ++u) if (e[u][v].w > 0 && st[u] != v && !s[st[u]]) upd(u, v);
  }
  void ins(i64 u){ if (u <= n) q[++t] = u; else for (i64 v : p[u]) ins(v); }
  void ch(i64 u, i64 w) { st[u] = w; if (u > n) for (i64 v : p[u]) ch(v, w); }
  i64 gr(i64 u, i64 v) {
    if ((v = find(begin(p[u]), end(p[u]), v) - p[u].begin()) & 1) {
      reverse(1 + begin(p[u]), end(p[u]));
      return (i64)p[u].size() - v;
    }
    return v;
  }
  void stm(i64 u, i64 v) {
    lk[u] = e[u][v].v;
    if (u <= n) return; Q w = e[u][v];
    i64 x = b[u][w.u], y = gr(u,x);
    for (i64 i = 0; i < y; ++i) stm(p[u][i], p[u][i^1]);
    stm(x, v); rotate(p[u].begin(), y+begin(p[u]), end(p[u]));
  }
  void aug(i64 u, i64 v) {
    i64 w = st[lk[u]];stm(u, v);if (!w) return;
    stm(w, st[f[w]]);
    aug(st[f[w]], w);
  }
  i64 lca(i64 u, i64 v) {
    for (id++; u|v; swap(u, v)) {
      if (!u) continue;if(ed[u] == id) return u;
      ed[u] = id; if((u = st[lk[u]])) u = st[f[u]]; // =, not ==
    }
    return 0;
  }
  void add(i64 u, i64 a, i64 v) {
    i64 x = n + 1; while (x <= m && st[x]) ++x;
    if (x > m) ++m;
    lab[x] = s[x] = st[x] = 0;
    lk[x] = lk[a];
    p[x].clear();
    p[x].push_back(a);
    for (i64 i = u, j = 0; i != a; i=st[f[j]]) p[x].push_back(i), p[x].push_back(j=st[lk[i]]), ins(j); reverse(1+begin(p[x]), end(p[x]));for (i64 i = v, j = 0; i != a; i=st[f[j]]) p[x].push_back(i), p[x].push_back(j=st[lk[i]]), ins(j);
    ch(x, x); for (i64 i = 1; i <= m; ++i) e[x][i].w = e[i][x].w = 0;
    fill(b[x], b[x]+n+1, 0);
    for (i64 u : p[x]) {
      for (i64 v = 1; v <= m; ++v) if (!e[x][v].w || (lab[e[u][v].u] + lab[e[u][v].v] - 2 * e[e[u][v].u][e[u][v].v].w) < (lab[e[x][v].u] + lab[e[x][v].v] - 2 * e[e[x][v].u][e[x][v].v].w)) e[x][v] = e[u][v], e[v][x] = e[v][u];
      for (i64 v = 1; v <= n; ++v) if (b[u][v]) b[x][v] = u;
    }
    ss(x);
  }
  void ex(i64 u) {
    for (i64 x : p[u]) ch(x, x);
    i64 a = b[u][e[u][f[u]].u], r = gr(u, a);
    for (i64 i = 0; i < r; i += 2) {
      i64 x = p[u][i], y = p[u][i + 1];
      f[x] = e[y][x].u; s[x] = 1; s[y] = 0; sl[x] = 0; ss(y); ins(y);
    }
    s[a] = 1; f[a] = f[u];
    for (i64 i = r + 1; i < (i64)p[u].size(); ++i) s[p[u][i]] = -1, ss(p[u][i]);
    st[u] = 0;
  }
  bool on(const Q &e) {
    i64 u = st[e.u], v = st[e.v], a;
    if (s[v] == -1) {
      f[v] = e.u, s[v] = 1, a = st[lk[v]], sl[v] = sl[a] = s[a] = 0, ins(a);
    } else if (!s[v]) {
      a = lca(u, v); if (!a) return aug(u, v), aug(v, u), 1; else add(u, a, v);
    }
    return 0;
  }
  bool bfs() {
    fill(s, s+m+1, -1);fill(sl, sl+m+1, 0); // s is filled with -1
    h = 1, t = 0; for (i64 i = 1; i <= m; ++i) if (st[i] == i && !lk[i]) f[i] = s[i] = 0, ins(i);
    if (h > t) return 0;
    while (1) {
      while (h <= t) {
        i64 u = q[h++];
        if (s[st[u]] != 1) {
          for (i64 v = 1; v <= n; ++v) if (e[u][v].w > 0 && st[u] != st[v]) {
              if ((lab[e[u][v].u] + lab[e[u][v].v] - 2 * e[e[u][v].u][e[u][v].v].w)) upd(u, st[v]); else if (on(e[u][v])) return 1;
            }
        }
      }
      i64 x = inf;
      for (i64 i = n+1; i <= m; ++i) if (st[i] == i && s[i] == 1) x = min(x, lab[i]/2);
      for (i64 i = 1; i <= m; ++i) if (st[i] == i && sl[i] && s[i] != 1) x = min(x, (lab[e[sl[i]][i].u] + lab[e[sl[i]][i].v] - 2 * e[e[sl[i]][i].u][e[sl[i]][i].v].w)>>(s[i]+1));
      for (i64 i = 1; i <= n; ++i) if (~s[st[i]]) if ((lab[i] += (s[st[i]] * 2 - 1) * x) <=0) return 0;
      for (i64 i = n + 1; i <= m; ++i) if (st[i] == i && ~s[st[i]]) lab[i] += (2 - 4 * s[st[i]]) * x;
      h = 1, t = 0;
      for (i64 i = 1; i <= m; ++i) if (st[i] == i && sl[i] && st[sl[i]] != i && !(lab[e[sl[i]][i].u] + lab[e[sl[i]][i].v] - 2 * e[e[sl[i]][i].u][e[sl[i]][i].v].w) && on(e[sl[i]][i])) return 1;
      for (i64 i = n+1; i <= m; ++i) if (st[i] == i && s[i] == 1 && !lab[i]) ex(i);
    }
  }
  pair<i64, vector<array<i64, 2>>> run(i64 N, vector<array<i64, 3>> edges) {
    for (auto &[u, v, w] : edges) ++u, ++v;
    fill(ed, ed+m+1, 0);
    fill(lk, lk+m+1, 0);
    n = m = N;
    id = 0;
    iota(st + 1, st + n + 1, 1);
    i64 wm = 0, weight = 0;
    for (i64 i = 1; i <= n; ++i) for (i64 j = 1; j <= n; ++j) e[i][j] = {i,j,0};
    for (auto [u, v, w] : edges) wm = max(wm, e[v][u].w = e[u][v].w = max(e[u][v].w, w));
    for (i64 i = 1; i <= n; ++i) p[i].clear();
    for (i64 i = 1; i <= n; ++i) for (i64 j = 1; j <= n; ++j) b[i][j] = i==j?i:0;
    fill_n(lab+1, n, wm); while (bfs());
    vector<array<i64, 2>> matching;
    for (i64 i = 1; i <= n; ++i) if (i < lk[i]) weight += e[i][lk[i]].w, matching.push_back({i - 1, lk[i] - 1});
    return {weight, matching};
  }
}
struct union_find {
  vector<i32> A;
  union_find(i32 n = 0) : A(n){
    iota(begin(A), end(A), 0);
  }
  i32 find(i32 a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }
  bool unite(i32 a, i32 b) {
    a = find(a);
    b = find(b);
    if(a == b) {
      return false;
    }
    A[a] = b;
    return true;
  }
};
const f64 TL = 1.9;
timer TIMER;
f32 get_elapsed() {
  return TIMER.elapsed();
}
struct pt {
  pt() { }
  pt(i32 x_, i32 y_) { x = x_; y = y_; }
  i32 x,y;
  __attribute__((always_inline))
  i32 dist2(pt const& o) const {
    return (x-o.x)*(x-o.x)+(y-o.y)*(y-o.y);
  }
  __attribute__((always_inline))
  i32 dist(pt const& o) const {
    return floor(sqrt((x-o.x)*(x-o.x)+(y-o.y)*(y-o.y)));
  }
};
ostream& operator<<(ostream& s, pt const& p) {
  return s << make_tuple(p.x, p.y);
}
struct rect;
struct ptf {
  ptf() { x = 0; y = 0; }
  ptf(f32 x_, f32 y_) { x = x_; y = y_; }
  f32 x,y;
  __attribute__((always_inline))
  f32 dist(ptf const& o) const {
    return sqrt((x-o.x)*(x-o.x)+(y-o.y)*(y-o.y));
  }
  void clamp(rect const& r);
  ptf& operator-=(ptf const& o) {
    x -= o.x; y -= o.y;
    return *this;
  }
  ptf operator-(ptf const& o) const {
    ptf r = *this; r -= o; return r;
  }
  ptf& operator+=(ptf const& o) {
    x += o.x; y += o.y;
    return *this;
  }
  ptf operator+(ptf const& o) const {
    ptf r = *this; r += o; return r;
  }
  ptf& operator/=(f32 z) {
    x /= z; y /= z;
    return *this;
  }
  ptf operator/(f32 z) const {
    ptf r = *this; r /= z; return r;
  }
  ptf& operator*=(f32 z) {
    x *= z; y *= z;
    return *this;
  }
  ptf operator*(f32 z) const {
    ptf r = *this; r *= z; return r;
  }
};
ostream& operator<<(ostream& s, ptf const& p) {
  return s << make_tuple(p.x, p.y);
}
struct rect {
  i32 x1, y1, x2, y2;
  void read() {
    cin>>x1>>x2>>y1>>y2;
  }
  bool intersects(rect const& o) {
    return
      ((x1<=o.x1&&o.x1<=x2) || (o.x1<=x1 && x1 <= o.x2)) &&
      ((y1<=o.y1&&o.y1<=y2) || (o.y1<=y1 && y1 <= o.y2));
  }
  ptf center() const {
    return ptf((x1+x2)/2.0, (y1+y2)/2.0);
  }
  ptf randomf() const {
    return ptf(rng.randomRange32(x1, x2), rng.randomRange32(y1, y2));
  }
  pt random() const {
    return pt(rng.randomRange32(x1, x2), rng.randomRange32(y1, y2));
  }
  template<class F>
  void iter_points(F&& f) const {
    f(x1, y1);
    f(x1, y2);
    f(x2, y1);
    f(x2, y2);
  }
  f64 min_dist(rect const& o) {
    if(intersects(o)) return 0.0;
    f64 r = 1e9;
    iter_points([&](i32 a1, i32 b1) {
      o.iter_points([&](i32 a2, i32 b2) {
        f64 d = sqrt(pow(a1-a2, 2.0) + pow(b1-b2, 2.0));
        r = min(r, d);
      });
    });
    return r;
  }
  f64 max_dist(rect const& o) {
    f64 r = 0.0;
    iter_points([&](i32 a1, i32 b1) {
      o.iter_points([&](i32 a2, i32 b2) {
        f64 d = sqrt(pow(a1-a2, 2.0) + pow(b1-b2, 2.0));
        r = max(r, d);
      });
    });
    return r;
  }
  i32 area() {
    return (y2-y1+1)*(x2-x1+1);
  }
};
void ptf::clamp(rect const& b) {
  x = std::clamp<f32>(x, b.x1, b.x2);
  y = std::clamp<f32>(y, b.y1, b.y2);
}
const i32 N = 800;
const i32 MAXM = 400;
const i32 Q = 400;
i32 m,l,w;
i32 G[MAXM];
rect bounding_rects[N];
pt truth[N];;
void read() {
  i32 n,q;
  cin>>n>>m>>q>>l>>w;
  // runtime_assert(n == N && q == Q);
  for(i32 i = 0; i < (i32)(m); ++i) cin>>G[i];
  cerr << "[DATA] M = " << m << endl;
  cerr << "[DATA] Mr = " << floor(sqrt(m)) << endl;
  cerr << "[DATA] L = " << l << endl;
  cerr << "[DATA] W = " << w << endl;
  cerr << "[DATA] Wr = " << (int)(w / 250) << endl;
  for(i32 i = 0; i < (i32)(N); ++i) {
    bounding_rects[i].read();
  }
}
vector<array<i32, 2>> query(vector<i32> Q) {
  cout << "? " << Q.size();
  for(i32 x : Q) cout << " " << x;
  cout << endl << flush;
  vector<array<i32, 2>> R;
  for(i32 i = 0; i < (i32)(Q.size()-1); ++i) {
    i32 a,b; cin>>a>>b;
    R.push_back({a,b});
  }
  return R;
}
u64 hash_used[N];
u64 hash_remaining[MAXM];
void init_hash() {
  for(i32 i = 0; i < (i32)(N); ++i) hash_used[i] = rng.randomInt64();
  for(i32 i = 0; i < (i32)(MAXM); ++i) hash_remaining[i] = rng.randomInt64();
}
vector<array<int, 2>> possible_edges;
void init_possible_edges() {
  vector<array<int, 3>> Q_min, Q_max;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    int dmin = bounding_rects[i].min_dist(bounding_rects[j]);
    int dmax = bounding_rects[i].max_dist(bounding_rects[j]);
    Q_min.push_back({dmin,i,j});
    Q_max.push_back({dmax,i,j});
  }
  sort(begin(Q_min), end(Q_min));
  sort(begin(Q_max), end(Q_max));
  union_find uf(N);
  int i = 0;
  for(auto [d, a, b] : Q_min) {
    while(i < (int)Q_max.size() && Q_max[i][0] < d) {
      auto [d2, x, y] = Q_max[i];
      uf.unite(x, y);
      i += 1;
    }
    if(uf.find(a) != uf.find(b)) {
      possible_edges.push_back({a,b});
    }
  }
}
void init() {
  init_hash();
  init_possible_edges();
}
i32 beam_dist[N][N];
struct beam_state {
  i32 root;
  bitset<N> used;
  bitset<MAXM> remaining;
  u16 color[N];
  i32 value;
  u64 h;
  void reset() {
    used = 0;
    root = 0;
    remaining = 0;
    for(i32 i = 0; i < (i32)(m); ++i) remaining[i] = 1;
    for(i32 i = 0; i < (i32)(N); ++i) color[i] = N;
    value = 0;
    h = 0;
  }
  template<class A, class F>
  void generate_children
  (vector<vector<i32>> const& T, A&& accept, F&& f)
  {
    static i32 parent[N];
    static i32 tree_size[N];
    static i32 GG[N+1];
    auto dfs = [&](auto dfs, i32 i, i32 p) -> void {
      parent[i] = p;
      tree_size[i] = 1;
      for(i32 j : T[i]) if(!used[j] && j != p) {
          dfs(dfs,j,i);
          tree_size[i] += tree_size[j];
        }
    };
    dfs(dfs, root, -1);
    auto dfs1 = [&](auto dfs1, beam_state& child, i32 c, i32 i, i32 p) -> void {
      child.used[i] = 1;
      child.h ^= hash_used[i];
      child.color[i] = c;
      for(i32 j : T[i]) if(!used[j] && j != p) {
          dfs1(dfs1, child, c, j, i);
        }
    };
    for(i32 x = 0; x < (i32)(N+1); ++x) GG[x] = -1;
    for(i32 i = 0; i < (i32)(m); ++i) if(remaining[i]) GG[G[i]] = i;
    for(i32 x = 0; x < (i32)(N); ++x) if(x != root && !used[x]) {
      i32 y = parent[x];
      i32 szx = tree_size[x];
      i32 szy = tree_size[root] - szx;
      i32 new_value = value + beam_dist[x][y];
      if(GG[szx] != -1 && accept(new_value)) {
        auto child = *this;
        child.root = y;
        child.remaining[GG[szx]] = 0;
        child.h ^= hash_remaining[GG[szx]];
        child.value = new_value;
        dfs1(dfs1, child, GG[szx], x, y);
        f(child);
      }
      if(GG[szy] != -1 && accept(new_value)) {
        auto child = *this;
        child.root = x;
        child.remaining[GG[szy]] = 0;
        child.h ^= hash_remaining[GG[szy]];
        child.value = new_value;
        dfs1(dfs1, child, GG[szy], y, x);
        f(child);
      }
    }
  }
};
vector<vector<array<i32, 2>>>
finish_solution(vector<vector<i32>> const& ans1, int& score) {
  score = 0;
  vector<vector<array<i32, 2>>> ans2(m);
  for(i32 i = 0; i < (i32)(m); ++i) {
    auto I = ans1[i];
    vector<array<i32,3>> E;
    for(i32 k = 0; k < (i32)(I.size()); ++k) for(i32 l = 0; l < (i32)(k); ++l) E.push_back({beam_dist[I[k]][I[l]],I[k],I[l]});
    sort(begin(E), end(E));
    union_find uf(N);
    vector<array<i32,2>> R;
    for(auto [d,a,b] : E) {
      if(uf.unite(a,b)) {
        score += d;
        R.push_back({a,b});
      }
    }
    ans2[i] = R;
  }
  return ans2;
}
vector<vector<int>> postprocess_blossom(vector<vector<int>> ans1) {
  int num1 = 0, num2 = 0;
  for(i32 i = 0; i < (i32)(m); ++i) if(G[i] == 1) num1 += 1;
  for(i32 i = 0; i < (i32)(m); ++i) if(G[i] == 2) num2 += 1;
  if(num1+num2 == 0) return ans1;
  vector<int> remaining;
  for(auto& w : ans1) {
    if(w.size() <= 2) {
      for(int x : w) remaining.push_back(x);
      w.clear();
    }
  }
  vector<array<i64, 3>> e;
  for(i32 i = 0; i < (i32)(num1); ++i) remaining.push_back(-1);
  for(i32 i = 0; i < (i32)(remaining.size()); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    int x = remaining[i], y = remaining[j];
    if(x == -1) {
      if(y != -1) {
        e.push_back({i,j, 10'000'000});
      }
    }else{
      e.push_back({i,j, 10'000'000 - beam_dist[x][y]});
    }
  }
  do { if(!((int)remaining.size() == 2*(num2+num1))) { throw runtime_error("main.cpp" ":" "506" " Assertion failed: " "(int)remaining.size() == 2*(num2+num1)"); } } while(0);
  auto [ans, ma] = blossom::run(remaining.size()+1, e);
  vector<vector<int>> G1;
  vector<vector<int>> G2;
  for(auto [x,y] : ma) {
    do { if(!(remaining[x] != -1)) { throw runtime_error("main.cpp" ":" "512" " Assertion failed: " "remaining[x] != -1"); } } while(0);
    if(remaining[y] == -1) {
      G1.push_back({remaining[x]});
    }else{
      G2.push_back({remaining[x],remaining[y]});
    }
  }
  for(i32 i = 0; i < (i32)(m); ++i) {
    if(G[i] == 1) {
      do { if(!(!G1.empty())) { throw runtime_error("main.cpp" ":" "522" " Assertion failed: " "!G1.empty()"); } } while(0);
      ans1[i] = G1.back(); G1.pop_back();
    }else if(G[i] == 2) {
      do { if(!(!G2.empty())) { throw runtime_error("main.cpp" ":" "525" " Assertion failed: " "!G2.empty()"); } } while(0);
      ans1[i] = G2.back(); G2.pop_back();
    }
  }
  return ans1;
}
void print_solution
(vector<vector<i32>> const& ans1,
 vector<vector<array<i32, 2>>> const& ans2)
{
  cout << "!" << endl;
  for(i32 i = 0; i < (i32)(m); ++i) {
    for(i32 x : ans1[i]) cout << x << " ";
    cout << endl;
    for(auto [a,b] : ans2[i]) {
      cout << a << " " << b << endl;
    }
  }
}
vector<vector<i32>>
finish_greedy(vector<vector<i32>> ans1, bitset<N> used, bitset<MAXM> remaining) {
  vector<i32> I;
  for(i32 i = 0; i < (i32)(m); ++i) if(remaining[i]) I.push_back(i);
  sort(begin(I), end(I), [&](i32 i, i32 j){ return G[i] > G[j]; });
  for(i32 gi : I) {
    i32 i;
    do { i = rng.random32(N); } while(used[i]);
    min_queue<array<i32, 2>> Q;
    vector<i32> vis(N);
    Q.push({0, i});
    i32 count = 0;
    while(!Q.empty()) {
      auto [d,i] = Q.top(); Q.pop();
      if(vis[i]) continue;
      vis[i] = 1;
      count += 1;
      if(count == G[gi]) break;
      for(i32 j = 0; j < (i32)(N); ++j) if(!used[j] && !vis[j]) {
        Q.push({beam_dist[i][j], j});
      }
    }
    for(i32 i = 0; i < (i32)(N); ++i) if(vis[i]) {
      ans1[gi].push_back(i);
      used[i] = 1;
    }
  }
  return ans1;
}
vector<vector<i32>>
solve_beam() {
  vector<array<i32, 3>> Q;
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(i); ++j) Q.push_back({beam_dist[i][j], i, j});
  sort(begin(Q), end(Q));
  union_find uf(N);
  vector<vector<i32>> T(N);
  for(auto [d,a,b] : Q) {
    if(uf.unite(a,b)) {
      T[a].push_back(b);
      T[b].push_back(a);
    }
  }
  hash_set<u64> HS;
  vector<beam_state> BEAM_A;
  BEAM_A.emplace_back(); BEAM_A.back().reset();
  const i32 BW = 80 * MAXM / m;
  for(i32 beam_step = 0; beam_step < (i32)(m-1); ++beam_step) {
    // if(TIMER.elapsed() > TL) break;
    vector<beam_state> BEAM_B;
    min_queue<i32> values;
    i32 na = BEAM_A.size();
    for(i32 ia = 0; ia < (i32)(na); ++ia) {
      BEAM_A[ia].generate_children
        (T,
         [&](i32 value) {
           return (i32)values.size() < BW || value > values.top();
         },
         [&](beam_state const& child) {
           if(HS.insert(child.h).second) {
             if((i32)values.size() == BW) {
               values.pop();
             }
             values.push(child.value);
             BEAM_B.push_back(child);
           }
         });
    }
    rng.shuffle(BEAM_B);
    sort(begin(BEAM_B), end(BEAM_B), [&](auto const& a, auto const& b) {
      return a.value > b.value;
    });
    if(BEAM_B.empty()) break;
    if((i32)BEAM_B.size() > BW) BEAM_B.resize(BW);
    BEAM_A = BEAM_B;
  }
  vector<vector<i32>> ans1(m);
  auto const& sa = BEAM_A[0];
  for(i32 i = 0; i < (i32)(N); ++i) {
    if(sa.color[i] != N) {
      ans1[sa.color[i]].push_back(i);
    }
  }
  auto ret = finish_greedy(ans1, sa.used, sa.remaining);
  if(TIMER.elapsed() < 1.2) ret = postprocess_blossom(ret);
  return ret;
}
set<array<i32, 4>> ineqs_set;
vector<array<int, 2>> ineqs_of[N][N];
vector<array<int, 3>> ineqs_of_point1[N];
vector<array<int, 3>> ineqs_of_point2[N];
vector<array<int, 2>> ineqs_of_point3[N];
void add_ineqs(vector<array<i32, 2>> const& A) {
  vector<vector<i32>> T(N);
  for(auto [a,b] : A) {
    T[a].push_back(b);
    T[b].push_back(a);
  }
  auto dfs = [&](auto dfs, i32 i, i32 p, vector<i32>& C) -> void {
    C.push_back(i);
    for(i32 j : T[i]) if(j != p) {
        dfs(dfs, j,i,C);
      }
  };
  for(auto [a,b] : A) {
    if(a > b) swap(a,b);
    vector<i32> L,R;
    dfs(dfs, a, b, L);
    dfs(dfs, b, a, R);
    for(i32 ix = 0; ix < (i32)(L.size()); ++ix) for(i32 iy = 0; iy < (i32)(R.size()); ++iy) {
      int x = L[ix], y = R[iy];
      if(x == a && y == b) continue;
      if(x > y) swap(x,y);
      if(bounding_rects[a].max_dist(bounding_rects[b]) >
         bounding_rects[x].min_dist(bounding_rects[y])) {
        array<i32,4> P = {a,b,x,y};
        if(ineqs_set.insert(P).second) {
          ineqs_of[a][b].push_back({x,y});
          ineqs_of[x][y].push_back({(int)a,(int)b});
          int same = -1;
          if(x == a) same = x;
          if(x == b) same = x;
          if(y == a) same = y;
          if(y == b) same = y;
          if(same != -1) {
            if(a != same) ineqs_of_point1[a].push_back({b, x,y});
            if(b != same) ineqs_of_point1[b].push_back({a, x,y});
            if(x != same) ineqs_of_point2[x].push_back({y, a,b});
            if(y != same) ineqs_of_point2[y].push_back({x, a,b});
            ineqs_of_point3[same].push_back({ a^b^same, x^y^same });
          }else{
            ineqs_of_point1[a].push_back({b, x,y});
            ineqs_of_point1[b].push_back({a, x,y});
            ineqs_of_point2[x].push_back({y, a,b});
            ineqs_of_point2[y].push_back({x, a,b});
          }
        }
      }
    }
  }
}
void dist_from_points(int NUM_ITERS, int repeats) {
  vector<array<i32, 4>> ineqs(begin(ineqs_set), end(ineqs_set));
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
    beam_dist[i][j] = 0;
  }
  vector<ptf> P(N);
  for(i32 k = 0; k < (i32)(repeats); ++k) {
    if(k == 0) {
      for(i32 i = 0; i < (i32)(N); ++i) P[i] = bounding_rects[i].center();
    }else{
      for(i32 i = 0; i < (i32)(N); ++i) P[i] = bounding_rects[i].randomf();
    }
    if(!ineqs.empty()) {
      f32 temp0 = 200.0;
      f32 temp1 = 1.0;
      f32 temp = temp0;
      for(i32 iter = 0; iter < (i32)(NUM_ITERS); ++iter) {
        if(iter % 512 == 0) {
          f32 done = 1.0 * iter / NUM_ITERS;
          temp = temp0 * pow(temp1/temp0, done);
        }
        auto const& [a,b,x,y] = rng.sample(ineqs);
        auto dab = P[a].dist(P[b]) / temp;
        auto dxy = P[x].dist(P[y]) / temp;
        if(dab > dxy) {
          ptf da = (P[b]-P[a]) / dab;
          ptf dx = (P[x]-P[y]) / dxy;
          P[a] += da;
          P[b] -= da;
          P[x] += dx;
          P[y] -= dx;
          P[a].clamp(bounding_rects[a]);
          P[b].clamp(bounding_rects[b]);
          P[x].clamp(bounding_rects[x]);
          P[y].clamp(bounding_rects[y]);
        }
      }
    }
    vector<pt> Q(N);
    for(i32 i = 0; i < (i32)(N); ++i) Q[i] = pt(P[i].x, P[i].y);
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
      beam_dist[i][j] += 10 * Q[i].dist(Q[j]);
    }
  }
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) beam_dist[i][j] /= repeats;
}
// void post(int niters) {
//   vector<pt> X(N);
//   static f64 total[N][N];
//   static int count[N][N];
//   FOR(i, N) FOR(j, i) {
//     total[i][j] = 0;
//     count[i][j] = 0;
//   }
//   FOR(ik, niters) {
//     if(ik % 100 == 99) debug(ik);
//     FOR(i, N) X[i] = bounding_rects[i].random();
//     static int dist[N][N];
//     static int nbad[N][N];
//     FOR(i, N) FOR(j, i) {
//       dist[i][j] = X[i].dist(X[j]);
//       nbad[i][j] = 0;
//     }
//     for(auto [p, q] : ineqs_set) {
//       auto [v1, u1] = p;
//       auto [v2, u2] = q;
//       if(dist[u1][v1] > dist[u2][v2]) {
//         // TODO: optimization: skip pair when proven impossible
//         nbad[u1][v1] += 1;
//         // nbad[u2][v2] += 1;
//       }
//     }
//     FOR(i, N) FOR(j, i) if(nbad[i][j] == 0) {
//       total[i][j] += dist[i][j];
//       count[i][j] += 1;
//     }
//   }
//   FOR(i, N) FOR(j, i) {
//     f64 d;
//     if(count[i][j] == 0) {
//       d = 1e9;
//     }else{
//       d = total[i][j] / count[i][j];
//     }
//     beam_dist[i][j] = beam_dist[j][i] = (i32)(10 * d);
//   }
// }
struct gibbs_old {
  pt X[N];
  int num_errors;
  f32 dist[N][N];
  f32 est_dist[N][N];
  int est_count[N][N];
  f32 new_dist[N];
  void reset() {
    for(i32 i = 0; i < (i32)(N); ++i) X[i] = bounding_rects[i].random();
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
      est_dist[i][j] = 0;
      est_count[i][j] = 0;
    }
    compute_errors();
  }
  void compute_errors() {
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) dist[i][j] = X[i].dist2(X[j]);
    num_errors = 0;
    for(auto [a,b,x,y] : ineqs_set) {
      if(dist[a][b] > dist[x][y]) num_errors += 1;
    }
  }
  bool transition() {
    int i = rng.random32(N);
    auto new_p = bounding_rects[i].random();
    f32 factor = sqrt(rng.randomFloat());
    pt p = pt(X[i].x + (new_p.x - X[i].x) * factor,
              X[i].y + (new_p.y - X[i].y) * factor);
    for(i32 j = 0; j < (i32)(N); ++j) new_dist[j] = p.dist2(X[j]);
    int delta = 0;
    for(auto [b,x,y] : ineqs_of_point1[i]) {
      if(dist[i][b] > dist[x][y]) delta -= 1;
      if(new_dist[b] > dist[x][y]) delta += 1;
    }
    for(auto [y,a,b] : ineqs_of_point2[i]) {
      if(dist[a][b] > dist[i][y]) delta -= 1;
      if(dist[a][b] > new_dist[y]) delta += 1;
    }
    for(auto [b,y] : ineqs_of_point3[i]) {
      if(dist[i][b] > dist[i][y]) delta -= 1;
      if(new_dist[b] > new_dist[y]) delta += 1;
    }
    if(delta <= 0) {
      num_errors += delta;
      X[i] = p;
      for(i32 j = 0; j < (i32)(N); ++j) dist[i][j] = dist[j][i] = new_dist[j];
    }
    for(i32 j = 0; j < (i32)(N); ++j) {
      est_dist[i][j] += dist[i][j];
      est_count[i][j] += 1;
    }
    return delta <= 0;
  }
};
void solve(){
  vector<ptf> X(N);
  for(i32 i = 0; i < (i32)(N); ++i) X[i] = bounding_rects[i].center();
  vector<i32> I(N); iota(begin(I), end(I), 0);
  sort(begin(I), end(I), [&](i32 i, i32 j){ return bounding_rects[i].area() > bounding_rects[j].area(); });
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j){
    beam_dist[i][j] = bounding_rects[i].center().dist(bounding_rects[j].center());
  }
  vector<int> used_root(N);
  int nused = 0;
  while(nused < Q) {
    vector<int> vis(N);
    for(i32 i : I) {
      if(used_root[i]) continue;
      if(nused == Q) break;
      if(vis[i] > 0) continue;
      used_root[i] = 1;
      vector<i32> J(N); iota(begin(J), end(J), 0);
      sort(begin(J), end(J), [&](i32 a, i32 b) {
        return
          beam_dist[i][a] < beam_dist[i][b];
      });
      vector<int> is_chosen(N);
      vector<int> choice;
      for(i32 i = 0; i < (i32)(l); ++i) {
        is_chosen[J[i]] = 1;
        choice.push_back(i);
      }
      int cur_eval = 0;
      for(i32 ix = 0; ix < (i32)(l); ++ix) for(i32 iy = 0; iy < (i32)(ix); ++iy) {
        int x = J[choice[ix]], y = J[choice[iy]];
        if(x > y) swap(x, y);
        for(auto [a,b] : ineqs_of[x][y]) if(is_chosen[a] && is_chosen[b]) {
            cur_eval += 1;
          }
      }
      do { if(!(cur_eval % 2 == 0)) { throw runtime_error("main.cpp" ":" "914" " Assertion failed: " "cur_eval % 2 == 0"); } } while(0);
      auto eval_swap_chosen = [&](int fr, int to) {
        int delta = 0;
        for(i32 ix = 0; ix < (i32)(l); ++ix) if(choice[ix] != fr) {
          int x = J[choice[ix]], y = J[fr];
          if(x > y) swap(x, y);
          for(auto [a,b] : ineqs_of[x][y]) if(is_chosen[a] && is_chosen[b]) {
              if(a == J[fr] || b == J[fr]) delta -= 1;
              else delta -= 2;
            }
        }
        do { if(!(delta % 2 == 0)) { throw runtime_error("main.cpp" ":" "927" " Assertion failed: " "delta % 2 == 0"); } } while(0);
        is_chosen[J[fr]] = 0;
        is_chosen[J[to]] = 1;
        for(i32 ix = 0; ix < (i32)(l); ++ix) if(choice[ix] != fr) {
          int x = J[choice[ix]], y = J[to];
          if(x > y) swap(x, y);
          for(auto [a,b] : ineqs_of[x][y]) if(is_chosen[a] && is_chosen[b]) {
              if(a == J[to] || b == J[to]) delta += 1;
              else delta += 2;
            }
        }
        do { if(!(delta % 2 == 0)) { throw runtime_error("main.cpp" ":" "940" " Assertion failed: " "delta % 2 == 0"); } } while(0);
        is_chosen[J[to]] = 0;
        is_chosen[J[fr]] = 1;
        return delta;
      };
      if(l != 5) {
        for(i32 i = (l); i <= (i32)(25); ++i) {
          int to = i;
          for(i32 j = (l-1); j >= (i32)(0); --j) {
            int delta = eval_swap_chosen(choice[j], to);
            if(delta < 0) {
              is_chosen[J[choice[j]]] = 0;
              is_chosen[J[to]] = 1;
              swap(choice[j], to);
              cur_eval += delta;
            }
          }
          sort(begin(choice), end(choice));
        }
      }
      vector<int> Q(l);
      for(i32 i = 0; i < (i32)(l); ++i) Q[i] = J[choice[i]];
      for(int j : Q) vis[j] += 1;
      auto ret = query(Q);
      add_ineqs(ret);
      nused += 1;
      if(nused % 12 == 0) {
        dist_from_points(1'000'000, 1);
      }
    }
  }
  bool use_gibbs = false;
  if(l == 3) {
    use_gibbs = true;
  }else if(l == 4) {
    use_gibbs = true;
  }else if(l == 5) {
    use_gibbs = ((int)(w/250) <= 6);
  }else if(l == 6) {
    use_gibbs = ((int)(w/250) <= 4);
  }else if(l <= 8) {
    use_gibbs = ((int)(w/250) <= 3);
  }else if(l <= 12) {
    use_gibbs = ((int)(w/250) <= 2);
  }
  if(use_gibbs) {
    static gibbs_old G2;
    G2.reset();
    do { cerr << "main.cpp" ":" "996" "  {" << "TIMER.elapsed()" << "} = {"; debug_impl_seq(TIMER.elapsed()); cerr << endl << flush; } while(0);
    int iter = 0;
    while(1) {
      iter += 1;
      if(iter % 256 == 0) {
        if(TIMER.elapsed() > 1.3) break;
      }
      G2.transition();
    }
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
      beam_dist[i][j] =
        10 * sqrt((G2.est_dist[i][j] + G2.est_dist[j][i])
                  / (G2.est_count[j][i] + G2.est_count[i][j]));
    }
    do { cerr << "main.cpp" ":" "1012" "  {" << "TIMER.elapsed()" << "} = {"; debug_impl_seq(TIMER.elapsed()); cerr << endl << flush; } while(0);
  }else{
    dist_from_points(2'000'000, 10);
  }
  { i32 iter = 0;
    while(1) {
      iter++;
      bool imp = 0;
      for(auto const& [a,b,x,y] : ineqs_set) {
        if(beam_dist[a][b] > beam_dist[x][y]) {
          beam_dist[x][y] = beam_dist[y][x] = beam_dist[a][b] + 1;
          imp = 1;
        }
      }
      if(!imp) break;
    }
    do { cerr << "main.cpp" ":" "1028" "  {" << "iter" << "} = {"; debug_impl_seq(iter); cerr << endl << flush; } while(0);
  }
  auto ans1 = solve_beam();
  int score;
  auto ans2 = finish_solution(ans1, score);
  print_solution(ans1, ans2);
}
int main() {
  TIMER.reset();
  rng.reset(time(0));
  read();
  init();
  solve();
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
