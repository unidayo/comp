#include <bits/stdc++.h>
constexpr int DEBUG = 0;
using namespace std;
using int64 = long long;

constexpr int N = 50;
constexpr int M = 100;

template<typename T>
vector<vector<T>> Make2D(int d1, int d2, T default_value) {
  return vector<vector<T>>(d1, vector<T>(d2, default_value));
}

class DSU {
 private:
  vector<int> rs, ps, ss;
 public:
  DSU(int n) : rs(n), ps(n), ss(n, 1) {
    for (int i = 0; i < n; i++) { ps[i] = i; }
  }
  int Find(int x) { if (x != ps[x]) { ps[x] = Find(ps[x]); } return ps[x]; }
  void Union(int x, int y) {
    int xr = Find(x); int yr = Find(y);
    if (rs[xr] > rs[yr]) { ps[yr] = ps[xr]; ss[xr] += ss[yr];
    } else if (rs[xr] < rs[yr]) { ps[xr] = ps[yr]; ss[yr] += ss[xr];
    } else if (ps[xr] != ps[yr]) { ps[yr] = ps[xr]; rs[xr]++; ss[xr] += ss[yr];
    }
  }
  int Size(int x) { return ss[Find(x)]; }
};

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

 private:
  chrono::time_point<chrono::steady_clock> start_;
};

bool RemoveRow(
    int r0,
    const vector<vector<int>>& input_map,
    int row_count,
    int col_count,
    vector<vector<int>>& output_map) {
  bool row_is_empty = true;
  for (int c = 0; c < col_count + 2; c++) {
    if (input_map[r0][c] > 0) {
      row_is_empty = false;
    }
  }
  if (row_is_empty) return false;

  for (int r = 0; r < row_count + 2; r++) {
    for (int c = 0; c < col_count + 2; c++) {
      output_map[r][c] = 0;
    }
  }
  for (int r = 0; r < row_count + 2; r++) {
    for (int c = 0; c < col_count + 2; c++) {
      if (r < r0) {
        output_map[r][c] = input_map[r][c];
      } else if (r > r0) {
        output_map[r - 1][c] = input_map[r][c];
      }
    }
  }
  return true;
}

bool RemoveCol(
    int c0,
    const vector<vector<int>>& input_map,
    int row_count,
    int col_count,
    vector<vector<int>>& output_map) {
  bool col_is_empty = true;
  for (int r = 0; r < row_count + 2; r++) {
    if (input_map[r][c0] > 0) {
      col_is_empty = false;
    }
  }
  if (col_is_empty) return false;

  for (int r = 0; r < row_count + 2; r++) {
    for (int c = 0; c < col_count + 2; c++) {
      output_map[r][c] = 0;
    }
  }
  for (int r = 0; r < row_count + 2; r++) {
    for (int c = 0; c < col_count + 2; c++) {
      if (c < c0) {
        output_map[r][c] = input_map[r][c];
      } else if (c > c0) {
        output_map[r][c - 1] = input_map[r][c];
      }
    }
  }
  return true;
}

void ComputeAdjacentMatrix(
    const vector<vector<int>>& input_map,
    int row_count,
    int col_count,
    vector<bitset<M + 1>>& output_adjacent_matrix) {
  for (int i = 0; i <= M; i++) {
    output_adjacent_matrix[i].reset();
  }

  for (int r = 0; r <= row_count + 1; r++) {
    for (int c = 0; c <= col_count + 1; c++) {
      int color_0 = input_map[r][c];

      if (r + 1 <= row_count + 1) {
        int color_1 = input_map[r + 1][c];
        if (color_0 != color_1) {
          output_adjacent_matrix[color_0].set(color_1);
          output_adjacent_matrix[color_1].set(color_0);
        }
      }

      if (c + 1 <= col_count + 1) {
        int color_1 = input_map[r][c + 1];
        if (color_0 != color_1) {
          output_adjacent_matrix[color_0].set(color_1);
          output_adjacent_matrix[color_1].set(color_0);
        }
      }
    }
  }
}

bool IsAllConnected(
    const vector<vector<int>>& in_map, int row_count, int col_count) {
  vector<int> color_exist(M + 1);
  DSU dsu((row_count + 2) * (col_count + 2));
  for (int r = 0; r <= row_count + 1; r++) {
    for (int c = 0; c <= col_count + 1; c++) {
      int color_0 = in_map[r][c];
      color_exist[color_0] = 1;

      if (r + 1 <= row_count + 1) {
        int color_1 = in_map[r + 1][c];
        if (color_0 == color_1) {
          dsu.Union(r * (col_count + 2) + c, (r + 1) * (col_count + 2) + c);
        }
      }

      if (c + 1 <= col_count + 1) {
        int color_1 = in_map[r][c + 1];
        if (color_0 == color_1) {
          dsu.Union(r * (col_count + 2) + c, r * (col_count + 2) + c + 1);
        }
      }
    }
  }

  for (int i = 0; i <= M; i++) {
    if (color_exist[i] == 0) {
      return false;
    }
  }

  int root_count = 0;
  for (int i = 0; i < (row_count + 2) * (col_count + 2); i++) {
    if (dsu.Find(i) == i) {
      root_count++;
    }
  }
  return root_count == M + 1;
}

