#if defined(__x86_64__)
#pragma GCC target("arch=skylake-avx512")
#pragma GCC optimize("O3","inline")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,avx2,fma,avx512f")
#endif

// #include <atcoder/dsu>
#include <atcoder/segtree>
// #include <atcoder/mincostflow>

#if defined(__x86_64__)
#include <immintrin.h>
#endif

#define FINAL_SUBMIT 1

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <climits>
#include <complex>
#include <cstdio>
#include <cstdlib>
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
  static const int64_t CYCLES_PER_SEC = 1200000000;
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

Timer timer(1.98);
XorShift rnd;

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0xFF;
  constexpr static double START_TEMP = 0.00150;
  constexpr static double END_TEMP   = 0.00001;
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

template <typename ScoreT>
struct BeamSearchManager {
  struct MinMax {
    ScoreT max_score, min_score;
    int max_index, min_index;
  };
  static MinMax id() {
    return MinMax{ -std::numeric_limits<ScoreT>::max(), std::numeric_limits<ScoreT>::max(), -1, -1 };
  }
  static MinMax op(MinMax a, MinMax b) {
    if (b.max_score > a.max_score) {
      a.max_score = b.max_score;
      a.max_index = b.max_index;
    }
    if (b.min_score < a.min_score) {
      a.min_score = b.min_score;
      a.min_index = b.min_index;
    }
    return a;
  }

  atcoder::segtree<MinMax, op, id> seg;

  BeamSearchManager() {}
  void Init(int beam_width, ScoreT init) {
    std::vector<MinMax> vec(beam_width);
    for (int i = 0; i < beam_width; i++) {
      vec[i] = MinMax{init, init, i, i};
    }
    seg = atcoder::segtree<MinMax, op, id>(vec);
  }
  inline int max_index() { return seg.all_prod().max_index; }
  inline int min_index() { return seg.all_prod().min_index; }
  inline ScoreT max_score() { return seg.all_prod().max_score; }
  inline ScoreT min_score() { return seg.all_prod().min_score; }
  inline void update(int i, ScoreT score) { seg.set(i, MinMax{score, score, i, i}); }
};

using namespace std;

constexpr int FIXED_N = 20;
constexpr int MAX_CELLS = FIXED_N * FIXED_N;
constexpr int EXIT_R = 0;
constexpr int MAX_CANDIDATE_CONVEYORS = 2 * FIXED_N;
constexpr int COST_INF = 30000;

struct Cell {
  int r, j;
};

struct Conveyor {
  vector<Cell> cells;
};

struct Operation {
  int id, dir;
};

struct Move {
  int to;
  Operation op;
};

int N;
int total_cells;
int exit_cell;
array<int, MAX_CELLS> initial_board;
array<int, MAX_CELLS> initial_pos;

int CellId(int r, int j) {
  return r * N + j;
}

int CellId(Cell c) {
  return CellId(c.r, c.j);
}

void SetUp() {
  cin >> N;
  total_cells = N * N;
  exit_cell = CellId(EXIT_R, N / 2);
  initial_pos.fill(-1);
  REP(i, N) REP(j, N) {
    int a;
    cin >> a;
    int v = CellId(i, j);
    initial_board[v] = a;
    initial_pos[a] = v;
  }
  LOG(N);
}

struct Solver {
  using CostRow = array<short, MAX_CELLS>;
  using ActionRow = array<signed char, MAX_CELLS>;

  struct State {
    array<int, MAX_CELLS> board;
    array<int, MAX_CELLS> pos;
    int next_box = 0;
  };

  struct Frame {
    int target_pos = -1;
    int next_box = 0;
    int allowed_mask = 0;
  };

  struct SimResult {
    vector<Frame> frames;
    bool target_removed = false;
  };

  struct TailInfo {
    array<short, MAX_CELLS> dist;
    array<int, MAX_CELLS> next_cell;
    array<Operation, MAX_CELLS> first_op;
  };

  struct ScheduledOperation {
    int time = 0;
    Operation op{-1, 0};
  };

  struct CandidatePlan {
    int total_extra = COST_INF;
    vector<ScheduledOperation> inserted;
    vector<Operation> tail;
    bool found = false;
  };

  static inline bool tables_ready = false;
  static inline vector<Conveyor> conveyors;
  vector<Operation> ops;
  static inline array<array<int, 2>, MAX_CELLS> cell_conveyors;
  static inline array<vector<Move>, MAX_CELLS> outgoing;
  static inline array<vector<Move>, MAX_CELLS> incoming;
  static inline array<array<int, MAX_CELLS>, MAX_CANDIDATE_CONVEYORS * 2> next_cell;
  static inline array<array<int, MAX_CELLS>, MAX_CANDIDATE_CONVEYORS * 2> prev_cell;
  static inline TailInfo tail_info;
  int used_mask = 0;
  int past_insertions = 0;
  int inserted_ops = 0;
  int tail_ops = 0;
  int searched_frames = 0;
  int beam_expansions = 0;
  int selected_tail_steps = 0;
  int selected_head_steps = 0;
  int tie_mode = 0;  // 0: default, 1: prefer fewer insertions, 2: prefer more insertions.

