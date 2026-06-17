# comp Workspace Guide

このリポジトリは競技プログラミング用の作業ディレクトリです。現在の `comp/` 配下に何があるかと、作業時の運用をまとめています。

## 全体構成

```text
comp/
├── contests/              # 通常コンテストの解答置き場
├── heuristic_contests/    # AHCなどヒューリスティックコンテスト用
├── virtual_contests/      # バーチャルコンテスト用
├── problems/              # 単発問題用。日時名のディレクトリを作る
├── mylib/                 # 自作ライブラリ集
├── scripts/               # コンテスト/問題作成などの補助スクリプト
├── gpu_experiments/       # CUDA/GPU 実験コード
├── archive/               # 過去の contests 退避先
├── typings/               # Python/PyPy 用の型定義
├── unordered_dense/       # 外部ライブラリのソースツリー
├── CMakeLists.txt         # CMake 設定
└── AGENTS.md              # このファイル
```

## contests/

AtCoder、Codeforces、その他通常コンテストの解答ファイルを置きます。
現在の `contests/` は空に近く、過去のコンテストは主に `archive/contests_日時/` に退避されています。

基本形は次の通りです。

```text
contests/{contest_id}/{label}/{contest_id}_{label}.cpp
contests/{contest_id}/{label}/test/
contests/{contest_id}/contest.acc.json
```

例:

```text
contests/abc459/a/abc459_a.cpp
contests/abc459/a/test/
contests/cf2205/c/cf2205_c.cpp
```

`contest.acc.json` があるディレクトリは `acc` で生成された AtCoder 系コンテストです。Codeforces 用は `cf{contest_id}` 形式で置かれます。

## heuristic_contests/

AHC などのヒューリスティックコンテスト用です。通常の `contests/` より、実験スクリプト、メモ、スコア計測、比較ツールなどを一緒に置く想定です。

現在の例:

```text
heuristic_contests/ahc063/
├── a/ahc063_a.cpp
├── gen/gen.cpp
├── compare.py
├── compare.sh
├── run.sh
├── score.py
├── notes.md
├── PROGRESS.md
└── tmp_profiles/

heuristic_contests/ahc065/a/
├── main.cpp
├── README.md
├── bin/
├── in/
├── out/
├── scripts/
├── statement/
├── tools/
└── vis/

heuristic_contests/ahc066/
├── a/
├── cases/
├── out/
├── O25rQjiK/tools/
└── batch.sh
```

AHC の新規作業は `heuristic_contests/ahcXXX/` に作るのが基本です。問題が 1 問だけの場合でも `a/` を切る既存例、直下に `cases/` や `batch.sh` を置く既存例、事前準備用に `statement/`、`tools/`、`in/`、`scripts/` を切る運用が混在しています。既存ディレクトリを触る場合は、そのコンテスト内の配置を優先します。

### AtCoder Heuristic Contest の事前準備

ユーザーから AtCoder Heuristic Contest の事前準備を依頼された場合は、コンテスト URL を受け取り、原則として次の手順で `heuristic_contests/{contest_id}/` 配下に準備します。

目的:

- 問題ページ HTML を保存する。
- 問題本文を LLM が読みやすい Markdown に変換する。
- ローカル版ツールをダウンロードする。
- ローカルツールから入力生成方法を特定する。
- テストケース 100 個を生成する。

不要:

- `oj` によるサンプル取得。
- `main.cpp` テンプレート生成。

重要:

- ローカル版ツールのリンク名はコンテストごとに異なるため、単純な文字列一致で判断しない。
- 問題ページ HTML、`tasks_print`、リンク周辺の文脈を読んで、どれが local tools かを判断する。
- 安全のため、ダウンロード対象は原則として `atcoder.jp` と `img.atcoder.jp` に限定する。
- 外部ドメインのリンクは、local tools らしく見えても自動ダウンロードしない。必要ならユーザーに確認する。

作業手順:

