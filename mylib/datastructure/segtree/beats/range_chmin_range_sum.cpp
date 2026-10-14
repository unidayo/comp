// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/datastructure/segtree/abstract_segtree_beats.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/beats/range_chmin_range_sum.cpp
#include <algorithm>
#include <cassert>
#include <optional>

// AbstractSegTreeBeats 用。通常の ACL lazy_segtree には渡せない。
// 要素・閾値は (-BEATS_INF, BEATS_INF) 内、総和と中間計算は long long 内。
constexpr long long BEATS_INF = 4'000'000'000'000'000'000LL;
struct S {
    long long sum = 0;
    long long max_value = -BEATS_INF, second_max = -BEATS_INF;
    int max_count = 0, size = 0;
};
using F = long long;
S e() { return {}; }
S make_leaf(long long x) {
    assert(-BEATS_INF < x && x < BEATS_INF);
    return {x, x, -BEATS_INF, 1, 1};
}
S op(S a, S b) {
    if (a.size == 0) return b;
    if (b.size == 0) return a;
    S c;
    c.sum = a.sum + b.sum;
    c.size = a.size + b.size;
    if (a.max_value == b.max_value) {
        c.max_value = a.max_value;
        c.second_max = std::max(a.second_max, b.second_max);
        c.max_count = a.max_count + b.max_count;
    } else if (a.max_value > b.max_value) {
        c.max_value = a.max_value;
        c.second_max = std::max(a.second_max, b.max_value);
        c.max_count = a.max_count;
    } else {
        c.max_value = b.max_value;
        c.second_max = std::max(a.max_value, b.second_max);
        c.max_count = b.max_count;
    }
    return c;
}
std::optional<S> mapping(F f, S x) {
    if (x.size == 0 || x.max_value <= f) return x;
    if (f <= x.second_max) return std::nullopt;
    x.sum += (f - x.max_value) * x.max_count;
    x.max_value = f;
    return x;
}
F composition(F f, F g) { return std::min(f, g); }
F id() { return BEATS_INF; }
