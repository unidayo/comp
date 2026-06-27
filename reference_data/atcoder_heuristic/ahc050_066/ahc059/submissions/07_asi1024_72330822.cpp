#pragma GCC target("arch=skylake-avx512")
#pragma GCC optimize("O3","inline")
// #pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f")

// #include <atcoder/dsu>
// #include <atcoder/segtree>
// #include <atcoder/mincostflow>

// #include <immintrin.h>

#define FINAL_SUBMIT 1

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <climits>
#include <complex>
#include <cstdio>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define REP(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(x) (x).begin(), (x).end()

#if FINAL_SUBMIT
#define HHH(x)
#define LOG(x)
#define FORCE_LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y)
#define CERR() if (false) std::cerr
#define NDEBUG
#else
#define HHH(x) std::cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << "\n"
#define LOG(x) std::cerr << #x << " = " << x << "\n"
#define FORCE_LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y) std::cerr << "graph " << x << " " << y << "\n"
#define CERR() std::cerr
#endif

template <typename T> T &chmin(T &a, const T &b) { return a = std::min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = std::max(a, b); }

using ll = long long;
using ld = double;
constexpr int INF = 1e9;
constexpr double PI = acos(-1.0);

template <typename T> T sq(T x) { return x * x; }

template <typename T, typename U>
std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& p) {
  return os << "(" << p.first << ", " << p.second << ")";
}
template <typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
  os << "[";
  for (const T& e : vec) os << " " << e;
  os << " ]";
  return os;
}

class Timer {
public:
  int64_t start, stop;

#ifdef __aarch64__
  static const int64_t CYCLES_PER_SEC = 800000000;
  inline int64_t get_cycle() const {
    uint64_t t;
    asm volatile("mrs %0, cntvct_el0" : "=r" (t));
    return t;
  }
#else
  static constexpr int64_t CYCLES_PER_SEC = 2400000000;
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
#endif

  Timer(double time_limit) : start(get_cycle()), stop(start + CYCLES_PER_SEC * time_limit) {}
  inline void reset_stop(double time_limit) { stop = start + CYCLES_PER_SEC *  time_limit; }
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
  inline double elapsed() const { return (get_cycle() - start) / double(CYCLES_PER_SEC); }
};

class XorShift {
  unsigned int x, y, z, w;
public:
  XorShift() : x(314159265), y(358979323), z(846264338), w(327950288) {}
  inline unsigned int next() {
    unsigned int t = x ^ x << 11;
    x = y; y = z; z = w;
    return w = w ^ w >> 19 ^ t ^ t >> 8;
  }
  inline int next(int n) { return next() % n; }
  inline int next(size_t n) { return next() % n; }
  inline int next(int lo, int hi) { return lo + next(hi - lo + 1); }
  inline double next(double x) { return double(next()) / (1LL << 32) * x; }
  inline double next(double lo, double hi) { return lo + next(hi - lo); }
};

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(std::string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int a_param = get_param("A_PARAM");
// int j_param = get_param("J_PARAM");

constexpr double TIME_LIMIT = 2.0;

Timer timer(TIME_LIMIT * 0.98);
XorShift rnd;

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0x1FF;
  constexpr static double START_TEMP = 1.5;
  constexpr static double END_TEMP   = 0.3;
  const int64_t CYCLES;
  // const double TEMP_RATIO = (END_TEMP - START_TEMP) / CYCLES;  // linear
  const double TEMP_RATIO; // = log(END_TEMP / START_TEMP) / CYCLES;  // exponential
  double logp[LOG_SIZE];
  long long iteration = 0;
  int64_t cycle = 0;
  int64_t start = timer.get_cycle();
  const int64_t medium;
  const int64_t stop;
  bool record = false;
  double temp = START_TEMP;

  SimulatedAnnealing(int64_t n_cycles) :
      CYCLES(n_cycles),
      TEMP_RATIO(log(END_TEMP / START_TEMP) / CYCLES),
      medium(start + CYCLES * 0.10),
      stop(start + CYCLES)
  {
    constexpr double inv = 1.0 / LOG_SIZE;
    REP(i,LOG_SIZE) logp[i] = log((i + 0.5) * inv);
    std::mt19937 rng;
    std::shuffle(logp, logp + LOG_SIZE, rng);
  }

  inline bool End() {
    iteration++;
    if ((iteration & UPDATE_INTERVAL) != 0) return false;
    cycle = timer.get_cycle();
    // temp = START_TEMP + (cycle - start) * TEMP_RATIO;  // linear
    temp = START_TEMP * exp((cycle - start) * TEMP_RATIO);  // exponential
    return cycle >= stop;
  }

  inline bool Accept(double diff) {
    static unsigned short index = 0;
    return diff >= 0 || diff > logp[index++] * temp;
  }
};

