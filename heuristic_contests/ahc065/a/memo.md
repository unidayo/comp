# Memo

- N = 20 固定
- 搬出口: `(0, 10)`
- 最大コンベア数: `N^2 = 400`
- 最大操作回数: `100000`
- 各マスが属せるコンベア数: 高々 2
- 操作後、搬出口に残り最小番号の箱があれば自動で搬出
- 初期状態で搬出口に 0 があれば、操作前に 0 が搬出される

## TODO

- 20 本リング解の seed batch 実行
- step-beam の評価関数の重み調整
- step-beam 幅と lookahead 長の比較
- 距離が減らない一時退避操作を安全に入れられるか検証
- 追加の局所最適化で無駄な往復を減らす

## Current Baseline

- Conveyor 0..9: horizontal 2x20 rings
- Conveyor 10..19: vertical 20x2 rings
- Each cell belongs to exactly 2 conveyors.
- For the current box, branch over one-step horizontal/vertical moves that reduce distance to the exit.
- Current eval: linear weights, `EVAL_LOOK=80`, `EVAL_WEIGHT=30`
- Current search: fixed-width step beam, `BEAM_WIDTH=300`
- Per-layer Zobrist deduplication is enabled.
- State copies are optimized with fixed-size arrays and parent pointers for operation history.
- Official seeds 0..99 with `BEAM_WIDTH=300`: avg_T = 2812.00, best_T = 2721, worst_T = 2917
- Flat/setup moves are implemented behind `SETUP_FUTURE`, but disabled by default.
  `BEAM_WIDTH=150 SETUP_FUTURE=3` gave avg_T = 2903.32 and was worse than the current baseline.
