// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/lazysegtree/range_flip_inversion.cpp
struct S {
    ll zero, one, inv;
};
using F = bool;

S op(S a, S b) { return {a.zero + b.zero, a.one + b.one, a.inv + b.inv + a.one * b.zero}; }
S e() { return {0, 0, 0}; }
S mapping(F f, S x) {
    if (f) {
        swap(x.zero, x.one);
        x.inv = x.zero * x.one - x.inv;
    }
    return x;
}
F composition(F f, F g) { return f ^ g; }
F id() { return false; }
