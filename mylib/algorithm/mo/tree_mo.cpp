// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/algorithm/mo/tree_mo.cpp
#include <bits/stdc++.h>
using namespace std;

/**
 * @brief 木上の Mo's Algorithm（頂点パス・辺パス）
 *
 * 静的な木に対するパスクエリをオフラインで処理する。
 * 頂点に要素がある場合は run、辺に要素がある場合は run_edges を使う。
 * run_edges の add / remove には add_edge が返した辺 ID が渡される。
 *
 * 【使い方】
 *   TreeMo mo(n);
 *   int eid = mo.add_edge(u, v);  // eid = 0, 1, ...
 *   int qid = mo.add_query(u, v); // 両端を含む単純パス
 *
 *   mo.run(add_vertex, remove_vertex, answer);
 *   // または
 *   mo.run_edges(add_edge, remove_edge, answer);
 *
 * build は run 内で root = 0 として自動的に行われる。
 * 根を指定したい場合だけ、run より前に mo.build(root) を呼ぶ。
 *
 * add / remove は、現在の集合への要素の追加・削除を表す。
 * answer(qid) が呼ばれた時点で、クエリ qid のパス上の要素だけが集合にある。
 * コールバック側の集合は run の開始時に空でなければならず、終了時には空に戻る。
 *
 * 頂点パスには u, v と LCA(u, v) を含む。
 * 辺パスには u-v パス上の辺だけを含み、u == v なら空集合になる。
 *
 * 【計算量】
 *   build: O(N log N)
 *   run  : O((N + Q) sqrt(Q) * T) 程度
 *          （T は add / remove 1 回の計算量。並べ方に依存する標準的な目安）
 *   memory: O(N log N + Q)
 *
 * 【注意】
 *   - 0-indexed、連結な木を仮定する。
 *   - 辺とクエリをすべて追加してから run を呼ぶ。
 *   - 値の更新を伴うクエリには対応しない。
 */
struct TreeMo {
    struct Edge {
        int to, id;
    };

    int n;
    vector<vector<Edge>> graph;
    vector<pair<int, int>> queries;
    int m = 0;

    vector<int> parent, parent_edge, depth, tin, tout, euler;
    vector<vector<int>> up;
    int root = -1;
    bool built = false;

    explicit TreeMo(int size)
        : n(size), graph(size), parent(size, -1), parent_edge(size, -1), depth(size), tin(size, -1), tout(size, -1) {
        assert(size > 0);
    }

    int add_edge(int u, int v) {
        assert(0 <= u && u < n && 0 <= v && v < n);
        int id = m++;
        graph[u].push_back({v, id});
        graph[v].push_back({u, id});
        built = false;
        return id;
    }

    int add_query(int u, int v) {
        assert(0 <= u && u < n && 0 <= v && v < n);
        int id = (int)queries.size();
        queries.emplace_back(u, v);
        return id;
    }

    int edge_count() const { return m; }

    int query_count() const { return (int)queries.size(); }

    void build(int new_root = 0) {
        assert(0 <= new_root && new_root < n);
        assert(edge_count() == n - 1);

        root = new_root;
        fill(parent.begin(), parent.end(), -2);
        fill(parent_edge.begin(), parent_edge.end(), -1);
        fill(depth.begin(), depth.end(), 0);
        fill(tin.begin(), tin.end(), -1);
        fill(tout.begin(), tout.end(), -1);
        euler.clear();
        euler.reserve(2 * n);

        parent[root] = -1;
        tin[root] = (int)euler.size();
        euler.push_back(root);

        vector<pair<int, int>> st;
        st.reserve(n);
        st.emplace_back(root, 0);
        int visited = 1;

        while (!st.empty()) {
            int v = st.back().first;
            int& next = st.back().second;

            if (next == (int)graph[v].size()) {
                tout[v] = (int)euler.size();
                euler.push_back(v);
                st.pop_back();
                continue;
            }

            auto [to, edge_id] = graph[v][next++];
            if (to == parent[v]) continue;
            assert(parent[to] == -2);  // 木でない入力（閉路または多重辺）
            parent[to] = v;
            parent_edge[to] = edge_id;
            depth[to] = depth[v] + 1;
            tin[to] = (int)euler.size();
            euler.push_back(to);
            ++visited;
            st.emplace_back(to, 0);
        }

        assert(visited == n);
        assert((int)euler.size() == 2 * n);

        int log = 1;
        while ((1LL << log) <= n) ++log;
        up.assign(log, vector<int>(n));
        for (int v = 0; v < n; ++v) up[0][v] = (parent[v] == -1 ? v : parent[v]);
        for (int k = 1; k < log; ++k) {
            for (int v = 0; v < n; ++v) up[k][v] = up[k - 1][up[k - 1][v]];
        }
        built = true;
    }

