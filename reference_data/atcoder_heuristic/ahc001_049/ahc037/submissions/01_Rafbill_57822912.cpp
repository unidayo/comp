#pragma GCC optimize "-O3,omit-frame-pointer,inline,unroll-all-loops,fast-math"
 #pragma GCC target "tune=native"
 #include <bits/stdc++.h>
 #include <sys/time.h>
 #include <immintrin.h>
 #include <x86intrin.h>
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

const f64 TL = 1.99;
timer TIMER;
const i64 N = 1000;
const i64 NUM_NODES = N + N-1;
struct pt {
  i32 x, y;
  bool operator==(pt const& o) const = default;
};
i64 combine_cost(pt const& a, pt const& b) {
  return abs(a.x-b.x) + abs(a.y-b.y);
}
pt combine(pt const& a, pt const& b) {
  return pt { min(a.x,b.x), min(a.y, b.y) };
}
pt input[N];
void read(){
  { i64 n; cin>>n; do { if(!(n == N)) { throw runtime_error("main.cpp" ":" "26" " Assertion failed: " "n == N"); } } while(0); }
  for(i32 i = 0; i < (i32)(N); ++i) {
    cin>>input[i].x>>input[i].y;
  }
}
struct node {
  bool is_leaf;
  pt point;
  array<i32, 2> children;
  i32 parent;
  i64 cost;
  __attribute__((always_inline)) inline i64 other(i64 x) const{
    for(i32 j = 0; j < (i32)(2); ++j) if(children[j] != x) return children[j];
  }
};
struct state {
  node nodes[NUM_NODES];
  i64 cost;
  void greedy_init() {
    cost = 0;
    i32 next_node = 0;
    vector<i32> I;
    for(i32 i = 0; i < (i32)(N); ++i) {
      nodes[next_node] = node {
        .is_leaf = true,
        .point = input[i],
        .children = {-1,-1},
        .parent = -1,
        .cost = 0,
      };
      I.emplace_back(next_node);
      next_node += 1;
    }
    while(I.size() >= 2) {
      i32 bi = -1, bj = -1; i64 bc = 1e18;
      for(i32 i0 = 0; i0 < (i32)(I.size()); ++i0) for(i32 j0 = 0; j0 < (i32)(i0); ++j0) {
        i32 i = I[i0], j = I[j0];
        auto c = combine_cost(nodes[i].point, nodes[j].point);
        if(c < bc) {
          bi = i;
          bj = j;
          bc = c;
        }
      }
      nodes[bi].parent = next_node;
      nodes[bj].parent = next_node;
      cost += combine_cost(nodes[bi].point, nodes[bj].point);
      nodes[next_node] = node {
        .is_leaf = false,
        .point = combine(nodes[bi].point, nodes[bj].point),
        .children = {bi, bj},
        .parent = -1,
        .cost = combine_cost(nodes[bi].point, nodes[bj].point),
      };
      I.emplace_back(next_node);
      I.erase(find(begin(I), end(I), bi));
      I.erase(find(begin(I), end(I), bj));
      next_node += 1;
    }
    do { if(!(next_node == NUM_NODES)) { throw runtime_error("main.cpp" ":" "94" " Assertion failed: " "next_node == NUM_NODES"); } } while(0);
    do { cerr << "main.cpp" ":" "95" "  {" << "TIMER.elapsed()" << "} = {"; debug_impl_seq(TIMER.elapsed()); cerr << endl << flush; } while(0);
  }
  vector<array<pt, 2>> reconstruct() const {
    i32 root = 0;
    for(i32 i = 0; i < (i32)(NUM_NODES); ++i) if(nodes[i].parent == -1) { root = i; break; }
    vector<array<pt, 2>> solution;
    auto dfs = [&](auto self, i32 i, pt par) -> void {
      solution.push_back({ par, nodes[i].point });
      if(!nodes[i].is_leaf) {
        for(i32 j = 0; j < (i32)(2); ++j) self(self, nodes[i].children[j], nodes[i].point);
      }
    };
    dfs(dfs, root, {0,0});
    return solution;
  }
  __attribute__((always_inline)) inline bool is_ancestor_of(i32 i, i32 j) const {
    while(i != j) {
      if(j == -1) return false;
      if(nodes[j].point.x < nodes[i].point.x ||
         nodes[j].point.y < nodes[j].point.y) return false;
      j = nodes[j].parent;
    }
    return true;
  }
  __attribute__((always_inline)) inline void cut(i32 i) {
    i32 p = nodes[i].parent;
    if(p == -1) return;
    nodes[i].parent = -1;
    for(i32 j = 0; j < (i32)(2); ++j) if(nodes[p].children[j] == i) {
      nodes[p].children[j] = -1;
      return;
    }
  }
  __attribute__((always_inline)) inline void link(i32 i, i32 p) {
    if(p != -1) {
      nodes[i].parent = p;
      for(i32 j = 0; j < (i32)(2); ++j) if(nodes[p].children[j] == -1) {
        nodes[p].children[j] = i;
        break;
      }
    }
  }
  void update(i32 p) {
    while(1) {
      if(p == -1) return;
      i32 x = nodes[p].children[0], y = nodes[p].children[1];
      if(x == -1 || y == -1) return;
      i64 new_cost = combine_cost(nodes[x].point, nodes[y].point);
      cost += new_cost - nodes[p].cost;
      nodes[p].cost = new_cost;
      auto new_point = combine(nodes[x].point, nodes[y].point);
      if(new_point == nodes[p].point) return;
      nodes[p].point = new_point;
      p = nodes[p].parent;
    }
  }
  // List of transitions:
  // - Rotation
  // - Swap two nodes
  // - Insert node as neighbor of other node
  template<class ACC>
  void transition_swap(ACC&& acc) {
    i32 x1 = rng.random32(NUM_NODES);
    if(nodes[x1].parent == -1) return;
    i32 x2 = rng.random32(NUM_NODES);
    if(nodes[x2].parent == -1) return;
    if(is_ancestor_of(x1, x2) || is_ancestor_of(x2, x1)) return;
    i32 p1 = nodes[x1].parent;
    i32 p2 = nodes[x2].parent;
    cut(x1);
    cut(x2);
    link(x1, p2);
    link(x2, p1);
    update(p1);
    update(p2);
    if(!acc()) {
      cut(x1);
      cut(x2);
      link(x1, p1);
      link(x2, p2);
      update(p1);
      update(p2);
    }
  }
  template<class ACC>
  void transition_rotate(ACC&& acc) {
    i32 x1 = rng.random32(NUM_NODES);
    if(nodes[x1].parent == -1) return;
    i32 p1 = nodes[x1].parent;
    if(nodes[p1].parent == -1) return;
    i32 q1 = nodes[p1].parent;
    auto r1 = nodes[q1].parent; // can be (-1);
    i32 x2 = nodes[p1].other(x1);
    // i32 p2 = nodes[q1].other(p1);
    // DO
    cut(x1);
    cut(x2);
    cut(p1);
    cut(q1);
    link(x1, p1);
    link(x2, q1);
    link(p1, r1);
    link(q1, p1);
    update(q1);
    update(p1);
    if(!acc()) {
      // UNDO
      cut(x1);
      cut(x2);
      cut(p1);
      cut(q1);
      link(x1, p1);
      link(x2, p1);
      link(p1, q1);
      link(q1, r1);
      update(p1);
      update(q1);
    }
  }
  template<class ACC>
  void transition_move_node(ACC&& acc) {
    i32 x1 = rng.random32(NUM_NODES);
    if(nodes[x1].parent == -1) return;
    i32 x2 = rng.random32(NUM_NODES);
    if(nodes[x2].parent == -1) return;
    i32 p1 = nodes[x1].parent;
    i32 p2 = nodes[x2].parent;
    i32 y1 = nodes[p1].other(x1);
    i32 y2 = nodes[p2].other(x2);
    if(x1 == y2) return;
    i64 delta = 0;
    delta -= combine_cost(nodes[x1].point, nodes[y1].point);
    delta -= combine_cost(nodes[x2].point, nodes[y2].point);
    delta += combine_cost(nodes[x1].point, nodes[y2].point);
    delta += combine_cost(nodes[x2].point, nodes[y1].point);
    if(delta > 2.5e8) return;
    if(is_ancestor_of(x1, x2) || is_ancestor_of(x2, x1)) {
      return;
    }
    i32 q1 = nodes[p1].parent; // possibly (-1)
    // DO
    cut(x1);
    cut(y1);
    cut(x2);
    cut(p1);
    link(x1, p1);
    link(x2, p1);
    link(p1, p2);
    link(y1, q1);
    update(p1);
    update(p2);
    update(q1);
    if(!acc()) {
      // UNDO
      cut(y1);
      cut(p1);
      cut(x2);
      cut(x1);
      link(x1, p1);
      link(y1, p1);
      link(p1, q1);
      link(x2, p2);
      update(p1);
      update(p2);
      update(q1);
    }
  }
};
void print_solution(vector<array<pt, 2>> const& sol) {
  cout << sol.size() << '\n';
  for(auto p : sol) {
    for(i32 i = 0; i < (i32)(2); ++i) cout << p[i].x << ' ' << p[i].y << ' ';
    cout << '\n';
  }
  cout << flush;
}
void combine_solutions(state const& S, state const& T){
  // each vertex: in either both or single
  // each edge: in either both (then include) or single (extend connected component)
}
void solve(){
  state S; S.greedy_init();
  do { cerr << "main.cpp" ":" "310" "  {" << "S.cost" << "} = {"; debug_impl_seq(S.cost); cerr << endl << flush; } while(0);
  state best_state = S;
  f64 temp0 = 5e8;
  f64 temp1 = 5e2;
  f64 temp = temp0;
  f64 done = 0.0;
  vector<state> snapshots;
  i64 niter = 0;
  while(1) {
    niter += 1;
    if(niter % (1<<13) == 0) {
      done = TIMER.elapsed() / TL;
      temp = temp0 * pow(temp1/temp0, done);
      if(done > 1.0) break;
    }
    if(niter % (1<<20) == 0) {
      snapshots.emplace_back(S);
    }
    auto old_cost = S.cost;
    auto accept = [&]() __attribute__((always_inline)) -> bool {
      auto delta = S.cost - old_cost;
      return delta <= 0 || delta <= temp * rng.randomDouble();
    };
    i32 ty = rng.random32(40);
    if(ty <= 35) {
      S.transition_move_node(accept);
    }else if(ty <= 39) {
      S.transition_rotate(accept);
    }
    if(done > 0.8 && S.cost < best_state.cost) best_state = S;
  }
  do { cerr << "main.cpp" ":" "349" "  {" << "niter" << "} = {"; debug_impl_seq(niter); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "350" "  {" << "S.cost" << "} = {"; debug_impl_seq(S.cost); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "352" "  {" << "snapshots.size()" << "} = {"; debug_impl_seq(snapshots.size()); cerr << endl << flush; } while(0);
  for(auto const& T : snapshots) {
    combine_solutions(best_state, T);
  }
  { auto sol = best_state.reconstruct();
    print_solution(sol);
  }
}
int main(){
  TIMER.reset();
  read();
  solve();
  return 0;
}
