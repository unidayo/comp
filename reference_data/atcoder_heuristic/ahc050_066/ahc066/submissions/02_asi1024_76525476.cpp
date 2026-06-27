#pragma GCC optimize("O3","inline")
#if defined(__x86_64__) || defined(__i386__)
#pragma GCC target("arch=skylake-avx512")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f")
#endif

// #include <atcoder/dsu>
// #include <atcoder/mincostflow>

#if defined(__x86_64__) || defined(__i386__)
#include <immintrin.h>
#endif

#define FINAL_SUBMIT 1

constexpr int TWO_F_INSERT_TRIALS = 1;

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
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
  static const int64_t CYCLES_PER_SEC = 600000000;
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

Timer timer(1.96);
XorShift rnd;

template <int UPDATE_INTERVAL>
class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static double START_TEMP = 3.0;
  constexpr static double END_TEMP = 0.003;
  const int64_t CYCLES;
  const double TEMP_RATIO;
  double logp[LOG_SIZE];
  long long iteration = 0;
  int64_t cycle = 0;
  int64_t start = timer.get_cycle();
  const int64_t stop;
  double temp = START_TEMP;

  SimulatedAnnealing(int64_t n_cycles) :
      CYCLES(n_cycles),
      TEMP_RATIO(std::log(END_TEMP / START_TEMP) / CYCLES),
      stop(start + CYCLES)
  {
    constexpr double inv = 1.0 / LOG_SIZE;
    REP(i, LOG_SIZE) logp[i] = std::log((i + 0.5) * inv);
    std::mt19937 rng;
    std::shuffle(logp, logp + LOG_SIZE, rng);
  }

  inline bool End() {
    iteration++;
    if ((iteration & UPDATE_INTERVAL) != 0) return false;
    cycle = timer.get_cycle();
    temp = START_TEMP * std::exp((cycle - start) * TEMP_RATIO);
    return cycle >= stop;
  }

  inline bool Accept(double diff) {
    static unsigned short index = 0;
    return diff >= 0 || diff > logp[index++] * temp;
  }
};

using namespace std;

constexpr int MIN_N = 10;
constexpr int MAX_N = 20;
constexpr int MIN_M = MIN_N / 2;
constexpr int MAX_M = 2 * MAX_N;
constexpr int MAX_T = 2 * MAX_N * MAX_N * MAX_M;
using DistValue = int16_t;
constexpr int DIST_INF = numeric_limits<DistValue>::max();
static_assert(MAX_T < DIST_INF);
using Label = uint32_t;
constexpr Label LABEL_INF = 0x7fffffffu;

inline Label PackLabel(int cost, int basic) {
  return (Label(cost) << 16) | Label(basic);
}

inline int LabelCost(Label label) {
  return (int)(label >> 16);
}

inline int LabelBasic(Label label) {
  return (int)(label & 0xffffu);
}

struct Op {
  enum Value : int {
    F = 0,
    L = 1,
    R = 2,
    P = 3,
    S = 4,
    M = 5,
  };

  Value value;

  Op(Value value) : value(value) {}

  int Index() const {
    return static_cast<int>(value);
  }

  char Char() const {
    if (value == F) return 'F';
    if (value == L) return 'L';
    if (value == R) return 'R';
    if (value == P) return 'P';
    if (value == S) return 'S';
    return 'M';
  }
};

struct Node {
  // 0: F
  // 1: L
  // 2: R
  // 3: S
  int dest[4];
};

using Graph = vector<Node>;

int N, M, T;
int wall_count;
Graph init_graph;
array<int, MAX_M> Ball, Bascket;
array<int, MAX_N * MAX_N> CellRank, RankCell;

constexpr int DX[4] = {1, 0, -1, 0};
constexpr int DY[4] = {0, 1, 0, -1};

int NodeIdByCell(int cell, int dir) {
  return (CellRank[cell] << 2) | dir;
}

int CellOfSpatial(int spatial) {
  return RankCell[spatial >> 2];
}

int NodeId(int y, int x, int dir) {
  return NodeIdByCell(y * N + x, dir);
}

void SetUp() {
  cin >> N >> M >> T;
  vector<string> v_wall(N), h_wall(N - 1);
  REP(i, N) cin >> v_wall[i];
  REP(i, N - 1) cin >> h_wall[i];
  vector<int> wall_parent;
  vector<int> endpoint_first_wall((N + 1) * (N + 1), -1);
  auto RootWall = [&](int v) {
    while (wall_parent[v] != v) {
      wall_parent[v] = wall_parent[wall_parent[v]];
      v = wall_parent[v];
    }
    return v;
  };
  auto UniteWall = [&](int a, int b) {
    a = RootWall(a);
    b = RootWall(b);
    if (a != b) wall_parent[b] = a;
  };
  auto AddWallEndpoint = [&](int wall_id, int y, int x) {
    int endpoint = y * (N + 1) + x;
    if (endpoint_first_wall[endpoint] < 0) {
      endpoint_first_wall[endpoint] = wall_id;
    } else {
      UniteWall(wall_id, endpoint_first_wall[endpoint]);
    }
  };
  auto AddWall = [&](int y0, int x0, int y1, int x1) {
    int wall_id = wall_parent.size();
    wall_parent.push_back(wall_id);
    AddWallEndpoint(wall_id, y0, x0);
    AddWallEndpoint(wall_id, y1, x1);
  };
  REP(y, N) REP(x, N - 1) {
    if (v_wall[y][x] == '1') AddWall(y, x + 1, y + 1, x + 1);
  }
  REP(y, N - 1) REP(x, N) {
    if (h_wall[y][x] == '1') AddWall(y + 1, x, y + 1, x + 1);
  }
  wall_count = 0;
  vector<char> counted_wall(wall_parent.size(), 0);
  REP(i, (int)wall_parent.size()) {
    int root = RootWall(i);
    if (!counted_wall[root]) {
      counted_wall[root] = 1;
      wall_count++;
    }
  }
  REP(k, M) {
    int ball_y, ball_x, bascket_y, bascket_x;
    cin >> ball_y >> ball_x >> bascket_y >> bascket_x;
    Ball[k] = ball_y * N + ball_x;
    Bascket[k] = bascket_y * N + bascket_x;
  }

  auto can_move = [&](int y, int x, int dir) {
    int ny = y + DY[dir];
    int nx = x + DX[dir];
    if (ny < 0 || ny >= N || nx < 0 || nx >= N) return false;
    if (dir == 0) return v_wall[y][x] == '0';
    if (dir == 1) return h_wall[y][x] == '0';
    if (dir == 2) return v_wall[y][x - 1] == '0';
    return h_wall[y - 1][x] == '0';
  };

  CellRank.fill(-1);
  RankCell.fill(-1);
  queue<int> cell_queue;
  int cell_rank_count = 0;
  auto PushCell = [&](int cell) {
    if (CellRank[cell] != -1) return;
    CellRank[cell] = cell_rank_count;
    RankCell[cell_rank_count] = cell;
    cell_rank_count++;
    cell_queue.push(cell);
  };
  PushCell(0);
  while (!cell_queue.empty()) {
    int cell = cell_queue.front();
    cell_queue.pop();
    int y = cell / N;
    int x = cell % N;
    REP(dir, 4) {
      if (!can_move(y, x, dir)) continue;
      PushCell((y + DY[dir]) * N + x + DX[dir]);
    }
  }
  REP(cell, N * N) PushCell(cell);

  init_graph.assign(N * N * 4, Node{});
  REP(y, N) REP(x, N) REP(dir, 4) {
    int id = NodeId(y, x, dir);
    init_graph[id].dest[Op(Op::F).Index()] = id;
    init_graph[id].dest[Op(Op::L).Index()] = NodeId(y, x, (dir + 3) & 3);
    init_graph[id].dest[Op(Op::R).Index()] = NodeId(y, x, (dir + 1) & 3);
    init_graph[id].dest[Op(Op::P).Index()] = id;
    if (can_move(y, x, dir)) {
      init_graph[id].dest[Op(Op::F).Index()] = NodeId(y + DY[dir], x + DX[dir], dir);
    }
  }

  LOG(N);
  LOG(M);
  LOG(T);
  LOG(wall_count);
}

constexpr int MAX_MACROS = 8;
constexpr int MAX_MACRO_STATES = MAX_MACROS + 1;
constexpr int BASIC_ACTIONS = 4;
constexpr int MAX_ACTIONS = BASIC_ACTIONS + MAX_MACROS;

int BasicEdgeIndex(Op op) {
  if (op.value == Op::F) return 0;
  if (op.value == Op::L) return 1;
  if (op.value == Op::R) return 2;
  assert(op.value == Op::S);
  return 3;
}

Op EdgeOp(int edge) {
  if (edge == 0) return Op::F;
  if (edge == 1) return Op::L;
  if (edge == 2) return Op::R;
  assert(edge == 3);
  return Op::S;
}

vector<Op> Ops(const string& s) {
  vector<Op> ret;
  for (char c : s) {
    if (c == 'F') ret.emplace_back(Op::F);
    if (c == 'L') ret.emplace_back(Op::L);
    if (c == 'R') ret.emplace_back(Op::R);
    if (c == 'S') ret.emplace_back(Op::S);
  }
  return ret;
}

