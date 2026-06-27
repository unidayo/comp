#include <bits/stdc++.h>
constexpr int DEBUG = 0;
using namespace std;
using int64 = long long;

constexpr int N = 30;
constexpr int Q_COUNT = 1000;

double Square(double x) { return x * x; }

double Norm(const vector<double>& xs) {
  double squared_sum = 0;
  for (int i = 0; i < int(xs.size()); i++) {
    squared_sum += xs[i] * xs[i];
  }
  return sqrt(squared_sum);
}

template<typename T>
vector<vector<T>> Make2D(int d1, int d2, T default_value) {
  return vector<vector<T>>(d1, vector<T>(d2, default_value));
}

template<class T>
struct WEdge {
  int s, t; T w;
  WEdge(int s, int t, T w) : s(s), t(t), w(w) {}
};
using WEdgeD = WEdge<double>;

template<class T>
tuple<vector<T>, vector<int>> ShortestPath(vector<vector<WEdge<T>>>& g, int s) {
  int n = g.size(); vector<int> visited(n);
  vector<T> ds(n, numeric_limits<T>::max()); vector<int> ps(n, -1);
  struct S { int v; T w; }; auto c = [](S s1, S s2) { return s1.w > s2.w; };
  priority_queue<S, vector<S>, decltype(c)> pq(c); pq.push(S({s, 0}));
  while(!pq.empty()) {
    S s = pq.top(); pq.pop(); if (visited[s.v]) { continue; } visited[s.v] = 1;
    for (const auto e : g[s.v]) {
      if (!visited[e.t] && s.w + e.w < ds[e.t]) {
        ds[e.t] = s.w + e.w; ps[e.t] = s.v; pq.push(S({e.t, ds[e.t]}));
      }
    }
  }
  return make_tuple(ds, ps);
}

struct IVD {
  int index; double value;
  IVD(int index, double value) : index(index), value(value) {}
};
using SparseVectorD = vector<IVD>;

// n: Number of features.
// m: Number of items.
vector<double> LinearRegression(
    int n, int m,
    const vector<SparseVectorD>& features,
    const vector<double>& labels,
    const vector<double>& item_weights,
    const vector<double>& ridge_weights,
    const vector<double>& initial_ws,
    int iter_count) {
  vector<double> ws = initial_ws;
  auto make_predictions = [&](
      const vector<double>& deltas, double d) -> vector<double> {
    vector<double> ps(m);
    for (int i = 0; i < m; i++) {
      for (const IVD& feature_iv : features[i]) {
        int k = feature_iv.index;
        double v = feature_iv.value;
        ps[i] += (ws[k] + deltas[k] * d) * v;
      }
    }
    return ps;
  };

  auto compute_cost = [&](
      const vector<double>& deltas, double d) -> double {
    double cost = 0;
    for (int k = 0; k < n; k++) {
      cost += ridge_weights[k] * Square(ws[k] + deltas[k] * d);
    }
    vector<double> ps = make_predictions(deltas, d);
    for (int i = 0; i < m; i++) {
      cost += item_weights[i] * Square(ps[i] - labels[i]);
    }
    return cost;
  };

  // vector<double> output_ws = ws;
  for (int iter = 0; iter < iter_count; iter++) {
    vector<double> ps = make_predictions(vector<double>(n, 0), 0);

    vector<double> deltas(n);
    // k: Feature index.
    for (int k = 0; k < n; k++) {
      deltas[k] -= 2.0 * ridge_weights[k] * ws[k];
    }
    // i: Item index.
    for (int i = 0; i < m; i++) {
      for (const IVD& feature_iv : features[i]) {
        int k = feature_iv.index;
        double v = feature_iv.value;
        deltas[k] -= 2.0 * item_weights[i] * (ps[i] - labels[i]) * v; 
      }
    }

    double norm = Norm(deltas);
    for (int k = 0; k < n; k++) {
      deltas[k] /= norm;
    }

    double min_cost = compute_cost(deltas, 0);
    double min_d = 0;
    constexpr double MIN_STEP = 1.0;
    double d = MIN_STEP;
    while (true) {
      double cost = compute_cost(deltas, d);
      if (cost < min_cost) {
        min_d = d;
        d *= 2;
      } else {
        break;
      }
    }

    for (int k = 0; k < n; k++) {
      ws[k] += deltas[k] * min_d;
    }
  }
  return ws;
}

