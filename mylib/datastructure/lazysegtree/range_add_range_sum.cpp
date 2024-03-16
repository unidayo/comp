template <class T> struct St {
    T value;
    ll size;
};

template <class T> auto range_add_range_sum(int n, auto id) {
    using S = St<T>;
    auto op = [](S l, S r) -> S { return S{l.value + r.value, l.size + r.size}; };
    auto e = []() -> S { return S{T(), 0}; };
    auto mapping = [](T f, S x) -> S { return S{x.value + x.size * f, x.size}; };
    auto composition = [](T f, T g) -> T { return f + g; };
    lazy_segtree<S, op, e, T, mapping, composition, id> seg(n);
    return seg;
}

template <class T> auto range_add_range_sum(vector<T> &a, auto id) {
    using S = St<T>;
    auto op = [](S l, S r) -> S { return S{l.value + r.value, l.size + r.size}; };
    auto e = []() -> S { return S{T(), 0}; };
    auto mapping = [](T f, S x) -> S { return S{x.value + x.size * f, x.size}; };
    auto composition = [](T f, T g) -> T { return f + g; };
    vector<S> vec;
    ranges::transform(a, back_inserter(vec), [](T x) { return S{x, 1}; });
    lazy_segtree<S, op, e, T, mapping, composition, id> seg(vec);
    return seg;
}