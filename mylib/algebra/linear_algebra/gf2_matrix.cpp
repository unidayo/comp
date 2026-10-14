// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algebra/linear_algebra/gf2_matrix.cpp
/*
使い方:

    // MAX_W は列数の上限。転置では行数、逆行列では 2 * n、
    // solve では m + 1 も bitset に入る必要がある。
    GF2Matrix<512> A(n, m);

    A.set(i, j);          // A[i][j] = 1
    A.set(i, j, false);   // A[i][j] = 0
    A.flip(i, j);         // A[i][j] ^= 1
    bool x = A.get(i, j);

    auto C = A + B;       // GF(2) なので加減算は XOR
    auto D = A * B;
    auto At = A.transpose();
    int r = A.rank();

    // 正方行列
    auto I = GF2Matrix<512>::identity(n);
    auto P = A.pow(k);
    bool nonzero_det = A.det();

    // 逆行列。存在しない可能性があるなら inverse_opt() を使う。
    auto inv_opt = A.inverse_opt();
    if (inv_opt) {
        auto inv = *inv_opt;
    }

    // 連立方程式 Ax = b を解く。返るのは一つの解。
    vector<int> b(n);
    auto sol = A.solve(b);
    if (sol) {
        vector<int> x = *sol;
    } else {
        // 解なし
    }

注意:
    std::bitset はサイズがコンパイル時固定なので、GF2Matrix<MAX_W> の
    MAX_W は必要な拡大係数行列の列数以上にする。
    逆行列: 2 * n <= MAX_W
    solve : m + 1 <= MAX_W
    転置 : n <= MAX_W
*/

template <size_t MAX_W>
struct GF2Matrix {
    int h, w;
    std::vector<std::bitset<MAX_W>> a;

    GF2Matrix() : h(0), w(0) {}

    GF2Matrix(int h, int w) : h(h), w(w), a(h) {
        assert(0 <= h && 0 <= w && w <= (int)MAX_W);
    }

    GF2Matrix(std::initializer_list<std::initializer_list<int>> init) {
        h = init.size();
        w = (h == 0 ? 0 : (int)init.begin()->size());
        assert(w <= (int)MAX_W);
        a.assign(h, std::bitset<MAX_W>());
        int i = 0;
        for (const auto& row : init) {
            assert((int)row.size() == w);
            int j = 0;
            for (int x : row) {
                if (x & 1) a[i].set(j);
                ++j;
            }
            ++i;
        }
    }

    std::bitset<MAX_W>& operator[](int i) { return a[i]; }
    const std::bitset<MAX_W>& operator[](int i) const { return a[i]; }

    bool get(int i, int j) const {
        assert(0 <= i && i < h && 0 <= j && j < w);
        return a[i][j];
    }

    void set(int i, int j, bool x = true) {
        assert(0 <= i && i < h && 0 <= j && j < w);
        a[i].set(j, x);
    }

    void flip(int i, int j) {
        assert(0 <= i && i < h && 0 <= j && j < w);
        a[i].flip(j);
    }

    static GF2Matrix identity(int n) {
        assert(n <= (int)MAX_W);
        GF2Matrix res(n, n);
        for (int i = 0; i < n; i++) res[i].set(i);
        return res;
    }

