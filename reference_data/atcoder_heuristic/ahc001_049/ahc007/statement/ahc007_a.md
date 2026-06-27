### ストーリー

巨大企業のAtCoder社は多数のオフィスを所有している。 
超機密情報である出題予定の問題文を安全に共有するため、いくつかのオフィス間に量子暗号を用いた専用回線を敷くことにした。 
専用回線で結ぶオフィスペアの候補はいくつかあり、全てのオフィスが専用回線によって連結となるようにしたい。 
専用回線を敷く費用は回線の長さに比例するが、物理的な制約により直線で引けるとは限らないため、各候補について正確な費用の見積もりを業者に依頼している。 
高橋社長はせっかちなため、一つの候補についての見積もりを受け取ったら、即座にその回線を敷くかどうかを決定する。 
高橋社長のサポートをして、出来るだけ安い費用で目的を達成してほしい。

### 問題文

頂点数が N、辺の数が M の無向グラフが与えられる。 
各頂点は二次元平面上にあり、i 番目の頂点の座標は (x_i, y_i) である。 
i 番目の辺は頂点 u_i と v_i を結び、その長さ l_i は、端点の間のユークリッド距離を整数に四捨五入したものを d_i=\mathrm{round}(\sqrt{(x_{u_i}-x_{v_i})^2+(y_{u_i}-y_{v_i})^2}) とすると、 
d_i \leq l_i \leq 3 d_i を満たすことが事前に分かっている。

辺の長さ l_i の正確な情報が i=0 番目の辺から i=M-1 番目の辺まで順番に一つずつ与えられる。 
辺の長さの情報を一つ受け取ったら、次の辺の情報を受け取る前に、その辺を採用するかしないかを決定せよ。

最終的に採用した辺の集合を S としたとき、全ての頂点対について、S に含まれる辺のみを通って行き来出来なければならない。 
採用した辺の総長が出来るだけ短くなるように意思決定して欲しい。

### 入出力

全てのテストケースにおいて、N=400、M=1995 で固定である。 
まずはじめに、N 個の頂点の座標 (x_0,y_0), \cdots, (x_{N-1},y_{N-1}) と、M 個の辺の端点 (u_0,v_0), \cdots, (u_{M-1},v_{M-1}) が以下の形式で標準入力から与えられる。

```text
x_0 y_0
\vdots
x_{N-1} y_{N-1}
u_0 v_0
\vdots
u_{M-1} v_{M-1}

```

以下を満たすことが保証されている。

- 0\leq x_i,y_i\leq 800 

- 0\leq u_i<v_i\leq N-1 

- 同じ (u_i,v_i) のペアは複数回現れることはない。 

- グラフは連結である。

次に、以下の処理を M 回繰り返す。

i 回目 (0\leq i\leq M-1) の処理では、まず i 番目の辺の長さ l_i が d_i 以上 3d_i 以下の整数値から一様ランダムに生成され、標準入力に一行で与えられる。 
l_i の値を読み込んだら、i 番目の辺を採用する場合には `1` を、採用しない場合は `0` を一行で標準出力に出力せよ。 
**出力を行うまで、次の入力は与えられないので注意せよ。出力のあとは標準出力を flush しなければならない。**そうしない場合、TLEとなる可能性がある。

### 例

 | i 
 | Input 
 | Output

 | 事前情報 
 | 

```text

406 19
347 786
\vdots
21 191

```
 
 |

 | 0 
 | 

```text
69
```
 
 | 

```text
1
```

 | 1 
 | 

```text
89
```
 
 | 

```text
0
```

 | \vdots

 | M-1 
 | 

```text
175
```
 
 | 

```text
0
```

### 得点

