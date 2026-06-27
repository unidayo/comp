### ストーリー

AtCoder社の高橋社長は明日のハロウィンに向けて準備をしている。 
AtCoder社のハロウィンは、仮装をした高橋社長が Trick or treat と唱えて、100 人の従業員から順番に1つずつキャンディーを貰うイベントである。 
高橋社長は格子状に 10\times 10 個のキャンディーが入る正方形の箱を用意し、各従業員はキャンディーが重ならないように空いているスペースにキャンディーを入れていく。 
キャンディーはイチゴ味、スイカ味、パンプキン味の 3 種類があり、事前調査によって各従業員がどの味のキャンディーを入れるかは分かっているが、入れる場所は分からない。 
几帳面な高橋社長は、キャンディーを一つ受け取る毎に、箱を前後左右に一度だけ傾けることでキャンディーを動かし、最終的に出来るだけ同じ種類のキャンディー同士が固まっているようにしたいと考えている。 
傾け方を決めるプログラムを作成することで高橋社長の手伝いをして欲しい。 
 
 ![](https://img.atcoder.jp/ahc015/b639c75d_1.gif) 
 5\times 5 の場合の例

### 問題文

格子状に 10\times 10 個のキャンディーが入る箱がある。 
箱は初期状態では空であり、100 個のキャンディーを順番に入れていく。 
キャンディーは全部で 3 種類の味があり、事前に t 番目に受け取るキャンディーの味の種類 f_t (1\leq f_t\leq 3) が分かっている。 
一方で、どの位置に入るかは事前には分からず、空いているマスの中から一様ランダムに選ばれる。 
キャンディーを受け取る順番は固定で、変更することは出来ない。

キャンディーを一つ受け取る度に、箱を前後左右にちょうど一回だけ傾ける。 
箱を傾けると、各キャンディーがその方向に、端か他のキャンディーにぶつかるまで一斉に移動する。 
例えば、左図の状態で箱を前方に傾けると、右図の状態となる。

![](https://img.atcoder.jp/ahc015/b639c75d_2.png) 
![](https://img.atcoder.jp/ahc015/b639c75d_3.png)

### 得点

以下のようにキャンディーの連結性を定義し、連結成分分解を考える。

2つのキャンディーは、同じ味であり、かつ、前後左右に同じ味のキャンディーのみを通って互いに到達可能な場合に、またその場合に限り、連結である。

例えば以下の図の状態は大きさ \{1, 1, 2, 2, 4, 6, 9\} の 7 個の連結成分からなる。

![](https://img.atcoder.jp/ahc015/b639c75d_4.png)

最終的に100個のキャンディーを受け取った状態における連結成分の大きさのリストを n_1,\cdots,n_k とし、i 番の味のキャンディーの総数を d_i とした時、以下のスコアが得られる。

\[\mathrm{round}\left(10^6\times\frac{\sum_{i=1}^k n_i^2}{\sum_{i=1}^3 d_i^2}\right)\]

出来るだけ高得点が得られるように傾け方を決定するプログラムを作成せよ。

テストケースは全部で 200 個あり、各テストケースの得点の合計が提出の得点となる。 
一つ以上のテストケースで不正な出力や制限時間超過をした場合、提出全体の判定がWAやTLEとなる。 
コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 同じ得点を複数の参加者が得た場合、提出時刻に関わらず同じ順位となる(従来の短期AHCから変更となっているので注意)。

### 入出力

まずはじめに、各キャンディーの味の情報が以下の形式で標準入力から与えられる。

```text
f_1 f_2 \cdots f_{100}

```

各 f_t は t 番目に受け取るキャンディーの味を表す、1 以上 3 以下の整数値である。

上記の情報を読み込んだら、以下の処理を 100 回繰り返す。

t (1\leq t\leq 100) 回目の処理ではまず、1 以上 101-t 以下の整数値 p_t が一つ標準入力から与えられる。 
下図の例のように、前から後ろ、左から右の優先順位で空きマスに 1 から 101-t の番号を順番に付けたとき、p_t 番目の空きマスに t 個目のキャンディーが入れられることを表している。

![](https://img.atcoder.jp/ahc015/b639c75d_5.png)

p_t の情報を読み込んだら、前後左右をそれぞれ `F`、`B`、`L`、`R` で表して、どの方向へ傾けるかを一文字で標準出力に出力せよ。 
**出力の後には改行をし、更に標準出力を flush しなければならない。** 
そうしない場合、TLEとなる可能性がある。 
**t 回目の方向を出力するまで、p_{t+1} の入力は与えられないので注意せよ。** 
100 回目はどの方向に傾けても同じであるため、出力をしてもしなくても良い。

#### 例

 | t 
 | Input 
 | Output

 | 事前情報 
 | 

```text
2 2 1 3 1 2 1 2 1 \cdots 3
```
 
 |

 | 1 
 | 

```text
3
```
 
 | 

```text
R
```

 | 2 
 | 

```text
98
```
 
 | 

```text
B
```

 | \vdots 
 |  
 |

 | 100 
 | 

```text
1
```
 
 | 

```text
L
```

[例を見る](https://img.atcoder.jp/ahc015/b639c75d.html?lang=ja&seed=0&output=R%0D%0AB%0D%0AB%0D%0AR%0D%0AF%0D%0AR%0D%0AF%0D%0AR%0D%0AR%0D%0AF%0D%0AB%0D%0AL%0D%0AB%0D%0AL%0D%0AF%0D%0AF%0D%0AB%0D%0AF%0D%0AB%0D%0AL%0D%0AL%0D%0AL%0D%0AL%0D%0AB%0D%0AF%0D%0AF%0D%0AR%0D%0AR%0D%0AF%0D%0AL%0D%0AL%0D%0AB%0D%0AL%0D%0AL%0D%0AL%0D%0AB%0D%0AL%0D%0AR%0D%0AF%0D%0AL%0D%0AB%0D%0AL%0D%0AF%0D%0AF%0D%0AF%0D%0AL%0D%0AL%0D%0AR%0D%0AB%0D%0AB%0D%0AF%0D%0AL%0D%0AF%0D%0AR%0D%0AB%0D%0AL%0D%0AF%0D%0AF%0D%0AR%0D%0AL%0D%0AR%0D%0AL%0D%0AR%0D%0AR%0D%0AB%0D%0AR%0D%0AB%0D%0AR%0D%0AR%0D%0AF%0D%0AB%0D%0AF%0D%0AR%0D%0AR%0D%0AF%0D%0AB%0D%0AF%0D%0AB%0D%0AF%0D%0AR%0D%0AB%0D%0AF%0D%0AF%0D%0AF%0D%0AB%0D%0AB%0D%0AL%0D%0AL%0D%0AR%0D%0AF%0D%0AB%0D%0AL%0D%0AB%0D%0AF%0D%0AB%0D%0AR%0D%0AF%0D%0AF%0D%0AL%0D%0AL%0D%0A)

### 入力生成方法

L 以上 U 以下の整数値を一様ランダムに生成する関数を \mathrm{rand}(L,U) で表す。 
各 f_t は \mathrm{rand}(1,3) により生成される。 
各 p_t は \mathrm{rand}(1,101-t) により生成される。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc015/b639c75d.html?lang=ja): ローカル版より高性能でアニメーション表示と手動プレイが可能です。 

- [ローカル版](https://img.atcoder.jp/ahc015/b639c75d.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc015/b639c75d_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### Story

AtCoder's CEO Takahashi prepares for Halloween tomorrow. 
At AtCoder's Halloween party, Takahashi will dress up in disguise and receive a piece of candy from 100 employees in turn by saying, "trick or treat!" 
He prepares a square box that can contain 10\times 10 pieces of candy in a grid pattern, and each employee puts a piece of candy in an empty space so that no candies overlap. 
There are 3 types of candies: strawberry, watermelon, and pumpkin flavors. 
He knows which flavor of candy each employee will put in by preliminary survey, but he doesn't know where each employee will put it. 
Since he is a clean freak, he will move the pieces of candy by tilting the box forward, backward, left, or right, just once for each piece of candy received, and eventually wants to make sure that the same type of candy is clustered together as much as possible. 
Please help him by writing a program to determine the direction to tilt. 
 
 ![](https://img.atcoder.jp/ahc015/b639c75d_1.gif) 
 Example for 5\times 5

### Problem Statement

There is a box that can contain 10\times 10 pieces of candy in a grid pattern. 
The box is initially empty, and 100 pieces of candy will be placed in order. 
There are 3 flavors of candy, and we know in advance the flavor f_t (1\leq f_t\leq 3) of the t-th candy. 
On the other hand, we do not know in advance to which cell each candy will be placed, and it will be chosen uniformly at random among the empty cells. 
You cannot change the order in which the pieces of candy are received.

Each time you receive one piece of candy, you must tilt the box forward, backward, left, or right exactly once. 
When you tilt the box, each piece of candy moves in that direction simultaneously until it reaches the edge or hits another candy. 
For example, if you tilt the box forward in the state shown in the left figure, the box will be in the state shown in the right figure.

![](https://img.atcoder.jp/ahc015/b639c75d_2.png) 
![](https://img.atcoder.jp/ahc015/b639c75d_3.png)

### Scoring

We define the connectivity of pieces of candy as follows and consider the connected components.

Two pieces of candy are connected if and only if they are of the same flavor and can reach each other through only pieces of candy of the same flavor in the four directions (front, back, left, right).

For example, the state in the figure below consists of 7 connected components of size \{1, 1, 2, 2, 4, 6, 9\}.

![](https://img.atcoder.jp/ahc015/b639c75d_4.png)

Let n_1,\cdots,n_k be the list of sizes of connected components in the final state after receiving 100 pieces of candy, and let d_i be the total number of pieces of candy of flavor i. 
Then the score for the test case is

\[\mathrm{round}\left(10^6\times\frac{\sum_{i=1}^k n_i^2}{\sum_{i=1}^3 d_i^2}\right)\]

Your task is to write a program to determine the tilting directions so that you can get as high a score as possible.

There are 200 test cases, and the score of a submission is the total score for each test case. 
If your submission produces an illegal output or exceeds the time limit for some test cases, the submission itself will be judged as WA or TLE , and the score of the submission will be zero. 
The highest score obtained during the contest will determine the final ranking, and there will be no system test after the contest. 
If more than one participant gets the same score, they will be ranked in the same place regardless of the submission time (note that this is changed from previous short-term AHCs).

### Input and Output

First, the flavor of each piece of candy is given from Standard Input in the following format.

```text
f_1 f_2 \cdots f_{100}

```

Each f_t is an integer value between 1 and 3, representing the flavor of the t-th piece of candy.

After reading the above information, repeat the following process 100 times.

In the t-th process (1\leq t\leq 100), a single integer p_t between 1 and 101-t is given from Standard Input. 
Let us number the empty cells from 1 to 101-t in front-to-back and left-to-right priority, as shown in the example figure below. 
Then the t-th piece of candy is placed in the p_t-th empty cell.

![](https://img.atcoder.jp/ahc015/b639c75d_5.png)

After reading p_t, by representing forward, backward, left, and right by `F`, `B`, `L`, and `R`, respectively, output a single character to Standard Output to indicate which direction to tilt the box.

**The output must be followed by a new line, and you have to flush Standard Output.** 
Otherwise, the submission might be judged as TLE. 
**Note that p_{t+1} will not be given until you output the t-th direction.** 
Since nothing happens at the 100th tilt, you may skip the output.

#### Example

 | t 
 | Input 
 | Output

 | Prior information 
 | 

```text
2 2 1 3 1 2 1 2 1 \cdots 3
```
 
 |

 | 1 
 | 

```text
3
```
 
 | 

```text
R
```

 | 2 
 | 

```text
98
```
 
 | 

```text
B
```

 | \vdots 
 |  
 |

 | 100 
 | 

```text
1
```
 
 | 

```text
L
```

[Show example](https://img.atcoder.jp/ahc015/b639c75d.html?lang=en&seed=0&output=R%0D%0AB%0D%0AB%0D%0AR%0D%0AF%0D%0AR%0D%0AF%0D%0AR%0D%0AR%0D%0AF%0D%0AB%0D%0AL%0D%0AB%0D%0AL%0D%0AF%0D%0AF%0D%0AB%0D%0AF%0D%0AB%0D%0AL%0D%0AL%0D%0AL%0D%0AL%0D%0AB%0D%0AF%0D%0AF%0D%0AR%0D%0AR%0D%0AF%0D%0AL%0D%0AL%0D%0AB%0D%0AL%0D%0AL%0D%0AL%0D%0AB%0D%0AL%0D%0AR%0D%0AF%0D%0AL%0D%0AB%0D%0AL%0D%0AF%0D%0AF%0D%0AF%0D%0AL%0D%0AL%0D%0AR%0D%0AB%0D%0AB%0D%0AF%0D%0AL%0D%0AF%0D%0AR%0D%0AB%0D%0AL%0D%0AF%0D%0AF%0D%0AR%0D%0AL%0D%0AR%0D%0AL%0D%0AR%0D%0AR%0D%0AB%0D%0AR%0D%0AB%0D%0AR%0D%0AR%0D%0AF%0D%0AB%0D%0AF%0D%0AR%0D%0AR%0D%0AF%0D%0AB%0D%0AF%0D%0AB%0D%0AF%0D%0AR%0D%0AB%0D%0AF%0D%0AF%0D%0AF%0D%0AB%0D%0AB%0D%0AL%0D%0AL%0D%0AR%0D%0AF%0D%0AB%0D%0AL%0D%0AB%0D%0AF%0D%0AB%0D%0AR%0D%0AF%0D%0AF%0D%0AL%0D%0AL%0D%0A)

### Input Generation

Let \mathrm{rand}(L,U) be a function that generates a uniform random integer between L and U, inclusive. 
Each f_t is generated by \mathrm{rand}(1,3). 
Each p_t is generated by \mathrm{rand}(1,101-t).

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc015/b639c75d.html?lang=en): This is more powerful than the local version providing animations and manual play. 

- [Local version](https://img.atcoder.jp/ahc015/b639c75d.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc015/b639c75d_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.
