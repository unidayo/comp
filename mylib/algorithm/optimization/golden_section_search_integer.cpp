// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/optimization/golden_section_search_integer.cpp
#pragma once

#include <cassert>
#include <optional>
#include <type_traits>
#include <utility>

// 整数版の黄金分割探索（フィボナッチ探索）。閉区間 [left, right] が対象。
// 最適区間の外側では狭義単調である単峰関数の最小値/最大値を求める。
// MIN=true: 最小化、false: 最大化。最適区間は平坦でもよい。
// 戻り値: {最も左の最適位置, 関数値}。
// 要素数が1500以下なら f の呼び出しは高々15回。区間外や同じ位置は評価しない。
// O(log(right-left+1)) 回評価、追加空間 O(1)。区間の演算は __int128。
template <bool MIN = true, class F>
auto golden_section_search_integer(long long left, long long right, F f) {
    assert(left <= right);
    using Wide = __int128;
    using Value = std::decay_t<decltype(f(left))>;
    if (left == right) return std::pair<long long, Value>{left, f(left)};
    const Wide count = Wide(right) - left + 1;
    auto better = [](const auto& a, const auto& b) {
        if constexpr (MIN) return a < b;
        else return b < a;
    };
    // 開区間 (0, a+b) を探索。offset=1 が left、offset=count が right。
    // 区間外はすべての実値より悪い番兵とし、f を呼ばない。
    auto position = [&](Wide offset) {
        return static_cast<long long>(Wide(left) + offset - 1);
    };
    auto evaluate = [&](Wide offset) -> std::optional<Value> {
        if (offset < 1 || offset > count) return std::nullopt;
        return f(position(offset));
    };
    Wide a = 1, b = 1;
    while (a + b < count + 1) {
        Wide next = a + b;
        a = b;
        b = next;
    }
    Wide lo = 0;
    Wide m1 = a, m2 = b;
    auto y1 = evaluate(m1);
    auto y2 = evaluate(m2);
    while (b > 1) {
        const bool move_left = y2 && (!y1 || better(*y2, *y1));
        if (move_left) {
            lo = m1;
            m1 = m2;
            y1 = y2;
        } else {
            m2 = m1;
            y2 = y1;
        }
        // (F[k-2], F[k-1]) -> (F[k-3], F[k-2])。
        Wide previous = b - a;
        b = a;
        a = previous;
        if (b == 1) break;
        if (move_left) {
            m2 = lo + b;
            y2 = evaluate(m2);
        } else {
            m1 = lo + a;
            y1 = evaluate(m1);
        }
    }
    // 最後は幅2の開区間に1候補だけ残る。値も保存済みなので再評価しない。
    assert(m1 == lo + 1 && y1);
    return std::pair<long long, Value>{position(m1), *y1};
}
