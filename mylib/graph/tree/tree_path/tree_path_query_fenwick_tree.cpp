// 最終更新: 2026-07-28 19:00:12 JST
// 依存: mylib/acl/fenwicktree.cpp
/*
重み付き木に対するLCAと距離クエリ（辺重み更新対応版）

使い方:
  LCA tree(n);
  // またはグローバル変数として LCA tree; と置き、tree.init(n);
  tree.add_edge(u, v, w);
  tree.build(root = 0);

メンバ関数:
  - add_edge(u, v, w): 頂点u-v間に重みwの辺を追加し、辺番号を返す
  - build(root): 追加した辺からLCA用のデータ構造を構築する
  - lca(u, v): 頂点u, vのLCAを返す O(1)
  - dist(u, v): 頂点u, v間の距離を返す O(log N)
  - set_cost(i, w): i番目の辺（0-indexed, 入力順）の重みをwに変更 O(log N)
*/

class LCA {
    using P = pair<int, int>;

    int n;
    vector<int> idx, vs, dep, lg;
    vector<P> eidx;
    vector<vector<int>> st;
    vector<vector<tuple<int, int, ll>>> g;
    fenwick_tree<ll> ft;

    int min_dep_idx(int a, int b) const { return dep[a] <= dep[b] ? a : b; }

   public:
    LCA() : LCA(0) {}
    explicit LCA(int n) { init(n); }

    void init(int n_) {
        n = n_;
        idx.assign(n, 0);
        vs.clear();
        dep.clear();
        lg.clear();
        eidx.clear();
        st.clear();
        g.assign(n, {});
        ft = fenwick_tree<ll>(0);
    }

    int add_edge(int u, int v, ll w = 1) {
        int i = (int)eidx.size();
        eidx.emplace_back();
        g[u].emplace_back(i, v, w);
        g[v].emplace_back(i, u, w);
        return i;
    }

    void build(int root = 0) {
        idx.assign(n, 0);
        vs.assign(2 * n - 1, 0);
        dep.assign(2 * n - 1, 0);
        ft = fenwick_tree<ll>(2 * n - 1);
        int k = 0;
        auto dfs = [&](auto& f, int u, int p, int d) -> void {
            idx[u] = k;
            vs[k] = u;
            dep[k++] = d;
            for (auto [i, v, w] : g[u])
                if (v != p) {
                    ft.add(k, w);
                    eidx[i].first = k;
                    f(f, v, u, d + 1);
                    vs[k] = u;
                    ft.add(k, -w);
                    eidx[i].second = k;
                    dep[k++] = d;
                }
        };
        dfs(dfs, root, -1, 0);

        lg.assign(k + 1, 0);
        for (int i = 2; i <= k; i++) lg[i] = lg[i >> 1] + 1;

        st.assign(lg[k] + 1, vector<int>(k));
        iota(st[0].begin(), st[0].end(), 0);
        for (int j = 1; j < (int)st.size(); j++) {
            int len = 1 << j;
            int half = len >> 1;
            for (int i = 0; i + len <= k; i++) {
                st[j][i] = min_dep_idx(st[j - 1][i], st[j - 1][i + half]);
            }
        }
    }

    int lca(int u, int v) {
        auto [l, r] = minmax(idx[u], idx[v]);
        int len = r - l + 1;
        int j = lg[len];
        return vs[min_dep_idx(st[j][l], st[j][r - (1 << j) + 1])];
    }

    ll dist(int u, int v) {
        int p = lca(u, v);
        return ft.sum(0, idx[u] + 1) + ft.sum(0, idx[v] + 1) - 2 * ft.sum(0, idx[p] + 1);
    }

    void set_cost(int i, ll w) {
        ll d = w - ft.sum(eidx[i].first, eidx[i].first + 1);
        ft.add(eidx[i].first, d);
        ft.add(eidx[i].second, -d);
    }
};
