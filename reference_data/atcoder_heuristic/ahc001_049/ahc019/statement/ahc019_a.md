### 問題文

AtCoder社はブロックを組み合わせて指定されたシルエットの立体を作る知育玩具を開発している。 
この玩具は 1\times 1\times 1 の立方体を面同士で接合したポリキューブ状のブロックが複数と2次元の白黒シルエット画像2枚の組がセットになっている。 
ブロックを組み合わせて一つの立体を作り、作った立体を正面から見た時のシルエットと、右側から見た時のシルエットが、シルエット画像に完全一致していたらゲームクリアである。

長く遊べるように、1セットのブロックに対して、シルエットの組を複数用意したい。 
例えば左図の例にある6ブロックのセットからは、右図にあるようなシルエットの組2つが達成可能である。

 ![](https://img.atcoder.jp/ahc019/b36525d8_1.png)

 ![](https://img.atcoder.jp/ahc019/b36525d8_2.png)

玩具デザイナーであるあなたの仕事は、正面・右側のシルエットのペアが2組与えられるので、両方を作ることが出来るようなブロックのセットと、その作り方を求めることである。 
小さなブロックは子供が誤って飲み込んでしまう可能性があるため、少数の大きなブロックのみからなるセットが望ましい。

#### パズルのルールについての補足

- ブロックは全て使い切らなくても良い(が、全て使い切るほうが良いスコアが得られる)。 

- 各ブロックは x軸、y軸、z軸に対して90度単位で回転させることが出来るが反転させることは出来ない。 

- ブロックの配置は各頂点が整数座標となるようにしなくてはならず、ブロック同士は正の体積の共通部分を持ってはならない。 

- 作成する立体は連結でなくても良い。簡単のため、宙に浮かせるような配置も可能であるとする(追加で透明なブロックが大量にあり、自由に支えることが出来ると解釈せよ)。

#### シルエットについて

左から右方向に x 軸、手前から奥方向に y 軸、上から下方向に z 軸を取る。 
全てのブロックは (0,0,0) と (D,D,D) を対角とする立方体領域内に収まっているとする。 
三次元の01配列 b(x,y,z) を、(x,y,z) と (x+1,y+1,z+1) を対角とする単位立方体の領域をブロックが占めている場合に b(x,y,z)=1、そうでない場合に b(x,y,z)=0 と定義する。 
このとき、前から見たシルエットは二次元の01配列 f(z,x) であり、以下のように定義される。

\[ 
 f(z,x)=\begin{cases} 
 1&(\sum_{y=0}^{D-1}b(x,y,z)\geq 1)\\ 
 0&(\sum_{y=0}^{D-1}b(x,y,z)=0) 
 \end{cases} 
\]

同様に、右から見たシルエットは二次元の01配列 r(z,y) であり、以下のように定義される。

\[ 
 r(z,y)=\begin{cases} 
 1&(\sum_{x=0}^{D-1}b(x,y,z)\geq 1)\\ 
 0&(\sum_{x=0}^{D-1}b(x,y,z)=0) 
 \end{cases} 
\]

### 得点

作成した玩具セットに含まれる各ブロックの体積を v_1,\cdots,v_n とする。 
ここで、ブロックの体積とはそれを構成する 1\times 1\times 1 の立方体の個数に等しい。 
各ブロックは連結、すなわち構成するどの立方体同士も面を共有する立方体への移動によって到達可能でなければならない。 
非連結なブロックが一つでもある場合は WA と判定される。 
各ブロックは連結でなければならないが、ブロックを組み合わせて作る立体は非連結でも構わないことに注意せよ。

i=1,2 番目のシルエットの組に対して、出力した立体の作り方において使用しなかったブロックの体積の合計を r_i 、両方で使用したブロックの集合を S とする。 
このとき、以下の評価値が得られる。評価値は低ければ低いほど良い。

\[ 
 \mathrm{round}\left(10^9\times \left(r_1+r_2+\sum_{i\in S} \frac{1}{v_i}\right)\right) 
\]

例えば、ブロックのセットが体積5のブロックA、体積3のブロックB、体積2のブロックCからなり、AとBを用いて作成できる立体で1組目のシルエットを達成し、AとCを用いて作成出来る立体で2組目のシルエットを達成した場合、得られる評価値は \mathrm{round}\left(10^9\times \left(2+3+1/5\right)\right) となる。

各テストケース毎に、\mathrm{round}(10^9\times \frac{全参加者中の最小評価値}{自身の評価値}) の**相対評価スコア**が得られ、その和が提出の得点となる。

最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定される。 
暫定テスト、システムテストともに、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースの相対評価スコアは0点となり、そのテストケースにおいては「全参加者中の最小評価値」の計算から除外される。 
システムテストは** CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意せよ。

#### テストケース数

- 暫定テスト: 50個 

- システムテスト: 2000個、コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc019/seeds.txt) (sha256=08db725cbdc37734a2e8fbe5bc0cbc1e95d7a68933b7f43dcdd0ffc3d2a95e34) を公開 

- システムテストは与えられるシルエットの大きさが D=5,6,\cdots,14 であるような入力を 200 個ずつ含む 

- seed=0 の入力は手動で作成されたものであり、暫定テスト、システムテストには含まれない。

#### 相対評価システムについて

暫定テスト、システムテストともに、CE 以外の結果を得た一番最後の提出のみが順位表に反映される。 
相対評価スコアの計算に用いられる各テストケース毎の全参加者中の最小評価値の算出においても、順位表に反映されている最終提出のみが用いられる。

順位表に表示されているスコアは相対評価スコアであり、新規提出があるたびに、相対評価スコアが再計算される。 
一方、提出一覧から確認出来る各提出のスコアは各テストケース毎の評価値をそのまま足し合わせたものであり、相対評価スコアは表示されない。 
最新以外の提出の、現在の順位表における相対評価スコアを知るためには、再提出が必要である。 
不正な出力や制限時間超過をした場合、提出一覧から確認出来るスコアは0となるが、順位表には正解したテストケースに対する相対スコアの和が表示されている。

#### 実行時間について

実行時間には多少のブレが生じます。 
また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
そのため、実行時間制限ギリギリの提出がシステムテストでTLEとなる可能性があります。 
プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
D
f_1(0,0) \cdots f_1(0,D-1)
\vdots
f_1(D-1,0) \cdots f_1(D-1,D-1)
r_1(0,0) \cdots r_1(0,D-1)
\vdots
r_1(D-1,0) \cdots r_1(D-1,D-1)
f_2(0,0) \cdots f_2(0,D-1)
\vdots
f_2(D-1,0) \cdots f_2(D-1,D-1)
r_2(0,0) \cdots r_2(0,D-1)
\vdots
r_2(D-1,0) \cdots r_2(D-1,D-1)

```

- D はシルエット画像の大きさを表し、各シルエット画像は D\times D の正方形である。5\leq D\leq 14 を満たす。 

- f_i,r_i は i 番目のシルエット画像の組であり、f_i が正面から見たシルエット、r_i が右側から見たシルエットである。それぞれ D\times D の 01 行列であり、長さ D の01文字列 D 個として与えられる。値の意味については先の「シルエットについて」の項目を参照せよ。 

- 各シルエットについて、全ての1の要素は1つの連結成分をなす。 

- z=0,\cdots,D-1 に対して、f_i(z,0)+\cdots+f_i(z,D-1)\geq 1、r_i(z,0)+\cdots+r_i(z,D-1)\geq 1 を満たす。この条件により、指定されたシルエットの組と合致する立体が必ず存在することが保証される。

### 出力

ブロックの総数を n とする。 
i 番目のシルエットの組に対応するブロックの組み方を、D\times D\times D の三次元配列 b_i(x,y,z) を用いて以下のように表現する。

(x,y,z) と (x+1,y+1,z+1) を対角とする単位立方体の領域を k (1\leq k\leq n) 番のブロックが占めている場合に b_i(x,y,z)=k、どのブロックもその領域を占めていない場合に b(x,y,z)=0。

ブロックは変形できないため、各 i=1,2 について、同じ番号が振られたブロックは同じ形状、すなわち、回転と平行移動によって完全に一致させることが出来なければならない。

このとき、以下の形式で標準出力に出力せよ。

```text
n
b_1(0,0,0) b_1(0,0,1) \cdots b_1(D-1,D-1,D-1)
b_2(0,0,0) b_2(0,0,1) \cdots b_2(D-1,D-1,D-1)

```

ここで、b_i の出力の x\times D^2+y\times D+z 個目が b_i(x,y,z) である。 
各 i=1,\cdots,n に対して、i 番のブロックは b_1 もしくは b_2 の少なくとも一方では使われていなければならない。どちらでも使われていないブロックが存在する場合、WA と判定される。

[例を見る](https://img.atcoder.jp/ahc019/b36525d8.html?lang=ja&seed=0&output=19%0D%0A0+1+2+3+0+4+5+0+3+3+6+0+0+0+3+7+7+0+3+3+0+7+0+8+0+0+5+9+0+0+0+5+0+0+0+0+0+0+0+0+0+0+0+0+0+0+7+0+0+0+0+0+9+10+0+0+0+0+10+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+9+9+0+0+0+9+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+11+12+12+0+13+0+0+12+14+0+0+0+0+14+0+0+0+14+14+0+0+0+14+0%0D%0A15+0+0+0+3+14+0+0+0+3+0+0+0+0+9+16+0+0+9+9+0+0+0+17+0+0+0+0+0+0+14+0+10+0+3+14+0+10+0+9+0+0+0+18+0+0+0+0+0+0+0+0+0+0+3+0+0+12+0+3+14+0+12+0+9+0+0+0+0+0+0+0+0+0+0+0+0+0+0+3+0+0+12+0+0+14+0+7+0+0+0+7+7+0+5+0+19+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+5+0+7+0+0+5+0+0+0+0+0%0D%0A)

### サンプルプログラム

Pythonでの解答例を示す。 
このプログラムでは、f(z,x)=1 かつ r(z,y)=1 であるような全ての (x,y,z) に対して、1\times 1\times 1 のブロックを敷き詰めることで目的のシルエットを達成している。

```text
D = int(input())
f = [[] for i in range(2)]
r = [[] for i in range(2)]
for i in range(2):
    for k in range(D):
        f[i].append(input())
    for k in range(D):
        r[i].append(input())

b = [[0 for j in range(D * D * D)] for i in range(2)]
n = 0
for i in range(2):
    for x in range(D):
        for y in range(D):
            for z in range(D):
                if f[i][z][x] == '1' and r[i][z][y] == '1':
                    n += 1
                    b[i][x * D * D + y * D + z] = n

print(n)
print(' '.join(map(str, b[0])))
print(' '.join(map(str, b[1])))

```

### 入力生成方法

L 以上 U 以下の整数値を一様ランダムに生成する関数を \mathrm{randint}(L,U) で表す。 
L 以上 U 未満の浮動小数値を一様ランダムに生成する関数を \mathrm{randdouble}(L,U) で表す。

#### D の生成

D=\mathrm{randint}(5,14) により生成する。

#### f,r の生成

まずはじめに、生成されるシルエットの特徴を決める数値を以下のように生成する。

- p(0)=0 

- p(1)=D^{\mathrm{randdouble}(-1,1)} 

- p(2)=D^{\mathrm{randdouble}(-1,1)} 

- p(3)=D^{\mathrm{randdouble}(-0.5,1.5)} 

- p(4)=D^{\mathrm{randdouble}(-0.5,1.5)}

次に、各 f_1,r_1,f_2,r_2 を以下のように生成する。

生成するシルエット配列を g とし、全ての (z,x) について、g(z,x)=0 と初期化する。 
1 の個数を表す値 m=\mathrm{randint}(2D,\lfloor D^2/2\rfloor) を生成する。 
z=\mathrm{randint}(0,D-1) と x=\mathrm{randint}(0,D-1) を生成し、g(z,x)=1 とする。 
1 の個数が m になるまで、以下の処理を繰り返す。

各 (z,x) について、4方向に隣接する 1 の値の個数を d(z,x) とする。 
g(z,x)=0 であるような (z,x) を p(d(z,x)) に比例する確率で一つ選び、g(z,x)=1 に更新する。

1 の個数が m となったとき、全ての z=0,\cdots,D-1 について、\sum_{x=0}^{D-1}g(z,x)\geq 1 であるかをチェックし、成り立っていない場合は g を0 に再度初期化して m を生成する処理からやり直す。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc019/b36525d8.html?lang=ja): 3D表示が可能です。 

- [ローカル版](https://img.atcoder.jp/ahc019/b36525d8.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc019/b36525d8_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、seed=0 に対するビジュアライザの出力画像(PNG)のみ twitter で共有が可能です。 
 
必ず指定されたハッシュタグを用い、公開アカウントを使用して下さい。共有出来るのはseed=0に対するビジュアライズ結果と点数のみで、動画や出力文字列、他のシードでの点数の共有や、解法・考察に関する言及は禁止です。

[共有された画像の一覧](https://twitter.com/search?q=%23AHC019%20%23visualizer&src=typed_query&f=live)

### 入力例 1

```text
5
10001
11011
11111
10101
10001
01110
11011
10000
11011
01110
11110
00011
01110
11000
11111
11110
00011
01110
00011
11110

```

### 出力例 1

```text
19
0 1 2 3 0 4 5 0 3 3 6 0 0 0 3 7 7 0 3 3 0 7 0 8 0 0 5 9 0 0 0 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0 7 0 0 0 0 0 9 10 0 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 9 9 0 0 0 9 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 11 12 12 0 13 0 0 12 14 0 0 0 0 14 0 0 0 14 14 0 0 0 14 0
15 0 0 0 3 14 0 0 0 3 0 0 0 0 9 16 0 0 9 9 0 0 0 17 0 0 0 0 0 0 14 0 10 0 3 14 0 10 0 9 0 0 0 18 0 0 0 0 0 0 0 0 0 0 3 0 0 12 0 3 14 0 12 0 9 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 12 0 0 14 0 7 0 0 0 7 7 0 5 0 19 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 5 0 7 0 0 5 0 0 0 0 0

```

### Problem Statement

AtCoder is developing an educational toy that combines blocks to create a three-dimensional object with a specified silhouette. 
The toy consists of a set of polycube-shaped blocks consisting of 1\times 1\times 1 cubes joined face to face and a pair of two 2D monochrome silhouette images. 
You will win the game if you can construct a single 3D object by combining the blocks, so that the two silhouettes of the created object, viewed from the front and from the right side, completely match the given silhouette images.

In order to allow children to play for a long time, we want to prepare multiple pairs of silhouettes for a single set of blocks. 
For example, the set of 6 blocks shown on the left figure can be used to create two pairs of silhouettes, as shown on the right.

 ![](https://img.atcoder.jp/ahc019/b36525d8_1.png)

 ![](https://img.atcoder.jp/ahc019/b36525d8_2.png)

As a toy designer, you are given two pairs of front/right silhouettes, and your task is to find a set of blocks from which you can construct two objects having the given pairs of silhouettes, and a way to construct them. 
Because children may accidentally swallow small blocks, sets of blocks consisting only of a small number of large blocks are preferable.

#### Detailed puzzle rules

- You don't have to use all the blocks (but you will get a better score if you do). 

- Each block can be rotated by 90 degrees around the x-axis, y-axis, and z-axis, but cannot be flipped. 

- The blocks must be arranged so that each vertex has integer coordinates, and different blocks must not have a positive common volume. 

- The constructed object do not have to be connected. For the sake of simplicity, we assume that a floating arrangement is also possible (you may interpret this as there are a large number of additional transparent blocks, which can be used to support blocks).

#### About silhouette

Take the x-axis from left to right, the y-axis from front to back, and the z-axis from top to bottom. 
We assume that all blocks are within a cubic region that has (0,0,0) and (D,D,D) as diagonal corners. 
We define a three-dimensional 01-array b(x,y,z) as b(x,y,z)=1 if some block occupies the cubic region that has (x,y,z) and (x+1,y+1,z+1) as diagonal corners, and b(x,y,z)=0 otherwise. 
Then, the silhouette viewed from the front is a two-dimensional 01-array f(z,x) defined as follows.

\[ 
 f(z,x)=\begin{cases} 
 1&(\sum_{y=0}^{D-1}b(x,y,z)\geq 1)\\ 
 0&(\sum_{y=0}^{D-1}b(x,y,z)=0) 
 \end{cases} 
\]

Similarly, the silhouette viewed from the right is a two-dimensional 01-array r(z,y), defined as follows

\[ 
 r(z,y)=\begin{cases} 
 1&(\sum_{x=0}^{D-1}b(x,y,z)\geq 1)\\ 
 0&(\sum_{x=0}^{D-1}b(x,y,z)=0) 
 \end{cases} 
\]

### Scoring

Let v_1,\cdots,v_n be the volume of each block in the created toy set. 
Here, the volume of a block is equal to the number of 1\times 1\times 1 cubes it comprises. 
Each block must be connected, i.e., every cube it comprises must be reachable by moving through shared faces. 
If there is a disconnected block, it will be judged as WA. 
Note that the 3D object created by combining the blocks can be disconnected.

For the i-th pair of silhouettes (i=1,2), let r_i be the sum of the volumes of the blocks that were not used in the output 3D object. 
Let S be the set of blocks that were used in both two objects. 
Then you get the following evaluation value. 
The lower the evaluation value, the better.

\[ 
 \mathrm{round}\left(10^9\times \left(r_1+r_2+\sum_{i\in S} \frac{1}{v_i}\right)\right) 
\]

For example, if the set of blocks consists of block A with volume 5, block B with volume 3, and block C with volume 2, and if the first set of silhouettes is achieved by a 3D object that can be created using A and B and the second set of silhouettes is achieved by a 3D object that can be created using A and C, the resulting evaluation value is \mathrm{round}\left(10^9\times \left(2+3+1/5\right)\right).

For each test case, we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{MIN}}{\mathrm{YOUR}}), where YOUR is your evaluation value and MIN is the lowest evaluation value among all competitors obtained on that test case. The score of the submission is the sum of the relative scores.

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MIN calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
Be careful not to make a mistake in the final submission.

#### Number of test cases

- Provisional test: 50 

- System test: 2000. We will publish [seeds.txt](https://img.atcoder.jp/ahc019/seeds.txt) (sha256=08db725cbdc37734a2e8fbe5bc0cbc1e95d7a68933b7f43dcdd0ffc3d2a95e34) after the contest is over. 

- The system test contains 200 inputs for each D=5,6,\cdots,14, where D is the size of each silhouette. 

- The input for seed=0 is manually created and is not included in the provisional or system test.

#### About relative evaluation system

In both the provisional/system test, the standings will be calculated using only the last submission which received a result other than CE. 
Only the last submissions are used to calculate the MIN for each test case when calculating the relative scores.

The scores shown in the standings are relative, and whenever a new submission arrives, all relative scores are recalculated. 
On the other hand, the score for each submission shown on the submissions page is the sum of the evaluation value for each test case, and the relative scores are not shown. 
In order to know the relative score of submission other than the latest one in the current standings, you need to resubmit it. 
If your submission produces illegal output or exceeds the time limit for some test cases, the score shown on the submissions page will be 0, but the standings show the sum of the relative scores for the test cases that were answered correctly.

#### About execution time

Execution time may vary slightly from run to run. 
In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percent compared to provisional tests. 
For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
Please measure the execution time in your program to terminate the process, or have enough margin in the execution time.

### Input

Input is given from Standard Input in the following format:

```text
D
f_1(0,0) \cdots f_1(0,D-1)
\vdots
f_1(D-1,0) \cdots f_1(D-1,D-1)
r_1(0,0) \cdots r_1(0,D-1)
\vdots
r_1(D-1,0) \cdots r_1(D-1,D-1)
f_2(0,0) \cdots f_2(0,D-1)
\vdots
f_2(D-1,0) \cdots f_2(D-1,D-1)
r_2(0,0) \cdots r_2(0,D-1)
\vdots
r_2(D-1,0) \cdots r_2(D-1,D-1)

```

- D is the size of the silhouette image and satisfies 5\leq D\leq 14. 

- f_i,r_i are the i-th silhouette image pair, where f_i is the silhouette viewed from the front and r_i is the silhouette viewed from the right. Each of them is 01 matrix of size D\times D and is given as D 01 strings of length D. For the meaning of these values, see the previous section "About silhouette". 

- For each silhouette, all 1 elements form one connected component. 

- For z=0,\cdots,D-1, it holds that f_i(z,0)+\cdots+f_i(z,D-1)\geq 1 and r_i(z,0)+\cdots+r_i(z,D-1)\geq 1. This condition guarantees that there always exists a 3D object that matches the specified pair of silhouettes.

### Output

Let n be the total number of blocks. 
The block arrangement corresponding to the i-th pair of silhouettes is expressed as follows using a three-dimensional array b_i(x,y,z) of size D\times D\times D.

b_i(x,y,z)=k if the k (1\leq k\leq n)-th block occupies the cubic region that has (x,y,z) and (x+1,y+1,z+1) as diagonal corners, and b(x,y,z)=0 if none of the blocks occupy that region.

Since blocks cannot be reshaped, the same numbered blocks appearing in both i=1,2 must have the same shape, i.e., they must be able to be perfectly matched by rotation and translation.

Then, output to Standard Output in the following format.

```text
n
b_1(0,0,0) b_1(0,0,1) \cdots b_1(D-1,D-1,D-1)
b_2(0,0,0) b_2(0,0,1) \cdots b_2(D-1,D-1,D-1)

```

Here, the (x\times D^2+y\times D+z)-th output for b_i is b_i(x,y,z). 
For each i=1,\cdots,n, the i-th block must be used in at least one of b_1 or b_2. If there is a block that is not used in either, it is judged as WA.

[Show example](https://img.atcoder.jp/ahc019/b36525d8.html?lang=en&seed=0&output=19%0D%0A0+1+2+3+0+4+5+0+3+3+6+0+0+0+3+7+7+0+3+3+0+7+0+8+0+0+5+9+0+0+0+5+0+0+0+0+0+0+0+0+0+0+0+0+0+0+7+0+0+0+0+0+9+10+0+0+0+0+10+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+9+9+0+0+0+9+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+11+12+12+0+13+0+0+12+14+0+0+0+0+14+0+0+0+14+14+0+0+0+14+0%0D%0A15+0+0+0+3+14+0+0+0+3+0+0+0+0+9+16+0+0+9+9+0+0+0+17+0+0+0+0+0+0+14+0+10+0+3+14+0+10+0+9+0+0+0+18+0+0+0+0+0+0+0+0+0+0+3+0+0+12+0+3+14+0+12+0+9+0+0+0+0+0+0+0+0+0+0+0+0+0+0+3+0+0+12+0+0+14+0+7+0+0+0+7+7+0+5+0+19+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+0+5+0+7+0+0+5+0+0+0+0+0%0D%0A)

### Sample Solution

This is a sample solution in Python. 
In this program, the target silhouette is achieved by placing a unit block of size 1\times 1\times 1 for all (x,y,z) such that f(z,x)=1 and r(z,y)=1.

```text
D = int(input())
f = [[] for i in range(2)]
r = [[] for i in range(2)]
for i in range(2):
    for k in range(D):
        f[i].append(input())
    for k in range(D):
        r[i].append(input())

b = [[0 for j in range(D * D * D)] for i in range(2)]
n = 0
for i in range(2):
    for x in range(D):
        for y in range(D):
            for z in range(D):
                if f[i][z][x] == '1' and r[i][z][y] == '1':
                    n += 1
                    b[i][x * D * D + y * D + z] = n

print(n)
print(' '.join(map(str, b[0])))
print(' '.join(map(str, b[1])))

```

### Input Generation

Let \mathrm{randint}(L,U) be a function that generates a uniform random integer between L and U, inclusive. 
Let \mathrm{randdouble}(L,U) be a function that generates a uniform random floating-point number at least L and less than U.

#### Generation of D

We generate D by D=\mathrm{randint}(5,14).

#### Generation of f,r

First, we generate the following values which determine the characteristics of the silhouettes to be generated.

- p(0)=0 

- p(1)=D^{\mathrm{randdouble}(-1,1)} 

- p(2)=D^{\mathrm{randdouble}(-1,1)} 

- p(3)=D^{\mathrm{randdouble}(-0.5,1.5)} 

- p(4)=D^{\mathrm{randdouble}(-0.5,1.5)}

Next, we generate each f_1,r_1,f_2,r_2 as follows.

Let g be the silhouette array to be generated and initialize it with g(z,x)=0 for all (z,x). 
Generate a value m=\mathrm{randint}(2D,\lfloor D^2/2\rfloor) representing the number of 1. 
Generate z=\mathrm{randint}(0,D-1) and x=\mathrm{randint}(0,D-1), and then set g(z,x)=1. 
Repeat the following process until the number of 1 becomes m.

For each (z,x), let d(z,x) be the number of adjacent 1 in the four directions. 
Choose (z,x) such that g(z,x)=0 with probability proportional to p(d(z,x)) and update g(z,x)=1.

When the number of 1 becomes m, check whether \sum_{x=0}^{D-1}g(z,x)\geq 1 for all z=0,\cdots,D-1, and if not, reinitialize g to 0 and retry the process from generating m.

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc019/b36525d8.html?lang=en): You can see 3D animation. 

- [Local version](https://img.atcoder.jp/ahc019/b36525d8.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc019/b36525d8_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

You are allowed to share output images (PNG) of the provided visualizer for seed=0 on twitter during the contest. 
 
You have to use the specified hashtag and public account. You can only share visualization results and scores for seed=0. Do not share videos, output itself, scores for other seeds or mention solutions or discussions.

[List of shared images](https://twitter.com/search?q=%23AHC019%20%23visualizer&src=typed_query&f=live)

### Sample Input 1

```text
5
10001
11011
11111
10101
10001
01110
11011
10000
11011
01110
11110
00011
01110
11000
11111
11110
00011
01110
00011
11110

```

### Sample Output 1

```text
19
0 1 2 3 0 4 5 0 3 3 6 0 0 0 3 7 7 0 3 3 0 7 0 8 0 0 5 9 0 0 0 5 0 0 0 0 0 0 0 0 0 0 0 0 0 0 7 0 0 0 0 0 9 10 0 0 0 0 10 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 9 9 0 0 0 9 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 11 12 12 0 13 0 0 12 14 0 0 0 0 14 0 0 0 14 14 0 0 0 14 0
15 0 0 0 3 14 0 0 0 3 0 0 0 0 9 16 0 0 9 9 0 0 0 17 0 0 0 0 0 0 14 0 10 0 3 14 0 10 0 9 0 0 0 18 0 0 0 0 0 0 0 0 0 0 3 0 0 12 0 3 14 0 12 0 9 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 12 0 0 14 0 7 0 0 0 7 7 0 5 0 19 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 5 0 7 0 0 5 0 0 0 0 0

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

- [ルール](https://atcoder.jp/contests/ahc019/rules)
 
- [用語集](https://atcoder.jp/contests/ahc019/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