using namespace std;

constexpr int N = 20;
constexpr int UPDATE_ITERATION = 1000000;

constexpr int NN = N * N;
unsigned char dist_tbl[NN][NN]; // max dist = 2*(N-1)=38 < 255

inline int dist(int p1, int p2) {
  return dist_tbl[p1][p2];
}

// int dist(int p1, int p2) {
//   return std::abs(p1 / N - p2 / N) + std::abs(p1 % N - p2 % N);
// }

int A[N * N];
int P[512];

void SetUp() {
  int N_;
  scanf("%d", &N_);
  REP(i,N*N) scanf("%d", &A[i]);
  vector<bool> visited(N * N);
  REP(i,N*N) {
    if (!visited[A[i]]) {
      P[A[i] * 2 + 0] = i;
      visited[A[i]] = true;
    } else {
      P[A[i] * 2 + 1] = i;
    }
  }
  REP(i, NN) {
    int y1 = i / N, x1 = i % N;
    REP(j, NN) {
      int y2 = j / N, x2 = j % N;
      dist_tbl[i][j] = (unsigned char)(abs(y1 - y2) + abs(x1 - x2));
    }
  }
}

constexpr int root = N * N / 2;

vector<uint16_t> best_state[N * N / 2 + 1];
int best_K = INF;

vector<uint16_t> state[N * N / 2 + 1];
int parent[N * N / 2 + 1];
int parent_tmp[N * N / 2 + 1];

bool HasCycle(int v) {
  int x = v;
  REP(loop, 200) {
    if (x == root) return false;
    x = parent[x];
    if (x == v) return true;
  }
  return false;
}

void MoveTo(int& from, int to) {
  int dy = to / N - from / N;
  int dx = to % N - from % N;
  while (dy > 0) { printf("D\n"), --dy; }
  while (dy < 0) { printf("U\n"), ++dy; }
  while (dx > 0) { printf("R\n"), --dx; }
  while (dx < 0) { printf("L\n"), ++dx; }
  from = to;
}

void Output(int x, int& pos, bool rev) {
  int n = best_state[x].size();
  assert(n % 2 == 0);
  if (rev) {
    std::reverse(ALL(best_state[x]));
  }
  for (int i = 0; i < n; i += 2) {
    MoveTo(pos, P[best_state[x][i]]);
    printf("Z\n");
    Output(best_state[x][i] / 2, pos, best_state[x][i] % 2);
    MoveTo(pos, P[best_state[x][i + 1]]);
    printf("Z\n");
  }
}

