// last local submission: ---
package main

import (
	"bufio"
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
	INF = 1000000000
	// Main
	TIME_LIMIT = 3050 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01

	MINIMIZE = true

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = true  // 複数ケースを試すか
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 3000
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FOLDER         = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "0026"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = true
	PRINT_ACTION       = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 3                                // 基本戦術、方針レベルの違い
	VERSION_MINOR = 1                                // 改善等
	VERSION_PATCH = 0                                // バグ修正,パラメータ変更など
	VERSION_NAME  = "Vertically PreDivide(HugeTime)" // バージョン名
	VERSION_VALID = false                            // 提出に利用できる正当なものか

)

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
	W, _ := strconv.Atoi(splitedInput[0])
	D, _ := strconv.Atoi(splitedInput[1])
	N, _ := strconv.Atoi(splitedInput[2])
	gConst.W = W
	gConst.D = D
	gConst.N = N

	gConst.Reservations = make([][]int, 0, D)
	gConst.SumAreas = make([]int, D)
	for d := 0; d < D; d++ {
		gConst.Scanner.Scan()
		splitedInput = strings.Split(gConst.Scanner.Text(), " ")
		reservationsPerDay := make([]int, 0, N)
		for k := 0; k < N; k++ {
			a, _ := strconv.Atoi(splitedInput[k])
			reservationsPerDay = append(reservationsPerDay, a)
			gConst.SumAreas[d] += a
		}
		gConst.Reservations = append(gConst.Reservations, reservationsPerDay)
		gConst.MeanArea += gConst.SumAreas[d]
	}
	gConst.MeanArea /= gConst.D

}

// 現在の状態を記録するもの
type GameState struct {
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	return &g
}

type RandGenerator struct {
	Source    *rand.Rand
	Rand      [1 << 7]float64
	LogRand   [1 << 7]float64
	randIndex int
	x         uint32
}

func NewRandGenerator(source *rand.Rand) *RandGenerator {
	rng := RandGenerator{}
	rng.Source = source
	seed := 265329665
	// seed := 265329467
	rng.x = uint32(seed)
	for i := range rng.LogRand {
		r := rng.Source.Float64()
		rng.Rand[i] = r
		rng.LogRand[i] = math.Log(r)
	}
	return &rng
}

func (rng *RandGenerator) Float64() float64 {
	r := rng.Rand[rng.randIndex]
	rng.randIndex++
	if rng.randIndex == 1<<7 {
		rng.randIndex = 1
	}
	return r
}

/*
	func (rng *RandGenerator) StrongFloat64() float64 {
		r := rng.Source.Float64()
		return r
	}
*/
func (rng *RandGenerator) LogFloat64() float64 {
	r := rng.LogRand[rng.randIndex]
	rng.randIndex++
	if rng.randIndex == 1<<7 {
		rng.randIndex = 1
	}
	return r
}

func (rng *RandGenerator) Intn(a int) int {
	i := int(float64(a) * (rng.Rand[rng.randIndex]))
	rng.randIndex++
	if rng.randIndex == 1<<7 {
		rng.randIndex = 1
	}
	return i
}

/*func (rng *RandGenerator) StrongIntn(a int) int {
	return rng.Source.Intn(a)
}*/

func (rg *RandGenerator) Next() uint32 {
	// 32bit乱数を生成
	rg.x ^= rg.x << 13
	rg.x ^= rg.x >> 17
	rg.x ^= rg.x << 5

	return rg.x
}

func (rg *RandGenerator) StrongIntn(n int) int {
	a := (int(rg.Next()) * n) >> 32
	return a
}

func (rg *RandGenerator) StrongFloat64() float64 {
	// TODO: 高速化
	a := float64(rg.Next()) / (1 << 32)
	return a
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	Random  RandGenerator
	Scanner *bufio.Scanner

	W            int
	D            int
	N            int
	Reservations [][]int // Reservations[d][k] d日目のk番目の予約
	SumAreas     []int
	MeanArea     int
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	// g.Random = *NewRandGenerator(rand.New(rand.NewSource(runner.randSeed)))
	g.Random = *NewRandGenerator(rand.New(rand.NewSource(0)))
	return &g
}

func printActions(actions []*Action, gameState *GameState, gameConst *GameConst, comment bool) {
	for _, action := range actions {
		printAction(action, gameState, gameConst, comment)
	}
}
func printAction(action *Action, gameState *GameState, gameConst *GameConst, comment bool) {
	for i := 0; i < gameConst.N; i++ {
		fmt.Fprintln(os.Stdout, action.posUL[i][0], action.posUL[i][1], action.posDR[i][0], action.posDR[i][1])
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
	start     time.Time
}

func NewRunner(caseIndex int, randseed int64) *Runner {
	runner := Runner{}
	runner.caseIndex = caseIndex
	runner.randSeed = randseed

	return &runner
}

func test(testCase int) Result {
	file := fmt.Sprintf("%04d", testCase)
	runner := NewRunner(testCase, time.Now().UnixNano())
	start := time.Now()
	result := runner.doGame(file)
	duration := time.Since(start).Microseconds()
	if DEBUG {
		fmt.Fprintf(os.Stderr, "score:%d (case=%04d) %d microseconds\n", result.score, testCase, duration)
	}
	return result
}

func parallelTester(prefix int, bests [TEST_NUM]Result) [TEST_NUM]Result {
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

		timeStamp := time.Now().Unix()
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
		start := time.Now()
		result := runner.doGame(FILE_NUMBER)
		duration := time.Since(start).Microseconds()
		if LOCAL_TEST && DEBUG {
			fmt.Fprintf(os.Stderr, "score:%d  %d microseconds\n", result.score, duration)
		}
	}
}

// Action 一日の割り当て
type Action struct {
	posUL [][2]int // 左上座標
	posDR [][2]int // 右下座標
}

func NewAction(N int) *Action {
	a := Action{}
	a.posUL = make([][2]int, N)
	a.posDR = make([][2]int, N)
	return &a
}

func (a *Action) Load(a2 *Action) {
	for i := range a.posUL {
		a.posUL[i] = a2.posUL[i]
	}
	for i := range a.posDR {
		a.posDR[i] = a2.posDR[i]
	}

}
func (a *Action) Copy() *Action {
	newa := NewAction(len(a.posUL))
	newa.Load(a)
	return newa
}

type BitSet uint64

func (bs *BitSet) Init() {
	*bs = BitSet(0)
}

func (bs *BitSet) AddK(k int) {
	*bs |= BitSet(1 << k)
}

func (bs *BitSet) DeleteK(k int) {
	*bs &= ^BitSet(1 << k)
}

func (bs *BitSet) Contains(k int) bool {
	return *bs&BitSet(1<<k) != 0
}

func getScore() float64 {
	return 0.0
}

func evaluate(gameState *GameState, gameConst *GameConst) float64 {
	value := 0.0
	return value
}

type Solver struct {
	timeLimit int // solverに与えられた時間制限 microsecond
	caseIndex int
}

func NewSolver(timeLimit int, caseIndex int) *Solver {
	s := Solver{}
	s.timeLimit = timeLimit
	s.caseIndex = caseIndex
	return &s
}

