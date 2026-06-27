

namespace atcoder {}

#ifdef LOCAL
#define dbg(x) cerr << __LINE__ << " : " << #x << " = " << (x) << endl;
#else
#define NDEBUG
#define dbg(x) true;
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#endif

#ifdef GTEST
#include <gtest/gtest.h>
#endif

#include <math.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#ifdef PERF
#include <gperftools/profiler.h>
#endif

using namespace std;
using namespace atcoder;
#define fast_io                     \
  ios_base::sync_with_stdio(false); \
  cin.tie(0);                       \
  cout.tie(0);
#define ll long long int
#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define reps(i, n) for (int i = 1; i <= (int)(n); i++)
#define REP(i, n) for (int i = n - 1; i >= 0; i--)
#define REPS(i, n) for (int i = n; i > 0; i--)
#define MOD (long long int)(1e9 + 7)
#define INF (int)(1e9)
#define LINF (long long int)(1e18)
#define chmax(a, b) a = (((a) < (b)) ? (b) : (a))
#define chmin(a, b) a = (((a) > (b)) ? (b) : (a))
#define all(v) v.begin(), v.end()
typedef pair<int, int> Pii;
typedef pair<ll, ll> Pll;
constexpr double PI = acos(-1);

#ifdef NDEBUG
#define CHECK(v1, op, v2)
#else
#define CHECK(v1, op, v2)                            \
  if (!((v1)op(v2))) {                               \
    cerr << "ERROR:" << (v1) << " " << (v2) << endl; \
    assert((v1)op(v2));                              \
  }
#endif

long double nCr(const int n, const int r) {
  long double ret = 1;
  rep(t, r) {
    ret *= (n - t);
    ret /= (r - t);
  }
  return ret;
}

template <typename T>
string to_string(const vector<T>& vec) {
  string ret = "";
  rep(i, vec.size()) {
    ret += vec[i].to_string();
    if (i + 1 != vec.size()) {
      ret += ",";
    }
  }
  return ret;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vec) {
  os << to_string(vec);
  return os;
}

uint32_t xorshift() {
  static uint32_t x = 12345789;
  static uint32_t y = 362436069;
  static uint32_t z = 521288629;
  static uint32_t w = 88675123;
  uint32_t t;
  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  w ^= t ^ (t >> 8) ^ (w >> 19);

  return w;
}

int rand(const uint32_t l, const uint32_t r) {
  return xorshift() % (r - l) + l;
}

uint32_t rand_other_than(const uint32_t l, const uint32_t r,
                         const uint32_t other) {
  const uint32_t num = rand(l, r - 1);
  return num + (num >= other);
}

template <typename T>
const T& rand_vec(const vector<T>& vec) {
  assert(vec.size() > 0);
  return vec[rand(0, vec.size())];
}

template <typename T>
void shuffle(vector<T>& vec) {
  rep(l, (int)vec.size() - 1) {
    const int idx = rand(l, vec.size());
    swap(vec[idx], vec[l]);
  }
}


class Timer {
  chrono::system_clock::time_point _start, _end;
  ll _sum = 0, _count = 0;

 public:
  void start() { _start = chrono::system_clock::now(); }

  void stop() { _end = chrono::system_clock::now(); }

  void add() {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    _sum += static_cast<double>(
        chrono::duration_cast<chrono::nanoseconds>(now - _start).count());
    _count++;
  }

  ll sum() const { return _sum / 1000; }

  int count() const { return _count; }

  string average() const {
    if (_count == 0) {
      return "NaN";
    }
    return to_string(_sum / 1000 / _count);
  }

  void reset() {
    _start = chrono::system_clock::now();
    _sum = 0;
    _count = 0;
  }

  inline int ms() const {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    return static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(now - _start).count() /
        1000);
  }

  inline int ns() const {
    const chrono::system_clock::time_point now = chrono::system_clock::now();
    return static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(now - _start).count());
  }
};

#ifdef LOCAL
struct Timers : unordered_map<string, Timer> {
  friend ostream& operator<<(ostream& os, const Timers& timers) {
    for (const auto& pa : timers) {
      os << pa.first << " time: " << pa.second.sum() / 1000
         << " count: " << pa.second.count() << endl;
    }
    return os;
  }
};
#else
struct Timers {
  struct Dummy {
    void start() const {}
    void add() const {}
  };
  Dummy dummy;
  const Dummy& operator[](const std::string& str) { return dummy; }
  friend ostream& operator<<(ostream& os, const Timers& timers) { return os; }
};
#endif

Timers global_timers;






enum Dir {
  // y-1, y+1, x-1, x+1
  kU,
  kD,
  kL,
  kR
};

Dir TurnLeft(Dir dir) {
  constexpr Dir rights[4] = {kL, kR, kD, kU};
  return rights[dir];
}

Dir TurnRight(Dir dir) {
  constexpr Dir rights[4] = {kR, kL, kU, kD};
  return rights[dir];
}

struct Pos {
  int idx_ = -1;
  Pos() {}
  explicit Pos(const int _idx) : idx_(_idx) {}
  Pos(int _x, int _y) : idx_(_y * N + _x) { assert(N > 0); }
  int X() const { return pos_2_x[*this]; }
  int Y() const { return pos_2_y[*this]; }
  int Idx() const { return idx_; }
  operator int() const { return Idx(); }
  operator size_t() const { return Idx(); }
  const vector<Pos>& Adj() const { return adj_poses[*this]; }
  const vector<Dir>& AdjDirs() const { return adj_dirs[*this]; }
  int Manhattan(const Pos& other) const {
    return abs(X() - other.X()) + abs(Y() - other.Y());
  }
  int Dist(const Pos& other) const { return dists[*this][other]; }
  int Euclid2(const Pos& other) const {
    const int dx = X() - other.X();
    const int dy = Y() - other.Y();
    return dx * dx + dy * dy;
  }
  bool Move(const Dir dir) {
    if (move_to[dir][*this].IsDummy()) {
      return false;
    } else {
      *this = move_to[dir][*this];
      return true;
    }
  }
  int Depth() const { return depths[*this]; }
  bool operator<(const Pos& other) const { return this->Idx() < other.Idx(); }
  bool operator==(const Pos& other) const { return this->Idx() == other.Idx(); }
  bool operator!=(const Pos& other) const { return this->Idx() != other.Idx(); }
  friend ostream& operator<<(ostream& os, const Pos& pos) {
    os << pos.X() << " " << pos.Y();
    return os;
  }
  bool IsDummy() const { return this->idx_ < 0; }
  static Pos Dummy() {
    Pos p;
    p.idx_ = -1;
    return p;
  }

