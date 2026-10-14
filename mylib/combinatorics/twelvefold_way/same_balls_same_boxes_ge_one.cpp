// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/twelvefold_way/same_balls_same_boxes_ge_one.cpp
// 写像12相 12.
// 区別なし玉 n 個 -> 区別なし箱 k 個、各箱少なくとも1個。
// 答え: n をちょうど k 個に分割する数。
template <class mint>
mint same_balls_same_boxes_ge_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (k > n) return 0;
    std::vector<std::vector<mint>> dp(n + 1, std::vector<mint>(k + 1));
    dp[0][0] = 1;
    for (int sum = 1; sum <= n; sum++) {
        for (int parts = 1; parts <= std::min(sum, k); parts++) {
            dp[sum][parts] = dp[sum - 1][parts - 1];
            if (sum >= parts) dp[sum][parts] += dp[sum - parts][parts];
        }
    }
    return dp[n][k];
}
