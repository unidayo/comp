// 最終更新: 2026-07-12 14:27:44 JST
template <class S, S (*op)(S, S), S (*e)()>
class dynamic_segtree {
   public:
    explicit dynamic_segtree(size_t size) : n(size), root(nullptr) {}
    dynamic_segtree(const dynamic_segtree &) = delete;
    dynamic_segtree &operator=(const dynamic_segtree &) = delete;
    dynamic_segtree(dynamic_segtree &&) = delete;
    dynamic_segtree &operator=(dynamic_segtree &&) = delete;

    ~dynamic_segtree() {
        destroy_tree(root, false);
        for (node *block : blocks) allocator.deallocate(block, block_size);
    }

    void set(size_t p, S x) {
        assert(p < n);
        set(root, 0, n, p, x);
    }

    S get(size_t p) const {
        assert(p < n);
        return get(root, 0, n, p);
    }

    S prod(size_t l, size_t r) const {
        assert(l <= r && r <= n);
        return prod(root, 0, n, l, r);
    }

    S all_prod() const { return root ? root->product : e(); }

    void reset(size_t l, size_t r) {
        assert(l <= r && r <= n);
        return reset(root, 0, n, l, r);
    }

    template <bool (*f)(S)>
    size_t max_right(size_t l) const {
        return max_right(l, [](S x) { return f(x); });
    }

    template <class F>
    size_t max_right(size_t l, const F &f) const {
        assert(l <= n);
        S product = e();
        assert(f(product));
        return max_right(root, 0, n, l, f, product);
    }

    template <bool (*f)(S)>
    size_t min_left(size_t r) const {
        return min_left(r, [](S x) { return f(x); });
    }

    template <class F>
    size_t min_left(size_t r, const F &f) const {
        assert(r <= n);
        S product = e();
        assert(f(product));
        return min_left(root, 0, n, r, f, product);
    }

   private:
    struct node;

    struct node {
        size_t index;
        S value, product;
        node *left, *right;

        node(size_t position, S initial_value)
            : index(position), value(initial_value), product(initial_value), left(nullptr), right(nullptr) {}

        void update() { product = op(op(left ? left->product : e(), value), right ? right->product : e()); }
    };

    const size_t n;
    node *root;

    // ノードをまとめて確保し、new/delete をノードごとに呼ぶコストを避ける。
    // reset で不要になったノードは free_nodes に戻して再利用する。
    static constexpr size_t block_size = 256;
    std::allocator<node> allocator;
    std::vector<node *> blocks;
    std::vector<node *> free_nodes;
    node *next_node = nullptr;
    size_t nodes_left = 0;

    node *make_node(size_t index, const S &value) {
        node *p;
        if (!free_nodes.empty()) {
            p = free_nodes.back();
            free_nodes.pop_back();
        } else {
            if (nodes_left == 0) {
                next_node = allocator.allocate(block_size);
                blocks.push_back(next_node);
                nodes_left = block_size;
            }
            p = next_node++;
            --nodes_left;
        }
        std::construct_at(p, index, value);
        return p;
    }

    void destroy_tree(node *p, bool recycle = true) {
        if (!p) return;
        destroy_tree(p->left, recycle);
        destroy_tree(p->right, recycle);
        std::destroy_at(p);
        if (recycle) free_nodes.push_back(p);
    }

    void set(node *&t, size_t a, size_t b, size_t p, S x) {
        if (!t) {
            t = make_node(p, x);
            return;
        }
        if (t->index == p) {
            t->value = x;
            t->update();
            return;
        }
        size_t c = (a + b) >> 1;
        if (p < c) {
            if (t->index < p) std::swap(t->index, p), std::swap(t->value, x);
            set(t->left, a, c, p, x);
        } else {
            if (p < t->index) std::swap(p, t->index), std::swap(x, t->value);
            set(t->right, c, b, p, x);
        }
        t->update();
    }

    S get(const node *t, size_t a, size_t b, size_t p) const {
        if (!t) return e();
        if (t->index == p) return t->value;
        size_t c = (a + b) >> 1;
        if (p < c)
            return get(t->left, a, c, p);
        else
            return get(t->right, c, b, p);
    }

    S prod(const node *t, size_t a, size_t b, size_t l, size_t r) const {
        if (!t || b <= l || r <= a) return e();
        if (l <= a && b <= r) return t->product;
        size_t c = (a + b) >> 1;
        S result = prod(t->left, a, c, l, r);
        if (l <= t->index && t->index < r) result = op(result, t->value);
        return op(result, prod(t->right, c, b, l, r));
    }

    void reset(node *&t, size_t a, size_t b, size_t l, size_t r) {
        if (!t || b <= l || r <= a) return;
        if (l <= a && b <= r) {
            destroy_tree(t);
            t = nullptr;
            return;
        }
        size_t c = (a + b) >> 1;
        reset(t->left, a, c, l, r);
        reset(t->right, c, b, l, r);
        if (l <= t->index && t->index < r) t->value = e();
        t->update();
    }

    template <class F>
    size_t max_right(const node *t, size_t a, size_t b, size_t l, const F &f, S &product) const {
        if (!t || b <= l) return n;
        if (f(op(product, t->product))) {
            product = op(product, t->product);
            return n;
        }
        size_t c = (a + b) >> 1;
        size_t result = max_right(t->left, a, c, l, f, product);
        if (result != n) return result;
        if (l <= t->index) {
            product = op(product, t->value);
            if (!f(product)) return t->index;
        }
        return max_right(t->right, c, b, l, f, product);
    }

    template <class F>
    size_t min_left(const node *t, size_t a, size_t b, size_t r, const F &f, S &product) const {
        if (!t || r <= a) return 0;
        if (f(op(t->product, product))) {
            product = op(t->product, product);
            return 0;
        }
        size_t c = (a + b) >> 1;
        size_t result = min_left(t->right, c, b, r, f, product);
        if (result != 0) return result;
        if (t->index < r) {
            product = op(t->value, product);
            if (!f(product)) return t->index + 1;
        }
        return min_left(t->left, a, c, r, f, product);
    }
};