template <bool USE_BASKET_TO_BALL>
vector<vector<Op>> GetMacros() {
  constexpr int DELIVERY_COST_WEIGHT = 10;
  constexpr int MATCHING_COST_WEIGHT = 10;
  constexpr int MAX_DEPTH = 32;
  constexpr int LAYER_WIDTH = 40;
  const int take_per_length = (N * N * M <= 4536 ? 2 : 3);
  constexpr double SEARCH_SECONDS = 0.85;
  constexpr int MAX_SPATIAL = MAX_N * MAX_N * 4;
  constexpr int MIN_OUTPUT_MACRO_LENGTH = 5;
  constexpr bool USE_ROTATE_TRANSITIONS = true;
  const double input_time_density = double(T) / double(N * N * M);
  const int matching_cost_weight = input_time_density <= 0.42292 ? 8 : MATCHING_COST_WEIGHT;
  const int two_f_insert_trials = M > 25 ? 0 : TWO_F_INSERT_TRIALS;
  const bool use_rotate_transitions = USE_ROTATE_TRANSITIONS && M > 13 && input_time_density > 1.448199446;
  // LOG(matching_cost_weight);
  // LOG(two_f_insert_trials);
  // LOG(use_rotate_transitions);
  const int spatial_count = N * N * 4;
  const int64_t start_cycle = timer.get_cycle();
  const int64_t stop_cycle = min(timer.stop, start_cycle + (int64_t)(Timer::CYCLES_PER_SEC * SEARCH_SECONDS));

  using Mask = uint64_t;
  using NodeT = uint16_t;
  using DestArray = array<NodeT, MAX_SPATIAL>;
  using MacroKey = uint64_t;
  // A candidate macro is encoded as a ternary key over {F, L, R}.
  // dest[node] stores the spatial node reached after executing that macro.
  struct Candidate {
    MacroKey key = 0;
    int cost = INF;
    DestArray dest{};
  };

  struct MacroScore {
    MacroKey key = 0;
    int cost = INF;
  };

  array<MacroKey, MAX_DEPTH + 1> pow3{};
  pow3[0] = 1;
  REP(i, MAX_DEPTH) pow3[i + 1] = pow3[i] * 3ULL;

  auto CodeToEdge = [](int code) {
    if (code == 0) return Op(Op::F).Index();
    if (code == 1) return Op(Op::L).Index();
    assert(code == 2);
    return Op(Op::R).Index();
  };

  auto CodeToOp = [](int code) {
    if (code == 0) return Op::F;
    if (code == 1) return Op::L;
    assert(code == 2);
    return Op::R;
  };

  array<DestArray, 3> basic_next{};
  REP(edge, 3) REP(node, spatial_count) basic_next[edge][node] = init_graph[node].dest[edge];

  array<array<NodeT, 4>, MAX_M> basket_nodes{};
  array<int8_t, MAX_SPATIAL> ball_id_by_node{};
  ball_id_by_node.fill(-1);
  REP(task, M) {
    REP(dir, 4) {
      basket_nodes[task][dir] = (NodeT)NodeIdByCell(Bascket[task], dir);
      ball_id_by_node[NodeIdByCell(Ball[task], dir)] = (int8_t)task;
    }
  }

  alignas(64) array<Mask, MAX_SPATIAL> bfs_visited{};
  alignas(64) array<array<Mask, MAX_SPATIAL>, 2> bfs_frontier{};
  alignas(64) array<array<NodeT, MAX_SPATIAL>, 2> bfs_active_nodes{};
  [[maybe_unused]] auto MinPerfectMatching = [&](const array<array<int, MAX_M>, MAX_M>& cost) {
    if (M <= 1) return 0;
    array<int, MAX_M + 1> u{};
    array<int, MAX_M + 1> v{};
    array<int, MAX_M + 1> minv{};
    array<int, MAX_M + 1> p{};
    array<int, MAX_M + 1> way{};
    array<uint8_t, MAX_M + 1> used{};
    for (int i = 1; i <= M; i++) {
      p[0] = i;
      int j0 = 0;
      fill(minv.begin(), minv.begin() + M + 1, INF);
      fill(used.begin(), used.begin() + M + 1, 0);
      do {
        used[j0] = 1;
        int i0 = p[j0];
        int delta = INF;
        int j1 = 0;
        for (int j = 1; j <= M; j++) {
          if (used[j]) continue;
          int cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
          if (cur < minv[j]) {
            minv[j] = cur;
            way[j] = j0;
          }
          if (minv[j] < delta) {
            delta = minv[j];
            j1 = j;
          }
        }
        for (int j = 0; j <= M; j++) {
          if (used[j]) {
            u[p[j]] += delta;
            v[j] -= delta;
          } else {
            minv[j] -= delta;
          }
        }
        j0 = j1;
      } while (p[j0] != 0);
      do {
        int j1 = way[j0];
        p[j0] = p[j1];
        j0 = j1;
      } while (j0 != 0);
    }
    return (int)(-v[0]);
  };

  // Score a macro by treating it as one extra BFS edge in addition to F/L/R.
  // The limit is the current worst kept score at the same depth.
  auto Evaluate = [&](const DestArray& macro_dest, int limit) {
    const Mask all_tasks = (1ULL << M) - 1ULL;

    fill(bfs_visited.begin(), bfs_visited.begin() + spatial_count, 0);
    fill(bfs_frontier[0].begin(), bfs_frontier[0].begin() + spatial_count, 0);
    fill(bfs_frontier[1].begin(), bfs_frontier[1].begin() + spatial_count, 0);

    int side = 0;
    int active_count[2] = {0, 0};

    auto PushInitial = [&](int node, Mask bit) {
      if (bfs_visited[node] & bit) return;
      bfs_visited[node] |= bit;
      if (bfs_frontier[side][node] == 0) {
        bfs_active_nodes[side][active_count[side]++] = (NodeT)node;
      }
      bfs_frontier[side][node] |= bit;
    };

    REP(task, M) {
      Mask bit = 1ULL << task;
      REP(dir, 4) PushInitial(NodeIdByCell(Ball[task], dir), bit);
    }

    int delivery_total = 0;
    Mask unresolved = all_tasks;
    for (int step = 0; active_count[side] > 0 && unresolved != 0; step++) {
      for (Mask remaining = unresolved; remaining != 0;) {
        int task = __builtin_ctzll(remaining);
        remaining &= remaining - 1;
        const auto& nodes = basket_nodes[task];
        Mask bit = 1ULL << task;
        Mask hit = bfs_frontier[side][nodes[0]] | bfs_frontier[side][nodes[1]] |
                   bfs_frontier[side][nodes[2]] | bfs_frontier[side][nodes[3]];
        if (hit & bit) {
          unresolved &= ~bit;
          delivery_total += step;
        }
      }
      if (unresolved == 0) break;

      int remaining = __builtin_popcountll(unresolved);
      int lower_bound_total = delivery_total + remaining * (step + 1);
      if constexpr (USE_BASKET_TO_BALL) {
        int lower_bound_score = lower_bound_total * DELIVERY_COST_WEIGHT;
        if (lower_bound_score >= limit) {
          return lower_bound_score;
        }
      } else {
        if (lower_bound_total >= limit) {
          return lower_bound_total;
        }
      }

      int next_side = side ^ 1;
      active_count[next_side] = 0;
      REP(idx, active_count[side]) {
        int node = bfs_active_nodes[side][idx];
        Mask mask = bfs_frontier[side][node] & unresolved;
        bfs_frontier[side][node] = 0;
        if (mask == 0) continue;

        auto Relax = [&](int next) {
          Mask add = mask & ~bfs_visited[next];
          if (add == 0) return;
          bfs_visited[next] |= add;
          if (bfs_frontier[next_side][next] == 0) {
            bfs_active_nodes[next_side][active_count[next_side]++] = (NodeT)next;
          }
          bfs_frontier[next_side][next] |= add;
        };

        Relax(basic_next[0][node]);
        Relax(basic_next[1][node]);
        Relax(basic_next[2][node]);
        Relax(macro_dest[node]);
      }
      side = next_side;
    }

    if constexpr (!USE_BASKET_TO_BALL) return delivery_total;

    if (delivery_total * DELIVERY_COST_WEIGHT >= limit) {
      return delivery_total * DELIVERY_COST_WEIGHT;
    }

    constexpr int MATCH_INF = INF / 4;
    array<array<int, MAX_M>, MAX_M> basket_to_ball{};
    REP(i, M) REP(j, M) basket_to_ball[i][j] = MATCH_INF;

    array<Mask, MAX_M> unresolved_by_ball{};
    REP(ball_id, M) unresolved_by_ball[ball_id] = all_tasks & ~(1ULL << ball_id);
    int remaining_pairs = M * (M - 1);

    fill(bfs_visited.begin(), bfs_visited.begin() + spatial_count, 0);
    fill(bfs_frontier[0].begin(), bfs_frontier[0].begin() + spatial_count, 0);
    fill(bfs_frontier[1].begin(), bfs_frontier[1].begin() + spatial_count, 0);
    side = 0;
    active_count[0] = active_count[1] = 0;

    REP(basket_id, M) {
      Mask bit = 1ULL << basket_id;
      REP(dir, 4) PushInitial(basket_nodes[basket_id][dir], bit);
    }

    for (int step = 0; active_count[side] > 0 && remaining_pairs > 0; step++) {
      REP(idx, active_count[side]) {
        int node = bfs_active_nodes[side][idx];
        Mask mask = bfs_frontier[side][node];
        int ball_id = ball_id_by_node[node];
        if (ball_id < 0) continue;
        Mask hit = mask & unresolved_by_ball[ball_id];
        if (hit == 0) continue;

        unresolved_by_ball[ball_id] &= ~hit;
        while (hit) {
          int basket_id = __builtin_ctzll(hit);
          hit &= hit - 1;
          basket_to_ball[basket_id][ball_id] = step;
          remaining_pairs--;
        }
      }
      if (remaining_pairs == 0) break;

      int next_side = side ^ 1;
      active_count[next_side] = 0;
      REP(idx, active_count[side]) {
        int node = bfs_active_nodes[side][idx];
        Mask mask = bfs_frontier[side][node];
        bfs_frontier[side][node] = 0;

        auto Relax = [&](int next) {
          Mask add = mask & ~bfs_visited[next];
          if (add == 0) return;
          bfs_visited[next] |= add;
          if (bfs_frontier[next_side][next] == 0) {
            bfs_active_nodes[next_side][active_count[next_side]++] = (NodeT)next;
          }
          bfs_frontier[next_side][next] |= add;
        };

        Relax(basic_next[0][node]);
        Relax(basic_next[1][node]);
        Relax(basic_next[2][node]);
        Relax(macro_dest[node]);
      }
      side = next_side;
    }

    int matching_cost = MinPerfectMatching(basket_to_ball);
    return delivery_total * DELIVERY_COST_WEIGHT + matching_cost * matching_cost_weight;
  };

  auto LessCandidate = [](const Candidate& lhs, const Candidate& rhs) {
    if (lhs.cost != rhs.cost) return lhs.cost < rhs.cost;
    return lhs.key < rhs.key;
  };
  auto LessMacroScore = [](const MacroScore& lhs, const MacroScore& rhs) {
    if (lhs.cost != rhs.cost) return lhs.cost < rhs.cost;
    return lhs.key < rhs.key;
  };

  auto WorstCandidateIndex = [&](const vector<Candidate>& candidates) {
    int worst = 0;
    REP(i, (int)candidates.size()) {
      if (LessCandidate(candidates[worst], candidates[i])) worst = i;
    }
    return worst;
  };

  auto WorstMacroScoreIndex = [&](const vector<MacroScore>& scores) {
    int worst = 0;
    REP(i, (int)scores.size()) {
      if (LessMacroScore(scores[worst], scores[i])) worst = i;
    }
    return worst;
  };

  auto HasAdjacentCancelTurnsInOutput = [&](MacroKey key, int depth) {
    int previous_code = -1;
    REP(_, depth) {
      int code = key % 3;
      key /= 3;
      if ((code == 1 && previous_code == 2) || (code == 2 && previous_code == 1)) return true;
      previous_code = code;
    }
    return false;
  };

  vector<vector<Candidate>> layers(MAX_DEPTH + 1);
  vector<vector<MacroScore>> best_by_depth(MAX_DEPTH + 1);
  vector<unordered_set<MacroKey>> seen_macro(MAX_DEPTH + 1);
  vector<unordered_set<uint64_t>> seen_dest(MAX_DEPTH + 1);
  unordered_map<uint64_t, int> eval_cache;
  REP(depth, MAX_DEPTH + 1) seen_macro[depth].reserve(LAYER_WIDTH * 8);
  REP(depth, MAX_DEPTH + 1) seen_dest[depth].reserve(LAYER_WIDTH * 16);
  eval_cache.reserve(1 << 16);

  // Bounded layered search: keep only promising open candidates per length,
  // while separately remembering the best output macros for each length.
  Candidate initial;
  initial.cost = 0;
  REP(node, spatial_count) initial.dest[node] = node;
  layers[0].push_back(initial);
  seen_macro[0].insert(initial.key);

  auto AddTop = [&](int depth, const Candidate& candidate) {
    if (depth < MIN_OUTPUT_MACRO_LENGTH) return;
    if (HasAdjacentCancelTurnsInOutput(candidate.key, depth)) {
      return;
    }
    vector<MacroScore>& top = best_by_depth[depth];
    MacroScore score{candidate.key, candidate.cost};
    if ((int)top.size() < take_per_length) {
      top.push_back(score);
      return;
    }
    int worst = WorstMacroScoreIndex(top);
    if (LessMacroScore(score, top[worst])) top[worst] = score;
  };

  auto AddOpen = [&](int depth, Candidate&& candidate) {
    vector<Candidate>& layer = layers[depth];
    AddTop(depth, candidate);
    if ((int)layer.size() < LAYER_WIDTH) {
      layer.push_back(std::move(candidate));
      return;
    }
    int worst = WorstCandidateIndex(layer);
    if (LessCandidate(candidate, layer[worst])) layer[worst] = std::move(candidate);
  };

  auto LayerWorstCost = [&](int depth) {
    const vector<Candidate>& layer = layers[depth];
    if ((int)layer.size() < LAYER_WIDTH) return INF;
    return layer[WorstCandidateIndex(layer)].cost;
  };

  // Compose one basic operation at the front or back without replaying the
  // whole macro string for every child.
  auto BuildChildDest = [&](const Candidate& parent, int edge, bool front, Candidate& child) {
    if (front) {
      REP(node, spatial_count) {
        child.dest[node] = parent.dest[basic_next[edge][node]];
      }
    } else {
      REP(node, spatial_count) {
        child.dest[node] = basic_next[edge][parent.dest[node]];
      }
    }
  };

  auto BuildDestFromKey = [&](MacroKey key, int depth, DestArray& dest) {
    array<int, MAX_DEPTH> codes{};
    for (int i = depth - 1; i >= 0; i--) {
      codes[i] = key % 3;
      key /= 3;
    }
    REP(node, spatial_count) dest[node] = node;
    REP(i, depth) {
      int edge = CodeToEdge(codes[i]);
      REP(node, spatial_count) {
        dest[node] = basic_next[edge][dest[node]];
      }
    }
  };

  auto HashDest = [&](const DestArray& dest) {
    uint64_t h = 1469598103934665603ULL;
    REP(node, spatial_count) {
      h ^= uint64_t(dest[node] + 1);
      h *= 1099511628211ULL;
    }
    return h;
  };

  auto KeyToOps = [&](MacroKey key, int depth) {
    array<int, MAX_DEPTH> codes{};
    for (int i = depth - 1; i >= 0; i--) {
      codes[i] = key % 3;
      key /= 3;
    }
    vector<Op> ret;
    ret.reserve(depth);
    REP(i, depth) ret.emplace_back(CodeToOp(codes[i]));
    return ret;
  };

  auto InsertTwoInternalFKey = [&](MacroKey key, int depth, int a, int b) {
    if (a > b) swap(a, b);
    int suffix_len = depth - b;
    MacroKey suffix_base = pow3[suffix_len];
    MacroKey prefix_and_mid = key / suffix_base;
    MacroKey suffix = key % suffix_base;
    int mid_len = b - a;
    MacroKey mid_base = pow3[mid_len];
    MacroKey prefix = prefix_and_mid / mid_base;
    MacroKey mid = prefix_and_mid % mid_base;
    return prefix * pow3[mid_len + suffix_len + 2] + mid * pow3[suffix_len + 1] + suffix;
  };

  auto InsertInternalFKey = [&](MacroKey key, int depth, int pos) {
    int suffix_len = depth - pos;
    MacroKey suffix_base = pow3[suffix_len];
    MacroKey prefix = key / suffix_base;
    MacroKey suffix = key % suffix_base;
    return prefix * pow3[suffix_len + 1] + suffix;
  };

  auto MoveOneFReinsertKey = [&](MacroKey key, int depth, MacroKey& moved_key) {
    if (depth < 3) return false;
    array<int, MAX_DEPTH> codes{};
    for (int i = depth - 1; i >= 0; i--) {
      codes[i] = key % 3;
      key /= 3;
    }

    int best_delete_pos = -1;
    int best_insert_gap = -1;
    int best_span = INF;
    int best_direction = INF;
    REP(delete_pos, depth) {
      if (codes[delete_pos] != 0) continue;
      REP(insert_gap, depth + 1) {
        // Inserting immediately before or after the deleted F is a no-op.
        if (insert_gap == delete_pos || insert_gap == delete_pos + 1) continue;

        int left = -1;
        int right = -1;
        int direction = 0;
        if (insert_gap < delete_pos) {
          left = insert_gap;
          right = delete_pos - 1;
          direction = 0;
        } else {
          left = delete_pos + 1;
          right = insert_gap - 1;
          direction = 1;
        }

        bool has_l = false;
        bool has_r = false;
        for (int i = left; i <= right; i++) {
          has_l |= codes[i] == 1;
          has_r |= codes[i] == 2;
        }
        if (!has_l || !has_r) continue;

        int span = right - left + 1;
        if (span < best_span ||
            (span == best_span && direction < best_direction) ||
            (span == best_span && direction == best_direction && delete_pos < best_delete_pos)) {
          best_span = span;
          best_direction = direction;
          best_delete_pos = delete_pos;
          best_insert_gap = insert_gap;
        }
      }
    }
    if (best_delete_pos < 0) return false;

    array<int, MAX_DEPTH> removed{};
    int removed_len = 0;
    REP(i, depth) {
      if (i == best_delete_pos) continue;
      removed[removed_len++] = codes[i];
    }
    int insert_pos = best_insert_gap <= best_delete_pos ? best_insert_gap : best_insert_gap - 1;
    moved_key = 0;
    int read = 0;
    REP(pos, depth) {
      int code = 0;
      if (pos == insert_pos) {
        code = 0;
      } else {
        code = removed[read++];
      }
      moved_key = moved_key * 3ULL + MacroKey(code);
    }
    return true;
  };

  auto RotateLeftKey = [&](MacroKey key, int depth) {
    array<int, MAX_DEPTH> codes{};
    for (int i = depth - 1; i >= 0; i--) {
      codes[i] = key % 3;
      key /= 3;
    }
    MacroKey ret = 0;
    for (int i = 1; i < depth; i++) ret = ret * 3 + codes[i];
    ret = ret * 3 + codes[0];
    return ret;
  };

  auto RotateRightKey = [&](MacroKey key, int depth) {
    array<int, MAX_DEPTH> codes{};
    for (int i = depth - 1; i >= 0; i--) {
      codes[i] = key % 3;
      key /= 3;
    }
    MacroKey ret = codes[depth - 1];
    REP(i, depth - 1) ret = ret * 3 + codes[i];
    return ret;
  };

  auto HasAdjacentOppositeTurns = [&](MacroKey key, int depth) {
    int previous_code = -1;
    REP(_, depth) {
      int code = key % 3;
      key /= 3;
      if (code != 0 && previous_code != 0 && code == previous_code) return true;
      previous_code = code;
    }
    return false;
  };

  vector<vector<Op>> macros;
  const array<int, 3> op_codes = {1, 2, 0};

  auto TryAddCandidate = [&](int target_depth, MacroKey child_key, auto&& build_dest,
                             bool allow_double_turn = false) {
    if (!timer.within(stop_cycle)) return false;
    if (!allow_double_turn && HasAdjacentOppositeTurns(child_key, target_depth)) {
      return true;
    }
    if (!seen_macro[target_depth].insert(child_key).second) return true;

    Candidate child;
    child.key = child_key;
    build_dest(child);

    uint64_t dest_hash = HashDest(child.dest);
    if (!seen_dest[target_depth].insert(dest_hash).second) {
      return true;
    }

    int cutoff = LayerWorstCost(target_depth);
    auto cache_it = eval_cache.find(dest_hash);
    if (cache_it != eval_cache.end()) {
      child.cost = cache_it->second;
    } else {
      child.cost = Evaluate(child.dest, cutoff);
      if (cutoff == INF || child.cost < cutoff) {
        eval_cache.emplace(dest_hash, child.cost);
      }
    }
    AddOpen(target_depth, std::move(child));
    return true;
  };

  auto AddDoubleTurnMotifSeeds = [&]() {
    const vector<int> lens = {1, 2, 3, 4, 5, 7, 10, 14, 20, 28};
    auto AddSeed = [&](const vector<int>& codes) {
      int depth = (int)codes.size();
      if (depth <= 0 || depth > MAX_DEPTH) return true;
      MacroKey key = 0;
      for (int code : codes) key = key * 3ULL + MacroKey(code);
      return TryAddCandidate(depth, key, [&](Candidate& child) {
        BuildDestFromKey(key, depth, child.dest);
      }, true);
    };
    auto AddFs = [&](vector<int>& codes, int count) {
      REP(_, count) codes.push_back(0);
    };

    for (int turn : {1, 2}) {
      vector<int> codes = {turn, turn};
      if (!AddSeed(codes)) return;
    }
    for (int a : lens) {
      for (int turn : {1, 2}) {
        vector<int> codes;
        AddFs(codes, a);
        codes.push_back(turn);
        codes.push_back(turn);
        if (!AddSeed(codes)) return;

        codes.clear();
        codes.push_back(turn);
        codes.push_back(turn);
        AddFs(codes, a);
        if (!AddSeed(codes)) return;

        codes.clear();
        codes.push_back(turn);
        AddFs(codes, a);
        codes.push_back(turn);
        if (!AddSeed(codes)) return;
      }
    }
  };

  AddDoubleTurnMotifSeeds();

  auto AddDemandRepairSeeds = [&]() {
    array<int, MAX_SPATIAL> dist{};
    array<NodeT, MAX_SPATIAL> prev_node{};
    array<int8_t, MAX_SPATIAL> prev_code{};
    array<NodeT, MAX_SPATIAL> que{};
    array<uint8_t, MAX_SPATIAL> is_target{};

    auto ShortestPathCodes = [&](int source, const vector<int>& targets) {
      fill(dist.begin(), dist.begin() + spatial_count, -1);
      fill(is_target.begin(), is_target.begin() + spatial_count, 0);
      for (int target : targets) is_target[target] = 1;

      int qh = 0;
      int qt = 0;
      dist[source] = 0;
      prev_node[source] = (NodeT)source;
      prev_code[source] = -1;
      que[qt++] = (NodeT)source;

      int goal = -1;
      while (qh < qt) {
        int node = que[qh++];
        if (is_target[node]) {
          goal = node;
          break;
        }
        REP(code, 3) {
          int next = basic_next[CodeToEdge(code)][node];
          if (dist[next] >= 0) continue;
          dist[next] = dist[node] + 1;
          prev_node[next] = (NodeT)node;
          prev_code[next] = (int8_t)code;
          que[qt++] = (NodeT)next;
        }
      }

      vector<int> path;
      if (goal < 0) return path;
      for (int cur = goal; cur != source; cur = prev_node[cur]) {
        path.push_back(prev_code[cur]);
      }
      reverse(ALL(path));
      return path;
    };

    auto TargetNodes = [&](int cell) {
      vector<int> ret;
      ret.reserve(4);
      REP(dir, 4) ret.push_back(NodeIdByCell(cell, dir));
      return ret;
    };

    auto BestPathBetweenCells = [&](int source_cell, bool use_all_source_dirs, int target_cell) {
      vector<int> targets = TargetNodes(target_cell);
      vector<int> best;
      int source_dirs = use_all_source_dirs ? 4 : 1;
      REP(dir, source_dirs) {
        vector<int> path = ShortestPathCodes(NodeIdByCell(source_cell, dir), targets);
        if (path.empty() && source_cell != target_cell) continue;
        if (best.empty() || path.size() < best.size()) best = std::move(path);
      }
      return best;
    };

    vector<pair<int, vector<int>>> repair_paths;
    repair_paths.reserve(M * 2);
    REP(task, M) {
      vector<int> pickup = BestPathBetweenCells(0, false, Ball[task]);
      if (!pickup.empty()) repair_paths.emplace_back((int)pickup.size(), std::move(pickup));
      vector<int> delivery = BestPathBetweenCells(Ball[task], true, Bascket[task]);
      if (!delivery.empty()) repair_paths.emplace_back((int)delivery.size() * 2, std::move(delivery));
    }
    sort(repair_paths.rbegin(), repair_paths.rend(),
         [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });
    if ((int)repair_paths.size() > 16) repair_paths.resize(16);

    unordered_map<uint64_t, int> substring_score;
    substring_score.reserve(1 << 14);
    for (const auto& [path_weight, path] : repair_paths) {
      int n = (int)path.size();
      for (int l = 0; l < n; l++) {
        MacroKey key = 0;
        for (int r = l; r < min(n, l + MAX_DEPTH); r++) {
          key = key * 3ULL + MacroKey(path[r]);
          int depth = r - l + 1;
          if (depth < MIN_OUTPUT_MACRO_LENGTH) continue;
          uint64_t packed = (uint64_t(depth) << 56) | uint64_t(key);
          substring_score[packed] += path_weight * (depth - 1);
        }
      }
    }

    vector<pair<int, uint64_t>> seeds;
    seeds.reserve(substring_score.size());
    for (auto [packed, score] : substring_score) seeds.emplace_back(-score, packed);
    sort(ALL(seeds));

    constexpr int MAX_REPAIR_SEEDS = 200;
    int added = 0;
    for (auto [neg_score, packed] : seeds) {
      if (added >= MAX_REPAIR_SEEDS) break;
      int depth = int(packed >> 56);
      MacroKey key = MacroKey(packed & ((1ULL << 56) - 1));
      bool ok = TryAddCandidate(depth, key, [&](Candidate& child) {
        BuildDestFromKey(key, depth, child.dest);
      });
      if (!ok) break;
      added++;
    }
  };

  AddDemandRepairSeeds();

  auto AddEdgeTransitions = [&](const Candidate& current, int depth) {
    int target_depth = depth + 1;
    for (int code : op_codes) {
      int edge = CodeToEdge(code);
      REP(front, 2) {
        bool prepend = front;
        MacroKey child_key = prepend ? MacroKey(code) * pow3[depth] + current.key : current.key * 3ULL + MacroKey(code);
        bool ok = TryAddCandidate(target_depth, child_key, [&](Candidate& child) {
          BuildChildDest(current, edge, prepend, child);
        });
        if (!ok) return;
      }
    }
  };

  auto AddInternalFInsertions = [&](const Candidate& current, int depth) {
    if (depth < 2) return;
    int target_depth = depth + 1;
    for (int insert_pos = 1; insert_pos < depth; insert_pos++) {
      MacroKey child_key = InsertInternalFKey(current.key, depth, insert_pos);
      bool ok = TryAddCandidate(target_depth, child_key, [&](Candidate& child) {
        BuildDestFromKey(child_key, target_depth, child.dest);
      });
      if (!ok) return;
    }
  };

  auto AddTwoInternalFInsertions = [&](const Candidate& current, int depth) {
    if (depth < 3 || depth + 2 > MAX_DEPTH) return;
    int target_depth = depth + 2;
    REP(two_insert_trial, two_f_insert_trials) {
      int insert_pos_a = rnd.next(1, depth - 1);
      int insert_pos_b = rnd.next(1, depth - 2);
      if (insert_pos_b >= insert_pos_a) insert_pos_b++;
      MacroKey child_key = InsertTwoInternalFKey(current.key, depth, insert_pos_a, insert_pos_b);
      bool ok = TryAddCandidate(target_depth, child_key, [&](Candidate& child) {
        BuildDestFromKey(child_key, target_depth, child.dest);
      });
      if (!ok) return;
    }
  };

  auto AddMoveFReinsertTransition = [&](MacroKey current_key, int depth) {
    MacroKey child_key = 0;
    if (!MoveOneFReinsertKey(current_key, depth, child_key)) return;
    bool ok = TryAddCandidate(depth, child_key, [&](Candidate& child) {
      BuildDestFromKey(child_key, depth, child.dest);
    });
    if (!ok) return;
  };

  auto AddRotateTransitions = [&](MacroKey current_key, int depth) {
    if (depth < 2) return;
    MacroKey left_key = RotateLeftKey(current_key, depth);
    bool ok = TryAddCandidate(depth, left_key, [&](Candidate& child) {
      BuildDestFromKey(left_key, depth, child.dest);
    });
    if (!ok) return;

    MacroKey right_key = RotateRightKey(current_key, depth);
    ok = TryAddCandidate(depth, right_key, [&](Candidate& child) {
      BuildDestFromKey(right_key, depth, child.dest);
    });
    if (!ok) return;
  };

  constexpr int SPLICE_POOL_LIMIT = 4;
  constexpr int SPLICE_INTERVAL = 2;
  auto AddSpliceTransitions = [&]() {
    struct SplicePart {
      MacroKey key;
      int depth;
      int cost;
    };

    vector<SplicePart> parts;
    parts.reserve(MAX_DEPTH);
    for (int depth = MIN_OUTPUT_MACRO_LENGTH; depth <= MAX_DEPTH; depth++) {
      const vector<MacroScore>& top = best_by_depth[depth];
      if (top.empty()) continue;
      int best = 0;
      REP(i, (int)top.size()) {
        if (LessMacroScore(top[i], top[best])) best = i;
      }
      parts.push_back(SplicePart{top[best].key, depth, top[best].cost});
    }

    sort(ALL(parts), [](const SplicePart& lhs, const SplicePart& rhs) {
      if (lhs.cost != rhs.cost) return lhs.cost < rhs.cost;
      if (lhs.depth != rhs.depth) return lhs.depth < rhs.depth;
      return lhs.key < rhs.key;
    });
    if ((int)parts.size() > SPLICE_POOL_LIMIT) parts.resize(SPLICE_POOL_LIMIT);

    REP(i, (int)parts.size()) REP(j, (int)parts.size()) {
      if (i == j) continue;
      const SplicePart& lhs = parts[i];
      const SplicePart& rhs = parts[j];
      if (lhs.depth == rhs.depth) continue;
      int target_depth = lhs.depth + rhs.depth;
      if (target_depth > MAX_DEPTH) continue;
      MacroKey child_key = lhs.key * pow3[rhs.depth] + rhs.key;
      bool ok = TryAddCandidate(target_depth, child_key, [&](Candidate& child) {
        BuildDestFromKey(child_key, target_depth, child.dest);
      });
      if (!ok) return false;
    }
    return true;
  };

  // Chokudai-style expansion: repeatedly pick the best open candidate from
  // each depth and apply the local macro-edit transitions below.
  int splice_iteration = 0;
  while (timer.within(stop_cycle)) {
    bool progressed = false;
    for (int depth = 0; depth < MAX_DEPTH && timer.within(stop_cycle); depth++) {
      vector<Candidate>& layer = layers[depth];
      if (layer.empty()) continue;
      int best_index = 0;
      REP(i, (int)layer.size()) {
        if (LessCandidate(layer[i], layer[best_index])) best_index = i;
      }
      Candidate& current = layer[best_index];
      MacroKey current_key = current.key;
      progressed = true;

      AddEdgeTransitions(current, depth);
      AddInternalFInsertions(current, depth);
      AddTwoInternalFInsertions(current, depth);

      if (best_index + 1 != (int)layer.size()) layer[best_index] = std::move(layer.back());
      layer.pop_back();
      AddMoveFReinsertTransition(current_key, depth);
      if (use_rotate_transitions) AddRotateTransitions(current_key, depth);
    }
    if (!progressed) break;
    if (splice_iteration == 0 && !AddSpliceTransitions()) break;
    splice_iteration++;
    if (splice_iteration == SPLICE_INTERVAL) splice_iteration = 0;
  }

  // Export a small number of good macros from every length class.
  for (int depth = MIN_OUTPUT_MACRO_LENGTH; depth <= MAX_DEPTH; depth++) {
    vector<MacroScore>& top = best_by_depth[depth];
    sort(ALL(top), LessMacroScore);
    REP(i, min(take_per_length, (int)top.size())) {
      macros.emplace_back(KeyToOps(top[i].key, depth));
    }
  }
  return macros;
}

