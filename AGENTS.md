# comp Workspace Guide

このリポジトリは競技プログラミング用の作業ディレクトリです。現在の `comp/` 配下に何があるかと、作業時の運用をまとめています。

## 全体構成

```text
comp/
├── contests/              # 通常コンテストの解答置き場
├── heuristic_contests/    # AHCなどヒューリスティックコンテスト用
├── virtual_contests/      # バーチャルコンテスト用
├── problems/              # 単発問題用。日時名のディレクトリを作る
├── templates/             # acc などで使う生成元テンプレート
├── mylib/                 # 自作ライブラリ集
├── notes/                 # 競プロで詰まったときに参照する Markdown ノート
├── scripts/               # コンテスト/問題作成などの補助スクリプト
├── gpu_experiments/       # CUDA/GPU 実験コード
├── archive/               # 過去の contests 退避先
├── typings/               # Python/PyPy 用の型定義
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

AHC の方針検討や改善で詰まった場合は、まず `reference_data/atcoder_heuristic/solution_summaries/` のコンテスト別まとめと手法別索引から、制約、状態遷移、操作、評価関数が似た問題を探します。その後、`reference_data/atcoder_heuristic/ahc001_049/` と `reference_data/atcoder_heuristic/ahc050_066/` の問題文・上位解答を読み、探索、近傍、評価、枝刈り、差分更新などの手法を現在の問題へ適用できないか検証します。

### AHC 実装用ライブラリ

AHC でビームサーチを書く場合、状態コピーが重い実装では `mylib/heuristic/differential_beam_search.cpp` の `DifferentialBeamSearch` を優先して使います。呼び出し側の `State` を `apply(action)` / `undo(action)` で差分更新し、ライブラリ側が探索木の復元、候補評価、hash による重複除去、解の action 復元を担当します。使い方は `mylib/heuristic/differential_beam_search.md` を参照します。

AHC で時間計測を書く場合は `mylib/heuristic/timer.cpp` の `Timer` を使います。`Timer timer(1950);` のようにミリ秒制限を渡し、`timer.elapsed_ms()`、`timer.expired()`、`timer.remaining_ms()` を使います。使い方は `mylib/heuristic/timer.md` を参照します。

AHC で乱数を使う場合は `mylib/heuristic/fast_random.cpp` の `FastRandom` を使います。整数一様乱数、浮動小数点一様乱数、`bernoulli`、`normal`、`exponential`、`geometric`、`weighted_index`、`shuffle` を提供します。使い方は `mylib/heuristic/fast_random.md` を参照します。

AHC の乱数 seed は、入力内容から決定するなど再現性のある方法で固定する。ローカルで手元にあるテストケース集合に対して seed 値だけを複数試して選ぶ調整は、テストケース依存の過適合になりやすいため行わない。

AHC で焼きなまし法を書く場合は `mylib/heuristic/simulated_annealing.cpp` の `SimulatedAnnealing` を使います。温度スケジュール、時間/反復回数の継続判定、最大化/最小化の採択判定、best 更新補助を提供します。使い方は `mylib/heuristic/simulated_annealing.md` を参照します。

AHC で重み付きランダムサンプリングを使う場合は `mylib/heuristic/weighted_random_sampler.cpp` を使います。静的重みには `WeightedRandomSampler`、探索中に重みを更新する場合は Fenwick 木ベースの `DynamicWeightedRandomSampler` を使います。使い方は `mylib/heuristic/weighted_random_sampler.md` を参照します。

AHC のパラメータチューニングに Optuna を使う場合は `.venv/bin/python scripts/optuna_tune.py` を使います。依存関係は `requirements-optuna.txt`、セットアップは `scripts/setup_optuna.sh` で行います。`--param NAME:int:LOW:HIGH[:STEP]` などで探索空間を定義し、各 trial の `-DNAME=value` は `CXXFLAGS` と `OPTUNA_DEFINES` に渡されます。

AHC の実装で、キリがよく有効なスコアを出したコードは、`a/milestones/` や `snapshots/` などの別ファイルに名前付きで保存する。`main.cpp` を何度も実験版で上書きし続けず、再現したい版へ戻せるようにする。`main.cpp` は常に現時点で最終提出に使う本命コードを置くファイルとして扱う。特にスコア改善実験を行う場合は、今までで最高スコアを出したコードを `main.cpp` に置き、実験用の変更は `a/snapshots/`、`a/experiments/`、または名前付きの別 `.cpp` ファイルで行う。

AHC のスコア実験では、一度実行した同一コード・同一主要コンパイルオプション・同一テストケース集合の結果を `RESULTS.md`、`experiments/results.md`、`out.txt` などに残し、再実行が不要な場合は既存結果を参照する。時間制限型のヒューリスティックで実行時間の揺れを見るために再実行する場合は、目的を明記し、同じ実験を漫然と繰り返さない。

AHC の提出用 `.cpp` は AtCoder ジャッジ上で単体コンパイルできる必要があります。`#include "mylib/..."` やワークスペース内相対 include は提出ファイルに残さず、使ったライブラリは同じ `.cpp` に貼り込みます。提出前に `python3 scripts/check_submit_cpp.py heuristic_contests/ahcXXX/a/main.cpp` を実行し、さらに `scripts/build.sh` は提出ファイルが `-I../..` なしで通る形にします。

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
- `unzip` が無い環境があるため、アーカイブ展開は原則 `python3 scripts/extract_archive.py ARCHIVE DEST` を使う。
- Web ビジュアライザの URL が `https://img.atcoder.jp/ahcXXX/name.html` の場合、ローカル版は同じ basename の `.zip` であることが多い。ただし必ず HTML やリンク文脈で確認してからダウンロードする。
- Rust ツールの `cargo run` では、Cargo への引数と実行バイナリへの引数を `--` で分ける。例: `cargo run -r --bin gen -- seeds.txt --dir ../../../in`。`cargo run -r --bin gen seeds.txt -- --dir ...` のように `--` の位置を誤らない。
- README に「seed=0..99 が同梱」と書かれていても実ファイル数を信用しない。`find ... -name '*.txt' | wc -l` で件数を確認し、100 未満なら生成する。
- Cargo が crates.io に接続できず DNS/network エラーになった場合は、同じコマンドを承認付きで再実行する。
- `statement/links.json` は空配列で上書きしない。HTML parser で `task.html` と `tasks_print.html` の両方から抽出し、抽出件数を確認する。

