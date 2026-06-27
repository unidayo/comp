#include <bits/stdc++.h>
constexpr int DEBUG = 0;
using namespace std;
using int64 = long long;

void ReportUnexpectedAndDie(int line) {
  cerr << "Unexpected@" << line << "!" << endl;
  exit(1);
}

#define UNEXPECTED() ReportUnexpectedAndDie(__LINE__)

template<typename T>
vector<vector<T>> Make2D(int d1, int d2, T default_value) {
  return vector<vector<T>>(d1, vector<T>(d2, default_value));
}

constexpr int kBoardSize = 10;
constexpr int kCandyCount = 100;
constexpr int kCandyTypeMax = 3;

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

class RandomGenerator {
 public:
  RandomGenerator() :
      random_engine_(
          std::chrono::system_clock::now().time_since_epoch().count()) {}

  int RandomInt(int a, int b, bool inclusive=false) {
    if (inclusive) b++;
    return uniform_int_distribution<int>(a, b - 1)(random_engine_);
  }

  double RandomDouble(double a, double b) {
    return uniform_real_distribution<double>(a, b)(random_engine_);
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
  const chrono::time_point<chrono::steady_clock> start_;
};

int ComputeScore(const vector<vector<int>>& board) {
  vector<int> type_count(kCandyTypeMax + 1);
  for (int y = 0; y < kBoardSize; y++) {
    for (int x = 0; x < kBoardSize; x++) {
      if (board[y][x] > 0) {
        type_count[board[y][x]]++;
      }
    }
  }

  DSU dsu(kBoardSize * kBoardSize);
  for (int y = 0; y < kBoardSize; y++) {
    for (int x = 0; x < kBoardSize; x++) {
      if (x + 1 < kBoardSize) {
        if (board[y][x] > 0 && board[y][x] == board[y][x + 1]) {
          dsu.Union(y * kBoardSize + x, y * kBoardSize + x + 1);
        }
      }
      if (y + 1 < kBoardSize) {
        if (board[y][x] > 0 && board[y][x] == board[y + 1][x]) {
          dsu.Union(y * kBoardSize + x, (y + 1) * kBoardSize + x);
        }
      }
    }
  }

  int num = 0;
  int den = 0;
  for (int i = 0; i < kBoardSize * kBoardSize; i++) {
    if (dsu.Find(i) == i) {
      num += dsu.Size(i) * dsu.Size(i);
    }
  }
  for (int type = 1; type <= kCandyTypeMax; type++) {
    den += type_count[type] * type_count[type];
  }
  return round(1.0E6 * double(num) / double(den));
}

struct Strategy {
  int type_1;
  int type_2;
  int type_3;
  char direction_1;
  char direction_2;
  char direction_3;
};

// array<Strategy, 24> strategy_list = {
//   Strategy({1, 2, 3, 'F', 'L', 'R'}),
//   Strategy({1, 3, 2, 'F', 'L', 'R'}),
//   Strategy({2, 1, 3, 'F', 'L', 'R'}),
//   Strategy({2, 3, 1, 'F', 'L', 'R'}),
//   Strategy({3, 1, 2, 'F', 'L', 'R'}),
//   Strategy({3, 2, 1, 'F', 'L', 'R'}),

//   Strategy({1, 2, 3, 'B', 'L', 'R'}),
//   Strategy({1, 3, 2, 'B', 'L', 'R'}),
//   Strategy({2, 1, 3, 'B', 'L', 'R'}),
//   Strategy({2, 3, 1, 'B', 'L', 'R'}),
//   Strategy({3, 1, 2, 'B', 'L', 'R'}),
//   Strategy({3, 2, 1, 'B', 'L', 'R'}),

//   Strategy({1, 2, 3, 'L', 'F', 'B'}),
//   Strategy({1, 3, 2, 'L', 'F', 'B'}),
//   Strategy({2, 1, 3, 'L', 'F', 'B'}),
//   Strategy({2, 3, 1, 'L', 'F', 'B'}),
//   Strategy({3, 1, 2, 'L', 'F', 'B'}),
//   Strategy({3, 2, 1, 'L', 'F', 'B'}),

//   Strategy({1, 2, 3, 'R', 'F', 'B'}),
//   Strategy({1, 3, 2, 'R', 'F', 'B'}),
//   Strategy({2, 1, 3, 'R', 'F', 'B'}),
//   Strategy({2, 3, 1, 'R', 'F', 'B'}),
//   Strategy({3, 1, 2, 'R', 'F', 'B'}),
//   Strategy({3, 2, 1, 'R', 'F', 'B'})
// };

array<Strategy, 1> strategy_list = {
  Strategy({1, 2, 3, 'F', 'L', 'R'}),
  // Strategy({1, 3, 2, 'F', 'L', 'R'}),
  // Strategy({2, 1, 3, 'F', 'L', 'R'}),
  // Strategy({2, 3, 1, 'F', 'L', 'R'}),
  // Strategy({3, 1, 2, 'F', 'L', 'R'}),
  // Strategy({3, 2, 1, 'F', 'L', 'R'}),
};

char OppositeDirection(char d) {
  if (d == 'F') return 'B';
  if (d == 'B') return 'F';
  if (d == 'L') return 'R';
  if (d == 'R') return 'L';
  UNEXPECTED(); exit(1);
}

void PutCandy(
    int empty_position_id, int candy_type, vector<vector<int>>& board) {
  int empty_count = 0;
  for (int y = 0; y < kBoardSize; y++) {
    for (int x = 0; x < kBoardSize; x++) {
      if (board[y][x] == 0) {
        empty_count++;
        if (empty_count == empty_position_id) {
          board[y][x] = candy_type;
          return;
        }
      }
    }
  }

  UNEXPECTED();
}

void ExecuteAction(char direction, vector<vector<int>>& board) {
  static vector<int> types;
  if (direction == 'F' || direction == 'B') {
    for (int x = 0; x < kBoardSize; x++) {
      types.clear();

      for (int y = 0; y < kBoardSize; y++) {
        if (board[y][x] > 0) {
          types.push_back(board[y][x]);
          board[y][x] = 0;
        }
      }

      for (int i = 0; i < int(types.size()); i++) {
        if (direction == 'F') {
          board[i][x] = types[i];
        } else if (direction == 'B') {
          board[kBoardSize - 1 - i][x] = types[int(types.size()) - 1 - i];
        } else {
          UNEXPECTED();
        }
      }
    }
    return;
  } else if (direction == 'L' || direction == 'R') {
    for (int y = 0; y < kBoardSize; y++) {
      types.clear();

      for (int x = 0; x < kBoardSize; x++) {
        if (board[y][x] > 0) {
          types.push_back(board[y][x]);
          board[y][x] = 0;
        }
      }

      for (int i = 0; i < int(types.size()); i++) {
        if (direction == 'L') {
          board[y][i] = types[i];
        } else if (direction == 'R') {
          board[y][kBoardSize - 1 - i] = types[int(types.size()) - 1 - i];
        } else {
          UNEXPECTED();
        }
      }
    }
    return;
  }

  cerr << "direction: " << int(direction) << endl;
  UNEXPECTED();
}

char ChooseActionGreedy(
    const int next_type, const char last_fb, const char last_lr,
    const Strategy strategy) {
  if (next_type == strategy.type_1) {
    return OppositeDirection(strategy.direction_1);
  } else {
    if (last_fb == strategy.direction_1 || last_lr == strategy.direction_1) {
      if (next_type == strategy.type_2) {
        return OppositeDirection(strategy.direction_2);
      } else if (next_type == strategy.type_3) {
        return OppositeDirection(strategy.direction_3);
      } else {
        cerr << "next_type: " << next_type << endl;
        UNEXPECTED(); exit(1);
      }
    } else {
      return strategy.direction_1;
    }
  }
}

int RunGreedy(
    const vector<vector<int>>& initial_board,
    const char initial_last_fb,
    const char initial_last_lr,
    const Strategy strategy,
    const vector<int>& input_types,
    const vector<int>& input_positions) {
  vector<vector<int>> board = initial_board;
  char last_fb = initial_last_fb;
  char last_lr = initial_last_lr;
  // auto choose_action = [&](int index) -> char {
  //   if (index == int(input_types.size()) - 1) {
  //     return 'F';
  //   }

  //   if (input_types[index + 1] == strategy.type_1) {
  //     return OppositeDirection(strategy.direction_1);
  //   } else {
  //     if (last_fb == strategy.direction_1 || last_lr == strategy.direction_1) {
  //       if (input_types[index + 1] == strategy.type_2) {
  //         return OppositeDirection(strategy.direction_2);
  //       } else if (input_types[index + 1] == strategy.type_3) {
  //         return OppositeDirection(strategy.direction_3);
  //       } else {
  //         UNEXPECTED(); exit(1);
  //       }
  //     } else {
  //       return strategy.direction_1;
  //     }
  //   }
  // };

  for (int index = 0; index < int(input_types.size()); index++) {
    PutCandy(input_positions[index], input_types[index], board);
    if (index == int(input_types.size()) - 1) {
      break;
    }
    char action = ChooseActionGreedy(
        input_types[index + 1], last_fb, last_lr, strategy);
    ExecuteAction(action, board);
    // char action = choose_action(index);
    if (action == 'F' || action == 'B') {
      last_fb = action;
    }
    if (action == 'L' || action == 'R') {
      last_lr = action;
    }
  }

  return ComputeScore(board);
}

class Main {
 private:
  RandomGenerator random_;
  TimeMeasurement time_;
  vector<vector<int>> board_;
  vector<int> types_;
  char last_fb_;
  char last_lr_;

