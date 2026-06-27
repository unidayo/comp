#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
    int N, M;
    cin >> N >> M;

    vector<ll> X(N), Y(N);
    for (int i = 0; i < N; i++) {
        cin >> X[i] >> Y[i];
    }

    vector<vector<int>> G(N);

    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v;
        --u, --v;
        G[u].push_back(v);
        G[v].push_back(u);
    }

    // 各頂点の隣接頂点を偏角順にソート
    for (int v = 0; v < N; v++) {
        sort(G[v].begin(), G[v].end(), [&](int a, int b) {
            long double aa = atan2l(Y[a] - Y[v], X[a] - X[v]);
            long double bb = atan2l(Y[b] - Y[v], X[b] - X[v]);
            return aa < bb;
        });
    }

    // pos[v][u] = G[v] の中で u が何番目か
    vector<vector<int>> pos(N, vector<int>(N, -1));
    for (int v = 0; v < N; v++) {
        for (int i = 0; i < (int)G[v].size(); i++) {
            pos[v][G[v][i]] = i;
        }
    }

    vector<vector<int>> used(N, vector<int>(N, 0));

    int ans = 0;

    for (int s = 0; s < N; s++) {
        for (int t : G[s]) {
            if (used[s][t]) continue;

            vector<int> face;

            int u = s;
            int v = t;

            while (!used[u][v]) {
                used[u][v] = 1;
                face.push_back(u);

                // v に u から来た。
                // v から見て u の1つ前の角度の頂点へ進む
                int p = pos[v][u];
                int deg = G[v].size();
                int w = G[v][(p - 1 + deg) % deg];

                u = v;
                v = w;
            }

            // 符号付き面積の2倍
            ll area2 = 0;
            int L = face.size();
            for (int i = 0; i < L; i++) {
                int a = face[i];
                int b = face[(i + 1) % L];
                area2 += X[a] * Y[b] - X[b] * Y[a];
            }

            // 頂点が重複していたら単純多角形ではない
            bool simple = true;
            vector<int> cnt(N, 0);
            for (int x : face) {
                cnt[x]++;
                if (cnt[x] >= 2) simple = false;
            }

            if (area2 > 0 && simple) {
                ans++;
            }
        }
    }

    cout << ans << '\n';
}