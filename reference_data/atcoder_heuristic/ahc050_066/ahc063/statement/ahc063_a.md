### ストーリー

高橋くんは「ウロボロス」と名付けた蛇をペットとして飼っている。 
この蛇に餌を与えると、しっぽが 1 m 伸び、伸びた部分が餌の色に染まる。 
また、自身の胴体を噛みちぎることができ、千切れたしっぽ側は餌となる。

高橋くんはウロボロスに餌を順に与え、体色の並びを自分の好みに変えようと考えた。 
ところが手が滑り、餌が部屋中に散乱してしまった。 
ウロボロスに指示を出して餌をすべて食べさせ、最終的な体色の並びをできるだけ自分の好みに近づけてほしい。 
指示を出す回数は少なければ少ないほど良い。

![example](https://img.atcoder.jp/ahc063/EbfTtMTp.gif)

### 問題文

N \times N マスの盤面がある。 
左上のマスの座標を (0,0) とし、そこから下方向に i マス、右方向に j マス進んだ先のマスの座標を (i,j) とする。

盤面上には M-5 個の餌が置かれており、各餌には整数 1,\cdots,C で表される色が付いている。

この盤面上を蛇が移動する。 
長さ k の蛇は、頭からしっぽに向かって、k 個の座標 (i_0,j_0),\cdots,(i_{k-1},j_{k-1}) と色 c_0,\cdots,c_{k-1} の列で表される。 
(i_0,j_0) を**頭**、(i_1,j_1),\cdots,(i_{k-2},j_{k-2}) を**胴体**、(i_{k-1},j_{k-1}) を**しっぽ**と呼ぶ。 
しっぽは他と同じ座標であってもよく、それ以外の座標はすべて互いに異なる。

初期状態では、長さ 5 で、色がすべて 1 の蛇が (4,0),(3,0),(2,0),(1,0),(0,0) の位置にいる。 
初期状態で蛇が占めるマスには餌は存在しない。

以下の操作を最大で 100000 ターン繰り返す。

各ターンでは、あなたは上下左右の方向 d を指示し、以下の順に蛇が行動する。

- **移動:** 
 まず、蛇がその方向に 1 マス移動する。 
 頭の位置から d 方向に 1 マス進んだ先の座標を (i_0',j_0') とすると、移動後の蛇の座標は (i_0',j_0'),(i_0,j_0),\cdots,(i_{k-2},j_{k-2}) となる。 
 ただし、Uターンとなる方向、すなわち (i_0',j_0')=(i_1,j_1) となる方向、および N \times N マスの外に出る方向は指定できない。 

- **食事:** 
 移動先に色 c' の餌がある場合、蛇の長さは 1 伸び、座標は (i_0',j_0'),(i_0,j_0),\cdots,(i_{k-2},j_{k-2}),(i_{k-1},j_{k-1}) となり、色は c_0,\cdots,c_{k-1},c' となる。 

- **噛みちぎり:** 
 移動後の頭の位置に、移動後の胴体がある場合を考える。 
 そのインデックスを h とする。すなわち、1\leq h\leq k-2 であって、移動後の座標列 (i_0',j_0'),\cdots,(i_{k-1}',j_{k-1}') に対して (i_0',j_0')=(i_h',j_h') が成り立つとする。 
 このとき、蛇は自らの胴体を噛みちぎって長さが h+1 になり、残りのしっぽ側は対応する色の餌に変わる。 
 すなわち、蛇の座標は (i_0',j_0'),\cdots,(i_h',j_h') となり、各 p=h+1,\cdots,k-1 について、(i_p',j_p') に色 c_p の餌が置かれる。 
 噛みちぎり後はしっぽと頭が同じ座標を共有した状態となる。 
 なお、食事と噛みちぎりが同一ターンに同時に発生することはない。

好みの色列 d_0,\cdots,d_{M-1} が与えられる。 
できるだけ短い操作列によって、最終的な蛇の色列を好みの色列に近づけてほしい。

### 得点

出力した操作列の長さを T、操作完了時点における蛇の長さを k、蛇の色列を c_0,\cdots,c_{k-1} とする。 
ここで、k は M 未満であってもよい。 
また、誤差 E を、c_p\neq d_p であるような p=0,\cdots,k-1 の個数と定義する。

このとき、以下の絶対スコアが得られる。

\[ 
T + 10000 \times (E + 2(M-k)) 
\]

絶対スコアは小さいほど良い。

各テストケース毎に、絶対スコアの小さい順での順位に応じた**順位スコア**が得られ、その和が提出の得点となる。 
順位スコアは以下のように計算され、高ければ高いほど良い。

コンテストの提出者数を n_{submit}、自身より小さい絶対スコアを獲得した参加者の人数を n_{lose}、自身と等しい絶対スコアを獲得した他の参加者の人数を n_{tie} とする。この時、このテストケースにおける順位は r=n_{lose}+0.5 n_{tie} と定まり、順位スコアは \mathrm{round}(10^9\times (1-\frac{r}{n_{submit}})) となる。

最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定される。 
暫定テスト、システムテストともに、不正な出力や制限時間超過をした場合、そのテストケースの順位スコアのみ0点となる。 
システムテストは** CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意せよ。

#### テストケース数

- 暫定テスト: 50個 

- システムテスト: 2000個、コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc063/seeds.txt) (sha256=daf8cb67f5deb3bcb35e3d34b97a3b7b7a3ee8da1fe1cbe24f39389ad64a6c85) を公開