作業手順:

1. URL から `contest_id` と `task_id` を得る。
2. task ページと `tasks_print` ページを保存する。
3. 全リンクを抽出して `statement/links.json` に保存する。
4. HTML と `links.json` を読んで、local tools のリンクを判断する。
5. local tools を `tools/archive/` にダウンロードする。
6. `zip`、`tar.gz`、`tgz` なら `python3 scripts/extract_archive.py tools/archive/xxx tools/local/` で展開する。
7. 展開後、`README.md`、`Cargo.toml`、`src/bin`、`gen`、`tester`、`vis` などを調べる。
8. 入力生成方法を判断する。
9. まず `seed=0` で試し実行する。
10. 出力が空でなく、終了コード 0 なら成功とみなす。
11. 同じ方法で `seed=0..99` の 100 ケースを `in/0000.txt` から `in/0099.txt` に生成する。生成自体は並列でなくてもよい。生成後に必ず `find in -maxdepth 1 -type f -name '*.txt' | wc -l` で100件あることを確認する。
12. 使った生成コマンドを `scripts/generate_inputs.sh` に保存する。
13. 生成済みテストケースに対して解答、tester、scorer、visualizer などを実行するスクリプトやコマンドを用意する場合は、可能な限り `xargs -P` や GNU Parallel などでテストケースごとに並列実行する。ただし、PC 全体が重くならないように全コアは使い切らず、少なくとも数コアは空ける。`JOBS` などで並列数を指定できる場合は、論理コア数より 2〜4 程度少ない値を既定にする。
14. 最後に `tools/tool_info.json` に archive、local_dir、generator、visualizer、注意点を書き残す。
15. 判断に迷った場合は、候補を列挙して理由を説明し、ユーザーに確認する。

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

## templates/

`acc` や補助スクリプトで使うテンプレートの正本を置きます。`acc-config/` は `~/.config/atcoder-cli-nodejs` への symlink で、生成済みファイルの配置先です。テンプレートを変更するときは原則として `acc-config/` を直接編集せず、`templates/` 配下を編集してから `python3 scripts/render_acc_templates.py` を実行します。