func (s *Solver) Solve(gameState *GameState, gameConst *GameConst, forceNumGroup int, start time.Time) ([]*Action, int, int) {

	bestActions := make([]*Action, 0)
	for i := 0; i < gameConst.D; i++ {
		bestActions = append(bestActions, NewAction(gameConst.N))
	}
	bestScore := 0.0
	_ = bestScore

	// 貪欲を書くならここ
	res := make([]int, gameConst.N)
	for d := 0; d < gameConst.D; d++ {
		for k := 0; k < gameConst.N; k++ {
			res[k] = maxInt(res[k], gameConst.Reservations[d][k])
		}
	}
	sumA := 0
	for k := range res {
		sumA += res[k]
	}
	// fmt.Fprintln(os.Stderr, sumA, res)

	// numGroups := gameConst.N - 10
	// numGroups := 33
	minNumGroup := 1
	maxNumGroup := gameConst.N
	for sumA > 1000*1000 && maxNumGroup > 1 {
		sumA -= res[maxNumGroup-2]
		maxNumGroup--
	}
	if forceNumGroup > 0 {
		minNumGroup = forceNumGroup
		maxNumGroup = forceNumGroup
	}
	success := false

	var bestPlan *BaseDivPlan
	// for numGroups := int(math.Sqrt(float64(gameConst.N))); ; numGroups = (minNumGroup + maxNumGroup + 1) / 2 {
	for numGroups := (minNumGroup + maxNumGroup + 1) / 2; ; numGroups = (minNumGroup + maxNumGroup + 1) / 2 {
		final := false
		if minNumGroup == maxNumGroup {
			if minNumGroup == 1 || !success {
				final = true
			} else {
				break
			}
		}
		// for numGroups := gameConst.N; numGroups > 0; numGroups-- {
		// for numGroups := 1; numGroups > 0; numGroups-- {
		bda := NewBDA()
		// starttemp := float64(500*1000) * (500 * 1000) * 0.000001
		// endtemp := float64(1*1000) * (1 * 1000) * 0.000001
		starttemp := float64(5 * 10)
		endtemp := float64(1 * 10)
		defaultPlan := NewBaseDivPlan(gameConst.D, gameConst.N, numGroups)
		defaultPlan.SetReasonableGroup(numGroups, gameConst)
		bda.Init(2*1000, 500000, starttemp, endtemp, defaultPlan, -INF)
		if numGroups > 1 {
			bda.Run(gameConst, numGroups, 1, start)
		}
		value := 0
		// fmt.Fprintln(os.Stderr, bda.best.group)
		partcost := 0
		for g := 0; g < numGroups; g++ {
			maxArea := 0
			numpart := 0
			for d := range bda.best.sumarea {
				maxArea = maxInt(maxArea, bda.best.sumarea[d][g])
				for k := range bda.best.group[d] {
					if bda.best.group[d][k] == g {
						numpart++
						// fmt.Fprintf(os.Stderr, "%d ", gameConst.Reservations[d][k])
					}
				}
				numpart--
				// fmt.Fprintln(os.Stderr, "")
			}
			partcost += 2 * numpart * (maxArea / 1000)

			// fmt.Fprintln(os.Stderr, maxArea)
			// fmt.Fprintln(os.Stderr, "")
			value += maxArea
		}
		// fmt.Fprintln(os.Stderr, partcost)
		// fmt.Fprintln(os.Stderr, value, gameConst.MeanArea)
		// fmt.Fprintln(os.Stderr, bda.bestScore)

		// for i := range groupMember {
		// 	groupMember[i] = 1<<gameConst.N - 1
		// }
		// for g := 0; g < numGroups; g++ {
		sumh := 0
		heights := make([]int, numGroups)
		for g := numGroups - 1; g >= 0; g-- {
			maxArea := 0

			for d := 0; d < gameConst.D; d++ {
				c := 0
				for k := 0; k < gameConst.N; k++ {
					if g == bda.best.group[d][k] {
						c++
					}
				}
				maxArea = maxInt(maxArea, bda.best.sumarea[d][g]*gameConst.W/(gameConst.W-(c-1)))
			}
			height := (maxArea-1)/gameConst.W + 1
			if numGroups == 1 {
				height = gameConst.W
			}
			heights[g] = sumh
			sumh += height
		}
		if sumh <= gameConst.W {
			success = true
			bestPlan = bda.best.Copy()
			minNumGroup = numGroups
		} else {
			maxNumGroup = numGroups - 1
		}
		if final {
			break
		}
	}
	numGroups1 := minNumGroup
	if !success {
		return bestActions, -1, numGroups1
	}
	actions1, score1 := s.TryPlan(bestPlan, gameState, gameConst, numGroups1, start)
	if score1 == -1 {
		score1 = INF
	}
	// if /*numGroups1 == 1 &&*/ float64(numGroups1) < math.Sqrt(float64(gameConst.N)) {
	/*if numGroups1 != 1 {
		numGroups2 := 1
		p := NewBaseDivPlan(gameConst.D, gameConst.N, numGroups2)
		p.SetReasonableGroup(numGroups2, gameConst)
		actions2, score2 := s.TryPlan(p, gameState, gameConst, numGroups2)
		fmt.Fprintln(os.Stderr, numGroups1, gameConst.N, s.caseIndex)
		fmt.Fprintln(os.Stderr, score1, score2)
		if score2 == -1 {
			score2 = INF
		}
		if score2 < score1 {
			bestActions = actions2
		} else {
			bestActions = actions1
		}
	} else {
		bestActions = actions1
		}*/
	fmt.Fprintln(os.Stderr, numGroups1, gameConst.N, s.caseIndex)
	bestActions = actions1

	// 貪欲ここまで

	return bestActions, 0, numGroups1
}
func (s *Solver) TryPlan(bestPlan *BaseDivPlan, gameState *GameState, gameConst *GameConst, numGroups int, start time.Time) ([]*Action, int) {
	actions := []*Action{}
	for d := 0; d < gameConst.D; d++ {
		actions = append(actions, NewAction(gameConst.N))
	}
	divmode := 0
	if numGroups > 1 {
		divmode = 1
	}
	heights := make([]int, numGroups)
	sumh := 0
	for g := numGroups - 1; g >= 0; g-- {
		maxArea := 0

		for d := 0; d < gameConst.D; d++ {
			c := 0
			for k := 0; k < gameConst.N; k++ {
				if g == bestPlan.group[d][k] {
					c++
				}
			}
			maxArea = maxInt(maxArea, bestPlan.sumarea[d][g]*gameConst.W/(gameConst.W-(c-1)))
		}
		height := (maxArea-1)/gameConst.W + 1
		if numGroups == 1 {
			height = gameConst.W
		}
		heights[g] = sumh
		sumh += height
	}
	// groupごとに分けて考えたい
	groupMember := make([]BitSet, gameConst.D)
	offset := [2]int{0, 0}
	for g := numGroups - 1; g >= 0; g-- {
		for d := 0; d < gameConst.D; d++ {
			groupMember[d] = 0
		}
		for d := 0; d < gameConst.D; d++ {
			for k := 0; k < gameConst.N; k++ {
				if g == bestPlan.group[d][k] {
					(&groupMember[d]).AddK(k)
				}
			}
		}

		maxArea := 0
		for d := 0; d < gameConst.D; d++ {
			c := 0
			for k := 0; k < gameConst.N; k++ {
				if g == bestPlan.group[d][k] {
					c++
				}
			}
			maxArea = maxInt(maxArea, bestPlan.sumarea[d][g]*gameConst.W/(gameConst.W-(c-1)))
		}
		height := (maxArea-1)/gameConst.W + 1
		if numGroups == 1 {
			height = gameConst.W
		}

		offset[0] = heights[g]
		s.SolveForGroup(actions, gameState, gameConst, groupMember, [2]int{height, gameConst.W}, offset, divmode, start)
		// 今回のgroupでの割り当てに添うように、今後のグループに余裕を持たせられそうならする
		for d := 0; d < gameConst.D; d++ {
			for k := gameConst.N - 1; k >= 0; k-- {
				if bestPlan.group[d][k] != g {
					continue
				}
				area := (actions[d].posUL[k][0] - actions[d].posDR[k][0]) * (actions[d].posUL[k][1] - actions[d].posDR[k][1])
				for k2 := gameConst.N - 1; k2 > k; k2-- {
					if gameConst.Reservations[d][k2] > area {
						continue
					}
					if bestPlan.group[d][k] >= g {
						continue
					}
					// 変更
					bestPlan.ApplySwap(gameConst, d, k, k2)
					actions[d].posUL[k2], actions[d].posUL[k] = actions[d].posUL[k], actions[d].posUL[k2]
					actions[d].posDR[k2], actions[d].posDR[k] = actions[d].posDR[k], actions[d].posDR[k2]
					break
				}
			}
		}
	}
	if divmode == 1 {
		newHeights := []int{}
		if sumh < 1000 {
			numGroups++
			newHeights = append(newHeights, sumh)
		}
		newHeights = append(newHeights, heights...)
		pda := NewPDA()
		partDivPlan := NewPartDivPlan(gameConst, numGroups, gameConst.D)
		partDivPlan.Import(gameConst, newHeights, actions)
		// pda.Init(gameConst, 2*1000, 470000, 100*1000*0.001, 200*0.001, partDivPlan, -INF)
		pda.Init(gameConst, 2*1000, 2100000, 100*0.001, 10*.001, partDivPlan, -INF)
		pda.Run(gameConst, newHeights, numGroups, 1, start)
		intervals := []int{}
		// fmt.Fprintln(os.Stderr, heights)
		for i := range newHeights {
			interval := gameConst.W - newHeights[i]
			if i > 0 {
				interval -= gameConst.W - newHeights[i-1]
			}
			intervals = append(intervals, interval)
		}
		for d := 0; d < gameConst.D; d++ {
			rects := pda.best.ToRects(gameConst, intervals, d)
			_ = rects
			areas := pda.best.Toareas(gameConst, intervals, d)
			_ = areas
			for i := 0; i < gameConst.N; i++ {
				k := gameConst.N - 1 - i
				index := len(rects) - 1 - i
				if index < 0 {
					break
				}
				actions[d].posUL[k][0] = rects[index][0][0]
				actions[d].posUL[k][1] = rects[index][0][1]
				actions[d].posDR[k][0] = rects[index][1][0]
				actions[d].posDR[k][1] = rects[index][1][1]
			}
		}
	}
	score := calcScore(actions, gameState, gameConst)
	// score := 1
	return actions, score
}

