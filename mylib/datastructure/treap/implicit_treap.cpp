// 添字で管理する Implicit Treap。
//
// 使い方:
//   ll op(ll a, ll b) { return a + b; }
//   ll e() { return 0; }
//   ImplicitTreap<ll, op, e> tr({1, 2, 3});
//
//   tr.insert(k, x);        // k 番目の直前に x を挿入
//   tr.erase(k);            // k 番目を削除して値を返す
//   tr.erase(l, r);         // [l, r) を削除
//   tr.set(k, x);           // k 番目を x に変更
//   tr.get(k);              // k 番目を取得
//   tr.prod(l, r);          // [l, r) の総積
//   tr.reverse(l, r);       // [l, r) を反転
//   tr.to_vector();         // 現在の列を vector にする
//
// 注意:
//   - 添字はすべて 0-indexed、区間は半開区間 [l, r)。
//   - `op` は非可換でもよい。反転後の区間積も正しい順序で返す。
//   - 各操作の計算量は期待 O(log N)。`to_vector` は O(N)。

template <class S, S (*op)(S, S), S (*e)()>
class ImplicitTreap {
   private:
    struct Node {
        S value, product, product_rev;
        Node *l, *r;
        unsigned priority;
        int cnt;
        bool rev;
    };
    using Tree = Node *;

    static constexpr int CHUNK_SIZE = 1 << 20;
    inline static vector<Node *> chunks;
    inline static int chunk_pos = CHUNK_SIZE;

    Tree root = nullptr;
    unsigned rng_state = 123456789;

    unsigned rng() {
        unsigned x = rng_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return rng_state = x;
    }

    Node *new_node(const S &value) {
        if (chunk_pos == CHUNK_SIZE) {
            chunks.push_back(new Node[CHUNK_SIZE]);
            chunk_pos = 0;
        }
        Node *p = &chunks.back()[chunk_pos++];
        *p = {value, value, value, nullptr, nullptr, rng(), 1, false};
        return p;
    }

    int count(Tree t) const { return t ? t->cnt : 0; }
    S product(Tree t) const { return t ? t->product : e(); }
    S product_rev(Tree t) const { return t ? t->product_rev : e(); }

    void toggle_reverse(Tree t) {
        if (!t) return;
        t->rev = !t->rev;
        swap(t->l, t->r);
        swap(t->product, t->product_rev);
    }

    void push(Tree t) {
        if (!t || !t->rev) return;
        toggle_reverse(t->l);
        toggle_reverse(t->r);
        t->rev = false;
    }

    Tree update(Tree t) {
        if (!t) return nullptr;
        t->cnt = count(t->l) + count(t->r) + 1;
        t->product = op(op(product(t->l), t->value), product(t->r));
        t->product_rev = op(op(product_rev(t->r), t->value), product_rev(t->l));
        return t;
    }

    Tree merge(Tree l, Tree r) {
        if (!l) return r;
        if (!r) return l;
        if (l->priority > r->priority) {
            push(l);
            l->r = merge(l->r, r);
            return update(l);
        }
        push(r);
        r->l = merge(l, r->l);
        return update(r);
    }

    pair<Tree, Tree> split(Tree t, int k) {
        if (!t) return {nullptr, nullptr};
        push(t);
        int lc = count(t->l);
        if (k <= lc) {
            auto [a, b] = split(t->l, k);
            t->l = b;
            return {a, update(t)};
        }
        auto [a, b] = split(t->r, k - lc - 1);
        t->r = a;
        return {update(t), b};
    }

    S get(Tree t, int k) {
        while (true) {
            push(t);
            int lc = count(t->l);
            if (k < lc) {
                t = t->l;
            } else if (k == lc) {
                return t->value;
            } else {
                k -= lc + 1;
                t = t->r;
            }
        }
    }

    void collect(Tree t, vector<S> &res) {
        if (!t) return;
        push(t);
        collect(t->l, res);
        res.push_back(t->value);
        collect(t->r, res);
        update(t);
    }

   public:
    ImplicitTreap() = default;
    explicit ImplicitTreap(const vector<S> &v) {
        for (const S &x : v) push_back(x);
    }

    int size() const { return count(root); }
    bool empty() const { return size() == 0; }
    S all_prod() const { return product(root); }

    S get(int k) {
        assert(0 <= k && k < size());
        return get(root, k);
    }

    void set(int k, const S &value) {
        assert(0 <= k && k < size());
        auto [a, b] = split(root, k);
        auto [c, d] = split(b, 1);
        c->value = value;
        root = merge(a, merge(update(c), d));
    }

    void insert(int k, const S &value) {
        assert(0 <= k && k <= size());
        auto [a, b] = split(root, k);
        root = merge(merge(a, new_node(value)), b);
    }

    void push_front(const S &value) { insert(0, value); }
    void push_back(const S &value) { insert(size(), value); }

    S erase(int k) {
        assert(0 <= k && k < size());
        auto [a, b] = split(root, k);
        auto [c, d] = split(b, 1);
        S res = c->value;
        root = merge(a, d);
        return res;
    }

    void erase(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);
        root = merge(a, d);
    }

    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);
        S res = product(c);
        root = merge(a, merge(c, d));
        return res;
    }

    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        auto [a, b] = split(root, l);
        auto [c, d] = split(b, r - l);
        toggle_reverse(c);
        root = merge(a, merge(c, d));
    }

    void merge_back(ImplicitTreap &other) {
        root = merge(root, other.root);
        other.root = nullptr;
    }

    vector<S> to_vector() {
        vector<S> res;
        res.reserve(size());
        collect(root, res);
        return res;
    }
};
