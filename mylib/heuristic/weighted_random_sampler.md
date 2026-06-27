# WeightedRandomSampler

重み付きランダムサンプリング用ライブラリ。重みは非負、サンプリング時の合計重みは正である必要がある。

```cpp
#include "mylib/heuristic/weighted_random_sampler.cpp"

FastRandom rng(123);

vector<int> weights = {1, 3, 6};
WeightedRandomSampler<int> sampler(weights);
int i = sampler.sample(rng); // P(i) = weights[i] / sum(weights)
```

重みを更新する場合:

```cpp
DynamicWeightedRandomSampler<long long> sampler(vector<long long>{1, 3, 6});

int i = sampler.sample(rng);
sampler.add_weight(i, -1);
sampler.set_weight(0, 10);
```

## API

### WeightedRandomSampler

- `WeightedRandomSampler(weights)`: 静的重みで構築する。
- `build(weights)`: 重みを作り直す。
- `sample(rng)`: 重みに比例して添字を返す。`O(log N)`。
- `total_weight()`: 合計重み。
- `can_sample()`: 合計重みが正なら true。

### DynamicWeightedRandomSampler

- `DynamicWeightedRandomSampler(n)`: 全重み 0 で作る。
- `DynamicWeightedRandomSampler(weights)`: 初期重みで作る。
- `add_weight(i, delta)`: `i` の重みに `delta` を足す。結果は非負であること。
- `set_weight(i, value)`: `i` の重みを `value` にする。
- `sample(rng)`: 重みに比例して添字を返す。`O(log N)`。
- `weight(i)`: 現在の重み。
- `total_weight()`: 合計重み。
- `can_sample()`: 合計重みが正なら true。

## 使い分け

- 重みを作った後に更新しない: `WeightedRandomSampler`
- 探索中に候補の重みを増減する: `DynamicWeightedRandomSampler`
- 一度だけ選ぶ小さい配列: `FastRandom::weighted_index()` でも十分

