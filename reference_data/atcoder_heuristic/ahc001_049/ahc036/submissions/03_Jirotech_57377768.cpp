

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
#define all(v) v.begin(), v.end()
typedef pair<int, int> Pii;
typedef pair<ll, ll> Pll;
const double PI = acos(-1);

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

int rand(const int l, const int r) {
  return xorshift() % (r - l) + l;
}

int rand_other_than(const int l, const int r,
                         const int other) {
  const int num = rand(l, r - 1);
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

template <class T, class U = T>
bool chmin(T& x, U&& y) {
  return y < x && (x = std::forward<U>(y), true);
}

template <class T, class U = T>
bool chmax(T& x, U&& y) {
  return x < y && (x = std::forward<U>(y), true);
}

template <typename Ret, typename T>
Ret Sum(const vector<T>& vec) {
  return std::accumulate(all(vec), (Ret)0);
}

template <typename Ret, typename T>
Ret Mean(const vector<T>& vec) {
  assert((int)vec.size() > 0);
  return Sum<T, Ret>(vec) / vec.size();
}

template <typename Ret, typename T>
Ret Std(const vector<T>& vec) {
  assert((int)vec.size() > 0);
  const auto mean =  Mean<Ret>(vec);
  const auto sum2 = std::accumulate(
      all(vec), (Ret)0,
      [](const Ret acc, const T val) { return acc + (Ret)val * val; });
  return (Ret)sum2 / vec.size() - mean * mean;
}

template<typename T, typename U>
T Ceil(const T a, const U b){
  assert(a >= 0);
  assert(b > 0);
  return (a + b - 1) / b;
}

template<typename T, typename U>
T Mod(const T a, const U b){
  return (a + b) % b;
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





/* start */

template<typename T>
struct IndexSet {
  vector<T> que;
  vector<int> pos;

  IndexSet(int n) : pos(n, -1) {}

  const vector<T>& ToVec() const{
    return que;
  }

  void addIfNoExist(T v) {
    if (contains(v)) {
      return;
    }
    pos[v] = que.size();
    que.push_back(v);
  }

  void add(T v) {
    assert(!contains(v));
    pos[v] = que.size();
    que.push_back(v);
  }

  void removeIfExist(T v) {
    if (!contains(v)) {
      return;
    }
    remove(v);
  }

  void remove(T v) {
    int p = pos[v];
    T b = que.back();
    que[p] = b;
    que.pop_back();
    pos[b] = p;
    pos[v] = -1;
  }

  bool contains(T v) const {
    return pos[v] != -1;
  }

  int size() const {
    return que.size();
  }

  void clear() {
    while(!que.empty()){
      remove(que.back());
    }
  }
};



/* start */

Timer global_timer;
int N, M, T, LA, LB;

struct Node {
  int id;
  Node() {}
  explicit Node(int _id) : id(_id) {}
  bool operator==(const Node& n) const { return id == n.id; }
  bool operator!=(const Node& n) const { return id != n.id; }
  bool operator<(const Node& n) const { return id < n.id; }
  bool operator>(const Node& n) const { return id > n.id; }
  bool operator<=(const Node& n) const { return id <= n.id; }
  bool operator>=(const Node& n) const { return id >= n.id; }
  bool IsValid() const { return 0 <= id && id < N; }

  // intへの変換
  operator int() const { return id; }
};

// unordered_set用のNodeのハッシュ関数
namespace std {
template <>
struct hash<Node> {
  size_t operator()(const Node& n) const { return hash<int>()(n.id); }
};
}  // namespace std

struct Query{
  int id;
  Node from;
  Node to;
  Query(int _id, Node _from, Node _to) : id(_id), from(_from), to(_to) {}
};

vector<Node> inputted_orders;
vector<pair<Node, Node>> inputted_queries;
vector<Query> inputtedQueries;
vector<vector<int>> node2QueryIndexes;
unordered_set<Node> allVisitNodes;
unordered_set<Node> allFromNodes;

struct Graph {
  vector<IndexSet<Node>> g;
  Graph() {}
  Graph(int n) : g(n, IndexSet<Node>(n)) {}
  const vector<Node>& NextNodes(Node v) const { return g[v.id].que; }
  void add_edge(Node u, Node v) {
    g[u.id].add(v);
    g[v.id].add(u);
  }
  void remove_edge(Node u, Node v) {
    g[u.id].remove(v);
    g[v.id].remove(u);
  }
  void try_remove_edge(Node u, Node v) {
    if (g[u.id].contains(v)) remove_edge(u, v);
    if (g[v.id].contains(u)) remove_edge(v, u);
  }
  bool HasEdge(Node u, Node v) const { return g[u.id].contains(v); }
};

Graph G;
vector<vector<int>> nodeDists;

template <typename Node>
struct WeightedEdge {
  Node to;
  int w;

  WeightedEdge(Node _to, int _w) : to(_to), w(_w) {}
};

template <typename Node>
struct WeightedGraph {
  vector<vector<WeightedEdge<Node>>> g;
  vector<vector<WeightedEdge<Node>>> rG;
  vector<vector<bool>> hasEdge;
  // 通常のグラフから重み付きグラフを構築
  // 通常のグラフは問題の元々のグラフを想定
  // 重みはINFを想定
  WeightedGraph(int n, Graph normalGraph, int baseWeight)
      : g(n), rG(n), hasEdge(n, vector<bool>(n, false)) {
    rep(i, n) {
      for (Node v : normalGraph.NextNodes(Node(i))) {
        add_edge(Node(i), v, baseWeight);
      }
    }
  }
  const vector<WeightedEdge<Node>>& Edges(Node v) const { return g[v.id]; }
  const vector<WeightedEdge<Node>>& RevEdges(Node v) const { return rG[v.id]; }
  const WeightedEdge<Node>& GetEdge(Node u, Node v) const {
    for (const auto& edge : g[u.id]) {
      if (edge.to == v) {
        return edge;
      }
    }
    assert(false);
    return g[u.id][0];
  }
  void add_edge(Node u, Node v, int w) {
    if (hasEdge[u][v]) {
      const auto edge = GetEdge(u, v);
      // TODO: 両方持っておく
      if (edge.w <= w) return;
      remove_edge(u, v);
    }
    g[u].emplace_back(v, w);
    rG[v].emplace_back(u, w);
    hasEdge[u][v] = true;
  }
  void remove_edge(Node u, Node v) {
    hasEdge[u][v] = false;
    g[u].erase(
        remove_if(all(g[u]), [&](const auto& edge) { return edge.to == v; }),
        g[u].end());
    rG[v].erase(
        remove_if(all(rG[v]), [&](const auto& edge) { return edge.to == u; }),
        rG[v].end());
  }
  // void try_remove_edge(Node u, Node v) {
  //   if (hasEdge[u][v]) remove_edge(u, v);
  // }
  bool HasEdge(Node u, Node v) const { return hasEdge[u][v]; }
};

void InitDist() {
  // ワーシャルフロイド
  nodeDists.assign(N, vector<int>(N, INF));
  rep(i, N) { nodeDists[i][i] = 0; }
  rep(i, N) {
    for (Node v : G.NextNodes(Node(i))) {
      nodeDists[i][v.id] = 1;
    }
  }
  rep(k, N) {
    rep(i, N) {
      rep(j, N) { chmin(nodeDists[i][j], nodeDists[i][k] + nodeDists[k][j]); }
    }
  }
}

int Dist(const Node u, const Node v) {
  if (nodeDists.empty()) {
    InitDist();
  }
  return nodeDists[u.id][v.id];
}

struct Command {
  static Command Change(int len, int ai, int bi) {
    return Command(len, ai, bi);
  }
  static Command Move(Node v) { return Command(v); }

  string ToString() const {
    if (isMove) {
      return "m " + to_string(v.id);
    } else {
      return "s " + to_string(len) + " " + to_string(ai) + " " + to_string(bi);
    }
  }

 private:
  Command(int len, int ai, int bi) : isMove(false), len(len), ai(ai), bi(bi) {}
  Command(Node v) : isMove(true), v(v) {}

  bool isMove;
  int len;
  int ai;
  int bi;
  Node v;
};

struct AObj {
  vector<Node> A;

  AObj(vector<Node> _A) : A(_A) {}
};

struct BObj {
  vector<Node> B;

  BObj() : B(LB, Node(-1)) {}

  BObj(const AObj& A, const int ai) {
    rep(i, LB) { B.push_back(A.A[ai + i]); }
  }
  BObj(vector<Node> _B) : B(_B) { assert((int)B.size() == LB); }

  void Change(const AObj& A, int l, int ai, int bi) {
    rep(i, l) { B[bi + i] = A.A[ai + i]; }
  }

  // Bがあることによってnodeが移動可能か
  bool IsAdj(const Node node) const {
    for (Node v : B) {
      if (!v.IsValid()) continue;
      // その場足踏みは意味がない
      if (node == v) continue;
      if (G.HasEdge(node, v)) return true;
    }
    return false;
  }

  vector<Node> MovableNodes(Node from) const {
    assert((int)B.size() == LB);
    // TODO: unordered_setに変更する
    unordered_set<Node> BSet(all(B));
    unordered_set<Node> ret;
    queue<Node> que;
    que.push(from);
    while (!que.empty()) {
      Node v = que.front();
      que.pop();
      for (Node u : G.NextNodes(v)) {
        if (ret.count(u)) continue;
        if (BSet.count(u) == 0) continue;
        ret.insert(u);
        que.push(u);
      }
    }

    return vector<Node>(all(ret));
  }

  // ある位置からある位置へ移動するコマンド列
  vector<Command> MoveCommands(Node from, Node to) const {
    assert((int)B.size() == LB);
    vector<Command> ret;
    if (from == to) {
      return ret;
    }
    set<Node> BSet(all(B));
    bitset<600> visited = 0;
    queue<Node> que;
    que.push(from);
    visited[from.id] = 1;
    vector<Node> prev(600);
    while (!que.empty()) {
      Node v = que.front();
      que.pop();
      if (v.id == to.id) {
        while (v.id != from.id) {
          ret.push_back(Command::Move(v));
          v = prev[v.id];
        }
        reverse(all(ret));
        return ret;
      }
      for (Node u : G.NextNodes(v)) {
        if (visited[u.id]) continue;
        if (BSet.count(u) == 0) continue;
        visited[u.id] = 1;
        prev[u.id] = v;
        que.push(u);
      }
    }

    cerr << "from: " << from.id << " to: " << to.id << endl;
    cerr << "B: ";
    for (const auto b : B) {
      cerr << b << " ";
    }
    cerr << endl;
    assert(false);
    return ret;
  }
};

struct Action {
  int len;
  int ai;
  int bi;
  // 次のオーダーがnextOrderになるように訪問したうえで、toに移動する
  Node to;
  int nextOrder;
  Action(int _len, int _ai, int _bi, Node _to, int _nextOrder)
      : len(_len), ai(_ai), bi(_bi), to(_to), nextOrder(_nextOrder) {}

  // Bを変更するので注意
  vector<Command> ToCommands(Node currentNode, const AObj& A, BObj* B,
                             int currentNextOrder) const {
    vector<Command> ret;
    ret.push_back(Command::Change(len, ai, bi));
    assert((int)A.A.size() == LA);
    assert((int)B->B.size() == LB);
    B->Change(A, len, ai, bi);
    // 現在位置から、オーダーを完了させる
    for (int order = currentNextOrder; order < nextOrder; order++) {
      // オーダーを完了させる
      const auto nextNode = inputted_orders[order];
      const vector<Command> commands = B->MoveCommands(currentNode, nextNode);
      ret.insert(ret.end(), all(commands));
      currentNode = nextNode;
    }

    // 最後に、目的地に移動する
    const vector<Command> commands = B->MoveCommands(currentNode, to);
    ret.insert(ret.end(), all(commands));
    return ret;
  }
};

void PrintAnswer(const AObj& A, const vector<Action>& actions) {
  for (const auto a : A.A) {
    cout << a.id << " ";
  }
  cout << endl;
  BObj B;
  Node currentNode(0);
  int nextOrder = 0;
  for (const auto action : actions) {
    const auto commands =
        action.ToCommands(currentNode, A, (BObj*)&B, nextOrder);
    for (const auto command : commands) {
      cout << command.ToString() << endl;
    }
    currentNode = action.to;
    nextOrder = action.nextOrder;
  }

#ifdef LOCAL
  vector<int> order2Steps(T, 0);
  int prevOrder = 0;
  for (const auto action : actions) {
    order2Steps[prevOrder]++;
    prevOrder = action.nextOrder;
  }
  map<int, int> step2OrderCount;
  rep(i, T) {
    step2OrderCount[order2Steps[i]]++;
    // cerr << "order: " << i << " steps: " << order2Steps[i] << endl;
  }
  cerr << "------------" << endl;
  for (const auto [step, count] : step2OrderCount) {
    cerr << "step: " << step << " count: " << count << endl;
  }
#endif

  cerr << "final_time: " << global_timer.ms() << endl;
  cerr << "score: " << actions.size() << endl;
}

vector<vector<vector<Node>>> minPathCache;

vector<Node> CalcMinPath(const Node from, const Node to) {
  if (minPathCache.empty()) {
    minPathCache.assign(N, vector<vector<Node>>(N));
  }

  if(from == to){
    return {};
  }

  if ((int)minPathCache[from.id][to.id].size() > 0) {
    return minPathCache[from.id][to.id];
  }
  vector<Node> ret;
  vector<int> dist(N, -1);
  vector<Node> prev(N, Node(-1));
  queue<Node> que;
  que.push(from);
  dist[from.id] = 0;
  while (!que.empty()) {
    Node v = que.front();
    que.pop();
    if (v == to) {
      while (v != from) {
        ret.push_back(v);
        v = prev[v.id];
      }
      reverse(all(ret));
      minPathCache[from.id][to.id] = ret;
      return ret;
    }
    for (Node u : G.NextNodes(v)) {
      if (dist[u.id] != -1) continue;
      dist[u.id] = dist[v.id] + 1;
      prev[u.id] = v;
      que.push(u);
    }
  }
  assert(false);
  return ret;
}

vector<Node> CalcBestMinPath(const Node from, const Node to,
                             unordered_map<Node, int> node2Priorities) {
  // 距離が最小のものを優先する
  // 距離が等しいなら、優先度の和が最大のものを優先する
  vector<Node> ret;
  vector<pair<int, int>> distAndPriorities(N, {INF, INF});
  vector<Node> prev(N, Node(-1));
  queue<Node> que;
  que.push(from);
  // 優先度の値が存在しないなら優先度は0
  distAndPriorities[from.id] = {0, -node2Priorities[from]};
  while (!que.empty()) {
    Node v = que.front();
    que.pop();
    if (v == to) {
      while (v != from) {
        ret.push_back(v);
        v = prev[v.id];
      }
      reverse(all(ret));
      return ret;
    }
    const int currentDist = distAndPriorities[v.id].first + 1;
    const int currentPriority = distAndPriorities[v.id].second;
    for (Node u : G.NextNodes(v)) {
      // 符号反転するために引き算
      const auto nextEval =
          make_pair(currentDist + 1, currentPriority - node2Priorities[u]);
      if (chmin(distAndPriorities[u.id], nextEval)) {
        prev[u.id] = v;
        que.push(u);
      }
    }
  }
  assert(false);
  return ret;
}

struct Pos {
  int x, y;
  Pos() {}
  Pos(int _x, int _y) : x(_x), y(_y) {}
};
vector<Pos> poses;

// 全ての頂点からの最大距離が最小になる頂点
// 同距離の場合は、次数が大きい頂点
Node centerNode;
vector<Node> centerNodes;

Node CalcNearestNode(const Pos p, const unordered_set<Node>& targetNodes) {
  Node ret = Node(0);
  ll minDist = LINF;
  for (const auto node : targetNodes) {
    const Pos q = poses[node.id];
    const ll dist =
        (ll)(p.x - q.x) * (p.x - q.x) + (ll)(p.y - q.y) * (p.y - q.y);
    if (dist < minDist) {
      minDist = dist;
      ret = node;
    }
  }
  return ret;
}

Node CalcNearestNode(const Pos p) {
  unordered_set<Node> targetNodes;
  rep(i, N) { targetNodes.insert(Node(i)); }
  return CalcNearestNode(p, targetNodes);
}

void InitCenterNode() {
  // 最大距離, -1 * 次数 が小さい頂点を中心とする
  pair<int, int> minCost = {INF, INF};
  centerNode = Node(0);
  for (int i = 0; i < N; i++) {
    int maxDist = 0;
    int distSum = 0;
    int degree = G.NextNodes(Node(i)).size();
    rep(j, N) { chmax(maxDist, Dist(Node(i), Node(j)));
      distSum += Dist(Node(i), Node(j));
    }
    const pair<int, int> cost = {distSum, -degree};
    if (chmin(minCost, cost)) {
      centerNode = Node(i);
    }
  }

  centerNodes = {centerNode};
  const auto candidates = G.NextNodes(centerNode);
  // 互いに隣接する複数のノードを中心とする
  // TODO: 個数の最大化
  for (const auto node : candidates) {
    bool ok = true;
    for (const auto center : centerNodes) {
      if (!G.HasEdge(center, node)) {
        ok = false;
        break;
      }
    }
    if (ok) {
      centerNodes.push_back(node);
    }
  }

  cerr << "centerNodeCount: " << centerNodes.size() << endl;
}

vector<uint64_t> node2RandomHashes;

struct Output {
  static void StaticInit(istream& is) {
    global_timer.start();
    is >> N >> M >> T >> LA >> LB;

    G = Graph(N);
    rep(i, M) {
      int u, v;
      is >> u >> v;
      G.add_edge(Node(u), Node(v));
    }

    node2QueryIndexes.assign(N, vector<int>());
    rep(i, T) {
      int t;
      is >> t;
      if (i == 0) {
        inputted_queries.push_back({Node(0), Node(t)});
        inputtedQueries.push_back(Query(i, Node(0), Node(t)));
      } else {
        inputted_queries.push_back({inputted_orders.back(), Node(t)});
        inputtedQueries.push_back(Query(i, inputted_orders.back(), Node(t)));
      }
      inputted_orders.push_back(Node(t));
      node2QueryIndexes[t].push_back(i);
    }

    rep(i, N) {
      int x, y;
      is >> x >> y;
      poses.push_back(Pos{x, y});
      node2RandomHashes.push_back((((uint64_t)xorshift()) << 32) | xorshift());
    }

    // 訪問する頂点
    allVisitNodes = unordered_set<Node>(all(inputted_orders));
    allVisitNodes.insert(Node(0));
    rep(i, N) { allFromNodes.insert(inputted_queries[i].first); }

    InitCenterNode();
#ifdef LOCAL
    DebugStatistics();
#endif
    // if(LB < 10){
    //   cerr << "score: 1000000000" << endl;
    //   exit(1);
    // }
  }

  static void DebugStatistics() {
    cerr << "N: " << N << endl;
    cerr << "M: " << M << endl;
    cerr << "T: " << T << endl;
    cerr << "LA: " << LA << endl;
    cerr << "LB: " << LB << endl;
    cerr << "visitNodeCount: " << allVisitNodes.size() << endl;

    // 距離の総和
    ll distSum = 0;
    ll maxDist = 0;
    vector<int> dists;
    map<int, int> dist2Counts;
    rep(i, T) {
      const auto [from, to] = inputted_queries[i];
      dists.push_back(Dist(from, to));
      distSum += Dist(from, to);
      chmax(maxDist, Dist(from, to));
      dist2Counts[Dist(from, to)]++;
    }
    cerr << "distSum: " << distSum << endl;
    cerr << "distSum/LB: " << (double)distSum / LB << endl;
    cerr << "meanDist: " << (double)distSum / T << endl;
    cerr << "meadDist/LB: " << (double)distSum / (T * LB) << endl;
    cerr << "maxDist: " << maxDist << endl;
    for (const auto [dist, count] : dist2Counts) {
      cerr << "dist: " << dist << " count: " << count << endl;
    }

    // 中央の点からの距離の平均値と最大値
    vector<int> centerDists;
    map<int, int> centerDist2Counts;
    rep(i, N) {
      centerDists.push_back(Dist(centerNode, Node(i)));
      centerDist2Counts[Dist(centerNode, Node(i))]++;
    }
    sort(all(centerDists));
    cerr << "centerDistMean: " << accumulate(all(centerDists), 0) / N << endl;
    cerr << "centerDistMax: " << centerDists.back() << endl;
    for (const auto [dist, count] : centerDist2Counts) {
      cerr << "centerDist: " << dist << " count: " << count << endl;
    }

    vector<int> centerPairDists;
    map<int, int> centerPairDist2Counts;
    rep(i, T) {
      const auto [from, to] = inputted_queries[i];
      centerPairDists.push_back(Dist(centerNode, from) + Dist(centerNode, to) + 1);
      centerPairDist2Counts[Dist(centerNode, from) + Dist(centerNode, to) + 1]++;
    }
    sort(all(centerPairDists));
    cerr << "centerPairDistMean: " << accumulate(all(centerPairDists), 0) / T << endl;
    cerr << "centerPairDistMax: " << centerPairDists.back() << endl;
    int centerPairCount = 0;
    for (const auto [dist, count] : centerPairDist2Counts) {
      centerPairCount += count;
      cerr << "centerPairDist: " << dist << " count: " << count << " / " << centerPairCount << endl;
    }
  }
  friend ostream& operator<<(ostream& os, const Output& output) { return os; }
};



/* start */

vector<double> PARAMS = {9, 1.0, 0.1, 20, 20, 50, 0, 0, 2.0};



/* start */





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
      if ((g & 0xff) == 0) {
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





/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

namespace NBD2 {

template <typename T>
struct MyDequeue {
  MyDequeue(int size, int n) : _data(size), _data2indexes(n) {
    _begin = size / 2;
    _end = size / 2;
  }
  void PushBack(const T& val) {
    _data2indexes[val].push_back(_end);
    _data[_end++] = val;
  }
  void PushFront(const T& val) {
    _data[--_begin] = val;
    _data2indexes[val].push_back(_begin);
  }
  T PopBack() {
    const auto data = _data[_end - 1];
    rep(i, _data2indexes[data].size()) {
      if (_data2indexes[data][i] == _end - 1) {
        swap(_data2indexes[data][i], _data2indexes[data].back());
        _data2indexes[data].pop_back();
        break;
      }
    }
    return _data[--_end];
  }
  T PopFront() {
    const auto data = _data[_begin];
    rep(i, _data2indexes[data].size()) {
      if (_data2indexes[data][i] == _begin) {
        swap(_data2indexes[data][i], _data2indexes[data].back());
        _data2indexes[data].pop_back();
        break;
      }
    }
    return _data[_begin++];
  }
  T Front() const { return _data[_begin]; }
  T Back() const { return _data[_end - 1]; }
  int Size() const { return _end - _begin; }
  vector<T> ToVector() const {
    vector<T> ret;
    for (int i = _begin; i < _end; i++) {
      ret.push_back(_data[i]);
    }
    return ret;
  }
  const vector<int>& GetDataIndexes(const T& data) const {
    return _data2indexes[data];
  }
  int NormalDist(int idx0, int idx1) const {
    if (idx0 > idx1) {
      swap(idx0, idx1);
    }
    return idx1 - idx0;
  }
  int TorusDist(int idx0, int idx1) const {
    if (idx0 > idx1) {
      swap(idx0, idx1);
    }
    return idx0 + Size() - idx1;
  }

  void Relocation() {
    _data2indexes.clear();
    _data2indexes.resize(N);
    const int mid = (int)_data.size() / 2;
    for (int i = _begin; i < _end; i++) {
      _data[mid + i - _begin] = _data[i];
      _data2indexes[_data[i]].push_back(mid + i - _begin);
    }
    _end = mid + _end - _begin;
    _begin = mid;
  }

  vector<T> _data;
  vector<vector<int>> _data2indexes;
  int _begin = 0;
  int _end = 0;
};

struct Evaluation {
  int existNodeCount = 0;
  vector<int> node2Counts = vector<int>(N, 0);
  int totalNodeCount = 0;
  ll queryDistSum = 0;
  int torusCount = 0;

  double Sum() const {
    const int noExistNodeCount = N - existNodeCount;
    // Aを後から補強できない
    if (totalNodeCount + noExistNodeCount > LA) {
      return INF;
    }
    // 小さいほうが良い
    return noExistNodeCount + (double)queryDistSum * 0.0001;
  }

  // 先頭か末尾に追加
  void AddNode(const Node& node) {
    node2Counts[node.id]++;
    totalNodeCount++;
    if (node2Counts[node.id] == 1) {
      existNodeCount++;
    }
    queryDistSum += torusCount;
  }

  // 先頭か末尾から削除
  void RemoveNode(const Node& node) {
    node2Counts[node.id]--;
    totalNodeCount--;
    if (node2Counts[node.id] == 0) {
      existNodeCount--;
    }
    queryDistSum -= torusCount;
  }
};

struct Solution {
  MyDequeue<Node> nodes = MyDequeue<Node>((int)1e6, N);
  vector<pair<int, int>> query2Indexes = vector<pair<int, int>>(T, {-1, -1});
  mutable Evaluation eval;
  vector<bool> query2Torus = vector<bool>(T, false);

  Solution(){
    eval = InitEval();
    PushBack(Node(0));
  }

  Evaluation InitEval() const {
    Evaluation eval;
    const auto nodeVec = nodes.ToVector();
    for (const auto node : nodeVec) {
      eval.node2Counts[node.id]++;
    }
    rep(i, N) {
      if (eval.node2Counts[i] > 0) {
        eval.existNodeCount++;
      }
    }
    return eval;
  }

  static Solution CreateInitialSol() {
    Solution sol;
    return sol;
  }

  void UpdateBestQuery(const int queryIndex) {
    const auto [from, to] = inputted_queries[queryIndex];
    const auto& fromIdxes = nodes.GetDataIndexes(from);
    const auto& toIdxes = nodes.GetDataIndexes(to);
    int minDist = INF;
    int bestFromIdx = -1;
    int bestToIdx = -1;
    bool isTorusDist = false;
    for (const auto fromIdx : fromIdxes) {
      for (const auto toIdx : toIdxes) {
        const auto normalDist = nodes.NormalDist(fromIdx, toIdx);
        const auto torusDist = nodes.TorusDist(fromIdx, toIdx);
        if (normalDist <= torusDist) {
          if (chmin(minDist, normalDist)) {
            bestFromIdx = fromIdx;
            bestToIdx = toIdx;
            isTorusDist = false;
          }
        } else {
          if (chmin(minDist, torusDist)) {
            bestFromIdx = fromIdx;
            bestToIdx = toIdx;
            isTorusDist = true;
          }
        }
      }
    }

    query2Indexes[queryIndex] = {bestFromIdx, bestToIdx};
    query2Torus[queryIndex] = isTorusDist;
  }

  void UpdateQueryEval(int queryIndex, bool isInc) {
    const auto [fromIdx, toIdx] = query2Indexes[queryIndex];
    if(fromIdx == -1 || toIdx == -1) return;
    const auto dist = query2Torus[queryIndex]
                          ? nodes.TorusDist(fromIdx, toIdx)
                          : nodes.NormalDist(fromIdx, toIdx);
    if (isInc) {
      eval.queryDistSum += dist;
      eval.torusCount += query2Torus[queryIndex];
    } else {
      eval.queryDistSum -= dist;
      eval.torusCount -= query2Torus[queryIndex];
    }
  }

  // node関連の評価値を削除または追加
  void UpdateQueryEval(const Node& node, bool isInc) {
    for (const auto queryIndex : node2QueryIndexes[node]) {
      UpdateQueryEval(queryIndex, isInc);
    }
  }

  void UpdateBestQuery(const Node& node) {
    for (const auto queryIndex : node2QueryIndexes[node]) {
      UpdateBestQuery(queryIndex);
    }
  }

  void PushBack(const Node& node) {
    UpdateQueryEval(node, false);
    eval.AddNode(node);
    nodes.PushBack(node);
    UpdateBestQuery(node);
    UpdateQueryEval(node, true);

  }

  void PushFront(const Node& node) {
    UpdateQueryEval(node, false);
    eval.AddNode(node);
    nodes.PushFront(node);
    UpdateBestQuery(node);
    UpdateQueryEval(node, true);
  }

  Node PopBack() {
    const auto node = nodes.Back();
    UpdateQueryEval(node, false);
    nodes.PopBack();
    eval.RemoveNode(node);
    UpdateBestQuery(node);
    UpdateQueryEval(node, true);

    return node;
  }

  Node PopFront() {
    const auto node = nodes.Front();
    UpdateQueryEval(node, false);
    nodes.PopFront();
    eval.RemoveNode(node);
    UpdateBestQuery(node);
    UpdateQueryEval(node, true);

    return node;
  }

  Node Front() const { return nodes.Front(); }
  Node Back() const { return nodes.Back(); }
  void Relocation() {
    eval.queryDistSum = 0;
    eval.torusCount = 0;
    // rep(q, T) {
    //   UpdateQueryEval(q, false);
    // }
    nodes.Relocation();
    rep(q,  T){
      UpdateBestQuery(q);
      UpdateQueryEval(q, true);
    }
  }

  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream& operator<<(ostream& os, const Solution& sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  void Update(Solution*) const {}
  virtual void Restore(Solution*) const = 0;
  double diff;
};

// ランダムウォークで追加
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution* const sol) {
    diff = -sol->Eval();
    const auto from = sol->Back();
    const auto to = sol->Front();
    vector<Node> addNodes;
    Node current = from;
    bool success = false;
    rep(t, 5) {
      const auto& nextNodes = G.NextNodes(current);
      const auto next = rand_vec(nextNodes);
      addNodes.push_back(next);
      current = next;
      if (G.HasEdge(current, to)) {
        success = true;
        break;
      }
    }
    if (success) {
      for (const auto node : addNodes) {
        sol->PushBack(node);
        pushCount++;
      }
      diff += sol->Eval();
      return;
    }

    // 失敗
    diff = INF;
  }
  void Restore(Solution* sol) const override {
    rep(i, pushCount) { sol->PopBack(); }
  }
  int pushCount = 0;
};

// 削除
struct NBD2 : public NBD {
  NBD2() {}
  NBD2(Solution* const sol) {
    if (sol->nodes.Size() <= 10) {
      diff = INF;
      return;
    }
    diff = -sol->Eval();
    bool success = false;
    rep(i, 5) {
      const auto node = sol->PopBack();
      poppedNodes.push_back(node);

      if (G.HasEdge(sol->Back(), sol->Front())) {
        success = true;
        break;
      }
    }

    if (success) {
      diff += sol->Eval();
      return;
    }

    diff = INF;
  }
  void Restore(Solution* sol) const override {
    REP(i, poppedNodes.size()) { sol->PushBack(poppedNodes[i]); }
  }
  vector<Node> poppedNodes;
};

struct NBDGenerator {
  NBD* Generate(Solution* const sol, const int g, const double progress) {
    // Rotate
    {
      const auto node = sol->PopFront();
      sol->PushBack(node);
    }
    if (g % 10000 == 0) {
      sol->Relocation();
    }
    const int random = rand(0, 100);
    if (random < 50) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else if (random < 100) {
      nbd2_ = NBD2(sol);
      return &nbd2_;
    } else {
      assert(false);
    }
  }

 private:
  NBD1 nbd1_;
  NBD2 nbd2_;
};

vector<Node> Run(int timeLimit) {
  Solution first_sol = Solution::CreateInitialSol();
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, 1.0, 0.000001);
  auto final_sol = SA.run(first_sol, gen);
  cerr << "score: " << final_sol.Score() << endl;
  cerr << "dist: " << final_sol.eval.queryDistSum << endl;
  cerr << "exist: " << final_sol.eval.existNodeCount << endl;
  final_sol.Relocation();
  cerr << "score: " << final_sol.Score() << endl;
  cerr << "dist: " << final_sol.eval.queryDistSum << endl;
  cerr << "exist: " << final_sol.eval.existNodeCount << endl;
  const auto ret = final_sol.nodes.ToVector();
  for (const auto node : ret) {
    cerr << node.id << " ";
  }
  cerr << endl;
  return ret;
}

}  // namespace NBD2


/* start */







/* start */





struct QueryNode {
  int idx;
  QueryNode() {}
  QueryNode(int queryId, bool isFrom) : idx(queryId * 2 + (isFrom ? 0 : 1)) {}
  Node GetNode() const {
    if (idx % 2 == 0) {
      return inputted_queries[idx / 2].first;
    } else {
      return inputted_queries[idx / 2].second;
    }
  }
  operator int() const { return ToIdx(); }
  int ToIdx() const { return idx; }
  int VisitOrder() const {
    // 0からTまで
    return (idx + 1) / 2;
  }
  static QueryNode CreateFromVisitOrder(int order) {
    if (order == T) {
      return QueryNode(T - 1, false);
    }
    return QueryNode(order, true);
  }
};

namespace NBD3{

/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

const int removeCount = 0;
struct Evaluation {
  int allDistSum = 0;
  int queryDistSum = 0;
  // 最大化問題の場合は符号を反転すること
  double Sum() const {
    return allDistSum;
  }
};

struct Solution {
  vector<QueryNode> queryNodes;
  vector<int> queryNode2Idxes;
  Evaluation eval;

  Solution() {
    queryNode2Idxes.resize(T * 2);
    rep(i, N) {
      queryNode2Idxes[QueryNode(i, true).ToIdx()] = queryNodes.size();
      queryNodes.emplace_back(i, true);
      queryNode2Idxes[QueryNode(i, false).ToIdx()] = queryNodes.size();
      queryNodes.emplace_back(i, false);
    }
    eval = InitEval();
  }
  Evaluation InitEval() const{
    Evaluation eval;
    rep(i, (int)queryNodes.size()) {
      const auto node = queryNodes[i].GetNode();
      const auto nextNode = queryNodes[(i+1)%queryNodes.size()].GetNode();
      eval.allDistSum += Dist(node, nextNode);
    }
    return eval;
  }
  static Solution CreateInitialSol() {
    Solution sol;
    return sol;
  }

  Node GetNode(int idx) const {
    return queryNodes[(idx + (int)queryNodes.size()) % queryNodes.size()].GetNode();
  }

  // 2-opt
  // order[l:r]を逆順にした場合の評価値を返す
  Evaluation EvalTwoOpt(int l, int r) const {
    auto ret = eval;
    if (l == r) return ret;
    if (l > r) swap(l, r);
    ret.allDistSum -= Dist(GetNode(l - 1), GetNode(l));
    ret.allDistSum -= Dist(GetNode(r - 1), GetNode(r));
    ret.allDistSum += Dist(GetNode(l - 1), GetNode(r - 1));
    ret.allDistSum += Dist(GetNode(r), GetNode(l));
    return ret;
  }

  void AcceptTwoOpt(int l, int r) {
    if (l == r) return;
    if (l > r) swap(l, r);
    eval = EvalTwoOpt(l, r);
    reverse(queryNodes.begin() + l, queryNodes.begin() + r);
    for(int i = l; i < r; i++){
      queryNode2Idxes[queryNodes[i].ToIdx()] = i;
    }
  }

  // 1.5-opt
  // fromを除外して、toの前に挿入する
  // toは、fromを削除した後のindex
  Evaluation EvalPointMove(int from, int to) const {
    auto ret = eval;
    if (from == to) return ret;
    if(from == (int)queryNodes.size() - 1 && to == 0) return ret;
    ret.allDistSum -= Dist(GetNode(from - 1), GetNode(from));
    ret.allDistSum -= Dist(GetNode(from), GetNode(from + 1));
    ret.allDistSum += Dist(GetNode(from - 1), GetNode(from + 1));
    if(to < from){
      ret.allDistSum += Dist(GetNode(to - 1), GetNode(from));
      ret.allDistSum += Dist(GetNode(from), GetNode(to));
      ret.allDistSum -= Dist(GetNode(to - 1), GetNode(to));
    }else{
      // 現在のtoの後ろに挿入される
      ret.allDistSum += Dist(GetNode(to), GetNode(from));
      ret.allDistSum += Dist(GetNode(from), GetNode(to + 1));
      ret.allDistSum -= Dist(GetNode(to), GetNode(to + 1));
    }
    return ret;
  }

  void AcceptPointMove(int from, int to) {
    if (from == to) return;
    if(from == (int)queryNodes.size() - 1 && to == 0) return;
    eval = EvalPointMove(from, to);
    auto node = queryNodes[from];
    queryNodes.erase(queryNodes.begin() + from);
    queryNodes.insert(queryNodes.begin() + to, node);
    if(from < to){
      for(int i = from; i <= to; i++){
        queryNode2Idxes[queryNodes[i].ToIdx()] = i;
      }
    }else{
      for(int i = to; i <= from; i++){
        queryNode2Idxes[queryNodes[i].ToIdx()] = i;
      }
    }
  }

  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream& operator<<(ostream& os, const Solution& sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  virtual void Update(Solution*) const = 0;
  virtual void Restore(Solution*) const = 0;
  double diff;
};

// 2-opt
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution* const sol) {
    diff = -sol->Eval();
    l = rand(0, sol->queryNodes.size());
    r = rand_other_than(0, sol->queryNodes.size(), l);
    auto eval = sol->EvalTwoOpt(l, r);
    diff += eval.Sum();
  }
  void Update(Solution* sol) const override { sol->AcceptTwoOpt(l, r); }
  void Restore(Solution* sol) const override {}
  int l, r;
};

// 1.5-opt
struct NBD2 : public NBD {
  NBD2() {}
  NBD2(Solution* const sol) {
    diff = -sol->Eval();
    from = rand(0, sol->queryNodes.size());
    to = rand_other_than(0, (int)sol->queryNodes.size() - 1, from);
    auto eval = sol->EvalPointMove(from, to);
    diff += eval.Sum();
  }
  void Update(Solution* sol) const override { sol->AcceptPointMove(from, to); }
  void Restore(Solution* sol) const override {}
  int from, to;
};


struct NBDGenerator {
  NBD* Generate(Solution* const sol, const int g, const double progress) {
    if (sol->Score() > best_sol.Score()) {
      best_sol = *sol;
    }
    const int random = rand(0, 100);
    if (random < 50) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    }else if(random < 100){
      nbd2_ = NBD2(sol);
      return &nbd2_;
    } else {
      assert(false);
    }
  }
  const Solution& GetBestSol() const { return best_sol; }

 private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  Solution best_sol;
};

