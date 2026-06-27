#pragma GCC optimize("O3")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")

#include <atcoder/dsu>
#include <atcoder/segtree>
// #include <atcoder/mincostflow>

#define FINAL_SUBMIT 0

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
#define GRAPH(x, y)
#define CERR() if (false) std::cerr
#define NDEBUG
#else
#define HHH(x) std::cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << "\n"
#define LOG(x) std::cerr << #x << " = " << x << "\n"
#define GRAPH(x, y) std::cerr << "graph " << x << " " << y << "\n"
#define CERR() std::cerr
#endif

template <typename T> T &chmin(T &a, const T &b) { return a = std::min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = std::max(a, b); }

using ll = long long;
using ld = double;
// using P = std::pair<int, int>;
constexpr int INF = 1e9;
constexpr double PI = acos(-1.0);

// Template for printing pair and vector
std::ostream& operator<<(std::ostream& os, const std::pair<int, int>& p) {
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
  // static constexpr double time_limit = 1.98;
  static constexpr int64_t CYCLES_PER_SEC = 2900000000;
  // static constexpr double CYCLES = CYCLES_PER_SEC * time_limit;
  int64_t start, stop;
#if 0
  // Deterministic mode
  inline int64_t get_cycle() const {
    static int64_t cycle = 0;
    return cycle += 30000;
  }
#else
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

Timer timer(1.99);
XorShift rnd;

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0xFF;
  constexpr static double START_TEMP = 2;
  constexpr static double END_TEMP = 0.05;
  constexpr static int64_t CYCLES = Timer::CYCLES_PER_SEC * 1.99;
  // constexpr static double TEMP_RATIO = (END_TEMP - START_TEMP) / CYCLES;  // linear
  constexpr static double TEMP_RATIO = log(END_TEMP / START_TEMP) / CYCLES;  // exponential
  double logp[LOG_SIZE];
  long long iteration = 0;
  int64_t cycle = 0;
  int64_t start = timer.get_cycle();
  int64_t stop = timer.stop;
  bool record = false;
  double temp = START_TEMP;

  SimulatedAnnealing() {
    constexpr double inv = 1.0 / LOG_SIZE;
    REP(i,LOG_SIZE) logp[i] = log((i + 0.5) * inv);
    std::mt19937 rng;
    std::shuffle(logp, logp + LOG_SIZE, rng);
  }

  inline bool End() {
    iteration++;
    // if ((iteration & UPDATE_INTERVAL) != 0) return false;
    cycle = timer.get_cycle();
    // temp = END_TEMP - (stop - cycle) * TEMP_RATIO;  // linear
    temp = START_TEMP * exp((cycle - start) * TEMP_RATIO);  // exponential
    return cycle >= stop;
  }

  inline bool Accept(double diff) {
    static unsigned short index = 0;
    // return diff >= 0;
    return diff >= 0 || diff > logp[index++] * temp;
  }
};

using namespace std;

template <typename T> T sq(T x) { return x * x; }

constexpr int N = 20;
constexpr int M = 40;
int P[M];

void SetUp() {
  int n, m;
  cin >> n >> m;
  REP(i,M) {
    int r, c;
    cin >> r >> c;
    P[i] = r * N + c;
  }
}

enum class Direction : int { Up = 0, Down = 1, Left = 2, Right = 3 };
enum class ActionType : int { Move = 0, Slide = 1, Alter = 2 };

constexpr int DirectionDiff[4] = {-20, 20, -1, 1};

struct Action {
  ActionType type;
  Direction dir;
};

inline std::ostream& operator<<(std::ostream& os, const Action& a) {
  static constexpr char type_c[] = { 'M', 'S', 'A' };
  static constexpr char dir_c[]  = { 'U', 'D', 'L', 'R' };

  os << type_c[static_cast<int>(a.type)]
     << ' '
     << dir_c [static_cast<int>(a.dir)];
  return os;
}

int visit_clock_ = 0;
int seen_[N * N];
int dist_[N * N];

class Board {
public:
  static constexpr int SIZE = N * N;
  std::array<bool, SIZE> grid_;
  std::array<int,  SIZE> slide_cache_[4];

