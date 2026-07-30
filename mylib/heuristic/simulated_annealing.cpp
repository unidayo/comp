// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/heuristic/fast_random.cpp, mylib/heuristic/timer.cpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "mylib/heuristic/fast_random.cpp"
#include "mylib/heuristic/timer.cpp"

/**
 * @brief Simulated annealing helper for AHC solvers.
 *
 * This class handles temperature scheduling and acceptance tests only. The
 * caller owns the state, generates neighborhoods, and applies/reverts moves.
 */
struct SimulatedAnnealing {
    enum class Objective {
        Maximize,
        Minimize,
    };

    enum class Schedule {
        Exponential,
        Linear,
    };

    struct Config {
        double start_temp = 1.0;
        double end_temp = 0.01;
        double time_limit_ms = -1.0;
        int max_iterations = -1;
        Objective objective = Objective::Maximize;
        Schedule schedule = Schedule::Exponential;
        std::uint64_t seed = 1;
    };

    Config cfg;
    Timer timer;
    FastRandom rng;
    int iteration = 0;

    explicit SimulatedAnnealing(Config config)
        : cfg(config), timer(config.time_limit_ms), rng(config.seed) {
        assert(cfg.start_temp >= 0.0);
        assert(cfg.end_temp >= 0.0);
        assert(cfg.time_limit_ms >= 0.0 || cfg.max_iterations >= 0);
    }

    void reset() {
        timer.reset(cfg.time_limit_ms);
        rng.reseed(cfg.seed);
        iteration = 0;
    }

    bool should_continue() const {
        if (cfg.time_limit_ms >= 0.0 && timer.expired()) return false;
        if (cfg.max_iterations >= 0 && iteration >= cfg.max_iterations) return false;
        return true;
    }

    void next_iteration() {
        ++iteration;
    }

    double progress() const {
        double p = 0.0;
        bool has_progress = false;
        if (cfg.time_limit_ms >= 0.0) {
            p = std::max(p, timer.progress());
            has_progress = true;
        }
        if (cfg.max_iterations > 0) {
            p = std::max(p, std::clamp(static_cast<double>(iteration) / cfg.max_iterations, 0.0, 1.0));
            has_progress = true;
        }
        return has_progress ? p : 1.0;
    }

    double temperature() const {
        const double p = progress();
        if (cfg.schedule == Schedule::Linear) {
            return cfg.start_temp + (cfg.end_temp - cfg.start_temp) * p;
        }
        if (cfg.start_temp <= 0.0 || cfg.end_temp <= 0.0) {
            return cfg.start_temp + (cfg.end_temp - cfg.start_temp) * p;
        }
        return cfg.start_temp * std::pow(cfg.end_temp / cfg.start_temp, p);
    }

    // Positive delta means improvement if objective is Maximize.
    bool accept_delta(double delta) {
        if (cfg.objective == Objective::Minimize) delta = -delta;
        if (delta >= 0.0) return true;
        const double temp = temperature();
        if (temp <= 0.0) return false;
        return rng.uniform01() < std::exp(delta / temp);
    }

    template <class Score>
    bool accept(Score current_score, Score next_score) {
        return accept_delta(static_cast<double>(next_score) - static_cast<double>(current_score));
    }

    template <class Score>
    bool update_best(Score score, Score& best_score) const {
        if (cfg.objective == Objective::Maximize) {
            if (score <= best_score) return false;
        } else {
            if (score >= best_score) return false;
        }
        best_score = score;
        return true;
    }

    bool is_better(double lhs, double rhs) const {
        return cfg.objective == Objective::Maximize ? lhs > rhs : lhs < rhs;
    }
};
