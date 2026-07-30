#!/usr/bin/env python3
import random
import sys


def main():
    n = int(sys.argv[1]) if len(sys.argv) >= 2 else 2000
    m = int(sys.argv[2]) if len(sys.argv) >= 3 else 2000
    seed = int(sys.argv[3]) if len(sys.argv) >= 4 else 0
    random.seed(seed)

    print(n, m)
    for _ in range(n):
        print(" ".join(str(random.randint(1, 999)) for _ in range(m)))


if __name__ == "__main__":
    main()