  Solver(int tie_mode = 0) : tie_mode(tie_mode) {
    if (!tables_ready) {
      BuildConveyorFamily();
      ValidateConveyors();
      BuildMoveTables();
      tail_info = BuildTailInfo((1 << conveyors.size()) - 1);
      tables_ready = true;
    }
  }

  int DirIndex(int dir) const {
    return dir == 1 ? 0 : 1;
  }

  signed char OpCode(Operation op) const {
    if (op.id < 0) return -1;
    return static_cast<signed char>(op.id * 2 + DirIndex(op.dir));
  }

  Operation DecodeOp(int code) const {
    return Operation{code / 2, (code % 2 == 0 ? 1 : -1)};
  }

  bool Adjacent(Cell a, Cell b) const {
    return abs(a.r - b.r) + abs(a.j - b.j) == 1;
  }

  void AddConveyor(const vector<Cell>& cells) {
    conveyors.push_back(Conveyor{cells});
  }

  void BuildConveyorFamily() {
    conveyors.clear();

    // Candidate conveyors are created lazily by the planner, but all used
    // conveyors are printed in the setup section at the end.  The row/column
    // strip family keeps every cell in at most two loops.
    for (int r = 0; r < N; r += 2) {
      vector<Cell> cells;
      for (int j = 0; j < N; j++) cells.push_back(Cell{r, j});
      for (int j = N - 1; j >= 0; j--) cells.push_back(Cell{r + 1, j});
      AddConveyor(cells);
    }
    for (int j = 0; j < N; j += 2) {
      vector<Cell> cells;
      for (int r = 0; r < N; r++) cells.push_back(Cell{r, j});
      for (int r = N - 1; r >= 0; r--) cells.push_back(Cell{r, j + 1});
      AddConveyor(cells);
    }
  }

  void ValidateConveyors() {
    array<int, MAX_CELLS> belong_count;
    belong_count.fill(0);
    REP(v, MAX_CELLS) cell_conveyors[v] = array<int, 2>{-1, -1};

    REP(id, (int)conveyors.size()) {
      const auto& cells = conveyors[id].cells;
      assert((int)cells.size() >= 2);
      bitset<MAX_CELLS> seen;
      REP(k, (int)cells.size()) {
        Cell c = cells[k];
        assert(0 <= c.r && c.r < N && 0 <= c.j && c.j < N);
        int v = CellId(c);
        assert(!seen[v]);
        seen[v] = true;
        assert(belong_count[v] < 2);
        cell_conveyors[v][belong_count[v]++] = id;
        assert(Adjacent(c, cells[(k + 1) % cells.size()]));
      }
    }
    REP(v, total_cells) assert(belong_count[v] <= 2);
  }

  void BuildMoveTables() {
    REP(v, MAX_CELLS) {
      outgoing[v].clear();
      incoming[v].clear();
    }

    REP(id, (int)conveyors.size()) {
      REP(idx, 2) {
        int key = id * 2 + idx;
        REP(v, total_cells) {
          next_cell[key][v] = v;
          prev_cell[key][v] = v;
        }
      }

      const auto& cells = conveyors[id].cells;
      int len = cells.size();
      REP(k, len) {
        int from = CellId(cells[k]);
        for (int dir : {-1, 1}) {
          int key = id * 2 + DirIndex(dir);
          int to = CellId(cells[(k + dir + len) % len]);
          next_cell[key][from] = to;
          prev_cell[key][to] = from;
          outgoing[from].push_back(Move{to, Operation{id, dir}});
          incoming[to].push_back(Move{from, Operation{id, dir}});
        }
      }
    }
  }

  int MoveCell(int cell, Operation op) const {
    return next_cell[op.id * 2 + DirIndex(op.dir)][cell];
  }

  int PrevCell(int cell, Operation op) const {
    return prev_cell[op.id * 2 + DirIndex(op.dir)][cell];
  }

  State InitialState() const {
    State st;
    st.board = initial_board;
    st.pos = initial_pos;
    st.next_box = 0;
    if (st.board[exit_cell] == 0) {
      st.board[exit_cell] = -1;
      st.pos[0] = -1;
      st.next_box = 1;
    }
    return st;
  }

