N, M, T = map(int, input().split())
SEED_COUNT = 2 * N * (N - 1)
X = []

for i in range(SEED_COUNT):
    X.append(list(map(int, input().split())))

for t in range(T):
    A = [[-1] * N for i in range(N)]
    best_scores=[0]*M
    for i in range(SEED_COUNT):
        for j in range(M):
            best_scores[j]=max(best_scores[j],X[i][j])
    best_achievers=[]
    for i in range(SEED_COUNT):
        best_achievers.append([])
        for j in range(M):
            if best_scores[j]<=X[i][j]:
                best_achievers[-1].append(1)
            elif best_scores[j]<=X[i][j]+1:
                best_achievers[-1].append(0.1)
            else:
                best_achievers[-1].append(0)
    spiral_order=[(2,2),(2,3),(3,3),(3,2),(3,1),(2,1),(1,1),(1,2),(1,3),(1,4),(2,4),(3,4),(4,4),(4,3),(4,2),(4,1),(4,0),(3,0),(2,0),(1,0),(0,0),(0,1),(0,2),(0,3),(0,4),(0,5),(1,5),(2,5),(3,5),(4,5),(5,5),(5,4),(5,3),(5,2),(5,1),(5,0)]
    # print(len(spiral_order))
    used_seeds=set()
    for i,j in spiral_order:
        best_score=0
        best_seed=0
        for k in range(SEED_COUNT):
            if k in used_seeds:
                continue
            score=0
            ct=0
            if i>0 and A[i-1][j]!=-1:
                ct+=1
                for l in range(M):
                    score+=abs(X[k][l]-X[A[i-1][j]][l])
                    score+=abs(best_achievers[k][l]-best_achievers[A[i-1][j]][l])*8*(T-t)
            if j>0 and A[i][j-1]!=-1:
                ct+=1
                for l in range(M):
                    score+=abs(X[k][l]-X[A[i][j-1]][l])
                    score+=abs(best_achievers[k][l]-best_achievers[A[i][j-1]][l])*8*(T-t)
            if i<N-1 and A[i+1][j]!=-1:
                ct+=1
                for l in range(M):
                    score+=abs(X[k][l]-X[A[i+1][j]][l])
                    score+=abs(best_achievers[k][l]-best_achievers[A[i+1][j]][l])*8*(T-t)
            if j<N-1 and A[i][j+1]!=-1:
                ct+=1
                for l in range(M):
                    score+=abs(X[k][l]-X[A[i][j+1]][l])
                    score+=abs(best_achievers[k][l]-best_achievers[A[i][j+1]][l])*8*(T-t)
            if ct>0:
                score//=ct
            score+=sum(X[k])*1.1
            score+=sum(best_achievers[k])*12*(T-t)
            if score>best_score:
                best_score=score
                best_seed=k
        A[i][j]=best_seed
        used_seeds.add(best_seed)
                    

    for i in range(N):
        print(' '.join(map(str, A[i])), flush=True)

    X = []

    for i in range(SEED_COUNT):
        X.append(list(map(int, input().split())))