vector<Node> Run(int timeLimit, double BeginTemperature, double EndTemperature) {
  Solution first_sol = Solution::CreateInitialSol();
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, BeginTemperature, EndTemperature);
  const auto final_sol = SA.run(first_sol, gen);
  const auto best_sol = gen.GetBestSol();
  cerr << "score: " << best_sol.Score() << endl;
  vector<Node> ret;
  rep(i, (int)best_sol.queryNodes.size()) {
    ret.push_back(best_sol.GetNode(i));
  }
  return ret;
}

} // namespace NBD3


/* start */







namespace NBD3_5 {

/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

const int removeCount = 0;
struct Evaluation {
  int allDistSum = 0;
  // 最大化問題の場合は符号を反転すること
  double Sum() const { return allDistSum; }
};

struct Solution {
  vector<Node> nodes;
  Evaluation eval;

  Solution() { eval.allDistSum = INF; }
  explicit Solution(vector<Node> targetNodes) : nodes(targetNodes) {
    eval = InitEval();
  }
  Evaluation InitEval() const {
    Evaluation eval;
    rep(i, (int)nodes.size()) {
      const auto node = nodes[i];
      const auto nextNode = GetNode(i + 1);
      eval.allDistSum += Dist(node, nextNode);
    }
    return eval;
  }

  Node GetNode(int idx) const { return nodes[Mod(idx, nodes.size())]; }

  // 2-opt
  // order[l:r]を逆順にした場合の評価値を返す
  Evaluation EvalTwoOpt(int l, int r) const {
    auto ret = eval;
    if (l == r) return ret;
    if (l > r) swap(l, r);
    ret.allDistSum -= Dist(GetNode(l - 1), GetNode(l));
    ret.allDistSum -= Dist(GetNode(r - 1), GetNode(r));
    ret.allDistSum += Dist(GetNode(l - 1), GetNode(r - 1));
    ret.allDistSum += Dist(GetNode(r), GetNode(l));
    return ret;
  }

  void AcceptTwoOpt(int l, int r) {
    if (l == r) return;
    if (l > r) swap(l, r);
    eval = EvalTwoOpt(l, r);
    reverse(nodes.begin() + l, nodes.begin() + r);
  }

  // 1.5-opt
  // fromを除外して、toの前に挿入する
  // toは、fromを削除した後のindex
  Evaluation EvalPointMove(int from, int to) const {
    auto ret = eval;
    if (from == to) return ret;
    if (from == (int)nodes.size() - 1 && to == 0) return ret;
    ret.allDistSum -= Dist(GetNode(from - 1), GetNode(from));
    ret.allDistSum -= Dist(GetNode(from), GetNode(from + 1));
    ret.allDistSum += Dist(GetNode(from - 1), GetNode(from + 1));
    if (to < from) {
      ret.allDistSum += Dist(GetNode(to - 1), GetNode(from));
      ret.allDistSum += Dist(GetNode(from), GetNode(to));
      ret.allDistSum -= Dist(GetNode(to - 1), GetNode(to));
    } else {
      // 現在のtoの後ろに挿入される
      ret.allDistSum += Dist(GetNode(to), GetNode(from));
      ret.allDistSum += Dist(GetNode(from), GetNode(to + 1));
      ret.allDistSum -= Dist(GetNode(to), GetNode(to + 1));
    }
    return ret;
  }

  void AcceptPointMove(int from, int to) {
    if (from == to) return;
    if (from == (int)nodes.size() - 1 && to == 0) return;
    eval = EvalPointMove(from, to);
    auto node = nodes[from];
    nodes.erase(nodes.begin() + from);
    nodes.insert(nodes.begin() + to, node);
  }

  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream& operator<<(ostream& os, const Solution& sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  virtual void Update(Solution*) const = 0;
  virtual void Restore(Solution*) const = 0;
  double diff;
};

// 2-opt
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution* const sol) {
    diff = -sol->Eval();
    l = rand(0, sol->nodes.size());
    r = rand_other_than(0, sol->nodes.size(), l);
    auto eval = sol->EvalTwoOpt(l, r);
    diff += eval.Sum();
  }
  void Update(Solution* sol) const override { sol->AcceptTwoOpt(l, r); }
  void Restore(Solution* sol) const override {}
  int l, r;
};

// 1.5-opt
struct NBD2 : public NBD {
  NBD2() {}
  NBD2(Solution* const sol) {
    diff = -sol->Eval();
    from = rand(0, sol->nodes.size());
    to = rand_other_than(0, (int)sol->nodes.size() - 1, from);
    auto eval = sol->EvalPointMove(from, to);
    diff += eval.Sum();
  }
  void Update(Solution* sol) const override {
    sol->AcceptPointMove(from, to);
  }
  void Restore(Solution* sol) const override {}
  int from, to;
};

struct NBDGenerator {
  NBD* Generate(Solution* const sol, const int g, const double progress) {
    if (sol->Score() > best_sol.Score()) {
      best_sol = *sol;
    }
    const int random = rand(0, 100);
    if (random < 50) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else if (random < 100) {
      nbd2_ = NBD2(sol);
      return &nbd2_;
    } else {
      assert(false);
    }
  }
  const Solution& GetBestSol() const { return best_sol; }

 private:
  NBD1 nbd1_;
  NBD2 nbd2_;
  Solution best_sol;
};

vector<Node> Run(int timeLimit, double BeginTemperature, double EndTemperature,
                 vector<Node> targetNodes) {
  Solution first_sol(targetNodes);
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, BeginTemperature,
                                                EndTemperature);
  const auto final_sol = SA.run(first_sol, gen);
  const auto best_sol = gen.GetBestSol();
  cerr << "score: " << best_sol.Score() << endl;
  vector<Node> ret;
  rep(i, (int)best_sol.nodes.size()) { ret.push_back(best_sol.GetNode(i)); }
  return ret;
}

}  // namespace NBD3_5


/* start */






namespace NBD5{

/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

const int removeCount = 0;
struct Evaluation {
  int allDistSum = 0;
  // 最大化問題の場合は符号を反転すること
  double Sum() const {
    return allDistSum;
  }
};

struct Solution {
  vector<Node> nodes;
  Evaluation eval;

  Solution(){
    eval.allDistSum = INF;
  }
  Solution(vector<Node> nodes): nodes(nodes) {
    eval = InitEval();
  }
  Evaluation InitEval() const{
    Evaluation eval;
    rep(i, (int)nodes.size()) {
      const auto node = nodes[i];
      const auto nextNode = nodes[(i+1)%nodes.size()];
      eval.allDistSum += Dist(node, nextNode);
    }
    return eval;
  }

