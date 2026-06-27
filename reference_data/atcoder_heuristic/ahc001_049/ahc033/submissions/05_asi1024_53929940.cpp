#pragma GCC target("avx2")
#pragma GCC optimize("O3")

#define NDEBUG  // TODO

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
#define HHH(x) cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << endl
// #define LOG(x) cerr << #x << " = " << x << endl
#define LOG(x)
#define GRAPH(x, y) cerr << "graph " << x << " " << y << endl;

using namespace std;

template <typename T> T &chmin(T &a, const T &b) { return a = min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = max(a, b); }

using ll = long long;
using ld = long double;
using P = pair<int,int>;

// Template
class Timer {
public:
  int64_t start, stop;
  static const int64_t CYCLES_PER_SEC = 2900000000;
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
  Timer(double time_limit) :
    start(get_cycle()), stop(start + time_limit * CYCLES_PER_SEC) {}
  Timer() : Timer(0) {}
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
};

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int i_param = get_param("I_PARAM");
// int j_param = get_param("J_PARAM");

Timer timer(2.95);
// Timer timer(6.0);

constexpr int N = 5;
int A[N][N];
array<double, N * N> prio_table;
int g_res_cost = 1e9;

void setup() {
  int n;
  cin >> n;
  assert(n == N);
  REP(i,N) REP(j,N) cin >> A[i][j];

  array<double, N * N> next_prio;
  prio_table.fill(0);
  REP(loop,30) {
    next_prio.fill(0);
    // REP(i,N) REP(j,N-1) next_prio[i*N+j] += prio_table[i*N+j+1] * 0.3 + 1.0;
    // REP(i,N) REP(j,N-1) next_prio[A[i][j]] += prio_table[A[i][j+1]] * 0.8 + 1.0;
    REP(i,N) REP(j,N-1) next_prio[i*N+j] += prio_table[i*N+j+1] * 0.2 + 1.0;
    REP(i,N) REP(j,N-1) next_prio[A[i][j]] += prio_table[A[i][j+1]] * 0.8 + 1.0;
    swap(prio_table, next_prio);
  }
  // REP(i,N*N) cerr << prio_table[i] << endl;
}

int MAX_T = 90;  // Param

struct Op {
  int id, value;
  string com1, com2;
};

vector<Op> op_cands;

// StrList
struct StrList {
  char c;
  int prev;
};

StrList strbuf[1 << 16];
int strbuf_ptr = 0;

string get_str(int index) {
  string ret;
  while (index != -1) {
    ret += strbuf[index].c;
    index = strbuf[index].prev;
  }
  reverse(ret.begin(), ret.end());
  return ret;
}

using Pos = int8_t;

vector<int> id_order = {0, 1, 2, 3, 4};

struct Solver {
  array<int8_t, N> enter;            // [N]
  array<int8_t, N> exit;             // [N]
  vector<array<uint8_t, N * N>> history; // [MAX_T][N * N]  xxxyyyyy
  array<Pos, N> crane;               // [N]
  array<int8_t, N * N> reserved_value;     // [N * N]
  array<int8_t, N * N> reserved_cell;      // [N * N]
  vector<string> commands;
  uint64_t hash;
  double redundant_moves = 0;

