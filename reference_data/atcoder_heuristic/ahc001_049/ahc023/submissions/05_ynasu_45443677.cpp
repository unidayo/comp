#pragma GCC optimize("Ofast")
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <chrono>
#include <iostream>
#include <map>
#include <queue>
#include <ranges>
#include <span>
#include <string>
#include <tuple>
#include <vector>
#include <cmath>
#include <cstdint>

constexpr double kTimeLimit = 1.95;
constexpr int kNumMonths = 100;
constexpr int kGridSize = 20;
constexpr int kNumBlocks = kGridSize * kGridSize;

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

struct BitSet : public std::bitset<kNumMonths + 1> {
  using std::bitset<kNumMonths + 1>::bitset;

  std::tuple<int, int> FindRange(int i) const {
    const auto as_array = std::bit_cast<std::array<std::uint64_t, 2>>(*this);
    const auto prev_mask = (1ull << (i & 63)) - 1;
    const auto next_mask = ~prev_mask << 1;
    i /= 64;
    int prev, next;
    if (const auto masked = as_array[i] & prev_mask) {
      prev = i * 64 + 63 - std::countl_zero(masked);
    } else {
      prev = 63 - std::countl_zero(as_array[0]);
    }
    if (const auto masked = as_array[i] & next_mask) {
      next = i * 64 + std::countr_zero(masked);
    } else {
      next = 64 + std::countr_zero(as_array[1]);
    }
    return {prev, next};
  }
};

struct IndexSet {
  IndexSet(int N) : indices(N, -1) {
    values.reserve(N);
  }

  void Add(int value) {
    indices[value] = values.size();
    values.push_back(value);
  }

  void Remove(int value) {
    indices[values.back()] = indices[value];
    std::swap(values[indices[value]], values.back());
    values.pop_back();
    indices[value] = -1;
  }

  std::vector<int> values;
  std::vector<int> indices;
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

  constexpr int MinMax(int min, int max) {
    return Range(max - min + 1) + min;
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
  for (int i = 0; ; ++i) {
    if ((i & ((1 << 10) - 1)) == 0) {
      const auto progress = (timer.GetTime() - start) * progress_scale;
      if (progress >= 1.0) break;
      T = T0 * ApproxPow(base, progress);
    }
    state.TryTransition(-T * std::log2(rng.Canonical()), rng);
  }
  return state.GetResult();
}

struct State {
  auto GetEdges(int block) const {
    return std::span(csr_edges.begin() + csr_indices[block],
                     csr_edges.begin() + csr_indices[block + 1]);
  }

  State(int root_, const std::vector<std::vector<int>>& edges,
        const std::vector<std::tuple<int, int>>& crops_) :
      root(root_), crops(crops_) {
    for (int block = 0; block < kNumBlocks; ++block) {
      csr_indices.push_back(csr_edges.size());
      csr_edges.insert(csr_edges.end(), edges[block].begin(), edges[block].end());
    }
    csr_indices.push_back(csr_edges.size());

    for (int origin = 0; origin < kNumBlocks; ++origin) {
      std::ranges::fill(distances[origin], kNumBlocks);
      std::queue<int> queue;
      distances[origin][origin] = 0;
      queue.push(origin);
      while (!queue.empty()) {
        const auto block = queue.front();
        queue.pop();
        for (const auto& next : GetEdges(block)) {
          if (distances[origin][next] < kNumBlocks) continue;
          distances[origin][next] = distances[origin][block] + 1;
          queue.push(next);
          blocks[origin][distances[origin][next]].push_back(next);
        }
      }
    }
    blocks_in_tree.assign(kNumMonths, kNumBlocks);

    std::ranges::fill(changed_months, 1);
    slot_counts[kNumMonths][kNumMonths] = kNumBlocks;
    next_slot_counts[kNumMonths][kNumMonths] = kNumBlocks;
    for (int block = 0; block < kNumBlocks; ++block) {
      ends[block].set(0);
      ends[block].set(kNumMonths);
      best_ends[block].set(0);
      best_ends[block].set(kNumMonths);
    }

    std::vector<std::map<int, int>> crop_counts_map(kNumMonths + 1);
    for (const auto& [S, D] : crops) {
      ++crop_counts_map[D][D - S];
    }
    for (int month = 0; month <= kNumMonths; ++month) {
      for (const auto& [length, count] : crop_counts_map[month] | std::views::reverse) {
        crop_counts[month].emplace_back(length, count);
      }
    }

    best_score = score = ComputeGain();
    scores = next_scores;
  }

