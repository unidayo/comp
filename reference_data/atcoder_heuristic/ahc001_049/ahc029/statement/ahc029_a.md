### 問題文

 あなたは会社を経営するゲームをプレイしている。 
 会社が進めている複数の**プロジェクト**に対して、毎ターンごとに手札から**方針カード**を一つ選び消費し、そのカードに従ってプロジェクトを進め、手札の方針カードを補充する、ということを繰り返すターン制のゲームである。 
 全 T ターンが終了した時点で**所持金**を最大化することがこのゲームの目的である。

 プロジェクトには**残務量**と**価値**という整数値のペアが定まっている。残務量が 0 以下になるとプロジェクトは完了し、価値が所持金に追加される。

 あなたはプロジェクトの進め方の方針が書かれた「方針カード」というカードを手札として N 枚持っている。ターンごとに手札の中から1枚の方針カードを選び、その内容にしたがってプロジェクトを進める。 
 方針カードには以下の 5 種類がある。

- **通常労働カード**: このカードには**労働力**という正整数が定まっている。このカードを選ぶと、現在管理しているプロジェクトのうち一つを指定し、そのプロジェクトの残務量を労働力分だけ減らす。 
 
- **全力労働カード**: このカードには**労働力**という正整数が定まっている。このカードを選ぶと、現在管理している全てのプロジェクトについてそれぞれの残務量を労働力分だけ減らす。 
 
- **キャンセルカード**: このカードを選ぶと、現在管理しているプロジェクトのうち一つを指定し、そのプロジェクトの実行を取りやめる。 
 
- **業務転換カード**: このカードを選ぶと、現在管理しているプロジェクト全てを取りやめる。 
 
- **増資カード**: このカードを選ぶと、今後出現するプロジェクトの残務量と価値 及び 今後出現する方針カードの労働力とコスト(後述)が 2 倍になる。すでに出現しているプロジェクトと手札にある方針カードの数値は変更されない。**このカードはテストケース内で 20 回までしか使用できない。**

#### ゲームの進め方

 ゲーム開始時点で、会社が管理している M 個のプロジェクトの情報と、あなたが持っている N 枚の方針カードの情報が与えられる。また、開始時点の所持金は 0 である。 
 各ターンは次の流れで進んでいく。

- 手札から方針カードを1枚選択し実行する。選択したカードは手札から消える。

- プロジェクトが完了した場合、プロジェクトの価値の分だけ所持金が増加し、そのプロジェクトは消える。 
 
- プロジェクトの実行を取りやめた場合、そのプロジェクトは消える。

- 存在するプロジェクトの数が M 個を下回った場合、 プロジェクトの数が M 個になるように新たなプロジェクトが与えられる。 
 
-  K 枚の方針カードおよびそれらのコストが提示される。あなたはその中から 1 枚を選び手札に加える。この際に選んだカードのコストの分だけ所持金が減る。

- コストの分だけ所持金が減ることで所持金が負になるような方針カードを選択することはできない。後述するが、K 枚のカード中にコスト 0 のカードが存在するため、必ずカードを選択できることが保証されている。

 以上の手順から分かる通り、ターン開始時には必ず手札に N 枚の方針カードがあり、会社は M 個のプロジェクトを管理している。

 このゲームを上手にプレイして、T ターン後の時点の所持金をできるだけ多くしてほしい。

### 順位付け

 相対評価システムを採用します。
 
 各テストケース毎に、  T  ターン後の時点での所持金の値を絶対スコアとして \mathrm{round} \lparen 10^9 \times \frac{自身の絶対スコア}{全参加者中の最大絶対スコア} \rparen の **相対評価スコア** が得られ、その和が提出の得点となります。

 最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定されます。暫定テスト・システムテストのどちらにおいても、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースに関する相対評価スコアは0点となり、そのテストケースにおいては「全参加者中の最大絶対スコア」の計算対象から除外されます。

 また、システムテストは **CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意してください。

#### テストケース数

-  
 暫定テスト: 50個

-  
 システムテスト: 3000個

- コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc029/seeds.txt) (sha256=`aa53f4b2ae15494c08f8304459afd7523eb10b704394ad1c6a0a92fa052bbe14`) を公開します。

#### 相対評価システムについて

 暫定テスト・システムテストともに、CE以外の結果を得た一番最後の提出のみが順位表に反映されます。
 
 相対評価スコアの計算に用いられる各テストケース毎の「全参加者中の最大絶対スコア」の算出には、各参加者の順位表に反映されている提出のみが用いられます。
 
 順位表に表示されるスコアは相対評価スコアであり、新規提出があるたびに相対評価スコアが再計算されます。 一方、提出一覧から確認できる各提出のスコアは、各テストケース毎の絶対スコアをそのまま足し合わせたものであり、相対評価スコアは表示されません。 最新以外の提出の、現在の順位表における相対評価スコアを知るためには、再提出が必要となります。
 
 不正な出力や制限時間超過をした場合、提出一覧から確認できるスコアは0となりますが、順位表には正解したテストケースに対する相対評価スコアの和が表示されます。

