template <class T> struct accumulate2d {
    /*0-based 2次元累積和*/
  public:
    explicit accumulate2d(const vector<vector<T>> &A) {
        int H = A.size(), W = A[0].size();
        S = vector<vector<T>>(H + 1, vector<T>(W + 1));
        for (int row : views::iota(0, H)) {
            for (int col : views::iota(0, W)) {
                S[row + 1][col + 1] = A[row][col] + S[row][col + 1] + S[row + 1][col] - S[row][col];
            }
        }
    }

    T operator()(int h1, int w1, int h2, int w2) {
        /*h1<=h<=h2, w1<=w<=w2の長方形範囲の総和を求める*/
        return S[h2 + 1][w2 + 1] - S[h2 + 1][w1] - S[h1][w2 + 1] + S[h1][w1];
    }

  private:
    vector<vector<T>> S;
};