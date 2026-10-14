// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/lazysegtree/range_set_range_max.cpp
using S = ll;
using F = ll;
const S INF = 8e18;
const F ID = 8e18;
S op(S a, S b){ return max(a, b); }
S e(){ return -INF; }
S mapping(F f, S x){ return (f == ID ? x : f); }
F composition(F f, F g){ return (f == ID ? g : f); }
F id(){ return ID; }