  void PrintBoardToError() {
    for (int y = 0; y < kBoardSize; y++) {
      for (int x = 0; x < kBoardSize; x++) {
        cerr << board_[y][x];
      }
      cerr << endl;
    }
  }

 public:
  void Run() {
    board_ = Make2D(kBoardSize, kBoardSize, 0);
    types_ = vector<int>(kCandyCount);
    last_fb_ = 'X';
    last_lr_ = 'X';

    for (int i = 0; i < kCandyCount; i++) {
      cin >> types_[i];
    }

    Strategy strategy({1, 2, 3, 'F', 'L', 'R'});

    for (int i = 0; i < kCandyCount; i++) {
      int position_id; cin >> position_id;
      PutCandy(position_id, types_[i], board_);

      if (i == kCandyCount - 1) {
        cout << 'F' << endl;
        break;
      }

      if (time_.CurrentMs() > 1800) {
        char action = ChooseActionGreedy(
            types_[i + 1], last_fb_, last_lr_, strategy);
        // cerr << "action: " << action << endl;
        ExecuteAction(action, board_);
        if (action == 'F' || action == 'B') {
          last_fb_ = action;
        }
        if (action == 'L' || action == 'R') {
          last_lr_ = action;
        }
        cout << action << endl;
        continue;
      }

      vector<int> future_input_types;
      for (int j = i + 1; j < kCandyCount; j++) {
        future_input_types.push_back(types_[j]);
      }
      const int kRandomSize = 150;
      vector<vector<int>> future_input_positions_list;
      for (int k = 0; k < kRandomSize; k++) {
        vector<int> future_input_positions;
        for (int j = i + 1; j < kCandyCount; j++) {
          future_input_positions.push_back(
              random_.RandomInt(1, kCandyCount - j, true));
        }
        future_input_positions_list.push_back(future_input_positions);
      }

      double max_score = 0;
      char max_action = 'X';
      for (char next_action : array<char, 4>({'F', 'B', 'L', 'R'})) {
        char last_fb_copied = last_fb_;
        char last_lr_copied = last_lr_;
        vector<vector<int>> board_copied = board_;
        ExecuteAction(next_action, board_copied);
        if (next_action == 'F' || next_action == 'B') {
          last_fb_copied = next_action;
        }
        if (next_action == 'L' || next_action == 'R') {
          last_lr_copied = next_action;
        }

        double max_score_for_action = 0;
        for (const auto strategy : strategy_list) {
          double score = 0;
          for (const auto& future_input_positions :
               future_input_positions_list) {
            double delta_score = RunGreedy(
                board_copied, last_fb_copied, last_lr_copied,
                strategy, future_input_types, future_input_positions);
            // cerr << delta_score << endl;
            score += delta_score;
          }
          if (score > max_score_for_action) {
            max_score_for_action = score;
          }
        }

        if (max_score_for_action > max_score) {
          max_score = max_score_for_action;
          max_action = next_action;
        }
      }

      // char action = ChooseActionGreedy(
      //     types_[i + 1], last_fb_, last_lr_, strategy);
      const char action = max_action;
      // cerr << "action: " << action << endl;
      ExecuteAction(action, board_);
      if (action == 'F' || action == 'B') {
        last_fb_ = action;
      }
      if (action == 'L' || action == 'R') {
        last_lr_ = action;
      }
      cout << action << endl;
    }

    cerr << "score: " << ComputeScore(board_) << endl;
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);

  Main main;
  main.Run();
}
