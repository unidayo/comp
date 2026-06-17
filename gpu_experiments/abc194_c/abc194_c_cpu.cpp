#include <bits/stdc++.h>
using namespace std;

void print_i128(__int128 x) {
    if (x == 0) {
        cout << 0 << '\n';
        return;
    }
    string s;
    while (x > 0) {
        s.push_back(static_cast<char>('0' + x % 10));
        x /= 10;
    }
    reverse(s.begin(), s.end());
    cout << s << '\n';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;
    vector<long long> a(n);
    for (long long &x : a) cin >> x;

    __int128 ans = 0;
    long long prefix_sum = 0;
    long long prefix_square_sum = 0;
    for (int i = 0; i < n; ++i) {
        ans += static_cast<__int128>(i) * a[i] * a[i] -
               2 * static_cast<__int128>(a[i]) * prefix_sum +
               prefix_square_sum;
        prefix_sum += a[i];
        prefix_square_sum += a[i] * a[i];
    }

    print_i128(ans);
    return 0;
}
