// last local submission: 1703590478 (1800)
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
	INF = 10000000000
	// Main
	TIME_LIMIT = 1950 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01

	SAMPLE_SIZE   = 1000
	MAX_DEPTH     = 16
	LEARN_TURN    = 10
	TURNING_POINT = T - MAX_DEPTH
	NUM_PLAYOUT   = 100
	MIN_PLAYOUT   = NUM_PLAYOUT
	ADJUST_RATE   = 0.005

	T = 1000

	MINIMIZE = false

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = true  // 複数ケースを試すか
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 3000
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FOLDER         = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "0002"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = true
	PRINT_ACTION       = true // 標準出力へ行動を出力するか
	REPRODUCE          = false

	VERSION_MAJOR = 3          // 基本戦術、方針レベルの違い
	VERSION_MINOR = 0          // 改善等
	VERSION_PATCH = 4          // バグ修正,パラメータ変更など
	VERSION_NAME  = "Speed Up" // バージョン名
	VERSION_VALID = true       // 提出に利用できる正当なものか

)

type RandGenerator struct {
	x uint32
}

func NewRandGenerator(seed int) *RandGenerator {
	rg := RandGenerator{}
	rg.x = uint32(seed)
	return &rg
}

func (rg *RandGenerator) Next() uint32 {
	// 32bit乱数を生成
	rg.x ^= rg.x << 13
	rg.x ^= rg.x >> 17
	rg.x ^= rg.x << 5

	return rg.x
}

func (rg *RandGenerator) Intn(n int) int {
	a := (int(rg.Next()) * n) >> 32
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

func getInitialInputs(gConst *GameConst) *GameState {
	// 例
	gConst.Scanner.Scan()
	splitedInput := strings.Split(gConst.Scanner.Text(), " ")
	N, _ := strconv.Atoi(splitedInput[0])
	M, _ := strconv.Atoi(splitedInput[1])
	K, _ := strconv.Atoi(splitedInput[2])
	gConst.N = N
	gConst.M = M
	gConst.K = K
	// T, _ := strconv.Atoi(splitedInput[3])
	gState := NewGameState(gConst)
	if LOCAL_TEST {
		gConst.TrueDataForLocal = NewTrueData(M, K)

		for i := 0; i < M; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			h, _ := strconv.Atoi(splitedInput[0])
			v, _ := strconv.Atoi(splitedInput[1])
			gState.Projects[i].Capacity = h
			gState.Projects[i].Reward = v
			gState.Projects[i].Progress = 0
		}
		// 今後得られるProjectを取得
		for i := 0; i < T*M; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			h, _ := strconv.Atoi(splitedInput[0])
			v, _ := strconv.Atoi(splitedInput[1])
			gConst.TrueDataForLocal.Projects[i].Capacity = h
			gConst.TrueDataForLocal.Projects[i].Reward = v
			gConst.TrueDataForLocal.Projects[i].Progress = 0
		}

		for i := 0; i < N; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			t, _ := strconv.Atoi(splitedInput[0])
			w, _ := strconv.Atoi(splitedInput[1])
			gState.Cards[i].Type = t
			gState.Cards[i].Value = w
			// gState.TypeCount[t]++
		}
		// 今後得られるCardを取得
		for i := 0; i < T*K; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			t, _ := strconv.Atoi(splitedInput[0])
			w, _ := strconv.Atoi(splitedInput[1])
			p, _ := strconv.Atoi(splitedInput[2])
			gConst.TrueDataForLocal.Cards[i].Type = t
			gConst.TrueDataForLocal.Cards[i].Value = w
			gConst.TrueDataForLocal.Prices[i] = p
		}
	} else {
		// 本番はcard,projectの順で与えられる
		for i := 0; i < N; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			t, _ := strconv.Atoi(splitedInput[0])
			w, _ := strconv.Atoi(splitedInput[1])
			gState.Cards[i].Type = t
			gState.Cards[i].Value = w
		}
		for i := 0; i < M; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			h, _ := strconv.Atoi(splitedInput[0])
			v, _ := strconv.Atoi(splitedInput[1])
			gState.Projects[i].Capacity = h
			gState.Projects[i].Reward = v
			gState.Projects[i].Progress = 0
		}
	}
	for i := range gState.Cards {
		gState.Power += gState.Cards[i].Value
		gState.ParamHistory = append(gState.ParamHistory, gState.Cards[i].Type)
	}
	// link
	for j := range gState.Cards {
		for i := j - 1; i >= 0; i-- {
			if gState.Cards[i].Type == gState.Cards[j].Type {
				if gState.Cards[i].Value == gState.Cards[j].Value {
					gState.Link(i, j)
					break
				}
			}
		}
	}
	return gState
}

func getInputs(gConst *GameConst, gState *GameState) {
	for i := 0; i < gConst.M; i++ {
		changed := false
		if gState.Projects[i].Progress == -1 {
			changed = true
		}
		gConst.Scanner.Scan()
		splitedInput := strings.Split(gConst.Scanner.Text(), " ")
		h, _ := strconv.Atoi(splitedInput[0])
		v, _ := strconv.Atoi(splitedInput[1])
		if changed {
			gState.Projects[i].Capacity = h
			gState.Projects[i].Reward = v
			gState.Projects[i].Progress = 0
		}
	}
	gConst.Scanner.Scan()
	money, _ := strconv.Atoi(gConst.Scanner.Text())
	if money != gState.Money {
		panic("")
	}
	gState.Money = money
}