1. URL から `contest_id` と `task_id` を得る。
2. task ページと `tasks_print` ページを保存する。
3. 全リンクを抽出して `statement/links.json` に保存する。
4. HTML と `links.json` を読んで、local tools のリンクを判断する。
5. local tools を `tools/archive/` にダウンロードする。
6. `zip`、`tar.gz`、`tgz` なら `tools/local/` に展開する。
7. 展開後、`README.md`、`Cargo.toml`、`src/bin`、`gen`、`tester`、`vis` などを調べる。
8. 入力生成方法を判断する。
9. まず `seed=0` で試し実行する。
10. 出力が空でなく、終了コード 0 なら成功とみなす。
11. 同じ方法で `seed=0..99` の 100 ケースを `in/0000.txt` から `in/0099.txt` に生成する。生成自体は並列でなくてもよい。
12. 使った生成コマンドを `scripts/generate_inputs.sh` に保存する。
13. 生成済みテストケースに対して解答、tester、scorer、visualizer などを実行するスクリプトやコマンドを用意する場合は、可能な限り `xargs -P` や GNU Parallel などでテストケースごとに並列実行する。
14. 判断に迷った場合は、候補を列挙して理由を説明し、ユーザーに確認する。

生成する構成:

```text
heuristic_contests/{contest_id}/
├── statement/
│   ├── task.html
│   ├── tasks_print.html
│   ├── task.md
│   └── links.json
├── tools/
│   ├── archive/
│   ├── local/
│   └── tool_info.json
├── in/
│   ├── 0000.txt
│   └── ...
├── scripts/
│   └── generate_inputs.sh
└── config.toml
```

## virtual_contests/

バーチャルコンテスト用です。`mov663`、`mov664` のような短い識別名のディレクトリと、日本語名を含むディレクトリが混在しています。

例:

```text
virtual_contests/mov664/abc333/
virtual_contests/もすーんバチャ #663/A. Product/
```

`scripts/new_virtual_contest.sh` は URL を入力して `oj-api` と `oj` で問題を取得します。
`scripts/new_virtual_contest.py` もあります。

## problems/

コンテスト単位ではない単発問題の置き場です。`scripts/new_standalone_problem.sh` は日時名でディレクトリを作り、テンプレートとサンプル用ファイルを配置します。
現在の `problems/` は空に近く、過去の単発問題は `archive/contests_日時/` 側に退避されているものがあります。

基本形:

```text
problems/YYYYMMDD_HHMMSS/YYYYMMDD_HHMMSS.cpp
problems/YYYYMMDD_HHMMSS/test/
```

Python の単発問題は `scripts/new_standalone_python_problem.sh` で作成します。

```text
problems/YYYYMMDD_HHMMSS/YYYYMMDD_HHMMSS.py
problems/YYYYMMDD_HHMMSS/test/
```

Python 解答は `uv` で作成した `.venv-pypy/bin/python`（PyPy 7.3.20 /
Python 3.11.13）で実行します。VS Code では `execute Python`、
`execute Python with input.txt`、`test Python sample` タスクを使用します。
環境の初回作成・再作成は `scripts/setup_pypy.sh` を実行します。
PyPy 環境の追加パッケージは `requirements-pypy.txt` で管理します。
Pylance 用の PyPy 固有モジュール型定義は `typings/` に置きます。

## mylib/

自作ライブラリ集です。カテゴリごとに `.cpp` ファイルを置いています。

主なカテゴリ:

```text
mylib/acl/             # AtCoder Library の展開済み/提出補助用
mylib/graph/           # dijkstra, hld, lowlink, rerooting など
mylib/datastructure/   # segtree, treap, binary_trie など
mylib/algebra/         # factorization, matrix, discrete_log など
mylib/combinatrics/    # comb, factorial など
mylib/fps/             # formal power series
mylib/geometry/        # convex_hull, arg_sort など
mylib/string/          # kmp, rolling_hash, aho_corasick など
mylib/algorithm/       # inversion_number, slope_trick など
mylib/accumulate/      # 2D 累積和など
mylib/math/            # isqrt など
mylib/misc/            # その他
```

本物の ACL は `/usr/lib/ac-library/` にあり、CMake で include path に追加されています。ローカルでは `ATCODER` が定義されるため `#include <atcoder/all>` が通ります。Codeforces などで ACL 展開が必要な場合は `mylib/acl/*.cpp` から必要分を貼る運用です。

## scripts/

作業補助スクリプトです。

