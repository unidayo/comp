### ストーリー

動物が大好きな高橋社長はAtCoder社のオフィスでペットを何匹も放し飼いにしている。 
AtCoder社の社員たちはペットに邪魔されて仕事に集中できないことに困っており、オフィスに仕切りを設置してペットの入ってこないスペースを確保することにした。 
出来るだけ広いスペースを確保してほしい。

### 問題文

30 \times 30 マスからなる部屋の中に、N 匹のペットと M 人の人間が居る。 
各マスは初期状態で全て通行可能であり、30 \times 30 マスの外部は全て通行不能である。 
上から x 行目(1\leq x\leq 30)、左から y 列目(1\leq y\leq 30)のマスの座標を (x, y) とする。 
以下の処理を 300 ターン繰り返す。

まず各人の行動をそれぞれ以下の3種類から選択し、同時に動く。

- 何もせずにその場にとどまる。 

- 現在位置に隣接するマスを選んで通行不能にする。このターンの開始時点でペットもしくは人が居るマスを選ぶことは出来ない。**隣接するマスにペットが居るマスを選ぶことも出来ない。**既に通行不能なマスを選んだ場合は何もしない。 

- 現在位置に隣接する通行可能マスに移動する。このターンの行動で他の人によって通行不能にされるマスに移動することは出来ない。

そのターンの人間の行動が完了後、各ペットが独立に移動する。 
ペットの移動規則はペットの種類ごとに異なり、1ターンの間に複数マスの移動をする場合もある。 
詳細については後ほど述べる。

既に人やペットが居るマスも通行可能で、同じマスに何体でも重なることが出来る。

### 得点

300 ターン終了時点で、各 i=1,\cdots,M について、人 i の最終位置から通行可能マスのみを通って到達可能なマスの集合を R_i、 
最終位置が R_i に含まれるペットの総数を n_i とする。 
このとき、人 i は s_i=\frac{|R_i|}{900}\times 2^{-n_i} の満足度を得る。 
テストケースに対して得られる得点は \mathrm{round}\left(10^8\times\frac{1}{M}\sum_{i=1}^M s_i\right) である。

#### テストケース数

- 暫定テスト 100個 

