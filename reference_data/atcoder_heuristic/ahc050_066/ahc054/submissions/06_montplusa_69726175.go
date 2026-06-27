// main.go (提出用・単一ファイル)
package main

import (
	"bufio"
	"container/list"
	"fmt"
	"math"
	"math/bits"
	"math/rand"
	"os"
	"strconv"
	"time"
)

/********** 定数（必要に応じて調整） **********/
const (
	INF        = 100000000
	TIME_LIMIT = 1800 * 1000 // microseconds
	START_TEMP = 10.0
	END_TEMP   = 0.01
	MINIMIZE   = true
)

var DIRS = [4][2]int{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}

var CASTLE_WALLS = [][][2]int{
	{{0, -1}, {1, 0}, {-1, 0}, {1, 1}, {0, 2}, {-2, 1}},
	{{0, -1}, {1, 0}, {-1, 0}, {-2, 0}, {1, 1}, {0, 2}, {-3, 1}},
	{{0, -1}, {1, 0}, {-1, 0}, {1, 1}, {0, 2}},
	{{0, -1}, {1, 0}, {-1, 0}, {-1, 1}, {1, 1}, {1, 2}, {0, 3}, {-2, 2}},
	{{0, -1}, {1, 0}, {-1, 0}, {-1, 1}, {1, 1}, {1, 2}, {0, 3}},
	{{0, -1}, {1, 0}, {-1, 0}},
}
var CASTLE_PATH = [][][2]int{
	{{0, 0}, {0, 1}, {-1, 1}},
	{{0, 0}, {0, 1}, {-1, 1}},
	{{0, 0}, {0, 1}, {-1, 1}},
	{{0, 0}, {0, 1}, {0, 2}, {-1, 2}},
	{{0, 0}, {0, 1}, {0, 2}, {-1, 2}},
	{{0, 0}, {0, 1}},
}
var CASTLE_PREFERENCE = []int{
	3000,
	3000,
	1000,
	0,
	-2000,
	-20000,
}

// var CASTLE_WALLS[0] = [][2]int{{0, -1}, {1, 0}, {-1, 0}, {0, 2}, {1, 1}}
// var CASTLE_PATH[0] = [][2]int{{0, 0}, {0, 1}, {-1, 1}}

var TRAP_WALLS = [][][2]int{
	{{0, -1}, {1, -1}, {1, 0}, {2, 0}, {-1, 0}, {1, 1}},
	{{0, -1}, {1, -1}, {1, 0}, {2, 0}, {-1, 0}, {0, 2}, {1, 1}, {-2, 1}},
	{{0, -1}, {1, -1}, {-1, -1}, {1, 0}, {2, 0}, {-2, 0}, {0, 2}, {1, 1}, {-3, 1}, {-4, 2}, {-1, 3}},
}
var TRAP_PATH = [][][2]int{
	{{0, 0}, {0, 1}},
	{{0, 0}, {0, 1}, {-1, 1}, {-1, 2}},
	{{0, 0}, {0, 1}, {-1, 1}, {-2, 1}, {-2, 2}, {-3, 2}},
}

/********** ユーティリティ（提出ファイルに内包） **********/
type RandGenerater struct{ x uint64 }

func NewRandGenerater(seed uint64) *RandGenerater {
	if seed == 0 {
		seed = 88172645463393265
	}
	return &RandGenerater{x: seed}
}
func (rg *RandGenerater) Next() uint64 {
	// xorshift64
	x := rg.x
	x ^= x << 7
	x ^= x >> 9
	rg.x = x
	return x
}
func (rg *RandGenerater) IntN(n int) int {
	if n <= 0 {
		return 0
	}
	return int(rg.Next() % uint64(n))
}

func fastPow(a float64, e int) float64 {
	if e == 0 {
		return 1
	}
	k := 64 - bits.LeadingZeros(uint(e))
	ret := 1.0
	a1 := a
	for i := 0; i < k; i++ {
		if e&(1<<i) != 0 {
			ret *= a1
		}
		a1 *= a1
	}
	return ret
}
func MaxInt(a, b int) int {
	if a > b {
		return a
	}
	return b
}
func MinInt(a, b int) int {
	if a < b {
		return a
	}
	return b
}
func AbsInt(a int) int {
	if a < 0 {
		return -a
	}
	return a
}
func Popcount64(x uint64) int { return bits.OnesCount64(x) }

/********** ゲーム定数・状態・アクション **********/
type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	// 問題定数
	N         int
	Ti        int
	Tj        int
	Si        int      // entrance i (=0)
	Sj        int      // entrance j (=N/2)
	B         []bool   // 盤面
	Neighbors [][4]int // 4近傍のindex 存在しない場合-1
	FlowerIJ  int
}

func NewGameConst(randSeed int64) *GameConst {
	return &GameConst{Random: rand.New(rand.NewSource(randSeed))}
}
func (g *GameConst) SetScanner(scanner *bufio.Scanner) { g.Scanner = scanner }

// トークン読み取り
func (g *GameConst) nextToken() (string, bool) {
	if !g.Scanner.Scan() {
		return "", false
	}
	return g.Scanner.Text(), true
}
func (g *GameConst) NextInt() (int, bool) {
	tok, ok := g.nextToken()
	if !ok {
		return 0, false
	}
	v, _ := strconv.Atoi(tok)
	return v, true
}
func (g *GameConst) NextString() (string, bool) {
	return g.nextToken()
}

// 入力読み取り（最初の固定部分のみ）
func GetInputs(gConst *GameConst) {
	// N ti tj
	n, ok := gConst.NextInt()
	if !ok {
		return
	}
	ti, _ := gConst.NextInt()
	tj, _ := gConst.NextInt()

	gConst.N = n
	gConst.Ti = ti
	gConst.Tj = tj
	gConst.FlowerIJ = gConst.Ti*gConst.N + gConst.Tj
	gConst.Si = 0
	gConst.Sj = n / 2

	gConst.B = make([]bool, n*n)
	for i := 0; i < n; i++ {
		// 1行まるごと（'.'と'T'からなる文字列）
		row, _ := gConst.NextString()
		for j := range row {
			isWall := false
			if row[j] == 'T' {
				isWall = true
			}
			gConst.B[i*gConst.N+j] = isWall
		}
	}
	gConst.Neighbors = make([][4]int, gConst.N*gConst.N)
	for ij := range gConst.Neighbors {
		i := ij / gConst.N
		j := ij % gConst.N
		for d := 0; d < 4; d++ {
			ni := i + DIRS[d][0]
			nj := j + DIRS[d][1]
			if ni < 0 || ni >= gConst.N || nj < 0 || nj >= gConst.N {
				gConst.Neighbors[ij][d] = -1
			} else {
				nij := ni*gConst.N + nj
				gConst.Neighbors[ij][d] = nij
			}
		}
	}
}

type GameState struct {
	N  int
	Ti int
	Tj int

	// 現在ターンの公開状態
	Pi int
	Pj int

	ShortestPath []int // 開始点からゴールまでのSP

	Stabilized bool // castleが確定したか
	Turn       int
	plan       []bool // 壁予定地
	fixed      []bool // castleにより仮確定
	passed     []int  // このマスの到達回数

	Revealed []bool // 確認済みマス（N×N）
}

func NewGameState(gConst *GameConst) *GameState {
	g := &GameState{
		N:  gConst.N,
		Ti: gConst.Ti,
		Tj: gConst.Tj,
	}
	g.ShortestPath = make([]int, 0)
	g.plan = make([]bool, g.N*g.N)
	g.fixed = make([]bool, g.N*g.N)
	g.passed = make([]int, g.N*g.N)
	g.Revealed = make([]bool, g.N*g.N)
	return g
}
func (g *GameState) Copy(gConst *GameConst) *GameState {
	ng := NewGameState(gConst)
	ng.Load(g, gConst)
	return ng
}
func (g *GameState) Load(src *GameState, gConst *GameConst) {
	g.N = src.N
	g.Ti = src.Ti
	g.Tj = src.Tj
	g.Pi = src.Pi
	g.Pj = src.Pj
	g.Turn = src.Turn
	g.Stabilized = src.Stabilized
	g.ShortestPath = g.ShortestPath[:0]
	g.ShortestPath = append(g.ShortestPath, src.ShortestPath...)
	for i := range g.plan {
		g.plan[i] = src.plan[i]
	}
	for i := range g.fixed {
		g.fixed[i] = src.fixed[i]
	}
	for i := range g.passed {
		g.passed[i] = src.passed[i]
	}
	for i := range g.Revealed {
		g.Revealed[i] = src.Revealed[i]
	}
}

// 各ターンの入力を読む（到達判定・EOF判定付き）
func (g *GameState) ReadTurn(c *GameConst) (done bool, ok bool) {
	pi, ok := c.NextInt()
	if !ok {
		return false, false
	}
	pj, ok2 := c.NextInt()
	if !ok2 {
		return false, false
	}
	g.Pi, g.Pj = pi, pj
	g.passed[pi*g.N+pj]++

	// 花に到達したら終了（このターンは出力不要）
	if g.Pi == g.Ti && g.Pj == g.Tj {
		return true, true
	}

	// 前ターンに新たに確認済みになったマス群
	n, ok3 := c.NextInt()
	if !ok3 {
		return false, false
	}
	for k := 0; k < n; k++ {
		x, _ := c.NextInt()
		y, _ := c.NextInt()
		if 0 <= x && x < g.N && 0 <= y && y < g.N {
			g.Revealed[x*g.N+y] = true
		}
	}
	g.Turn++
	return false, true
}

type Action struct {
	// 今ターンに配置するトレント群（未確認の空きに限る）
	X      []int
	Y      []int
	finish bool
}

func NewAction() *Action         { return &Action{} }
func (a *Action) Load(b *Action) { a.X = append(a.X[:0], b.X...); a.Y = append(a.Y[:0], b.Y...) }
func (a *Action) Copy() *Action  { na := NewAction(); na.Load(a); return na }

// 出力整形（問題の出力仕様に合わせて実装）
func FormatAction(a *Action, g *GameState, c *GameConst) string {
	if a.finish {
		return "-1"
	}
	if a == nil || len(a.X) == 0 || len(a.Y) == 0 {
		return "0"
	}
	if len(a.X) != len(a.Y) {
		// 不整合の場合は安全側で 0
		return "0"
	}
	m := len(a.X)
	// 1行に m x0 y0 x1 y1 ...
	out := fmt.Sprintf("%d", m)
	for i := 0; i < m; i++ {
		out += fmt.Sprintf(" %d %d", a.X[i], a.Y[i])
	}
	return out
}

/********** ソルバ **********/
type Solver struct {
	timeLimit time.Duration
}

func NewSolver(us int) *Solver { return &Solver{timeLimit: time.Duration(us) * time.Microsecond} }

