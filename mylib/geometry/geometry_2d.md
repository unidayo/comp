# 2 次元幾何ライブラリ

`geometry_2d.cpp` は、競技プログラミングで頻出する 2 次元幾何をまとめた貼り付け用ライブラリである。

## 基本方針

- すべて `geometry2d` 名前空間に入っている。
- 整数座標では `dot`、`cross`、`orientation`、線分交差判定、面積の 2 倍を `__int128_t` で計算する。
- 距離、射影、回転、交点は `long double`（`Real`）を返す。
- 浮動小数点座標の判定には `EPS = 1e-12` を使う。入力のスケールに対してこの絶対誤差が不適切なら変更する。
- 浮動小数点の `parallel` と `orthogonal` は、方向ベクトルを最大成分でスケーリングしてから相対誤差で判定する。
- 浮動小数点の `orientation` は、スケーリング後の外積を構成する 2 つの積の大きさに対する相対誤差で判定する。
- 浮動小数点の `on_line` と `on_segment` は、垂線距離が `EPS` 以下かで判定する。
- 整数の積が `__int128_t` の範囲を超える座標は扱えない。

```cpp
#include "mylib/geometry/geometry_2d.cpp"
using namespace geometry2d;

using P = Point<long long>;
P a{0, 0}, b{4, 0}, c{2, 3};

int turn = orientation(a, b, c); // +1: 反時計回り
auto twice_area = cross(a, b, c); // __int128_t
bool hit = intersects(Segment<long long>{a, c},
                      Segment<long long>{P{0, 2}, P{4, 2}});
```

提出時は通常どおり `.cpp` の内容を解答へ貼り付ける。

## 点と基本演算

`Point<T>` は `+`, `-`, スカラー倍・除算、辞書順比較、入出力を持つ。

| 関数 | 内容 |
|---|---|
| `dot(a, b)` | 内積 |
| `cross(a, b)` | 外積 |
| `cross(o, a, b)` | `(a-o) x (b-o)` |
| `norm2(p)` | 長さの 2 乗 |
| `norm(p)` | 長さ |
| `distance(a, b)` | 2 点間距離 |
| `orientation(a, b, c)` | 左折 `+1`、右折 `-1`、一直線 `0` |
| `rotate(p, theta)` | 原点中心にラジアンで回転 |
| `rotate(p, center, theta)` | 指定点中心に回転 |
| `same_point(a, b)` | 2 点の等価判定 |
| `angle_between_vectors(u, v)` | 2 ベクトルの角 `[0, PI]` |
| `angle(a, b, c)` | `b` を頂点とする `∠abc` |

`Point<long long>` 同士の除算は整数除算になる。正規化や交点計算には `Point<Real>`、またはライブラリが返す `Point<Real>` を使う。

`Point<T>` の `operator==` は座標の完全一致である。計算結果の `Point<Real>` を比較するときは、各座標を `EPS` で比較する `same_point` を使う。整数座標に対する `same_point` は完全一致判定になる。

角度の単位はラジアンである。`angle(a,b,c)` は `a-b` と `c-b` の角、つまり中央の `b` を頂点とする角を返す。角度を定義できないゼロベクトルを含むと `nullopt` を返す。度数法が必要なら `theta * 180 / PI` で変換できる。

## 直線と線分

`Line<T>{a,b}` は 2 点を通る無限直線、`Segment<T>{a,b}` は両端を含む線分を表す。数学的な意味と関数の誤用を区別するため、両者は継承関係のない独立した構造体である。

線分の両端を通る無限直線が必要な場合だけ、`supporting_line(segment)` で明示的に `Line<T>` へ変換する。退化線分を変換すると退化直線になるため、変換後に直線用関数を使う場合は事前に確認する。

`ax + by + c = 0` 形式とは次のように相互変換できる。

```cpp
// 2x + 4y - 6 = 0 から直線を作る。戻り値は Line<Real>
auto line = line_from_equation(2LL, 4LL, -6LL);

// 2 点形式から係数を得る
auto equation = line_equation(Line<long long>{{1, 2}, {3, 4}});
auto [a, b, c] = equation;
```

