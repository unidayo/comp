#pragma GCC optimize("Ofast")
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <chrono>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr double kTimeLimit = 2.95;
constexpr int kMaxGridSize = 20;
constexpr int kMaxNumCells = kMaxGridSize * kMaxGridSize;
constexpr int kMaxNumOilFields = 20;

constexpr int kNumArrangementCandidates = 1000;
constexpr int kTableSize = 192;

namespace Global {

alignas(64) float probabilities[kTableSize][kTableSize][kTableSize];
alignas(64) float log_likelihoods[kTableSize][kTableSize][kTableSize];

alignas(64) float expected_means[kTableSize][kTableSize];
alignas(64) float expected_vars[kTableSize][kTableSize];

std::uint8_t overlap_counts[2 * kMaxNumCells][kMaxNumOilFields][kMaxNumCells];

}

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
  constexpr explicit Random(std::uint64_t seed = 0) :
      state_(seed ^ 0x07a8ded236b339c8ull) {}

  constexpr double Canonical() {
    return (Next() >> 1) * 0x1p-63;
  }

  constexpr double Exponential2() {
    return -std::log2(Canonical());
  }

  constexpr std::uint32_t Range(std::uint32_t size) {
    return __uint128_t(Next()) * size >> 64;
  }

  constexpr auto PairInRange(std::uint32_t size) {
    const std::uint64_t r = Next();
    const std::uint32_t a = (r >> 32) * size >> 32;
    const std::uint32_t b = (r << 32 >> 32) * --size >> 32;
    return std::make_tuple(a, (b == a) ? size : b);
  }

  constexpr int MinMax(int min, int max) {
    return Range(max - min + 1) + min;
  }

  template <typename T>
  constexpr const T& Choose(const std::vector<T>& vector) {
    return vector[Range(vector.size())];
  }

  template <typename V>
  constexpr void Shuffle(V& vector) {
    for (auto i = vector.size() - 1; i > 0; --i) {
      std::swap(vector[i], vector[Range(i + 1)]);
    }
  }

 private:
  constexpr std::uint64_t Next() {
    const auto product = __uint128_t(state_ ^ 0xdbd2abd4066651c5ull) * state_;
    state_ += 0xdbd2abd4066651c5ull;
    return (product >> 64) ^ product;
  }

  std::uint64_t state_;
};

class Position {
 public:
  explicit Position(std::uint16_t index = 0) : index_(index) {}
  Position(std::uint8_t y, std::uint8_t x) : index_(kMaxGridSize * y + x) {}

  constexpr std::uint8_t X() const {
    return index_ % kMaxGridSize;
  }

  constexpr std::uint8_t Y() const {
    return index_ / kMaxGridSize;
  }

  template <std::size_t Index>
  constexpr std::uint8_t get() const {
    if constexpr (Index == 0) {
      return Y();
    } else {
      return X();
    }
  }

  operator std::uint16_t() const {
    return index_;
  }

 private:
  std::uint16_t index_;
};

template <>
struct std::tuple_size<Position> : std::integral_constant<std::size_t, 2> {};

template <std::size_t Index>
struct std::tuple_element<Index, Position> {
  using type = std::uint8_t;
};

struct Polyomino {
  std::vector<Position> squares;
  std::vector<Position> offsets;
  std::bitset<kMaxNumCells> available_offsets;
  std::bitset<kMaxNumCells> squares_bitset;
};

struct Divination {
  std::vector<Position> cells;
  const float* log_likelihoods;
  std::bitset<kMaxNumCells> cells_bitset;
};

struct Island {
  int size;
  double epsilon;
  std::vector<Polyomino> polyominoes;
};