// Solve は (今ターンのアクション, 内部評価スコア, 反復回数) を返す。
func (solver *Solver) Solve(gameState *GameState, gameConst *GameConst) (*Action, int, int) {
	best := NewAction() // デフォルト: 置かない（m=0）
	bestScore := 0
	loop := 1

	// if gameState.Turn > 1 {
	// 	best.finish = true
	// 	return best, bestScore, loop
	// }
	numSample := 1
	cands := make([]*GameState, 0, numSample)
	if gameState.Turn == 1 {
		for sampleIndex := 0; sampleIndex < numSample; sampleIndex++ {
			cGameState := gameState.Copy(gameConst)
			cGameState.MakePlan(gameConst, false)
			cands = append(cands, cGameState)
		}
		bestValue := -float64(INF)
		bestIndex := 0
		for sampleIndex := 0; sampleIndex < numSample; sampleIndex++ {
			value := 0.0
			cGameState := cands[sampleIndex].Copy(gameConst)
			value = evaluateState(cGameState, gameConst)

			if value > bestValue {
				bestValue = value
				bestIndex = sampleIndex
			}
		}
		gameState.Load(cands[bestIndex], gameConst)

	}

	if true {

		// 適宜公開する場合
		if gameState.passed[gameState.Pi*gameConst.N+gameState.Pj] == 1 {
			// 初めて到達したので、木を必要に応じて確定させる
			first := true
			for {
				best.X = []int{}
				best.Y = []int{}
				rerun := false
				fixedIsWall := make([]bool, gameConst.N*gameConst.N)
				for ij := range fixedIsWall {
					fixedIsWall[ij] = gameState.plan[ij] && (gameConst.B[ij] || gameState.Revealed[ij])
					fixedIsWall[ij] = fixedIsWall[ij] || gameState.fixed[ij]
				}
				for d := 0; d < 4; d++ {
					// 最初に見える木をマーク
					// playerを一定方向に進ませる
					playerij := gameState.Pi*gameConst.N + gameState.Pj
					for {
						playerij = gameConst.Neighbors[playerij][d]
						if playerij == -1 {
							break
						}
						if gameState.plan[playerij] && gameState.Revealed[playerij] {
							// 既に木が確定しているのでスルー
							break
						}
						dirFromGoal := -1
						for d2 := 0; d2 < 4; d2++ {
							nij := gameConst.Neighbors[playerij][d2]
							if nij == gameConst.Ti*gameConst.N+gameConst.Tj {
								dirFromGoal = d2
								break
							}
						}
						if dirFromGoal != -1 && first && !gameState.Stabilized {
							fmt.Fprintln(os.Stderr, "castle adjustment start...(pi,pj)=", "(", gameState.Pi, ",", gameState.Pj, ")")
							// goalに隣接している
							// castleの方向をどうするか考える必要あり
							// 適切な壁が既に見えていれば除去で問題ない
							bestValue := -float64(INF)
							bestRot := 0
							bestFlip := 0
							bestC := 0
							bestPattern := 0
							for pattern := range CASTLE_WALLS {
								for rot := 0; rot < 4; rot++ {
									for flip := 0; flip < 2; flip++ {
										ok := true
										// 壁設置可能？
										for wIndex := range CASTLE_WALLS[pattern] {
											di, dj := CASTLE_WALLS[pattern][wIndex][0], CASTLE_WALLS[pattern][wIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											wi := gameConst.Ti + di
											wj := gameConst.Tj + dj
											if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
												continue
											}
											if gameState.Revealed[wi*gameConst.N+wj] {
												if !gameState.plan[wi*gameConst.N+wj] {
													ok = false
													break
												}
											}
										}
										// path確保可能？
										for pIndex := range CASTLE_PATH[pattern] {
											di, dj := CASTLE_PATH[pattern][pIndex][0], CASTLE_PATH[pattern][pIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											wi := gameConst.Ti + di
											wj := gameConst.Tj + dj
											if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
												ok = false
												break
											}
											if gameConst.B[wi*gameConst.N+wj] {
												ok = false
												break
											}
											if gameState.Revealed[wi*gameConst.N+wj] {
												if gameState.plan[wi*gameConst.N+wj] {
													ok = false
													break
												}
											}
										}
										if !ok {
											continue
										}
										// この方向へのcastle設置を考える
										// revealedとこのcastle配置で到達可能性に問題がないか確認する
										currentIsWall := make([]bool, gameConst.N*gameConst.N)
										newPlan := make([]bool, gameConst.N*gameConst.N)
										for ij := range currentIsWall {
											// 初期壁、もう見えている木は確定
											currentIsWall[ij] = gameConst.B[ij] || (gameState.plan[ij] && gameState.Revealed[ij])
										}
										for ij := range newPlan {
											newPlan[ij] = gameState.plan[ij]
										}
										// CASTLE_WALLS[pattern]に合わせて壁追加
										for wIndex := range CASTLE_WALLS[pattern] {
											di, dj := CASTLE_WALLS[pattern][wIndex][0], CASTLE_WALLS[pattern][wIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											wi := gameConst.Ti + di
											wj := gameConst.Tj + dj
											if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
												continue
											}
											currentIsWall[wi*gameConst.N+wj] = true
											newPlan[wi*gameConst.N+wj] = true
										}
										for pIndex := range CASTLE_PATH[pattern] {
											di, dj := CASTLE_PATH[pattern][pIndex][0], CASTLE_PATH[pattern][pIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											pi := gameConst.Ti + di
											pj := gameConst.Tj + dj
											if pi < 0 || pi >= gameConst.N || pj < 0 || pj >= gameConst.N {
												continue
											}
											newPlan[pi*gameConst.N+pj] = false
										}
										//
										gs := NewGridSolver(gameConst.N, currentIsWall, gameConst.Neighbors, gameConst.N/2, gameConst.FlowerIJ)
										distances := gs.bfsFrom(gameState.Pi*gameConst.N + gameState.Pj)
										gs2 := NewGridSolver(gameConst.N, newPlan, gameConst.Neighbors, gameConst.N/2, gameConst.FlowerIJ)
										distances2 := gs2.bfsFrom(gameState.Pi*gameConst.N + gameState.Pj)
										fmt.Fprintf(os.Stderr, "pattern: %d, rot: %d, flip: %d, distnce: %d, distance2: %d\n", pattern, rot, flip, distances[gameConst.FlowerIJ], distances2[gameConst.FlowerIJ])
										if distances[gameConst.Ti*gameConst.N+gameConst.Tj] == -1 {
											// 到達不可能なのでスキップ
											continue
										}
										value := 0.0
										for ij := range distances {
											if distances[ij] != -1 {
												value += 500
											}
										}
										for ij := range distances2 {
											if distances2[ij] != -1 {
												value += 20
											}
										}
										value += 1 * float64(distances[gameConst.Ti*gameConst.N+gameConst.Tj])

										// この状態で、四方向に見えるものを確認する
										revealed := make([]bool, gameConst.N*gameConst.N)
										for ij := range revealed {
											revealed[ij] = gameState.Revealed[ij]
										}
										for d2 := 0; d2 < 4; d2++ {
											ij := gameState.Pi*gameConst.N + gameState.Pj
											for {
												ij = gameConst.Neighbors[ij][d2]
												if ij == -1 {
													break
												}
												revealed[ij] = true
												if currentIsWall[ij] {
													break
												}
											}
										}
										c := 0
										for wIndex := 0; wIndex < 2; wIndex++ {
											di, dj := CASTLE_WALLS[pattern][wIndex][0], CASTLE_WALLS[pattern][wIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											wi := gameConst.Ti + di
											wj := gameConst.Tj + dj
											if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
												continue
											}
											if revealed[wi*gameConst.N+wj] {
												c++
											}
										}
										if c < 2 {
											// 直結していないかを確認
											disti := abs(gameState.Pi - gameConst.Ti)
											distj := abs(gameState.Pj - gameConst.Tj)
											if distances[gameConst.Ti*gameConst.N+gameConst.Tj] <= disti+distj+2 {
												value -= 10000 * float64(disti+distj+3-distances[gameConst.Ti*gameConst.N+gameConst.Tj])

											}

										}
										value += float64(c * 10000)
										// if c == 2 {
										// 	value += 20000
										// }
										for wIndex := range CASTLE_WALLS[pattern] {
											di, dj := CASTLE_WALLS[pattern][wIndex][0], CASTLE_WALLS[pattern][wIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											wi := gameConst.Ti + di
											wj := gameConst.Tj + dj
											if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
												continue
											}
											// if gameState.plan[wi*gameConst.N+wj] {
											// 	value += 5
											// }
										}
										if !currentIsWall[playerij] && revealed[playerij] {
											if c < 2 {
												value -= 50000
											}
										}
										value += float64(CASTLE_PREFERENCE[pattern])
										if pattern == 2 {
											bonus := false
											wIndex := len(CASTLE_WALLS[pattern]) - 1
											di, dj := CASTLE_WALLS[pattern][wIndex][0], CASTLE_WALLS[pattern][wIndex][1]
											di, dj = transformIJ(di, dj, rot, flip == 1)
											wi := gameConst.Ti + di
											wj := gameConst.Tj + dj
											if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
												bonus = true
											} else {
												if revealed[wi*gameConst.N+wj] && currentIsWall[wi*gameConst.N+wj] {
													bonus = true
												}
											}
											if bonus {
												value += float64(CASTLE_PREFERENCE[0] - CASTLE_PREFERENCE[2])
											}
										}

										fmt.Fprintln(os.Stderr, "castle_candidate", "pattern:", pattern, "rot:", rot, "flip:", flip, "value:", value, c)
										if value > bestValue {
											bestValue = value
											bestRot = rot
											bestFlip = flip
											bestC = c
											bestPattern = pattern
										}
									}
								}
							}

							if bestC == 2 {
								// gameState.Stabilized = true
							}
							for ij := range gameState.fixed {
								gameState.fixed[ij] = false
							}
							// CASTLE_WALLS[bestPattern]に合わせて壁追加
							for wIndex := range CASTLE_WALLS[bestPattern] {
								di, dj := CASTLE_WALLS[bestPattern][wIndex][0], CASTLE_WALLS[bestPattern][wIndex][1]
								di, dj = transformIJ(di, dj, bestRot, bestFlip == 1)
								wi := gameConst.Ti + di
								wj := gameConst.Tj + dj
								if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
									continue
								}

								gameState.plan[wi*gameConst.N+wj] = true
								gameState.fixed[wi*gameConst.N+wj] = true
							}
							fmt.Fprintln(os.Stderr, "change castle", "rot:", bestRot, "flip:", bestFlip, "value:", bestValue, bestC)
							first = false
							rerun = true
							break
						}
						if gameConst.B[playerij] {
							// 既に木が確定しているのでスルー
							break
						}
						if gameState.plan[playerij] {
							// 関節点でなければ採用
							ap := NewGridArticulation(gameConst.N, fixedIsWall, gameConst.Neighbors)
							// 知りたいセル ij が関節点か？
							isArticulation := ap.IsArticulation(playerij)
							if isArticulation {
								// 経路が短くなってしまうようなところはarticulationでもスキップしない
								gs := NewGridSolver(gameConst.N, gameState.plan, gameConst.Neighbors, gameState.Pi*gameConst.N+gameState.Pj, gameConst.FlowerIJ)
								distances := gs.bfsFrom(gameState.Pi*gameConst.N + gameState.Pj)
								for d1 := 0; d1 < 4; d1++ {
									ij1 := gameConst.Neighbors[playerij][d1]
									if ij1 == -1 {
										continue
									}
									if distances[ij1] == -1 {
										continue
									}
									for d2 := 0; d2 < 4; d2++ {
										ij2 := gameConst.Neighbors[playerij][d2]
										if ij2 == -1 {
											continue
										}
										if distances[ij2] == -1 {
											continue
										}
										if distances[ij2] > distances[ij1]+6 {
											// 目的地へ到達不可能にしないときのみ、取り除かない
											fixedIsWall2 := make([]bool, gameConst.N*gameConst.N)
											for i := range fixedIsWall2 {
												fixedIsWall2[i] = fixedIsWall[i]
											}
											fixedIsWall2[playerij] = true
											gs2 := NewGridSolver(gameConst.N, fixedIsWall2, gameConst.Neighbors, gameState.Pi*gameConst.N+gameState.Pj, gameConst.FlowerIJ)
											distances2 := gs2.bfsFrom(gameState.Pi*gameConst.N + gameState.Pj)
											if distances2[gameConst.FlowerIJ] != -1 {
												isArticulation = false
											}
										}
									}
								}
							}
							if !isArticulation {
								// castle周りの可視化を妨げているなら関節点でなくても取り除く
								critical := false
								for d1 := 0; d1 < 4; d1++ {
									nij := gameConst.Neighbors[gameConst.FlowerIJ][d1]
									if nij == -1 {
										continue
									}
									related := false
									ok := false

									for d2 := 0; d2 < 4; d2++ {
										nnij := gameConst.Neighbors[nij][d2]
										if nnij == playerij {
											related = true
											continue
										}
										if nnij == -1 {
											continue
										}
										if gameConst.B[nnij] {
											continue
										}
										if nnij == gameConst.FlowerIJ {
											continue
										}
										if !gameState.plan[nnij] || !gameState.Revealed[nnij] {
											ok = true
										}
									}
									// ok = false
									if related && !ok {
										critical = true
										break
									}
								}
								if critical {
									isArticulation = true

									fmt.Fprintf(os.Stderr, "(%d, %d) is not articulation point, but criticalpoint.\n", playerij/gameConst.N, playerij%gameConst.N)
								}
							}
							if isArticulation && !gameState.fixed[playerij] {
								fmt.Fprintln(os.Stderr, playerij/gameConst.N, playerij%gameConst.N, "is articulation point. skip")
								gameState.plan[playerij] = false
							} else {
								best.X = append(best.X, playerij/gameConst.N)
								best.Y = append(best.Y, playerij%gameConst.N)
								fixedIsWall[playerij] = true
								break
							}
						}

					}
					if rerun {
						break
					}
				}
				if !rerun {
					break
				}
			}
		}
	} else {
		if gameState.Turn > 1 {
			best.finish = true
		} else {
			// 全部決める場合
			for ij := range gameState.plan {
				if gameState.plan[ij] && !gameConst.B[ij] {
					best.X = append(best.X, ij/gameConst.N)
					best.Y = append(best.Y, ij%gameConst.N)
				}
			}

		}

		// fmt.Fprintln(os.Stderr, bestPath)
	}

	return best, bestScore, loop
}

func evaluateState(gameState *GameState, gameConst *GameConst) float64 {
	return 0
	value := 0.0
	isWall := make([]bool, gameConst.N*gameConst.N)
	for i := range isWall {
		isWall[i] = gameState.plan[i]
	}
	// ゴール周りの特殊仕様を処理しておく
	// for d := 0; d < 4; d++ {
	// 	nij := gameConst.Neighbors[gameConst.FlowerIJ][d]
	// 	if nij == -1 {
	// 		continue
	// 	}
	// 	if !gameState.fixed[nij] {
	// 		isWall[nij] = false
	// 	}
	// }
	for _, ij := range gameState.ShortestPath {
		isWall[ij] = false
	}
	gs := NewGridSolver(gameConst.N, isWall, gameConst.Neighbors, gameConst.N/2, gameConst.FlowerIJ)
	path := gs.Path(gameConst.FlowerIJ)
	revealed := make([]bool, gameConst.N*gameConst.N)
	revealed[gameConst.N/2] = true
	fmt.Fprintln(os.Stderr, path)
	for pindex, ij := range path {
		// pathから四方向に伸ばして見えるマスをチェックする
		ok := true // flowerが見えてしまうか
		for d := 0; d < 4; d++ {
			nij := ij
			for {
				nij = gameConst.Neighbors[nij][d]
				if nij == -1 {
					break
				}
				if nij == gameConst.FlowerIJ {
					ok = false
					break
				}
				if isWall[nij] {
					break
				}
			}
		}
		if !ok {
			// flowerが見えてしまうターンなので算入しない
			fmt.Fprintln(os.Stderr, pindex, len(path), len(gameState.ShortestPath))
			break
		}
		for d := 0; d < 4; d++ {
			nij := ij
			for {
				nij = gameConst.Neighbors[nij][d]
				if nij == -1 {
					break
				}
				revealed[nij] = true
				if isWall[nij] {
					break
				}
			}
		}
	}
	// violationCount := 0
	// for ij := range gameState.fixed {
	// 	if gameState.fixed[ij] {
	// 		if !revealed[ij] {
	// 			violationCount++
	// 		}
	// 	}
	// }
	// if violationCount == 0 {
	// 	gameState.Stabilized = true
	// }
	// _ = violationCount
	// fmt.Fprintf(os.Stderr, "violation: %d\n", violationCount)
	dist := 0
	s := 0
	for {
		if !gameConst.B[s] {
			break
		}
		s += gameConst.N
	}
	t := gameConst.N - 1
	for {
		if !gameConst.B[t] {
			break
		}
		t += gameConst.N
	}
	gs2 := NewGridSolver(gameConst.N, isWall, gameConst.Neighbors, s, gameConst.FlowerIJ)
	path = gs2.Path(t)
	dist = len(path)
	fmt.Fprintf(os.Stderr, "dist: %d\n", dist)
	// value = -float64(violationCount)
	value = float64(dist)
	return value
}

