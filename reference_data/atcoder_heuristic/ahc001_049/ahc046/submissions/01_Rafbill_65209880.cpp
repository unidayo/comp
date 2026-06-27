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

const f64 TL = 1.9;
timer TIMER;
const int N = 20;
const int NN = N*N;
const int M = 40;
array<int, 2> targets[M];
int targeti[N][N];
void read() {
  { int n,m; cin>>n>>m;
    do { if(!(n == N && m == M)) { throw runtime_error("main.cpp" ":" "16" " Assertion failed: " "n == N && m == M"); } } while(0);
  }
  for(i32 i = 0; i < (i32)(M); ++i) {
    for(i32 k = 0; k < (i32)(2); ++k) cin>>targets[i][k];
    targeti[targets[i][0]][targets[i][1]] = i;
  }
}
u64 hash_built[NN];
u64 hash_can_build[NN];
void init() {
  for(i32 i = 0; i < (i32)(NN); ++i) hash_built[i] = rng.randomInt64();
  for(i32 i = 0; i < (i32)(NN); ++i) hash_can_build[i] = rng.randomInt64();
}
bool in_bounds(int x, int y){
  return !(x < 0 || x >= N || y < 0 || y >= N);
}
struct history_list {
  int x,y;
  int prev;
};
const int NHPOOL = 1<<24;
history_list hpool[NHPOOL];
int next_hpool = 0;
int make_history(int x, int y, int p) {
  hpool[next_hpool] = {x,y,p};
  return next_hpool++;
}
struct state {
  bitset<NN> built;
  bitset<NN> can_build;
  int cost;
  u64 h;
  int history;
  void reset() {
    built.reset();
    can_build.reset();
    cost = 0;
    h = 0;
    history = -1;
  }
  void set_built(int i, bool b) {
    if(built[i] != b) {
      h ^= hash_built[i];
      built[i] = b;
    }
  }
  void set_can_build(int i, bool b) {
    if(can_build[i] != b) {
      h ^= hash_can_build[i];
      can_build[i] = b;
    }
  }
  bool blocked(int x, int y) const {
    return (x < 0 || x >= N || y < 0 || y >= N || built[x*N+y]);
  }
  bool buildable(int x, int y) const {
    return can_build[x*N+y];
  }
};
const int INF = 1e6;
const int dx[4] = {-1,1,0,0};
const int dy[4] = {0,0,-1,1};
const char* dc = "UDLR";
template<class T>
void bfs(state const& st, int step, int num_children, T&& f) {
  auto [fx, fy] = targets[step];
  auto [tx, ty] = targets[step+1];
  static bool avoid_build[N][N];
  for(i32 x = 0; x < (i32)(N); ++x) for(i32 y = 0; y < (i32)(N); ++y) avoid_build[x][y] = 0;
 retry:;
  {
    static bool seen[N][N][5]; // sliding / not sliding
    static int dist[N][N][5]; // sliding / not sliding
    static array<int, 3> from[N][N][5][8];
    static int nfrom[N][N][5];
    for(i32 x = 0; x < (i32)(N); ++x) for(i32 y = 0; y < (i32)(N); ++y) for(i32 s = 0; s < (i32)(5); ++s) {
      nfrom[x][y][s] = 0;
      dist[x][y][s] = INF;
      seen[x][y][s] = 0;
    }
    static array<int, 3> Q[N*N*10+2];
    int q0 = N*N*5+1;
    int q1 = N*N*5+1;
    auto add0 = [&](int x, int y, int s, int d, array<int,3> fr) {
      if(d < dist[x][y][s]) {
        dist[x][y][s] = d;
        Q[--q0] = {x,y,s};
        nfrom[x][y][s] = 0;
      }
      if(d == dist[x][y][s]) {
        from[x][y][s][nfrom[x][y][s]++] = fr;
      }
    };
    auto add1 = [&](int x, int y, int s, int d, array<int,3> fr) {
      if(d < dist[x][y][s]) {
        dist[x][y][s] = d;
        Q[q1++] = {x,y,s};
        nfrom[x][y][s] = 0;
      }
      if(d == dist[x][y][s]) {
        from[x][y][s][nfrom[x][y][s]++] = fr;
      }
    };
    add0(fx, fy, 4, 0, {-1,-1,-1});
    while(q0 < q1) {
      auto [x,y,s] = Q[q0++];
      if(seen[x][y][s]) continue;
      seen[x][y][s] = 1;
      if(s == 4) {
        for(i32 d = 0; d < (i32)(4); ++d) {
          int x2 = x + dx[d];
          int y2 = y + dy[d];
          if(!st.blocked(x2, y2)) {
            add1(x2, y2, 4, dist[x][y][s] + 1, {x,y,s});
            add1(x2, y2, d, dist[x][y][s] + 1, {x,y,s});
          }
          // TODO: destruct with cost 2 ?
        }
      }else{
        int x2 = x + dx[s];
        int y2 = y + dy[s];
        if(st.blocked(x2, y2)) {
          add0(x,y,4, dist[x][y][s], {x,y,s});
        }else if(st.buildable(x2, y2) && !avoid_build[x2][y2]) {
          add1(x,y,4, dist[x][y][s]+1, {x,y,s});
          add0(x2, y2, s, dist[x][y][s], {x,y,s});
        }else{
          add0(x2, y2, s, dist[x][y][s], {x,y,s});
        }
      }
    }
    // debug(step, abs(fx-tx)+abs(fy-ty), dist[tx][ty][4]);
    // debug(dist[tx][ty][4]);
    if(dist[tx][ty][4] == INF) return;
    bool any_success = false;
    for(i32 ichild = 0; ichild < (i32)(num_children); ++ichild) {
      state child = st;
      child.cost += dist[tx][ty][4];
      static array<int, 3> path[NN*10];
      int npath = 0;
      { int x = tx, y = ty, s = 4;
        while(x != -1) {
          // debug(npath, x, y, s);
          path[npath++] = {x,y,s};
          int ifr = rng.random32(nfrom[x][y][s]);
          auto [px, py, ps] = from[x][y][s][ifr];
          x = px;
          y = py;
          s = ps;
        }
      }
      reverse(path, path+npath);
      for(i32 ipath = 0; ipath < (i32)(npath); ++ipath) {
        auto [x,y,s] = path[ipath];
        if(child.blocked(x,y)) {
          child.cost += 1;
          child.set_built(x*N+y,false);
          // avoid_build[x][y] = 1;
          // goto fail_child;
        }
        child.set_can_build(x*N+y, 0);
        if(s == 4){
          for(i32 d = 0; d < (i32)(4); ++d) {
            int x2 = x+dx[d], y2 = y+dy[d];
            if(!child.blocked(x2, y2) && targeti[x2][y2] <= step) {
              child.set_can_build(x2*N+y2, 1);
            }
          }
          if(ipath > 0) {
            child.history = make_history(x,y, child.history);
          }
        }else{
          if(path[ipath+1][2] == 4) {
            int x2 = x+dx[s], y2 = y+dy[s];
            if(in_bounds(x2, y2) && !child.built[x2*N+y2]) {
              if(!child.buildable(x2,y2)) {
                avoid_build[x2][y2] = 1;
                goto fail_child;
              }
              child.set_built(x2*N+y2, 1);
              child.set_can_build(x2*N+y2, 0);
            }
          }
        }
      }
      any_success = true;
      f(child);
    fail_child:;
    }
    if(!any_success) {
      goto retry;
    }
  }
}
void reconstruct(int h) {
  vector<array<int,2>> V;
  for(; h != -1; h = hpool[h].prev) {
    auto v = hpool[h];
    V.push_back({v.x,v.y});
  }
  V.push_back(targets[0]);
  bitset<NN> built = 0;
  reverse(begin(V), end(V));
  vector<bitset<4>> builds(V.size());
  array<int, 2> unbuild_at[N][N];
  for(i32 i = 0; i < (i32)(V.size()-1); ++i) {
    auto [x1,y1] = V[i];
    auto [x2,y2] = V[i+1];
    int s = -1;
    if(x1 > x2) s = 0;
    if(x1 < x2) s = 1;
    if(y1 > y2) s = 2;
    if(y1 < y2) s = 3;
    auto proc = [&](int x, int y) {
      if(built[x*N+y]) {
        auto [a,b] = unbuild_at[x][y];
        builds[a][b] = 1;
      }
    };
    { int x = x1, y = y1;
      while(x != x2 && y != y2) {
        x += dx[s]; y += dy[s];
        proc(x,y);
      }
      proc(x2,y2);
    };
    // debug(x1, y1, x2, y2);
    if(abs(x1-x2)+abs(y1-y2) != 1) {
      int x3 = x2+dx[s], y3 = y2+dy[s];
      if(in_bounds(x3,y3)) {
        if(!built[x3*N+y3]) {
          // debug("B?", x3,y3);
          bool done = 0;
          for(i32 j = (i); j >= (i32)(0); --j) {
            auto [x4,y4] = V[j];
            for(i32 d = 0; d < (i32)(4); ++d) {
              auto x5 = x4+dx[d], y5 = y4+dy[d];
              if(x5 == x3 && y5 == y3) {
                // debug("B!", x4, y4, d);
                builds[j][d] = 1;
                done = 1;
                break;
              }
            }
            if(done) break;
          }
          do { if(!(done)) { throw runtime_error("main.cpp" ":" "298" " Assertion failed: " "done"); } } while(0);
          built[x3*N+y3] = 1;
        }
        unbuild_at[x3][y3] = {i+1, s};
      }
    }
  }
  vector<array<char, 2>> OUT;
  for(i32 i = 0; i < (i32)(V.size()-1); ++i) {
    auto [x1,y1] = V[i];
    auto [x2,y2] = V[i+1];
    int s = -1;
    if(x1 > x2) s = 0;
    if(x1 < x2) s = 1;
    if(y1 > y2) s = 2;
    if(y1 < y2) s = 3;
    for(i32 d = 0; d < (i32)(4); ++d) if(builds[i][d]) {
      OUT.push_back({'A', dc[d]});
    }
    if(abs(x1-x2)+abs(y1-y2) != 1) {
      OUT.push_back({'S', dc[s]});
    }else{
      OUT.push_back({'M', dc[s]});
    }
  }
  for(auto [x,y] : OUT) {
    cout << x << ' ' << y << endl;
  }
}
const int BW = 128;
const int BRANCH = 3;
state BEAM_A[BW];
state BEAM_B[BW * BRANCH];
void solve() {
  state best_state;
  best_state.cost = INF;
  while(TIMER.elapsed() < TL) {
    int na = 1;
    BEAM_A[0].reset();
    for(i32 step = 0; step < (i32)(M-1); ++step) {
      // if(!BEAM_A.empty()) debug(step, BEAM_A.size(), BEAM_A[0].cost);
      int nb = 0;
      for(i32 ia = 0; ia < (i32)(na); ++ia) {
        if(TIMER.elapsed() > TL) goto tl_reached;
        auto const& sa = BEAM_A[ia];
        bfs(sa, step, step == 0 ? BW*BRANCH : BRANCH, [&](state const& t){
          BEAM_B[nb++] = t;
        });
      }
      rng.shuffle(BEAM_B, BEAM_B+nb);
      sort(BEAM_B, BEAM_B+nb, [&](auto const& a, auto const& b) {
        return a.cost < b.cost;
      });
      na = 0;
      hash_set<u64> HS;
      for(i32 ib = 0; ib < (i32)(nb); ++ib) {
        auto const& s = BEAM_B[ib];
        if(HS.insert(s.h).second) {
          BEAM_A[na++] = s;
          if(na == BW) break;
        }
      }
    }
    if(na && BEAM_A[0].cost < best_state.cost) {
      do { cerr << "main.cpp" ":" "377" "  {" << "BEAM_A[0].cost" << "} = {"; debug_impl_seq(BEAM_A[0].cost); cerr << endl << flush; } while(0);
      best_state = BEAM_A[0];
    }
  }
 tl_reached:;
  do { cerr << "main.cpp" ":" "383" "  {" << "next_hpool" << "} = {"; debug_impl_seq(next_hpool); cerr << endl << flush; } while(0);
  reconstruct(best_state.history);
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
