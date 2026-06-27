// 値でソートされた Treap に Segment Tree のような区間積を持たせたもの。
//
// 使い方:
//   ll op(ll a, ll b) { return a + b; }
//   ll e() { return 0; }
//   TreapSegtree<ll, op, e> tr;
//
//   tr.insert(x);              // x を1つ挿入（重複可）
//   tr.erase(x);               // x を1つ削除。存在しなければ false
//   tr.kth(k);                 // 小さい方から k 番目（0-indexed）
//   tr.kth_largest(k);         // 大きい方から k 番目（0-indexed）
//   tr.prod(l, r);             // 小さい方から [l, r) 番目の総積
//   tr.prod_largest(l, r);     // 大きい方から [l, r) 番目の総積
//
// 例:
//   0番目に大きい値から5番目に大きい値までの総和（6個分）は
//   tr.prod_largest(0, 6)
//
// 注意:
//   - `prod(l, r)` は Segment Tree と同じ半開区間 [l, r)。
//   - `prod_largest(l, r)` も大きい順での半開区間 [l, r)。
//   - `op` は非可換でもよい。`prod` は小さい順、`prod_largest` は大きい順に集約する。

template <class S, S (*op)(S, S), S (*e)(), class Compare = less<S>>
class TreapSegtree {
   private:
    struct Node {
        S key, product, product_rev;
        Node *l, *r;
        unsigned priority;
        int cnt;
    };
    using Tree = Node *;

    static constexpr int CHUNK_SIZE = 1 << 20;
    inline static vector<Node *> chunks;
    inline static int chunk_pos = CHUNK_SIZE;
    Tree root = nullptr;
    Compare comp = Compare();
    unsigned rng_state = 123456789;

    unsigned rng() {
        unsigned x = rng_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return rng_state = x;
    }

    Node *new_node(const S &key) {
        if (chunk_pos == CHUNK_SIZE) {
            chunks.push_back(new Node[CHUNK_SIZE]);
            chunk_pos = 0;
        }
        Node *p = &chunks.back()[chunk_pos++];
        *p = {key, key, key, nullptr, nullptr, rng(), 1};
        return p;
    }

    bool eq(const S &a, const S &b) const { return !comp(a, b) && !comp(b, a); }
    int count(Tree t) const { return t ? t->cnt : 0; }
    S product(Tree t) const { return t ? t->product : e(); }
    S product_rev(Tree t) const { return t ? t->product_rev : e(); }

    Tree update(Tree t) {
        if (!t) return nullptr;
        t->cnt = count(t->l) + count(t->r) + 1;
        t->product = op(op(product(t->l), t->key), product(t->r));
        t->product_rev = op(op(product_rev(t->r), t->key), product_rev(t->l));
        return t;
    }

    Tree rotate_right(Tree t) {
        Tree s = t->l;
        t->l = s->r;
        s->r = update(t);
        return update(s);
    }

    Tree rotate_left(Tree t) {
        Tree s = t->r;
        t->r = s->l;
        s->l = update(t);
        return update(s);
    }

    Tree merge(Tree l, Tree r) {
        if (!l) return r;
        if (!r) return l;
        if (l->priority > r->priority) {
            l->r = merge(l->r, r);
            return update(l);
        }
        r->l = merge(l, r->l);
        return update(r);
    }

    Tree insert(Tree t, const S &key) {
        if (!t) return new_node(key);
        if (comp(key, t->key)) {
            t->l = insert(t->l, key);
            if (t->l->priority > t->priority) t = rotate_right(t);
        } else {
            t->r = insert(t->r, key);
            if (t->r->priority > t->priority) t = rotate_left(t);
        }
        return update(t);
    }

    Tree erase(Tree t, const S &key, bool &erased) {
        if (!t) return nullptr;
        if (eq(key, t->key)) {
            erased = true;
            return merge(t->l, t->r);
        }
        if (comp(key, t->key)) t->l = erase(t->l, key, erased);
        else t->r = erase(t->r, key, erased);
        return update(t);
    }

    bool contains(Tree t, const S &key) const {
        while (t) {
            if (eq(key, t->key)) return true;
            t = comp(key, t->key) ? t->l : t->r;
        }
        return false;
    }

    S kth(Tree t, int k) const {
        int lc = count(t->l);
        if (k < lc) return kth(t->l, k);
        if (k == lc) return t->key;
        return kth(t->r, k - lc - 1);
    }

    S prod(Tree t, int l, int r, int offset, bool rev) const {
        if (!t || r <= offset || offset + count(t) <= l) return e();
        if (l <= offset && offset + count(t) <= r) return rev ? product_rev(t) : product(t);
        int lc = count(t->l);
        int idx = offset + lc;
        if (!rev) {
            S res = prod(t->l, l, r, offset, false);
            if (l <= idx && idx < r) res = op(res, t->key);
            return op(res, prod(t->r, l, r, idx + 1, false));
        }
        S res = prod(t->r, l, r, idx + 1, true);
        if (l <= idx && idx < r) res = op(res, t->key);
        return op(res, prod(t->l, l, r, offset, true));
    }

    int lower_bound(Tree t, const S &key) const {
        int res = 0;
        while (t) {
            if (!comp(t->key, key)) {
                t = t->l;
            } else {
                res += count(t->l) + 1;
                t = t->r;
            }
        }
        return res;
    }

    int upper_bound(Tree t, const S &key) const {
        int res = 0;
        while (t) {
            if (comp(key, t->key)) {
                t = t->l;
            } else {
                res += count(t->l) + 1;
                t = t->r;
            }
        }
        return res;
    }

   public:
    TreapSegtree() = default;
    explicit TreapSegtree(const vector<S> &v) {
        for (const S &x : v) insert(x);
    }

    int size() const { return count(root); }
    bool empty() const { return size() == 0; }
    bool contains(const S &key) const { return contains(root, key); }

    void insert(const S &key) { root = insert(root, key); }

    bool erase(const S &key) {
        bool erased = false;
        root = erase(root, key, erased);
        return erased;
    }

    S kth(int k) const {
        assert(0 <= k && k < size());
        return kth(root, k);
    }

    S kth_largest(int k) const {
        assert(0 <= k && k < size());
        return kth(root, size() - 1 - k);
    }

    S all_prod() const { return product(root); }
    S all_prod_largest() const { return product_rev(root); }

    S prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        return prod(root, l, r, 0, false);
    }

    S prod_largest(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        int n = size();
        return prod(root, n - r, n - l, 0, true);
    }

    int lower_bound(const S &key) const { return lower_bound(root, key); }
    int upper_bound(const S &key) const { return upper_bound(root, key); }
    pair<int, int> equal_range(const S &key) const { return {lower_bound(key), upper_bound(key)}; }
};
