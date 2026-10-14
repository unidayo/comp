// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/twelvefold_way/same_balls_dist_boxes_le_one.cpp
// 写像12相 5.
// 区別なし玉 n 個 -> 区別あり箱 k 個、各箱高々1個。
// 答え: C(k, n)
template <class mint>
mint same_balls_dist_boxes_le_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (n > k) return 0;
    int r = std::min(n, k - n);
    mint res = 1;
    for (int i = 1; i <= r; i++) {
        res *= k - r + i;
        res /= i;
    }
    return res;
}
