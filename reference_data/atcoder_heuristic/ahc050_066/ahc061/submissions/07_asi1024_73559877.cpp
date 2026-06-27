#ifndef __aarch64__
#pragma GCC target("arch=skylake-avx512")
#pragma GCC optimize("O3","inline")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f,avx512vl,avx512bw,avx512dq")

#include <immintrin.h>
#endif

// #include <atcoder/dsu>
// #include <atcoder/segtree>
// #include <atcoder/mincostflow>

#define FINAL_SUBMIT 1

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
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
constexpr double PI = std::acos(-1.0);

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
  static const int64_t CYCLES_PER_SEC = 1200000000;
  inline int64_t get_cycle() const {
    uint64_t t;
    asm volatile("mrs %0, cntvct_el0" : "=r" (t));
    return t;
  }
#else
  static constexpr int64_t CYCLES_PER_SEC = 2400000000;
  // static constexpr int64_t CYCLES_PER_SEC = 6000000000;
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

Timer timer(1.96);
XorShift rnd;
std::mt19937 rng(12345);

using namespace std;

constexpr int N = 10;
constexpr int NN = N * N;

static inline int get_id(int x, int y) { return x * N + y; }
static inline int ix(int k) { return k / N; }
static inline int iy(int k) { return k % N; }

constexpr int MIN_M = 2, MAX_M = 8;
constexpr int T = 100;
constexpr int MIN_U = 1, MAX_U = 5;

int M, U;
int V[104];
int SX[MAX_M], SY[MAX_M];

// Precompute Manhattan distances on 10x10
int MDIST[NN][NN];

using U128 = unsigned __int128;

static inline U128 to_bit(int id) { return (U128)1 << (id + id / 10); }
static inline int from_bit(U128 bit, int id) { return (bit >> (id + id / 10)) & 1; }

// 11x11 row-major (id = x*11+y)
U128 IN_BOARD_MASK;

// 4-neighbor expansion for a set of cells
static inline U128 adj4(U128 m) {
  return ((m << 1) | (m >> 1) | (m << 11) | (m >> 11)) & IN_BOARD_MASK;
}

/* ---------- Initialization and Input ---------- */

constexpr int NS = 2048;
int seed = 0;
double epsilons[NS];
int order[NS][NN];

void SetUp() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int N_dummy, T_dummy;
  cin >> N_dummy >> M >> T_dummy >> U;
  REP(i,NN) cin >> V[i];
  REP(i,M) cin >> SX[i] >> SY[i];

  LOG(M);
  LOG(U);

  // dist
  REP(a,NN) REP(b,NN) {
    MDIST[a][b] = abs(ix(a) - ix(b)) + abs(iy(a) - iy(b));
  }

  IN_BOARD_MASK = 0;
  REP(i,NN) IN_BOARD_MASK |= to_bit(i);

  REP(i,NS) {
    epsilons[i] = rnd.next(0.0, 1.0);
    REP(k,NN) {
      order[i][k] = k;
      std::swap(order[i][k], order[i][rnd.next() % (k + 1)]); // shuffle
    }
  }
}

/* --------- AI Parameters and Evaluation --------- */

// type: 0=empty,1=own<U,2=own=U,3=enemy1,4=enemy2+
static inline int CellType(int p, int id, const int owner[NN], const int level[NN]) {
  if (owner[id] == -1) return 0;
  if (owner[id] == p) return (level[id] < U ? 1 : 2);
  return (level[id] == 1 ? 3 : 4);
}

struct ParamsAI {
  double wa = 0.65, wb = 0.65, wc = 0.65, wd = 0.65; // init mid
};

// Evaluate Cell
static inline double EvalA(int id, int type, ParamsAI params) {
  switch (type) {
  case 0: return V[id] * params.wa;
  case 1: return V[id] * params.wb;
  case 2: return 0.0;
  case 3: return V[id] * params.wc;
  case 4: return V[id] * params.wd;
  default: assert(false);
  }
}

static inline double ChooseParam(int p, int id, const int owner[NN], const int level[NN], ParamsAI params) {
  int own = owner[id];
  if (own == -1) return params.wa;
  int lev = level[id];
  if (own == p) return (lev < U ? params.wb : 0.0);
  return (lev == 1 ? params.wc : params.wd);
}

/* --------- Estimator for enemy parameters --------- */

struct Estimator {
  struct Statistic {
    double size_inv;
    int chosen_index;

    // (id, type: 0=empty,1=own<U,2=own=U,3=enemy1,4=enemy2+, probability)
    vector<tuple<int, int, double>> cand_vec;
  };

  int turn = 0;
  Statistic stats[T];
  ParamsAI params;
  double eps = (0.1 + 0.5) / 2;
  double logit = -1;

  Estimator() {}

  void Register(int p, int owner[NN], int level[NN], bool* cand, int chosen) {
    assert(turn < T);
    auto& stat = stats[turn++];
    stat.cand_vec.clear();
    stat.cand_vec.reserve(8);
    int size = 0;
    static vector<int> ids[5];
    int max_value[5];
    REP(i,5) {
      ids[i].clear();
      max_value[i] = -1;
    }

    REP(id,NN) {
      if (!cand[id]) continue;
      // if (id == chosen) stat.chosen_index = stat.size;
      int type = CellType(p, id, owner, level);
      int value = V[id];
      if (value > max_value[type]) {
        max_value[type] = value;
        ids[type].clear();
      }
      if (value == max_value[type]) {
        ids[type].push_back(id);
      }
      ++size;
    }
    REP(i,5) {
      // Clear type == 2
      if (i != 2 && !ids[i].empty()) ids[2].clear();
    }

    stat.size_inv = 1.0 / size;
    stat.chosen_index = -1;
    REP(type,5) {
      for (int id : ids[type]) {
        if (id == chosen) stat.chosen_index = stat.cand_vec.size();
        stat.cand_vec.emplace_back(id, type, 1.0 / ids[type].size());
      }
    }
    assert(size > 0);
    assert(!stat.cand_vec.empty());
    assert(stat.chosen_index < int(stat.cand_vec.size()));
    logit = Logit(params, eps);
  }

