#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <chrono>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr double kTimeLimit = 1.95;
constexpr int kNumColors = 100;
constexpr int kGridSize = 50;
constexpr int kPaddedGridSize = kGridSize + 2;
constexpr int kNumCells = kPaddedGridSize * kPaddedGridSize;
constexpr int kDeltas[] = {-kPaddedGridSize, -1, 1, kPaddedGridSize};
constexpr int kNeighbors[] = {-kPaddedGridSize - 1, -kPaddedGridSize, -kPaddedGridSize + 1, 1, kPaddedGridSize + 1, kPaddedGridSize, kPaddedGridSize - 1, -1};

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
  constexpr explicit Random(std::uint64_t seed) {
    state_[0] = seed;
    state_[1] = std::rotl(seed, 11);
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
    const auto result = std::rotl(s0 * 5, 7) * 9;
    s1 ^= s0;
    state_[0] = std::rotl(s0, 24) ^ s1 ^ (s1 << 16);
    state_[1] = std::rotl(s1, 37);
    return result;
  }

  std::uint64_t state_[2] = {};
};

struct IndexSet {
  IndexSet(int N) : indices(N, -1) {
    values.reserve(N);
  }

  bool Empty() const {
    return values.empty();
  }

  int Size() const {
    return values.size();
  }

  bool Contains(int value) const {
    return indices[value] >= 0;
  }

  int Choose(Random& random) const {
    return values[random.Range(values.size())];
  }

  void Add(int value) {
    indices[value] = values.size();
    values.push_back(value);
  }

  void Erase(int value) {
    indices[values.back()] = indices[value];
    std::swap(values[indices[value]], values.back());
    values.pop_back();
    indices[value] = -1;
  }

  void Clear() {
    while (!values.empty()) {
      indices[values.back()] = -1;
      values.pop_back();
    }
  }

  std::vector<int> values;
  std::vector<int> indices;
};

class DisjointSets {
 public:
  DisjointSets(int size) : data_(size, -1) {}

  void Reset() {
    std::fill(data_.begin(), data_.end(), -1);
  }

  void Merge(int a, int b) {
    if ((a = Find(a)) != (b = Find(b))) {
      if (data_[a] > data_[b]) std::swap(a, b);
      data_[a] += data_[b];
      data_[b] = a;
    }
  }

  int Find(int a) {
    if (data_[a] < 0) return a;
    return data_[a] = Find(data_[a]);
  }

  bool IsLeader(int a) const {
    return data_[a] < 0;
  }

  int GetSize(int a) {
    return -data_[Find(a)];
  }

 private:
  std::vector<int> data_;
};

float ApproxPow(float base, float exponent) {
  constexpr int kOffset = (127 << 23) - 486408;
  return std::bit_cast<float>(static_cast<int>(exponent * (std::bit_cast<int>(base) - kOffset)) + kOffset);
}

template <typename State>
auto Anneal(State state, Random& rng, double T0, double T1,
            Timer& timer, double time_limit) {
  auto T = T0;
  const float base = T1 / T0;
  const auto start = timer.GetTime();
  const auto progress_scale = 1.0 / (time_limit - start);
  double progress = 0.0;
  for (int i = 0; ; ++i) {
    if ((i & ((1 << 4) - 1)) == 0) {
      progress = (timer.GetTime() - start) * progress_scale;
      if (progress >= 1.0) break;
      T = T0 * ApproxPow(base, progress);
    }
    const auto tolerance = -T * std::log2(rng.Canonical());
    state.TryTransition(progress, tolerance, rng);
  }
  return state.GetResult();
}

struct AnnealingState {
  AnnealingState(const std::vector<std::vector<int>>& c) : cells(kNumCells), dsu(kNumCells) {
    std::ranges::fill(colors, 0);
    for (int i = 0; i < kGridSize; ++i) {
      for (int j = 0; j < kGridSize; ++j) {
        const int index = kPaddedGridSize * (i + 1) + j + 1;
        colors[index] = c[i][j];
        cells.Add(index);
      }
    }
    best_score = score = 1;
    Compress();
  }

  void Initialize() {
    for (int i = 0; i <= kNumColors; ++i) {
      std::ranges::fill(adjacent_counts[i], 0);
    }
    for (int i = kPaddedGridSize; i < kNumCells; ++i) {
      for (int d = 0; d < 2; ++d) {
        const auto [a, b] = std::minmax(colors[i], colors[i + kDeltas[d]]);
        if (a == b) continue;
        ++adjacent_counts[a][b];
      }
    }
    for (int i = 0; i < kNumCells; ++i) {
      std::ranges::fill(surroundings[i], 0);
      for (int j = 0; j < 8; ++j) {
        const int next = i + kNeighbors[j];
        if (next < 0 || next >= kNumCells) continue;
        surroundings[i][colors[next]] |= 1 << j;
      }
    }
    score = 1;
    for (int i = 1; i <= kGridSize; ++i) {
      for (int j = 1; j <= kGridSize; ++j) {
        const int index = kPaddedGridSize * i + j;
        score += colors[index] == 0;
      }
    }
    if (score > best_score) {
      best_score = score;
      best_colors = colors;
    }
  }

