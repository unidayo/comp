### ストーリー

AtCoder社は、ロゴ入りの限定グッズをいくつか販売している。 
このたび、それらの限定グッズをまとめて値引きした「スペシャルセット」の販売を開始することになった。 
高橋くんは、ベルトコンベアで順番に運ばれてくるグッズをダンボール箱に梱包し、配送業者に渡す作業を担当する予定である。 
そのため、販売開始に備えて梱包の練習を行うことにした。

配送料はダンボール箱の横幅と縦幅の合計に比例するため、できるだけ小さな箱に詰める必要がある。 
各グッズは長方形であり、手元に計測器具がないため、高橋くんは横幅と縦幅を目分量で測り、最適化が得意なあなたに電話で相談することにした。

あなたが電話越しにグッズの配置方法を指示すると、その指示に従って高橋くんは商品を配置する。 
そして、その配置において必要となるダンボール箱の横幅と縦幅を再び目分量で計測し、結果をあなたに伝える。 
あなたはその情報を元に新しい配置方法を指示する。

このやりとりを繰り返し、すべてのグッズをできるだけ小さなダンボール箱に収める配置方法を求めて欲しい。

### 問題文

N 個の長方形が与えられる。 
i 番目の長方形の大きさは、横幅 w_i と縦幅 h_i である。 
入力として、それぞれの値に計測誤差が乗った観測値 w'_i=\mathrm{normal}(w_i, \sigma) と h'_i=\mathrm{normal}(h_i, \sigma) が与えられる。 
ここで、\mathrm{normal}(\mu, \sigma) は次の手順で値を生成する関数である。

- 平均 \mu、標準偏差 \sigma の正規分布からランダムに値を生成する。 

- 生成した値を四捨五入し、整数に変換する。 

- 値が 1 未満の場合は 1 とし、10^9 を超える場合は 10^9 とする。

これらの長方形を、平面上で互いに重ならないように番号順に配置する。 
長方形は必要に応じて 90^\circ 回転させ、横幅と縦幅を入れ替えることができる。

右方向に x 軸を、下方向に y 軸を取る。 
長方形は x \geq 0 かつ y \geq 0 の領域に配置することができる。 
配置方法は以下のような列 (p_0, r_0, d_0, b_0), (p_1, r_1, d_1, b_1), \dots として指定する。

- p_i は i 番目に配置する長方形の番号 (0 \leq p_i \leq N-1) を表す。**一部の長方形だけを配置することもできるが、番号は昇順に並んでいなければならず、すべての i < j に対して p_i < p_j を満たす必要がある。** 

- r_i は長方形を 90^\circ 回転させて配置するかどうかを表す。r_i = 0 の場合、長方形は回転させず、r_i = 1 の場合、回転させる。 

- d_i は長方形を配置する方向を表す。d_i が `U` の場合、長方形を下から上へ動かし、既に配置した他の長方形の下端または y = 0 の線に接触したところで停止して配置する。d_i が `L` の場合、長方形を右から左へ動かし、既に配置した他の長方形の右端または x = 0 の線に接触したところで停止して配置する。 

- b_i は長方形を配置する際の基準となる長方形の番号を表す。b_i は -1 または既に配置した長方形の番号である必要がある。 

- 下から上に配置する場合 (`U`)、b_i は新しい長方形の左端をどの長方形の右端と揃えるかを表す。b_i = -1 の場合、左端が x = 0 になるように配置する。 

- 右から左に配置する場合 (`L`)、b_i は新しい長方形の上端をどの長方形の下端と揃えるかを表す。b_i = -1 の場合、上端が y = 0 になるように配置する。

#### 操作の例

以下の例では、3番の長方形を回転させず、各(d,b)の組に対して配置した結果を図示している。

  | 操作(d,b) 
  | 操作前 
  | (U,-1) 
  | (U,0) 
  | (U,1) 
  | (U,2)

  | 結果 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_0.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_1.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_2.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_3.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_4.png)

  | 操作(d,b) 
  | 操作前 
  | (L,-1) 
  | (L,0) 
  | (L,1) 
  | (L,2)

  | 結果 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_0.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_5.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_6.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_7.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_8.png)

以下の操作を T 回繰り返す。

- 長方形の配置方法を指定する。前回の配置の続きからではなく、何も置かれていない状態から再度配置をすることに注意。 

- 配置後の横幅（長方形が存在する x 座標の最大値）を W、縦幅（長方形が存在する y 座標の最大値）を H とする。このとき、W' = \mathrm{normal}(W, \sigma) と H' = \mathrm{normal}(H, \sigma) の値が計測結果として得られる。