  static Pos TryCreate(const int x, const int y, const int z) {
    if (y < 0 || y >= N || x < 0 || x >= N) {
      return Pos::Dummy();
    } else {
      return Pos(x, y);
    }
  }

  static void StaticInit(const int n, const int i0,
                         const vector<vector<bool>>& blockedH,
                         const vector<vector<bool>>& blockedW) {
    N = n;
    N2 = N * N;
    N4 = N2 * N2;
    adj_poses = vector<vector<Pos>>(N2);
    adj_dirs = vector<vector<Dir>>(N2);
    move_to = vector<vector<Pos>>(4, vector<Pos>(N2, Pos::Dummy()));
    dists = vector<vector<int>>(N * N, vector<int>(N * N, INF));
    depth2poses_lower.resize(400);
    depth2poses_higher.resize(400);
    max_depth = 0;
    pos_2_y.resize(N2);
    pos_2_x.resize(N2);

    rep(y, N) {
      rep(x, N) {
        const Pos p{x, y};
        all_poses.push_back(p);
        pos_2_y[p] = y;
        pos_2_x[p] = x;
        for (int dy = -1; dy <= 1; ++dy) {
          for (int dx = -1; dx <= 1; ++dx) {
            if (abs(dy) + abs(dx) != 1) continue;
            const int adj_y = y + dy;
            const int adj_x = x + dx;
            if (adj_y < 0 || adj_y >= N) continue;
            if (adj_x < 0 || adj_x >= N) continue;
            Dir dir;
            if (dy == -1) {
              if (blockedH[y + dy][x]) continue;
              dir = kU;
            } else if (dy == 1) {
              if (blockedH[y][x]) continue;
              dir = kD;
            } else if (dx == -1) {
              if (blockedW[y][x + dx]) continue;
              dir = kL;
            } else if (dx == 1) {
              if (blockedW[y][x]) continue;
              dir = kR;
            } else {
              assert(false);
            }

            adj_poses[p].emplace_back(adj_x, adj_y);
            move_to[dir][p] = {adj_x, adj_y};
            adj_dirs[p].emplace_back(dir);
          }
        }
      }
    }
    all_poses.shrink_to_fit();

    S = Pos(0, i0);

    rep(idx, N * N) {
      Pos start_pos(idx);
      auto& array = dists[start_pos];
      array[start_pos] = 0;
      queue<Pos> q;
      q.push(start_pos);
      int dist = 1;
      while (q.size() > 0) {
        queue<Pos> next_q;
        while (q.size() > 0) {
          auto p = q.front();
          q.pop();
          for (const auto adj : p.Adj()) {
            if (array[adj] != INF) continue;
            array[adj] = dist;
            next_q.push(adj);
          }
        }
        q = std::move(next_q);
        dist++;
      }
    }

    depths = dists[S];

    rep(idx, N * N) {
      Pos p(idx);
      chmax(max_depth, p.Depth());
      for (int d = p.Depth(); d < 400; d++) {
        depth2poses_lower[d].push_back(p);
      }
      for (int d = p.Depth(); d >= 0; d--) {
        depth2poses_higher[d].push_back(p);
      }
    }
  }
  static int N, N2, N4;
  static vector<vector<Pos>> adj_poses;
  static vector<vector<Dir>> adj_dirs;
  static vector<vector<Pos>> move_to;
  static vector<vector<int>> dists;
  static vector<int> pos_2_x, pos_2_y;
  static vector<int> depths;
  static vector<Pos> all_poses;
  static int max_depth;
  static vector<vector<Pos>> depth2poses_lower;
  static vector<vector<Pos>> depth2poses_higher;
  static Pos S;
};

vector<vector<Pos>> Pos::adj_poses;
vector<vector<Dir>> Pos::adj_dirs;
vector<vector<Pos>> Pos::move_to;
vector<vector<int>> Pos::dists;
vector<int> Pos::pos_2_x, Pos::pos_2_y;
vector<int> Pos::depths;
vector<Pos> Pos::all_poses;
vector<vector<Pos>> Pos::depth2poses_lower;
vector<vector<Pos>> Pos::depth2poses_higher;
int Pos::max_depth;
Pos Pos::S;
int Pos::N, Pos::N2, Pos::N4;


/* start */

int T, H, W, K;
Pos S;
vector<vector<bool>> blockedH, blockedW;
Timer global_timer;
map<string, Timer> debug_timers;

void PrintTimers() {
  for (const auto& [name, timer] : debug_timers) {
    cerr << name << ": " << timer.sum() / 1000 << "[ms]" << endl;
  }
}

struct Crop {
  // 半開区間
  int k, s, d;

  Crop(int k_, int s_, int d_) : k(k_), s(s_), d(d_) {}
  int Days() const { return d - s; }

  static vector<Crop> all_crops;
  static vector<vector<Crop>> s2crops;
  static vector<vector<Crop>> d2crops;
  static vector<vector<int>> sd2counts;
  static vector<int> ds2counts;
};

struct Solution {
  // クラスタに割り当てるCropID
  vector<int> crop_ids;
  // 設置タイミング、回収の最終タイミング
  int l = 0, r = 0;
};

int ScoreCluster(const vector<Solution>& sol_vec) {
  int sum = 0;
  for (const auto& sol : sol_vec) {
    for (const auto id : sol.crop_ids) {
      sum += Crop::all_crops[id].Days();
    }
  }
  return sum;
}

int ScoreAnswer(const vector<vector<Solution>>& sol_vec_vec) {
  int sum = 0;
  for (const auto& sol_vec : sol_vec_vec) {
    sum += ScoreCluster(sol_vec);
  }
  return sum;
}

vector<Crop> Crop::all_crops;
vector<vector<Crop>> Crop::s2crops;
vector<vector<Crop>> Crop::d2crops;
vector<vector<int>> Crop::sd2counts;
vector<int> Crop::ds2counts;