#### 実行時間について

 実行時間には多少のブレが生じます。 
 また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
 そのため、実行時間制限ギリギリの提出がシステムテストで TLE となる可能性があります。 
 プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入出力

**※この問題はインタラクティブ形式である。以下の内容を読み、ジャッジプログラムとのやり取りを行うこと。**

最初に問題のパラメタと初期状態が以下の形式で標準入力から与えられる。

```text
\(
N ~ M ~ K ~ T \\
t_0 ~ w_0 \\
\vdots \\
t_{N-1} ~ w_{N-1} \\
h_0 ~ v_0 \\
\vdots \\
h_{M-1} ~ v_{M-1} \)

```

- 1行目には4つの整数 N,M,K,T がスペース区切りで与えられる。

- N は手札に持つ方針カードの枚数で、 2 \le N \le 7 を満たす。 
 
- M は会社が管理するプロジェクトの数で、 2 \le M \le 8 を満たす。 
 
- K は手札に補充する方針カードの提示枚数で、 2 \le K \le 5 を満たす。 
 
- T はゲームのターン数で、 T = 1000 を満たす。

- 続く N 行には開始時点の手札の方針カードの情報が与えられる。

- 各行には 2 つの整数 t_i, w_i がスペース区切りで含まれており、i 番目の方針カードの種類と労働力を表す。

-  t_i = 0 のとき そのカードが労働力 w_i の通常労働カードであることを表す。  
 
-  t_i = 1 のとき そのカードが労働力 w_i の全力労働カードであることを表す。  
 
-  t_i = 2 のとき そのカードがキャンセルカードであることを表す。  
 
-  t_i = 3 のとき そのカードが業務転換カードであることを表す。  
 
-  t_i = 4 のとき そのカードが増資カードであることを表す。

- t_i, w_i (0 \leq i \lt N) の値の範囲は「入力生成方法」セクションを参照すること。

- 続く M 行には開始時点のプロジェクトの情報が与えられる。

- 各行には 2 つの整数 h_i, v_i がスペース区切りで含まれており、i 番目のプロジェクトの残務量と価値を表す。 
 
- h_i, v_i (0 \leq i \lt M) の値の範囲は「入力生成方法」セクションを参照すること。

上記の入力を受け取った後は T ターンに渡りゲームを進めていく。ターンごとに以下の形式でやりとりを行うこと。

 まず手札から使用する方針カードを1枚選び、そのカードの情報を以下の形式で標準出力へ出力せよ。

```text
\(
c ~ m\)

```

-  1 行に 2 つの整数 c,m を出力せよ。

- c は手札のうち何番目の方針カードを使用するかを表す。 
 
- m はカードの効果を適用するプロジェクトの番号を表す。 
 
-  0 \leq c \lt N, 0 \leq m \lt M を満たす必要がある。 
 
- 選んだカードが全力労働カード, 業務転換カード, 増資カードのいずれかの場合は  m = 0  を満たす必要がある。

- テストケース内で増資カードは 20 回までしか使用できない。 
 
- 制約を満たさない出力をした場合、 WA となる。 
 
- 出力の後には改行をし、標準出力をflushする必要がある。そうしなかった場合、 TLE となる可能性がある。

 続いて、カード使用後のプロジェクトの状況の情報を標準入力から受け取る。

```text
\(          
h_0 ~ v_0 \\
\vdots \\
h_{M-1} ~ v_{M-1} \)

```

- 入力は M 行で、各行には直前の方針カードを使用した直後のプロジェクトの情報が、最初の入力と同じ形式で与えられる。 
 
- 完了したり取りやめたりしたプロジェクトがあった場合は、そのプロジェクトと同じ位置に新たに生成されたプロジェクトの情報が与えられる。

 続いて、現在の所持金の情報を標準入力から受け取る。

```text
\(
money\)

```

- 1 つの整数 money が与えられる。これは現時点での所持金を表す。

 続いて、手札に補充する方針カードの候補の情報を標準入力から受け取る。

```text
\(
t_0 ~ w_0 ~ p_0 \\
\vdots \\
t_{K-1} ~ w_{K-1} ~ p_{K-1} \)

```

- 入力は K 行で、各行には3つの整数が空白区切りで含まれている。 
 
-  t_i, w_i  は  i  番目の方針カード候補の情報を最初の入力と同じ形式で表したものである。 
 
-  p_i は i  番目の方針カードのコストを表している。 
 
