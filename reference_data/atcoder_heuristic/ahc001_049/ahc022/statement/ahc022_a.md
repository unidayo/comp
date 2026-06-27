### ストーリー

 人類は我々の世界の外にあるもう一つの空間 Another Space を発見した。
 
 Another Space は L 行 L 列のグリッドで表されており、グリッドはトーラスを成している。 
 つまり、最も上の行から1マス上に移動すると同じ列の最も下の行に到達する。左右も同様である。

 Another Space への侵入には、 **ワームホール** を用いる。
 
 **ワームホール** は N 個あり、それぞれが独立していて、我々のいる世界と Another Space 内のセルのどれか一つを繋げている。
 
 **ワームホール** が繋がっている Another Space 内のセルを **出口セル** と呼ぶ。 
 **出口セル** は N 個存在し、各 **ワームホール** は 1 つの **出口セル** と一対一対応している。 
 しかし、N 個の **出口セル** のグリッド上での位置はわかっているが、どの **ワームホール** が どの **出口セル** と繋がっているかは分かっていない。
 
 その対応を見つけたい。

Xさんは、**ワームホール**の対応推定を以下の手順で行うことを思いついた。

- Another Space の各セルに空調設備を1つずつ置き、それぞれ整数の温度を設定する。  
 
- 次の計測を繰り返し行う。

- **ワームホール**を1つ選択し、その入口から Another Space に侵入する。 
 
- 事前に計画していた移動を Another Space 内で行い、移動終了後にそのセルの気温を計測する。

なお、計測機器の都合上、計測した気温には誤差が含まれる。

 設定する温度には注意が必要だ。空調設備はセル内を設定された温度に保つが、上下左右に隣接するセル同士の温度差が激しいと、多くのエネルギーを使ってしまう。
 
 また、 Another Space 内での気温計測にもエネルギーが必要で、計測回数や移動距離が多いほど多くのエネルギーが必要になる。

 天才助手であるあなたのミッションは、Xさんの**ワームホール**対応推定をなるべく少ないエネルギーで行えるように計画することである。 
 Another Space の**ワームホール**対応を解き明かし、人類の未来を拓いてほしい。

### 問題文

 L \times L のグリッドが存在し、グリッドの上から i 行目（  0 \leq i \lt L  ）、左から j 列目（  0 \leq j \lt L  ）のセルを (i, j) で表す。 
 グリッドの上下、左右はトーラス型に繋がっている。 
 すなわち、 (0, j) と (L - 1, j) は上下に隣接し、 (i, 0) と (i, L - 1) は左右に隣接する。
 
 セルのうち N 個は **出口セル** と呼ばれ、それらの座標を (Y_0, X_0), (Y_1, X_1), \ldots, (Y_{N-1}, X_{N-1}) とする。
 
 ある 0,1,\ldots,N-1 の順列 A=(A_0,A_1,\ldots,A_{N-1}) が存在する。 
 これは i 番目の **ワームホール** に入ると、 A_i 番目の **出口セル** に出ることを表す。 
 A の値は隠されており、それを推定することが問題の目的である。

 推定は、以下で説明する **配置**, **計測**, **回答** を順に行うことで達成する。

#### 配置

 **配置** として、グリッドの各セルに 0 以上 1000 以下の整数値を指定する。
 
 (i,j) に指定した値を P_{i,j} （0 \leq P_{i,j} \leq 1000）とする。 
 
 すべての上下左右に隣り合うセルについて、指定した値の差の2乗の和をとったものを**配置時コスト**とする。
 
 **配置時コスト** =  \sum_{i=0}^{L-1} \sum_{j=0}^{L-1} ((P_{i,j} - P_{(i+1)\%L,j})^2 + (P_{i,j} - P_{i,(j+1)\%L})^2)

#### 計測

以下の**計測** を0 回以上 10000 回以下行う。

- 整数 i, y, x を指定する。

- i は**ワームホール**の番号を表す。

- (Y_{A_i}, X_{A_i}) から下に y ,右に x 移動したセルを (r, c)とする。 y, x が負の数の場合は、それぞれ 上に |y|, 左に |x| 移動することを意味する。

- すなわち、r \equiv Y_{A_i}+y \pmod L, c \equiv X_{A_i}+x \pmod L (0 \leq r,c \lt L) である。 
 