bool IsEqualMatrix(
    const vector<bitset<M + 1>> matrix_0,
    const vector<bitset<M + 1>> matrix_1,
    bool is_debug=false) {
  for (int i = 0; i < int(matrix_0.size()); i++) {
    if (matrix_0[i] != matrix_1[i]) {
      return false;
    }
  }
  return true;
}

void CopyRangedMatrix(
    const vector<vector<int>>& in_matrix, int max_r, int max_c,
    vector<vector<int>>& out_matrix) {
  for (int r = 0; r < max_r; r++) {
    for (int c = 0; c < max_c; c++) {
      out_matrix[r][c] = in_matrix[r][c];
    }
  }
}

class Main {
 private:
  Random random_;
  TimeMeasurement time_;

  vector<vector<int>> original_map_;
  vector<bitset<M + 1>> original_adjacent_matrix_;

  vector<vector<int>> current_map_;
  int current_row_count_ = N;
  int current_col_count_ = N;

  vector<vector<int>> candidate_map_;
  vector<bitset<M + 1>> candidate_adjacent_matrix_;

  void UpdateCurrentMapSize() {
    int r_max = current_row_count_;
    int c_max = current_col_count_;
    current_row_count_ = 1;
    current_col_count_ = 1;
    for (int r = 1; r <= r_max; r++) {
      for (int c = 1; c <= c_max; c++) {
        if (current_map_[r][c] > 0) {
          current_row_count_ = max(r, current_row_count_);
          current_col_count_ = max(c, current_col_count_);
        }
      }
    }
  }

  bool IsCandidateMapValid() {
    if (!IsAllConnected(
            candidate_map_, current_row_count_, current_col_count_)) {
      return false;
    }

    ComputeAdjacentMatrix(
        candidate_map_, current_row_count_, current_col_count_,
        candidate_adjacent_matrix_);
    return IsEqualMatrix(original_adjacent_matrix_, candidate_adjacent_matrix_);
  }

  void RunGreedyDeleteLoop() {
    while (true) {
      bool updated = false;

      for (int r0 = 1; r0 <= N; r0++) {
        bool remove_row_success =
            RemoveRow(r0, current_map_, current_row_count_, current_col_count_, candidate_map_);
        if (!remove_row_success) continue;
        if (IsCandidateMapValid()) {
          current_map_ = candidate_map_;
          UpdateCurrentMapSize();
          updated = true;
          break;
        }
      }
      if (updated) continue;

      for (int c0 = 1; c0 <= N; c0++) {
        bool remove_col_success = RemoveCol(c0, current_map_, current_row_count_, current_col_count_, candidate_map_);
        if (!remove_col_success) continue;
        if (IsCandidateMapValid()) {
          current_map_ = candidate_map_;
          UpdateCurrentMapSize();
          updated = true;
          break;
        }
      }
      if (updated) continue;

      break;
    }
  }