class Judge {
 public:
  Judge() {
    int num_polyominoes;
    std::cin >> island_.size >> num_polyominoes >> island_.epsilon;
    num_available_operations_ = 2 * island_.size * island_.size;
    cost_ = 0.0;
    for (int i = 0; i < num_polyominoes; ++i) {
      int d;
      std::cin >> d;
      std::vector<Position> squares;
      std::bitset<kMaxNumCells> squares_bitset;
      int max_y = 0;
      int max_x = 0;
      for (int j = 0; j < d; ++j) {
        int y, x;
        std::cin >> y >> x;
        squares.emplace_back(y, x);
        squares_bitset.set(Position(y, x));
      }

      for (const auto& [y, x] : squares) {
        max_y = std::max<int>(max_y, y);
        max_x = std::max<int>(max_x, x);
      }
      std::vector<Position> offsets;
      std::bitset<kMaxNumCells> available_offsets;
      for (int y = 0; y + max_y < island_.size; ++y) {
        for (int x = 0; x + max_x < island_.size; ++x) {
          const Position pos(y, x);
          offsets.push_back(pos);
          available_offsets.set(pos);
        }
      }
      island_.polyominoes.emplace_back(squares, offsets, available_offsets, squares_bitset);
    }
  }

  Island& GetIsland() {
    return island_;
  }

  int GetNumAvailableOperations() const {
    return num_available_operations_;
  }

  int Divine(const std::vector<Position>& cells) {
    --num_available_operations_;
    cost_ += 1.0 / std::sqrt(cells.size());
    std::cout << "q " << cells.size();
    for (const auto& [y, x] : cells) {
      std::cout << ' ' << static_cast<int>(y) << ' ' << static_cast<int>(x);
    }
    std::cout << std::endl;
    int value;
    std::cin >> value;
    return value;
  }

  bool Guess(const std::vector<Position>& cells) {
    --num_available_operations_;
    std::cout << "a " << cells.size();
    for (const auto& [y, x] : cells) {
      std::cout << ' ' << static_cast<int>(y) << ' ' << static_cast<int>(x);
    }
    std::cout << std::endl;
    int correct;
    std::cin >> correct;
    if (correct) {
      return true;
    }
    cost_ += 1.0;
    return false;
  }

 private:
  Island island_;
  int num_available_operations_;
  double cost_;
};

void ComputeProbabilityTable(int max_num_cells, double epsilon) {
  constexpr float kMinLogLikelihood = -1000.0;

  for (int sum = 0; sum < kTableSize; ++sum) {
    for (int value = 0; value < kTableSize; ++value) {
      Global::probabilities[1][sum][value] = 0.0;
      Global::log_likelihoods[1][value][sum] = kMinLogLikelihood;
    }
    Global::probabilities[1][sum][sum] = 1.0;
    Global::log_likelihoods[1][sum][sum] = 0.0;
  }
  for (int num_cells = 2; num_cells <= std::min(max_num_cells, kTableSize - 1); ++num_cells) {
    const double variance = num_cells * epsilon * (1.0 - epsilon);
    const double scale = 1.0 / std::sqrt(2.0 * variance);
    for (int sum = 0; sum < kTableSize; ++sum) {
      const double mean = (num_cells - sum) * epsilon + sum * (1.0 - epsilon);
      for (int value = 0; value < kTableSize; ++value) {
        Global::probabilities[num_cells][sum][value] = -(value + 0.5 - mean) * scale;
      }
      for (int value = 0; value < kTableSize; ++value) {
        Global::probabilities[num_cells][sum][value] = 0.5 * (1.0 - std::erf(Global::probabilities[num_cells][sum][value]));
      }
      Global::probabilities[num_cells][sum][kTableSize - 1] = 1.0;
      for (int value = kTableSize - 1; value > 0; --value) {
        Global::probabilities[num_cells][sum][value] -= Global::probabilities[num_cells][sum][value - 1];
      }
      for (int value = 0; value < kTableSize; ++value) {
        Global::log_likelihoods[num_cells][value][sum] = std::max(kMinLogLikelihood, std::log(Global::probabilities[num_cells][sum][value]));
      }
    }
  }

  for (int num_cells = 1; num_cells <= std::min(max_num_cells, kTableSize - 1); ++num_cells) {
    for (int value = 0; value < kTableSize; ++value) {
      double sum_likelihoods = 0.0;
      double sum_log_likelihoods = 0.0;
      for (int sum = 0; sum < kTableSize; ++sum) {
        const auto likelihood = Global::probabilities[num_cells][sum][value];
        sum_likelihoods += likelihood;
        sum_log_likelihoods += likelihood * Global::log_likelihoods[num_cells][value][sum];
      }
      Global::expected_means[num_cells][value] = sum_log_likelihoods / sum_likelihoods;

      double sum_squared_differences = 0.0;
      for (int sum = 0; sum < kTableSize; ++sum) {
        const auto delta = Global::log_likelihoods[num_cells][value][sum] - Global::expected_means[num_cells][value];
        const auto likelihood = Global::probabilities[num_cells][sum][value];
        sum_squared_differences += likelihood * delta * delta;
      }
      Global::expected_vars[num_cells][value] = sum_squared_differences / sum_likelihoods;
    }
  }
}

