// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/lazysegtree/range_affine_range_sum.cpp
struct S {
    mint value;
    int size;
};
struct F {
    mint b, c;
};

S op(S a, S b) { return {a.value + b.value, a.size + b.size}; }
S e() { return {0, 0}; }
S mapping(F f, S x) { return {f.b * x.value + f.c * x.size, x.size}; }
F composition(F f, F g) { return {f.b * g.b, f.b * g.c + f.c}; }
F id() { return {1, 0}; }