// groupMember[d]:その日該当groupに入っている予約
func (s *Solver) SolveForGroup(bestActions []*Action, gameState *GameState, gameConst *GameConst, groupmember []BitSet, groupArea [2]int, groupOffset [2]int, divmode int, start time.Time) {
	prevTree := NewRectDivisionTree(groupArea)

	nextd := 0
	for d := 0; d < gameConst.D; d++ {
		numMember := popcount(int(groupmember[d]))
		iterator := NewBitIterator(uint64(groupmember[d]))
		reservationIndexes := []int{}
		for !iterator.IsEmpty() {
			k := iterator.Next()
			reservationIndexes = append(reservationIndexes, k)
		}
		if nextd == d {
			reservations := []int{}
			iterator := NewBitIterator(uint64(groupmember[d]))
			for !iterator.IsEmpty() {
				k := iterator.Next()
				reservations = append(reservations, gameConst.Reservations[d][k])
			}
			// d+plusd日目までをまとめて計算したい
			maxReservations := []int{}
			iterator = NewBitIterator(uint64(groupmember[d]))
			for !iterator.IsEmpty() {
				k := iterator.Next()
				maxReservations = append(maxReservations, gameConst.Reservations[d][k])
			}
			plusd := 0
			boarder := 995 * groupArea[0] * groupArea[1] / 1000
			if d > 0 {
				boarder = 950 * groupArea[0] * groupArea[1] / 1000
			}
			for {
				plusd++
				if d+plusd >= gameConst.D {
					plusd--
					break
				}
				if popcount(int(groupmember[d+plusd])) != numMember {
					plusd--
					break
				}
				sum := 0
				iterator = NewBitIterator(uint64(groupmember[d+plusd]))
				for i := 0; i < numMember; i++ {
					k := iterator.Next()
					maxReservations[i] = maxInt(maxReservations[i], gameConst.Reservations[d+plusd][k])

					sum += maxReservations[i]
				}

				if sum > boarder {
					plusd--
					break
				}
			}
			for d2 := 1; d2 <= plusd; d2++ {
				iterator = NewBitIterator(uint64(groupmember[d+d2]))
				for i := 0; i < numMember; i++ {
					k := iterator.Next()
					reservations[i] = maxInt(reservations[i], gameConst.Reservations[d+d2][k])

				}
			}
			nextd = d + plusd + 1
			sumA := 0
			for i := range reservations {
				sumA += reservations[i]
			}
			merginPermill := (sumA + gameConst.W*gameConst.W) / 2 / 1000
			merginPermill = maxInt(merginPermill, 997)

			// prevTreeにできるだけ合わせたい
			baseTree := NewRectDivisionTree(groupArea)
			leaves := []*RectDivisionTreeNode{baseTree.root}
			correspondence := []*RectDivisionTreeNode{prevTree.root}
			divisible := []bool{prevTree.root.divdir != -1}
			members := []BitSet{1<<numMember - 1}
			areaDiffs := []int{gameConst.SumAreas[d] - baseTree.root.size[0]*baseTree.root.size[1]*merginPermill/1000}
			for {
				if len(leaves) == numMember {
					break
				}
				if time.Since(start).Microseconds() > TIME_LIMIT*95/100 {
					break
				}
				// 最大の葉を取得する
				// 切断量で大きさを定義？
				var maxLeaf *RectDivisionTreeNode
				leafIndex := -1
				maxSize := 0
				for i := range leaves {
					if !divisible[i] {
						continue
					}
					// size := leaves[i].size[0] * leaves[i].size[1]
					size := leaves[i].size[1-correspondence[i].divdir]
					if size > maxSize {
						maxSize = size
						maxLeaf = leaves[i]
						leafIndex = i
					}
				}
				if leafIndex == -1 {
					break
				}
				// 割る
				divdir := correspondence[leafIndex].divdir
				target := correspondence[leafIndex].childs[0].size[divdir]
				maxLeaf.Divide(divdir, target, NewRectDivisionTreeNode(), NewRectDivisionTreeNode())
				baseTree.numGroup++
				newLeaves := []*RectDivisionTreeNode{}
				newCorrespondence := []*RectDivisionTreeNode{}
				newDivisible := []bool{}
				newMembers := []BitSet{}
				newAreaDiffs := []int{}
				newLeaves = append(newLeaves, leaves...)
				newCorrespondence = append(newCorrespondence, correspondence...)
				newDivisible = append(newDivisible, divisible...)
				newMembers = append(newMembers, members...)
				newAreaDiffs = append(newAreaDiffs, areaDiffs...)
				c0 := maxLeaf.childs[0]
				cc0 := correspondence[leafIndex].childs[0]
				c1 := maxLeaf.childs[1]
				cc1 := correspondence[leafIndex].childs[1]
				newLeaves[leafIndex] = c0
				newCorrespondence[leafIndex] = cc0
				newDivisible[leafIndex] = cc0.divdir != -1
				newAreaDiffs[leafIndex] += c1.size[0] * c1.size[1] * merginPermill / 1000
				newLeaves = append(newLeaves, c1)
				newCorrespondence = append(newCorrespondence, cc1)
				newDivisible = append(newDivisible, cc1.divdir != -1)
				newMembers = append(newMembers, 0)
				newAreaDiffs = append(newAreaDiffs, -c1.size[0]*c1.size[1]*merginPermill/1000)

				// ここでmemberを調整

				// 初期解の構成
				for i := range newMembers {
					newMembers[i] = 0
					newAreaDiffs[i] = -newLeaves[i].size[0] * newLeaves[i].size[1] * merginPermill / 1000
				}
				order := make([]int, len(newLeaves))
				for i := range order {
					order[i] = i
				}

				// 初期解の構成のためにいつもの逆順でソート
				sort.Slice(order, func(i, j int) bool { return newAreaDiffs[order[i]] < newAreaDiffs[order[j]] })

				progress := -1
				for k := numMember - 1; k >= 0; k-- {

					if progress+k+1 == len(newLeaves)-1 {
						// 新しいところにひとつづつ入れる必要あり
						j := progress + 1
						i := order[j]
						(&newMembers[i]).AddK(k)
						newAreaDiffs[i] += reservations[k]
						progress = maxInt(progress, j)
						continue
					}
					ok := false
					for j := range order {
						i := order[j]
						if newAreaDiffs[i]+reservations[k] < 0 {
							(&newMembers[i]).AddK(k)
							newAreaDiffs[i] += reservations[k]
							progress = maxInt(progress, j)
							ok = true
							break
						}
					}
					if !ok {
						minOrderIndex := -1
						minValue := INF
						for j := range order {
							i := order[j]
							if newAreaDiffs[i] < minValue {
								minOrderIndex = j
								minValue = newAreaDiffs[i]
							}
						}
						// 一番ましなところへ入れる
						(&newMembers[order[minOrderIndex]]).AddK(k)
						newAreaDiffs[order[minOrderIndex]] += reservations[k]
						progress = maxInt(progress, minOrderIndex)
					}
				}

				sort.Slice(order, func(i, j int) bool { return newAreaDiffs[order[i]] > newAreaDiffs[order[j]] })

				for l := 0; l < len(newLeaves)*3; l++ {
					if newAreaDiffs[order[0]] < 0 {
						break
					}
					g1 := order[0]
					g2 := order[gameConst.Random.StrongIntn(len(order)-1)+1]
					member := newMembers[g1] | newMembers[g2]
					// 入れ替えを行う
					sa := NewSA()

					threshold := maxInt(-(newAreaDiffs[g1]+newAreaDiffs[g2]), 100) / 2 / 3
					goal := newLeaves[g1].size[0]*newLeaves[g1].size[1]*merginPermill/1000 - threshold
					sa.Init(2*1000, 300*popcount(int(member)), float64(goal), float64(maxInt(threshold, 10000)), NewSimulatedAnnealingPlan(), -INF)
					sa.Run(gameConst, reservations, goal, threshold, member, 1, start)
					member1 := sa.best.member
					member2 := member & ^member1
					newAreaDiffs[g1] = -newLeaves[g1].size[0] * newLeaves[g1].size[1] * merginPermill / 1000
					minIndex := -1
					minArea := INF
					for k := 0; k < gameConst.N; k++ {
						if member1.Contains(k) {
							newAreaDiffs[g1] += reservations[k]
							if minArea > reservations[k] {
								minIndex = k
								minArea = reservations[k]
							}
						}
					}
					newAreaDiffs[g2] = -newLeaves[g2].size[0] * newLeaves[g2].size[1] * merginPermill / 1000
					for k := 0; k < gameConst.N; k++ {
						if member2.Contains(k) {
							newAreaDiffs[g2] += reservations[k]
							if minArea > reservations[k] {
								minIndex = k
								minArea = reservations[k]
							}
						}
					}
					if member2 == 0 {
						(&member1).DeleteK(minIndex)
						newAreaDiffs[g1] -= reservations[minIndex]
						(&member2).AddK(minIndex)
						newAreaDiffs[g2] += reservations[minIndex]
					}
					if member1 == 0 {
						(&member2).DeleteK(minIndex)
						newAreaDiffs[g2] -= reservations[minIndex]
						(&member1).AddK(minIndex)
						newAreaDiffs[g1] += reservations[minIndex]
					}
					newMembers[g1] = member1
					newMembers[g2] = member2

					sort.Slice(order, func(i, j int) bool { return newAreaDiffs[order[i]] > newAreaDiffs[order[j]] })
				}
				if newAreaDiffs[order[0]] >= 0 {
					maxLeaf.Join()
					baseTree.numGroup--
					divisible[leafIndex] = false
					continue
				}
				leaves = newLeaves
				correspondence = newCorrespondence
				divisible = newDivisible
				members = newMembers
				areaDiffs = newAreaDiffs
			}
			//

			// fmt.Fprintln(os.Stderr, baseTree)

			for index := range leaves {

				leaf := leaves[index]
				m := members[index]
				bestValue := INF
				_ = bestValue
				bestTree := NewRectDivisionTree(groupArea)
				_ = bestTree
				leafSize := popcount(int(m))
				if leafSize == 1 {
					continue
				}
				for l := 0; l < 10*leafSize; l++ {
					if l > 0 {
						if time.Since(start).Microseconds() > TIME_LIMIT*95/100 {
							break
						}
					}
					tree := NewRectDivisionTree(leaf.size)
					memberStack := make([]BitSet, leafSize)
					nodeStack := make([]*RectDivisionTreeNode, leafSize)
					lenStack := 0
					memberStack[lenStack] = m
					nodeStack[lenStack] = tree.root
					lenStack++
					success := true
					for lenStack > 0 {
						member := memberStack[lenStack-1]
						node := nodeStack[lenStack-1]
						lenStack--
						// memberを二分する
						member1, member2 := getHalf(member, gameConst, reservations)

						target := 0
						iterator := NewBitIterator(uint64(member1))
						sumarea1 := 0
						for !iterator.IsEmpty() {
							k := iterator.Next()
							sumarea1 += reservations[k]
						}
						iterator = NewBitIterator(uint64(member2))
						sumarea2 := 0
						for !iterator.IsEmpty() {
							k := iterator.Next()
							sumarea2 += reservations[k]
						}
						sumarea := sumarea1 + sumarea2

						divdir := 0
						if divmode == 1 {
							divdir = 1
						} else {
							if node.size[0] < int(1250/(float64(gameConst.N)/5)) {
								if node.size[0] < node.size[1] {
									divdir = 1
								}
							}
						}

						space := (node.size[0]*node.size[1] - sumarea)
						add := int(float64(space) * (float64(popcount(int(member1))) - 0.99) / (float64(popcount(int(member))) - 1.98))
						prop := float64(sumarea1+add) / float64(node.size[0]*node.size[1])
						tmin := (sumarea1-1)/node.size[1-divdir] + 1
						p1 := sumarea1 - node.size[1-divdir]*(tmin-1)
						tmax := node.size[divdir] - ((sumarea2-1)/node.size[1-divdir] + 1)
						p2 := sumarea2 - node.size[1-divdir]*((node.size[divdir]-tmax)-1)
						if tmin > tmax {
							if p1 > p2 {
								tmax++
							} else {
								tmin--
							}
						}
						target = int(math.Round(prop*float64(node.size[divdir])) + 0.1)
						target = maxInt(target, tmin)
						target = minInt(target, tmax)

						if node.size[divdir] <= 1 {
							success = false
							break
						}
						target = maxInt(target, 1)
						target = minInt(target, node.size[divdir]-1)

						node.Divide(divdir, target, NewRectDivisionTreeNode(), NewRectDivisionTreeNode())
						tree.numGroup++
						if popcount(int(member1)) > 1 {
							memberStack[lenStack] = member1
							nodeStack[lenStack] = node.childs[0]
							lenStack++
						}
						if popcount(int(member2)) > 1 {
							memberStack[lenStack] = member2
							nodeStack[lenStack] = node.childs[1]
							lenStack++
						}
					}
					if !success {
						continue
					}

					rects, areas := tree.ToRects()
					_ = areas
					order := make([]int, len(rects))
					for i := range order {
						order[i] = i
					}
					sort.Slice(order, func(i, j int) bool { return areas[order[i]] < areas[order[j]] })

					action := NewAction(gameConst.N)
					iterator := NewBitIterator(uint64(m))
					for i := range rects {
						rindex := iterator.Next()
						k := reservationIndexes[rindex]
						action.posUL[k][0] = rects[order[i]][0][0] + groupOffset[0]
						action.posUL[k][1] = rects[order[i]][0][1] + groupOffset[1]
						action.posDR[k][0] = rects[order[i]][1][0] + groupOffset[0]
						action.posDR[k][1] = rects[order[i]][1][1] + groupOffset[1]
					}
					// value := 0
					value := 0
					// value := calcPenalty(action, reservations, d, gameConst, false)
					penalty := 0
					iterator = NewBitIterator(uint64(m))
					for i := range areas {
						rindex := iterator.Next()
						area := areas[order[i]]
						penalty += maxInt(0, area-reservations[rindex])
					}
					value += penalty * 100
					if d > 0 {
						value += calcPartitionCost(bestActions[d-1], action, gameConst)
					}
					if value < bestValue {
						bestTree = tree.Copy()
						bestValue = value
					}

				}
				// 最善のもので葉を更新
				bestTree.root.parent = leaf.parent
				bestTree.root.side = leaf.side

				*leaf = *bestTree.root
				if leaf.parent != nil {
					leaf.parent.childs[leaf.side] = leaf
				}
				baseTree.numGroup += bestTree.numGroup - 1
			}
			// fmt.Fprintln(os.Stderr, baseTree)
			rects, areas := baseTree.ToRects()
			_ = areas
			_ = rects
			order := make([]int, 0, numMember)
			for i := 0; i < numMember; i++ {
				order = append(order, i)
			}
			// fmt.Fprintln(os.Stderr, len(areas), len(order))
			sort.Slice(order, func(i, j int) bool { return areas[order[i]] < areas[order[j]] })

			iterator = NewBitIterator(1<<numMember - 1)
			for i := range rects {
				rindex := iterator.Next()
				k := reservationIndexes[rindex]
				bestActions[d].posUL[k][0] = rects[order[i]][0][0] + groupOffset[0]
				bestActions[d].posUL[k][1] = rects[order[i]][0][1] + groupOffset[1]
				bestActions[d].posDR[k][0] = rects[order[i]][1][0] + groupOffset[0]
				bestActions[d].posDR[k][1] = rects[order[i]][1][1] + groupOffset[1]
			}
			prevTree = baseTree.Copy()
		} else {
			rects, areas := prevTree.ToRects()
			_ = areas
			order := make([]int, 0, numMember)
			for i := 0; i < numMember; i++ {
				order = append(order, i)
			}
			// fmt.Fprintln(os.Stderr, len(areas), len(order))
			sort.Slice(order, func(i, j int) bool { return areas[order[i]] < areas[order[j]] })

			iterator := NewBitIterator(1<<numMember - 1)
			for i := range rects {
				rindex := iterator.Next()
				k := reservationIndexes[rindex]
				bestActions[d].posUL[k][0] = rects[order[i]][0][0] + groupOffset[0]
				bestActions[d].posUL[k][1] = rects[order[i]][0][1] + groupOffset[1]
				bestActions[d].posDR[k][0] = rects[order[i]][1][0] + groupOffset[0]
				bestActions[d].posDR[k][1] = rects[order[i]][1][1] + groupOffset[1]
			}
		}

	}
}

