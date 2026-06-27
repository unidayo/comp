// last local submission: 1743964134
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

var WEIGHT_PENALTY = [799]int{632, 579, 548, 530, 517, 508, 500, 494, 488, 483, 479, 475, 471, 468, 464, 461, 458, 455, 453, 451, 448, 446, 445, 443, 441, 439, 437, 435, 433, 432, 430, 429, 427, 426, 425, 423, 421, 420, 419, 418, 416, 415, 414, 413, 412, 410, 409, 408, 407, 406, 405, 404, 403, 402, 401, 400, 399, 398, 397, 396, 395, 394, 393, 392, 392, 391, 390, 389, 388, 387, 386, 386, 385, 384, 383, 382, 381, 380, 380, 379, 378, 377, 376, 376, 375, 374, 374, 373, 372, 371, 371, 370, 369, 369, 368, 367, 367, 366, 365, 365, 364, 363, 363, 362, 361, 360, 360, 359, 359, 358, 357, 357, 356, 355, 355, 354, 354, 353, 353, 352, 351, 351, 350, 349, 349, 348, 348, 347, 347, 346, 345, 345, 344, 344, 343, 343, 342, 341, 341, 340, 340, 339, 339, 338, 338, 337, 337, 336, 336, 335, 334, 334, 333, 333, 332, 332, 331, 331, 330, 330, 329, 329, 328, 327, 327, 327, 326, 325, 325, 324, 324, 323, 323, 322, 322, 322, 321, 321, 320, 319, 319, 318, 318, 318, 317, 317, 316, 316, 315, 315, 314, 314, 313, 313, 313, 312, 312, 311, 311, 310, 310, 309, 309, 308, 308, 307, 307, 306, 306, 305, 305, 305, 304, 304, 303, 303, 302, 302, 301, 301, 300, 300, 299, 299, 299, 298, 298, 297, 297, 296, 296, 295, 295, 295, 294, 294, 293, 293, 292, 292, 292, 291, 291, 290, 290, 289, 289, 289, 288, 288, 287, 287, 286, 286, 286, 285, 285, 284, 284, 283, 283, 282, 282, 282, 281, 281, 280, 280, 279, 279, 279, 278, 278, 277, 277, 277, 276, 276, 275, 275, 274, 274, 274, 273, 273, 272, 272, 271, 271, 271, 270, 270, 269, 269, 269, 268, 268, 267, 267, 267, 266, 266, 266, 265, 265, 264, 264, 264, 263, 263, 262, 262, 262, 261, 261, 260, 260, 260, 259, 259, 258, 258, 258, 257, 257, 256, 256, 255, 255, 255, 254, 254, 253, 253, 253, 252, 252, 252, 251, 251, 250, 250, 250, 249, 249, 248, 248, 247, 247, 247, 246, 246, 246, 245, 245, 244, 244, 244, 243, 243, 243, 242, 242, 241, 241, 241, 240, 240, 240, 239, 239, 238, 238, 238, 237, 237, 236, 236, 236, 235, 235, 235, 234, 234, 233, 233, 233, 232, 232, 231, 231, 231, 230, 230, 229, 229, 229, 228, 228, 228, 227, 227, 226, 226, 226, 225, 225, 224, 224, 224, 223, 223, 222, 222, 222, 221, 221, 221, 220, 220, 219, 219, 219, 218, 218, 218, 217, 217, 216, 216, 216, 215, 215, 215, 214, 214, 213, 213, 213, 212, 212, 212, 211, 211, 211, 210, 210, 209, 209, 209, 208, 208, 208, 207, 207, 207, 206, 206, 205, 205, 205, 204, 204, 204, 203, 203, 202, 202, 202, 201, 201, 201, 200, 200, 199, 199, 199, 198, 198, 198, 197, 197, 196, 196, 196, 195, 195, 195, 194, 194, 194, 193, 193, 192, 192, 192, 191, 191, 191, 190, 190, 189, 189, 189, 188, 188, 187, 187, 187, 186, 186, 185, 185, 185, 184, 184, 183, 183, 183, 182, 182, 182, 181, 181, 180, 180, 180, 179, 179, 178, 178, 178, 177, 177, 176, 176, 176, 175, 175, 174, 174, 174, 173, 173, 172, 172, 172, 171, 171, 170, 170, 170, 169, 169, 168, 168, 167, 167, 167, 166, 166, 165, 165, 165, 164, 164, 163, 163, 162, 162, 162, 161, 161, 161, 160, 160, 159, 159, 158, 158, 158, 157, 157, 156, 156, 155, 155, 155, 154, 154, 153, 153, 152, 152, 152, 151, 151, 150, 150, 150, 149, 149, 148, 148, 147, 147, 147, 146, 146, 145, 145, 144, 144, 143, 143, 143, 142, 142, 141, 141, 141, 140, 140, 139, 139, 138, 138, 138, 137, 137, 136, 136, 135, 135, 134, 134, 134, 133, 133, 132, 132, 131, 131, 130, 130, 129, 129, 128, 128, 128, 127, 127, 126, 126, 125, 125, 124, 124, 123, 123, 122, 122, 121, 121, 120, 120, 119, 119, 118, 118, 117, 117, 116, 116, 115, 115, 114, 114, 113, 113, 112, 112, 111, 111, 110, 109, 109, 108, 108, 107, 107, 106, 106, 105, 105, 104, 104, 103, 103, 102, 102, 101, 101, 100, 99, 99, 98, 98, 97, 97, 96, 95, 95, 94, 93, 93, 92, 92, 91, 91, 90, 89, 89, 88, 87, 87, 86, 86, 85, 84, 84, 83, 82, 82, 81, 81, 80, 79, 79, 78, 77, 77, 76, 75, 75, 74, 73, 73, 72, 71, 70, 69, 69, 68, 67, 66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 40, 39, 38, 37, 35, 34, 32, 31, 29, 28, 26, 23, 21, 19, 16, 13, 9}
var SUM_WEIGHT_PENALTY = [799]int{620, 1194, 1743, 2276, 2796, 3308, 3812, 4309, 4799, 5283, 5763, 6239, 6711, 7180, 7645, 8107, 8566, 9022, 9476, 9927, 10376, 10823, 11268, 11711, 12151, 12590, 13027, 13462, 13895, 14327, 14758, 15187, 15615, 16042, 16467, 16891, 17314, 17735, 18155, 18574, 18991, 19407, 19822, 20236, 20649, 21061, 21471, 21880, 22288, 22695, 23101, 23506, 23910, 24313, 24715, 25116, 25516, 25915, 26313, 26710, 27106, 27501, 27896, 28290, 28683, 29075, 29466, 29856, 30245, 30633, 31020, 31407, 31793, 32178, 32562, 32945, 33328, 33710, 34091, 34471, 34851, 35230, 35608, 35985, 36361, 36737, 37112, 37486, 37859, 38232, 38604, 38975, 39345, 39715, 40084, 40452, 40820, 41187, 41553, 41919, 42284, 42648, 43012, 43375, 43737, 44099, 44460, 44820, 45180, 45539, 45898, 46256, 46613, 46969, 47325, 47680, 48034, 48388, 48741, 49094, 49446, 49798, 50149, 50499, 50849, 51198, 51547, 51895, 52243, 52590, 52936, 53282, 53627, 53972, 54316, 54659, 55002, 55344, 55686, 56027, 56368, 56708, 57048, 57387, 57726, 58064, 58401, 58738, 59074, 59410, 59745, 60080, 60414, 60747, 61080, 61412, 61744, 62075, 62406, 62736, 63066, 63395, 63724, 64052, 64380, 64707, 65034, 65360, 65686, 66011, 66336, 66660, 66984, 67307, 67630, 67952, 68274, 68595, 68916, 69236, 69556, 69875, 70194, 70512, 70830, 71147, 71464, 71780, 72096, 72411, 72726, 73040, 73354, 73667, 73980, 74292, 74604, 74915, 75226, 75536, 75846, 76155, 76464, 76772, 77080, 77387, 77694, 78001, 78307, 78613, 78918, 79223, 79527, 79831, 80134, 80437, 80739, 81041, 81343, 81644, 81945, 82245, 82545, 82844, 83143, 83441, 83739, 84037, 84334, 84631, 84927, 85223, 85519, 85814, 86109, 86403, 86697, 86990, 87283, 87575, 87867, 88159, 88450, 88741, 89031, 89321, 89610, 89899, 90187, 90475, 90763, 91050, 91337, 91624, 91910, 92196, 92481, 92766, 93050, 93334, 93618, 93901, 94184, 94466, 94748, 95029, 95310, 95591, 95871, 96151, 96430, 96709, 96987, 97265, 97543, 97820, 98097, 98373, 98649, 98925, 99200, 99475, 99749, 100023, 100296, 100569, 100842, 101114, 101386, 101657, 101928, 102198, 102468, 102738, 103007, 103276, 103544, 103812, 104079, 104346, 104613, 104879, 105145, 105410, 105675, 105939, 106203, 106467, 106730, 106993, 107255, 107517, 107779, 108040, 108301, 108561, 108821, 109081, 109340, 109599, 109857, 110115, 110373, 110630, 110887, 111143, 111399, 111655, 111910, 112165, 112419, 112673, 112927, 113180, 113433, 113686, 113938, 114190, 114441, 114692, 114943, 115193, 115443, 115692, 115941, 116190, 116438, 116686, 116934, 117181, 117428, 117674, 117920, 118166, 118411, 118656, 118900, 119144, 119388, 119631, 119874, 120116, 120358, 120600, 120841, 121082, 121322, 121562, 121802, 122041, 122280, 122518, 122756, 122994, 123231, 123468, 123704, 123940, 124176, 124411, 124646, 124881, 125115, 125349, 125583, 125816, 126049, 126281, 126513, 126744, 126975, 127206, 127436, 127666, 127896, 128125, 128354, 128583, 128811, 129039, 129266, 129493, 129720, 129946, 130172, 130398, 130623, 130848, 131072, 131296, 131519, 131742, 131965, 132187, 132409, 132631, 132852, 133073, 133294, 133514, 133734, 133953, 134172, 134391, 134609, 134827, 135045, 135262, 135479, 135695, 135911, 136127, 136342, 136557, 136772, 136986, 137200, 137413, 137626, 137839, 138051, 138263, 138475, 138686, 138897, 139107, 139317, 139527, 139736, 139945, 140154, 140362, 140570, 140777, 140984, 141191, 141397, 141603, 141808, 142013, 142218, 142422, 142626, 142830, 143033, 143236, 143439, 143641, 143843, 144044, 144245, 144446, 144646, 144846, 145045, 145244, 145443, 145641, 145839, 146036, 146233, 146430, 146626, 146822, 147018, 147213, 147408, 147602, 147796, 147990, 148183, 148376, 148568, 148760, 148952, 149143, 149334, 149524, 149714, 149904, 150093, 150282, 150470, 150658, 150846, 151033, 151220, 151406, 151592, 151778, 151963, 152148, 152333, 152517, 152701, 152884, 153067, 153250, 153432, 153614, 153796, 153977, 154158, 154338, 154518, 154698, 154877, 155056, 155235, 155413, 155591, 155768, 155945, 156122, 156298, 156474, 156649, 156824, 156999, 157173, 157347, 157520, 157693, 157866, 158038, 158210, 158381, 158552, 158723, 158893, 159063, 159232, 159401, 159570, 159738, 159906, 160073, 160240, 160406, 160572, 160738, 160903, 161068, 161232, 161396, 161560, 161723, 161886, 162048, 162210, 162371, 162532, 162693, 162853, 163013, 163172, 163331, 163489, 163647, 163805, 163962, 164119, 164275, 164431, 164587, 164742, 164897, 165051, 165205, 165359, 165512, 165665, 165817, 165969, 166121, 166272, 166423, 166573, 166723, 166872, 167021, 167169, 167317, 167465, 167612, 167759, 167905, 168051, 168197, 168342, 168487, 168631, 168775, 168918, 169061, 169203, 169345, 169487, 169628, 169769, 169909, 170049, 170188, 170327, 170466, 170604, 170741, 170878, 171015, 171151, 171287, 171422, 171557, 171691, 171825, 171959, 172092, 172225, 172357, 172489, 172620, 172751, 172881, 173011, 173140, 173269, 173397, 173525, 173652, 173779, 173905, 174031, 174156, 174281, 174405, 174529, 174653, 174776, 174899, 175021, 175143, 175264, 175385, 175505, 175625, 175744, 175863, 175981, 176099, 176216, 176333, 176449, 176565, 176681, 176796, 176910, 177024, 177137, 177250, 177362, 177474, 177585, 177696, 177806, 177916, 178025, 178134, 178242, 178350, 178457, 178564, 178670, 178776, 178881, 178985, 179089, 179192, 179295, 179397, 179499, 179600, 179701, 179801, 179901, 180000, 180098, 180196, 180293, 180390, 180486, 180581, 180676, 180770, 180864, 180957, 181050, 181142, 181234, 181325, 181415, 181505, 181594, 181683, 181771, 181858, 181945, 182031, 182117, 182202, 182286, 182370, 182453, 182535, 182617, 182698, 182778, 182858, 182937, 183015, 183093, 183170, 183246, 183322, 183397, 183471, 183545, 183618, 183690, 183761, 183832, 183902, 183971, 184039, 184106, 184173, 184239, 184304, 184368, 184431, 184494, 184556, 184617, 184677, 184736, 184794, 184851, 184908, 184964, 185019, 185073, 185126, 185178, 185230, 185280, 185329, 185377, 185424, 185470, 185515, 185559, 185602, 185644, 185685, 185725, 185763, 185800, 185836, 185870, 185903, 185934, 185964, 185992, 186018, 186042, 186064, 186083, 186099, 186112, 186121}