- **ワームホールと出口セルの対応は分かっていないので、何番目の出口セルから移動を開始するかは不明である。(Y_{A_i}, X_{A_i}) は不明な値である事に注意せよ。**

- 平均 0 ,標準偏差 \sigma の正規分布から値をサンプリングする関数を f(\sigma) とする。 
 
- 計測値として \mathrm{max}(0, \mathrm{min}(1000, \mathrm{round}(P_{r,c} + f(S)))) が得られる。

- ここで、 S は入力で与えられる正整数である。 
 
- P_{r,c} の真値は分からず、計測値のみが得られる事に注意せよ。

 1回の**計測**によって 100 * (10 + |y| + |x|) のコストが発生する。
 
 すべての**計測**についてのコストの和を**計測時コスト**とする。
 
 同じ位置を繰り返し**計測**しても良い。**計測**のたびに新たにサンプリングされた値が得られる。

#### 回答

 A の推定値 E_0, E_1, \ldots , E_{N-1} を出力する。

### 得点

 N個の回答のうち誤りだったものの数、すなわち A_i \ne E_i となる i の数を W とすると、**得点**は以下のようになる。
 
 **得点** =  \lceil 10^{14} * 0.8^W / (配置時コスト + 計測時コスト + 10^{5}) \rceil  
 
 ここで、  \lceil x \rceil  は x 以上の最小の整数を表す。

### 順位付け

 相対評価システムを採用します。
 
 各テストケース毎に、\mathrm{round} \lparen 10^9 \times \frac{自身の得点}{全参加者中の最高得点} \rparen の相対評価得点が得られ、その和が提出の得点となります。

 最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定されます。暫定テスト・システムテストのどちらにおいても、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースに関する相対評価得点は0点となり、そのテストケースにおいては「全参加者中の最大得点」の計算対象から除外されます。

 また、システムテストは **CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意してください。

#### テストケース数

-  
 暫定テスト: 50個

-  
 システムテスト: 3000個

- システムテストでは、S のそれぞれの値に対して100個ずつのテストケースが存在します。 
 
- コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc022/seeds.txt) (sha256=`2154215b63e063812cf2bb0ee2370c5193ae4682a3f7ce260e82a52a07663206`) を公開します。

#### 相対評価システムについて

 暫定テスト・システムテストともに、CE以外の結果を得た一番最後の提出のみが順位表に反映されます。
 
 相対評価得点の計算に用いられる各テストケース毎の「全参加者中の最大得点」の算出には、各参加者の順位表に反映されている提出のみが用いられます。
 
 順位表に表示される得点は相対評価得点であり、新規提出があるたびに相対評価得点が再計算されます。 一方、提出一覧から確認できる各提出の得点は、各テストケース毎の得点をそのまま足し合わせたものであり、相対評価得点は表示されません。 最新以外の提出の、現在の順位表における相対評価得点を知るためには、再提出が必要となります。
 
 不正な出力や制限時間超過をした場合、提出一覧から確認できる得点は0となりますが、順位表には正解したテストケースに対する相対評価得点の和が表示されます。

#### 実行時間について

 実行時間には多少のブレが生じます。 
 また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
 そのため、実行時間制限ギリギリの提出がシステムテストで TLE となる可能性があります。 
 プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入出力

**※この問題はインタラクティブ形式である。以下の内容を読み、ジャッジプログラムとのやり取りを行うこと。**

最初に問題のパラメタが以下の形式で標準入力から与えられる。

```text
\(
L ~ N ~ S \\
Y_0 ~ X_0 \\
\vdots \\
Y_{N-1} ~ X_{N-1} \)

```

- 1行目には3つの整数 L,N,S がスペース区切りで与えられる。

- L はグリッドのサイズを表す整数で、 10 \le L \le 50 を満たす。 
 
- N は **出口セル** の数で、60 \le N \le 100 を満たす。 
 
- S は計測値の計算に用いられる標準偏差で、S \in \{i^2 | 1 \leq i \leq 30\} を満たす。i は整数である。

- 続く N 行には **出口セル** の座標情報が与えられる。

- 各行には 2 つの整数 Y_i, X_i がスペース区切りで含まれており、i 番目の **出口セル** の座標が (Y_i, X_i) であることを表す。 
 