`templates/` にはソース本体だけを置き、`template.json` は置きません。`acc-config/cpp/template.json` と `acc-config/pypy/template.json` は既存の `acc` 設定ファイルとしてそのまま使います。

現在の構成:

```text
templates/
├── cpp/
│   ├── manifest.txt       # parts/ の結合順
│   └── parts/
│       ├── 00_prelude.cpp
│       ├── 10_io.cpp
│       ├── 20_math.cpp
│       ├── 30_algorithms.cpp
│       ├── 40_data_structure.cpp
│       ├── 50_string.cpp
│       ├── 60_grid.cpp
│       ├── 70_bit.cpp
│       ├── 80_random.cpp
│       ├── 90_util.cpp
│       ├── 91_constants.cpp
│       └── 99_main.cpp
└── pypy/
    └── main.py
```

C++ テンプレートは `templates/cpp/manifest.txt` の順に `parts/` を結合し、`acc-config/cpp/main.cpp` を生成します。Python/PyPy テンプレートは `templates/pypy/main.py` から `acc-config/pypy/main.py` を生成します。

テンプレート更新後の確認:

```bash
python3 scripts/render_acc_templates.py
/opt/gcc-15.2/bin/g++ -std=c++23 -fsyntax-only acc-config/cpp/main.cpp
```

## mylib/

自作ライブラリ集です。カテゴリごとに `.cpp` ファイルを置いています。

`mylib/` 配下のライブラリ実装ファイルには、先頭行に `// 最終更新: YYYY-MM-DD HH:MM:SS JST` 形式で最終更新日時を記載する。新規作成時だけでなく、既存ライブラリのコード、コメント、説明を変更した場合も、作業完了時の日本時間へ更新する。複数のライブラリを一括更新した場合は、同一の作業時刻を記載してよい。

別のライブラリを事前に貼り付ける必要がある場合は、最終更新行の直下に `// 依存: mylib/path/to/library.cpp` と記載する。複数ある場合はカンマ区切りで、先に貼る順に列挙する。依存がない場合は記載しない。

### ライブラリの公開と参照URL

- ライブラリを新規作成・更新した際は、対象実装と関連ドキュメントを GitHub の公開リポジトリ `https://github.com/unidayo/comp` に commit / push し、完了報告にも参照URLを添える。この公開はユーザーから継続的に指示されているため、毎回の確認は不要。
- 実装ファイルの冒頭に `// 参照URL: https://github.com/unidayo/comp/blob/main/mylib/...` を記載する。配置順は最終更新日時、依存（必要な場合）、参照URLとする。既存の出典・ライセンス表記も保持する。
- コンテストでコピーする際は、コンテスト開始前に公開済みの版を使い、提出コードに `// 参照URL: https://github.com/unidayo/comp/blob/<公開済みコミットSHA>/mylib/...` という固定URLを添える。`main` のURLは更新されるため、公開時点の証明にはコピー元のコミットSHAを含むURLを使う。依存ライブラリを貼る際も各参照URLを保持する。
- 公開済みであることはローカルの更新日時や commit 日時だけでは判断せず、push の成功と、認証なしで対象の公開版を閲覧できることを確認する。コンテスト開始後の公開を開始前の公開として扱わない。
- push する変更は対象ライブラリと関連ファイルに限定し、無関係な解答・設定変更を含めない。ネットワークや認証の問題で公開できない場合は、未公開であることと残りの手順を明記し、公開完了と報告しない。

主なカテゴリ:

```text
mylib/acl/             # AtCoder Library の展開済み/提出補助用
mylib/graph/           # shortest_path, tree, connectivity など
mylib/datastructure/   # segtree, treap, range_query, trie など
mylib/algebra/         # number_theory, linear_algebra など
mylib/combinatorics/   # comb, factorial など
mylib/fps/             # formal power series
mylib/geometry/        # convex_hull, arg_sort など
mylib/string/          # matching, automaton
mylib/algorithm/       # mo, permutation, optimization, assignment など
mylib/accumulate/      # prefix_sum, imos
mylib/math/            # isqrt など
```

本物の ACL は `/usr/lib/ac-library/` にあり、CMake で include path に追加されています。ローカルでは `ATCODER` が定義されるため `#include <atcoder/all>` が通ります。Codeforces などで ACL 展開が必要な場合は `mylib/acl/*.cpp` から必要分を貼る運用です。

