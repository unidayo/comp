class LowLink {
  private:
    vector<int> seen, ord, low;
    const Graph &G;
    int dfs(int u, int p, int &k);

  public:
    vector<int> articulations;
    vector<pair<int, int>> bridges;
    void build();
    LowLink(const Graph &G);
};

LowLink::LowLink(const Graph &g) : G(g) {
    seen.assign(ssize(g), 0);
    ord.assign(ssize(g), 0);
    low.assign(ssize(g), 0);
}

int LowLink::dfs(int u, int p, int &k) {
    seen[u] = true;
    ord[u] = k++;
    low[u] = ord[u];
    int cnt = 0;
    for (auto &e : G[u]) {
        if (!seen[e.to]) {
            cnt++;
            k = dfs(e.to, u, k);
            low[u] = min(low[u], low[e.to]);
            if (ord[u] <= low[e.to]) {
                articulations.push_back(u);
            }
            if (ord[u] < low[e.to])
                bridges.emplace_back(minmax(u, e.to));
        } else if (e.to != p) {
            low[u] = min(low[u], ord[e.to]);
        }
    }
    if (p == -1 && cnt >= 2)
        articulations.push_back(u);
    return k;
}

void LowLink::build() {
    int k = 0;
    for (int u : views::iota(0, ssize(G))) {
        if (!seen[u]) {
            k = dfs(u, -1, k);
        }
    }
}