// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/dynamic_lazysegtree.cpp
/* 区間長はSに持たせず mapping に渡す。
S mapping(F f, S x, ll len) { return x + f * len; }
composition(f, g) は g の後に f を適用する合成。
get/prod はノードを生成しない。詳細は dynamic_lazysegtree.md を参照。
*/

template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S, ll), F (*composition)(F, F), F (*id)()>
struct lazy_segtree_sparse {
    using index_type = uint32_t;
    static constexpr index_type lazy_bit = index_type(1) << 31;
    static constexpr index_type index_mask = lazy_bit - 1;

    struct Node {
        // 左の子の番号と遅延フラグを共有する。0 は未生成の子。
        index_type l = 0, r = 0;
        S val;
        F lazy;
        explicit Node(const S &v) : val(v), lazy(id()) {}
        index_type left() const { return l & index_mask; }
        bool has_lazy() const { return (l & lazy_bit) != 0; }
        void set_left(index_type child) { l = (l & lazy_bit) | child; }
    };

    ll n;
    index_type root = 0;
    function<S(ll)> range_e;  // 長さlenの区間の初期値（区間の位置には依存しない）

    // デフォルト: 各区間の初期値は e()。
    lazy_segtree_sparse(ll _n)
        : n(_n), range_e([](ll) { return e(); }), default_initial(true) {
        check_size();
    }
    // 小さいN用: vectorから初期化。
    lazy_segtree_sparse(const vector<S> &v)
        : lazy_segtree_sparse((ll)v.size()) {
        for (ll i = 0; i < n; i++) {
            if (!(v[i] == e())) set(i, v[i]);
        }
    }
    // range_e_fn(0)=e(), range_e_fn(a+b)=op(range_e_fn(a),range_e_fn(b)) が必要。
    // 例: 全要素をvで初期化し op が加算なら [v](ll len){ return v * len; }。
    lazy_segtree_sparse(ll _n, function<S(ll)> range_e_fn)
        : n(_n), range_e(std::move(range_e_fn)), default_initial(false) {
        check_size();
    }

    lazy_segtree_sparse(const lazy_segtree_sparse &) = delete;
    lazy_segtree_sparse &operator=(const lazy_segtree_sparse &) = delete;
    lazy_segtree_sparse(lazy_segtree_sparse &&) = delete;
    lazy_segtree_sparse &operator=(lazy_segtree_sparse &&) = delete;

    ~lazy_segtree_sparse() {
        // 木をたどらず、実際に構築したノードだけを破棄する。
        for (size_t i = 0; i < used_nodes; ++i)
            std::destroy_at(blocks[i / block_size] + i % block_size);
        for (Node *block : blocks) allocator.deallocate(block, block_size);
    }

    void set(ll idx, S x) {
        check_idx(idx);
        root = set_rec(root, 0, n, idx, x);
    }
    S get(ll idx) const {
        check_idx(idx);
        return prod_rec(root, 0, n, idx, idx + 1, id(), false);
    }
    void apply(ll l, ll r, F f) {
        check_range(l, r);
        if (l == r) return;
        root = apply_rec(root, 0, n, l, r, f);
    }
    S prod(ll l, ll r) const {
        check_range(l, r);
        if (l == r) return e();
        return prod_rec(root, 0, n, l, r, id(), false);
    }
    size_t node_count() const { return used_nodes; }

   private:
    // vector<Node> の容量倍増・再確保を避け、固定長ブロックを追加する。
    // Node 自体のアドレスは固定なので、再帰中も参照が無効にならない。
    static constexpr size_t block_size = 256;
    bool default_initial;
    std::allocator<Node> allocator;
    vector<Node *> blocks;
    size_t used_nodes = 0;

    Node &node(index_type p) { return blocks[(p - 1) / block_size][(p - 1) % block_size]; }
    const Node &node(index_type p) const { return blocks[(p - 1) / block_size][(p - 1) % block_size]; }