using InitialOrderPlan = tuple<vector<Op>, vector<int>, int>;

vector<InitialOrderPlan> PlanInitialOrder(double seconds, const vector<vector<Op>>& macros) {
  const int spatial_count = N * N * 4;
  vector<vector<Op>> eval_macros = macros;
  if (eval_macros.empty()) eval_macros.push_back({});

  constexpr int MAX_SPATIAL = MAX_N * MAX_N * 4;
  using PlanNode = uint16_t;
  using PlanDist = uint16_t;
  static_assert(MAX_SPATIAL < numeric_limits<PlanNode>::max());
  static_assert(MAX_T < numeric_limits<PlanDist>::max());
  array<array<int, MAX_SPATIAL>, 3> basic_next{};
  REP(edge, 3) REP(node, spatial_count) basic_next[edge][node] = init_graph[node].dest[edge];

  array<PlanNode, MAX_SPATIAL> macro_dest{};
  array<int, MAX_SPATIAL> seen{};
  array<PlanDist, MAX_SPATIAL> node_command_dist{};
  array<PlanDist, MAX_SPATIAL> node_expanded_dist{};
  array<int, MAX_SPATIAL> queued_stamp{};
  array<PlanDist, MAX_SPATIAL> queued_command{};
  array<int, MAX_SPATIAL> target_mark{};
  array<array<PlanNode, MAX_SPATIAL>, 2> bfs_queues{};
  int stamp = 0;
  int target_stamp = 0;

  auto ApplyMacroOp = [](int node, Op op) {
    if (op.value == Op::F || op.value == Op::L || op.value == Op::R) {
      return init_graph[node].dest[op.Index()];
    }
    return node;
  };

  struct MoveCost {
    int command = INF;
    int expanded = INF;
  };

  auto AddMoveCost = [](MoveCost lhs, MoveCost rhs) {
    if (lhs.command >= INF || rhs.command >= INF) return MoveCost{};
    return MoveCost{lhs.command + rhs.command, lhs.expanded + rhs.expanded};
  };

  auto BetterMoveCost = [](MoveCost lhs, MoveCost rhs) {
    if (lhs.command != rhs.command) return lhs.command < rhs.command;
    return lhs.expanded < rhs.expanded;
  };

  const int expanded_fixed_cost = 2 * M + 2;
  auto RouteScore = [&](MoveCost cost) {
    if (cost.command >= INF) return (double)INF;
    int total_expanded = cost.expanded + expanded_fixed_cost;
    if (total_expanded > T) return total_expanded * 100.0;
    return (double)cost.command;
  };

  auto BetterRouteCost = [&](MoveCost lhs, MoveCost rhs) {
    double lhs_score = RouteScore(lhs);
    double rhs_score = RouteScore(rhs);
    if (lhs_score != rhs_score) return lhs_score < rhs_score;
    if (lhs.command != rhs.command) return lhs.command < rhs.command;
    return lhs.expanded < rhs.expanded;
  };

  struct DirectionModel {
    array<array<MoveCost, 4>, MAX_M> start{};
    array<array<MoveCost, 4>, MAX_M> finish{};
    array<array<array<array<MoveCost, 4>, MAX_M>, 4>, MAX_M> transition{};
  };

  array<array<int, 4>, MAX_M> ball_nodes{};
  array<array<int, 4>, MAX_M> basket_nodes{};
  array<int, MAX_M * 4> all_ball_targets{};
  REP(task, M) REP(dir, 4) {
    ball_nodes[task][dir] = NodeIdByCell(Ball[task], dir);
    basket_nodes[task][dir] = NodeIdByCell(Bascket[task], dir);
    all_ball_targets[task * 4 + dir] = ball_nodes[task][dir];
  }

  auto BuildModel = [&](const vector<Op>& macro) {
    DirectionModel model;
    REP(node, spatial_count) {
      int cur = node;
      for (Op op : macro) cur = ApplyMacroOp(cur, op);
      macro_dest[node] = (PlanNode)cur;
    }

    int macro_expanded_cost = (int)macro.size();

    auto RunBfs = [&](int start_node, const int* targets, int target_count) {
      stamp++;
      target_stamp++;
      int remaining_targets = 0;
      REP(i, target_count) {
        int target = targets[i];
        if (target_mark[target] == target_stamp) continue;
        target_mark[target] = target_stamp;
        remaining_targets++;
      }

      auto ConsumeTarget = [&](int node) {
        if (target_mark[node] != target_stamp) return;
        target_mark[node] = 0;
        remaining_targets--;
      };

      int current_buffer = 0;
      int tail = 1;
      seen[start_node] = stamp;
      node_command_dist[start_node] = 0;
      node_expanded_dist[start_node] = 0;
      bfs_queues[current_buffer][0] = (PlanNode)start_node;
      ConsumeTarget(start_node);
      if (remaining_targets == 0) return;

      for (int command = 0; tail > 0; command++) {
        int next_buffer = current_buffer ^ 1;
        int next_tail = 0;
        int next_command = command + 1;
        auto& current_queue = bfs_queues[current_buffer];
        auto& next_queue = bfs_queues[next_buffer];

        auto Relax = [&](int next, int next_expanded) {
          if (seen[next] != stamp) {
            seen[next] = stamp;
            node_command_dist[next] = (PlanDist)next_command;
            node_expanded_dist[next] = (PlanDist)next_expanded;
            ConsumeTarget(next);
          } else {
            if (node_command_dist[next] != next_command) return;
            if (next_expanded >= node_expanded_dist[next]) return;
            node_expanded_dist[next] = (PlanDist)next_expanded;
          }
          if (queued_stamp[next] == stamp && queued_command[next] == next_command) return;
          queued_stamp[next] = stamp;
          queued_command[next] = (PlanDist)next_command;
          next_queue[next_tail++] = (PlanNode)next;
        };

        REP(head, tail) {
          int node = current_queue[head];
          if (seen[node] != stamp || node_command_dist[node] != command) continue;
          int base_expanded = node_expanded_dist[node];
          Relax(basic_next[0][node], base_expanded + 1);
          Relax(basic_next[1][node], base_expanded + 1);
          Relax(basic_next[2][node], base_expanded + 1);
          Relax(macro_dest[node], base_expanded + macro_expanded_cost);
        }

        if (remaining_targets == 0) break;
        current_buffer = next_buffer;
        tail = next_tail;
      }
    };

    auto NodeCost = [&](int node) {
      if (seen[node] != stamp) return MoveCost{};
      return MoveCost{node_command_dist[node], node_expanded_dist[node]};
    };

    array<array<array<MoveCost, 4>, 4>, MAX_M> deliver_dist{};
    array<array<array<array<MoveCost, 4>, MAX_M>, 4>, MAX_M> basket_to_ball_dist{};

    RunBfs(NodeId(0, 0, 0), all_ball_targets.data(), 4 * M);
    REP(task, M) REP(dir, 4) {
      model.start[task][dir] = NodeCost(ball_nodes[task][dir]);
    }

    REP(task, M) REP(pickup_dir, 4) {
      RunBfs(ball_nodes[task][pickup_dir], basket_nodes[task].data(), 4);
      MoveCost best_finish;
      REP(basket_dir, 4) {
        MoveCost dist = NodeCost(basket_nodes[task][basket_dir]);
        deliver_dist[task][pickup_dir][basket_dir] = dist;
        if (BetterMoveCost(dist, best_finish)) best_finish = dist;
      }
      model.finish[task][pickup_dir] = best_finish;
    }

    REP(task, M) REP(basket_dir, 4) {
      RunBfs(basket_nodes[task][basket_dir], all_ball_targets.data(), 4 * M);
      REP(next_task, M) REP(next_dir, 4) {
        basket_to_ball_dist[task][basket_dir][next_task][next_dir] =
            NodeCost(ball_nodes[next_task][next_dir]);
      }
    }

    REP(task, M) REP(pickup_dir, 4) REP(next_task, M) REP(next_dir, 4) {
      MoveCost best;
      REP(basket_dir, 4) {
        MoveCost cost = AddMoveCost(
            deliver_dist[task][pickup_dir][basket_dir],
            basket_to_ball_dist[task][basket_dir][next_task][next_dir]);
        if (BetterMoveCost(cost, best)) best = cost;
      }
      model.transition[task][pickup_dir][next_task][next_dir] = best;
    }

    return model;
  };

  auto BuildCommandOnlyModel = [&](const vector<Op>& macro) {
    DirectionModel model;
    REP(node, spatial_count) {
      int cur = node;
      for (Op op : macro) cur = ApplyMacroOp(cur, op);
      macro_dest[node] = (PlanNode)cur;
    }

    auto RunBfs = [&](int start_node, const int* targets, int target_count) {
      stamp++;
      target_stamp++;
      int remaining_targets = 0;
      REP(i, target_count) {
        int target = targets[i];
        if (target_mark[target] == target_stamp) continue;
        target_mark[target] = target_stamp;
        remaining_targets++;
      }

      auto ConsumeTarget = [&](int node) {
        if (target_mark[node] != target_stamp) return;
        target_mark[node] = 0;
        remaining_targets--;
      };

      int current_buffer = 0;
      int tail = 1;
      seen[start_node] = stamp;
      node_command_dist[start_node] = 0;
      bfs_queues[current_buffer][0] = (PlanNode)start_node;
      ConsumeTarget(start_node);
      if (remaining_targets == 0) return;

      for (int command = 0; tail > 0; command++) {
        int next_buffer = current_buffer ^ 1;
        int next_tail = 0;
        int next_command = command + 1;
        auto& current_queue = bfs_queues[current_buffer];
        auto& next_queue = bfs_queues[next_buffer];

        auto Relax = [&](int next) {
          if (seen[next] == stamp) return;
          seen[next] = stamp;
          node_command_dist[next] = (PlanDist)next_command;
          ConsumeTarget(next);
          next_queue[next_tail++] = (PlanNode)next;
        };

        REP(head, tail) {
          int node = current_queue[head];
          Relax(basic_next[0][node]);
          Relax(basic_next[1][node]);
          Relax(basic_next[2][node]);
          Relax(macro_dest[node]);
        }

        if (remaining_targets == 0) break;
        current_buffer = next_buffer;
        tail = next_tail;
      }
    };

    auto NodeCost = [&](int node) {
      if (seen[node] != stamp) return MoveCost{};
      int command = node_command_dist[node];
      return MoveCost{command, command};
    };

    array<array<array<MoveCost, 4>, 4>, MAX_M> deliver_dist{};
    array<array<array<array<MoveCost, 4>, MAX_M>, 4>, MAX_M> basket_to_ball_dist{};

    RunBfs(NodeId(0, 0, 0), all_ball_targets.data(), 4 * M);
    REP(task, M) REP(dir, 4) {
      model.start[task][dir] = NodeCost(ball_nodes[task][dir]);
    }

    REP(task, M) REP(pickup_dir, 4) {
      RunBfs(ball_nodes[task][pickup_dir], basket_nodes[task].data(), 4);
      MoveCost best_finish;
      REP(basket_dir, 4) {
        MoveCost dist = NodeCost(basket_nodes[task][basket_dir]);
        deliver_dist[task][pickup_dir][basket_dir] = dist;
        if (BetterMoveCost(dist, best_finish)) best_finish = dist;
      }
      model.finish[task][pickup_dir] = best_finish;
    }

    REP(task, M) REP(basket_dir, 4) {
      RunBfs(basket_nodes[task][basket_dir], all_ball_targets.data(), 4 * M);
      REP(next_task, M) REP(next_dir, 4) {
        basket_to_ball_dist[task][basket_dir][next_task][next_dir] =
            NodeCost(ball_nodes[next_task][next_dir]);
      }
    }

    REP(task, M) REP(pickup_dir, 4) REP(next_task, M) REP(next_dir, 4) {
      MoveCost best;
      REP(basket_dir, 4) {
        MoveCost cost = AddMoveCost(
            deliver_dist[task][pickup_dir][basket_dir],
            basket_to_ball_dist[task][basket_dir][next_task][next_dir]);
        if (BetterMoveCost(cost, best)) best = cost;
      }
      model.transition[task][pickup_dir][next_task][next_dir] = best;
    }

    return model;
  };

  struct PickupStep {
    int task = 0;
    int dir = 0;
  };

  struct DirectionOrderResult {
    vector<PickupStep> order;
    MoveCost cost;
  };

  auto OptimizeByModel = [&](const DirectionModel& model, double anneal_seconds, int restart_id = 0) {
    auto StepCost = [&](const PickupStep& prev, const PickupStep& next) {
      return model.transition[prev.task][prev.dir][next.task][next.dir];
    };

    auto PathCost = [&](const vector<PickupStep>& order) {
      MoveCost cost{0, 0};
      if (order.empty()) return cost;
      cost = AddMoveCost(cost, model.start[order[0].task][order[0].dir]);
      for (int i = 1; i < (int)order.size(); i++) {
        cost = AddMoveCost(cost, StepCost(order[i - 1], order[i]));
      }
      const PickupStep& last = order.back();
      cost = AddMoveCost(cost, model.finish[last.task][last.dir]);
      return cost;
    };

    // Small instances can be solved exactly on the same (task, pickup direction) model used by SA.
    if (M <= 12) {
      int full_mask = (1 << M) - 1;
      int state_stride = M * 4;
      int state_count = (full_mask + 1) * state_stride;
      vector<int> dp_command(state_count, INF);
      vector<int> dp_expanded(state_count, INF);
      vector<int> prev_state(state_count, -1);

      auto StateId = [&](int mask, int task, int dir) {
        return mask * state_stride + task * 4 + dir;
      };

      auto BetterState = [&](int lhs_command, int lhs_expanded, int rhs_command, int rhs_expanded) {
        return BetterRouteCost(MoveCost{lhs_command, lhs_expanded}, MoveCost{rhs_command, rhs_expanded});
      };

      REP(task, M) REP(dir, 4) {
        int id = StateId(1 << task, task, dir);
        dp_command[id] = model.start[task][dir].command;
        dp_expanded[id] = model.start[task][dir].expanded;
        prev_state[id] = -2;
      }

      for (int mask = 1; mask <= full_mask; mask++) {
        REP(last_task, M) {
          if (((mask >> last_task) & 1) == 0) continue;
          REP(last_dir, 4) {
            int from = StateId(mask, last_task, last_dir);
            int current_command = dp_command[from];
            int current_expanded = dp_expanded[from];
            if (current_command == INF) continue;
            int rest = full_mask ^ mask;
            for (int bit = rest; bit; bit &= bit - 1) {
              int next_task = __builtin_ctz(bit);
              int next_mask = mask | (1 << next_task);
              REP(next_dir, 4) {
                int to = StateId(next_mask, next_task, next_dir);
                MoveCost edge = model.transition[last_task][last_dir][next_task][next_dir];
                int next_command = current_command + edge.command;
                int next_expanded = current_expanded + edge.expanded;
                if (BetterState(next_command, next_expanded, dp_command[to], dp_expanded[to])) {
                  dp_command[to] = next_command;
                  dp_expanded[to] = next_expanded;
                  prev_state[to] = from;
                }
              }
            }
          }
        }
      }

      int best_state = -1;
      MoveCost best_cost;
      REP(last_task, M) REP(last_dir, 4) {
        int state = StateId(full_mask, last_task, last_dir);
        MoveCost cost = AddMoveCost(
            MoveCost{dp_command[state], dp_expanded[state]},
            model.finish[last_task][last_dir]);
        if (BetterRouteCost(cost, best_cost)) {
          best_cost = cost;
          best_state = state;
        }
      }
      assert(best_state != -1);

      vector<PickupStep> best_order;
      for (int state = best_state; state != -2; state = prev_state[state]) {
        int last = state % state_stride;
        best_order.push_back(PickupStep{last / 4, last % 4});
      }
      reverse(ALL(best_order));
      assert((int)best_order.size() == M);
      return DirectionOrderResult{std::move(best_order), best_cost};
    }

    auto LinkCost = [&](const vector<PickupStep>& order, int edge) {
      if (edge == 0) return model.start[order[0].task][order[0].dir];
      if (edge == M) return model.finish[order[M - 1].task][order[M - 1].dir];
      return StepCost(order[edge - 1], order[edge]);
    };

    auto ConnectCost = [&](const vector<PickupStep>& order, int lhs, int rhs) {
      if (lhs == -1) return model.start[order[rhs].task][order[rhs].dir];
      if (rhs == M) return model.finish[order[lhs].task][order[lhs].dir];
      return StepCost(order[lhs], order[rhs]);
    };

    auto AddEdge = [](array<int, 8>& edges, int& edge_count, int edge) {
      REP(i, edge_count) if (edges[i] == edge) return;
      edges[edge_count++] = edge;
    };

    auto AddAffectedPosition = [&](array<int, 8>& edges, int& edge_count, int pos) {
      AddEdge(edges, edge_count, pos);
      AddEdge(edges, edge_count, pos + 1);
    };

    auto SwappedStep = [&](const vector<PickupStep>& order, int pos, int a, int b) {
      if (pos == a) return order[b];
      if (pos == b) return order[a];
      return order[pos];
    };

    auto LinkCostAfterSwap = [&](const vector<PickupStep>& order, int edge, int a, int b) {
      if (edge == 0) {
        PickupStep first = SwappedStep(order, 0, a, b);
        return model.start[first.task][first.dir];
      }
      if (edge == M) {
        PickupStep last = SwappedStep(order, M - 1, a, b);
        return model.finish[last.task][last.dir];
      }
      PickupStep prev = SwappedStep(order, edge - 1, a, b);
      PickupStep next = SwappedStep(order, edge, a, b);
      return StepCost(prev, next);
    };

    auto StepWithDir = [&](const vector<PickupStep>& order, int pos, int target_pos, int dir) {
      PickupStep step = order[pos];
      if (pos == target_pos) step.dir = dir;
      return step;
    };

    auto LinkCostAfterDirChange = [&](const vector<PickupStep>& order, int edge, int pos, int dir) {
      if (edge == 0) {
        PickupStep first = StepWithDir(order, 0, pos, dir);
        return model.start[first.task][first.dir];
      }
      if (edge == M) {
        PickupStep last = StepWithDir(order, M - 1, pos, dir);
        return model.finish[last.task][last.dir];
      }
      PickupStep prev = StepWithDir(order, edge - 1, pos, dir);
      PickupStep next = StepWithDir(order, edge, pos, dir);
      return StepCost(prev, next);
    };

    vector<PickupStep> current_order;
    current_order.reserve(M);
    {
      vector<char> used(M, false);
      REP(pos, M) {
        struct GreedyCandidate {
          PickupStep step;
          MoveCost cost;
          MoveCost finish;
        };
        array<GreedyCandidate, MAX_M * 4> greedy_candidates{};
        int greedy_candidate_count = 0;
        REP(task, M) {
          if (used[task]) continue;
          REP(dir, 4) {
            PickupStep candidate{task, dir};
            MoveCost cost = pos == 0 ? model.start[task][dir] : StepCost(current_order.back(), candidate);
            MoveCost finish = model.finish[task][dir];
            greedy_candidates[greedy_candidate_count++] = GreedyCandidate{candidate, cost, finish};
          }
        }
        sort(greedy_candidates.begin(), greedy_candidates.begin() + greedy_candidate_count,
             [&](const GreedyCandidate& lhs, const GreedyCandidate& rhs) {
               if (BetterMoveCost(lhs.cost, rhs.cost)) return true;
               if (BetterMoveCost(rhs.cost, lhs.cost)) return false;
               if (BetterMoveCost(lhs.finish, rhs.finish)) return true;
               if (BetterMoveCost(rhs.finish, lhs.finish)) return false;
               if (lhs.step.task != rhs.step.task) return lhs.step.task < rhs.step.task;
               return lhs.step.dir < rhs.step.dir;
             });
        int selected = 0;
        if (restart_id > 0) {
          selected = rnd.next(min(4, greedy_candidate_count));
        }
        PickupStep best = greedy_candidates[selected].step;
        used[best.task] = true;
        current_order.push_back(best);
      }
    }

    vector<PickupStep> best_order = current_order;
    MoveCost current_cost = PathCost(current_order);
    double current_score = RouteScore(current_cost);
    double best_score = current_score;

    int64_t anneal_cycles = (int64_t)(Timer::CYCLES_PER_SEC * anneal_seconds);
    if (anneal_cycles > 0) {
      SimulatedAnnealing<0xFF> sa(anneal_cycles);
      while (!sa.End()) {
        int type = rnd.next(4);
        int delta_command = 0;
        int delta_expanded = 0;
        double delta_score = 0.0;
        bool moved = false;
        if (type == 0 && M >= 2) {
          int a = rnd.next(M);
          int b = rnd.next(M);
          if (a == b) continue;
          array<int, 8> edges{};
          int edge_count = 0;
          AddAffectedPosition(edges, edge_count, a);
          AddAffectedPosition(edges, edge_count, b);
          MoveCost before{0, 0};
          MoveCost after{0, 0};
          REP(i, edge_count) {
            before = AddMoveCost(before, LinkCost(current_order, edges[i]));
            after = AddMoveCost(after, LinkCostAfterSwap(current_order, edges[i], a, b));
          }
          delta_command = after.command - before.command;
          delta_expanded = after.expanded - before.expanded;
          MoveCost next_cost{current_cost.command + delta_command, current_cost.expanded + delta_expanded};
          double next_score = RouteScore(next_cost);
          delta_score = next_score - current_score;
          if (!sa.Accept(-delta_score)) continue;
          swap(current_order[a], current_order[b]);
          moved = true;
        } else if (type == 1 && M >= 3) {
          int a = rnd.next(M - 2);
          int b = rnd.next(a + 1, M - 1);
          int c = rnd.next(b + 1, M);
          MoveCost before = AddMoveCost(
              AddMoveCost(ConnectCost(current_order, a - 1, a),
                          ConnectCost(current_order, b - 1, b)),
              ConnectCost(current_order, c - 1, c));
          MoveCost after = AddMoveCost(
              AddMoveCost(ConnectCost(current_order, a - 1, b),
                          ConnectCost(current_order, c - 1, a)),
              ConnectCost(current_order, b - 1, c));
          delta_command = after.command - before.command;
          delta_expanded = after.expanded - before.expanded;
          MoveCost next_cost{current_cost.command + delta_command, current_cost.expanded + delta_expanded};
          double next_score = RouteScore(next_cost);
          delta_score = next_score - current_score;
          if (!sa.Accept(-delta_score)) continue;
          rotate(current_order.begin() + a, current_order.begin() + b, current_order.begin() + c);
          moved = true;
        } else if (type == 2 && M >= 2) {
          int a = rnd.next(M - 1);
          int b = rnd.next(a + 1, M);
          if (b == M) {
            delta_score = 0.0;
          } else {
            MoveCost before = AddMoveCost(
                AddMoveCost(ConnectCost(current_order, a - 1, a),
                            ConnectCost(current_order, b - 1, b)),
                ConnectCost(current_order, M - 1, M));
            MoveCost after = AddMoveCost(
                AddMoveCost(ConnectCost(current_order, a - 1, b),
                            ConnectCost(current_order, M - 1, a)),
                ConnectCost(current_order, b - 1, M));
            delta_command = after.command - before.command;
            delta_expanded = after.expanded - before.expanded;
            MoveCost next_cost{current_cost.command + delta_command, current_cost.expanded + delta_expanded};
            double next_score = RouteScore(next_cost);
            delta_score = next_score - current_score;
          }
          if (!sa.Accept(-delta_score)) continue;
          rotate(current_order.begin() + a, current_order.begin() + b, current_order.end());
          moved = true;
        } else if (type == 3) {
          int pos = rnd.next(M);
          int old_dir = current_order[pos].dir;
          int dir = rnd.next(4);
          if (old_dir == dir) continue;
          array<int, 8> edges{};
          int edge_count = 0;
          AddAffectedPosition(edges, edge_count, pos);
          MoveCost before{0, 0};
          MoveCost after{0, 0};
          REP(i, edge_count) {
            before = AddMoveCost(before, LinkCost(current_order, edges[i]));
            after = AddMoveCost(after, LinkCostAfterDirChange(current_order, edges[i], pos, dir));
          }
          delta_command = after.command - before.command;
          delta_expanded = after.expanded - before.expanded;
          MoveCost next_cost{current_cost.command + delta_command, current_cost.expanded + delta_expanded};
          double next_score = RouteScore(next_cost);
          delta_score = next_score - current_score;
          if (!sa.Accept(-delta_score)) continue;
          current_order[pos].dir = dir;
          moved = true;
        } else {
          continue;
        }
        if (!moved) continue;

        current_cost.command += delta_command;
        current_cost.expanded += delta_expanded;
        current_score += delta_score;
        if (current_score < best_score) {
          best_score = current_score;
          best_order = current_order;
        }
      }
    }

    MoveCost final_cost = PathCost(best_order);
    return DirectionOrderResult{std::move(best_order), final_cost};
  };

  struct MacroOrderCandidate {
    int macro_id = -1;
    MoveCost moves;
    int cost = INF;
    int expanded = INF;
    double score = INF;
    vector<PickupStep> order;
  };

  struct RoughMacroCandidate {
    int macro_id = -1;
    double score = INF;
    int cost = INF;
    int expanded = INF;
  };

  const int exact_macro_candidate_limit = (1LL * N * N * M > 4864 ? 32 : 64);
  vector<int> exact_macro_ids;
  exact_macro_ids.reserve(eval_macros.size());
  const bool use_command_only_prefilter =
      M > 12 && (int)eval_macros.size() > exact_macro_candidate_limit;
  if (use_command_only_prefilter) {
    vector<RoughMacroCandidate> rough_candidates;
    rough_candidates.reserve(eval_macros.size());
    REP(macro_id, (int)eval_macros.size()) {
      DirectionModel model = BuildCommandOnlyModel(eval_macros[macro_id]);
      DirectionOrderResult candidate = OptimizeByModel(model, 0.0);
      if (candidate.order.empty()) continue;
      MoveCost macro_moves = candidate.cost;
      int macro_cost = macro_moves.command + (int)eval_macros[macro_id].size() + 1 + 2 * M;
      int macro_expanded = macro_moves.expanded + expanded_fixed_cost;
      double macro_score = macro_expanded > T ? macro_expanded * 100.0 : (double)macro_cost;
      rough_candidates.push_back(RoughMacroCandidate{macro_id, macro_score, macro_cost, macro_expanded});
    }
    stable_sort(ALL(rough_candidates), [](const RoughMacroCandidate& lhs, const RoughMacroCandidate& rhs) {
      if (lhs.score != rhs.score) return lhs.score < rhs.score;
      if (lhs.cost != rhs.cost) return lhs.cost < rhs.cost;
      if (lhs.expanded != rhs.expanded) return lhs.expanded < rhs.expanded;
      return lhs.macro_id < rhs.macro_id;
    });
    if ((int)rough_candidates.size() > exact_macro_candidate_limit) {
      rough_candidates.resize(exact_macro_candidate_limit);
    }
    for (const RoughMacroCandidate& candidate : rough_candidates) {
      exact_macro_ids.push_back(candidate.macro_id);
    }
  } else {
    REP(macro_id, (int)eval_macros.size()) exact_macro_ids.push_back(macro_id);
    if (M <= 12 && (int)exact_macro_ids.size() > exact_macro_candidate_limit) {
      exact_macro_ids.resize(exact_macro_candidate_limit);
    }
  }

  vector<pair<int, DirectionModel>> macro_models;
  macro_models.reserve(exact_macro_ids.size());
  for (int macro_id : exact_macro_ids) {
    macro_models.emplace_back(macro_id, BuildModel(eval_macros[macro_id]));
  }

  const int anneal_restart_count = M > 12 ? 2 : 1;
  vector<MacroOrderCandidate> candidates;
  candidates.reserve(macro_models.size() * anneal_restart_count);

  double anneal_seconds = seconds / max(1, (int)macro_models.size());
  REP(i, (int)macro_models.size()) {
    int macro_id = macro_models[i].first;
    const DirectionModel& model = macro_models[i].second;
    REP(restart_id, anneal_restart_count) {
      DirectionOrderResult candidate = OptimizeByModel(model, anneal_seconds, restart_id);
      MoveCost macro_moves = candidate.cost;
      int macro_cost = macro_moves.command + (int)eval_macros[macro_id].size() + 1 + 2 * M;
      int macro_expanded = macro_moves.expanded + expanded_fixed_cost;
      double macro_score = macro_expanded > T ? macro_expanded * 100.0 : (double)macro_cost;
      if (candidate.order.empty()) continue;
      candidates.push_back(MacroOrderCandidate{
          macro_id, macro_moves, macro_cost, macro_expanded, macro_score, std::move(candidate.order)});
    }
  }
  stable_sort(ALL(candidates), [](const MacroOrderCandidate& lhs, const MacroOrderCandidate& rhs) {
    if (lhs.score != rhs.score) return lhs.score < rhs.score;
    if (lhs.cost != rhs.cost) return lhs.cost < rhs.cost;
    if (lhs.expanded != rhs.expanded) return lhs.expanded < rhs.expanded;
    return lhs.macro_id < rhs.macro_id;
  });
  assert(!candidates.empty());

  vector<InitialOrderPlan> results;
  results.reserve(candidates.size());
  for (const MacroOrderCandidate& candidate : candidates) {
    vector<int> order;
    order.reserve(M);
    for (PickupStep step : candidate.order) order.push_back(step.task);
    assert((int)order.size() == M);
    results.emplace_back(eval_macros[candidate.macro_id], std::move(order), candidate.cost);
  }

  return results;
}

