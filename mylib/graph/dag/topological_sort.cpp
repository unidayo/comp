// 最終更新: 2026-07-30 12:26:58 JST
// Kahn 法によるトポロジカルソート。計算量 O(V + E)、追加メモリ O(V)。
// DAG なら全頂点を返し、閉路があれば閉路の影響を受けない頂点だけを返す。
vector<int> topological_sort(const vector<vector<int>>& graph) {
    const int n = int(graph.size());
    vector<int> indegree(n, 0);
    for (const auto& edges : graph) {
        for (const int v : edges) ++indegree[v];
    }

    // order を返却値とキューの両方に使う。
    // deque と返却用 vector を別々に持たず、連続メモリを順に読むことで高速化する。
    vector<int> order;
    order.reserve(n);
    for (int v = 0; v < n; ++v) {
        if (indegree[v] == 0) order.push_back(v);
    }

    for (size_t head = 0; head < order.size(); ++head) {
        const int u = order[head];
        for (const int v : graph[u]) {
            if (--indegree[v] == 0) order.push_back(v);
        }
    }
    return order;
}

// 旧版の綴り間違いを使っているコードとの互換性を保つ。
vector<int> topologiral_sort(const vector<vector<int>>& graph) {
    return topological_sort(graph);
}