func (gameState *GameState) MakePlan(gameConst *GameConst, randomize bool) {
	isWall := make([]bool, gameConst.N*gameConst.N)
	for ij := range isWall {
		isWall[ij] = gameConst.B[ij]
	}
	trapwidth := 6
	if gameConst.N > 22 {
		trapwidth = 7
	}
	// if gameConst.N > 30 {
	// 	trapwidth = 8
	// }

	isWall, embedded, rot, flip := EmbedCastle(gameConst.N, isWall, gameConst.Neighbors, [2]int{gameConst.Ti, gameConst.Tj}, trapwidth, randomize, gameConst.Random)
	for ij := range gameState.fixed {
		gameState.fixed[ij] = false
	}
	if embedded {
		for wIndex := range CASTLE_WALLS[0] {
			di, dj := CASTLE_WALLS[0][wIndex][0], CASTLE_WALLS[0][wIndex][1]
			di, dj = transformIJ(di, dj, rot, flip == 1)
			wi := gameConst.Ti + di
			wj := gameConst.Tj + dj
			if wi < 0 || wi >= gameConst.N || wj < 0 || wj >= gameConst.N {
				continue
			}

			gameState.fixed[wi*gameConst.N+wj] = true
		}
	}
	// 四近傍とも埋めておく
	for d1 := 0; d1 < 4; d1++ {
		nij := gameConst.Neighbors[gameConst.Ti*gameConst.N+gameConst.Tj][d1]
		if nij == -1 {
			continue
		}
		isWall[nij] = true
	}
	forbidden := make([]bool, gameConst.N*gameConst.N)
	forbidden[gameConst.Si*gameConst.N+gameConst.Sj] = true
	forbidden[gameConst.Ti*gameConst.N+gameConst.Tj] = true
	length := gameConst.N - 5

	if trapwidth == 8 {
		isWall = PlaceStraightWallAndKeepConnectivity(gameConst.N, isWall, gameConst.Neighbors, length, 0, 0, 8, forbidden)
		isWall = PlaceStraightWallAndKeepConnectivity(gameConst.N, isWall, gameConst.Neighbors, length, 0, 0, gameConst.N-1-8, forbidden)
		te := NewTrapEmbedder(gameConst.N, gameConst.Neighbors)
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 0}, 2, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 7}, 1, 0, true)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 1}, 1, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 6}, 1, 0, true)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 0}, 0, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 7}, 0, 0, true)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 0}, 2, 0, true)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 7}, 1, 0, false)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 1}, 1, 0, true)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 6}, 1, 0, false)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 0}, 0, 0, true)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 7}, 0, 0, false)
		}
	} else if trapwidth == 7 {
		isWall = PlaceStraightWallAndKeepConnectivity(gameConst.N, isWall, gameConst.Neighbors, length, 0, 0, 7, forbidden)
		isWall = PlaceStraightWallAndKeepConnectivity(gameConst.N, isWall, gameConst.Neighbors, length, 0, 0, gameConst.N-1-7, forbidden)
		te := NewTrapEmbedder(gameConst.N, gameConst.Neighbors)
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 0}, 1, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 6}, 1, 0, true)
		}
		for i := 0; i < length; i++ {

			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 1}, 1, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 5}, 1, 0, true)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 0}, 0, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 6}, 0, 0, true)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1}, 1, 0, true)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 6}, 1, 0, false)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 1}, 1, 0, true)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 5}, 1, 0, false)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 0}, 0, 0, true)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 6}, 0, 0, false)
		}
	} else if trapwidth == 6 {
		isWall = PlaceStraightWallAndKeepConnectivity(gameConst.N, isWall, gameConst.Neighbors, length, 0, 0, 6, forbidden)
		isWall = PlaceStraightWallAndKeepConnectivity(gameConst.N, isWall, gameConst.Neighbors, length, 0, 0, gameConst.N-1-6, forbidden)
		te := NewTrapEmbedder(gameConst.N, gameConst.Neighbors)
		// for i := 0; i < length; i++ {
		// 	isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 0}, 1, 0, false)
		// }
		// for i := 0; i < length; i++ {
		// 	isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 5}, 0, 0, true)
		// }
		// for i := 0; i < length; i++ {
		// 	isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 5}, 0, 0, false)
		// }
		// for i := 0; i < length; i++ {
		// 	isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1}, 1, 0, true)
		// }
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 0}, 1, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, 5}, 1, 0, true)
		}
		for i := 0; i < length; i++ {
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1 - 5}, 1, 0, false)
			isWall, _ = te.EmbedTrap(isWall, forbidden, [2]int{i, gameConst.N - 1}, 1, 0, true)
		}
	}

	// ws := NewWallSolver(gameConst.N, gameConst.B, gameConst.Neighbors, gameConst.Si*gameConst.N+gameConst.Sj, gameConst.Ti*gameConst.N+gameConst.Tj)
	s := 0
	for {
		if !gameConst.B[s] {
			break
		}
		s += gameConst.N
	}
	t := gameConst.N - 1
	for {
		if !gameConst.B[t] {
			break
		}
		t += gameConst.N
	}
	ws := NewWallSolver(gameConst.N, isWall, gameConst.Neighbors, s, t, trapwidth)
	ws.f1 = gameConst.Si*gameConst.N + gameConst.Sj
	ws.f2 = gameConst.Ti*gameConst.N + gameConst.Tj
	ws.TimeLimit = 1500 * time.Millisecond
	ws.T0 = 2.0
	ws.Tend = 0.01
	ws.pAdd = 0.80
	ws.WallPenalty = 0.05
	ws.Seed = 1234567 // 任意
	// ws.EnableDebug(100 * time.Millisecond)
	isWall = ws.Solve() // out が出力 isWall（初期壁 ∪ 最良追加壁）

	// ps := NewPathSolver(gameConst.N, gameConst.B, gameConst.Neighbors, gameConst.Si*gameConst.N+gameConst.Sj, gameConst.Ti*gameConst.N+gameConst.Tj, true, 900*time.Millisecond)
	// isWall = ps.Solve()
	center := gameConst.Ti*gameConst.N + gameConst.Tj
	if gameConst.Tj <= trapwidth || gameConst.Tj >= gameConst.N-1-trapwidth {
		center = (gameConst.N/2)*gameConst.N + (gameConst.N / 2)
	}
	gs := NewGridSolver(gameConst.N, isWall, gameConst.Neighbors, gameConst.N/2, center)
	paths := make([][]int, 2)
	paths[0] = gs.Path(s)
	paths[1] = gs.Path(t)
	sp := gs.Path(gameConst.FlowerIJ)
	gameState.ShortestPath = sp
	onSP := make([]bool, gameConst.N*gameConst.N)
	for pindex := range paths {
		for i := range paths[pindex] {
			ij := paths[pindex][i]
			onSP[ij] = true
			// if ij%gameConst.N <= trapwidth {
			// 	break
			// }
			// if ij%gameConst.N >= gameConst.N-1-trapwidth {
			// 	break
			// }
		}
	}
	for pindex := range paths {
		for i := range paths[pindex] {
			ij := paths[pindex][i]
			if ij%gameConst.N <= trapwidth {
				break
			}
			if ij%gameConst.N >= gameConst.N-1-trapwidth {
				break
			}
			for d := 0; d < 4; d++ {
				nij := gameConst.Neighbors[ij][d]
				if nij == -1 {
					continue
				}
				if !onSP[nij] {
					isWall[nij] = true
				}
			}
		}
	}

	// 非連結部分をすべて木にする
	gs2 := NewGridSolver(gameConst.N, isWall, gameConst.Neighbors, gameConst.N/2, gameConst.FlowerIJ)
	distances := gs2.bfsFrom(gameConst.N / 2)
	for ij := range distances {
		if distances[ij] == -1 && ij != gameConst.FlowerIJ {
			isWall[ij] = true
		}
	}
	gameState.plan = isWall
}

// ==== 公開関数 ====
//
// EmbedCastle は、CASTLE_WALLS[0] を核に CASTLE_PATH[0] を一周分だけ渦状に拡張し、
// その外側に支え壁を自動生成してから、左回り/右回り × 8変換（回転×反転）で
// 埋め込みを試みる。成功案が複数あるときは、（拡張後の）CASTLE_PATH[0] の末尾の
// 変換先 i（行）が最大の案を採用する。
// 連結条件は「空きマスの連結成分数が（埋め込み前から）不変」。
func EmbedCastle(N int, isWall []bool, neighbors [][4]int, castlePos [2]int, trapwidth int, randomize bool, rng *rand.Rand) ([]bool, bool, int, int) {
	size := N * N
	if len(isWall) != size || len(neighbors) != size {
		return isWall, false, 0, 0
	}
	i0, j0 := castlePos[0], castlePos[1]
	if i0 < 0 || i0 >= N || j0 < 0 || j0 >= N {
		return isWall, false, 0, 0
	}
	if len(CASTLE_PATH[0]) < 2 {
		// 末尾方向が決められないため最低2点は必要
		return isWall, false, 0, 0
	}

	// 埋め込み前の空き連結成分数
	baseComp := countEmptyComponents(isWall, neighbors)

	type candidate struct {
		walls [][2]int // 相対座標（元の WALLS + 生成された外壁）
		path  [][2]int // 相対座標（元の PATH + 渦巻き拡張）
	}
	var cands []candidate

	// 生成に失敗した場合は、拡張なしで試す（後方互換）
	if len(cands) == 0 {
		cands = append(cands, candidate{walls: safeCopy2(CASTLE_WALLS[0]), path: safeCopy2(CASTLE_PATH[0])})
	}

	// ベスト案の選定
	var bestWalls []bool
	bestFound := false
	bestValue := -float64(INF)
	br := 0
	bf := 0

	for _, cand := range cands {
		for rot := 0; rot < 4; rot++ {
			for flip := 0; flip < 2; flip++ {
				newWalls, ok, lastI, lastJ := tryTransformKeepCompCountParam(
					N, isWall, neighbors, i0, j0, rot, flip == 1,
					cand.walls, cand.path, baseComp,
				)
				if !ok {
					continue
				}
				value := float64(4*AbsInt(lastJ-N/2) + lastI + ((rot)%2)*100)
				_ = lastI
				_ = lastJ
				if rot == 0 && flip == 0 {
					if castlePos[1] > N-1-(trapwidth+2) {
						value += 10000
					}
					if castlePos[1] < trapwidth-2 {
						value += 10000
					}
				}
				if rot == 0 && flip == 1 {
					if castlePos[1] < trapwidth+2 {
						value += 10000
					}
					if castlePos[1] > N-1-(trapwidth-2) {
						value += 10000
					}
				}
				if rot == 3 && flip == 0 {
					if castlePos[1] >= N-1-(trapwidth) {
						value += 10000
					}
					if castlePos[1] < trapwidth+2 {
						value += 10000
					}

				}
				if rot == 1 && flip == 1 {
					if castlePos[1] <= trapwidth {
						value += 10000
					}
					if castlePos[1] > N-1-(trapwidth+2) {
						value += 10000
					}
				}
				if randomize {
					value = rng.Float64()
				}
				fmt.Fprintln(os.Stderr, rot, flip, value, lastI, lastJ)

				if !bestFound || value > bestValue {
					bestWalls = newWalls
					bestValue = value
					br = rot
					bf = flip
					bestFound = true
				}
			}
		}
	}

	if bestFound {
		fmt.Fprintln(os.Stderr, "bestCastle:", br, bf, bestValue)
		return bestWalls, true, br, bf
	}
	return isWall, false, br, bf
}

// ==== ここから内部実装 ====

// --- 渦巻き拡張 & 外壁生成 ---
//
// 手順：
//  1. INNER = CASTLE_WALLS[0] を障害物集合（4連結）とみなす。
//  2. Pcand = INNER の 4 近傍にあるセル（INNER 自身は除く）。=> 外周1マス帯（“歩道候補”）
//  3. 末尾の PATH 点 s を始点に、左手法（ccw=true）/右手法（ccw=false）で Pcand 上を一周トレース。
//     （方向優先：CCWは Left→Straight→Right→Back、CWは Right→Straight→Left→Back）
//  4. 得られたサイクルから、始点 s を重複しないように除き「拡張 PATH」を作る。
//  5. トレースの各ステップで進行方向の“外側”法線側に壁セルを置き、「支え外壁」を作る。
//     ※ ただしリングが閉じて連結成分数を増加させないよう、始点側の外壁を1セル分だけ
//     敢えて置かずギャップを設ける。
func generateSpiralAndOuterWalls(
	innerWalls [][2]int,
	basePath [][2]int,
	ccw bool,
) (extPath [][2]int, outerWalls [][2]int, ok bool) {

	type P = [2]int
	add := func(a, b P) P { return P{a[0] + b[0], a[1] + b[1]} }

	// 方向ベクトル（N,E,S,W）
	dirs := [4]P{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}
	leftOf := func(d int) int { return (d + 3) % 4 }
	rightOf := func(d int) int { return (d + 1) % 4 }

	// セット構築
	inner := make(map[P]bool, len(innerWalls))
	for _, w := range innerWalls {
		inner[P{w[0], w[1]}] = true
	}
	// Pcand：INNERの4近傍
	pcand := make(map[P]bool)
	for w := range inner {
		for d := 0; d < 4; d++ {
			q := add(w, dirs[d])
			if !inner[q] {
				pcand[q] = true
			}
		}
	}
	if len(basePath) < 2 {
		return nil, nil, false
	}
	start := P{basePath[len(basePath)-1][0], basePath[len(basePath)-1][1]}
	prev := P{basePath[len(basePath)-2][0], basePath[len(basePath)-2][1]}
	if !pcand[start] {
		// 末尾が外周帯にない場合は拡張できない
		return nil, nil, false
	}

	// 初期進行方向（末尾2点から）
	di, dj := start[0]-prev[0], start[1]-prev[1]
	dir, okdir := deltaToDir(di, dj)
	if !okdir {
		// 近傍から適当に選択（E,S,W,N）
		for _, cand := range []int{1, 2, 3, 0} {
			nx := add(start, dirs[cand])
			if pcand[nx] {
				dir = cand
				okdir = true
				break
			}
		}
		if !okdir {
			return nil, nil, false
		}
	}

	// トレース（左手/右手法）
	cur := start
	startDir := dir
	path := []P{cur}
	stepDirs := []int{}             // 各ステップの進行方向
	maxSteps := 10*len(pcand) + 100 // 安全上限

	for steps := 0; steps < maxSteps; steps++ {
		var order [4]int
		if ccw {
			order = [4]int{leftOf(dir), dir, rightOf(dir), (dir + 2) % 4}
		} else {
			order = [4]int{rightOf(dir), dir, leftOf(dir), (dir + 2) % 4}
		}
		moved := false
		for _, nd := range order {
			nx := add(cur, dirs[nd])
			if pcand[nx] {
				cur = nx
				dir = nd
				path = append(path, cur)
				stepDirs = append(stepDirs, nd)
				moved = true
				break
			}
		}
		if !moved {
			// 行き止まり（通常ありえないが中断）
			break
		}
		// 始点＆方向一致で一周完了
		if cur == start && dir == startDir && len(path) > 1 {
			break
		}
	}
	// 正常にリング形成できていない場合は失敗
	if !(cur == start && dir == startDir && len(path) > 1) {
		return nil, nil, false
	}

	// 拡張 PATH：始点の重複を除く（start を含めず“延伸部分のみ”）
	// path は [start, ... , start] の形になっているので、[1 : len-1] を採用
	if len(path) < 2 {
		return nil, nil, false
	}
	ep := make([][2]int, 0, len(path)-2)
	for k := 1; k < len(path)-1; k++ {
		ep = append(ep, [2]int{path[k][0], path[k][1]})
	}

	// 外壁生成：各ステップで外側法線方向へ 1 マス
	// CCW なら進行方向の “右側” が外側、CW なら “左側” が外側
	outer := make(map[P]bool)
	for t := 0; t < len(stepDirs); t++ {
		stepDir := stepDirs[t]
		var norm int
		if ccw {
			norm = rightOf(stepDir)
		} else {
			norm = leftOf(stepDir)
		}
		at := path[t+1]           // このステップで到達したセル
		wc := add(at, dirs[norm]) // そのセルの外側
		// WALKWAY（Pcand）やINNER上に外壁は置かない
		if !pcand[wc] && !inner[wc] {
			outer[wc] = true
		}
	}
	// 連結成分を増やさないため、開始点の外側に「ギャップ」を作る（1個だけ除外）
	{
		var gapNorm int
		if ccw {
			gapNorm = rightOf(startDir)
		} else {
			gapNorm = leftOf(startDir)
		}
		gap := add(start, dirs[gapNorm])
		delete(outer, gap)
	}

	ow := make([][2]int, 0, len(outer))
	for p := range outer {
		ow = append(ow, [2]int{p[0], p[1]})
	}
	return ep, ow, true
}