func getCandidates(gConst *GameConst, gState *GameState) ([]Card, []int) {
	candidates := make([]Card, 0, gConst.K)
	prices := make([]int, 0, gConst.K)
	if LOCAL_TEST {
		for i := 0; i < gConst.K; i++ {
			index := gState.Turn2*gConst.K + i
			candidates = append(candidates, gConst.TrueDataForLocal.Cards[index])
			prices = append(prices, gConst.TrueDataForLocal.Prices[index])
			candidates[i].Value = candidates[i].Value << gState.L
			prices[i] = prices[i] << gState.L
		}

	} else {
		for i := 0; i < gConst.K; i++ {
			gConst.Scanner.Scan()
			splitedInput := strings.Split(gConst.Scanner.Text(), " ")
			t, _ := strconv.Atoi(splitedInput[0])
			w, _ := strconv.Atoi(splitedInput[1])
			p, _ := strconv.Atoi(splitedInput[2])
			candidates = append(candidates, Card{Type: t, Value: w})
			prices = append(prices, p)
		}
	}
	for i := 1; i < gConst.K; i++ {
		gState.EstimatedParam[candidates[i].Type]++
		gState.ParamHistory = append(gState.ParamHistory, candidates[i].Type)
	}
	return candidates, prices
}

func getCandidatesByEstimate(gConst *GameConst, gState *GameState, randForCard *RandGenerator, candidates []Card, prices []int, end int) ([]Card, []int) {
	candidates[0].Type = 0
	candidates[0].Value = 1
	prices[0] = 0
	candidates[0].Value = candidates[0].Value << gState.L
	prices[0] = prices[0] << gState.L

	for i := 1; i < gConst.K; i++ {
		// sumParam := 0
		// for j := range gState.EstimatedParam {
		// 	sumParam += gState.EstimatedParam[j]
		// }
		// cSum := 0
		// r := randForCard.Intn(sumParam)
		// cardType := -1
		// for j := range gState.EstimatedParam {
		// 	cSum += gState.EstimatedParam[j]
		// 	if r < cSum {
		// 		cardType = j
		// 		break
		// 	}
		// }
		cardType := gState.ParamHistory[randForCard.Intn(len(gState.ParamHistory))]
		if gState.Turn1 == end-1 && i == 1 {
			cardType = 4
		}
		index := randForCard.Intn(SAMPLE_SIZE) + cardType*SAMPLE_SIZE
		candidates[i].Type = cardType
		candidates[i].Value = gConst.Sample.Cards[index].Value
		prices[i] = gConst.Sample.Prices[index]
		candidates[i].Value = candidates[i].Value << gState.L
		prices[i] = prices[i] << gState.L
	}

	return candidates, prices
}

func updateProject(gConst *GameConst, gState *GameState) {
	for i := 0; i < gConst.M; i++ {
		if gState.Projects[i].Progress == -1 {
			gState.Projects[i] = gConst.TrueDataForLocal.Projects[gState.ProjectIndex]
			gState.Projects[i].Capacity = gState.Projects[i].Capacity << gState.L
			gState.Projects[i].Reward = gState.Projects[i].Reward << gState.L
			gState.ProjectIndex++
		}
	}
}
func updateProjectByEstimate(gConst *GameConst, gState *GameState, randForProject *RandGenerator) {
	for i := 0; i < gConst.M; i++ {
		if gState.Projects[i].Progress == -1 {
			index := randForProject.Intn(len(gConst.Sample.Projects))
			gState.Projects[i] = gConst.Sample.Projects[index]
			gState.Projects[i].Capacity = gState.Projects[i].Capacity << gState.L
			gState.Projects[i].Reward = gState.Projects[i].Reward << gState.L
		}
	}
}

type Queue interface {
	Init()
	Pop() int
	Push()
	Len() int
}

// 最大lengthがわかっている場合のqueue
type BFSQueue struct {
	queue     []int // BFSの対象をなんらかのindexで持っておく想定 距離の場合はposなど
	popIndex  int
	pushIndex int
}

