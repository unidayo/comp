// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/lazysegtree/range_chmax_range_min.cpp
#include <algorithm>
#include <limits>

// range chmax / range min: 0-indexed、半開区間 [l, r)。
// 要素値・更新の閾値は long long の最小値・最大値を除く範囲で使う。
// 初期値は vector<S> で渡す。seg(n) は各葉が e() になるので注意。
// atcoder::lazy_segtree<S, op, e, F, mapping, composition, id> seg(a);
// seg.apply(l, r, x);  // a[i] = std::max(a[i], x)
// seg.prod(l, r);      // 区間 min。空区間は e()。
using S = long long;
using F = long long;
S op(S a, S b) { return std::min(a, b); }
S e() { return std::numeric_limits<S>::max(); }
S mapping(F f, S x) { return x == e() ? e() : std::max(f, x); }
F composition(F f, F g) { return std::max(f, g); }
F id() { return std::numeric_limits<F>::lowest(); }
