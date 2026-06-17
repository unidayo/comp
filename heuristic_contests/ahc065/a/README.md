# AHC065 A - Conveyor Design

作業ディレクトリ:

```bash
cd /home/wsl2ubuntu/comp/heuristic_contests/ahc065/a
```

基本操作:

```bash
./scripts/build.sh
python3 tools/gen.py 0 > in/seed0.txt
./scripts/run.sh in/seed0.txt out/seed0.txt
python3 tools/check.py in/seed0.txt out/seed0.txt
./scripts/batch.sh 100 8
./scripts/make_output.sh 0
```

`tools/gen.py` は公式ビジュアライザ同梱の入力を使います。
seed 0..99 は `vis/tools/in/0000.txt` .. `0099.txt` をそのまま出力し、それ以外は公式 `cargo run -r --bin gen` を呼びます。

`main.cpp` は、2x20 の横リング 10 本と 20x2 の縦リング 10 本を置きます。
現在の箱について、所属する横リング・縦リングそれぞれの `+1/-1` 操作を試し、搬出口までの距離が 1 減る操作だけを beam search で選びます。
これにより、縦・横のコンベアを交互に使いながら少しずつ搬出口へ近づけます。
現在のデフォルトは `EVAL_LOOK=80`, `EVAL_WEIGHT=30`, `BEAM_WIDTH=300` です。
各 beam 層では Zobrist hash で同一盤面をまとめ、評価順で最良の状態だけを残します。

問題ページの保存元:

- `statement/original.mhtml`
- `statement/problem.md`