  Node GetNode(int idx) const {
    return nodes[(idx + (int)nodes.size()) % nodes.size()];
  }

  // 2-opt
  // order[l:r]を逆順にした場合の評価値を返す
  Evaluation EvalTwoOpt(int l, int r) const {
    auto ret = eval;
    if (l == r) return ret;
    if (l > r) swap(l, r);
    ret.allDistSum -= Dist(GetNode(l - 1), GetNode(l));
    ret.allDistSum -= Dist(GetNode(r - 1), GetNode(r));
    ret.allDistSum += Dist(GetNode(l - 1), GetNode(r - 1));
    ret.allDistSum += Dist(GetNode(r), GetNode(l));
    return ret;
  }

  void AcceptTwoOpt(int l, int r) {
    if (l == r) return;
    if (l > r) swap(l, r);
    eval = EvalTwoOpt(l, r);
    reverse(nodes.begin() + l, nodes.begin() + r);
  }

  double Eval() const { return eval.Sum(); }
  double Score() const { return -Eval(); }
  friend ostream& operator<<(ostream& os, const Solution& sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  virtual void Update(Solution*) const = 0;
  virtual void Restore(Solution*) const = 0;
  double diff;
};

struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution* const sol) {
    diff = -sol->Eval();
    l = rand(0, sol->nodes.size());
    r = rand_other_than(0, sol->nodes.size(), l);
    auto eval = sol->EvalTwoOpt(l, r);
    diff += eval.Sum();
  }
  void Update(Solution* sol) const override { sol->AcceptTwoOpt(l, r); }
  void Restore(Solution* sol) const override {}
  int l, r;
};


struct NBDGenerator {
  NBD* Generate(Solution* const sol, const int g, const double progress) {
    if (sol->Score() > best_sol.Score()) {
      best_sol = *sol;
    }
    const int random = rand(0, 100);
    if (random < 100) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else {
      assert(false);
    }
  }
  const Solution& GetBestSol() const { return best_sol; }

 private:
  NBD1 nbd1_;
  Solution best_sol;
};

vector<Node> Run(int timeLimit, Solution first_sol){
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, 3.0, 0.1);
  const auto final_sol = SA.run(first_sol, gen);
  const auto best_sol = gen.GetBestSol();
  cerr << "score: " << best_sol.Score() << endl;
  vector<Node> ret;
  rep(i, (int)best_sol.nodes.size()) {
    ret.push_back(best_sol.GetNode(i));
  }
  return ret;
}

vector<Node> Run(int timeLimit, const vector<Node>& initNodes) {
  Solution first_sol(initNodes);
  return Run(timeLimit, first_sol);
}

} // namespace NBD5









/* start */

unordered_map<uint64_t, vector<Node>> hash2PrimResults;

vector<Node> Prim(vector<Node> nodes, vector<Node> baseNodes = {},
                  bool isOld = true) {
  // baseNodesからスタートして、全てのノードを繋ぐ最小の経路を求める
  // 追加した分だけ返す
  vector<Node> allNodes;
  vector<Node> addedNodes;
  vector<bool> isAdded(nodes.size(), false);
  if (isOld && baseNodes.empty()) {
    // 古い実装との整合性のために存在
    allNodes.push_back(nodes[0]);
    addedNodes.push_back(nodes[0]);
    isAdded.erase(isAdded.begin());
    nodes.erase(nodes.begin());
  } else {
    assert(!baseNodes.empty());
    allNodes.insert(allNodes.end(), all(baseNodes));
  }

  uint64_t hash = 0;
  if (!isOld) {
    for (const auto node : nodes) {
      hash += node2RandomHashes[node];
    }
    hash ^= 536021785489048919ull;
    for (const auto node : allNodes) {
      hash += node2RandomHashes[node];
    }

    if (hash2PrimResults.count(hash)) {
      return hash2PrimResults[hash];
    }
  }

  // nodesやbaseNodesの順番に関わらず、結果を一意にするため、ソートする
  if (!isOld) {
    sort(all(nodes));
    sort(all(allNodes));
  }

  // プリム法でシュタイナー木を構築
  rep(_t, nodes.size()) {
    int minIdx = -1;
    Node minBaseNode = Node(-1);
    int minDist = INF;
    for (const auto node : allNodes) {
      rep(i, (int)nodes.size()) {
        if (isAdded[i]) {
          continue;
        }
        const auto dist = Dist(node, nodes[i]);
        if (chmin(minDist, dist)) {
          minIdx = i;
          minBaseNode = node;
        }
      }
    }

    assert(minIdx != -1);
    auto path = CalcMinPath(minBaseNode, nodes[minIdx]);
    allNodes.insert(allNodes.end(), all(path));
    addedNodes.insert(addedNodes.end(), all(path));
    isAdded[minIdx] = true;
  }

  hash2PrimResults[hash] = addedNodes;

  return addedNodes;
}

unordered_map<uint64_t, pair<vector<Node>, int>> hash2NewPrimResults;

// 失敗した場合は、追加できなかった個数を返す
pair<vector<Node>, int> NewPrim(vector<Node> nodes, vector<Node> baseNodes) {
  // baseNodesからスタートして、全てのノードを繋ぐ最小の経路を求める
  // 追加した分だけ返す
  vector<Node> allNodes;
  vector<Node> addedNodes;
  vector<bool> isAdded(nodes.size(), false);
  assert(!baseNodes.empty());
  allNodes.insert(allNodes.end(), all(baseNodes));

  uint64_t hash = 0;
  for (const auto node : nodes) {
    hash += node2RandomHashes[node];
  }
  hash ^= 536021785489048919ull;
  for (const auto node : allNodes) {
    hash += node2RandomHashes[node];
  }

  if (hash2NewPrimResults.count(hash)) {
    return hash2NewPrimResults[hash];
  }

  // nodesやbaseNodesの順番に関わらず、結果を一意にするため、ソートする
  // 高速化のため、中心から近い順とする
  sort(all(nodes), [&](const Node& a, const Node& b) {
    if (Dist(centerNode, a) == Dist(centerNode, b)) {
      return a < b;
    }
    return Dist(centerNode, a) < Dist(centerNode, b);
  });
  sort(all(allNodes));

  // プリム法でシュタイナー木を構築
  rep(idx, nodes.size()) {
    Node minBaseNode = Node(-1);
    int minDist = INF;
    for (const auto node : allNodes) {
      const auto dist = Dist(node, nodes[idx]);
      if (chmin(minDist, dist)) {
        minBaseNode = node;
      }
    }

    // 打ち切り
    if (minDist + (int)addedNodes.size() > LB) {
      hash2NewPrimResults[hash] = {vector<Node>(), (int)nodes.size() - idx};
      return hash2NewPrimResults[hash];
    }

    auto path = CalcMinPath(minBaseNode, nodes[idx]);
    allNodes.insert(allNodes.end(), all(path));
    addedNodes.insert(addedNodes.end(), all(path));
    isAdded[idx] = true;
  }

  hash2NewPrimResults[hash] = {addedNodes, 0};
  assert((int)addedNodes.size() <= LB);

  return hash2NewPrimResults[hash];
}

unordered_map<uint64_t, pair<vector<Node>, int>> hash2CompletePrimResults;

// 完遂する
pair<vector<Node>, int> CompletePrim(vector<Node> nodes, vector<Node> baseNodes) {
  // baseNodesからスタートして、全てのノードを繋ぐ最小の経路を求める
  // 追加した分だけ返す
  vector<Node> allNodes;
  vector<Node> addedNodes;
  vector<bool> isAdded(nodes.size(), false);
  assert(!baseNodes.empty());
  allNodes.insert(allNodes.end(), all(baseNodes));

  uint64_t hash = 0;
  for (const auto node : nodes) {
    hash += node2RandomHashes[node];
  }
  hash ^= 536021785489048919ull;
  for (const auto node : allNodes) {
    hash += node2RandomHashes[node];
  }

  if (hash2NewPrimResults.count(hash)) {
    return hash2NewPrimResults[hash];
  }

  // nodesやbaseNodesの順番に関わらず、結果を一意にするため、ソートする
  // 高速化のため、中心から近い順とする
  sort(all(nodes), [&](const Node& a, const Node& b) {
    if (Dist(centerNode, a) == Dist(centerNode, b)) {
      return a < b;
    }
    return Dist(centerNode, a) < Dist(centerNode, b);
  });
  sort(all(allNodes));

  // プリム法でシュタイナー木を構築
  rep(idx, nodes.size()) {
    Node minBaseNode = Node(-1);
    int minDist = INF;
    for (const auto node : allNodes) {
      const auto dist = Dist(node, nodes[idx]);
      if (chmin(minDist, dist)) {
        minBaseNode = node;
      }
    }

    auto path = CalcMinPath(minBaseNode, nodes[idx]);
    allNodes.insert(allNodes.end(), all(path));
    addedNodes.insert(addedNodes.end(), all(path));
    isAdded[idx] = true;
  }

  hash2NewPrimResults[hash] = {addedNodes, 0};

  return hash2NewPrimResults[hash];
}

/* start */

struct Branch {
  vector<Node> orders;
  vector<Node> allNodes;
  unordered_map<Node, int> dists;

  Branch(const vector<Node> orders) : orders(orders) {
    // 貪欲にシュタイナー木を構築
    allNodes = Prim(orders);
  }

  Branch CloneAdd(Node order) const {
    auto newOrders = orders;
    newOrders.push_back(order);
    return Branch(newOrders);
  }

  int MinDist(const Node order) const {
    int minDist = INF;
    for (const auto baseNode : allNodes) {
      chmin(minDist, Dist(baseNode, order));
    }
    return minDist;
  }

  int Size() const { return (int)allNodes.size(); }

  const vector<Node>& GetOrders() const { return orders; }

  const vector<Node>& GetAllNodes() const { return allNodes; }

  vector<Node> CreateAllNodesFirstIs(Node firstNode) const{
    assert((int)dists.size() > 0);
    vector<Node> allNodesFirstIs;
    unordered_set<Node> addedNodes;
    // まず、firstNodeまでの経路を逆順で格納
    assert(orders[0] == centerNode);
    const auto baseNode = orders[0];
    Node currentNode = firstNode;
    while (currentNode != baseNode) {
      allNodesFirstIs.push_back(currentNode);
      addedNodes.insert(currentNode);
      const auto currentDist = dists.at(currentNode);
      for(const auto prevNode : G.NextNodes(currentNode)){
        if(dists.count(prevNode) == 0){
          continue;
        }
        if (dists.at(prevNode) == currentDist - 1) {
          currentNode = prevNode;
          break;
        }
      }
    }
    allNodesFirstIs.push_back(baseNode);
    addedNodes.insert(baseNode);
    reverse(all(allNodesFirstIs));

    // 残りを適当に格納
    for (const auto node : allNodes) {
      if (addedNodes.find(node) == addedNodes.end()) {
        allNodesFirstIs.push_back(node);
      }
    }

    return allNodesFirstIs;
  }

  void InitDists() {
    for (const auto node : allNodes) {
      dists[node] = INF;
    }
    dists[orders[0]] = 0;
    unordered_set<Node> usedNodes(all(allNodes));
    queue<Node> que;
    que.push(orders[0]);
    while (!que.empty()) {
      const auto node = que.front();
      que.pop();
      for (const auto& to : G.NextNodes(node)) {
        if (usedNodes.find(to) == usedNodes.end()) {
          continue;
        }
        if (chmin(dists[to], dists[node] + 1)) {
          que.push(to);
        }
      }
    }
  }

  int GetDistFromBase(Node node) const {
    assert((int)dists.size() > 0);
    return dists.at(node);
  }
};



/* start */











/* start */

// 差分ダイクストラ
struct Dijkstra {
  Node baseNode;
  vector<ll> allDists;
  vector<Node> allPrevNodes;
  vector<int> allDepthes;
  IndexSet<Node> shouldUpdateNodes;

  Dijkstra(Node baseNode, const WeightedGraph<Node>& graph) : baseNode(baseNode), allDists(N, LINF), allPrevNodes(N, Node(-1)), allDepthes(N, INF), shouldUpdateNodes(N) {
    allDists[baseNode] = 0;
    allDepthes[baseNode] = 0;
    rep(i, N){
      const Node node(i);
      for(const auto& edge : graph.Edges(node)){
        AddEdge(node, edge.to, edge.w);
      }
    }

    UpdateDists(graph);
  }

  ll Dist(Node node) const {
    return allDists[node];
  }

  void AddEdge(Node node0, Node node1, int dist) {
    const auto oldEdgeDist = abs(allDists[node0] - allDists[node1]);

    // 既存の辺よりも新しい辺の方が長い場合は何もしない
    if (oldEdgeDist <= dist) {
      return;
    }

    // 新しい辺の方が短い場合は、深いほうにフラグを立てる
    if (allDepthes[node0] < allDepthes[node1]) {
      shouldUpdateNodes.addIfNoExist(node1);
    } else {
      shouldUpdateNodes.addIfNoExist(node0);
    }
  }

  // void RemoveEdge(Node node0, Node node1, int dist) {
  //   const auto oldEdgeDist = abs(allDists[node0] - allDists[node1]);

  //   // 既存の辺が使われてない場合は何もしない
  //   if (oldEdgeDist < dist || abs(allDists[node0] - allDists[node1]) != 1) {
  //     return;
  //   }

  //   // 既存の辺が使われている場合は、深いほうにフラグを立て、深いほうの距離をINFにする
  //   if (allDepthes[node0] < allDepthes[node1]) {
  //     shouldUpdateNodes.addIfNoExist(node1);
  //     allDists[node1] = INF;
  //   } else {
  //     shouldUpdateNodes.addIfNoExist(node0);
  //     allDists[node0] = INF;
  //   }
  // }

  void UpdateDists(const WeightedGraph<Node>& graph){
    // フラグが立っているノードを、前ノードからの距離で更新しつつpqに突っ込む
    auto shouldUpdateVec = shouldUpdateNodes.ToVec();
    priority_queue<pair<ll, Node>, vector<pair<ll, Node>>, greater<pair<ll, Node>>> pq;
    for (const auto node : shouldUpdateVec) {
      shouldUpdateNodes.remove(node);
      ll minDist = LINF;
      Node minPrevNode = Node(-1);
      for (const auto edge : graph.RevEdges(node)) {
        const auto prevNode = edge.to;
        const auto dist = edge.w;
        if (allDists[prevNode] + dist < minDist) {
          minDist = allDists[prevNode] + dist;
          minPrevNode = prevNode;
        }
      }
      if (minDist < allDists[node]) {
        pq.push({minDist, node});
        allDists[node] = minDist;
        allPrevNodes[node] = minPrevNode;
        allDepthes[node] = allDepthes[minPrevNode] + 1;
      }
    }

    // ダイクストラで更新
    while (!pq.empty()) {
      const auto [dist, node] = pq.top();
      pq.pop();
      if (allDists[node] < dist) {
        continue;
      }
      for (const auto edge : graph.Edges(node)) {
        const auto nextNode = edge.to;
        const auto nextDist = dist + edge.w;
        if (nextDist < allDists[nextNode]) {
          pq.push({nextDist, nextNode});
          allDists[nextNode] = nextDist;
          allPrevNodes[nextNode] = node;
          allDepthes[nextNode] = allDepthes[node] + 1;
        }
      }
    }
  }
};

struct AllDijkstra{
  vector<Dijkstra> dijkstras;
  vector<Node> baseNodes;
  vector<int> baseNode2Idxes;

  AllDijkstra(const WeightedGraph<Node>& graph, unordered_set<Node> _baseNodes):baseNodes(all(_baseNodes)), baseNode2Idxes(N, -1){
    rep(i, (int)baseNodes.size()){
      const auto baseNode = baseNodes[i];
      dijkstras.emplace_back(baseNode, graph);
      baseNode2Idxes[baseNode] = i;
    }
  }

  void AddEdge(Node node0, Node node1, int dist){
    for(auto& dijkstra : dijkstras){
      dijkstra.AddEdge(node0, node1, dist);
    }
  }

  // void RemoveEdge(Node node0, Node node1, int dist){
  //   for(auto& dijkstra : dijkstras){
  //     dijkstra.RemoveEdge(node0, node1, dist);
  //   }
  // }

  void UpdateDists(const WeightedGraph<Node>& graph){
    for(auto& dijkstra : dijkstras){
      dijkstra.UpdateDists(graph);
    }
  }

  ll Dist(Node node0, Node node1) const{
    const int idx = baseNode2Idxes[node0];
    return dijkstras[idx].Dist(node1);
  }
};
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






/* start */

// Aを受け取って最適解を求める
struct OptA {
  // quick_exit(0)でメモリ解放をさぼるために、グローバルに置いている
  static vector<vector<tuple<int, Node, int>>> dp;
  static vector<vector<vector<Node>>> from2ai2MovableNodes;
  static vector<Action> Solve(const AObj& A) {
    // dp[t][node] :=
    // ノードnodeにいて、t時間経過したときの、到達済みオーダー数の最大値・直前のノード
    dp = vector<vector<tuple<int, Node, int>>>(
        1, vector<tuple<int, Node, int>>(
               N, tuple<int, Node, int>{-1, Node(-1), -1}));
    dp[0][Node(0)] = {0, Node(-1), -1};
    dp.reserve(T * 100);

    cerr << global_timer.ms() << "[ms]" << endl;

    // 各nodeに対して有効なaiを事前に列挙しておく
    vector<vector<int>> node2ValidAi(N);
    rep(nodeId, N) {
      const Node node(nodeId);
      for (int ai = 0; ai + LB <= LA; ai++) {
        BObj B(A, ai);
        if (B.IsAdj(node)) {
          node2ValidAi[nodeId].push_back(ai);
        }
      }
    }

    cerr << global_timer.ms() << "[ms]" << endl;

    // 事前計算しておく
    // 目的地に移動した後別の場所に移動する用に、aiごとに分けている
    from2ai2MovableNodes.resize(N);
    // 目的地に到達しない場合には、移動して終わりなので、aiごとに分けなくてよい
    vector<unordered_map<Node, int>> from2To2Ais(N);
    rep(fromId, N) {
      const Node from(fromId);
      const auto adjs = G.NextNodes(from);
      unordered_set<Node> adjsSet(all(adjs));
      int adjCount = 0;
      for (int ai = 0; ai + LB <= LA; ai++) {
        if (ai == 0) {
          // 隣接数を初期化
          for (int bi = 0; bi < LB; bi++) {
            adjCount += adjsSet.count(A.A[bi]);
          }
        }

        // 隣接がないなら行わない
        if (adjCount > 0) {
          BObj B(A, ai);
          const auto movableNodes = B.MovableNodes(from);
          from2ai2MovableNodes[fromId].push_back(movableNodes);
          for (const auto to : movableNodes) {
            from2To2Ais[fromId][to] = ai;
          }
        }else{
          from2ai2MovableNodes[fromId].push_back(vector<Node>());
        }

        // 隣接数を更新
        adjCount -= adjsSet.count(A.A[ai]);
        if (ai + LB < LA) {
          adjCount += adjsSet.count(A.A[ai + LB]);
        }
      }
    }

    cerr << global_timer.ms() << "[ms]" << endl;

    // TODO: 到達できない場合に早めにエラーを出す？
    int bestNextOrder = 0;
    bitset<600> checked = 0;
    rep(t, T * 100) {
      // cerr << t << " " << bestNextOrder << endl;
      dp.emplace_back(dp.back());
      int nextBestNextOrder = bestNextOrder;
      rep(nodeId, N) {
        if (get<0>(dp[t][nodeId]) < bestNextOrder) continue;
        if (checked[nodeId]) continue;
        checked[nodeId] = 1;
        const Node baseNode(nodeId);
        // 目的地への移動手段がないなら、Aiを区別する必要はない
        {
          const int nextOrder = get<0>(dp[t][baseNode]);
          if (from2To2Ais[baseNode].count(inputted_orders[nextOrder]) == 0) {
            const auto toes = from2To2Ais[baseNode];
            for (const auto [to, ai] : toes) {
              if (get<0>(dp[t + 1][to.id]) < nextOrder) {
                dp[t + 1][to.id] = {nextOrder, baseNode, ai};
              }
            }
            continue;
          }
        }

        // 全てのBを試す
        const int bi = 0;
        const int len = LB;
        for (const auto ai : node2ValidAi[nodeId]) {
          BObj B(A, ai);
          auto currentNode = baseNode;
          int nextOrder = get<0>(dp[t][currentNode.id]);
          while (true) {
            if (nextOrder == T) {
              // 全てのオーダーを完了したので復元して終了
              vector<Action> ret;
              int ttt = t + 1;
              assert(get<0>(dp[ttt][currentNode.id]) == T);
              while (ttt > 0) {
                const auto [nextOrder, prevNode, prevAi] =
                    dp[ttt][currentNode.id];
                const auto action =
                    Action(len, prevAi, bi, currentNode, nextOrder);
                ret.push_back(action);
                ttt--;
                currentNode = prevNode;
              }
              reverse(all(ret));
              return ret;
            }
            bool updated = false;
            const auto movableNodes = from2ai2MovableNodes[currentNode.id][ai];
            for (const auto to : movableNodes) {
              if (to == inputted_orders[nextOrder]) {
                // オーダーを完了した
                // 直前のノードは、連鎖に関わらずbaseNode
                if (get<0>(dp[t + 1][to.id]) < nextOrder + 1) {
                  dp[t + 1][to.id] = {nextOrder + 1, baseNode, ai};
                }
                chmax(nextBestNextOrder, nextOrder + 1);
                // その位置からの連鎖的移動を考える
                updated = true;
                currentNode = to;
                nextOrder++;
                // オーダーが完了できるのにオーダーを完了しないパターンは考える必要がないので、breakする
                break;
              } else {
                // 移動した
                // 直前のノードは、連鎖に関わらずbaseNode
                if (get<0>(dp[t + 1][to.id]) < nextOrder) {
                  dp[t + 1][to.id] = {nextOrder, baseNode, ai};
                }
              }
            }
            if (!updated) break;
          }
        }
      }
      if (bestNextOrder != nextBestNextOrder) {
        checked = 0;
        bestNextOrder = nextBestNextOrder;
      }
    }

    set<Node> ASet(all(A.A));
    cerr << "ASet.size() = " << ASet.size() << endl;
    cerr << "N = " << N << endl;
    assert(false);

    return {};
  }
};

vector<vector<tuple<int, Node, int>>> OptA::dp;
vector<vector<vector<Node>>> OptA::from2ai2MovableNodes;


/* start */