func NewBFSQueue(maxLength int) *BFSQueue {
	q := BFSQueue{}
	q.queue = make([]int, maxLength)
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

type Stack struct {
	stack []int
	index int
}

func NewStack(maxLength int) *Stack {
	s := Stack{}
	s.stack = make([]int, maxLength)
	return &s
}

func (s *Stack) Init() {
	s.index = 0
}

func (s *Stack) Pop() int {
	v := s.stack[s.index]
	s.index--
	return v
}

func (s *Stack) Push(v int) {
	s.index++
	s.stack[s.index] = v
}

func (s *Stack) Len() int {
	return s.index
}

type SampleData struct {
	Projects []Project
	Cards    []Card // 5*samplesize
	Prices   []int
}

func NewSampleData(sampleSize int) *SampleData {
	sd := SampleData{}
	sd.Projects = make([]Project, sampleSize)
	sd.Cards = make([]Card, 5*sampleSize)
	sd.Prices = make([]int, 5*sampleSize)

	return &sd
}

func (sd *SampleData) GenerateSample(gConst *GameConst) {
	for i := range sd.Projects {
		b := gConst.Random.Float64()*6 + 2
		b2 := gConst.Random.NormFloat64()*0.5 + b
		b2 = math.Max(b2, 0.0)
		b2 = math.Min(b2, 10.0)
		sd.Projects[i].Capacity = int(math.Round(math.Pow(2, b)))
		sd.Projects[i].Reward = int(math.Round(math.Pow(2, b2)))
	}
	for cardtype := 0; cardtype < 5; cardtype++ {
		for i := 0; i < SAMPLE_SIZE; i++ {
			w := 0
			if cardtype == 0 || cardtype == 1 {
				w = gConst.Random.Intn(50) + 1
			}
			p := 0
			switch cardtype {
			case 0:
				p = int(math.Round(gConst.Random.NormFloat64()*float64(w)/3)) + w
				p = maxInt(p, 1)
				p = minInt(p, 10000)
			case 1:
				p = int(math.Round(gConst.Random.NormFloat64()*float64(w*gConst.M)/3)) + w*gConst.M
				p = maxInt(p, 1)
				p = minInt(p, 10000)
			case 2:
				p = gConst.Random.Intn(11)
			case 3:
				p = gConst.Random.Intn(11)
			case 4:
				p = gConst.Random.Intn(801) + 200
			}
			sd.Cards[i+cardtype*SAMPLE_SIZE].Type = cardtype
			sd.Cards[i+cardtype*SAMPLE_SIZE].Value = w
			sd.Prices[i+cardtype*SAMPLE_SIZE] = p
		}
	}

}

// 現在の状態を記録するもの
type GameState struct {
	Projects []Project
	Cards    []Card
	// TypeCount      [5]int
	Power          int
	Money          int
	L              int
	EstimatedParam [5]int
	ParamHistory   []int // コピーしない
	Adjust         [5]float64
	numPlayout     int
	prev           []int
	next           []int

	// LocalTestでのみ使用されるもの
	ProjectIndex int

	Turn1 int
	Turn2 int
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	g.Projects = make([]Project, gConst.M)
	g.Cards = make([]Card, gConst.N)
	g.EstimatedParam[0] = 0
	g.EstimatedParam[1] = 0
	g.EstimatedParam[2] = 0
	g.EstimatedParam[3] = 0
	g.EstimatedParam[4] = 0
	g.numPlayout = NUM_PLAYOUT
	g.ParamHistory = []int{}
	g.prev = make([]int, gConst.N)
	g.next = make([]int, gConst.N)
	for i := range g.prev {
		g.prev[i] = -1
		g.next[i] = -1
	}
	// g.EstimatedParam[0] = 654
	// g.EstimatedParam[1] = 509
	// g.EstimatedParam[2] = 105
	// g.EstimatedParam[3] = 547
	// g.EstimatedParam[4] = 210
	return &g
}

func (g *GameState) Load(g2 *GameState) {
	for i := range g.Projects {
		g.Projects[i].Capacity = g2.Projects[i].Capacity
		g.Projects[i].Progress = g2.Projects[i].Progress
		g.Projects[i].Reward = g2.Projects[i].Reward
	}
	for i := range g.Cards {
		g.Cards[i].Type = g2.Cards[i].Type
		g.Cards[i].Value = g2.Cards[i].Value
	}
	for i := range g.Adjust {
		g.Adjust[i] = g2.Adjust[i]
	}
	for i := range g.next {
		g.next[i] = g2.next[i]
	}
	for i := range g.prev {
		g.prev[i] = g2.prev[i]
	}
	// for i := range g.TypeCount {
	// 	g.TypeCount[i] = g2.TypeCount[i]
	// }
	g.Power = g2.Power
	g.Money = g2.Money
	g.numPlayout = g2.numPlayout
	g.L = g2.L
	g.EstimatedParam = g2.EstimatedParam
	g.ProjectIndex = g2.ProjectIndex
	g.Turn1 = g2.Turn1
	g.Turn2 = g2.Turn2
	g.ParamHistory = g2.ParamHistory
}

func (g *GameState) Link(i, j int) {
	// iの後ろにjをつなげる（割り込ませる）操作
	k := g.next[i]
	g.next[j] = k
	g.next[i] = j
	if k != -1 {
		g.prev[k] = j
	}
	g.prev[j] = i

}
func (g *GameState) Link2(j, k int) {
	// kの前にjをつなげる操作
	// kは先頭とする
	g.next[j] = k
	g.prev[k] = j

}

func (g *GameState) Cut(j int) {
	// jを消去する操作
	i := g.prev[j]
	k := g.next[j]
	if i != -1 {
		g.next[i] = k
	}
	if k != -1 {
		g.prev[k] = i
	}
	g.prev[j] = -1
	g.next[j] = -1
}
func (g *GameState) ApplyAction1(action *Action, gameConst *GameConst) {
	// カード消費
	cardType := g.Cards[action.Index].Type
	param := action.Param
	switch cardType {
	case 0:
		// 通常労働
		// param: 対象プロジェクト
		subject := param
		g.Projects[subject].Progress += g.Cards[action.Index].Value
		if g.Projects[subject].Capacity <= g.Projects[subject].Progress {
			g.Projects[subject].Progress = -1
			g.Money += g.Projects[subject].Reward
		}

	case 1:
		// 全力労働
		for subject := range g.Projects {
			g.Projects[subject].Progress += g.Cards[action.Index].Value
			if g.Projects[subject].Capacity <= g.Projects[subject].Progress {
				g.Projects[subject].Progress = -1
				g.Money += g.Projects[subject].Reward
			}
		}
	case 2:
		// キャンセル
		// param: 対象プロジェクト
		subject := param
		g.Projects[subject].Progress = -1
	case 3:
		// 業務転換
		for subject := range g.Projects {
			g.Projects[subject].Progress = -1
		}
	case 4:
		// 増資
		g.L++
		if g.L > 20 {
			panic("")
		}

	}
	g.Power -= g.Cards[action.Index].Value
	// g.TypeCount[cardType]--
	g.Cut(action.Index)
	g.Cards[action.Index].Type = -1
	g.Turn1++
}

func (g *GameState) ApplyAction2(action *Action, gameConst *GameConst, candidates []Card, prices []int) {
	// カード獲得
	index := -1
	prev := -1
	next := -1
	for i := range g.Cards {
		if g.Cards[i].Type == -1 {
			index = i
			break
		}
		if g.Cards[i].Type == candidates[action.Index].Type {
			if g.Cards[i].Value == candidates[action.Index].Value {
				prev = i
			}
		}
	}
	if prev == -1 {
		for k := index + 1; k < gameConst.N; k++ {
			if g.Cards[k].Type == candidates[action.Index].Type {
				if g.Cards[k].Value == candidates[action.Index].Value {
					next = k
					break
				}
			}
		}
	}
	g.Cards[index] = candidates[action.Index]
	// g.TypeCount[g.Cards[index].Type]++
	if prev != -1 {
		g.Link(prev, index)
	} else if next != -1 {
		g.Link2(index, next)

	}
	g.Power += candidates[action.Index].Value
	g.Money -= prices[action.Index]
	g.Turn2++
}

// 状態を何らかの指標で数値化する
func (g *GameState) CalcAssets(gConst *GameConst) float64 {
	assets := 0.0
	assets += float64(g.Money)
	if g.Turn1 == T {
		assets *= 5
	}
	for i := range g.Cards {
		if g.Cards[i].Type == 0 || g.Cards[i].Type == 1 {
			v := float64(g.Cards[i].Value)
			if g.Cards[i].Type == 1 {
				v *= float64(gConst.M) * 0.8
			}
			assets += v
		}
	}
	for i := range g.Projects {
		if g.Projects[i].Progress == -1 {
			continue
		}
		prop := (float64(g.Power) + float64(g.Money)) / float64(g.Projects[i].Capacity-g.Projects[i].Progress)
		prop = math.Min(prop, 1)
		v := float64(g.Projects[i].Progress) * float64(g.Projects[i].Reward) / float64(g.Projects[i].Capacity) * prop
		// v := (float64(g.Projects[i].Reward) - float64(g.Projects[i].Capacity-g.Projects[i].Progress)) * 0.8

		assets += v
	}
	assets += float64((T - g.Turn1) * (3 << g.L))
	return assets
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	N int
	M int
	K int

	Sample           *SampleData
	TrueDataForLocal *TrueData

	Random  *rand.Rand
	Scanner *bufio.Scanner
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func (g *GameConst) SetSample() {
	g.Sample = NewSampleData(SAMPLE_SIZE)
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	// g.Random = rand.New(rand.NewSource(runner.randSeed))
	g.Random = rand.New(rand.NewSource(100))
	return &g
}

type TrueData struct {
	Projects []Project
	Cards    []Card
	Prices   []int
}

func NewTrueData(m, k int) *TrueData {
	d := TrueData{}
	d.Projects = make([]Project, m*T)
	d.Cards = make([]Card, k*T)
	d.Prices = make([]int, k*T)
	return &d
}

type Project struct {
	Capacity int
	Reward   int
	Progress int
}
type Card struct {
	Type  int
	Value int
}

func printAction(action *Action, gameState *GameState, gameConst *GameConst, comment bool) {
	useParam := false
	if action.ActionType == 0 {
		useParam = true
	}
	if useParam {
		fmt.Println(action.Index, action.Param)
	} else {
		fmt.Println(action.Index)

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
	ActionType int //0: use 1:get
	Index      int
	Param      int
}

func NewAction() *Action {
	a := Action{}
	return &a
}
func NewUseAction() *Action {
	a := NewAction()
	return a
}
func NewGetAction() *Action {
	a := NewAction()
	a.ActionType = 1
	return a
}

func (a *Action) Override(a2 *Action) {

}
func (a *Action) Copy() *Action {
	newa := NewAction()
	newa.Override(a)
	return newa
}

/*func (a *Action) addAction() {
}*/

type SimulatedAnnealing struct {
	startTime             time.Time
	duration              int     // 焼きなまし実行時間　microsecond
	startTemp             float64 // 開始時の温度
	endTemp               float64 // 終了時の温度
	minimize              bool    // 最小化？
	updateTempIntervalExp int     // 温度の更新間隔 1<<updateTempIntervalexp が間隔になる

	// 状態
	sumIter   int     // loopの回った回数
	valid     int     // validな解を得た回数
	accepted  int     // 採用された回数
	temp      float64 // 現在の温度
	best      *Action // これまでの最善解
	bestScore float64 // 最善解のスコア
	now       *Action // 現在の遷移元
	nowScore  float64 // 現在の遷移元のスコア
}

func NewSA() *SimulatedAnnealing {
	sa := SimulatedAnnealing{}
	return &sa
}

func (sa *SimulatedAnnealing) getAcceptRate(score float64, bestScore float64) float64 {
	diff := score - bestScore
	if sa.minimize {
		diff *= -1
	}
	p := math.Exp(diff / sa.temp)
	return p
}

func (sa *SimulatedAnnealing) isBest(score float64, bestScore float64) bool {
	if sa.minimize {
		return score < bestScore
	} else {
		return score > bestScore
	}
}

func (sa *SimulatedAnnealing) Init(duration int, startTemp float64, endTemp float64, minimize bool, best *Action, bestScore float64) {
	sa.startTime = time.Now()
	sa.startTemp = startTemp
	sa.endTemp = endTemp
	sa.minimize = minimize

	sa.duration = duration
	sa.best = best
	sa.bestScore = bestScore
	sa.now = best.Copy()
	sa.nowScore = bestScore

}

func (sa *SimulatedAnnealing) Run(gameState *GameState, gameConst *GameConst) {
	prop := 0.0
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
			sa.temp = math.Pow(END_TEMP, prop) * math.Pow(START_TEMP, 1-prop)
		}
		sa.sumIter++

		// ここで何かnowを変更する

		sa.valid++
		score := 0.0
		p := sa.getAcceptRate(score, sa.bestScore)
		r := gameConst.Random.Float64()
		if p >= r {
			// 採用
			sa.accepted++
			sa.nowScore = score
			if sa.isBest(score, sa.bestScore) {
				sa.best = sa.now.Copy()
				sa.bestScore = sa.nowScore
			}
			sa.bestScore = score
			// 最善解を変更
		} else {
			// rollback（nowを戻す）
		}
	}
	fmt.Fprintln(os.Stderr, "--- SA Summary ---")
	fmt.Fprintln(os.Stderr, "sumIter: ", sa.sumIter)
	fmt.Fprintln(os.Stderr, "valid: ", sa.valid)
	fmt.Fprintln(os.Stderr, "accepted: ", sa.accepted)
	fmt.Fprintln(os.Stderr, "bestScore: ", sa.bestScore)
}

func getScore() float64 {
	return 0.0
}

func evaluate(gameState *GameState, gameConst *GameConst, action *Action, seed1 int, seed2 int, candidates []Card, prices []int) float64 {
	value := 0.0
	rand1 := NewRandGenerator(seed1)
	rand2 := NewRandGenerator(seed2)
	end := gameState.Turn1 + MAX_DEPTH
	end = minInt(end, T)
	if gameState.Turn1 >= TURNING_POINT {
		end = T
	}
	cg := NewGameState(gameConst)
	cg.Load(gameState)
	switch action.ActionType {
	case 0:
		cg.ApplyAction1(action, gameConst)
	case 1:
		cg.ApplyAction2(action, gameConst, candidates, prices)
	}
	value = playout(cg, gameConst, rand1, rand2, end)

	return value
}

type Solver struct {
	timeLimit int // solverに与えられた時間制限 microsecond
	start     time.Time
}

func NewSolver(timeLimit int, start time.Time) *Solver {
	s := Solver{}
	s.timeLimit = timeLimit
	s.start = start
	return &s
}

func (s *Solver) DecideCardToUse(gameState *GameState, gameConst *GameConst) (*Action, int, bool) {
	start := time.Now()
	maxAction := NewUseAction()
	action := NewUseAction()
	action2 := NewUseAction()
	// maxScore := -10000.0
	maxScore := -float64(INF)
	_ = maxScore

	// 貪欲を書くならここ
	var s1, s2 float64
	timeover := time.Since(s.start).Microseconds() > int64(s.timeLimit*(gameState.Turn1+gameState.Turn2-20))/int64(2*T)
	timeover2 := time.Since(s.start).Microseconds() > int64(s.timeLimit*(gameState.Turn1+gameState.Turn2-20))*7/int64(2*T*10)
	timeover3 := time.Since(s.start).Microseconds() > int64(s.timeLimit*(gameState.Turn1+gameState.Turn2-20))*8/int64(2*T*10)
	if !timeover2 {
		gameState.numPlayout++
	}
	if timeover3 {
		gameState.numPlayout *= 97
		gameState.numPlayout /= 100
		gameState.numPlayout = maxInt(gameState.numPlayout, MIN_PLAYOUT)
	}
	action.Index, action.Param, action2.Index, action2.Param, s1, s2 = GleedyUse(gameState, gameConst)
	if !SUCCESSIVE {
		fmt.Println("#", gameState.next)
		fmt.Println("#", gameState.prev)
		fmt.Println("#", action.Index, action.Param, s1)
		fmt.Println("#", action2.Index, action2.Param, s2)
	}
	if action2.Index == -1 || gameState.next[action.Index] != -1 {
		return action, 0, timeover
	}
	if action.Param == action2.Param {
		if gameState.Cards[action.Index].Type == gameState.Cards[action2.Index].Type {
			if gameState.Cards[action.Index].Value == gameState.Cards[action2.Index].Value {
				return action, 0, timeover
			}

		}
	}

	if gameState.Turn1 < TURNING_POINT {
		if gameState.Turn1 < LEARN_TURN || (timeover && gameState.numPlayout == MIN_PLAYOUT) {
			return action, 0, timeover
		}
	}
	// maxAction, maxScore = GleedyUse(gameState, gameConst)
	numPlayout := gameState.numPlayout
	sumV1 := 0.0
	sumV2 := 0.0
	diff := 0.0
	judge := -1
	for loop := 0; loop < numPlayout; loop++ {
		seed1 := gameConst.Random.Intn(1<<20) + 100000
		seed2 := gameConst.Random.Intn(1<<20) + 100000
		v1 := evaluate(gameState, gameConst, action, seed1, seed2, []Card{}, []int{})
		v2 := evaluate(gameState, gameConst, action2, seed1, seed2, []Card{}, []int{})
		sumV1 += v1
		sumV2 += v2
		diff += math.Abs(v1 - v2)
		if judge == -1 && loop > 20 && math.Abs(sumV1-sumV2)/(diff/float64(loop)) > math.Sqrt(float64(loop))*4 {
			// fmt.Fprintln(os.Stderr, diff, loop)
			if sumV1 >= sumV2 {
				judge = 0
			} else {
				judge = 1
			}
			break
		}
	}
	if judge == -1 {
		if math.Abs(sumV1-sumV2)/(diff/float64(numPlayout))*2 < math.Sqrt(float64(numPlayout)) {
			judge = 0
		} else {
			if sumV1 >= sumV2 {
				judge = 0
			} else {
				judge = 1

			}
		}
	}
	// if sumV1 > maxScore {
	if judge == 0 {
		maxScore = sumV1
		maxAction.Index = action.Index
		maxAction.Param = action.Param
	}
	// if sumV2 > maxScore {
	if judge == 1 {
		maxScore = sumV2
		maxAction.Index = action2.Index
		maxAction.Param = action2.Param
	}
	// maxAction, _ = GleedyUse(gameState, gameConst)
	// 貪欲ここまで

	sa := NewSA()
	sa.Init(s.timeLimit-int(time.Since(start).Microseconds()), START_TEMP, END_TEMP, MINIMIZE, maxAction, maxScore)
	// sa.Run(gameState, gameConst)
	score := 0
	// if DEBUG {
	// 	fmt.Fprintln(os.Stderr, gameState.Turn1, maxScore)
	// }
	return maxAction, score, timeover
}

func GleedyUse(gameState *GameState, gameConst *GameConst) (int, int, int, int, float64, float64) {
	maxScore := -float64(INF)
	maxActionIndex := -1
	maxActionParam := 0
	maxScore2 := -float64(INF)
	maxActionIndex2 := -1
	maxActionParam2 := 0
	checked2 := false
	checked3 := false
	for i := range gameState.Cards {
		if gameState.prev[i] != -1 {
			continue
		}
		if gameState.Cards[i].Type == 0 {
			for j := range gameState.Projects {
				value := calcEffect0(gameState, gameState.Cards[i].Type, gameState.Cards[i].Value, j)
				if value > maxScore {
					maxScore2 = maxScore
					maxActionIndex2 = maxActionIndex
					maxActionParam2 = maxActionParam
					maxScore = value
					maxActionIndex = i
					maxActionParam = j
				} else if value > maxScore2 {
					maxScore2 = value
					maxActionIndex2 = i
					maxActionParam2 = j
				}
			}
		} else if gameState.Cards[i].Type == 1 {
			value := calcEffect1(gameState, gameState.Cards[i].Type, gameState.Cards[i].Value, 0)
			if value > maxScore {
				maxScore2 = maxScore
				maxActionIndex2 = maxActionIndex
				maxActionParam2 = maxActionParam
				maxScore = value
				maxActionIndex = i
				maxActionParam = 0
			} else if value > maxScore2 {
				maxScore2 = value
				maxActionIndex2 = i
				maxActionParam2 = 0
			}
		} else if gameState.Cards[i].Type == 2 {
			if checked2 {
				continue
			}
			checked2 = true
			for j := range gameState.Projects {
				value := calcEffect2(gameState, gameState.Cards[i].Type, gameState.Cards[i].Value, j)
				if value > maxScore {
					maxScore2 = maxScore
					maxActionIndex2 = maxActionIndex
					maxActionParam2 = maxActionParam
					maxScore = value
					maxActionIndex = i
					maxActionParam = j
				} else if value > maxScore2 {
					maxScore2 = value
					maxActionIndex2 = i
					maxActionParam2 = j
				}
			}
		} else if gameState.Cards[i].Type == 3 {
			if checked3 {
				continue
			}
			checked3 = true
			value := calcEffect3(gameState, gameConst, gameState.Cards[i].Type, gameState.Cards[i].Value, 0)
			if value > maxScore {
				maxScore2 = maxScore
				maxActionIndex2 = maxActionIndex
				maxActionParam2 = maxActionParam
				maxScore = value
				maxActionIndex = i
				maxActionParam = 0
			} else if value > maxScore2 {
				maxScore2 = value
				maxActionIndex2 = i
				maxActionParam2 = 0
			}
		} else if gameState.Cards[i].Type == 4 {
			if gameState.L /*+gameState.TypeCount[4]*/ == 20 {
				continue
			}
			value := calcEffect4(gameState, gameState.Cards[i].Type, gameState.Cards[i].Value, 0)
			if value > maxScore {
				maxScore2 = maxScore
				maxActionIndex2 = maxActionIndex
				maxActionParam2 = maxActionParam
				maxScore = value
				maxActionIndex = i
				maxActionParam = 0
			} else if value > maxScore2 {
				maxScore2 = value
				maxActionIndex2 = i
				maxActionParam2 = 0
			}
		}

	}
	return maxActionIndex, maxActionParam, maxActionIndex2, maxActionParam2, maxScore, maxScore2
}

/*
	func calcEffect(gameState *GameState, cardType int, cardValue int, subject int) float64 {
		value := 0.0
		switch cardType {
		case 0:
			prop := float64(minInt(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress, cardValue)) / float64(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress)
			value = prop * prop * float64(gameState.Projects[subject].Reward)
			if gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress > 64<<gameState.L && prop < 0.1 {
				value *= 0.7
			}
			if float64(gameState.Power)+float64(gameState.Money)*1.2+float64(int(10)<<gameState.L) < float64(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress) {
				value *= 0.7
			}
			value -= float64(cardValue) * prop

		case 1:
			for j := range gameState.Projects {
				prop := float64(minInt(gameState.Projects[j].Capacity-gameState.Projects[j].Progress, cardValue)) / float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)
				value += prop * prop * float64(gameState.Projects[j].Reward)
				value -= float64(cardValue) * 0.8 * prop
			}
		case 2:
			value = float64(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress)*0.95 - float64(gameState.Projects[subject].Reward)
		case 3:
			for j := range gameState.Projects {
				value += float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)*0.95 - float64(gameState.Projects[j].Reward)
			}
		case 4:
			value = float64((int(1000) << gameState.L))
		}
		return value

}
*/
func calcEffect0(gameState *GameState, cardType int, cardValue int, subject int) float64 {
	value := 0.0
	prop := float64(minInt(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress, cardValue)) / float64(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress)
	value = prop * prop * float64(gameState.Projects[subject].Reward)
	if gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress > 64<<gameState.L && prop < 0.1 {
		value *= 0.7
	}
	if float64(gameState.Power)+float64(gameState.Money)*1.2+float64(int(10)<<gameState.L) < float64(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress) {
		value *= 0.7
	}
	value -= float64(cardValue) * prop
	return value
}

func calcEffect1(gameState *GameState, cardType int, cardValue int, subject int) float64 {
	value := 0.0
	for j := range gameState.Projects {
		prop := float64(minInt(gameState.Projects[j].Capacity-gameState.Projects[j].Progress, cardValue)) / float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)
		value += prop * prop * float64(gameState.Projects[j].Reward)
		value -= float64(cardValue) * 0.8 * prop
	}
	return value

}
func calcEffect2(gameState *GameState, cardType int, cardValue int, subject int) float64 {
	value := 0.0
	value = float64(gameState.Projects[subject].Capacity-gameState.Projects[subject].Progress)*0.95 - float64(gameState.Projects[subject].Reward)
	return value

}
func calcEffect3(gameState *GameState, gameConst *GameConst, cardType int, cardValue int, subject int) float64 {
	value := 0.0
	for j := range gameState.Projects {
		value += float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)*0.95 - float64(gameState.Projects[j].Reward)
	}
	value2 := float64(INF)
	for j := range gameState.Projects {
		value2 = math.Min(value2, float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)*(0.9+float64(gameConst.M)/10)-float64(gameState.Projects[j].Reward))
	}
	value = math.Max(value, value2)

	return value

}
func calcEffect4(gameState *GameState, cardType int, cardValue int, subject int) float64 {
	value := 0.0
	value = float64((int(1000) << gameState.L))
	return value

}

