#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr double kTimeLimit = 5.95;
constexpr int kGridSize = 16;
constexpr int kNumCubes = kGridSize * kGridSize * kGridSize;
constexpr int kMaxNumBlocks = 30;

constexpr int kP0 = 1;
constexpr int kN0 = -1;
constexpr int kP1 = kGridSize;
constexpr int kN1 = -kGridSize;
constexpr int kP2 = kGridSize * kGridSize;
constexpr int kN2 = -kGridSize * kGridSize;
constexpr int kOrientations[24][3] = {
  {kP0, kP1, kP2}, {kP0, kN1, kN2}, {kP0, kP2, kN1}, {kP0, kN2, kP1},
  {kN0, kP1, kN2}, {kN0, kN1, kP2}, {kN0, kP2, kP1}, {kN0, kN2, kN1},
  {kP1, kP0, kN2}, {kP1, kN0, kP2}, {kP1, kP2, kP0}, {kP1, kN2, kN0},
  {kN1, kP0, kP2}, {kN1, kN0, kN2}, {kN1, kP2, kN0}, {kN1, kN2, kP0},
  {kP2, kP0, kP1}, {kP2, kN0, kN1}, {kP2, kP1, kN0}, {kP2, kN1, kP0},
  {kN2, kP0, kN1}, {kN2, kN0, kP1}, {kN2, kP1, kP0}, {kN2, kN1, kN0},
};
constexpr int kDeltas[6][3] = {
  {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1},
};

constexpr auto CreateDeltaTable() {
  std::array<std::array<std::array<int, 2>, 6>, 24> table = {};
  const auto& [ox0, oy0, oz0] = kOrientations[0];
  for (int i = 0; i < 24; ++i) {
    const auto& [ox1, oy1, oz1] = kOrientations[i];
    for (int j = 0; j < 6; ++j) {
      const auto& [dx, dy, dz] = kDeltas[j];
      table[i][j][0] = dx * ox0 + dy * oy0 + dz * oz0;
      table[i][j][1] = dx * ox1 + dy * oy1 + dz * oz1;
    }
  }
  return table;
}

constexpr auto kDeltaTable = CreateDeltaTable();

template <typename T, int Capacity>
class StaticVector {
 public:
  constexpr T& operator[](int index) {
    return data_[index];
  }

  constexpr const T& operator[](int index) const {
    return data_[index];
  }

  constexpr const T& back() const {
    return data_[size_ - 1];
  }

  constexpr int size() const {
    return size_;
  }

  constexpr bool empty() const {
    return size_ == 0;
  }

  constexpr const T* begin() const {
    return data_;
  }

  constexpr const T* end() const {
    return data_ + size_;
  }

  constexpr void push_back(const T& value) {
    data_[size_++] = value;
  }

  constexpr void pop_back() {
    --size_;
  }

  constexpr void clear() {
    size_ = 0;
  }

 private:
  T data_[Capacity];
  int size_ = 0;
};

template <typename T, int Capacity>
class StaticQueue {
 public:
  constexpr const T& front() const {
    return data_[begin_];
  }

  constexpr bool empty() const {
    return begin_ == end_;
  }

  constexpr void push(const T& value) {
    data_[end_] = value;
    end_ = (end_ + 1) % Capacity;
  }

  constexpr void pop() {
    begin_ = (begin_ + 1) % Capacity;
  }

  constexpr void clear() {
    begin_ = end_;
  }

 private:
  T data_[Capacity];
  int begin_ = 0;
  int end_ = 0;
};

class Position {
 public:
  explicit Position(std::uint16_t index = 0) : index_(index) {}
  Position(std::uint8_t x, std::uint8_t y, std::uint8_t z)
      : index_(kGridSize * (kGridSize * x + y) + z) {}

  constexpr std::uint8_t X() const {
    return index_ / (kGridSize * kGridSize);
  }

  constexpr std::uint8_t Y() const {
    return index_ / kGridSize % kGridSize;
  }

  constexpr std::uint8_t Z() const {
    return index_ % kGridSize;
  }

  template <std::size_t Index>
  constexpr std::uint8_t get() const {
    if constexpr (Index == 0) {
      return X();
    } else if constexpr (Index == 1) {
      return Y();
    } else {
      return Z();
    }
  }

  operator std::uint16_t() const {
    return index_;
  }

 private:
  std::uint16_t index_;
};

template <>
struct std::tuple_size<Position> : std::integral_constant<std::size_t, 3> {};

