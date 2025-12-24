pair<int, vector<int>> calc_diameter(const vector<vector<int>>& G, int start = 0) {
    // 木の直径（パスの本数）とパス上の頂点を求める
    const int N = G.size();
    auto bfs = [&](int s) -> tuple<int, int, vector<int>> {
        queue<int> que;
        vector<int> dist(N, -1), prev(N, -1);
        dist[s] = 0;
        que.push(s);
        while (!que.empty()) {
            int u = que.front();
            que.pop();
            int c = dist[u];
            for (int v : G[u]) {
                if (dist[v] == -1) {
                    dist[v] = c + 1;
                    prev[v] = u;
                    que.push(v);
                }
            }
        }
        int max_dist = 0, md_index = 0;
        for (int i = 0; i < N; i++) {
            if (dist[i] > max_dist) {
                max_dist = dist[i];
                md_index = i;
            }
        }
        return {max_dist, md_index, prev};
    };

    auto [m1, v1, prev1] = bfs(start);
    auto [diameter, v2, prev2] = bfs(v1);

    vector<int> L(diameter + 1, -1);
    L[0] = v2;
    int curr = v2;
    for (int i = 0; i < diameter; i++) {
        curr = prev2[curr];
        L[i + 1] = curr;
    }

    return {diameter, L};
}