  Board() {
    grid_.fill(false);
    BuildSlideTable();
  }

  void ToggleBlock(int idx) noexcept {
    assert(0 <= idx && idx < SIZE);
    grid_[idx] = !grid_[idx];

    auto [ci, cj] = toCell(idx);

    // --- Up (0) と Down (1) のキャッシュを、列 cj について更新 ---
    // Up: 上方向に向かって最初のブロックまたは端で停止
    {
      int last_block = -1;  // 直前にあったブロックの行番号
      for (int i = 0; i < N; i++) {
        int cur = toIdx(i, cj);
        if (grid_[cur]) {
          last_block = i;
          slide_cache_[static_cast<int>(Direction::Up)][cur] = cur;
        } else {
          // 自分より上の最後のブロック+1（行番号0始まりなので max(0, last_block+1)）
          int stop_i = std::max(0, last_block + 1);
          slide_cache_[static_cast<int>(Direction::Up)][cur] = toIdx(stop_i, cj);
        }
      }
    }
    // Down: 下方向に向かって最初のブロックまたは端で停止
    {
      int last_block = -1;
      for (int di = 0; di < N; di++) {
        int i = N - 1 - di;
        int cur = toIdx(i, cj);
        if (grid_[cur]) {
          last_block = i;
          slide_cache_[static_cast<int>(Direction::Down)][cur] = cur;
        } else {
          int stop_i = std::min(N - 1, last_block == -1 ? N - 1 : last_block - 1);
          slide_cache_[static_cast<int>(Direction::Down)][cur] = toIdx(stop_i, cj);
        }
      }
    }

    // --- Left (2) と Right (3) のキャッシュを、行 ci について更新 ---
    // Left: 左方向に向かって最初のブロックまたは端で停止
    {
      int last_block = -1;
      for (int j = 0; j < N; j++) {
        int cur = toIdx(ci, j);
        if (grid_[cur]) {
          last_block = j;
          slide_cache_[static_cast<int>(Direction::Left)][cur] = cur;
        } else {
          int stop_j = std::max(0, last_block + 1);
          slide_cache_[static_cast<int>(Direction::Left)][cur] = toIdx(ci, stop_j);
        }
      }
    }
    // Right: 右方向に向かって最初のブロックまたは端で停止
    {
      int last_block = -1;
      for (int dj = 0; dj < N; dj++) {
        int j = N - 1 - dj;
        int cur = toIdx(ci, j);
        if (grid_[cur]) {
          last_block = j;
          slide_cache_[static_cast<int>(Direction::Right)][cur] = cur;
        } else {
          int stop_j = std::min(N - 1, last_block == -1 ? N - 1 : last_block - 1);
          slide_cache_[static_cast<int>(Direction::Right)][cur] = toIdx(ci, stop_j);
        }
      }
    }
  }

  void BuildSlideTable() noexcept {
    // --- Up (0) ---
    for (int j = 0; j < N; j++) {
      int last_block = -1;
      for (int i = 0; i < N; i++) {
        int idx = toIdx(i, j);
        if (grid_[idx]) {
          // ブロックセルは自分自身が停止先
          slide_cache_[static_cast<int>(Direction::Up)][idx] = idx;
          last_block = i;
        } else {
          // 最後に見つかったブロックの1つ下、盤外は0行目
          int stop_i = std::max(0, last_block + 1);
          slide_cache_[static_cast<int>(Direction::Up)][idx] = toIdx(stop_i, j);
        }
      }
    }

    // --- Down (1) ---
    for (int j = 0; j < N; j++) {
      int last_block = -1;
      for (int di = 0; di < N; di++) {
        int i = N - 1 - di;
        int idx = toIdx(i, j);
        if (grid_[idx]) {
          slide_cache_[static_cast<int>(Direction::Down)][idx] = idx;
          last_block = i;
        } else {
          int stop_i = (last_block == -1 ? N - 1 : last_block - 1);
          slide_cache_[static_cast<int>(Direction::Down)][idx] = toIdx(stop_i, j);
        }
      }
    }

    // --- Left (2) ---
    for (int i = 0; i < N; i++) {
      int last_block = -1;
      for (int j = 0; j < N; j++) {
        int idx = toIdx(i, j);
        if (grid_[idx]) {
          slide_cache_[static_cast<int>(Direction::Left)][idx] = idx;
          last_block = j;
        } else {
          int stop_j = std::max(0, last_block + 1);
          slide_cache_[static_cast<int>(Direction::Left)][idx] = toIdx(i, stop_j);
        }
      }
    }

    // --- Right (3) ---
    for (int i = 0; i < N; i++) {
      int last_block = -1;
      for (int dj = 0; dj < N; dj++) {
        int j = N - 1 - dj;
        int idx = toIdx(i, j);
        if (grid_[idx]) {
          slide_cache_[static_cast<int>(Direction::Right)][idx] = idx;
          last_block = j;
        } else {
          int stop_j = (last_block == -1 ? N - 1 : last_block - 1);
          slide_cache_[static_cast<int>(Direction::Right)][idx] = toIdx(i, stop_j);
        }
      }
    }
  }

