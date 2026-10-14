// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/optimization/convex_hull_trick.cpp
#pragma once

#include <cassert>
#include <functional>
#include <limits>
#include <set>

// 整数 x における直線 a*x+b の最小値（MIN=true）または最大値を管理する。
// 直線の追加順・クエリの x の順序に制約はない。空集合への query は不可。
// 追加: 償却 O(log N)、クエリ: O(log N)、空間: O(N)。
// a, b, x は long long。計算結果が long long を超える場合は query_wide を使う。
template <bool MIN = true>
struct ConvexHullTrick {
    using i128 = __int128_t;

   private:
    static constexpr i128 INF = i128(1) << 120;

    struct Line {
        i128 a, b;
        mutable i128 p;  // この直線が次の直線以上となる最後の整数 x

        bool operator<(const Line& other) const { return a < other.a; }
        bool operator<(i128 x) const { return p < x; }
    };

    std::multiset<Line, std::less<>> lines_;

    // b / a の数学的な floor。a != 0。
    static i128 floor_div(i128 b, i128 a) {
        i128 q = b / a, r = b % a;
        return q - (r != 0 && ((r < 0) != (a < 0)));
    }

    // x とその次の直線の交点を更新し、x が次の直線を不要にするなら true。
    bool intersect(typename std::multiset<Line, std::less<>>::iterator x,
                   typename std::multiset<Line, std::less<>>::iterator y) {
        if (y == lines_.end()) {
            x->p = INF;
            return false;
        }
        if (x->a == y->a)
            x->p = x->b > y->b ? INF : -INF;
        else
            x->p = floor_div(y->b - x->b, x->a - y->a);
        return x->p >= y->p;
    }

   public:
    bool empty() const { return lines_.empty(); }
    int size() const { return static_cast<int>(lines_.size()); }
    void clear() { lines_.clear(); }

    void add_line(long long a, long long b) {
        i128 sign = MIN ? -1 : 1;
        auto z = lines_.insert({sign * a, sign * b, 0});
        auto y = z++;
        auto x = y;
        while (intersect(y, z)) z = lines_.erase(z);
        if (x != lines_.begin() && intersect(--x, y)) intersect(x, y = lines_.erase(y));
        while ((y = x) != lines_.begin() && (--x)->p >= y->p) intersect(x, lines_.erase(y));
    }

    i128 query_wide(long long x) const {
        assert(!empty());
        const auto& line = *lines_.lower_bound(i128(x));
        i128 value = line.a * x + line.b;
        return MIN ? -value : value;
    }

    long long query(long long x) const {
        i128 value = query_wide(x);
        assert(std::numeric_limits<long long>::min() <= value &&
               value <= std::numeric_limits<long long>::max());
        return static_cast<long long>(value);
    }
};
