class FastSet {
   public:
    FastSet() : size_(0), height_(0) {}

    explicit FastSet(int n) { build(n); }

    template <typename Predicate>
    FastSet(int n, Predicate shouldInclude) {
        build(n, shouldInclude);
    }

    bool contains(int i) const { return (layers_[0][i / BITS] >> (i % BITS)) & 1; }

    void insert(int i) {
        for (int h = 0; h < height_; h++) {
            layers_[h][i / BITS] |= uint64_t(1) << (i % BITS);
            i /= BITS;
        }
    }

    void erase(int i) {
        uint64_t parentBit = 0;
        for (int h = 0; h < height_; h++) {
            layers_[h][i / BITS] &= ~(uint64_t(1) << (i % BITS));
            layers_[h][i / BITS] |= parentBit << (i % BITS);
            parentBit = (layers_[h][i / BITS] != 0) ? 1 : 0;
            i /= BITS;
        }
    }

    // [start, n) の範囲で最小の要素を返す。見つからない場合は n
    int findNext(int start) {
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
    int findPrev(int end) {
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
        layers_.clear();
        size_ = n;

        int layerSize = n;
        do {
            int numBlocks = (layerSize + BITS - 1) / BITS;
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
            for (int i = 0; i < static_cast<int>(layers_[h].size()); i++) {
                if (layers_[h][i] != 0) {
                    layers_[h + 1][i / BITS] |= uint64_t(1) << (i % BITS);
                }
            }
        }
    }
};