// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/acl/convolution.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/fps/fps_inv.cpp
using mint = modint998244353;

/*
 * fps_inv:
 *  f[0] != 0 を仮定
 *  f の逆元 g = 1/f mod x^n を返す
 *  計算量: O(n log n)
 */
vector<mint> fps_inv(const vector<mint>& f, int n) {
    assert(n >= 0);
    assert(!f.empty());
    assert(f[0].val() != 0);
    if (n == 0) return {};

    vector<mint> g(1);
    g[0] = f[0].inv();

    int m = 1;
    while (m < n) {
        int len = min(2 * m, n);

        vector<mint> f_cut(len);
        for (int i = 0; i < len && i < (int)f.size(); i++) {
            f_cut[i] = f[i];
        }

        vector<mint> fg = convolution(f_cut, g);
        fg.resize(len);

        for (int i = 0; i < len; i++) {
            fg[i] = -fg[i];
        }
        fg[0] += 2;

        vector<mint> ng = convolution(g, fg);
        ng.resize(len);

        g.swap(ng);
        m = len;
    }

    g.resize(n);
    return g;
}
