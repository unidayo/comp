// LowLink: 関節点・橋を求める
// - add_edge(u, v) で無向辺を追加（辺番号 = 追加順）
// - build() で構築
// - articulations: 関節点リスト（昇順）
// - bridges: 橋リスト。Bridge{u, v, id} で両端点と辺番号を保持
// 多重辺対応（辺IDで親管理）、非連結グラフ対応
struct LowLink {
    struct Bridge {
        int u, v, id;  // 橋の両端点（u < v）と辺番号
    };

    int n;
    vector<int> articulations;
    vector<Bridge> bridges;

    LowLink(int n) : n(n), G(n), ord(n, -1), low(n) {}

    void add_edge(int u, int v) {
        int id = (int)edges.size();
        edges.push_back({u, v});
        G[u].emplace_back(v, id);
        G[v].emplace_back(u, id);
    }

    void build() {
        vector<bool> is_art(n, false);
        int timer = 0;
        for (int i = 0; i < n; i++) {
            if (ord[i] == -1) dfs(i, -1, timer, is_art);
        }
        for (int i = 0; i < n; i++) {
            if (is_art[i]) articulations.push_back(i);
        }
    }

   private:
    vector<pair<int, int>> edges;
    vector<vector<pair<int, int>>> G;  // {to, edge_id}
    vector<int> ord, low;

    void dfs(int u, int p_eid, int &timer, vector<bool> &is_art) {
        ord[u] = low[u] = timer++;
        int children = 0;
        bool art = false;
        for (auto [v, eid] : G[u]) {
            if (eid == p_eid) continue;
            if (ord[v] != -1) {
                // 後退辺
                low[u] = min(low[u], ord[v]);
            } else {
                // 木辺
                children++;
                dfs(v, eid, timer, is_art);
                low[u] = min(low[u], low[v]);
                if (p_eid == -1) {
                    if (children >= 2) art = true;
                } else {
                    if (ord[u] <= low[v]) art = true;
                }
                if (ord[u] < low[v]) {
                    int a = min(u, v), b = max(u, v);
                    bridges.push_back({a, b, eid});
                }
            }
        }
        if (art) is_art[u] = true;
    }
};
