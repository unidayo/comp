// 最終更新: 2026-07-12 14:27:44 JST
const int bs_size = 300;
int gaussJordan(vector<bitset<bs_size>> &A) {
    int N = ssize(A);
    int M = bs_size;
    int rank = 0;
    for (int col = 0; col < M; ++col) {
        int pivot = -1;
        for (int row = rank; row < N; ++row) {
            if (A[row][col] == 1) {
                pivot = row;
                break;
            }
        }
        if (pivot == -1) continue;
        swap(A[rank], A[pivot]);
        for (int row = rank + 1; row < N; ++row) {
            if (A[row][col] == 1) {
                A[row] ^= A[rank];
            }
        }
        ++rank;
    }
    return rank;
}
