// last local submission: 1697959506
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
	TIME_LIMIT = 1800 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01

	WEIGHT_MEAN = 100000

	MINIMIZE = true

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = false  // 複数ケースを試すか
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 1000
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FILE           = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "0000"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = false
	PRINT_ACTION       = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 0    // 基本戦術、方針レベルの違い
	VERSION_MINOR = 0    // 改善等
	VERSION_PATCH = 0    // バグ修正,パラメータ変更など
	VERSION_VALID = true // 提出に利用できる正当なものか

)

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

func getInputs(scanner *bufio.Scanner, gConst *GameConst) {
	scanner.Scan()
	splitedInput := strings.Split(scanner.Text(), " ")
	// アイテムの種類数
	N, _ := strconv.Atoi(splitedInput[0])
	// グループ数
	D, _ := strconv.Atoi(splitedInput[1])
	// 実行するクエリ数
	Q, _ := strconv.Atoi(splitedInput[2])
	gConst.NumItems = N
	gConst.NumBags = D
	gConst.NumQueries = Q

	if LOCAL_TEST {
		// 真値を受け取る
		scanner.Scan()
		splitedInput = strings.Split(scanner.Text(), " ")
		gConst.TrueWeights = make([]int, gConst.NumItems)
		for i, v := range splitedInput {
			weight, _ := strconv.Atoi(v)
			gConst.TrueWeights[i] = weight
		}

	}
	// fmt.Fprintln(os.Stderr, gConst.PlantsPrefixSum)
}

/*type Queue interface {
Pop() int
Push()
Len() int
}

type DistanceQueue struct {
	queue     [H * W]int
	popIndex  int
	pushIndex int
	length    int
}

func (q *DistanceQueue) Pop() int {
	v := q.queue[q.popIndex]
	q.popIndex++
	return v
}

func (q *DistanceQueue) Push(v int) {
	q.queue[q.pushIndex] = v
	q.pushIndex++
}

func (q *DistanceQueue) Len() int {
	return q.pushIndex - q.popIndex
}*/

type GameState struct {
	BagIndex      []int
	NumItemsInBag []int
	Order         []int
	NumPositive   int

	ItemRank []int

	QueryCount int

	EstimatedWeights []float64
	CompareHistory   [][2][]int
	ResultHistory    []int

	RankHistory [][][]int

	DiffHistory []float64
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	g.BagIndex = make([]int, gConst.NumItems)
	g.NumItemsInBag = make([]int, gConst.NumBags)
	g.Order = make([]int, gConst.NumBags)
	g.NumItemsInBag[0] = gConst.NumItems
	for i := range g.Order {
		g.Order[i] = i
	}
	g.NumItemsInBag[0] = gConst.NumItems
	g.NumPositive = 1
	g.ItemRank = []int{}

	g.EstimatedWeights = make([]float64, gConst.NumItems)
	for i := range g.EstimatedWeights {
		g.EstimatedWeights[i] = WEIGHT_MEAN
	}
	g.CompareHistory = make([][2][]int, 0, gConst.NumQueries*gConst.NumBags)
	g.ResultHistory = make([]int, 0, gConst.NumQueries*gConst.NumBags)
	g.RankHistory = make([][][]int, 0, gConst.NumQueries)
	return &g
}

type GameConst struct {
	NumItems    int
	NumBags     int
	NumQueries  int
	TrueWeights []int

	random *rand.Rand
}

func NewGameConst() *GameConst {
	g := GameConst{}
	g.random = rand.New(rand.NewSource(time.Now().UnixNano()))
	return &g
}

func printAction(action *Action, gameState *GameState, gameConst *GameConst, comment bool) {
	// fmt.Println("アクションを出力")
	message := ""
	if comment {
		message += "#c "
	}
	for i := range gameState.BagIndex {
		message += fmt.Sprintf("%d ", gameState.BagIndex[i])
	}
	message = message[:len(message)-1]
	fmt.Println(message)
}

type Result struct {
	loopNum int
	score   int
}

func test(testCase int) Result {
	file := fmt.Sprintf("%04d", testCase)
	result := doGame(file)
	if DEBUG {
		fmt.Fprintf(os.Stderr, "score:%d (case=%04d)\n", result.score, testCase)
	}
	return result
}

