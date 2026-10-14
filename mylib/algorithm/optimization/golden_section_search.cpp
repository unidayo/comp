// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/optimization/golden_section_search.cpp
#pragma once

#include <cassert>
#include <cmath>
#include <numeric>
#include <type_traits>
#include <utility>

// 閉区間 [left, right] で f の最小値（MIN=true）/最大値を近似する実数探索。
// 最適区間の外側では狭義単調である単峰関数が対象。最適区間は平坦でもよい。
// 戻り値: {評価した最良の位置, 関数値}。同値の位置の選び方は保証しない。
// iterations 回区間を縮小。初期評価後は一回につき新たな関数評価が高々一回。
// O(iterations) 時間（f の評価が O(1) の場合）、追加空間 O(1)。C++20 以上。
template <bool MIN = true, class Real, class F>
auto golden_section_search(Real left, Real right, F f, int iterations = 200) {
    static_assert(std::is_floating_point_v<Real>);
    assert(std::isfinite(left) && std::isfinite(right) && left <= right);
    assert(iterations >= 0);
    auto better = [](const auto& a, const auto& b) {
        if constexpr (MIN) return a < b;
        else return b < a;
    };
    auto value = f(left);
    std::pair<Real, decltype(value)> best{left, value};
    auto consider = [&](Real x) {
        auto y = f(x);
        if (better(y, best.second)) best = {x, y};
        return y;
    };
    if (left == right) return best;
    consider(right);
    if (iterations > 0) {
        const Real ratio = (std::sqrt(Real(5)) - Real(1)) / Real(2);
        Real m1 = std::lerp(left, right, Real(1) - ratio);
        Real m2 = std::lerp(left, right, ratio);
        if (left < m1 && m1 < m2 && m2 < right) {
            auto y1 = consider(m1);
            auto y2 = consider(m2);
            for (int i = 0; i < iterations; ++i) {
                if (better(y2, y1)) {
                    left = m1;
                    m1 = m2;
                    y1 = y2;
                    if (i + 1 == iterations) break;
                    m2 = std::lerp(left, right, ratio);
                    if (!(left < m1 && m1 < m2 && m2 < right)) break;
                    y2 = consider(m2);
                } else {
                    right = m2;
                    m2 = m1;
                    y2 = y1;
                    if (i + 1 == iterations) break;
                    m1 = std::lerp(left, right, Real(1) - ratio);
                    if (!(left < m1 && m1 < m2 && m2 < right)) break;
                    y1 = consider(m1);
                }
            }
        }
    }
    consider(std::midpoint(left, right));
    return best;
}