struct Output {
  static void StaticInit(istream& is) {
    int i0;
    is >> T >> H >> W >> i0;
    global_timer.start();

    rep(y, H - 1) {
      blockedH.emplace_back();
      string s;
      is >> s;
      rep(x, W) {
        int h = s[x] - '0';
        blockedH.back().emplace_back(h);
      }
    }

    rep(y, H) {
      blockedW.emplace_back();
      string s;
      is >> s;
      rep(x, W - 1) {
        int w = s[x] - '0';
        blockedW.back().emplace_back(w);
      }
    }

    is >> K;
    Crop::all_crops.clear();
    Crop::s2crops.assign(T + 1, vector<Crop>());
    Crop::d2crops.assign(T + 1, vector<Crop>());
    Crop::sd2counts.assign(T + 1, vector<int>(T + 1));
    Crop::ds2counts.assign((T + 1) * (T + 1), 0);
    rep(k, K) {
      int s, d;
      is >> s >> d;
      const Crop crop(k, s - 1, d);
      Crop::all_crops.push_back(crop);
      Crop::s2crops[crop.s].push_back(crop);
      Crop::d2crops[crop.d].push_back(crop);
      Crop::sd2counts[crop.s][crop.d]++;
      Crop::ds2counts[crop.d * 101 + crop.s]++;
    }
    Crop::all_crops.shrink_to_fit();
    rep(s, T + 1) {
      Crop::s2crops[s].shrink_to_fit();
      Crop::d2crops[s].shrink_to_fit();
    }

    Pos::StaticInit(H, i0, blockedH, blockedW);

    S = Pos(0, i0);
  }
  friend ostream& operator<<(ostream& os, const Output& output) { return os; }
};



/* start */
// a
// a
// a
// a

// vector<double> PARAMS = {29.98081933651087,
//                          5.903872289617962,
//                          0.22280241805677398,
//                          39.698915747944746,
//                          6.017031913346574,
//                          5.642046353266762,
//                          0.7924437160152313,
//                          40.0,
//                          2.1};
// a
vector<double> PARAMS = {19.296016669130548,
                         3.247298952161528,
                         0.24621230398489155,
                         -1,
                         -1,
                         -1.0,
                         0.426001110820503,
                         23.506584967691754,
                         10.626577081366797,
                         4.822491616017625,
                         8.369162899517848,
                         -1.0};


#ifndef ATCODER_DSU_HPP
#define ATCODER_DSU_HPP 1

#include <algorithm>
#include <cassert>
#include <vector>

namespace atcoder {

// Implement (union by size) + (path compression)
// Reference:
// Zvi Galil and Giuseppe F. Italiano,
// Data structures and algorithms for disjoint set union problems
struct dsu {
  public:
    dsu() : _n(0) {}
    dsu(int n) : _n(n), parent_or_size(n, -1) {}

    int merge(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        int x = leader(a), y = leader(b);
        if (x == y) return x;
        if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        return x;
    }

    bool same(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int leader(int a) {
        assert(0 <= a && a < _n);
        if (parent_or_size[a] < 0) return a;
        return parent_or_size[a] = leader(parent_or_size[a]);
    }

    int size(int a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++) {
            result[i].reserve(group_size[i]);
        }
        for (int i = 0; i < _n; i++) {
            result[leader_buf[i]].push_back(i);
        }
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&](const std::vector<int>& v) { return v.empty(); }),
            result.end());
        return result;
    }

  private:
    int _n;
    // root node: -1 * component size
    // otherwise: parent
    std::vector<int> parent_or_size;
};

}  // namespace atcoder

#endif  // ATCODER_DSU_HPP

#ifndef ATCODER_MINCOSTFLOW_HPP
#define ATCODER_MINCOSTFLOW_HPP 1

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <vector>