  auto FindEdges(const std::array<int, kNumCells>& map) {
    std::bitset<(kNumColors + 1) * kNumColors> edges;
    for (int i = kPaddedGridSize; i < kNumCells; ++i) {
      for (int d = 0; d < 2; ++d) {
        const auto [a, b] = std::minmax(map[i], map[i + kDeltas[d]]);
        if (a == b) continue;
        edges.set((kNumColors + 1) * a + b);
      }
    }
    return edges;
  }


  bool IsConnected(const std::array<int, kNumCells>& map) {
    dsu.Reset();
    for (int i = 0; i < kNumCells; ++i) {
      for (int d = 0; d < 2; ++d) {
        const int next = i + kDeltas[d];
        if (next < 0 || map[next] != map[i]) continue;
        dsu.Merge(i, next);
      }
    }
    int num_components = 0;
    for (int i = 0; i < kNumCells; ++i) {
      num_components += dsu.IsLeader(i);
    }
    return num_components == kNumColors + 1;
  }

  void Compress() {
    const auto edges = FindEdges(colors);
    for (int row = 1; row <= kGridSize; ++row) {
      bool has_color = false;
      for (int j = 1; j <= kGridSize; ++j) {
        const int index = kPaddedGridSize * row + j;
        if (colors[index] == 0) continue;
        has_color = true;
        break;
      }
      if (!has_color) continue;
      auto updated_colors = colors;
      for (int i = row; i <= kGridSize; ++i) {
        for (int j = 1; j <= kGridSize; ++j) {
          const int index = kPaddedGridSize * i + j;
          updated_colors[index] = colors[index + kPaddedGridSize];
        }
      }
      if (FindEdges(updated_colors) == edges && IsConnected(updated_colors)) {
        colors = updated_colors;
        --row;
      }
    }
    for (int column = 1; column <= kGridSize; ++column) {
      bool has_color = false;
      for (int i = 1; i <= kGridSize; ++i) {
        const int index = kPaddedGridSize * i + column;
        if (colors[index] == 0) continue;
        has_color = true;
        break;
      }
      if (!has_color) continue;
      auto updated_colors = colors;
      for (int i = 1; i <= kGridSize; ++i) {
        for (int j = column; j <= kGridSize; ++j) {
          const int index = kPaddedGridSize * i + j;
          updated_colors[index] = colors[index + 1];
        }
      }
      if (FindEdges(updated_colors) == edges && IsConnected(updated_colors)) {
        colors = updated_colors;
        --column;
      }
    }
    Initialize();
  }

  bool ChangeColor(int cell, int color) {
    bool feasible = true;
    for (const auto& delta : kDeltas) {
      const auto next = cell + delta;
      if (colors[next] == color) continue;
      const auto [a, b] = std::minmax(colors[next], color);
      if (adjacent_counts[a][b]++ == 0) {
        feasible = false;
      }
    }
    for (const auto& delta : kDeltas) {
      const auto next = cell + delta;
      if (colors[next] == colors[cell]) continue;
      const auto [a, b] = std::minmax(colors[next], colors[cell]);
      if (--adjacent_counts[a][b] == 0) {
        feasible = false;
      }
    }
    colors[cell] = color;
    return feasible;
  }

  void TryTransition(double progress, double tolerance, Random& rng) {
    constexpr double period = 0.0005;
    if (progress > last_compressed + period) {
      Compress();
      last_compressed += period;
      return;
    }

    int cell = -1;
    int color = -1;
    while (true) {
      cell = cells.Choose(rng);
      color = colors[cell + kDeltas[rng.Range(4)]];
      if (color == colors[cell]) continue;
      const std::uint8_t s = surroundings[cell][colors[cell]];
      if (std::popcount<std::uint8_t>(std::rotl(s, 1) ^ s) != 2) continue;
      break;
    }
    const int original_color = colors[cell];

    if (!ChangeColor(cell, color)) {
      ChangeColor(cell, original_color);
      return;
    }

    const int delta = (color == 0) - (original_color == 0);

    if (delta >= -tolerance) {
      for (int j = 0; j < 8; ++j) {
        const int next = cell + kNeighbors[j ^ 4];
        surroundings[next][color] ^= 1 << j;
        surroundings[next][original_color] ^= 1 << j;
      }

      score += delta;
      if (score > best_score) {
        best_score = score;
        best_colors = colors;
      }
    } else {
      ChangeColor(cell, original_color);
    }
  }

  auto GetResult() {
    colors = best_colors;
    Compress();
    return colors;
  }

  int score;
  int best_score;

  std::array<int, kNumCells> colors;
  std::array<int, kNumCells> best_colors;
  std::array<std::array<int, kNumColors + 1>, kNumColors + 1> adjacent_counts;
  std::array<std::array<std::uint8_t, kNumColors + 1>, kNumCells> surroundings;
  IndexSet cells;
  DisjointSets dsu;
  double last_compressed = 0.0;
};

int main() {
  int n, m;
  std::cin >> n >> m;
  std::vector c(n, std::vector<int>(n));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      std::cin >> c[i][j];
    }
  }

  Timer timer;
  Random rng(299792458);
  const double T0 = 1.0e1;
  const double T1 = 1.0e-1;
  AnnealingState state(c);
  const auto result = Anneal(state, rng, T0, T1, timer, kTimeLimit);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      const int index = kPaddedGridSize * (i + 1) + j + 1;
      std::cout << result[index] << (j + 1 == n ? '\n' : ' ');
    }
  }

  return 0;
}
