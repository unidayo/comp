// last local submission: 
package main

import (
	"bufio"
	"container/heap"
	"fmt"
	"math"
	"math/bits"
	"math/rand"
	"os"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"
)

const (
	INF = 100000000
	// Main
	TIME_LIMIT = 1800 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01

	MINIMIZE = false

	BOARD_WIDTH = 50
	MAX_TURN    = 800

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = true  // 複数ケースを試すか
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 3000
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FOLDER         = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "0734"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	RECORD_CODE        = true
	CODE_FILE          = "ignore\\code" // テストケースの保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = false
	PRINT_ACTION       = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 0     // 基本戦術、方針レベルの違い
	VERSION_MINOR = 0     // 改善等
	VERSION_PATCH = 0     // バグ修正,パラメータ変更など
	VERSION_NAME  = ""    // バージョン名
	VERSION_VALID = false // 提出に利用できる正当なものか

)

var DIRS = [4]int{-BOARD_WIDTH, -1, 1, BOARD_WIDTH}
var ANGLE = [4]float64{180, 270, 90, 0}
var NEAR_DIJ = [13]int{-2 * BOARD_WIDTH, -BOARD_WIDTH - 1, -BOARD_WIDTH, -BOARD_WIDTH + 1, -2, -1, 0, 1, 2, BOARD_WIDTH - 1, BOARD_WIDTH, BOARD_WIDTH + 1, 2 * BOARD_WIDTH}
var PERMUTE = [24][4]int{{0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1}, {0, 3, 1, 2}, {0, 3, 2, 1}, {1, 0, 2, 3}, {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0}, {2, 1, 0, 3}, {2, 1, 3, 0}, {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 3, 1, 0}, {2, 3, 0, 1}, {3, 1, 2, 0}, {3, 1, 0, 2}, {3, 2, 1, 0}, {3, 2, 0, 1}, {3, 0, 1, 2}, {3, 0, 2, 1}}

var RAIL_CONNECTED = [7][4]bool{{true, true, true, true}, {false, true, true, false}, {true, false, false, true}, {false, true, false, true}, {true, true, false, false}, {true, false, true, false}, {false, false, true, true}}

type RandGenerater struct {
	x int
}

func (rg *RandGenerater) Next() int {
	// 32bit乱数を生成
	return 0
}

func (rg *RandGenerater) IntN(n int) int {
	a := (rg.Next() * n) >> 32
	return a
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

func maxInt(a int, b int) int {
	if a > b {
		return a
	}
	return b
}

func minInt(a int, b int) int {
	if a < b {
		return a
	}
	return b
}

func absInt(a int) int {
	if a < 0 {
		return -a
	}
	return a
}

func popcount(x int) int {
	x = x - ((x >> 1) & 0x5555555555555555)

	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333)

	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f
	x = x + (x >> 8)
	x = x + (x >> 16)
	x = x + (x >> 32)
	return x & 0x0000007f
}

func getScanner(file string) *bufio.Scanner {
	var scanner *bufio.Scanner
	if LOCAL_TEST {
		fp, err := os.Open(file)
		if err != nil {
			panic("Error: Can not open file ")
		}
		if !REPEAT {
			fmt.Fprintf(os.Stderr, "---CASE %s---\n", file)
		}
		scanner = bufio.NewScanner(fp)
	} else {
		scanner = bufio.NewScanner(os.Stdin)
	}
	scanner.Buffer(make([]byte, 1000000), 1000000)
	return scanner
}

func getInputs(gConst *GameConst) {
	// 例
	gConst.Scanner.Scan()
	splitedInput := strings.Split(gConst.Scanner.Text(), " ")
	M, _ := strconv.Atoi(splitedInput[1])
	K, _ := strconv.Atoi(splitedInput[2])
	gConst.NumPeople = M
	gConst.InitialMoney = K

	gConst.Poss = make([][2]int, 0, gConst.NumPeople)
	gConst.Homes = make([][2]int, 0, gConst.NumPeople)
	gConst.WorkPlaces = make([][2]int, 0, gConst.NumPeople)
	gConst.Fares = make([]int, 0, gConst.NumPeople)

	for peopleIndex := 0; peopleIndex < gConst.NumPeople; peopleIndex++ {
		gConst.Scanner.Scan()
		splitedInput = strings.Split(gConst.Scanner.Text(), " ")
		i0, _ := strconv.Atoi(splitedInput[0])
		j0, _ := strconv.Atoi(splitedInput[1])
		i1, _ := strconv.Atoi(splitedInput[2])
		j1, _ := strconv.Atoi(splitedInput[3])
		gConst.Poss = append(gConst.Poss, [2]int{i0*BOARD_WIDTH + j0, i1*BOARD_WIDTH + j1})
		gConst.Homes = append(gConst.Homes, [2]int{i0, j0})
		gConst.WorkPlaces = append(gConst.WorkPlaces, [2]int{i1, j1})
		ij0 := i0*BOARD_WIDTH + j0
		ij1 := i1*BOARD_WIDTH + j1
		gConst.Fares = append(gConst.Fares, gConst.distances[ij0][ij1])
		gConst.sumFares += gConst.distances[ij0][ij1]
		distance := gConst.distances[ij0][ij1]
		for _, nij := range gConst.nearIJs[ij0] {
			if nij == -1 {
				continue
			}
			gConst.potential[nij] += distance
			gConst.IncludedPoints[nij][0] = append(gConst.IncludedPoints[nij][0], peopleIndex)
		}
		for _, nij := range gConst.nearIJs[ij1] {
			if nij == -1 {
				continue
			}
			gConst.potential[nij] += distance
			gConst.IncludedPoints[nij][1] = append(gConst.IncludedPoints[nij][1], peopleIndex)
		}
		for _, dij0 := range NEAR_DIJ {
			nij0 := ij0 + dij0
			if nij0 < 0 || nij0 >= BOARD_WIDTH*BOARD_WIDTH {
				continue
			}
			if gConst.distances[nij0][ij0] > 2 {
				continue
			}
			for _, dij1 := range NEAR_DIJ {
				nij1 := ij1 + dij1
				if nij1 < 0 || nij1 >= BOARD_WIDTH*BOARD_WIDTH {
					continue
				}
				if gConst.distances[nij1][ij1] > 2 {
					continue
				}
				gConst.effects[nij0][nij1] += distance
				gConst.effects[nij1][nij0] += distance
				// if gConst.effects[nij0][nij1] > 200 {
				// 	fmt.Fprintln(os.Stderr, nij0, nij1, gConst.effects[nij0][nij1])
				// }
			}
		}
	}
	for i := range gConst.potential {
		if gConst.potential[i] > 0 {
			gConst.candidates = append(gConst.candidates, i)
		}
	}
	// fmt.Fprintln(os.Stderr, gConst.sumFares)
	fmt.Fprintln(os.Stderr, len(gConst.candidates))
}

// 現在の状態を記録するもの
type GameState struct {
	covered [][2]bool
	income  int
	money   int
	turn    int
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	g.covered = make([][2]bool, gConst.NumPeople)
	g.income = 0
	g.money = gConst.InitialMoney
	g.turn = 0
	return &g
}

func (g *GameState) Afford(action *Action) bool {
	if action.ActionType == -1 {
		return true
	} else if action.ActionType == 0 {
		return g.money >= 5000
	}
	return g.money >= 100
}

func (g *GameState) ApplyAction(gameConst *GameConst, action *Action) {
	ij := action.I*BOARD_WIDTH + action.J
	if action.ActionType == 0 {
		for k := range gameConst.IncludedPoints[ij] {
			for _, houseIndex := range gameConst.IncludedPoints[ij][k] {
				if g.covered[houseIndex][k] == false && g.covered[houseIndex][1-k] == true {
					// 収入が増加
					from := gameConst.Homes[houseIndex]
					to := gameConst.WorkPlaces[houseIndex]
					g.income += absInt(to[0]-from[0]) + absInt((to[1] - from[1]))
				}
				g.covered[houseIndex][k] = true
			}
		}
		g.money -= 5000
	} else if action.ActionType > 0 {
		g.money -= 100
	}
	g.money += g.income
	g.turn++
}

func (g *GameState) EstimateScore(gameConst *GameConst) int {
	return g.money + g.income*(MAX_TURN-g.turn)
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	NumPeople      int
	InitialMoney   int
	Homes          [][2]int
	WorkPlaces     [][2]int
	Poss           [][2]int
	sumFares       int
	Fares          []int
	distances      [BOARD_WIDTH * BOARD_WIDTH][BOARD_WIDTH * BOARD_WIDTH]int // 各マス間の距離
	IncludedPoints [BOARD_WIDTH * BOARD_WIDTH][2][]int                       // 各地点に駅を設置した場合に含まれる建物の列
	effects        [BOARD_WIDTH * BOARD_WIDTH][BOARD_WIDTH * BOARD_WIDTH]int // 各駅間の効果量
	potential      [BOARD_WIDTH * BOARD_WIDTH]int

	candidates []int

	nIJs    [BOARD_WIDTH * BOARD_WIDTH][4]int
	nearIJs [BOARD_WIDTH * BOARD_WIDTH][13]int
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	g.candidates = make([]int, 0)
	g.Random = rand.New(rand.NewSource(runner.randSeed))
	// g.Random = rand.New(rand.NewSource(0))
	for ij0 := 0; ij0 < BOARD_WIDTH*BOARD_WIDTH; ij0++ {
		for ij1 := 0; ij1 < BOARD_WIDTH*BOARD_WIDTH; ij1++ {
			g.distances[ij0][ij1] = calcdistance(ij0, ij1)
		}
	}
	for i := 0; i < BOARD_WIDTH; i++ {
		for j := 0; j < BOARD_WIDTH; j++ {
			ij := i*BOARD_WIDTH + j
			for d := 0; d < 4; d++ {
				g.nIJs[ij][d] = -1
				if i == 0 && d == 0 {
					continue
				}
				if j == 0 && d == 1 {
					continue
				}
				if j == BOARD_WIDTH-1 && d == 2 {
					continue
				}
				if i == BOARD_WIDTH-1 && d == 3 {
					continue
				}
				g.nIJs[ij][d] = ij + DIRS[d]
			}
			for d := range NEAR_DIJ {
				g.nearIJs[ij][d] = -1
				nij := ij + NEAR_DIJ[d]
				if nij < 0 || nij >= BOARD_WIDTH*BOARD_WIDTH {
					continue
				}
				if g.distances[ij][nij] > 2 {
					continue
				}
				g.nearIJs[ij][d] = ij + NEAR_DIJ[d]
			}
		}
	}
	return &g
}

func printAction(action *Action, gameState *GameState, gameConst *GameConst, fp *os.File, comment bool) {
	if fp == nil {
		if action.ActionType == -1 {
			fmt.Println(action.ActionType)
		} else {
			fmt.Println(action.ActionType, action.I, action.J)
		}
	} else {
		if action.ActionType == -1 {
			fmt.Fprintln(fp, action.ActionType)
		} else {
			fmt.Fprintln(fp, action.ActionType, action.I, action.J)
		}
	}
	// fmt.Println("アクションを出力")
}

type Result struct {
	loopNum int
	score   int
}

type Runner struct {
	caseIndex int
	randSeed  int64
}

func NewRunner(caseIndex int, randseed int64) *Runner {
	runner := Runner{}
	runner.caseIndex = caseIndex
	runner.randSeed = randseed

	return &runner
}

func test(testCase int) Result {
	start := time.Now()
	file := fmt.Sprintf("%04d", testCase)
	runner := NewRunner(testCase, time.Now().UnixNano())
	result := runner.doGame(file)
	duration := time.Since(start).Milliseconds()
	fmt.Fprintf(os.Stderr, "score:%d (case=%04d, %d ms)\n", result.score, testCase, duration)
	return result
}

func parallelTester(prefix int, bests [TEST_NUM]Result) [TEST_NUM]Result {
	fmt.Fprintln(os.Stderr, "waiting...")
	time.Sleep(3 * time.Second)
	fmt.Fprintln(os.Stderr, "start!")

	// channelを作成
	results := [TEST_NUM]Result{}
	var wg sync.WaitGroup
	channels := make(chan struct{}, PARALLEL_NUM)

	wg.Add(TEST_NUM)
	// ケースを並行に実行
	for num := 0; num < TEST_NUM; num++ {
		channels <- struct{}{}
		go func(i int) {
			defer wg.Done()
			number := 0
			if REPEAT {
				number, _ = strconv.Atoi(FILE_NUMBER)
			} else {
				number = i + prefix
			}
			r := test(number)
			// }
			results[i] = r
			<-channels
		}(num)
	}
	wg.Wait()
	return results
}

func getBests() [TEST_NUM]Result {

	results := [TEST_NUM]Result{}

	scanner := getScanner(BEST_FILE)
	for caseIndex := 0; caseIndex < TEST_NUM; caseIndex++ {
		scanner.Scan()
		var caseNumber, score, loopNum int
		fmt.Sscan(scanner.Text(), &caseNumber, &score, &loopNum)
		results[caseIndex].score = score
		results[caseIndex].loopNum = loopNum

	}
	return results
}

