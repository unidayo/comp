// 最終更新: 2026-07-13 03:15:42 JST
// LowLink: 関節点・橋を求める
// - add_edge(u, v) で無向辺を追加（辺番号 = 追加順）
// - build() で構築
// - articulations: 関節点リスト（昇順）
// - bridges: 橋リスト。Bridge{u, v, id} で両端点と辺番号を保持
// 多重辺対応（辺IDで親管理）、非連結グラフ対応
// build() は O(V + E) 時間、O(V + E) メモリ。
// 隣接辺をbuild時にCSR形式へ変換し、反復DFSで走査するため、大規模な木でも再帰スタックが溢れない。
struct LowLink {
    struct Bridge {
        int u, v, id;  // 橋の両端点（u < v）と辺番号
    };

    int n;
    vector<int> articulations;
    vector<Bridge> bridges;

    explicit LowLink(int vertex_count) : n(vertex_count), ord(vertex_count, -1), low(vertex_count) {
        assert(vertex_count >= 0);
    }

    void add_edge(int u, int v) {
        assert(0 <= u && u < n && 0 <= v && v < n);
        edges.push_back({u, v});
    }

    void reserve_edges(int m) { edges.reserve(m); }

    void build() {
        articulations.clear();
        bridges.clear();
        fill(ord.begin(), ord.end(), -1);

        struct AdjacentEdge { int to, id; };
        vector<int> offset(n + 1);
        for (auto [u, v] : edges) {
            ++offset[u + 1];
            ++offset[v + 1];
        }
        partial_sum(offset.begin(), offset.end(), offset.begin());
        vector<int> cursor = offset;
        vector<AdjacentEdge> adjacency(edges.size() * 2);
        for (int id = 0; id < (int)edges.size(); ++id) {
            auto [u, v] = edges[id];
            adjacency[cursor[u]++] = {v, id};
            adjacency[cursor[v]++] = {u, id};
        }

        vector<unsigned char> is_art(n);
        vector<int> parent(n, -1), parent_edge(n, -1), next_edge(n), child_count(n);
        vector<int> stack;
        stack.reserve(n);
        int timer = 0;
        for (int root = 0; root < n; ++root) {
            if (ord[root] != -1) continue;
            ord[root] = low[root] = timer++;
            next_edge[root] = offset[root];
            stack.push_back(root);

            while (!stack.empty()) {
                int u = stack.back();
                if (next_edge[u] < offset[u + 1]) {
                    auto [v, edge_id] = adjacency[next_edge[u]++];
                    if (edge_id == parent_edge[u]) continue;
                    if (ord[v] != -1) {
                        low[u] = min(low[u], ord[v]);
                        continue;
                    }

                    parent[v] = u;
                    parent_edge[v] = edge_id;
                    ++child_count[u];
                    ord[v] = low[v] = timer++;
                    next_edge[v] = offset[v];
                    stack.push_back(v);
                    continue;
                }

                stack.pop_back();
                int p = parent[u];
                if (p == -1) {
                    if (child_count[u] >= 2) is_art[u] = true;
                    continue;
                }

                low[p] = min(low[p], low[u]);
                if (parent[p] != -1 && ord[p] <= low[u]) is_art[p] = true;
                if (ord[p] < low[u]) {
                    int a = min(p, u), b = max(p, u);
                    bridges.push_back({a, b, parent_edge[u]});
                }
            }
        }
        for (int i = 0; i < n; i++) {
            if (is_art[i]) articulations.push_back(i);
        }
    }

   private:
    vector<pair<int, int>> edges;
    vector<int> ord, low;
};
