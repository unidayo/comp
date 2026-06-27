### ストーリー

高橋市では現在、新たなごみ処理場を建設中である。 
この処理場にはさまざまな種類のごみが運び込まれるため、処理を行う前に分別する必要がある。 
分別は、例えば磁石によって鉄を含むものとそれ以外を分けるなど、いくつかの機器を使って行うことができる。 
ただし、分別器は確率的にごみを二つの経路に振り分ける仕組みであり、ごみの種類を正確に識別することはできない。 
そのため、例えば形状によって缶とそれ以外を分けた後に、磁石によってアルミ缶とスチール缶を分けるなど、複数の分別器を適切に組み合わせることが必要である。 
複数の分別器をうまく組み合わせることにより、できるだけ正確にごみの分別を行ってほしい。

### 問題文

N 種類のごみを分別して処理するためのごみ処理場を建設中である。 
ごみ処理場は2次元平面上の 0\leq x\leq 10^4、0\leq y\leq 10^4 からなる正方形領域として表現される。 
この処理場にはさまざまな種類のごみが**ごみ搬入口**から運び込まれる予定であり、**処理装置**と**分別器**を設置し、それらを**ベルトコンベア**で結ぶことにより、できるだけ正確に分別を行った上で処理を行うことができるようにしたい。

#### ごみ搬入口

処理場内に一つだけ存在し、座標は (0,5000) である。 
搬入口からは一本のベルトコンベアを伸ばし、処理装置または分別器へとごみを運ぶ。

#### 処理装置

処理場内に各ごみの種類ごとに処理装置を 1 台ずつ設置する。 
各ごみを正しい処理装置に運び込むことが目的である。

ごみ処理場内には処理装置を設置できるスペースがちょうど N 箇所存在し、i 番目の設置場所の座標は (x^d_i,y^d_i) である。 
どの設置場所にどの種類のごみの処理装置を設置するかは自由に選ぶことができる。

#### 分別器

K 種類のごみ自動分別器がある。 
**同じ分別器を複数設置しても構わない。**

各分別器は運び込まれたごみを確率的に2つの経路のいずれかへと振り分け、出口1または出口2から運び出す。 
このとき、どの経路に進むかはごみの種類ごとに定まった確率に従う。 
i 番目の自動分別器が種類 j のごみを出口1から運び出す確率は p_{i,j} であり、出口2から運び出す確率は 1-p_{i,j} である。

ごみ処理場内には分別器を設置するためのスペースが M 箇所存在し、i 番目の設置場所の座標は (x^s_i,y^s_i) である。 
各スペースには高々 1 台の分別器を設置することができる。

#### ベルトコンベア

ごみ搬入口ならびに各分別器の二つの出口からはベルトコンベアを伸ばし、処理装置もしくは他の分別器へとごみを運ぶ必要がある。 
ベルトコンベアは始点と終点を結ぶ線分として敷設され、端点をひとつも共有しない二つのベルトコンベアは共有点を持ってはならない。 
この条件では、分別器の二つの出口の行き先が同じであることも許容されている。

ごみ搬入口・処理装置・分別器を頂点、ベルトコンベアを辺とした有向グラフを考えたとき、有向グラフに閉路（自己閉路を含む）が存在してはならない。

#### ヒント: 線分の交差判定について

線分 p_1p_2 と q_1q_2 が、共有点を持つかどうかは、以下の疑似コードにより判定できる。 
なお、計算される値はすべて整数値のため、誤差は発生しない。

```text

def sign(x):
    return 1 if x > 0 else -1 if x < 0 else 0

def orientation(a, b, c):
    cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)
    return sign(cross)

def segments_intersect(p1, p2, q1, q2):
    if (max(p1.x, p2.x) < min(q1.x, q2.x) or
        max(q1.x, q2.x) < min(p1.x, p2.x) or
        max(p1.y, p2.y) < min(q1.y, q2.y) or
        max(q1.y, q2.y) < min(p1.y, p2.y)):
        return False
    o1 = orientation(p1, p2, q1)
    o2 = orientation(p1, p2, q2)
    o3 = orientation(q1, q2, p1)
    o4 = orientation(q1, q2, p2)
    return (o1 * o2 <= 0) and (o3 * o4 <= 0)

```

### 得点

i 種類目のごみが、それに対応する処理装置に最終的に運び込まれる確率を q_i と定義する。 
このとき、以下の絶対スコアが得られる：

\[ 
 \mathrm{round}\left(10^9\times \frac{1}{N}\sum_{i=0}^{N-1}(1-q_i)\right) 
\]

**絶対スコアは小さければ小さいほど良い。**

各テストケース毎に、\mathrm{round}(10^9\times \frac{全参加者中の最小絶対スコア}{自身の絶対スコア}) の**相対評価スコア**が得られ、その和が提出の得点となる。

最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定される。 
暫定テスト、システムテストともに、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースの相対評価スコアは0点となり、そのテストケースにおいては「全参加者中の最小絶対スコア」の計算から除外される。 
システムテストは** CE 以外の結 
果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意せよ。

