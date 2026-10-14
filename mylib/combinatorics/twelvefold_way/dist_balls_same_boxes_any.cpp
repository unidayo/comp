// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/twelvefold_way/dist_balls_same_boxes_any.cpp
// 写像12相 7.
// 区別あり玉 n 個 -> 区別なし箱 k 個、制約なし。
// 答え: sum_{i=0}^k S(n, i)
// S(n, i) は第2種スターリング数。
template <class mint>
mint dist_balls_same_boxes_any(int n, int k) {
    assert(n >= 0 && k >= 0);
    std::vector<std::vector<mint>> s(n + 1, std::vector<mint>(k + 1));
    s[0][0] = 1;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= std::min(i, k); j++) {
            s[i][j] = s[i - 1][j - 1] + s[i - 1][j] * j;
        }
    }
    mint res = 0;
    for (int i = 0; i <= k; i++) res += s[n][i];
    return res;
}

// 包除原理版。
// 答え: sum_{j=0}^k S(n, j)
// k >= n ならベル数 B_n。
// 各 S(n, j) を包除原理で計算する。
template <class mint>
mint dist_balls_same_boxes_any_inclusion_exclusion(int n, int k) {
    assert(n >= 0 && k >= 0);
    auto pow_mint = [](mint a, long long e) {
        mint res = 1;
        while (e > 0) {
            if (e & 1) res *= a;
            a *= a;
            e >>= 1;
        }
        return res;
    };

    mint ans = 0;
    for (int boxes = 0; boxes <= std::min(n, k); boxes++) {
        mint comb = 1;
        mint ways = 0;
        for (int i = 0; i <= boxes; i++) {
            mint term = comb * pow_mint(mint(boxes - i), n);
            if (i & 1) ways -= term;
            else ways += term;
            if (i < boxes) {
                comb *= boxes - i;
                comb /= i + 1;
            }
        }
        for (int i = 1; i <= boxes; i++) ways /= i;
        ans += ways;
    }
    return ans;
}
