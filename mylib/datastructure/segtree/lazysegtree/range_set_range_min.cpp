// 最終更新: 2026-07-12 14:27:44 JST
using S = ll;
using F = ll;
const S INF = 8e18;
const F ID = 8e18;
S op(S a, S b){ return min(a, b); }
S e(){ return INF; }
S mapping(F f, S x){ return (f == ID ? x : f); }
F composition(F f, F g){ return (f == ID ? g : f); }
F id(){ return ID; }
