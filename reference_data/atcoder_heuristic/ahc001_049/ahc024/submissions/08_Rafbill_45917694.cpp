#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops"
 #include <functional>
 #include <cmath>
 #include <array>
 #include <algorithm>
 #include <numeric>
 #include <queue>
 #include <vector>
 #include <map>
 #include <set>
 #include <unordered_set>
 #include <chrono>
 #include <iostream>
 #include <iomanip>
 #include <cstring>
 #include <cstdarg>
 #include <climits>
 #include <bitset>
 #include <stdio.h>
 #include <sys/types.h>
 #include <ifaddrs.h>
 #include <netinet/in.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <sys/time.h>
 #include <sys/resource.h>
 #include <sys/stat.h>
 #include <signal.h>
 #include <netdb.h>
 #include <cstdint>
 #include <cinttypes>
 #include <x86intrin.h>
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
  u32 x, y, z, w;
  RNG(u64 seed) {
    reset(seed);
  }
  RNG() {
    reset(time(0));
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
    splitmix64_state s { seed };
    x = (u32)s.splitmix64();
    y = (u32)s.splitmix64();
    z = (u32)s.splitmix64();
    w = (u32)s.splitmix64();
  }
  inline u32 xorshift128() {
    u32 t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
  }
  // // UniformRandomBitGenerator interface
  // using result_type = u32;
  // constexpr u32 min(){ return numeric_limits<u32>::min(); }
  // constexpr u32 max(){ return numeric_limits<u32>::max(); }
  // u32 operator()() { return randomInt32(); }
  inline u32 randomInt32() {
    return xorshift128();
  }
  inline u64 randomInt64() {
    return (((u64)randomInt32())<<32ll) | ((u64)randomInt32());
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

timer TM;
const double TL = 1.95;
const i32 N = 50;
const i32 NN = N*N;
const i32 M = 100;
i32 color[N][N];
unordered_set<i32> graph[M+1];
bool adj[M+1][M+1];
const i32 dx[4] = {-1,1,0,0};
const i32 dy[4] = {0,0,-1,1};
void read(){
  { i32 x; cin>>x>>x;}
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
    cin>>color[i][j];
  }
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) {
    for(i32 d = 0; d < (i32)(4); ++d) {
      i32 i2 = i+dx[d];
      i32 j2 = j+dy[d];
      if (0<=i2&&i2<N&&0<=j2&&j2<N) {
        if(color[i][j] != color[i2][j2]) graph[color[i][j]].insert(color[i2][j2]);
      }else{
        graph[color[i][j]].insert(0);
        graph[0].insert(color[i][j]);
      }
    }
  }
  for(i32 i = 0; i < (i32)(M+1); ++i) adj[i][i] = 1;
  for(i32 i = 0; i < (i32)(M+1); ++i) for(i32 j : graph[i]) adj[i][j] = 1;
}
template<class F>
static inline
void iter4(i32 x, i32 y, i32 sx, i32 sy, F&& f) {
  if(x > 0) f(x-1,y);
  if(x+1 < sx) f(x+1,y);
  if(y > 0) f(x,y-1);
  if(y+1 < sy) f(x,y+1);
}
i32 cur[N][N];
i64 last[N][N];
i64 lastcol[M+1];
i64 date = 0;
void bfs(i32 x0, i32 y0, i32 sx, i32 sy) {
  i32 c = cur[x0][y0];
  date += 1;
  static array<i32,2> Q[NN];
  i32 nq = 1;
  Q[0] = {x0,y0};
  last[x0][y0] = date;
  for(i32 iq = 0; iq < (i32)(nq); ++iq) {
    auto p = Q[iq];
    iter4(p[0],p[1],sx,sy, [&](i32 x, i32 y) {
      lastcol[cur[x][y]] = date;
      if(cur[x][y] != c) return;
      if(last[x][y] == date) return;
      last[x][y] = date;
      Q[nq++] = {x,y};
    });
    if(p[0]==0||p[0]+1==sx||p[1]==0||p[1]+1==sy) lastcol[0] = date;
  }
}
struct sa_t {
  float time_limit;
  float done;
  float temp0;
  float alpha;
  float temp;
  timer t;
  sa_t() {}
  sa_t(double t0, double a, double tl) {
    temp0 = t0;
    alpha = a;
    time_limit = tl;
    updateDone();
  }
  void updateDone() {
    done = t.elapsed() / time_limit;
    temp = pow(1-done, alpha) * temp0;
  }
  bool accept(double x) {
    return (x <= 1e-6) || rng.randomDouble() * temp > x;
  }
};
float sqrtt[N+1];
i32 sx = N, sy = N;
void check() {
  for(i32 i = 0; i < (i32)(sx); ++i) for(i32 j = 0; j < (i32)(sy); ++j) {
    bfs(i,j,sx,sy);
    for(i32 i2 = 0; i2 < (i32)(sx); ++i2) for(i32 j2 = 0; j2 < (i32)(sy); ++j2) {
      do { if(!((last[i2][j2] == date) == (cur[i][j] == cur[i2][j2]))) { throw runtime_error("main.cpp" ":" "110" " Assertion failed: " "(last[i2][j2] == date) == (cur[i][j] == cur[i2][j2])"); } } while(0);
    }
  }
  for(i32 i = 0; i < (i32)(sx); ++i) for(i32 j = 0; j < (i32)(sy); ++j) {
    if(i+1<sx) do { if(!(adj[cur[i][j]][cur[i+1][j]])) { throw runtime_error("main.cpp" ":" "115" " Assertion failed: " "adj[cur[i][j]][cur[i+1][j]]"); } } while(0);
    if(j+1<sx) do { if(!(adj[cur[i][j]][cur[i][j+1]])) { throw runtime_error("main.cpp" ":" "116" " Assertion failed: " "adj[cur[i][j]][cur[i][j+1]]"); } } while(0);
    if(i==0||j==0||i+1==sx||j+1==sy) do { if(!(adj[cur[i][j]][0])) { throw runtime_error("main.cpp" ":" "117" " Assertion failed: " "adj[cur[i][j]][0]"); } } while(0);
  }
  // FOR(i,M+1) FOR(j,M) if(j>0 && adj[i][j]) {
  //   bool ok = 0;
  //   FOR(x,sx) FOR(y,sy) if(cur[x][y] == i) {
  //     iter4(x,y,sx,sy, [&](i32 x2, i32 y2) {
  //       if(
  //     });
  //   }
  //   runtime_assert(ok);
  // }
}
i32 best_sol[N][N];
i32 best_sol_score = 2501;
void solve_until(double tl){
  sx = N; sy = N;
  for(i32 i = 0; i < (i32)(N+1); ++i) sqrtt[i] = pow(i, 0.6);
  for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) cur[i][j] = color[i][j];
  float score = 0;
  i32 count_x[N];
  i32 count_y[N];
  bool include_x[N];
  bool include_y[N];
  float include_mx[N];
  float include_my[N];
  auto recalc_score = [&](){
    for(i32 i = 0; i < (i32)(sx); ++i) count_x[i] = 0;
    for(i32 i = 0; i < (i32)(sy); ++i) count_y[i] = 0;
    bool focus_x = rng.random32(2);
    if(rng.random32(2)) {
      for(i32 i = 0; i < (i32)(sx); ++i) include_x[i] = focus_x && rng.random32(3) == 0;
      for(i32 i = 0; i < (i32)(sy); ++i) include_y[i] = !focus_x && rng.random32(3) == 0;
    }else{
      i32 which = focus_x ? rng.random32(sx-1) : rng.random32(sy-1);
      for(i32 i = 0; i < (i32)(sx); ++i) include_x[i] = focus_x && i == which;
      for(i32 i = 0; i < (i32)(sy); ++i) include_y[i] = !focus_x && i == which;
    }
    for(i32 i = 0; i < (i32)(sx); ++i) include_mx[i] = include_x[i] ? 1.0 : 0.1;
    for(i32 i = 0; i < (i32)(sy); ++i) include_my[i] = include_y[i] ? 1.0 : 0.1;
    for(i32 i = 0; i < (i32)(sx); ++i) for(i32 j = 0; j < (i32)(sy); ++j) {
      if(i+1<sx) count_x[i] += cur[i][j] != cur[i+1][j];
      if(j+1<sy) count_y[j] += cur[i][j] != cur[i][j+1];
    }
    score = 0;
    for(i32 i = 0; i < (i32)(sx-1); ++i) score += include_mx[i] * sqrtt[count_x[i]];
    for(i32 i = 0; i < (i32)(sy-1); ++i) score += include_my[i] * sqrtt[count_y[i]];
  };
  recalc_score();
  auto remove_x = [&](i32 x0) {
    sx -= 1;
    for(i32 x = (x0+1); x <= (i32)(sx-1); ++x) {
      for(i32 y = 0; y < (i32)(sy); ++y) {
        cur[x][y] = cur[x+1][y];
      }
    }
    recalc_score();
  };
  auto remove_y = [&](i32 y0) {
    sy -= 1;
    for(i32 y = (y0+1); y <= (i32)(sy-1); ++y) {
      for(i32 x = 0; x < (i32)(sx); ++x) {
        cur[x][y] = cur[x][y+1];
      }
    }
    recalc_score();
  };
  sa_t SA(0.3, 0.7, tl - TM.elapsed());
  i64 niter = 0;
  while(TM.elapsed() < tl) {
    niter++;
    if(niter % 1024 == 0) {
      SA.updateDone();
    }
    if(niter % (1<<15) == 0){
      recalc_score();
    }
    // if(niter % (1<<20) == 0) {
    //   debug(niter);
    // }
    i32 x1 = rng.random32(sx), y1 = rng.random32(sy);
    array<i32,2> con[4];
    i32 ncon = 0;
    iter4(x1,y1,sx,sy, [&](i32 x, i32 y) __attribute__((always_inline)) {
      if (cur[x1][y1] == cur[x][y]) {
        con[ncon++] = {x,y};
      }
    });
    if (ncon == 0) continue;
    i32 d = rng.random32(4);
    i32 x2 = x1+dx[d], y2 = y1+dy[d];
    if(0<=x2&&x2<sx&&0<=y2&&y2<sy) {} else continue;
    i32 c1 = cur[x1][y1];
    i32 c2 = cur[x2][y2];
    if(c1 == c2) continue;
    { bool ok = 1;
      iter4(x1,y1,sx,sy, [&](i32 x, i32 y) {
        if (!adj[c2][cur[x][y]]) ok = 0;
      });
      if(x1==0||x1+1==sx||y1==0||y1+1==sy) {
        if(!adj[0][c2]) ok = 0;
      }
      if(!ok) continue;
    }
    cur[x1][y1] = c2;
    bfs(con[0][0],con[0][1],sx,sy);
    cur[x1][y1] = c1;
    bool ok = 1;
    for(i32 i = 0; i < (i32)(ncon); ++i) if(last[con[i][0]][con[i][1]] != date) ok = 0;
    for(auto j : graph[c1]) if(lastcol[j] != date) ok = 0;
    if(!ok) continue;
    float delta = 0;
    if(x1 > 0) {
      i32 d = (cur[x1-1][y1] != c2) - (cur[x1-1][y1] != c1);
      delta += include_mx[x1-1] * (sqrtt[count_x[x1-1] + d] - sqrtt[count_x[x1-1]]);
    }
    if(x1+1 < sx) {
      i32 d = (cur[x1+1][y1] != c2) - (cur[x1+1][y1] != c1);
      delta += include_mx[x1] *(sqrtt[count_x[x1] + d] - sqrtt[count_x[x1]]);
    }
    if(y1 > 0) {
      i32 d = (cur[x1][y1-1] != c2) - (cur[x1][y1-1] != c1);
      delta += include_my[y1-1] *(sqrtt[count_y[y1-1] + d] - sqrtt[count_y[y1-1]]);
    }
    if(y1+1 < sy) {
      i32 d = (cur[x1][y1+1] != c2) - (cur[x1][y1+1] != c1);
      delta += include_my[y1] * (sqrtt[count_y[y1] + d] - sqrtt[count_y[y1]]);
    }
    if(SA.accept(delta)) {
      cur[x1][y1] = c2;
      if(x1 > 0) {
        count_x[x1-1] += (cur[x1-1][y1] != c2) - (cur[x1-1][y1] != c1);
      }
      if(x1+1 < sx) {
        count_x[x1] += (cur[x1+1][y1] != c2) - (cur[x1+1][y1] != c1);
      }
      if(y1 > 0) {
        count_y[y1-1] += (cur[x1][y1-1] != c2) - (cur[x1][y1-1] != c1);
      }
      if(y1+1 < sy) {
        count_y[y1] += (cur[x1][y1+1] != c2) - (cur[x1][y1+1] != c1);
      }
      score += delta;
      if(x1+1 < sx && count_x[x1] == 0) { remove_x(x1); }
      else if(x1 > 0 && count_x[x1-1] == 0) { remove_x(x1-1); }
      if(y1 > 0 && count_y[y1-1] == 0) { remove_y(y1-1); }
      else if(y1+1 < sy && count_y[y1] == 0) { remove_y(y1); }
    }
  }
  i32 rscore = sx*sy;
  while(1) {
    bool imp = 0;
    for(i32 x1 = 0; x1 < (i32)(sx); ++x1) for(i32 y1 = 0; y1 < (i32)(sy); ++y1) {
      if(cur[x1][y1] == 0) continue;
      { bool adj0 = 0;
        iter4(x1,y1,sx,sy, [&](i32 x, i32 y) {
          if(cur[x][y] == 0) adj0 = 1;
        });
        if(x1==0||y1==0||x1+1==sx||y1+1==sy) adj0=1;
        if(!adj0) continue;
      }
      array<i32,2> con[4];
      i32 ncon = 0;
      iter4(x1,y1,sx,sy, [&](i32 x, i32 y) __attribute__((always_inline)) {
        if (cur[x1][y1] == cur[x][y]) {
          con[ncon++] = {x,y};
        }
      });
      if (ncon == 0) continue;
      i32 c1 = cur[x1][y1];
      { bool ok = 1;
        iter4(x1,y1,sx,sy, [&](i32 x, i32 y) {
          if (!adj[0][cur[x][y]]) ok = 0;
        });
        if(!ok) continue;
      }
      { cur[x1][y1] = 0;
        bfs(con[0][0],con[0][1],sx,sy);
        cur[x1][y1] = c1;
        bool ok = 1;
        for(i32 i = 0; i < (i32)(ncon); ++i) if(last[con[i][0]][con[i][1]] != date) ok = 0;
        for(auto j : graph[c1]) if(lastcol[j] != date) ok = 0;
        if(!ok) continue;
      }
      cur[x1][y1] = 0;
      imp = 1;
    }
    if(!imp) break;
  }
  for(i32 i = 0; i < (i32)(sx); ++i) for(i32 j = 0; j < (i32)(sy); ++j) {
    if(i+1<sx) do { if(!(adj[cur[i][j]][cur[i+1][j]])) { throw runtime_error("main.cpp" ":" "342" " Assertion failed: " "adj[cur[i][j]][cur[i+1][j]]"); } } while(0);
    if(j+1<sy) do { if(!(adj[cur[i][j]][cur[i][j+1]])) { throw runtime_error("main.cpp" ":" "343" " Assertion failed: " "adj[cur[i][j]][cur[i][j+1]]"); } } while(0);
  }
  // FOR(i,N-1) {
  //   cout << count_x[i] << ' ';
  // }
  // cout << endl;
  // FOR(i,N-1) {
  //   cout << count_y[i] << ' ';
  // }
  // cout << endl;
  do { cerr << "main.cpp" ":" "355" "  {" << "rscore" << "} = {"; debug_impl_seq(rscore); cerr << endl << flush; } while(0);
  if(rscore < best_sol_score) {
    for(i32 i = 0; i < (i32)(N); ++i) for(i32 j = 0; j < (i32)(N); ++j) best_sol[i][j] = (i<sx && j<sy ? cur[i][j] : 0);
    best_sol_score = rscore;
  }
}
i32 main(){
  TM.reset();
  rng.reset(time(0));
  read();
  for(i32 i = 0; i < (i32)(3); ++i) solve_until((i+1)*TL/3);
  for(i32 i = 0; i < (i32)(N); ++i) {
    for(i32 j = 0; j < (i32)(N); ++j) {
      cout << setw(2) << best_sol[i][j] << ' ';
    }
    cout << endl;
  }
  cerr << "Elapsed: " << TM.elapsed() << endl;
  // cerr << "Steps: " << best_solution.size() << endl;
  // cerr << "Score: " << 100000-5*best_solution.size() << endl;
  return 0;
}