  void ApplyOperation(State& st, Operation op) const {
    const auto& cells = conveyors[op.id].cells;
    int len = cells.size();
    array<int, 2 * FIXED_N> old_values;

    REP(i, len) old_values[i] = st.board[CellId(cells[i])];
    REP(i, len) {
      int dst = CellId(cells[(i + op.dir + len) % len]);
      int box = old_values[i];
      st.board[dst] = box;
      if (box >= 0) st.pos[box] = dst;
    }

    if (st.next_box < total_cells && st.board[exit_cell] == st.next_box) {
      st.board[exit_cell] = -1;
      st.pos[st.next_box] = -1;
      st.next_box++;
    }
  }

  void ChangeLock(array<int, MAX_CANDIDATE_CONVEYORS>& lock_count,
                  int& allowed_mask, int cell, int delta) const {
    for (int id : cell_conveyors[cell]) {
      if (id < 0) continue;
      int before = lock_count[id];
      int after = before + delta;
      assert(after >= 0);
      lock_count[id] = after;
      if (before == 0 && after > 0) {
        allowed_mask &= ~(1 << id);
      } else if (before > 0 && after == 0) {
        allowed_mask |= 1 << id;
      }
    }
  }

  void ApplyOperationWithLocks(State& st, Operation op, int target,
                               array<int, MAX_CANDIDATE_CONVEYORS>& lock_count,
                               int& allowed_mask) const {
    const auto& cells = conveyors[op.id].cells;
    int len = cells.size();
    array<int, 2 * FIXED_N> old_values;

    REP(i, len) {
      int src = CellId(cells[i]);
      int box = st.board[src];
      old_values[i] = box;
      if (0 <= box && box < target) ChangeLock(lock_count, allowed_mask, src, -1);
    }
    REP(i, len) {
      int dst = CellId(cells[(i + op.dir + len) % len]);
      int box = old_values[i];
      st.board[dst] = box;
      if (box >= 0) st.pos[box] = dst;
      if (0 <= box && box < target) ChangeLock(lock_count, allowed_mask, dst, 1);
    }

    if (st.next_box < total_cells && st.board[exit_cell] == st.next_box) {
      if (st.next_box < target) ChangeLock(lock_count, allowed_mask, exit_cell, -1);
      st.board[exit_cell] = -1;
      st.pos[st.next_box] = -1;
      st.next_box++;
    }
  }

  int AllowedMask(const State& st, int target) const {
    int mask = (1 << conveyors.size()) - 1;
    REP(box, target) {
      int p = st.pos[box];
      if (p < 0) continue;
      for (int id : cell_conveyors[p]) {
        if (id >= 0) mask &= ~(1 << id);
      }
    }
    return mask;
  }

  SimResult SimulateForTarget(int target) const {
    State st = InitialState();
    array<int, MAX_CANDIDATE_CONVEYORS> lock_count;
    lock_count.fill(0);
    int allowed_mask = (1 << conveyors.size()) - 1;
    REP(box, target) {
      int p = st.pos[box];
      if (p >= 0) ChangeLock(lock_count, allowed_mask, p, 1);
    }

    SimResult result;
    result.frames.reserve(ops.size() + 1);

    for (int t = 0; t <= (int)ops.size(); t++) {
      Frame frame;
      frame.target_pos = st.pos[target];
      frame.next_box = st.next_box;
      frame.allowed_mask = allowed_mask;
      result.frames.push_back(frame);

      if (st.next_box > target) result.target_removed = true;
      if (t == (int)ops.size()) break;
      ApplyOperationWithLocks(st, ops[t], target, lock_count, allowed_mask);
    }
    return result;
  }

  TailInfo BuildTailInfo(int allowed_mask) const {
    TailInfo info;
    info.dist.fill(COST_INF);
    info.next_cell.fill(-1);
    REP(v, MAX_CELLS) info.first_op[v] = Operation{-1, 0};

    queue<int> que;
    info.dist[exit_cell] = 0;
    info.next_cell[exit_cell] = exit_cell;
    que.push(exit_cell);

    while (!que.empty()) {
      int to = que.front();
      que.pop();
      for (const Move& mv : incoming[to]) {
        int id = mv.op.id;
        if (((allowed_mask >> id) & 1) == 0) continue;
        int from = mv.to;
        if (info.dist[from] != COST_INF) continue;
        info.dist[from] = info.dist[to] + 1;
        info.next_cell[from] = to;
        info.first_op[from] = mv.op;
        que.push(from);
      }
    }
    return info;
  }

  bool ConveyorTouchesExit(int id) const {
    for (Cell c : conveyors[id].cells) {
      if (CellId(c) == exit_cell) return true;
    }
    return false;
  }

  Operation DummyOperation() const {
    REP(id, (int)conveyors.size()) {
      if (!ConveyorTouchesExit(id)) return Operation{id, 1};
    }
    return Operation{0, 1};
  }

