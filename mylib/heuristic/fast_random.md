# FastRandom

AHC 用の軽量乱数生成器。`xoshiro256**` を使い、`std::mt19937` より軽く固定 seed で再現しやすい。

```cpp
#include "mylib/heuristic/fast_random.cpp"

FastRandom rng(123456789);

int i = rng.uniform_int(0, 99);          // [0, 99]
long long x = rng.uniform_ll(-10, 10);   // [-10, 10]
double u = rng.uniform01();              // [0, 1)
double y = rng.uniform_real(-1.0, 1.0);  // [-1, 1)

if (rng.bernoulli(0.25)) {
    // probability 25%
}

double z = rng.normal(0.0, 1.0);
double wait = rng.exponential(2.0);
int failures = rng.geometric(0.1);

vector<int> weights = {1, 3, 6};
int k = rng.weighted_index(weights);

vector<int> a = {0, 1, 2, 3, 4};
rng.shuffle(a.begin(), a.end());
int picked = rng.choice(a);
```

## API

- `FastRandom rng(seed)`: seed を指定して作る。
- `rng.reseed(seed)`: seed を設定し直す。
- `rng.next_u64()`, `rng.next_u32()`: 生の乱数。
- `rng.uniform_u64(bound)`: `[0, bound)` の整数。剰余バイアスを避ける。
- `rng.uniform_int(l, r)`: `[l, r]` の `int`。
- `rng.uniform_ll(l, r)`: `[l, r]` の `long long`。
- `rng.uniform01()`: `[0, 1)` の `double`。
- `rng.uniform_real(l, r)`: `[l, r)` の `double`。
- `rng.bernoulli(p)`: 確率 `p` で true。
- `rng.normal(mean, stddev)`: 正規分布。
- `rng.exponential(lambda)`: 指数分布。
- `rng.geometric(p)`: 成功確率 `p` で、初成功までの失敗回数。
- `rng.weighted_index(weights)`: 非負重みから添字を選ぶ。
- `rng.shuffle(first, last)`: ランダムシャッフル。
- `rng.choice(values)`: `vector` から一様に1要素を選ぶ。

## 用途の目安

- 焼きなましの採択判定: `uniform01()` または `bernoulli(p)`。
- 近傍選択: `uniform_int()`、`choice()`、`weighted_index()`。
- 連続値の摂動: `uniform_real()`、`normal()`。
- ランダムな待ち時間・偏ったステップ幅: `exponential()`、`geometric()`。