### 得点

t 回目の配置における横幅を W_t、縦幅を H_t、使用しなかった長方形の集合を U_t とする。このとき、t 回目のスコア s_t を次のように定義する。

\[ 
s_t = W_t + H_t + \sum_{i\in U_t}(w_i+h_i) 
\]

このとき、\min_t s_t の値が絶対スコアとして得られる。 
**絶対スコアは小さければ小さいほど良い。**

各テストケース毎に、\mathrm{round}(10^9\times \frac{全参加者中の最小絶対スコア}{自身の絶対スコア}) の**相対評価スコア**が得られ、その和が提出の得点となる。

最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定される。 
暫定テスト、システムテストともに、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースの相対評価スコアは0点となり、そのテストケースにおいては「全参加者中の最小絶対スコア」の計算から除外される。 
システムテストは** CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意せよ。

#### テストケース数

- 暫定テスト: 50個 

- システムテスト: 3000個、コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc040/seeds.txt) (sha256=1e93374aa02130a1167c2893f1904b4234a3b517d1e7b9d25022a9125ff3777d) を公開

#### 相対評価システムについて

暫定テスト、システムテストともに、CE 以外の結果を得た一番最後の提出のみが順位表に反映される。 
相対評価スコアの計算に用いられる各テストケース毎の全参加者中の最小絶対スコアの算出においても、順位表に反映されている最終提出のみが用いられる。

順位表に表示されているスコアは相対評価スコアであり、新規提出があるたびに、相対評価スコアが再計算される。 
一方、提出一覧から確認出来る各提出のスコアは各テストケース毎の絶対スコアをそのまま足し合わせたものであり、相対評価スコアは表示されない。 
最新以外の提出の、現在の順位表における相対評価スコアを知るためには、再提出が必要である。 
不正な出力や制限時間超過をした場合、提出一覧から確認出来るスコアは0となるが、順位表には正解したテストケースに対する相対スコアの和が表示されている。

#### 実行時間について

実行時間には多少のブレが生じます。 
また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
そのため、実行時間制限ギリギリの提出がシステムテストでTLEとなる可能性があります。 
プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入出力

まず初めに、長方形の個数 N、操作ターン数 T、計測に発生する誤差の標準偏差 \sigma、各長方形の大きさの計測値 w'_i, h'_i が、以下の形式で標準入力から与えられる。

```text
N T \sigma
w'_0 h'_0
\vdots
w'_{N-1} h'_{N-1}

```

- 長方形の個数 N は 30\leq N\leq 100 を満たす。 

- 操作回数 T は N/2\leq T\leq 4N を満たす。 

- 計測時に発生する誤差の標準偏差 \sigma は 1000\leq\sigma\leq 10000 を満たす整数値である。 

- 横幅と縦幅の計測値 w'_i, h'_i は 1 以上 10^9 以下の整数値である。

上記の情報を読み込んだ後、以下の処理を T 回繰り返す。

長方形の配置方法を表す列を (p_0, r_0, d_0, b_0), (p_1, r_1, d_1, b_1), \dots, (p_{n-1}, r_{n-1}, d_{n-1}, b_{n-1}) とする。 
ここで、n は配置する長方形の個数を表し、n < N であっても良い。 
この列を以下の形式で標準出力に出力せよ。

```text
n
p_0 r_0 d_0 b_0
\vdots 
p_{n-1} r_{n-1} d_{n-1} b_{n-1}

```

出力後に、配置の横幅と縦幅の計測値を表す整数 W' と H' が、以下の形式で標準入力から与えられる。

```text
W' H'

```

**出力の後には改行をし、更に標準出力を flush しなければならない。** 
そうしない場合、TLEとなる可能性がある。

解答プログラムは、# から始まるコメント行を出力に含めても良い。 Web版ビジュアライザを使用すると、コメント行を対応するタイミングで表示出来るため、デバッグや考察等に役立てることが出来る。 ジャッジプログラムはコメント行を全て無視するため、コメント行を出力するプログラムをそのまま提出可能である。

#### 例

 | t 
 | Output 
 | Input

 | 事前情報 
 |  
 | 

```text
4 3 1000
77685 46130
32459 75368
54192 88183
60740 42948
```

 | 1 
 | 

```text
2
0 0 U -1
1 1 U 0
```
 
 | 

```text
153220 47195
```

 | 2 
 | 