namespace atcoder {

template <class Cap, class Cost> struct mcf_graph {
  public:
    mcf_graph() {}
    mcf_graph(int n) : _n(n), g(n) {}

    int add_edge(int from, int to, Cap cap, Cost cost) {
        assert(0 <= from && from < _n);
        assert(0 <= to && to < _n);
        int m = int(pos.size());
        pos.push_back({from, int(g[from].size())});
        g[from].push_back(_edge{to, int(g[to].size()), cap, cost});
        g[to].push_back(_edge{from, int(g[from].size()) - 1, 0, -cost});
        return m;
    }

    struct edge {
        int from, to;
        Cap cap, flow;
        Cost cost;
    };

    edge get_edge(int i) {
        int m = int(pos.size());
        assert(0 <= i && i < m);
        auto _e = g[pos[i].first][pos[i].second];
        auto _re = g[_e.to][_e.rev];
        return edge{
            pos[i].first, _e.to, _e.cap + _re.cap, _re.cap, _e.cost,
        };
    }
    std::vector<edge> edges() {
        int m = int(pos.size());
        std::vector<edge> result(m);
        for (int i = 0; i < m; i++) {
            result[i] = get_edge(i);
        }
        return result;
    }

    std::pair<Cap, Cost> flow(int s, int t) {
        return flow(s, t, std::numeric_limits<Cap>::max());
    }
    std::pair<Cap, Cost> flow(int s, int t, Cap flow_limit) {
        return slope(s, t, flow_limit).back();
    }
    std::vector<std::pair<Cap, Cost>> slope(int s, int t) {
        return slope(s, t, std::numeric_limits<Cap>::max());
    }
    std::vector<std::pair<Cap, Cost>> slope(int s, int t, Cap flow_limit) {
        assert(0 <= s && s < _n);
        assert(0 <= t && t < _n);
        assert(s != t);
        // variants (C = maxcost):
        // -(n-1)C <= dual[s] <= dual[i] <= dual[t] = 0
        // reduced cost (= e.cost + dual[e.from] - dual[e.to]) >= 0 for all edge
        std::vector<Cost> dual(_n, 0), dist(_n);
        std::vector<int> pv(_n), pe(_n);
        std::vector<bool> vis(_n);
        auto dual_ref = [&]() {
            std::fill(dist.begin(), dist.end(),
                      std::numeric_limits<Cost>::max());
            std::fill(pv.begin(), pv.end(), -1);
            std::fill(pe.begin(), pe.end(), -1);
            std::fill(vis.begin(), vis.end(), false);
            struct Q {
                Cost key;
                int to;
                bool operator<(Q r) const { return key > r.key; }
            };
            std::priority_queue<Q> que;
            dist[s] = 0;
            que.push(Q{0, s});
            while (!que.empty()) {
                int v = que.top().to;
                que.pop();
                if (vis[v]) continue;
                vis[v] = true;
                if (v == t) break;
                // dist[v] = shortest(s, v) + dual[s] - dual[v]
                // dist[v] >= 0 (all reduced cost are positive)
                // dist[v] <= (n-1)C
                for (int i = 0; i < int(g[v].size()); i++) {
                    auto e = g[v][i];
                    if (vis[e.to] || !e.cap) continue;
                    // |-dual[e.to] + dual[v]| <= (n-1)C
                    // cost <= C - -(n-1)C + 0 = nC
                    Cost cost = e.cost - dual[e.to] + dual[v];
                    if (dist[e.to] - dist[v] > cost) {
                        dist[e.to] = dist[v] + cost;
                        pv[e.to] = v;
                        pe[e.to] = i;
                        que.push(Q{dist[e.to], e.to});
                    }
                }
            }
            if (!vis[t]) {
                return false;
            }

            for (int v = 0; v < _n; v++) {
                if (!vis[v]) continue;
                // dual[v] = dual[v] - dist[t] + dist[v]
                //         = dual[v] - (shortest(s, t) + dual[s] - dual[t]) + (shortest(s, v) + dual[s] - dual[v])
                //         = - shortest(s, t) + dual[t] + shortest(s, v)
                //         = shortest(s, v) - shortest(s, t) >= 0 - (n-1)C
                dual[v] -= dist[t] - dist[v];
            }
            return true;
        };
        Cap flow = 0;
        Cost cost = 0, prev_cost = -1;
        std::vector<std::pair<Cap, Cost>> result;
        result.push_back({flow, cost});
        while (flow < flow_limit) {
            if (!dual_ref()) break;
            Cap c = flow_limit - flow;
            for (int v = t; v != s; v = pv[v]) {
                c = std::min(c, g[pv[v]][pe[v]].cap);
            }
            for (int v = t; v != s; v = pv[v]) {
                auto& e = g[pv[v]][pe[v]];
                e.cap -= c;
                g[v][e.rev].cap += c;
            }
            Cost d = -dual[s];
            flow += c;
            cost += c * d;
            if (prev_cost == d) {
                result.pop_back();
            }
            result.push_back({flow, cost});
            prev_cost = cost;
        }
        return result;
    }

  private:
    int _n;

    struct _edge {
        int to, rev;
        Cap cap;
        Cost cost;
    };

    std::vector<std::pair<int, int>> pos;
    std::vector<std::vector<_edge>> g;
};

}  // namespace atcoder

#endif  // ATCODER_MINCOSTFLOW_HPP







// NBD = neighborhood
template <class Solution, class NBDGenerator>
struct SimulatedAnnealing {
 public:
  SimulatedAnnealing(double end_time, double start_temp, double end_temp)
      : end_time_(end_time * 1000),  // ns
        inv_time_(1.0 / end_time_),
        cur_time_(0.0),
        start_temp_(start_temp),
        end_temp_(end_temp),
        diff_temp_(end_temp_ / start_temp_),
        cur_temp_(start_temp) {
    assert(start_temp >= end_temp);
    timer_.start();
  }

  Solution run(const Solution& initial_sol, NBDGenerator& nbd_generator) {
    Solution sol(initial_sol);

    int acc = 0;
    int g = 0;
    for (;; ++g) {
      if ((g & 0xf) == 0) {
#ifdef SA_MAX_G
        if (g >= SA_MAX_G) {
          break;
        }
#endif
        UpdateTime();
        UpdateTemp();
        if (cur_time_ > end_time_) {
          break;
        }
      }

      const auto nbd = nbd_generator.Generate(&sol, g, cur_time_ * inv_time_);
      constexpr int mod = 1'000'000'000;
      const int r = rand(0, mod);
      if (static_cast<double>(r) / mod < exp(-nbd->Diff() / cur_temp_)) {
        acc++;
        nbd->Update(&sol);
      } else {
        nbd->Restore(&sol);
      }
    }
    cerr << "g,acc: " << g << " " << acc << endl;
    return sol;
  }

 private:
  double end_time_, inv_time_, cur_time_;
  double start_temp_, end_temp_, diff_temp_, cur_temp_;
  Timer timer_;

  void UpdateTime() { cur_time_ = timer_.ns(); }
  void UpdateTemp() {
    cur_temp_ = start_temp_ * pow(diff_temp_, cur_time_ * inv_time_);
    // cur_temp_ = max(0.0, start_temp_ - cur_time_ * diff_temp_ * inv_time_);
  }
};







struct Action {
  int k = -1, s = 0;
  Pos p;
  Action() {}
  Action(int k_, Pos p_, int s_) : k(k_), s(s_), p(p_) {}
};

struct Plan {
  vector<Action> actions;
  Plan() {}
  void Add(const Crop& crop, Pos p, int s) {
    Action action(crop.k, p, s);
    Add(action);
  }
  void Add(const Action& action) { actions.push_back(action); }

  int Score() const {
    int sum = 0;
    for (const auto& action : actions) {
      sum += Crop::all_crops[action.k].Days();
    }
    return sum * 25 * 50;
  }

  friend ostream& operator<<(ostream& os, const Plan& plan) {
    os << plan.actions.size() << "\n";
    rep(i, plan.actions.size()) {
      const auto& action = plan.actions[i];
      os << action.k + 1 << " " << action.p.Y() << " " << action.p.X() << " "
         << action.s + 1;
      //   cerr << action.k + 1 << " " << action.p.Depth() << endl;
      if (i + 1 != (int)plan.actions.size()) {
        os << "\n";
      }
    }
    return os;
  }
};

// #define DEBUG

// d, s, countを受け取って、d'==d,
// s'>=sであるものを大きい順に並べた場合のcount個の累積和を返す
vector<float> dscount2ruiseki;
// 各posのmodを良い感じにクラスタに割り当てる
const int kMod = PARAMS[8];
vector<int> pos2mods;

// d,sを受け取って、s' >= sを満たす実在する最小のs'を返す
vector<int> ds2next_s;
// d,sを受け取って、s' <= sを満たす実在する最大のs'を返す
vector<int> ds2prev_s;

