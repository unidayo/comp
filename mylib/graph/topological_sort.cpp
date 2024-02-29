vector<int> topologiral_sort(vector<vector<int>> &g) {
    vector<int> indeg(ssize(g));
    for (auto u : views::iota(0, ssize(g))) {
        for (auto v : g[u]) {
            indeg[v]++;
        }
    }
    vector<int> ret;
    deque<int> S;
    for (auto i : views::iota(0, ssize(g))) {
        if (indeg[i] == 0) {
            S.push_back(i);
        }
    }
    while (!S.empty()) {
        int u = S.front();
        S.pop_front();
        ret.push_back(u);
        for (auto v : g[u]) {
            indeg[v]--;
            if (indeg[v] == 0) {
                S.push_back(v);
            }
        }
    }
    return ret;
}