#pragma GCC optimize "O3,unroll-loops"
 #pragma GCC target "avx2,bmi,bmi2,popcnt,lzcnt"
 #include <stdio.h>
 #include <vector>
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
using namespace std;
template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T>
using max_queue = priority_queue<T>;
template<class T>
void print_collection(ostream& out, T const& x);
template<class A>
ostream& operator<<(ostream& out, vector<A> const& x) { print_collection(out, x); return out; }
template<class A, int N>
ostream& operator<<(ostream& out, array<A, N> const& x) { print_collection(out, x); return out; }
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
struct RNG {
  uint32_t x, y, z, w;
  RNG(uint64_t seed) {
    reset(seed);
  }
  RNG() {
    reset(time(0));
  }
  inline void reset(uint64_t seed) {
    struct splitmix64_state {
      uint64_t s;
      uint64_t splitmix64() {
        uint64_t result = (s += 0x9E3779B97f4A7C15);
        result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
        result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
        return result ^ (result >> 31);
      }
    };
    splitmix64_state s { seed };
    x = (uint32_t)s.splitmix64();
    y = (uint32_t)s.splitmix64();
    z = (uint32_t)s.splitmix64();
    w = (uint32_t)s.splitmix64();
  }
  inline uint32_t xorshift128() {
    uint32_t t = x;
    t ^= t << 11;
    t ^= t >> 8;
    x = y; y = z; z = w;
    w ^= w >> 19;
    w ^= t;
    return w;
  }
  inline uint32_t randomInt32() {
    return xorshift128();
  }
  inline uint64_t randomInt64() {
    return (((uint64_t)randomInt32())<<32ll) | ((uint64_t)randomInt32());
  }
  inline uint32_t random32(uint32_t r) {
    return (((uint64_t)randomInt32())*r)>>32;
  }
  inline uint64_t random64(uint64_t r) {
    return randomInt64()%r;
  }
  inline uint32_t randomRange32(uint32_t l, uint32_t r) {
    return l + random32(r-l+1);
  }
  inline uint64_t randomRange64(uint64_t l, uint64_t r) {
    return l + random64(r-l+1);
  }
  inline double randomDouble() {
    return (double)randomInt32() / 4294967296.0;
  }
  inline float randomFloat() {
    return (float)randomInt32() / 4294967296.0;
  }
  template<class T>
  void shuffle(T* l, T* r) {
    int sz = distance(l,r);
    for(int i = sz; i > 1; i--) {
      int p = random32(i);
      swap(l[i-1],l[p]);
    }
  }
  template<class T>
  void shuffle(vector<T>& v) {
    int sz = v.size();
    for(int i = sz; i > 1; i--) {
      int p = random32(i);
      swap(v[i-1],v[p]);
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
static inline void debug_impl_seq() {
  cerr << "}";
}
template <class T, class... V>
void debug_impl_seq(T const& t, V const&... v) {
  cerr << t;
  if(sizeof...(v)) { cerr << ", "; }
  debug_impl_seq(v...);
}
const float sa_temp_mult = 1.185966316288979;
const float sa_alpha = 1.0809128070174019;
const float when_allow = 0.5514876361108629;
const float border_size_start = 0.001995720514522783;
const float border_size_factor = 1.6897162500811698;

struct sa_t {
  float done;
  float temp0;
  float alpha;
  sa_t() = default;
  sa_t(double t0, double a) {
    temp0 = t0;
    alpha = a;
    done = 0;
  }
  bool accept(float x) {
    bool r = (x < 0) || rng.randomDouble() * pow(1-done, alpha) * temp0 > x;
    return r;
  }
};
const int dx[8] = {0,1,1,1,0,-1,-1,-1};
const int dy[8] = {1,1,0,-1,-1,-1,0,1};
timer TM;
const double TL = 4.9;
const int N = 64;
const int NN = N*N;
int n,m,nn,halfn;
struct pt { int x,y; void read() { cin>>y>>x; x = n-1-x; } };
vector<pt> pts;
using solution = vector<int>;
int best_score = 0;
solution best_solution;
struct state {
  uint64_t board0[N];
  uint64_t board1[2*N];
  uint64_t board2[N];
  uint64_t board3[2*N];
  uint64_t board4[N];
  uint64_t board5[2*N];
  uint64_t board6[N];
  uint64_t board7[2*N];
  uint64_t segments0[N];
  uint64_t segments1[2*N];
  uint64_t segments2[N];
  uint64_t segments3[2*N];
  uint64_t segments4[N];
  uint64_t segments5[2*N];
  uint64_t segments6[N];
  uint64_t segments7[2*N];
  int nqueue;
  int atqueue[8*NN];
  int queue[8*NN];
  int queue_cost[8*NN];
  int score;
  int64_t date;
  struct rect {
    int xyd[4];
    int64_t time;
  };
  int nrect;
  int qrect[NN*8];
  int atrect[NN*8];
  rect rects[NN*8];
  int rectof[NN*8];
  bool parity0;
  bool parity1;
  bool restrict_odd;
  int border_size;
  int nstack;
  struct stack_item {
    int i,x,w;
  } stack[NN*8];
  void reset(){
    memset(this, 0, sizeof(state));
    for(int32_t i = 0; i < (int32_t)(8*n*64); ++i) atqueue[i] = -1;
    for(int32_t i = 0; i < (int32_t)(8*n*64); ++i) qrect[i] = i;
    for(int32_t i = 0; i < (int32_t)(8*n*64); ++i) atrect[i] = -1;
    for(int32_t i = 0; i < (int32_t)(8*n*64); ++i) rectof[i] = -1;
    date = 0;
    parity0 = parity1 = 0;
    restrict_odd = 1;
    nstack = 0;
    border_size = 0;
  }
  void print_solution(){
    cout << nrect << endl;
    vector<int> I(nrect); iota(begin(I), end(I),0); sort(begin(I), end(I), [&](int i, int j){ return rects[qrect[i]].time < rects[qrect[j]].time; });
    for(int32_t i = 0; i < (int32_t)(nrect); ++i) {
      for(int32_t j = 0; j < (int32_t)(4); ++j) {
        int xyd = rects[qrect[I[i]]].xyd[j];
        int x = xyd/8/64, y = xyd/8%64;
        cout << y << ' ' << n-1-x << ' ';
      }
      cout << endl;
    }
  }
  void copy_from(state const& S) {
    for(int32_t i = 0; i < (int32_t)(n); ++i) board0[i] = S.board0[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) board1[i] = S.board1[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) board2[i] = S.board2[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) board3[i] = S.board3[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) board4[i] = S.board4[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) board5[i] = S.board5[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) board6[i] = S.board6[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) board7[i] = S.board7[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) segments0[i] = S.segments0[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) segments1[i] = S.segments1[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) segments2[i] = S.segments2[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) segments3[i] = S.segments3[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) segments4[i] = S.segments4[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) segments5[i] = S.segments5[i];
    for(int32_t i = 0; i < (int32_t)(n); ++i) segments6[i] = S.segments6[i];
    for(int32_t i = 0; i < (int32_t)(2*n); ++i) segments7[i] = S.segments7[i];
    while(nqueue) dequeue(queue[nqueue-1]);
    for(int32_t i = 0; i < (int32_t)(S.nqueue); ++i) enqueue(S.queue[i], S.queue_cost[S.queue[i]]);
    clear_stack();
    score = S.score;
    date = S.date;
    border_size = S.border_size;
    parity0 = S.parity0;
    parity1 = S.parity1;
    restrict_odd = S.restrict_odd;
    while(nrect) {
      auto r = rects[qrect[nrect-1]];
      for(int32_t i = 0; i < (int32_t)(4); ++i) rectof[r.xyd[i]] = -1;
      del_rect(qrect[nrect-1]);
    }
    for(int32_t i = 0; i < (int32_t)(S.nrect); ++i) {
      int irect = add_rect();
      rects[irect] = S.rects[S.qrect[i]];
      for(int32_t i = 0; i < (int32_t)(4); ++i) rectof[rects[irect].xyd[i]] = irect;
    }
  }
  __attribute__((always_inline)) inline
  int add_rect() {
    int ix = qrect[nrect];
    atrect[ix] = nrect;
    nrect += 1;
    return ix;
  }
  __attribute__((always_inline)) inline
  void del_rect(int i) {
    nrect -= 1;
    int x = atrect[i];
    atrect[i] = -1;
    swap(qrect[x], qrect[nrect]);
    atrect[qrect[x]] = x;
  }
  __attribute__((always_inline)) inline
  void dequeue(int xyd) {
    if(atqueue[xyd] == -1) return;
    stack[nstack++] = stack_item { xyd, atqueue[xyd], queue_cost[xyd] };
    atqueue[queue[nqueue-1]] = atqueue[xyd];
    swap(queue[atqueue[xyd]], queue[nqueue-1]);
    atqueue[xyd] = -1;
    nqueue -= 1;
  }
  __attribute__((always_inline)) inline
  void dequeue(int x, int y, int d) {
    dequeue((x*64+y)*8+d);
  }
  __attribute__((always_inline)) inline
  void enqueue(int xyd, int w) {
    if(atqueue[xyd] != -1) return;
    stack[nstack++] = stack_item { xyd, -1, -1 };
    atqueue[xyd] = nqueue;
    queue[nqueue++] = xyd;
    queue_cost[xyd] = w;
  }
  __attribute__((always_inline)) inline
  bool can_enqueue(int x, int y, int, int dist1, int dist2) {
    if(x < n/4-border_size || x > 3*n/4+border_size) return 0;
    if(y < n/4-border_size || y > 3*n/4+border_size) return 0;
    if((dist1&dist2&1) == 0) {
      if(restrict_odd) return 0;
    }
    return 1;
  }
  __attribute__((always_inline)) inline
  void try_enqueue(int x, int y, int d, int dist1, int dist2, int w) {
    if(x < n/4-border_size || x > 3*n/4+border_size) return;
    if(y < n/4-border_size || y > 3*n/4+border_size) return;
    if((dist1&dist2&1) == 0) {
      if(restrict_odd) return;
      w += 2;
      if((d==0||d==4)&&((parity0^x^y)%2 == 0)) w += 1;
      if((d==2||d==6)&&((parity0^x^y)%2 == 1)) w += 1;
      if((d==1||d==5)&&((parity1^x^y)%2 == 0)) w += 1;
      if((d==3||d==7)&&((parity1^x^y)%2 == 1)) w += 1;
    }
    enqueue((x*64+y)*8+d, w);
  }
  __attribute__((always_inline)) inline
  void clear_stack() { nstack = 0; }
  __attribute__((always_inline)) inline
  void undo_stack() {
    int ns = nstack;
    while(ns) {
      ns -= 1;
      auto ix = stack[ns];
      if(ix.x == -1) {
        dequeue(ix.i);
      }else{
        enqueue(ix.i,ix.w);
      }
    }
    nstack = 0;
  }
  __attribute__((always_inline)) inline
  void pre_update(int x, int y, int mask) {
#pragma GCC unroll 8
    for(int32_t d1 = 0; d1 < (int32_t)(8); ++d1) {
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) continue;
      dist1 = 1+__builtin_ctzll(dist1);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
#pragma GCC unroll 2
      for(int32_t e = 0; e < (int32_t)(2); ++e) {
        int d2 = (d1+(e?2:6))%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        do {
          int d3 = d1^4;
          uint64_t dist3 = has(x,y,d3);
          if(!dist3) continue;
          dist3 = 1+__builtin_ctzll(dist3);
          int x3=x+dist3*dx[d3],y3=y+dist3*dy[d3];
          if(!can(x2,y2,d3) || dist(x2,y2,d3) <= dist1+dist3) continue;
          if(!can(x3,y3,d2) || dist(x3,y3,d2) <= dist2) continue;
          int x4=x3+dist2*dx[d2],y4=y3+dist2*dy[d2];
          if(x4<0||y4<0||x4>=n||y4>=n) continue;
          int dd = (d2+(e?5:3))%8;
          dequeue(x4,y4,dd);
        }while(0);
        do{
          int d3 = d1^4;
          uint64_t dist3 = has(x2,y2,d3);
          if(!dist3) continue;
          dist3 = 1+__builtin_ctzll(dist3);
          if(dist3 <= dist1 && !(mask&(1u<<d1))) continue;
          int x3=x2+dist3*dx[d3],y3=y2+dist3*dy[d3];
          if(!can(x3,y3,d2^4) || dist(x3,y3,d2^4) <= dist2) continue;
          if(!can(x1,y1,d3) || dist(x1,y1,d3) <= dist3) continue;
          int x4=x1+dist3*dx[d3],y4=y1+dist3*dy[d3];
          if(x4<0||y4<0||x4>=n||y4>=n) continue;
          if(x4==x&&y4==y) continue;
          int dd = (d2+(e?7:1))%8;
          dequeue(x4,y4,dd);
        }while(0);
      }
    }
  }
  __attribute__((always_inline)) inline
  void pre_update_line(int x, int y, int d1){
#pragma GCC unroll 8
    for(int32_t d0 = 0; d0 < (int32_t)(8); ++d0) if(d0==d1) {
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) return;
      dist1 = 1+__builtin_ctzll(dist1);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
#pragma GCC unroll 2
      for(int32_t e = 0; e < (int32_t)(2); ++e) {
        int d2 = (d1+(e?2:6))%8;
        uint64_t dist2 = has(x,y,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
        if(!can(x1,y1,d2) || dist(x1,y1,d2) <= dist2) continue;
        if(!can(x2,y2,d1) || dist(x2,y2,d1) <= dist1) continue;
        int x3=x1+dist2*dx[d2],y3=y1+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+(e?5:3))%8;
        dequeue(x3,y3,dd);
      }
#pragma GCC unroll 2
      for(int32_t e = 0; e < (int32_t)(2); ++e){
        int d2 = (d1+(e?2:6))%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        int d3 = d1^4;
        uint64_t dist3 = has(x2,y2,d3);
        if(!dist3) continue;
        dist3 = 1+__builtin_ctzll(dist3);
        int x3=x2+dist3*dx[d3],y3=y2+dist3*dy[d3];
        if(!can(x3,y3,d2^4) || dist(x3,y3,d2^4) <= dist2) continue;
        if(!can(x1,y1,d3) || dist(x1,y1,d3) <= dist3) continue;
        int x4=x1+dist3*dx[d3],y4=y1+dist3*dy[d3];
        if(x4<0||y4<0||x4>=n||y4>=n) continue;
        int dd = (d2+(e?7:1))%8;
        dequeue(x4,y4,dd);
      }
    }
  }
  __attribute__((always_inline)) inline
  void post_update(int x, int y) {
#pragma GCC unroll 8
    for(int32_t d1 = 0; d1 < (int32_t)(8); ++d1) {
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) continue;
      dist1 = 1+__builtin_ctzll(dist1);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
      do{ int d2 = (d1+2)%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        if(!can(x2,y2,d1^4) || dist(x2,y2,d1^4) <= dist1) continue;
        if(!can(x,y,d2) || dist(x,y,d2) <= dist2) continue;
        int x3=x+dist2*dx[d2],y3=y+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+7)%8;
        try_enqueue(x3,y3,dd,dist1,dist2,dist1+dist2);
      }while(0);
      do {
        int d2 = (d1+2)%8;
        uint64_t dist2 = has(x,y,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
        int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
        if(!can(x1,y1,d2) || dist(x1,y1,d2) <= dist2) continue;
        if(!can(x2,y2,d1) || dist(x2,y2,d1) <= dist1) continue;
        int x3=x1+dist2*dx[d2],y3=y1+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+5)%8;
        try_enqueue(x3,y3,dd,dist1,dist2,dist1+dist2);
      }while(0);
      do {
        int d2 = (d1+6)%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        if(!can(x2,y2,d1^4) || dist(x2,y2,d1^4) <= dist1) continue;
        if(!can(x,y,d2) || dist(x,y,d2) <= dist2) continue;
        int x3=x+dist2*dx[d2],y3=y+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+1)%8;
        try_enqueue(x3,y3,dd,dist1,dist2,dist1+dist2);
      }while(0);
    }
  }
  void set_point(int x, int y) {
    score += (x-halfn)*(x-halfn) + (y-halfn)*(y-halfn) + 1;
    pre_update(x,y,0);
    for(int32_t d = 0; d < (int32_t)(8); ++d) dequeue(x,y,d);
    board0[x] |= (1ull<<y);
    board1[x+(n-1-y)] |= (1ull<<x);
    board2[y] |= (1ull<<x);
    board3[x+y] |= (1ull<<x);
    board4[x] |= (1ull<<(n-1-y));
    board5[x+(n-1-y)] |= (1ull<<(n-1-x));
    board6[y] |= (1ull<<(n-1-x));
    board7[x+y] |= (1ull<<(n-1-x));
    post_update(x,y);
  }
  __attribute__((always_inline)) inline
  void add_seg0(int dist,
                int x1, int y1,
                int, int y2) {
    segments0[x1] |= ((1ull<<dist)-1) << y1;
    segments4[x1] |= ((1ull<<dist)-1) << (n-1-y2);
  }
  __attribute__((always_inline)) inline
  void add_seg2(int dist,
                int x1, int y1,
                int x2, int) {
    segments2[y1] |= ((1ull<<dist)-1) << x1;
    segments6[y1] |= ((1ull<<dist)-1) << (n-1-x2);
  }
  __attribute__((always_inline)) inline
  void add_seg1(int dist,
                int x1, int y1,
                int x2, int) {
    segments1[x1+(n-1-y1)] |= ((1ull<<dist)-1) << x1;
    segments5[x1+(n-1-y1)] |= ((1ull<<dist)-1) << (n-1-x2);
  }
  __attribute__((always_inline)) inline
  void add_seg3(int dist,
                int x1, int y1,
                int x2, int) {
    segments3[x1+y1] |= ((1ull<<dist)-1) << x1;
    segments7[x1+y1] |= ((1ull<<dist)-1) << (n-1-x2);
  }
  __attribute__((always_inline)) inline
  void add_rect13(int dist1, int dist2,
                  int x1, int y1,
                  int x2, int y2,
                  int x3, int y3,
                  int x4, int y4) {
    add_seg1(dist1, x1,y1, x2,y2);
    add_seg1(dist1, x3,y3, x4,y4);
    add_seg3(dist2, x3,y3, x1,y1);
    add_seg3(dist2, x4,y4, x2,y2);
  }
  __attribute__((always_inline)) inline
  void add_rect02(int dist1, int dist2,
                  int x1, int y1,
                  int x2, int y2,
                  int x3, int y3,
                  int x4, int y4) {
    add_seg0(dist1, x1,y1, x2,y2);
    add_seg0(dist1, x3,y3, x4,y4);
    add_seg2(dist2, x1,y1, x3,y3);
    add_seg2(dist2, x2,y2, x4,y4);
  }
  __attribute__((always_inline)) inline
  void rem_seg0(int x1, int y1,
                int, int y2) {
    segments0[x1] ^= ((1ull<<(y2-y1))-1) << y1;
    segments4[x1] ^= ((1ull<<(y2-y1))-1) << (n-1-y2);
  }
  __attribute__((always_inline)) inline
  void rem_seg2(int x1, int y1,
                int x2, int) {
    segments2[y1] ^= ((1ull<<(x2-x1))-1) << x1;
    segments6[y1] ^= ((1ull<<(x2-x1))-1) << (n-1-x2);
  }
  __attribute__((always_inline)) inline
  void rem_seg1(int x1, int y1,
                int x2, int) {
    segments1[x1+(n-1-y1)] ^= ((1ull<<(x2-x1))-1) << x1;
    segments5[x1+(n-1-y1)] ^= ((1ull<<(x2-x1))-1) << (n-1-x2);
  }
  __attribute__((always_inline)) inline
  void rem_seg3(int x1, int y1,
                int x2, int) {
    segments3[x1+y1] ^= ((1ull<<(x2-x1))-1) << x1;
    segments7[x1+y1] ^= ((1ull<<(x2-x1))-1) << (n-1-x2);
  }
  __attribute__((always_inline)) inline
  void rem_rect13(int x1, int y1,
                  int x2, int y2,
                  int x3, int y3,
                  int x4, int y4) {
    rem_seg1(x1,y1, x2,y2);
    rem_seg1(x3,y3, x4,y4);
    rem_seg3(x3,y3, x1,y1);
    rem_seg3(x4,y4, x2,y2);
  }
  __attribute__((always_inline)) inline
  void rem_rect02(int x1, int y1,
                  int x2, int y2,
                  int x3, int y3,
                  int x4, int y4) {
    rem_seg0(x1,y1, x2,y2);
    rem_seg0(x3,y3, x4,y4);
    rem_seg2(x1,y1, x3,y3);
    rem_seg2(x2,y2, x4,y4);
  }
  void set_rect(int x, int y, int d) {
    do { if(__builtin_expect(!(atqueue[(x*64+y)*8+d] != -1), 0)) { throw runtime_error("main.cpp" ":" "537" " Assertion failed: " "atqueue[(x*64+y)*8+d] != -1"); } } while(0);
    score += (x-halfn)*(x-halfn) + (y-halfn)*(y-halfn) + 1;
    int d1 = (d+1)%8;
    int d2 = (d+7)%8;
    uint64_t dist1 = dist(x,y,d1);
    int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
    uint64_t dist2 = dist(x,y,d2);
    int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
    int x3=x2+dist1*dx[d1],y3=y2+dist1*dy[d1];
    int irect = add_rect();
    rects[irect].xyd[0] = (x*64+y) *8+d;
    rects[irect].xyd[1] = (x1*64+y1)*8+(d+6)%8;
    rects[irect].xyd[2] = (x3*64+y3)*8+(d+4)%8;
    rects[irect].xyd[3] = (x2*64+y2)*8+(d+2)%8;
    rects[irect].time = date++;
    for(int32_t i = 0; i < (int32_t)(4); ++i) {
      rectof[rects[irect].xyd[i]] = irect;
    }
    pre_update(x,y,(1u<<d1)|(1u<<d2));
    pre_update_line(x1,y1,d2);
    pre_update_line(x2,y2,d1);
    pre_update_line(x3,y3,d1^4);
    pre_update_line(x3,y3,d2^4);
    for(int32_t d = 0; d < (int32_t)(8); ++d) dequeue(x,y,d);
    board0[x] |= (1ull<<y);
    board1[x+(n-1-y)] |= (1ull<<x);
    board2[y] |= (1ull<<x);
    board3[x+y] |= (1ull<<x);
    board4[x] |= (1ull<<(n-1-y));
    board5[x+(n-1-y)] |= (1ull<<(n-1-x));
    board6[y] |= (1ull<<(n-1-x));
    board7[x+y] |= (1ull<<(n-1-x));
    if(d == 0) {
      add_rect13(dist1,dist2,x,y,x1,y1,x2,y2,x3,y3);
    }else if(d == 1) {
      add_rect02(dist2,dist1,x,y,x2,y2,x1,y1,x3,y3);
    }else if(d == 2) {
      add_rect13(dist2,dist1,x1,y1,x3,y3,x,y,x2,y2);
    }else if(d == 3) {
      add_rect02(dist1,dist2,x1,y1,x,y,x3,y3,x2,y2);
    }else if(d == 4) {
      add_rect13(dist1,dist2,x3,y3,x2,y2,x1,y1,x,y);
    }else if(d == 5) {
      add_rect02(dist2,dist1,x3,y3,x1,y1,x2,y2,x,y);
    }else if(d == 6) {
      add_rect13(dist2,dist1,x2,y2,x,y,x3,y3,x1,y1);
    }else if(d == 7) {
      add_rect02(dist1,dist2,x2,y2,x3,y3,x,y,x1,y1);
    }else{
      do { throw runtime_error("main.cpp" ":" "604" " impossible"); } while(0);
    }
    post_update(x,y);
  }
  void set_rect_fast(int x, int y, int d) {
    score += (x-halfn)*(x-halfn) + (y-halfn)*(y-halfn) + 1;
    int d1 = (d+1)%8;
    int d2 = (d+7)%8;
    uint64_t dist1 = dist(x,y,d1);
    int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
    uint64_t dist2 = dist(x,y,d2);
    int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
    int x3=x2+dist1*dx[d1],y3=y2+dist1*dy[d1];
    int irect = add_rect();
    rects[irect].xyd[0] = (x*64+y) *8+d;
    rects[irect].xyd[1] = (x1*64+y1)*8+(d+6)%8;
    rects[irect].xyd[2] = (x3*64+y3)*8+(d+4)%8;
    rects[irect].xyd[3] = (x2*64+y2)*8+(d+2)%8;
    rects[irect].time = date++;
    for(int32_t i = 0; i < (int32_t)(4); ++i) rectof[rects[irect].xyd[i]] = irect;
    board0[x] |= (1ull<<y);
    board1[x+(n-1-y)] |= (1ull<<x);
    board2[y] |= (1ull<<x);
    board3[x+y] |= (1ull<<x);
    board4[x] |= (1ull<<(n-1-y));
    board5[x+(n-1-y)] |= (1ull<<(n-1-x));
    board6[y] |= (1ull<<(n-1-x));
    board7[x+y] |= (1ull<<(n-1-x));
    if(d == 0) {
      add_rect13(dist1,dist2,x,y,x1,y1,x2,y2,x3,y3);
    }else if(d == 1) {
      add_rect02(dist2,dist1,x,y,x2,y2,x1,y1,x3,y3);
    }else if(d == 2) {
      add_rect13(dist2,dist1,x1,y1,x3,y3,x,y,x2,y2);
    }else if(d == 3) {
      add_rect02(dist1,dist2,x1,y1,x,y,x3,y3,x2,y2);
    }else if(d == 4) {
      add_rect13(dist1,dist2,x3,y3,x2,y2,x1,y1,x,y);
    }else if(d == 5) {
      add_rect02(dist2,dist1,x3,y3,x1,y1,x2,y2,x,y);
    }else if(d == 6) {
      add_rect13(dist2,dist1,x2,y2,x,y,x3,y3,x1,y1);
    }else if(d == 7) {
      add_rect02(dist1,dist2,x2,y2,x3,y3,x,y,x1,y1);
    }else{
      do { throw runtime_error("main.cpp" ":" "659" " impossible"); } while(0);
    }
  }
  __attribute__((always_inline)) inline
  void pre_remove(int x, int y) {
#pragma GCC unroll 8
    for(int32_t d1 = 0; d1 < (int32_t)(8); ++d1) {
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) continue;
      dist1 = 1+__builtin_ctzll(dist1);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
      do{ int d2 = (d1+2)%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        if(!can(x2,y2,d1^4) || dist(x2,y2,d1^4) <= dist1) continue;
        if(!can(x,y,d2) || dist(x,y,d2) <= dist2) continue;
        int x3=x+dist2*dx[d2],y3=y+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+7)%8;
        dequeue(x3,y3,dd);
      }while(0);
      do {
        int d2 = (d1+2)%8;
        uint64_t dist2 = has(x,y,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
        int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
        if(!can(x1,y1,d2) || dist(x1,y1,d2) <= dist2) continue;
        if(!can(x2,y2,d1) || dist(x2,y2,d1) <= dist1) continue;
        int x3=x1+dist2*dx[d2],y3=y1+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+5)%8;
        dequeue(x3,y3,dd);
      }while(0);
      do {
        int d2 = (d1+6)%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        if(!can(x2,y2,d1^4) || dist(x2,y2,d1^4) <= dist1) continue;
        if(!can(x,y,d2) || dist(x,y,d2) <= dist2) continue;
        int x3=x+dist2*dx[d2],y3=y+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+1)%8;
        dequeue(x3,y3,dd);
      }while(0);
    }
  }
  __attribute__((always_inline)) inline
  void post_remove(int x, int y, int mask) {
#pragma GCC unroll 8
    for(int32_t d1 = 0; d1 < (int32_t)(8); ++d1) {
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) continue;
      dist1 = 1+__builtin_ctzll(dist1);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
#pragma GCC unroll 2
      for(int32_t e = 0; e < (int32_t)(2); ++e) {
        int d2 = (d1+(e?2:6))%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        do {
          int d3 = d1^4;
          uint64_t dist3 = has(x,y,d3);
          if(!dist3) continue;
          dist3 = 1+__builtin_ctzll(dist3);
          int x3=x+dist3*dx[d3],y3=y+dist3*dy[d3];
          if(!can(x2,y2,d3) || dist(x2,y2,d3) <= dist1+dist3) continue;
          if(!can(x3,y3,d2) || dist(x3,y3,d2) <= dist2) continue;
          int x4=x3+dist2*dx[d2],y4=y3+dist2*dy[d2];
          if(x4<0||y4<0||x4>=n||y4>=n) continue;
          int dd = (d2+(e?5:3))%8;
          try_enqueue(x4,y4,dd,dist1+dist3,dist2,dist1+dist2+dist3);
        }while(0);
        do{
          int d3 = d1^4;
          uint64_t dist3 = has(x2,y2,d3);
          if(!dist3) continue;
          dist3 = 1+__builtin_ctzll(dist3);
          if(dist3 <= dist1 && !(mask&(1u<<d1))) continue;
          int x3=x2+dist3*dx[d3],y3=y2+dist3*dy[d3];
          if(!can(x3,y3,d2^4) || dist(x3,y3,d2^4) <= dist2) continue;
          if(!can(x1,y1,d3) || dist(x1,y1,d3) <= dist3) continue;
          int x4=x1+dist3*dx[d3],y4=y1+dist3*dy[d3];
          if(x4<0||y4<0||x4>=n||y4>=n) continue;
          if(x4==x&&y4==y) continue;
          int dd = (d2+(e?7:1))%8;
          try_enqueue(x4,y4,dd,dist2,dist3,dist2+dist3);
        }while(0);
      }
    }
  }
  __attribute__((always_inline)) inline
  void post_remove_line(int x, int y, int d1){
#pragma GCC unroll 8
    for(int32_t d0 = 0; d0 < (int32_t)(8); ++d0) if(d0==d1) {
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) return;
      dist1 = 1+__builtin_ctzll(dist1);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
#pragma GCC unroll 2
      for(int32_t e = 0; e < (int32_t)(2); ++e) {
        int d2 = (d1+(e?2:6))%8;
        uint64_t dist2 = has(x,y,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
        if(!can(x1,y1,d2) || dist(x1,y1,d2) <= dist2) continue;
        if(!can(x2,y2,d1) || dist(x2,y2,d1) <= dist1) continue;
        int x3=x1+dist2*dx[d2],y3=y1+dist2*dy[d2];
        if(x3<0||y3<0||x3>=n||y3>=n) continue;
        int dd = (d1+(e?5:3))%8;
        try_enqueue(x3,y3,dd,dist1,dist2,dist1+dist2);
      }
#pragma GCC unroll 2
      for(int32_t e = 0; e < (int32_t)(2); ++e){
        int d2 = (d1+(e?2:6))%8;
        uint64_t dist2 = has(x1,y1,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x2=x1+dist2*dx[d2],y2=y1+dist2*dy[d2];
        int d3 = d1^4;
        uint64_t dist3 = has(x2,y2,d3);
        if(!dist3) continue;
        dist3 = 1+__builtin_ctzll(dist3);
        int x3=x2+dist3*dx[d3],y3=y2+dist3*dy[d3];
        if(!can(x3,y3,d2^4) || dist(x3,y3,d2^4) <= dist2) continue;
        if(!can(x1,y1,d3) || dist(x1,y1,d3) <= dist3) continue;
        int x4=x1+dist3*dx[d3],y4=y1+dist3*dy[d3];
        if(x4<0||y4<0||x4>=n||y4>=n) continue;
        int dd = (d2+(e?7:1))%8;
        try_enqueue(x4,y4,dd,dist2,dist3,dist2+dist3);
      }
    }
  }
  __attribute__((always_inline)) inline
  void post_remove_point(int x, int y){
#pragma GCC unroll 8
    for(int32_t d = 0; d < (int32_t)(8); ++d) {
      int d1 = (d+1)%8;
      int d2 = (d+7)%8;
      uint64_t dist1 = has(x,y,d1);
      if(!dist1) continue;
      dist1 = 1+__builtin_ctzll(dist1);
      uint64_t dist2 = has(x,y,d2);
      if(!dist2) continue;
      dist2 = 1+__builtin_ctzll(dist2);
      int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
      int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
      if(!can(x1,y1,d2) || dist(x1,y1,d2) != dist2) continue;
      if(!can(x2,y2,d1) || dist(x2,y2,d1) != dist1) continue;
      try_enqueue(x,y,d,dist1,dist2,dist1+dist2);
    }
  }
  void unset_rect_point(int x, int y, vector<int>& stack){
    for(int32_t d = 0; d < (int32_t)(8); ++d) if(rectof[(x*64+y)*8+d] != -1) {
      unset_rect(rectof[(x*64+y)*8+d], stack);
    }
  }
  void unset_rect(int irect, vector<int>& stack) {
    auto r = rects[irect];
    int x = r.xyd[0]/8/64, y = r.xyd[0]/8%64, d = r.xyd[0]%8;
    for(int32_t i = 0; i < (int32_t)(4); ++i) rectof[r.xyd[i]] = -1;
    del_rect(irect);
    score -= (x-halfn)*(x-halfn) + (y-halfn)*(y-halfn) + 1;
    int d1 = (d+1)%8;
    int d2 = (d+7)%8;
    int x1 = r.xyd[1]/8/64, y1 = r.xyd[1]/8%64;
    int x3 = r.xyd[2]/8/64, y3 = r.xyd[2]/8%64;
    int x2 = r.xyd[3]/8/64, y2 = r.xyd[3]/8%64;
    uint64_t dist1 = max(abs(x1-x),abs(y1-y));
    uint64_t dist2 = max(abs(x2-x),abs(y2-y));
    auto plop = [&](int x, int y, int d, uint64_t dist) {
      while(1) {
        auto distt = fdist(x,y,d);
        if(distt == dist) break;
        int x2=x+distt*dx[d],y2=y+distt*dy[d];
        unset_rect_point(x2,y2,stack);
      }
    };
    plop(x,y,d1,dist1);
    plop(x,y,d2,dist2);
    plop(x3,y3,d1^4,dist1);
    plop(x3,y3,d2^4,dist2);
    for(int32_t d = 0; d < (int32_t)(8); ++d) if(rectof[(x*64+y)*8+d] != -1) {
      unset_rect(rectof[(x*64+y)*8+d], stack);
    }
    stack.emplace_back(r.xyd[0]);
    pre_remove(x,y);
    board0[x] ^= (1ull<<y);
    board1[x+(n-1-y)] ^= (1ull<<x);
    board2[y] ^= (1ull<<x);
    board3[x+y] ^= (1ull<<x);
    board4[x] ^= (1ull<<(n-1-y));
    board5[x+(n-1-y)] ^= (1ull<<(n-1-x));
    board6[y] ^= (1ull<<(n-1-x));
    board7[x+y] ^= (1ull<<(n-1-x));
    if(d == 0) {
      rem_rect13(x,y,x1,y1,x2,y2,x3,y3);
    }else if(d == 1) {
      rem_rect02(x,y,x2,y2,x1,y1,x3,y3);
    }else if(d == 2) {
      rem_rect13(x1,y1,x3,y3,x,y,x2,y2);
    }else if(d == 3) {
      rem_rect02(x1,y1,x,y,x3,y3,x2,y2);
    }else if(d == 4) {
      rem_rect13(x3,y3,x2,y2,x1,y1,x,y);
    }else if(d == 5) {
      rem_rect02(x3,y3,x1,y1,x2,y2,x,y);
    }else if(d == 6) {
      rem_rect13(x2,y2,x,y,x3,y3,x1,y1);
    }else if(d == 7) {
      rem_rect02(x2,y2,x3,y3,x,y,x1,y1);
    }else{
      do { throw runtime_error("main.cpp" ":" "888" " impossible"); } while(0);
    }
    post_remove_point(x,y);
    post_remove(x,y,(1u<<d1)|(1u<<d2));
    post_remove_line(x1,y1,d2);
    post_remove_line(x2,y2,d1);
    post_remove_line(x3,y3,d1^4);
    post_remove_line(x3,y3,d2^4);
  }
  void unset_rect_point_fast(int x, int y, vector<int>& stack){
    for(int32_t d = 0; d < (int32_t)(8); ++d) if(rectof[(x*64+y)*8+d] != -1) {
      unset_rect_fast(rectof[(x*64+y)*8+d], stack);
    }
  }
  void unset_rect_fast(int irect, vector<int>& stack) {
    auto r = rects[irect];
    int x = r.xyd[0]/8/64, y = r.xyd[0]/8%64, d = r.xyd[0]%8;
    for(int32_t i = 0; i < (int32_t)(4); ++i) rectof[r.xyd[i]] = -1;
    del_rect(irect);
    score -= (x-halfn)*(x-halfn) + (y-halfn)*(y-halfn) + 1;
    int d1 = (d+1)%8;
    int d2 = (d+7)%8;
    int x1 = r.xyd[1]/8/64, y1 = r.xyd[1]/8%64;
    int x3 = r.xyd[2]/8/64, y3 = r.xyd[2]/8%64;
    int x2 = r.xyd[3]/8/64, y2 = r.xyd[3]/8%64;
    uint64_t dist1 = max(abs(x1-x),abs(y1-y));
    uint64_t dist2 = max(abs(x2-x),abs(y2-y));
    auto plop = [&](int x, int y, int d, uint64_t dist) {
      while(1) {
        auto distt = fdist(x,y,d);
        if(distt == dist) break;
        int x2=x+distt*dx[d],y2=y+distt*dy[d];
        unset_rect_point_fast(x2,y2,stack);
      }
    };
    plop(x,y,d1,dist1);
    plop(x,y,d2,dist2);
    plop(x3,y3,d1^4,dist1);
    plop(x3,y3,d2^4,dist2);
    for(int32_t d = 0; d < (int32_t)(8); ++d) if(rectof[(x*64+y)*8+d] != -1) {
      unset_rect_fast(rectof[(x*64+y)*8+d], stack);
    }
    stack.emplace_back(r.xyd[0]);
    board0[x] ^= (1ull<<y);
    board1[x+(n-1-y)] ^= (1ull<<x);
    board2[y] ^= (1ull<<x);
    board3[x+y] ^= (1ull<<x);
    board4[x] ^= (1ull<<(n-1-y));
    board5[x+(n-1-y)] ^= (1ull<<(n-1-x));
    board6[y] ^= (1ull<<(n-1-x));
    board7[x+y] ^= (1ull<<(n-1-x));
    if(d == 0) {
      rem_rect13(x,y,x1,y1,x2,y2,x3,y3);
    }else if(d == 1) {
      rem_rect02(x,y,x2,y2,x1,y1,x3,y3);
    }else if(d == 2) {
      rem_rect13(x1,y1,x3,y3,x,y,x2,y2);
    }else if(d == 3) {
      rem_rect02(x1,y1,x,y,x3,y3,x2,y2);
    }else if(d == 4) {
      rem_rect13(x3,y3,x2,y2,x1,y1,x,y);
    }else if(d == 5) {
      rem_rect02(x3,y3,x1,y1,x2,y2,x,y);
    }else if(d == 6) {
      rem_rect13(x2,y2,x,y,x3,y3,x1,y1);
    }else if(d == 7) {
      rem_rect02(x2,y2,x3,y3,x,y,x1,y1);
    }else{
      do { throw runtime_error("main.cpp" ":" "970" " impossible"); } while(0);
    }
  }
  __attribute__((always_inline)) inline
  bool can(int x, int y, int d) {
    if(d == 0) return !(segments0[x]&(1ull<<(y)));
    else if(d == 1) return !(segments1[x+(n-1-y)]&(1ull<<(x)));
    else if(d == 2) return !(segments2[y]&(1ull<<(x)));
    else if(d == 3) return !(segments3[x+y]&(1ull<<(x)));
    else if(d == 4) return !(segments4[x]&(1ull<<(n-1-y)));
    else if(d == 5) return !(segments5[x+(n-1-y)]&(1ull<<(n-1-x)));
    else if(d == 6) return !(segments6[y]&(1ull<<(n-1-x)));
    else if(d == 7) return !(segments7[x+y]&(1ull<<(n-1-x)));
    do { throw runtime_error("main.cpp" ":" "986" " impossible"); } while(0);
  }
  __attribute__((always_inline)) inline
  uint64_t has(int x, int y, int d) {
    if(d == 0) return (segments0[x]&(1ull<<(y))) ? 0 : (board0[x]>>(y+1));
    else if(d == 1) return (segments1[x+(n-1-y)]&(1ull<<(x))) ? 0 : (board1[x+(n-1-y)]>>(x+1));
    else if(d == 2) return (segments2[y]&(1ull<<(x))) ? 0 : (board2[y]>>(x+1));
    else if(d == 3) return (segments3[x+y]&(1ull<<(x))) ? 0 : (board3[x+y]>>(x+1));
    else if(d == 4) return (segments4[x]&(1ull<<(n-1-y))) ? 0 : (board4[x]>>((n-1-y)+1));
    else if(d == 5) return (segments5[x+(n-1-y)]&(1ull<<(n-1-x))) ? 0 : (board5[x+(n-1-y)]>>((n-1-x)+1));
    else if(d == 6) return (segments6[y]&(1ull<<(n-1-x))) ? 0 : (board6[y]>>((n-1-x)+1));
    else if(d == 7) return (segments7[x+y]&(1ull<<(n-1-x))) ? 0 : (board7[x+y]>>((n-1-x)+1));
    do { throw runtime_error("main.cpp" ":" "999" " impossible"); } while(0);
  }
  __attribute__((always_inline)) inline
  uint64_t fhas(int x, int y, int d) {
    if(d == 0) return (board0[x]>>(y+1));
    else if(d == 1) return (board1[x+(n-1-y)]>>(x+1));
    else if(d == 2) return (board2[y]>>(x+1));
    else if(d == 3) return (board3[x+y]>>(x+1));
    else if(d == 4) return (board4[x]>>((n-1-y)+1));
    else if(d == 5) return (board5[x+(n-1-y)]>>((n-1-x)+1));
    else if(d == 6) return (board6[y]>>((n-1-x)+1));
    else if(d == 7) return (board7[x+y]>>((n-1-x)+1));
    do { throw runtime_error("main.cpp" ":" "1012" " impossible"); } while(0);
  }
  __attribute__((always_inline)) inline
  uint64_t fdist(int x, int y, int d) {
    auto di = fhas(x,y,d);
    return di?1+__builtin_ctzll(di):64;
  }
  __attribute__((always_inline)) inline
  uint64_t dist(int x, int y, int d) {
    auto di = has(x,y,d);
    return di?1+__builtin_ctzll(di):64;
  }
  void update_queue(){
    for(int32_t x = 0; x < (int32_t)(n); ++x) for(int32_t y = 0; y < (int32_t)(n); ++y) if(!(board0[x]&(1ull<<y))) {
      for(int32_t d = 0; d < (int32_t)(8); ++d) {
        int d1 = (d+1)%8;
        int d2 = (d+7)%8;
        uint64_t dist1 = has(x,y,d1);
        if(!dist1) continue;
        dist1 = 1+__builtin_ctzll(dist1);
        uint64_t dist2 = has(x,y,d2);
        if(!dist2) continue;
        dist2 = 1+__builtin_ctzll(dist2);
        int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
        int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
        if(!can(x1,y1,d2) || dist(x1,y1,d2) != dist2) continue;
        if(!can(x2,y2,d1) || dist(x2,y2,d1) != dist1) continue;
        try_enqueue(x,y,d,dist1,dist2,dist1+dist2);
      }
    }
    clear_stack();
  }
  void check(){
    for(int32_t x = 0; x < (int32_t)(n); ++x) for(int32_t y = 0; y < (int32_t)(n); ++y) if(!(board0[x]&(1ull<<y))) {
      for(int32_t d = 0; d < (int32_t)(8); ++d) {
        bool ok = 1;
        int d1 = (d+1)%8;
        int d2 = (d+7)%8;
        uint64_t dist1 = has(x,y,d1);
        uint64_t dist2 = has(x,y,d2);
        if(!dist1) {ok=0;goto l_end;}
        dist1 = 1+__builtin_ctzll(dist1);
        if(!dist2) {ok=0;goto l_end;}
        dist2 = 1+__builtin_ctzll(dist2);
        { int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
          int x2=x+dist2*dx[d2],y2=y+dist2*dy[d2];
          if(!can(x1,y1,d2) || dist(x1,y1,d2) != dist2) {ok=0;goto l_end;}
          if(!can(x2,y2,d1) || dist(x2,y2,d1) != dist1) {ok=0;goto l_end;}
        }
      l_end:;
        if(ok && can_enqueue(x,y,d,dist1,dist2)) {
          if(atqueue[(x*64+y)*8+d] == -1) {
            do { cerr << "main.cpp" ":" "1069" "  {" << "x,y,d,dist1,dist2" << "} = {"; debug_impl_seq(x,y,d,dist1,dist2); cerr << endl << flush; } while(0);
            print();
          }
          do { if(__builtin_expect(!(atqueue[(x*64+y)*8+d] != -1), 0)) { throw runtime_error("main.cpp" ":" "1072" " Assertion failed: " "atqueue[(x*64+y)*8+d] != -1"); } } while(0);
        }else{
          if(atqueue[(x*64+y)*8+d] != -1) {
            do { cerr << "main.cpp" ":" "1075" "  {" << "x,y,d" << "} = {"; debug_impl_seq(x,y,d); cerr << endl << flush; } while(0);
            print();
          }
          do { if(__builtin_expect(!(atqueue[(x*64+y)*8+d] == -1), 0)) { throw runtime_error("main.cpp" ":" "1078" " Assertion failed: " "atqueue[(x*64+y)*8+d] == -1"); } } while(0);
        }
      }
    }
  }
  void print(){
    cerr << "     ";
    for(int32_t j = 0; j < (int32_t)(n); ++j) cerr << setw(2) << j << " ";
    cerr << "\n";
    cerr << "   +-";
    for(int32_t j = 0; j < (int32_t)(n); ++j) cerr << "---";
    cerr << "+" << "\n";
    for(int32_t i = 0; i < (int32_t)(n); ++i) {
      cerr << setw(2) << i << " | ";
      for(int32_t j = 0; j < (int32_t)(n); ++j) cerr << " " << ".#"[(board0[i]>>j)&1] << " ";
      cerr << "|" << "\n";
    }
    cerr << "   +-";
    for(int32_t j = 0; j < (int32_t)(n); ++j) cerr << "---";
    cerr << "+" << endl;
  }
};
void read(){
  cin>>n;
  nn=n*n;
  halfn = (n-1)/2;
  cerr << "[DATA] n = " << n << endl;
  cin>>m; pts.resize(m);
  cerr << "[DATA] m = " << m << endl;
  for(int32_t i = 0; i < (int32_t)(m); ++i) pts[i].read();
}
state S;
state best_S;
state empty_S;
void solve(){
  S.reset();
  for(auto p : pts) S.set_point(p.x,p.y);
  S.clear_stack();
  best_S.reset(); best_S.copy_from(S);
  empty_S.reset(); empty_S.copy_from(S);
  vector<int> tmp;
  auto complete0 = [&](vector<int>& stack_add){
    while(S.nqueue) {
      int i = rng.random32(S.nqueue);
      int xyd = S.queue[i];
      stack_add.emplace_back(xyd);
      S.set_rect(xyd/8/64, xyd/8%64, xyd%8);
    }
  };
  auto complete1 = [&](vector<int>& stack_add){
    while(S.nqueue) {
      tmp.clear();
      int bcost = 1ll<<30;
      for(int32_t i = 0; i < (int32_t)(S.nqueue); ++i) {
        int cost = S.queue_cost[S.queue[i]];
        if(cost < bcost) {tmp.clear(); bcost = cost;}
        if(cost == bcost) tmp.emplace_back(i);
      }
      int i = rng.sample(tmp);
      int xyd = S.queue[i];
      stack_add.emplace_back(xyd);
      S.set_rect(xyd/8/64, xyd/8%64, xyd%8);
    }
  };
  while(TM.elapsed() < 0.1) {
    S.copy_from(empty_S);
    S.parity0 = rng.random32(2);
    S.parity1 = rng.random32(2);
    vector<int> stack; complete1(stack);
    S.clear_stack();
    if(S.score > best_S.score) {
      best_S.copy_from(S);
    }
  }
  S.copy_from(best_S);
  auto t0 = TM.elapsed();
  sa_t SA(sa_temp_mult * nn, sa_alpha);
  int64_t niter = 0;
  int naccept = 0;
  vector<int> stack_del;
  vector<int> stack_add;
  vector<int> discard;
  while(1){
    niter += 1;
    if(niter % 512 == 0) {
      float done = (TM.elapsed()-t0)/(TL-t0);
      SA.done = done;
      if(TM.elapsed() > TL) break;
      int b_size = max(0.0, 1.6*done * ((n+7)/4));
      if(S.border_size < b_size) {
        S.copy_from(best_S);
        S.border_size = b_size;
        S.update_queue();
        complete1(discard); discard.clear();
        S.clear_stack();
        best_S.copy_from(S);
      }
      if(S.restrict_odd && TM.elapsed() > TL * when_allow) {
        S.copy_from(best_S);
        S.restrict_odd = 0;
        S.update_queue();
        complete1(discard); discard.clear();
        S.clear_stack();
        best_S.copy_from(S);
      }
    }
    int old_score = S.score;
    if(rng.random32(10) == 0) {
      S.unset_rect(S.qrect[rng.random32(S.nrect)], stack_del);
    }else{
      do {
        int x = rng.random32(n), y = rng.random32(n);
        if(S.board0[x]&(1ull<<y)) continue;
        int d1 = rng.random32(8);
        uint64_t dist1 = S.has(x,y,d1);
        if(!dist1) continue;
        dist1 = 1+__builtin_ctzll(dist1);
        int x1=x+dist1*dx[d1],y1=y+dist1*dy[d1];
        S.unset_rect_point(x1,y1,stack_del);
      } while(stack_del.empty());
    }
    while(S.nrect && rng.random32(10) == 0) {
      S.unset_rect(S.qrect[rng.random32(S.nrect)], stack_del);
    }
    if(rng.random32(5) == 0) {
      for(int32_t i = ((int)stack_del.size()-2); i >= (int32_t)(0); --i) {
        int xyd = stack_del[i];
        if(S.atqueue[xyd] != -1) {
          stack_add.emplace_back(xyd);
          S.set_rect(xyd/8/64, xyd/8%64, xyd%8);
        }
      }
    }
    if(rng.random32(10) < 5) complete0(stack_add);
    else complete1(stack_add);
    if(!SA.accept(old_score-S.score)) {
      reverse(begin(stack_add), end(stack_add));
      for(int xyd : stack_add) {
        S.unset_rect_point_fast(xyd/8/64, xyd/8%64, discard);
      }
      while(!stack_del.empty()) {
        int xyd = stack_del.back();
        stack_del.pop_back();
        S.set_rect_fast(xyd/8/64, xyd/8%64, xyd%8);
      }
      discard.clear();
      S.undo_stack();
    }else{
      S.clear_stack();
      naccept += 1;
    }
    stack_del.clear();
    stack_add.clear();
    if(S.score > best_S.score) {
      best_S.copy_from(S);
    }
  }
  do { cerr << "main.cpp" ":" "1259" "  {" << "best_S.score" << "} = {"; debug_impl_seq(best_S.score); cerr << endl << flush; } while(0);
  int base_score = 0;
  for(int32_t i = 0; i < (int32_t)(n); ++i) for(int32_t j = 0; j < (int32_t)(n); ++j) base_score += (halfn-i)*(halfn-i)+(halfn-j)*(halfn-j)+1;
  int score = round(1e6 * nn / m * best_S.score / base_score);
  do { cerr << "main.cpp" ":" "1263" "  {" << "score" << "} = {"; debug_impl_seq(score); cerr << endl << flush; } while(0);
  best_S.print_solution();
  do { cerr << "main.cpp" ":" "1265" "  {" << "niter, naccept" << "} = {"; debug_impl_seq(niter, naccept); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "1266" "  {" << "TM.elapsed()" << "} = {"; debug_impl_seq(TM.elapsed()); cerr << endl << flush; } while(0);
}
int main(){
  TM.reset();
  read();
  solve();
  return 0;
}