#### テストケース数

- 暫定テスト: 50個 

- システムテスト: 2000個、コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc051/seeds.txt) (sha256=b61fb5678d480a79e25a50141403e14e8e245d5699e90aa53d4fad893e10dee8) を公開

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

### 入力

入力は以下の形式で標準入力から与えられる。

```text
N M K
x^d_0 y^d_0
\vdots
x^d_{N-1} y^d_{N-1}
x^s_0 y^s_0
\vdots
x^s_{M-1} y^s_{M-1}
p_{0,0} \cdots p_{0,N-1}
\vdots
p_{K-1,0} \cdots p_{K-1,N-1}

```

**制約を修正しました(8/1 19:30)**

- ごみの種類数 N は 5\leq N\leq 20 を満たす。 

- 分別器設置場所の個数 M は 10N\leq M\leq 50N を満たす。 

- 分別器の種類数 K は N\leq K\leq 4N を満たす。 

- i 番目の処理装置設置場所の座標 (x^d_i,y^d_i) は、0\leq x^d_i,y^d_i\leq 10^4 を満たす整数である。 

- i 番目の分別器設置場所の座標 (x^s_i,y^s_i) は、0\leq x^s_i,y^s_i\leq 10^4 を満たす整数である。 

- 搬入口およびすべての設置場所の座標は互いに異なる。 

- i 番目の分別器が種類 j のごみを出口1から運び出す確率 p_{i,j} は実数であり、0\leq p_{i,j}\leq 1 を満たす。

### 出力

ベルトコンベアの行き先を、以下のように番号で表現する。

- 行き先が i 番目の処理装置設置場所である場合は i 

- 行き先が i 番目の分別器設置場所である場合は N+i

i 番目の処理装置設置場所に設置する処理装置の番号を d_i (0\leq d_i\leq N-1) とする。 
ごみ搬入口から出るベルトコンベアの行き先を表す番号を s (0\leq s\leq N+M-1) とする。 
このとき、以下の形式で標準出力に出力せよ。

```text
d_0 \cdots d_{N-1}
s

```

続けて、M 行を標準出力に出力せよ。
 
i 行目は、i 番目の分別器設置場所の状態に応じて以下のように出力する。

- 分別器を設置しない場合：

```text
-1

```

- k 番目 (0\leq k\leq K-1) の分別器を設置し、出口1から出るベルトコンベアの行き先を表す番号が v_1、出口2から出るベルトコンベアの行き先を表す番号が v_2 である場合（0\leq v_1,v_2\leq N+M-1）：

```text
k v_1 v_2

```

ベルトコンベアの行き先が分別器設置場所である場合、そこには分別器が設置されていなければならない。