  Operation DummyOperation(int allowed_mask) const {
    REP(id, (int)conveyors.size()) {
      if (((allowed_mask >> id) & 1) && !ConveyorTouchesExit(id)) {
        return Operation{id, 1};
      }
    }
    return Operation{-1, 0};
  }

  vector<Operation> RestoreTailPath(int start) const {
    if (start < 0) return {};
    if (start == exit_cell) return {DummyOperation()};

    vector<Operation> path;
    int cur = start;
    while (cur != exit_cell) {
      Operation op = tail_info.first_op[cur];
      assert(op.id >= 0);
      path.push_back(op);
      cur = tail_info.next_cell[cur];
      assert(cur >= 0);
    }
    return path;
  }

  int TailCost(int cell) const {
    if (cell < 0) return 0;
    if (cell == exit_cell) return 1;
    return tail_info.dist[cell];
  }

  bool BetterPlanCost(int cost, int inserted_count,
                      int best_cost, int best_inserted_count,
                      int plan_tie_mode) const {
    if (cost != best_cost) return cost < best_cost;
    if (plan_tie_mode == 1) return inserted_count < best_inserted_count;
    if (plan_tie_mode == 2) return inserted_count > best_inserted_count;
    return false;
  }

  vector<CostRow> BuildBackwardCosts(int target, const SimResult& sim) const {
    int T = ops.size();
    vector<CostRow> rem(T + 1);

    REP(v, total_cells) rem[T][v] = TailCost(v);
    for (int t = T - 1; t >= 0; t--) {
      Operation op = ops[t];
      REP(v, total_cells) {
        int after = MoveCell(v, op);
        if (after == exit_cell && sim.frames[t + 1].next_box >= target) {
          rem[t][v] = 0;
        } else {
          rem[t][v] = rem[t + 1][after];
        }
      }
    }
    return rem;
  }

  vector<Operation> RestoreInsertedPath(int start, int goal,
                                        const array<int, MAX_CELLS>& parent,
                                        const array<Operation, MAX_CELLS>& parent_op) const {
    vector<Operation> path;
    int cur = goal;
    while (cur != start) {
      Operation op = parent_op[cur];
      assert(op.id >= 0);
      path.push_back(op);
      cur = parent[cur];
      assert(cur >= 0);
    }
    reverse(ALL(path));
    return path;
  }

  bool BestInsertedBlock(int start, int allowed_mask, const CostRow& rem,
                         int max_total, vector<Operation>& block, int& total) const {
    array<short, MAX_CELLS> dist;
    array<int, MAX_CELLS> parent;
    array<Operation, MAX_CELLS> parent_op;
    dist.fill(COST_INF);
    parent.fill(-1);
    REP(v, MAX_CELLS) parent_op[v] = Operation{-1, 0};

    int best_cell = -1;
    int best_total = max_total;
    queue<int> que;
    dist[start] = 0;
    que.push(start);

    while (!que.empty()) {
      int v = que.front();
      que.pop();

      if (dist[v] > 0 && rem[v] < COST_INF) {
        int cand = dist[v] + rem[v];
        if (cand < best_total) {
          best_total = cand;
          best_cell = v;
        }
      }
      if (dist[v] + 1 >= best_total) continue;

      for (const Move& mv : outgoing[v]) {
        int id = mv.op.id;
        if (((allowed_mask >> id) & 1) == 0) continue;
        int to = mv.to;
        if (dist[to] != COST_INF) continue;
        dist[to] = dist[v] + 1;
        parent[to] = v;
        parent_op[to] = mv.op;
        que.push(to);
      }
    }

    if (best_cell < 0) return false;
    block = RestoreInsertedPath(start, best_cell, parent, parent_op);
    total = best_total;
    return true;
  }

  pair<bool, int> TraceTargetAfterInsertion(int target, const SimResult& sim,
                                            int insert_time,
                                            const vector<Operation>& block) const {
    int pos = sim.frames[insert_time].target_pos;
    int ready = sim.frames[insert_time].next_box;

    for (Operation op : block) {
      pos = MoveCell(pos, op);
      if (pos == exit_cell && ready >= target) return {true, -1};
    }
    for (int t = insert_time; t < (int)ops.size(); t++) {
      pos = MoveCell(pos, ops[t]);
      if (pos == exit_cell && sim.frames[t + 1].next_box >= target) {
        return {true, -1};
      }
    }
    return {false, pos};
  }

  vector<int> CandidateTimes(int T) const {
    vector<int> times;
    int budget = 260;
    int stride = max(1, T / budget);
    for (int t = 0; t <= T; t += stride) times.push_back(t);
    if (times.empty() || times.back() != T) times.push_back(T);
    return times;
  }