func main() {
	// defer profile.Start(profile.ProfilePath(".")).Stop()
	if SUCCESSIVE && LOCAL_TEST {
		// ローカルでの複数実行モード

		timeStamp := time.Now().Unix()
		if RECORD_CODE {
			// 現コードを保存
			dir := fmt.Sprintf("%s\\code_%d-%d_%d", CODE_FILE, PREFIX, PREFIX+TEST_NUM-1, timeStamp)
			os.Mkdir(dir, 0755)
			codeReadPath := "main.go"
			codePrintPath := fmt.Sprintf("%s\\code_%d-%d_%d\\code_%d-%d_%d.go", CODE_FILE, PREFIX, PREFIX+TEST_NUM-1, timeStamp, PREFIX, PREFIX+TEST_NUM-1, timeStamp)
			scanner := getScanner(codeReadPath)
			fp2, _ := os.Create(codePrintPath)
			loop := 0
			for {
				remains := scanner.Scan()
				if !remains {
					break
				}
				if loop == 0 {
					fmt.Fprintf(fp2, "// last local submission: %d\n", timeStamp)
				} else {
					fmt.Fprintln(fp2, scanner.Text())
				}
				loop++
			}
			fp2.Close()
		}

		bests := [TEST_NUM]Result{}
		if USE_RELATIVE_SCORE {
			bests = getBests()
		}
		// 複数ケースを並行で実行
		results := parallelTester(PREFIX, bests)

		// スコア・相対スコア計算
		sumScore := 0
		sumRelativeScore := 0.0
		minsScore := INF
		for i := 0; i < TEST_NUM; i++ {
			sumScore += results[i].score
			if USE_RELATIVE_SCORE {
				if MINIMIZE {
					if results[i].score != 0 {
						sumRelativeScore += math.Min(float64(bests[i].score)/float64(results[i].score), 1)
					} else {
						sumRelativeScore++
					}
				} else {
					if bests[i].score != 0 {
						sumRelativeScore += float64(results[i].score) / float64(bests[i].score)
					} else {
						sumRelativeScore++
					}
				}
			}
			if results[i].score == INF {
				fmt.Fprintf(os.Stderr, "WARN: case %d failed.\n", i)
			}
			minsScore = minInt(minsScore, results[i].score)
		}
		fmt.Println(sumRelativeScore)

		// 結果の出力
		fmt.Fprintf(os.Stderr, "\n---END---\n%d case finished.\n", TEST_NUM)
		fmt.Fprintln(os.Stderr, "sumScore:", sumScore)
		if USE_RELATIVE_SCORE {
			fmt.Fprintln(os.Stderr, "sumRelativeScore:", sumRelativeScore)
		}
		fmt.Fprintf(os.Stderr, "\n")
		for i := 0; i < TEST_NUM; i++ {
			fmt.Fprintf(os.Stderr, "%d ", results[i].score)
		}
		fmt.Fprintln(os.Stderr, "")

		fmt.Fprintf(os.Stderr, "// last local submission: %d\n", timeStamp)

		if !REPEAT {
			if RECORD {
				// 複数実行結果をファイルに保存
				path := fmt.Sprintf("%s\\%d-%d_%d.txt", RECORD_FOLDER, PREFIX, PREFIX+TEST_NUM-1, timeStamp)
				fp, _ := os.Create(path)
				fmt.Fprintf(fp, "(%d.%d.%d)%s\n%t\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NAME, VERSION_VALID)
				for i := 0; i < TEST_NUM; i++ {
					fmt.Fprintf(fp, "%d %d %d\n", i, results[i].score, results[i].loopNum)
				}
				fp.Close()
			}
		} else {
			fmt.Fprintf(os.Stderr, "minScore: %d", minsScore)
		}
		fmt.Fprintf(os.Stderr, "\n")

	} else {
		// 単体ケースを実行
		caseIndex := 0
		if LOCAL_TEST {
			caseIndex, _ = strconv.Atoi(FILE_NUMBER)
		}
		runner := NewRunner(caseIndex, time.Now().UnixNano())
		result := runner.doGame(FILE_NUMBER)
		if LOCAL_TEST && DEBUG {
			fmt.Fprintf(os.Stderr, "score: %d\n", result.score)
		}
	}
}

type Action struct {
	ActionType int
	I          int
	J          int
}

func NewAction() *Action {
	a := Action{}
	a.ActionType = -1
	return &a
}

func (a *Action) Load(a2 *Action) {
	a.ActionType = a2.ActionType
	a.I = a2.I
	a.J = a2.J

}
func (a *Action) Copy() *Action {
	newa := NewAction()
	newa.Load(a)
	return newa
}

func getScore() float64 {
	return 0.0
}

func evaluate(gameState *GameState, gameConst *GameConst) float64 {
	value := 0.0
	return value
}

type Solver struct {
	start     *time.Time
	timeLimit int // solverに与えられた時間制限 microsecond
}

func NewSolver(timeLimit int) *Solver {
	s := Solver{}
	s.timeLimit = timeLimit
	return &s
}

func (s *Solver) Solve(gameState *GameState, gameConst *GameConst) ([MAX_TURN]*Action, int) {
	start := time.Now()
	_ = start
	maxActions := [MAX_TURN]*Action{}
	for i := range maxActions {
		maxActions[i] = NewAction()
	}
	maxScore := 0.0
	_ = maxScore

	// 貪欲を書くならここ

	// 貪欲ここまで
	candidates := [BOARD_WIDTH * BOARD_WIDTH]int{}
	for i := range candidates {
		candidates[i] = 1
	}

	// sa := NewSA()
	// sa.Init(10000*1000, 1, 0.04, NewStartAnnealingPlan(gameConst), -INF, gameConst)
	// sa.Run(gameConst)
	// fmt.Fprintln(os.Stderr, sa.best.stations)
	// for i := range sa.best.stations {
	// 	candidates[sa.best.stations[i]] = 1
	// }
	// 序盤(最後まで)の展開をビームサーチ
	bs := NewBeamSearch()
	bs.Init(15, 800, 0)
	state := NewBeamSearchState(gameConst)
	state.Init(gameConst)
	bestState := bs.Run(state, gameConst, candidates)

	// 改善を試みる
	fmt.Fprintln(os.Stderr, bs.Evaluate(bestState, gameConst, false))
	bestState.improve(gameConst)
	fmt.Fprintln(os.Stderr, bs.Evaluate(bestState, gameConst, false))
	newState := NewBeamSearchState(gameConst)
	for i := 0; i < 100000; i++ {
		if time.Since(start).Microseconds() > 2500*1000 {
			break
		}
		newState.Load(bestState)
		r := gameConst.Random.Intn(6)
		valid := true
		if r < 4 {
			valid = newState.improve2(gameConst)
		} else if r < 5 {
			valid = newState.improve4(gameConst)
		} else {
			valid = newState.improve3(gameConst)

		}

		if valid {
			newValue := bs.EvaluatetrueScore(newState, gameConst, false)
			oldValue := bs.EvaluatetrueScore(bestState, gameConst, false)
			if newValue >= oldValue {
				// if r==4{
				//
				// 	fmt.Fprintln(os.Stderr,oldValue,"->",newValue)
				// }
				bestState.Load(newState)
			}
		}
	}
	fmt.Fprintln(os.Stderr, bs.Evaluate(bestState, gameConst, false))
	// bs = NewBeamSearch()
	// bs.Init(10, 800, 1)
	//
	// bestState = bs.Run(bestState.Copy(gameConst), gameConst, candidates)
	// fmt.Fprintln(os.Stderr, bs.Evaluate(bestState, gameConst, false))
	/*sa := NewCA()
	plan := NewCoverAnnealingPlan(gameConst)

	for i := range bestState.IJs {
		if i > 15 {
			continue
		}
		action := NewCoverAnnealingAction()
		action.ij = bestState.IJs[i]
		action.ij2 = -1
		plan.ApplyAction(gameConst, action, true)
	}
	fmt.Fprintln(os.Stderr, plan.Evaluate(gameConst), plan.placed)
	sa.Init(10000*1000, 100, 0.1, plan, -INF, gameConst)
	sa.Run(gameConst)
	fmt.Fprintln(os.Stderr, sa.best.placed)
	candidates = sa.best.placed
	bs = NewBeamSearch()
	bs.Init(10, 800, 0)
	state = NewBeamSearchState(gameConst)
	state.Init(gameConst)
	bestState = bs.Run(state, gameConst, candidates)*/

	// 最小全域木を作る
	// for i := range candidates {
	// 	candidates[i] = 0
	// }
	// for i := range bs.bestState.IJs {
	// 	candidates[bestState.IJs[i]] = 1
	// }
	// bs.Init(20, 800)
	// bestState = bs.Run(NewBeamSearchState(gameConst), gameConst, candidates)
	// _ = bestState
	if DEBUG {
		for i := range bestState.procedures {
			if bestState.procedures[i][0] == 0 {
				fmt.Fprintln(os.Stderr, "Vertex", bestState.IJs[bestState.procedures[i][1]]/BOARD_WIDTH, bestState.IJs[bestState.procedures[i][1]]%BOARD_WIDTH)
			} else {
				edgeindex := bestState.procedures[i][1]
				fmt.Fprintln(os.Stderr, "Edge", bestState.IJs[bestState.edges[edgeindex][0]]/BOARD_WIDTH, bestState.IJs[bestState.edges[edgeindex][0]]%BOARD_WIDTH, "->", bestState.IJs[bestState.edges[edgeindex][1]]/BOARD_WIDTH, bestState.IJs[bestState.edges[edgeindex][1]]%BOARD_WIDTH)

			}
		}
	}

	// relaxしておく
	for vertexIndex := range bestState.IJs {
		for portIndex, edgeIndex := range bestState.IncidentEdges[vertexIndex] {
			if edgeIndex == -1 {
				continue
			}
			k := 0
			if bestState.edges[edgeIndex][1] == vertexIndex {
				k = 1
			}
			bestState.PortCandidates[vertexIndex][portIndex] = calcPortCandidates(bs.bestState.IJs[bs.bestState.edges[edgeIndex][k]], bs.bestState.IJs[bs.bestState.edges[edgeIndex][1-k]], true, false)
		}
	}

	// RouteSolver に入力データをセット
	rs := RouteSolver{
		boardWidth:     BOARD_WIDTH,
		boardHeight:    BOARD_WIDTH,
		IJs:            bestState.IJs,
		edges:          bestState.edges,
		procedures:     bestState.procedures,
		incidentEdges:  bestState.IncidentEdges,
		portCandidates: bestState.PortCandidates,
	}
	rs.Init(gameConst)
	// Run して最終盤面を得る
	board, ok := rs.Run(gameConst)
	if DEBUG {
		for i, v := range board {
			fmt.Fprintf(os.Stderr, "%3d ", v)
			if i%BOARD_WIDTH == BOARD_WIDTH-1 {
				fmt.Fprintln(os.Stderr)
			}
		}
	}

	actionQueue := make([]*Action, 0, MAX_TURN)
	railed := [BOARD_WIDTH * BOARD_WIDTH]int{} // 線路敷設の方向
	for i := range railed {
		railed[i] = -1
	}
	rerunRouteSolver := false
	for {
		if !ok {
			rerunRouteSolver = true
		}
		if !rerunRouteSolver {

			// actionqueueに変換
			for procedureIndex := range bestState.procedures {
				if bestState.procedures[procedureIndex][0] == 0 {
					// vertexの場合の処理
					vertexIndex := bestState.procedures[procedureIndex][1]
					pos := bestState.IJs[vertexIndex]
					action := NewAction()
					action.ActionType = 0
					action.I = pos / BOARD_WIDTH
					action.J = pos % BOARD_WIDTH
					actionQueue = append(actionQueue, action)
					railed[pos] = 0
				} else {
					edgeIndex := bestState.procedures[procedureIndex][1]
					start := bestState.IJs[bestState.edges[edgeIndex][0]]
					end := bestState.IJs[bestState.edges[edgeIndex][1]]
					// edgeの場合の処理
					prev := -1
					now := start
					// startからendの一マス前まで線路を引く
					for now != end {
						if railed[now] == -1 {
							// 駅をまだ置いていないので線路でつなぐ。ひとつ前の線路を取得
							if now == start {
								for d := 0; d < 4; d++ {
									p := gameConst.nIJs[now][d]
									if p == -1 {
										continue
									}
									if railed[p] == -1 {
										continue
									}
									// 相手側から見てこちらに接続可能か見る
									if RAIL_CONNECTED[railed[p]][3-d] {
										prev = p
										break
									}
								}
							}
						}
						next := -1
						for d := 0; d < 4; d++ {
							p := gameConst.nIJs[now][d]
							if p == -1 {
								continue
							}
							if board[p] == procedureIndex && railed[p] == -1 {
								next = p
								break
							}
							if p == end {
								next = p
								break
							}
						}
						if railed[now] == -1 {
							// 前後から線路の形を決定して線路を置く
							action := NewAction()
							diff1 := prev - now
							diff2 := next - now
							if diff1 > diff2 {
								diff1, diff2 = diff2, diff1
							}
							if diff1 == DIRS[0] && diff2 == DIRS[1] {
								action.ActionType = 4
							}
							if diff1 == DIRS[0] && diff2 == DIRS[2] {
								action.ActionType = 5
							}
							if diff1 == DIRS[0] && diff2 == DIRS[3] {
								action.ActionType = 2
							}
							if diff1 == DIRS[1] && diff2 == DIRS[2] {
								action.ActionType = 1
							}
							if diff1 == DIRS[1] && diff2 == DIRS[3] {
								action.ActionType = 3
							}
							if diff1 == DIRS[2] && diff2 == DIRS[3] {
								action.ActionType = 6
							}
							action.I = now / BOARD_WIDTH
							action.J = now % BOARD_WIDTH
							actionQueue = append(actionQueue, action)

							railed[now] = action.ActionType
						}
						prev = now
						now = next
						if now == -1 {
							fmt.Fprintf(os.Stderr, "ERROR: pathfind failed (proc=%d)\n", procedureIndex)
							fmt.Fprintln(os.Stderr, gameConst.NumPeople)
							// 足掻く
							gomakaseru := false
							for d, nij := range gameConst.nIJs[end] {
								if nij == -1 {
									continue
								}
								if railed[nij] == -1 {
									continue
								}
								if RAIL_CONNECTED[railed[nij]][3-d] {
									// 隣から引っ張れるようなのでOK
									gomakaseru = true

								}
							}
							if !gomakaseru {
								rerunRouteSolver = true
							}
							now = end
							break
						}

					}
					if rerunRouteSolver {
						break
					}

				}
			}

		}
		if !rerunRouteSolver {
			break
		}
		// 再実行
		rs = RouteSolver{
			boardWidth:     BOARD_WIDTH,
			boardHeight:    BOARD_WIDTH,
			IJs:            bestState.IJs,
			edges:          bestState.edges,
			procedures:     bestState.procedures,
			incidentEdges:  bestState.IncidentEdges,
			portCandidates: bestState.PortCandidates,
		}
		rs.Init(gameConst)
		// Run して最終盤面を得る
		board, ok = rs.Run(gameConst)

		actionQueue = make([]*Action, 0, MAX_TURN)
		railed = [BOARD_WIDTH * BOARD_WIDTH]int{} // 線路敷設の方向
		for i := range railed {
			railed[i] = -1
		}
		rerunRouteSolver = false
		if !ok {
			rerunRouteSolver = true
		}

	}
	bestTurningPoint := 0
	bestV := 0
	for turningPoint := 0; turningPoint < len(actionQueue); turningPoint++ {
		v := decide(turningPoint, NewGameState(gameConst), actionQueue, gameConst)
		// fmt.Fprintln(os.Stderr, turningPoint, v)
		if v > bestV {
			bestTurningPoint = turningPoint
			bestV = v
		}
	}
	// bestTurn := 0
	// bestScore := gameState.money
	// for i := 0; i < len(actionQueue); {
	// 	if gameState.turn >= MAX_TURN {
	// 		break
	// 	}
	// 	if !gameState.Afford(actionQueue[i]) {
	// 		gameState.ApplyAction(gameConst, NewAction())
	// 		continue
	// 	}
	// 	maxActions[gameState.turn] = actionQueue[i]
	// 	gameState.ApplyAction(gameConst, actionQueue[i])
	// 	estimatedScore := gameState.EstimateScore(gameConst)
	// 	if estimatedScore > bestScore {
	// 		bestScore = estimatedScore
	// 		bestTurn = gameState.turn
	// 	}
	// 	i++
	// }
	bestScore := gameState.money
	bestTurn := 0
	independent := false
	index1 := 0
	index0 := 0
	for index1 < len(actionQueue) || index0 < len(actionQueue) {
		if gameState.turn >= MAX_TURN {
			break
		}
		// fmt.Fprintln(os.Stderr,gameState.turn,gameState.money,gameState.income)
		if index0 == bestTurningPoint {
			independent = true
			for {
				index0++
				if index0 >= len(actionQueue) {
					break
				}
				if actionQueue[index0].ActionType == 0 {
					break
				}
			}
		}
		if !independent {
			if !gameState.Afford(actionQueue[index1]) {
				gameState.ApplyAction(gameConst, NewAction())
				continue
			} else {
				maxActions[gameState.turn] = actionQueue[index1]
				gameState.ApplyAction(gameConst, actionQueue[index1])
				index1++
				index0++
			}
		} else {
			if index0 < index1 {
				if gameState.Afford(actionQueue[index0]) {
					maxActions[gameState.turn] = actionQueue[index0]
					gameState.ApplyAction(gameConst, actionQueue[index0])
					for {
						index0++
						if index0 >= len(actionQueue) {
							break
						}
						if actionQueue[index0].ActionType == 0 {
							break
						}
					}
				} else {
					if index1 < len(actionQueue) {
						if gameState.Afford(actionQueue[index1]) {
							maxActions[gameState.turn] = actionQueue[index1]
							gameState.ApplyAction(gameConst, actionQueue[index1])
							for {
								index1++
								if index1 >= len(actionQueue) {
									break
								}
								if actionQueue[index1].ActionType != 0 {
									break
								}
							}
						} else {
							gameState.ApplyAction(gameConst, NewAction())
							continue
						}
					} else {
						gameState.ApplyAction(gameConst, NewAction())
						continue
					}
				}
			} else {
				if index1 < len(actionQueue) {
					if gameState.Afford(actionQueue[index1]) {
						maxActions[gameState.turn] = actionQueue[index1]
						gameState.ApplyAction(gameConst, actionQueue[index1])
						for {
							index1++
							if index1 >= len(actionQueue) {
								break
							}
							if actionQueue[index1].ActionType != 0 {
								break
							}
						}
					} else {
						gameState.ApplyAction(gameConst, NewAction())
						continue
					}
				}
			}

		}
		estimatedScore := gameState.EstimateScore(gameConst)
		if estimatedScore > bestScore {
			bestTurn = gameState.turn
			bestScore = estimatedScore
		}
	}
	// fmt.Fprintln(os.Stderr, gameState)
	fmt.Fprintln(os.Stderr, bs.bestValue)
	fmt.Fprintln(os.Stderr, bestScore, bestTurn)
	for i := bestTurn; i < MAX_TURN; i++ {
		maxActions[i].ActionType = -1
	}

	score := bestScore

	return maxActions, score
}

func decide(turningPoint int, gameState *GameState, actionQueue []*Action, gameConst *GameConst) int {
	bestScore := gameState.money
	independent := false
	index1 := 0
	index0 := 0
	for index1 < len(actionQueue) || index0 < len(actionQueue) {
		if gameState.turn >= MAX_TURN {
			break
		}
		if index0 == turningPoint {
			independent = true
			for {
				index0++
				if index0 >= len(actionQueue) {
					break
				}
				if actionQueue[index0].ActionType == 0 {
					break
				}
			}
		}
		if !independent {
			if !gameState.Afford(actionQueue[index1]) {
				gameState.ApplyAction(gameConst, NewAction())
				continue
			} else {
				gameState.ApplyAction(gameConst, actionQueue[index1])
				index1++
				index0++
			}
		} else {
			if index0 < index1 {
				if gameState.Afford(actionQueue[index0]) {
					gameState.ApplyAction(gameConst, actionQueue[index0])
					for {
						index0++
						if index0 >= len(actionQueue) {
							break
						}
						if actionQueue[index0].ActionType == 0 {
							break
						}
					}
				} else {
					if index1 < len(actionQueue) {
						if gameState.Afford(actionQueue[index1]) {
							gameState.ApplyAction(gameConst, actionQueue[index1])
							for {
								index1++
								if index1 >= len(actionQueue) {
									break
								}
								if actionQueue[index1].ActionType != 0 {
									break
								}
							}
						} else {
							gameState.ApplyAction(gameConst, NewAction())
							continue
						}
					} else {
						gameState.ApplyAction(gameConst, NewAction())
						continue
					}
				}
			} else {
				if index1 < len(actionQueue) {
					if gameState.Afford(actionQueue[index1]) {
						gameState.ApplyAction(gameConst, actionQueue[index1])
						for {
							index1++
							if index1 >= len(actionQueue) {
								break
							}
							if actionQueue[index1].ActionType != 0 {
								break
							}
						}
					} else {
						gameState.ApplyAction(gameConst, NewAction())
						continue
					}
				}
			}

		}
		estimatedScore := gameState.EstimateScore(gameConst)
		if estimatedScore > bestScore {
			bestScore = estimatedScore
		}
	}
	return bestScore
}

func (runner *Runner) doGame(file string) Result {
	start := time.Now()
	result := Result{}
	scanner := getScanner(FILE + "\\" + file + ".txt")

	// 初期値設定処理

	gameConst := NewGameConst(runner)
	gameConst.SetScanner(scanner)
	// データ取得
	getInputs(gameConst)

	gameState := NewGameState(gameConst)

	// 初期値設定処理　ここまで
	solver := NewSolver(TIME_LIMIT)
	solver.start = &start
	actions, score := solver.Solve(gameState, gameConst)

	// アクションを出力
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		for i := range actions {
			printAction(actions[i], gameState, gameConst, nil, false)
		}
	}
	if LOCAL_TEST && SUCCESSIVE {
		outPath := fmt.Sprintf("ignore\\out\\%04d.txt", runner.caseIndex)
		fp, _ := os.Create(outPath)
		for i := range actions {
			printAction(actions[i], gameState, gameConst, fp, false)
		}
	}

	if LOCAL_TEST {
		ok := validate(gameConst, actions)
		if !ok {
			fmt.Fprintln(os.Stderr, "failed case=", runner.caseIndex)
			panic("")
		}
		// score = calcScore(gameState, gameConst)
	}

	result.score = score

	return result
}

func validate(gameConst *GameConst, actions [MAX_TURN]*Action) bool {
	board := [BOARD_WIDTH * BOARD_WIDTH]int{}
	for i := range board {
		board[i] = -1
	}
	initialStation := actions[0].I*BOARD_WIDTH + actions[0].J
	for i := range actions {
		if actions[i].ActionType == -1 {
			continue
		}
		ij := actions[i].I*BOARD_WIDTH + actions[i].J
		board[ij] = actions[i].ActionType
		if actions[i].ActionType == 0 {
			// 最初の鉄道につながっているか確認
			visited := make([]bool, BOARD_WIDTH*BOARD_WIDTH)
			queue := make([]int, BOARD_WIDTH*BOARD_WIDTH)
			popindex := 0
			pushindex := 0
			visited[ij] = true
			queue[pushindex] = ij
			pushindex++
			for popindex < pushindex {
				p := queue[popindex]
				popindex++
				for d, np := range gameConst.nIJs[p] {
					if np == -1 {
						continue
					}
					if board[np] == -1 {
						continue
					}
					if visited[np] {
						continue
					}
					if !RAIL_CONNECTED[board[p]][d] || !RAIL_CONNECTED[board[np]][3-d] {
						continue
					}
					visited[np] = true
					queue[pushindex] = np
					pushindex++
				}
			}
			if !visited[initialStation] {
				fmt.Fprintln(os.Stderr, "not connected!!!!!!! turn:", i)
				return false
			}
		}
	}

	return true

}
func calcScore(gameState *GameState, gameConst *GameConst) int {
	score := 0
	return score
}

type BeamItem struct {
	prevIndex  int               // 遷移元の順位
	prevDepth  int               // 遷移元の順位
	prevAction *BeamSearchAction // 遷移元からここに来るために必要なAction

	beamValue  float64 // この状態の評価値
	totalValue float64
}

// 盤面情報などを扱う構造体
type BeamSearchState struct {
	IJs            []int
	IncidentEdges  [][4]int // 接続している辺
	PortCandidates [][4]int // 接続している辺のport候補 4bitでもつ
	counts         []int
	edges          [][2]int
	procedures     [][2]int // [0]: 0 -> vertex 1 -> edge [1]: index of vertex (edge)
	field          [BOARD_WIDTH * BOARD_WIDTH]int
	numCovered     [][2]int

	potentials [BOARD_WIDTH * BOARD_WIDTH]int
	// incomes    [BOARD_WIDTH * BOARD_WIDTH]int

	railStock int

	income    int
	potential int
	money     int
	turn      int
}

// NewBeamSearchState() 新しい状態を生成する関数
func NewBeamSearchState(gameConst *GameConst) *BeamSearchState {
	state := &BeamSearchState{}
	state.IJs = make([]int, 0)
	state.IncidentEdges = make([][4]int, 0)
	state.PortCandidates = make([][4]int, 0)
	state.counts = make([]int, 0)
	state.edges = make([][2]int, 0)
	state.procedures = make([][2]int, 0)
	for i := range state.field {
		state.field[i] = 0
	}
	state.numCovered = make([][2]int, gameConst.NumPeople)

	state.railStock = 0
	state.income = 0
	state.money = gameConst.InitialMoney
	state.turn = 0
	return state
}

func (g *BeamSearchState) Init(gameConst *GameConst) {
	for i := range g.potentials {
		g.potentials[i] = gameConst.potential[i]
	}
	// for i := range g.numCovered {
	// 	g.numCovered[i][0] = 0
	// 	g.numCovered[i][1] = 0
	// }

	// g.railStock = 0
	// g.income = 0
	// g.money = gameConst.InitialMoney
	// g.turn = 0
}

// Copy() Stateをコピーする関数
func (g *BeamSearchState) Copy(gameConst *GameConst) *BeamSearchState {
	newg := NewBeamSearchState(gameConst)
	newg.Load(g)
	return newg
}

// Load() 既存のStateを上書きする関数
func (g *BeamSearchState) Load(gameState *BeamSearchState) {
	g.IJs = g.IJs[:0]
	g.IJs = append(g.IJs, gameState.IJs...)
	g.counts = g.counts[:0]
	g.counts = append(g.counts, gameState.counts...)
	g.IncidentEdges = g.IncidentEdges[:0]
	g.IncidentEdges = append(g.IncidentEdges, gameState.IncidentEdges...)
	g.PortCandidates = g.PortCandidates[:0]
	g.PortCandidates = append(g.PortCandidates, gameState.PortCandidates...)
	g.edges = g.edges[:0]
	g.edges = append(g.edges, gameState.edges...)
	g.procedures = g.procedures[:0]
	g.procedures = append(g.procedures, gameState.procedures...)
	g.railStock = gameState.railStock
	g.income = gameState.income
	g.money = gameState.money
	g.turn = gameState.turn
	g.potential = gameState.potential
	g.field = gameState.field
	for i := range g.numCovered {
		g.numCovered[i] = gameState.numCovered[i]
	}
	g.potentials = gameState.potentials
	// g.incomes = gameState.incomes

}

