#pragma GCC optimize("O3")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx")

#include <atcoder/dsu>
#include <atcoder/segtree>
// #include <atcoder/mincostflow>

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
using P = std::pair<int, int>;
constexpr int INF = 1e9;
constexpr double PI = acos(-1.0);

// Template for printing pair and vector
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

Timer timer(1.95);
XorShift rnd;

class SimulatedAnnealing {
public:
  constexpr static int LOG_SIZE = 0x10000;
  constexpr static int UPDATE_INTERVAL = 0xFF;
  constexpr static double START_TEMP = 20;
  constexpr static double END_TEMP = 1;
  constexpr static int64_t CYCLES = Timer::CYCLES_PER_SEC * 0.4;
  constexpr static double TEMP_RATIO = (END_TEMP - START_TEMP) / CYCLES;  // linear
  // constexpr static double TEMP_RATIO = log(END_TEMP / START_TEMP) / CYCLES;  // exponential
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
    temp = END_TEMP - (stop - cycle) * TEMP_RATIO;  // linear
    // temp = START_TEMP * exp((cycle - start) * TEMP_RATIO);  // exponential
    return cycle >= stop;
  }

  inline bool Accept(double diff) {
    static unsigned short index = 0;
    return diff >= 0 || diff > logp[index++] * temp;
  }
};

using namespace std;

constexpr int N = 800;
constexpr int MAX_M = 400;
constexpr int Q = 400;
constexpr int MIN_L = 3;
constexpr int MAX_L = 15;
constexpr int MIN_W = 500;
constexpr int MAX_W = 2500;
int M, L, W;
int G[MAX_M];
int G_index[MAX_M];
pair<int, int> X[N], Y[N];
double D[N][N];
double DSum[N][N];
int NMCMC = 0;
uint64_t hash_city[N];
mt19937_64 rng;

template <typename T> T sq(T x) { return x * x; }

void SetUp() {
  int N_, Q_;
  scanf("%d%d%d%d%d", &N_, &M, &Q_, &L, &W);
  assert(N_ == N);
  assert(Q_ == Q);
  REP(i,M) scanf("%d", G + i);
  REP(i,N) {
    scanf("%d%d", &X[i].first, &X[i].second);
    scanf("%d%d", &Y[i].first, &Y[i].second);
    // constexpr double R = 0.05;
    // int lx = (X[i].first * (1 - R) + X[i].second * R) + 0.5;
    // int rx = (X[i].first * R + X[i].second * (1 - R)) + 0.5;
    // int ly = (Y[i].first * (1 - R) + Y[i].second * R) + 0.5;
    // int ry = (Y[i].first * R + Y[i].second * (1 - R)) + 0.5;
    // X[i] = {lx, rx};
    // Y[i] = {ly, ry};
  }
  LOG(M);
  LOG(L);
  LOG(W);
  std::iota(G_index, G_index + M, 0);
  std::sort(G_index, G_index + M, [&](int i, int j) { return G[i] > G[j]; });
  REP(i,N) hash_city[i] = rng();
}

// Query Phase
vector<int> affiliation[N];
unordered_set<uint64_t> query_hash;
vector<int> query_next[N];
int num_queried[N];

struct QueryData {
  vector<int> cities;
  vector<pair<int, int>> edges;

  bool Query(int index) {
    uint64_t hash = 0;
    for (int i: cities) {
      hash += hash_city[i];
    }
    if (!query_hash.insert(hash).second) return false;

    assert(int(cities.size()) == L);
    printf("? %d", L);
    REP(i,L) {
      printf(" %d", cities[i]);
    }
    printf("\n");
    fflush(stdout);
    edges.reserve(L - 1);
    REP(i,L-1) {
      int s, t;
      scanf("%d%d", &s, &t);
      edges.emplace_back(s, t);
      assert(s < t);
      query_next[s].push_back(t);
      query_next[t].push_back(s);
      num_queried[s]++;
      num_queried[t]++;
    }
    for (int v : cities) {
      affiliation[v].push_back(index);
    }
    return true;
  }
};

vector<QueryData> query;

using Sample = vector<pair<int, int>>;

vector<tuple<int, int, int>> vec;
bool centered[N];

vector<int> GenerateCities(const Sample& sample) {
  vec.reserve(N);
  vec.clear();
  REP(i,N) {
    if (centered[i]) continue;
    vec.emplace_back(num_queried[i], rnd.next(), i);
  }
  sort(vec.begin(), vec.end());

  REP(i_try, N + 1) {
    int center_node = get<2>(vec[std::max(i_try - 1, 0)]);
    auto [x, y] = sample[center_node];

    vector<pair<int, int>> dist(N);
    REP(i,N) {
      auto [x2, y2] = sample[i];
      dist[i] = {sq(x - x2) + sq(y - y2), i};
    }
    sort(dist.begin(), dist.end());

    set<int> group;
    set<int> ng;

    for (auto [_, node] : dist) {
      if (ng.count(node)) {
        if (i_try == 0) continue;
        if (rnd.next(100) < 95) continue;
      }
      group.insert(node);
      ng.erase(node);
      for (int v : query_next[node]) {
        ng.insert(v);
      }
      if (int(group.size()) == L) break;
    }
    if (int(group.size()) < L) continue;
    assert(int(group.size()) == L);
    centered[center_node] = true;
    return vector<int>(group.begin(), group.end());
  }
  assert(false);
  return {};
}