```text
4
0 0 U -1
1 1 L -1
2 1 L 1
3 0 U -1
```
 
 | 

```text
167543 86611
```

 | 3 
 | 

```text
4
0 0 U -1
1 0 L -1
2 0 U -1
3 0 U 2
```
 
 | 

```text
113031 134437
```

[例を見る](https://img.atcoder.jp/ahc040/RGoXy7re.html?lang=ja&seed=0&output=sample)

### サンプルプログラム 
 
Pythonでの解答例を示す。 このプログラムでは、各長方形を順番にランダムな回転・位置で配置することを T 回試している。

```text

import random

def query(prdb):
    print(len(prdb))
    for p, r, d, b in prdb:
        print(p, r, d, b)
    W, H = map(int, input().split())
    return W, H

N, T, sigma = map(int, input().split())
wh = [tuple(map(int, input().split())) for _ in range(N)]

rng = random.Random(1234)

for _ in range(T):
    prdb = []
    for i in range(N):
        prdb.append((
            i,
            rng.randint(0, 1),
            ['U', 'L'][rng.randint(0, 1)],
            rng.randint(-1, i - 1),
        ))
    query(prdb)

```

### 入力生成方法 

- \mathrm{rand}(L,U): L 以上 U 以下の整数値を一様ランダムに生成する。 

- \mathrm{rand\_double}(L,U): L 以上 U 以下の実数値を一様ランダムに生成する。

#### N, T, \sigma の生成

- N=\mathrm{rand}(30,100) 

- T=\mathrm{round}(N\times 2^{\mathrm{rand\_double}(-1,2)}) 

- \sigma=\mathrm{rand}(1000,10000)

#### w, h の生成

U=10^5 とし、L=\mathrm{rand}(U/10,U/2) を生成する。 
各 i に対し、w_i=\mathrm{rand}(L,U), h_i=\mathrm{rand}(L,U) により生成される。

### ツール(入力ジェネレータ・ローカルテスタ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc040/RGoXy7re.html?lang=ja): ローカル版より高性能でアニメーション表示が可能です。 

- [ローカル版](https://img.atcoder.jp/ahc040/RGoXy7re.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc040/RGoXy7re_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

#### ツールで用いられる入力ファイルの仕様

ローカルテスタに与える入力ファイルは解答プログラムに与えられる事前情報の末尾に以下の形式の情報が追加されている。

```text
w_0 h_0
\vdots
w_{N-1} h_{N-1}
dW_0 dH_0
\vdots
dW_{T-1} dH_{T-1}

```

- w_i, h_i は i 番目の長方形の真の横幅と縦幅である。 

- dW_t, dH_t は t ターン目の計測結果に加算される計測誤差である。

### Story

AtCoder sells several limited-edition goods featuring its logo. 
They have now decided to launch a "Special Set," which bundles these limited-edition goods at a discounted price. 
Takahashi is assigned to pack the goods, which are delivered one by one on a conveyor belt, into a single cardboard box and hand it over to a delivery service. 
To prepare for the launch, he has decided to practice packing.

Since the shipping cost is proportional to the sum of the width and height of the cardboard box, it is necessary to make an effort to pack the goods into as small a box as possible. 
Each good is rectangular, but Takahashi does not have measuring tools, so he estimates the width and height by eye and consults you, an expert in optimization, over the phone.

When you instruct Takahashi on how to arrange the goods over the phone, he follows your instructions and arranges the items accordingly. 
He then estimates by eye the width and height of the cardboard box required for that arrangement and reports the results back to you. 
Based on that information, you provide new instructions for the arrangement.

Repeat this process to find an arrangement that packs all the goods into as small a cardboard box as possible.

### Problem Statement

You are given N rectangles. 
The i-th rectangle has width w_i and height h_i. 
For each rectangle, observed values w'_i=\mathrm{normal}(w_i, \sigma) and h'_i=\mathrm{normal}(h_i, \sigma) are provided as input, which include measurement errors. 
Here, \mathrm{normal}(\mu, \sigma) is a function that generates a value through the following steps:

- Generate a random value from a normal distribution with mean \mu and standard deviation \sigma. 

- Round the generated value to the nearest integer. 

- If the resulting value is less than 1, it is set to 1, and if it exceeds 10^9, it is set to 10^9.

Place these rectangles on a plane in the order of their indices such that they do not overlap. 
Each rectangle can be rotated 90^\circ, swapping its width and height if necessary.

The positive x-axis extends to the right, and the positive y-axis extends downward. 
Rectangles can be placed in the region where x \geq 0 and y \geq 0. 
You are required to specify the placement method as a sequence (p_0, r_0, d_0, b_0), (p_1, r_1, d_1, b_1), \dots, such that:

- p_i represents the index of the rectangle to be placed at the i-th step (0 \leq p_i \leq N-1). **You can try placing only some of the rectangles, but the indices must be in ascending order; in other words, for all i < j, p_i < p_j must hold.** 

- r_i indicates whether the rectangle should be rotated by 90^\circ. If r_i = 0, the rectangle is not rotated; if r_i = 1, the rectangle is rotated. 

- d_i specifies the direction in which the rectangle is placed. 

- If d_i is `U`, the rectangle is moved upward until it stops at the bottom edge of another rectangle that has already been placed or at the line y = 0. 

- If d_i is `L`, the rectangle is moved leftward until it stops at the right edge of another rectangle that has already been placed or at the line x = 0.

- b_i represents the reference rectangle for placement. b_i must be either -1 or the index of a previously placed rectangle. 

- When placing the rectangle upward (`U`), b_i specifies which rectangle’s right edge should align with the left edge of the new rectangle. If b_i = -1, the left edge of the rectangle aligns with x = 0. 

- When placing the rectangle leftward (`L`), b_i specifies which rectangle’s bottom edge should align with the top edge of the new rectangle. If b_i = -1, the top edge of the rectangle aligns with y = 0.

#### Example of Operations

In the following example, the third rectangle is placed without rotation, and the results of placement are illustrated for each pair of (d, b).

  | Operation (d,b) 
  | Before 
  | (U,-1) 
  | (U,0) 
  | (U,1) 
  | (U,2)

  | Result 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_0.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_1.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_2.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_3.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_4.png)

  | Operation (d,b) 
  | Before 
  | (L,-1) 
  | (L,0) 
  | (L,1) 
  | (L,2)

  | Result 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_0.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_5.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_6.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_7.png) 
  | ![](https://img.atcoder.jp/ahc040/RGoXy7re_8.png)

Repeat the following operations T times:

- Specify how to place the rectangles. Note that placement starts from an empty state, not continuing from the previous placement. 

- After placement, let the width (the maximum x-coordinate where rectangles exist) be W, and the height (the maximum y-coordinate where rectangles exist) be H. Then, the measured values W' = \mathrm{normal}(W, \sigma) and H' = \mathrm{normal}(H, \sigma) are obtained as the results of the measurement.

### Scoring

Let W_t and H_t be the width and height of the placement at the t-th turn, respectively, and let U_t be the set of rectangles that are not used in this turn. Then, the score s_t for the t-th turn is defined as follows:

\[ 
s_t = W_t + H_t + \sum_{i\in U_t}(w_i+h_i) 
\]

Then you obtain an absolute score of \min_t s_t. 
**The lower the absolute score, the better.**

For each test case, we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{MIN}}{\mathrm{YOUR}}), where YOUR is your absolute score and MIN is the lowest absolute score among all competitors obtained on that test case. The score of the submission is the sum of the relative scores.

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MIN calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
Be careful not to make a mistake in the final submission.

