#!/bin/bash

set -eu

CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"
CMAKE_BUILD_DIR="/home/wsl2ubuntu/comp/build"
TEMPLATE_PATH="/home/wsl2ubuntu/comp/acc-config/cpp/main.cpp"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

cd "$CONTESTS_DIR"
mkdir -p "$1"
cd "$1"
for alp in {a..h}; do
    cp "$TEMPLATE_PATH" "${1}_${alp}.cpp"
done
cmake "$CMAKE_BUILD_DIR"
find . -name "*.cpp" -print0 | sort -z | while IFS= read -r -d '' file_path; do
    sleep 0.1
    "$SCRIPT_DIR/open_at_main.sh" "$file_path"
done
