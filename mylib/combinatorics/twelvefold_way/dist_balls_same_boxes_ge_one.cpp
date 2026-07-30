// 最終更新: 2026-07-12 14:27:44 JST
// 写像12相 9.
// 区別あり玉 n 個 -> 区別なし箱 k 個、各箱少なくとも1個。
// 答え: S(n, k)
// S(n, k) は第2種スターリング数。
template <class mint>
mint dist_balls_same_boxes_ge_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (k > n) return 0;
    std::vector<std::vector<mint>> s(n + 1, std::vector<mint>(k + 1));
    s[0][0] = 1;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= std::min(i, k); j++) {
            s[i][j] = s[i - 1][j - 1] + s[i - 1][j] * j;
        }
    }
    return s[n][k];
}

// 包除原理版。
// 答え: S(n, k) = 1/k! * sum_{i=0}^k (-1)^i C(k, i) (k-i)^n
template <class mint>
mint dist_balls_same_boxes_ge_one_inclusion_exclusion(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (k > n) return 0;
    auto pow_mint = [](mint a, long long e) {
        mint res = 1;
        while (e > 0) {
            if (e & 1) res *= a;
            a *= a;
            e >>= 1;
        }
        return res;
    };

    mint comb = 1;
    mint res = 0;
    for (int i = 0; i <= k; i++) {
        mint term = comb * pow_mint(mint(k - i), n);
        if (i & 1) res -= term;
        else res += term;
        if (i < k) {
            comb *= k - i;
            comb /= i + 1;
        }
    }
    for (int i = 1; i <= k; i++) res /= i;
    return res;
}
