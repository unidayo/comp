// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/combinatorics/twelvefold_way/dist_balls_same_boxes_le_one.cpp
// 写像12相 8.
// 区別あり玉 n 個 -> 区別なし箱 k 個、各箱高々1個。
// 答え: n <= k なら 1、そうでなければ 0
template <class mint>
mint dist_balls_same_boxes_le_one(int n, int k) {
    assert(n >= 0 && k >= 0);
    return n <= k ? mint(1) : mint(0);
}
