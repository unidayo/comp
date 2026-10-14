// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/geometry/arg_sort.cpp
// 反時計回りに偏角ソート（0° から開始、反時計回り）
// 注意: 原点 (0,0) は使用前に除外すること
bool cmp(pair<ll, ll> lhs, pair<ll, ll> rhs) {
    // 上半平面 (y > 0) または正のx軸 (y == 0 && x > 0) → ah = 1
    // 下半平面 (y < 0) または負のx軸 (y == 0 && x <= 0) → ah = 0
    int ah = (lhs.second > 0 || (lhs.second == 0 && lhs.first > 0));
    int bh = (rhs.second > 0 || (rhs.second == 0 && rhs.first > 0));
    if (ah != bh) return ah > bh;  // 上半平面を先に
    // 外積が正 → lhs が rhs より反時計回りで手前
    return (__int128)lhs.first * rhs.second - (__int128)lhs.second * rhs.first > 0;
}
