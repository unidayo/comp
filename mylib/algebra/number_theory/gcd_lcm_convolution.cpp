// GCD 畳み込み・LCM 畳み込み。
//
// 使い方:
//   vector<long long> a(n + 1), b(n + 1);  // a[0], b[0] は未使用
//   auto c = gcd_convolution(a, b);
//   // c[g] = sum_{gcd(i, j) = g} a[i] * b[j]
//
//   auto d = lcm_convolution(a, b);
//   // d[l] = sum_{lcm(i, j) = l} a[i] * b[j] ただし l <= n
//
// 注意:
//   - 1-indexed。0 番目は使わず、返り値の 0 番目も未使用。
//   - a.size() == b.size()。
//   - T は +=, -=, *= ができる型。
//   - 計算量 O(N log N), N = a.size() - 1。

template <class T>
void divisor_zeta_transform(vector<T> &f) {
    int n = (int)f.size() - 1;
    vector<T> g(n + 1);
    for (int d = 1; d <= n; d++) {
        for (int m = d; m <= n; m += d) {
            g[m] += f[d];
        }
    }
    f.swap(g);
}

vector<int> mobius_table(int n) {
    vector<int> mu(n + 1), primes;
    vector<bool> is_composite(n + 1);
    mu[1] = 1;
    for (int i = 2; i <= n; i++) {
        if (!is_composite[i]) {
            primes.push_back(i);
            mu[i] = -1;
        }
        for (int p : primes) {
            if (i * p > n) break;
            is_composite[i * p] = true;
            if (i % p == 0) {
                mu[i * p] = 0;
                break;
            }
            mu[i * p] = -mu[i];
        }
    }
    return mu;
}

template <class T>
void divisor_mobius_transform(vector<T> &f) {
    int n = (int)f.size() - 1;
    vector<int> mu = mobius_table(n);
    vector<T> g(n + 1);
    for (int d = 1; d <= n; d++) {
        for (int k = 1, m = d; m <= n; k++, m += d) {
            if (mu[k] == 1) g[m] += f[d];
            else if (mu[k] == -1) g[m] -= f[d];
        }
    }
    f.swap(g);
}

template <class T>
void multiple_zeta_transform(vector<T> &f) {
    int n = (int)f.size() - 1;
    vector<T> g(n + 1);
    for (int d = 1; d <= n; d++) {
        for (int m = d; m <= n; m += d) {
            g[d] += f[m];
        }
    }
    f.swap(g);
}

template <class T>
void multiple_mobius_transform(vector<T> &f) {
    int n = (int)f.size() - 1;
    vector<int> mu = mobius_table(n);
    vector<T> g(n + 1);
    for (int d = 1; d <= n; d++) {
        for (int k = 1, m = d; m <= n; k++, m += d) {
            if (mu[k] == 1) g[d] += f[m];
            else if (mu[k] == -1) g[d] -= f[m];
        }
    }
    f.swap(g);
}

template <class T>
vector<T> gcd_convolution(vector<T> a, vector<T> b) {
    assert(a.size() == b.size());
    assert(!a.empty());
    int n = (int)a.size() - 1;
    multiple_zeta_transform(a);
    multiple_zeta_transform(b);
    vector<T> c(n + 1);
    for (int i = 1; i <= n; i++) c[i] = a[i] * b[i];
    multiple_mobius_transform(c);
    return c;
}

template <class T>
vector<T> lcm_convolution(vector<T> a, vector<T> b) {
    assert(a.size() == b.size());
    assert(!a.empty());
    int n = (int)a.size() - 1;
    divisor_zeta_transform(a);
    divisor_zeta_transform(b);
    vector<T> c(n + 1);
    for (int i = 1; i <= n; i++) c[i] = a[i] * b[i];
    divisor_mobius_transform(c);
    return c;
}