// 評価
struct BranchEvaluation {
  // ブランチ外から移動してくる回数。
  int moveFromOtherBranchCount = 0;
  // どれかの階層が単体でLBより多いとinvalid
  int overBSizeCount = 0;

  int ASize = 0;

  // 最小化問題として実装
  // 他のブランチから移動してくるのにかかる手数 + ブランチ内を移動する手数 =
  // 最終的な手数
  double Sum() const {
    return moveFromOtherBranchCount + overBSizeCount * PARAMS[8] +
           max(0, ASize - LA) * 50;
  }

  bool IsValid() const { return ASize <= LA && overBSizeCount == 0; }

  friend ostream& operator<<(ostream& os, const BranchEvaluation& eval) {
    os << "moveFromOtherBranchCount: " << eval.moveFromOtherBranchCount << "\n";
    os << "overBSizeCount: " << eval.overBSizeCount << "\n";
    os << "ASize: " << eval.ASize;
    return os;
  }

  BranchEvaluation operator+(const BranchEvaluation& other) const {
    return BranchEvaluation{
        moveFromOtherBranchCount + other.moveFromOtherBranchCount,
        overBSizeCount + other.overBSizeCount,
        ASize + other.ASize,
    };
  }

  BranchEvaluation operator-(const BranchEvaluation& other) const {
    return BranchEvaluation{
        moveFromOtherBranchCount - other.moveFromOtherBranchCount,
        overBSizeCount - other.overBSizeCount,
        ASize - other.ASize,
    };
  }
};

struct BranchState {
 private:
  IndexSet<QueryNode> queryNodes;
  IndexSet<Node> tree;
  bool changed = false;
  Node baseNode;
  set<int> visitOrders;

 public:
  BranchEvaluation eval;

  explicit BranchState(Node baseNode)
      : queryNodes(N * 2), tree(N), baseNode(baseNode) {
    RebuildAll();
  }

  vector<Node> ToA() const {
    // TODO: ちゃんとやる
    // baseから遠い順に追加していく
    auto vec = tree.ToVec();
    if (vec.back() == baseNode) {
      vec.pop_back();
      vec.insert(vec.begin(), baseNode);
    }
    return vec;
    // vector<Node> A;
    // unordered_set<Node> existNodes(all(vec));
    // sort(all(vec), [&](const Node& a, const Node& b) {
    //   return Dist(baseNode, a) > Dist(baseNode, b);
    // });
    // unordered_set<Node> addedSet;
    // addedSet.insert(baseNode);
    // A.push_back(baseNode);
    // int tailCount = 1;
    // for (const auto a : vec) {
    //   if (addedSet.count(a)) continue;

    //   // addedSetまで、存在するノードのみを経由して向かう
    //   queue<Node> que;
    //   que.push(a);
    //   unordered_map<Node, Node> prevNodes;
    //   while (!que.empty()) {
    //     auto current = que.front();
    //     que.pop();
    //     if (addedSet.count(current)) {
    //       // 復元
    //       vector<Node> path;
    //       while (current != a) {
    //         current = prevNodes[current];
    //         path.push_back(current);
    //       }
    //       // LB個をオーバーするなら、先頭に入れる？
    //       if (tailCount + (int)path.size() > LB) {
    //         reverse(all(path));
    //         A.insert(A.begin(), all(path));
    //       } else {
    //         A.insert(A.end(), all(path));
    //         tailCount += path.size();
    //       }
    //       addedSet.insert(all(path));
    //       break;
    //     }

    //     for (const auto next : G.NextNodes(current)) {
    //       if (existNodes.count(next) && prevNodes.count(next) == 0) {
    //         que.push(next);
    //         prevNodes[next] = current;
    //       }
    //     }
    //   }
    // }

    // assert(A.size() == vec.size());
    // return A;
  }

  void InsertQueryNode(QueryNode queryNode) {
    queryNodes.add(queryNode);
    changed = true;
    visitOrders.insert(queryNode.VisitOrder());
  }

  void EraseQueryNode(QueryNode queryNode) {
    queryNodes.remove(queryNode);
    changed = true;
    visitOrders.erase(queryNode.VisitOrder());
  }

  const vector<QueryNode>& GetQueryNodes() const { return queryNodes.ToVec(); }

  vector<Node> GetNodes() const {
    vector<Node> nodes;
    for (const auto& queryNode : queryNodes.ToVec()) {
      nodes.push_back(queryNode.GetNode());
    }
    return nodes;
  }

  void RebuildAll() {
    // 初期化
    ResetTree();
    BuildTree();
    UpdateEvaluation();
  }

 private:
  void UpdateEvaluation() {
    // 各階層がLB以下ではない場合は、はみ出した分だけinvalid
    eval = BranchEvaluation();

    eval.ASize += tree.size();
    eval.overBSizeCount += max(0, (int)tree.size() - LB);

    // マージの有無は上の階層で管理する　
  }

  void ResetTree() { tree.clear(); }

  void BuildTree() {
    const auto targetNodes = GetNodes();
    // 内側の階層を使って構築
    vector<Node> baseNodes;
    baseNodes.push_back(baseNode);

    const auto [addedNodes, remainCount] = CompletePrim(targetNodes, baseNodes);
    if (remainCount > 0) {
      return;
    }
    for (const auto node : addedNodes) {
      tree.add(node);
    }
    tree.add(baseNode);
  }
};

struct ChangeData {
  int oldBranchIdx;
  int newBranchIdx;
  QueryNode queryNode;
  ChangeData(int oldBranchIdx, int newBranchIdx, QueryNode queryNode)
      : oldBranchIdx(oldBranchIdx),
        newBranchIdx(newBranchIdx),
        queryNode(queryNode) {}
};

struct AllBranchState {
 private:
  vector<BranchState> branchStates;
  // keyは0からTまでの訪問順
  vector<int> visitOrder2BranchIdx;
  IndexSet<int> unmergedPrevVisitOrder;

  BranchEvaluation branchEvalSum;

 public:
  AllBranchState() : unmergedPrevVisitOrder(0) {}
  AllBranchState(vector<Node> branchBaseNodes)
      : visitOrder2BranchIdx(T + 1, -1), unmergedPrevVisitOrder(T) {
    branchStates.reserve(branchBaseNodes.size());
    rep(i, branchBaseNodes.size()) {
      branchStates.emplace_back(branchBaseNodes[i]);
      branchEvalSum = branchEvalSum + branchStates[i].eval;
    }
  }

  const vector<int>& UnmergedPrevVisitOrders() const {
    return unmergedPrevVisitOrder.ToVec();
  }

  vector<Node> ToA() const {
    vector<Node> A;
    vector<vector<Node>> branchNodes;
    vector<set<QueryNode>> queryNodes;
    for (const auto& branchState : branchStates) {
      const auto& nodes = branchState.ToA();
      // if ((int)nodes.size() <= LB) {
      branchNodes.push_back(nodes);
      const auto qs = branchState.GetQueryNodes();
      queryNodes.emplace_back(all(qs));
      // } else {
      //   A.insert(A.end(), all(nodes));
      // }
    }

    // iとjの間で連続するクエリの個数を雑に数える
    vector<vector<int>> mergeScores(branchNodes.size(),
                                    vector<int>(branchNodes.size(), 0));
    rep(i, branchNodes.size()) {
      rep(j, branchNodes.size()) {
        if (i == j) continue;
        int count = 0;
        for (const auto& q : queryNodes[i]) {
          const int prevVisitOrder = q.VisitOrder() - 1;
          const int nextVisitOrder = q.VisitOrder() + 1;
          if (prevVisitOrder >= 0 &&
              queryNodes[j].count(
                  QueryNode::CreateFromVisitOrder(prevVisitOrder))) {
            int i_idx =
                find(all(branchNodes[i]), q.GetNode()) - branchNodes[i].begin();
            int j_idx =
                find(
                    all(branchNodes[j]),
                    QueryNode::CreateFromVisitOrder(prevVisitOrder).GetNode()) -
                branchNodes[j].begin();
            if (i_idx + j_idx + 1 <= LB) {
              count++;
            }
          }
          if (nextVisitOrder <= T &&
              queryNodes[j].count(
                  QueryNode::CreateFromVisitOrder(nextVisitOrder))) {
            int i_idx =
                find(all(branchNodes[i]), q.GetNode()) - branchNodes[i].begin();
            int j_idx =
                find(
                    all(branchNodes[j]),
                    QueryNode::CreateFromVisitOrder(nextVisitOrder).GetNode()) -
                branchNodes[j].begin();
            if (i_idx + j_idx + 1 <= LB) {
              count++;
            }
          }
        }
        mergeScores[i][j] = count;
      }
    }

    // マージスコアが高いほうから順にマージしていく
    vector<int> mergeTargets(branchNodes.size(), -1);
    while (true) {
      int best_i = -1;
      int best_j = -1;
      int maxScore = 0;
      rep(i, branchNodes.size()) {
        if (mergeTargets[i] >= 0) continue;
        rep(j, branchNodes.size()) {
          if (i == j || mergeTargets[j] >= 0) continue;
          if (chmax(maxScore, mergeScores[i][j])) {
            best_i = i;
            best_j = j;
          }
        }
      }
      if (best_i == -1) break;
      mergeTargets[best_i] = best_j;
      mergeTargets[best_j] = best_i;
      // マージする
      // auto rev = branchNodes[best_i];
      // reverse(all(rev));
      // rev.pop_back();
      // A.insert(A.end(), all(rev));
      // A.insert(A.end(), all(branchNodes[best_j]));
      // isMerged[best_i] = true;
      // isMerged[best_j] = true;
    }

    // 少し山登り
    rep(t, (int)1e6){
      int i = rand(0, mergeTargets.size());
      int j = rand_other_than(0, mergeTargets.size(), i);
      if (mergeTargets[i] == j) continue;
      int iii = mergeTargets[i];
      int jjj = mergeTargets[j];
      int scoreDiff = 0;
      scoreDiff -= mergeScores[i][iii];
      scoreDiff -= mergeScores[j][jjj];
      scoreDiff += mergeScores[i][jjj];
      scoreDiff += mergeScores[j][iii];
      if (scoreDiff > 0) {
        swap(mergeTargets[i], mergeTargets[j]);
        swap(mergeTargets[iii], mergeTargets[jjj]);
      }
    }

    // マージする
    vector<bool> isMerged(mergeTargets.size(), false);
    rep(i, mergeTargets.size()){
      if (isMerged[i]) continue;
      if (mergeTargets[i] == -1) {
        A.insert(A.end(), all(branchNodes[i]));
        continue;
      }
      auto rev = branchNodes[i];
      reverse(all(rev));
      rev.pop_back();
      A.insert(A.end(), all(rev));
      A.insert(A.end(), all(branchNodes[mergeTargets[i]]));
      isMerged[i] = true;
      isMerged[mergeTargets[i]] = true;
    }

    return A;
  }

  bool IsValid() const {
    if (!branchEvalSum.IsValid()) return false;
    rep(i, visitOrder2BranchIdx.size()) {
      if (visitOrder2BranchIdx[i] < 0) return false;
    }
    return true;
  }

  bool IsConnected(int prevVisitOrder) const {
    assert(prevVisitOrder >= 0);
    assert(prevVisitOrder < T);
    return visitOrder2BranchIdx[prevVisitOrder] ==
           visitOrder2BranchIdx[prevVisitOrder + 1];
  }

  vector<int> ConnectedVisitOrders(int baseVisitOrder) const {
    vector<int> connectedVisitOrders;
    connectedVisitOrders.push_back(baseVisitOrder);
    for (int delta : {-1, 1}) {
      for (int visitOrder = baseVisitOrder + delta;
           0 <= visitOrder && visitOrder <= T; visitOrder += delta) {
        if (visitOrder2BranchIdx[visitOrder] !=
            visitOrder2BranchIdx[baseVisitOrder]) {
          break;
        }
        connectedVisitOrders.push_back(visitOrder);
      }
    }
    return connectedVisitOrders;
  }

  void Backward(const vector<ChangeData>& changes) {
    unordered_set<int> changedBranchIdxes;
    // 逆順に適用する
    REP(i, changes.size()) {
      const auto& change = changes[i];
      changedBranchIdxes.insert(change.oldBranchIdx);
      changedBranchIdxes.insert(change.newBranchIdx);
      if (change.newBranchIdx >= 0) {
        RemoveQueryNode(change.queryNode, true);
      }
      if (change.oldBranchIdx >= 0) {
        InsertQueryNode(change.oldBranchIdx, change.queryNode, true);
      }
    }
    for (const auto branchIdx : changedBranchIdxes) {
      if (branchIdx < 0) continue;
      Rebuild(branchIdx);
    }
  }

  static AllBranchState CreateGreedy(int branchCount, Node baseNode) {
    vector<Node> branchBaseNodes(branchCount, baseNode);
    vector<Node> visitOrderBaseNodes(T + 1, baseNode);
    return CreateGreedy(branchBaseNodes, visitOrderBaseNodes);
  }

  static AllBranchState CreateGreedy(vector<Node> branchBaseNodes,
                                     vector<Node> visitOrderBaseNodes) {
    // ASizeが小さくなるように貪欲にクエリを追加していく
    AllBranchState allBranchState(branchBaseNodes);

    vector<QueryNode> allQueryNodes;
    rep(i, T) { allQueryNodes.emplace_back(i, true); }
    allQueryNodes.emplace_back(T - 1, false);

    // 基準点から遠い順にソート
    sort(all(allQueryNodes), [&](const QueryNode& a, const QueryNode& b) {
      return Dist(visitOrderBaseNodes[a.VisitOrder()], a.GetNode()) >
             Dist(visitOrderBaseNodes[b.VisitOrder()], b.GetNode());
    });
    vector<bool> isRemain(allQueryNodes.size(), true);

    rep(i, T + 1) {
      if (!isRemain[i]) {
        continue;
      }
      // 各ブランチにクエリを追加し、以下の評価値で比較する
      // 1. LBを超えた量が少ないほうが良い
      // 2. ASizeの増加量が小さいほうが良い
      int bestBranchIdx = -1;
      {
        const auto queryNode = allQueryNodes[i];
        pair<int, int> minEval = {INF, INF};
        const int oldOverBSizeCount = allBranchState.Eval().overBSizeCount;
        const int oldASize = allBranchState.Eval().ASize;
        rep(branchIdx, allBranchState.BranchCount()) {
          allBranchState.InsertQueryNode(branchIdx, queryNode);
          pair<int, int> eval = {
              allBranchState.Eval().overBSizeCount - oldOverBSizeCount,
              allBranchState.Eval().ASize - oldASize};
          if (chmin(minEval, eval)) {
            bestBranchIdx = branchIdx;
          }
          allBranchState.RemoveQueryNode(queryNode);
        }

        assert(bestBranchIdx != -1);
        // LBを超えるなら、追加しない
        if (minEval.first > 0) {
          continue;
        }
        allBranchState.InsertQueryNode(bestBranchIdx, queryNode);
        isRemain[i] = false;
      }

      // その枝に追加できるだけ追加していく
      while (true) {
        int bestQueryIdx = -1;
        pair<int, int> minEval = {INF, INF};
        for (int j = i + 1; j <= T; j++) {
          if (!isRemain[j]) {
            continue;
          }
          const auto queryNode = allQueryNodes[j];
          allBranchState.InsertQueryNode(bestBranchIdx, queryNode);
          pair<int, int> eval = {allBranchState.Eval().overBSizeCount,
                                 allBranchState.Eval().ASize};
          if (chmin(minEval, eval)) {
            bestQueryIdx = j;
          }
          allBranchState.RemoveQueryNode(queryNode);
        }

        if (bestQueryIdx == -1) {
          break;
        }
        // LBを超えるなら、追加しない
        if (minEval.first > 0) {
          break;
        }

        allBranchState.InsertQueryNode(bestBranchIdx,
                                       allQueryNodes[bestQueryIdx]);
        isRemain[bestQueryIdx] = false;
      }
    }

    cerr << "Greedy: " << allBranchState.Eval() << endl;

    return allBranchState;
  }

  int BranchCount() const { return branchStates.size(); }

  int GetBranchIdx(QueryNode queryNode) const {
    return visitOrder2BranchIdx[queryNode.VisitOrder()];
  }

  const BranchState& GetBranchState(int branchIdx) const {
    return branchStates[branchIdx];
  }

  void Rebuild(int branchIdx) {
    auto& branchState = branchStates[branchIdx];
    branchEvalSum = branchEvalSum - branchState.eval;
    branchState.RebuildAll();
    branchEvalSum = branchEvalSum + branchState.eval;
  }

  ChangeData InsertQueryNode(int branchIdx, QueryNode queryNode,
                             bool noBuild = false) {
    auto& branchState = branchStates[branchIdx];
    branchState.InsertQueryNode(queryNode);
    if (!noBuild) {
      Rebuild(branchIdx);
    }
    visitOrder2BranchIdx[queryNode.VisitOrder()] = branchIdx;
    const int visitOrder = queryNode.VisitOrder();
    if (visitOrder - 1 >= 0 && visitOrder2BranchIdx[visitOrder - 1] >= 0 &&
        visitOrder2BranchIdx[visitOrder - 1] != branchIdx) {
      unmergedPrevVisitOrder.add(visitOrder - 1);
    }
    if (visitOrder + 1 <= T && visitOrder2BranchIdx[visitOrder + 1] >= 0 &&
        visitOrder2BranchIdx[visitOrder + 1] != branchIdx) {
      unmergedPrevVisitOrder.add(visitOrder);
    }
    branchEvalSum.moveFromOtherBranchCount = unmergedPrevVisitOrder.size() + 1;

    return ChangeData(-1, branchIdx, queryNode);
  }

  ChangeData RemoveQueryNode(QueryNode queryNode, bool noBuild = false) {
    ChangeData changeData(GetBranchIdx(queryNode), -1, queryNode);
    const auto branchIdx = GetBranchIdx(queryNode);
    auto& branchState = branchStates[branchIdx];
    branchState.EraseQueryNode(queryNode);
    if (!noBuild) {
      Rebuild(branchIdx);
    }
    visitOrder2BranchIdx[queryNode.VisitOrder()] = -1;
    const int visitOrder = queryNode.VisitOrder();
    if (visitOrder - 1 >= 0) {
      unmergedPrevVisitOrder.removeIfExist(visitOrder - 1);
    }
    if (visitOrder + 1 <= T) {
      unmergedPrevVisitOrder.removeIfExist(visitOrder);
    }
    branchEvalSum.moveFromOtherBranchCount = unmergedPrevVisitOrder.size() + 1;

    return changeData;
  }

  BranchEvaluation Eval() const { return branchEvalSum; }
};





namespace NBDBranch {

vector<vector<QueryNode>> node2QueryNodes;
vector<vector<QueryNode>> visitOrder2PrevNextQueryNodes;
vector<vector<QueryNode>> node2NearQueryNodes;
double globalProgress = 0.0;

void Init() {
  node2QueryNodes.resize(N);
  node2NearQueryNodes.resize(N);
  vector<QueryNode> queryNodes;
  rep(i, T) { queryNodes.push_back(QueryNode(i, true)); }
  queryNodes.push_back(QueryNode(T - 1, false));

  for (const auto queryNode : queryNodes) {
    const auto node = queryNode.GetNode();
    node2QueryNodes[node].push_back(queryNode);
    rep(i, N) {
      Node otherNode(i);
      if (Dist(node, otherNode) <= 1) {
        node2NearQueryNodes[otherNode].push_back(queryNode);
      }
    }
  }
  visitOrder2PrevNextQueryNodes.resize(T + 1);
  rep(i, T + 1) {
    if (i - 1 >= 0) {
      visitOrder2PrevNextQueryNodes[i].push_back(
          QueryNode::CreateFromVisitOrder(i - 1));
    }
    if (i + 1 <= T) {
      visitOrder2PrevNextQueryNodes[i].push_back(
          QueryNode::CreateFromVisitOrder(i + 1));
    }
  }
}

/*

実装手順
0. Output::StaticInit(istream&)を実装
1. Solutionの初期解生成を書く
2. Solutionに対する操作とEvalの更新を書く
3. Evalの初期値計算を書く
4. Eval::Sum()とSolution::Score()を書く
5. 近傍操作と復元操作を書く
6. 近傍の各確率を調整

*/

struct Solution {
  AllBranchState allBranchState;
  Solution() {}
  Solution(AllBranchState allBranchState) : allBranchState(allBranchState) {}
  Solution(int branchCount, Node baseNode) {
    allBranchState = AllBranchState::CreateGreedy(branchCount, baseNode);
    cerr << "score: " << Score() << endl;
  }

