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
// #include "network_simplex.hpp"
struct weighted_sample {
  vector<float> base_weights;
  vector<float> weights;
  vector<i32> alias;
  weighted_sample() { }
  weighted_sample(vector<float> const& weights_) {
    weights = weights_;
    base_weights = weights_;
    i32 nweights = weights.size();
    alias.resize(nweights);
    { float tot = 0;
      for(i32 i = 0; i < (i32)(nweights); ++i) tot += weights[i];
      for(i32 i = 0; i < (i32)(nweights); ++i) base_weights[i] /= tot;
      for(i32 i = 0; i < (i32)(nweights); ++i) weights[i] /= (tot / nweights);
    }
    vector<i32> large, small;
    for(i32 i = 0; i < (i32)(nweights); ++i) {
      if(weights[i] >= 1.0) large.emplace_back(i);
      else small.emplace_back(i);
    }
    while(!small.empty() && !large.empty()) {
      auto ismall = small.back(); small.pop_back();
      auto ilarge = large.back(); large.pop_back();
      alias[ismall] = ilarge;
      weights[ilarge] = weights[ilarge] + weights[ismall] - 1.0;
      if(weights[ilarge] >= 1.0) large.emplace_back(ilarge);
      else small.emplace_back(ilarge);
    }
    while(!small.empty()) {
      auto ismall = small.back(); small.pop_back();
      weights[ismall] = 1.0;
    }
    while(!large.empty()) {
      auto ilarge = large.back(); large.pop_back();
      weights[ilarge] = 1.0;
    }
  }
  __attribute__((always_inline)) inline
  i32 sample() const {
    i32 i = rng.random32(weights.size());
    bool side = rng.randomFloat() < weights[i];
    return side ? i : alias[i];
  }
};
timer TIMER;
const f64 TL = 1.990;
const int N = 20;
const int NN = N*N;
int dist[NN][NN];
int W[NN];
int D[NN];
void read() {
  { int n; cin>>n; do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "70" " Assertion failed: " "n == N"); } } while(0); }
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) cin>>W[i*N+j];
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) cin>>D[i*N+j];
}
const int dx[4] = {-1,1,0,0};
const int dy[4] = {0,0,-1,1};
const int dxy[4] = {-N,N,-1,1};
const char* dc = "UDLR1";
int G[NN];
vector<array<int, 2>> similar_pairs;
weighted_sample pairs_sampler;
void init() {
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) for(i32 d = 0; d < (i32)(4); ++d) {
    int i2 = i+dx[d], j2 = j+dy[d];
    if(0<=i2&&i2<N&&0<=j2&&j2<N) {
      G[i*N+j] |= bit(d);
    }
  }
  for(i32 i = 0; i < (i32)(NN); ++i) for(i32 j = 0; j < (i32)(NN); ++j) {
    dist[i][j] = abs(i/N-j/N) + abs(i%N-j%N);
  }
  vector<f32> w;
  for(i32 i = (1); i <= (i32)(NN-1); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
    f64 ratio = 1.0 * min(D[i], D[j]) / max(D[i], D[j]);
    if(ratio > 0.3) {
      similar_pairs.push_back({i,j});
      w.push_back(1.0 / pow(dist[i][j], 1.5));
    }
  }
  pairs_sampler = weighted_sample(w);
  //   if(ratio > 0.25) similar_pairs.pb({i,j});
  // }
}
struct state {
  int npaths[NN];
  int paths[NN][NN];
  array<int, 2> path_of[NN];
  int path_len[NN];
  int total_len = 0;
  void reset() {
    for(i32 i = (1); i <= (i32)(NN-1); ++i) {
      npaths[i] = 1;
      paths[i][0] = i;
      path_of[i] = {i,0};
      path_len[i] = 2*dist[i][0];
      total_len += 2*dist[i][0];
    }
  }
  __attribute__((always_inline)) inline
  int sim_path_sc(int path[NN], int npath) {
    int at = 0;
    int time = 0;
    int dur = 0;
    int total_time = 0;
    for(i32 ix = 0; ix < (i32)(npath); ++ix) {
      int i = path[ix];
      if(dur >= D[i]) return -1;
      time += dist[at][i];
      at = i;
      dur += time * W[i];
    }
    return time + dist[0][at];
  }
  __attribute__((always_inline)) inline
  int sim_path(int path[NN], int lens[NN], int color[NN], int npath) {
    int at = 0;
    int time = 0;
    int dur = 0;
    int total_time = 0;
    int cur_color = 0;
    for(i32 ix = 0; ix < (i32)(npath); ++ix) {
      int i = path[ix];
      if(dur >= D[i]) {
        lens[cur_color] = time + dist[0][at];
        total_time += lens[cur_color];
        cur_color += 1;
        dur = 0;
        time = dist[0][i];
      }else{
        time += dist[at][i];
      }
      at = i;
      dur += time * W[i];
      color[ix] = cur_color;
    }
    lens[cur_color] = time + dist[0][at];
    total_time += lens[cur_color];
    return total_time;
  }
  void check_path(int pi) {
    if(npaths[pi] == 0) return;
    static int color[NN];
    static int lens[NN];
    auto b = sim_path(paths[pi], lens, color, npaths[pi]);
    do { if(!(path_len[pi] == b)) { throw runtime_error("main.cpp" ":" "183" " Assertion failed: " "path_len[pi] == b"); } } while(0);
    do { if(!(color[npaths[pi]-1] == 0)) { throw runtime_error("main.cpp" ":" "184" " Assertion failed: " "color[npaths[pi]-1] == 0"); } } while(0);
  }
  void check_paths() {
    for(i32 pi = (1); pi <= (i32)(NN-1); ++pi) check_path(pi);
    int tot = 0;
    for(i32 pi = (1); pi <= (i32)(NN-1); ++pi) tot += path_len[pi];
    if(tot != total_len) {
      do { cerr << "main.cpp" ":" "192" "  {" << "tot, total_len" << "} = {"; debug_impl_seq(tot, total_len); cerr << endl << flush; } while(0);
    }
    do { if(!(tot == total_len)) { throw runtime_error("main.cpp" ":" "194" " Assertion failed: " "tot == total_len"); } } while(0);
  }
  void transition0(f64 temp) {
    int i = 1+rng.random32(NN-1);
    int j = 1+rng.random32(NN-1);
    auto [pi, di] = path_of[i];
    auto [pj, dj] = path_of[j];
    if(pi == pj) return;
    static int new_path[NN];
    int sz = 0;
    int ai = 0, aj = 0;
    int ni = npaths[pi], nj = npaths[pj];
    while(ai < ni && aj < nj) {
      if(rng.random32(2)) {
        new_path[sz++] = paths[pi][ai++];
      }else{
        new_path[sz++] = paths[pj][aj++];
      }
    }
    while(ai < ni) {
      new_path[sz++] = paths[pi][ai++];
    }
    while(aj < nj) {
      new_path[sz++] = paths[pj][aj++];
    }
    static int color[NN];
    static int lens[NN];
    int new_length = sim_path(new_path, lens, color, sz);
    if(color[sz-1] > 1) return;
    int delta = new_length - path_len[pi] - path_len[pj];
    if(delta <= 0 || delta <= temp * rng.randomDouble()) {
      total_len += delta;
      npaths[pi] = 0;
      npaths[pj] = 0;
      for(i32 i = 0; i < (i32)(sz); ++i) {
        if(color[i] == 0) {
          path_of[new_path[i]] = {pi, npaths[pi]};
          paths[pi][npaths[pi]++] = new_path[i];
        }else{
          path_of[new_path[i]] = {pj,npaths[pj]};
          paths[pj][npaths[pj]++] = new_path[i];
        }
      }
      path_len[pi] = lens[0];
      path_len[pj] = color[sz-1] == 1 ? lens[1] : 0;
    }
  }
  void transition1(f64 temp) {
    auto [i,j] = similar_pairs[pairs_sampler.sample()];
    auto [pi, di] = path_of[i];
    auto [pj, dj] = path_of[j];
    swap(paths[pi][di], paths[pj][dj]);
    if(pi == pj) {
      auto new_length = sim_path_sc(paths[pi], npaths[pi]);
      if(new_length < 0) goto fail;
      int delta = new_length - path_len[pi];
      if(delta <= 0 || delta <= temp * rng.randomDouble()) {
        path_len[pi] = new_length;
        total_len += delta;
        swap(path_of[i], path_of[j]);
        return;
      }else{
        goto fail;
      }
    }else{
      auto new_length_i = sim_path_sc(paths[pi], npaths[pi]);
      if(new_length_i < 0) goto fail;
      auto new_length_j = sim_path_sc(paths[pj], npaths[pj]);
      if(new_length_j < 0) goto fail;
      int delta = new_length_i + new_length_j - path_len[pi] - path_len[pj];
      if(delta <= 0 || delta <= temp * rng.randomDouble()) {
        path_len[pi] = new_length_i;
        path_len[pj] = new_length_j;
        swap(path_of[i], path_of[j]);
        total_len += delta;
        return;
      }else{
        goto fail;
      }
    }
  fail:;
    swap(paths[pi][di], paths[pj][dj]);
  }
  void transition2(f64 temp) {
    // auto [__i,__j] = similar_pairs[pairs_sampler.sample()];
    // auto [__i,__j] = 
    int __i = 1+rng.random32(NN-1);
    int __j = 1+rng.random32(NN-1);
    if(__i == __j) return;
    auto [pi, di] = path_of[__i];
    auto [pj, dj] = path_of[__j];
    if(pi == pj) return;
    int ai = rng.random32(npaths[pi]);
    int bi = rng.random32(npaths[pi]);
    if(ai > bi) swap(ai, bi);
    int aj = rng.random32(npaths[pj]);
    int bj = rng.random32(npaths[pj]);
    if(aj > bj) swap(aj, bj);
    if(ai == bi && aj == bj) return;
    static int new_path_i[NN];
    static int new_path_j[NN];
    int szi = 0, szj = 0;
    for(i32 i = (0); i <= (i32)(ai-1); ++i) new_path_i[szi++] = paths[pi][i];
    for(i32 i = (aj); i <= (i32)(bj-1); ++i) new_path_i[szi++] = paths[pj][i];
    for(i32 i = (bi); i <= (i32)(npaths[pi]-1); ++i) new_path_i[szi++] = paths[pi][i];
    int new_length_i = sim_path_sc(new_path_i, szi);
    if(new_length_i < 0) return;
    for(i32 i = (0); i <= (i32)(aj-1); ++i) new_path_j[szj++] = paths[pj][i];
    for(i32 i = (ai); i <= (i32)(bi-1); ++i) new_path_j[szj++] = paths[pi][i];
    for(i32 i = (bj); i <= (i32)(npaths[pj]-1); ++i) new_path_j[szj++] = paths[pj][i];
    // debug(vector<int>(paths[pi], paths[pi]+npaths[pi]));
    // debug(vector<int>(new_path_i, new_path_i+szi));
    // debug(vector<int>(paths[pj], paths[pj]+npaths[pj]));
    // debug(vector<int>(new_path_j, new_path_j+szj));
    int new_length_j = sim_path_sc(new_path_j, szj);
    if(new_length_j < 0) return;
    int delta = new_length_i + new_length_j - path_len[pi] - path_len[pj];
    if(delta <= 0 || delta <= temp * rng.randomDouble()) {
      total_len += delta;
      npaths[pi] = szi;
      npaths[pj] = szj;
      path_len[pi] = new_length_i;
      path_len[pj] = new_length_j;
      for(i32 i = 0; i < (i32)(szi); ++i) {
        path_of[new_path_i[i]] = {pi,i};
        paths[pi][i] = new_path_i[i];
      }
      for(i32 i = 0; i < (i32)(szj); ++i) {
        path_of[new_path_j[i]] = {pj,i};
        paths[pj][i] = new_path_j[i];
      }
    }
  }
  void transition(f64 temp) {
    int ty = rng.random32(34);
    if(ty <= 1) transition0(temp);
    else if(ty <= 6) transition2(temp);
    else transition1(temp);
    // check_paths();
  }
  // void lns() {
  //   vector<vector<int>> L, R;
  //   FORU(i, 1, NN-1) if(npaths[i] > 0) {
  //     int split = rng.random32(npaths[i]+1);
  //     vector<int> l,r;
  //     FOR(j, npaths[i]) {
  //       if(j < split) l.pb(paths[i][j]);
  //       else r.pb(paths[i][j]);
  //     }
  //     if(!l.empty()) L.pb(l);
  //     if(!r.empty()) R.pb(r);
  //   }
  //   static int path[NN];
  //   static int color[NN];
  //   static int lens[NN];
  //   static int CL[NN];
  //   static int CR[NN];
  //   static int C[NN][NN];
  //   static int E[NN][NN];
  //   int nl = L.size(), nr = R.size();
  //   FOR(i, nl) {
  //     int sz = 0;
  //     for(auto x : L[i]) path[sz++] = x;
  //     auto c = sim_path(path, lens, color, sz);
  //     if(color[sz-1] != 0) {
  //       CL[i] = -1;
  //     }else{
  //       CL[i] = c;
  //     }
  //   }
  //   FOR(i, nr) {
  //     int sz = 0;
  //     for(auto x : R[i]) path[sz++] = x;
  //     auto c = sim_path(path, lens, color, sz);
  //     if(color[sz-1] != 0) {
  //       CR[i] = -1;
  //     }else{
  //       CR[i] = c;
  //     }
  //   }
  //   FOR(i, nl) FOR(j, nr) {
  //     int sz = 0;
  //     for(auto x : L[i]) path[sz++] = x;
  //     for(auto x : R[j]) path[sz++] = x;
  //     auto c = sim_path(path, lens, color, sz);
  //     if(color[sz-1] != 0) {
  //       C[i][j] = -1;
  //     }else{
  //       C[i][j] = c;
  //     }
  //   }
  //   network_simplex<int, i64> flow(2 + nl + nr);
  //   int S = nl + nr;
  //   int T = nl + nr+1;
  //   FOR(i, nl) flow.add(S, i, 1, 0);
  //   FOR(i, nr) flow.add(nl+i, T, 1, 0);
  //   flow.add(S, T, NN, 0);
  //   FOR(i, nl) if(CL[i] != -1) {
  //     flow.add(i, T, 1, CL[i]);
  //   }
  //   FOR(i, nr) if(CR[i] != -1) {
  //     flow.add(S, nl+i, 1, CR[i]);
  //   }
  //   FOR(i, nl) FOR(j, nr) if(C[i][j] != -1) {
  //     E[i][j] = flow.add(i, nl+j, 1, C[i][j]);
  //   }
  //   FOR(i, nl) flow.set_supply(i, 1);
  //   FOR(i, nr) flow.set_demand(nl+i, 1);
  //   flow.set_supply(S, NN - nl);
  //   flow.set_demand(T, NN - nr);
  //   bool ok = flow.mincost_circulation();
  //   runtime_assert(ok);
  //   auto min_cost = flow.circulation_cost();
  //   vector<bool> usedl(nl, 0), usedr(nr, 0);
  //   vector<vector<int>> new_paths;
  //   FOR(i, nl) FOR(j, nr) if(C[i][j] != -1 && flow.get_flow(E[i][j])) {
  //     usedl[i] = 1;
  //     usedr[j] = 1;
  //     vector<int> p = L[i];
  //     p.insert(end(p), all(R[j]));
  //     new_paths.pb(p);
  //   }
  //   FOR(i, nl) if(!usedl[i]) new_paths.pb(L[i]);
  //   FOR(i, nr) if(!usedr[i]) new_paths.pb(R[i]);
  //   total_len = 0;
  //   FORU(i, 1, NN-1) {
  //     npaths[i] = 0;
  //     path_len[i] = 0;
  //     if(!new_paths.empty()) {
  //       auto const& p = new_paths.back();
  //       npaths[i] = p.size();
  //       FOR(j, npaths[i]) {
  //         path_of[p[j]] = {i,j};
  //         paths[i][j] = p[j];
  //       }
  //       path_len[i] = sim_path(paths[i], lens, color, npaths[i]);
  //       total_len += path_len[i];
  //       new_paths.pop_back();
  //     }
  //   }
  // }
  vector<int> reconstruct() {
    vector<int> solution;
    int total_time = 0;
    auto move_to = [&](int& at, int to) {
      while(at != to) {
        for(i32 d = 0; d < (i32)(4); ++d) if((G[at]&bit(d)) && dist[at+dxy[d]][to] < dist[at][to]) {
          total_time += 1;
          solution.push_back(d^1);
          at += dxy[d];
          break;
        }
      }
    };
    for(i32 i = 0; i < (i32)(NN); ++i) if(npaths[i]) {
      int at = 0;
      for(i32 ix = 0; ix < (i32)(npaths[i]); ++ix) {
        int to = paths[i][ix];
        move_to(at, to);
        solution.push_back(4);
      }
      move_to(at, 0);
    }
    reverse(begin(solution), end(solution));
    return solution;
  }
};
void solve_sa(){
  static state S;
  S.reset();
  vector<int> best_S;
  bool save_best = false;
  int best_score = S.total_len;
  int niter = 0;
  f64 temp0 = 8.0;
  f64 temp1 = 1.0;
  f64 temp = temp0;
  while(1) {
    niter += 1;
    if((niter & 2047) == 0) {
      f64 done = TIMER.elapsed() / TL;
      if(done > 0.9) save_best = true;
      if(done > 1) break;
      done = pow(done, 0.6);
      temp = temp0*pow(temp1/temp0, done);
    }
    S.transition(temp);
    if(save_best && S.total_len < best_score) {
      best_score = S.total_len;
      best_S = S.reconstruct();
      // debug(niter, best_score);
    }
  }
  do { cerr << "main.cpp" ":" "532" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
  for(auto x : best_S) cout << dc[x] << endl;
}
int main() {
  TIMER.reset();
  rng.reset(time(0));
  read();
  init();
  solve_sa();
  cerr << "[DATA] time = " << (i32)(TIMER.elapsed()*1000) << endl;
  return 0;
}
