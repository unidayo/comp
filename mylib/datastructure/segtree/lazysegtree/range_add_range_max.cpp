using S = long long;
using F = long long;
S op(S a, S b){ return max(a, b); }
S e(){ return -INF64; }
S mapping(F f, S x){ return f+x; }
F composition(F f, F g){ return f+g; }
F id(){ return 0; }
