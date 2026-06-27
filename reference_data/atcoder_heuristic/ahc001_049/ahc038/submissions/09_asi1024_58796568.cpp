#pragma GCC optimize("O3")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")

#define NDEBUG  // TODO

// #include <atcoder/mincostflow>

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
#define HHH(x) cerr << "L" << __LINE__ << ": " << #x << " : " << (x) << endl
#define LOG(x)
#define GRAPH(x, y) cerr << "graph " << x << " " << y << endl;

using namespace std;

template <typename T> T &chmin(T &a, const T &b) { return a = min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = max(a, b); }

using ll = long long;
using ld = double;
using P = pair<int,int>;

// Template
ostream& operator<<(ostream& os, const P& p) {
  return os << "(" << p.first << ", " << p.second << ")";
}

template <typename T> ostream& operator<<(ostream& os, const vector<T>& vec) {
  os << "[";
  for (const T& e : vec) os << " " << e;
  os << " ]";
  return os;
}

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

class XorShift {
  unsigned int x, y, z, w;
  double nL[65536];
public:
  XorShift() : x(314159265), y(358979323), z(846264338), w(327950288) {}
  inline unsigned int next() {
    unsigned int t = x ^ x << 11;
    x = y; y = z; z = w;
    return w = w ^ w >> 19 ^ t ^ t >> 8;
  }
  inline int next(int n) { return next() % n; }
  inline int next(int lo, int hi) { return lo + next(hi - lo + 1); }
  inline double next(double x) { return double(next()) / (1LL << 32) * x; }
};