  // Log-likelihood of observed choices given params.
  inline double Logit(ParamsAI params_, double eps_) {
    assert(0.3 <= params_.wa && params_.wa <= 1.0);
    assert(0.3 <= params_.wb && params_.wb <= 1.0);
    assert(0.3 <= params_.wc && params_.wc <= 1.0);
    assert(0.3 <= params_.wd && params_.wd <= 1.0);
    assert(0.1 <= eps_ && eps_ <= 0.5);

    double ll = 0.0;
    REP(t, turn) {
      const auto& st = stats[t];
      const int ci = st.chosen_index;
      double max_a = -1e100;
      bool chosen_is_max = false;

      if (ci >= 0) {
        int K = st.cand_vec.size();
        REP(i,K) {
          int id = get<0>(st.cand_vec[i]);
          int type = get<1>(st.cand_vec[i]);
          double a = EvalA(id, type, params_);
          if (a > max_a) {
            max_a = a;
            chosen_is_max = (i == ci);
          } else if (a == max_a) {
            if (i == ci) chosen_is_max = true;
          }
        }
      }

      double p = eps_ * st.size_inv;
      if (chosen_is_max) {
        p += (1.0 - eps_) * get<2>(st.cand_vec[ci]);
      }
      ll += std::log(p);
    }
    return ll;
  }

  void Sampling(int num_update = 20) {  // TODO: Param
    double stddev = 0.15 / std::sqrt(std::max(1, turn));  // TODO: Param

    auto rnd_loop = [&](double cur_val, double lb, double ub) {
      std::normal_distribution<double> dist(cur_val, stddev);
      for (;;) {
        double v = dist(rng);
        if (lb < v && v < ub) return v;
      }
    };

    REP(iter, num_update) {
      double wa_new = rnd_loop(params.wa, 0.3, 1.0);
      double wb_new = rnd_loop(params.wb, 0.3, 1.0);
      double wc_new = rnd_loop(params.wc, 0.3, 1.0);
      double wd_new = rnd_loop(params.wd, 0.3, 1.0);
      ParamsAI params_new{wa_new, wb_new, wc_new, wd_new};
      double eps_new = rnd_loop(eps, 0.1, 0.5);

      double ll_new = Logit(params_new, eps_new);

      double accept_prob = std::exp(ll_new - logit);
      if (rnd.next() < accept_prob * (1ULL << 32)) {
        params = params_new;
        eps = eps_new;
        logit = ll_new;
      }
    }
  }
};  // struct Estimator

Estimator estimators[MAX_M];

constexpr int N_ESTIMATE = 63;
pair<ParamsAI, double> estimations[MAX_M][N_ESTIMATE];
int seed2 = 0;

/* ---------- Board simulator ---------- */

struct Simulator {
  int owner[104];   // -1 or player id
  int level[104];   // 0..U
  int pos[MAX_M];  // current piece positions at start of turn
  U128 terr_mask[MAX_M];
  double bonus[104];

  bool reachable_cached[MAX_M];
  bool reachable[MAX_M][104];

  inline void RebuildTerritoryMasks() {
    for (int p = 0; p < M; p++) terr_mask[p] = 0;
    for (int id = 0; id < NN; id++) {
      int o = owner[id];
      if (o >= 0) terr_mask[o] |= to_bit(id);
    }
  }

  Simulator() {}
  void CopyFrom(const Simulator& other) {
    memcpy(owner, other.owner, sizeof(owner));
    memcpy(level, other.level, sizeof(level));
    memcpy(pos, other.pos, sizeof(int) * M);
    memcpy(reachable, other.reachable, sizeof(reachable));
    memcpy(reachable_cached, other.reachable_cached, sizeof(reachable_cached));
    memcpy(terr_mask, other.terr_mask, sizeof(terr_mask));
  }

  inline int ComputeScore(int p) const {
    int s = 0;
    REP(i,NN) s += owner[i] == p ? V[i] * level[i] : 0;
    return s;
  }

