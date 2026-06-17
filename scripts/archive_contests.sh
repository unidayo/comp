#!/usr/bin/env bash
set -euo pipefail
shopt -s nullglob

ROOT="/home/wsl2ubuntu/comp"
targets=(
    "$ROOT"/contests/*/
    "$ROOT"/problems/*/
)

if ((${#targets[@]} == 0)); then
    echo "No contests or problems to archive."
    exit 0
fi

echo "Archive ${#targets[@]} contest/problem directories and remove build-debug/build-release."
read -r -p "Continue? [y/N] " answer
case "$answer" in
    [yY] | [yY][eE][sS]) ;;
    *)
        echo "Canceled."
        exit 0
        ;;
esac

dst="$ROOT/archive/contests_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$ROOT/archive"
mkdir "$dst"

mv "${targets[@]}" "$dst/"

rm -rf -- "$ROOT/build-debug" "$ROOT/build-release"
echo "Archived contests and problems to $dst"
echo "Removed build-debug and build-release"