func parallelTester(prefix int, bests [TEST_NUM]Result) [TEST_NUM]Result {
	// channelを作成

	result := [TEST_NUM]Result{}
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
			var r Result
			r = test(number)
			// }
			result[i] = r
			<-channels
		}(num)
	}
	wg.Wait()
	return result
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
		results := parallelTester(PREFIX, bests)
		sumScore := 0
		sumRelativeScore := 0.0
		minsScore := INF
		for i := 0; i < TEST_NUM; i++ {
			sumScore += results[i].score
			sumRelativeScore += math.Min(float64(bests[i].score)/float64(results[i].score), 1)
			if results[i].score == INF {
				fmt.Fprintf(os.Stderr, "WARN: case %d failed.\n", i)
			}
			minsScore = minInt(minsScore, results[i].score)
		}
		fmt.Println(sumRelativeScore)
		fmt.Fprintf(os.Stderr, "\n---END---\n%d case finished.\n", TEST_NUM)
		fmt.Fprintln(os.Stderr, "sumScore:", sumScore)
		fmt.Fprintln(os.Stderr, "sumRelativeScore:", sumRelativeScore)
		fmt.Fprintf(os.Stderr, "\n")
		timeStamp := time.Now().Unix()
		for i := 0; i < TEST_NUM; i++ {
			fmt.Fprintf(os.Stderr, "%d ", results[i].score)
		}
		fmt.Fprintln(os.Stderr, "")
		fmt.Fprintf(os.Stderr, "// last local submission: %d\n", timeStamp)
		if !REPEAT {
			if RECORD {
				path := fmt.Sprintf("%s\\%d-%d_%d.txt", RECORD_FOLDER, PREFIX, PREFIX+TEST_NUM-1, timeStamp)
				fp, _ := os.Create(path)
				fmt.Fprintf(fp, "%d.%d.%d\n%t\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_VALID)
				for i := 0; i < TEST_NUM; i++ {
					fmt.Fprintf(fp, "%d %d %d\n", i, results[i].score, results[i].loopNum)
				}
				fp.Close()
			}
		} else {
			fmt.Fprintf(os.Stderr, "minScore: %d", minsScore)
		}
		// 結果を書き込む
		fmt.Fprintf(os.Stderr, "\n")
	} else {
		// 単体ケースを実行
		result := doGame(FILE_NUMBER)
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

/*func (a *Action) addAction() {
}*/

type SimulatedAnnealing struct {
	startTime time.Time
	duration  int64 // microsecond

	sumIter   int
	valid     int
	accepted  int
	temp      float64
	best      *Action
	bestScore float64
	now       *Action
	nowScore  float64
}

func NewSA() *SimulatedAnnealing {
	sa := SimulatedAnnealing{}
	sa.duration = TIME_LIMIT
	sa.best = NewAction()
	return &sa
}

func getAcceptRate(score float64, bestScore float64, temp float64) float64 {
	diff := score - bestScore
	if MINIMIZE {
		diff *= -1
	}
	p := math.Exp(diff / temp)
	return p
}
func isBest(score float64, bestScore float64) bool {
	if MINIMIZE {
		return score <= bestScore
	} else {
		return score >= bestScore

	}
}

func (sa *SimulatedAnnealing) Run(gameState *GameState, gameConst *GameConst) {
	updateTemp := (1 << 11) - 1
	prop := 0.0
	for {
		if (sa.sumIter & updateTemp) == 0 {
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
		p := getAcceptRate(score, sa.bestScore, sa.temp)
		r := gameConst.random.Float64()
		if p >= r {
			// 採用
			sa.accepted++
			/*if isBest(score, sa.bestScore) {
			sa.
			}*/
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

func evaluate(gameState *GameState, gameConst *GameConst) float64 {
	value := 0.0
	return value
}

func setRank(scanner *bufio.Scanner, gameState *GameState, gameConst *GameConst, b []int, ecomode bool) {
	if ecomode {
		weights := make([]float64, gameConst.NumBags)
		for i := 0; i < gameConst.NumItems; i++ {
			weights[gameState.BagIndex[i]] += gameState.EstimatedWeights[i]
		}
		sort.Slice(gameState.Order, func(i, j int) bool { return weights[gameState.Order[i]] > weights[gameState.Order[j]] })
		return
	}
	if len(b) == 0 {
		return
	}
	oldOrder := []int{}
	for _, v := range gameState.Order {
		exist := false
		for i := range b {
			if v == b[i] {
				exist = true
				break
			}
		}
		if exist {
			continue
		}
		oldOrder = append(oldOrder, v)
	}
	for i := range b {
		rank, ok := searchRank(scanner, b[i], oldOrder, gameState, gameConst, 0, len(oldOrder))
		if !ok {
			return
		}
		// fmt.Fprintln(os.Stderr, b[i], rank)
		newOrder := []int{}
		newOrder = append(newOrder, oldOrder[:rank]...)
		newOrder = append(newOrder, b[i])
		newOrder = append(newOrder, oldOrder[rank:]...)
		oldOrder = newOrder
		gameState.Order = newOrder
	}
	// fmt.Fprintln(os.Stderr, gameState.Order)
}

func searchRank(scanner *bufio.Scanner, bag int, order []int, gameState *GameState, gameConst *GameConst, minIndex int, maxIndex int) (int, bool) {
	for minIndex < maxIndex {
		index := (minIndex + maxIndex) / 2
		b2 := order[index]
		result, ok := compareTwoBags(scanner, gameState, gameConst, bag, b2)
		if !ok {
			return 0, false
		}
		if result == 0 {
			maxIndex = index
		}
		if result == 1 {
			maxIndex = index
		}
		if result == 2 {
			minIndex = index + 1
		}
	}
	rank := maxIndex
	return rank, true
}

func compareTwoBags(scanner *bufio.Scanner, gameState *GameState, gameConst *GameConst, b1 int, b2 int) (int, bool) {
	group1 := make([]int, 0, gameState.NumItemsInBag[b1])
	group2 := make([]int, 0, gameState.NumItemsInBag[b2])
	for i := range gameState.BagIndex {
		if gameState.BagIndex[i] == b1 {
			group1 = append(group1, i)
		}
		if gameState.BagIndex[i] == b2 {
			group2 = append(group2, i)
		}
	}
	result, ok := compareAndUpdate(scanner, gameState, gameConst, group1, group2)
	return result, ok
}

func compareTwoGroups(scanner *bufio.Scanner, gameState *GameState, gameConst *GameConst, g1 []int, g2 []int) (int, bool, bool) {
	// 自明ならクエリを飛ばさない
	if len(g2) == 0 {
		if len(g1) == 0 {
			return 0, true, false
		} else {
			return 1, true, false
		}
	}
	if len(g1) == 0 {
		return 2, true, false
	}
	// もし同じクエリを飛ばしたことがあるなら飛ばさない
	for i := range gameState.CompareHistory {
		same := true
		if len(gameState.CompareHistory[i][0]) != len(g1) {
			same = false
		} else if len(gameState.CompareHistory[i][1]) != len(g2) {
			same = false
		} else {
			for j := range gameState.CompareHistory[i][0] {
				if gameState.CompareHistory[i][0][j] != g1[j] {
					same = false
					break
				}
			}
			if same {
				for j := range gameState.CompareHistory[i][1] {
					if gameState.CompareHistory[i][1][j] != g2[j] {
						same = false
						break
					}
				}
			}
		}
		if same {
			// fmt.Fprintln(os.Stderr, "!!!")
			return gameState.ResultHistory[i], true, false
		}
	}
	for i := range gameState.CompareHistory {
		same := true
		if len(gameState.CompareHistory[i][0]) != len(g2) {
			same = false
		} else if len(gameState.CompareHistory[i][1]) != len(g1) {
			same = false
		} else {
			for j := range gameState.CompareHistory[i][0] {
				if gameState.CompareHistory[i][0][j] != g2[j] {
					same = false
					break
				}
			}
			if same {
				for j := range gameState.CompareHistory[i][1] {
					if gameState.CompareHistory[i][1][j] != g1[j] {
						same = false
						break
					}
				}
			}
		}
		if same {
			// fmt.Fprintln(os.Stderr, "!!!")
			r := 0
			if gameState.ResultHistory[i] == 2 {
				r = 1
			}
			if gameState.ResultHistory[i] == 1 {
				r = 2
			}
			return r, true, false
		}
	}
	// 既存の情報から導けるか・・・？
	if len(gameState.ItemRank) == gameConst.NumItems {
		sortedg1 := make([]int, 0, len(g1))
		sortedg2 := make([]int, 0, len(g2))
		sortedg1 = append(sortedg1, g1...)
		sortedg2 = append(sortedg2, g2...)
		sort.Slice(sortedg1, func(i, j int) bool { return gameState.ItemRank[sortedg1[i]] < gameState.ItemRank[sortedg1[j]] })
		sort.Slice(sortedg2, func(i, j int) bool { return gameState.ItemRank[sortedg2[i]] < gameState.ItemRank[sortedg2[j]] })
		possible := [2]bool{true, true}
		if len(g1) < len(g2) {
			possible[0] = false
		}
		if len(g1) > len(g2) {
			possible[1] = false
		}
		for i := 0; i < minInt(len(g1), len(g2)); i++ {
			if gameState.ItemRank[sortedg1[i]] < gameState.ItemRank[sortedg2[i]] {
				possible[1] = false
			} else {
				possible[0] = false
			}
		}
		if possible[0] {
			return 1, true, true
		}
		if possible[1] {
			return 2, true, true
		}
	}
	// 簡易チェック
	// r, sure := check(gameState, gameConst, g1, g2)
	// r2, sure2 := check(gameState, gameConst, g1, g2)
	// sure = sure && sure2 && r == r2
	// if sure {
	// 	fmt.Println("# 多分", r)
	//
	// 	// return r, true, false
	// }

	if gameState.QueryCount == gameConst.NumQueries {
		return 0, false, false
	}
	gameState.QueryCount += 1
	if !LOCAL_TEST || (PRINT_ACTION && !SUCCESSIVE) {
		message := ""
		message += fmt.Sprintf("%d ", len(g1))
		message += fmt.Sprintf("%d ", len(g2))
		for _, v := range g1 {
			message += fmt.Sprintf("%d ", v)

		}
		for _, v := range g2 {
			message += fmt.Sprintf("%d ", v)

		}

		message = message[:len(message)-1]
		fmt.Println(message)
	}
	if LOCAL_TEST {
		weights1 := 0
		weights2 := 0
		for _, v := range g1 {
			weights1 += gameConst.TrueWeights[v]
		}
		for _, v := range g2 {
			weights2 += gameConst.TrueWeights[v]
		}
		result := 0

		if weights1 > weights2 {
			result = 1
		}
		if weights1 < weights2 {
			result = 2
		}
		// if sure {
		// 	fmt.Println("#", result == r)
		// }

		return result, true, true
	} else {
		scanner.Scan()
		text := scanner.Text()
		if text == "=" {
			return 0, true, true
		}
		if text == ">" {
			return 1, true, true
		}
		if text == "<" {
			return 2, true, true
		}
	}
	return -1, false, false // equal
}

func check(gameState *GameState, gameConst *GameConst, g1 []int, g2 []int) (int, bool) {
	// 自明ならクエリを飛ばさない
	if len(g2) == 0 {
		if len(g1) == 0 {
			return 0, true
		} else {
			return 1, true
		}
	}
	if len(g1) == 0 {
		return 2, true
	}
	sure := true
	testEstimate := make([]float64, 0, gameConst.NumItems)
	testEstimate = append(testEstimate, gameState.EstimatedWeights...)
	w1 := 0.0
	w2 := 0.0
	for i := range g1 {
		w1 += testEstimate[g1[i]]
	}
	for i := range g2 {
		w2 += testEstimate[g2[i]]
	}
	newW1 := 0.0
	newW2 := 0.0
	oldResult := 0
	over := 0.003

	if w1 > w2 {
		oldResult = 1
	}
	if w1 < w2 {
		oldResult = 2
	}
	for loop := 0; loop < 10; loop++ {
		for i := 0; i < gameConst.NumItems; i++ {
			testEstimate[i] *= 1 // (1 + 0.50*(gameConst.random.Float64()-0.25))
		}
		w1 = 0.0
		w2 = 0.0
		for i := range g1 {
			w1 += testEstimate[g1[i]]
		}
		for i := range g2 {
			w2 += testEstimate[g2[i]]
		}
		newW1 = (w1 + w2) / 2
		newW2 = (w1 + w2) / 2

		if w1 > w2 {
			newW1 = newW1 * (1 - over)
			newW2 = newW2 * (1 + over)
		}
		if w1 < w2 {
			newW1 = newW1 * (1 + over)
			newW2 = newW2 * (1 - over)
		}
		p1 := newW1 / w1
		p2 := newW2 / w2
		for _, v := range g1 {
			testEstimate[v] *= p1
			testEstimate[v] = math.Max(testEstimate[v], 300)
		}
		for _, v := range g2 {
			testEstimate[v] *= p2
			testEstimate[v] = math.Max(testEstimate[v], 300)
		}
		for i := range gameState.CompareHistory {
			updateEstimation(testEstimate, gameState, gameConst, gameState.CompareHistory[i][0], gameState.CompareHistory[i][1], gameState.ResultHistory[i], false, 15, false)
		}
		newResult := 0
		w1 = 0.0
		w2 = 0.0
		for i := range g1 {
			w1 += testEstimate[g1[i]]
		}
		for i := range g2 {
			w2 += testEstimate[g2[i]]
		}
		if w1 > w2 {
			newResult = 1
		}
		if w1 < w2 {
			newResult = 2
		}
		if newResult != oldResult || newResult == 0 {
			sure = false
			break
		}
	}
	return oldResult, sure
}

func updateEstimation(estimate []float64, gameState *GameState, gameConst *GameConst, g1 []int, g2 []int, result int, newinfo bool, div2 float64, real bool) {
	if result == -1 {
		return
	}
	w1 := 0.0
	w2 := 0.0
	for _, v := range g1 {
		w1 += estimate[v]
	}
	for _, v := range g2 {
		w2 += estimate[v]
	}
	estimatedResult := 0
	if w1 > w2 {
		estimatedResult = 1
	}
	if w1 < w2 {
		estimatedResult = 2
	}
	needUpdate := false
	if estimatedResult != result {
		needUpdate = true
	}
	if needUpdate {
		/*if newinfo {
			fmt.Println("# 知らなかった", w1, w2)
		}*/
		// このとき、w1,w2はともに0でないことが保証されている
		newW1 := (w1 + w2) / 2
		newW2 := (w1 + w2) / 2
		// 過剰に見積もる
		div := math.Max(1, math.Pow(2, float64(gameState.QueryCount)/float64(gameConst.NumItems)/2))
		if len(gameState.ItemRank) != 0 {
			offset := gameConst.NumItems * int(math.Log(float64(gameConst.NumItems-1))-1)
			div = math.Max(1, math.Pow(2, float64(gameState.QueryCount-offset)/math.Pow(3+math.Log(float64(gameConst.NumBags)), 2.0)/5))
			// div = math.Max(1, math.Log(float64(gameState.QueryCount-gameConst.NumItems*int(math.Log(float64(gameConst.NumItems-1))-1))/float64(gameConst.NumItems))/math.Log(2))
		}
		// div := math.Max(1, math.Log(float64(gameState.QueryCount)/float64(gameConst.NumItems))/math.Log(2))
		// if len(gameState.ItemRank) != 0 {
		// 	div = math.Max(1, math.Log(float64(gameState.QueryCount-gameConst.NumItems*int(math.Log(float64(gameConst.NumItems-1))-1))/float64(gameConst.NumItems))/math.Log(2))
		// }
		over := 0.08 / div / div2
		if len(gameState.ItemRank) != 0 {
			// 全アイテムの重さ順がわかっているため影響を受ける
			over = 0.06 / div / div2
		}
		// if real {
		// 	diff := math.Abs(w1-w2) / (w1 + w2) / 2
		// 	gameState.DiffHistory = append(gameState.DiffHistory, diff)
		// }
		// maxDiff := 0.0
		// meanDiff := 0.0
		// c := 0
		// growth := 0.8
		// for i := maxInt(0, len(gameState.DiffHistory)-100); i < len(gameState.DiffHistory); i++ {
		// 	maxDiff = float64(math.Max(maxDiff, gameState.DiffHistory[i]))
		// 	// meanDiff += gameState.DiffHistory[i]
		// 	meanDiff = meanDiff*growth + gameState.DiffHistory[i]
		// 	c++
		// }
		// meanDiff /= float64(c)
		// meanDiff *= (1 - growth)
		// fmt.Fprintf(os.Stderr, "%06f,%06f, %06f\n", maxDiff, meanDiff, over)
		// if c > 50 {
		// 	if over < meanDiff*0.1 && real {
		// 		fmt.Fprintf(os.Stderr, "%06f, %06f\n", meanDiff, over)
		// 	}
		// 	over = math.Max(over, meanDiff*0.1)
		// }
		// if c > 50 {
		// 	over = meanDiff * 1.1
		// }
		// if len(es)

		// div := math.Max(1, float64(gameState.QueryCount)/float64(gameConst.NumBags)/5)
		// over := 0.10 / div / div2
		// if len(gameState.ItemRank) != 0 {
		// 	// 全アイテムの重さ順がわかっているため影響を受ける
		// 	over = 0.05 / div / div2
		// }
		if result == 2 {
			//if newW1*(1-over/4) > w1 {
			//	return
			//}
			newW1 = newW1 * (1 - over)
			newW2 = newW2 * (1 + over)

		} else if result == 1 {
			//if newW2*(1-over/4) > w2 {
			//	return
			//}
			newW1 = newW1 * (1 + over)
			newW2 = newW2 * (1 - over)
		}
		p1 := newW1 / w1
		p2 := newW2 / w2
		for _, v := range g1 {
			estimate[v] *= p1
			estimate[v] = math.Max(estimate[v], 300)
		}
		for _, v := range g2 {
			estimate[v] *= p2
			estimate[v] = math.Max(estimate[v], 300)
		}
	} else {
		/*if newinfo {
			fmt.Println("# 知ってた", w1, w2)
		}*/
	}

}

func updateEstimationByRank(estimate []float64, gameState *GameState, gameConst *GameConst, groups [][]int) {
	indexes := make([]int, len(groups))
	weights := make([]float64, len(groups))
	for i := range indexes {
		indexes[i] = i
	}
	for i := range groups {
		for j := range groups[i] {
			weights[i] += estimate[groups[i][j]]
		}
	}
	// newWeights := make([]float64, len(groups))
	// for i := 0; i < len(groups); i++ {
	// 	for j := i; j < len(groups); j++ {
	// 		newWeights[i] = math.Max(newWeights[i], weights[j])
	// 	}
	// }
	sort.Slice(indexes, func(i, j int) bool { return weights[indexes[i]] > weights[indexes[j]] })
	for i := range groups {
		for j := range groups[i] {
			estimate[groups[i][j]] *= weights[indexes[i]] / weights[i]
		}
	}
	// for i := range groups {
	// 	for j := range groups[i] {
	// 		estimate[groups[i][j]] *= newWeights[i] / weights[i]
	// 	}
	// }

}

func compareAndUpdate(scanner *bufio.Scanner, gameState *GameState, gameConst *GameConst, g1 []int, g2 []int) (int, bool) {
	result, ok, newinfo := compareTwoGroups(scanner, gameState, gameConst, g1, g2)
	if newinfo {
		gameState.CompareHistory = append(gameState.CompareHistory, [2][]int{g1, g2})
		gameState.ResultHistory = append(gameState.ResultHistory, result)
		updateEstimation(gameState.EstimatedWeights, gameState, gameConst, g1, g2, result, newinfo, 1, true)
	}
	for i := range gameState.CompareHistory {
		updateEstimation(gameState.EstimatedWeights, gameState, gameConst, gameState.CompareHistory[i][0], gameState.CompareHistory[i][1], gameState.ResultHistory[i], false, 1, false)
	}
	if DEBUG && newinfo {
		fmt.Printf("# ")
		for i := 0; i < gameConst.NumItems; i++ {
			fmt.Printf("%d ", int(gameState.EstimatedWeights[i]+1))

		}
		fmt.Printf("\n")
	}
	return result, ok
}

func splitBag(gameState *GameState, gameConst *GameConst, b1 int) [2][]int {
	group1 := [2][]int{}
	group1[0] = make([]int, 0, gameState.NumItemsInBag[b1])
	group1[1] = make([]int, 0, gameState.NumItemsInBag[b1])

	for i := 0; i < gameConst.NumItems; i++ {
		if gameState.BagIndex[i] == b1 {
			group1[0] = append(group1[0], i)
		}
	}
	// 二つに分ける
	for i := 0; i < gameState.NumItemsInBag[b1]*1/2; i++ {
		index := gameConst.random.Intn(len(group1[0]))
		group1[1] = append(group1[1], group1[0][index])
		group1[0][index] = group1[0][len(group1[0])-1]
		group1[0] = group1[0][:len(group1[0])-1]
	}
	return group1
}

func getReasonableDivision(gameState *GameState, gameConst *GameConst, b1 int, b2 int, mode int) ([2][]int, [2][]int, bool) {
	sumWeight := 0.0
	group := make([]int, 0, gameState.NumItemsInBag[b1]+gameState.NumItemsInBag[b2])
	w := 0.0
	w2 := 0.0
	promised := true
	plan := make([]int, 0)
	for i := range gameState.BagIndex {
		if gameState.BagIndex[i] == b1 {
			group = append(group, i)
			plan = append(plan, 0)
			w += gameState.EstimatedWeights[i]
		}
		if gameState.BagIndex[i] == b2 {
			group = append(group, i)
			if mode == 0 {
				plan = append(plan, 0)
			} else {
				plan = append(plan, 1)
			}
			w2 += gameState.EstimatedWeights[i]
		}
	}
	d := math.Abs(w - w2)
	perturbedWeights := make([]float64, len(group))
	for i := range group {
		weight := gameState.EstimatedWeights[group[i]] // * (0.95 + rand.Float64()*0.1)
		perturbedWeights[i] = weight
		sumWeight += weight
	}
	weights := [2]float64{}

	if mode == 1 {
		weights = [2]float64{w, w2}
		bestr := [3]int{-1, -1, -1}
		bestd := 100000000.0
		for i := 0; i < 1000; i++ {
			// for i := 0; i < minInt(len(group)*400, 3000); i++ {
			// for i := 0; i < 10000; i++ {
			r := [3]int{-1, -1, -1}
			r[0] = gameConst.random.Intn(len(group))
			if gameConst.random.Float64() < 0.8 {
				r[1] = gameConst.random.Intn(len(group))
				for j, v := range r {
					if j == 1 {
						continue
					}
					if v == r[1] {
						r[1] = -1
						break
					}
				}
			}
			if gameConst.random.Float64() < 0.7 {
				r[2] = gameConst.random.Intn(len(group))
				for j, v := range r {
					if j == 2 {
						continue
					}
					if v == r[2] {
						r[2] = -1
						break
					}
				}
			}
			diff1 := math.Abs(weights[1] - weights[0])
			for _, v := range r {
				if v == -1 {
					continue
				}
				weights[plan[v]] -= perturbedWeights[v]
				plan[v] = 1 - plan[v]
				weights[plan[v]] += perturbedWeights[v]
			}
			diff2 := math.Abs(weights[1] - weights[0])
			if mode == 1 {
				if diff2 < bestd {
					bestd = diff2
					for i := range r {
						bestr[i] = r[i]
					}
				}
			}
			if diff1 < diff2 || mode == 1 {
				for _, v := range r {
					if v == -1 {
						continue
					}
					weights[plan[v]] -= perturbedWeights[v]
					plan[v] = 1 - plan[v]
					weights[plan[v]] += perturbedWeights[v]
				}
			}

		}
		if mode == 1 {
			for _, v := range bestr {
				if v == -1 {
					continue
				}
				weights[plan[v]] -= perturbedWeights[v]
				plan[v] = 1 - plan[v]
				weights[plan[v]] += perturbedWeights[v]
			}
		}
	} else {
		plan = getOptimalPlan(gameState, gameConst, group, 2, 0.5, 2000, 0)
		for i := range plan {
			weights[plan[i]] += perturbedWeights[i]
		}
	}
	d2 := math.Abs(weights[1] - weights[0])
	if d-1 < d2 {
		// fmt.Fprintln(os.Stderr, "???")
		promised = false
	}
	group1 := [2][]int{}
	group2 := [2][]int{}
	for i := range plan {
		if plan[i] == 0 {
			if gameState.BagIndex[group[i]] == b1 {
				group1[0] = append(group1[0], group[i])
			} else {
				group2[1] = append(group2[1], group[i])
			}
		}
		if plan[i] == 1 {
			if gameState.BagIndex[group[i]] == b1 {
				group1[1] = append(group1[1], group[i])
			} else {
				group2[0] = append(group2[0], group[i])
			}
		}
	}
	// group1 := splitBag(gameState, gameConst, b1)
	// group2 := splitBag(gameState, gameConst, b2)
	return group1, group2, promised
}

func getHalf(gameState *GameState, gameConst *GameConst) ([]int, []int) {
	items := make([]int, gameConst.NumItems)
	for i := range items {
		items[i] = i
	}
	plan := getOptimalPlan(gameState, gameConst, items, 2, 1, 10000, 0)
	// sumWeight := 0.0
	// perturbedWeights := make([]float64, gameConst.NumItems)
	// for i := range perturbedWeights {
	// 	weight := gameState.EstimatedWeights[i] * (0.995 + rand.Float64()*0.01)
	// 	perturbedWeights[i] = weight
	// 	sumWeight += weight
	// }
	// plan := make([]int, gameConst.NumItems)
	// weights := [2]float64{sumWeight, 0}
	// num := [2]int{gameConst.NumItems, 0}
	// for i := 0; i < 10000; i++ {
	// 	r := [3]int{-1, -1, -1}
	// 	r[0] = gameConst.random.Intn(gameConst.NumItems)
	// 	if gameConst.random.Float64() < 0.7 {
	// 		r[1] = gameConst.random.Intn(gameConst.NumItems)
	// 		for j, v := range r {
	// 			if j == 1 {
	// 				continue
	// 			}
	// 			if v == r[1] {
	// 				r[1] = -1
	// 				break
	// 			}
	// 		}
	// 	}
	// 	if gameConst.random.Float64() < 0.5 {
	// 		r[2] = gameConst.random.Intn(gameConst.NumItems)
	// 		for j, v := range r {
	// 			if j == 2 {
	// 				continue
	// 			}
	// 			if v == r[2] {
	// 				r[2] = -1
	// 				break
	// 			}
	// 		}
	// 	}
	// 	diff1 := math.Abs(weights[1] - weights[0]) // + 30000*float64(absInt(num[0]-num[1]))
	// 	for _, v := range r {
	// 		if v == -1 {
	// 			continue
	// 		}
	// 		weights[plan[v]] -= perturbedWeights[v]
	// 		num[plan[v]]--
	// 		plan[v] = 1 - plan[v]
	// 		num[plan[v]]++
	// 		weights[plan[v]] += perturbedWeights[v]
	// 	}
	// 	diff2 := math.Abs(weights[1] - weights[0]) // + 30000*float64(absInt(num[0]-num[1]))
	// 	if diff1 < diff2 {
	// 		for _, v := range r {
	// 			if v == -1 {
	// 				continue
	// 			}
	// 			weights[plan[v]] -= perturbedWeights[v]
	// 			num[plan[v]]--
	// 			plan[v] = 1 - plan[v]
	// 			num[plan[v]]++
	// 			weights[plan[v]] += perturbedWeights[v]
	// 		}
	// 	}
	//
	// }
	g1 := []int{}
	g2 := []int{}
	for i := range plan {
		if plan[i] == 0 {
			g1 = append(g1, i)
		}
		if plan[i] == 1 {

			g2 = append(g2, i)
		}
	}
	return g1, g2
}

func setInitialState(scanner *bufio.Scanner, gameState *GameState, gameConst *GameConst, mode int) {
	if len(gameState.ItemRank) == 0 && mode == 0 {
		for i := range gameState.BagIndex {
			gameState.BagIndex[i] = -1
		}
		ids := make([]int, gameConst.NumItems)
		for i := range ids {
			ids[i] = i % gameConst.NumBags
		}
		// gameConst.random.Shuffle(gameConst.NumItems, func(i, j int) { ids[i], ids[j] = ids[j], ids[i] })
		gameState.NumItemsInBag[0] = 0
		gameState.Order = []int{}
		gameState.NumPositive = 0
		// ここでもうswapを初めておく
		// count := 0
		// offset := gameState.QueryCount
		// limit := offset + gameConst.NumQueries/20
		// 均せるかを試す
		/*for gameState.QueryCount < limit {
			b1 := gameConst.random.Intn(gameConst.NumBags)
			b2 := gameConst.random.Intn(gameConst.NumBags)
			if b1 == b2 {
				continue
			}
			g1 := []int{}
			g2 := []int{}
			for i := range ids {
				if ids[i] == b1 {
					g1 = append(g1, i)
				}
				if ids[i] == b2 {
					g2 = append(g2, i)
				}
			}
			w1 := 0.0
			w2 := 0.0
			for i := range g1 {
				w1 += gameState.EstimatedWeights[g1[i]]
			}
			for i := range g2 {
				w2 += gameState.EstimatedWeights[g2[i]]
			}
			if w1 < w2 {
				w1, w2 = w2, w1
				g1, g2 = g2, g1
				b1, b2 = b2, b1
			}


			if result == 1 {
				fmt.Fprintln(os.Stderr, "成功")
				count++
			} else {
				fmt.Fprintln(os.Stderr, "失敗")
			}
		}*/
		// fmt.Fprintln(os.Stderr, count, "/", gameState.QueryCount-offset)
		for gameState.NumPositive < gameConst.NumBags {
			for i := range ids {
				if ids[i] == gameState.NumPositive {
					gameState.BagIndex[i] = gameState.NumPositive
					gameState.NumItemsInBag[gameState.NumPositive]++
				}
			}
			// for i := gameState.NumPositive; i < gameConst.NumItems; i += gameConst.NumBags {
			// 	gameState.BagIndex[i] = gameState.NumPositive
			// 	gameState.NumItemsInBag[gameState.NumPositive]++
			// }
			rank, _ := searchRank(scanner, gameState.NumPositive, gameState.Order, gameState, gameConst, 0, len(gameState.Order))

			newOrder := []int{}
			newOrder = append(newOrder, gameState.Order[:rank]...)
			newOrder = append(newOrder, gameState.NumPositive)
			newOrder = append(newOrder, gameState.Order[rank:]...)
			gameState.Order = newOrder
			gameState.NumPositive++
		}
	} else {

		for i := range gameState.BagIndex {
			gameState.BagIndex[i] = -1
		}
		gameState.NumItemsInBag[0] = 0
		gameState.Order = []int{}
		gameState.NumPositive = 0
		items := make([]int, gameConst.NumItems)
		for i := range items {
			items[i] = i
		}

		bagIndex := getOptimalPlan(gameState, gameConst, items, gameConst.NumBags, 1, 1000000, 0)

		for gameState.NumPositive < gameConst.NumBags {
			for i := 0; i < gameConst.NumItems; i++ {
				if bagIndex[i] == gameState.NumPositive {
					gameState.BagIndex[i] = gameState.NumPositive
					gameState.NumItemsInBag[gameState.NumPositive]++
				}
			}
			rank, _ := searchRank(scanner, gameState.NumPositive, gameState.Order, gameState, gameConst, 0, len(gameState.Order))

			newOrder := []int{}
			newOrder = append(newOrder, gameState.Order[:rank]...)
			newOrder = append(newOrder, gameState.NumPositive)
			newOrder = append(newOrder, gameState.Order[rank:]...)
			gameState.Order = newOrder
			gameState.NumPositive++
		}
	}
}

func getOptimalPlan(gameState *GameState, gameConst *GameConst, items []int, numBags int, durationMult float64, maxIter int, goal float64) []int {
	bagIndex := make([]int, len(items))
	bagWeight := make([]float64, numBags)
	sumWeight := 0.0
	estimates := []float64{}
	for i := range items {
		sumWeight += gameState.EstimatedWeights[items[i]]
		estimates = append(estimates, gameState.EstimatedWeights[items[i]])
	}
	bagWeight[0] = sumWeight
	idealWeight := sumWeight / float64(numBags)
	nowScore := 0.0
	for i := 0; i < numBags; i++ {
		nowScore += math.Pow(bagWeight[i]-idealWeight, 2)
	}
	bestScore := nowScore
	bestPlan := make([]int, len(bagIndex))
	copy(bestPlan, bagIndex)
	duration := 30 * 1000 * durationMult
	startTime := time.Now()
	updateTemp := (1 << 11) - 1
	prop := 0.0
	sumIter := 0
	valid := 0
	accepted := 0
	startTemp := 300000.0 / float64(numBags)
	endTemp := 2000.0 / float64(numBags)
	// startTemp := math.Pow(10000.0, 2)
	// endTemp := math.Pow(500.0, 2)
	temp := startTemp
	for {
		if (sumIter & updateTemp) == 0 {
			prop = float64(time.Since(startTime).Microseconds()) / float64(duration)
			if prop >= 1 {
				// time up
				break
			}
			// if DEBUG {
			// 	fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, nowScore: %f\n", prop, sumIter, math.Sqrt(nowScore/float64(numBags)))
			// 	fmt.Fprintf(os.Stderr, "prop: %.2f, iter: %d, bestScore: %f\n", prop, sumIter, math.Sqrt(bestScore/float64(numBags)))
			// }
			temp = math.Pow(endTemp, prop) * math.Pow(startTemp, 1-prop)
		}
		sumIter++
		if sumIter > maxIter {
			break
		}

		// ここで何かnowを変更する
		index := gameConst.random.Intn(len(items))
		oldBag := bagIndex[index]
		newBag := gameConst.random.Intn(numBags)
		if oldBag == newBag {
			continue
		}
		valid++

		score := nowScore
		score -= math.Pow(bagWeight[oldBag]-idealWeight, 2)
		score -= math.Pow(bagWeight[newBag]-idealWeight, 2)
		bagIndex[index] = newBag
		bagWeight[oldBag] -= estimates[index]
		bagWeight[newBag] += estimates[index]
		score += math.Pow(bagWeight[oldBag]-idealWeight, 2)
		score += math.Pow(bagWeight[newBag]-idealWeight, 2)
		//

		p := getAcceptRate(math.Sqrt(score/float64(numBags)), math.Sqrt(nowScore/float64(numBags)), temp)
		// p := getAcceptRate(score, nowScore, temp)
		r := gameConst.random.Float64()
		if p >= r {
			// 採用
			accepted++

			nowScore = score
			if nowScore < bestScore {
				bestScore = nowScore
				copy(bestPlan, bagIndex)
			}
			if nowScore < goal {
				break
			}
		} else {
			// rollback（nowを戻す）
			bagIndex[index] = oldBag
			bagWeight[newBag] -= estimates[index]
			bagWeight[oldBag] += estimates[index]
		}
	}
	return bestPlan
}

func solve(gameState *GameState, gameConst *GameConst, scanner *bufio.Scanner) (*Action, int) {
	start := time.Now()
	maxAction := NewAction()
	maxScore := 0.0
	_ = maxScore

	// 貪欲を書くならここ
	loop := 0
	itemOrder := []int{}
	// boarder := (int(math.Log(float64(gameConst.NumItems))/math.Log(2)) + 3) * gameConst.NumItems
	oldboarder := (int(math.Log(float64(gameConst.NumItems))/math.Log(2)+1.001))*gameConst.NumItems + (int(math.Log(float64(gameConst.NumBags))/math.Log(2)+1.001))*gameConst.NumBags + 30
	boarder := int((math.Log(float64(gameConst.NumItems))/math.Log(2)+1.001)*float64(gameConst.NumItems)) + (int(math.Log(float64(gameConst.NumBags))/math.Log(2)+1.001))*gameConst.NumBags + 30
	if gameConst.NumQueries < boarder && gameConst.NumQueries > oldboarder {
		fmt.Fprintln(os.Stderr, "!!!")

	}
	if gameConst.NumQueries > boarder {
		for {
			// 最初にアイテムの重い順を決める
			ok := true
			for i := 0; i < gameConst.NumItems; i++ {
				minIndex := 0
				maxIndex := len(itemOrder)
				for minIndex < maxIndex {
					index := (minIndex + maxIndex) / 2
					i2 := itemOrder[index]
					result := -1
					result, ok, _ = compareTwoGroups(scanner, gameState, gameConst, []int{i}, []int{i2})
					if !ok {
						break
					}
					if result == 0 {
						maxIndex = index
					}
					if result == 1 {
						maxIndex = index
					}
					if result == 2 {
						minIndex = index + 1
					}
				}
				if !ok {
					break
				}
				rank := maxIndex
				newOrder := []int{}
				newOrder = append(newOrder, itemOrder[:rank]...)
				newOrder = append(newOrder, i)
				newOrder = append(newOrder, itemOrder[rank:]...)
				itemOrder = newOrder
			}
			if !ok {
				break
			}
			s := 0.0
			for i := gameConst.NumItems - 1; i >= 0; i-- {
				s += WEIGHT_MEAN / float64(i+1)
				gameState.EstimatedWeights[itemOrder[i]] = s
				if i == 0 {
					continue
				}
				gameState.CompareHistory = append(gameState.CompareHistory, [2][]int{{itemOrder[i-1]}, {itemOrder[i]}})
				gameState.ResultHistory = append(gameState.ResultHistory, 1)
			}
			// fmt.Fprintln(os.Stderr, gameState.EstimatedWeights)
			break
		}
	}
	if len(itemOrder) != 0 {
		gameState.ItemRank = make([]int, gameConst.NumItems)
		for i, v := range itemOrder {
			gameState.ItemRank[v] = i
		}
	}
	keyIndex := len(gameState.CompareHistory)
	if len(gameState.ItemRank) != 0 {
		// if gameConst.NumQueries > gameConst.NumItems*4 {
		c := 0
		for c < 20 {
			g1, g2 := getHalf(gameState, gameConst)
			compareAndUpdate(scanner, gameState, gameConst, g1, g2)
			c++
		}
	} else {
		// c := 0
		// for c < 5 {
		// 	g1, g2 := getHalf(gameState, gameConst)
		// 	compareAndUpdate(scanner, gameState, gameConst, g1, g2)
		// 	c++
		// }
		for loop := 0; loop < int(math.Log(float64(gameConst.NumItems))+1); loop++ {
			g1 := []int{}
			g2 := []int{}
			w := [2]float64{}
			for i := 0; i < gameConst.NumItems; i++ {
				num := (i / int(fastPow(2, loop)+0.1))
				if num/2 == (gameConst.NumItems-1)/int(fastPow(2, loop)+0.1)/2 {
					if w[0] <= w[1] {
						g1 = append(g1, i)
						w[0] += gameState.EstimatedWeights[i]
					} else {
						g2 = append(g2, i)
						w[1] += gameState.EstimatedWeights[i]
					}
					continue
				}
				num2 := num % 2
				if num2 == 0 {
					g1 = append(g1, i)
					w[0] += gameState.EstimatedWeights[i]
				} else {
					g2 = append(g2, i)
					w[1] += gameState.EstimatedWeights[i]
				}
			}
			compareAndUpdate(scanner, gameState, gameConst, g1, g2)
		}
		// panic("")
	}
	setInitialState(scanner, gameState, gameConst, 0)

	// setInitialState(scanner, gameState, gameConst, 0)
	minWeight := make([]float64, gameConst.NumBags)
	maxWeight := make([]float64, gameConst.NumBags)
	averageWeight := make([]float64, gameConst.NumBags)
	fakeOrder := make([]int, gameConst.NumItems)
	for i := range fakeOrder {
		fakeOrder[i] = i
	}
	if len(gameState.ItemRank) == 0 {
		// ここでありがちな初期状態を予想しておく
		try := 0.0
		start := time.Now()
		randomWeight := make([]float64, gameConst.NumItems)
		numItemsInBag := make([]int, gameConst.NumBags)
		randomBagWeight := make([]float64, gameConst.NumBags)
		sumWeight := make([]float64, gameConst.NumBags)
		for i := range minWeight {
			minWeight[i] = 100000000
		}
		randomOrder := make([]int, gameConst.NumBags)
		for i := range randomOrder {
			randomOrder[i] = i
		}
		loop := 0.0
		for {
			loop++
			mult := 1.0
			if time.Since(start).Microseconds() > 30*1000 {
				break
			}
			for i := range randomBagWeight {
				numItemsInBag[i] = 0
				randomBagWeight[i] = 0
			}
			valid := true
			totalWeight := 0.0
			for i := range randomWeight {
				randomWeight[i] = gameConst.random.ExpFloat64() * 100000
				if randomWeight[i] > WEIGHT_MEAN*float64(gameConst.NumItems)/float64(gameConst.NumBags) {
					valid = false
					break
				}
				totalWeight += randomWeight[i]
			}

			for i := range randomWeight {
				randomWeight[i] *= float64(WEIGHT_MEAN*gameConst.NumItems) / totalWeight
			}
			for i := range randomWeight {
				randomBagWeight[gameState.BagIndex[i]] += randomWeight[i]
				numItemsInBag[gameState.BagIndex[i]]++
			}
			if !valid {
				continue
			}
			sort.Slice(randomOrder, func(i, j int) bool { return randomBagWeight[randomOrder[i]] > randomBagWeight[randomOrder[j]] })
			for i := range randomOrder {
				if numItemsInBag[randomOrder[i]] != gameState.NumItemsInBag[gameState.Order[i]] {
					mult *= 0.5
				}
			}
			try += mult
			for i := range randomBagWeight {
				sumWeight[i] += randomBagWeight[randomOrder[i]] * mult
				if loop < 1000 {
					maxWeight[i] = math.Max(maxWeight[i], randomBagWeight[randomOrder[i]])
					minWeight[i] = math.Min(minWeight[i], randomBagWeight[randomOrder[i]])
				}
			}

		}
		// fmt.Fprintln(os.Stderr, "try:", try, loop)
		oldWeights := make([]float64, gameConst.NumBags)
		for i := 0; i < gameConst.NumItems; i++ {
			oldWeights[gameState.BagIndex[i]] += gameState.EstimatedWeights[i]
		}
		for i := 0; i < gameConst.NumBags; i++ {
			bagIndex := gameState.Order[i]
			// fmt.Fprintln(os.Stderr, minWeight[i], "~", maxWeight[i])
			// fmt.Fprintln(os.Stderr, oldWeights[bagIndex], "->", (sumWeight[i] / float64(try)))
			averageWeight[i] = sumWeight[i] / try
			for j := 0; j < gameConst.NumItems; j++ {
				if gameState.BagIndex[j] == bagIndex {
					gameState.EstimatedWeights[j] *= averageWeight[i] / oldWeights[bagIndex]
				}
			}
		}

		// setInitialState(scanner, gameState, gameConst, 1)
		/*for j := 0; j < 10; j++ {
			for i := range gameState.CompareHistory {
				updateEstimation(gameState, gameConst, gameState.CompareHistory[i][0], gameState.CompareHistory[i][1], gameState.ResultHistory[i], false)
			}
		}*/
		// 賭けに出る
		// sort.Slice(fakeOrder, func(i, j int) bool {
		// 	return gameState.EstimatedWeights[fakeOrder[i]] > gameState.EstimatedWeights[fakeOrder[j]]
		// })
		// s := 0.0
		// for i := gameConst.NumItems - 1; i >= 0; i-- {
		// 	s += WEIGHT_MEAN / float64(i+1)
		// 	gameState.EstimatedWeights[fakeOrder[i]] = s
		// }

	} else {
		// ここでありがちな初期状態を予想しておく
		try := 0.0
		start := time.Now()
		sortedWeight := make([]float64, gameConst.NumItems)
		sumWeight := make([]float64, gameConst.NumItems)
		for {
			mult := 1.0
			if time.Since(start).Microseconds() > 30*1000 {
				break
			}
			valid := true
			for i := range sortedWeight {
				sortedWeight[i] = gameConst.random.ExpFloat64() * 100000
				if sortedWeight[i] > WEIGHT_MEAN*float64(gameConst.NumItems)/float64(gameConst.NumBags) {
					valid = false
					break
				}
			}
			if !valid {
				continue
			}
			sort.Slice(sortedWeight, func(i, j int) bool { return sortedWeight[i] > sortedWeight[j] })
			for c := 0; c < 20; c++ {
				g1 := gameState.CompareHistory[keyIndex+c][0]
				g2 := gameState.CompareHistory[keyIndex+c][1]
				w1 := 0.0
				w2 := 0.0
				for _, v := range g1 {
					w1 += sortedWeight[gameState.ItemRank[v]]
				}
				for _, v := range g2 {
					w2 += sortedWeight[gameState.ItemRank[v]]
				}
				result := gameState.ResultHistory[keyIndex+c]
				if result == 1 {
					if w1 < w2 {
						mult *= 0.5 / (1 + 100*(w2-w1)/(w2+w1))
					}
				}
				if result == 2 {
					if w1 > w2 {
						mult *= 0.5 / (1 + 100*(w1-w2)/(w2+w1))
					}
				}
			}
			try += mult
			// fmt.Fprintln(os.Stderr, "mult:", mult)
			for i := range sumWeight {
				sumWeight[i] += sortedWeight[i] * mult
			}
		}
		// fmt.Fprintln(os.Stderr, "try:", try)
		// sum1 := 0.0
		// sum2 := 0.0
		// sum3 := 0.0
		// for i := range gameState.EstimatedWeights {
		// 	sum1 += gameState.EstimatedWeights[i]
		// 	sum2 += sumWeight[i] / try
		// 	sum3 += float64(gameConst.TrueWeights[i])
		// }
		// d1 := 0.0
		// d2 := 0.0
		for i := range gameState.EstimatedWeights {
			// fmt.Fprintln(os.Stderr, gameState.EstimatedWeights[i]/sum1, "->", (sumWeight[gameState.ItemRank[i]]/float64(try))/sum2, float64(gameConst.TrueWeights[i])/sum3)
			// d1 += math.Pow(gameState.EstimatedWeights[i]/sum1-float64(gameConst.TrueWeights[i])/sum3, 2)
			// d2 += math.Pow((sumWeight[gameState.ItemRank[i]]/float64(try))/sum2-float64(gameConst.TrueWeights[i])/sum3, 2)
			gameState.EstimatedWeights[i] = sumWeight[gameState.ItemRank[i]] / float64(try)
		}
		// fmt.Fprintln(os.Stderr, math.Sqrt(d1/float64(gameConst.NumItems))*100, math.Sqrt(d2/float64(gameConst.NumItems))*100)

	}
	if DEBUG {
		printAction(maxAction, gameState, gameConst, true)
	}
	// fmt.Fprintln(os.Stderr, b1, rank)
	failcount := 0
	failcount2 := 0
	for i := range gameState.Order {
		if i == 0 {
			continue
		}
		bag1 := gameState.Order[i-1]
		bag2 := gameState.Order[i]
		g1 := []int{}
		g2 := []int{}
		for j := range gameState.BagIndex {
			if gameState.BagIndex[j] == bag1 {
				g1 = append(g1, j)
			}
			if gameState.BagIndex[j] == bag2 {
				g2 = append(g2, j)
			}
		}
		gameState.CompareHistory = append(gameState.CompareHistory, [2][]int{g1, g2})
		gameState.ResultHistory = append(gameState.ResultHistory, 1)
		groups := make([][]int, gameConst.NumBags)
		for i := range gameState.Order {
			for j := range gameState.BagIndex {
				if gameState.BagIndex[j] == gameState.Order[i] {
					groups[i] = append(groups[i], j)
				}
			}
		}
	}
	mode := 1
	modeDone := make([]int, gameConst.NumItems)
	if len(itemOrder) != 0 {
		groups := [][]int{}
		for i := range itemOrder {
			groups = append(groups, []int{itemOrder[i]})
		}
		gameState.RankHistory = append(gameState.RankHistory, groups)
	}
	done := make([][]int, gameConst.NumItems)
	for i := range done {
		done[i] = make([]int, gameConst.NumItems)
	}
	ecomode := false
	for {
		/*if len(gameState.ItemRank) == 0 {
			// 可能な限り推測
			for i := 0; i < gameConst.NumItems; i++ {
				for j := i + 1; j < gameConst.NumItems; j++ {
					if done[i][j] == 1 {
						continue
					}
					r, sure := check(gameState, gameConst, []int{i}, []int{j})
					if sure {
						// mark := ">"
						success := gameConst.TrueWeights[i] > gameConst.TrueWeights[j]
						if r == 2 {
							success = !success
							// mark = "<"
						}
						// fmt.Fprintln(os.Stderr, "多分", i, mark, j, success)
						gameState.CompareHistory = append(gameState.CompareHistory, [2][]int{[]int{i}, []int{j}})
						gameState.ResultHistory = append(gameState.ResultHistory, r)
						done[i][j] = 1

					}
				}
			}
		}*/
		now := time.Since(start).Microseconds()
		if now > TIME_LIMIT {
			fmt.Fprintln(os.Stderr, "timeout!", gameState.QueryCount, gameConst.NumQueries, float64(gameConst.NumQueries)/float64(gameConst.NumItems), gameConst.NumBags, float64(gameConst.NumItems)/float64(gameConst.NumBags))
			break
		}
		failcount++
		/*if failcount > 10 {
			mode = 0
		}*/
		if len(itemOrder) == 0 {
			mode = 1
		} else {
			mode = 0
		}
		loop++
		if loop > 100000 {
			fmt.Fprintln(os.Stderr, "too many loops")
			break
		}
		if len(gameState.ItemRank) == 0 && gameState.QueryCount*2 > gameConst.NumQueries {
			for loop := 0; loop < 3; loop++ {
				for i := range gameState.CompareHistory {
					updateEstimation(gameState.EstimatedWeights, gameState, gameConst, gameState.CompareHistory[i][0], gameState.CompareHistory[i][1], gameState.ResultHistory[i], false, 1, false)
				}
			}
			for i := range gameState.RankHistory {
				updateEstimationByRank(gameState.EstimatedWeights, gameState, gameConst, gameState.RankHistory[i])
			}
		}
		for i := range gameState.RankHistory {
			updateEstimationByRank(gameState.EstimatedWeights, gameState, gameConst, gameState.RankHistory[i])
		}
		if int(float64(gameConst.NumQueries-gameState.QueryCount)/math.Log(float64(gameConst.NumBags)+0.1)) < 0 {
			ecomode = true
		}
		setRank(scanner, gameState, gameConst, []int{}, ecomode)
		/*if gameState.NumPositive == gameConst.NumBags {
		break
		}*/
		if gameState.QueryCount == gameConst.NumQueries {
			break
		}
		// 大きいほうの袋番号
		b1 := -1
		// 小さいほうの袋番号
		b2 := -1
		if failcount > 200 {
			if gameConst.NumBags == 2 || time.Since(start).Microseconds() < int64(TIME_LIMIT*0.5) /* gameState.QueryCount < gameConst.NumQueries*7/10*/ {
				g1, g2 := getHalf(gameState, gameConst)
				compareAndUpdate(scanner, gameState, gameConst, g1, g2)

			} else {
				// もう無理そうなので3点で強引に入れ替えを考える
				r := []int{100, 100, 100}
				for i := range r {
					if i == 1 {
						r[i] = gameConst.random.Intn(gameConst.NumBags) / 3
					}
					r[i] = int(math.Sqrt(gameConst.random.Float64())*float64(gameConst.NumBags-i-(gameConst.NumBags)/3)) + gameConst.random.Intn(gameConst.NumBags-i)/3
					for j := 0; j < i; j++ {
						if r[i] >= r[j] {
							r[i]++
						}
					}
					sort.Slice(r, func(i, j int) bool { return r[i] < r[j] })
				}
				b := [3]int{}
				for i := range b {
					b[i] = gameState.Order[r[i]]
				}
				// b[0] = gameState.Order[0]
				// b[2] = gameState.Order[len(gameState.Order)-1]
				// b[1] = gameState.Order[gameConst.random.Intn(len(gameState.Order)-2)+1]
				items := []int{}
				w := [3]float64{}
				for i := range gameState.BagIndex {
					for j := range b {
						if gameState.BagIndex[i] == b[j] {
							items = append(items, i)
							w[j] += gameState.EstimatedWeights[i]
						}
					}
				}
				idealw := (w[0] + w[1] + w[2]) / 3
				v := 0.0
				for i := range w {
					v += math.Pow(idealw-w[i], 2)
				}
				plan := getOptimalPlan(gameState, gameConst, items, 3, 5, 300000, v/5)
				// w = [3]float64{}
				// for i := range gameState.BagIndex {
				// 	for j := range b {
				// 		if gameState.BagIndex[i] == b[j] {
				// 			w[j] += float64(gameConst.TrueWeights[i])
				// 		}
				// 	}
				// }
				newW := [3]float64{}
				for i := range plan {
					// newW[plan[i]] += float64(gameConst.TrueWeights[items[i]])
					newW[plan[i]] += gameState.EstimatedWeights[items[i]]
				}
				newv := 0.0
				for i := range newW {
					newv += math.Pow(idealw-newW[i], 2)
				}
				// fmt.Fprintln(os.Stderr, newv < v)
				// fmt.Fprintln(os.Stderr, w)
				// fmt.Fprintln(os.Stderr, newW)
				// fmt.Fprintln(os.Stderr, "")
				// 成功（疑似）の条件はすべての最大値が元の最大値を下回っていること
				result := 0
				ok := true
				good := true
				for bIndex := range b {
					count := make([]int, gameConst.NumItems)
					for i := range gameState.BagIndex {
						if gameState.BagIndex[i] == b[0] {
							count[i]++
						}
					}
					for i := range plan {
						if plan[i] == bIndex {
							count[items[i]]--
						}
					}
					g1 := []int{}
					g2 := []int{}
					for i := range count {
						if count[i] == 1 {
							g1 = append(g1, i)
						}
						if count[i] == -1 {
							g2 = append(g2, i)
						}
					}
					result, ok = compareAndUpdate(scanner, gameState, gameConst, g1, g2)
					if !ok {
						break
					}
					if result != 1 {
						good = false
						break
					}
				}
				if !ok {
					break
				}
				if good {
					fmt.Fprintln(os.Stderr, good)
					// 思い切って更新する
					for i := range items {
						gameState.NumItemsInBag[gameState.BagIndex[items[i]]]--
						gameState.BagIndex[items[i]] = b[plan[i]]
						gameState.NumItemsInBag[gameState.BagIndex[items[i]]]++
					}
					setRank(scanner, gameState, gameConst, b[:], false)
				}
			}
			failcount = 0
			continue
		} else if gameState.NumPositive < gameConst.NumBags || ( /*gameConst.NumQueries < 16*gameConst.NumItems &&*/ failcount < 40) {
			// まずは空のバッグをなくす
			for i := 0; i < gameState.NumPositive; i++ {
				b1 = gameState.Order[i]
				if gameState.NumItemsInBag[b1] > 1 {
					break
				}
			}
			if gameState.NumPositive < gameConst.NumBags {
				b2 = gameState.Order[gameState.NumPositive]
			} else {
				b2 = gameState.Order[len(gameState.Order)-1]
			}
		} else {
			r1 := gameConst.random.Float64()
			index1 := int(math.Pow(r1, 2)*float64(gameConst.NumBags-1)) / 2
			r2 := gameConst.random.Float64()
			index2 := gameConst.NumBags - 1 - int(math.Pow(r2, 2)*float64(gameConst.NumBags-index1-1))/2
			b1 = gameState.Order[index1]
			b2 = gameState.Order[index2]
		}
		// fmt.Fprintln(os.Stderr, b1, b2)
		/*for i := range gameState.EstimatedWeights {
			gameState.EstimatedWeights[i] = float64(gameConst.TrueWeights[i])
		}*/
		group1 := [2][]int{}
		group2 := [2][]int{}
		promised := true
		if mode == 1 {
			group1 = [2][]int{}
			group2 = [2][]int{}
			ids := []int{}
			for i := range gameState.BagIndex {
				if gameState.BagIndex[i] == b1 {
					group1[0] = append(group1[0], i)
					ids = append(ids, len(ids))
				}
				if gameState.BagIndex[i] == b2 {
					group2[0] = append(group2[0], i)
				}
			}
			sort.Slice(ids, func(i, j int) bool {
				return gameState.EstimatedWeights[group1[0][ids[i]]] > gameState.EstimatedWeights[group1[0][ids[j]]]
			})
			minId := -1
			for i := range ids {
				if modeDone[group1[0][ids[i]]] != 0 {
					continue
				}
				modeDone[group1[0][ids[i]]] += 1
				minId = ids[i]
				break
			}
			if minId == -1 {
				mode = 0
				// fmt.Fprintln(os.Stderr, gameState.QueryCount)
				// continue
			} else {
				// minId = gameConst.random.Intn(len(group1))
				newg := []int{}
				newg = append(newg, group1[0][:minId]...)
				newg = append(newg, group1[0][minId+1:]...)
				group1[1] = append(group1[1], group1[0][minId])
				group1[0] = newg
			}
		}
		if mode == 0 {
			if failcount2 < 2 && len(gameState.ItemRank) == 0 {
				group1, group2, promised = getReasonableDivision(gameState, gameConst, b1, b2, 1)
				if !promised {
					failcount2++
				}
			} else {
				group1, group2, promised = getReasonableDivision(gameState, gameConst, b1, b2, 0)
			}
			// fmt.Fprintln(os.Stderr, promised)
		}
		if !promised {
			if time.Since(start).Microseconds() < TIME_LIMIT*int64(float64(gameState.QueryCount)/float64(gameConst.NumQueries)) {
				p := gameConst.random.Float64()
				if p < 0.8 {
					continue
				}
			}
		}
		// fmt.Fprintln(os.Stderr, failcount2)
		if promised {
			failcount2 = 0
		}
		r, sure := 0, false
		if time.Since(start).Microseconds() < TIME_LIMIT*int64(math.Pow(float64(gameState.QueryCount)/float64(gameConst.NumQueries), 2)) {
			r, sure = check(gameState, gameConst, group1[1], group2[1])

		}
		swap := false
		if sure && r == 2 {
			// 不穏なのでこちらから済ませる
			swap = true
		} else {
			r2, sure2 := 0, false
			if time.Since(start).Microseconds() < TIME_LIMIT*int64(math.Pow(float64(gameState.QueryCount)/float64(gameConst.NumQueries), 2)) {
				r2, sure2 = check(gameState, gameConst, group1[0], group2[0])
			}
			if sure2 && r2 == 1 {
				// 安心なので後回しにする
				swap = true
			}
		}
		if swap {
			// fmt.Fprintln(os.Stderr, "swap!")
			// fmt.Println("# swap!")
			group1[0], group1[1] = group1[1], group1[0]
			group2[0], group2[1] = group2[1], group2[0]
		}

		result, ok := compareAndUpdate(scanner, gameState, gameConst, group1[0], group2[0])
		if !ok {
			break
		}
		if result != 1 {
			// if swap {
			// 	fmt.Fprintln(os.Stderr, "hit!")
			// 	// 	fmt.Println("# hit!")
			// }
			if mode == 1 {
				modeDone[group1[1][0]]++
			}
			if DEBUG {
				fmt.Println("# 失敗したので新しい挑戦")
			}
			continue
		} else {
			if mode == 1 {
				for i := 0; i < gameConst.NumItems; i++ {
					if i == group1[1][0] {
						continue
					}
					if modeDone[i] == 2 {
						// fmt.Fprintln(os.Stderr, i, ">", group1[1][0])
						gameState.CompareHistory = append(gameState.CompareHistory, [2][]int{{i}, {group1[1][0]}})
						gameState.ResultHistory = append(gameState.ResultHistory, 1)
					}
				}
			}
		}
		result, ok = compareAndUpdate(scanner, gameState, gameConst, group1[1], group2[1])
		if !ok {
			break
		}
		if result != 1 {
			// if swap {
			// 	fmt.Fprintln(os.Stderr, "...")
			// 	// 	fmt.Println("# hit!")
			// }
			if DEBUG {
				fmt.Println("# 失敗したので新しい挑戦")
			}
			continue
		}
		failcount = 0
		for _, v := range group1[0] {
			gameState.BagIndex[v] = b1
		}
		for _, v := range group2[1] {
			gameState.BagIndex[v] = b1
		}
		gameState.NumItemsInBag[b1] = len(group1[0]) + len(group2[1])
		for _, v := range group1[1] {
			gameState.BagIndex[v] = b2
		}
		for _, v := range group2[0] {
			gameState.BagIndex[v] = b2
		}
		gameState.NumItemsInBag[b2] = len(group1[1]) + len(group2[0])
		if gameState.NumPositive < gameConst.NumBags {
			gameState.NumPositive++
		}

		setRank(scanner, gameState, gameConst, []int{b1, b2}, ecomode)
		if DEBUG {
			printAction(maxAction, gameState, gameConst, true)
		}
		/*for i := range gameState.Order {
			if i == 0 {
				continue
			}
			bag1 := gameState.Order[i-1]
			bag2 := gameState.Order[i]
			g1 := []int{}
			g2 := []int{}
			for j := range gameState.BagIndex {
				if gameState.BagIndex[j] == bag1 {
					g1 = append(g1, j)
				}
				if gameState.BagIndex[j] == bag2 {
					g2 = append(g2, j)
				}
			}
			gameState.CompareHistory = append(gameState.CompareHistory, [2][]int{g1, g2})
			gameState.ResultHistory = append(gameState.ResultHistory, 1)
		}*/
		if !ecomode {
			groups := make([][]int, gameConst.NumBags)
			for i := range gameState.Order {
				for j := range gameState.BagIndex {
					if gameState.BagIndex[j] == gameState.Order[i] {
						groups[i] = append(groups[i], j)
					}
				}
			}
			// fmt.Fprintln(os.Stderr, gameState.Order)
			// fmt.Fprintln(os.Stderr, groups)
			gameState.RankHistory = append(gameState.RankHistory, groups)
		}
	}
	if DEBUG {
		fmt.Fprintln(os.Stderr, "#", gameState.QueryCount)
	}
	// 埋める
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		for gameState.QueryCount < gameConst.NumQueries {
			fmt.Println("1 1 0 1")
			gameState.QueryCount++
		}
	}
	// fmt.Println(gameState.BagIndex)
	// 貪欲ここまで

	sa := NewSA()
	sa.best = maxAction
	// sa.bestScore = float64(gameState.score)
	//
	sa.now = maxAction
	sa.nowScore = maxScore
	sa.startTime = start
	// sa.Run(gameState, gameConst)
	score := 0
	return maxAction, score
}

func doGame(file string) Result {
	result := Result{}
	var scanner *bufio.Scanner
	scanner = getScanner(FILE + "\\" + file + ".txt")

	// 初期値設定処理
	gameConst := NewGameConst()
	// データ取得
	getInputs(scanner, gameConst)

	gameState := NewGameState(gameConst)

	// 初期値設定処理　ここまで

	// cGameState := gameState.copy()
	// if gameConst.NumQueries > 4*gameConst.NumItems && gameConst.NumQueries < 8*gameConst.NumItems {
	// 	panic("")
	// }
	action, score := solve(gameState, gameConst, scanner)
	// アクションを出力
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		printAction(action, gameState, gameConst, false)
	}
	if DEBUG {
		fmt.Fprintln(os.Stderr, gameState.Order)
		sumTrueWeight := 0
		sumWeight := 0.0
		for i := 0; i < gameConst.NumItems; i++ {
			sumTrueWeight += gameConst.TrueWeights[i]
			sumWeight += gameState.EstimatedWeights[i]
		}
		v := 0.0
		for i := 0; i < gameConst.NumItems; i++ {
			// fmt.Fprintln(os.Stderr, float64(gameConst.TrueWeights[i])/float64(sumTrueWeight), gameState.EstimatedWeights[i]/sumWeight)
			v += math.Pow(float64(gameConst.TrueWeights[i])/float64(sumTrueWeight)-gameState.EstimatedWeights[i]/sumWeight, 2)
		}
		fmt.Fprintln(os.Stderr, math.Sqrt(v/float64(gameConst.NumItems)))
		for i := 0; i < gameConst.NumItems; i++ {
			fmt.Fprintf(os.Stderr, "%d ", int(gameState.EstimatedWeights[i]+1))

		}
	}
	/*for i := 0; i < gameConst.NumQueries; i++ {
		fmt.Fprintln(os.Stderr, gameState.CompareHistory[i][0])
		fmt.Fprintln(os.Stderr, gameState.CompareHistory[i][1])
		fmt.Fprintln(os.Stderr, gameState.ResultHistory[i])
	}*/

	if LOCAL_TEST {
		for i := range gameState.BagIndex {
			if gameState.BagIndex[i] < 0 {
				panic("")
			}
		}
		for i := 0; i < len(gameState.CompareHistory); i++ {

		}
		score = calcScore(gameState, gameConst)
	}

	result.score = score

	return result
}

func calcScore(gameState *GameState, gameConst *GameConst) int {
	sumWeight := 0
	for i := range gameConst.TrueWeights {
		sumWeight += gameConst.TrueWeights[i]
	}
	meanWeight := float64(sumWeight) / float64(gameConst.NumBags)
	V := 0.0
	for i := 0; i < gameConst.NumBags; i++ {
		weight := 0
		for j := range gameState.BagIndex {
			if gameState.BagIndex[j] == i {
				weight += gameConst.TrueWeights[j]
			}
		}
		V += math.Pow((meanWeight - float64(weight)), 2)
	}
	V /= float64(gameConst.NumBags)
	score := 1 + int(100*math.Sqrt(V))
	return score
}