struct Arrangement {
  bool operator<(const Arrangement& other) const {
    return log_likelihood > other.log_likelihood;
  }

  std::bitset<kMaxNumCells> oily_cells;
  std::vector<Position> offsets;
  std::vector<int> oil_reserves;
  double log_likelihood;
  double scaled_likelihood;
};

struct ArrangementCandidates {
  void AddCandidate(const std::bitset<kMaxNumCells>& oily_cells, const std::vector<Position>& offsets, const std::vector<int>& oil_reserves, double log_likelihood) {
    if (best_log_likelihoods.size() == kNumArrangementCandidates && log_likelihood <= queue.top().log_likelihood) return;
    if (incorrect.contains(oily_cells)) return;
    if (const auto iter = best_log_likelihoods.find(oily_cells); iter != best_log_likelihoods.end()) {
      if (log_likelihood <= iter->second) return;
      iter->second = log_likelihood;
      queue.emplace(oily_cells, offsets, oil_reserves, log_likelihood);
      return;
    }
    while (best_log_likelihoods.size() >= kNumArrangementCandidates) {
      const auto iter = best_log_likelihoods.find(queue.top().oily_cells);
      if (queue.top().log_likelihood == iter->second) {
        best_log_likelihoods.erase(iter);
      }
      queue.pop();
    }
    best_log_likelihoods[oily_cells] = log_likelihood;
    queue.emplace(oily_cells, offsets, oil_reserves, log_likelihood);
  }

  std::vector<Arrangement> Withdraw() {
    std::vector<Arrangement> arrangements;
    while (!queue.empty()) {
      const auto iter = best_log_likelihoods.find(queue.top().oily_cells);
      if (queue.top().log_likelihood == iter->second) {
        best_log_likelihoods.erase(iter);
        arrangements.push_back(queue.top());
      }
      queue.pop();
    }
    std::ranges::reverse(arrangements);
    return arrangements;
  }

  void AddIncorrect(const std::bitset<kMaxNumCells>& oily_cells) {
    incorrect.insert(oily_cells);
  }

  std::priority_queue<Arrangement> queue;
  std::unordered_map<std::bitset<kMaxNumCells>, double> best_log_likelihoods;
  std::unordered_set<std::bitset<kMaxNumCells>> incorrect;
};

float ApproxPow(float base, float exponent) {
  constexpr int kOffset = (127 << 23) - 486408;
  return std::bit_cast<float>(static_cast<int>(exponent * (std::bit_cast<int>(base) - kOffset)) + kOffset);
}

template <typename State>
void Anneal(std::vector<State>& states, Random& rng, double T0, double T1, int num_iters) {
  const int num_levels = 32 - __builtin_clz(states.size());
  num_iters /= num_levels;
  std::vector<int> indices;
  for (std::size_t i = 0; i < states.size(); ++i) {
    indices.push_back(i);
  }
  auto temperature = T0;
  const float base = T1 / T0;
  const auto progress_scale = 1.0 / (num_iters * num_levels - 1);
  double progress = 0.0;
  for (int level = 0; level < num_levels; ++level) {
    const int num_iters_in_level = num_iters / indices.size();
    for (int i = 0; i < num_iters_in_level; ++i) {
      if ((i & ((1 << 4) - 1)) == 0) {
        progress = (num_iters * level + indices.size() * i) * progress_scale;
        temperature = T0 * ApproxPow(base, progress);
      }
      for (const auto& index : indices) {
        states[index].TryTransition(temperature, rng);
      }
    }
    std::ranges::sort(indices, std::greater{}, [&](int index) {
      return states[index].best_score;
    });
    const auto size = indices.size() >> 1;
    indices.erase(indices.begin() + size, indices.end());
  }
}

