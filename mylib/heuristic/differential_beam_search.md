# DifferentialBeamSearch

AHC 用の差分更新ビームサーチ部品。状態はコピーせず、呼び出し側が持つ `State` を `apply(action)` / `undo(action)` で更新する。

```cpp
#include "mylib/heuristic/differential_beam_search.cpp"

struct Action {
    int v;
};

State st;
auto apply = [&](const Action& a) -> bool {
    // st に a を適用する。適用不可なら false を返す。
    return true;
};
auto undo = [&](const Action& a) {
    // apply(a) を戻す。
};
auto evaluate = [&](int depth) -> long long {
    return st.score;
};
auto get_hash = [&]() -> uint64_t {
    return st.hash;
};
auto expand = [&](int depth, const auto& node, auto&& try_action) {
    for (Action a : enumerate_actions(st)) {
        try_action(a); // 内部で apply -> evaluate/hash -> undo される
    }
};

DifferentialBeamSearch<Action>::Config cfg;
cfg.beam_width = 1000;
cfg.max_depth = 200;
cfg.larger_score_better = true;
cfg.reserve_nodes = cfg.beam_width * cfg.max_depth + 1;

DifferentialBeamSearch<Action> beam(cfg);
int best = beam.run(evaluate(0), get_hash(), expand, apply, undo, evaluate, get_hash);
vector<Action> answer = beam.actions_to(best);
```

## 使い分け

- 状態コピーが重い場合はこの部品を優先する。
- `State` が小さくコピーが十分速い場合は、問題ごとの単純な `vector<State>` beam のままでもよい。
- 同一状態除去は `hash()` の値で行う。ハッシュ衝突が気になる場合は `deduplicate_by_hash = false` にするか、十分な Zobrist hash を用意する。
- `apply()` が `bool` を返す場合、`false` の action は候補に追加されず `undo()` も呼ばれない。
- `expand()` が自分で状態を変更する場合は、戻してから返ること。