#### 相対評価システムについて

暫定テスト、システムテストともに、CE 以外の結果を得た一番最後の提出のみが順位表に反映される。

順位表に表示されているスコアは相対評価スコアであり、新規提出があるたびに、相対評価スコアが再計算される。 
一方、提出一覧から確認出来る各提出のスコアは各テストケース毎の絶対スコアをそのまま足し合わせたものであり、相対評価スコアは表示されない。 
最新以外の提出の、現在の順位表における相対評価スコアを知るためには、再提出が必要である。 
不正な出力や制限時間超過をした場合、提出一覧から確認出来るスコアは0となるが、順位表には正解したテストケースに対する順位スコアの和が表示されている。

#### 実行時間について

実行時間には多少のブレが生じます。 
また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
そのため、実行時間制限ギリギリの提出がシステムテストでTLEとなる可能性があります。 
プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
N M C
d_0 \cdots d_{M-1}
f_{0,0} \cdots f_{0,N-1}
\vdots
f_{N-1,0} \cdots f_{N-1,N-1}

```

- 盤面サイズ N は 8 以上 16 以下の整数である。 

- M は好みの色列の長さを表し、N^2/4 以上 3N^2/4 以下の整数である。 

- 色数 C は 3 以上 7 以下の整数である。 

- d_0,\cdots,d_{M-1} は好みの色列を表し、各 d_p は 1 以上 C 以下の整数である。また、d_0=\cdots=d_4=1 を満たす。 

- f_{i,j} は初期状態においてマス (i,j) に置かれている餌の色を表す 0 以上 C 以下の整数である。 

- f_{i,j}=0 の場合、そのマスには餌は置かれていない。 

- 1 \leq f_{i,j} \leq C の場合、そのマスには色 f_{i,j} の餌が置かれている。 

- f_{0,0}=\cdots=f_{4,0}=0 が成り立つ。 

- 餌が置かれているマスの数はちょうど M-5 である。

### 出力

操作回数を T とする。 
t ターン目の移動方向を、上下左右に対応する 1 文字 `U`、`D`、`L`、`R` で表し、これを a_t とする。

このとき、以下の形式で標準出力に出力せよ。

```text
a_0
\vdots
a_{T-1}

```

[例を見る](https://img.atcoder.jp/ahc063/EbfTtMTp.html?lang=ja&seed=0&output=sample)

### サンプルプログラム(Python) 
Python での解答例を示す。 
このプログラムでは、上下に蛇行しながらすべての餌を順に食べている。

```text

