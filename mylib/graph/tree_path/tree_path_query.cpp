/* 
重みつき木の2頂点u, vの距離を求める
辺の重みを変更する
2頂点のLCAを求める
*/

struct Edge {
    /* 
    id: 辺のid
    to: 辺の行き先
    weight: 辺の重み
    */
    int id, to;
    ll weight;
};
using Graph = vector<vector<Edge>>;

class treePathQuery {
    using seg_lca =
        segtree<pair<int, int>,
                [](pair<int, int> l, pair<int, int> r) { return min(l, r); },
                []() -> pair<int, int> {
                    return {1 << 30, 0};
                }>;

   private:
    int _n;
    vector<int> vertex_to_index, vs;
    vector<ll> dist;
    vector<pair<int, int>> depth, edge_to_index;
    fenwick_tree<ll> ft;
    seg_lca depth_st;

    void dfs(Graph &g, int u, int p, int d, int &k) {
        vertex_to_index[u] = k;
        vs[k] = u;
        depth[k] = {d, k};
        k++;
        for (auto [id, v, w] : g[u]) {
            if (v != p) {
                ft.add(k, w);
                edge_to_index[id].first = k;
                dfs(g, v, u, d + 1, k);
                vs[k] = u;
                ft.add(k, -w);
                edge_to_index[id].second = k;
                depth[k] = {d, k};
                k++;
            }
        }
    }

   public:
    explicit LCA(int n, vector<vector<Edge>> &g, int root = 0)
        : vertex_to_index(n),
          vs(2 * n - 1),
          depth(2 * n - 1),
          edge_to_index(n - 1),
          ft(2 * n - 1) {
        int k = 0;
        dfs(g, root, -1, 0, k);
        depth_st = seg_lca(depth);
    }

    int get_lca(int u, int v) {
        auto [x, y] = minmax(vertex_to_index[u], vertex_to_index[v]);
        return vs[depth_st.prod(x, y + 1).second];
    }

    ll get_dist(int u, int v) {
        int p = get_lca(u, v);
        return ft.sum(0, vertex_to_index[u] + 1) +
               ft.sum(0, vertex_to_index[v] + 1) -
               2 * ft.sum(0, vertex_to_index[p] + 1);
    }

    void change_cost(int edge_id, ll new_cost) {
        auto [fi, se] = edge_to_index[edge_id];
        ll cost_before = ft.sum(fi, fi + 1);
        ft.add(fi, new_cost - cost_before);
        ft.add(se, -new_cost + cost_before);
    }
};