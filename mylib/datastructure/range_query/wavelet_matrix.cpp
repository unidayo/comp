// 最終更新: 2026-07-13 00:21:35 JST
// Wavelet Matrix。
//
// 使い方:
//   WaveletMatrix<long long> wm(a);  // 負の値を含む signed 整数にも対応
//
//   wm.access(k);                 // a[k]
//   wm.rank(r, x);                // [0, r) にある x の個数
//   wm.rank(l, r, x);             // [l, r) にある x の個数
//   wm.kth_smallest(l, r, k);     // [l, r) の k 番目に小さい値（0-indexed）
//   wm.kth_largest(l, r, k);      // [l, r) の k 番目に大きい値（0-indexed）
//   wm.range_freq(l, r, upper);   // [l, r) にある upper 未満の値の個数
//   wm.range_freq(l, r, lower, upper);  // [l, r) にある [lower, upper) の値の個数
//   wm.prev_value(l, r, upper);   // [l, r) にある upper 未満の最大値。なければ nullopt
//   wm.next_value(l, r, lower);   // [l, r) にある lower 以上の最小値。なければ nullopt
//
// Static Range Count-Distinct も使う場合:
//   RangeCountDistinct<long long> wm(a);
//   wm.count_distinct(l, r);      // [l, r) にある相異なる値の個数
//   wm.kth_smallest(l, r, k);     // 通常の Wavelet Matrix 機能も使える
//
// 注意:
//   - T は整数型を想定。
//   - デフォルトの LOG では signed の負の値にも対応し、大小順も正しく扱う。
//     内部では符号ビットを反転して unsigned として並べる。
//   - signed 型で LOG を小さく指定する場合は、値が非負で 2^LOG 未満に収まる用途を想定。
//   - 構築 O(N log A)、`access` は O(1)、それ以外のクエリは O(log A)。

#if defined(__has_include)
#if __has_include(<immintrin.h>)
#include <immintrin.h>
#endif
#endif

template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits>
class WaveletMatrix {
    static_assert(is_integral_v<T>, "WaveletMatrix<T>: T must be an integral type");

   private:
    using U = make_unsigned_t<T>;
    static constexpr int FULL_LOG = numeric_limits<U>::digits;

    struct BitVector {
        int n = 0;
        int zeros = 0;
        vector<unsigned long long> bit;
        vector<int> sum;

        BitVector() = default;
        explicit BitVector(int n_) : n(n_), bit((n_ + 63) >> 6, 0) {}

        void set(int k) { bit[k >> 6] |= 1ULL << (k & 63); }
        void build() {
            sum.assign(bit.size() + 1, 0);
            for (int i = 0; i < (int)bit.size(); i++) {
                sum[i + 1] = sum[i] + popcount64(bit[i]);
            }
            zeros = n - sum.back();
        }
        bool get(int k) const { return (bit[k >> 6] >> (k & 63)) & 1ULL; }
        static int popcount64(unsigned long long x) {
#if defined(__POPCNT__)
            return (int)_mm_popcnt_u64(x);
#else
            return __builtin_popcountll(x);
#endif
        }
        int rank1(int k) const {
            int block = k >> 6;
            int offset = k & 63;
            int res = sum[block];
            if (offset) res += popcount64(bit[block] & ((1ULL << offset) - 1));
            return res;
        }
        int rank0(int k) const { return k - rank1(k); }
        pair<int, int> rank1_pair(int l, int r) const {
            int lb = l >> 6, lo = l & 63;
            int rb = r >> 6, ro = r & 63;
            int left = sum[lb];
            int right = sum[rb];
            if (lo) left += popcount64(bit[lb] & ((1ULL << lo) - 1));
            if (ro) right += popcount64(bit[rb] & ((1ULL << ro) - 1));
            return {left, right};
        }
        pair<int, int> rank0_pair(int l, int r) const {
            auto [l1, r1] = rank1_pair(l, r);
            return {l - l1, r - r1};
        }
    };

    int n = 0;
    vector<T> data;
    array<BitVector, LOG> matrix;

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

