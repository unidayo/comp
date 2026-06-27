#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include <cmath>

constexpr double kTimeLimit = 2.0;
constexpr int kGridSize = 30;
constexpr int kPaddedGridSize = kGridSize + 2;
constexpr int kNumTiles = kPaddedGridSize * kPaddedGridSize;
constexpr int kDeltas[4] = {-1, -kPaddedGridSize, 1, kPaddedGridSize};
constexpr int kTileDirections[9][4] = {
  {1, 0, -1, -1},
  {3, -1, -1, 0},
  {-1, -1, 3, 2},
  {-1, 2, 1, -1},
  {1, 0, 3, 2},
  {3, 2, 1, 0},
  {2, -1, 0, -1},
  {-1, 3, -1, 1},
  {-1, -1, -1, -1},
};
const std::vector<std::vector<int>> kRotatedTiles = {
  {0, 1, 2, 3},
  {0, 1, 2, 3},
  {0, 1, 2, 3},
  {0, 1, 2, 3},
  {4, 5},
  {4, 5},
  {6, 7},
  {6, 7},
  {8},
};

constexpr int ToTileIndex(int row, int column) {
  return kPaddedGridSize * row + column;
}

class Position {
 public:
  explicit Position(int index) : index_(index) {}
  Position(int y, int x) : index_(ToTileIndex(y, x)) {}

  constexpr int Y() const {
    return index_ / kPaddedGridSize;
  }

  constexpr int X() const {
    return index_ % kPaddedGridSize;
  }

  template <std::size_t Index>
  constexpr int get() const {
    if constexpr (Index == 0) {
      return Y();
    } else {
      return X();
    }
  }

  operator int() const {
    return index_;
  }

  Position& operator+=(int delta) {
    index_ += delta;
    return *this;
  }

  friend Position operator+(Position position, int delta) {
    return position += delta;
  }

 private:
  int index_;
};

template <>
struct std::tuple_size<Position> : std::integral_constant<std::size_t, 2> {};

template <std::size_t Index>
struct std::tuple_element<Index, Position> {
  using type = int;
};

auto Now() {
  return std::chrono::high_resolution_clock::now();
}

struct Timer {
  Timer() : start(Now()) {}

  double GetTime() {
    return std::chrono::duration<double>(Now() - start).count();
  }

  const decltype(Now()) start;
};

struct Constraints {
  void Initialize() {
    candidates.clear();
    for (int i = 0; i < kNumTiles; ++i) {
      if (fixed[i]) continue;
      candidates.push_back(i);
    }
  }

  template <typename RNG>
  Position ChoosePosition(RNG& rng) const {
    return Position(candidates[std::uniform_int_distribution<std::size_t>(0, candidates.size() - 1)(rng)]);
  }

  std::vector<int> fixed = std::vector<int>(kNumTiles);
  std::vector<int> candidates;
};

struct State {
  State(const Constraints& constraints_, const std::vector<int>& initial_tiles) :
      constraints(constraints_), tiles(initial_tiles) {
    score = 0;
  }

  State(const State& state) :
      constraints(state.constraints), score(state.score), tiles(state.tiles), last_visited(state.last_visited) {}

  State& operator=(const State& state) {
    tiles = state.tiles;
    score = state.score;
    last_visited = state.last_visited;
    return *this;
  }

  template <typename RNG>
  double SuggestChange(RNG& rng) {
    next = tiles;
    fixed.assign(kNumTiles, 0);
    visited.assign(4 * kNumTiles, 0);
    while (true) {
      search_count = 0;
      const auto start = constraints.ChoosePosition(rng);
      const int original = next[start];
      auto rotated = kRotatedTiles[original];
      std::shuffle(rotated.begin(), rotated.end(), rng);
      for (std::size_t i = 1; i < rotated.size(); ++i) {
        if (rotated[i] == original) {
          std::swap(rotated[0], rotated[i]);
          break;
        }
      }
      fixed[start] = 1;
      for (const auto& tile : rotated) {
        next[start] = tile;
        for (int d = 0; d < 4; ++d) {
          if (kTileDirections[tile][d] < 0) continue;
          visited[4 * start + d] = 1;
          if (Search(start, kTileDirections[tile][d], 2, rng)) {
            return next_score - score;
          }
          visited[4 * start + d] = 0;
        }
      }
      fixed[start] = 0;
      next[start] = original;
    }
  }