struct EdgePair {
  pair<int, int> e;
  vector<int> gA, gB;
};

struct Batch {
  vector<int> cities;
  vector<EdgePair> pairs;
};

int mapping[N];
vector<Batch> dataset;

void SGD(Sample& sample, const int n_iters) {
  using fp = float;
  using Vec = pair<fp, fp>;

  dataset.reserve(query.size());
  for (const QueryData& q : query) {
    Batch batch;
    const int n = q.cities.size();
    batch.cities = q.cities;
    batch.pairs.reserve(q.edges.size());
    REP(i,n) mapping[q.cities[i]] = i;
    for (auto [s, t] : q.edges) {
      EdgePair pair;
      pair.e = {s, t};
      atcoder::dsu uf(n);
      for (auto [ss, tt] : q.edges) {
        if (s == ss && t == tt) continue;
        uf.merge(mapping[ss], mapping[tt]);
      }
      REP(i,n) {
        if (uf.same(i, mapping[s])) {
          pair.gA.push_back(q.cities[i]);
        } else {
          assert(uf.same(i, mapping[t]));
          pair.gB.push_back(q.cities[i]);
        }
      }
      batch.pairs.push_back(std::move(pair));
    }
    dataset.push_back(std::move(batch));
  }
  std::shuffle(ALL(dataset), rng);

  // vector<pair<int, int>> es;
  // for (const QueryData& q : query) {
  //   es.insert(es.end(), ALL(q.edges));
  // }
  // LOG(es.size());
  vector<Vec> x(N), grad(N), v(N);
  std::copy(ALL(sample), x.begin());

  for (int iter = 0; iter < n_iters; iter++) {
    for (int i = 0; i < N; i++) {
      grad[i].first = 0.0;
      grad[i].second = 0.0;
    }
    // fp loss = 0.0;
    constexpr fp eps = 1e-7;

    const Batch& batch = dataset[iter % dataset.size()];
    // for (const auto& edge : es) {
    //   int u = edge.first, v = edge.second;
    //   fp dx = x[u].first - x[v].first;
    //   fp dy = x[u].second - x[v].second;
    //   loss += (dx * dx + dy * dy) / 2;
    //   grad[u].first += dx;
    //   grad[u].second += dy;
    //   grad[v].first -= dx;
    //   grad[v].second -= dy;
    // }
    for (const auto& pair : batch.pairs) {
      auto [s, t] = pair.e;
      const auto& gA = pair.gA;
      const auto& gB = pair.gB;
      fp dx1 = x[s].first - x[t].first;
      fp dy1 = x[s].second - x[t].second;
      fp d1 = dx1 * dx1 + dy1 * dy1;

      for (int ss : gA) {
        for (int tt : gB) {
          if (ss == s && tt == t) continue;
          fp dx2 = x[ss].first - x[tt].first;
          fp dy2 = x[ss].second - x[tt].second;
          fp d2 = dx2 * dx2 + dy2 * dy2;
          if (d1 <= d2 - 5) continue;  // TODO: Param
          fp inv_d1 = 1 / sqrt(d1 + eps);
          fp inv_d2 = 1 / sqrt(d2 + eps);
          grad[s].first += dx1 * inv_d1;
          grad[s].second += dy1 * inv_d1;
          grad[t].first -= dx1 * inv_d1;
          grad[t].second -= dy1 * inv_d1;
          grad[ss].first -= dx2 * inv_d2;
          grad[ss].second -= dy2 * inv_d2;
          grad[tt].first += dx2 * inv_d2;
          grad[tt].second += dy2 * inv_d2;
        }
      }
    }
    
    for (int i : batch.cities) {
      // // Momentum SGD
      // constexpr fp lr = 0.01;
      // constexpr fp momentum = 0.9;
      // v[i].first = momentum * v[i].first - lr * grad[i].first;
      // v[i].second = momentum * v[i].second - lr * grad[i].second;
      // x[i].first += v[i].first;
      // x[i].second += v[i].second;

      // AdaGrad
      constexpr fp lr = 200;
      v[i].first += grad[i].first * grad[i].first;
      v[i].second += grad[i].second * grad[i].second;
      x[i].first -= lr * grad[i].first / (sqrt(v[i].first) + eps);
      x[i].second -= lr * grad[i].second / (sqrt(v[i].second) + eps);

      // Clamp
      x[i].first = std::clamp<fp>(x[i].first, X[i].first, X[i].second);
      x[i].second = std::clamp<fp>(x[i].second, Y[i].first, Y[i].second);

      // if (!(0 <= x[i].first && x[i].first <= 10000) ||
      //     !(0 <= x[i].second && x[i].second <= 10000)) {
      //   LOG(x[i]);
      //   LOG(grad[i]);
      //   LOG(v[i]);
      // }
      // assert(0 <= x[i].first && x[i].first <= 10000);
      // assert(0 <= x[i].second && x[i].second <= 10000);
    }
    // if (iter % 20 == 0) {
    //   // TODO: Print loss
    //   LOG(loss);
    // }
  }
  for (int i = 0; i < N; i++) {
    sample[i] = { (int)round(x[i].first), (int)round(x[i].second) };
  }
}

