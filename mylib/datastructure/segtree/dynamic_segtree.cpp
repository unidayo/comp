// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/segtree/dynamic_segtree.cpp
// 固定長ブロック + 32ビットの子番号。reset で解放した領域を再利用する。
// 詳細は dynamic_segtree.md を参照。
template <class S, S (*op)(S, S), S (*e)()>
class dynamic_segtree {
   public:
    explicit dynamic_segtree(size_t size) : n(size) {}
    dynamic_segtree(const dynamic_segtree &) = delete;
    dynamic_segtree &operator=(const dynamic_segtree &) = delete;
    dynamic_segtree(dynamic_segtree &&) = delete;
    dynamic_segtree &operator=(dynamic_segtree &&) = delete;

    ~dynamic_segtree() {
        destroy_tree(root);
        for (size_t i = 0; i < used_slots; ++i)
            std::destroy_at(blocks[i / block_size] + i % block_size);
        for (Slot *block : blocks) allocator.deallocate(block, block_size);
    }

    void set(size_t p, S x) {
        assert(p < n);
        root = set(root, 0, n, p, x);
    }
    S get(size_t p) const {
        assert(p < n);
        return get(root, 0, n, p);
    }
    S prod(size_t l, size_t r) const {
        assert(l <= r && r <= n);
        return l == r ? e() : prod(root, 0, n, l, r);
    }
    S all_prod() const { return root ? node(root).product : e(); }
    void reset(size_t l, size_t r) {
        assert(l <= r && r <= n);
        if (l != r) root = reset(root, 0, n, l, r);
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

    size_t node_count() const { return live_nodes; }
    // reset 後も確保済みブロックは保持する。
    size_t allocated_node_count() const { return used_slots; }
    static constexpr size_t node_size() { return sizeof(Node); }

   private:
    using index_type = uint32_t;
    struct Node {
        size_t index;
        S value, product;
        index_type left = 0, right = 0;
        Node(size_t position, const S &initial_value)
            : index(position), value(initial_value), product(initial_value) {}
    };
    // 生きているノードと、空き領域の連結リストを同じ領域に保持する。
    // S のデストラクタを呼んでから next_free の生存期間を開始する。
    union Slot {
        Node data;
        index_type next_free;
        Slot() {}
        ~Slot() {}
    };

    const size_t n;
    index_type root = 0, free_head = 0;
    static constexpr size_t block_size = 256;
    std::allocator<Slot> allocator;
    std::vector<Slot *> blocks;
    size_t used_slots = 0, live_nodes = 0;

    Slot &slot(index_type p) { return blocks[(p - 1) / block_size][(p - 1) % block_size]; }
    const Slot &slot(index_type p) const { return blocks[(p - 1) / block_size][(p - 1) % block_size]; }
    Node &node(index_type p) { return slot(p).data; }
    const Node &node(index_type p) const { return slot(p).data; }

    index_type make_node(size_t position, const S &value) {
        index_type p;
        if (free_head) {
            p = free_head;
            index_type next = slot(p).next_free;
            try {
                std::construct_at(&slot(p).data, position, value);
            } catch (...) {
                std::construct_at(&slot(p).next_free, next);
                throw;
            }
            free_head = next;
        } else {
            if (used_slots == std::numeric_limits<index_type>::max())
                throw std::length_error("dynamic_segtree node limit");
            if (used_slots % block_size == 0 && used_slots / block_size == blocks.size()) {
                Slot *block = allocator.allocate(block_size);
                try {
                    blocks.push_back(block);
                } catch (...) {
                    allocator.deallocate(block, block_size);
                    throw;
                }
            }
            p = static_cast<index_type>(used_slots + 1);
            std::construct_at(&slot(p));
            try {
                std::construct_at(&slot(p).data, position, value);
            } catch (...) {
                std::destroy_at(&slot(p));
                throw;
            }
            ++used_slots;
        }
        ++live_nodes;
        return p;
    }

    void destroy_tree(index_type p) {
        if (!p) return;
        destroy_tree(node(p).left);
        destroy_tree(node(p).right);
        std::destroy_at(&slot(p).data);
        std::construct_at(&slot(p).next_free, free_head);
        free_head = p;
        --live_nodes;
    }

    void update(index_type p) {
        Node &t = node(p);
        t.product = op(op(t.left ? node(t.left).product : e(), t.value),
                       t.right ? node(t.right).product : e());
    }

    index_type set(index_type t, size_t a, size_t b, size_t p, S x) {
        if (!t) return make_node(p, x);
        Node &v = node(t);  // ブロックのアドレスは再帰中も変わらない。
        if (v.index == p) {
            v.value = x;
            update(t);
            return t;
        }
        size_t c = a + (b - a) / 2;
        if (p < c) {
            if (v.index < p) std::swap(v.index, p), std::swap(v.value, x);
            v.left = set(v.left, a, c, p, x);
        } else {
            if (p < v.index) std::swap(p, v.index), std::swap(x, v.value);
            v.right = set(v.right, c, b, p, x);
        }
        update(t);
        return t;
    }

    S get(index_type t, size_t a, size_t b, size_t p) const {
        if (!t) return e();
        const Node &v = node(t);
        if (v.index == p) return v.value;
        size_t c = a + (b - a) / 2;
        return p < c ? get(v.left, a, c, p) : get(v.right, c, b, p);
    }

    S prod(index_type t, size_t a, size_t b, size_t l, size_t r) const {
        if (!t || b <= l || r <= a) return e();
        const Node &v = node(t);
        if (l <= a && b <= r) return v.product;
        size_t c = a + (b - a) / 2;
        S result = prod(v.left, a, c, l, r);
        if (l <= v.index && v.index < r) result = op(result, v.value);
        return op(result, prod(v.right, c, b, l, r));
    }

    index_type reset(index_type t, size_t a, size_t b, size_t l, size_t r) {
        if (!t || b <= l || r <= a) return t;
        if (l <= a && b <= r) {
            destroy_tree(t);
            return 0;
        }
        Node &v = node(t);
        size_t c = a + (b - a) / 2;
        v.left = reset(v.left, a, c, l, r);
        v.right = reset(v.right, c, b, l, r);
        if (l <= v.index && v.index < r) v.value = e();
        update(t);
        return t;
    }

    template <class F>
    size_t max_right(index_type t, size_t a, size_t b, size_t l, const F &f, S &product) const {
        if (!t || b <= l) return n;
        const Node &v = node(t);
        // 探索範囲に丸ごと含まれる部分木だけをまとめて採用する。
        if (l <= a) {
            S next = op(product, v.product);
            if (f(next)) {
                product = std::move(next);
                return n;
            }
        }
        size_t c = a + (b - a) / 2;
        size_t result = max_right(v.left, a, c, l, f, product);
        if (result != n) return result;
        if (l <= v.index) {
            product = op(product, v.value);
            if (!f(product)) return v.index;
        }
        return max_right(v.right, c, b, l, f, product);
    }

    template <class F>
    size_t min_left(index_type t, size_t a, size_t b, size_t r, const F &f, S &product) const {
        if (!t || r <= a) return 0;
        const Node &v = node(t);
        if (b <= r) {
            S next = op(v.product, product);
            if (f(next)) {
                product = std::move(next);
                return 0;
            }
        }
        size_t c = a + (b - a) / 2;
        size_t result = min_left(v.right, c, b, r, f, product);
        if (result != 0) return result;
        if (v.index < r) {
            product = op(v.value, product);
            if (!f(product)) return v.index + 1;
        }
        return min_left(v.left, a, c, r, f, product);
    }
};