import sys
input = sys.stdin.readline

N, M, C = map(int, input().split())
d = list(map(int, input().split()))
f = [list(map(int, input().split())) for _ in range(N)]

ans = []

for _ in range(4, N - 1):
    ans.append('D')

for col in range(1, N):
    ans.append('R')
    if col % 2 == 1:
        for _ in range(N - 1):
            ans.append('U')
    else:
        for _ in range(N - 1):
            ans.append('D')

print('\n'.join(ans))

```

### 入力生成方法

\mathrm{rand}(L, U) を、L 以上 U 以下の整数を一様ランダムに生成する関数とする。

#### N, M, C の生成

- N=\mathrm{rand}(8,16) 

- M=\mathrm{rand}(\lceil\frac{N^2}{4}\rceil,\lfloor\frac{3N^2}{4}\rfloor) 

- C=\mathrm{rand}(3,7)

#### d の生成

d_0=\cdots=d_4=1 とし、残りの M-5 個を以下のように生成する。

n_0=0, n_C=M-5-C とし、C-1 個の乱数 n_1,\cdots,n_{C-1} を \mathrm{rand}(0,M-5-C) により生成し、昇順に並べる。 
これを用いて、各色の個数 m_c を m_c=n_c-n_{c-1}+1 により定める。 
ただし、m_c>(M-5)/2 となる c が存在する場合は、n の生成からやり直す。

各色 c を m_c 個ずつ用意し、ランダムな順に並び替えることにより、d_5,\cdots,d_{M-1} とする。

#### f の生成

初期状態で蛇が占める 5 マス (4,0),(3,0),(2,0),(1,0),(0,0) 以外の N^2-5 マスからランダムに M-5 マスを選び、(i_5,j_5),\cdots,(i_{M-1},j_{M-1}) とする。 
色 d_5,\cdots,d_{M-1} をランダムに並び替えた列を d_5',\cdots,d_{M-1}' とする。 
これらを用いて、各 p=5,\cdots,M-1 について、f_{i_p,j_p}=d_p' とする。

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc063/EbfTtMTp.html?lang=ja): ローカル版より高性能でアニメーション表示と手動プレイが可能です。 

- [ローカル版](https://img.atcoder.jp/ahc063/EbfTtMTp.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc063/EbfTtMTp_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### Story

Takahashi has a pet snake named "Ouroboros". 
When fed, this snake grows its tail by 1 meter, and the newly grown part takes on the color of the food. 
It can also bite off its own body, and the tail part that was bitten off becomes food.

Takahashi planned to feed Ouroboros in sequence so that the order of its body colors would match his preference. 
However, he accidentally dropped the food, scattering it all over the room. 
Your task is to give instructions to Ouroboros so that it eats all the food and make the final order of body colors as close as possible to Takahashi's preference. 
Fewer instructions are better.

![example](https://img.atcoder.jp/ahc063/EbfTtMTp.gif)

### Problem Statement

There is an N \times N grid. 
The coordinate of the top-left cell is (0,0), and the coordinate of the cell obtained by moving i cells downward and j cells to the right from there is (i,j).

There are M-5 pieces of food placed on the grid, and each piece has a color represented by an integer from 1 to C.

A snake moves on this grid. 
A snake of length k is represented by a sequence of k coordinates (i_0,j_0),\cdots,(i_{k-1},j_{k-1}) and colors c_0,\cdots,c_{k-1}, from head to tail. 
(i_0,j_0) is called the **head**, (i_1,j_1),\cdots,(i_{k-2},j_{k-2}) the **body**, and (i_{k-1},j_{k-1}) the **tail**. 
The tail may share the same coordinate with another part, but all other coordinates must be distinct.

Initially, the snake has length 5, all colors are 1, and it occupies (4,0),(3,0),(2,0),(1,0),(0,0). 
No food is placed on the cells occupied by the snake in the initial state.

The following operation is repeated for at most 100000 turns.

In each turn, you specify one of the four directions, up, down, left, or right, and then the snake acts in the following order.

- **Move:** 
 First, the snake moves one cell in that direction. 
 Let (i_0',j_0') be the coordinate reached by moving one cell from the head in direction d. Then, after the move, the coordinates of the snake become (i_0',j_0'),(i_0,j_0),\cdots,(i_{k-2},j_{k-2}). 
 A direction that results in a U-turn, that is, a direction such that (i_0',j_0')=(i_1,j_1), or a direction that goes outside the N \times N grid, cannot be specified. 

- **Eating:** 
 If there is a piece of food of color c' at the destination cell, the length of the snake increases by 1, its coordinates become (i_0',j_0'),(i_0,j_0),\cdots,(i_{k-2},j_{k-2}),(i_{k-1},j_{k-1}), and its colors become c_0,\cdots,c_{k-1},c'. 

- **Biting off:** 
 Consider the case where, after moving, the head is on a cell occupied by the body. 
 Let its index be h. That is, suppose 1\leq h\leq k-2 and (i_0',j_0')=(i_h',j_h') holds for the coordinate sequence after the move, (i_0',j_0'),\cdots,(i_{k-1}',j_{k-1}'). 
 In this case, the snake bites off its own body, its length becomes h+1, and the remaining tail side turns into food with the corresponding colors. 
 More precisely, the coordinates of the snake become (i_0',j_0'),\cdots,(i_h',j_h'), and for each p=h+1,\cdots,k-1, a piece of food of color c_p is placed at (i_p',j_p'). 
 After biting off, the tail and the head share the same coordinate. 
 Note that eating and biting off never occur in the same turn.

A preferred color sequence d_0,\cdots,d_{M-1} is given. 
Your task is to make the final color sequence of the snake as close as possible to the preferred color sequence, using as short a sequence of operations as possible.

### Scoring

Let T be the length of the output operation sequence, k be the length of the snake at the end of the operations, and c_0,\cdots,c_{k-1} be the color sequence of the snake. 
Here, k may be less than M. 
Also, let the error E be the number of indices p=0,\cdots,k-1 such that c_p\neq d_p.

Then, the following absolute score is obtained.

\[ 
T + 10000 \times (E + 2(M-k)) 
\]

A smaller absolute score is better.

For each test case, you will obtain a **rank score** according to your rank determined by lower absolute score. The score of the submission is the total rank score for each test case. The rank score is calculated as follows, and the higher the rank score, the better.

Let n_{submit} be the number of contestants with submissions, n_{lose} be the number of contestants who received an absolute score lower than yours, and n_{tie} be the number of other contestants who received an absolute score equal to yours. Then your rank in this test case is determined as r=n_{lose}+0.5 n_{tie}, and your rank score is \mathrm{round}(10^9\times (1-\frac{r}{n_{submit}})).

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the rank score for those test cases will be zero. 
The system test will be performed only for **the last submission which received a result other than CE **. 
Be careful not to make a mistake in the final submission.

#### Number of test cases

- Provisional test: 50 

- System test: 2000. We will publish [seeds.txt](https://img.atcoder.jp/ahc063/seeds.txt) (sha256=daf8cb67f5deb3bcb35e3d34b97a3b7b7a3ee8da1fe1cbe24f39389ad64a6c85) after the contest is over.

#### About relative evaluation system

In both the provisional/system test, the standings will be calculated using only the last submission which received a result other than CE.

The scores shown in the standings are relative, and whenever a new submission arrives, all relative scores are recalculated. 
On the other hand, the score for each submission shown on the submissions page is the sum of the absolute score for each test case, and the relative scores are not shown. 
In order to know the relative score of submission other than the latest one in the current standings, you need to resubmit it. 
If your submission produces illegal output or exceeds the time limit for some test cases, the score shown on the submissions page will be 0, but the standings show the sum of the relative scores for the test cases that were answered correctly.

#### About execution time

Execution time may vary slightly from run to run. 
In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percent compared to provisional tests. 
For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
Please measure the execution time in your program to terminate the process, or have enough margin in the execution time.

### Input

The input is given from Standard Input in the following format.

```text
N M C
d_0 \cdots d_{M-1}
f_{0,0} \cdots f_{0,N-1}
\vdots
f_{N-1,0} \cdots f_{N-1,N-1}

