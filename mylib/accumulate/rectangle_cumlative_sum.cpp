template <class T> struct RectSum {
  public:
    explicit RectSum(int h, int w, T init = T()) {
        A = vector(H + 1, vector(W + 1, init));
        H = h, W = w;
        this->init = init;
    };

    void rect_add(int h1, int w1, int h2, int w2) {
        /* h1 <= h <= h2, w1 <= w <= w2 の長方形範囲にxを足す
        左下の点が(h1, w1), 右上の点が(h2, w2)の長方形範囲にxを足す
        */
        A[h1][w1] += x;
        A[h2 + 1][w1] -= x;
        A[h1][w2 + 1] -= x;
        A[h2 + 1][w2 + 1] += x;
    }

    auto build() {
        vector ret = vector(H, vector(W, init));
        for (int row : views::iota(0, H)) {
            for (int col : views::iota(0, W)) {
                ret[row][col] = A[row][col];
                if (row > 0) {
                    ret[row][col] += ret[row - 1][col];
                }
                if (col > 0) {
                    ret[row][col] += ret[row][col - 1];
                }
                if (row > 0 && col > 0) {
                    ret[row][col] -= ret[row - 1][col - 1];
                }
            }
        }
        return ret;
    }

  private:
    vector<vector<T>> A;
    vector<vector<T>> S;
    int H, W;
    T init;
};