#ahc035_6
#真ん中から順に嬉しさが最大になるものを植える
#植える候補を「各パラメータの上位から」取ってくる
import time
t0 = time.time()
import random, math
random.seed(14)
N, M, T = map(int,input().split())
NN = N * N
Seedscnt = 2 * N * (N-1)
Seeds = []
whatPlant = []
AllPairs = []
dx = [1, 0, -1, 0]
dy = [0, 1, 0, -1]
SeeOrder = []
turn = 0

def merge(x, y):
    return x * N + y

def sep(xy):
    return xy // N, xy % N

for i in range(N):
    for j in range(N):
        if(i + 1 < N):
            AllPairs.append((merge(i, j), merge(i + 1, j)))
        if(j + 1 < N):
            AllPairs.append((merge(i, j), merge(i, j + 1)))

for i in range(1, N-1):
    for j in range(1, N-1):
        SeeOrder.append(merge(i, j))

for i in range(1, N-1):
    SeeOrder.append(merge(0, i))

for i in range(1, N-1):
    SeeOrder.append(merge(N-1, i))

for i in range(1, N-1):
    SeeOrder.append(merge(i, 0))

for i in range(1, N-1):
    SeeOrder.append(merge(i, N-1))

SeeOrder.append(merge(0, 0))
SeeOrder.append(merge(0, N-1))
SeeOrder.append(merge(N-1, 0))
SeeOrder.append(merge(N-1, N-1))

#Pで植えたときの新たな種たちを返す
def mate_simulate(P):
    nxtSeeds = []
    for (s, t) in AllPairs:
        newSeed = []
        for i in range(M):
            if(random.random() < 0.5):
                newSeed.append(Seeds[P[s]][i])
            else:
                newSeed.append(Seeds[P[t]][i])
        nxtSeeds.append(newSeed)
    return nxtSeeds


#whatPlantで植えたときの「嬉しさ」を返す
def calculate_Plant_score():
    score = 0
    for (s, t) in AllPairs:
        for i in range(M):
            score += max(Seeds[whatPlant[s]][i], Seeds[whatPlant[t]][i])
    return score

#ijに何か植えた時のijに関わる嬉しさを返す
def calculate_Plant_score_diff(xy):
    diff = 0
    x, y = sep(xy)
    for k in range(4):
        newx, newy = x + dx[k], y + dy[k]
        if(0 <= newx < N and 0 <= newy < N):
            newxy = merge(newx, newy)
            for s in range(M):
                diff += max(Seeds[whatPlant[xy]][s], Seeds[whatPlant[newxy]][s])
    return diff

#Seedsのスコアを返す
#とりあえず生スコアで
def calculate_Seeds_score(Seeds):
    best_v = 0
    for i in range(Seedscnt):
        v = 0
        for j in range(M):
            v += Seeds[i][j]
        if(v > best_v):
            best_v = v
    # print("#", best_v)
    return best_v

def plant():
    global Seeds
    global whatPlant
    Seeds=[]
    for i in range(Seedscnt):
        Seeds.append(list(map(int,input().split())))

    #神の種(God Seed)
    Seeds.append([100] * M)
    # whatPlant = random.sample(range(Seedscnt), NN)

    
    #神の種で埋める
    whatPlant = [Seedscnt] * NN
    
    #種が使用済みかどうか
    used = [False] * Seedscnt
    
    #使用する種
    to_use = set()
    
    Xs = []
    for i in range(M):
        X = [Seeds[j][i] for j in range(Seedscnt)]
        X.sort(reverse=True)
        Xs.append(X)
    
    
    # rank = 0
    # while len(to_use) < NN +  turn:
    #     for j in range(M):
    #         for i in range(Seedscnt):
    #             if(i in to_use):
    #                 continue
    #             if(Seeds[i][j] == Xs[j][rank]):
    #                 to_use.add(i)
    #                 break
    #         if(len(to_use) == NN +  turn):
    #             break
    #     rank += 1
    
    #希少さを表す
    Reality = [0] * Seedscnt
    
    for i in range(Seedscnt):
        for j in range(M):
            if(Seeds[i][j] == Xs[j][0]):
                Reality[i] += 1 + 2/Xs[j].count(Xs[j][0])
            
    
    # print("#", rank)
    # print("#",to_use)
    # print("#", len(to_use))
    # for i in range(Seedscnt):
    #     if(i not in to_use):
    #         used[i] = True
    

    # print("#", Reality)
    #内側から
    for ij in SeeOrder:
        best_seed = -1
        best_score = 0
        for k in range(Seedscnt):
            if(used[k]):
                continue
            whatPlant[ij] = k
            new_score = calculate_Plant_score_diff(ij) + 15 * Reality[k] * (10 - turn)
            if(best_score < new_score):
                best_score = new_score
                best_seed = k
        whatPlant[ij] = best_seed
        used[best_seed] = True
    

    for i in range(N):
        print(*whatPlant[i * N: (i + 1) * N])
    
    return


def main():
    global turn
    for _ in range(T):
        
        plant()
        turn += 1

main()