- **t_0, w_0, p_0 は t_0 = 0, w_0 = 2^L, p_0 = 0 を満たす。**ただし L はその時点で増資カードを使用した回数( 0 \leq L \leq 20 )を表す整数とする。 
 
- t_i, w_i, p_i (1 \leq i \lt K) の値の範囲は「入力生成方法」セクションを参照すること。

 最後に、K 枚の方針カード候補のうち何番目のカードを手札に加えるかの情報を標準出力へ出力せよ。

```text
\(
r\)

```

-  1 行に整数 r を出力せよ。

- 方針カード候補から r 番目の方針カードを選ぶ。 
 
- r は 0 \le r \lt K を満たす必要がある。 
 
- 所持金が負になるカードを選択することはできない。

- ここで選んだカードはこのターンの最初に使用したカードと同じ位置、つまり手札の c 番目に補充される。 
 
- 制約を満たさない出力をした場合、 WA となる。 
 
- 出力の後には改行をし、標準出力をflushする必要がある。そうしなかった場合、 TLE となる可能性がある。

 以上で1ターンが終了する。

### 入力生成方法

- l \leq r を満たす整数 l, r に対して、l 以上 r 以下の整数を一様ランダムに生成する関数を \mathrm{rand}(l, r) とする。 
 
- l \leq r を満たす実数 l, r に対して、l 以上 r 以下の実数を一様ランダムに生成する関数を \mathrm{rand\_double}(l, r) とする。 
 
- 平均 μ、標準偏差 σ の正規分布から値を生成する関数を \mathrm{gauss}(μ, σ)とする。 
 
- l \leq r を満たす実数 l, r 及び 実数 x に対して、 \mathrm{max}(l, \mathrm{min}(r, x))  を求める関数を \mathrm{clamp}(x, l, r) とする。

#### N, M, Kの生成

- N = \mathrm{rand}(2, 7) で生成する。 
 
- M = \mathrm{rand}(2, 8) で生成する。 
 
- K = \mathrm{rand}(2, 5) で生成する。

#### プロジェクトの生成

- L をその時点で増資カードを使用した回数( 0 \leq L \leq 20 )を表す整数とする。 
 
- b = \mathrm{rand\_double}(2.0, 8.0) とする。 
 
- 残務量 h を h = \mathrm{round}(2 ^ b) * 2^L で生成する。 
 
- 価値 v を v = \mathrm{round}(2 ^ {\mathrm{clamp}(\mathrm{gauss}(b, 0.5), 0.0, 10.0)}) * 2^L で生成する。

#### 方針カードの生成

- テストケースごとに、カードの種類の生成確率をコントロールする隠しパラメータ x_0, x_1, x_2, x_3, x_4 を以下のように定める。 
 
- x_0=\mathrm{rand}(1,20), x_1=\mathrm{rand}(1,10), x_2=\mathrm{rand}(1,10), x_3=\mathrm{rand}(1,5), x_4=\mathrm{rand}(1,3) とする。 
 
- カードの種類 t は、 t = i (0 \leq i \leq 4) となる確率が x_i に比例するように生成する。 
 
- L をその時点で増資カードを使用した回数( 0 \leq L \leq 20 )を表す整数とする。 
 
- t=0 または t = 1 の場合は、労働力 w を w=\mathrm{rand}(1, 50)* 2^L で生成する。t が 0,1 以外の場合は w=0 とする。

- カードのコスト p は以下のように決める。

- t = 0 の場合 w' = w/2^L とし p = \mathrm{clamp}(\mathrm{round}(\mathrm{gauss}(w', w'/3)), 1, 10000) * 2^L で生成する。 
 
- t = 1 の場合 w' = w/2^L とし p = \mathrm{clamp}(\mathrm{round}(\mathrm{gauss}(w'*M, w'*M/3)), 1, 10000) * 2^L で生成する。 
 
- t = 2 の場合 p = \mathrm{rand}(0, 10) * 2^L で生成する。 
 
- t = 3 の場合 p = \mathrm{rand}(0, 10) * 2^L で生成する。 
 
- t = 4 の場合 p = \mathrm{rand}(200, 1000) * 2^L で生成する。

### ツール

