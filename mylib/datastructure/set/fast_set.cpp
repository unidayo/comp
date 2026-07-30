// 最終更新: 2026-07-13 00:54:24 JST
// [0, n) のint型の整数集合を64分木で管理する。
// nに比例するbit列を確保するため、実際の上限は利用可能メモリに依存する。
//
// 計算量:
//   - FastSet(n): O(n / 64) 時間、O(n) bitメモリ
//   - FastSet(n, predicate): O(n) 時間、O(n) bitメリ
//   - contains(i): O(1)
//   - insert(i), erase(i): O(log_64 n)
//   - findNext(i), findPrev(i): O(log_64 n)
//
// 各階層は直下の64ブロックが非空かを1bitで持つ。
// insert / erase はブロックの空・非空が変化しなければ途中で伝播を打ち切る。
// intの値域では64分木の高さは最大6程度なので、更新・検索の実際の定数は小さい。
class FastSet {
   public:
    FastSet() : size_(0), height_(0) {}

    explicit FastSet(int n) { build(n); }

    template <typename Predicate>
    FastSet(int n, Predicate shouldInclude) {
        build(n, shouldInclude);
    }

    int size() const { return size_; }

    bool contains(int i) const { return (layers_[0][i / BITS] >> (i % BITS)) & 1; }

    void insert(int i) {
        for (int h = 0; h < height_; h++) {
            uint64_t &word = layers_[h][i / BITS];
            uint64_t before = word;
            word |= uint64_t(1) << (i % BITS);
            // このwordは更新前から非空なので、上位層のbitはすでに立っている。
            if (before != 0) break;
            i /= BITS;
        }
    }

    void erase(int i) {
        for (int h = 0; h < height_; h++) {
            uint64_t &word = layers_[h][i / BITS];
            word &= ~(uint64_t(1) << (i % BITS));
            // まだ同じwordに要素がある間は、上位層を変更する必要がない。
            if (word != 0) break;
            i /= BITS;
        }
    }

    // [start, n) の範囲で最小の要素を返す。見つからない場合は n
    int findNext(int start) const {
        if (start < 0) start = 0;
        if (start >= size_) return size_;

        int i = start;
        for (int h = 0; h < height_; h++) {
            if (i / BITS >= static_cast<int>(layers_[h].size())) {
                break;
            }

            uint64_t mask = layers_[h][i / BITS] >> (i % BITS);
            if (mask == 0) {
                i = i / BITS + 1;
                continue;
            }

            i += std::countr_zero(mask);

            for (int g = h - 1; g >= 0; g--) {
                i *= BITS;
                i += std::countr_zero(layers_[g][i / BITS]);
            }
            return i;
        }
        return size_;
    }

    // [0, end] の範囲で最大の要素を返す。見つからない場合は -1
    int findPrev(int end) const {
        if (end < 0) return -1;
        if (end >= size_) end = size_ - 1;

        int i = end;
        for (int h = 0; h < height_; h++) {
            if (i == -1) break;

            uint64_t mask = layers_[h][i / BITS] << (63 - i % BITS);
            if (mask == 0) {
                i = i / BITS - 1;
                continue;
            }

            i -= std::countl_zero(mask);

            for (int g = h - 1; g >= 0; g--) {
                i *= BITS;
                i += 63 - std::countl_zero(layers_[g][i / BITS]);
            }
            return i;
        }
        return -1;
    }

   private:
    static constexpr int BITS = 64;

    int size_;
    int height_;
    std::vector<std::vector<uint64_t>> layers_;

    void build(int n) {
        assert(n >= 0);
        layers_.clear();
        size_ = n;
        int layerSize = n;
        do {
            int numBlocks = layerSize / BITS + (layerSize % BITS != 0);
            layers_.push_back(std::vector<uint64_t>(numBlocks, 0));
            layerSize = numBlocks;
        } while (layerSize > 1);
        height_ = static_cast<int>(layers_.size());
    }

    template <typename Predicate>
    void build(int n, Predicate shouldInclude) {
        build(n);

        for (int i = 0; i < n; i++) {
            if (shouldInclude(i)) {
                layers_[0][i / BITS] |= uint64_t(1) << (i % BITS);
            }
        }

        for (int h = 0; h < height_ - 1; h++) {
            for (size_t i = 0; i < layers_[h].size(); i++) {
                if (layers_[h][i] != 0) {
                    layers_[h + 1][i / BITS] |= uint64_t(1) << (i % BITS);
                }
            }
        }
    }
};
