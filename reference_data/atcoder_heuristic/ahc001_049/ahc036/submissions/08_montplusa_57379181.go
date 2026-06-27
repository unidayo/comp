// last local submission: 1725257792
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
	INF = 100000000
	// Main
	TIME_LIMIT = 2400 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01
	RANDOMIZE  = true

	MINIMIZE = true

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = true  // 複数ケースを試すか
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 3000
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FOLDER         = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "0021"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = true
	VISUALIZE          = false
	PRINT_ACTION       = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 2    // 基本戦術、方針レベルの違い
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

func getInputs(gConst *GameConst) {
	// 例
	gConst.Scanner.Scan()
	splitedInput := strings.Split(gConst.Scanner.Text(), " ")
	N, _ := strconv.Atoi(splitedInput[0]) // N=600
	M, _ := strconv.Atoi(splitedInput[1])
	T, _ := strconv.Atoi(splitedInput[2]) // T=600
	La, _ := strconv.Atoi(splitedInput[3])
	Lb, _ := strconv.Atoi(splitedInput[4])
	gConst.NumCities = N
	gConst.NumRoads = M
	gConst.NumTravelPoints = T + 1
	gConst.LenCandidateArray = La
	gConst.MaxGreenNum = Lb

	gConst.Neighbors = make([][]int, N)
	for loop := 0; loop < M; loop++ {
		gConst.Scanner.Scan()
		splitedInput = strings.Split(gConst.Scanner.Text(), " ")
		u, _ := strconv.Atoi(splitedInput[0])
		v, _ := strconv.Atoi(splitedInput[1])
		gConst.Neighbors[u] = append(gConst.Neighbors[u], v)
		gConst.Neighbors[v] = append(gConst.Neighbors[v], u)
	}
	gConst.Scanner.Scan()
	splitedInput = strings.Split(gConst.Scanner.Text(), " ")
	gConst.TravelPoints = make([]int, 0, gConst.NumTravelPoints)
	gConst.TravelPoints = append(gConst.TravelPoints, 0)
	for i := 0; i < T; i++ {
		t, _ := strconv.Atoi(splitedInput[i])
		gConst.TravelPoints = append(gConst.TravelPoints, t)
	}
	gConst.Poss = make([][2]int, N)
	for index := 0; index < N; index++ {
		gConst.Scanner.Scan()
		splitedInput = strings.Split(gConst.Scanner.Text(), " ")
		x, _ := strconv.Atoi(splitedInput[0])
		y, _ := strconv.Atoi(splitedInput[1])
		gConst.Poss[index][0] = x
		gConst.Poss[index][1] = y
	}
}

// 現在の状態を記録するもの
type GameState struct {
	Progress   int // 到達済み旅行地点数
	Pos        int
	GreenArray []int // 通行可能都市
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	g.GreenArray = make([]int, gConst.MaxGreenNum)
	for i := range g.GreenArray {
		g.GreenArray[i] = -1
	}
	return &g
}

// actionが適応可能かどうかを返す
func (g *GameState) JudgeValidAction(gameConst *GameConst, action *Action) bool {
	ok := true
	if action.ActionType == 0 {
		if action.param1 < 1 || action.param1 > gameConst.MaxGreenNum {
			if DEBUG {
				fmt.Fprintf(os.Stderr, "ERROR:out of range (param1: %d)\n", action.param1)
			}
			ok = false
		}
		if action.param2 < 0 || action.param2+action.param1 > gameConst.LenCandidateArray {
			if DEBUG {
				fmt.Fprintf(os.Stderr, "ERROR:out of range (param2)\n")
			}
			ok = false
		}
		if action.param3 < 0 || action.param3+action.param1 > gameConst.MaxGreenNum {
			if DEBUG {
				fmt.Fprintf(os.Stderr, "ERROR:out of range (param3)\n")
			}
			ok = false
		}
	} else if action.ActionType == 1 {
		if gameConst.DistanceMap[g.Pos][action.param1] != 1 {
			if DEBUG {
				fmt.Fprintf(os.Stderr, "ERROR:can not move from %d to %d\n", g.Pos, action.param1)
			}
			ok = false
		}
		exist := false
		for i := range g.GreenArray {
			if g.GreenArray[i] == action.param1 {
				exist = true
				break
			}
		}
		if !exist {
			if DEBUG {
				fmt.Fprintf(os.Stderr, "ERROR:the light is red (city %d)\n", action.param1)
			}
			ok = false
		}
	}
	return ok
}