pair<int,int> edgeArr[16];

void ComputeMSTFromSample(const Sample& sample, const vector<int>& cities) {
  int n = cities.size();
  bool used[16] = { false };
  int minDist[16], parent[16];
  for (int i = 0; i < n; i++) {
    minDist[i] = INT_MAX;
    parent[i] = -1;
  }
  minDist[0] = 0;
  for (int i = 0; i < n; i++) {
    int u = -1, best = INT_MAX;
    for (int j = 0; j < n; j++) {
      if (!used[j] && minDist[j] < best) {
        best = minDist[j];
        u = j;
      }
    }
    if (u == -1) break;
    used[u] = true;
    for (int v = 0; v < n; v++) {
      if (!used[v]) {
        int cityU = cities[u], cityV = cities[v];
        int dx = sample[cityU].first - sample[cityV].first;
        int dy = sample[cityU].second - sample[cityV].second;
        int d = dx * dx + dy * dy;
        if (d < minDist[v]) {
          minDist[v] = d;
          parent[v] = u;
        }
      }
    }
  }
  int edgeCount = 0;
  for (int v = 1; v < n; v++) {
    int u = parent[v];
    if (u == -1) continue;
    int a = cities[u], b = cities[v];
    if (a > b) swap(a, b);
    edgeArr[edgeCount++] = {a, b};
  }
  for (int i = 1; i < edgeCount; i++) {
    auto key = edgeArr[i];
    int j = i - 1;
    while (j >= 0 && (edgeArr[j].first > key.first ||
                      (edgeArr[j].first == key.first && edgeArr[j].second > key.second))) {
      edgeArr[j+1] = edgeArr[j];
      j--;
    }
    edgeArr[j+1] = key;
  }
}

int GetCost(const QueryData& query, const Sample& sample) {
  const vector<pair<int, int>>& edges = query.edges;
  ComputeMSTFromSample(sample, query.cities);  // Output to edgeArr
  assert(int(edges.size()) == L - 1);
  int res = 0;
  REP(i,L-1) {
    auto [s, t] = edgeArr[i];
    bool exist = false;
    REP(j,L-1) {
      if (edges[j].first == s && edges[j].second == t) {
        exist = true;
        break;
      }
    }
    // bool exist = std::binary_search(ALL(edges), make_pair(s, t));
    // bool exist = std::find(ALL(edges), make_pair(s, t)) != edges.end();
    if (!exist) res++;
  }
  return res;
}

bool use_prio[N][N];
pair<double, double> sum_samples[N];
int cnt_samples = 0;

