#include <bits/stdc++.h>
constexpr int DEBUG = 0;
using namespace std;
using int64 = long long;
using uint64 = unsigned long long;

constexpr int INF = 1.0E9;

template <typename T>
ostream& operator<<(ostream& s, const vector<T>& v) {
  s << "[";
  for (int i = 0; i < int(v.size()); i++) {
    if (i > 0) s << ", ";
    s << v[i];
  }
  s << "]";
  return s;
}

template<typename T>
vector<vector<T>> Make2D(int d1, int d2, T default_value) {
  return vector<vector<T>>(d1, vector<T>(d2, default_value));
}

template<typename T>
vector<vector<vector<T>>>
Make3D(int d1, int d2, int d3, T default_value) {
  return vector<vector<vector<T>>>(
      d1, vector<vector<T>>(d2, vector<T>(d3, default_value)));
}

class Random {
 public:
  Random()
      : random_engine_(
            std::chrono::system_clock::now().time_since_epoch().count()) {}

  Random(int seed) : random_engine_(seed) {}

  int RandomInt(int a, int b, bool inclusive = false) {
    if (inclusive) b++;
    return uniform_int_distribution<int>(a, b - 1)(random_engine_);
  }

  double RandomDouble(double a, double b) {
    return uniform_real_distribution<double>(a, b)(random_engine_);
  }

  template <class T>
  void Shuffle(vector<T>& v) {
    shuffle(v.begin(), v.end(), random_engine_);
  }

 private:
  default_random_engine random_engine_;
};

class TimeMeasurement {
 public:
  TimeMeasurement() : start_(chrono::steady_clock::now()) {}

  int64 CurrentMs() const {
    auto now = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(now - start_).count();
  }

  int64 CurrentUs() const {
    auto now = chrono::steady_clock::now();
    return chrono::duration_cast<chrono::microseconds>(now - start_).count();
  }

  void Reset() {
    start_ = chrono::steady_clock::now();
  }

 private:
  chrono::time_point<chrono::steady_clock> start_;
};

struct Edge {
  int id, s, t;
  int w;
  Edge(int id, int s, int t, int w) : id(id), s(s), t(t), w(w) {}
};
const Edge kInvalidEdge = Edge(-1, -1, -1, 0);

// A smaller data structure to represent an edge to a parent node in
// the shorest path tree.
struct ParentEdge {
  int id;
  int t;
  ParentEdge(int id, int t) : id(id), t(t) {}
};
const ParentEdge kInvalidParentEdge = ParentEdge(-1, -1);

struct Int2D {
  int x, y;
  Int2D(double x, double y) : x(x), y(y) {}
};

class DSU {
 private:
  vector<int> rs, ps, ss;
 
 public:
  DSU(int n) : rs(n), ps(n), ss(n, 1) {
    for (int i = 0; i < n; i++) {
      ps[i] = i;
    }
  }
  int Find(int x) {
    if (x != ps[x]) {
      ps[x] = Find(ps[x]);
    }
    return ps[x];
  }
  void Union(int x, int y) {
    int xr = Find(x);
    int yr = Find(y);
    if (rs[xr] > rs[yr]) {
      ps[yr] = ps[xr];
      ss[xr] += ss[yr];
    } else if (rs[xr] < rs[yr]) {
      ps[xr] = ps[yr];
      ss[yr] += ss[xr];
    } else if (ps[xr] != ps[yr]) {
      ps[yr] = ps[xr];
      rs[xr]++;
      ss[xr] += ss[yr];
    }
  }
  int Size(int x) { return ss[Find(x)]; }
  bool IsRoot(int x) { return x == Find(x); }
};

void ShortestPath(
    const vector<vector<Edge>>& graph,
    const vector<int>& is_edge_active, int s,
    vector<int>& distance_vector,
    vector<ParentEdge>& parent_edge_vector) {
  int n = graph.size();
  for (int v = 0; v < n; v++) {
    distance_vector[v] = INF;
    parent_edge_vector[v] = kInvalidParentEdge;
  }

  vector<int> visited(n);
  struct S {
    int v;
    int w;
  };
  auto c = [](const S& s1, const S& s2) { return s1.w > s2.w; };
  priority_queue<S, vector<S>, decltype(c)> pq(c);
  distance_vector[s] = 0;
  pq.push(S({s, 0}));
  while (!pq.empty()) {
    S s = pq.top();
    pq.pop();
    if (visited[s.v]) {
      continue;
    }
    visited[s.v] = 1;
    for (const Edge e : graph[s.v]) {
      if (!is_edge_active[e.id]) continue;
      if (!visited[e.t] && s.w + e.w < distance_vector[e.t]) {
        distance_vector[e.t] = s.w + e.w;
        parent_edge_vector[e.t] = ParentEdge(e.id, e.s);
        pq.push(S({e.t, distance_vector[e.t]}));
      }
    }
  }
}

