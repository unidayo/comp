#!/usr/bin/env python3
import math
import sys
from collections import Counter


def die(msg: str) -> None:
    print(f"WA: {msg}", file=sys.stderr)
    sys.exit(1)


def read_ints(path: str) -> list[int]:
    with open(path, "r", encoding="utf-8") as f:
        return [int(x) for x in f.read().split()]


def main() -> None:
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} INPUT OUTPUT", file=sys.stderr)
        sys.exit(2)

    inp = read_ints(sys.argv[1])
    out = read_ints(sys.argv[2])

    ptr = 0
    if not inp:
        die("empty input")
    n = inp[ptr]
    ptr += 1
    if len(inp) != 1 + n * n:
        die("bad input length")

    board: list[list[int | None]] = []
    vals = inp[ptr:]
    if sorted(vals) != list(range(n * n)):
        die("input is not a permutation")
    for i in range(n):
        board.append(vals[i * n : (i + 1) * n])

    p = 0
    if p >= len(out):
        die("missing M")
    m = out[p]
    p += 1
    if not (0 <= m <= n * n):
        die("M out of range")

    conveyors: list[list[tuple[int, int]]] = []
    used = Counter()
    for ci in range(m):
        if p >= len(out):
            die(f"missing length for conveyor {ci}")
        l = out[p]
        p += 1
        if l < 2:
            die(f"conveyor {ci} length < 2")
        if p + 2 * l > len(out):
            die(f"missing cells for conveyor {ci}")
        cells = []
        seen = set()
        for _ in range(l):
            i, j = out[p], out[p + 1]
            p += 2
            if not (0 <= i < n and 0 <= j < n):
                die(f"cell ({i}, {j}) out of range in conveyor {ci}")
            if (i, j) in seen:
                die(f"duplicate cell in conveyor {ci}")
            seen.add((i, j))
            used[(i, j)] += 1
            if used[(i, j)] > 2:
                die(f"cell ({i}, {j}) belongs to more than 2 conveyors")
            cells.append((i, j))
        for k in range(l):
            i1, j1 = cells[k]
            i2, j2 = cells[(k + 1) % l]
            if abs(i1 - i2) + abs(j1 - j2) != 1:
                die(f"non-adjacent edge in conveyor {ci}")
        conveyors.append(cells)

    if p >= len(out):
        die("missing T")
    t = out[p]
    p += 1
    if not (0 <= t <= 100000):
        die("T out of range")
    if p + 2 * t != len(out):
        die("operation length mismatch")

    exit_cell = (0, n // 2)
    next_box = 0
    if board[exit_cell[0]][exit_cell[1]] == next_box:
        board[exit_cell[0]][exit_cell[1]] = None
        next_box += 1

    for turn in range(t):
        op_m, d = out[p], out[p + 1]
        p += 2
        if not (0 <= op_m < m):
            die(f"operation {turn}: conveyor index out of range")
        if d not in (-1, 1):
            die(f"operation {turn}: direction must be -1 or 1")

        cells = conveyors[op_m]
        old = [board[i][j] for i, j in cells]
        l = len(cells)
        for x, (i, j) in enumerate(cells):
            board[cells[(x + d) % l][0]][cells[(x + d) % l][1]] = old[x]

        ei, ej = exit_cell
        if board[ei][ej] == next_box:
            board[ei][ej] = None
            next_box += 1

    b = next_box
    if b == n * n:
        score = 1000000 + round(1000000 * math.log2(100000 / max(1, t)))
    else:
        score = round(1000000 * b / (n * n))

    print(f"AC: B={b} T={t} score={score}")


if __name__ == "__main__":
    main()

