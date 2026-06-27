![](https://img.atcoder.jp/ahc018/a8a5bc5edd5e6d6301e8f4bd11f3c531.gif)

### 問題文

 N 行 N 列のグリッドで表される土地があります。
 
 グリッドのうち W 箇所に**水源**があり、 K 箇所に**家**があります。
 
 土地の上から i 行目（  0 \leq i \lt N  ）、左から j 列目（  0 \leq j \lt N  ）のセルを (i, j) と表すことにします。
 
 各セル (i, j) の**岩盤**には、10 以上 5000 以下の整数で表される **頑丈さ** S_{i,j} が予め設定されていますが、これらの値は入力では与えられません。

 **水源**が存在するセルの**岩盤**を破壊すると、そのセルに水が流れます。
 
 また、水が流れているセルの上下左右に隣接する**岩盤**が破壊されたセルにも水が流れます。
 
 土地の管理者であるXさんは、いくつかの**岩盤**を破壊して、**家**が存在する全てのセルに水が流れるようにしたいです。
 
 **家**が存在するセルに水を流すには、そのセルの**岩盤**も破壊する必要がある点に注意してください。

 **岩盤**を破壊するためには、Xさんは以下の**掘削**を繰り返し行います。

-  
 まだ**岩盤**が破壊されていないセル (i, j) を一つと、**パワー**を表す 5000 以下の正の整数 P を一つ選びます。

-  
 Xさんの**体力**を C+P （C は入力で与えられる正の整数）消費して、セルの**岩盤**の**頑丈さ** S_{i,j} を P 減らします。

-  
 この操作で S_{i,j} \leq 0 となった場合、セル (i, j) の**岩盤**が破壊されます。

 なるべくXさんの**体力**を消費しないように工夫しながら、**家**が存在する全てのセルに水が流れるようにしてください。

### 得点・順位付け

 相対評価システムを採用します。
 
 各テストケース毎に、\mathrm{round} \lparen 10^9 \times \frac{全参加者中の最小消費体力}{自身の消費体力} \rparen の相対評価スコアが得られ、その和が提出の得点となります。

 最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定されます。暫定テスト・システムテストのどちらにおいても、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースに関する相対評価スコアは0点となり、そのテストケースにおいては「全参加者中の最小消費体力」の計算対象から除外されます。

 また、システムテストは **CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意してください。

#### テストケース数

-  
 暫定テスト: 50個

-  
 システムテスト: 3000個

- システムテストでは、C は \{ 1,2,4,8,16,32,64,128 \} のそれぞれの値について、375個ずつのテストケースが存在します。 
 
- コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc018/seeds.txt) (sha256=`3bbc58026730d18fcb0e03ead929ab2afb6f9f0c5eb7f0c10ac44df4e6b84a3f`) を公開します。

#### 相対評価システムについて

 暫定テスト・システムテストともに、CE以外の結果を得た一番最後の提出のみが順位表に反映されます。
 
 相対評価スコアの計算に用いられる各テストケース毎の「全参加者中の最小消費体力」の算出には、各参加者の順位表に反映されている提出のみが用いられます。
 
 順位表に表示されるスコアは相対評価スコアであり、新規提出があるたびに相対評価スコアが再計算されます。 一方、提出一覧から確認できる各提出のスコアは、各テストケース毎の消費体力をそのまま足し合わせたものであり、相対評価スコアは表示されません。 最新以外の提出の、現在の順位表における相対評価スコアを知るためには、再提出が必要となります。
 
 不正な出力や制限時間超過をした場合、提出一覧から確認できるスコアは0となりますが、順位表には正解したテストケースに対する相対評価スコアの和が表示されます。

#### 実行時間について

 実行時間には多少のブレが生じます。 
 また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
 そのため、実行時間制限ギリギリの提出がシステムテストでTLEとなる可能性があります。 
 プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入出力

 **※この問題はインタラクティブ形式です。以下の内容を読み、ジャッジプログラムとのやり取りを行ってください。**

 最初に、入力が以下の形式で標準入力から与えられます。

