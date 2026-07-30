// 最終更新: 2026-07-12 14:27:44 JST
/**
 * @brief Heavy-Light Decomposition（HL分解）
 *
 * 木上のパスクエリ・部分木クエリをセグ木と組み合わせて O(log^2 N) で処理する。
 * スタック溢れを避けるため DFS は反復で実装している。
 *
 * 【使い方】
 *   HLD hld(n);
 *   hld.add_edge(u, v);
 *   hld.build(root);       // デフォルト root=0
 *
 *   // セグ木は hld.pos[v] の位置に頂点 v の値を置いて初期化する
 *
 *   // 頂点クエリ（可換演算）
 *   ll ans = 0;
 *   for (auto [l, r, rev] : hld.path(u, v)) ans += seg.prod(l, r);
 *
 *   // 辺クエリ: 辺の重みを子側の頂点に持たせ、vertex=false で LCA を除く
 *   //   build 後に辺 (u,v) の重み w を seg.set(hld.pos[child], w) で設定する
 *   //   child は depth が深い方: (hld.depth[u] > hld.depth[v]) ? u : v
 *   for (auto [l, r, rev] : hld.path(u, v, false)) ans += seg.prod(l, r);
 *
 *   // 非可換演算（行列積など）
 *   // rev=true のセグメントは右→左方向（逆順積が必要）
 *   Matrix ans = I;
 *   for (auto [l, r, rev] : hld.path(u, v)) {
 *       ans = ans * (rev ? seg.prod_rev(l, r) : seg.prod(l, r));
 *   }
 *
 *   // 部分木クエリ
 *   auto [l, r] = hld.subtree(v);
 *   ans = seg.prod(l, r);
 *
 * 【メンバ】
 *   pos[v]   : v の HLD 配列上の位置
 *   sz[v]    : v の部分木サイズ
 *   depth[v] : 根からの深さ
 *   parent[v]: 親頂点（根は -1）
 *   head[v]  : v が属するチェーンの先頭頂点
 */
struct HLD {
    int n;
    vector<vector<int>> G;
    vector<int> parent, depth, sz, heavy, head, pos;
    int cur_pos = 0;

    HLD(int n) : n(n), G(n), parent(n, -1), depth(n, 0), sz(n, 1), heavy(n, -1), head(n, 0), pos(n, 0) {}

    void add_edge(int u, int v) {
        G[u].push_back(v);
        G[v].push_back(u);
    }

    void build(int root = 0) {
        if (n == 0) return;
        fill(parent.begin(), parent.end(), -1);
        fill(depth.begin(), depth.end(), 0);
        fill(sz.begin(), sz.end(), 1);
        fill(heavy.begin(), heavy.end(), -1);
        fill(head.begin(), head.end(), 0);
        fill(pos.begin(), pos.end(), 0);
        cur_pos = 0;

        // 部分木サイズと heavy child を求める（BFS 順で逆順に処理）
        vector<int> order;
        order.reserve(n);
        stack<pair<int, int>> stk;
        stk.push({root, -1});
        while (!stk.empty()) {
            auto [v, p] = stk.top();
            stk.pop();
            parent[v] = p;
            order.push_back(v);
            for (int u : G[v]) {
                if (u == p) continue;
                depth[u] = depth[v] + 1;
                stk.push({u, v});
            }
        }
        for (int i = (int)order.size() - 1; i >= 0; i--) {
            int v = order[i];
            for (int u : G[v]) {
                if (u == parent[v]) continue;
                sz[v] += sz[u];
                if (heavy[v] == -1 || sz[u] > sz[heavy[v]]) heavy[v] = u;
            }
        }
        // HLD ラベリング（heavy child を優先して DFS）
        stk.push({root, root});
        while (!stk.empty()) {
            auto [v, h] = stk.top();
            stk.pop();
            head[v] = h;
            pos[v] = cur_pos++;
            // light children を先に積む（後回し）
            for (int u : G[v]) {
                if (u == parent[v] || u == heavy[v]) continue;
                stk.push({u, u});
            }
            // heavy child を後に積む（先に処理 → 連続した pos が割り当たる）
            if (heavy[v] != -1) stk.push({heavy[v], h});
        }
    }

    // パス u-v 上の区間リストを返す（u→v の順）
    // 各要素は {l, r, rev}: セグ木上の区間 [l, r)
    //   rev=false: 左→右方向（LCA→v 側）
    //   rev=true : 右→左方向（u→LCA 側、非可換演算で逆順クエリが必要）
    // vertex=false のとき辺クエリ（LCA 頂点を除く）
    // 可換演算なら rev を無視して prod(l, r) するだけでよい
    vector<tuple<int, int, bool>> path(int u, int v, bool vertex = true) {
        vector<tuple<int, int, bool>> segs_u, segs_v;
        for (; head[u] != head[v];) {
            if (depth[head[u]] >= depth[head[v]]) {
                segs_u.emplace_back(pos[head[u]], pos[u] + 1, true);
                u = parent[head[u]];
            } else {
                segs_v.emplace_back(pos[head[v]], pos[v] + 1, false);
                v = parent[head[v]];
            }
        }
        if (depth[u] >= depth[v]) {
            segs_u.emplace_back(pos[v] + (vertex ? 0 : 1), pos[u] + 1, true);
        } else {
            segs_v.emplace_back(pos[u] + (vertex ? 0 : 1), pos[v] + 1, false);
        }
        // u 側は u→LCA 順に並んでいる。v 側は逆順に積んでいるので reverse して連結
        reverse(segs_v.begin(), segs_v.end());
        for (auto& s : segs_v) segs_u.push_back(s);
        return segs_u;
    }

    // 部分木 v の区間 [l, r) を返す
    pair<int, int> subtree(int v) { return {pos[v], pos[v] + sz[v]}; }

    // u, v の LCA を返す
    int lca(int u, int v) {
        for (; head[u] != head[v];) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }

    // u, v 間の距離（辺の数）を返す
    int dist(int u, int v) {
        return depth[u] + depth[v] - 2 * depth[lca(u, v)];
    }
};