  int ShortestDistance(int src, int dst) const {
    assert(0 <= src && src < SIZE);
    assert(0 <= dst && dst < SIZE);
    if (src == dst) return 0;

    constexpr int INF = SIZE + 5;
    // タイムスタンプ開始
    int clk = ++visit_clock_;

    // 固定長キュー
    int que[SIZE], qh = 0, qt = 0;
    que[qt++] = src;
    seen_[src] = clk;
    dist_[src] = 0;

    int dist = 0;
    while (qh < qt) {
      int u = que[qh++];
      int d = dist_[u];
      int ui = u / N, uj = u % N;

      // --- UP: Move ---
      if (ui > 0) {
        int v = u - N;
        if (seen_[v] != clk && !grid_[v]) {
          seen_[v] = clk;
          dist_[v] = d + 1;
          if (v == dst) return dist_[v];
          que[qt++] = v;
        }
      }
      // --- UP: Slide ---
      {
        int v2 = slide_cache_[0][u];
        if (v2 != u && seen_[v2] != clk) {
          seen_[v2] = clk;
          dist_[v2] = d + 1;
          if (v2 == dst) return dist_[v2];
          que[qt++] = v2;
        }
      }

      // --- DOWN: Move ---
      if (ui + 1 < N) {
        int v = u + N;
        if (seen_[v] != clk && !grid_[v]) {
          seen_[v] = clk;
          dist_[v] = d + 1;
          if (v == dst) return dist_[v];
          que[qt++] = v;
        }
      }
      // --- DOWN: Slide ---
      {
        int v2 = slide_cache_[1][u];
        if (v2 != u && seen_[v2] != clk) {
          seen_[v2] = clk;
          dist_[v2] = d + 1;
          if (v2 == dst) return dist_[v2];
          que[qt++] = v2;
        }
      }

      // --- LEFT: Move ---
      if (uj > 0) {
        int v = u - 1;
        if (seen_[v] != clk && !grid_[v]) {
          seen_[v] = clk;
          dist_[v] = d + 1;
          if (v == dst) return dist_[v];
          que[qt++] = v;
        }
      }
      // --- LEFT: Slide ---
      {
        int v2 = slide_cache_[2][u];
        if (v2 != u && seen_[v2] != clk) {
          seen_[v2] = clk;
          dist_[v2] = d + 1;
          if (v2 == dst) return dist_[v2];
          que[qt++] = v2;
        }
      }

      // --- RIGHT: Move ---
      if (uj + 1 < N) {
        int v = u + 1;
        if (seen_[v] != clk && !grid_[v]) {
          seen_[v] = clk;
          dist_[v] = d + 1;
          if (v == dst) return dist_[v];
          que[qt++] = v;
        }
      }
      // --- RIGHT: Slide ---
      {
        int v2 = slide_cache_[3][u];
        if (v2 != u && seen_[v2] != clk) {
          seen_[v2] = clk;
          dist_[v2] = d + 1;
          if (v2 == dst) return dist_[v2];
          que[qt++] = v2;
        }
      }
    }

    return INF;
  }

