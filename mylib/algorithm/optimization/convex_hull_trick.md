# Convex Hull Trick

`convex_hull_trick.cpp` は、追加した直線 `a*x+b` の最小値または最大値を整数 `x` で求めます。傾きの追加順とクエリの `x` の順序に制約はありません。

```cpp
ConvexHullTrick<> cht;  // 最小値。最大値は ConvexHullTrick<false>
cht.add_line(2, 3);
cht.add_line(-1, 7);
long long y = cht.query(4);  // min(11, 3) = 3
```

追加は償却 `O(log N)`、クエリは `O(log N)`、空間は `O(N)` です。`clear()` で全直線を消せます。直線がない状態ではクエリできません。等しい傾きは自動的に有利な切片だけが残ります。

係数 `a, b` と座標 `x` は `long long` です。`query()` の答えも `long long` に収まる必要があります。より大きい答えには `query_wide()` を使い、戻り値を `__int128` として扱ってください。比較と直線の評価は内部で `__int128` を使うため、`long long` の乗算によるオーバーフローは起きません。

クエリは**整数座標**を対象にします。実数の交点や実数 `x` に対する答えを扱う用途には使えません。