func (g *BeamSearchState) improve(gameConst *GameConst) {
	g.money = gameConst.InitialMoney
	g.potential = 0
	g.income = 0
	g.turn = 0
	for i := range g.field {
		g.field[i] = 0
	}
	for i := range g.numCovered {
		for k := range g.numCovered[i] {
			g.numCovered[i][k] = 0
		}
	}

	newProcedures := make([][2]int, 0)

	applied := make([]bool, len(g.procedures))
	numApplied := 0
	prevPos := -1
	prevType := -1

	i := 0
	for {
		if numApplied == len(g.procedures) {
			break
		}
		for applied[i] {
			i++
		}

		incomeMinus := 0
		if g.income > 700 {
			incomeMinus = 100
		}
		neededmoney := 0
		neededturn := 0
		neededRail := 0
		procIndex := i
		if g.procedures[procIndex][0] == 1 && prevType == 1 {

			if g.income > 2000 && g.money+g.income >= 4900 {
				for {
					procIndex++
					if g.procedures[procIndex][0] == 1 {
						continue
					}
					v := g.procedures[procIndex][1]
					if v != prevPos {
						continue
					}
					break

				}
			}
		}
		if g.procedures[procIndex][0] == 1 {
			edge := g.edges[g.procedures[procIndex][1]]
			p0 := g.IJs[edge[0]]
			p1 := g.IJs[edge[1]]
			distance := gameConst.distances[p0][p1]
			neededturn += distance
			neededRail += distance - 1
			neededmoney += 100 * distance
			neededmoney -= incomeMinus * distance
			prevPos = edge[1]
			prevType = 1
		} else {
			neededmoney += 4900
			if prevPos != g.procedures[procIndex][1] {
				neededmoney += 100
				neededturn += 1
				neededRail += 1
			}
			prevType = 0
		}
		neededturn2 := 0
		if neededmoney <= g.money {
			neededturn2 = 0
		} else {
			if g.income-incomeMinus == 0 {
				neededturn2 = MAX_TURN
			} else {
				neededturn2 = (neededmoney-g.money-1)/(g.income-incomeMinus) + 1
			}
		}
		if g.procedures[procIndex][0] == 1 {
			// edge
			if neededturn2 >= neededturn {
				g.money += (neededturn2) * (g.income - incomeMinus)
				g.turn += (neededturn2)
			} else {
				if neededturn-neededturn2 <= (g.railStock) {
					g.money += (0) * (g.income - incomeMinus)
					g.turn += (0)
					g.railStock -= neededturn - neededturn2
				} else {
					g.money += (neededturn - neededturn2 - g.railStock) * (g.income - incomeMinus)
					g.turn += (neededturn - neededturn2 - g.railStock)
					g.railStock = 0

				}
			}
		} else {
			// vertex
			if neededturn2 >= 1 {
				g.money += (neededturn2) * (g.income - incomeMinus)
				if incomeMinus > 0 {
					g.railStock += neededturn2 - neededturn
				}
				g.turn += (neededturn2)
			} else {
				g.money += (neededturn) * (g.income - incomeMinus)
				// if incomeMinus > 0 {
				// 	g.railStock += neededturn2 - neededturn
				// 	g.railStock = maxInt(0, g.railStock)
				// }
				g.turn += (neededturn)
			}
		}
		g.money -= neededmoney

		if g.procedures[procIndex][0] == 0 {
			ij := g.IJs[g.procedures[procIndex][1]]
			for k := range gameConst.IncludedPoints[ij] {
				for _, peopleIndex := range gameConst.IncludedPoints[ij][k] {
					if g.numCovered[peopleIndex][k] == 0 {
						if g.numCovered[peopleIndex][1-k] > 0 {
							g.income += gameConst.Fares[peopleIndex]
						}
						g.potential += gameConst.Fares[peopleIndex]

						// 近隣のpotentialを減少させる
						pos := gameConst.Poss[peopleIndex][k]
						for _, nij := range gameConst.nearIJs[pos] {
							if nij == -1 {
								continue
							}
							g.potentials[nij] -= gameConst.Fares[peopleIndex]
						}

					}
					g.numCovered[peopleIndex][k]++
				}
			}
		}
		newProcedures = append(newProcedures, g.procedures[procIndex])
		applied[procIndex] = true
		numApplied++
	}
	g.procedures = g.procedures[:0]
	g.procedures = append(g.procedures, newProcedures...)
}
func (g *BeamSearchState) improve2(gameConst *GameConst) bool {
	vertexIndex := -1
	bestNearDir := -1
	for {
		vertexIndex = gameConst.Random.Intn(len(g.IJs))
		// 選ばれた頂点を動かしてみる
		prevSumDistance := 0
		start := g.IJs[vertexIndex]
		for _, edge := range g.IncidentEdges[vertexIndex] {
			if edge == -1 {
				continue
			}
			nv := g.edges[edge][0]
			if nv == vertexIndex {
				nv = g.edges[edge][1]
			}
			end := g.IJs[nv]
			prevSumDistance += gameConst.distances[start][end]
		}
		// 近隣二マス以内で最もよさそうなところに移動
		// crossは考えない
		bestSumDistnce := INF
		tiebreak := 0.0
		for d := 0; d < 13; d++ {
			start2 := gameConst.nearIJs[g.IJs[vertexIndex]][d]
			if start2 == -1 {
				continue
			}
			if d == 6 {
				continue
			}
			ok := true
			sumDistance := 0
			for _, edge := range g.IncidentEdges[vertexIndex] {
				if edge == -1 {
					continue
				}
				nv := g.edges[edge][0]
				if nv == vertexIndex {
					nv = g.edges[edge][1]
				}
				end := g.IJs[nv]
				sumDistance += gameConst.distances[start2][end]
				if start2 == end {
					ok = false
					break
				}
			}
			if !ok {
				continue
			}
			sumDistance = maxInt(sumDistance, prevSumDistance)
			if sumDistance < bestSumDistnce {
				bestSumDistnce = sumDistance
				bestNearDir = d
				tiebreak = gameConst.Random.Float64()
			}
			if sumDistance == bestSumDistnce {
				r := gameConst.Random.Float64()
				if r > tiebreak {
					bestSumDistnce = sumDistance
					bestNearDir = d
					tiebreak = r
				}
			}
		}
		if bestSumDistnce <= prevSumDistance {
			break
		}
	}
	// 実際に動かす
	newij := gameConst.nearIJs[g.IJs[vertexIndex]][bestNearDir]
	g.IJs[vertexIndex] = newij
	// 各辺ごとに処理
	valid := true
	for i, edge := range g.IncidentEdges[vertexIndex] {
		if edge == -1 {
			continue
		}
		// portCandidateを更新
		v1 := g.edges[edge][0]
		v2 := g.edges[edge][1]
		if v2 == vertexIndex {
			v1, v2 = v2, v1
		}
		g.PortCandidates[v1][i] = calcPortCandidates(g.IJs[v1], g.IJs[v2], false, false)
		for j := range g.IncidentEdges[v2] {
			if g.IncidentEdges[v2][j] != edge {
				continue
			}
			g.PortCandidates[v2][j] = calcPortCandidates(g.IJs[v2], g.IJs[v1], false, false)
		}
	}
	for _, edge := range g.IncidentEdges[vertexIndex] {
		if edge == -1 {
			continue
		}
		//portcandidateが問題ないか確認
		v1 := g.edges[edge][0]
		v2 := g.edges[edge][1]
		if v2 == vertexIndex {
			v1, v2 = v2, v1
		}
		valid = valid && g.IsValidPortCandidate(v2)
	}
	valid = valid && g.IsValidPortCandidate(vertexIndex)

	for _, edge := range g.IncidentEdges[vertexIndex] {
		if edge == -1 {
			continue
		}
		// cross
		valid = valid && g.CrossCheck(edge)
	}
	if !valid {
		return false
	}

	g.money = gameConst.InitialMoney
	g.potential = 0
	g.income = 0
	g.turn = 0
	for i := range g.field {
		g.field[i] = 0
	}
	for i := range g.numCovered {
		for k := range g.numCovered[i] {
			g.numCovered[i][k] = 0
		}
	}

	prevPos := -1

	for i := 0; i < len(g.procedures); i++ {

		incomeMinus := 0
		if g.income > 700 {
			incomeMinus = 100
		}
		neededmoney := 0
		neededturn := 0
		neededRail := 0
		if g.procedures[i][0] == 1 {
			edge := g.edges[g.procedures[i][1]]
			p0 := g.IJs[edge[0]]
			p1 := g.IJs[edge[1]]
			distance := gameConst.distances[p0][p1]
			neededturn += distance
			neededRail += distance - 1
			neededmoney += 100 * distance
			neededmoney -= incomeMinus * distance
			prevPos = edge[1]
		} else {
			neededmoney += 4900
			if prevPos != g.procedures[i][1] {
				neededmoney += 100
				neededturn += 1
				neededRail += 1
			}
		}
		neededturn2 := 0
		if neededmoney <= g.money {
			neededturn2 = 0
		} else {
			if g.income-incomeMinus == 0 {
				neededturn2 = MAX_TURN
			} else {
				neededturn2 = (neededmoney-g.money-1)/(g.income-incomeMinus) + 1
			}
		}
		if g.procedures[i][0] == 1 {
			// edge
			if neededturn2 >= neededturn {
				g.money += (neededturn2) * (g.income - incomeMinus)
				g.turn += (neededturn2)
			} else {
				if neededturn-neededturn2 <= (g.railStock) {
					g.money += (0) * (g.income - incomeMinus)
					g.turn += (0)
					g.railStock -= neededturn - neededturn2
				} else {
					g.money += (neededturn - neededturn2 - g.railStock) * (g.income - incomeMinus)
					g.turn += (neededturn - neededturn2 - g.railStock)
					g.railStock = 0

				}
			}
		} else {
			// vertex
			if neededturn2 >= 1 {
				g.money += (neededturn2) * (g.income - incomeMinus)
				if incomeMinus > 0 {
					g.railStock += neededturn2 - neededturn
				}
				g.turn += (neededturn2)
			} else {
				g.money += (neededturn) * (g.income - incomeMinus)
				// if incomeMinus > 0 {
				// 	g.railStock += neededturn2 - neededturn
				// 	g.railStock = maxInt(0, g.railStock)
				// }
				g.turn += (neededturn)
			}
		}
		g.money -= neededmoney

		if g.procedures[i][0] == 0 {
			ij := g.IJs[g.procedures[i][1]]
			for k := range gameConst.IncludedPoints[ij] {
				for _, peopleIndex := range gameConst.IncludedPoints[ij][k] {
					if g.numCovered[peopleIndex][k] == 0 {
						if g.numCovered[peopleIndex][1-k] > 0 {
							g.income += gameConst.Fares[peopleIndex]
						}
						g.potential += gameConst.Fares[peopleIndex]

						// 近隣のpotentialを減少させる
						pos := gameConst.Poss[peopleIndex][k]
						for _, nij := range gameConst.nearIJs[pos] {
							if nij == -1 {
								continue
							}
							g.potentials[nij] -= gameConst.Fares[peopleIndex]
						}

					}
					g.numCovered[peopleIndex][k]++
				}
			}
		}
	}

	return valid
}
func (g *BeamSearchState) improve3(gameConst *GameConst) bool {
	vertexIndex := -1
	parentEdge := -1
	nearestEdge := -1
	promising := false
	edgeProcInvert := make([]int, len(g.edges))
	vertexProcInvert := make([]int, len(g.IJs))
	for i := range g.procedures {
		if g.procedures[i][0] == 0 {
			vertexProcInvert[g.procedures[i][1]] = i
		} else {
			edgeProcInvert[g.procedures[i][1]] = i
		}
	}
	// for i:=range g.edges{
	// 	if vertexProcInvert[g.edges[i][0]]==9{
	// 		fmt.Fprintln(os.Stderr,vertexProcInvert[g.edges[i][0]],edgeProcInvert[i],vertexProcInvert[g.edges[i][1]])
	// 	}
	// }
	// fmt.Fprintln(os.Stderr,"")
	for loop := 0; loop < 100; loop++ {
		vertexIndex = gameConst.Random.Intn(len(g.IJs))
		// 選ばれた頂点を近くのedgeに埋め込んでみる
		if g.counts[vertexIndex] > 3 {
			// 結合後が望み薄なのでスルー
			continue
		}
		count := 0
		procIndex := vertexProcInvert[vertexIndex]
		for _, edge := range g.IncidentEdges[vertexIndex] {
			if edge == -1 {
				continue
			}
			if edgeProcInvert[edge] < procIndex {
				parentEdge = edge
				count++
			}
		}
		if count >= 2 {
			// 頂点生成前に二辺存在しているため、そのままほかのedgeには挿入できない
			continue
		}
		if count == 0 {
			// 最初の点は扱いが難しいため暫定でなしとする
			continue
		}
		bestDiff := INF
		for i, edge := range g.edges {
			v := edge[0]
			if vertexProcInvert[v] > edgeProcInvert[parentEdge] {
				/*c := 0
				for j := range g.IncidentEdges[v] {
					if g.IncidentEdges[v][j] == -1 {
						continue
					}
					if edgeProcInvert[g.IncidentEdges[v][j]] < vertexProcInvert[v] {
						c++
					}
				}
				if c >= 2 {
					continue
				}*/
				continue
			}
			if edge[0] == vertexIndex || edge[1] == vertexIndex {
				continue
			}
			from := g.IJs[edge[0]]
			to := g.IJs[edge[1]]
			ij := g.IJs[vertexIndex]
			diff := gameConst.distances[from][ij] + gameConst.distances[ij][to] - gameConst.distances[from][to]
			if diff < bestDiff {
				bestDiff = diff
				nearestEdge = i
			}
		}
		if bestDiff > 2 {
			continue
		}
		promising = true
		break
	}
	if !promising {
		return false
	}

	valid := true
	// 候補が見つかったので実際に試してみる
	// if edgeProcInvert[parentEdge] != vertexProcInvert[vertexIndex]-1 {
	// 	fmt.Fprintln(os.Stderr, edgeProcInvert[parentEdge], vertexProcInvert[vertexIndex], g.counts[vertexIndex])
	// 	panic("")
	// }

	fromIndex := g.edges[nearestEdge][0]
	toIndex := g.edges[nearestEdge][1]
	parentVertex := g.edges[parentEdge][0]

	// fmt.Fprintln(os.Stderr,"a",g.counts[fromIndex],g.counts[vertexIndex],g.counts[toIndex])

	// edge情報を削除
	g.deleteEdgeInfo(parentEdge)
	g.deleteEdgeInfo(nearestEdge)
	// fmt.Fprintln(os.Stderr,"b",g.counts[fromIndex],g.counts[vertexIndex],g.counts[toIndex])
	if edgeProcInvert[parentEdge] > edgeProcInvert[nearestEdge] {
		g.edges[nearestEdge][0] = fromIndex
		g.edges[nearestEdge][1] = vertexIndex
		g.edges[parentEdge][0] = vertexIndex
		g.edges[parentEdge][1] = toIndex
	} else {
		g.edges[nearestEdge][0] = vertexIndex
		g.edges[nearestEdge][1] = toIndex
		g.edges[parentEdge][0] = fromIndex
		g.edges[parentEdge][1] = vertexIndex
	}

	g.addEdgeInfo(parentEdge)
	// fmt.Fprintln(os.Stderr,"c",g.counts[fromIndex],g.counts[vertexIndex],g.counts[toIndex])
	g.addEdgeInfo(nearestEdge)
	// fmt.Fprintln(os.Stderr,"d",g.counts[fromIndex],g.counts[vertexIndex],g.counts[toIndex])

	valid = valid && g.IsValidPortCandidate(fromIndex)
	valid = valid && g.IsValidPortCandidate(toIndex)
	valid = valid && g.IsValidPortCandidate(vertexIndex)

	valid = valid && g.CrossCheck(parentEdge)
	valid = valid && g.CrossCheck(nearestEdge)

	if !valid {
		return false
	}

	newProcedures := make([][2]int, len(g.procedures))
	if edgeProcInvert[parentEdge] > edgeProcInvert[nearestEdge] {
		// edgeの生成が早まるため、ほかの順序が変更になる
		minIndex := edgeProcInvert[nearestEdge] + 1
		maxIndex := edgeProcInvert[parentEdge] - 1
		for i := range edgeProcInvert {
			if minIndex <= edgeProcInvert[i] && edgeProcInvert[i] <= maxIndex {
				edgeProcInvert[i] += 1
			}
		}
		for i := range vertexProcInvert {
			if minIndex <= vertexProcInvert[i] && vertexProcInvert[i] <= maxIndex {
				vertexProcInvert[i] += 1
			}
		}
		edgeProcInvert[parentEdge] = edgeProcInvert[nearestEdge] + 1
	}

	if vertexProcInvert[parentVertex] != 0 {
		ppedge := -1
		proc2 := vertexProcInvert[parentVertex] //二番手の値
		proc1 := proc2                          // 最小値
		for i := range g.IncidentEdges[parentVertex] {
			if g.IncidentEdges[parentVertex][i] == -1 {
				continue
			}
			if edgeProcInvert[g.IncidentEdges[parentVertex][i]] < proc1 {
				ppedge = g.IncidentEdges[parentVertex][i]
				proc1 = edgeProcInvert[g.IncidentEdges[parentVertex][i]]
			}
		}
		for i := range g.IncidentEdges[parentVertex] {
			if g.IncidentEdges[parentVertex][i] == -1 {
				continue
			}
			if edgeProcInvert[g.IncidentEdges[parentVertex][i]] == proc1 {
				continue
			}
			if edgeProcInvert[g.IncidentEdges[parentVertex][i]] < proc2 {
				proc2 = edgeProcInvert[g.IncidentEdges[parentVertex][i]]
			}
		}
		if proc1 < proc2-1 {
			for i := range edgeProcInvert {
				if proc1 < edgeProcInvert[i] && edgeProcInvert[i] < proc2 {
					edgeProcInvert[i] -= 1
				}
			}
			for i := range vertexProcInvert {
				if proc1 < vertexProcInvert[i] && vertexProcInvert[i] < proc2 {
					vertexProcInvert[i] -= 1
				}
			}
		}
		edgeProcInvert[ppedge] = proc2 - 1
	}
	for i := range edgeProcInvert {
		newProcedures[edgeProcInvert[i]][0] = 1
		newProcedures[edgeProcInvert[i]][1] = i
	}
	for i := range vertexProcInvert {
		newProcedures[vertexProcInvert[i]][0] = 0
		newProcedures[vertexProcInvert[i]][1] = i
	}
	g.procedures = newProcedures

	g.money = gameConst.InitialMoney
	g.potential = 0
	g.income = 0
	g.turn = 0
	for i := range g.field {
		g.field[i] = 0
	}
	for i := range g.numCovered {
		for k := range g.numCovered[i] {
			g.numCovered[i][k] = 0
		}
	}

	prevPos := -1

	for i := 0; i < len(g.procedures); i++ {

		incomeMinus := 0
		if g.income > 700 {
			incomeMinus = 100
		}
		neededmoney := 0
		neededturn := 0
		neededRail := 0
		if g.procedures[i][0] == 1 {
			edge := g.edges[g.procedures[i][1]]
			p0 := g.IJs[edge[0]]
			p1 := g.IJs[edge[1]]
			distance := gameConst.distances[p0][p1]
			neededturn += distance
			neededRail += distance - 1
			neededmoney += 100 * distance
			neededmoney -= incomeMinus * distance
			prevPos = edge[1]
		} else {
			neededmoney += 4900
			if prevPos != g.procedures[i][1] {
				neededmoney += 100
				neededturn += 1
				neededRail += 1
			}
		}
		neededturn2 := 0
		if neededmoney <= g.money {
			neededturn2 = 0
		} else {
			if g.income-incomeMinus == 0 {
				neededturn2 = MAX_TURN
			} else {
				neededturn2 = (neededmoney-g.money-1)/(g.income-incomeMinus) + 1
			}
		}
		if g.procedures[i][0] == 1 {
			// edge
			if neededturn2 >= neededturn {
				g.money += (neededturn2) * (g.income - incomeMinus)
				g.turn += (neededturn2)
			} else {
				if neededturn-neededturn2 <= (g.railStock) {
					g.money += (0) * (g.income - incomeMinus)
					g.turn += (0)
					g.railStock -= neededturn - neededturn2
				} else {
					g.money += (neededturn - neededturn2 - g.railStock) * (g.income - incomeMinus)
					g.turn += (neededturn - neededturn2 - g.railStock)
					g.railStock = 0

				}
			}
		} else {
			// vertex
			if neededturn2 >= 1 {
				g.money += (neededturn2) * (g.income - incomeMinus)
				if incomeMinus > 0 {
					g.railStock += neededturn2 - neededturn
				}
				g.turn += (neededturn2)
			} else {
				g.money += (neededturn) * (g.income - incomeMinus)
				// if incomeMinus > 0 {
				// 	g.railStock += neededturn2 - neededturn
				// 	g.railStock = maxInt(0, g.railStock)
				// }
				g.turn += (neededturn)
			}
		}
		g.money -= neededmoney

		if g.procedures[i][0] == 0 {
			ij := g.IJs[g.procedures[i][1]]
			for k := range gameConst.IncludedPoints[ij] {
				for _, peopleIndex := range gameConst.IncludedPoints[ij][k] {
					if g.numCovered[peopleIndex][k] == 0 {
						if g.numCovered[peopleIndex][1-k] > 0 {
							g.income += gameConst.Fares[peopleIndex]
						}
						g.potential += gameConst.Fares[peopleIndex]

						// 近隣のpotentialを減少させる
						pos := gameConst.Poss[peopleIndex][k]
						for _, nij := range gameConst.nearIJs[pos] {
							if nij == -1 {
								continue
							}
							g.potentials[nij] -= gameConst.Fares[peopleIndex]
						}

					}
					g.numCovered[peopleIndex][k]++
				}
			}
		}
	}

	return valid
}

