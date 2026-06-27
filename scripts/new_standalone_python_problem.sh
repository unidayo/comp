#!/bin/bash

set -eu

ROOT="/home/wsl2ubuntu/comp"
PROBLEMS_DIR="$ROOT/problems"
TEMPLATE="$ROOT/acc-config/pypy/main.py"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

NAME="$(date +%Y%m%d_%H%M%S)"
TARGET_DIR="$PROBLEMS_DIR/$NAME"
PYTHON_FILE="$TARGET_DIR/$NAME.py"
TEST_DIR="$TARGET_DIR/test"

mkdir -p "$TEST_DIR"

if [ ! -f "$PYTHON_FILE" ]; then
  cp "$TEMPLATE" "$PYTHON_FILE"
fi

touch "$TEST_DIR/sample-1.in" "$TEST_DIR/sample-1.out"

echo "Created: $PYTHON_FILE"

"$SCRIPT_DIR/open_at_main.sh" "$PYTHON_FILE"
