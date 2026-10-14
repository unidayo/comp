// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/twelvefold_way/same_balls_dist_boxes_ge_one.cpp
// 写像12相 6.
// 区別なし玉 n 個 -> 区別あり箱 k 個、各箱少なくとも1個。
// 答え: C(n-1, k-1)
template <class mint>
mint same_balls_dist_boxes_ge_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (k == 0) return n == 0 ? mint(1) : mint(0);
    if (n < k) return 0;
    int r = std::min(k - 1, n - k);
    mint res = 1;
    for (int i = 1; i <= r; i++) {
        res *= n - 1 - r + i;
        res /= i;
    }
    return res;
}