struct ArrangementState {
  ArrangementState(const std::vector<Polyomino>& polyominoes_, const std::vector<Divination>& divinations_, ArrangementCandidates& arrangement_candidates_) :
    polyominoes(polyominoes_), divinations(divinations_), oil_reserves(kMaxNumCells), sums(divinations.size()), next_sums(divinations.size()), arrangement_candidates(arrangement_candidates_) {
  }

  void Initialize(const std::vector<Position>& offsets_) {
    offsets = offsets_;
    Initialize();
  }

  void Initialize(Random& rng) {
    offsets.clear();
    for (const auto& polyomino : polyominoes) {
      const auto offset = rng.Choose(polyomino.offsets);
      offsets.push_back(offset);
    }
    Initialize();
  }

  void Initialize() {
    oily_cells.reset();
    std::ranges::fill(oil_reserves, 0);
    for (std::size_t i = 0; i < polyominoes.size(); ++i) {
      for (const auto& square : polyominoes[i].squares) {
        ++oil_reserves[offsets[i] + square];
        oily_cells.set(offsets[i] + square);
      }
    }
    for (std::size_t i = 0; i < divinations.size(); ++i) {
      sums[i] = 0;
      for (const auto& cell : divinations[i].cells) {
        sums[i] += oil_reserves[cell];
      }
    }
    best_score = score = ComputeScore();
    stuck_count = 0;
    arrangement_candidates.AddCandidate(oily_cells, offsets, oil_reserves, score);
  }

  bool TryTransition(double temperature, Random& rng) {
    move.clear();
    do {
      if (rng.Canonical() < 0.5) {
        const int polyomino_index = rng.Range(polyominoes.size());
        const auto offset = rng.Choose(polyominoes[polyomino_index].offsets);
        if (offset == offsets[polyomino_index]) continue;
        move.emplace_back(polyomino_index, offsets[polyomino_index], offset);
      } else {
        const auto [a, b] = rng.PairInRange(polyominoes.size());
        if (offsets[a] == offsets[b]) continue;
        Position offset_a = offsets[a];
        Position offset_b = offsets[b];
        constexpr int kMaxShift = 2;
        constexpr double kShiftProbability = 0.5;
        if (rng.Canonical() < kShiftProbability) {
          const int dy = rng.MinMax(-kMaxShift, kMaxShift);
          const int dx = rng.MinMax(-kMaxShift, kMaxShift);
          offset_a = Position(offset_a + Position(dy, dx));
        }
        if (rng.Canonical() < kShiftProbability) {
          const int dy = rng.MinMax(-kMaxShift, kMaxShift);
          const int dx = rng.MinMax(-kMaxShift, kMaxShift);
          offset_b = Position(offset_b + Position(dy, dx));
        }
        if (offset_b >= kMaxNumCells || !polyominoes[a].available_offsets.test(offset_b)) continue;
        if (offset_a >= kMaxNumCells || !polyominoes[b].available_offsets.test(offset_a)) continue;
        move.emplace_back(a, offsets[a], offset_b);
        move.emplace_back(b, offsets[b], offset_a);
      }
    } while (move.empty());

    double delta = 0.0;
    next_sums = sums;
    for (std::size_t i = 0; i < divinations.size(); ++i) {
      for (const auto& [polyomino_index, from, to] : move) {
        next_sums[i] += Global::overlap_counts[i][polyomino_index][to] - Global::overlap_counts[i][polyomino_index][from];
      }
      if (sums[i] == next_sums[i]) continue;
      delta -= divinations[i].log_likelihoods[std::min(kTableSize - 1, sums[i])];
      delta += divinations[i].log_likelihoods[std::min(kTableSize - 1, next_sums[i])];
    }

    const auto tolerance = temperature * rng.Exponential2();
    if (++stuck_count > 1000 || delta >= -tolerance) {
      stuck_count = 0;
      score += delta;
      sums = next_sums;
      for (const auto& [polyomino_index, from, to] : move) {
        for (const auto& square : polyominoes[polyomino_index].squares) {
          if (--oil_reserves[from + square] == 0) {
            oily_cells.reset(from + square);
          }
          if (++oil_reserves[to + square] == 1) {
            oily_cells.set(to + square);
          }
        }
        offsets[polyomino_index] = to;
      }
      arrangement_candidates.AddCandidate(oily_cells, offsets, oil_reserves, score);
      best_score = std::max(best_score, score);
      return true;
    } else {
      return false;
    }
  }

