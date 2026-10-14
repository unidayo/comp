// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/treap/implicit_treap_segtree.cpp
// 区間積を持つ Implicit Treap。
//
// 使い方:
//   ll op(ll a, ll b) { return a + b; }
//   ll e() { return 0; }
//   ImplicitTreapSegtree<ll, op, e> tr({1, 2, 3});
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
//   - vectorからの構築は O(N)。
//   - 各操作の計算量は期待 O(log N)。`to_vector` は O(N)。

template <class S, S (*op)(S, S), S (*e)()>
class ImplicitTreapSegtree {
   private:
    struct Node {
        S value, product, product_rev;
        Node *l, *r;
        unsigned priority;
        int cnt;
        bool rev;
    };
    using Tree = Node *;

    struct FreeNode { FreeNode *next; };
    static constexpr int CHUNK_SIZE = 1 << 20;
    inline static vector<Node *> chunks;
    inline static FreeNode *free_head = nullptr;
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
        Node *p;
        if (free_head) [[unlikely]] {
            FreeNode *slot = free_head;
            free_head = free_head->next;
            p = reinterpret_cast<Node *>(slot);
        } else {
            if (chunk_pos == CHUNK_SIZE) {
                chunks.push_back(static_cast<Node *>(::operator new(sizeof(Node) * CHUNK_SIZE)));
                chunk_pos = 0;
            }
            p = &chunks.back()[chunk_pos++];
        }
        ::new (static_cast<void *>(p)) Node{value, value, value, nullptr, nullptr, rng(), 1, false};
        return p;
    }

    static void release_node(Node *p) {
        p->~Node();
        free_head = ::new (static_cast<void *>(p)) FreeNode{free_head};
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

    pair<Tree, S> erase_node(Tree t, int k) {
        push(t);
        int left_count = count(t->l);
        if (k == left_count) {
            Tree result_tree = merge(t->l, t->r);
            S result = std::move(t->value);
            release_node(t);
            return {result_tree, std::move(result)};
        }
        if (k < left_count) {
            auto [child, result] = erase_node(t->l, k);
            t->l = child;
            return {update(t), std::move(result)};
        }
        auto [child, result] = erase_node(t->r, k - left_count - 1);
        t->r = child;
        return {update(t), std::move(result)};
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

    void set_node(Tree t, int k, const S &value) {
        push(t);
        int left_count = count(t->l);
        if (k < left_count) set_node(t->l, k, value);
        else if (k == left_count) t->value = value;
        else set_node(t->r, k - left_count - 1, value);
        update(t);
    }

    void collect(Tree t, vector<S> &res) {
        if (!t) return;
        push(t);
        collect(t->l, res);
        res.push_back(t->value);
        collect(t->r, res);
        update(t);
    }

    void rebuild(Tree t) {
        if (!t) return;
        rebuild(t->l);
        rebuild(t->r);
        update(t);
    }

    Tree build_from_vector(const vector<S> &v) {
        vector<Tree> stack;
        stack.reserve(v.size());
        for (const S &value : v) {
            Tree node = new_node(value);
            Tree last = nullptr;
            while (!stack.empty() && stack.back()->priority < node->priority) {
                last = stack.back();
                stack.pop_back();
            }
            node->l = last;
            if (!stack.empty()) stack.back()->r = node;
            stack.push_back(node);
        }
        Tree result = stack.empty() ? nullptr : stack.front();
        rebuild(result);
        return result;
    }

   public:
    ImplicitTreapSegtree() = default;
    ImplicitTreapSegtree(const ImplicitTreapSegtree &) = delete;
    ImplicitTreapSegtree &operator=(const ImplicitTreapSegtree &) = delete;
    ImplicitTreapSegtree(ImplicitTreapSegtree &&other) noexcept
        : root(std::exchange(other.root, nullptr)), rng_state(other.rng_state) {}
    ImplicitTreapSegtree &operator=(ImplicitTreapSegtree &&other) noexcept {
        if (this != &other) {
            root = std::exchange(other.root, nullptr);
            rng_state = other.rng_state;
        }
        return *this;
    }
    explicit ImplicitTreapSegtree(const vector<S> &v) { root = build_from_vector(v); }

    int size() const { return count(root); }
    bool empty() const { return size() == 0; }
    S all_prod() const { return product(root); }

    S get(int k) {
        assert(0 <= k && k < size());
        return get(root, k);
    }

    void set(int k, const S &value) {
        assert(0 <= k && k < size());
        set_node(root, k, value);
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
        auto [new_root, result] = erase_node(root, k);
        root = new_root;
        return result;
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

    void merge_back(ImplicitTreapSegtree &other) {
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
