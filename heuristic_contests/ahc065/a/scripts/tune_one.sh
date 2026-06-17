#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

look="${1:?usage: $0 LOOK [CASES] [JOBS]}"
cases="${2:-100}"
jobs="${3:-8}"
weight="${EVAL_WEIGHT_OVERRIDE:-30}"
mode="${EVAL_MODE_OVERRIDE:-0}"
beam="${BEAM_WIDTH_OVERRIDE:-300}"

tmp="out/tune_look${look}_w${weight}_m${mode}_b${beam}"
mkdir -p "$tmp/in" "$tmp/out" "$tmp/result"
rm -f "$tmp/result"/*.txt

bin="$tmp/main"
g++ -std=gnu++23 -O2 -pipe -Wall -Wextra -DLOCAL \
  -DEVAL_LOOK="$look" -DEVAL_WEIGHT="$weight" -DEVAL_MODE="$mode" -DBEAM_WIDTH="$beam" \
  main.cpp -o "$bin"

run_one() {
  seed="$1"
  input="$tmp/in/seed${seed}.txt"
  output="$tmp/out/seed${seed}.txt"
  python3 tools/gen.py "$seed" > "$input"
  "$bin" < "$input" > "$output"
  result="$(python3 tools/check.py "$input" "$output")"
  t="$(printf '%s\n' "$result" | sed -n 's/.*T=\([0-9][0-9]*\).*/\1/p')"
  score="$(printf '%s\n' "$result" | sed -n 's/.*score=\([0-9][0-9]*\).*/\1/p')"
  printf 'seed=%d T=%d score=%d\n' "$seed" "$t" "$score" > "$tmp/result/$(printf '%04d' "$seed").txt"
}
export -f run_one
export tmp bin

seq 0 "$((cases - 1))" | xargs -P "$jobs" -I{} bash -c 'run_one "$@"' _ {}

awk -v cases="$cases" -v look="$look" -v weight="$weight" -v mode="$mode" -v beam="$beam" '
  {
    split($2, tkv, "=");
    t = tkv[2] + 0;
    sum += t;
    if (NR == 1 || t < best) best = t;
    if (t > worst) worst = t;
  }
  END {
    printf "look=%d weight=%d mode=%d beam=%d cases=%d avg_T=%.2f best_T=%d worst_T=%d\n", look, weight, mode, beam, cases, sum / cases, best, worst;
  }
' "$tmp/result"/*.txt

