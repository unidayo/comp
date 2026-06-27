// XOR 畳み込み。
//
// 使い方:
//   auto c = xor_convolution(a, b);
//   // c[S] = sum_{X ^ Y = S} a[X] * b[Y]
//
// 注意:
//   - a.size() == b.size() かつ 2 冪。
//   - T は +=, -=, *=, /= int ができる型。
//   - modint でも使える。
//   - 計算量 O(N log N), N = a.size()。

template <class T>
void xor_walsh_hadamard_transform(vector<T> &f, bool inverse = false) {
    int n = (int)f.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    for (int len = 1; len < n; len <<= 1) {
        for (int l = 0; l < n; l += len << 1) {
            for (int i = 0; i < len; i++) {
                T x = f[l + i];
                T y = f[l + len + i];
                f[l + i] = x + y;
                f[l + len + i] = x - y;
            }
        }
    }
    if (inverse) {
        for (T &x : f) x /= n;
    }
}

template <class T>
vector<T> xor_convolution(vector<T> a, vector<T> b) {
    assert(a.size() == b.size());
    int n = (int)a.size();
    assert(n > 0 && (n & (n - 1)) == 0);
    xor_walsh_hadamard_transform(a);
    xor_walsh_hadamard_transform(b);
    for (int i = 0; i < n; i++) a[i] *= b[i];
    xor_walsh_hadamard_transform(a, true);
    return a;
}
