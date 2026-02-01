/*
重み付き木に対するLCAと距離クエリ

コンストラクタ: LCA(edges, root = 0)
  - edges: 辺のリスト vector<tuple<int, int, ll>>
           各要素は {u, v, w}（頂点u-v間の重みwの辺）
  - root: 根の頂点番号（デフォルト0）

メンバ関数:
  - lca(u, v): 頂点u, vのLCAを返す
  - dist(u, v): 頂点u, v間の距離を返す
  - set_cost(i, w): i番目の辺（0-indexed, 入力順）の重みをwに変更
*/

class LCA {
    using P = pair<int, int>;
    using ST = segtree<P, [](P a, P b) { return min(a, b); }, [] { return P{1 << 30, 0}; }>;

    int n;
    vector<int> idx, vs;
    vector<P> eidx;
    fenwick_tree<ll> ft;
    ST st;

   public:
    explicit LCA(const vector<tuple<int, int, ll>>& edges, int root = 0) : n(edges.size() + 1), idx(n), vs(2 * n - 1), eidx(n - 1), ft(2 * n - 1) {
        vector<vector<tuple<int, int, ll>>> g(n);
        for (int i = 0; i < n - 1; i++) {
            auto [u, v, w] = edges[i];
            g[u].emplace_back(i, v, w);
            g[v].emplace_back(i, u, w);
        }
        vector<P> dep(2 * n - 1);
        int k = 0;
        auto dfs = [&](auto& f, int u, int p, int d) -> void {
            idx[u] = k;
            vs[k] = u;
            dep[k++] = {d, k};
            for (auto [i, v, w] : g[u])
                if (v != p) {
                    ft.add(k, w);
                    eidx[i].first = k;
                    f(f, v, u, d + 1);
                    vs[k] = u;
                    ft.add(k, -w);
                    eidx[i].second = k;
                    dep[k++] = {d, k};
                }
        };
        dfs(dfs, root, -1, 0);
        st = ST(dep);
    }

    int lca(int u, int v) {
        auto [l, r] = minmax(idx[u], idx[v]);
        return vs[st.prod(l, r + 1).second];
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