  inline double LogScore(int turn) {
    int sum[MAX_M];
    memset(sum, 0, sizeof(sum));
    REP(i,NN) {
      int o = owner[i];
      if (o < 0) continue;
      sum[o] += V[i] * level[i];
    }

    // std::sort(sum + 1, sum + M);
    for (int i = 1; i < M - 1; i++) {
      auto it = std::min_element(sum + i, sum + M);
      std::swap(*it, sum[i]);
    }
    double r = 0.3 * (T - turn) / turn;  // Param
    double s = 0.0;
    for (int i = 1; i < M; i++) {
      s = s * r + sum[i];
    }

    // double ratio = 1.0 * sum[0] / (*std::max_element(sum + 1, sum + M) + 1e-9);
    double ratio = 1.0 * sum[0] / (s + 1e-9);
    double score = std::log2(1.0 + ratio);

    // if (M > 2) {
    //   double mx[MAX_M], my[MAX_M];
    //   for (int p = 1; p < M; p++) {
    //     double sum_x = 0, sum_y = 0;
    //     int count = 0;
    //     ComputeReachable(p);
    //     REP(id,NN) if (reachable[p][id]) {
    //       sum_x += ix(id);
    //       sum_y += iy(id);
    //       count++;
    //     }
    //     assert(count > 0);
    //     mx[p] = sum_x / count;
    //     my[p] = sum_y / count;
    //   }
    //   for (int p = 1; p < M; p++) {
    //     for (int q = p + 1; q < M; q++) {
    //       double dist = std::abs(mx[p] - mx[q]) + std::abs(my[p] - my[q]);
    //       score -= dist * 1e-3 * (T - turn) / (M * (M - 1) / 2);
    //     }
    //   }
    // }
    return score;

    // constexpr double penalty = 0.1;

    // REP(i,N) REP(j,N) {
    //   int id = get_id(i, j);
    //   int o = owner[id];
    //   if (o < 0) continue;
    //   if (i < 9) {
    //     int id2 = get_id(i + 1, j);
    //     int o2 = owner[id2];
    //     if (o2 >= 0 && o2 != o) {
    //       sum[o] -= V[id] * penalty;
    //       sum[o2] -= V[id2] * penalty;
    //     }
    //   }
    //   if (j < 9) {
    //     int id2 = get_id(i, j + 1);
    //     int o2 = owner[id2];
    //     if (o2 >= 0 && o2 != o) {
    //       sum[o] -= V[id] * penalty;
    //       sum[o2] -= V[id2] * penalty;
    //     }
    //   }
    // }
    // double ratio = 1.0 * sum[0] / (*std::max_element(sum + 1, sum + M) + 1e-9);
    // return std::log2(1.0 + ratio);
  }

  inline void Read() {
    // Inputs at end of turn:
    // tx_p ty_p for all p
    // ex_p ey_p for all p
    // owner grid O
    // level grid L
    REP(p,M) {
      int x, y;
      cin >> x >> y;
      assert(0 <= x && x < N);
      assert(0 <= y && y < N);
      int id = get_id(x, y);

      if (p > 0) {
        ComputeReachable(p);
        estimators[p].Register(p, owner, level, reachable[p], id);
      }
      reachable_cached[p] = false;
    }
    REP(p,M) {
      int x, y;
      cin >> x >> y;
      pos[p] = get_id(x, y);
    }
    REP(i,NN) cin >> owner[i];
    REP(i,NN) cin >> level[i];
    RebuildTerritoryMasks();
  }
  
  inline void Init() {
    // Initialize member variables
    REP(id,NN) {
      owner[id] = -1;
      level[id] = 0;
    }
    REP(p,M) {
      int id = get_id(SX[p], SY[p]);
      owner[id] = p;
      level[id] = 1;
      pos[p] = id;
    }
    memset(reachable, 0, sizeof(reachable));
    memset(reachable_cached, 0, sizeof(reachable_cached));
    RebuildTerritoryMasks();
  }

  inline void ComputeReachable(int p) {
    if (reachable_cached[p]) return;
    reachable_cached[p] = true;

    // 1) own territory bitmask
    U128 own = terr_mask[p];

    // 2) flood fill within own territory starting from pos[p]
    U128 reach = to_bit(pos[p]);
    U128 prev = 0;
    while (reach != prev) {
      prev = reach;
      reach |= adj4(reach) & own;
    }
    reach |= to_bit(pos[p]);

    // 3) candidate = reach + 4-neighbors of reach
    U128 c = reach | adj4(reach);

    // 4) cannot move onto other player's piece (at decision time)
    for (int q = 0; q < M; q++) {
      if (q == p) continue;
      c &= ~to_bit(pos[q]);
    }
    // assert(c > 0);

    // 5) export to bool array (NN=100 so this is cheap)
    // for (int id = 0; id < NN; id++) {
    //   reachable[p][id] = from_bit(c, id);
    // }
    memset(reachable[p], 0, sizeof(reachable[p]));
    uint64_t low = (uint64_t)c;
    uint64_t high = (uint64_t)(c >> 64);
    while (low) {
      int id = __builtin_ctzll(low);
      low &= low - 1;
      reachable[p][id - id / 11] = true;
    }
    while (high) {
      int id = __builtin_ctzll(high) + 64;
      high &= high - 1;
      reachable[p][id - id / 11] = true;
    }
  }

