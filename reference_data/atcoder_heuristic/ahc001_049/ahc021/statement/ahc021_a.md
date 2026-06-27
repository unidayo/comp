### 問題文

N(N+1)/2 個のボールが下図のように N 段のピラミッド型に並んでいる。 
ピラミッドの頂上のボールの座標を (0, 0) とし、x (0\leq x\leq N-1) 段目の左から y (0\leq y\leq x) 個目のボールの座標を (x,y) とする。 
 
 ![](https://img.atcoder.jp/ahc021/d17182d7.png)

各ボールには 0 〜 N(N+1)/2-1 の数字が書かれており、各ボールの数字は全て異なる。 
一回の操作で6方向に隣接する2つのボールを入れ替えることが出来る。 
ここで、座標 (x_1,y_1) と (x_2,y_2) にあるボールは以下のいずれかの条件を満たす場合に6方向に隣接している。

- x_1=x_2-1 かつ y_1=y_2-1 

- x_1=x_2-1 かつ y_1=y_2 

- x_1=x_2 かつ y_1=y_2-1 

- x_1=x_2 かつ y_1=y_2+1 

- x_1=x_2+1 かつ y_1=y_2 

- x_1=x_2+1 かつ y_1=y_2+1

この操作を最大 10000 回繰り返すことで、最下層を除くどのボール (x,y) (0\leq x\leq N-2, 0\leq y\leq x) も自身の直下にある2つのボール (x+1,y), (x+1,y+1) よりも小さい数字となるようにしたい。 
出来るだけ操作回数が少なくなるような操作列を求めて欲しい。 
 
 ![](https://img.atcoder.jp/ahc021/d17182d7.gif)

### 得点

操作回数を K、操作終了後に条件に違反しているボールのペアの数を E とする。 
E は (x,y) と (x+1,y') (y'\in\{y,y+1\}) の組であって、(x,y) のボールの数字の方が (x+1,y') のボールの数字より大きいようなものの総数である。

このとき、以下のスコアが得られる。

- E=0 の場合、100000-5K 

- E>0 の場合、50000-50E

テストケースは全部で 150 個あり、各テストケースの得点の合計が提出の得点となる。 
一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなる。 
コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 同じ得点を複数の参加者が得た場合、提出時刻に関わらず同じ順位となる。

### 入力

全ての入力でピラミッドの段数は N=30 で固定であり、ボールの総数は N(N+1)/2=465 となる。 
入力は以下の形式で標準入力から与えられる。

```text
b_{0,0}
b_{1,0} b_{1,1}
b_{2,0} b_{2,1} b_{2,2}
\vdots
b_{29,0} \cdots b_{29,29}

```

b_{x,y} は初期状態で座標 (x,y) にあるボールに書かれた数字を表す整数であり、0\leq b_{x,y}\leq 464 を満たす。 
同じ数字のボールは複数存在しない。

### 出力

操作回数を K、i 回目の操作で入れ替える2つのボールの座標を (x_i,y_i), (x'_i,y'_i) としたとき、以下の形式で標準出力に出力せよ。

```text
K
x_0 y_0 x'_0 y'_0
\vdots
x_{K-1} y_{K-1} x'_{K-1} y'_{K-1}

```

操作回数 K は 10000 を超えてはならず、入れ替える2つのボールは6方向に隣接していなければならない。 
これらの条件を満たさない場合、WA と判定される。

[例を見る](https://img.atcoder.jp/ahc021/d17182d7.html?lang=ja&seed=0&output=15%0D%0A2+0+3+1%0D%0A8+5+9+5%0D%0A24+5+25+6%0D%0A9+5+10+6%0D%0A10+10+11+10%0D%0A1+0+2+1%0D%0A28+9+29+10%0D%0A0+0+1+0%0D%0A18+7+19+8%0D%0A18+18+19+19%0D%0A20+5+21+5%0D%0A11+0+12+1%0D%0A27+12+28+12%0D%0A19+1+20+2%0D%0A7+3+8+3%0D%0A)

### 入力生成方法

0 〜 464 の数字の書かれた 465 個のボールをランダムな順番に並び替えることで入力は生成される。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc021/d17182d7.html?lang=ja): ローカル版より高性能でアニメーション表示が可能です。 

- [ローカル版](https://img.atcoder.jp/ahc021/d17182d7.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc021/d17182d7_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### 入力例 1

```text
236
11 200
453 2 378
85 410 239 54
50 240 113 25 294
303 231 146 65 155 252
368 327 321 251 451 182 142
101 17 43 403 217 161 347 398
350 287 363 48 80 447 385 233 197
438 424 439 121 357 380 51 245 57 304
141 91 100 344 194 250 432 322 58 281 219
412 266 26 318 269 111 59 450 99 301 36 320
218 135 278 225 227 268 313 162 420 214 42 166 55
181 191 75 335 332 372 144 342 29 94 427 62 334 409
377 213 369 117 307 428 280 152 242 88 460 175 351 340 230
209 139 288 132 47 456 167 205 455 38 27 326 306 134 108 34
389 441 393 361 120 296 331 316 458 183 170 40 397 274 402 103 408
136 463 82 0 364 150 462 157 67 92 419 371 156 228 1 138 53 349
174 129 71 169 199 367 87 443 359 172 298 22 244 415 401 373 417 160 305
254 158 84 9 435 130 118 430 203 345 185 388 379 207 220 238 196 208 289 153
425 356 109 237 116 212 358 396 270 179 262 76 370 444 308 229 105 148 365 429 248
386 93 376 86 442 184 273 243 81 69 189 204 355 106 154 257 464 107 70 215 77 241
127 404 180 123 362 124 234 300 173 222 72 329 140 147 232 3 187 114 381 133 44 387 60
145 4 210 49 382 433 457 343 315 354 149 255 299 12 28 421 394 211 102 297 360 8 314 328
46 256 416 336 423 295 90 112 168 309 64 96 260 437 440 366 323 35 383 346 79 293 61 312 202
18 23 20 221 31 452 178 461 6 21 201 143 324 277 341 283 291 5 41 66 411 317 258 406 19 188
164 261 16 137 426 264 98 78 45 310 246 176 319 95 384 68 392 37 265 190 249 73 198 15 32 330 311
131 282 434 271 400 445 104 56 119 263 374 337 216 177 449 353 24 192 390 39 33 223 407 348 195 279 290 110
267 89 375 284 13 63 391 422 272 259 30 97 125 459 413 333 52 448 399 122 83 126 275 302 115 14 338 446 339
171 206 74 163 292 454 405 247 395 285 193 325 418 352 286 186 128 7 159 431 226 165 224 436 10 276 414 151 235 253

```

### 出力例 1

```text
15
2 0 3 1
8 5 9 5
24 5 25 6
9 5 10 6
10 10 11 10
1 0 2 1
28 9 29 10
0 0 1 0
18 7 19 8
18 18 19 19
20 5 21 5
11 0 12 1
27 12 28 12
19 1 20 2
7 3 8 3

```

### Problem Statement

There are N(N+1)/2 balls arranged in an N-tiered pyramid as shown in the figure below. 
Let (0, 0) be the coordinates of the ball at the top of the pyramid, and let (x,y) be the coordinates of the y (0\leq y\leq x)-th ball from the left in the x (0\leq x-1)-th tier from the top. 
 
 ![](https://img.atcoder.jp/ahc021/d17182d7.png)

Each ball is labeled with a number from 0 to N(N+1)/2-1, and the numbers on each ball are all different. 
You can swap two adjacent balls in six directions in a single operation. 
Here, the balls at coordinates (x_1,y_1) and (x_2,y_2) are adjacent in six directions if one of the following conditions is satisfied.

- x_1=x_2-1 and y_1=y_2-1 

- x_1=x_2-1 and y_1=y_2 

- x_1=x_2 and y_1=y_2-1 

- x_1=x_2 and y_1=y_2+1 

- x_1=x_2+1 and y_1=y_2 

- x_1=x_2+1 and y_1=y_2+1

By performing this operation at most 10000 times, please arrange the balls so that every ball (x,y) (0\leq x\leq N-2, 0\leq y\leq x) except those in the lowest tier has a smaller number than the two balls (x+1,y), (x+1,y+1) directly below it. 
Please achieve this with as few operations as possible. 
 
 ![](https://img.atcoder.jp/ahc021/d17182d7.gif)

### Scoring

Let K be the number of operations and E be the number of pairs of balls violating the condition after finishing the operations. 
Here E is the total number of pairs of (x,y) and (x+1,y') (y'\in\{y,y+1\}) such that the ball (x,y) has a larger number than the ball (x+1,y'). 
Then you will obtain the following score.

- If E=0, 100000-5K. 

- If E>0, 50000-50E.

There are 150 test cases, and the score of a submission is the total score for each test case. 
If your submission produces an illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
The highest score obtained during the contest will determine the final ranking, and there will be no system test after the contest. 
If more than one participant gets the same score, they will be ranked in the same place regardless of the submission time.

### Input

The number of pyramid tiers is fixed to N=30 for all inputs, and the total number of balls is N(N+1)/2=465. 
Input is given from Standard Input in the following format:

```text
b_{0,0}
b_{1,0} b_{1,1}
b_{2,0} b_{2,1} b_{2,2}
\vdots
b_{29,0} \cdots b_{29,29}

```

b_{x,y} is an integer satisfying 0\leq b_{x,y}\leq 464 and represents the number written on the ball initially at the coordinates (x,y). 
All the numbers are distinct.

### Output

Let K be the number of operations and (x_i,y_i), (x'_i,y'_i) be the coordinates of the two balls to be swapped in the i-th operation. 
Then, output to Standard Output in the following format.

```text
K
x_0 y_0 x'_0 y'_0
\vdots
x_{K-1} y_{K-1} x'_{K-1} y'_{K-1}

```

The number of operations K must not exceed 10000 and any two balls to be swapped must be adjacent in 6 directions. 
If not satisfied, the submission will be judged as WA .

[Show example](https://img.atcoder.jp/ahc021/d17182d7.html?lang=ja&seed=0&output=15%0D%0A2+0+3+1%0D%0A8+5+9+5%0D%0A24+5+25+6%0D%0A9+5+10+6%0D%0A10+10+11+10%0D%0A1+0+2+1%0D%0A28+9+29+10%0D%0A0+0+1+0%0D%0A18+7+19+8%0D%0A18+18+19+19%0D%0A20+5+21+5%0D%0A11+0+12+1%0D%0A27+12+28+12%0D%0A19+1+20+2%0D%0A7+3+8+3%0D%0A)

### Input Generation

The input is generated by randomly shuffling 465 balls with numbers 0 to 464.

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc021/d17182d7.html?lang=en): This is more powerful than the local version providing animations. 

- [Local version](https://img.atcoder.jp/ahc021/d17182d7.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc021/d17182d7_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

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

- [ルール](https://atcoder.jp/contests/ahc021/rules)
 
- [用語集](https://atcoder.jp/contests/ahc021/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
