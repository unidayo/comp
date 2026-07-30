# combinations

二項係数 `C(n, r)` を1件だけ計算するライブラリです。前計算を行わず、計算量は `O(min(r, n-r))`、追加メモリは `O(1)` です。

## 法なし

```cpp
long long value = combination(60, 30);
```

返却型はテンプレート引数で変更できます。

```cpp
unsigned long long value = combination<unsigned long long>(66, 33);
```

組み込み整数型では、最終的な二項係数が返却型に収まる必要があります。積は128-bitへ拡張してから割るため、答えが収まる場合の中間積オーバーフローは避けられます。

Boost.Multiprecisionなどを利用できる環境では、任意精度整数型も指定できます。

```cpp
using boost::multiprecision::cpp_int;
cpp_int value = combination<cpp_int>(1000, 500);
```

## 素数法

```cpp
long long value = combination_mod_prime(1000, 500, 1'000'000'007);
```

Fermatの小定理で分母の逆元を求めるため、次の条件が必要です。

- `mod` が素数
- `0 <= n < mod`

`n >= mod` では階乗にmodの因子が現れるため、この関数は使えません。Lucasの定理などを使います。合成数modや多数クエリでは `comb_mod_dp.cpp` の `CombinationDP` が適しています。

従来の `comb_mod(n, r, mod)` も互換用として残していますが、新しいコードでは前提が名前から分かる `combination_mod_prime` を推奨します。

## 使い分け

| 用途 | 選択 |
|---|---|
| 正確値を1件 | `combination<T>` |
| 素数modで1件 | `combination_mod_prime` |
| 合成数mod | `CombinationDP` |
| 同じ範囲で多数クエリ | `CombinationDP` または `comb_mint.cpp` |
