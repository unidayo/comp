// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/lazysegtree/range_add_range_max.cpp
using S = long long;
using F = long long;
S op(S a, S b){ return max(a, b); }
S e(){ return -INF64; }
S mapping(F f, S x){ return f+x; }
F composition(F f, F g){ return f+g; }
F id(){ return 0; }
