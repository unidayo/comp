// main.go (提出用・単一ファイル)
package main

import (
	"bufio"
	"bytes"
	"container/heap"
	"fmt"
	"math"
	"math/rand"
	"os"
	"sort"
	"strconv"
	"sync"
	"time"
)

/********** 定数 **********/
const (
	INF        = 1 << 29
	TIME_LIMIT = 1800 * 1000 // microseconds（テンプレ準拠）
	START_TEMP = 6.00
	END_TEMP   = 0.02
)

/********** 方向定義 **********/
const (
	DirU = 0
	DirD = 1
	DirL = 2
	DirR = 3
)

var opp = []int{DirD, DirU, DirR, DirL}
var dirChar = []byte{'U', 'D', 'L', 'R'}

/********** ゲーム定数 **********/
type Point struct{ i, j int }

type GameConst struct {
	Random  *rand.Rand
	Scanner *bufio.Scanner

	N  int      // グリッドのサイズ
	K  int      // 目的地の数
	T  int      // 長さ制約
	v  []string // N行, 各 N-1（左右の壁）
	h  []string // N-1行, 各 N  （上下の壁）
	tg []Point  // 目的地

	nei [][]int // cellID -> 4方向の隣接 cellID（壁: -1）
	TOT int     // 4*N*N（向き付き状態数）

	// 追加（高速化と候補絞り込み用）
	deg      []int8 // 各マスの開口数（0..4）
	connPat  []int8 // 開口2のとき「残り2方向をつなぐときのパターン」（1,2,3）/ それ以外は -1
	isTarget []bool // 目的地フラグ
}

func NewGameConst(randSeed int64) *GameConst {
	return &GameConst{Random: rand.New(rand.NewSource(randSeed))}
}
func (g *GameConst) SetScanner(scanner *bufio.Scanner) { g.Scanner = scanner }

// 入力
func GetInputs(g *GameConst) {
	sc := g.Scanner
	sc.Split(bufio.ScanWords)
	readInt := func() int { sc.Scan(); x, _ := strconv.Atoi(sc.Text()); return x }
	readStr := func() string { sc.Scan(); return sc.Text() }

	g.N, g.K, g.T = readInt(), readInt(), readInt()
	g.v = make([]string, g.N)
	for i := 0; i < g.N; i++ {
		g.v[i] = readStr()
	}
	g.h = make([]string, g.N-1)
	for i := 0; i < g.N-1; i++ {
		g.h[i] = readStr()
	}
	g.tg = make([]Point, g.K)
	for k := 0; k < g.K; k++ {
		g.tg[k] = Point{readInt(), readInt()}
	}

	// 隣接（壁を考慮）
	g.nei = make([][]int, g.N*g.N)
	for i := 0; i < g.N; i++ {
		for j := 0; j < g.N; j++ {
			id := i*g.N + j
			g.nei[id] = []int{id, id, id, id}
			if i > 0 && g.h[i-1][j] == '0' {
				g.nei[id][DirU] = (i-1)*g.N + j
			}
			if i < g.N-1 && g.h[i][j] == '0' {
				g.nei[id][DirD] = (i+1)*g.N + j
			}
			if j > 0 && g.v[i][j-1] == '0' {
				g.nei[id][DirL] = i*g.N + (j - 1)
			}
			if j < g.N-1 && g.v[i][j] == '0' {
				g.nei[id][DirR] = i*g.N + (j + 1)
			}
		}
	}
	g.TOT = (g.N * g.N) << 2

	// 前計算：deg / connPat / isTarget
	g.deg = make([]int8, g.N*g.N)
	g.connPat = make([]int8, g.N*g.N)
	for id := 0; id < g.N*g.N; id++ {
		open := 0
		ds := [4]int{}
		cnt := 0
		for d := 0; d < 4; d++ {
			if g.nei[id][d] >= 0 {
				open++
				if cnt < 4 {
					ds[cnt] = d
					cnt++
				}
			}
		}
		g.deg[id] = int8(open)
		g.connPat[id] = -1
		if open == 2 {
			d1, d2 := ds[0], ds[1]
			if d1 == opp[d2] {
				g.connPat[id] = 1 // 直進
			} else if (d1 == DirU && d2 == DirL) || (d1 == DirL && d2 == DirU) || (d1 == DirD && d2 == DirR) || (d1 == DirR && d2 == DirD) {
				g.connPat[id] = 2
			} else {
				g.connPat[id] = 3
			}
		}
	}
	g.isTarget = make([]bool, g.N*g.N)
	for k := 0; k < g.K; k++ {
		id := g.tg[k].i*g.N + g.tg[k].j
		g.isTarget[id] = true
	}
}

/********** ゲーム状態 **********/
// assign[cell] = -1: ターミナル, 0..3: 固定パターン
type GameState struct{ assign []int8 }

func NewGameState(gc *GameConst) *GameState {
	st := &GameState{assign: make([]int8, gc.N*gc.N)}
	for i := range st.assign {
		st.assign[i] = -1
	} // 初期は全ターミナル
	return st
}

/********** 出力構造 **********/
type Rule struct {
	c, q, A, S int
	D          byte
}
type Action struct {
	C, Q, M int
	s       [][]int
	rules   []Rule
}

// 出力整形
func FormatAction(a *Action, _ *GameState, c *GameConst) string {
	var buf bytes.Buffer
	fmt.Fprintf(&buf, "%d %d %d\n", a.C, a.Q, a.M)
	for i := 0; i < c.N; i++ {
		for j := 0; j < c.N; j++ {
			if j > 0 {
				buf.WriteByte(' ')
			}
			buf.WriteString(strconv.Itoa(a.s[i][j]))
		}
		buf.WriteByte('\n')
	}
	for idx, r := range a.rules {
		fmt.Fprintf(&buf, "%d %d %d %d %c", r.c, r.q, r.A, r.S, r.D)
		if idx+1 < len(a.rules) {
			buf.WriteByte('\n')
		}
	}
	return buf.String()
}

/********** 固定パターン写像 F_c(q) と逆算 **********/
func patternMap(c, q int) int {
	switch c {
	case 0:
		return opp[q] // 反射
	case 1:
		return q // 直進
	case 2:
		switch q {
		case DirU:
			return DirL
		case DirL:
			return DirU
		case DirD:
			return DirR
		case DirR:
			return DirD
		}
	case 3:
		switch q {
		case DirU:
			return DirR
		case DirR:
			return DirU
		case DirD:
			return DirL
		case DirL:
			return DirD
		}
	}
	return -1
}
func patternFrom(qIn, qOut int) int {
	if opp[qIn] == qOut {
		return 0
	}
	if qIn == qOut {
		return 1
	}
	if (qIn == DirU && qOut == DirL) || (qIn == DirL && qOut == DirU) || (qIn == DirD && qOut == DirR) || (qIn == DirR && qOut == DirD) {
		return 2
	}
	return 3
}

/********** Deque（0-1 BFS 用リングバッファ, 2の冪長・ビット演算） **********/
type Deque struct {
	data       []int
	head, tail int
	mask       int
}

func NewDeque(cap int) *Deque {
	if cap < 8 {
		cap = 8
	}
	// 2 の冪に丸める
	n := 1
	for n < cap {
		n <<= 1
	}
	d := &Deque{
		data: make([]int, n),
		head: n / 2, tail: n / 2,
		mask: n - 1,
	}
	return d
}
func (dq *Deque) Empty() bool { return dq.head == dq.tail }
func (dq *Deque) PushFront(x int) {
	dq.head = (dq.head - 1) & dq.mask
	dq.data[dq.head] = x
}
func (dq *Deque) PushBack(x int) {
	dq.data[dq.tail] = x
	dq.tail = (dq.tail + 1) & dq.mask
}
func (dq *Deque) PopFront() int {
	x := dq.data[dq.head]
	dq.head = (dq.head + 1) & dq.mask
	return x
}
func (dq *Deque) Reset() { mid := len(dq.data) / 2; dq.head, dq.tail = mid, mid }

/********** 0-1 BFS / 4×4コスト行列 / 4状態DP **********/
func nodeID(cell, q int) int       { return (cell << 2) | q }
func fromNodeID(id int) (int, int) { return id >> 2, id & 3 }