    int range_freq_less_encoded_from(int level, int l, int r, U upper) const {
        int res = 0;
        for (; level < LOG; level++) {
            int b = LOG - 1 - level;
            auto [l0, r0] = matrix[level].rank0_pair(l, r);
            if (bit_at(upper, b)) {
                res += r0 - l0;
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }
        return res;
    }

    int range_freq_less_encoded(int l, int r, U upper) const {
        return range_freq_less_encoded_from(0, l, r, upper);
    }

    int range_freq_encoded(int l, int r, U lower, U upper) const {
        U different = lower ^ upper;
        int different_bit = bit_width(different) - 1;
        int different_level = LOG - 1 - different_bit;

        for (int level = 0; level < different_level; ++level) {
            int b = LOG - 1 - level;
            auto [l0, r0] = matrix[level].rank0_pair(l, r);
            if (bit_at(lower, b)) {
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
            } else {
                l = l0;
                r = r0;
            }
        }

        auto [l0, r0] = matrix[different_level].rank0_pair(l, r);
        int zeros = r0 - l0;
        int lower_count = range_freq_less_encoded_from(different_level + 1, l0, r0, lower);
        int upper_l = matrix[different_level].zeros + l - l0;
        int upper_r = matrix[different_level].zeros + r - r0;
        int upper_count = range_freq_less_encoded_from(different_level + 1, upper_l, upper_r, upper);
        return zeros + upper_count - lower_count;
    }

   public:
    WaveletMatrix() = default;
    explicit WaveletMatrix(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        n = (int)v.size();
        data = v;
        vector<U> cur(n), nxt(n);
        for (int i = 0; i < n; i++) cur[i] = encode(v[i]);

        for (int level = 0; level < LOG; level++) {
            int b = LOG - 1 - level;
            matrix[level] = BitVector(n);

            int zero_count = 0;
            for (int i = 0; i < n; i++) {
                if (bit_at(cur[i], b)) matrix[level].set(i);
                else zero_count++;
            }
            matrix[level].build();

            int zi = 0, oi = zero_count;
            for (int i = 0; i < n; i++) {
                if (bit_at(cur[i], b)) nxt[oi++] = cur[i];
                else nxt[zi++] = cur[i];
            }
            cur.swap(nxt);
        }
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T access(int k) const {
        assert(0 <= k && k < n);
        return data[k];
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
            auto [l0, r0] = matrix[level].rank0_pair(l, r);
            if (bit_at(y, b)) {
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
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
            auto [l0, r0] = matrix[level].rank0_pair(l, r);
            int zero_count = r0 - l0;
            if (k < zero_count) {
                l = l0;
                r = r0;
            } else {
                x |= U(1) << b;
                k -= zero_count;
                l = matrix[level].zeros + l - l0;
                r = matrix[level].zeros + r - r0;
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
        return range_freq_encoded(l, r, encode(lower), encode(upper));
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
};

// 静的配列の区間 distinct 数 + 通常 Wavelet Matrix 機能。
//
// `WaveletMatrix<T>` と next position 用の `WaveletMatrix<int>` を別々に持つ。
// count_distinct が不要な場合は通常の `WaveletMatrix<T>` を使う。
template <class T, int LOG = numeric_limits<make_unsigned_t<T>>::digits>
class RangeCountDistinct {
   private:
    int n = 0;
    WaveletMatrix<T, LOG> wm;
    WaveletMatrix<int> next_pos;

   public:
    RangeCountDistinct() = default;
    explicit RangeCountDistinct(const vector<T> &v) { build(v); }

    void build(const vector<T> &v) {
        n = (int)v.size();
        wm.build(v);

        vector<int> nxt(n);
        unordered_map<T, int> last;
        last.reserve(n * 2 + 1);
        for (int i = n - 1; i >= 0; i--) {
            auto it = last.find(v[i]);
            nxt[i] = (it == last.end() ? n : it->second);
            last[v[i]] = i;
        }
        next_pos.build(nxt);
    }

    int size() const { return n; }
    bool empty() const { return n == 0; }

    T access(int k) const { return wm.access(k); }
    int rank(int r, T x) const { return wm.rank(r, x); }
    int rank(int l, int r, T x) const { return wm.rank(l, r, x); }
    T kth_smallest(int l, int r, int k) const { return wm.kth_smallest(l, r, k); }
    T kth_largest(int l, int r, int k) const { return wm.kth_largest(l, r, k); }
    int range_freq(int l, int r, T upper) const { return wm.range_freq(l, r, upper); }
    int range_freq(int l, int r, T lower, T upper) const { return wm.range_freq(l, r, lower, upper); }
    optional<T> prev_value(int l, int r, T upper) const { return wm.prev_value(l, r, upper); }
    optional<T> next_value(int l, int r, T lower) const { return wm.next_value(l, r, lower); }

    int count_distinct(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        return r - l - next_pos.range_freq(l, r, r);
    }
};
