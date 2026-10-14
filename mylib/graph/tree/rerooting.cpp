// 最終更新: 2026-10-14 18:14:44 JST
// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/graph/tree/rerooting.cpp
/**
 * @brief 全方位木DP（ReRooting / Rerooting DP）
 *
 * 木の各頂点を根としたときの DP 値を O(N) で全頂点分求める。
 *
 * @tparam Monoid  辺/部分木の DP 値を表す型。
 *
 * 【使い方】
 *   1. Monoid 型を定義する（DP 値を保持する構造体など）
 *   2. 以下の 3 つを用意して構築する:
 *
 *      - identity : Monoid の単位元（子が 0 個のときの merge 結果）
 *      - merge(Monoid a, Monoid b) -> Monoid
 *            2 つの部分木の結果を統合する関数。
 *            結合律を満たす必要がある: merge(a, merge(b, c)) == merge(merge(a, b), c)
 *      - addnode(int v, Monoid x) -> Monoid
 *            全ての子部分木を merge で畳み込んだ結果 x に、
 *            頂点 v 自身の寄与を加えて返す関数。
 *            （葉の場合は x = identity で呼ばれる）
 *
 *   3. コンストラクタで自動的に build() が呼ばれ、全頂点分の DP が計算される。
 *   4. get(v) で「頂点 v を根としたときの DP 値」を取得する。
 *
 * 【計算量】
 *   構築: O(N)　／　クエリ: O(deg(v))（get 1 回あたり）
 *
 * 【使用例】
 *   // 部分木サイズの順列数を求める例 (ABC160-F)
 *   struct S { mint val; ll cnt; };
 *   S identity = {1, 0};
 *   auto merge = [&](S a, S b) -> S {
 *       return {a.val * b.val * C(a.cnt + b.cnt, a.cnt), a.cnt + b.cnt};
 *   };
 *   auto addnode = [&](int v, S x) -> S {
 *       return {x.val, x.cnt + 1};
 *   };
 *   auto rr = ReRooting<S>(G, identity, merge, addnode);
 *   for (int i = 0; i < N; i++) cout << rr.get(i).val << '\n';
 */
template <class Monoid>
struct ReRooting {
    using Graph = vector<vector<int>>;
    using MergeFunc = function<Monoid(Monoid, Monoid)>;
    using AddNodeFunc = function<Monoid(int, Monoid)>;

    // core member
    Graph G;
    Monoid IDENTITY;
    MergeFunc MERGE;
    AddNodeFunc ADDNODE;

    // inner data
    vector<vector<Monoid>> dp;

    // constructor
    ReRooting() {}
    ReRooting(const Graph &g, const Monoid &identity, const MergeFunc &merge, const AddNodeFunc &addnode) {
        G = g;
        IDENTITY = identity;
        MERGE = merge;
        ADDNODE = addnode;
        build();
    }

    // re-rooting dp
    Monoid rec(int v, int p) {
        Monoid res = IDENTITY;
        dp[v].assign(ssize(G[v]), IDENTITY);
        for (int i = 0; i < ssize(G[v]); ++i) {
            int v2 = G[v][i];
            if (v2 == p) continue;
            dp[v][i] = rec(v2, v);
            res = MERGE(res, dp[v][i]);
        }
        return ADDNODE(v, res);
    }
    void rerec(int v, int p, Monoid pval) {
        for (int i = 0; i < ssize(G[v]); ++i) {
            int v2 = G[v][i];
            if (v2 == p) {
                dp[v][i] = pval;
                continue;
            }
        }
        vector<Monoid> left(G[v].size() + 1, IDENTITY);
        vector<Monoid> right(G[v].size() + 1, IDENTITY);
        for (int i = 0; i < ssize(G[v]); ++i) {
            left[i + 1] = MERGE(left[i], dp[v][i]);
        }
        for (int i = ssize(G[v]) - 1; i >= 0; --i) {
            right[i] = MERGE(dp[v][i], right[i + 1]);
        }
        for (int i = 0; i < ssize(G[v]); ++i) {
            int v2 = G[v][i];
            if (v2 == p) continue;
            Monoid pval2 = MERGE(left[i], right[i + 1]);
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
    friend ostream &operator<<(ostream &os, const ReRooting<Monoid> &rr) {
        for (int v = 0; v < ssize(rr.G); ++v) {
            for (int i = 0; i < ssize(rr.G[v]); ++i) {
                os << v << " -> " << rr.G[v][i] << ": " << rr.dp[v][i] << endl;
            }
        }
        return os;
    }
};