| ファイル | 用途 |
|---|---|
| `scripts/new_contest.sh {id}` | `contests/` 配下に AtCoder コンテストを `acc new` で作成 |
| `scripts/new_problem.sh {id}` | 既存/新規 AtCoder コンテストに問題を追加 |
| `scripts/new_cf_contest.sh {id} {n}` | `contests/cf{id}` に Codeforces の a から n 問分を作成 |
| `scripts/new_cf_problem.sh {id} {label}` | Codeforces の単一問題を追加 |
| `scripts/new_other_contest.sh` | AtCoder/Codeforces 以外などのコンテスト作成補助 |
| `scripts/new_virtual_contest.sh` | バーチャルコンテスト URL から問題を取得 |
| `scripts/new_virtual_contest.py` | バーチャルコンテスト作成補助の Python 版 |
| `scripts/new_standalone_problem.sh` | `problems/` に日時名の単発問題を作成 |
| `scripts/new_standalone_python_problem.sh` | `problems/` に Python 用の日時名の単発問題を作成 |
| `scripts/archive_contests.sh` | `contests/*/` を `archive/contests_日時/` に退避 |

一部スクリプトは `code` コマンドで VSCode を開きます。サンドボックス環境や GUI のない環境では作成処理だけ使い、エディタ起動は失敗しても本質ではありません。

## ビルド

ビルドシステムは CMake です。

主な設定:

- CMake 最低バージョン: `4.1.2`
- コンパイラ: `/opt/gcc-15.2/bin/g++`
- C++ 標準: C++23
- リンカ: mold
- `compile_commands.json` を出力
- `ccache` があれば使用
- Debug では `-O0 -g _GLIBCXX_DEBUG`
- Release では `-O2 -march=native -mtune=native`
- sanitizer: `undefined,address`、leak sanitizer は既定で無効
- 共通 PCH: `pch.cpp` で `<bits/stdc++.h>`

対象ソースは次の glob で拾われます。

```cmake
contests/*.cpp
codes/*.cpp
heuristic_contests/*.cpp
problems/*.cpp
```

実際には `GLOB_RECURSE` なので各配下の深い `.cpp` も対象になります。現在 `codes/` は存在しませんが、再作成された場合は CMake の対象になります。`gpu_experiments/` の `.cpp` や `.cu` はこの CMake には含まれません。

## CMake ターゲット名

ターゲット名は `{grandparent}_{parent}_{filename}` 形式です。

例:

```text
contests/abc446/f/abc446_f.cpp
  -> abc446_f_abc446_f

heuristic_contests/ahc063/a/ahc063_a.cpp
  -> ahc063_a_ahc063_a

problems/20260522_150017/20260522_150017.cpp
  -> problems_20260522_150017_20260522_150017
```

## gpu_experiments/

CUDA/GPU で競プロ解答を試す実験用ディレクトリです。現在は `abc194_c`、`abc200_c`、`abc206_c`、`abc405_c` などがあり、各ディレクトリに CPU 実装、GPU 実装、`build.sh`、乱択チェック、最大ケース生成、サンプル実行スクリプトなどがあります。

CMake 管理外です。各実験ディレクトリの `README.md` や `build.sh` に従います。

## codes/

一時コード置き場として CMake 設定には残っていますが、現在トップレベルに `codes/` は存在しません。必要になった場合だけ作成します。CMake は `codes/**/*.cpp` を拾いますが、Python はビルド対象外です。

## 生成物・キャッシュ・外部コード

基本的に次は編集対象ではなく、存在する場合も必要なときだけ参照します。

```text
build-debug/
build-release/
CMakeFiles/
CMakeCache.txt
Makefile
libpch.a
a.out
gmon.out
pch.dir/
.cache/
.venv/
.venv-pypy/
unordered_dense/
```

`unordered_dense/` は外部ライブラリのソースツリーです。競プロ解答の作業とは別扱いにします。

## 作業時の注意

- 新しい通常コンテストは `contests/{id}/` に作る。
- 新しいヒューリスティックコンテストは `heuristic_contests/ahcXXX/` に作る。
- 単発問題は `problems/YYYYMMDD_HHMMSS/` に作る。
- 解答 `.cpp` は CMake に自動登録されるため、深い階層に置いても基本的に拾われる。
- `gpu_experiments/` は CMake 管理外なので、各ディレクトリのスクリプトに従う。
- 既存のビルド成果物やキャッシュは不要に触らない。
- `mylib/acl/` は Codeforces など提出用の補助として使う。ローカルでは `/usr/lib/ac-library/` と `ATCODER` 定義がある。
- 日本語やスペースを含むディレクトリ名があるため、シェルではパスを必ずクォートする。
