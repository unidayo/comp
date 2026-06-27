#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <random>
#include <tuple>
#include <vector>
#include <cmath>
#include <cstdint>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <atcoder/dsu>

constexpr double kTimeLimit = 3.0;
constexpr int kNumComputers = 100;
constexpr int kMaxNumTypes = 5;

constexpr int kPaddedGridSize = 50;
constexpr int kNumCells = kPaddedGridSize * kPaddedGridSize;
constexpr int kDeltas[] = {-kPaddedGridSize, -1, kPaddedGridSize, 1};

constexpr int ToCellIndex(int row, int column) {
  return kPaddedGridSize * row + column;
}

class Position {
 public:
  explicit Position(int index = 0) : index_(index) {}
  Position(int y, int x) : index_(ToCellIndex(y, x)) {}

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

  double GetTime() const {
    return std::chrono::duration<double>(Now() - start).count();
  }

  const decltype(Now()) start;
};

template <typename State, typename RNG>
State Anneal(State state, RNG& rng, double T0, double T1,
             Timer& timer, double time_limit) {
  auto best_state = state;
  auto T = T0;
  const auto start = timer.GetTime();
  for (int i = 0; ; ++i) {
    if ((i & ((1 << 4) - 1)) == 0) {
      const auto progress = (timer.GetTime() - start) / (time_limit - start);
      if (progress >= 1.0) break;
      T = std::pow(T0, 1.0 - progress) * std::pow(T1, progress);
    }
    const auto gain = state.SuggestChange(rng) / T;
    if (std::uniform_real_distribution(0.0, 1.0)(rng) < std::exp(gain)) {
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

struct Solution {
  std::vector<std::tuple<Position, Position>> moves;
  std::vector<std::tuple<Position, Position>> links;
};

struct Change {
  bool valid = false;
  Position origin;
  std::int8_t role;
  std::vector<Position> path;
};

struct RightShift {
  template <typename T>
  int operator()(const T& a, int offset) const {
    return std::get<0>(a) >> offset;
  }
};

struct State {
  void Initialize(int N, int K, const std::vector<std::string>& c) {
    finishing = false;
    grid_size = N;
    num_types = K;
    initial_stamina = stamina = kNumComputers * num_types;
    cells.assign(kNumCells, -1);
    for (int y = 0; y < grid_size; ++y) {
      for (int x = 0; x < grid_size; ++x) {
        cells[ToCellIndex(y + 1, x + 1)] = c[y][x] - '0';
      }
    }
    roles = cells;
    edges.resize(3);
    paths.resize(kNumCells);
    visitors.resize(kNumCells);
    origins.resize(kNumCells);
    occupied_horizontal.resize(kPaddedGridSize);
    occupied_vertical.resize(kPaddedGridSize);
    counts.resize(kNumCells);
    for (int y = 1; y <= grid_size; ++y) {
      for (int x = 1; x <= grid_size; ++x) {
        const Position position(y, x);
        paths[position].assign(1, position);
        origins[position] = position;
        if (cells[position] != 0) {
          occupied_horizontal[y].set(x);
          occupied_vertical[x].set(y);
        }
      }
    }
    score = ComputeScore();
  }

  Solution GetSolution() {
    Solution solution;
    ComputeScore(&solution);
    std::queue<Position> queue;
    for (int y = 1; y <= grid_size; ++y) {
      for (int x = 1; x <= grid_size; ++x) {
        const Position pos(y, x);
        if (paths[pos].size() > 1) {
          queue.push(pos);
        }
      }
    }
    while (!queue.empty()) {
      const auto pos = queue.front();
      queue.pop();
      while (paths[pos].size() > 1) {
        const auto prev = *(paths[pos].rbegin() + 1);
        if (cells[prev] != 0) break;
        solution.moves.emplace_back(prev, paths[pos].back());
        std::swap(cells[prev], cells[paths[pos].back()]);
        paths[pos].pop_back();
      }
      if (paths[pos].size() > 1) {
        queue.push(pos);
      }
    }
    std::reverse(solution.moves.begin(), solution.moves.end());
    return solution;
  }

  int CheckTargets(Position position, int target_type) const {
    int bits = 0;
    const auto [py, px] = position;
    for (int y = py - 1; ; --y) {
      if (const auto& role = roles[Position(y, px)]) {
        bits |= (role == target_type) << 0;
        break;
      }
    }
    for (int x = px - 1; ; --x) {
      if (const auto& role = roles[Position(py, x)]) {
        bits |= (role == target_type) << 1;
        break;
      }
    }
    for (int y = py + 1; ; ++y) {
      if (const auto& role = roles[Position(y, px)]) {
        bits |= (role == target_type) << 2;
        break;
      }
    }
    for (int x = px + 1; ; ++x) {
      if (const auto& role = roles[Position(py, x)]) {
        bits |= (role == target_type) << 3;
        break;
      }
    }
    return bits;
  }

  template <typename RNG>
  double SuggestChange(RNG& rng) {
    change.valid = false;
    while (!change.valid) {
      const int target_type = std::uniform_int_distribution(1, finishing ? num_types : 2)(rng);
      const int y = std::uniform_int_distribution(1, grid_size)(rng);
      const int x = std::uniform_int_distribution(1, grid_size)(rng);
      const Position current(y, x);
      if (cells[current] == 0) {
        bool is_cross = false;
        std::array<Position, 4> nexts;
        for (int d = 0; d < 4; ++d) {
          for (auto next = current + kDeltas[d]; ; next += kDeltas[d]) {
            if (cells[next] == 0) continue;
            nexts[d] = next;
            break;
          }
        }
        for (int d = 0; d < 4; ++d) {
          if (roles[nexts[d]] == target_type &&
              roles[nexts[(d + 1) % 4]] == target_type) {
            if (std::min(dsu.size(nexts[d]), dsu.size(nexts[(d + 1) % 4])) >= 2) {
              is_cross = true;
              break;
            }
          }
        }
        if (is_cross) {
          change.origin = Position(0);
          Position meet(0);
          const auto delta = kDeltas[std::uniform_int_distribution(0, 3)(rng)];
          int distance = 0;
          for (meet = current; cells[meet] >= 0 && distance <= 4; ++distance, meet += delta) {
            if (visitors[meet] != 0) {
              if (cells[paths[visitors[meet]].back()] == target_type) {
                change.origin = visitors[meet];
              }
              break;
            }
            if (cells[meet] != 0) {
              if (cells[paths[origins[meet]].back()] == target_type) {
                change.origin = origins[meet];
              }
              break;
            }
          }
          if (change.origin != 0) {
            change.valid = true;
            change.role = target_type;
            change.path = paths[change.origin];
            const auto iter = std::find(change.path.begin(), change.path.end(), meet);
            change.path.erase(std::next(iter), change.path.end());
            while (meet != current) {
              change.path.push_back(meet += -delta);
            }
            break;
          }
        }
      }
      if (cells[current] <= 0) continue;
      change.origin = origins[current];
      auto& path = paths[change.origin];
      change.role = roles[current];
      change.path = path;
      if (path.size() > 1 && std::uniform_int_distribution(0, 1)(rng) == 0) {
        const auto index = std::uniform_int_distribution<std::size_t>(0, path.size() - 2)(rng);
        if (cells[path[index]] != 0) continue;
        change.valid = true;
        change.path.resize(index + 1);
      } else if (visitors[current] == 0 && std::uniform_int_distribution(0, 1)(rng) == 0) {
        if (roles[current] != target_type) {
          if (CheckTargets(current, target_type) == 0) continue;
        }
        const auto delta = kDeltas[std::uniform_int_distribution(0, 3)(rng)];
        const Position next = current + delta;
        if (cells[next] != 0) continue;
        change.valid = true;
        if (const auto iter = std::find(change.path.begin(), change.path.end(), next); iter != change.path.end()) {
          change.path.erase(std::next(iter), change.path.end());
        } else {
          change.path.push_back(next);
        }
      }
      if (cells[current] != target_type) {
        if (roles[current] != cells[current]) {
          if (!change.valid || std::uniform_int_distribution(0, 1)(rng) == 0) {
            change.valid = true;
            change.role = cells[current];
          }
        } else {
          if (!change.valid || std::uniform_int_distribution(0, 1)(rng) == 0) {
            const auto backup = std::exchange(roles[current], 0);
            const int bits = CheckTargets(change.path.back(), target_type);
            if (__builtin_popcount(bits) >= 2) {
              change.valid = true;
              change.role = target_type;
            }
            roles[current] = backup;
          }
        }
      }
    }
    MakeChange();
    next_score = ComputeScore();
    return next_score - score;
  }

  void MakeChange() {
    const auto current = paths[change.origin].back();
    auto& path = paths[change.origin];
    const auto [y0, x0] = path.back();
    const auto [y1, x1] = change.path.back();
    occupied_horizontal[y0].reset(x0);
    occupied_horizontal[y1].set(x1);
    occupied_vertical[x0].reset(y0);
    occupied_vertical[x1].set(y1);
    stamina += path.size();
    stamina -= change.path.size();
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
      visitors[path[i]] = Position(0);
    }
    std::swap(roles[current], change.role);
    std::swap(origins[current], origins[change.path.back()]);
    std::swap(cells[current], cells[change.path.back()]);
    std::swap(roles[current], roles[change.path.back()]);
    std::swap(path, change.path);
    for (std::size_t i = 0; i + 1 < path.size(); ++i) {
      visitors[path[i]] = change.origin;
    }
  }

  void AcceptChange() {
    score = next_score;
  }

  void RejectChange() {
    MakeChange();
    std::swap(dsu, previous_dsu);
  }

  double ComputeScore(Solution* solution = nullptr) {
    std::swap(dsu, previous_dsu);
    dsu = atcoder::dsu(kNumCells);
    if (!finishing && stamina < std::min<int>(kNumComputers * 2, initial_stamina / 2)) {
      return -std::numeric_limits<double>::infinity();
    }
    int stamina_left = stamina;
    used_cells = cells;
    for (std::size_t i = 0; i < edges.size(); ++i) {
      edges[i].clear();
    }
    for (int y = 1; y <= grid_size; ++y) {
      if (occupied_horizontal[y].count() < 2) continue;
      int prev_x = occupied_horizontal[y]._Find_first();
      for (int x = occupied_horizontal[y]._Find_next(prev_x); x <= grid_size;
           prev_x = x, x = occupied_horizontal[y]._Find_next(x)) {
        const Position prev(y, prev_x);
        const Position current(y, x);
        if (roles[prev] != roles[current]) continue;
        edges[std::min(2, roles[prev] - 1)].emplace_back(x - prev_x, prev, current);
      }
    }
    for (int x = 1; x <= grid_size; ++x) {
      if (occupied_vertical[x].count() < 2) continue;
      int prev_y = occupied_vertical[x]._Find_first();
      for (int y = occupied_vertical[x]._Find_next(prev_y); y <= grid_size;
           prev_y = y, y = occupied_vertical[x]._Find_next(y)) {
        const Position prev(prev_y, x);
        const Position current(y, x);
        if (roles[prev] != roles[current]) continue;
        edges[std::min(2, roles[prev] - 1)].emplace_back(y - prev_y, prev, current);
      }
    }
    for (int target_index = 0; target_index < 2 + finishing; ++target_index) {
      if (stamina_left == 0) break;
      boost::sort::spreadsort::integer_sort(edges[target_index].begin(), edges[target_index].end(), RightShift());
      std::unique_ptr<Solution> solution_backup;
      if (solution) {
        solution_backup.reset(new Solution);
        *solution_backup = *solution;
      }
      used_cells_backup = used_cells;
      dsu_backup = dsu;
      int left = stamina_left;
      const auto connect = [&](int distance, Position a, Position b) {
        if (dsu.same(a, b)) return;
        const auto [ya, xa] = a;
        const auto [yb, xb] = b;
        if (distance > 1) {
          bool available = true;
          if (ya == yb) {
            const auto& [min, max] = std::minmax(xa, xb);
            for (int x = min + 1; x < max; ++x) {
              if (used_cells[Position(ya, x)] < 0) {
                available = false;
              }
            }
          } else {
            const auto& [min, max] = std::minmax(ya, yb);
            for (int y = min + 1; y < max; ++y) {
              if (used_cells[Position(y, xa)] < 0) {
                available = false;
              }
            }
          }
          if (!available) return;
          if (ya == yb) {
            const auto& [min, max] = std::minmax(xa, xb);
            for (int x = min + 1; x < max; ++x) {
              used_cells[Position(ya, x)] = -1;
            }
          } else {
            const auto& [min, max] = std::minmax(ya, yb);
            for (int y = min + 1; y < max; ++y) {
              used_cells[Position(y, xa)] = -1;
            }
          }
        }
        dsu.merge(a, b);
        if (solution) {
          solution->links.emplace_back(a, b);
        }
        --left;
      };
      for (const auto& [distance, a, b] : edges[target_index]) {
        connect(distance, a, b);
      }
      if (left < 0) {
        edges_with_distance.clear();
        for (const auto& [distance, a, b] : edges[target_index]) {
          edges_with_distance.emplace_back(-dsu.size(a), distance, a, b);
        }
        boost::sort::spreadsort::integer_sort(edges_with_distance.begin(), edges_with_distance.end(), RightShift());
        std::swap(used_cells, used_cells_backup);
        std::swap(dsu, dsu_backup);
        if (solution) {
          std::swap(solution->links, solution_backup->links);
        }
        left = stamina_left;
        for (const auto& [size, distance, a, b] : edges_with_distance) {
          if (left <= 0) break;
          connect(distance, a, b);
        }
      }
      stamina_left = left;
    }

    int sum = 0;
    leaders.clear();
    if (grid_size * grid_size < kNumComputers * num_types * 3) {
      for (int y = 1; y <= grid_size; ++y) {
        for (int x = 1; x <= grid_size; ++x) {
          const Position pos(y, x);
          if (dsu.size(pos) < 2) continue;
          ++counts[dsu.leader(pos)][cells[pos]];
          if (pos == dsu.leader(pos)) {
            leaders.push_back(pos);
          }
        }
      }
    } else {
      for (int y = 1; y <= grid_size; ++y) {
        for (int x = occupied_horizontal[y]._Find_first(); x <= grid_size;
             x = occupied_horizontal[y]._Find_next(x)) {
          const Position pos(y, x);
          if (dsu.size(pos) < 2) continue;
          ++counts[dsu.leader(pos)][cells[pos]];
          if (pos == dsu.leader(pos)) {
            leaders.push_back(pos);
          }
        }
      }
    }
    for (const auto& leader : leaders) {
      for (int i = 1; i <= num_types; ++i) {
        if (i == roles[leader]) {
          sum += counts[leader][i] * (counts[leader][i] - 1) / 2;
        }
        for (int j = 1; j < i; ++j) {
          sum -= counts[leader][i] * counts[leader][j];
        }
      }
      std::fill(counts[leader].begin(), counts[leader].end(), 0);
    }
    if (!finishing) {
      sum += std::min<int>(kNumComputers * 2, stamina);
    }
    return sum;
  }

  double score;
  double next_score;

  int num_types;
  int grid_size;
  int initial_stamina;
  int stamina;
  Change change;
  bool finishing;
  std::vector<std::vector<Position>> paths;
  std::vector<Position> origins;
  std::vector<Position> visitors;
  std::vector<std::int8_t> roles;
  std::vector<std::int8_t> cells;
  std::vector<std::bitset<50>> occupied_horizontal;
  std::vector<std::bitset<50>> occupied_vertical;

  std::vector<std::int8_t> used_cells;
  std::vector<std::int8_t> used_cells_backup;
  std::vector<std::vector<std::tuple<int, Position, Position>>> edges;
  std::vector<std::tuple<int, int, Position, Position>> edges_with_distance;
  atcoder::dsu dsu;
  atcoder::dsu dsu_backup;
  atcoder::dsu previous_dsu;
  std::vector<Position> leaders;
  std::vector<std::array<int, kMaxNumTypes + 1>> counts;
};

int main() {
  Timer timer;

  int N, K;
  std::cin >> N >> K;
  std::vector<std::string> c(N);
  for (auto& row : c) {
    std::cin >> row;
  }

  std::mt19937 rng;
  const double T0 = 1.0e1;
  const double T1 = 1.0e0;
  State state;
  state.Initialize(N, K, c);
  if (K > 2) {
    state = Anneal(state, rng, T0, T1, timer, 0.98 * kTimeLimit);
    state.finishing = true;
    state = Anneal(state, rng, T1, T1, timer, 0.99 * kTimeLimit);
  } else {
    state = Anneal(state, rng, T0, T1, timer, 0.99 * kTimeLimit);
  }
  const auto solution = state.GetSolution();

  std::cout << solution.moves.size() << std::endl;
  for (const auto& [from, to] : solution.moves) {
    std::cout << from.Y() - 1 << ' ' << from.X() - 1 << ' '
              << to.Y() - 1 << ' ' << to.X() - 1 << std::endl;
  }
  std::cout << solution.links.size() << std::endl;
  for (const auto& [from, to] : solution.links) {
    std::cout << from.Y() - 1 << ' ' << from.X() - 1 << ' '
              << to.Y() - 1 << ' ' << to.X() - 1 << std::endl;
  }

  return 0;
}
