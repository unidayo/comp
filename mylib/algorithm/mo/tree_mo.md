# 木上の Mo's Algorithm

実装: `mylib/algorithm/mo/tree_mo.cpp`

静的な木のパスに対するクエリを、頂点列または辺列の追加・削除ができるデータ構造と組み合わせてオフライン処理する。

## 基本的な使い方

```cpp
TreeMo mo(n);
for (int i = 0; i < n - 1; ++i) {
    int u, v;
    cin >> u >> v;
    --u, --v;
    mo.add_edge(u, v);  // 返り値は 0-indexed の辺 ID
}

for (int i = 0; i < q; ++i) {
    int u, v;
    cin >> u >> v;
    --u, --v;
    mo.add_query(u, v);
}
```

### 頂点に要素がある場合

次は、各パスに現れる色の種類数を求める例である。

```cpp
vector<int> count_color(max_color + 1);
vector<int> answer(q);
int distinct = 0;

auto add = [&](int v) {
    if (count_color[color[v]]++ == 0) ++distinct;
};
auto remove = [&](int v) {
    if (--count_color[color[v]] == 0) --distinct;
};
auto output = [&](int query_id) {
    answer[query_id] = distinct;
};

mo.run(add, remove, output);
```

`u-v` パスの両端と LCA は、いずれも集合に含まれる。特に `u == v` のクエリでは頂点 `u` が 1 個だけ含まれる。

`run_vertices(add, remove, output)` も `run` の別名として使用できる。

### 辺に要素がある場合

`add_edge` が返した ID の順に辺の値を保存する。`run_edges` の `add` と `remove` には、その辺 ID が渡される。

```cpp
vector<long long> edge_value(n - 1);
vector<long long> answer(q);
long long sum = 0;

auto add = [&](int edge_id) {
    sum += edge_value[edge_id];
};
auto remove = [&](int edge_id) {
    sum -= edge_value[edge_id];
};
auto output = [&](int query_id) {
    answer[query_id] = sum;
};

mo.run_edges(add, remove, output);
```

`u-v` パス上の辺だけが集合に含まれる。LCA に入る親辺は含まれず、`u == v` のクエリでは集合は空になる。

## 仕組み

DFS の行きがけと帰りがけに各頂点を 1 回ずつ記録し、長さ `2N` の Euler tour を作る。Mo の現在区間を 1 要素伸縮するたび、記録された頂点の有効・無効を反転する。同じ頂点が区間内に 2 回現れれば打ち消し合うため、適切な区間を取るとパス上の頂点だけが残る。

- 一方が他方の祖先なら、基本区間は LCA を含む。
- それ以外なら、基本区間は LCA を含まない。
- 頂点クエリでは、後者だけ LCA を一時的に追加する。
- 辺クエリでは各辺を子側頂点に対応させ、前者だけ LCA を一時的に除く。

## 制約と注意

- 木は 0-indexed で、連結かつ辺数 `N-1` でなければならない。
- `build` は `run` / `run_edges` の初回呼び出し時に根を頂点 `0` として自動実行される。
- 根を別の頂点にしたい場合だけ、辺をすべて追加した後、`run` より前に `build(root)` を明示的に呼ぶ。
- `add` と `remove` は互いに逆操作として実装する。
- コールバック側の集合は `run` 開始時に空にする。`run` 終了時にはライブラリが空へ戻す。
- クエリ途中の値更新には対応しない。
- LCA 前処理は `O(N log N)`、追加メモリは `O(N log N + Q)`。Mo 部分の計算量は並べ方にも依存するが、標準的には `O((N+Q)sqrt(Q) * T)` 程度。`T` は 1 回の追加・削除にかかる時間。