  CandidatePlan RestorePlanFromActions(int target, const SimResult& sim,
                                       const vector<ActionRow>& action) const {
    CandidatePlan plan;
    int T = ops.size();
    int start = sim.frames[0].target_pos;
    if (start < 0) return plan;

    int t = 0;
    int pos = start;
    bool removed = false;
    int guard = 0;
    while (t < T) {
      assert(guard++ <= COST_INF + T + 5);
      int code = action[t][pos];
      if (code >= 0) {
        Operation op = DecodeOp(code);
        plan.inserted.push_back(ScheduledOperation{t, op});
        pos = MoveCell(pos, op);
        if (pos == exit_cell && sim.frames[t].next_box >= target) {
          removed = true;
          break;
        }
      } else {
        pos = MoveCell(pos, ops[t]);
        if (pos == exit_cell && sim.frames[t + 1].next_box >= target) {
          removed = true;
          break;
        }
        t++;
      }
    }

    if (!removed) plan.tail = RestoreTailPath(pos);
    plan.total_extra = (int)plan.inserted.size() + (int)plan.tail.size();
    plan.found = true;
    return plan;
  }

  array<CandidatePlan, 2> BuildPlanVariantsForTarget(int target,
                                                     const SimResult& sim) const {
    int T = ops.size();
    vector<ActionRow> tail_action(T);
    vector<ActionRow> head_action(T);

    CostRow next_dp;
    CostRow next_min_inserted;
    CostRow next_max_inserted;
    next_dp.fill(COST_INF);
    next_min_inserted.fill(0);
    next_max_inserted.fill(0);
    REP(v, total_cells) next_dp[v] = TailCost(v);

    vector<int> que;
    que.reserve(MAX_CELLS * 8);

    for (int t = T - 1; t >= 0; t--) {
      CostRow dist;
      CostRow min_inserted;
      CostRow max_inserted;
      ActionRow tail_row;
      ActionRow head_row;
      tail_row.fill(-1);
      head_row.fill(-1);

      que.clear();
      int que_head = 0;
      array<char, MAX_CELLS> in_queue;
      in_queue.fill(false);

      auto Push = [&](int v) {
        if (!in_queue[v]) {
          que.push_back(v);
          in_queue[v] = true;
        }
      };

      REP(v, total_cells) {
        int after = MoveCell(v, ops[t]);
        if (after == exit_cell && sim.frames[t + 1].next_box >= target) {
          dist[v] = 0;
          min_inserted[v] = 0;
          max_inserted[v] = 0;
        } else {
          dist[v] = next_dp[after];
          min_inserted[v] = next_min_inserted[after];
          max_inserted[v] = next_max_inserted[after];
        }
        if (dist[v] < COST_INF) Push(v);
      }

      auto RelaxValue = [&](int from, int cand, int cand_min_inserted,
                             int cand_max_inserted, Operation op) {
        int code = OpCode(op);
        bool changed = false;
        if (cand < dist[from]) {
          dist[from] = cand;
          min_inserted[from] = cand_min_inserted;
          max_inserted[from] = cand_max_inserted;
          tail_row[from] = code;
          head_row[from] = code;
          changed = true;
        } else if (cand == dist[from]) {
          if (cand_min_inserted < min_inserted[from]) {
            min_inserted[from] = cand_min_inserted;
            tail_row[from] = code;
            changed = true;
          }
          if (cand_max_inserted > max_inserted[from]) {
            max_inserted[from] = cand_max_inserted;
            head_row[from] = code;
            changed = true;
          }
        }
        if (changed) Push(from);
      };

      auto Relax = [&](int from, int to, Operation op) {
        if (dist[to] >= COST_INF) return;
        RelaxValue(from, dist[to] + 1, min_inserted[to] + 1, max_inserted[to] + 1, op);
      };

      int allowed_mask = sim.frames[t].allowed_mask;
      if (sim.frames[t].next_box >= target) {
        Operation dummy = DummyOperation(allowed_mask);
        if (dummy.id >= 0) RelaxValue(exit_cell, 1, 1, 1, dummy);
        for (const Move& mv : incoming[exit_cell]) {
          int id = mv.op.id;
          if (((allowed_mask >> id) & 1) == 0) continue;
          RelaxValue(mv.to, 1, 1, 1, mv.op);
        }
      }

      while (que_head < (int)que.size()) {
        int to = que[que_head++];
        in_queue[to] = false;
        if (dist[to] >= COST_INF) continue;

        for (const Move& mv : incoming[to]) {
          int id = mv.op.id;
          if (((allowed_mask >> id) & 1) == 0) continue;
          Relax(mv.to, to, mv.op);
        }
      }

      tail_action[t] = tail_row;
      head_action[t] = head_row;
      next_dp = dist;
      next_min_inserted = min_inserted;
      next_max_inserted = max_inserted;
    }

    array<CandidatePlan, 2> plans;
    int start = sim.frames[0].target_pos;
    if (start < 0 || next_dp[start] >= COST_INF) return plans;
    plans[0] = RestorePlanFromActions(target, sim, tail_action);
    plans[1] = RestorePlanFromActions(target, sim, head_action);
    return plans;
  }

