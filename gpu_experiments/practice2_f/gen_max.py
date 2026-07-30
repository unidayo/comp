#!/usr/bin/env python3
import random
import sys


MOD = 998244353


def main():
    n = int(sys.argv[1]) if len(sys.argv) >= 2 else 524288
    m = int(sys.argv[2]) if len(sys.argv) >= 3 else 524288
    seed = int(sys.argv[3]) if len(sys.argv) >= 4 else 0
    mod = int(sys.argv[4]) if len(sys.argv) >= 5 else MOD
    random.seed(seed)

    print(n, m)
    print(" ".join(str(random.randrange(mod)) for _ in range(n)))
    print(" ".join(str(random.randrange(mod)) for _ in range(m)))


if __name__ == "__main__":
    main()
