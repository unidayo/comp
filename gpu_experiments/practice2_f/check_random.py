#!/usr/bin/env python3
import random
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent
MOD = 998244353


def run(cmd, data):
    return subprocess.check_output(cmd, input=data.encode(), cwd=ROOT).decode().strip()


def main():
    random.seed(0)
    cases = int(sys.argv[1]) if len(sys.argv) >= 2 else 100
    max_n = int(sys.argv[2]) if len(sys.argv) >= 3 else 512

    for t in range(cases):
        n = random.randint(1, max_n)
        m = random.randint(1, max_n)
        a = [random.randrange(MOD) for _ in range(n)]
        b = [random.randrange(MOD) for _ in range(m)]
        data = f"{n} {m}\n{' '.join(map(str, a))}\n{' '.join(map(str, b))}\n"

        cpu = run(["./practice2_f_cpu"], data)
        gpu = run(["./practice2_f_gpu"], data)
        if cpu != gpu:
            print(f"NG case {t}: n={n} m={m}", file=sys.stderr)
            print(data, file=sys.stderr)
            print(f"cpu={cpu}", file=sys.stderr)
            print(f"gpu={gpu}", file=sys.stderr)
            return 1

    print(f"OK: {cases} cases")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