// --- 1 変換（回転×反転）での埋め込み可否判定（成分数不変） ---
func tryTransformKeepCompCountParam(
	N int,
	isWall []bool,
	neighbors [][4]int,
	i0, j0 int,
	rot int,
	flip bool,
	relWalls [][2]int, // 相対：CASTLE_WALLS[0] + 外壁
	relPath [][2]int, // 相対：CASTLE_PATH[0] + 渦巻き拡張
	baseComp int,
) ([]bool, bool, int, int) {
	size := len(isWall)
	inBounds := func(i, j int) bool { return i >= 0 && i < N && j >= 0 && j < N }
	idx := func(i, j int) int { return i*N + j }

	// 壁配置（範囲内のみ有効化）と衝突セット
	wallIdxSet := make(map[int]struct{})
	for _, rc := range relWalls {
		di, dj := transformIJ(rc[0], rc[1], rot, flip)
		ii, jj := i0+di, j0+dj
		if inBounds(ii, jj) {
			wallIdxSet[idx(ii, jj)] = struct{}{}
		}
		// 範囲外の壁は“既に壁”扱い → 何もしない
	}

	// PATH 制約：範囲外不可、初期壁不可、今回置く壁とも衝突不可
	for _, rc := range relPath {
		di, dj := transformIJ(rc[0], rc[1], rot, flip)
		ii, jj := i0+di, j0+dj
		if !inBounds(ii, jj) {
			return nil, false, 0, 0
		}
		id := idx(ii, jj)
		if isWall[id] {
			return nil, false, 0, 0
		}
		if _, collide := wallIdxSet[id]; collide {
			return nil, false, 0, 0
		}
	}

	// 実際に壁を立てる
	newWalls := make([]bool, size)
	copy(newWalls, isWall)
	for id := range wallIdxSet {
		newWalls[id] = true
	}

	// 連結成分数が不変か
	newComp := countEmptyComponents(newWalls, neighbors)
	if newComp != baseComp {
		return nil, false, 0, 0
	}

	// タイブレーク用：拡張後 PATH の末尾
	var lastI int
	var lastJ int
	if len(relPath) > 0 {
		last := relPath[len(relPath)-1]
		di, dj := transformIJ(last[0], last[1], rot, flip)
		li, lj := i0+di, j0+dj
		if !inBounds(li, lj) {
			return nil, false, 0, 0
		}
		// lastI = 4*AbsInt(lj-N/2) + li + ((rot+1)%2)*100
		lastI = li
		lastJ = lj
	} else {
		lastI = 0
		lastJ = 0
	}

	return newWalls, true, lastI, lastJ
}

// ---- 共通ユーティリティ ----

func transformIJ(di, dj, rot int, flip bool) (int, int) {
	// flip: 垂直軸反転（j → -j）
	if flip {
		dj = -dj
	}
	rot = ((rot % 4) + 4) % 4
	for k := 0; k < rot; k++ {
		// 90度時計回り: (di, dj) -> (-dj, di)
		di, dj = dj, -di
	}
	return di, dj
}

// (di,dj) ∈ {(-1,0),(0,1),(1,0),(0,-1)} を方向インデックス化
func deltaToDir(di, dj int) (int, bool) {
	switch {
	case di == -1 && dj == 0:
		return 0, true // N
	case di == 0 && dj == 1:
		return 1, true // E
	case di == 1 && dj == 0:
		return 2, true // S
	case di == 0 && dj == -1:
		return 3, true // W
	default:
		return 0, false
	}
}

// 空き（false）の連結成分数
func countEmptyComponents(walls []bool, neighbors [][4]int) int {
	size := len(walls)
	visited := make([]bool, size)
	comps := 0
	for s := 0; s < size; s++ {
		if walls[s] || visited[s] {
			continue
		}
		comps++
		q := []int{s}
		visited[s] = true
		for h := 0; h < len(q); h++ {
			u := q[h]
			for _, v := range neighbors[u] {
				if v < 0 || visited[v] || walls[v] {
					continue
				}
				visited[v] = true
				q = append(q, v)
			}
		}
	}
	return comps
}

func safeCopy2(a [][2]int) [][2]int {
	out := make([][2]int, len(a))
	copy(out, a)
	return out
}

/********** メトリクス出力（stderr, localtest がパース） **********/
func emitMetrics(score, loop int) {
	// 提出環境では通常 stderr は無視される前提。localtest のみ有効化。
	if os.Getenv("LOCAL_TEST") == "1" {
		fmt.Fprintf(os.Stderr, "METRICS score=%d loop=%d\n", score, loop)
	}
}

/********** main（標準入出力のみ） **********/
func main() {
	// defer profile.Start(profile.ProfilePath(".")).Stop()

	// 入力スキャナ
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1<<20), 1<<20)
	scanner.Split(bufio.ScanWords)

	// 乱数シード：環境変数 SEED があれば優先（再現性）
	seed := time.Now().UnixNano()
	if s := os.Getenv("SEED"); s != "" {
		if v, err := strconv.ParseInt(s, 10, 64); err == nil {
			seed = v
		}
	}

	// 出力（必ず flush する）
	writer := bufio.NewWriter(os.Stdout)
	defer writer.Flush()

	gameConst := NewGameConst(seed)
	gameConst.SetScanner(scanner)
	GetInputs(gameConst)

	gameState := NewGameState(gameConst)
	solver := NewSolver(TIME_LIMIT)

	// 各ターンのインタラクション
	totalScore := 0
	totalLoop := 0

	for {
		done, ok := gameState.ReadTurn(gameConst)
		if !ok {
			// EOF など異常終了時は抜ける
			break
		}
		if done {
			// 花に到達 → プログラム終了（このターンは出力不要）
			break
		}

		act, score, loop := solver.Solve(gameState, gameConst)
		totalScore += score
		totalLoop += loop

		// 提出要件：1ターン分の出力 + 改行 + flush
		out := FormatAction(act, gameState, gameConst)
		fmt.Fprintln(writer, out)
		writer.Flush()
		if act.finish {
			break
		}
	}

	// localtest 向けメトリクス
	emitMetrics(totalScore, totalLoop)
}

// 焼きなましここから

// ==================== Solver: SA + Non-Articulation (multi-tier) + Parent-Count Dynamic BFS + Perf ====================

type wallSolver struct {
	N         int
	BaseWall  []bool
	Neighbors [][4]int
	Start     int
	End       int
	f1        int
	f2        int
	trapWidth int

	// Annealing
	TimeLimit   time.Duration
	Seed        uint64
	T0, Tend    float64
	pAdd        float64
	WallPenalty float64

	// Debug
	debugEnabled  bool
	debugInterval time.Duration
	stats         SAStats

	// State
	added      []bool
	blocked    []bool
	bestAdded  []bool
	posInAdded []int
	addedList  []int

	distS []int16
	distT []int16
	parS  []uint8
	parT  []uint8

	queue    []int // reusable queue
	evalDist []int16

	// invalid region (add side)
	inv      []bool
	invTouch []int

	// validation buffers
	dbgFullS []int16
	dbgFullT []int16
	dbgVis   []bool

	// SP cache
	spList  []int
	spDirty bool

	// 3x3 sufficient tables
	noInc3x3  [512]bool // 「中心を塞いでも3x3内のCC数が増えない」→ 非関節 充分
	nbConn3x3 [512]bool // 「3x3(中心除去)で中心の4近傍が同一成分」→ 非関節 充分

	// 全域BFSの訪問スタンプ（ゼロクリア不要）
	vis      []int32
	visStamp int32

	// 再計算集合構築用（スタンプ＆再利用バッファ）
	tmpSeen      []int32
	tmpSeenStamp int32
	bufRecalc    []int
	bufChanged   []int

	// 初期オープン成分数（検証では cur ≤ base を期待）
	baseOpenCC int

	rng rng64
}

type SAStats struct {
	Iterations int

	ProposalsAdd    int
	ProposalsRemove int

	AcceptedAdd             int
	AcceptedRemove          int
	AcceptedAddOnSP         int
	AcceptedAddOffSP        int
	AcceptedAddBy3x3        int
	AcceptedAddByArticBFS   int
	RejectedAddArtic        int
	RejectedAddThreshold    int
	RemoveAcceptedByReach1  int
	RemoveAcceptedByApprox  int
	RejectedRemoveThreshold int

	ArticBFSCalls int

	DistRepairAddS    int
	DistRepairAddT    int
	DistRepairRemoveS int
	DistRepairRemoveT int

	BestScore  float64
	BestDist   int
	BestWalls  int
	FinalScore float64
	FinalDist  int
	FinalWalls int

	TotalTime time.Duration
}

// --------- Public API ---------

func NewWallSolver(N int, isWall []bool, neighbors [][4]int, start, end int, trapwidth int) *wallSolver {
	ws := &wallSolver{
		N:           N,
		BaseWall:    make([]bool, len(isWall)),
		Neighbors:   neighbors,
		Start:       start,
		End:         end,
		TimeLimit:   100 * time.Millisecond,
		Seed:        1,
		T0:          2.0,
		Tend:        0.02,
		pAdd:        0.70,
		WallPenalty: 0.10,
		trapWidth:   trapwidth,
	}
	copy(ws.BaseWall, isWall)
	ws.noInc3x3 = buildNoInc3x3Table()
	ws.nbConn3x3 = buildNbConn3x3Table()
	return ws
}

func (ws *wallSolver) EnableDebug(interval time.Duration) {
	ws.debugEnabled = true
	if interval <= 0 {
		interval = 100 * time.Millisecond
	}
	ws.debugInterval = interval
}

func (ws *wallSolver) Stats() SAStats { return ws.stats }

// --------- Solve ---------

