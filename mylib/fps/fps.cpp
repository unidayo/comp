using mint = modint998244353;

// 形式的冪級数ライブラリ (mod 998244353)
// ACL の convolution を使用

struct FPS : vector<mint> {
    using vector<mint>::vector;

    FPS(const vector<mint>& v) : vector<mint>(v) {}

    // サイズを n に縮小/拡張
    void shrink() {
        while (!empty() && back() == mint(0)) pop_back();
    }

    FPS pre(int n) const {
        FPS res(begin(), begin() + min((int)size(), n));
        res.resize(n);
        return res;
    }

    // 加算
    FPS operator+(const FPS& r) const {
        FPS res = *this;
        if (res.size() < r.size()) res.resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) res[i] += r[i];
        return res;
    }

    // 減算
    FPS operator-(const FPS& r) const {
        FPS res = *this;
        if (res.size() < r.size()) res.resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) res[i] -= r[i];
        return res;
    }

    // 乗算 (ACL convolution)
    FPS operator*(const FPS& r) const {
        if (empty() || r.empty()) return FPS();
        auto res = convolution(*this, r);
        return FPS(res);
    }

    FPS operator*(mint c) const {
        FPS res = *this;
        for (auto& x : res) x *= c;
        return res;
    }

    FPS& operator+=(const FPS& r) { return *this = *this + r; }
    FPS& operator-=(const FPS& r) { return *this = *this - r; }
    FPS& operator*=(const FPS& r) { return *this = *this * r; }
    FPS& operator*=(mint c) {
        for (auto& x : *this) x *= c;
        return *this;
    }

    // 微分
    FPS diff() const {
        if (size() <= 1) return FPS();
        FPS res(size() - 1);
        for (int i = 1; i < (int)size(); i++) {
            res[i - 1] = (*this)[i] * mint(i);
        }
        return res;
    }

    // 積分
    FPS integral() const {
        FPS res(size() + 1);
        res[0] = 0;
        for (int i = 0; i < (int)size(); i++) {
            res[i + 1] = (*this)[i] / mint(i + 1);
        }
        return res;
    }

    // 逆元 f^{-1} mod x^n (Newton法)
    // f[0] != 0 が必要
    FPS inv(int n) const {
        assert(!empty() && (*this)[0] != mint(0));
        FPS res = {(*this)[0].inv()};
        for (int k = 1; k < n; k <<= 1) {
            FPS f = pre(2 * k);
            FPS g = res;
            f.resize(2 * k);
            g.resize(2 * k);
            FPS fg = f * g;
            fg.resize(2 * k);
            for (auto& x : fg) x = -x;
            fg[0] += mint(2);
            res = (g * fg).pre(2 * k);
        }
        res.resize(n);
        return res;
    }

    // log f(x) mod x^n
    // f[0] = 1 が必要
    FPS log(int n) const {
        assert(!empty() && (*this)[0] == mint(1));
        FPS res = (diff() * inv(n)).pre(n - 1).integral();
        res.resize(n);
        return res;
    }

    // exp f(x) mod x^n (Newton法)
    // f[0] = 0 が必要
    FPS exp(int n) const {
        assert(empty() || (*this)[0] == mint(0));
        FPS res = {mint(1)};
        for (int k = 1; k < n; k <<= 1) {
            FPS f = pre(2 * k);
            FPS g = res.log(2 * k);
            for (int i = 0; i < 2 * k; i++) {
                g[i] = (i < (int)f.size() ? f[i] : mint(0)) - g[i];
            }
            g[0] += mint(1);
            res = (res * g).pre(2 * k);
        }
        res.resize(n);
        return res;
    }

    // f(x)^m mod x^n
    // a_0 = 1 の場合: exp(m * log(f))
    // a_0 != 1 の場合: x^k を括り出して計算
    FPS pow(long long m, int n) const {
        if (m == 0) {
            FPS res(n);
            if (n > 0) res[0] = mint(1);
            return res;
        }

        // 最初の非零項を見つける
        int k = 0;
        while (k < (int)size() && (*this)[k] == mint(0)) k++;

        // 全部 0
        if (k == (int)size()) return FPS(n);

        // x^k を括り出すと x^{k*m} になる
        // k*m >= n なら結果は 0
        if (k > 0 && (m >= (n + k - 1) / k || k * m >= n)) {
            return FPS(n);
        }

        long long km = k * m;
        int new_n = n - km;

        // f(x) = x^k * g(x) where g[0] != 0
        // f(x)^m = x^{km} * g(x)^m
        // g(x)^m = g[0]^m * (g(x)/g[0])^m = g[0]^m * exp(m * log(g(x)/g[0]))

        mint a0 = (*this)[k];
        mint a0_inv = a0.inv();
        mint a0_m = a0.pow(m);

        // h(x) = g(x) / g[0]  (h[0] = 1)
        FPS h(new_n);
        for (int i = 0; i < new_n && k + i < (int)size(); i++) {
            h[i] = (*this)[k + i] * a0_inv;
        }

        // h(x)^m = exp(m * log(h))
        // m が大きいので m mod (MOD-1) ではなく、log の係数に m をかける
        FPS log_h = h.log(new_n);
        mint m_mod = mint(m);
        for (auto& x : log_h) x *= m_mod;
        FPS h_m = log_h.exp(new_n);

        // g(x)^m = a0^m * h(x)^m
        for (auto& x : h_m) x *= a0_m;

        // f(x)^m = x^{km} * g(x)^m
        FPS res(n);
        for (int i = 0; i < new_n; i++) {
            res[km + i] = h_m[i];
        }
        return res;
    }
};