func (g *BeamSearchState) improve4(gameConst *GameConst) bool {
	edgeIndex := -1
	newFromIndex := -1
	promising := false
	edgeProcInvert := make([]int, len(g.edges))
	vertexProcInvert := make([]int, len(g.IJs))
	for i := range g.procedures {
		if g.procedures[i][0] == 0 {
			vertexProcInvert[g.procedures[i][1]] = i
		} else {
			edgeProcInvert[g.procedures[i][1]] = i
		}
	}
	// for i:=range g.edges{
	// 	if vertexProcInvert[g.edges[i][0]]==9{
	// 		fmt.Fprintln(os.Stderr,vertexProcInvert[g.edges[i][0]],edgeProcInvert[i],vertexProcInvert[g.edges[i][1]])
	// 	}
	// }
	// fmt.Fprintln(os.Stderr,"")
	for loop := 0; loop < 100; loop++ {
		edgeIndex = gameConst.Random.Intn(len(g.edges))
		ij := g.IJs[g.edges[edgeIndex][1]]
		// 選ばれた辺のfromを付け替えてみる
		bestDistance := INF
		for i := range g.IJs {
			if vertexProcInvert[i] > edgeProcInvert[edgeIndex] {
				continue
			}
			if g.counts[i] == 4 {
				continue
			}
			ij2 := g.IJs[i]
			distance := gameConst.distances[ij2][ij]
			if distance < bestDistance {
				bestDistance = distance
				newFromIndex = i
			}
		}

		if bestDistance > gameConst.distances[g.IJs[g.edges[edgeIndex][0]]][g.IJs[g.edges[edgeIndex][1]]] {
			continue
		}

		promising = true
		break
	}
	if !promising {
		return false
	}

	valid := true
	// 候補が見つかったので実際に試してみる
	// if edgeProcInvert[parentEdge] != vertexProcInvert[vertexIndex]-1 {
	// 	fmt.Fprintln(os.Stderr, edgeProcInvert[parentEdge], vertexProcInvert[vertexIndex], g.counts[vertexIndex])
	// 	panic("")
	// }

	fromIndex := g.edges[edgeIndex][0]
	toIndex := g.edges[edgeIndex][1]

	// fmt.Fprintln(os.Stderr,"a",g.counts[fromIndex],g.counts[vertexIndex],g.counts[toIndex])

	// edge情報を削除
	g.deleteEdgeInfo(edgeIndex)
	g.edges[edgeIndex][0] = newFromIndex
	g.edges[edgeIndex][1] = toIndex

	g.addEdgeInfo(edgeIndex)

	valid = valid && g.IsValidPortCandidate(newFromIndex)
	valid = valid && g.IsValidPortCandidate(toIndex)

	valid = valid && g.CrossCheck(edgeIndex)

	if !valid {
		return false
	}

	newProcedures := make([][2]int, len(g.procedures))

	if vertexProcInvert[fromIndex] != 0 {
		ppedge := -1
		proc2 := vertexProcInvert[fromIndex] //二番手の値
		proc1 := proc2                       // 最小値
		for i := range g.IncidentEdges[fromIndex] {
			if g.IncidentEdges[fromIndex][i] == -1 {
				continue
			}
			if edgeProcInvert[g.IncidentEdges[fromIndex][i]] < proc1 {
				ppedge = g.IncidentEdges[fromIndex][i]
				proc1 = edgeProcInvert[g.IncidentEdges[fromIndex][i]]
			}
		}
		for i := range g.IncidentEdges[fromIndex] {
			if g.IncidentEdges[fromIndex][i] == -1 {
				continue
			}
			if edgeProcInvert[g.IncidentEdges[fromIndex][i]] == proc1 {
				continue
			}
			if edgeProcInvert[g.IncidentEdges[fromIndex][i]] < proc2 {
				proc2 = edgeProcInvert[g.IncidentEdges[fromIndex][i]]
			}
		}
		if proc1 < proc2-1 {
			for i := range edgeProcInvert {
				if proc1 < edgeProcInvert[i] && edgeProcInvert[i] < proc2 {
					edgeProcInvert[i] -= 1
				}
			}
			for i := range vertexProcInvert {
				if proc1 < vertexProcInvert[i] && vertexProcInvert[i] < proc2 {
					vertexProcInvert[i] -= 1
				}
			}
		}
		edgeProcInvert[ppedge] = proc2 - 1
	}
	for i := range edgeProcInvert {
		newProcedures[edgeProcInvert[i]][0] = 1
		newProcedures[edgeProcInvert[i]][1] = i
	}
	for i := range vertexProcInvert {
		newProcedures[vertexProcInvert[i]][0] = 0
		newProcedures[vertexProcInvert[i]][1] = i
	}
	g.procedures = newProcedures

	g.money = gameConst.InitialMoney
	g.potential = 0
	g.income = 0
	g.turn = 0
	for i := range g.field {
		g.field[i] = 0
	}
	for i := range g.numCovered {
		for k := range g.numCovered[i] {
			g.numCovered[i][k] = 0
		}
	}

	prevPos := -1

	for i := 0; i < len(g.procedures); i++ {

		incomeMinus := 0
		if g.income > 700 {
			incomeMinus = 100
		}
		neededmoney := 0
		neededturn := 0
		neededRail := 0
		if g.procedures[i][0] == 1 {
			edge := g.edges[g.procedures[i][1]]
			p0 := g.IJs[edge[0]]
			p1 := g.IJs[edge[1]]
			distance := gameConst.distances[p0][p1]
			neededturn += distance
			neededRail += distance - 1
			neededmoney += 100 * distance
			neededmoney -= incomeMinus * distance
			prevPos = edge[1]
		} else {
			neededmoney += 4900
			if prevPos != g.procedures[i][1] {
				neededmoney += 100
				neededturn += 1
				neededRail += 1
			}
		}
		neededturn2 := 0
		if neededmoney <= g.money {
			neededturn2 = 0
		} else {
			if g.income-incomeMinus == 0 {
				neededturn2 = MAX_TURN
			} else {
				neededturn2 = (neededmoney-g.money-1)/(g.income-incomeMinus) + 1
			}
		}
		if g.procedures[i][0] == 1 {
			// edge
			if neededturn2 >= neededturn {
				g.money += (neededturn2) * (g.income - incomeMinus)
				g.turn += (neededturn2)
			} else {
				if neededturn-neededturn2 <= (g.railStock) {
					g.money += (0) * (g.income - incomeMinus)
					g.turn += (0)
					g.railStock -= neededturn - neededturn2
				} else {
					g.money += (neededturn - neededturn2 - g.railStock) * (g.income - incomeMinus)
					g.turn += (neededturn - neededturn2 - g.railStock)
					g.railStock = 0

				}
			}
		} else {
			// vertex
			if neededturn2 >= 1 {
				g.money += (neededturn2) * (g.income - incomeMinus)
				if incomeMinus > 0 {
					g.railStock += neededturn2 - neededturn
				}
				g.turn += (neededturn2)
			} else {
				g.money += (neededturn) * (g.income - incomeMinus)
				// if incomeMinus > 0 {
				// 	g.railStock += neededturn2 - neededturn
				// 	g.railStock = maxInt(0, g.railStock)
				// }
				g.turn += (neededturn)
			}
		}
		g.money -= neededmoney

		if g.procedures[i][0] == 0 {
			ij := g.IJs[g.procedures[i][1]]
			for k := range gameConst.IncludedPoints[ij] {
				for _, peopleIndex := range gameConst.IncludedPoints[ij][k] {
					if g.numCovered[peopleIndex][k] == 0 {
						if g.numCovered[peopleIndex][1-k] > 0 {
							g.income += gameConst.Fares[peopleIndex]
						}
						g.potential += gameConst.Fares[peopleIndex]

						// 近隣のpotentialを減少させる
						pos := gameConst.Poss[peopleIndex][k]
						for _, nij := range gameConst.nearIJs[pos] {
							if nij == -1 {
								continue
							}
							g.potentials[nij] -= gameConst.Fares[peopleIndex]
						}

					}
					g.numCovered[peopleIndex][k]++
				}
			}
		}
	}

	return valid
}

func (g *BeamSearchState) deleteEdgeInfo(edgeIndex int) {
	for _, vertexIndex := range g.edges[edgeIndex] {
		k := 0
		for k < g.counts[vertexIndex] {
			if g.IncidentEdges[vertexIndex][k] == edgeIndex {
				g.IncidentEdges[vertexIndex][k] = -1
				g.IncidentEdges[vertexIndex][k] = g.IncidentEdges[vertexIndex][g.counts[vertexIndex]-1]
				g.IncidentEdges[vertexIndex][g.counts[vertexIndex]-1] = -1
				g.PortCandidates[vertexIndex][k] = 0b1111
				g.PortCandidates[vertexIndex][k] = g.PortCandidates[vertexIndex][g.counts[vertexIndex]-1]
				g.PortCandidates[vertexIndex][g.counts[vertexIndex]-1] = 0b1111
				g.counts[vertexIndex]--

				break
			} else {
				k++
			}
		}
	}
}
func (g *BeamSearchState) addEdgeInfo(edgeIndex int) {
	for i, vertexIndex := range g.edges[edgeIndex] {
		k := g.counts[vertexIndex]
		g.IncidentEdges[vertexIndex][k] = edgeIndex
		fromij := g.IJs[g.edges[edgeIndex][i]]
		toij := g.IJs[g.edges[edgeIndex][1-i]]
		g.PortCandidates[vertexIndex][k] = calcPortCandidates(fromij, toij, false, false)

		g.counts[vertexIndex]++

	}
}

func calcPortCandidates(ij0 int, ij1 int, relax bool, strict bool) int {
	i0 := ij0 / BOARD_WIDTH
	j0 := ij0 % BOARD_WIDTH
	i1 := ij1 / BOARD_WIDTH
	j1 := ij1 % BOARD_WIDTH
	angle := math.Atan2(float64(j1-j0), float64(i1-i0)) * 180 / math.Pi
	result := 0
	if strict && false {
		if math.Abs(angle-180) < 44.9 || math.Abs(angle+180) < 44.9 {
			result |= 1
		}
		if math.Abs(angle+90) < 44.9 {
			result |= 1 << 1
		}
		if math.Abs(angle-90) < 44.9 {
			result |= 1 << 2
		}
		if math.Abs(angle+0) < 44.9 {
			result |= 1 << 3
		}
	} else {
		if math.Abs(angle-180) < 89.9 || math.Abs(angle+180) < 89.9 {
			result |= 1
		}
		if math.Abs(angle+90) < 89.9 {
			result |= 1 << 1
		}
		if math.Abs(angle-90) < 89.9 {
			result |= 1 << 2
		}
		if math.Abs(angle+0) < 89.9 {
			result |= 1 << 3
		}
	}
	if !relax {
		if absInt(i0-i1) <= 1 {
			if j0 < j1 {
				result &= 0b0110
			}

		}

		if absInt(j0-j1) <= 1 {
			if j0 > j1 {
				result &= 0b1001
			}
		}
	}
	// if i0 == 7 && j0 == 34 && i1 == 5 && j1 == 35 {
	// 	fmt.Fprintf(os.Stderr, "%04b,%d,%d\n", result, i1, j1)
	// }
	return result
}
func (g *BeamSearchState) CrossCheck(edgeindex int) bool {
	from := g.IJs[g.edges[edgeindex][0]]
	to := g.IJs[g.edges[edgeindex][1]]
	return g.GeneralCrossCheck(edgeindex, from, to)
}
func (g *BeamSearchState) GeneralCrossCheck(excludeIndex int, from int, to int) bool {
	if from > to {
		from, to = to, from
	}
	ax := from / BOARD_WIDTH
	ay := from % BOARD_WIDTH
	bx := to / BOARD_WIDTH
	by := to % BOARD_WIDTH
	// if ax == bx || ay == by {
	// 	return false
	// }
	for i := range g.edges {
		if i == excludeIndex {
			continue
		}
		if g.IJs[g.edges[i][0]] == from || g.IJs[g.edges[i][1]] == from {
			continue
		}
		if g.IJs[g.edges[i][0]] == to || g.IJs[g.edges[i][1]] == to {
			continue
		}
		from2 := g.IJs[g.edges[i][0]]
		to2 := g.IJs[g.edges[i][1]]
		if from2 > to2 {
			from2, to2 = to2, from2
		}
		cx := from2 / BOARD_WIDTH
		cy := from2 % BOARD_WIDTH
		dx := to2 / BOARD_WIDTH
		dy := to2 % BOARD_WIDTH
		s := (ax-bx)*(cy-ay) - (ay-by)*(cx-ax)
		t := (ax-bx)*(dy-ay) - (ay-by)*(dx-ax)
		s2 := (cx-dx)*(ay-cy) - (cy-dy)*(ax-cx)
		t2 := (cx-dx)*(by-cy) - (cy-dy)*(bx-cx)
		if s*t <= 0 && s2*t2 <= 0 {
			if s*t == 0 && s2*t2 == 0 {
				if (from-to2)*(to-from2) < 0 {
					return false
				}
			} else {
				return false
			}
		}
	}
	return true
}

func (g *BeamSearchState) addField(from int, to int, mult int, gameConst *GameConst) {
	fromi := from / BOARD_WIDTH
	fromj := from % BOARD_WIDTH
	toi := to / BOARD_WIDTH
	toj := to % BOARD_WIDTH
	if fromi > toi {
		fromi, toi = toi, fromi
	}
	if fromj > toj {
		fromj, toj = toj, fromj
	}
	for i := fromi; i <= toi; i++ {
		for j := fromj; j <= toj; j++ {
			g.field[i*BOARD_WIDTH+j] += mult
		}
	}
}

// 行動を適用する関数
func (g *BeamSearchState) ApplyAction(action *BeamSearchAction, gameConst *GameConst) bool {
	broken := false
	if action.actionType == 2 {
		ij0 := action.IJ / (BOARD_WIDTH * BOARD_WIDTH)
		ij1 := action.IJ % (BOARD_WIDTH * BOARD_WIDTH)
		a1 := NewBeamSearchAction()
		a1.actionType = 0
		if action.param != -1 {
			a1.actionType = 1
		}
		a1.param = action.param
		a1.IJ = ij0
		a2 := NewBeamSearchAction()
		a2.actionType = 0
		a2.param = len(g.IJs)
		a2.IJ = ij1
		valid := g.ApplyAction(a1, gameConst)
		valid = valid && g.ApplyAction(a2, gameConst)
		return valid
	}
	valid := true

	procedures := g.procedures
	// fmt.Fprintln(os.Stderr, len(procedures), len(g.counts), action.param)

	// 頂点を追加
	g.IJs = append(g.IJs, action.IJ)
	g.IncidentEdges = append(g.IncidentEdges, [4]int{-1, -1, -1, -1})
	g.PortCandidates = append(g.PortCandidates, [4]int{0b1111, 0b1111, 0b1111, 0b1111})

	if action.actionType == 1 {
		// 前半部分を新しいedgeとしてedgeの更新
		fromIndex := g.edges[action.param][0]
		toIndex := g.edges[action.param][1]
		if gameConst.distances[g.IJs[fromIndex]][g.IJs[toIndex]] < gameConst.distances[g.IJs[fromIndex]][action.IJ]+gameConst.distances[action.IJ][g.IJs[toIndex]] {
			broken = true
		}

		from := g.IJs[fromIndex]
		to := g.IJs[toIndex]
		g.addField(from, to, -1, gameConst)

		g.edges = append(g.edges, [2]int{fromIndex, len(g.IJs) - 1})
		g.addField(from, action.IJ, 1, gameConst)

		g.edges[action.param][0] = len(g.IJs) - 1
		g.addField(action.IJ, to, 1, gameConst)

		// 接続情報追加
		g.IncidentEdges[len(g.IJs)-1][0] = action.param
		g.PortCandidates[len(g.IJs)-1][0] = calcPortCandidates(g.IJs[g.edges[action.param][0]], g.IJs[g.edges[action.param][1]], false, true)
		g.IncidentEdges[len(g.IJs)-1][1] = len(g.edges) - 1
		g.PortCandidates[len(g.IJs)-1][1] = calcPortCandidates(g.IJs[g.edges[len(g.edges)-1][1]], g.IJs[g.edges[len(g.edges)-1][0]], false, true)
		for k := range g.IncidentEdges[fromIndex] {
			if g.IncidentEdges[fromIndex][k] == action.param {
				g.IncidentEdges[fromIndex][k] = len(g.edges) - 1
				g.PortCandidates[fromIndex][k] = calcPortCandidates(g.IJs[g.edges[len(g.edges)-1][0]], g.IJs[g.edges[len(g.edges)-1][1]], false, true)
				break
			}
		}
		for k := range g.IncidentEdges[toIndex] {
			if g.IncidentEdges[toIndex][k] == action.param {
				g.IncidentEdges[toIndex][k] = action.param
				g.PortCandidates[toIndex][k] = calcPortCandidates(g.IJs[g.edges[action.param][1]], g.IJs[g.edges[action.param][0]], false, true)
				break
			}
		}
		if valid {
			valid = g.IsValidPortCandidate(fromIndex)
		}
		if valid {
			valid = g.IsValidPortCandidate(toIndex)
		}
		if valid {
			valid = g.IsValidPortCandidate(len(g.IJs) - 1)
		}
		if valid {
			valid = g.CrossCheck(len(g.edges) - 1)
		}
		if valid {
			valid = g.CrossCheck(action.param)
		}

		// procedureの更新
		g.procedures = make([][2]int, 0, len(procedures)+2)
		for i := range procedures {
			if procedures[i][0] == 1 && procedures[i][1] == action.param {
				// 前半部分を挿入
				g.procedures = append(g.procedures, [2]int{1, len(g.edges) - 1})
			}
			g.procedures = append(g.procedures, procedures[i])
		}
		g.counts = append(g.counts, 2)
	} else if action.actionType == 0 {

		if action.param != -1 {
			g.edges = append(g.edges, [2]int{action.param, len(g.IJs) - 1})
			g.addField(g.IJs[action.param], action.IJ, 1, gameConst)
			for k := range g.IncidentEdges[action.param] {
				if g.IncidentEdges[action.param][k] == -1 {
					g.IncidentEdges[action.param][k] = len(g.edges) - 1
					g.PortCandidates[action.param][k] = calcPortCandidates(g.IJs[g.edges[len(g.edges)-1][0]], g.IJs[g.edges[len(g.edges)-1][1]], false, true)
					break
				}
			}
			if valid {
				valid = g.IsValidPortCandidate(action.param)
			}
			g.IncidentEdges[len(g.IJs)-1][0] = len(g.edges) - 1
			g.PortCandidates[len(g.IJs)-1][0] = calcPortCandidates(g.IJs[g.edges[len(g.edges)-1][1]], g.IJs[g.edges[len(g.edges)-1][0]], false, true)
			if valid {
				valid = g.IsValidPortCandidate(len(g.IJs) - 1)
			}

			g.procedures = append(g.procedures, [2]int{1, len(g.edges) - 1})
			g.counts[action.param]++
			g.counts = append(g.counts, 1)
		} else {
			g.counts = append(g.counts, 0)
		}
	}
	if !valid {
		return valid
	}
	// for k := range gameConst.IncludedPoints[action.IJ] {
	// 	for _, peopleIndex := range gameConst.IncludedPoints[action.IJ][k] {
	// 		if g.numCovered[peopleIndex][k] == 0 {
	// 			if g.numCovered[peopleIndex][1-k] > 0 {
	// 				g.income += gameConst.Fares[peopleIndex]
	// 			}
	// 			g.potential += gameConst.Fares[peopleIndex]
	// 		}
	// 		g.numCovered[peopleIndex][k]++
	// 	}
	// }
	g.procedures = append(g.procedures, [2]int{0, len(g.IJs) - 1})

	prevPos := -1
	// if len(g.procedures) > prevLenProc {
	// 	if g.procedures[len(g.procedures)-3][0] == 1 {
	// 		edge := g.edges[g.procedures[len(g.procedures)-3][1]]
	// 		prevPos = edge[1]
	// 	}
	// }
	startIndex := len(g.procedures) - 2
	if action.actionType == 1 {
		startIndex++
	}
	if broken {
		startIndex = 0
		g.money = gameConst.InitialMoney
		g.potential = 0
		g.income = 0
		g.turn = 0
	}
	for i := startIndex; i < len(g.procedures); i++ {
		if i < 0 {
			continue
		}
		incomeMinus := 0
		if g.income > 700 {
			incomeMinus = 100
		}
		neededmoney := 0
		neededturn := 0
		neededRail := 0
		if g.procedures[i][0] == 1 {
			edge := g.edges[g.procedures[i][1]]
			p0 := g.IJs[edge[0]]
			p1 := g.IJs[edge[1]]
			distance := gameConst.distances[p0][p1]
			neededturn += distance
			neededRail += distance - 1
			neededmoney += 100 * distance
			neededmoney -= incomeMinus * distance
			prevPos = edge[1]
		} else {
			neededmoney += 4900
			if prevPos != g.procedures[i][1] {
				neededmoney += 100
				neededturn += 1
				neededRail += 1
			}
		}
		neededturn2 := 0
		if neededmoney <= g.money {
			neededturn2 = 0
		} else {
			if g.income-incomeMinus == 0 {
				neededturn2 = MAX_TURN
			} else {
				neededturn2 = (neededmoney-g.money-1)/(g.income-incomeMinus) + 1
			}
		}
		if g.procedures[i][0] == 1 {
			// edge
			if neededturn2 >= neededturn {
				g.money += (neededturn2) * (g.income - incomeMinus)
				g.turn += (neededturn2)
			} else {
				if neededturn-neededturn2 <= (g.railStock) {
					g.money += (0) * (g.income - incomeMinus)
					g.turn += (0)
					g.railStock -= neededturn - neededturn2
				} else {
					g.money += (neededturn - neededturn2 - g.railStock) * (g.income - incomeMinus)
					g.turn += (neededturn - neededturn2 - g.railStock)
					g.railStock = 0

				}
			}
		} else {
			// vertex
			if neededturn2 >= 1 {
				g.money += (neededturn2) * (g.income - incomeMinus)
				if incomeMinus > 0 {
					g.railStock += neededturn2 - neededturn
				}
				g.turn += (neededturn2)
			} else {
				g.money += (neededturn) * (g.income - incomeMinus)
				// if incomeMinus > 0 {
				// 	g.railStock += neededturn2 - neededturn
				// 	g.railStock = maxInt(0, g.railStock)
				// }
				g.turn += (neededturn)
			}
		}
		g.money -= neededmoney

		if g.procedures[i][0] == 0 {
			ij := g.IJs[g.procedures[i][1]]
			for k := range gameConst.IncludedPoints[ij] {
				for _, peopleIndex := range gameConst.IncludedPoints[ij][k] {
					if g.numCovered[peopleIndex][k] == 0 {
						if g.numCovered[peopleIndex][1-k] > 0 {
							g.income += gameConst.Fares[peopleIndex]
						}
						g.potential += gameConst.Fares[peopleIndex]

						// 近隣のpotentialを減少させる
						pos := gameConst.Poss[peopleIndex][k]
						for _, nij := range gameConst.nearIJs[pos] {
							if nij == -1 {
								continue
							}
							g.potentials[nij] -= gameConst.Fares[peopleIndex]
						}

					}
					g.numCovered[peopleIndex][k]++
				}
			}
		}
	}

	// g.potential += gameConst.potential[action.IJ]
	return valid
}