  double ComputeScore() const {
    double log_likelihood = 0.0;
    for (const auto& divination : divinations) {
      int sum = 0;
      for (const auto& cell : divination.cells) {
        sum += oil_reserves[cell];
      }
      log_likelihood += divination.log_likelihoods[std::min(kTableSize - 1, sum)];
    }
    return log_likelihood;
  }

  const std::vector<Polyomino>& polyominoes;
  const std::vector<Divination>& divinations;
  double score;
  double best_score;
  std::vector<Position> offsets;
  std::vector<int> oil_reserves;
  std::vector<int> sums;
  std::vector<int> next_sums;
  std::vector<std::tuple<int, Position, Position>> move;
  int stuck_count;
  std::bitset<kMaxNumCells> oily_cells;
  ArrangementCandidates& arrangement_candidates;
};

struct DivinationState {
  DivinationState(const std::vector<Polyomino>& polyominoes_, const std::vector<Arrangement>& arrangements_, int size, Random& rng) :
    polyominoes(polyominoes_), arrangements(arrangements_),
    oil_reserves(arrangements.size()) {
    for (int i = 0; i < kTableSize; ++i) {
      const int y = rng.Range(size);
      const int x = rng.Range(size);
      targets.flip(Position(y, x));
    }
    best_targets = targets;
    for (int i = 0; i < kMaxNumCells; ++i) {
      if (!targets.test(i)) continue;
      for (std::size_t j = 0; j < arrangements.size(); ++j) {
        oil_reserves[j] += arrangements[j].oil_reserves[i];
      }
    }
    best_score = score = ComputeScore();

    shuffled_positions.clear();
    shuffled_position_index = 0;
    for (int y = 0; y < size; ++y) {
      for (int x = 0; x < size; ++x) {
        shuffled_positions.emplace_back(y, x);
      }
    }
    rng.Shuffle(shuffled_positions);
  }

  bool TryTransition(double temperature, Random& rng) {
    const Position target = shuffled_positions[shuffled_position_index];
    if (++shuffled_position_index == shuffled_positions.size()) {
      shuffled_position_index = 0;
    }
    targets.flip(target);
    const int sign = targets.test(target) ? 1 : -1;
    for (std::size_t i = 0; i < arrangements.size(); ++i) {
      oil_reserves[i] += sign * arrangements[i].oil_reserves[target];
    }
    const auto next_score = ComputeScore();
    const auto tolerance = temperature * rng.Exponential2();
    if (next_score >= score - tolerance) {
      score = next_score;
      if (score > best_score) {
        best_score = score;
        best_targets = targets;
      }
      return true;
    } else {
      targets.flip(target);
      for (std::size_t i = 0; i < arrangements.size(); ++i) {
        oil_reserves[i] -= sign * arrangements[i].oil_reserves[target];
      }
      return false;
    }
  }

