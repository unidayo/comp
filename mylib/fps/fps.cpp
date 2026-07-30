// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/acl/convolution.cpp
using mint = modint998244353;

// 形式的冪級数ライブラリ (mod 998244353)
// ACL の convolution を使用

struct FPS : vector<mint> {
    using vector<mint>::vector;

    FPS(const vector<mint> &v) : vector<mint>(v) {}

  private:
    static const vector<mint> &inverse_table(int n) {
        static vector<mint> inverse = {mint(0), mint(1)};
        assert(n < mint::mod());
        if ((int)inverse.size() <= n) {
            int old_size = inverse.size();
            inverse.resize(n + 1);
            for (int i = max(2, old_size); i <= n; ++i) {
                inverse[i] = -mint(mint::mod() / i) * inverse[mint::mod() % i];
            }
        }
        return inverse;
    }

    static optional<mint> modular_sqrt(mint a) {
        if (a == mint(0)) return mint(0);
        int mod = mint::mod();
        if (a.pow((mod - 1) / 2) != mint(1)) return nullopt;
        if (mod % 4 == 3) {
            mint result = a.pow((mod + 1) / 4);
            if (result.val() > mod - result.val()) result = -result;
            return result;
        }
        int s = 0;
        int q = mod - 1;
        while ((q & 1) == 0) q >>= 1, ++s;
        mint z = 2;
        while (z.pow((mod - 1) / 2) == mint(1)) z += 1;
        mint c = z.pow(q);
        mint x = a.pow((q + 1) / 2);
        mint t = a.pow(q);
        int m = s;
        while (t != mint(1)) {
            int i = 1;
            mint squared = t * t;
            while (squared != mint(1)) squared *= squared, ++i;
            mint b = c.pow(1LL << (m - i - 1));
            x *= b;
            c = b * b;
            t *= c;
            m = i;
        }
        if (x.val() > mod - x.val()) x = -x;
        return x;
    }

  public:

    // サイズを n に縮小/拡張
    void shrink() {
        while (!empty() && back() == mint(0)) pop_back();
    }

    FPS pre(int n) const {
        assert(n >= 0);
        FPS res(begin(), begin() + min((int)size(), n));
        res.resize(n);
        return res;
    }