  CandidatePlan BuildPlanForTarget(int target, const SimResult& sim,
                                   int plan_tie_mode) const {
    int T = ops.size();
    vector<ActionRow> action(T);

    CostRow next_dp;
    CostRow next_inserted_count;
    next_dp.fill(COST_INF);
    next_inserted_count.fill(0);
    REP(v, total_cells) {
      next_dp[v] = TailCost(v);
    }

    vector<int> que;
    que.reserve(MAX_CELLS * 8);

    for (int t = T - 1; t >= 0; t--) {
      CostRow dist;
      CostRow inserted_count;
      ActionRow action_row;
      action_row.fill(-1);

      que.clear();
      int que_head = 0;
      array<char, MAX_CELLS> in_queue;
      in_queue.fill(false);

      REP(v, total_cells) {
        int after = MoveCell(v, ops[t]);
        if (after == exit_cell && sim.frames[t + 1].next_box >= target) {
          dist[v] = 0;
          inserted_count[v] = 0;
        } else {
          dist[v] = next_dp[after];
          inserted_count[v] = next_inserted_count[after];
        }
        if (dist[v] < COST_INF) {
          que.push_back(v);
          in_queue[v] = true;
        }
      }

      int allowed_mask = sim.frames[t].allowed_mask;
      if (sim.frames[t].next_box >= target) {
        Operation dummy = DummyOperation(allowed_mask);
        if (dummy.id >= 0 &&
            BetterPlanCost(1, 1, dist[exit_cell], inserted_count[exit_cell], plan_tie_mode)) {
          dist[exit_cell] = 1;
          inserted_count[exit_cell] = 1;
          action_row[exit_cell] = OpCode(dummy);
          if (!in_queue[exit_cell]) {
            que.push_back(exit_cell);
            in_queue[exit_cell] = true;
          }
        }
        for (const Move& mv : incoming[exit_cell]) {
          int id = mv.op.id;
          if (((allowed_mask >> id) & 1) == 0) continue;
          int from = mv.to;
          if (BetterPlanCost(1, 1, dist[from], inserted_count[from], plan_tie_mode)) {
            dist[from] = 1;
            inserted_count[from] = 1;
            action_row[from] = OpCode(mv.op);
            if (!in_queue[from]) {
              que.push_back(from);
              in_queue[from] = true;
            }
          }
        }
      }

      while (que_head < (int)que.size()) {
        int to = que[que_head++];
        in_queue[to] = false;
        if (dist[to] >= COST_INF) continue;

        for (const Move& mv : incoming[to]) {
          int id = mv.op.id;
          if (((allowed_mask >> id) & 1) == 0) continue;
          int from = mv.to;
          int cand = dist[to] + 1;
          int cand_inserted_count = inserted_count[to] + 1;
          if (BetterPlanCost(cand, cand_inserted_count,
                             dist[from], inserted_count[from], plan_tie_mode)) {
            dist[from] = cand;
            inserted_count[from] = cand_inserted_count;
            action_row[from] = OpCode(mv.op);
            if (!in_queue[from]) {
              que.push_back(from);
              in_queue[from] = true;
            }
          }
        }
      }

      action[t] = action_row;
      next_dp = dist;
      next_inserted_count = inserted_count;
    }

    CandidatePlan plan;
    int start = sim.frames[0].target_pos;
    if (start < 0 || next_dp[start] >= COST_INF) return plan;

    int t = 0;
    int pos = start;
    bool removed = false;
    int guard = 0;
    while (t < T) {
      assert(guard++ <= next_dp[start] + T + 5);
      int code = action[t][pos];
      if (code >= 0) {
        Operation op = DecodeOp(code);
        plan.inserted.push_back(ScheduledOperation{t, op});
        pos = MoveCell(pos, op);
        if (pos == exit_cell && sim.frames[t].next_box >= target) {
          removed = true;
          break;
        }
      } else {
        pos = MoveCell(pos, ops[t]);
        if (pos == exit_cell && sim.frames[t + 1].next_box >= target) {
          removed = true;
          break;
        }
        t++;
      }
    }

    if (!removed) plan.tail = RestoreTailPath(pos);
    plan.total_extra = (int)plan.inserted.size() + (int)plan.tail.size();
    plan.found = true;
    return plan;
  }

