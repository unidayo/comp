#include <algorithm>
#include <bit>
#include <bitset>
#include <chrono>
#include <compare>
#include <iostream>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cmath>
#include <cstdint>
#include <boost/functional/hash.hpp>

constexpr double kTimeLimit = 1.95;
constexpr int kMaxNumItems = 100;
constexpr int kMaxNumQueries = 32 * kMaxNumItems;

using BitSet = std::bitset<kMaxNumItems>;

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

struct Balance {
  Balance(int N, int Q) : num_items(N), num_available_queries(Q) {}

  std::weak_ordering Compare(const BitSet& left, const BitSet& right) {
    std::vector<int> left_indices, right_indices;
    for (int i = 0; i < num_items; ++i) {
      if (left.test(i)) {
        left_indices.push_back(i);
      }
      if (right.test(i)) {
        right_indices.push_back(i);
      }
    }
    return Compare(left_indices, right_indices);
  }

  std::weak_ordering Compare(const std::vector<int>& left, const std::vector<int>& right) {
    --num_available_queries;
    std::vector<int> used(num_items);
    for (const auto& set : {left, right}) {
      for (const auto& index : set) {
        used[index] = true;
      }
    }
    std::cout << left.size() << ' ' << right.size();
    for (const auto& set : {left, right}) {
      for (const auto& index : set) {
        std::cout << ' ' << index;
      }
    }
    std::cout << std::endl;
    char result;
    std::cin >> result;
    if (result == '<') return std::weak_ordering::less;
    if (result == '>') return std::weak_ordering::greater;
    return std::weak_ordering::equivalent;
  }

  void ConsumeQueries() {
    while (num_available_queries > 0) {
      Compare(std::vector<int>{0}, std::vector<int>{1});
    }
  }

 private:
  const int num_items;
  int num_available_queries;
};

struct Comparer {
  Comparer(int N, int Q) : balance(N, Q), num_available_queries(Q) {}

  std::partial_ordering Compare(BitSet a, BitSet b, bool without_balance) {
    if (a == b) {
      return std::weak_ordering::equivalent;
    }
    if ((a & b) == a) {
      return std::weak_ordering::less;
    }
    if ((a & b) == b) {
      return std::weak_ordering::greater;
    }
    const auto intersection = a & b;
    a ^= intersection;
    b ^= intersection;
    if (const auto iter = cache.find({a, b}); iter != cache.end()) {
      return iter->second;
    }
    const bool less_or_equivalent = IsReachable(a, b);
    const bool greater_or_equivalent = IsReachable(b, a);
    if (less_or_equivalent || greater_or_equivalent) {
      auto result = std::weak_ordering::equivalent;
      if (less_or_equivalent && !greater_or_equivalent) {
        result = std::weak_ordering::less;
      }
      if (!less_or_equivalent && greater_or_equivalent) {
        result = std::weak_ordering::greater;
      }
      cache.insert({{a, b}, result});
      cache.insert({{b, a}, 0 <=> result});
      return result;
    }
    if (without_balance) {
      return std::partial_ordering::unordered;
    }

    --num_available_queries;
    if (num_available_queries < 0) {
      return std::partial_ordering::unordered;
    }
    const int index_a = GetIndex(a);
    const int index_b = GetIndex(b);
    const auto result = balance.Compare(a, b);
    if (result != std::weak_ordering::greater) {
      AddEdge(index_a, index_b);
    }
    if (result != std::weak_ordering::less) {
      AddEdge(index_b, index_a);
    }
    cache.insert({{a, b}, result});
    cache.insert({{b, a}, 0 <=> result});
    return result;
  }

  int GetIndex(const BitSet& a) {
    if (const auto iter = indices.find(a); iter != indices.end()) {
      return iter->second;
    }
    const int index = nodes.size();
    reachable_nodes.emplace_back();
    reachable_nodes[index].set(index);
    nodes.push_back(a);
    return indices[a] = index;
  }

