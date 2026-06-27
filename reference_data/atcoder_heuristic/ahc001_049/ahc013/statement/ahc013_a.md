### 問題文

![](https://img.atcoder.jp/ahc013/d82f46552bdf5f406aaba84ec8a57d9d.gif)

 株式会社リクルートには N 行 N 列のグリッドで表されるサーバ室があります。
 
 サーバ室の上から i 行目（  0 \leq i \lt N  ）、左から j 列目（  0 \leq j \lt N  ）のセルを (i, j) と表します。
 
 各セルには、コンピュータを最大1個まで置くことができます。
 
 また、サーバ室には K 種類のコンピュータが 100 個ずつあります。
 
 既に  100 \times K  個のコンピュータが  100 \times K  個のセルに置かれていて、残りの  N^2 - 100 \times K  個のセルには何も置かれていません。

 Xさんはコンピュータを何度か**移動**した後、コンピュータ同士をまっすぐなケーブルで**接続**します。
 
 **移動** と **接続** とは次のような操作を指します。

-  
 **移動**：動かしたいコンピュータを1個選び、上下左右の四方向に隣接するセルのどれかへ動かします。一度動かしたコンピュータを複数回動かすこともできます。次の条件をすべて満たす必要があります。

- 移動先として他のコンピュータが存在するセルを選ぶことはできない。 
 
- コンピュータをサーバ室の外へ移動することはできない。

-  
 **接続**：つなぎたいコンピュータを2個選び、それら同士をまっすぐなケーブルでつなぎます。次の条件をすべて満たす必要があります。

- 選んだ2個のコンピュータは同じ行か同じ列に存在し、間に他のコンピュータが存在しない。  
 
- ケーブルは他のケーブルと交差しない。（あるコンピュータに複数の方向からケーブルを接続することは可能） 
 
- 既にケーブルで直接つながっているコンピュータ同士を再び指定することはできない。 
 
- あるコンピュータの接続先として、そのコンピュータ自身を指定することはできない。

 コンピュータたちを**接続**することで**クラスタ**が作られます。
 
 あるコンピュータ C_i から 別のコンピュータ C_jにケーブルを1回以上通ってたどり着くことができる場合に、またその場合に限り、 C_i と C_j は「同じ**クラスタ**に属する」とみなします。

 Xさんは上手に**クラスタ**を作り、**サーバ室の処理性能**をなるべく上げたいです。
 
 **サーバ室の処理性能**は、**クラスタ**内に種類が同じコンピュータの組の数が多ければ多いほど上がります。
 
 一方で、**クラスタ**内に種類が異なるコンピュータの組の数が多ければ多いほど**サーバ室の処理性能**は下がってしまいます。

 厳密には、**サーバ室の処理性能**は、次のように計算されます。

-  サーバ室に置かれている 100 \times K 個のコンピュータを C_0, C_1, \cdots, C_{100K-1} と表す。 
 
-  
 0 \leq i \lt j \lt 100 \times K  を満たす全ての整数の組 (i, j) について、以下の点を計算したものの和を**サーバ室の処理性能**とする。

-  C_i と C_j が同じ**クラスタ**に属していて、種類が同じ場合は 1 点  
 
-  C_i と C_j が同じ**クラスタ**に属していて、種類が異なる場合は -1 点  
 
-  C_i と C_j が同じ**クラスタ**に属さない場合は 0 点

 Xさんの体力は限られているため、**移動** を行う回数と **接続** を行う回数の和は  100 \times K  以下である必要があります。
 
 なるべく**サーバ室の処理性能**が大きくなるような**移動**と**接続**の仕方を見つけてください。

### 得点

 1つのテストケースでは、**サーバ室の処理性能**が 0 点より大きい場合はそれがそのまま得点となり、そうでない場合は 0 点となります。

 各テストケースの得点の合計が提出の得点となります。 
 暫定テストでは、一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなります。 
 システムテストでは、不正な出力や制限時間超過をした場合、そのテストケースのみ 0 点となります。

#### テストケース数

-  
 暫定テスト: 50個

- 暫定テストでは、入力の制約を満たす N および K がランダムに選ばれます。

-  
 システムテスト: 2000個

- システムテストでは、各 N と K の組（計100組）のデータが20個ずつ存在します。 
 
- コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc013/seeds.txt) (sha256=`0a50dccf4689292f69a36be0ea1a99767de055434b854f2b0bc7f1fd40f9033e`) を公開します。

