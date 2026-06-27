// Dynamic Wavelet Matrix。
//
// 使い方:
//   DynamicWaveletMatrix<long long> wm(a);  // 負の値を含む signed 整数にも対応
//
//   wm.access(k);                 // a[k]
//   wm.set(k, x);                 // a[k] = x
//   wm.insert(k, x);              // k 番目の直前に x を挿入
//   wm.erase(k);                  // k 番目を削除して値を返す。なければ nullopt
//   wm.rank(r, x);                // [0, r) にある x の個数
//   wm.rank(l, r, x);             // [l, r) にある x の個数
//   wm.kth_smallest(l, r, k);     // [l, r) の k 番目に小さい値（0-indexed）
//   wm.kth_largest(l, r, k);      // [l, r) の k 番目に大きい値（0-indexed）
//   wm.range_freq(l, r, upper);   // [l, r) にある upper 未満の値の個数
//   wm.range_freq(l, r, lower, upper);  // [l, r) にある [lower, upper) の値の個数
//   wm.prev_value(l, r, upper);   // [l, r) にある upper 未満の最大値。なければ nullopt
//   wm.next_value(l, r, lower);   // [l, r) にある lower 以上の最小値。なければ nullopt
//
// Dynamic Range Count-Distinct:
//   DynamicRangeCountDistinct<long long> cd(a);
//   cd.set(k, x);
//   cd.insert(k, x);
//   cd.erase(k);
//   cd.count(l, r);               // [l, r) にある相異なる値の個数
//   cd.kth_smallest(l, r, k);     // 通常の Dynamic Wavelet Matrix 機能も使える
//
// 注意:
//   - T は整数型を想定。
//   - デフォルトの LOG では signed の負の値にも対応し、大小順も正しく扱う。
//     内部では符号ビットを反転して unsigned として並べる。
//   - signed 型で LOG を小さく指定する場合は、値が非負で 2^LOG 未満に収まる用途を想定。
//   - 各階層を implicit treap の動的 bitvector で持つ。
//   - 構築 O(N log A log N)、各操作 O(log A log N)。

template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits>
class DynamicWaveletMatrix {
    static_assert(is_integral_v<T>, "DynamicWaveletMatrix<T>: T must be an integral type");

   private:
    using U = make_unsigned_t<T>;
    static constexpr int FULL_LOG = numeric_limits<U>::digits;

    class DynamicBitVector {
        struct Node {
            Node *l, *r;
            unsigned priority;
            int cnt, sum;
            bool bit;
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

        Node *new_node(bool bit) {
            if (chunk_pos == CHUNK_SIZE) {
                chunks.push_back(new Node[CHUNK_SIZE]);
                chunk_pos = 0;
            }
            Node *p = &chunks.back()[chunk_pos++];
            *p = {nullptr, nullptr, rng(), 1, bit ? 1 : 0, bit};
            return p;
        }

        int count(Tree t) const { return t ? t->cnt : 0; }
        int sum(Tree t) const { return t ? t->sum : 0; }