    GF2Matrix transpose() const {
        assert(h <= (int)MAX_W);
        GF2Matrix res(w, h);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (a[i][j]) res[j].set(i);
            }
        }
        return res;
    }

    GF2Matrix operator+(const GF2Matrix& other) const {
        assert(h == other.h && w == other.w);
        GF2Matrix res = *this;
        res += other;
        return res;
    }

    GF2Matrix operator-(const GF2Matrix& other) const {
        return *this + other;
    }

    GF2Matrix& operator+=(const GF2Matrix& other) {
        assert(h == other.h && w == other.w);
        for (int i = 0; i < h; i++) a[i] ^= other[i];
        return *this;
    }

    GF2Matrix& operator-=(const GF2Matrix& other) {
        return *this += other;
    }

    GF2Matrix operator*(const GF2Matrix& other) const {
        assert(w == other.h);
        GF2Matrix res(h, other.w);
        for (int i = 0; i < h; i++) {
            for (int k = 0; k < w; k++) {
                if (a[i][k]) res[i] ^= other[k];
            }
        }
        return res;
    }

    GF2Matrix& operator*=(const GF2Matrix& other) {
        *this = *this * other;
        return *this;
    }

    std::vector<int> operator*(const std::vector<int>& v) const {
        assert(w == (int)v.size());
        std::bitset<MAX_W> bv;
        for (int i = 0; i < w; i++) {
            if (v[i] & 1) bv.set(i);
        }
        std::vector<int> res(h);
        for (int i = 0; i < h; i++) res[i] = ((a[i] & bv).count() & 1);
        return res;
    }

    std::bitset<MAX_W> multiply_vector(const std::bitset<MAX_W>& v) const {
        assert(h <= (int)MAX_W);
        std::bitset<MAX_W> res;
        for (int i = 0; i < h; i++) {
            if ((a[i] & v).count() & 1) res.set(i);
        }
        return res;
    }

    GF2Matrix pow(long long e) const {
        assert(h == w);
        GF2Matrix res = identity(h);
        GF2Matrix base = *this;
        while (e > 0) {
            if (e & 1) res *= base;
            base *= base;
            e >>= 1;
        }
        return res;
    }

    int gauss_jordan(bool reduced = true) {
        int rank = 0;
        for (int col = 0; col < w && rank < h; col++) {
            int pivot = -1;
            for (int row = rank; row < h; row++) {
                if (a[row][col]) {
                    pivot = row;
                    break;
                }
            }
            if (pivot == -1) continue;
            std::swap(a[rank], a[pivot]);
            int first = (reduced ? 0 : rank + 1);
            for (int row = first; row < h; row++) {
                if (row != rank && a[row][col]) a[row] ^= a[rank];
            }
            rank++;
        }
        return rank;
    }

    int rank() const {
        GF2Matrix tmp = *this;
        return tmp.gauss_jordan(false);
    }

    bool det() const {
        assert(h == w);
        return rank() == h;
    }

    std::optional<GF2Matrix> inverse_opt() const {
        assert(h == w);
        assert(2 * w <= (int)MAX_W);

        GF2Matrix aug(h, 2 * w);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (a[i][j]) aug[i].set(j);
            }
            aug[i].set(w + i);
        }

        int rank = 0;
        for (int col = 0; col < w; col++) {
            int pivot = -1;
            for (int row = rank; row < h; row++) {
                if (aug[row][col]) {
                    pivot = row;
                    break;
                }
            }
            if (pivot == -1) return std::nullopt;
            std::swap(aug[rank], aug[pivot]);
            for (int row = 0; row < h; row++) {
                if (row != rank && aug[row][col]) aug[row] ^= aug[rank];
            }
            rank++;
        }

        GF2Matrix inv(h, w);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (aug[i][w + j]) inv[i].set(j);
            }
        }
        return inv;
    }

    GF2Matrix inverse() const {
        auto res = inverse_opt();
        assert(res.has_value());
        return *res;
    }

    std::optional<std::vector<int>> solve(const std::vector<int>& b) const {
        assert(h == (int)b.size());
        assert(w + 1 <= (int)MAX_W);

        GF2Matrix aug(h, w + 1);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (a[i][j]) aug[i].set(j);
            }
            if (b[i] & 1) aug[i].set(w);
        }

        std::vector<int> pivot_col;
        int rank = 0;
        for (int col = 0; col < w && rank < h; col++) {
            int pivot = -1;
            for (int row = rank; row < h; row++) {
                if (aug[row][col]) {
                    pivot = row;
                    break;
                }
            }
            if (pivot == -1) continue;
            std::swap(aug[rank], aug[pivot]);
            for (int row = 0; row < h; row++) {
                if (row != rank && aug[row][col]) aug[row] ^= aug[rank];
            }
            pivot_col.push_back(col);
            rank++;
        }

        for (int row = rank; row < h; row++) {
            if (aug[row][w]) return std::nullopt;
        }

        std::vector<int> x(w);
        for (int i = 0; i < rank; i++) x[pivot_col[i]] = aug[i][w];
        return x;
    }
};

template <size_t MAX_W>
std::ostream& operator<<(std::ostream& os, const GF2Matrix<MAX_W>& m) {
    for (int i = 0; i < m.h; i++) {
        for (int j = 0; j < m.w; j++) os << m[i][j] << (j + 1 == m.w ? '\n' : ' ');
    }
    return os;
}
