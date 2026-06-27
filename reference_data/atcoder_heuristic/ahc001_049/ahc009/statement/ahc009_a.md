### ストーリー

下図のような地図が与えられる。 
赤い丸印に高橋くんの自宅があり、青い丸印にAtCoder社のオフィスがある。 
高橋くんはオフィスへの行き方を、下右下右のような文字列で覚えている。 
高橋くんは忘れっぽいため、覚えている文字列の一部を忘れてしまうことがある。 
たとえば、3文字目を忘れて下右右と移動した場合、オフィスにたどり着くことが出来ずに迷子になってしまう。 
そこで、一部を忘れても高確率でオフィスまでたどり着けるような、頑強な文字列を記憶することにした。 
例えば、下右下右下右という文字列を覚えておけば、どの1文字を忘れたとしても無事オフィスにたどり着くことが出来る。 
オフィスへ到達出来る確率が高く、かつ出来るだけ早くオフィスに到達出来るような文字列を求めてほしい。

 |  
 
![](https://img.atcoder.jp/ahc009/f29506fb2_1.svg) 
下右下右

 |  
 
![](https://img.atcoder.jp/ahc009/f29506fb2_2.svg) 
下右下右

 |  
 
![](https://img.atcoder.jp/ahc009/f29506fb2_3.svg) 
下右下右下右

### 問題文

20\times 20 マスの地図が与えられる。 
外周は壁に囲まれており、隣接するマスとマスの間にも壁がある場合がある。 
一番左上のマスの座標を (0, 0) とし、そこから下方向に i マス、右方向に j マス進んだ先のマスの座標を (i,j) とする。 
マス (s_i, s_j) に高橋くんの自宅があり、マス (t_i, t_j) に AtCoder社のオフィスがある。 
自宅からオフィスへの通勤経路を、上下左右の移動をそれぞれ `U`, `D`, `L`, `R` で表した長さ 200 以下の文字列として出力せよ。

出力された文字列の長さを L とする。 
高橋くんは自宅から出発し、t=1,\cdots,L ターン目には以下のように行動する。

- 一定の確率 p で、t 文字目を思い出すことが出来ずにその場にとどまる。 

- 残りの確率 1-p で、t 文字目の示す方向へ1マス移動する。その方向が壁の場合はその場にとどまる。

途中でオフィスに到着した場合はその時点で移動を終了する。

### 得点

確率変数 S を、t ターン目の行動後にオフィスに到着した場合は S=401-t、最後までオフィスに到着出来なかった場合は S=0 として定義し、期待値 E[S] を計算する。 
このとき、\mathrm{round}(250000\times E[S]) の得点が得られる。 
不正な出力(長さが200を超えるか、`U`, `D`, `L`, `R` 以外の文字を含む)がされた場合は WA と判定される。

テストケースは全部で 100 個あり、各テストケースの得点の合計が提出の得点となる。 
1つ以上のテストケースで AC 以外の判定がされた場合、提出の得点は0点となる。 
コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 
同じ得点を複数の参加者が得た場合、その得点を獲得した提出の早い方が上位となる。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
s_i s_j t_i t_j p
h_{0,0} \cdots h_{0,18}
\vdots
h_{19,0} \cdots h_{19,18}
v_{0,0} \cdots v_{0,19}
\vdots
v_{18,0} \cdots v_{18,19}

```

自宅とオフィスの座標は 0\leq s_i\leq 4, 0\leq s_j\leq 4, 15\leq t_i\leq 19, 15\leq t_j\leq 19 を満たす。 
p は各文字を忘れてしまう確率で 0.1 以上 0.5 以下の実数である。 
h_{i,0} \cdots h_{i,18} は 0 もしくは 1 のみからなる 19 文字の文字列であり、(i,j) と (i,j+1) のマスの間に壁がある場合は h_{i,j}=1、無い場合は h_{i,j}=0 である。 
v_{i,0} \cdots v_{i,19} は 0 もしくは 1 のみからなる 20 文字の文字列であり、(i,j) と (i+1,j) のマスの間に壁がある場合は v_{i,j}=1、無い場合は v_{i,j}=0 である。 
全てのマスは自宅から到達可能であることが保証されている。

### 出力

高橋くんが記憶する文字列を1行に出力せよ。

[例を見る](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60.html?lang=ja&show=example)

### 入力生成方法

L 以上 U 以下の整数値を一様ランダムに生成する関数を \mathrm{rand}(L,U) で表す。

#### (s_i, s_j), (t_i, t_j), p の生成

s_i=\mathrm{rand}(0, 4), s_j=\mathrm{rand}(0, 4), t_i=\mathrm{rand}(15, 19), t_j=\mathrm{rand}(15, 19), p=\mathrm{rand}(10, 50) / 100 により生成される。

#### h_{i,j}, v_{i,j} の生成

[k]=\{0,1,\cdots,k-1\} と定義する。 
頂点集合 V=[20]\times[20]、辺集合 E=\{\{(i,j),(i,j+1)\}\mid i\in[20],j\in[19]\}\cup\{\{(i,j),(i+1,j)\}\mid i\in[19],j\in[20]\} のグリッドグラフ上の2つの全域木 G_r=(V,E_r) (r=1,2) を以下の処理を独立に二回行うことで生成する。

- まず、辺集合Eをランダムな順番にシャッフルして e_0,\cdots,e_{759} とする。 

- E_r=\emptyset から開始し、k=0 から k=759 まで順番に、各 e_k=\{(i,j),(i',j')\} に対して、(i,j) と (i',j') が G_r 上で連結でないならば e_k を E_r に加える。

得られた2つの全域木を用いて、h と v を以下のように設定する。

- h_{i,j}=0 \iff \{(i,j),(i,j+1)\}\in E_1\cup E_2 

- v_{i,j}=0 \iff \{(i,j),(i+1,j)\}\in E_1\cup E_2

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60.html): ローカル版より高性能でアニメーション表示が可能です。 

- [ローカル版](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

**コンテスト終了までビジュアライズ結果の共有は禁止されています。ご注意下さい。**

### 入力例 1

```text
1 2 19 16 0.10
0010000100110011100
1000001000001000001
0000000000000000010
0100000000000110100
0000000000000000100
1000101100000101010
0010001011000110000
0000001001000000000
0000000100010001001
0010010000100000001
0001000010000100000
0011010000000001000
0000000101010100000
0000001000000100010
0110100010000000000
0010011101000101000
0000100110010000000
0010000101101000010
1001000000000000000
1000110000000000000
00000001000000000100
00001000010001000000
00010001010000010000
01110010101000010100
00000000000001100000
00001000010000000100
00101000000010110011
01010100000000000000
00001101010010010010
10000000000000010100
01011010000001100100
00000000000000010011
00001100111000110100
00000010000000000000
00010000100111000000
11010000001001010100
01100010011001011001
00000101000010101010
00100000000000000001

```

### 出力例 1

```text
RDLRLRRDUUDRLURDRRDDLRLRUURUUUUDRDRUUDLRUDLLDDDDURLRUDDLDRDLRLLLLUDRUDRRULRULRDLRLDLLUUULDLUURRLRDRRRRDULDRLRRRRDDDDDRULDDURDDDRDLRRLDLDLDLRLLLUDDRDDDUURUDDLLLUULLLLDLUDRLLDLLULRLDLDDRLDRDLURURULRDDDR

```

### Story

A map like in the figure below is given. 
Takahashi's home is located in the square with the red circle, and AtCoder's office is located in the square with the blue circle. 
He memorizes his commuting route to the office as a string of characters such as DRDR, which means Down, Right, Down, and Right. 
Because he is quite forgetful, he sometimes forgets some parts of the string he has memorized. 
For example, if he forgets the third character, he will move down, right, and right, and will be lost without reaching the office. 
Therefore, he decided to memorize a robust string that would allow him to reach the office with a high probability even if he forgets some parts of the string. 
For example, if he memorizes a string DRDRDR, he can reach the office even if he forgets any one of the characters. 
Your task is to find a string that will allow Takahashi to reach the office quickly and with a high probability.

 |  
 
![](https://img.atcoder.jp/ahc009/f29506fb2_1.svg) 
DRDR

 |  
 
![](https://img.atcoder.jp/ahc009/f29506fb2_2.svg) 
DRDR

 |  
 
![](https://img.atcoder.jp/ahc009/f29506fb2_3.svg) 
DRDRDR

### Problem Statement

You are given a map consisting of 20\times 20 squares. 
The outside of the map is surrounded by walls. 
There may also be walls between adjacent squares. 
Let (0,0) denote the top-left square, and (i,j) denote the square at the i-th row from the top and j-th column from the left. 
Takahashi's home is located at (s_i, s_j) and AtCoder's office is located at (t_i, t_j). 
By representing up, down, left, and right movements as `U`, `D`, `L`, and `R`, respectively, output a commuting route from the home to the office as a string of length less than or equal to 200.

Let L be the length of the output string. 
Starting from the home, Takahashi will do the following action in each t=1,\cdots,L turn.

- With constant probability p, he cannot recall the t-th character and stays in the current square. 

- With the remaining probability 1-p, he moves one square in the direction represented by the t-th character. If there is a wall in that direction, he stays in the current square.

When he gets to the office, he immediately terminate the move.

### Scoring

Let S be a random variable defined as S=401-t if he gets to the office after t turns of actions and S=0 if he fails to get to the office, and compute its expected value, E[S]. 
Then, you will get a score of \mathrm{round}(250000\times E[S]). 
If your output is invalid (the length exceeds 200 or contains characters other than `U`, `D`, `L`, and `R`), it will be judged as WA.

There are 100 test cases, and the score of a submission is the total score for each test case. If you get a result other than AC for one or more test cases, the score of the submission will be zero. The highest score obtained during the contest time will determine the final ranking, and there will be no system test after the contest. If more than one participant gets the same score, the ranking will be determined by the submission time of the submission that received that score.

### Input

Input is given from Standard Input in the following format:

```text
s_i s_j t_i t_j p
h_{0,0} \cdots h_{0,18}
\vdots
h_{19,0} \cdots h_{19,18}
v_{0,0} \cdots v_{0,19}
\vdots
v_{18,0} \cdots v_{18,19}

```

The coordinates of the home and the office satisfy 0\leq s_i\leq 4, 0\leq s_j\leq 4, 15\leq t_i\leq 19, and 15\leq t_j\leq 19. 
p is a real number representing the probability of forgetting each character and satisfies 0.1\leq p\leq 0.5. 
h_{i,0} \cdots h_{i,18} is a string of 19 characters consisting of only 0 or 1. 
If there is a wall between the squares (i,j) and (i,j+1), then h_{i,j}=1, otherwise h_{i,j}=0. 
v_{i,0} \cdots v_{i,19} is a string of 20 characters consisting of only 0 or 1. 
If there is a wall between the squares (i,j) and (i+1,j), then v_{i,j}=1, otherwise v_{i,j}=0. 
It is guaranteed that all squares are reachable from the home.

### Output

Output a string that Takahashi memorizes in one line to Standard Output.

[Show example](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60.html?lang=en&show=example)

### Input Generation

Let \mathrm{rand}(L,U) be a function that generates a uniform random integer between L and U, inclusive.

#### Generation of (s_i, s_j), (t_i, t_j), and p

We generate s_i=\mathrm{rand}(0, 4), s_j=\mathrm{rand}(0, 4), t_i=\mathrm{rand}(15, 19), t_j=\mathrm{rand}(15, 19), and p=\mathrm{rand}(10, 50) / 100.

#### Generation of h_{i,j} and v_{i,j}

Let [k]=\{0,1,\cdots,k-1\}. 
Let G=(V,E) be a grid graph such that V=[20]\times[20] and E=\{\{(i,j),(i,j+1)\}\mid i\in[20],j\in[19]\}\cup\{\{(i,j),(i+1,j)\}\mid i\in[19],j\in[20]\}. 
We generate two spanning trees G_r=(V,E_r) (r=1,2) of G by performing the following process twice independently.

- First, we randomly shuffle the edges E and obtain an ordered edge list e_0,\cdots,e_{759}. 

- Starting from E_r=\emptyset, for each e_k=\{(i,j),(i',j')\} in order from k=0 to k=759, we insert e_k into E_r if (i,j) and (i',j') are not connected in G_r.

Using the obtained two spanning trees, we generate h and v as follows.

- h_{i,j}=0 \iff \{(i,j),(i,j+1)\}\in E_1\cup E_2 

- v_{i,j}=0 \iff \{(i,j),(i+1,j)\}\in E_1\cup E_2

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60.html?lang=en): This is more powerful than the local version and can display animations. 

- [Local version](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc009/cf3f791aac0f80374c60_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

**Sharing visualization results is not allowed until the end of the contest. **

### Sample Input 1

```text
1 2 19 16 0.10
0010000100110011100
1000001000001000001
0000000000000000010
0100000000000110100
0000000000000000100
1000101100000101010
0010001011000110000
0000001001000000000
0000000100010001001
0010010000100000001
0001000010000100000
0011010000000001000
0000000101010100000
0000001000000100010
0110100010000000000
0010011101000101000
0000100110010000000
0010000101101000010
1001000000000000000
1000110000000000000
00000001000000000100
00001000010001000000
00010001010000010000
01110010101000010100
00000000000001100000
00001000010000000100
00101000000010110011
01010100000000000000
00001101010010010010
10000000000000010100
01011010000001100100
00000000000000010011
00001100111000110100
00000010000000000000
00010000100111000000
11010000001001010100
01100010011001011001
00000101000010101010
00100000000000000001

```

### Sample Output 1

```text
RDLRLRRDUUDRLURDRRDDLRLRUURUUUUDRDRUUDLRUDLLDDDDURLRUDDLDRDLRLLLLUDRUDRRULRULRDLRLDLLUUULDLUURRLRDRRRRDULDRLRRRRDDDDDRULDDURDDDRDLRRLDLDLDLRLLLUDDRDDDUURUDDLLLUULLLLDLUDRLLDLLULRLDLDDRLDRDLURURULRDDDR

```

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

- [ルール](https://atcoder.jp/contests/ahc009/rules)
 
- [用語集](https://atcoder.jp/contests/ahc009/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
