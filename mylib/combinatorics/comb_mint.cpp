// 最終更新: 2026-10-14 18:14:44 JST
// 依存: mylib/acl/modint.cpp
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/comb_mint.cpp
using mint = modint998244353;

class Combination {
   private:
    std::vector<mint> fac, ifac;

    void extend(int n) {
        if (n < (int)fac.size()) return;
        int old_size = fac.size();
        fac.resize(n + 1);
        ifac.resize(n + 1);
        for (int i = old_size; i <= n; i++) {
            fac[i] = fac[i - 1] * i;
        }
        ifac[n] = fac[n].inv();
        for (int i = n; i >= old_size; i--) {
            ifac[i - 1] = ifac[i] * i;
        }
    }

   public:
    Combination(int n = 1) : fac(n + 1), ifac(n + 1) {
        fac[0] = 1;
        for (int i = 0; i < n; i++) {
            fac[i + 1] = fac[i] * (i + 1);
        }
        ifac[n] = fac[n].inv();
        for (int i = n; i > 0; i--) {
            ifac[i - 1] = ifac[i] * i;
        }
    }

    mint C(int n, int r) {
        if (r < 0 || r > n) return 0;
        extend(n);
        return fac[n] * ifac[r] * ifac[n - r];
    }

    mint P(int n, int r) {
        if (r < 0 || r > n) return 0;
        extend(n);
        return fac[n] * ifac[n - r];
    }

    mint H(int n, int r) {  // 重複組合せ
        if (n == 0 && r == 0) return 1;
        return C(n + r - 1, r);
    }

    mint factorial(int n) {
        extend(n);
        return fac[n];
    }

    mint inv_factorial(int n) {
        extend(n);
        return ifac[n];
    }
};
