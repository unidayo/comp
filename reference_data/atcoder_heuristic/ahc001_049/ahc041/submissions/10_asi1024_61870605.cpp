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
#define HHH(x)
#define LOG(x)
#define GRAPH(x, y) cerr << "graph " << x << " " << y << endl;

template <typename T> T &chmin(T &a, const T &b) { return a = std::min(a, b); }
template <typename T> T &chmax(T &a, const T &b) { return a = std::max(a, b); }

using ll = long long;
using ld = double;
using P = std::pair<int, int>;
const int INF = 1e9;

// Template
std::ostream& operator<<(std::ostream& os, const P& p) {
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
  static constexpr double time_limit = 1.99;
  static constexpr int64_t CYCLES_PER_SEC = 2900000000;
  static constexpr double CYCLES = CYCLES_PER_SEC * time_limit;
  int64_t start, stop;
  inline int64_t get_cycle() const {
    uint32_t low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return int64_t(low) | (int64_t(high) << 32);
  }
  Timer(double time_limit) : start(get_cycle()), stop(start + CYCLES) {}
  Timer() : Timer(0) {}
  inline bool within(int64_t end) const { return get_cycle() < end; }
  inline bool within() const { return within(stop); }
  inline double elapsed() const { return (get_cycle() - start) / double(CYCLES_PER_SEC); }
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
  return stod(std::string(ret));
}

// double a_param = get_param("A_PARAM");
// double b_param = get_param("B_PARAM");
// double c_param = get_param("C_PARAM");
// int a_param = get_param("A_PARAM");
// int j_param = get_param("J_PARAM");

Timer timer;
XorShift rnd;

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0xFF;
  constexpr static double START_TEMP = 250;
  constexpr static double END_TEMP = 3;
  constexpr static double TEMP_RATIO = (END_TEMP - START_TEMP) / Timer::CYCLES;
  double logp[LOG_SIZE];
  long long iteration = 0;
  int64_t cycle = 0;
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
    if ((iteration & UPDATE_INTERVAL) != 0) return false;
    cycle = timer.get_cycle();
    temp = START_TEMP + (cycle - timer.start) * TEMP_RATIO;
    return cycle >= timer.stop;
  }

  inline bool Accept(double diff) {
    static unsigned short index = 0;
    return diff >= 0 || diff > logp[index++] * temp;
  }
};

using namespace std;

constexpr int N = 1000;
constexpr int MAX_M = 3000;
int M;
constexpr int H = 10;
pair<int, int> E[MAX_M];
int A[N];
vector<int> G[N];

void setup() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, h;
  cin >> n >> M >> h;
  REP(i,N) cin >> A[i];
  REP(i,M) {
    int u, v;
    cin >> u >> v;
    E[i] = {u, v};
    G[u].push_back(v);
    G[v].push_back(u);
  }
}

struct Node {
  int parent;
  int root_depth;
  int leaf_depth;
  int leaf_sum;
  vector<int> children;
  Node() : parent(-1), root_depth(0), leaf_depth(0), leaf_sum(0) {
    children.reserve(5);
  }
};

struct State {
  std::vector<Node> nodes;
  State() : nodes(N) {
    REP(i,N) nodes[i].leaf_sum = A[i];
  }
  int GetDiff(int node, int new_parent) const {
    if (nodes[node].parent == new_parent) return -INF;
    int new_root_depth = new_parent == -1 ? 0 : nodes[new_parent].root_depth + 1;
    Node my_node = nodes[node];
    // LOG(node);
    // LOG(new_parent);
    // LOG(nodes[node].leaf_depth);
    // for (int child : my_node.children) {
    //   std::cerr << child << " " << nodes[child].leaf_depth << ": ";
    //   for (int child2 : nodes[child].children) {
    //     std::cerr << child2 << " " << nodes[child2].leaf_depth << ", ";
    //   }
    //   std::cerr << std::endl;
    // }
    // std::cerr << std::endl;
    // LOG(new_root_depth);
    // LOG(my_node.leaf_depth);
    int new_depth = new_root_depth + my_node.leaf_depth;
    // LOG(new_depth);
    if (new_depth > H) return -INF;

    // Check cycle
    int ptr = new_parent;
    while (ptr >= 0) {
      if (ptr == node) return -INF;
      ptr = nodes[ptr].parent;
    }

    int depth_diff = new_root_depth - my_node.root_depth;
    return depth_diff * my_node.leaf_sum;
  }

  int GetDiff2(int node1, int node2) {
    return A[node1] - A[node2];
  }

  // void CheckRoot() const {
  //   REP(i,N) {
  //     if (nodes[i].parent < 0) {
  //       assert(nodes[i].root_depth == 0);
  //     } else {
  //       assert(nodes[i].root_depth == nodes[nodes[i].parent].root_depth + 1);
  //     }
  //   }
  // }

  // void CheckLeaf() const {
  //   REP(i,N) {
  //     int max_leaf_depth = -1;
  //     for (int child : nodes[i].children) {
  //       chmax(max_leaf_depth, nodes[child].leaf_depth);
  //     }
  //     assert(nodes[i].leaf_depth == max_leaf_depth + 1);
  //   }
  // }

