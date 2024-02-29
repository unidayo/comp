
auto bellman_ford(const Graph &G, int start, bool detect_negative_cycle = false) {
    int N = ssize(G);
    vector<ll> dist(N, INF64);
    vector<int> pred(N, -1);
    for (int i : ranges::iota_view(0, N - 1)) {
        for (int u : ranges::iota_view(0, N)) {
            if (dist[u] == INF64) {
                continue;
            }
            for (auto [v, d] : G[u]) {
                if (chmin(dist[v], dist[u] + d)) {
                    pred[v] = u;
                }
            }
        }
    }

    if (detect_negative_cycle) {
        for (int i : ranges::iota_view(0, N)) {
            for (int u : ranges::iota_view(0, N)) {
                if (dist[u] == INF64) {
                    continue;
                }
                for (auto [v, d] : G[u]) {
                    if (dist[u] + d < dist[v]) {
                        dist[v] = -INF64; // 頂点vは負閉路に属する
                        pred[v] = u;
                    }
                }
            }
        }
    }
    return make_tuple(dist, pred);
}