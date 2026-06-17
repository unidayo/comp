#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
cases="${1:-100}"
jobs="${2:-$(nproc)}"

./scripts/build.sh

result_dir="out/batch_results"
mkdir -p "$result_dir"
rm -f "$result_dir"/*.txt

run_one() {
  seed="$1"
  input="in/seed${seed}.txt"
  output="out/seed${seed}.txt"
  python3 tools/gen.py "$seed" > "$input"
  ./bin/main < "$input" > "$output"
  result="$(python3 tools/check.py "$input" "$output")"
  t="$(printf '%s\n' "$result" | sed -n 's/.*T=\([0-9][0-9]*\).*/\1/p')"
  score="$(printf '%s\n' "$result" | sed -n 's/.*score=\([0-9][0-9]*\).*/\1/p')"
  printf 'seed=%d T=%d score=%d\n' "$seed" "$t" "$score" > "$result_dir/$(printf '%04d' "$seed").txt"
}
export -f run_one
export result_dir

seq 0 "$((cases - 1))" | xargs -P "$jobs" -I{} bash -c 'run_one "$@"' _ {}

cat "$result_dir"/*.txt

awk -v cases="$cases" '
  {
    split($2, tkv, "=");
    t = tkv[2] + 0;
    sum += t;
    if (NR == 1 || t < best) best = t;
    if (t > worst) worst = t;
  }
  END {
    printf "cases=%d jobs=%d avg_T=%.2f best_T=%d worst_T=%d\n", cases, '"$jobs"', sum / cases, best, worst;
  }
' "$result_dir"/*.txt
