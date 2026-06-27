#pragma GCC optimize "-O3"
 #include <stdio.h>
 #include <bits/stdc++.h>
 #include <ext/pb_ds/assoc_container.hpp>
using namespace std;
template<class T>
using min_queue = priority_queue<T, vector<T>, greater<T>>;
template<class T>
using max_queue = priority_queue<T>;
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

const double TL = 1.95;
timer TM;
const int N = 10;
const int NN = N*N;
int A[NN];
struct state {
  uint8_t board[N][N];
  void reset(){
    for(int32_t i = 0; i < (int32_t)(N); ++i) for(int32_t j = 0; j < (int32_t)(N); ++j) board[i][j] = 0;
  }
  void shift(int d) {
    if(d == 0) {
      for(int32_t x = 0; x < (int32_t)(N); ++x) {
        int y = 0;
        while(y < N && board[x][y]) y += 1;
        int cur = y;
        while(y < N) {
          if(board[x][y]) {
            board[x][cur++] = board[x][y];
            board[x][y] = 0;
          }
          y += 1;
        }
      }
    } else if(d == 1) {
      for(int32_t x = 0; x < (int32_t)(N); ++x) {
        int y = 0;
        while(y < N && board[y][x]) y += 1;
        int cur = y;
        while(y < N) {
          if(board[y][x]) {
            board[cur++][x] = board[y][x];
            board[y][x] = 0;
          }
          y += 1;
        }
      }
    } else if(d == 2) {
      for(int32_t x = 0; x < (int32_t)(N); ++x) {
        int y = N-1;
        while(y >= 0 && board[x][y]) y -= 1;
        int cur = y;
        while(y >= 0) {
          if(board[x][y]) {
            board[x][cur--] = board[x][y];
            board[x][y] = 0;
          }
          y -= 1;
        }
      }
    }else if(d == 3) {
      for(int32_t x = 0; x < (int32_t)(N); ++x) {
        int y = N-1;
        while(y >= 0 && board[y][x]) y -= 1;
        int cur = y;
        while(y >= 0) {
          if(board[y][x]) {
            board[cur--][x] = board[y][x];
            board[y][x] = 0;
          }
          y -= 1;
        }
      }
    }
  }
  void set(int p, int v) {
    for(int32_t x = 0; x < (int32_t)(N); ++x) for(int32_t y = 0; y < (int32_t)(N); ++y) if(board[x][y] == 0){
      p -= 1;
      if(p == 0) {
        board[x][y] = v;
        return;
      }
    }
  }
  int score() const {
    static int uf[NN];
    static int sz[NN];
    for(int32_t i = 0; i < (int32_t)(NN); ++i) uf[i] = i;
    for(int32_t i = 0; i < (int32_t)(NN); ++i) sz[i] = 1;
    auto find = letrec([&](auto find, int u) -> int {
      return u==uf[u]?u:uf[u]=find(uf[u]);
    });
    auto unite = [&](int u, int v) {
      find(u); find(v);
      u=uf[u]; v=uf[v];
      if(u == v) return;
      if(sz[u] > sz[v]) swap(u,v);
      uf[u] = v;
      sz[v] += sz[u];
    };
    int sc = 0;
    for(int32_t x = 0; x < (int32_t)(N-1); ++x) for(int32_t y = 0; y < (int32_t)(N); ++y) {
      if(board[x][y] == board[x+1][y]) unite(x*N+y, (x+1)*N+y);
    }
    for(int32_t x = 0; x < (int32_t)(N); ++x) for(int32_t y = 0; y < (int32_t)(N-1); ++y) {
      if(board[x][y] == board[x][y+1]) unite(x*N+y, x*N+(y+1));
    }
    for(int32_t x = 0; x < (int32_t)(N); ++x) for(int32_t y = 0; y < (int32_t)(N); ++y) if(board[x][y]) {
      if(x*N+y == uf[x*N+y]) {
        sc += sz[x*N+y]*sz[x*N+y];
      }
    }
    return sc;
  }
};
void read(){
  for(int32_t i = 0; i < (int32_t)(NN); ++i) cin>>A[i];
}
int step(int istep, state current) {
  const int width = 20;
  const float cuct = current.score();
  float total[4] = {0};
  int cnt[4] = {0};
  int niter = 0;
  while(TM.elapsed() < TL * (istep+1)/100) {
    niter++;
    int d0 = 0;
    float w = 0;
    for(int32_t d = 0; d < (int32_t)(4); ++d) {
      if(cnt[d] == 0) { d0 = d; break; }
      int dw = total[d]/cnt[d] + cuct * sqrt(log(niter-1) / cnt[d]);
      if(dw > w) { w = dw; d0 = d; }
    }
    int scnt[4];
    for(int32_t i = 0; i < (int32_t)(4); ++i) scnt[i] = cnt[i];
    sort(begin(scnt), end(scnt));
    if(scnt[3] > 10 && scnt[3] > 1.5 * scnt[2]) {
      break;
    }
    int p[NN]; for(int32_t i = 0; i < (int32_t)(NN); ++i) p[i] = 1+rng.random32(NN-i);
    static vector<state> A;
    static vector<state> B;
    static vector<int> H;
    static vector<int> I;
    A.emplace_back(current);
    A.back().shift(d0);
    float bestH = A.back().score();
    float coeff = 1.0;
    for(int32_t istep2 = (istep+1); istep2 <= (int32_t)(min(istep+4, NN-1)); ++istep2) {
      for(int32_t ia = 0; ia < (int32_t)(A.size()); ++ia) {
        state& sa = A[ia];
#pragma GCC unroll 4
        for(int32_t d = 0; d < (int32_t)(4); ++d) {
          int ib = B.size();
          B.emplace_back();
          state& sb = B[ib];
          sb = sa;
          sb.set(p[istep2], ::A[istep2]);
          sb.shift(d);
          H.emplace_back(sb.score());
          I.emplace_back(ib);
        }
      }
      if((int)I.size() > width) {
        nth_element(begin(I), begin(I)+width, end(I),
                    [&](int i, int j){ return H[i] > H[j]; });
        I.resize(width);
      }
      A.clear();
      for(int i : I) {
        A.emplace_back(B[i]);
        bestH = max(bestH, H[i] * coeff);
      }
      B.clear();
      H.clear();
      I.clear();
      coeff *= 0.99;
    }
    A.clear();
    total[d0] += bestH;
    cnt[d0] += 1;
  }
  int d = 0;
  for(int32_t d0 = 0; d0 < (int32_t)(4); ++d0) if(total[d0]/cnt[d0] > total[d]/cnt[d]) d = d0;
  return d;
}
void solve(){
  state current; current.reset();
  for(int32_t istep = 0; istep < (int32_t)(100); ++istep){
    { int p; cin>>p;
      for(int32_t x = 0; x < (int32_t)(N); ++x) for(int32_t y = 0; y < (int32_t)(N); ++y) if(current.board[x][y] == 0){
        p -= 1;
        if(p == 0) {
          current.board[x][y] = A[istep];
          goto lok;
        }
      }
      do { if(!(false)) { throw runtime_error("main.cpp" ":" "233" " Assertion failed: " "false"); } } while(0);
    lok:;
    }
    int d = step(istep, current);
    current.shift(d);
    cout << "LFRB"[d] << endl;
  }
  int cnt[4] = {0,0,0,0};
  for(int32_t i = 0; i < (int32_t)(NN); ++i) cnt[A[i]]++;
  int raw = 0;
  for(int32_t i = 0; i < (int32_t)(4); ++i) raw += cnt[i]*cnt[i];
  int sc = current.score();
  cerr << "Score: " << 1.0f*sc/raw << endl;
}
int main(){
  TM.reset();
  read();
  solve();
  return 0;
}