```text
\(
N ~ W ~ K ~ C \\
a_0 ~ b_0 \\
\vdots \\
a_{W-1} ~ b_{W-1} \\
c_0 ~ d_0 \\
\vdots \\
c_{K-1} ~ d_{K-1}\)

```

-  
 1行目には4つの整数 N,W,K,C がスペース区切りで与えられます。

- N は土地のサイズを表し、 N = 200 です。 
 
- W は**水源**の数を表し、 1 \leq W \leq 4 を満たします。 
 
- K は**家**の数を表し、 1 \leq K \leq 10 を満たします。 
 
- C は**体力**の消費に関係するパラメータで、 C \in \{ 1,2,4,8,16,32,64,128 \} を満たします。

-  
 続く W+K 行には、**水源**と**家**が存在するセルの位置を表す2つの整数がスペース区切りで与えられます。

- 最初に、 W 行で与えられる2整数 a_i, b_i は、 セル (a_i, b_i) に**水源**が存在することを表します。0 \leq a_i \lt N, 0 \leq b_i \lt N を満たします。 
 
- 続いて、 K 行で与えられる2整数 c_i, d_i は、 セル (c_i, d_i) に**家**が存在することを表します。0 \leq c_i \lt N, 0 \leq d_i \lt N を満たします。 
 
