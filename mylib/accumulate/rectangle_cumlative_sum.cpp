template <class T>
struct RectSum {
    vector<vector<T>> A;
    int H, W;
    bool flag;
    explicit RectSum(int h, int w, T init = T()) {
        H = h, W = w;
        A = vector(h + 1, vector(w + 1, init));
        flag = false;
    };

    void rect_add(int h1, int w1, int h2, int w2, T x) {
        /* h1 <= h <= h2, w1 <= w <= w2 の長方形範囲にxを足す */
        assert(!flag);
        A[h1][w1] += x;
        A[h2 + 1][w1] -= x;
        A[h1][w2 + 1] -= x;
        A[h2 + 1][w2 + 1] += x;
    }

    void build() {
        assert(!flag);
        for (int row = 0; row < H; ++row) {
            for (int col = 0; col < W; ++col) {
                A[row][col + 1] += A[row][col];
            }
        }
        for (int col = 0; col < W; ++col) {
            for (int row = 0; row < H; ++row) {
                A[row + 1][col] += A[row][col];
            }
        }
        flag = true;
    }

    T get(int row, int col) {
        assert(flag);
        return A[row][col];
    }
};