struct PriorityQueue {
  static constexpr int MAX_FULL_NODES =
      MAX_N * MAX_N * 4 * (2 * MAX_M + 1) * 2 * MAX_MACRO_STATES;
  static constexpr int MAX_ITEMS = MAX_FULL_NODES * 8;
  static constexpr int MAX_BUCKETS = MAX_T + 2;

  int bucket_count = 0;
  int current_cost = 0;
  int item_count = 0;
  int next_unused = 0;
  int free_head = -1;
  array<int, MAX_BUCKETS> bucket_head{};
  array<int, MAX_ITEMS> next_item{};
  array<uint64_t, MAX_ITEMS> items{};

  PriorityQueue() = default;
  PriorityQueue(int max_edge_cost) {
    Reset(max_edge_cost);
  }

  void Reset(int max_edge_cost) {
    bucket_count = max_edge_cost + 1;
    current_cost = 0;
    item_count = 0;
    next_unused = 0;
    free_head = -1;
    fill(bucket_head.begin(), bucket_head.begin() + bucket_count, -1);
  }

  bool empty() const {
    return item_count == 0;
  }

  void push(Label label, int node) {
    uint64_t item = (uint64_t(label) << 32) | uint32_t(node);
    int item_index;
    if (free_head != -1) {
      item_index = free_head;
      free_head = next_item[free_head];
    } else {
      item_index = next_unused++;
    }
    items[item_index] = item;
    int bucket_index = LabelCost(label) % bucket_count;
    next_item[item_index] = bucket_head[bucket_index];
    bucket_head[bucket_index] = item_index;
    item_count++;
  }

