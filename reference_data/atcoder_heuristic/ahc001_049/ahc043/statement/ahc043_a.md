###  ストーリー

Nihonbashi Simulator は架空の国Rの鉄道会社Xを経営するターン制のシミュレーションゲームである。あなたはこのゲームの大ファンであり、高い収益を上げる会社にすることを目標としている。線路と駅を適切に建設することで、R国に住む人の通勤を手助けしつつ、Xを最高の鉄道会社に育て上げよう!

### 問題文

 R国は N 行 N 列のグリッド状に分けられた区画からなる。上から i 行目 (0 \le i < N)、左から j 列目 (0 \le j < N) の区画を (i,j) とする。各区画は更地、線路、駅のいずれかである:

- 更地

- 他の区画と接続しない。 
 
- コストを払うことで、駅または線路を建設することができる。

- 線路

- 上下左右の更地でない区画のうち、最大2つの区画と接続する。線路は接続先に応じて6種類あり、以下の図のように番号付けられる。 
 
- コストを払うことで、駅に置き換えることができる。

- 駅

- 上下左右の最大4つの更地でない区画と接続する。

 1

 2

 3

 4

 5

 6

 線路の種類

 R国には M 人の人が住んでいる。人 c の家は区画 (i_{c,s}, j_{c,s})、職場は区画 (i_{c,t}, j_{c,t}) にあり、この間を通勤している。

ゲーム開始時、鉄道会社Xは K の資金を持っており、全ての区画は更地である。ここから T ターンの間ゲームを行う。

### ゲームのすすめ方

各ターンでは、建設フェーズと集金フェーズがこの順で実行される。

#### 建設フェーズ

建設フェーズでは、線路の配置・駅の配置・待機のうち1つの行動を行う。ただし、資金が0未満になるような行動はできない。資金が0未満かどうかの判定は集金フェーズの前に行われる。

- 線路の配置: 更地の区画を1つ選び、その区画を6種類の線路のいずれか1つに変更する。資金が100減る。 
 
- 駅の配置: 更地または線路の区画を1つ選び、その区画を駅に変更する。資金が5000減る。 
 
- 待機: 区画の状態と資金は変化しない。

#### 集金フェーズ

R国に住む人が一斉に通勤を行う。人 c は、以下の条件を満たす区画 (i_p, j_p), (i_q, j_q) が存在するときのみ、鉄道会社Xの鉄道を利用して料金を払う。 
人 c が鉄道を利用したとき、鉄道会社Xの資金が |i_{c,s} - i_{c,t}| + |j_{c, s} - j _{c, t} | 増える。

- 区画 (i_p, j_p), (i_q, j_q) は駅である。 
 
- 区画 (i_p, j_p) から、0 個以上の互いに接続された駅の区画または線路の区画を経由して、 (i_q, j_q) に到達できる。 
 
-  | i_{c,s} - i_p| + | j_{c,s} - j_p| \leq 2  
 
-  | i_{c,t} - i_q| + | j_{c,t} - j_q| \leq 2

T ターン終了時点での資金ができるだけ大きくなるように行動を決めよ。

### 得点

 各テストケース毎に、 T ターン終了時点での資金の値を絶対スコアとして 
 \( \text{round}(10^9 \times \left(\frac{自身の絶対スコア}{全参加者中の最大絶対スコア}\right)) \)の**相対評価スコア**が得られ、その和が提出の得点となる。 
 ただし、 全参加者中の最大絶対スコア が 0 のテストケースにおいては、 AC した全参加者の相対評価スコアが 10^9 となる。

 以下に挙げる不正な出力をした場合、WAと判定される。

- 駅または線路の区画への線路の配置 
 
- 駅の区画への駅の配置 
 
- 資金が 0 未満になるような配置 
 
- 線路の種類や区画の指定が不正な配置 
 
- 出力した行動の数が T 個でない

 最終順位はコンテスト終了後に実施されるより多くの入力に対するシステムテストにおける得点で決定される。暫定テスト、システムテストともに、一部のテストケースで不正な出力や制限時間超過をした場合、そのテストケースの相対評価スコアは0点となり、そのテストケースにおいては「全参加者中の最大絶対スコア」の計算から除外される。システムテストは **CE 以外の結果を得た一番最後の提出**に対してのみ行われるため、最終的に提出する解答を間違えないよう注意せよ。

#### テストケース数

-  
 暫定テスト: 50個

-  
 システムテスト: 2000個

