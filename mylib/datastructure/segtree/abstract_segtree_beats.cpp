// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/abstract_segtree_beats.cpp
#pragma once
#include <cassert>
#include <optional>
#include <stdexcept>
#include <vector>

// ACL 風 Segment Tree Beats。mapping(F, S) は std::optional<S> を返す。
// 更新可能なら更新後の S、情報不足なら std::nullopt を返す。
// 葉・空区間・恒等作用への mapping は必ず成功させる。
// composition(f, g) は g の後に f。計算量の償却解析は具体的な作用ごとに必要。
template <class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
class AbstractSegTreeBeats {
    int n_ = 0, base_ = 1;
    std::vector<S> data_;
    std::vector<F> lazy_;
    std::vector<bool> pending_;

    void pull(int k) { data_[k] = op(data_[2 * k], data_[2 * k + 1]); }

    void apply_node(int k, const F& f) {
        std::optional<S> next = mapping(f, data_[k]);
        if (next) {
            data_[k] = *next;
            if (k < base_) {
                lazy_[k] = pending_[k] ? composition(f, lazy_[k]) : f;
                pending_[k] = true;
            }
            return;
        }
        if (k >= base_) throw std::logic_error("Segment Tree Beats: mapping failed at a leaf");
        push(k);
        apply_node(2 * k, f);
        apply_node(2 * k + 1, f);
        pull(k);
    }

    void push(int k) {
        if (k >= base_ || !pending_[k]) return;
        F f = lazy_[k];
        pending_[k] = false;
        lazy_[k] = id();
        // 合成済みの作用が子で失敗する場合も apply_node が処理する。
        apply_node(2 * k, f);
        apply_node(2 * k + 1, f);
    }

    void set_impl(int p, const S& x, int k, int l, int r) {
        if (r - l == 1) {
            data_[k] = x;
            return;
        }
        push(k);
        int m = l + (r - l) / 2;
        if (p < m) set_impl(p, x, 2 * k, l, m);
        else set_impl(p, x, 2 * k + 1, m, r);
        pull(k);
    }

    S prod_impl(int a, int b, int k, int l, int r) {
        if (b <= l || r <= a) return e();
        if (a <= l && r <= b) return data_[k];
        push(k);
        int m = l + (r - l) / 2;
        // op の引数は常に左から右。可換性は不要。
        S left = prod_impl(a, b, 2 * k, l, m);
        S right = prod_impl(a, b, 2 * k + 1, m, r);
        return op(left, right);
    }

    void apply_impl(int a, int b, const F& f, int k, int l, int r) {
        if (b <= l || r <= a) return;
        if (a <= l && r <= b) {
            apply_node(k, f);
            return;
        }
        push(k);
        int m = l + (r - l) / 2;
        apply_impl(a, b, f, 2 * k, l, m);
        apply_impl(a, b, f, 2 * k + 1, m, r);
        pull(k);
    }

    template <class G>
    int max_right_impl(int a, G& g, S& acc, int k, int l, int r) {
        if (r <= a || n_ <= l) return -1;
        if (a <= l) {
            S next = op(acc, data_[k]);
            if (g(next)) {
                acc = next;
                return -1;
            }
            if (r - l == 1) return l;
        }
        push(k);
        int m = l + (r - l) / 2;
        int found = max_right_impl(a, g, acc, 2 * k, l, m);
        if (found != -1) return found;
        return max_right_impl(a, g, acc, 2 * k + 1, m, r);
    }

    template <class G>
    int min_left_impl(int b, G& g, S& acc, int k, int l, int r) {
        if (b <= l || n_ <= l) return -1;
        if (r <= b) {
            S next = op(data_[k], acc);
            if (g(next)) {
                acc = next;
                return -1;
            }
            if (r - l == 1) return r;
        }
        push(k);
        int m = l + (r - l) / 2;
        int found = min_left_impl(b, g, acc, 2 * k + 1, m, r);
        if (found != -1) return found;
        return min_left_impl(b, g, acc, 2 * k, l, m);
    }

    static std::vector<S> initial(int n) {
        if (n < 0) throw std::invalid_argument("Segment Tree Beats: negative size");
        return std::vector<S>(n, e());
    }

   public:
    AbstractSegTreeBeats() : AbstractSegTreeBeats(0) {}
    explicit AbstractSegTreeBeats(int n) : AbstractSegTreeBeats(initial(n)) {}
    explicit AbstractSegTreeBeats(const std::vector<S>& a) : n_(static_cast<int>(a.size())) {
        while (base_ < n_) base_ <<= 1;
        data_.assign(2 * base_, e());
        lazy_.assign(base_, id());
        pending_.assign(base_, false);
        for (int i = 0; i < n_; ++i) data_[base_ + i] = a[i];
        for (int k = base_ - 1; k >= 1; --k) pull(k);
    }

    int size() const { return n_; }
    void set(int p, const S& x) {
        assert(0 <= p && p < n_);
        set_impl(p, x, 1, 0, base_);
    }
    S get(int p) {
        assert(0 <= p && p < n_);
        return prod(p, p + 1);
    }
    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        return l == r ? e() : prod_impl(l, r, 1, 0, base_);
    }
    S all_prod() const { return data_[1]; }
    void apply(int p, const F& f) {
        assert(0 <= p && p < n_);
        apply(p, p + 1, f);
    }
    void apply(int l, int r, const F& f) {
        assert(0 <= l && l <= r && r <= n_);
        if (l < r) apply_impl(l, r, f, 1, 0, base_);
    }

    template <class G>
    int max_right(int l, G g) {
        assert(0 <= l && l <= n_);
        S acc = e();
        assert(g(acc));
        if (l == n_) return n_;
        int found = max_right_impl(l, g, acc, 1, 0, base_);
        return found == -1 ? n_ : found;
    }
    template <auto g>
    int max_right(int l) { return max_right(l, g); }

    template <class G>
    int min_left(int r, G g) {
        assert(0 <= r && r <= n_);
        S acc = e();
        assert(g(acc));
        if (r == 0) return 0;
        int found = min_left_impl(r, g, acc, 1, 0, base_);
        return found == -1 ? 0 : found;
    }
    template <auto g>
    int min_left(int r) { return min_left(r, g); }
};