  double Eval() const { return allBranchState.Eval().Sum(); }
  double Score() const { return -Eval(); }
  friend ostream& operator<<(ostream& os, const Solution& sol) { return os; }
};

struct NBD {
  double Diff() const { return diff; }
  virtual void Update(Solution*) const = 0;
  virtual void Restore(Solution*) const = 0;
  double diff;
};

// ブランチ間でのクエリの移動
struct NBD1 : public NBD {
  NBD1() {}
  NBD1(Solution* const sol) {
    diff = -sol->Eval();
    rep(_t, 100) {
      queryNode = QueryNode::CreateFromVisitOrder(rand(0, T + 1));
      fromBranchIdx = sol->allBranchState.GetBranchIdx(queryNode);
      toBranchIdx =
          rand_other_than(0, sol->allBranchState.BranchCount(), fromBranchIdx);
      sol->allBranchState.RemoveQueryNode(queryNode);
      sol->allBranchState.InsertQueryNode(toBranchIdx, queryNode);
      diff += sol->Eval();
      return;
    }
    assert(false);
  }
  void Update(Solution* sol) const override {
    if (diff < -1.0 + 1e-3) {
      // cerr << "NBD1 :" << diff << endl;
    }
  }
  void Restore(Solution* sol) const override {
    sol->allBranchState.RemoveQueryNode(queryNode);
    sol->allBranchState.InsertQueryNode(fromBranchIdx, queryNode);
  }
  int fromBranchIdx, toBranchIdx;
  QueryNode queryNode;
};

// 近くのノードのある別ブランチに移動する
struct NBD3 : public NBD {
  NBD3() {}
  NBD3(Solution* const sol) {
    diff = -sol->Eval();
    rep(_t, 10) {
      queryNode = QueryNode::CreateFromVisitOrder(rand(0, T + 1));
      fromBranchIdx = sol->allBranchState.GetBranchIdx(queryNode);

      // 同じ位置または隣接位置のノード
      const auto& nearQueryNodes = node2NearQueryNodes[queryNode.GetNode()];
      if (nearQueryNodes.size() == 1) {
        assert(nearQueryNodes.back() == queryNode);
        continue;
      }

      const auto anotherQueryNode = rand_vec(nearQueryNodes);
      if (anotherQueryNode == queryNode) {
        continue;
      }
      toBranchIdx = sol->allBranchState.GetBranchIdx(anotherQueryNode);
      if (fromBranchIdx == toBranchIdx) continue;
      sol->allBranchState.RemoveQueryNode(queryNode);
      sol->allBranchState.InsertQueryNode(toBranchIdx, queryNode);
      diff += sol->Eval();
      return;
    }
    diff = INF;
    return;
  }
  void Update(Solution* sol) const override {
    if (diff < -1.0 + 1e-3) {
      // cerr << "NBD3 :" << diff << endl;
    }
  }
  void Restore(Solution* sol) const override {
    if (diff == INF) return;
    sol->allBranchState.RemoveQueryNode(queryNode);
    sol->allBranchState.InsertQueryNode(fromBranchIdx, queryNode);
  }
  int fromBranchIdx, toBranchIdx;
  QueryNode queryNode;
};

// 対となるクエリがある場所に移動
struct NBD4 : public NBD {
  NBD4() {}
  NBD4(Solution* const sol) {
    diff = -sol->Eval();
    rep(_t, 100) {
      const auto& unmergedPrevVisitOrders =
          sol->allBranchState.UnmergedPrevVisitOrders();
      QueryNode anotherQueryNode;
      if ((int)unmergedPrevVisitOrders.size() > 0) {
        const int prevOrder = rand_vec(unmergedPrevVisitOrders);
        if (rand(0, 2) == 0) {
          queryNode = QueryNode::CreateFromVisitOrder(prevOrder);
          anotherQueryNode = QueryNode::CreateFromVisitOrder(prevOrder + 1);
        } else {
          queryNode = QueryNode::CreateFromVisitOrder(prevOrder + 1);
          anotherQueryNode = QueryNode::CreateFromVisitOrder(prevOrder);
        }
      } else {
        // そうはならんやろ
        break;
      }

      fromBranchIdx = sol->allBranchState.GetBranchIdx(queryNode);
      toBranchIdx = sol->allBranchState.GetBranchIdx(anotherQueryNode);
      if (fromBranchIdx == toBranchIdx) continue;
      sol->allBranchState.RemoveQueryNode(queryNode);
      sol->allBranchState.InsertQueryNode(toBranchIdx, queryNode);
      diff += sol->Eval();
      return;
    }
    diff = INF;
    return;
  }
  void Update(Solution* sol) const override {
    if (diff < -1.0 + 1e-3) {
      // cerr << "NBD4 :" << diff << endl;
    }
  }
  void Restore(Solution* sol) const override {
    if (diff == INF) return;
    sol->allBranchState.RemoveQueryNode(queryNode);
    sol->allBranchState.InsertQueryNode(fromBranchIdx, queryNode);
  }
  int fromBranchIdx, toBranchIdx;
  QueryNode queryNode;
};

// ブランチ間でのクエリのスワップ
struct NBD7 : public NBD {
  NBD7() {}
  NBD7(Solution* const sol) {
    diff = -sol->Eval();
    rep(_t, 100) {
      const int order0 = rand(0, T + 1);
      queryNode0 = QueryNode::CreateFromVisitOrder(order0);
      branchIdx0 = sol->allBranchState.GetBranchIdx(queryNode0);
      queryNode1 =
          QueryNode::CreateFromVisitOrder(rand_other_than(0, T + 1, order0));
      branchIdx1 = sol->allBranchState.GetBranchIdx(queryNode1);
      if (branchIdx1 == branchIdx0) continue;
      sol->allBranchState.RemoveQueryNode(queryNode0, true);
      sol->allBranchState.RemoveQueryNode(queryNode1, true);
      sol->allBranchState.InsertQueryNode(branchIdx1, queryNode0, true);
      sol->allBranchState.InsertQueryNode(branchIdx0, queryNode1, true);
      sol->allBranchState.Rebuild(branchIdx0);
      sol->allBranchState.Rebuild(branchIdx1);
      diff += sol->Eval();
      return;
    }
    diff = INF;
    return;
  }
  void Update(Solution* sol) const override {
    if (diff < -1.0 + 1e-3) {
      // cerr << "NBD7 :" << diff << endl;
    }
  }
  void Restore(Solution* sol) const override {
    if (diff == INF) return;
    sol->allBranchState.RemoveQueryNode(queryNode0, true);
    sol->allBranchState.RemoveQueryNode(queryNode1, true);
    sol->allBranchState.InsertQueryNode(branchIdx0, queryNode0, true);
    sol->allBranchState.InsertQueryNode(branchIdx1, queryNode1, true);
    sol->allBranchState.Rebuild(branchIdx0);
    sol->allBranchState.Rebuild(branchIdx1);
  }
  int branchIdx0, branchIdx1;
  QueryNode queryNode0;
  QueryNode queryNode1;
};

// つながりを意識した、ブランチ間でのクエリのスワップ
struct NBD8 : public NBD {
  NBD8() {}
  NBD8(Solution* const sol) {
    diff = -sol->Eval();
    rep(_t, 100) {
      const int prevOrder = rand(0, T);
      const int nextOrder = prevOrder + 1;
      if (sol->allBranchState.IsConnected(prevOrder)) continue;
      const bool toNext = rand(0, 2) == 0;
      const int fromOrder = toNext ? prevOrder : nextOrder;
      const int toOrder = toNext ? nextOrder : prevOrder;
      queryNode0 = QueryNode::CreateFromVisitOrder(fromOrder);

      // fromOrderからtoOrderに移動するので、
      // 代わりにtoOrderのbranchから連結でないクエリを取り出す
      const int fromBranchIdx = sol->allBranchState.GetBranchIdx(queryNode0);
      const int toBranchIdx = sol->allBranchState.GetBranchIdx(
          QueryNode::CreateFromVisitOrder(toOrder));
      if (fromBranchIdx == toBranchIdx) continue;

      // TODO: 0階層以外を考慮する
      const auto& toQueryNodes =
          sol->allBranchState.GetBranchState(toBranchIdx).GetQueryNodes();
      if (toQueryNodes.size() == 0) continue;

      bool ok = false;
      rep(_tt, 10) {
        queryNode1 = rand_vec(toQueryNodes);
        const int visitOrder = queryNode1.VisitOrder();
        if (visitOrder - 1 >= 0 &&
            sol->allBranchState.IsConnected(visitOrder - 1))
          continue;
        if (visitOrder + 1 <= T && sol->allBranchState.IsConnected(visitOrder))
          continue;
        ok = true;
        break;
      }

      if (!ok) continue;

      // スワップ
      branchIdx0 = fromBranchIdx;
      branchIdx1 = toBranchIdx;

      sol->allBranchState.RemoveQueryNode(queryNode0, true);
      sol->allBranchState.RemoveQueryNode(queryNode1, true);
      sol->allBranchState.InsertQueryNode(branchIdx1, queryNode0, true);
      sol->allBranchState.InsertQueryNode(branchIdx0, queryNode1, true);
      sol->allBranchState.Rebuild(branchIdx0);
      sol->allBranchState.Rebuild(branchIdx1);
      diff += sol->Eval();

      return;
    }
    diff = INF;
    return;
  }
  void Update(Solution* sol) const override {
    if (diff < -1.0 + 1e-3) {
      // cerr << "NBD8 :" << diff << endl;
    }
  }
  void Restore(Solution* sol) const override {
    if (diff == INF) return;
    sol->allBranchState.RemoveQueryNode(queryNode0, true);
    sol->allBranchState.RemoveQueryNode(queryNode1, true);
    sol->allBranchState.InsertQueryNode(branchIdx0, queryNode0, true);
    sol->allBranchState.InsertQueryNode(branchIdx1, queryNode1, true);
    sol->allBranchState.Rebuild(branchIdx0);
    sol->allBranchState.Rebuild(branchIdx1);
  }
  int branchIdx0, branchIdx1;
  QueryNode queryNode0;
  QueryNode queryNode1;
};

vector<int> randomThresholds;

struct NBDGenerator {
  NBD* Generate(Solution* const sol, const int g, const double progress) {
    // if (g % 1000 == 0) cerr << g << " " << sol->allBranchState.Eval() <<
    // endl;
    globalProgress = progress;
    const int random = rand(0, 10000);
    if (random < randomThresholds[0]) {
      nbd1_ = NBD1(sol);
      return &nbd1_;
    } else if (random < randomThresholds[1]) {
      nbd3_ = NBD3(sol);
      return &nbd3_;
    } else {
      nbd4_ = NBD4(sol);
      return &nbd4_;
    }
    //  else if (random < randomThresholds[3]) {
    //   nbd7_ = NBD7(sol);
    //   return &nbd7_;
    // } else {
    //   nbd8_ = NBD8(sol);
    //   return &nbd8_;
    // }
  }

 private:
  NBD1 nbd1_;
  NBD3 nbd3_;
  NBD4 nbd4_;
  // NBD7 nbd7_;
  // NBD8 nbd8_;
};

vector<Node> Run(int timeLimit, Solution first_sol) {
  if (!first_sol.allBranchState.IsValid()) {
    cerr << "Validな初期解を生成できませんでした。" << endl;
    // cerr << "score: 100000000" << endl;
    // exit(0);
    return vector<Node>();
  }
  int sum = 0;
  rep(i, 5) {
    sum += PARAMS[i + 3];
    randomThresholds.push_back(sum);
  }
  rep(i, 5) { randomThresholds[i] = randomThresholds[i] * 10000 / sum; }
  // return first_sol.allBranchState.ToA();
  NBDGenerator gen;
  SimulatedAnnealing<Solution, NBDGenerator> SA(timeLimit, PARAMS[1],
                                                PARAMS[2]);
  const auto final_sol = SA.run(first_sol, gen);
  cerr << final_sol.allBranchState.Eval() << endl;
  cerr << "score: " << final_sol.Score() << endl;
  const auto A = final_sol.allBranchState.ToA();
  cerr << "true ASize: " << A.size() << endl;
  // if (!final_sol.allBranchState.IsValid()) {
  //   cerr << "score: 100000000" << endl;
  //   exit(0);
  // }
  return A;
}

vector<Node> Run(int timeLimit, AllBranchState allBranchState) {
  Init();
  Solution first_sol(allBranchState);
  return Run(timeLimit, first_sol);
}

vector<Node> Run(int timeLimit) {
  Init();
  Solution first_sol(LA / LB, centerNode);
  return Run(timeLimit, first_sol);
}

}  // namespace NBDBranch












/* start */

vector<Node> BranchFromCenterOld() {
  // 中心からの距離が遠い順に追加していく
  vector<Node> sortedNodes(all(allVisitNodes));
  sort(all(sortedNodes), [&](const Node& a, const Node& b) {
    return Dist(centerNode, a) > Dist(centerNode, b);
  });

  vector<Branch> branches;
  vector<bool> hasAddedNode(N, false);
  for (const auto toNode : sortedNodes) {
    if (hasAddedNode[toNode]) {
      continue;
    }
    // 新たに枝を作成する
    Branch baseBranch(vector<Node>{centerNode, toNode});
    if ((int)baseBranch.Size() > LB - 1) {
      cerr << "LBに収まらないのでBranchをスキップします" << endl;
      continue;
    }

    hasAddedNode[toNode] = true;
    // その枝に追加するコストが最も低い頂点を、追加していく
    // コストが同じ場合は、中心から遠いほうを優先
    while ((int)baseBranch.Size() < LB) {
      int minSizeAfter = INF;
      Node bestNode = Node(-1);
      for (const auto addNode : sortedNodes) {
        if (hasAddedNode[addNode]) continue;
        const auto addBranch = baseBranch.CloneAdd(addNode);
        int sizeAfter = addBranch.Size();
        if (sizeAfter > LB) continue;
        if (chmin(minSizeAfter, sizeAfter)) {
          bestNode = addNode;
          // 無条件で追加
          if (sizeAfter == baseBranch.Size()) break;
        }
      }

      if (minSizeAfter == INF) break;
      baseBranch = baseBranch.CloneAdd(bestNode);
      hasAddedNode[bestNode] = true;
    }

    branches.push_back(baseBranch);
  }

  cerr << "exit" << endl;
  exit(0);

  rep(i, (int)sortedNodes.size()) {
    if (hasAddedNode[sortedNodes[i]]) continue;
    cerr << "全ての頂点を追加できませんでした" << endl;
    return vector<Node>();
  }

  int sizeSum = 0;
  for (const auto branch : branches) {
    sizeSum += branch.Size();
  }
  cerr << "sizeSum: " << sizeSum << endl;
  if ((int)sizeSum > LA) {
    cerr << "LAを超えてしまいました" << endl;
    return vector<Node>();
  }

  // ブランチのマージする対象を決める
  vector<vector<int>> node2BracnhIdxes(N);
  rep(i, (int)branches.size()) {
    for (const auto node : branches[i].GetAllNodes()) {
      node2BracnhIdxes[node].push_back(i);
    }
  }
  vector<int> mergePair(branches.size(), -1);
  for (const auto [node0, node1] : inputted_queries) {
    pair<int, int> bestPair = {-1, -1};
    for (const auto branchIdx0 : node2BracnhIdxes[node0]) {
      if (mergePair[branchIdx0] != -1) {
        continue;
      }
      const int idx0 = branches[branchIdx0].GetDistFromBase(node0);
      for (const auto branchIdx1 : node2BracnhIdxes[node1]) {
        if (mergePair[branchIdx1] != -1) {
          continue;
        }
        // 既に見つかっていても、等しいほうが嬉しいので、確認する
        if (branchIdx0 == branchIdx1) {
          bestPair = {branchIdx0, branchIdx1};
          break;
        }
        if (bestPair.first != -1) break;
        const int idx1 = branches[branchIdx1].GetDistFromBase(node1);

        // idx0への移動 + idx1への移動 + 1(中心) <=
        // LBなら、マージすると手数圧縮できる
        const int len = idx0 + idx1 + 1;
        if (len <= LB) {
          bestPair = {branchIdx0, branchIdx1};
          continue;
        }
      }
    }

    if (bestPair.first == -1) continue;
    const auto [branchIdx0, branchIdx1] = bestPair;
    mergePair[branchIdx0] = branchIdx1;
    mergePair[branchIdx1] = branchIdx0;
  }

  // ブランチをマージしてAを生成する
  vector<Node> A;
  int mergeCount = 0;
  rep(i, (int)branches.size()) {
    // マージ先が無い場合や、自分自身にマージする場合は、そのまま追加
    if (mergePair[i] == -1 || mergePair[i] == i) {
      A.insert(A.end(), all(branches[i].GetAllNodes()));
      continue;
    }

    // マージ先が既に追加済みなら、スキップ
    if (mergePair[i] < i) {
      continue;
    }
    mergeCount++;

    // マージ先を反転してcenterを除いたものと、自身を、追加する
    const auto branch = branches[i];
    const auto anotherBranch = branches[mergePair[i]];
    auto flipAnotherA = anotherBranch.GetAllNodes();
    reverse(all(flipAnotherA));
    assert(flipAnotherA.back() == centerNode);
    flipAnotherA.pop_back();
    A.insert(A.end(), all(flipAnotherA));
    A.insert(A.end(), all(branch.GetAllNodes()));
  }
  cerr << mergeCount << "回マージしました" << endl;

  return A;
}

// クエリ単体・クエリペアでプリムをした結果の情報

struct QueryMergeData {
  vector<Query> queries;
  Branch branch;
  QueryMergeData(const vector<Query>& queries, const Branch& branch)
      : queries(queries), branch(branch) {}
};

vector<QueryMergeData> queryMetaDatas;
vector<QueryMergeData> singleQueryMetaDatas;

void InitQueryMetaDatas() {
  rep(query_i, T) {
    vector<Query> queries;
    queries.push_back(inputtedQueries[query_i]);
    rep(query_j, query_i + 1) {
      if (query_i != query_j) {
        // 等しいときは、クエリ単体のみ
        queries.push_back(inputtedQueries[query_j]);
      }

      vector<Node> nodes;
      nodes.push_back(centerNode);
      unordered_set<Node> added;
      added.insert(centerNode);
      for (const auto& query : queries) {
        if (added.find(query.from) == added.end()) {
          nodes.push_back(query.from);
          added.insert(query.from);
        }
        if (added.find(query.to) == added.end()) {
          nodes.push_back(query.to);
          added.insert(query.to);
        }
      }

      Branch branch(nodes);
      if (branch.Size() <= LB) {
        queryMetaDatas.emplace_back(queries, branch);
      }

      if (query_i != query_j) {
        queries.pop_back();
      } else {
        singleQueryMetaDatas.emplace_back(queries, branch);
      }
    }
  }
}

vector<Node> BranchFromCenterOld2() {
  InitQueryMetaDatas();
  sort(all(queryMetaDatas),
       [&](const QueryMergeData& a, const QueryMergeData& b) {
         if (a.queries.size() != b.queries.size()) {
           // 多いほうが嬉しい
           return a.queries.size() > b.queries.size();
         }
         // 小さいほうが嬉しい
         return a.branch.Size() < b.branch.Size();
       });

  vector<int> query2BranchIdxes(T, -1);
  vector<bool> isRemainNode(N, false);
  int remainNodeCount = allVisitNodes.size();
  for (const auto node : allVisitNodes) {
    isRemainNode[node] = true;
  }
  int queryCount = 0;
  int branchSizeSum = 0;
  vector<Branch> branches;
  for (const auto& queryMetaData : queryMetaDatas) {
    const auto& queries = queryMetaData.queries;
    const auto& branch = queryMetaData.branch;
    // 既に追加済みのクエリがある場合は、スキップ
    bool hasAddedQuery = false;
    bool hasAddedNode = false;
    for (const auto& query : queries) {
      if (query2BranchIdxes[query.id] != -1) {
        hasAddedQuery = true;
      }
      if (!isRemainNode[query.from] || !isRemainNode[query.to]) {
        hasAddedNode = true;
      }
    }
    // TODO: スキップしないで、既存のブランチへの追加を試みるべきかも
    if (hasAddedQuery) {
      continue;
    }
    // TODO: スキップしないで、クエリの短縮を試みるべきかも
    if (hasAddedNode) {
      continue;
    }

    // ブランチを追加
    const int nextBranchIdx = (int)branches.size();
    auto newBranch = branch;
    for (const auto& query : queries) {
      query2BranchIdxes[query.id] = nextBranchIdx;
      queryCount++;
      remainNodeCount -= isRemainNode[query.from];
      isRemainNode[query.from] = false;
      remainNodeCount -= isRemainNode[query.to];
      isRemainNode[query.to] = false;
    }

    // 追加できるクエリがあるなら追加する
    bitset<600> isExistNode = 0;
    for (const auto node : newBranch.GetOrders()) {
      isExistNode[node] = 1;
    }
    // 最も消費が小さいものを選んだほうが良い
    for (const auto& query : inputtedQueries) {
      if (query2BranchIdxes[query.id] != -1) {
        continue;
      }
      const auto [_, node0, node1] = query;
      // ノードが被ってないならやめておく
      if (!isExistNode[node0] && !isExistNode[node1]) {
        continue;
      }
      auto nodes = newBranch.GetOrders();
      if (!isExistNode[node0]) {
        nodes.push_back(node0);
      }
      if (!isExistNode[node1]) {
        nodes.push_back(node1);
      }

      auto mergedBranch = Branch(nodes);
      // 単体で入れるよりも効率よかったら追加
      if (mergedBranch.Size() <= LB &&
          singleQueryMetaDatas[query.id].branch.Size() >=
              mergedBranch.Size() - newBranch.Size() + 5 &&
          mergedBranch.Size() - newBranch.Size() <= 3) {
        newBranch = mergedBranch;
        query2BranchIdxes[query.id] = nextBranchIdx;
        queryCount++;
        isExistNode[node0] = 1;
        isExistNode[node1] = 1;
        remainNodeCount -= isRemainNode[node0];
        isRemainNode[node0] = false;
        remainNodeCount -= isRemainNode[node1];
        isRemainNode[node1] = false;
      }
    }

    // ブランチのすべてのノードをremainNodeに反映
    for (const auto node : newBranch.GetAllNodes()) {
      remainNodeCount -= isRemainNode[node];
      isRemainNode[node] = false;
    }

    branches.push_back(newBranch);
    branchSizeSum += newBranch.Size();
    cerr << "branchCount: " << (int)branches.size() << endl;
    cerr << "queryCount: " << queryCount << endl;
    cerr << "branchSizeSum: " << branchSizeSum << endl;
    cerr << "remainNodeCount: " << remainNodeCount << endl;
    cerr << endl;
    if (branchSizeSum + remainNodeCount + 100 >= LA) {
      cerr << "途中ですが終了します" << endl;
      break;
    }
  }

  cerr << "branchCount: " << (int)branches.size() << endl;
  cerr << "queryCount: " << queryCount << endl;
  cerr << "branchSizeSum: " << branchSizeSum << endl;
  cerr << "remainNodeCount: " << remainNodeCount << endl;
  return vector<Node>();
}