  template <class LabelArray>
  pair<int, int> pop(const LabelArray& labels) {
    while (item_count > 0) {
      int bucket_index = current_cost % bucket_count;
      while (bucket_head[bucket_index] != -1) {
        int item_index = bucket_head[bucket_index];
        bucket_head[bucket_index] = next_item[item_index];
        uint64_t item = items[item_index];
        next_item[item_index] = free_head;
        free_head = item_index;
        item_count--;
        int node = int(uint32_t(item));
        Label pushed_label = Label(item >> 32);
        if (labels[node] == pushed_label) return {LabelCost(pushed_label), node};
      }
      current_cost++;
    }
    return {INF, -1};
  }
};

PriorityQueue GlobalPriorityQueue;

struct Solver {
  vector<vector<Op>> macros;
  vector<int> order;
  vector<Op> answer;
  int macro_count = 0;
  int no_macro = 0;
  int macro_state_count = 0;
  int action_count = 0;
  int spatial_count = 0;
  int normal_base_count = 0;
  int base_count = 0;
  int max_edge_cost = 1;

  int Cost[MAX_MACRO_STATES][MAX_ACTIONS];
  int BasicCost[MAX_MACRO_STATES][MAX_ACTIONS];
  vector<Op> ToMacro[MAX_MACRO_STATES][MAX_ACTIONS];
  vector<int> SwapNextBase;
  vector<int> MacroNextSpatial;
  vector<int> MacroNextBase;
  vector<int> MacroFullIndex;
  vector<int> NextBasicBase;
  vector<int> NextMacroNode;
  bool swap_next_valid = false;