func (ws *wallSolver) Solve() []bool {
	V := ws.N * ws.N

	// state
	ws.added = make([]bool, V)
	ws.blocked = make([]bool, V)
	for i := 0; i < V; i++ {
		ws.blocked[i] = ws.BaseWall[i]
	}
	ws.blocked[ws.Start], ws.blocked[ws.End] = false, false

	ws.bestAdded = make([]bool, V)
	ws.posInAdded = make([]int, V)
	for i := range ws.posInAdded {
		ws.posInAdded[i] = -1
	}
	ws.addedList = ws.addedList[:0]

	ws.distS = make([]int16, V)
	ws.distT = make([]int16, V)
	ws.parS = make([]uint8, V)
	ws.parT = make([]uint8, V)

	ws.queue = make([]int, 0, V)
	ws.evalDist = make([]int16, V)

	ws.inv = make([]bool, V)
	ws.invTouch = make([]int, 0, V)

	ws.dbgFullS = make([]int16, V)
	ws.dbgFullT = make([]int16, V)
	ws.dbgVis = make([]bool, V)

	ws.spList = ws.spList[:0]
	ws.spDirty = true

	// stamps & scratch buffers
	ws.vis = make([]int32, V)
	ws.visStamp = 1
	ws.tmpSeen = make([]int32, V)
	ws.tmpSeenStamp = 1
	ws.bufRecalc = make([]int, 0, 256)
	ws.bufChanged = make([]int, 0, 256)

	if ws.Seed == 0 {
		ws.Seed = 88172645463393265
	}
	ws.rng.s = ws.Seed

	ws.stats = SAStats{}
	tStart := time.Now()
	lastLog := tStart

	// initial BFS + parent counts
	ws.bfsFullFrom(ws.Start, ws.distS)
	ws.bfsFullFrom(ws.End, ws.distT)
	ws.buildParentCounts(ws.distS, ws.parS)
	ws.buildParentCounts(ws.distT, ws.parT)

	curDist := int(ws.distS[ws.End])
	if curDist < 0 {
		out := make([]bool, V)
		copy(out, ws.BaseWall)
		out[ws.Start], out[ws.End] = false, false
		return out
	}
	ws.rebuildSPList(curDist)

	ws.baseOpenCC = ws.countOpenCC()

	curScore := float64(curDist) - ws.WallPenalty*float64(len(ws.addedList))
	ws.stats.BestScore = curScore
	ws.stats.BestDist = curDist
	ws.stats.BestWalls = len(ws.addedList)
	copy(ws.bestAdded, ws.added)

	deadline := time.Now().Add(ws.TimeLimit)

	for time.Now().Before(deadline) {
		T := ws.temperature(deadline)
		u := ws.rng.Float64()
		if u <= 0 {
			u = 1e-12
		}
		thr := math.Log(u) * T
		ws.stats.Iterations++

		doAdd := ws.rng.Float64() < ws.pAdd
		if !doAdd && len(ws.addedList) == 0 {
			doAdd = true
		}

		if doAdd {
			// propose add
			idx, ok := ws.sampleAddCandidate()

			if !ok || idx == ws.Start || idx == ws.End || idx == ws.f1 || idx == ws.f2 || ws.blocked[idx] {
				goto periodic_log
			}
			ws.stats.ProposalsAdd++

			onSP := (ws.distS[idx] >= 0 && ws.distT[idx] >= 0 &&
				int(ws.distS[idx])+int(ws.distT[idx]) == curDist)

			// score gate
			if !onSP {
				if -ws.WallPenalty < thr {
					ws.stats.RejectedAddThreshold++
					goto periodic_log
				}
			}

			// non-articulation check (multi-tier)
			nonArtic, usedBFS := ws.nonArticulation(idx)
			if !nonArtic {
				ws.stats.RejectedAddArtic++
				goto periodic_log
			}
			if usedBFS {
				ws.stats.AcceptedAddByArticBFS++
			} else {
				ws.stats.AcceptedAddBy3x3++
			}

			// (debug) CC should not increase
			var ccBefore int
			if ws.debugEnabled {
				ccBefore = ws.countOpenCC()
			}

			// apply
			ws.addWall(idx)
			cs := ws.repairAfterAddBlockWithParents(idx, ws.distS, ws.parS)
			ct := ws.repairAfterAddBlockWithParents(idx, ws.distT, ws.parT)
			ws.stats.DistRepairAddS += cs
			ws.stats.DistRepairAddT += ct

			if ws.debugEnabled {
				ccAfter := ws.countOpenCC()
				if ccAfter > ccBefore {
					x, y := idx%ws.N, idx/ws.N
					fmt.Fprintf(os.Stderr, "           [ASSERT][ADD] CC increased at (%d,%d) id=%d: before=%d after=%d\n",
						x, y, idx, ccBefore, ccAfter)
				}
			}

			ws.stats.AcceptedAdd++
			if onSP {
				ws.stats.AcceptedAddOnSP++
			} else {
				ws.stats.AcceptedAddOffSP++
			}

			curDist = int(ws.distS[ws.End])
			ws.spDirty = true

			curScore = float64(curDist) - ws.WallPenalty*float64(len(ws.addedList))
			if curScore > ws.stats.BestScore {
				ws.stats.BestScore = curScore
				ws.stats.BestDist = curDist
				ws.stats.BestWalls = len(ws.addedList)
				copy(ws.bestAdded, ws.added)
			}

		} else {
			// propose remove
			idx := ws.sampleRemoveCandidate()
			if idx < 0 {
				goto periodic_log
			}
			ws.stats.ProposalsRemove++

			// reach count
			reachCnt := 0
			nb := ws.Neighbors[idx]
			for k := 0; k < 4; k++ {
				v := nb[k]
				if v >= 0 && !ws.blocked[v] && ws.distS[v] >= 0 {
					reachCnt++
				}
			}

			accepted := false
			if reachCnt <= 1 {
				accepted = true
				ws.stats.RemoveAcceptedByReach1++
			} else {
				minS, minT := 1<<30, 1<<30
				for k := 0; k < 4; k++ {
					a := nb[k]
					if a >= 0 && !ws.blocked[a] && ws.distS[a] >= 0 {
						if int(ws.distS[a]) < minS {
							minS = int(ws.distS[a])
						}
					}
				}
				for k := 0; k < 4; k++ {
					b := nb[k]
					if b >= 0 && !ws.blocked[b] && ws.distT[b] >= 0 {
						if int(ws.distT[b]) < minT {
							minT = int(ws.distT[b])
						}
					}
				}
				if minS < (1<<30) && minT < (1<<30) {
					newApprox := minS + 2 + minT
					deltaScore := float64(newApprox-curDist) + ws.WallPenalty
					if deltaScore >= thr {
						accepted = true
						ws.stats.RemoveAcceptedByApprox++
					} else {
						ws.stats.RejectedRemoveThreshold++
					}
				}
			}
			if !accepted {
				goto periodic_log
			}

			ws.stats.AcceptedRemove++

			ws.removeWall(idx)
			cs := ws.repairAfterRemoveBlockWithParents(idx, ws.distS, ws.parS)
			ct := ws.repairAfterRemoveBlockWithParents(idx, ws.distT, ws.parT)
			ws.stats.DistRepairRemoveS += cs
			ws.stats.DistRepairRemoveT += ct

			curDist = int(ws.distS[ws.End])
			ws.spDirty = true

			curScore = float64(curDist) - ws.WallPenalty*float64(len(ws.addedList))
			if curScore > ws.stats.BestScore {
				ws.stats.BestScore = curScore
				ws.stats.BestDist = curDist
				ws.stats.BestWalls = len(ws.addedList)
				copy(ws.bestAdded, ws.added)
			}
		}

	periodic_log:
		if ws.spDirty {
			ws.rebuildSPList(curDist)
			ws.spDirty = false
		}

		if ws.debugEnabled && time.Since(lastLog) >= ws.debugInterval {
			el := time.Since(tStart)
			acc := ws.stats.AcceptedAdd + ws.stats.AcceptedRemove
			accRate := 0.0
			if ws.stats.Iterations > 0 {
				accRate = float64(acc) / float64(ws.stats.Iterations) * 100.0
			}
			fmt.Fprintf(os.Stderr,
				"[+%4d ms] iters=%d acc=%.1f%% curScore=%.3f best=%.3f curDist=%d walls=%d | add: prop=%d acc=%d (onSP=%d offSP=%d) | rm: prop=%d acc=%d | articBFS=%d\n",
				int(el.Milliseconds()),
				ws.stats.Iterations, accRate, curScore, ws.stats.BestScore, curDist, len(ws.addedList),
				ws.stats.ProposalsAdd, ws.stats.AcceptedAdd, ws.stats.AcceptedAddOnSP, ws.stats.AcceptedAddOffSP,
				ws.stats.ProposalsRemove, ws.stats.AcceptedRemove,
				ws.stats.ArticBFSCalls,
			)
			fmt.Fprintf(os.Stderr,
				"           add-accept by3x3=%d byArticBFS=%d | add-reject artic=%d thr=%d | rm-accept reach<=1=%d approx=%d | ΔS(add/rm)=%d/%d ΔT(add/rm)=%d/%d\n",
				ws.stats.AcceptedAddBy3x3, ws.stats.AcceptedAddByArticBFS,
				ws.stats.RejectedAddArtic, ws.stats.RejectedRemoveThreshold,
				ws.stats.RemoveAcceptedByReach1, ws.stats.RemoveAcceptedByApprox,
				ws.stats.DistRepairAddS, ws.stats.DistRepairRemoveS,
				ws.stats.DistRepairAddT, ws.stats.DistRepairRemoveT,
			)
			ws.validateCurrentState(curDist)
			lastLog = time.Now()
		}
	}

	ws.stats.TotalTime = time.Since(tStart)
	ws.stats.FinalScore = curScore
	ws.stats.FinalDist = curDist
	ws.stats.FinalWalls = len(ws.addedList)

	if true {
		acc := ws.stats.AcceptedAdd + ws.stats.AcceptedRemove
		accRate := 0.0
		if ws.stats.Iterations > 0 {
			accRate = float64(acc) / float64(ws.stats.Iterations) * 100.0
		}
		fmt.Fprintf(os.Stderr, "\n==== SA Summary ====\n")
		fmt.Fprintf(os.Stderr, "time=%d ms  iters=%d  accept=%d (%.1f%%)\n",
			int(ws.stats.TotalTime.Milliseconds()), ws.stats.Iterations, acc, accRate)
		fmt.Fprintf(os.Stderr, "curScore=%.3f curDist=%d walls=%d | bestScore=%.3f bestDist=%d bestWalls=%d\n",
			ws.stats.FinalScore, ws.stats.FinalDist, ws.stats.FinalWalls,
			ws.stats.BestScore, ws.stats.BestDist, ws.stats.BestWalls)
		fmt.Fprintf(os.Stderr, "add: prop=%d acc=%d (onSP=%d offSP=%d)  by3x3=%d byArticBFS=%d  reject(artic=%d thr=%d)\n",
			ws.stats.ProposalsAdd, ws.stats.AcceptedAdd, ws.stats.AcceptedAddOnSP, ws.stats.AcceptedAddOffSP,
			ws.stats.AcceptedAddBy3x3, ws.stats.AcceptedAddByArticBFS,
			ws.stats.RejectedAddArtic, ws.stats.RejectedAddThreshold)
		fmt.Fprintf(os.Stderr, "rm : prop=%d acc=%d  accept(reach<=1=%d approx=%d)  reject(thr=%d)\n",
			ws.stats.ProposalsRemove, ws.stats.AcceptedRemove,
			ws.stats.RemoveAcceptedByReach1, ws.stats.RemoveAcceptedByApprox,
			ws.stats.RejectedRemoveThreshold)
		fmt.Fprintf(os.Stderr, "artic-check BFS calls=%d | ΔS(add/rm)=%d/%d  ΔT(add/rm)=%d/%d\n\n",
			ws.stats.ArticBFSCalls,
			ws.stats.DistRepairAddS, ws.stats.DistRepairRemoveS,
			ws.stats.DistRepairAddT, ws.stats.DistRepairRemoveT)

		ws.validateCurrentState(int(ws.distS[ws.End]))
	}

	out := make([]bool, V)
	for i := 0; i < V; i++ {
		out[i] = ws.BaseWall[i] || ws.bestAdded[i]
	}
	out[ws.Start], out[ws.End] = false, false
	return out
}

// ==================== Annealing helpers ====================

func (ws *wallSolver) temperature(deadline time.Time) float64 {
	if ws.TimeLimit <= 0 {
		return ws.Tend
	}
	left := time.Until(deadline)
	p := 1.0 - float64(left)/float64(ws.TimeLimit)
	if p < 0 {
		p = 0
	} else if p > 1 {
		p = 1
	}
	return ws.T0 * math.Pow(ws.Tend/ws.T0, p)
}

// SP 85% 優先
func (ws *wallSolver) sampleAddCandidate() (int, bool) {
	V := ws.N * ws.N
	useSP := len(ws.spList) > 0 && (ws.rng.Float64() < 0.85)
	if useSP {
		return ws.spList[ws.rng.Intn(len(ws.spList))], true
	}
	for t := 0; t < 16; t++ {
		i := ws.rng.Intn(V)
		if i == ws.Start || i == ws.End || ws.blocked[i] {
			continue
		}
		return i, true
	}
	for i := 0; i < V; i++ {
		if i == ws.Start || i == ws.End || ws.blocked[i] {
			continue
		}
		return i, true
	}
	return -1, false
}

func (ws *wallSolver) sampleRemoveCandidate() int {
	if len(ws.addedList) == 0 {
		return -1
	}
	return ws.addedList[ws.rng.Intn(len(ws.addedList))]
}

// ==================== Apply walls ====================

func (ws *wallSolver) addWall(i int) {
	if ws.added[i] {
		return
	}
	ws.added[i] = true
	ws.blocked[i] = true
	ws.posInAdded[i] = len(ws.addedList)
	ws.addedList = append(ws.addedList, i)
}
func (ws *wallSolver) removeWall(i int) {
	if !ws.added[i] {
		return
	}
	pos := ws.posInAdded[i]
	last := len(ws.addedList) - 1
	ws.addedList[pos] = ws.addedList[last]
	ws.posInAdded[ws.addedList[pos]] = pos
	ws.addedList = ws.addedList[:last]
	ws.posInAdded[i] = -1

	ws.added[i] = false
	ws.blocked[i] = ws.BaseWall[i]
	if i == ws.Start || i == ws.End {
		ws.blocked[i] = false
	}
}

func (ws *wallSolver) isOpen(i int) bool {
	if i < 0 {
		return false
	}
	if i == ws.Start || i == ws.End {
		return true
	}
	return !ws.blocked[i]
}

// ==================== 3x3 tables ====================

func buildNoInc3x3Table() [512]bool {
	var ok [512]bool
	neighbors := [9][4]int{
		{1, 3, -1, -1}, {0, 2, 4, -1}, {1, 5, -1, -1},
		{0, 4, 6, -1}, {1, 3, 5, 7}, {2, 4, 8, -1},
		{3, 7, -1, -1}, {4, 6, 8, -1}, {5, 7, -1, -1},
	}
	compCount := func(mask int) int {
		vis := [9]bool{}
		cc := 0
		for i := 0; i < 9; i++ {
			if ((mask>>i)&1) == 0 || vis[i] {
				continue
			}
			cc++
			var q [9]int
			h, t := 0, 0
			q[t] = i
			t++
			vis[i] = true
			for h < t {
				u := q[h]
				h++
				for _, v := range neighbors[u] {
					if v < 0 {
						continue
					}
					if ((mask>>v)&1) == 0 || vis[v] {
						continue
					}
					vis[v] = true
					q[t] = v
					t++
				}
			}
		}
		return cc
	}
	for m := 0; m < 512; m++ {
		if ((m >> 4) & 1) == 0 {
			ok[m] = true
			continue
		}
		ccBefore := compCount(m)
		ccAfter := compCount(m &^ (1 << 4))
		ok[m] = (ccAfter == ccBefore)
	}
	return ok
}

func buildNbConn3x3Table() [512]bool {
	var ok [512]bool
	neighbors := [9][4]int{
		{1, 3, -1, -1}, {0, 2, 4, -1}, {1, 5, -1, -1},
		{0, 4, 6, -1}, {1, 3, 5, 7}, {2, 4, 8, -1},
		{3, 7, -1, -1}, {4, 6, 8, -1}, {5, 7, -1, -1},
	}
	for m := 0; m < 512; m++ {
		openMask := m &^ (1 << 4) // center removed
		// collect center 4-neighbors indices in 3x3
		nidx := []int{1, 3, 5, 7}
		openN := make([]int, 0, 4)
		for _, id := range nidx {
			if ((openMask >> id) & 1) != 0 {
				openN = append(openN, id)
			}
		}
		if len(openN) <= 1 {
			ok[m] = true
			continue
		}
		comp := [9]int{}
		for i := 0; i < 9; i++ {
			comp[i] = -1
		}
		var q [9]int
		h, t := 0, 0
		ccID := 0
		for i := 0; i < 9; i++ {
			if ((openMask>>i)&1) == 0 || comp[i] >= 0 {
				continue
			}
			comp[i] = ccID
			q[t] = i
			t++
			for h < t {
				u := q[h]
				h++
				for _, v := range neighbors[u] {
					if v < 0 {
						continue
					}
					if ((openMask>>v)&1) == 0 || comp[v] >= 0 {
						continue
					}
					comp[v] = ccID
					q[t] = v
					t++
				}
			}
			ccID++
		}
		c := comp[openN[0]]
		same := true
		for _, id := range openN[1:] {
			if comp[id] != c {
				same = false
				break
			}
		}
		ok[m] = same
	}
	return ok
}

func (ws *wallSolver) mask3x3(center int) int {
	N := ws.N
	cx, cy := center%N, center/N
	mask := 0
	k := 0
	for dy := -1; dy <= 1; dy++ {
		y := cy + dy
		for dx := -1; dx <= 1; dx++ {
			x := cx + dx
			open := false
			if x >= 0 && y >= 0 && x < N && y < N {
				id := y*N + x
				open = ws.isOpen(id)
			}
			if open {
				mask |= 1 << k
			}
			k++
		}
	}
	return mask
}

func (ws *wallSolver) noIncrease3x3Now(center int) bool { return ws.noInc3x3[ws.mask3x3(center)] }
func (ws *wallSolver) nbConn3x3Now(center int) bool     { return ws.nbConn3x3[ws.mask3x3(center)] }

// ==================== Non-articulation (fast tiers -> global BFS) ====================

func (ws *wallSolver) nonArticulation(center int) (bool, bool) {
	// gather open 4-neighbors (inline isOpen)
	nb := ws.Neighbors[center]
	start, end := ws.Start, ws.End
	blocked := ws.blocked

	var nbList [4]int
	nc := 0
	for k := 0; k < 4; k++ {
		v := nb[k]
		if v >= 0 {
			if v == start || v == end || !blocked[v] {
				nbList[nc] = v
				nc++
			}
		}
	}
	if nc <= 1 {
		return true, true // leaf or isolated in open graph
	}

	// O(1) sufficient: 3x3 neighbors already connected (center removed)
	if ws.nbConn3x3Now(center) || ws.noIncrease3x3Now(center) {
		return true, false
	}

	// 5x5-local BFS (center removed): sufficient
	if ws.neighborsConnectedInLocalBox(center, nbList[:nc]) {
		ws.stats.ArticBFSCalls++
		return true, true
	}
	if ws.rng.Float64() > 0.1 {
		return false, true
	}
	// Global BFS in G - {center} from one neighbor; early-exit when all others reached
	ws.stats.ArticBFSCalls++
	if ws.bfsNeighborsConnectedGlobal(center, nbList[:nc]) {
		return true, true
	}
	return false, true
}

