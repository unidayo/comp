#!/usr/bin/env python3
import random
import sys


def main():
    n = int(sys.argv[1]) if len(sys.argv) >= 2 else 300000
    seed = int(sys.argv[2]) if len(sys.argv) >= 3 else 0
    random.seed(seed)
    print(n)
    print(" ".join(str(random.randint(-200, 200)) for _ in range(n)))


if __name__ == "__main__":
    main()