func (g *BeamSearchState) IsValidPortCandidate(vertexIndex int) bool {
	ok := false
	for permutateIndex := range PERMUTE {
		fit := true
		for k := range g.PortCandidates[vertexIndex] {
			if g.PortCandidates[vertexIndex][k]&(1<<PERMUTE[permutateIndex][k]) == 0 {
				fit = false
				break
			}
		}
		if fit {
			ok = true
			break
		}
	}

	return ok
}

func (g *BeamSearchState) IsValid(action *BeamSearchAction, gameConst *GameConst) bool {
	if action.actionType == 1 {
		// from := g.IJs[g.edges[action.param][0]]
		// to := action.IJ
		// ax := from / BOARD_WIDTH
		// ay := from % BOARD_WIDTH
		// bx := to / BOARD_WIDTH
		// by := to % BOARD_WIDTH
		// if ax == bx || ay == by {
		// 	return false
		// }
		// from = g.IJs[g.edges[action.param][1]]
		// to = action.IJ
		// ax = from / BOARD_WIDTH
		// ay = from % BOARD_WIDTH
		// bx = to / BOARD_WIDTH
		// by = to % BOARD_WIDTH
		// if ax == bx || ay == by {
		// 	return false
		// }
	} else if action.actionType == 0 {
		from := g.IJs[action.param]
		to := action.IJ
		valid := g.GeneralCrossCheck(-1, from, to)
		if !valid {
			return valid
		}
	}
	return true
}

// ビームサーチでの一行動を表す構造体
type BeamSearchAction struct {
	IJ         int
	actionType int // 0: append, 1: insert, 2:pair
	param      int // insert-> index of the edge, append -> index of the vertex, pair -> ij0,ij1
}

func NewBeamSearchAction() *BeamSearchAction {
	action := BeamSearchAction{}
	return &action
}

// ビームサーチを扱う構造体
type BeamSearch struct {
	defaultWidth       int                  // ビームサーチの幅
	depth              int                  // ビームサーチの深さ(終了までにかかりそうなAction数)
	nowDepth           int                  // 現在該当する深さ 計算が完了している時に使用される
	calculated         bool                 // 計算完了しているか
	effectiveStates    [][]*BeamSearchState // [depth][width]*BeamSearchState　有力候補を管理する変数
	numEffectiveStates []int                // 各深さでの有力候補数
	bestState          *BeamSearchState
	bestValue          float64

	mode int

	boarder float64
}

func NewBeamSearch() *BeamSearch {
	return &BeamSearch{}
}

func (bs *BeamSearch) Init(width int, depth int, mode int) {
	bs.defaultWidth = width
	bs.depth = depth
	bs.nowDepth = 0
	bs.mode = mode
	bs.calculated = false
	bs.effectiveStates = make([][]*BeamSearchState, depth+1)
	// for i := range bs.effectiveStates {
	// 	// 最初に列を用意しているが幅を動的に変えたい場合は書き換えを検討
	// 	bs.effectiveStates[i] = make([]*BeamSearchState, width)
	// }
	bs.numEffectiveStates = make([]int, depth+1)

	bs.bestValue = -INF
	bs.boarder = -INF
}

func (bs *BeamSearch) EvaluatefromIncome(income int, money int, potential int, turn int, gameConst *GameConst) float64 {
	value := 0.0
	/*if income == 0 {
		value = -5000 - float64(turn)
	}
	if income > 1 {
		area := 0.0
		plus := 0.0
		estimateKeyTurn := turn
		income += minInt(income*5, potential) / 10
		if income < 500 {
			fact := 70 / math.Log(2)
			estimateKeyTurn = turn + int((math.Log2(500/float64(income))-float64(money*50)/6000/float64(income))*70)
			if estimateKeyTurn >= MAX_TURN {
				area = (float64(income)*math.Exp2((MAX_TURN-float64(turn))/70) - float64(income)) * fact
			} else {
				area = (500 - float64(income)) * fact
			}
		}
		if estimateKeyTurn < MAX_TURN {
			additionalTurn := MAX_TURN - estimateKeyTurn
			if income < 500 {
				plus = float64(additionalTurn*additionalTurn*10) + 500*float64(additionalTurn)
			} else {
				plus = float64(additionalTurn*additionalTurn*10) + float64(income)*float64(additionalTurn)
			}
		}
		value = float64(money) + area + plus
	}*/
	if income == 0 {
		value = -5000 + float64(potential)
	} else {
		value = float64((income)*(MAX_TURN+20-turn)) + float64(money)*1
		if bs.mode == 0 {
			if turn < MAX_TURN-100 {
				if income > 2000 {
					value += float64(minInt(income*5, (potential-2*income))) * float64(MAX_TURN-turn) * 0.3
				} else if income > 1000 {
					value += float64(minInt(income*5, (potential-2*income))) * float64(MAX_TURN-turn) * 0.2
				} else {
					value += float64(minInt(income*5, (potential-2*income))) * float64(MAX_TURN-turn) * 0.1

				}
			}
		} else {

			if turn < MAX_TURN-100 {
				if income > 5000 {
					value += float64(minInt(income*5, (potential-2*income))) * float64(MAX_TURN-turn) * 0.3
				} else {
					value += float64(minInt(income*5, (potential-2*income))) * float64(MAX_TURN-turn) * 0.3

				}
			}
		}
	}
	return value
}

func (bs *BeamSearch) EvaluatetrueScore(gameState *BeamSearchState, gameConst *GameConst, comment bool) int {
	value := gameState.income*(MAX_TURN+1-gameState.turn) + gameState.money
	return value
}
func (bs *BeamSearch) Evaluate(gameState *BeamSearchState, gameConst *GameConst, comment bool) float64 {
	value := bs.EvaluatefromIncome(gameState.income, gameState.money, gameState.potential, gameState.turn, gameConst)
	// money := gameConst.InitialMoney
	// income := 0
	// turn := 0
	// value := 0.0

	// if gameState.income == 0 {
	// 	value = -5000 + float64(gameState.potential)
	// }
	// if gameState.income == 0 {
	// 	value = -5000 - float64(gameState.turn)
	// }
	// if income > 1 {
	// 	area := 0.0
	// 	plus := 0.0
	// 	estimateKeyTurn := gameState.turn
	// 	if income < 1000 {
	// 		fact := 70 / math.Log(2)
	// 		estimateKeyTurn = gameState.turn + int(math.Log2(1000/float64(income))*70)
	// 		if estimateKeyTurn >= MAX_TURN {
	// 			area = (float64(income)*math.Exp2((MAX_TURN-float64(gameState.turn))/70) - float64(income)) * fact
	// 		} else {
	// 			area = (1000 - float64(income)) * fact
	// 		}
	// 	}
	// 	if estimateKeyTurn < MAX_TURN {
	// 		additionalTurn := MAX_TURN - estimateKeyTurn
	// 		if income < 1000 {
	// 			plus = float64(additionalTurn*additionalTurn*10) + 1000*float64(additionalTurn)
	// 		} else {
	// 			plus = float64(additionalTurn*additionalTurn*10) + float64(income)*float64(additionalTurn)
	// 		}
	// 	}
	// 	value = float64(money) + area + plus
	// }
	// value = -float64(gameState.turn) - float64(5000-gameState.money)/float64(gameState.income)
	// value = float64(gameState.income)
	// value = float64(gameState.potential)
	// money = gameState.money
	// turn = gameState.turn
	//value = float64(gameState.income*(MAX_TURN+1-gameState.turn)) + float64(gameState.money)
	//
	//if gameState.turn < MAX_TURN-100 {
	//
	//	value += float64(minInt(gameState.income*5, (gameState.potential-2*gameState.income))) * float64(MAX_TURN-gameState.turn) * 0.3
	//}
	return value
}

func calcdistance(p0 int, p1 int) int {
	i0 := p0 / BOARD_WIDTH
	j0 := p0 % BOARD_WIDTH
	i1 := p1 / BOARD_WIDTH
	j1 := p1 % BOARD_WIDTH
	return absInt(i0-i1) + absInt(j0-j1)
}

func (bs *BeamSearch) EvaluateDiff(gameState *BeamSearchState, action *BeamSearchAction, gameConst *GameConst) float64 {
	return 0.0
}

func makeDfromStation(stations []int, gameConst *GameConst) []int {
	distances := make([]int, BOARD_WIDTH*BOARD_WIDTH)
	visited := make([]bool, BOARD_WIDTH*BOARD_WIDTH)

	for i := range distances {
		distances[i] = INF
	}
	ijqueues := make([]int, BOARD_WIDTH*BOARD_WIDTH)
	popIndex := 0
	pushIndex := 0
	for i := range stations {
		ij := stations[i]

		visited[ij] = true
		distances[ij] = 0
		ijqueues[pushIndex] = ij
		pushIndex++
	}

	for popIndex < pushIndex {
		ij := ijqueues[popIndex]
		popIndex++
		for _, nij := range gameConst.nIJs[ij] {
			if nij == -1 {
				continue
			}
			if visited[nij] {
				continue
			}
			visited[nij] = true
			distances[nij] = distances[ij] + 1
			ijqueues[pushIndex] = nij
			pushIndex++
		}
	}
	return distances
}

func (bs *BeamSearch) Run(gameState *BeamSearchState, gameConst *GameConst, candidates [BOARD_WIDTH * BOARD_WIDTH]int) *BeamSearchState {
	// 既に計算済みならいきなり復元パートに行ってよい
	if !bs.calculated {
		// 計算
		bsQueues := make([]*BeamSearchQueue, bs.depth+1)
		for i := range bsQueues {
			if i == 1 {
				bsQueues[i] = NewBeamSearchQueue(1)
			} else if i <= (gameConst.InitialMoney-10000)/100+1 {
				// bsQueues[i] = NewBeamSearchQueue(100)
				bsQueues[i] = NewBeamSearchQueue(bs.defaultWidth)
			} else {
				bsQueues[i] = NewBeamSearchQueue(bs.defaultWidth)
			}
			bsQueues[i].Init()
		}
		possibleAction := make([]int, 0, len(gameConst.candidates))
		order := make([]int, 0, len(gameConst.candidates))
		priority := make([]float64, 0, len(gameConst.candidates))
		incomes := make([]int, 0, len(gameConst.candidates))
		potentials := make([]int, 0, len(gameConst.candidates))
		for d := 0; d < bs.depth+1; d++ {
			// Queueの上位width個分だけ取り出す
			if d > 0 {
				bsQueues[d].Sort()
				if bs.numEffectiveStates[d] == 0 {
					index := 0
					bs.effectiveStates[d] = make([]*BeamSearchState, 0, bs.defaultWidth)
					lastValue := -float64(INF)

					for bsQueues[d].Len() > 0 {
						item := bsQueues[d].Pop()
						if DEBUG && index < 10 {
							fmt.Fprintln(os.Stderr, d, item.beamValue, item.totalValue, bs.boarder)

						}
						lastValue = item.beamValue
						prevState := bs.effectiveStates[item.prevDepth][item.prevIndex].Copy(gameConst)
						prevState.ApplyAction(item.prevAction, gameConst)
						/*if d == 22 && index == 0 {
							for i := range prevState.procedures {
								if prevState.procedures[i][0] == 0 {
									fmt.Fprintln(os.Stderr, "Vertex", prevState.IJs[prevState.procedures[i][1]]/BOARD_WIDTH, prevState.IJs[prevState.procedures[i][1]]%BOARD_WIDTH)
								} else {
									edgeindex := prevState.procedures[i][1]
									fmt.Fprintln(os.Stderr, "Edge", prevState.IJs[prevState.edges[edgeindex][0]]/BOARD_WIDTH, prevState.IJs[prevState.edges[edgeindex][0]]%BOARD_WIDTH, "->", prevState.IJs[prevState.edges[edgeindex][1]]/BOARD_WIDTH, prevState.IJs[prevState.edges[edgeindex][1]]%BOARD_WIDTH)

								}
							}
							fmt.Fprintln(os.Stderr, item.beamValue)
						}*/
						bs.effectiveStates[d] = append(bs.effectiveStates[d], prevState)
						index++
					}
					bs.boarder = math.Max(bs.boarder, lastValue)
					// EffectiveStatesにいれた候補数を格納
					bs.numEffectiveStates[d] = index
				}
				bsQueues[d].Init()

				//if bs.numEffectiveStates[d] == 0 {
				//	// あんまり好ましい状態ではないが、探索の完了している状態 (bs.depthの設定を見直したほうがいいかも?)
				//	fmt.Fprintln(os.Stderr, "WARN: bs.Depth changed", bs.depth, "->", d)
				//	bs.depth = d
				//}
				if d == bs.depth {
					bs.calculated = true
					break
				}
			} else {
				if bs.mode == 0 {
					bs.effectiveStates[d] = append(bs.effectiveStates[d], gameState.Copy(gameConst))
					bs.numEffectiveStates[d] = 1

					// ありうるペアを全列挙して突っ込む
					maxDistance := (gameState.money-10000)/100 + 1
					minDistance := 5
					// minDistance := maxDistance / 2
					for ij0 := 0; ij0 < BOARD_WIDTH*BOARD_WIDTH; ij0++ {
						for ij1 := ij0 + 1; ij1 < BOARD_WIDTH*BOARD_WIDTH; ij1++ {
							// if ij1 != 11*50+46 {
							// 	continue
							// }
							// if ij0 != 11*50+13 {
							// 	continue
							// }
							if candidates[ij0] != 1 {
								continue
							}
							if candidates[ij1] != 1 {
								continue
							}
							distance := gameConst.distances[ij0][ij1]
							if distance < minDistance {
								continue
							}
							if distance > maxDistance {
								continue
							}
							income := gameConst.effects[ij0][ij1]
							if income < 40 && income < (gameConst.InitialMoney-10000)/100-5 {
								continue
							}
							potential := gameConst.potential[ij0] + gameConst.potential[ij0]
							money := gameConst.InitialMoney - 100*(distance-1) - 10000

							action := NewBeamSearchAction()
							action.actionType = 2
							action.IJ = ij0*BOARD_WIDTH*BOARD_WIDTH + ij1
							action.param = -1
							item := &BeamItem{}
							item.prevIndex = 0
							item.prevDepth = 0
							item.prevAction = action
							item.beamValue = bs.EvaluatefromIncome(income, money, potential, distance, gameConst)
							item.totalValue = item.beamValue // - gameConst.Random.Float64()*float64(1*(MAX_TURN-distance))
							// item.totalValue = item.beamValue + gameConst.Random.Float64()*float64(10*(800-distance))
							bsQueues[distance].Push(item)
						}
					}
				} else {
					bs.bestState = gameState.Copy(gameConst)
					bs.effectiveStates[gameState.turn] = append(bs.effectiveStates[gameState.turn], gameState.Copy(gameConst))
					bs.numEffectiveStates[gameState.turn] = 1
				}
			}

			potentialBoarder := 5500 / (MAX_TURN - d)
			// potentialBoarder = 1

			for i := 0; i < bs.numEffectiveStates[d]; i++ {
				if d == 0 {
					continue
				}
				// 第i番目の有力候補を見る
				nowState := bs.effectiveStates[d][i]

				isStation := [BOARD_WIDTH * BOARD_WIDTH]bool{}
				for i := range nowState.IJs {
					isStation[nowState.IJs[i]] = true
				}

				dFromStation := makeDfromStation(nowState.IJs, gameConst)
				possibleAction = possibleAction[:0]
				order = order[:0]
				incomes = incomes[:0]
				potentials = potentials[:0]
				priority = priority[:0]
				// fmt.Fprintln(os.Stderr, nowState.potentials)
				for i := range gameConst.candidates {
					if nowState.potentials[gameConst.candidates[i]] < potentialBoarder {
						continue
					}
					possibleAction = append(possibleAction, gameConst.candidates[i])
					order = append(order, len(order))
					incomes = append(incomes, 0)
					potentials = append(potentials, 0)
					priority = append(priority, 0)
				}

				cNowState := NewBeamSearchState(gameConst)

				// for i := range possibleAction {
				// 	possibleAction[i] = gameConst.candidates[i]
				// }
				for i := range order {
					order[i] = i
				}
				for index, ij := range possibleAction {
					for k := range gameConst.IncludedPoints[ij] {
						for _, peopleIndex := range gameConst.IncludedPoints[ij][k] {
							if nowState.numCovered[peopleIndex][k] == 0 {
								if nowState.numCovered[peopleIndex][1-k] > 0 {
									incomes[index] += gameConst.Fares[peopleIndex]
								}
								// if index == 186 {
								// 	fmt.Fprintln(os.Stderr, gameConst.IncludedPoints[ij][k])
								// 	fmt.Fprintln(os.Stderr, potentials[index], float64(gameConst.Fares[peopleIndex]))
								// }
								potentials[index] += gameConst.Fares[peopleIndex]
							}
						}
					}
					// if potentials[index] != nowState.potentials[ij] {
					// 	fmt.Fprintln(os.Stderr, potentials[index], nowState.potentials[ij])
					// }
				}
				// fmt.Fprintln(os.Stderr, MAX_TURN-d, potentialBoarder, potentials, nowState.potentials[possibleAction[0]])

				for index := range possibleAction {
					priority[index] = float64(incomes[index]) + 0.3*float64(potentials[index]-incomes[index])
				}

				for index, ij := range possibleAction {
					mindistance := dFromStation[ij]
					ok := true
					if mindistance == 2 {
						ok = false
						k := 0
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
						k = 4
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
						k = 8
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
						k = 12
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
					}

					if mindistance <= 0 || !ok {
						priority[index] = 0
					}
					if candidates[ij] == 0 {
						priority[index] *= 0
					}
					if nowState.field[ij] == 0 {
						priority[index] *= 0
					}
				}

				sort.Slice(order, func(i, j int) bool { return priority[order[i]] > priority[order[j]] })
				// fmt.Fprintln(os.Stderr, priority)

				// 有効手についてloop

				// insert
				for _, actionIndex := range order {
					if priority[actionIndex] == 0 {
						break
					}
					ij := possibleAction[actionIndex]
					// if ij == 42*BOARD_WIDTH+16 {
					// 	fmt.Fprintln(os.Stderr, priority[actionIndex], 42, 16)
					// }

					if potentials[actionIndex] < potentialBoarder {
						continue
					}
					if incomes[actionIndex] < 1 && len(nowState.IJs) > 0 && nowState.potential-2*nowState.income > 300 {
						continue
					}

					nMoney, nTurn, nRailStock := calcNextState(nowState.money, nowState.income, nowState.turn, nowState.railStock, 1, true)
					if nTurn > MAX_TURN {
						continue
					} else {
						_ = nRailStock
						estimatedValue := bs.EvaluatefromIncome(nowState.income+incomes[actionIndex], nMoney, nowState.potential+potentials[actionIndex], nTurn, gameConst)
						ok := true
						for t := nowState.turn + 1; t <= nTurn; t++ {
							if !bsQueues[t].Promising(estimatedValue) {
								ok = false
								break
							}
						}
						if !ok {
							continue
						}
					}

					action, beamValue, ok := bs.evalInsert(ij, nowState, gameConst, cNowState)
					// if ij == 42*BOARD_WIDTH+15 {
					// 	for i := range nowState.procedures {
					// 		if nowState.procedures[i][0] == 0 {
					// 			fmt.Fprintln(os.Stderr, "Vertex", nowState.IJs[nowState.procedures[i][1]]/BOARD_WIDTH, nowState.IJs[nowState.procedures[i][1]]%BOARD_WIDTH)
					// 		} else {
					// 			edgeindex := nowState.procedures[i][1]
					// 			fmt.Fprintln(os.Stderr, "Edge", nowState.IJs[nowState.edges[edgeindex][0]]/BOARD_WIDTH, nowState.IJs[nowState.edges[edgeindex][0]]%BOARD_WIDTH, "->", nowState.IJs[nowState.edges[edgeindex][1]]/BOARD_WIDTH, nowState.IJs[nowState.edges[edgeindex][1]]%BOARD_WIDTH)
					//
					// 		}
					// 	}
					// 	fmt.Fprintln(os.Stderr, ok, priority[actionIndex], 42, 15)
					// 	fmt.Fprintln(os.Stderr, ok, priority[actionIndex], beamValue, 42, 15)
					// }
					// if estimatedValue != beamValue && beamValue != -INF {
					// 	fmt.Fprintln(os.Stderr, estimatedValue, beamValue)
					// 	fmt.Fprintln(os.Stderr, "turn", nTurn, cNowState.turn)
					// 	fmt.Fprintln(os.Stderr, "money", nMoney, cNowState.money)
					// 	fmt.Fprintln(os.Stderr, "railstock", nRailStock, cNowState.railStock)
					// }
					if !ok {
						continue
					}
					item := &BeamItem{}

					item.beamValue = beamValue
					item.prevDepth = d
					item.prevIndex = i
					item.prevAction = action

					nd := cNowState.turn
					item.totalValue = item.beamValue // - gameConst.Random.Float64()*float64(1*(MAX_TURN-d))

					if nd > bs.depth {
						continue
					} else {
						if item.beamValue != -INF {
							value := float64(bs.EvaluatetrueScore(cNowState, gameConst, false))
							// value := float64(cNowState.money) + float64(MAX_TURN+1-cNowState.turn)*float64(cNowState.income)
							if bs.bestValue < value {
								bs.bestValue = value
								bs.bestState = cNowState.Copy(gameConst)
							}
						}
						// if cNowState.income < 500 {
						bsQueues[nd].Push(item)
						// }
					}
					break
				}
				// append
				for i := range priority {
					priority[i] = float64(incomes[i]) + 0.3*float64(potentials[i]-incomes[i])
				}
				for index, ij := range possibleAction {
					mindistance := dFromStation[ij]
					ok := true
					if mindistance == 2 {
						ok = false
						k := 0
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
						k = 4
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
						k = 8
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
						k = 12
						if gameConst.nearIJs[ij][k] != -1 && isStation[gameConst.nearIJs[ij][k]] {
							ok = true
						}
					}
					if mindistance <= 0 || !ok {
						priority[index] = 0
					} else {
						priority[index] /= float64(mindistance)
					}
					if candidates[ij] == 0 {
						priority[index] *= 0
					}
				}

				sort.Slice(order, func(i, j int) bool { return priority[order[i]] > priority[order[j]] })

				for _, actionIndex := range order {
					// if actionIndex == 186 {
					// 	fmt.Fprintln(os.Stderr, priority[actionIndex], potentials[actionIndex], incomes[actionIndex], possibleAction[actionIndex])
					// }

					ij := possibleAction[actionIndex]
					if priority[actionIndex] == 0 {
						break
					}

					if potentials[actionIndex] < potentialBoarder {
						continue
					}
					if incomes[actionIndex] < 1 && len(nowState.IJs) > 0 && nowState.potential-2*nowState.income > 300 {
						continue
					}
					mindistance := dFromStation[ij]
					// if ij == 18*50+34 {
					// 	fmt.Fprintln(os.Stderr, priority[actionIndex], potentials[actionIndex], incomes[actionIndex], possibleAction[actionIndex])
					// }
					nMoney, nTurn, nRailStock := calcNextState(nowState.money, nowState.income, nowState.turn, nowState.railStock, mindistance, false)
					if nTurn > MAX_TURN {
						continue
					} else {
						_ = nRailStock
						estimatedValue := bs.EvaluatefromIncome(nowState.income+incomes[actionIndex], nMoney, nowState.potential+potentials[actionIndex], nTurn, gameConst)
						ok := true
						for t := nowState.turn + 1; t <= nTurn; t++ {
							if !bsQueues[t].Promising(estimatedValue) {
								ok = false
								break
							}
						}
						if !ok {
							continue
						}
					}

					if true {
						action := NewBeamSearchAction()
						action.actionType = 1
						bestDiff := INF
						for insertIndex := 0; insertIndex < len(nowState.edges); insertIndex++ {
							from := nowState.edges[insertIndex][0]
							to := nowState.edges[insertIndex][1]
							diff := gameConst.distances[nowState.IJs[from]][ij] + gameConst.distances[ij][nowState.IJs[to]] - gameConst.distances[nowState.IJs[from]][nowState.IJs[to]]
							action.param = insertIndex

							if !nowState.IsValid(action, gameConst) {
								continue
							}

							if diff < bestDiff {

								bestDiff = diff
							}
						}
						if bestDiff == 0 {
							continue
						}
					}

					action, beamValue, ok := bs.evalAppend(ij, nowState, gameConst, cNowState)
					if !ok {
						continue
					}

					item := &BeamItem{}

					item.prevDepth = d
					item.prevIndex = i
					item.prevAction = action

					// 状態を評価
					item.beamValue = beamValue
					nd := cNowState.turn
					item.totalValue = item.beamValue // - gameConst.Random.Float64()*float64(1*(MAX_TURN-d))

					if nd > bs.depth {
						continue
					} else {
						if item.beamValue != -INF {
							value := float64(bs.EvaluatetrueScore(cNowState, gameConst, false))
							// value := float64(cNowState.money) + float64(MAX_TURN+1-cNowState.turn)*float64(cNowState.income)
							if bs.bestValue < value {
								bs.bestValue = value
								bs.bestState = cNowState.Copy(gameConst)
							}
						}
						bsQueues[nd].Push(item)
					}

				}
			}
		}
	}
	// 復元(はじめにどの手を採用すればいいのか)
	bs.Evaluate(bs.bestState, gameConst, true)
	return bs.bestState
}