        Tree update(Tree t) {
            if (!t) return nullptr;
            t->cnt = count(t->l) + 1 + count(t->r);
            t->sum = sum(t->l) + (t->bit ? 1 : 0) + sum(t->r);
            return t;
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

        pair<Tree, Tree> split(Tree t, int k) {
            if (!t) return {nullptr, nullptr};
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

      public:
        DynamicBitVector() = default;

        int size() const { return count(root); }
        int count1() const { return sum(root); }
        int count0() const { return size() - count1(); }

        bool get(int k) const {
            assert(0 <= k && k < size());
            Tree t = root;
            while (true) {
                int lc = count(t->l);
                if (k < lc) {
                    t = t->l;
                } else if (k == lc) {
                    return t->bit;
                } else {
                    k -= lc + 1;
                    t = t->r;
                }
            }
        }

        int rank1(int k) const {
            assert(0 <= k && k <= size());
            int res = 0;
            Tree t = root;
            while (t) {
                int lc = count(t->l);
                if (k <= lc) {
                    t = t->l;
                } else {
                    res += sum(t->l);
                    if (t->bit) res++;
                    k -= lc + 1;
                    t = t->r;
                }
            }
            return res;
        }

        int rank0(int k) const {
            assert(0 <= k && k <= size());
            return k - rank1(k);
        }

        void insert(int k, bool bit) {
            assert(0 <= k && k <= size());
            auto [a, b] = split(root, k);
            root = merge(merge(a, new_node(bit)), b);
        }

        optional<bool> erase(int k) {
            if (k < 0 || k >= size()) return nullopt;
            auto [a, b] = split(root, k);
            auto [c, d] = split(b, 1);
            bool res = c->bit;
            root = merge(a, d);
            return res;
        }
    };

    int n = 0;
    array<DynamicBitVector, LOG> matrix;
    array<int, LOG> zeros{};

    static U sign_mask() {
        if constexpr (FULL_LOG == 0) return U(0);
        return U(1) << (FULL_LOG - 1);
    }

    static U encode(T x) {
        U u = static_cast<U>(x);
        if constexpr (is_signed_v<T> && LOG == FULL_LOG) {
            return u ^ sign_mask();
        } else {
            return u;
        }
    }

    static T decode(U x) {
        if constexpr (is_signed_v<T> && LOG == FULL_LOG) {
            x ^= sign_mask();
        }
        return static_cast<T>(x);
    }

    static bool bit_at(U x, int b) { return (x >> b) & U(1); }

    int range_freq_less_encoded(int l, int r, U upper) const {
        int res = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            if (bit_at(upper, b)) {
                res += r0 - l0;
                l = zeros[level] + l - l0;
                r = zeros[level] + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }
        return res;
    }

   public:
    DynamicWaveletMatrix() = default;
    explicit DynamicWaveletMatrix(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        *this = DynamicWaveletMatrix();
        for (const T &x : v) insert(size(), x);
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T access(int k) const {
        assert(0 <= k && k < n);
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            bool bit = matrix[level].get(k);
            if (bit) {
                x |= U(1) << b;
                k = zeros[level] + matrix[level].rank1(k);
            } else {
                k = matrix[level].rank0(k);
            }
        }
        return decode(x);
    }

    void insert(int k, T value) {
        assert(0 <= k && k <= n);
        U x = encode(value);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            bool bit = bit_at(x, b);
            int next_k;
            if (bit) {
                next_k = zeros[level] + matrix[level].rank1(k);
            } else {
                next_k = matrix[level].rank0(k);
                zeros[level]++;
            }
            matrix[level].insert(k, bit);
            k = next_k;
        }
        n++;
    }

    optional<T> erase(int k) {
        if (k < 0 || k >= n) return nullopt;
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            bool bit = matrix[level].get(k);
            int next_k;
            if (bit) {
                x |= U(1) << b;
                next_k = zeros[level] + matrix[level].rank1(k);
            } else {
                next_k = matrix[level].rank0(k);
                zeros[level]--;
            }
            matrix[level].erase(k);
            k = next_k;
        }
        n--;
        return decode(x);
    }

    void set(int k, T value) {
        assert(0 <= k && k < n);
        erase(k);
        insert(k, value);
    }

    int rank(int r, T x) const {
        assert(0 <= r && r <= n);
        return rank(0, r, x);
    }

    int rank(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        U y = encode(x);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            if (bit_at(y, b)) {
                l = zeros[level] + l - l0;
                r = zeros[level] + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }
        return r - l;
    }

