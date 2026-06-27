#pragma GCC target("arch=skylake-avx512")
#pragma GCC optimize("O3","inline")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f")

#include <atcoder/dsu>
// #include <atcoder/segtree>
#include <atcoder/mincostflow>

#include <immintrin.h>

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
  static constexpr int64_t CYCLES_PER_SEC = 2400000000;
  int64_t start, stop;
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
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

Timer timer(TIME_LIMIT * 0.997);
XorShift rnd;

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0xFF;
  constexpr static double START_TEMP = 0.150;
  constexpr static double END_TEMP   = 0.001;
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

constexpr int N = 10;
constexpr int L = 4;
constexpr int T = 500;
constexpr int K = 1;
ll A[N];
ll C[L * N];

void SetUp() {
  int N_, L_, T_, K_;
  scanf("%d %d %d %d", &N_, &L_, &T_, &K_);
  REP(j,N) scanf("%lld", &A[j]);
  REP(i,L) REP(j,N) scanf("%lld", &C[i * N + j]);
}

using State = vector<int>;

template <int Output = 0>
ll Simulate(const State& state, int def_value) {
  ll score = K;
  int piv = 0;
  ll P[L * N], B[L * N];
  std::fill(B, B + L * N, 1LL);
  std::fill(P, P + L * N, 0);
  REP(t,T) {
    // Command
    int ij = piv < int(state.size()) ? state[piv] : def_value;
    int i = ij / N;
    int j = ij % N;
    ll cost = C[ij] * (P[ij] + 1);
    if (cost <= score) {
      score -= cost;
      P[ij]++;
      ++piv;
      if (Output) {
        printf("%d %d\n", i, j);
      }
    } else {
      if (Output) {
        printf("-1\n");
      }
    }

    // Update
    REP(j,N) {
      score += A[j] * B[j] * P[j];
    }
    for (int ij = N; ij < L * N; ++ij) {
      B[ij - N] += B[ij] * P[ij];
    }
  }
  // ll new_score = 0;
  // REP(j,N) {
  //   new_score += A[j] * B[j] * P[j];
  // }
  if (Output) fflush(stdout);
  return score;
  // return new_score;
}

int main() {
  SetUp();

  vector<int> state;
  State best_state;
  ll best_cnt = 0;
  int best_def_value = -1;
  constexpr int MAX_LEN = 500;  // state の上限（適宜調整）
  constexpr int OFF_LEN = 400;  // state の上限（適宜調整）
  ll cur_cnt = 0;
  int def_value = 0;
  int iteration = 0;

  auto try_update = [&] {
      int op = rnd.next(112);
      ++iteration;

      // 変更を記録してリバートできるようにする
      int pos1 = -1, pos2 = -1;
      int old1 = -1;
      int inserted_pos = -1, inserted_val = -1;
      int erased_pos = -1, erased_val = -1;

      // 近傍生成
      if (op < 25) {
        // change
        if (state.empty()) return;
        pos1 = rnd.next((int)state.size());
        int nij = rnd.next() % 128 < 48 ? rnd.next(L) * N + def_value : rnd.next(L * N);
        if (state[pos1] == nij) return;
        old1 = state[pos1];
        state[pos1] = nij;
      } else if (op < 35) {
        // swap
        if ((int)state.size() < 2) return;
        pos1 = rnd.next((int)state.size());
        // pos2 = pos1 + rnd.next(1, 150);
        pos2 = pos1 + rnd.next(1, 150);
        // pos2 = pos1 + rnd.next() % 180 + rnd.next() % 180 - 180;
        // if (pos2 < 0) return;
        if (pos2 >= (int)state.size()) return;
        if (state[pos1] == state[pos2]) return;
        std::swap(state[pos1], state[pos2]);
      } else if (op < 60) {
        // rotate
        if ((int)state.size() < 2) return;
        pos1 = rnd.next((int)state.size());
        // pos2 = pos1 + rnd.next(1, 150);
        pos2 = pos1 + rnd.next(1, 60);
        // pos2 = pos1 + rnd.next() % 20 + rnd.next() % 20 + 1;
        // if (pos2 < 0) return;
        if (pos2 >= (int)state.size()) return;
        if (state[pos1] == state[pos2]) return;
        // std::swap(state[pos1], state[pos2]);
        if (pos1 < pos2) {
          std::rotate(state.begin() + pos1, state.begin() + pos1 + 1, state.begin() + pos2 + 1);
        } else {
          std::rotate(state.begin() + pos2, state.begin() + pos1, state.begin() + pos1 + 1);
        }
      } else if (op < 85) {
        // insert
        if ((int)state.size() >= MAX_LEN) return;
        inserted_pos = rnd.next((int)state.size() + 1);
        inserted_val = rnd.next() % 128 < 48 ? rnd.next(L) * N + def_value : rnd.next(L * N);
        // inserted_val = inserted_pos >= OFF_LEN ? rnd.next(L) * N + def_value : rnd.next(L * N);
        // inserted_val = rnd.next(L * N);
        state.insert(state.begin() + inserted_pos, inserted_val);
      } else {
        // erase
        if ((int)state.size() <= 1) return;
        erased_pos = rnd.next((int)state.size());
        erased_val = state[erased_pos];
        state.erase(state.begin() + erased_pos);
      }

      // 評価
      ll nxt_cnt = Simulate(state, def_value);
      ll diff = nxt_cnt - cur_cnt;

      if (diff >= 0 /* sa.Accept(diff) */) {
        // accept
        cur_cnt = nxt_cnt;
        if (cur_cnt > best_cnt) {
          best_cnt = cur_cnt;
          best_state = state;
          best_def_value = def_value;
        }
      } else {
        // revert
        if (op < 25) {
          state[pos1] = old1;
          // LOG(pos1);
          // LOG(state[pos1]);
        } else if (op < 35) {
          std::swap(state[pos1], state[pos2]);
        } else if (op < 60) {
          if (pos1 < pos2) {
            std::rotate(state.begin() + pos1, state.begin() + pos2, state.begin() + pos2 + 1);
          } else {
            std::rotate(state.begin() + pos2, state.begin() + pos2 + 1, state.begin() + pos1 + 1);
          }
        } else if (op < 85) {
          state.erase(state.begin() + inserted_pos);
          // LOG(inserted_pos);
        } else {
          state.insert(state.begin() + erased_pos, erased_val);
          // LOG(erased_pos);
        }
      }
  };    

  vector<State> initial_states(N);
  vector<pair<ll, int>> initial_counts(N);

  for (def_value = 0; def_value < N; ++def_value) {
    state = {0};
    cur_cnt = Simulate(state, def_value);

    Timer timer(TIME_LIMIT * 0.045);
    while (timer.within()) {
      try_update();
    }
    initial_states[def_value] = state;
    initial_counts[def_value] = {cur_cnt, def_value};
  }

  std::sort(initial_counts.rbegin(), initial_counts.rend());

  REP(i,4) {
    def_value = initial_counts[i].second;
    cur_cnt = initial_counts[i].first;
    state = initial_states[def_value];

    Timer timer(TIME_LIMIT * 0.060);
    while (timer.within()) {
      try_update();
    }
  }

  state = best_state;
  def_value = best_def_value;
  cur_cnt = best_cnt;
  LOG(iteration);

  while (timer.within()) {
    try_update();
  }

  Simulate<1>(best_state, best_def_value);
  // LOG(count);
  // LOG(score);
  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
}