func (g *GameState) ApplyAction(gameConst *GameConst, candidateArray []int, action *Action) bool {
	ok := true
	if action.ActionType == 0 {
		for i := 0; i < action.param1; i++ {
			g.GreenArray[action.param3+i] = candidateArray[action.param2+i]
		}
	} else if action.ActionType == 1 {
		g.Pos = action.param1
		if g.Progress < gameConst.NumTravelPoints && gameConst.TravelPoints[g.Progress] == g.Pos {
			g.Progress++
		}
	}
	return ok
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	NumCities         int
	NumRoads          int
	NumTravelPoints   int
	MaxGreenNum       int
	LenCandidateArray int

	Neighbors    [][]int
	TravelPoints []int
	Poss         [][2]int
	DistanceMap  [][]int
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func (g *GameConst) CalcDistanceMap(gameConst *GameConst) {
	distanceMap := make([][]int, gameConst.NumCities)
	for i := range distanceMap {
		distanceMap[i] = make([]int, gameConst.NumCities)
		for j := range distanceMap[i] {
			distanceMap[i][j] = 1000
		}
	}
	queue := NewBFSQueue(gameConst.NumCities)
	for start := 0; start < gameConst.NumCities; start++ {
		queue.Init()
		distanceMap[start][start] = 0
		queue.Push(start)
		for queue.Len() > 0 {
			pos := queue.Pop()
			d := distanceMap[start][pos]
			for _, np := range gameConst.Neighbors[pos] {
				if distanceMap[start][np] <= d+1 {
					continue
				}
				distanceMap[start][np] = d + 1
				queue.Push(np)
			}
		}
	}
	gameConst.DistanceMap = distanceMap
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	g.Random = rand.New(rand.NewSource(runner.randSeed))
	return &g
}

func printAction(action *Action, gameState *GameState, gameConst *GameConst, comment bool) {
	if action.ActionType == 0 {
		fmt.Println("s", action.param1, action.param2, action.param3)
	} else if action.ActionType == 1 {
		fmt.Println("m", action.param1)
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
	file := fmt.Sprintf("%04d", testCase)
	runner := NewRunner(testCase, time.Now().UnixNano())
	result := runner.doGame(file)
	if DEBUG {
		fmt.Fprintf(os.Stderr, "score:%d (case=%04d)\n", result.score, testCase)
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
		result := runner.doGame(FILE_NUMBER)
		if LOCAL_TEST && DEBUG {
			fmt.Fprintf(os.Stderr, "score: %d\n", result.score)
		}
	}
}

type Action struct {
	ActionType int // 0:s, 1:m
	param1     int // s 更新幅 m 移動先
	param2     int // s candidateArray対応座標
	param3     int // s greenArray対応座標
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

func getScore() float64 {
	return 0.0
}

func evaluate(gameState *GameState, gameConst *GameConst) float64 {
	value := 0.0
	return value
}

type Solver struct {
	timeLimit int // solverに与えられた時間制限 microsecond
}

func NewSolver(timeLimit int) *Solver {
	s := Solver{}
	s.timeLimit = timeLimit
	return &s
}

func (s *Solver) Solve(gameState *GameState, gameConst *GameConst, pathorders []int, final bool) ([]*Action, []int, int) {
	start := time.Now()
	_ = start
	maxActions := make([]*Action, 0, 10000)
	maxScore := 0.0
	_ = maxScore

	// 貪欲を書くならここ
	minfirstLength := 0
	maxfirstLength := 100

	for minfirstLength != maxfirstLength {
		fLength := (minfirstLength + maxfirstLength + 1) / 2
		// fLength := minfirstLength + 1
		// fLength := maxfirstLength
		_, _, ok := makeCandidateArray(gameConst, fLength, pathorders, false)
		if ok {
			minfirstLength = fLength
		} else {
			maxfirstLength = fLength - 1
		}
	}
	candidateArray, _, ok := makeCandidateArray(gameConst, minfirstLength, pathorders, true)
	if !ok {
		return []*Action{}, []int{}, 0
	}
	inverseCandidateArray := make([][]int, gameConst.NumCities)
	for i := range candidateArray {
		pos := candidateArray[i]
		inverseCandidateArray[pos] = append(inverseCandidateArray[pos], i)
	}

	// candidateArray依存の前処理
	availableLength := make([]int, gameConst.LenCandidateArray)
	for i := 0; i < gameConst.LenCandidateArray; i++ {
		availableLength[i] = minInt(gameConst.MaxGreenNum, gameConst.LenCandidateArray-i)
	}
	possibleOperations := make([][]int, gameConst.NumCities) // 各都市に対応する信号変更候補
	for i := range possibleOperations {
		possibleOperations[i] = make([]int, 0, 100)
	}
	queue := NewBFSQueue(gameConst.NumCities)
	borderqueue := NewBFSQueue(gameConst.NumCities)
	checked := make([]int, gameConst.NumCities)
	borderChecked := make([]int, gameConst.NumCities)
	uniqueId := 0
	borderUniqueId := 0
	for operation := 0; operation < gameConst.LenCandidateArray; operation++ {
		ended := false
		for !ended {
			uniqueId++
			// operationに対応する結果
			// 暫定的にoperationの結果青信号が連結になっていないものは除外
			pos := candidateArray[operation]
			borderUniqueId++
			queue.Init()
			borderqueue.Init()
			queue.Push(pos)
			checked[pos] = uniqueId
			for queue.Len() > 0 {
				pos = queue.Pop()
				for i := range gameConst.Neighbors[pos] {
					np := gameConst.Neighbors[pos][i]
					if checked[np] == uniqueId || borderChecked[np] == borderUniqueId {
						continue
					}
					exist := false
					for j := range inverseCandidateArray[np] {
						if inverseCandidateArray[np][j] < operation {
							continue
						}
						if inverseCandidateArray[np][j] < operation+availableLength[operation] {
							exist = true
						} else {
							exist = false
						}
						break
					}
					if exist {
						queue.Push(np)
						borderqueue.Push(np)
						checked[np] = uniqueId
					} else {
						borderqueue.Push(np)
						borderChecked[np] = borderUniqueId
					}
				}
			}
			// よくないがqueueのパラメータを利用して一覧取得
			connected := true
			for greenlightIndex := 0; greenlightIndex < availableLength[operation]; greenlightIndex++ {
				pos = candidateArray[operation+greenlightIndex]
				if checked[pos] != uniqueId {
					connected = false
					break
				}
			}
			if !connected {
				availableLength[operation]--
				continue
			}
			for borderqueue.Len() > 0 {
				border := borderqueue.Pop()
				possibleOperations[border] = append(possibleOperations[border], operation)
			}
			ended = true
		}
	}
	// fmt.Fprintln(os.Stderr, availableLength)
	// fmt.Fprintln(os.Stderr, possibleOperations)

	// 最短経路に沿ってつなげる
	/*
		for gameState.Progress < gameConst.NumTravelPoints {
			goal := gameConst.TravelPoints[gameState.Progress]
			d := gameConst.DistanceMap[gameState.Pos][goal]
			nextp := -1
			for _, p := range gameConst.Neighbors[gameState.Pos] {
				if gameConst.DistanceMap[p][goal] == d-1 {
					nextp = p
					break
				}
			}
			exist := false
			for i := range gameState.GreenArray {
				if gameState.GreenArray[i] == nextp {
					exist = true
					break
				}
			}
			if !exist {
				a := NewAction()
				a.ActionType = 0
				a.param1 = gameConst.MaxGreenNum
				a.param2 = minInt(inverseCandidateArray[nextp][0], gameConst.LenCandidateArray-gameConst.MaxGreenNum)
				a.param3 = 0
				maxActions = append(maxActions, a)
				gameState.ApplyAction(gameConst, candidateArray, a)
			}

			a2 := NewAction()
			a2.ActionType = 1
			a2.param1 = nextp
			maxActions = append(maxActions, a2)
			gameState.ApplyAction(gameConst, candidateArray, a2)
		}*/

	// 信号の変え方についてdijkstra法で求める
	maxActions = getActionsByDijkstra(gameConst, availableLength, candidateArray, inverseCandidateArray, possibleOperations, final)

	// 貪欲ここまで

	// データチェック
	counts := make([]int, gameConst.NumCities)
	for i := range gameConst.TravelPoints {
		counts[gameConst.TravelPoints[i]]++
	}
	// fmt.Fprintln(os.Stderr, counts)
	sumd := 0
	for i := range gameConst.TravelPoints {
		now := 0
		if i > 0 {
			now = gameConst.TravelPoints[i-1]
		}
		next := gameConst.TravelPoints[i]
		// fmt.Fprintln(os.Stderr, now, next, gameConst.DistanceMap[now][next])
		sumd += gameConst.DistanceMap[now][next]
	}
	// fmt.Fprintln(os.Stderr, sumd)

	score := 0
	for i := range maxActions {
		if maxActions[i].ActionType == 0 {
			score += 1
		}
	}

	return maxActions, candidateArray, score
}

func (runner *Runner) doGame(file string) Result {
	result := Result{}
	scanner := getScanner(FILE + "\\" + file + ".txt")

	// 初期値設定処理

	gameConst := NewGameConst(runner)
	gameConst.SetScanner(scanner)
	// データ取得
	getInputs(gameConst)
	gameConst.CalcDistanceMap(gameConst)

	gameState := NewGameState(gameConst)

	// 初期値設定処理　ここまで
	solver := NewSolver(TIME_LIMIT)
	bestActions := []*Action{}
	bestPathOrders := []int{}
	bestcandidateArray := []int{}
	bestScore := INF
	start := time.Now()
	for {
		if RANDOMIZE {
			if time.Since(start).Microseconds() > TIME_LIMIT {
				break
			}
		}
		pathorders := makePathOrders(gameConst)
		_, _, score := solver.Solve(gameState, gameConst, pathorders, false)
		if score < bestScore {
			bestPathOrders = pathorders
			bestScore = score
		}
		if !RANDOMIZE {
			break
		}
	}
	// bestPathOrdersから生成
	bestActions, bestcandidateArray, bestScore = solver.Solve(gameState, gameConst, bestPathOrders, true)

	// アクションを出力
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		for i := range bestcandidateArray {
			fmt.Printf("%d", bestcandidateArray[i])
			if i != len(bestcandidateArray)-1 {
				fmt.Printf(" ")
			} else {
				fmt.Printf("\n")
			}
		}
		for i := range bestActions {
			printAction(bestActions[i], gameState, gameConst, false)
		}
	}

	if LOCAL_TEST {
		bestScore = calcScore(gameConst, bestActions, bestcandidateArray)
	}

	result.score = bestScore

	return result
}

func calcScore(gameConst *GameConst, actions []*Action, candidateArray []int) int {
	gameState := NewGameState(gameConst)
	gameState.Progress = 1
	score := 0
	for i := range actions {
		ok := gameState.JudgeValidAction(gameConst, actions[i])
		if !ok {
			score = -1
			break
		}
		gameState.ApplyAction(gameConst, candidateArray, actions[i])
		if actions[i].ActionType == 0 {
			score++
		}
	}
	if gameState.Progress != gameConst.NumTravelPoints {
		fmt.Fprintf(os.Stderr, "ERROR:did not finish travel %d/%d\n", gameState.Progress, gameConst.NumTravelPoints)
		score = -1
	}
	return score
}

// 最大Loopがわかっている場合のqueue BFSで使うことが多い
type BFSQueue struct {
	queue     []int // 対象をなんらかのindexで持っておく想定 距離の場合はposなど
	popIndex  int
	pushIndex int
}

func NewBFSQueue(maxLoop int) *BFSQueue {
	q := BFSQueue{}
	q.queue = make([]int, maxLoop)
	return &q
}

func (q *BFSQueue) Init() {
	q.popIndex = 0
	q.pushIndex = 0
}

func (q *BFSQueue) Pop() int {
	v := q.queue[q.popIndex]
	q.popIndex++
	return v
}

func (q *BFSQueue) Push(v int) {
	q.queue[q.pushIndex] = v
	q.pushIndex++
}

func (q *BFSQueue) Len() int {
	return q.pushIndex - q.popIndex
}

// priorityが非負整数値かつ範囲が決まっている場合
type FakeHeapQueue struct {
	queue    []*BFSQueue // 対象をなんらかのindexで持っておく想定 距離の場合はposなど
	minIndex int
	length   int
}

func NewFakeHeapQueue(maxLoop int, maxDistance int) *FakeHeapQueue {
	q := FakeHeapQueue{}
	q.queue = make([]*BFSQueue, maxDistance)
	for i := range q.queue {
		q.queue[i] = NewBFSQueue(maxLoop)
	}
	return &q
}

func (q *FakeHeapQueue) Init() {
	for i := range q.queue {
		q.queue[i].Init()
	}
	q.minIndex = len(q.queue)
	q.length = 0
}

func (q *FakeHeapQueue) Pop() (int, int) {
	v := q.queue[q.minIndex].Pop()
	p := q.minIndex
	q.length--
	if q.length == 0 {
		q.minIndex = len(q.queue)
	} else {
		for {
			if q.queue[q.minIndex].Len() > 0 {
				break
			}
			q.minIndex++
		}
	}
	return v, p
}

func (q *FakeHeapQueue) Push(v int, priority int) {
	q.queue[priority].Push(v)
	if priority < q.minIndex {
		q.minIndex = priority
	}
	q.length++
}

func (q *FakeHeapQueue) Len() int {
	return q.length
}

func makeCandidateArray(gameConst *GameConst, firstLength int, pathorders []int, useSA bool) ([]int, int, bool) {
	candidateArray := make([]int, gameConst.LenCandidateArray)
	ok := true
	// candidateArrayを決定
	/*for i := 0; i < gameConst.LenCandidateArray; i++ {
		pos := 0
		if i < gameConst.NumCities {
			pos = i
		} else {
			pos = 0
		}
		candidateArray[i] = pos
	}*/
	listIndex := 0
	list := make([]int, gameConst.NumCities+1) // travelindexを格納したもの
	listNext := make([]int, gameConst.NumCities+1)
	indexList := make([]int, gameConst.NumCities+1)
	start := -1
	for i := range list {
		list[i] = -1
	}
	for i := range listNext {
		listNext[i] = -1
	}
	placed := make([]bool, gameConst.NumTravelPoints)
	distances := make([]int, gameConst.NumTravelPoints)
	for i := range distances {
		from := 0
		if i == 0 {
			from = 0
		} else {
			from = gameConst.TravelPoints[i-1]
		}
		to := gameConst.TravelPoints[i]
		distances[i] = gameConst.DistanceMap[from][to]
	}

	// 初期地点を追加
	sumd := 0
	numSkipped := 0
	for i := 0; i < firstLength+numSkipped; i++ {
		index := pathorders[i]
		if index == 0 {
			continue
		}
		if placed[index] || placed[index-1] {
			numSkipped++
			continue
		}
		sumd += distances[pathorders[i]]
		listNext[listIndex] = start
		list[listIndex] = gameConst.TravelPoints[index]
		indexList[listIndex] = index
		start = listIndex
		listIndex++
		placed[index] = true

		listNext[listIndex] = start
		list[listIndex] = gameConst.TravelPoints[index-1]
		indexList[listIndex] = index - 1
		start = listIndex
		listIndex++
		placed[index-1] = true
	}
	// 各地点を順に追加
	prevCount := 0
	for travelIndex := 0; travelIndex < gameConst.NumTravelPoints; travelIndex++ {
		if placed[travelIndex] {
			continue
		}
		pos := gameConst.TravelPoints[travelIndex]
		best := -1
		bestvalue := INF
		bestCount := -1
		if start != -1 {
			bestvalue = gameConst.DistanceMap[pos][list[start]] * 1000
		}
		prev := start
		next := -1
		count := 0
		for {
			count++
			if prev == -1 {
				break
			}

			next = listNext[prev]
			value := gameConst.DistanceMap[list[prev]][pos]
			if next != -1 {
				value += gameConst.DistanceMap[pos][list[next]]
				value -= gameConst.DistanceMap[list[prev]][list[next]]
			}
			// if count == prevCount || count == prevCount+1 {
			// 	value -= 1
			// }
			if count == prevCount || count == prevCount+1 {
				value -= 1
			}

			value *= 1000
			value += absInt(count-prevCount) * 1
			if value < bestvalue {
				bestvalue = value
				best = prev
				bestCount = count
			}
			prev = next
		}
		if best == -1 {
			listNext[listIndex] = start
			list[listIndex] = pos
			indexList[listIndex] = travelIndex
			start = listIndex
		} else {
			listNext[listIndex] = listNext[best]
			listNext[best] = listIndex
			list[listIndex] = pos
			indexList[listIndex] = travelIndex
		}
		prevCount = bestCount
		listIndex++
		placed[travelIndex] = true
	}
	// 総距離計算
	prev := start
	next := -1
	value := 0
	for {
		if prev == -1 {
			break
		}
		next = listNext[prev]
		if next != -1 {
			// fmt.Fprintln(os.Stderr, gameConst.DistanceMap[list[prev]][list[next]])
			value += gameConst.DistanceMap[list[prev]][list[next]]
		}

		prev = next
	}
	// 仮評価値計算
	// sc := 0.0
	// lIndex := start
	// rIndex := start
	// d := 0
	// for i := 0; i < 3; i++ {
	// 	nrIndex := listNext[rIndex]
	// 	d += gameConst.DistanceMap[list[rIndex]][list[nrIndex]]
	// 	rIndex = nrIndex
	// }
	// for {
	// 	nrIndex := listNext[rIndex]
	// 	if nrIndex == -1 {
	// 		break
	// 	}
	// 	// sc += float64(d)/ float64(maxInt(gameConst.DistanceMap[list[rIndex]][list[lIndex]], 1))
	// 	// sc += float64(d-gameConst.DistanceMap[list[rIndex]][list[lIndex]]) / float64(maxInt(d, 1))
	// 	sc += float64(d - gameConst.DistanceMap[list[rIndex]][list[lIndex]])
	// 	d += gameConst.DistanceMap[list[rIndex]][list[nrIndex]]
	// 	nlIndex := listNext[lIndex]
	// 	d -= gameConst.DistanceMap[list[lIndex]][list[nlIndex]]
	// 	rIndex = nrIndex
	// 	lIndex = nlIndex
	//
	// }
	// sc /= float64(value)
	// fmt.Fprintln(os.Stderr, "length: ", value, "score: ", sc, "(firstLength:", firstLength, ")")

	// fmt.Fprintln(os.Stderr, "length: ", value, "(firstLength:", firstLength, ")")
	// if value+1 > gameConst.LenCandidateArray {
	// 	if VISUALIZE {
	// 		ok = false
	// 	} else {
	// 		return []int{}, 0, false
	// 	}
	// }
	if VISUALIZE {
		candidateArray = make([]int, maxInt(value+100, gameConst.LenCandidateArray))
	}
	// indexListを列にする
	travelOrders := make([]int, 0)
	curIndex := start
	travelOrders = append(travelOrders, indexList[curIndex])
	for {

		curIndex = listNext[curIndex]
		if curIndex == -1 {
			break
		}
		travelOrders = append(travelOrders, indexList[curIndex])
	}
	/*if false && useSA {
		border := 0
		for i := range travelOrders {
			if i == 0 {
				continue
			}
			border += gameConst.DistanceMap[gameConst.TravelPoints[travelOrders[i-1]]][gameConst.TravelPoints[travelOrders[i]]]
		}
		sa := NewSA()
		sa.Init(1800*1000, 10.0, 0.05, NewSimulatedAnnealingPlan(len(travelOrders)), 0, border, gameConst.MaxGreenNum)
		sa.setInitialPlan(gameConst, travelOrders)
		sa.Run(gameConst)
		fmt.Fprintln(os.Stderr, sa.best.travelOrders)
		travelOrders = sa.now.travelOrders
	}*/
	// fmt.Fprintln(os.Stderr, connectNext)
	keyPoints := make([]int, gameConst.NumTravelPoints)
	for i := range keyPoints {
		keyPoints[i] = gameConst.TravelPoints[travelOrders[i]]
	}

	arrayIndex := 0
	keyIndex := 0
	pos := keyPoints[keyIndex]
	if VISUALIZE {
		pos = 0
	}
	history := make([]int, gameConst.MaxGreenNum)
	for i := range history {
		history[i] = -1
	}
	historyIndex := 0
	candidateArray[arrayIndex] = pos
	arrayIndex++
	history[historyIndex] = pos
	historyIndex++

	for {
		for {
			if pos == keyPoints[keyIndex] {
				break
			}
			bestp := -1
			bestValue := float64(-INF)
			for _, np := range gameConst.Neighbors[pos] {
				if gameConst.DistanceMap[np][keyPoints[keyIndex]] == gameConst.DistanceMap[pos][keyPoints[keyIndex]]-1 {
					value := 0.0
					value -= float64((gameConst.Poss[np][0] - 500) * (gameConst.Poss[np][0] - 500))
					value -= float64((gameConst.Poss[np][1] - 500) * (gameConst.Poss[np][1] - 500))
					if value > bestValue {
						bestValue = value
						bestp = np
					}
				}
			}
			if true {
				exist := false
				for i := range history {
					if i == historyIndex {
						continue
					}
					if history[i] == bestp {
						exist = true
						break
					}
				}
				if !exist {
					if arrayIndex >= gameConst.LenCandidateArray {
						if VISUALIZE {
							ok = false
						} else {
							return []int{}, 0, false
						}
					}
					candidateArray[arrayIndex] = bestp
					arrayIndex++
					history[historyIndex] = bestp
					historyIndex++
					if historyIndex >= gameConst.MaxGreenNum {
						historyIndex = 0
					}
				}
				pos = bestp
			}

		}
		if keyIndex == len(keyPoints)-1 {
			break
		}

		keyIndex++
	}

	// 余ったら追加
	if arrayIndex < gameConst.LenCandidateArray {
		i := 0
		pos := candidateArray[arrayIndex-1]
		goal := gameConst.TravelPoints[i]
		for {

			if arrayIndex == gameConst.LenCandidateArray {
				break
			}
			if pos == goal {
				i++
				goal = gameConst.TravelPoints[i]
			}
			for _, np := range gameConst.Neighbors[pos] {
				if gameConst.DistanceMap[np][goal] == gameConst.DistanceMap[pos][goal]-1 {
					candidateArray[arrayIndex] = np
					arrayIndex++
					pos = np
					break
				}
			}

		}
	}
	if VISUALIZE {
		candidateArray = candidateArray[:gameConst.LenCandidateArray]
		fmt.Println("--------------------------------------------------------------------------------")
		fmt.Println("--------------------------------------------------------------------------------")
		for i := range candidateArray {
			fmt.Printf("%d", candidateArray[i])
			if i != gameConst.LenCandidateArray-1 {
				fmt.Printf(" ")
			} else {
				fmt.Printf("\n")
			}
		}
		for i := range candidateArray {
			if i == 0 {
				continue
			}
			fmt.Println("s", 1, i, 0)
			fmt.Println("m", candidateArray[i])
		}
	}
	if useSA && false {
		sa := NewSA()
		sa.Init(1800*1000, 0, 0, NewSimulatedAnnealingPlan(gameConst.LenCandidateArray), -INF, gameConst.LenCandidateArray, gameConst.MaxGreenNum)
		sa.setInitialPlan(gameConst, candidateArray, arrayIndex)
	}
	for arrayIndex < gameConst.LenCandidateArray {
		candidateArray[arrayIndex] = candidateArray[arrayIndex-1]
		arrayIndex++
	}

	return candidateArray, arrayIndex, ok
}

// ほぼBFS
func getActionsByDijkstra(gameConst *GameConst, availableLength []int, candidateArray []int, inverseCandidateArray [][]int, possibleOperation [][]int, final bool) []*Action {
	maxAction := make([]*Action, 0, 10000)
	groupDistance := make([][]int, gameConst.NumTravelPoints)
	for i := range groupDistance {
		groupDistance[i] = make([]int, gameConst.LenCandidateArray)
		for j := range groupDistance[i] {
			groupDistance[i][j] = INF
		}
	}
	adjacent := make([][]bool, gameConst.LenCandidateArray)
	terminal := make([][]int, gameConst.LenCandidateArray) // group iからgroup jへ移る時の待機点
	for i := range adjacent {
		adjacent[i] = make([]bool, gameConst.LenCandidateArray)
	}
	for i := range terminal {
		terminal[i] = make([]int, gameConst.LenCandidateArray)
		for j := range terminal[i] {
			terminal[i][j] = -1
		}
	}
	// 隣接情報を確認
	for candidateIndex := 0; candidateIndex < gameConst.LenCandidateArray; candidateIndex++ {
		pos := candidateArray[candidateIndex]
		// posが所属するgroup(group1)を考える
		for group1 := candidateIndex - gameConst.MaxGreenNum + 1; group1 <= candidateIndex; group1++ {
			if group1 < 0 {
				continue
			}
			if group1+availableLength[group1] <= candidateIndex {
				continue
			}
			for _, group2 := range possibleOperation[pos] {
				adjacent[group1][group2] = true
				terminal[group1][group2] = pos
			}
		}
	}

	// 辺を構築
	edges := make([][]int, gameConst.LenCandidateArray)
	for i := range edges {
		edges[i] = make([]int, 0, 300)
	}
	revEdges := make([][]int, gameConst.LenCandidateArray)
	for i := range revEdges {
		revEdges[i] = make([]int, 0, 300)
	}
	for i := range adjacent {
		for j := range adjacent[i] {
			if adjacent[i][j] {
				edges[i] = append(edges[i], j)
				revEdges[j] = append(revEdges[j], i)
			}
		}
	}

	queue1 := NewBFSQueue(gameConst.LenCandidateArray)
	queue2 := NewBFSQueue(gameConst.LenCandidateArray)
	progress := 1

	// progressが進む対象のgroupかを保持する isTargetGroup[i]==progressで判定
	prevIsTargetGroup := make([]int, gameConst.LenCandidateArray)
	for i := range prevIsTargetGroup {
		prevIsTargetGroup[i] = -1
	}
	isTargetGroup := make([]int, gameConst.LenCandidateArray)
	for i := range isTargetGroup {
		isTargetGroup[i] = -1
	}

	// specialPointかを保持する
	isSpecialPoint := make([][]int, gameConst.NumTravelPoints)
	for i := range isSpecialPoint {
		isSpecialPoint[i] = make([]int, gameConst.NumCities)
		for j := range isSpecialPoint[i] {
			isSpecialPoint[i][j] = INF
		}
	}
	// specialPointがどのgroupから来たかを保持する
	specialPrevGroup := make([][]int, gameConst.NumTravelPoints)
	for i := range specialPrevGroup {
		specialPrevGroup[i] = make([]int, gameConst.NumCities)
	}
	// specialPointがどのprgから来たかを保持する
	specialPrevPrg := make([][]int, gameConst.NumTravelPoints)
	for i := range specialPrevPrg {
		specialPrevPrg[i] = make([]int, gameConst.NumCities)
	}
	// specialPointがどこを接続点としてから来たかを保持する
	specialPrevTerminal := make([][]int, gameConst.NumTravelPoints)
	for i := range specialPrevTerminal {
		specialPrevTerminal[i] = make([]int, gameConst.NumCities)
	}
	// specialPointはどのoperationを行って来るのかを保持する
	specialPrevOperation := make([][]int, gameConst.NumTravelPoints)
	for i := range specialPrevOperation {
		specialPrevOperation[i] = make([]int, gameConst.NumCities)
	}
	// specialPointがどの信号操作方法で来たかを保持する 変更位置
	specialPrevForward := make([][]bool, gameConst.NumTravelPoints)
	for i := range specialPrevForward {
		specialPrevForward[i] = make([]bool, gameConst.NumCities)
	}
	// specialPointがどの信号操作方法で来たかを保持する 変更長
	specialPrevLength := make([][]int, gameConst.NumTravelPoints)
	for i := range specialPrevLength {
		specialPrevLength[i] = make([]int, gameConst.NumCities)
	}

	for candidateIndex := 0; candidateIndex < gameConst.LenCandidateArray; candidateIndex++ {
		pos := candidateArray[candidateIndex]
		if pos != gameConst.TravelPoints[progress] {
			continue
		}
		for group := candidateIndex - gameConst.MaxGreenNum + 1; group <= candidateIndex; group++ {
			if group < 0 {
				continue
			}
			if group+availableLength[group] <= candidateIndex {
				continue
			}
			isTargetGroup[group] = progress
		}
	}

	distance := 0
	goToNextPoint := false
	for _, v := range possibleOperation[0] {
		groupDistance[progress][v] = distance + 1
		queue2.Push(v)
		if isTargetGroup[v] == progress {
			goToNextPoint = true
		}
	}
	// ここには1<<15以上の値が入ることがある。その場合はspecialpoint由来
	groupPrev := make([][]int, gameConst.NumTravelPoints)
	for i := range groupPrev {
		groupPrev[i] = make([]int, gameConst.LenCandidateArray)
		for j := range groupPrev[i] {
			groupPrev[i][j] = -1
		}
	}
	dataForcalc := make([]int, gameConst.NumCities)
	dataForcalc2 := make([]int, gameConst.NumCities)
	dataForcalc3 := make([]int, gameConst.NumCities)
	greenlightsForcalc := make([]int, gameConst.MaxGreenNum)
	greenlightsForcalc2 := make([]int, gameConst.MaxGreenNum)
	queueForcalc := NewBFSQueue(gameConst.NumCities)
	heapQueueForcalc := NewFakeHeapQueue(gameConst.NumCities, gameConst.MaxGreenNum+1)
	uniqueIdForcalc := -1000

	specialProgress := 0
	checkSpecialPoint := false
	n1 := -1
	for {
		if progress > 100 && n1 == -1 {
			n1 = distance
		}
		// fmt.Fprintln(os.Stderr, distance, queue1.Len(), queue2.Len(), goToNextPoint, progress)
		if distance == 30000 {
			// バグ
			fmt.Fprintln(os.Stderr, candidateArray)
			fmt.Fprintln(os.Stderr, gameConst.TravelPoints[progress], progress)
			for i := range isTargetGroup {
				fmt.Fprintln(os.Stderr, isTargetGroup[i], prevIsTargetGroup[i], groupDistance[progress][i])
			}
		}
		queue1, queue2 = queue2, queue1
		queue2.Init()
		if goToNextPoint {
			// progressを進める
			goToNextPoint = false
			for i := range prevIsTargetGroup {
				prevIsTargetGroup[i] = isTargetGroup[i]
			}
			progress++
			if progress >= gameConst.NumTravelPoints {
				break
			}
			// 次の目標groupを見つける
			for candidateIndex := 0; candidateIndex < gameConst.LenCandidateArray; candidateIndex++ {
				pos := candidateArray[candidateIndex]
				if pos != gameConst.TravelPoints[progress] {
					continue
				}
				for group := candidateIndex - gameConst.MaxGreenNum + 1; group <= candidateIndex; group++ {
					if group < 0 {
						continue
					}
					if group+availableLength[group] <= candidateIndex {
						continue
					}
					isTargetGroup[group] = progress
				}
			}

			// 前回ゴールに到達しているものをそのまま上の階層に持ち上げる
			for queue1.Len() > 0 {
				group := queue1.Pop()
				if prevIsTargetGroup[group] != progress-1 {
					continue
				}
				groupDistance[progress][group] = distance + 1
				groupPrev[progress][group] = group
				queue2.Push(group)
				if isTargetGroup[group] == progress {
					goToNextPoint = true
				}
			}

		} else {
			distance++
			if checkSpecialPoint {
				if specialProgress > progress {
					// specialPointが先行しているのでqueue1は破棄
					queue1.Init()
					progress = specialProgress - 1
					// 次の目標groupを見つける
					for candidateIndex := 0; candidateIndex < gameConst.LenCandidateArray; candidateIndex++ {
						pos := candidateArray[candidateIndex]
						if pos != gameConst.TravelPoints[progress] {
							continue
						}
						for group := candidateIndex - gameConst.MaxGreenNum + 1; group <= candidateIndex; group++ {
							if group < 0 {
								continue
							}
							if group+availableLength[group] <= candidateIndex {
								continue
							}
							isTargetGroup[group] = progress
						}
					}
					for i := range isTargetGroup {
						prevIsTargetGroup[i] = isTargetGroup[i]
					}
					progress++
					for candidateIndex := 0; candidateIndex < gameConst.LenCandidateArray; candidateIndex++ {
						pos := candidateArray[candidateIndex]
						if pos != gameConst.TravelPoints[progress] {
							continue
						}
						for group := candidateIndex - gameConst.MaxGreenNum + 1; group <= candidateIndex; group++ {
							if group < 0 {
								continue
							}
							if group+availableLength[group] <= candidateIndex {
								continue
							}
							isTargetGroup[group] = progress
						}
					}
				} else if specialProgress < progress {
					// specialPointが遅れているのでspecialPointsは破棄
					checkSpecialPoint = false
				}

			}
			for queue1.Len() > 0 {
				group := queue1.Pop()
				for _, v := range edges[group] {
					if groupDistance[progress][v] > distance+1 {
						groupDistance[progress][v] = distance + 1
						groupPrev[progress][v] = group
						queue2.Push(v)
						if isTargetGroup[v] == progress {
							goToNextPoint = true

							if progress < gameConst.NumTravelPoints-1 {
								// specialpointを考える
								// まずusedLengthを求める
								// usedLength := calcOptimalSwitchLength(gameConst)
								uniqueIdForcalc -= 1000
								heapQueueForcalc.Init()
								for i := range greenlightsForcalc {
									greenlightsForcalc[i] = -1
								}
								for i := 0; i < availableLength[group]; i++ {
									greenlightsForcalc[i] = candidateArray[group+i]
								}
								for i := range greenlightsForcalc {
									if greenlightsForcalc[i] == -1 {
										continue
									}
									dataForcalc[greenlightsForcalc[i]] = uniqueIdForcalc + 0
									heapQueueForcalc.Push(greenlightsForcalc[i], 0)
								}
								for i := availableLength[v] - 1; i >= 0; i-- {
									dataForcalc2[candidateArray[v+i]] = uniqueIdForcalc + i
								}
								usedLength := 0

								goal := gameConst.TravelPoints[progress]
								for heapQueueForcalc.Len() > 0 {
									pos, priority := heapQueueForcalc.Pop()
									if pos == goal {
										usedLength = priority + 1
										break
									}
									for _, np := range gameConst.Neighbors[pos] {
										// npがグループvの何番目の要素か取得する
										d := dataForcalc2[np] - uniqueIdForcalc
										if d >= availableLength[v] {
											continue
										}
										newPriority := maxInt(priority, d)
										if dataForcalc[np] > uniqueIdForcalc+newPriority {
											dataForcalc[np] = uniqueIdForcalc + newPriority
											heapQueueForcalc.Push(np, newPriority)
										}
									}
								}
								if usedLength == gameConst.MaxGreenNum {
									// 改善の余地なし
									continue
								}

								forward := true
								for loop := 0; loop < 2; loop++ {
									uniqueIdForcalc -= 1000
									queueForcalc.Init()
									for i := range greenlightsForcalc2 {
										greenlightsForcalc2[i] = greenlightsForcalc[i]
									}
									// 青信号一覧をgreenlightsForCalc2に格納
									// 青信号一覧をdataForCalc2に格納
									if forward {
										for i := 0; i < usedLength; i++ {
											greenlightsForcalc2[i] = candidateArray[v+i]
										}
										for i := 0; i < usedLength; i++ {
											dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc + 2
										}
										for i := usedLength; i < gameConst.MaxGreenNum; i++ {
											if greenlightsForcalc2[i] == -1 {
												continue
											}
											dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc
										}
									} else {
										for i := 0; i < usedLength; i++ {
											greenlightsForcalc2[gameConst.MaxGreenNum-usedLength+i] = candidateArray[v+i]
										}
										for i := gameConst.MaxGreenNum - usedLength; i < gameConst.MaxGreenNum; i++ {
											dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc + 2
										}
										for i := 0; i < gameConst.MaxGreenNum-usedLength; i++ {
											if greenlightsForcalc2[i] == -1 {
												continue
											}
											dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc
										}
									}

									t := -1
									// BFSで到達可能点を求める
									queueForcalc.Push(goal)

									for queueForcalc.Len() > 0 {
										pos := queueForcalc.Pop()
										for _, np := range gameConst.Neighbors[pos] {
											if dataForcalc2[np] == uniqueIdForcalc || dataForcalc2[np] == uniqueIdForcalc+2 {
												if dataForcalc2[np] == uniqueIdForcalc {
													if t == -1 {
														t = np
													}
												}
												dataForcalc2[np] = uniqueIdForcalc + 1
												queueForcalc.Push(np)
											}
										}
									}
									checkSpecialPoint = true
									prg := progress + 1

									for {
										for pos := range dataForcalc2 {
											if dataForcalc2[pos] == uniqueIdForcalc+1 {
												if isSpecialPoint[prg][pos] > distance+1 {
													isSpecialPoint[prg][pos] = distance + 1
													specialPrevPrg[prg][pos] = progress
													specialPrevGroup[prg][pos] = group
													specialPrevTerminal[prg][pos] = t
													specialPrevForward[prg][pos] = forward
													specialPrevLength[prg][pos] = usedLength
													specialPrevOperation[prg][pos] = v
												}
											}
										}
										if dataForcalc2[gameConst.TravelPoints[prg]] != uniqueIdForcalc+1 {
											break
										}
										if prg >= gameConst.NumTravelPoints-1 {
											break
										}
										prg++
									}
									specialProgress = maxInt(specialProgress, prg)

									forward = false
								}
							}
						}
					}

				}
			}
			if checkSpecialPoint {
				for pos := 0; pos < gameConst.NumCities; pos++ {
					if isSpecialPoint[progress][pos] != distance {
						continue
					}
					for _, operation := range possibleOperation[pos] {
						if groupDistance[progress][operation] > distance+1 {
							groupDistance[progress][operation] = distance + 1
							groupPrev[progress][operation] = 1<<15 + pos
							queue2.Push(operation)
							if isTargetGroup[operation] == progress {
								goToNextPoint = true

								if progress < gameConst.NumTravelPoints-1 && final {
									// specialpointを考える

									// まずusedLengthを求める
									// usedLength := calcOptimalSwitchLength(gameConst)
									uniqueIdForcalc -= 1000
									heapQueueForcalc.Init()

									group := 1 << 15
									for i := range greenlightsForcalc {
										greenlightsForcalc[i] = -1
									}
									if true {
										prg := progress
										p := pos
										first := true
										for group >= 1<<15 {
											if first {
												first = false
											} else {
												prg = specialPrevPrg[prg][p]
												p = group ^ (1 << 15)
											}
											overrideIndex := 0
											if !specialPrevForward[prg][p] {
												overrideIndex = gameConst.MaxGreenNum - specialPrevLength[prg][p]
											}
											for i := 0; i < specialPrevLength[prg][p]; i++ {
												if greenlightsForcalc[overrideIndex+i] != -1 {
													continue
												}
												greenlightsForcalc[overrideIndex+i] = candidateArray[specialPrevOperation[prg][p]+i]
											}

											group = specialPrevGroup[prg][p]
										}
									}
									for i := 0; i < availableLength[group]; i++ {
										if greenlightsForcalc[i] != -1 {
											continue
										}
										greenlightsForcalc[i] = candidateArray[group+i]
									}
									// for i := range greenlightsForcalc {
									// 	if greenlightsForcalc[i] == -1 {
									// 		continue
									// 	}
									// 	dataForcalc[greenlightsForcalc[i]] = uniqueIdForcalc + 0
									// 	heapQueueForcalc.Push(greenlightsForcalc[i], 0)
									// }
									dataForcalc[pos] = uniqueIdForcalc + 0
									heapQueueForcalc.Push(pos, 0)
									for i := availableLength[operation] - 1; i >= 0; i-- {
										dataForcalc2[candidateArray[operation+i]] = uniqueIdForcalc + i + 1
									}
									usedLength := 0

									goal := gameConst.TravelPoints[progress]
									for heapQueueForcalc.Len() > 0 {
										p, priority := heapQueueForcalc.Pop()
										if p == goal {
											usedLength = maxInt(priority, 1)
											break
										}
										for _, np := range gameConst.Neighbors[p] {
											// npがグループvの何番目の要素か取得する
											d := dataForcalc2[np] - uniqueIdForcalc
											if d >= availableLength[operation]+1 {
												continue
											}
											newPriority := maxInt(priority, d)
											if dataForcalc[np] > uniqueIdForcalc+newPriority {
												dataForcalc[np] = uniqueIdForcalc + newPriority
												heapQueueForcalc.Push(np, newPriority)
											}
										}
									}
									if usedLength == gameConst.MaxGreenNum {
										// 改善の余地なし
										continue
									}

									forward := true
									for loop := 0; loop < 2; loop++ {
										uniqueIdForcalc -= 1000
										queueForcalc.Init()
										for i := range greenlightsForcalc2 {
											greenlightsForcalc2[i] = greenlightsForcalc[i]
										}
										// 青信号一覧をgreenlightsForCalc2に格納
										// 青信号一覧をdataForCalc2に格納
										if forward {
											for i := 0; i < usedLength; i++ {
												greenlightsForcalc2[i] = candidateArray[operation+i]
											}
											for i := 0; i < usedLength; i++ {
												dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc + 2
											}
											for i := usedLength; i < gameConst.MaxGreenNum; i++ {
												if greenlightsForcalc2[i] == -1 {
													continue
												}
												dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc
											}
										} else {
											for i := 0; i < usedLength; i++ {
												greenlightsForcalc2[gameConst.MaxGreenNum-usedLength+i] = candidateArray[operation+i]
											}
											for i := gameConst.MaxGreenNum - usedLength; i < gameConst.MaxGreenNum; i++ {
												dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc + 2
											}
											for i := 0; i < gameConst.MaxGreenNum-usedLength; i++ {
												if greenlightsForcalc2[i] == -1 {
													continue
												}
												dataForcalc2[greenlightsForcalc2[i]] = uniqueIdForcalc
											}
										}

										// BFSで到達可能点を求める
										dataForcalc3[goal] = uniqueIdForcalc
										queueForcalc.Push(goal)

										for queueForcalc.Len() > 0 {
											pos := queueForcalc.Pop()
											for _, np := range gameConst.Neighbors[pos] {
												if dataForcalc2[np] == uniqueIdForcalc+3 && dataForcalc2[np] == uniqueIdForcalc+0 {
													continue
												}
												if dataForcalc2[np] == uniqueIdForcalc || dataForcalc2[np] == uniqueIdForcalc+2 {
													dataForcalc2[np] = dataForcalc2[np] + 1
													dataForcalc3[np] = dataForcalc3[pos] + 1
													queueForcalc.Push(np)
												}
											}
										}
										checkSpecialPoint = true
										prg := progress + 1
										t := pos

										for {
											for p := range dataForcalc2 {
												if dataForcalc2[p] == uniqueIdForcalc+1 || dataForcalc2[p] == uniqueIdForcalc+3 {
													if isSpecialPoint[prg][p] > distance+1 {
														isSpecialPoint[prg][p] = distance + 1
														specialPrevPrg[prg][p] = progress
														specialPrevGroup[prg][p] = pos + 1<<15
														specialPrevTerminal[prg][p] = t
														specialPrevForward[prg][p] = forward
														specialPrevLength[prg][p] = usedLength
														specialPrevOperation[prg][p] = operation
													}
												}

											}
											if dataForcalc2[gameConst.TravelPoints[prg]] != uniqueIdForcalc+1 {
												break
											}
											if prg >= gameConst.NumTravelPoints-1 {
												break
											}
											prg++
										}
										specialProgress = maxInt(specialProgress, prg)

										forward = false
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// 復元処理
	// 最終グループを取得
	cur := 0
	for {
		cur = queue1.Pop()
		if isTargetGroup[cur] == gameConst.NumTravelPoints-1 {
			break
		}
	}
	prg := gameConst.NumTravelPoints - 1
	optimalOperationsTerminal := make([]int, 0, 5000)

	optimalOperations := make([]int, 0, 5000)
	optimalOperationsTerminal = append(optimalOperationsTerminal, 0)
	optimalOperations = append(optimalOperations, cur)
	optimalOperationsParam1 := make([]int, 0, 5000)
	optimalOperationsParam1 = append(optimalOperationsParam1, availableLength[cur])
	optimalOperationsParam3 := make([]int, 0, 5000)
	optimalOperationsParam3 = append(optimalOperationsParam3, 0)
	for {
		if groupDistance[prg][cur] == 1 {
			break
		}
		prev := groupPrev[prg][cur]
		usedSpecialPoint := prev >= 1<<15
		if usedSpecialPoint {
			// 	supecialPoint
			pprev := 1 << 15
			fixedTerminal := -1
			for pprev >= 1<<15 {
				t := prev ^ (1 << 15)
				fixedTerminal = specialPrevTerminal[prg][t]
				length := specialPrevLength[prg][t]

				index := 0
				if !specialPrevForward[prg][t] {
					index = gameConst.MaxGreenNum - length
				}
				pprev = specialPrevGroup[prg][t]
				prev = specialPrevOperation[prg][t]
				prg = specialPrevPrg[prg][t]
				optimalOperations = append(optimalOperations, prev)
				optimalOperationsParam1 = append(optimalOperationsParam1, length)
				optimalOperationsParam3 = append(optimalOperationsParam3, index)
				optimalOperationsTerminal = append(optimalOperationsTerminal, t)

				prev = pprev
			}

			optimalOperations = append(optimalOperations, pprev)
			optimalOperationsParam1 = append(optimalOperationsParam1, availableLength[pprev])
			optimalOperationsParam3 = append(optimalOperationsParam3, 0)
			optimalOperationsTerminal = append(optimalOperationsTerminal, fixedTerminal)
			cur = pprev

		} else {
			if prev == cur {
				prg--
			} else {
				optimalOperations = append(optimalOperations, prev)
				optimalOperationsParam1 = append(optimalOperationsParam1, availableLength[prev])
				optimalOperationsParam3 = append(optimalOperationsParam3, 0)
				optimalOperationsTerminal = append(optimalOperationsTerminal, terminal[prev][cur])
				cur = prev

			}
		}
	}
	// fmt.Fprintln(os.Stderr, distance)
	// fmt.Fprintln(os.Stderr, len(optimalOperations))
	// fmt.Fprintln(os.Stderr, len(optimalOperationsTerminal))

	// 逆順なので修正する
	for i := range optimalOperations {
		if i >= len(optimalOperations)-1-i {
			break
		}
		optimalOperations[i], optimalOperations[len(optimalOperations)-1-i] = optimalOperations[len(optimalOperations)-1-i], optimalOperations[i]
		optimalOperationsParam1[i], optimalOperationsParam1[len(optimalOperationsParam1)-1-i] = optimalOperationsParam1[len(optimalOperationsParam1)-1-i], optimalOperationsParam1[i]
		optimalOperationsParam3[i], optimalOperationsParam3[len(optimalOperationsParam3)-1-i] = optimalOperationsParam3[len(optimalOperationsParam3)-1-i], optimalOperationsParam3[i]
		optimalOperationsTerminal[i], optimalOperationsTerminal[len(optimalOperationsTerminal)-1-i] = optimalOperationsTerminal[len(optimalOperationsTerminal)-1-i], optimalOperationsTerminal[i]
	}

	distances := make([]int, gameConst.NumCities)
	for i := range distances {
		distances[i] = 10000
	}
	queue := NewBFSQueue(gameConst.NumCities)
	cur = 0
	prg = 1
	step := 0
	reachable := make([]int, gameConst.NumCities)
	for i := range reachable {
		reachable[i] = -1
	}
	greenlights := make([]int, gameConst.MaxGreenNum)
	for i := range greenlights {
		greenlights[i] = -1
	}
	loop := 0
	pattern := 1
	lenReversedMoves := 0
	reversedMoves := make([]int, 30000)
	for {
		loop++

		if pattern == 1 {
			action := NewAction()
			action.ActionType = 0
			action.param1 = optimalOperationsParam1[step]
			action.param2 = optimalOperations[step]
			action.param3 = optimalOperationsParam3[step]
			maxAction = append(maxAction, action)
		}
		if pattern == 0 {
			prg++
		}
		if prg >= gameConst.NumTravelPoints {
			break
		}
		group := optimalOperations[step]
		length := optimalOperationsParam1[step]
		overrideIndex := optimalOperationsParam3[step]
		for i := 0; i < length; i++ {
			greenlights[overrideIndex+i] = candidateArray[group+i]
		}
		for i := range greenlights {
			if greenlights[i] == -1 {
				continue
			}
			reachable[greenlights[i]] = step
		}

		queue.Init()
		queue.Push(cur)
		// fmt.Fprintln(os.Stderr, "cur:", cur)
		distances[cur] = -loop * (1 << 15)
		for queue.Len() > 0 {
			pos := queue.Pop()
			d := distances[pos]
			for _, np := range gameConst.Neighbors[pos] {
				if reachable[np] != step {
					continue
				} else if distances[np] <= d+1 {
					continue
				}
				distances[np] = d + 1
				queue.Push(np)
			}
		}
		goal := gameConst.TravelPoints[prg]
		// パターン1 次の目標点に向かう
		if distances[goal] >= -(loop-1)*(1<<15) {
			// パターン2 次の接続点に向かうへ移行
			goal = optimalOperationsTerminal[step]
			pattern = 1
		} else {
			pattern = 0
		}
		pos := goal
		lenReversedMoves = 0
		ok := true
		for {
			if distances[pos] > -(loop-1)*(1<<15) {
				fmt.Fprintln(os.Stderr, pos, goal, cur)
				ok = false
				break
			}
			if distances[pos] == -(loop)*(1<<15) {
				break
			}
			reversedMoves[lenReversedMoves] = pos
			lenReversedMoves++
			exist := false
			for _, np := range gameConst.Neighbors[pos] {
				// fmt.Fprintln(os.Stderr, np, pos, distances[np], distances[pos])

				if distances[np] == distances[pos]-1 {
					pos = np
					exist = true
					break
				}
			}
			if !exist {
				fmt.Fprintln(os.Stderr, pos, optimalOperations[step], optimalOperations[step+1], terminal[optimalOperations[step]][optimalOperations[step+1]], pattern)
			}
		}
		for i := lenReversedMoves - 1; i >= 0; i-- {
			action := NewAction()
			action.ActionType = 1
			action.param1 = reversedMoves[i]
			maxAction = append(maxAction, action)
		}
		if pattern == 1 {
			step++
		}
		if !ok && step > 100 {
			break
		}
		cur = goal

	}

	return maxAction
}

type SimulatedAnnealingPlan struct {
	travelOrders []int
	// inverseOrders []int
	lenTravel   int
	sumDistance int
	// sumEfficiency float64
}

func NewSimulatedAnnealingPlan(numTravels int) *SimulatedAnnealingPlan {
	plan := &SimulatedAnnealingPlan{}
	plan.travelOrders = make([]int, numTravels)
	// plan.inverseOrders = make([]int, numTravels)
	return plan
}

func (plan *SimulatedAnnealingPlan) Copy() *SimulatedAnnealingPlan {
	newP := NewSimulatedAnnealingPlan(len(plan.travelOrders))
	newP.Load(plan)
	return newP
}
func (plan *SimulatedAnnealingPlan) Load(plan2 *SimulatedAnnealingPlan) {
	for i := range plan.travelOrders {
		plan.travelOrders[i] = plan2.travelOrders[i]
	}
	// for i := range plan.inverseOrders {
	// 	plan.inverseOrders[i] = plan2.inverseOrders[i]
	// }
	plan.lenTravel = plan2.lenTravel
	plan.sumDistance = plan2.sumDistance
	// plan.sumEfficiency = plan2.sumEfficiency
}

func (plan *SimulatedAnnealingPlan) ApplyAction(gameConst *GameConst, index1 int, index2 int) {
	if index1 > index2 {
		index1, index2 = index2, index1
	}
	if index1 > 0 {
		plan.sumDistance -= gameConst.DistanceMap[plan.travelOrders[index1-1]][plan.travelOrders[index1]]
	}
	plan.sumDistance -= gameConst.DistanceMap[plan.travelOrders[index1]][plan.travelOrders[index1+1]]
	if index2 < plan.lenTravel-1 {
		plan.sumDistance -= gameConst.DistanceMap[plan.travelOrders[index2]][plan.travelOrders[index2+1]]
	}
	plan.sumDistance -= gameConst.DistanceMap[plan.travelOrders[index2-1]][plan.travelOrders[index2]]

	// 反転
	i1 := index1
	i2 := index2
	for i1 < i2 {
		plan.travelOrders[i1], plan.travelOrders[i2] = plan.travelOrders[i2], plan.travelOrders[i1]
		i1++
		i2--
	}
	// for i := index1; i <= index2; i++ {
	// 	plan.inverseOrders[plan.travelOrders[i]] = i
	// }

	if index1 > 0 {
		plan.sumDistance += gameConst.DistanceMap[plan.travelOrders[index1-1]][plan.travelOrders[index1]]
	}
	plan.sumDistance += gameConst.DistanceMap[plan.travelOrders[index1]][plan.travelOrders[index1+1]]
	if index2 < plan.lenTravel-1 {
		plan.sumDistance += gameConst.DistanceMap[plan.travelOrders[index2]][plan.travelOrders[index2+1]]
	}
	plan.sumDistance += gameConst.DistanceMap[plan.travelOrders[index2-1]][plan.travelOrders[index2]]

}

func (plan *SimulatedAnnealingPlan) RevertAction(gameConst *GameConst, index1 int, index2 int) {
	plan.ApplyAction(gameConst, index1, index2)
}

// func (plan *SimulatedAnnealingPlan) SetSumEfficiency(gameConst *GameConst, scale int) {
// 	sc := 0.0
// 	for i := range plan.travelOrders {
// 		sc += plan.CalcEfficiency(gameConst, i, scale)
// 	}
// 	sc /= 2
// 	plan.sumEfficiency = sc
// }

func (plan *SimulatedAnnealingPlan) Evaluate(gameConst *GameConst, border int, scale int) float64 {
	value := 0.0
	if plan.sumDistance > border {
		value -= float64(plan.sumDistance - border)
	}
	if plan.sumDistance > border-20 {
		value -= float64(plan.sumDistance-(border-20)) * 0.1
	}
	// for travelIndex := 0; travelIndex < gameConst.NumTravelPoints; travelIndex++ {
	// 	if travelIndex == 0 {
	// 		continue
	// 	}
	// 	prevTravelIndex := travelIndex - 1
	// 	if absInt(plan.inverseOrders[prevTravelIndex]-plan.inverseOrders[travelIndex]) >= 10 {
	// 		continue
	// 	}
	// 	index1 := plan.inverseOrders[prevTravelIndex]
	// 	index2 := plan.inverseOrders[travelIndex]
	// 	if index1 > index2 {
	// 		index1, index2 = index2, index1
	// 	}
	// 	d := 0
	// 	for i := index1; i < index2; i++ {
	// 		d += gameConst.DistanceMap[gameConst.TravelPoints[plan.travelOrders[i]]][gameConst.TravelPoints[plan.travelOrders[i+1]]]
	// 	}
	// 	if d*5 <= gameConst.DistanceMap[gameConst.TravelPoints[prevTravelIndex]][gameConst.TravelPoints[travelIndex]]*5 {
	// 		value += float64(gameConst.DistanceMap[gameConst.TravelPoints[prevTravelIndex]][gameConst.TravelPoints[travelIndex]]) * 0.03
	// 	}
	// }
	for index := 0; index < gameConst.NumTravelPoints; index++ {
		if index == 0 {
			continue
		}
		if index >= gameConst.NumTravelPoints-1 {
			continue
		}
		pPos := gameConst.TravelPoints[plan.travelOrders[index-1]]
		cPos := gameConst.TravelPoints[plan.travelOrders[index]]
		d1 := 0
		d1 += gameConst.DistanceMap[pPos][cPos]
		index2 := index
		nPos := -1
		for {
			newcPos := gameConst.TravelPoints[plan.travelOrders[index2]]
			nPos = gameConst.TravelPoints[plan.travelOrders[index2+1]]
			d1 += gameConst.DistanceMap[newcPos][nPos]
			if d1 > scale {
				break
			}
			if index2 >= gameConst.NumTravelPoints-2 {
				break
			}
			index2++

		}

		d2 := 0
		d2 += gameConst.DistanceMap[pPos][nPos]
		value -= float64(d1-d2) / float64(maxInt(d2, 1)) * 0.1
		// value += float64(d2) / float64(maxInt(d1, 1)) * 0.1
		index = index2 + 1
	}
	return value
}

// func (plan *SimulatedAnnealingPlan) CalcPartEfficiency(gameConst *GameConst, index1 int, index2 int, scale int) float64 {
// 	sc := plan.CalcEfficiency(gameConst, index1, scale)
// 	sc += plan.CalcEfficiency(gameConst, index2, scale)
// 	return sc
// }

// func (plan *SimulatedAnnealingPlan) CalcEfficiency(gameConst *GameConst, index int, scale int) float64 {
// 	first := true
// 	d := 0
// 	sc := 0.0
// 	for i := index - scale; i <= index; i++ {
// 		if i < 0 {
// 			continue
// 		}
// 		if first {
// 			for l := 0; l < scale; l++ {
// 				d += gameConst.DistanceMap[plan.travelOrders[i+l]][plan.travelOrders[i+l+1]]
// 			}
// 			first = false
// 		}
// 		sc -= float64(d - gameConst.DistanceMap[plan.travelOrders[i]][plan.travelOrders[i+scale]])
//
// 		if i+scale >= lenTravels-1 {
// 			break
// 		}
// 		// 差分
// 		d -= gameConst.DistanceMap[plan.travelOrders[i]][plan.travelOrders[i+1]]
// 		d += gameConst.DistanceMap[plan.travelOrders[i+scale]][plan.travelOrders[i+scale+1]]
// 	}
// 	return sc
// }

type SimulatedAnnealing struct {
	startTime             time.Time
	duration              int     // 焼きなまし実行時間　microsecond
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	scale  int
	border int

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

func (sa *SimulatedAnnealing) setInitialPlan(gameConst *GameConst, travelOrders []int, lenTravel int) {
	plan := NewSimulatedAnnealingPlan(len(travelOrders))
	plan.travelOrders = travelOrders
	plan.lenTravel = lenTravel
	// for i := range plan.travelOrders {
	// 	plan.inverseOrders[plan.travelOrders[i]] = i
	// }
	plan.sumDistance = 0
	for i := range travelOrders {
		if i == plan.lenTravel-1 {
			break
		}
		plan.sumDistance += gameConst.DistanceMap[plan.travelOrders[i]][travelOrders[i+1]]
	}
	// plan.SetSumEfficiency(gameConst, sa.scale)
	sa.best = plan
	// sa.bestScore = plan.Evaluate(gameConst, plan.sumEfficiency)
	sa.bestScore = plan.Evaluate(gameConst, sa.border, sa.scale)
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

func (sa *SimulatedAnnealing) Init(duration int, startTemp float64, endTemp float64, best *SimulatedAnnealingPlan, bestScore float64, border int, scale int) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp

	sa.duration = duration
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy()
	sa.nowScore = bestScore
	sa.scale = scale
	sa.border = border

}

func (sa *SimulatedAnnealing) Run(gameConst *GameConst) {
	prop := 0.0
	sa.updateTempIntervalExp = 12
	sa.now = sa.best.Copy()
	sa.nowScore = sa.bestScore
	for {
		if (sa.sumIter & (1<<sa.updateTempIntervalExp - 1)) == 0 {
			prop = float64(time.Since(sa.startTime).Microseconds()) / float64(sa.duration)
			if prop >= 1 {
				// time up
				break
			}
			// if DEBUG {
			// 	fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sa.sumIter, sa.bestScore)
			// }
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
		index1 := gameConst.Random.Intn(sa.now.lenTravel)
		index2 := gameConst.Random.Intn(sa.now.lenTravel)
		if absInt(index1-index2) <= sa.scale {
			continue
		}
		// 差分が先に計算できる場合はこちらを採用
		// scoreDiff := sa.now.EvaluateDiff(gameConst, action)
		// p := sa.getAcceptRateFromDiff(scoreDiff, sa.bestScore)
		// r := gameConst.Random.Float64()
		// if p < r {
		// 	continue
		// }
		// score := sa.nowScore + scoreDiff
		// efficiency := sa.now.sumEfficiency
		// efficiency -= sa.now.CalcPartEfficiency(gameConst, index1, index2, sa.scale)
		sa.now.ApplyAction(gameConst, index1, index2)
		// efficiency += sa.now.CalcPartEfficiency(gameConst, index1, index2, sa.scale)
		sa.valid++
		// score := sa.now.Evaluate(gameConst, efficiency)
		score := sa.now.Evaluate(gameConst, sa.border, sa.scale)
		p := sa.getAcceptRate(score, sa.nowScore)
		// p := sa.getAcceptRate(score, sa.bestScore)
		r := gameConst.Random.Float64()
		if p >= r {
			// 採用
			sa.accepted++
			sa.nowScore = score
			// sa.now.sumEfficiency = efficiency
			if sa.isBest(score, sa.bestScore) {
				// 最善解を変更
				sa.best = sa.now.Copy()
				sa.bestScore = sa.nowScore
			}
		} else {
			// rollback（nowを戻す）
			sa.now.RevertAction(gameConst, index1, index2)
		}
	}
	fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}

func makePathOrders(gameConst *GameConst) []int {
	if gameConst.NumRoads > 1100 || gameConst.MaxGreenNum > 12 {
		travelDistances := make([]int, gameConst.NumTravelPoints)
		for i := range travelDistances {
			from := 0
			if i == 0 {
				from = 0
			} else {
				from = gameConst.TravelPoints[i-1]
			}
			to := gameConst.TravelPoints[i]
			travelDistances[i] = gameConst.DistanceMap[from][to]
		}

		if RANDOMIZE {
			for i := range travelDistances {
				travelDistances[i] += gameConst.Random.Intn(10)
			}
		}
		pathorders := make([]int, gameConst.NumTravelPoints)
		for i := range pathorders {
			pathorders[i] = i
		}
		sort.Slice(pathorders, func(i, j int) bool { return travelDistances[pathorders[i]] > travelDistances[pathorders[j]] })
		return pathorders
	} else {

		pathorders := make([]int, gameConst.NumTravelPoints)
		biasedDistances := make([][]int, gameConst.NumCities)
		for i := range biasedDistances {
			biasedDistances[i] = make([]int, gameConst.NumCities)
			for j := range biasedDistances[i] {
				biasedDistances[i][j] = gameConst.DistanceMap[i][j] * gameConst.MaxGreenNum
			}
		}
		estimatedPath := make([][]int, gameConst.NumTravelPoints)
		for travelIndex := range estimatedPath {
			if travelIndex == 0 {
				continue
			}
			prev := gameConst.TravelPoints[travelIndex-1]
			goal := gameConst.TravelPoints[travelIndex]
			estimatedPath[travelIndex] = make([]int, gameConst.DistanceMap[prev][goal])
			estimatedPath[travelIndex] = append(estimatedPath[travelIndex], prev)
			for prev != goal {
				for _, np := range gameConst.Neighbors[prev] {
					if gameConst.DistanceMap[np][goal] == gameConst.DistanceMap[prev][goal]-1 {
						prev = np
						estimatedPath[travelIndex] = append(estimatedPath[travelIndex], prev)
					}
				}
			}
		}
		pathordersIndex := 0
		used := make([]bool, gameConst.NumTravelPoints)
		for {
			longestPathLength := float64(-INF)
			longestPath := -1
			for travelIndex := 0; travelIndex < gameConst.NumTravelPoints; travelIndex++ {
				if used[travelIndex] {
					continue
				}
				pathLength := 0.0
				if travelIndex == 0 {
					pathLength = -1
				} else {
					pathLength = float64(biasedDistances[gameConst.TravelPoints[travelIndex-1]][gameConst.TravelPoints[travelIndex]])
				}
				if RANDOMIZE {
					pathLength *= (0.6 + gameConst.Random.Float64()*0.8)
				}
				if pathLength > longestPathLength {
					longestPathLength = pathLength
					longestPath = travelIndex
				}
			}
			// if longestPath != 0 {
			// 	fmt.Fprintln(os.Stderr, longestPathLength/float64(gameConst.MaxGreenNum), gameConst.DistanceMap[gameConst.TravelPoints[longestPath-1]][gameConst.TravelPoints[longestPath]])
			// }
			pathorders[pathordersIndex] = longestPath
			used[longestPath] = true

			if pathordersIndex == gameConst.NumTravelPoints-1 {
				break
			}
			if pathordersIndex < 15 {

				// 距離更新
				for pos := 0; pos < gameConst.NumCities; pos++ {
					dFromPath := INF
					minP := -1
					for _, pos2 := range estimatedPath[longestPath] {
						if dFromPath > biasedDistances[pos][pos2] {
							dFromPath = biasedDistances[pos][pos2]
							minP = pos2
						}
					}

					for _, pos2 := range estimatedPath[longestPath] {
						if biasedDistances[pos][pos2] < dFromPath+gameConst.MaxGreenNum+gameConst.DistanceMap[minP][pos2] {
							continue
						}
						for toPos := 0; toPos < gameConst.NumCities; toPos++ {
							biasedDistances[pos][toPos] = minInt(biasedDistances[pos][toPos], dFromPath+gameConst.MaxGreenNum+gameConst.DistanceMap[minP][pos2]+biasedDistances[pos2][toPos])
						}
					}
				}
			}

			pathordersIndex++
		}
		return pathorders
	}

}
