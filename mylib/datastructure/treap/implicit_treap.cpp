// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/treap/implicit_treap.cpp
// 区間積を持たない軽量な Implicit Treap。
//
// 使い方:
//   ImplicitTreap<long long> tr({1, 2, 3});
//   tr.insert(k, x);        // k番目の直前にxを挿入
//   tr.erase(k);            // k番目を削除して値を返す
//   tr.erase(l, r);         // [l, r) を削除
//   tr.set(k, x);           // k番目をxに変更
//   tr.get(k);              // k番目を取得
//   tr.reverse(l, r);       // [l, r) を反転
//   tr.to_vector();         // 現在の列をvectorにする
//
// 計算量:
//   vectorからの構築: O(N)
//   insert / erase / set / get / reverse / merge_back: 期待 O(log N)
//   size / empty: O(1)
//   to_vector: O(N)
//
// 注意:
//   - 添字は0-indexed、区間は半開区間 [l, r)。
//   - prodやall_prodは持たない。区間積が必要な場合は
//     implicit_treap_segtree.cpp の ImplicitTreapSegtree を使う。

template <class S>
class ImplicitTreap {
   private:
    struct Node {
        S value;
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
        ::new (static_cast<void *>(p)) Node{value, nullptr, nullptr, rng(), 1, false};
        return p;
    }

    static void release_node(Node *p) {
        p->~Node();
        free_head = ::new (static_cast<void *>(p)) FreeNode{free_head};
    }

    static void release_tree(Tree t) {
        if (!t) return;
        release_tree(t->l);
        release_tree(t->r);
        release_node(t);
    }

    static int count(Tree t) { return t ? t->cnt : 0; }

    static void toggle_reverse(Tree t) {
        if (!t) return;
        t->rev = !t->rev;
        swap(t->l, t->r);
    }

    static void push(Tree t) {
        if (!t || !t->rev) return;
        toggle_reverse(t->l);
        toggle_reverse(t->r);
        t->rev = false;
    }

    static Tree update(Tree t) {
        if (t) t->cnt = count(t->l) + count(t->r) + 1;
        return t;
    }

    static Tree merge(Tree l, Tree r) {
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

    static pair<Tree, Tree> split(Tree t, int k) {
        if (!t) return {nullptr, nullptr};
        push(t);
        int left_count = count(t->l);
        if (k <= left_count) {
            auto [a, b] = split(t->l, k);
            t->l = b;
            return {a, update(t)};
        }
        auto [a, b] = split(t->r, k - left_count - 1);
        t->r = a;
        return {update(t), b};
    }

    static pair<Tree, S> erase_node(Tree t, int k) {
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

    static Node *find_node(Tree t, int k) {
        while (true) {
            push(t);
            int left_count = count(t->l);
            if (k < left_count) t = t->l;
            else if (k == left_count) return t;
            else {
                k -= left_count + 1;
                t = t->r;
            }
        }
    }

    static void collect(Tree t, vector<S> &result) {
        if (!t) return;
        push(t);
        collect(t->l, result);
        result.push_back(t->value);
        collect(t->r, result);
    }

    static void rebuild(Tree t) {
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
    ImplicitTreap() = default;
    explicit ImplicitTreap(const vector<S> &v) { root = build_from_vector(v); }
    ImplicitTreap(const ImplicitTreap &) = delete;
    ImplicitTreap &operator=(const ImplicitTreap &) = delete;
    ImplicitTreap(ImplicitTreap &&other) noexcept
        : root(std::exchange(other.root, nullptr)), rng_state(other.rng_state) {}
    ImplicitTreap &operator=(ImplicitTreap &&other) noexcept {
        if (this != &other) {
            release_tree(root);
            root = std::exchange(other.root, nullptr);
            rng_state = other.rng_state;
        }
        return *this;
    }
    ~ImplicitTreap() { release_tree(root); }

    int size() const { return count(root); }
    bool empty() const { return root == nullptr; }

    S get(int k) {
        assert(0 <= k && k < size());
        return find_node(root, k)->value;
    }

    void set(int k, const S &value) {
        assert(0 <= k && k < size());
        find_node(root, k)->value = value;
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
        auto [removed, c] = split(b, r - l);
        root = merge(a, c);
        release_tree(removed);
    }

    void reverse(int l, int r) {
        assert(0 <= l && l <= r && r <= size());
        auto [a, b] = split(root, l);
        auto [middle, c] = split(b, r - l);
        toggle_reverse(middle);
        root = merge(a, merge(middle, c));
    }

    void merge_back(ImplicitTreap &other) {
        root = merge(root, other.root);
        other.root = nullptr;
    }

    vector<S> to_vector() {
        vector<S> result;
        result.reserve(size());
        collect(root, result);
        return result;
    }
};