  // int copy_success = 0;
  bool RunCopyAndDeleteStep() {
    int axis = random_.RandomInt(0, 1, true);
    int direction = random_.RandomInt(0, 1, true);
    if (axis == 0) {
      int r0 = random_.RandomInt(1, current_row_count_ - 1, true);
      int r1 = r0 + 1;
      CopyRangedMatrix(
          current_map_,
          current_row_count_ + 2,
          current_col_count_ + 2,
          candidate_map_);
      // candidate_map_ = current_map_;
      int c0 = random_.RandomInt(1, current_col_count_, true);
      int c1 = random_.RandomInt(1, current_col_count_, true);
      if (c0 > c1) swap(c0, c1);
      for (int c = c0; c <= c1; c++) {
        if (direction == 0) {
          candidate_map_[r1][c] = candidate_map_[r0][c];
        } else {
          candidate_map_[r0][c] = candidate_map_[r1][c];
        }
      }

      if (IsCandidateMapValid()) {
        current_map_ = candidate_map_;
        UpdateCurrentMapSize();

        if (random_.RandomInt(0, 1, true) == 1) {
          swap(r0, r1);
        }

        RemoveRow(r0, current_map_, current_row_count_, current_col_count_, candidate_map_);
        if (IsCandidateMapValid()) {
          current_map_ = candidate_map_;
          UpdateCurrentMapSize();
          return true;
        }

        RemoveRow(r1, current_map_, current_row_count_, current_col_count_, candidate_map_);
        if (IsCandidateMapValid()) {
          current_map_ = candidate_map_;
          UpdateCurrentMapSize();
          return true;
        }

        return true;
      } else {
        return false;
      }
    } else {
      int c0 = random_.RandomInt(1, current_col_count_ - 1, true);
      int c1 = c0 + 1;
      CopyRangedMatrix(
          current_map_,
          current_row_count_ + 2,
          current_col_count_ + 2,
          candidate_map_);
      // candidate_map_ = current_map_;
      int r0 = random_.RandomInt(1, current_row_count_, true);
      int r1 = random_.RandomInt(1, current_row_count_, true);
      if (r0 > r1) swap(r0, r1);
      for (int r = r0; r <= r1; r++) {
        if (direction == 0) {
          candidate_map_[r][c1] = candidate_map_[r][c0];
        } else {
          candidate_map_[r][c0] = candidate_map_[r][c1];
        }
      }

      if (IsCandidateMapValid()) {
        current_map_ = candidate_map_;
        UpdateCurrentMapSize();

        if (random_.RandomInt(0, 1, true) == 1) {
          swap(c0, c1);
        }

        RemoveCol(c0, current_map_, current_row_count_, current_col_count_, candidate_map_);
        if (IsCandidateMapValid()) {
          current_map_ = candidate_map_;
          UpdateCurrentMapSize();
          return true;
        }

        RemoveRow(c1, current_map_, current_row_count_, current_col_count_, candidate_map_);
        if (IsCandidateMapValid()) {
          current_map_ = candidate_map_;
          UpdateCurrentMapSize();
          return true;
        }

        return true;
      } else {
        return false;
      }
    }
  }

  void DeleteCellStep() {
    int r0 = random_.RandomInt(1, current_row_count_);
    int c0 = random_.RandomInt(1, current_col_count_);
    if (current_map_[r0][c0] == 0) return;
    candidate_map_ = current_map_;
    int axis = random_.RandomInt(0, 1, true);
    int direction = random_.RandomInt(0, 1, true);
    if (axis == 0) {
      if (direction == 0) {
        for (int r = 1; r <= r0; r++) {
          candidate_map_[r][c0] = current_map_[r - 1][c0];
        }
      } else {
        for (int r = r0; r <= current_row_count_; r++) {
          candidate_map_[r][c0] = current_map_[r + 1][c0];
        }
      }

      if (IsCandidateMapValid()) {
        current_map_ = candidate_map_;
        UpdateCurrentMapSize();
      }
    } else {
      if (direction == 0) {
        for (int c = 1; c <= c0; c++) {
          candidate_map_[r0][c] = current_map_[r0][c - 1];
        }
      } else {
        for (int c = c0; c <= current_col_count_; c++) {
          candidate_map_[r0][c] = current_map_[r0][c + 1];
        }
      }

      if (IsCandidateMapValid()) {
        current_map_ = candidate_map_;
        UpdateCurrentMapSize();
      }
    }
  }

 public:
  Main() {}

  void Run() {
    int unused_n, unused_m; cin >> unused_n >> unused_m;
    original_map_ = Make2D(N + 2, N + 2, 0);
    for (int r = 1; r <= N; r++) {
      for (int c = 1; c <= N; c++) {
        cin >> original_map_[r][c];
      }
    }

    original_adjacent_matrix_ = vector<bitset<M + 1>>(M + 1);
    ComputeAdjacentMatrix(original_map_, N, N, original_adjacent_matrix_);

    current_map_ = original_map_;
    UpdateCurrentMapSize();

    candidate_map_ = original_map_;
    candidate_adjacent_matrix_ = original_adjacent_matrix_;

    RunGreedyDeleteLoop();

    int run_copy_and_delete_step_count = 0;
    int update_success_count = 0;
    while (time_.CurrentMs() < 1710) {
      if (RunCopyAndDeleteStep()) {
        update_success_count++;
      }
      run_copy_and_delete_step_count++;
    }
    cerr << "run_copy_and_delete_step_count: "
         << run_copy_and_delete_step_count << endl;
    cerr << "update_success_count: " << update_success_count << endl;

    cerr << "current_row_count_: " << current_row_count_ << endl;
    cerr << "current_col_count_: " << current_col_count_ << endl;

    int delete_cell_step_count = 0;
    while (time_.CurrentMs() < 1900) {
      // RunCopyAndDeleteStep();
      DeleteCellStep();
      delete_cell_step_count++;
    }
    cerr << "delete_cell_step_count: " << delete_cell_step_count << endl;

    for (int r = 1; r <= N; r++) {
      for (int c = 1; c <= N; c++) {
        cout << current_map_[r][c] << " ";
      }
      cout << endl;
    }
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);

  Main main;
  main.Run();
}
