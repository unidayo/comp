#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops"
 #include <stdio.h>
 #include <vector>
 #include <fstream>
 #include <algorithm>
 #include <numeric>
 #include <queue>
 #include <iostream>
 #include <set>
 #include <map>
 #include <chrono>
 #include <memory>
 #include <cstring>
 #include <iomanip>
 #include <cmath>
 #include <array>
 #include <bitset>
 #include <type_traits>
 #include <x86intrin.h>
 #include <sys/time.h>
 #include <cassert>
 #include <climits>
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

const i32 N = 30;
const i32 TN = 465;
timer TM;
i32 src[TN];
void read(){
  for(i32 i = 0; i < (i32)(TN); ++i) cin>>src[i];
}
i32 nup[TN];
i32 Gup[TN][2];
i32 ndown[TN];
i32 Gdown[TN][2];
array<i32,2> decode[TN];
float row[TN];
float sqrttable[TN+10];
u64 permhash[TN][TN];
void init(){
  {
    for(i32 i = 0; i < (i32)(N-1); ++i) {
      i32 base1 = i*(i+1)/2;
      i32 base2 = (i+1)*(i+2)/2;
      for(i32 j = 0; j < (i32)(i+1); ++j) {
        Gdown[base1+j][ndown[base1+j]++] = base2+j;
        Gdown[base1+j][ndown[base1+j]++] = base2+j+1;
        Gup[base2+j][nup[base2+j]++] = base1+j;
        Gup[base2+j+1][nup[base2+j+1]++] = base1+j;
      }
    }
  }
  for(i32 i = 0; i < (i32)(N); ++i) {
    i32 base = i*(i+1)/2;
    for(i32 j = 0; j < (i32)(i+1); ++j) {
      decode[base+j] = {i,j};
      row[base+j] = i;
    }
  }
  for(i32 i = 0; i < (i32)(TN+10); ++i) {
    sqrttable[i] = sqrt(i);
  }
  for(i32 i = 0; i < (i32)(TN); ++i) for(i32 j = 0; j < (i32)(TN); ++j) permhash[i][j] = rng.randomInt64();
};
vector<array<i32,2>> best_solution;
struct state {
  i32 perm[TN];
  i32 iperm[TN];
  void reset(){
    for(i32 i = 0; i < (i32)(TN); ++i) perm[i] = src[i];
    for(i32 i = 0; i < (i32)(TN); ++i) iperm[perm[i]] = i;
  }
  void swap(i32 i, i32 j) {
    i32 pi = perm[i], pj = perm[j];
    perm[i] = pj; iperm[pj] = i;
    perm[j] = pi; iperm[pi] = j;
  }
  bool solved() {
    for(i32 i = 0; i < (i32)(TN); ++i) for(i32 j = 0; j < (i32)(nup[i]); ++j) {
      if(perm[i] < perm[Gup[i][j]]) return false;
    }
    return true;
  }
};
struct beam_state {
  i32 perm[TN];
  i32 iperm[TN];
  i32 cur;
  i32 hist[10'000];
  float histvalue[10'000];
  i32 nhist;
  float value;
  void reset(){
    for(i32 i = 0; i < (i32)(TN); ++i) perm[i] = src[i];
    for(i32 i = 0; i < (i32)(TN); ++i) iperm[perm[i]] = i;
    cur = 0;
    nhist = 0;
    value = 0;
    for(i32 i = 0; i < (i32)(TN); ++i) {
      value += sqrttable[perm[i]] * row[i];
    }
  }
  void swap0(i32 i, i32 j) {
    i32 pi = perm[i], pj = perm[j];
    perm[i] = pj; iperm[pj] = i;
    perm[j] = pi; iperm[pi] = j;
  }
  void swap(i32 i, i32 j) {
    i32 pi = perm[i], pj = perm[j];
    perm[i] = pj; iperm[pj] = i;
    perm[j] = pi; iperm[pi] = j;
    value -= sqrttable[pi] * row[i];
    value -= sqrttable[pj] * row[j];
    value += sqrttable[pi] * row[j];
    value += sqrttable[pj] * row[i];
  }
  void do_move(i32 data) {
    hist[nhist] = cur;
    histvalue[nhist] = value;
    nhist += 1;
    swap(data/2, Gup[data/2][data%2]);
    while(cur < TN) {
      bool imp = false;
      i32 icur = iperm[cur];
      for(i32 iup = 0; iup < (i32)(nup[icur]); ++iup) if(perm[Gup[icur][iup]] > cur) {
        imp = true;
        break;
      }
      if(imp) break;
      value += 1000.0;
      cur += 1;
    }
  }
  void undo_move(i32 data) {
    nhist -= 1;
    cur = hist[nhist];
    value = histvalue[nhist];
    swap0(data/2, Gup[data/2][data%2]);
  }
};
vector<array<i32,2>> optimize(vector<array<i32,2>> sol) {
  while(1) {
    bool imp = 0;
    for(i32 i = 0; i < (i32)(sol.size()); ++i) {
      state S; S.reset();
      for(i32 j = 0; j < (i32)(sol.size()); ++j) if(i != j) S.swap(sol[j][0],sol[j][1]);
      if(S.solved()){
        vector<array<i32,2>> nsol;
        for(i32 j = 0; j < (i32)(sol.size()); ++j) if(j != i) nsol.emplace_back(sol[j]);
        sol = nsol;
        imp = 1;
      }
    }
    if(!imp) break;
  }
  return sol;
}
void save_solution(vector<array<i32,2>> sol) {
  sol = optimize(sol);
  if(best_solution.empty() || sol.size() < best_solution.size()) {
    best_solution = sol;
  }
}
void print_solution(vector<array<i32,2>> sol) {
  cout << sol.size() << endl;
  for(auto p : sol) {
    auto q0 = decode[p[0]];
    auto q1 = decode[p[1]];
    cout << q0[0] << " " << q0[1] << " " << q1[0] << " " << q1[1] << endl;
  }
}
struct euler_tour_edge {
  bool is_down;
  bool is_leaf;
  i32 data;
};
struct euler_tour {
  euler_tour_edge* ptr;
  i32 sz;
  __attribute__((always_inline)) inline void push(euler_tour_edge e) {
    ptr[sz++] = e;
  }
};
void reconstruct(vector<i32> const& stack_moves){
  vector<array<i32,2>> solution;
  for(auto data : stack_moves) {
    solution.push_back({data/2, Gup[data/2][data%2]});
  }
  save_solution(solution);
}
bool traverse_euler_tour
(beam_state const& initial_beam_state,
 euler_tour const& tour_current,
 euler_tour& tour_next,
 vector<i32>& histogram,
 i32 cutoff, float cutoff_keep_probability,
 i32& lowv, i32& highv)
{
  lowv = 1ll<<20;
  highv = 0;
  beam_state S = initial_beam_state;
  static i32 stack_moves[10'000];
  i32 nstack_moves = 0;
  int ncommit = 0;
  for(i32 iedge = 0; iedge < (i32)((i32)tour_current.sz); ++iedge) {
    auto const& edge = tour_current.ptr[iedge];
    if(edge.is_down) {
      stack_moves[nstack_moves++] = edge.data;
      S.do_move(edge.data);
      if(S.cur == TN) {
        reconstruct(vector<i32>(stack_moves, stack_moves+nstack_moves));
        return true;
      }
    }else{
      if(edge.is_leaf) {
        if(edge.data < cutoff || (edge.data == cutoff && rng.randomFloat() < cutoff_keep_probability)) {
          while(ncommit < nstack_moves) {
            tour_next.push(euler_tour_edge {
                .is_down = 1,
                .is_leaf = 0,
                .data = stack_moves[ncommit],
              });
            ncommit += 1;
          }
          for(i32 iup = 0; iup < (i32)(nup[S.iperm[S.cur]]); ++iup) if(S.perm[Gup[S.iperm[S.cur]][iup]] > S.cur) {
            i32 movedata = S.iperm[S.cur]*2+iup;
            S.do_move(movedata);
            i32 v = 1'000'000 - S.value / 8;
            lowv = min(lowv, v);
            highv = max(highv, v);
            histogram[v] += 1;
            tour_next.push(euler_tour_edge {
                .is_down = 1,
                .is_leaf = 0,
                .data = (i32)movedata,
              });
            tour_next.push(euler_tour_edge {
                .is_down = 0,
                .is_leaf = 1,
                .data = (i32)v,
              });
            S.undo_move(movedata);
          }
        }
      }
      if(nstack_moves == 0) {
        tour_next.push(euler_tour_edge {
            .is_down = 0,
            .is_leaf = 0,
            .data = 0,
          });
        return false;
      }
      i32 d = stack_moves[nstack_moves-1];
      S.undo_move(d);
      if(ncommit == nstack_moves) {
        tour_next.push(euler_tour_edge {
            .is_down = 0,
            .is_leaf = 0,
            .data = 0,
          });
        ncommit -= 1;
      }
      nstack_moves -= 1;
    }
  }
  assert(false);
}
euler_tour_edge *tour0, *tour1;
void beam_search(int width) {
  beam_state initial_beam_state; initial_beam_state.reset();
  timer TM;
  const int max_score = 1'000'000;
  vector<int> histogram(max_score+1, 0);
  euler_tour tour_current, tour_next;
  tour_current.ptr = tour0;
  tour_current.sz = 0;
  tour_next.ptr = tour1;
  tour_next.sz = 0;
  tour_current.push(euler_tour_edge{
      .is_down = 0,
      .is_leaf = 1,
      .data = 0,
    });
  i32 cutoff = max_score;
  float cutoff_keep_probability = 1.0;
  i32 niter = 0;
  while(1) {
    niter++;
    if(niter%100==0) do { cerr << "main.cpp" ":" "325" "  {" << "niter" << "} = {"; debug_impl_seq(niter); cerr << endl << flush; } while(0);
    i32 lowv, highv;
    if(traverse_euler_tour(initial_beam_state,
                           tour_current, tour_next,
                           histogram,
                           cutoff, cutoff_keep_probability,
                           lowv, highv)){
      break;
    }
    if(TM.elapsed() > 1.975) return;
    i32 lwidth = TM.elapsed() > 1.8 ? width/10 : width;
    { i32 total_count = 0;
      cutoff = max_score;
      cutoff_keep_probability = 1.0;
      for(i32 i = (lowv); i <= (i32)(highv); ++i) {
        if(total_count+histogram[i] > lwidth) {
          cutoff = i;
          cutoff_keep_probability = (float)(lwidth-total_count) / (float)histogram[i];
          break;
        }
        total_count += histogram[i];
      }
      for(i32 i = (lowv); i <= (i32)(highv); ++i) histogram[i] = 0;
    }
    swap(tour_current, tour_next);
    tour_next.sz = 0;
  }
}
void solve(){
  for(i32 i = 0; i < (i32)(3); ++i) beam_search(2000);
  print_solution(best_solution);
}
i32 main(){
  TM.reset();
  rng.reset(time(0));
  tour0 = new euler_tour_edge[10'000'000];
  tour1 = new euler_tour_edge[10'000'000];
  read();
  init();
  solve();
  cerr << "Elapsed: " << TM.elapsed() << endl;
  cerr << "Steps: " << best_solution.size() << endl;
  cerr << "Score: " << 100000-5*best_solution.size() << endl;
  return 0;
}