  void AddEdge(int index_a, int index_b) {
    for (const auto& index : {index_a, index_b}) {
      for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (reachable_nodes[i].test(index)) continue;
        if (!IsReachableFast(nodes[i], nodes[index])) continue;
        reachable_nodes[i] |= reachable_nodes[index];
      }
      for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (reachable_nodes[index].test(i)) continue;
        if (!IsReachableFast(nodes[index], nodes[i])) continue;
        reachable_nodes[index] |= reachable_nodes[i];
      }
    }
    for (std::size_t i = 0; i < nodes.size(); ++i) {
      if (!reachable_nodes[i].test(index_a)) continue;
      reachable_nodes[i] |= reachable_nodes[index_b];
    }
  }

  bool IsReachableFast(BitSet a, BitSet b) {
    const auto intersection = a & b;
    if (intersection == a) return true;
    if (intersection == b) return false;
    a &= ~intersection;
    b &= ~intersection;
    if (const auto iter = cache.find({a, b}); iter != cache.end()) {
      return iter->second <= 0;
    }
    const auto iter_a = indices.find(a);
    if (iter_a == indices.end()) return false;
    const auto iter_b = indices.find(b);
    if (iter_b == indices.end()) return false;
    return reachable_nodes[iter_a->second].test(iter_b->second);
  }

  bool IsReachable(const BitSet& a, const BitSet& b) {
    std::bitset<kMaxNumQueries * 2> reachable;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
      if (!IsReachableFast(a, nodes[i])) continue;
      reachable |= reachable_nodes[i];
    }
    for (std::size_t i = 0; i < nodes.size(); ++i) {
      if (!reachable.test(i)) continue;
      if (IsReachableFast(nodes[i], b)) return true;
    }
    return false;
  }

  struct PairHasher {
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& value) const {
      std::size_t seed = 0;
      boost::hash_combine(seed, std::hash<T>{}(value.first));
      boost::hash_combine(seed, std::hash<U>{}(value.second));
      return seed;
    }
  };

  Balance balance;
  int num_available_queries;
  std::vector<BitSet> nodes;
  std::unordered_map<BitSet, int> indices;
  std::unordered_map<std::pair<BitSet, BitSet>, std::weak_ordering, PairHasher> cache;
  std::vector<std::bitset<kMaxNumQueries * 2>> reachable_nodes;
};

struct ItemSet {
  ItemSet(std::initializer_list<int> items = {}) : size(items.size()) {
    std::ranges::fill(values, -1);
    std::ranges::fill(indices, -1);
    std::ranges::copy(items, values);
    for (int i = 0; i < size; ++i) {
      bitset.set(values[i]);
      indices[values[i]] = i;
    }
  }

  bool Empty() const {
    return size == 0;
  }

  int Size() const {
    return size;
  }

  int Choose(Random& random) const {
    return values[random.Range(size)];
  }

  void Add(int value) {
    bitset.set(value);
    values[size] = value;
    indices[value] = size++;
  }

  void Remove(int value) {
    bitset.reset(value);
    indices[values[--size]] = indices[value];
    std::swap(values[indices[value]], values[size]);
    indices[value] = -1;
  }

  auto begin() const {
    return values;
  }

  auto end() const {
    return values + size;
  }

  std::partial_ordering operator<=>(const ItemSet& other) const {
    return comparer->Compare(bitset, other.bitset, false);
  }

  std::partial_ordering CompareWithoutBalance(const ItemSet& other) const {
    return comparer->Compare(bitset, other.bitset, true);
  }

  static void Initialize(int N, int Q) {
    comparer = new Comparer(N, Q);
  }

  static void Finalize() {
    comparer->balance.ConsumeQueries();
  }

  static inline Comparer* comparer;

 private:
  BitSet bitset;
  int size;
  std::int8_t values[kMaxNumItems];
  std::int8_t indices[kMaxNumItems];
};

struct State {
  bool operator<=(const State& other) const {
    if (!sorted) return false;
    if (!(item_sets.front() >= other.item_sets.front())) return false;
    if (excluded_item_sets.size() == other.excluded_item_sets.size()) {
      return item_sets.back() <= other.item_sets.back();
    }
    return GetHeaviest() <= other.GetHeaviest();
  }

