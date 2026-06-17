struct S {
    ll value;
    int size;
};
using F = bool;

S op(S a, S b) { return {a.value + b.value, a.size + b.size}; }
S e() { return {0, 0}; }
S mapping(F f, S x) {
    if (f) x.value = x.size - x.value;
    return x;
}
F composition(F f, F g) { return f ^ g; }
F id() { return false; }