- コンテスト終了後に [seeds.txt](https://img.atcoder.jp/ahc043/seeds.txt) (sha256=`ed5b3f922dc3633bade3f70cc6f1dd1bb3087dba36ce26ce9d4888ce2ec7a247`) を公開します。

#### 相対評価システムについて

 暫定テスト・システムテストともに、CE以外の結果を得た一番最後の提出のみが順位表に反映される。
 
 相対評価スコアの計算に用いられる各テストケース毎の全参加者中の最大絶対スコアの算出についても、順位表に反映されている最終提出のみが用いられる。

 順位表に表示されているスコアは相対評価スコアであり、新規提出があるたびに、相対評価スコアが再計算される。一方、提出一覧から確認出来る各提出のスコアは各テストケース毎の絶対スコアをそのまま足し合わせたものであり、相対評価スコアは表示されない。最新以外の提出の、現在の順位表における相対評価スコアを知るためには、再提出が必要である。不正な出力や制限時間超過をした場合、提出一覧から確認出来るスコアは0となるが、順位表には正解したテストケースに対する相対評価スコアの和が表示されている。

#### 実行時間について

 実行時間には多少のブレが生じます。 
 また、システムテストでは同時に大量の実行を行うため、暫定テストに比べて数%程度実行時間が伸びる現象が確認されています。 
 そのため、実行時間制限ギリギリの提出がシステムテストで TLE となる可能性があります。 
 プログラム内で時間を計測して処理を打ち切るか、実行時間に余裕を持たせるようお願いします。

### 入力

入力は以下の形式で標準入力から与えられる。

```text
\(
N ~ M ~ K ~ T\\
i_{0, s} ~ j_{0, s} ~ i_{0, t} ~ j_{0, t} \\
i_{1, s} ~ j_{1, s} ~ i_{1, t} ~ j_{1, t} \\
\vdots \\
i_{M-1, s} ~ j_{M-1, s} ~ i_{M-1, t} ~ j_{M-1, t}
\)

```

- 1行目には4つの整数 N,M,K,T がスペース区切りで与えられる。

-  N は区画の縦・横の数で、 N = 50 を満たす。 
 
-  M はR国の人の数で、 50 \le M \le 1600 を満たす。 
 
-  K は鉄道会社Xの初期資金であり、 11000 \leq K \leq 20000  を満たす。 
 
-  T はゲームのターン数であり、 T = 800 を満たす。

- 続く M 行にR国に住む人の通勤に関する情報が与えられる。各行はスペース区切りの4つの整数であり、人 c の家の位置は区画 (i_{c,s}, j_{c,s})、職場の位置は区画 (i_{c, t}, j_{c,t} ) である。0 \leq i_{c, s}, i_{c, t} \lt N, 0 \leq j_{c, s}, j_{c, t} \lt N, | i_{c,s} - i_{c,t} | + | j_{c,s} - j_{c,t} | \gt 4 (0 \leq c \lt M) を満たす。

### 出力

 T 行出力せよ。t 行目には、t 回目の建設フェーズでの行動を以下のフォーマットで出力せよ。

-  
 線路の配置を行う場合

 区画 (i, j) に種類 p の線路を建設するとき、p, i, j をスペース区切りで出力せよ。

 線路の種類は、問題文中の番号付けに従う。

-  
 駅の配置を行う場合

 区画 (i, j) に駅を建設するとき、0, i, j をスペース区切りで出力せよ。

-  
 待機を行う場合

 `-1` を出力せよ。

 [例を見る](https://img.atcoder.jp/ahc043/de43f43a9c.html?lang=ja&output=sample)

### 入力生成方法 
  
 入力生成方法の詳細

 L 以上 U 以下の整数を一様ランダムに生成する関数を \mathrm{rand}(L, U) とし、L 以上 U 未満の実数を一様ランダムに生成する関数を \mathrm{rand\_double}(L, U) とする。 
 平均 \mu 、標準偏差 \sigma の正規分布からランダムに実数値を生成する関数を \mathrm{normal}(\mu,\sigma) とする。

#### M の生成

M = \mathrm{round}(50 \times 2 ^{\mathrm{rand\_double}(0,5)}) で生成する。

#### 家と職場の座標の生成

 次のようにして家の座標を生成するための混合ガウス分布を作成する：
 
 クラスタ数 (\text{num\_cluster}) を \mathrm{rand}(5,15) で生成する。 
 各クラスタ i に対して、重み w_i = \mathrm{rand\_double}(0, 1) 、 中心座標  (r_i, c_i) = (\mathrm{rand\_double}(0, N-1), \mathrm{rand\_double}(0, N-1)) 、 標準偏差 \sigma_i = \mathrm{rand\_double}(2, 15) を生成する。

 職場の座標を生成するための混合ガウス分布も同様にして作成する。

 以下の処理を M 回繰り返し、M 人の家と職場の座標を生成する。

- 家の座標を生成するための混合ガウス分布から、重み w_i に比例する確率で、クラスタ i をランダムに選択する。 
 
- 選択された i に対し、家の座標 r_0 = \text{round}(\text{normal}(r_i, \sigma_i)), c_0 = \text{round}(\text{normal}(c_i, \sigma_i)) を生成する。 
 
- 生成された座標 (r_0, c_0) が 0 \leq r_0 \lt N  または 0 \leq c_0 \lt N  を満たさない場合は、ステップ1からやり直す。 
 
- 職場の座標 (r_1, c_1)  についても同様に生成する。 
 
- 家の座標  (r_0, c_0) と職場の座標 (r_1, c_1) のマンハッタン距離が 4 以下である場合は、ステップ1からやり直す。

#### K の生成

 M 人について、家と職場の座標のマンハッタン距離の最小値を d とする。K = \mathrm{rand}(\mathrm{max}(10, d) \times 100, 2 \times N \times 100) + 10000 で生成する。

### ツール(入力ジェネレータ・ビジュアライザ)

- [Web版](https://img.atcoder.jp/ahc043/de43f43a9c.html): ローカル版より高機能でアニメーション表示が可能です。 
 
- [ローカル版](https://img.atcoder.jp/ahc043/de43f43a9c.zip): 使用するには[Rust言語](https://www.rust-lang.org/ja)のコンパイル環境をご用意下さい。 
 
- [Windows用のコンパイル済みバイナリ](https://img.atcoder.jp/ahc043/de43f43a9c_windows.zip): Rust言語の環境構築が面倒な方は代わりにこちらをご利用下さい。

 **コンテスト期間中に、ビジュアライズ結果の共有や、解法・考察に関する言及は禁止されています。ご注意下さい。**

 解答プログラムは、`#` から始まるコメント行を出力に含めてもかまいません。 
 Web版ビジュアライザを使用すると、コメント行を対応するタイミングで表示できるため、デバッグや考察等に役立てることができます。 
 ジャッジプログラムはコメント行を全て無視するため、コメント行を出力するプログラムをそのまま提出可能です。

### サンプルプログラム 
  
 Pythonによるサンプル実装

```text

import sys

Pos = tuple[int, int]
EMPTY = -1
DO_NOTHING = -1
STATION = 0
RAIL_HORIZONTAL = 1
RAIL_VERTICAL = 2
RAIL_LEFT_DOWN = 3
RAIL_LEFT_UP = 4
RAIL_RIGHT_UP = 5
RAIL_RIGHT_DOWN = 6
COST_STATION = 5000
COST_RAIL = 100


class UnionFind:
    def __init__(self, n: int):
        self.n = n
        self.parents = [-1 for _ in range(n * n)]

    def _find_root(self, idx: int) -> int:
        if self.parents[idx] < 0:
            return idx
        self.parents[idx] = self._find_root(self.parents[idx])
        return self.parents[idx]

    def is_same(self, p: Pos, q: Pos) -> bool:
        p_idx = p[0] * self.n + p[1]
        q_idx = q[0] * self.n + q[1]
        return self._find_root(p_idx) == self._find_root(q_idx)

    def unite(self, p: Pos, q: Pos) -> None:
        p_idx = p[0] * self.n + p[1]
        q_idx = q[0] * self.n + q[1]
        p_root = self._find_root(p_idx)
        q_root = self._find_root(q_idx)
        if p_root != q_root:
            p_size = -self.parents[p_root]
            q_size = -self.parents[q_root]
            if p_size > q_size:
                p_root, q_root = q_root, p_root
            self.parents[q_root] += self.parents[p_root]
            self.parents[p_root] = q_root


def distance(a: Pos, b: Pos) -> int:
    return abs(a[0] - b[0]) + abs(a[1] - b[1])


class Action:
    def __init__(self, type: int, pos: Pos):
        self.type = type
        self.pos = pos

    def __str__(self):
        if self.type == DO_NOTHING:
            return "-1"
        else:
            return f"{self.type} {self.pos[0]} {self.pos[1]}"


class Result:
    def __init__(self, actions: list[Action], score: int):
        self.actions = actions
        self.score = score

    def __str__(self):
        return "\n".join(map(str, self.actions))


class Field:
    def __init__(self, N: int):
        self.N = N
        self.rail = [[EMPTY] * N for _ in range(N)]
        self.uf = UnionFind(N)

    def build(self, type: int, r: int, c: int) -> None:
        assert self.rail[r][c] != STATION
        if 1 <= type <= 6:
            assert self.rail[r][c] == EMPTY
        self.rail[r][c] = type

        # 隣接する区画と接続
        # 上
        if type in (STATION, RAIL_VERTICAL, RAIL_LEFT_UP, RAIL_RIGHT_UP):
            if r > 0 and self.rail[r - 1][c] in (STATION, RAIL_VERTICAL, RAIL_LEFT_DOWN, RAIL_RIGHT_DOWN):
                self.uf.unite((r, c), (r - 1, c))
        # 下
        if type in (STATION, RAIL_VERTICAL, RAIL_LEFT_DOWN, RAIL_RIGHT_DOWN):
            if r < self.N - 1 and self.rail[r + 1][c] in (STATION, RAIL_VERTICAL, RAIL_LEFT_UP, RAIL_RIGHT_UP):
                self.uf.unite((r, c), (r + 1, c))
        # 左
        if type in (STATION, RAIL_HORIZONTAL, RAIL_LEFT_DOWN, RAIL_LEFT_UP):
            if c > 0 and self.rail[r][c - 1] in (STATION, RAIL_HORIZONTAL, RAIL_RIGHT_DOWN, RAIL_RIGHT_UP):
                self.uf.unite((r, c), (r, c - 1))
        # 右
        if type in (STATION, RAIL_HORIZONTAL, RAIL_RIGHT_DOWN, RAIL_RIGHT_UP):
            if c < self.N - 1 and self.rail[r][c + 1] in (STATION, RAIL_HORIZONTAL, RAIL_LEFT_DOWN, RAIL_LEFT_UP):
                self.uf.unite((r, c), (r, c + 1))

    def is_connected(self, s: Pos, t: Pos) -> bool:
        assert distance(s, t) > 4  # 前提条件
        stations0 = self.collect_stations(s)
        stations1 = self.collect_stations(t)
        for station0 in stations0:
            for station1 in stations1:
                if self.uf.is_same(station0, station1):
                    return True
        return False

    def collect_stations(self, pos: Pos) -> list[Pos]:
        stations = []
        for dr in range(-2, 3):
            for dc in range(-2, 3):
                if abs(dr) + abs(dc) > 2:
                    continue
                r = pos[0] + dr
                c = pos[1] + dc
                if 0 <= r < self.N and 0 <= c < self.N and self.rail[r][c] == STATION:
                    stations.append((r, c))
        return stations


class Solver:
    def __init__(self, N: int, M: int, K: int, T: int, home: list[Pos], workplace: list[Pos]):
        self.N = N
        self.M = M
        self.K = K
        self.T = T
        self.home = home
        self.workplace = workplace
        self.field = Field(N)
        self.money = K
        self.actions = []

    def calc_income(self) -> int:
        income = 0
        for i in range(self.M):
            if self.field.is_connected(self.home[i], self.workplace[i]):
                income += distance(self.home[i], self.workplace[i])
        return income

    def build_rail(self, type: int, r: int, c: int) -> None:
        self.field.build(type, r, c)
        self.money -= COST_RAIL
        self.actions.append(Action(type, (r, c)))

    def build_station(self, r: int, c: int) -> None:
        self.field.build(STATION, r, c)
        self.money -= COST_STATION
        self.actions.append(Action(STATION, (r, c)))

    def build_nothing(self) -> None:
        self.actions.append(Action(DO_NOTHING, (0, 0)))

    def solve(self) -> Result:
        # 接続する人を見つける
        rail_count = (self.K - COST_STATION * 2) // COST_RAIL
        person_idx = 0
        while person_idx < self.M:
            if distance(self.home[person_idx], self.workplace[person_idx]) - 1 <= rail_count:
                break
            person_idx += 1
        assert person_idx != self.M

        # 駅の配置
        self.build_station(*self.home[person_idx])
        self.build_station(*self.workplace[person_idx])

        # 線路を配置して駅を接続する
        r0, c0 = self.home[person_idx]
        r1, c1 = self.workplace[person_idx]
        # r0 -> r1
        if r0 < r1:
            for r in range(r0 + 1, r1):
                self.build_rail(RAIL_VERTICAL, r, c0)
            if c0 < c1:
                self.build_rail(RAIL_RIGHT_UP, r1, c0)
            elif c0 > c1:
                self.build_rail(RAIL_LEFT_UP, r1, c0)
        elif r0 > r1:
            for r in range(r0 - 1, r1, -1):
                self.build_rail(RAIL_VERTICAL, r, c0)
            if c0 < c1:
                self.build_rail(RAIL_RIGHT_DOWN, r1, c0)
            elif c0 > c1:
                self.build_rail(RAIL_LEFT_DOWN, r1, c0)
        # c0 -> c1
        if c0 < c1:
            for c in range(c0 + 1, c1):
                self.build_rail(RAIL_HORIZONTAL, r1, c)
        elif c0 > c1:
            for c in range(c0 - 1, c1, -1):
                self.build_rail(RAIL_HORIZONTAL, r1, c)

        income = self.calc_income()
        self.money += income

        # あとは待機
        while len(self.actions) < self.T:
            self.build_nothing()
            self.money += income

        return Result(self.actions, self.money)


def main():
    N, M, K, T = map(int, input().split())
    home = []
    workplace = []
    for _ in range(M):
        r0, c0, r1, c1 = map(int, input().split())
        home.append((r0, c0))
        workplace.append((r1, c1))

    solver = Solver(N, M, K, T, home, workplace)
    result = solver.solve()
    print(result)
    print(f"score={result.score}", file=sys.stderr)


if __name__ == "__main__":
    main()

```

以下の処理を実装しています。

- 人 0, 人 1, \ldots と順に見て、家と職場の位置に駅を配置して駅の間を最短距離で線路で接続することが初期資金で可能であるような最初の人を見つける 
 
- 見つけた人の家と職場の位置に駅を配置し、家から職場までを接続する線路を配置する 
 
- その後は T ターン目まで待機を行う

資金のシミュレーションを行うコードを含んでいます。参考にしてください。

### Story

Nihonbashi Simulator is a turn-based simulation game where you manage railway company X in the fictional country R. As a big fan of this game, your goal is to make the company highly profitable. By appropriately constructing rails and stations, you will assist the people living in country R in their commuting while developing X into the best railway company!

### Problem Statement

 Country R is composed of sections arranged in an N \times N grid. The section in the i-th row from the top (0 \le i < N), and the j-th column from the left (0 \le j < N) is denoted as (i,j). 
 Each section can be either empty, rail, or station:

- Empty

- Does not connect to other sections. 
 
- Can be changed to a station or rail by paying a cost.

- Rail

- Connects to a maximum of two adjacent non-empty sections on the up, down, left, and right. Rails are of six types, numbered as shown in the diagram below, depending on their connections. 
 
- Can be changed to a station by paying a cost.

- Station

- Connects to a maximum of four adjacent non-empty sections on the up, down, left, and right.

 1

 2

 3

 4

 5

 6

 Types of rails

There are M people living in country R. The home of person c is located at section (i_{c,s}, j_{c,s}), and workplace is at section (i_{c,t}, j_{c,t}), between which he/she commutes.

At the start of the game, railway company X has money of K, and all sections are empty. The game will proceed for T turns.

### How the game progresses

Each turn consists of a construction phase followed by a collection phase.

#### Construction phase

During the construction phase, one of the following actions is performed: placing a rail, placing a station, or waiting. Actions that would result in money going below zero are not allowed. 
 The check for whether money is below zero is performed before the collection phase.

- Placing a rail: Select one empty section and change it to one of the six types of rails. Money decreases by 100. 
 
- Placing a station: Select one empty or rail section and change it to a station. Money decreases by 5000. 
 
- Waiting: The state of the sections and the money do not change.

#### Collection phase

The people living in country R commute simultaneously. Person c will use train and pay a fare to railway company X only if there exist sections (i_p, j_p) and (i_q, j_q) that satisfy the following conditions:

- Sections (i_p, j_p), (i_q, j_q) are stations. 
 
- It is possible to reach section (i_q, j_q) from section (i_p, j_p) via zero or more connected station or rail sections. 
 
-  | i_{c,s} - i_p| + | j_{c,s} - j_p| \leq 2  
 
-  | i_{c,t} - i_q| + | j_{c,t} - j_q| \leq 2

When person c uses the train, railway company X's money increases by |i_{c,s} - i_{c,t}| + |j_{c,s} - j_{c,t}|.

Please determine the actions to maximize the money at the end of turn T.

### Scoring

For each test case, the absolute score is obtained as the value of the money at the end of T turns, and we compute the **relative score** \mathrm{round}(10^9\times \frac{\mathrm{YOUR}}{\mathrm{MAX}}), where YOUR is your absolute score and MAX is the maximum absolute score among all competitors obtained on that test case. The score of the submission is the sum of the relative scores. 
 In test cases where MAX is 0, the relative score for all participants who received AC will be 10^9.

 If the following invalid outputs are produced, it will be judged as WA。

- Placing a rail into a station or rail section 
 
- Placing a station into a station section 
 
- An action that results in money being less than 0 
 
- Invalid action with incorrect rail types or section position 
 
- The number of actions output is not T

The final ranking will be determined by the system test with more inputs which will be run after the contest is over. 
 In both the provisional/system test, if your submission produces illegal output or exceeds the time limit for some test cases, only the score for those test cases will be zero, and your submission will be excluded from the MAX calculation for those test cases.

The system test will be performed only for **the last submission which received a result other than CE **. 
 Be careful not to make a mistake in the final submission.

#### Number of test cases

-  
 Provisional test: 50

-  
 System test: 2000

- We will publish [seeds.txt](https://img.atcoder.jp/ahc043/seeds.txt) (sha256=`ed5b3f922dc3633bade3f70cc6f1dd1bb3087dba36ce26ce9d4888ce2ec7a247`) after the contest is over.

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

### Input

Input will be provided from Standard Input in the following format.

```text
\(
N ~ M ~ K ~ T\\
i_{0, s} ~ j_{0, s} ~ i_{0, t} ~ j_{0, t} \\
i_{1, s} ~ j_{1, s} ~ i_{1, t} ~ j_{1, t} \\
\vdots \\
i_{M-1, s} ~ j_{M-1, s} ~ i_{M-1, t} ~ j_{M-1, t}
\)

```

- On the first line, there are four integers N,M,K,T separated by spaces.

-  N represents the number of rows and columns in country R and satisfies N = 50. 
 
-  M represents the number of people in country R and satisfies 50 \le M \le 1600. 
 
-  K represents the initial money of railway company X and satisfies 11000 \leq K \leq 20000. 
 
-  T represents the number of turns in the game and satisfies T = 800.

-  
 The following M lines provide information about the commuting of people living in country R. 
 Each line contains four space-separated integers, where the home of person c is at section (i_{c,s}, j_{c,s}) and the workplace is at section (i_{c, t}, j_{c,t} ). 
 The following conditions are satisfied: 0 \leq i_{c, s}, i_{c, t} \lt N, 0 \leq j_{c, s}, j_{c, t} \lt N, | i_{c,s} - i_{c,t} | + | j_{c,s} - j_{c,t} | \gt 4 (0 \leq c \lt M).

### Output

Output T lines. For the t-th row, output the action taken during the t-th construction phase in the following format:

-  
 When placing a rail

 If you are constructing a rail of type p at section (i, j), output p, i, j separated by spaces.

 The types of rails are numbered according to the diagram provided in the problem statement.

-  
 When placing a station

 If you are constructing a station at section (i, j), output 0, i, j separated by spaces.

-  
 When waiting

 Output `-1`.

[show example](https://img.atcoder.jp/ahc043/de43f43a9c.html?lang=en&output=sample)

### Input Generation 
  
 Details

 Let \mathrm{rand}(L, U) be a function that generates a uniform random integer between L and U, inclusive, 
 and \mathrm{rand\_double}(L, U) be a function that generates a uniform random real number greater than or equal to L and less than U, 
 and \mathrm{normal}(\mu,\sigma) be a function that generates a random real number from a normal distribution with mean \mu and standard deviation \sigma.

#### Generation of M

M = \mathrm{round}(50 \times 2 ^{\mathrm{rand\_double}(0,5)})

#### Generation of the locations of homes and workplaces

 Create a Gaussian mixture model to generate home locations as follows:
 
 Generate the number of clusters with \mathrm{rand}(5,15). 
 For each cluster i, generate the weight w_i = \mathrm{rand\_double}(0, 1), the center coordinates  (r_i, c_i) = (\mathrm{rand\_double}(0, N-1), \mathrm{rand\_double}(0, N-1)), and the standard deviation \sigma_i = \mathrm{rand\_double}(2, 15).

 Similarly, create a Gaussian mixture model to generate workplace locations.

 Repeat the following process M times to generate the locations for people's homes and workplaces:

- Randomly select a cluster i from the Gaussian mixture model for generating home locations with a probability proportional to the weight w_i. 
 
- For the selected i, generate the home location as r_0 = \text{round}(\text{normal}(r_i, \sigma_i)), c_0 = \text{round}(\text{normal}(c_i, \sigma_i)). 
 
- If the generated location (r_0, c_0) do not satisfy 0 \leq r_0 \lt N  or 0 \leq c_0 \lt N , start over from step 1. 
 
- Similarly, generate the workplace location (r_1, c_1) . 
 
- If the Manhattan distance between the home location  (r_0, c_0) and the workplace location (r_1, c_1) is less than or equal to 4, start over from step 1.

#### Generation of K

 For M people, let the minimum Manhattan distance between their home and workplace locations be d. 
 K is generated as \mathrm{rand}(\mathrm{max}(10, d) \times 100, 2 \times N \times 100) + 10000.

### Tools (Input generator and visualizer)

- [Web version](https://img.atcoder.jp/ahc043/de43f43a9c.html?lang=en): This is more powerful than the local version providing animations. 
 
- [Local version](https://img.atcoder.jp/ahc043/de43f43a9c.zip): You need a compilation environment of [Rust language](https://www.rust-lang.org/). 
 
- [Pre-compiled binary for Windows](https://img.atcoder.jp/ahc043/de43f43a9c_windows.zip): If you are not familiar with the Rust language environment, please use this instead.

 **Please be aware that sharing visualization results or discussing solutions/ideas during the contest is prohibited.**

 Your program may output comment lines starting with `#`. 
 The web version of the visualizer displays the comment lines with the corresponding output, which may be useful for debugging and analysis. 
 Since the judge program ignores all comment lines, you can submit a program that outputs comment lines as is.

### Sample Solution 
  
 Sample implementation in Python

```text

import sys

Pos = tuple[int, int]
EMPTY = -1
DO_NOTHING = -1
STATION = 0
RAIL_HORIZONTAL = 1
RAIL_VERTICAL = 2
RAIL_LEFT_DOWN = 3
RAIL_LEFT_UP = 4
RAIL_RIGHT_UP = 5
RAIL_RIGHT_DOWN = 6
COST_STATION = 5000
COST_RAIL = 100


class UnionFind:
    def __init__(self, n: int):
        self.n = n
        self.parents = [-1 for _ in range(n * n)]

    def _find_root(self, idx: int) -> int:
        if self.parents[idx] < 0:
            return idx
        self.parents[idx] = self._find_root(self.parents[idx])
        return self.parents[idx]

    def is_same(self, p: Pos, q: Pos) -> bool:
        p_idx = p[0] * self.n + p[1]
        q_idx = q[0] * self.n + q[1]
        return self._find_root(p_idx) == self._find_root(q_idx)

    def unite(self, p: Pos, q: Pos) -> None:
        p_idx = p[0] * self.n + p[1]
        q_idx = q[0] * self.n + q[1]
        p_root = self._find_root(p_idx)
        q_root = self._find_root(q_idx)
        if p_root != q_root:
            p_size = -self.parents[p_root]
            q_size = -self.parents[q_root]
            if p_size > q_size:
                p_root, q_root = q_root, p_root
            self.parents[q_root] += self.parents[p_root]
            self.parents[p_root] = q_root


def distance(a: Pos, b: Pos) -> int:
    return abs(a[0] - b[0]) + abs(a[1] - b[1])


class Action:
    def __init__(self, type: int, pos: Pos):
        self.type = type
        self.pos = pos

    def __str__(self):
        if self.type == DO_NOTHING:
            return "-1"
        else:
            return f"{self.type} {self.pos[0]} {self.pos[1]}"


class Result:
    def __init__(self, actions: list[Action], score: int):
        self.actions = actions
        self.score = score

    def __str__(self):
        return "\n".join(map(str, self.actions))


class Field:
    def __init__(self, N: int):
        self.N = N
        self.rail = [[EMPTY] * N for _ in range(N)]
        self.uf = UnionFind(N)

    def build(self, type: int, r: int, c: int) -> None:
        assert self.rail[r][c] != STATION
        if 1 <= type <= 6:
            assert self.rail[r][c] == EMPTY
        self.rail[r][c] = type

        # connect adjacent cells
        # top
        if type in (STATION, RAIL_VERTICAL, RAIL_LEFT_UP, RAIL_RIGHT_UP):
            if r > 0 and self.rail[r - 1][c] in (STATION, RAIL_VERTICAL, RAIL_LEFT_DOWN, RAIL_RIGHT_DOWN):
                self.uf.unite((r, c), (r - 1, c))
        # bottom
        if type in (STATION, RAIL_VERTICAL, RAIL_LEFT_DOWN, RAIL_RIGHT_DOWN):
            if r < self.N - 1 and self.rail[r + 1][c] in (STATION, RAIL_VERTICAL, RAIL_LEFT_UP, RAIL_RIGHT_UP):
                self.uf.unite((r, c), (r + 1, c))
        # left
        if type in (STATION, RAIL_HORIZONTAL, RAIL_LEFT_DOWN, RAIL_LEFT_UP):
            if c > 0 and self.rail[r][c - 1] in (STATION, RAIL_HORIZONTAL, RAIL_RIGHT_DOWN, RAIL_RIGHT_UP):
                self.uf.unite((r, c), (r, c - 1))
        # right
        if type in (STATION, RAIL_HORIZONTAL, RAIL_RIGHT_DOWN, RAIL_RIGHT_UP):
            if c < self.N - 1 and self.rail[r][c + 1] in (STATION, RAIL_HORIZONTAL, RAIL_LEFT_DOWN, RAIL_LEFT_UP):
                self.uf.unite((r, c), (r, c + 1))

    def is_connected(self, s: Pos, t: Pos) -> bool:
        assert distance(s, t) > 4  # precondition
        stations0 = self.collect_stations(s)
        stations1 = self.collect_stations(t)
        for station0 in stations0:
            for station1 in stations1:
                if self.uf.is_same(station0, station1):
                    return True
        return False

    def collect_stations(self, pos: Pos) -> list[Pos]:
        stations = []
        for dr in range(-2, 3):
            for dc in range(-2, 3):
                if abs(dr) + abs(dc) > 2:
                    continue
                r = pos[0] + dr
                c = pos[1] + dc
                if 0 <= r < self.N and 0 <= c < self.N and self.rail[r][c] == STATION:
                    stations.append((r, c))
        return stations


class Solver:
    def __init__(self, N: int, M: int, K: int, T: int, home: list[Pos], workplace: list[Pos]):
        self.N = N
        self.M = M
        self.K = K
        self.T = T
        self.home = home
        self.workplace = workplace
        self.field = Field(N)
        self.money = K
        self.actions = []

    def calc_income(self) -> int:
        income = 0
        for i in range(self.M):
            if self.field.is_connected(self.home[i], self.workplace[i]):
                income += distance(self.home[i], self.workplace[i])
        return income

    def build_rail(self, type: int, r: int, c: int) -> None:
        self.field.build(type, r, c)
        self.money -= COST_RAIL
        self.actions.append(Action(type, (r, c)))

    def build_station(self, r: int, c: int) -> None:
        self.field.build(STATION, r, c)
        self.money -= COST_STATION
        self.actions.append(Action(STATION, (r, c)))

    def build_nothing(self) -> None:
        self.actions.append(Action(DO_NOTHING, (0, 0)))

    def solve(self) -> Result:
        # find a person to connect
        rail_count = (self.K - COST_STATION * 2) // COST_RAIL
        person_idx = 0
        while person_idx < self.M:
            if distance(self.home[person_idx], self.workplace[person_idx]) - 1 <= rail_count:
                break
            person_idx += 1
        assert person_idx != self.M

        # build stations
        self.build_station(*self.home[person_idx])
        self.build_station(*self.workplace[person_idx])

        # connect stations with rails
        r0, c0 = self.home[person_idx]
        r1, c1 = self.workplace[person_idx]
        # r0 -> r1
        if r0 < r1:
            for r in range(r0 + 1, r1):
                self.build_rail(RAIL_VERTICAL, r, c0)
            if c0 < c1:
                self.build_rail(RAIL_RIGHT_UP, r1, c0)
            elif c0 > c1:
                self.build_rail(RAIL_LEFT_UP, r1, c0)
        elif r0 > r1:
            for r in range(r0 - 1, r1, -1):
                self.build_rail(RAIL_VERTICAL, r, c0)
            if c0 < c1:
                self.build_rail(RAIL_RIGHT_DOWN, r1, c0)
            elif c0 > c1:
                self.build_rail(RAIL_LEFT_DOWN, r1, c0)
        # c0 -> c1
        if c0 < c1:
            for c in range(c0 + 1, c1):
                self.build_rail(RAIL_HORIZONTAL, r1, c)
        elif c0 > c1:
            for c in range(c0 - 1, c1, -1):
                self.build_rail(RAIL_HORIZONTAL, r1, c)

        income = self.calc_income()
        self.money += income

        # do nothing for the rest of the time
        while len(self.actions) < self.T:
            self.build_nothing()
            self.money += income

        return Result(self.actions, self.money)


def main():
    N, M, K, T = map(int, input().split())
    home = []
    workplace = []
    for _ in range(M):
        r0, c0, r1, c1 = map(int, input().split())
        home.append((r0, c0))
        workplace.append((r1, c1))

    solver = Solver(N, M, K, T, home, workplace)
    result = solver.solve()
    print(result)
    print(f"score={result.score}", file=sys.stderr)


if __name__ == "__main__":
    main()

```

The following processes are implemented:

- Examine person 0, 1, \ldots in order to find the first person for whom it is possible to place stations at their home and workplace locations and connect them with rails at the initial money. 
 
- Place stations at the identified person's home and workplace locations, and place rails to connect from home to workplace. 
 
- After that, wait until turn T.

The code includes a simulation of the changes in money. You may use it as a reference when creating your solution.

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

- [ルール](https://atcoder.jp/contests/ahc043/rules)
 
- [用語集](https://atcoder.jp/contests/ahc043/glossary)

- [利用規約](https://atcoder.jp/tos)
 
- [プライバシーポリシー](https://atcoder.jp/privacy)
 
- [個人情報保護方針](https://atcoder.jp/personal)
 
- [企業情報](https://atcoder.jp/company)
 
- [よくある質問](https://atcoder.jp/faq)
 
- [お問い合わせ](https://atcoder.jp/contact)
 
- [資料請求](https://atcoder.jp/documents/request)

 Copyright Since 2012 ©[AtCoder Inc.](http://atcoder.co.jp) All rights reserved.

  ページトップ
