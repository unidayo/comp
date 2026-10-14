// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/twelvefold_way/dist_balls_dist_boxes_ge_one.cpp
// 写像12相 3.
// 区別あり玉 n 個 -> 区別あり箱 k 個、各箱少なくとも1個。
// 答え: k! * S(n, k)
// S(n, k) は第2種スターリング数。
template <class mint>
mint dist_balls_dist_boxes_ge_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (k > n) return 0;
    std::vector<std::vector<mint>> s(n + 1, std::vector<mint>(k + 1));
    s[0][0] = 1;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= std::min(i, k); j++) {
            s[i][j] = s[i - 1][j - 1] + s[i - 1][j] * j;
        }
    }
    mint fact = 1;
    for (int i = 1; i <= k; i++) fact *= i;
    return fact * s[n][k];
}

// 包除原理版。
// 答え: sum_{i=0}^k (-1)^i C(k, i) (k-i)^n
// 空の箱集合を包除する。
template <class mint>
mint dist_balls_dist_boxes_ge_one_inclusion_exclusion(int n, int k) {
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
    return res;
}