enum State {
  INITIAL,
  M1,
  M2
};

template<typename T> ostream& operator<<(ostream& s, const vector<T>& v) {
  s << "[";
  for (int i = 0; i < int(v.size()); i++) {
    if (i > 0) { s << ", "; } s << v[i];
  }
  s << "]";
  return s;
}

class Main {
 public:
  void Run() {
    RunInternal();
  }
 private:
  double edge_weight_base = 5000.0;
  double base_width = 3000.0;
  const double delta_width = 1000.0;

  default_random_engine random_engine;

  // To get feature indexes.
  // These are set up in SetUpFeatureIndexes().
  vector<int> h0_base_index;
  vector<int> h1_base_index;
  vector<vector<int>> h_delta_index;
  vector<int> v0_base_index;
  vector<int> v1_base_index;
  vector<vector<int>> v_delta_index;
  int feature_count;

  // For linear regression.
  vector<SparseVectorD> items;
  vector<double> labels;
  vector<double> item_weights;
  vector<double> ridge_weights;
  vector<double> ws;

  vector<vector<int>> paths;

  vector<int> h0_bs;
  vector<int> v0_bs;

  // Vector of (feature_index, weight) for each horizontal edge.
  vector<vector<SparseVectorD>> h_feature_weights;
  // Vector of (feature_index, weight) for each vertical edge.
  vector<vector<SparseVectorD>> v_feature_weights;

  vector<double> results;

  vector<vector<int>> h_use_count;
  vector<vector<int>> v_use_count;

  State state = INITIAL;

  void SetEdgeFeatureWeights() {
    h_feature_weights = Make2D(N, N - 1, SparseVectorD());
    v_feature_weights = Make2D(N - 1, N, SparseVectorD());

    for (int r = 0; r < N; r++) {
      for (int c = 0; c < N - 1; c++) {
        if (state == INITIAL) {
          double p = double((N - 1) - c) / (N - 1);
          h_feature_weights[r][c].push_back({h0_base_index[r], p});
          h_feature_weights[r][c].push_back({h1_base_index[r], 1.0 - p});
        } else if (state == M1) {
          h_feature_weights[r][c].push_back({h0_base_index[r], 1.0});
        } else if (state == M2) {
          int b = h0_bs[r];
          if (b % 2 == 0) {
            if (c < b / 2) {
              h_feature_weights[r][c].push_back({h0_base_index[r], 1.0});
            } else if (c == b / 2) {
              h_feature_weights[r][c].push_back({h0_base_index[r], 0.5});
              h_feature_weights[r][c].push_back({h1_base_index[r], 0.5});
            } else {
              h_feature_weights[r][c].push_back({h1_base_index[r], 1.0});
            }
          } else {
            if (c <= b / 2) {
              h_feature_weights[r][c].push_back({h0_base_index[r], 1.0});
            } else {
              h_feature_weights[r][c].push_back({h1_base_index[r], 1.0});
            }
          }
        } else {
          exit(1);
        }

        h_feature_weights[r][c].push_back({h_delta_index[r][c], 1.0});
      }
    }

    for (int r = 0; r < N - 1; r++) {
      for (int c = 0; c < N; c++) {
        if (state == INITIAL) {
          double p = double((N - 1) - r) / (N - 1);
          v_feature_weights[r][c].push_back({v0_base_index[c], p});
          v_feature_weights[r][c].push_back({v1_base_index[c], 1.0 - p});
        } else if (state == M1) {
          v_feature_weights[r][c].push_back({v0_base_index[c], 1.0});
        } else if (state == M2) {
          int b = v0_bs[c];
          if (b % 2 == 0) {
            if (r < b / 2) {
              v_feature_weights[r][c].push_back({v0_base_index[c], 1.0});
            } else if (r == b / 2) {
              v_feature_weights[r][c].push_back({v0_base_index[c], 0.5});
              v_feature_weights[r][c].push_back({v1_base_index[c], 0.5});
            } else {
              v_feature_weights[r][c].push_back({v1_base_index[c], 1.0});
            }
          } else {
            if (r <= b / 2) {
              v_feature_weights[r][c].push_back({v0_base_index[c], 1.0});
            } else {
              v_feature_weights[r][c].push_back({v1_base_index[c], 1.0});
            }
          }
        } else {
          exit(1);
        }

        v_feature_weights[r][c].push_back({v_delta_index[r][c], 1.0});
      }
    }
  }

