// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/assignment/hungarian.cpp
#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>

template <class T>
struct HungarianResult {
    T cost;
    std::vector<int> row_match;  // row_match[i] = matched column, or -1
    std::vector<int> col_match;  // col_match[j] = matched row, or -1
};

// 最小コスト割当。長方形の場合は小さい側をすべてマッチさせる。
// 計算量: O(min(H, W)^2 max(H, W))
template <class T>
HungarianResult<T> hungarian(const std::vector<std::vector<T>>& cost) {
    int h = (int)cost.size();
    int w = (h == 0 ? 0 : (int)cost[0].size());
    for (int i = 0; i < h; i++) assert((int)cost[i].size() == w);

    HungarianResult<T> res{T(), std::vector<int>(h, -1), std::vector<int>(w, -1)};
    if (h == 0 || w == 0) return res;

    bool transposed = h > w;
    int n = std::min(h, w), m = std::max(h, w);
    std::vector<std::vector<T>> a(n, std::vector<T>(m));
    if (!transposed) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) a[i][j] = cost[i][j];
    } else {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) a[i][j] = cost[j][i];
    }

    const T INF = std::numeric_limits<T>::max() / 4;
    std::vector<T> u(n + 1), v(m + 1), minv(m + 1);
    std::vector<int> p(m + 1), way(m + 1);

    for (int i = 1; i <= n; i++) {
        p[0] = i;
        int j0 = 0;
        std::fill(minv.begin(), minv.end(), INF);
        std::vector<char> used(m + 1, false);
        do {
            used[j0] = true;
            int i0 = p[j0], j1 = 0;
            T delta = INF;
            for (int j = 1; j <= m; j++) {
                if (used[j]) continue;
                T cur = a[i0 - 1][j - 1] - u[i0] - v[j];
                if (cur < minv[j]) {
                    minv[j] = cur;
                    way[j] = j0;
                }
                if (minv[j] < delta) {
                    delta = minv[j];
                    j1 = j;
                }
            }
            for (int j = 0; j <= m; j++) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);

        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0 != 0);
    }

    for (int j = 1; j <= m; j++) {
        if (p[j] == 0) continue;
        int i = p[j] - 1;
        int k = j - 1;
        if (!transposed) {
            res.row_match[i] = k;
            res.col_match[k] = i;
            res.cost += cost[i][k];
        } else {
            res.row_match[k] = i;
            res.col_match[i] = k;
            res.cost += cost[k][i];
        }
    }
    return res;
}

// 最大コスト割当。長方形の場合は小さい側をすべてマッチさせる。
// 計算量: O(min(H, W)^2 max(H, W))
template <class T>
HungarianResult<T> hungarian_max(const std::vector<std::vector<T>>& cost) {
    int h = (int)cost.size();
    int w = (h == 0 ? 0 : (int)cost[0].size());
    for (int i = 0; i < h; i++) assert((int)cost[i].size() == w);

    if (h == 0 || w == 0) {
        return HungarianResult<T>{T(), std::vector<int>(h, -1), std::vector<int>(w, -1)};
    }

    T mx = cost[0][0];
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) mx = std::max(mx, cost[i][j]);

    std::vector<std::vector<T>> converted(h, std::vector<T>(w));
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) converted[i][j] = mx - cost[i][j];

    auto res = hungarian(converted);
    res.cost = T();
    for (int i = 0; i < h; i++) {
        if (res.row_match[i] != -1) res.cost += cost[i][res.row_match[i]];
    }
    return res;
}
