#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include "mylib/heuristic/fast_random.cpp"

/**
 * @brief Weighted random sampling utilities for AHC solvers.
 *
 * WeightedRandomSampler:
 *   Static weights. build O(N), sample O(log N).
 *
 * DynamicWeightedRandomSampler:
 *   Mutable weights with Fenwick tree. build O(N log N), update/sample O(log N).
 *
 * Weights must be non-negative and total weight must be positive when sampling.
 */
template <class Weight = long long>
struct WeightedRandomSampler {
    static_assert(std::is_arithmetic_v<Weight>);

    std::vector<long double> prefix;

    WeightedRandomSampler() = default;

    explicit WeightedRandomSampler(const std::vector<Weight>& weights) {
        build(weights);
    }

    void build(const std::vector<Weight>& weights) {
        prefix.assign(weights.size() + 1, 0.0L);
        for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
            assert(weights[i] >= Weight(0));
            prefix[i + 1] = prefix[i] + static_cast<long double>(weights[i]);
        }
    }

    int size() const {
        return static_cast<int>(prefix.size()) - 1;
    }

    long double total_weight() const {
        return prefix.empty() ? 0.0L : prefix.back();
    }

    bool empty() const {
        return size() <= 0;
    }

    bool can_sample() const {
        return total_weight() > 0.0L;
    }

    int sample(FastRandom& rng) const {
        assert(can_sample());
        long double x = static_cast<long double>(rng.uniform01()) * total_weight();
        int idx = static_cast<int>(std::upper_bound(prefix.begin(), prefix.end(), x) - prefix.begin()) - 1;
        if (idx < 0) idx = 0;
        if (idx >= size()) idx = size() - 1;
        return idx;
    }
};

template <class Weight = long long>
struct DynamicWeightedRandomSampler {
    static_assert(std::is_arithmetic_v<Weight>);

    int n = 0;
    std::vector<long double> bit;
    std::vector<long double> weights;

    DynamicWeightedRandomSampler() = default;

    explicit DynamicWeightedRandomSampler(int n) {
        init(n);
    }

    explicit DynamicWeightedRandomSampler(const std::vector<Weight>& initial_weights) {
        build(initial_weights);
    }

    void init(int size) {
        assert(size >= 0);
        n = size;
        bit.assign(n + 1, 0.0L);
        weights.assign(n, 0.0L);
    }

    void build(const std::vector<Weight>& initial_weights) {
        init(static_cast<int>(initial_weights.size()));
        for (int i = 0; i < n; ++i) {
            assert(initial_weights[i] >= Weight(0));
            set_weight(i, initial_weights[i]);
        }
    }

    int size() const {
        return n;
    }

    bool empty() const {
        return n == 0;
    }

    long double weight(int idx) const {
        assert(0 <= idx && idx < n);
        return weights[idx];
    }

    long double total_weight() const {
        return prefix_sum(n);
    }

    bool can_sample() const {
        return total_weight() > 0.0L;
    }

    void add_weight(int idx, Weight delta) {
        assert(0 <= idx && idx < n);
        long double nd = weights[idx] + static_cast<long double>(delta);
        assert(nd >= 0.0L);
        add_internal(idx, static_cast<long double>(delta));
        weights[idx] = nd;
    }

    void set_weight(int idx, Weight value) {
        assert(0 <= idx && idx < n);
        assert(value >= Weight(0));
        long double next = static_cast<long double>(value);
        add_internal(idx, next - weights[idx]);
        weights[idx] = next;
    }

    int sample(FastRandom& rng) const {
        assert(can_sample());
        long double x = static_cast<long double>(rng.uniform01()) * total_weight();
        int idx = lower_bound_prefix_greater(x);
        if (idx >= n) idx = n - 1;
        return idx;
    }

private:
    void add_internal(int idx, long double delta) {
        for (++idx; idx <= n; idx += idx & -idx) bit[idx] += delta;
    }

    long double prefix_sum(int count) const {
        long double res = 0.0L;
        for (int idx = count; idx > 0; idx -= idx & -idx) res += bit[idx];
        return res;
    }

    int lower_bound_prefix_greater(long double x) const {
        int idx = 0;
        long double sum = 0.0L;
        int step = 1;
        while ((step << 1) <= n) step <<= 1;
        for (; step > 0; step >>= 1) {
            int next = idx + step;
            if (next <= n && sum + bit[next] <= x) {
                idx = next;
                sum += bit[next];
            }
        }
        return idx;
    }
};