  int completed = 0;
  int expanded_basic_len = 0;

  Solver() = default;

  void SetMacros(const vector<vector<Op>>& macros_) {
    macros = macros_;
    macro_count = (int)macros.size();
    assert(macro_count <= MAX_MACROS);
    no_macro = macro_count;
    macro_state_count = macro_count + 1;
    action_count = BASIC_ACTIONS + macro_count;
    spatial_count = N * N * 4;
    int next_normal_base_count = spatial_count * (2 * M + 1);
    int next_base_count = next_normal_base_count * 2;
    if (base_count != next_base_count) {
      normal_base_count = next_normal_base_count;
      base_count = next_base_count;
      SwapNextBase.clear();
      swap_next_valid = false;
    } else {
      normal_base_count = next_normal_base_count;
    }
    MacroNextSpatial.clear();
    MacroNextBase.clear();
    MacroFullIndex.clear();
    NextBasicBase.clear();
    NextMacroNode.clear();
    answer.clear();
    completed = 0;
    expanded_basic_len = 0;

    BuildCommands();
  }

  void SetOrderData(const vector<int>& order_) {
    assert((int)order_.size() == M);
    if (order != order_) {
      order = order_;
      swap_next_valid = false;
    }
  }

  bool SetOrder(const vector<int>& order_) {
    assert(macro_state_count > 0);
    SetOrderData(order_);
    return BuildNextNode();
  }

  int BaseNodeId(int spatial, int complete) const {
    return complete * spatial_count + spatial;
  }

  bool IsRepairPending(int base_node) const {
    return base_node >= normal_base_count;
  }

  int NormalOf(int base_node) const {
    return IsRepairPending(base_node) ? base_node - normal_base_count : base_node;
  }

  int RepairBaseNodeId(int base_node) const {
    return NormalOf(base_node) + normal_base_count;
  }

  int SpatialOf(int base_node) const {
    return NormalOf(base_node) % spatial_count;
  }

  int CompleteOf(int base_node) const {
    return NormalOf(base_node) / spatial_count;
  }

  int FullNodeId(int base_node, int macro_id) const {
    return base_node * macro_state_count + macro_id;
  }

  bool CellHasBlockingBall(int cell, int complete) const {
    int order_pos = complete / 2;
    if ((complete & 1) == 0) {
      for (int i = order_pos; i < M; i++) if (Ball[order[i]] == cell) return true;
    } else {
      for (int i = order_pos + 1; i < M; i++) if (Ball[order[i]] == cell) return true;
    }
    for (int i = 0; i < order_pos; i++) if (Bascket[order[i]] == cell) return true;
    return false;
  }

  bool CellHasBasket(int cell) const {
    REP(i, M) if (Bascket[i] == cell) return true;
    return false;
  }

  int NextBySwap(int base) const {
    if (IsRepairPending(base)) return NormalOf(base);

    int complete = CompleteOf(base);
    int spatial = SpatialOf(base);

    int cell = CellOfSpatial(spatial);
    if (complete == 2 * M) {
      if (CellHasBasket(cell)) return RepairBaseNodeId(base);
      return BaseNodeId(spatial, complete);
    }

    int task = order[complete / 2];
    if ((complete & 1) == 0) {
      if (cell == Ball[task]) return BaseNodeId(spatial, complete + 1);
      if (CellHasBlockingBall(cell, complete)) return RepairBaseNodeId(base);
      return BaseNodeId(spatial, complete);
    } else {
      if (cell == Bascket[task]) return BaseNodeId(spatial, complete + 1);
      return RepairBaseNodeId(base);
    }
  }

  int NextNodeAt(int base, int edge) const {
    assert(edge < action_count);
    int complete = CompleteOf(base);
    int spatial = SpatialOf(base);
    if (edge < 3) {
      int next_base = BaseNodeId(init_graph[spatial].dest[edge], complete);
      if (!IsRepairPending(base)) return next_base;
      if (edge == 0) return -1;
      return RepairBaseNodeId(next_base);
    }
    if (edge == 3) return SwapNextBase[base];
    int target_macro = edge - BASIC_ACTIONS;
    int full_index = MacroFullIndex[target_macro];
    if (full_index != -1) return MacroNextBase[full_index * base_count + base];
    if (IsRepairPending(base)) return -1;
    return BaseNodeId(MacroNextSpatial[target_macro * spatial_count + spatial], complete);
  }

  bool MatchMacro(int current_macro, const vector<Op>& target, int pos) const {
    if (current_macro == no_macro) return false;
    const vector<Op>& cur = macros[current_macro];
    if (pos + (int)cur.size() > (int)target.size()) return false;
    REP(i, (int)cur.size()) {
      if (cur[i].value != target[pos + i].value) return false;
    }
    return true;
  }

