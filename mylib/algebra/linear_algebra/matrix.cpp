// 最終更新: 2026-07-12 14:27:44 JST
template <class T>
struct Matrix {
    int h, w;
    std::vector<std::vector<T>> a;

    Matrix() : h(0), w(0) {}
    Matrix(int h, int w, T val = T()) : h(h), w(w), a(h, std::vector<T>(w, val)) {}

    Matrix(std::initializer_list<std::initializer_list<T>> init) {
        h = init.size();
        w = (h == 0 ? 0 : (int)init.begin()->size());
        a.reserve(h);
        for (auto& row : init) {
            assert((int)row.size() == w);
            a.emplace_back(row);
        }
    }

    std::vector<T>& operator[](int i) { return a[i]; }
    const std::vector<T>& operator[](int i) const { return a[i]; }

    static Matrix identity(int n) {
        Matrix I(n, n);
        for (int i = 0; i < n; i++) I[i][i] = T(1);
        return I;
    }

    Matrix transpose() const {
        Matrix res(w, h);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[j][i] = a[i][j];
        return res;
    }

    /* ===== 行列同士の演算 ===== */

    Matrix operator+(const Matrix& other) const {
        assert(h == other.h && w == other.w);
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] + other[i][j];
        return res;
    }

    Matrix operator-(const Matrix& other) const {
        assert(h == other.h && w == other.w);
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] - other[i][j];
        return res;
    }

    Matrix operator*(const Matrix& other) const {
        assert(w == other.h);
        Matrix res(h, other.w, T());
        for (int i = 0; i < h; i++)
            for (int k = 0; k < w; k++)
                for (int j = 0; j < other.w; j++) res[i][j] += a[i][k] * other[k][j];
        return res;
    }

    std::vector<T> operator*(const std::vector<T>& v) const {
        assert(w == (int)v.size());
        std::vector<T> res(h, T());
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i] += a[i][j] * v[j];
        return res;
    }

    Matrix& operator+=(const Matrix& other) {
        assert(h == other.h && w == other.w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] += other[i][j];
        return *this;
    }

    Matrix& operator-=(const Matrix& other) {
        assert(h == other.h && w == other.w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] -= other[i][j];
        return *this;
    }

    Matrix& operator*=(const Matrix& other) {
        assert(w == other.h);
        *this = (*this) * other;
        return *this;
    }

    /* ===== スカラー演算 ===== */

    Matrix operator+(const T& x) const {
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] + x;
        return res;
    }

    Matrix operator-(const T& x) const {
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] - x;
        return res;
    }

    Matrix operator*(const T& x) const {
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] * x;
        return res;
    }

    Matrix operator/(const T& x) const {
        assert(x != T(0));
        Matrix res(h, w);
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) res[i][j] = a[i][j] / x;
        return res;
    }

    Matrix& operator+=(const T& x) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] += x;
        return *this;
    }

    Matrix& operator-=(const T& x) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] -= x;
        return *this;
    }

    Matrix& operator*=(const T& x) {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] *= x;
        return *this;
    }

    Matrix& operator/=(const T& x) {
        assert(x != T(0));
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) a[i][j] /= x;
        return *this;
    }

    /* ===== 累乗 ===== */

    Matrix pow(long long e) const {
        assert(h == w);
        Matrix res = identity(h);
        Matrix base = *this;
        while (e > 0) {
            if (e & 1) res *= base;
            base *= base;
            e >>= 1;
        }
        return res;
    }

    /* ===== 逆行列 ===== */

    Matrix inverse() const {
        assert(h == w);
        int n = h;

        Matrix<T> res = identity(n);
        Matrix<T> tmp = *this;

        for (int i = 0; i < n; i++) {
            // ピボット探索
            int pivot = i;
            for (int j = i; j < n; j++) {
                if (tmp[j][i] != T(0)) {
                    pivot = j;
                    break;
                }
            }
            assert(tmp[pivot][i] != T(0));  // 逆行列が存在しない

            // 行入れ替え
            if (i != pivot) {
                std::swap(tmp.a[i], tmp.a[pivot]);
                std::swap(res.a[i], res.a[pivot]);
            }

            // 正規化
            T inv = T(1) / tmp[i][i];
            for (int j = 0; j < n; j++) {
                tmp[i][j] *= inv;
                res[i][j] *= inv;
            }

            // 消去
            for (int r = 0; r < n; r++) {
                if (r == i) continue;
                T factor = tmp[r][i];
                for (int c = 0; c < n; c++) {
                    tmp[r][c] -= factor * tmp[i][c];
                    res[r][c] -= factor * res[i][c];
                }
            }
        }
        return res;
    }
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
    for (int i = 0; i < m.h; i++) {
        for (int j = 0; j < m.w; j++) os << m[i][j] << (j + 1 == m.w ? '\n' : ' ');
    }
    return os;
}

template <class T>
std::vector<T> operator*(const std::vector<T>& v, const Matrix<T>& m) {
    assert((int)v.size() == m.h);
    std::vector<T> res(m.w, T());
    for (int i = 0; i < m.h; i++)
        for (int j = 0; j < m.w; j++) res[j] += v[i] * m[i][j];
    return res;
}
