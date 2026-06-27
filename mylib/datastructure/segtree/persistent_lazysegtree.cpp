// ACL風の永続遅延セグメント木
//
// 使い方:
//   S op(S a, S b) { ... }
//   S e() { ... }
//   S mapping(F f, S x) { ... }
//   F composition(F f, F g) { ... }
//   F id() { ... }
//   PersistentLazySegTree<S, op, e, F, mapping, composition, id> seg(n);
//   PersistentLazySegTree<S, op, e, F, mapping, composition, id> seg(v);
//
//   int t0 = 0;
//   int t1 = seg.set(t0, p, x);              // version t0 を元に a[p] = x
//   int t2 = seg.apply(t1, p, f);             // version t1 を元に a[p] に作用
//   int t3 = seg.apply_range(t2, l, r, f);    // version t2 を元に区間 [l, r) に作用
//   int t4 = seg.copy_range(t3, t0, l, r);    // version t3 の [l, r) を version t0 の同区間で上書き
//
//   S x = seg.get(t3, p);
//   S y = seg.prod(t3, l, r);
//   S z = seg.all_prod(t3);
//
//   seg.get(p), seg.prod(l, r), seg.all_prod()
//   は常に最新バージョンに対して作用する。
//
// 注意:
//   - `PersistentLazySegTree(n)` は ACL と同様に `vector<S>(n, e())` から構築する。
//   - `S` が区間長に依存する場合は、ACL と同様に `S` 自身に長さを持たせる。
//   - 更新のたびに新しいバージョンが末尾に追加され、その番号を返す。
//   - `set/apply/apply_range/copy_range` は `node*` を直接渡して、その root から分岐することもできる。

#pragma once

template <class S, auto op, auto e, class F, auto mapping, auto composition, auto id, int NODES = 20000000>
struct PersistentLazySegTree {
    struct node {
        S data;
        F lazy;
        bool has_lazy;
        node *l, *r;
        node(const S &data_, const F &lazy_, bool has_lazy_, node *l_, node *r_)
            : data(data_), lazy(lazy_), has_lazy(has_lazy_), l(l_), r(r_) {}
    };

    node *pool;
    int pid;
    int _n;
    node *nil;
    vector<node *> roots;

    PersistentLazySegTree() : PersistentLazySegTree(0) {}

    explicit PersistentLazySegTree(int n) : PersistentLazySegTree(vector<S>(n, e())) {}

    explicit PersistentLazySegTree(const vector<S> &v)
        : pool(static_cast<node *>(::operator new(sizeof(node) * NODES))), pid(0), _n((int)v.size()), nil(nullptr) {
        nil = new_node(e(), id(), false, nullptr, nullptr);
        nil->l = nil->r = nil;
        roots.reserve(262144);
        roots.push_back(_n == 0 ? nil : build(0, _n, v));
    }

    ~PersistentLazySegTree() {
        for (int i = 0; i < pid; i++) pool[i].~node();
        ::operator delete(pool);
    }

   private:
    node *new_node(const S &data, const F &lazy, bool has_lazy, node *l, node *r) {
        assert(pid < NODES);
        node *ptr = &pool[pid++];
        ::new (static_cast<void *>(ptr)) node(data, lazy, has_lazy, l, r);
        return ptr;
    }

    node *clone_node(node *n) {
        assert(pid < NODES);
        node *ptr = &pool[pid++];
        ::new (static_cast<void *>(ptr)) node(*n);
        return ptr;
    }

    node *build(int l, int r, const vector<S> &v) {
        if (l + 1 == r) return new_node(v[l], id(), false, nil, nil);
        int m = (l + r) >> 1;
        node *lc = build(l, m, v);
        node *rc = build(m, r, v);
        return new_node(op(lc->data, rc->data), id(), false, lc, rc);
    }

    void all_apply(node *n, const F &f) {
        n->data = mapping(f, n->data);
        if (n->has_lazy)
            n->lazy = composition(f, n->lazy);
        else {
            n->lazy = f;
            n->has_lazy = true;
        }
    }

    void push(node *n, int l, int r) {
        if (!n->has_lazy || l + 1 == r) return;
        n->l = clone_node(n->l);
        n->r = clone_node(n->r);
        all_apply(n->l, n->lazy);
        all_apply(n->r, n->lazy);
        n->lazy = id();
        n->has_lazy = false;
    }

    node *set_impl(node *n, int p, const S &x, int l, int r) {
        node *cur = clone_node(n);
        if (l + 1 == r) {
            cur->data = x;
            cur->lazy = id();
            cur->has_lazy = false;
            return cur;
        }
        push(cur, l, r);
        int m = (l + r) >> 1;
        if (p < m)
            cur->l = set_impl(cur->l, p, x, l, m);
        else
            cur->r = set_impl(cur->r, p, x, m, r);
        cur->data = op(cur->l->data, cur->r->data);
        return cur;
    }

    node *apply_point_impl(node *n, int p, const F &f, int l, int r) {
        node *cur = clone_node(n);
        if (l + 1 == r) {
            all_apply(cur, f);
            return cur;
        }
        push(cur, l, r);
        int m = (l + r) >> 1;
        if (p < m)
            cur->l = apply_point_impl(cur->l, p, f, l, m);
        else
            cur->r = apply_point_impl(cur->r, p, f, m, r);
        cur->data = op(cur->l->data, cur->r->data);
        return cur;
    }