const (
	INF = 10000000000
	// Main
	TIME_LIMIT = 1800 * 1000
	START_TEMP = 10
	END_TEMP   = 0.01
	NUM_SAMPLE = 15

	MINIMIZE = true

	LOCAL_TEST = false // テストファイルを利用するか

	SUCCESSIVE         = true  // 複数ケースを試すか
	REPEAT             = false // 同一ケースを回すか
	TEST_NUM           = 100
	PARALLEL_NUM       = 6 // 6
	PREFIX             = 0 // nケース目から始めたい場合
	OUT_FOLDER         = "ignore\\out.txt"
	FILE               = "ignore\\in"    // テストケースの保存場所
	FILE_NUMBER        = "0000"          // 単一ケースを試す場合の番号
	RECORD             = true            // nケース結果を保存するか
	RECORD_FOLDER      = "ignore\\tries" // nケース結果保存場所
	RECORD_CODE        = true
	CODE_FILE          = "ignore\\code" // テストケースの保存場所
	USE_RELATIVE_SCORE = false
	BEST_FILE          = "ignore\\tries\\bests.txt"
	DEBUG              = true
	PRINT_ACTION       = true // 標準出力へ行動を出力するか

	VERSION_MAJOR = 1             // 基本戦術、方針レベルの違い
	VERSION_MINOR = 0             // 改善等
	VERSION_PATCH = 0             // バグ修正,パラメータ変更など
	VERSION_NAME  = "ignore_time" // バージョン名
	VERSION_VALID = true          // 提出に利用できる正当なものか

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
	N, _ := strconv.Atoi(splitedInput[0])
	M, _ := strconv.Atoi(splitedInput[1])
	Q, _ := strconv.Atoi(splitedInput[2])
	L, _ := strconv.Atoi(splitedInput[3])
	W, _ := strconv.Atoi(splitedInput[4])
	gConst.NumCities = N
	gConst.NumGroups = M
	gConst.NumQueries = Q
	gConst.MaxQuerySize = L
	gConst.MaxErrorRange = W

	gConst.Scanner.Scan()
	splitedInput = strings.Split(gConst.Scanner.Text(), " ")
	gConst.GroupSizes = make([]int, gConst.NumGroups)
	gConst.GoalSizeCount = make([]int, gConst.NumCities+1)
	for i := 0; i < gConst.NumGroups; i++ {
		G, _ := strconv.Atoi(splitedInput[i])
		gConst.GroupSizes[i] = G
		for j := 0; j <= G; j++ {
			gConst.GoalSizeCount[j]++
		}
	}
	gConst.ls = make([][2]int, gConst.NumCities)
	gConst.rs = make([][2]int, gConst.NumCities)
	for i := 0; i < gConst.NumCities; i++ {
		gConst.Scanner.Scan()
		splitedInput = strings.Split(gConst.Scanner.Text(), " ")
		lx, _ := strconv.Atoi(splitedInput[0])
		rx, _ := strconv.Atoi(splitedInput[1])
		ly, _ := strconv.Atoi(splitedInput[2])
		ry, _ := strconv.Atoi(splitedInput[3])
		gConst.ls[i][0] = lx
		gConst.ls[i][1] = ly
		gConst.rs[i][0] = rx
		gConst.rs[i][1] = ry

	}
	gConst.trueData = NewTrueData(gConst)
	if LOCAL_TEST {
		for i := 0; i < gConst.NumCities; i++ {
			gConst.Scanner.Scan()
			splitedInput = strings.Split(gConst.Scanner.Text(), " ")
			x, _ := strconv.Atoi(splitedInput[0])
			y, _ := strconv.Atoi(splitedInput[1])
			gConst.trueData.poss[i][0] = x
			gConst.trueData.poss[i][1] = y
		}
		for i := 0; i < gConst.NumCities; i++ {
			for j := i + 1; j < gConst.NumCities; j++ {
				dx := gConst.trueData.poss[j][0] - gConst.trueData.poss[i][0]
				dy := gConst.trueData.poss[j][1] - gConst.trueData.poss[i][1]
				distance := int(math.Sqrt(float64(dx*dx + dy*dy)))
				gConst.trueData.distances[i][j] = distance
				gConst.trueData.distances[j][i] = distance
			}
		}
	}
}

// 現在の状態を記録するもの
type GameState struct {
	groups [][]int
}

func NewGameState(gConst *GameConst) *GameState {
	g := GameState{}
	g.groups = make([][]int, gConst.NumGroups)
	for i := range g.groups {
		g.groups[i] = make([]int, gConst.GroupSizes[i])
		for j := range g.groups[i] {
			g.groups[i][j] = -1
		}
	}
	return &g
}

// 1ケース実行中において不変なものを保存する構造体
type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	NumCities     int
	NumGroups     int
	NumQueries    int
	MaxQuerySize  int
	MaxErrorRange int
	GroupSizes    []int
	ls            [][2]int
	rs            [][2]int
	GoalSizeCount []int

	trueData *TrueData
}

func (g *GameConst) SetScanner(scanner *bufio.Scanner) {
	g.Scanner = scanner
}

func NewGameConst(runner *Runner) *GameConst {
	g := GameConst{}
	g.Random = rand.New(rand.NewSource(runner.randSeed))
	return &g
}

type TrueData struct {
	poss      [][2]int
	distances [][]int
}

func NewTrueData(gameConst *GameConst) *TrueData {
	td := TrueData{}
	td.poss = make([][2]int, gameConst.NumCities)
	td.distances = make([][]int, gameConst.NumCities)
	for i := range td.distances {
		td.distances[i] = make([]int, gameConst.NumCities)
	}
	return &td
}