// 半径2 (5x5) の局所領域だけで近傍同士が繋がるか（十分条件）
func (ws *wallSolver) neighborsConnectedInLocalBox(center int, openNbs []int) bool {
	N := ws.N
	R := 2
	cx, cy := center%N, center/N
	x0 := cx - R
	if x0 < 0 {
		x0 = 0
	}
	y0 := cy - R
	if y0 < 0 {
		y0 = 0
	}
	x1 := cx + R
	if x1 >= N {
		x1 = N - 1
	}
	y1 := cy + R
	if y1 >= N {
		y1 = N - 1
	}
	w := x1 - x0 + 1
	h := y1 - y0 + 1
	size := w * h
	if size <= 1 {
		return true
	}

	// map: global -> local index
	toLocal := func(id int) int {
		x := id % N
		y := id / N
		if x < x0 || x > x1 || y < y0 || y > y1 {
			return -1
		}
		return (y-y0)*w + (x - x0)
	}

	// local targets
	t1, t2, t3 := -1, -1, -1
	if len(openNbs) >= 2 {
		t1 = toLocal(openNbs[1])
	}
	if len(openNbs) >= 3 {
		t2 = toLocal(openNbs[2])
	}
	if len(openNbs) >= 4 {
		t3 = toLocal(openNbs[3])
	}
	src := toLocal(openNbs[0])
	if src < 0 {
		return false // outside box (theoretically never happens for R>=1)
	}

	// small visited & queue (max 5x5 = 25; allocate on stack)
	var vis [49]bool
	var q [49]int
	head, tail := 0, 0
	vis[src] = true
	q[tail] = src
	tLeft := len(openNbs) - 1

	start, end := ws.Start, ws.End
	blocked := ws.blocked

	for head < tail {
		u := q[head]
		head++

		if u == t1 {
			t1 = -1
			tLeft--
			if tLeft == 0 {
				return true
			}
		} else if u == t2 {
			t2 = -1
			tLeft--
			if tLeft == 0 {
				return true
			}
		} else if u == t3 {
			t3 = -1
			tLeft--
			if tLeft == 0 {
				return true
			}
		}

		ux := u % w
		uy := u / w

		// expand 4-neighbors (stay in box)
		// Right
		if ux+1 < w {
			v := u + 1
			if !vis[v] {
				gx, gy := x0+(ux+1), y0+uy
				id := gy*N + gx
				if id != center && (id == start || id == end || !blocked[id]) {
					vis[v] = true
					q[tail] = v
					tail++
				}
			}
		}
		// Left
		if ux-1 >= 0 {
			v := u - 1
			if !vis[v] {
				gx, gy := x0+(ux-1), y0+uy
				id := gy*N + gx
				if id != center && (id == start || id == end || !blocked[id]) {
					vis[v] = true
					q[tail] = v
					tail++
				}
			}
		}
		// Down
		if uy+1 < h {
			v := u + w
			if !vis[v] {
				gx, gy := x0+ux, y0+(uy+1)
				id := gy*N + gx
				if id != center && (id == start || id == end || !blocked[id]) {
					vis[v] = true
					q[tail] = v
					tail++
				}
			}
		}
		// Up
		if uy-1 >= 0 {
			v := u - w
			if !vis[v] {
				gx, gy := x0+ux, y0+(uy-1)
				id := gy*N + gx
				if id != center && (id == start || id == end || !blocked[id]) {
					vis[v] = true
					q[tail] = v
					tail++
				}
			}
		}
	}
	return false
}

// G - {center} で openNbs[0] から BFS。全他近傍に到達すれば非関節。
func (ws *wallSolver) bfsNeighborsConnectedGlobal(center int, openNbs []int) bool {
	ws.visStamp++
	stamp := ws.visStamp
	q := ws.queue[:0]

	src := openNbs[0]
	ws.vis[src] = stamp
	q = append(q, src)

	t1, t2, t3 := -1, -1, -1
	if len(openNbs) >= 2 {
		t1 = openNbs[1]
	}
	if len(openNbs) >= 3 {
		t2 = openNbs[2]
	}
	if len(openNbs) >= 4 {
		t3 = openNbs[3]
	}
	tLeft := len(openNbs) - 1

	start, end := ws.Start, ws.End
	blocked := ws.blocked

	for head := 0; head < len(q); head++ {
		u := q[head]
		if u == t1 {
			t1 = -1
			tLeft--
			if tLeft == 0 {
				return true
			}
		} else if u == t2 {
			t2 = -1
			tLeft--
			if tLeft == 0 {
				return true
			}
		} else if u == t3 {
			t3 = -1
			tLeft--
			if tLeft == 0 {
				return true
			}
		}
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			v := nbrs[k]
			if v < 0 || v == center {
				continue
			}
			if v != start && v != end && blocked[v] {
				continue
			}
			if ws.vis[v] == stamp {
				continue
			}
			ws.vis[v] = stamp
			q = append(q, v)
		}
	}
	return false
}

// ==================== BFS & parent counts ====================

func (ws *wallSolver) bfsFullFrom(src int, dist []int16) {
	V := ws.N * ws.N
	for i := 0; i < V; i++ {
		dist[i] = -1
	}
	start, end := ws.Start, ws.End
	blocked := ws.blocked

	q := ws.queue[:0]
	if !(src == start || src == end || !blocked[src]) {
		return
	}
	dist[src] = 0
	q = append(q, src)
	for head := 0; head < len(q); head++ {
		u := q[head]
		du := dist[u] + 1
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			v := nbrs[k]
			if v < 0 {
				continue
			}
			if dist[v] >= 0 {
				continue
			}
			if !(v == start || v == end || !blocked[v]) {
				continue
			}
			dist[v] = du
			q = append(q, v)
		}
	}
}

func (ws *wallSolver) buildParentCounts(dist []int16, par []uint8) {
	start, end := ws.Start, ws.End
	blocked := ws.blocked
	V := ws.N * ws.N
	for i := 0; i < V; i++ {
		if dist[i] < 0 {
			par[i] = 0
			continue
		}
		c := 0
		nbrs := ws.Neighbors[i]
		for k := 0; k < 4; k++ {
			p := nbrs[k]
			if p >= 0 && (p == start || p == end || !blocked[p]) && dist[p] == dist[i]-1 {
				c++
			}
		}
		par[i] = uint8(c)
	}
}

// ---- Add-block repair with parent counts ----
func (ws *wallSolver) repairAfterAddBlockWithParents(blockedID int, dist []int16, par []uint8) int {
	changed := 0
	start, end := ws.Start, ws.End
	blocked := ws.blocked

	// 1) invalidate the blocked vertex; decrease par of its children
	d0 := dist[blockedID]
	if d0 >= 0 {
		if dist[blockedID] != -1 {
			dist[blockedID] = -1
			changed++
		}
		nbrs := ws.Neighbors[blockedID]
		for k := 0; k < 4; k++ {
			v := nbrs[k]
			if v < 0 || !(v == start || v == end || !blocked[v]) {
				continue
			}
			if dist[v] == d0+1 {
				if par[v] > 0 {
					par[v]--
				}
				if par[v] == 0 {
					if !ws.inv[v] {
						ws.inv[v] = true
						ws.invTouch = append(ws.invTouch, v)
					}
				}
			}
		}
	}
	if !ws.inv[blockedID] {
		ws.inv[blockedID] = true
		ws.invTouch = append(ws.invTouch, blockedID)
	}

	// 2) propagate invalidation (forward)
	qInv := ws.queue[:0]
	for _, v := range ws.invTouch {
		if v == blockedID {
			continue
		}
		qInv = append(qInv, v)
	}
	for head := 0; head < len(qInv); head++ {
		u := qInv[head]
		old := dist[u]
		if old >= 0 {
			dist[u] = -1
			changed++
		}
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			w := nbrs[k]
			if w < 0 || !(w == start || w == end || !blocked[w]) {
				continue
			}
			if ws.inv[w] {
				continue
			}
			if dist[w] == old+1 {
				if par[w] > 0 {
					par[w]--
				}
				if par[w] == 0 {
					ws.inv[w] = true
					ws.invTouch = append(ws.invTouch, w)
					qInv = append(qInv, w)
				}
			}
		}
	}

	// 3) relabel from boundary
	qFix := ws.queue[:0]
	for _, u := range ws.invTouch {
		if u == blockedID {
			continue
		}
		best := 1 << 30
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			p := nbrs[k]
			if p < 0 || ws.inv[p] || !(p == start || p == end || !blocked[p]) || dist[p] < 0 {
				continue
			}
			if int(dist[p])+1 < best {
				best = int(dist[p]) + 1
			}
		}
		if best != 1<<30 {
			if dist[u] != int16(best) {
				dist[u] = int16(best)
				changed++
			}
			qFix = append(qFix, u)
		} else {
			par[u] = 0
		}
	}
	for head := 0; head < len(qFix); head++ {
		u := qFix[head]
		du := dist[u] + 1
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			v := nbrs[k]
			if v < 0 || !ws.inv[v] || !(v == start || v == end || !blocked[v]) {
				continue
			}
			if dist[v] < 0 || dist[v] > du {
				if dist[v] != du {
					dist[v] = du
					changed++
				}
				qFix = append(qFix, v)
			}
		}
	}

	// 4) recompute par only for (inv ∪ N(inv))
	ws.tmpSeenStamp++
	stamp := ws.tmpSeenStamp
	ws.bufRecalc = ws.bufRecalc[:0]
	for _, u := range ws.invTouch {
		if ws.tmpSeen[u] != stamp {
			ws.tmpSeen[u] = stamp
			ws.bufRecalc = append(ws.bufRecalc, u)
		}
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			w := nbrs[k]
			if w >= 0 && ws.tmpSeen[w] != stamp {
				ws.tmpSeen[w] = stamp
				ws.bufRecalc = append(ws.bufRecalc, w)
			}
		}
	}
	for _, x := range ws.bufRecalc {
		if dist[x] < 0 {
			par[x] = 0
			continue
		}
		c := 0
		nbrs := ws.Neighbors[x]
		for k := 0; k < 4; k++ {
			p := nbrs[k]
			if p >= 0 && (p == start || p == end || !blocked[p]) && dist[p] == dist[x]-1 {
				c++
			}
		}
		par[x] = uint8(c)
	}

	// 5) clear inv flags
	for _, i := range ws.invTouch {
		ws.inv[i] = false
	}
	ws.invTouch = ws.invTouch[:0]
	return changed
}

// ---- Remove-block repair (shortening) with local par recompute ----
func (ws *wallSolver) repairAfterRemoveBlockWithParents(opened int, dist []int16, par []uint8) int {
	changed := 0
	start, end := ws.Start, ws.End
	blocked := ws.blocked

	ws.bufChanged = ws.bufChanged[:0]

	best := 1 << 30
	nbrs := ws.Neighbors[opened]
	for k := 0; k < 4; k++ {
		p := nbrs[k]
		if p < 0 || !(p == start || p == end || !blocked[p]) || dist[p] < 0 {
			continue
		}
		if int(dist[p])+1 < best {
			best = int(dist[p]) + 1
		}
	}
	q := ws.queue[:0]
	if best != 1<<30 && (dist[opened] < 0 || int(dist[opened]) > best) {
		dist[opened] = int16(best)
		q = append(q, opened)
		changed++
		ws.bufChanged = append(ws.bufChanged, opened)
	}
	for head := 0; head < len(q); head++ {
		u := q[head]
		du := dist[u] + 1
		nbrsU := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			v := nbrsU[k]
			if v < 0 || !(v == start || v == end || !blocked[v]) {
				continue
			}
			if dist[v] < 0 || dist[v] > du {
				if dist[v] != du {
					dist[v] = du
					changed++
				}
				q = append(q, v)
				ws.bufChanged = append(ws.bufChanged, v)
			}
		}
	}

	// recompute par for (changed ∪ N(changed))
	ws.tmpSeenStamp++
	stamp := ws.tmpSeenStamp
	ws.bufRecalc = ws.bufRecalc[:0]
	for _, u := range ws.bufChanged {
		if ws.tmpSeen[u] != stamp {
			ws.tmpSeen[u] = stamp
			ws.bufRecalc = append(ws.bufRecalc, u)
		}
		nbrs := ws.Neighbors[u]
		for k := 0; k < 4; k++ {
			w := nbrs[k]
			if w >= 0 && ws.tmpSeen[w] != stamp {
				ws.tmpSeen[w] = stamp
				ws.bufRecalc = append(ws.bufRecalc, w)
			}
		}
	}
	for _, x := range ws.bufRecalc {
		if dist[x] < 0 {
			par[x] = 0
			continue
		}
		c := 0
		nbrs := ws.Neighbors[x]
		for k := 0; k < 4; k++ {
			p := nbrs[k]
			if p >= 0 && (p == start || p == end || !blocked[p]) && dist[p] == dist[x]-1 {
				c++
			}
		}
		par[x] = uint8(c)
	}
	return changed
}

// ==================== SP list ====================

func (ws *wallSolver) rebuildSPList(curDist int) {
	V := ws.N * ws.N
	ws.spList = ws.spList[:0]
	for i := 0; i < V; i++ {
		if !ws.isOpen(i) {
			continue
		}
		if ws.distS[i] >= 0 && ws.distT[i] >= 0 &&
			int(ws.distS[i])+int(ws.distT[i]) == curDist &&
			i != ws.Start && i != ws.End {
			ws.spList = append(ws.spList, i)
		}
	}
}

// ==================== Validation (Debug only) ====================

func (ws *wallSolver) validateCurrentState(curDist int) {
	if !ws.debugEnabled {
		return
	}
	ws.bfsFullFrom(ws.Start, ws.dbgFullS)
	ws.bfsFullFrom(ws.End, ws.dbgFullT)

	V := ws.N * ws.N
	misS, misT := 0, 0
	samples := 0
	const maxSamples = 6
	for i := 0; i < V && samples < maxSamples; i++ {
		if ws.dbgFullS[i] != ws.distS[i] {
			misS++
			fmt.Fprintf(os.Stderr, "           [VALID][S] mismatch at (%d,%d) id=%d: diff=%d vs full=%d\n",
				i%ws.N, i/ws.N, i, ws.distS[i], ws.dbgFullS[i])
			samples++
		}
	}
	for i := 0; i < V && samples < maxSamples; i++ {
		if ws.dbgFullT[i] != ws.distT[i] {
			misT++
			fmt.Fprintf(os.Stderr, "           [VALID][T] mismatch at (%d,%d) id=%d: diff=%d vs full=%d\n",
				i%ws.N, i/ws.N, i, ws.distT[i], ws.dbgFullT[i])
			samples++
		}
	}

	fullEnd := int(ws.dbgFullS[ws.End])
	cc := ws.countOpenCC()

	if misS == 0 && misT == 0 && fullEnd == curDist && cc <= ws.baseOpenCC {
		fmt.Fprintf(os.Stderr, "           [VALID] ok: misS=0 misT=0 endDist=%d openCC=%d (base=%d)\n",
			curDist, cc, ws.baseOpenCC)
	} else {
		fmt.Fprintf(os.Stderr, "           [VALID] NG: misS=%d misT=%d endDist(cur=%d full=%d) openCC(cur=%d base=%d)\n",
			misS, misT, curDist, fullEnd, cc, ws.baseOpenCC)
	}
}