func (s *Solver) DecideCardToGet(gameState *GameState, gameConst *GameConst, candidates []Card, prices []int) (*Action, int, bool) {
	start := time.Now()
	maxAction := NewGetAction()
	action := NewGetAction()
	action2 := NewGetAction()
	maxScore := -float64(INF)
	_ = maxScore

	// 貪欲を書くならここ
	var s1, s2 float64
	action.Index, action2.Index, s1, s2 = GleedyGet(gameState, gameConst, candidates, prices)
	if !SUCCESSIVE {
		fmt.Println("#", action.Index, s1, gameState.Adjust[candidates[action.Index].Type])
		if action2.Index != -1 {

			fmt.Println("#", action2.Index, s2, gameState.Adjust[candidates[action2.Index].Type])
		} else {
			fmt.Println("#", action2.Index, s2)
		}
	}
	timeover := time.Since(s.start).Microseconds() > int64(s.timeLimit*(gameState.Turn1+gameState.Turn2-20))/int64(2*T)
	timeover2 := time.Since(s.start).Microseconds() > int64(s.timeLimit*(gameState.Turn1+gameState.Turn2-20))*7/int64(2*T*10)
	timeover3 := time.Since(s.start).Microseconds() > int64(s.timeLimit*(gameState.Turn1+gameState.Turn2-20))*8/int64(2*T*10)
	if !timeover2 {
		gameState.numPlayout++
	}
	if timeover3 {
		gameState.numPlayout *= 97
		gameState.numPlayout /= 100
		gameState.numPlayout = maxInt(gameState.numPlayout, MIN_PLAYOUT)
	}
	if gameState.Turn1 < TURNING_POINT || action2.Index == -1 {
		if action2.Index == -1 || gameState.Turn2 < LEARN_TURN || (timeover && gameState.numPlayout == MIN_PLAYOUT) {
			return action, 0, timeover
		}
	}
	numPlayout := gameState.numPlayout
	sumV1 := 0.0
	sumV2 := 0.0
	diff := 0.0
	judge := -1
	for loop := 0; loop < numPlayout; loop++ {
		seed1 := gameConst.Random.Intn(1<<20) + 100000
		seed2 := gameConst.Random.Intn(1<<20) + 100000
		v1 := evaluate(gameState, gameConst, action, seed1, seed2, candidates, prices)
		v2 := evaluate(gameState, gameConst, action2, seed1, seed2, candidates, prices)
		sumV1 += v1
		sumV2 += v2
		diff += math.Abs(v1 - v2)
		if judge == -1 && loop > 20 && math.Abs(sumV1-sumV2)/(diff/float64(loop)) > math.Sqrt(float64(loop))*4 {
			if sumV1 >= sumV2 {
				judge = 0
			} else {
				judge = 1
			}
			break
		}
	}
	if judge == -1 {
		if math.Abs(sumV1-sumV2)/(diff/float64(numPlayout))*2 < math.Sqrt(float64(numPlayout)) {
			judge = 0
		} else {
			if sumV1 >= sumV2 {
				judge = 0
			} else {
				judge = 1

			}
		}
	}
	// if sumV1 > maxScore {
	if judge == 0 {
		maxScore = sumV1
		maxAction.Index = action.Index
		maxAction.Param = action.Param
	}
	// if sumV2 > maxScore {
	if judge == 1 {
		maxScore = sumV2
		maxAction.Index = action2.Index
		maxAction.Param = action2.Param
		gameState.Adjust[candidates[action.Index].Type] -= ADJUST_RATE
		gameState.Adjust[candidates[action2.Index].Type] += ADJUST_RATE
	}
	// 貪欲ここまで

	sa := NewSA()
	sa.Init(s.timeLimit-int(time.Since(start).Microseconds()), START_TEMP, END_TEMP, MINIMIZE, maxAction, maxScore)
	// sa.Run(gameState, gameConst)
	score := 0

	return maxAction, score, timeover
}