    index_type make_node(ll len) {
        if (used_nodes == index_mask) throw std::length_error("lazy_segtree_sparse node limit");
        if (used_nodes % block_size == 0 && used_nodes / block_size == blocks.size()) {
            Node *block = allocator.allocate(block_size);
            try {
                blocks.push_back(block);
            } catch (...) {
                allocator.deallocate(block, block_size);
                throw;
            }
        }
        std::construct_at(blocks[used_nodes / block_size] + used_nodes % block_size, initial_value(len));
        return static_cast<index_type>(++used_nodes);
    }

    S initial_value(ll len) const { return default_initial ? e() : range_e(len); }

    void check_size() const {
        if (n < 0) throw std::invalid_argument("lazy_segtree_sparse negative size");
    }
    void check_idx(ll i) const {
        if (i < 0 || i >= n) throw std::out_of_range("lazy_segtree_sparse index");
    }
    void check_range(ll l, ll r) const {
        if (l < 0 || r > n || l > r) throw std::out_of_range("lazy_segtree_sparse range");
    }

    void apply_node(index_type p, F f, ll len) {
        Node &v = node(p);
        v.val = mapping(f, v.val, len);
        // 葉には伝播先がないので、遅延タグを保持する必要はない。
        if (len == 1) return;
        v.lazy = v.has_lazy() ? composition(f, v.lazy) : f;
        v.l |= lazy_bit;
    }

    void push(index_type p, ll len) {
        Node &v = node(p);
        if (!v.has_lazy()) return;
        ll L = len / 2, R = len - L;
        if (!v.left()) v.set_left(make_node(L));
        if (!v.r) v.r = make_node(R);
        apply_node(v.left(), v.lazy, L);
        apply_node(v.r, v.lazy, R);
        v.lazy = id();
        v.l &= index_mask;
    }

    void pull(index_type p, ll len) {
        Node &v = node(p);
        ll L = len / 2;
        S lv = v.left() ? node(v.left()).val : initial_value(L);
        S rv = v.r ? node(v.r).val : initial_value(len - L);
        v.val = op(lv, rv);
    }

    index_type set_rec(index_type p, ll nl, ll nr, ll idx, S x) {
        if (!p) p = make_node(nr - nl);
        Node &v = node(p);
        if (nr - nl == 1) {
            v.val = x;
            return p;
        }
        push(p, nr - nl);
        ll mid = nl + (nr - nl) / 2;
        if (idx < mid)
            v.set_left(set_rec(v.left(), nl, mid, idx, x));
        else
            v.r = set_rec(v.r, mid, nr, idx, x);
        pull(p, nr - nl);
        return p;
    }

    index_type apply_rec(index_type p, ll nl, ll nr, ll l, ll r, F f) {
        if (r <= nl || nr <= l) return p;
        if (!p) p = make_node(nr - nl);
        if (l <= nl && nr <= r) {
            apply_node(p, f, nr - nl);
            return p;
        }
        push(p, nr - nl);
        Node &v = node(p);
        ll mid = nl + (nr - nl) / 2;
        v.set_left(apply_rec(v.left(), nl, mid, l, r, f));
        v.r = apply_rec(v.r, mid, nr, l, r, f);
        pull(p, nr - nl);
        return p;
    }

    // carry は祖先の未伝播タグ。祖先の更新はこのノードのタグより新しい。
    // 子を生成せず、取得結果にだけ carry を適用する。
    S prod_rec(index_type p, ll nl, ll nr, ll l, ll r, F carry, bool has_carry) const {
        if (r <= nl || nr <= l) return e();
        if (!p) {
            ll len = min(r, nr) - max(l, nl);
            S value = initial_value(len);
            return has_carry ? mapping(carry, value, len) : value;
        }
        const Node &v = node(p);
        if (l <= nl && nr <= r)
            return has_carry ? mapping(carry, v.val, nr - nl) : v.val;
        if (v.has_lazy()) {
            carry = has_carry ? composition(carry, v.lazy) : v.lazy;
            has_carry = true;
        }
        ll mid = nl + (nr - nl) / 2;
        return op(prod_rec(v.left(), nl, mid, l, r, carry, has_carry),
                  prod_rec(v.r, mid, nr, l, r, carry, has_carry));
    }
};