func getHalf(member BitSet, gameConst *GameConst, reservations []int) (BitSet, BitSet) {
	sumArea := 0
	iterator := NewBitIterator(uint64(member))
	for !iterator.IsEmpty() {
		k := iterator.Next()
		sumArea += reservations[k]
	}
	// prop := gameConst.Random.StrongFloat64()*0.4 + 0.1
	for {
		member1 := BitSet(0)
		member2 := BitSet(0)

		/*iterator := NewBitIterator(uint64(member))
		sumarea := 0
		for !iterator.IsEmpty() {
			k := iterator.Next()
			sumarea += reservations[k]
		}
		iterator = NewBitIterator(uint64(member))
		sumarea2 := 0
		for !iterator.IsEmpty() {
			k := iterator.Next()
			sumarea2 += reservations[k]
			if sumarea2*2 > sumarea {
				break
			}
			(&member2).AddK(k)
		}
		member1 = member & ^member2*/

		/*sumArea1 := 0
		for {
			iterator := NewBitIterator(uint64(member))
			index := int(math.Sqrt(gameConst.Random.StrongFloat64() * float64(popcount(int(member))*popcount(int(member)))))
			for l := 0; l < index; l++ {
				iterator.Next()
			}
			k := iterator.Next()
			if member1.Contains(k) {
				continue
			}
			(&member1).AddK(k)
			sumArea1 += reservations[k]
			if float64(sumArea1) > float64(sumArea)*prop {
				break
			}
			}*/

		iterator = NewBitIterator(uint64(member))
		l := 0
		for !iterator.IsEmpty() {
			l++
			k := iterator.Next()
			r := gameConst.Random.StrongFloat64()
			// if r  <  prop {
			if r < float64(l)/float64(popcount(int(member))+1) {
				(&member1).AddK(k)
			}
		}

		member2 = member & ^member1
		if member1 == 0 || member2 == 0 {
			continue
		}
		/*r := gameConst.Random.StrongIntn(2)
		if r == 0 {
			member1, member2 = member2, member1
		}*/
		return member1, member2
	}
	return 0, 0
}

func (runner *Runner) doGame(file string) Result {
	result := Result{}
	scanner := getScanner(FILE + "\\" + file + ".txt")
	start := time.Now()
	// 初期値設定処理

	gameConst := NewGameConst(runner)
	gameConst.SetScanner(scanner)
	// データ取得
	getInputs(gameConst)

	gameState := NewGameState(gameConst)

	// 初期値設定処理　ここまで
	solver := NewSolver(TIME_LIMIT, runner.caseIndex)
	// _ = solver
	// actions := []*Action{}
	actions, _, numGroups := solver.Solve(gameState, gameConst, -1, start)
	plus := 0
	minScore := calcScore(actions, gameState, gameConst)
	initialmin := minScore
	success := true
	for l := 0; l < 100; l++ {
		if gameConst.N > numGroups+plus && success {
			plus++
		}
		if !success {
			plus--
			plus = maxInt(-numGroups+1, plus)
		}
		actions2, _, _ := solver.Solve(gameState, gameConst, numGroups+plus, start)
		score2 := calcScore(actions2, gameState, gameConst)
		if score2 != -1 {
			success = true
			if score2 < minScore {
				minScore = score2
				actions = actions2
			}
		} else {
			success = false
		}
		if time.Since(start).Microseconds() > TIME_LIMIT*95/100 {
			break
		}
	}
	fmt.Fprintln(os.Stderr, initialmin, minScore)
	_ = start
	// アクションを出力
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		printActions(actions, gameState, gameConst, false)
	}
	score := 0
	if LOCAL_TEST {
		/*
			bests := make([]int, gameConst.D)
			bestValue := 0.0
			nows := make([]int, gameConst.D)
			members := make([]BitSet, gameConst.D)
			for i := range members {
				members[i] = 1<<gameConst.N - 1
			}
			iterators := make([]*BitIterator, 0, gameConst.D)
			for d := 0; d < gameConst.D; d++ {
				iterators = append(iterators, NewBitIterator(uint64(members[d])))
			}
			for {
				maxArea := 0
				maxIndex := 0
				_ = maxIndex
				minArea := INF
				minIndex := 0
				for d := range nows {
					if maxArea < gameConst.Reservations[d][nows[d]] {
						maxArea = gameConst.Reservations[d][nows[d]]
						maxIndex = d
					}
					if minArea > gameConst.Reservations[d][nows[d]] {
						minArea = gameConst.Reservations[d][nows[d]]
						minIndex = d
					}
				}
				value := float64(minArea) / float64(maxArea)
				if value > bestValue {
					bestValue = value
					for i := range bests {
						bests[i] = nows[i]
					}
				}
				// 最も小さいものを一つ大きいものに変える
				if iterators[minIndex].IsEmpty() {
					break
				}
				nows[minIndex] = iterators[minIndex].Next()

			}
			for i := range bests {
				fmt.Fprintln(os.Stderr, gameConst.Reservations[i][bests[i]])
			}
		*/

		/*sum := 0

		for k := 0; k < gameConst.N; k++ {
			maxArea := 0
			for d := 0; d < gameConst.D; d++ {
				maxArea = maxInt(maxArea, gameConst.Reservations[d][k])
			}
			sum += maxArea
		}*/
		// score = 0
		// if sum <= gameConst.W*gameConst.W*98/100 {
		// 	score = 1
		// }
		// partitionコスト以外を算出
		score = calcScore(actions, gameState, gameConst)

	}

	result.score = score

	return result
}

func calcScore(actions []*Action, gameState *GameState, gameConst *GameConst) int {
	score := INF
	valid := true
	for d := 0; d < gameConst.D; d++ {
		v := actions[d].isValid(gameState, gameConst)
		if !v {
			valid = false
			break
		}
	}
	if !valid {
		// fmt.Fprintln(os.Stderr, "the Action is invalid")
		return -1
	}
	score = 1
	// 面積不足によるpenaltyを計算
	for d := 0; d < gameConst.D; d++ {
		score += calcPenalty(actions[d], gameConst.Reservations[d], d, gameConst, true)
	}
	// Partitionによるコストを計算
	for d := 1; d < gameConst.D; d++ {
		score += calcPartitionCost(actions[d-1], actions[d], gameConst)
	}

	return score
}

func calcPartitionCost(prevAction *Action, action *Action, gameConst *GameConst) int {
	partitionCost := 0
	prevlines := Lines{}
	lines := Lines{}
	// 水平方向・垂直方向に分けて計算する
	for dir := 0; dir < 2; dir++ {
		prevlines.Init(2*gameConst.N + 2)
		lines.Init(2*gameConst.N + 2)
		prevlines.addLine(0, 0, gameConst.W)
		prevlines.addLine(gameConst.W, 0, gameConst.W)
		lines.addLine(0, 0, gameConst.W)
		lines.addLine(gameConst.W, 0, gameConst.W)
		// 長方形の辺を登録
		for k := 0; k < gameConst.N; k++ {
			prevlines.addLine(prevAction.posUL[k][dir], prevAction.posUL[k][1-dir], prevAction.posDR[k][1-dir])
			prevlines.addLine(prevAction.posDR[k][dir], prevAction.posUL[k][1-dir], prevAction.posDR[k][1-dir])
		}
		for k := 0; k < gameConst.N; k++ {
			lines.addLine(action.posUL[k][dir], action.posUL[k][1-dir], action.posDR[k][1-dir])
			lines.addLine(action.posDR[k][dir], action.posUL[k][1-dir], action.posDR[k][1-dir])
		}
		prevlines.sort()
		lines.sort()
		// lineの変更を数える
		numChanged := 0
		prevOrderIndex := 0
		orderIndex := 0
		progressParam := 0
		progressStart := 0
		for progressParam < gameConst.W {
			param1 := 0
			param2 := 0
			start1 := 0
			start2 := 0
			end1 := 0
			end2 := 0
			for {
				param1 = prevlines.param1[prevlines.order[prevOrderIndex]]
				start1 = prevlines.start[prevlines.order[prevOrderIndex]]
				end1 = prevlines.end[prevlines.order[prevOrderIndex]]
				if param1 == progressParam {
					if end1 <= progressStart {
						prevOrderIndex++
						continue
					}
					start1 = maxInt(start1, progressStart)
				}
				break
			}
			for {
				param2 = lines.param1[lines.order[orderIndex]]
				start2 = lines.start[lines.order[orderIndex]]
				end2 = lines.end[lines.order[orderIndex]]
				if param2 == progressParam {
					if end2 <= progressStart {
						orderIndex++
						continue
					}
					start2 = maxInt(start2, progressStart)
				}
				break
			}
			// fmt.Fprintln(os.Stderr, param1)
			// fmt.Fprintln(os.Stderr, start1, end1)
			// fmt.Fprintln(os.Stderr, param2)
			// fmt.Fprintln(os.Stderr, start2, end2)
			// fmt.Fprintln(os.Stderr, "")

			if param1 < param2 {
				// 列違い
				numChanged += end1 - start1
				progressParam = param1
				progressStart = end1
			}
			if param1 > param2 {
				// 列違い
				numChanged += end2 - start2
				progressParam = param2
				progressStart = end2
			}
			if param1 == param2 {
				// 同じ列の場合
				if end1 < start2 {
					numChanged += end1 - start1
					progressParam = param1
					progressStart = end1
				} else if end2 < start1 {
					numChanged += end2 - start2
					progressParam = param2
					progressStart = end2
				} else {
					// 重なりを考慮することになるのでちょっと面倒なパターン
					minStart := minInt(start1, start2)
					maxStart := maxInt(start1, start2)
					minEnd := minInt(end1, end2)
					numChanged += maxStart - minStart
					progressParam = param1
					progressStart = minEnd
				}
			}
		}
		partitionCost += numChanged
	}

	return partitionCost
}
func calcPenalty(action *Action, reservations []int, day int, gameConst *GameConst, judge bool) int {
	penalty := 0
	for k := 0; k < gameConst.N; k++ {
		area := 1
		for i := 0; i < 2; i++ {
			area *= action.posDR[k][i] - action.posUL[k][i]
		}
		if area < reservations[k] {
			penalty += reservations[k] - area
		}
	}
	if penalty > 0 {
		sumarea := 0
		for i := range reservations {
			sumarea += reservations[i]
		}
		// if judge {
		// 	fmt.Fprintln(os.Stderr, "Division failed. ", gameConst.N, day, sumarea, penalty)
		// }

	}
	penalty *= 100
	return penalty
}