[例を見る](https://img.atcoder.jp/ahc051/jdd9gfQC.html?lang=ja&seed=0&output=sample)

### 入力生成方法 

- \mathrm{rand}(L,U)：L 以上 U 以下の整数を一様ランダムに生成する。

N、M、K は、それぞれの範囲内から一様ランダムに生成される。

N 個の処理装置設置場所と M 個の分別器設置場所の座標は、以下の手順で N+M 個の座標を順に生成し、前半の N 個を処理装置設置場所、後半の M 個を分別器設置場所とする。

使用済み座標集合 S を S=\{(0,5000)\} により初期化する。
 
x=\mathrm{rand}(0,10^4), y=\mathrm{rand}(0,10^4) により座標 (x,y) を生成する。
 
生成した点とのユークリッド距離が 100 以下の点が S に含まれない場合、この点を採用し、S に加える。
 
この操作を N+M 個の点が生成されるまで繰り返す。

分類確率 p_{i,j} はそれぞれ独立に、\mathrm{rand}(1000,9000)\times 10^{-4} により生成される。

### サンプルプログラム(Python) 
Python による解答例を示す。 
このプログラムでは、以下の処理を行っている。

1. i 番の位置に i 番の処理装置を設置する
 
2. 搬入口と最も近い分別器設置場所を結ぶ
 
3. 0 番の分別器を設置し、出口1を最も確率が高いごみ種の処理装置に、出口2を最も確率が低いごみ種の処理装置に接続する

```text

import sys
import math

input = sys.stdin.readline

# 入力読み込み
N, M, K = map(int, input().split())
processor_positions = []
for _ in range(N):
    x, y = map(int, input().split())
    processor_positions.append((x, y))
sorter_positions = []
for _ in range(M):
    x, y = map(int, input().split())
    sorter_positions.append((x, y))
prob = []
for _ in range(K):
    row = list(map(float, input().split()))
    prob.append(row)

# i番の位置にi番の処理装置を設置
proc_assign = ' '.join(str(i) for i in range(N))
# 搬入口 (0,5000) と最も近い設置場所を結ぶ
inlet = (0, 5000)
dist_sq = [((x - inlet[0])**2 + (y - inlet[1])**2, i) for i, (x, y) in enumerate(sorter_positions)]
_, nearest_i = min(dist_sq)
inlet_conn = N + nearest_i

# 0番の分別器を設置し、出口1を一番確率の高いごみ種の処理装置と、出口2を一番確率の低いごみ種の処理装置と結ぶ
first_row = prob[0]
imax = first_row.index(max(first_row))
imin = first_row.index(min(first_row))
sorter_assigns = []
for i in range(M):
    if i == nearest_i:
        sorter_assigns.append(f"0 {imax} {imin}")
    else:
        sorter_assigns.append("-1")

print(proc_assign)
print(inlet_conn)
print("\n".join(sorter_assigns))

```

### ツール(入力ジェネレータ・ビジュアライザ) 

- [Web版](https://img.atcoder.jp/ahc051/jdd9gfQC.html?lang=ja): ローカル版より高性能でアニメーション表示が可能です。 

- [ローカル版](https://img.atcoder.jp/ahc051/jdd9gfQC.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 

- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc051/jdd9gfQC_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。

### 入力例 1

```text
13 133 47
1858 8963
2368 1117
2507 4965
7710 890
1468 578
3244 9299
5025 8511
9642 4334
599 4476
1388 2454
4779 5389
8676 3397
6156 8034
4102 6415
8251 3509
9320 4268
1193 1486
4847 2021
4440 9046
8991 8142
6633 5942
3778 8637
6876 74
6067 1863
8407 3044
1604 822
9775 4772
9083 1045
9803 847
7455 5575
3183 1259
8407 2922
371 3058
4890 3331
4014 5211
2054 4998
4121 2752
6559 6840
5346 2079
5773 6895
335 4826
2920 8652
7735 1367
6162 8431
9157 7821
4653 704
6642 7613
8857 6949
3623 8476
7022 6237
4959 8592
2981 3756
2329 7006
2328 3175
249 3861
3051 8548
2673 996
714 7101
9170 2616
70 490
9474 7442
3173 4483
6962 8988
129 8154
1965 3249
9297 1953
6438 9318
5926 6158
1136 6845
2191 7256
475 5531
3383 5500
4499 7348
1062 6942
3835 7850
6357 1687
7532 3576
8723 8738
760 2627
8460 5002
4686 7214
9095 2003
5976 7913
3428 9235
266 2484
527 8521
6142 4813
588 1327
1267 4124
5523 7202
6286 4698
2459 6133
3068 689
5979 5443
3284 133
4996 3507
7965 9183
5634 316
4940 7810
1672 8535
6853 4148
9340 5193
8069 632
1836 7667
2546 2085
4054 4133
3953 2870
842 1426
3115 3095
3310 6422
2990 4161
9395 8903
5047 8714
5566 3742
6731 7534
6722 749
4764 7151
1713 7025
4254 6934
6297 8912
4215 462
3514 8224
398 9070
6611 8531
9596 5432
1411 5057
7552 8981
6596 3135
6479 5998
7299 423
2041 3637
710 6793
2958 189
1252 5867
3588 6591
1057 5697
8112 9569
6711 6792
6763 6384
616 7507
5311 7674
9751 5960
5991 4435
3668 4186
7865 9219
1535 4695
0.7947 0.5717 0.1458 0.7273 0.1395 0.5274 0.7105 0.6630 0.5254 0.1861 0.8898 0.4124 0.8373
0.8285 0.6842 0.7812 0.7602 0.8161 0.6065 0.2316 0.3189 0.1706 0.1754 0.5127 0.1849 0.1398
0.5147 0.6140 0.5256 0.7881 0.4148 0.3610 0.7228 0.5083 0.7442 0.6778 0.7014 0.1979 0.7725
0.3479 0.5942 0.2791 0.6912 0.5923 0.7850 0.8101 0.4307 0.1955 0.8941 0.5841 0.7685 0.3365
0.4116 0.3389 0.7719 0.8792 0.5796 0.5751 0.1132 0.2053 0.5233 0.8496 0.5604 0.7999 0.8949
0.6201 0.3431 0.3317 0.8918 0.7173 0.8363 0.8314 0.7755 0.2087 0.1003 0.3819 0.8538 0.1853
0.5341 0.2075 0.3356 0.1762 0.2904 0.8025 0.5798 0.8951 0.5324 0.6448 0.3785 0.5311 0.8397
0.7767 0.4043 0.7621 0.4336 0.7617 0.3717 0.5117 0.8811 0.3867 0.5574 0.2229 0.4948 0.2109
0.2918 0.1748 0.6747 0.8715 0.2433 0.5604 0.8109 0.6585 0.4850 0.4937 0.7828 0.5056 0.4923
0.2074 0.5636 0.2931 0.7722 0.4598 0.3112 0.2513 0.1173 0.8946 0.2885 0.3746 0.4158 0.1289
0.6646 0.8395 0.1890 0.5334 0.1160 0.7439 0.5963 0.1967 0.3549 0.2631 0.4714 0.3812 0.1210
0.5868 0.4050 0.2678 0.2920 0.6503 0.3704 0.4336 0.5353 0.6986 0.1129 0.2086 0.1183 0.2040
0.4108 0.1430 0.2123 0.4538 0.6206 0.2628 0.1833 0.6750 0.7179 0.4557 0.5837 0.3481 0.8241
0.7121 0.6299 0.6964 0.6125 0.7442 0.4419 0.2720 0.1329 0.2044 0.6554 0.2180 0.7193 0.7337
0.7011 0.1872 0.6595 0.1431 0.4645 0.5205 0.7408 0.1855 0.8220 0.7275 0.2853 0.3204 0.4627
0.5218 0.6348 0.3389 0.8584 0.2942 0.2740 0.4592 0.2506 0.8733 0.8386 0.6548 0.2667 0.8584
0.8958 0.7544 0.4056 0.1245 0.6517 0.8921 0.7763 0.3818 0.7530 0.3751 0.6342 0.7434 0.4215
0.2749 0.3961 0.4172 0.1673 0.4351 0.3963 0.5220 0.2972 0.3792 0.5992 0.7891 0.5944 0.4958
0.5049 0.1172 0.6678 0.4131 0.2468 0.7376 0.6962 0.5892 0.3075 0.4466 0.6821 0.3681 0.3261
0.2087 0.1496 0.2146 0.6869 0.4596 0.7237 0.2283 0.2242 0.7294 0.1675 0.8079 0.6417 0.7165
0.4842 0.3767 0.7664 0.7524 0.5782 0.1098 0.3663 0.5658 0.3647 0.1522 0.6769 0.7871 0.5725
0.4457 0.2750 0.3091 0.7717 0.6955 0.1686 0.5195 0.5703 0.7169 0.4944 0.5972 0.2510 0.3722
0.6483 0.3851 0.3562 0.8226 0.4714 0.5673 0.2170 0.8736 0.8041 0.4816 0.6178 0.1427 0.1894
0.1430 0.2933 0.3126 0.8151 0.7037 0.5676 0.2704 0.3305 0.6220 0.1465 0.2080 0.5824 0.1398
0.2149 0.2998 0.1146 0.8847 0.8714 0.1295 0.8328 0.4172 0.1081 0.3282 0.3044 0.7896 0.5094
0.5783 0.8544 0.7956 0.2967 0.7709 0.1041 0.1021 0.6928 0.5704 0.2814 0.5847 0.5448 0.6967
0.3834 0.5092 0.8551 0.7886 0.1314 0.3960 0.6900 0.4573 0.5070 0.6776 0.6555 0.5269 0.3392
0.1611 0.5018 0.1274 0.1432 0.3938 0.4762 0.5130 0.8322 0.8457 0.8087 0.1703 0.1075 0.1604
0.1997 0.7419 0.1717 0.4030 0.4894 0.3377 0.3162 0.6859 0.1615 0.8698 0.5788 0.8138 0.5818
0.4859 0.3316 0.8085 0.4157 0.8243 0.4599 0.5926 0.5802 0.6918 0.1755 0.3048 0.5425 0.3713
0.5403 0.1570 0.4645 0.7979 0.2681 0.8216 0.2177 0.5360 0.6601 0.7411 0.4305 0.8996 0.8515
0.3653 0.8646 0.2023 0.2481 0.5197 0.8658 0.8288 0.2090 0.3178 0.3384 0.6897 0.4686 0.1087
0.2876 0.4619 0.5801 0.5664 0.6368 0.6487 0.8110 0.6732 0.1831 0.4944 0.6158 0.5406 0.5401
0.4234 0.3891 0.2370 0.7762 0.8843 0.8458 0.6728 0.3381 0.4985 0.2366 0.8303 0.1040 0.5701
0.1297 0.3075 0.6430 0.4957 0.7719 0.6939 0.7531 0.5474 0.5737 0.7540 0.4508 0.1765 0.7806
0.5881 0.7300 0.7944 0.7544 0.1365 0.1780 0.5800 0.5754 0.3439 0.8311 0.4328 0.7358 0.3163
0.2535 0.5585 0.6483 0.1642 0.1291 0.8911 0.7827 0.8863 0.7378 0.4760 0.3481 0.8681 0.7335
0.4697 0.1786 0.3000 0.4476 0.3325 0.7877 0.5377 0.6881 0.5053 0.8013 0.5359 0.6286 0.3323
0.8809 0.5817 0.2729 0.3885 0.2775 0.4435 0.8356 0.2798 0.5751 0.6667 0.4997 0.7838 0.6363
0.6267 0.7182 0.6751 0.7085 0.4753 0.7842 0.8322 0.4385 0.7766 0.1075 0.1791 0.8696 0.1037
0.4336 0.8460 0.5259 0.5350 0.7094 0.1762 0.4207 0.7375 0.4378 0.2340 0.5642 0.7896 0.3129
0.1505 0.5580 0.3774 0.7196 0.7524 0.8902 0.4146 0.1722 0.7837 0.3907 0.2465 0.3367 0.6686
0.7948 0.1337 0.8616 0.4249 0.3155 0.2698 0.1273 0.4942 0.7133 0.2159 0.7039 0.7372 0.4182
0.5986 0.3626 0.1562 0.1946 0.5486 0.1648 0.2525 0.7382 0.8283 0.3628 0.4541 0.2651 0.8978
0.1668 0.4790 0.3921 0.7325 0.3574 0.4265 0.8023 0.8647 0.8215 0.4398 0.2749 0.2215 0.8907
0.4803 0.6133 0.1684 0.1372 0.6250 0.3488 0.5522 0.1836 0.2820 0.2808 0.1747 0.1864 0.7046
0.4214 0.6384 0.4454 0.7190 0.2064 0.4953 0.3946 0.7983 0.7207 0.3334 0.5841 0.6460 0.2462

```

### 出力例 1

```text
0 1 2 3 4 5 6 7 8 9 10 11 12
40
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
0 10 4
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1

```

### Story

The city of Takahashi is currently constructing a new waste processing facility. 
Various types of waste will be brought into this facility, so it must be sorted before processing. 
Sorting can be performed using several types of equipment, such as using magnets to separate items containing iron from others. 
However, the sorters probabilistically distribute waste into two paths and cannot accurately identify the type of waste. 
Therefore, it is necessary to appropriately combine multiple sorters, such as first separating cans from other items by shape, and then separating aluminum cans from steel cans using magnets. 
By effectively combining multiple sorters, please perform waste sorting as accurately as possible.

### Problem Statement

A waste processing facility is under construction to process and sort N types of waste. 
The facility is represented as a square region on a 2D plane with 0\leq x\leq 10^4 and 0\leq y\leq 10^4. 
Various types of waste will be brought into the facility through the **waste inlet**, and by installing **processors** and **sorters** connected by **conveyor belts**, the aim is to sort the waste as accurately as possible before processing.

#### Waste Inlet

There is exactly one waste inlet in the facility, located at coordinates (0,5000). 
A single conveyor belt extends from the inlet to transport waste to a processor or a sorter.

#### Processors

Exactly one processor is installed for each type of waste. 
The goal is to deliver each type of waste to its correct processor.

There are exactly N locations within the facility where processors can be installed, and the coordinates of the i-th location are (x^d_i,y^d_i). 
You are free to assign which type of waste processor to install at each location.

#### Sorters

There are K types of automatic waste sorters. 
**The same sorter may be installed in multiple locations.**

Each sorter probabilistically routes incoming waste into one of two paths, discharging it through either Exit 1 or Exit 2. 
The probability that the i-th sorter routes waste of type j to Exit 1 is p_{i,j}, and the probability it goes to Exit 2 is 1-p_{i,j}.

There are M available locations within the facility for installing sorters, and the coordinates of the i-th location are (x^s_i,y^s_i). 
At most one sorter can be installed at each location.

#### Conveyor Belts

From the waste inlet and from each of the two exits of a sorter, conveyor belts must extend to deliver waste to either a processor or another sorter. 
Each conveyor belt is laid as a line segment connecting a start and end point, and two belts that do not share any endpoints must not intersect. 
Under this condition, it is allowed for the two exits of a sorter to be directed to the same destination.

Considering a directed graph where the waste inlet, processors, and sorters are vertices, and conveyor belts are edges, the graph must not contain any cycles (including self-loops).

#### Hint: Segment Intersection Detection

To determine whether two line segments p_1p_2 and q_1q_2 share any point, use the following pseudocode. 
Since all computed values are integers, no rounding errors will occur.

```text

def sign(x):
    return 1 if x > 0 else -1 if x < 0 else 0

def orientation(a, b, c):
    cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)
    return sign(cross)

def segments_intersect(p1, p2, q1, q2):
    if (max(p1.x, p2.x) < min(q1.x, q2.x) or
        max(q1.x, q2.x) < min(p1.x, p2.x) or
        max(p1.y, p2.y) < min(q1.y, q2.y) or
        max(q1.y, q2.y) < min(p1.y, p2.y)):
        return False
    o1 = orientation(p1, p2, q1)
    o2 = orientation(p1, p2, q2)
    o3 = orientation(q1, q2, p1)
    o4 = orientation(q1, q2, p2)
    return (o1 * o2 <= 0) and (o3 * o4 <= 0)

```

### Score

Let q_i be the probability that waste of type i is ultimately delivered to its corresponding processor. 
Then, the following absolute score is awarded:

\[ 
 \mathrm{round}\left(10^9\times \frac{1}{N}\sum_{i=0}^{N-1}(1-q_i)\right) 
\]

**The lower the absolute score, the better.**

For each test case, we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{MIN}}{\mathrm{YOUR}}), where YOUR is your absolute score and MIN is the lowest absolute score among all competitors obtained on that test case. The score of the submission is the sum of the relative scores.

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MIN calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
Be careful not to make a mistake in the final submission.

#### Number of test cases

- Provisional test: 50 

- System test: 2000. We will publish [seeds.txt](https://img.atcoder.jp/ahc051/seeds.txt) (sha256=b61fb5678d480a79e25a50141403e14e8e245d5699e90aa53d4fad893e10dee8) after the contest is over.

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

### Input

Input is given from Standard Input in the following format.

```text
N M K
x^d_0 y^d_0
\vdots
x^d_{N-1} y^d_{N-1}
x^s_0 y^s_0
\vdots
x^s_{M-1} y^s_{M-1}
p_{0,0} \cdots p_{0,N-1}
\vdots
p_{K-1,0} \cdots p_{K-1,N-1}

```

**Constraints have been revised (August 1, 19:30, JST)**

- The number of waste types N satisfies 5\leq N\leq 20. 

- The number of sorter installation locations M satisfies 10N\leq M\leq 50N. 

- The number of sorter types K satisfies N\leq K\leq 4N. 

- The coordinates (x^d_i,y^d_i) of the i-th processor installation location are integers satisfying 0\leq x^d_i,y^d_i\leq 10^4. 

- The coordinates (x^s_i,y^s_i) of the i-th sorter installation location are integers satisfying 0\leq x^s_i,y^s_i\leq 10^4. 

- The coordinates of the inlet and all installation locations are mutually distinct. 

- The probability p_{i,j} that sorter i routes waste of type j to Exit 1 is a real number satisfying 0\leq p_{i,j}\leq 1.

### Output

The destinations of conveyor belts are represented by the following numbers:

- If the destination is the i-th processor installation location: i 

- If the destination is the i-th sorter installation location: N+i

Let d_i (0\leq d_i\leq N-1) be the waste type of the processor installed at the i-th processor installation location. 
Let s (0\leq s\leq N+M-1) be the number representing the destination of the conveyor belt from the waste inlet. 
Output the following to Standard Output in the specified format:

```text
d_0 \cdots d_{N-1}
s

```

Then output M additional lines.
 
The i-th line should be output according to the state of the i-th sorter installation location:

- If no sorter is installed:

```text
-1

```

- If the k-th (0\leq k\leq K-1) sorter is installed, and the destination of the conveyor belt from Exit 1 is v_1, and from Exit 2 is v_2 (0\leq v_1,v_2\leq N+M-1):

```text
k v_1 v_2

```

If the destination of a conveyor belt is a sorter installation location, a sorter must be installed there.

[Show example](https://img.atcoder.jp/ahc051/jdd9gfQC.html?lang=en&seed=0&output=sample)

### Sample Solution (Python) 
An example solution written in Python is shown below. 
This program performs the following steps:

1. Installs processor of type i at the i-th processor installation location
 
2. Connects the waste inlet to the nearest sorter installation location
 
3. Installs sorter type 0 at that location, connecting Exit 1 to the processor of the waste type with the highest probability, and Exit 2 to the processor of the type with the lowest probability

```text

import sys
import math

input = sys.stdin.readline

# Read input
N, M, K = map(int, input().split())
processor_positions = []
for _ in range(N):
    x, y = map(int, input().split())
    processor_positions.append((x, y))
sorter_positions = []
for _ in range(M):
    x, y = map(int, input().split())
    sorter_positions.append((x, y))
prob = []
for _ in range(K):
    row = list(map(float, input().split()))
    prob.append(row)

# Install processor of type i at position i
proc_assign = ' '.join(str(i) for i in range(N))
# Connect inlet (0,5000) to the nearest sorter installation location
inlet = (0, 5000)
dist_sq = [((x - inlet[0])**2 + (y - inlet[1])**2, i) for i, (x, y) in enumerate(sorter_positions)]
_, nearest_i = min(dist_sq)
inlet_conn = N + nearest_i

# Install sorter type 0, connect Exit 1 to processor with highest prob, Exit 2 to lowest
first_row = prob[0]
imax = first_row.index(max(first_row))
imin = first_row.index(min(first_row))
sorter_assigns = []
for i in range(M):
    if i == nearest_i:
        sorter_assigns.append(f"0 {imax} {imin}")
    else:
        sorter_assigns.append("-1")

print(proc_assign)
print(inlet_conn)
print("\n".join(sorter_assigns))

```

### Input Generation 

- \mathrm{rand}(L,U): Uniformly generates an integer between L and U, inclusive.

N, M, and K are each generated uniformly at random within their respective ranges.

The coordinates for the N processor installation locations and M sorter installation locations are generated using the following procedure to create a total of N+M coordinates, where the first N are used for processor locations and the remaining M for sorter locations.

Initialize the set of used coordinates as S=\{(0,5000)\}.
 
Generate a coordinate (x,y) with x=\mathrm{rand}(0,10^4), y=\mathrm{rand}(0,10^4).
 
If no point within a Euclidean distance of 100 from the generated point is in S, accept the point and add it to S.
 
Repeat this until N+M points have been generated.

Each classification probability p_{i,j} is generated independently as \mathrm{rand}(1000,9000)\times 10^{-4}.

### Tools (Input generator and visualizer) 

- [Web version](https://img.atcoder.jp/ahc051/jdd9gfQC.html?lang=en): This is more powerful than the local version providing animations. 

- [Local version](https://img.atcoder.jp/ahc051/jdd9gfQC.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 

- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc051/jdd9gfQC_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.

### Sample Input 1

```text
13 133 47
1858 8963
2368 1117
2507 4965
7710 890
1468 578
3244 9299
5025 8511
9642 4334
599 4476
1388 2454
4779 5389
8676 3397
6156 8034
4102 6415
8251 3509
9320 4268
1193 1486
4847 2021
4440 9046
8991 8142
6633 5942
3778 8637
6876 74
6067 1863
8407 3044
1604 822
9775 4772
9083 1045
9803 847
7455 5575
3183 1259
8407 2922
371 3058
4890 3331
4014 5211
2054 4998
4121 2752
6559 6840
5346 2079
5773 6895
335 4826
2920 8652
7735 1367
6162 8431
9157 7821
4653 704
6642 7613
8857 6949
3623 8476
7022 6237
4959 8592
2981 3756
2329 7006
2328 3175
249 3861
3051 8548
2673 996
714 7101
9170 2616
70 490
9474 7442
3173 4483
6962 8988
129 8154
1965 3249
9297 1953
6438 9318
5926 6158
1136 6845
2191 7256
475 5531
3383 5500
4499 7348
1062 6942
3835 7850
6357 1687
7532 3576
8723 8738
760 2627
8460 5002
4686 7214
9095 2003
5976 7913
3428 9235
266 2484
527 8521
6142 4813
588 1327
1267 4124
5523 7202
6286 4698
2459 6133
3068 689
5979 5443
3284 133
4996 3507
7965 9183
5634 316
4940 7810
1672 8535
6853 4148
9340 5193
8069 632
1836 7667
2546 2085
4054 4133
3953 2870
842 1426
3115 3095
3310 6422
2990 4161
9395 8903
5047 8714
5566 3742
6731 7534
6722 749
4764 7151
1713 7025
4254 6934
6297 8912
4215 462
3514 8224
398 9070
6611 8531
9596 5432
1411 5057
7552 8981
6596 3135
6479 5998
7299 423
2041 3637
710 6793
2958 189
1252 5867
3588 6591
1057 5697
8112 9569
6711 6792
6763 6384
616 7507
5311 7674
9751 5960
5991 4435
3668 4186
7865 9219
1535 4695
0.7947 0.5717 0.1458 0.7273 0.1395 0.5274 0.7105 0.6630 0.5254 0.1861 0.8898 0.4124 0.8373
0.8285 0.6842 0.7812 0.7602 0.8161 0.6065 0.2316 0.3189 0.1706 0.1754 0.5127 0.1849 0.1398
0.5147 0.6140 0.5256 0.7881 0.4148 0.3610 0.7228 0.5083 0.7442 0.6778 0.7014 0.1979 0.7725
0.3479 0.5942 0.2791 0.6912 0.5923 0.7850 0.8101 0.4307 0.1955 0.8941 0.5841 0.7685 0.3365
0.4116 0.3389 0.7719 0.8792 0.5796 0.5751 0.1132 0.2053 0.5233 0.8496 0.5604 0.7999 0.8949
0.6201 0.3431 0.3317 0.8918 0.7173 0.8363 0.8314 0.7755 0.2087 0.1003 0.3819 0.8538 0.1853
0.5341 0.2075 0.3356 0.1762 0.2904 0.8025 0.5798 0.8951 0.5324 0.6448 0.3785 0.5311 0.8397
0.7767 0.4043 0.7621 0.4336 0.7617 0.3717 0.5117 0.8811 0.3867 0.5574 0.2229 0.4948 0.2109
0.2918 0.1748 0.6747 0.8715 0.2433 0.5604 0.8109 0.6585 0.4850 0.4937 0.7828 0.5056 0.4923
0.2074 0.5636 0.2931 0.7722 0.4598 0.3112 0.2513 0.1173 0.8946 0.2885 0.3746 0.4158 0.1289
0.6646 0.8395 0.1890 0.5334 0.1160 0.7439 0.5963 0.1967 0.3549 0.2631 0.4714 0.3812 0.1210
0.5868 0.4050 0.2678 0.2920 0.6503 0.3704 0.4336 0.5353 0.6986 0.1129 0.2086 0.1183 0.2040
0.4108 0.1430 0.2123 0.4538 0.6206 0.2628 0.1833 0.6750 0.7179 0.4557 0.5837 0.3481 0.8241
0.7121 0.6299 0.6964 0.6125 0.7442 0.4419 0.2720 0.1329 0.2044 0.6554 0.2180 0.7193 0.7337
0.7011 0.1872 0.6595 0.1431 0.4645 0.5205 0.7408 0.1855 0.8220 0.7275 0.2853 0.3204 0.4627
0.5218 0.6348 0.3389 0.8584 0.2942 0.2740 0.4592 0.2506 0.8733 0.8386 0.6548 0.2667 0.8584
0.8958 0.7544 0.4056 0.1245 0.6517 0.8921 0.7763 0.3818 0.7530 0.3751 0.6342 0.7434 0.4215
0.2749 0.3961 0.4172 0.1673 0.4351 0.3963 0.5220 0.2972 0.3792 0.5992 0.7891 0.5944 0.4958
0.5049 0.1172 0.6678 0.4131 0.2468 0.7376 0.6962 0.5892 0.3075 0.4466 0.6821 0.3681 0.3261
0.2087 0.1496 0.2146 0.6869 0.4596 0.7237 0.2283 0.2242 0.7294 0.1675 0.8079 0.6417 0.7165
0.4842 0.3767 0.7664 0.7524 0.5782 0.1098 0.3663 0.5658 0.3647 0.1522 0.6769 0.7871 0.5725
0.4457 0.2750 0.3091 0.7717 0.6955 0.1686 0.5195 0.5703 0.7169 0.4944 0.5972 0.2510 0.3722
0.6483 0.3851 0.3562 0.8226 0.4714 0.5673 0.2170 0.8736 0.8041 0.4816 0.6178 0.1427 0.1894
0.1430 0.2933 0.3126 0.8151 0.7037 0.5676 0.2704 0.3305 0.6220 0.1465 0.2080 0.5824 0.1398
0.2149 0.2998 0.1146 0.8847 0.8714 0.1295 0.8328 0.4172 0.1081 0.3282 0.3044 0.7896 0.5094
0.5783 0.8544 0.7956 0.2967 0.7709 0.1041 0.1021 0.6928 0.5704 0.2814 0.5847 0.5448 0.6967
0.3834 0.5092 0.8551 0.7886 0.1314 0.3960 0.6900 0.4573 0.5070 0.6776 0.6555 0.5269 0.3392
0.1611 0.5018 0.1274 0.1432 0.3938 0.4762 0.5130 0.8322 0.8457 0.8087 0.1703 0.1075 0.1604
0.1997 0.7419 0.1717 0.4030 0.4894 0.3377 0.3162 0.6859 0.1615 0.8698 0.5788 0.8138 0.5818
0.4859 0.3316 0.8085 0.4157 0.8243 0.4599 0.5926 0.5802 0.6918 0.1755 0.3048 0.5425 0.3713
0.5403 0.1570 0.4645 0.7979 0.2681 0.8216 0.2177 0.5360 0.6601 0.7411 0.4305 0.8996 0.8515
0.3653 0.8646 0.2023 0.2481 0.5197 0.8658 0.8288 0.2090 0.3178 0.3384 0.6897 0.4686 0.1087
0.2876 0.4619 0.5801 0.5664 0.6368 0.6487 0.8110 0.6732 0.1831 0.4944 0.6158 0.5406 0.5401
0.4234 0.3891 0.2370 0.7762 0.8843 0.8458 0.6728 0.3381 0.4985 0.2366 0.8303 0.1040 0.5701
0.1297 0.3075 0.6430 0.4957 0.7719 0.6939 0.7531 0.5474 0.5737 0.7540 0.4508 0.1765 0.7806
0.5881 0.7300 0.7944 0.7544 0.1365 0.1780 0.5800 0.5754 0.3439 0.8311 0.4328 0.7358 0.3163
0.2535 0.5585 0.6483 0.1642 0.1291 0.8911 0.7827 0.8863 0.7378 0.4760 0.3481 0.8681 0.7335
0.4697 0.1786 0.3000 0.4476 0.3325 0.7877 0.5377 0.6881 0.5053 0.8013 0.5359 0.6286 0.3323
0.8809 0.5817 0.2729 0.3885 0.2775 0.4435 0.8356 0.2798 0.5751 0.6667 0.4997 0.7838 0.6363
0.6267 0.7182 0.6751 0.7085 0.4753 0.7842 0.8322 0.4385 0.7766 0.1075 0.1791 0.8696 0.1037
0.4336 0.8460 0.5259 0.5350 0.7094 0.1762 0.4207 0.7375 0.4378 0.2340 0.5642 0.7896 0.3129
0.1505 0.5580 0.3774 0.7196 0.7524 0.8902 0.4146 0.1722 0.7837 0.3907 0.2465 0.3367 0.6686
0.7948 0.1337 0.8616 0.4249 0.3155 0.2698 0.1273 0.4942 0.7133 0.2159 0.7039 0.7372 0.4182
0.5986 0.3626 0.1562 0.1946 0.5486 0.1648 0.2525 0.7382 0.8283 0.3628 0.4541 0.2651 0.8978
0.1668 0.4790 0.3921 0.7325 0.3574 0.4265 0.8023 0.8647 0.8215 0.4398 0.2749 0.2215 0.8907
0.4803 0.6133 0.1684 0.1372 0.6250 0.3488 0.5522 0.1836 0.2820 0.2808 0.1747 0.1864 0.7046
0.4214 0.6384 0.4454 0.7190 0.2064 0.4953 0.3946 0.7983 0.7207 0.3334 0.5841 0.6460 0.2462

```

### Sample Output 1

```text
0 1 2 3 4 5 6 7 8 9 10 11 12
40
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
0 10 4
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1
-1

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

- [ルール](https://atcoder.jp/contests/ahc051/rules)
 
- [用語集](https://atcoder.jp/contests/ahc051/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
