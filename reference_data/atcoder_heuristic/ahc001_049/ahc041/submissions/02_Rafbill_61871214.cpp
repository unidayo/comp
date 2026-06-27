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
u64 bit(u64 x) { return 1ull<<x; }

const f32 TL = 1.995;
timer TIMER;
f32 get_elapsed() {
  return TIMER.elapsed();
}
const int N = 1000;
const int M = 3000;
const int H = 11;
i32 m;
i32 A[N];
template<class T>
struct carray {
  i64 n;
  vector<i64> pos;
  vector<T> data;
  void init(vector<vector<T>> const& G) {
    i64 cur = 0;
    n = G.size();
    pos.resize(n+1, 0);
    i64 ne = 0;
    for(i32 i = 0; i < (i32)(n); ++i) ne += G[i].size();
    data.resize(ne);
    for(i32 i = 0; i < (i32)(n); ++i) {
      for(auto j : G[i]) {
        data[cur] = j;
        cur += 1;
      }
      pos[i+1] = cur;
    }
  }
  template<class F>
  __attribute__((always_inline)) inline
  void iter(i64 i, F&& f) const {
    for(i32 x = (pos[i]); x <= (i32)(pos[i+1]-1); ++x) f(data[x]);
  }
  i64 size(i64 i) const {
    return pos[i+1]-pos[i];
  }
  i64 get(i64 i, i64 j) const {
    return data[pos[i]+j];
  }
  T sample(i64 i) const {
    return data[pos[i]+rng.random32(pos[i+1]-pos[i])];
  }
  template<class F>
  void sort(i32 i, F&& f) {
    std::sort(begin(data)+pos[i], begin(data)+pos[i+1], f);
  }
  void shuffle(i32 i) {
    rng.shuffle(data.data()+pos[i], data.data()+pos[i+1]);
  }
};
carray<i32> G;
carray<i32> Gsorted;
void read() {
  { i32 n,h;
    cin>>n>>m>>h;
    do { if(!(n == N && h+1 == H)) { throw runtime_error("main.cpp" ":" "74" " Assertion failed: " "n == N && h+1 == H"); } } while(0);
  }
  for(i32 i = 0; i < (i32)(N); ++i) cin>>A[i];
  vector<vector<i32>> G0(N);
  for(i32 i = 0; i < (i32)(m); ++i) {
    i32 u,v; cin>>u>>v;
    G0[u].push_back(v);
    G0[v].push_back(u);
  }
  G.init(G0);
  Gsorted.init(G0);
  for(i32 i = 0; i < (i32)(N); ++i) Gsorted.sort(i, [&](i32 a, i32 b) { return A[a] < A[b]; });
}
struct state {
  i32 depth[N];
  i32 parent[N];
  i32 score;
  i32 queue[N];
  i32 queue_pos[N];
  int nqueue = 0;
  i32 save_score;
  i32 save_parent[N];
  i32 save_depth[N];
  i32 save[N];
  int nsave;
  void enqueue(i32 i) {
    queue_pos[i] = nqueue;
    queue[nqueue++] = i;
  }
  void dequeue(i32 i) {
    i32 p = queue_pos[i];
    swap(queue[p], queue[nqueue-1]);
    queue_pos[queue[p]] = p;
    nqueue -= 1;
    queue_pos[i] = -1;
  }
  void reset() {
    score = 0;
    nqueue = 0;
    for(i32 i = 0; i < (i32)(N); ++i) enqueue(i);
    process_queue(0);
  }
  void bfs(i32 i, i32 d, i32 gty) {
    static i32 Q[N];
    i32 nq = 0;
    Q[nq++] = i;
    dequeue(i);
    depth[i] = d;
    for(i32 iq = 0; iq < (i32)(nq); ++iq) {
      i32 i = Q[iq];
      score += (1+depth[i]) * A[i];
      if(depth[i]+1 == H) continue;
      G.shuffle(i);
      for(i32 x = (G.pos[i]); x <= (i32)(G.pos[i+1]-1); ++x) {
        auto j = G.data[x];
        if(queue_pos[j] != -1) {
          parent[j] = i;
          depth[j] = depth[i]+1;
          Q[nq++] = j;
          dequeue(j);
        }
      };
    }
  }
  void dfs(i32 i, i32 d, i32 gty) {
    depth[i] = d;
    score += (1+depth[i]) * A[i];
    dequeue(i);
    if(d+1 == H) return;
    if(gty == 0 || (gty == 1 && rng.randomFloat() <= 0.25)) {
      static f32 B[N];
      for(i32 x = (G.pos[i]); x <= (i32)(G.pos[i+1]-1); ++x) {
        auto j = G.data[x];
        B[j] = A[j] + 100 * rng.randomFloat();
      };
      G.sort(i, [&](i32 a, i32 b) __attribute__((always_inline)) { return B[a]<B[b]; });
      for(i32 x = (G.pos[i]); x <= (i32)(G.pos[i+1]-1); ++x) {
        auto j = G.data[x];
        if(queue_pos[j] != -1) {
          parent[j] = i;
          dfs(j, d+1, gty);
        }
      };
    }else{
      for(i32 x = (Gsorted.pos[i]); x <= (i32)(Gsorted.pos[i+1]-1); ++x) {
        auto j = Gsorted.data[x];
        if(queue_pos[j] != -1) {
          parent[j] = i;
          dfs(j, d+1, gty);
        }
      };
    }
  };
  void process_queue_single_bfs(i32 gty, i32 i = -1) {
    if(i == -1) i = queue[rng.random32(nqueue)];
    i32 d = 0;
    parent[i] = -1;
    if(rng.random32(32) != 0){
      G.shuffle(i);
      for(i32 x = (G.pos[i]); x <= (i32)(G.pos[i+1]-1); ++x) {
        auto j = G.data[x];
        if(queue_pos[j] == -1 && depth[j]+1 < H && depth[j]+1 > d) {
          d = depth[j]+1;
          parent[i] = j;
          break;
        }
      };
    }
    bfs(i, d, gty);
  }
  void process_queue_single(i32 gty, i32 i = -1) {
    if(i == -1) i = queue[rng.random32(nqueue)];
    i32 d = 0;
    parent[i] = -1;
    if(rng.random32(32) != 0){
      G.shuffle(i);
      for(i32 x = (G.pos[i]); x <= (i32)(G.pos[i+1]-1); ++x) {
        auto j = G.data[x];
        if(queue_pos[j] == -1 && depth[j]+1 < H && depth[j]+1 > d) {
          d = depth[j]+1;
          parent[i] = j;
          break;
        }
      };
    }
    dfs(i, d, gty);
  }
  void process_queue_bfs(i32 gty) {
    while(nqueue) {
      process_queue_single_bfs(gty);
    }
  }
  void process_queue(i32 gty) {
    while(nqueue) {
      process_queue_single(gty);
    }
  }
  void enqueue_dfs(i32 i){
    if(queue_pos[i] != -1) return;
    static i32 Q[N];
    i32 nq = 0;
    Q[nq++] = i;
    enqueue(i);
    for(i32 iq = 0; iq < (i32)(nq); ++iq) {
      i32 i = Q[iq];
      save_depth[nsave] = depth[i];
      save_parent[nsave] = parent[i];
      save[nsave++] = i;
      score -= (1+depth[i])*A[i];
      for(i32 x = (G.pos[i]); x <= (i32)(G.pos[i+1]-1); ++x) {
        auto j = G.data[x];
        if(parent[j] == i && queue_pos[j] == -1) {
          Q[nq++] = j;
          enqueue(j);
        }
      };
    }
  }
  void check() {
    i32 check_score = 0;
    for(i32 i = 0; i < (i32)(N); ++i) check_score += depth[i] * A[i];
    if(check_score != score) {
      do { cerr << "main.cpp" ":" "273" "  {" << "check_score, score" << "} = {"; debug_impl_seq(check_score, score); cerr << endl << flush; } while(0);
    }
    do { if(!(check_score == score)) { throw runtime_error("main.cpp" ":" "275" " Assertion failed: " "check_score == score"); } } while(0);
  }
  void transition1(f32 temp) {
    nsave = 0;
    save_score = score;
    i32 i = rng.random32(N);
    if(rng.random32(32) == 0) {
      while(depth[i] != 0) i = parent[i];
      enqueue_dfs(i);
    }else{
      enqueue_dfs(i);
    }
    i32 gty = 1+rng.random32(2);
    if(rng.random32(10) == 0) {
      process_queue_single(gty, i);
    }
    process_queue(gty);
    auto delta = score - save_score;
    if(delta >= 0 || -delta < temp * rng.randomDouble()) {
      // check();
    }else{
      score = save_score;
      for(i32 i = 0; i < (i32)(nsave); ++i) {
        depth[save[i]] = save_depth[i];
        parent[save[i]] = save_parent[i];
      }
    }
  }
  void transition2(f32 temp) {
    nsave = 0;
    save_score = score;
    i32 i = rng.random32(N);
    enqueue_dfs(i);
    process_queue_bfs(rng.random32(3));
    auto delta = score - save_score;
    if(delta >= 0 || -delta < temp * rng.randomDouble()) {
      // check();
    }else{
      score = save_score;
      for(i32 i = 0; i < (i32)(nsave); ++i) {
        depth[save[i]] = save_depth[i];
        parent[save[i]] = save_parent[i];
      }
    }
  }
  void transition(f32 temp) {
    i32 ty = rng.random32(16);
    if(ty <= 0) transition2(temp);
    else if(ty <= 15) transition1(temp);
    // else transition3(temp);
  }
};
// state init_beam() {
//   const i32 W = 1;
//   vector<vector<state>> BEAM(N+1);
//   { state I;
//     I.reset();
//     BEAM[0].eb(I);
//   }
//   FOR(i, N+1) {
//     sort(all(BEAM[i]), [&](auto const& a, auto const& b){
//       return a.score > b.score;
//     });
//     if(BEAM[i].size() > W) BEAM[i].resize(W);
//     FOR(k, 2) for(auto S : BEAM[i]) if(S.nqueue) {
//         S.process_queue_single(0);
//         BEAM[N - S.nqueue].pb(S);
//     }
//   }
//   return BEAM[N][0];
// }
void solve(){
  state S;
  S.reset();
  state best_S = S;
  do { cerr << "main.cpp" ":" "365" "  {" << "S.score" << "} = {"; debug_impl_seq(S.score); cerr << endl << flush; } while(0);
  i64 niter = 0;
  f32 temp0 = 7500;
  f32 temp1 = 30;
  f32 temp = temp0;
  while(1) {
    niter += 1;
    if(niter % 512 == 0) {
      f32 done = TIMER.elapsed() / TL;
      if(done > 1.0) break;
      temp = temp0 * pow(temp1/temp0, pow(done, 0.425));
      if(done > 0.99) temp = 0.0;
    }
    S.transition(temp);
    if(S.score > best_S.score) {
      best_S = S;
      // debug(best_S.score);
    }
  }
  do { cerr << "main.cpp" ":" "390" "  {" << "niter" << "} = {"; debug_impl_seq(niter); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "391" "  {" << "best_S.score" << "} = {"; debug_impl_seq(best_S.score); cerr << endl << flush; } while(0);
  for(i32 i = 0; i < (i32)(N); ++i) {
    cout << best_S.parent[i] << ' ';
  }
  cout << endl;
}
int main() {
  rng.reset(time(0));
  read();
  solve();
  cerr << "[DATA] time = " << (int)(TIMER.elapsed()*1000) << endl;
  return 0;
}