vector<Node> BranchFromCenter() {
  // 距離が小さいクエリから、順に枝を作成していく
  vector<int> sortedQueryIdxes(T);
  iota(all(sortedQueryIdxes), 0);
  sort(all(sortedQueryIdxes), [&](const int a, const int b) {
    return Dist(inputted_queries[a].first, inputted_queries[a].second) <
           Dist(inputted_queries[b].first, inputted_queries[b].second);
  });

  vector<bool> isCheckedNode(T, false);
  vector<int> query2BranchIdxes(N, -1);
  int remainNodeCount = allVisitNodes.size();
  vector<Branch> generatedBranches;
  int mergeCount = 0;
  int sizeSum = 0;
  for (const auto queryIdx : sortedQueryIdxes) {
    const auto [node0, node1] = inputted_queries[queryIdx];
    // TODO: 異なるクエリの同じノードの二重追加も検討する
    if (isCheckedNode[node0] || isCheckedNode[node1]) {
      continue;
    }

    // 枝を生成できるか確認
    Branch branch(vector<Node>{centerNode, node0, node1});
    if (branch.Size() > LB) {
      // マージ不可能
      continue;
    }
    Branch branch0(vector<Node>{centerNode, node0});
    Branch branch1(vector<Node>{centerNode, node1});
    if (branch.Size() >= max(branch0.Size(), branch1.Size()) + 5) {
      // マージ効率が悪く、サイズが増えすぎる場合は、追加をやめる
      continue;
    }

    mergeCount++;
    remainNodeCount -= !isCheckedNode[node0];
    remainNodeCount -= !isCheckedNode[node1];
    isCheckedNode[node0] = true;
    isCheckedNode[node1] = true;
    query2BranchIdxes[queryIdx] = (int)generatedBranches.size();
    // node0, node1が絡む他のクエリも更にマージできるならマージする
    unordered_set<int> relativeQueryIdxes;
    relativeQueryIdxes.insert(node2QueryIndexes[node0].begin(),
                              node2QueryIndexes[node0].end());
    relativeQueryIdxes.insert(node2QueryIndexes[node1].begin(),
                              node2QueryIndexes[node1].end());
    while (true) {
      int minBranchSize = INF;
      int bestQueryIdx = -1;
      Node bestOtherNode = Node(-1);
      for (const auto relativeQueryIdx : relativeQueryIdxes) {
        auto [relativeNode0, relativeNode1] =
            inputted_queries[relativeQueryIdx];
        if (isCheckedNode[relativeNode0] && isCheckedNode[relativeNode1]) {
          continue;
        }
        auto otherNode =
            isCheckedNode[relativeNode0] ? relativeNode1 : relativeNode0;
        const auto newBranch = branch.CloneAdd(otherNode);
        if (newBranch.Size() <= LB) {
          if (chmin(minBranchSize, newBranch.Size())) {
            bestQueryIdx = relativeQueryIdx;
            bestOtherNode = otherNode;
          }
        }
      }

      if (minBranchSize == INF) {
        break;
      }

      // マージ効率が悪く、サイズが増えすぎる場合は、追加をやめる
      if (minBranchSize >= branch.Size() + 5) break;

      mergeCount++;
      branch = branch.CloneAdd(bestOtherNode);
      remainNodeCount -= !isCheckedNode[bestOtherNode];
      isCheckedNode[bestOtherNode] = true;
      query2BranchIdxes[bestQueryIdx] = (int)generatedBranches.size();
      relativeQueryIdxes.insert(node2QueryIndexes[bestOtherNode].begin(),
                                node2QueryIndexes[bestOtherNode].end());
    }

    // 追加
    generatedBranches.push_back(branch);
    sizeSum += branch.Size();
    if (sizeSum + remainNodeCount + 50 >= LA) break;
  }

  int queryCount = 0;
  for (const auto queryIdx : sortedQueryIdxes) {
    if (isCheckedNode[inputted_queries[queryIdx].first] &&
        isCheckedNode[inputted_queries[queryIdx].second]) {
      queryCount++;
    }
  }
  cerr << "sizeSum: " << sizeSum << endl;
  cerr << "queryCount: " << queryCount << endl;
  cerr << "branchCount: " << (int)generatedBranches.size() << endl;
  cerr << "mergeCount: " << mergeCount << endl;
  cerr << "remainNodeCount: " << remainNodeCount << endl;

  // 各ブランチの空きに、残りのノードを追加していく
  // (ブランチ, ノード)の組について、0から生成した場合の消費量 -
  // 追加した場合の消費量 が最大のノードを追加する
  vector<vector<double>> branchNode2EvalsCache(generatedBranches.size(),
                                               vector<double>(N, INF));
  while (true) {
    // 単体で追加するほうが偉いなら、マージしない
    double maxEval = -1e-5;
    int bestBranchIdx = -1;
    Node bestNode = Node(-1);
    for (int branchIdx = 0; branchIdx < (int)generatedBranches.size();
         branchIdx++) {
      auto branch = generatedBranches[branchIdx];
      if (branch.Size() >= LB) continue;
      for (const auto node : allVisitNodes) {
        if (isCheckedNode[node]) continue;
        if (branchNode2EvalsCache[branchIdx][node] == INF) {
          Branch onlyBranch(vector<Node>{centerNode, node});
          auto addedBranch = branch.CloneAdd(node);
          if (addedBranch.Size() > LB) continue;
          auto addedSize = addedBranch.Size() - branch.Size();
          branchNode2EvalsCache[branchIdx][node] =
              onlyBranch.Size() - addedSize;
        }
        const auto eval = branchNode2EvalsCache[branchIdx][node];
        if (chmax(maxEval, eval)) {
          bestBranchIdx = branchIdx;
          bestNode = node;
        }
      }
    }

    if (bestBranchIdx == -1){
      // 単体で追加
      // 一番サイズが大きいものを選ぶ
      int maxSize = -INF;
      for (const auto node : allVisitNodes) {
        if(isCheckedNode[node]) continue;
        Branch onlyBranch(vector<Node>{centerNode, node});
        if(onlyBranch.Size() > LB) continue;
        if(sizeSum + onlyBranch.Size() > LA) continue;
        if(chmax(maxSize, onlyBranch.Size())){
          bestNode = node;
        }
      }
      if(maxSize == -INF) break;

      generatedBranches.push_back(Branch(vector<Node>{centerNode, bestNode}));
      sizeSum += maxSize;
      isCheckedNode[bestNode] = true;
      remainNodeCount--;

      // キャッシュ追加・リセット
      branchNode2EvalsCache.push_back(vector<double>(N, INF));
      rep(i, (int)generatedBranches.size()) {
        branchNode2EvalsCache[i][bestNode] = INF;
      }

      continue;
    }
    sizeSum -= generatedBranches[bestBranchIdx].Size();
    generatedBranches[bestBranchIdx] =
        generatedBranches[bestBranchIdx].CloneAdd(bestNode);
    sizeSum += generatedBranches[bestBranchIdx].Size();
    isCheckedNode[bestNode] = true;
    remainNodeCount--;

    // キャッシュリセット
    rep(i, (int)generatedBranches.size()) {
      branchNode2EvalsCache[i][bestNode] = INF;
    }
    for (const auto node : allVisitNodes) {
      branchNode2EvalsCache[bestBranchIdx][node] = INF;
    }
  }

  cerr << endl;
  cerr << "sizeSum: " << sizeSum << endl;
  cerr << "branchCount: " << (int)generatedBranches.size() << endl;
  cerr << "remainNodeCount: " << remainNodeCount << endl;
  cerr << global_timer.ms() << endl;

  if (sizeSum > LA) {
    cerr << "LAを超えてしまいました" << endl;
    return vector<Node>();
  }

  if (remainNodeCount > 0) {
    cerr << "全ての頂点を追加できませんでした" << endl;
    return vector<Node>();
  }

  // ブランチをマージしてAを生成する
  // TODO: 1つのペアで2つ以上の連鎖を考慮する
  // TODO: A->B->Aを考慮する

  for (auto& branch : generatedBranches) {
    branch.InitDists();
  }

  // 各ブランチのマージ可能対象を求める
  vector<vector<int>> node2BranchIdxes(N);
  rep(i, (int)generatedBranches.size()) {
    for (const auto node : generatedBranches[i].GetAllNodes()) {
      node2BranchIdxes[node].push_back(i);
    }
  }

  // branchIdx, queryIdx, 自身の担当ノード
  vector<vector<pair<int, pair<int, Node>>>> branch2MergeCandidates(N);
  rep(queryIdx, T) {
    if (query2BranchIdxes[queryIdx] >= 0) continue;
    // 未マージのクエリについて、どの2つのブランチをマージすると良いかを求める
    const auto [node0, node1] = inputted_queries[queryIdx];
    for (const auto branchIdx0 : node2BranchIdxes[node0]) {
      for (const auto branchIdx1 : node2BranchIdxes[node1]) {
        if (branchIdx0 == branchIdx1) {
          cerr << "WARNING: ここに来るのはおかしい" << endl;
          continue;
        }
        // マージしても1手で移動できない
        if (generatedBranches[branchIdx0].GetDistFromBase(node0) +
                generatedBranches[branchIdx1].GetDistFromBase(node1) + 1 >
            LB) {
          continue;
        }
        // 各ブランチのマージ対象・そのクエリ・自身のノード
        branch2MergeCandidates[branchIdx0].emplace_back(
            branchIdx1, std::make_pair(queryIdx, node0));
        branch2MergeCandidates[branchIdx1].emplace_back(
            branchIdx0, std::make_pair(queryIdx, node1));
      }
    }
  }

  // branchIdx, queryで担当するノード
  vector<pair<int, Node>> mergeResults(generatedBranches.size(),
                                       {-1, Node(-1)});
  // マージ候補が少ないものからマージしていく
  while (true) {
    int bestMerge0 = -1;
    int bestMerge1 = -1;
    int bestQueryIdx = -1;
    Node bestNode0 = Node(-1);
    Node bestNode1 = Node(-1);
    double minEval = INF;
    rep(i, (int)generatedBranches.size()) {
      if (mergeResults[i].first != -1) continue;
      for (const auto [mergeBranchIdx, pa] : branch2MergeCandidates[i]) {
        const auto [queryIdx, node] = pa;
        if (mergeResults[mergeBranchIdx].first != -1) continue;
        // min(size0, size1)
        double eval =
            -log(exp(-(int)branch2MergeCandidates[i].size()) +
                 exp(-(int)branch2MergeCandidates[mergeBranchIdx].size()));
        if (chmin(minEval, eval)) {
          bestMerge0 = i;
          bestMerge1 = mergeBranchIdx;
          bestQueryIdx = queryIdx;
          bestNode0 = node;
          bestNode1 = inputted_queries[queryIdx].first == node
                          ? inputted_queries[queryIdx].second
                          : inputted_queries[queryIdx].first;
        }
      }
    }

    if (bestMerge0 == -1) break;

    mergeResults[bestMerge0] = {bestMerge1, bestNode0};
    mergeResults[bestMerge1] = {bestMerge0, bestNode1};
    // 候補から削除
    for (auto& candidates : branch2MergeCandidates) {
      for (auto itr = candidates.begin(); itr != candidates.end();) {
        if (itr->first == bestMerge0 || itr->first == bestMerge1) {
          itr = candidates.erase(itr);
        } else if (itr->second.first == bestQueryIdx) {
          itr = candidates.erase(itr);
        } else {
          itr++;
        }
      }
    }
  }

  // マージの結果を反映したうえで、Aに変換して返す
  vector<Node> A;
  int subMergeCount = 0;
  rep(i, (int)generatedBranches.size()) {
    const int anotherBranchIdx = mergeResults[i].first;
    // マージ先が無い場合や、自分自身にマージする場合は、そのまま追加
    if (anotherBranchIdx == -1 || anotherBranchIdx == i) {
      const auto& allNodes = generatedBranches[i].GetAllNodes();
      A.insert(A.end(), all(allNodes));
      continue;
    }

    // マージ先が既に追加済みなら、スキップ
    if (anotherBranchIdx < i) {
      continue;
    }

    // マージ先を反転してcenterを除いたものと、自身を、追加する
    const auto myBranch = generatedBranches[i];
    const auto myTo = mergeResults[i].second;
    const auto anotherBranch = generatedBranches[anotherBranchIdx];
    const auto anotherTo = mergeResults[anotherBranchIdx].second;
    auto flipAnotherA = anotherBranch.CreateAllNodesFirstIs(anotherTo);
    reverse(all(flipAnotherA));
    assert(flipAnotherA.back() == centerNode);
    flipAnotherA.pop_back();
    A.insert(A.end(), all(flipAnotherA));
    const auto myNodes = myBranch.CreateAllNodesFirstIs(myTo);
    A.insert(A.end(), all(myNodes));
    subMergeCount++;
  }

  cerr << endl;
  cerr << "mergeCount: " << mergeCount << endl;
  cerr << "subMergeCount: " << subMergeCount << endl;
  cerr << "ASize: " << (int)A.size() << " / " << LA << endl;
  return A;
}











/* start */

namespace BranchOld {

struct Branch{
  vector<Node> orders;
  vector<Node> allNodes;

  Branch(const vector<Node> orders):orders(orders){
    // 貪欲にシュタイナー木を構築
    allNodes = Prim(orders);
  }

  int MinDist(const Node order) const{
    int minDist = INF;
    for(const auto baseNode : allNodes){
      chmin(minDist, Dist(baseNode, order));
    }
    return minDist;
  }

  void AddOrder(const Node order){
    orders.push_back(order);
    int minDist = INF;
    Node minBaseNode = Node(-1);
    for(const auto baseNode : allNodes){
      const auto dist = Dist(baseNode, order);
      if(dist < minDist){
        minDist = dist;
        minBaseNode = baseNode;
      }
    }

    const auto minPath = CalcMinPath(minBaseNode, order);
    allNodes.insert(allNodes.end(), all(minPath));
  }

  int Size() const{
    return (int)allNodes.size();
  }

  const vector<Node>& GetAllNodes() const{
    return allNodes;
  }

  int GetIdx(Node node) const{
    rep(i, (int)allNodes.size()){
      if(allNodes[i] == node){
        return i;
      }
    }
    assert(false);
    return -1;
  }
};

vector<Node> BranchFromCenter() {
  // 中心からの距離が遠い順に追加していく
  vector<Node> sortedNodes(all(allVisitNodes));
  sort(all(sortedNodes), [&](const Node& a, const Node& b) {
    return Dist(centerNode, a) > Dist(centerNode, b);
  });

  vector<Branch> branches;
  vector<bool> hasAddedNode(N, false);
  for (const auto toNode : sortedNodes) {
    if (hasAddedNode[toNode]) {
      continue;
    }
    // 新たに枝を作成する
    Branch baseBranch(vector<Node>{centerNode, toNode});
    if ((int)baseBranch.Size() > LB - 1) {
      cerr << "LBに収まらないのでBranchをスキップします" << endl;
      continue;
    }

    hasAddedNode[toNode] = true;
    // その枝に追加するコストが最も低い頂点を、追加していく
    // コストが同じ場合は、中心から遠いほうを優先
    while ((int)baseBranch.Size() < LB) {
      int minSizeAfter = INF;
      Node bestNode = Node(-1);
      for (const auto addNode : sortedNodes) {
        if (hasAddedNode[addNode]) continue;
        const auto dist = baseBranch.MinDist(addNode);
        int sizeAfter = dist + baseBranch.Size();
        if (sizeAfter > LB) continue;
        if (chmin(minSizeAfter, sizeAfter)) {
          bestNode = addNode;
          // 無条件で追加
          if (dist == 0) break;
        }
      }

      if (minSizeAfter == INF) break;
      baseBranch.AddOrder(bestNode);
      hasAddedNode[bestNode] = true;
    }

    branches.push_back(baseBranch);
  }

  for (int t = 2; t <= 2; t++) {
    // 追加できなかった頂点を、他の枝に隣接させる形で置いていく
    for (const auto toNode : sortedNodes) {
      if (hasAddedNode[toNode]) continue;
      // 一番近い枝を探す
      int minDist = INF;
      int bestBranchIdx = -1;
      rep(i, (int)branches.size()) {
        const auto dist = branches[i].MinDist(toNode);
        // LB*tを超えると連結性が失われるので、それ以上は追加しない
        if (branches[i].Size() + dist > LB * t) continue;
        if (chmin(minDist, dist)) {
          bestBranchIdx = i;
        }
      }

      if (bestBranchIdx == -1) {
        cerr << t << "段目の枝が追加できないのでスキップします" << endl;
        continue;
      }

      // 一番近い枝に追加
      branches[bestBranchIdx].AddOrder(toNode);
      hasAddedNode[toNode] = true;
    }
  }

  rep(i, (int)sortedNodes.size()) {
    if (hasAddedNode[sortedNodes[i]]) continue;
    cerr << "全ての頂点を追加できませんでした" << endl;
    return vector<Node>();
  }

  int sizeSum = 0;
  for (const auto branch : branches) {
    sizeSum += branch.Size();
  }
  cerr << "sizeSum: " << sizeSum << endl;
  if ((int)sizeSum > LA) {
    cerr << "LAを超えてしまいました" << endl;
    return vector<Node>();
  }

  // ブランチのマージする対象を決める
  vector<vector<int>> node2BracnhIdxes(N);
  rep(i, (int)branches.size()) {
    for (const auto node : branches[i].GetAllNodes()) {
      node2BracnhIdxes[node].push_back(i);
    }
  }
  vector<int> mergePair(branches.size(), -1);
  for (const auto [node0, node1] : inputted_queries) {
    pair<int, int> bestPair = {-1, -1};
    for (const auto branchIdx0 : node2BracnhIdxes[node0]) {
      if (mergePair[branchIdx0] != -1) {
        continue;
      }
      const int idx0 = branches[branchIdx0].GetIdx(node0);
      for (const auto branchIdx1 : node2BracnhIdxes[node1]) {
        if (mergePair[branchIdx1] != -1) {
          continue;
        }
        // 既に見つかっていても、等しいほうが嬉しいので、確認する
        if (branchIdx0 == branchIdx1) {
          bestPair = {branchIdx0, branchIdx1};
          break;
        }
        if (bestPair.first != -1) break;
        const int idx1 = branches[branchIdx1].GetIdx(node1);

        // idx0への移動 + idx1への移動 + 1(中心) <=
        // LBなら、マージすると手数圧縮できる
        const int len = idx0 + idx1 + 1;
        if (len <= LB) {
          bestPair = {branchIdx0, branchIdx1};
          continue;
        }
      }
    }

    if (bestPair.first == -1) continue;
    const auto [branchIdx0, branchIdx1] = bestPair;
    mergePair[branchIdx0] = branchIdx1;
    mergePair[branchIdx1] = branchIdx0;
  }

  // ブランチをマージしてAを生成する
  vector<Node> A;
  int mergeCount = 0;
  rep(i, (int)branches.size()) {
    // マージ先が無い場合や、自分自身にマージする場合は、そのまま追加
    if (mergePair[i] == -1 || mergePair[i] == i) {
      A.insert(A.end(), all(branches[i].GetAllNodes()));
      continue;
    }

    // マージ先が既に追加済みなら、スキップ
    if (mergePair[i] < i) {
      continue;
    }
    mergeCount++;

    // マージ先を反転してcenterを除いたものと、自身を、追加する
    const auto branch = branches[i];
    const auto anotherBranch = branches[mergePair[i]];
    auto flipAnotherA = anotherBranch.GetAllNodes();
    reverse(all(flipAnotherA));
    assert(flipAnotherA.back() == centerNode);
    flipAnotherA.pop_back();
    A.insert(A.end(), all(flipAnotherA));
    A.insert(A.end(), all(branch.GetAllNodes()));
  }
  cerr << mergeCount << "回マージしました" << endl;

  return A;
}

}  // namespace BranchOld













/* start */

// 一直線
struct Line {
 private:
  vector<Node> nodes;
  bitset<600> visitBS = 0;

 public:
  Line() {}
  explicit Line(vector<Node> nodes) : nodes(nodes) {
    for (const auto node : nodes) {
      visitBS[node] = true;
    }
  }

  const vector<Node>& Nodes() const { return nodes; }

  const bitset<600>& GetVisitBS() const { return visitBS; }

  pair<Line, int> CreateAddLine(const Node to) const {
    // toまでの最短経路を求める
    Node bestFrom(-1);
    int bestIdx = -1;
    double minEval = INF;
    rep(i, Nodes().size()) {
      const auto from = Nodes()[i];
      const int dist = Dist(from, to);
      // 距離は小さいほうが良い
      // idxは小さいほうが良い
      double eval = dist + 1e-5 * i;
      if (chmin(minEval, eval)) {
        bestFrom = from;
        bestIdx = i;
      }
    }

    // bestFromからtoまでの最短経路を求める
    // GetVisitBS()は適当
    const auto newLine = Line::CreateFromTo(bestFrom, to, GetVisitBS());
    return {newLine, bestIdx};
  }

  static Line CreateFromTo(const Node from, const Node to,
                           bitset<600> visitedBS) {
    // fromからtoまでの最短経路であり、
    // 未通貨のノードを最も多く通る経路を
    // DPで求める
    // toのクエリに対応するノードを踏むと点数
    bitset<600> queryBonusBS = 0;
    for(const auto queryIdx : node2QueryIndexes[to]){
      const auto& query = inputtedQueries[queryIdx];
      if(query.from == to){
        queryBonusBS[query.to] = true;
      }else{
        assert(query.to == to);
        queryBonusBS[query.from] = true;
      }
    }
    vector<Node> prevNodes(N, Node(-1));
    vector<int> dp(N, -1);
    dp[from] = 0;
    queue<Node> que;
    que.push(from);
    int nextDist = 0;
    while (!que.empty()) {
      nextDist++;
      queue<Node> nextQue;
      while (!que.empty()) {
        const auto node = que.front();
        que.pop();
        for (const auto next : G.NextNodes(node)) {
          if (Dist(from, next) != nextDist) continue;
          // 未訪問なら1点
          int nextScore = dp[node] + !visitedBS[next] + queryBonusBS[next] * 10;
          if (chmax(dp[next], nextScore)) {
            prevNodes[next] = node;
            nextQue.push(next);
          }
        }
      }

      swap(que, nextQue);
    }

    // 復元
    vector<Node> nodes;
    Node currentNode = to;
    while (currentNode != from) {
      nodes.push_back(currentNode);
      currentNode = prevNodes[currentNode];
    }
    nodes.push_back(from);
    reverse(all(nodes));
    return Line(nodes);
  }

  static Line CreateFrom(const Node from, bitset<600> visitedBS) {
    // fromから寄り道しないで移動するとき、未通貨のノードを最も多く通る経路を
    // DPで求める
    vector<Node> prevNodes(N, Node(-1));
    vector<int> dp(N, -1);
    dp[from] = 0;
    queue<Node> que;
    que.push(from);
    int nextDist = 0;
    while (!que.empty()) {
      nextDist++;
      queue<Node> nextQue;
      while (!que.empty()) {
        const auto node = que.front();
        que.pop();
        for (const auto next : G.NextNodes(node)) {
          if (Dist(from, next) != nextDist) continue;
          // 未訪問なら1点
          int nextScore = dp[node] + !visitedBS[next];
          if (chmax(dp[next], nextScore)) {
            prevNodes[next] = node;
            nextQue.push(next);
          }
        }
      }

      swap(que, nextQue);
    }

    // 一番点数の高いdpを探す
    Node bestNode(-1);
    double maxScore = -1;
    rep(i, N) {
      double score = dp[i] - 1e-5 * Dist(from, Node(i));
      if (chmax(maxScore, score)) {
        bestNode = Node(i);
      }
    }

    // 復元
    vector<Node> nodes;
    Node currentNode = bestNode;
    while (currentNode != from) {
      nodes.push_back(currentNode);
      currentNode = prevNodes[currentNode];
    }
    nodes.push_back(from);
    reverse(all(nodes));
    return Line(nodes);
  }
};



