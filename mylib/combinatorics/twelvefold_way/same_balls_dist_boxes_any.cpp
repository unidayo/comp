// 最終更新: 2026-07-12 14:27:44 JST
// 写像12相 4.
// 区別なし玉 n 個 -> 区別あり箱 k 個、制約なし。
// 答え: C(n+k-1, k-1)
template <class mint>
mint same_balls_dist_boxes_any(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (k == 0) return n == 0 ? mint(1) : mint(0);
    int r = std::min(k - 1, n);
    mint res = 1;
    for (int i = 1; i <= r; i++) {
        res *= n + k - i;
        res /= i;
    }
    return res;
}