  bool SamePlan(const CandidatePlan& a, const CandidatePlan& b) const {
    if (a.inserted.size() != b.inserted.size() || a.tail.size() != b.tail.size()) return false;
    REP(i, (int)a.inserted.size()) {
      if (a.inserted[i].time != b.inserted[i].time) return false;
      if (a.inserted[i].op.id != b.inserted[i].op.id) return false;
      if (a.inserted[i].op.dir != b.inserted[i].op.dir) return false;
    }
    REP(i, (int)a.tail.size()) {
      if (a.tail[i].id != b.tail[i].id || a.tail[i].dir != b.tail[i].dir) return false;
    }
    return true;
  }

  void MarkUsed(Operation op) {
    used_mask |= 1 << op.id;
  }

  void ApplyPlan(const CandidatePlan& plan) {
    if (!plan.inserted.empty()) {
      bool inserted_to_past = false;
      inserted_ops += plan.inserted.size();
      int old_size = ops.size();

      vector<Operation> new_ops;
      new_ops.reserve(old_size + plan.inserted.size() + plan.tail.size());
      int insert_index = 0;
      REP(t, old_size + 1) {
        while (insert_index < (int)plan.inserted.size() &&
               plan.inserted[insert_index].time == t) {
          ScheduledOperation scheduled = plan.inserted[insert_index++];
          if (scheduled.time != old_size) inserted_to_past = true;
          MarkUsed(scheduled.op);
          new_ops.push_back(scheduled.op);
        }
        if (t < old_size) new_ops.push_back(ops[t]);
      }
      assert(insert_index == (int)plan.inserted.size());
      ops.swap(new_ops);
      if (inserted_to_past) past_insertions++;
    }

    tail_ops += plan.tail.size();
    for (Operation op : plan.tail) MarkUsed(op);
    ops.insert(ops.end(), ALL(plan.tail));
  }

  int CountRemoved() const {
    State st = InitialState();
    for (Operation op : ops) ApplyOperation(st, op);
    return st.next_box;
  }

  int EstimateScore(int B, int T) const {
    if (B == total_cells) {
      return 1000000 + (int)llround(1000000.0 * log2(100000.0 / max(1, T)));
    }
    return (int)llround(1000000.0 * B / total_cells);
  }

  int Run() {
    REP(target, total_cells) {
      SimResult sim = SimulateForTarget(target);
      if (sim.target_removed) continue;

      searched_frames += ops.size() + 1;
      CandidatePlan plan = BuildPlanForTarget(target, sim, tie_mode);
      if (!plan.found || ops.size() + plan.total_extra > 100000) break;
      ApplyPlan(plan);
    }

    int B = CountRemoved();
    int T = ops.size();
    return EstimateScore(B, T);
  }