  inline int MoveTo(int p, const ParamsAI params) const {
#ifndef __aarch64__
    // ---- constants ----
    constexpr double NEG_INF = -1e100;

    __m512d bestv = _mm512_set1_pd(NEG_INF);
    __m512i besti = _mm512_set1_epi64((long long)pos[p]); // lane-wise best id

    const __m256i v_p     = _mm256_set1_epi32(p);
    const __m256i v_m1    = _mm256_set1_epi32(-1);
    const __m256i v_one   = _mm256_set1_epi32(1);
    const __m256i v_U     = _mm256_set1_epi32(U);

    const __m512d w_wa    = _mm512_set1_pd(params.wa);
    const __m512d w_wb    = _mm512_set1_pd(params.wb);
    const __m512d w_wc    = _mm512_set1_pd(params.wc);
    const __m512d w_wd    = _mm512_set1_pd(params.wd);
    const __m512d w_zero  = _mm512_setzero_pd();
    const __m512d v_neginf= _mm512_set1_pd(NEG_INF);

    __m512i idx = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
    __m512i incr = _mm512_set1_epi64(8);

    for (int id = 0; id < NN; id += 8, idx = _mm512_add_epi64(idx, incr)) {
      unsigned mask_bits = 0;
      mask_bits |= (unsigned)reachable[p][id + 0] << 0;
      mask_bits |= (unsigned)reachable[p][id + 1] << 1;
      mask_bits |= (unsigned)reachable[p][id + 2] << 2;
      mask_bits |= (unsigned)reachable[p][id + 3] << 3;
      mask_bits |= (unsigned)reachable[p][id + 4] << 4;
      mask_bits |= (unsigned)reachable[p][id + 5] << 5;
      mask_bits |= (unsigned)reachable[p][id + 6] << 6;
      mask_bits |= (unsigned)reachable[p][id + 7] << 7;
      __mmask8 m_reach = (__mmask8)mask_bits;
      if (!m_reach) continue;

      // load 8x int
      __m256i own32 = _mm256_loadu_si256((const __m256i*)(owner + id));
      __m256i lvl32 = _mm256_loadu_si256((const __m256i*)(level + id));
      __m256i v32   = _mm256_loadu_si256((const __m256i*)(V + id));

      // V -> double (8 lanes)
      __m512d vv = _mm512_cvtepi32_pd(v32);

      // masks
      __mmask8 m_empty = _mm256_cmpeq_epi32_mask(own32, v_m1);
      __mmask8 m_own   = _mm256_cmpeq_epi32_mask(own32, v_p);

      __mmask8 m_own_ltU = m_own & _mm256_cmplt_epi32_mask(lvl32, v_U);
      __mmask8 m_own_eqU = m_own & (~m_own_ltU);

      __mmask8 m_enemy = (~m_empty) & (~m_own);
      __mmask8 m_enemy_l1  = m_enemy & _mm256_cmpeq_epi32_mask(lvl32, v_one);
      __mmask8 m_enemy_l2p = m_enemy & (~m_enemy_l1);

      // weight select: start wa then overwrite by masks
      __m512d w = w_wa;
      w = _mm512_mask_mov_pd(w, m_own_ltU, w_wb);
      w = _mm512_mask_mov_pd(w, m_own_eqU, w_zero);
      w = _mm512_mask_mov_pd(w, m_enemy_l1, w_wc);
      w = _mm512_mask_mov_pd(w, m_enemy_l2p, w_wd);

      // a = w * V
      __m512d a = _mm512_mul_pd(w, vv);

      // reachable 以外は -INF
      a = _mm512_mask_mov_pd(v_neginf, m_reach, a);

      // update best lanes
      __mmask8 m_better = _mm512_cmp_pd_mask(a, bestv, _CMP_GT_OQ);
      bestv = _mm512_mask_mov_pd(bestv, m_better, a);
      besti = _mm512_mask_mov_epi64(besti, m_better, idx);
    }

    double best = _mm512_reduce_max_pd(bestv);
    __mmask8 m_eq = _mm512_cmp_pd_mask(bestv, _mm512_set1_pd(best), _CMP_EQ_OQ);
    int lane = __builtin_ctz((unsigned)m_eq);

    alignas(64) long long bi[8];
    _mm512_store_si512((__m512i*)bi, besti);

    int best_id = (int)bi[lane];
    return best_id;

#else
    double best = -1e100;
    int best_id = pos[p];
    for (int id = 0; id < NN; id++) if (reachable[p][id]) {
        double a = ChooseParam(p, id, owner, level, params) * V[id];
        if (a > best) { best = a; best_id = id; }
      }
    return best_id;
#endif
  }

inline int MoveToWithTarget(const ParamsAI& params,
                            int target_id,        // = target cell id (0..99)
                            int attack_target_id  // bitmask over players
                            ) const {
#ifdef __aarch64__
  // ARM: scalar fallback
  double best = -1e100;
  int best_id = pos[0];

  const int dist_coeff = (owner[target_id] == 0 ? 0 : 1000);
  for (int id = 0; id < NN; id++) if (reachable[0][id]) {
      double w = ChooseParam(0, id, owner, level, params);
      double a = w * V[id] - (double)MDIST[target_id][id] * (double)dist_coeff;
      int own = owner[id];
      if (own > 0 && ((attack_target_id >> own) & 1)) a += 1000.0;
      a += bonus[id];
      if (a > best) { best = a; best_id = id; }
    }
  return best_id;

#else
  constexpr double NEG_INF = -1e100;

  __m512d bestv = _mm512_set1_pd(NEG_INF);
  __m512i besti = _mm512_set1_epi64((long long)pos[0]);

  const __m256i v_p   = _mm256_set1_epi32(0);
  const __m256i v_m1  = _mm256_set1_epi32(-1);
  const __m256i v_one = _mm256_set1_epi32(1);
  const __m256i v_U   = _mm256_set1_epi32(U);
  const __m256i v_z   = _mm256_setzero_si256();

  const __m512d w_wa   = _mm512_set1_pd(params.wa);
  const __m512d w_wb   = _mm512_set1_pd(params.wb);
  const __m512d w_wc   = _mm512_set1_pd(params.wc);
  const __m512d w_wd   = _mm512_set1_pd(params.wd);
  const __m512d w_zero = _mm512_setzero_pd();

  const int dist_coeff_i = (owner[target_id] == 0 ? 0 : 1000);
  const __m512d v_distc  = _mm512_set1_pd((double)dist_coeff_i);
  const __m512d v_neginf = _mm512_set1_pd(NEG_INF);
  const __m512d v_1000   = _mm512_set1_pd(1000.0);

  __m512i idx  = _mm512_set_epi64(7, 6, 5, 4, 3, 2, 1, 0);
  const __m512i incr = _mm512_set1_epi64(8);

  // MDIST row pointer (contiguous in id)
  const int* dist_row = MDIST[target_id];

  for (int base = 0; base < NN; base += 8, idx = _mm512_add_epi64(idx, incr)) {
    // reachable mask (scalar gather is fine; only 8 lanes)
    unsigned reach_bits = 0;
    reach_bits |= (unsigned)reachable[0][base + 0] << 0;
    reach_bits |= (unsigned)reachable[0][base + 1] << 1;
    reach_bits |= (unsigned)reachable[0][base + 2] << 2;
    reach_bits |= (unsigned)reachable[0][base + 3] << 3;
    reach_bits |= (unsigned)reachable[0][base + 4] << 4;
    reach_bits |= (unsigned)reachable[0][base + 5] << 5;
    reach_bits |= (unsigned)reachable[0][base + 6] << 6;
    reach_bits |= (unsigned)reachable[0][base + 7] << 7;
    __mmask8 m_reach = (__mmask8)reach_bits;
    if (!m_reach) continue;

    // load owner/level/V (8x int32)
    __m256i own32 = _mm256_loadu_si256((const __m256i*)(owner + base));
    __m256i lvl32 = _mm256_loadu_si256((const __m256i*)(level + base));
    __m256i v32   = _mm256_loadu_si256((const __m256i*)(V + base));

    // V -> double
    __m512d vv = _mm512_cvtepi32_pd(v32);

    // dist -> double
    __m256i d32 = _mm256_loadu_si256((const __m256i*)(dist_row + base));
    __m512d dd  = _mm512_cvtepi32_pd(d32);

    // bonus (double)
    __m512d bb = _mm512_loadu_pd(bonus + base);

    // ---- ChooseParam(0, id, ...) をベクトル化（MoveToと同じ） ----
    __mmask8 m_empty = _mm256_cmpeq_epi32_mask(own32, v_m1);
    __mmask8 m_own   = _mm256_cmpeq_epi32_mask(own32, v_p);

    __mmask8 m_own_ltU = m_own & _mm256_cmplt_epi32_mask(lvl32, v_U);
    __mmask8 m_own_eqU = m_own & (~m_own_ltU);

    __mmask8 m_enemy = (~m_empty) & (~m_own);
    __mmask8 m_enemy_l1  = m_enemy & _mm256_cmpeq_epi32_mask(lvl32, v_one);
    __mmask8 m_enemy_l2p = m_enemy & (~m_enemy_l1);

    // weight select
    __m512d w = w_wa;
    w = _mm512_mask_mov_pd(w, m_own_ltU, w_wb);
    w = _mm512_mask_mov_pd(w, m_own_eqU, w_zero);
    w = _mm512_mask_mov_pd(w, m_enemy_l1, w_wc);
    w = _mm512_mask_mov_pd(w, m_enemy_l2p, w_wd);

    // a = w*V + bonus
    __m512d a = _mm512_fmadd_pd(w, vv, bb);

    // a -= dist * dist_coeff
    // (dist_coeff==0 のときは v_distc が 0 なので自然に消える)
    a = _mm512_fnmadd_pd(dd, v_distc, a);

    // attack bonus: own>0 && bit(own) in attack_target_id
    // 8 lanesだけなのでmaskはscalarで作るのが速い/簡単
    alignas(32) int own_arr[8];
    _mm256_store_si256((__m256i*)own_arr, own32);
    unsigned atk_bits = 0;
    if (dist_coeff_i || attack_target_id) { // tiny branch hint (optional)
      for (int k = 0; k < 8; k++) {
        int o = own_arr[k];
        if (o > 0 && ((attack_target_id >> o) & 1)) atk_bits |= (1u << k);
      }
    }
    __mmask8 m_attack = (__mmask8)atk_bits;
    a = _mm512_mask_add_pd(a, m_attack, a, v_1000);

    // unreachable -> -INF
    a = _mm512_mask_mov_pd(v_neginf, m_reach, a);

    // update best
    __mmask8 m_better = _mm512_cmp_pd_mask(a, bestv, _CMP_GT_OQ);
    bestv = _mm512_mask_mov_pd(bestv, m_better, a);
    besti = _mm512_mask_mov_epi64(besti, m_better, idx);
  }

  // reduce bestv/besti
  double best = _mm512_reduce_max_pd(bestv);
  __mmask8 m_eq = _mm512_cmp_pd_mask(bestv, _mm512_set1_pd(best), _CMP_EQ_OQ);
  int lane = __builtin_ctz((unsigned)m_eq);

  alignas(64) long long bi[8];
  _mm512_store_si512((__m512i*)bi, besti);
  return (int)bi[lane];
#endif
}
};

