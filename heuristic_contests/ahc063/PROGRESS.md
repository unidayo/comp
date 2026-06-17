# AHC063 進捗まとめ

## 問題概要
- `score = T + 10000 * (E + 2 * (M-k))`
- 実質的には `M-k=0` をまず保証し、その上で `E` を 1 減らす価値が極めて大きい
- `T` も見るが、今の solver では `E` 改善が主目的

## 現在の採用アルゴリズム

### Phase 1
- `build_path()` で色つき Hamiltonian path を構築
- 固定 prefix を置いた上で `rebuild_path()` を流用して経路候補を複数試す
- `compute_E()` で色列を評価し、最良の path を採用
- その path に沿って全餌を食べ切り、`M-k=0` を保証

### Phase 2
- `State A`
  - Phase 1 終了時点の状態
- `State B`
  - Phase 1 の path を終端まで歩いた状態
- `fertile state`
  - 「今の score」ではなく「次に改善しやすいか」で保持する探索母体

- 初期大撹拌
  - `try_initial_stir()` で短い random walk
  - その後 1 回 bite
  - 続けて `rebuild_path()`
  - 良かったものだけ `fertile` と `best` に反映

- 本体ループ
  - `State A / State B / fertile` を `state_metrics()` に基づいて重み付き選択
  - 頭の隣接 4 マスの bite 候補を列挙
  - 深さ優先をベースに、prefix の汚さを軽く見たスコアで候補順を決定
  - bite 後は `rebuild_path()` で suffix を再構築
  - `best` は最終 score で更新
  - `fertile` は `state_metrics()` で更新

### `state_metrics()` の役割
- `fertile` 更新と初期状態選択の両方に使う
- 現在見ている要素
  - `potential`
  - 全体 `E`
  - prefix 不一致数
  - 頭の自由度
  - 隣接 bite 候補数
  - 最深 bite の深さ
  - `T`

- 現在の採用プロファイル
  - `AHC063_PROFILE=2`
  - デフォルト値として本体に埋め込み済み

## 現在の探索の流れ

1. `build_path()` で Phase 1 の初期完成ヘビを作る
2. そのままの状態 `A` と、終端まで歩いた状態 `B` を作る
3. 初期大撹拌を数回回して、良い探索母体を `fertile` に入れる
4. `A/B/fertile` の中から次の trial の母体を選ぶ
5. 隣接 bite 候補を並べて順に `rebuild_path()` を試す
6. 良い score は `best` に、次に伸びそうな状態は `fertile` に反映する
7. 時間切れまで 4-6 を繰り返す

## 比較方法

### 基本方針
- 20 seed 比較はノイズが大きいので採用判断に使わない
- 原則 `100 seeds` 比較で採否を決める

### 比較ツール
- `compare.py`
- `compare.sh`

### 見る指標
- `avg score`
- `wins / losses / draws`
- `avg delta E`
- `avg delta T`
- 必要に応じて `median diff` や seed ごとの明細

## ここまでで効いた変更

### 1. `fertile state` の導入
- 「良いヘビ」と「次に伸びるヘビ」を分けて持つようにした
- これが Phase 2 では最も効いた変更

### 2. `fertile` 更新基準の強化
- `potential + E` だけでなく
  - prefix
  - 頭自由度
  - bite 候補数
  - 深い bite
  を入れた
- 100 seed で明確に改善

### 3. 初期状態選択の重み付き化
- 固定比率で `A/B/fertile` を選ぶのをやめた
- `state_metrics()` の `key` 差に応じて重み付きで選ぶようにした
- これも 100 seed で改善

### 4. `state_metrics` 重みの調整
- `T` の重みを少し下げ
- prefix / head_free / bite_cnt / best_bh を強めた
- 複数プロファイル比較の結果、`profile 2` が現状ベスト

### 5. 初期大撹拌の bite 候補選択改善
- `try_initial_stir()` で最深 bite 固定をやめた
- 軽い prefix-aware score を入れた
- 改善幅は小さいが 100 seed でプラス

### 6. Phase 2 本体の bite 候補順改善
- 深さ優先のみから
- `fixed_E * 512 - idx * 8` の軽い prefix-aware 順に変更
- 100 seed でプラス

## 試して戻した変更

### Phase 1 関連
- Phase 1 を単純蛇行に戻す
- Phase 1 を完全に捨てて大量撹拌にする
- Phase 1 の prefix 分岐や軽い beam
- 状態依存 `prefix_lim`

### Phase 2 関連
- 3 本以上の fertile pool
- local repair
- 明示的 random shuffle
- `数手歩いてから bite`
- `no adjacent bite` のときの短距離 walk-to-bite
- bite 試行順の分散強化
- `fertile` 更新にヒステリシスを入れる

### なぜ戻したか
- 多くは `E` 改善より `T` だけが増えた
- あるいは一部 seed で当たるが 100 seed 平均では負けた
- 特に「適当に動いて混ぜる」系は、今の solver と相性が悪かった

## 現在の理解

- `Phase1` の多様化は効きにくい
- `Phase2` は「bite 候補を広げる」より「良い探索母体を残す」方が効く
- `E` を 1 個下げる価値が非常に高いので、`T` を多少使っても `E` が下がる変更は採用しやすい
- 逆に `T` だけ節約する改善はほぼ効かない
- 今の solver は `fertile` 管理の質がかなり支配的

## 次に有望な方向

1. `state_metrics` の追加プロファイル比較を継続
- 今のコードは `AHC063_PROFILE` で切替可能
- 重み探索を続けやすい状態

2. `try_initial_stir()` をもう少し `fertile` 指向にする
- walk 長分布
- bite 候補の軽い選別

3. bite 候補スコアの微調整
- 今は軽い prefix-aware score
- ここを壊さない範囲で係数を動かす余地はある

## 関連ファイル
- `a/ahc063_a.cpp`
- `run.sh`
- `score.py`
- `compare.py`
- `compare.sh`
