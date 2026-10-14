// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/heuristic/fast_random.cpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

/**
 * @brief Lightweight random generator for AHC solvers.
 *
 * xoshiro256** with splitmix64 seeding. This is faster and more predictable
 * than std::mt19937 for typical heuristic search usage.
 */
struct FastRandom {
    using u64 = std::uint64_t;

    u64 s[4]{};
    bool has_normal_cache = false;
    double normal_cache = 0.0;

    explicit FastRandom(u64 seed = 1) {
        reseed(seed);
    }

    static u64 splitmix64(u64 x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static u64 rotl(u64 x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    void reseed(u64 seed) {
        for (int i = 0; i < 4; ++i) {
            seed = splitmix64(seed);
            s[i] = seed;
        }
        has_normal_cache = false;
    }

    u64 next_u64() {
        const u64 result = rotl(s[1] * 5, 7) * 9;
        const u64 t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);
        return result;
    }

    std::uint32_t next_u32() {
        return static_cast<std::uint32_t>(next_u64() >> 32);
    }

    // Uniform integer in [0, bound). Rejection avoids modulo bias.
    u64 uniform_u64(u64 bound) {
        assert(bound > 0);
        const u64 threshold = -bound % bound;
        while (true) {
            u64 x = next_u64();
            if (x >= threshold) return x % bound;
        }
    }

    int uniform_int(int l, int r) {
        assert(l <= r);
        const long long span = static_cast<long long>(r) - static_cast<long long>(l) + 1;
        return l + static_cast<int>(uniform_u64(static_cast<u64>(span)));
    }

    long long uniform_ll(long long l, long long r) {
        assert(l <= r);
        const __int128 span = static_cast<__int128>(r) - static_cast<__int128>(l) + 1;
        if (span > static_cast<__int128>(std::numeric_limits<u64>::max())) {
            return static_cast<long long>(next_u64());
        }
        return l + static_cast<long long>(uniform_u64(static_cast<u64>(span)));
    }

    // Uniform real in [0, 1).
    double uniform01() {
        return static_cast<double>(next_u64() >> 11) * (1.0 / 9007199254740992.0);
    }

    // Uniform real in [l, r).
    double uniform_real(double l, double r) {
        assert(l <= r);
        return l + (r - l) * uniform01();
    }

    bool bernoulli(double p) {
        assert(0.0 <= p && p <= 1.0);
        return uniform01() < p;
    }

    // Normal distribution by Box-Muller transform.
    double normal(double mean = 0.0, double stddev = 1.0) {
        assert(stddev >= 0.0);
        if (has_normal_cache) {
            has_normal_cache = false;
            return mean + stddev * normal_cache;
        }
        double u1 = std::max(uniform01(), std::numeric_limits<double>::min());
        double u2 = uniform01();
        double radius = std::sqrt(-2.0 * std::log(u1));
        double theta = 6.283185307179586476925286766559 * u2;
        normal_cache = radius * std::sin(theta);
        has_normal_cache = true;
        return mean + stddev * (radius * std::cos(theta));
    }

    // Exponential distribution with density lambda * exp(-lambda x), x >= 0.
    double exponential(double lambda = 1.0) {
        assert(lambda > 0.0);
        double u = std::max(1.0 - uniform01(), std::numeric_limits<double>::min());
        return -std::log(u) / lambda;
    }

    // Number of failures before first success.
    int geometric(double p) {
        assert(0.0 < p && p <= 1.0);
        if (p == 1.0) return 0;
        double u = std::max(1.0 - uniform01(), std::numeric_limits<double>::min());
        return static_cast<int>(std::log(u) / std::log(1.0 - p));
    }

    template <class Weight>
    int weighted_index(const std::vector<Weight>& weights) {
        assert(!weights.empty());
        long double total = 0.0L;
        for (Weight w : weights) {
            assert(w >= Weight(0));
            total += static_cast<long double>(w);
        }
        assert(total > 0.0L);
        long double x = static_cast<long double>(uniform01()) * total;
        for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
            x -= static_cast<long double>(weights[i]);
            if (x < 0.0L) return i;
        }
        return static_cast<int>(weights.size()) - 1;
    }

    template <class Iter>
    void shuffle(Iter first, Iter last) {
        int n = static_cast<int>(last - first);
        for (int i = n - 1; i > 0; --i) {
            std::iter_swap(first + i, first + uniform_int(0, i));
        }
    }

    template <class T>
    const T& choice(const std::vector<T>& values) {
        assert(!values.empty());
        return values[uniform_int(0, static_cast<int>(values.size()) - 1)];
    }
};