func printAction(edges [][][2]int, gameState *GameState, gameConst *GameConst, comment bool) {
	fmt.Println("!")
	for i := 0; i < gameConst.NumGroups; i++ {
		indices := make([]string, 0, gameConst.GroupSizes[i])
		for _, v := range gameState.groups[i] {
			indices = append(indices, strconv.Itoa(v))
		}
		fmt.Println(strings.Join(indices, " "))
		for j := range edges[i] {
			fmt.Println(edges[i][j][0], edges[i][j][1])
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
	file := fmt.Sprintf("%04d", testCase)
	runner := NewRunner(testCase, time.Now().UnixNano())
	result := runner.doGame(file)
	if DEBUG {
		fmt.Fprintf(os.Stderr, "score:%d (case=%04d)\n", result.score, testCase)
	}
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

func (s *Solver) Solve(gameState *GameState, gameConst *GameConst) ([][][2]int, int) {
	start := time.Now()
	_ = start
	edges := make([][][2]int, gameConst.NumGroups)
	for i := range edges {
		edges[i] = make([][2]int, gameConst.GroupSizes[i])
	}
	maxScore := 0.0
	_ = maxScore

	queryHistry := make([][]int, 0, gameConst.NumQueries)
	resultHistry := make([][][2]int, 0, gameConst.NumQueries)

	samples := make([]*Sample, NUM_SAMPLE*(2+15)/(2+gameConst.MaxQuerySize))
	for i := range samples {
		samples[i] = NewSample(gameConst)
		if i == 0 {
			samples[i].setCenterPoss(gameConst)
		} else {
			samples[i].setRandomPoss(gameConst)
		}
	}
	sample := samples[0]

	adjustOrder := make([]int, gameConst.NumQueries)
	for i := range adjustOrder {
		adjustOrder[i] = i
	}

	uncertainty := make([]float64, gameConst.NumCities)

	// クエリを消費する
	startMult := float64(gameConst.MaxErrorRange) * 0.3
	endMult := float64(gameConst.MaxErrorRange) * 0.4 / float64(gameConst.MaxQuerySize-1)
	ms := NewMSTSolver(gameConst.NumCities, sample.poss)
	for queryCount := 0; queryCount < gameConst.NumQueries; queryCount++ {
		meanPos := make([][2]int, gameConst.NumCities)
		for i := range uncertainty {
			uncertainty[i] = 0
			for j := 0; j < 2; j++ {
				meanP := 0.0
				for k := range samples {
					p := samples[k].poss[i][j]
					meanP += float64(p)
				}
				meanP /= float64(len(samples))
				uncertainty[i] += meanP
				meanPos[i][j] = int(meanP)
			}
		}
		bestQuery := []int{}
		bestValue := -1.0
		bestCounts := [][]int{}
		indice := make([]int, gameConst.NumCities)
		for try := 0; try < 20; try++ {
			index := gameConst.Random.Intn(gameConst.NumCities)
			r2 := gameConst.Random.Intn(len(samples))
			query := NearestCandidates(samples[r2].poss, index, gameConst.MaxQuerySize, gameConst)
			sort.Slice(query, func(i, j int) bool { return query[i] < query[j] })
			value := 0.0
			counts := make([][]int, gameConst.MaxQuerySize)
			for i := range counts {
				counts[i] = make([]int, gameConst.MaxQuerySize)
			}
			for i := range query {
				indice[query[i]] = i
			}

			for i := 0; i < len(samples); i++ {
				ms.updateCoords(samples[i].poss)
				_, estimatedResult := ms.SolveQuery(query, [][2]int{})
				for j := range estimatedResult {
					u := estimatedResult[j][0]
					v := estimatedResult[j][1]
					counts[indice[u]][indice[v]]++
					counts[indice[v]][indice[u]]++
				}
			}
			// fmt.Fprintln(os.Stderr, v, math.Sqrt(float64(v)))
			for i := range counts {
				for j := range counts[i] {
					value += float64(counts[i][j] * (len(samples) - counts[i][j]))
				}
			}
			if bestValue < value {
				bestValue = value
				bestQuery = query
				bestCounts = counts
			}
		}
		// もっともよいものを改善していく
		// fmt.Fprintln(os.Stderr, worstIndex, worstValue)
		// bestValue1 := bestValue
		for try := 0; try < 20; try++ {
			for i := range indice {
				indice[i] = -1
			}
			worstIndex := -1
			worstValue := float64(INF)
			for i := range bestCounts {
				c := 0.0
				for j := range bestCounts[i] {
					c += float64(bestCounts[i][j] * (len(samples) - bestCounts[i][j]))
				}
				c += gameConst.Random.Float64() * 0.5

				if c < worstValue {
					worstValue = c
					worstIndex = i
				}
			}
			query := make([]int, 0, len(bestQuery))
			query = append(query, bestQuery...)
			for i := range query {
				indice[query[i]] = i
			}
			for {
				newVertex := gameConst.Random.Intn(gameConst.NumCities)
				if indice[newVertex] != -1 {
					continue
				}
				dx := sample.poss[newVertex][0] - sample.poss[query[worstIndex]][0]
				dy := sample.poss[newVertex][1] - sample.poss[query[worstIndex]][1]
				sqD := dx*dx + dy*dy
				if sqD < 2000*2000 {
					query[worstIndex] = newVertex
					indice[newVertex] = worstIndex
					break
				}
			}
			sort.Slice(query, func(i, j int) bool { return query[i] < query[j] })
			value := 0.0
			counts := make([][]int, gameConst.MaxQuerySize)
			for i := range counts {
				counts[i] = make([]int, gameConst.MaxQuerySize)
			}
			for i := range query {
				indice[query[i]] = i
			}

			for i := 0; i < len(samples); i++ {
				ms.updateCoords(samples[i].poss)
				_, estimatedResult := ms.SolveQuery(query, [][2]int{})
				for j := range estimatedResult {
					u := estimatedResult[j][0]
					v := estimatedResult[j][1]
					counts[indice[u]][indice[v]]++
					counts[indice[v]][indice[u]]++
				}
			}
			// fmt.Fprintln(os.Stderr, v, math.Sqrt(float64(v)))
			for i := range counts {
				for j := range counts[i] {
					value += float64(counts[i][j] * (len(samples) - counts[i][j]))
				}
			}
			if bestValue < value {
				bestValue = value
				bestQuery = query
				bestCounts = counts
			}
		}
		// fmt.Fprintln(os.Stderr, bestValue1, "->", bestValue)

		/*ignore := false
		for i := range queryHistry {
			count := 0
			j1 := 0
			j2 := 0
			for {
				if queryHistry[i][j1] == bestQuery[j2] {
					count++
				}
				if queryHistry[i][j1] < bestQuery[j2] {
					j1++
				} else {
					j2++
				}
				if j1 == gameConst.MaxQuerySize || j2 == gameConst.MaxQuerySize {
					break
				}
			}
			if count >= gameConst.MaxQuerySize {
				// fmt.Fprintln(os.Stderr, "almast same", count, "/", gameConst.MaxQuerySize, queryCount, i)
				ignore = true
				break
			}
		}
		if ignore {
			queryCount--
			continue
		}*/
		result := throwQuery(bestQuery, gameConst)

		queryHistry = append(queryHistry, bestQuery)
		resultHistry = append(resultHistry, result)

		mult := math.Pow(startMult, 1-float64(queryCount)/float64(gameConst.NumQueries)) * math.Pow(endMult, float64(queryCount)/float64(gameConst.NumQueries))
		if queryCount%10 == 9 {
			for j := range samples {
				gameConst.Random.Shuffle(gameConst.NumQueries, func(i, j int) { adjustOrder[i], adjustOrder[j] = adjustOrder[j], adjustOrder[i] })
				for i := 0; i < gameConst.NumQueries; i++ {
					if adjustOrder[i] > queryCount {
						continue
					}
					samples[j].Adjust(ms, queryCount, queryHistry[adjustOrder[i]], resultHistry[adjustOrder[i]], mult, uncertainty, gameConst, adjustOrder[i] == queryCount)
				}
			}
		} else {
			for j := range samples {
				samples[j].Adjust(ms, queryCount, queryHistry[queryCount], resultHistry[queryCount], mult, uncertainty, gameConst, true)
			}

		}
		if LOCAL_TEST && DEBUG {

			sumDiff := 0
			for i := range sample.poss {
				for j := 0; j < 2; j++ {
					sumDiff += absInt(sample.poss[i][j] - gameConst.trueData.poss[i][j])
				}
			}
			// fmt.Fprintln(os.Stderr, sumDiff)
		}
	}

	// sampleかさ増し
	initialNumSamples := len(samples)
	for i := 0; i < initialNumSamples*2; i++ {
		s := NewSample(gameConst)
		s.setRandomPoss(gameConst)
		for l := 0; l < 10; l++ {
			mult := math.Pow(startMult, 1-float64(l)/float64(10)) * math.Pow(endMult, float64(l)/float64(10))
			gameConst.Random.Shuffle(gameConst.NumQueries, func(i, j int) { adjustOrder[i], adjustOrder[j] = adjustOrder[j], adjustOrder[i] })
			for j := 0; j < gameConst.NumQueries; j++ {
				s.Adjust(ms, l, queryHistry[adjustOrder[j]], resultHistry[adjustOrder[j]], mult, uncertainty, gameConst, true)
			}
		}
		samples = append(samples, s)
	}

	sample = samples[0]

	groupSizeOrders := make([]int, gameConst.NumGroups)
	for i := range groupSizeOrders {
		groupSizeOrders[i] = i
	}
	sort.Slice(groupSizeOrders, func(i, j int) bool {
		return gameConst.GroupSizes[groupSizeOrders[i]] > gameConst.GroupSizes[groupSizeOrders[j]]
	})
	sortedGroupsizes := make([]int, gameConst.NumGroups)
	groupsizeRanks := make([]int, gameConst.NumGroups)
	for i := range groupSizeOrders {
		groupsizeRanks[groupSizeOrders[i]] = i
		sortedGroupsizes[i] = gameConst.GroupSizes[groupSizeOrders[i]]
	}
	biasedDists := make([][]int, gameConst.NumCities)
	for i := range biasedDists {
		biasedDists[i] = make([]int, gameConst.NumCities)
	}
	for i := range biasedDists {
		for j := range biasedDists[i] {
			biasedDists[i][j] = 0
			for sampleIdx := range samples {
				dx := samples[sampleIdx].poss[i][0] - samples[sampleIdx].poss[j][0]
				dy := samples[sampleIdx].poss[i][1] - samples[sampleIdx].poss[j][1]
				biasedDists[i][j] += int(math.Sqrt(float64(dx*dx + dy*dy)))
			}
			biasedDists[i][j] /= len(samples)

		}
	}
	ms = NewMSTSolverFromDist(gameConst.NumCities, biasedDists)
	globalVertice := make([]int, gameConst.NumCities)
	for i := range globalVertice {
		globalVertice[i] = i
	}
	adjacentList := makeAdjacentList(gameConst, biasedDists)

	var initialplan [][]int
	if gameConst.NumGroups < 80 {
		numTry := 5
		bestValue := INF
		forbiddenList := make([][2]int, 0)
		for i := 0; i < numTry; i++ {
			cost, globalMST := ms.SolveQuery(globalVertice, forbiddenList)
			initialplan2, value, ok := PartitionGraph3(gameConst, gameConst.NumCities, gameConst.GroupSizes, adjacentList, biasedDists, globalMST, false)
			if ok {
				// fmt.Fprintln(os.Stderr, bestValue, cost+value)
				if cost+value < bestValue {
					initialplan = initialplan2
					bestValue = cost + value
				}
			}
			forbiddenList = forbiddenList[:0]
			for i := 0; i < 5; i++ {
				r := gameConst.Random.Intn(len(globalMST))
				forbiddenList = append(forbiddenList, globalMST[r])
			}
		}
	} else {
		numTry := 2
		bestValue := INF
		for i := 0; i < numTry; i++ {
			initialplan2, value, ok := PartitionGraph(biasedDists, gameConst, gameConst.NumGroups, gameConst.GroupSizes, i > 0)
			if ok {
				if value < bestValue {
					initialplan = initialplan2
					bestValue = value
				}
			}
		}
		/*
			fmt.Fprintln(os.Stderr, bestValue)
			allVertice := make([]int, gameConst.NumCities)
			for i := range allVertice {
				allVertice[i] = i
			}
			_, mst := ms.SolveQuery(allVertice, [][2]int{})
			sort.Slice(mst, func(i, j int) bool { return biasedDists[mst[i][0]][mst[i][1]] < biasedDists[mst[j][0]][mst[j][1]] })
			lower_bound := 0
			mst = mst[:gameConst.NumCities-gameConst.NumGroups]
			for i := range mst {
				u := mst[i][0]
				v := mst[i][1]
				lower_bound += biasedDists[u][v]
			}
			fmt.Fprintln(os.Stderr, bestValue, lower_bound)
		*/
	}

	initialEdges := make([][][2]int, len(initialplan))
	for i := range initialEdges {
		ms := NewMSTSolverFromDist(gameConst.NumCities, biasedDists)
		_, initialEdges[i] = ms.SolveQuery(initialplan[i], [][2]int{})
	}

	plan := initialplan

	sort.Slice(plan, func(i, j int) bool { return len(plan[i]) > len(plan[j]) })
	for i := range plan {
		gameState.groups[groupSizeOrders[i]] = plan[i]
	}

	for i := range plan {
		ms := NewMSTSolverFromDist(gameConst.NumCities, biasedDists)
		_, mst := ms.SolveQuery(gameState.groups[i], [][2]int{})
		edges[i] = mst
	}

	score := 0

	return edges, score
}

// NearestCandidates は、頂点リスト poss のうち、
// 指定した index の頂点からのユークリッド距離が最も近い頂点の index を
// numSelect 個返します。
func NearestCandidates(poss [][2]int, index int, numSelect int, gameConst *GameConst) []int {
	// indexのチェック
	if index < 0 || index >= len(poss) || numSelect <= 0 {
		return nil
	}

	base := poss[index]

	// 距離（の平方）とindexを保持する構造体
	sortedQueue := NewSortedQueue(numSelect)
	sortedQueue.Init()

	// 指定頂点以外の全頂点に対して距離を計算
	for i, pos := range poss {
		if i == index {
			continue
		}
		dx := pos[0] - base[0]
		dy := pos[1] - base[1]
		d := int(math.Sqrt(float64(dx*dx + dy*dy)))
		if d < 2500 {
			sortedQueue.Push([2]int{i, d})
		}
	}

	// 結果のsliceに候補のindexを抽出
	sortedQueue.Sort()
	res := make([]int, 0, numSelect)
	for sortedQueue.Len() > 0 {
		item := sortedQueue.Pop()
		res = append(res, item)
	}

	return res
}

func (runner *Runner) doGame(file string) Result {
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
	edges, score := solver.Solve(gameState, gameConst)

	// アクションを出力
	if !LOCAL_TEST || (!SUCCESSIVE && PRINT_ACTION) {
		printAction(edges, gameState, gameConst, false)
	}

	if LOCAL_TEST {
		score = calcScore(edges, gameState, gameConst)
	}

	result.score = score

	return result
}

func calcScore(edges [][][2]int, gameState *GameState, gameConst *GameConst) int {
	sumCost := 0
	for i := range gameState.groups {
		if len(gameState.groups[i]) != gameConst.GroupSizes[i] {
			return -1
		}
		if len(edges[i]) != gameConst.GroupSizes[i]-1 {
			return -1
		}
		for j := range edges[i] {
			u := edges[i][j][0]
			v := edges[i][j][1]
			dx := gameConst.trueData.poss[u][0] - gameConst.trueData.poss[v][0]
			dy := gameConst.trueData.poss[u][1] - gameConst.trueData.poss[v][1]
			cost := int(math.Sqrt(float64(dx*dx + dy*dy)))
			sumCost += cost
		}
	}
	score := sumCost
	return score
}

func throwQuery(vertices []int, gameConst *GameConst) [][2]int {
	fmt.Printf("?")
	fmt.Printf(" %d", len(vertices))
	for i := range vertices {
		fmt.Printf(" %d", vertices[i])
	}
	fmt.Printf("\n")
	if !LOCAL_TEST {
		result := make([][2]int, 0, len(vertices)-1)
		for i := 0; i < len(vertices)-1; i++ {
			gameConst.Scanner.Scan()
			splitedInput := strings.Split(gameConst.Scanner.Text(), " ")
			u, _ := strconv.Atoi(splitedInput[0])
			v, _ := strconv.Atoi(splitedInput[1])
			result = append(result, [2]int{u, v})
		}
		return result
	} else {
		ms := NewMSTSolver(gameConst.NumCities, gameConst.trueData.poss)
		_, mstEdges := ms.SolveQuery(vertices, [][2]int{})
		gameConst.Random.Shuffle(len(mstEdges), func(i, j int) { mstEdges[i], mstEdges[j] = mstEdges[j], mstEdges[i] })
		return mstEdges
	}
}

type Sample struct {
	poss        [][2]int
	lastUpdated []int
}

func NewSample(gameConst *GameConst) *Sample {
	sample := Sample{}
	sample.poss = make([][2]int, gameConst.NumCities)
	sample.lastUpdated = make([]int, gameConst.NumCities)
	for i := range sample.lastUpdated {
		sample.lastUpdated[i] = -1
	}
	return &sample
}

func (sample *Sample) setCenterPoss(gameConst *GameConst) {
	for i := range sample.poss {
		for j := 0; j < 2; j++ {
			v := (gameConst.rs[i][j] + gameConst.ls[i][j]) / 2
			sample.poss[i][j] = v
		}
	}
}

func (sample *Sample) setRandomPoss(gameConst *GameConst) {
	for i := range sample.poss {
		for j := 0; j < 2; j++ {
			minV := gameConst.ls[i][j]
			randWidth := gameConst.rs[i][j] - gameConst.ls[i][j]
			v := minV + gameConst.Random.Intn(randWidth+1)
			sample.poss[i][j] = v
		}
	}
}

// クエリ結果を用いて修正する関数
func (sample *Sample) Adjust(ms *MSTSolver, queryCount int, query []int, result [][2]int, mult float64, uncertainty []float64, gameConst *GameConst, force bool) {
	if force {
		maxLastUpdated := -1
		for i := range query {
			if maxLastUpdated < sample.lastUpdated[query[i]] {
				maxLastUpdated = sample.lastUpdated[query[i]]
			}
		}
		if maxLastUpdated < queryCount-2000 {
			return
		}
	}
	ms.updateCoords(sample.poss)
	_, estimatedResult := ms.SolveQuery(query, [][2]int{})
	result1, result2 := diffEdges(result, estimatedResult)
	adjustVertices(sample, queryCount, uncertainty, result1, 0, mult, gameConst)
	adjustVertices(sample, queryCount, uncertainty, result2, 1, mult, gameConst)
}

// adjustVertices は、頂点の座標(pos)と不確実性(uncertainty)に基づいて、
// violatedEdges に格納された各エッジ(u,v)の2頂点の距離を mult の値だけ「縮める」(actionType=0)または「伸ばす」(actionType=1)関数です。
// なお、変更量は各頂点の uncertainty の比に従って分配されます。
func adjustVertices(sample *Sample, queryCount int, uncertainty []float64, violatedEdges [][2]int, actionType int, mult float64, gameConst *GameConst) {
	pos := sample.poss
	for _, edge := range violatedEdges {
		u, v := edge[0], edge[1]

		// 2頂点間の差分ベクトル（浮動小数点に変換）
		dx := float64(pos[v][0] - pos[u][0])
		dy := float64(pos[v][1] - pos[u][1])

		// 現在の距離
		d := math.Sqrt(dx*dx + dy*dy)
		// 0距離の場合は動かさない
		if d == 0 {
			continue
		}

		// 不確実性の比に従って変化量を計算
		sumUnc := uncertainty[u] + uncertainty[v]
		// 両頂点とも不確実性がゼロの場合は等分割とする
		if sumUnc == 0 {
			sumUnc = 2
		}
		// 各頂点が移動する距離
		moveU := (uncertainty[u] / sumUnc) * mult
		moveV := (uncertainty[v] / sumUnc) * mult

		// 単位ベクトル (u→v)
		nx := dx / d
		ny := dy / d

		if actionType == 0 {
			// 「縮める」場合:
			// uは v 方向に、v は u 方向（逆向き）に移動
			pos[u][0] += int(math.Round(nx * moveU))
			pos[u][1] += int(math.Round(ny * moveU))
			pos[v][0] -= int(math.Round(nx * moveV))
			pos[v][1] -= int(math.Round(ny * moveV))
		} else if actionType == 1 {
			// 「伸ばす」場合:
			// uは v 方向と反対に、v はそのままの方向に移動
			pos[u][0] -= int(math.Round(nx * moveU))
			pos[u][1] -= int(math.Round(ny * moveU))
			pos[v][0] += int(math.Round(nx * moveV))
			pos[v][1] += int(math.Round(ny * moveV))
		}
		sample.lastUpdated[u] = queryCount
		sample.lastUpdated[v] = queryCount
		for j := 0; j < 2; j++ {
			if pos[u][j] < gameConst.ls[u][j] {
				pos[u][j] = gameConst.ls[u][j]
			}
			if pos[u][j] > gameConst.rs[u][j] {
				pos[u][j] = gameConst.rs[u][j]
			}
			if pos[v][j] < gameConst.ls[v][j] {
				pos[v][j] = gameConst.ls[v][j]
			}
			if pos[v][j] > gameConst.rs[v][j] {
				pos[v][j] = gameConst.rs[v][j]
			}
		}
	}
}

func diffEdges(edges1, edges2 [][2]int) (result1, result2 [][2]int) {
	// array1の各要素について、array2に存在するかを線形探索でチェック
	for i, p1 := range edges1 {
		found := false
		for _, p2 := range edges2 {
			if p1 == p2 {
				found = true
				break
			}
		}
		if !found {
			result1 = append(result1, edges1[i])
		}
	}

	// array2の各要素について、array1に存在するかを線形探索でチェック
	for i, p2 := range edges2 {
		found := false
		for _, p1 := range edges1 {
			if p2 == p1 {
				found = true
				break
			}
		}
		if !found {
			result2 = append(result2, edges2[i])
		}
	}

	return
}

// 以下、MST処理

// MSTSolver は頂点数と各頂点の座標を保持します。
type MSTSolver struct {
	n         int      // 頂点数
	coords    [][2]int // 各頂点の (x, y) 座標
	dists     [][]int
	solveType int
}

// NewMSTSolver は頂点数 n と座標 coords を受け取り、MSTSolver を生成します。
func NewMSTSolver(n int, coords [][2]int) *MSTSolver {
	ms := &MSTSolver{
		n:         n,
		coords:    coords,
		solveType: 0,
	}
	return ms

}

func (ms *MSTSolver) updateCoords(coords [][2]int) {
	ms.coords = coords
}
func NewMSTSolverFromDist(n int, dists [][]int) *MSTSolver {
	ms := &MSTSolver{
		n:         n,
		dists:     dists,
		solveType: 1,
	}
	return ms
}

// sqDistance は、頂点 u と頂点 v の間のユークリッド距離の二乗を計算
func (ms *MSTSolver) sqDistance(u, v int) int {
	dx := ms.coords[u][0] - ms.coords[v][0]
	dy := ms.coords[u][1] - ms.coords[v][1]
	// ユークリッド距離の二乗の計算
	return (dx*dx + dy*dy)
}

// SolveQuery は、queryVertices で与えられる頂点集合に対して、
// Prim のアルゴリズムにより部分グラフの MST を構築し、
// MST の総コスト（切り捨てたユークリッド距離の和）と、
// MST に含まれる辺（[2]int。必ず第一要素 < 第二要素）を返します。
// 連結でない場合は cost = -1 と空のエッジスライスを返します。
func (ms *MSTSolver) SolveQuery(queryVertices []int, forbiddenList [][2]int) (int, [][2]int) {
	k := len(queryVertices)
	if k == 0 {
		return 0, nil
	}

	minDs := make([]int, k)
	parents := make([]int, k)
	checked := make([]bool, k)

	// 初期化：すべて INF、親は -1
	for i := 0; i < k; i++ {
		minDs[i] = INF
		parents[i] = -1
		checked[i] = false
	}
	// 任意の開始頂点：queryVertices[0] を採用
	minDs[0] = 0

	totalCost := 0
	mstEdges := make([][2]int, 0, k-1)

	processed := 0
	minIndex := 0
	minD := 0
	for processed < k {
		uIdx := minIndex
		u := queryVertices[uIdx] // 実際の頂点番号
		if minIndex == -1 {
			// 連結でない場合
			return -1, nil
		}
		totalCost += int(math.Sqrt(float64(minD)))
		processed++
		// u が開始頂点でなければ、(parent, u) の辺を採用
		if parents[uIdx] != -1 {
			pu := parents[uIdx] // 実際の頂点番号
			// 両端を昇順に整形
			v1, v2 := pu, u
			if v1 > v2 {
				v1, v2 = v2, v1
			}
			mstEdges = append(mstEdges, [2]int{v1, v2})
		}
		checked[uIdx] = true
		minIndex = -1
		minD = INF
		// u を固定したので、残りの各頂点 v ∈ queryVertices について key を更新
		for j := 0; j < k; j++ {
			if !checked[j] {
				update := true
				v := queryVertices[j]
				if len(forbiddenList) != 0 {
					ok := true
					for k := range forbiddenList {
						u1 := forbiddenList[k][0]
						v1 := forbiddenList[k][1]
						if u == u1 && v == v1 {
							ok = false
							break
						}
						if u == v1 && v == u1 {
							ok = false
							break
						}
					}
					if !ok {
						update = false
					}
				}
				if update {
					d := 0
					if ms.solveType == 0 {
						d = ms.sqDistance(u, v)
					} else {
						d = ms.dists[u][v]
					}
					if d < minDs[j] {
						minDs[j] = d
						parents[j] = u
					}
				}
				if minDs[j] < minD {
					minD = minDs[j]
					minIndex = j
				}
			}
		}
	}

	if processed != k {
		return -1, nil
	}
	return totalCost, mstEdges
}

// MST処理ここまで

// splitTree は、与えられた treeEdges から DFS で各辺の切断時の連結成分サイズを求め、
// 2種類のシナリオ（ケース2, ケース3）により tree を2つに分割し、
// さらに候補リストから最も軽い接続辺を加えることで目的の分割を作る関数です。
// 戻り値は、各分割先の頂点リスト groups と、その tree 内の辺 edges です。
func splitTree(numVertice int, remainingVertice []int, treeEdges [][2]int, candidateList [][]int, dist [][]int, groupSize int) (bool, int, [2][]int, [2][][2]int) {
	// 隣接リストを構築（無向グラフ）
	adj := make([][]int, numVertice)
	root := -1
	for i, edge := range treeEdges {
		u, v := edge[0], edge[1]
		adj[u] = append(adj[u], v)
		adj[v] = append(adj[v], u)
		if i == 0 {
			root = edge[0]
		}
	}

	// DFS により、親・子関係、subtree サイズ、Euler順序(tin,tout) を求める
	parent := make([]int, numVertice)
	for i := range parent {
		parent[i] = -1
	}
	subSize := make([]int, numVertice)
	tin := make([]int, numVertice)
	for i := range tin {
		tin[i] = -1
	}
	tout := make([]int, numVertice)
	order := make([]int, 0, numVertice)
	time := 0

	candidates := make([][]int, numVertice+1)
	for i := range candidates {
		candidates[i] = make([]int, 0)
	}

	// 部分木情報を保存する構造体
	type subtreeInfo struct {
		v      int // v:頂点
		size   int // 連結成分サイズ（subtreeサイズ）
		weight int // 辺の重み（dist[v][p]）
	}
	var subtreesInfo []subtreeInfo

	var dfs func(u, p int)
	dfs = func(u, p int) {
		parent[u] = p
		tin[u] = time
		time++
		order = append(order, u)
		subSize[u] = 1
		for _, w := range adj[u] {
			if w == p {
				continue
			}
			dfs(w, u)
			subSize[u] += subSize[w]
		}
		sI := subtreeInfo{
			v:    u,
			size: subSize[u],
		}
		if p != -1 {
			sI.weight = dist[u][p]
		}
		subtreesInfo = append(subtreesInfo, sI)
		candidates[subSize[u]] = append(candidates[subSize[u]], len(subtreesInfo)-1)
		tout[u] = time
	}
	dfs(root, -1)

	// 補助関数：uが v のsubtree内にあるか（Euler順序を利用）
	isInSubtree := func(u, root int) bool {
		return tin[root] <= tin[u] && tin[u] < tout[root]
	}

	// 補助関数：componentA の頂点から、candidateList を利用して componentB との間で最小の接続辺を探す
	// 戻り値は (u,v,weight,found)
	findMinCandidateEdge := func(scenario int, r1, r2 subtreeInfo) (int, int, int, bool) {
		bestW := math.MaxInt64
		bestU, bestV := -1, -1
		found := false
		if scenario == 2 {
			for orderIndex := tin[r2.v]; orderIndex < tout[r2.v]; orderIndex++ {
				u := order[orderIndex]
				for _, cand := range candidateList[u] {
					if cand < 0 || cand >= numVertice {
						continue
					}
					orderIndex2 := tin[cand]
					if orderIndex2 == -1 {
						continue
					}
					if orderIndex2 < tin[r1.v] || orderIndex2 >= tout[r1.v] {
						w := dist[u][cand]
						if w < bestW {
							bestW = w
							bestU = u
							bestV = cand
							found = true
						}
						// 候補リストは近い順と仮定して break してもよい
						break
					}
				}
			}
		}
		if scenario == 3 {
			for orderIndex := tin[r2.v]; orderIndex < tout[r2.v]; orderIndex++ {
				u := order[orderIndex]
				for _, cand := range candidateList[u] {
					if cand < 0 || cand >= numVertice {
						continue
					}
					orderIndex2 := tin[cand]
					if orderIndex2 == -1 {
						continue
					}
					if orderIndex2 >= tin[r1.v] && orderIndex2 < tout[r1.v] {
						w := dist[u][cand]
						if w < bestW {
							bestW = w
							bestU = u
							bestV = cand
							found = true
						}
						// 候補リストは近い順と仮定して break してもよい
						break
					}
				}
			}
		}
		return bestU, bestV, bestW, found
	}

	// シナリオ候補の結果を保持する構造体
	type candidateSolution struct {
		scenario int // 2 または 3
		R1       subtreeInfo
		R2       subtreeInfo
		candU    int
		candV    int
		candW    int
		cost     int // (candW - (E1.weight + E2.weight))
	}
	bestSol := candidateSolution{cost: math.MaxInt64}
	foundSolution := false
	// シナリオ1：ちょうどk個に分ける方法が存在する場合
	for k := groupSize; k == groupSize; k++ {
		for _, r2Index := range candidates[k] {
			r2 := subtreesInfo[r2Index]
			cost := -(r2.weight)

			// fmt.Fprintln(os.Stderr, r2.size, r1.size, cost)
			if cost < bestSol.cost {
				bestSol = candidateSolution{
					scenario: 1,
					R1:       subtreesInfo[candidates[len(remainingVertice)][0]],
					R2:       r2,
					candU:    -1,
					candV:    -1,
					candW:    0,
					cost:     cost,
				}
				foundSolution = true
			}
		}
	}
	if bestSol.cost >= 0 {
		// ※ シナリオ2：E1 の子側の連結成分が groupsize+k となり、その中にある E2 で subtree size = k となるもの
		for k := 0; k < len(remainingVertice)-groupSize; k++ {
			for _, r2Index := range candidates[k] {
				r2 := subtreesInfo[r2Index]
				// e2 は、子側の連結成分として k を持つので、その親候補 e1 を探す
				for _, r1Index := range candidates[groupSize+k] {
					r1 := subtreesInfo[r1Index]

					// E2 が e1 の子側の領域内にある必要がある（e1 の子 v のsubtree内に e2.v がある）
					if !isInSubtree(r2.v, r1.v) {
						continue
					}

					// シナリオ2の場合、分割後は以下の3成分となる：
					// A: E2 の子のsubtree (サイズ k)
					// B: e1 の子のsubtree から A を除いた部分 (サイズ groupsize)
					// C: e1 の子のsubtree の補集合 (サイズ numVertice - groupsize - k)
					// fmt.Fprintln(os.Stderr, r2.size, r1.size)
					uCand, vCand, candW, ok := findMinCandidateEdge(2, r1, r2)
					if !ok {
						continue
					}
					cost := candW - (r1.weight + r2.weight)

					// fmt.Fprintln(os.Stderr, r2.size, r1.size, cost)
					if cost < bestSol.cost {
						bestSol = candidateSolution{
							scenario: 2,
							R1:       r1,
							R2:       r2,
							candU:    uCand,
							candV:    vCand,
							candW:    candW,
							cost:     cost,
						}
						foundSolution = true
					}
				}
			}
		}

		// シナリオ3：E1 の子側の連結成分が groupsize-k となり、E2（subtree size k）が e1 の子側の外部にあるもの
		for k := 0; k < groupSize; k++ {
			for _, r2Index := range candidates[k] {
				r2 := subtreesInfo[r2Index]
				// r2 は、子側の連結成分として k を持つので、その親候補 e1 を探す
				for _, r1Index := range candidates[groupSize-k] {
					r1 := subtreesInfo[r1Index]
					// 今回は r2 が r1 の子側の領域外にある必要がある
					if isInSubtree(r2.v, r1.v) {
						continue
					}
					if isInSubtree(r1.v, r2.v) {
						continue
					}
					// シナリオ3の場合、分割後は以下の3成分となる：
					// A: r2 の子のsubtree (サイズ k)
					// B: r1 の子のsubtree (サイズ groupsize - k)
					// C: 補集合 (サイズ numVertice - groupSize)
					uCand, vCand, candW, ok := findMinCandidateEdge(3, r1, r2)
					if !ok {
						continue
					}
					cost := candW - (r1.weight + r2.weight)
					if cost < bestSol.cost {
						bestSol = candidateSolution{
							scenario: 3,
							R1:       r1,
							R2:       r2,
							candU:    uCand,
							candV:    vCand,
							candW:    candW,
							cost:     cost,
						}
						foundSolution = true
					}
				}
			}
		}
	}

	// もし有効な解が見つからなければ、空の結果を返す
	if !foundSolution {
		return false, INF, [2][]int{}, [2][][2]int{}
	}

	// bestSol に従い、最終的な分割を構成する
	// 元の treeEdges から、選ばれた 2 辺 (E1, E2) を取り除き、さらに candidate connecting edge を加える
	// ここでは、最終グラフを再構成し、DFS により連結成分 (2つ) を求める

	// build final graph (無向グラフ)
	finalAdj := make([][]int, numVertice)
	// helper: addEdge
	addEdge := func(u, v int) {
		finalAdj[u] = append(finalAdj[u], v)
		finalAdj[v] = append(finalAdj[v], u)
	}
	// まず元の treeEdges から
	removeEdge := func(u, v int) bool {
		// 2辺 E1, E2 が (u,v) として取り除くべきかを判定（順序はどちらでもよい）
		if (u == parent[bestSol.R1.v] && v == bestSol.R1.v) || (u == bestSol.R1.v && v == parent[bestSol.R1.v]) {
			return true
		}
		if (u == parent[bestSol.R2.v] && v == bestSol.R2.v) || (u == bestSol.R2.v && v == parent[bestSol.R2.v]) {
			return true
		}
		return false
	}
	for _, edge := range treeEdges {
		u, v := edge[0], edge[1]
		if removeEdge(u, v) {
			continue
		}
		addEdge(u, v)
	}
	// candidate connecting edge を追加
	if bestSol.candU != -1 {
		addEdge(bestSol.candU, bestSol.candV)
	}

	// finalAdj により、DFS/BFS で 2 つの連結成分を求める
	visited := make([]bool, numVertice)
	var components [][]int
	var dfsComp func(u int, comp *[]int)
	dfsComp = func(u int, comp *[]int) {
		visited[u] = true
		*comp = append(*comp, u)
		for _, w := range finalAdj[u] {
			if !visited[w] {
				dfsComp(w, comp)
			}
		}
	}
	for _, v := range remainingVertice {
		if !visited[v] {
			var comp []int
			dfsComp(v, &comp)
			components = append(components, comp)
		}
	}
	// components が必ず2個であると仮定
	if len(components) != 2 {
		fmt.Fprintln(os.Stderr, "???")
		// for i := range components {
		// 	fmt.Fprintln(os.Stderr, components[i])
		// }
		// 分割に失敗した場合
		return false, INF, [2][]int{}, [2][][2]int{}
	}

	// finalAdj 上の辺から、各連結成分に含まれる辺を抽出
	// 各辺は (u,v) で u < v となるように重複なく抽出
	compEdgeMap := make([]map[[2]int]bool, 2)
	for i := 0; i < 2; i++ {
		compEdgeMap[i] = make(map[[2]int]bool)
	}
	// 各コンポーネントのメンバーを membership マップに
	compMember := make([][]bool, 2)
	for i, comp := range components {
		mem := make([]bool, numVertice)
		for _, v := range comp {
			mem[v] = true
		}
		compMember[i] = mem
	}
	// finalAdj から全辺を列挙
	// 注意：無向グラフのため、各辺を一度だけ記録
	seenEdge := make(map[[2]int]bool)
	for u := 0; u < numVertice; u++ {
		for _, v := range finalAdj[u] {
			// 順序を揃える
			a, b := u, v
			if a > b {
				a, b = b, a
			}
			edgeKey := [2]int{a, b}
			if seenEdge[edgeKey] {
				continue
			}
			seenEdge[edgeKey] = true
			// どちらのコンポーネントに属するか調べる
			for i := 0; i < 2; i++ {
				if compMember[i][a] && compMember[i][b] {
					compEdgeMap[i][edgeKey] = true
					break
				}
			}
		}
	}

	// groups および edges の作成
	var groups [2][]int
	var edgesOut [2][][2]int
	for i := 0; i < 2; i++ {
		groups[i] = components[i]
		for edgeKey := range compEdgeMap[i] {
			edgesOut[i] = append(edgesOut[i], edgeKey)
		}
	}
	if len(groups[0]) != groupSize {
		groups[0], groups[1] = groups[1], groups[0]
		edgesOut[0], edgesOut[1] = edgesOut[1], edgesOut[0]
	}
	// if len(groups[0])+len(groups[1]) != len(remainingVertice) {
	// 	fmt.Fprintln(os.Stderr, len(groups[0]), len(groups[1]), len(remainingVertice))
	// 	panic("")
	// }

	return true, bestSol.cost, groups, edgesOut
}

func makeAdjacentList(gameConst *GameConst, dist [][]int) [][]int {

	isAdjacent := make([][]bool, gameConst.NumCities)
	for i := range isAdjacent {
		isAdjacent[i] = make([]bool, gameConst.NumCities)
	}
	for from := 0; from < gameConst.NumCities; from++ {
		order := make([]int, 0, gameConst.NumCities)

		for j := 0; j < gameConst.NumCities; j++ {
			if from == j {
				continue
			}
			order = append(order, j)
		}
		sort.Slice(order, func(i, j int) bool { return dist[from][order[i]] < dist[from][order[j]] })
		for i := 0; i < 10; i++ {
			isAdjacent[from][order[i]] = true
			isAdjacent[order[i]][from] = true
		}
	}
	adjacentList := make([][]int, gameConst.NumCities)
	for from := 0; from < gameConst.NumCities; from++ {
		order := make([]int, 0, gameConst.NumCities)

		for j := 0; j < gameConst.NumCities; j++ {
			if !isAdjacent[from][j] {
				continue
			}
			order = append(order, j)
		}
		sort.Slice(order, func(i, j int) bool { return dist[from][order[i]] < dist[from][order[j]] })
		adjacentList[from] = make([]int, 0, 30)
		adjacentList[from] = append(adjacentList[from], order...)
	}
	return adjacentList
}

//

func ComputeGroups(poss [][2]int, groupSizes []int, gameConst *GameConst) [][]int {
	groupIndexes := make([]int, gameConst.NumCities)
	for i := range groupIndexes {
		groupIndexes[i] = -1
	}
	candidates := make([]int, 0, gameConst.NumCities)
	weights := make([]int, gameConst.NumCities)
	for groupIdx := range groupSizes {
		target := -1
		maxDist := -1
		// 最遠点を求める
		for i := 0; i < gameConst.NumCities; i++ {
			if groupIndexes[i] != -1 {
				continue
			}
			dist := poss[i][0]*poss[i][0] + poss[i][1]*poss[i][1]
			if dist > maxDist {
				maxDist = dist
				target = i
			}
		}
		candidates = candidates[:0]
		for i := 0; i < gameConst.NumCities; i++ {
			if groupIndexes[i] != -1 {
				continue
			}
			dx := poss[i][0] - poss[target][0]
			dy := poss[i][1] - poss[target][1]
			dist := dx*dx + dy*dy
			candidates = append(candidates, i)
			weights[i] = dist
		}
		sort.Slice(candidates, func(i, j int) bool { return weights[candidates[i]] < weights[candidates[j]] })
		for i := 0; i < groupSizes[groupIdx]; i++ {
			groupIndexes[candidates[i]] = groupIdx
		}
	}

	// グループをスライスに変換して返す
	groups := make([][]int, gameConst.NumGroups)
	for i := range groups {
		groups[i] = make([]int, 0, groupSizes[i])
	}
	for i := range groupIndexes {
		groups[groupIndexes[i]] = append(groups[groupIndexes[i]], i)
	}
	return groups
}

// ComputeGroups は、頂点座標 poss とツリー構造のエッジ edges を受け取り、
// 重み(距離)の大きい順に k (ここでは len(edges)/2 としている) 個のエッジを取り除いた後の
// 連結成分（各成分は頂点番号のスライス）を返します。
func OldComputeGroups(poss [][2]int, edges [][2]int, numGroup int) [][]int {
	k := numGroup - 1

	// エッジ情報を保持するための構造体（距離の二乗を重みとして使用）
	type edgeInfo struct {
		u, v   int
		weight int64 // 距離の二乗
	}
	nEdges := len(edges)
	nVertices := len(poss)

	// 各エッジの重み（距離の二乗）を計算し、エッジ情報スライスに格納
	edgesWithWeight := make([]edgeInfo, nEdges)
	for i, e := range edges {
		u, v := e[0], e[1]
		dx := poss[u][0] - poss[v][0]
		dy := poss[u][1] - poss[v][1]
		w := int64(dx*dx + dy*dy)
		edgesWithWeight[i] = edgeInfo{u: u, v: v, weight: w}
	}

	// エッジを重みの降順にソート（距離が大きい順）
	sort.Slice(edgesWithWeight, func(i, j int) bool {
		return edgesWithWeight[i].weight > edgesWithWeight[j].weight
	})

	// Union-Find の初期化
	parent := make([]int, nVertices)
	rank := make([]int, nVertices)
	for i := 0; i < nVertices; i++ {
		parent[i] = i
		rank[i] = 0
	}

	// find: 経路圧縮付き
	var find func(x int) int
	find = func(x int) int {
		if parent[x] != x {
			parent[x] = find(parent[x])
		}
		return parent[x]
	}

	// union: ランク付き
	union := func(x, y int) {
		rootX := find(x)
		rootY := find(y)
		if rootX == rootY {
			return
		}
		if rank[rootX] < rank[rootY] {
			parent[rootX] = rootY
		} else if rank[rootX] > rank[rootY] {
			parent[rootY] = rootX
		} else {
			parent[rootY] = rootX
			rank[rootX]++
		}
	}

	// ソート後、上位 k 個のエッジを除外し、残りのエッジで union を実行
	for i := k; i < nEdges; i++ {
		edge := edgesWithWeight[i]
		union(edge.u, edge.v)
	}

	// 各頂点の連結成分の代表元ごとにグループ化
	groupsMap := make(map[int][]int)
	for i := 0; i < nVertices; i++ {
		root := find(i)
		groupsMap[root] = append(groupsMap[root], i)
	}

	// グループをスライスに変換して返す
	groups := make([][]int, 0, len(groupsMap))
	for _, group := range groupsMap {
		groups = append(groups, group)
	}
	return groups
}

// ComputeGroups は、頂点座標 poss とツリー構造のエッジ edges を受け取り、
// 重み(距離)の大きい順に k (ここでは len(edges)/2 としている) 個のエッジを取り除いた後の
// 連結成分（各成分は頂点番号のスライス）を返します。
func OldComputeGroups2(dist [][]int, edges [][2]int, numGroup int) [][]int {
	k := numGroup - 1

	// エッジ情報を保持するための構造体（距離の二乗を重みとして使用）
	type edgeInfo struct {
		u, v   int
		weight int64 // 距離の二乗
	}
	nEdges := len(edges)
	nVertices := len(edges) + 1

	// 各エッジの重み（距離の二乗）を計算し、エッジ情報スライスに格納
	edgesWithWeight := make([]edgeInfo, nEdges)
	for i, e := range edges {
		u, v := e[0], e[1]
		w := int64(dist[u][v])
		edgesWithWeight[i] = edgeInfo{u: u, v: v, weight: w}
	}

	// エッジを重みの降順にソート（距離が大きい順）
	sort.Slice(edgesWithWeight, func(i, j int) bool {
		return edgesWithWeight[i].weight > edgesWithWeight[j].weight
	})

	// Union-Find の初期化
	parent := make([]int, nVertices)
	rank := make([]int, nVertices)
	for i := 0; i < nVertices; i++ {
		parent[i] = i
		rank[i] = 0
	}

	// find: 経路圧縮付き
	var find func(x int) int
	find = func(x int) int {
		if parent[x] != x {
			parent[x] = find(parent[x])
		}
		return parent[x]
	}

	// union: ランク付き
	union := func(x, y int) {
		rootX := find(x)
		rootY := find(y)
		if rootX == rootY {
			return
		}
		if rank[rootX] < rank[rootY] {
			parent[rootX] = rootY
		} else if rank[rootX] > rank[rootY] {
			parent[rootY] = rootX
		} else {
			parent[rootY] = rootX
			rank[rootX]++
		}
	}

	// ソート後、上位 k 個のエッジを除外し、残りのエッジで union を実行
	for i := k; i < nEdges; i++ {
		edge := edgesWithWeight[i]
		union(edge.u, edge.v)
	}

	// 各頂点の連結成分の代表元ごとにグループ化
	groupsMap := make(map[int][]int)
	for i := 0; i < nVertices; i++ {
		root := find(i)
		groupsMap[root] = append(groupsMap[root], i)
	}

	// グループをスライスに変換して返す
	groups := make([][]int, 0, len(groupsMap))
	for _, group := range groupsMap {
		groups = append(groups, group)
	}
	return groups
}

//

type DataForCalc struct {
	data     []int
	uniqueId int
}

func NewDataForCalc(length int) *DataForCalc {
	d := DataForCalc{}
	d.data = make([]int, length)
	d.uniqueId = 1
	return &d
}

func (d *DataForCalc) Init() {
	d.uniqueId++
}

func (d *DataForCalc) SetTrue(index int) {
	d.data[index] = d.uniqueId
}

func (d *DataForCalc) IsTrue(index int) bool {
	return d.data[index] == d.uniqueId
}

// MSTから順にカットする
type TreeCutter struct {
	dist         [][]int
	adjacentList [][]int

	isAssigned []bool

	mainTree *Tree
}

type Tree struct {
	root     int
	indice   []int // 0~799がvertexのどのindexにあるか
	vertex   []int
	edgeList [][]int
}

func NewTree() *Tree {
	tree := Tree{}
	tree.indice = make([]int, 800)
	// tree.vertex=make([]int, )
	tree.root = 0
	return &tree
}

// PartitionGraph は、完全グラフの頂点間距離 dist,
// 分割するグループ数 numGroup,
// 各グループのサイズ groupSizes
// を引数にとり、指定アルゴリズムにより頂点を分割して groups を返します。
// ※最終的なグループのサイズ（multiset）が groupSizes と一致しなければ nil を返します。
func PartitionGraph(dist [][]int, gameConst *GameConst, numGroup int, groupSizes []int, randomize bool) ([][]int, int, bool) {
	// 頂点数 n と、グループサイズの総和が n になることを確認
	n := len(dist)
	total := 0
	maxTarget := 0
	for _, gs := range groupSizes {
		total += gs
		if gs > maxTarget {
			maxTarget = gs
		}
	}
	if total != n {
		return nil, -INF, false
	}

	// 各閾値 t (2～maxTarget) について、最終的に許される「サイズが t 以上のグループ数」
	allowedCount := make([]int, maxTarget+1)
	for t := 2; t <= maxTarget; t++ {
		count := 0
		for _, gs := range groupSizes {
			if gs >= t {
				count++
			}
		}
		allowedCount[t] = count
	}

	// 現在の連結成分のうち、サイズが t 以上であるものの個数
	globalCount := make([]int, maxTarget+1)
	for t := 2; t <= maxTarget; t++ {
		globalCount[t] = 0
	}

	// ローカルな Union-Find の実装
	type ufStruct struct {
		parent []int
		size   []int
	}
	uf := &ufStruct{
		parent: make([]int, n),
		size:   make([]int, n),
	}
	for i := 0; i < n; i++ {
		uf.parent[i] = i
		uf.size[i] = 1
	}
	var find func(x int) int
	find = func(x int) int {
		if uf.parent[x] != x {
			uf.parent[x] = find(uf.parent[x])
		}
		return uf.parent[x]
	}
	union := func(x, y int) (bool, int) {
		rx := find(x)
		ry := find(y)
		if rx == ry {
			return false, 0
		}
		if uf.size[rx] < uf.size[ry] {
			rx, ry = ry, rx
		}
		uf.parent[ry] = rx
		newSize := uf.size[rx] + uf.size[ry]
		uf.size[rx] = newSize
		return true, newSize
	}

	// 完全グラフの全辺を (weight, u, v) として作成
	type Edge struct {
		w  int
		rw int
		u  int
		v  int
	}
	numEdges := n * (n - 1) / 2
	edges := make([]Edge, 0, numEdges)
	for i := 0; i < n; i++ {
		for j := i + 1; j < n; j++ {
			edge := Edge{w: dist[i][j] * (1), u: i, v: j}
			if randomize {
				edge.rw = int(float64(edge.w) * (1 + gameConst.Random.Float64()*0.1))
			} else {
				edge.rw = edge.w
			}
			edges = append(edges, edge)
		}
	}
	// 辺を重みの昇順にソート
	sort.Slice(edges, func(i, j int) bool {
		return edges[i].rw < edges[j].rw
	})

	currentComponents := n
	// 全辺を走査
	edgeIndex := 0
	potentialEdges := make([]int, 0)
	potentialEdgeIndex := 0
	nPotentialEdges := make([]int, 0)
	totalCost := 0
	for {
		if edgeIndex >= len(edges) {
			break
		}
		if currentComponents == numGroup {
			break
		}
		var edge Edge
		index := 0
		if len(potentialEdges) == potentialEdgeIndex {
			edge = edges[edgeIndex]
			index = edgeIndex
			edgeIndex++
		} else {
			edge = edges[potentialEdges[potentialEdgeIndex]]
			index = potentialEdges[potentialEdgeIndex]
			potentialEdgeIndex++
		}
		ru := find(edge.u)
		rv := find(edge.v)
		if ru == rv {
			continue
		}
		sizeU := uf.size[ru]
		sizeV := uf.size[rv]
		newSize := sizeU + sizeV
		if newSize > maxTarget {
			continue
		}

		// 併合後、全ての閾値 t (2～maxTarget) について新たな連結成分数が許容内かチェック
		canMerge := true
		for t := 2; t <= maxTarget; t++ {
			// それぞれの連結成分が t 以上なら寄与 1、それ以外は 0
			contribU := 0
			if sizeU >= t {
				contribU = 1
			}
			contribV := 0
			if sizeV >= t {
				contribV = 1
			}
			contribNew := 0
			if newSize >= t {
				contribNew = 1
			}
			newVal := globalCount[t] - contribU - contribV + contribNew
			if newVal > allowedCount[t] {
				canMerge = false
				break
			}
		}
		if !canMerge {
			nPotentialEdges = append(nPotentialEdges, index)
			continue
		}
		_, mergedSize := union(edge.u, edge.v)

		// globalCount の更新
		for t := 2; t <= maxTarget; t++ {
			if sizeU >= t {
				globalCount[t]--
			}
			if sizeV >= t {
				globalCount[t]--
			}
			if mergedSize >= t {
				globalCount[t]++
			}
		}
		totalCost += edge.w
		if sizeU > 1 && sizeV > 1 {
			nPotentialEdges = append(nPotentialEdges, potentialEdges[potentialEdgeIndex:]...)
			potentialEdges = nPotentialEdges
			potentialEdgeIndex = 0
			nPotentialEdges = make([]int, 0, len(potentialEdges))
		}
		currentComponents--
	}

	// 最終的な連結成分を各グループとして取得
	groupsMap := make(map[int][]int)
	for i := 0; i < n; i++ {
		root := find(i)
		groupsMap[root] = append(groupsMap[root], i)
	}
	groups := make([][]int, 0, len(groupsMap))
	for _, group := range groupsMap {
		groups = append(groups, group)
	}

	finalSizes := make([]int, 0, len(groups))
	for _, group := range groups {
		finalSizes = append(finalSizes, len(group))
	}
	sort.Ints(finalSizes)
	targetSorted := append([]int(nil), groupSizes...)
	sort.Ints(targetSorted)

	return groups, totalCost, true
}

// PartitionGraph は、頂点数 numVertice、各グループのサイズ groupSizes（総和は numVertice と一致）、
// および完全グラフの重み dist を入力とし、以下のアルゴリズムでグラフの頂点集合を分割します。
//  1. TSPの2-opt ILS（摂動はdouble-bridge法）によりサイクル（ツアー）を求める。
//  2. 得られたツアーから最も重い辺を除去し path を得る。
//  3. path を、グループサイズの回転と方向（正順／逆順）の2×(グループ数)通り候補で分割し、
//     各候補のグループ境界での切断辺の重みの合計が最大になるものを選びます。
func PartitionGraph2(numVertice int, groupSizes []int, dist [][]int) [][]int {
	// ツアーの総コストを計算するヘルパー
	tourCost := func(tour []int) int {
		cost := 0
		n := len(tour)
		for i := 0; i < n; i++ {
			cost += dist[tour[i]][tour[(i+1)%n]]
		}
		return cost
	}

	// 2-opt による局所最適化（ツアーの先頭は固定）
	twoOpt := func(tour []int) {
		n := len(tour)
		improved := true
		for improved {
			improved = false
			for i := 1; i < n-1; i++ {
				for j := i + 1; j < n; j++ {
					a, b := tour[i-1], tour[i]
					c, d := tour[j], tour[(j+1)%n]
					if dist[a][c]+dist[b][d] < dist[a][b]+dist[c][d] {
						// 反転
						for k, l := i, j; k < l; k, l = k+1, l-1 {
							tour[k], tour[l] = tour[l], tour[k]
						}
						improved = true
					}
				}
			}
		}
	}

	// double-bridge による摂動（標準的な4箇所で分割して再結合）
	doubleBridge := func(tour []int) []int {
		n := len(tour)
		// ツアーが小さい場合はフォールバック（念のため）
		if n < 8 {
			cand := make([]int, n)
			copy(cand, tour)
			i := rand.Intn(n)
			j := rand.Intn(n)
			if i != j {
				cand[i], cand[j] = cand[j], cand[i]
			}
			return cand
		}
		// 4箇所の分割点を選ぶ（各区間が非空となるように）
		a := rand.Intn(n-3) + 1       // [1, n-3]
		b := rand.Intn(n-a-2) + a + 1 // [a+1, n-2]
		c := rand.Intn(n-b-1) + b + 1 // [b+1, n-1]
		d := rand.Intn(n-c-0) + c + 1 // [c+1, n-1]
		// 各セグメントをコピー
		seg1 := make([]int, a)
		copy(seg1, tour[0:a])
		seg2 := make([]int, b-a)
		copy(seg2, tour[a:b])
		seg3 := make([]int, c-b)
		copy(seg3, tour[b:c])
		seg4 := make([]int, d-c)
		copy(seg4, tour[c:d])
		seg5 := make([]int, n-d)
		copy(seg5, tour[d:n])
		// 再結合： seg1, seg3, seg5, seg4, seg2
		newTour := make([]int, 0, n)
		newTour = append(newTour, seg1...)
		newTour = append(newTour, seg3...)
		newTour = append(newTour, seg5...)
		newTour = append(newTour, seg4...)
		newTour = append(newTour, seg2...)
		return newTour
	}

	// 初期ツアーは単純な順序 (0, 1, 2, ..., numVertice-1)
	tour := make([]int, numVertice)
	for i := 0; i < numVertice; i++ {
		tour[i] = i
	}

	rand.Seed(time.Now().UnixNano())

	// 初回2-opt による局所最適化
	twoOpt(tour)
	bestTour := make([]int, numVertice)
	copy(bestTour, tour)
	bestCost := tourCost(bestTour)

	// ILS：double-bridge で摂動し、2-opt で局所最適化
	iterations := 10 // 反復回数（必要に応じて調整）
	for it := 0; it < iterations; it++ {
		candidate := doubleBridge(bestTour)
		twoOpt(candidate)
		cost := tourCost(candidate)
		if cost < bestCost {
			bestCost = cost
			copy(bestTour, candidate)
		}
	}

	// 得られた TSP サイクルから最も重い辺を除去して path を作成
	maxWeight := -1
	removeIdx := 0
	n := numVertice
	sumW := 0
	for i := 0; i < n; i++ {
		next := (i + 1) % n
		w := dist[bestTour[i]][bestTour[next]]
		sumW += w
		if w > maxWeight {
			maxWeight = w
			removeIdx = i
		}
	}
	// fmt.Fprintln(os.Stderr, sumW)
	path := make([]int, 0, n)
	idx := (removeIdx + 1) % n
	weights := make([]int, 0, n-1)
	for {
		path = append(path, bestTour[idx])
		if idx == removeIdx {
			break
		}
		next := (idx + 1) % n
		weights = append(weights, dist[bestTour[idx]][bestTour[next]])
		idx = next
	}

	// 以下、path をグループに分割する際、
	// グループサイズ groupSizes の回転と方向（正順／逆順）の全 2×(グループ数) 通りの候補で
	// 各候補において「グループ境界の切断辺の重みの合計」を計算し、
	// その値が最大となる分割候補を貪欲に選択します。
	m := len(groupSizes)
	left := 0
	right := len(weights)

	added := make([]bool, m)
	groups := make([][]int, 0, m)
	for cutNum := 0; cutNum < m-1; cutNum++ {
		maxW := -INF
		maxG := -1
		maxP := -1
		for groupIdx := 0; groupIdx < m; groupIdx++ {
			if added[groupIdx] {
				continue
			}
			size := groupSizes[groupIdx]
			for pattern := 0; pattern < 2; pattern++ {
				w := weights[left+size-1]
				if pattern == 1 {
					w = weights[right-size]
				}
				if w > maxW {
					maxW = w
					maxG = groupIdx
					maxP = pattern
				}
			}
		}
		if maxP == 0 {
			size := groupSizes[maxG]
			groups = append(groups, path[left:left+size])
			left = left + size
		} else {
			size := groupSizes[maxG]
			groups = append(groups, path[right-size+1:right+1])
			right = right - size
		}
		added[maxG] = true
	}
	groups = append(groups, path[left:right+1])

	return groups
}

func PartitionGraph3(gameconst *GameConst, numVertice int, groupSizes []int, adjacentList [][]int, dist [][]int, globalMST [][2]int, randomize bool) ([][]int, int, bool) {
	numGroups := len(groupSizes)
	sizeOrder := make([]int, numGroups)
	for i := range sizeOrder {
		sizeOrder[i] = i
	}
	sort.Slice(sizeOrder, func(i, j int) bool { return groupSizes[sizeOrder[i]] < groupSizes[sizeOrder[j]] })
	ranks := make([]int, numGroups)
	for i := range sizeOrder {
		ranks[sizeOrder[i]] = i
	}
	if randomize {
		for i := range ranks {
			ranks[i] += gameconst.Random.Intn(3)
		}
	}
	sort.Slice(sizeOrder, func(i, j int) bool { return ranks[sizeOrder[i]] < ranks[sizeOrder[j]] })
	sortedGraphSizes := make([]int, 0, numGroups)
	for i := range sizeOrder {
		sortedGraphSizes = append(sortedGraphSizes, groupSizes[sizeOrder[i]])
	}
	groups := make([][]int, 0, numGroups)
	remainingEdges := globalMST
	remainingVertice := make([]int, numVertice)
	for i := range remainingVertice {
		remainingVertice[i] = i
	}
	groupIndex := 0
	assigned := make([]bool, numGroups)
	value := 0
	for len(groups) < numGroups {
		if groupIndex == numGroups {
			groupIndex = 0
		}
		if len(groups) == numGroups-1 {
			groups = append(groups, remainingVertice)
			break
		}

		if assigned[groupIndex] {
			groupIndex++
			continue
		}
		ok, v1, gs, es := splitTree(numVertice, remainingVertice, remainingEdges, adjacentList, dist, sortedGraphSizes[groupIndex])
		ok2, v2, gs2, es2 := splitTree(numVertice, remainingVertice, remainingEdges, adjacentList, dist, len(remainingVertice)-sortedGraphSizes[groupIndex])
		gs2[0], gs2[1] = gs2[1], gs2[0]
		es2[0], es2[1] = es2[1], es2[0]
		if v2 < v1 {
			gs = gs2
			es = es2
			v1 = v2
		}
		if !ok && !ok2 {
			fmt.Fprintln(os.Stderr, "WARN: split failed!!!", numGroups, len(groups))
			groups = append(groups, remainingVertice)
			break
		}
		value += v1
		groups = append(groups, gs[0])
		remainingVertice = gs[1]
		remainingEdges = es[1]
		assigned[groupIndex] = true
		groupIndex++
	}

	return groups, value, len(groups) == numGroups
}

type SortedQueue struct {
	items     [][2]int
	popIndex  int
	pushIndex int
	width     int
	sorted    bool

	boarder int
}

// 新しいQueueを生成する関数
func NewSortedQueue(width int) *SortedQueue {
	q := SortedQueue{}
	q.items = make([][2]int, 2*width)
	q.popIndex = 0
	q.pushIndex = 0
	q.boarder = INF
	q.width = width
	return &q
}

// Queueを初期化する関数(幅の再設定は要書き換え)
func (q *SortedQueue) Init() {
	q.popIndex = 0
	q.pushIndex = 0
	q.width = len(q.items) / 2
	q.boarder = INF
	q.sorted = false
}

// Queueから最もbeamValue大きいものをとる操作
func (q *SortedQueue) Pop() int {
	ret := q.items[q.popIndex]
	q.popIndex++
	return ret[0]
}

// StateをQueueに入れる操作
func (q *SortedQueue) Push(item [2]int) bool {
	pushed := false
	if q.pushIndex >= len(q.items) {
		q.Sort()
	}
	if q.boarder < item[1] {
		// この段階で棄却
		return pushed
	}
	pushed = true
	q.sorted = false
	q.items[q.pushIndex] = item
	q.pushIndex++
	return pushed
}

// valueの大きい順に並べ替える
func (q *SortedQueue) Sort() {
	sort.Slice(q.items[:q.pushIndex], func(i, j int) bool { return q.items[i][1] < q.items[j][1] })
	if q.pushIndex > q.width {
		// 上位width分以外は捨てる
		q.pushIndex = q.width
		q.boarder = q.items[q.width-1][1]
	}
	q.sorted = true
}

func (q *SortedQueue) Promising(v int) bool {
	return q.boarder > v
}

func (q *SortedQueue) Len() int {
	// 正しくはこれだが、安全性を無視するなら二つ目のほうが高速
	m := minInt(q.width, q.pushIndex)
	l := maxInt(m-q.popIndex, 0)
	// l := q.pushIndex - q.popIndex
	return l
}