  void TryTransition(double tolerance, Random& rng) {
    int target_block;
    int target_month;
    while (true) {
      target_month = rng.MinMax(3, kNumMonths - 1);
      target_block = rng.Range(kNumBlocks);
      if (ends[target_block].test(target_month) || rng.Range(2) == 0) break;
    }
    changed_blocks.clear();
    std::ranges::fill(changed_months, 0);
    changed_months[target_month] = 1;
    next_slot_counts[target_month] = slot_counts[target_month];

    if (ends[target_block].test(target_month)) {
      std::ranges::fill(reachable, false);
      if (target_block != root) {
        int count = 0;
        for (const auto& next : GetEdges(target_block)) {
          count += ends[next].test(target_month);
        }
        if (count == 1) {
          changed_blocks.push_back(target_block);
        } else {
          reachable[root] = true;
          stack.push_back(root);
          while (!stack.empty()) {
            const auto block = stack.back();
            stack.pop_back();
            for (const auto& next : GetEdges(block)) {
              if (reachable[next] || next == target_block || !ends[next].test(target_month)) continue;
              reachable[next] = true;
              stack.push_back(next);
            }
          }
          for (const auto& block : blocks_in_tree[target_month].values) {
            if (reachable[block]) continue;
            changed_blocks.push_back(block);
          }
        }

        for (const auto& block : changed_blocks) {
          const auto [prev, next] = ends[block].FindRange(target_month);
          if (!changed_months[next]) {
            changed_months[next] = 1;
            next_slot_counts[next] = slot_counts[next];
          }
          --next_slot_counts[target_month][target_month - prev];
          --next_slot_counts[next][next - target_month];
          ++next_slot_counts[next][next - prev];
        }
      }
    } else {
      int destination = root;
      if (ends[destination].test(target_month)) {
        int count = 0;
        for (int distance = 1; distance < kNumBlocks; ++distance) {
          for (const auto& block : blocks[target_block][distance]) {
            if (!ends[block].test(target_month)) continue;
            if (rng.Range(++count) == 0) {
              destination = block;
            }
            break;
          }
          if (count > 0) break;
        }
      } else {
        changed_blocks.push_back(destination);
      }
      while (destination != target_block) {
        int count = 0;
        int block = -1;
        for (const auto& next : GetEdges(destination)) {
          if (distances[target_block][next] >= distances[target_block][destination]) continue;
          if (rng.Range(++count) == 0) {
            block = next;
          }
        }
        destination = block;
        changed_blocks.push_back(destination);
      }
      for (const auto& block : changed_blocks) {
        const auto [prev, next] = ends[block].FindRange(target_month);
        if (!changed_months[next]) {
          changed_months[next] = 1;
          next_slot_counts[next] = slot_counts[next];
        }
        --next_slot_counts[next][next - prev];
        ++next_slot_counts[next][next - target_month];
        ++next_slot_counts[target_month][target_month - prev];
      }
    }

    if (!ends[target_block].test(target_month)) {
      tolerance *= 0.2;
    }
    const auto gain = ComputeGain();
    if (gain < -tolerance) return;

    for (int month = 0; month <= kNumMonths; ++month) {
      if (!changed_months[month]) continue;
      scores[month] = next_scores[month];
      slot_counts[month] = next_slot_counts[month];
    }
    if (ends[target_block].test(target_month)) {
      for (const auto& block : changed_blocks) {
        blocks_in_tree[target_month].Remove(block);
        ends[block].flip(target_month);
      }
    } else {
      for (const auto& block : changed_blocks) {
        blocks_in_tree[target_month].Add(block);
        ends[block].flip(target_month);
      }
    }
    score += gain;
    if (score > best_score) {
      best_score = score;
      best_ends = ends;
    }
  }

  int ComputeGain() {
    int gain = 0;
    for (int month = 0; month <= kNumMonths; ++month) {
      if (!changed_months[month]) continue;
      int sum = 0;
      int capacity = kNumMonths;
      auto count = next_slot_counts[month][capacity];
      for (const auto& [length, num_crops] : crop_counts[month]) {
        while (capacity > length) {
          count += next_slot_counts[month][--capacity];
        }
        const int num_planted = std::min(count, num_crops);
        sum += num_planted * length;
        count -= num_planted;
      }
      next_scores[month] = sum;
      gain += sum - scores[month];
    }
    return gain;
  }

