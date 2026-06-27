from time import time
from random import shuffle, randrange, random
from math import exp

try:
    LOCAL
except NameError:
    LOCAL = 0

loop = 1
if LOCAL:
    DEBUG = 1
    print("# Seed ?")
    I = int(input().replace("#", "").replace(" ", ""))
    if I < 0:
        loop = -I
        if loop >= 5:
            DEBUG = 0
            if 0:
                print("# Loop ID ?", "-1:seriatim")
                I = input()
                multicase_index = -1 if not I else int(I)
    else:
        N, M, X = preset_testcase_txt(I)
else:
    DEBUG = 0
    N, M = map(int, input().split())
    X = [[int(a) for a in input().split()] for _ in range(M)]

if DEBUG:
    print("N =", N)
    print("M =", M)
    print("X =", X)

def move(_frm, _to, cnt):
    if _frm == _to or cnt == 0:
        return
    if 0:
        print("move", _frm, _to, cnt)
        print(X[_frm])
        print(X[_to])
    global score
    score -= cnt + 1
    v = X[_frm][-cnt]
    ANS.append((v, _to))
    X[_to] += X[_frm][-cnt:]
    X[_frm][-cnt:] = []
    sorted_count[_frm] = min(sorted_count[_frm], len(X[_frm]))
def carry_out():
    global carried_out_count
    for i in range(M):
        if X[i] and X[i][-1] == carried_out_count + 1:
            X[i].pop()
            carried_out_count += 1
            ANS.append((carried_out_count, -1))
            sorted_count[i] = min(sorted_count[i], len(X[i]))
            return 1
    return 0
def space_sort(block_id, k = 9):
    global carried_out_count
    def is_sorted(b):
        return 1 if X[b] == sorted(X[b]) else 0
    def decreasing_count(b, IL = None):
        if not X[b]:
            return 0
        i = 1
        while 1:
            if i >= len(X[b]):
                break
            a1 = X[b][~(i - 1)]
            a2 = X[b][~i]
            if (a1 > a2):
                i += 1
            elif IL and a1 in IL and a2 in IL and IL[a1] == IL[a2] - 1:
                i += 1
            else:
                break
        return i
    def find_best_block(x):
        best_score = -1
        best_block = -1
        for i in range(M):
            if i == block_id:
                continue
            
            if X[i]:
                score = 1000 - (x - X[i][-1]) if x > X[i][-1] else X[i][-1]
            else:
                score = 1
            if score > best_score:
                best_score = score
                best_block = i
        return best_block
    if DEBUG:
        print("sorted_count =", sorted_count)
    
    L = sorted(X[block_id])
    IL = {a: i for i, a in enumerate(L)}
    
    # 各列に分散
    while X[block_id]:
        c = decreasing_count(block_id, IL)
        if X[block_id][-c] == carried_out_count + 1:
            c -= 1
        if c:
            x = X[block_id][-c]
            best_block = find_best_block(x)
            if 0:
                print("x =", x)
                print("best_block =", best_block)
            move(block_id, best_block, c)
        carry_out()
        if DEBUG:
            for x in X:
                print(x)
    
    # 元の列に昇順になるように戻す
    cnt_to_return = [decreasing_count(i, IL) for i in range(M)]
    if DEBUG:
        print("cnt_to_return =", sum(cnt_to_return), cnt_to_return)
    
    L = []
    for i, c in enumerate(cnt_to_return):
        if not c:
            continue
        L += X[i][-c:]
    L.sort()
    while L:
        x = L.pop()
        for i in range(M):
            if X[i] and X[i][-1] == carried_out_count + 1:
                carry_out()
            if i == block_id:
                continue
            for cc in range(5, 0, -1):
                if len(X[i]) - cc >= sorted_count[i] and X[i][-cc] == x:
                    move(i, block_id, cc)
    sorted_count[block_id] = len(X[block_id])
best_ANS = []
score_sum = 0
for loop_id in range(loop):
    if loop > 1:
        N, M, X = preset_testcase_txt(loop_id)
        # print("-" * 10)
        # print("loop_id =", loop_id)
    org_X = X
    sTime = time()
    best_score = -10 ** 4
    score = -10 ** 4
    II = [i for i in range(M)]
    trial_count = 0
    time_limit = 1.90 * (6 if LOCAL else 1)
    T_start = 30
    T_end = 2
    while 1:
        if trial_count:
            DEBUG = 0
        time_ratio = (time() - sTime) / time_limit
        if time_ratio > 1:
            break
        if DEBUG:
            print("TRIAL =", trial_id)
        
        # 焼きなまし（2 点スワップ）
        T = T_start * (T_end / T_start) ** time_ratio
        ia, ib = 0, 0
        while ia == ib:
            ia = randrange(M)
            ib = randrange(M)
        prev_II = II[:]
        prev_score = score
        II[ia], II[ib] = II[ib], II[ia]
        
        
        X = [x[:] for x in org_X]
        score = 10000
        carried_out_count = 0
        sorted_count = [0 for _ in range(M)]
        ANS = []
        while carry_out():
            pass
        for i in II:
            space_sort(i)
        while carried_out_count < N:
            if not carry_out():
                score = -10 ** 9
                break

        if score > best_score:
            best_score = score
            best_ANS = ANS
        
        score_movement = score - prev_score
        prob = 1 if score_movement >= 0 else exp(score_movement / T)
        if random() > prob:
            prev_II = II[:]
            prev_score = score
        else:
            pass
            
        
        trial_count += 1
    if LOCAL and loop == 1:
        DEBUG = 1
    
    if best_ANS:
        score = best_score
        ANS = best_ANS
    if loop == 1:
        for v, ii in ANS:
            print(v, ii + 1)
    if DEBUG:
        print("score =", score, "trial =", trial_count)
        print("sorted_count =", sorted_count)
    
    if loop > 1:
        score_sum += score
        average = score_sum / (loop_id + 1)
        print("loop_id =", loop_id, "score =", score, "trial =", trial_count, "average =", round(average))
if 0:
    print("score =", score, "trial =", trial_count)
