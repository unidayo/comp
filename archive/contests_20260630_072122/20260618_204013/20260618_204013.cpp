#include <bits/stdc++.h>
using namespace std;

using ll = long long;

const ll INF = (1LL << 60);

// g[x] = min_i f[i] + (x - i)^2 を O(n) で求める
vector<ll> distance_transform_1d(const vector<ll>& f) {
    int n = (int)f.size();

    vector<ll> g(n, INF);
    vector<int> v(n);
    vector<long double> z(n + 1);

    auto intersect = [&](int a, int b) -> long double {
        // f[a] + (x-a)^2 = f[b] + (x-b)^2 となる x
        return (((long double)f[b] + (long double)b * b) - ((long double)f[a] + (long double)a * a)) / (2.0L * (b - a));
    };

    int k = -1;

    for (int q = 0; q < n; q++) {
        if (f[q] >= INF / 2) continue;

        if (k == -1) {
            k = 0;
            v[0] = q;
            z[0] = -1e100L;
            z[1] = 1e100L;
        } else {
            while (true) {
                long double s = intersect(v[k], q);

                if (s <= z[k]) {
                    k--;
                    if (k == -1) {
                        k = 0;
                        v[0] = q;
                        z[0] = -1e100L;
                        z[1] = 1e100L;
                        break;
                    }
                } else {
                    k++;
                    v[k] = q;
                    z[k] = s;
                    z[k + 1] = 1e100L;
                    break;
                }
            }
        }
    }

    if (k == -1) return g;

    int cur = 0;
    for (int x = 0; x < n; x++) {
        while (z[cur + 1] < x) cur++;

        ll d = x - v[cur];
        g[x] = f[v[cur]] + d * d;
    }

    return g;
}

int main() {
    int H, W;
    cin >> H >> W;

    // 外側にはすべてワープスポットがあるので、周囲を '.' で囲む
    int HH = H + 2;
    int WW = W + 2;

    vector<string> grid(HH, string(WW, '.'));

    for (int y = 0; y < H; y++) {
        string s;
        cin >> s;
        for (int x = 0; x < W; x++) {
            grid[y + 1][x + 1] = s[x];
        }
    }

    vector<ll> row_dist((ll)HH * WW, INF);

    // 横方向の距離変換
    for (int y = 0; y < HH; y++) {
        vector<ll> f(WW, INF);

        for (int x = 0; x < WW; x++) {
            if (grid[y][x] == '.') {
                f[x] = 0;
            }
        }

        vector<ll> g = distance_transform_1d(f);

        for (int x = 0; x < WW; x++) {
            row_dist[(ll)y * WW + x] = g[x];
        }
    }

    ll best = 0;

    // 縦方向の距離変換
    for (int x = 0; x < WW; x++) {
        vector<ll> f(HH);

        for (int y = 0; y < HH; y++) {
            f[y] = row_dist[(ll)y * WW + x];
        }

        vector<ll> g = distance_transform_1d(f);

        for (int y = 1; y <= H; y++) {
            if (grid[y][x] == '#') {
                best = max(best, g[y]);
            }
        }
    }

    cout << fixed << setprecision(12) << sqrt((long double)best) << '\n';
}