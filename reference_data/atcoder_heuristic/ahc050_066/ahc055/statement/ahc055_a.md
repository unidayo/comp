### ストーリー

Nihonbashi Treasure Island は架空の国Rで大人気の小説であり、あなたはこの小説をベースにしたゲームの大ファンである。 このゲームでは、たくさんの宝箱が出現する。宝箱を開くと武器が入手でき、武器を用いてゲームを有利に進めることができる。

宝箱を開くには、素手または武器を用いて宝箱を攻撃する必要がある。ゲーム内のすべての宝箱を開けると隠しエンディングが開放されるという話を聞いたあなたは、いち早くエンディングを見るべく、少ない攻撃回数ですべての宝箱を開く方法を探すことにした。

### 問題文

N 個の宝箱があり、 i 番目の宝箱を宝箱 i と呼ぶ (0 \le i \lt N) 。宝箱 i の初期の硬さは H_i である。また、宝箱 i の中には武器 i が入っている。

宝箱 i の硬さを 0 以下にすると宝箱 i が開き、武器 i が利用可能になる。武器には耐久値が存在し、武器 i の耐久値は C_i である。

素手または利用可能な武器を用いて、宝箱を攻撃することができる。

- 素手で攻撃: 素手で宝箱 b に攻撃したとき、宝箱 b の硬さを 1 減らす。素手での攻撃回数に制限はない。 
 
- 武器で攻撃: 武器 w で宝箱 b に攻撃したとき、宝箱 b の硬さを A_{w,b} 減らし、武器 w の耐久値が 1 減る。武器 w の耐久値が 0 になったとき、武器 w は壊れて攻撃に用いることができなくなる。

できるだけ少ない回数ですべての宝箱を開ける攻撃手順を求めよ。

### 得点

すべての宝箱を開けるために攻撃した回数が T のとき、\( \sum_{i=0}^{N-1} H_i - T + 1 \) の得点が得られる。

以下に挙げる不正な出力をした場合、WAと判定される。

- すでに開いた宝箱を攻撃する 
 
- 利用可能になっていない武器を使用して攻撃する 
 
- 耐久値が 0 になった武器を使用して攻撃する 
 
- 存在しない武器の番号や宝箱の番号を出力する 
 
- すべての攻撃を終えた状態で開いていない宝箱がある

合計で 150 個のテストケースがあり、各テストケースの得点の合計が提出の得点となる。 
 一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなる。 
 コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 同じ得点を複数の参加者が得た場合、提出時刻に関わらず同じ順位となる。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
\(
N \\
H_0 ~ H_1 ~ \ldots ~ H_{N-1} \\
C_0 ~ C_1 ~ \ldots ~ C_{N-1} \\
A_{0,0} ~ A_{0,1} ~ \ldots ~ A_{0,N-1} \\
A_{1,0} ~ A_{1,1} ~ \ldots ~ A_{1,N-1} \\
\vdots \\
A_{N-1,0} ~ A_{N-1,1} ~ \ldots ~ A_{N-1,N-1}
\)

```

- 1行目には1つの整数 N が与えられる。 N は宝箱の数で、 N = 200 を満たす。 
 
- 2行目には、宝箱の初期の硬さを表す N 個の整数 H_0, H_1, \ldots, H_{N-1} が空白区切りで与えられる。宝箱 i の硬さは H_i であり、100 \leq H_i \leq 500 を満たす。 
 
- 3行目には、武器の耐久値を表す N 個の整数 C_0, C_1, \ldots, C_{N-1} が空白区切りで与えられる。武器 i の耐久値は C_i であり、1 \leq C_i \leq 6 を満たす。 
 
- 続く N 行には、各行について N 個の整数 A_{i, j} が空白区切りで与えられる。

- A_{i,j} は、武器 i で宝箱 j を攻撃したときに宝箱 j の硬さを減らす値であり、1 \leq A_{i,j} \leq 500 を満たす。

### 出力

宝箱を攻撃した回数を T とする。 T 行出力せよ。

```text
\(
W_{0}~B_{0}\\
\vdots \\
W_{T-1} ~ B_{T-1}
\)

