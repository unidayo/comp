// 最終更新: 2026-07-12 14:27:44 JST
auto bellman_ford(const vector<vector<pair<int, ll>>> &G, int start, bool detect_negative_cycle = false) {
    int N = ssize(G);
    vector<ll> dist(N, INF64);
    dist[start] = 0;
    vector<int> pred(N, -1);
    for (int i = 0; i < N - 1; ++i) {
        for (int u = 0; u < N; ++u) {
            if (dist[u] == INF64) continue;
            for (auto [v, d] : G[u]) {
                if (chmin(dist[v], dist[u] + d)) {
                    pred[v] = u;
                }
            }
        }
    }

    if (detect_negative_cycle) {
        for (int i = 0; i < N; ++i) {
            for (int u = 0; u < N; ++u) {
                if (dist[u] == INF64) continue;
                for (auto [v, d] : G[u]) {
                    if (dist[u] + d < dist[v]) {
                        dist[v] = -INF64;  // 頂点vは負閉路に属する
                        pred[v] = u;
                    }
                }
            }
        }
    }
    return make_tuple(dist, pred);
}
