// 最終更新: 2026-07-12 14:27:44 JST
// 写像12相 2.
// 区別あり玉 n 個 -> 区別あり箱 k 個、各箱高々1個。
// 答え: kPn
template <class mint>
mint dist_balls_dist_boxes_le_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    if (n > k) return 0;
    mint res = 1;
    for (int i = 0; i < n; i++) res *= k - i;
    return res;
}
