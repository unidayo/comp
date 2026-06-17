#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
cases="${1:-30}"
jobs="${2:-8}"

configs=(
  "look=8 weight=80 mode=0 beam=32"
  "look=12 weight=60 mode=0 beam=32"
  "look=16 weight=50 mode=0 beam=32"
  "look=20 weight=40 mode=0 beam=32"
  "look=26 weight=35 mode=0 beam=32"
  "look=32 weight=30 mode=0 beam=32"
  "look=16 weight=4 mode=1 beam=32"
  "look=20 weight=3 mode=1 beam=32"
  "look=26 weight=2 mode=1 beam=32"
  "look=12 weight=1 mode=2 beam=32"
  "look=16 weight=1 mode=2 beam=32"
  "look=20 weight=1 mode=2 beam=32"
)

for cfg in "${configs[@]}"; do
  eval "$cfg"
  export CXXFLAGS="-DEVAL_LOOK=$look -DEVAL_WEIGHT=$weight -DEVAL_MODE=$mode -DBEAM_WIDTH=$beam"
  printf '\nconfig look=%s weight=%s mode=%s beam=%s\n' "$look" "$weight" "$mode" "$beam"
  ./scripts/batch.sh "$cases" "$jobs" | tail -1
done