// Pos * 2^8 + [0, 2^8)のbit列を受け取って削除可能かを返す
bitset<256 * 400> can_remove_bs;
vector<vector<Pos>> adjadjs_check_remove;

void InitRemoveBS() {
  can_remove_bs = 0;
  adjadjs_check_remove.resize(400);
  rep(idx, H * W) {
    const Pos p(idx);
    set<Pos> S;
    for (const auto adj : p.Adj()) {
      for (const auto adjadj : adj.Adj()) {
        if (adjadj == p) continue;
        if (S.count(adjadj) > 0) {
          adjadjs_check_remove[p].push_back(adjadj);
        } else {
          S.insert(adjadj);
        }
      }
    }
    assert(adjadjs_check_remove[p].size() <= 4);
    rep(bit, 256) {
      // adj0, adj1, ..., adjadj0, adjdadj1, ... を詰めずに並べる
      bitset<8> bs(bit);
      set<Pos> adjs, adjadjs;
      rep(j, p.Adj().size()) {
        if (bs[j]) {
          adjs.insert(p.Adj()[j]);
        }
      }
      rep(j, adjadjs_check_remove[p].size()) {
        const auto adjadj = adjadjs_check_remove[p][j];
        if (bs[j + 4]) {
          int count = 0;
          for (const auto adjadjadj : adjadj.Adj()) {
            count += adjs.count(adjadjadj);
          }
          if (count >= 2) {
            adjadjs.insert(adjadj);
          }
        }
      }

      can_remove_bs[(ulong)p * 256 + bs.to_ulong()] =
          ((int)adjadjs.size() >= (int)adjs.size() - 1);
    }
  }
}
int ExistNextS(const int d, const int s) { return ds2next_s[d * 101 + s]; }
int ExistPrevS(const int d, const int s) { return ds2prev_s[d * 101 + s]; }

void InitExistNextS() {
  ds2next_s.assign(101 * 101, 100);
  for (const auto crop : Crop::all_crops) {
    ds2next_s[crop.d * 101 + crop.s] = crop.s;
  }
  rep(d, 101) {
    REP(s, 100) { chmin(ds2next_s[d * 101 + s], ds2next_s[d * 101 + s + 1]); }
  }

  ds2prev_s.assign(101 * 101, -1);
  for (const auto crop : Crop::all_crops) {
    ds2prev_s[crop.d * 101 + crop.s] = crop.s;
  }
  rep(d, 101) {
    rep(s, 100) { chmax(ds2prev_s[d * 101 + s + 1], ds2prev_s[d * 101 + s]); }
  }
}

float DSC(const int d, const int s, const int c) {
  return dscount2ruiseki[d * 101 * 401 + s * 401 + c];
}

void SetDSC(const int d, const int s, const int c, const float val) {
  dscount2ruiseki[d * 101 * 401 + s * 401 + c] = val;
}

vector<int> D_for_random;

int RandD() { return rand(2, 100); }
Pos RandPos(const double progress) { return rand_vec(Pos::all_poses); }

using BS = bitset<101 * 400>;

struct State {
  // 各マス,各tについてそのタイミングをdとするか
  BS exist_Ds;
  // d,sを受け取って個数を返す
  vector<int> mp;
  float eval = 0;

  static void KMeans() {
    // pos2modsを作成する
    pos2mods.assign(400, -1);
    vector<Pos> centers;
    // 適当に初期化
    // TODO: 工夫
    rep(t, kMod) { centers.push_back(Pos(20 * t / kMod, 20 * t / kMod)); }

    const auto nearest_center = [](const vector<Pos>& centers, const Pos p) {
      // 最も近いcenterに属する
      int min_dist = INF;
      int best_ci = 0;
      rep(ci, centers.size()) {
        const auto center = centers[ci];
        const int dist = center.Dist(p);
        if (dist < min_dist) {
          min_dist = dist;
          best_ci = ci;
        }
      }
      return best_ci;
    };

    // 100iterやる
    rep(t, 100) {
      vector<vector<int>> next_center_preffers(kMod);
      rep(mod, kMod) {
        next_center_preffers[mod].resize(centers[mod].Adj().size());
      }
      rep(idx, H * W) {
        Pos p(idx);

        // deltaを確認
        const int best_ci = nearest_center(centers, p);
        // cerr << idx << " " << best_ci << endl;
        const auto best_center = centers[best_ci];

        rep(ai, best_center.Adj().size()) {
          const auto& adj = best_center.Adj()[ai];
          if (best_center.Dist(p) > adj.Dist(p)) {
            next_center_preffers[best_ci][ai]++;
          } else if (best_center.Dist(p) < adj.Dist(p)) {
            next_center_preffers[best_ci][ai]--;
          }
        }
      }

      rep(ci, kMod) {
        // TODO: ちゃんと実装する
        int best_ai = -1;
        int max_preffer = 0;
        rep(ai, next_center_preffers[ci].size()) {
          const int val = next_center_preffers[ci][ai];
          if (val > max_preffer) {
            max_preffer = val;
            best_ai = ai;
          }
        }
        if (best_ai != -1) {
          centers[ci] = centers[ci].Adj()[best_ai];
        }
      }
    }

    // 結果に基づいてクラスタリング
    rep(idx, H * W) {
      Pos p(idx);
      const int ci = nearest_center(centers, p);
      pos2mods[p] = ci;
    }
  }

  static void StaticInit() {
    InitRemoveBS();
    KMeans();

    // ExistNextSを初期化
    InitExistNextS();

    for (const auto& crop : Crop::all_crops) {
      if (crop.d != 100) {
        D_for_random.push_back(crop.d);
      }
    }
    dscount2ruiseki.assign(101 * 101 * 401, 0);
    rep(d, 101) {
      auto crops = Crop::d2crops[d];
      sort(all(crops), [&](const auto& l, const auto& r) { return l.s < r.s; });
      int start_ci = 0;
      rep(s, d) {
        while (start_ci < (int)crops.size() && crops[start_ci].s < s)
          start_ci++;
        if (start_ci == (int)crops.size()) break;
        for (int t = 0; t < 400; t++) {
          SetDSC(d, s, t + 1, DSC(d, s, t));
          if (t + start_ci < (int)crops.size()) {
            const int days = crops[t + start_ci].Days();
            // TODO: 調整
            const float discounted_days = days * pow(PARAMS[6], (d - s - days));
            SetDSC(d, s, t + 1, max(0.0f, discounted_days) + DSC(d, s, t));
          }
        }
      }
    }
  }

