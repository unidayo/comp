// 最終更新: 2026-07-12 14:27:44 JST
struct S{
    ll value;
    int size;
};
using F = ll;
S op(S a, S b){ return {a.value+b.value, a.size+b.size}; }
S e(){ return {0, 0}; }
S mapping(F f, S x){ return {x.value + f*x.size, x.size}; }
F composition(F f, F g){ return f+g; }
F id(){ return 0; }
