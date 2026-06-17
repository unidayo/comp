#!/bin/bash
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"
BIN="$ROOT/a/main"
SRC="$ROOT/a/main.cpp"
CASE_DIR="${1:-$ROOT/cases}"
OUT_DIR="${2:-$ROOT/out}"
JOBS="${JOBS:-$(nproc)}"

mkdir -p "$OUT_DIR"

if [ ! -x "$BIN" ] || [ "$SRC" -nt "$BIN" ]; then
  /opt/gcc-15.2/bin/g++ -std=c++23 -O2 -Wall -Wextra "$SRC" -o "$BIN"
fi

find "$CASE_DIR" -maxdepth 1 -type f -name '[0-9][0-9][0-9][0-9].txt' | sort |
  xargs -P "$JOBS" -I {} bash -c '
    in="$1"
    out_dir="$2"
    bin="$3"
    base="$(basename "$in")"
    "$bin" < "$in" > "$out_dir/$base"
  ' _ {} "$OUT_DIR" "$BIN"

echo "outputs: $OUT_DIR"