  std::vector<std::tuple<int, int, int>> GetResult() const {
    std::vector<std::vector<std::tuple<int, int>>> sorted_crops(kNumMonths + 1);
    for (std::size_t i = 0; i < crops.size(); ++i) {
      const auto& [S, D] = crops[i];
      sorted_crops[D].emplace_back(D - S, i);
    }
    for (int month = 0; month <= kNumMonths; ++month) {
      std::ranges::sort(sorted_crops[month], std::greater{});
    }

    std::vector<std::vector<std::tuple<int, int>>> slots(kNumMonths + 1);
    for (int block = 0; block < kNumBlocks; ++block) {
      for (int prev = 0, month = 1; month <= kNumMonths; ++month) {
        if (!best_ends[block].test(month)) continue;
        slots[month].emplace_back(month - prev, block);
        prev = month;
      }
    }
    int sum = 0;
    std::vector<std::tuple<int, int, int>> plan;
    for (int month = 0; month <= kNumMonths; ++month) {
      std::ranges::sort(slots[month], std::greater{});
      auto iter = sorted_crops[month].begin();
      for (const auto& [capacity, block] : slots[month]) {
        while (iter != sorted_crops[month].end()) {
          const auto [length, index] = *iter++;
          if (length > capacity) continue;
          plan.emplace_back(index, block, month - capacity);
          sum += length;
          break;
        }
      }
    }
    return plan;
  }

  const int root;
  const std::vector<std::tuple<int, int>> crops;
  std::vector<int> csr_edges;
  std::vector<int> csr_indices;

  int score;
  int best_score;
  std::array<BitSet, kNumBlocks> ends = {};
  std::array<BitSet, kNumBlocks> best_ends = {};

  std::vector<int> changed_blocks;
  std::array<int, kNumMonths + 1> changed_months = {};
  std::array<int, kNumMonths + 1> scores = {};
  std::array<int, kNumMonths + 1> next_scores = {};
  std::array<bool, kNumBlocks> reachable = {};
  std::array<std::array<std::uint16_t, kNumBlocks>, kNumBlocks> distances = {};
  std::array<std::array<std::vector<std::uint16_t>, kNumBlocks>, kNumBlocks> blocks = {};
  std::array<std::vector<std::tuple<std::uint16_t, std::uint16_t>>, kNumMonths + 1> crop_counts = {};
  std::array<std::array<std::uint16_t, kNumMonths + 1>, kNumMonths + 1> slot_counts = {};
  std::array<std::array<std::uint16_t, kNumMonths + 1>, kNumMonths + 1> next_slot_counts = {};
  std::vector<IndexSet> blocks_in_tree;
  std::vector<int> stack;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  int T, H, W, i0;
  std::cin >> T >> H >> W >> i0;

  const int root = W * i0;
  std::vector<std::vector<int>> edges(kNumBlocks);
  for (int i = 0; i + 1 < H; ++i) {
    std::string h;
    std::cin >> h;
    for (int j = 0; j < W; ++j) {
      if (h[j] == '1') continue;
      const int a = W * i + j;
      const int b = W * (i + 1) + j;
      edges[a].push_back(b);
      edges[b].push_back(a);
    }
  }
  for (int i = 0; i < H; ++i) {
    std::string w;
    std::cin >> w;
    for (int j = 0; j + 1 < W; ++j) {
      if (w[j] == '1') continue;
      const int a = W * i + j;
      const int b = W * i + j + 1;
      edges[a].push_back(b);
      edges[b].push_back(a);
    }
  }

  int K;
  std::cin >> K;
  std::vector<std::tuple<int, int>> crops(K);
  for (auto& [S, D] : crops) {
    std::cin >> S >> D;
    --S;
  }

  Timer timer;
  Random rng(299792458);
  const double T0 = 1.0e1;
  const double T1 = 1.0e-1;
  State state(root, edges, crops);
  const auto plan = Anneal(state, rng, T0, T1, timer, kTimeLimit);

  std::cout << plan.size() << std::endl;
  for (const auto& [crop_index, block, month] : plan) {
    std::cout << crop_index + 1 << ' ' << block / W << ' ' << block % W << ' '
              << month + 1 << std::endl;
  }

  return 0;
}