  Solver() : history(MAX_T), commands(N) {
    enter.fill(1);
    exit.fill(0);
    reserved_value.fill(0);
    reserved_cell.fill(0);
    for (int t = 0; t < MAX_T; ++t) {
      history[t].fill(0x1f);
      REP(i,N) history[t][i * N] = A[i][0];
    }
    REP(i,N) crane[i] = i * N + 0;
  }
  void output() const {
    vector<string> com = commands;
    size_t max_len = 0;
    REP(i,N) max_len = max(max_len, com[i].size());
    REP(i,N) if (com[i].size() < max_len) com[i] += 'B';
    REP(i,N) cout << com[i] << endl;
  }
  uint64_t Hash() const {
    uint64_t seed = 0;
    REP(i,N) REP(j,N) {
      seed ^= history[MAX_T - 1][i * N + j] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    REP(i,N) {
      seed ^= crane[i] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      seed ^= enter[i] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
  double GetCost() const {
    const auto& table = history[MAX_T - 1];
    vector<int> dist(N * N, 999);
    // using P = pair<int, int>;
    // priority_queue<P, vector<P>, greater<P>> que;
    static deque<pair<int, int>> que;
    que.clear();
    que.emplace_back(0, 4);
    dist[4] = 0;
    while (!que.empty()) {
      auto [d, v] = que.front();
      que.pop_front();
      if (d > dist[v]) continue;
      int y = v / N, x = v % N;
      auto push = [&](int nv) -> void {
        int c = (table[nv] & 0x1f) == 0x1f ? 0 : 1;
        int new_d = d + c;
        if (new_d >= dist[nv]) return;
        dist[nv] = new_d;
        if (c == 0) que.emplace_front(new_d, nv);
        else que.emplace_back(new_d, nv);
      };
      if (x > 0) push(v - 1);
      if (x < 4) push(v + 1);
      if (y > 0) push(v - N);
      if (y < 4) push(v + N);
    }
    // cerr << "reachable" << endl;
    // REP(i,N) {
    //   REP(j,N) cerr << dist[i * N + j] << " ";
    //   cerr << endl;
    // }
    double cost = 0;
    REP(i,N) REP(j,N) {
      int value = table[i * N + j] & 0x1f;
      if (value == 0x1f) continue;
      int d = abs(i - value / N) + 4 - j;
      cost += (0.355 + dist[i * N + j] * 0.1675) * d + 0.211;  // Param
    }
    REP(i,N) {
      for (int j = enter[i]; j < 5; ++j) {
        int value = A[i][j];
        int d = abs(i - value / N) + 4;
        // cost += 0.650 * d + (prio_table[value] + 2.5) * 0.162;
        cost += 0.550 * d + (prio_table[value] + 1.8) * 0.22;
      }
    }
    cost *= 0.720;
    cost += redundant_moves * 0.33;  // Param

    // Inversion Number
    int inv_num = 0;
    array<int, N * N> inv;
    inv.fill(0);
    REP(i,N) {
      REP(j,N) {
        uint8_t value = table[i * N + j] & 0x1f;
        if (value == 0x1f) continue;
        inv[value] = 4 - j;
      }
      for (int j = exit[i]; j < N; ++j) {
        int value = A[i][j];
        inv[value] = 5 + j - exit[i];
      }
    }
    REP(i,N) {
      REP(j,N-2) {
        if (inv[i * N + j] > inv[i * N + j + 1]) ++inv_num;
      }
    }
    cost *= 0.880;
    cost += inv_num * 0.160;

    double penalty = commands[0].size() + cost * 0.323;  // Param
    double size = 0;
    REP(i,N) size += commands[i].size();
    chmax(penalty, size * 0.180 + cost);  // Param
    REP(i,N) chmax(penalty, double(commands[i].size()));
    return penalty;
  }
  void Dump(int t) const {
    cerr << "===== BEGIN =====" << endl;
    REP(i,N) {
      REP(j,N) cerr << (history[t][i * N + j] & 0x1f) << " ";
      cerr << endl;
    }
    // cerr << "- reserved cell" << endl;
    // REP(i,N) {
    //   REP(j,N) cerr << reserved_cell[i * N + j] << " ";
    //   cerr << endl;
    // }
    REP(id,N) {
      int y = crane[id] / N;
      int x = crane[id] % N;
      cerr << "#" << id << ": (" << y << ", " << x << ")" << endl;
    }
    cerr << "====== END ======" << endl;
  }
  void Dump() const {
    return Dump(MAX_T - 1);
  }
  bool IsOK() const {
    for (int i = 0; i < N; ++i) {
      if (exit[i] < 5) return false;
    }
    return true;
  }
  pair<int, int> GetRoute(int t, Pos src, Pos dest, bool allow_overlap) const {
    if (t >= MAX_T - 1) return {-1, -1};
    // TODO A-star like
    // ++cnt_getroute;
    // cerr << ">> Start GetRoute" << endl;

    if (src == dest) {
      uint8_t value = history[t + 1][dest];
      if ((value >> 5) != 7 && (allow_overlap || (value & 0x1f) == 0x1f)) return {0, -1};
    }

    array<int, N * N> dp;
    dp.fill(-2);
    {
      auto board = history[t + 1];
      auto push = [&](Pos to, char c, uint8_t dir) -> void {
        if (dp[to] != -2) return;
        uint8_t crane_conflict = board[to] >> 5;
        if (crane_conflict == 7 || crane_conflict == dir) return;
        if (!allow_overlap && (board[to] & 0x1f) != 0x1f) return;
        strbuf[strbuf_ptr] = {c, -1};
        dp[to] = strbuf_ptr++;
        assert (strbuf_ptr < (1 << 16));
      };
      push(src, '.', 7);
      if (src % N > 0) push(src - 1, 'L', 3);
      if (src / N < 4) push(src + N, 'D', 2);
      if (src / N > 0) push(src - N, 'U', 4);
      if (src % N < 4) push(src + 1, 'R', 1);
    }

    array<int, N * N> next_dp;
    next_dp.fill(-2);
    for (int loop = 1; loop < 60; ++loop) {
      int tt = t + loop;
      if (tt >= MAX_T - 1) return {-1, -1};
      if (dp[dest] != -2) {
        uint8_t value = history[tt + 1][dest];
        if ((value >> 5) != 7 && (allow_overlap || (value & 0x1f) == 0x1f)) return {loop, dp[dest]};
      }
      REP(pos,25) {
        if (dp[pos] == -2) continue;
        auto board = history[tt + 1];
        auto push = [&](Pos to, char c, uint8_t dir) -> void {
          if (next_dp[to] != -2) return;
          uint8_t crane_conflict = board[to] >> 5;
          if (crane_conflict == 7 || crane_conflict == dir) return;
          if (!allow_overlap && (board[to] & 0x1f) != 0x1f) return;
          strbuf[strbuf_ptr] = {c, dp[pos]};
          next_dp[to] = strbuf_ptr++;
          assert (strbuf_ptr < (1 << 16));
        };
        push(pos, '.', 7);
        if (pos % N > 0) push(pos - 1, 'L', 3);
        if (pos / N < 4) push(pos + N, 'D', 2);
        if (pos / N > 0) push(pos - N, 'U', 4);
        if (pos % N < 4) push(pos + 1, 'R', 1);
      }
      swap(dp, next_dp);
      next_dp.fill(-2);
    }
    return {-1, -1};
  }
  void Move(int id, string str) {
    int t = commands[id].size();
    int y = crane[id] / N;
    int x = crane[id] % N;
    assert (y >= 0 && x >= 0 && y < 5 && x < 5);
    assert (str != "X"); // Bomb
    chmax(reserved_cell[y * N + x], int8_t(t));
    for (char c : str) {
      if (c == 'R') {
        history[t + 1][y * N + x] |= (uint8_t{3} << 5); // crane
        ++x;
      } else if (c == 'L') {
        history[t + 1][y * N + x] |= (uint8_t{1} << 5); // crane
        --x;
      } else if (c == 'D') {
        history[t + 1][y * N + x] |= (uint8_t{4} << 5); // crane
        ++y;
      } else if (c == 'U') {
        history[t + 1][y * N + x] |= (uint8_t{2} << 5); // crane
        --y;
      } else if (c == '.') {}
      else assert(false);
      ++t;
      history[t][y * N + x] |= 0xe0;  // crane
      assert (y >= 0 && x >= 0 && y < 5 && x < 5);
      chmax(reserved_cell[y * N + x], int8_t(t));
    }
    crane[id] = y * N + x;
    commands[id] += str;
  }
  void Pick(int id) {
    int t = commands[id].size();
    commands[id] += 'P';
    int y = crane[id] / N;
    int x = crane[id] % N;
    assert (y >= 0 && x >= 0 && y < 5 && x < 5);
    crane[id] = y * N + x;

    int next_value = 0x1f;
    if (x == 0 && enter[y] < 5) {
      next_value = A[y][enter[y]];
      enter[y] += 1;
    }
    int value = history[t][y * N + x] & 0x1f;
    assert (reserved_value[value] <= t);
    assert (value != 0x1f);
    for (int tt = t + 1; tt < MAX_T; ++tt) {
      assert ((history[tt][y * N + x] & 0x1f) == value);
      history[tt][y * N + x] &= 0xe0;
      history[tt][y * N + x] |= (next_value & 0x1f);
    }
    if (next_value != 0x1f) reserved_value[next_value] = t + 1;
    history[t + 1][y * N + x] |= 0xe0; // crane
  }
  void Put(int id, int value) {
    int t = commands[id].size();
    commands[id] += 'Q';
    int y = crane[id] / N;
    int x = crane[id] % N;
    assert (y >= 0 && x >= 0 && y < 5 && x < 5);
    crane[id] = y * N + x;
    history[t + 1][y * N + x] |= 0xe0; // crane

    if (x == 4 && exit[y] < 5) {
      assert (value % N == exit[y]);
      exit[y] += 1;
      assert (reserved_cell[y * N + x] <= t);
      reserved_cell[y * N + x] = t + 1;
      return;
    }
    for (int tt = t + 1; tt < MAX_T; ++tt) {
      assert ((history[tt][y * N + x] & 0x1f) == 0x1f);
      history[tt][y * N + x] &= (0xe0 | value);
    }
    reserved_value[value] = t + 1;
  }
  Pos LastPosOf(int id) const {
    return crane[id];
  }
  int LastValueOf(Pos pos) const {
    return history[MAX_T - 1][pos] & 0x1f;
  }
  void ApplyCommand(const Op& op) {
    // int t = commands[op.id].size();
    for (char c : op.com1) {
      if (c == 'R') redundant_moves += 2;
      else if (c != 'L') redundant_moves += 1;
    }
    int u = 0, d = 0;
    for (char c : op.com2) {
      if (c == 'L') redundant_moves += 2;
      else if (c == '.') redundant_moves += 1;
      else if (c == 'U') ++u;
      else if (c == 'D') ++d;
    }
    redundant_moves += 2 * min(u, d);
    Move(op.id, op.com1);
    assert (reserved_value[op.value] <= int(commands[op.id].size()));
    Pick(op.id);
    Move(op.id, op.com2);
    Put(op.id, op.value);
    int x = crane[op.id] % N;
    if (x < 4) redundant_moves += 2.0;  // Param
    reserved_value[op.value] = commands[op.id].size();
    hash = Hash();
  }
  bool TryMove(Pos from, Pos to) const {
    strbuf_ptr = 0;
    int value = LastValueOf(from);
    static vector<tuple<int, int, int, int>> cands;
    cands.clear();
    int min_t = g_res_cost - 1;
    // REP(id, N) {
    for (int id : id_order) {
      // HHH(id);
      int t = commands[id].size();
      auto pos = crane[id];
      int min1 = abs(pos / N - from / N) + abs(pos % N - from % N);
      int min2 = abs(to / N - from / N) + abs(to % N - from % N);
      if (t + min1 + min2 + 2 > min_t) continue;
      auto [len1, com1] = GetRoute(t, pos, from, true);  // TODO: merge
      if (len1 == -1) continue;
      // assert (int(get_str(com1).size()) == len1);
      t += len1 + 1;
      if (t < reserved_value[value]) break; // continue;
      if (t < reserved_cell[from]) break; // continue;
      if (t + min2 + 1 > min_t) continue;
      auto [len2, com2] = GetRoute(t, from, to, id == 0);  // TODO: merge
      // HHH(com2);
      if (len2 == -1) continue;
      // assert (int(get_str(com2).size()) == len2);
      t += len2 + 1;
      if (t < reserved_cell[to]) break; // continue;
      if (t >= g_res_cost) continue;
      if (id == 0) t += 1; // Param t == 0 penalty
      chmin(min_t, t);
      // t = t * 2 + (t == 0);
      cands.emplace_back(t, id, com1, com2);
    }
    if (cands.empty()) return false;

    std::sort(cands.begin(), cands.end(),
              [](const auto& l, const auto& r) {
                return std::get<0>(l) < std::get<0>(r);
              });
    for (size_t i = 0; i < cands.size(); ++i) {
      auto [t, id, com1, com2] = cands[i];
      if (t - std::get<0>(cands[0]) > 0) break;
      op_cands.push_back(Op{id, value, get_str(com1), get_str(com2)});
    }
    return true;
  }

  void Step() const {
    std::sort(id_order.begin(), id_order.end(), [&](int i, int j) {
      return commands[i].size() < commands[j].size();
    });

    // ToGoal
    {
      // TODO prio
      for (int j = 3; j >= 0; --j) {
        for (int i = 0; i < N; ++i) {
          int value = LastValueOf(i * N + j);
          if (value == 0x1f) continue;
          if (exit[value / N] == 5) continue;
          if (exit[value / N] != value % N) continue;
          TryMove(i * N + j, value / N * N + 4);
        }
      }

      // Setup
      vector<int> prio = {0, 4, 1, 3, 2};
      for (int row : prio) {
        if (enter[row] < 5) {
          int value = history[MAX_T - 1][row * N] & 0x1f;
          assert (value != -1);
          if (exit[value / N] == value % N) continue;
          for (int i = 0; i < N; ++i) {
            if (abs(row - i) + abs(i - value / N) > abs(row - value / N) + 1) continue;
            int col = -1;
            for (int j = 1; j <= 3; ++j) {
              if ((history[MAX_T - 1][i * N + j] & 0x1f) == 0x1f) col = j;
            }
            if (col != -1) TryMove(row * N + 0, i * N + col);
          }
        }
      }
    }
  }
};

int main() {
  setup();
  Solver res;

  // Naive
  {
    Solver solver;
    for (int i = 0; i < 300; ++i) {
      if (solver.IsOK()) break;
      op_cands.clear();
      solver.Step();
      if (op_cands.empty()) break;
      solver.ApplyCommand(op_cands[0]);
    }
    if (solver.IsOK()) {
      res = solver;
      g_res_cost = solver.GetCost();
    }
  }
  // cerr << "cost: " << g_res_cost << endl;

  // Beam
  {
    constexpr int MAX_ITER = 50;
    Solver solver;
    unordered_map<uint64_t, double> memo;
    memo.reserve(1 << 20);
    using P = pair<double, Solver>;
    auto compare = [](const P& a, const P& b) { return a.first < b.first; };
    // using PQue = priority_queue<P, vector<P>, decltype(compare)>;
    using PQue = std::set<P, decltype(compare)>;
    PQue beams[MAX_ITER];
    beams[0].emplace(solver.GetCost(), std::move(solver));
    int beam_width = 0;
    int min_iter = 999;
    int max_pque = 0;
    int64_t timer_start = timer.get_cycle();
    for (beam_width = 0;; beam_width++) {
      // if (beam_width % 500 == 0) {
      //   cerr << "beam_width: " << beam_width << endl;
      //   REP(i, MAX_ITER) cerr << i << " " << beams[i].size() << endl;
      // }
      int it_start = 12 * double(timer.get_cycle() - timer_start) / (timer.stop - timer_start);
      REP(iter,it_start) beams[iter] = PQue();
      for (int iter = it_start; iter < MAX_ITER - 1; ++iter) {
        if (!timer.within()) {
          // Exit
          LOG(beam_width);
          assert(g_res_cost < 1e9);
          res.output();
          double time = (timer.get_cycle() - timer.start) / (double)timer.CYCLES_PER_SEC;
          LOG(time);
          LOG(memo.size());
          LOG(min_iter);
          // LOG(cnt_getroute);
          LOG(max_pque);
          std::_Exit(0);
          return 0;
        }
        chmax(max_pque, int(beams[iter].size()));
        if (beams[iter].empty()) continue;
        Solver solver = std::move(beams[iter].begin()->second);
        solver.history.resize(MAX_T);
        beams[iter].erase(beams[iter].begin());
        int size = 0;
        REP(i,N) chmax(size, int(solver.commands[i].size()));
        if (size >= g_res_cost) { --iter; continue; }
        
        op_cands.clear();
        solver.Step();

        for (size_t i_op = 0; i_op < op_cands.size(); ++i_op) {
          const auto& op = op_cands[i_op];
          if (int(solver.commands[op.id].size() + op.com1.size() + op.com2.size() + 2) >= g_res_cost) continue;
          Solver tmp_solver = i_op == op_cands.size() - 1 ? std::move(solver) : solver;
          tmp_solver.ApplyCommand(op);
          int tmp_size = 0;
          REP(i,N) chmax(tmp_size, int(tmp_solver.commands[i].size()));

          if (tmp_solver.IsOK()) {
            if (tmp_size < g_res_cost) {
              res = tmp_solver;
              g_res_cost = tmp_size;
              // cerr << "cost: " << g_res_cost << endl;
              chmin(MAX_T, g_res_cost + 2);
              chmin(min_iter, iter);
            }
          } else {
            if (tmp_size >= g_res_cost) continue;
            double tmp_cost = tmp_solver.GetCost();
            if (memo.count(tmp_solver.hash) && tmp_cost >= memo[tmp_solver.hash]) continue;
            memo[tmp_solver.hash] = tmp_cost;
            beams[iter + 1].emplace(tmp_cost, std::move(tmp_solver));
            if (beams[iter + 1].size() > 1000) beams[iter + 1].erase(std::prev(beams[iter + 1].end()));
          }
        }
      }
    }
  }
}