    // 加算
    FPS operator+(const FPS &r) const {
        FPS res = *this;
        if (res.size() < r.size()) res.resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) res[i] += r[i];
        return res;
    }

    // 減算
    FPS operator-(const FPS &r) const {
        FPS res = *this;
        if (res.size() < r.size()) res.resize(r.size());
        for (int i = 0; i < (int)r.size(); i++) res[i] -= r[i];
        return res;
    }

    // 乗算 (ACL convolution)
    FPS operator*(const FPS &r) const {
        if (empty() || r.empty()) return FPS();
        auto res = convolution(*this, r);
        return FPS(res);
    }

    FPS operator*(mint c) const {
        FPS res = *this;
        for (auto &x : res) x *= c;
        return res;
    }

    FPS &operator+=(const FPS &r) {
        if (size() < r.size()) resize(r.size());
        for (int i = 0; i < (int)r.size(); ++i) (*this)[i] += r[i];
        return *this;
    }
    FPS &operator-=(const FPS &r) {
        if (size() < r.size()) resize(r.size());
        for (int i = 0; i < (int)r.size(); ++i) (*this)[i] -= r[i];
        return *this;
    }
    FPS &operator*=(const FPS &r) { return *this = *this * r; }
    FPS &operator*=(mint c) {
        for (auto &x : *this) x *= c;
        return *this;
    }

    // 疎な多項式との乗算 O(n * |G|)
    // G は {(次数, 係数)} のリスト（次数昇順）
    FPS sparse_mul(const vector<pair<int, mint>> &g) const {
        int n = size();
        FPS res(n);
        for (auto [degree, coefficient] : g) {
            if (degree >= n) break;
            for (int i = 0; i + degree < n; ++i) {
                res[i + degree] += (*this)[i] * coefficient;
            }
        }
        return res;
    }

    // 疎な多項式での除算 O(n * |G|)
    // F(x) / G(x) mod x^n を計算
    // G は {(次数, 係数)} のリスト（次数昇順）, g[0] != 0 が必要
    // Q[i] = (F[i] - Σ_{(d,c)∈G, d≥1} c·Q[i-d]) / g0
    FPS sparse_div(const vector<pair<int, mint>> &g, int n = -1) const {
        assert(!g.empty() && g[0].first == 0 && g[0].second != mint(0));
        if (n < 0) n = size();
        mint inv0 = g[0].second.inv();
        FPS res(n);
        for (int i = 0; i < n; i++) {
            mint s = (i < (int)size()) ? (*this)[i] : mint(0);
            for (auto &[d, c] : g) {
                if (d == 0) continue;
                if (d > i) break;
                s -= c * res[i - d];
            }
            res[i] = s * inv0;
        }
        return res;
    }

    // FPS から疎表現 {(次数, 係数)} を生成するヘルパー
    static vector<pair<int, mint>> to_sparse(const FPS &f) {
        vector<pair<int, mint>> res;
        for (int i = 0; i < (int)f.size(); i++) {
            if (f[i] != mint(0)) res.emplace_back(i, f[i]);
        }
        return res;
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
        const auto &inverse = inverse_table(size());
        for (int i = 0; i < (int)size(); i++) {
            res[i + 1] = (*this)[i] * inverse[i + 1];
        }
        return res;
    }

    // 逆元 f^{-1} mod x^n (Newton法)
    // f[0] != 0 が必要
    FPS inv(int n) const {
        assert(n >= 0);
        assert(!empty() && (*this)[0] != mint(0));
        FPS res = {(*this)[0].inv()};
        while ((int)res.size() < n) {
            int next_size = min(2 * (int)res.size(), n);
            FPS f = pre(next_size);
            FPS g = res;
            g.resize(next_size);
            FPS fg = f * g;
            fg.resize(next_size);
            for (auto &x : fg) x = -x;
            fg[0] += mint(2);
            res = (g * fg).pre(next_size);
        }
        res.resize(n);
        return res;
    }

    // log f(x) mod x^n
    // f[0] = 1 が必要
    FPS log(int n) const {
        assert(n >= 0);
        assert(!empty() && (*this)[0] == mint(1));
        if (n == 0) return FPS();
        FPS res = (diff() * inv(n)).pre(n - 1).integral();
        res.resize(n);
        return res;
    }

    // exp f(x) mod x^n (Newton法)
    // f[0] = 0 が必要
    FPS exp(int n) const {
        assert(n >= 0);
        assert(empty() || (*this)[0] == mint(0));
        FPS res = {mint(1)};
        while ((int)res.size() < n) {
            int next_size = min(2 * (int)res.size(), n);
            FPS f = pre(next_size);
            FPS g = res.log(next_size);
            for (int i = 0; i < next_size; i++) {
                g[i] = (i < (int)f.size() ? f[i] : mint(0)) - g[i];
            }
            g[0] += mint(1);
            res = (res * g).pre(next_size);
        }
        res.resize(n);
        return res;
    }

    // f(x) の形式的平方根 mod x^n。存在しなければ nullopt。
    optional<FPS> sqrt(int n) const {
        assert(n >= 0);
        if (n == 0) return FPS();
        int first = 0;
        while (first < (int)size() && (*this)[first] == mint(0)) ++first;
        if (first == (int)size()) return FPS(n);
        if (first & 1) return nullopt;

        int shift = first / 2;
        if (shift >= n) return FPS(n);
        auto constant = modular_sqrt((*this)[first]);
        if (!constant) return nullopt;

        int required = n - shift;
        FPS normalized(required);
        for (int i = 0; i < required && first + i < (int)size(); ++i) {
            normalized[i] = (*this)[first + i];
        }

        FPS result = {*constant};
        const mint inverse_two = mint(2).inv();
        for (int length = 1; length < required; length <<= 1) {
            int next_length = min(2 * length, required);
            FPS quotient = (normalized.pre(next_length) * result.inv(next_length)).pre(next_length);
            result.resize(next_length);
            for (int i = 0; i < next_length; ++i) result[i] = (result[i] + quotient[i]) * inverse_two;
        }

        FPS answer(n);
        for (int i = 0; i < required; ++i) answer[shift + i] = result[i];
        return answer;
    }

    // 1点評価。O(N)。
    mint eval(mint x) const {
        mint result = 0;
        for (int i = (int)size() - 1; i >= 0; --i) result = result * x + (*this)[i];
        return result;
    }

    // 形式的級数として *this / denominator mod x^n を返す。
    FPS div_series(const FPS &denominator, int n) const {
        assert(n >= 0);
        assert(!denominator.empty() && denominator[0] != mint(0));
        return (pre(n) * denominator.inv(n)).pre(n);
    }

    // 多項式f(x+c)。O(M(N))。
    FPS taylor_shift(mint c) const {
        int n = size();
        if (n == 0) return FPS();
        const auto &inverse = inverse_table(n);
        vector<mint> factorial(n, 1), inverse_factorial(n, 1);
        for (int i = 1; i < n; ++i) {
            factorial[i] = factorial[i - 1] * i;
            inverse_factorial[i] = inverse_factorial[i - 1] * inverse[i];
        }
        FPS left(n), right(n);
        for (int i = 0; i < n; ++i) left[n - 1 - i] = (*this)[i] * factorial[i];
        mint power = 1;
        for (int i = 0; i < n; ++i) {
            right[i] = power * inverse_factorial[i];
            power *= c;
        }
        FPS product = left * right;
        FPS result(n);
        for (int i = 0; i < n; ++i) result[i] = product[n - 1 - i] * inverse_factorial[i];
        return result;
    }

    // 多項式としての商と余り。O(M(N))。
    pair<FPS, FPS> div_mod(FPS denominator) const {
        FPS numerator = *this;
        numerator.shrink();
        denominator.shrink();
        assert(!denominator.empty());
        if (numerator.size() < denominator.size()) return {FPS(), numerator};

        int quotient_size = numerator.size() - denominator.size() + 1;
        FPS reversed_numerator = numerator;
        FPS reversed_denominator = denominator;
        reverse(reversed_numerator.begin(), reversed_numerator.end());
        reverse(reversed_denominator.begin(), reversed_denominator.end());
        FPS quotient = (reversed_numerator.pre(quotient_size) * reversed_denominator.inv(quotient_size)).pre(quotient_size);
        reverse(quotient.begin(), quotient.end());
        FPS remainder = numerator - denominator * quotient;
        remainder.resize(min<int>(remainder.size(), denominator.size() - 1));
        remainder.shrink();
        return {quotient, remainder};
    }

    // f(x)^m mod x^n
    // a_0 = 1 の場合: exp(m * log(f))
    // a_0 != 1 の場合: x^k を括り出して計算
    FPS pow(long long m, int n) const {
        assert(m >= 0 && n >= 0);
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
        for (auto &x : log_h) x *= m_mod;
        FPS h_m = log_h.exp(new_n);

        // g(x)^m = a0^m * h(x)^m
        for (auto &x : h_m) x *= a0_m;

        // f(x)^m = x^{km} * g(x)^m
        FPS res(n);
        for (int i = 0; i < new_n; i++) {
            res[km + i] = h_m[i];
        }
        return res;
    }
};
