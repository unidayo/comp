// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/linear_algebra/gauss_jordan/xor_base.cpp
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
