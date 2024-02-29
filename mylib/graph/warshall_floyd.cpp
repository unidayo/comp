auto warshall_floyd(const Graph &G) {
    int N = ssize(G);
    vector dist(N, vector(N, INF64));
    vector pred(N, vector(N, -1));
    bool exists_negative_cycle = false;
    for (int u = 0; u < N; ++u) {
        for (auto [v, weight] : G[u]) {
            dist[u][v] = weight;
        }
    }
    for (int k = 0; k < N; ++k) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (chmin(dist[i][j], dist[i][k] + dist[k][j])) {
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