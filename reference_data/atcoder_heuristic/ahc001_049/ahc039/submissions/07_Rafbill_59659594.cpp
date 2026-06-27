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

const i64 MAX_PERIMETER = 400'000;
const f64 TL = 1.95;
u32 bit(u32 i){ return 1<<i; }
timer TIMER;
const i64 SZ = 100'000;
struct pt {
  i64 x, y;
  pt() { x = y = 0; }
  pt(i64 x_, i64 y_) { x = x_; y = y_; }
  pt& operator+=(pt const& o) {
    x += o.x; y += o.y;
    return *this;
  }
  pt operator+(pt const& o) const {
    pt r = *this; r += o; return r;
  }
  pt& operator-=(pt const& o) {
    x -= o.x; y -= o.y;
    return *this;
  }
  pt operator-(pt const& o) const {
    pt r = *this; r -= o; return r;
  }
  bool valid() const {
    return 0 <= x && x <= SZ && 0 <= y && y <= SZ;
  }
  auto operator<=>(pt const& o) const = default;
};
ostream& operator<<(ostream& os, pt const& o) {
  return os << make_tuple(o.x,o.y);
}
i64 nx, ny;
struct rect {
  pt fr, to;
  auto operator<=>(rect const& o) const = default;
  bool intersects(rect const& o) const {
    return fr.x < o.to.x+1 && o.fr.x < to.x+1 &&
      fr.y < o.to.y+1 && o.fr.y < to.y+1;
  }
  void modify() {
    fr.x += rng.random32(3)-1;
    fr.y += rng.random32(3)-1;
    to.x += rng.random32(3)-1;
    to.y += rng.random32(3)-1;
    if(fr.x < 0) fr.x = 0;
    if(fr.y < 0) fr.y = 0;
    if(to.x >= nx) to.x = nx-1;
    if(to.y >= ny) to.y = ny-1;
    if(to.x < fr.x) to.x = fr.x;
    if(to.y < fr.y) to.y = fr.y;
  }
};
ostream& operator<<(ostream& os, rect const& o) {
  return os << make_tuple(o.fr,o.to);
}
const i64 N = 5000;
pt A[N], B[N];
i64 X[N+N];
i64 Y[N+N];
void read(){
  { i64 n; cin>>n; do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "83" " Assertion failed: " "n == N"); } } while(0); }
  for(i32 i = 0; i < (i32)(N); ++i) { cin>>A[i].x>>A[i].y; }
  for(i32 i = 0; i < (i32)(N); ++i) { cin>>B[i].x>>B[i].y; }
}
i16 queries[2*N+2][1<<15];
struct seg_tree {
  struct node {
    i64 min_elem = 0, max_elem = 0;
    i64 delta = 0;
    __attribute__((always_inline)) inline
    void add(i64 x) {
      min_elem += x;
      max_elem += x;
      delta += x;
    }
    __attribute__((always_inline)) inline
    void update(node const& a, node const& b) {
      min_elem = min(a.min_elem, b.min_elem);
      max_elem = max(a.max_elem, b.max_elem);
    }
  };
  node A[1<<15];
  __attribute__((always_inline)) inline
  void push(i64 i) {
    if(i < (1<<14) && A[i].delta) {
      A[2*i].add(A[i].delta);
      A[2*i+1].add(A[i].delta);
      A[i].delta = 0;
    }
  }
  template<class F>
  void add_(i64 i, i64 a, i64 b, i64 x, i64 y, F&& f) {
    push(i);
    if(x <= a && b <= y) {
      do { if(!(A[i].min_elem >= 0)) { throw runtime_error("main.cpp" ":" "125" " Assertion failed: " "A[i].min_elem >= 0"); } } while(0);
      if(A[i].min_elem > 0) {
        A[i].add(1);
        return;
      }
      if(A[i].max_elem == 0) {
        A[i].add(1);
        f(i,a,b);
        return;
      }
    }
    if(b < x || y < a) return;
    auto c = (a+b)/2;
    add_(2*i, a,c, x,y, f);
    add_(2*i+1, c+1,b, x,y, f);
    A[i].update(A[2*i], A[2*i+1]);
  }
  template<class F>
  void add(i64 x, i64 y, F&& f) {
    add_(1, 0, bit(14)-1, x, y, f);
  }
  template<class F>
  void rem_(i64 i, i64 a, i64 b, i64 x, i64 y, F&& f) {
    push(i);
    if(x <= a && b <= y) {
      do { if(!(A[i].min_elem >= 1)) { throw runtime_error("main.cpp" ":" "152" " Assertion failed: " "A[i].min_elem >= 1"); } } while(0);
      if(A[i].min_elem > 1) {
        A[i].add(-1);
        return;
      }
      if(A[i].max_elem == 1) {
        A[i].add(-1);
        f(i,a,b);
        return;
      }
    }
    if(x > b || y < a) return;
    auto c = (a+b)/2;
    rem_(2*i, a,c, x,y, f);
    rem_(2*i+1, c+1,b, x,y, f);
    A[i].update(A[2*i], A[2*i+1]);
  }
  template<class F>
  void rem(i64 x, i64 y, F&& f) {
    rem_(1, 0, bit(14)-1, x, y, f);
  }
};
template<class F>
void iter_(i32 i, i32 a, i32 b, i32 x, i32 y, F&& f) {
  if(x <= a && b <= y) {
    f(i,a,b);
      return;
  }
  if(x > b || y < a) return;
  auto c = (a+b)/2;
  iter_(2*i, a,c, x,y, f);
  iter_(2*i+1, c+1,b, x,y, f);
}
template<class F>
void iter(i32 x, i32 y, F&& f) {
  iter_(1,0,bit(14)-1, x,y,f);
}
void init() {
  set<i64> XS, YS;
  for(i32 i = 0; i < (i32)(N); ++i) { XS.insert(A[i].x); YS.insert(A[i].y); }
  for(i32 i = 0; i < (i32)(N); ++i) { XS.insert(B[i].x); YS.insert(B[i].y); }
  XS.insert(0); XS.insert(100'000);
  YS.insert(0); YS.insert(100'000);
  vector<i64> AXS(begin(XS), end(XS)), AYS(begin(YS), end(YS));
  nx = AXS.size(); ny = AYS.size();
  map<i64, i64> MXS, MYS;
  for(i32 i = 0; i < (i32)(nx); ++i) X[i] = AXS[i];
  for(i32 i = 0; i < (i32)(ny); ++i) Y[i] = AYS[i];
  for(i32 i = 0; i < (i32)(nx); ++i) MXS[AXS[i]] = i;
  for(i32 i = 0; i < (i32)(ny); ++i) MYS[AYS[i]] = i;
  for(i32 i = 0; i < (i32)(N); ++i) { A[i].x = MXS[A[i].x]; A[i].y = MYS[A[i].y]; }
  for(i32 i = 0; i < (i32)(N); ++i) { B[i].x = MXS[B[i].x]; B[i].y = MYS[B[i].y]; }
  for(i32 i = 0; i < (i32)(N); ++i) {
    queries[A[i].x+1][(1<<14) + (A[i].y+1)] += 1;
    queries[B[i].x+1][(1<<14) + (B[i].y+1)] -= 1;
  }
  for(i32 i = 0; i < (i32)(nx); ++i) {
    for(i32 j = 0; j < (i32)(ny); ++j) {
      queries[i+1][(1<<14)+j] += queries[i][(1<<14)+j];
    }
    for(i32 j = ((1<<14)-1); j >= (i32)(1); --j) queries[i][j] = queries[i][2*j] + queries[i][2*j+1];
  }
  do { cerr << "main.cpp" ":" "219" "  {" << "TIMER.elapsed()" << "} = {"; debug_impl_seq(TIMER.elapsed()); cerr << endl << flush; } while(0);
}
struct union_find {
  vector<i64> A;
  union_find(i64 n = 0) : A(n) {
    iota(begin(A), end(A), 0);
  }
  i64 addNode() {
    A.push_back(A.size());
    return A.size()-1;
  }
  i64 find(i64 a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }
  void unite(i64 a, i64 b) {
    a = find(a); b = find(b);
    A[a] = b;
  }
};
i32 last[1<<14][1<<14];
i32 date = 0;
struct state {
  vector<rect> rectangles;
  f64 eval(i32 rectangle_limit) const {
    static seg_tree ST;
    if(rectangles.size() > rectangle_limit) return -100'000'000;
    union_find uf(rectangles.size());
    for(i32 i = 0; i < (i32)(rectangles.size()); ++i) for(i32 j = 0; j < (i32)(i); ++j) {
      if(rectangles[i].intersects(rectangles[j])) {
        uf.unite(i,j);
      }
    }
    i64 nsz = 0;
    for(i32 i = 0; i < (i32)(rectangles.size()); ++i) if(uf.find(i) == i) nsz += 1;
    if(nsz > 1) return -100'000'000;
    i64 value = 0, perimeter = 0;
    date += 4;
    bool ok_pt = 1;
    auto add_pt = [&](i32 x, i32 y){
      if(last[x][y] < date) last[x][y] = date;
      last[x][y] += 1;
      if(last[x][y] - date > 2) ok_pt = 0;
    };
    {
      vector<array<i64, 4>> events;
      for(auto const& r : rectangles) {
        events.push_back({r.fr.x,0,r.fr.y,r.to.y});
        events.push_back({r.to.x,1,r.fr.y,r.to.y});
      }
      sort(begin(events), end(events));
      for(auto e : events) {
        i64 x = e[0];
        i64 l = e[2], r = e[3];
        if(e[1] == 0) {
          ST.add(l,r,[&](i32 i, i64 a, i64 b){
            value -= queries[x][i];
            perimeter += Y[b+1]-Y[a];
            add_pt(x,a);
            add_pt(x,b+1);
          });
        }else{
          ST.rem(l,r,[&](i32 i, i64 a, i64 b){
            value += queries[x+1][i];
            perimeter += Y[b+1]-Y[a];
            add_pt(x+1,a);
            add_pt(x+1,b+1);
          });
        }
      }
    }
    if(!ok_pt) return -100'000'000;
    {
      vector<array<i64, 4>> events;
      for(auto const& r : rectangles) {
        events.push_back({r.fr.y,0,r.fr.x,r.to.x});
        events.push_back({r.to.y,1,r.fr.x,r.to.x});
      }
      sort(begin(events), end(events));
      for(auto e : events) {
        auto y = e[0];
        i64 l = e[2], r = e[3];
        if(e[1] == 0) {
          ST.add(l,r,[&](i32, i64 a, i64 b){
            perimeter += X[b+1]-X[a];
            add_pt(a,y);
            add_pt(b+1,y);
          });
        }else{
          ST.rem(l,r,[&](i32, i64 a, i64 b){
            perimeter += X[b+1]-X[a];
            add_pt(a,y+1);
            add_pt(b+1,y+1);
          });
        }
      }
    }
    if(!ok_pt) return -100'000'000;
    if(perimeter > MAX_PERIMETER) {
      return -100'000'000;
    }
    return value + (MAX_PERIMETER - perimeter) * 1e-6;
  }
  void construct() const {
    static seg_tree ST;
    vector<tuple<pt,pt>> edges;
    {
      vector<array<i64, 4>> events;
      for(auto const& r : rectangles) {
        events.push_back({r.fr.x,0,r.fr.y,r.to.y});
        events.push_back({r.to.x,1,r.fr.y,r.to.y});
      }
      sort(begin(events), end(events));
      for(auto e : events) {
        i64 x = e[0];
        i64 l = e[2], r = e[3];
        if(e[1] == 0) {
          ST.add(l,r,[&](i32, i64 a, i64 b){
            edges.push_back(make_tuple(pt(X[x],Y[a]),pt(X[x],Y[b+1])));
          });
        }else{
          ST.rem(l,r,[&](i32, i64 a, i64 b){
            edges.push_back(make_tuple(pt(X[x+1],Y[a]),pt(X[x+1],Y[b+1])));
          });
        }
      }
    }
    {
      vector<array<i64, 4>> events;
      for(auto const& r : rectangles) {
        events.push_back({r.fr.y,0,r.fr.x,r.to.x});
        events.push_back({r.to.y,1,r.fr.x,r.to.x});
      }
      sort(begin(events), end(events));
      for(auto e : events) {
        auto y = e[0];
        i64 l = e[2], r = e[3];
        if(e[1] == 0) {
          ST.add(l,r,[&](i32, i64 a, i64 b){
            edges.push_back(make_tuple(pt(X[a],Y[y]),pt(X[b+1],Y[y])));
          });
        }else{
          ST.rem(l,r,[&](i32, i64 a, i64 b){
            edges.push_back(make_tuple(pt(X[a],Y[y+1]),pt(X[b+1],Y[y+1])));
          });
        }
      }
    }
    map<pt, vector<pt>> G;
    for(auto [p,q] : edges) {
      G[p].push_back(q);
      G[q].push_back(p);
    }
    for(auto const& p : G) {
      do { if(!(p.second.size() == 2)) { throw runtime_error("main.cpp" ":" "401" " Assertion failed: " "p.second.size() == 2"); } } while(0);
    }
    auto p0 = begin(G)->first;
    auto p = p0;
    auto last = p;
    vector<pt> cycle;
    do {
      cycle.push_back(p);
      for(auto q : G[p]) if(q != last) {
          last = q;
          break;
        }
      swap(p, last);
    } while(p != p0);
    i64 ncycle = cycle.size();
    vector<pt> cycle2;
    for(i32 i = 0; i < (i32)(ncycle); ++i) {
      bool ok = 1;
      if(i > 0 && i+1 < ncycle &&
         ((cycle[i-1].x == cycle[i].x && cycle[i].x == cycle[i+1].x) ||
          (cycle[i-1].y == cycle[i].y && cycle[i].y == cycle[i+1].y)))
        {
           ok = 0;
        }
      if(ok) cycle2.push_back(cycle[i]);
    }
    cout << cycle2.size() << endl;
    for(auto p : cycle2) {
      cout << p.x << ' ' << p.y << '\n';
    }
    cout << flush;
  }
};
i32 eval_rect(rect const& r) {
  i32 ans = 0;
  iter(r.fr.y, r.to.y, [&](i32 i, i32 a, i32 b) {
    ans += queries[r.to.x+1][i];
    ans -= queries[r.fr.x][i];
  });
  return ans;
}
void solve() {
  state S;
  f64 temp0 = 300.0;
  f64 temp1 = 0.1;
  f64 temp = temp0;
  f64 done = 0.0;
  i64 niter = 0;
  f64 score = 0;
  f64 best_score = score;
  auto best_S = S;
  i32 rectangle_limit = 4;
  while(1) {
    niter += 1;
    if(niter % (1<<4) == 0) {
      done = TIMER.elapsed() / TL;
      temp = temp0 * pow(temp1/temp0, done);
      rectangle_limit = 4.0 * pow(50.0 / 4.0, done);
      if(done > 1.0) break;
    }
    auto accept = [&](f64 new_score) __attribute__((always_inline)) -> bool {
      auto delta = score - new_score;
      return delta <= 0 || delta <= temp * rng.randomDouble();
    };
    i64 ty = rng.random32(3);
    if(ty <= 0) {
      i64 nrem = rng.random32(2);
      i64 nadd = rng.random32(3);
      nrem = min<i64>(S.rectangles.size(), nrem);
      if(nadd == 0) continue;
      auto T = S;
      for(i32 i = 0; i < (i32)(nrem); ++i) {
        i64 j = rng.random32(T.rectangles.size());
        T.rectangles.erase(begin(T.rectangles)+j);
      }
      for(i32 i = 0; i < (i32)(nadd); ++i) {
        i64 x1 = rng.random32(nx-1), x2 = rng.random32(nx-1);
        if(x1 > x2) swap(x1, x2);
        i64 y1 = rng.random32(ny-1), y2 = rng.random32(ny-1);
        if(y1 > y2) swap(y1, y2);
        if(x1 == x2 || y1 == y2) continue;
        auto r = rect { pt(x1,y1), pt(x2, y2) };
        if(eval_rect(r) > -100) T.rectangles.push_back(r);
      }
      auto new_score = T.eval(rectangle_limit);
      if(accept(new_score)) {
        if(new_score > best_score) {
          best_score = new_score;
          best_S = T;
        }
        S = T;
        score = new_score;
      }
    }else if(ty == 1) {
      if(S.rectangles.empty()) continue;
      i32 i = rng.random32(S.rectangles.size());
      auto T = S;
      auto r = T.rectangles[i];
      i32 ty2 = rng.random32(4);
      if(ty2 == 0) {
        r.fr.x = rng.random32(r.to.x);
      }else if(ty2 == 1) {
        r.to.x = r.fr.x+1+rng.random32(nx-2-r.fr.x);
      }else if(ty2 == 2) {
        r.fr.y = rng.random32(r.to.y);
      }else if(ty2 == 3) {
        r.to.y = r.fr.y+1+rng.random32(ny-2-r.fr.y);
      }
      T.rectangles[i] = r;
      auto new_score = T.eval(rectangle_limit);
      if(accept(new_score)) {
        if(new_score > best_score) {
          best_score = new_score;
          best_S = T;
        }
        S = T;
        score = new_score;
      }
    }else{
      if(S.rectangles.empty()) continue;
      auto T = S;
      i32 to_mod = 1;
      i32 to_add = rng.random32(3);
      for(i32 k = 0; k < (i32)(to_mod); ++k) {
        i32 i = rng.random32(S.rectangles.size());
        auto r = T.rectangles[i];
        i32 ty2 = rng.random32(4);
        if(ty2 == 0) {
          r.fr.x = rng.random32(r.to.x);
        }else if(ty2 == 1) {
          r.to.x = r.fr.x+1+rng.random32(nx-2-r.fr.x);
        }else if(ty2 == 2) {
          r.fr.y = rng.random32(r.to.y);
        }else if(ty2 == 3) {
          r.to.y = r.fr.y+1+rng.random32(ny-2-r.fr.y);
        }
        T.rectangles[i] = r;
      }
      for(i32 i = 0; i < (i32)(to_add); ++i) {
        i64 x1 = rng.random32(nx-1), x2 = rng.random32(nx-1);
        if(x1 > x2) swap(x1, x2);
        i64 y1 = rng.random32(ny-1), y2 = rng.random32(ny-1);
        if(y1 > y2) swap(y1, y2);
        if(x1 == x2 || y1 == y2) continue;
        auto r = rect { pt(x1,y1), pt(x2, y2) };
        if(eval_rect(r) > -100) T.rectangles.push_back(r);
      }
      auto new_score = T.eval(rectangle_limit);
      if(accept(new_score)) {
        if(new_score > best_score) {
          best_score = new_score;
          best_S = T;
        }
        S = T;
        score = new_score;
      }
    }
  }
  best_S.construct();
  do { cerr << "main.cpp" ":" "584" "  {" << "best_score" << "} = {"; debug_impl_seq(best_score); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "585" "  {" << "niter" << "} = {"; debug_impl_seq(niter); cerr << endl << flush; } while(0);
}
int main(){
  TIMER.reset();
  read();
  init();
  solve();
  cerr << "[DATA] time=" << (int)(TIMER.elapsed() * 1000) << endl;
  return 0;
}
