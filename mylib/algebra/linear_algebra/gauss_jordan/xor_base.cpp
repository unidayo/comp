// 最終更新: 2026-07-12 14:27:44 JST
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
