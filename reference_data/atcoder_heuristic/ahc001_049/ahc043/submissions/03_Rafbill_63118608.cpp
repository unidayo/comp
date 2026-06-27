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

template<class T, int N>
struct smallvec {
  int size;
  T data[N];
  smallvec() { size = 0; }
  void clear() { size = 0; }
  void push(T const& v) { data[size++] = v; }
  void pop() { size -= 1; }
  template<class F>
  void iter(F&& f) const {
    for(i32 i = 0; i < (i32)(size); ++i) f(data[i]);
  }
};
struct union_find {
  vector<i64> A;
  union_find(i64 n = 0) : A(n) {
    iota(begin(A), end(A), 0);
  }
  i64 find(i64 a) {
    return A[a] == a ? a : A[a] = find(A[a]);
  }
  bool unite(i64 a, i64 b) {
    a = find(a);
    b = find(b);
    if(a == b) {
      return false;
    }
    A[a] = b;
    return true;
  }
};
template<class T>
struct compact_array {
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
const f32 TL_BEAM = 2.6;
const f32 TL_BEAM_STOP = 2.7;
const f32 TL_POST = 2.95;
timer TIMER;
const int N = 50;
const int NN = N*N;
const int T = 800;
const int MAX_EDGES = 1600;
int num_edges;
int initial_money;
struct edge {
  int src, tgt;
};
edge edges[MAX_EDGES];
int edge_income[MAX_EDGES];
void read() {
  int n,t; cin >> n >> num_edges >> initial_money >> t;
  do { if(!(n == N && t == T)) { throw runtime_error("main.cpp" ":" "114" " Assertion failed: " "n == N && t == T"); } } while(0);
  cerr << "[DATA] m = " << round(2*log2(1.0*num_edges/50)) << endl;
  cerr << "[DATA] k = " << round(initial_money/1000)*1000 << endl;
  for(i32 i = 0; i < (i32)(num_edges); ++i) {
    int x1, y1, x2, y2; cin >> x1 >> y1 >> x2 >> y2;
    edges[i] = { .src = x1*N+y1, .tgt = x2*N+y2 };
    edge_income[i] = abs(x1-x2) + abs(y1-y2);
  }
}
const int dx[4] = {-1,0,1,0};
const int dy[4] = {0,-1,0,1};
const int dxy[4] = {-N,-1,N,1};
u8 graph[NN];
u64 hash_station[NN];
u64 hash_destination[2*MAX_EDGES];
compact_array<int> reachable_from_station;
int ngroups;
int group_of[NN];
compact_array<int> group_stations;
compact_array<int> reachable_from_group;
compact_array<int> rem_at_group;
compact_array<int> add_at_group;
int cell_score[NN];
vector<int> destinations_stations[2*MAX_EDGES];
void init(){
  // Compute graph
  for(i32 x1 = 0; x1 < (i32)(N); ++x1) for(i32 y1 = 0; y1 < (i32)(N); ++y1) {
    graph[x1*N+y1] = 0;
    for(i32 d = 0; d < (i32)(4); ++d) {
      int x2 = x1+dx[d], y2 = y1 + dy[d];
      if(0<=x2&&x2<N && 0<=y2&&y2<N) {
        graph[x1*N+y1] |= bit(d);
      }
    }
  }
  // Compute reachability
  vector<vector<int>> reachable_from_station_(NN);
  for(i32 i = 0; i < (i32)(num_edges); ++i) {
    auto add = [&](int xy, int label) {
      for(i32 dx = (-2); dx <= (i32)(2); ++dx) for(i32 dy = (-2); dy <= (i32)(2); ++dy) if(abs(dx)+abs(dy) <= 2) {
        int x = xy / N + dx;
        int y = xy % N + dy;
        if(0<=x&&x<N && 0<=y&&y<N) {
          reachable_from_station_[x*N+y].push_back(label);
        }
      }
    };
    add(edges[i].src, 2*i);
    add(edges[i].tgt, 2*i+1);
  }
  reachable_from_station.init(reachable_from_station_);
  // Compute dominance and equivalent sets
  vector<bitset<2*MAX_EDGES>> reachable_bs(NN);
  for(i32 i = 0; i < (i32)(NN); ++i) for(int j : reachable_from_station_[i]) {
    reachable_bs[i][j] = 1;
  };
  unordered_map<bitset<2*MAX_EDGES>, vector<int>> reachable_sets;
  for(i32 i = 0; i < (i32)(NN); ++i) reachable_sets[reachable_bs[i]].push_back(i);
  vector<vector<int>> group_stations_;
  for(auto const& p : reachable_sets) {
    if(p.first.none()) continue;
    bool dominated = false;
    for(auto const& q : reachable_sets) {
      if((p.first & ~q.first).none() && (p != q)) {
        dominated = true;
        break;
      }
    }
    if(!dominated) {
      group_stations_.push_back(p.second);
    }
  }
  sort(begin(group_stations_), end(group_stations_), [&](auto const& a, auto const& b){
    auto x = a[0], y = b[0];
    auto xx = x/N, xy = x%N;
    auto yx = y/N, yy = y%N;
    if(xx&1) xy = -xy;
    if(yx&1) yy = -yy;
    return make_tuple(xx,xy) < make_tuple(yx,yy);
  });
  ngroups = group_stations_.size();
  for(i32 i = 0; i < (i32)(NN); ++i) group_of[i] = -1;
  for(i32 i = 0; i < (i32)(ngroups); ++i) for(int u : group_stations_[i]) {
    group_of[u] = i;
  }
  vector<vector<int>> reachable_from_group_;
  for(auto const& group : group_stations_) {
    reachable_from_group_.push_back(reachable_from_station_[group[0]]);
  }
  ngroups = group_stations_.size();
  group_stations.init(group_stations_);
  reachable_from_group.init(reachable_from_group_);
  do { cerr << "main.cpp" ":" "222" "  {" << "ngroups, group_stations.data.size(), reachable_from_group.data.size()" << "} = {"; debug_impl_seq(ngroups, group_stations.data.size(), reachable_from_group.data.size()); cerr << endl << flush; } while(0);
  {
    vector<vector<int>> add_at_group_;
    vector<vector<int>> rem_at_group_;
    vector<int> last;
    for(i32 i = 0; i < (i32)(ngroups); ++i) {
      vector<int> add_, rem_;
      vector<int> next;
      reachable_from_group.iter(i, [&](int u) {
        next.push_back(u);
        auto it = find(begin(last), end(last), u);
        if(it == end(last)) {
          add_.push_back(u);
        }else{
          last.erase(it);
        }
      });
      rem_ = last;
      last = next;
      rem_at_group_.push_back(rem_);
      add_at_group_.push_back(add_);
    }
    rem_at_group.init(rem_at_group_);
    add_at_group.init(add_at_group_);
    do { cerr << "main.cpp" ":" "249" "  {" << "rem_at_group.data.size(), add_at_group.data.size()" << "} = {"; debug_impl_seq(rem_at_group.data.size(), add_at_group.data.size()); cerr << endl << flush; } while(0);
  }
  // Compute hashes
  for(i32 i = 0; i < (i32)(NN); ++i) hash_station[i] = rng.randomInt64();
  for(i32 i = 0; i < (i32)(2*MAX_EDGES); ++i) hash_destination[i] = rng.randomInt64();
  for(i32 i = 0; i < (i32)(NN); ++i) {
    cell_score[i] = 0;
    if(group_of[i] != -1) {
      for(int v : reachable_from_station_[i]) {
        cell_score[i] += edge_income[v/2];
      }
    }
  }
  for(i32 i = 0; i < (i32)(NN); ++i) {
    reachable_from_station.iter(i, [&](int j) {
      destinations_stations[j].push_back(i);
    });
  }
}
const int HISTORY_SIZE = 1<<26;
struct history_list {
  int data;
  int prev;
};
history_list *history_pool = nullptr;
int history_size = 0;
void reset_history() {
  history_size = 0;
}
int make_history(int data, int prev) {
  int ix = history_size++;
  history_pool[ix] = { .data = data, .prev = prev };
  return ix;
}
template<size_t N>
struct mybitset {
  static constexpr size_t NBITS = (N+63)/64;
  u64 data[NBITS];
  void reset() { for(i32 i = 0; i < (i32)(NBITS); ++i) data[i] = 0; }
  __attribute__((always_inline)) inline
  void set(int i) { data[i/64] |= bit(i%64); }
  __attribute__((always_inline)) inline
  void unset(int i) { data[i/64] &= ~bit(i%64); }
  __attribute__((always_inline)) inline
  bool get(int i) const { return data[i/64] & bit(i%64); }
};
struct beam_state {
  mybitset<NN> built_stations;
  mybitset<NN> built_rail;
  mybitset<2*MAX_EDGES> active_destinations;
  i8 parent[NN];
  int income;
  int half_income;
  int turn;
  int money;
  int overbuilds;
  int history;
  u64 hash; // hash of active destinations
  void reset(u64 h0) {
    built_stations.reset();
    active_destinations.reset();
    income = 0;
    half_income = 0;
    turn = 0;
    money = initial_money;
    overbuilds = 0;
    history = -1;
    hash = h0;
  }
  void reset_parent() {
    for(i32 i = 0; i < (i32)(NN); ++i) parent[i] = 0;
    built_rail.reset();
  }
  void add_destination(int u) {
    active_destinations.set(u);
    hash ^= hash_destination[u];
    if(active_destinations.get(u^1)) {
      income += edge_income[u/2];
      half_income -= edge_income[u/2];
    }else{
      half_income += edge_income[u/2];
    }
  }
  void rem_destination(int u) {
    active_destinations.unset(u);
    hash ^= hash_destination[u];
    if(active_destinations.get(u^1)) {
      income -= edge_income[u/2];
      half_income += edge_income[u/2];
    }else{
      half_income -= edge_income[u/2];
    }
  }
  void init_add_station(int where) {
    built_stations.set(where);
    reachable_from_station.iter(where, [&](int u) __attribute__((always_inline)) -> void {
      if(!active_destinations.get(u)) {
        add_destination(u);
      }
    });
  }
};
const int MAX_BEAM_WIDTH = 512;
beam_state BEAM[T+1][MAX_BEAM_WIDTH];
int BEAM_COUNT[T+1];
i64 BEAM_VALUE[T+1][MAX_BEAM_WIDTH];
int BEAM_QUEUE[T+1][MAX_BEAM_WIDTH];
const int HASH_BITS = 18;
const int HASH_SIZE = 1<<HASH_BITS;
const int HASH_MASK = HASH_SIZE-1;
struct hash_entry {
  u64 hash;
  int turn;
  int money;
};
hash_entry HASH_TABLE[HASH_SIZE];
void reset_beam() {
  reset_history();
  for(i32 i = 0; i < (i32)(T+1); ++i) BEAM_COUNT[i] = 0;
}
const int money_increment = 100;
i64 compute_value(int turn, int income, int half_income, int money, int overbuilds) {
  i64 capped_income = min(income, 1000);
  if(turn > 400) capped_income = 0;
  i64 outcome = (money / money_increment) * money_increment + (T-turn) * income;
  int money_rem = money % money_increment;
  return ((capped_income * 50ll + overbuilds) * 100ll + outcome) * 100ll +
          money_rem + half_income * (T-turn);
}
i64 num_nodes = 0;
template<class F>
void beam_add_state
(int beam_width,
 int turn, int income, int half_income, int money, int overbuilds,
 u64 hash,
 F&& build_state)
{
  if(turn > T) return;
  auto &entry = HASH_TABLE[hash&HASH_MASK];
  if(entry.hash == hash &&
     (turn > entry.turn ||
      (turn == entry.turn && money <= entry.money))) {
    return;
  }
  i64 value = compute_value(turn, income, half_income, money, overbuilds);
  auto heap_cmp = [&](int u, int v) __attribute__((always_inline)) {
    return BEAM_VALUE[turn][u] > BEAM_VALUE[turn][v];
  };
  if(BEAM_COUNT[turn] < beam_width) {
    num_nodes += 1;
    int ix = BEAM_QUEUE[turn][BEAM_COUNT[turn]];
    auto& sb = BEAM[turn][ix];
    build_state(sb);
    entry = { .hash = sb.hash, .turn = sb.turn, .money = sb.money };
    BEAM_VALUE[turn][BEAM_COUNT[turn]] = value;
    BEAM_COUNT[turn] += 1;
    std::push_heap(BEAM_QUEUE[turn], BEAM_QUEUE[turn]+BEAM_COUNT[turn], heap_cmp);
  }else if(value > BEAM_VALUE[turn][BEAM_QUEUE[turn][0]]) {
    num_nodes += 1;
    int ix = BEAM_QUEUE[turn][0];
    std::pop_heap(BEAM_QUEUE[turn], BEAM_QUEUE[turn]+BEAM_COUNT[turn], heap_cmp);
    auto& sb = BEAM[turn][ix];
    build_state(sb);
    entry = { .hash = sb.hash, .turn = sb.turn, .money = sb.money };
    BEAM_VALUE[turn][ix] = value;
    std::push_heap(BEAM_QUEUE[turn], BEAM_QUEUE[turn]+BEAM_COUNT[turn], heap_cmp);
  }
};
int bfs_dist[NN];
int bfs_score[NN];
int bfs_from[NN];
int bfs_root[NN];
int bfs_last[NN];
int bfs_time = 0;
int bfs_queue[NN];
void bfs_add(int& nq, int i, int d, int fr, int root, int score) {
  score += cell_score[i];
  if(bfs_last[i] != bfs_time || d < bfs_dist[i]) {
    bfs_root[i] = root;
    bfs_from[i] = fr;
    bfs_dist[i] = d;
    bfs_last[i] = bfs_time;
    bfs_score[i] = score;
    bfs_queue[nq++] = i;
  } else if(bfs_last[i] == bfs_time && d == bfs_dist[i]
            && score > bfs_score[i]) {
    bfs_score[i] = score;
    bfs_from[i] = fr;
    bfs_root[i] = root;
  }else if(bfs_last[i] == bfs_time && d == bfs_dist[i]
            && score == bfs_score[i] && rng.random32(2) == 0) {
    bfs_from[i] = fr;
    bfs_root[i] = root;
  }
}
void do_bfs(beam_state const& sa) {
  bfs_time += 1;
  int nq = 0;
  for(i32 i = 0; i < (i32)(NN); ++i) {
    if(sa.built_stations.get(i)) {
      bfs_add(nq, i, 0, -1, i, 0);
    }else if(sa.built_rail.get(i)) {
      bfs_last[i] = bfs_time;
      bfs_dist[i] = 1;
      bfs_from[i] = -1;
      bfs_root[i] = i;
    }
  }
  for(i32 iq = 0; iq < (i32)(nq); ++iq) {
    int u = bfs_queue[iq];
#pragma GCC unroll 4
    for(i32 d = 0; d < (i32)(4); ++d) if(graph[u] & bit(d)) {
      int v = u+dxy[d];
      int penalty = 0;
      if(sa.built_stations.get(u)) {
        int d1 = (d+1)%4;
        int d2 = (d+3)%4;
        if((graph[u] & bit(d1)) && sa.built_rail.get(u+dxy[d1])) {
          penalty += 1000;
        }
        if((graph[u] & bit(d2)) && sa.built_rail.get(u+dxy[d2])) {
          penalty += 1000;
        }
      }
      if(!sa.built_rail.get(v)) {
        bfs_add(nq, v, bfs_dist[u]+1, u, bfs_root[u], bfs_score[u] - penalty);
      }
    }
  }
}
void do_bfs_init(int init_station) {
  bfs_time += 1;
  int nq = 0;
  bfs_add(nq, init_station, 0, -1, init_station, 0);
  for(i32 iq = 0; iq < (i32)(nq); ++iq) {
    int u = bfs_queue[iq];
#pragma GCC unroll 4
    for(i32 d = 0; d < (i32)(4); ++d) if(graph[u] & bit(d)) {
      int v = u+dxy[d];
      bfs_add(nq, v, bfs_dist[u]+1, u, bfs_root[u], bfs_score[u]);
    }
  }
}
void init_beam(int beam_width, f32 &init_beam_time) {
  reset_beam();
  u64 h0 = rng.randomInt64();
  f32 t0 = TIMER.elapsed();
  for(i32 i = 0; i < (i32)(T+1); ++i) for(i32 j = 0; j < (i32)(MAX_BEAM_WIDTH); ++j) BEAM_QUEUE[i][j] = j;
  beam_state s;
  for(i32 x = 0; x < (i32)(NN); ++x) if(reachable_from_station.size(x) > 0) {
    if(TIMER.elapsed() > TL_BEAM) break;
    do_bfs_init(x);
    set<int> stations;
    reachable_from_station.iter(x, [&](int d) {
      for(int g : destinations_stations[d^1]) {
        stations.insert(g);
      }
    });
    for(int y : stations) {
      if(TIMER.elapsed() > TL_BEAM) break;
      int dist = abs(x/N-y/N) + abs(x%N-y%N);
      if(initial_money - 2 * 5000 - (dist-1) * 100 < 0) continue;
      s.reset(h0);
      s.turn = dist+1;
      s.money = initial_money - 2 * 5000 - (dist-1) * 100;
      s.hash ^= hash_station[x] ^ hash_station[y];
      s.init_add_station(x);
      s.init_add_station(y);
      if(s.income == 0) continue;
      s.money += s.income;
      beam_add_state
        (beam_width,
         s.turn, s.income, s.half_income, s.money, 0,
         s.hash,
         [&](beam_state& sb) __attribute__((always_inline)) -> void {
           sb = s;
           sb.reset_parent();
           sb.history = make_history(x, -1);
           for(int u = y; u != -1; u = bfs_from[u]){
             if(!sb.built_stations.get(u)) {
               sb.built_rail.set(u);
             }
             sb.parent[u] = bfs_from[u]-u;
           }
           sb.parent[x] = 0;
           sb.history = make_history(y, sb.history);
         });
    }
  }
  f32 t1 = TIMER.elapsed();
  init_beam_time = t1-t0;
}
vector<int> reconstruct_build_order(int best_history) {
  vector<int> build_order;
  for(int h = best_history; h != -1; h = history_pool[h].prev) {
    build_order.push_back(history_pool[h].data);
  }
  reverse(begin(build_order), end(build_order));
  return build_order;
}
int get_rail_type(int a, int b, int c) {
  int d1 = -1, d2 = -1;
  for(i32 d = 0; d < (i32)(4); ++d) if(b == a + dxy[d]) d1 = d;
  for(i32 d = 0; d < (i32)(4); ++d) if(c == a + dxy[d]) d2 = d;
  if(d1 > d2) swap(d1, d2);
  if(d1 == 0 && d2 == 2) return 2;
  if(d1 == 1 && d2 == 3) return 1;
  if(d1 == 0 && d2 == 1) return 4;
  if(d1 == 0 && d2 == 3) return 5;
  if(d1 == 1 && d2 == 2) return 3;
  if(d1 == 2 && d2 == 3) return 6;
  do { if(!(false)) { throw runtime_error("main.cpp" ":" "614" " Assertion failed: " "false"); } } while(0);
}
int dirs_from_rail_type(int ty) {
  if(ty == 0) return 15;
  if(ty == 1) return bit(1) | bit(3);
  if(ty == 2) return bit(0) | bit(2);
  if(ty == 3) return bit(1) | bit(2);
  if(ty == 4) return bit(0) | bit(1);
  if(ty == 5) return bit(0) | bit(3);
  if(ty == 6) return bit(2) | bit(3);
  do { if(!(false)) { throw runtime_error("main.cpp" ":" "625" " Assertion failed: " "false"); } } while(0);
}
vector<int> resolve_build_order(vector<i8> parent, vector<int> build_order) {
  static int post_time = 0;
  static int post_vis[NN];
  static int post_is_station[NN];
  post_time += 1;
  vector<int> bo;
  for(int x : build_order) if(x < NN) {
    if(post_vis[x] != post_time) {
      int prev = x;
      int y = parent[x] == 0 ? -1 : x+parent[x];
      for(; y != -1 && post_vis[y] != post_time; y = parent[y] == 0 ? -1 : y+parent[y]) {
        post_vis[y] = post_time;
        bo.push_back(y*7 + get_rail_type(y, prev, parent[y] == 0 ? -1 : y+parent[y]));
        prev = y;
      }
      if(y != -1 && post_is_station[y] != post_time) {
        bo.push_back(y*7+0);
        post_is_station[y] = post_time;
      }
    }
    if(post_is_station[x] != post_time) {
      bo.push_back(x*7+0);
      post_vis[x] = post_time;
      post_is_station[x] = post_time;
    }
  }
  return bo;
}
// vector<int> resolve_build_order(vector<int> build_order) {
//   bitset<NN> active_stations = 0;
//   int nbuild_order = build_order.size();
//   vector<int> out(nbuild_order);
//   FOR(i, nbuild_order) {
//     int x = build_order[i];
//     if(x < NN) {
//       out[i] = 7*x;
//       active_stations[x] = 1;
//     }else{
//       x -= NN;
//       int a = -1, b = -1;
//       if(i > 0 && build_order[i-1] >= NN) {
//         a = build_order[i-1] - NN;
//       }else{
//         FOR(d, 4) if((graph[x] & bit(d)) && active_stations[x + dxy[d]]) {
//           a = x + dxy[d];
//         }
//       }
//       b = build_order[i+1] % NN;
//       out[i] = 7*x + get_rail_type(x,a,b);
//     }
//   }
//   return out;
// }
vector<int> reconstruct_build_order
(smallvec<int, NN> const& bo,
 int& obtained_result,
 int &obtained_result_at,
 int overbuild_repeat_limit,
 int escape_to_2) {
  int money = initial_money;
  int income = 0;
  int turn = 0;
  int best_outcome = money;
  int best_outcome_at = 0;
  bitset<2*MAX_EDGES> active_destinations = 0;
  vector<int> out;
  deque<int> overbuilds;
  bo.iter([&](int x) {
    int old_income = income;
    if(x%7 == 0) {
      reachable_from_station.iter(x/7, [&](int u) {
        if(!active_destinations[u]) {
          active_destinations[u] = 1;
          if(active_destinations[u^1]) {
            income += edge_income[u/2];
          }
        }
      });
    }
    int cost = (x%7 == 0 ? 5000 : 100);
    while(cost > money) {
      if(turn == T) break;
      if(money/100 > (int)overbuilds.size()) overbuilds.push_back(turn);
      money += old_income;
      turn += 1;
      out.push_back(-1);
    }
    if(turn == T) return;
    if(cost == 100 && !overbuilds.empty()) {
      out[overbuilds.front()] = x;
      overbuilds.pop_front();
      money -= cost;
      if((int)overbuilds.size() > money / 100) overbuilds.resize(money/100);
    }else{
      int num_repeat = 0;
    redo_wait:;
      if(turn == T) return;
      int money_if_nowait = money - cost;
      int overbuild_if_nowait =
        min<int>((money_if_nowait + income) / 100,
                 min<int>(money_if_nowait/100, overbuilds.size()) + 1);
      int money_if_wait = money + old_income - cost;
      int overbuild_if_wait = min<int>(money_if_wait/100, overbuilds.size()+1);
      if((num_repeat < overbuild_repeat_limit && overbuild_if_wait > overbuild_if_nowait)
         || (overbuild_if_wait > overbuild_if_nowait+escape_to_2)) {
        num_repeat += 1;
        if(money/100 > (int)overbuilds.size()) overbuilds.push_back(turn);
        money += old_income;
        turn += 1;
        out.push_back(-1);
        goto redo_wait;
      }
      turn += 1;
      out.push_back(x);
      money -= cost;
      if((int)overbuilds.size() > money / 100) overbuilds.resize(money/100);
      money += income;
    }
    best_outcome = max(best_outcome, money + (T-turn) * income);
    if(money + (T-turn) * income == best_outcome) {
      best_outcome_at = turn;
    }
    do { if(!(turn == (int)out.size())) { throw runtime_error("main.cpp" ":" "769" " Assertion failed: " "turn == (int)out.size()"); } } while(0);
  });
  // while(turn < T) {
  //   turn += 1;
  //   money += income;
  //   out.pb(-1);
  // }
  obtained_result = best_outcome; // money;
  obtained_result_at = best_outcome_at;
  return out;
}
void sim_build_order
(smallvec<int, NN> const& bo,
 int &obtained_result,
 int overbuild_repeat_limit,
 int escape_to_2) {
  int money = initial_money;
  int income = 0;
  int turn = 0;
  int best_outcome = money;
  int best_outcome_at = 0;
  bitset<2*MAX_EDGES> active_destinations = 0;
  static int overbuilds[2*NN+1];
  int ob_front = NN;
  int ob_back = NN;
  auto num_overbuilds = [&](){
    return ob_back-ob_front;
  };
  auto overbuilds_pb = [&](int x){
    overbuilds[ob_back++] = x;
  };
  auto overbuilds_resize = [&](int sz){
    ob_back = ob_front+sz;
  };
  bo.iter([&](int x) __attribute__((always_inline)) -> void {
    int old_income = income;
    if(x%7 == 0) {
      reachable_from_station.iter(x/7, [&](int u) {
        if(!active_destinations[u]) {
          active_destinations[u] = 1;
          if(active_destinations[u^1]) {
            income += edge_income[u/2];
          }
        }
      });
    }
    int cost = (x%7 == 0 ? 5000 : 100);
    while(cost > money) {
      if(turn == T) break;
      if(money/100 > num_overbuilds()) overbuilds_pb(turn);
      money += old_income;
      turn += 1;
    }
    if(turn == T) return;
    if(cost == 100 && ob_back != ob_front) {
      ob_front++;
      money -= cost;
      if(num_overbuilds() > money / 100) overbuilds_resize(money/100);
    }else{
      int num_repeat = 0;
    redo_wait:;
      if(turn == T) return;
      int money_if_nowait = money - cost;
      int overbuild_if_nowait =
        min<int>((money_if_nowait + income) / 100,
                 min<int>(money_if_nowait/100, num_overbuilds()) + 1);
      int money_if_wait = money + old_income - cost;
      int overbuild_if_wait = min<int>(money_if_wait/100, num_overbuilds()+1);
      if((num_repeat < overbuild_repeat_limit && overbuild_if_wait > overbuild_if_nowait)
         || (overbuild_if_wait > overbuild_if_nowait+escape_to_2)) {
        num_repeat += 1;
        if(money/100 > num_overbuilds()) overbuilds_pb(turn);
        money += old_income;
        turn += 1;
        goto redo_wait;
      }
      turn += 1;
      money -= cost;
      if(num_overbuilds() > money / 100) overbuilds_resize(money/100);
      money += income;
    }
    best_outcome = max(best_outcome, money + (T-turn) * income);
    if(money + (T-turn) * income == best_outcome) {
      best_outcome_at = turn;
    }
  });
  obtained_result = best_outcome; // money;
}
tuple<int, vector<i8>, vector<int>> solve_beam(int beam_width, f32& init_beam_time) {
  init_beam(beam_width, init_beam_time);
  vector<i8> best_parents(NN, 0);
  int best_outcome = initial_money;
  int best_history = -1;
  for(i32 beam_turn = 0; beam_turn < (i32)(T); ++beam_turn) {
    if(TIMER.elapsed() > TL_BEAM_STOP) {
      do { cerr << "main.cpp" ":" "885" "  {" << "\"stop\", beam_turn" << "} = {"; debug_impl_seq("stop", beam_turn); cerr << endl << flush; } while(0);
      break;
    }
    for(i32 ia = 0; ia < (i32)(BEAM_COUNT[beam_turn]); ++ia) {
      auto sa = BEAM[beam_turn][ia];
      auto const& entry = HASH_TABLE[sa.hash&HASH_MASK];
      if(entry.hash == sa.hash &&
         (entry.turn != sa.turn ||
          entry.money != sa.money)) {
        continue;
      }
      do_bfs(sa);
      static int ray[NN][4];
      for(i32 d = 0; d < (i32)(2); ++d) for(i32 u = 0; u < (i32)(NN); ++u) {
        if(graph[u] & bit(d)) ray[u][d] = ray[u+dxy[d]][d];
        else ray[u][d] = -1;
        if(sa.built_stations.get(u) || sa.built_rail.get(u)) ray[u][d] = u;
      }
      for(i32 d = (2); d <= (i32)(3); ++d) for(i32 u = (NN-1); u >= (i32)(0); --u) {
        if(graph[u] & bit(d)) ray[u][d] = ray[u+dxy[d]][d];
        else ray[u][d] = -1;
        if(sa.built_stations.get(u) || sa.built_rail.get(u)) ray[u][d] = u;
      }
      static int count_destination[2*MAX_EDGES];
      for(i32 i = 0; i < (i32)(2*num_edges); ++i) count_destination[i] = sa.active_destinations.get(i) ? 1 : 0;
      int num_active = 0;
      int old_income = sa.income;
      for(i32 igroup = 0; igroup < (i32)(ngroups); ++igroup) {
        for(i32 x = (rem_at_group.pos[igroup]); x <= (i32)(rem_at_group.pos[igroup+1]-1); ++x) {
          int u = rem_at_group.data[x];
          count_destination[u] -= 1;
          if(count_destination[u] == 0) {
            num_active -= 1;
            sa.rem_destination(u);
          }
        }
        for(i32 x = (add_at_group.pos[igroup]); x <= (i32)(add_at_group.pos[igroup+1]-1); ++x) {
          int u = add_at_group.data[x];
          if(count_destination[u] == 0) {
            num_active += 1;
            sa.add_destination(u);
          }
          count_destination[u] += 1;
        }
        if(num_active == 0) continue;
        auto nth_parent = [&](int u, int n) __attribute__((always_inline)) -> int {
          for(i32 i = 0; i < (i32)(n); ++i) {
            u = u+sa.parent[u];
            if(i != n-1 && !sa.built_rail.get(u)) return -1;
            if(u == -1) return u;
          }
          return u;
        };
        auto test_ray = [&](int station) -> int {
          if(bfs_last[station] != bfs_time || bfs_dist[station] != 1) {
#pragma GCC unroll 4
            for(i32 d1 = 0; d1 < (i32)(4); ++d1) {
              int d2 = (d1+1)%4;
              int u1 = ray[station][d1];
              int u2 = ray[station][d2];
              if(u1 != -1 && u2 != -1) {
                int dist1 = abs(u1-station) / abs(dxy[d1]);
                int dist2 = abs(u2-station) / abs(dxy[d2]);
                int v1 = nth_parent(u1, dist1+dist2);
                if(v1 == u2) { return d1; }
                int v2 = nth_parent(u2, dist1+dist2);
                if(v2 == u1) { return d1; }
              }
            }
          }
          return -1;
        };
        int where = -1;
        int cost = 1e9;
        int cost_plus = 1e9;
        int where_rng_count = 0;
        int do_ray = 0;
        for(i32 x = (group_stations.pos[igroup]); x <= (i32)(group_stations.pos[igroup+1]-1); ++x) {
          int station = group_stations.data[x];
          int can_ray = test_ray(station);
          if(can_ray == -1 && bfs_last[station] != bfs_time) continue;
          int local_cost =
            can_ray != -1
            ? 5000
            : (bfs_dist[station]-1) * 100 + 5000;
          int local_cost_plus = 10000*local_cost;
          // if(can_ray != -1) {
          //   local_cost_plus -=
          //     pow(bfs_root[station]/N-station/N, 2) + 
          //     pow(bfs_root[station]%N-station%N, 2);
          // }
          if(local_cost_plus < cost_plus) {
            cost_plus = local_cost_plus;
            where_rng_count = 0;
          }
          if(local_cost_plus == cost_plus) {
            where_rng_count += 1;
            if(rng.random32(where_rng_count) == 0) {
              where = station;
              do_ray = can_ray != -1;
              cost_plus = local_cost_plus;
              cost = local_cost;
            }
          }
        }
        if(where == -1) continue;
        if(cost > sa.money && old_income == 0) continue;
        int money_turn;
        if(cost <= sa.money) {
          money_turn = sa.turn;
        } else {
          money_turn = sa.turn + (cost - sa.money + old_income-1) / old_income;
        }
        int use_overbuilds = min((do_ray ? 1 : bfs_dist[where]) - 1, sa.overbuilds);
        int build_turn = sa.turn + ((do_ray ? 1 : bfs_dist[where]) - 1 - use_overbuilds);
        int turn = max(money_turn, build_turn) + 1;
        int money = sa.money + (turn - sa.turn - 1) * old_income;
        int new_overbuilds
          = min(sa.overbuilds - use_overbuilds + max(0, money_turn - build_turn),
                (money - cost) / 100);
      redo_wait2:;
        int money_if_nowait = money - cost;
        int overbuild_if_nowait =
          min<int>((money_if_nowait + sa.income) / 100,
                   min<int>(money_if_nowait/100, new_overbuilds) + 1);
        int money_if_wait = money + old_income - cost;
        int overbuild_if_wait = min<int>(money_if_wait/100, new_overbuilds+1);
        if(overbuild_if_wait > overbuild_if_nowait+1) {
          money += old_income;
          turn += 1;
          new_overbuilds
            = min(sa.overbuilds - use_overbuilds + max(0, money_turn - build_turn),
                  (money - cost) / 100);
          goto redo_wait2;
        }
        money -= cost;
        money += sa.income;
        beam_add_state
          (beam_width, turn, sa.income, sa.half_income, money, new_overbuilds,
           sa.hash, [&](beam_state& sb) __attribute__((always_inline)) -> void {
             sb = sa; sb.turn = turn; sb.money = money;
             int can_ray = test_ray(where);
             if(can_ray != -1) {
               int d1 = can_ray;
               int d2 = (can_ray+1)%4;
               int u1 = ray[where][d1];
               int u2 = ray[where][d2];
               int dist1 = abs(u1-where) / abs(dxy[d1]);
               int dist2 = abs(u2-where) / abs(dxy[d2]);
               if(nth_parent(u1, dist1+dist2) == u2) { }
               else { swap(u1, u2); swap(d1, d2); swap(dist1, dist2); }
               int uu = u1+sa.parent[u1];
               for(i32 i = 0; i < (i32)(dist1+dist2 - 1); ++i) {
                 sb.built_rail.unset(uu);
                 sb.parent[uu] = 0;
                 uu = uu+sa.parent[uu];
               }
               sb.built_stations.set(where);
               uu = u1;
               for(i32 i = 0; i < (i32)(dist1); ++i) {
                 sb.parent[uu] = dxy[d1^2];
                 uu += dxy[d1^2];
                 if(i != dist1-1) sb.built_rail.set(uu);
               }
               for(i32 i = 0; i < (i32)(dist2); ++i) {
                 sb.parent[uu] = dxy[d2];
                 uu += dxy[d2];
                 if(i != dist2-1) sb.built_rail.set(uu);
               }
               sb.built_rail.unset(where);
               sb.overbuilds = new_overbuilds;
               sb.history = make_history(where, sb.history);
             }else{
               sb.built_stations.set(where);
               for(int u = bfs_from[where]; u != -1; u = bfs_from[u]){
                 if(!sb.built_stations.get(u)) {
                   sb.built_rail.set(u);
                   sb.parent[u] = bfs_from[u]-u;
                 }
               }
               if(!sb.built_rail.get(where)) {
                 sb.parent[where] = bfs_from[where]-where;
               }
               sb.built_rail.unset(where);
               sb.history = make_history(where, sb.history);
               sb.overbuilds = new_overbuilds;
             }
             int outcome = sb.money + (T-sb.turn) * sb.income;
             if(outcome > best_outcome) {
               best_outcome = outcome;
               best_history = sb.history;
               best_parents.assign(sb.parent, sb.parent+NN);
             }
           });
      }
    }
  }
  // FOR(i, N) {
  //   FOR(j, N) {
  //     cerr << setw(4) << (int)best_parents[i*N+j] << ' ';
  //   }
  //   cerr << endl;
  // }
  do { cerr << "main.cpp" ":" "1121" "  {" << "best_outcome" << "} = {"; debug_impl_seq(best_outcome); cerr << endl << flush; } while(0);
  return {best_outcome, best_parents, reconstruct_build_order(best_history)};
}
smallvec<int, NN> postprocess(vector<int> build_order) {
  union_find uf(NN);
  vector<vector<int>> tree(NN);
  vector<int> station_order;
  int repeat_limit = 50;
  int escape = 5;
  static bool built_station[NN];
  static bool built_rail[NN];
  for(i32 i = 0; i < (i32)(NN); ++i) {
    built_station[i] = 0;
    built_rail[i] = 0;
  }
  for(int x : build_order) {
    if(x%7 == 0) { built_station[x/7] = 1; built_rail[x/7] = 0; }
    else built_rail[x/7] = 1;
  }
  for(int x : build_order) {
    if(x % 7 == 0) {
      station_order.push_back(x / 7);
    }
    for(i32 d = 0; d < (i32)(4); ++d) if(dirs_from_rail_type(x % 7) & graph[x / 7] & bit(d)) {
      int y = (x/7)+dxy[d];
      if((built_station[y] || built_rail[y]) && uf.unite(x/7,y)) {
        tree[x/7].push_back(y);
        tree[y].push_back(x/7);
      }
    }
  }
  static int parent[NN];
  auto compute_parents = [&]() {
    auto dfs = [&](auto self, int i, int p) -> void {
      parent[i] = p;
      for(int j : tree[i]) if(j != p) {
          self(self, j,i);
        }
    };
    dfs(dfs, station_order[0], -1);
  };
  compute_parents();
  auto build_order_from_station_order
    = [&](smallvec<int, NN>& bo, vector<int> const& station_order)
    {
      static int post_time = 0;
      static int post_vis[NN];
      static int post_is_station[NN];
      post_time += 1;
      bo.clear();
      for(int x : station_order) {
        if(post_vis[x] != post_time) {
          int prev = x;
          int y = parent[x];
          for(; y != -1 && post_vis[y] != post_time; y = parent[y]) {
            post_vis[y] = post_time;
            bo.push(y*7 + get_rail_type(y, prev, parent[y]));
            prev = y;
          }
          if(y != -1 && post_is_station[y] != post_time) {
            bo.push(y*7+0);
            post_is_station[y] = post_time;
          }
        }
        if(post_is_station[x] != post_time) {
          bo.push(x*7+0);
          post_vis[x] = post_time;
          post_is_station[x] = post_time;
        }
      }
      return bo;
    };
  smallvec<int, NN> bo;
  int score;
  {
    build_order_from_station_order(bo, station_order);
    sim_build_order(bo, score, repeat_limit,escape);
  }
  do { cerr << "main.cpp" ":" "1211" "  {" << "score" << "} = {"; debug_impl_seq(score); cerr << endl << flush; } while(0);
  auto best_station_order = station_order;
  int best_score = score;
  f32 temp0 = 600;
  f32 temp1 = 10;
  int niter = 0;
  f32 t0 = TIMER.elapsed();
  f32 temp = temp0;
  while(1) {
    niter += 1;
    if(niter % 512 == 0) {
      f32 done = (TIMER.elapsed()-t0) / (TL_POST - t0);
      temp = temp0 * pow(temp1 / temp0, done);
      if(done > 1.0) break;
    }
    int ty = rng.random32(30)-2;
    if(niter < 1000) ty = 2;
    if(ty == -2) {
      int i = rng.random32(NN);
      if(!built_station[i]) continue;
      // if(tree[i].size() != 2) continue;
      if(station_order[0] == i) continue;
      int j = rng.sample(tree[i]);
      if(built_station[j]) continue;
      do { if(!(built_rail[j])) { throw runtime_error("main.cpp" ":" "1244" " Assertion failed: " "built_rail[j]"); } } while(0);
      auto new_station_order = station_order;
      for(int& x : new_station_order) if(x == i) x = j;
      build_order_from_station_order(bo, new_station_order);
      int new_score;
      sim_build_order(bo, new_score, repeat_limit,escape);
      if(new_score >= score || score - new_score <= temp * rng.randomDouble()) {
        if(new_score > best_score) {
          best_score = new_score;
          best_station_order = new_station_order;
          do { cerr << "main.cpp" ":" "1255" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
        }
        score = new_score;
        built_station[i] = 0;
        built_rail[i] = 1;
        built_station[j] = 1;
        built_rail[j] = 0;
        station_order = new_station_order;
      }
    }else if(ty == -1) {
      int i = rng.random32(NN);
      if(!built_station[i] || parent[i] == -1) continue;
      // disconnect and reconnect along the shortest path
      int j = i;
      for(; j == i || (built_rail[j] && tree[j].size() == 1); j = parent[j]) {
        if(j != i) {
          built_rail[j] = 0;
        }
        // runtime_assert(find(all(tree[j]), parent[j]) != end(tree[j]));
        // runtime_assert(find(all(tree[parent[j]]), j) != end(tree[parent[j]]));
        tree[j].erase(find(begin(tree[j]), end(tree[j]), parent[j]));
        tree[parent[j]].erase(find(begin(tree[parent[j]]), end(tree[parent[j]]), j));
      }
      built_rail[j] = 0;
      // find a path from i to j
      bfs_time += 1;
      int nq = 0;
      bfs_add(nq, i, 0, -1, i, 0);
      for(i32 iq = 0; iq < (i32)(nq); ++iq) {
        int u = bfs_queue[iq];
#pragma GCC unroll 4
        for(i32 d = 0; d < (i32)(4); ++d) if(graph[u] & bit(d)) {
          int v = u+dxy[d];
          if(!built_rail[v] && (v == j || !built_station[v])) {
            bfs_add(nq, v, bfs_dist[u]+1, u, bfs_root[u], bfs_score[u]);
          }
        }
        if(bfs_last[j] == bfs_time) break;
      }
      for(; bfs_from[j] != -1; j = bfs_from[j]) {
        tree[j].push_back(bfs_from[j]);
        tree[bfs_from[j]].push_back(j);
        if(!built_station[j]) {
          built_rail[j] = 1;
        }
      }
      compute_parents();
      int new_score;
      build_order_from_station_order(bo, station_order);
      sim_build_order(bo, new_score, repeat_limit,escape);
      if(new_score < score) {
        do { cerr << "main.cpp" ":" "1311" "  {" << "\"maybe bad\", new_score, score" << "} = {"; debug_impl_seq("maybe bad", new_score, score); cerr << endl << flush; } while(0);
      }
      if(new_score > score) {
        do { cerr << "main.cpp" ":" "1314" "  {" << "\"good\", new_score, score" << "} = {"; debug_impl_seq("good", new_score, score); cerr << endl << flush; } while(0);
      }
      // runtime_assert(new_score >= score);
      score = new_score;
      if(new_score > best_score) {
        best_score = new_score;
        best_station_order = station_order;
        do { cerr << "main.cpp" ":" "1322" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
      }
    } else if(ty == 0) {
      int i = rng.random32(NN);
      if(!built_rail[i] || built_station[i]
         || reachable_from_station.size(i) == 0) continue;
      int j = 1+rng.random32(station_order.size());
      auto new_station_order = station_order;
      new_station_order.insert(begin(new_station_order)+j, i);
      build_order_from_station_order(bo, new_station_order);
      int new_score;
      sim_build_order(bo, new_score, repeat_limit,escape);
      if(new_score >= score || score - new_score <= temp * rng.randomDouble()) {
        if(new_score > best_score) {
          best_score = new_score;
          best_station_order = new_station_order;
          do { cerr << "main.cpp" ":" "1341" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
        }
        score = new_score;
        built_station[i] = 1;
        built_rail[i] = 0;
        station_order = new_station_order;
      }
    }else if(ty == 1) {
      if(station_order.size() <= 1) continue;
      int i = 1+rng.random32(station_order.size()-1);
      auto new_station_order = station_order;
      new_station_order.erase(begin(new_station_order)+i);
      build_order_from_station_order(bo, new_station_order);
      int new_score;
      sim_build_order(bo, new_score, repeat_limit,escape);
      if(new_score >= score || score - new_score <= temp * rng.randomDouble()) {
        if(new_score > best_score) {
          best_score = new_score;
          best_station_order = new_station_order;
          do { cerr << "main.cpp" ":" "1362" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
        }
        score = new_score;
        built_station[station_order[i]] = 0;
        built_rail[station_order[i]] = 1;
        station_order = new_station_order;
      }
    }else if(niter < 1000 || (ty == 2 && rng.random32(100) == 0)) {
      int new_repeat_limit = rng.random32(51);
      int new_escape = 1+rng.random32(5);
      build_order_from_station_order(bo, station_order);
      int new_score;
      sim_build_order(bo, new_score, new_repeat_limit,new_escape);
      if(new_score >= score || score - new_score <= temp * rng.randomDouble()) {
        if(new_score > best_score) {
          best_score = new_score;
          best_station_order = station_order;
          do { cerr << "main.cpp" ":" "1380" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
        }
        score = new_score;
        repeat_limit = new_repeat_limit;
        escape = new_escape;
      }
    }else{
      int i = 1+rng.random32(station_order.size());
      int j = 1+rng.random32(station_order.size());
      int k = 1+rng.random32(station_order.size());
      if(i>j) swap(i,j);
      if(j>k) swap(j,k);
      if(i>j) swap(i,j);
      if(j > i+2 && k > j+2) continue;
      auto new_station_order = station_order;
      rotate(begin(new_station_order)+i,
             begin(new_station_order)+j,
             begin(new_station_order)+k);
      build_order_from_station_order(bo, new_station_order);
      int new_score;
      sim_build_order(bo, new_score, repeat_limit,escape);
      if(new_score >= score || score - new_score <= temp * rng.randomDouble()) {
        if(new_score > best_score) {
          best_score = new_score;
          best_station_order = new_station_order;
          do { cerr << "main.cpp" ":" "1405" "  {" << "niter, best_score" << "} = {"; debug_impl_seq(niter, best_score); cerr << endl << flush; } while(0);
        }
        score = new_score;
        station_order = new_station_order;
      }
    }
  }
  do { cerr << "main.cpp" ":" "1412" "  {" << "niter" << "} = {"; debug_impl_seq(niter); cerr << endl << flush; } while(0);
  build_order_from_station_order(bo, best_station_order);
  return bo;
}
void solve() {
  f32 init_beam_time;
  auto t0 = TIMER.elapsed();
  auto [best_outcome, best_parents, best_build_order] = solve_beam(1, init_beam_time);
  do { cerr << "main.cpp" ":" "1422" "  {" << "best_outcome" << "} = {"; debug_impl_seq(best_outcome); cerr << endl << flush; } while(0);
  auto t1 = TIMER.elapsed();
  f32 beam4_time = (t1-t0) - init_beam_time;
  do { cerr << "main.cpp" ":" "1427" "  {" << "init_beam_time, beam4_time" << "} = {"; debug_impl_seq(init_beam_time, beam4_time); cerr << endl << flush; } while(0);
  do { cerr << "main.cpp" ":" "1429" "  {" << "t0, t1" << "} = {"; debug_impl_seq(t0, t1); cerr << endl << flush; } while(0);
  f32 remain = (TL_BEAM-t1);
  int new_beam_width
    = ceil(max(0.0f, remain - init_beam_time) / beam4_time * 1);
  new_beam_width = min(MAX_BEAM_WIDTH, new_beam_width);
  if(new_beam_width < 1) new_beam_width = 1;
  cerr << "[DATA] bw = " << new_beam_width << endl;
  {
    auto [outcome, parents, build_order] = solve_beam(new_beam_width, init_beam_time);
    do { cerr << "main.cpp" ":" "1441" "  {" << "outcome" << "} = {"; debug_impl_seq(outcome); cerr << endl << flush; } while(0);
    if(outcome > best_outcome) {
      best_outcome = outcome;
      best_parents = parents;
      best_build_order = build_order;
    }
  }
  do { cerr << "main.cpp" ":" "1449" "  {" << "init_beam_time" << "} = {"; debug_impl_seq(init_beam_time); cerr << endl << flush; } while(0);
  auto bo = resolve_build_order(best_parents, best_build_order);
  auto bo2 = postprocess(bo);
  int best_result = -1;
  vector<int> best_out;
  for(i32 repeat_limit = (0); repeat_limit <= (i32)(100); ++repeat_limit) for(i32 escape = (1); escape <= (i32)(6); ++escape) {
    int obtained_result;
    int at;
    auto out = reconstruct_build_order(bo2, obtained_result, at, repeat_limit, escape);
    out.resize(at);
    while(out.size() != T) out.push_back(-1);
    // debug(repeat_limit, obtained_result);
    if(obtained_result > best_result) {
      best_out = out;
      best_result = obtained_result;
    }
  }
  do { cerr << "main.cpp" ":" "1468" "  {" << "best_result" << "} = {"; debug_impl_seq(best_result); cerr << endl << flush; } while(0);
  for(auto p : best_out) {
    if(p < 0) cout << -1 << endl;
    else cout << p%7 << "  " << p/7/N << " " << p/7%N << endl;
  }
}
int main() {
  history_pool = (history_list*)malloc(sizeof(history_list) * HISTORY_SIZE);
  rng.reset(time(0));
  read();
  init();
  solve();
  cerr << "[DATA] time = " << (int)(TIMER.elapsed()*1000) << endl;
  do { cerr << "main.cpp" ":" "1482" "  {" << "num_nodes" << "} = {"; debug_impl_seq(num_nodes); cerr << endl << flush; } while(0);
  return 0;
}