  template <typename RNG>
  bool Search(Position prev, int direction, int depth, RNG& rng) {
    if (++search_count > 1000) return false;
    const Position pos = prev + kDeltas[direction];
    if (tiles[pos] == 8) {
      return false;
    }
    direction ^= 2;
    if (visited[4 * pos + direction]) {
      next_score = depth - visited[4 * pos + direction];
      return true;
    }
    const int original = next[pos];
    std::vector<int> rotated;
    const bool was_fixed = fixed[pos];
    if (was_fixed || constraints.fixed[pos]) {
      rotated.push_back(next[pos]);
    } else {
      rotated = kRotatedTiles[original];
      std::shuffle(rotated.begin(), rotated.end(), rng);
      for (std::size_t i = 1; i < rotated.size(); ++i) {
        if (rotated[i] == original) {
          std::swap(rotated[0], rotated[i]);
          break;
        }
      }
    }
    fixed[pos] = 1;
    visited[4 * pos + direction] = depth;
    for (const auto& tile : rotated) {
      if (kTileDirections[tile][direction] < 0) continue;
      next[pos] = tile;
      if (Search(pos, kTileDirections[tile][direction], depth + 1, rng)) {
        return true;
      }
    }
    visited[4 * pos + direction] = 0;
    fixed[pos] = was_fixed;
    next[pos] = original;
    return false;
  }

  void AcceptChange() {
    tiles.swap(next);
    score = next_score;
    last_visited.swap(visited);
  }

  void RejectChange() {
  }

  const Constraints& constraints;
  int score;
  int next_score;
  std::vector<int> tiles;
  std::vector<int> next;
  std::vector<int> fixed;
  std::vector<int> visited;
  std::vector<int> last_visited;
  int search_count;
};

template <typename State, typename RNG>
State Anneal(State state, RNG& rng, double T0, double T1,
             Timer& timer, double time_limit) {
  State best_state = state;
  const double start = timer.GetTime();
  for (int i = 0; ; ++i) {
    const double progress = (timer.GetTime() - start) / (time_limit - start);
    if (progress >= 1.0) break;
    const double T = std::pow(T0, 1.0 - progress) * std::pow(T1, progress);
    const double gain = state.SuggestChange(rng);
    if (std::uniform_real_distribution(0.0, 1.0)(rng) < std::exp(gain / T)) {
      state.AcceptChange();
      if (state.score > best_state.score) {
        best_state = state;
      }
    } else {
      state.RejectChange();
    }
  }
  return best_state;
}

class Solver {
 public:
  Solver(const std::vector<int>& tiles) : tiles_(tiles) {}

  std::vector<int> Solve() {
    Timer timer;
    std::mt19937 rng;
    auto result = tiles_;
    std::vector<int> visited;
    for (int t = 0; t < 2; ++t) {
      Constraints constraints;
      for (int i = 0; i < kPaddedGridSize; ++i) {
        for (int j = 0; j < kPaddedGridSize; ++j) {
          const auto index = ToTileIndex(i, j);
          bool fixed = i == 0 || i + 1 == kPaddedGridSize ||
                            j == 0 || j + 1 == kPaddedGridSize;
          if (t == 0) {
            if (j < kPaddedGridSize / 2) {
              fixed = true;
            }
          } else {
            for (int d = 0; d < 4; ++d) {
              if (visited[4 * index + d]) {
                fixed = true;
              }
            }
          }
          constraints.fixed[index] = fixed;
        }
      }
      if (t == 1) {
        for (int i = 1; i <= kGridSize; ++i) {
          bool fixed = false;
          for (int j = 1; j <= kGridSize; ++j) {
            const auto index = ToTileIndex(i, j);
            if (constraints.fixed[index]) {
              fixed = true;
            }
            constraints.fixed[index] = fixed;
          }
        }
      }
      constraints.Initialize();

      const double time_limit = t == 0 ? 0.49 * kTimeLimit : 0.98 * kTimeLimit;
      const double T0 = 1.0e2;
      const double T1 = 1.0e0;
      State state(constraints, result);
      state = Anneal(state, rng, T0, T1, timer, time_limit);
      result = state.tiles;
      visited = state.last_visited;
    }

    return result;
  }

 private:
  std::vector<int> tiles_;
};

int main() {
  std::vector<int> tiles(kNumTiles, 8);
  for (int i = 1; i <= kGridSize; ++i) {
    std::string row;
    std::cin >> row;
    for (int j = 1; j <= kGridSize; ++j) {
      tiles[ToTileIndex(i, j)] = row[j - 1] - '0';
    }
  }

  Solver solver(tiles);
  const auto result = solver.Solve();
  std::string rotation;
  for (int i = 1; i <= kGridSize; ++i) {
    for (int j = 1; j <= kGridSize; ++j) {
      const auto original = tiles[ToTileIndex(i, j)];
      const auto rotated = result[ToTileIndex(i, j)];
      if (rotated < 4) {
        rotation.push_back('0' + (rotated + 4 - original) % 4);
      } else {
        rotation.push_back('0' + (rotated != original));
      }
    }
  }
  std::cout << rotation << std::endl;

  return 0;
}
