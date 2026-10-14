# 抽象化 Segment Tree Beats

本体: [abstract_segtree_beats.cpp](abstract_segtree_beats.cpp)

ACL の `lazy_segtree` に近い形で、区間情報 `S`、更新 `F`、
`op / e / mapping / composition / id` を外から渡す。
`mapping` がその節点の情報だけで更新できない場合に、子へ降りて更新を続ける。
本体は独立して貼り付けられ、ACL に依存しない。C++17 以降で使用可能。

## 貼り付けて使う

本体を先に貼り、その後で以下のいずれか1種類を貼る。

- [range chmin / range sum](beats/range_chmin_range_sum.cpp)
- [range chmax / range sum](beats/range_chmax_range_sum.cpp)

これらの定義は `std::optional<S>` を返すため、通常の ACL `lazy_segtree` には渡せない。
既存の `lazysegtree/` の定義は引き続き ACL 用。
以下は chmin / sum の例。chmax / sum も同じ呼び出し方で使える。

```cpp
std::vector<long long> a = {5, 1, 8, 3};
std::vector<S> init;
for (auto x : a) init.push_back(make_leaf(x));

AbstractSegTreeBeats<S, op, e, F, mapping, composition, id> seg(init);
seg.apply(0, 3, 4);          // chmin: {4, 1, 4, 3}
auto sum = seg.prod(0, 4).sum;  // 12
auto mx = seg.prod(0, 4).max_value;  // 4
seg.set(1, make_leaf(10));   // 点代入: {4, 10, 4, 3}
auto value = seg.get(1).sum; // 10
```

chmax / sum の定義では、最小値を `.min_value` で取得できる。
2つの定義は別々のテンプレートであり、同じ木で chmin と chmax を混在させる定義ではない。

区間はすべて 0-indexed の半開区間 `[l, r)`。
`seg(n)` は ACL と同様に `vector<S>(n, e())` から構築する。
これらの具体例では `e()` は「長さ0」なので、実際の要素は `make_leaf` で初期化する。
ゼロ埋めなら `vector<S>(n, make_leaf(0))` を渡す。

## インターフェース

| 操作 | 意味 |
|---|---|
| `size()` | 配列の長さ |
| `set(p, x)` | 葉 `p` に `S` を代入 |
| `get(p)` | 葉 `p` の `S` を取得 |
| `prod(l, r)` | 区間を左から順に `op` で集約 |
| `all_prod()` | 全体の集約。`const`、`O(1)` |
| `apply(p, f)` | 1要素に更新を適用 |
| `apply(l, r, f)` | 区間に更新を適用 |
| `max_right(l, g)` | `g(prod(l, r))` を満たす最大の `r` |
| `min_left(r, g)` | `g(prod(l, r))` を満たす最小の `l` |

`max_right` / `min_left` は ACL と同様に `g(e()) == true` が必要で、
区間を伸ばすと判定が true から false に一度だけ変わる条件で使う。
例えば負数を含む配列の総和に対する `sum <= limit` は通常この条件を満たさない。
関数ポインタ形式の `max_right<g>(l)` / `min_left<g>(r)` も使える。

## 定義すべき条件

- `op` は結合的で、`e()` は左右の単位元。可換性は不要。
- `mapping(F f, S x)` は `std::optional<S>` を返す。
  実際の要素に `f` を適用した集約を正しく求められるならその値を返し、
  情報不足なら `std::nullopt` を返す。失敗時に外部状態を書き換えない。
- 葉、空区間 `e()`、恒等作用 `id()` に対する `mapping` は必ず成功する。
  葉で失敗すると `std::logic_error` を送出する。
- `composition(f, g)` は **g を先に、f を後に適用する**更新を表す。
  `id()` は更新の恒等元。`F` に等値比較は不要。
- 更新の意味と集約の意味が整合し、分割して更新後に `op` でまとめても同じ結果になる必要がある。
  ただし、情報不足で失敗する場合があるため、ACL のように常に
  `mapping(f, op(a, b))` が直接計算できる必要はない。

本体は成功した更新を遅延情報に合成する。
失敗時は以前の遅延情報を子へ伝え、今回の更新を両方の子に適用し、親を再集約する。
子に合成済みの遅延情報を伝える際も、`mapping` が失敗したらさらに降りる。

## 具体例の状態と計算量

chmin / sum は最大値、厳密な第二最大値、最大値の個数、総和、長さを保持する。
`第二最大値 < 閾値 < 最大値` なら最大値の要素だけが変わるため、
`sum += (閾値 - 最大値) * 最大値の個数` で成功できる。
閾値が第二最大値以下なら子へ降りる。等号でも値のグループが合流するため失敗させる。
chmax / sum は最小値側で対称に処理する。

要素値・更新の閾値は `(-BEATS_INF, BEATS_INF)` 内に収める。
総和、差、差と個数の積などの中間値も `long long` 内に収める。

本体の構築・メモリは、`S` と `F` が定数サイズなら `O(n)`。
**抽象化した本体だけでは、更新や取得の `O(log n)` は保証できない。**
失敗が続く1回の区間更新は全葉まで降りる可能性がある。
遅延情報の伝播中にも失敗を処理するため、取得や点代入にも追加の探索が発生し得る。
失敗の回数を抑えられることを具体的な状態・作用ごとに証明する必要がある。

同梱した単方向の chmin / sum または chmax / sum では、構築後の
区間更新・点代入・取得など計 `q` 回の操作を合わせて償却 `O((n + q) log(n + 1))`。
第二最大/最小値を跨ぐ失敗は異なる値の種類を減らし、区間境界や点代入が増やせる種類数は
1操作あたり `O(log n)` に抑えられる。区間取得・点代入・二分探索自体は `O(log n)`。
この評価を、別の `mapping` や chmin/chmax の混在へそのまま適用しない。