採用した辺集合の総長を A、 
各辺の真の長さ l_i を事前に全て知っている状態での最適な辺の選び方 ([最小全域木](https://ja.wikipedia.org/wiki/%E5%85%A8%E5%9F%9F%E6%9C%A8#%E6%9C%80%E5%B0%8F%E5%85%A8%E5%9F%9F%E6%9C%A8)) の総長を B とすると、\mathrm{round}(10^8\times B/A) の得点が得られる。 
採用した辺集合がグラフを連結にしない場合は、WA と判定される。 
提出プログラムが異常終了した場合、 RE ではなく WA と判定される場合があるので注意せよ。

テストケースは全部で 150 個あり、各テストケースの得点の合計が提出の得点となる。 
1つ以上のテストケースで AC 以外の判定がされた場合、提出の得点は0点となる。 
コンテスト時間中に得た最高得点で最終順位が決定され、コンテスト終了後のシステムテストは行われない。 
同じ得点を複数の参加者が得た場合、その得点を獲得した提出の早い方が上位となる。

### 入力生成方法

L 以上 U 以下の整数値を一様ランダムに生成する関数を \mathrm{rand}(L,U) で表す。

#### (x_i,y_i) の生成

各 i=0,\cdots,N-1 に対して順番に、x_i=\mathrm{rand}(0,800)、y_i=\mathrm{rand}(0,800) を生成する。 
既に生成した頂点の座標(x_j,y_j) (j<i)とのユークリッド距離が 5 以下の間、生成をやり直す。

#### (u_i,v_i) の生成

頂点 i と頂点 j の間に長さ \mathrm{round}(\sqrt{(x_i-x_j)^2+(y_i-y_j)^2}) の辺を張った完全グラフを G とする。 
以下の処理を 5 回繰り返すことで、M=5(N-1) 本の辺集合 E を生成する。

G の最小全域木 T を一つ求め、T に含まれる辺を全て G から削除し、E に追加する。

最後に、E に含まれる辺をランダムな順番に並び替えることで、(u_0,v_0),\cdots,(u_{M-1},v_{M-1}) を生成する。

#### l_i の生成

端点の間のユークリッド距離を整数に四捨五入したものを d_i=\mathrm{round}(\sqrt{(x_{u_i}-x_{v_i})^2+(y_{u_i}-y_{v_i})^2}) とすると、 
l_i=\mathrm{rand}(d_i,3 d_i) により生成する。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [ローカル版](https://img.atcoder.jp/ahc007/a855b6a456c9892b747e147001b0f89.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Web版](https://img.atcoder.jp/ahc007/a855b6a456c9892b747e147001b0f89.html?lang=ja): ローカル版より高性能でアニメーション表示が可能です。

コンテスト期間中に、seed=0 に対するビジュアライザの出力画像(pngもしくはgif)のみ twitter で共有が可能です。必ず指定されたハッシュタグを用い、公開アカウントを使用して下さい。[共有された画像の一覧](https://twitter.com/search?q=%23AHC007%20%23visualizer&src=typed_query&f=live)

### Story

AtCoder, a big tech company, has many offices. 
In order to securely share super-secret information of problem statements for future contests, we decided to set up private lines using quantum cryptography between the offices. 
There are several candidates for office pairs that can be connected by private lines, and we want to make sure that all offices are connected by private lines. 
The cost of setting up a private line is proportional to the length of the line, but due to physical limitations, it is not always possible to set up a straight line, so we have asked vendors to estimate the exact cost for each candidate. 
Since CEO Takahashi is impatient, once he receives the estimate for one candidate, he immediately decides whether to set up that line or not. 
Please support Takahashi and help him achieve his goal at a lower cost as possible.

### Problem Statement

You are given an undirected graph with N vertices and M edges. 
Each vertex is on a two-dimensional plane, and the coordinates of the i-th vertex is (x_i, y_i). 
The i-th edge connects vertices u_i and v_i, and we know in advance that its length l_i satisfies d_i \leq l_i \leq 3 d_i, where d_i=\mathrm{round}(\sqrt{(x_{u_i}-x_{v_i})^2+(y_{u_i}-y_{v_i})^2}) is the Euclidean distance between the endpoints rounded to the nearest integer.

The true edge length l_i will be given one by one in order from i=0 to i=M-1. 
After receiving the length l_i of the i-th edge, you have to decide whether to adopt that edge or not before receiving the length l_{i+1} of the next edge.

Let S be the set edges you eventually adopt, then for every vertex pair (u,v), S must contain a path between u and v. 
Please make decisions so that the total length of the adopted edges is as short as possible.

### Input and Output

For all test cases, we fix N=400 and M=1995.

At the start of the execution, the coordinates of N vertices (x_0,y_0), \cdots, (x_{N-1},y_{N-1}) and the endpoints of M edges (u_0,v_0), \cdots, (u_{M-1},v_{M-1}) are given from Standard Input in the following format.

```text
x_0 y_0
\vdots
x_{N-1} y_{N-1}
u_0 v_0
\vdots
u_{M-1} v_{M-1}

```

It is guaranteed to satisfy the following.

- 0\leq x_i,y_i\leq 800 

- 0\leq u_i<v_i\leq N-1 

- The same (u_i,v_i) pair never appears more than once. 

- The graph is connected.

Then, repeat the following process M times.

In the i-th (0\leq i\leq M-1) process, the length l_i of the i-th edge is generated uniformly at random from integers between d_i and 3 d_i, and given to Standard Input in a single line. 
After reading l_i, output `1` if you adopt the i-th edge, or `0` if you don't, in one line to Standard Output. 
**Note that the next input is not given until your program outputs 0 or 1. After the output, you have to flush Standard Output.** Otherwise, the submission might be judged as TLE .

### Example

 | i 
 | Input 
 | Output

 | Prior information 
 | 

```text

406 19
347 786
\vdots
21 191

```
 
 |

 | 0 
 | 

```text
69
```
 
 | 

```text
1
```

 | 1 
 | 

```text
89
```
 
 | 

```text
0
```

 | \vdots

 | M-1 
 | 

```text
175
```
 
 | 

```text
0
```

### Scoring

Let A be the total length of the set of adopted edges, 
B be the total length of the optimal set of edges under the condition that the true length l_i of every edge is known in advance ([minimum spanning tree](https://en.wikipedia.org/wiki/Minimum_spanning_tree)). 
Then, you will get a score of \mathrm{round}(10^8\times B/A). 
When the set of adopted edges does not make the graph connected, your submission will be judged as WA. 
Note that if your program terminates abnormally, it may be judged as WA instead of RE.

There are 150 test cases, and the score of a submission is the total score for each test case. If you get a result other than AC for one or more test cases, the score of the submission will be zero. The highest score obtained during the contest time will determine the final ranking, and there will be no system test after the contest. If more than one participant gets the same score, the ranking will be determined by the submission time of the submission that received that score.

### Input Generation

Let \mathrm{rand}(L,U) be a function that generates a uniform random integer between L and U, inclusive.

#### Generation of (x_i,y_i)

For each i=0,\cdots,N-1, we generate x_i=\mathrm{rand}(0,800) and y_i=\mathrm{rand}(0,800). 
If the Euclidean distance to the coordinates (x_j,y_j) of an already generated vertex (j<i) is less than or equal to 5, we regenerate (x_i,y_i).

#### Generation of (u_i,v_i)

Let G be a complete graph with edge length \mathrm{round}(\sqrt{(x_i-x_j)^2+(y_i-y_j)^2}) between vertex i and j. 
By repeating the following process 5 times, we generate a set E of M=5(N-1) edges.

We compute a minimum spanning tree T of G, and remove all edges in T from G and insert them to E.

Finally, by randomly shuffling the order of edges in E, we generate the list of edges (u_0,v_0),\cdots,(u_{M-1},v_{M-1}).

#### Generation of l_i

Let d_i=\mathrm{round}(\sqrt{(x_{u_i}-x_{v_i})^2+(y_{u_i}-y_{v_i})^2}) be the Euclidean distance between the endpoints rounded to the nearest integer. 
Then we generate l_i=\mathrm{rand}(d_i,3 d_i).

### Tools (Input generator and visualizer) 

- [Local version](https://img.atcoder.jp/ahc007/a855b6a456c9892b747e147001b0f89.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Web version](https://img.atcoder.jp/ahc007/a855b6a456c9892b747e147001b0f89.html?lang=en): This is more powerful than the local version and can display animations.

You are allowed to share output images (png or gif) of the provided visualizer for seed=0 on twitter during the contest. You have to use the specified hashtag and public account. [List of shared images.](https://twitter.com/search?q=%23AHC007%20%23visualizer&src=typed_query&f=live)