`line_equation` が返す係数は正規化しない。係数全体の 0 でない定数倍は同じ直線を表す。整数座標の `Line<T>` から得る係数型は `__int128_t` である。

`line_from_equation` は、元の係数が整数でも直線上の点が整数になるとは限らないため `Line<Real>` を返す。`a = b = 0` は直線を表さないため使用できない。係数は内部で `max(|a|,|b|)` により正規化するため、非ゼロなら非常に小さい係数も扱える。

| 関数 | 内容 |
|---|---|
| `line_from_equation(a,b,c)` | `ax+by+c=0` から `Line<Real>` を作る |
| `line_equation(l)` | 2 点形式から `LineEquation{a,b,c}` を得る |
| `line_from_point_direction(p,d)` | 点 `p` を通り方向 `d` と平行な直線 |
| `supporting_line(s)` | 線分の両端を通る無限直線 |
| `same_line(l, m)` | 同じ無限直線を表すか |
| `l == m` | `same_line(l,m)` と同じ直線比較 |
| `same_segment(s, t)` | 端点の順序を無視した線分比較 |
| `s == t` | `same_segment(s,t)` と同じ線分比較 |
| `parallel(l, m)` | 平行判定 |
| `orthogonal(l, m)` | 直交判定 |
| `angle_between_lines(l, m)` | 直線同士の小さい方の角 `[0, PI/2]` |
| `angle_between_segments(s, t)` | 線分の方向ベクトル同士の角 `[0, PI]` |
| `perpendicular_bisector(a, b)` | 2 点の垂直二等分線 |
| `on_line(l, p)` | 直線上判定 |
| `on_segment(s, p)` | 閉線分上判定 |
| `projection(l, p)` | 直線への射影 |
| `reflection(l, p)` | 直線に関する鏡映 |
| `distance(l, p)` | 直線と点の距離 |
| `distance_to_segment(s, p)` | 線分と点の距離 |
| `distance(s, t)` | 線分間距離 |
| `intersects(s, t)` | 端点・重なりを含む線分交差 |
| `properly_intersects(s, t)` | 両線分の内部で 1 点交差するか |
| `line_intersection(l, m)` | 一意な直線交点。平行・同一直線は `nullopt` |
| `intersects(l, s)` | 直線と線分が交わるか |
| `intersections(l, s)` | 直線と線分の共通部分を 0/1/2 点で表す |
| `segment_intersection(s, t)` | 交点を 0/1 個、重なり区間なら両端 2 個返す |

退化した直線（`a == b`）に `same_line`、`parallel`、`orthogonal`、`projection`、直線と点の `distance`、円との交点を渡してはいけない。長さ 0 の線分は線分用関数で扱える。

`Line<T>` の `operator==` は、保持する 2 点の一致ではなく同じ無限直線かを判定する。`Segment<T>` の `operator==` は端点の順序を無視する。どちらも整数では厳密判定、浮動小数点では `EPS` による近似判定になる。そのため、浮動小数点版の `==` は厳密な同値関係として `set` のキーやハッシュに使用しない。

`angle_between_lines` は直線に向きがないものとして、平行なら `0`、直交なら `PI/2` となる小さい方の角を返す。`angle_between_segments` は各線分を `a` から `b` へのベクトルとして扱うため、端点を入れ替えると `theta` と `PI-theta` が入れ替わる。退化直線・退化線分に対する角は `nullopt` になる。

`parallel` と `orthogonal` は、整数座標なら座標差を `__int128_t` へ変換してから外積・内積を計算し、差を元の整数型で先に計算することによるオーバーフローを避ける。浮動小数点なら各方向ベクトルを最大成分で割り、成分の絶対値が高々 1 の状態で次を判定する。

```cpp
abs(cross(u, v)) <= EPS * norm(u) * norm(v); // parallel
abs(dot(u, v))   <= EPS * norm(u) * norm(v); // orthogonal
```

