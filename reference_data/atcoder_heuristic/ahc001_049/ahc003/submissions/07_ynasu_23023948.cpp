#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <cmath>

constexpr double kInfinity = std::numeric_limits<double>::infinity();
constexpr int kSize = 30;
constexpr int kPaddedSize = kSize + 2;
constexpr int kNumNodes = kPaddedSize * kPaddedSize;
constexpr int kDeltas[4] = {-kPaddedSize, kPaddedSize, -1, 1};
constexpr const char* kDirections = "UDLR";

template <int N>
constexpr std::array<std::array<double, N>, kPaddedSize> CreateTable() {
  std::array<std::array<double, N>, kPaddedSize> table = {};
  for (int i = 0; i < kPaddedSize; ++i) {
    const double x = 1.0 * i / kSize;
    double sum = 0.0;
    for (int j = 0; j < N; ++j) {
      const double peak = 1.0 * j / (N - 1);
      table[i][j] = std::max(0.0, 1.0 - 2.0 * std::max(x - peak, peak - x));
      sum += table[i][j];
    }
    for (int j = 0; j < N; ++j) {
      table[i][j] /= sum;
    }
  }
  return table;
}

template <int N>
constexpr auto kContributionTable = CreateTable<N>();

class Estimator {
  struct Statistics {
    Statistics() : edge_sums(2), uniform_sums(2), smoothed_sums(2) {}
    double target_value;
    double denominator;
    std::vector<std::unordered_map<int, double>> edge_sums;
    std::vector<std::unordered_map<int, double>> uniform_sums;
    std::vector<std::unordered_map<int, std::vector<double>>> smoothed_sums;
  };

 public:
  Estimator(double edge_scale, double uniform_scale,
            double smoothed_scale, int num_divisions) :
      rng_(1), edge_scale_(edge_scale), uniform_scale_(uniform_scale),
      smoothed_scale_(smoothed_scale), num_divisions_(num_divisions),
      edge_weights_(2, std::vector<double>(kNumNodes)),
      uniform_weights_(2, std::vector<double>(kPaddedSize)),
      smoothed_weights_(2, std::vector<std::vector<double>>(
          kPaddedSize, std::vector<double>(num_divisions))) {}

  double GetWeight(int axis, int edge) const {
    const int indices[2] = {edge / kPaddedSize, edge % kPaddedSize};
    double weight = 3500.0 + edge_weights_[axis][edge];
    weight += uniform_weights_[axis][indices[axis]];
    if (num_divisions_ == 4) {
      for (int i = 0; i < num_divisions_; ++i) {
        const auto coefficient = kContributionTable<4>[indices[1 - axis]][i];
        weight += coefficient * smoothed_weights_[axis][indices[axis]][i];
      }
    }
    if (num_divisions_ == 8) {
      for (int i = 0; i < num_divisions_; ++i) {
        const auto coefficient = kContributionTable<8>[indices[1 - axis]][i];
        weight += coefficient * smoothed_weights_[axis][indices[axis]][i];
      }
    }
    return weight;
  }

  void Update(const std::vector<std::vector<int>>& edges, int feedback) {
    auto& added_stats = statistics_.emplace_back();
    added_stats.target_value = feedback;
    added_stats.denominator = 0.0;
    for (int axis = 0; axis < 2; ++axis) {
      for (const int edge : edges[axis]) {
        const int indices[2] = {edge / kPaddedSize, edge % kPaddedSize};
        if (edge_scale_ != 0.0) {
          added_stats.edge_sums[axis][edge] += 1.0;
        }
        if (uniform_scale_ != 0.0) {
          added_stats.uniform_sums[axis][indices[axis]] += 1.0;
        }
        if (smoothed_scale_ != 0.0) {
          auto& sums = added_stats.smoothed_sums[axis][indices[axis]];
          sums.resize(num_divisions_);
          if (num_divisions_ == 4) {
            for (int i = 0; i < num_divisions_; ++i) {
              sums[i] += kContributionTable<4>[indices[1 - axis]][i];
            }
          }
          if (num_divisions_ == 8) {
            for (int i = 0; i < num_divisions_; ++i) {
              sums[i] += kContributionTable<8>[indices[1 - axis]][i];
            }
          }
        }
      }
      for (const auto [edge, sum] : added_stats.edge_sums[axis]) {
        added_stats.denominator += sum * sum;
      }
      for (const auto [index, sum] : added_stats.uniform_sums[axis]) {
        added_stats.denominator += sum * sum;
      }
      for (const auto& [index, sums] : added_stats.smoothed_sums[axis]) {
        for (const auto sum : sums) {
          added_stats.denominator += sum * sum;
        }
      }
    }

    for (auto& weights : uniform_weights_) {
      for (auto& w : weights) {
        w *= 0.99;
      }
    }
    for (auto& weights : edge_weights_) {
      for (auto& w : weights) {
        w *= 0.99;
      }
    }
    for (auto& weights : smoothed_weights_) {
      for (auto& v : weights) {
        for (auto& w : v) {
          w *= 0.99;
        }
      }
    }

    const int time = statistics_.size();
    std::vector<int> times(1, time - 1);
    for (int t = 0; t < time; ++t) {
      times.push_back(t);
    }
    std::shuffle(times.begin() + 1, times.end(), rng_);
    for (std::size_t i = 0; i < times.size(); ++i) {
      const auto& stats = statistics_[times[i]];
      double estimated = 0.0;
      for (int axis = 0; axis < 2; ++axis) {
        for (const auto [edge, sum] : stats.edge_sums[axis]) {
          estimated += GetWeight(axis, edge);
        }
      }
      const double error = estimated - stats.target_value;
      const double gradient = std::pow(0.997, i) * error / stats.denominator;
      for (int axis = 0; axis < 2; ++axis) {
        for (const auto [edge, sum] : stats.edge_sums[axis]) {
          auto& weight = edge_weights_[axis][edge];
          weight -= edge_scale_ * sum * gradient;
          weight = std::clamp(weight, -1000.0, 1000.0);
        }
        for (const auto [index, sum] : stats.uniform_sums[axis]) {
          auto& weight = uniform_weights_[axis][index];
          weight -= uniform_scale_ * sum * gradient;
          weight = std::clamp(weight, -20000.0, 20000.0);
        }
        for (const auto& [index, sums] : stats.smoothed_sums[axis]) {
          for (int k = 0; k < num_divisions_; ++k) {
            auto& weight = smoothed_weights_[axis][index][k];
            weight -= smoothed_scale_ * sums[k] * gradient;
            weight = std::clamp(weight, -20000.0, 20000.0);
          }
        }
      }
    }
  }

