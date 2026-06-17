template <class T>
class Treap {
   private:
    struct Node {
        T key;
        Node *l, *r;
        uint priority;
        int cnt;
    } *root = nullptr;
    using Tree = Node *;

    static constexpr int CHUNK_SIZE = 1 << 20;
    inline static vector<Node *> chunks;
    inline static int chunk_pos = CHUNK_SIZE;
    uint rng_state = 123456789;

    uint rng() {
        uint x = rng_state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return rng_state = x;
    }

    Node *new_node(T key) {
        if (chunk_pos == CHUNK_SIZE) {
            chunks.push_back(new Node[CHUNK_SIZE]);
            chunk_pos = 0;
        }
        Node *p = &chunks.back()[chunk_pos++];
        *p = {key, nullptr, nullptr, rng(), 1};
        return p;
    }

    int count(const Tree t) const { return t ? t->cnt : 0; }
    Tree update(Tree t) {
        t->cnt = count(t->l) + count(t->r) + 1;
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

    Tree insert(Tree t, T key) {
        if (!t) return new_node(key);
        if (key < t->key) {
            t->l = insert(t->l, key);
            if (t->l->priority > t->priority) t = rotate_right(t);
        } else {
            t->r = insert(t->r, key);
            if (t->r->priority > t->priority) t = rotate_left(t);
        }
        return update(t);
    }

    Tree erase(Tree t, T key) {
        if (!t) return nullptr;
        if (key == t->key) {
            return merge(t->l, t->r);
        }
        if (key < t->key) {
            t->l = erase(t->l, key);
        } else {
            t->r = erase(t->r, key);
        }
        return update(t);
    }

    bool find(Tree t, T key) const {
        while (t) {
            if (key == t->key) return true;
            t = key < t->key ? t->l : t->r;
        }
        return false;
    }

    int lower_bound(Tree t, T key) const {
        int res = 0;
        while (t) {
            if (key <= t->key) {
                t = t->l;
            } else {
                res += count(t->l) + 1;
                t = t->r;
            }
        }
        return res;
    }

    int upper_bound(Tree t, T key) const {
        int res = 0;
        while (t) {
            if (key < t->key) {
                t = t->l;
            } else {
                res += count(t->l) + 1;
                t = t->r;
            }
        }
        return res;
    }

    optional<T> prev(Tree t, T x) const {
        Tree res = nullptr;
        while (t) {
            if (t->key < x) {
                res = t;
                t = t->r;
            } else {
                t = t->l;
            }
        }
        if (!res) return nullopt;
        return res->key;
    }
    optional<T> next(Tree t, T x) const {
        Tree res = nullptr;
        while (t) {
            if (t->key > x) {
                res = t;
                t = t->l;
            } else {
                t = t->r;
            }
        }
        if (!res) return nullopt;
        return res->key;
    }

    T min(Tree t) const {
        while (t->l) t = t->l;
        return t->key;
    }
    T max(Tree t) const {
        while (t->r) t = t->r;
        return t->key;
    }

    T kth(Tree t, int k) const {
        while (true) {
            int lc = count(t->l);
            if (k < lc) {
                t = t->l;
            } else if (k == lc) {
                return t->key;
            } else {
                k -= lc + 1;
                t = t->r;
            }
        }
    }

   public:
    Treap() = default;
    // key を1つ挿入する
    void insert(T key) { root = insert(root, key); }
    // key が存在しなければ挿入する
    bool insert_unique(T key) {
        if (find(key)) return false;
        root = insert(root, key);
        return true;
    }
    // key が存在するか判定する
    bool find(T key) { return find(root, key); }
    // 最小値を返す
    T min() { return min(root); }
    // 最大値を返す
    T max() { return max(root); }
    // key 未満の最大の要素を返す。存在しない場合は nullopt
    optional<T> prev(T key) { return prev(root, key); }
    // key より大きい最小の要素を返す。存在しない場合は nullopt
    optional<T> next(T key) { return next(root, key); }
    // key 未満の要素の個数（= key 以上の要素が始まるインデックス）を返す
    ll lower_bound(T key) { return lower_bound(root, key); }
    // key 以下の要素の個数（= key より大きい要素が始まるインデックス）を返す
    ll upper_bound(T key) { return upper_bound(root, key); }
    // key に等しい要素の [lower_bound, upper_bound) を返す
    pair<int, int> equal_range(T key) { return {lower_bound(key), upper_bound(key)}; }
    // key を1つ削除する。存在しなければ false を返す
    bool erase(T key) {
        if (!find(key)) return false;
        root = erase(root, key);
        return true;
    }
    // 昇順 k 番目（0-indexed）の要素を返す
    T kth(int k) { return kth(root, k); }
    // 要素数を返す
    int size() { return root ? root->cnt : 0; }
};