#!/bin/bash
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"
BIN="$ROOT/a/main"
SRC="$ROOT/a/main.cpp"
IN_DIR="$ROOT/cases"
OUT_DIR="$ROOT/out"

mkdir -p "$OUT_DIR"

if [ ! -x "$BIN" ] || [ "$SRC" -nt "$BIN" ]; then
  /opt/gcc-15.2/bin/g++ -std=c++23 -O2 -Wall -Wextra "$SRC" -o "$BIN"
fi

INPUT="${1:-$IN_DIR/0000.txt}"
BASENAME="$(basename "$INPUT")"
OUTPUT="$OUT_DIR/$BASENAME"

"$BIN" < "$INPUT" > "$OUTPUT"

echo "$OUTPUT"
