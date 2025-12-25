from itertools import product,permutations
import sys

def judge(perm,state):
    def place(sr,sc,a,b):
        for row in range(sr,sr+a):
            for col in range(sc,sc+b):
                if G[row][col] == 1:
                    return False
                G[row][col] = 1
        return True

    G = [[0] * W for _ in range(H)]
    cnt = 0
    for p,s in zip(perm,state):
        a,b=AB[p]
        if s != 0:
            cnt += a*b
        if s == 1:
            if (a > H or b > W):
                return False
        if s == 2:
            if (b > H or a > W):
                return False
    if cnt != H * W:
        return False

    for p,s in zip(perm,state):
        if s ==0:
            continue
        if s == 1:
            a, b = AB[p]
        else:
            b, a = AB[p]
        for sr,sc in product(range(H-a+1),range(W-b+1)):
            if G[sr][sc] == 0:
                break
        if not place(sr,sc,a,b):
            return False
    return True

def dfs(n):
    sr,sc =-1,-1
    for row in range(H):
        for col in range(W):
            if G[row][col] == 0:
                sr,sc=row,col
                break
    if sr == -1: return
    for i in range(N):
        if seen[i]:
            continue
        

N,H,W = map(int,input().split())
AB = [tuple(map(int,input().split())) for _ in range(N)]
seen = [0] * N
G = [[0] * W for _ in range(H)]
pieces = []
dfs(0)
print("No")