  static void UpdateDSC(const State& state) {
    dscount2ruiseki.assign(101 * 101 * 401, 0);
    rep(d, 101) {
      int base_s = ExistNextS(d, 0);
      while (base_s < d) {
        int s = base_s;
        // ぴったりのやつは減衰不要
        int t = 1;
        rep(_t, Crop::ds2counts[d * 101 + s]) {
          SetDSC(d, base_s, t, DSC(d, base_s, t - 1) + d - s);
          t++;
        }

        // ぴったりじゃないやつは、見に行って、埋まる余地があるなら埋める
        // TODO: 他のやつの雪崩も考慮した方が良いか？
        s = ExistNextS(d, s + 1);
        while (s < d) {
          // 空いてる分だけ埋める
          rep(_t, Crop::ds2counts[d * 101 + s] - state.MP(d, s)) {
            SetDSC(d, base_s, t, DSC(d, base_s, t - 1) + d - s);
            t++;
          }
          s = ExistNextS(d, s + 1);
        }

        // 一応埋める
        // TODO: 計算時間的にやらないほうが良いか？
        for (; t < 400; t++) {
          SetDSC(d, base_s, t, DSC(d, base_s, t - 1));
        }

        base_s = ExistNextS(d, base_s + 1);
      }
    }
  }

  void UpdateNewEval() {
    eval = 0.0f;
    rep(d, 101) {
      int s = ExistNextS(d, 0);
      while (s < d) {
        eval += DSCEvalInternal(d, s);
        s = ExistNextS(d, s + 1);
      }
    }
  }

  State(int dummy1, int dummy2) {}

  State() : exist_Ds(0), mp(101 * 101) {
    rep(idx, H * W) {
      ChangeToExist(Pos(idx), 0);
      ChangeToExist(Pos(idx), 100);
    }
    Pos g = S;
    rep(idx, H * W) {
      Pos p(idx);
      if (g.Depth() < p.Depth()) {
        g = p;
      }
    }

    // gまで
    vector<Pos> poses;
    poses.push_back(g);
    while (poses.back() != S) {
      const Pos p = poses.back();
      for (const auto adj : p.Adj()) {
        if (p.Depth() - 1 == adj.Depth()) {
          poses.push_back(adj);
          break;
        }
      }
    }
    reverse(all(poses));

    for (int d = 2; d <= 99; d++) {
#if 0
      // ランダムウォーク
      bitset<400> checked = 0;
      vector<Pos> q;
      q.push_back(S);
      checked[S] = true;

      rep(t, Crop::d2crops[d].size()) {
        if (q.size() == 0) break;
        int i = rand(0, q.size());
        const auto p = q[i];
        q.erase(q.begin() + i);
        ChangeToExist(p, d);
        for (const auto adj : p.Adj()) {
          if (checked[adj]) continue;
          checked[adj] = true;
          q.push_back(adj);
        }
      }
#elif 0
      // 最も遠くへ伸ばす
      rep(t, Crop::d2crops[d].size()) {
        if (t >= (int)poses.size()) break;
        ChangeToExist(poses[t], d);
      }
#elif 0
      // 全埋めの状態からスタート
      rep(idx, H * W) { ChangeToExist(Pos(idx), d); }
#elif 0
      // ランダムに埋めてばらつきを狙う
      vector<bool> exist(400);
      rep(idx, H * W) { exist[idx] = rand(0, 2); }
      exist[S] = true;
      dsu D(H * W);
      rep(idx, H * W) {
        Pos p(idx);
        if (!exist[p]) continue;
        for (const auto adj : p.Adj()) {
          if (!exist[adj]) continue;
          D.merge(p, adj);
        }
      }

      while (true) {
        bool ok = true;
        rep(idx, H * W) {
          Pos p(idx);
          if (!exist[p]) continue;
          if (D.same(p, S)) continue;

          ok = false;
          for (const auto adj : p.Adj()) {
            exist[adj] = true;
            D.merge(p, adj);
            for (const auto adjadj : adj.Adj()) {
              if (!exist[adjadj]) continue;
              D.merge(adj, adjadj);
            }
          }
        }

        if (ok) break;
      }

      rep(idx, H * W) {
        Pos p(idx);
        if (!exist[p]) continue;
        ChangeToEmpty(p, d);
      }
#else
      // modに従って初期化
      rep(idx, H * W) {
        Pos p(idx);
        const int mod = pos2mods[p];
        if (mod == d % kMod) {
          ChangeToExist(p, d);
        }
      }

      // スタートにつなぐ
      {
        int min_depth = INF;
        Pos best_p = Pos::Dummy();
        rep(idx, H * W) {
          Pos p(idx);
          if (pos2mods[p] != d % kMod) continue;
          const int depth = p.Depth();
          if (depth < min_depth) {
            min_depth = depth;
            best_p = p;
          }
        }

        // もうつながってる
        if (min_depth == 0) continue;

        // 最も近いところから入口まで繋ぐ
        Pos current = best_p;
        while (current.Depth() > 0) {
          if (current != best_p) {
            ChangeToExist(current, d);
          }

          for (const auto adj : current.Adj()) {
            if (adj.Depth() < current.Depth()) {
              current = adj;
              break;
            }
          }
        }
        ChangeToExist(current, d);
      }
#endif
    }
    UpdateNewEval();
  }

  int& MP(const int d, const int s) { return mp[d * 101 + s]; }
  int MP(const int d, const int s) const { return mp[d * 101 + s]; }

  bool ExistD(const Pos p, const int d) const {
    assert(0 <= d && d <= 100);
    return exist_Ds[(int)p * 101 + d];
  }

  int PrevS(const Pos p, const int d) const {
    // debug_timers["PrevS"].start();
    int s = d - 1;
    for (; s >= 0 && !ExistD(p, s); s--) {
    }
    // debug_timers["PrevS"].add();

    return s;
  }