  double ComputeScore() {
    const int num_cells = targets.count();
    if (num_cells >= kTableSize) return 0.0;

    double sum_prior = 0.0;
    for (int i = 0; i < kNumArrangements; ++i) {
      prior_probabilities[i] = arrangements[i].scaled_likelihood;
      sum_prior += prior_probabilities[i];
    }
    for (int i = 0; i < kNumArrangements; ++i) {
      prior_probabilities[i] /= sum_prior;
    }

    std::ranges::fill(output_probabilities, 0.0);
    for (int i = 0; i < kNumArrangements; ++i) {
      for (int j = 0; j < kTableSize; ++j) {
        output_probabilities[j] += prior_probabilities[i] * Global::probabilities[num_cells][oil_reserves[i]][j];
      }
      log_prior_probabilities[i] = std::log(prior_probabilities[i]);
    }

    double gain = 0.0;
    for (int j = 0; j < kTableSize; ++j) {
      if (output_probabilities[j] < 1.0e-6) continue;
      auto max_log_likelihood = -std::numeric_limits<float>::infinity();
      for (int i = 0; i < kNumArrangements; ++i) {
        log_posterior_probabilities[i] = arrangements[i].log_likelihood + Global::log_likelihoods[std::min(kTableSize - 1, num_cells)][j][std::min(kTableSize - 1, oil_reserves[i])];
        max_log_likelihood = std::max(max_log_likelihood, log_posterior_probabilities[i]);
      }
      double sum_posterior = 0.0;
      for (int i = 0; i < kNumArrangements; ++i) {
        posterior_probabilities[i] = std::exp(log_posterior_probabilities[i] -= max_log_likelihood);
        sum_posterior += posterior_probabilities[i];
      }
      if (!std::isfinite(sum_posterior)) continue;
      const auto offset = std::log(sum_posterior);
      for (int i = 0; i < kNumArrangements; ++i) {
        posterior_probabilities[i] /= sum_posterior;
        log_posterior_probabilities[i] -= offset;
      }
      double sum = 0.0;
      for (int i = 0; i < kNumArrangements; ++i) {
        if (posterior_probabilities[i] < 1.0e-6) continue;
        sum += posterior_probabilities[i] * (log_posterior_probabilities[i] - log_prior_probabilities[i]);
      }
      gain += output_probabilities[j] * std::max(0.0, sum);
    }

    return gain * std::pow(num_cells, 0.3);
  }

  auto GetResult() const {
    return best_targets;
  }

  const std::vector<Polyomino>& polyominoes;
  const std::vector<Arrangement>& arrangements;
  double score;
  double best_score;
  std::bitset<kMaxNumCells> targets;
  std::bitset<kMaxNumCells> best_targets;
  std::vector<int> oil_reserves;
  std::size_t shuffled_position_index;
  std::vector<Position> shuffled_positions;

  static constexpr int kNumArrangements = 16;
  alignas(64) double output_probabilities[kTableSize];
  alignas(64) float log_prior_probabilities[kNumArrangements];
  alignas(64) float prior_probabilities[kNumArrangements];
  alignas(64) float log_posterior_probabilities[kNumArrangements];
  alignas(64) float posterior_probabilities[kNumArrangements];
};

