// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/lazysegtree/range_set_range_sum_mint.cpp
struct S {
    mint value;
    int size;
};
struct F {
    mint value;
    bool flag;
};

S op(S a, S b) { return {a.value + b.value, a.size + b.size}; }
S e() { return {0, 0}; }
S mapping(F f, S x) {
    if (f.flag) x.value = f.value * x.size;
    return x;
}
F composition(F f, F g) { return (f.flag ? f : g); }
F id() { return {0, false}; }
