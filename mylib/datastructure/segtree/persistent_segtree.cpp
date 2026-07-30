// 最終更新: 2026-07-13 00:25:22 JST
// ACL風の永続セグメント木
//
// 使い方:
//   S op(S a, S b) { ... }
//   S e() { ... }
//   PersistentSegmentTree<S, op, e> seg(n);   // 空木で初期化
//   PersistentSegmentTree<S, op, e> seg(v);   // vector から構築
//
//   int t0 = 0;                               // 初期バージョン
//   int t1 = seg.set(t0, p, x);               // version t0 を元に a[p] = x
//
//   S x = seg.get(t1, p);                     // version t1 の p 番目の値
//   S y = seg.prod(t1, l, r);                 // version t1 の区間 [l, r) の積
//   S z = seg.all_prod(t1);                   // version t1 全体の積
//
//   seg.get(p), seg.prod(l, r), seg.all_prod()
//   は常に最新バージョンに対して作用する。
//
// 注意:
//   - 更新のたびに新しいバージョンが末尾に追加され、その番号を返す。
//   - `set` は `node*` を直接渡して、その root から分岐することもできる。
#pragma once

template <class S, auto op, auto e, int NODES = 20000000>
struct PersistentSegmentTree {
    using ll = long long;

    struct node {
        S data;
        node *l, *r;
        explicit node(const S &data_) : data(data_), l(nullptr), r(nullptr) {}
        node(const S &data_, node *l_, node *r_) : data(data_), l(l_), r(r_) {}
    };

    node *pool;
    int pid;
    int _n;
    node *nil;
    vector<node *> roots;

    PersistentSegmentTree() : PersistentSegmentTree(0) {}

    explicit PersistentSegmentTree(int n)
        : pool(static_cast<node *>(::operator new(sizeof(node) * NODES))), pid(0), _n(n), nil(nullptr) {
        nil = new_node(e());
        nil->l = nil->r = nil;
        roots.reserve(262144);
        roots.push_back(nil);
    }

    explicit PersistentSegmentTree(const vector<S> &v)
        : pool(static_cast<node *>(::operator new(sizeof(node) * NODES))), pid(0), _n((int)v.size()), nil(nullptr) {
        nil = new_node(e());
        nil->l = nil->r = nil;
        roots.reserve(262144);
        roots.push_back(_n == 0 ? nil : build(0, _n, v));
    }

    PersistentSegmentTree(const PersistentSegmentTree &) = delete;
    PersistentSegmentTree &operator=(const PersistentSegmentTree &) = delete;
    PersistentSegmentTree(PersistentSegmentTree &&) = delete;
    PersistentSegmentTree &operator=(PersistentSegmentTree &&) = delete;

    ~PersistentSegmentTree() {
        if constexpr (!is_trivially_destructible_v<node>) {
            for (int i = 0; i < pid; i++) pool[i].~node();
        }
        ::operator delete(pool);
    }

   private:
    node *new_node_unchecked(const S &data, node *l, node *r) {
        node *ptr = &pool[pid++];
        ::new (static_cast<void *>(ptr)) node(data, l, r);
        return ptr;
    }

    node *new_node(const S &data) {
        if (pid >= NODES) throw std::length_error("PersistentSegmentTree node pool exhausted");
        return new_node_unchecked(data, nil, nil);
    }

    node *merge(node *l, node *r) {
        if (pid >= NODES) throw std::length_error("PersistentSegmentTree node pool exhausted");
        return new_node_unchecked(op(l->data, r->data), l, r);
    }

    node *merge_unchecked(node *l, node *r) {
        return new_node_unchecked(op(l->data, r->data), l, r);
    }

    node *build(int l, int r, const vector<S> &v) {
        if (l + 1 == r) return new_node(v[l]);
        int m = (l + r) >> 1;
        return merge(build(l, m, v), build(m, r, v));
    }

    node *set_impl(int p, const S &x, node *n, int l, int r) {
        if (l + 1 == r) return new_node_unchecked(x, nil, nil);
        int m = (l + r) >> 1;
        if (p < m) return merge_unchecked(set_impl(p, x, n->l, l, m), n->r);
        return merge_unchecked(n->l, set_impl(p, x, n->r, m, r));
    }

    S get_impl(int p, node *n, int l, int r) const {
        while (n != nil && l + 1 != r) {
            int m = (l + r) >> 1;
            if (p < m) {
                n = n->l;
                r = m;
            } else {
                n = n->r;
                l = m;
            }
        }
        return n == nil ? e() : n->data;
    }

    S prod_impl(int a, int b, node *n, int l, int r) const {
        if (n == nil) return e();
        if (r <= a || b <= l) return e();
        if (a <= l && r <= b) return n->data;
        int m = (l + r) >> 1;
        return op(prod_impl(a, b, n->l, l, m), prod_impl(a, b, n->r, m, r));
    }

    int append_root(node *root) {
        roots.push_back(root);
        return (int)roots.size() - 1;
    }

   public:
    int size() const { return _n; }

    int versions() const { return (int)roots.size(); }

    node *root(int t) const {
        assert(0 <= t && t < versions());
        return roots[t];
    }

    int set(node *n, int p, const S &x) {
        assert(0 <= p && p < _n);
        int required = bit_width((unsigned)(_n - 1)) + 1;
        if (pid > NODES - required) throw std::length_error("PersistentSegmentTree node pool exhausted");
        return append_root(set_impl(p, x, n, 0, _n));
    }

    int set(int t, int p, const S &x) {
        assert(0 <= t && t < versions());
        return set(roots[t], p, x);
    }

    int set(int p, const S &x) { return set(roots.back(), p, x); }

    S get(node *n, int p) const {
        assert(0 <= p && p < _n);
        return get_impl(p, n, 0, _n);
    }

    S get(int t, int p) const {
        assert(0 <= t && t < versions());
        return get(roots[t], p);
    }

    S get(int p) const { return get(roots.back(), p); }

    S prod(node *n, int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        return prod_impl(l, r, n, 0, _n);
    }

    S prod(int t, int l, int r) const {
        assert(0 <= t && t < versions());
        return prod(roots[t], l, r);
    }

    S prod(int l, int r) const { return prod(roots.back(), l, r); }

    S all_prod(node *n) const { return n == nil ? e() : n->data; }

    S all_prod(int t) const {
        assert(0 <= t && t < versions());
        return all_prod(roots[t]);
    }

    S all_prod() const { return all_prod(roots.back()); }

    node *new_tree() const { return nil; }
};
