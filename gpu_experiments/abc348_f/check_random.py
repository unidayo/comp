#!/usr/bin/env python3
import random
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent


def run(cmd, data):
    return subprocess.check_output(cmd, input=data.encode(), cwd=ROOT).decode().strip()


def main():
    random.seed(0)
    cases = int(sys.argv[1]) if len(sys.argv) >= 2 else 100
    max_n = int(sys.argv[2]) if len(sys.argv) >= 3 else 200
    max_m = int(sys.argv[3]) if len(sys.argv) >= 4 else 200

    for t in range(cases):
        n = random.randint(1, max_n)
        m = random.randint(1, max_m)
        a = [[random.randint(1, 999) for _ in range(m)] for _ in range(n)]
        lines = [f"{n} {m}"]
        lines += [" ".join(map(str, row)) for row in a]
        data = "\n".join(lines) + "\n"

        cpu = run(["./abc348_f_cpu"], data)
        gpu = run(["./abc348_f_gpu"], data)
        if cpu != gpu:
            print(f"NG case {t}: n={n} m={m} cpu={cpu} gpu={gpu}", file=sys.stderr)
            print(data, file=sys.stderr)
            return 1

    print(f"OK: {cases} cases")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
