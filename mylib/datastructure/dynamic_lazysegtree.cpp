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

    int n;
    Node *root;

    lazy_segtree_sparse(int _n) : n(_n), root(nullptr) {}
    lazy_segtree_sparse(const vector<S> &v) : n((int)v.size()), root(nullptr) {
        for (int i = 0; i < n; i++) {
            if (!(v[i] == e())) {
                set(i, v[i]);
            }
        }
    }

    ~lazy_segtree_sparse() { destroy(root); }

    void set(int idx, S x) {
        check_idx(idx);
        set_rec(root, 0, n, idx, x);
    }
    S get(int idx) {
        check_idx(idx);
        return get_rec(root, 0, n, idx);
    }
    void apply(int l, int r, F f) {
        check_range(l, r);
        apply_rec(root, 0, n, l, r, f);
    }
    S prod(int l, int r) {
        check_range(l, r);
        return prod_rec(root, 0, n, l, r);
    }

   private:
    void check_idx(int i) {
        if (i < 0 || i >= n) throw;
    }
    void check_range(int l, int r) {
        if (l < 0 || r > n || l > r) throw;
    }

    void destroy(Node *p) {
        if (!p) return;
        destroy(p->l);
        destroy(p->r);
        delete p;
    }

    inline Node *ensure_node(Node *&node, int len) {
        if (!node) node = new Node(len);
        return node;
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
        int L = p->len / 2;
        int R = p->len - L;
        if (!p->l) p->l = new Node(L);
        if (!p->r) p->r = new Node(R);
        apply_node(p->l, p->lazy);
        apply_node(p->r, p->lazy);
        p->lazy = id();
        p->has_lazy = false;
    }

    void pull(Node *p) {
        S lv = p->l ? p->l->val : e();
        S rv = p->r ? p->r->val : e();
        p->val = op(lv, rv);
    }

    S get_rec(Node *p, int nl, int nr, int idx) {
        if (!p) return e();
        if (nr - nl == 1) return p->val;
        push(p);
        int mid = (nl + nr) >> 1;
        if (idx < mid)
            return get_rec(p->l, nl, mid, idx);
        else
            return get_rec(p->r, mid, nr, idx);
    }

    void set_rec(Node *&p, int nl, int nr, int idx, S x) {
        if (!p) p = new Node(nr - nl);
        if (nr - nl == 1) {
            p->val = x;
            p->lazy = id();
            p->has_lazy = false;
            return;
        }
        push(p);
        int mid = (nl + nr) >> 1;
        if (idx < mid)
            set_rec(p->l, nl, mid, idx, x);
        else
            set_rec(p->r, mid, nr, idx, x);
        pull(p);
    }

    void apply_rec(Node *&p, int nl, int nr, int l, int r, F f) {
        if (r <= nl || nr <= l) return;
        if (!p) p = new Node(nr - nl);
        if (l <= nl && nr <= r) {
            apply_node(p, f);
            return;
        }
        push(p);
        int mid = (nl + nr) >> 1;
        apply_rec(p->l, nl, mid, l, r, f);
        apply_rec(p->r, mid, nr, l, r, f);
        pull(p);
    }

    S prod_rec(Node *p, int nl, int nr, int l, int r) {
        if (r <= nl || nr <= l) return e();
        if (!p) return e();
        if (l <= nl && nr <= r) return p->val;
        push(p);
        int mid = (nl + nr) >> 1;
        return op(prod_rec(p->l, nl, mid, l, r), prod_rec(p->r, mid, nr, l, r));
    }
};