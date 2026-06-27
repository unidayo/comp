#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <tuple>
#include <utility>
#include <vector>
#include <cmath>
#include <cstdint>
#include <atcoder/maxflow>

constexpr int kMaxTemperature = 1'000;
constexpr int kMaxNumMeasurements = 10'000;

std::vector<std::vector<double>> ComputeLogLikelihoods(double sigma) {
  std::vector<double> cdf(kMaxTemperature + 1);
  for (int i = 0; i <= kMaxTemperature; ++i) {
    cdf[i] = 0.5 * std::erfc((i - 0.5) / (std::sqrt(2.0) * sigma));
  }
  std::vector log_likelihoods(kMaxTemperature + 1, std::vector<double>(kMaxTemperature + 1));
  for (int i = 0; i <= kMaxTemperature; ++i) {
    log_likelihoods[i][0] = std::log(cdf[i]);
    log_likelihoods[i][kMaxTemperature] = std::log(cdf[kMaxTemperature - i]);
    for (int j = 1; j < kMaxTemperature; ++j) {
      const int delta = std::abs(j - i);
      log_likelihoods[i][j] = std::log(cdf[delta] - cdf[delta + 1]);
    }
    for (int j = 0; j <= kMaxTemperature; ++j) {
      log_likelihoods[i][j] = std::max(-100.0, log_likelihoods[i][j]);
    }
  }
  return log_likelihoods;
}

std::vector<double> ComputeExpectedEntropies(const std::vector<std::vector<double>>& log_likelihoods) {
  std::vector<double> entropies(kMaxTemperature + 1);
  for (int delta = 1; delta <= kMaxTemperature; ++delta) {
    const int low = (kMaxTemperature - delta) / 2;
    const int high = low + delta;
    for (int i = 0; i <= kMaxTemperature; ++i) {
      const double p0 = std::exp(log_likelihoods[low][i]);
      const double p1 = std::exp(log_likelihoods[high][i]);
      if (p0 == p1 || p0 + p1 < 1.0e-10) continue;
      entropies[delta] += 0.5 * (p0 + p1) * std::min(8.0, std::abs(log_likelihoods[low][i] - log_likelihoods[high][i]));
    }
    entropies[delta] = std::min(100.0, entropies[delta]);
  }
  return entropies;
}

class Solver {
 public:
  Solver(int L, int S, const std::vector<std::pair<int, int>>& exits) :
      grid_size_(L), num_exits_(exits.size()), stddev_(S), exits_(exits),
      assigned_(num_exits_), penalties_(num_exits_, std::vector<int>(num_exits_)),
      penalty_table_(kMaxTemperature + 1, std::vector<int>(kMaxTemperature + 1)),
      rng_(1), num_measurements_allowed_(kMaxNumMeasurements) {
    std::iota(assigned_.begin(), assigned_.end(), 0);

    const auto log_likelihoods = ComputeLogLikelihoods(stddev_);
    entropies_ = ComputeExpectedEntropies(log_likelihoods);
    penalty_table_.resize(kMaxTemperature + 1);
    for (int i = 0; i <= kMaxTemperature; ++i) {
      penalty_table_[i].resize(kMaxTemperature + 1);
      for (int j = 0; j <= kMaxTemperature; ++j) {
        penalty_table_[i][j] = static_cast<int>(-std::round(log_likelihoods[i][j] * 1000));
      }
    }
  }

