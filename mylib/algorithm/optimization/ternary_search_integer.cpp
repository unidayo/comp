// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/optimization/ternary_search_integer.cpp
#pragma once

#include <cassert>
#include <utility>

// 閉区間 [left, right] の整数で f の最小値（MIN=true）/最大値を求める。
// 最適区間の外側では狭義単調である単峰関数が対象。最適区間は平坦でもよい。
// 戻り値: {最適位置, 関数値}。同値なら最も左の位置を返す。
// O(log(right-left+1)) 回評価、追加空間 O(1)。区間幅は __int128 で計算する。
template <bool MIN = true, class F>
auto ternary_search_integer(long long left, long long right, F f) {
    assert(left <= right);
    auto better = [](const auto& a, const auto& b) {
        if constexpr (MIN) return a < b;
        else return b < a;
    };
    while (static_cast<__int128>(right) - left > 3) {
        __int128 third = (static_cast<__int128>(right) - left) / 3;
        long long m1 = static_cast<long long>(static_cast<__int128>(left) + third);
        long long m2 = static_cast<long long>(static_cast<__int128>(right) - third);
        auto y1 = f(m1);
        auto y2 = f(m2);
        if (better(y2, y1)) left = m1 + 1;
        else right = m2;
    }
    auto value = f(left);
    std::pair<long long, decltype(value)> best{left, value};
    for (long long x = left; x != right;) {
        ++x;
        auto y = f(x);
        if (better(y, best.second)) best = {x, y};
    }
    return best;
}
