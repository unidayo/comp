#!/usr/bin/env python3

import math
import os
import re
import statistics
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path


SCRIPT_DIR = Path(__file__).resolve().parent
COMP_DIR = SCRIPT_DIR.parent.parent
BUILD_DEBUG = COMP_DIR / "build-debug"
DEFAULT_GEN = BUILD_DEBUG / "ahc063_gen_gen"
DEFAULT_SCORE = SCRIPT_DIR / "score.py"
BASE_ENV = {
    **os.environ,
    "ASAN_OPTIONS": "detect_leaks=0",
    "LSAN_OPTIONS": "detect_leaks=0",
}

DETAIL_RE = re.compile(r"T=(\d+)\s+k=(\d+)\s+M=(\d+)\s+E=(\d+)\s+M-k=(\d+)")


def usage() -> None:
    print(
        "usage: compare.py BIN_A BIN_B [seed_from] [seed_to] [jobs]\n"
        "example: compare.py ./bin_a ./bin_b 1 100 8",
        file=sys.stderr,
    )


def percentile(sorted_vals: list[float], p: float) -> float:
    if not sorted_vals:
        return math.nan
    idx = (len(sorted_vals) - 1) * p
    lo = math.floor(idx)
    hi = math.ceil(idx)
    if lo == hi:
        return sorted_vals[lo]
    frac = idx - lo
    return sorted_vals[lo] * (1.0 - frac) + sorted_vals[hi] * frac


def run_case(seed: int, solver: str) -> tuple[int, int, int, int]:
    input_text = subprocess.check_output([str(DEFAULT_GEN), str(seed)], text=True, env=BASE_ENV)
    proc = subprocess.run(
        [solver],
        input=input_text,
        text=True,
        capture_output=True,
        timeout=2.5,
        env=BASE_ENV,
    )
    output = proc.stdout
    score_proc = subprocess.run(
        [sys.executable, str(DEFAULT_SCORE)],
        input=input_text + "\n---\n" + output,
        text=True,
        capture_output=True,
    )
    if score_proc.returncode != 0:
        raise RuntimeError(
            f"score failed for seed={seed}, solver={solver}\n"
            f"solver stderr:\n{proc.stderr}\nscore stderr:\n{score_proc.stderr}"
        )
    score = int(score_proc.stdout.strip().splitlines()[0])
    m = DETAIL_RE.search(score_proc.stderr)
    if not m:
        raise RuntimeError(f"detail parse failed for seed={seed}, solver={solver}\n{score_proc.stderr}")
    T, _k, _M, E, _mk = map(int, m.groups())
    return score, T, E, len(output.splitlines())


def main() -> int:
    if len(sys.argv) < 3:
        usage()
        return 2

    bin_a = sys.argv[1]
    bin_b = sys.argv[2]
    seed_from = int(sys.argv[3]) if len(sys.argv) >= 4 else 1
    seed_to = int(sys.argv[4]) if len(sys.argv) >= 5 else seed_from
    jobs = int(sys.argv[5]) if len(sys.argv) >= 6 else 4
    seeds = list(range(seed_from, seed_to + 1))

    def work(seed: int):
        a_score, a_T, a_E, _ = run_case(seed, bin_a)
        b_score, b_T, b_E, _ = run_case(seed, bin_b)
        return {
            "seed": seed,
            "a_score": a_score,
            "b_score": b_score,
            "diff": b_score - a_score,
            "ratio": (b_score - a_score) / a_score,
            "a_T": a_T,
            "b_T": b_T,
            "a_E": a_E,
            "b_E": b_E,
        }

    with ThreadPoolExecutor(max_workers=jobs) as ex:
        rows = list(ex.map(work, seeds))

    rows.sort(key=lambda r: r["seed"])
    wins_a = sum(1 for r in rows if r["a_score"] < r["b_score"])
    wins_b = sum(1 for r in rows if r["b_score"] < r["a_score"])
    draws = len(rows) - wins_a - wins_b

    diffs = [r["diff"] for r in rows]
    ratios = [r["ratio"] for r in rows]
    sorted_diffs = sorted(diffs)
    sorted_ratios = sorted(ratios)

    avg_a = sum(r["a_score"] for r in rows) / len(rows)
    avg_b = sum(r["b_score"] for r in rows) / len(rows)
    avg_a_T = statistics.mean(r["a_T"] for r in rows)
    avg_b_T = statistics.mean(r["b_T"] for r in rows)
    med_a_T = statistics.median(r["a_T"] for r in rows)
    med_b_T = statistics.median(r["b_T"] for r in rows)
    avg_diff = statistics.mean(diffs)
    med_diff = statistics.median(diffs)
    avg_ratio = statistics.mean(ratios)
    med_ratio = statistics.median(ratios)
    p25_diff = percentile(sorted_diffs, 0.25)
    p75_diff = percentile(sorted_diffs, 0.75)

    avg_delta_E = statistics.mean(r["b_E"] - r["a_E"] for r in rows)
    avg_delta_T = statistics.mean(r["b_T"] - r["a_T"] for r in rows)

    print(f"cases={len(rows)}  seeds={seed_from}-{seed_to}")
    print(f"A={bin_a}")
    print(f"B={bin_b}")
    print("---")
    print(f"avg score A: {avg_a:.1f}")
    print(f"avg score B: {avg_b:.1f}")
    print(f"avg T A: {avg_a_T:.1f}")
    print(f"avg T B: {avg_b_T:.1f}")
    print(f"median T A: {med_a_T:.1f}")
    print(f"median T B: {med_b_T:.1f}")
    print(f"avg diff (B-A): {avg_diff:.1f}")
    print(f"median diff (B-A): {med_diff:.1f}")
    print(f"diff IQR (B-A): [{p25_diff:.1f}, {p75_diff:.1f}]")
    print(f"avg ratio (B-A)/A: {avg_ratio:.6f}")
    print(f"median ratio (B-A)/A: {med_ratio:.6f}")
    print(f"wins: A={wins_a}  B={wins_b}  draws={draws}")
    print(f"avg delta E (B-A): {avg_delta_E:.3f}")
    print(f"avg delta T (B-A): {avg_delta_T:.3f}")
    print("---")
    for r in rows:
        print(
            f"seed={r['seed']}  A={r['a_score']}  B={r['b_score']}  "
            f"diff={r['diff']}  ratio={r['ratio']:.6f}  "
            f"E:{r['a_E']}->{r['b_E']}  T:{r['a_T']}->{r['b_T']}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