```

各行には、 i 回目の攻撃で用いた武器の番号 W_i と攻撃対象の宝箱の番号 B_i をこの順に空白区切りで出力せよ。なお、 i 回目の攻撃が素手で行われた場合は、 W_i は `-1` として出力せよ。

[例を見る](https://img.atcoder.jp/ahc055/ys4u9l6aru.html?lang=ja&output=sample)

### 入力生成方法

 L 以上 U 以下の整数を一様ランダムに生成する関数を \mathrm{rand}(L, U) 、L 以上 U 以下の実数を一様ランダムに生成する関数を \mathrm{rand\_double}(L, U) とする。

H_i, C_i, A_{i,j} (0 \le i \lt N, 0 \le j \lt N) をそれぞれ独立に、以下のようにして生成する。

- H_i = \mathrm{rand}(100, 500) 
 
- C_i = \mathrm{rand}(1, 6) 
 
- A_{i,j} = \mathrm{round}(500.0 / \mathrm{rand\_double}(1.0, 500.0))

### ツール(入力ジェネレータ・ビジュアライザ)

- [Web版](https://img.atcoder.jp/ahc055/ys4u9l6aru.html): ローカル版より高機能でアニメーション表示が可能です。 
 
- [ローカル版](https://img.atcoder.jp/ahc055/ys4u9l6aru.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 
 
- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc055/ys4u9l6aru_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

 コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### Story

"Nihonbashi Treasure Island" is a hugely popular novel in the fictional country R, and you are a big fan of the game based on this novel. In this game, numerous treasure boxes appear. Opening a treasure box allows you to acquire a weapon, which you can use to gain an advantage in the game.

To open a treasure box, you must attack it using either your bare hands or a weapon. Having heard that a secret ending is unlocked by opening all the treasure boxes in the game, you decide to find a way to open all of them with the minimum number of attacks, hoping to see the ending as soon as possible.

### Problem Statement

There are N treasure boxes, and the i-th treasure box is called treasure box i (0 \le i \lt N). The initial hardness of treasure box i is H_i. Treasure box i contains weapon i.

When the hardness of treasure box i is reduced to 0 or less, treasure box i opens, and weapon i becomes available. Weapons have durability, and the durability of weapon i is C_i.

You can attack a treasure box using either your bare hands or an available weapon.

- Attack with bare hands: When you attack treasure box b with your bare hands, the hardness of treasure box b is reduced by 1. There is no limit to the number of bare-handed attacks. 
 
- Attack with a weapon: When you attack treasure box b with weapon w, the hardness of treasure box b is reduced by A_{w,b}, and the durability of weapon w decreases by 1. When the durability of weapon w reaches 0, it breaks and can no longer be used for attacks.

Find a sequence of attacks to open all the treasure boxes with as few attacks as possible.

### Scoring

Your score is \( \sum_{i=0}^{N-1} H_i - T + 1 \), where T is the number of attacks performed to open all treasure boxes.

 If the following invalid outputs are produced, it will be judged as WA.

- Attacking an already opened treasure box 
 
- Attacking with a weapon that is not yet available 
 
- Attacking with a weapon whose durability has reached 0 
 
- Outputting an invalid weapon number or treasure box number 
 
- Leaving any unopened treasure boxes after all attacks are finished

There are 150 test cases, and the score of a submission is the total score for each test case. 
 If your submission produces an illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
 The highest score obtained during the contest will determine the final ranking, and there will be no system test after the contest. 
 If more than one participant gets the same score, they will be ranked in the same place regardless of the submission time.

### Input

Input will be provided from Standard Input in the following format.

```text
\(
N \\
H_0 ~ H_1 ~ \ldots ~ H_{N-1} \\
C_0 ~ C_1 ~ \ldots ~ C_{N-1} \\
A_{0,0} ~ A_{0,1} ~ \ldots ~ A_{0,N-1} \\
A_{1,0} ~ A_{1,1} ~ \ldots ~ A_{1,N-1} \\
\vdots \\
A_{N-1,0} ~ A_{N-1,1} ~ \ldots ~ A_{N-1,N-1}
\)

```

- N is the number of treasure boxes. It is fixed at N = 200 in all test cases. 
 
- The second line contains N space-separated integers H_0, H_1, \ldots, H_{N-1} representing the initial hardness of the treasure boxes. The hardness of treasure box i is H_i, satisfying 100 \leq H_i \leq 500. 
 
- The third line contains N space-separated integers C_0, C_1, \ldots, C_{N-1} representing the durability of the weapons. The durability of weapon i is C_i, satisfying 1 \leq C_i \leq 6. 
 
- The following N lines each contains N space-separated integers, A_{i, j}.

- A_{i,j} represents the value by which the hardness of treasure box j is reduced when attacked with weapon i, satisfying 1 \leq A_{i,j} \leq 500.

### Output

Let T be the total number of attacks on the treasure boxes. Output T lines.

```text
\(
W_{0}~B_{0}\\
\vdots \\
W_{T-1} ~ B_{T-1}
\)

```

For each line, output the weapon index W_i used for the i-th attack and the treasure box index B_i being attacked, in that order, separated by a space. If the i-th attack was performed with bare hands, W_i should be output as `−1`.

[show example](https://img.atcoder.jp/ahc055/ys4u9l6aru.html?lang=en&output=sample)

### Input Generation

- \mathrm{rand}(L, U): Generates an integer uniformly at random between L and U, inclusive. 
 
- \mathrm{rand\_double}(L, U): Generates a real number uniformly at random between L and U, inclusive.

Generate H_i, C_i, and A_{i,j} (0 \le i \lt N, 0 \le j \lt N) independently, as follows:

- H_i = \mathrm{rand}(100, 500) 
 
- C_i = \mathrm{rand}(1, 6) 
 
- A_{i,j} = \mathrm{round}(500.0 / \mathrm{rand\_double}(1.0, 500.0))

### Tools (Input generator and visualizer)

- [Web version](https://img.atcoder.jp/ahc055/ys4u9l6aru.html?lang=en): This is more powerful than the local version providing animations. 
 
- [Local version](https://img.atcoder.jp/ahc055/ys4u9l6aru.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 
 
- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc055/ys4u9l6aru_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

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

- [ルール](https://atcoder.jp/contests/ahc055/rules)
 
- [用語集](https://atcoder.jp/contests/ahc055/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