void MCMC(Sample &sample, double time_limit) {
  // REP(i,N) accum[i] = (X[i].second - X[i].first) + (Y[i].second - Y[i].first);
  // std::partial_sum(accum, accum + N, accum);
  // int max_sum = accum[N - 1];
  memset(use_prio, 0, sizeof(use_prio));
  REP(i,N) sum_samples[i] = {0.0, 0.0};
  cnt_samples = 0;

  int query_size = query.size();
  vector<int> cost(query_size);
  int sum_cost = 0;
  REP(i,query_size) {
    cost[i] = GetCost(query[i], sample);
    sum_cost += cost[i];
  }
  LOG(sum_cost);

  REP(i,query_size) CERR() << cost[i] << " ";
  CERR() << endl;

  vector<int> prev_cost(query_size);
  // Timer timer(M == 1 ? 1.75 : (M < 85 ? 1.3 : 1.1));
  // Timer timer(19.6);
  // Timer timer(199.7);
  int loop = 0;
  int accept = 0;
  double next_time = timer.elapsed();  // TODO: Param
  vector<pair<int, int>> prio;
  int cnt_prio = 0, cnt_all = 0;
  for (;;) {
    // while (timer.within()) {
    ++loop;
    double elapsed = timer.elapsed();
    if (elapsed > time_limit) break;

    if (elapsed > next_time) {
      constexpr int N_ALL = 20;
      constexpr int N_PRIO_PER_ALL = 40;
      // Sampling
      next_time += 1.2 / (N_ALL * N_PRIO_PER_ALL);
      if (cnt_prio == 0) {
        ++cnt_all;
        ++cnt_prio;
        prio.reserve(N * (N - 1) / 2);
        REP(i,N) REP(j,i) {
          D[j][i] = sqrt(sq(sample[i].first - sample[j].first) + sq(sample[i].second - sample[j].second));
          prio.emplace_back(j, i);
        }
        int len = int(prio.size()) / N_PRIO_PER_ALL;
        partial_sort(prio.begin(), prio.begin() + len, prio.end(), [&](const auto& a, const auto& b) {
          return D[a.first][a.second] < D[b.first][b.second];
        });
        prio.resize(len);
      } else if (cnt_prio % N_PRIO_PER_ALL == 0) {
        constexpr int inter_sgd = 1;
        if (cnt_all % inter_sgd == inter_sgd - 1) SGD(sample, 4000);
        ++cnt_all;
        ++cnt_prio;
        REP(i,N) REP(j,i) {
          D[j][i] += sqrt(sq(sample[i].first - sample[j].first) + sq(sample[i].second - sample[j].second));
        }

        // Sum sample
        ++cnt_samples;
        REP(i,N) {
          // if (!(0 <= sample[i].first && sample[i].first <= 10000) ||
          //     !(0 <= sample[i].second && sample[i].second <= 10000)) {
          //   CERR() << sample[i] << endl;
          // }
          // assert(0 <= sample[i].first && sample[i].first <= 10000);
          // assert(0 <= sample[i].second && sample[i].second <= 10000);
          sum_samples[i].first += sample[i].first;
          sum_samples[i].second += sample[i].second;
        }
      } else {
        ++cnt_prio;
        for (auto [j, i] : prio) {
          D[j][i] += sqrt(sq(sample[i].first - sample[j].first) + sq(sample[i].second - sample[j].second));
        }
      }
    }

    int node = rnd.next(N);
    // int node = std::lower_bound(accum, accum + N, rnd.next(max_sum)) - accum;

    assert(node >= 0 && node < N);
    pair<int, int> point = {
      // rnd.next(max(X[node].first, sample[node].first - 100), min(X[node].second, sample[node].first + 100)),
      // rnd.next(max(Y[node].first, sample[node].second - 100), min(Y[node].second, sample[node].second + 100)),
      rnd.next(X[node].first, X[node].second),
      rnd.next(Y[node].first, Y[node].second),
    };
    swap(sample[node], point);
    int diff_cost = 0;
    for (int index : affiliation[node]) {
      prev_cost[index] = cost[index];
      cost[index] = GetCost(query[index], sample);
      diff_cost += cost[index] - prev_cost[index];
    }
    if (diff_cost <= 0) {
      // Accept
      sum_cost += diff_cost;
      ++accept;
    } else {
      // Revert
      sample[node] =  point;
      for (int index : affiliation[node]) {
        cost[index] = prev_cost[index];
      }
    }
  }
  int best_cost = sum_cost;
  LOG(best_cost);
  LOG(loop);
  LOG(accept);
  double accept_ratio = 1.0 * accept / loop;
  LOG(accept_ratio);

  REP(i,query_size) CERR() << cost[i] << " ";
  CERR() << endl;

  ++NMCMC;
  for (auto [s, t] : prio) {
    use_prio[s][t] = true;
    DSum[s][t] += D[s][t] / cnt_prio;
  }
  REP(i,N) REP(j,i) {
    if (!use_prio[j][i]) {
      DSum[j][i] += D[j][i] / cnt_all;
    }
    DSum[i][j] = DSum[j][i];
    D[i][j] = 0;
    D[j][i] = 0;
  }

  // int final_cost = 0;
  // REP(i,query_size) final_cost += cost[i];
  // LOG(final_cost);
}

struct MCTResult {
  vector<int> cities;
  vector<pair<int, int>> edges;
  double cost;
  MCTResult() : cost(0) {}
};

// Solve Phase
struct Solver {
  vector<bool> used;
  vector<vector<int>> cities;
  vector<vector<pair<int, int>>> edges;
  vector<double> costs;
  vector<tuple<int, int, int, int>> ranges;

  set<int> live_node;
  vector<vector<int>> graph;
  vector<bool> visited;
  vector<tuple<int, double, int, int>> cut;  // size, -len, v, w

  void Init() {
    used.assign(N, false);
    cities.assign(M, {});
    edges.resize(M);
  }