これにより、座標の大きさによるオーバーフロー・アンダーフローを避けつつ、角度に対する誤差として `EPS` を使う。

`perpendicular_bisector(a,b)` は `a` と `b` の中点を通り、`b-a` に垂直な `Line<Real>` を返す。`a == b` のときは 2 点から等距離な点が平面全体になり、一意な直線として表せないため `nullopt` を返す。

```cpp
auto bisector = perpendicular_bisector(Point<long long>{0, 0},
                                        Point<long long>{4, 0});
// *bisector は x = 2
```

## 三角形

| 関数 | 内容 |
|---|---|
| `degenerate_triangle(a,b,c)` | 3 点が三角形を作らないか |
| `incenter(a,b,c)` | 内心を `optional<Point<Real>>` で返す |
| `circumcenter(a,b,c)` | 外心を `optional<Point<Real>>` で返す |

内心は 3 辺から等距離にある内角の二等分線の交点、外心は 3 頂点から等距離にある辺の垂直二等分線の交点である。座標は一般に整数にならないため `Point<Real>` を返す。3 点が一直線上、または同一点を含む場合は退化三角形として `nullopt` を返す。

```cpp
using P = Point<long long>;
auto i = incenter(P{0, 0}, P{4, 0}, P{0, 3});
auto o = circumcenter(P{0, 0}, P{4, 0}, P{0, 3});

// *i == (1, 1), *o == (2, 1.5)
```

## 円

円は常に `Circle{Point<Real>{x,y}, radius}` で表す。半径は 0 以上である必要がある。交点関数は半径 0 の点円も扱うが、接線関数は半径が正の円だけを対象とする。

- `intersections(Line<Real>, Circle)` は直線と円の交点を 0～2 個返す。
- `intersections(Segment<T>, Circle)` は線分と円周の交点を 0～2 個返す。
- `intersects(Segment<T>, Circle)` は線分が円周と共通点を持つか返す。
- `intersections(Circle, Circle)` は 2 円の交点を 0～2 個返す。
- `tangent_lines(Circle, Point<T>)` は指定点を通る円の接線を 0～2 本返す。
- `tangent_lines(Circle, Circle)` は 2 円の共通接線を 0～4 本返す。
- 接する場合は同じ点を重複させず 1 個だけ返す。
- 同心円は、交点なし・無限個の区別を戻り値で表せないため、どちらも空配列を返す。必要なら呼び出し側で中心と半径を比較する。

線分と円の `intersects` は「円周と交わるか」を判定する。線分全体が円の内部にあり、円周へ触れない場合は `false` になる。退化線分は、その 1 点が円周上なら交点 1 個として扱う。

`tangent_lines(c,p)` は `p` が円の内部なら 0 本、円周上なら 1 本、外部なら 2 本返す。`tangent_lines(a,b)` が返す共通接線数は円の位置関係で 0～4 本になる。同心同半径の場合は共通接線が無限本で戻り値に表せないため空配列を返す。接線の定義が退化する半径 0 の円は対象外である。

```cpp
Circle c{{0, 0}, 1};
auto from_point = tangent_lines(c, Point<long long>{2, 0}); // 2 本
auto common = tangent_lines(c, Circle{{4, 0}, 1});           // 4 本
```

## 多角形と凸包

| 関数 | 内容 |
|---|---|
| `signed_area2(poly)` | 符号付き面積の 2 倍。反時計回りなら正 |
| `area(poly)` | 面積を `Real` で返す |
| `contains(poly, p)` | 単純多角形に対して `Outside/Boundary/Inside` |
| `convex_hull(points, keep_collinear)` | 凸包。反時計回りで辞書順最小点から開始 |

`convex_hull` は入力を値渡しするため、呼び出し元の点列を変更しない。既定では辺上の不要な点を除き、`keep_collinear=true` なら辺上の全点を残す。全点が一直線上なら、既定では両端だけを返す。

`contains` は自己交差しない単純多角形を前提とし、頂点列の時計回り・反時計回りは問わない。
