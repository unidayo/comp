/*
使い方:

    // 先に mylib/algebra/linear_algebra/matrix.cpp を貼る。
    Matrix<mint> A(n, m);
    vector<mint> b(n);

    auto res = linear_equation(A, b);  // Ax = b を解く
    if (!res.has_solution) {
        // 解なし
    } else {
        vector<mint> x = res.solution;  // 一つの解
        if (res.unique()) {
            // 一意解
        } else {
            // 一般解は res.solution + res.basis の線形結合
            // res.basis は Ax = 0 の基底
        }
    }

    // double など誤差判定が必要な型では is_zero を渡す。
    auto res2 = linear_equation(A, b, [](double x) {
        return abs(x) < 1e-9;
    });

戻り値:
    has_solution : 解が存在するか
    rank         : 係数行列 A の rank
    solution     : 一つの解。自由変数は 0 にしたもの
    basis        : 斉次方程式 Ax = 0 の基底
    pivot_col    : pivot に使われた列
*/

template <class T>
struct LinearEquationResult {
    bool has_solution;
    int rank;
    std::vector<T> solution;
    std::vector<std::vector<T>> basis;
    std::vector<int> pivot_col;

    bool unique() const {
        return has_solution && basis.empty();
    }
};

template <class T, class IsZero>
LinearEquationResult<T> linear_equation(Matrix<T> A, const std::vector<T>& b, IsZero is_zero) {
    assert(A.h == (int)b.size());

    int n = A.h;
    int m = A.w;
    Matrix<T> aug(n, m + 1);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) aug[i][j] = A[i][j];
        aug[i][m] = b[i];
    }

    std::vector<int> pivot_col;
    std::vector<int> where(m, -1);
    int rank = 0;

    for (int col = 0; col < m && rank < n; col++) {
        int pivot = -1;
        for (int row = rank; row < n; row++) {
            if (!is_zero(aug[row][col])) {
                pivot = row;
                break;
            }
        }
        if (pivot == -1) continue;

        std::swap(aug[rank], aug[pivot]);

        T inv = T(1) / aug[rank][col];
        for (int j = col; j <= m; j++) aug[rank][j] *= inv;

        for (int row = 0; row < n; row++) {
            if (row == rank || is_zero(aug[row][col])) continue;
            T factor = aug[row][col];
            for (int j = col; j <= m; j++) aug[row][j] -= factor * aug[rank][j];
        }

        where[col] = rank;
        pivot_col.push_back(col);
        rank++;
    }

    for (int row = rank; row < n; row++) {
        if (!is_zero(aug[row][m])) {
            return {false, rank, {}, {}, pivot_col};
        }
    }

    std::vector<T> solution(m, T(0));
    for (int col = 0; col < m; col++) {
        if (where[col] != -1) solution[col] = aug[where[col]][m];
    }

    std::vector<std::vector<T>> basis;
    for (int free_col = 0; free_col < m; free_col++) {
        if (where[free_col] != -1) continue;

        std::vector<T> v(m, T(0));
        v[free_col] = T(1);
        for (int col : pivot_col) {
            v[col] = -aug[where[col]][free_col];
        }
        basis.push_back(v);
    }

    return {true, rank, solution, basis, pivot_col};
}

template <class T>
LinearEquationResult<T> linear_equation(Matrix<T> A, const std::vector<T>& b) {
    return linear_equation(A, b, [](const T& x) { return x == T(0); });
}
