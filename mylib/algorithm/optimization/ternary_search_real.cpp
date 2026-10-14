// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/optimization/ternary_search_real.cpp
#pragma once

#include <cassert>
#include <cmath>
#include <numeric>
#include <type_traits>
#include <utility>

// 閉区間 [left, right] で f の最小値（MIN=true）/最大値を近似する。
// 最適区間の外側では狭義単調である単峰関数が対象。最適区間は平坦でもよい。
// 戻り値: {位置, 関数値}。O(iterations) 回評価、追加空間 O(1)。
// Real は double / long double など。端点は有限、f の値は NaN を含まないこと。
template <bool MIN = true, class Real, class F>
auto ternary_search_real(Real left, Real right, F f, int iterations = 200) {
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
    consider(right);
    for (int i = 0; i < iterations; ++i) {
        Real m1 = std::lerp(left, right, Real(1) / Real(3));
        Real m2 = std::lerp(left, right, Real(2) / Real(3));
        // 丸めによって区間を縮められなくなったら終了する。
        if (!(left < m1 && m1 < m2 && m2 < right)) break;
        auto y1 = consider(m1);
        auto y2 = consider(m2);
        if (better(y2, y1)) left = m1;
        else right = m2;
    }
    consider(std::midpoint(left, right));
    return best;
}
