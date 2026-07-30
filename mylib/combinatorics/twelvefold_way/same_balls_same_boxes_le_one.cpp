// 最終更新: 2026-07-12 14:27:44 JST
// 写像12相 11.
// 区別なし玉 n 個 -> 区別なし箱 k 個、各箱高々1個。
// 答え: n <= k なら 1、そうでなければ 0
template <class mint>
mint same_balls_same_boxes_le_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    return n <= k ? mint(1) : mint(0);
}