- 全ての**水源**同士のペア・全ての**家**同士のペア・全ての**水源**と**家**のペアは、[マンハッタン距離](https://ja.wikipedia.org/wiki/%E3%83%9E%E3%83%B3%E3%83%8F%E3%83%83%E3%82%BF%E3%83%B3%E8%B7%9D%E9%9B%A2)で  \mathrm{round}(400/(W+K)) 以上離れていることが保証されます。

 ここまでの入力を受け取った後は、**掘削**をどのように実行するかを以下の形式で標準出力へ出力してください。

```text

\(y ~ x ~ P\)

```

-  
 1行目に3つの整数 y,x,P をスペース区切りで出力します。

- セル (y,x) に対して、**パワー** P で**掘削**を行います。 
 
- セル (y,x) の**岩盤**はまだ破壊されていない必要があり、かつ、1 \leq P \leq 5000 を満たす必要があります。そうしなかった場合、WA となります。 
 
- 出力の後には**改行をし、標準出力をflushする必要があります**。そうしなかった場合、TLE となる可能性があります。

 出力後、**掘削**を実行した結果が入力として以下の形式で標準入力から与えられます。

```text

\(r\)

```

-  
 1行に1つの整数 r が与えられます。

- 指定したセルの**岩盤**が破壊できなかった場合は `0` となります。 
 
- 指定したセルの**岩盤**が破壊できた場合、**家**が存在するセルでまだ水が流れていないものがあるならば `1` 、**家**が存在する全てのセルに水が流れているならば `2` となります。 
 
- 不正な出力だった場合は `-1` となります。

- r \in \{-1, 2\} の場合は、直ちにプログラムを終了してください。そうしなければ、TLE となる可能性があります。 
 
- r \in \{0, 1\} の場合は、**家**が存在するセルでまだ水が流れていないものがあるため、引き続き**掘削**を行う必要があります。r \in \{-1, 2\} となるまで、**掘削**をどのように実行するかを繰り返し出力し、繰り返し入力を受け取ってください。

### 入力生成方法

L 以上 U 以下の整数値を一様ランダムに生成する関数を \mathrm{rand}(L,U) で、L 以上 U 以下の実数値を一様ランダムに生成する関数を \mathrm{randf}(L,U) で、b の p 乗を返す関数を \mathrm{pow}(b,p) で表す。

#### S_{i,j} の生成

-  
 乱数シード seed を元に、 -1.0以上1.0以下の範囲にスケーリングされた二次元の [Perlin noise](https://ja.wikipedia.org/wiki/%E3%83%91%E3%83%BC%E3%83%AA%E3%83%B3%E3%83%8E%E3%82%A4%E3%82%BA) を生成する関数を \mathrm{noise}(y,x,seed) で表す。
 
 以下のようにパラメタを選ぶ。

- f0 = \mathrm{randf}(2.0,8.0) 
 
- f1 = \mathrm{randf}(10.0,20.0) 
 
- dy0 = \mathrm{randf}(0.0,1.0) 
 
- dy1 = \mathrm{randf}(0.0,1.0) 
 
- dx0 = \mathrm{randf}(0.0,1.0) 
 
- dx1 = \mathrm{randf}(0.0,1.0) 
 
- seed0 = \mathrm{rand}(0,2^{32} - 1) 
 
- seed1 = \mathrm{rand}(0,2^{32} - 1) 
  
 各 i, j（  0 \leq i, j \lt N  ）に対して、 S_{i,j} = \mathrm{noise}(f0 \times i / N + dy0, f0 \times j / N + dx0, seed0) + 0.2 \times \mathrm{noise}(f1 \times i / N + dy1, f1 \times j / N + dx1, seed1) とする。

-  
 S_{i,j} = 1.0 / (1.0 + \mathrm{exp}(-3.0 \times (S_{i,j} - 0.25))) と更新する。（ロジスティック関数で変換をしている）

-  
 p = \mathrm{randf}(2.0,4.0) を選ぶ。 S_{i,j} = \mathrm{pow}(S_{i,j},p) と更新する。

-  
 S_{i,j} を、 10 以上 5000 以下の範囲に線形スケーリングする。
 
 すなわち、 S_{i,j}（  0 \leq i, j \lt N  ）の中で最小の値を l, 最大の値を u としたとき、 S_{i,j} = \mathrm{round}((S_{i,j} - l) \times (5000 - 10) / (u - l) + 10) と更新する。

#### 水源と家の位置の生成

- W = \mathrm{rand}(1,4), K = \mathrm{rand}(1,10) とする。 
 
- W 箇所の**水源**の位置(a_i, b_i), K 箇所の**家**の位置(c_i, d_i) を、それぞれ独立に、N ^ 2 個のセルの中から S_{i,j}に反比例する確率で選ぶ。 
 
- W+K 箇所の位置の中にマンハッタン距離が \mathrm{round}(400/(W+K)) 未満の組がある場合、2. からやり直す。

#### C の生成

C = \mathrm{pow}(2,\mathrm{rand}(0,7)) と生成する。

必ずしも理解する必要はありません。

厳密な実装は入力ジェネレータのソースコードをご覧ください。

### ツール（入力ジェネレータ・ビジュアライザ・ローカルテスタ・サンプルコード）

- [Web版](https://img.atcoder.jp/ahc018/6bada50282.html?lang=ja): アニメーション表示が可能です。 
 
- [ローカル版](https://img.atcoder.jp/ahc018/6bada50282.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 
 
- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc018/6bada50282_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

- [サンプルコード（C++, Python）](https://img.atcoder.jp/ahc018/6bada50282_sample.zip): サンプルの解答です。次の処理を実装しています。

- それぞれの**家**について、一つ目の**水源**まで最短距離でまっすぐに**掘削**を行う 
 
- **パワー**は常に `100` に設定する 
 
- ジャッジプログラムから `2` が返されたら、消費体力を標準エラー出力に出力して終了する

 **コンテスト終了までビジュアライズ結果の共有は禁止されています。ご注意下さい。**

#### ツールで用いられる入出力ファイルの仕様

ローカルテスタに与える入力ファイルは以下の形式を用いています。

```text
\(
N ~ W ~ K ~ C \\
S_{0,0} ~ \cdots ~ S_{0,N-1} \\
\vdots \\
S_{N-1,0} ~ \cdots ~ S_{N-1 ,N-1} \\
a_0 ~ b_0 \\
\vdots \\
a_{W-1} ~ b_{W-1} \\
c_0 ~ d_0 \\
\vdots \\
c_{K-1} ~ d_{K-1} \)

```

S_{i,j} はセル (i, j) にある**岩盤**の**頑丈さ**の初期値です。

ローカルテスタは解答プログラムの出力をそのまま出力ファイルに書き出します。 
 解答プログラムは、`#` から始まるコメント行を出力に含めてもかまいません。 
 Web版ビジュアライザを使用すると、コメント行を対応する**掘削**と合わせて表示できるため、デバッグや考察等に役立てることができます。 
 ジャッジプログラムはコメント行を全て無視するため、コメント行を出力するプログラムをそのまま提出可能です。

### 入出力例

  | Input 
  | Output 
  | 説明

  |

```text

3 1 1 128
0 0
1 1

```

  |  
  |  
 入力の制約を満たさない、説明用の入力である点に注意してください。
 
 3 \times 3 のグリッドで表される土地の (0,0) に**水源**が存在し、 (1,1) に**家**が存在します。

  |  
  |

```text

0 0 872

```

  |  
 セル (0,0) に対して、**パワー** 872 で**掘削**をします。この際、**体力**を128+872 = 1000 消費します。

  |

```text

0

```

  |  
  |  
 この**掘削**では、セル (0,0) の**岩盤**は壊れませんでした。
 
 ですが、セル (0,0) の**岩盤**の**頑丈さ**は 872 減りました。

  |  
  |

```text

0 0 2

```

  |  
 セル (0,0) に対して、**パワー** 2 で**掘削**をします。この際、**体力**を128+2 = 130 消費します。

  |

```text

1

```

  |  
  |  
 この**掘削**で、セル (0,0) の**岩盤**が壊れました。
 
 セル (0,0) に水が流れるようになります。

  |  
  |

```text

1 1 872

```

  |  
 セル (1,1) に対して、**パワー** 872 で**掘削**をします。この際、**体力**を128+872 = 1000 消費します。

  |

```text

1

```

  |  
  |  
 セル (1,1) の**岩盤**が壊れました。

  |  
  |

```text

1 0 872

```

  |  
 セル (1,0) に対して、**パワー** 872 で**掘削**をします。この際、**体力**を128+872 = 1000 消費します。

  |

```text

2

```

  |  
  |  
 セル (1,0) の**岩盤**が壊れました。
 
 セル (0,0), (1,0), (1,1) に水が流れるようになり、**家**が存在する全てのセルに水が流れるようになりました。
 
 ジャッジプログラムから`2` が返されたら、直ちにプログラムを終了してください。
 
 消費した**体力**の合計である 1000+130+1000+1000 = 3130 が相対評価スコアの計算に用いられます。

![](https://img.atcoder.jp/ahc018/a8a5bc5edd5e6d6301e8f4bd11f3c531.gif)

### Problem Statement

 There is plot of land, represented by a grid of N rows and N columns.
 
 On the grid, there are **water sources** in W spots, and **houses** in K spots.
 
 Let us call the cell on ith ( 0 \leq i \lt N ) row from top and jth column ( 0 \leq j \lt N ) from left, (i, j).
 
 Each cell (i, j) has an associated integer S_{i,j} between 10 and 5000 (inclusive), representing the **sturdiness** of the **bedrock** of the cell. The **sturdiness** is pre-determined, but it's not given as a part of the input.

 When the **bedrock** of a cell containing a **water source** is crushed, water flows into the cell.
 
 Furthermore, with cells with flowing water as starting points, water will flow horizontally and vertically into neighboring cells with crushed **bedrock** and span every cell it can reach this way.
 
 X-san, who is in charge of the land, wants to crush **bedrock** to build watercourses to make the water flow into every cell with **houses**.
 
 Please note that for water to flow into a cell with a **house**, the **bedrock** of that cell must be crushed too.

 To crush **bedrock**, X-san repeatedly performs **excavations** as explained below.

-  
 Choose a cell (i, j) whose **bedrock** isn't crushed yet, and a positive integer P less or equal than 5000, representing **power**.

-  
 As C+P (C is a positive integer received as an input) of X-san's **stamina** gets consumed, the **sturdiness** S_{i,j} of the **bedrock** in the cell diminishes by P.

-  
 If **sturdiness** then reaches zero S_{i,j} \leq 0, the **bedrock** of the cell (i, j) gets crushed.

 Make the water flow into every cell with a **house**, while trying to conserve X-san's **stamina** as much as possible.

### Scoring

For each test case, we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{MIN}}{\mathrm{YOUR}}), where YOUR is your stamina consumption and MIN is the lowest stamina consumption among all competitors obtained on that test case. The score of the submission is the sum of the relative scores.

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
 In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MIN calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
 Be careful not to make a mistake in the final submission.

#### Number of test cases

-  
 Provisional test: 50

-  
 System test: 3000

- For the system test, there will be 375 data inputs for each C (C \in \{ 1,2,4,8,16,32,64,128 \}) 
 
- We will publish [seeds.txt](https://img.atcoder.jp/ahc018/seeds.txt) (sha256=`3bbc58026730d18fcb0e03ead929ab2afb6f9f0c5eb7f0c10ac44df4e6b84a3f`) after the contest is over.

#### About Relative Evaluation System

 In both the provisional/system test, the standings will be calculated using only the last submission which received a result other than CE. 
 Only the last submissions are used to calculate the MIN for each test case when calculating the relative scores.

 The scores shown in the standings are relative, and whenever a new submission arrives, all relative scores are recalculated. 
 On the other hand, the score for each submission shown on the submissions page is the sum of the stamina consumption for each test case, and the relative scores are not shown. 
 In order to know the relative score of submission other than the latest one in the current standings, you need to resubmit it. 
 If your submission produces illegal output or exceeds the time limit for some test cases, the score shown on the submissions page will be 0, but the standings show the sum of the relative scores for the test cases that were answered correctly.

#### About execution time

 Execution time may vary slightly from run to run. 
 In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percents compared to provisional tests. 
 For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
 Please measure the execution time in your program to terminate the process, or make sure to have enough time margin for the execution.

### Input and Output

 **Note: This problem is interactive. Read the following explanation, and communicate with the judge program accordingly.**

 At first, input will be provided via stdin in the following format.

```text
\(
N ~ W ~ K ~ C \\
a_0 ~ b_0 \\
\vdots \\
a_{W-1} ~ b_{W-1} \\
c_0 ~ d_0 \\
\vdots \\
c_{K-1} ~ d_{K-1}\)

```

-  
 On the 1st row, there are four integers N,W,K,C separated by spaces.

- N represents the size of the plot and is N = 200. 
 
- W represents the number of the **water sources** and satisfies 1 \leq W \leq 4. 
 
- K represents the number of the **houses** and satisfies 1 \leq K \leq 10. 
 
- C is a parameter related to **stamina** consumption and satisfies C \in \{ 1,2,4,8,16,32,64,128 \}.

-  
 The following W+K rows contain the coordinates of the cells containing **water sources** and **houses**, each expressed as a pair of space-separated integers.

- First W rows contain pairs of integers a_i, b_i, representing cells (a_i, b_i) that contain **water sources**. The integers satisfy 0 \leq a_i, b_i \lt N. 
 
- Following K rows contain pairs of integers c_i, d_i, representing cells (c_i, d_i) that contain **houses**. The integers satisfy 0 \leq c_i, d_i \lt N. 
 
- Each pair of **water sources**, each pair of **houses** and each pair of **water sources** and **houses** are guaranteed to be separated by at least [Manhattan distance](https://en.wikipedia.org/wiki/Taxicab_geometry) of  \mathrm{round}(400/(W+K)).

 After receiving the input this far, please output how to perform **excavations** to stdout in the following format.

```text

\(y ~ x ~ P\)

```

-  
 On a single row, output three integers y,x,P separated by spaces.

- An **excavation** will be performed at cell (y,x) with **power** P. 
 
- The **bedrock** on cell (y,x) must not be crushed yet, and **power** must satisfy 1 \leq P \leq 5000. If these conditions are not met, the result will be WA. 
 
- You must **output a newline after the integers, and flush stdout**. If you don't, the result might turn out to be TLE.

 After the output, the result of the performed **excavation** will be provided as an input in the following format.

```text

\(r\)

```

-  
 On a single row, there is a single integer r.

- If the **bedrock** of the designated cell doesn't get crushed, r will be `0`. 
 
- If the **bedrock** of the designated cell gets crushed; in case there are still cells with **houses** without flowing water, r will be `1`, and in the case every cell with a **house** has flowing water, r will be `2`. 
 
- In case of invalid output, r will be `-1`.

- In case of r \in \{-1, 2\}, please exit the program immediately. If you fail to do so, the result might turn out to be TLE. 
 
- In case of r \in \{0, 1\}, there still exists cells with **houses** without flowing water; in that case, you need to continue performing **excavations**. Please continue receiving input and providing output how to perform **excavations** until r \in \{-1, 2\}.

### Input Generation

Let \mathrm{rand}(L,U) be a function that generates a uniform random integer between L and U, inclusive, and \mathrm{randf}(L,U) be a function that generates a uniform random real number between L and U, inclusive, and \mathrm{pow}(b,p) be a function that returns b to the power of p.

#### Generating S_{i,j}

-  
 Let \mathrm{noise}(y,x,seed) be a function that generates two-dimensional [Perlin noise](https://en.wikipedia.org/wiki/Perlin_noise) scaled to range -1.0 - 1.0, inclusive, according to a random seed seed.
 
 Select parameters as the following.

- f0 = \mathrm{randf}(2.0,8.0) 
 
- f1 = \mathrm{randf}(10.0,20.0) 
 
- dy0 = \mathrm{randf}(0.0,1.0) 
 
- dy1 = \mathrm{randf}(0.0,1.0) 
 
- dx0 = \mathrm{randf}(0.0,1.0) 
 
- dx1 = \mathrm{randf}(0.0,1.0) 
 
- seed0 = \mathrm{rand}(0,2^{32} - 1) 
 
- seed1 = \mathrm{rand}(0,2^{32} - 1) 
  
 For each i, j ( 0 \leq i, j \lt N ), let S_{i,j} = \mathrm{noise}(f0 \times i / N + dy0, f0 \times j / N + dx0, seed0) + 0.2 \times \mathrm{noise}(f1 \times i / N + dy1, f1 \times j / N + dx1, seed1).

-  
 Update S_{i,j} = 1.0 / (1.0 + \mathrm{exp}(-3.0 \times (S_{i,j} - 0.25))). (Transformation by logistic function.)

-  
 Select p = \mathrm{randf}(2.0,4.0). Update S_{i,j} = \mathrm{pow}(S_{i,j},p).

-  
 Scale S_{i,j} linearly to range 10 - 5000, inclusive.
 
 I.e. let l be minimum and u be maximum of S_{i,j} ( 0 \leq i, j \lt N ), and update S_{i,j} = \mathrm{round}((S_{i,j} - l) \times (5000 - 10) / (u - l) + 10).

#### Generating the locations of water sources and houses

- Let W = \mathrm{rand}(1,4), K = \mathrm{rand}(1,10). 
 
- Choose the coordinates (a_i, b_i) of the W water sources and the coordinates (c_i, d_i) of the K houses independently from the N ^ 2 cells with a probability inversely proportional to S_{i,j}. 
 
- In case the Manhattan distance between any pairs of the W+K coordinates is under \mathrm{round}(400/(W+K)), go back to step 2 and regenerate.

#### Generating C

Given by C = \mathrm{pow}(2,\mathrm{rand}(0,7)).

You are not required to understand this.

Please refer to the source code of the input generator for implementation details.

### Tools (Input generator, visualizer, local tester, and sample code)

- [Web Version](https://img.atcoder.jp/ahc018/6bada50282.html?lang=en): For displaying animated visualizations. 
 
- [Local Version](https://img.atcoder.jp/ahc018/6bada50282.zip): Please set up a [Rust language](https://www.rust-lang.org) build environment to use this. 
 
- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc018/6bada50282_windows.zip): If you have troubles setting up Rust environment, use this.

- [Sample code (C++, Python)](https://img.atcoder.jp/ahc018/6bada50282_sample.zip): Sample answers. Implemented as follows:

- Performs **excavations** to build watercourses from the first **water source** to each **house** by the shortest, direct route. 
 
- Uses constantly `100` as the **power** setting. 
 
- Once the judge program returns `2`, prints the consumed **stamina** to stderr and exits.

 **Note that sharing visualization results is forbidden until the contest ends.**

#### Input format used in the tools

The input file for the local tester uses the following format.

```text
\(
N ~ W ~ K ~ C \\
S_{0,0} ~ \cdots ~ S_{0,N-1} \\
\vdots \\
S_{N-1,0} ~ \cdots ~ S_{N-1 ,N-1} \\
a_0 ~ b_0 \\
\vdots \\
a_{W-1} ~ b_{W-1} \\
c_0 ~ d_0 \\
\vdots \\
c_{K-1} ~ d_{K-1} \)

```

S_{i,j} is the **sturdiness** value of the **bedrock** on cell (i, j).

 The local tester writes outputs from your program directly to the output file. 
 Your program may output comment lines starting with `#`. The web version of the visualizer displays the comment lines with the corresponding **excavations**, which may be useful for debugging and analysis. 
 Since the judge program ignores all comment lines, you can submit a program that outputs comment lines as is.

### I/O Examples

  | Input 
  | Output 
  | Explanation

  |

```text

3 1 1 128
0 0
1 1

```

  |  
  |  
 Please note that this input is for explanation purposes, and thus doesn't meet all of the guarantees for input.
 
 On a 3 \times 3 grid, there exists a **water source** on cell (0,0), and a **house** on cell (1,1).

  |  
  |

```text

0 0 872

```

  |  
 An **excavation** is performed at cell (0,0) with **power** 872. Accordingly, **stamina** is consumed by 128+872 = 1000.

  |

```text

0

```

  |  
  |  
 This **excavation** doesn't manage to crush the **bedrock** on cell (0,0).
 
 However, the **sturdiness** of the **bedrock** on cell (0,0) diminishes by 872.

  |  
  |

```text

0 0 2

```

  |  
 An **excavation** with **power** 2 is performed at cell (0,0). Accordingly, **stamina** is consumed by 128+2 = 130.

  |

```text

1

```

  |  
  |  
 This **excavation** manages to crush the **bedrock** at cell (0,0).
 
 Water flows into cell (0,0).

  |  
  |

```text

1 1 872

```

  |  
 An **excavation** with **power** 872 is performed at cell (1,1). Accordingly, **stamina** is consumed by 128+872 = 1000.

  |

```text

1

```

  |  
  |  
 The **bedrock** on cell (1,1) gets crushed.

  |  
  |

```text

1 0 872

```

  |  
 An **excavation** with **power** 872 is performed at cell (1,0). Accordingly, **stamina** is consumed by 128+872 = 1000.

  |

```text

2

```

  |  
  |  
 The **bedrock** on cell (1,0) gets crushed.
 
 Water is now flowing into cells (0,0), (1,0) and (1,1), and every cell with a **house** now has flowing water.
 
 As the judge program now returns `2`, please exit the program immediately.
 
 The sum of the consumed **stamina**, which turns out to be 1000+130+1000+1000 = 3130, is used to calculate the relative score.

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

- [ルール](https://atcoder.jp/contests/ahc018/rules)
 
- [用語集](https://atcoder.jp/contests/ahc018/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