  static int VertexId(int r, int c) {
    return r * N + c;
  }

  static pair<int, int> DecodeVertexId(int id) {
    int r = id / N;
    int c = id % N;
    return {r, c};
  }

  void SetUpFeatureIndexes() {
    h0_base_index = vector<int>(N, -1);
    h1_base_index = vector<int>(N, -1);
    h_delta_index = Make2D(N, N - 1, -1);
    v0_base_index = vector<int>(N, -1);
    v1_base_index = vector<int>(N, -1);
    v_delta_index = Make2D(N - 1, N, -1);

    int current_index = 0;
    for (int i = 0; i < N; i++) {
      h0_base_index[i] = current_index++;
    }
    for (int i = 0; i < N; i++) {
      h1_base_index[i] = current_index++;
    }
    for (int r = 0; r < N; r++) {
      for (int c = 0; c < N - 1; c++) {
        h_delta_index[r][c] = current_index++;
      }
    }
    for (int i = 0; i < N; i++) {
      v0_base_index[i] = current_index++;
    }
    for (int i = 0; i < N; i++) {
      v1_base_index[i] = current_index++;
    }
    for (int r = 0; r < N - 1; r++) {
      for (int c = 0; c < N; c++) {
        v_delta_index[r][c] = current_index++;
      }
    }
    feature_count = current_index;
  }

  // Should be called after feature_count is initialized in
  // SetUpFeatureIndexes().
  void SetUpLinearRegressionVariables() {
    items = vector<SparseVectorD>();
    labels = vector<double>();
    item_weights = vector<double>();
    ridge_weights = vector<double>(feature_count);
    ws = vector<double>(feature_count);
  }

  void SetUpRidgeWeights() {
    for (int i = 0; i < N; i++) {
      ridge_weights[h0_base_index[i]] = 1.0 / Square(base_width);
    }
    for (int i = 0; i < N; i++) {
      ridge_weights[h1_base_index[i]] = 1.0 / Square(base_width);
    }
    for (int r = 0; r < N; r++) {
      for (int c = 0; c < N - 1; c++) {
        ridge_weights[h_delta_index[r][c]] = 1.0 / Square(delta_width);
      }
    }
    for (int i = 0; i < N; i++) {
      ridge_weights[v0_base_index[i]] = 1.0 / Square(base_width);
    }
    for (int i = 0; i < N; i++) {
      ridge_weights[v1_base_index[i]] = 1.0 / Square(base_width);
    }
    for (int r = 0; r < N - 1; r++) {
      for (int c = 0; c < N; c++) {
        ridge_weights[v_delta_index[r][c]] = 1.0 / Square(delta_width);
      }
    }
  }

  vector<vector<WEdgeD>> MakeGraph(
      const vector<vector<double>>& h_table,
      const vector<vector<double>>& v_table) {
    vector<vector<WEdgeD>> graph(N * N);
    for (int y = 0; y < N; y++) {
      for (int x = 0; x + 1 < N; x++) {
        int v0 = VertexId(y, x);
        int v1 = VertexId(y, x + 1);
        double w = h_table[y][x];
        graph[v0].push_back({v0, v1, w});
        graph[v1].push_back({v1, v0, w});
      }
    }
    for (int y = 0; y + 1 < N; y++) {
      for (int x = 0; x < N; x++) {
        int v0 = VertexId(y, x);
        int v1 = VertexId(y + 1, x);
        double w = v_table[y][x];
        graph[v0].push_back({v0, v1, w});
        graph[v1].push_back({v1, v0, w});
      }
    }
    return graph;
  }