  // Greedy
  int dfs(int v) {
    visited[v] = true;
    int size = 1;
    for (int w: graph[v]) {
      if (visited[w]) continue;
      int child_size = dfs(w);
      if (child_size < int(live_node.size()) - child_size) {
        cut.emplace_back(child_size, -D[v][w], v, w);
      } else {
        cut.emplace_back(int(live_node.size()) - child_size, -D[v][w], w, v);
      }
      size += child_size;
    }
    return size;
  }
  void register_dfs(int group, int v) {
    assert(live_node.count(v));
    live_node.erase(v);
    cities[group].push_back(v);

    visited[v] = true;
    for (int w: graph[v]) {
      if (visited[w]) continue;
      edges[group].emplace_back(min(v, w), max(v, w));
      register_dfs(group, w);
    }
  }
  bool IsOK(priority_queue<int> que, multiset<int> sizes) {
    while (!que.empty()) {
      assert(!sizes.empty());
      int A = que.top();
      que.pop();
      auto it = std::prev(sizes.end());
      int B = *it;
      if (B == 1) return true;
      sizes.erase(it);
      if (A == B) continue;
      if (A < B) return false;
      que.push(A - B);
    }
    return true;
  }
  void Kruskal(const vector<tuple<double, int, int>>& edge_set_vec) {
    atcoder::dsu uf(N);
    // vector<tuple<double, int, int>> edge_set_vec;
    // edge_set_vec.reserve(N * (N - 1) / 2);
    // REP(i,N) REP(j,i) edge_set_vec.emplace_back(D[i][j], j, i);
    // assert(int(edge_set_vec.size()) == N * (N - 1) / 2);
    // sort(ALL(edge_set_vec));
    // LOG(timer.elapsed());
    set<tuple<double, int, int>> edge_set(ALL(edge_set_vec));

    multiset<int> sizes;
    REP(i,N) sizes.insert(1);
    // CERR() << sizes.size() << endl;

    priority_queue<int> que;
    REP(i,M) que.push(G[i]);

    int uf_size = N;
    double cost = 0;
    assert(IsOK(que, sizes));
    vector<pair<int, int>> es;
    es.reserve(N - M);

    REP(loop,N-M-1) {
      if (timer.elapsed() > 1.6) return;
      while (!que.empty() && !sizes.empty() && que.top() == *std::prev(sizes.end())) {
        auto size_it = std::prev(sizes.end());
        sizes.erase(size_it);
        que.pop();
      }
      auto it = edge_set.begin();
      for (;;) {
        if (timer.elapsed() > 1.6) return;
        assert (it != edge_set.end());
        auto [d, s, t] = *it;
        if (uf.same(s, t)) {
          it = edge_set.erase(it);
          continue;
        }
        // Unite sizes
        int s_size = uf.size(s);
        int t_size = uf.size(t);
        assert(!sizes.empty());
        if (s_size + t_size > que.top()) {
          it = edge_set.erase(it);
          continue;
        }
        auto size_it = sizes.find(s_size);
        assert(size_it != sizes.end());
        sizes.erase(size_it);
        size_it = sizes.find(t_size);
        assert(size_it != sizes.end());
        sizes.erase(size_it);
        sizes.insert(s_size + t_size);

        if (IsOK(que, sizes)) {
          uf.merge(s, t);
          es.emplace_back(s, t);
          --uf_size;
          cost += d;

          // CERR() << uf_size << "/" << M << " " << cost << " " << timer.elapsed() << " : ";
          // // for (int x: sizes) CERR() << x << " ";
          // CERR() << endl;
          break;
        } else {
          size_it = sizes.find(s_size + t_size);
          assert(size_it != sizes.end());
          sizes.erase(size_it);
          sizes.insert(s_size);
          sizes.insert(t_size);
          ++it;
        }
      }
      if (uf_size == M) break;
    }
    while (!que.empty()) {
      auto it = sizes.find(que.top());
      que.pop();
      if (it != sizes.end()) sizes.erase(it);
    }
    assert(sizes.size());
    int s_size_expected = *sizes.begin();
    int t_size_expected = *std::next(sizes.begin());
    CERR() << s_size_expected << " " << t_size_expected << endl;
    {
      for (auto [d, s, t] : edge_set) {
        if (uf.same(s, t)) continue;
        int s_size = uf.size(s);
        int t_size = uf.size(t);
        if (s_size_expected != std::min(s_size, t_size)) continue;
        if (t_size_expected != std::max(s_size, t_size)) continue;

        uf.merge(s, t);
        es.emplace_back(s, t);
        --uf_size;
        cost += d;
        CERR() << uf_size << "/" << M << " " << cost << " " << timer.elapsed() << " : ";
        for (int x: sizes) CERR() << x << " ";
        CERR() << endl;
        break;
      }
    }

    assert(uf_size == M);
    vector<pair<int, int>> connect(N);
    REP(i,N) {
      connect[i].second = i;
      connect[uf.leader(i)].first++;
      used[i] = true;
    }
    sort(connect.rbegin(), connect.rend());
    if (M < N) assert(connect[M].first == 0);
    connect.resize(M);
    vector<int> mapping(N, -1);
    REP(i,M) mapping[connect[i].second] = G_index[i];

    REP(i,N) {
      assert(mapping[uf.leader(i)] != -1);
      cities[mapping[uf.leader(i)]].push_back(i);
    }
    for (auto [s, t] : es) {
      int group = mapping[uf.leader(s)];
      assert(group == mapping[uf.leader(t)]);
      edges[group].emplace_back(s, t);
    }
    REP(i,M) {
      assert(int(cities[i].size()) == G[i]);
      assert(int(edges[i].size()) == G[i] - 1);
    }

    // LOG(timer.elapsed());
    // assert(false);
  }
  void Run() {
    {
      graph.resize(N);
      atcoder::dsu uf(N);
      vector<tuple<double, int, int>> edges;
      REP(i,N) REP(j,i) edges.emplace_back(D[i][j], i, j);
      sort(ALL(edges));
      for (auto [_, u, v] : edges) {
        if (uf.same(u, v)) continue;
        graph[u].push_back(v);
        graph[v].push_back(u);
        uf.merge(u, v);
      }
      REP(i,N) live_node.insert(i);
    }
    REP(loop,M-1) {
      assert(!live_node.empty());
      visited.assign(N, false);
      cut.clear();
      dfs(*live_node.begin());
      std::sort(ALL(cut));
      assert(cut.size() + 1 == live_node.size());
      // for (auto [a, b, c, d] : cut) CERR() << a << " " << b << " " << c << " " << d << endl;

      bool done = false;
      REP(diff,1000) {
        REP(i,M) {
          int g = G_index[i];
          if (!cities[g].empty()) continue;
          int size = G[g] - diff;
          assert(size > 0);
          bool swapped = false;
          if (int(live_node.size()) - size < size) {
            size = int(live_node.size()) - size;
            swapped = true;
          }
          auto it = lower_bound(ALL(cut), tuple<int, double, int, int>(size, -INF, 0, 0));
          if (it == cut.end()) continue;
          if (get<0>(*it) == size) {
            // Do cut
            auto [_1, _2, v, w] = *it;
            auto it1 = std::find(ALL(graph[v]), w);
            assert(it1 != graph[v].end());
            graph[v].erase(it1);
            auto it2 = std::find(ALL(graph[w]), v);
            assert(it2 != graph[w].end());
            graph[w].erase(it2);

            visited.assign(N, false);
            register_dfs(g, swapped ? v : w);
            // CERR() << endl;
            // CERR() << loop << "/" << M << " " << cities[g].size() << " " << size << " " << swapped << endl;
            // CERR() << i << " " << _1 << " " << _2 << " " << v << " " << w << " " << endl;
            // CERR() << live_node.size() << " " << diff << endl;

            // assert(int(cities[g].size()) == size);
            // assert(int(edges[g].size()) == size - 1);

            REP(j,diff) {
              // CERR() << "special phase" << endl;
              double min_cost = INF;
              int from = -1, to = -1;
              for (int x : live_node) {
                // CERR() << x << " " << graph[x].size() << endl;
                assert(!graph[x].empty());
                if (int(graph[x].size()) != 1) continue;
                double c = INF;
                int fr = -1;
                assert(!cities[g].empty());
                for (int y : cities[g]) {
                  if (D[x][y] < c) {
                    c = D[x][y];
                    fr = y;
                  }
                }
                assert(fr != -1);
                c -= D[x][graph[x][0]];
                if (c < min_cost) {
                  min_cost = c;
                  from = fr;
                  to = x;
                }
              }
              assert(from != -1);
              assert(to != -1);
              // CERR() << from << " " << to << endl;
              live_node.erase(to);
              assert(int(graph[to].size()) == 1);
              int to2 = graph[to][0];
              auto it1 = std::find(ALL(graph[to]), to2);
              assert(it1 != graph[to].end());
              graph[to].erase(it1);
              auto it2 = std::find(ALL(graph[to2]), to);
              assert(it2 != graph[to2].end());
              graph[to2].erase(it2);
              cities[g].push_back(to);
              edges[g].emplace_back(from, to);
            }

            done = true;
            break;
          }
        }
        if (done) break;
      }
    }
    assert(!live_node.empty());
    int gg = -1;
    REP(i,M) {
      if (cities[i].empty()) {
        assert(gg == -1);
        gg = i;
      }
    }
    assert(gg != -1);
    visited.assign(N, false);
    register_dfs(gg, *live_node.begin());
    assert(int(cities[gg].size()) == G[gg]);
    assert(int(edges[gg].size()) == G[gg] - 1);
  }

