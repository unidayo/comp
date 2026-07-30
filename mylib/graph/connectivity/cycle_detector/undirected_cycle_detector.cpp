// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/acl/dsu.cpp
/**
 * UndirectedCycleDetector
 *
 * 無向グラフにおけるサイクル検出と、サイクルに含まれる頂点の列挙を行うライブラリ。
 *
 * 機能:
 *   - has_cycle()             : サイクルが存在するか判定
 *   - find_one_cycle()        : サイクルを1つ見つけて頂点列として返す
 *   - find_all_cycle_vertices(): 何らかのサイクルに含まれる全頂点を返す
 *
 * 計算量: O(V + E)
 *
 * 使用例:
 *   UndirectedCycleDetector g(5);
 *   g.add_edge(0, 1);
 *   g.add_edge(1, 2);
 *   g.add_edge(2, 0);
 *   g.add_edge(2, 3);
 *   g.add_edge(3, 4);
 *
 *   g.has_cycle();              // true
 *   g.find_one_cycle();         // {0, 1, 2} (順序は探索順)
 *   g.find_all_cycle_vertices(); // {0, 1, 2}
 */
struct UndirectedCycleDetector {
    int n;
    std::vector<std::vector<std::pair<int, int>>> adj;  // adj[u] = {(v, edge_id), ...}
    int edge_count = 0;

    explicit UndirectedCycleDetector(int n) : n(n), adj(n) {}

    void add_edge(int u, int v) {
        int id = edge_count++;
        adj[u].push_back({v, id});
        adj[v].push_back({u, id});
    }

    // --- サイクルが存在するか判定 ---
    bool has_cycle() const {
        std::vector<int> color(n, 0);  // 0: 未訪問, 1: 訪問中, 2: 完了
        bool found = false;

        auto dfs = [&](auto &&self, int u, int par_edge) -> void {
            if (found) return;
            color[u] = 1;
            for (auto [v, eid] : adj[u]) {
                if (found) return;
                if (eid == par_edge) continue;
                if (color[v] == 1) {
                    found = true;
                    return;
                }
                if (color[v] == 0) self(self, v, eid);
            }
            color[u] = 2;
        };

        for (int i = 0; i < n && !found; i++) {
            if (color[i] == 0) dfs(dfs, i, -1);
        }
        return found;
    }

    // --- サイクルを1つ見つけて頂点列として返す (存在しなければ空) ---
    std::vector<int> find_one_cycle() const {
        std::vector<int> color(n, 0);
        std::vector<int> parent(n, -1);
        int cycle_start = -1, cycle_end = -1;
        bool found = false;

        auto dfs = [&](auto &&self, int u, int par_edge) -> void {
            if (found) return;
            color[u] = 1;
            for (auto [v, eid] : adj[u]) {
                if (found) return;
                if (eid == par_edge) continue;
                if (color[v] == 1) {
                    cycle_start = v;
                    cycle_end = u;
                    found = true;
                    return;
                }
                if (color[v] == 0) {
                    parent[v] = u;
                    self(self, v, eid);
                }
            }
            color[u] = 2;
        };

        for (int i = 0; i < n && !found; i++) {
            if (color[i] == 0) dfs(dfs, i, -1);
        }

        if (!found) return {};

        std::vector<int> cycle;
        for (int v = cycle_end; v != cycle_start; v = parent[v]) {
            cycle.push_back(v);
        }
        cycle.push_back(cycle_start);
        std::reverse(cycle.begin(), cycle.end());
        return cycle;
    }

    // --- 何らかのサイクルに含まれる全頂点を集合として返す ---
    // 方針: lowlink で橋を検出し、橋でない辺で繋がる成分(サイズ>=2)の頂点を列挙
    std::set<int> find_all_cycle_vertices() const {
        std::vector<int> ord(n, -1), low(n, 0);
        std::vector<bool> is_bridge(edge_count, false);
        int timer = 0;

        auto dfs = [&](auto &&self, int u, int par_edge) -> void {
            ord[u] = low[u] = timer++;
            for (auto [v, eid] : adj[u]) {
                if (eid == par_edge) continue;
                if (ord[v] == -1) {
                    self(self, v, eid);
                    low[u] = std::min(low[u], low[v]);
                    if (low[v] > ord[u]) {
                        is_bridge[eid] = true;
                    }
                } else {
                    low[u] = std::min(low[u], ord[v]);
                }
            }
        };

        for (int i = 0; i < n; i++) {
            if (ord[i] == -1) dfs(dfs, i, -1);
        }

        // ACL の dsu で橋でない辺の端点をまとめる
        atcoder::dsu uf(n);
        for (int u = 0; u < n; u++) {
            for (auto [v, eid] : adj[u]) {
                if (!is_bridge[eid]) {
                    uf.merge(u, v);
                }
            }
        }

        std::set<int> result;
        for (int i = 0; i < n; i++) {
            if (uf.size(i) >= 2) {
                result.insert(i);
            }
        }
        for (int u = 0; u < n; ++u) {
            for (auto [v, eid] : adj[u]) {
                if (u == v) result.insert(u);
            }
        }
        return result;
    }
};