func calcNextState(money int, income int, turn int, railStock int, distance int, insert bool) (nMoney int, nTurn int, nRailStock int) {
	incomeMinus := 0
	neededturn := 0 // 距離、手数由来
	neededmoney := 100*distance + 4900
	if income > 700 {
		incomeMinus = 100
	}
	if insert {
		neededturn = 1
	} else {
		neededturn = distance
	}
	neededturn2 := 0 // money由来
	if neededmoney <= money {
		neededturn2 = 1
	} else {
		if income == 0 {
			neededturn2 = MAX_TURN
		} else {
			neededturn2 = (neededmoney-money-1)/(income-incomeMinus) + 1
		}
	}
	turnplus := 0
	if neededturn-railStock < neededturn2 {
		turnplus = neededturn2
		if incomeMinus > 0 {
			nRailStock = railStock + neededturn2 - neededturn
		}

	} else {
		turnplus = neededturn - railStock
		nRailStock = 0
	}
	nTurn = turn + turnplus
	nMoney = money + turnplus*(income-incomeMinus) - neededmoney
	return
}

func (bs *BeamSearch) evalInsert(ij int, nowState *BeamSearchState, gameConst *GameConst, cNowState *BeamSearchState) (*BeamSearchAction, float64, bool) {
	bestInsertIndex := 0
	bestDiff := INF
	tieBreak := INF

	action := NewBeamSearchAction()

	action.actionType = 1
	action.IJ = ij

	for insertIndex := 0; insertIndex < len(nowState.edges); insertIndex++ {
		from := nowState.edges[insertIndex][0]
		to := nowState.edges[insertIndex][1]
		diff := gameConst.distances[nowState.IJs[from]][ij] + gameConst.distances[ij][nowState.IJs[to]] - gameConst.distances[nowState.IJs[from]][nowState.IJs[to]]
		action.param = insertIndex

		if !nowState.IsValid(action, gameConst) {
			continue
		}

		fromi := from / BOARD_WIDTH
		fromj := from % BOARD_WIDTH
		toi := to / BOARD_WIDTH
		toj := to % BOARD_WIDTH
		iji := ij / BOARD_WIDTH
		ijj := ij % BOARD_WIDTH
		tb := (absInt(fromi-toi)+1)*(absInt(fromj-toj)+1) - (absInt(fromi-iji)+1)*(absInt(fromj-ijj)+1) - (absInt(iji-toi)+1)*(absInt(ijj-toj)+1)

		if !nowState.IsValid(action, gameConst) {
			continue
		}
		if diff < bestDiff {
			bestDiff = diff
			bestInsertIndex = insertIndex
			tieBreak = tb
		}
		if diff == bestDiff && tb < tieBreak {
			bestDiff = diff
			bestInsertIndex = insertIndex
			tieBreak = tb
		}
	}
	if bestDiff > 0 {
		if bestDiff == INF || true {
			return NewBeamSearchAction(), -INF, false
		}
		from := nowState.edges[bestInsertIndex][0]
		to := nowState.edges[bestInsertIndex][1]
		d1 := gameConst.distances[nowState.IJs[from]][ij]
		d2 := gameConst.distances[ij][nowState.IJs[to]]
		if d1-bestDiff/2 <= 2 || d2-bestDiff/2 <= 2 || bestDiff/2 <= 2 {
			return NewBeamSearchAction(), -INF, false
		}
		cur := ij
		for i := 0; i < bestDiff/2; i++ {
			for _, nij := range gameConst.nIJs[cur] {
				if nij == -1 {
					continue
				}
				if gameConst.distances[nowState.IJs[from]][cur] > gameConst.distances[nowState.IJs[from]][nij] {
					if gameConst.distances[cur][nowState.IJs[to]] > gameConst.distances[nij][nowState.IJs[to]] {
						cur = nij
						break
					}
				}
			}
		}
		action.actionType = 2
		action.param = bestInsertIndex
		action.IJ = cur*BOARD_WIDTH*BOARD_WIDTH + ij

		// 行動を試す
		cNowState.Load(nowState)

		ok := cNowState.ApplyAction(action, gameConst)
		// if ij == 24*BOARD_WIDTH+38 {
		// 	if cur == 24*BOARD_WIDTH+32 {
		// 		fmt.Fprintln(os.Stderr, ok, gameConst.distances[nowState.IJs[from]][cur], gameConst.distances[nowState.IJs[to]][cur], gameConst.distances[nowState.IJs[from]][nowState.IJs[to]])
		// 	}
		// }
		if !ok {
			return NewBeamSearchAction(), -INF, false
		}
		return action, bs.Evaluate(cNowState, gameConst, false), true
	} else {
		action.param = bestInsertIndex

		// 行動を試す
		cNowState.Load(nowState)

		ok := cNowState.ApplyAction(action, gameConst)
		if !ok {
			return NewBeamSearchAction(), -INF, false
		}
		return action, bs.Evaluate(cNowState, gameConst, false), true
	}

}

func (bs *BeamSearch) evalAppend(ij int, nowState *BeamSearchState, gameConst *GameConst, cNowState *BeamSearchState) (*BeamSearchAction, float64, bool) {

	action := NewBeamSearchAction()

	action.actionType = 0
	action.IJ = ij

	nearest := -1
	nearestDistance := INF
	// 接続先を探す
	for i := range nowState.IJs {
		if nowState.counts[i] == 4 {
			continue
		}
		distance := gameConst.distances[nowState.IJs[i]][ij]
		diff := absInt(nowState.IJs[i] - ij)
		if diff == BOARD_WIDTH-1 {
			continue
		} else if diff == BOARD_WIDTH+1 {
			continue
		}
		if distance < nearestDistance {
			action.param = i
			if nowState.IsValid(action, gameConst) {
				nearest = i
				nearestDistance = distance
			}
		}
	}
	if nearest == -1 && len(nowState.IJs) > 0 {
		// if true {
		return NewBeamSearchAction(), -INF, false
	}
	action.param = nearest

	// 行動を試す
	cNowState.Load(nowState)

	ok := cNowState.ApplyAction(action, gameConst)
	if !ok {
		return NewBeamSearchAction(), -INF, false
	}
	return action, bs.Evaluate(cNowState, gameConst, false), true
}

// 連続して追加->連続してdeleteminの場合のみ使えるPriorityQueue
// 長さ（BeamSearchの幅）指定あり
type BeamSearchQueue struct {
	items     []*BeamItem
	popIndex  int
	pushIndex int
	width     int
	sorted    bool

	boarder float64
}

// 新しいQueueを生成する関数
func NewBeamSearchQueue(width int) *BeamSearchQueue {
	q := BeamSearchQueue{}
	q.items = make([]*BeamItem, 2*width)
	q.popIndex = 0
	q.pushIndex = 0
	q.boarder = -INF
	q.width = width
	return &q
}

// Queueを初期化する関数(幅の再設定は要書き換え)
func (q *BeamSearchQueue) Init() {
	q.popIndex = 0
	q.pushIndex = 0
	q.width = len(q.items) / 2
	q.boarder = -INF
	q.sorted = false
}

func (q *BeamSearchQueue) Shorten(newLength int) {
	q.Sort()
	if q.pushIndex >= newLength {
		q.pushIndex = newLength
	}
	q.width = newLength
	q.items = q.items[:newLength*2]
	q.Sort()
}

// Queueから最もbeamValue大きいものをとる操作
func (q *BeamSearchQueue) Pop() *BeamItem {
	ret := q.items[q.popIndex]
	q.popIndex++
	return ret
}

// StateをQueueに入れる操作
func (q *BeamSearchQueue) Push(item *BeamItem) bool {
	pushed := false
	if q.pushIndex >= len(q.items) {
		q.Sort()
	}
	if q.boarder > item.totalValue {
		// この段階で棄却
		return pushed
	}
	if item.beamValue > 250000 && q.width > 5 {
		q.Shorten(5)
	}
	if item.beamValue > 800000 && q.width > 3 {
		q.Shorten(3)
	}
	if item.beamValue > 1300000 && q.width > 2 {
		q.Shorten(2)
	}
	pushed = true
	q.sorted = false
	q.items[q.pushIndex] = item
	q.pushIndex++
	if q.pushIndex == len(q.items)/2 {
		q.Sort()
	}
	return pushed
}

// valueの大きい順に並べ替える
func (q *BeamSearchQueue) Sort() {
	sort.Slice(q.items[:q.pushIndex], func(i, j int) bool { return q.items[i].totalValue > q.items[j].totalValue })
	if q.pushIndex > q.width {
		// 上位width分以外は捨てる
		q.pushIndex = q.width
		q.boarder = q.items[q.width-1].totalValue
	}
	q.sorted = true
}

func (q *BeamSearchQueue) Promising(v float64) bool {
	return q.boarder <= v
}

func (q *BeamSearchQueue) Len() int {
	// 正しくはこれだが、安全性を無視するなら二つ目のほうが高速
	m := minInt(q.width, q.pushIndex)
	l := maxInt(m-q.popIndex, 0)
	// l := q.pushIndex - q.popIndex
	return l
}

type Vertex struct {
	id             int
	x, y           int // 復元された元座標
	boardX, boardY int // 盤面上の座標（この例では x,y と同一）
	incidentEdges  []IncidentEdge
	assignedPorts  map[int]int // edge id -> port (0: up,1: right,2: down,3: left)
	procIdx        int         // この頂点を配置した procedure の index（procedures内での順番）
}

type IncidentEdge struct {
	edgeID         int
	portCandidates int
	idealAngle     float64
}

type Edge struct {
	id             int
	source, target int // 頂点番号（IJs のインデックス）
	procIdx        int // このエッジを配置した procedure の index
}

type Cell struct {
	typ        string // "vertex" or "edge"
	procIdx    int    // procedure index (displayed number)
	isEndpoint bool
}

type Pos struct {
	row, col int
}

// ---------- RouteSolver 構造体 ----------
type RouteSolver struct {
	boardWidth, boardHeight int
	IJs                     []int    // 各頂点の座標が x*BOARD_WIDTH+y 形式
	edges                   [][2]int // 各辺の両端点の頂点番号（IJs のインデックス）
	procedures              [][2]int // 構築手順　[0]: 0→vertex, 1→edge, [1]: 対応する番号
	incidentEdges           [][4]int
	portCandidates          [][4]int

	isPort    [BOARD_WIDTH * BOARD_WIDTH]int
	vertices  []*Vertex          // 生成した頂点
	edgeList  []Edge             // 生成した辺
	vertexMap map[string]*Vertex // キー "x,y" → Vertex
}

func (rs *RouteSolver) Init(gameConst *GameConst) {
	for i := range rs.isPort {
		rs.isPort[i] = -1
	}
	for i := range rs.IJs {
		rs.isPort[rs.IJs[i]] = i
		for _, nij := range gameConst.nIJs[rs.IJs[i]] {
			if nij == -1 {
				continue
			}
			rs.isPort[nij] = i
		}
	}
}

// ---------- ヘルパー関数 ----------

func (rs *RouteSolver) radToDeg(rad float64) float64 {
	return rad * (180 / math.Pi)
}

func (rs *RouteSolver) getPortAngle(port int) float64 {
	return ANGLE[port]
}

func (rs *RouteSolver) angleDifference(a, b float64) float64 {
	diff := a - b
	for diff > 180 {
		diff -= 360
	}
	for diff < -180 {
		diff += 360
	}
	return math.Abs(diff)
}

// ---------- RouteSolver のメソッド ----------

// TransformCoordinates: IJs から各頂点を復元。
// IJs[i]の値から x = IJs[i] / boardWidth, y = IJs[i] % boardWidth とする。
// 盤面座標はそのまま x, y とする。
func (rs *RouteSolver) TransformCoordinates() {
	n := len(rs.IJs)
	rs.vertices = make([]*Vertex, n)
	for i := 0; i < n; i++ {
		val := rs.IJs[i]
		x := val / rs.boardWidth
		y := val % rs.boardWidth
		rs.vertices[i] = &Vertex{
			id:            i,
			x:             x,
			y:             y,
			boardX:        x,
			boardY:        y,
			incidentEdges: []IncidentEdge{},
			assignedPorts: make(map[int]int),
		}
	}
}

// AssignProcedureIndices: proceduresに従い、
// ・type==0 の手順では、該当する頂点の procIdx を設定。
// ・type==1 の手順では、該当する辺の procIdx を設定。
func (rs *RouteSolver) AssignProcedureIndices() {
	for procIdx, proc := range rs.procedures {
		if proc[0] == 0 {
			vertexIdx := proc[1]
			if vertexIdx < len(rs.vertices) {
				rs.vertices[vertexIdx].procIdx = procIdx
			}
		}
	}
	// 辺のprocIdxは BuildEdges で設定
}

// BuildEdges: rs.edges から Edge 型の配列を生成する。
// procedures の type==1 の順序で edge の procIdx を設定する。
func (rs *RouteSolver) BuildEdges() {
	rs.edgeList = make([]Edge, len(rs.edges))
	edgeProc := make(map[int]int)
	for procIdx, proc := range rs.procedures {
		if proc[0] == 1 {
			edgeIdx := proc[1]
			edgeProc[edgeIdx] = procIdx
		}
	}
	for i, e := range rs.edges {
		proc, ok := edgeProc[i]
		if !ok {
			proc = -1
		}
		rs.edgeList[i] = Edge{
			id:      i,
			source:  e[0],
			target:  e[1],
			procIdx: proc,
		}
	}
}

// BuildVertexMap: 各頂点を "x,y" キーで管理する。
func (rs *RouteSolver) BuildVertexMap() {
	rs.vertexMap = make(map[string]*Vertex)
	for _, v := range rs.vertices {
		key := fmt.Sprintf("%d,%d", v.x, v.y)
		rs.vertexMap[key] = v
	}
}

// ComputeIncidentEdges: 各edgeについて、両端点に理想角情報を追加する。
// 理想角は、相手頂点への角度（度）。
func (rs *RouteSolver) ComputeIncidentEdges() {
	for _, edge := range rs.edgeList {
		vS := rs.vertices[edge.source]
		vT := rs.vertices[edge.target]
		angleS := rs.radToDeg(math.Atan2(float64(vT.y-vS.y), float64(vT.x-vS.x)))
		angleT := rs.radToDeg(math.Atan2(float64(vS.y-vT.y), float64(vS.x-vT.x)))
		for k := 0; k < 4; k++ {
			if rs.incidentEdges[vS.id][k] == edge.id {
				vS.incidentEdges = append(vS.incidentEdges, IncidentEdge{edgeID: edge.id, idealAngle: angleS, portCandidates: rs.portCandidates[vS.id][k]})
			}
			if rs.incidentEdges[vT.id][k] == edge.id {
				vT.incidentEdges = append(vT.incidentEdges, IncidentEdge{edgeID: edge.id, idealAngle: angleT, portCandidates: rs.portCandidates[vT.id][k]})
			}

		}
	}
}

