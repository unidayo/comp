# CombinationDP

Pascalの三角形により、`0 <= n <= max_n` の二項係数 `C(n, k)` をまとめて前計算します。除算を使わないため、法が素数でなくても利用できます。

## 法を使わない場合

```cpp
CombinationDP<long long> comb(60);

cout << comb(5, 2) << '\n';   // 10
cout << comb(5, -1) << '\n';  // 0
cout << comb(5, 6) << '\n';   // 0
```

`long long` で正確に保持できるのは `C(66, 33)` までです。それより大きい値が必要なら、Boost.Multiprecisionなどを利用できる環境では任意精度整数を指定できます。

```cpp
#include <boost/multiprecision/cpp_int.hpp>
using boost::multiprecision::cpp_int;

CombinationDP<cpp_int> comb(1000);
cout << comb(1000, 500) << '\n';
```

## 法を指定する場合

```cpp
constexpr long long MOD = 1'000'000'007;
CombinationDP<long long> comb(2000, MOD);

cout << comb(2000, 1000) << '\n';
```

加算だけで構築するので、`MOD` は合成数でも構いません。組み込み整数型では加算を128-bitへ拡張してから剰余を取るため、`MOD` が型の上限に近い場合も中間加算はオーバーフローしません。

## modintを使う場合

`T` 自身が剰余を管理するため、コンストラクタへ法を渡しません。

```cpp
using mint = atcoder::modint998244353;
CombinationDP<mint> comb(2000);

cout << comb(2000, 1000).val() << '\n';
```

## 計算量と注意点

- 構築時間: `O(max_n^2)`
- メモリ: `O(max_n^2)`
- 取得: `O(1)`
- `n` が前計算範囲外ならassertに失敗します。
- `k < 0` または `n < k` なら0を返します。
- 表は三角形だけを保存するため、従来の正方形配列のおよそ半分のメモリです。

表全体が必要な場合は `table()` から読み取り専用で参照できます。
