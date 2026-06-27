### ストーリー

本日 2月2日、日本では**節分**という伝統行事が行われる。 
節分は、季節の変わり目に邪気を払い、福を招くための行事であり、人々は 「鬼は外！」 と唱えながら炒った豆をまいて邪気（鬼）を追い払い、「福は内！」 と唱えながら福を招き入れる。

高橋くんは、そんな節分にちなんだ「鬼は外福は内ボードゲーム」を遊んでいる。 
このゲームでも、鬼を追い払い、福を呼び込むことが目的である。

できるだけ高得点を獲得するような手順を求めて欲しい。

### 問題文

N \times N のマス目からなる盤面がある。 
一番左上のマスの座標を (0,0) とし、そこから下方向に i マス、右方向に j マス進んだ先のマスの座標を (i,j) とする。 
マス (i,0),(i,1),\cdots,(i,N-1) を **行 i**、マス (0,j),(1,j),\cdots,(N-1,j) を **列 j** と呼ぶ。 
初期状態では、盤面上に **鬼** ![](https://img.atcoder.jp/ahc042/cnhLtdRT_oni.png) と **福** ![](https://img.atcoder.jp/ahc042/cnhLtdRT_fuku.png) の駒がそれぞれ 2N 個ずつ、全て異なるマスに存在している。

あなたは 1 回の操作で行または列を 1 つ選び、選んだ行/列を、左右/上下のいずれかの方向に 1 マスだけ動かすことが出来る。

- 行 i を左方向に動かす: マス (i,0) にある駒は盤面から取り除かれ、各 j=1, \dots, N-1 に対し、マス (i,j) にある駒は (i,j-1) に移動する。 

- 行 i を右方向に動かす: マス (i,N-1) にある駒は盤面から取り除かれ、各 j=0, \dots, N-2 に対し、マス (i,j) にある駒は (i,j+1) に移動する。 

- 列 j を上方向に動かす: マス (0,j) にある駒は盤面から取り除かれ、各 i=1, \dots, N-1 に対し、マス (i,j) にある駒は (i-1,j) に移動する。 

- 列 j を下方向に動かす: マス (N-1,j) にある駒は盤面から取り除かれ、各 i=0, \dots, N-2 に対し、マス (i,j) にある駒は (i+1,j) に移動する。

操作は最大で 4N^2 回まで行うことができる。 
あなたの目的は、できるだけ少ない操作回数で、福を一つも取り除くことなく、盤面上のすべての鬼を取り除くことである。

### 得点

出力した操作列における操作回数を T、最終的に盤面上に残る鬼の個数を X、盤面から取り除かれた福の個数を Y とする。 
このとき、以下の得点が得られる。

- X=Y=0 の場合、8N^2 - T 

- X>0 または Y>0 の場合、4N^2 - N (X+Y)

合計で 150 個のテストケースがあり、各テストケースの得点の合計が提出の得点となる。 
一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなる。 
コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 同じ得点を複数の参加者が得た場合、提出時刻に関わらず同じ順位となる。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
N
C_0
\vdots
C_{N-1}

```

- 盤面の大きさ N は 全てのテストケースで N=20 に固定されている。 

- 各 i=0,1,\cdots,N-1 に対し、C_i は行 i の初期状態を表す長さ N の文字列であり、その j 文字目は、次のいずれかで表される。 

- `x`: 鬼が存在する 

- `o`: 福が存在する 

- `.`: 鬼も福も存在しない

- 鬼と福の個数はそれぞれ 2N である。 

- **初期状態で鬼が存在する各マスについて、次のいずれかの条件を満たすことが保証されている。これにより、盤面上のすべての鬼を取り除き、すべての福を残すような、4N^2 手以下の操作手順が必ず存在する。** 

- 同じ列の上方向にあるすべてのマスに福が存在しない。 

- 同じ列の下方向にあるすべてのマスに福が存在しない。 

- 同じ行の左方向にあるすべてのマスに福が存在しない。 

- 同じ行の右方向にあるすべてのマスに福が存在しない。

#### ヒント

鬼が存在するマス (i,j) について、上方向に福が存在しない場合、上方向の操作を i+1 回、下方向の操作を i+1 回行うと、福を一つも取り除くことなく鬼 (i,j) を取り除くことが出来る。 
同様に、下・左・右の各方向に福が存在しない場合も、対応する操作を行うことで鬼を取り除くことができる。 
この操作を行う前後で盤面上に残る駒の位置は変化しないため、すべての鬼に対してこの操作を適用することで、福を一つも取り除くことなく、すべての鬼を取り除くことが可能である。

### 出力

t 手目の操作を以下のようにして方向を表す文字 d_t と行・列の番号を表す数値 p_t の組 (d_t, p_t) で表す。

- 行 i を左方向に動かす: (`L`, i) 

- 行 i を右方向に動かす: (`R`, i) 

- 列 j を上方向に動かす: (`U`, j) 

- 列 j を下方向に動かす: (`D`, j)

このとき、以下の形式で標準出力に出力せよ。

```text
d_0 p_0
\vdots
d_{T-1} p_{T-1}

```

出力の操作回数 T は 4N^2 以下でなければならない。 
上限を超えた場合、WA と判定される。

[例を見る](https://img.atcoder.jp/ahc042/cnhLtdRT.html?lang=ja&seed=0&output=sample)

### 入力生成方法

まず、N^2 個のマスの中から異なる 2N マスをランダムに選び、2N 個の福の駒の位置を決定する。

次に、次のいずれかの条件を満たすマスを列挙し、これを S とする。

- 同じ列の上方向にあるすべてのマスに福が存在しない。 

- 同じ列の下方向にあるすべてのマスに福が存在しない。 

- 同じ行の左方向にあるすべてのマスに福が存在しない。 

- 同じ行の右方向にあるすべてのマスに福が存在しない。

もし S の要素数が 2N より小さい場合、福の生成をやり直す。

最後に、S からランダムに 2N 個のマスを選び、鬼の駒の位置を決定する。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc042/cnhLtdRT.html?lang=ja): ローカル版より高性能でアニメーション表示と手動プレイが可能です。 

- [ローカル版](https://img.atcoder.jp/ahc042/cnhLtdRT.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc042/cnhLtdRT_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### Story

Today, on February 2nd, the traditional Japanese event **Setsubun** is being celebrated in Japan. 
Setsubun is a ritual to ward off evil spirits and invite good fortune at the turning of the seasons. 
People chant "Oni wa soto!" ("Demons out!") while throwing roasted soybeans to drive away evil spirits (oni) and chant "Fuku wa uchi!" ("Fortune gods in!") to welcome **Fukunokami**, the deities of good fortune.

Takahashi is playing a board game called **"Oni wa Soto, Fuku wa Uchi"**, inspired by the Setsubun tradition. 
The objective of this game is also to drive away demons and invite Fukunokami.

Find a sequence of moves that maximizes the score.

### Problem Statement

There is an N\times N square board. 
Let (0, 0) be the coordinates of the top-left square, and (i, j) be the coordinates of the square located i squares down and j squares to the right from there. 
The set of squares (i,0), (i,1), \dots, (i,N-1) is referred to as **row i**, and the set of squares (0,j), (1,j), \dots, (N-1,j) is referred to as **column j**.

Initially, there are **Oni (demons)** ![](https://img.atcoder.jp/ahc042/cnhLtdRT_oni.png) and **Fukunokami (fortune gods)** ![](https://img.atcoder.jp/ahc042/cnhLtdRT_fuku.png) pieces on the board, with 2N pieces of each type, all placed on distinct squares.

You can select one row or column in a single operation and shift the chosen row/column by one square in either the left/right or up/down direction.

- Shifting row i to the left: The piece on square (i,0) is removed from the board, and for each j=1, \dots, N-1, the piece on square (i,j) moves to (i,j-1). 

- Shifting row i to the right: The piece on square (i,N-1) is removed from the board, and for each j=0, \dots, N-2, the piece on square (i,j) moves to (i,j+1). 

- Shifting column j upward: The piece on square (0,j) is removed from the board, and for each i=1, \dots, N-1, the piece on square (i,j) moves to (i-1,j). 

- Shifting column j downward: The piece on square (N-1,j) is removed from the board, and for each i=0, \dots, N-2, the piece on square (i,j) moves to (i+1,j).

You can perform up to 4N^2 operations.

Your goal is to remove all the Oni from the board using as few moves as possible, without removing any Fukunokami.

### Scoring

Let T be the number of operations in the output sequence, X be the number of Oni remaining on the board, and Y be the number of Fukunokami removed from the board. 
Then, you will obtain the following score.

- If X=0 and Y=0, the score is 8N^2 - T. 

- If X>0 or Y>0, the score is 4N^2 - N (X+Y).

There are 150 test cases, and the score of a submission is the total score for each test case. 
If your submission produces an illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
The highest score obtained during the contest will determine the final ranking, and there will be no system test after the contest. 
If more than one participant gets the same score, they will be ranked in the same place regardless of the submission time.

### Input

Input is given from Standard Input in the following format:

```text
N
C_0
\vdots
C_{N-1}

```

- The board size N is fixed at N=20 for all test cases. 

- For each i=0,1,\dots,N-1, C_i represents the initial state of row i as a string of length N, where the j-th character is one of the following: 

- `x`: An Oni is present. 

- `o`: A Fukunokami is present. 

- `.`: Neither an Oni nor a Fukunokami is present.

- There are exactly 2N Oni and 2N Fukunokami on the board. 

- **For every square initially occupied by an Oni, at least one of the following conditions is guaranteed to hold. This ensures that there always exists a sequence of at most 4N^2 moves that removes all Oni while keeping all Fukunokami on the board.** 

- All squares above it in the same column do not contain a Fukunokami. 

- All squares below it in the same column do not contain a Fukunokami. 

- All squares to the left of it in the same row do not contain a Fukunokami. 

- All squares to the right of it in the same row do not contain a Fukunokami.

#### Hint

For a square (i,j) occupied by an Oni, if there is no Fukunokami in the upward direction, performing an upward shift i+1 times followed by a downward shift i+1 times will remove the Oni at (i,j) without removing any Fukunokami. 
Similarly, if there are no Fukunokami in the downward, leftward, or rightward directions, the Oni can be removed using the corresponding sequence of moves.

Since this operation does not change the positions of the remaining pieces on the board, applying it to all Oni ensures that all Oni are removed without removing any Fukunokami.

### Output

Each operation at step t is represented as a pair (d_t, p_t), where d_t is a character indicating the direction and p_t is an integer indicating the row or column index.

- Shifting row i to the left: (`L`, i) 

- Shifting row i to the right: (`R`, i) 

- Shifting column j upward: (`U`, j) 

- Shifting column j downward: (`D`, j)

Then, output to Standard Output in the following format:

```text
d_0 p_0
\vdots
d_{T-1} p_{T-1}

```

The number of operations T must not exceed 4N^2. 
If the limit is exceeded, the output will be judged as WA.

[Show example](https://img.atcoder.jp/ahc042/cnhLtdRT.html?lang=en&seed=0&output=sample)

### Input Generation

First, 2N distinct squares are randomly selected from the N^2 squares on the board to determine the positions of the 2N Fukunokami pieces.

Next, all squares satisfying at least one of the following conditions are listed, forming the set S:

- All squares above it in the same column do not contain a Fukunokami. 

- All squares below it in the same column do not contain a Fukunokami. 

- All squares to the left of it in the same row do not contain a Fukunokami. 

- All squares to the right of it in the same row do not contain a Fukunokami.

If the number of elements in S is less than 2N, the Fukunokami placement process is repeated.

Finally, 2N squares are randomly selected from S to determine the positions of the 2N Oni pieces.

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc042/cnhLtdRT.html?lang=en): This is more powerful than the local version providing animations and manual play. 

- [Local version](https://img.atcoder.jp/ahc042/cnhLtdRT.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc042/cnhLtdRT_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

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

- [ルール](https://atcoder.jp/contests/ahc042/rules)
 
- [用語集](https://atcoder.jp/contests/ahc042/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
