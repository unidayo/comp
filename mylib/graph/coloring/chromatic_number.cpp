// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/graph/coloring/chromatic_number.cpp
// 無向単純グラフの彩色数を独立集合の数え上げと包除原理で厳密に求める。
//
// chromatic_number(N, edges):
//   頂点数 N と、無向辺 {u, v} の vector<pair<int, int>> を渡す（推奨）。
//   多重辺は許すが、自己ループは許さない。
//
// chromatic_number(graph):
//   graph[v] に v の隣接頂点を入れた隣接リストを渡す。
//   片方向だけに辺が入っていても無向辺として扱い、多重辺も許す。
//   自己ループは許さない。
//
// chromatic_number(neighbor):
//   neighbor[v] の bit u が辺 {u, v} の存在を表す対称な隣接 bit mask を渡す。
//
// 空グラフ（頂点数 0）の彩色数は 0 とする。
// 時間計算量: O(N 2^N) 回の多倍長整数演算
// 空間計算量: O(2^N)
// 剰余を使わないため衝突による誤答はない。実用上は N <= 22 程度が目安。
// 内部の多倍長整数には、N < 63 で必要十分な 4096 bit を確保している。

namespace chromatic_number_internal {

struct BigUInt {
    static constexpr int capacity = 64;
    std::array<std::uint64_t, capacity> digit;
    int size;

    BigUInt(std::uint64_t x = 0) : size(x != 0) {
        if (x != 0) digit[0] = x;
    }

    void multiply(std::uint64_t x) {
        if (x == 0 || size == 0) {
            size = 0;
            return;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < size; ++i) {
            unsigned __int128 value = (unsigned __int128)digit[i] * x + carry;
            digit[i] = std::uint64_t(value);
            carry = value >> 64;
        }
        if (carry != 0) {
            assert(size < capacity);
            digit[size++] = std::uint64_t(carry);
        }
    }

    BigUInt &operator+=(const BigUInt &other) {
        const int new_size = std::max(size, other.size);
        unsigned __int128 carry = 0;
        for (int i = 0; i < new_size; ++i) {
            unsigned __int128 value = carry;
            if (i < size) value += digit[i];
            if (i < other.size) value += other.digit[i];
            digit[i] = std::uint64_t(value);
            carry = value >> 64;
        }
        size = new_size;
        if (carry != 0) {
            assert(size < capacity);
            digit[size++] = std::uint64_t(carry);
        }
        return *this;
    }

    friend bool operator==(const BigUInt &a, const BigUInt &b) {
        if (a.size != b.size) return false;
        for (int i = 0; i < a.size; ++i) {
            if (a.digit[i] != b.digit[i]) return false;
        }
        return true;
    }
};

}  // namespace chromatic_number_internal

int chromatic_number(const std::vector<std::uint64_t> &neighbor) {
    using chromatic_number_internal::BigUInt;

    const int n = int(neighbor.size());
    if (n == 0) return 0;
    if (n >= 63) throw std::length_error("chromatic_number requires N < 63");

    const std::uint64_t full_mask = (std::uint64_t{1} << n) - 1;
    for (int v = 0; v < n; ++v) {
        assert((neighbor[v] & ~full_mask) == 0);
        assert(((neighbor[v] >> v) & 1) == 0);
        for (int u = 0; u < n; ++u) {
            assert(((neighbor[v] >> u) & 1) == ((neighbor[u] >> v) & 1));
        }
    }

    const std::uint64_t subset_count = std::uint64_t{1} << n;

    // independent_count[S] = G[S] に含まれる独立集合（空集合を含む）の個数。
    // v を含まないものと、v を含むものに分ける。
    std::vector<std::uint64_t> independent_count(subset_count);
    independent_count[0] = 1;
    for (std::uint64_t subset = 1; subset < subset_count; ++subset) {
        const int v = std::countr_zero(subset);
        const std::uint64_t without_v = subset ^ (std::uint64_t{1} << v);
        independent_count[subset] =
            independent_count[without_v] +
            independent_count[without_v & ~neighbor[v]];
    }

    // G が k 色で彩色可能であることは、各色で使う独立集合を選んだとき
    // 全頂点が少なくとも一度選ばれる方法が存在することと同値。
    // 包除原理より、その方法の個数は
    //   sum_{S subseteq V} (-1)^(N-|S|) independent_count[S]^k
    // となる。
    std::vector<BigUInt> positive(n + 1), negative(n + 1);
    for (std::uint64_t subset = 0; subset < subset_count; ++subset) {
        BigUInt power = 1;
        auto &sum = ((n - std::popcount(subset)) & 1) ? negative : positive;
        for (int colors = 1; colors <= n; ++colors) {
            power.multiply(independent_count[subset]);
            sum[colors] += power;
        }
    }
    for (int colors = 1; colors <= n; ++colors) {
        if (!(positive[colors] == negative[colors])) return colors;
    }

    // 自己ループのない N 頂点グラフは必ず N 色で彩色できる。
    assert(false);
    return n;
}

int chromatic_number(
    int vertex_count, const std::vector<std::pair<int, int>> &edges) {
    assert(vertex_count >= 0);
    if (vertex_count >= 63) {
        throw std::length_error("chromatic_number requires N < 63");
    }

    std::vector<std::uint64_t> neighbor(vertex_count);
    for (auto [u, v] : edges) {
        assert(0 <= u && u < vertex_count);
        assert(0 <= v && v < vertex_count);
        assert(u != v);
        neighbor[u] |= std::uint64_t{1} << v;
        neighbor[v] |= std::uint64_t{1} << u;
    }
    return chromatic_number(neighbor);
}

int chromatic_number(const std::vector<std::vector<int>> &graph) {
    const int n = int(graph.size());
    if (n >= 63) throw std::length_error("chromatic_number requires N < 63");

    std::vector<std::uint64_t> neighbor(n);
    for (int v = 0; v < n; ++v) {
        for (int u : graph[v]) {
            assert(0 <= u && u < n && u != v);
            neighbor[v] |= std::uint64_t{1} << u;
            neighbor[u] |= std::uint64_t{1} << v;
        }
    }
    return chromatic_number(neighbor);
}