 private:
  std::mt19937 rng_;
  const double edge_scale_;
  const double uniform_scale_;
  const double smoothed_scale_;
  const int num_divisions_;
  std::vector<std::vector<double>> edge_weights_;
  std::vector<std::vector<double>> uniform_weights_;
  std::vector<std::vector<std::vector<double>>> smoothed_weights_;
  std::vector<Statistics> statistics_;
};

class Solver {
 public:
  Solver() : is_outside_(kNumNodes) {
    for (int i = 0; i < kPaddedSize; ++i) {
      is_outside_[i] = 1;
      is_outside_[kPaddedSize * (kPaddedSize - 1) + i] = 1;
      is_outside_[kPaddedSize * i] = 1;
      is_outside_[kPaddedSize * i + (kPaddedSize - 1)] = 1;
    }
    estimators_.emplace_back(0.2, 1.0, 0.0, 0);
    estimators_.emplace_back(0.2, 0.0, 1.0, 4);
    estimators_.emplace_back(0.1, 0.0, 1.0, 8);
    last_estimates_.resize(estimators_.size());
    errors_.resize(estimators_.size());
  }

  std::string Search(int start, int goal, int feedback) {
    const Estimator* best_estimator = &estimators_[0];
    if (feedback > 0) {
      for (std::size_t i = 0; i < estimators_.size(); ++i) {
        estimators_[i].Update(last_edges_, feedback);
        errors_[i] *= 0.995;
        errors_[i] += std::abs(last_estimates_[i] / feedback - 1.0);
      }
      const auto best = std::min_element(errors_.begin(), errors_.end());
      best_estimator = &estimators_[best - errors_.begin()];
    }

    std::vector<double> distances(kNumNodes, kInfinity);
    std::vector<int> move_indices(kNumNodes, -1);
    using T = std::tuple<double, int>;
    std::priority_queue<T, std::vector<T>, std::greater<T>> queue;
    queue.emplace(0.0, goal);
    distances[goal] = 0.0;
    while (!queue.empty()) {
      const auto [distance, node] = queue.top();
      queue.pop();
      if (distance > distances[node]) continue;
      if (node == start) break;
      for (int move_index = 0; move_index < 4; ++move_index) {
        const int prev = node - kDeltas[move_index];
        if (is_outside_[prev]) continue;
        const int axis = move_index < 2;
        const int edge = std::min(node, prev);
        const double weight = best_estimator->GetWeight(axis, edge);
        const double prev_distance = distance + std::clamp(weight, 1000.0, 9000.0);
        if (prev_distance >= distances[prev]) continue;
        distances[prev] = prev_distance;
        move_indices[prev] = move_index;
        queue.emplace(prev_distance, prev);
      }
    }

    std::string moves;
    last_edges_.clear();
    last_edges_.resize(2);
    for (std::size_t i = 0; i < estimators_.size(); ++i) {
      last_estimates_[i] = 0.0;
    }
    for (int next, node = start; node != goal; node = next) {
      const int move_index = move_indices[node];
      moves.push_back(kDirections[move_index]);
      next = node + kDeltas[move_index];
      const int axis = move_index < 2;
      const int edge = std::min(node, next);
      last_edges_[axis].push_back(edge);
      for (std::size_t i = 0; i < estimators_.size(); ++i) {
        last_estimates_[i] += estimators_[i].GetWeight(axis, edge);
      }
    }
    return moves;
  }

 private:
  std::vector<int> is_outside_;
  std::vector<std::vector<int>> last_edges_;
  std::vector<double> last_estimates_;
  std::vector<double> errors_;
  std::vector<Estimator> estimators_;
};

int main() {
  Solver solver;
  int feedback = 0;
  for (int k = 0; k < 1000; ++k) {
    int si, sj, ti, tj;
    std::cin >> si >> sj >> ti >> tj;
    const int start = kPaddedSize * (si + 1) + (sj + 1);
    const int goal = kPaddedSize * (ti + 1) + (tj + 1);
    const std::string moves = solver.Search(start, goal, feedback);
    std::cout << moves << std::endl;
    std::cin >> feedback;
  }
  return 0;
}