double get_param(const char* env_var) {
  char* ret = getenv(env_var);
  assert (ret != nullptr);
  return stod(string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int a_param = get_param("A_PARAM");
// int j_param = get_param("J_PARAM");

Timer timer(2.97);
XorShift rnd;

struct BipartiteMatching {
  int V;
  vector<vector<int>> g;
  vector<int> match;
  vector<bool> used;
  bool dfs(int v) {
    used[v] = true;
    for (int u: g[v]) {
      int w = match[u];
      if (w < 0 || (!used[w] && dfs(w))) {
        match[v] = u; match[u] = v;
        return true;
      }
    }
    return false;
  }

  BipartiteMatching (int v) : V(v), g(v), match(v), used(v) {}
  void Reset() {
    REP(v,V) g[v].clear();
    std::fill(ALL(match), -1);
    std::fill(ALL(used), false);
  }
  void add_edge(int u, int v) {
    assert (u < V);
    assert (v < V);
    g[u].push_back(v);
    g[v].push_back(u);
  }
  int maximum_matching(const std::deque<int>& nodes) {
    int res = 0;
    // fill (ALL(match), -1);
    for (int v: nodes) {
      if (match[v] < 0) {
        fill (ALL(used), 0);
        if (dfs(v)) res++;
      }
    }
    return res;
  }
};

constexpr int MAX_N = 30;
constexpr int MAX_M = MAX_N * MAX_N / 2;
constexpr int MAX_V = 15;
int N, M, V;
P From[MAX_M], To[MAX_M];

const int prio_base[16][16] = {
  {99, 94, 86, 78, 70, 71, 79, 87, 95, 96},
  {93, 63, 59, 51, 42, 43, 44, 52, 60, 88},
  {85, 58, 34, 28, 20, 21, 29, 35, 53, 80},
  {77, 50, 27, 12,  5,  6, 13, 30, 45, 72},
  {69, 41, 19,  4,  0,  1,  7, 22, 36, 64},
  {68, 40, 18, 11,  3,  2,  8, 23, 37, 65},
  {76, 49, 26, 15, 10,  9, 14, 31, 46, 73},
  {84, 57, 33, 25, 17, 16, 24, 32, 54, 81},
  {92, 62, 56, 48, 39, 38, 47, 55, 61, 89},
  {98, 91, 83, 75, 67, 66, 74, 82, 90, 97},
};

int prio_y[100], prio_x[100];

void setup() {
  std::cin >> N >> M >> V;
  int M0 = M;
  vector<string> fr_s(N), to_s(N);
  REP(i,N) std::cin >> fr_s[i];
  REP(i,N) std::cin >> to_s[i];

  int count_f = 0, count_t = 0;
  REP(i,N) REP(j,N) {
    if (fr_s[i][j] == '1' && to_s[i][j] == '1') { --M; continue; }
    if (fr_s[i][j] == '1') From[count_f++] = {i, j};
    if (to_s[i][j] == '1') To[count_t++] = {i, j};
  }
  assert(count_f == M);
  assert(count_t == M);
  LOG(N);
  LOG(M0);
  LOG(M);
  LOG(V);

  memset(prio_y, -1, sizeof(prio_y));
  REP(i,10) REP(j,10) {
    assert(prio_y[prio_base[i][j]] == -1);
    prio_y[prio_base[i][j]] = i;
    prio_x[prio_base[i][j]] = j;
  }
}

constexpr int dx[4] = {1, 0, -1, 0};
constexpr int dy[4] = {0, 1, 0, -1};
constexpr string dir_char = "RDLU";

struct State {
  int y_init, x_init, y0, x0;
  vector<int> par, len, dir;
  vector<string> com;
  int cost() const {
    if (com.empty()) return 999999999;
    return com.size();
  }
  State() : y_init(0), x_init(0), y0(0), x0(0) {}
  State(int y_init, int x_init) :
    y_init(y_init), x_init(x_init), y0(y_init), x0(x_init),
    par(V - 1, -1), len(V - 1, 1), dir(V - 1, 0) {}
  void move(int dir) {
    int t = com.size() - 1;
    while (t >= 0 && com[t][0] == '.' && std::count(com[t].begin() + V, com[t].end(), 'P') == 0) --t;
    ++t;
    x0 += dx[dir];
    y0 += dy[dir];
    if (int(com.size()) == t) com.emplace_back(V * 2, '.');
    com[t][0] = dir_char[dir];
  }
  void move_to(int y, int x) {
    if (com.empty()) {
      y_init = y0 = y;
      x_init = x0 = x;
      return;
    }
    while (x0 < x) move(0);
    while (x0 > x) move(2);
    while (y0 < y) move(1);
    while (y0 > y) move(3);
  }
  tuple<int, int, int> get_dir_coordinate(int id) const {
    int y = y0, x = x0, d = 0;
    if (par[id] != -1) std::tie(y, x, d) = get_dir_coordinate(par[id]);
    d = (d + dir[id]) & 3;
    y += dy[d] * len[id];
    x += dx[d] * len[id];
    return {y, x, d};
  }
  int get_coordinate(int id) const {
    auto [y, x, d] = get_dir_coordinate(id);
    return y * N + x;
  }
  void output() const {
    cout << V << endl;
    REP(i,V-1) cout << par[i] + 1 << " " << len[i] << endl;
    cout << y_init << " " << x_init << endl;
    for (const string& s : com) cout << s << endl;
  }
  bool can_rot(int id, int t) const {
    vector<bool> picked(V - 1, false);
    for (int i = V - 2; i >= id; --i) {
      if (com[t][i + V + 1] == 'P') picked[i] = true;
      if (picked[i] && par[i] >= 0) picked[par[i]] = true;
    }
    return !picked[id];
  }
  void rot(int id, char dir_char) {
    int t = com.size() - 1;
    while (t >= 0 && com[t][id + 1] == '.' && can_rot(id, t)) --t;
    ++t;
    if (int(com.size()) == t) com.emplace_back(V * 2, '.');
    if (dir_char == 'R') dir[id] = (dir[id] + 1) & 3;
    else dir[id] = (dir[id] + 3) & 3;
    com[t][id + 1] = dir_char;
  }
  void rot_diff(int id, int dir_diff) {
    switch (dir_diff & 3) {
    case 0: { return; }
    case 1: { rot(id, 'R'); return; }
    case 3: { rot(id, 'L'); return; }
    default: { rot(id, 'R'); rot(id, 'R'); return; }
    }
  }
  int get_dir(int id) const {
    int res = 0;
    while (id >= 0) {
      res = (res + dir[id]) & 3;
      id = par[id];
    }
    return res;
  }
  void rot_to(int id, int new_dir) {
    new_dir = (new_dir - get_dir(id)) & 3;
    rot_diff(id, new_dir);
  }
  void pick(int id) {
    if (com.empty() || com.back()[V + id + 1] != '.') com.emplace_back(V * 2, '.');
    com.back()[V + id + 1] = 'P';
  }
};

State best_state;
int loop_count = 0;
int choice = 0;

void EarlyExit() {
  LOG(choice);
  LOG(best_state.cost());
  double occupancy = double(M) / best_state.cost() / V * 400;
  LOG(loop_count);
  LOG(occupancy);
  LOG(best_state.len);
  best_state.output();
  double elapsed = (timer.get_cycle() - timer.start) / double(Timer::CYCLES_PER_SEC);
  LOG(elapsed);
  std::_Exit(0);
}

// vector<int> get_rots(int &dy, int &dx, const vector<int>& lens) {
//   vector<int> res;
//   for (int len: lens) {
//     if (abs(dy) > abs(dx)) {
//       if (dy > 0) {
//         res.push_back(1);
//         dy -= len;
//       } else {
//         res.push_back(3);
//         dy += len;
//       }
//     } else {
//       if (dx > 0) {
//         res.push_back(0);
//         dx -= len;
//       } else {
//         res.push_back(2);
//         dx += len;
//       }
//     }
//   }
//   return res;
// }

// namespace v5 {

// using Move = std::tuple<std::pair<int, int>, std::pair<int, int>, pair<int, int>>;

// State SolveLine(int n_childs, int max_len, const vector<Move>& moves) {
//   auto [y_init, x_init] = std::get<0>(moves[0]);
//   State state(y_init, x_init);
//   REP(i,n_childs) {
//     state.par[i] = i - 1;
//     state.len[i] = max_len;
//     max_len /= 2;
//   }
//   vector<int> lens(state.len.begin(), state.len.begin() + n_childs);

//   REP(index,M) {
//     auto [y_root, x_root] = std::get<0>(moves[index]);
//     {
//       pair<int, int> pos = std::get<1>(moves[index]);
//       int dy = pos.first - y_root;
//       int dx = pos.second - x_root;
//       vector<int> rots = get_rots(dy, dx, lens);
//       REP(i,n_childs) state.rot_to(i, rots[i]);
//       state.move_to(y_root + dy, x_root + dx);
//     }
//     state.pick(n_childs - 1);
//     {
//       pair<int, int> pos = std::get<2>(moves[index]);
//       int dy = pos.first - y_root;
//       int dx = pos.second - x_root;
//       vector<int> rots = get_rots(dy, dx, lens);
//       REP(i,n_childs) state.rot_to(i, rots[i]);
//       state.move_to(y_root + dy, x_root + dx);
//     }
//     state.pick(n_childs - 1);
//   }
//   return state;
// }

// State SolveMinMove(int n_childs) {
//   int max_len = (1 << n_childs) - 1;
//   atcoder::mcf_graph<int, int> graph(M * 2 + 2);
//   int src = M * 2;
//   int sink = M * 2 + 1;
//   REP(i,M) graph.add_edge(src, i, 1, 0);
//   REP(i,M) graph.add_edge(M + i, sink, 1, 0);
//   vector<vector<int>> edges(M, vector<int>(M));
//   REP(i,M) REP(j,M) {
//     int dy = From[i].first - To[j].first;
//     int dx = From[i].second - To[j].second;
//     int cost = max(0, abs(dy) + abs(dx) - max_len);
//     edges[i][j] = graph.add_edge(i, M + j, 1, cost);
//   }
//   auto [maxmat, _] = graph.flow(src, sink);
//   assert(maxmat == M);

//   if (maxmat < M) return State();
//   vector<Move> moves;
//   REP(i,M) REP(j,M) {
//     if (graph.get_edge(edges[i][j]).flow == 1) {
//       int cy = (From[i].first + To[j].first) / 2;
//       int cx = (From[i].second + To[j].second) / 2;
//       chmax(cy, 1); chmin(cy, N - 2);
//       chmax(cx, 1); chmin(cx, N - 2);
//       moves.emplace_back(P(cy, cx), From[i], To[j]);
//     }
//   }
//   assert(int(moves.size()) == M);
//   auto prio = [&](const Move& mov) {
//     auto [y, x] = std::get<0>(mov);
//     return y * N + (y % 2 == 0 ? x : N - x - 1);
//   };
//   std::sort(ALL(moves), [&](const Move& a, const Move& b) { return prio(a) < prio(b); });
//   return SolveLine(n_childs, 1 << (n_childs - 1), moves);
// }

// }  // namespace v5

// State SolveDual(int n_childs, int n_arms = 2) {
//   int y_init = N / 2, x_init = N / 2;
//   State state(y_init, x_init);

//   REP(index2, n_arms) {
//     int max_len = 1 << (n_childs - 1);
//     REP(i,n_childs) {
//       state.par[i + n_childs * index2] = i - 1 + n_childs * index2;
//       state.len[i + n_childs * index2] = min(N - 1, max_len);  // TODO: limit
//       max_len /= 2;
//     }
//     state.par[index2 * n_childs] = -1;
//   }
//   vector<int> lens(state.len.begin(), state.len.begin() + n_childs);
//   int lensum = accumulate(ALL(lens), 0);

//   vector<bool> used_from(M, false), used_to(M, false);

//   for (int index = 0; index < M; index += n_arms) {
//     REP(index2, n_arms) {
//       if (index + index2 >= M) break;
//       int best = -1, best_cost = 9;
//       REP(i,M) {
//         int cost = 0;
//         if (used_from[i]) continue;
//         int y_root = y_init, x_root = x_init;
//         P pos = From[i];
//         if ((pos.first + pos.second + y_root + x_root + lensum) & 1) {
//           x_root -= 1; cost += 2 * (n_arms - 1);
//         }
//         int dy = pos.first - y_root;
//         int dx = pos.second - x_root;
//         vector<int> rots = get_rots(dy, dx, lens);
//         REP(i,n_childs) rots[i] = (rots[i] - state.get_dir(i + n_childs * index2)) & 3;
//         for (int i = n_childs - 2; i >= 0; --i) rots[i+1] -= rots[i];
//         if (std::count(ALL(rots), 2) >= 1) cost++;
//         if (cost < best_cost) { best_cost = cost; best = i; }
//       }
//       used_from[best] = true;
//       int y_root = y_init, x_root = x_init;
//       P pos = From[best];
//       if ((pos.first + pos.second + y_root + x_root + lensum) & 1) x_root -= 1;
//       int dy = pos.first - y_root;
//       int dx = pos.second - x_root;
//       vector<int> rots = get_rots(dy, dx, lens);
//       REP(i,n_childs) state.rot_to(i + n_childs * index2, rots[i]);
//       state.move_to(y_root + dy, x_root + dx);
//       state.pick(n_childs - 1 + n_childs * index2);
//     }
//     REP(index2, n_arms) {
//       if (index + index2 >= M) break;
//       int best = -1, best_cost = 9;
//       REP(i,M) {
//         int cost = 0;
//         if (used_to[i]) continue;
//         int y_root = y_init, x_root = x_init;
//         P pos = To[i];
//         if ((pos.first + pos.second + y_root + x_root + lensum) & 1) {
//           x_root -= 1; cost += 2 * (n_arms - 1);
//         }
//         int dy = pos.first - y_root;
//         int dx = pos.second - x_root;
//         vector<int> rots = get_rots(dy, dx, lens);
//         REP(i,n_childs) rots[i] = (rots[i] - state.get_dir(i + n_childs * index2)) & 3;
//         for (int i = n_childs - 2; i >= 0; --i) rots[i+1] -= rots[i];
//         if (std::count(ALL(rots), 2) >= 1) cost++;
//         if (cost < best_cost) { best_cost = cost; best = i; }
//       }
//       used_to[best] = true;
//       int y_root = y_init, x_root = x_init;
//       P pos = To[best];
//       if ((pos.first + pos.second + y_root + x_root + lensum) & 1) x_root -= 1;
//       int dy = pos.first - y_root;
//       int dx = pos.second - x_root;
//       vector<int> rots = get_rots(dy, dx, lens);
//       // assert(dy == 0 && dx == 0);
//       REP(i,n_childs) state.rot_to(i + n_childs * index2, rots[i]);
//       state.move_to(y_root + dy, x_root + dx);
//       state.pick(n_childs - 1 + n_childs * index2);
//     }
//   }
//   return state;
// }

vector<pair<P, int>> GetNeighbors(const vector<int>& leaf_len) {
  std::map<P, int> mp;
  REP(i,leaf_len.size()) REP(j,leaf_len.size()) {
    if (i == j) continue;
    REP(dir1,4) REP(dir2,4) {
      int y_ = dy[dir1] * leaf_len[i] + dy[dir2] * leaf_len[j];
      int x_ = dx[dir1] * leaf_len[i] + dx[dir2] * leaf_len[j];
      if (y_ == 0 && x_ == 0) continue;
      mp[{y_, x_}]++;
    }
  }
  return vector<pair<P, int>>(mp.begin(), mp.end());
}

float GetCost1(int leaf_size, const vector<pair<P, int>>& neighbors, const vector<int>& board) {
  constexpr float doubley_rate = 0.10;  // param
  float cost = 0;
  // REP(y,N) {
  //   REP(x,N) cerr << board[y * N + x] << " ";
  //   cerr << endl;
  // }

  float c = doubley_rate / leaf_size;

  REP(xy, N * N) {
    if (board[xy] == 0) continue;
    int y = xy / N, x = xy % N;
    float isolation = 1.0;
    for (auto [diff, cnt]: neighbors) {
      int y_ = y + diff.first;
      int x_ = x + diff.second;
      if (y_ < 0 || y_ >= N || x_ < 0 || x_ >= N) continue;
      if (board[y_ * N + x_] != 0) {
        isolation *= 1 - c * cnt;
      }
    }
    cost += isolation;
  }
  return cost;
}

int y_count[32], x_count[32];

float GetCost2(const vector<int>& board) {
  float cost = 0;
  memset(y_count, 0, sizeof(y_count));
  memset(x_count, 0, sizeof(x_count));
  REP(xy, N * N) {
    if (board[xy] == 0) continue;
    int y = xy / N, x = xy % N;
    y_count[y] += 1;
    x_count[x] += 1;
  }
  REP(i,N) {
    if (y_count[i] > 0) cost -= 10.0 / (1 << y_count[i]);
    if (x_count[i] > 0) cost -= 10.0 / (1 << x_count[i]);
    // cost -= 0.01 * (1 << y_count[i]);
    // cost -= 0.01 * (1 << x_count[i]);
  }
  return cost;
}

float GetCost3(const vector<int>& board) {
  float cost = 0;
  REP(i,N) REP(j,N) {
    int xy = i * N + j;
    if (board[xy] == 0) continue;
    int cnt1 = 0, cnt2 = 0;
    REP(k,9) {
      int y = i + k - 4;
      if (0 <= y && y < N && board[y * N + j] != 0) cnt1++;
      int x = j + k - 4;
      if (0 <= x && x < N && board[i * N + x] != 0) cnt2++;
    }
    cost -= std::pow(1.1, std::max(cnt1, cnt2));
  }
  return cost;
}

vector<pair<float, int>> matvec;
deque<int> matnodes;

vector<float> EvalBoard(const vector<int>& arm_len, const vector<int>& leaf_len, int root_dy, int root_dx) {
  const int arm_size = arm_len.size();
  const int box_size = (arm_size <= 2 ? 100 : (arm_size <= 3 ? 64 : 36));
  std::map<int, int> se;
  for (int x : leaf_len) se[x] += 1;
  vector<pair<int, int>> lens(se.begin(), se.end());
  vector<float> reach(N * N);
  REP(bit, 1 << (2 * arm_size)) {
    int y = N / 2 + 4 + root_dy;
    int x = N / 2 + 4 + root_dx;
    REP(i,arm_size) {
      int d = (bit >> (2 * (arm_size - i - 1))) & 3;
      y += dy[d] * arm_len[i];
      x += dx[d] * arm_len[i];
    }
    if (y < -7 || x < -7 || y >= N + 7 || x >= N + 7) continue;
    for (auto [len, cnt] : lens) {
      REP(i,box_size) REP(d,4) {
        int ny = y + dy[d] * len - prio_y[i];
        int nx = x + dx[d] * len - prio_x[i];
        if (ny < 0 || nx < 0 || ny >= N || nx >= N) continue;
        reach[ny * N + nx] += 1;
      }
    }
  }
  return reach;
}

vector<float> StepBoard(int leaf_size, const vector<pair<P, int>>& neighbors, const vector<int>& board) {
  constexpr float doubley_rate = 0.10;  // param
  vector<float> res(N * N);
  float c = doubley_rate / leaf_size;

  REP(xy, N * N) {
    if (board[xy] == 0) continue;
    int y = xy / N, x = xy % N;
    float isolation = 1.0;
    for (auto [diff, cnt]: neighbors) {
      int y_ = y + diff.first;
      int x_ = x + diff.second;
      if (y_ < 0 || y_ >= N || x_ < 0 || x_ >= N) continue;
      if (board[y_ * N + x_] != 0) {
        isolation *= std::max(0.0f, 1 - c * cnt);
      }
    }
    res[xy] = isolation;
  }
  return res;
}

std::map<pair<ll, ll>, double> memoize;

double LenCost(const vector<int>& arm_len, const vector<int>& leaf_len, int root_dy, int root_dx) {
  pair<ll, ll> hash(0, 0);
  hash.first = (root_dy + 1) * 3 + (root_dx + 1);
  REP(i,arm_len.size()) hash.first = hash.first * 32 + arm_len[i];
  REP(i,leaf_len.size()) hash.second |= 1LL << leaf_len[i];
  // REP(i,leaf_len.size()) hash.second = hash.second * 32 + leaf_len[i];
  if (memoize.count(hash)) return memoize[hash];

  auto reach = EvalBoard(arm_len, leaf_len, root_dy, root_dx);
  vector<int> board(N * N);
  REP(i,M) {
    board[From[i].first * N + From[i].second] += 1;
    board[To[i].first * N + To[i].second] += 1;
  }

  double cost = 0;
  // cerr << endl;
  REP(i,N) REP(j,N) {
    if (board[i * N + j] > 0)  {
      if (reach[i * N + j] == 0) return 1e9;
      cost += double(board[i * N + j]) / reach[i * N + j];
    }
  }
  return memoize[hash] = cost;
}

namespace beam_search {

BipartiteMatching mat(0);
vector<int> leaf_cat;
vector<P> len_count;
vector<pair<P, int>> neighbors;
// vector<float> eval_board;

int beam_width = 7;  // param
int cur_index = 0;

struct Command {
  int score = 0;
  int y = -1, x = -1;
  vector<int> rot;
  double cost = -1;
  int index = -1;
};

vector<Command> next_com;

constexpr int padding = 6;

struct Solver {
  int arm_size, leaf_size;
  vector<int> arm_len, leaf_len;
  State state;
  int rest;
  vector<bool> leaf_have;
  vector<int> board, next_board, cache;
  vector<float> step_board;
  vector<int> diff, dir;

  Solver(const vector<int>& arm_len, const vector<int>& leaf_len)
    : arm_size(arm_len.size()), leaf_size(leaf_len.size()), arm_len(arm_len), leaf_len(leaf_len),
      state(N / 2, N / 2), rest(M * 2),
      leaf_have(leaf_size, false), board(N * N), next_board(N * N),
      cache((N + padding * 2) * (N + padding * 2), 999),
      diff(arm_size), dir(arm_size) {
    REP(i,arm_size) state.par[i] = i - 1;
    REP(i,leaf_size) state.par[i + arm_size] = arm_size - 1;
    REP(i,arm_size) state.len[i] = arm_len[i];
    REP(i,leaf_size) state.len[i + arm_size] = leaf_len[i];

    REP(i,M) {
      board[From[i].first * N + From[i].second] += 1;
      board[To[i].first * N + To[i].second] -= 1;
    }
  }

  void Init(int root_dy, int root_dx) {
    neighbors = GetNeighbors(leaf_len);
    // std::cerr << "=====" << std::endl;
    // for (auto pair : neighbors) {
    //   std::cerr << pair.first << " " << pair.second << std::endl;
    // }
    // eval_board = EvalBoard(arm_len, leaf_len, root_dy, root_dx);
    step_board = StepBoard(leaf_size, neighbors, board);
    leaf_cat.resize(leaf_size);
    REP(i,leaf_size-1) leaf_cat[i+1] = leaf_cat[i] + (leaf_len[i] != leaf_len[i+1]);

    std::map<int, int> mp;
    REP(i,leaf_size) mp[leaf_len[i]]++;
    len_count = vector<P>(mp.begin(), mp.end());

    mat = BipartiteMatching(leaf_size + 4 * (leaf_cat.back() + 1));
  }

  int get_matching(int y, int x, int turn) {
    mat.Reset();
    matnodes.clear();
    REP(i,leaf_size) {
      int my_dir = dir.back() + state.dir[i + arm_size];

      // Check last move
      bool last_moved = false;
      if (state.com.empty()) {
        last_moved = true;
      } else {
        const string& com = state.com.back();
        if (com[1 + arm_size + i] != '.') last_moved = true;
        if (com[V + 1 + arm_size + i] != '.') last_moved = true;
      }
      matvec.clear();
      REP(rot,4) {
        if (turn == 0 && rot == 2 && last_moved) continue;
        int new_dir = (my_dir + rot) & 3;
        int ny = y + leaf_len[i] * dy[new_dir];
        int nx = x + leaf_len[i] * dx[new_dir];
        if (ny < 0 || ny >= N || nx < 0 || nx >= N) continue;
        if (board[ny * N + nx] == (leaf_have[i] ? -1 : 1)) {
          matvec.emplace_back(-step_board[ny * N + nx], leaf_size + leaf_cat[i] * 4 + new_dir);
          // mat.add_edge(i, leaf_size + leaf_cat[i] * 4 + new_dir);
        }
      }
      if (leaf_have[i] == 1) matnodes.push_back(i);
      else matnodes.push_front(i);
      std::sort(ALL(matvec));
      for (auto [_, index]: matvec) mat.add_edge(i, index);
    }
    return mat.maximum_matching(matnodes);
  }

  void MayUpdate(int score, int y_root, int x_root, int y, int x) {
    if (int(next_com.size()) < beam_width || score >= next_com.back().score) {
      double cost = 0;
      if (score != 0) {
        std::copy(ALL(board), next_board.begin());
        REP(i,leaf_size) {
          if (mat.match[i] == -1) continue;
          int my_dir = dir.back() + state.dir[i + arm_size];
          int new_dir = (my_dir + (mat.match[i] - leaf_size - my_dir)) & 3;
          int ny = y + leaf_len[i] * dy[new_dir];
          int nx = x + leaf_len[i] * dx[new_dir];
          assert(board[ny * N + nx] == (leaf_have[i] ? -1 : 1));
          next_board[ny * N + nx] = 0;
        }
        cost = GetCost1(leaf_size, neighbors, next_board);
        // cost = GetCost3(next_board);
      }
      // double cost = GetCost(leaf_size, neighbors, next_board);
      // REP(i,leaf_size) {
      //   if (mat.match[i] == -1) continue;
      //   int my_dir = dir.back() + state.dir[i + arm_size];
      //   int new_dir = (my_dir + (mat.match[i] - leaf_size - my_dir)) & 3;
      //   int ny = y + leaf_len[i] * dy[new_dir];
      //   int nx = x + leaf_len[i] * dx[new_dir];
      //   assert(nx >= 0 && nx < N && ny >= 0 && ny < N);
      //   cost += eval_board[ny * N + nx];
      // }
      if (int(next_com.size()) >= beam_width && score == next_com.back().score && cost >= next_com.back().cost) return;

      Command com{score, y_root, x_root, diff, cost, cur_index};
      REP(i,leaf_size) {
        int my_dir = dir.back() + state.dir[i + arm_size];
        if (mat.match[i] == -1) com.rot.push_back(-1);
        else com.rot.push_back((mat.match[i] - leaf_size - my_dir) & 3);
      }
      assert(com.index != -1);
      // next_com.push_back(com);
      // cerr << n_mat << " " << diff << " " << dir << " " << P(y, x) << " " << turn << endl;

      // std::stable_sort(ALL(next_com), [](const Command& a, const Command& b) {
      //   if (a.score != b.score) return a.score > b.score;
      //   return a.cost < b.cost;
      // });
      auto it = std::lower_bound(ALL(next_com), com, [](const Command& a, const Command& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.cost < b.cost;
      });
      next_com.insert(it, com);
      if (int(next_com.size()) > beam_width) next_com.resize(beam_width);
    }
  }

  int max_count(int y, int x) const {
    int res = 0;
    for (auto [len, cnt] : len_count) {
      int c = 0;
      REP(dir,4) {
        int ny = y + dy[dir] * len;
        int nx = x + dx[dir] * len;
        if (ny < 0 || ny >= N || nx < 0 || nx >= N) continue;
        if (board[ny * N + nx] != 0) c++;
      }
      res += std::min(c, cnt);
    }
    return res;
  }

  void SearchNode(int turn, int bit, int root_dy, int root_dx) {
    int y_root = N / 2 + 4 + root_dy - prio_y[bit >> (2 * arm_size)];
    int x_root = N / 2 + 4 + root_dx - prio_x[bit >> (2 * arm_size)];
    int y = y_root, x = x_root;
    if (turn == 0 && !state.com.empty()) {
      // assert(std::find(ALL(diff), 2) == diff.end());
      if (abs(state.y0 - y) + abs(state.x0 - x) > 1) return;
    }
    if (!timer.within()) EarlyExit();
    REP(i,arm_size) diff[i] = (bit >> (2 * (arm_size - i - 1))) & 3;  // rot_diff
    REP(i,arm_size) dir[i] = (diff[i] + state.dir[i]) & 3;
    REP(i,arm_size-1) dir[i+1] = (dir[i] + dir[i+1]) & 3;
    REP(i,arm_size) {
      y += dy[dir[i]] * arm_len[i];
      x += dx[dir[i]] * arm_len[i];
    }
    if (y < -padding || y >= N + padding || x < -padding || x >= N + padding) return;
    int& cache_ = cache[(N + padding * 2) * (y + padding) + (x + padding)];
    if (int(next_com.size()) >= beam_width && next_com.back().score > cache_ - rest) return;
    int new_cache = max_count(y, x);
    assert(cache_ >= new_cache);
    cache_ = new_cache;
    if (int(next_com.size()) >= beam_width && next_com.back().score > cache_ - rest) return;

    // cerr << diff << " " << P(y, x) << " " << turn << endl;

    int n_mat = this->get_matching(y, x, turn);
    if (n_mat == 0) return;
    int score = n_mat - rest;
    if (turn == 1) {
      int mvs = abs(state.y0 - y_root) + abs(state.x0 - x_root);
      if (mvs >= 2) score -= mvs * 10;
    }
    this->MayUpdate(score, y_root, x_root, y, x);
  }

  void Step(const Command& com) {
    state.move_to(com.y, com.x);
    REP(i,arm_size) state.rot_diff(i, com.rot[i]);
    REP(i,leaf_size) {
      if (com.rot[i + arm_size] == -1) continue;
      state.rot_diff(i + arm_size, com.rot[i + arm_size]);
      state.pick(i + arm_size);
      assert(board[state.get_coordinate(i + arm_size)] == (leaf_have[i] ? -1 : 1));
      board[state.get_coordinate(i + arm_size)] = 0;
      --rest;
      leaf_have[i] = !leaf_have[i];
    }
    step_board = StepBoard(leaf_size, neighbors, board);
  }
};

State Solve(const vector<int>& arm_len, const vector<int>& leaf_len, int root_dy, int root_dx) {
  if (!timer.within()) EarlyExit();
  int arm_size = arm_len.size();

  vector<Solver> beam, next_beam;
  {
    Solver solver(arm_len, leaf_len);
    solver.Init(root_dy, root_dx);
    beam.push_back(solver);
  }

  vector<int> bits;
  int box_size = (arm_size <= 2 ? 100 : (arm_size <= 3 ? 64 : 36));
  // LOG(box_size);
  // LOG(beam[0].state.len);
  REP(bit, (1 << (2 * arm_size)) * box_size) {
    bool is_ok = true;
    REP(i,arm_size) {
      int d = (bit >> (2 * (arm_size - i - 1))) & 3;
      if (d == 2) { is_ok = false; break; }
    }
    if (is_ok) bits.push_back(bit);
  }

  // LOG(LenCost(arm_len, leaf_len));

  for (int loop = 0;; ++loop) {
    if (loop > 0 && beam[0].state.cost() >= best_state.cost() - 1) return State();
    // // beam_width = std::max(2, std::min(1000, 30000 / beam[0].rest / beam[0].rest));
    // // if (beam[0].rest < 15) beam_width = 200;
    // if (beam[0].rest < 30) beam_width = 100;
    // // else if (beam[0].rest < 100) beam_width = 35;
    // else if (beam[0].rest < 100) beam_width = 35;
    // else beam_width = 3;
    if (best_state.cost() >= 99999999) beam_width = 2;
    else if (beam[0].rest < 200) beam_width = std::min(120, 2000 / beam[0].rest);
    else beam_width = 3;

    // LOG(loop);
    next_com.clear();
    // turn = 0
    for (cur_index = 0; cur_index < int(beam.size()); ++cur_index) {
      if (!next_com.empty() && next_com[0].score == 0) break;
      Solver& solver = beam[cur_index];
      // LOG(GetCost(leaf_size, neighbors, board));
      // cerr << "===== #" << loop << " ====" << endl;

      for (int bit: bits) {
        solver.SearchNode(0, bit, root_dy, root_dx);
      }
    }

    // turn = 1
    if (next_com.empty()) {
      for (cur_index = 0; cur_index < int(beam.size()); ++cur_index) {
        Solver& solver = beam[cur_index];
        REP(bit, (1 << (2 * arm_size)) * box_size) {
          solver.SearchNode(1, bit, root_dy, root_dx);
        }
      }
    }
    if (next_com.empty()) break;
    for (const Command& com: next_com) {
      assert(com.index != -1);
      assert(com.index < int(beam.size()));
      Solver next_solver = beam[com.index];
      next_solver.Step(com);
      // assert (next_solver.rest == -com.score);
      next_beam.push_back(next_solver);
    }
    std::swap(beam, next_beam);
    next_beam.clear();
    if (beam[0].rest == 0) {
      // LOG(beam[0].state.com.size());
      return beam[0].state;
    }
  }
  // REP(i,N) {
  //   REP(j,N) cerr << board[i * N + j] << " ";
  //   cerr << endl;
  // }

  assert(beam.empty() ||  beam[0].rest > 0);
  return State();
}

}  // namespace beam_search

using beam_search::Solve;

int main() {
  setup();
  // if (V >= 7) return 0;
  // V = 5 +5,944,688,473 / 7 cases (85%)
  // V = 6 +2,791,310,541 / 3 cases (93%)

  auto Update = [&](State state) {
    ++loop_count;
    if (best_state.cost() <= state.cost()) return false;
    best_state = std::move(state);
    return true;
  };
  auto next_cnt = [](vector<int>& cnt, int n_childs) -> bool{
    int limit = 2;
    // if (n_childs <= 8) limit = 3;
    // if (n_childs <= 8) limit = 2;
    int index = -1;
    bool ok = false;
    for (int i = int(cnt.size()) - 1; i >= 0; --i) {
      if (cnt[i] >= 1 && ok) { index = i; break; }
      if (cnt[i] < limit) ok = true;
    }
    if (index == -1) return false;
    --cnt[index];
    int rest = n_childs;
    REP(i,index+1) rest -= cnt[i];
    for (int i = index + 1; i < int(cnt.size()); ++i) {
      cnt[i] = std::min(rest, limit);
      rest -= cnt[i];
    }
    return rest == 0;
  };
  // if (false) {
  //   if (V == 5) {
  //     vector<v5::Move> moves;
  //     REP(i,M) moves.emplace_back(P(N / 2, N / 2), From[i], To[i]);
  //     Update(v5::SolveLine(4, N / 2 + 0, moves));
  //     Update(v5::SolveLine(4, N / 2 + 1, moves));
  //     Update(v5::SolveLine(4, N / 2 + 2, moves));
  //     Update(v5::SolveMinMove(4));
  //   }
  //   // Update(SolveDual(4, 1));
  //   // if (V >= 6) Update(SolveDual(5, 1));
  //   // if (V >= 9) Update(SolveDual(4));
  //   // if (V >= 11) Update(SolveDual(5));

  //   if (V == 5 && false) {
  //     Update(Solve({N - 13, 8, 4}, {2}));
  //     Update(Solve({N - 13, 8, 4}, {1}));
  //     Update(Solve({N / 2, 8, 4}, {2}));
  //     Update(Solve({N / 2, 8, 4}, {1}));
  //     Update(Solve({N / 2 - 1, 8, 4}, {2}));
  //     Update(Solve({N / 2 - 1, 8, 4}, {1}));
  //     Update(Solve({N / 2, N / 4, 3}, {2}));
  //     Update(Solve({N / 2, N / 4, 3}, {1}));
  //     Update(Solve({N / 2, N / 4, N / 8}, {2}));
  //     Update(Solve({N / 2, N / 4, N / 8}, {1}));
  //     Update(Solve({std::min(16, N - 1), 8, 4}, {2}));
  //     Update(Solve({std::min(16, N - 1), 8}, {3, 2}));
  //     Update(Solve({N / 2, 8}, {1, 1}));
  //   }
  //   if (V == 6 && false) {
  //     Update(Solve({std::min(16, N - 1), 8, 4}, {1, 2}));
  //     Update(Solve({std::min(16, N - 1), 8, 4}, {1, 3}));
  //     Update(Solve({std::min(16, N - 1), 8, 4}, {2, 3}));
  //     Update(Solve({N / 2, N / 4}, {1, 1, 1}));
  //     Update(Solve({N / 2, N / 4}, {1, 1, 2}));
  //     Update(Solve({N / 2, N / 4}, {1, 1, 3}));
  //     Update(Solve({N / 2, N / 4}, {1, 2, 3}));
  //     Update(Solve({N / 2, N / 4}, {2, 2, 3}));
  //     Update(Solve({N / 2, N / 4, N / 8}, {1, 2}));
  //     Update(Solve({N / 2, N / 4, N / 8}, {1, 3}));
  //     Update(Solve({N / 2, N / 4, N / 8}, {2, 3}));
  //     Update(Solve({N - 13, 8, 4}, {1, 2}));
  //     Update(Solve({N - 13, 8, 4}, {1, 3}));
  //     Update(Solve({N - 13, 8, 4}, {2, 3}));
  //     Update(Solve({8, 4, 2}, {1, 1}));
  //     Update(Solve({8, 4, 2}, {1, 2}));
  //     Update(Solve({8, 4, 2}, {1, 3}));
  //     Update(Solve({8, 4, 2}, {2, 3}));
  //   }
  // }

  // Update(Solve({2, 4, 7}, {1, 1, 1, 2, 3, 3, 4, 4}));

  vector<pair<double, tuple<vector<int>, vector<int>, P>>> vec;
  double tl1 = timer.get_cycle() + 0.2 * Timer::CYCLES_PER_SEC;

  if (true) {
    vector<vector<int>> arms_list;
    if (V < 8) {  // param
      arms_list = vector<vector<int>>{
        {N - 15, 8},
        {N - 15, 7},
        {N - 15, 9},
        {N - 16, 8},
        {N - 16, 7},
        {N - 16, 9},
        {N - 17, 8},
        {N - 17, 7},
        {N - 17, 9},
      };
    } else if (V < 12) {  // param
      arms_list = vector<vector<int>>{
        // {N - 15, 8, 4},
        // {N - 15, 7, 4},
        {N - 19, 8, 3},
        {N - 19, 7, 3},
      };
    } else {
      arms_list = vector<vector<int>>{
        {N - 15, 8, 4, 2},
        {N - 15, 7, 4, 2},
        // {N - 16, 8, 4, 2},
        // {N - 16, 7, 4, 2},
      };
    }
    auto normalize_arms = [](vector<int>& arms) {
      std::sort(arms.rbegin(), arms.rend());
      if (arms.back() <= 0) arms.pop_back();
      std::reverse(ALL(arms));   // Revisit
      REP(i,arms.size()-1) {
        if (arms[i] == arms[i+1]) ++arms[i];
      }
    };
    for (vector<int>& arms: arms_list) {
      normalize_arms(arms);
    }

    if (V < 8) {
      // V = 9
      if (N == 15) arms_list = {{5, 7}}; // 71.17
      else if (N == 16) arms_list = {{5, 8}};  //
      else if (N == 17) arms_list = {{5, 8}};  //
      else if (N == 18) arms_list = {{5, 9}};  //
      else if (N == 19) arms_list = {{5, 9}};  //
      else if (N == 20) arms_list = {{6, 10}}; //
      else if (N == 21) arms_list = {{6, 10}}; //
      else if (N == 22) arms_list = {{6, 11}}; //
      else if (N == 23) arms_list = {{6, 11}}; //
      else if (N == 24) arms_list = {{6, 12}}; //
      else if (N == 25) arms_list = {{6, 12}}; //
      else if (N == 26) arms_list = {{7, 12}}; //
      else if (N == 27) arms_list = {{7, 13}}; //
      else if (N == 28) arms_list = {{7, 13}}; //
      else if (N == 29) arms_list = {{7, 14}}; //
      else if (N == 30) arms_list = {{7, 14}}; // 67.51
    } else if (V >= 12) {
      if (N == 15) arms_list = {{3, 7, 2, 4}};  // 72.31
      else if (N == 16) arms_list = {{3, 7, 2, 4}};  // 74.50
      else if (N == 17) arms_list = {{3, 7, 2, 4}};  // 75.08
      else if (N == 18) arms_list = {{3, 7, 2, 4}};  // 77.27
      else if (N == 19) arms_list = {{3, 7, 2, 4}};  // 77.61
      else if (N == 20) arms_list = {{3, 7, 2, 4}};  // 78.96
      else if (N == 21) arms_list = {{3, 7, 2, 4}};  // 79.50
      else if (N == 22) arms_list = {{3, 7, 2, 4}};  // 81.18
      else if (N == 23) arms_list = {{3, 8, 2, 4}};  // 80.91
      else if (N == 24) arms_list = {{5, 9, 2, 4}};  // 79.96
      else if (N == 25) arms_list = {{5, 10, 2, 4}}; // 80.19
      else if (N == 26) arms_list = {{6, 11, 2, 4}}; //
      else if (N == 27) arms_list = {{7, 12, 2, 4}}; // 81.97
      else if (N == 28) arms_list = {{7, 13, 2, 4}}; //
      else if (N == 29) arms_list = {{7, 14, 2, 4}}; // 80.85
      else if (N == 30) arms_list = {{2, 4, 8, 15}}; // 82.54
    } else if (V >= 8) {
      // V = 9
      if (N == 15) arms_list = {{3, 7, 2}};  // 81.37
      if (N == 16) arms_list = {{3, 7, 2}};  //
      if (N == 17) arms_list = {{3, 7, 2}};  //
      if (N == 18) arms_list = {{3, 7, 2}};  //
      if (N == 19) arms_list = {{3, 7, 2}};  //
      if (N == 20) arms_list = {{4, 7, 2}};  // 84.69
      if (N == 20) arms_list = {{4, 7, 2}};  // 84.69
      if (N == 21) arms_list = {{4, 8, 2}};  //
      if (N == 22) arms_list = {{4, 8, 2}};  // 85.75
      if (N == 23) arms_list = {{3, 9, 5}}; // TODO
      if (N == 24) arms_list = {{3, 9, 5}}; // TODO
      if (N == 25) arms_list = {{3, 10, 5}}; // 85.20
      if (N == 26) arms_list = {{3, 10, 5}}; //
      if (N == 27) arms_list = {{3, 11, 6}}; // 86.57
      if (N == 28) arms_list = {{3, 11, 6}}; //
      if (N == 29) arms_list = {{4, 11, 6}}; //
      if (N == 30) arms_list = {{4, 11, 6}}; // 85.91
    }

    // std::reverse(ALL(arms8));   // Revisit
    // std::reverse(ALL(arms7));

    int n_childs = V - arms_list.front().size() - 1;

    bool first_run = true;
    int max_len;
    for (max_len = 1; timer.get_cycle() < tl1 && max_len < 12; ++max_len) {
      if (n_childs <= 1) break;
      int limit = 2;
      // if (n_childs <= 8) limit = 3;
      // if (n_childs <= 8) limit = 2;

      vector<int> cnt(max_len);
      int rest = n_childs;
      REP(i,max_len) {
        cnt[i] = std::min(rest, limit);
        rest -= cnt[i];
      }
      if (rest > 0) continue;

      do {
        // if (!first_run && std::count(ALL(cnt), 2) >= 3) continue;
        if (cnt.back() == 0) continue;
        vector<int> leaf_len;
        leaf_len.reserve(n_childs);
        REP(i,max_len) REP(j,cnt[i]) leaf_len.push_back(i + 1);
        // LOG(leaf_len);
        for (const vector<int>& arms : arms_list) {
          for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
              if (first_run && dy == 0 && dx == 0) {
                vec.emplace_back(-1, std::make_tuple(arms, leaf_len, P(dy, dx)));
                first_run = false;
              } else {
                double cost = LenCost(arms, leaf_len, dy, dx);
                vec.emplace_back(cost, std::make_tuple(arms, leaf_len, P(dy, dx)));
              }
            }
          }
        }
        if (vec.size() > 15000) break;
      } while (next_cnt(cnt, n_childs));
      if (vec.size() > 15000) break;
    }
    LOG(max_len);
  }

  std::sort(ALL(vec));
  LOG(vec.size());
  for (int i = 0; i < int(vec.size()) && timer.within(); ++i) {
    // std::cerr << vec[i].first << " " << vec[i].second.second << std::endl;
    auto [arms, leafs, root_d] = vec[i].second;
    bool updated = Update(Solve(arms, leafs, root_d.first, root_d.second));
    if (updated) choice = i;
    if (i == 0) {
      double first_elapsed = (timer.get_cycle() - timer.start) / double(Timer::CYCLES_PER_SEC);
      LOG(first_elapsed);
    }
  }
  EarlyExit();
  return 0;
}