  // Makes random h_table and v_table for the exploration phase.
  tuple<vector<vector<double>>, vector<vector<double>>>
  MakeRandomEdgeWeights() {
    vector<vector<double>> h_table = Make2D(N, N - 1, 0.0);
    vector<vector<double>> v_table = Make2D(N - 1, N, 0.0);
    for (int r = 0; r < N; r++) {
      for (int c = 0; c < N - 1; c++) {
        double random = uniform_real_distribution<double>(0, 1)(random_engine);
        h_table[r][c] = 1.0 + random;
      }
    }
    for (int r = 0; r < N - 1; r++) {
      for (int c = 0; c < N; c++) {
        double random = uniform_real_distribution<double>(0, 1)(random_engine);
        v_table[r][c] = 1.0 + random;
      }
    }

    return {h_table, v_table};
  }

  // Makes h_table and v_table from linear regression variables.
  tuple<vector<vector<double>>, vector<vector<double>>>
  MakeEdgeWeights(int q_index) {
    vector<double> adjusted_ws = ws;
    double bonus_sum = 0;
    if (q_index < 300) {
      constexpr double MAX_BONUS = 3000.0;
      constexpr double MAX_COUNT = 15.0;

      auto bonus_function = [&](int count) -> double {
        double ratio = min(count / MAX_COUNT, 1.0);
        return MAX_BONUS * Square(1.0 - ratio);
      };

      for (int r = 0; r < N; r++) {
        int count_0 = 0;
        int count_1 = 0;
        if (state != M1) {
          for (int c = 0; c < N - 1; c++) {
            if (c < N / 3) {
              count_0 += h_use_count[r][c];
            } else if (c >= (N - 1) - (N / 3)) {
              count_1 += h_use_count[r][c];
            }
          }
        } else {
          for (int c = 0; c < N - 1; c++) {
            count_0 += h_use_count[r][c];
            count_1 += h_use_count[r][c];
          }
        }

        double bonus_0 = bonus_function(count_0);
        double bonus_1 = bonus_function(count_1);
        adjusted_ws[h0_base_index[r]] -= bonus_0;
        adjusted_ws[h1_base_index[r]] -= bonus_1;

        bonus_sum += bonus_0;
        bonus_sum += bonus_1;
      }

      for (int c = 0; c < N; c++) {
        int count_0 = 0;
        int count_1 = 0;
        if (state != M1) {
          for (int r = 0; r < N - 1; r++) {
            if (r < N / 3) {
              count_0 += v_use_count[r][c];
            } else if (r >= (N - 1) - (N / 3)) {
              count_1 += v_use_count[r][c];
            }
          }
        } else {
          for (int r = 0; r < N - 1; r++) {
            count_0 += v_use_count[r][c];
            count_1 += v_use_count[r][c];
          }
        }

        double bonus_0 = bonus_function(count_0);
        double bonus_1 = bonus_function(count_1);
        adjusted_ws[v0_base_index[c]] -= bonus_0;
        adjusted_ws[v1_base_index[c]] -= bonus_1;

        bonus_sum += bonus_0;
        bonus_sum += bonus_1;
      }

      double bonus_average = bonus_sum / (N * 4.0);
      // if (q_index % 10 == 0) {
      //   cerr << "q_index: " << q_index << " bonus_average: " << bonus_average << endl;
      // }
      for (int i = 0; i < N; i++) {
        adjusted_ws[h0_base_index[i]] += bonus_average;
        adjusted_ws[h1_base_index[i]] += bonus_average;
        adjusted_ws[v0_base_index[i]] += bonus_average;
        adjusted_ws[v1_base_index[i]] += bonus_average;
      }
    }

    vector<vector<double>> h_table = Make2D(N, N - 1, 0.0);
    vector<vector<double>> v_table = Make2D(N - 1, N, 0.0);

    for (int r = 0; r < N; r++) {
      for (int c = 0; c < N - 1; c++) {
        double weight = edge_weight_base;
        for (const auto& feature_iv : h_feature_weights[r][c]) {
          weight += adjusted_ws[feature_iv.index] * feature_iv.value;
        }
        h_table[r][c] = weight;
      }
    }
    for (int r = 0; r < N - 1; r++) {
      for (int c = 0; c < N; c++) {
        double weight = edge_weight_base;
        for (const auto& feature_iv : v_feature_weights[r][c]) {
          weight += adjusted_ws[feature_iv.index] * feature_iv.value;
        }
        v_table[r][c] = weight;
      }
    }

    return {h_table, v_table};
  }