  void BuildInsideMacroCommand(int current_macro, int target_macro, vector<Op>& out,
                               vector<int>& dp, vector<int>& prev, vector<Op>& prev_op) const {
    const vector<Op>& target = macros[target_macro];
    int len = (int)target.size();
    if (current_macro == no_macro) {
      out.insert(out.end(), ALL(target));
      return;
    }

    const vector<Op>& cur_macro = macros[current_macro];
    int cur_len = (int)cur_macro.size();
    dp.assign(len + 1, INF);
    prev.assign(len + 1, -1);
    prev_op.assign(len + 1, Op::F);
    dp[0] = 0;

    REP(pos, len) {
      if (dp[pos] == INF) continue;
      int next_cost = dp[pos] + 1;
      if (next_cost < dp[pos + 1]) {
        dp[pos + 1] = next_cost;
        prev[pos + 1] = pos;
        prev_op[pos + 1] = target[pos];
      }
      if (pos + cur_len <= len) {
        bool match = true;
        REP(i, cur_len) {
          if (cur_macro[i].value != target[pos + i].value) {
            match = false;
            break;
          }
        }
        if (!match) continue;
        int next_pos = pos + cur_len;
        if (next_cost < dp[next_pos]) {
          dp[next_pos] = next_cost;
          prev[next_pos] = pos;
          prev_op[next_pos] = Op::P;
        }
      }
    }

    int output_begin = (int)out.size();
    for (int cur = len; cur > 0; cur = prev[cur]) {
      assert(prev[cur] != -1);
      out.push_back(prev_op[cur]);
    }
    reverse(out.begin() + output_begin, out.end());
  }

  void BuildCommands() {
    REP(i, macro_state_count) REP(j, action_count) {
      Cost[i][j] = INF;
      BasicCost[i][j] = INF;
      ToMacro[i][j].clear();
    }

    max_edge_cost = 1;
    REP(cur_macro, macro_state_count) REP(edge, BASIC_ACTIONS) {
      Cost[cur_macro][edge] = 1;
      BasicCost[cur_macro][edge] = 1;
      ToMacro[cur_macro][edge].emplace_back(EdgeOp(edge));
    }

    vector<int> dp, prev;
    vector<Op> prev_op;
    REP(cur_macro, macro_state_count) REP(target_macro, macro_count) {
      int edge = BASIC_ACTIONS + target_macro;
      vector<Op>& command = ToMacro[cur_macro][edge];
      int target_len = (int)macros[target_macro].size();
      BasicCost[cur_macro][edge] = target_len;
      if (cur_macro == target_macro) {
        Cost[cur_macro][edge] = 1;
        command.emplace_back(Op::P);
      } else {
        command.reserve(target_len + 2);
        command.emplace_back(Op::M);
        BuildInsideMacroCommand(cur_macro, target_macro, command, dp, prev, prev_op);
        command.emplace_back(Op::M);
        Cost[cur_macro][edge] = (int)command.size();
        chmax(max_edge_cost, Cost[cur_macro][edge]);
      }
    }
  }

  bool BuildNextNode() {
    if (!timer.within()) return false;

    if (!swap_next_valid) {
      SwapNextBase.assign(base_count, -1);
      REP(base, base_count) {
        if ((base & 1023) == 0 && !timer.within()) return false;
        SwapNextBase[base] = NextBySwap(base);
      }
      swap_next_valid = true;
    }
    if (!timer.within()) return false;

    MacroFullIndex.assign(macro_count, -1);
    int full_macro_count = 0;
    REP(target_macro, macro_count) {
      bool has_swap = false;
      for (Op op : macros[target_macro]) {
        if (op.value == Op::S) {
          has_swap = true;
          break;
        }
      }
      if (has_swap) MacroFullIndex[target_macro] = full_macro_count++;
    }

    if (!timer.within()) return false;
    MacroNextSpatial.assign(spatial_count * macro_count, -1);
    if (!timer.within()) return false;
    MacroNextBase.assign(base_count * full_macro_count, -1);
    NextBasicBase.clear();
    NextMacroNode.clear();

    REP(target_macro, macro_count) REP(spatial, spatial_count) {
      if ((spatial & 255) == 0 && !timer.within()) return false;
      if (MacroFullIndex[target_macro] != -1) continue;
      int cur = spatial;
      for (Op op : macros[target_macro]) {
        int edge = BasicEdgeIndex(op);
        cur = init_graph[cur].dest[edge];
      }
      MacroNextSpatial[target_macro * spatial_count + spatial] = cur;
    }

    REP(target_macro, macro_count) {
      int full_index = MacroFullIndex[target_macro];
      if (full_index == -1) continue;
      int offset = full_index * base_count;
      REP(base, base_count) {
        if ((base & 1023) == 0 && !timer.within()) return false;
        int cur = base;
        for (Op op : macros[target_macro]) {
          if (cur == -1) break;
          int edge = BasicEdgeIndex(op);
          cur = NextNodeAt(cur, edge);
        }
        MacroNextBase[offset + base] = cur;
      }
    }

    if (!timer.within()) return false;
    NextBasicBase.assign((size_t)base_count * BASIC_ACTIONS, -1);
    NextMacroNode.assign((size_t)base_count * macro_count, -1);
    REP(base, base_count) {
      if ((base & 1023) == 0 && !timer.within()) return false;
      int basic_offset = base * BASIC_ACTIONS;
      REP(edge, BASIC_ACTIONS) NextBasicBase[basic_offset + edge] = NextNodeAt(base, edge);
      int macro_offset = base * macro_count;
      REP(target_macro, macro_count) {
        int next_base = NextNodeAt(base, BASIC_ACTIONS + target_macro);
        if (next_base != -1) {
          NextMacroNode[macro_offset + target_macro] = FullNodeId(next_base, target_macro);
        }
      }
    }
    return true;
  }

  int RestoreEdge(int from, int to) const {
    int from_base = from / macro_state_count;
    int from_macro = from % macro_state_count;
    int to_base = to / macro_state_count;
    int to_macro = to % macro_state_count;
    int basic_offset = from_base * BASIC_ACTIONS;
    int macro_offset = from_base * macro_count;

    if (from_macro != to_macro) {
      int edge = BASIC_ACTIONS + to_macro;
      assert(edge < action_count);
      assert(NextMacroNode[macro_offset + to_macro] == to);
      return edge;
    }

    REP(edge, BASIC_ACTIONS) {
      if (NextBasicBase[basic_offset + edge] == to_base) return edge;
    }

    if (to_macro < macro_count) {
      int edge = BASIC_ACTIONS + to_macro;
      if (NextMacroNode[macro_offset + to_macro] == to) return edge;
    }

    assert(false);
    return -1;
  }

  vector<Op> RestoreAnswer(int start, int goal, const vector<int>& prev) const {
    vector<pair<int, int>> steps;
    for (int cur = goal; cur != start; cur = prev[cur]) {
      assert(cur != -1);
      int p = prev[cur];
      assert(p != -1);
      steps.push_back({p % macro_state_count, RestoreEdge(p, cur)});
    }
    reverse(ALL(steps));

    vector<Op> ret;
    for (auto [macro_id, edge] : steps) {
      const vector<Op>& command = ToMacro[macro_id][edge];
      ret.insert(ret.end(), ALL(command));
    }
    return ret;
  }

  void Run() {
    assert(macro_state_count > 0);
    assert((int)order.size() == M);
    assert(!SwapNextBase.empty());
    assert(macro_count == 0 || !MacroFullIndex.empty());
    assert(!NextBasicBase.empty());
    assert(macro_count == 0 || !NextMacroNode.empty());
    answer.clear();
    completed = 0;
    expanded_basic_len = 0;

    int start_base = BaseNodeId(NodeId(0, 0, 0), 0);
    int start = FullNodeId(start_base, no_macro);
    int total = base_count * macro_state_count;
    int max_total = N * N * 4 * (2 * M + 1) * 2 * (MAX_MACROS + 1);
    assert(total <= max_total);

    static vector<Label> label_buffer;
    static vector<int> prev_buffer;
    if ((int)label_buffer.size() < max_total + 1) {
      label_buffer.resize(max_total + 1);
      prev_buffer.resize(max_total);
    }
    vector<Label>& labels = label_buffer;
    vector<int>& prev = prev_buffer;

    fill(labels.begin(), labels.begin() + total + 1, LABEL_INF);
    PriorityQueue& que = GlobalPriorityQueue;
    que.Reset(max_edge_cost);

    labels[start] = PackLabel(0, 0);
    que.push(labels[start], start);

    int goal = -1;
    while (true) {
      auto [cost, node] = que.pop(labels);
      if (node == -1) break;

      int base = node / macro_state_count;
      int macro_id = node % macro_state_count;
      if (!IsRepairPending(base) && CompleteOf(base) == 2 * M) {
        goal = node;
        break;
      }

      if (!timer.within()) {
        completed = 0;
        expanded_basic_len = INF;
        return;
      }
      const int current_basic = LabelBasic(labels[node]);
      const int* next_basic_row = NextBasicBase.data() + base * BASIC_ACTIONS;
      const int* next_macro_node_row =
          macro_count == 0 ? nullptr : NextMacroNode.data() + base * macro_count;
      auto relax_node = [&](int next_node, int add_cost, int add_basic) {
        if (next_node == -1) return;
        int next_cost = cost + add_cost;
        int next_basic = current_basic + add_basic;
        if (next_cost > T || next_basic > T) return;

        Label next_label = PackLabel(next_cost, next_basic);
        if (next_label < labels[next_node]) {
          labels[next_node] = next_label;
          prev[next_node] = node;
          que.push(next_label, next_node);
        }
      };
      auto relax_to = [&](int next_base, int next_macro, int add_cost, int add_basic) {
        if (next_base == -1) return;
        relax_node(FullNodeId(next_base, next_macro), add_cost, add_basic);
      };

      REP(edge, BASIC_ACTIONS) relax_to(next_basic_row[edge], macro_id, 1, 1);
      REP(target_macro, macro_count) {
        int edge = BASIC_ACTIONS + target_macro;
        relax_node(next_macro_node_row[target_macro], Cost[macro_id][edge], BasicCost[macro_id][edge]);
      }
    }

    if (goal == -1) {
      int best_placed = -1;
      REP(node, total) {
        if (labels[node] == LABEL_INF) continue;
        int base = node / macro_state_count;
        if (IsRepairPending(base)) continue;
        int placed = CompleteOf(base) / 2;
        if (goal == -1 || placed > best_placed || (placed == best_placed && LabelCost(labels[node]) < LabelCost(labels[goal]))) {
          best_placed = placed;
          goal = node;
        }
      }
    }

    completed = CompleteOf(goal / macro_state_count) / 2;
    expanded_basic_len = LabelBasic(labels[goal]);
    answer = RestoreAnswer(start, goal, prev);
  }
};

namespace macro_refinement {

constexpr int TARGET_MACRO_COUNT = 6;
constexpr int MAX_ITERATION = 1;
constexpr int SUBSTRING_MIN_LENGTH = 4;
constexpr double REBUILD_START_LIMIT = 1.95;

bool IsBasicOp(Op op) {
  return op.value == Op::F || op.value == Op::L || op.value == Op::R || op.value == Op::S;
}

string OpsKey(const vector<Op>& ops) {
  string ret;
  ret.reserve(ops.size());
  for (Op op : ops) {
    assert(IsBasicOp(op));
    ret.push_back(op.Char());
  }
  return ret;
}

struct SimulationResult {
  vector<vector<Op>> registered_macros;
  string expanded;
};

SimulationResult SimulateCommands(const vector<Op>& commands) {
  SimulationResult result;
  vector<Op> recording_macro;
  bool recording = false;

  auto append_basic_sequence = [&](const vector<Op>& ops) {
    for (Op op : ops) {
      assert(IsBasicOp(op));
      result.expanded.push_back(op.Char());
      if (recording) recording_macro.push_back(op);
    }
  };

  for (Op op : commands) {
    if (op.value == Op::M) {
      if (!recording) {
        recording = true;
        recording_macro.clear();
      } else {
        recording = false;
        result.registered_macros.push_back(recording_macro);
      }
    } else if (op.value == Op::P) {
      if (!result.registered_macros.empty()) {
        append_basic_sequence(result.registered_macros.back());
      }
    } else {
      assert(IsBasicOp(op));
      result.expanded.push_back(op.Char());
      if (recording) recording_macro.push_back(op);
    }
  }

  return result;
}

bool AddMacroString(vector<vector<Op>>& macros, unordered_set<string>& seen, const string& macro) {
  if (macro.empty()) return false;
  if ((int)macros.size() >= TARGET_MACRO_COUNT) return false;
  if (!seen.insert(macro).second) return false;
  macros.push_back(Ops(macro));
  return true;
}

bool AddMacroOps(vector<vector<Op>>& macros, unordered_set<string>& seen, const vector<Op>& macro) {
  if (macro.empty()) return false;
  if ((int)macros.size() >= TARGET_MACRO_COUNT) return false;
  string key = OpsKey(macro);
  if (!seen.insert(key).second) return false;
  macros.push_back(macro);
  return true;
}

string MostFrequentSubstring(const string& expanded, int len, const unordered_set<string>& seen,
                             const unordered_set<string>& forbidden) {
  if ((int)expanded.size() < len) return "";
  struct Count {
    int count = 0;
    int first_pos = INF;
  };
  unordered_map<string, Count> counts;
  counts.reserve(expanded.size());

  for (int pos = 0; pos + len <= (int)expanded.size(); pos++) {
    string sub = expanded.substr(pos, len);
    if (seen.count(sub)) continue;
    if (forbidden.count(sub)) continue;
    Count& count = counts[sub];
    if (count.count == 0) count.first_pos = pos;
    count.count++;
  }

  string best;
  int best_count = -1;
  int best_first_pos = INF;
  for (const auto& [sub, count] : counts) {
    if (count.count > best_count || (count.count == best_count && count.first_pos < best_first_pos)) {
      best = sub;
      best_count = count.count;
      best_first_pos = count.first_pos;
    }
  }
  return best;
}

vector<vector<Op>> BuildMacrosFromAnswer(const vector<Op>& answer,
                                         const vector<vector<Op>>& previous_macros) {
  SimulationResult simulation = SimulateCommands(answer);
  vector<vector<Op>> macros;
  unordered_set<string> seen;
  seen.reserve(TARGET_MACRO_COUNT * 2);
  unordered_set<string> registered;
  registered.reserve(simulation.registered_macros.size() * 2 + 1);

  for (const vector<Op>& registered_macro : simulation.registered_macros) {
    string key = OpsKey(registered_macro);
    registered.insert(key);
    AddMacroString(macros, seen, key);
  }

  if (!previous_macros.empty()) {
    const vector<Op>& single_macro = previous_macros[0];
    auto AddExtended = [&](Op op, bool prefix, bool suffix) {
      if ((int)macros.size() >= TARGET_MACRO_COUNT) return;
      vector<Op> extended;
      extended.reserve(single_macro.size() + (prefix ? 1 : 0) + (suffix ? 1 : 0));
      if (prefix) extended.push_back(op);
      extended.insert(extended.end(), ALL(single_macro));
      if (suffix) extended.push_back(op);
      AddMacroOps(macros, seen, extended);
    };

    AddExtended(Op::S, true, false);
    AddExtended(Op::S, false, true);
    AddExtended(Op::S, true, true);
    AddExtended(Op::F, true, false);
    AddExtended(Op::F, false, true);
    // for (Op op : {Op(Op::L), Op(Op::R)}) {
    //   AddExtended(op, true, false);
    //   AddExtended(op, false, true);
    // }
  }

  unordered_set<string> forbidden;
  forbidden.reserve(previous_macros.size() * 2 + 1);
  for (const vector<Op>& previous_macro : previous_macros) {
    string key = OpsKey(previous_macro);
    if (!registered.count(key)) forbidden.insert(key);
  }

  for (int len = SUBSTRING_MIN_LENGTH;
       (int)macros.size() < TARGET_MACRO_COUNT && len <= (int)simulation.expanded.size(); len++) {
    string macro = MostFrequentSubstring(simulation.expanded, len, seen, forbidden);
    AddMacroString(macros, seen, macro);
  }

  return macros;
}

int AnswerScore(int completed, int answer_size) {
  if (completed == M) return answer_size;
  return T * (M - completed);
}

} // namespace macro_refinement