/* ---------- Main solver logic ---------- */

int g_sims = 0;

struct Solver {
  Simulator sim;
  ParamsAI my;
  int my_target_pid = 0;        // target coordinate (cell id) for rollout policy
  int my_attack_target_id = 0;  // NEW: attack target id (0=disabled, >=1=player id)

  void Init() {
    sim.Init();
    my_target_pid = sim.pos[0];
    my_attack_target_id = 0;
  }
  inline void Read() { sim.Read(); }

  inline pair<int, int> MoveAI(int p, Simulator& st) const {
    const auto& e = estimations[p][seed2];

    int greedy = st.MoveTo(p, e.first);
    if (epsilons[seed] < e.second) {
      REP(i,NN) {
        int id = order[seed][i];
        if (st.reachable[p][id]) {
          seed = (seed + 1) % NS;
          return {id, greedy};
        }
      }
      assert(false);
    }
    seed = (seed + 1) % NS;
    return {greedy, greedy};
  }

  // apply one full turn (simultaneous moves, conflict, update, restore)
  inline void SimulateOneTurn(Simulator& st, auto my_move_fn) const {
    int start_pos[MAX_M];
    memcpy(start_pos, st.pos, sizeof(start_pos));

    for (int p = 1; p < M; p++) {
      st.ComputeReachable(p);
    }

    int dest[MAX_M];
    memset(st.bonus, 0, sizeof(st.bonus));
    // decide moves simultaneously
    for (int p = 1; p < M; ++p) {
      auto [d, d_greedy] = MoveAI(p, st);
      dest[p] = d;
      assert(0 <= d && d < NN);
      assert(0 <= d_greedy && d_greedy < NN);
      if (st.owner[d_greedy] == 0) {
        // my area
        int lev = st.level[d_greedy];
        if (lev == U) continue;
        st.bonus[d_greedy] += (3000 + V[d_greedy]) * (1 - estimations[p][seed2].second);
      } else if (st.owner[d_greedy] == p || (M >= 3 && st.owner[d_greedy] != 0)) {
        st.reachable[0][d_greedy] = false;
      }
    }

    dest[0] = my_move_fn();
    assert(0 <= dest[0] && dest[0] < NN);

    // conflict resolution
    bool alive[MAX_M];
    REP(p,M) {
      alive[p] = true;
      REP(q,p) {
        if (dest[p] == dest[q]) {
          int own = st.owner[dest[p]];
          if (own != p) alive[p] = false;
          if (own != q) alive[q] = false;
        }
      }
    }

    // move (temporarily), removed -> restored
    REP(p,M) {
      if (alive[p]) {
        st.pos[p] = dest[p];
      } else {
        st.pos[p] = start_pos[p];
        st.reachable_cached[p] = false;
      }
    }

    auto add_owner = [&](int id, int p) {
      st.owner[id] = p;
      st.terr_mask[p] |= to_bit(id);
      // if (st.reachable_cached[p]) {
      //   if (id / N > 0) st.reachable[p][id - N] = true;
      //   if (id % N > 0) st.reachable[p][id - 1] = true;
      //   if (id % N < N - 1) st.reachable[p][id + 1] = true;
      //   if (id / N < N - 1) st.reachable[p][id + N] = true;
      // }
    };

    auto remove_owner = [&](int id, int p) {
      st.terr_mask[p] &= ~to_bit(id);
      st.reachable_cached[p] = false;
      // if (st.reachable_cached[p]) {
      //   int x1 = id / N > 0 && id % N > 0 && st.owner[id - N - 1] == p;
      //   int x2 = id / N > 0 && st.owner[id - N] == p;
      //   int x3 = id / N > 0 && id % N < N - 1 && st.owner[id - N + 1] == p;
      //   int x4 = id % N > 0 && st.owner[id - 1] == p;
      //   // int x5 = st.owner[id] == p;
      //   int x6 = id % N < N - 1 && st.owner[id + 1] == p;
      //   int x7 = id / N < N - 1 && id % N > 0 && st.owner[id + N - 1] == p;
      //   int x8 = id / N < N - 1 && st.owner[id + N] == p;
      //   int x9 = id / N < N - 1 && id % N < N - 1 && st.owner[id + N + 1] == p;
      //   int s = x2 + x4 + x6 + x8;
      //   int s2 = s;
      //   s2 -= x1 * x2 * x4;
      //   s2 -= x2 * x3 * x6;
      //   s2 -= x4 * x7 * x8;
      //   s2 -= x6 * x8 * x9;
      //   if (s2 == 1) st.reachable[p][id] = false;
      //   else st.reachable_cached[p] = false;
      // }
    };

    // territory update (note: attack can additionally remove attacker)
    for (int p=0;p<M;p++) if (alive[p]) {
      int to = dest[p];
      int o = st.owner[to];
      int l = st.level[to];
      if (o == -1) {
        add_owner(to, p);
        st.level[to] = 1;
      } else if (o == p) {
        if (l < U) st.level[to] = l + 1;
      } else {
        // attack
        st.level[to] = l - 1;
        if (st.level[to] == 0) {
          remove_owner(to, o);
          add_owner(to, p);
          st.level[to] = 1;
        } else {
          // attack failed: attacker removed and restored
          alive[p] = false;
          st.pos[p] = start_pos[p];
          st.reachable_cached[p] = false;
        }
      }
    }

    REP(p,M) st.reachable_cached[p] = false;
  }