  SparseVectorD PathToItermVector(const vector<int>& path) {
    vector<pair<int, int>> h_edges;
    vector<pair<int, int>> v_edges;

    for (int i = 0; i + 1 < int(path.size()); i++) {
      const auto [r0, c0] = DecodeVertexId(path[i]);
      const auto [r1, c1] = DecodeVertexId(path[i + 1]);

      if (r1 - r0 == 1) {
        v_edges.push_back({r0, c0});
      } else if (r1 - r0 == -1) {
        v_edges.push_back({r0 - 1, c0});
      } else if (c1 - c0 == 1) {
        h_edges.push_back({r0, c0});
      } else if (c1 - c0 == -1) {
        h_edges.push_back({r0, c0 - 1});
      } else {
        exit(1);
      }
    }

    vector<double> feature_weights(feature_count);
    for (const auto [r, c] : h_edges) {
      for (const IVD feature_iv : h_feature_weights[r][c]) {
        feature_weights[feature_iv.index] += feature_iv.value;
      }
    }
    for (const auto [r, c] : v_edges) {
      for (const IVD feature_iv : v_feature_weights[r][c]) {
        feature_weights[feature_iv.index] += feature_iv.value;
      }
    }

    SparseVectorD item_vector;
    for (int k = 0; k < feature_count; k++) {
      if (feature_weights[k] > 0) {
        item_vector.push_back({k, feature_weights[k]});
      }
    }
    return item_vector;
  }

