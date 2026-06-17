#!/usr/bin/env python3
import random
import subprocess
import sys


def run(cmd, data):
    cwd = __file__.rsplit("/", 1)[0]
    return subprocess.check_output(cmd, input=data.encode(), cwd=cwd).decode().strip()


def main():
    random.seed(0)
    cases = int(sys.argv[1]) if len(sys.argv) >= 2 else 100
    max_n = int(sys.argv[2]) if len(sys.argv) >= 3 else 2000

    for t in range(cases):
        n = random.randint(2, max_n)
        a = [random.randint(1, 10**9) for _ in range(n)]
        data = f"{n}\n{' '.join(map(str, a))}\n"
        cpu = run(["./abc206_c_cpu"], data)
        gpu = run(["./abc206_c_gpu"], data)
        if cpu != gpu:
            print(f"NG case {t}: cpu={cpu} gpu={gpu}", file=sys.stderr)
            print(data, file=sys.stderr)
            return 1

    print(f"OK: {cases} cases")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
