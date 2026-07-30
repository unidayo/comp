// 最終更新: 2026-07-12 14:27:44 JST
/*区間長はSに持たせない
mappingの引数にSの区間長lenをもたせる
S mapping(F f, S x, ll len) { return x + f * len; }
*/

template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S, ll), F (*composition)(F, F), F (*id)()>
struct lazy_segtree_sparse {
    struct Node {
        Node *l = nullptr, *r = nullptr;
        S val;
        F lazy;
        bool has_lazy;
        ll len;  // このノードが表す区間の長さ
        Node(ll len_, const S &v = e()) : val(v), lazy(id()), has_lazy(false), len(len_) {}
    };

    ll n;
    Node *root;
    function<S(ll)> range_e;  // range_e(len) = 長さlenの区間の初期値

    // デフォルト: 全要素 e()
    lazy_segtree_sparse(ll _n) : n(_n), root(nullptr), range_e([](ll) { return e(); }), default_initial(true) {}
    // 小さいN用: vectorから初期化
    lazy_segtree_sparse(const vector<S> &v) : n((ll)v.size()), root(nullptr), range_e([](ll) { return e(); }), default_initial(true) {
        for (ll i = 0; i < n; i++) {
            if (!(v[i] == e())) set(i, v[i]);
        }
    }
    // 大きいN用: 全要素を同じ初期値で埋める
    // range_e_fn(len) は長さlenの区間の初期値を返す関数
    // 例: 全要素をvで初期化し op が加算なら range_e_fn = [v](ll len){ return v * len; }
    lazy_segtree_sparse(ll _n, function<S(ll)> range_e_fn)
        : n(_n), root(nullptr), range_e(range_e_fn), default_initial(false) {
        root = make_node(n, range_e_fn(n));
    }

    lazy_segtree_sparse(const lazy_segtree_sparse &) = delete;
    lazy_segtree_sparse &operator=(const lazy_segtree_sparse &) = delete;
    lazy_segtree_sparse(lazy_segtree_sparse &&) = delete;
    lazy_segtree_sparse &operator=(lazy_segtree_sparse &&) = delete;

    ~lazy_segtree_sparse() {
        destroy(root);
        for (Node *block : blocks) allocator.deallocate(block, block_size);
    }

    void set(ll idx, S x) {
        check_idx(idx);
        set_rec(root, 0, n, idx, x);
    }
    S get(ll idx) {
        check_idx(idx);
        return get_rec(root, 0, n, idx);
    }
    void apply(ll l, ll r, F f) {
        check_range(l, r);
        apply_rec(root, 0, n, l, r, f);
    }
    S prod(ll l, ll r) {
        check_range(l, r);
        return prod_rec(root, 0, n, l, r);
    }

   private:
    // 個別の new/delete は動的セグメント木では支配的なコストになりやすい。
    // 連続領域からまとめて払い出し、ポインタの安定性も保つ。
    static constexpr size_t block_size = 256;
    bool default_initial;
    std::allocator<Node> allocator;
    vector<Node *> blocks;
    Node *next_node = nullptr;
    size_t nodes_left = 0;

    Node *make_node(ll len, const S &value) {
        if (nodes_left == 0) {
            next_node = allocator.allocate(block_size);
            blocks.push_back(next_node);
            nodes_left = block_size;
        }
        Node *p = next_node++;
        --nodes_left;
        std::construct_at(p, len, value);
        return p;
    }

    // 通常は全要素 e() なので、std::function の間接呼び出しを避ける。
    S initial_value(ll len) const { return default_initial ? e() : range_e(len); }

    void check_idx(ll i) {
        if (i < 0 || i >= n) throw std::out_of_range("lazy_segtree_sparse index");
    }
    void check_range(ll l, ll r) {
        if (l < 0 || r > n || l > r) throw std::out_of_range("lazy_segtree_sparse range");
    }

    void destroy(Node *p) {
        if (!p) return;
        destroy(p->l);
        destroy(p->r);
        std::destroy_at(p);
    }

    void apply_node(Node *p, F f) {
        p->val = mapping(f, p->val, p->len);
        if (p->has_lazy)
            p->lazy = composition(f, p->lazy);
        else {
            p->lazy = f;
            p->has_lazy = true;
        }
    }

    void push(Node *p) {
        if (!p || !p->has_lazy) return;
        ll L = p->len / 2;
        ll R = p->len - L;
        if (!p->l) p->l = make_node(L, initial_value(L));
        if (!p->r) p->r = make_node(R, initial_value(R));
        apply_node(p->l, p->lazy);
        apply_node(p->r, p->lazy);
        p->lazy = id();
        p->has_lazy = false;
    }

    void pull(Node *p) {
        ll L = p->len / 2;
        S lv = p->l ? p->l->val : initial_value(L);
        S rv = p->r ? p->r->val : initial_value(p->len - L);
        p->val = op(lv, rv);
    }

    S get_rec(Node *p, ll nl, ll nr, ll idx) {
        if (!p) return initial_value(1);
        if (nr - nl == 1) return p->val;
        push(p);
        ll mid = (nl + nr) >> 1;
        if (idx < mid)
            return get_rec(p->l, nl, mid, idx);
        else
            return get_rec(p->r, mid, nr, idx);
    }

    void set_rec(Node *&p, ll nl, ll nr, ll idx, S x) {
        if (!p) p = make_node(nr - nl, initial_value(nr - nl));
        if (nr - nl == 1) {
            p->val = x;
            p->lazy = id();
            p->has_lazy = false;
            return;
        }
        push(p);
        ll mid = (nl + nr) >> 1;
        if (idx < mid)
            set_rec(p->l, nl, mid, idx, x);
        else
            set_rec(p->r, mid, nr, idx, x);
        pull(p);
    }

    void apply_rec(Node *&p, ll nl, ll nr, ll l, ll r, F f) {
        if (r <= nl || nr <= l) return;
        if (!p) p = make_node(nr - nl, initial_value(nr - nl));
        if (l <= nl && nr <= r) {
            apply_node(p, f);
            return;
        }
        push(p);
        ll mid = (nl + nr) >> 1;
        apply_rec(p->l, nl, mid, l, r, f);
        apply_rec(p->r, mid, nr, l, r, f);
        pull(p);
    }

    S prod_rec(Node *p, ll nl, ll nr, ll l, ll r) {
        if (r <= nl || nr <= l) return e();
        if (!p) return initial_value(min(r, nr) - max(l, nl));
        if (l <= nl && nr <= r) return p->val;
        push(p);
        ll mid = (nl + nr) >> 1;
        return op(prod_rec(p->l, nl, mid, l, r), prod_rec(p->r, mid, nr, l, r));
    }
};