// AssignProvisionalPorts: 各頂点で incidentEdges に対して利用可能な4ポートから全順列探索し、
// 総誤差が最小となる割当を決定する。
func (rs *RouteSolver) AssignProvisionalPorts(occupancy [][]*Cell, gameConst *GameConst) {
	rs.ComputeIncidentEdges()
	for loop := 0; loop < 20; loop++ {
		success := true
		dangerPoints := make([]int, BOARD_WIDTH*BOARD_WIDTH)
		for i := range dangerPoints {
			dangerPoints[i] = -1
		}
		for i:=range rs.IJs{
			dangerPoints[rs.IJs[i]]=100000
		}
		vertices := make([]*Vertex, 0, len(rs.vertices))
		vertices = append(vertices, rs.vertices...)
		priority := make([]float64, len(rs.vertices))
		for index, v := range vertices {
			n := len(v.incidentEdges)
			if n == 0 {
				continue
			}
			// port候補の更新(確定edgeによって利用不可の可能性があるため)
			for d := 0; d < 4; d++ {
				ij := v.x*BOARD_WIDTH + v.y
				nij := gameConst.nIJs[ij][d]
				if nij == -1 {
					continue
				}
				nx := nij / BOARD_WIDTH
				ny := nij % BOARD_WIDTH
				if occupancy[nx][ny] != nil {
					if rs.procedures[occupancy[nx][ny].procIdx][0] == 1 {
						edgeIdx := rs.procedures[occupancy[nx][ny].procIdx][1]
						for i := range v.incidentEdges {
							if v.incidentEdges[i].edgeID != edgeIdx {
								v.incidentEdges[i].portCandidates &= ^(1 << d)
							}

						}
					}
				}
			}

			var calcPriority func(arr, m []int)
			calcPriority = func(arr, m []int) {
				if len(m) == n {
					totalError := 0
					for i, port := range m {
						candidates := v.incidentEdges[i].portCandidates

						if candidates&(1<<port) == 0 {
							totalError += 1
						}

					}
					if totalError == 0 {
						priority[index] -= 1
					}
				} else {
					for i, val := range arr {
						newArr := append([]int(nil), arr[:i]...)
						newArr = append(newArr, arr[i+1:]...)
						calcPriority(newArr, append(m, val))
					}
				}
			}
			available := []int{0, 1, 2, 3}
			calcPriority(available, []int{})
		}
		for i := range priority {
			if priority[i] == -1 {
				priority[i] += 3
			}
		}
		for i := range vertices {
			if len(vertices[i].incidentEdges) == 4 {
				priority[i] += 2
			}
		}
		for i := range priority {
			priority[i] += gameConst.Random.Float64() * 2
		}
		sort.Slice(vertices, func(i, j int) bool { return priority[vertices[i].id] > priority[vertices[j].id] })
		for _, v := range vertices {
			// fmt.Fprintln(os.Stderr, priority[v.id], v.procIdx)
			n := len(v.incidentEdges)
			if n == 0 {
				continue
			}
			available := []int{0, 1, 2, 3}
			bestAssignment := []int{}
			bestError := math.Inf(1)

			var permute func(arr, m []int)
			permute = func(arr, m []int) {
				if len(m) == n {
					totalError := 0.0
					for i, port := range m {
						candidates := v.incidentEdges[i].portCandidates
						if candidates&(1<<port) == 0 {
							totalError += 1000 * 1000
						}
						// もう一方とのかみ合わせがおかしかったら減点
						edge := v.incidentEdges[i].edgeID
						targetIdx := rs.edgeList[edge].target
						if v.id == targetIdx {
							targetIdx = rs.edgeList[edge].source
						}
						v2 := rs.vertices[targetIdx]
						ok := true

						port2, exist := v2.assignedPorts[edge]
						if exist {
							if port+port2 == 3 {
								if port == 0 || port2 == 0 {

									if absInt(rs.IJs[targetIdx]/BOARD_WIDTH-rs.IJs[v.id]/BOARD_WIDTH) <= 1 {
										if absInt(rs.IJs[targetIdx]%BOARD_WIDTH-rs.IJs[v.id]%BOARD_WIDTH) > 0 {
											ok = false
										}
									}
								}
								if port == 1 || port2 == 1 {

									if absInt(rs.IJs[targetIdx]%BOARD_WIDTH-rs.IJs[v.id]%BOARD_WIDTH) <= 1 {
										if absInt(rs.IJs[targetIdx]/BOARD_WIDTH-rs.IJs[v.id]/BOARD_WIDTH) > 0 {
											ok = false
										}
									}
								}
							}
						}
						if !ok {
							// fmt.Fprintln(os.Stderr, "!!!!!!!!!!!!!")
							totalError += 1000 * 1000
						}
						nij := gameConst.nIJs[rs.IJs[v.id]][port]
						if absInt(rs.IJs[targetIdx]%BOARD_WIDTH-rs.IJs[v.id]%BOARD_WIDTH) +absInt(rs.IJs[targetIdx]/BOARD_WIDTH-rs.IJs[v.id]/BOARD_WIDTH)>1{
							if nij == -1 || (dangerPoints[nij] >= 0 && dangerPoints[nij] != v.incidentEdges[i].edgeID) {
								// fmt.Fprintln(os.Stderr, "!!!!!!!!!!!!!")
								totalError += 1000 * 1000
							}
						}
						ideal := v.incidentEdges[i].idealAngle
						diff := rs.angleDifference(ideal, rs.getPortAngle(port))
						totalError += diff * diff

					}
					// if v.procIdx == 21 {
					// 	fmt.Fprintln(os.Stderr, m, totalError)
					// }
					if totalError < bestError {
						bestError = totalError
						bestAssignment = append([]int(nil), m...)
					}
				} else {
					for i, val := range arr {
						newArr := append([]int(nil), arr[:i]...)
						newArr = append(newArr, arr[i+1:]...)
						permute(newArr, append(m, val))
					}
				}
			}

			permute(available, []int{})
			if bestError > 1000*1000 {
				fmt.Fprintln(os.Stderr, bestError, v.procIdx, available, bestAssignment, len(v.incidentEdges) /*v.incidentEdges[0].portCandidates,v.incidentEdges[1].portCandidates*/)
				// for i:=range v.incidentEdges{
				// 	fmt.Fprintln(os.Stderr, bestError, v.procIdx, available, bestAssignment,v.incidentEdges[i].portCandidates)
				// }
				success = false
				break
			}
			v.assignedPorts = make(map[int]int)
			for i, inc := range v.incidentEdges {
				nij := gameConst.nIJs[rs.IJs[v.id]][bestAssignment[i]]
				if rs.IJs[rs.edgeList[inc.edgeID].source] != nij && rs.IJs[rs.edgeList[inc.edgeID].target] != nij {
					dangerPoints[nij] = inc.edgeID
				}
				v.assignedPorts[inc.edgeID] = bestAssignment[i]
			}
		}
		if success {
			break
		}
		fmt.Fprintln(os.Stderr, "assign failed!", gameConst.NumPeople)
		for i := range rs.vertices {
			// assignをやり直す
			rs.vertices[i].assignedPorts = make(map[int]int)
		}
	}
}

// GetGridBounds: 盤面全体 (0～boardHeight-1, 0～boardWidth-1)
func (rs *RouteSolver) GetGridBounds() (minRow, maxRow, minCol, maxCol int) {
	return 0, rs.boardHeight - 1, 0, rs.boardWidth - 1
}

// ConvertToGridCoord: 盤面座標はそのまま使う（(x,y) → (row, col) として扱う）
func (rs *RouteSolver) ConvertToGridCoord(x, y int) Pos {
	return Pos{row: x, col: y}
}

// CopyGrid: occupancy のディープコピー
func (rs *RouteSolver) CopyGrid(grid [][]*Cell) [][]*Cell {
	newGrid := make([][]*Cell, len(grid))
	for i := range grid {
		newGrid[i] = make([]*Cell, len(grid[i]))
		copy(newGrid[i], grid[i])
	}
	return newGrid
}

// ResolveConflictCluster: 衝突しているエッジ群（edge id のスライス）をまとめて再ルーティングする。
// 各エッジについて、再ルーティング候補が元の最短路長以内なら占有グリッドを更新する。
func (rs *RouteSolver) ResolveConflictCluster(conflictEdges []int, occupancy [][]*Cell, solutionRoutes map[int]RouteSolution, gameConst *GameConst) bool {
	occCopy := rs.CopyGrid(occupancy)
	for _, procIdx := range conflictEdges {
		edgeID := rs.procedures[procIdx][1]
		var edge *Edge
		for i := range rs.edgeList {
			if rs.edgeList[i].id == edgeID {
				edge = &rs.edgeList[i]
				break
			}
		}
		if edge == nil {
			continue
		}
		keyS := fmt.Sprintf("%d,%d", rs.vertices[edge.source].x, rs.vertices[edge.source].y)
		keyT := fmt.Sprintf("%d,%d", rs.vertices[edge.target].x, rs.vertices[edge.target].y)
		vS, ok1 := rs.vertexMap[keyS]
		vT, ok2 := rs.vertexMap[keyT]
		if !ok1 || !ok2 {
			continue
		}
		sourcePort := vS.assignedPorts[edge.id]
		targetPort := vT.assignedPorts[edge.id]
		var cs, ct Pos
		// ※ ここでは ConvertToGridCoord を使わず、board座標そのまま利用（既存コードに合わせる）
		switch sourcePort {
		case 0:
			cs = Pos{vS.boardX - 1, vS.boardY}
		case 1:
			cs = Pos{vS.boardX, vS.boardY - 1}
		case 2:
			cs = Pos{vS.boardX, vS.boardY + 1}
		case 3:
			cs = Pos{vS.boardX + 1, vS.boardY}
		}
		switch targetPort {
		case 0:
			ct = Pos{vT.boardX - 1, vT.boardY}
		case 1:
			ct = Pos{vT.boardX, vT.boardY - 1}
		case 2:
			ct = Pos{vT.boardX, vT.boardY + 1}
		case 3:
			ct = Pos{vT.boardX + 1, vT.boardY}
		}
		oldRoute := solutionRoutes[edge.id].route
		// fmt.Fprintln(os.Stderr, solutionRoutes[edge.id].route)
		for i := 0; i < len(oldRoute); i++ {
			if occCopy[oldRoute[i].row][oldRoute[i].col].procIdx == procIdx {
				occCopy[oldRoute[i].row][oldRoute[i].col] = nil
			}
		}
		newRoute := rs.BFSConstrained(cs, ct, Pos{vS.boardX, vS.boardY}, cs, Pos{vT.boardX, vT.boardY}, ct, occCopy, rs.boardHeight, rs.boardWidth, gameConst)
		if newRoute == nil {
			return false
		}
		if solutionRoutes[edge.id].routeLength > 0 && len(newRoute) > solutionRoutes[edge.id].routeLength {
			return false
		}
		for i, cell := range newRoute {
			occCopy[cell.row][cell.col] = &Cell{typ: "edge", procIdx: edge.procIdx, isEndpoint: (i == 0 || i == len(newRoute)-1)}
		}
		solutionRoutes[edge.id] = struct {
			route       []Pos
			routeLength int
			sourcePort  int
			targetPort  int
		}{route: newRoute, routeLength: len(newRoute), sourcePort: sourcePort, targetPort: targetPort}
	}
	for r := 0; r < rs.boardHeight; r++ {
		for c := 0; c < rs.boardWidth; c++ {
			occupancy[r][c] = occCopy[r][c]
		}
	}
	return true
}

// RouteEdgesSequentially: procedures の順序（type==1 の順序）に沿ってエッジを順次ルーティングする。
// ルーティング時、各エッジは仮決定された接続方向に従い、BFSConstrained を使って指定された接続セルからのみ出発するルートを求める。
func (rs *RouteSolver) RouteEdgesSequentially(edges []Edge, occupancy [][]*Cell, solutionRoutes map[int]RouteSolution, index int, gameConst *GameConst) bool {
	if index == len(edges) {
		return true
	}
	edge := edges[index]
	keyS := fmt.Sprintf("%d,%d", rs.vertices[edge.source].x, rs.vertices[edge.source].y)
	keyT := fmt.Sprintf("%d,%d", rs.vertices[edge.target].x, rs.vertices[edge.target].y)
	vS, ok1 := rs.vertexMap[keyS]
	vT, ok2 := rs.vertexMap[keyT]
	if !ok1 || !ok2 {
		return false
	}
	sourcePort := vS.assignedPorts[edge.id]
	targetPort := vT.assignedPorts[edge.id]
	var cs, ct Pos
	// ここ、既存コードに合わせ、盤面座標は (boardX, boardY) として扱う
	switch sourcePort {
	case 0:
		cs = Pos{vS.boardX - 1, vS.boardY}
	case 1:
		cs = Pos{vS.boardX, vS.boardY - 1}
	case 2:
		cs = Pos{vS.boardX, vS.boardY + 1}
	case 3:
		cs = Pos{vS.boardX + 1, vS.boardY}
	}
	switch targetPort {
	case 0:
		ct = Pos{vT.boardX - 1, vT.boardY}
	case 1:
		ct = Pos{vT.boardX, vT.boardY - 1}
	case 2:
		ct = Pos{vT.boardX, vT.boardY + 1}
	case 3:
		ct = Pos{vT.boardX + 1, vT.boardY}
	}
	// 制約付きBFS: 始点・終点の隣接制約を加えるため、各頂点の位置を渡す
	srcVertex := Pos{vS.boardX, vS.boardY}
	tgtVertex := Pos{vT.boardX, vT.boardY}
	candidate := rs.BFSConstrained(cs, ct, srcVertex, cs, tgtVertex, ct, occupancy, rs.boardHeight, rs.boardWidth, gameConst)
	if candidate == nil {
		fmt.Fprintln(os.Stderr, srcVertex, tgtVertex)

		return false
	}
	var conflictEdges []int
	for i := 1; i < len(candidate)-1; i++ {
		cell := candidate[i]
		if occupancy[cell.row][cell.col] != nil && occupancy[cell.row][cell.col].typ == "edge" {
			conflictEdges = append(conflictEdges, occupancy[cell.row][cell.col].procIdx)
		}
	}
	solutionRoutes[edge.id] = RouteSolution{route: candidate, routeLength: len(candidate), sourcePort: sourcePort, targetPort: targetPort}
	for i, cell := range candidate {
		occupancy[cell.row][cell.col] = &Cell{typ: "edge", procIdx: edge.procIdx, isEndpoint: (i == 0 || i == len(candidate)-1)}
	}
	if len(conflictEdges) > 0 {
		fmt.Fprintln(os.Stderr, index, edge.procIdx, conflictEdges)
		fmt.Fprintln(os.Stderr, solutionRoutes[8])
		if !rs.ResolveConflictCluster(conflictEdges, occupancy, solutionRoutes, gameConst) {
			fmt.Fprintln(os.Stderr, conflictEdges, "could not resolved")
			return false
		}
		fmt.Fprintln(os.Stderr, solutionRoutes[8])
	}
	if rs.RouteEdgesSequentially(edges, occupancy, solutionRoutes, index+1, gameConst) {
		return true
	}
	return false
}

// BFSConstrained: 上記ヘルパー関数の改良版。start から target までの最短路を求める際、
// 始点（srcVertex）および終点（tgtVertex）の4近傍に属するセルについて、
// 指定された接続セル (srcDesignated, tgtDesignated) 以外は通行禁止とする。
type Node struct {
	pos  Pos
	dist int
}

type NodeHeap []*Node

func (h NodeHeap) Len() int           { return len(h) }
func (h NodeHeap) Less(i, j int) bool { return h[i].dist < h[j].dist }
func (h NodeHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }

func (h *NodeHeap) Push(x interface{}) {
	// Push and Pop use pointer receivers because they modify the slice's length,
	// not just its contents.
	*h = append(*h, x.(*Node))
}
func (h *NodeHeap) Pop() interface{} {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[0 : n-1]
	return x
}

func (rs *RouteSolver) BFSConstrained(start, target, srcVertex, srcDesignated, tgtVertex, tgtDesignated Pos, occupancy [][]*Cell, rows, cols int, gameConst *GameConst) []Pos {
	if srcVertex.row == target.row && srcVertex.col == target.col {
		return []Pos{}
	}
	queue := NodeHeap{}
	heap.Init(&queue)
	heap.Push(&queue, &Node{start, 0})
	visited := make([][]bool, rows)
	prev := make([][]*Pos, rows)
	for i := 0; i < rows; i++ {
		visited[i] = make([]bool, cols)
		prev[i] = make([]*Pos, cols)
	}
	visited[start.row][start.col] = true
	targetij := target.row*BOARD_WIDTH + target.col
	// 探索方向：上、左、右、下
	dirs := []Pos{{-1, 0}, {0, -1}, {0, 1}, {1, 0}}
	for len(queue) > 0 {
		cur := heap.Pop(&queue).(*Node)
		curij := cur.pos.row*BOARD_WIDTH + cur.pos.col
		if cur.pos == target {
			break
		}
		for _, d := range dirs {
			nr := cur.pos.row + d.row
			nc := cur.pos.col + d.col
			if nr < 0 || nr >= rows || nc < 0 || nc >= cols {
				continue
			}
			if visited[nr][nc] {
				continue
			}

			next := Pos{nr, nc}
			ij := nr*BOARD_WIDTH + nc
			if gameConst.distances[ij][targetij] > gameConst.distances[curij][targetij]-1 {
				continue
			}

			if rs.isPort[ij] != -1 {
				ok := true
				v := rs.vertices[rs.isPort[ij]]
				vij := v.x*BOARD_WIDTH + v.y
				for _, d := range v.assignedPorts {
					pij := gameConst.nIJs[vij][d]
					if pij == ij {
						ok = false
					}
				}
				if next.row == start.row && next.col == start.col {
					ok = true
				}
				if next.row == target.row && next.col == target.col {
					ok = true
				}
				if !ok {
					continue
				}
			}
			ok1 := occupancy[cur.pos.row][cur.pos.col] == nil
			ok2 := occupancy[nr][nc] == nil
			if ok1 == ok2 {
				visited[nr][nc] = true
				prev[nr][nc] = &cur.pos
				heap.Push(&queue, &Node{Pos{nr, nc}, cur.dist})

			} else {
				visited[nr][nc] = true
				prev[nr][nc] = &cur.pos
				heap.Push(&queue, &Node{Pos{nr, nc}, cur.dist + 1})

			}
		}
	}
	if !visited[target.row][target.col] {
		return nil
	}
	var path []Pos
	cur := target
	for {
		path = append([]Pos{cur}, path...)
		if cur == start {
			break
		}
		p := prev[cur.row][cur.col]
		if p == nil {
			break
		}
		cur = *p
	}
	return path
}

type RouteSolution struct {
	route       []Pos
	routeLength int
	sourcePort  int
	targetPort  int
}

// Run executes the entire procedure.
// ・TransformCoordinates() で頂点を復元し、procedures の type==0 で頂点のprocIdxを設定。
// ・BuildVertexMap(), BuildEdges() により内部データを構築し、procedures の type==1 で辺のprocIdxを設定。
// ・AssignProvisionalPorts() により仮決定し、procedures の順序に沿ってエッジをルーティングする。
// ・最終的に、盤面（BOARD_WIDTH×BOARD_HEIGHT）の各セルに、そのセルが配置されたときの procedure index (procIdx) を出力する。
func (rs *RouteSolver) Run(gameConst *GameConst) ([]int, bool) {
	ok := true
	// 1. TransformCoordinates
	rs.TransformCoordinates()
	// 2. AssignProcedureIndices
	rs.AssignProcedureIndices()
	// 3. BuildVertexMap
	rs.BuildVertexMap()
	// 4. BuildEdges
	rs.BuildEdges()
	// 5. occupancy の初期化（盤面全体）
	occupancy := make([][]*Cell, rs.boardHeight)
	for i := 0; i < rs.boardHeight; i++ {
		occupancy[i] = make([]*Cell, rs.boardWidth)
	}

	solutionRoutes := make(map[int]RouteSolution)
	// 6. 確定しているedgeを処理
	rs.RouteFixedEdges(occupancy, gameConst, solutionRoutes)
	// 7. AssignProvisionalPorts
	rs.AssignProvisionalPorts(occupancy, gameConst)

	// 8. procedures のうち type==1 の順序でエッジを処理する
	var edgeProcOrder []Edge
	for _, proc := range rs.procedures {
		if proc[0] == 1 {
			idx := proc[1]
			_, exist := solutionRoutes[idx]
			// fmt.Fprintln(os.Stderr, exist)
			if !exist {
				edgeProcOrder = append(edgeProcOrder, rs.edgeList[idx])
			}
		}
	}
	// 9. 各頂点を盤面に配置（表示番号は頂点のprocIdx）※注意：ここでは盤面インデックスは [boardX][boardY] とする
	for _, v := range rs.vertices {
		occupancy[v.boardX][v.boardY] = &Cell{typ: "vertex", procIdx: v.procIdx, isEndpoint: true}
	}
	// 10. エッジの順次ルーティング
	if !rs.RouteEdgesSequentially(edgeProcOrder, occupancy, solutionRoutes, 0, gameConst) {
		// fmt.Println("ルーティング解が見つかりませんでした。")
		for i := range occupancy {
			for j := range occupancy[i] {
				v := -1
				if occupancy[i][j] != nil {
					v = occupancy[i][j].procIdx
				}
				if DEBUG {
					fmt.Fprintf(os.Stderr, "%3d ", v)
					if j == BOARD_WIDTH-1 {
						fmt.Fprintln(os.Stderr)
					}
				}
			}
		}
		fmt.Fprintln(os.Stderr, gameConst.NumPeople)
		ok = false
		// panic("")

	}
	// 11. occupancy を一次元配列に変換。各セルには、そのセルのprocedure index (procIdx) を格納
	board := make([]int, rs.boardWidth*rs.boardHeight)
	for r := 0; r < rs.boardHeight; r++ {
		for c := 0; c < rs.boardWidth; c++ {
			idx := r*rs.boardWidth + c
			if occupancy[r][c] != nil {
				board[idx] = occupancy[r][c].procIdx
			} else {
				board[idx] = -1
			}
		}
	}
	return board, ok
}