`ankerl::unordered_dense` はワークスペース外の `~/.local/include/ankerl/` にヘッダを置き、`#include <ankerl/unordered_dense.h>` で使います。`comp/` 直下に `unordered_dense/` のソースツリーを置かない運用です。

## notes/

競技プログラミング中に詰まったときや、公式・手法を忘れたときに参照する Markdown ノートを置きます。テーマごとに英小文字のファイル名で分けます。

現在の例:

```text
notes/binomial_coefficients.md  # 二項係数・組合せ
notes/burn_bury_min_cut.md      # 燃やす埋める・Project Selection
notes/max_flow_min_cost_flow.md # 最大流・最小費用流
notes/shortest_paths.md         # 最短経路・差分制約（牛ゲー）
```

ノートを新規作成・更新するときは、次を優先します。

- 忘れやすい公式、変形、適用条件、アルゴリズムの選択基準をまとめる。
- 典型問題をどのようにモデル化するか、実装時に間違えやすい点を書く。
- 必要に応じて計算量、制約別の使い分け、最小限のコード例を載せる。
- ワークスペース内に対応する `mylib/` があれば、場所と利用上の前提を記載する。
- 基本的すぎる題材は網羅しなくてよい。言語の初歩、自明な操作、教科書的な導入を増やすより、コンテスト中の判断や確認に役立つ題材を優先する。
- 基本事項を省くことと、説明を省略することは区別する。掲載する公式や手法については、式やコードだけを並べず、意味、適用条件、使いどころ、変形やモデル化の考え方を丁寧に説明する。
- 特に誤用しやすい前提、成立しない場合、例外、添字や辺の向きなどは明記し、初見でもその項目だけで正しく適用できる記述にする。
- 既存ノートと内容が重複する場合は、新規ファイルを増やす前に既存ノートへ追記する。

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
| `scripts/render_acc_templates.py` | `templates/` から `acc-config/` のテンプレートを生成 |
| `scripts/open_at_main.sh` | VS Code で C++/Python ファイルの `main` 行を開く補助 |

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
```

## 作業時の注意

- 新しい通常コンテストは `contests/{id}/` に作る。
- 新しいヒューリスティックコンテストは `heuristic_contests/ahcXXX/` に作る。
- 単発問題は `problems/YYYYMMDD_HHMMSS/` に作る。
- `acc` 用テンプレートは `templates/` を正本とし、変更後に `python3 scripts/render_acc_templates.py` で `acc-config/` へ反映する。`acc-config/cpp/main.cpp` など生成先だけを直接編集しない。
- 競プロの公式・アルゴリズムの参照資料は `notes/` にテーマ別の Markdown として追加する。基本的すぎる題材は省いてよいが、掲載する内容の説明は丁寧に書く。
- 解答 `.cpp` は CMake に自動登録されるため、深い階層に置いても基本的に拾われる。
- `gpu_experiments/` は CMake 管理外なので、各ディレクトリのスクリプトに従う。
- 既存のビルド成果物やキャッシュは不要に触らない。
- `mylib/acl/` は Codeforces など提出用の補助として使う。ローカルでは `/usr/lib/ac-library/` と `ATCODER` 定義がある。
- `ankerl::unordered_dense` は `~/.local/include/ankerl/` の共通ヘッダを使う。`comp/` 配下に外部ソースツリーを増やさない。
- 日本語やスペースを含むディレクトリ名があるため、シェルではパスを必ずクォートする。

## Use of Generative AI
I am currently participating in an AtCoder Heuristic Contest, and I will use this generative AI to assist in developing my solution.

When using this generative AI, the "AtCoder Heuristic Contest Generative AI Usage Rules - Version 20250616" apply.

https://info.atcoder.jp/entry/ahc-llm-rules-en

Most importantly, after running the solution program, you must not modify or improve the solution, its approach, or its code based on the execution results unless the user gives a new explicit instruction to do so.

You may run the solution program and report its execution results, logs, scores, or other observations. After reporting them, you must stop and wait for a new instruction from the user before making any improvement based on those results.

Here, "solution program" refers to any program created or being created for the purpose of solving this contest problem, regardless of whether it was created by the user or by generative AI, and regardless of whether it is still in progress or already complete.