func (action *Action) isValid(gameState *GameState, gameConst *GameConst) bool {
	for k := 0; k < gameConst.N; k++ {
		for i := 0; i < 2; i++ {
			if action.posUL[k][i] >= action.posDR[k][i] {
				// 座標が逆転しており長方形が形成できていない
				return false
			}
		}
	}
	for k := 0; k < gameConst.N; k++ {
		for i := 0; i < 2; i++ {
			if action.posUL[k][i] < 0 {
				// out of field
				return false
			}
			if action.posDR[k][i] > gameConst.W {
				// out of field
				return false
			}
		}
	}
	for k := 0; k < gameConst.N; k++ {
		for k2 := 0; k2 < gameConst.N; k2++ {
			if k == k2 {
				continue
			}
			ul := [2]int{}
			dr := [2]int{}
			for i := 0; i < 2; i++ {
				ul[i] = maxInt(action.posUL[k][i], action.posUL[k2][i])
				dr[i] = minInt(action.posDR[k][i], action.posDR[k2][i])
			}
			if ul[0] < dr[0] && ul[1] < dr[1] {
				// conflict
				return false
			}
		}
	}
	return true
}

type Lines struct {
	param1 []int // 列or行
	start  []int
	end    []int
	order  []int
}

func (lines *Lines) Init(cap int) {
	lines.param1 = make([]int, 0, cap)
	lines.start = make([]int, 0, cap)
	lines.end = make([]int, 0, cap)
	lines.order = make([]int, 0, cap)
}

func (lines *Lines) addLine(param1 int, start int, end int) {
	lines.param1 = append(lines.param1, param1)
	lines.start = append(lines.start, start)
	lines.end = append(lines.end, end)
	lines.order = append(lines.order, len(lines.order))
}

func (lines *Lines) sort() {
	sort.Slice(lines.order, func(i, j int) bool {
		if lines.param1[lines.order[i]] < lines.param1[lines.order[j]] {
			return true
		} else if lines.param1[lines.order[i]] == lines.param1[lines.order[j]] {
			if lines.start[lines.order[i]] < lines.start[lines.order[j]] {
				return true
			} else if lines.start[lines.order[i]] == lines.start[lines.order[j]] {
				if lines.end[lines.order[i]] > lines.end[lines.order[j]] {
					return true
				}
			}
		}
		return false
	})
}

type RectDivisionTree struct {
	root     *RectDivisionTreeNode
	numGroup int
}

func NewRectDivisionTree(size [2]int) *RectDivisionTree {
	tree := RectDivisionTree{}
	root := NewRectDivisionTreeNode()
	root.SetSize(size)
	tree.root = root
	tree.numGroup = 1
	return &tree
}

func (tree *RectDivisionTree) Copy() *RectDivisionTree {
	newTree := NewRectDivisionTree(tree.root.size)

	// 元の木を見て同じように分割する
	stack := make([]*RectDivisionTreeNode, tree.numGroup)
	stack2 := make([]*RectDivisionTreeNode, tree.numGroup)
	lenStack := 0
	stack[lenStack] = tree.root
	stack2[lenStack] = newTree.root
	lenStack++
	for lenStack > 0 {
		node := stack[lenStack-1]
		node2 := stack2[lenStack-1]
		lenStack--
		if node.divdir != -1 {
			node2.Divide(node.divdir, node.childs[0].size[node.divdir], NewRectDivisionTreeNode(), NewRectDivisionTreeNode())
			newTree.numGroup++
			stack[lenStack] = node.childs[0]
			stack2[lenStack] = node2.childs[0]
			lenStack++

			stack[lenStack] = node.childs[1]
			stack2[lenStack] = node2.childs[1]
			lenStack++
		}
	}
	return newTree
}

func (tree *RectDivisionTree) randomlyDivide(gameConst *GameConst) {
	targetNode := tree.root
	for {
		targetNode = tree.root
		for targetNode.divdir != -1 {
			r := gameConst.Random.StrongIntn(2)
			targetNode = targetNode.childs[r]
		}
		if targetNode.size[0] > 1 || targetNode.size[1] > 1 {
			break
		}
	}
	divdir := 0
	if targetNode.size[0] < targetNode.size[1] {
		divdir = 1
	}
	target := gameConst.Random.StrongIntn(targetNode.size[divdir]-1) + 1
	targetNode.Divide(divdir, target, NewRectDivisionTreeNode(), NewRectDivisionTreeNode())
	tree.numGroup++
}

func (tree *RectDivisionTree) randomlyJoin(gameConst *GameConst) {
	targetNode := tree.root
	for {
		targetNode = tree.root
		for targetNode.divdir != -1 {
			r := gameConst.Random.StrongIntn(2)
			targetNode = targetNode.childs[r]
		}
		targetNode = targetNode.parent
		break
	}
	targetNode.Join()
	tree.numGroup--
}

func (tree *RectDivisionTree) ToRects() ([][2][2]int, []int) {
	rects := make([][2][2]int, 0, tree.numGroup)
	areas := make([]int, 0, tree.numGroup)
	stack := make([]*RectDivisionTreeNode, tree.numGroup)
	offsets1 := make([]int, tree.numGroup)
	offsets2 := make([]int, tree.numGroup)
	lenStack := 0
	stack[lenStack] = tree.root
	offsets1[lenStack] = 0
	offsets2[lenStack] = 0
	lenStack++
	for lenStack > 0 {
		node := stack[lenStack-1]
		offset1 := offsets1[lenStack-1]
		offset2 := offsets2[lenStack-1]
		lenStack--
		if node.divdir != -1 {
			stack[lenStack] = node.childs[0]
			offsets1[lenStack] = offset1
			offsets2[lenStack] = offset2
			lenStack++

			if node.divdir == 0 {
				offset1 += node.childs[0].size[0]
			} else {
				offset2 += node.childs[0].size[1]
			}
			stack[lenStack] = node.childs[1]
			offsets1[lenStack] = offset1
			offsets2[lenStack] = offset2
			lenStack++
		} else {
			rects = append(rects, [2][2]int{{offset1, offset2}, {offset1 + node.size[0], offset2 + node.size[1]}})
			areas = append(areas, node.size[0]*node.size[1])
		}
	}
	return rects, areas
}

type RectDivisionTreeNode struct {
	parent *RectDivisionTreeNode
	side   int
	childs [2]*RectDivisionTreeNode
	size   [2]int
	divdir int // 0:上下分割 1:左右分割
}

func NewRectDivisionTreeNode() *RectDivisionTreeNode {
	node := RectDivisionTreeNode{}
	node.parent = nil
	node.childs[0] = nil
	node.childs[1] = nil
	node.divdir = -1
	return &node
}

func (node *RectDivisionTreeNode) Init() {
	node.parent = nil
	node.childs[0] = nil
	node.childs[1] = nil
	node.size[0] = 0
	node.size[1] = 0
	node.side = 0
	node.divdir = -1
}

func (node *RectDivisionTreeNode) SetSize(size [2]int) {
	node.size = size
}

func (node *RectDivisionTreeNode) Divide(divdir int, target int, child1 *RectDivisionTreeNode, child2 *RectDivisionTreeNode) {
	node.divdir = divdir
	child1.Init()
	child2.Init()
	size1 := node.size
	size2 := node.size
	size1[divdir] = target
	size2[divdir] = size2[divdir] - target
	child1.SetSize(size1)
	child2.SetSize(size2)
	child1.parent = node
	child2.parent = node
	child1.side = 0
	child2.side = 1
	node.childs[0] = child1
	node.childs[1] = child2
}

func (node *RectDivisionTreeNode) Join() {
	node.childs[0] = nil
	node.childs[1] = nil
	node.divdir = -1
}

type GroupDivisionTree struct {
	root     *GroupDivisionTreeNode
	numGroup int
}

func NewGroupDivisionTree(size [2]int) *GroupDivisionTree {
	tree := GroupDivisionTree{}
	root := NewGroupDivisionTreeNode()
	root.SetSize(size)
	tree.root = root
	tree.numGroup = 1
	return &tree
}

func (tree *GroupDivisionTree) Copy() *GroupDivisionTree {
	newTree := NewGroupDivisionTree(tree.root.size)

	// 元の木を見て同じように分割する
	stack := make([]*GroupDivisionTreeNode, tree.numGroup)
	stack2 := make([]*GroupDivisionTreeNode, tree.numGroup)
	lenStack := 0
	stack[lenStack] = tree.root
	stack2[lenStack] = newTree.root
	lenStack++
	for lenStack > 0 {
		node := stack[lenStack-1]
		node2 := stack2[lenStack-1]
		lenStack--
		if node.divdir != -1 {
			node2.Divide(node.divdir, node.childs[0].size[node.divdir], NewGroupDivisionTreeNode(), NewGroupDivisionTreeNode())
			newTree.numGroup++
			stack[lenStack] = node.childs[0]
			stack2[lenStack] = node2.childs[0]
			lenStack++

			stack[lenStack] = node.childs[1]
			stack2[lenStack] = node2.childs[1]
			lenStack++
		}
	}
	return newTree
}

func (tree *GroupDivisionTree) randomlyDivide(gameConst *GameConst) {
	targetNode := tree.root
	for {
		targetNode = tree.root
		for targetNode.divdir != -1 {
			r := gameConst.Random.StrongIntn(2)
			targetNode = targetNode.childs[r]
		}
		if targetNode.size[0] > 1 || targetNode.size[1] > 1 {
			break
		}
	}
	divdir := 0
	if targetNode.size[0] < targetNode.size[1] {
		divdir = 1
	}
	target := gameConst.Random.StrongIntn(targetNode.size[divdir]-1) + 1
	targetNode.Divide(divdir, target, NewGroupDivisionTreeNode(), NewGroupDivisionTreeNode())
	tree.numGroup++
}

type GroupDivisionTreeNode struct {
	parent *GroupDivisionTreeNode
	childs [2]*GroupDivisionTreeNode
	size   [2]int
	divdir int // 0:上下分割 1:左右分割
}

func NewGroupDivisionTreeNode() *GroupDivisionTreeNode {
	node := GroupDivisionTreeNode{}
	node.parent = nil
	node.childs[0] = nil
	node.childs[1] = nil
	node.divdir = -1
	return &node
}

func (node *GroupDivisionTreeNode) Init() {
	node.parent = nil
	node.childs[0] = nil
	node.childs[1] = nil
	node.size[0] = 0
	node.size[1] = 0
	node.divdir = -1
}

func (node *GroupDivisionTreeNode) SetSize(size [2]int) {
	node.size = size
}

func (node *GroupDivisionTreeNode) Divide(divdir int, target int, child1 *GroupDivisionTreeNode, child2 *GroupDivisionTreeNode) {
	node.divdir = divdir
	child1.Init()
	child2.Init()
	size1 := node.size
	size2 := node.size
	size1[divdir] = target
	size2[divdir] = size2[divdir] - target
	child1.SetSize(size1)
	child2.SetSize(size2)
	child1.parent = node
	child2.parent = node
	node.childs[0] = child1
	node.childs[1] = child2
}

func (node *GroupDivisionTreeNode) Join() {
	node.childs[0] = nil
	node.childs[1] = nil
	node.divdir = -1
}

type BitIterator struct {
	BitData uint64
}

func NewBitIterator(bitData uint64) *BitIterator {
	b := BitIterator{}
	b.BitData = bitData
	return &b
}

func (b *BitIterator) Init(bitData uint64) {
	b.BitData = bitData
}

func (b *BitIterator) IsEmpty() bool {
	return b.BitData == 0
}

