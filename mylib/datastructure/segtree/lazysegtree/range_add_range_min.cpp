// 最終更新: 2026-07-12 14:27:44 JST
using S = ll;
using F = ll;
S op(S a, S b){ return min(a, b); }
S e(){ return INF64; }
S mapping(F f, S x){ return f+x; }
F composition(F f, F g){ return f+g; }
F id(){ return 0; }