#### 実行時間について

 実行時間には多少のブレが生じます。 
 また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
 そのため、実行時間制限ギリギリの提出がシステムテストでTLEとなる可能性があります。 
 プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入力

入力は以下の形式で標準入力から与えられます。

```text

\(\begin{array}{lll}
    N ~ K & & \\
    c_{ 0,0 } & \ldots & c_{ 0,N-1 } \\
    \vdots & \ddots & \vdots \\
    c_{ N-1,0 } & \ldots & c_{ N-1,N-1 }
  \end{array}\)

```

 1行目には2つの整数 N と K を含みます。続く N 行のそれぞれには、長さ N の文字列が1つ与えられます。

-  
 N と K はサーバ室の大きさとコンピュータの種類数を表す整数で、以下のいずれかを満たします。

- K=2 かつ 15 \leq N \leq 39 
 
- K=3 かつ 18 \leq N \leq 42 
 
- K=4 かつ 21 \leq N \leq 45 
 
- K=5 かつ 24 \leq N \leq 48

-  
 c_{ i,j } は セル (i, j)の初期状態を表す整数で、 0 \leq c_{i,j} \leq K  を満たします。

- c_{ i,j } = 0 の場合、コンピュータが置かれていないことを表します。コンピュータが置かれていないセルは必ず N^2 - 100 \times K 個であることが保証されます。 
 
-  1 \leq c_{ i,j } \leq K の場合、種類が c_{i,j} であるコンピュータが置かれていることを表します。各種コンピュータは必ず 100 個ずつ置かれていることが保証されます。

### 出力

以下の形式で標準出力へ出力してください。

```text

\(
X \\
a_0 ~ b_0 ~ c_0 ~ d_0 \\
\vdots \\
a_{X-1} ~ b_{X-1} ~ c_{X-1} ~ d_{X-1} \\
Y \\
e_0 ~ f_0 ~ g_0 ~ h_0 \\
\vdots \\
e_{Y-1} ~ f_{Y-1} ~ g_{Y-1} ~ h_{Y-1}
\)

```

 1行目に、**移動**を行う回数を表す整数 X を出力してください。
 
 続く X 行には、**移動**の情報を表す4つの整数を**移動**を行う順に出力してください。
 
 i 行目の整数 a_i, b_i, c_i, d_i は、 (a_i, b_i) に置かれているコンピュータを (c_i, d_i) へ**移動**することを表します。

 その後、**接続**を行う回数を表す整数 Y を出力してください。
 
 続く Y 行には、**接続**の情報を表す4つの整数を出力してください。
 
 i 行目の整数 e_i, f_i, g_i, h_i は、 (e_i, f_i) に置かれているコンピュータ と (g_i, h_i) に置かれているコンピュータを**接続**することを表します。

 また、以下の制約をすべて満たす必要があります。満たさない場合はWAとなります。

-  0 \leq X, ~ 0 \leq Y 
 
-  X + Y \leq 100 \times K 
 
- **移動**と**接続**は、問題文中に書かれている条件を満たしていること

 解が複数出力された場合、一番 **最初** のもののみが採点に用いられます。ビジュアライザでは複数出力された解の比較が可能です。

### 入力生成方法

 各ケースはテストケースジェネレータによって生成されています。
 
 テストケースジェネレータはページ下部のリンクより提供しています。
 
 テストケースジェネレータは以下の手順でケースを生成します。厳密には実装を見てください（必ずしも目を通す必要はありません）。

- K は、seed値を4で割った余り + 2 により生成されます。seed値を調整することで特定の K の値を持つ入力を生成できます。 
 
- N は、入力の条件を満たすものの中から一様ランダムに生成されます。 
 
- c_{i,j} は、入力の条件を満たすものの中から一様ランダムに生成されます。

### ツール