template <std::size_t Index>
struct std::tuple_element<Index, Position> {
  using type = std::uint8_t;
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

class Random {
 public:
  constexpr explicit Random(std::int64_t seed) {
    state_[0] = seed;
    state_[1] = RotateLeft(seed, 11);
    for (int i = 0; i < 16; ++i) {
      Next();
    }
  }

  constexpr double Canonical() {
    return 0x1.p-64 * Next();
  }

  constexpr int Range(int size) {
    return Next() % size;
  }

 private:
  constexpr std::uint64_t Next() {
    auto [s0, s1] = state_;
    const auto result = RotateLeft(s0 * 5, 7) * 9;
    s1 ^= s0;
    state_[0] = RotateLeft(s0, 24) ^ s1 ^ (s1 << 16);
    state_[1] = RotateLeft(s1, 37);
    return result;
  }

  static constexpr std::uint64_t RotateLeft(std::uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
  }

  std::uint64_t state_[2] = {};
};

template <typename State>
State Anneal(State state, Random& rng, double T0, double T1,
             Timer& timer, double time_limit) {
  int count = 0;
  auto best_state = state;
  auto T = T0;
  const auto start = timer.GetTime();
  const auto progress_scale = 1.0 / (time_limit - start);
  double progress = 0.0;
  for (int i = 0; ; ++i) {
    if ((i & ((1 << 4) - 1)) == 0) {
      progress = (timer.GetTime() - start) * progress_scale;
      if (progress >= 1.0) break;
      T = std::pow(T0, 1.0 - progress) * std::pow(T1, progress);
    }
    const auto tolerance = -T * std::log2(rng.Canonical());
    if (state.TryTransition(progress, tolerance, rng)) {
      if (state.score > best_state.score) {
        count = 0;
        best_state = state;
      }
    }
    if (++count >= 100'000) {
      count = 0;
      state = best_state;
    }
  }
  return best_state;
}

struct State {
  State(int D, const std::vector<std::vector<std::string>>& silhouettes_string) : toy_size(D) {
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        for (int z = 0; z < toy_size; ++z) {
          for (int x = 0; x < toy_size; ++x) {
            silhouettes[i][z + 1][j].set(x + 1, silhouettes_string[2 * i + j][z][x] == '1');
          }
        }
      }
    }

