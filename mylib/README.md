# mylib

競技プログラミング用の貼り付けライブラリ置き場。

## 公開とコンテストでの参照

公開先: [unidayo/comp の mylib](https://github.com/unidayo/comp/tree/main/mylib)

各実装の冒頭に参照URLを記載しています。新規作成・更新時は実装と関連ドキュメントを GitHub に commit / push し、コンテストで使う版を開始前に公開してください。ローカルの更新日時や commit の作成だけでは公開済みとはいえません。

コンテストでコピーする際は、開始前に公開済みのコミットを選び、GitHub のファイル画面で `y` キーを押して固定URLに切り替えます。コピー先の提出コードにも、その版のURLをコメントとして添えてください。`main` のURLは後から内容が変わるため、提出時はコミットSHAを含む固定URLを使います。依存ライブラリも、それぞれの参照URLを残してください。

```cpp
// 参照URL: https://github.com/unidayo/comp/blob/<公開済みコミットSHA>/mylib/heuristic/timer.cpp
```

URLを付けただけで未公開のコードを使用できるわけではありません。他者のコードについても、元の出典・ライセンス表記を保持してください。

## Categories

| Directory | Contents |
|---|---|
| `acl/` | AtCoder Library の展開済み/提出補助 |
| `accumulate/prefix_sum/` | 累積和 |
| `accumulate/imos/` | imos、長方形加算 |
| `algebra/number_theory/` | 素数判定、素因数分解、逆元、離散対数 |
| `algebra/linear_algebra/` | 行列、連立一次方程式、GF(2) |
| `algebra/bitwise_transform/` | 高速ゼータ変換、メビウス変換 |
| `algorithm/mo/` | Mo 系 |
| `algorithm/permutation/` | 転倒数、順列の辞書順 |
| `algorithm/optimization/` | slope trick |
| `algorithm/assignment/` | Hungarian |
| `combinatorics/` | 二項係数、階乗、Lucas theorem、写像12相 |
| `datastructure/segtree/` | segtree、lazy segtree、永続 segtree、beats |
| `datastructure/treap/` | Treap、Implicit Treap |
| `datastructure/range_query/` | Wavelet Matrix、Dynamic Wavelet Matrix、Cartesian Tree |
| `datastructure/set/` | set 系データ構造 |
| `datastructure/trie/` | Trie 系 |
| `datastructure/fenwick/` | Fenwick Tree 系 |
| `datastructure/union_find/` | Union-Find、重み付き Union-Find |
| `fps/` | formal power series、Bostan-Mori |
| `geometry/` | 幾何 |
| `graph/shortest_path/` | 最短路 |
| `graph/tree/` | 木 DP、HLD、Euler Tour、木パスクエリ |
| `graph/connectivity/` | LowLink、閉路検出 |
| `graph/coloring/` | 彩色数 |
| `graph/dag/` | トポロジカルソート |
| `heuristic/` | AHC 用の乱数、タイマー、探索補助 |
| `math/` | 数値計算寄りの小物 |
| `string/matching/` | KMP、Manacher、Rolling Hash |
| `string/automaton/` | Aho-Corasick、Suffix Automaton |