  std::vector<Action> ShortestPath(int src, int dst) const {
    // 範囲チェック（デバッグ時のみ）
    assert(0 <= src && src < SIZE);
    assert(0 <= dst && dst < SIZE);
    if (src == dst) return {};

    constexpr int INF = std::numeric_limits<int>::max();
    std::vector<int> dist(SIZE, INF);
    struct PrevInfo { int prev; Action action; };
    std::vector<PrevInfo> prev(SIZE);
    std::deque<int> que;

    dist[src] = 0;
    que.push_back(src);

    static constexpr int di[4] = {-1, +1,  0,  0};
    static constexpr int dj[4] = { 0,  0, -1, +1};

    while (!que.empty()) {
      int u = que.front(); que.pop_front();
      int d = dist[u];
      int ui = u / N, uj = u % N;

      for (int dir = 0; dir < 4; dir++) {
        int ni = ui + di[dir];
        int nj = uj + dj[dir];
        if (0 <= ni && ni < N && 0 <= nj && nj < N) {
          int v = toIdx(ni, nj);
          if (!grid_[v] && dist[v] == INF) {
            dist[v] = d + 1;
            prev[v] = {u, Action{ActionType::Move, static_cast<Direction>(dir)}};
            if (v == dst) break;
            que.push_back(v);
          }
        }
        int v2 = slide_cache_[dir][u];
        if (v2 != u && dist[v2] == INF) {
          dist[v2] = d + 1;
          prev[v2] = {u, Action{ActionType::Slide, static_cast<Direction>(dir)}};
          if (v2 == dst) break;
          que.push_back(v2);
        }
      }
      if (dist[dst] != INF) break;
    }

    if (dist[dst] == INF) return {};

    std::vector<Action> path;
    for (int cur = dst; cur != src; ) {
      const auto &info = prev[cur];
      path.push_back(info.action);
      cur = info.prev;
    }
    std::reverse(path.begin(), path.end());
    return path;
  }

  static inline int toIdx(int i, int j) noexcept { return i * N + j; }
  static inline std::pair<int,int> toCell(int idx) noexcept { return { idx / N, idx % N }; }
};

struct State {
  array<vector<pair<int, Direction>>, M - 1> alters;

  State() {}

  int Eval() const {
    Board board;
    int cur = P[0];
    int T = 0;

    for (int k = 1; k < M; k++) {
      for (auto [pos, dir] : alters[k - 1]) {
        T += board.ShortestDistance(cur, pos);
        cur = pos;
        board.ToggleBlock(cur + DirectionDiff[int(dir)]);
        T += 1;
      }
      int dst = P[k];
      int cost = board.ShortestDistance(cur, dst);
      T += cost;
      cur = dst;
    }
    return T;
  }