func (b *BitIterator) Next() int {
	// この形で書くことを想定
	// for !b.IsEmpty() {
	// 	i:= b.Next()
	// }
	if b.BitData == 0 {
		return -1
	}
	index := bits.TrailingZeros(uint(b.BitData))
	b.BitData &= b.BitData - 1
	// trailingzerosを用いる
	return index
}

type SimulatedAnnealingPlan struct {
	member  BitSet
	sumarea int
}

func NewSimulatedAnnealingPlan() *SimulatedAnnealingPlan {
	return &SimulatedAnnealingPlan{}
}

func (plan *SimulatedAnnealingPlan) Copy() *SimulatedAnnealingPlan {
	newP := NewSimulatedAnnealingPlan()
	newP.Load(plan)
	return newP
}
func (plan *SimulatedAnnealingPlan) Load(plan2 *SimulatedAnnealingPlan) {
	plan.member = plan2.member
	plan.sumarea = plan2.sumarea
}

func (plan *SimulatedAnnealingPlan) ApplyAction(gameConst *GameConst, reservations []int, action int) {
	if plan.member.Contains(action) {
		plan.member.DeleteK(action)
		plan.sumarea -= reservations[action]
	} else {
		plan.member.AddK(action)
		plan.sumarea += reservations[action]
	}
}

func (plan *SimulatedAnnealingPlan) RevertAction(gameConst *GameConst, reservations []int, action int) {
	plan.ApplyAction(gameConst, reservations, action)
}

func (plan *SimulatedAnnealingPlan) Evaluate(gameConst *GameConst, goal int, threshold int) float64 {
	diff := absInt(plan.sumarea - goal)
	value := 0.0
	if diff > threshold {
		value -= float64(diff)
	} else {
		value -= float64(diff) / float64(goal)
		value -= float64(popcount(int(plan.member))) * 0.2
	}
	return value
}

func (plan *SimulatedAnnealingPlan) EvaluateDiff(gameConst *GameConst, action int) float64 {
	return 0.0
}

type SimulatedAnnealing struct {
	startTime             time.Time
	duration              int // 焼きなまし実行時間　microsecond
	loopLimit             int
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int                     // loopの回った回数
	valid          int                     // validな解を得た回数
	accepted       int                     // 採用された回数
	temp           float64                 // 現在の温度
	reciprocaltemp float64                 // 温度の逆数
	best           *SimulatedAnnealingPlan // これまでの最善解
	bestScore      float64                 // 最善解のスコア
	now            *SimulatedAnnealingPlan // 現在の遷移元
	nowScore       float64                 // 現在の遷移元のスコア
}

func NewSA() *SimulatedAnnealing {
	sa := SimulatedAnnealing{}
	return &sa
}

func (sa *SimulatedAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	p := sa.getAcceptRateFromDiff(diff, bestScore)
	return p
}
func (sa *SimulatedAnnealing) getAcceptRateFromDiff(diff float64, bestScore float64) float64 {
	p := math.Exp(diff * sa.reciprocaltemp)
	return p
}

func (sa *SimulatedAnnealing) isBest(score float64, bestScore float64) bool {
	return score > bestScore

}

func (sa *SimulatedAnnealing) Init(duration int, loopLimit int, startTemp float64, endTemp float64, best *SimulatedAnnealingPlan, bestScore float64) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.loopLimit = loopLimit
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy()
	sa.nowScore = bestScore

}

func (sa *SimulatedAnnealing) Run(gameConst *GameConst, reservations []int, goal int, threshold int, member BitSet, timeType int, start time.Time) {
	prop := 0.0
	sa.updateTempIntervalExp = 5
	candidates := make([]int, 0, popcount(int(member)))
	sa.bestScore = sa.best.Evaluate(gameConst, goal, threshold)
	sa.nowScore = sa.now.Evaluate(gameConst, goal, threshold)
	for i := 0; i < gameConst.N; i++ {
		if member.Contains(i) {
			candidates = append(candidates, i)
		}
	}
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			if time.Since(start).Microseconds() > TIME_LIMIT*95/100 {
				break
			}
			if timeType == 0 {
				prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			} else {
				prop = float64(sa.sumIter) / float64(sa.loopLimit)
			}
			if prop >= 1 {
				// time up
				break
			}
			if DEBUG {
				// fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sa.sumIter, sa.bestScore)
			}
			// 指数ver.
			sa.temp = math.Pow(sa.startTemp, 1-prop) * math.Pow(sa.endTemp, prop)
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			// 最善解に戻す
			sa.now = sa.best.Copy()
			sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		action := candidates[gameConst.Random.StrongIntn(len(candidates))]
		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.StrongFloat64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff

		sa.now.ApplyAction(gameConst, reservations, action)
		sa.valid++
		score := sa.now.Evaluate(gameConst, goal, threshold)
		diff := score - sa.nowScore
		// p := sa.getAcceptRate(score, sa.nowScore)
		// r := gameConst.Random.StrongFloat64()
		// if p >= r {
		r := gameConst.Random.LogFloat64()
		if sa.temp*r < diff {
			// 採用
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				sa.best = sa.now.Copy()
				sa.bestScore = sa.nowScore
			}
		} else {
			// rollback（nowを戻す）
			sa.now.RevertAction(gameConst, reservations, action)
		}
	}
	// fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	// fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	// fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	// fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	// fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}

type BaseDivPlan struct {
	group   [][]int    // group[d][k]
	sumarea [][]int    // sumarea[d][gid]
	members [][]BitSet // members[d][gid]
	maxArea []int      // maxArea[gid]
	sumPart []int      // sumPart[gid]
}

func NewBaseDivPlan(d int, k int, numGroups int) *BaseDivPlan {
	plan := BaseDivPlan{}
	plan.group = make([][]int, d)
	for i := range plan.group {
		plan.group[i] = make([]int, k)
	}
	plan.sumarea = make([][]int, d)
	for i := range plan.sumarea {
		plan.sumarea[i] = make([]int, numGroups)
	}
	plan.members = make([][]BitSet, d)
	for i := range plan.members {
		plan.members[i] = make([]BitSet, numGroups)
	}
	plan.maxArea = make([]int, numGroups)
	plan.sumPart = make([]int, numGroups)
	return &plan
}

func (plan *BaseDivPlan) SetReasonableGroup(numGroup int, gameConst *GameConst) {
	for d := range plan.sumarea {
		for g := range plan.sumarea[d] {
			plan.sumarea[d][g] = 0
		}
	}
	for d := range plan.group {
		for k := range plan.group[d] {
			/*minArea := INF
			/*index := -1
			for g := 0; g < numGroup; g++ {
				if plan.sumarea[d][g] < minArea {
					index = g
					minArea = plan.sumarea[d][g]
				}

			}
			g := index*/
			g := (gameConst.N - 1 - k) % numGroup
			// g := numGroup * (gameConst.N - 1 - k) / gameConst.N
			plan.group[d][k] = g
			plan.sumPart[g]++
			plan.sumarea[d][plan.group[d][k]] += gameConst.Reservations[d][k]
			(&(plan.members[d][plan.group[d][k]])).AddK(k)
		}
	}

	for g := range plan.maxArea {
		maxArea := 0
		for d := range plan.sumarea {
			maxArea = maxInt(maxArea, plan.sumarea[d][g])
		}
		plan.maxArea[g] = maxArea
	}

	for g := range plan.sumPart {
		plan.sumPart[g] -= len(plan.group)
	}
}

func (plan *BaseDivPlan) Copy() *BaseDivPlan {
	newP := NewBaseDivPlan(len(plan.group), len(plan.group[0]), len(plan.sumarea[0]))
	newP.Load(plan)
	return newP
}
func (plan *BaseDivPlan) Load(plan2 *BaseDivPlan) {
	for d := range plan.group {
		for k := range plan.group[d] {
			plan.group[d][k] = plan2.group[d][k]
		}
	}
	for d := range plan.sumarea {
		for g := range plan.sumarea[d] {
			plan.sumarea[d][g] = plan2.sumarea[d][g]
		}
	}
	for d := range plan.members {
		for g := range plan.members[d] {
			plan.members[d][g] = plan2.members[d][g]
		}
	}
	for g := range plan.maxArea {
		plan.maxArea[g] = plan2.maxArea[g]
	}
	for g := range plan.sumPart {
		plan.sumPart[g] = plan2.sumPart[g]
	}
}

func (plan *BaseDivPlan) ApplyMove(gameConst *GameConst, day int, k1 int, prevg int, newg int) {
	update := plan.sumarea[day][prevg] == plan.maxArea[prevg]
	plan.sumarea[day][prevg] -= gameConst.Reservations[day][k1]
	(&(plan.members[day][prevg])).DeleteK(k1)
	plan.group[day][k1] = newg
	plan.sumarea[day][newg] += gameConst.Reservations[day][k1]
	(&(plan.members[day][newg])).AddK(k1)

	plan.sumPart[prevg]--
	plan.sumPart[newg]++
	if update {
		maxArea := 0
		for d := range plan.sumarea {
			maxArea = maxInt(maxArea, plan.sumarea[d][prevg])
		}
		plan.maxArea[prevg] = maxArea
	}

	plan.maxArea[newg] = maxInt(plan.maxArea[newg], plan.sumarea[day][newg])

}

func (plan *BaseDivPlan) RevertMove(gameConst *GameConst, day int, k1 int, prevg int, newg int) {
	plan.ApplyMove(gameConst, day, k1, newg, prevg)
}

func (plan *BaseDivPlan) ApplySwap(gameConst *GameConst, day int, k1 int, k2 int) {
	g1 := plan.group[day][k1]
	g2 := plan.group[day][k2]
	plan.ApplyMove(gameConst, day, k1, g1, g2)
	plan.ApplyMove(gameConst, day, k2, g2, g1)

}

func (plan *BaseDivPlan) RevertSwap(gameConst *GameConst, day int, k1 int, k2 int) {
	plan.ApplySwap(gameConst, day, k1, k2)
}

func (plan *BaseDivPlan) Evaluate(gameConst *GameConst, numGroups int) float64 {
	value := 0.0
	value2 := 0.0
	value3 := 0.0
	for g := 0; g < numGroups; g++ {
		maxArea := plan.maxArea[g]
		numpart := plan.sumPart[g]
		// value += float64(maxArea*maxArea) * 0.000001 * 0.001
		value -= float64(maxArea)
		// value3 -= float64(maxArea*maxArea) * 0.000001 * 0.01
		if numGroups*2 > gameConst.N {
			value2 -= float64(maxArea) * 0.001 * float64(numpart) * 100
		} else {
			value2 -= float64(maxArea) * 0.001 * float64(numpart) * 1
		}
	}
	value = math.Min(value, float64(-980000))
	value += value3
	value += value2
	return value
}

func (plan *BaseDivPlan) EvaluateEasyDiff(gameConst *GameConst, action int) float64 {
	return 0.0
}

type BaseDivAnnealing struct {
	startTime             time.Time
	duration              int // 焼きなまし実行時間　microsecond
	loopLimit             int
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int          // loopの回った回数
	valid          int          // validな解を得た回数
	accepted       int          // 採用された回数
	temp           float64      // 現在の温度
	reciprocaltemp float64      // 温度の逆数
	best           *BaseDivPlan // これまでの最善解
	bestScore      float64      // 最善解のスコア
	now            *BaseDivPlan // 現在の遷移元
	nowScore       float64      // 現在の遷移元のスコア
}

