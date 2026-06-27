# Timer

AHC 用の薄い時間計測部品。`steady_clock` を使い、ミリ秒単位の制限時間を持てる。

```cpp
#include "mylib/heuristic/timer.cpp"

Timer timer(1950); // 1950 ms

while (!timer.expired()) {
    improve();
}

if (timer.elapsed_ms() < 1000) {
    run_extra_phase();
}
```

## API

- `Timer timer(limit_ms)`: 作成時点から計測開始。`limit_ms < 0` なら期限なし。
- `timer.reset()`: 開始時刻を現在に戻す。
- `timer.reset(limit_ms)`: 開始時刻と制限時間を更新する。
- `timer.elapsed_ms()`: 経過時間を `double` の ms で返す。
- `timer.elapsed_sec()`: 経過時間を秒で返す。
- `timer.elapsed_us()`: 経過時間を整数 us で返す。
- `timer.remaining_ms()`: 残り ms。期限なしなら大きな値を返す。
- `timer.expired()`: 制限時間を過ぎたら true。
- `timer.should_continue()`: `!expired()`。
- `timer.progress()`: `elapsed_ms / limit_ms` を `[0, 1]` に丸めた値。

