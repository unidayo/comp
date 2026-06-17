#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
seed="${1:-0}"

./scripts/build.sh
python3 tools/gen.py "$seed" > "in/seed${seed}.txt"
./bin/main < "in/seed${seed}.txt" > "out/seed${seed}.txt"
python3 tools/check.py "in/seed${seed}.txt" "out/seed${seed}.txt"

printf 'input:  %s\n' "$(pwd)/in/seed${seed}.txt"
printf 'output: %s\n' "$(pwd)/out/seed${seed}.txt"