  int RunBeamSearch(int beam_width, int beam_target_limit,
                    int early_beam_width, int early_beam_target_limit) {
    struct BeamCandidate {
      int parent = -1;
      int mode = 0;
      int ops_after = 0;
      int inserted_after = 0;
      CandidatePlan plan;
      bool pass_through = false;
    };

    vector<Solver> beam;
    beam.reserve(max(beam_width, early_beam_width));
    beam.push_back(*this);

    REP(target, total_cells) {
      int current_beam_width = 1;
      if (target < early_beam_target_limit) {
        current_beam_width = early_beam_width;
      } else if (target < beam_target_limit) {
        current_beam_width = beam_width;
      }
      if ((int)beam.size() > current_beam_width) {
        stable_sort(ALL(beam), [](const Solver& a, const Solver& b) {
          if (a.ops.size() != b.ops.size()) return a.ops.size() < b.ops.size();
          return a.inserted_ops < b.inserted_ops;
        });
        beam.resize(current_beam_width);
      }
      bool use_beam = current_beam_width > 1;

      vector<BeamCandidate> candidates;
      candidates.reserve(beam.size() * (use_beam ? 2 : 1));

      REP(i, (int)beam.size()) {
        const Solver& cur = beam[i];
        SimResult sim = cur.SimulateForTarget(target);
        if (sim.target_removed) {
          candidates.push_back(BeamCandidate{
              i, 0, (int)cur.ops.size(), cur.inserted_ops, CandidatePlan{}, true});
          continue;
        }

        if (use_beam) {
          array<CandidatePlan, 2> plans = cur.BuildPlanVariantsForTarget(target, sim);
          REP(mode_index, 2) {
            int mode = mode_index + 1;
            CandidatePlan plan = std::move(plans[mode_index]);
            if (!plan.found || cur.ops.size() + plan.total_extra > 100000) continue;
            BeamCandidate cand;
            cand.parent = i;
            cand.mode = mode;
            cand.ops_after = cur.ops.size() + plan.total_extra;
            cand.inserted_after = cur.inserted_ops + (int)plan.inserted.size();
            cand.plan = std::move(plan);
            candidates.push_back(std::move(cand));
          }
        } else {
          CandidatePlan plan = cur.BuildPlanForTarget(target, sim, 1);
          if (!plan.found || cur.ops.size() + plan.total_extra > 100000) continue;
          BeamCandidate cand;
          cand.parent = i;
          cand.mode = 1;
          cand.ops_after = cur.ops.size() + plan.total_extra;
          cand.inserted_after = cur.inserted_ops + (int)plan.inserted.size();
          cand.plan = std::move(plan);
          candidates.push_back(std::move(cand));
        }
      }

      if (candidates.empty()) break;

      stable_sort(ALL(candidates), [](const BeamCandidate& a, const BeamCandidate& b) {
        if (a.ops_after != b.ops_after) return a.ops_after < b.ops_after;
        if (a.inserted_after != b.inserted_after) return a.inserted_after < b.inserted_after;
        if (a.parent != b.parent) return a.parent < b.parent;
        return a.mode < b.mode;
      });

      vector<BeamCandidate> selected;
      selected.reserve(current_beam_width);
      for (BeamCandidate& cand : candidates) {
        bool duplicate = false;
        for (const BeamCandidate& picked : selected) {
          if (cand.parent != picked.parent) continue;
          if (cand.pass_through && picked.pass_through) {
            duplicate = true;
            break;
          }
          if (!cand.pass_through && !picked.pass_through && SamePlan(cand.plan, picked.plan)) {
            duplicate = true;
            break;
          }
        }
        if (duplicate) continue;
        selected.push_back(std::move(cand));
        if ((int)selected.size() == current_beam_width) break;
      }

      vector<Solver> next_beam;
      next_beam.reserve(selected.size());
      for (const BeamCandidate& cand : selected) {
        Solver child = beam[cand.parent];
        child.beam_expansions += candidates.size();
        if (!cand.pass_through) {
          child.searched_frames += child.ops.size() + 1;
          if (cand.mode == 1) child.selected_tail_steps++;
          if (cand.mode == 2) child.selected_head_steps++;
          child.ApplyPlan(cand.plan);
        }
        next_beam.push_back(std::move(child));
      }
      beam.swap(next_beam);
    }

    int best_index = 0;
    int best_score = -1;
    REP(i, (int)beam.size()) {
      int B = beam[i].CountRemoved();
      int T = beam[i].ops.size();
      int score = beam[i].EstimateScore(B, T);
      if (score > best_score ||
          (score == best_score && T < (int)beam[best_index].ops.size())) {
        best_score = score;
        best_index = i;
      }
    }
    *this = std::move(beam[best_index]);
    return best_score;
  }

  void LogStats(int score) const {
    int B = CountRemoved();
    int T = ops.size();
    int M = __builtin_popcount((unsigned)used_mask);
    LOG(M);
    LOG(B);
    LOG(T);
    LOG(past_insertions);
    LOG(inserted_ops);
    LOG(tail_ops);
    LOG(searched_frames);
    LOG(beam_expansions);
    LOG(selected_tail_steps);
    LOG(selected_head_steps);
    LOG(score);
  }

  void Output() {
    vector<int> id_map(conveyors.size(), -1);
    vector<int> used_ids;
    REP(id, (int)conveyors.size()) {
      if ((used_mask >> id) & 1) {
        id_map[id] = used_ids.size();
        used_ids.push_back(id);
      }
    }

    cout << used_ids.size() << '\n';
    for (int id : used_ids) {
      const auto& conveyor = conveyors[id];
      cout << conveyor.cells.size();
      for (Cell c : conveyor.cells) cout << ' ' << c.r << ' ' << c.j;
      cout << '\n';
    }

    cout << ops.size() << '\n';
    for (Operation op : ops) {
      cout << id_map[op.id] << ' ' << op.dir << '\n';
    }
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  SetUp();
  constexpr int BEAM_WIDTH = 2;
  constexpr int BEAM_TARGET_LIMIT = 300;
  constexpr int EARLY_BEAM_WIDTH = 3;
  constexpr int EARLY_BEAM_TARGET_LIMIT = 20;
  Solver solver;
  int score = solver.RunBeamSearch(BEAM_WIDTH, BEAM_TARGET_LIMIT,
                                   EARLY_BEAM_WIDTH, EARLY_BEAM_TARGET_LIMIT);
  LOG(BEAM_WIDTH);
  LOG(BEAM_TARGET_LIMIT);
  LOG(EARLY_BEAM_WIDTH);
  LOG(EARLY_BEAM_TARGET_LIMIT);
  solver.LogStats(score);
  solver.Output();
  double elapsed = timer.elapsed();
  LOG(elapsed);
  cout.flush();
  cerr.flush();
  std::_Exit(0);
}
