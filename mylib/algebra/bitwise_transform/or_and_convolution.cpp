// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/algebra/bitwise_transform/zeta_mobius_transform.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/bitwise_transform/or_and_convolution.cpp
// OR 畳み込み・AND 畳み込み。
//
// 使い方:
//   auto c = or_convolution(a, b);
//   // c[S] = sum_{X | Y = S} a[X] * b[Y]
//
//   auto d = and_convolution(a, b);
//   // d[S] = sum_{X & Y = S} a[X] * b[Y]
//
// 注意:
//   - a.size() == b.size() かつ 2 冪。
//   - T は +=, -=, *= ができる型。
//   - 計算量 O(N log N), N = a.size()。

template <class T>
vector<T> or_convolution(vector<T> a, vector<T> b) {
    assert(a.size() == b.size());
    int n = (int)a.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    subset_zeta_transform(a);
    subset_zeta_transform(b);
    for (int i = 0; i < n; i++) a[i] *= b[i];
    subset_mobius_transform(a);
    return a;
}

template <class T>
vector<T> and_convolution(vector<T> a, vector<T> b) {
    assert(a.size() == b.size());
    int n = (int)a.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    superset_zeta_transform(a);
    superset_zeta_transform(b);
    for (int i = 0; i < n; i++) a[i] *= b[i];
    superset_mobius_transform(a);
    return a;
}