#### Number of test cases

- Provisional test: 50 

- System test: 3000. We will publish [seeds.txt](https://img.atcoder.jp/ahc040/seeds.txt) (sha256=1e93374aa02130a1167c2893f1904b4234a3b517d1e7b9d25022a9125ff3777d) after the contest is over.

#### About relative evaluation system

In both the provisional/system test, the standings will be calculated using only the last submission which received a result other than CE. 
Only the last submissions are used to calculate the MIN for each test case when calculating the relative scores.

The scores shown in the standings are relative, and whenever a new submission arrives, all relative scores are recalculated. 
On the other hand, the score for each submission shown on the submissions page is the sum of the absolute score for each test case, and the relative scores are not shown. 
In order to know the relative score of submission other than the latest one in the current standings, you need to resubmit it. 
If your submission produces illegal output or exceeds the time limit for some test cases, the score shown on the submissions page will be 0, but the standings show the sum of the relative scores for the test cases that were answered correctly.

#### About execution time

Execution time may vary slightly from run to run. 
In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percent compared to provisional tests. 
For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
Please measure the execution time in your program to terminate the process, or have enough margin in the execution time.

### Input and Output

First, the number of rectangles N, the number of operation turns T, the standard deviation \sigma of the measurement error, and the measured sizes w'_i and h'_i of each rectangle are given from Standard Input in the following format.

```text
N T \sigma
w'_0 h'_0
\vdots
w'_{N-1} h'_{N-1}

```

- The number of rectangles N satisfies 30 \leq N \leq 100.  

- The number of operations T satisfies N/2 \leq T \leq 4N.  

- The standard deviation of the measurement error \sigma is an integer satisfying 1000 \leq \sigma \leq 10000.  

- The measured values of the width and height, w'_i and h'_i, are integers between 1 and 10^9 inclusive.

After reading the above information, repeat the following process T times.

Let the sequence representing the rectangle placement method be (p_0, r_0, d_0, b_0), (p_1, r_1, d_1, b_1), \dots, (p_{n-1}, r_{n-1}, d_{n-1}, b_{n-1}). 
Here, n represents the number of rectangles to place, and n may be less than N. 
Output this sequence to Standard Output in the following format.

```text
n
p_0 r_0 d_0 b_0
\vdots 
p_{n-1} r_{n-1} d_{n-1} b_{n-1}

```

After the output, the measured values of the width and height, W' and H', are given from Standard Input in the following format.

```text
W' H'

```

**The output must be followed by a new line, and you have to flush Standard Output.** 
Otherwise, the submission might be judged as TLE.

Your program may output comment lines starting with `#`. The web version of the visualizer displays the comment lines at the corresponding timing, which may be useful for debugging and analysis. Since the judge program ignores all comment lines, you can submit a program that outputs comment lines as is.

#### Example

 | t 
 | Output 
 | Input

 | Prior Information 
 |  
 | 

```text
4 3 1000
77685 46130
32459 75368
54192 88183
60740 42948
```

 | 1 
 | 

```text
2
0 0 U -1
1 1 U 0
```
 
 | 

```text
153220 47195
```

 | 2 
 | 

```text
4
0 0 U -1
1 1 L -1
2 1 L 1
3 0 U -1
```
 
 | 

```text
167543 86611
```

 | 3 
 | 

```text
4
0 0 U -1
1 0 L -1
2 0 U -1
3 0 U 2
```
 
 | 

```text
113031 134437
```

[Show example](https://img.atcoder.jp/ahc040/RGoXy7re.html?lang=en&seed=0&output=sample)

### Sample Solution 
 
This is a sample solution in Python. This program tries to place each rectangle in a random rotation and position T times.

```text

import random

def query(prdb):
    print(len(prdb))
    for p, r, d, b in prdb:
        print(p, r, d, b)
    W, H = map(int, input().split())
    return W, H

N, T, sigma = map(int, input().split())
wh = [tuple(map(int, input().split())) for _ in range(N)]

rng = random.Random(1234)

for _ in range(T):
    prdb = []
    for i in range(N):
        prdb.append((
            i,
            rng.randint(0, 1),
            ['U', 'L'][rng.randint(0, 1)],
            rng.randint(-1, i - 1),
        ))
    query(prdb)

```

### Input Generation 

- \mathrm{rand}(L,U): Generates an integer uniformly at random between L and U, inclusive.  

- \mathrm{rand\_double}(L,U): Generates a real number uniformly at random between L and U, inclusive.

#### Generation of N, T, and \sigma

- N = \mathrm{rand}(30, 100)  

- T = \mathrm{round}(N \times 2^{\mathrm{rand\_double}(-1, 2)})  

- \sigma = \mathrm{rand}(1000, 10000)

#### Generation of w and h

Let U = 10^5, and generate L = \mathrm{rand}(U/10, U/2).
 
For each i, w_i = \mathrm{rand}(L, U) and h_i = \mathrm{rand}(L, U) are generated.

### Tools (Input generator, local tester, and visualizer) 

- [Web version](https://img.atcoder.jp/ahc040/RGoXy7re.html?lang=en): This is more powerful than the local version providing animations. 

- [Local version](https://img.atcoder.jp/ahc040/RGoXy7re.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc040/RGoXy7re_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.

#### Specification of input files used by the tools

The input file provided to the local tester includes additional information in the following format appended to the end of the prior-information given to the solution program.

```text
w_0 h_0
\vdots
w_{N-1} h_{N-1}
dW_0 dH_0
\vdots
dW_{T-1} dH_{T-1}

```

- w_i and h_i represent the true width and height of the i-th rectangle. 

- dW_t and dH_t are the measurement errors added to the results of the t-th turn.

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

- [ルール](https://atcoder.jp/contests/ahc040/rules)
 
- [用語集](https://atcoder.jp/contests/ahc040/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
