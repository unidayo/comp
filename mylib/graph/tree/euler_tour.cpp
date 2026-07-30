// 最終更新: 2026-07-12 14:27:44 JST
/**
 * @brief Euler Tour for vertices of a tree
 *
 * order       : first-visit order. subtree(v) is [in[v], out[v]).
 * euler       : DFS walk vertices, length 2 * N - 1 for a connected tree.
 * first[v]    : first index of v in euler.
 * parent[root]: -1.
 */
struct EulerTour {
    int n;
    vector<vector<int>> G;
    vector<int> parent, depth, in, out, sz, order, euler, first;

    EulerTour(int n) : n(n), G(n), parent(n, -1), depth(n), in(n, -1), out(n, -1), sz(n), first(n, -1) {}

    EulerTour(const vector<vector<int>> &g) : n((int)g.size()), G(g), parent(n, -1), depth(n), in(n, -1), out(n, -1), sz(n), first(n, -1) {}

    void add_edge(int u, int v) {
        G[u].push_back(v);
        G[v].push_back(u);
    }

    void build(int root = 0) {
        reset();
        dfs(root, -1, 0);
    }

    void build_all() {
        reset();
        for (int root = 0; root < n; ++root) {
            if (in[root] == -1) dfs(root, -1, 0);
        }
    }

    pair<int, int> subtree(int v) const { return {in[v], out[v]}; }

    bool is_ancestor(int u, int v) const {
        return in[u] <= in[v] && out[v] <= out[u];
    }

   private:
    void reset() {
        fill(parent.begin(), parent.end(), -1);
        fill(depth.begin(), depth.end(), 0);
        fill(in.begin(), in.end(), -1);
        fill(out.begin(), out.end(), -1);
        fill(sz.begin(), sz.end(), 0);
        fill(first.begin(), first.end(), -1);
        order.clear();
        euler.clear();
    }

    void dfs(int root, int p, int d) {
        parent[root] = p;
        depth[root] = d;
        in[root] = (int)order.size();
        first[root] = (int)euler.size();
        order.push_back(root);
        euler.push_back(root);

        vector<pair<int, int>> st;
        st.emplace_back(root, 0);
        while (!st.empty()) {
            int v = st.back().first;
            int &i = st.back().second;

            if (i == (int)G[v].size()) {
                out[v] = (int)order.size();
                sz[v] = out[v] - in[v];
                st.pop_back();
                if (!st.empty()) euler.push_back(st.back().first);
                continue;
            }

            int to = G[v][i++];
            if (to == parent[v]) continue;
            parent[to] = v;
            depth[to] = depth[v] + 1;
            in[to] = (int)order.size();
            first[to] = (int)euler.size();
            order.push_back(to);
            euler.push_back(to);
            st.emplace_back(to, 0);
        }
    }
};
