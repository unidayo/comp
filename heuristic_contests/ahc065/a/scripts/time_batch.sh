#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
cases="${1:-100}"
jobs="${2:-4}"

./scripts/build.sh

result_dir="out/time_results"
mkdir -p "$result_dir"
rm -f "$result_dir"/*.txt

run_one() {
  seed="$1"
  input="in/seed${seed}.txt"
  output="out/seed${seed}.txt"
  log="$result_dir/$(printf '%04d' "$seed").txt"
  time_file="$result_dir/$(printf '%04d' "$seed").time"
  python3 tools/gen.py "$seed" > "$input"
  /usr/bin/time -f '%e' -o "$time_file" ./bin/main < "$input" > "$output"
  elapsed="$(cat "$time_file")"
  result="$(python3 tools/check.py "$input" "$output")"
  t="$(printf '%s\n' "$result" | sed -n 's/.*T=\([0-9][0-9]*\).*/\1/p')"
  score="$(printf '%s\n' "$result" | sed -n 's/.*score=\([0-9][0-9]*\).*/\1/p')"
  printf 'seed=%d elapsed=%s T=%s score=%s input=%s output=%s\n' "$seed" "$elapsed" "$t" "$score" "$input" "$output" > "$log"
}
export -f run_one
export result_dir

seq 0 "$((cases - 1))" | xargs -P "$jobs" -I{} bash -c 'run_one "$@"' _ {}

python3 - "$result_dir" <<'PY' | tee "$result_dir/summary.txt"
from pathlib import Path
import sys

rows = []
for path in Path(sys.argv[1]).glob("????.txt"):
    line = path.read_text().strip()
    kv = {}
    for part in line.split():
        if "=" in part:
            k, v = part.split("=", 1)
            kv[k] = v
    if kv.get("elapsed"):
        rows.append((float(kv["elapsed"]), int(kv["seed"]), line))

for _, _, line in sorted(rows, reverse=True):
    print(line)
PY
