template <class T>
class Treap {
   private:
    mt19937 rnd;
    struct Node {
        T key;
        Node *l, *r;
        uint priority;
        int cnt;
        Node(T key, uint priority) : key(key), l(nullptr), r(nullptr), priority(priority), cnt(1){};
    } *root = nullptr;
    using Tree = Node *;

    int count(const Tree t) { return !t ? 0 : t->cnt; }
    Tree update(Tree t) {
        t->cnt = count(t->l) + count(t->r) + 1;
        return t;
    }
    bool find(const Tree &t, T key) {
        if (!t) {
            return false;
        } else if (t->key == key) {
            return true;
        } else {
            return find(key < t->key ? t->l : t->r, key);
        }
    }

    Tree merge(Tree l, Tree r) {
        if (!l || !r) {
            return l ? l : r;
        }
        if (l->priority > r->priority) {
            l->r = merge(l->r, r);
            return update(l);
        } else {
            r->l = merge(l, r->l);
            return update(r);
        }
    }
    pair<Tree, Tree> split(Tree &t, int k) {
        if (!t) {
            return {nullptr, nullptr};
        }
        if (k <= count(t->l)) {
            auto s = split(t->l, k);
            t->l = s.second;
            return {s.first, update(t)};
        } else {
            auto s = split(t->r, k - count(t->l) - 1);
            t->r = s.first;
            return {update(t), s.second};
        }
        return {nullptr, nullptr};
    }

    Tree insert(Tree &t, Tree item) {
        if (!t) {
            t = item;
            return t;
        } else {
            auto s = split(t, lower_bound(t, item->key));
            return merge(merge(s.first, item), s.second);
        }
    }

    ll lower_bound(Tree &t, T key) {
        if (!t) return 0;
        if (key <= t->key) {
            return lower_bound(t->l, key);
        } else {
            return count(t->l) + 1 + lower_bound(t->r, key);
        }
    }

    ll upper_bound(Tree &t, T key) {
        if (!t) return 0;
        if (key < t->key) {
            return upper_bound(t->l, key);
        } else {
            return count(t->l) + 1 + upper_bound(t->r, key);
        }
    }

    optional<T> prev(Tree &t, T x) {
        if (!t) return nullopt;
        if (t->key < x) {
            auto res = prev(t->r, x);
            return res ? res : t->key;
        } else {
            return prev(t->l, x);
        }
    }
    optional<T> next(Tree &t, T x) {
        if (!t) return nullopt;
        if (t->key > x) {
            auto res = next(t->l, x);
            return res ? res: t->key;
        } else {
            return next(t->r, x);
        }
    }

    T min(const Tree &t) { return t->l ? min(t->l) : t->key; }
    T max(const Tree &t) { return t->r ? max(t->r) : t->key; }

    T kth(Tree &t, int k) {
        if (count(t->l) > k) {
            return kth(t->l, k);
        } else if (count(t->l) == k) {
            return t->key;
        } else {
            return kth(t->r, k - count(t->l) - 1);
        }
    }

   public:
    Treap() {
        random_device seed_gen;
        mt19937 rnd(seed_gen());
    };
    void insert(T key) { root = insert(root, new Node(key, rnd())); }
    bool find(T key) { return find(root, key); }
    T min() { return min(root); }
    T max() { return max(root); }
    T prev(T key) { return prev(root, key).value(); }
    T next(T key) { return next(root, key).value(); }
    ll lower_bound(T key) { return lower_bound(root, key); }
    ll upper_bound(T key) { return upper_bound(root, key); }
    pair<int, int> equal_range(T key) { return {lower_bound(key), upper_bound(key)}; }
    pair<Tree, Tree> split(T key) { return split(root, lower_bound(key)); }
    bool erase(T key) {
        int li = lower_bound(key);
        int ri = upper_bound(key);
        if (li == ri) return false;
        auto s1 = split(root, li);
        auto s2 = split(s1.second, 1);
        root = merge(s1.first, s2.second);
        return true;
    }

    T kth(int k) { return kth(root, k); }
    int size() { return root ? root->cnt : 0; }
};