    node *apply_range_impl(node *n, int a, int b, const F &f, int l, int r) {
        if (b <= l || r <= a) return n;
        node *cur = clone_node(n);
        if (a <= l && r <= b) {
            all_apply(cur, f);
            return cur;
        }
        push(cur, l, r);
        int m = (l + r) >> 1;
        cur->l = apply_range_impl(cur->l, a, b, f, l, m);
        cur->r = apply_range_impl(cur->r, a, b, f, m, r);
        cur->data = op(cur->l->data, cur->r->data);
        return cur;
    }

    node *materialize(node *n, const F &f, bool has_f) {
        node *cur = clone_node(n);
        if (has_f) all_apply(cur, f);
        return cur;
    }

    node *copy_range_impl(node *dst, node *src, int a, int b, int l, int r, const F &fd, bool has_fd, const F &fs,
                          bool has_fs) {
        if (b <= l || r <= a) return materialize(dst, fd, has_fd);
        node *cur_src = materialize(src, fs, has_fs);
        if (a <= l && r <= b) return cur_src;

        node *cur_dst = materialize(dst, fd, has_fd);
        if (l + 1 == r) return cur_src;

        push(cur_dst, l, r);
        push(cur_src, l, r);
        int m = (l + r) >> 1;
        cur_dst->l = copy_range_impl(cur_dst->l, cur_src->l, a, b, l, m, id(), false, id(), false);
        cur_dst->r = copy_range_impl(cur_dst->r, cur_src->r, a, b, m, r, id(), false, id(), false);
        cur_dst->data = op(cur_dst->l->data, cur_dst->r->data);
        return cur_dst;
    }

    S get_impl(node *n, int p, int l, int r, const F &f, bool has_f) const {
        S cur = has_f ? mapping(f, n->data) : n->data;
        if (l + 1 == r) return cur;
        int m = (l + r) >> 1;
        F next_f = f;
        bool next_has_f = has_f;
        if (n->has_lazy) {
            if (next_has_f)
                next_f = composition(next_f, n->lazy);
            else {
                next_f = n->lazy;
                next_has_f = true;
            }
        }
        if (p < m) return get_impl(n->l, p, l, m, next_f, next_has_f);
        return get_impl(n->r, p, m, r, next_f, next_has_f);
    }

    S prod_impl(node *n, int a, int b, int l, int r, const F &f, bool has_f) const {
        if (b <= l || r <= a) return e();
        S cur = has_f ? mapping(f, n->data) : n->data;
        if (a <= l && r <= b) return cur;
        int m = (l + r) >> 1;
        F next_f = f;
        bool next_has_f = has_f;
        if (n->has_lazy) {
            if (next_has_f)
                next_f = composition(next_f, n->lazy);
            else {
                next_f = n->lazy;
                next_has_f = true;
            }
        }
        return op(prod_impl(n->l, a, b, l, m, next_f, next_has_f), prod_impl(n->r, a, b, m, r, next_f, next_has_f));
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
        return append_root(set_impl(n, p, x, 0, _n));
    }

    int set(int t, int p, const S &x) {
        assert(0 <= t && t < versions());
        return set(roots[t], p, x);
    }

    int set(int p, const S &x) { return set(roots.back(), p, x); }

    S get(node *n, int p) const {
        assert(0 <= p && p < _n);
        return get_impl(n, p, 0, _n, id(), false);
    }

    S get(int t, int p) const {
        assert(0 <= t && t < versions());
        return get(roots[t], p);
    }

    S get(int p) const { return get(roots.back(), p); }

    S prod(node *n, int l, int r) const {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return e();
        return prod_impl(n, l, r, 0, _n, id(), false);
    }

    S prod(int t, int l, int r) const {
        assert(0 <= t && t < versions());
        return prod(roots[t], l, r);
    }

    S prod(int l, int r) const { return prod(roots.back(), l, r); }

    S all_prod(node *n) const { return n->data; }

    S all_prod(int t) const {
        assert(0 <= t && t < versions());
        return all_prod(roots[t]);
    }

    S all_prod() const { return all_prod(roots.back()); }

    int apply(node *n, int p, const F &f) {
        assert(0 <= p && p < _n);
        return append_root(apply_point_impl(n, p, f, 0, _n));
    }

    int apply(int t, int p, const F &f) {
        assert(0 <= t && t < versions());
        return apply(roots[t], p, f);
    }

    int apply(int p, const F &f) { return apply(roots.back(), p, f); }

    int apply_range(node *n, int l, int r, const F &f) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return append_root(n);
        return append_root(apply_range_impl(n, l, r, f, 0, _n));
    }

    int apply_range(int t, int l, int r, const F &f) {
        assert(0 <= t && t < versions());
        return apply_range(roots[t], l, r, f);
    }

    int apply_range(int l, int r, const F &f) { return apply_range(roots.back(), l, r, f); }

    int copy_range(node *dst, node *src, int l, int r) {
        assert(0 <= l && l <= r && r <= _n);
        if (l == r) return append_root(dst);
        return append_root(copy_range_impl(dst, src, l, r, 0, _n, id(), false, id(), false));
    }

    int copy_range(int td, int ts, int l, int r) {
        assert(0 <= td && td < versions());
        assert(0 <= ts && ts < versions());
        return copy_range(roots[td], roots[ts], l, r);
    }
};
