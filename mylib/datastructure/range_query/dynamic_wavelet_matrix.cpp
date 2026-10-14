// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/datastructure/range_query/dynamic_wavelet_matrix.cpp
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
//   - 各階層を bit-packed block implicit treap の動的 bitvector で持つ。
//   - 構築 O(N log A) 程度、各操作 O(log A (log N + B))。B=256。

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
            int len, ones;
            std::array<unsigned long long, 4> bits;
        };
        using Tree = Node *;

        static constexpr int BLOCK_BITS = 256;
        static constexpr int WORDS = BLOCK_BITS / 64;
        static constexpr int CHUNK_SIZE = 1 << 14;
        inline static vector<Node *> chunks;
        inline static vector<Node *> free_nodes;
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

        static int popcount64(unsigned long long x) { return __builtin_popcountll(x); }

        static bool get_block_bit(const Node *p, int k) {
            return (p->bits[k >> 6] >> (k & 63)) & 1ULL;
        }

        static void set_block_bit(Node *p, int k, bool bit) {
            unsigned long long mask = 1ULL << (k & 63);
            if (bit) p->bits[k >> 6] |= mask;
            else p->bits[k >> 6] &= ~mask;
        }

        static int block_rank1(const Node *p, int k) {
            int full = k >> 6;
            int rem = k & 63;
            int res = 0;
            for (int i = 0; i < full; i++) res += popcount64(p->bits[i]);
            if (rem) res += popcount64(p->bits[full] & ((1ULL << rem) - 1));
            return res;
        }

        static void recompute_block(Node *p) {
            p->ones = 0;
            for (int i = 0; i < WORDS; i++) p->ones += popcount64(p->bits[i]);
        }

        static void insert_block_bit(Node *p, int k, bool bit) {
            assert(0 <= k && k <= p->len && p->len < BLOCK_BITS);
            int first_word = k >> 6;
            int last_word = p->len >> 6;
            for (int word = last_word; word > first_word; --word) {
                p->bits[word] = (p->bits[word] << 1) | (p->bits[word - 1] >> 63);
            }
            int offset = k & 63;
            unsigned long long low_mask = offset == 0 ? 0 : (1ULL << offset) - 1;
            unsigned long long old = p->bits[first_word];
            p->bits[first_word] = (old & low_mask) | ((old & ~low_mask) << 1);
            set_block_bit(p, k, bit);
            p->len++;
            if (bit) p->ones++;
        }

        static bool erase_block_bit(Node *p, int k) {
            assert(0 <= k && k < p->len);
            bool res = get_block_bit(p, k);
            int first_word = k >> 6;
            int last_word = (p->len - 1) >> 6;
            int offset = k & 63;
            unsigned long long low_mask = offset == 0 ? 0 : (1ULL << offset) - 1;
            unsigned long long old = p->bits[first_word];
            unsigned long long shifted = (old >> 1);
            if (first_word < last_word) shifted |= p->bits[first_word + 1] << 63;
            p->bits[first_word] = (old & low_mask) | (shifted & ~low_mask);
            for (int word = first_word + 1; word <= last_word; ++word) {
                p->bits[word] >>= 1;
                if (word < last_word) p->bits[word] |= p->bits[word + 1] << 63;
            }
            p->len--;
            set_block_bit(p, p->len, false);
            if (res) p->ones--;
            return res;
        }

        Node *new_node() {
            Node *p;
            if (!free_nodes.empty()) {
                p = free_nodes.back();
                free_nodes.pop_back();
            } else {
                if (chunk_pos == CHUNK_SIZE) {
                    chunks.push_back(static_cast<Node *>(::operator new(sizeof(Node) * CHUNK_SIZE)));
                    chunk_pos = 0;
                }
                p = &chunks.back()[chunk_pos++];
            }
            ::new (static_cast<void *>(p)) Node{nullptr, nullptr, rng(), 0, 0, 0, 0, {}};
            return p;
        }

        static void release_node(Node *p) {
            p->~Node();
            free_nodes.push_back(p);
        }

        static void release_tree(Tree t) {
            if (!t) return;
            release_tree(t->l);
            release_tree(t->r);
            release_node(t);
        }

        Node *new_block_from(const vector<bool> &v, int l, int r) {
            Node *p = new_node();
            p->len = r - l;
            for (int i = l; i < r; i++) {
                if (v[i]) set_block_bit(p, i - l, true);
            }
            recompute_block(p);
            return update(p);
        }

        Node *new_single(bool bit) {
            Node *p = new_node();
            p->len = 1;
            if (bit) {
                p->bits[0] = 1;
                p->ones = 1;
            }
            return update(p);
        }

        int count(Tree t) const { return t ? t->cnt : 0; }
        int sum(Tree t) const { return t ? t->sum : 0; }

        Tree update(Tree t) {
            if (!t) return nullptr;
            t->cnt = count(t->l) + t->len + count(t->r);
            t->sum = sum(t->l) + t->ones + sum(t->r);
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

        Tree insert_rec(Tree t, int k, bool bit) {
            if (!t) return new_single(bit);
            int lc = count(t->l);
            if (k < lc) {
                t->l = insert_rec(t->l, k, bit);
                t = update(t);
                if (t->l && t->l->priority > t->priority) t = rotate_right(t);
                return t;
            }
            if (k > lc + t->len) {
                t->r = insert_rec(t->r, k - lc - t->len, bit);
                t = update(t);
                if (t->r && t->r->priority > t->priority) t = rotate_left(t);
                return t;
            }
            if (t->len < BLOCK_BITS) {
                insert_block_bit(t, k - lc, bit);
                return update(t);
            }
            vector<bool> v(t->len + 1);
            int pos = k - lc;
            for (int i = 0; i < pos; i++) v[i] = get_block_bit(t, i);
            v[pos] = bit;
            for (int i = pos; i < t->len; i++) v[i + 1] = get_block_bit(t, i);
            int mid = (int)v.size() >> 1;
            Tree l = t->l;
            Tree r = t->r;
            Tree a = new_block_from(v, 0, mid);
            Tree b = new_block_from(v, mid, (int)v.size());
            t->l = t->r = nullptr;
            release_node(t);
            Tree m = merge(a, b);
            return merge(merge(l, m), r);
        }

        pair<Tree, bool> erase_rec(Tree t, int k) {
            int lc = count(t->l);
            if (k < lc) {
                auto [nl, res] = erase_rec(t->l, k);
                t->l = nl;
                return {update(t), res};
            }
            if (k >= lc + t->len) {
                auto [nr, res] = erase_rec(t->r, k - lc - t->len);
                t->r = nr;
                return {update(t), res};
            }
            bool res = erase_block_bit(t, k - lc);
            if (t->len == 0) {
                Tree merged = merge(t->l, t->r);
                t->l = t->r = nullptr;
                release_node(t);
                return {merged, res};
            }
            return {update(t), res};
        }

      public:
        DynamicBitVector() = default;
        DynamicBitVector(const DynamicBitVector &) = delete;
        DynamicBitVector &operator=(const DynamicBitVector &) = delete;
        DynamicBitVector(DynamicBitVector &&other) noexcept
            : root(std::exchange(other.root, nullptr)), rng_state(other.rng_state) {}
        DynamicBitVector &operator=(DynamicBitVector &&other) noexcept {
            if (this != &other) {
                release_tree(root);
                root = std::exchange(other.root, nullptr);
                rng_state = other.rng_state;
            }
            return *this;
        }
        ~DynamicBitVector() { release_tree(root); }

        int size() const { return count(root); }
        int count1() const { return sum(root); }
        int count0() const { return size() - count1(); }

        bool get(int k) const {
            assert(0 <= k && k < size());
            return get_and_rank1(k).first;
        }

        // k番目のbitと [0, k) の1の個数を1回のtree探索で返す。
        pair<bool, int> get_and_rank1(int k) const {
            assert(0 <= k && k < size());
            int rank = 0;
            Tree t = root;
            while (true) {
                int lc = count(t->l);
                if (k < lc) {
                    t = t->l;
                } else if (k < lc + t->len) {
                    rank += sum(t->l) + block_rank1(t, k - lc);
                    return {get_block_bit(t, k - lc), rank};
                } else {
                    rank += sum(t->l) + t->ones;
                    k -= lc + t->len;
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
                } else if (k <= lc + t->len) {
                    res += sum(t->l);
                    res += block_rank1(t, k - lc);
                    break;
                } else {
                    res += sum(t->l);
                    res += t->ones;
                    k -= lc + t->len;
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
            root = insert_rec(root, k, bit);
        }

        optional<bool> erase(int k) {
            if (k < 0 || k >= size()) return nullopt;
            auto [nr, res] = erase_rec(root, k);
            root = nr;
            return res;
        }

        void build(const vector<bool> &v) {
            release_tree(root);
            root = nullptr;
            for (int l = 0; l < (int)v.size(); l += BLOCK_BITS) {
                int r = min<int>(l + BLOCK_BITS, v.size());
                root = merge(root, new_block_from(v, l, r));
            }
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
        n = (int)v.size();
        vector<U> cur(n), nxt(n);
        for (int i = 0; i < n; i++) cur[i] = encode(v[i]);
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            vector<bool> bits(n);
            int zero_count = 0;
            for (int i = 0; i < n; i++) {
                bits[i] = bit_at(cur[i], b);
                if (!bits[i]) zero_count++;
            }
            matrix[level].build(bits);
            zeros[level] = zero_count;
            int zi = 0, oi = zero_count;
            for (int i = 0; i < n; i++) {
                if (bits[i]) nxt[oi++] = cur[i];
                else nxt[zi++] = cur[i];
            }
            cur.swap(nxt);
        }
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T access(int k) const {
        assert(0 <= k && k < n);
        U x = 0;
        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            auto [bit, rank1] = matrix[level].get_and_rank1(k);
            if (bit) {
                x |= U(1) << b;
                k = zeros[level] + rank1;
            } else {
                k -= rank1;
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
            auto [bit, rank1] = matrix[level].get_and_rank1(k);
            int next_k;
            if (bit) {
                x |= U(1) << b;
                next_k = zeros[level] + rank1;
            } else {
                next_k = k - rank1;
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
//   prev_pos[i] = i より左にある同じ値の直前位置、なければ -1。
//   prev_wm には prev_pos[i] + 1 を載せる。
//   count(l, r) は [l, r) のうち prev_pos[i] < l である i の個数。
//
// 計算量:
//   set O(log A (log N + B) + log N (log N + B))
//   insert/erase O(N(log A (log N + B) + log N (log N + B))) 補助構造を再構築するため重い
//   count O(log N (log N + B))
//
// 注意:
//   - 挿入削除が多い巨大ケース向けではない。
//   - set と count が多い用途向け。
template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits, int POS_LOG = numeric_limits<unsigned int>::digits>
class DynamicRangeCountDistinct {
   private:
    DynamicWaveletMatrix<T, LOG> wm;
    DynamicWaveletMatrix<int, POS_LOG> prev_wm;
    unordered_map<T, std::set<int>> positions;

    void rebuild_auxiliary() {
        positions.clear();
        positions.reserve(wm.size() * 2 + 1);
        vector<int> prev_key(wm.size(), 0);
        unordered_map<T, int> last;
        last.reserve(wm.size() * 2 + 1);
        for (int i = 0; i < wm.size(); i++) {
            T x = wm.access(i);
            auto it = last.find(x);
            if (it != last.end()) prev_key[i] = it->second + 1;
            last[x] = i;
            positions[x].insert(i);
        }
        prev_wm.build(prev_key);
    }

    static int prev_it(const std::set<int> &s, std::set<int>::const_iterator it) {
        if (it == s.begin()) return -1;
        return *std::prev(it);
    }

    static int next_it(const std::set<int> &s, std::set<int>::const_iterator it) {
        ++it;
        return it == s.end() ? -1 : *it;
    }

   public:
    DynamicRangeCountDistinct() = default;
    explicit DynamicRangeCountDistinct(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        wm.build(v);
        rebuild_auxiliary();
    }

    int size() const { return wm.size(); }
    bool empty() const { return wm.empty(); }

    T access(int k) const { return wm.access(k); }

    void set(int k, const T &x) {
        assert(0 <= k && k < size());
        T old = wm.access(k);
        if (old == x) return;

        auto old_map_it = positions.find(old);
        assert(old_map_it != positions.end());
        auto old_it = old_map_it->second.find(k);
        assert(old_it != old_map_it->second.end());
        int old_prev = prev_it(old_map_it->second, old_it);
        int old_next = next_it(old_map_it->second, old_it);

        int new_prev = -1;
        int new_next = -1;
        auto new_map_it = positions.find(x);
        if (new_map_it != positions.end()) {
            auto lb = new_map_it->second.lower_bound(k);
            new_next = lb == new_map_it->second.end() ? -1 : *lb;
            new_prev = lb == new_map_it->second.begin() ? -1 : *std::prev(lb);
        }

        old_map_it->second.erase(old_it);
        if (old_map_it->second.empty()) positions.erase(old_map_it);
        positions[x].insert(k);

        wm.set(k, x);
        prev_wm.set(k, new_prev + 1);
        if (old_next != -1) prev_wm.set(old_next, old_prev + 1);
        if (new_next != -1) prev_wm.set(new_next, k + 1);
    }

    void insert(int k, const T &x) {
        assert(0 <= k && k <= size());
        wm.insert(k, x);
        rebuild_auxiliary();
    }

    optional<T> erase(int k) {
        if (k < 0 || k >= size()) return nullopt;
        optional<T> res = wm.erase(k);
        rebuild_auxiliary();
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
        return prev_wm.range_freq(l, r, l + 1);
    }

    vector<T> to_vector() const { return wm.to_vector(); }
};