  void Update01Borders() {
    for (int r = 0; r < N; r++) {
      bool increase = false;
      bool decrease = false;
      int b = h0_bs[r];
      if (ws[h0_base_index[r]] < ws[h1_base_index[r]]) {
        if (b % 2 == 0) {
          int m = b / 2;
          if (ws[h_delta_index[r][m]] > 0) {
            decrease = true;
          }
          if (m - 1 >= 0 &&
              ws[h_delta_index[r][m]] + ws[h_delta_index[r][m - 1]] > 0) {
            decrease = true;
          }

          if (ws[h_delta_index[r][m]] < 0) {
            increase = true;
          }
          if (m + 1 < N - 1 &&
              ws[h_delta_index[r][m]] + ws[h_delta_index[r][m + 1]] < 0) {
            increase = true;
          }
        } else {
          int l = b / 2;
          int u = b / 2 + 1;
          if (ws[h_delta_index[r][l]] > 0) {
            decrease = true;
          }
          if (l - 1 >= 0 &&
              ws[h_delta_index[r][l]] + ws[h_delta_index[r][l - 1]] > 0) {
            decrease = true;
          }

          if (ws[h_delta_index[r][u]] < 0) {
            increase = true;
          }
          if (u + 1 < N - 1 &&
              ws[h_delta_index[r][u]] + ws[h_delta_index[r][u + 1]] < 0) {
            increase = true;
          }
        }
      } else if (ws[h0_base_index[r]] > ws[h1_base_index[r]]) {
        int b = h0_bs[r];
        if (b % 2 == 0) {
          int m = b / 2;
          if (ws[h_delta_index[r][m]] < 0) {
            decrease = true;
          }
          if (m - 1 >= 0 &&
              ws[h_delta_index[r][m]] + ws[h_delta_index[r][m - 1]] < 0) {
            decrease = true;
          }

          if (ws[h_delta_index[r][m]] > 0) {
            increase = true;
          }
          if (m + 1 < N - 1 &&
              ws[h_delta_index[r][m]] + ws[h_delta_index[r][m + 1]] > 0) {
            increase = true;
          }
        } else {
          int l = b / 2;
          int u = b / 2 + 1;
          if (ws[h_delta_index[r][l]] < 0) {
            decrease = true;
          }
          if (l - 1 >= 0 &&
              ws[h_delta_index[r][l]] + ws[h_delta_index[r][l - 1]] < 0) {
            decrease = true;
          }

          if (ws[h_delta_index[r][u]] > 0) {
            increase = true;
          }
          if (u + 1 < N - 1 &&
              ws[h_delta_index[r][u]] + ws[h_delta_index[r][u + 1]] > 0) {
            increase = true;
          }
        }
      }

      if (b == 1) decrease = false;
      if (b == 2 * (N - 1 - 1) - 1) increase = false;

      if (increase && decrease) {
        double random = uniform_real_distribution<double>(0, 1)(random_engine);
        if (random < 0.5) {
          h0_bs[r]++;
        } else {
          h0_bs[r]--;
        }
      } else if (increase) {
        h0_bs[r]++;
      } else if (decrease) {
        h0_bs[r]--;
      }
    }

    for (int c = 0; c < N; c++) {
      bool increase = false;
      bool decrease = false;
      int b = v0_bs[c];
      if (ws[v0_base_index[c]] < ws[v1_base_index[c]]) {
        if (b % 2 == 0) {
          int m = b / 2;
          if (ws[v_delta_index[m][c]] > 0) {
            decrease = true;
          }
          if (m - 1 >= 0 &&
              ws[v_delta_index[m][c]] + ws[h_delta_index[m - 1][c]] > 0) {
            decrease = true;
          }
          
          if (ws[v_delta_index[m][c]] < 0) {
            increase = true;
          }
          if (m + 1 < N - 1 &&
              ws[v_delta_index[m][c]] + ws[h_delta_index[m + 1][c]] < 0) {
            increase = true;
          }
        } else {
          int l = b / 2;
          int u = b / 2 + 1;
          if (ws[v_delta_index[l][c]] > 0) {
            decrease = true;
          }
          if (l - 1 >= 0 &&
              ws[v_delta_index[l][c]] + ws[h_delta_index[l - 1][c]] > 0) {
            decrease = true;
          }

          if (ws[v_delta_index[u][c]] < 0) {
            increase = true;
          }
          if (u + 1 < N - 1 &&
              ws[v_delta_index[u][c]] + ws[h_delta_index[u + 1][c]] < 0) {
            increase = true;
          }
        }
      } else if (ws[v0_base_index[c]] > ws[v1_base_index[c]]) {
        if (b % 2 == 0) {
          int m = b / 2;
          if (ws[v_delta_index[m][c]] < 0) {
            decrease = true;
          }
          if (m - 1 >= 0 &&
              ws[v_delta_index[m][c]] + ws[h_delta_index[m - 1][c]] < 0) {
            decrease = true;
          }

          if (ws[v_delta_index[m][c]] > 0) {
            increase = true;
          }
          if (m + 1 < N - 1 &&
              ws[v_delta_index[m][c]] + ws[h_delta_index[m + 1][c]] > 0) {
            increase = true;
          }
        } else {
          int l = b / 2;
          int u = b / 2 + 1;
          if (ws[v_delta_index[l][c]] < 0) {
            decrease = true;
          }
          if (l - 1 >= 0 &&
              ws[v_delta_index[l][c]] + ws[h_delta_index[l - 1][c]] < 0) {
            decrease = true;
          }

          if (ws[v_delta_index[u][c]] > 0) {
            increase = true;
          }
          if (u + 1 < N - 1 &&
              ws[v_delta_index[u][c]] + ws[h_delta_index[u + 1][c]] > 0) {
            increase = true;
          }
        }
      }

      if (b == 1) decrease = false;
      if (b == 2 * (N - 1 - 1) - 1) increase = false;

      if (increase && decrease) {
        double random = uniform_real_distribution<double>(0, 1)(random_engine);
        if (random < 0.5) {
          v0_bs[c]++;
        } else {
          v0_bs[c]--;
        }
      } else if (increase) {
        v0_bs[c]++;
      } else if (decrease) {
        v0_bs[c]--;
      }
    }
  }