func (ws *wallSolver) countOpenCC() int {
	V := ws.N * ws.N
	for i := 0; i < V; i++ {
		ws.dbgVis[i] = false
	}
	cc := 0
	q := ws.queue[:0]
	for i := 0; i < V; i++ {
		if ws.dbgVis[i] || !ws.isOpen(i) {
			continue
		}
		cc++
		ws.dbgVis[i] = true
		q = q[:0]
		q = append(q, i)
		for head := 0; head < len(q); head++ {
			u := q[head]
			nbrs := ws.Neighbors[u]
			for k := 0; k < 4; k++ {
				v := nbrs[k]
				if v < 0 || ws.dbgVis[v] || !ws.isOpen(v) {
					continue
				}
				ws.dbgVis[v] = true
				q = append(q, v)
			}
		}
	}
	return cc
}

// ==================== RNG ====================

type rng64 struct{ s uint64 }

func (r *rng64) next() uint64 {
	x := r.s
	x ^= x >> 12
	x ^= x << 25
	x ^= x >> 27
	r.s = x
	return x * 2685821657736338717
}
func (r *rng64) Intn(n int) int   { return int(r.next() % uint64(n)) }
func (r *rng64) Float64() float64 { return float64(r.next()>>11) * (1.0 / (1 << 53)) }

// 直線壁設置関数ここから

// PlaceStraightWallAndKeepConnectivity は、直線の壁を設置し、
// その結果で初期より空き成分が増えた場合に限り、
// 設置した直線壁（新設セルのみ）を「最低限」だけ除去して
// 初期の連結成分数（K0）に戻します。
// m>=2 の“即効穴”を優先し、それでも足りない場合は 0-1 BFS で
// 最小除去パスを一括開通します。
//
// 入力：
//
//	N               : グリッド一辺（N×N）
//	isWall          : 初期壁配置（一次元化 N*N）
//	neighbors       : 各セルの4近傍インデックス（範囲外は -1）
//	length          : 設置する直線壁の長さ
//	dir             : 0=縦(i増), 1=横(j増)
//	iOffset, jOffset: 直線壁の開始位置
//	forbidden       : true のセルは設置禁止（初期壁はこの値によらず不可変）
//
// 出力：
//
//	isWall          : 最終的な壁配置（破壊的更新）
func PlaceStraightWallAndKeepConnectivity(
	N int,
	isWall []bool,
	neighbors [][4]int,
	length int,
	dir int,
	iOffset, jOffset int,
	forbidden []bool,
) []bool {

	total := N * N
	if len(isWall) != total || len(neighbors) != total || length <= 0 {
		return isWall
	}

	// 1) 初期の空き成分数
	K0 := countOpenComponents(isWall, neighbors)

	// 2) 直線壁の設置（forbidden は設置スキップ、初期壁は触らない）
	placed := make([]bool, total) // 今回新設した壁（除去可能）
	placedList := make([]int, 0, length)

	canPlaceAt := func(idx int) bool {
		if isWall[idx] {
			return false // 既に壁（初期壁含む）は触らない
		}
		if forbidden != nil {
			if len(forbidden) == total {
				if forbidden[idx] {
					return false
				}
			} else if idx < len(forbidden) && forbidden[idx] {
				return false
			}
		}
		return true
	}

	i, j := iOffset, jOffset
	for t := 0; t < length; t++ {
		if i < 0 || i >= N || j < 0 || j >= N {
			break
		}
		idx := i*N + j
		if canPlaceAt(idx) {
			isWall[idx] = true
			placed[idx] = true
			placedList = append(placedList, idx)
		}
		if dir == 0 {
			i++
		} else {
			j++
		}
	}

	// 3) 設置後の空き成分数
	K1 := countOpenComponents(isWall, neighbors)
	if K1 <= K0 {
		// 連結悪化なし
		return isWall
	}

	// 4) 連結回復：まず現在の active / DSU を構築
	totalCells := total
	active := make([]bool, totalCells) // 空きマス
	for v := 0; v < totalCells; v++ {
		active[v] = !isWall[v]
	}

	d := newDSU(totalCells)
	compCount := 0
	for v := 0; v < totalCells; v++ {
		if active[v] {
			compCount++
		}
	}
	for v := 0; v < totalCells; v++ {
		if !active[v] {
			continue
		}
		for k := 0; k < 4; k++ {
			nb := neighbors[v][k]
			if nb >= 0 && active[nb] && d.union(v, nb) {
				compCount--
			}
		}
	}

	// compCount を K0 まで減らす
	for compCount > K0 {
		// 4-1) m が最大の即効穴（m>=2）を探して1つ開ける
		bestIdx := -1
		bestM := 0
		for _, idx := range placedList {
			if !placed[idx] || active[idx] {
				continue // 既に開いている
			}
			m := distinctOpenComponentCount(idx, active, neighbors, d)
			if m > bestM {
				bestM = m
				bestIdx = idx
				if bestM == 4 {
					break
				}
			}
		}

		if bestIdx != -1 && bestM >= 2 {
			// 即効穴を開ける：m 個の成分を束ねるので net で (m-1) 減
			isWall[bestIdx] = false
			placed[bestIdx] = false
			active[bestIdx] = true

			compCount++ // ノード追加
			for k := 0; k < 4; k++ {
				nb := neighbors[bestIdx][k]
				if nb >= 0 && active[nb] && d.union(bestIdx, nb) {
					compCount--
				}
			}
			continue // まだ compCount > K0 なら繰り返し
		}

		// 4-2) 即効穴が無い（bestM <= 1）→ 0-1 BFS で最小除去数の経路を一括開通
		if !openMinPathBy01BFS(N, neighbors, isWall, placed, active, d, &compCount, K0) {
			// これ以上改善不能（理論上は起きにくい）→ 打ち切り
			break
		}
	}

	return isWall
}

/*** ---------- フォールバック：0-1 BFS で最小除去パス開通 ---------- ***/

// openMinPathBy01BFS は、現在の active/DSU 状態で、
// 「最大サイズの空き成分」からスタートし、他成分のいずれかへ到達する
// “壁除去数が最小”のパスを 0-1 BFS で求め、その経路上の「今回設置した壁」
// を一括で開けます。1回の呼び出しで compCount を最低1減らします。
// 戻り値：改善できたら true、経路が見つからなければ false。
func openMinPathBy01BFS(
	N int,
	neighbors [][4]int,
	isWall []bool,
	placed []bool,
	active []bool,
	d *dsu,
	compCount *int,
	K0 int,
) bool {
	total := len(isWall)

	// a) 現在の空き成分のうち「最大サイズの成分」を main とする
	rootSize := make(map[int]int, 64)
	var mainRoot int
	mainRoot = -1
	for v := 0; v < total; v++ {
		if !active[v] {
			continue
		}
		r := d.find(v)
		if rSize, ok := rootSize[r]; ok {
			rootSize[r] = rSize + 1
		} else {
			rootSize[r] = 1
		}
		if mainRoot == -1 || rootSize[r] > rootSize[mainRoot] {
			mainRoot = r
		}
	}
	if mainRoot == -1 {
		return false
	}

	// b) 0-1 BFS 準備
	const INF = -1
	dist := make([]int, total) // 最小除去数
	prev := make([]int, total) // 経路復元
	for i := 0; i < total; i++ {
		dist[i] = INF
		prev[i] = -1
	}

	// 並行探索キュー（双方向デック）
	dq := list.New()

	// mainRoot に属する全空きノードを距離0で初期化
	for v := 0; v < total; v++ {
		if active[v] && d.find(v) == mainRoot {
			dist[v] = 0
			prev[v] = -1
			dq.PushBack(v)
		}
	}

	// c) 探索
	target := -1
	for dq.Len() > 0 {
		front := dq.Front()
		u := front.Value.(int)
		dq.Remove(front)

		// mainRoot 以外の空き成分に到達したら終了（最小コスト確定）
		if active[u] && d.find(u) != mainRoot {
			target = u
			break
		}

		// 4近傍へ遷移
		for k := 0; k < 4; k++ {
			nb := neighbors[u][k]
			if nb < 0 {
				continue
			}

			if !isWall[nb] {
				// 空きセル：コスト0
				if dist[nb] == INF || dist[nb] > dist[u] {
					dist[nb] = dist[u]
					prev[nb] = u
					dq.PushFront(nb)
				}
			} else {
				// 壁セル：今回設置した壁のみコスト1で通過可
				if placed[nb] {
					newD := dist[u] + 1
					if dist[nb] == INF || newD < dist[nb] {
						dist[nb] = newD
						prev[nb] = u
						dq.PushBack(nb)
					}
				}
				// 初期壁は通れない
			}
		}
	}

	if target == -1 {
		// 見つからないのは、初期から到達不能な領域（K0 側の事情）など極例
		return false
	}

	// d) 経路復元：target から mainRoot のいずれかに至るまで戻る
	// 経路上で placed==true のセルだけを開ければよい
	openList := make([]int, 0, 8)
	for cur := target; cur != -1; cur = prev[cur] {
		if isWall[cur] && placed[cur] {
			openList = append(openList, cur)
		}
		// prev[cur]==-1 まで遡る（mainRoot の初期到達点は空き）
	}

	// e) 経路上の新設壁を一括で開ける
	for _, idx := range openList {
		if !placed[idx] {
			continue
		}
		isWall[idx] = false
		placed[idx] = false
		active[idx] = true

		// 新規空きノードの追加 → compCount++
		*compCount++

		// 近傍の空きと union（繋がるたび compCount--）
		for k := 0; k < 4; k++ {
			nb := neighbors[idx][k]
			if nb >= 0 && active[nb] && d.union(idx, nb) {
				*compCount--
			}
		}
	}
	return true
}

/*** ---------- 補助実装 ---------- ***/

// 近傍に存在する“現在の空き成分”のユニーク個数を返す。
func distinctOpenComponentCount(idx int, active []bool, neighbors [][4]int, d *dsu) int {
	var rep [4]int
	n := 0
	seen := func(x int) bool {
		for i := 0; i < n; i++ {
			if rep[i] == x {
				return true
			}
		}
		return false
	}
	for k := 0; k < 4; k++ {
		nb := neighbors[idx][k]
		if nb < 0 || !active[nb] {
			continue
		}
		p := d.find(nb)
		if !seen(p) {
			rep[n] = p
			n++
		}
	}
	return n
}

// isWall に基づいて「空きマスの連結成分数」を数える（全再計算）。
func countOpenComponents(isWall []bool, neighbors [][4]int) int {
	n := len(isWall)
	d := newDSU(n)
	active := make([]bool, n)
	comp := 0
	for v := 0; v < n; v++ {
		active[v] = !isWall[v]
		if active[v] {
			comp++
		}
	}
	for v := 0; v < n; v++ {
		if !active[v] {
			continue
		}
		for k := 0; k < 4; k++ {
			nb := neighbors[v][k]
			if nb >= 0 && active[nb] && d.union(v, nb) {
				comp--
			}
		}
	}
	return comp
}

/*** --- Union-Find (Disjoint Set Union) --- ***/

type dsu struct {
	parent []int
	rank   []byte
}

func newDSU(n int) *dsu {
	p := make([]int, n)
	r := make([]byte, n)
	for i := 0; i < n; i++ {
		p[i] = i
	}
	return &dsu{parent: p, rank: r}
}

func (d *dsu) find(x int) int {
	px := d.parent[x]
	if px != x {
		d.parent[x] = d.find(px)
	}
	return d.parent[x]
}

func (d *dsu) union(a, b int) bool {
	ra, rb := d.find(a), d.find(b)
	if ra == rb {
		return false
	}
	if d.rank[ra] < d.rank[rb] {
		ra, rb = rb, ra
	}
	d.parent[rb] = ra
	if d.rank[ra] == d.rank[rb] {
		d.rank[ra]++
	}
	return true
}

// 直線壁設置関数ここまで

// トラップ設置関数ここから

type TrapEmbedder struct {
	N         int      // 盤の一辺（N×N）
	Neighbors [][4]int // 各マスの4近傍（範囲外は -1）

	// パターンを依存注入したい場合に設定（nil の場合はグローバルを参照）
	TrapWalls [][][2]int
	TrapPath  [][][2]int
}

// 任意の簡易コンストラクタ
func NewTrapEmbedder(N int, neighbors [][4]int) TrapEmbedder {
	return TrapEmbedder{N: N, Neighbors: neighbors}
}

// EmbedTrap は、pattern で指定された Trap 形状を、trapPos(中心, (i,j)) を基準に
// 反転(j→-j) → 回転(時計回り rot*90°) して埋め込みます。
// 仕様:
//   - TRAP_PATH: 必ずグリッド内＆初期壁でないこと（初期壁は除去不可）
//   - TRAP_WALLS: 範囲外なら「既に壁」と見なして無視（失敗条件ではない）
//   - forbidden[idx]==true のマスは状態変更不可（初期と同じでなければならない）
//   - とくに、WALL 設置が必要で、かつ当該マスが空き && forbidden の場合は失敗
//   - 埋め込み後の空き(!isWall)の連結成分数は元より増えてはならない（after <= before）
//
// 戻り値:
//   - 成功: 反映後 isWall, true
//   - 失敗: 初期 isWall のコピー, false（入力は非破壊）
func (te TrapEmbedder) EmbedTrap(
	isWall []bool,
	forbidden []bool, // len==N*N を推奨。nil/長さ0なら「制限なし」と解釈
	trapPos [2]int, // (ci, cj)
	pattern int, // 使用するパターンのインデックス
	rot int, // 0..3 (0°,90°,180°,270°; 時計回り)
	reflect bool, // true で回転前に左右反転 (j→-j)
) ([]bool, bool) {
	total := te.N * te.N
	if len(isWall) != total || len(te.Neighbors) != total {
		return te.clone(isWall), false
	}
	if len(forbidden) != 0 && len(forbidden) != total {
		// forbidden は nil(または長さ0) か total と一致している必要あり
		return te.clone(isWall), false
	}
	isForbid := func(idx int) bool {
		return len(forbidden) > 0 && forbidden[idx]
	}

	// パターンの取得（依存注入 > グローバル の順で参照）
	walls, ok := te.getWalls(pattern)
	if !ok {
		return te.clone(isWall), false
	}
	path, ok := te.getPath(pattern)
	if !ok {
		return te.clone(isWall), false
	}

	// 初期の空き成分数
	initialComponents := te.countEmptyComponents(isWall)

	// 1) PATH 検証（必ず範囲内 & 初期壁ではない）
	pathIdx := make(map[int]struct{}, len(path))
	for _, p := range path {
		di, dj := p[0], p[1]
		ti, tj := te.applyTransformIJ(di, dj, rot, reflect)
		ai := trapPos[0] + ti
		aj := trapPos[1] + tj

		if !te.inBounds(ai, aj) {
			// PATH は範囲外不可（範囲外=壁扱いのため NG）
			return te.clone(isWall), false
		}
		idx := ai*te.N + aj
		if isWall[idx] {
			// 初期壁は除去不可
			return te.clone(isWall), false
		}
		// PATH は変更しないので forbidden との衝突は発生しない（初期が空きならOK）
		pathIdx[idx] = struct{}{}
	}

	// 2) WALLS 反映（範囲外は既存壁扱い／PATH と衝突は失敗）
	cand := te.clone(isWall)
	for _, w := range walls {
		di, dj := w[0], w[1]
		ti, tj := te.applyTransformIJ(di, dj, rot, reflect)
		ai := trapPos[0] + ti
		aj := trapPos[1] + tj

		if !te.inBounds(ai, aj) {
			// 範囲外は「既に壁」とみなす → 何もしない
			continue
		}
		idx := ai*te.N + aj

		// PATH と WALL の衝突は矛盾
		if _, onPath := pathIdx[idx]; onPath {
			return te.clone(isWall), false
		}

		// 既に壁なら変更なし → forbidden でも問題なし
		if cand[idx] {
			continue
		}

		// ここで壁を新規に立てる必要がある
		if isForbid(idx) {
			// forbidden マスの変更は禁止 → 埋め込み不可
			return te.clone(isWall), false
		}
		cand[idx] = true
	}

	// 2.5) 念のため、forbidden マスが初期と同値であることを検証（安全ネット）
	if len(forbidden) > 0 {
		for idx, f := range forbidden {
			if f && cand[idx] != isWall[idx] {
				return te.clone(isWall), false
			}
		}
	}

	// 3) 連結成分チェック（空き成分が増えていないか）
	afterComponents := te.countEmptyComponents(cand)
	if afterComponents > initialComponents {
		return te.clone(isWall), false
	}

	return cand, true
}