- [Web版ビジュアライザ・入力ジェネレータ](https://img.atcoder.jp/ahc029/45e6da0b06.html): アニメーション表示が可能です。 
 
- [ローカル版テスタ・ビジュアライザ・入力ジェネレータ](https://img.atcoder.jp/ahc029/45e6da0b06.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 
 
- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc029/45e6da0b06_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

- [サンプルコード（C++, Python）](https://img.atcoder.jp/ahc029/45e6da0b06_sample.zip): サンプルの解答です。次の処理を実装しています。

- 方針カードを使用する際は、常に 0 番目のカードを使用する。そのカードが通常労働カードまたはキャンセルカードの場合、 0 番目のプロジェクトを対象とする。 
 
- 候補となる方針カードからどれを手札に加えるかの選択では、常に 0 番目の方針カードを選択する。

- 0 番目の方針カードのコストは  0  であることが保証されているため、現在の所持金に関わらずこの選択は常に可能である。

 **コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。**

#### ツールで用いられる入出力ファイルの仕様

ローカルテスタに与える入力ファイルは以下の形式を用いています。

```text
\(
N ~ M ~ K ~ T \\
h_{init,0} ~ v_{init,0} \\
\vdots \\
h_{init,M-1} ~ v_{init,M-1} \\
h_{0} ~ v_{0} \\
\vdots \\
h_{T*M-1} ~ v_{T*M-1} \\
t_{init,0} ~ w_{init,0} \\
\vdots \\
t_{init,N-1} ~ w_{init,N-1} \\
t_{0,0} ~ w_{0,0} ~ p_{0,0} \\
\vdots \\
t_{0,K-1} ~ w_{0,K-1} ~ p_{0,K-1} \\
t_{1,0} ~ w_{1,0} ~ p_{1,0} \\
\vdots \\
t_{T-1,K-1} ~ w_{T-1,K-1} ~ p_{T-1,K-1} \)

```

-  h_{init,i}, v_{init,i}  は、はじめに与えられるプロジェクトのうち  i  番目のものの残務量と価値を表します。 
 
-  h_{i}, v_{i}  は、新たに発生するプロジェクトの残務量と価値を表し、先頭から順に使用されます。新しいプロジェクトを生成する際は、それまでに増資カードを使用した回数を  L  として  2^L  倍された値が使われます。 
 
-  t_{init,i}, w_{init,i}  は、はじめに与えられる方針カードのうち  i  番目のものの種類と労働力を表します。 
 
-  t_{i,j}, w_{i,j}, p_{i,j}  は、  i  ターン目で候補として与えられる  j  番目の方針カードの種類、労働力、コストを表します。方針カードを生成する際は、それまでに増資カードを使用した回数を  L  として  2^L  倍された値が使われます。

ローカルテスタは解答プログラムの出力をそのまま出力ファイルに書き出します。
 
 解答プログラムは、`#` から始まるコメント行を出力に含めてもかまいません。 
 Web版ビジュアライザを使用すると、コメント行を対応するタイミングで表示できるため、デバッグや考察等に役立てることができます。 
 ジャッジプログラムはコメント行を全て無視するため、コメント行を出力するプログラムをそのまま提出可能です。

### 入出力例

  | Input 
  | Output 
  | 説明

  |

```text

3 2 4 3
0 29
3 0
4 0
38 113
17 46

```

  |  
  |  
 入力の制約を満たさない、説明用の入力である点に注意せよ。
 
 初期状態の手札の 3 枚の方針カードの情報と、2 つのプロジェクトの情報を受け取る。

  |  
  |

```text

0 1

```

  |  
 ここから最初のターンが始まる。
 
 手札の 0 番目にあった通常労働カードを 1 番目のプロジェクトに適用する。
 
 このプロジェクトの残務量は 17-29=-12 と負になるため、プロジェクトは完了する。
 
 プロジェクトが完了したため所持金は 46 に増加する。

  |

```text

38 113
51 82

```

  |

  |  
 現在のプロジェクトの情報を受け取る。
 
 直前の行動で 1 番目のプロジェクトが完了したため、1 番目の位置には新しいプロジェクトが出現している。

  |

```text

46

```

  |   
  |  
 現在の所持金の情報を受け取る。

  |

```text

0 1 0
3 0 8
1 18 22
4 0 685

```

  |

  |  
 4 枚の方針カードとコストが提示される。なお、3 番目のカードはコストが所持金を上回るため選択することはできない。

  |  
  
  |

```text

2

```

  |  
 2 番目の全力労働カードを選択した。このカードは手札の 0 番目の位置に置かれる。現時点での手札は以下のようになっている。

```text

1 18
3 0
4 0

```
 
 所持金は  46-22=24  に減少する。
 
 以上でターンは終了となる。

  |  
  |

```text

1 0

```

  |  
 ここから次のターンが始まる。
 
 手札の 1 番目にあった業務転換カードを使用する。
 
 全てのプロジェクトは取りやめられ、所持金は変化しない。

  |

```text

49 84
140 127

```

  |

  |  
 現在のプロジェクトの情報を受け取る。
 
 実行を取りやめたプロジェクトに代わり、新たなプロジェクトの情報が与えられる。

  |

```text

24

```

  |   
  |  
 現在の所持金の情報を受け取る。

  |

```text

0 1 0
2 0 2
0 25 44
1 43 130

```

  |

  |  
 4 枚の方針カードとコストが提示される。

  |  
  
  |

```text

1

```

  |  
 1 番目のキャンセルカードを選択した。このカードは手札の 1 番目の位置に置かれる。現時点での手札は以下のようになっている。

```text

1 18
2 0
4 0

```
 
 所持金は  24-2=22  に減少する。
 
 以上でターンは終了となる。

  |  
  |

```text

2 0

```

  |  
 ここから最後のターンが始まる。
 
 手札の 2 番目にあった増資カードを使用する。
 
 すでに出現している方針カードやプロジェクトには影響はなく、所持金は増加しない。

  |

```text

49 84
140 127

```

  |

  |  
 現在のプロジェクトの情報を受け取る。

  |

```text

22

```

  |   
  |  
 現在の所持金の情報を受け取る。

  |

```text

0 2 0
3 0 32
2 0 18
1 84 212

```

  |

  |  
 4 枚の方針カードとコストが提示される。
 
 増資カードを使ったため、提示された方針カードの労働力やコストの生成に用いるパラメータが変わっている。

  |  
  
  |

```text

0

```

  |  
 0 番目の通常労働カードを選択した。このカードは手札の 2 番目の位置に置かれる。現時点での手札は以下のようになっている。

```text

1 18
2 0
0 2

```
 
 所持金は  22-0=22  となる。
 
 以上でターンは終了となる。
 
 全ターン終了時の所持金 22 が本テストケースのスコアとなる。

### Problem Statement

 You are playing a business management game where the company is working on multiple **projects**. 
 In this game, you choose and consume a **policy card** from your hand each turn to advance the projects, and then refill your hand with a policy card. 
 The objective of this game is to maximize your **money** at the end of T turns.

 Each project is described by a pair of integer values: **remaining work** and **value**. When the remaining work becomes 0 or less, the project is completed, and the value is added to your money.

 You have N policy cards in your hand, each containing a strategy for managing projects. Each turn, you select one policy card from your hand and manage the project according to its contents. 
 There are five types of policy cards:

- **Regular Work card**: This card is associated with a positive integer representing **labor power**. When you choose this card, you specify one of the projects you are currently managing, and reduce the remaining work of that project by the amount of labor power. 
 
- **Hard Work card**: This card is associated with a positive integer representing **labor power**. When you choose this card, the remaining work of each of the projects you are currently managing is reduced by the amount of labor power. 
 
- **Cancel card**: When you choose this card, you specify one of the projects you are currently managing and abort it. 
 
- **Restructuring card**: When you choose this card, you abort all the projects you are currently managing. 
 
- **Investment card**: Choosing this card doubles the remaining work and value of future projects, as well as the labor power and cost(described below) of future policy cards. The projects that have already appeared and the policy cards in your hand will not be affected. 
 **This card can only be used up to 20 times within a single test case.**

#### How the game is played

 At the start of the game, you are given information about the M projects the company is managing and the N policy cards you have. Your initial money is 0. 
 Each turn progresses as follows:

- Choose and execute one policy card from your hand. The selected card is removed from your hand.

- If a project is completed, your money increases by the value of the project, and the project disappears. 
 
- If a project is aborted, the project disappears.

- If the number of existing projects falls below M, new projects are provided to make the total number of projects equal to M. 
 
- You are presented with K policy cards and their respective costs. You choose one card from them to add to your hand. When you do so, your money decreases by the cost of the chosen card.

- You cannot choose a policy card that would result in your money becoming negative due to the cost. However, it is guaranteed that you can always choose a card from the set of K cards, as there exists a card with a cost of 0 among them, as will be explained later.

 As can be inferred from the above steps, at the beginning of each turn, you will always have N policy cards in your hand, and the company will be managing M projects.

 Play the game skillfully and maximize your money at the end of T turns.

### Scoring

For each test case, the absolute score is obtained as the value of the money at the end of T turns, and we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{YOUR}}{\mathrm{MAX}}), where YOUR is your absolute score and MAX is the maximum absolute score among all competitors obtained on that test case. The score of the submission is the sum of the relative scores.

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
 In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MAX calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
 Be careful not to make a mistake in the final submission.

#### Number of test cases

-  
 Provisional test: 50

-  
 System test: 3000

- We will publish [seeds.txt](https://img.atcoder.jp/ahc029/seeds.txt) (sha256=`aa53f4b2ae15494c08f8304459afd7523eb10b704394ad1c6a0a92fa052bbe14`) after the contest is over.

#### About Relative Evaluation System

 In both the provisional/system test, the standings will be calculated using only the last submission which received a result other than CE. 
 Only the last submissions are used to calculate the MAX for each test case when calculating the relative scores.

 The scores shown in the standings are relative, and whenever a new submission arrives, all relative scores are recalculated. 
 On the other hand, the score for each submission shown on the submissions page is the sum of the absolute score for each test case, and the relative scores are not shown. 
 In order to know the relative score of submission other than the latest one in the current standings, you need to resubmit it. 
 If your submission produces illegal output or exceeds the time limit for some test cases, the score shown on the submissions page will be 0, but the standings show the sum of the relative scores for the test cases that were answered correctly.

#### About execution time

 Execution time may vary slightly from run to run. 
 In addition, since system tests simultaneously perform a large number of executions, it has been observed that execution time increases by several percent compared to provisional tests. 
 For these reasons, submissions that are very close to the time limit may result in TLE in the system test. 
 Please measure the execution time in your program to terminate the process, or make sure to have enough time margin for the execution.

### Input and Output

 **This problem is interactive. Read the following explanation, and communicate with the judge program accordingly.**

At first, problem parameters and the initial state will be provided from Standard Input in the following format.

```text
\(
N ~ M ~ K ~ T \\
t_0 ~ w_0 \\
\vdots \\
t_{N-1} ~ w_{N-1} \\
h_0 ~ v_0 \\
\vdots \\
h_{M-1} ~ v_{M-1} \)

```

- On the first line, there are four integers N,M,K,T separated by spaces.

- N represents the number of policy cards in your hand and satisfies 2 \le N \le 7. 
 
- M represents the number of projects managed by the company and satisfies 2 \le M \le 8. 
 
- K represents the number of policy card candidates presented for refilling the hand and satisfies 2 \le K \le 5. 
 
- T represents the number of turns and satisfies T = 1000.

- The following N lines provide information about the policy cards in the starting hand.

- Each line contains space-separated two integers t_i and w_i, representing the type and labor power of the i-th policy card.

- When t_i = 0, it indicates that the card is a Regular Work card with labor power w_i. 
 
- When t_i = 1, it indicates that the card is a Hard Work card with labor power w_i. 
 
- When t_i = 2, it indicates that the card is a Cancel card. 
 
- When t_i = 3, it indicates that the card is a Restructuring card. 
 
- When t_i = 4, it indicates that the card is an Investment card.

- The ranges of values for t_i, w_i (0 \leq i \lt N) are described in the "Input Generation" section.

- The following M lines provide information about the projects at the start of the game.

- Each line contains space-separated two integers h_i and v_i, representing the remaining work and value of the i-th project. 
 
- The ranges of values for h_i, v_i (0 \leq i \lt M) are described in the "Input Generation" section.

After receiving the above input, the game will progress over T turns. Communicate with the judge program in the following format each turn.

 First, choose one policy card to use from your hand, and output the information of that card to the Standard Output in the following format.

```text
\(
c ~ m\)

```

- On a single line, output two integers c,m separated by a space.

- c represents the position of the policy card to be used from the hand. 
 
- m represents the position of the project on which to apply the effect of the card. 
 
-  0 \leq c \lt N, 0 \leq m \lt M must be satisfied. 
 
- If the chosen card is a Hard Work card, Restructuring card, or Investment card, m must be 0.

- Investment card can only be used up to 20 times within a single test case. 
 
- If the output does not satisfy the constraints, the result will be WA. 
 
- The output must be followed by a new line, and you have to flush Standard Output. Otherwise, the submission might be judged as TLE

 Next, receive the information about the status of the projects after using the card from the Standard Input.

```text
\(          
h_0 ~ v_0 \\
\vdots \\
h_{M-1} ~ v_{M-1} \)

```

- The input consists of M lines, with each line providing the information about the projects immediately after using the previous policy card, in the same format as the initial input. 
 
- If there are completed or aborted projects, the information about the newly generated projects will be provided at the same position as those projects.

 Next, receive the information about the current money from the Standard Input.

```text
\(
money\)

```

- A single integer money is provided, representing the current money at this point.

 Next, receive the information about the candidate policy cards to refill the hand from the Standard Input.

```text
\(
t_0 ~ w_0 ~ p_0 \\
\vdots \\
t_{K-1} ~ w_{K-1} ~ p_{K-1} \)

```

- The input consists of K lines, each containing three integers separated by spaces 
 
- t_i, w_i represent the information of the i-th candidate policy card in the same format as the initial input. 
 
- p_i represents the cost of the i-th policy card. 
 
- **t_0, w_0, p_0 satisfy t_0 = 0, w_0 = 2^L, p_0 = 0.** Here, let L ( 0 \leq L \leq 20 ) be an integer representing the number of times the Investment card has been used at this point. 
 
- The ranges of values for t_i, w_i, p_i (1 \leq i \lt K) are described in the "Input Generation" section.

 Finally, output to the standard output the information about which of the K candidate policy cards to add to the hand.

```text
\(
r\)

```

- On a single line, output an integer r.

- This indicates selecting the r-th policy card from the candidate policy cards. 
 
- 0 \le r \lt K must be satisfied. 
 
- You cannot choose a card that would result in a negative money.

- The card chosen here will refill the hand in the same position as the card used at the beginning of this turn, which is the c-th position in the hand. 
 
- If the output does not satisfy the constraints, the result will be WA. 
 
- The output must be followed by a new line, and you have to flush Standard Output. Otherwise, the submission might be judged as TLE

 This concludes one turn.

### Input Generation

- For integers l and r satisfying l \leq r, let \mathrm{rand}(l, r) be a function that generates a uniformly random integer between l and r, inclusive. 
 
- For real numbers l and r satisfying l \leq r, let \mathrm{rand\_double}(l, r) be a function that generates a uniformly random real number between l and r, inclusive. 
 
- Let \mathrm{gauss}(μ, σ) be a function that generates values from a normal distribution with mean μ and standard deviation σ. 
 
- For real numbers l and r satisfying l \leq r, and a real number x, let \mathrm{clamp}(x, l, r) be a function that calculates  \mathrm{max}(l, \mathrm{min}(r, x)) .

#### Generation of N, M, K

- N = \mathrm{rand}(2, 7) 
 
- M = \mathrm{rand}(2, 8) 
 
- K = \mathrm{rand}(2, 5)

#### Generation of projects

- Let L ( 0 \leq L \leq 20 ) be an integer representing the number of times the Investment card has been used at this point. 
 
- Generate a parameter b = \mathrm{rand\_double}(2.0, 8.0). 
 
- Remaining work h is generated as h = \mathrm{round}(2 ^ b) * 2^L. 
 
- Value v is generated as v = \mathrm{round}(2 ^ {\mathrm{clamp}(\mathrm{gauss}(b, 0.5), 0.0, 10.0)}) * 2^L.

#### Generation of policy cards

- For each test case, the hidden parameters x_0, x_1, x_2, x_3, x_4 are defined to control the generation probabilities of the card types as follows: 
 
- x_0=\mathrm{rand}(1,20), x_1=\mathrm{rand}(1,10), x_2=\mathrm{rand}(1,10), x_3=\mathrm{rand}(1,5), x_4=\mathrm{rand}(1,3) 
 
- The probability of the card type t being i is proportional to x_i (0 \leq i \leq 4). 
 
- Let L ( 0 \leq L \leq 20 ) be an integer representing the number of times the Investment card has been used at this point. 
 
- If t=0 or t = 1, labor power w is generated as w=\mathrm{rand}(1, 50)* 2^L. For t other than 0,1, w is set to 0.

- The cost p of the policy card is determined as follows:

- When t = 0, let w' = w/2^L and p is generated as p = \mathrm{clamp}(\mathrm{round}(\mathrm{gauss}(w', w'/3)), 1, 10000) * 2^L . 
 
- When t = 1, let w' = w/2^L and p is generated as p = \mathrm{clamp}(\mathrm{round}(\mathrm{gauss}(w'*M, w'*M/3)), 1, 10000) * 2^L . 
 
- When t = 2, p is generated as p = \mathrm{rand}(0, 10) * 2^L . 
 
- When t = 3, p is generated as p = \mathrm{rand}(0, 10) * 2^L . 
 
- When t = 4, p is generated as p = \mathrm{rand}(200, 1000) * 2^L .

### Tools (Input generator, local tester and visualizer)

- [Web version](https://img.atcoder.jp/ahc029/45e6da0b06.html?lang=en): For displaying animated visualizations. 
 
- [Local version](https://img.atcoder.jp/ahc029/45e6da0b06.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org). 
 
- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc029/45e6da0b06_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

- [Sample code (C++, Python)](https://img.atcoder.jp/ahc029/45e6da0b06_sample.zip): Sample answers. Implemented as follows:

- When using a policy card, always use the 0-th card. If the card is a Regular Work card or a Cancel card, it targets the 0-th project. 
 
- When selecting which policy card to add to the hand from the candidate policy cards, always choose the 0-th policy card.

- The cost of the 0-th candidate policy card is guaranteed to be 0, making this choice always possible regardless of the current money.

 **Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.**

#### Specification of input/output files used by the tools

Input files given to the local tester have the following format.

```text
\(
N ~ M ~ K ~ T \\
h_{init,0} ~ v_{init,0} \\
\vdots \\
h_{init,M-1} ~ v_{init,M-1} \\
h_{0} ~ v_{0} \\
\vdots \\
h_{T*M-1} ~ v_{T*M-1} \\
t_{init,0} ~ w_{init,0} \\
\vdots \\
t_{init,N-1} ~ w_{init,N-1} \\
t_{0,0} ~ w_{0,0} ~ p_{0,0} \\
\vdots \\
t_{0,K-1} ~ w_{0,K-1} ~ p_{0,K-1} \\
t_{1,0} ~ w_{1,0} ~ p_{1,0} \\
\vdots \\
t_{T-1,K-1} ~ w_{T-1,K-1} ~ p_{T-1,K-1} \)

```

-  h_{init,i}, v_{init,i}  represent the remaining work and value of the i-th project given at the beginning. 
 
-  h_{i}, v_{i}  represent the remaining work and value of the newly generated projects, and they are used sequentially from the beginning. When generating new projects, these values are multiplied by 2^L, where L is the number of times the Investment card has been used. 
 
-  t_{init,i}, w_{init,i}  represent the type and labor power of the i-th policy card given at the beginning 
 
-  t_{i,j}, w_{i,j}, p_{i,j}  represent the type, labor power, and cost of the j-th candidate policy card given in the i-th turn. When generating new policy card, these values are multiplied by 2^L, where L is the number of times the Investment card has been used.

 The local tester writes outputs from your program directly to the output file. 
 Your program may output comment lines starting with `#`. The web version of the visualizer displays the comment lines with the corresponding turn, which may be useful for debugging and analysis. 
 Since the judge program ignores all comment lines, you can submit a program that outputs comment lines as is.

### I/O Examples

  | Input 
  | Output 
  | Explanation

  |

```text

3 2 4 3
0 29
3 0
4 0
38 113
17 46

```

  |  
  |  
 Please note that this input is for explanation purposes, and thus doesn't meet all of the constraints for input.
 
 Information about the three policy cards in the initial hand and the information about two initial projects are provided.

  |  
  |

```text

0 1

```

  |  
 The first turn begins from here.
 
 Applying the Regular Work card that was in position 0 of the hand to the project in position 1.
 
 The remaining work of this project becomes -12 (17-29=-12), resulting in the project being completed.
 
 As the project is completed, the money increases to 46.

  |

```text

38 113
51 82

```

  |

  |  
 Receive the information about the current projects.
 
 As a result of the last action, the project in position 1 has been completed, and a new project has appeared at that position.

  |

```text

46

```

  |   
  |  
 Receive the information about the current money.

  |

```text

0 1 0
3 0 8
1 18 22
4 0 685

```

  |

  |  
 Four candidate policy cards and their costs are presented. The cost of the card at position 3 exceeds the money, so it cannot be selected.

  |  
  
  |

```text

2

```

  |  
 Chose the Hard Work card at position 2. This card will be placed in position 0 of the hand. The current hand are as follows:

```text

1 18
3 0
4 0

```
 
 Your money decreased to 24 (46-22=24).
 
 This concludes the turn.

  |  
  |

```text

1 0

```

  |  
 The next turn begins from here.
 
 Using the Restructuring card that was in position 1 of the hand.
 
 All projects are aborted, and the money remains unchanged.

  |

```text

49 84
140 127

```

  |

  |  
 Receive the information about the current projects.
 
 In place of the aborted projects, information about the new projects is provided.

  |

```text

24

```

  |   
  |  
 Receive the information about the current money.

  |

```text

0 1 0
2 0 2
0 25 44
1 43 130

```

  |

  |  
 Four candidate policy cards and their costs are presented.

  |  
  
  |

```text

1

```

  |  
 Chose the Cancel card at position 1. This card will be placed in position 1 of the hand. The current hand are as follows:

```text

1 18
2 0
4 0

```
 
 Your money decreased to 22 (24-2=22).
 
 This concludes the turn.

  |  
  |

```text

2 0

```

  |  
 The last turn begins from here.
 
 Using the Investment card that was in position 2 of the hand.
 
 There is no impact on the already existing policy cards and projects, and the possession money does not increase.

  |

```text

49 84
140 127

```

  |

  |  
 Receive the information about the current projects.

  |

```text

22

```

  |   
  |  
 Receive the information about the current money.

  |

```text

0 2 0
3 0 32
2 0 18
1 84 212

```

  |

  |  
 Four candidate policy cards and their costs are presented.
 
 Due to the use of the Investment card, the parameters used for generating the labor power and cost of the presented policy cards have changed.

  |  
  
  |

```text

0

```

  |  
 Chose the Normal Work card at position 0. This card will be placed in position 2 of the hand. The current hand are as follows:

```text

1 18
2 0
0 2

```
 
 Your current money is 22.
 
 This concludes the turn.
 
 The money at the end of all turns, which is 22, will be the score for this test case.

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

- [ルール](https://atcoder.jp/contests/ahc029/rules)
 
- [用語集](https://atcoder.jp/contests/ahc029/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
