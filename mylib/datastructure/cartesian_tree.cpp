/**
 * Cartesian Tree ライブラリ
 *
 * 配列 a から、次の 2 条件を満たす二分木を O(n) で構築する。
 *
 * 1. 中間順巡回をすると添字 0,1,2,...,n-1 の順に頂点が現れる
 * 2. 親子関係は値の大小で決まる
 *    - `Compare = less<T>`   : 親の値が子以上になる Max-Cartesian Tree
 *    - `Compare = greater<T>`: 親の値が子以下になる Min-Cartesian Tree
 *
 * 典型用途:
 * - 区間最大値 / 最小値の位置関係を木として扱いたい
 * - 「区間の max / min を根にして再帰する」分割を陽に持ちたい
 * - RMQ と LCA の対応を考えるときの補助構造
 *
 * 例:
 *   a = {3, 1, 5, 2, 4} で Max-Cartesian Tree を作ると
 *   - 根は値 5 の位置 `2`
 *   - 左部分木は区間 [0,2) = {3,1}
 *   - 右部分木は区間 [3,5) = {2,4}
 *
 *   実際の親子関係はこうなる:
 *     index:   0  1  2  3  4
 *     value:   3  1  5  2  4
 *     parent:  2  0 -1  4  2
 *     left:   -1 -1  0 -1  3
 *     right:   1 -1  4 -1 -1
 *
 *   木の形:
 *           2(=5)
 *          /     \
 *       0(=3)   4(=4)
 *         \      /
 *        1(=1) 3(=2)
 *
 * 使い方:
 *   vector<int> a = {3, 1, 5, 2, 4};
 *
 *   CartesianTree<int> ct(a);
 *   // Max-Cartesian Tree。値が大きい方が上に来る
 *
 *   CartesianTree<int, greater<>> ct2(a);
 *   // Min-Cartesian Tree。値が小さい方が上に来る
 *
 * メンバ:
 * - `root`
 *   根の添字。空配列なら `-1`
 * - `parent[i]`
 *   頂点 i の親。根なら `-1`
 * - `left[i]`, `right[i]`
 *   頂点 i の左右の子。存在しなければ `-1`
 * - `depth[i]`
 *   根からの深さ
 * - `subtree_size[i]`
 *   頂点 i を根とする部分木サイズ
 * - `children()`
 *   子だけ取りたいときの隣接リスト
 * - `max_depth()`
 *   木の最大深さ。空配列なら `-1`
 *
 * 注意:
 * - ノード番号は「木を作った順」ではなく、元の配列の添字そのもの
 * - 同値がある場合の形は `Compare` とスタック処理の都合で一意に決まる
 * - `parent / left / right` を直接見れば十分なことが多い
 */
template <typename T, typename Compare = std::less<T>>
struct CartesianTree {
    int n;
    int root;
    std::vector<int> parent, left, right, depth, subtree_size;

    // Compare = less<T>  → 大きい値が親 (Max-Cartesian Tree)
    // Compare = greater<T> → 小さい値が親 (Min-Cartesian Tree)
    CartesianTree() : n(0), root(-1) {}

    explicit CartesianTree(const std::vector<T>& a, Compare comp = Compare()) { build(a, comp); }

    // 最大深さを返す
    int max_depth() const {
        if (n == 0) return -1;
        return *std::max_element(depth.begin(), depth.end());
    }

    // 各ノードの子をリストとして取得（隣接リスト表現）
    std::vector<std::vector<int>> children() const {
        std::vector<std::vector<int>> ch(n);
        for (int i = 0; i < n; i++) {
            if (left[i] != -1) ch[i].push_back(left[i]);
            if (right[i] != -1) ch[i].push_back(right[i]);
        }
        return ch;
    }

   private:
    // O(n) 構築
    void build(const std::vector<T>& a, Compare comp) {
        n = (int)a.size();
        parent.assign(n, -1);
        left.assign(n, -1);
        right.assign(n, -1);
        depth.assign(n, 0);
        subtree_size.assign(n, 1);

        if (n == 0) {
            root = -1;
            return;
        }

        // スタックベースの O(n) 構築
        // comp(a[x], a[y]) == true は「a[x] が a[y] より優先度が低い（a[y]が親になる）」
        std::stack<int> stk;

        for (int i = 0; i < n; i++) {
            int last_popped = -1;

            while (!stk.empty() && comp(a[stk.top()], a[i])) {
                last_popped = stk.top();
                stk.pop();
            }

            if (last_popped != -1) {
                left[i] = last_popped;
                parent[last_popped] = i;
            }

            if (!stk.empty()) {
                right[stk.top()] = i;
                parent[i] = stk.top();
            }

            stk.push(i);
        }

        // 根を見つける
        root = -1;
        for (int i = 0; i < n; i++) {
            if (parent[i] == -1) {
                root = i;
                break;
            }
        }

        // 深さと部分木サイズを計算
        compute_depth_and_size();
    }

    void compute_depth_and_size() {
        std::vector<int> topo;
        topo.reserve(n);
        std::stack<int> stk;
        stk.push(root);
        while (!stk.empty()) {
            int v = stk.top();
            stk.pop();
            topo.push_back(v);
            if (right[v] != -1) stk.push(right[v]);
            if (left[v] != -1) stk.push(left[v]);
        }

        depth[root] = 0;
        for (int v : topo) {
            if (left[v] != -1) depth[left[v]] = depth[v] + 1;
            if (right[v] != -1) depth[right[v]] = depth[v] + 1;
        }

        for (int i = (int)topo.size() - 1; i >= 0; i--) {
            int v = topo[i];
            subtree_size[v] = 1;
            if (left[v] != -1) subtree_size[v] += subtree_size[left[v]];
            if (right[v] != -1) subtree_size[v] += subtree_size[right[v]];
        }
    }
};