  const ItemSet& GetHeaviest() const {
    if (excluded_item_sets.empty()) {
      return item_sets.back();
    }
    return excluded_item_sets.front();
  }

  void Normalize() {
    if (!sorted) return;
    while (item_sets.back().Size() == 1) {
      excluded_item_sets.push_back(std::move(item_sets.back()));
      item_sets.pop_back();
    }
  }

  void Perturb(Random& rng) {
    const int num_sets = item_sets.size();
    std::vector<int> items;
    for (const auto& index : {num_sets - 1, 0}) {
      for (const auto& item : item_sets[index]) {
        items.push_back(item);
      }
      item_sets.erase(item_sets.begin() + index);
    }
    std::vector<ItemSet> randomized(2);
    for (const auto& item : items) {
      randomized[rng.Range(2)].Add(item);
    }
    Insert(randomized);
  }

  bool TryTransition(bool only_max_to_min_move, Random& rng) {
    const int size = item_sets.size();
    int a, b;
    if (only_max_to_min_move) {
      a = 0;
      b = size - 1;
    } else {
      const auto biased_random = [&rng, size]() {
        int index = size - 1;
        for (int i = 0; i < 10; ++i) {
          index = rng.Range(index + 1);
        }
        return index;
      };
      do {
        a = biased_random();
        b = size - 1 - biased_random();
      } while (a >= b);
    }

    auto left = item_sets[a];
    auto right = item_sets[b];
    bool updated = false;
    for (int i = 0; i < 5; ++i) {
      const auto order = left <=> right;
      if (order == std::partial_ordering::unordered) break;
      if (order > 0) {
        std::swap(left, right);
      }
      if (right.Empty()) continue;
      const auto old_left = left;
      const auto old_right = right;
      if (only_max_to_min_move || rng.Range(2)) {
        const int item = right.Choose(rng);
        left.Add(item);
        right.Remove(item);
      } else {
        if (left.Empty()) continue;
        const int item_a = left.Choose(rng);
        const int item_b = right.Choose(rng);
        if (!(ItemSet{item_a} < ItemSet{item_b})) continue;
        left.Remove(item_a);
        left.Add(item_b);
        right.Add(item_a);
        right.Remove(item_b);
      }
      if (!(left <= old_right)) {
        left = old_left;
        right = old_right;
        continue;
      }
      updated = true;
    }
    if (!updated) return false;

    item_sets.erase(item_sets.begin() + b);
    item_sets.erase(item_sets.begin() + a);
    Insert({left, right});
    Normalize();
    return true;
  }

  void OptimizeSimilarItems(const std::vector<int>& sorted_items) {
    const int num_items = sorted_items.size();
    std::vector<int> set_indices(num_items, -1);
    for (std::size_t i = 0; i < item_sets.size(); ++i) {
      for (const auto& item : item_sets[i]) {
        set_indices[item] = i;
      }
    }

    bool updated = false;
    for (int trial = 0; trial < 10; ++trial) {
      bool converged = true;
      for (int i = num_items - 1; i > 0; --i) {
        auto& a = set_indices[sorted_items[i - 1]];
        auto& b = set_indices[sorted_items[i]];
        if (a < 0 || b < 0 || !(item_sets[a] < item_sets[b])) continue;
        auto left = item_sets[a];
        left.Remove(sorted_items[i - 1]);
        left.Add(sorted_items[i]);
        if (!(left < item_sets[b])) continue;
        updated = true;
        converged = false;
        item_sets[a] = left;
        item_sets[b].Remove(sorted_items[i]);
        item_sets[b].Add(sorted_items[i - 1]);
        std::swap(a, b);
      }
      if (converged) break;
    }
    if (!updated) return;
    std::vector<ItemSet> unsorted_item_sets;
    item_sets.swap(unsorted_item_sets);
    Insert(unsorted_item_sets);
  }

