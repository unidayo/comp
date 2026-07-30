// 最終更新: 2026-07-12 14:27:44 JST
auto warshall_floyd(const Graph &G) {
    int N = ssize(G);
    vector dist(N, vector(N, INF64));
    vector pred(N, vector(N, -1));
    bool exists_negative_cycle = false;
    for (int u = 0; u < N; ++u) dist[u][u] = 0;
    for (int u = 0; u < N; ++u) {
        for (auto [v, weight] : G[u]) {
            dist[u][v] = min(dist[u][v], weight);
        }
    }
    for (int k = 0; k < N; ++k) {
        for (int i = 0; i < N; ++i) {
            if (dist[i][k] == INF64) continue;
            for (int j = 0; j < N; ++j) {
                if (dist[k][j] == INF64) continue;
                __int128 candidate = (__int128)dist[i][k] + dist[k][j];
                candidate = max<__int128>(-INF64, min<__int128>(INF64, candidate));
                if (chmin(dist[i][j], (ll)candidate)) {
                    pred[i][j] = k;
                }
            }
        }
    }
    for (int u = 0; u < N; ++u) {
        if (dist[u][u] < 0) {
            exists_negative_cycle = true;
        }
    }
    return make_tuple(dist, pred, exists_negative_cycle);
}