func NewBDA() *BaseDivAnnealing {
	sa := BaseDivAnnealing{}
	return &sa
}

func (sa *BaseDivAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	p := sa.getAcceptRateFromDiff(diff, bestScore)
	return p
}
func (sa *BaseDivAnnealing) getAcceptRateFromDiff(diff float64, bestScore float64) float64 {
	p := math.Exp(diff * sa.reciprocaltemp)
	return p
}

func (sa *BaseDivAnnealing) isBest(score float64, bestScore float64) bool {
	return score > bestScore

}

func (sa *BaseDivAnnealing) Init(duration int, loopLimit int, startTemp float64, endTemp float64, best *BaseDivPlan, bestScore float64) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.loopLimit = loopLimit
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy()
	sa.nowScore = bestScore

}

func (sa *BaseDivAnnealing) Run(gameConst *GameConst, numGroups int, timeType int, start time.Time) {
	prop := 0.0
	sa.updateTempIntervalExp = 11

	sa.bestScore = sa.best.Evaluate(gameConst, numGroups)
	sa.nowScore = sa.now.Evaluate(gameConst, numGroups)
	sumMaxArea := 0
	for g := range sa.best.maxArea {
		sumMaxArea += sa.best.maxArea[g]
	}
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			if time.Since(start).Microseconds() > TIME_LIMIT*95/100 {
				break
			}
			if timeType == 0 {
				prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			} else {
				prop = float64(sa.sumIter) / float64(sa.loopLimit)
			}
			if prop >= 1 {
				// time up
				break
			}
			if DEBUG {
				// fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sa.sumIter, sa.bestScore)
			}
			// 指数ver.
			sa.temp = math.Pow(sa.startTemp, 1-prop) * math.Pow(sa.endTemp, prop)
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			// 最善解に戻す
			// sa.now.Load(sa.best)
			// sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		actiontype := 0
		r2 := gameConst.Random.StrongIntn(2)
		// r2 = 1
		day := -1
		k1 := -1
		prevg := -1
		if r2 == 0 {
			prevg = gameConst.Random.StrongIntn(numGroups)
			// k1,dayを改善する可能性のあるものにする
			for d := 0; d < gameConst.D; d++ {
				if sa.now.maxArea[prevg] == sa.now.sumarea[d][prevg] {
					day = d
					break
				}
			}
			kIndex := gameConst.Random.StrongIntn(popcount(int(sa.now.members[day][prevg])))
			iterator := NewBitIterator(uint64(sa.now.members[day][prevg]))
			for i := 0; i < kIndex; i++ {
				iterator.Next()
			}
			k1 = iterator.Next()
		} else {
			day = gameConst.Random.StrongIntn(gameConst.D)
			k1 = gameConst.Random.StrongIntn(gameConst.N)
			prevg = sa.now.group[day][k1]
		}
		// k1 := gameConst.Random.StrongIntn(gameConst.N)
		if sa.now.sumarea[day][prevg] != gameConst.Reservations[day][k1] {
			// move可能
			r := gameConst.Random.StrongFloat64()
			if r < 0.5 {
				actiontype = 1
			}
		}
		k2 := k1
		newg := prevg
		if actiontype == 0 {
			for k2 == k1 {
				k2 = gameConst.Random.StrongIntn(gameConst.N)
			}
			if sa.now.group[day][k1] == sa.now.group[day][k2] {
				continue
			}
			newg = sa.now.group[day][k2]

		} else {
			for newg == prevg {
				newg = gameConst.Random.StrongIntn(numGroups)
			}
		}

		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.StrongFloat64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff
		diffArea := 0
		diffArea -= sa.now.maxArea[prevg]
		diffArea -= sa.now.maxArea[newg]
		if actiontype == 0 {
			sa.now.ApplySwap(gameConst, day, k1, k2)
		} else {

			sa.now.ApplyMove(gameConst, day, k1, prevg, newg)
		}
		diffArea += sa.now.maxArea[prevg]
		diffArea += sa.now.maxArea[newg]
		// if sa.sumIter%10000 == 0 {
		// 	fmt.Fprintln(os.Stderr, day, k1, k2)
		// }
		sa.valid++
		score := sa.now.Evaluate(gameConst, numGroups)
		diff := score - sa.nowScore
		// p := sa.getAcceptRate(score, sa.nowScore)
		// r := gameConst.Random.StrongFloat64()
		// if p >= r {
		r := gameConst.Random.LogFloat64()

		if sa.temp*r < diff {
			// 採用
			sumMaxArea += diffArea
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				// sa.best = sa.now.Copy()
				sa.best.Load(sa.now)
				sa.bestScore = sa.nowScore
			}
		} else {
			// rollback（nowを戻す）
			if actiontype == 0 {
				sa.now.RevertSwap(gameConst, day, k1, k2)
			} else {

				sa.now.RevertMove(gameConst, day, k1, prevg, newg)
			}
		}
	}
	// fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	// fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	// fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	// fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	// fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)

}

type PartDivPlan struct {
	partition     [][][]int // partition[d][g] 最後の余白も入れてもよいかも？
	lenPartition  [][]int   // lenPartition[d][g]
	penalties     []int     // penalties [d]
	partitionCost int
	numRect       []int // numRect[d]
}

func NewPartDivPlan(gameConst *GameConst, numGroups int, day int) *PartDivPlan {
	plan := PartDivPlan{}
	plan.partition = make([][][]int, day)
	plan.lenPartition = make([][]int, day)

	for d := range plan.partition {
		plan.partition[d] = make([][]int, numGroups)
		plan.lenPartition[d] = make([]int, numGroups)
		for g := range plan.partition[d] {
			plan.partition[d][g] = make([]int, 0, 3)
			plan.partition[d][g] = append(plan.partition[d][g], 0)
			plan.partition[d][g] = append(plan.partition[d][g], gameConst.W)
			plan.lenPartition[d][g] = 2
		}
	}

	plan.penalties = make([]int, day)
	plan.partitionCost = 0
	plan.numRect = make([]int, day)
	for d := range plan.numRect {
		plan.numRect[d] = numGroups
	}
	return &plan
}

func (plan *PartDivPlan) Import(gameConst *GameConst, heights []int, actions []*Action) {
	intervals := []int{}
	// fmt.Fprintln(os.Stderr, heights)
	for i := range heights {
		interval := gameConst.W - heights[i]
		if i > 0 {
			interval -= gameConst.W - heights[i-1]
		}
		intervals = append(intervals, interval)
	}
	for d := range actions {
		action := actions[d]
		for k := range action.posUL {
			height := action.posUL[k][0]
			hindex := 0
			for {
				if heights[hindex] == height {
					break
				}
				hindex++
			}
			point := action.posUL[k][1]
			if point == 0 {
				continue
			}
			plan.SetPartition(gameConst, intervals, d, hindex, point)
		}
	}
}

func (plan *PartDivPlan) InitialPenalty(gameConst *GameConst, intervals []int) {
	for d := range plan.penalties {
		plan.penalties[d] = plan.CalcPenalty(gameConst, intervals, d)
	}
}

func (plan *PartDivPlan) Copy(gameConst *GameConst) *PartDivPlan {
	newP := NewPartDivPlan(gameConst, len(plan.partition[0]), len(plan.partition))
	newP.Load(plan)
	return newP
}
func (plan *PartDivPlan) Load(plan2 *PartDivPlan) {

	for d := range plan.partition {
		for g := range plan.partition[d] {
			for i := 0; i < plan2.lenPartition[d][g]; i++ {
				if i < len(plan.partition[d][g]) {
					plan.partition[d][g][i] = plan2.partition[d][g][i]
				} else {
					plan.partition[d][g] = append(plan.partition[d][g], plan2.partition[d][g][i])
				}
			}
			plan.lenPartition[d][g] = plan2.lenPartition[d][g]
		}
	}
	for d := range plan.penalties {
		plan.penalties[d] = plan2.penalties[d]
	}
	for d := range plan.numRect {
		plan.numRect[d] = plan2.numRect[d]
	}
	plan.partitionCost = plan2.partitionCost
}

func (plan *PartDivPlan) SetPartition(gameConst *GameConst, intervals []int, day int, g int, point int) {
	plan.numRect[day]++
	if plan.lenPartition[day][g] == len(plan.partition[day][g]) {
		plan.partition[day][g] = append(plan.partition[day][g], gameConst.W)
	} else {
		plan.partition[day][g][plan.lenPartition[day][g]] = gameConst.W
	}
	plan.lenPartition[day][g]++
	index := 0
	for i := plan.lenPartition[day][g] - 1; i > 0; i-- {
		index = i
		if plan.partition[day][g][i-1] < point {
			break
		}
		plan.partition[day][g][i] = plan.partition[day][g][i-1]
	}
	plan.partition[day][g][index] = point
	if day > 0 {
		if plan.Contains(day-1, g, point) {
			plan.partitionCost -= intervals[g]
		} else {
			plan.partitionCost += intervals[g]
		}
	}
	if day < gameConst.D-1 {
		if plan.Contains(day+1, g, point) {
			plan.partitionCost -= intervals[g]
		} else {
			plan.partitionCost += intervals[g]
		}
	}
}

func (plan *PartDivPlan) RemovePartition(gameConst *GameConst, intervals []int, day int, g int, point int) {
	plan.numRect[day]--
	prev := 0
	for i := plan.lenPartition[day][g] - 1; i > 0; i-- {
		if plan.partition[day][g][i] < point {
			break
		}
		now := plan.partition[day][g][i]
		plan.partition[day][g][i] = prev
		prev = now
	}
	plan.lenPartition[day][g]--

	// plan.partition[day][g] = plan.partition[day][g][:len(plan.partition[day][g])-1]
	if day > 0 {
		if plan.Contains(day-1, g, point) {
			plan.partitionCost += intervals[g]
		} else {
			plan.partitionCost -= intervals[g]
		}
	}
	if day < gameConst.D-1 {
		if plan.Contains(day+1, g, point) {
			plan.partitionCost += intervals[g]
		} else {
			plan.partitionCost -= intervals[g]
		}
	}
}
func (plan *PartDivPlan) Contains(day int, g int, point int) bool {
	for i := 0; i < plan.lenPartition[day][g]; i++ {
		if plan.partition[day][g][i] > point {
			break
		}
		if plan.partition[day][g][i] == point {
			return true
		}
	}
	return false
}

func (plan *PartDivPlan) Evaluate(gameConst *GameConst, numGroups int) float64 {
	penalty := 0
	for d := range plan.penalties {
		penalty += plan.penalties[d]
		if plan.numRect[d] < gameConst.N {
			penalty += 10 * (gameConst.N - plan.numRect[d])
		}
	}
	value := (-float64(penalty)*100 - float64(plan.partitionCost)) * 0.001
	return value
}

