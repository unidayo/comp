#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    array<long long, 200> cnt{};
    for (int i = 0; i < n; ++i) {
        int x;
        cin >> x;
        ++cnt[x % 200];
    }

    long long ans = 0;
    for (long long c : cnt) ans += c * (c - 1) / 2;
    cout << ans << '\n';
    return 0;
}
