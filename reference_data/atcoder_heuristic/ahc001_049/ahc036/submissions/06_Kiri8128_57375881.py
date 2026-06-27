from collections import deque
from random import random, randrange, seed as randomseed
from math import exp, log
from time import time
import datetime
from collections import defaultdict

DEBUG = 0


class TimeManager():
    def __init__(self, initial_type_name="Initial"):
        now = time()
        self.start_time = now
        self.checkpoint_time = now
        self.type_time = now
        self.D = defaultdict(int)
        self.checklist = []
        self.type_name = initial_type_name
        self.set_type(initial_type_name)
        self.disp_format = "{:.4f}"

    def get_type(self):
        return self.type_name

    def set_type(self, new_type_name):
        now = time()
        self.D[self.type_name] += now - self.type_time
        self.type_time = now
        self.type_name = new_type_name

    def checkpoint(self, comment):
        now = time()
        self.checklist.append((now - self.checkpoint_time, now - self.start_time, comment))
        self.checkpoint_time = now

    def elapsed_time(self):
        return time() - self.start_time

    def elapsed_time_from_checkpoint(self):
        return time() - self.checkpoint_time

    def current_time(self):
        return datetime.datetime.now()

    def disp(self):
        now = time()
        self.D[self.type_name] += now - self.type_time
        self.type_time = now

        print("#", "-" * 5, "Time Manager", "-" * 5)
        total_time = 0
        for checkpoint_time, total_time, comment in self.checklist:
            print("#", self.disp_format.format(checkpoint_time), self.disp_format.format(total_time), comment)
        print("#", self.disp_format.format(now - self.start_time - total_time),
              self.disp_format.format(now - self.start_time), "Disp")
        print("#", "-" * 20)
        t = 0
        for key, value in self.D.items():
            print("#", key, self.disp_format.format(value))
            t += value
        assert abs(t - (now - self.start_time)) < 0.001
        print("#", "Total =", self.disp_format.format(t))
        print("#", "-" * 20)


class UnionFind():
    def __init__(self, n):
        self.n = n
        self.PA = [-1] * n

    def root(self, a):
        L = []
        while self.PA[a] >= 0:
            L.append(a)
            a = self.PA[a]
        for l in L:
            self.PA[l] = a
        return a

    def unite(self, a, b):
        ra, rb = self.root(a), self.root(b)
        if ra == rb:
            return 0
        if self.PA[rb] >= self.PA[ra]:
            self.PA[ra] += self.PA[rb]
            self.PA[rb] = ra
        else:
            self.PA[rb] += self.PA[ra]
            self.PA[ra] = rb
        return 1

    def same(self, a, b):
        return 1 if self.root(a) == self.root(b) else 0

    def size(self, a):
        return -self.PA[self.root(a)]

    def groups(self):
        G = [[] for _ in range(self.n)]
        for i in range(self.n):
            G[self.root(i)].append(i)
        return [g for g in G if g]

    def groups_index(self):
        G = [[] for _ in range(self.n)]
        for i in range(self.n):
            G[self.root(i)].append(i)
        cnt = 0
        GG = []
        I = [-1] * self.n
        for i in range(self.n):
            if G[i]:
                GG.append(G[i])
                I[i] = cnt
                cnt += 1
        for i in range(self.n):
            I[i] = I[self.root(i)]
        return GG, I

    def group_size(self):
        G = [[] for _ in range(self.n)]
        for i in range(self.n):
            G[self.root(i)].append(i)
        return [len(g) for g in G if g]


def BFS(n, E, i0=0):
    SUPER_EXIT()
    D = [-1] * n
    if type(i0) == int:
        Q = deque([i0])
        D[i0] = 0
        remaining_count = n - 1
        while Q:
            x = Q.popleft()
            for c in E[x]:
                if D[c] == -1:
                    D[c] = D[x] + 1
                    Q.append(c)
                    remaining_count -= 1
                    if not remaining_count:
                        return D
        assert 0
    elif type(i0) == list:
        Q = deque(i0[:])
        for i in i0:
            D[i] = 0
        while Q:
            x = Q.popleft()
            for c in E[x]:
                if D[c] == -1:
                    D[c] = D[x] + 1
                    Q.append(c)
        return D
    else:
        assert 0

def calc_best_center():
    best = 10 ** 9
    for i in range(N):
        if SU[i] < best:
            best = SU[i]
            best_i = i
    return best_i

