import math

def f(x):
    return P/pow(2,x/1.5) + x


def solve():
    l = 0
    r = 100
    while abs(r-l) >= 10**-10:
        l2 = l/3 * 2 + r/3
        r2 = l/3 +r/3*2
        if(f(l2) < f(r2)):
            r = r2
        else:
            l = l2
    return f(l)

P = float(input())
print(solve())