  std::vector<std::vector<int>> CreateTemperatureMap() {
    enum class Pattern {
      kSquare,
      kGingham,
    };

    Pattern pattern;
    int low;
    int smoothing_count;
    const int s = static_cast<int>(std::sqrt(stddev_));
    if (stddev_ <= 400) {
      pattern = Pattern::kGingham;
      smoothing_count = static_cast<int>(std::round(std::pow((grid_size_ + 12) / 7.0, 2.0)));
      low = std::max(0, 480 - 25 * s);
    } else {
      pattern = Pattern::kSquare;
      if (stddev_ <= 484) {
        low = 100;
        smoothing_count = static_cast<int>(std::round(grid_size_ * 1.5 - 12));
      } else {
        low = 0;
        const double a = (2 - (s - 24) / 6.0) * 0.75;
        const double b = (s - 24) / 6.0 * 4 - 12;
        smoothing_count = static_cast<int>(std::round(grid_size_ * a + b));
      }
    }
    smoothing_count = std::max(smoothing_count, 10);

    const int high = kMaxTemperature - low;
    const int half = grid_size_ / 2;
    temperature_map_.assign(grid_size_, std::vector<int>(grid_size_));
    for (int y = 0; y < grid_size_; ++y) {
      for (int x = 0; x < grid_size_; ++x) {
        if (pattern == Pattern::kSquare) {
          temperature_map_[y][x] = (y < half && x < half) ? high : low;
        }
        if (pattern == Pattern::kGingham) {
          if (std::max(y, x) < half) {
            temperature_map_[y][x] = low;
          } else if (std::min(y, x) < half) {
            temperature_map_[y][x] = kMaxTemperature / 2;
          } else {
            temperature_map_[y][x] = high;
          }
        }
      }
    }
    for (int i = 0; i < smoothing_count; ++i) {
      const auto prev = temperature_map_;
      for (int y = 0; y < grid_size_; ++y) {
        for (int x = 0; x < grid_size_; ++x) {
          if (pattern == Pattern::kSquare) {
            if (half - 1 <= y && y <= half + 5 && half - 1 <= x && x <= half + 5) continue;
          }
          if (pattern == Pattern::kGingham) {
            if (half - 1 <= y && y <= half && half - 1 <= x && x <= half) continue;
          }

          temperature_map_[y][x] = prev[y][x];
          for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
              if (std::abs(dy) + std::abs(dx) != 1) continue;
              temperature_map_[y][x] += prev[(y + dy + grid_size_) % grid_size_][(x + dx + grid_size_) % grid_size_] * 2;
            }
          }
          temperature_map_[y][x] = (temperature_map_[y][x] + 4) / 9;
        }
      }
    }
    return temperature_map_;
  }

  std::tuple<int, int, int> DetermineMove(int measured_temperature) {
    if (measured_temperature >= 0) {
      const auto& [wormhole, dy, dx] = last_measurement_;
      for (int i = 0; i < num_exits_; ++i) {
        penalties_[wormhole][i] += penalty_table_[GetTemperature(i, dy, dx)][measured_temperature];
      }
      const auto min = *std::min_element(penalties_[wormhole].begin(), penalties_[wormhole].end());
      for (int i = 0; i < num_exits_; ++i) {
        penalties_[wormhole][i] -= min;
      }
      for (int i = 0; i < num_exits_; ++i) {
        if (penalties_[i][assigned_[i]] == 0) continue;
        UpdateAssignment();
        break;
      }
    }

    if (num_measurements_allowed_-- == 0) {
      return {-1, -1, -1};
    }

    std::pair<int, int> target;
    bool separated = true;
    for (int i = 0; i < num_exits_; ++i) {
      int second = -1;
      for (int j = 0; j < num_exits_; ++j) {
        if (j == assigned_[i]) continue;
        if (second >= 0 && penalties_[i][j] >= penalties_[i][second]) continue;
        second = j;
      }
      if (penalties_[i][second] > penalties_[i][assigned_[i]] + 1500) continue;
      separated = false;
      target = {i, second};
      break;
    }

    if (stddev_ <= 64 || separated) {
      const auto [cost, wormhole0, wormhole1] = FindMinimumLoop();
      target = {wormhole0, assigned_[wormhole1]};
      if (cost > 6000) {
        return {-1, -1, -1};
      }
    }

    const auto& [wormhole, exit] = target;
    std::vector<std::pair<int, int>> candidates;
    for (int i = 0; i < num_exits_; ++i) {
      if (i == assigned_[wormhole] || i == exit) continue;
      const auto priority = penalties_[wormhole][assigned_[wormhole]] + 1000 - penalties_[wormhole][i] + std::uniform_int_distribution(0, 100)(rng_);
      if (priority > 0) {
        candidates.emplace_back(priority, i);
      }
    }
    const std::size_t limit = 10;
    if (candidates.size() > limit) {
      std::sort(candidates.rbegin(), candidates.rend());
      candidates.resize(limit);
    }
    std::vector<int> confusing_exits;
    confusing_exits.push_back(assigned_[wormhole]);
    confusing_exits.push_back(exit);
    for (const auto& [p, i] : candidates) {
      confusing_exits.push_back(i);
    }
    std::vector<int> temperatures(confusing_exits.size());

    std::pair<int, int> best_offset;
    const int half = grid_size_ / 2;
    double best_gain = 0.0;
    for (int dy = -half; dy <= half; ++dy) {
      for (int dx = -half; dx <= half; ++dx) {
        for (std::size_t i = 0; i < confusing_exits.size(); ++i) {
          temperatures[i] = GetTemperature(confusing_exits[i], dy, dx);
        }
        double sum = 0.0;
        for (std::size_t i = 0; i < confusing_exits.size(); ++i) {
          for (std::size_t j = 0; j < i; ++j) {
            sum += ((i == 1) + 1) * entropies_[std::abs(temperatures[i] - temperatures[j])];
          }
        }
        sum /= 10 + std::abs(dy) + std::abs(dx);
        if (sum > best_gain) {
          best_offset = {dy, dx};
          best_gain = sum;
        }
      }
    }

    const auto& [dy, dx] = best_offset;
    return last_measurement_ = {wormhole, dy, dx};
  }

  std::vector<int> GetAssignment() const {
    return assigned_;
  }

 private:
  int GetTemperature(int exit_index, int dy, int dx) const {
    const auto& [y, x] = exits_[exit_index];
    return temperature_map_[(y + dy + grid_size_) % grid_size_][(x + dx + grid_size_) % grid_size_];
  }

  std::tuple<int, int, int> FindMinimumLoop() const {
    std::vector distances(num_exits_, std::vector<int>(num_exits_));
    for (int i = 0; i < num_exits_; ++i) {
      for (int j = 0; j < num_exits_; ++j) {
        distances[i][j] = penalties_[i][assigned_[j]];
      }
    }
    for (int k = 0; k < num_exits_; ++k) {
      for (int i = 0; i < num_exits_; ++i) {
        for (int j = 0; j < num_exits_; ++j) {
          distances[i][j] = std::min(distances[i][j], distances[i][k] + distances[k][j]);
        }
      }
    }
    constexpr auto kInfinity = std::numeric_limits<int>::max();
    std::pair<int, int> min_cost = {kInfinity, kInfinity};
    std::pair<int, int> best_edge;
    for (int i = 0; i < num_exits_; ++i) {
      for (int j = 0; j < num_exits_; ++j) {
        if (j == i || distances[i][j] != penalties_[i][assigned_[j]]) continue;
        const std::pair<int, int> cost = {distances[i][j] + distances[j][i], distances[i][j]};
        if (cost < min_cost) {
          min_cost = cost;
          best_edge = {i, j};
        }
      }
    }
    return {min_cost.first, best_edge.first, best_edge.second};
  }

  void UpdateAssignment() {
    for (int j = 0; j < num_exits_; ++j) {
      auto min = std::numeric_limits<int>::max();
      for (int i = 0; i < num_exits_; ++i) {
        min = std::min(min, penalties_[i][j]);
      }
      for (int i = 0; i < num_exits_; ++i) {
        penalties_[i][j] -= min;
      }
    }

    while (true) {
      const int source = num_exits_ * 2;
      const int sink = source + 1;
      atcoder::mf_graph<int> graph(sink + 1);
      for (int i = 0; i < num_exits_; ++i) {
        graph.add_edge(source, i, 1);
        graph.add_edge(num_exits_ + i, sink, 1);
        for (int j = 0; j < num_exits_; ++j) {
          if (penalties_[i][j] == 0) {
            graph.add_edge(i, num_exits_ + j, 1);
          }
        }
      }

      if (graph.flow(source, sink) == num_exits_) {
        for (const auto [from, to, cap, flow] : graph.edges()) {
          if (from == source || to == sink || flow == 0) continue;
          assigned_[from] = to - num_exits_;
        }
        return;
      }

      std::vector<int> wormhole_used(num_exits_);
      std::vector<int> exit_used(num_exits_);
      const auto min_cut = graph.min_cut(source);
      for (int i = 0; i < num_exits_; ++i) {
        if (!min_cut[i]) {
          wormhole_used[i] = 1;
        }
      }
      for (int i = 0; i < num_exits_; ++i) {
        if (min_cut[num_exits_ + i]) {
          exit_used[i] = 1;
        }
      }
      auto min = std::numeric_limits<int>::max();
      for (int i = 0; i < num_exits_; ++i) {
        if (wormhole_used[i]) continue;
        for (int j = 0; j < num_exits_; ++j) {
          if (exit_used[j]) continue;
          min = std::min(min, penalties_[i][j]);
        }
      }
      for (int i = 0; i < num_exits_; ++i) {
        for (int j = 0; j < num_exits_; ++j) {
          if (wormhole_used[i] && exit_used[j]) {
            penalties_[i][j] += min;
          }
          if (!wormhole_used[i] && !exit_used[j]) {
            penalties_[i][j] -= min;
          }
        }
      }
    }
  }

  const int grid_size_;
  const int num_exits_;
  const int stddev_;
  const std::vector<std::pair<int, int>> exits_;
  std::vector<int> assigned_;
  std::vector<std::vector<int>> penalties_;
  std::vector<std::vector<int>> penalty_table_;
  std::vector<double> entropies_;
  std::mt19937 rng_;
  std::vector<std::vector<int>> temperature_map_;
  std::tuple<int, int, int> last_measurement_;
  int num_measurements_allowed_;
};

int main() {
  int L, N, S;
  std::cin >> L >> N >> S;
  std::vector<std::pair<int, int>> exits(N);
  for (auto& [Y, X] : exits) {
    std::cin >> Y >> X;
  }

  Solver solver(L, S, exits);
  const auto temperature_map = solver.CreateTemperatureMap();
  for (int y = 0; y < L; ++y) {
    for (int x = 0; x < L; ++x) {
      std::cout << temperature_map[y][x] << (x + 1 == L ? '\n' : ' ');
    }
  }
  std::cout.flush();

  int measured_temperature = -1;
  while (true) {
    const auto [wormhole, dy, dx] = solver.DetermineMove(measured_temperature);
    std::cout << wormhole << ' ' << dy << ' ' << dx << std::endl;
    if (wormhole < 0) {
      for (const auto& exit : solver.GetAssignment()) {
        std::cout << exit << std::endl;
      }
      break;
    }
    std::cin >> measured_temperature;
  }

  return 0;
}