    T kth_smallest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            int l0 = matrix[level].rank0(l);
            int r0 = matrix[level].rank0(r);
            int zero_count = r0 - l0;
            if (k < zero_count) {
                l = l0;
                r = r0;
            } else {
                x |= U(1) << b;
                k -= zero_count;
                l = zeros[level] + l - l0;
                r = zeros[level] + r - r0;
            }
        }
        return decode(x);
    }

    T kth_largest(int l, int r, int k) const {
        assert(0 <= l && l <= r && r <= n);
        assert(0 <= k && k < r - l);
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int range_freq(int l, int r, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        return range_freq_less_encoded(l, r, encode(upper));
    }

    int range_freq(int l, int r, T lower, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        if (!(lower < upper)) return 0;
        return range_freq(l, r, upper) - range_freq(l, r, lower);
    }

    optional<T> prev_value(int l, int r, T upper) const {
        assert(0 <= l && l <= r && r <= n);
        int cnt = range_freq(l, r, upper);
        if (cnt == 0) return nullopt;
        return kth_smallest(l, r, cnt - 1);
    }

    optional<T> next_value(int l, int r, T lower) const {
        assert(0 <= l && l <= r && r <= n);
        int cnt = range_freq(l, r, lower);
        if (cnt == r - l) return nullopt;
        return kth_smallest(l, r, cnt);
    }

    vector<T> to_vector() const {
        vector<T> res;
        res.reserve(n);
        for (int i = 0; i < n; i++) res.push_back(access(i));
        return res;
    }
};

// 動的配列の区間 distinct 数。
//
// 実装:
//   各値ごとに出現位置の set を持つ。
//
// 計算量:
//   set O(log N)
//   insert/erase O(N log N) 位置をずらすため重い
//   count O(M log N) M は異なる値の種類数
//
// 注意:
//   - 挿入削除が多い巨大ケース向けではない。
//   - 値の種類数が小さい、または count distinct が補助的な用途向け。
template <class T>
class DynamicRangeCountDistinct {
   private:
    DynamicWaveletMatrix<T> wm;
    map<T, std::set<int>> positions;

    void rebuild_positions() {
        positions.clear();
        for (int i = 0; i < wm.size(); i++) {
            positions[wm.access(i)].insert(i);
        }
    }

   public:
    DynamicRangeCountDistinct() = default;
    explicit DynamicRangeCountDistinct(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        wm.build(v);
        rebuild_positions();
    }

    int size() const { return wm.size(); }
    bool empty() const { return wm.empty(); }

    T access(int k) const { return wm.access(k); }

    void set(int k, const T &x) {
        assert(0 <= k && k < size());
        T old = wm.access(k);
        auto it = positions.find(old);
        assert(it != positions.end());
        it->second.erase(k);
        if (it->second.empty()) positions.erase(it);
        wm.set(k, x);
        positions[x].insert(k);
    }

    void insert(int k, const T &x) {
        assert(0 <= k && k <= size());
        wm.insert(k, x);
        rebuild_positions();
    }

    optional<T> erase(int k) {
        if (k < 0 || k >= size()) return nullopt;
        optional<T> res = wm.erase(k);
        rebuild_positions();
        return res;
    }

    int rank(int r, T x) const { return wm.rank(r, x); }
    int rank(int l, int r, T x) const { return wm.rank(l, r, x); }
    T kth_smallest(int l, int r, int k) const { return wm.kth_smallest(l, r, k); }
    T kth_largest(int l, int r, int k) const { return wm.kth_largest(l, r, k); }
    int range_freq(int l, int r, T upper) const { return wm.range_freq(l, r, upper); }
    int range_freq(int l, int r, T lower, T upper) const { return wm.range_freq(l, r, lower, upper); }
    optional<T> prev_value(int l, int r, T upper) const { return wm.prev_value(l, r, upper); }
    optional<T> next_value(int l, int r, T lower) const { return wm.next_value(l, r, lower); }

    int count(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        int res = 0;
        for (const auto &[value, pos] : positions) {
            auto it = pos.lower_bound(l);
            if (it != pos.end() && *it < r) res++;
        }
        return res;
    }

    vector<T> to_vector() const { return wm.to_vector(); }
};
