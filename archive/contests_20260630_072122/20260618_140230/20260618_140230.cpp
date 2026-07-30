#include <bits/stdc++.h>
using namespace std;

const int MAX = 205;
using BS = bitset<MAX>;
using Mat = vector<BS>;

int smod(int x, int m) {
    x %= m;
    if (x < 0) x += m;
    return x;
}

// A: n x k
// B: k x m
// return C: n x m
Mat mul(const Mat& A, const Mat& B, int n, int k, int m) {
    Mat C(n);

    for (int i = 0; i < n; i++) {
        for (int p = 0; p < k; p++) {
            if (A[i][p]) {
                C[i] ^= B[p];
            }
        }

        // 念のため m 以上のbitを消す
        for (int j = m; j < MAX; j++) {
            C[i][j] = 0;
        }
    }

    return C;
}

Mat make_identity(int n) {
    Mat I(n);
    for (int i = 0; i < n; i++) {
        I[i][i] = 1;
    }
    return I;
}

Mat matpow(Mat base, long long e, int n) {
    Mat res = make_identity(n);

    while (e > 0) {
        if (e & 1) res = mul(res, base, n, n, n);
        base = mul(base, base, n, n, n);
        e >>= 1;
    }

    return res;
}

int main() {
    int n, m;
    long long t;
    cin >> n >> m >> t;

    Mat A(n);

    for (int i = 0; i < n; i++) {
        string s;
        cin >> s;
        for (int j = 0; j < m; j++) {
            if (s[j] == 'S') {
                A[i][j] = 1;
            }
        }
    }

    Mat V(n);
    for (int i = 0; i < n; i++) {
        V[i][smod(i - 1, n)] = 1;
        V[i][i] = 1;
        V[i][smod(i + 1, n)] = 1;
    }

    Mat H(m);
    for (int j = 0; j < m; j++) {
        H[j][smod(j - 1, m)] = 1;
        H[j][j] = 1;
        H[j][smod(j + 1, m)] = 1;
    }

    Mat VT = matpow(V, t, n);
    Mat HT = matpow(H, t, m);

    Mat B = mul(VT, A, n, n, m);
    Mat C = mul(B, HT, n, m, m);

    long long ans = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (C[i][j]) ans++;
        }
    }

    cout << ans << '\n';
}