  inline double MonteCarloEvaluateOne(Simulator& st, const ParamsAI& prm,
                                      int target_pid, int attack_target_id, int turn_now, int horizon) const {
    int rem = T - turn_now;
    int H = std::min(horizon, rem);
    if (H <= 0) return st.LogScore(turn_now);

    for (int h=0; h<H; h++) {
      st.ComputeReachable(0);
      SimulateOneTurn(st, [&]{ return st.MoveToWithTarget(prm, target_pid, attack_target_id); });
    }
    return st.LogScore(turn_now + H);
  }

  inline double MonteCarloEvaluate(Simulator& st_from, const ParamsAI& prm,
                                  int target_pid, int attack_target_id, int turn_now, int horizon, int rollouts) const {
    double acc = 0.0;
    int rem = T - turn_now;
    int H = std::min(horizon, rem);
    if (H <= 0) {
      return st_from.LogScore(turn_now);
    }

    for (int r=0;r<rollouts;r++) {
      Simulator st;
      st.CopyFrom(st_from);

      for (int h=0; h<H; h++) {
        st.ComputeReachable(0);
        seed = (turn_now * 197 + r * 123) % NS;
        seed2 = (40 + turn_now + r) % N_ESTIMATE;
        SimulateOneTurn(st, [&]{ return st.MoveToWithTarget(prm, target_pid, attack_target_id); });
      }
      acc += st.LogScore(turn_now + H);
    }
    return acc / rollouts;
  }

