from itertools import permutations

N = 6
K = 3
for p in permutations(range(1,N+1),K):
    print(*p)
