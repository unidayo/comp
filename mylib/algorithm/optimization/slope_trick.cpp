// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/optimization/slope_trick.cpp
#pragma once

#include <algorithm>
#include <cassert>
#include <limits>
#include <queue>
#include <vector>

/**
 * @brief Slope Trick — 区分線形凸関数を効率的に管理するデータ構造
 *
 * 関数 f(x) を「最小値」と「左右の傾きの変化点（breakpoint）」で表現する。
 *   - L (max-heap): 傾きが減少する変化点群（左側、f が右下がりの区間）
 *   - R (min-heap): 傾きが増加する変化点群（右側、f が右上がりの区間）
 *
 * 遅延加算 (add_l, add_r) により、全変化点の一括シフトを O(1) で実現する。
 *
 * @see https://maspypy.com/slope-trick-1-%E8%A7%A3%E8%AA%AC%E7%B7%A8
 */
template <typename T>
struct SlopeTrick {
    static constexpr T INF = std::numeric_limits<T>::max() / 3;

    SlopeTrick() = default;

    // ──────────────────────────── クエリ ────────────────────────────

    struct Minimum {
        T left;   // min f を達成する区間の左端
        T right;  // min f を達成する区間の右端
        T value;  // min f の値
    };

    /// f の最小値と、それを達成する区間 [left, right] を返す
    Minimum query() const { return {top_left(), top_right(), min_f_}; }

    // ──────────────────── 関数の加算・変形操作 ────────────────────

    /// f(x) += constant
    void add_constant(T c) { min_f_ += c; }

    /// f(x) += max(a - x, 0)    （左下がり: ＼＿ の形）
    void add_left_ramp(T a) {
        min_f_ += std::max(T(0), a - top_right());
        push_right(a);
        push_left(pop_right());
    }

    /// f(x) += max(x - a, 0)    （右上がり: ＿／ の形）
    void add_right_ramp(T a) {
        min_f_ += std::max(T(0), top_left() - a);
        push_left(a);
        push_right(pop_left());
    }

    /// f(x) += |x - a|          （V字: ＼／ の形）
    void add_abs(T a) {
        add_left_ramp(a);
        add_right_ramp(a);
    }

    // ──────────────── 累積 min による平坦化 ────────────────

    /// f(x) = min_{y <= x} f(y)  （右側を平坦化: ＼＿ の形にする）
    void prefix_min() {
        while (!right_heap_.empty()) right_heap_.pop();
    }

    /// f(x) = min_{y >= x} f(y)  （左側を平坦化: ＿／ の形にする）
    void suffix_min() {
        while (!left_heap_.empty()) left_heap_.pop();
    }

    // ──────────────────── スライド min ────────────────────

    /// f(x) = min_{x-b <= y <= x-a} f(y)   (a <= b)
    /// 左の変化点を a、右の変化点を b だけシフトする
    void slide_min(T a, T b) {
        assert(a <= b);
        lazy_left_ += a;
        lazy_right_ += b;
    }

    /// f(x) = f(x - a)   （グラフ全体を右に a だけ平行移動）
    void translate(T a) { slide_min(a, a); }

    // ──────────────── 点 x での値の取得（破壊的） ────────────────

    /// f(x) を計算して返す。内部状態（ヒープ）は破壊される。
    T evaluate_destructive(T x) {
        T result = min_f_;
        while (!left_heap_.empty()) result += std::max(T(0), pop_left() - x);
        while (!right_heap_.empty()) result += std::max(T(0), x - pop_right());
        return result;
    }

    // ──────────────── 他の SlopeTrick との合成 ────────────────

    /// this に other を加算する（other は破壊される）
    void merge(SlopeTrick& other) {
        // 小さい方を大きい方にマージする（マージテク）
        if (other.size() > size()) {
            std::swap(left_heap_, other.left_heap_);
            std::swap(right_heap_, other.right_heap_);
            std::swap(lazy_left_, other.lazy_left_);
            std::swap(lazy_right_, other.lazy_right_);
            std::swap(min_f_, other.min_f_);
        }
        while (!other.right_heap_.empty()) add_right_ramp(other.pop_right());
        while (!other.left_heap_.empty()) add_left_ramp(other.pop_left());
        min_f_ += other.min_f_;
    }

   private:
    T min_f_ = 0;       // f の最小値
    T lazy_left_ = 0;   // 左ヒープへの遅延加算
    T lazy_right_ = 0;  // 右ヒープへの遅延加算

    std::priority_queue<T, std::vector<T>, std::less<>> left_heap_;      // max-heap
    std::priority_queue<T, std::vector<T>, std::greater<>> right_heap_;  // min-heap

    // ── 右ヒープ操作（遅延加算を考慮） ──

    void push_right(T val) { right_heap_.push(val - lazy_right_); }

    T top_right() const { return right_heap_.empty() ? INF : right_heap_.top() + lazy_right_; }

    T pop_right() {
        T val = top_right();
        if (!right_heap_.empty()) right_heap_.pop();
        return val;
    }

    // ── 左ヒープ操作（遅延加算を考慮） ──

    void push_left(T val) { left_heap_.push(val - lazy_left_); }

    T top_left() const { return left_heap_.empty() ? -INF : left_heap_.top() + lazy_left_; }

    T pop_left() {
        T val = top_left();
        if (!left_heap_.empty()) left_heap_.pop();
        return val;
    }

    // ── ユーティリティ ──

    std::size_t size() const { return left_heap_.size() + right_heap_.size(); }
};