  void ProcessQuery(int q_index) {
    bool require_reset = false;
    bool mode_changed = false;
    if (q_index == 0) {
      require_reset = true;
    } else if (
        state == INITIAL &&
        (q_index == 100 || q_index == 200 || q_index == 300)) {
      SetMode(q_index);
      require_reset = true;
      if (state != INITIAL) {
        mode_changed = true;
      }
    }

    if (state == M2 &&
        ((q_index % 5 == 0 && q_index < 500) || q_index % 10 == 0)) {
      Update01Borders();
      require_reset = true;
    }

    if (require_reset) {
      SetEdgeFeatureWeights();
      items.clear();
      for (int i = 0; i < q_index; i++) {
        items.push_back(PathToItermVector(paths[i]));
      }
      labels.clear();
      for (int i = 0; i < q_index; i++) {
        labels.push_back(results[i] - edge_weight_base * (paths[i].size() - 1));
      }
    }

    if (q_index > 0) {
      int iter_count = 5;
      if (require_reset) {
        iter_count = 10;
      }
      if (mode_changed) {
        iter_count = 100;
      }
      if (state == M2 && q_index >= 500 && !require_reset) {
        iter_count = 2;
      }
      ws = LinearRegression(
          feature_count,
          items.size(),
          items,
          labels,
          item_weights,
          ridge_weights,
          ws,
          iter_count);
    }

    // DEBUG for V22.
    // if (q_index >= 100 && q_index <= 300 && q_index % 50 == 0) {
    //   cerr << "q_index: " << q_index << endl;
    //   for (int r = 0; r < N; r++) {
    //     cerr << "[" << r << "] = ";
    //     cerr << "(" << int(ws[h0_base_index[r]] / 100) << ", "
    //          << int(ws[h1_base_index[r]] / 100) << "), ";
    //   }
    //   cerr << endl;
    //   for (int c = 0; c < N; c++) {
    //     cerr << "[" << c << "] = ";
    //     cerr << "(" << int(ws[v0_base_index[c]] / 100) << ", "
    //          << int(ws[v1_base_index[c]] / 100) << ") ";
    //   }
    //   cerr << endl;

    //   cerr << endl;

    //   int a_sum = 0;
    //   int b_sum = 0;
    //   for (int r = 0; r < N; r++) {
    //     int a = 0;
    //     int b = 0;
    //     for (int c = 0; c < N - 1; c++) {
    //       if (c < N / 3) {
    //         a += h_use_count[r][c];
    //       } else if (c >= (N - 1) - (N / 3)) {
    //         b += h_use_count[r][c];
    //       }
    //     }
    //     cerr << "[" << r << "] = (" << a << ", " << b << ") ";
    //     a_sum += a;
    //     b_sum += b;
    //   }
    //   cerr << endl;

    //   cerr << a_sum << " " << b_sum << endl;
    //   cerr << endl;
    // }

    int sr, sc, tr, tc; cin >> sr >> sc >> tr >> tc;

    vector<vector<double>> h_table, v_table;
    if (q_index < 0) {
      tie(h_table, v_table) = MakeRandomEdgeWeights();
    } else {
      tie(h_table, v_table) = MakeEdgeWeights(q_index);
    }
    vector<vector<WEdgeD>> graph = MakeGraph(h_table, v_table);

    int s = VertexId(sr, sc);
    int t = VertexId(tr, tc);
    vector<double> ds; vector<int> ps; tie(ds, ps) = ShortestPath(graph, s);

    auto extract_path = [](int s, int t, const vector<int>& ps) -> vector<int> {
      vector<int> vs;
      int v = t;
      while (v != s) {
        vs.push_back(v);
        v = ps[v];
      }
      vs.push_back(v);
      reverse(vs.begin(), vs.end());
      return vs;
    };
    vector<int> path = extract_path(s, t, ps);
    paths.push_back(path);

    auto process_path = [](const vector<int>& path) -> 
        tuple<string, vector<pair<int, int>>, vector<pair<int, int>>> {
      string output;
      vector<pair<int, int>> h_edges;
      vector<pair<int, int>> v_edges;
      for (int i = 0; i + 1 < int(path.size()); i++) {
        const auto [r0, c0] = DecodeVertexId(path[i]);
        const auto [r1, c1] = DecodeVertexId(path[i + 1]);

        if (r1 - r0 == 1) {
          output.push_back('D');
          v_edges.push_back({r0, c0});
        } else if (r1 - r0 == -1) {
          output.push_back('U');
          v_edges.push_back({r0 - 1, c0});
        } else if (c1 - c0 == 1) {
          output.push_back('R');
          h_edges.push_back({r0, c0});
        } else if (c1 - c0 == -1) {
          output.push_back('L');
          h_edges.push_back({r0, c0 - 1});
        } else {
          exit(1);
        }
      }

      return {output, h_edges, v_edges};
    };
    string output;
    vector<pair<int, int>> h_edges;
    vector<pair<int, int>> v_edges;
    tie(output, h_edges, v_edges) = process_path(path);

    cout << output << endl;
    cout.flush();

    for (const auto [r, c] : h_edges) {
      h_use_count[r][c]++;
    }
    for (const auto [r, c] : v_edges) {
      v_use_count[r][c]++;
    }
  
    double result; cin >> result;

    results.push_back(result);
    labels.push_back(result - edge_weight_base * output.size());
    item_weights.push_back(1.0 / Square(0.1 * result));

    items.push_back(PathToItermVector(path));
  }

