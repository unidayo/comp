// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/graph/tree/diameter.cpp
// 木の直径（パスの本数）とパス上の頂点を求める
pair<int, vector<int>> calc_diameter(const vector<vector<int>> &G, int start = 0) {
    const int N = G.size();

    struct BFSResult {
        int dist;
        int farthest;
        vector<int> prev;
    };

    auto bfs = [&](int s) -> BFSResult {
        queue<int> q;
        vector<int> dist(N, -1), prev(N, -1);
        dist[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : G[u]) {
                if (dist[v] != -1) continue;
                dist[v] = dist[u] + 1;
                prev[v] = u;
                q.push(v);
            }
        }

        int far = s;
        for (int i = 0; i < N; i++) {
            if (dist[i] > dist[far]) far = i;
        }

        return {dist[far], far, prev};
    };

    auto r1 = bfs(start);
    auto r2 = bfs(r1.farthest);

    vector<int> path;
    for (int cur = r2.farthest; cur != -1; cur = r2.prev[cur]) {
        path.push_back(cur);
    }

    return {r2.dist, path};
}