    available_blocks = (1 << kMaxNumBlocks) - 1;
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < kNumCubes; ++j) {
        const Position pos(j);
        position_table[i][j] = pos;
        const auto [x, y, z] = pos;
        if (!silhouettes[i][z][0].test(x) || !silhouettes[i][z][1].test(y)) continue;
        position_candidates[i].push_back(pos);
        masks[i].set(j);
      }
    }

    sum_block_size = 0.0;
    sum_unsatisfied = 0.0;
    sum_overlap = 0.0;
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        for (int z = 1; z <= toy_size; ++z) {
          for (int x = 1; x <= toy_size; ++x) {
            if (!silhouettes[i][z][j].test(x)) continue;
            sum_unsatisfied += 1;
          }
        }
      }
    }

    best_score = score = ComputeScore();
  }

  bool TryTransition(double progress, double tolerance, Random& rng) {
    double weight_unsatisfied = 0.25;
    double weight_overlap = 0.025;
    if (best_score < -10.0 && progress > 0.5) {
      weight_unsatisfied += 2.5 * std::pow(2.0 * (progress - 0.5), 2.0);
    }
    if (best_score < -100.0 && progress > 0.75) {
      weight_overlap += 25.0 * std::pow(4.0 * (progress - 0.75), 2.0);
    }

    double delta_block_size = 0.0;
    double delta_unsatisfied = 0.0;
    double delta_overlap = 0.0;
    int added_block = -1;
    int removed_block = -1;

    const auto remove_block = [&](int block_index) {
      removed_block = used_blocks[block_index];

      delta_block_size -= 1.0 / positions[removed_block][0].size();

      used_blocks[block_index] = used_blocks.back();
      used_blocks.pop_back();
      for (int i = 0; i < 2; ++i) {
        for (const auto& pos : positions[removed_block][i]) {
          const auto [x, y, z] = Position(pos);
          delta_unsatisfied += --silhouette_counts[i][z][0][x] == 0;
          delta_unsatisfied += --silhouette_counts[i][z][1][y] == 0;
          delta_overlap -= --overlap_counts[i][pos];
        }
      }
    };

    const auto add_block = [&](const Position starts[2], int orientation_index) {
      block_positions.clear();
      block_positions.push_back(starts[0]);

      const auto& delta_table = kDeltaTable[orientation_index];
      const auto add_neighbors = [&](Position pos) {
        for (const auto& [delta0, delta1] : delta_table) {
          const Position nexts[2] = {
            Position(position_table[0][pos] + delta0),
            Position(position_table[1][pos] + delta1),
          };
          if (seen[nexts[0]] == generation) continue;
          seen[nexts[0]] = generation;
          if (!masks[0].test(nexts[0]) || !masks[1].test(nexts[1])) continue;
          candidates_queue.push(nexts[0]);
          position_table[1][nexts[0]] = nexts[1];
        }
      };

      delta_block_size += 1.0;
      for (int i = 0; i < 2; ++i) {
        const auto& pos = starts[i];;
        const auto [x, y, z] = pos;
        delta_unsatisfied -= ++silhouette_counts[i][z][0][x] == 1;
        delta_unsatisfied -= ++silhouette_counts[i][z][1][y] == 1;
        delta_overlap += overlap_counts[i][pos]++;
      }
      position_table[1][starts[0]] = starts[1];
      seen[starts[0]] = ++generation;
      add_neighbors(starts[0]);

      while (!candidates_queue.empty()) {
        const auto next = candidates_queue.front();
        candidates_queue.pop();
        const double d_block_size = 1.0 / (block_positions.size() + 1) - 1.0 / block_positions.size();
        double d_unsatisfied = 0.0;
        double d_overlap = 0.0;
        for (int i = 0; i < 2; ++i) {
          const auto& pos = position_table[i][next];
          const auto [x, y, z] = pos;
          d_unsatisfied -= silhouette_counts[i][z][0][x] == 0;
          d_unsatisfied -= silhouette_counts[i][z][1][y] == 0;
          d_overlap += overlap_counts[i][pos];
        }
        const auto delta = d_block_size + weight_unsatisfied * d_unsatisfied + weight_overlap * d_overlap;
        if (delta > 0.0) continue;
        for (int i = 0; i < 2; ++i) {
          const auto& pos = position_table[i][next];
          const auto [x, y, z] = pos;
          ++silhouette_counts[i][z][0][x];
          ++silhouette_counts[i][z][1][y];
          ++overlap_counts[i][pos];
        }
        delta_block_size += d_block_size;
        delta_unsatisfied += d_unsatisfied;
        delta_overlap += d_overlap;
        block_positions.push_back(next);
        add_neighbors(Position(next));
      }

      const auto block = __builtin_ctz(available_blocks);
      orientations[block][1] = orientation_index;
      available_blocks &= available_blocks - 1;
      added_block = block;
      used_blocks.push_back(block);
      for (int i = 0; i < 2; ++i) {
        for (const auto& p : block_positions) {
          const auto& pos = position_table[i][p];
          positions[block][i].push_back(pos);
        }
      }
    };

    if (available_blocks == 0 ||
        (!used_blocks.empty() && rng.Canonical() < 0.98)) {
      const int block_index = rng.Range(used_blocks.size());
      const int block = used_blocks[block_index];
      remove_block(block_index);
      const auto delta = delta_block_size + weight_unsatisfied * delta_unsatisfied + weight_overlap * delta_overlap;
      if (available_blocks != 0 && delta > 0 && rng.Canonical() < 0.95) {
        Position starts[2];
        int orientation_index = orientations[block][1];
        if (rng.Canonical() < 0.3) {
          const int cube_index = rng.Range(positions[block][0].size());
          for (int i = 0; i < 2; ++i) {
            starts[i] = Position(positions[block][i][cube_index]);
          }
        } else {
          orientation_index = rng.Range(24);
          for (int i = 0; i < 2; ++i) {
            const auto& candidates = positions[block][i];
            starts[i] = Position(candidates[rng.Range(candidates.size())]);
          }
        }
        add_block(starts, orientation_index);
      }
    } else {
        Position starts[2];
        for (int i = 0; i < 2; ++i) {
          const auto& candidates = position_candidates[i];
          starts[i] = Position(candidates[rng.Range(candidates.size())]);
        }
        add_block(starts, rng.Range(24));
    }

    sum_unsatisfied += delta_unsatisfied;
    sum_block_size += delta_block_size;
    sum_overlap += delta_overlap;
    const auto delta = delta_block_size + weight_unsatisfied * delta_unsatisfied + weight_overlap * delta_overlap;

    if (delta <= tolerance) {
      if (removed_block != -1) {
        positions[removed_block][0].clear();
        positions[removed_block][1].clear();
        available_blocks |= 1 << removed_block;
      }
      score = ComputeScore();
      best_score = std::max(best_score, score);
      return true;
    } else {
      sum_unsatisfied -= delta_unsatisfied;
      sum_block_size -= delta_block_size;
      sum_overlap -= delta_overlap;
      if (added_block != -1) {
        available_blocks |= 1 << added_block;
        while (!positions[added_block][0].empty()) {
          for (int i = 0; i < 2; ++i) {
            const auto& pos = positions[added_block][i].back();
            const auto [x, y, z] = Position(pos);
            --silhouette_counts[i][z][0][x];
            --silhouette_counts[i][z][1][y];
            --overlap_counts[i][pos];

            positions[added_block][i].pop_back();
          }
        }
        used_blocks.pop_back();
      }
      if (removed_block != -1) {
        used_blocks.push_back(removed_block);
        for (int i = 0; i < 2; ++i) {
          for (const auto& pos : positions[removed_block][i]) {
            const auto [x, y, z] = Position(pos);
            ++silhouette_counts[i][z][0][x];
            ++silhouette_counts[i][z][1][y];
            ++overlap_counts[i][pos];
          }
        }
      }
      return false;
    }
  }

  double ComputeScore() const {
    return -(sum_block_size + sum_unsatisfied * 10.0 + 100.0 * sum_overlap);
  }

  std::tuple<int, std::vector<std::vector<int>>> Construct() {
    const auto to_index = [D = toy_size](int x, int y, int z) {
      return D * D * (x - 1) + D * (y - 1) + (z - 1);
    };
    std::vector b(2, std::vector<int>(toy_size * toy_size * toy_size));
    std::vector<int> num_blocks(2);
    int N = 0;
    if (sum_overlap == 0) {
      for (const auto& block : used_blocks) {
        ++N;
        for (int i = 0; i < 2; ++i) {
          for (const auto& pos : positions[block][i]) {
            const auto [x, y, z] = Position(pos);
            b[i][to_index(x, y, z)] = N;
          }
        }
      }
    }
    for (int i = 0; i < 2; ++i) {
      for (int z = 1; z <= toy_size; ++z) {
        for (int x = 1; x <= toy_size; ++x) {
          if (!silhouettes[i][z][0].test(x)) continue;
          for (int y = 1; y <= toy_size; ++y) {
            if (!silhouettes[i][z][1].test(y)) continue;
            if (silhouette_counts[i][z][1][y] && silhouette_counts[i][z][0][x]) continue;
            b[i][to_index(x, y, z)] = ++N;
            ++silhouette_counts[i][z][0][x];
            ++silhouette_counts[i][z][1][y];
          }
        }
      }
    }
    return {N, b};
  }

  int toy_size;
  std::bitset<kGridSize> silhouettes[2][kGridSize][2];
  std::bitset<kNumCubes> masks[2];
  std::uint32_t available_blocks;
  StaticVector<std::uint16_t, kMaxNumBlocks> used_blocks;
  StaticVector<std::uint16_t, kNumCubes> positions[kMaxNumBlocks][2];
  std::uint16_t orientations[kNumCubes][2];
  std::uint16_t silhouette_counts[2][kGridSize][2][kGridSize];
  std::uint16_t overlap_counts[2][kNumCubes] = {};
  Position position_table[2][kNumCubes];
  StaticVector<std::uint16_t, kNumCubes> position_candidates[2];
  StaticVector<std::uint16_t, kNumCubes> block_positions;
  StaticQueue<std::uint16_t, kNumCubes> candidates_queue;
  int generation = 0;
  int seen[kNumCubes] = {};
  double sum_block_size;
  double sum_unsatisfied;
  double sum_overlap;
  double score;
  double best_score;
};

class Solver {
 public:
  std::tuple<int, std::vector<std::vector<int>>>
  Solve(int D, const std::vector<std::vector<std::string>>& silhouettes) {
    Timer timer;
    Random rng(1);
    const double T0 = 2.5e-1;
    const double T1 = 2.5e-2;
    const State initial_state(D, silhouettes);
    return Anneal(initial_state, rng, T0, T1, timer, kTimeLimit).Construct();
  }
};

int main() {
  int D;
  std::cin >> D;
  std::vector silhouettes(4, std::vector<std::string>(D));
  for (auto& silhouette : silhouettes) {
    for (auto& row : silhouette) {
      std::cin >> row;
    }
  }

  Solver solver;
  const auto [N, B] = solver.Solve(D, silhouettes);
  std::cout << N << std::endl;
  for (const auto& x : B) {
    for (int d = 0; d < D * D * D; ++d) {
      std::cout << x[d] << (d + 1 == D * D * D ? '\n' : ' ');
    }
  }

  return 0;
}
