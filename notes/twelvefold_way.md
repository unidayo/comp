# 写像12相

`n` 個の玉を `k` 個の箱へ入れる数え上げを、玉と箱が区別できるか、箱への個数制約が何かで分類する。
競技中は「玉が区別ありか」「箱が区別ありか」「空箱を許すか」を先に固定すると誤用しにくい。

ワークスペースでは `mylib/combinatorics/twelvefold_way/` に 12 相を 1 ファイルずつ置いている。
各ファイルは `template <class mint>` の単体関数で、`modint998244353` など割り算可能な modint を想定する。

## 1. 用語

- 区別あり玉: `1,2,...,n` とラベルが付いた玉。関数の `dist_balls`。
- 区別なし玉: 同じ玉が `n` 個ある。関数の `same_balls`。
- 区別あり箱: `1,2,...,k` とラベルが付いた箱。関数の `dist_boxes`。
- 区別なし箱: 箱の並び替えを同一視する。関数の `same_boxes`。
- 制約なし: 空箱を許す。関数の `any`。
- 高々 1 個: 各箱に 0 個または 1 個。関数の `le_one`。
- 少なくとも 1 個: 空箱を許さない。関数の `ge_one`。

ここで「区別なし箱」は、箱を並び替えて一致する分配を同じものとみなす。例えば区別なし玉 3 個を区別なし箱 2 個へ制約なしで入れる方法は、`3+0` と `2+1` の 2 通り。

## 2. 12 相の表

| 玉 | 箱 | 制約 | 答え | ライブラリ |
|---|---|---|---|---|
| 区別あり | 区別あり | 制約なし | $k^n$ | `dist_balls_dist_boxes_any` |
| 区別あり | 区別あり | 高々 1 個 | ${}_kP_n$ | `dist_balls_dist_boxes_le_one` |
| 区別あり | 区別あり | 少なくとも 1 個 | $k!\,S(n,k)$ | `dist_balls_dist_boxes_ge_one` |
| 区別なし | 区別あり | 制約なし | $\binom{n+k-1}{k-1}$ | `same_balls_dist_boxes_any` |
| 区別なし | 区別あり | 高々 1 個 | $\binom{k}{n}$ | `same_balls_dist_boxes_le_one` |
| 区別なし | 区別あり | 少なくとも 1 個 | $\binom{n-1}{k-1}$ | `same_balls_dist_boxes_ge_one` |
| 区別あり | 区別なし | 制約なし | $\sum_{i=0}^{k} S(n,i)$ | `dist_balls_same_boxes_any` |
| 区別あり | 区別なし | 高々 1 個 | $[n\le k]$ | `dist_balls_same_boxes_le_one` |
| 区別あり | 区別なし | 少なくとも 1 個 | $S(n,k)$ | `dist_balls_same_boxes_ge_one` |
| 区別なし | 区別なし | 制約なし | $p_{\le k}(n)$ | `same_balls_same_boxes_any` |
| 区別なし | 区別なし | 高々 1 個 | $[n\le k]$ | `same_balls_same_boxes_le_one` |
| 区別なし | 区別なし | 少なくとも 1 個 | $p_k(n)$ | `same_balls_same_boxes_ge_one` |

`S(n,k)` は第 2 種スターリング数で、区別あり `n` 個を空でない `k` グループへ分ける数。
`p_k(n)` は `n` をちょうど `k` 個の正整数の和へ分割する数、`p_{\le k}(n)` は高々 `k` 個への分割数。
`[P]` は条件 `P` が真なら 1、偽なら 0。

整数分割の記号は文献によって揺れやすい。
`P(n,k)` を「高々 `k` 個への分割」または「最大パーツが `k` 以下の分割」と定義するなら、

$$
P(n,k)=P(n,k-1)+P(n-k,k)
$$

が成り立つ。右辺は「パーツ `k` を使わない」場合と「パーツ `k` を少なくとも 1 個使う」場合への分割。
これは上の表では 10 番の `p_{\le k}(n)` に対応する。

一方、12 番の `p_k(n)` は「ちょうど `k` 個の正整数への分割」なので、

$$
p_k(n)=p_{k-1}(n-1)+p_k(n-k)
$$

