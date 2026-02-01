auto xor_base(vector<ull> a) {
    vector<ull> base;
    for (auto &x : a) {
        for (auto e : base) {
            x = min(x, x ^ e);
        }
        if (x > 0) {
            base.push_back(x);
        }
    }
    ranges::sort(base, greater());
    return base;
}