  void Insert(const std::vector<ItemSet>& sets) {
    for (const auto& item_set : sets) {
      auto begin = item_sets.begin();
      auto end = item_sets.end();
      while (begin != end && begin->CompareWithoutBalance(item_set) < 0) {
        ++begin;
      }
      while (end != begin && (end - 1)->CompareWithoutBalance(item_set) >= 0) {
        --end;
      }
      const auto index = std::lower_bound(begin, end, item_set, [&](const ItemSet& a, const ItemSet& b) {
        const auto order = a <=> b;
        if (order == std::partial_ordering::unordered) {
          sorted = false;
        }
        return order < 0;
      }) - item_sets.begin();
      item_sets.insert(item_sets.begin() + index, item_set);
    }
  }

  std::vector<int> Serialize() const {
    int num_items = 0;
    for (const auto& sets : {item_sets, excluded_item_sets}) {
      for (const auto& set : sets) {
        num_items += set.Size();
      }
    }
    std::vector<int> indices(num_items);
    int index = 0;
    for (const auto& sets : {item_sets, excluded_item_sets}) {
      for (const auto& set : sets) {
        for (const auto& item : set) {
          indices[item] = index;
        }
        ++index;
      }
    }
    return indices;
  }

  bool sorted = true;
  std::vector<ItemSet> item_sets;
  std::vector<ItemSet> excluded_item_sets;
};

struct Solver {
  Solver(int N, int D, int Q) : rng(299792458) {
    int threshold = 20 + (N - D) * static_cast<int>(std::ceil(std::log2(D)));
    for (int i = 1; i < N; ++i) {
      threshold += static_cast<int>(std::ceil(std::log2(i)));
    }
    if (Q > threshold) {
      for (int i = 0; i < N; ++i) {
        sorted_items.insert(std::ranges::lower_bound(sorted_items, i, [](int a, int b) {
          return ItemSet{a} < ItemSet{b};
        }), i);
      }
      state.item_sets.resize(D);
      for (const auto& item : sorted_items | std::views::reverse) {
        auto a = state.item_sets.front();
        a.Add(item);
        state.item_sets.erase(state.item_sets.begin());
        state.Insert({a});
      }
      only_max_to_min_move = false;
    } else {
      std::vector<ItemSet> sets(D);
      for (int i = 0; i < N; ++i) {
        sets[i % D].Add(i);
      }
      state.Insert(sets);
      only_max_to_min_move = true;
    }

    state.Normalize();
    best_state = state;
    is_best_state = true;
  }

  std::vector<int> Optimize(Timer& timer) {
    for (int i = 0; timer.GetTime() < kTimeLimit; ++i) {
      if (TryTransition()) {
        i = 0;
      }
      if (i > 20) {
        only_max_to_min_move = false;
      }
      if (i > 1000) {
        i = 0;
        if (!sorted_items.empty()) {
          best_state.OptimizeSimilarItems(sorted_items);
        }
        state = best_state;
        state.Perturb(rng);
        is_best_state = false;
      }
    }
    if (!sorted_items.empty()) {
      best_state.OptimizeSimilarItems(sorted_items);
    }
    return best_state.Serialize();
  }

  bool TryTransition() {
    if (!state.TryTransition(only_max_to_min_move, rng)) return false;
    if (is_best_state || (state <= best_state)) {
      if (!is_best_state && !sorted_items.empty()) {
        state.OptimizeSimilarItems(sorted_items);
      }
      best_state = state;
      is_best_state = true;
    }
    return true;
  }

  Random rng;
  bool only_max_to_min_move;
  bool is_best_state;
  State state;
  State best_state;
  std::vector<int> sorted_items;
};

int main() {
  Timer timer;

  int N, D, Q;
  std::cin >> N >> D >> Q;
  ItemSet::Initialize(N, Q);

  Solver solver(N, D, Q);
  const auto indices = solver.Optimize(timer);
  ItemSet::Finalize();

  for (const auto index : indices) {
    std::cout << index << ' ';
  }
  std::cout << std::endl;

  return 0;
}