void Solve(Judge& judge, Timer& timer) {
  Random rng;
  const Island& island = judge.GetIsland();
  ComputeProbabilityTable(island.size * island.size, island.epsilon);

  std::vector<Arrangement> arrangements;
  std::vector<Arrangement> incorrect_arrangements;
  for (int i = 0; i < kNumArrangementCandidates; ++i) {
    std::vector<Position> offsets;
    for (const auto& polyomino : island.polyominoes) {
      const auto offset = rng.Choose(polyomino.offsets);
      offsets.push_back(offset);
    }
    std::bitset<kMaxNumCells> oily_cells;
    std::vector<int> oil_reserves(kMaxNumCells);
    for (std::size_t j = 0; j < island.polyominoes.size(); ++j) {
      for (const auto& square : island.polyominoes[j].squares) {
        ++oil_reserves[offsets[j] + square];
        oily_cells.set(offsets[j] + square);
      }
    }
    arrangements.emplace_back(oily_cells, offsets, oil_reserves, 0.0);
  }

  std::vector<Divination> divinations;
  ArrangementCandidates arrangement_candidates;
  double expected_mean = 0.0;
  double expected_var = 0.0;

  while (judge.GetNumAvailableOperations() > 0) {
    const double progress = timer.GetTime() / kTimeLimit;
    if (progress > 1.0) {
      for (const auto& arrangement : arrangements) {
        std::vector<Position> cells;
        for (int pos = 0; pos < kMaxNumCells; ++pos) {
          if (!arrangement.oily_cells.test(pos)) continue;
          cells.push_back(Position(pos));
        }
        if (judge.Guess(cells)) break;
        if (judge.GetNumAvailableOperations() == 0) break;
      }
      break;
    }

    const auto log_likelihood_threshold = expected_mean - 3.0 * std::sqrt(expected_var);
    if (judge.GetNumAvailableOperations() == 1 ||
        arrangements[0].log_likelihood > std::max(arrangements[1].log_likelihood + 5.0, log_likelihood_threshold)) {
      std::vector<Position> cells;
      for (int i = 0; i < kMaxNumCells; ++i) {
        if (!arrangements[0].oily_cells.test(i)) continue;
        cells.emplace_back(i);
      }
      if (judge.Guess(cells)) break;
      arrangement_candidates.AddIncorrect(arrangements[0].oily_cells);
      incorrect_arrangements.push_back(arrangements[0]);
      arrangements.erase(arrangements.begin());
    }
    if (judge.GetNumAvailableOperations() <= 1) continue;

    for (auto& arrangement : arrangements) {
      arrangement.scaled_likelihood = std::exp(arrangement.log_likelihood - arrangements[0].log_likelihood);
    }

    std::vector divination_states(1, DivinationState(island.polyominoes, arrangements, island.size, rng));
    Anneal(divination_states, rng, 1.0e-2, 1.0e-5, 1000);
    std::bitset<kMaxNumCells> cells_bitset = divination_states[0].GetResult();
    std::vector<Position> cells;
    for (int i = 0; i < kMaxNumCells; ++i) {
      if (!cells_bitset.test(i)) continue;
      cells.emplace_back(i);
    }

    for (std::size_t polyomino_index = 0; polyomino_index < island.polyominoes.size(); ++polyomino_index) {
      for (const auto& offset : island.polyominoes[polyomino_index].offsets) {
        const int count = ((island.polyominoes[polyomino_index].squares_bitset << offset) & cells_bitset).count();
        Global::overlap_counts[divinations.size()][polyomino_index][offset] = std::min(count, 255);
      }
    }

    const int num_cells = std::min<int>(kTableSize - 1, cells.size());
    const int value = std::min(kTableSize - 1, judge.Divine(cells));
    divinations.emplace_back(cells, &Global::log_likelihoods[num_cells][value][0], cells_bitset);
    expected_mean += Global::expected_means[num_cells][value];
    expected_var += Global::expected_vars[num_cells][value];

    for (auto& arrangement : arrangements) {
      int sum = 0;
      for (const auto& cell : cells) {
        sum += arrangement.oil_reserves[cell];
      }
      arrangement.log_likelihood += Global::log_likelihoods[num_cells][value][std::min(kTableSize - 1, sum)];
      arrangement_candidates.AddCandidate(arrangement.oily_cells, arrangement.offsets, arrangement.oil_reserves, arrangement.log_likelihood);
    }
    for (auto& arrangement : incorrect_arrangements) {
      int sum = 0;
      for (const auto& cell : cells) {
        sum += arrangement.oil_reserves[cell];
      }
      arrangement.log_likelihood += Global::log_likelihoods[num_cells][value][std::min(kTableSize - 1, sum)];
    }

    auto sorted_arrangements = arrangements;
    sorted_arrangements.insert(sorted_arrangements.end(), incorrect_arrangements.begin(), incorrect_arrangements.end());
    std::sort(sorted_arrangements.begin(), sorted_arrangements.end());
    std::vector arrangement_states(64, ArrangementState(island.polyominoes, divinations, arrangement_candidates));
    for (std::size_t i = 0; i < arrangement_states.size(); ++i) {
      if (i < std::min<std::size_t>(50, sorted_arrangements.size())) {
        arrangement_states[i].Initialize(sorted_arrangements[i].offsets);
      } else {
        arrangement_states[i].Initialize(rng);
      }
    }
    const int num_iters = (progress < 0.5 && island.polyominoes.size() <= 6) ? 100000 : 10000;
    Anneal(arrangement_states, rng, 1.0e1, 1.0e-1, num_iters);
    arrangements = arrangement_candidates.Withdraw();
  }
}

void SolveNaive(Judge& judge) {
  Island& island = judge.GetIsland();
  int sum = 0;
  for (const auto& polyomino : island.polyominoes) {
    sum += polyomino.squares.size();
  }

  std::vector<Position> cells;
  for (int i = 0; i < island.size && sum > 0; ++i) {
    for (int j = 0; j < island.size && sum > 0; ++j) {
      if (const int value = judge.Divine({Position(i, j)})) {
        cells.emplace_back(i, j);
        sum -= value;
      }
    }
  }
  judge.Guess(cells);
}

int main() {
  Timer timer;
  Judge judge;
  if (judge.GetIsland().polyominoes.size() > 12) {
    SolveNaive(judge);
  } else {
    Solve(judge, timer);
  }

  return 0;
}
