// 最終更新: 2026-07-12 14:27:44 JST
/**
 * @brief Dijkstra 法
 *
 * 非負辺重み付きグラフの単一始点最短路を求める。
 *
 * 前提:
 * - `G[u]` は `{to, cost}` の配列
 * - 辺重み `cost` は non-negative
 *
 * 計算量:
 * - O((N + M) log N)
 *
 * 使い方:
 *   vector<vector<pair<int, ll>>> G(n);
 *   G[u].push_back({v, w});
 *
 *   auto dist = dijkstra(G, s);
 *   auto res = dijkstra_with_prev(G, s);
 *
 *   // t への最短距離
 *   ll d = dist[t];
 *
 *   // 経路復元
 *   vector<int> path;
 *   for (int cur = t; cur != -1; cur = res.prev[cur]) path.push_back(cur);
 *   reverse(path.begin(), path.end());
 *
 * 注意:
 * - 到達不能な頂点の距離は `INF64`
 */

struct DijkstraResult {
    vector<ll> dist;
    vector<int> prev;
};

vector<ll> dijkstra(const vector<vector<pair<int, ll>>> &G, int s) {
    int N = G.size();
    vector<ll> dist(N, INF64);
    dist[s] = 0;
    priority_queue_min<pair<ll, int>> pq;
    pq.emplace(0, s);

    while (!pq.empty()) {
        auto [cost, u] = pq.top();
        pq.pop();
        if (cost > dist[u]) continue;
        for (auto [v, c] : G[u]) {
            if (dist[u] > INF64 - c) continue;
            if (dist[u] + c < dist[v]) {
                dist[v] = dist[u] + c;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

DijkstraResult dijkstra_with_prev(const vector<vector<pair<int, ll>>> &G, int s) {
    int N = G.size();
    vector<ll> dist(N, INF64);
    vector<int> prev(N, -1);
    dist[s] = 0;
    priority_queue_min<pair<ll, int>> pq;
    pq.emplace(0, s);

    while (!pq.empty()) {
        auto [cost, u] = pq.top();
        pq.pop();
        if (cost > dist[u]) continue;
        for (auto [v, c] : G[u]) {
            if (dist[u] > INF64 - c) continue;
            if (dist[u] + c < dist[v]) {
                dist[v] = dist[u] + c;
                prev[v] = u;
                pq.emplace(dist[v], v);
            }
        }
    }
    return {dist, prev};
}