  // void CheckSanity() const {
  //   REP(i,N) {
  //     for (int child : nodes[i].children) {
  //       if (nodes[child].parent != i) {
  //         std::cerr << i << " " << child << " " << nodes[child].parent << std::endl;
  //         assert(false);
  //       }
  //     }
  //   }

  //   REP(i,N) {
  //     int parent = nodes[i].parent;
  //     if (parent >= 0) {
  //       int count = std::count(ALL(nodes[parent].children), i);
  //       assert(count == 1);
  //     }
  //   }

  //   CheckRoot();
  //   CheckLeaf();

  //   int max_root_depth = 0;
  //   REP(i,N) chmax(max_root_depth, nodes[i].root_depth);
  // }

  void ResetRootSide(int node, int diff_sum) {
    while (node >= 0) {
      int new_leaf_depth = -1;
      for (int child : nodes[node].children) {
        chmax(new_leaf_depth, nodes[child].leaf_depth);
      }
      new_leaf_depth++;

      nodes[node].leaf_depth = new_leaf_depth;
      nodes[node].leaf_sum += diff_sum;
      node = nodes[node].parent;
    }
  }

  // void ResetLeafSide(int node, int root_depth_diff) {
  //   nodes[node].root_depth += root_depth_diff;
  //   for (int child : nodes[node].children) {
  //     ResetLeafSide(child, root_depth_diff);
  //   }
  // }

  void Move(int node, int new_parent) {
    Node my_node = nodes[node];
    if (my_node.parent >= 0) {
      // Cut
      auto it = std::find(ALL(nodes[my_node.parent].children), node);
      assert(it != nodes[my_node.parent].children.end());
      nodes[my_node.parent].children.erase(it);
      ResetRootSide(my_node.parent, -my_node.leaf_sum);
    }

    nodes[node].parent = new_parent;

    if (new_parent >= 0) {
      // Paste
      nodes[new_parent].children.push_back(node);
      ResetRootSide(new_parent, my_node.leaf_sum);
    }

    // Update
    int new_root_depth = new_parent == -1 ? 0 : nodes[new_parent].root_depth + 1;
    if (my_node.root_depth != new_root_depth) {
      int depth_diff = new_root_depth - my_node.root_depth;
      static std::vector<int> stack;
      stack.push_back(node);
      while (!stack.empty()) {
        int node = stack.back();
        stack.pop_back();
        nodes[node].root_depth += depth_diff;
        for (int child : nodes[node].children) {
          stack.push_back(child);
        }
      }
    }
  }
  void Output() const {
    REP(i,N) {
      cout << nodes[i].parent << " ";
    }
    cout << endl;
  }
};

int main() {
  setup();

  SimulatedAnnealing sa;

  State state;
  int score = 1;
  REP(i,N) score += A[i];

  int best_score = score;
  State best_state = state;
  // int count_ok = 0;

  while (!sa.End()) {
    if (rnd.next(1000) == 0) {
      int u = rnd.next(N);
      if (state.nodes[u].leaf_depth != 1 || state.nodes[u].children.size() != 1) continue;
      int v = state.nodes[u].children[0];
      int parent = state.nodes[u].parent;
      if (std::find(G[v].begin(), G[v].end(), parent) == G[v].end()) continue;
      assert(state.nodes[v].parent == u);
      int diff = state.GetDiff2(u, v);
      if (sa.Accept(diff)) {
        Node& node_u = state.nodes[u];
        node_u.parent = v;
        node_u.root_depth++;
        node_u.leaf_depth = 0;
        node_u.leaf_sum = A[u];
        node_u.children.clear();

        Node& node_v = state.nodes[v];
        node_v.parent = parent;
        node_v.root_depth--;
        node_v.leaf_depth = 1;
        node_v.leaf_sum = A[v] + A[u];
        assert(node_v.children.empty());
        node_v.children.push_back(u);

        if (parent >= 0) {
          auto it = std::find(ALL(state.nodes[parent].children), u);
          assert(it != state.nodes[parent].children.end());
          *it = v;
        }
        // state.CheckSanity();

        score += diff;
        if (score > best_score) {
          best_state = state;
          best_score = score;
        }
      }
      continue;
    }

    auto [u, v] = E[rnd.next(M)];
    int diff = 0;
    if (state.nodes[u].parent == -1 && state.nodes[v].parent == -1) {
      int diff1 = state.GetDiff(u, v);
      int diff2 = state.GetDiff(v, u);
      if (diff1 > diff2) {
        diff = diff1;
      } else {
        diff = diff2;
        swap(u, v);
      }
    }
    else {
      if (rnd.next(2)) swap(u, v);
      if (rnd.next(10) == 0) v = -1;
      diff = state.GetDiff(u, v);
    }
    if (sa.Accept(diff)) {
      state.Move(u, v);
      score += diff;
      if (score > best_score) {
        best_state = state;
        best_score = score;
      }
    }
  }

  best_state.Output();
  LOG(best_score);
  LOG(sa.iteration);
  // LOG(count_ok);
  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
  return 0;
}