  MCTResult Group(int node, int size) {
    MCTResult res;
    assert(!used[node]);
    using T = tuple<int, int, int>;
    priority_queue<T, vector<T>, greater<T>> que;
    auto add = [&](int node) {
      used[node] = true;
      res.cities.push_back(node);
      REP(i,N) {
        if (used[i]) continue;
        int d = D[node][i];
        que.emplace(d, node, i);
      }
    };
    add(node);
    REP(i, size - 1) {
      for (;;) {
        assert(!que.empty());
        auto [d, from, to] = que.top();
        que.pop();
        if (used[to]) continue;

        // OK
        res.edges.emplace_back(from, to);
        res.cost += d;
        add(to);
        break;
      }
    }
    for (int city : res.cities) {
      used[city] = false;
    }
    return res;
  }
  void Apply(int group, MCTResult result) {
    cities[group] = std::move(result.cities);
    edges[group] = std::move(result.edges);
    for (int city: cities[group]) {
      used[city] = true;
    }
  }

  // SA
  bool GroupFromUnused(int index, int node, int size, const vector<int>& unused) {
    assert(!used[node]);
    using T = tuple<int, int, int>;
    priority_queue<T, vector<T>, greater<T>> que;
    auto add = [&](int node) {
      used[node] = true;
      cities[index].push_back(node);
      for (int i: unused) {
        if (used[i]) continue;
        int d = D[node][i];
        que.emplace(d, node, i);
      }
    };
    add(node);
    REP(i, size - 1) {
      for (;;) {
        if (!timer.within()) return false;
        assert(!que.empty());
        auto [d, from, to] = que.top();
        que.pop();
        if (used[to]) continue;

        // OK
        edges[index].emplace_back(from, to);
        add(to);
        break;
      }
    }
    return true;
    // std::cerr << size << " " << edges.back().size() << std::endl;
  }
  void RevertGroup(int group, vector<int>& tmp_city, vector<pair<int, int>>& tmp_edge) {
    for (int city: cities[group]) {
      used[city] = false;
    }
    tmp_city.swap(cities[group]);
    tmp_edge.swap(edges[group]);
  }
  inline void ResetRange(int group, const Sample& sample) {
    int xl = INF, yl = INF, xr = -INF, yr = -INF;
    for (int city : cities[group]) {
      chmin(xl, sample[city].first);
      chmin(yl, sample[city].second);
      chmax(xr, sample[city].first);
      chmax(yr, sample[city].second);
    }
    ranges[group] = {xl, yl, xr, yr};
  }
  inline bool Overlap(int group1, int group2) {
    auto [xl1, yl1, xr1, yr1] = ranges[group1];
    auto [xl2, yl2, xr2, yr2] = ranges[group2];
    if (xr1 <= xl2 || xr2 <= xl1 || yr1 <= yl2 || yr2 <= yl1) {
      // Not overlap
      return std::min({
          std::abs(xr1 - xl2),
          std::abs(xr2 - xl1),
          std::abs(yr1 - yl2),
          std::abs(yr2 - yl1),
        }) < 350;  // TODO: Fix threshold
    }
    // Overlap
    return true;
  }
  inline int GetRandomUnused(const vector<int>& unused) const {
    const int n = unused.size();
    int node = unused[rnd.next(n)];
    while (used[node]) {
      node = unused[rnd.next(n)];
    }
    return node;
  }
  void SA(const Sample& sample) {
    costs.resize(M);
    ranges.resize(M);
    double sa_cost = 0;
    REP(i,M) {
      costs[i] = 0;
      for (auto [s, t] : edges[i]) costs[i] += D[s][t];
      sa_cost += costs[i];
      ResetRange(i, sample);
    }
    LOG(sa_cost);

    SimulatedAnnealing sa;
    vector<int> tmp_city1, tmp_city2;
    vector<pair<int, int>> tmp_edge1, tmp_edge2;
    vector<int> unused;
    int iteration = 0;
    // while (timer.within()) {
    while (!sa.End()) {
      // if (sa.iteration > 100) break;
      int group1 = rnd.next(M);
      int group2 = rnd.next(M);
      if (group1 == group2) continue;
      if (!Overlap(group1, group2)) continue;
      if (iteration % 10000 == 0) {
        CERR() << iteration << " " << sa_cost << endl;
      }
      ++iteration;
      tmp_city1.clear(); tmp_edge1.clear();
      tmp_city2.clear(); tmp_edge2.clear();
      RevertGroup(group1, tmp_city1, tmp_edge1);
      RevertGroup(group2, tmp_city2, tmp_edge2);

      unused.clear();
      unused.insert(unused.end(), ALL(tmp_city1));
      unused.insert(unused.end(), ALL(tmp_city2));
      assert(!unused.empty());

      bool timeout = true;
      int node1 = GetRandomUnused(unused);
      timeout &= GroupFromUnused(group1, node1, G[group1], unused);
      int node2 = GetRandomUnused(unused);
      timeout &= GroupFromUnused(group2, node2, G[group2], unused);

      double new_cost1 = 0, new_cost2 = 0;
      for (auto [s, t] : edges[group1]) new_cost1 += D[s][t];
      for (auto [s, t] : edges[group2]) new_cost2 += D[s][t];
      double diff = (new_cost1 + new_cost2) - (costs[group1] + costs[group2]);
      // CERR() << diff << std::endl;
      // if (diff <= 0 && timeout) {
      if (timeout && sa.Accept(-diff)) {
        // Accept
        costs[group1] = new_cost1;
        costs[group2] = new_cost2;
        ResetRange(group1, sample);
        ResetRange(group2, sample);
        sa_cost += diff;
      } else {
        // Revert
        tmp_city1.swap(cities[group1]);
        tmp_city2.swap(cities[group2]);
        tmp_edge1.swap(edges[group1]);
        tmp_edge2.swap(edges[group2]);
      }
    }
    LOG(iteration);
    double sa_final = sa_cost;
    LOG(sa_final);
  }
  void Output() const {
    assert(int(cities.size()) == M);
    assert(int(edges.size()) == M);
    printf("!\n");
    REP(i,M) {
      assert(int(cities[i].size()) == G[i]);
      assert(int(edges[i].size()) == G[i] - 1);
      REP(j,G[i]) {
        if (j > 0) printf(" ");
        printf("%d", cities[i][j]);
      }
      printf("\n");
      for (auto e : edges[i]) {
        if (e.first == e.second) continue;
        printf("%d %d\n", e.first, e.second);
      }
    }
    fflush(stdout);
  }
};

