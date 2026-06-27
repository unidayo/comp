/**
 * @brief 全方位木DP（辺情報あり版）（ReRooting / Rerooting DP with Edge Info）
 *
 * 辺に情報（重みなど）がある木の各頂点を根としたときの DP 値を O(N) で全頂点分求める。
 *
 * @tparam Monoid   辺/部分木の DP 値を表す型。
 * @tparam EdgeInfo 辺の情報を表す型（重みなど）。
 *
 * 【使い方】
 *   1. Monoid 型・EdgeInfo 型を定義する
 *   2. グラフを vector<vector<pair<int, EdgeInfo>>> で構築する
 *   3. 以下の 4 つを用意して構築する:
 *
 *      - identity : Monoid の単位元（子が 0 個のときの merge 結果）
 *      - merge(Monoid a, Monoid b) -> Monoid
 *            2 つの部分木の結果を統合する関数。
 *            結合律を満たす必要がある: merge(a, merge(b, c)) == merge(merge(a, b), c)
 *      - addedge(int v_src, EdgeInfo e, Monoid x) -> Monoid
 *            隣接頂点 v_src 側の部分木の値 x に、辺 e を渡る寄与を加えて返す関数。
 *            v_src は「値が来る側の頂点」（子なら子番号、親方向なら親番号）。
 *      - addnode(int v, Monoid x) -> Monoid
 *            全ての隣接方向の値を merge で畳み込んだ結果 x に、
 *            頂点 v 自身の寄与を加えて返す関数。
 *            （葉の場合は x = identity で呼ばれる）
 *
 *   4. コンストラクタで自動的に build() が呼ばれ、全頂点分の DP が計算される。
 *   5. get(v) で「頂点 v を根としたときの DP 値」を取得する。
 *
 * 【dp[v][i] の意味】
 *   頂点 v の i 番目の隣接方向からの寄与値（addedge 適用済み）。
 *   すなわち dp[v][i] = addedge(v_i, e_i, addnode(v_i, ...)) の形。
 *   get(v) = addnode(v, merge(dp[v][0], dp[v][1], ...))
 *
 * 【計算量】
 *   構築: O(N)　／　クエリ: O(deg(v))（get 1 回あたり）
 *
 * 【使用例】
 *   // 辺の重みつき最長パスを全頂点について求める例
 *   using S = ll;
 *   S identity = 0;
 *   auto merge   = [](S a, S b) { return max(a, b); };
 *   auto addedge = [](int, ll w, S x) { return x + w; };
 *   auto addnode = [](int, S x) { return x; };
 *   auto rr = ReRootingEdge<S, ll>(G, identity, merge, addedge, addnode);
 *   for (int i = 0; i < N; i++) cout << rr.get(i) << '\n';
 */
template <class Monoid, class EdgeInfo>
struct ReRootingEdge {
    using Edge = pair<int, EdgeInfo>;
    using Graph = vector<vector<Edge>>;
    using MergeFunc = function<Monoid(Monoid, Monoid)>;
    using AddEdgeFunc = function<Monoid(int, EdgeInfo, Monoid)>;
    using AddNodeFunc = function<Monoid(int, Monoid)>;

    // core member
    Graph G;
    Monoid IDENTITY;
    MergeFunc MERGE;
    AddEdgeFunc ADDEDGE;
    AddNodeFunc ADDNODE;

    // inner data
    vector<vector<Monoid>> dp;

    // constructor
    ReRootingEdge() {}
    ReRootingEdge(const Graph &g, const Monoid &identity,
                  const MergeFunc &merge, const AddEdgeFunc &addedge, const AddNodeFunc &addnode) {
        G = g;
        IDENTITY = identity;
        MERGE = merge;
        ADDEDGE = addedge;
        ADDNODE = addnode;
        build();
    }

    // re-rooting dp
    Monoid rec(int v, int p) {
        Monoid res = IDENTITY;
        dp[v].assign(ssize(G[v]), IDENTITY);
        for (int i = 0; i < ssize(G[v]); ++i) {
            auto [v2, e] = G[v][i];
            if (v2 == p) continue;
            dp[v][i] = ADDEDGE(v2, e, rec(v2, v));
            res = MERGE(res, dp[v][i]);
        }
        return ADDNODE(v, res);
    }
    void rerec(int v, int p, Monoid pval) {
        for (int i = 0; i < ssize(G[v]); ++i) {
            auto [v2, e] = G[v][i];
            if (v2 == p) {
                dp[v][i] = ADDEDGE(p, e, pval);
                continue;
            }
        }
        vector<Monoid> left(G[v].size() + 1, IDENTITY);
        vector<Monoid> right(G[v].size() + 1, IDENTITY);
        for (int i = 0; i < ssize(G[v]); ++i) {
            left[i + 1] = MERGE(left[i], dp[v][i]);
            right[i + 1] = MERGE(right[i], dp[v][ssize(G[v]) - i - 1]);
        }
        for (int i = 0; i < ssize(G[v]); ++i) {
            auto [v2, e] = G[v][i];
            if (v2 == p) continue;
            Monoid pval2 = MERGE(left[i], right[ssize(G[v]) - i - 1]);
            rerec(v2, v, ADDNODE(v, pval2));
        }
    }
    void build() {
        dp.assign(ssize(G), vector<Monoid>());
        int root = 0, nullparent = -1;
        rec(root, nullparent);
        rerec(root, nullparent, IDENTITY);
    }

    // getter
    Monoid get(int v) {
        Monoid res = IDENTITY;
        for (int i = 0; i < ssize(G[v]); ++i) {
            res = MERGE(res, dp[v][i]);
        }
        return ADDNODE(v, res);
    }

    // dump
    friend ostream &operator<<(ostream &os, const ReRootingEdge<Monoid, EdgeInfo> &rr) {
        for (int v = 0; v < ssize(rr.G); ++v) {
            for (int i = 0; i < ssize(rr.G[v]); ++i) {
                os << v << " -> " << rr.G[v][i].first << ": " << rr.dp[v][i] << endl;
            }
        }
        return os;
    }
};
