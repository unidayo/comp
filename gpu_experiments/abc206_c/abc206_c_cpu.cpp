#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    unordered_map<int, long long> cnt;
    cnt.reserve(static_cast<size_t>(n) * 2);

    long long equal_pairs = 0;
    for (int i = 0; i < n; ++i) {
        int x;
        cin >> x;
        equal_pairs += cnt[x];
        ++cnt[x];
    }

    const long long total_pairs = 1LL * n * (n - 1) / 2;
    cout << total_pairs - equal_pairs << '\n';
    return 0;
}