- [Web版ビジュアライザ・入力ジェネレータ](https://img.atcoder.jp/ahc013/a42302b3af.html?lang=ja): アニメーション表示が可能です。 
 
- [ローカル版ビジュアライザ・入力ジェネレータ](https://img.atcoder.jp/ahc013/a42302b3af.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 
 
- [サンプルコード（C++, Python）](https://img.atcoder.jp/ahc013/4c1bf2fcb4366d2f8b77ed1f53aeaf16.zip): そのまま提出もできるサンプルの解答です。次の処理を実装しています。

- ランダムな**移動**を何度か行う 
 
- それぞれのコンピュータから、右と下に向かって、同じ種類のコンピュータに**接続**できるならする 
 
- スコアを計算し、標準エラー出力に出力する

 **コンテスト終了までビジュアライズ結果の共有は禁止されています。ご注意下さい。**

### 入力例 1 Copy 
 Copy

```text

5 2
10000
00000
00200
00000
00111

```
 
 この入力は説明用のものなので、入力の制約を満たさないことに注意してください。

### 出力例 1 Copy 
 Copy

```text

2
0 0 0 1
0 1 0 2
4
0 2 2 2
2 2 4 2
4 2 4 3
4 3 4 4

```

- (0,0) にあるコンピュータを2回連続で**移動**し、 (0,2) へ動かします。 
 
- すべてのコンピュータを**接続**します。 
 
-  
 この時の**サーバ室の処理性能**は2となります。

- すべてのコンピュータからなる**クラスタ**が1つあり、種類1のコンピュータが4個、種類2のコンピュータが1個で構成されています。 
 
- **クラスタ**内に種類1同士のコンピュータの組は  {4 \choose 2} = 6 つあり、種類1と種類2のコンピュータの組は  4 \times 1 = 4つあるので、  6 - 4 = 2  が **サーバ室の処理性能** となります。

### 入力例 2 Copy 
 Copy

```text

33 3
200000020020030300300012000000200
000000000333000000002200002030201
000100010000003200020000031001010
000023200000000000202300010000030
000010000023000300310300003000010
100010030001000032100000000103000
000000030100030300000000002000000
100000000120200000000000001001000
120030000200000000001001032000302
000000020003033020000013020002301
030000301300000000020200002003000
002000000020000000030103002000100
331210020020020000200200200103000
000202320000003000000000001000000
000010010030000303120001200000001
103000022010000313000000000000003
000012000010000001000000030000030
002000200003010100000002332000000
000003023000230020000003030000130
010000020001202302210002102030000
000003000000100002002000002002000
010003000200200000010030103000212
001000300000300000130020000003120
000000320300200100300000030002200
000000100003000000020100000300000
010000130110030200200100000110000
000231100000113000102000201003010
000000100030100230000000021000030
300103010000010100100300001022000
201021000001001000020000002000333
000002030300000000030000103100300
020000110320030200012100001030300
000100000010000000020001220030021

```
 
 このケースは、テストケースジェネレータを seed=`1` で実行すると生成されます。出力例は省略します。

### Problem Statement

![](https://img.atcoder.jp/ahc013/d82f46552bdf5f406aaba84ec8a57d9d.gif)

 Recruit Co. has a server room represented by a grid of N rows and N columns.
 
 Let (i, j) denote the cell in row i (0 \leq i \lt N) from the top and column j (0 \leq j \lt N) from the left of the server room.
 
 Each cell can contain up to one computer.
 
 There are K types of computers and 100 computers of each type in the server room.
 
 In the beginning, 100 \times K computers are already placed in 100 \times K cells, and the remaining N^2 - 100 \times K cells are empty.

 X-san can **move** computers around first, and then **connect** them with straight cables.
 
 **Move** and **Connect** is defined as follows:

-  
 **Move** : Select one of the computers and move it to one of the adjacent cells, up, down, left, or right. You can move a computer multiple times. All of the following requirements should be met for each movement:

- You cannot move a computer to a cell where another computer is already located. 
 
- You cannot move a computer outside the grid.

-  
 **Connect** : Select two computers and connect them to each other with a straight cable. All of the following requirements should be met for each connection:

- The two computers you choose must be on the same row or in the same column, with no other computers in between. 
 
- The cables do not intersect with any other cables. (Connecting multiple cables in one computer is allowed.) 
 
- You cannot connect the same pair of computers twice. 
 
- The two computers you choose must be different (one cannot connect a computer to itself with a single cable).

 **Clusters** are created by connecting computers.
 
 Computers C_i and C_j are considered to be "in the same cluster" if and only if computer C_j can be reached from computer C_i by a path over computers connected with cables.

 X-san wants to create good **clusters** and increase the **processing performance of the server room** as much as possible.
 
 The more pairs of computers of the same type in a **cluster**, the better the **processing performance of the server room**.
 
 On the other hand, the more pairs of computers of different types there are in the **cluster**, the lower the **processing performance of the server room**.

 The **processing performance of a server room** is calculated as follows:

- Denote the 100 \times K computers in the server room as C_0, C_1, \cdots, C_{100K-1} 
 
-  
 The **processing performance of the server room** is the sum of the following points summed over every integer pair (i, j) satisfying 0 \leq i \lt j \lt 100 \times K:

- 1 point if C_i and C_j belong to the same **cluster** and have the same type 
 
- -1 point if C_i and C_j belong to the same **cluster** and are of different types 
 
- 0 points if C_i and C_j do not belong to the same **cluster**

 Considering the physical burden of X-san, the number of **move**s and the number of **connection**s must be less than or equal to 100 \times K in total.
 
 Your challenge is to find a way to maximize the **processing performance of the server room** under all the restrictions we’ve placed above.

### Scoring

 In each test case, if the **processing performance of the server room** is greater than 0 points, it will be the score of the case; otherwise, the test case will be scored 0 points.

 The score of a submission is the total scores for each test case. 
 In the provisional test, if your submission produces illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
 In the system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero.

#### Number of test cases

-  
 Provisional test: 50

- In provisional testing, N and K that satisfy the input constraints are randomly selected.

-  
 System test: 2000

- For the system test, there will be 20 data inputs for each N and K pair (100 pairs in total). 
 
- We will publish [seeds.txt](https://img.atcoder.jp/ahc013/seeds.txt) (sha256=`0a50dccf4689292f69a36be0ea1a99767de055434b854f2b0bc7f1fd40f9033e`) after the contest is over.

#### About execution time

 Execution time may vary slightly from run to run. 
 In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percents compared to provisional tests. 
 For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
 Please measure the execution time in your program to terminate the process, or make sure to have enough time margin for the execution.

### Input

Input is given from standard input (stdin) in the following format:

```text

\(\begin{array}{lll}
    N ~ K & & \\
    c_{ 0,0 } & \ldots & c_{ 0,N-1 } \\
    \vdots & \ddots & \vdots \\
    c_{ N-1,0 } & \ldots & c_{ N-1,N-1 }
  \end{array}\)

```

 The first line contains two integers N and K. Each of the following N lines is given one string of length N.

-  
 N and K are integers representing the size of the server room and the number of computer types, satisfying one of the following

- K=2 and 15 \leq N \leq 39 
 
- K=3 and 18 \leq N \leq 42 
 
- K=4 and 21 \leq N \leq 45 
 
- K=5 and 24 \leq N \leq 48

-  
 c_{ i,j } is an integer representing the initial state of the i-th cell from the top and j-th cell from the left in the server room. 0 \leq c_{i,j} \leq K .

- c_{i,j} = 0 means that no computer is placed in the cell (i, j). There are always N^2 - 100 \times K cells without a computer. 
 
- if  1 \leq c_{ i,j } \leq K, it means the computer of type c_{i,j} is placed in the cell (i,j). It is guaranteed that 100 computers of each type are placed in the grid.

### Output

Please output to standard output (stdout) in the following format:

```text

\(
X \\
a_0 ~ b_0 ~ c_0 ~ d_0 \\
\vdots \\
a_{X-1} ~ b_{X-1} ~ c_{X-1} ~ d_{X-1} \\
Y \\
e_0 ~ f_0 ~ g_0 ~ h_0 \\
\vdots \\
e_{Y-1} ~ f_{Y-1} ~ g_{Y-1} ~ h_{Y-1}
\)

```

 On the first line, output an integer X representing the number of **move**s to be made.
 
 On the following X lines, output four integers representing the information on the **move**s in the order in which they are to be made.
 
 The integers a_i, b_i, c_i, d_i on line i represent **moving** the computer located at (a_i, b_i) to (c_i, d_i)

 Then output an integer Y representing the number of **connection**s to be made.
 
 On the following Y lines, output four integers representing the information of the **connection**.
 
 The integers e_i, f_i, g_i, h_i on line i represent the **connection** between the computer located at (e_i, f_i) and the computer located at (g_i, h_i).

 In addition, all of the following constraints must be satisfied, or the submission will be judged as a WA.

-  0 \leq X, ~ 0 \leq Y 
 
-  X + Y \leq 100 \times K 
 
- The **move**s and the **connection**s must satisfy the conditions written in the problem statement

 If you output more than one solution, only the **first** one is used for scoring. You can compare solutions by using the web visualizer.

### Input Generation

 Each case is generated by a test case generator.
 
 The test case generator is provided at the link at the bottom of the page.
 
 The test case generator generates cases in the following steps. For details, see the exact implementation (you are not required to read it through, however).

- K is generated by the \mathit{seed} \mod 4 + 2. By adjusting the seed value, an input with a specific K value can be generated. 
 
- N is generated uniformly at random from among those satisfying the input conditions. 
 
- c_{i,j} is generated uniformly at random from among those satisfying the input conditions.

### Tools

- [Input generator and visualizer (Web version)](https://img.atcoder.jp/ahc013/a42302b3af.html?lang=en): This can display animations. 
 
- [Input generator and visualizer (Local version)](https://img.atcoder.jp/ahc013/a42302b3af.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org). 
 
- [Sample solution (C++, Python)](https://img.atcoder.jp/ahc013/4c1bf2fcb4366d2f8b77ed1f53aeaf16.zip): They implement the following:

- Make several random **move**s 
 
- From each computer, **connect** to right and/or bottom if it can reach a computer of the same type  
 
- Calculate the score and output it to stderr

 **Sharing visualization results is not allowed until the end of the contest. **

### Sample Input 1 Copy 
 Copy

```text

5 2
10000
00000
00200
00000
00111

```
 
 Note that this input is for explanatory purposes only and does not meet the constraints of the input.

### Sample Output 1 Copy 
 Copy

```text

2
0 0 0 1
0 1 0 2
4
0 2 2 2
2 2 4 2
4 2 4 3
4 3 4 4

```

- Move the computer at (0, 0) to (0, 2) by moving twice. 
 
- Connect all computers. 
 
-  
 The **processing performance of the server room** at this time is 2.

- There is one **cluster** consisting of all computers, 4 computers of type 1 and 1 computer of type 2. 
 
- The pairs of type 1 computers in the **cluster** are  {4 \choose 2} = 6, and there are  4 \times 1 = 4 pairs of computers between type 1 and type 2 in the **cluster**, so the **processing performance of the server room** is  6 - 4 = 2 .

### Sample Input 2 Copy 
 Copy

```text

33 3
200000020020030300300012000000200
000000000333000000002200002030201
000100010000003200020000031001010
000023200000000000202300010000030
000010000023000300310300003000010
100010030001000032100000000103000
000000030100030300000000002000000
100000000120200000000000001001000
120030000200000000001001032000302
000000020003033020000013020002301
030000301300000000020200002003000
002000000020000000030103002000100
331210020020020000200200200103000
000202320000003000000000001000000
000010010030000303120001200000001
103000022010000313000000000000003
000012000010000001000000030000030
002000200003010100000002332000000
000003023000230020000003030000130
010000020001202302210002102030000
000003000000100002002000002002000
010003000200200000010030103000212
001000300000300000130020000003120
000000320300200100300000030002200
000000100003000000020100000300000
010000130110030200200100000110000
000231100000113000102000201003010
000000100030100230000000021000030
300103010000010100100300001022000
201021000001001000020000002000333
000002030300000000030000103100300
020000110320030200012100001030300
000100000010000000020001220030021

```
 
 This case is generated by running the test case generator with seed=`1`. Output examples are omitted.

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

- [ルール](https://atcoder.jp/contests/ahc013/rules)
 
- [用語集](https://atcoder.jp/contests/ahc013/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