  struct Cand {
    ParamsAI p;
    int target;
    int attack_target_id; // NEW
    int n = 0;
    double sum = 0.0;
    double mean() const { return n ? sum / n : -1e100; }
  };

  inline void TuneParams(int turn_now) {
    constexpr int Hstart = 22, Hend = 12;
    const int H = Hstart + (Hend - Hstart) * turn_now / T;  // horizon
    constexpr int K1 = 30;
    constexpr int R1 = 10;
    constexpr int K2 = 5;
    constexpr int R2 = 20;

    vector<Cand> cand(K1);

    REP(i,K1) {
      Cand c;
      c.p.wa = rnd.next() % 4;
      c.p.wb = rnd.next() % 4;
      c.p.wc = rnd.next() % 4;
      c.p.wd = rnd.next() % 4;
      c.target = (rnd.next() % 200 > turn_now + 100) ? rnd.next(NN) : my_target_pid;

      int at = my_attack_target_id;
      REP(i,M) {
        int sw = rnd.next() % 100 < 30;
        if (sw) at ^= (1 << i);
      }

      if (U < 2) at = 0;
      c.attack_target_id = at;

      cand[i] = c;
    }

    cand[0].p = my;
    cand[0].target = my_target_pid;
    cand[0].attack_target_id = my_attack_target_id;

    auto rollout_one = [&](const ParamsAI& p, int target, int attack_target_id) -> double {
      Simulator st;
      st.CopyFrom(sim);
      st.ComputeReachable(0);
      SimulateOneTurn(st, [&]{ return st.MoveToWithTarget(p, target, attack_target_id); });
      return MonteCarloEvaluateOne(st, p, target, attack_target_id, turn_now + 1, H);
    };

    auto add_rollouts = [&](Cand& c, int R) {
      REP(r,R) {
        seed  = (turn_now * 197 + c.n * 19) % NS;
        seed2 = (turn_now + c.n) % N_ESTIMATE;
        c.sum += rollout_one(c.p, c.target, c.attack_target_id);
        c.n++;
      }
    };

    REP(i,K1) add_rollouts(cand[i], R1);

    nth_element(cand.begin(), cand.begin() + K2, cand.end(),
                [](const Cand& a, const Cand& b){ return a.mean() > b.mean(); });
    cand.resize(K2);

    REP(i,K2) add_rollouts(cand[i], R2);

    int best_i = 0;
    REP(i,K2) if (cand[i].mean() > cand[best_i].mean()) best_i = i;
    my = cand[best_i].p;
    my_target_pid = cand[best_i].target;
    my_attack_target_id = cand[best_i].attack_target_id; // NEW
    // CERR() << "#" << turn_now << ": " << my_attack_target_id << " -> (" << my_target_pid / N << "," << my_target_pid % N << ")" << endl;
  }

  struct BanditArm {
    int move_id;     // cell id
    int n = 0;
    double sum = 0.0;
    double sumsq = 0.0; // for variance
    double mean() const { return (n ? sum / n : 0.0); }
    double var() const {
      if (n <= 1) return 0.25; // pessimistic prior (max variance in [0,1])
      double m = sum / n;
      double v = sumsq / n - m * m;
      if (v < 0) v = 0;
      return v;
    }
    void add(double x) { n++; sum += x; sumsq += x * x; }
  };