- システムテスト 2000個、 コンテスト終了後に[seeds.txt](https://img.atcoder.jp/ahc008/seeds.txt) (md5=27bf0702bbe0265900374c3b6b9846b4, sha256=33973e4ded08e3a607fc2e841e14751ff110ae10154b286e7fd5f766ff86d706) を公開

各テストケースの得点の合計が提出の得点となる。 
暫定テストでは、一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなる。 
システムテストでは、不正な出力や制限時間超過をした場合、そのテストケースのみ0点となる。 
提出プログラムが異常終了した場合、 RE ではなく WA と判定される可能性があるので注意せよ。

#### 実行時間について

実行時間には多少のブレが生じます。 
また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
そのため、実行時間制限ギリギリの提出がシステムテストでTLEとなる可能性があります。 
プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入出力

まずはじめに、各ペットの初期位置と種類、各人間の初期位置が以下の形式で標準入力から与えられる。

```text
N
px_1 py_1 pt_1
\vdots
px_N py_N pt_N
M
hx_1 hy_1
\vdots
hx_M hy_M

```

N はペットの数を表す 10 以上 20 以下の整数値である。 
(px_i,py_i) は i 番目のペットの初期位置の座標を表し、pt_i は i 番目のペットの種類を表す 1 以上 5 以下の整数値である。 
M は人間の数を表す 5 以上 10 以下の整数値である。 
(hx_i,hy_i) は i 番目の人間の初期位置の座標を表す。 
全てのペット・人間の初期位置は異なることが保証されている。

上記の情報を読み込んだら、以下の処理を 300 回繰り返す。

まず、各人間の行動を以下のように1文字で表し、M 人の行動を i 文字目が i 番目の人間の行動を表す長さ M の文字列として標準出力に一行で出力せよ。 
**出力のあとは標準出力を flush しなければならない。**そうしない場合、TLEとなる可能性がある。

- `.`: 何もしない 

- `u`, `d`, `l`, `r`: 現在位置を(x,y)としたとき、それぞれ (x-1,y)、(x+1,y)、(x,y-1)、(x,y+1) のマスを通行不能にする。 

- `U`, `D`, `L`, `R`: 現在位置を(x,y)としたとき、それぞれ (x-1,y)、(x+1,y)、(x,y-1)、(x,y+1) のマスに移動する。

出力後、N個の文字列がスペース区切りで標準入力に一行で与えられる。 
i 番目の文字列は i 番目のペットのそのターンの移動を表し、そのターンに移動しない場合は `.`、移動する場合は上下左右への1マスの移動をそれぞれ `U`, `D`, `L`, `R` で表して繋げたものである。

[例を見る](https://img.atcoder.jp/ahc008/f828b9475ffb41d54f05619db6ccbd4f.html?lang=ja&show=example)

### ペットの移動規則

基本行動を次のように定義する。 
隣接する通行可能マスの中から一様ランダムに選んだマスへ移動する。(通行不能に出来るマスの条件から、そのようなマスは常に存在する)

各ペット i は、種類を表す 1 以上 5 以下の整数値 pt_i に応じて、以下のように行動する。

- ![](https://img.atcoder.jp/ahc008/cow.png) 牛: 基本行動を1回行う。 

- ![](https://img.atcoder.jp/ahc008/pig.png) 豚: 基本行動を2回行う。 

- ![](https://img.atcoder.jp/ahc008/rabbit.png) 兎: 基本行動を3回行う。 

- ![](https://img.atcoder.jp/ahc008/dog.png) 犬: 以下のようにして目的の人に向かって進む。1ターン目は目的無しの状態から開始。現在の目的が無い、現在位置に目的の人が居る、もしくは目的の人への移動経路が存在しない場合は、現在位置から到達可能な人(現在位置に居る人は除く)の中から一様ランダムに一人選ぶ。そのような人が存在しない場合は目的なしの状態になり、基本行動を1回行う。目的の人が居る場合は、目的の人の現在位置への最短距離が短くなる方向(複数ある場合はその中から一様ランダムに選択)へ1マス移動してから基本行動を1回行う。1回目もしくは2回目の移動後に目的地に到達した場合は目的無しの状態に戻す。 

- ![](https://img.atcoder.jp/ahc008/cat.png) 猫: 以下のようにして目的地に向かって進む。1ターン目は目的無しの状態から開始。現在の目的が無い、もしくは目的地への移動経路が存在しない場合は、現在位置から到達可能なマス(現在位置を除く)の中から一様ランダムに目的地を一つ選ぶ。目的地への最短距離が短くなる方向(複数ある場合はその中から一様ランダムに選択)へ1マス移動してから基本行動を1回行う。1回目もしくは2回目の移動後に目的地に到達した場合は目的無しの状態に戻す。

### 入力生成方法

L 以上 U 以下の整数値を一様ランダムに生成する関数を \mathrm{rand}(L,U) で表す。

ペットの数は N=\mathrm{rand}(10, 20) により生成する。 
各ペットの初期位置は、まだ選ばれていない座標の中から一様ランダムに選択する。 
各ペットの種類は pt_i=\mathrm{rand}(1, 5) により生成する。

人間の数は M=\mathrm{rand}(5, 10) により生成する。 
各人間の初期位置は、まだ選ばれていない座標の中から一様ランダムに選択する。

### ツール 

- [ローカルテスタ](https://img.atcoder.jp/ahc008/tools_v3.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- Rust言語の環境構築が面倒な方向けに、Windows用のコンパイル済みバイナリを用意しました。[tools_x86_64-pc-windows-gnu.zip](https://img.atcoder.jp/ahc008/tools_x86_64-pc-windows-gnu_v3.zip) 

- 一番最初のものは猫の移動にバグがあったため、コンテスト開始130分時点で修正が入っています。再ダウンロードして下さい。 

- READMEの実行例を充実させました。使い方が分からない方は参考にして下さい。また、ルールにあるとおり、提供されたツール類の動かし方に関する情報は自由に共有して構いません。

- [Web版ビジュアライザ](https://img.atcoder.jp/ahc008/f828b9475ffb41d54f05619db6ccbd4f.html?lang=ja): ローカルテスタが生成する出力ファイルの中身をOutput欄に貼り付けることで、実行結果のアニメーション表示が可能です。

コンテスト期間中に、seed=0 に対するビジュアライザの出力画像(pngもしくはgif)のみ twitter で共有が可能です。必ず指定されたハッシュタグを用い、公開アカウントを使用して下さい。(追記)共有出来るのはseed=0に対するビジュアライズ結果と点数のみで、他のシードでの点数や解法や考察に関する言及は禁止です。[共有された画像の一覧](https://twitter.com/search?q=%23AHC008%20%23visualizer&src=typed_query&f=live)

#### ツールで用いられる入出力ファイルの仕様

ローカルテスタに与える入力ファイルとして、事前情報である各ペットの初期位置と種類、各人間の初期位置の末尾に、ペットの移動を生成するための乱数シード値を追記したものを用いている。 
ペットの移動は人間の行動に依存するため、入力ファイルには乱数シード値のみが記載され、具体的な移動は含まれない。 
ローカルテスタは解答プログラムの出力をそのまま出力ファイルに書き出す。 
解答プログラムは、`#` から始まるコメント行を出力に含めても良い。 
Web版ビジュアライザを使用すると、コメント行が出力されたタイミングで表示されるため、デバッグや考察等に役立てることが出来る。 
ジャッジプログラムはコメント行を全て無視するため、コメント行を出力するプログラムをそのまま提出可能である。

### Story

AtCoder's CEO, Takahashi, loves animals and has a number of pets running free in the AtCoder office. 
AtCoder's employees have trouble with the pets interrupting their work, so they have decided to place partitions in the office to create a space where pets cannot come in. 
Please create as large a space as possible.

### Problem Statement

There are N pets and M people in a room with a floor of 30 \times 30 squares. 
All squares are initially passable, and outside of the 30 \times 30 squares are impassable. 
Let (x, y) be the coordinates of the square in row x from the top (1\leq x\leq 30) and column y from the left (1\leq y\leq 30). 
Repeat the following process for 300 turns.

First, you choose each person's action from the following three types, and perform each action simultaneously.

- Do nothing and stay in the current position. 

- Choose a square adjacent to the current position and make it impassable. You cannot choose a square that contains pets or humans at the start of this turn. **You cannot choose a square whose adjacent square contains a pet, either.** If you choose a square that is already impassable, nothing happens. 

- Move to an adjacent passable square. It is not possible to move to a square that becomes impassable by another person's action in this turn.

After all the people have completed their actions for that turn, each pet moves independently. 
Rules for pet movement depend on the type of pet, and some pets may move multiple squares in a single turn. 
Details are described later.

Squares containing humans or pets are also passable, and each square can contain any number of humans and pets.

### Scoring

At the end of 300 turn, for each i=1,\cdots,M, let R_i be the set of squares reachable from the final position of person i through only passable squares, and n_i be the number of pets whose final position is in R_i. 
Then, person i obtains satisfaction of s_i=\frac{|R_i|}{900}\times 2^{-n_i}. 
The score for the test case is \mathrm{round}\left(10^8\times\frac{1}{M}\sum_{i=1}^M s_i\right).

#### Number of test cases

- Provisional test: 100 

- System test: 2000. We will publish [seeds.txt](https://img.atcoder.jp/ahc008/seeds.txt) (md5=27bf0702bbe0265900374c3b6b9846b4, sha256=33973e4ded08e3a607fc2e841e14751ff110ae10154b286e7fd5f766ff86d706) after the contest is over.

The score of a submission is the total scores for each test case. 
In the provisional test, if your submission produces illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
In the system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero. 
Note that if your program terminates abnormally, it may be judged as WA instead of RE.

#### About execution time

Execution time may vary slightly from run to run. 
In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percent compared to provisional tests. 
For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
Please measure the execution time in your program to terminate the process, or have enough margin in the execution time.

### Input and Output

First, the initial position and type of each pet, and the initial position of each person are given from Standard Input in the following format

```text
N
px_1 py_1 pt_1
\vdots
px_N py_N pt_N
M
hx_1 hy_1
\vdots
hx_M hy_M

```

N is an integer between 10 and 20 representing the number of pets. 
(px_i,py_i) represents the coordinates of the initial position of the i-th pet, and pt_i is an integer between 1 and 5 representing the type of the i-th pet. 
M is an integer between 5 and 10 representing the number of humans. 
(hx_i,hy_i) represents the coordinates of the initial position of the i-th human. 
The initial positions of all pets and humans are guaranteed to be distinct.

After reading the above information, repeat the following process 300 turns.

First, output a string of length M where the i-th character represents the action of the ith person as follows on a single line to Standard Output. 
**After the output, you have to flush Standard Output.** Otherwise, the submission might be judged as TLE .

- `.`: Do nothing and stay in the current position. 

- `u`, `d`, `l`, `r`: Let (x,y) be the current position. Make the square (x-1,y), (x+1,y), (x,y-1), or (x,y+1) impassable, respectively. 

- `U`, `D`, `L`, `R`: Let (x,y) be the current position. Move to the the square (x-1,y), (x+1,y), (x,y-1), or (x,y+1), respectively.

After the output, N strings are given to Standard Input in a single line, separated by spaces. 
The i-th string represents movement of the i-th pet in that turn. 
If the pet does not move, the string is `.`. 
If it does move, the string is a sequence of characters `U`, `D`, `L`, and `R` representing the movement of one square up, down, left, and right, respectively.

[Show example](https://img.atcoder.jp/ahc008/f828b9475ffb41d54f05619db6ccbd4f.html?lang=en&show=example)

### Pets Movement Rules

We define a basic move as follows: move to a square chosen at random among the adjacent passable squares. From the condition of the squares that can be made impassable, such squares always exist.

Each pet i performs the following moves depending on pt_i, an integer value between 1 and 5 representing its type.

- ![](https://img.atcoder.jp/ahc008/cow.png) Cow: Perform one basic move. 

- ![](https://img.atcoder.jp/ahc008/pig.png) Pig: Perform two basic moves. 

- ![](https://img.atcoder.jp/ahc008/rabbit.png) Rabbit: Perform three basic moves. 

- ![](https://img.atcoder.jp/ahc008/dog.png) Dog: Move toward a target person as follows. The first turn starts with no target. If it has no target, the target person is in the current position, or there exists no path to the target person, then it selects one person uniformly at random among those reachable from the current position, excluding those in the current position. If there is no such person, reset to no target and perform one basic move. Otherwise, move to an adjacent passable square that shortens the shortest distance to the target person (if there are multiple such squares, choose one of them uniformly at random), and then perform one basic move. If it reaches the destination after the first or the second move, reset to no target. 

- ![](https://img.atcoder.jp/ahc008/cat.png) Cat: Move toward a target square as follows. The first turn starts with no target. If it has no target or there exists no path to the target square, then it selects one square uniformly at random among those reachable from the current position, excluding the current position. If there exists no such square, do nothing. Otherwise, move to an adjacent passable square that shortens the shortest distance to the target square (if there are multiple such squares, choose one of them uniformly at random), and then perform one basic move. If it reaches the destination after the first or the second move, reset to no target.

### Input Generation

Let \mathrm{rand}(L,U) be a function that generates a uniform random integer between L and U, inclusive.

We generate the number of pets by N=\mathrm{rand}(10, 20). 
The initial position of each pet is chosen uniformly at random from the coordinates that have not been chosen yet. 
We generate the type of each pet by pt_i=\mathrm{rand}(1, 5).

We generate the number of humans by M=\mathrm{rand}(5, 10). 
The initial position of each human is chosen uniformly at random from the coordinates that have not been chosen yet.

### Tools 

- [Local tester](https://img.atcoder.jp/ahc008/tools_v3.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- For those who are not familiar with the Rust language environment, we have prepared a pre-compiled binary for Windows. [tools_x86_64-pc-windows-gnu.zip](https://img.atcoder.jp/ahc008/tools_x86_64-pc-windows-gnu_v3.zip) 

- The first version contained a bug in the cat's movement, which has been fixed at 130 minutes after the contest started. Please re-download it. 

- We have added more examples in README. If you don't know how to use the tools, please refer to README. Also, as stated in the rules, you are free to share information on how to run the provided tools.

- [Web visualizer](https://img.atcoder.jp/ahc008/f828b9475ffb41d54f05619db6ccbd4f.html?lang=en): By pasting the output generated by the local tester into the Output field, you can display the animation of the execution result.

You are allowed to share output images (png or gif) of the provided visualizer for seed=0 on twitter during the contest. You have to use the specified hashtag and public account. You can only share visualization results and scores for seed=0. Do not share scores for other seeds or mention solutions or discussions. [List of shared images.](https://twitter.com/search?q=%23AHC008%20%23visualizer&src=typed_query&f=live)

#### Specification of input/output files used by the tools

Input files for the local tester consist of the prior information (the initial position and type of each pet, and the initial position of each person) followed by a random seed value to generate pet movements. 
Since the pet's movement depends on human actions, the input file contains only the random seed value and not specific movements. 
The local tester writes outputs from your program directly to the output file. 
Your program may output comment lines starting with `#`. 
The web version of the visualizer displays the comment lines at the time they are output, which may be useful for debugging and analysis. 
Since the judge program ignores all comment lines, you can submit a program that outputs comment lines as is.

 言語

 ><> (fishr 0.1.0)
 
 Ada 2022 (GNAT 15.2.0)
 
 APL (GNU APL 1.9)
 
 Assembly MIPS O32 ABI (GNU assembler 2.42)
 
 Assembly x64 (NASM 2.16.03)
 
 AWK (GNU awk 5.2.1)
 
 A言語 (interpreter af48a2a)
 
 Bash (bash 5.3)
 
 BASIC (FreeBASIC 1.10.1)
 
 bc (GNU bc 1.08.2)
 
 Befunge93 (TBC 1.0)
 
 Brainfuck (Tritium 1.2.73)
 
 C23 (Clang 21.1.0)
 
 C23 (GCC 14.2.0)
 
 C# 13.0 (.NET 9.0.8)
 
 C# 13.0 (.NET Native AOT 9.0.8)
 
 C++23 (GCC 15.2.0)
 
 C3 (c3c 0.7.5)
 
 Carp(Carp 0.5.5)
 
 cLay (cLay 20250308-1 (GCC 15.2.0))
 
 Clojure (babashka 1.12.208)
 
 Clojure (clojure 1.12.2)
 
 Clojure (Clojure AOT 1.12.2)
 
 Clojure (ClojureScript 1.12.42 (Clojure 1.12.2 Node.js 22.19.0))
 
 COBOL (Free) (GnuCOBOL 3.2)
 
 Common Lisp (SBCL 2.5.8)
 
 Crystal (Crystal 1.17.0)
 
 Cyber (Cyber v0.3)
 
 D (DMD 2.111.0)
 
 D (GDC 15.2)
 
 D (LDC 1.41.0)
 
 Dart (Dart 3.9.2)
 
 dc 1.5.2 (GNU bc 1.08.2)
 
 ECLiPSe (ECLiPSe 7.1_13)
 
 Eiffel (Gobo Eiffel 22.01)
 
 Eiffel (Liberty Eiffel 07829e3)
 
 Elixir (Elixir 1.18.4 (OTP 28.0.2))
 
 Emacs Lisp(Native Compile)(GNU Emacs 29.4)
 
 Emojicode 1.0 beta 2 (emojicodec 1.0 beta 2)
 
 Erlang (Erlang 28.0.2)
 
 F# 9.0 (.NET 9.0.8)
 
 Factor (Factor 0.100)
 
 Fish (fish 4.0.2)
 
 Forth (gforth 0.7.3)
 
 Fortran2018 (Flang 20.1.7)
 
 Fortran2023 (GCC 14.2.0)
 
 FORTRAN77 (GCC 14.2.0)
 
 Gleam (Gleam 1.12.0 (OTP 28.0.2))
 
 Go 1.18 (gccgo 15.2.0)
 
 Go (go 1.25.1)
 
 Haskell (GHC 9.8.4)
 
 Haxe/JVM 4.3.7 (hxjava 4.2.0)
 
 C++ IOI-Style(GNU++20) (GCC 14.2.0)
 
 ISLisp (Easy-ISLisp 5.43)
 
 Java24 (OpenJDK 24.0.2)
 
 JavaScript (Bun 1.2.21)
 
 JavaScript (Deno 2.4.5)
 
 JavaScript (Node.js 22.19.0)
 
 Jule (jule 0.1.6)
 
 Koka (koka v3.2.2)
 
 Kotlin (Kotlin/JVM 2.2.10)
 
 Kuin (kuincl v.2021.8.17)
 
 Lazy K (irori v1.0.0)
 
 Lean (lean v4.22.0)
 
 LLVM IR (Clang 21.1.0)
 
 Lua (Lua 5.4.7)
 
 Lua (LuaJIT 2.1.1703358377)
 
 Mercury (Mercury 22.01.8)
 
 Nim (Nim 1.6.20)
 
 Nim (Nim 2.2.4)
 
 OCaml (ocamlopt 5.3.0)
 
 Octave (GNU Octave 10.2.0)
 
 Pascal (fpc 3.2.2)
 
 Perl (perl 5.38.2)
 
 PHP (PHP 8.4.12)
 
 Piet (your-diary/piet_programming_language 3.0.0) (PPM image)
 
 Pony (ponyc 0.59.0)
 
 PowerShell (PowerShell 7.5.2)
 
 Prolog (SWI-Prolog 9.2.9)
 
 Python (CPython 3.13.7)
 
 Python (PyPy 3.11-v7.3.20)
 
 R (GNU R 4.5.0)
 
 ReasonML (reson 3.16.0)
 
 Ruby 3.3 (truffleruby 25.0.0)
 
 Ruby 3.4 (ruby 3.4.5)
 
 Rust (rustc 1.89.0)
 
 SageMath (SageMath 10.7)
 
 Scala (Dotty 3.7.2)
 
 Scala 3.7.2 (Scala Native 0.5.8)
 
 Scheme (ChezScheme 10.2.0)
 
 Scheme (Gauche 0.9.15)
 
 Seed7 (Seed7 3.5.0)
 
 Swift 6.2
 
 Tcl (tclsh 9.0.1)
 
 Terra (Terra 1.2.0)
 
 TeX (tex 3.141592653)
 
 Text (cat 9.4)
 
 TypeScript 5.8 (Deno 2.4.5)
 
 TypeScript 5.9 (tsc 5.9.2 (Bun 1.2.21))
 
 TypeScript 5.9 (tsc 5.9.2 (Node.js 22.19.0))
 
 Uiua (uiua 0.16.2)
 
 Unison (Unison 0.5.47)
 
 V (0.4.10)
 
 Vala (valac 0.56.18)
 
 Verilog 2012 (Icarus Verilog 12.0)
 
 Veryl (veryl 0.16.4)
 
 WebAssembly (wabt 1.0.34 + iwasm 2.4.1)
 
 Whitespace (whitespacers 1.3.0)
 
 Zig (Zig 0.15.1)
 
 なでしこ (cnako3 3.7.8 (Node.js 22.19.0))
 
 プロデル (mono版プロデル 2.0.1353)
 
 Julia (Julia 1.11.6)
 
 Python (Codon 0.19.3)
 
 C++23 (Clang 21.1.0)
 
 Fix (1.1.0-alpha.12)
 
 SQL (DuckDB 1.3.2)

 var currentLang = getLS('defaultLang');

 ソースコード

    ファイルを開く

    カスタマイズ

 エディタ切り替え

 高さ自動調節

 ※ 512 KiB まで

 提出

 []
 []
 
 []
 
 [](https://www.addtoany.com/share)

- [ルール](https://atcoder.jp/contests/ahc008/rules)
 
- [用語集](https://atcoder.jp/contests/ahc008/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
