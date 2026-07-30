// 最終更新: 2026-07-12 14:27:44 JST
#pragma once

#include <cassert>
#include <vector>

/**
 * @brief Linear Imos — 累積和による区間への一次関数加算
 *
 * 2本の差分配列 da, db を用いる。
 *   da: 係数 a の差分配列
 *   db: 定数項 b の差分配列
 *
 * add(l, r, a, b) で区間 [l, r) の各インデックス i に a*i + b を加算し、
 * build() で累積和を取ることで A[i] = Σ(a*i + b) の寄与を O(N) で一括計算する。
 *
 * 計算量:
 *   add:   O(1)
 *   build: O(N)
 */
template <typename T>
struct LinearImos {
    int n;
    std::vector<T> da, db;

    explicit LinearImos(int n) : n(n), da(n + 1, T(0)), db(n + 1, T(0)) {}

    // ──────────────────────────── 更新 ────────────────────────────

    /// 区間 [l, r) の各 i に a*i + b を加算する（i はグローバルインデックス）
    void add(int l, int r, T a, T b) {
        assert(0 <= l && l <= r && r <= n);
        da[l] += a;
        da[r] -= a;
        db[l] += b;
        db[r] -= b;
    }

    /// 区間 [l, r) に等差数列を加算する
    /// A[l] += first, A[l+1] += first+d, A[l+2] += first+2d, ...
    void add_arith(int l, int r, T d, T first) {
        // a*(i - l) + first = a*i + (first - a*l) として add() に変換
        add(l, r, d, first - d * T(l));
    }

    // ──────────────────────────── 構築 ────────────────────────────

    /// 累積和を計算して結果配列を返す。
    /// 戻り値の res[i] が A[i] に対する加算済みの値。
    /// 初期値を持つ場合は init を渡す。
    std::vector<T> build(const std::vector<T>& init = {}) const {
        std::vector<T> res(n, T(0));
        if (!init.empty()) {
            assert((int)init.size() == n);
            res = init;
        }
        T ca = 0, cb = 0;
        for (int i = 0; i < n; i++) {
            ca += da[i];
            cb += db[i];
            res[i] += ca * T(i) + cb;
        }
        return res;
    }
};