/* start */

struct Goban {
 private:
  vector<Line> lines;
  vector<Node> otherNodes;

 public:
  Goban(vector<Line> lines, vector<Node> otherNodes)
      : lines(lines), otherNodes(otherNodes) {}

  const vector<Line>& Lines() const { return lines; }

  int ASize() const {
    int ret = 0;
    for (const auto line : lines) {
      ret += (int)line.Nodes().size();
    }
    ret += (int)otherNodes.size();
    return ret;
  }

  vector<Node> ToA() const {
    // TODO: なるべくシームレスになるように並び替える
    vector<vector<Node>> retNodes;
    auto remainOtherNodes = otherNodes;
    for (const auto line : lines) {
      retNodes.push_back(line.Nodes());
    }
    // otherNodesをlineと接続できるなら、する
    while (true) {
      bool updated = false;
      for (auto itr = remainOtherNodes.begin();
           itr != remainOtherNodes.end();) {
        bool found = false;
        for (auto& nodes : retNodes) {
          if (Dist(nodes.front(), *itr) == 1) {
            nodes.insert(nodes.begin(), *itr);
            found = true;
            break;
          }
          if (Dist(nodes.back(), *itr) == 1) {
            nodes.push_back(*itr);
            found = true;
            break;
          }
        }
        if (found) {
          updated = true;
          itr = remainOtherNodes.erase(itr);
        } else {
          ++itr;
        }
      }

      if (!updated) {
        break;
      }
    }

    // line同士をつなげられるならつなげる
    while (true) {
      bool updated = false;
      for (int i = 0; i < (int)retNodes.size(); ++i) {
        for (int j = i + 1; j < (int)retNodes.size(); ++j) {
          // 一致している場合
          if (retNodes[i].front() == retNodes[j].front()) {
            reverse(all(retNodes[j]));
            retNodes[j].pop_back();
            retNodes[i].insert(retNodes[i].begin(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (retNodes[i].front() == retNodes[j].back()) {
            retNodes[j].pop_back();
            retNodes[i].insert(retNodes[i].begin(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (retNodes[i].back() == retNodes[j].front()) {
            retNodes[i].pop_back();
            retNodes[i].insert(retNodes[i].end(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (retNodes[i].back() == retNodes[j].back()) {
            reverse(all(retNodes[j]));
            retNodes[i].pop_back();
            retNodes[i].insert(retNodes[i].end(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          // 隣接している場合
          if (Dist(retNodes[i].back(), retNodes[j].front()) == 1) {
            retNodes[i].insert(retNodes[i].end(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (Dist(retNodes[j].back(), retNodes[i].front()) == 1) {
            retNodes[j].insert(retNodes[j].end(), all(retNodes[i]));
            retNodes.erase(retNodes.begin() + i);
            updated = true;
            break;
          }
          if (Dist(retNodes[i].back(), retNodes[j].back()) == 1) {
            reverse(all(retNodes[j]));
            retNodes[i].insert(retNodes[i].end(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (Dist(retNodes[i].front(), retNodes[j].front()) == 1) {
            reverse(all(retNodes[i]));
            retNodes[j].insert(retNodes[j].end(), all(retNodes[i]));
            retNodes.erase(retNodes.begin() + i);
            updated = true;
            break;
          }
        }
        if (updated) {
          break;
        }
      }
      if (!updated) {
        break;
      }
    }

    while (true) {
      bool updated = false;
      for (int i = 0; i < (int)retNodes.size(); ++i) {
        for (int j = i + 1; j < (int)retNodes.size(); ++j) {
          // 距離が2離れている場合でも、一応つなげておく
          if (Dist(retNodes[i].back(), retNodes[j].front()) == 2) {
            retNodes[i].insert(retNodes[i].end(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (Dist(retNodes[j].back(), retNodes[i].front()) == 2) {
            retNodes[j].insert(retNodes[j].end(), all(retNodes[i]));
            retNodes.erase(retNodes.begin() + i);
            updated = true;
            break;
          }
          if (Dist(retNodes[i].back(), retNodes[j].back()) == 2) {
            reverse(all(retNodes[j]));
            retNodes[i].insert(retNodes[i].end(), all(retNodes[j]));
            retNodes.erase(retNodes.begin() + j);
            updated = true;
            break;
          }
          if (Dist(retNodes[i].front(), retNodes[j].front()) == 2) {
            reverse(all(retNodes[i]));
            retNodes[j].insert(retNodes[j].end(), all(retNodes[i]));
            retNodes.erase(retNodes.begin() + i);
            updated = true;
            break;
          }
        }
        if (updated) {
          break;
        }
      }
      if (!updated) {
        break;
      }
    }

    vector<Node> ret;
    for (const auto nodes : retNodes) {
      ret.insert(ret.end(), all(nodes));
    }
    ret.insert(ret.end(), all(remainOtherNodes));
    return ret;
  }

  static Goban Create() {
    // vector<Node> sortedNodes(all(allVisitNodes));
    // sort(all(sortedNodes), [&](const Node& a, const Node& b) {
    //   return Dist(centerNode, a) > Dist(centerNode, b);
    // });

    vector<Query> sortedQueries(all(inputtedQueries));
    sort(all(sortedQueries), [&](const Query& a, const Query& b) {
      return Dist(a.from, a.to) > Dist(b.from, b.to);
    });

    bitset<600> allVisited = 0;
    rep(i, N) {
      if (allVisitNodes.find(Node(i)) == allVisitNodes.end()) {
        // 訪問不要な頂点はフラグを立てておく
        allVisited[i] = true;
      }
    }

    vector<vector<Line>> lastValidLines;
    vector<int> lastValidASize;
    vector<bitset<600>> lastValidAllVisited;

    vector<Line> lines;
    unordered_map<int, Line> query2Lines;
    int ASize = 0;
    bool allClear = false;
    rep(t, 1000) {
      if ((int)allVisited.count() == N) {
        allClear = true;
      }
      Line bestLine;
      double maxScore = -INF;
      int bestQueryIdx = -1;
      for (const auto query : sortedQueries) {
        auto fromNode = query.from;
        auto toNode = query.to;
        if (!allClear && allVisited[fromNode] && allVisited[toNode]) {
          continue;
        }
        if (query2Lines.count(query.id) > 0) {
          continue;
        }

        // 折り返すのを見越す
        // TODO: prevもnextもある場合はどちらから決めるかによって結果が変わる
        const int prevQueryIdx = query.id - 1;
        const int nextQueryIdx = query.id + 1;
        if (prevQueryIdx >= 0 && query2Lines.count(prevQueryIdx) > 0) {
          const auto prevLine = query2Lines[prevQueryIdx];
          // size-LBからsize-1のLB個で到達するので、そのなかで最も近いものを選ぶ
          int minDist = INF;
          const auto& prevNodes = prevLine.Nodes();
          for (int i = max(0, (int)prevNodes.size() - LB);
               i < (int)prevNodes.size(); ++i) {
            if (chmin(minDist, Dist(prevNodes[i], toNode))) {
              fromNode = prevNodes[i];
            }
          }
        }

        if (nextQueryIdx < T && query2Lines.count(nextQueryIdx) > 0) {
          const auto nextLine = query2Lines[nextQueryIdx];
          // 0からLB-1のLB個で開始するので、そのなかで最も近いものを選ぶ
          int minDist = INF;
          const auto& nextNodes = nextLine.Nodes();
          for (int i = 0; i < min(LB, (int)nextNodes.size()); ++i) {
            if (chmin(minDist, Dist(fromNode, nextNodes[i]))) {
              toNode = nextNodes[i];
            }
          }
        }

        // fromNodeから線を引く
        auto line = Line::CreateFromTo(fromNode, toNode, allVisited);
        const auto newVisited = allVisited | line.GetVisitBS();
        if (allClear) {
          // 全頂点通過後は、なるべく長いクエリを選ぶ
          const double newScore = line.Nodes().size();
          if (chmax(maxScore, newScore)) {
            bestLine = line;
            bestQueryIdx = query.id;
          }
        } else {
          const double newScore =
              newVisited.count() - 1e-5 * line.Nodes().size();
          if (chmax(maxScore, newScore)) {
            bestLine = line;
            bestQueryIdx = query.id;
          }
        }
      }

      allVisited |= bestLine.GetVisitBS();
      lines.push_back(bestLine);
      ASize += bestLine.Nodes().size();
      query2Lines[bestQueryIdx] = bestLine;
      // cerr << "size = " << bestLine.Nodes().size()
      //      << " visitCount = " << allVisited.count() << endl;
      if (ASize > LA) {
        break;
      }
      {
        // 未訪問の頂点数 + 現在のASize + buffer <=
        // LAならvalidとして、保存しておく
        const int kBuffer = 0;
        if (N - (int)allVisited.count() + ASize + kBuffer <= LA) {
          // cerr << "valid" << endl;
          lastValidLines.push_back(lines);
          lastValidASize.push_back(ASize);
          lastValidAllVisited.push_back(allVisited);
        }
      }
    }

    while (lastValidLines.size()) {
      // 最後のvalidな解を使用する
      lines = lastValidLines.back();
      ASize = lastValidASize.back();
      allVisited = lastValidAllVisited.back();

      lastValidLines.pop_back();
      lastValidASize.pop_back();
      lastValidAllVisited.pop_back();

      // 未訪問頂点から訪問済み頂点までの最短距離の分布を確認する
      unordered_set<Node> visitedNodes;
      for (const auto line : lines) {
        for (const auto node : line.Nodes()) {
          visitedNodes.insert(node);
        }
      }

      // デバッグ
      vector<int> node2MinDists(N, INF);
      vector<Node> remainNodes;
      // map<int, int> dists;
      for (const auto node : allVisitNodes) {
        if (visitedNodes.count(node) > 0) {
          continue;
        }
        remainNodes.push_back(node);
        int minDist = INF;
        for (const auto visitedNode : visitedNodes) {
          chmin(minDist, Dist(node, visitedNode));
        }
        // dists[minDist]++;
        node2MinDists[node] = minDist;
      }

      // for (const auto [dist, count] : dists) {
      //   cerr << "dist = " << dist << " count = " << count << endl;
      // }

      // 余りのノードを追加
      // 未訪問頂点を中心から遠い順にソート
      // TODO: starと違い、中心である意味はない
      // 距離が同じなら、最短距離が大きい順
      sort(all(remainNodes), [&](const Node& a, const Node& b) {
        if (Dist(centerNode, a) != Dist(centerNode, b)) {
          return Dist(centerNode, a) > Dist(centerNode, b);
        }
        return node2MinDists[a] > node2MinDists[b];
      });

      vector<Node> otherNodes;
      for (const auto node : remainNodes) {
        if (allVisited[node]) {
          continue;
        }
        // 追加する必要のある線の長さが短い対象に追加していく
        int bestIdx = -1;
        double minEval = INF;
        rep(i, (int)lines.size()) {
          const auto [addedLine, _] = lines[i].CreateAddLine(node);
          const double eval = addedLine.Nodes().size();
          if (chmin(minEval, eval)) {
            bestIdx = i;
          }
        }

        const auto [addedLine, _] = lines[bestIdx].CreateAddLine(node);
        allVisited |= addedLine.GetVisitBS();
        auto nodes = addedLine.Nodes();
        nodes.erase(nodes.begin());
        otherNodes.insert(otherNodes.end(), all(nodes));
        ASize += nodes.size();
      }
      remainNodes.clear();

      cerr << "ASize = " << ASize << endl;
      if (ASize > LA) {
        continue;
      }
      return Goban(lines, otherNodes);
    }

#ifdef LOCAL
    // ここには到達しない想定
    cerr << "score: 1000000000000" << endl;
    exit(0);
#endif

    return Goban(lines, {});
  }
};

/* start */

vector<Node> CreateGoban() {
  auto goban = Goban::Create();
  cerr << "ASize: " << goban.ASize() << endl;
  if(goban.ASize() > LA){
    cerr << "LAを超えています" << endl;
    return vector<Node>();
  }
  return goban.ToA();
}












/* start */

struct AddedLine {
  // 元となるline[baseIdx]から伸ばす
  int baseIdx;
  Line line;
};

struct DequeLine {
 private:
  Line line;
  vector<AddedLine> addedLines;
  bitset<600> visitBS = 0;

 public:
  DequeLine(Line line) : line(line), visitBS(line.GetVisitBS()) {}

  const Line& GetLine() const { return line; }

  const vector<AddedLine>& GetAddedLines() const { return addedLines; }

  int ASize() const {
    int ret = line.Nodes().size();
    for (const auto addedLine : addedLines) {
      ret += (int)addedLine.line.Nodes().size() - 1;
    }
    return ret;
  }

  vector<Node> ToA() const {
    vector<Node> ret(line.Nodes());
    int addedCount = 0;
    // baseIndexの大きいものから前に追加していく
    // TODO: 要素数が多いものは後回し
    // NOTE: 基本的には要素数は1になるはずなので、気にしなくてもよいかも
    auto addedLines = this->addedLines;
    sort(all(addedLines), [&](const AddedLine& a, const AddedLine& b) {
      return a.baseIdx < b.baseIdx;
    });
    vector<bool> hasAdded(addedLines.size(), false);
    REP(i, addedLines.size()) {
      if (hasAdded[i]) continue;
      const auto& addedLine = addedLines[i];
      auto nodes = addedLine.line.Nodes();
      // 最初は重複するので除外している
      nodes.erase(nodes.begin());
      // 前に追加しても届かないならスキップ
      if (addedLine.baseIdx + 1 + (int)nodes.size() + addedCount > LB) {
        continue;
      }

      // 前に追加
      ret.insert(ret.begin(), all(nodes));
      addedCount += (int)nodes.size();
      hasAdded[i] = true;
    }

    // 残りを後ろに追加
    REP(i, addedLines.size()) {
      if (hasAdded[i]) continue;
      const auto& addedLine = addedLines[i];
      auto nodes = addedLine.line.Nodes();
      // 最初は重複するので除外している
      nodes.erase(nodes.begin());
      // 後ろに追加
      ret.insert(ret.end(), all(nodes));
      hasAdded[i] = true;
    }
    return ret;
  }

  AddedLine CreateAddLine(const Node to) const {
    const auto [newLine, baseIdx] = line.CreateAddLine(to);
    return {baseIdx, newLine};
  }

  void AddAddedLine(const AddedLine addedLine) {
    visitBS |= addedLine.line.GetVisitBS();
    addedLines.push_back(addedLine);
  }
};

struct Star {
 private:
  // 中心からの線分
  vector<DequeLine> dequeLines;

 public:
  Star(const vector<DequeLine>& dequeLines) : dequeLines(dequeLines) {}

  const vector<DequeLine>& DequeLines() const { return dequeLines; }

  int ASize() const {
    int ret = 0;
    for (const auto& dequeLine : dequeLines) {
      ret += dequeLine.ASize();
    }
    return ret;
  }

  vector<Node> ToA() const {
    vector<Node> ret;
    for (const auto& dequeLine : dequeLines) {
      const auto nodes = dequeLine.ToA();
      ret.insert(ret.end(), all(nodes));
    }
    return ret;
  }

  static Star CreateFromCenter() {
    vector<Node> sortedNodes(all(allVisitNodes));
    sort(all(sortedNodes), [&](const Node& a, const Node& b) {
      return Dist(centerNode, a) > Dist(centerNode, b);
    });

    bitset<600> allVisited = 0;
    rep(i, N) {
      if (allVisitNodes.find(Node(i)) == allVisitNodes.end()) {
        // 訪問不要な頂点はフラグを立てておく
        allVisited[i] = true;
      }
    }

    vector<vector<Line>> lastValidLines;
    vector<int> lastValidASize;
    vector<bitset<600>> lastValidAllVisited;

    vector<Line> lines;
    int ASize = 0;
    rep(t, 1000) {
      if ((int)allVisited.count() == N) {
        // TODO: それでも引き続けるべき
        break;
      }
      Line bestLine;
      double maxScore = -INF;
      for (const auto toNode : sortedNodes) {
        if (allVisited[toNode]) {
          continue;
        }
        // 中心からtoNodeまでの線を引く
        for (const auto center : centerNodes) {
          auto line = Line::CreateFromTo(center, toNode, allVisited);
          const auto newVisited = allVisited | line.GetVisitBS();
          const double newScore =
              newVisited.count() - 1e-5 * line.Nodes().size();
          if (chmax(maxScore, newScore)) {
            bestLine = line;
          }
        }
      }

      allVisited |= bestLine.GetVisitBS();
      lines.push_back(bestLine);
      ASize += bestLine.Nodes().size();
      // cerr << "size = " << bestLine.Nodes().size()
      //      << " visitCount = " << allVisited.count() << endl;
      if (ASize > LA) {
        break;
      }
      {
        // 未訪問の頂点数 + 現在のASize + buffer <=
        // LAならvalidとして、保存しておく
        const int kBuffer = 0;
        if (N - (int)allVisited.count() + ASize + kBuffer <= LA) {
          // cerr << "valid" << endl;
          lastValidLines.push_back(lines);
          lastValidASize.push_back(ASize);
          lastValidAllVisited.push_back(allVisited);
        }
      }
    }

    while (lastValidLines.size()) {
      // 最後のvalidな解を使用する
      lines = lastValidLines.back();
      ASize = lastValidASize.back();
      allVisited = lastValidAllVisited.back();
      lastValidLines.pop_back();
      lastValidASize.pop_back();
      lastValidAllVisited.pop_back();

      // 未訪問頂点から訪問済み頂点までの最短距離の分布を確認する
      unordered_set<Node> visitedNodes;
      for (const auto line : lines) {
        for (const auto node : line.Nodes()) {
          visitedNodes.insert(node);
        }
      }

      // デバッグ
      vector<int> node2MinDists(N, INF);
      vector<Node> remainNodes;
      // map<int, int> dists;
      for (const auto node : allVisitNodes) {
        if (visitedNodes.count(node) > 0) {
          continue;
        }
        remainNodes.push_back(node);
        int minDist = INF;
        for (const auto visitedNode : visitedNodes) {
          chmin(minDist, Dist(node, visitedNode));
        }
        // dists[minDist]++;
        node2MinDists[node] = minDist;
      }

      // for (const auto [dist, count] : dists) {
      //   cerr << "dist = " << dist << " count = " << count << endl;
      // }

      // 未訪問頂点を、DequeLineに追加していく
      vector<DequeLine> dequeLines;
      for (const auto line : lines) {
        dequeLines.push_back(DequeLine(line));
      }

      // 未訪問頂点を中心から遠い順にソート
      // 距離が同じなら、最短距離が大きい順
      sort(all(remainNodes), [&](const Node& a, const Node& b) {
        if (Dist(centerNode, a) != Dist(centerNode, b)) {
          return Dist(centerNode, a) > Dist(centerNode, b);
        }
        return node2MinDists[a] > node2MinDists[b];
      });

      for (const auto node : remainNodes) {
        if (allVisited[node]) {
          continue;
        }
        // 追加する必要のある線の長さが短い対象に追加していく
        // 追加する線の長さが同じ場合には、ASizeが小さいものを選択
        int bestIdx = -1;
        double minEval = INF;
        rep(i, (int)dequeLines.size()) {
          const auto addedLine = dequeLines[i].CreateAddLine(node);
          const double eval =
              addedLine.line.Nodes().size() + 1e-5 * dequeLines[i].ASize();
          if (chmin(minEval, eval)) {
            bestIdx = i;
          }
        }

        const auto addedLine = dequeLines[bestIdx].CreateAddLine(node);
        allVisited |= addedLine.line.GetVisitBS();
        dequeLines[bestIdx].AddAddedLine(addedLine);
        ASize += (int)addedLine.line.Nodes().size() - 1;
      }
      remainNodes.clear();

      cerr << "ASize = " << ASize << endl;
      if(ASize > LA){
        continue;
      }
      return Star(dequeLines);
    }

    // ここには到達しない想定
#ifdef LOCAL
    // cerr << global_timer.ms() << "[ms]" << endl;
    // cerr << "score: 1000000000000000" << endl;
    // exit(0);
#endif
    return Star({});
  }
};

/* start */

vector<Node> CreateStar() {
  auto star = Star::CreateFromCenter();
  cerr << "ASize: " << star.ASize() << endl;
  if(star.ASize() > LA){
    cerr << "LAを超えています" << endl;
    return vector<Node>();
  }
  return star.ToA();
}





/* start */

class Solver {
 public:
  Solver() {}

  vector<Node> AddKanjosenA(const vector<Node>& originalA, const int r) {
    vector<Node> A = originalA;
    vector<Pos> basePoses = {Pos(r, r), Pos(r, 1000 - r),
                             Pos(1000 - r, 1000 - r), Pos(1000 - r, r)};
    rep(i, (int)basePoses.size()) {
      const auto fromPos = basePoses[i];
      const auto toPos = basePoses[(i + 1) % basePoses.size()];
      const auto from = CalcNearestNode(fromPos);
      const auto to = CalcNearestNode(toPos);
      const auto path = CalcMinPath(from, to);
      A.insert(A.end(), all(path));
    }
    return A;
  }

  vector<Node> AddTSPResult(const vector<Node>& originalA,
                            const vector<Node>& tspResult,
                            WeightedGraph<Node>* WG) {
    assert((int)originalA.size() == 0);
    vector<Node> A = originalA;
    rep(i, (int)tspResult.size()) {
      const auto from = tspResult[i];
      const auto to = tspResult[(i + 1) % tspResult.size()];
      const auto path = CalcMinPath(from, to);
      A.insert(A.end(), all(path));
      if ((int)A.size() > LA) {
        cerr << "AのサイズがLAを超えました" << endl;
        while ((int)A.size() > LA) {
          A.pop_back();
        }
        return A;
      }
    }

    // グラフにも反映
    if (WG != nullptr) {
      for (int i = 0; i < (int)A.size(); i++) {
        // [i-LB, i+LB]にコストLBで動ける
        for (int j = i - LB; j <= i + LB; j++) {
          // トーラス
          const int jjj = Mod(j, (int)A.size());
          WG->add_edge(A[i], A[jjj], LB);
        }

        // 周囲も更新
        for (const auto adjNode : G.NextNodes(A[i])) {
          for (int j = i - LB + 1; j < i + LB; j++) {
            // トーラス
            const int jjj = Mod(j, (int)A.size());
            WG->add_edge(adjNode, A[jjj], LB);
          }
        }
      }
    }
#ifdef LOCAL
    for (const auto a : A) {
      cerr << a.id << " ";
    }
    cerr << endl;
#endif
    return A;
  }

  vector<Node> AddPath(const vector<Node>& originalA, vector<Node> path,
                       WeightedGraph<Node>* WG, AllDijkstra* allDijkstra) {
    vector<Node> A = originalA;
    while ((int)A.size() + (int)path.size() > LA) {
      path.pop_back();
    }

    A.insert(A.end(), all(path));
    if (WG != nullptr) {
      rep(i, path.size()) {
        // pathの周囲のノードは、(i-LB, i+LB)にコストLBで動ける
        auto adjNodes = G.NextNodes(path[i]);
        for (const auto adjNode : adjNodes) {
          for (int j = max(0, i - LB + 1); j < min((int)path.size(), i + LB);
               j++) {
            WG->add_edge(adjNode, path[j], LB);
            if (allDijkstra != nullptr) {
              allDijkstra->AddEdge(adjNode, path[j], LB);
            }
          }
        }
      }

      // pathは、[i-LB, i+LB]にコストLBで動ける
      rep(i, path.size()) {
        for (int j = max(0, i - LB); j < min((int)path.size(), i + LB + 1);
             j++) {
          WG->add_edge(path[i], path[j], LB);
          if (allDijkstra != nullptr) {
            allDijkstra->AddEdge(path[i], path[j], LB);
          }
        }
      }
    }
    return A;
  }

  vector<Node> AddNothingOptimalA(const vector<Node>& originalA,
                                  bool ignoreNoVistA) {
    vector<Node> A = originalA;

    set<Node> ASet(all(A));
    int nothingCount = 0;
    rep(i, N) {
      if (ASet.count(Node(i))) continue;
      if (ignoreNoVistA && allVisitNodes.count(Node(i)) == 0) continue;
      nothingCount++;
      ASet.insert(Node(i));
      // 前後との距離が最小になる位置に入れる
      int minDist = INF;
      int bestPos = -1;
      rep(j, (int)A.size() + 1) {
        int dist = 0;
        dist +=
            pow(Dist(A[(j - 1 + (int)A.size()) % (int)A.size()], Node(i)), 2);
        dist += pow(Dist(Node(i), A[j % (int)A.size()]), 2);
        if (chmin(minDist, dist)) {
          bestPos = j;
        }
      }
      A.insert(A.begin() + bestPos, Node(i));
    }
    cerr << "nothingCount: " << nothingCount << endl;
    assert((int)A.size() <= LA);
    // int idx = 0;
    // while ((int)A.size() < LA && idx < LB - 1) {
    //   // トーラスにすることで逆走をシームレスにする
    //   A.push_back(A[idx++]);
    // }
    return A;
  }

  vector<Node> AddStraightLine(const vector<Node>& originalA, Pos pos0,
                               Pos pos1) {
    vector<Node> A = originalA;
    const auto node0 = CalcNearestNode(pos0, allVisitNodes);
    const auto node1 = CalcNearestNode(pos1, allVisitNodes);
    auto path = CalcMinPath(node0, node1);
    reverse(all(path));
    path.push_back(node0);
    while ((int)A.size() + (int)path.size() > LA) {
      path.pop_back();
      reverse(all(path));
    }
    A.insert(A.end(), all(path));
    return A;
  }

  vector<Node> AddDirectSen(const vector<Node>& originalA) {
    auto A = originalA;
    // 現在の環状線上での距離の差が最も大きいクエリについて、その間に直通線を引く
    vector<bool> isCheckedQuery(T, false);
    while ((int)A.size() < LA) {
      unordered_map<Node, vector<int>> node2Indexes;
      rep(i, (int)originalA.size()) { node2Indexes[originalA[i]].push_back(i); }
      int maxDiff = -1;
      int bestQueryIdx = -1;
      for (int i = 0; i < (int)inputted_queries.size(); i++) {
        if (isCheckedQuery[i]) continue;
        const auto query = inputted_queries[i];
        const auto from = query.first;
        const auto to = query.second;

        int diff = INF;
        for (const auto fromIdx : node2Indexes[from]) {
          for (const auto toIdx : node2Indexes[to]) {
            chmin(diff, abs(fromIdx - toIdx));
            chmin(diff, abs(fromIdx + N - toIdx));
          }
        }
        if (chmax(maxDiff, diff)) {
          bestQueryIdx = i;
        }
      }
      const auto path = CalcMinPath(inputted_queries[bestQueryIdx].first,
                                    inputted_queries[bestQueryIdx].second);
      isCheckedQuery[bestQueryIdx] = true;
      int idx = 0;
      while (idx < (int)path.size() && (int)A.size() < LA) {
        A.push_back(path[idx++]);
      }
    }

    return A;
  }

  vector<Node> AddNoops(const vector<Node>& originalA) {
    vector<Node> A = originalA;
    while ((int)A.size() < LA) {
      A.push_back(*allVisitNodes.begin());
    }

    // 連結にする
    set<Node> ASet(all(A));
    vector<Node> addA;
    bool primed = false;
    while (true) {
      dsu D(N);
      for (const auto node : ASet) {
        for (const auto adjNode : G.NextNodes(node)) {
          if (ASet.count(adjNode) == 0) continue;
          D.merge(node.id, adjNode.id);
        }
      }
      // 最大連結成分を取得
      auto groups = D.groups();
      sort(all(groups),
           [](const auto& a, const auto& b) { return a.size() > b.size(); });
      const int mainLeader = D.leader(groups[0][0]);

      // 訪問するグループ
      unordered_set<int> visitGroups;
      for (const auto node : allVisitNodes) {
        visitGroups.insert(D.leader(node.id));
      }

      // 訪問するグループが1つにまとまってるなら終わり
      if (visitGroups.size() == 1) {
        break;
      }

      primed = true;
      // mainグループと最短距離でつなぐ(プリム法ベース)
      int minDist = INF;
      Node bestFrom = Node(-1);
      Node bestTo = Node(-1);
      rep(i, N) {
        if (D.leader(i) != mainLeader) continue;
        const auto from = Node(i);
        rep(j, N) {
          if (D.leader(j) == mainLeader) continue;
          if (visitGroups.count(D.leader(j)) == 0) continue;
          const auto to = Node(j);
          const auto dist = Dist(from, to);
          if (chmin(minDist, dist)) {
            bestFrom = from;
            bestTo = to;
          }
        }
      }

      auto path = CalcMinPath(bestFrom, bestTo);
      // bestToが既に含まれてるなら、不要
      if (ASet.count(path.back()) > 0) {
        path.pop_back();
      }

      for (const auto p : path) {
        assert(ASet.count(p) == 0);
        addA.push_back(p);
        ASet.insert(p);
      }
    }

    if (primed) {
      cerr << "全ての頂点が連結でないので、プリム法ベースで連結させました"
           << endl;
    }

    // 追加すべきノードがあるなら、削除してもいい後ろのノードと入れ替える
    int idx = (int)A.size() - 1;
    unordered_map<Node, int> node2Counts;
    for (const auto a : A) {
      node2Counts[a]++;
    }
    for (const auto a : addA) {
      while (node2Counts[A[idx]] == 1) {
        idx--;
        assert(idx >= 0);
      }
      assert(idx >= 0);
      node2Counts[A[idx]]--;
      A[idx] = a;
      node2Counts[a]++;
      idx--;
    }

    return A;
  }

  void Debug(AObj A) {
    // Node(0)を末尾に持ってくる
    rep(i, LA) {
      if (A.A[i].id == 0) {
        AObj newAObj(vector<Node>(A.A.begin() + i, A.A.end()));
        newAObj.A.insert(newAObj.A.end(), A.A.begin(), A.A.begin() + i);
        A = newAObj;
        break;
      }
    }

    for (const auto a : A.A) {
      cout << a.id << " ";
    }
    cout << endl;

    reps(i, LA - 1) {
      cout << "s 1 " << i << " " << 0 << endl;
      cout << "m " << A.A[i].id << endl;
    }
    quick_exit(0);
  }

  vector<Node> Tekito() {
    vector<Node> A;
    vector<vector<int>> lines = {
        {0, 1000, 0, 1000},  {0, 1000, 1000, 0},  {0, 1000, 500, 500},
        {500, 500, 0, 1000}, {0, 1000, 250, 250}, {0, 1000, 750, 750},
        {250, 250, 0, 1000}, {750, 750, 0, 1000}, {250, 750, 0, 1000},
        {750, 250, 0, 1000}, {0, 1000, 250, 750}, {0, 1000, 750, 250}};
    for (const auto line : lines) {
      A = AddStraightLine(A, Pos(line[0], line[2]), Pos(line[1], line[3]));
    }

    unordered_set<Node> existNodes(all(A));
    // 追加済みノードまでの最短距離, ノード
    vector<pair<int, Node>> remainNodes;
    for (const auto node : allVisitNodes) {
      if (existNodes.count(node)) continue;
      int minDist = INF;
      for (const auto otherNode : existNodes) {
        chmin(minDist, Dist(node, otherNode));
      }
      remainNodes.emplace_back(minDist, node);
    }

    // 遠いものから順に追加
    sort(all(remainNodes), greater<pair<int, Node>>());
    for (const auto [dist, node] : remainNodes) {
      int minDist = INF;
      Node bestOtherNode = Node(-1);
      // 最も距離が短いものを探す
      for (const auto otherNode : existNodes) {
        const auto dist = Dist(otherNode, node);
        if (chmin(minDist, dist)) {
          bestOtherNode = otherNode;
        }
      }

      assert(minDist != INF);
      // 最も距離が短いものからのパスを追加
      const auto path = CalcMinPath(bestOtherNode, node);
      A.insert(A.end(), all(path));
      existNodes.insert(all(path));
    }

    if ((int)A.size() > LA) {
      cerr << "AのサイズがLAを超えました" << endl;
      return vector<Node>();
    }

    // A = AddDirectSen(A);

    return A;
  }

  // 遠いクエリから順に繋いでいく
  vector<Node> LongGreedy() {
    vector<Node> A;
    // オーバーフローしない程度のINFで初期化
    const int infinity = 1e6;
    vector<vector<int>> dists(N, vector<int>(N, (int)infinity));
    rep(i, N) { dists[i][i] = 0; }

    unordered_set<Node> existNodes;
    vector<bool> isCheckedQuery(T, false);
    rep(_t, T) {
      // 最も遠いクエリをつなぐ
      int maxDist = -1;
      int bestQueryIdx = -1;
      rep(i, T) {
        if (isCheckedQuery[i]) continue;
        const auto query = inputted_queries[i];
        const auto from = query.first;
        const auto to = query.second;
        if (dists[from][to] < infinity) {
          isCheckedQuery[i] = true;
          continue;
        }
        if (dists[from][to] > maxDist) {
          maxDist = dists[from][to];
          bestQueryIdx = i;
        }
      }

      if (bestQueryIdx == -1) {
        break;
      }

      isCheckedQuery[bestQueryIdx] = true;
      const auto bestQuery = inputted_queries[bestQueryIdx];

      // どこを経由するのが一番近いか確認する
      int bestFromKeiyu = -1;
      int bestToKeiyu = -1;
      int minDist = maxDist;
      rep(fromKeiyu, N) {
        rep(toKeiyu, N) {
          if (fromKeiyu == toKeiyu) continue;
          const auto from = bestQuery.first;
          const auto to = bestQuery.second;
          const int dist = dists[from][fromKeiyu] + dists[fromKeiyu][toKeiyu] +
                           dists[toKeiyu][to];
          if (chmin(minDist, dist)) {
            bestFromKeiyu = fromKeiyu;
            bestToKeiyu = toKeiyu;
          }
        }
      }

      vector<vector<Node>> pathes;

      if (bestFromKeiyu == -1) {
        // 経由が不要な場合は、直接のパスを生成
        auto path = CalcMinPath(bestQuery.first, bestQuery.second);
        path.insert(path.begin(), bestQuery.first);
        pathes.push_back(path);
        existNodes.insert(all(path));
      } else {
        // 経由する場合は、それぞれ生成
        auto pathFirst = CalcMinPath(bestQuery.first, Node(bestFromKeiyu));
        // TODO: 先頭への追加は本当に必要か再検討する
        pathFirst.insert(pathFirst.begin(), bestQuery.first);
        pathes.push_back(pathFirst);
        existNodes.insert(all(pathFirst));

        auto pathSecond =
            CalcMinPath(Node(bestToKeiyu), Node(bestQuery.second));
        pathSecond.insert(pathSecond.begin(), Node(bestToKeiyu));
        pathes.push_back(pathSecond);
        existNodes.insert(all(pathSecond));
      }

      unordered_set<int> updatedNodes;
      // パスを追加し、距離を更新
      for (const auto path : pathes) {
        A.insert(A.end(), all(path));
        rep(i, (int)path.size() - 1) {
          dists[path[i].id][path[i + 1].id] = 1;
          dists[path[i + 1].id][path[i].id] = 1;
          updatedNodes.insert(path[i].id);
          updatedNodes.insert(path[i + 1].id);
        }
        // 関節点の接続
        for (const auto node : path) {
          for (const auto adjNode : G.NextNodes(node)) {
            if (existNodes.count(adjNode) == 0) continue;
            // TODO: LBで良いか再確認
            chmin(dists[node.id][adjNode.id], LB);
            chmin(dists[adjNode.id][node.id], LB);
            updatedNodes.insert(adjNode.id);
          }
        }
      }

      // 更新されたノードを経由するパターンについて、他のノードとの距離を更新
      for (const auto k : updatedNodes) {
        rep(i, N) {
          rep(j, N) { chmin(dists[i][j], dists[i][k] + dists[k][j]); }
        }
      }
    }

    if ((int)A.size() > LA) {
      cerr << "LongGreedyでAのサイズがLAを超えました: " << A.size() << endl;
      return vector<Node>();
    }
    return A;
  }

  vector<Node> SA() {
    const auto nodes = NBD3::Run(1000, 1.0, 0.01);
    vector<Node> A;
    WeightedGraph<Node> WG(N, G, INF);
    A = AddTSPResult(A, nodes, &WG);
    cerr << "TSP len: " << A.size() << endl;

    // 通常の移動コストを1とし、Aで直接つながっていないノード間の距離をLBとし、Aが存在しない移動をINFとする
    AllDijkstra allDijkstra(WG, allFromNodes);

#if 0
    // テスト
    rep(i, N/2){
      Node from(i);
      Node to(i+1);
      WG.add_edge(from, to, 0);
      WG.add_edge(to, from, 0);
      allDijkstra.AddEdge(from, to, 0);
      allDijkstra.AddEdge(to, from, 0);
    }
    allDijkstra.UpdateDists(WG);

    for(const auto query : inputted_queries){
      cerr << "query: " << query.first.id << " " << query.second.id << " dist: " << allDijkstra.Dist(query.first, query.second) << endl;
    }
#endif

    // LB*2 + 1個置きにノードを選んで、TSP
    // vector<Node> kaisokuNodes;
    // for(int i = 0; i < (int)A.size(); i += LB * 2 + 1){
    //   kaisokuNodes.push_back(A[i]);
    // }
    // const auto kaisokuTSPResults = NBD5::Run(1000, kaisokuNodes);
    // A = AddTSPResult(A, kaisokuTSPResults);

    // from_x, to_x, from_y, to_y
    // vector<vector<int>> lines = {
    //     {0, 1000, 0, 1000},  {0, 1000, 1000, 0},  {0, 1000, 500, 500},
    //     {500, 500, 0, 1000}, {0, 1000, 250, 250}, {0, 1000, 750, 750},
    //     {250, 250, 0, 1000}, {750, 750, 0, 1000}, {250, 750, 0, 1000},
    //     {750, 250, 0, 1000}, {0, 1000, 250, 750}, {0, 1000, 750, 250}};
    // vector<vector<int>> lines = {
    //     {0, 1000, 0, 1000},  {0, 1000, 1000, 0},  {0, 1000, 500, 500},
    //     {500, 500, 0, 1000}};

    // for (const auto line : lines) {
    //   const auto node0 = CalcNearestNode(Pos(line[0], line[2]),
    //   allVisitNodes); const auto node1 = CalcNearestNode(Pos(line[1],
    //   line[3]), allVisitNodes); auto path = CalcMinPath(node0, node1);
    //   path.insert(path.begin(), node0);
    //   A = AddPath(A, path, &WG, &allDijkstra);
    // }
    // allDijkstra.UpdateDists(WG);
    // cerr << "Straight len: " << A.size() << endl;

    // 一番遠いクエリから順に繋いでいく
    while ((int)A.size() < LA) {
      double maxEval = -1;
      int bestQueryIdx = -1;
      rep(i, T) {
        const auto currentDist = allDijkstra.Dist(inputted_queries[i].first,
                                                  inputted_queries[i].second);
        const auto idealDist =
            Dist(inputted_queries[i].first, inputted_queries[i].second);

        // double eval = (double)(Ceil(currentDist, LB) - Ceil(idealDist, LB)) /
        // pathLen;
        double eval = currentDist - idealDist;
        if (chmax(maxEval, eval)) {
          bestQueryIdx = i;
        }
      }

      assert(bestQueryIdx >= 0);

      cerr << bestQueryIdx << " " << inputted_queries[bestQueryIdx].first.id
           << " " << inputted_queries[bestQueryIdx].second.id << " " << maxEval
           << " "
           << Dist(inputted_queries[bestQueryIdx].first,
                   inputted_queries[bestQueryIdx].second)
           << endl;

      unordered_set<Node> existNodes(all(A));
      unordered_map<Node, int> node2Priorities;
      for (const auto node : allVisitNodes) {
        if (existNodes.count(node) == 0) {
          node2Priorities[node]++;
        }
      }
      auto path = CalcBestMinPath(inputted_queries[bestQueryIdx].first,
                                  inputted_queries[bestQueryIdx].second,
                                  node2Priorities);
      path.insert(path.begin(), inputted_queries[bestQueryIdx].first);
      A = AddPath(A, path, &WG, &allDijkstra);
      allDijkstra.UpdateDists(WG);
    }
    cerr << "Path len: " << A.size() << endl;

    A = AddDirectSen(A);
    cerr << "Direct len: " << A.size() << endl;
    return A;
  }

  vector<Node> ManyStainers() {
    vector<Node> A;

    // 4つの代表点を取り、その代表点から伸びるシュタイナー木を作成する
    vector<Pos> basePoses = {Pos(150, 150), Pos(150, 850), Pos(850, 850),
                             Pos(850, 150)};
    vector<Node> baseNodes;
    for (const auto pos : basePoses) {
      baseNodes.push_back(CalcNearestNode(pos));
    }

    // 代表点間の最短パスを追加する
    for (int i = 0; i < 4; i++) {
      const auto from = baseNodes[i];
      const auto to = baseNodes[(i + 1) % 4];
      const auto path = CalcMinPath(from, to);
      A.insert(A.end(), all(path));
    }

    // 代表点からシュタイナー木を作成する
    vector<Node> visitOrderBaseNodes;
    rep(t, T + 1) {
      const auto queryNode = QueryNode::CreateFromVisitOrder(t);
      Node bestBaseNode = Node(-1);
      int minDist = INF;
      for (const auto baseNode : baseNodes) {
        const auto dist = Dist(queryNode.GetNode(), baseNode);
        if (chmin(minDist, dist)) {
          bestBaseNode = baseNode;
        }
      }
      visitOrderBaseNodes.push_back(bestBaseNode);
    }

    vector<Node> branchBaseNodes;
    for (const auto baseNode : baseNodes) {
      rep(_t, Ceil(LA, LB * 4)) { branchBaseNodes.push_back(baseNode); }
    }

    LA -= A.size();
    auto allBranchState =
        AllBranchState::CreateGreedy(branchBaseNodes, visitOrderBaseNodes);
    cerr << allBranchState.Eval() << endl;
    if (!allBranchState.IsValid()) {
      cerr << "score: 1000000000" << endl;
      exit(0);
      return vector<Node>();
    }
    LA += A.size();

    const auto addA = NBDBranch::Run(2000, allBranchState);

    A.insert(A.end(), all(addA));

    A = AddNoops(A);

    return A;
  }

  vector<Node> AroundSA() {
    set<Node> targetNodesSet;
    set<Node> notTargetNodesSet;
    for (const auto node : allVisitNodes) {
      if (Dist(node, centerNode) >= LB - 7) {
        targetNodesSet.insert(node);
      } else {
        notTargetNodesSet.insert(node);
      }
    }
    const auto tspResult =
        NBD3_5::Run(1000, 1.0, 0.01, vector<Node>(all(targetNodesSet)));
    const auto A = AddTSPResult(vector<Node>(), tspResult, nullptr);

    cerr << "A size: " << A.size() << endl;
    cerr << "targetNodes size: " << targetNodesSet.size() << endl;
    cerr << "notTargetNodes size: " << notTargetNodesSet.size() << endl;
    exit(0);
  }

  void Solve() {
    vector<Node> A;
    // if(LB < 10){
    //   cerr << "score: 100000000" << endl;
    //   exit(0);
    // }
    // A = CreateStar(centerNode);
    // A = AroundSA();
    if ((int)A.size() == 0) {
      A = NBDBranch::Run(1300);
      // if(A.size() > 0){
      //   cerr << "score: 900000000" << endl;
      //   exit(0);
      // }
    }
    if ((int)A.size() == 0) {
      // cerr << "score: 1000000000" << endl;
      // exit(0);
      if (LB >= PARAMS[0]) {
        // cerr << "score: 1000000000" << endl;
        // exit(0);
        A = CreateStar();
      } else {
        A = CreateGoban();
      }
    }
    if ((int)A.size() == 0) {
      // cerr << "score: 1000000000" << endl;
      // exit(0);
      // A = BranchOld::BranchFromCenter();
    }
    if ((int)A.size() == 0) {
      // cerr << "score: 1000000000" << endl;
      // exit(0);
      // A = ManyStainers();
    }
    if ((int)A.size() == 0) {
      // シュタイナー木の方針に失敗した場合は、SAで解く
      // A = SA();
      // seed=3141はCreateStarにもSAにも失敗するので、碁盤でごまかす
      A = CreateGoban();
    }

    A = AddNoops(A);

    global_timers["solve1"].start();
    const auto actions = OptA::Solve(A);
    global_timers["solve1"].add();

    cerr << global_timers << endl;
    global_timers["PrintAnswer"].start();
    PrintAnswer(A, actions);
    global_timers["PrintAnswer"].add();
    std::quick_exit(0);

    return;
  }

 private:
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
  Output::StaticInit(cin);
  Solver solver;
  solver.Solve();
  return 0;
}