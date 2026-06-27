// -*- compile-command: "make" -*-
#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
 #include <ext/pb_ds/assoc_container.hpp>
 #include <immintrin.h>
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
  template<class T, size_t N>
  void shuffle(array<T, N>& v) {
    for(i32 i = N; i > 1; i--) {
      i32 p = random32(i);
      swap(v[i-1],v[p]);
    }
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
u64 popcount(u64 x) { return __builtin_popcountll(x); }
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
// more
template<class T, size_t SZ>
struct smallvec {
  int size;
  T elems[SZ];
  __attribute__((always_inline)) inline void reset() { size = 0; }
  __attribute__((always_inline)) inline void push(T const& x) { elems[size++] = x; }
};
struct union_find {
  vector<i32> A, S;
  union_find(i32 n = 0) : A(n) {
    iota(begin(A), end(A), 0);
  }
  i32 find(i32 a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }
  bool unite(i32 a, i32 b) {
    a = find(a); b = find(b);
    if(a != b) {
      A[a] = b;
      return true;
    }else{
      return false;
    }
  }
};
const f32 TIME_FACTOR = 1.0;
const f32 TL = 1.96 * TIME_FACTOR;
timer TIMER;
const int MAXN = 42;
const int MAXNN = 2048;
int n, nn;
int source;
int target;
const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, -1, 0, 1};
int dxy[4];
int dxy8[8];
bool is_border[MAXN*MAXN];
bool initial_trees[MAXN*MAXN];
void read() {
  cin>>n; n += 2; nn = n*n;
  cerr << "[DATA] N = " << (n-2) << endl;
  { int tx, ty; cin>>tx>>ty;
    target = (n+1) + tx*n+ty;
  }
  for(i32 i = 0; i < (i32)(n); ++i) for(i32 j = 0; j < (i32)(n); ++j) {
    if(i==0||i+1==n||j==0||j+1==n) {
      is_border[i*n+j] = 1;
      initial_trees[i*n+j] = 1;
    }
  }
  for(i32 i = 0; i < (i32)(n-2); ++i) for(i32 j = 0; j < (i32)(n-2); ++j) {
    char c; cin>>c;
    initial_trees[(n+1) + i*n+j] = (c == 'T');
  }
  source = (n+1) + ((n-2)/2);
  do{}while(0);
  int K = 0;
  for(i32 i = 0; i < (i32)(nn); ++i) if(!is_border[i] && initial_trees[i]) K += 1;
  cerr << "[DATA] K = " << 1.0*K/((n-2)*(n-2)/6.0) << endl;
  dxy[0] = -n;
  dxy[1] = n;
  dxy[2] = -1;
  dxy[3] = 1;
  dxy8[0] = -n-1;
  dxy8[1] = -n;
  dxy8[2] = -n+1;
  dxy8[3] = -1;
  dxy8[4] = +1;
  dxy8[5] = +n-1;
  dxy8[6] = +n;
  dxy8[7] = +n+1;
}
namespace util {
  string color(f32 val) {
    val = clamp<f32>(val, 0.0, 1.0);
    f32 r,g,b;
    if(val < 0.5) {
      f32 x = val * 2.0;
      r = 30. * (1.0 - x) + 144. * x;
      g = 144. * (1.0 - x) + 255. * x;
      b = 255. * (1.0 - x) + 30. * x;
    }else{
      f32 x = val * 2.0 - 1.0;
      r = 144. * (1.0 - x) + 255. * x;
      g = 255. * (1.0 - x) + 30. * x;
      b = 30. * (1.0 - x) + 70. * x;
    }
    ostringstream os;
    os << setw(2) << setfill('0') << hex << ((int)r);
    os << setw(2) << setfill('0') << hex << ((int)g);
    os << setw(2) << setfill('0') << hex << ((int)b);
    return os.str();
  }
  string svg_for_prob
  (vector<u8> const& is_tree,
   vector<f32> const& p) {
    int D = 800 / (n-2);
    int W = D * (n-2);
    int H = D * (n-2);
    string svg;
    svg +=
      "<svg height=\"805\" id=\"vis\" stroke=\"red\" stroke-width=\"2\" style=\"background-color:white\" viewBox=\"-5 -5 " + to_string(W+10) + " " + to_string(H+10) + "\" width=\"805\" xmlns=\"http://www.w3.org/2000/svg\">\n"
      "<style>\n"
      "text {text-anchor: middle;dominant-baseline: central;}\n"
      "</style>\n";
    for(i32 i = 0; i < (i32)(n-2); ++i) for(i32 j = 0; j < (i32)(n-2); ++j) {
      string col;
      int u = (i+1)*n + (j+1);
      auto prob = p[u];
      if(is_tree[u]) {
        col = "444444";
      } else {
        col = color(prob);
      }
      svg += "<g>\n";
      svg += "<title>(" + to_string(i) + "," + to_string(j) + ") " + to_string(prob) + "</title>\n";
      svg += "<rect fill=\"#" + col + "\" height=\"" + to_string(D) + "\" stroke=\"black\" stroke-width=\"1\" width=\"" + to_string(D) + "\" x=\"" + to_string(j*D) + "\" y=\"" + to_string(i*D) + "\"/>\n";
      svg += "</g>\n";
    }
    svg += "</svg>\n";
    return svg;
  }
}
u64 hash_open[MAXNN];
void init() {
  for(i32 i = 0; i < (i32)(nn); ++i) hash_open[i] = rng.randomInt64();
}
namespace UF {
static int uf[2*MAXNN];
static array<int,2> uf_history[MAXNN*16];
static int uf_nhistory;
void uf_init() {
  for(i32 i = 0; i < (i32)(nn); ++i) uf[i] = i;
  for(i32 i = 0; i < (i32)(nn); ++i) uf[MAXNN+i] = 1;
  uf_nhistory = 0;
  for(i32 i = 0; i < (i32)(nn); ++i) if(is_border[i]) uf[i] = 0;
}
int uf_find(int x) {
  while(x != uf[x]) {
    uf_history[uf_nhistory++] = {x,uf[x]};
    uf[x] = uf[uf[x]];
    x = uf[x];
  }
  return x;
}
void uf_unite(int x, int y) {
  x = uf_find(x);
  y = uf_find(y);
  if(x == y) return;
  if(uf[MAXNN+x] > uf[MAXNN+y]) swap(x,y);
  uf_history[uf_nhistory++] = {x,uf[x]};
  uf_history[uf_nhistory++] = {MAXNN+y,uf[MAXNN+y]};
  uf[x] = y;
  uf[MAXNN+y] += uf[MAXNN+x];
}
void uf_rollback_to(int to) {
  while(uf_nhistory > to) {
    auto [x,y] = uf_history[--uf_nhistory];
    uf[x] = y;
  }
}
}
const int NUM_PRIO = 3;
static int date = 0;
static int vis_last[MAXNN];
static int open_last[MAXNN];
static i16 parent[MAXNN]; // parent in the tree
static i16 parent_dir[MAXNN]; // parent in the tree
static i16 dist[MAXNN]; // distance from the root
static i16 T[MAXNN]; // bfs-order of tree vertices;
static i16 degree[MAXNN];
static i16 G[MAXNN][4];
static f32 prob[MAXNN];
static i16 tree_size[MAXNN];
static i16 reveals[MAXNN][4];
static i16 nreveals[MAXNN];
static i16 reveal_count[MAXNN];
u64 last_hash = 0xABCDEF;
f32 last_eval = -1.0;
i64 num_same_hash = 0;
int best_target_tree_size = MAXNN;
void init_search() {
  UF::uf_init();
}
tuple<u64, f32> evaluate_finish(int nt, u64 hash) {
  { if(open_last[target] != date) return {hash,0};
    if(degree[parent[target]] != 1) return {hash,0.001};
  }
  if(last_hash == hash) {
    num_same_hash += 1;
    return {last_hash, last_eval};
  }
  f32 score = 0.0;
  for(i32 it = 0; it < (i32)(nt); ++it) tree_size[T[it]] = 1;
  for(i32 u = 0; u < (i32)(nn); ++u) nreveals[u] = 0;
  for(i32 u = 0; u < (i32)(nn); ++u) if(!is_border[u] && open_last[u] != date) {
    int count = 0;
    int w;
    for(i32 d = 0; d < (i32)(4); ++d) {
      int v = u+dxy[d];
      if(open_last[v] == date) {
        count += 1;
        w = v;
      }
      while(open_last[v] == date) {
        reveals[v][nreveals[v]++] = u;
        v += dxy[d];
      }
    }
    // Ideally w is the lca of all open neighbours,
    // but computing that is too slow...
    if(count == 1) tree_size[w] += 1;
  }
  for(i32 it = (nt-1); it >= (i32)(1); --it) {
    tree_size[parent[T[it]]] += tree_size[T[it]];
  }
  if(tree_size[target] > best_target_tree_size) {
    return {hash,0.000};
  }
  for(i32 i = 0; i < (i32)(nn); ++i) if(is_border[i]) {
    reveal_count[i] = 1;
  }
  f32 average_count = 1e-9;
  auto dfs = [&](auto&& dfs, int u) -> void {
    int uf_at_enter = UF::uf_nhistory;
    prob[u] = 0.0;
    if(parent[u] != -1 && degree[u] != 0) {
      int d1=parent_dir[u];
      int d2=d1^2;
      int nsee = 0;
      for(int x = u+dxy[d2]; open_last[x] == date; x += dxy[d2]) {
        nsee += 1;
      }
      for(int x = u+dxy[d2^1]; open_last[x] == date; x += dxy[d2^1]) {
        nsee += 1;
      }
      if(nsee != 0) {
        int b1 = u+dxy[d2]+(open_last[u+dxy[d2]] == date)*dxy[d1];
        int b2 = u+dxy[d2^1]+(open_last[u+dxy[d2^1]] == date)*dxy[d1];
        if(UF::uf_find(b1) == UF::uf_find(b2)) {
          int subtree_size = tree_size[u]-1;
          for(int x = u+dxy[d1^1]; open_last[x] == date; x += dxy[d1^1]) {
            subtree_size -= 1;
          }
          prob[u] = 1.0 * nsee / subtree_size;
          average_count += prob[u];
        }else{
          prob[u] = 1.0 * nsee / (n-2)/(n-2);
        }
      }
    }
    for(i32 iw = 0; iw < (i32)(nreveals[u]); ++iw) {
      int w = reveals[u][iw];
      if(reveal_count[w] == 0) {
        for(i32 d = 0; d < (i32)(8); ++d) if(int z = w+dxy8[d]; reveal_count[z] > 0) {
          UF::uf_unite(w,z);
        }
      }
      reveal_count[w] += 1;
    }
    for(i32 iv = 0; iv < (i32)(degree[u]); ++iv) {
      int v = G[u][iv];
      dfs(dfs, v);
    }
    for(i32 iw = 0; iw < (i32)(nreveals[u]); ++iw) {
      int w = reveals[u][iw];
      reveal_count[w] -= 1;
    }
    UF::uf_rollback_to(uf_at_enter);
  };
  dfs(dfs, source);
  do { if(!(UF::uf_nhistory == 0)) { throw runtime_error("main.cpp" ":" "296" " Assertion failed: " "UF::uf_nhistory == 0"); } } while(0);
  if(prob[parent[target]] > 0.5) {
    best_target_tree_size = tree_size[target];
  }
  f32 prob_parent_target = prob[parent[target]];
  for(i32 it = (nt-1); it >= (i32)(1); --it) {
    prob[parent[T[it]]] += prob[T[it]];
    score += (average_count - prob[T[it]]) * prob[T[it]];
  }
  score /= average_count;
  score += 1'000'000.0 * prob_parent_target;
  if(prob_parent_target < 0.4999) {
    score += 5 * dist[target];
  }
  if(prob_parent_target < 1.0 / n) {
    score += 100'000.0 / tree_size[target];
  }
  return {hash,score};
}
tuple<u64, f32> evaluate_plan(vector<int> const& prio) {
  int nt = 0;
  u64 hash = 0;
  // 1. Construct the tree
  {
    date += 1;
    static i16 Q[NUM_PRIO][MAXNN];
    int nq[NUM_PRIO];
    for(i32 i = 0; i < (i32)(NUM_PRIO); ++i) nq[i] = 0;
    u64 whichq = 0;
    auto push = [&](int u) __attribute__((always_inline)) {
      if(vis_last[u] == date || prio[u] == NUM_PRIO) return;
      vis_last[u] = date;
      Q[prio[u]][nq[prio[u]]++] = u;
      whichq |= bit(prio[u]);
    };
    push(source);
    while(whichq != 0) {
      int iq = lsb(whichq);
      int u = Q[iq][--nq[iq]];
      if(nq[iq] == 0) whichq ^= bit(iq);
      parent[u] = -1;
      int nparent = 0;
      for(i32 d = 0; d < (i32)(4); ++d) if(int p = u+dxy[d]; open_last[p] == date) {
        nparent += 1;
        parent[u] = p;
        parent_dir[u] = d;
      }
      if(nparent > 1) continue;
      if(parent[u] != -1) {
        G[parent[u]][degree[parent[u]]++] = u;
        dist[u] = dist[parent[u]]+1;
      }else{
        dist[u] = 0;
      }
      degree[u] = 0;
      T[nt++] = u;
      open_last[u] = date;
      hash ^= hash_open[u];
      for(i32 d = (3); d >= (i32)(0); --d) if(int v = u+dxy[d]; !initial_trees[v]) {
        push(v);
      }
    }
  }
  return evaluate_finish(nt, hash);
}
void solve() {
  vector<array<int,2>> sa_pairs;
  for(i32 i = 0; i < (i32)(nn); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    auto dist = abs(i/n-j/n)+abs(i%n-j%n);
    if(!initial_trees[i] &&
       !initial_trees[j] &&
       dist <= 4)
      {
        sa_pairs.push_back({i,j});
      }
  }
  do{}while(0);
  rng.shuffle(sa_pairs);
  init_search();
  vector<int> active;
  for(i32 i = 0; i < (i32)(nn); ++i) if(!initial_trees[i]) active.push_back(i);
  vector<int> prio(nn);
  for(i32 i = 0; i < (i32)(nn); ++i) prio[i] = NUM_PRIO-1;
  f32 score = get<1>(evaluate_plan(prio));
  int iter = 0;
  f32 best = score;
  auto best_prio = prio;
  // n=20: {'temp0': 57.697220668791886, 'temp1': 8.102754661347278, 'alpha': 1.0846890334196921}
  // n=30: {'temp0': 184.62778348996193, 'temp1': 6.356418467951082, 'alpha': 1.0014705825175925}
  // n=40: {'temp0': 225.71004790799086, 'temp1': 6.486979518052743, 'alpha': 0.8457872127104838}.
  f32 temp0, temp1, alpha;
  if(n <= 30) {
    temp0 = std::lerp(57.697220668791886, 184.62778348996193, 1.0*(n-20)/(30-20));
    temp1 = std::lerp(8.102754661347278, 6.356418467951082, 1.0*(n-20)/(30-20));
    alpha = 1.0;
  }else{
    temp0 = std::lerp(184.62778348996193, 225.7, 1.0*(n-20)/(30-20));
    temp1 = std::lerp(6.35, 6.5, 1.0*(n-20)/(30-20));
    alpha = std::lerp(1.0, 0.85, 1.0*(n-30)/(40-30));
  }
  // f32 temp0 = env_param("AHC_TEMP0", 236.50);
  // f32 temp1 = env_param("AHC_TEMP1", 1.00);
  // f32 alpha = env_param("AHC_ALPHA", 1.00);
  f32 done = 0.0;
  f32 temp = temp0;
  i64 num_accept = 0;
  i64 num_same = 0;
  i64 num_reject = 0;
  while(1) {
    iter += 1;
    if(iter % 128 == 0) {
      done = TIMER.elapsed()/TL;
      temp = temp0*pow(temp1/temp0, pow(done, alpha));
      if(iter % 1024 == 0) do{}while(0);
      if(done > 1.0) break;
    }
    int ty = 0;
    static array<int,2> changes[MAXNN];
    int nchanges = 0;
    if(score < 1000.0 && iter % 128 == 0) {
      for(int i : active) {
        changes[nchanges++] = {i,prio[i]};
      }
    }else if(ty == 0) {
      auto [i,j] = sa_pairs[iter%sa_pairs.size()];
      changes[nchanges++] = {i,prio[i]};
      changes[nchanges++] = {j,prio[j]};
    }
    for(i32 i = 0; i < (i32)(nchanges); ++i) {
      auto [x,oldv] = changes[i];
      int newv;
      do { newv = rng.random32(NUM_PRIO); } while(newv == oldv);
      if(rng.random32(48) == 0) newv = NUM_PRIO;
      prio[x] = newv;
    }
    auto [new_hash, new_score] = evaluate_plan(prio);
    // { vector<int> is_tree(nn);
    //   FOR(i, nn) is_tree[i] = open_last[i] != date;
    //   auto [chk_hash, chk_score] = evaluate_tree(is_tree);
    //   runtime_assert(new_hash == chk_hash);
    //   runtime_assert(abs(new_score-chk_score) < 1e-4);
    // }
    f32 delta = new_score - score;
    if(delta >= 0 || -delta <= temp*rng.randomDouble()) {
      num_accept += 1;
      if(delta == 0) num_same += 1;
      score = new_score;
      last_hash = new_hash;
      last_eval = new_score;
    }else{
      num_reject += 1;
      for(i32 i = 0; i < (i32)(nchanges); ++i) {
        auto [x,oldv] = changes[i];
        prio[x] = oldv;
      }
    }
    if(score > best) {
      best = score;
      best_prio = prio;
    }
  }
  cerr << "[DATA] found = " << (score > 1000000) << endl;
  do{}while(0);
  evaluate_plan(best_prio);
  cerr << "[DATA] iter = " << iter << endl;
  // VISU svg
  // { vector<u8> is_tree(nn);
  //   FOR(i, nn) is_tree[i] = open_last[i] != date;
  //   string svg = util::svg_for_prob(is_tree, vector<f32>(prob,prob+nn));
  //   ofstream os("visu_main.html"); os << svg << endl;
  // }
  vector<int> new_trees;
  for(i32 i = 0; i < (i32)(nn); ++i) if(!initial_trees[i] && open_last[i] != date) new_trees.push_back(i);
  // Was this an interactive problem?
  cout << new_trees.size();
  for(int t : new_trees) cout << " " << t/n-1 << " " << t%n-1;
  cout << endl;
  cout << -1 << endl;
}
int main() {
  cerr << setprecision(5) << fixed;
  TIMER.reset();
  read();
  init();
  solve();
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