```

- N, the size of the grid, is an integer between 8 and 16, inclusive. 

- M represents the length of the preferred color sequence, and is an integer between N^2/4 and 3N^2/4, inclusive. 

- C, the number of colors, is an integer between 3 and 7, inclusive. 

- d_0,\cdots,d_{M-1} represents the preferred color sequence, and each d_p is an integer between 1 and C, inclusive. It is guaranteed that d_0=\cdots=d_4=1. 

- f_{i,j} is an integer between 0 and C representing the color of the food placed at cell (i,j) in the initial state. 

- If f_{i,j}=0, no food is placed on that cell. 

- If 1 \leq f_{i,j} \leq C, a piece of food of color f_{i,j} is placed on that cell. 

- It is guaranteed that f_{0,0}=\cdots=f_{4,0}=0. 

- The number of cells with food is exactly M-5.

### Output

Let T be the number of operations. 
Let a_t denote the direction of movement on turn t, represented by one of the characters `U`, `D`, `L`, and `R`, corresponding to up, down, left, and right.

Then, output to Standard Output in the following format.

```text
a_0
\vdots
a_{T-1}

```

[Show example](https://img.atcoder.jp/ahc063/EbfTtMTp.html?lang=en&seed=0&output=sample)

### Sample Solution (Python) 
A sample solution in Python is shown below. 
This program eats all the food in order while moving in a vertical zigzag pattern.

```text

