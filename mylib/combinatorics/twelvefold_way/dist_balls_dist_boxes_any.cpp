// 最終更新: 2026-07-12 14:27:44 JST
// 写像12相 1.
// 区別あり玉 n 個 -> 区別あり箱 k 個、制約なし。
// 答え: k^n
template <class mint>
mint dist_balls_dist_boxes_any(int n, int k) {
    assert(n >= 0 && k >= 0);
    mint a = k;
    mint res = 1;
    while (n > 0) {
        if (n & 1) res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}
