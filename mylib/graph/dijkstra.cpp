struct Edge {
    int to;
    ll weight;
};
using Graph = vector<vector<Edge>>;

auto dijkstra(Graph &G, int start) {
    int N = ssize(G);
    vector<ll> dist(N, INF64);
    vector<int> pred(N, -1);
    dist[start] = 0;
    priority_queue_min<pair<ll, int>> pq;
    pq.emplace(0, start);
    while (!pq.empty()) {
        auto [cost, u] = pq.top();
        pq.pop();
        if (dist[u] < cost) continue;
        for (auto [v, weight] : G[u]) {
            ll new_cost = cost + weight;
            if (chmin(dist[v], new_cost)) {
                pred[v] = u;
                pq.emplace(new_cost, v);
            }
        }
    }
    return make_tuple(dist, pred);
}