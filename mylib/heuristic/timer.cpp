#pragma once

#include <algorithm>
#include <chrono>

/**
 * @brief Small steady-clock timer for AHC solvers.
 *
 * Example:
 *   Timer timer(1950);
 *   while (!timer.expired()) {
 *       ...
 *   }
 */
struct Timer {
    using Clock = std::chrono::steady_clock;

    Clock::time_point started;
    double limit_ms = -1.0;

    explicit Timer(double limit_ms = -1.0) : started(Clock::now()), limit_ms(limit_ms) {}

    void reset(double new_limit_ms = -1.0) {
        started = Clock::now();
        if (new_limit_ms >= 0.0) limit_ms = new_limit_ms;
    }

    double elapsed_ms() const {
        return std::chrono::duration<double, std::milli>(Clock::now() - started).count();
    }

    double elapsed_sec() const {
        return std::chrono::duration<double>(Clock::now() - started).count();
    }

    long long elapsed_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - started).count();
    }

    double remaining_ms() const {
        if (limit_ms < 0.0) return 1e100;
        return limit_ms - elapsed_ms();
    }

    bool expired() const {
        return limit_ms >= 0.0 && elapsed_ms() >= limit_ms;
    }

    bool should_continue() const {
        return !expired();
    }

    double progress() const {
        if (limit_ms <= 0.0) return 0.0;
        return std::clamp(elapsed_ms() / limit_ms, 0.0, 1.0);
    }
};