- 0 \leq Y_i \lt L, 0 \leq X_i \lt L を満たす。 
 
- 座標は全て相異なり、辞書順に与えられる。

上記の入力を受け取った後は**配置**を行う。各セルの値 P_{i, j} を以下の形式で標準出力へ出力せよ。

```text
\(
P_{0,0} ~ \cdots ~ P_{0,L-1} \\
\vdots \\
P_{L-1,0} ~ \cdots ~ P_{L-1,L-1}\)

```

- L 行のそれぞれに L 個の整数をスペース区切りで出力せよ。 
 
- 各 P_{i, j} は整数で、 0 \le P_{i, j} \le 1000 を満たす必要がある。 
 
- 制約を満たさなかった場合は、 WA となる。 
 
- **出力の後には改行をし、標準出力をflushする必要がある。そうしなかった場合、 TLE となる可能性がある。**

 続けて**計測** を行う。**計測** は 0 回以上 10000 回以下繰り返し行う事ができる。
 
 **計測**に関するパラメタを以下の形式で標準出力へ出力せよ。

```text
\(
i ~ y ~ x\)

```

- 1行目に3つの整数 i, y, x をスペース区切りで出力せよ。

- i は **ワームホール** の番号を表す。 
 
- y, x は i 番目の **ワームホール** に対応する **出口セル** から移動する距離を表す。 
 
- i, y, x は  0 \leq i \lt N, -L \lt y, x \lt L を満たす必要がある。そうしなかった場合は WA となる。

- 出力の後には改行をし、標準出力をflushする必要がある。そうしなかった場合、 TLE となる可能性がある。

**計測**する位置を出力すると、**計測**によって得られた値が以下の形式で標準入力から与えられる。

```text
\(
m\)

```

- 1行目に1つの整数 m が与えられる。

- m は \mathrm{max}(0, \mathrm{min}(1000, \mathrm{round}(P_{r,c} + f(S)))) で計算される計測値である。

- 不正な値を出力した場合は `-1` が返されるので、プログラムを終了せよ。しなかった場合の結果は不定である。 
 
- **配置**で出力した P の値が不正だった場合は、初回の計測のときに `-1` を返す。

**計測** を終了して **回答** するときは、以下の形式で **ワームホール** と **出口セル** の対応を標準出力へ出力せよ。

```text
\(
-1 ~ -1 ~ -1 \\
E_{0} \\
\vdots \\
E_{N-1}\)

```

- 1行目は3つの整数 -1 ~ -1 ~ -1 をスペース区切りで出力せよ。

- これは **計測** の終了を表す。

- 続く N 行に、 A の推定値である E の値を改行区切りでを出力せよ。

- これは、i 番目の **ワームホール** が E_{i} 番目の **出口セル** に対応すると **回答** することを表す。 
 
- 0 \le E_{i} \lt N を満たさなかった場合は WA になる。 
 
- 同じ番号を複数回出力しても良い。

- 出力の後には改行をし、標準出力を flush する必要がある。そうしなかった場合、 TLE となる可能性がある。

### 入力生成方法

- L, N, S は、それぞれあり得る値の中から一様ランダムに一つを選んで生成する。 
 
- Y_{i}, X_{i} は、L \times L 通りのグリッドの位置の中から互いに異なる N 個を一様ランダムに選び、それらを整数ペアとして辞書順ソートすることで生成する。 
 
- A_{i} は、0,1,\ldots,N-1 の順列の中から一様ランダムに一つを選ぶことで生成する。

### ツール

