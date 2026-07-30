/* grid */
// d=0:右, 1:下, 2:左, 3:上 の移動量
[[maybe_unused]] constexpr int dr4[] = {0, 1, 0, -1};
[[maybe_unused]] constexpr int dc4[] = {1, 0, -1, 0};
// d=0から時計回り: 右, 右下, 下, 左下, 左, 左上, 上, 右上
[[maybe_unused]] constexpr int dr8[] = {0, 1, 1, 1, 0, -1, -1, -1};
[[maybe_unused]] constexpr int dc8[] = {1, 1, 0, -1, -1, -1, 0, 1};
// グリッド(h行w列)の範囲内かチェック
bool grid_check(ll row, ll col, ll h, ll w) { return 0 <= row && row < h && 0 <= col && col < w; }

// 2次元コンテナを時計回りに90度回転したものを返す。
// vector<string>, vector<vector<T>> など、Grid[row][col] でアクセスできる矩形を想定。
template <class Grid>
Grid rotate90_clockwise(const Grid &a) {
    if (a.empty() || a[0].empty()) return Grid{};
    using Row = ranges::range_value_t<Grid>;
    using Cell = ranges::range_value_t<Row>;
    const int h = int(a.size());
    const int w = int(a[0].size());
    Grid b(w, Row(h, Cell{}));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            b[j][h - 1 - i] = a[i][j];
        }
    }
    return b;
}

// 2次元コンテナを反時計回りに90度回転したものを返す。
template <class Grid>
Grid rotate90_counterclockwise(const Grid &a) {
    if (a.empty() || a[0].empty()) return Grid{};
    using Row = ranges::range_value_t<Grid>;
    using Cell = ranges::range_value_t<Row>;
    const int h = int(a.size());
    const int w = int(a[0].size());
    Grid b(w, Row(h, Cell{}));
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            b[w - 1 - j][i] = a[i][j];
        }
    }
    return b;
}

template <class Grid>
Grid rotate90(const Grid &a) {
    return rotate90_clockwise(a);
}