  int NextD(const Pos p, const int d) const {
    // debug_timers["NextD"].start();
    int ddd = d + 1;
    for (; ddd <= 100 && !ExistD(p, ddd); ddd++) {
    }
    // debug_timers["NextD"].add();

    return ddd;
  }

  float DSCEvalInternal(const int d, int s) const {
    if (s < 0) return 0;
    s = ExistNextS(d, s);
    const int current = min(MP(d, s), (int)Crop::ds2counts[d * 101 + s]);
    const int remain = MP(d, s) - current;
    const int next_s = ExistNextS(d, s + 1);

    return DSC(d, s, current) + (DSC(d, next_s, MP(d, next_s) + remain) -
                                 DSC(d, next_s, MP(d, next_s)));
  }

  float DSCEval(const int d, int s) const {
    s = ExistNextS(d, s);
    return DSCEvalInternal(d, s) + DSCEvalInternal(d, ExistPrevS(d, s - 1));
  }

  void IncCount(const int s, const int d) {
    eval -= DSCEval(d, s);
    MP(d, ExistNextS(d, s))++;
    eval += DSCEval(d, s);
  }

  void DecCount(const int s, const int d) {
    eval -= DSCEval(d, s);
    MP(d, ExistNextS(d, s))--;
    eval += DSCEval(d, s);
  }

  bool CanPut(const Pos p, const int d) const {
    if (ExistD(p, d)) return false;
    // 隣接が存在するまたはSが必須
    if (p == S) return true;
    for (const auto adj : p.Adj()) {
      if (ExistD(adj, d)) return true;
    }
    return false;
  }

  void ChangeToExist(const Pos p, const int d) {
    assert(!ExistD(p, d));
    // debug_timers["ChangeToExist"].start();

    // 前後をカウントから削除
    const int prev_s = PrevS(p, d);
    const int next_d = NextD(p, d);
    if (0 <= prev_s && next_d <= 100) {
      DecCount(prev_s, next_d);
    }
    exist_Ds[(int)p * 101 + d] = true;
    if (0 <= prev_s) {
      IncCount(prev_s, d);
    }
    if (next_d <= 100) {
      IncCount(d, next_d);
    }
    // debug_timers["ChangeToExist"].add();
  }

  void DebugConnect(const int d) const {
    dsu D(H * W);
    rep(idx, H * W) {
      Pos p(idx);
      if (!ExistD(p, d)) continue;
      for (const auto adj : p.Adj()) {
        if (!ExistD(adj, d)) continue;
        D.merge(p, adj);
      }
    }

    rep(idx, H * W) {
      Pos p(idx);
      if (!ExistD(p, d)) continue;
      assert(D.same(S, p));
    }
  }

  bool CanRemove(const Pos p, const int d) const {
    if (!ExistD(p, d)) return false;
    // 隣接が複数存在している場合は、それらが斜めを介して繋がってないといけないことにする
    // TODO: 事前計算で高速化
    bitset<8> bs = 0;
    int adj_count = 0;
    int i = -1;
    for (const auto adj : p.Adj()) {
      i++;
      if (ExistD(adj, d)) {
        adj_count++;
        bs[i] = true;
      }
    }
    if (p == S && adj_count >= 1) return false;

    if (adj_count <= 1) return true;

    // 高速化 && 4つと繋がってるところは残しておいた方が嬉しい
    if (adj_count >= 4) return false;

    i = 3;
    for (const auto adjadj : adjadjs_check_remove[p]) {
      i++;
      bs[i] = ExistD(adjadj, d);
    }
    return can_remove_bs[(ulong)p * 256 + bs.to_ulong()];
  }

  void ChangeToEmpty(const Pos p, const int d) {
    if (!ExistD(p, d)) return;
    // Emptyにする
    exist_Ds[(int)p * 101 + d] = false;

    const int prev_s = PrevS(p, d);
    const int next_d = NextD(p, d);
    if (0 <= prev_s) {
      DecCount(prev_s, d);
    }
    if (next_d <= 100) {
      DecCount(d, next_d);
    }
    // 前後をカウントに追加
    if (0 <= prev_s && next_d <= 100) {
      IncCount(prev_s, next_d);
    }
  }

  float Evaluate() const { return eval; }

  int CalcScore() const {
    int sum = 0;
    for (int d = 2; d <= 100; d++) {
      int s = ExistNextS(d, 0);
      int remain = 0;
      while (s < d) {
        remain += MP(d, s);
        const int limit = Crop::ds2counts[d * 101 + s];
        if (limit >= remain) {
          sum += remain * (d - s);
          remain = 0;
        } else {
          sum += limit * (d - s);
          remain -= limit;
        }
        s = ExistNextS(d, s + 1);
      }
    }
    return sum;
  }

  Plan ConvertToPlan() const {
    Plan plan;
    vector<bool> used(K);
    rep(idx, H * W) {
      const Pos p(idx);
      int s = 0;
      int d = NextD(p, s);
      while (d <= 100) {
        if (ExistD(p, s) && ExistD(p, d)) {
          int max_days = -INF;
          int best_k = -1;
          for (const auto crop : Crop::d2crops[d]) {
            if (used[crop.k]) continue;
            if (crop.s < s) continue;
            const int days = crop.Days();
            if (days > max_days) {
              max_days = days;
              best_k = crop.k;
            }
          }
          if (best_k != -1) {
            used[best_k] = true;
            plan.Add(Crop::all_crops[best_k], p, s);
          }
        }

        s = d;
        d = NextD(p, d);
      }
    }

    return plan;
  }
};

struct NBD {
  virtual double Diff() const = 0;
  void Update(State*) const {}
  virtual void Restore(State*) const = 0;
};

struct NBDChange : public NBD {
  NBDChange() {}
  NBDChange(State* const sol, const double progress) {
    diff = -sol->Evaluate();
    success = false;
    rep(t, 10) {
      p = RandPos(progress);
      d = RandD();
      exist = sol->ExistD(p, d);

      if (exist) {
        if (!sol->CanRemove(p, d)) continue;
        sol->ChangeToEmpty(p, d);
      } else {
        if (pos2mods[p] != d % kMod && rand(0, 100) < PARAMS[7]) continue;
        if (!sol->CanPut(p, d)) continue;
        sol->ChangeToExist(p, d);
      }
      success = true;
      break;
    }
    diff += sol->Evaluate();
  }
  double Diff() const override { return -diff; }
  void Restore(State* sol) const override {
    if (!success) return;
    if (!exist) {
      sol->ChangeToEmpty(p, d);
    } else {
      sol->ChangeToExist(p, d);
    }
  }

