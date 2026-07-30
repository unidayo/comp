// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/acl/scc.cpp
/**
 * DirectedCycleDetector
 *
 * 有向グラフにおけるサイクル検出と、
 * サイクルに含まれる頂点の列挙を行うライブラリ。
 *
 * 機能:
 *   - has_cycle()             : サイクルが存在するか判定
 *   - find_one_cycle()        : サイクルを1つ見つけて頂点列として返す
 *   - find_all_cycle_vertices(): 何らかのサイクルに含まれる全頂点を vector<bool> で返す
 *
 * 計算量: O(V + E)
 *
 * 使用例:
 *   DirectedCycleDetector g(4);
 *   g.add_edge(0, 1);
 *   g.add_edge(1, 2);
 *   g.add_edge(2, 0);
 *   g.add_edge(2, 3);
 *
 *   g.has_cycle();               // true
 *   g.find_one_cycle();          // {0, 1, 2}
 *   g.find_all_cycle_vertices(); // {true, true, true, false}
 */
struct DirectedCycleDetector {
    int n;
    std::vector<std::vector<int>> adj;

    explicit DirectedCycleDetector(int n) : n(n), adj(n) {}

    void add_edge(int from, int to) { adj[from].push_back(to); }

    // --- サイクルが存在するか判定 ---
    bool has_cycle() const {
        std::vector<int> color(n, 0);  // 0: 未訪問, 1: 訪問中, 2: 完了
        bool found = false;

        auto dfs = [&](auto&& self, int u) -> void {
            if (found) return;
            color[u] = 1;
            for (int v : adj[u]) {
                if (found) return;
                if (color[v] == 1) {
                    found = true;
                    return;
                }
                if (color[v] == 0) self(self, v);
            }
            color[u] = 2;
        };

        for (int i = 0; i < n && !found; i++) {
            if (color[i] == 0) dfs(dfs, i);
        }
        return found;
    }

    // --- サイクルを1つ見つけて頂点列として返す (存在しなければ空) ---
    std::vector<int> find_one_cycle() const {
        std::vector<int> color(n, 0);
        std::vector<int> parent(n, -1);
        int cycle_start = -1, cycle_end = -1;
        bool found = false;

        auto dfs = [&](auto&& self, int u) -> void {
            if (found) return;
            color[u] = 1;
            for (int v : adj[u]) {
                if (found) return;
                if (color[v] == 1) {
                    cycle_start = v;
                    cycle_end = u;
                    found = true;
                    return;
                }
                if (color[v] == 0) {
                    parent[v] = u;
                    self(self, v);
                }
            }
            color[u] = 2;
        };

        for (int i = 0; i < n && !found; i++) {
            if (color[i] == 0) dfs(dfs, i);
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

    // --- 何らかのサイクルに含まれる全頂点を返す ---
    // サイズ >= 2 の強連結成分に属する頂点 (atcoder::scc_graph)
    std::vector<bool> find_all_cycle_vertices() const {
        atcoder::scc_graph sg(n);
        for (int u = 0; u < n; u++) {
            for (int v : adj[u]) {
                sg.add_edge(u, v);
            }
        }

        // 自己ループを持つ頂点を記録
        std::vector<bool> has_self_loop(n, false);
        for (int u = 0; u < n; u++) {
            for (int v : adj[u]) {
                if (u == v) has_self_loop[u] = true;
            }
        }

        std::vector<bool> on_cycle(n, false);
        for (auto& comp : sg.scc()) {
            if ((int)comp.size() >= 2) {
                for (int v : comp) {
                    on_cycle[v] = true;
                }
            } else {
                // サイズ1でも自己ループがあればサイクル
                if (has_self_loop[comp[0]]) {
                    on_cycle[comp[0]] = true;
                }
            }
        }
        return on_cycle;
    }
};