  // Set M1 or M2.
  void SetMode(int q_index) {
    double gap_01_squared_sum = 0;
    for (int r = 0; r < N; r++) {
      gap_01_squared_sum +=
          Square(ws[h0_base_index[r]] - ws[h1_base_index[r]]);
    }
    for (int c = 0; c < N; c++) {
      gap_01_squared_sum +=
          Square(ws[v0_base_index[c]] - ws[v1_base_index[c]]);
    }
    double gap_01_sd = sqrt(gap_01_squared_sum / (2 * N));

    cerr << "gap_01_sd: " << int(gap_01_sd) << endl;
    if (q_index < 300) {
      if (gap_01_sd < 1000) {
        state = M1;
        cerr << "state: M1" << endl;
      } else if (gap_01_sd >= 1400) {
        // state = M2;
        // cerr << "state: M2" << endl;
      }
    } else {
      if (gap_01_sd < 1200) {
        state = M1;
        cerr << "state: M1" << endl;
      } else {
        state = M2;
        cerr << "state: M2" << endl;
      }
    }
  }

  void RunInternal() {
    SetUpFeatureIndexes();

    SetUpLinearRegressionVariables();

    SetUpRidgeWeights();

    state = INITIAL;

    h0_bs = vector<int>(N, N - 1);
    v0_bs = vector<int>(N, N - 1);

    h_use_count = Make2D(N, N - 1, 0);
    v_use_count = Make2D(N - 1, N, 0);

    for (int q_index = 0; q_index < Q_COUNT; q_index++) {
      ProcessQuery(q_index);
    }

    cerr << "h0_bs: " << h0_bs << endl;
    cerr << "v0_bs: " << v0_bs << endl;
    if (DEBUG) {
      ofstream debug_output;
      debug_output.open("debug_output.txt");

      vector<vector<double>> h_table, v_table;
      tie(h_table, v_table) = MakeEdgeWeights(Q_COUNT);

      for (int r = 0; r < N; r++) {
        for (int c = 0; c < N - 1; c++) {
          debug_output << int(h_table[r][c]) << " ";
        }
        debug_output << endl;
      }

      for (int r = 0; r < N - 1; r++) {
        for (int c = 0; c < N; c++) {
          debug_output << int(v_table[r][c]) << " ";
        }
        debug_output << endl;
      }
    }
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);

  Main main;
  main.Run();
}
