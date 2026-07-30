// 最終更新: 2026-07-12 14:27:44 JST
/**
 * @brief DSU on Tree (Sack)
 *
 * 木の各頂点 v に対して「v の部分木に関する情報」を
 * small-to-large で O(N log N) 程度で処理するためのライブラリ。
 *
 * 内部で Euler Tour により部分木を連続区間にし、heavy child を 1 本だけ残して
 * light subtree を add / erase しながら答えを作る。
 *
 * 【使い方】
 *   DSUOnTree dsu(n);
 *   dsu.add_edge(u, v);
 *
 *   vector<int> color(n), cnt(MAXC), ans(n);
 *   int distinct = 0;
 *
 *   auto add = [&](int v) {
 *       if (cnt[color[v]]++ == 0) distinct++;
 *   };
 *   auto erase = [&](int v) {
 *       if (--cnt[color[v]] == 0) distinct--;
 *   };
 *   auto query = [&](int v) {
 *       ans[v] = distinct;  // 現在，v の部分木全体が載っている
 *   };
 *
 *   dsu.run(add, erase, query);   // root はデフォルトで 0
 *
 * 【コールバックの意味】
 *   add(v)   : 頂点 v を現在のデータ構造に追加する
 *   erase(v) : 頂点 v を現在のデータ構造から削除する
 *   query(v) : v の部分木がちょうど載っているタイミングで呼ばれる
 *
 * 【計算量】
 *   前処理 O(N)
 *   run    O(N log N)（各頂点が light edge をまたぐ回数だけ add / erase される）
 *
 * 【注意】
 *   - 再帰 DFS を使っているので、非常に深い木ではスタックに注意。
 *   - query(v) 実行時点では、現在のデータ構造には「v の部分木」だけが入っている。
 */
struct DSUOnTree {
    int n;
    vector<vector<int>> G;
    vector<int> parent, sz, heavy, tin, tout, euler;
    int timer = 0;

    DSUOnTree(int n) : n(n), G(n), parent(n, -1), sz(n, 1), heavy(n, -1), tin(n), tout(n), euler(n) {}

    void add_edge(int u, int v) {
        G[u].push_back(v);
        G[v].push_back(u);
    }

    void build(int root = 0) {
        timer = 0;
        fill(parent.begin(), parent.end(), -1);
        fill(sz.begin(), sz.end(), 1);
        fill(heavy.begin(), heavy.end(), -1);

        vector<int> order;
        order.reserve(n);
        stack<int> stk;
        stk.push(root);
        parent[root] = -1;
        while (!stk.empty()) {
            int v = stk.top();
            stk.pop();
            order.push_back(v);
            for (int to : G[v]) {
                if (to == parent[v]) continue;
                parent[to] = v;
                stk.push(to);
            }
        }

        for (int i = (int)order.size() - 1; i >= 0; --i) {
            int v = order[i];
            for (int to : G[v]) {
                if (to == parent[v]) continue;
                sz[v] += sz[to];
                if (heavy[v] == -1 || sz[heavy[v]] < sz[to]) heavy[v] = to;
            }
        }

        stack<pair<int, int>> dfs;
        dfs.push({root, 0});
        while (!dfs.empty()) {
            auto [v, state] = dfs.top();
            dfs.pop();
            if (state == 0) {
                tin[v] = timer;
                euler[timer++] = v;
                dfs.push({v, 1});
                for (int i = (int)G[v].size() - 1; i >= 0; --i) {
                    int to = G[v][i];
                    if (to == parent[v]) continue;
                    dfs.push({to, 0});
                }
            } else {
                tout[v] = timer;
            }
        }
    }

    template <class Add, class Erase, class Query>
    void run(Add add, Erase erase, Query query, int root = 0) {
        build(root);
        dfs(root, true, add, erase, query);
    }

   private:
    template <class Add>
    void add_subtree(int v, Add& add) {
        for (int i = tin[v]; i < tout[v]; ++i) add(euler[i]);
    }

    template <class Erase>
    void erase_subtree(int v, Erase& erase) {
        for (int i = tin[v]; i < tout[v]; ++i) erase(euler[i]);
    }

    template <class Add, class Erase, class Query>
    void dfs(int v, bool keep, Add& add, Erase& erase, Query& query) {
        for (int to : G[v]) {
            if (to == parent[v] || to == heavy[v]) continue;
            dfs(to, false, add, erase, query);
        }
        if (heavy[v] != -1) dfs(heavy[v], true, add, erase, query);
        for (int to : G[v]) {
            if (to == parent[v] || to == heavy[v]) continue;
            add_subtree(to, add);
        }
        add(v);
        query(v);
        if (!keep) erase_subtree(v, erase);
    }
};