def merge_paths(paths):
    def calc_size(paths):
        i = len(paths) - 1
        s = 0
        while i >= 0:
            if i >= 1 and paths[i][0] == paths[i - 1][0]:
                s += len(paths[i]) + len(paths[i - 1]) - 1
                i -= 2
            else:
                s += len(paths[i])
                i -= 1
        return s

    paths.sort()
    while calc_size(paths) > la:
        SUPER_EXIT()
        best = 10 ** 9
        best_i = None
        best_r1 = None
        for r1 in range(len(paths) - 1):
            r2 = r1 + 1
            com = 0
            i = 0
            while i < len(paths[r1]) and i < len(paths[r2]) and paths[r1][i] == paths[r2][i]:
                i += 1
            if not i:
                continue
            a = len(paths[r1]) - i
            b = len(paths[r2]) - i
            aa = sum(target_count[j] for j in paths[r1][i:])
            bb = sum(target_count[j] for j in paths[r2][i:])
            cost1 = min(a, lb * 3 // 4) * bb / i
            cost2 = min(b, lb * 3 // 4) * aa / i
            cost = min(cost1, cost2)
            if cost < best:
                best = cost
                best_i = i
                best_r1 = r1
                # tp = 0 if a * bb < aa * b else 1
                tp = 0 if cost1 < cost2 else 1
                # tp = 0 if a < b else 1
        if DEBUG:
            print("best_r1, best_i", best_r1, best_i, len(paths))
        r1, r2, i = best_r1, best_r1 + 1, best_i
        if DEBUG:
            print("best", best, r1, r2, i)
        if tp == 0:
            if DD[paths[r1][-1]][paths[r2][i]] == 1:
                paths[r1] += paths[r2][i:]
            elif DD[paths[r1][-1]][paths[r2][-1]] == 1:
                paths[r1] += paths[r2][i:][::-1]
            else:
                paths[r1] += paths[r2][i:]
        else:
            if DD[paths[r2][-1]][paths[r1][i]] == 1:
                paths[r1] = paths[r2] + paths[r1][i:]
            elif DD[paths[r2][-1]][paths[r1][-1]] == 1:
                paths[r1] = paths[r2] + paths[r1][i:][::-1]
            else:
                paths[r1] = paths[r2] + paths[r1][i:]
        for b in paths[r2]:
            assert b in paths[r1]
        # paths[r2:] = paths[r2+1:]
        del paths[r2]
    return paths

def calc_pairs_1st(paths):
    def calc_connect_benefit(i, j):
        def left(k):
            return paths[i][~k]

        def right(k):
            return paths[j][~k]

        if i == j:
            return 0
        for k in range(lb // 2 + 1):
            for k1 in range(k + 1):
                if k1 >= len(paths[i]):
                    break
                k2 = k - k1
                if k2 >= len(paths[j]):
                    continue
                if DD[left(k1)][right(k2)] <= 1:
                    return max(0, lb - k) * 100 / lb * (1 + abs((len(paths[i]) - k1) - (len(paths[j]) - k2)) / 5)
        return 0
    nn = len(paths)
    connect_benefit_matrix = [[0] * nn for _ in range(nn)]
    for i in range(nn):
        for j in range(nn):
            cb = calc_connect_benefit(i, j)
            connect_benefit_matrix[i][j] = cb
    
    if 0:
        print("#", "connect_benefit_matrix")
        for aa in connect_benefit_matrix:
            print("#", aa)
    
    pairs = [k for k in range(-1, nn)]
    if nn % 2 == 0:
        pairs.append(-1)
    
    temperature_start = 200
    temperature_end = 10
    sa_loop_cnt = 2 * 10 ** 5
    best_acc_score = -10 ** 9
    best_pairs = None
    acc_score = 0
    for sa_loop in range(sa_loop_cnt):
        if sa_loop % 1000 == 0:
            SUPER_EXIT()
        temperature = temperature_start * (temperature_end / temperature_start) ** (sa_loop / sa_loop_cnt)
        a, b = 0, 0
        while a | 1 == b | 1:
            a = randrange(1, nn + 1)
            b = randrange(1, nn + 1)
        
        aa = a ^ 1
        bb = b ^ 1
        prev_score = connect_benefit_matrix[pairs[a]][pairs[aa]] + connect_benefit_matrix[pairs[b]][pairs[bb]]
        score = connect_benefit_matrix[pairs[b]][pairs[aa]] + connect_benefit_matrix[pairs[a]][pairs[bb]]
        diff = score - prev_score
        probability = 1.0 if diff >= 0 else exp(diff / temperature)
        if random() < probability:
            pairs[a], pairs[b] = pairs[b], pairs[a]
            acc_score += score - prev_score
            if acc_score > best_acc_score:
                best_acc_score = acc_score
                best_pairs = pairs[:]
                if DEBUG:
                    print("#", "SA score =", acc_score)
        else:
            pass
    
    pairs = best_pairs
    return pairs
def calc_pairs_2nd(paths, pairs):
    nn = len(paths)
    benefit_matrix = [[0] * nn for _ in range(nn)]
    ipath = [[] for _ in range(N)]
    for r, path in enumerate(paths):
        for i in path:
            ipath[i].append(r)
    for i, ni in zip(Ts0, Ts):
        if len(ipath[i]) == len(ipath[ni]) == 1:
            a = ipath[i][0]
            b = ipath[ni][0]
            if a != b:
                benefit_matrix[a][b] += 1
                benefit_matrix[b][a] += 1
    
    temperature_start = 50
    temperature_end = 0.2
    sa_loop_cnt = 2 * 10 ** 5
    best_acc_score = -10 ** 9
    best_pairs = None
    acc_score = 0
    for sa_loop in range(sa_loop_cnt):
        if sa_loop % 1000 == 0:
            SUPER_EXIT()
        temperature = temperature_start * (temperature_end / temperature_start) ** (sa_loop / sa_loop_cnt)
        a, b = 0, 0
        while a == b:
            a = randrange(2, len(pairs), 2)
            b = randrange(2, len(pairs), 2)
        if a > b:
            a, b = b, a
        prev_score = benefit_matrix[pairs[a-1]][pairs[a]] + benefit_matrix[pairs[b-1]][pairs[b]]
        score = benefit_matrix[pairs[a-1]][pairs[b-1]] + benefit_matrix[pairs[a]][pairs[b]]
        diff = score - prev_score
        probability = 1.0 if diff >= 0 else exp(diff / temperature)
        if random() < probability:
            pairs = pairs[:a] + pairs[a:b][::-1] + pairs[b:]
            acc_score += score - prev_score
            if acc_score > best_acc_score:
                best_acc_score = acc_score
                best_pairs = pairs[:]
                if DEBUG:
                    print("#", "SA score 2nd =", acc_score)
    pairs = best_pairs
    return pairs
def calc_A_from_pairs(paths, pairs):
    A = []
    for r, k in enumerate(pairs):
        if k < 0:
            continue
        if r % 2:
            A += paths[k][::-1]
        else:
            if A and A[-1] == paths[k][0]:
                A += paths[k][1:]
            else:
                A += paths[k]
    return A

def restricted_BFS(i, j, st):
    # A[st:st+lb] 内で BFS する
    se = set(A[st:st + lb])
    assert j in se
    sel = list(se)
    II = {r: a for a, r in enumerate(sel)}
    nn = len(sel)
    D = [-1] * nn
    prev = [None] * nn
    L = []
    if 1:
        ni = i
        if ni in se:
            a = II[ni]
            D[a] = 1
            L.append(a)
    for ni in EE[i]:
        if ni in se:
            a = II[ni]
            D[a] = 1
            L.append(a)
    assert L
    if II[j] in L:
        return [j]
    while 1:
        nL = []
        for a in L:
            _pi = sel[a]
            for ni in EE[_pi]:
                if ni in se:
                    b = II[ni]
                    if D[b] < 0:
                        assert D[a] > 0
                        D[b] = D[a] + 1
                        prev[b] = a
                        nL.append(b)
                        if sel[b] == j:
                            LL = [j]
                            x = b
                            while prev[x] is not None:
                                x = prev[x]
                                LL.append(sel[x])
                            return LL[::-1]
        L = nL
        if not L:
            return None

def restricted_BFS_inv(prev_st, j, st):
    se_prev = set(A[prev_st:prev_st + lb])
    se = set(A[st:st + lb])
    assert j in se
    sel = list(se)
    II = {r: a for a, r in enumerate(sel)}
    nn = len(sel)
    D = [-1] * nn
    prev = [None] * nn
    L = [II[j]]
    D[II[j]] = 0
    while 1:
        nL = []
        for a in L:
            _pi = sel[a]
            for ni in EE[_pi]:
                if ni in se_prev:
                    LL = [ni, _pi]
                    x = II[_pi]
                    while prev[x] is not None:
                        x = prev[x]
                        LL.append(sel[x])
                    return LL
                if ni in se:
                    b = II[ni]
                    if D[b] < 0:
                        assert D[a] >= 0
                        D[b] = D[a] + 1
                        prev[b] = a
                        nL.append(b)
        L = nL
def restricted_BFS_special(avail, i, j):
    # i から j に avail 内を通って行く
    # i は avail に含まれるが、 j は含まれるとは限らない
    se = set(avail)
    assert i in se
    sel = list(se)
    II = {r: a for a, r in enumerate(sel)}
    nn = len(sel)
    D = [-1] * nn
    prev = [None] * nn
    L = [II[i]]
    D[II[i]] = 0
    while 1:
        nL = []
        for a in L:
            _pi = sel[a]
            for ni in EE[_pi]:
                if ni == j:
                    LL = [ni, _pi]
                    x = II[_pi]
                    while prev[x] is not None:
                        x = prev[x]
                        LL.append(sel[x])
                    return LL
                if ni in se:
                    b = II[ni]
                    if D[b] < 0:
                        assert D[a] >= 0
                        D[b] = D[a] + 1
                        prev[b] = a
                        nL.append(b)
        L = nL
        if not L:
            return None

def calc_A_split():
    _A = []
    L = []
    for a in A:
        if a == terminal:
            L.append(a)
            _A.append(L)
            L = [a]
        else:
            L.append(a)
    if L:
        _A.append(L)
    return _A

# 移動方法の理論値を DP で計算
def connected_area_breakdown(st):
    # A[st:st+lb] の範囲を連結成分に分解
    se = set(A[st:st + lb])
    sel = list(se)
    II = {r: a for a, r in enumerate(sel)}
    nn = len(sel)
    uf = UnionFind(nn)
    for a, j in enumerate(sel):
        for k in EE[j]:
            if k in se:
                b = II[k]
                uf.unite(a, b)
    gg = uf.groups()
    return [[sel[a] for a in g] for g in gg]

def calc_part_part():
    tm.set_type("Calc Part Part (1)")
    part_part_list = []
    for st in range(la - lb + 1):
        SUPER_EXIT()
        L = A[st:st + lb]
        if 0:
            print("L =", L)
        gg = connected_area_breakdown(st)
        for k, g in enumerate(gg):
            if 0:
                print("  ", (st, k, g))
            flg = 1
            part_part_list.append([st, k, g, set(g), flg])

    if LOCAL and loop == 1:
        print("#", "len(part_par_list) before trim =", len(part_part_list))

    tm.set_type("Calc Part Part (2) 上位互換チェック")
    # 上位互換がある場合は消す（フラグをゼロにする）
    for a in range(len(part_part_list)):
        SUPER_EXIT()
        st_a, k_a, g_a, seg_a, flg_a = part_part_list[a]
        if not flg_a:
            continue
        b = a + 1
        while b < len(part_part_list):
            if b - a + len(g_a) > lb:
                break
            st_b, k_b, g_b, seg_b, flg_b = part_part_list[b]
            if not flg_b:
                b += 1
                continue
            if seg_a.issubset(seg_b):
                part_part_list[a][-1] = 0
            b += 1
    for a in range(len(part_part_list)):
        SUPER_EXIT()
        st_a, k_a, g_a, seg_a, flg_a = part_part_list[a]
        if not flg_a:
            continue
        b = a - 1
        while b >= 0:
            if a - b + len(g_a) > lb:
                break
            st_b, k_b, g_b, seg_b, flg_b = part_part_list[b]
            if not flg_b:
                b -= 1
                continue
            if seg_a.issubset(seg_b):
                part_part_list[a][-1] = 0
            b -= 1
    part_part_st = [st for st, k, g, seg, flg in part_part_list if flg]
    part_part = [g for st, k, g, seg, flg in part_part_list if flg]
    part_part_set = [seg for st, k, g, seg, flg in part_part_list if flg]
    if LOCAL and loop == 1:
        print("#", "len(part_par_list) after trim =", len(part_part))
    return part_part_st, part_part, part_part_set


def calc_part_part_for_vertex(part_part):
    tm.set_type("Calc Part for Vertex (1)")
    PL = [[] for _ in range(N)]
    for p, L in enumerate(part_part):
        for i in L:
            PL[i].append(p)
    if LOCAL and DEBUG:
        for i in range(N):
            print("#", i, PL[i])

    # 辺を計算
    E_pp = [[] for _ in range(len(part_part))]
    for a in range(len(part_part)):
        SUPER_EXIT()
        se = set()
        for i in part_part[a]:
            for j in EE[i]:
                se.add(j)
        sep = set()
        for j in se:
            for p in PL[j]:
                sep.add(p)
        for b in sep:
            E_pp[a].append(b)

    if LOCAL:
        edge_count = sum([len(ee) for ee in E_pp])
        print("#", "len(E_pp) =", len(E_pp), "Edge Count =", edge_count)

    tm.set_type("Calc Part for Vertex (2) - BFS")
    DDDDD = []
    for i in range(len(E_pp)):
        SUPER_EXIT()
        DDDDD.append(BFS(len(E_pp), E_pp, i))
        
    if DEBUG:
        print("DDDDD[0] =", DDDDD[0])

    tm.set_type("Calc Part for Vertex (3) - DP")

    # DP を書く（うしろからが良さそう）
    DP = [[10 ** 9] * len(PL[t]) for t in Ts0]
    DP[-1] = [0] * len(PL[Ts0[-1]])
    for t in range(len(Ts0) - 2, -1, -1):
        SUPER_EXIT()
        frm = Ts0[t]
        to = Ts0[t + 1]
        frmL = PL[frm]
        toL = PL[to]
        for r in range(len(PL[frm])):
            assert 0 <= t < len(DP), t
            assert 0 <= r < len(DP[t]), r
            DP[t][r] = min([DP[t + 1][s] + DDDDD[frmL[r]][toL[s]] for s in range(len(toL))] + [10 ** 9])
    if DEBUG:
        print(min(DP[0]), DP[0])
        for t in range(len(Ts0) - 2, -1, -1):
            print("t", t, Ts0[t], "->", Ts0[t + 1])
            print("min", min(DP[t]), DP[t])
    return PL, DP, E_pp, DDDDD


def set_signal_ANS(st, state):
    global current_signal_st, current_state, signal_change_count
    if LOCAL and loop == 1:
        print("#", f"current_state -> {state}, st={part_part_st[state]}")
    ANS.append((0, lb, st, 0))
    current_state = state
    current_signal_st = st
    signal_change_count += 1
    for r in range(lb):
        current_signal[r] = A[st + r]
def set_signal_ANS_special(l, a, b):
    global current_signal_st, current_state, signal_change_count
    if LOCAL and loop == 1:
        print("#", f"Set Signal Special -> {(l, a, b)}")
    ANS.append((0, l, a, b))
    current_state = None
    current_signal_st = None
    signal_change_count += 1
    for r in range(l):
        current_signal[r + b] = A[a + r]


def move_to_ANS(i):
    global current_position
    if current_position == i:
        return
    ANS.append((1, i))
    current_position = i

def SUPER_EXIT():
    if BEST_ANS is None:
        return 0
    if tm.elapsed_time() < target_time:
        return 0
    if LOCAL:
        return 1
    print("#", "SUPER EXIT")
    answer(BEST_A, BEST_ANS)
    print("#", f"score={BEST_SCORE}")
    if CODETEST:
        tm.disp()
    exit()
def answer(_A, _ANS):
    print("#", "answer")
    if CODETEST:
        print("#", "Skip Code Test")
    else:
        print(*_A)
        for ans in _ANS:
            if ans[0] == 0:
                _, l, a, b = ans
                print("s", l, a, b)
            else:
                _, i = ans
                print("m", i)


# ########## ########## ########## ########## ########## ########## ########## ########## ########## ##########
try:
    LOCAL
except NameError:
    LOCAL = 0
if not LOCAL:
    DEBUG = 0
loop = 1
CODETEST = 0
# 入力
if LOCAL:
    print("Seed ?")
    I = input()
    I = 0 if not I else int(I)
    if I < 0:
        loop = -I
        DEBUG = 0
    else:
        seed = I
        N, M, T, la, lb, E, Ts, XY = preset_testcase_txt(seed)
else:
    I = input()
    if len(I) <= 3 or I[0] in "CODETESTcodetest":
        CODETEST = 1
        I = input()
    N, M, T, la, lb = map(int, I.split())
    E = []
    for _ in range(M):
        a, b = map(int, input().split())
        E.append((a, b))
    Ts = [int(a) for a in input().split()]
    XY = []
    for _ in range(N):
        x, y = map(int, input().split())
        XY.append((x, y))
    seed = None


BEST_A = None
BEST_ANS = None
BEST_SCORE = None

target_time = 2.80 * (4.0 if LOCAL else 1.0)
score_log_sum = 0
ssTime = time()
for lp in range(loop):
    if LOCAL:
        randomseed(81288128)
    if loop > 1:
        seed = lp
        N, M, T, la, lb, E, Ts, XY = preset_testcase_txt(seed)
    
    tm = TimeManager()
    Ts0 = [0] + Ts
    if loop == 1:
        print("# N, M, T, la, lb =", N, M, T, la, lb)
    target_set = set(Ts)
    target_count = [0] * N
    target_count[0] += 1
    for g in Ts:
        target_count[g] += 1
    EE = [[] for _ in range(N)]
    for a, b in E:
        EE[a].append(b)
        EE[b].append(a)

    DD = [BFS(N, EE, i) for i in range(N)]
    MA = [max(dd) for dd in DD]
    SU = [sum([target_count[i] * d for i, d in enumerate(dd)]) for dd in DD]
    if DEBUG:
        print("MA =", MA)
        print("SU =", SU)


    # Terminal （木の根とする頂点）を決める
    # ターゲットまでの距離の総和が最小のものを選ぶ
    tm.set_type("Terminal")
    terminal = calc_best_center()
    if DEBUG:
        print(f"terminal = {terminal}")
    
    BEST_A = None
    BEST_ANS = None
    BEST_SCORE = None
    try_count = 6 if LOCAL and loop == 1 else 10
    for try_id in range(try_count):
        if SUPER_EXIT():
            break
        
        if try_id:
            # 分散が大きくなるようにひっくり返す
            for r in range(0, len(EE)):
                EE[r] = EE[r][::-1]
        
        # Terminal から各頂点までの距離
        tm.set_type("Dist from Terminal")
        dist_from_terminal = BFS(N, EE, terminal)

        # ターゲットにならない頂点のうち不要なものは削除
        tm.set_type("Unnecessary")
        unnecessary = [0] * N
        for i in sorted(list(range(N)), key=lambda x: -dist_from_terminal[x]):
            if target_count[i]:
                continue
            for j in EE[i]:
                if (not unnecessary[j]) and dist_from_terminal[j] > dist_from_terminal[i]:
                    break

            else:
                unnecessary[i] = 1
        
        SUPER_EXIT()
        for _ in range(3):
            for i in range(N):
                if target_count[i]:
                    continue
                if unnecessary[i] == 0:
                    total_count = 0
                    same_distance_existence = 0
                    target_j = None
                    necessary = 0
                    larger_k = 0
                    special_js = []
                    for j in EE[i]:
                        if unnecessary[j]:
                            continue
                        if dist_from_terminal[j] <= dist_from_terminal[i]:
                            continue
                        total_count += 1
                        target_j = j
                        smaller_k = 0
                        larger_k_ = 0
                        same_distance_count = 0
                        for k in EE[j]:
                            if k == i:
                                continue
                            if unnecessary[k]:
                                continue
                            if dist_from_terminal[k] < dist_from_terminal[j]:
                                smaller_k = 1
                            elif dist_from_terminal[k] == dist_from_terminal[j]:
                                same_distance_count += 1
                                same_distance_existence = 1
                            elif dist_from_terminal[k] > dist_from_terminal[j]:
                                larger_k_ += 1
                        if not smaller_k:
                            # j は i より大きく、 j より小さい k がなかった
                            # この時点でこの i は必要っぽい
                            if same_distance_count >= 1 and larger_k_ == 0:
                                special_js.append(j)
                            else:
                                necessary = 1
                            # break
                    if not necessary:
                        # print("#", "New Unnecessary", i)
                        if not special_js:
                            unnecessary[i] = 1
                        elif len(special_js) == 1:
                            if LOCAL and loop == 1:
                                print("#", f"Special Js i={i} js={special_js} dist={dist_from_terminal[j]}")

                            unnecessary[i] = 1
                            for j in special_js:
                                dist_from_terminal[j] += 1
                            if LOCAL and loop == 1:
                                print("#", f"Special Js i={i} js={special_js} dist={dist_from_terminal[j]}")
                        continue
                    elif 0 and dist_from_terminal[i] >= 2 and total_count == 1 and same_distance_existence and larger_k == 0:
                        if LOCAL and loop == 1:
                            print("#", "i, target_j =", i, target_j)
                        unnecessary[i] = 1
                        dist_from_terminal[target_j] += 1

        if DEBUG:
            print("unnecessary =", sum(unnecessary), [i for i in range(N) if unnecessary[i]])
        SUPER_EXIT()
        # Terminal を根とする木構造にする
        parent = [None] * N
        children = [[] for _ in range(N)]
        for i in sorted(list(range(N)), key=lambda x: dist_from_terminal[x]):
            if dist_from_terminal[i] == 0:
                continue
            if unnecessary[i]:
                continue
            SUPER_EXIT()
            best = 5
            best_j = None
            for j in EE[i]:
                if unnecessary[j]:
                    continue
                if dist_from_terminal[j] == dist_from_terminal[i] - 1:
                    s = 2 if unnecessary[j] else 1
                    if s < best:
                        best = s
                        best_j = j
            j = best_j
            if j is None:
                print("#", f"i={i} {XY[i]}, un={unnecessary[i]}, d={dist_from_terminal[i]}, j={j}")
                assert 0
            parent[i] = j
            children[j].append(i)
        for i in sorted(list(range(N)), key=lambda x: -dist_from_terminal[x]):
            if unnecessary[i]:
                assert not children[i]
                if parent[i] is not None:
                    j = parent[i]
                    parent[i] = None
                    children[j].remove(i)
        
        SUPER_EXIT()
        # 葉を計算
        leaves = [i for i in range(N) if unnecessary[i] == 0 and len(children[i]) == 0]
        if DEBUG:
            print("parent =", parent)
            print("children =", children)
            print("leaves =", len(leaves), leaves)
        
        SUPER_EXIT()
        # 各葉から、（木の辺を取って） Terminal までを最短距離で結ぶ
        tm.set_type("Paths")
        paths = []
        for i in leaves:
            L = [i]
            while parent[i] is not None:
                i = parent[i]
                L.append(i)
            paths.append(L[::-1])
        
        SUPER_EXIT()
        # 葉→根のかたまりをいい感じにくっ付ける
        tm.set_type("Merge Paths")
        paths = merge_paths(paths)
        tm.set_type("Calc A SA")
        pairs = calc_pairs_1st(paths)
        SUPER_EXIT()
        pairs = calc_pairs_2nd(paths, pairs)
        SUPER_EXIT()
        A = calc_A_from_pairs(paths, pairs)
        SUPER_EXIT()

        assert 0 in A
        A += [0] * (la - len(A))
        # A += connect_sub_terminals
        # la += len(connect_sub_terminals)
        
        A_split = calc_A_split()
        tm.set_type("---")
        ANS = []
        part_part_st, part_part, part_part_set = calc_part_part()
        tm.set_type("---")
        SUPER_EXIT()
        PL, DP, E_pp, DDDDD = calc_part_part_for_vertex(part_part)
        PL_set = [set(a) for a in PL]

        # state をある r に対する A[r:r+lb] のひとつの連結成分（これらに ID を振る）で表す
        # PL[頂点ID]: その頂点を含む state
        # E_pp[state]: その state から遷移できる state のリスト

        tm.set_type("Fin")
        current_state = None
        current_signal_st = None
        current_signal = [-1] * lb
        current_position = 0
        signal_change_count = 0
        for _t, (t1, t2) in enumerate(zip(Ts0, Ts)):
            SUPER_EXIT()
            if LOCAL and loop == 1:
                print("#", f"t={_t}->{_t + 1} {t1}->{t2} ###")
            while 1:
                if current_state is None:
                    _E = list(set(sum([PL[i] for i in EE[0]], PL[0])))
                else:
                    _E = E_pp[current_state]

                # 現在の state から目的地に到達できる場合はする
                if current_state in PL[t2]:
                    r_path = restricted_BFS(current_position, t2, part_part_st[current_state])
                    for i in r_path:
                        move_to_ANS(i)
                    break

                # 到達可能な state のうち最も良いものを選ぶ
                mimi = 10 ** 9
                mimi_state = None
                for state in _E:
                    if (current_state is not None) and DDDDD[current_state][state] != 1:
                        continue
                    mi = 10 ** 9
                    mir = None
                    for r in range(len(PL[t2])):
                        d = DP[_t + 1][r] + DDDDD[state][PL[t2][r]]
                        if d < mi:
                            mi = d
                            mir = r
                    if mi < mimi:
                        mimi = mi
                        mimi_state = state

                def find_lr(r_path, st):
                    II = {A[k]: k for k in range(st, st+lb)}
                    l = 99999
                    r = -1
                    for a in r_path[1:]:
                        k = II[a]
                        l = min(l, k)
                        r = max(r, k)
                    return l, r
                def tmp_area_uf(s, next_signal):
                    uf = UnionFind(lb)
                    for a in range(lb):
                        for b in range(a):
                            i = next_signal[a]
                            j = next_signal[b]
                            if DD[i][j] == 1:
                                uf.unite(a, b)
                    G = uf.groups()
                    for g in G:
                        for r in g:
                            if next_signal[r] == s:
                                return [next_signal[r] for r in g]
                    assert 0

                # next_state が決まったら隣接マス経由で移動
                next_state = mimi_state
                next_state_evaluation = mimi
                st = part_part_st[next_state]
                #####
                if current_state is not None and t2 in part_part_set[next_state] and tm.elapsed_time() < target_time * 0.98:
                    # 現状の信号が一部使えそうだったら使いたい
                    next_set = part_part_set[next_state]
                    prev_st = part_part_st[current_state]
                    r_path = restricted_BFS_inv(prev_st, t2, st)
                    l, r = find_lr(r_path, st)
                    le = r - l + 1
                    if _t == len(Ts) - 1:
                        pass
                    elif le >= lb:
                        # le が全体の場合は改善の余地がないのでスルー
                        pass
                    else:
                        for i in range(r - lb + 1, l + lb):
                            # 現在の state からつなげられそうなのでスルー
                            if prev_st - 1 <= i < prev_st + lb + 1:
                                break
                        else:
                            best_ins_pos = None
                            best_size = -1
                            for ins_pos in [0, lb-le]:
                                tmp_signal = A[st:st+lb]
                                next_signal = current_signal[:ins_pos] + A[l:r+1] + current_signal[ins_pos+le:]
                                assert len(next_signal) == lb
                                g = tmp_area_uf(t2, next_signal)
                                if len(g) > best_size:
                                    best_size = len(g)
                                    best_ins_pos = ins_pos
                                    best_g = g
                            g = best_g
                            ins_pos = best_ins_pos
                            Is = set(g)
                            Js = set(Is)
                            for i in Is:
                                for j in EE[i]:
                                    Js.add(j)
                            avail_states = set()
                            for j in Js:
                                avail_states |= PL_set[j]
                            t3 = Ts[_t + 1]
                            mimi = 10 ** 9
                            mimi_state = None
                            for state in avail_states:
                                if (current_state is not None) and DDDDD[current_state][state] != 1:
                                    continue
                                mi = 10 ** 9
                                mir = None
                                for r in range(len(PL[t3])):
                                    d = DP[_t + 2][r] + DDDDD[state][PL[t3][r]]
                                    if d < mi:
                                        mi = d
                                        mir = r
                                if mi < mimi:
                                    mimi = mi
                                    mimi_state = state

                            # tmp_next_state が決まったら隣接マス経由で移動
                            tmp_next_state = mimi_state
                            tmp_next_state_evaluation = mimi + 1
                            tmp_st = part_part_st[tmp_next_state]

                            for j in part_part[tmp_next_state]:
                                if j in Js:
                                    jj = j
                                    break

                            if tmp_next_state_evaluation < next_state_evaluation:
                                # r_path[0] へ
                                rr_path = restricted_BFS(current_position, r_path[0], part_part_st[current_state])
                                if rr_path is None:
                                    # なぜか無限ループになることがあるので緊急回避
                                    print("#", "Unexpected Error")
                                else:
                                    for i in rr_path:
                                        move_to_ANS(i)
                                    # t2 へ
                                    set_signal_ANS_special(le, l, ins_pos)
                                    for i in r_path[1:]:
                                        move_to_ANS(i)
                                    rrr_path = restricted_BFS_special(g, current_position, jj)[::-1][1:]
                                    for i in rrr_path[:-1]:
                                        move_to_ANS(i)

                                    set_signal_ANS(tmp_st, tmp_next_state)
                                    break


                #####
                li = part_part[next_state]
                if current_state is not None:
                    flg = 0
                    for i in part_part[current_state]:
                        for j in part_part[next_state]:
                            if i == j or DD[i][j] == 1:
                                r_path = restricted_BFS(current_position, i, part_part_st[current_state])
                                for ii in r_path:
                                    move_to_ANS(ii)
                                flg = 1
                                break
                        if flg:
                            break

                set_signal_ANS(st, next_state)
                r_path = restricted_BFS(current_position, li[0], st)
                for i in r_path:
                    move_to_ANS(i)
        print("#", f"score={signal_change_count}")
        if BEST_ANS is None or signal_change_count < BEST_SCORE:
            BEST_ANS = ANS
            BEST_A = A
            BEST_SCORE = signal_change_count
    if loop == 1:
        answer(BEST_A, BEST_ANS)
        print("#", f"seed={seed} score={BEST_SCORE}")
        tm.disp()
    else:
        score_log_sum += log(BEST_SCORE)
        print("#", f"seed={seed} M={M:>4} La={la:>4} Lb={lb:>2} time={'{:.1f}'.format(tm.elapsed_time()):>4} {'{:.1f}'.format(time() - ssTime)} score={BEST_SCORE:>4} average={round(exp(score_log_sum / (lp + 1)))}")
print("#", "Done!!")