を使う。右辺は「1 がある」場合と「全パーツが 2 以上」の場合への分割。
全パーツが 2 以上なら各パーツから 1 を引くことで `n-k` をちょうど `k` 個へ分割する問題になる。

## 3. 覚え方

区別あり玉から区別あり箱への制約なしは、各玉が独立に `k` 個の箱を選ぶので $k^n$。
全箱 1 個以上にすると全射の数になり、空箱集合を包除して数えるか、先に `S(n,k)` 個のグループへ分けてから箱へ対応させるので $k!\,S(n,k)$。

区別なし玉から区別あり箱は、整数解の個数になる。

$$
x_1+\cdots+x_k=n
$$

制約なしなら非負整数解で $\binom{n+k-1}{k-1}$、少なくとも 1 個なら正整数解で $\binom{n-1}{k-1}$。
高々 1 個なら、玉が入る箱を `n` 個選ぶだけなので $\binom{k}{n}$。

箱が区別なしになると、箱の並び替えを同一視する。
区別あり玉なら「玉集合の分割」になり、第 2 種スターリング数が出る。
区別なし玉なら「整数分割」になり、`p_k(n)` や `p_{\le k}(n)` が出る。

## 4. 包除原理で出る形

全射、つまり区別あり玉から区別あり箱へ全箱 1 個以上入れる数は、空になる箱集合を包除して

$$
\sum_{i=0}^{k}(-1)^i\binom{k}{i}(k-i)^n
$$

となる。これは $k!\,S(n,k)$ と等しい。

したがって第 2 種スターリング数は

$$
S(n,k)=\frac{1}{k!}\sum_{i=0}^{k}(-1)^i\binom{k}{i}(k-i)^n
$$

で求められる。ライブラリでは次の関数を同じファイル内に用意している。

- `dist_balls_dist_boxes_ge_one_inclusion_exclusion`
- `dist_balls_same_boxes_ge_one_inclusion_exclusion`
- `dist_balls_same_boxes_any_inclusion_exclusion`

`dist_balls_same_boxes_any` は $\sum_{i=0}^{k}S(n,i)$ を返す。特に `k >= n` ならベル数 $B_n$ になる。
包除原理版は各 `S(n,i)` を上の式で計算して足し上げる。

## 5. 境界ケース

`n=0,k=0` は「何も入れない」1 通りとする。
空箱を許さない相でも、箱が 0 個なら玉も 0 個のときだけ 1 通り。

`k=0,n>0` はどの相も 0 通り。
`高々 1 個` では `n>k` なら 0 通り。
`少なくとも 1 個` では `n<k` なら 0 通り。

重複組合せの $\binom{n+k-1}{k-1}$ は `k=0` では式のまま使えないため、実装では `(n,k)=(0,0)` を別扱いしている。

## 6. ライブラリの使い方

必要な相のファイルだけ include する。

```cpp
#include "mylib/combinatorics/twelvefold_way/dist_balls_same_boxes_ge_one.cpp"

using mint = modint998244353;

mint ans = dist_balls_same_boxes_ge_one<mint>(n, k);
mint ans_ie = dist_balls_same_boxes_ge_one_inclusion_exclusion<mint>(n, k);
```

ファイル一覧:

- `dist_balls_dist_boxes_any.cpp`
- `dist_balls_dist_boxes_le_one.cpp`
- `dist_balls_dist_boxes_ge_one.cpp`
- `same_balls_dist_boxes_any.cpp`
- `same_balls_dist_boxes_le_one.cpp`
- `same_balls_dist_boxes_ge_one.cpp`
- `dist_balls_same_boxes_any.cpp`
- `dist_balls_same_boxes_le_one.cpp`
- `dist_balls_same_boxes_ge_one.cpp`
- `same_balls_same_boxes_any.cpp`
- `same_balls_same_boxes_le_one.cpp`
- `same_balls_same_boxes_ge_one.cpp`

## 7. 計算量の目安

単純な式だけで済む相は $O(n)$ または $O(k)$。
第 2 種スターリング数を DP する相は $O(nk)$。
整数分割を DP する相もおおむね $O(nk)$。

包除原理版の `S(n,k)` は $O(k\log n)$。
`n` が非常に大きく `k` が小さい場合は DP より包除原理版が使いやすい。
ただし $\frac{1}{k!}$ を使うため、mod 上で `1,2,...,k` が逆元を持つ必要がある。