func GleedyGet(gameState *GameState, gameConst *GameConst, candidates []Card, prices []int) (int, int, float64, float64) {
	maxIndex := -1
	maxValue := -float64(INF)
	maxIndex2 := -1
	maxValue2 := -float64(INF)

	for i := range candidates {
		if prices[i] > gameState.Money {
			continue
		}
		t := candidates[i].Type
		value := 0.0

		if t == 0 || t == 1 {
			value = float64(candidates[i].Value)
			if t == 1 {
				value *= float64(gameConst.M) * 0.9
			}
			value *= 0.95
			value -= float64(int(3) << gameState.L)

		}
		if t == 2 {
			value = -INF
			for j := range gameState.Projects {
				value = math.Max(value, float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)*0.95-float64(gameState.Projects[j].Reward-1<<gameState.L))
			}
			value *= 2

		}
		if t == 3 {
			value = 0
			for j := range gameState.Projects {
				value += float64(gameState.Projects[j].Capacity-gameState.Projects[j].Progress)*0.95 - float64(gameState.Projects[j].Reward-1<<gameState.L)
			}
			value *= 2
			// continue
		}
		if t == 4 {
			if gameState.L /*+gameState.TypeCount[4]*/ == 20 {
				continue
			}
			value = float64(int(3)<<gameState.L) * (T - float64(gameState.Turn1))
			// value = float64(int(700) << gameState.L)
		}
		value *= 1 + gameState.Adjust[candidates[i].Type]
		value -= float64(prices[i]) * 1.1
		if gameState.Money < 100<<gameState.L {
			value -= float64(prices[i]) * 0.1
		}
		if gameState.Money < 10<<gameState.L {
			value -= float64(prices[i]) * 0.1
		}
		viewDetail := value+float64(int(2)<<gameState.L) > maxValue2
		// viewDetail := true
		if viewDetail {
			if t == 0 || t == 1 {
				v2 := float64(-INF)
				if t == 0 {
					for j := range gameState.Projects {
						v2 = math.Max(v2, calcEffect0(gameState, t, candidates[i].Value, j))
					}
				}
				if t == 1 {
					v2 = math.Max(v2, calcEffect1(gameState, t, candidates[i].Value, 0))
				}
				value += v2 * 0.1
			}
			if t == 2 {
				v2 := float64(-INF)
				for j := range gameState.Projects {
					v2 = math.Max(v2, calcEffect2(gameState, t, candidates[i].Value, j))
				}

				value += v2 * 0.1
			}
			if t == 3 {
				v2 := float64(-INF)
				v2 = math.Max(v2, calcEffect3(gameState, gameConst, t, candidates[i].Value, 0))
				value += v2 * 0.1
				// continue
			}
			if t == 4 {
				v2 := float64(-INF)
				v2 = math.Max(v2, calcEffect4(gameState, t, candidates[i].Value, 0))
				value += v2 * 0.1
				// value = float64(int(700) << gameState.L)
			}
		} else {
			continue
		}

		if value > maxValue {
			if maxIndex != -1 && candidates[i].Type != candidates[maxIndex].Type {
				maxIndex2 = maxIndex
				maxValue2 = maxValue
			}
			maxIndex = i
			maxValue = value
		} else if candidates[i].Type != candidates[maxIndex].Type && value > maxValue2 {
			maxIndex2 = i
			maxValue2 = value
		}
	}
	return maxIndex, maxIndex2, maxValue, maxValue2
}

