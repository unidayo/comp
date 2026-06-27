# SimulatedAnnealing

AHC 用の焼きなまし補助。状態や近傍生成は問題ごとに違うため、ライブラリは温度スケジュール、継続判定、採択判定だけを担当する。

```cpp
#include "mylib/heuristic/simulated_annealing.cpp"

SimulatedAnnealing::Config cfg;
cfg.start_temp = 100.0;
cfg.end_temp = 0.1;
cfg.time_limit_ms = 1950;
cfg.objective = SimulatedAnnealing::Objective::Maximize;
cfg.schedule = SimulatedAnnealing::Schedule::Exponential;
cfg.seed = 123456789;

SimulatedAnnealing sa(cfg);

long long score = initial_score();
long long best_score = score;
State best_state = state;

while (sa.should_continue()) {
    Move mv = random_move(sa.rng);
    long long old_score = score;
    apply(mv);
    score = evaluate();

    if (sa.accept(old_score, score)) {
        if (sa.update_best(score, best_score)) {
            best_state = state;
        }
    } else {
        undo(mv);
        score = old_score;
    }

    sa.next_iteration();
}

state = best_state;
```

## API

- `SimulatedAnnealing::Config`
  - `start_temp`, `end_temp`: 開始温度と終了温度。
  - `time_limit_ms`: 時間制限。使わない場合は `-1`。
  - `max_iterations`: 反復回数制限。使わない場合は `-1`。
  - `objective`: `Maximize` または `Minimize`。
  - `schedule`: `Exponential` または `Linear`。
  - `seed`: 内部の `FastRandom` seed。
- `sa.should_continue()`: 時間または反復回数制限に達するまで true。
- `sa.next_iteration()`: 1反復進める。
- `sa.progress()`: 進捗 `[0, 1]`。
- `sa.temperature()`: 現在温度。
- `sa.accept_delta(delta)`: 差分スコアで採択判定。`Maximize` では正が改善。
- `sa.accept(current_score, next_score)`: 現在スコアと次スコアで採択判定。
- `sa.update_best(score, best_score)`: objective に応じて best を更新する。
- `sa.rng`: 近傍生成にも使える `FastRandom`。

## 注意

- `accept()` は採択判定だけを行う。状態の `apply` / `undo` は呼び出し側で行う。
- 差分スコアが安く計算できる場合は、`evaluate()` し直さず `accept_delta(delta)` を使う。
- 時間制限と反復回数制限を両方指定した場合、どちらかに到達した時点で止まる。