double ShortestPathDeltaWithEdgeDeletion(
    const vector<vector<Edge>>& graph, const vector<int>& is_edge_active,
    const Edge& deleted_edge, int s, vector<int>& distance_vector,
    vector<ParentEdge>& parent_edge_vector,
    vector<tuple<int, int, ParentEdge>>& history_tuples) {
  // static int count = 0;
  // static int invalidated_count = 0;
  // count++;
  // if (count % 100000 == 0) {
  //   cerr << "ShortestPathDeltaWithEdgeDeletion() count: " << count << endl;
  //   cerr << "ShortestPathDeltaWithEdgeDeletion() invalidated_count: "
  //        << invalidated_count << endl;
  // }

  int r = -1;
  if (parent_edge_vector[deleted_edge.s].t == deleted_edge.t) {
    r = deleted_edge.s;
  } else if (parent_edge_vector[deleted_edge.t].t == deleted_edge.s) {
    r = deleted_edge.t;
  } else {
    return 0.0;
  }

  static vector<int> invalidated_vertex_list;
  static deque<int> q;
  invalidated_vertex_list.push_back(r);
  q.push_back(r);
  while (!q.empty()) {
    int v = q.front();
    q.pop_front();
    for (const Edge& e : graph[v]) {
      if (parent_edge_vector[e.t].t == v) {
        invalidated_vertex_list.push_back(e.t);
        q.push_back(e.t);
      }
    }
  };

  constexpr int MAX_VERTEX_COUNT = 1000;
  static vector<int> invalidated(MAX_VERTEX_COUNT);
  for (int v : invalidated_vertex_list) {
    invalidated[v] = 1;
    // invalidated_count++;
  }
  static vector<int> finalized(MAX_VERTEX_COUNT);
  static vector<int> finalized_vertex_list;

  struct S {
    int v;
    int w;
  };
  static auto c = [](const S& s1, const S& s2) { return s1.w > s2.w; };
  static priority_queue<S, vector<S>, decltype(c)> pq(c);

  double distance_delta = 0;
  auto update = [&](int v, int d, int edge_id, int p) -> void {
    history_tuples.push_back({v, distance_vector[v], parent_edge_vector[v]});

    distance_delta += d - distance_vector[v];
    distance_vector[v] = d;
    parent_edge_vector[v] = ParentEdge(edge_id, p);
  };

  for (int v : invalidated_vertex_list) {
    ParentEdge parent_edge = parent_edge_vector[v];
    int min_d = INF;
    Edge min_edge = kInvalidEdge;
    for (const Edge& e : graph[v]) {
      if (!is_edge_active[e.id]) continue;
      if (!invalidated[e.t]) {
        if (distance_vector[e.t] + e.w < min_d) {
          min_d = distance_vector[e.t] + e.w;
          min_edge = e;
        }
      }
    }

    if (min_edge.id >= 0) {
      invalidated[v] = 0;
      update(v, min_d, min_edge.id, min_edge.t);
      if (min_edge.id != parent_edge.id) {
        pq.push(S({v, min_d}));
      }
    }
  }

  while (!pq.empty()) {
    S s = pq.top();
    pq.pop();
    if (finalized[s.v]) {
      continue;
    }
    finalized[s.v] = 1;
    finalized_vertex_list.push_back(s.v);
    for (const Edge e : graph[s.v]) {
      if (!is_edge_active[e.id]) continue;
      if (invalidated[e.t]) {
        invalidated[e.t] = 0;
        update(e.t, s.w + e.w, e.id, e.s);
        pq.push(S({e.t, distance_vector[e.t]}));
      } else if (!finalized[e.t] && s.w + e.w < distance_vector[e.t]) {
        update(e.t, s.w + e.w, e.id, e.s);
        pq.push(S({e.t, distance_vector[e.t]}));
      }
    }
  }

  for (int v : invalidated_vertex_list) {
    if (invalidated[v]) {
      update(v, INF, -1, -1);
    }
  }

  // Reset invalidated and invalidated_vertex_list for reuse.
  for (int v : invalidated_vertex_list) {
    invalidated[v] = 0;
  }
  invalidated_vertex_list.clear();

  // Reset finalized and finalized_vertex_list for reuse.
  for (int v : finalized_vertex_list) {
    finalized[v] = 0;
  }
  finalized_vertex_list.clear();

  return distance_delta;
}

double ShortestPathDeltaWithEdgeAddition(
    const vector<vector<Edge>>& graph, const vector<int>& is_edge_active,
    const Edge& added_edge, int s, vector<int>& distance_vector,
    vector<ParentEdge>& parent_edge_vector,
    vector<tuple<int, int, ParentEdge>>& history_tuples) {
  // static int count = 0;
  // static int loop_count = 0;
  // static int finalized_count = 0;
  // count++;
  // if (count % 100000 == 0) {
  //   cerr << "ShortestPathDeltaWithEdgeAddition(): " << count << endl;
  //   cerr << "loop_count: " << loop_count << endl;
  //   cerr << "finalized_count: " << finalized_count << endl;
  // }
  if (distance_vector[added_edge.s] + added_edge.w >=
          distance_vector[added_edge.t] &&
      distance_vector[added_edge.t] + added_edge.w >=
          distance_vector[added_edge.s]) {
    return 0.0;
  }

  double distance_delta = 0;
  auto update = [&](int v, double d, int edge_id, int p) -> void {
    history_tuples.push_back({v, distance_vector[v], parent_edge_vector[v]});

    distance_delta += d - distance_vector[v];
    distance_vector[v] = d;
    parent_edge_vector[v] = ParentEdge(edge_id, p);
  };

  int r = -1;
  int d_r = 0;
  if (distance_vector[added_edge.s] + added_edge.w <
      distance_vector[added_edge.t]) {
    r = added_edge.t;
    d_r = distance_vector[added_edge.s] + added_edge.w;
    update(r, d_r, added_edge.id, added_edge.s);
  } else if (distance_vector[added_edge.t] + added_edge.w <
             distance_vector[added_edge.s]) {
    r = added_edge.s;
    d_r = distance_vector[added_edge.t] + added_edge.w;
    update(r, d_r, added_edge.id, added_edge.t);
  } else {
    exit(1);
  }

  constexpr int MAX_VERTEX_COUNT = 1000;
  static vector<int> visited(MAX_VERTEX_COUNT);
  static vector<int> visited_vertex_list;

  struct S {
    int v;
    int w;
  };
  static auto c = [](const S& s1, const S& s2) { return s1.w > s2.w; };
  static priority_queue<S, vector<S>, decltype(c)> pq(c);

  pq.push(S({r, d_r}));
  while (!pq.empty()) {
    // loop_count++;
    S s = pq.top();
    pq.pop();
    if (visited[s.v]) {
      continue;
    }
    visited[s.v] = 1;
    visited_vertex_list.push_back(s.v);
    for (const Edge e : graph[s.v]) {
      if (!is_edge_active[e.id]) continue;
      if (!visited[e.t] && s.w + e.w < distance_vector[e.t]) {
        update(e.t, s.w + e.w, e.id, e.s);
        pq.push(S({e.t, distance_vector[e.t]}));
      }
    }
  }

  // Reset visited and visited_vertex_list for reuse.
  for (int v : visited_vertex_list) {
    visited[v] = 0;
    // finalized_count++;
  }
  visited_vertex_list.clear();

  return distance_delta;
}