import sys
input = sys.stdin.readline

N, M, C = map(int, input().split())
d = list(map(int, input().split()))
f = [list(map(int, input().split())) for _ in range(N)]

ans = []

for _ in range(4, N - 1):
    ans.append('D')

for col in range(1, N):
    ans.append('R')
    if col % 2 == 1:
        for _ in range(N - 1):
            ans.append('U')
    else:
        for _ in range(N - 1):
            ans.append('D')

print('\n'.join(ans))

```

### Input Generation

Let \mathrm{rand}(L, U) be a function that generates an integer uniformly at random between L and U, inclusive.

#### Generation of N, M, and C

- N=\mathrm{rand}(8,16) 

- M=\mathrm{rand}(\lceil\frac{N^2}{4}\rceil,\lfloor\frac{3N^2}{4}\rfloor) 

- C=\mathrm{rand}(3,7)

#### Generation of d

Set d_0=\cdots=d_4=1, and generate the remaining M-5 values as follows.

Let n_0=0 and n_C=M-5-C. 
Generate C-1 random numbers n_1,\cdots,n_{C-1} by \mathrm{rand}(0,M-5-C), and sort them in ascending order. 
Using these, define the number of occurrences of each color m_c by m_c=n_c-n_{c-1}+1. 
If there exists a c such that m_c>(M-5)/2, generate n again from scratch.

Prepare m_c copies of each color c, and shuffle them in random order to obtain d_5,\cdots,d_{M-1}.

#### Generation of f

Choose M-5 cells uniformly at random from the N^2-5 cells other than the five cells occupied by the snake in the initial state, (4,0),(3,0),(2,0),(1,0),(0,0), and denote them by (i_5,j_5),\cdots,(i_{M-1},j_{M-1}). 
Let d_5',\cdots,d_{M-1}' be a random permutation of d_5,\cdots,d_{M-1}. 
Using these, set f_{i_p,j_p}=d_p' for each p=5,\cdots,M-1.

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc063/EbfTtMTp.html?lang=en): This is more powerful than the local version providing animations and manual play. 

- [Local version](https://img.atcoder.jp/ahc063/EbfTtMTp.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc063/EbfTtMTp_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.