  double diff;
  Pos p;
  int d;
  bool exist;
  bool success;
};

struct NBDAdjCpy : public NBD {
  NBDAdjCpy() {}
  NBDAdjCpy(State* const sol, const double progress) {
    diff = -sol->Evaluate();
    while (true) {
      p = RandPos(progress);
      d = -1;
      adj = rand_vec(p.Adj());
      const int len =
          rand((int)PARAMS[9], min(99, (int)(PARAMS[9] + PARAMS[10])));
      start_d = rand(0, 100 - len);
      end_d = start_d + len;

      for (int ddd = start_d; ddd < end_d; ddd++) {
        exist = sol->ExistD(p, ddd);
        const bool adj_exist = sol->ExistD(adj, ddd);
        if (exist == adj_exist) continue;
        if (exist) {
          if (!sol->CanRemove(p, ddd)) continue;
          sol->ChangeToEmpty(p, ddd);
          d = ddd;
          break;
        } else {
          sol->ChangeToExist(p, ddd);
          d = ddd;
          break;
        }
      }
      break;
    }
    diff += sol->Evaluate();
  }
  double Diff() const override { return -diff; }
  void Restore(State* sol) const override {
    if (d == -1) return;
    if (!exist) {
      sol->ChangeToEmpty(p, d);
    } else {
      sol->ChangeToExist(p, d);
    }
  }

  double diff;
  bool exist;
  Pos p;
  int d;
  int start_d, end_d;
  Pos adj;
};

struct NBDMove : public NBD {
  NBDMove() {}
  NBDMove(State* const sol, const double progress) {
    // 近傍として筋が悪いっぽい？
    diff = -sol->Evaluate();
    success = false;
    rep(_t, 10) {
      p = RandPos(progress);
      d = RandD();
      if (!sol->ExistD(p, d)) continue;
      next_d = rand(0, 2) ? d + 1 : d - 1;
      if (d < 2 || d > 98) continue;
      if (sol->ExistD(p, next_d)) continue;

      if (!sol->CanRemove(p, d)) continue;
      if (!sol->CanPut(p, next_d)) continue;

      sol->ChangeToEmpty(p, d);
      sol->ChangeToExist(p, next_d);
      success = true;
      break;
    }
    diff += sol->Evaluate();
  }
  double Diff() const override { return -diff; }
  void Restore(State* sol) const override {
    if (!success) return;
    sol->ChangeToEmpty(p, next_d);
    sol->ChangeToExist(p, d);
  }

  double diff;
  Pos p;
  int d, next_d;
  bool success;
};

struct NBDAddRange : public NBD {
  NBDAddRange() {}
  NBDAddRange(State* const sol, const double progress) {
    diff = -sol->Evaluate();
    // 近傍として筋が悪いらしい？
    rep(t, 10) {
      ps.clear();
      d = RandD();
      auto p = RandPos(progress);

      if (!sol->CanPut(p, d)) continue;
      ps.push_back(p);
      sol->ChangeToExist(p, d);

      for (const auto adj : p.Adj()) {
        if (!sol->CanPut(adj, d)) continue;
        ps.push_back(adj);
        sol->ChangeToExist(adj, d);
      }
      break;
    }
    diff += sol->Evaluate();
  }
  double Diff() const override { return -diff; }
  void Restore(State* sol) const override {
    REP(i, ps.size()) { sol->ChangeToEmpty(ps[i], d); }
  }

  double diff;
  vector<Pos> ps;
  int d;
};

struct NBDGenerator {
  NBDGenerator() : best_state(0, 0) {}
  NBD* Generate(State* const sol, const int g, const double progress) {
#ifdef DEBUG
    if (g == 1'000'000) {
      const auto best_plan = sol->ConvertToPlan();
      cout << best_plan << endl;
      cerr << "pseudo_score: " << (int)std::round(sol->Evaluate()) * 25 * 50
           << endl;
      cerr << "score: " << best_plan.Score() << endl;
      std::quick_exit(0);
    }
#endif
    if (best_state.Evaluate() < sol->Evaluate()) {
      best_state = *sol;
    }
    const int random = rand(0, 100);
    if (random < PARAMS[0]) {
      nbd1_ = NBDChange(sol, progress);
      return &nbd1_;
    } else if (random < 100) {
      nbd2_ = NBDAdjCpy(sol, progress);
      return &nbd2_;
    } else {
      nbd4_ = NBDAddRange(sol, progress);
      return &nbd4_;
    }
  }

  State BestState() const { return best_state; }

 private:
  NBDChange nbd1_;
  NBDAdjCpy nbd2_;
  NBDMove nbd3_;
  NBDAddRange nbd4_;
  State best_state;
};

class Solver {
 public:
  explicit Solver(istream& is) {
    Output::StaticInit(is);
    State::StaticInit();
  }

  void Solve(const int time_limit) {
    // 暫定順位表2位のため.......
    xorshift();

    Timer timer;
    timer.start();

    State sol;
    NBDGenerator gen;
    SimulatedAnnealing<State, NBDGenerator> SA(time_limit - timer.ms(),
                                               PARAMS[1], PARAMS[2]);
    const auto final_sol = SA.run(sol, gen);
    auto best_sol = gen.BestState();
    const auto best_plan = best_sol.ConvertToPlan();
    cout << best_plan << endl;
    cerr << "pseudo_score: " << (int)std::round(best_sol.Evaluate()) * 25 * 50
         << endl;
    // best_sol.UpdateNewEval();
    // cerr << (int)std::round(best_sol.Evaluate()) * 25 * 50 << endl;
    cerr << "score: " << best_plan.Score() << endl;
    std::quick_exit(0);
    return;
  }
};

int main(int argc, char* argv[]) {
  fast_io;

  if (argc >= 2) {
    int idx = 0;
    for (int i = 1; i < argc; ++i) {
      PARAMS[idx++] = std::stod(argv[i]);
    }
  }

  Timer timer;
  timer.start();
  Solver solver(cin);
  solver.Solve(1980 - timer.ms());
  return 0;
}