func (plan *PartDivPlan) EvaluateDiff(gameConst *GameConst, action int) float64 {
	return 0.0
}
func (plan *PartDivPlan) CalcPenalty(gameConst *GameConst, intervals []int, day int) int {
	areas := plan.Toareas(gameConst, intervals, day)
	penalty := 0
	for k := gameConst.N - 1; k >= 0; k-- {
		reservation := gameConst.Reservations[day][k]
		area := 0
		areaIndex := len(areas) - 1 - (gameConst.N - 1 - k)
		if areaIndex >= 0 {
			area = areas[areaIndex]
		}
		penalty += maxInt(0, reservation-area)
	}
	return penalty
}
func (plan *PartDivPlan) Toareas(gameConst *GameConst, intervals []int, day int) []int {
	areas := make([]int, plan.numRect[day])
	for g := range plan.partition[day] {
		prev := 0
		for i := 0; i < plan.lenPartition[day][g]; i++ {
			if i == 0 {
				continue
			}
			areas = append(areas, intervals[g]*(plan.partition[day][g][i]-prev))
			prev = plan.partition[day][g][i]
		}
	}
	sort.Slice(areas, func(i, j int) bool { return areas[i] < areas[j] })
	// fmt.Fprintln(os.Stderr, areas)
	return areas
}
func (plan *PartDivPlan) ToRects(gameConst *GameConst, intervals []int, day int) [][2][2]int {
	rects := [][2][2]int{}
	h := 0
	for g := range plan.partition[day] {
		prev := 0
		for i := 0; i < plan.lenPartition[day][g]; i++ {
			if i == 0 {
				continue
			}
			rects = append(rects, [2][2]int{{h, prev}, {h + intervals[g], (plan.partition[day][g][i])}})
			prev = plan.partition[day][g][i]
		}
		h += intervals[g]
	}
	sort.Slice(rects, func(i, j int) bool {
		return (rects[i][1][0]-rects[i][0][0])*(rects[i][1][1]-rects[i][0][1]) < (rects[j][1][0]-rects[j][0][0])*(rects[j][1][1]-rects[j][0][1])
	})
	return rects
}

type PartDivAnnealing struct {
	startTime             time.Time
	duration              int // 焼きなまし実行時間　microsecond
	loopLimit             int
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int          // loopの回った回数
	valid          int          // validな解を得た回数
	accepted       int          // 採用された回数
	temp           float64      // 現在の温度
	reciprocaltemp float64      // 温度の逆数
	best           *PartDivPlan // これまでの最善解
	bestScore      float64      // 最善解のスコア
	now            *PartDivPlan // 現在の遷移元
	nowScore       float64      // 現在の遷移元のスコア
}

func NewPDA() *PartDivAnnealing {
	sa := PartDivAnnealing{}
	return &sa
}

func (sa *PartDivAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	p := sa.getAcceptRateFromDiff(diff, bestScore)
	return p
}
func (sa *PartDivAnnealing) getAcceptRateFromDiff(diff float64, bestScore float64) float64 {
	p := math.Exp(diff * sa.reciprocaltemp)
	return p
}

func (sa *PartDivAnnealing) isBest(score float64, bestScore float64) bool {
	return score > bestScore

}

func (sa *PartDivAnnealing) Init(gameConst *GameConst, duration int, loopLimit int, startTemp float64, endTemp float64, best *PartDivPlan, bestScore float64) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.loopLimit = loopLimit
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy(gameConst)
	sa.nowScore = bestScore

}

func (sa *PartDivAnnealing) Run(gameConst *GameConst, heights []int, numGroups int, timeType int, start time.Time) {
	prop := 0.0
	sa.updateTempIntervalExp = 12
	intervals := []int{}
	// fmt.Fprintln(os.Stderr, heights)
	lastupd := 0
	for i := range heights {
		interval := gameConst.W - heights[i]
		if i > 0 {
			interval -= gameConst.W - heights[i-1]
		}
		intervals = append(intervals, interval)
	}
	sa.best.InitialPenalty(gameConst, intervals)
	sa.bestScore = sa.best.Evaluate(gameConst, numGroups)
	sa.now.InitialPenalty(gameConst, intervals)
	sa.nowScore = sa.now.Evaluate(gameConst, numGroups)
	// fmt.Fprintln(os.Stderr, sa.now.partitionCost)
	if numGroups >= gameConst.N+1 {
		return
	}
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			if time.Since(start).Microseconds() > TIME_LIMIT*95/100 {
				break
			}
			if sa.sumIter > lastupd+maxInt(lastupd*3, 100000) {
				// fmt.Fprintln(os.Stderr, numGroups, gameConst.N, gameConst.N, sa.sumIter, sa.bestScore)
				break
			}
			if timeType == 0 {
				prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			} else {
				prop = float64(sa.sumIter) / float64(sa.loopLimit)
			}
			if prop >= 1 {
				// time up
				break
			}
			if DEBUG {
				// fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sa.sumIter, sa.bestScore)
			}
			// 指数ver.
			sa.temp = math.Pow(sa.startTemp, 1-prop) * math.Pow(sa.endTemp, prop)
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			// fmt.Fprintln(os.Stderr, sa.best.lenPartition[0])

			// 最善解に戻す
			// sa.now.Load(sa.best)
			// sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		d := gameConst.Random.StrongIntn(gameConst.D)
		g := gameConst.Random.StrongIntn(numGroups)
		g2 := gameConst.Random.StrongIntn(numGroups)
		point := 0
		point2 := 0

		actiontype := 0
		actiontype = gameConst.Random.StrongIntn(5)
		if sa.now.numRect[d] == numGroups {
			actiontype = 0
		} else if sa.now.numRect[d] < gameConst.N {
			actiontype = 0
		} else if sa.now.numRect[d] == gameConst.N+1 {
			if actiontype == 0 {
				actiontype = 3
			}
		}
		if actiontype == 0 {
			// 分割
			g2, point2 = sa.now.getRandomNewPartition(gameConst, numGroups, d, g, point)
		} else if actiontype == 1 {
			// 結合
			g, point = sa.now.getRandomPartition(gameConst, numGroups, d, g, point)
		} else if actiontype == 2 {
			// 同グループ内移動

			g, point = sa.now.getRandomPartition(gameConst, numGroups, d, g, point)
			g, point2 = sa.now.getRandomNewPartition(gameConst, numGroups, d, g, point2)
			// for point2 <= 0 || point2 >= gameConst.W || sa.now.Contains(d, g, point2) {
			// 	r := gameConst.Random.StrongIntn(21) - 10
			// 	point2 = point + r
			// }
		} else if actiontype == 3 {
			// 移動
			g2, point2 = sa.now.getRandomNewPartition(gameConst, numGroups, d, g2, point2)
			g, point = sa.now.getRandomPartition(gameConst, numGroups, d, g, point)
		} else {
			// flipする
			g, point = sa.now.getRandomPartition(gameConst, numGroups, d, g, point)
			before := 0
			after := gameConst.W
			for i := 0; i < sa.now.lenPartition[d][g]; i++ {
				if sa.now.partition[d][g][i] == point {
					break
				}
				before = sa.now.partition[d][g][i]
			}
			for i := sa.now.lenPartition[d][g] - 1; i >= 0; i-- {
				if sa.now.partition[d][g][i] == point {
					break
				}
				after = sa.now.partition[d][g][i]
			}
			c := sa.now.partitionCost
			sa.now.RemovePartition(gameConst, intervals, d, g, point)
			sa.now.SetPartition(gameConst, intervals, d, g, after-(point-before))
			c2 := sa.now.partitionCost
			if c < c2 {
				sa.now.RemovePartition(gameConst, intervals, d, g, after-(point-before))
				sa.now.SetPartition(gameConst, intervals, d, g, point)
			}

			continue

		}

		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.StrongFloat64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff
		diff := float64(sa.now.partitionCost)
		if actiontype == 0 {
			sa.now.SetPartition(gameConst, intervals, d, g2, point2)
		} else if actiontype == 1 {
			sa.now.RemovePartition(gameConst, intervals, d, g, point)
		} else if actiontype == 2 {
			sa.now.RemovePartition(gameConst, intervals, d, g, point)
			sa.now.SetPartition(gameConst, intervals, d, g, point2)
		} else if actiontype == 3 {
			sa.now.RemovePartition(gameConst, intervals, d, g, point)
			sa.now.SetPartition(gameConst, intervals, d, g2, point2)
		}
		diff -= float64(sa.now.partitionCost)
		prevPenalty := sa.now.penalties[d]
		score := -float64(INF)
		if prevPenalty > 0 || diff >= 0 {

			newPenalty := sa.now.CalcPenalty(gameConst, intervals, d)
			// fmt.Fprintln(os.Stderr, prevPenalty, newPenalty)
			sa.now.penalties[d] = newPenalty
			// if sa.sumIter%10000 == 0 {
			// 	fmt.Fprintln(os.Stderr, day, k1, k2)
			// }
			sa.valid++
			score = sa.now.Evaluate(gameConst, numGroups)
			diff = score - sa.nowScore
		}
		// p := sa.getAcceptRate(score, sa.nowScore)
		// r := gameConst.Random.StrongFloat64()
		// if p >= r {
		r := gameConst.Random.LogFloat64()
		if sa.temp*r < diff {
			// 採用
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				// sa.best = sa.now.Copy()
				sa.best.Load(sa.now)
				sa.bestScore = sa.nowScore
				lastupd = sa.sumIter
			}
		} else {
			// rollback（nowを戻す）
			if actiontype == 0 {
				sa.now.RemovePartition(gameConst, intervals, d, g2, point2)
			} else if actiontype == 1 {
				sa.now.SetPartition(gameConst, intervals, d, g, point)
			} else if actiontype == 2 {
				sa.now.SetPartition(gameConst, intervals, d, g, point)
				sa.now.RemovePartition(gameConst, intervals, d, g, point2)
			} else if actiontype == 3 {
				sa.now.RemovePartition(gameConst, intervals, d, g2, point2)
				sa.now.SetPartition(gameConst, intervals, d, g, point)

			}
			sa.now.penalties[d] = prevPenalty
		}
	}
	// fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	// fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	// fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	// fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	// fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}

func (plan *PartDivPlan) getRandomNewPartition(gameConst *GameConst, numGroups int, d int, g2 int, point2 int) (int, int) {
	r := 0
	rRange := 1
	rOffset := 0
	if d > 0 && plan.lenPartition[d-1][g2] > 2 {
		rRange++
	}
	if d < gameConst.D-1 && plan.lenPartition[d+1][g2] > 2 {
		rRange++
		rOffset--
	}
	for plan.Contains(d, g2, point2) {
		r = gameConst.Random.StrongIntn(rRange) + rOffset
		if r == 0 {
			point2 = gameConst.Random.StrongIntn(gameConst.W-1) + 1
		} else if r == 1 {
			_, point2 = plan.getRandomPartition(gameConst, numGroups, d-1, g2, point2)
		} else if r == -1 {
			_, point2 = plan.getRandomPartition(gameConst, numGroups, d+1, g2, point2)
		}
	}
	return g2, point2
}
func (plan *PartDivPlan) getRandomPartition(gameConst *GameConst, numGroups int, d int, g int, point int) (int, int) {
	for plan.lenPartition[d][g] == 2 {
		g = gameConst.Random.StrongIntn(numGroups)
	}
	point = plan.partition[d][g][gameConst.Random.StrongIntn(plan.lenPartition[d][g]-2)+1]
	return g, point
}
