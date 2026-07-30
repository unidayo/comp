#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;
    vector<vector<int>> a(n, vector<int>(m));
    for (auto &row : a) {
        for (int &x : row) cin >> x;
    }

    int w = (n + 63) / 64;
    vector<vector<unsigned long long>> parity(
        n, vector<unsigned long long>(w, 0));

    vector<vector<unsigned long long>> masks(
        1000, vector<unsigned long long>(w));

    for (int col = 0; col < m; ++col) {
        for (auto &v : masks) fill(v.begin(), v.end(), 0);
        for (int row = 0; row < n; ++row) {
            int value = a[row][col];
            masks[value][row >> 6] |= 1ull << (row & 63);
        }
        for (int row = 0; row < n; ++row) {
            int value = a[row][col];
            for (int word = 0; word < w; ++word) {
                parity[row][word] ^= masks[value][word];
            }
        }
    }

    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        for (int word = (i + 1) >> 6; word < w; ++word) {
            unsigned long long bits = parity[i][word];
            int base = word << 6;
            if (base <= i) {
                int remove = i - base + 1;
                if (remove >= 64) bits = 0;
                else bits &= ~((1ull << remove) - 1);
            }
            if (word == w - 1 && (n & 63)) {
                bits &= (1ull << (n & 63)) - 1;
            }
            ans += __builtin_popcountll(bits);
        }
    }

    cout << ans << '\n';
}
