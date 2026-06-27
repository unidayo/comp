// last local submission:
package main

import (
	"bufio"
	"fmt"
	"math"
	"math/bits"
	"math/rand"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

const (
	INF = 100000000
	// Main
	NUM_KIND = 6

	TIME_LIMIT = 1800 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01

	MINIMIZE = true

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE    = false // 複数ケースを試すか
	REPEAT        = false // 同一ケースを回すか
	TEST_NUM      = 1000
	PARALLEL_NUM  = 6 // 6
	PREFIX        = 0 // nケース目から始めたい場合
	OUT_FOLDER    = "ignore\\out.txt"
	FILE          = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER   = "0002"          // 単一ケースを試す場合の番号
	RECORD        = true            // nケース結果を保存するか
	RECORD_FOLDER = "ignore\\tries" // nケース結果保存場所
	RECORD_CODE   = true
	CODE_FILE     = "ignore\\code" // テストケースの保存場所

	DEBUG        = true
	PRINT_ACTION = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 0    // 基本戦術、方針レベルの違い
	VERSION_MINOR = 0    // 改善等
	VERSION_PATCH = 0    // バグ修正,パラメータ変更など
	VERSION_NAME  = ""   // バージョン名
	VERSION_VALID = true // 提出に利用できる正当なものか

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

func getInputs(gameConst *GameConst) {
	// 例
	gameConst.Scanner.Scan()
	splitedInput := strings.Split(gameConst.Scanner.Text(), " ")
	N, _ := strconv.Atoi(splitedInput[0])
	M, _ := strconv.Atoi(splitedInput[1])
	L, _ := strconv.Atoi(splitedInput[2])
	gameConst.N = N
	gameConst.M = M
	gameConst.L = L

	gameConst.words = make([][]int, gameConst.N)
	gameConst.importances = make([]int, gameConst.N)

	for i := 0; i < N; i++ {
		gameConst.Scanner.Scan()
		splitedInput = strings.Split(gameConst.Scanner.Text(), " ")
		word := splitedInput[0]
		importance, _ := strconv.Atoi(splitedInput[1])
		gameConst.importances[i] = importance
		gameConst.sumImportance += importance
		gameConst.words[i] = make([]int, 0, 12)
		for j := range word {
			kind := abcdefToInt(word[j])
			gameConst.words[i] = append(gameConst.words[i], kind)
		}
	}
}

// 現在の状態を記録するもの
type GameState struct {
	Cs []int
	A  [][]int
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	// スライス型の変数は長さ0、適切なcapで初期化する
	g.Cs = make([]int, gConst.M)
	g.A = make([][]int, gConst.M)
	for i := range g.A {
		g.A[i] = make([]int, gConst.M)
	}
	return &g
}

func (g *GameState) Copy(gConst *GameConst) *GameState {
	newg := NewGameState(gConst)
	newg.Load(g, gConst)
	return newg
}

// gの各要素をgameの各要素で上書きする
func (g *GameState) Load(gameState *GameState, gConst *GameConst) {
	// 整数などの場合
	// g.N = gameState.N

	// 固定長配列の場合
	// g.array = gameState.array

	// スライスの場合
	// 注:各要素がスライスの場合にはこの構造を再帰的に繰り返す
	// g.sliceData:=g.sliceData[:0]
	// for i :=range gameState.sliceData {
	// 	g.sliceData = append(g.sliceData, gameState.siceData[i])
	// }
	for i := range g.Cs {
		g.Cs[i] = gameState.Cs[i]
	}
	for i := range g.A {
		for j := range g.A[i] {
			g.A[i][j] = gameState.A[i][j]
		}
	}

	// 構造体の場合
	// g.structData.Load(gameState.structData.Load)

}

func (g *GameState) Init(gameConst *GameConst) {
	for i := range g.Cs {
		g.Cs[i] = i % NUM_KIND
	}
	// g.Cs[1] = 3
	for i := range g.A {
		for j := range g.A[i] {
			if j < 4 {
				g.A[i][j] = 9
			} else {
				g.A[i][j] = 8

			}
		}
	}
}

func (g *GameState) Evaluate(gameConst *GameConst, mult float64) float64 {
	props := make([]float64, gameConst.M)
	nprops := make([]float64, gameConst.M)
	for i := range props {
		props[i] = 1 / float64(gameConst.M)
	}
	for loop := 0; loop < 5; loop++ {
		for i := range props {
			for j := range g.A[i] {
				nprops[j] += props[i] * float64(g.A[i][j]) * 0.01
			}
		}
		props, nprops = nprops, props
		for i := range nprops {
			nprops[i] = 0
		}
	}
	value := 0.0
	// 各ワードの生成確率を見る
	for w := 0; w < gameConst.N; w++ {
		if mult > 0.0 {
			if gameConst.importances[w] < 2000 {
				continue
			}
		}
		ps := make([]float64, gameConst.M)
		for i := gameConst.words[w][0]; i < gameConst.M; i += NUM_KIND {
			ps[i] = props[i]
		}
		for index := 0; index < len(gameConst.words[w])-1; index++ {
			nowi := gameConst.words[w][index]
			nexti := gameConst.words[w][index+1]

			for i := nowi; i < gameConst.M; i += NUM_KIND {
				for j := nexti; j < gameConst.M; j += NUM_KIND {
					nprops[j] += ps[i] * float64(g.A[i][j]) * 0.01
				}
			}
			ps, nprops = nprops, ps
			for i := range nprops {
				nprops[i] = 0
			}
		}
		p := 0.0
		for i := range ps {
			p += ps[i]
		}
		failp := fastPow(1-p, 1000000)
		if failp > 1-1e-10 {
			failp = 1 - 1e-10
		}
		lg := math.Log10(1 - failp)
		value += lg * float64(gameConst.importances[w]) * 0.2 * mult
		value += (1 - failp) * float64(gameConst.importances[w]) * 1
	}
	return value
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	N             int
	M             int
	L             int
	words         [][]int
	importances   []int
	relations     [NUM_KIND][NUM_KIND]int
	sumImportance int
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	g.Random = rand.New(rand.NewSource(runner.randSeed))
	return &g
}

func printAction(gameState *GameState, gameConst *GameConst, comment bool) {
	for i := 0; i < gameConst.M; i++ {
		abcdef := intToabcdef(gameState.Cs[i])
		fmt.Printf("%s", string(abcdef))
		for j := range gameState.A[i] {
			fmt.Printf(" %d", gameState.A[i][j])
		}
		fmt.Printf("\n")
	}
	// fmt.Println("アクションを出力")
}

func abcdefToInt(alphabet byte) int {
	if alphabet == 'a' {
		return 0
	}
	if alphabet == 'b' {
		return 1
	}
	if alphabet == 'c' {
		return 2
	}
	if alphabet == 'd' {
		return 3
	}
	if alphabet == 'e' {
		return 4
	}
	if alphabet == 'f' {
		return 5
	}
	return -1
}
func intToabcdef(kind int) byte {
	data := "abcdef"
	return data[kind]
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
	file := fmt.Sprintf("%04d", testCase)
	runner := NewRunner(testCase, time.Now().UnixNano())
	result := runner.doGame(file)
	if DEBUG {
		fmt.Fprintf(os.Stderr, "score:%d (case=%04d)\n", result.score, testCase)
	}
	return result
}

func parallelTester(prefix int) [TEST_NUM]Result {
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

		// 複数ケースを並行で実行
		results := parallelTester(PREFIX)

		// スコア・相対スコア計算
		sumScore := 0
		sumRelativeScore := 0.0
		minsScore := INF
		for i := 0; i < TEST_NUM; i++ {
			sumScore += results[i].score

			if results[i].score == INF {
				fmt.Fprintf(os.Stderr, "WARN: case %d failed.\n", i)
			}
			minsScore = minInt(minsScore, results[i].score)
		}
		fmt.Println(sumRelativeScore)

		// 結果の出力
		fmt.Fprintf(os.Stderr, "\n---END---\n%d case finished.\n", TEST_NUM)
		fmt.Fprintln(os.Stderr, "sumScore:", sumScore)

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
}

func NewAction() *Action {
	a := Action{}
	return &a
}

func (a *Action) Load(a2 *Action) {

}
func (a *Action) Copy() *Action {
	newa := NewAction()
	newa.Load(a)
	return newa
}

type Solver struct {
	timeLimit int // solverに与えられた時間制限 microsecond
}

func NewSolver(timeLimit int) *Solver {
	s := Solver{}
	s.timeLimit = timeLimit
	return &s
}

func (s *Solver) Solve(gameConst *GameConst) (*GameState, int) {
	start := time.Now()
	_ = start
	bestState := NewGameState(gameConst)
	bestState.Init(gameConst)
	maxScore := 0.0
	_ = maxScore

	// 貪欲を書くならここ

	// 貪欲ここまで
	sa := NewSA()
	sa.Init(1800*1000, 10, 1, 1, 0.2, bestState, bestState.Evaluate(gameConst, 1), gameConst)
	sa.Run2(gameConst)
	bestState = sa.best

	score := 0

	return bestState, score
}

func (runner *Runner) doGame(file string) Result {
	result := Result{}
	scanner := getScanner(FILE + "\\" + file + ".txt")

	// 初期値設定処理

	gameConst := NewGameConst(runner)
	gameConst.SetScanner(scanner)
	// データ取得
	getInputs(gameConst)

	// 初期値設定処理　ここまで
	solver := NewSolver(TIME_LIMIT)
	bestState, score := solver.Solve(gameConst)

	// アクションを出力
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		printAction(bestState, gameConst, false)
	}

	if LOCAL_TEST {
		score = calcScore(bestState, gameConst)
	}

	result.score = score

	return result
}

func calcScore(gameState *GameState, gameConst *GameConst) int {
	score := 0
	return score
}

func (plan *GameState) ApplyAction(gameConst *GameConst, action *SimulatedAnnealingAction) {
	plan.A[action.target][action.from] -= action.amount
	plan.A[action.target][action.to] += action.amount
}

func (plan *GameState) RevertAction(gameConst *GameConst, action *SimulatedAnnealingAction) {
	rAction := NewSimulatedAnnealingAction()
	rAction.target = action.target
	rAction.from = action.to
	rAction.to = action.from
	rAction.amount = action.amount
	plan.ApplyAction(gameConst, rAction)
}

func (plan *GameState) ApplyAction2(gameConst *GameConst, action *SimulatedAnnealingAction2) {
	numloop := action.strength * (100 - plan.A[action.target][action.to]) / 100
	for loop := 0; loop < numloop; loop++ {
		r := gameConst.Random.Intn(100 - plan.A[action.target][action.to])
		from := -1
		cSum := 0
		for f := 0; f < gameConst.N; f++ {
			if f == action.to {
				continue
			}
			cSum += plan.A[action.target][f]
			if r < cSum {
				from = f
				break
			}
		}
		plan.A[action.target][from]--
		plan.A[action.target][action.to]++
	}
}
func (plan *GameState) ApplyAction2s(gameConst *GameConst, actions []*SimulatedAnnealingAction2) {
	for i := range actions {
		plan.ApplyAction2(gameConst, actions[i])
	}
}

func (plan *GameState) EvaluateDiff(gameConst *GameConst, action *SimulatedAnnealingAction) float64 {
	return 0.0
}

type SimulatedAnnealingAction struct {
	target int
	from   int
	to     int
	amount int
}

func NewSimulatedAnnealingAction() *SimulatedAnnealingAction {
	return &SimulatedAnnealingAction{}
}

type SimulatedAnnealingAction2 struct {
	target   int
	to       int
	strength int
}

func NewSimulatedAnnealingAction2() *SimulatedAnnealingAction2 {
	return &SimulatedAnnealingAction2{}
}

type SimulatedAnnealing struct {
	startTime             time.Time
	duration              int     // 焼きなまし実行時間　microsecond
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	startTemp2            float64 // 開始時の温度
	endTemp2              float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter        int        // loopの回った回数
	valid          int        // validな解を得た回数
	accepted       int        // 採用された回数
	temp           float64    // 現在の温度
	temp2          float64    // 現在の温度
	reciprocaltemp float64    // 温度の逆数
	best           *GameState // これまでの最善解
	bestScore      float64    // 最善解のスコア
	now            *GameState // 現在の遷移元
	nowScore       float64    // 現在の遷移元のスコア
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

func (sa *SimulatedAnnealing) Init(duration int, startTemp float64, endTemp float64, startTemp2 float64, endTemp2 float64, best *GameState, bestScore float64, gameConst *GameConst) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp
	sa.startTemp2 = startTemp2
	sa.endTemp2 = endTemp2

	sa.duration = duration
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy(gameConst)
	sa.nowScore = bestScore

}

func (sa *SimulatedAnnealing) Run(gameConst *GameConst) {
	prop := 0.0
	sa.updateTempIntervalExp = 5
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
			sa.now = sa.best.Copy(gameConst)
			sa.nowScore = sa.bestScore
		}
		sa.sumIter++

		// ここで何かnowを変更する
		action := NewSimulatedAnnealingAction()
		action.target = gameConst.Random.Intn(gameConst.M)
		action.from = gameConst.Random.Intn(gameConst.M)
		action.to = gameConst.Random.Intn(gameConst.M)
		if action.from == action.to {
			continue
		}
		maxAmount := 100
		maxAmount = minInt(maxAmount, sa.now.A[action.target][action.from]-1)
		if maxAmount <= 0 {
			continue
		}
		action.amount = gameConst.Random.Intn(maxAmount) + 1

		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.Float64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff

		sa.now.ApplyAction(gameConst, action)
		sa.valid++
		score := sa.now.Evaluate(gameConst, sa.temp2)
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
	fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}

