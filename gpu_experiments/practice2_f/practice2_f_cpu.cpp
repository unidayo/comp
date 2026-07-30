#include <atcoder/convolution>
#include <atcoder/modint>

#include <bits/stdc++.h>
using namespace std;
using atcoder::convolution;

#ifndef MOD_VALUE
#define MOD_VALUE 998244353
#endif

using mint = atcoder::static_modint<MOD_VALUE>;

unsigned long long checksum_result(const vector<mint> &a) {
    unsigned long long h = 1469598103934665603ull;
    for (const auto &x : a) {
        h ^= static_cast<unsigned>(x.val());
        h *= 1099511628211ull;
    }
    return h;
}

int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    bool checksum = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--checksum") checksum = true;
    }

    int n, m;
    cin >> n >> m;
    vector<mint> a(n), b(m);
    for (auto &x : a) {
        int v;
        cin >> v;
        x = v;
    }
    for (auto &x : b) {
        int v;
        cin >> v;
        x = v;
    }

    vector<mint> c = convolution(a, b);
    if (checksum) {
        cout << checksum_result(c) << '\n';
        return 0;
    }
    for (int i = 0; i < static_cast<int>(c.size()); ++i) {
        if (i) cout << ' ';
        cout << c[i].val();
    }
    cout << '\n';
}