int main() {
  SetUp();

  // Phase 1
  CERR() << "<<< Phase 1 >>>" << std::endl;
  Sample sample(N);
  REP(i,N) sample[i] = { (X[i].first + X[i].second) / 2, (Y[i].first + Y[i].second) / 2 };

  // if (L < 3) {
  //   REP(i,N) REP(j,N) {
  //     double dx = (X[i].first + X[i].second - X[j].first - X[j].second) / 2.0;
  //     double dy = (Y[i].first + Y[i].second - Y[j].first - Y[j].second) / 2.0;
  //     D[i][j] = sqrt(sq(dx) + sq(dy));
  //   }
  //   REP(i,N) {
  //     sum_samples[i].first = (X[i].first + X[i].second) / 2.0;
  //     sum_samples[i].second = (Y[i].first + Y[i].second) / 2.0;
  //   }
  //   cnt_samples = 1;
  // } else {

  vector<pair<int, int>> center_prep(N);
  REP(i,N) {
    int dx = X[i].second - X[i].first;
    int dy = Y[i].second - Y[i].first;
    center_prep[i] = { dx + dy, i };
  }
  sort(ALL(center_prep));
  REP(i,300) centered[center_prep[i].second] = true;

  REP(i,Q) {
    query.resize(i + 1);
    query.back().cities = GenerateCities(sample);
    bool success = query.back().Query(i);
    // CERR() << success << std::endl;
    // CERR() << i << " " << query[i].cities.size() << " " << query[i].edges.size() << std::endl;
    if (!success) { --i; continue; }
    assert(int(query[i].cities.size()) == L);
    assert(int(query[i].edges.size()) == L - 1);

    if (i % 100 == 99) {
      SGD(sample, (i + 1) * 10);
    }
  }
  double phase1_time = timer.elapsed();
  LOG(phase1_time);

  // MCMC(sample, M == 1 ? 1.0 : (M < 85 ? 0.7 : 0.6));

  // REP(i,N) sample[i] = { (X[i].first + X[i].second) / 2, (Y[i].first + Y[i].second) / 2 };
  // SGD(sample, 5000);
  MCMC(sample, M == 1 ? 1.9 : (M < 85 ? 1.35 : 1.15));

  REP(i,N) REP(j,N) D[i][j] = DSum[i][j] / NMCMC;

  // }
  Sample ave_sample(N);
  REP(i,N) {
    ave_sample[i] = {
      int(sum_samples[i].first / cnt_samples + 0.5),
      int(sum_samples[i].second / cnt_samples + 0.5)
    };
  }

  double phase2_time = timer.elapsed();
  LOG(phase2_time);

  // Phase 3
  CERR() << "<<< Phase 3 >>>" << std::endl;
  Solver solver;
  solver.Init();

  if (M < 70) {
    solver.Run();
  } else {
    vector<tuple<double, int, int>> edges;
    edges.reserve(N * (N - 1) / 2);
    REP(i,N) REP(j,i) {
      edges.emplace_back(D[i][j], j, i);
    }
    sort(edges.begin(), edges.end());

    solver.Kruskal(edges);

    if (solver.cities[0].empty()) {
    reverse(ALL(edges));

    REP(i,M) {
      if (G[G_index[i]] == 1) {
        int node = -1;
        REP(j,N) {
          if (solver.used[j]) continue;
          node = j;
          break;
        }
        assert(node != -1);
        MCTResult res = solver.Group(node, G[G_index[i]]);
        solver.Apply(G_index[i], std::move(res));
      } else {
        int node = -1;
        for (;;) {
          assert(!edges.empty());
          auto [_, s, t] = edges.back();
          edges.pop_back();
          if (solver.used[s] || solver.used[t]) continue;
          node = s;
          break;
        }
        int size = G[G_index[i]];
        MCTResult res = solver.Group(node, size);
        if (M > 1) {
          REP(loop,10) {
            int tmp_node = rnd.next(N);
            if (solver.used[tmp_node]) { --loop; continue; }
            MCTResult tmp = solver.Group(tmp_node, size);
            if (tmp.cost < res.cost) {
              res = std::move(tmp);
            }
          }
        }
        solver.Apply(G_index[i], std::move(res));
      }
    }
    }
  }

  double phase3_time = timer.elapsed();
  if (M > 1) LOG(phase3_time);

  // Phase 4
  CERR() << "<<< Phase 4 >>>" << std::endl;
  if (M > 1) solver.SA(ave_sample);

  solver.Output();
  double elapsed = timer.elapsed();
  LOG(elapsed);
  std::_Exit(0);
  return 0;
}
