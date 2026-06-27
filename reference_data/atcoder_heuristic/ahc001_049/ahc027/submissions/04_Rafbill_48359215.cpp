#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
// #ifndef CP_GEN
// #define BACKWARD_HAS_BFD 1
// #include "backward.hpp"
// backward::SignalHandling sh;
// #endif
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
// Letrec
template<class Fun>
class letrec_result {
  Fun fun_;
  public:
    template<class T>
    explicit letrec_result(T &&fun): fun_(forward<T>(fun)) {}
    template<class ...Args>
    decltype(auto) operator()(Args &&...args) {
      return fun_(ref(*this), forward<Args>(args)...);
    }
};
template<class Fun>
decltype(auto) letrec(Fun &&fun) {
  return letrec_result<decay_t<Fun>>(forward<Fun>(fun));
}
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

const i64 INF = 1e9;
__attribute__((always_inline)) inline i64 f2(i64 x) {
  return x*(x-1)/2;
}
timer TM;
const i32 N = 40;
const i32 NN = N*N;
i32 n,nn;
bitset<N> gridh[N];
bitset<N> gridv[N];
i32 gridd[NN];
i32 parity[NN];
i32 G[NN][4];
i32 G_edge[NN][4];
i32 nedges;
i32 edge_from[2*NN];
i32 edge_to[2*NN];
i32 edge_xor[2*NN];
i32 get_edge(i32 u, i32 v) {
  for(i32 d = 0; d < (i32)(4); ++d) if(G[u][d] == v) return G_edge[u][d];
  assert(false);
}
void read(){
  cin>>n;
  nn=n*n;
  for(i32 i = 0; i < (i32)(n-1); ++i) for(i32 j = 0; j < (i32)(n); ++j) { char c; cin>>c; gridh[i][j] = (c=='1'); }
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n-1); ++j) { char c; cin>>c; gridv[i][j] = (c=='1'); }
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) cin>>gridd[i*n+j];
  cerr << "[DATA] N = " << n << endl;
}
const char* DIRS = "RDLU";
i32 dist[NN][NN];
i32 dist_from[NN][NN];
void calc_dist(i32 u) {
  static i32 Q[NN];
  for(i32 v = 0; v < (i32)(nn); ++v) dist[u][v] = INF;
  i32 nq = 0;
  Q[nq++] = u;
  dist[u][u] = 0;
  dist_from[u][u] = -1;
  for(i32 iq = 0; iq < (i32)(nq); ++iq) {
    i32 v = Q[iq];
    for(i32 d = 0; d < (i32)(4); ++d) {
      i32 w = G[v][d];
      if(w == -1) continue;
      if(dist[u][v]+1 < dist[u][w]){
        Q[nq++] = w;
        dist[u][w] = dist[u][v]+1;
        dist_from[u][w] = v;
      }
    }
  }
}
void init(){
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) parity[i*n+j] = (i+j)%2;
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) for(i32 d = 0; d < (i32)(4); ++d) G[i*n+j][d] = -1;
  nedges = 0;
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) {
    if(i+1 < n && !gridh[i][j]) {
      G[i*n+j][1] = (i+1)*n+j;
      G_edge[i*n+j][1] = nedges;
      G[(i+1)*n+j][3] = i*n+j;
      G_edge[(i+1)*n+j][3] = nedges;
      edge_from[nedges] = i*n+j;
      edge_to[nedges] = (i+1)*n+j;
      nedges++;
    }
    if(j+1 < n && !gridv[i][j]) {
      G[i*n+j][0] = i*n+(j+1);
      G_edge[i*n+j][0] = nedges;
      G[i*n+(j+1)][2] = i*n+j;
      G_edge[i*n+(j+1)][2] = nedges;
      edge_from[nedges] = i*n+j;
      edge_to[nedges] = i*n+(j+1);
      nedges++;
    }
  }
  for(i32 i = 0; i < (i32)(nedges); ++i) edge_xor[i] = edge_from[i]^edge_to[i];
  for(i32 ij = 0; ij < (i32)(nn); ++ij) {
    calc_dist(ij);
  }
}
double minimize_sum2(double a, double b){
  double lo = 0.5, hi = 0.5;
  auto f = [&](double x) {
    return a/x + b/(1-x);
  };
  while(f(lo/2) < f(lo)) lo /= 2;
  lo /= 2;
  while(f(1-(1-hi)/2) < f(hi)) hi = 1-(1-hi)/2;
  hi = 1-(1-hi)/2;
  while(hi-lo > 1e-10) {
    double mi1 = (2*lo+hi)/3;
    double mi2 = (lo+2*hi)/3;
    if(f(mi1) < f(mi2)) {
      hi = mi2;
    }else{
      lo = mi1;
    }
  }
  return lo;
}
struct group {
  i32 A[NN+1];
  i32 na = 0;
  i32 length;
  i32 lengths[NN+1];
  void reverse() {
    std::reverse(A,A+na+1);
  }
  void init(i32 root){
    A[0] = A[1] = root;
    na = 1;
    length = 0;
    lengths[0] = lengths[1] = 0;
  }
  void update_lengths() {
    length = 0;
    for(i32 i = 0; i < (i32)(na); ++i) {
      length += dist[A[i]][A[i+1]];
      lengths[i+1] = length;
    }
  }
  void add(i32 x) {
    i32 besti = 0;
    i32 best = INF;
    for(i32 i = 0; i < (i32)(na); ++i) {
      i32 cur = dist[A[i]][x] + dist[A[i+1]][x] - dist[A[i]][A[i+1]];
      if(cur < best) {
        best = cur;
        besti = i;
      }
    }
    for(i32 j = (na); j >= (i32)(besti+1); --j) A[j+1] = A[j];
    A[besti+1] = x;
    na++;
    update_lengths();
  }
  __attribute__((always_inline)) inline void optimize_step(){
    i32 i,j;
    if(rng.next()&1) {
      do {
        i = rng.random32(na);
        j = rng.random32(na);
      }while(i == j);
      if(i > j) swap(i,j);
    }else{
      do {
        i = rng.random32(na);
        j = i+1+rng.random32(20);
      }while(j >= na);
    }
    i64 delta = 0;
    delta -= dist[A[i]][A[i+1]];
    delta += dist[A[i]][A[j]];
    delta -= dist[A[j+1]][A[j]];
    delta += dist[A[j+1]][A[i+1]];
    if(delta <= 0) {
      length += delta;
      std::reverse(A+i+1, A+j+1);
    }
  }
  void optimize(i32 nsteps) {
    for(i32 niter = 0; niter < (i32)(nsteps); ++niter) optimize_step();
    update_lengths();
  }
  void optimize_until(double tl) {
    timer t;
    i64 niter = 0;
    while(1){
      optimize_step();
      niter++;
      if(niter%1024==0 && t.elapsed() > tl) {
        break;
      }
    }
    update_lengths();
  }
};
struct state {
  vector<group> groups;
  i64 score;
  vector<i32> get_cycle() const {
    i32 sz = groups.size();
    vector<i32> cycle;
    for(i32 i = 0; i < (i32)(sz); ++i) {
      cycle.insert(end(cycle), groups[i].A, groups[i].A+groups[i].na);
    }
    vector<i32> ecycle;
    for(i32 i = 0; i < (i32)(cycle.size()); ++i) {
      i32 x = cycle[i];
      i32 y = cycle[(i+1)%cycle.size()];
      while(x != y) {
        ecycle.emplace_back(x);
        x = dist_from[y][x];
      }
    }
    return ecycle;
  }
  i64 total_length() const {
    i64 l = 0;
    for(auto const& g : groups) l += g.length;
    return l;
  }
  void calc_score() {
    vector<i64> last(nn);
    i64 di = 0;
    for(auto const& g : groups) {
      for(i32 i = 0; i < (i32)(g.na); ++i) {
        di += dist[g.A[i]][g.A[i+1]];
        last[g.A[i+1]] = di;
      }
    }
    score = 0;
    for(auto const& g : groups) {
      for(i32 i = 0; i < (i32)(g.na); ++i) {
        di += dist[g.A[i]][g.A[i+1]];
        score += gridd[g.A[i+1]] * f2(di - last[g.A[i+1]]);
        last[g.A[i+1]] = di;
      }
    }
  }
  vector<i32> get_dist_left(i32 base){
    i32 sz = groups.size();
    vector<i32> dist_left(nn, INF);
    i32 di = 0;
    for(i32 i = 0; i < (i32)(sz-1); ++i) {
      auto& g = groups[(base-1-i+sz)%sz];
      for(i32 k = (g.na-1); k >= (i32)(0); --k) {
        di += dist[g.A[k]][g.A[k+1]];
        dist_left[g.A[k]] = min(dist_left[g.A[k]], di);
      }
    }
    return dist_left;
  }
  vector<i32> get_dist_right(i32 base){
    i32 sz = groups.size();
    vector<i32> dist_right(nn, INF);
    i32 di = 0;
    for(i32 i = 0; i < (i32)(sz-1); ++i) {
      auto& g = groups[(base+1+i)%sz];
      for(i32 k = 0; k < (i32)(g.na); ++k) {
        di += dist[g.A[k]][g.A[k+1]];
        dist_right[g.A[k+1]] = min(dist_right[g.A[k+1]], di);
      }
    }
    return dist_right;
  }
  vector<i32> get_dist_inner(i32 base){
    auto& g = groups[base];
    vector<i32> dist_inner(nn, INF);
    i32 di = 0;
    for(i32 i = 0; i < (i32)(g.na); ++i) {
      di += dist[g.A[i]][g.A[i+1]];
      dist_inner[g.A[i+1]] = min(dist_inner[g.A[i+1]], di);
    }
    return dist_inner;
  }
  void try_insert(i32 base, i32 root, i32 x) {
    // calc_score();
    auto& g = groups[base];
    if(find(g.A,g.A+g.na,x) != g.A+g.na) return;
    auto dist_left = get_dist_left(base);
    auto dist_right = get_dist_right(base);
    auto dist_inner = get_dist_inner(base);
    i32 length_left = 0;
    i64 length = total_length();
    i32 besti = -1;
    i64 best_value = score / length;
    i64 best_score = score;
    i64 c2 = 0, c1 = 0;
    c2 += gridd[root];
    c1 += gridd[root] * (-1 + 2 * g.length);
    for(i32 y = 0; y < (i32)(nn); ++y) if(y != root && y != x) {
      if(dist_inner[y] == INF) {
        c2 += gridd[y];
        c1 += gridd[y] * (-1 + 2 * (dist_left[y] + dist_right[y] + g.length));
      }else{
        if(dist_left[y] != INF) {
          c2 += gridd[y];
          c1 += gridd[y] * (-1 + 2 * (dist_left[y] + dist_inner[y]));
        }else{
          c2 += gridd[y];
          c1 += gridd[y] * (-1 + 2 * length);
        }
      }
    }
    for(i32 i = 0; i < (i32)(g.na); ++i) {
      // g.A[i] --- x --- g.A[i+1]
      i32 length_right = g.length - length_left - dist[g.A[i]][g.A[i+1]];
      i32 delta_length = - dist[g.A[i]][g.A[i+1]] + dist[x][g.A[i]] + dist[x][g.A[i+1]];
      i64 delta_score = c2 * delta_length * delta_length + c1 * delta_length;
      delta_score += gridd[x] * f2(dist_left[x] + length_left + dist[x][g.A[i]]);
      delta_score += gridd[x] * f2(dist_right[x] + length_right + dist[x][g.A[i+1]]);
      delta_score -= gridd[x] * f2(dist_left[x] + dist_right[x] + g.length);
      i64 new_score = (score + delta_score) / (length + delta_length);
      if(new_score < best_value) {
        besti = i;
        best_value = new_score;
        best_score = score + delta_score;
      }
      length_left += dist[g.A[i]][g.A[i+1]];
      { i32 y = g.A[i+1];
        if(dist_left[y] != INF) {
          c1 -= gridd[y] * (-1 + 2 * (dist_left[y] + dist_inner[y]));
          c1 += gridd[y] * (-1 + 2 * (dist_right[y] + (g.length-dist_inner[y])));
        }
      }
    }
    if(besti != -1) {
      for(i32 j = (g.na); j >= (i32)(besti+1); --j) {
        g.A[j+1] = g.A[j];
      }
      g.A[besti+1] = x;
      g.na++;
      g.length = 0;
      for(i32 i = 0; i < (i32)(g.na); ++i) g.length += dist[g.A[i]][g.A[i+1]];
      score = best_score;
      // calc_score();
      // runtime_assert(score == best_score);
    }
  }
};
i64 cycle_full_score(vector<i32> const& A) {
  i32 na = A.size();
  vector<i32> last(nn,-1);
  for(i32 i = 0; i < (i32)(na); ++i) last[A[i]] = i;
  i64 score = 0;
  for(i32 i = 0; i < (i32)(na); ++i) {
    score += gridd[A[i]] * f2(i <= last[A[i]] ? i-last[A[i]]+na : i-last[A[i]]);
    last[A[i]] = i;
  }
  return score;
}
i64 cycle_score(vector<i32> const& A) {
  i32 na = A.size();
  vector<i32> last(nn,-1);
  for(i32 i = 0; i < (i32)(na); ++i) last[A[i]] = i;
  i64 score = 0;
  for(i32 i = 0; i < (i32)(na); ++i) {
    score += gridd[A[i]] * f2(i <= last[A[i]] ? i-last[A[i]]+na : i-last[A[i]]);
    last[A[i]] = i;
  }
  return round(score / na);
}
template<class T, size_t CAP>
struct static_deque {
  T data[CAP];
  T buf[CAP];
  size_t from, sz;
  static_deque() {
    from = CAP/2;
    sz = 0;
  }
  void clear() {
    from = CAP/2;
    sz = 0;
  }
  void recenter() {
    size_t new_from = CAP/2 - sz/2;
    for(i32 i = 0; i < (i32)(sz); ++i) buf[i] = data[from+i];
    for(i32 i = 0; i < (i32)(sz); ++i) data[new_from+i] = buf[i];
    from = new_from;
  }
  void push_back(T x) {
    if(__builtin_expect(from+sz == CAP, false)) {
      recenter();
    }
    data[from+sz] = x;
    sz += 1;
  }
  void push_front(T x) {
    if(__builtin_expect(from == 0, false)) {
      recenter();
    }
    from -= 1;
    data[from] = x;
    sz += 1;
  }
  void pop_front() {
    from += 1;
    sz -= 1;
  }
  void pop_back() {
    sz -= 1;
  }
  T const& back() const {
    return data[from+sz-1];
  }
  T const& front() const {
    return data[from];
  }
  T& back() {
    return data[from+sz-1];
  }
  T& front() {
    return data[from];
  }
  T& operator[](size_t ix) {
    return data[from+ix];
  }
  T const& operator[](size_t ix) const {
    return data[from+ix];
  }
  size_t size() const {
    return sz;
  }
};
struct state2 {
  static_deque<array<i32,3>, 65536> path;
  i32 dist_left[NN];
  i32 dist_right[NN];
  i32 offset_left, offset_right;
  i64 score0;
  i64 score1;
  i64 score2;
  void reset(){
    path.clear();
    for(i32 i = 0; i < (i32)(nn); ++i) dist_left[i] = dist_right[i] = INF;
    offset_left = offset_right = 0;
    score0 = score1 = score2 = 0;
    score0 = (i64)nn*INF;
  }
  void push_right(i32 x) {
    offset_right += 1;
    path.push_back({x,INF,INF});
    score0 += score1;
    score1 += score2;
    if(dist_right[x] == INF) {
      dist_left[x] = -offset_left + (path.size() - 1);
      dist_right[x] = -offset_right;
      score2 += gridd[x];
      score1 += gridd[x] * path.size();
      score0 += gridd[x] * f2(path.size());
      score0 -= INF;
    }else{
      i32 di_r = dist_right[x] + offset_right;
      i32 i = (path.size() - 1) - di_r;
      path.back()[1] = di_r;
      path[i][2] = di_r;
      dist_right[x] = -offset_right;
      i32 di_l = dist_left[x] + offset_left;
      score1 -= gridd[x] * di_r;
      score0 -= gridd[x] * f2(di_l + di_r + 1);
      score0 += gridd[x] * f2(di_l + 1);
      score0 += gridd[x] * f2(di_r);
    }
  }
  void pop_right() {
    i32 x = path.back()[0];
    if(path.back()[1] == INF) {
      score0 += INF;
      score0 -= gridd[x] * f2(path.size());
      score1 -= gridd[x] * path.size();
      score2 -= gridd[x];
      dist_left[x] = INF;
      dist_right[x] = INF;
    }else {
      i32 di_r = path.back()[1];
      i32 i = (path.size() - 1) - di_r;
      path.back()[1] = INF;
      path[i][2] = INF;
      dist_right[x] = -offset_right + di_r;
      i32 di_l = dist_left[x] + offset_left;
      score1 += gridd[x] * di_r;
      score0 += gridd[x] * f2(di_l + di_r + 1);
      score0 -= gridd[x] * f2(di_l + 1);
      score0 -= gridd[x] * f2(di_r);
    }
    score1 -= score2;
    score0 -= score1;
    path.pop_back();
    offset_right -= 1;
  }
  void push_left(i32 x) {
    offset_left += 1;
    path.push_front({x,INF,INF});
    score0 += score1;
    score1 += score2;
    if(dist_left[x] == INF) {
      dist_left[x] = -offset_left;
      dist_right[x] = -offset_right + (path.size() - 1);
      score2 += gridd[x];
      score1 += gridd[x] * path.size();
      score0 += gridd[x] * f2(path.size());
      score0 -= INF;
    }else{
      i32 di_l = dist_left[x] + offset_left;
      i32 i = di_l;
      path.front()[2] = di_l;
      path[i][1] = di_l;
      dist_left[x] = -offset_left;
      i32 di_r = dist_right[x] + offset_right;
      score1 -= gridd[x] * di_l;
      score0 -= gridd[x] * f2(di_l + di_r + 1);
      score0 += gridd[x] * f2(di_l);
      score0 += gridd[x] * f2(di_r + 1);
    }
  }
  void pop_left() {
    i32 x = path.front()[0];
    if(path.front()[2] == INF) {
      score0 += INF;
      score0 -= gridd[x] * f2(path.size());
      score1 -= gridd[x] * path.size();
      score2 -= gridd[x];
      dist_left[x] = INF;
      dist_right[x] = INF;
    }else {
      i32 di_l = path.front()[2];
      i32 i = di_l;
      path.back()[2] = INF;
      path[i][1] = INF;
      dist_left[x] = -offset_left + di_l;
      i32 di_r = dist_right[x] + offset_right;
      score1 += gridd[x] * di_l;
      score0 += gridd[x] * f2(di_l + di_r + 1);
      score0 -= gridd[x] * f2(di_l);
      score0 -= gridd[x] * f2(di_r + 1);
    }
    score1 -= score2;
    score0 -= score1;
    path.pop_front();
    offset_left -= 1;
  }
};
i32 B[1<<17];
bool optimize_cycle0(vector<i32>& A, double tl) {
  static state2 S; S.reset();
  for(auto x : A) S.push_right(x);
  i32 na = A.size();
  bool anyimp = 0;
  for(i32 ia = 0; ia < (i32)(na); ++ia) {
    if(TM.elapsed() > tl) break;
    { i32 x = S.path[0][0];
      S.pop_left();
      S.push_right(x);
    }
    if(S.path.back()[1] != INF) {
      double best_value = (double)S.score0 / S.path.size();
      i32 x = S.path.back()[0];
      S.pop_right();
      i32 nb = 0;
      while(S.path.back()[0] != x) {
        B[nb++] = S.path.back()[0];
        S.pop_right();
      }
      for(i32 i = 0; i < (i32)(nb); ++i) S.push_right(B[i]);
      S.push_right(x);
      double value = (double)S.score0 / S.path.size();
      if((i64)value <= (i64)best_value) {
        if((i64)value < (i64)best_value) anyimp = true;
        // debug((i64)value);
      }else{
        for(i32 i = 0; i < (i32)(nb+1); ++i) S.pop_right();
        for(i32 i = (nb-1); i >= (i32)(0); --i) S.push_right(B[i]);
        S.push_right(x);
      }
    }
  }
  A.clear();
  for(i32 i = 0; i < (i32)(S.path.size()); ++i) A.emplace_back(S.path[i][0]);
  return anyimp;
}
bool optimize_cycle1(vector<i32>& A, double tl) {
  static state2 S; S.reset();
  for(auto x : A) S.push_right(x);
  i32 na = A.size();
  bool anyimp = 0;
  for(i32 ia = 0; ia < (i32)(na); ++ia) {
    if(TM.elapsed() > tl) break;
    { i32 x = S.path[0][0];
      S.pop_left();
      S.push_right(x);
    }
    if(S.path.back()[1] != INF) {
      double best_value = (double)S.score0 / S.path.size();
      i32 x = S.path.back()[0];
      S.pop_right();
      i32 nb = 0;
      while(S.path.back()[0] != x) {
        B[nb++] = S.path.back()[0];
        S.pop_right();
      }
      double value = (double)S.score0 / S.path.size();
      if((i64)value <= (i64)best_value) {
        if((i64)value < (i64)best_value) anyimp = true;
        // debug((i64)value);
      }else{
        for(i32 i = (nb-1); i >= (i32)(0); --i) S.push_right(B[i]);
        S.push_right(x);
      }
    }
  }
  A.clear();
  for(i32 i = 0; i < (i32)(S.path.size()); ++i) A.emplace_back(S.path[i][0]);
  return anyimp;
}
bool optimize_cycle(vector<i32>& A, double tl, bool simple = false, bool loop = false) {
  static state2 S; S.reset();
  for(auto x : A) S.push_right(x);
  bool anyimp = 0;
  static i32 best_seq[32];
  i32 upto = simple ? 2 : n <= 23 ? 7 : 6;
  while(TM.elapsed() < tl) {
    bool imp0 = 0;
    for(i32 goal_size = 1; goal_size <= (simple ? 2 : upto); goal_size += 1) {
      for(i32 base_size = 0; base_size <= goal_size + 4; base_size += 1) {
        if(goal_size == 0 && base_size == 0) continue;
        if((goal_size&1) != (base_size&1)) continue;
        i32 nrot = S.path.size();
        for(i32 irot = 0; irot < (i32)(nrot); ++irot) {
          if(TM.elapsed() > tl) break;
          i32 nbest_seq = base_size;
          double best_value = (double)S.score0 / S.path.size();
          bool imp = 0;
          for(i32 i = 0; i < (i32)(base_size); ++i) {
            best_seq[base_size - 1 - i] = S.path.back()[0];
            S.pop_right();
          }
          i32 x0 = S.path.front()[0];
          auto bt = letrec([&](auto bt, i32 i) -> void {
            static i32 seq[32];
            if(i == goal_size) {
              double value = (double)S.score0 / S.path.size();
              if(value <= best_value) {
                imp = 1;
                if((i64)value < (i64)best_value) {
                  anyimp = 1;
                  imp0 = 1;
                  nrot += max(goal_size, base_size);
                }
                best_value = value;
                for(i32 i = 0; i < (i32)(goal_size); ++i) best_seq[i] = seq[i];
                nbest_seq = goal_size;
              }
              return;
            }
            i32 x = S.path.back()[0];
            for(i32 d = 0; d < (i32)(4); ++d) {
              i32 y = G[x][d];
              if(y == -1 || dist[x0][y] > goal_size-i) continue;
              seq[i] = y;
              S.push_right(y);
              bt(i+1);
              S.pop_right();
            }
          });
          bt(0);
          for(i32 i = 0; i < (i32)(nbest_seq); ++i) S.push_right(best_seq[i]);
          // if(imp) {
          //   // i64 value = S.score0 / S.path.size();
          //   // runtime_assert(best_value == value);
          // }
          { i32 x = S.path[0][0];
            S.pop_left();
            S.push_right(x);
          }
        }
      }
    }
    do { cerr << "main.cpp" ":" "781" "  {" << "simple, imp0, S.score0/S.path.size()" << "} = {"; debug_impl_seq(simple, imp0, S.score0/S.path.size()); cerr << endl << flush; } while(0);
    if(!loop || !imp0) break;
  }
  A.clear();
  for(i32 i = 0; i < (i32)(S.path.size()); ++i) A.emplace_back(S.path[i][0]);
  return anyimp;
}
vector<i32> best_solution;
i64 best_score = 1ll<<50;
void solve1(double tl1, double tl2, double tl3){
  vector<long double> freq(nn, 1.0);
  {
    for(i32 i = 0; i < (i32)(nn); ++i) freq[i] = 0.5 + 0.5 * rng.randomDouble();
    double sumfreq = 0;
    for(i32 i = 0; i < (i32)(nn); ++i) sumfreq += freq[i];
    for(i32 i = 0; i < (i32)(nn); ++i) freq[i] *= nn / sumfreq;
  }
  { long double last = 0;
    while(1) {
      vector<long double> grad(nn, 0.0);
      long double value = 0;
      for(i32 i = 0; i < (i32)(nn); ++i) {
        value += gridd[i] / (freq[i] / nn) / 2;
        grad[i] = -gridd[i] / freq[i];
      }
      for(i32 i = 0; i < (i32)(nn); ++i) freq[i] -= 1e-4 * grad[i];
      long double sumfreq = 0;
      for(i32 i = 0; i < (i32)(nn); ++i) sumfreq += freq[i];
      for(i32 i = 0; i < (i32)(nn); ++i) freq[i] *= nn / sumfreq;
      if(abs(last-value) < 1e-3) break;
      last = value;
    }
    cerr << "lower bound = " << last << endl;
  }
  i32 D = (double)nn * (2.35 * (40-n) + 1.8 * (n-20)) / (40-20);
  vector<i32> count(nn);
  { i32 s = 0;
    double alpha = 2.0;
    do {
      s = 0;
      for(i32 i = 0; i < (i32)(nn); ++i) s += (count[i] = max<i32>(1,round(alpha * D*freq[i]/nn)));
      alpha *= 0.999;
    }while(s > D);
  }
  for(i32 i = 0; i < (i32)(nn); ++i) while(__builtin_popcount(count[i]) != 1) count[i] -= 1;
  for(i32 i = 0; i < (i32)(nn); ++i) count[i] *= 2;
  for(i32 i = 0; i < (i32)(nn); ++i) count[i] = min(16,count[i]);
  while(1) {
    bool imp = 0;
    i32 totcnt = 0;
    for(i32 i = 0; i < (i32)(nn); ++i) totcnt += count[i];
    for(i32 i = 0; i < (i32)(nn); ++i) if(count[i] < 16) {
      long double value0 = 0, value1 = 0;
      i32 cnti = 2*count[i];
      for(i32 j = 0; j < (i32)(nn); ++j) {
        value0 += (double) gridd[j] / ((double) count[j] / totcnt) / 2;
        value1 += (double) gridd[j] / ((double) (i==j?cnti:count[j]) / (totcnt + count[i])) / 2;
      }
      if(value1 < value0 - 1) {
        //debug(value1);
        imp = 1;
        totcnt += count[i];
        count[i] *= 2;
      }
    }
    for(i32 i = 0; i < (i32)(nn); ++i) if(count[i] > 1) {
      long double value0 = 0, value1 = 0;
      i32 cnti = count[i] / 2;
      for(i32 j = 0; j < (i32)(nn); ++j) {
        value0 += (double) gridd[j] / ((double) count[j] / totcnt) / 2;
        value1 += (double) gridd[j] / ((double) (i==j?cnti:count[j]) / (totcnt - cnti)) / 2;
      }
      if(value1 < value0 - 1) {
        //debug(value1);
        imp = 1;
        totcnt -= cnti;
        count[i] /= 2;
      }
    }
    if(!imp) break;
  }
  i32 root = distance(begin(count), max_element(begin(count), end(count)));
  i32 sz = count[root];
  // map<i32,group> GI;
  // FOR(i,nn) GI[count[i]].init(root);
  // FOR(i,nn) if(i != root) GI[count[i]].add(i);
  // for(auto& p : GI) {
  //   rng.shuffle(p.second.A+1,p.second.A+p.second.na);
  //   if(p.second.na > 3) p.second.optimize(1'000'000);
  // }
  group GI;
  GI.init(root);
  for(i32 i = 0; i < (i32)(nn); ++i) if(i != root) GI.add(i);
  rng.shuffle(GI.A+1,GI.A+GI.na);
  GI.optimize(1'000'000);
  vector<i32> I;
  // for(auto const& p : GI) {
  //   FOR(i,p.second.na) if(i>0) I.eb(p.second.A[i]);
  // }
  for(i32 i = 0; i < (i32)(nn); ++i) I.emplace_back(GI.A[i]);
  // rng.shuffle(I);
  // stable_sort(all(I), [&](i32 i, i32 j){ return dist[root][i] < dist[root][j]; });
  stable_sort(begin(I), end(I), [&](i32 i, i32 j){ return round(40*freq[i]) > round(40*freq[j]); });
  do { cerr << "main.cpp" ":" "895" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);
  state S;
  S.groups.resize(sz);
  for(i32 i = 0; i < (i32)(sz); ++i) S.groups[i].init(root);
  for(i32 x : I) {
    if(x == root) continue;
    for(i32 i = 0; i < (i32)(sz); ++i) if(S.groups[i].na > 5) S.groups[i].optimize(500);
    vector<group> ngroups(sz);
    for(i32 i = 0; i < (i32)(sz); ++i) {
      ngroups[i] = S.groups[i];
      ngroups[i].add(x);
      ngroups[i].optimize(1500);
    }
    i32 step = sz / count[x];
    i32 beststart = -1;
    double best = 1e300;
    for(i32 start = 0; start < (i32)(step); ++start) {
      double cur = 0;
      for(i32 i = 0; i < (i32)(sz); ++i) {
        cur += (i % step == start) ?
          pow(1.02, (double)ngroups[i].length) :
          pow(1.02, (double)S.groups[i].length);
          // pow((double)ngroups[i].length, 4.0):
          // pow((double)S.groups[i].length, 4.0);
      }
      if(cur < best) {
        best = cur;
        beststart = start;
      }
    }
    do { if(!(beststart != -1)) { throw runtime_error("main.cpp" ":" "929" " Assertion failed: " "beststart != -1"); } } while(0);
    for(i32 i = 0; i < (i32)(sz); ++i) {
      if(i%step == beststart) {
        S.groups[i] = ngroups[i];
      }
    }
  }
  // FOR(i,sz) {
  //   vector<i32> x(nn);
  //   FOR(j,S.groups[i].na) x[S.groups[i].A[j]] = 1;
  //   FOR(i,n) {
  //     FOR(j,n) {
  //       cerr << x[i*n+j];
  //     }
  //     cerr << endl;
  //   }
  //   cerr << endl;
  // }
  // FOR(i,sz) debug(S.groups[i].length, S.groups[i].na);
  do { cerr << "main.cpp" ":" "950" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);
  for(i32 i = 0; i < (i32)(sz); ++i) S.groups[i].optimize(100'000);
  do { cerr << "main.cpp" ":" "954" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);
  { i64 score = cycle_score(S.get_cycle());
    while(1) {
      bool imp = 0;
      for(i32 i = 0; i < (i32)(sz); ++i) {
        S.groups[i].reverse();
        auto A = S.get_cycle();
        // optimize_cycle(A, true);
        i64 cur = cycle_score(A);
        if(cur <= score) {
          if(cur < score) imp = 1;
          score = cur;
          do { cerr << "main.cpp" ":" "967" "  {" << "score" << "} = {"; debug_impl_seq(score); cerr << endl << flush; } while(0);
        }else{
          S.groups[i].reverse();
        }
      }
      if(!imp) break;
    }
  }
  S.calc_score();
  do { cerr << "main.cpp" ":" "978" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);
  // for(i32 x : I) {
  //   if(x == root) continue;
  //   if(TM.elapsed() > TL1) break;
  //   FOR(i,sz) {
  //     if(TM.elapsed() > TL1) break;
  //     S.try_insert(i,root,x);
  //   }
  // }
  do { cerr << "main.cpp" ":" "989" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);
  { auto A = S.get_cycle();
    optimize_cycle(A, tl2, true);
    while(TM.elapsed() < tl2) {
      bool imp = false;
      imp |= optimize_cycle0(A, tl2);
      imp |= optimize_cycle1(A, tl2);
      imp |= optimize_cycle(A, tl2);
      if(!imp) break;
    }
    for(i32 k = 0; k < (i32)(2); ++k) {
      i32 na = A.size();
      for(i32 i = 0; i < (i32)(na); ++i) A.emplace_back(A[i]);
      optimize_cycle(A, tl3, true, true);
    }
    i64 score = cycle_score(A);
    if(score < best_score) {
      best_solution = A;
      best_score = score;
    }
  }
  for(i32 x : I) {
    if(x == root) continue;
    if(TM.elapsed() > tl1) break;
    for(i32 i = 0; i < (i32)(sz); ++i) {
      if(TM.elapsed() > tl1) break;
      S.try_insert(i,root,x);
    }
  }
  { auto A = S.get_cycle();
    optimize_cycle(A, tl2, true);
    while(TM.elapsed() < tl2) {
      bool imp = false;
      imp |= optimize_cycle0(A, tl2);
      imp |= optimize_cycle1(A, tl2);
      imp |= optimize_cycle(A, tl2);
      if(!imp) break;
    }
    for(i32 k = 0; k < (i32)(2); ++k) {
      i32 na = A.size();
      for(i32 i = 0; i < (i32)(na); ++i) A.emplace_back(A[i]);
      optimize_cycle(A, tl3, true, true);
    }
    i64 score = cycle_score(A);
    if(score < best_score) {
      best_solution = A;
      best_score = score;
    }
  }
}
void solve(){
  init();
  i32 nrepeat = (n <= 30 ? 2 : 1);
  const double TL1 = 1.5;
  const double TL2 = 1.94;
  const double TL3 = 1.98;
  for(i32 irep = 0; irep < (i32)(nrepeat); ++irep) {
    double tl1 = (irep+1) * (TL1 / nrepeat);
    double tl2 = (irep+1) * (TL2 / nrepeat);
    double tl3 = (irep+1) * (TL3 / nrepeat);
    solve1(tl1, tl2, tl3);
  }
  { cerr << "Score: " << best_score << endl;
    { i32 i = 0;
      while(best_solution[i] != 0) i++;
      rotate(begin(best_solution),begin(best_solution)+i,end(best_solution));
    }
    i32 na = best_solution.size();
    string ans;
    for(i32 i = 0; i < (i32)(na); ++i) {
      i32 x = best_solution[i];
      i32 y = best_solution[(i+1)%na];
      char c = 'A';
      for(i32 d = 0; d < (i32)(4); ++d) if(G[x][d] == y) c = DIRS[d];
      do { if(!(c != 'A')) { throw runtime_error("main.cpp" ":" "1070" " Assertion failed: " "c != 'A'"); } } while(0);
      ans += c;
    }
    cout << ans << endl;
  }
}
i32 main(){
  TM.reset();
  read();
  solve();
  cerr << "Elapsed: " << TM.elapsed() << endl;
  cerr << "[DATA] time = " << TM.elapsed() << endl;
  return 0;
}
