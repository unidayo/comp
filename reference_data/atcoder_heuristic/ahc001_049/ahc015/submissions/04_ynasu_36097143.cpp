#pragma GCC optimize("O3,unroll-loops")
#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr int kBoxSize = 10;
constexpr int kNumCandies = kBoxSize * kBoxSize;

class Random {
 public:
  constexpr explicit Random(std::int64_t seed) {
    state_[0] = seed;
    state_[1] = RotateLeft(seed, 11);
    for (int i = 0; i < 16; ++i) {
      Next();
    }
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

class Solver {
 public:
  Solver(const std::vector<int>& flavors) :
      rng_(299792458), count_(0), flavors_(flavors), box_(kNumCandies),
      next_box_(kNumCandies), rows_(kBoxSize), dsu_(kNumCandies) {
    std::vector<int> counts(4);
    for (int i = 0; i < kNumCandies; ++i) {
      ++counts[flavors_[i]];
    }
    majority_ = std::max_element(counts.begin(), counts.end()) - counts.begin();
  }

  char DetermineDirection(int position) {
    for (int i = 0, j = 0; i < kNumCandies; ++i) {
      if (box_[i] || ++j < position) continue;
      box_[i] = flavors_[count_++];
      break;
    }

    double best_estimate = 0.0;
    char best_direction = 'L';
    const std::string directions = "LRFB";
    for (const auto direction : directions) {
      const auto next = TiltBox(box_, direction);
      const auto estimate = EstimateScore(next);
      if (estimate > best_estimate) {
        best_estimate = estimate;
        best_direction = direction;
      }
    }
    box_ = TiltBox(box_, best_direction);

    return best_direction;
  }

 private:
  double EstimateScore(const std::vector<int>& box) {
    const int num_trials = 1600;
    double sum = 0.0;
    double sum2 = 0.0;
    for (int trial = 0; trial < num_trials; ++trial) {
      std::copy(box.begin(), box.end(), next_box_.begin());
      for (int i = 0, j = rng_.Range(kNumCandies - count_);
           i < kNumCandies; ++i) {
        if (next_box_[i] || j-- > 0) continue;
        next_box_[i] = flavors_[count_];
        break;
      }
      for (int i = 0; i < kBoxSize; ++i) {
        rows_[i].clear();
        for (int j = 0; j < kBoxSize; ++j) {
          const int index = kBoxSize * i + j;
          if (next_box_[index] == 0) continue;
          rows_[i].push_back(next_box_[index]);
        }
      }
      for (int k = count_ + 1; k < kNumCandies; ++k) {
        int position = rng_.Range(kNumCandies - k);
        for (int i = 0; i < kBoxSize; ++i) {
          const int num_slots = kBoxSize - rows_[i].size();
          if (position < num_slots) {
            if (flavors_[k] == majority_) {
              rows_[i].push_back(flavors_[k]);
            } else {
              rows_[i].push_front(flavors_[k]);
            }
            break;
          } else {
            position -= num_slots;
          }
        }
      }
      dsu_.Reset();
      for (int i = 0; i < kBoxSize; ++i) {
        std::copy(rows_[i].begin(), rows_[i].end(), next_box_.begin() + kBoxSize * i);
        for (int j = 0; j < kBoxSize; ++j) {
          const int index = kBoxSize * i + j;
          if (i > 0 && next_box_[index] == next_box_[index - kBoxSize]) {
            dsu_.Merge(index, index - kBoxSize);
          }
          if (j > 0 && next_box_[index] == next_box_[index - 1]) {
            dsu_.Merge(index, index - 1);
          }
        }
      }
      double score = 0;
      for (int i = 0; i < kNumCandies; ++i) {
        if (!dsu_.IsLeader(i)) continue;
        const int size = dsu_.GetSize(i);
        score += size * size;
      }
      sum += score;
      sum2 += score * score;
    }
    const double mean = sum / num_trials;
    const double sd = std::sqrt(sum2 / num_trials - mean * mean);
    return mean + 0.1 * sd;
  }

  static std::vector<int> TiltBox(const std::vector<int>& box, char direction) {
    std::vector<int> tilted(kNumCandies);
    std::vector<int> candies;
    if (direction == 'L' || direction == 'R') {
      for (int i = 0; i < kBoxSize; ++i) {
        candies.clear();
        for (int j = 0; j < kBoxSize; ++j) {
          const auto& flavor = box[kBoxSize * i + j];
          if (flavor == 0) continue;
          candies.push_back(flavor);
        }
        const int offset = (direction == 'L') ? 0 : (kBoxSize - candies.size());
        for (std::size_t j = 0; j < candies.size(); ++j) {
          tilted[kBoxSize * i + offset + j] = candies[j];
        }
      }
    } else {
      for (int j = 0; j < kBoxSize; ++j) {
        candies.clear();
        for (int i = 0; i < kBoxSize; ++i) {
          const auto& flavor = box[kBoxSize * i + j];
          if (flavor == 0) continue;
          candies.push_back(flavor);
        }
        const int offset = (direction == 'F') ? 0 : (kBoxSize - candies.size());
        for (std::size_t i = 0; i < candies.size(); ++i) {
          tilted[kBoxSize * (offset + i) + j] = candies[i];
        }
      }
    }
    return tilted;
  }

  Random rng_;
  int count_;
  int majority_;
  std::vector<int> flavors_;
  std::vector<int> box_;
  std::vector<int> next_box_;
  std::vector<std::deque<int>> rows_;
  DisjointSets dsu_;
};

int main() {
  std::vector<int> flavors(kNumCandies);
  for (auto& flavor : flavors) {
    std::cin >> flavor;
  }

  Solver solver(flavors);
  for (int i = 1; i < kNumCandies; ++i) {
    int position;
    std::cin >> position;
    std::cout << solver.DetermineDirection(position) << std::endl;
  }

  return 0;
}