- [Web版ビジュアライザ・入力ジェネレータ](https://img.atcoder.jp/ahc022/u4u4grds7n.html): アニメーション表示が可能です。 
 
- [ローカル版テスタ・ビジュアライザ・入力ジェネレータ](https://img.atcoder.jp/ahc022/u4u4grds7n.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 
 
- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc022/u4u4grds7n_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

- [サンプルコード（C++, Python）](https://img.atcoder.jp/ahc022/u4u4grds7n_sample.zip): サンプルの解答です。次の処理を実装しています。

- i 番目の**出口セル**の位置について P の値を 10 * i に、**出口セル**が存在しない位置は 0 に設定する 
 
- 各**ワームホール**について、 y=0, x=0 で1回**計測**し、**出口セル**の中で P の値が計測値と最も近いものを推定結果として回答する

 **コンテスト終了までビジュアライズ結果の共有は禁止されています。ご注意下さい。**

#### ツールで用いられる入出力ファイルの仕様

ローカルテスタに与える入力ファイルは以下の形式を用いています。

```text
\(
L ~ N ~ S \\
Y_0 ~ X_0 \\
\vdots \\
Y_{N-1} ~ X_{N-1} \\
A_{0} \\
\vdots \\
A_{N-1} \\
f_{0} \\
\vdots \\
f_{9999}\)

```

f_{i} は、i 回目の**計測**で P の値に加えられる誤差を表す整数です。

ローカルテスタは解答プログラムの出力をそのまま出力ファイルに書き出します。
 
 解答プログラムは、`#` から始まるコメント行を出力に含めてもかまいません。 
 Web版ビジュアライザを使用すると、コメント行を対応する**計測**と合わせて表示できるため、デバッグや考察等に役立てることができます。 
 ジャッジプログラムはコメント行を全て無視するため、コメント行を出力するプログラムをそのまま提出可能です。

### 入出力例

  | Input 
  | Output 
  | 説明

  |

```text

3 3 1
0 0
0 1
2 1

```

  |  
  |  
 入力の制約を満たさない、説明用の入力である点に注意せよ。
 
 3 \times 3 のグリッドの (0,0), (0,1), (2,1) に **出口セル** が存在する。

  |  
  |

```text

4 1 2
1 2 1
2 9 2

```

  |  
 最初の出力として、 **配置** を行う。各セルに対応する P_{i, j} の値を設定する。

  |   
  |

```text

0 0 0

```

  |  
 続けて **計測** を行う。 
 この **計測** では、0 番目の **ワームホール** を指定し、**出口セル** から移動はしない。

  |

```text

8

```

  |   
  |  
 計測値として 8 が得られた。

  |   
  |

```text

1 1 1

```

  |  
 続く **計測** では、1 番目の **ワームホール** を指定し、**出口セル** から下に1、右に1移動した場所を対象とした。

  |

```text

1

```

  |   
  |  
 **計測** の結果として 1 が得られた。

  |   
  |

```text

2 1 1

```

  |  
 直前のものと同様の計測を 2 番目の **ワームホール** に対して行う。

  |

```text

1

```

  |   
  |  
 再び計測値として 1 が得られた。

  |   
  |

```text

-1 -1 -1
2
0
0

```

  |  
 **計測** を終了し、 **回答** を行う。
 
 0 番目の **ワームホール** は (2, 1) に位置する 2 番目の **出口セル** と対応すると推定した。 
 1 番目と 2 番目の **ワームホール** は判断できなかったため、どちらも0番目の **出口セル** が対応していると **回答** した。

### Story

 Humanity has discovered another area outside of our world, called `Another Space`. 
 
 Another Space is represented by an L \times L grid, which forms a torus. 
 This means that moving one square up from the top row will bring you to the bottom row of the same column, and the same applies for left and right movements.

 To enter Another Space, we use **wormholes**.
 
 There are N **wormholes**, each of which is independent and connects our world to one cell in Another Space. 
 The cells in Another Space that are connected to the **wormholes** are called **exit cells**. 
 There are N **exit cells** in Another Space, and each **wormhole** corresponds to one **exit cell**. 
 We know the positions of the **exit cells** on the grid. However, we don't know the specific correspondence between the **wormholes** and the **exit cells**. We want to find this correspondence.

X-san, a great scientist, came up with a plan to estimate the correspondence between the **wormholes** and the **exit cells** using the following steps:

- X-san would place an air conditioning unit in each cell of Another Space and set an integer temperature for each unit. 
 
- X-san would repeat the following measurements:

- Choose one **wormhole** and enter Another Space through it. 
 
- Perform the predetermined movements within Another Space and measure the temperature of the cell after completing the movements.

It's important to note that there will be errors in the measured temperatures due to the limitations of the measuring equipment.

 We must be careful when setting the temperatures. The air conditioning units can maintain the temperature within each cell, but if there is a significant temperature difference between adjacent cells, it will consume a considerable amount of energy.
 
 Furthermore, measuring the temperatures within Another Space also requires energy, and the more measurements and longer movements are involved, the more energy will be required.

 As the brilliant assistant, your mission is to devise a plan that allows X-san to estimate the **wormhole** correspondence in Another Space with the least amount of energy possible. 
 Unravel the mystery of the **wormhole** correspondence and pave the way for the future of humanity.

### Problem Statement

 There exists an L \times L grid, where the cell at the i-th row ( 0 \leq i \lt L ) from the top and j-th column ( 0 \leq j \lt L ) from the left is denoted as (i, j). 
 The top and bottom, as well as the left and right edges of the grid, are connected in a torus shape. 
 In other words, (0, j) is adjacent to (L-1, j) vertically, and (i, 0) is adjacent to (i, L-1) horizontally.
 
 There are N **exit cells** in the grid, and their coordinates are denoted as (Y_0, X_0), (Y_1, X_1), \ldots, (Y_{N-1}, X_{N-1}).
 
 There exists a permutation A=(A_0,A_1,\ldots,A_{N-1}) of the integers 0,1,\ldots,N-1. 
 This permutation indicates that when entering the i-th **wormhole**, one will exit through the A_i-th **exit cell**. 
 The values of A are hidden, and the purpose of this problem is to estimate them.

 The estimation process is achieved through the following steps: **placement**, **measurement**, and **answer**, which are performed in sequence.

#### Placement

 In the **placement** step, assign an integer value ranging from 0 to 1000 (inclusive) to each cell in the grid. 
 
 The assigned value for cell (i, j) is denoted as P_{i,j} （0 \leq P_{i,j} \leq 1000）.
 
 **Placement_cost** is calculated as the sum of the squares of the differences between the specified values for each cell and its adjacent cells in all directions (up, down, left, and right).

 **placement_cost** =  \sum_{i=0}^{L-1} \sum_{j=0}^{L-1} ((P_{i,j} - P_{(i+1)\%L,j})^2 + (P_{i,j} - P_{i,(j+1)\%L})^2)

#### Measurement

Perform the following **measurements** 0 or more times, but no more than 10,000 times:

- Specify integers i, y, and x.

- i represents the index of **wormhole**.

- Let (Y_{A_i}, X_{A_i}) be the starting coordinates. Move y cells down and x cells to the right from (Y_{A_i}, X_{A_i}), resulting in the cell (r, c). 
 If y or x is a negative number, it means moving up by |y| cells or moving left by |x| cells, respectively.

- In other words, r \equiv Y_{A_i}+y \pmod L, c \equiv X_{A_i}+x \pmod L (0 \leq r,c \lt L) . 
 
- Note that the correspondence between the **wormholes** and the **exit cells** is unknown, so it is unclear which **exit cell** to start the movement from. (Y_{A_i}, X_{A_i}) is an unknown value.

- Let f(\sigma) be the function that samples values from a normal distribution with mean 0 and standard deviation \sigma. 
 
- The measured value is obtained as \mathrm{max}(0, \mathrm{min}(1000, \mathrm{round}(P_{r,c} + f(S)))).

- Here, S is a positive integer given as input 
 
- Note that the true value of P_{r, c} is unknown.

 Each **measurement** incurs a cost of 100 * (10 + |y| + |x|).
 
 The sum of costs for all **measurements** is referred to as the **measurement_cost**.
 
 It is allowed to perform **measurements** at the same position repeatedly. Each **measurement** will provide a newly sampled value every time.

#### Answer

 Output the estimated values of A as E_0, E_1, \ldots , E_{N-1}.

### Scoring

 Let W denote the number of wrong answers out of N, i.e., the number of i for which A_i \ne E_i, then the evaluation values are as follows.
 
 evaluation value =  \lceil (10^{14} * 0.8^W / (\mathrm{placement\_cost} + \mathrm{measurement\_cost} + 10^{5})) \rceil  
 
 Here,  \lceil x \rceil  represents the smallest integer greater than or equal to x.

For each test case, we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{YOUR}}{\mathrm{MAX}}), where YOUR is your evaluation value and MAX is the maximum evaluation value among all competitors obtained on that test case. The score of the submission is the sum of the relative scores.

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
 In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MAX calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
 Be careful not to make a mistake in the final submission.

#### Number of test cases

-  
 Provisional test: 50

-  
 System test: 3000

- For the system test, there will be 100 data inputs for each S. 
 
- We will publish [seeds.txt](https://img.atcoder.jp/ahc022/seeds.txt) (sha256=`2154215b63e063812cf2bb0ee2370c5193ae4682a3f7ce260e82a52a07663206`) after the contest is over.

#### About Relative Evaluation System

 In both the provisional/system test, the standings will be calculated using only the last submission which received a result other than CE. 
 Only the last submissions are used to calculate the MAX for each test case when calculating the relative scores.

 The scores shown in the standings are relative, and whenever a new submission arrives, all relative scores are recalculated. 
 On the other hand, the score for each submission shown on the submissions page is the sum of the evaluation value for each test case, and the relative scores are not shown. 
 In order to know the relative score of submission other than the latest one in the current standings, you need to resubmit it. 
 If your submission produces illegal output or exceeds the time limit for some test cases, the score shown on the submissions page will be 0, but the standings show the sum of the relative scores for the test cases that were answered correctly.

#### About execution time

 Execution time may vary slightly from run to run. 
 In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percents compared to provisional tests. 
 For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
 Please measure the execution time in your program to terminate the process, or make sure to have enough time margin for the execution.

### Input and Output

 **Note: This problem is interactive. Read the following explanation, and communicate with the judge program accordingly.**

At first, problem parameters will be provided from Standard Input in the following format.

```text
\(
L ~ N ~ S \\
Y_0 ~ X_0 \\
\vdots \\
Y_{N-1} ~ X_{N-1} \)

```

- On the 1st line, there are three integers L,N,S separated by spaces.

- L represents the size of the grid and satisfies 10 \le L \le 50. 
 
- N represents the number of **exit cells** and satisfies 60 \le N \le 100. 
 
- S represents the standard deviation used in the calculation of measured value and satisfies S \in \{i^2 | 1 \leq i \leq 30\}. Here, i is an integer.

- The following N lines contain the coordinates of the **exit cells**.

- Each line contains space-separated two integers Y_i and X_i, representing the coordinates of the i-th **exit cell** is (Y_i, X_i). 
 
- The integers satisfy 0 \leq Y_i \lt L, 0 \leq X_i \lt L. 
 
- The coordinates all differ from each other and are given in lexicographical order.

After receiving the above input, perform the **placement** step. Output the values of each cell P_{i, j} to Standard Output in the following format.

```text
\(
P_{0,0} ~ \cdots ~ P_{0,L-1} \\
\vdots \\
P_{L-1,0} ~ \cdots ~ P_{L-1,L-1}\)

```

- Output L lines, each containing L integers separated by spaces. 
 
- P_{i, j} must be an integer and satisfy 0 \le P_{i, j} \le 1000. 
 
- If these conditions are not satisfied, the result will be WA. 
 
- **The output must be followed by a new line, and you have to flush Standard Output.** Otherwise, the submission might be judged as TLE.

 Next, proceed with the **measurement** step. The measurement can be repeated from 0 to 10000 times.
 
 Output the parameters related to the **measurement** step in the following format to Standard Output:

```text
\(
i ~ y ~ x\)

```

- On a single line, output three integers i,y,x separated by spaces.

- i represents the index of **wormhole**. 
 
- y, x represent the distance to move from the **exit cell** corresponding to the i-th **wormhole**. 
 
- i, y, x must satisfy  0 \leq i \lt N, -L \lt y, x \lt L. Otherwise, the result will be WA.

- The output must be followed by a new line, and you have to flush Standard Output. Otherwise, the submission might be judged as TLE.

Then, the values obtained from the **measurement** will be provided in the following format from standard input:

```text
\(
m\)

```

- On a single line, there is a single integer m.

- m represents the measured value calculated as \mathrm{max}(0, \mathrm{min}(1000, \mathrm{round}(P_{r,c} + f(S)))).

- In case of invalid output, m will be `-1`. In that case, terminate the program. If you don't, the result is undefined. 
 
- If the values of P that were output during the **placement** step are invalid, `-1` will be returned during the first **measurement**

When finishing the **measurements** and providing the answer, output the correspondence between the **wormholes** and the **exit cells** to the standard output in the following format:

```text
\(
-1 ~ -1 ~ -1 \\
E_{0} \\
\vdots \\
E_{N-1}\)

```

- On the 1st line, output three integers -1 ~ -1 ~ -1 separated by spaces.

- This represents the end of the **measurements**

- On the following N lines, output the value of E, the estimated value of A, separated by newlines.

- E_{i} indicates that the i-th **wormhole** is estimated to correspond to the E_{i}-th **exit cell** 
 
- If E doesn't satisfy 0 \le E_{i} \lt N, the result will be WA. 
 
- You can output a same number multiple times.

- The output must be followed by a new line, and you have to flush Standard Output. Otherwise, the submission might be judged as TLE.

### Input Generation

- L, N, S are generated by choosing one value uniformly at random from all possible values. 
 
- Y_{i}, X_{i} are generated by selecting N distinct positions uniformly at random from the L \times L grid and lexicographically sorting them 
 
- A_{i} is generated by selecting one permutation uniformly at random from the all possible permutations of the integers 0,1,\ldots,N-1

### Tools

- [Web Visualizer / Input Generator](https://img.atcoder.jp/ahc022/u4u4grds7n.html?lang=en): For displaying animated visualizations. 
 
- [Local Tester / Visualizer / Input Generator](https://img.atcoder.jp/ahc022/u4u4grds7n.zip): Please set up a [Rust language](https://www.rust-lang.org) build environment to use this. 
 
- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc022/u4u4grds7n_windows.zip): If you have troubles setting up Rust environment, use this.

- [Sample code (C++, Python)](https://img.atcoder.jp/ahc022/u4u4grds7n_sample.zip): Sample answers. Implemented as follows:

- Set the value of P to 10 * i for the i-th **exit cell** position, and set the value to 0 for positions where there is no **exit cell**. 
 
- For each **wormhole**, perform one **measurement** with y=0 and x=0. Then, estimate the result by selecting the **exit cell** with the P value that is closest to the measured value.

 **Note that sharing visualization results is forbidden until the contest ends.**

#### Input format used in the tools

The input file for the local tester uses the following format.

```text
\(
L ~ N ~ S \\
Y_0 ~ X_0 \\
\vdots \\
Y_{N-1} ~ X_{N-1} \\
A_{0} \\
\vdots \\
A_{N-1} \\
f_{0} \\
\vdots \\
f_{9999}\)

```

f_{i} represents an integer that denotes the error added to the value of P in the i-th measurement.

 The local tester writes outputs from your program directly to the output file. 
 Your program may output comment lines starting with `#`. The web version of the visualizer displays the comment lines with the corresponding **measurements**, which may be useful for debugging and analysis. 
 Since the judge program ignores all comment lines, you can submit a program that outputs comment lines as is.

### I/O Examples

  | Input 
  | Output 
  | Explanation

  |

```text

3 3 1
0 0
0 1
2 1

```

  |  
  |  
 Please note that this input is for explanation purposes, and thus doesn't meet all of the constraints for input.
 
 There are **exit cells** at (0,0), (0,1), and (2,1) in a 3 \times 3 grid.

  |  
  |

```text

4 1 2
1 2 1
2 9 2

```

  |  
 As the initial output, perform the **placement** step. Set the values of P_{i, j} corresponding to each cell.

  |   
  |

```text

0 0 0

```

  |  
 Next, proceed with the **measurement** step. 
 In this **measurement**, the 0-th **wormhole** is specified, and there is no movement from the **exit cell**.

  |

```text

8

```

  |   
  |  
 A measurement value of 8 is obtained.

  |   
  |

```text

1 1 1

```

  |  
 In the following **measurement**, specify the 1st **wormhole** and target the position obtained by moving down 1 cell and right 1 cell from the **exit cell**.

  |

```text

1

```

  |   
  |  
 A measurement value of 1 is obtained.

  |   
  |

```text

2 1 1

```

  |  
 Perform the same **measurement** as the previous one for the 2nd **wormhole**.

  |

```text

1

```

  |   
  |  
 A measurement value of 1 is obtained again.

  |   
  |

```text

-1 -1 -1
2
0
0

```

  |  
 Finish the **measurements** and provide the **answer**.
 
 The 0-th **wormhole** is estimated to correspond to the 2nd **exit cell** located at (2, 1). 
 The 1st and 2nd **wormholes** could not be determined, so both are answered to correspond to the 0-th **exit cell**.

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

- [ルール](https://atcoder.jp/contests/ahc022/rules)
 
- [用語集](https://atcoder.jp/contests/ahc022/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
