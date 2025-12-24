import random
from collections import Counter

N = 10 ** 4
S = 1000

A = [125,60,-5]
B = [-180,-195,-210,-225,-240,-255]
C = [20,5,-5,-18]
D = [1400,1600,1800,2000,3000,4500]
E = [2800,3200,3600,4000,6000,9000]
P = [0.2533,0.2925,0.2656,0.1886]

def sim():
    rem = S
    curdan = 4
    curpt = 3000
    while rem>0:
        J = random.choices((0,1,2,3),weights=P)[0]
        if J != 3:
            curpt += A[J]
        else:
            curpt += B[curdan]
        curpt += C[J]
        if curpt >= E[curdan]:
            curdan+=1
            if curdan ==6:
                break
            curpt = D[curdan]
        elif curpt < 0:
            curdan-=1
            if curdan == -1:
                break
            curpt = D[curdan]
        rem -= 1
    return curdan

tbl = {-1:"雀傑3",0:"雀豪1",1:"雀豪2",2:"雀豪3",3:"雀聖1",4:"雀聖2",5:"雀聖3",6:"魂天1"}

def main():
    cnt = Counter()
    for _ in range(N):
        res = sim()
        cnt[res]+=1
    for i in range(-1,7):
        print(f"{tbl[i]}: {cnt[i]}")

main()