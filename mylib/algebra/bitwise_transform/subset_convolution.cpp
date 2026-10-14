// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/algebra/bitwise_transform/zeta_mobius_transform.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/bitwise_transform/subset_convolution.cpp
// Subset Convolution。
//
// 使い方:
//   auto c = subset_convolution(a, b);
//   // c[S] = sum_{T subset S} a[T] * b[S ^ T]
//
// 注意:
//   - a.size() == b.size() かつ 2 冪。
//   - T は +=, -=, *= ができる型。
//   - 計算量 O(K^2 2^K), K = log2(a.size())。

template <class T>
vector<T> subset_convolution(const vector<T> &a, const vector<T> &b) {
    assert(a.size() == b.size());
    int n = (int)a.size();
    assert(n > 0 && (n & (n - 1)) == 0);

    int k = countr_zero((unsigned)n);
    vector fa(k + 1, vector<T>(n, T()));
    vector fb(k + 1, vector<T>(n, T()));
    for (int mask = 0; mask < n; mask++) {
        int pc = popcount((unsigned)mask);
        fa[pc][mask] = a[mask];
        fb[pc][mask] = b[mask];
    }

    for (int i = 0; i <= k; i++) {
        subset_zeta_transform(fa[i]);
        subset_zeta_transform(fb[i]);
    }

    vector fc(k + 1, vector<T>(n, T()));
    for (int i = 0; i <= k; i++) {
        for (int j = 0; i + j <= k; j++) {
            for (int mask = 0; mask < n; mask++) {
                fc[i + j][mask] += fa[i][mask] * fb[j][mask];
            }
        }
    }

    for (int i = 0; i <= k; i++) subset_mobius_transform(fc[i]);

    vector<T> c(n);
    for (int mask = 0; mask < n; mask++) {
        c[mask] = fc[popcount((unsigned)mask)][mask];
    }
    return c;
}