    bool is_ancestor(int u, int v) const {
        assert(built);
        return tin[u] <= tin[v] && tout[v] <= tout[u];
    }

    int lca(int u, int v) const {
        assert(built);
        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;
        for (int k = (int)up.size() - 1; k >= 0; --k) {
            if (!is_ancestor(up[k][u], v)) u = up[k][u];
        }
        return parent[u];
    }

    template <class Add, class Remove, class Answer>
    void run(Add add, Remove remove, Answer answer) {
        ensure_built();
        auto vertex_to_element = [](int v) { return v; };
        run_impl(false, add, remove, answer, vertex_to_element);
    }

    template <class Add, class Remove, class Answer>
    void run_vertices(Add add, Remove remove, Answer answer) {
        run(add, remove, answer);
    }

    template <class Add, class Remove, class Answer>
    void run_edges(Add add, Remove remove, Answer answer) {
        ensure_built();
        auto vertex_to_element = [&](int v) { return parent_edge[v]; };
        run_impl(true, add, remove, answer, vertex_to_element);
    }

   private:
    struct MoQuery {
        int l, r, lca, id;
        bool lca_is_endpoint_ancestor;
    };

    void ensure_built() {
        if (!built) build(root == -1 ? 0 : root);
    }

    template <class Add, class Remove, class Answer, class VertexToElement>
    void run_impl(bool edge_mode, Add& add, Remove& remove, Answer& answer,
                  VertexToElement& vertex_to_element) const {
        assert(built);
        int q = (int)queries.size();
        if (q == 0) return;

        vector<MoQuery> mo_queries;
        mo_queries.reserve(q);
        for (int id = 0; id < q; ++id) {
            auto [u, v] = queries[id];
            if (tin[u] > tin[v]) swap(u, v);
            int w = lca(u, v);
            bool ancestor_case = (w == u);
            if (ancestor_case) {
                mo_queries.push_back({tin[u], tin[v] + 1, w, id, true});
            } else {
                mo_queries.push_back({tout[u], tin[v] + 1, w, id, false});
            }
        }

        int block = max(1, (int)(euler.size() / sqrt((double)q)));
        sort(mo_queries.begin(), mo_queries.end(), [&](const MoQuery& a, const MoQuery& b) {
            int ab = a.l / block, bb = b.l / block;
            if (ab != bb) return ab < bb;
            return (ab & 1) ? a.r > b.r : a.r < b.r;
        });

        vector<char> active(n, false);
        auto toggle = [&](int v) {
            int element = vertex_to_element(v);
            if (active[v]) {
                if (element != -1) remove(element);
            } else {
                if (element != -1) add(element);
            }
            active[v] ^= 1;
        };

        int l = 0, r = 0;
        for (const MoQuery& query : mo_queries) {
            while (l > query.l) toggle(euler[--l]);
            while (r < query.r) toggle(euler[r++]);
            while (l < query.l) toggle(euler[l++]);
            while (r > query.r) toggle(euler[--r]);

            // 基本区間は、祖先ケースでは LCA を含み、それ以外では LCA を含まない。
            bool toggle_lca = edge_mode ? query.lca_is_endpoint_ancestor
                                        : !query.lca_is_endpoint_ancestor;
            if (toggle_lca) toggle(query.lca);
            answer(query.id);
            if (toggle_lca) toggle(query.lca);
        }

        // コールバック側の集合も空へ戻す。
        while (l < r) toggle(euler[l++]);
    }
};