  int StepMyGreedyWithParams(int turn_now) {
    TuneParams(turn_now);

    sim.ComputeReachable(0);

    // collect arms
    BanditArm arms[NN];
    int K = 0;
    for (int i=0;i<NN;i++) if (sim.reachable[0][i]) {
      if (sim.owner[i] == 0 && sim.level[i] == U) continue; // same as your skip
      arms[K].move_id = i;
      K++;
    }
    if (K == 0) return sim.pos[0];

    // ---- bandit knobs ----
    constexpr int Hstart = 20, Hend = 10;
    const int H = Hstart + (Hend - Hstart) * turn_now / T;  // horizon
    constexpr int INIT_PER_ARM = 4;     // initial pulls for each arm (>=1 recommended)

    int t_total = 0;

    // initial pulls
    for (int a=0;a<K;a++) {
      for (int k=0;k<INIT_PER_ARM;k++) {
        Simulator st;
        st.CopyFrom(sim);
        // assert(arms[a].n == k);
        seed = (arms[a].n * 123) % NS;
        seed2 = arms[a].n % N_ESTIMATE;
        SimulateOneTurn(st, [&]{ return arms[a].move_id; });
        double rew = MonteCarloEvaluateOne(st, my, my_target_pid, my_attack_target_id, turn_now + 1, H);
        arms[a].add(rew);
        t_total++;
      }
    }

    // main loop
    int64_t start = timer.get_cycle();
    int64_t stop = timer.start + int64_t(1.98 * pow((turn_now + 1.0) / T, 1.2) * Timer::CYCLES_PER_SEC);
    int64_t mid = start + (stop - start) * 0.5;
    timer.stop = mid;
    // while (t_total < BUDGET) {
    int max_tried = 0;
    int max_trials = 0;
    while (timer.within()) {
      REP(best_a,K) {
        ++g_sims;
        Simulator st;
        st.CopyFrom(sim);
        seed = (arms[best_a].n * 123) % NS;
        seed2 = arms[best_a].n % N_ESTIMATE;
        SimulateOneTurn(st, [&]{ return arms[best_a].move_id; });
        double rew = MonteCarloEvaluateOne(st, my, my_target_pid, my_attack_target_id, turn_now + 1, H);
        arms[best_a].add(rew);
        t_total++;
      }
    }

    std::sort(arms, arms + K, [](const BanditArm& a, const BanditArm& b) {
      return a.mean() > b.mean();
    });
    if (K > 10) K = 10;

    timer.stop = stop;
    while (timer.within()) {
      REP(best_a,K) {
        ++g_sims;
        Simulator st;
        st.CopyFrom(sim);
        seed = (arms[best_a].n * 123) % NS;
        seed2 = arms[best_a].n % N_ESTIMATE;
        SimulateOneTurn(st, [&]{ return arms[best_a].move_id; });
        double rew = MonteCarloEvaluateOne(st, my, my_target_pid, my_attack_target_id, turn_now + 1, H);
        arms[best_a].add(rew);
        t_total++;
        if (arms[best_a].n > max_trials) {
          max_trials = arms[best_a].n;
          max_tried = best_a;
        }
      }
    }

    // choose by number of trials
    // std::sort(arms, arms + K, [](const BanditArm& a, const BanditArm& b) { return a.n > b.n; });
    // std::sort(arms, arms + std::min(2, K), [](const BanditArm& a, const BanditArm& b) { return a.mean() > b.mean(); });
    int best_move = arms[0].move_id;
    double best_mean = arms[0].mean();
    REP(a,K) {
      double m = arms[a].mean();
      // CERR() << "(" << arms[a].n << " " << arms[a].var() << " " << m << ") ";
      if (m > best_mean) { best_mean = m; best_move = arms[a].move_id; }
    }
    // CERR() << "\n";
    return best_move;
  }
};

int main() {
  SetUp();
  Solver solver;
  solver.Init();
  // int64_t est_cycles = 0;
  REP(t,T) {
    // int64_t start_cycle = timer.get_cycle();
    for (int i = 1; i < M; i++) {
      Estimator& e = estimators[i];
      e.Sampling(30);
      REP(j,N_ESTIMATE) {
        e.Sampling(1);
        estimations[i][j * 5 % N_ESTIMATE] = {e.params, e.eps};
      }
    }
    // int64_t end_cycle = timer.get_cycle();
    // est_cycles += (end_cycle - start_cycle);

    int res = solver.StepMyGreedyWithParams(t);
    // int res = solver.StepGreedy();
    cout << ix(res) << " " << iy(res) << "\n";
    cout.flush();
    solver.Read();

    // cerr << "turn " << t << "\n";
    // for (int i = 1; i < M; i++) {
    //   cerr << "- player " << i << "\n";
    //   const ParamsAI& params = estimators[i].params;
    //   cerr << fixed << setprecision(6)
    //        << params.wa << " "
    //        << params.wb << " "
    //        << params.wc << " "
    //        << params.wd << " "
    //        << estimators[i].eps << " "
    //        << estimators[i].logit << "\n";
    // }
    // cerr.flush();

    // if (t == 30) {
    //   const ParamsAI& prm30 = solver.my;
    //   LOG(prm30.wa);
    //   LOG(prm30.wb);
    //   LOG(prm30.wc);
    //   LOG(prm30.wd);
    // }
    // if (t == 60) {
    //   const ParamsAI& prm60 = solver.my;
    //   LOG(prm60.wa);
    //   LOG(prm60.wb);
    //   LOG(prm60.wc);
    //   LOG(prm60.wd);
    // }
  }

  // double est_time = 1.0 * est_cycles / Timer::CYCLES_PER_SEC;
  // LOG(est_time);

  // Statistics
  // const ParamsAI& prm_fin = solver.my;
  // LOG(prm_fin.wa);
  // LOG(prm_fin.wb);
  // LOG(prm_fin.wc);
  // LOG(prm_fin.wd);
  LOG(g_sims);

  // const Simulator& sim = solver.sim;
  // int player = sim.ComputeScore(0);
  // LOG(player);
  // int enemy = 0;
  // for (int p = 1; p < M; p++) {
  //   enemy = std::max(enemy, sim.ComputeScore(p));
  // }
  // LOG(enemy);
  // double ratio = 1.0 * player / enemy;
  // LOG(ratio);
  // int score = std::round(1e5 * std::log2(1 + ratio));
  // LOG(score);
  double elapsed = timer.elapsed();
  LOG(elapsed);
}
