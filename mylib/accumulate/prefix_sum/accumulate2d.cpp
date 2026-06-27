template <class T>
struct accumulate2d {
    /*0-based 2次元累積和*/
    vector<vector<T>> S;
    int H, W;
    bool flag;

    explicit accumulate2d(int h, int w) {
        H = h, W = w;
        S = vector<vector<T>>(H + 1, vector<T>(W + 1));
        flag = false;
    }

    void add(int h, int w, T x) {
        assert(!flag);
        S[h][w] += x;
    }

    void build() {
        assert(!flag);
        for (int row = 0; row < H; ++row) {
            for (int col = 0; col < W; ++col) {
                S[row][col + 1] += S[row][col];
            }
        }
        for (int col = 0; col < W; ++col) {
            for (int row = 0; row < H; ++row) {
                S[row + 1][col] += S[row][col];
            }
        }
        flag = true;
    }

    T sum(int h1, int w1, int h2, int w2) {
        /*h1<=h<=h2, w1<=w<=w2の長方形範囲の総和を求める*/
        assert(flag);
        T ret = S[h2][w2];
        if (h1 > 0) ret -= S[h1 - 1][w2];
        if (w1 > 0) ret -= S[h2][w1 - 1];
        if (h1 > 0 && w1 > 0) ret += S[h1 - 1][w1 - 1];
        return ret;
    }
};