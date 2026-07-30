// 最終更新: 2026-07-12 14:27:44 JST
// 写像12相 10.
// 区別なし玉 n 個 -> 区別なし箱 k 個、制約なし。
// 答え: n を高々 k 個に分割する数。
template <class mint>
mint same_balls_same_boxes_any(int n, int k) {
    assert(n >= 0 && k >= 0);
    std::vector<mint> dp(n + 1);
    dp[0] = 1;
    for (int part = 1; part <= k; part++) {
        for (int sum = part; sum <= n; sum++) {
            dp[sum] += dp[sum - part];
        }
    }
    return dp[n];
}