func (rs *RouteSolver) RouteFixedEdges(occupancy [][]*Cell, gameConst *GameConst, solutionRoutes map[int]RouteSolution) {
	for _, edge := range rs.edgeList {
		from := rs.IJs[edge.source]
		to := rs.IJs[edge.target]
		isLine := false
		if from%BOARD_WIDTH == to%BOARD_WIDTH {
			isLine = true
		}
		if from/BOARD_WIDTH == to/BOARD_WIDTH {
			isLine = true
		}
		if isLine {
			path := make([]Pos, 0, gameConst.distances[from][to])
			cur := from

			sourcePort := 0
			targetPort := 0
			for {
				p := Pos{cur / BOARD_WIDTH, cur % BOARD_WIDTH}
				if cur != from && cur != to {
					path = append(path, p)
				}
				if cur == to {
					break
				}
				for d := 0; d < 4; d++ {
					next := gameConst.nIJs[cur][d]
					if next == -1 {
						continue
					}
					if gameConst.distances[next][to] > gameConst.distances[cur][to]-1 {
						continue
					}
					if cur == from {
						sourcePort = d
						targetPort = 3 - d
					}
					cur = next
					break
				}
			}
			// pathを確定
			solutionRoutes[edge.id] = RouteSolution{route: path, routeLength: len(path), sourcePort: sourcePort, targetPort: targetPort}
			// fmt.Fprintln(os.Stderr,path)
			for i, cell := range path {
				occupancy[cell.row][cell.col] = &Cell{typ: "edge", procIdx: edge.procIdx, isEndpoint: (i == 0 || i == len(path)-1)}
			}
		}
	}
}

type CoverAnnealingPlan struct {
	fixed      [BOARD_WIDTH * BOARD_WIDTH]bool
	placed     [BOARD_WIDTH * BOARD_WIDTH]int
	numCovered [][2]int
	income     int
	potential  int
	cost       float64
}

func NewCoverAnnealingPlan(gameConst *GameConst) *CoverAnnealingPlan {
	plan := CoverAnnealingPlan{}
	plan.numCovered = make([][2]int, gameConst.NumPeople)
	return &plan
}

func (plan *CoverAnnealingPlan) Copy(gameConst *GameConst) *CoverAnnealingPlan {
	newP := NewCoverAnnealingPlan(gameConst)
	newP.Load(plan)
	return newP
}

func (plan *CoverAnnealingPlan) Load(plan2 *CoverAnnealingPlan) {
	plan.placed = plan2.placed
	for i := range plan.numCovered {
		plan.numCovered[i] = plan2.numCovered[i]
	}
	plan.fixed = plan2.fixed
	plan.income = plan2.income
	plan.potential = plan2.potential
	plan.cost = plan2.cost
}

func (plan *CoverAnnealingPlan) setIJs(gameConst *GameConst, IJs []int) {
	action := NewCoverAnnealingAction()
	for i := range IJs {
		action.ij = IJs[i]
		plan.ApplyAction(gameConst, action, false)
	}
}

func (plan *CoverAnnealingPlan) ApplyAction(gameConst *GameConst, action *CoverAnnealingAction, fix bool) {
	if action.ij2 != -1 {
		a1 := NewCoverAnnealingAction()
		a1.ij = action.ij
		a2 := NewCoverAnnealingAction()
		a2.ij = action.ij2
		plan.ApplyAction(gameConst, a1, fix)
		plan.ApplyAction(gameConst, a2, fix)
		return
	}
	if fix {
		plan.fixed[action.ij] = true
	}
	if plan.placed[action.ij] == 0 {
		// add
		plan.placed[action.ij] = 1
		for k := range gameConst.IncludedPoints[action.ij] {
			for _, peopleIndex := range gameConst.IncludedPoints[action.ij][k] {
				if plan.numCovered[peopleIndex][k] == 0 {
					if plan.numCovered[peopleIndex][1-k] > 0 {
						plan.income += gameConst.Fares[peopleIndex]
					}
					plan.potential += gameConst.Fares[peopleIndex]
				}
				plan.numCovered[peopleIndex][k]++
			}
		}
		for _, nij := range gameConst.nearIJs[action.ij] {
			if nij == -1 {
				continue
			}
			if action.ij == nij {
				continue
			}
			if plan.placed[nij] == 1 {
				plan.cost += 1000
			}
		}
		i := action.ij / BOARD_WIDTH
		j := action.ij % BOARD_WIDTH
		di := absInt(i - BOARD_WIDTH/2)
		dj := absInt(j - BOARD_WIDTH/2)
		plan.cost += math.Sqrt(float64(di*di+dj*dj)) + 20
	} else {
		// remove
		plan.placed[action.ij] = 0
		for k := range gameConst.IncludedPoints[action.ij] {
			for _, peopleIndex := range gameConst.IncludedPoints[action.ij][k] {
				if plan.numCovered[peopleIndex][k] == 1 {
					if plan.numCovered[peopleIndex][1-k] > 0 {
						plan.income -= gameConst.Fares[peopleIndex]
					}
					plan.potential -= gameConst.Fares[peopleIndex]
				}
				plan.numCovered[peopleIndex][k]--
			}
		}
		for _, nij := range gameConst.nearIJs[action.ij] {
			if nij == -1 {
				continue
			}
			if action.ij == nij {
				continue
			}
			if plan.placed[nij] == 1 {
				plan.cost -= 1000
			}
		}
		i := action.ij / BOARD_WIDTH
		j := action.ij % BOARD_WIDTH
		di := absInt(i - BOARD_WIDTH/2)
		dj := absInt(j - BOARD_WIDTH/2)
		plan.cost -= math.Sqrt(float64(di*di+dj*dj)) + 20
	}
}

func (plan *CoverAnnealingPlan) RevertAction(gameConst *GameConst, action *CoverAnnealingAction) {
	plan.ApplyAction(gameConst, action, false)
}

func (plan *CoverAnnealingPlan) Evaluate(gameConst *GameConst) float64 {
	if float64(plan.income) > float64(gameConst.sumFares-40)*0.98 {
		return -float64(plan.cost)
	}
	return -INF + 100*float64(plan.income)
}

func (plan *CoverAnnealingPlan) EvaluateDiff(gameConst *GameConst, action *CoverAnnealingAction) float64 {
	return 0.0
}

type CoverAnnealingAction struct {
	ij  int
	ij2 int
}

func NewCoverAnnealingAction() *CoverAnnealingAction {
	return &CoverAnnealingAction{ij2: -1}
}

type CoverAnnealing struct {
	startTime             time.Time
	duration              int     // 焼きなまし実行時間　microsecond
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int                 // loopの回った回数
	valid          int                 // validな解を得た回数
	accepted       int                 // 採用された回数
	temp           float64             // 現在の温度
	reciprocaltemp float64             // 温度の逆数
	best           *CoverAnnealingPlan // これまでの最善解
	bestScore      float64             // 最善解のスコア
	now            *CoverAnnealingPlan // 現在の遷移元
	nowScore       float64             // 現在の遷移元のスコア
}

func NewCA() *CoverAnnealing {
	sa := CoverAnnealing{}
	return &sa
}

func (sa *CoverAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	p := sa.getAcceptRateFromDiff(diff, bestScore)
	return p
}
func (sa *CoverAnnealing) getAcceptRateFromDiff(diff float64, bestScore float64) float64 {
	p := math.Exp(diff * sa.reciprocaltemp)
	return p
}

func (sa *CoverAnnealing) isBest(score float64, bestScore float64) bool {
	return score > bestScore

}

func (sa *CoverAnnealing) Init(duration int, startTemp float64, endTemp float64, best *CoverAnnealingPlan, bestScore float64, gameConst *GameConst) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy(gameConst)
	sa.nowScore = bestScore

}

func (sa *CoverAnnealing) Run(gameConst *GameConst) {
	prop := 0.0
	sa.updateTempIntervalExp = 13
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			if prop >= 1 {
				// time up
				break
			}
			if sa.isBest(sa.nowScore, sa.bestScore) {
				// 最善解を変更
				sa.best = sa.now.Copy(gameConst)
				sa.bestScore = sa.nowScore
			}
			if DEBUG {
				fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sa.sumIter, sa.bestScore)
			}
			// 指数ver.
			sa.temp = math.Pow(sa.startTemp, 1-prop) * math.Pow(sa.endTemp, prop)
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			// 最善解に戻す
			// sa.now = sa.best.Copy(gameConst)
			// sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		action := NewCoverAnnealingAction()
		pattern := gameConst.Random.Intn(10)
		if pattern < 0 {
			action.ij = gameConst.Random.Intn(BOARD_WIDTH * BOARD_WIDTH)
		} else {
			peopleIndex := gameConst.Random.Intn(gameConst.NumPeople)
			k := gameConst.Random.Intn(2)
			if sa.now.numCovered[peopleIndex][k] == 0 {
				continue
			}
			ij := gameConst.Poss[peopleIndex][k]
			for {
				action.ij = ij + NEAR_DIJ[gameConst.Random.Intn(13)]
				if action.ij < 0 || action.ij >= BOARD_WIDTH*BOARD_WIDTH {
					continue
				}
				if sa.now.placed[action.ij] == 0 {
					continue
				}
				break
			}
			for {
				action.ij2 = ij + NEAR_DIJ[gameConst.Random.Intn(13)]
				if action.ij2 < 0 || action.ij2 >= BOARD_WIDTH*BOARD_WIDTH {
					continue
				}

				break
			}

			action.ij = gameConst.Random.Intn(BOARD_WIDTH * BOARD_WIDTH)

		}
		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.Float64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff
		if sa.now.fixed[action.ij] {
			continue
		}
		if action.ij2 != -1 && sa.now.fixed[action.ij2] {
			continue
		}

		sa.now.ApplyAction(gameConst, action, false)
		sa.valid++
		score := sa.now.Evaluate(gameConst)
		p := sa.getAcceptRate(score, sa.nowScore)
		// p := sa.getAcceptRate(score, sa.bestScore)
		r := gameConst.Random.Float64()
		if p >= r {
			// 採用
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				// sa.best = sa.now.Copy(gameConst)
				// sa.bestScore = sa.nowScore
			}
		} else {
			// rollback（nowを戻す）
			sa.now.RevertAction(gameConst, action)
		}
	}
	sa.best = sa.now.Copy(gameConst)
	sa.bestScore = sa.nowScore
	fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}

type StartAnnealingPlan struct {
	stations  []int
	income    int
	potential int
}

func NewStartAnnealingPlan(gameConst *GameConst) *StartAnnealingPlan {
	plan := StartAnnealingPlan{}
	plan.stations = make([]int, 4)
	for i := range plan.stations {
		plan.stations[i] = 57 * i
	}
	for i := range plan.stations {
		plan.potential += gameConst.potential[plan.stations[i]]
	}
	for i := range plan.stations {
		for j := range plan.stations {
			plan.income += gameConst.effects[plan.stations[i]][plan.stations[j]]
		}
	}
	return &plan
}

func (plan *StartAnnealingPlan) Copy(gameConst *GameConst) *StartAnnealingPlan {
	newP := NewStartAnnealingPlan(gameConst)
	newP.Load(plan)
	return newP
}

func (plan *StartAnnealingPlan) Load(plan2 *StartAnnealingPlan) {
	for i := range plan2.stations {
		plan.stations[i] = plan2.stations[i]
	}
	plan.potential = plan2.potential
	plan.income = plan2.income
}

func (plan *StartAnnealingPlan) ApplyAction(gameConst *GameConst, action *StartAnnealingAction) {
	// revert
	for i := range plan.stations {
		if i == action.index {
			continue
		}
		plan.income -= gameConst.effects[plan.stations[i]][action.ij]
	}
	plan.potential -= gameConst.potential[action.ij]
	plan.stations[action.index] = -1

	// apply
	for i := range plan.stations {
		if i == action.index {
			continue
		}
		plan.income += gameConst.effects[plan.stations[i]][action.ij2]
	}
	plan.potential += gameConst.potential[action.ij2]
	plan.stations[action.index] = action.ij2
}

func (plan *StartAnnealingPlan) RevertAction(gameConst *GameConst, action *StartAnnealingAction) {
	revA := NewStartAnnealingAction()
	revA.index = action.index
	revA.ij = action.ij2
	revA.ij2 = action.ij
	plan.ApplyAction(gameConst, revA)
}

func (plan *StartAnnealingPlan) IsValid(gameConst *GameConst) bool {
	for i := range plan.stations {
		for j := range plan.stations {
			if i == j {
				continue
			}
			if gameConst.distances[plan.stations[i]][plan.stations[j]] <= 4 {
				return false
			}
		}
	}
	return true
}

func (plan *StartAnnealingPlan) Evaluate(gameConst *GameConst, comment bool) float64 {
	groups := make([]int, len(plan.stations))
	for i := range groups {
		groups[i] = i
	}
	edges := make([][2]int, len(plan.stations)-1)
	for loop := 0; loop < len(plan.stations)-1; loop++ {
		mindistance := INF
		mini := 0
		minj := 0
		for i := range plan.stations {
			for j := i + 1; j < len(plan.stations); j++ {
				if groups[i] == groups[j] {
					continue
				}
				if gameConst.distances[plan.stations[i]][plan.stations[j]] < mindistance {
					mindistance = gameConst.distances[plan.stations[i]][plan.stations[j]]
					mini = i
					minj = j
				}
			}
		}
		edges[loop][0] = mini
		edges[loop][1] = minj
		oldg := groups[minj]
		newg := groups[mini]
		for i := range groups {
			if groups[i] == oldg {
				groups[i] = newg
			}
		}
	}
	turn := 1
	income := 0
	money := gameConst.InitialMoney
	money -= 5000
	// 貪欲に採用する
	bestEdge := -1
	bestTurn := INF

	used := make([]bool, len(edges))
	usable := make([]bool, len(edges))
	connected := make([]bool, len(plan.stations))

	// 貪欲に採用する
	bestEdge = -1
	bestTurn = INF
	for i := range edges {
		// 次7000に到達するのが最速な辺を選択
		distance := gameConst.distances[plan.stations[edges[i][0]]][plan.stations[edges[i][1]]]
		cost := 4900 + 100*distance
		t := 0
		if cost > money {
			t = MAX_TURN
		} else if gameConst.effects[plan.stations[edges[i][0]]][plan.stations[edges[i][1]]] == 0 {
			t = MAX_TURN
		} else {
			t = (cost-money+income*distance)/gameConst.effects[plan.stations[edges[i][0]]][plan.stations[edges[i][1]]] + 1
			t = maxInt(t, 0)
			t += (7000-maxInt(0, -(cost-money+income*distance)))/(income+gameConst.effects[plan.stations[edges[i][0]]][plan.stations[edges[i][1]]]) + 1
		}

		if bestTurn > t+distance {
			bestTurn = t + distance
			bestEdge = i
		}
	}
	distance := gameConst.distances[plan.stations[edges[bestEdge][0]]][plan.stations[edges[bestEdge][1]]]
	cost := 4900 + 100*distance
	turn += distance
	money += income * distance
	// t := (cost-money+income*distance)/gameConst.effects[plan.stations[edges[bestEdge][0]]][plan.stations[edges[bestEdge][1]]] + 1
	// t = maxInt(t, 0)
	// money += income * t
	money -= cost
	// turn += t
	for _, v := range edges[bestEdge] {
		if connected[v] {
			continue
		}
		for i := range connected {
			if connected[i] {
				income += gameConst.effects[plan.stations[i]][plan.stations[v]]
			}
		}
		connected[v] = true
	}
	used[bestEdge] = true
	usable[bestEdge] = false
	for i := range usable {
		if used[i] {
			continue
		}
		if edges[i][0] == edges[bestEdge][0] {
			usable[i] = true
		}
		if edges[i][0] == edges[bestEdge][1] {
			usable[i] = true
		}
		if edges[i][1] == edges[bestEdge][0] {
			usable[i] = true
		}
		if edges[i][1] == edges[bestEdge][1] {
			usable[i] = true
		}
	}

	if income == 0 {
		return -10000 + float64(plan.potential)
	}

	for loop := 0; loop < len(edges)-1; loop++ {
		// 貪欲に採用する
		bestEdge = -1
		bestTurn = INF
		for i := range edges {
			if used[i] {
				continue
			}
			if !usable[i] {
				continue
			}
			// 次7000に到達するのが最速な辺を選択
			distance := gameConst.distances[plan.stations[edges[i][0]]][plan.stations[edges[i][1]]]
			cost := 4900 + 100*distance
			t := 0
			plusincome := 0
			for _, v := range edges[i] {
				if connected[v] {
					continue
				}
				for j := range connected {
					if connected[j] {
						plusincome += gameConst.effects[plan.stations[j]][plan.stations[v]]
					}
				}
			}
			t = (cost-money+income*distance)/(income) + 1
			t = maxInt(t, 0)
			t += (7000-maxInt(0, -(cost-money+income*distance)))/(income+plusincome) + 1

			if bestTurn > t+distance {
				bestTurn = t + distance
				bestEdge = i
			}
		}
		distance := gameConst.distances[plan.stations[edges[bestEdge][0]]][plan.stations[edges[bestEdge][1]]]
		cost := 4900 + 100*distance
		turn += distance
		money += income * distance
		t := (cost-money+income*distance)/income + 1
		t = maxInt(t, 0)
		money += income * t
		money -= cost
		turn += t
		for _, v := range edges[bestEdge] {
			if connected[v] {
				continue
			}
			for i := range connected {
				if connected[i] {
					income += gameConst.effects[plan.stations[i]][plan.stations[v]]
				}
			}
			connected[v] = true
		}
		used[bestEdge] = true
		usable[bestEdge] = false
		for i := range usable {
			if used[i] {
				continue
			}
			if edges[i][0] == edges[bestEdge][0] {
				usable[i] = true
			}
			if edges[i][0] == edges[bestEdge][1] {
				usable[i] = true
			}
			if edges[i][1] == edges[bestEdge][0] {
				usable[i] = true
			}
			if edges[i][1] == edges[bestEdge][1] {
				usable[i] = true
			}
		}
	}
	if comment {
		fmt.Fprintln(os.Stderr, plan.income, income, turn)
	}
	return float64(plan.income) / float64(turn)
	// return float64(plan.income) + float64(plan.potential)*0.01
}

func (plan *StartAnnealingPlan) EvaluateDiff(gameConst *GameConst, action *StartAnnealingAction) float64 {
	return 0.0
}

type StartAnnealingAction struct {
	index int
	ij    int
	ij2   int
}

func NewStartAnnealingAction() *StartAnnealingAction {
	return &StartAnnealingAction{}
}

type StartAnnealing struct {
	startTime             time.Time
	duration              int     // 焼きなまし実行時間　microsecond
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int                 // loopの回った回数
	valid          int                 // validな解を得た回数
	accepted       int                 // 採用された回数
	temp           float64             // 現在の温度
	reciprocaltemp float64             // 温度の逆数
	best           *StartAnnealingPlan // これまでの最善解
	bestScore      float64             // 最善解のスコア
	now            *StartAnnealingPlan // 現在の遷移元
	nowScore       float64             // 現在の遷移元のスコア
}

func NewSA() *StartAnnealing {
	sa := StartAnnealing{}
	return &sa
}

func (sa *StartAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	p := sa.getAcceptRateFromDiff(diff, bestScore)
	return p
}
func (sa *StartAnnealing) getAcceptRateFromDiff(diff float64, bestScore float64) float64 {
	p := math.Exp(diff * sa.reciprocaltemp)
	return p
}

func (sa *StartAnnealing) isBest(score float64, bestScore float64) bool {
	return score > bestScore

}

func (sa *StartAnnealing) Init(duration int, startTemp float64, endTemp float64, best *StartAnnealingPlan, bestScore float64, gameConst *GameConst) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy(gameConst)
	sa.nowScore = bestScore

}

func (sa *StartAnnealing) Run(gameConst *GameConst) {
	prop := 0.0
	sa.updateTempIntervalExp = 11
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			if prop >= 1 {
				// time up
				break
			}

			if DEBUG {
				fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sa.sumIter, sa.bestScore)
			}
			// 指数ver.
			sa.temp = math.Pow(sa.startTemp, 1-prop) * math.Pow(sa.endTemp, prop)
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			// 最善解に戻す
			// sa.now = sa.best.Copy(gameConst)
			// sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		action := NewStartAnnealingAction()
		k := gameConst.Random.Intn(len(sa.now.stations))
		ij := gameConst.candidates[gameConst.Random.Intn(len(gameConst.candidates))]
		action.index = k
		action.ij = sa.now.stations[k]
		action.ij2 = ij
		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.Float64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff

		sa.now.ApplyAction(gameConst, action)
		if !sa.now.IsValid(gameConst) {
			sa.now.RevertAction(gameConst, action)
			continue
		}
		sa.valid++
		score := sa.now.Evaluate(gameConst, false)
		p := sa.getAcceptRate(score, sa.nowScore)
		// p := sa.getAcceptRate(score, sa.bestScore)
		r := gameConst.Random.Float64()
		if p >= r {
			// 採用
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				sa.best = sa.now.Copy(gameConst)
				sa.bestScore = sa.nowScore
			}
		} else {
			// rollback（nowを戻す）
			sa.now.RevertAction(gameConst, action)
		}
	}
	if DEBUG {
		sa.now.Evaluate(gameConst, true)
		fmt.Fprintln(os.Stderr, "--- SA Summary ---")
		fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
		fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
		fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
		fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
	}
}