double ExponentialInterpolation(
      double x0, double y0, double x1, double y1, double x) {
  double r = (x - x0) / (x1 - x0);
  return exp(log(y0) + r * (log(y1) - log(y0)));
}

struct BitSet {
 public:
  BitSet(int size) : xs_((size + 64 - 1) >> 6, 0) {}

  void SetTrue(int p) {
    int i = p / 64;
    int b = p & 63;
    xs_[i] |= 1ULL << b;
  }

  bool HasIntersection(const BitSet& other) const {
    for (int i = 0; i < int(min(xs_.size(), other.xs_.size())); i++) {
      if (xs_[i] & other.xs_[i]) {
        return true;
      }
    }
    return false;
  }

  int Count() const {
    int c = 0;
    for (int i = 0; i < int(xs_.size()); i++) {
      c += __builtin_popcountll(xs_[i]);
    }
    return c;
  }

 private:
  vector<uint64> xs_;
};

class Main {
 public:
  Main(int vertex_count, int edge_count, int day_count,
       int max_construction_count_per_day, const vector<Edge>& edges,
       const vector<Int2D>& points, const bool output_score)
      : vertex_count_(vertex_count),
        edge_count_(edge_count),
        day_count_(day_count),
        max_construction_count_per_day_(max_construction_count_per_day),
        edges_(edges),
        points_(points),
        output_score_(output_score),
        graph_(GraphFromEdges(vertex_count, edges)),
        base_distance_matrix_(BaseDistanceMatrix(vertex_count, edges)) {}

  void Run() { RunInternal(); }

 private:
  Random random_ = Random(0);
  TimeMeasurement time_ = TimeMeasurement();

  const int vertex_count_;
  const int edge_count_;
  const int day_count_;
  const int max_construction_count_per_day_;
  const vector<Edge> edges_;
  const vector<Int2D> points_;
  const bool output_score_;

  const vector<vector<Edge>> graph_;
  const vector<vector<int>> base_distance_matrix_;

  static vector<vector<Edge>> GraphFromEdges(int vertex_count,
                                             const vector<Edge>& edges) {
    vector<vector<Edge>> graph(vertex_count);
    for (int edge_id = 0; edge_id < int(edges.size()); edge_id++) {
      const Edge edge = edges[edge_id];
      graph[edge.s].push_back(Edge(edge_id, edge.s, edge.t, edge.w));
      graph[edge.t].push_back(Edge(edge_id, edge.t, edge.s, edge.w));
    }
    return graph;
  }

  static vector<vector<int>> BaseDistanceMatrix(int vertex_count,
                                                const vector<Edge>& edges) {
    vector<vector<Edge>> graph = GraphFromEdges(vertex_count, edges);
    vector<vector<int>> base_distance_matrix =
        Make2D(vertex_count, vertex_count, INF);
    vector<vector<ParentEdge>> parent_edge_vectors =
        Make2D(vertex_count, vertex_count, kInvalidParentEdge);
    for (int s = 0; s < vertex_count; s++) {
      ShortestPath(
          graph, vector<int>(edges.size(), 1), s, base_distance_matrix[s],
          parent_edge_vectors[s]);
    }
    return base_distance_matrix;
  }

  int CountUnconnectedPair(const vector<int>& edge_assignment, int day) {
    DSU dsu(vertex_count_);
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      if (edge_assignment[edge_id] != day) {
        dsu.Union(edges_[edge_id].s, edges_[edge_id].t);
      }
    }
 
    vector<int> connected_component_sizes;
    for (int vertex_id = 0; vertex_id < vertex_count_; vertex_id++) {
      if (dsu.IsRoot(vertex_id)) {
        connected_component_sizes.push_back(dsu.Size(vertex_id));
      }
    }
 
    if (connected_component_sizes.size() == 1) return 0;
 
