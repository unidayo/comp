// 最終更新: 2026-07-13 00:04:49 JST
// 長さが変わらない配列向けの、一点更新と区間頻度検索を扱うデータ構造。
//
// DynamicWaveletMatrix から insert / erase を外し、位置の平方分割で実装している。
// 各ブロックは元の値とソート済みの値を持つ。ポインタ木を使わないため、
// set と range_freq が中心の用途では DynamicWaveletMatrix より軽い。
//
// 使い方:
//   PointSetWaveletMatrix<int> wm(a);
//   wm.set(k, x);                      // a[k] = x
//   wm.access(k);                      // a[k]
//   wm.rank(x, r);                     // [0, r) にある x の個数
//   wm.range_freq(l, r, upper);        // upper 未満の個数
//   wm.range_freq(l, r, lower, upper); // [lower, upper) の個数
//
// 計算量 (B はブロック長):
//   build O(N log B), access O(1), set O(B)
//   rank / range_freq O(B + (N/B) log B)
//
// 注意:
//   - 配列長は build 後に変更できない。
//   - 第2テンプレート引数でブロック長を変更できる。例: PointSetWaveletMatrix<int, 1024>

template <class T, int BLOCK_SIZE = 2048>
class PointSetWaveletMatrix {
    static_assert(is_integral_v<T>);
    static_assert(BLOCK_SIZE > 0);

    vector<T> values;
    vector<vector<T>> sorted_blocks;

    void rebuild_block(int b) {
        int l = b * BLOCK_SIZE;
        int r = min<int>(l + BLOCK_SIZE, values.size());
        auto &s = sorted_blocks[b];
        s.resize(r - l);
        for (int i = l; i < r; ++i) s[i - l] = values[i];
        sort(s.begin(), s.end());
    }

    int count_less(int l, int r, T upper) const {
        int res = 0;
        while (l < r && l % BLOCK_SIZE != 0) res += values[l++] < upper;
        while (l + BLOCK_SIZE <= r) {
            const auto &s = sorted_blocks[l / BLOCK_SIZE];
            res += lower_bound(s.begin(), s.end(), upper) - s.begin();
            l += BLOCK_SIZE;
        }
        while (l < r) res += values[l++] < upper;
        return res;
    }

    int count_range(int l, int r, T lower, T upper) const {
        int res = 0;
        while (l < r && l % BLOCK_SIZE != 0) {
            res += lower <= values[l] && values[l] < upper;
            ++l;
        }
        while (l + BLOCK_SIZE <= r) {
            const auto &s = sorted_blocks[l / BLOCK_SIZE];
            res += lower_bound(s.begin(), s.end(), upper) - lower_bound(s.begin(), s.end(), lower);
            l += BLOCK_SIZE;
        }
        while (l < r) {
            res += lower <= values[l] && values[l] < upper;
            ++l;
        }
        return res;
    }

  public:
    PointSetWaveletMatrix() = default;
    explicit PointSetWaveletMatrix(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        values = v;
        sorted_blocks.resize((values.size() + BLOCK_SIZE - 1) / BLOCK_SIZE);
        for (int b = 0; b < (int)sorted_blocks.size(); ++b) rebuild_block(b);
    }

    int size() const { return values.size(); }

    T access(int k) const {
        assert(0 <= k && k < size());
        return values[k];
    }

    void set(int k, T x) {
        assert(0 <= k && k < size());
        if (values[k] == x) return;
        T old_key = values[k];
        T new_key = x;
        auto &s = sorted_blocks[k / BLOCK_SIZE];
        auto old_it = lower_bound(s.begin(), s.end(), old_key);
        assert(old_it != s.end() && *old_it == old_key);
        s.erase(old_it);
        s.insert(lower_bound(s.begin(), s.end(), new_key), new_key);
        values[k] = x;
    }

    int rank(T x, int r) const { return rank(x, 0, r); }

    int rank(T x, int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        int res = 0;
        while (l < r && l % BLOCK_SIZE != 0) res += values[l++] == x;
        while (l + BLOCK_SIZE <= r) {
            const auto &s = sorted_blocks[l / BLOCK_SIZE];
            auto [first, last] = equal_range(s.begin(), s.end(), x);
            res += last - first;
            l += BLOCK_SIZE;
        }
        while (l < r) res += values[l++] == x;
        return res;
    }

    int range_freq(int l, int r, T upper) const {
        assert(0 <= l && l <= r && r <= size());
        return count_less(l, r, upper);
    }

    int range_freq(int l, int r, T lower, T upper) const {
        assert(0 <= l && l <= r && r <= size());
        if (!(lower < upper)) return 0;
        return count_range(l, r, lower, upper);
    }

    const vector<T> &to_vector() const { return values; }
};