int main() {
  SetUp();
  // REP(i,root+1) state[i].reserve(400);
  // REP(i,root+1) best_state[i].reserve(400);

  REP(i,root) {
    state[root].push_back(i * 2);
    state[root].push_back(i * 2 + 1);
    parent[i] = root;
  }
  int K = 0;
  REP(i,N*N) {
    if (i == 0) K += dist(0, P[state[root][i]]);
    else K += dist(P[state[root][i-1]], P[state[root][i]]);
  }

  SimulatedAnnealing sa(Timer::CYCLES_PER_SEC * 1.995);
  while (!sa.End()) {
    int op = rnd.next() % 100;
    if (op < 15) {
      // Reverse
      int i = rnd.next() % (root + 1);
      int n = state[i].size() / 2;
      if (n == 0) continue;
      int l = rnd.next(n);
      int r = rnd.next(n);
      if (l > r) swap(l, r);
      ++r;
      l *= 2; r *= 2; n *= 2;
      int dist1 = 0, dist2 = 0;
      int left_prev  = P[l == 0 ? i * 2 : state[i][l-1]];
      int left_curr  = P[state[i][l]];
      int right_next = P[r == n ? i * 2 + 1 : state[i][r]];
      int right_curr = P[state[i][r-1]];

      dist1 += dist(left_prev, left_curr);
      if (i < root || r < n) {
        dist1 += dist(right_next, right_curr);
      }
      dist2 += dist(left_prev, right_curr);
      if (i < root || r < n) {
        dist2 += dist(right_next, left_curr);
      }
      int diff = dist2 - dist1;
      if (sa.Accept(-diff)) {
        // if (diff < 0) {
        reverse(state[i].begin() + l, state[i].begin() + r);
        K += diff;
        if (sa.iteration >= UPDATE_ITERATION && K < best_K) {
          best_K = K;
          REP(i,root+1) best_state[i] = state[i];
        }
      }
    } else if (op < 40) {
      // Insert
      int from_to_i = rnd.next() % ((root + 1) * (root + 1));
      int from_i = from_to_i / (root + 1);
      int to_i = from_to_i % (root + 1);
      if (from_i == to_i) continue;

      int n = state[from_i].size() / 2;
      if (n == 0) continue;
      int lr = rnd.next() % (n * n);
      int l = lr / n;
      int r = lr % n;
      if (l > r) swap(l, r);
      ++r;
      l *= 2; r *= 2; n *= 2;

      int n2 = state[to_i].size() / 2;
      int pos = rnd.next(n2 + 1);
      pos *= 2; n2 *= 2;
      int lval = P[state[from_i][l]];
      int rval = P[state[from_i][r-1]];
      bool rev = rnd.next() % 2;
      int from_l = P[l == 0 ? from_i * 2 : state[from_i][l-1]];
      int from_r = P[r == n ? from_i * 2 + 1 : state[from_i][r]];
      int to_l = P[pos == 0 ? to_i * 2 : state[to_i][pos - 1]];
      int to_r = P[pos == n2 ? to_i * 2 + 1 : state[to_i][pos]];

      int dist1 = 0, dist2 = 0;
      dist1 += dist(from_l, lval);
      if (from_i < root || r < n) {
        dist1 += dist(from_r, rval);
      }
      if (to_i < root || pos < n2) {
        dist1 += dist(to_l, to_r);
      }

      if (rev) swap(lval, rval);
      dist2 += dist(to_l, lval);
      if (to_i < root || pos < n2) {
        dist2 += dist(rval, to_r);
      }
      if (from_i < root || r < n) {
        dist2 += dist(from_l, from_r);
      }

      int diff = dist2 - dist1;
      if (sa.Accept(-diff)) {
        std::copy(parent, parent + root + 1, parent_tmp);
        for (int j = l; j < r; j += 2) {
          parent[state[from_i][j] / 2] = to_i;
        }
        if (HasCycle(to_i)) {
          std::copy(parent_tmp, parent_tmp + root + 1, parent);
          continue;
        }

        if (rev) {
          std::reverse(state[from_i].begin() + l,
                       state[from_i].begin() + r);
        }
        state[to_i].insert(state[to_i].begin() + pos,
                           state[from_i].begin() + l,
                           state[from_i].begin() + r);
        state[from_i].erase(state[from_i].begin() + l,
                            state[from_i].begin() + r);
        K += diff;
        if (sa.iteration >= UPDATE_ITERATION && K < best_K) {
          best_K = K;
          REP(i,root+1) best_state[i] = state[i];
        }
      }
    } else {
      // Swap
      int from_to_i = rnd.next() % ((root + 1) * (root + 1));
      int from_i = from_to_i / (root + 1);
      int to_i = from_to_i % (root + 1);
      if (from_i == to_i) continue;

      int n = state[from_i].size() / 2;
      if (n == 0) continue;
      int n2 = state[to_i].size() / 2;
      if (n2 == 0) continue;

      int lr = rnd.next() % (n * n);
      int l = lr / n;
      int r = lr % n;
      if (l > r) swap(l, r);
      ++r;
      l *= 2; r *= 2; n *= 2;

      int pos = rnd.next() % (n2 * n2);
      int pos1 = pos / n2;
      int pos2 = pos % n2;
      if (pos1 > pos2) swap(pos1, pos2);
      ++pos2;
      pos1 *= 2; pos2 *= 2; n2 *= 2;

      int from_lval = P[state[from_i][l]];
      int from_rval = P[state[from_i][r-1]];
      int to_lval = P[state[to_i][pos1]];
      int to_rval = P[state[to_i][pos2-1]];
      int rev = rnd.next() % 4;
      bool rev_from = rev / 2;
      bool rev_to = rev % 2;
      int from_l = P[l == 0 ? from_i * 2 : state[from_i][l - 1]];
      int from_r = P[r == n ? from_i * 2 + 1 : state[from_i][r]];
      int to_l = P[pos1 == 0 ? to_i * 2 : state[to_i][pos1 - 1]];
      int to_r = P[pos2 == n2 ? to_i * 2 + 1 : state[to_i][pos2]];

      int dist1 = 0, dist2 = 0;
      dist1 += dist(from_l, from_lval);
      if (from_i < root || r < n) {
        dist1 += dist(from_r, from_rval);
      }
      dist1 += dist(to_l, to_lval);
      if (to_i < root || pos2 < n2) {
        dist1 += dist(to_rval, to_r);
      }

      if (rev_from) swap(from_lval, from_rval);
      if (rev_to) swap(to_lval, to_rval);

      dist2 += dist(to_l, from_lval);
      if (to_i < root || pos2 < n2) {
        dist2 += dist(from_rval, to_r);
      }
      dist2 += dist(from_l, to_lval);
      if (from_i < root || r < n) {
        dist2 += dist(to_rval, from_r);
      }

      int diff = dist2 - dist1;
      if (sa.Accept(-diff)) {
        std::copy(parent, parent + root + 1, parent_tmp);
        for (int j = l; j < r; j += 2) {
          parent[state[from_i][j] / 2] = to_i;
        }
        for (int j = pos1; j < pos2; j += 2) {
          parent[state[to_i][j] / 2] = from_i;
        }
        if (HasCycle(from_i) || HasCycle(to_i)) {
          std::copy(parent_tmp, parent_tmp + root + 1, parent);
          continue;
        }

        if (rev_from) {
          std::reverse(state[from_i].begin() + l,
                       state[from_i].begin() + r);
        }
        if (rev_to) {
          std::reverse(state[to_i].begin() + pos1,
                       state[to_i].begin() + pos2);
        }

        int16_t tmp[512];
        int len = pos2 - pos1;
        std::copy(state[to_i].begin() + pos1, state[to_i].begin() + pos2, tmp);
        state[to_i].erase(state[to_i].begin() + pos1, state[to_i].begin() + pos2);

        state[to_i].insert(state[to_i].begin() + pos1,
                           state[from_i].begin() + l,
                           state[from_i].begin() + r);
        state[from_i].erase(state[from_i].begin() + l,
                            state[from_i].begin() + r);

        state[from_i].insert(state[from_i].begin() + l, tmp, tmp + len);

        K += diff;
        if (sa.iteration >= UPDATE_ITERATION && K < best_K) {
          best_K = K;
          REP(i,root+1) best_state[i] = state[i];
        }
      }
    }
  }

  if (K < best_K) {
    best_K = K;
    REP(i,root+1) best_state[i] = state[i];
  }

  LOG(sa.iteration);
  int pos = 0;
  Output(root, pos, false);
  fflush(stdout);
  LOG(K);
  LOG(best_K);
  int score = N * N + 2 * N * N * N - best_K;
  LOG(score);
  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
}