func playout(gameState *GameState, gameConst *GameConst, randForProject *RandGenerator, randForCard *RandGenerator, end int) float64 {
	dForAction := NewAction()
	dForCandidates := make([]Card, gameConst.K)
	dForPrices := make([]int, gameConst.K)
	assets := 0.0
	for gameState.Turn1 < end {
		playturn(gameState, gameConst, randForProject, randForCard, dForAction, dForCandidates, dForPrices, end)
	}
	assets += gameState.CalcAssets(gameConst)
	return assets
}

func playturn(gameState *GameState, gameConst *GameConst, randForProject *RandGenerator, randForCard *RandGenerator, dForAction *Action, dForCandidates []Card, dForPrices []int, end int) {
	if gameState.Turn1 > gameState.Turn2 {
		// 獲得ターン

		// Projectを新しいもので埋める
		updateProjectByEstimate(gameConst, gameState, randForProject)
		// candidates,pricesを得る
		candidates, prices := getCandidatesByEstimate(gameConst, gameState, randForCard, dForCandidates, dForPrices, end)

		dForAction.ActionType = 1
		dForAction.Param = 0
		dForAction.Index, _, _, _ = GleedyGet(gameState, gameConst, candidates, prices)
		gameState.ApplyAction2(dForAction, gameConst, candidates, prices)

	} else {
		// 消費ターン
		dForAction.ActionType = 0
		dForAction.Index, dForAction.Param, _, _, _, _ = GleedyUse(gameState, gameConst)
		gameState.ApplyAction1(dForAction, gameConst)
	}
}