namespace order_hill_climb {

constexpr int MAX_ITERATION = 1200;
constexpr double START_LIMIT = 1.95;

void ApplyNeighbor(vector<int>& order) {
  if (M <= 1) return;
  int type = rnd.next(3);
  int a = rnd.next(M);
  int b = rnd.next(M);
  if (a == b) return;
  if (a > b) swap(a, b);

  if (type == 0) {
    swap(order[a], order[b]);
  } else if (type == 1) {
    int c = rnd.next(M);
    if (c < b) swap(c, b);
    if (b < a) swap(b, a);
    if (b == c) return;
    rotate(order.begin() + a, order.begin() + b, order.begin() + c);
  } else {
    rotate(order.begin() + a, order.begin() + b, order.end());
  }
}

void Optimize(Solver& solver, vector<Op>& best, int& completed, int& expanded_basic_len, int& best_score) {
  if (M <= 1) return;

  vector<int> current_order = solver.order;
  int current_score = macro_refinement::AnswerScore(solver.completed, (int)solver.answer.size());

  int iteration = 0;
  while (iteration < MAX_ITERATION && timer.elapsed() < START_LIMIT) {
    vector<int> candidate_order = current_order;
    ApplyNeighbor(candidate_order);
    if (candidate_order == current_order) continue;

    if (!solver.SetOrder(candidate_order)) break;
    solver.Run();
    iteration++;

    int candidate_score = macro_refinement::AnswerScore(solver.completed, (int)solver.answer.size());
    if (candidate_score < current_score) {
      current_order = std::move(candidate_order);
      current_score = candidate_score;

      if (candidate_score < best_score) {
        best_score = candidate_score;
        best = solver.answer;
        completed = solver.completed;
        expanded_basic_len = solver.expanded_basic_len;

        vector<vector<Op>> next_macros = macro_refinement::BuildMacrosFromAnswer(solver.answer, solver.macros);
        solver.SetMacros(next_macros);
      }
    }
  }

}

} // namespace order_hill_climb

vector<vector<Op>> BuildSingleMacroSet(const vector<Op>& macro) {
  constexpr int SINGLE_MACRO_SET_LIMIT = 6;
  struct Candidate {
    vector<Op> ops;
    int score = 0;
    int len = 0;
    int order = 0;
    bool original = false;
  };

  vector<Candidate> candidates;
  unordered_set<string> seen;
  auto OpsKey = [](const vector<Op>& ops) {
    string key;
    key.reserve(ops.size());
    for (Op op : ops) key.push_back(op.Char());
    return key;
  };

  int order_index = 0;
  auto AddCandidate = [&](vector<Op>&& candidate, int count, bool original) {
    if (candidate.empty()) return;
    string key = OpsKey(candidate);
    if (!seen.insert(key).second) return;
    int len = (int)candidate.size();
    int score = original ? INF : (count - 1) * (len - 1);
    candidates.push_back({std::move(candidate), score, len, order_index++, original});
  };

  AddCandidate(vector<Op>(ALL(macro)), 1, true);

  int len = (int)macro.size();
  for (int sub_len = len - 1; sub_len >= 2; sub_len--) {
    for (int start = 0; start + sub_len <= len; start++) {
      vector<Op> sub(macro.begin() + start, macro.begin() + start + sub_len);
      string key = OpsKey(sub);
      if (seen.count(key)) continue;

      int count = 0;
      int next_available = 0;
      for (int pos = 0; pos + sub_len <= len; pos++) {
        if (pos < next_available) continue;
        bool match = true;
        REP(i, sub_len) {
          if (macro[pos + i].value != sub[i].value) {
            match = false;
            break;
          }
        }
        if (!match) continue;
        count++;
        next_available = pos + sub_len;
      }
      if (count >= 2) AddCandidate(std::move(sub), count, false);
    }
  }

  sort(ALL(candidates), [](const Candidate& lhs, const Candidate& rhs) {
    if (lhs.original != rhs.original) return lhs.original;
    if (lhs.score != rhs.score) return lhs.score > rhs.score;
    if (lhs.len != rhs.len) return lhs.len > rhs.len;
    return lhs.order < rhs.order;
  });
  if ((int)candidates.size() > SINGLE_MACRO_SET_LIMIT) candidates.resize(SINGLE_MACRO_SET_LIMIT);

  vector<vector<Op>> ret;
  ret.reserve(candidates.size());
  for (Candidate& candidate : candidates) {
    ret.push_back(std::move(candidate.ops));
  }
  return ret;
}

int main() {
  SetUp();
  const bool use_basket_to_ball = T <= 3861;
  vector<vector<Op>> macros =
      use_basket_to_ball ? GetMacros<1>() : GetMacros<0>();

  Solver solver;

  auto initial_orders = PlanInitialOrder(0.10, macros);
  assert(!initial_orders.empty());

  double after_plan_init = timer.elapsed();
  LOG(after_plan_init);

  constexpr int INITIAL_ORDER_CANDIDATE_COUNT = 120;
  struct SingleMacroCandidate {
    int score = INF;
    int initial_index = -1;
    int macro_length = 0;
    vector<int> order;
    vector<Op> answer;
    vector<vector<Op>> macros;
    int completed = 0;
    int expanded_basic_len = INF;
  };
  struct SingleMacroCandidateCompare {
    bool operator()(const SingleMacroCandidate& lhs, const SingleMacroCandidate& rhs) const {
      if (lhs.score != rhs.score) return lhs.score > rhs.score;
      return lhs.initial_index > rhs.initial_index;
    }
  };

  int initial_try_count = 0;
  priority_queue<SingleMacroCandidate, vector<SingleMacroCandidate>, SingleMacroCandidateCompare> refine_queue;
  int selected_initial_order = -1;
  int selected_macro_length = 0;
  vector<int> best_order;
  vector<vector<Op>> best_macros;
  vector<Op> best;
  int completed = 0;
  int expanded_basic_len = INF;
  int best_score = INF;
  bool solver_ready = true;
  Solver single_macro_solver;

  auto UpdateBestByCandidate = [&](const SingleMacroCandidate& candidate) {
    if (candidate.score < best_score) {
      selected_initial_order = candidate.initial_index;
      selected_macro_length = candidate.macro_length;
      best_order = candidate.order;
      best_macros = candidate.macros;
      best = candidate.answer;
      completed = candidate.completed;
      expanded_basic_len = candidate.expanded_basic_len;
      best_score = candidate.score;
    }
  };

  auto RefineOneCandidate = [&]() {
    if (!solver_ready || refine_queue.empty()) return false;
    if (timer.elapsed() >= macro_refinement::REBUILD_START_LIMIT) return false;

    SingleMacroCandidate candidate = refine_queue.top();
    refine_queue.pop();
    vector<Op> current_answer = candidate.answer;
    vector<vector<Op>> previous_macros = candidate.macros;
    solver.SetOrderData(candidate.order);

    REP(_, macro_refinement::MAX_ITERATION) {
      if (timer.elapsed() >= macro_refinement::REBUILD_START_LIMIT) break;
      vector<vector<Op>> next_macros = macro_refinement::BuildMacrosFromAnswer(current_answer, previous_macros);
      if (next_macros.empty()) break;
      solver.SetMacros(next_macros);
      solver.SetOrderData(candidate.order);
      if (!solver.BuildNextNode()) {
        solver_ready = false;
        return false;
      }
      solver.Run();
      current_answer = solver.answer;
      previous_macros = next_macros;

      int current_score = macro_refinement::AnswerScore(solver.completed, (int)solver.answer.size());
      if (current_score < best_score) {
        selected_initial_order = candidate.initial_index;
        selected_macro_length = candidate.macro_length;
        best_order = candidate.order;
        best_macros = next_macros;
        best_score = current_score;
        best = solver.answer;
        completed = solver.completed;
        expanded_basic_len = solver.expanded_basic_len;
      }
    }
    return solver_ready;
  };

  REP(i, initial_orders.size()) {
    if (initial_try_count >= INITIAL_ORDER_CANDIDATE_COUNT) break;

    const vector<Op>& initial_macro = get<0>(initial_orders[i]);
    const vector<int>& initial_order = get<1>(initial_orders[i]);
    int initial_cost = get<2>(initial_orders[i]);

    vector<vector<Op>> single_macros = BuildSingleMacroSet(initial_macro);
    single_macro_solver.SetMacros(single_macros);
    if (!single_macro_solver.SetOrder(initial_order)) break;
    single_macro_solver.Run();

    int single_macro_score = macro_refinement::AnswerScore(
        single_macro_solver.completed, (int)single_macro_solver.answer.size());

    SingleMacroCandidate candidate;
    candidate.score = single_macro_score;
    candidate.initial_index = i;
    candidate.macro_length = (int)initial_macro.size();
    candidate.order = initial_order;
    candidate.answer = single_macro_solver.answer;
    candidate.macros = single_macros;
    candidate.completed = single_macro_solver.completed;
    candidate.expanded_basic_len = single_macro_solver.expanded_basic_len;
    UpdateBestByCandidate(candidate);

    bool found_by_single_macro = single_macro_solver.completed == M
                              && single_macro_solver.expanded_basic_len <= T
                              && (int)single_macro_solver.answer.size() <= T;
    if (!found_by_single_macro) {
      if (initial_try_count % 2 == 1) RefineOneCandidate();
      continue;
    }

    if (single_macro_score <= initial_cost) initial_try_count++;

    candidate.macros = std::move(single_macros);
    refine_queue.push(std::move(candidate));

    if (initial_try_count % 2 == 1) RefineOneCandidate();
  }
  if (selected_initial_order < 0) {
    selected_initial_order = 0;
    if (!initial_orders.empty()) {
      selected_macro_length = (int)get<0>(initial_orders[0]).size();
      best_order = get<1>(initial_orders[0]);
    } else {
      selected_macro_length = 0;
      best_order.resize(M);
      iota(ALL(best_order), 0);
    }
    best_macros.clear();
    best.clear();
    completed = 0;
    expanded_basic_len = 0;
    best_score = macro_refinement::AnswerScore(completed, (int)best.size());
    solver_ready = false;
  }

  LOG(initial_try_count);

  while (RefineOneCandidate()) {}

  if (solver_ready) {
    solver.SetMacros(best_macros);
    solver.SetOrderData(best_order);
    solver.answer = best;
    solver.completed = completed;
    solver.expanded_basic_len = expanded_basic_len;
    order_hill_climb::Optimize(solver, best, completed, expanded_basic_len, best_score);
  }

  LOG(selected_macro_length);
  LOG(selected_initial_order);
  LOG(completed);
  LOG(best.size());
  LOG(expanded_basic_len);
  LOG(best_score);

  for (Op op : best) cout << op.Char() << '\n';
  cout.flush();
  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
}