// --- 内部ヘルパ ---

// (i,j) が盤内か
func (te TrapEmbedder) inBounds(i, j int) bool {
	return 0 <= i && i < te.N && 0 <= j && j < te.N
}

// 反転(j→-j) → 時計回り回転(90°単位) を適用した (di,dj) を返す
// 回転定義（画面上の時計回り）:
//
//	r=0: (i, j)
//	r=1: (j, -i)
//	r=2: (-i, -j)
//	r=3: (-j,  i)
func (te TrapEmbedder) applyTransformIJ(di, dj int, rot int, reflect bool) (int, int) {
	i, j := di, dj
	if reflect {
		j = -j
	}
	switch ((rot % 4) + 4) % 4 {
	case 0:
		return i, j
	case 1:
		return j, -i
	case 2:
		return -i, -j
	case 3:
		return -j, i
	default:
		return i, j
	}
}

// スライスのコピー（入力を非破壊に保つ）
func (te TrapEmbedder) clone(src []bool) []bool {
	dst := make([]bool, len(src))
	copy(dst, src)
	return dst
}

// 空き(!isWall) の 4近傍連結成分数を BFS で数える
func (te TrapEmbedder) countEmptyComponents(isWall []bool) int {
	total := len(isWall)
	visited := make([]bool, total)
	comps := 0

	queue := make([]int, 0, 64)
	for v := 0; v < total; v++ {
		if isWall[v] || visited[v] {
			continue
		}
		comps++
		visited[v] = true
		queue = queue[:0]
		queue = append(queue, v)

		for head := 0; head < len(queue); head++ {
			cur := queue[head]
			for _, nb := range te.Neighbors[cur] {
				if nb < 0 || isWall[nb] || visited[nb] {
					continue
				}
				visited[nb] = true
				queue = append(queue, nb)
			}
		}
	}
	return comps
}

// パターン参照（依存注入 > グローバル）。pattern が範囲外なら false。
func (te TrapEmbedder) getWalls(pattern int) ([][2]int, bool) {
	if te.TrapWalls != nil {
		if pattern < 0 || pattern >= len(te.TrapWalls) {
			return nil, false
		}
		return te.TrapWalls[pattern], true
	}
	if pattern < 0 || pattern >= len(TRAP_WALLS) {
		return nil, false
	}
	return TRAP_WALLS[pattern], true
}

func (te TrapEmbedder) getPath(pattern int) ([][2]int, bool) {
	if te.TrapPath != nil {
		if pattern < 0 || pattern >= len(te.TrapPath) {
			return nil, false
		}
		return te.TrapPath[pattern], true
	}
	if pattern < 0 || pattern >= len(TRAP_PATH) {
		return nil, false
	}
	return TRAP_PATH[pattern], true
}

// 最短経路
// GridSolver は N×N グリッド上の最短距離／最短経路（指定のタイブレーク付き）を求める。
type GridSolver struct {
	N         int
	Start     int
	IsWall    []bool   // len = N*N
	Neighbors [][4]int // len = N*N, out-of-bounds や存在しない近傍は -1 を想定

	flower int // flower タイブレーク用

	// メモ化（Distances 用）
	dist     []int
	parent   []int // BFS の親（主にデバッグ/フォールバック用）
	computed bool
}

// NewGridSolver は入力を受け取り初期化する。
func NewGridSolver(N int, isWall []bool, neighbors [][4]int, start int, flower int) *GridSolver {
	return &GridSolver{
		N:         N,
		Start:     start,
		IsWall:    isWall,
		Neighbors: neighbors,
		flower:    flower,
	}
}

// Distances は start からの最短距離（壁/未到達は -1）を返す。
// 返り値は内部配列への参照（読み取り専用として利用してください）。
func (g *GridSolver) Distances() []int {
	g.ensureBFSFromStart()
	return g.dist
}

// Path は start から end までの最短経路を返す。
// タイブレークは「i（行）座標が N/2 に近い候補を貪欲に選ぶ」。
// 到達不可または start/end が壁なら空スライスを返す。
func (g *GridSolver) Path(end int) []int {
	total := g.N * g.N
	if end < 0 || end >= total {
		return []int{}
	}
	g.ensureBFSFromStart()
	if g.invalid(g.Start) || g.invalid(end) || g.dist[end] == -1 {
		return []int{}
	}

	// end からの距離も計算（最短経路上の候補判定に用いる）
	distE := g.bfsFrom(end)

	path := make([]int, 0, g.dist[end]+1)
	cur := g.Start
	path = append(path, cur)

	for cur != end {
		best := -1
		bestScore := int(^uint(0) >> 1) // math.MaxInt の代替

		curDist := g.dist[cur]
		nb := g.Neighbors[cur]

		for k := 0; k < 4; k++ {
			to := nb[k]
			if to < 0 || g.IsWall[to] {
				continue
			}
			// (1) start から 1 ステップ進む
			if g.dist[to] != curDist+1 {
				continue
			}
			// (2) end へも最短距離を維持できる（= 最短経路上の点である）
			if distE[to] == -1 || g.dist[end] != g.dist[to]+distE[to] {
				continue
			}
			// (3) タイブレーク：i が N/2 に近い方を優先（より小さい方が良い）
			score := g.rowCenterScore(to)
			if score < bestScore || (score == bestScore && to < best) {
				bestScore = score
				best = to
			}
		}

		// 理論上、best は必ず見つかるはず（上の条件で最短経路上に限定しているため）
		if best == -1 {
			// 念のためのフォールバック：親ポインタで復元
			return g.pathByParents(end)
		}
		path = append(path, best)
		cur = best
	}
	return path
}

// -------- 内部実装 --------

func (g *GridSolver) ensureBFSFromStart() {
	if g.computed {
		return
	}
	total := g.N * g.N
	if g.dist == nil || len(g.dist) != total {
		g.dist = make([]int, total)
		g.parent = make([]int, total)
	}
	for i := 0; i < total; i++ {
		g.dist[i] = -1
		g.parent[i] = -1
	}
	if g.invalid(g.Start) {
		g.computed = true
		return
	}

	// 単方向 BFS（キューをスライス＋head で高速運用）
	q := make([]int, 0, total)
	head := 0
	g.dist[g.Start] = 0
	q = append(q, g.Start)

	for head < len(q) {
		u := q[head]
		head++
		nu := g.Neighbors[u]

		for i := 0; i < 4; i++ {
			v := nu[i]
			if v < 0 || g.IsWall[v] {
				continue
			}
			if g.dist[v] == -1 {
				g.dist[v] = g.dist[u] + 1
				g.parent[v] = u
				q = append(q, v)
			} else if g.dist[v] == g.dist[u]+1 {
				// 同距離タイ（親の差し替え）。※Distances の返値には影響しないが一貫性のため実装
				if g.preferParent(u, g.parent[v]) {
					g.parent[v] = u
				}
			}
		}
	}
	g.computed = true
}

// 任意の src から BFS 距離だけを計算（タイブレーク不要）
func (g *GridSolver) bfsFrom(src int) []int {
	total := g.N * g.N
	dist := make([]int, total)
	for i := 0; i < total; i++ {
		dist[i] = -1
	}
	if g.invalid(src) {
		return dist
	}
	q := make([]int, 0, total)
	head := 0
	dist[src] = 0
	q = append(q, src)

	for head < len(q) {
		u := q[head]
		head++
		nu := g.Neighbors[u]
		for i := 0; i < 4; i++ {
			v := nu[i]
			if v < 0 || g.IsWall[v] {
				continue
			}
			if dist[v] == -1 {
				dist[v] = dist[u] + 1
				q = append(q, v)
			}
		}
	}
	return dist
}

// 親候補 a と b の選好：i が N/2 に近い方を優先。
// 近さは整数だけで判定：score(i) = |2*i - N|（小さいほど中心行に近い）。
// 同点ならインデックスの小さい方を採用し決定性を担保。
func (g *GridSolver) preferParent(a, b int) bool {
	if b == -1 {
		return true
	}
	ra := a / g.N
	rb := b / g.N
	sa := abs(2*ra - g.N)
	sb := abs(2*rb - g.N)
	if sa != sb {
		return sa < sb
	}
	return a < b
}

// 行方向の中心からの「近さ」スコア。小さいほど良い。
func (g *GridSolver) rowCenterScore(idx int) int {
	i := idx / g.N
	j := idx % g.N
	ti := g.flower / g.N
	tj := g.flower % g.N
	// | j - N/2 | に比例する整数量に落とす（浮動小数を使わず高速＆確定的）
	return abs(i-ti) + abs(j-tj)
}

func (g *GridSolver) pathByParents(end int) []int {
	if g.dist[end] == -1 {
		return []int{}
	}
	buf := make([]int, 0, g.dist[end]+1)
	for v := end; v != -1; v = g.parent[v] {
		buf = append(buf, v)
	}
	// 反転
	for l, r := 0, len(buf)-1; l < r; l, r = l+1, r-1 {
		buf[l], buf[r] = buf[r], buf[l]
	}
	return buf
}

func (g *GridSolver) invalid(x int) bool {
	total := g.N * g.N
	return x < 0 || x >= total || g.IsWall[x]
}

func abs(x int) int {
	if x >= 0 {
		return x
	}
	return -x
}

// GridArticulation は N×N グリッドの（壁を除いた）4近傍無向グラフに対して、
// 各セルが関節点かどうかを高速に問い合わせできる構造体です。
type GridArticulation struct {
	N         int
	n         int
	isWall    []bool
	neighbors [][4]int

	// 前計算済み
	art    []bool // art[v] == true なら v は関節点
	disc   []int  // DFS での発見時刻（未訪問は -1）
	low    []int  // lowlink 値
	parent []int  // DFS 木における親
}

// NewGridArticulation は前処理(O(V+E))を行い、以後の問い合わせを O(1) にします。
// - isWall の長さと neighbors の長さは N*N である必要があります。
// - neighbors[v][k] は v の k 番目の近傍インデックス（範囲外は -1 を推奨）。
func NewGridArticulation(N int, isWall []bool, neighbors [][4]int) *GridArticulation {
	n := N * N
	if len(isWall) != n {
		panic("isWall length must be N*N")
	}
	if len(neighbors) != n {
		panic("neighbors length must be N*N")
	}
	g := &GridArticulation{
		N:         N,
		n:         n,
		isWall:    isWall,
		neighbors: neighbors,
	}
	g.precompute()
	return g
}

// IsArticulation は一次元インデックス ij のセルが関節点かどうかを返します。
// 壁セル / 範囲外に対しては false を返します。
func (g *GridArticulation) IsArticulation(ij int) bool {
	if ij < 0 || ij >= g.n {
		return false
	}
	if g.isWall[ij] {
		return false
	}
	return g.art[ij]
}

// 必要なら壁やグラフが変わったあとに再前処理できます。
func (g *GridArticulation) Rebuild() {
	g.precompute()
}

// ---- 実装詳細 ----

func (g *GridArticulation) precompute() {
	n := g.n
	disc := make([]int, n)
	low := make([]int, n)
	parent := make([]int, n)
	iter := make([]int, n)  // 各頂点で走査中の近傍ポインタ（0..4）
	child := make([]int, n) // DFS 木における子の数（ルートの関節点判定用）

	for i := 0; i < n; i++ {
		disc[i] = -1
		parent[i] = -1
		iter[i] = 0
		child[i] = 0
	}
	art := make([]bool, n)

	time := 0
	stack := make([]int, 0, n)

	for s := 0; s < n; s++ {
		if g.isWall[s] || disc[s] != -1 {
			continue
		}
		// 新しい DFS 木のルート
		parent[s] = -1
		stack = append(stack, s)

		for len(stack) > 0 {
			v := stack[len(stack)-1]

			// 初回訪問
			if disc[v] == -1 {
				disc[v] = time
				low[v] = time
				time++
			}

			// 近傍を順に処理
			if iter[v] < 4 {
				w := g.neighbors[v][iter[v]]
				iter[v]++

				// 範囲外 / 壁は無視
				if w < 0 || w >= n {
					continue
				}
				if g.isWall[w] {
					continue
				}

				if disc[w] == -1 {
					// ツリー枝
					parent[w] = v
					child[v]++
					stack = append(stack, w) // 疑似再帰
				} else if w != parent[v] {
					// 後退枝
					if disc[w] < low[v] {
						low[v] = disc[w]
					}
				}
				continue
			}

			// v の近傍をすべて処理し終えた
			stack = stack[:len(stack)-1]
			p := parent[v]
			if p != -1 {
				if low[v] < low[p] {
					low[p] = low[v]
				}
				// v の部分木が p 以上に戻れないなら p は関節点
				if low[v] >= disc[p] {
					art[p] = true
				}
			} else {
				// ルートは子が2つ以上なら関節点
				if child[v] >= 2 {
					art[v] = true
				}
			}
		}
	}

	g.disc = disc
	g.low = low
	g.parent = parent
	g.art = art
}