  void Output() const {
    Board board;
    int cur = P[0];

    for (int k = 1; k < M; k++) {
      for (auto [pos, dir] : alters[k - 1]) {
        auto path = board.ShortestPath(cur, pos);
        for (auto &a : path) {
          cout << a << "\n";
        }
        cur = pos;
        board.ToggleBlock(cur + DirectionDiff[int(dir)]);
        cout << Action(ActionType::Alter, dir) << "\n";
      }
      int dst = P[k];

      auto path = board.ShortestPath(cur, dst);
      for (auto &a : path) {
        cout << a << "\n";
      }
      cur = dst;
    }
    fflush(stdout);
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  SetUp();

  State state;
  int cur_cost = state.Eval();

  State best_state = state;
  int best_cost = cur_cost;

  SimulatedAnnealing sa;
  while (!sa.End()) {
    int op = rnd.next(110);
    if (op < 25) {
      // Add
      int i = rnd.next(M - 1);
      int j = rnd.next(int(state.alters[i].size()) + 1);
      int from = j == 0 ? P[i] : state.alters[i][j-1].first;
      int to = j == int(state.alters[i].size()) ? P[i+1] : state.alters[i][j].first;
      int ys = from / N, xs = from % N;
      int yt = to / N, xt = to % N;
      int y = rnd.next(min(ys, yt), max(ys, yt));
      int x = rnd.next(min(xs, xt), max(xs, xt));
      int dir = rnd.next(4);
      if (dir == 0) {
        if (y == 0) continue;
      } else if (dir == 1) {
        if (y == N - 1) continue;
      } else if (dir == 2) {
        if (x == 0) continue;
      } else {
        if (x == N - 1) continue;
      }
      state.alters[i].insert(state.alters[i].begin() + j, make_pair(y * N + x, Direction(dir)));

      int cost = state.Eval();
      if (sa.Accept(cur_cost - cost)) {
        cur_cost = cost;
      } else {
        state.alters[i].erase(state.alters[i].begin() + j);
      }
    } else if (op < 50) {
      // Remove
      int i = rnd.next(M - 1);
      if (state.alters[i].empty()) continue;
      int j = rnd.next(int(state.alters[i].size()));
      pair<int, Direction> old = state.alters[i][j];
      state.alters[i].erase(state.alters[i].begin() + j);

      int cost = state.Eval();
      if (sa.Accept(cur_cost - cost)) {
        cur_cost = cost;
      } else {
        state.alters[i].insert(state.alters[i].begin() + j, old);
      }
    } else if (op < 95) {
      // ChangePos
      int i = rnd.next(M - 1);
      if (state.alters[i].empty()) continue;
      int j = rnd.next(int(state.alters[i].size()));
      pair<int, Direction> old = state.alters[i][j];

      int from = j == 0 ? P[i] : state.alters[i][j-1].first;
      int to = j == int(state.alters[i].size() - 1) ? P[i+1] : state.alters[i][j + 1].first;
      int ys = from / N, xs = from % N;
      int yt = to / N, xt = to % N;
      int y = rnd.next(min(ys, yt), max(ys, yt));
      int x = rnd.next(min(xs, xt), max(xs, xt));
      int dir = rnd.next(4);
      if (dir == 0) {
        if (y == 0) continue;
      } else if (dir == 1) {
        if (y == N - 1) continue;
      } else if (dir == 2) {
        if (x == 0) continue;
      } else {
        if (x == N - 1) continue;
      }

      state.alters[i][j] = make_pair(y * N + x, Direction(dir));

      int cost = state.Eval();
      if (sa.Accept(cur_cost - cost)) {
        cur_cost = cost;
      } else {
        state.alters[i][j] = old;
      }
    } else {
      // ChangeTiming
      int si = rnd.next(M - 1);
      int ti = rnd.next(M - 1);
      if (si == ti) continue;
      if (state.alters[si].empty()) continue;
      int sj = rnd.next(int(state.alters[si].size()));
      pair<int, Direction> old = state.alters[si][sj];
      state.alters[si].erase(state.alters[si].begin() + sj);

      int tj = rnd.next(int(state.alters[ti].size()) + 1);
      int from = tj == 0 ? P[ti] : state.alters[ti][tj-1].first;
      int to = tj == int(state.alters[ti].size()) ? P[ti+1] : state.alters[ti][tj].first;
      int ys = from / N, xs = from % N;
      int yt = to / N, xt = to % N;
      int y = old.first / N, x = old.first % N;
      if ((y < ys && y < yt) || (y > ys && y > yt) || (x < xs && x < xt) || (x > xs && x > xt)) {
        // Revert
        state.alters[si].insert(state.alters[si].begin() + sj, old);
        continue;
      }
      state.alters[ti].insert(state.alters[ti].begin() + tj, old);

      int cost = state.Eval();
      if (sa.Accept(cur_cost - cost)) {
        cur_cost = cost;
      } else {
        state.alters[ti].erase(state.alters[ti].begin() + tj);
        state.alters[si].insert(state.alters[si].begin() + sj, old);
      }
    }
    if (cur_cost < best_cost) best_state = state;
  }
  LOG(sa.iteration);

  int T = best_state.Eval();
  LOG(T);
  int score = M + 2 * N * M - T;
  LOG(score);
  best_state.Output();

  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
  return 0;
}
