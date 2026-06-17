#!/bin/bash

set -eu

ROOT="/home/wsl2ubuntu/comp"
PROBLEMS_DIR="$ROOT/problems"
TEMPLATE="$ROOT/acc-config/cpp/main.cpp"

NAME="$(date +%Y%m%d_%H%M%S)"
TARGET_DIR="$PROBLEMS_DIR/$NAME"
CPP_FILE="$TARGET_DIR/$NAME.cpp"
TEST_DIR="$TARGET_DIR/test"

mkdir -p "$TEST_DIR"

if [ ! -f "$CPP_FILE" ]; then
  cp "$TEMPLATE" "$CPP_FILE"
fi

touch "$TEST_DIR/sample-1.in" "$TEST_DIR/sample-1.out"

echo "Created: $CPP_FILE"

if command -v code >/dev/null 2>&1; then
  code "$CPP_FILE" || true
fi
