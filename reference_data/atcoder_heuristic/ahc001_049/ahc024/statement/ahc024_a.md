### ストーリー

高橋市の高橋市長は市役所のロビーの床に色の付いた正方形タイルを用いて高橋市の地図を描くことにした。 
高橋市はいくつかの区に別れており、この地図では各区は同じ色の連結なタイルの集合として表現する。 
業者に依頼して正確な地図の案を作成したところ、使用するタイル数が多く、予算オーバーとなってしまった。 
グラフが大好きな高橋市長は、区の間の隣接関係にしか興味がなく、各区の形状や大きさなど隣接関係以外の情報を無視すればより少ないタイル数で地図が描けるのではと考えている。 
使用するタイル数が出来るだけ少ない地図を作成して欲しい。

 ![](https://img.atcoder.jp/ahc024/424c43d223359f34250d50550689967b.png)

正確な地図

 ![](https://img.atcoder.jp/ahc024/e490f6d700b89293edfe2c9288d0e54a.png)

隣接関係を正しく表現した小さな地図

### 問題文

n\times n マスのグリッド上に表現された高橋市の地図が与えられる。 
一番左上のマスの座標を (0,0) とし、そこから下方向に i マス、右方向に j マス移動した先のマスの座標を (i,j) とする。 
高橋市は m 個の区からなり、c (1\leq c\leq m) 番の色で塗られたマスは c 番の区に対応する。 
n\times n マスの外は市の外部であり、0 番の色で塗られているものとする。

2つのマスは辺を共有する場合に「隣接している」と定義し、マス集合は隣接するマスを経由して互いに到達可能な場合に「連結である」と定義される。 
与えられた地図は、各色について、その色で塗られたマス集合は連結であることが保証されている。

n\times n マスのグリッド上に表現された地図であって、以下の条件を全て満たすものを作成せよ。

- 全ての色 c (0\leq c\leq m) について、色 c のマスは連結である。ただし、n\times n マスの外部は色 0 で塗られているため、色 0 のマスは外部のマスを経由して連結であれば良い。 

- 全ての色 c,d (0\leq c<d\leq m) に対して、元の地図と作成した地図で色 c のマス集合と色 d のマス集合の隣接関係が等しい。すなわち、元の地図で色 c で塗られたマスに隣接する色 d で塗られたマスが存在する場合、そしてその場合に限り、作成した地図においても 色 c のマスとそれに隣接する色 d のマスが存在する。ただし、n\times n マスの外部は全て色 0 で塗られているため、外周上のマスは色 0 のマスと隣接しているとみなす。

### 得点

作成した地図に含まれる色 0 のマスの総数を E としたとき、E+1 の得点が得られる。

テストケースは全部で 150 個あり、各テストケースの得点の合計が提出の得点となる。 
一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなる。 
コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 同じ得点を複数の参加者が得た場合、提出時刻に関わらず同じ順位となる。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
n m
c_{0,0} c_{0,1} \cdots c_{0,n-1}
\vdots
c_{n-1,0} c_{n-1,1} \cdots c_{n-1,n-1}

```

全てのテストケースで n = 50, m = 100 で固定である。 
c_{i,j} は座標 (i,j) のマスの色を表す整数値であり、1\leq c_{i,j}\leq m を満たす。 
各 k=1,2,\cdots,m に対して、c_{i,j}=k であるような (i,j) が一つ以上存在する。

### 出力

作成した地図における座標 (i,j) のマスの色を d_{i,j} (0\leq d_{i,j}\leq m) としたとき、以下の形式で標準出力に出力せよ。

```text
d_{0,0} d_{0,1} \cdots d_{0,n-1}
\vdots
d_{n-1,0} d_{n-1,1} \cdots d_{n-1,n-1}

```

出力された地図が問題文に記された条件を満たさない場合、WA と判定される。

プログラムは解を複数回出力しても良い。 
複数回出力された場合、一番最後の解のみが採点に用いられる。 
Web版のビジュアライザを使用すると、複数の解の比較が可能である。

[例を見る](https://img.atcoder.jp/ahc024/AU5KcDyn.html?lang=ja&seed=0&output=sample)

### 入力生成方法

まずはじめに、全ての (i,j) に対して c_{i,j}=0 で初期化する。 
次に、各 k=1,2,\cdots,m について、c_{i,j}=0 であるマスの中からランダムに一つ選んで c_{i,j}=k と設定する。 
最後に、以下の処理を c_{i,j}=0 であるマスが無くなるまで繰り返す。

c_{i,j}=0 であるマスをランダムに一つ選び、そのマスに隣接するマス (i',j') をランダムに選ぶ。 
c_{i,j}=c_{i',j'} と設定する。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc024/AU5KcDyn.html?lang=ja): ローカル版より高性能でアニメーション表示と手動プレイが可能です。 

- [ローカル版](https://img.atcoder.jp/ahc024/AU5KcDyn.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc024/AU5KcDyn_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### Story

Mr. Takahashi, the mayor of Takahashi City, decided to draw a map of Takahashi City on the floor of the city hall lobby using colored square tiles. 
Takahashi City is divided into several wards, and in this map, each ward should be represented as a set of connected tiles of the same color. 
He commissioned a contractor to create a draft of an accurate map, but the number of tiles to be used was too large, and the budget was exceeded. 
Mayor Takahashi, who loves graphs, is only interested in the adjacencies between the wards and thinks that the map could be drawn with fewer tiles if information other than adjacencies, such as the shape and size of each ward, is ignored. 
Please create a map using as few tiles as possible.

 ![](https://img.atcoder.jp/ahc024/424c43d223359f34250d50550689967b.png)

Accurate map

 ![](https://img.atcoder.jp/ahc024/e490f6d700b89293edfe2c9288d0e54a.png)

Small map correctly representing adjacencies

### Problem Statement

Given a map of Takahashi City represented on a grid of n\times n squares. 
Let (0,0) be the coordinates of the top-left square, and (i,j) be the coordinates of the square located i squares down and j squares to the right from there. 
The city consists of m wards, and the square of color c (1\leq c\leq m) corresponds to the c-th ward. 
The outside of the n\times n squares correspond to the outside of the city and is colored 0.

Two squares are defined as "adjacent" if they share an edge, and a set of squares is defined as "connected" if any two squares can reach each other via adjacent squares. 
In the given map, for each color c, the set of squares of color c is guaranteed to be connected.

Your task is to create a map represented on a grid of n\times n squares that satisfies all of the following conditions.

- For every color c (0\leq c\leq m), squares of color c must be connected. Note that since the outside of the n\times n squares is colored 0, squares of color 0 can be connected through the outside squares. 

- For every pair of colors c and d (0\leq c<d\leq m), the adjacency of a set of squares of color c and a set of squares of color d in the original map and the created map must be identical. That is, if and only if there exist adjacent squares of color c and d in the original map, there exist adjacent squares of color c and d in the created map. Note that since the outside of the n\times n squares is colored 0, the squares on the boundary are considered to be adjacent to squares of color 0.

### Scoring

Let E be the total number of squares of color 0 in the created map. 
Then you will obtain a score of E+1.

There are 150 test cases, and the score of a submission is the total score for each test case. 
If your submission produces an illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
The highest score obtained during the contest will determine the final ranking, and there will be no system test after the contest. 
If more than one participant gets the same score, they will be ranked in the same place regardless of the submission time.

### Input

Input is given from Standard Input in the following format.

```text
n m
c_{0,0} c_{0,1} \cdots c_{0,n-1}
\vdots
c_{n-1,0} c_{n-1,1} \cdots c_{n-1,n-1}

```

For all test cases, we fix n = 50 and m = 100. 
c_{i,j} is an integer value representing the color of the square at coordinates (i,j) and satisfies 1\leq c_{i,j}\leq m. 
For every k=1,2,\cdots,m, there exists at least one (i,j) with c_{i,j}=k.

### Output

Let d_{i,j} (0\leq d_{i,j}\leq m) be the color of the square at coordinates (i,j) in the created map. 
Then, output to Standard Output in the following format.

```text
d_{0,0} d_{0,1} \cdots d_{0,n-1}
\vdots
d_{n-1,0} d_{n-1,1} \cdots d_{n-1,n-1}

```

If the output map does not satisfy the conditions specified in the problem statement, the submission will be judged as WA.

Your program may output multiple solutions. 
If multiple solutions are output, only the last one is used for scoring. 
You can compare multiple solutions using the web version of the visualizer.

[Show example](https://img.atcoder.jp/ahc024/AU5KcDyn.html?lang=en&seed=0&output=sample)

### Input Generation

First, we initialize with c_{i,j}=0 for all (i,j). 
Next, for each k=1,2,\cdots,m, we randomly select a square with c_{i,j}=0 and set c_{i,j}=k. 
Finally, we repeat the following process while squares with c_{i,j}=0 remain.

Randomly select a square with c_{i,j}=0 and randomly select its adjacent square (i',j'). 
We set c_{i,j}=c_{i',j'}.

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc024/AU5KcDyn.html?lang=en): This is more powerful than the local version providing animations and manual play. 

- [Local version](https://img.atcoder.jp/ahc024/AU5KcDyn.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc024/AU5KcDyn_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.

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

- [ルール](https://atcoder.jp/contests/ahc024/rules)
 
- [用語集](https://atcoder.jp/contests/ahc024/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