    int pair_count = 0;
    for (int i = 0; i < int(connected_component_sizes.size()); i++) {
      for (int j = i + 1; j < int(connected_component_sizes.size()); j++) {
        pair_count +=
            connected_component_sizes[i] * connected_component_sizes[j];
      }
    }
    return pair_count;
  }

  void RunMakeConnectedPhase(vector<int>& edge_assignment) {
    vector<int> day_to_unconnected_pair_count = vector<int>(day_count_);
    for (int day = 0; day < day_count_; day++) {
      day_to_unconnected_pair_count[day] =
          CountUnconnectedPair(edge_assignment, day);
    }
 
    while (time_.CurrentMs() < 5700) {
      vector<int> candidate_days;
      for (int day = 0; day < day_count_; day++) {
        if (day_to_unconnected_pair_count[day] > 0) {
          candidate_days.push_back(day);
        }
      }
      if (candidate_days.empty()) {
        break;
      }
 
      int day_0 = candidate_days[random_.RandomInt(0, candidate_days.size())];
      int day_1 = -1;
      while (true) {
        day_1 = random_.RandomInt(0, day_count_);
        if (day_0 != day_1) {
          break;
        }
      }
 
      DSU dsu(vertex_count_);
      for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
        if (edge_assignment[edge_id] != day_0) {
          dsu.Union(edges_[edge_id].s, edges_[edge_id].t);
        }
      }
 
      vector<int> candidate_edges_0;
      vector<int> candidate_edges_1;
      for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
        if (edge_assignment[edge_id] == day_0) {
          if (dsu.Find(edges_[edge_id].s) != dsu.Find(edges_[edge_id].t)) {
            candidate_edges_0.push_back(edge_id);
          }
        }
        if (edge_assignment[edge_id] == day_1) {
          candidate_edges_1.push_back(edge_id);
        }
      }
 
      int current_unconnected_0 =
          day_to_unconnected_pair_count[day_0];
      int current_unconnected_1 =
          day_to_unconnected_pair_count[day_1];
      int edge_0 =
          candidate_edges_0[random_.RandomInt(0, candidate_edges_0.size())];
      int edge_1 =
          candidate_edges_1[random_.RandomInt(0, candidate_edges_1.size())];
      edge_assignment[edge_0] = day_1;
      edge_assignment[edge_1] = day_0;
      day_to_unconnected_pair_count[day_0] =
          CountUnconnectedPair(edge_assignment, day_0);
      day_to_unconnected_pair_count[day_1] =
          CountUnconnectedPair(edge_assignment, day_1);
      if (current_unconnected_0 + current_unconnected_1 <
          day_to_unconnected_pair_count[day_0] +
          day_to_unconnected_pair_count[day_1]) {
        edge_assignment[edge_0] = day_0;
        edge_assignment[edge_1] = day_1;
        day_to_unconnected_pair_count[day_0] = current_unconnected_0;
        day_to_unconnected_pair_count[day_1] = current_unconnected_1;
      }
    }
  }

  vector<vector<tuple<int, double>>> BuildDualCostSparseMatrix() {
    int grid_size = 5;
    if (edge_count_ < 1500) {
      grid_size = 6;
    }
    vector<int> sample_vertex_ids;
    vector<double> weights;
    tie(sample_vertex_ids, weights) = MakeSampleVertexIds(grid_size);
    const int SAMPLE_VERTEX_COUNT = sample_vertex_ids.size();

    vector<int> point_to_grid_id(vertex_count_);
    for (int i = 0; i < vertex_count_; i++) {
      int x = points_[i].x;
      int y = points_[i].y;
      int grid_x = min(x / 125, 7);
      int grid_y = min(y / 125, 7);
      point_to_grid_id[i] = grid_y * 8 + grid_x;
    }

    vector<int> is_edge_active(edge_count_, 1);
    vector<vector<int>> distance_vectors =
        Make2D(SAMPLE_VERTEX_COUNT, vertex_count_, 0);
    vector<vector<ParentEdge>> parent_vectors =
        Make2D(SAMPLE_VERTEX_COUNT, vertex_count_, kInvalidParentEdge);

    EstimatePenalty(
        sample_vertex_ids, is_edge_active, distance_vectors, parent_vectors);

    vector<vector<double>> edge_to_cost_vector =
        Make2D(edge_count_, SAMPLE_VERTEX_COUNT, 0.0);

    // constexpr int MAX_VERTEX_COUNT = 1000;
    vector<vector<BitSet>> edge_to_changed_set_vector =
        Make2D(edge_count_, SAMPLE_VERTEX_COUNT, BitSet(vertex_count_));
    vector<vector<uint64>> edge_to_changed_grid_set_vector =
        Make2D(edge_count_, SAMPLE_VERTEX_COUNT, 0ULL);

    vector<tuple<int, int, ParentEdge>> history_vector_0;
    vector<tuple<int, int, ParentEdge>> history_vector_1;
    for (int i = 0; i < SAMPLE_VERTEX_COUNT; i++) {
      int s = sample_vertex_ids[i];
      for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
        is_edge_active[edge_id] = 0;

        double delta = ShortestPathDeltaWithEdgeDeletion(
            graph_, is_edge_active, edges_[edge_id], s,
            distance_vectors[i], parent_vectors[i],
            history_vector_0);
        delta /= vertex_count_ - 1;
        edge_to_cost_vector[edge_id][i] = delta;

        is_edge_active[edge_id] = 1;

        edge_to_changed_set_vector[edge_id][i].SetTrue(edges_[edge_id].s);
        edge_to_changed_grid_set_vector[edge_id][i] |=
            1ULL << point_to_grid_id[edges_[edge_id].s];
        edge_to_changed_set_vector[edge_id][i].SetTrue(edges_[edge_id].t);
        edge_to_changed_grid_set_vector[edge_id][i] |=
            1ULL << point_to_grid_id[edges_[edge_id].t];
        while (!history_vector_0.empty()) {
          const auto [v, d, e] = history_vector_0.back();
          history_vector_0.pop_back();

          edge_to_changed_set_vector[edge_id][i].SetTrue(v);
          edge_to_changed_grid_set_vector[edge_id][i] |=
              1ULL << point_to_grid_id[v];
          if (parent_vectors[i][v].id >= 0) {
            edge_to_changed_set_vector[edge_id][i].SetTrue(
                parent_vectors[i][v].t);
            edge_to_changed_grid_set_vector[edge_id][i] |=
                1ULL << point_to_grid_id[parent_vectors[i][v].t];
          }

          distance_vectors[i][v] = d;
          parent_vectors[i][v] = e;
        }
      }
    }

    vector<vector<int>> edge_to_changed_count_vector =
        Make2D(edge_count_, SAMPLE_VERTEX_COUNT, 0);
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      for (int i = 0; i < SAMPLE_VERTEX_COUNT; i++) {
        edge_to_changed_count_vector[edge_id][i] =
            edge_to_changed_set_vector[edge_id][i].Count();
      }
    }

    vector<vector<double>> dual_cost_matrix =
        Make2D(edge_count_, edge_count_, 0.0);

    cerr << "Started dual cost calculation: "
         << time_.CurrentMs() << "msec" << endl;

    for (int i = 0; i < SAMPLE_VERTEX_COUNT; i++) {
      int s = sample_vertex_ids[i];
      for (int edge_id_0 = 0; edge_id_0 < edge_count_; edge_id_0++) {
        const BitSet& changed_set_0 =
            edge_to_changed_set_vector[edge_id_0][i];
        uint64 changed_grid_set_0 =
            edge_to_changed_grid_set_vector[edge_id_0][i];

        is_edge_active[edge_id_0] = 0;
        ShortestPathDeltaWithEdgeDeletion(
            graph_, is_edge_active, edges_[edge_id_0], s,
            distance_vectors[i], parent_vectors[i],
            history_vector_0);
        for (int edge_id_1 = 0; edge_id_1 < edge_count_; edge_id_1++) {
          if (edge_id_0 == edge_id_1) continue;

          int count_0 = edge_to_changed_count_vector[edge_id_0][i];
          int count_1 = edge_to_changed_count_vector[edge_id_1][i];
          if (count_0 < count_1) {
            // It's more efficient to compute when count_0 > count_1.
            continue;
          } else if (count_0 == count_1 && edge_id_0 > edge_id_1) {
            // When count_0 == count_1, compute when edge_id_0 < edge_id_1.
            continue;
          }
          uint64 changed_grid_set_1 =
              edge_to_changed_grid_set_vector[edge_id_1][i];
          if ((changed_grid_set_0 & changed_grid_set_1) == 0) {
            continue;
          }
          const BitSet& changed_set_1 =
              edge_to_changed_set_vector[edge_id_1][i];
          if (!(changed_set_0.HasIntersection(changed_set_1))) {
            continue;
          }

          is_edge_active[edge_id_1] = 0;

          double delta = ShortestPathDeltaWithEdgeDeletion(
              graph_, is_edge_active, edges_[edge_id_1], s, distance_vectors[i],
              parent_vectors[i], history_vector_1);
          delta /= vertex_count_ - 1;
          double dual_delta = delta - edge_to_cost_vector[edge_id_1][i];
          dual_cost_matrix[edge_id_0][edge_id_1] +=
              dual_delta / SAMPLE_VERTEX_COUNT;
          dual_cost_matrix[edge_id_1][edge_id_0] +=
              dual_delta / SAMPLE_VERTEX_COUNT;

          is_edge_active[edge_id_1] = 1;
          for (int history_index = history_vector_1.size() - 1;
                history_index >= 0; history_index--) {
            const auto [v, d, e] = history_vector_1[history_index];
            distance_vectors[i][v] = d;
            parent_vectors[i][v] = e;
          }
          history_vector_1.clear();
        }

        is_edge_active[edge_id_0] = 1;
        for (int history_index = history_vector_0.size() - 1;
              history_index >= 0; history_index--) {
          const auto [v, d, e] = history_vector_0[history_index];
          distance_vectors[i][v] = d;
          parent_vectors[i][v] = e;
        }
        history_vector_0.clear();
      }
    }

    cerr << "Finished dual cost calculation: "
         << time_.CurrentMs() << "msec" << endl;
    cerr << endl;

    vector<vector<tuple<int, double>>> dual_cost_sparse_matrix(edge_count_);
    for (int i = 0; i < edge_count_; i++) {
      for (int j = 0; j < edge_count_; j++) {
        if (abs(dual_cost_matrix[i][j]) > 1.0E-3) {
          dual_cost_sparse_matrix[i].push_back({j, dual_cost_matrix[i][j]});
        }
      }
    }
    return dual_cost_sparse_matrix;
  }

  void RunPairwiseApproximationPhase(
      vector<int>& edge_assignment,
      vector<vector<int>>& day_to_is_edge_active,
      const vector<vector<tuple<int, double>>>& dual_cost_sparse_matrix) {


    vector<int> day_to_assignment_count(day_count_);
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      day_to_assignment_count[edge_assignment[edge_id]]++;
    }

    cerr << "Started pairwise approximation SA: "
        << time_.CurrentMs() << "msec" << endl;
    int sa_count = 0;
    int sa_update_count = 0;
    double delta_total = 0.0;
    double sa_start_time = time_.CurrentMs();
    double sa_end_time = 2850.0;
    while (true) {
      double current_time = time_.CurrentMs();
      if (current_time >= sa_end_time) {
        break;
      }
      constexpr double MAX_TEMPERATURE = 100.0;
      constexpr double MIN_TEMPERATURE = 1.0;
      double temperature = ExponentialInterpolation(
          sa_start_time, MAX_TEMPERATURE, sa_end_time, MIN_TEMPERATURE,
          current_time);

      int edge_id = random_.RandomInt(0, edge_count_);
      int day_0 = edge_assignment[edge_id];
      int day_1 = random_.RandomInt(0, day_count_);
      if (day_0 == day_1) continue;
      if (day_to_assignment_count[day_1] >= max_construction_count_per_day_) {
        continue;
      }

      sa_count++;

      double delta = 0.0;
      for (const auto& [other_edge_id, dual_cost] :
           dual_cost_sparse_matrix[edge_id]) {
        if (edge_assignment[other_edge_id] == day_0) {
          delta -= dual_cost;
        } else if (edge_assignment[other_edge_id] == day_1) {
          delta += dual_cost;
        }
      }
      // for (int i = 0; i < edge_count_; i++) {
      //   if (edge_assignment[i] == day_0) {
      //     delta -= dual_cost_matrix[edge_id][i];
      //   } else if (edge_assignment[i] == day_1) {
      //     delta += dual_cost_matrix[edge_id][i];
      //   }
      // }

      if (exp(-delta / temperature) >= random_.RandomDouble(0, 1)) {
        // Connectivity check.
        int s = edges_[edge_id].s;
        int t = edges_[edge_id].t;

        auto check_connectivity = [&]() -> bool {
          static deque<int> q;
          constexpr int MAX_VERTEX_COUNT = 1000;
          static vector<int> visited(MAX_VERTEX_COUNT);
          static vector<int> visited_vector;

          q.push_back(s);
          while (!q.empty()) {
            int v = q.front();
            q.pop_front();
            for (const auto& e : graph_[v]) {
              if (!day_to_is_edge_active[day_1][e.id]) continue;
              if (e.id == edge_id) continue;
              if (e.t == t) {
                q.clear();
                for (int v : visited_vector) {
                  visited[v] = 0;
                }
                visited_vector.clear();
                return true;
              }
              if (!visited[e.t]) {
                q.push_back(e.t);
                visited[e.t] = 1;
                visited_vector.push_back(e.t);
              }
            }
          }

          for (int v : visited_vector) {
            visited[v] = 0;
          }
          visited_vector.clear();
          return false;
        };
        
        if (check_connectivity()) {
          delta_total += delta / day_count_;
          sa_update_count++;

          edge_assignment[edge_id] = day_1;
          day_to_is_edge_active[day_0][edge_id] = 1;
          day_to_is_edge_active[day_1][edge_id] = 0;
          day_to_assignment_count[day_0]--;
          day_to_assignment_count[day_1]++;
        }
      }
      if (sa_count % 500000 == 0) {
        cerr << "sa_count: " << sa_count << endl;
        cerr << "sa_update_count: " << sa_update_count << endl;
        cerr << "delta_total: " << delta_total << endl;
      }
    }
    cerr << "sa_count: " << sa_count << endl;
    cerr << "sa_update_count: " << sa_update_count << endl;
    cerr << "delta_total: " << delta_total << endl;
    cerr << "Finished pairwise approximation SA: "
        << time_.CurrentMs() << "msec" << endl;
    cerr << endl;
  }

  void RunInternal() {
    vector<int> edge_assignment = InitializeEdgeAssignment();

    RunMakeConnectedPhase(edge_assignment);

    vector<vector<int>> day_to_is_edge_active =
        InitializeDayToIsEdgeActive(edge_assignment);

    vector<vector<tuple<int, double>>> dual_cost_sparse_matrix =
        BuildDualCostSparseMatrix();
    RunPairwiseApproximationPhase(
        edge_assignment, day_to_is_edge_active, dual_cost_sparse_matrix);

    RunMainPhase(edge_assignment, day_to_is_edge_active);

    if (output_score_) {
      int64 score = ComputeScore(day_to_is_edge_active);
      cerr << "score: " << score << endl;
    }

    // Write output.
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      cout << edge_assignment[edge_id] + 1 << " ";
    }
    cout << endl;
  }

  vector<int> InitializeEdgeAssignment() {
    vector<int> edge_assignment(edge_count_);
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      edge_assignment[edge_id] = edge_id % day_count_;
    }
    return edge_assignment;
  }

  vector<vector<int>> InitializeDayToIsEdgeActive(
      const vector<int>& edge_assignment) {
    vector<vector<int>> day_to_is_edge_active =
        Make2D(day_count_, edge_count_, 1);
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      day_to_is_edge_active[edge_assignment[edge_id]][edge_id] = 0;
    }
    return day_to_is_edge_active;
  }

  tuple<vector<int>, vector<double>> MakeSampleVertexIds(int grid_size) {
    auto get_closest_vertex = [&](double x, double y) -> int {
      int min_v = 0;
      double min_d2 = (points_[0].x - x) * (points_[0].x - x) +
                      (points_[0].y - y) * (points_[0].y - y);
      for (int v = 1; v < vertex_count_; v++) {
        double d2 = (points_[v].x - x) * (points_[v].x - x) +
                      (points_[v].y - y) * (points_[v].y - y);
        if (d2 < min_d2) {
          min_v = v;
          min_d2 = d2;
        }
      }
      return min_v;
    };

    vector<int> sample_vertex_ids;
    for (int dy_index = 0; dy_index < grid_size; dy_index++) {
      for (int dx_index = 0; dx_index < grid_size; dx_index++) {
        double dx = 1000.0 / (2 * grid_size) * (2 * dx_index + 1) - 500.0;
        double dy = 1000.0 / (2 * grid_size) * (2 * dy_index + 1) - 500.0;
        if (dx * dx + dy * dy > 500.0 * 500.0) continue;
        sample_vertex_ids.push_back(get_closest_vertex(500.0 + dx, 500.0 + dy));
      }
    }

    vector<double> weights(sample_vertex_ids.size());
    for (int v = 0; v < vertex_count_; v++) {
      double d2_min = DBL_MAX;
      int d2_min_cluster_id = -1;
      for (int cluster_id = 0; cluster_id < int(sample_vertex_ids.size()); cluster_id++) {
        double dx = points_[v].x - points_[sample_vertex_ids[cluster_id]].x;
        double dy = points_[v].y - points_[sample_vertex_ids[cluster_id]].y;
        double d2 = dx * dx + dy * dy;
        if (d2 < d2_min) {
          d2_min = d2;
          d2_min_cluster_id = cluster_id;
        }
      }
      weights[d2_min_cluster_id] += 1.0;
    }
    for (int i = 0; i < int(sample_vertex_ids.size()); i++) {
      weights[i] /= vertex_count_;
    }
    // cerr << weights << endl;

    // if (int(sample_vertex_ids.size()) != sample_count) {
    //   cerr << "Unexpected sample_vertex_ids.size(): "
    //        << sample_vertex_ids.size() << endl;
    //   exit(1);
    // }

    // K-means
    // constexpr int K_MEANS_LOOP_COUNT = 10;
    // for (int k_means_loop = 0; k_means_loop < K_MEANS_LOOP_COUNT;
    //      k_means_loop++) {
    //   vector<vector<int>> cluster_to_vertex_ids(sample_count);
    //   for (int v = 0; v < vertex_count_; v++) {
    //     double d2_min = DBL_MAX;
    //     int d2_min_cluster_id = -1;
    //     for (int cluster_id = 0; cluster_id < sample_count; cluster_id++) {
    //       double dx = points_[v].x - points_[sample_vertex_ids[cluster_id]].x;
    //       double dy = points_[v].y - points_[sample_vertex_ids[cluster_id]].y;
    //       double d2 = dx * dx + dy * dy;
    //       if (d2 < d2_min) {
    //         d2_min = d2;
    //         d2_min_cluster_id = cluster_id;
    //       }
    //     }
    //     cluster_to_vertex_ids[d2_min_cluster_id].push_back(v);
    //   }
    //   for (int cluster_id = 0; cluster_id < sample_count; cluster_id++) {
    //     cerr << cluster_to_vertex_ids[cluster_id].size() << " ";
    //   }
    //   cerr << endl;
    //   for (int cluster_id = 0; cluster_id < sample_count; cluster_id++) {
    //     double x_sum = 0;
    //     double y_sum = 0;
    //     for (int v : cluster_to_vertex_ids[cluster_id]) {
    //       x_sum += points_[v].x;
    //       y_sum += points_[v].y;
    //     }
    //     double x_mean = x_sum / cluster_to_vertex_ids[cluster_id].size();
    //     double y_mean = y_sum / cluster_to_vertex_ids[cluster_id].size();

    //     double d2_min = DBL_MAX;
    //     int d2_min_v_id = -1;
    //     for (int v = 0; v < vertex_count_; v++) {
    //       double dx = points_[v].x - x_mean;
    //       double dy = points_[v].y - y_mean;
    //       double d2 = dx * dx + dy * dy;
    //       if (d2 < d2_min) {
    //         d2_min = d2;
    //         d2_min_v_id = v;
    //       }
    //     }

    //     sample_vertex_ids[cluster_id] = d2_min_v_id;
    //   }
    // }

    return {sample_vertex_ids, weights};
  }

  void RunMainPhase(
      vector<int>& edge_assignment,
      vector<vector<int>>& day_to_is_edge_active) {
    vector<int> day_to_assignment_count(day_count_);
    for (int edge_id = 0; edge_id < edge_count_; edge_id++) {
      day_to_assignment_count[edge_assignment[edge_id]]++;
    }

    vector<int> sample_vertex_ids;
    vector<double> weights;
    tie(sample_vertex_ids, weights)= MakeSampleVertexIds(6);
    const int SAMPLE_VERTEX_COUNT = sample_vertex_ids.size();

    vector<vector<vector<int>>> day_to_distance_vectors =
        Make3D(day_count_, SAMPLE_VERTEX_COUNT, vertex_count_, INF);
    vector<vector<vector<ParentEdge>>> day_to_parent_vectors =
        Make3D(
            day_count_, SAMPLE_VERTEX_COUNT, vertex_count_, kInvalidParentEdge);
    for (int day = 0; day < day_count_; day++) {
      EstimatePenalty(
          sample_vertex_ids, day_to_is_edge_active[day],
          day_to_distance_vectors[day], day_to_parent_vectors[day]);
    }

    int sa_count = 0;
    int sa_success_count = 0;
    double delta_total = 0;
    const double sa_start_time = time_.CurrentMs();
    const double sa_end_time = 5700;
    vector<vector<tuple<int, int, ParentEdge>>> history_vectors_0(
        SAMPLE_VERTEX_COUNT);
    vector<vector<tuple<int, int, ParentEdge>>> history_vectors_1(
        SAMPLE_VERTEX_COUNT);

    cerr << "Started main SA: "
         << time_.CurrentMs() << "msec" << endl;
    while (true) {
      double current_time = time_.CurrentMs();
      if (current_time >= sa_end_time) {
        break;
      }

      constexpr double MAX_TEMPERATURE = 1.0;
      constexpr double MIN_TEMPERATURE = 0.1;
      double temperature = ExponentialInterpolation(
          sa_start_time, MAX_TEMPERATURE, sa_end_time, MIN_TEMPERATURE,
          current_time);

      int edge_id_0 = random_.RandomInt(0, edge_count_);
      // int edge_id_1 = random_.RandomInt(0, edge_count_);
      int day_0 = edge_assignment[edge_id_0];
      int day_1 = random_.RandomInt(0, day_count_);
      if (day_0 == day_1) continue;
      if (day_to_assignment_count[day_1] >= max_construction_count_per_day_) {
        continue;
      }

      sa_count++;

      double delta_0 = EstimatePenaltyDelta(
          sample_vertex_ids, weights, edge_id_0, -1, day_to_is_edge_active[day_0],
          day_to_distance_vectors[day_0], day_to_parent_vectors[day_0],
          history_vectors_0);

      double delta_1 = EstimatePenaltyDelta(
          sample_vertex_ids, weights, -1, edge_id_0, day_to_is_edge_active[day_1],
          day_to_distance_vectors[day_1], day_to_parent_vectors[day_1],
          history_vectors_1);

      double delta = delta_0 + delta_1;
      if (exp(-delta / temperature) >= random_.RandomDouble(0, 1)) {
        delta_total += delta / day_count_;
        edge_assignment[edge_id_0] = day_1;
        day_to_assignment_count[day_0]--;
        day_to_assignment_count[day_1]++;
        sa_success_count++;
      } else {
        // Revert variables.
        day_to_is_edge_active[day_0][edge_id_0] = 0;
        day_to_is_edge_active[day_1][edge_id_0] = 1;

        for (int i = 0; i < int(history_vectors_0.size()); i++) {
          for (int history_index = history_vectors_0[i].size() - 1;
               history_index >= 0; history_index--) {
            const auto [v, d, e] = history_vectors_0[i][history_index];
            day_to_distance_vectors[day_0][i][v] = d;
            day_to_parent_vectors[day_0][i][v] = e;
          }
        }

        for (int i = 0; i < int(history_vectors_1.size()); i++) {
          for (int history_index = history_vectors_1[i].size() - 1;
               history_index >= 0; history_index--) {
            const auto [v, d, e] = history_vectors_1[i][history_index];
            day_to_distance_vectors[day_1][i][v] = d;
            day_to_parent_vectors[day_1][i][v] = e;
          }
        }
      }

      for (int i = 0; i < SAMPLE_VERTEX_COUNT; i++) {
        history_vectors_0[i].clear();
        history_vectors_1[i].clear();
      }

      if (sa_count % 50000 == 0) {
        cerr << "sa_count: " << sa_count << endl;
        cerr << "sa_success_count: "
             << sa_success_count << endl;
        cerr << "delta_total: " << delta_total << endl;
      }
    }
    cerr << "Finished main SA: "
         << time_.CurrentMs() << "msec" << endl;
    cerr << "sa_count: " << sa_count << endl;
    cerr << "sa_success_count: " << sa_success_count << endl;
    cerr << "delta_total: " << delta_total << endl;
  }

  double ComputeExactPenalty(const vector<int>& is_edge_active) {
    vector<int> distance_vector(vertex_count_, INF);
    vector<ParentEdge> parent_edge_vector(vertex_count_, kInvalidParentEdge);

    double f = 0.0;
    for (int s = 0; s < vertex_count_; s++) {
      ShortestPath(
          graph_, is_edge_active, s, distance_vector, parent_edge_vector);
      for (int v = 0; v < vertex_count_; v++) {
        if (v == s) continue;
        f += distance_vector[v] - base_distance_matrix_[s][v];
      }
    }

    f /= vertex_count_ * (vertex_count_ - 1);
    // cerr << "exact_penalty: " << f << endl;
    return f;
  }

  double EstimatePenaltyDelta(
      const vector<int>& vertex_ids,
      const vector<double>& weights,
      int added_edge_id,
      int deleted_edge_id,
      vector<int>& is_edge_active,
      vector<vector<int>>& distance_vectors,
      vector<vector<ParentEdge>>& parent_edge_vectors,
      vector<vector<tuple<int, int, ParentEdge>>>& history_vectors) {
    double delta = 0;

    if (added_edge_id >= 0) {
      is_edge_active[added_edge_id] = 1;
      for (int i = 0; i < int(vertex_ids.size()); i++) {
        int s = vertex_ids[i];
        delta += ShortestPathDeltaWithEdgeAddition(
            graph_, is_edge_active, edges_[added_edge_id], s,
            distance_vectors[i], parent_edge_vectors[i],
            history_vectors[i]) * weights[i];
      } 
    }

    if (deleted_edge_id >= 0) {
      is_edge_active[deleted_edge_id] = 0;
      for (int i = 0; i < int(vertex_ids.size()); i++) {
        int s = vertex_ids[i];
        delta += ShortestPathDeltaWithEdgeDeletion(
            graph_, is_edge_active, edges_[deleted_edge_id], s,
            distance_vectors[i], parent_edge_vectors[i],
            history_vectors[i]) * weights[i];
      }
    }

    delta /= (vertex_count_ - 1);
    return delta;
  }

  double EstimatePenalty(
      const vector<int>& vertex_ids,
      const vector<int>& is_edge_active,
      vector<vector<int>>& distance_vectors,
      vector<vector<ParentEdge>>& parent_edge_vectors) {
    for (int i = 0; i < int(vertex_ids.size()); i++) {
      int s = vertex_ids[i];
      ShortestPath(graph_, is_edge_active, s, distance_vectors[i],
                   parent_edge_vectors[i]);
    }
    double f = 0.0;
    for (int i = 0; i < int(vertex_ids.size()); i++) {
      for (int v = 0; v < vertex_count_; v++) {
        if (v == vertex_ids[i]) continue;
        f += distance_vectors[i][v];
      }
    }
    f /= vertex_ids.size() * (vertex_count_ - 1);
    return f;
  }

  int64 ComputeScore(const vector<vector<int>>& day_to_is_edge_active) {
    double f_sum = 0;
    for (int day = 0; day < day_count_; day++) {
      f_sum += ComputeExactPenalty(day_to_is_edge_active[day]);
    }

    int64 score = round(f_sum / day_count_ * 1000.0);
    return score;
  }
};

int main(int argc, char **argv) {
  ios::sync_with_stdio(false);
  cin.tie(0);

  bool output_score = false;
  if (argc >= 2) {
    string arg_1 = argv[1];
    if (arg_1 == "1") {
      output_score = true;
    }
  }

  int vertex_count;
  cin >> vertex_count;
  int edge_count;
  cin >> edge_count;
  int day_count;
  cin >> day_count;
  int max_construction_per_day;
  cin >> max_construction_per_day;

  vector<Edge> edges;
  for (int edge_id = 0; edge_id < edge_count; edge_id++) {
    int s, t;
    double w;
    cin >> s >> t >> w;
    s--;
    t--;
    edges.push_back(Edge(edge_id, s, t, w));
  }

  vector<Int2D> points;
  for (int vertex_id = 0; vertex_id < vertex_count; vertex_id++) {
    double x, y;
    cin >> x >> y;
    points.push_back(Int2D(x, y));
  }

  Main main(vertex_count, edge_count, day_count, max_construction_per_day,
            edges, points, output_score);
  main.Run();
}