// 0-1 BFS（コスト：出発セルがターミナル=1、固定=0）: スタンプ法で O(TOT) 初期化を排除
func bfs01ToTarget(cn *GameConst, st *GameState, srcCell, srcQ, dstCell int,
	dist []int, seen []int, stamp *int, dq *Deque) [4]int {

	*stamp++
	curStamp := *stamp
	dq.Reset()

	src := (srcCell << 2) | srcQ
	dist[src] = 0
	seen[src] = curStamp
	dq.PushFront(src)

	ans := [4]int{INF, INF, INF, INF}
	left := 4

	for !dq.Empty() {
		now := dq.PopFront()
		// fromNodeID(now)
		cell := now >> 2
		qIn := now & 3
		d0 := dist[now] // seen[now]==curStamp

		// 目的セルの各 qIn ごと最短が確定したらカウントダウン
		if cell == dstCell && ans[qIn] == INF {
			ans[qIn] = d0
			left--
			if left == 0 {
				break
			}
		}

		// 出発コスト（固定=0, ターミナル=1）
		w := 1
		if st.assign[cell] >= 0 {
			w = 0
		}

		typ := int(st.assign[cell])
		if typ >= 0 {
			// 固定セル：1 通りだけ遷移
			// patternMap(typ, qIn) をインライン
			var d int
			switch typ {
			case 0:
				d = opp[qIn]
			case 1:
				d = qIn
			case 2:
				switch qIn {
				case DirU:
					d = DirL
				case DirL:
					d = DirU
				case DirD:
					d = DirR
				default:
					d = DirD
				}
			default: // 3
				switch qIn {
				case DirU:
					d = DirR
				case DirR:
					d = DirU
				case DirD:
					d = DirL
				default:
					d = DirD
				}
			}
			if nc := cn.nei[cell][d]; nc >= 0 {
				nid := (nc << 2) | d
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				nd := d0 + w
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
		} else {
			// ターミナル：4 方向へ
			nei := cn.nei[cell]
			nd := d0 + w
			// U,D,L,R の順に展開
			if nc := nei[0]; nc >= 0 {
				nid := (nc << 2) | 0
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
			if nc := nei[1]; nc >= 0 {
				nid := (nc << 2) | 1
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
			if nc := nei[2]; nc >= 0 {
				nid := (nc << 2) | 2
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
			if nc := nei[3]; nc >= 0 {
				nid := (nc << 2) | 3
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
		}
	}
	return ans
}

type Dist4x4 struct{ m [4][4]int }

// evaluateLite（使っていなくてもビルドのために更新）
func evaluateLite(cn *GameConst, st *GameState, dist []int, seen []int, stamp *int, dq *Deque) int {
	M := buildCostMatrices(cn, st, dist, seen, stamp, dq)
	cost, _ := dpMinTerminal(cn, M)
	return cost
}

// buildCostMatrices
func buildCostMatrices(cn *GameConst, st *GameState, dist []int, seen []int, stamp *int, dq *Deque) []Dist4x4 {
	if cn.K <= 1 {
		return nil
	}
	M := make([]Dist4x4, cn.K-1)
	for i := 0; i < cn.K-1; i++ {
		ai := cn.tg[i].i*cn.N + cn.tg[i].j
		bi := cn.tg[i+1].i*cn.N + cn.tg[i+1].j
		var dm Dist4x4
		for q := 0; q < 4; q++ {
			ans := bfs01ToTarget(cn, st, ai, q, bi, dist, seen, stamp, dq)
			for q2 := 0; q2 < 4; q2++ {
				dm.m[q][q2] = ans[q2]
			}
		}
		M[i] = dm
	}
	return M
}
func dpMinTerminal(cn *GameConst, M []Dist4x4) (int, []int) {
	if cn.K <= 1 {
		return 0, []int{0}
	}
	dp := make([][4]int, cn.K)
	par := make([][4]int, cn.K)
	for q := 0; q < 4; q++ {
		dp[0][q], par[0][q] = INF, -1
	}
	dp[0][0] = 0
	for i := 0; i < cn.K-1; i++ {
		for q2 := 0; q2 < 4; q2++ {
			best, arg := INF, -1
			for q := 0; q < 4; q++ {
				if dp[i][q] >= INF {
					continue
				}
				if v := dp[i][q] + M[i].m[q][q2]; v < best {
					best, arg = v, q
				}
			}
			dp[i+1][q2], par[i+1][q2] = best, arg
		}
	}
	best, last := INF, 0
	for q := 0; q < 4; q++ {
		if dp[cn.K-1][q] < best {
			best, last = dp[cn.K-1][q], q
		}
	}
	if best >= INF {
		return INF, nil
	}
	qs := make([]int, cn.K)
	qs[cn.K-1] = last
	for i := cn.K - 1; i >= 1; i-- {
		qs[i-1] = par[i][qs[i]]
	}
	return best, qs
}

// 経路復元（0-1 BFS を1回, スタンプ法）
func restorePath01(cn *GameConst, st *GameState, srcCell, srcQ, dstCell, dstQ int,
	dist []int, seen []int, stamp *int, dq *Deque, parent []int) []int {

	*stamp++
	curStamp := *stamp
	dq.Reset()

	src := (srcCell << 2) | srcQ
	dst := (dstCell << 2) | dstQ

	dist[src] = 0
	seen[src] = curStamp
	parent[src] = -1
	dq.PushFront(src)

	for !dq.Empty() {
		now := dq.PopFront()
		if now == dst {
			break
		}
		cell := now >> 2
		qIn := now & 3
		d0 := dist[now]

		w := 1
		if st.assign[cell] >= 0 {
			w = 0
		}

		typ := int(st.assign[cell])
		if typ >= 0 {
			// patternMap(typ, qIn) をインライン
			var d int
			switch typ {
			case 0:
				d = opp[qIn]
			case 1:
				d = qIn
			case 2:
				switch qIn {
				case DirU:
					d = DirL
				case DirL:
					d = DirU
				case DirD:
					d = DirR
				default:
					d = DirD
				}
			default:
				switch qIn {
				case DirU:
					d = DirR
				case DirR:
					d = DirU
				case DirD:
					d = DirL
				default:
					d = DirD
				}
			}
			if nc := cn.nei[cell][d]; nc >= 0 {
				nid := (nc << 2) | d
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				nd := d0 + w
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					parent[nid] = now
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
		} else {
			nei := cn.nei[cell]
			nd := d0 + w
			if nc := nei[0]; nc >= 0 {
				nid := (nc << 2) | 0
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					parent[nid] = now
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
			if nc := nei[1]; nc >= 0 {
				nid := (nc << 2) | 1
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					parent[nid] = now
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
			if nc := nei[2]; nc >= 0 {
				nid := (nc << 2) | 2
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					parent[nid] = now
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
			if nc := nei[3]; nc >= 0 {
				nid := (nc << 2) | 3
				old := INF
				if seen[nid] == curStamp {
					old = dist[nid]
				}
				if nd < old {
					dist[nid] = nd
					seen[nid] = curStamp
					parent[nid] = now
					if w == 0 {
						dq.PushFront(nid)
					} else {
						dq.PushBack(nid)
					}
				}
			}
		}
	}

	// 未到達
	if seen[dst] != curStamp {
		return nil
	}

	// 復元
	path := []int{}
	for cur := dst; cur != -1; cur = parent[cur] {
		path = append(path, cur)
	}
	for i, j := 0, len(path)-1; i < j; i, j = i+1, j-1 {
		path[i], path[j] = path[j], path[i]
	}
	return path
}

/********** 評価（厳密） **********/
type Step struct{ cell, qIn, qOut int }

type EvalInfo struct {
	TermCost int
	Steps    []Step
	StepLen  int
	CEst     int
	visitC   map[int][]int
	visitQ   map[int][]int
	visitD   map[int][]int
}

func evaluate(cn *GameConst, st *GameState, dist []int, seen []int, stamp *int, dq *Deque, parent []int) EvalInfo {
	M := buildCostMatrices(cn, st, dist, seen, stamp, dq)
	cost, qs := dpMinTerminal(cn, M)
	if cost >= INF {
		return EvalInfo{TermCost: INF}
	}

	steps := make([]Step, 0, cost+16)
	visitC, visitQ, visitD := map[int][]int{}, map[int][]int{}, map[int][]int{}
	stepLen := 0

	for i := 0; i < cn.K-1; i++ {
		ti := cn.tg[i].i*cn.N + cn.tg[i].j
		tj := cn.tg[i+1].i*cn.N + cn.tg[i+1].j
		qi, qj := qs[i], qs[i+1]
		path := restorePath01(cn, st, ti, qi, tj, qj, dist, seen, stamp, dq, parent)
		if path == nil {
			return EvalInfo{TermCost: INF}
		}
		stepLen += len(path) - 1
		for k := 0; k+1 < len(path); k++ {
			id0, id1 := path[k], path[k+1]
			cell, qIn := fromNodeID(id0)
			_, qOut := fromNodeID(id1)
			steps = append(steps, Step{cell: cell, qIn: qIn, qOut: qOut})
			cp := patternFrom(qIn, qOut)
			visitC[cell] = append(visitC[cell], cp)
			visitQ[cell] = append(visitQ[cell], qIn)
			visitD[cell] = append(visitD[cell], qOut)
		}
	}
	// --- ここから置換：接尾固定を織り込んだ色コスト推定 ---
	// 固定パターン色の種類集合（固定セルはもちろん、非固定セルでも接尾固定に使う cp を入れる）
	patSet := map[int]bool{}
	// ターミナル固有色：各セルの「最後にパターンが切り替わる位置」までの訪問回数を加算
	termColorCost := 0

	for cell, cs := range visitC {
		_ = cell
		if len(cs) == 0 {
			continue
		}
		// 接尾が一定となる開始位置 s を求める（cs[s:] がすべて cp）
		cp := cs[len(cs)-1]
		s := len(cs) - 1
		for s > 0 && cs[s-1] == cp {
			s--
		}
		// 固定パターン色集合に cp を追加（接尾固定で使う）
		patSet[cp] = true
		// 接尾開始 s までが固有色を要する範囲
		termColorCost += s
	}

	// 必要な固定パターン色の種類数
	patKinds := len(patSet)
	// 最終コスト（CEst）は「固定パターン色」＋「固有色（接尾前）」
	cEst := patKinds + termColorCost

	return EvalInfo{
		TermCost: cost,
		Steps:    steps,
		StepLen:  stepLen,
		CEst:     cEst,
		visitC:   visitC,
		visitQ:   visitQ,
		visitD:   visitD,
	}

}

func MaxInt(a int, b int) int {
	if a > b {
		return a
	}
	return b
}

func buildOutput2(cn *GameConst, st *GameState, ev EvalInfo) *Action {
	seq, finalPattern := buildKeySeqAndFinalPattern(cn, ev)
	revSeq := []*KeyPoint{}
	for i := range seq {
		revSeq = append(revSeq, seq[len(seq)-1-i])
	}
	fmt.Fprintf(os.Stderr, "num_terminal: %d\n", len(seq))
	bs := NewBeamSearch()
	start := time.Now()
	maxLane := maxInt(1, int(0.8*math.Sqrt(float64(len(seq)))/4+0.7))
	bs.Init(190*1000/maxInt(1, len(revSeq))*20/(2*maxLane+1), len(seq))
	// fmt.Println(len(seq))
	bs.maxLane = maxLane
	bestState := NewBeamSearchState(len(seq)+100, cn)
	bestState.Init(cn, revSeq, finalPattern)
	bestState = bs.Run(bestState, cn, revSeq)
	fmt.Fprintln(os.Stderr, "$time=", time.Since(start).Microseconds())
	fmt.Fprintln(os.Stderr, "$maxLane=", maxLane)
	fmt.Fprintln(os.Stderr, "$lenseq=", len(seq))
	action := &Action{rules: make([]Rule, 0)}
	action.C = bestState.usedColor + 4
	action.Q = bs.maxLane * 4

	for c := 0; c < 4; c++ {
		for q := 0; q < action.Q; q++ {
			action.AddFixedRule(c, q, c)
		}
	}
	plusq := (-bestState.requiredLane + bs.maxLane) * 4
	for c := 4; c < action.C; c++ {
		plusC := c - 4
		for q := 0; q < action.Q; q++ {
			index := plusC*action.Q + (q+(action.Q-plusq))%action.Q
			A := bestState.nextColor[index] + 4
			S := (bestState.nextQ[index] + plusq) % action.Q
			action.AddRule(c, q, A, S>>2, S&0b11)
		}
	}
	action.s = make([][]int, cn.N)
	for i := range action.s {
		action.s[i] = make([]int, cn.N)
		for j := range action.s[i] {
			index := i*cn.N + j
			c := bestState.requiredColor[index] + 4
			if c < 0 {
				c = 0
			}
			action.s[i][j] = c
		}
	}
	// fmt.Println(action)
	// fmt.Println(bestState.maxIndex)
	// fmt.Println(bestState.usedColor)
	// fmt.Println(bestState.nextColor)
	// fmt.Println(bestState.nextQ)
	return action
}

func (action *Action) AddFixedRule(c, q, pattern int) {
	lane := q >> 2
	indir := q & 0b11
	dir := patternMap(pattern, indir)
	action.AddRule(c, q, c, lane, dir)
}
func (action *Action) AddRule(c, q, A, lane, dir int) {
	action.rules = append(action.rules, Rule{c: c, q: q, A: A, S: lane*4 + dir, D: dirChar[dir]})
	action.M++
}

// 出力生成（固定は共有色、ターミナルは「レーン付き（Q=4*a）」＋方向共通色＋接尾固定化）
func buildOutput(cn *GameConst, st *GameState, ev EvalInfo) *Action {
	N, CUR := cn.N, 0

	// 1) 全訪問が同一パターンのセルを固定
	fixedType := map[int]int{}
	usedPat := map[int]bool{}
	for cell, cs := range ev.visitC {
		first, ok := cs[0], true
		for _, x := range cs {
			if x != first {
				ok = false
				break
			}
		}
		if ok {
			fixedType[cell] = first
			usedPat[first] = true
		}
	}

	// 1.5) 接尾が一定のセル（固定セルは対象外）
	suffixStart := map[int]int{} // 接尾開始位置
	suffixPat := map[int]int{}   // 接尾パターン
	for cell, cs := range ev.visitC {
		if _, ok := fixedType[cell]; ok {
			continue
		}
		cp := cs[len(cs)-1]
		s := len(cs) - 1
		for s > 0 && cs[s-1] == cp {
			s--
		}
		if s > 0 {
			suffixStart[cell] = s
			suffixPat[cell] = cp
			usedPat[cp] = true // 接尾固定色も確保
		}
	}

	// 2) パターン→色ID（使ったものだけ割当）
	pat2color := map[int]int{}
	for p := 0; p < 4; p++ {
		if usedPat[p] {
			pat2color[p] = CUR
			CUR++
		}
	}

	// 3) 接尾「前」のターミナル到達のみ数える
	L := len(ev.Steps)
	stepPos := make([]int, L) // 各ステップでのそのセルの訪問 index（全ステップで保持）
	pos := map[int]int{}      // cell -> 次に割り当てる訪問 index
	isTerm := make([]bool, L) // このステップが接尾前のターミナルか
	dirCnt := [4]int{}        // 方向別カウント（接尾前のみ）
	idxDir := make([]int, L)  // このステップの「方向内での」通し番号

	for i := 0; i < L; i++ {
		cell := ev.Steps[i].cell
		p := pos[cell]
		stepPos[i] = p
		pos[cell] = p + 1
		if _, ok := fixedType[cell]; ok {
			continue
		}
		if sfx, ok := suffixStart[cell]; ok && p >= sfx {
			continue // 接尾は固定扱い
		}
		// 接尾前のターミナル
		isTerm[i] = true
		d := ev.Steps[i].qIn
		idxDir[i] = dirCnt[d]
		dirCnt[d]++
	}

	// 4) a を選ぶ（4*a + ceil(max_dir/a) を最小化）
	maxDir := 0
	for d := 0; d < 4; d++ {
		if dirCnt[d] > maxDir {
			maxDir = dirCnt[d]
		}
	}
	a := 1
	if maxDir > 0 {
		best, bestA := 1<<60, 1
		for cand := 1; cand <= maxDir; cand++ {
			termColors := (maxDir + cand - 1) / cand
			v := 4*cand + termColors // 固定色は定数項なのでここでは無視
			if v < best {
				best, bestA = v, cand
			}
		}
		a = bestA
	}
	// ターミナル共通色（方向共通プール）：バケット個数
	termBuckets := 0
	if maxDir > 0 {
		termBuckets = (maxDir + a - 1) / a
	}
	termBase := CUR
	CUR += termBuckets

	// 5) 接尾前ターミナルに「色」と「レーン k」を割当
	colorByCellPos := map[int]map[int]int{} // cell,pos -> 色
	laneStep := make([]int, L)              // 各ステップのレーン（接尾前のみ）
	dirSeen := [4]int{}
	for i := 0; i < L; i++ {
		if !isTerm[i] {
			continue
		}
		d := ev.Steps[i].qIn
		idx := dirSeen[d]
		dirSeen[d]++
		b := idx / a
		k := idx % a
		col := termBase + b
		laneStep[i] = k
		if colorByCellPos[ev.Steps[i].cell] == nil {
			colorByCellPos[ev.Steps[i].cell] = map[int]int{}
		}
		colorByCellPos[ev.Steps[i].cell][stepPos[i]] = col
	}

	// 6) 盤面初期色
	sgrid := make([][]int, N)
	for i := 0; i < N; i++ {
		sgrid[i] = make([]int, N)
	}
	for cell := range ev.visitC {
		ii, jj := cell/cn.N, cell%cn.N
		if ft, ok := fixedType[cell]; ok {
			sgrid[ii][jj] = pat2color[ft]
		} else if c0, ok := colorByCellPos[cell][0]; ok {
			sgrid[ii][jj] = c0
		} else if sp, ok := suffixPat[cell]; ok {
			sgrid[ii][jj] = pat2color[sp]
		}
	}

	// 7) 「このステップの後に来る次のターミナル」のレーンを前計算
	nextLane := make([]int, L)
	for i := 0; i < L; i++ {
		nextLane[i] = -1
	}
	last := -1
	for i := L - 1; i >= 0; i-- {
		nextLane[i] = last
		if isTerm[i] {
			last = laneStep[i]
		}
	}

	// 8) 規則出力（遭遇 (c,q) のみ）
	type Key struct{ c, q int }
	seen := map[Key]bool{}
	rules := make([]Rule, 0, L+16)

	emitFixed := func(colorID, qInAdv, dOut int) {
		k := Key{colorID, qInAdv}
		if seen[k] {
			return
		}
		seen[k] = true
		kCur := qInAdv / 4
		rules = append(rules, Rule{c: colorID, q: qInAdv, A: colorID, S: 4*kCur + dOut, D: dirChar[dOut]})
	}

	// レーンはターミナルでだけ更新。固定セルはレーン維持（S=4*k + dirOut）
	kCur := 0 // 初期状態 q=0 → k=0（dp[0][0]=0 と整合）
	for i := 0; i < L; i++ {
		cell := ev.Steps[i].cell
		dirIn, dirOut := ev.Steps[i].qIn, ev.Steps[i].qOut
		qInAdv := 4*kCur + dirIn

		// 固定セル（全訪問一定）
		if ft, ok := fixedType[cell]; ok {
			emitFixed(pat2color[ft], qInAdv, dirOut)
			continue
		}
		// 接尾（固定化済み）
		if s, ok := suffixStart[cell]; ok && stepPos[i] >= s {
			emitFixed(pat2color[suffixPat[cell]], qInAdv, dirOut)
			continue
		}
		// 接尾前ターミナル
		if isTerm[i] {
			colorID := colorByCellPos[cell][stepPos[i]]
			// このセルの次訪問で読む色 A
			A := colorID
			if nextCol, ok := colorByCellPos[cell][stepPos[i]+1]; ok {
				A = nextCol
			} else if sp, ok := suffixPat[cell]; ok && stepPos[i]+1 >= suffixStart[cell] {
				A = pat2color[sp]
			}
			// 次のターミナルで使うレーンに切替（無ければ現レーン維持）
			kNext := kCur
			if nextLane[i] >= 0 {
				kNext = nextLane[i]
			}

			k := Key{colorID, qInAdv}
			if !seen[k] {
				seen[k] = true
				rules = append(rules, Rule{c: colorID, q: qInAdv, A: A, S: 4*kNext + dirOut, D: dirChar[dirOut]})
			}
			kCur = kNext
			continue
		}
	}

	if len(rules) > cn.T {
		rules = rules[:cn.T]
	}
	return &Action{C: CUR, Q: 4 * a, M: len(rules), s: sgrid, rules: rules}
}

/********** 近傍用：候補パターン制限 **********/
func connectPatternForTwo(cn *GameConst, cell int) (int, bool) {
	open := []int{}
	for d := 0; d < 4; d++ {
		if cn.nei[cell][d] >= 0 {
			open = append(open, d)
		}
	}
	if len(open) != 2 {
		return -1, false
	}
	a, b := open[0], open[1]
	for c := 1; c <= 3; c++ {
		if patternMap(c, opp[a]) == b && patternMap(c, opp[b]) == a {
			return c, true
		}
	}
	return -1, false
}
func fixedCandidates(cn *GameConst, cell int) []int {
	deg := int(cn.deg[cell])
	isT := cn.isTarget[cell]
	if deg == 1 { // 3壁
		if isT {
			return []int{0}
		}
		return nil
	}
	if deg == 2 { // 2方向のみ開口： {0, 残りをつなぐ1色} or {残り1色}
		c := int(cn.connPat[cell]) // 1(直進)/2/3 が入っている or -1
		if c < 1 {
			return nil
		}
		if isT {
			return []int{0, c}
		}
		return []int{c}
	}
	// 開口3/4：非目的地なら0は選ぶ理由なし
	if isT {
		return []int{1, 2, 3, 0}
	}
	return []int{1, 2, 3}
}

// 元の deg をそのまま使う（可読のための薄いラッパ）
func (cn *GameConst) openCntDummy(cell int) int { return int(cn.deg[cell]) }

/********** ソルバ（焼きなまし） **********/
type Solver struct{ timeLimit time.Duration }

func NewSolver(us int) *Solver { return &Solver{timeLimit: time.Duration(us) * time.Microsecond} }

func (s *Solver) Solve(st *GameState, cn *GameConst) (*Action, int, int) {
	/*deadline := time.Now().Add(s.timeLimit)
	start := deadline.Add(-s.timeLimit)      // 進行0時点
	logTR := math.Log(END_TEMP / START_TEMP) // 温度用（START_TEMP==END_TEMPなら0）

	// 進捗ログ用（0,5,10,...,100%）
	nextLogTick := 0 // 0..20
	logIfNeeded := func(p float64, loops int, lambda float64, cur EvalInfo, best EvalInfo) {
		// p in [0,1] を 5% 刻みの tick(=0..20) に
		tick := int(p*20 + 1e-9)
		if tick < 0 {
			tick = 0
		}
		if tick >= 20 && p >= 0.999999 {
			tick = 20
		}
		for nextLogTick <= tick {
			percent := nextLogTick * 5

			// 現在λで score を再計算（現在解・最良解ともに）
			curScore := calcScore(cur, lambda, cn)
			bestScoreNow := calcScore(best, lambda, cn)

			fmt.Fprintf(os.Stderr,
				"[PROG %3d%%] loops=%d | cur: score=%.6g Term=%d Len=%d CEst=%d | best: score=%.6g Term=%d Len=%d CEst=%d | lambda=%.6g\n",
				percent, loops,
				curScore, cur.TermCost, cur.StepLen, cur.CEst,
				bestScoreNow, best.TermCost, best.StepLen, best.CEst,
				lambda,
			)
			nextLogTick++
		}
	}

	// ワーク
	dist := make([]int, cn.TOT)
	seen := make([]int, cn.TOT) // ★追加: スタンプ法用
	stamp := 0                  // ★追加: BFSごとに++するカウンタ
	dq := NewDeque(1 << 12)
	parent := make([]int, cn.TOT)

	// 初期（全ターミナル）
	bestEval := evaluate(cn, st, dist, seen, &stamp, dq, parent)
	if bestEval.TermCost >= INF {
		a := &Action{C: 1, Q: 1, M: 0, s: make([][]int, cn.N)}
		for i := 0; i < cn.N; i++ {
			a.s[i] = make([]int, cn.N)
		}
		return a, 0, 0
	}
	startLambda := 1e-3
	endLambda := 1e2
	if bestEval.StepLen*21 > cn.T*20 {
		startLambda = 1e5
		endLambda = 1e5
	}
	lambda := startLambda

	bestScore := calcScore(bestEval, 0.0, cn)
	bestAssign := make([]int8, len(st.assign))
	copy(bestAssign, st.assign)

	curEval, curScore := bestEval, bestScore
	curAssign := make([]int8, len(st.assign))
	copy(curAssign, st.assign)

	// ★ 初期時点(0%)のログを出す
	logIfNeeded(0.0, 0, lambda, curEval, bestEval)

	loops := 0
	for {
		now := time.Now()
		if now.After(deadline) {
			break
		}
		p := float64(now.Sub(start)) / float64(s.timeLimit)
		if p < 0 {
			p = 0
		} else if p > 1 {
			p = 1
		}
		temp := START_TEMP * math.Exp(logTR*p)
		// 例: 1e-3 * e^(endLambda*p)（現行コードのまま）
		lambda = startLambda * math.Exp(math.Log(endLambda/startLambda)*p)

		// --- 進捗ログ（5%刻み） ---
		logIfNeeded(p, loops, lambda, curEval, bestEval)

		// 近傍：基本は経路上のセルから（以下、元コードそのまま）
		var u int
		if len(curEval.Steps) > 0 {
			u = curEval.Steps[cn.Random.Intn(len(curEval.Steps))].cell
		} else {
			u = cn.Random.Intn(cn.N * cn.N)
		}

		old := curAssign[u]
		prop := old

		cands := fixedCandidates(cn, u)
		if old == -1 {
			if len(cands) == 0 {
				loops++
				continue
			}
			bestc, bestv := -1, -1
			cnt := [4]int{}
			if cs, ok := curEval.visitC[u]; ok {
				for _, x := range cs {
					cnt[x]++
				}
			}
			for _, cc := range cands {
				if cc == int(old) {
					continue
				}
				if cnt[cc] > bestv {
					bestv, bestc = cnt[cc], cc
				}
			}
			if bestc < 0 {
				bestc = cands[cn.Random.Intn(len(cands))]
			}
			prop = int8(bestc)
		} else {
			if cn.Random.Float64() < 0.3 {
				prop = -1
			} else {
				if len(cands) == 0 {
					loops++
					continue
				}
				alt := -1
				for _, cc := range cands {
					if cc != int(old) {
						alt = cc
						break
					}
				}
				if alt < 0 {
					alt = cands[cn.Random.Intn(len(cands))]
				}
				prop = int8(alt)
			}
		}
		if prop == old {
			loops++
			continue
		}

		// まず軽量評価を挟まず、直接厳密評価（現行方針のまま）
		curAssign[u] = prop
		copy(st.assign, curAssign)

		ev := evaluate(cn, st, dist, seen, &stamp, dq, parent)
		score := calcScore(ev, lambda, cn)
		curScore = calcScore(curEval, lambda, cn)
		delta := score - curScore
		accept := (delta <= 0) || (cn.Random.Float64() < math.Exp(-delta/temp))
		if accept {
			curEval, curScore = ev, score
			// lambda が変動しているため、bestScore も再計算
		} else {
			curAssign[u] = old
			copy(st.assign, curAssign)
		}
		bestScore = calcScore(bestEval, lambda, cn)
		if curScore < bestScore {
			bestScore = curScore
			bestEval = ev
			copy(bestAssign, curAssign)
		}
		loops++
	}

	// 最終(100%)の進捗ログを明示（未出力なら）
	logIfNeeded(1.0, loops, lambda, curEval, bestEval)
	// fmt.Fprintln(os.Stderr, "# VISIT_PATTERNS: cell -> patterns per arrival")
	// totalCells := cn.N * cn.N
	// for cell := 0; cell < totalCells; cell++ {
	// 	cs := bestEval.visitC[cell] // []int（到達ごとのパターン）
	// 	ii, jj := cell/cn.N, cell%cn.N
	// 	fmt.Fprintf(os.Stderr, "CELL %4d (%2d,%2d): %v\n", cell, ii, jj, cs)
	// }
	// fmt.Fprintln(os.Stderr, "[VISIT_DUMP] begin")
	// for i := 0; i < cn.N; i++ {
	// 	for j := 0; j < cn.N; j++ {
	// 		id := i*cn.N + j
	// 		pats := bestEval.visitC[id] // 0..3 の配列
	// 		ins := bestEval.visitQ[id]  // 入方向 0..3
	// 		outs := bestEval.visitD[id] // 出方向 0..3
	// 		// pats は数値配列のまま、in/out は UDLR 文字列で表示
	// 		if pats == nil && ins == nil && outs == nil {
	// 			fmt.Fprintf(os.Stderr, "cell=(%d,%d) id=%d pats=[] in=[] out=[]\n", i, j, id)
	// 		} else {
	// 			fmt.Fprintf(os.Stderr, "cell=(%d,%d) id=%d pats=%v in=%s out=%s\n",
	// 				i, j, id, pats, seqToUDLR(ins), seqToUDLR(outs))
	// 		}
	// 	}
	// }
	// fmt.Fprintln(os.Stderr, "[VISIT_DUMP] end")

	// （デバッグ）visitC のダンプはローカルのみ
	if os.Getenv("LOCAL_TEST") == "1" {
		for i := range bestEval.visitC {
			fmt.Fprintln(os.Stderr, i, bestEval.visitC[i])
		}
	}

	// copy(st.assign, bestAssign)*/
	bestAssign := make([]int8, cn.N*cn.N)
	ps := NewPathSolver(cn)
	bPath, loop := ps.Solve(bestAssign)
	bestEval := EvalFromPath(bPath, cn)
	act := buildOutput2(cn, st, bestEval)
	// fmt.Fprintln(os.Stderr, bestEval)
	return act, bestEval.CEst, loop
}

func calcScore(ev EvalInfo, lambda float64, cn *GameConst) float64 {
	if ev.TermCost >= INF {
		return 1e18
	}
	score := float64(ev.CEst)
	if ev.StepLen > cn.T {
		score += float64(ev.StepLen-cn.T) * float64(ev.StepLen-cn.T) * lambda
	}
	return score
}

/********** メトリクス（stderr; local 用） **********/
func emitMetrics(score, loop int) {
	if os.Getenv("LOCAL_TEST") == "1" {
		fmt.Fprintf(os.Stderr, "METRICS score=%d loop=%d\n", score, loop)
	}
}

/********** main **********/
func main() {
	// defer profile.Start(profile.ProfilePath(".")).Stop()
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1<<20), 1<<20)

	seed := time.Now().UnixNano()
	if s := os.Getenv("SEED"); s != "" {
		if v, err := strconv.ParseInt(s, 10, 64); err == nil {
			seed = v
		}
	}
	gameConst := NewGameConst(seed)
	gameConst.SetScanner(scanner)
	GetInputs(gameConst)

	gameState := NewGameState(gameConst)
	solver := NewSolver(TIME_LIMIT)

	act, score, loop := solver.Solve(gameState, gameConst)
	if out := FormatAction(act, gameState, gameConst); out != "" {
		fmt.Println(out)
	}
	emitMetrics(score, loop)
	fmt.Fprintln(os.Stderr, "$loop =", loop)
}

// --- 追加: デバッグ用 UDLR 変換（[]int -> "[U D L R]" 形式） ---
func seqToUDLR(seq []int) string {
	if len(seq) == 0 {
		return "[]"
	}
	s := "["
	for i, d := range seq {
		if i > 0 {
			s += " "
		}
		s += string(dirChar[d]) // 0..3 -> 'U','D','L','R'
	}
	s += "]"
	return s
}

// --- ここから追加 ---

// ターミナル到達のキー点
type KeyPoint struct {
	terminal int // マスの一次元座標 (cell id)
	inDir    int // 入力 q (到達方向)
	outDir   int // 出力 q (出発方向)
}

// パス(EvalInfo)から
//  1. ターミナル(かつ接尾前)への到達のみを抜き出した列 seq
//  2. 各マスの「一番最後に出発した時」の使用パターン finalPattern
//
// を構築する。
// ここで「接尾前」とは、各セルの訪問パターン列 visitC[cell] において
// 末尾の一定パターンが始まる直前まで（= 最後にパターンが切り替わる位置より前）を指す。
func buildKeySeqAndFinalPattern(cn *GameConst, ev EvalInfo) ([]*KeyPoint, []int) {
	// 1) 全訪問が同一パターンのセルを固定扱いにする
	fixedType := make(map[int]int, len(ev.visitC))
	for cell, cs := range ev.visitC {
		if len(cs) == 0 {
			continue
		}
		ok, first := true, cs[0]
		for _, x := range cs {
			if x != first {
				ok = false
				break
			}
		}
		if ok {
			fixedType[cell] = first
		}
	}

	// 2) 接尾が一定となる開始位置 s（= 最後に切替が起きた直後の位置）を計算
	//    固定セルは対象外。s==0 の場合は「最初から一定」なので接尾前は存在しない。
	suffixStart := make(map[int]int, len(ev.visitC))
	for cell, cs := range ev.visitC {
		if _, isFixed := fixedType[cell]; isFixed || len(cs) == 0 {
			continue
		}
		cp := cs[len(cs)-1] // 末尾で一定のパターン
		s := len(cs) - 1    // ここから左に遡って同じなら縮める
		for s > 0 && cs[s-1] == cp {
			s--
		}
		if s > 0 {
			suffixStart[cell] = s // [0..s-1] が接尾「前」
		}
		// s==0 なら接尾前は存在しない（何も登録しない）
	}

	// 3) finalPattern: 各セルの最後の出発パターン（= visitC[cell] の末尾）
	total := cn.N * cn.N
	finalPattern := make([]int, total)
	for i := 0; i < total; i++ {
		finalPattern[i] = -1
	}
	for cell, cs := range ev.visitC {
		if len(cs) > 0 {
			finalPattern[cell] = cs[len(cs)-1]
		}
	}

	// 4) seq 構築：Steps を左から走査し、各セルの訪問 index を数えながら
	//    「非固定 かつ 接尾前（pos < suffixStart[cell]）」のときだけ KeyPoint を追加
	L := len(ev.Steps)
	pos := make(map[int]int, len(ev.visitC)) // cell -> 現在の訪問 index
	seq := make([]*KeyPoint, 0, L)

	for i := 0; i < L; i++ {
		s := ev.Steps[i]
		cell := s.cell
		p := pos[cell]
		pos[cell] = p + 1

		if _, isFixed := fixedType[cell]; isFixed {
			continue // 全訪問が一定 → 端から固定色運用、ターミナル到達列には含めない
		}
		sfx, hasSfx := suffixStart[cell]
		if !hasSfx {
			continue // 接尾前が存在しない（=最初から一定）→ 含めない
		}
		if p >= sfx {
			continue // 接尾区間（固定化対象）→ 含めない
		}

		seq = append(seq, &KeyPoint{
			terminal: cell,
			inDir:    s.qIn,
			outDir:   s.qOut,
		})
	}

	return seq, finalPattern
}

// --- 追加ここまで ---

// ====== bitset（usedの最適化：[]bool → bitset） =====================================

type bitset []uint64

func newBitset(nbits int) bitset {
	words := (nbits + 63) >> 6
	return make([]uint64, words)
}
func (b bitset) get(i int) bool {
	w := i >> 6
	if w >= len(b) || i < 0 {
		return false
	}
	return ((b[w] >> (uint(i) & 63)) & 1) != 0
}
func (b bitset) set(i int) {
	w := i >> 6
	if w >= len(b) || i < 0 {
		return
	}
	b[w] |= 1 << (uint(i) & 63)
}
func (b bitset) rem(i int) {
	w := i >> 6
	if w >= len(b) || i < 0 {
		return
	}
	b[w] &= ^(1 << (uint(i) & 63))
}

// ====== BeamSearchState ==========================================================

// 盤面情報などを扱う構造体
type BeamSearchState struct {
	usedColor int
	maxIndex  int // 情報が入っている最大index（実質「使用済みbitが有効な上限」）

	// ここに盤面の情報を格納する変数
	nextQ     []int
	nextColor []int
	used      bitset // 高速化のため bitset 化

	requiredLane  int   // 次要求レーン
	requiredColor []int // 各マスの次要求色
	numUsedState  int
	numUsedColor  int

	estimatedCounts [4]int
}

// プールで割り当て回数を削減
var statePool = sync.Pool{
	New: func() any { return &BeamSearchState{} },
}

// NewBeamSearchState() 新しい状態を生成する関数（配列を必要容量で再利用）
func NewBeamSearchState(statecap int, gameConst *GameConst) *BeamSearchState {
	bState := statePool.Get().(*BeamSearchState)

	// nextQ / nextColor は statecap に合わせてスライス長を調整（容量不足なら再確保）
	if cap(bState.nextQ) < statecap {
		bState.nextQ = make([]int, statecap)
		bState.nextColor = make([]int, statecap)
	} else {
		bState.nextQ = bState.nextQ[:statecap]
		bState.nextColor = bState.nextColor[:statecap]
	}

	// used bitset は statecap（ビット数）に合わせて words を確保
	words := (statecap + 63) >> 6
	if cap(bState.used) < words {
		bState.used = make(bitset, words)
	} else {
		bState.used = bState.used[:words]
		// クリアは必須でない（maxIndex を常に参照ガードに使う）
		for i := range bState.used {
			bState.used[i] = 0
		}
	}

	// requiredColor は N*N
	rcap := gameConst.N * gameConst.N
	if cap(bState.requiredColor) < rcap {
		bState.requiredColor = make([]int, rcap)
	} else {
		bState.requiredColor = bState.requiredColor[:rcap]
	}

	bState.estimatedCounts = [4]int{}
	return bState
}

func (g *BeamSearchState) Free() {
	statePool.Put(g)
}

func (g *BeamSearchState) Init(gameConst *GameConst, revSeq []*KeyPoint, finalPattern []int) {
	g.usedColor = 0
	g.maxIndex = 0

	g.requiredLane = 0
	for i := range g.requiredColor {
		g.requiredColor[i] = finalPattern[i] - 4
	}
	g.numUsedState = 0
	g.numUsedColor = 0
	g.estimatedCounts = [4]int{}
	for i := range revSeq {
		g.estimatedCounts[revSeq[i].inDir]++
	}
	// used は maxIndex=0 のガードにより参照されないためゼロ化不要だが、
	// 念のため words をクリア済み（New でクリア）。
}

// Copy() Stateをコピーする関数
func (g *BeamSearchState) Copy(gameConst *GameConst) *BeamSearchState {
	newg := NewBeamSearchState(len(g.nextQ), gameConst)
	newg.Load(g)
	return newg
}

// Load() 既存のStateを上書きする関数（copy による高速化）
func (g *BeamSearchState) Load(src *BeamSearchState) {
	g.usedColor = src.usedColor
	g.maxIndex = src.maxIndex

	// 有効範囲のみまとめてコピー
	if n := src.maxIndex; n > 0 {
		for i := 0; i < n; i++ {
			g.nextQ[i] = src.nextQ[i]
			g.nextColor[i] = src.nextColor[i]
		}
		// copy(g.nextQ[:n], src.nextQ[:n])
		// copy(g.nextColor[:n], src.nextColor[:n])

		// used はビット単位だが words 単位でコピー（余分は未参照）
		words := (n + 63) >> 6
		if words > 0 {
			for i := 0; i < words; i++ {
				g.used[i] = src.used[i]
			}
			// copy(g.used[:words], src.used[:words])
			// words 以降は未参照なので放置
		}
	} else {
		// 0 のときは used を空に
		for i := range g.used {
			g.used[i] = 0
		}
	}

	g.requiredLane = src.requiredLane
	copy(g.requiredColor, src.requiredColor)
	g.numUsedState = src.numUsedState
	g.numUsedColor = src.numUsedColor
	g.estimatedCounts = src.estimatedCounts
}

// ====== BeamSearchAction（値型化） =================================================

type BeamSearchAction struct {
	C    int
	Lane int
}

// ====== BeamSearch =================================================================

type BeamSearch struct {
	defaultWidth       int  // ビームサーチの幅
	depth              int  // ビームサーチの深さ(終了までにかかりそうなAction数)
	nowDepth           int  // 現在該当する深さ 計算が完了している時に使用される
	calculated         bool // 計算完了しているか
	effectiveStates    [][]*BeamSearchState
	effectiveItems     [][]Item // 値型に変更
	numEffectiveStates []int

	maxLane int
}

func NewBeamSearch() *BeamSearch {
	return &BeamSearch{}
}

func (bs *BeamSearch) Init(width int, depth int) {
	bs.defaultWidth = width
	bs.depth = depth
	bs.nowDepth = 0
	bs.calculated = false
	bs.effectiveStates = make([][]*BeamSearchState, depth+1)
	bs.effectiveItems = make([][]Item, depth+1)
	bs.numEffectiveStates = make([]int, depth+1)
}

// // Stateに対し評価値を返す関数（必要なら差分化）
// // func (bs *BeamSearch) Evaluate(gameState *BeamSearchState, gameConst *GameConst) float64 {
// // 	v := -float64(gameState.numUsedState) - float64(gameState.numUsedColor)*10
// // 	if gameState.used.get(gameState.usedColor*bs.maxLane*4) {
// // 		v -= 0.5
// // 	}
// // 	return v
// // }

// 次以降の深さでは引き継がれないボーナス点
func (bs *BeamSearch) EvaluateBonus(gameState *BeamSearchState, gameConst *GameConst, action BeamSearchAction, keyPoint *KeyPoint, nextKeyPoint *KeyPoint) float64 {
	v := 0.0
	nextLane := action.Lane
	nextColor := gameState.requiredColor[nextKeyPoint.terminal]
	if keyPoint.terminal == nextKeyPoint.terminal {
		nextColor = action.C
	}
	exist := false
	stride := bs.maxLane << 2
	base := nextKeyPoint.inDir // 固定

	for l := 0; l < bs.maxLane; l++ {
		for c := 0; c < gameState.numUsedColor; c++ {
			index := l*4 + base + c*stride
			if index >= gameState.maxIndex || !gameState.used.get(index) {
				break
			}
			if gameState.nextColor[index] == nextColor && gameState.nextQ[index] == (nextLane<<2)+nextKeyPoint.outDir {
				exist = true
			}
		}
		if exist {
			break
		}
	}
	if exist {
		v += 0.5
	}
	return v
}

func (bs *BeamSearch) EvaluateDiff(gameState *BeamSearchState, gameConst *GameConst, action BeamSearchAction, keyPoint *KeyPoint) float64 {
	v := 0.0
	stride := bs.maxLane << 2
	index := action.C*stride + (action.Lane<<2 | keyPoint.inDir)

	if index < gameState.maxIndex && gameState.used.get(index) {
		// inDir の推定出現回数が他より大きいなら微加点
		in := keyPoint.inDir
		ismax := true
		for i := 0; i < 4; i++ {
			if i == in {
				continue
			}
			if gameState.estimatedCounts[in] <= gameState.estimatedCounts[i] {
				ismax = false
				break
			}
		}
		if ismax {
			v += 0.1
		}
		return v
	}

	// 未使用スロットを使う
	v -= 1
	if action.C >= gameState.numUsedColor {
		if action.C > bs.maxLane*4/3-5 {
			v -= 100
		}
	}
	return v
}

// CheckValid を値型 Action + bitset 版
func (bs *BeamSearch) CheckValid(gameState *BeamSearchState, action BeamSearchAction, keyPoint *KeyPoint, gameConst *GameConst) bool {
	stride := bs.maxLane << 2
	index := action.C*stride + (action.Lane<<2 | keyPoint.inDir)

	if gameState.maxIndex <= index {
		return true
	}
	if !gameState.used.get(index) {
		return true
	}
	nextLane := gameState.requiredLane
	nextQ := (nextLane << 2) | keyPoint.outDir
	if gameState.nextQ[index] != nextQ {
		return false
	}
	nextColor := gameState.requiredColor[keyPoint.terminal]
	if gameState.nextColor[index] != nextColor {
		return false
	}
	return true
}

// 行動を適用する関数（bitset & 事前判定で分岐削減）
func (g *BeamSearchState) ApplyAction(bs *BeamSearch, action BeamSearchAction, keyPoint *KeyPoint) {
	stride := bs.maxLane << 2
	index := action.C*stride + (action.Lane<<2 | keyPoint.inDir)

	nextColor := g.requiredColor[keyPoint.terminal]
	nextLane := g.requiredLane
	nextQ := (nextLane << 2) | keyPoint.outDir

	// maxIndex 更新（未初期領域のクリアは不要：参照時に maxIndex ガード）
	if g.maxIndex < index+1 {
		for i := g.maxIndex; i < index+1; i++ {
			g.used.rem(i)
		}
		g.maxIndex = index + 1
	}

	g.usedColor = maxInt(g.usedColor, action.C+1)
	g.numUsedColor = maxInt(g.numUsedColor, action.C+1)
	g.nextColor[index] = nextColor
	g.nextQ[index] = nextQ
	g.requiredLane = action.Lane
	g.requiredColor[keyPoint.terminal] = action.C

	prevUsed := index < g.maxIndex && g.used.get(index)
	if !prevUsed {
		g.used.set(index)
		g.numUsedState++
	} else {
		g.estimatedCounts[keyPoint.inDir]--
	}
}

// ====== BeamSearchQueue：Top‑K min-heap（randValue基準） =============================

type Item struct {
	prevDepth  int
	prevIndex  int              // 遷移元の順位
	prevAction BeamSearchAction // 遷移元からここに来るために必要なAction
	beamValue  float64          // 状態の評価値
	randValue  float64          // ビーム幅選別用（rand + bonus を加味）
}

// 最小ヒープ（randValue 最小が頂点）で Top-K を維持
type beamMinHeap []Item

func (h beamMinHeap) Len() int           { return len(h) }
func (h beamMinHeap) Less(i, j int) bool { return h[i].randValue < h[j].randValue }
func (h beamMinHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *beamMinHeap) Push(x any)        { *h = append(*h, x.(Item)) }
func (h *beamMinHeap) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

type BeamSearchQueue struct {
	h       beamMinHeap
	width   int
	boarder float64 // 現在の Top‑K の最小 randValue（この値未満は棄却）
}

func NewBeamSearchQueue(width int) *BeamSearchQueue {
	q := BeamSearchQueue{}
	q.h = make([]Item, 0, width)
	q.width = width
	q.boarder = -INF
	return &q
}

func (q *BeamSearchQueue) Init() {
	q.h = q.h[:0]
	heap.Init(&q.h)
	q.boarder = -INF
}

func (q *BeamSearchQueue) Len() int { return len(q.h) }

func (q *BeamSearchQueue) Promising(v float64) bool {
	return q.boarder <= v
}

// Push：満杯なら最小（boarder）より良いもののみ入れ替える
func (q *BeamSearchQueue) Push(item Item) bool {
	if len(q.h) < q.width {
		heap.Push(&q.h, item)
		if len(q.h) == q.width {
			q.boarder = q.h[0].randValue
		}
		return true
	}
	if item.randValue <= q.h[0].randValue {
		return false
	}
	heap.Pop(&q.h)
	heap.Push(&q.h, item)
	q.boarder = q.h[0].randValue
	return true
}

// randValue で Top‑K 済みの候補を、beamValue で最終整列
func (q *BeamSearchQueue) FinalizeByBeam() {
	sort.Slice(q.h, func(i, j int) bool { return q.h[i].beamValue > q.h[j].beamValue })
}

// Finalize 後の Pop（先頭から取り出し）
func (q *BeamSearchQueue) Pop() Item {
	ret := q.h[0]
	q.h = q.h[1:]
	return ret
}

// ====== BeamSearch.Run ==============================================================

func (bs *BeamSearch) Run(gameState *BeamSearchState, gameConst *GameConst, revSeq []*KeyPoint) *BeamSearchState {
	// 既に計算済みならいきなり復元パートに行ってよい
	randFactor := 0.7
	const bonusMax = 0.5 // EvaluateBonus の上界

	if !bs.calculated {
		// 計算
		bsQueues := make([]*BeamSearchQueue, bs.depth+1)
		for i := range bsQueues {
			bsQueues[i] = NewBeamSearchQueue(bs.defaultWidth)
			bsQueues[i].Init()
		}

		for d := 0; d < bs.depth+1; d++ {
			// 受け皿
			bs.effectiveStates[d] = make([]*BeamSearchState, 0, bs.defaultWidth)
			bs.effectiveItems[d] = make([]Item, 0, bs.defaultWidth)

			if d == 0 {
				cGameState := gameState.Copy(gameConst)
				bs.effectiveStates[d] = append(bs.effectiveStates[d], cGameState)
				bs.effectiveItems[d] = append(bs.effectiveItems[d], Item{}) // beamValue=0 でOK
				bs.numEffectiveStates[d] = 1
			} else {
				// 深さ d の候補：randValue で Top‑K 済 → beamValue で整列してから取り出す
				bsQueues[d].FinalizeByBeam()

				index := 0
				for bsQueues[d].Len() > 0 {
					item := bsQueues[d].Pop()

					var nowState *BeamSearchState
					if d >= 2 && index < bs.numEffectiveStates[d-2] {
						nowState = bs.effectiveStates[d-2][index]
						nowState.Load(bs.effectiveStates[item.prevDepth][item.prevIndex])
					} else {
						nowState = bs.effectiveStates[item.prevDepth][item.prevIndex].Copy(gameConst)
					}
					nowState.ApplyAction(bs, item.prevAction, revSeq[d-1])

					bs.effectiveStates[d] = append(bs.effectiveStates[d], nowState)
					bs.effectiveItems[d] = append(bs.effectiveItems[d], item)
					index++
				}
				// EffectiveStatesにいれた候補数を格納
				bs.numEffectiveStates[d] = index
			}

			if bs.numEffectiveStates[d] == 0 {
				// あんまり好ましい状態ではないが、探索の完了している状態 (bs.depthの設定を見直したほうがいいかも?)
				fmt.Fprintln(os.Stderr, "WARN: bs.Depth changed", bs.depth, "->", d)
				bs.depth = d
			}
			if d == bs.depth {
				bs.calculated = true
				break
			}

			// 展開：深さ d の各有力状態から d+1 へ
			stride := bs.maxLane << 2
			for i := 0; i < bs.numEffectiveStates[d]; i++ {
				nowState := bs.effectiveStates[d][i]
				in := revSeq[d].inDir
				finalL := false

				for lane := 0; lane < bs.maxLane; lane++ {
					if finalL {
						break
					}
					base := (lane << 2) | in
					finalc := false

					for c := 0; c <= nowState.numUsedColor; c++ {
						if finalc {
							break
						}
						idx := c*stride + base

						// 最初の未使用に到達したら打ち切り
						if idx >= nowState.maxIndex || !nowState.used.get(idx) {
							finalc = true
							if c == 0 {
								finalL = true
							}
						}

						act := BeamSearchAction{C: c, Lane: lane}
						if !bs.CheckValid(nowState, act, revSeq[d], gameConst) {
							continue
						}

						v := bs.effectiveItems[d][i].beamValue + bs.EvaluateDiff(nowState, gameConst, act, revSeq[d])

						// 上界フィルタ（EvaluateBonus / 乱数の前に早期棄却）
						// randFactor + bonusMax を足しても boarder を超えないならスキップ
						if !bsQueues[d+1].Promising(v + randFactor + bonusMax) {
							continue
						}

						r := gameConst.Random.Float64() * randFactor
						if d == bs.depth-1 {
							// 最終層は使用色の少なさを優先
							v = -float64(maxInt(nowState.numUsedColor, act.C+1))
						}
						if d < len(revSeq)-1 {
							r += bs.EvaluateBonus(nowState, gameConst, act, revSeq[d], revSeq[d+1])
						}
						if !bsQueues[d+1].Promising(v + r) {
							continue
						}

						// キューに積む（randValue で Top‑K を維持）
						item := Item{
							prevDepth:  d,
							prevIndex:  i,
							prevAction: act,
							beamValue:  v,
							randValue:  v + r,
						}
						bsQueues[d+1].Push(item)
					}
				}
			}
		}
	}

	return bs.effectiveStates[bs.depth][0]
}

// ====== ユーティリティ =============================================================

func minInt(a, b int) int {
	if a < b {
		return a
	}
	return b
}
func maxInt(a, b int) int {
	if a > b {
		return a
	}
	return b
}

type CostInfo struct {
	history      [][]int  // 各マスのパターン履歴
	dirhistory   [][]int  // 各マスのinDir履歴
	costs        [][4]int // 各マスのコスト inごと
	lastpatterns []int    // 各マスのpre側固定方向
	nextpatterns []int    // post側固定方向
	isActive     []bool   // 既に複数パターンを抱えている？
	cellCount    int

	fixedCosts [4]int

	totalCost [4]int
}

func (dst *CostInfo) Load(src *CostInfo) {

	dst.totalCost = src.totalCost

	// 1次元スライス群
	dst.costs = dst.costs[:0]
	dst.costs = append(dst.costs, src.costs...)
	dst.lastpatterns = dst.lastpatterns[:0]
	dst.lastpatterns = append(dst.lastpatterns, src.lastpatterns...)
	dst.nextpatterns = dst.nextpatterns[:0]
	dst.nextpatterns = append(dst.nextpatterns, src.nextpatterns...)
	dst.isActive = dst.isActive[:0]
	dst.isActive = append(dst.isActive, src.isActive...)
	dst.cellCount = src.cellCount

	dst.fixedCosts = src.fixedCosts

	// 2次元スライス（history）
	for i := range src.history {
		dst.history[i] = dst.history[i][:0]
		dst.history[i] = append(dst.history[i], src.history[i]...)
	}
	for i := range src.dirhistory {
		dst.dirhistory[i] = dst.dirhistory[i][:0]
		dst.dirhistory[i] = append(dst.dirhistory[i], src.dirhistory[i]...)
	}

}

// Copyは受け手(*CostInfo)の内容を深く複製した新インスタンスを返します。
// 内部では必要容量ちょうどで新規確保し、その後Loadを使ってアロケーション無しで詰め替えます。
func (src *CostInfo) Copy(gameConst *GameConst) *CostInfo {
	dst := NewCostInfo(gameConst)
	dst.Load(src)
	return dst
}

func dirOf(u, v int, gc *GameConst) int {
	for d := 0; d < 4; d++ {
		if gc.nei[u][d] == v {
			return d
		}
	}
	fmt.Fprintf(os.Stderr, "ERROR:invalid move (%d -> %d)\n", u, v)
	return -1
}

func NewCostInfo(gameConst *GameConst) *CostInfo {
	cInfo := &CostInfo{}
	cInfo.history = make([][]int, gameConst.N*gameConst.N)
	for i := range cInfo.history {
		cInfo.history[i] = make([]int, 0)
	}
	cInfo.dirhistory = make([][]int, gameConst.N*gameConst.N)
	for i := range cInfo.dirhistory {
		cInfo.dirhistory[i] = make([]int, 0)
	}
	cInfo.costs = make([][4]int, gameConst.N*gameConst.N)
	cInfo.lastpatterns = make([]int, gameConst.N*gameConst.N)
	cInfo.nextpatterns = make([]int, gameConst.N*gameConst.N)
	cInfo.isActive = make([]bool, gameConst.N*gameConst.N)
	return cInfo
}

func (cInfo *CostInfo) Init(gameConst *GameConst) {
	for i := range cInfo.history {
		cInfo.history[i] = cInfo.history[i][:0]
	}
	for i := range cInfo.dirhistory {
		cInfo.dirhistory[i] = cInfo.dirhistory[i][:0]
	}
	for i := range cInfo.costs {
		cInfo.costs[i] = [4]int{}
	}
	for i := range cInfo.lastpatterns {
		cInfo.lastpatterns[i] = -1
	}
	for i := range cInfo.nextpatterns {
		cInfo.nextpatterns[i] = -1
	}
	for i := range cInfo.isActive {
		cInfo.isActive[i] = true
	}
	cInfo.totalCost = [4]int{}
	cInfo.cellCount = 0
	cInfo.fixedCosts = [4]int{}
}

func (cInfo *CostInfo) calcScore(gameConst *GameConst) int {
	cost := 0
	costs := [4]int{}
	for d := 0; d < 4; d++ {
		cost += cInfo.totalCost[d]
		costs[d] += cInfo.totalCost[d]
	}
	for pos := range cInfo.costs {
		add := false
		if !cInfo.isActive[pos] {
			add = true
		} else if cInfo.nextpatterns[pos] != -1 {
			if cInfo.nextpatterns[pos] != cInfo.lastpatterns[pos] {
				add = true
			}
		}
		if add {
			for d := 0; d < 4; d++ {
				cost += cInfo.costs[pos][d]
				costs[d] += cInfo.costs[pos][d]
			}
		}
	}
	for d := 0; d < 4; d++ {
		cost += cInfo.fixedCosts[d]
		costs[d] += cInfo.fixedCosts[d]
	}
	cmax := 0
	cmin := 100000
	for i := range costs {
		cmax = maxInt(cmax, costs[i])
		cmin = minInt(cmin, costs[i])
	}
	// cost = cmax + cost/3
	cost = cmax*1 - cmin*0 + cost
	return cost
}

func (cInfo *CostInfo) getCostOfEdge(pos, inDir, outDir int, bestAssign []int8) int {
	pat := patternFrom(inDir, outDir)
	cost := 0
	if cInfo.nextpatterns[pos] == -1 {
		// 以降がない
		if cInfo.lastpatterns[pos] != pat {
			for d := 0; d < 4; d++ {
				cost += cInfo.costs[pos][d]
			}
		}
	} else if cInfo.isActive[pos] {
		if pat != cInfo.nextpatterns[pos] {
			cost++
			if cInfo.nextpatterns[pos] == cInfo.lastpatterns[pos] {
				for d := 0; d < 4; d++ {
					cost += cInfo.costs[pos][d]
				}
			}
		}
	} else {
		cost++
	}
	return cost
}
func (cInfo *CostInfo) FixByPath(prev int, path []int, next int, gameConst *GameConst) {
	for i := len(path) - 1; i >= 0; i-- {
		p := 0
		if i > 0 {
			p = path[i-1]
		} else {
			if prev == -1 {
				continue
			}
			p = prev
		}
		c := path[i]
		n := next
		if i < len(path)-1 {
			n = path[i+1]
		} else {
			if next == -1 {
				continue
			}
		}
		inDir := 0
		if prev != -2 {
			inDir = dirOf(p, c, gameConst)
		}
		outDir := dirOf(c, n, gameConst)
		cInfo.Fix(c, inDir, outDir)
	}
}

func (cInfo *CostInfo) Fix(pos int, inDir int, outDir int) {
	pat := patternFrom(inDir, outDir)
	if cInfo.nextpatterns[pos] != pat {
		// fmt.Fprintf(os.Stderr, "(%d) %d -> %d\n", pos, cInfo.nextpatterns[pos], pat)
		if cInfo.nextpatterns[pos] == -1 {
			cInfo.nextpatterns[pos] = pat
		} else {
			// fmt.Fprintf(os.Stderr, "%d deactivated\n", cInfo.nextpatterns[pos])
			cInfo.isActive[pos] = false
		}
	}
	if !cInfo.isActive[pos] {
		cInfo.fixedCosts[inDir]++
		// cInfo.totalCost++
	}
	cInfo.cellCount++
}
func (cInfo *CostInfo) DecrementByPath(prev int, path []int, next int, gameConst *GameConst) {
	for i := len(path) - 1; i >= 0; i-- {
		p := 0
		if i > 0 {
			p = path[i-1]
		} else {
			if prev == -1 {
				continue
			}
			p = prev
		}
		c := path[i]
		n := next
		if i < len(path)-1 {
			n = path[i+1]
		} else {
			if next == -1 {
				continue
			}
		}
		inDir := 0
		if p != -2 {
			inDir = dirOf(p, c, gameConst)
		}
		outDir := dirOf(c, n, gameConst)
		cInfo.Decrement(c, inDir, outDir)
	}
}

func (cInfo *CostInfo) Decrement(pos int, inDir int, outDir int) {
	cInfo.costs[pos][inDir]--
	cInfo.history[pos] = cInfo.history[pos][:len(cInfo.history[pos])-1]
	cInfo.dirhistory[pos] = cInfo.dirhistory[pos][:len(cInfo.dirhistory[pos])-1]
	turningPoint := true
	for i := range cInfo.costs[pos] {
		if cInfo.costs[pos][i] != 0 {
			turningPoint = false
		}
	}
	if turningPoint {
		lastp := -1
		cost := [4]int{}
		for i := len(cInfo.history[pos]) - 1; i >= 0; i-- {
			if lastp != cInfo.history[pos][i] {
				if lastp == -1 {
					lastp = cInfo.history[pos][i]
				} else {
					break
				}
			}
			ind := cInfo.dirhistory[pos][i]
			cost[ind]++
		}
		cInfo.costs[pos] = cost
		cInfo.lastpatterns[pos] = lastp
		for d := 0; d < 4; d++ {
			cInfo.totalCost[d] -= cost[d]
		}
	}
	cInfo.cellCount--
}

func (cInfo *CostInfo) Increment(pos int, inDir int, outDir int) {
	pat := patternFrom(inDir, outDir)
	if cInfo.lastpatterns[pos] != pat {
		for d := 0; d < 4; d++ {
			cInfo.totalCost[d] += cInfo.costs[pos][d]
		}
		cInfo.lastpatterns[pos] = pat
		cInfo.costs[pos] = [4]int{}
	}
	cInfo.costs[pos][inDir]++
	cInfo.history[pos] = append(cInfo.history[pos], pat)
	cInfo.dirhistory[pos] = append(cInfo.dirhistory[pos], inDir)
	cInfo.cellCount++
}
func (cInfo *CostInfo) IncrementSingle(p, c, n int, gameConst *GameConst) {
	inDir := 0
	if p != -2 {
		inDir = dirOf(p, c, gameConst)
	}

	outDir := dirOf(c, n, gameConst)
	cInfo.Increment(c, inDir, outDir)
}
func (cInfo *CostInfo) IncrementByPath(prev int, path []int, next int, gameConst *GameConst) {
	for i := 0; i < len(path); i++ {
		p := 0
		if i > 0 {
			p = path[i-1]
		} else {
			if prev == -1 {
				continue
			}
			p = prev
		}
		c := path[i]
		n := next
		if i < len(path)-1 {
			n = path[i+1]
		} else {
			if next == -1 {
				continue
			}
		}
		cInfo.IncrementSingle(p, c, n, gameConst)
	}

}
func (cInfo *CostInfo) SetInitialInfo(gameConst *GameConst, paths [][]int) {
	cInfo.Init(gameConst)
	for i := range paths {
		prev := -2
		if i > 0 {
			prev = paths[i-1][len(paths[i-1])-2]
		}
		cInfo.IncrementByPath(prev, paths[i], -1, gameConst)
	}

}

// --- PathSolver ------------------------------------------------

type PathSolver struct {
	gc    *GameConst
	paths [][]int // cellID の列
	cInfo *CostInfo

	rng  *rand.Rand
	N    int
	T    int
	oppo [4]int // 反対方向
}

// NewPathSolver : コンストラクタ
func NewPathSolver(gc *GameConst) *PathSolver {
	ps := &PathSolver{
		gc:  gc,
		N:   gc.N,
		T:   gc.T,
		rng: gc.Random,
	}
	ps.oppo = [4]int{DirD, DirU, DirR, DirL}
	return ps
}

// Solve : 最終パスを返す（cellID の列）
func (ps *PathSolver) Solve(bestAssign []int8) ([]int, int) {
	// 1) まずは各目的地を順に繋ぐ最短路（壁のみを考慮）で初期化
	ps.paths = ps.buildInitialPath()
	ps.cInfo = NewCostInfo(ps.gc)
	ps.cInfo.Init(ps.gc)
	ps.cInfo.SetInitialInfo(ps.gc, ps.paths)
	bestPaths := make([][]int, len(ps.paths))
	for i := range bestPaths {
		bestPaths[i] = make([]int, 0, len(ps.paths[i]))
		bestPaths[i] = append(bestPaths[i], ps.paths[i]...)
	}
	// bestCost := float64(ps.exactCost(ps.paths))
	bestCost := float64(ps.cInfo.calcScore(ps.gc))
	nowCost := bestCost

	timeLimit := 1000 * time.Millisecond
	startAt := time.Now()

	// 温度パラメータ（簡易）
	T0 := 400.0
	Tend := 400.0
	logTR := math.Log(Tend / T0) // 温度用（START_TEMP==END_TEMPなら0）

	// fmt.Fprintln(os.Stderr, bestPaths)
	loop := 0
	goalIndex := 0
	newcInfo := ps.cInfo.Copy(ps.gc)
	for iter := 0; time.Since(startAt) < timeLimit; iter++ {
		loop++
		// if loop > ps.gc.N*ps.gc.N*5 {
		// 	break
		// }
		goalIndex--
		if goalIndex < 0 {
			goalIndex += len(ps.paths)
			ps.cInfo.SetInitialInfo(ps.gc, ps.paths)
		}
		// if iter > 1 {
		// 	break
		// }
		prev := -2
		next := -1
		if goalIndex < len(ps.paths)-1 {
			next = ps.paths[goalIndex+1][1]
		}
		if goalIndex > 0 {
			prev = ps.paths[goalIndex-1][len(ps.paths[goalIndex-1])-2]
		}
		// fmt.Fprintln(os.Stderr, ps.paths[goalIndex])
		ps.cInfo.DecrementByPath(prev, ps.paths[goalIndex], next, ps.gc)
		newcInfo.Load(ps.cInfo)
		// make path
		startInDir := -1
		goalOutDir := -1
		if goalIndex < len(ps.paths)-1 {
			goalOutDir = dirBetween(ps.paths[goalIndex+1][0], ps.paths[goalIndex+1][1], ps.gc)
		}
		if goalIndex > 0 {
			startInDir = dirBetween(ps.paths[goalIndex-1][len(ps.paths[goalIndex-1])-2], ps.paths[goalIndex-1][len(ps.paths[goalIndex-1])-1], ps.gc)
		}
		newPath := FindShortestPathWithInDir(ps.paths[goalIndex][0], startInDir, ps.paths[goalIndex][len(ps.paths[goalIndex])-1], goalOutDir, ps.gc.nei, ps.cInfo, ps.gc.Random, bestAssign)
		// calc delta cost
		newcInfo.IncrementSingle(prev, newPath[0], newPath[1], ps.gc)
		newcInfo.FixByPath(-1, newPath, next, ps.gc)
		newCost := float64(newcInfo.calcScore(ps.gc))
		if newcInfo.cellCount > ps.gc.T {
			newCost += 10000
		}
		delta := newCost - nowCost
		if iter%1000 == 0 {
			fmt.Fprintln(os.Stderr, iter, newcInfo.calcScore(ps.gc), nowCost)
		}
		now := time.Now()
		p := float64(now.Sub(startAt)) / float64(timeLimit)
		if p < 0 {
			p = 0
		} else if p > 1 {
			p = 1
		}
		temp := START_TEMP * math.Exp(logTR*p)
		accept := (delta <= 0) || (ps.gc.Random.Float64() < math.Exp(-delta/temp))

		// accept := false
		// p := math.Exp(-delta / maxFloat(1e-6, Tcur))
		// if ps.rng.Float64() < p {
		// 	accept = true
		// }
		if accept {
			ps.paths[goalIndex] = newPath
			ps.cInfo.Load(newcInfo)
			nowCost = newCost
			if newCost < bestCost {
				bestCost = newCost
				bestPaths = make([][]int, len(ps.paths))
				for i := range bestPaths {
					bestPaths[i] = make([]int, 0, len(ps.paths[i]))
					bestPaths[i] = append(bestPaths[i], ps.paths[i]...)
				}
			}
		} else {
			// 差分を更新
			ps.cInfo.IncrementSingle(prev, ps.paths[goalIndex][0], ps.paths[goalIndex][1], ps.gc)
			ps.cInfo.FixByPath(-1, ps.paths[goalIndex], next, ps.gc)
		}
		// 温度更新
	}
	// bestPaths = ps.paths
	// ps.cInfo.SetInitialInfo(ps.gc, bestPaths)
	// fmt.Fprintln(os.Stderr, bestCost, "bestCInfo:", ps.cInfo.calcScore(ps.gc))
	bestPath := make([]int, 0, ps.gc.T)
	for i := range bestPaths {
		fmt.Fprintln(os.Stderr, i, bestPaths[i])
		start := 1
		if i == 0 {
			start = 0
		}
		bestPath = append(bestPath, bestPaths[i][start:]...)
	}

	return bestPath, loop
}

// --- 初期パス（各目的地を BFS で順に接続） ---------------------

func (ps *PathSolver) buildInitialPath() [][]int {
	ids := make([]int, ps.gc.K)
	for i := 0; i < ps.gc.K; i++ {
		ids[i] = ps.idOf(ps.gc.tg[i])
	}
	paths := make([][]int, 0, len(ids)-1)
	for i := 0; i+1 < len(ids); i++ {
		seg := ps.bfs(ids[i], ids[i+1])
		paths = append(paths, seg)
	}
	return paths
}

func (ps *PathSolver) idOf(p Point) int { return p.i*ps.N + p.j }

func (ps *PathSolver) bfs(s, t int) []int {
	if s == t {
		return []int{s}
	}
	nn := ps.N * ps.N
	prev := make([]int, nn)
	for i := range prev {
		prev[i] = -2
	}
	q := make([]int, 0, nn)
	head := 0
	q = append(q, s)
	prev[s] = -1
	for head < len(q) {
		u := q[head]
		head++
		for d := 0; d < 4; d++ {
			v := ps.gc.nei[u][d]
			if v < 0 {
				continue
			}
			if prev[v] != -2 {
				continue
			}
			prev[v] = u
			if v == t {
				// 復元
				path := []int{t}
				for cur := t; prev[cur] != -1; cur = prev[cur] {
					path = append(path, prev[cur])
				}
				reverseInts(path)
				return path
			}
			q = append(q, v)
		}
	}
	return nil
}

// --- コスト（正確） -------------------------------------------

// exactCost : 全セルについてパターン列の「最後の切替位置」を合計
func (ps *PathSolver) exactCost(paths [][]int) int {
	// ステップ中心の配列
	cellOf := make([]int, 0, ps.gc.K) // t=1..L-1 で有効
	patOf := make([]int, 0, ps.gc.K)  // パターン
	for goalIndex := 0; goalIndex < ps.gc.K; goalIndex++ {
		for t := 0; t < len(paths[goalIndex])-1; t++ {
			u := 0
			if t == 0 {
				if goalIndex == 0 {
					u = 0
				} else {
					u = paths[goalIndex][len(paths[goalIndex])-2]
				}
			}
			c := paths[goalIndex][t]
			v := paths[goalIndex][t+1]
			din := ps.dirOf(u, c)
			dout := ps.dirOf(c, v)
			cellOf = append(cellOf, c)
			patOf = append(patOf, ps.patternOf(din, dout))

		}
	}
	L := len(cellOf) - 1

	// cell -> パターン列
	m := make(map[int][]int, L)
	for t := range cellOf {
		c := cellOf[t]
		m[c] = append(m[c], patOf[t])
	}
	total := 0
	for _, seq := range m {
		total += lastChangeIndex(seq)
	}
	return total
}

func lastChangeIndex(seq []int) int {
	k := len(seq)
	if k <= 1 {
		return 0
	}
	last := 0
	for i := 1; i < k; i++ {
		if seq[i] != seq[i-1] {
			last = i + 1 // 1-indexed
		}
	}
	return last
}

// --- パターン --------------------------------------------------

// patternOf : (din,dout) -> {0:反射,1:直進,2:カーブ1,3:カーブ2}
func (ps *PathSolver) patternOf(din, dout int) int {
	if dout == ps.oppo[din] {
		return 0 // 反射
	}
	if dout == din {
		return 1 // 直進
	}
	// カーブ1: U<->L, D<->R
	if (din == DirU && dout == DirL) || (din == DirL && dout == DirU) ||
		(din == DirD && dout == DirR) || (din == DirR && dout == DirD) {
		return 2
	}
	// カーブ2: U<->R, D<->L
	return 3
}

func (ps *PathSolver) dirOf(u, v int) int {
	for d := 0; d < 4; d++ {
		if ps.gc.nei[u][d] == v {
			return d
		}
	}
	return -1
}

func reverseInts(a []int) {
	for i, j := 0, len(a)-1; i < j; i, j = i+1, j-1 {
		a[i], a[j] = a[j], a[i]
	}
}
func maxFloat(a, b float64) float64 {
	if a > b {
		return a
	}
	return b
}

// dirBetween はセル a -> b の移動方向 (U/D/L/R = 0..3) を返す。
// 隣接していない/同一セルなら -1。
func dirBetween(a, b int, cn *GameConst) int {
	nei := cn.nei[a]
	for d := 0; d < 4; d++ {
		if nei[d] == b {
			return d
		}
	}
	return -1
}

// EvalFromPath は、目的地列をすべて通過する「セルIDのみ」の通しパスから EvalInfo を構築する。
// ・最初のステップの qIn は 0（問題仕様に合わせる）
// ・各ステップの qOut は path[k] -> path[k+1] の方向
// ・各セルの訪問ごとのパターン cp = patternFrom(qIn, qOut) を集計
// ・コストは「各セルで末尾パターンが連続し始める直前まで」の固有色数の総和 + 末尾パターンの種類数
// ・不正なパス（非隣接/同一セル連続）が含まれていれば TermCost=INF を返す
func EvalFromPath(path []int, cn *GameConst) EvalInfo {
	L := len(path)
	if L < 2 {
		return EvalInfo{
			TermCost: 0,
			Steps:    nil,
			StepLen:  0,
			CEst:     0,
			visitC:   map[int][]int{},
			visitQ:   map[int][]int{},
			visitD:   map[int][]int{},
		}
	}

	// 方向列を作る（各エッジごとの移動方向 qOut）
	dirs := make([]int, L-1)
	for k := 0; k+1 < L; k++ {
		d := dirBetween(path[k], path[k+1], cn)
		if d < 0 {
			// 不正パス
			fmt.Fprintln(os.Stderr, k, path[k], path[k+1])
			return EvalInfo{TermCost: INF}
		}
		dirs[k] = d
	}

	steps := make([]Step, 0, L-1)
	visitC := make(map[int][]int, cn.N*cn.N/8)
	visitQ := make(map[int][]int, cn.N*cn.N/8)
	visitD := make(map[int][]int, cn.N*cn.N/8)

	// 最初の qIn は 0（開始直後の状態q=0）
	prevDir := 0
	for k := 0; k+1 < L; k++ {
		cell := path[k]
		qIn := prevDir
		qOut := dirs[k]

		steps = append(steps, Step{cell: cell, qIn: qIn, qOut: qOut})

		cp := patternFrom(qIn, qOut)
		visitC[cell] = append(visitC[cell], cp)
		visitQ[cell] = append(visitQ[cell], qIn)
		visitD[cell] = append(visitD[cell], qOut)

		prevDir = qOut
	}

	// 接尾固定前提の色コスト計算：
	// 各セルで末尾パターン cp が連続し始める位置 s を求め、s 回分が固有色。
	// 固定パターン色は末尾パターンの種類数。
	patSet := map[int]bool{}
	termColorCost := 0
	for cell, cs := range visitC {
		_ = cell
		if len(cs) == 0 {
			continue
		}
		cp := cs[len(cs)-1]
		s := len(cs) - 1
		for s > 0 && cs[s-1] == cp {
			s--
		}
		patSet[cp] = true
		termColorCost += s
	}
	cEst := len(patSet) + termColorCost

	return EvalInfo{
		TermCost: termColorCost, // このパスに基づく固有色必要数（接尾直前まで）
		Steps:    steps,
		StepLen:  L - 1,
		CEst:     cEst,
		visitC:   visitC,
		visitQ:   visitQ,
		visitD:   visitD,
	}
}

// --- 4-ary indexed heap ---
type dHeap4 struct {
	h    []int
	pos  []int
	dist []float64
}

func newDHeap4(total int, dist []float64) *dHeap4 {
	return &dHeap4{
		h:    make([]int, 0, total/4+16),
		pos:  make([]int, total),
		dist: dist,
	}
}
func (q *dHeap4) Len() int           { return len(q.h) }
func (q *dHeap4) less(i, j int) bool { return q.dist[q.h[i]] < q.dist[q.h[j]] }
func (q *dHeap4) swap(i, j int) {
	q.h[i], q.h[j] = q.h[j], q.h[i]
	q.pos[q.h[i]] = i + 1
	q.pos[q.h[j]] = j + 1
}
func (q *dHeap4) up(i int) {
	for i > 0 {
		p := (i - 1) >> 2
		if !q.less(i, p) {
			break
		}
		q.swap(i, p)
		i = p
	}
}
func (q *dHeap4) down(i int) {
	n := len(q.h)
	for {
		c := (i << 2) + 1
		if c >= n {
			break
		}
		m := c
		if c+1 < n && q.less(c+1, m) {
			m = c + 1
		}
		if c+2 < n && q.less(c+2, m) {
			m = c + 2
		}
		if c+3 < n && q.less(c+3, m) {
			m = c + 3
		}
		if !q.less(m, i) {
			break
		}
		q.swap(i, m)
		i = m
	}
}
func (q *dHeap4) InHeap(s int) bool { return q.pos[s] != 0 }
func (q *dHeap4) Push(s int) {
	q.h = append(q.h, s)
	i := len(q.h) - 1
	q.pos[s] = i + 1
	q.up(i)
}
func (q *dHeap4) Pop() int {
	n := len(q.h) - 1
	top := q.h[0]
	last := q.h[n]
	q.h = q.h[:n]
	q.pos[top] = 0
	if n > 0 {
		q.h[0] = last
		q.pos[last] = 1
		q.down(0)
	}
	return top
}
func (q *dHeap4) DecreaseKey(s int) {
	if i := q.pos[s] - 1; i >= 0 {
		q.up(i)
	}
}

// --- 本体（差分） ---
func FindShortestPathWithInDir(
	start, startInDir, goal, goalOutDir int,
	nei [][]int,
	cInfo *CostInfo,
	rng *rand.Rand,
	bestAssign []int8,
) []int {
	randFactor := rng.Float64() * 2
	randFactor *= randFactor
	randFactor *= randFactor

	n := len(nei)
	if n == 0 {
		return nil
	}

	const D = 4
	total := n * D
	INF := math.Inf(1)

	// state = (cell<<2)|dir
	dist := make([]float64, total)
	prev := make([]int, total)
	for i := 0; i < total; i++ {
		dist[i] = INF
		prev[i] = -1
	}

	// 4-ary ヒープ
	pq := newDHeap4(total, dist)

	pushStart := func(s int) {
		dist[s] = 0
		pq.Push(s)
	}
	if startInDir >= 0 && startInDir < D {
		pushStart((start << 2) | startInDir)
	} else {
		base := start << 2
		pushStart(base | 0)
		pushStart(base | 1)
		pushStart(base | 2)
		pushStart(base | 3)
	}

	target := -1
	seed := rng.Uint64() // ノイズ用 1 回だけ取得

	for pq.Len() > 0 {
		curState := pq.Pop()
		cell := curState >> 2
		inDir := curState & 3

		if cell == goal {
			target = curState
			break
		}

		neiCell := nei[cell]
		_ = neiCell[3] // BCE ヒント

		// 4 方向を手動アンローリング
		// relax をローカル関数にするとインライン化されにくいので直書き
		if nextCell := neiCell[0]; nextCell >= 0 {
			nextState := (nextCell << 2) | 0
			cost := cInfo.getCostOfEdge(cell, inDir, 0, bestAssign) * 10
			v := seed ^ uint64(nextState)*0x9E3779B185EBCA87
			v ^= v >> 30
			v *= 0xBF58476D1CE4E5B9
			v ^= v >> 27
			v *= 0x94D049BB133111EB
			v ^= v >> 31
			noise := float64(v&1023) * (randFactor / 1024.0)
			if nd := dist[curState] + float64(cost) + noise; nd < dist[nextState] {
				dist[nextState] = nd
				prev[nextState] = curState
				if pq.InHeap(nextState) {
					pq.DecreaseKey(nextState)
				} else {
					pq.Push(nextState)
				}
			}
		}
		if nextCell := neiCell[1]; nextCell >= 0 {
			nextState := (nextCell << 2) | 1
			cost := cInfo.getCostOfEdge(cell, inDir, 1, bestAssign) * 10
			v := seed ^ uint64(nextState)*0x9E3779B185EBCA87
			v ^= v >> 30
			v *= 0xBF58476D1CE4E5B9
			v ^= v >> 27
			v *= 0x94D049BB133111EB
			v ^= v >> 31
			noise := float64(v&1023) * (randFactor / 1024.0)
			if nd := dist[curState] + float64(cost) + noise; nd < dist[nextState] {
				dist[nextState] = nd
				prev[nextState] = curState
				if pq.InHeap(nextState) {
					pq.DecreaseKey(nextState)
				} else {
					pq.Push(nextState)
				}
			}
		}
		if nextCell := neiCell[2]; nextCell >= 0 {
			nextState := (nextCell << 2) | 2
			cost := cInfo.getCostOfEdge(cell, inDir, 2, bestAssign) * 10
			v := seed ^ uint64(nextState)*0x9E3779B185EBCA87
			v ^= v >> 30
			v *= 0xBF58476D1CE4E5B9
			v ^= v >> 27
			v *= 0x94D049BB133111EB
			v ^= v >> 31
			noise := float64(v&1023) * (randFactor / 1024.0)
			if nd := dist[curState] + float64(cost) + noise; nd < dist[nextState] {
				dist[nextState] = nd
				prev[nextState] = curState
				if pq.InHeap(nextState) {
					pq.DecreaseKey(nextState)
				} else {
					pq.Push(nextState)
				}
			}
		}
		if nextCell := neiCell[3]; nextCell >= 0 {
			nextState := (nextCell << 2) | 3
			cost := cInfo.getCostOfEdge(cell, inDir, 3, bestAssign) * 10
			v := seed ^ uint64(nextState)*0x9E3779B185EBCA87
			v ^= v >> 30
			v *= 0xBF58476D1CE4E5B9
			v ^= v >> 27
			v *= 0x94D049BB133111EB
			v ^= v >> 31
			noise := float64(v&1023) * (randFactor / 1024.0)
			if nd := dist[curState] + float64(cost) + noise; nd < dist[nextState] {
				dist[nextState] = nd
				prev[nextState] = curState
				if pq.InHeap(nextState) {
					pq.DecreaseKey(nextState)
				} else {
					pq.Push(nextState)
				}
			}
		}
	}

	if target == -1 {
		return nil
	}

	// 経路復元
	var path []int
	for s := target; s != -1; s = prev[s] {
		path = append(path, s>>2)
	}
	for i, j := 0, len(path)-1; i < j; i, j = i+1, j-1 {
		path[i], path[j] = path[j], path[i]
	}
	return path
}