// 大胆に道を通すパターン
func (sa *SimulatedAnnealing) Run2(gameConst *GameConst) {
	prop := 0.0
	sa.updateTempIntervalExp = 9
	pState := NewGameState(gameConst)
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
			sa.temp2 = math.Pow(sa.startTemp2, 1-prop) * math.Pow(sa.endTemp2, prop)
			if prop > 0.8 {
				sa.temp2 = 0
			}
			// 線形ver.
			// sa.temp = sa.startTemp*(1-prop) + sa.endTemp*prop
			sa.reciprocaltemp = 1 / sa.temp

			sa.bestScore = sa.best.Evaluate(gameConst, sa.temp2)
			// 最善解に戻す
			// sa.now = sa.best.Copy(gameConst)
			// sa.nowScore = sa.bestScore
		}
		pState.Load(sa.now, gameConst)
		sa.sumIter++

		// いずれかのwordが出やすくなるように調整する
		w := -1
		if true {
			r := gameConst.Random.Intn(gameConst.sumImportance)
			cSum := 0
			for wIndex := 0; wIndex < gameConst.N; wIndex++ {
				cSum += gameConst.importances[wIndex]
				if r < cSum {
					w = wIndex
					break
				}
			}
		}
		strength := gameConst.Random.Intn(99) + 2
		index := gameConst.Random.Intn(len(gameConst.words[w]) - 1)
		start := -1
		// 	ランダムに選出
		for {
			r := gameConst.Random.Intn(gameConst.M)
			if sa.now.Cs[r] == gameConst.words[w][index] {
				start = r
				break
			}
		}
		action := NewSimulatedAnnealingAction2()
		action.target = start
		action.strength = strength
		index++

		for {
			r := gameConst.Random.Intn(gameConst.M)
			if sa.now.Cs[r] == gameConst.words[w][index] {
				start = r
				break
			}
		}
		action.to = start

		/*actions := make([]*SimulatedAnnealingAction2, 0, len(gameConst.words[w])-1)
		start := -1
		for index := 0; index < len(gameConst.words[w]); index++ {
			if start == -1 {
				// 	先頭のため、ランダムに選出
				for {
					r := gameConst.Random.Intn(gameConst.M)
					if sa.now.Cs[r] == gameConst.words[w][index] {
						start = r
						break
					}
				}
				continue
			}
			action := NewSimulatedAnnealingAction2()
			action.target = start
			action.strength = strength

			for {
				r := gameConst.Random.Intn(gameConst.M)
				if sa.now.Cs[r] == gameConst.words[w][index] {
					start = r
					break
				}
			}
			action.to = start
			actions = append(actions, action)

		}
		// for i := range actions {
		// 	fmt.Fprintf(os.Stderr, "%s", string(intToabcdef(sa.now.Cs[actions[i].target])))
		// }
		// fmt.Fprintf(os.Stderr, "\n")
		// fmt.Fprintln(os.Stderr, gameConst.words[w])

		sa.now.ApplyAction2s(gameConst, actions)
		*/
		sa.now.ApplyAction2(gameConst, action)
		sa.valid++
		score := sa.now.Evaluate(gameConst, sa.temp2)
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
			sa.now.Load(pState, gameConst)
		}
	}
	fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}