func (runner *Runner) doGame(file string) Result {
	result := Result{}
	scanner := getScanner(FILE + "\\" + file + ".txt")

	// 初期値設定処理

	gameConst := NewGameConst(runner)
	gameConst.SetScanner(scanner)
	gameConst.SetSample()
	// データ取得
	gameState := getInitialInputs(gameConst)
	start := time.Now()
	gameConst.Sample.GenerateSample(gameConst)
	if REPRODUCE {
		fmt.Println(gameConst.N, gameConst.M, gameConst.K, T)
		for i := range gameState.Cards {
			fmt.Println(gameState.Cards[i].Type, gameState.Cards[i].Value)
		}
		for i := range gameState.Projects {
			fmt.Println(gameState.Projects[i].Capacity-gameState.Projects[i].Progress, gameState.Projects[i].Reward)
		}
	}

	// 初期値設定処理　ここまで
	solver := NewSolver(TIME_LIMIT, start)
	for {
		action, _, tover := solver.DecideCardToUse(gameState, gameConst)
		if !tover {
			result.loopNum++
		}
		// アクションを出力
		if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
			printAction(action, gameState, gameConst, false)
		}
		gameState.ApplyAction1(action, gameConst)

		if LOCAL_TEST {
			updateProject(gameConst, gameState)
		} else {
			getInputs(gameConst, gameState)
		}
		if REPRODUCE {
			for i := range gameState.Projects {
				fmt.Println(gameState.Projects[i].Capacity-gameState.Projects[i].Progress, gameState.Projects[i].Reward)
			}
			fmt.Println(gameState.Money)
		}
		candidates, prices := getCandidates(gameConst, gameState)
		if REPRODUCE {
			for i := range candidates {
				fmt.Println(candidates[i].Type, candidates[i].Value, prices[i])
			}
		}
		action, _, tover = solver.DecideCardToGet(gameState, gameConst, candidates, prices)
		if !tover {
			result.loopNum++
		}
		if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
			printAction(action, gameState, gameConst, false)
		}
		gameState.ApplyAction2(action, gameConst, candidates, prices)
		if gameState.Turn2 >= T {
			break
		}
	}
	// fmt.Fprintln(os.Stderr, gameState.Projects)
	// fmt.Fprintln(os.Stderr, gameState.Cards)
	score := 0
	if LOCAL_TEST {
		// fmt.Fprintln(os.Stderr, gameState.EstimatedParam)
		score = calcScore(gameState, gameConst)
		if time.Since(start).Microseconds() > TIME_LIMIT+100*1000 {
			score = -1
		}
		// fmt.Fprintln(os.Stderr, gameState.numPlayout)
	}

	result.score = score

	return result
}

func calcScore(gameState *GameState, gameConst *GameConst) int {
	score := gameState.Money
	return score
}
