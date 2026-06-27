// 高速ゼータ変換・メビウス変換。
//
// 使い方:
//   vector<long long> f(1 << n);
//
//   subset_zeta_transform(f);
//   // f[S] = sum_{T subset S} original[T]
//
//   subset_mobius_transform(f);
//   // subset_zeta_transform の逆変換
//
//   superset_zeta_transform(f);
//   // f[S] = sum_{S subset T} original[T]
//
//   superset_mobius_transform(f);
//   // superset_zeta_transform の逆変換
//
// 注意:
//   - f.size() は 2 冪。
//   - T は +=, -= ができる型。
//   - 計算量 O(N log N), N = f.size()。

template <class T>
void subset_zeta_transform(vector<T> &f) {
    int n = (int)f.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    for (int bit = 1; bit < n; bit <<= 1) {
        for (int mask = 0; mask < n; mask++) {
            if (mask & bit) f[mask] += f[mask ^ bit];
        }
    }
}

template <class T>
void subset_mobius_transform(vector<T> &f) {
    int n = (int)f.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    for (int bit = 1; bit < n; bit <<= 1) {
        for (int mask = 0; mask < n; mask++) {
            if (mask & bit) f[mask] -= f[mask ^ bit];
        }
    }
}

template <class T>
void superset_zeta_transform(vector<T> &f) {
    int n = (int)f.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    for (int bit = 1; bit < n; bit <<= 1) {
        for (int mask = 0; mask < n; mask++) {
            if ((mask & bit) == 0) f[mask] += f[mask ^ bit];
        }
    }
}

template <class T>
void superset_mobius_transform(vector<T> &f) {
    int n = (int)f.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    for (int bit = 1; bit < n; bit <<= 1) {
        for (int mask = 0; mask < n; mask++) {
            if ((mask & bit) == 0) f[mask] -= f[mask ^ bit];
        }
    }
}
