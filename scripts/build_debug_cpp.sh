#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 SOURCE.cpp" >&2
    exit 2
fi

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="$1"
if [[ "$SRC" != /* ]]; then
    SRC="$PWD/$SRC"
fi
SRC="$(realpath "$SRC")"

case "$SRC" in
    "$ROOT"/*) ;;
    *)
        echo "Source must be under $ROOT: $SRC" >&2
        exit 2
        ;;
esac

src_dir="$(dirname "$SRC")"
parent="$(basename "$src_dir")"
grandparent="$(basename "$(dirname "$src_dir")")"
stem="$(basename "$SRC" .cpp)"
target="${grandparent}_${parent}_${stem}"

out_dir="$ROOT/build-debug"
out="$out_dir/$target"
debug_flags="${DEBUG_CXXFLAGS:--g}"
pch_key="debug_$(printf '%s' "$debug_flags" | sha1sum | cut -c 1-12)"

COMP_FAST_ENABLE_CHECKS=1 COMP_FAST_PCH_KEY="$pch_key" PCH_CXXFLAGS="$debug_flags" "$ROOT/scripts/build_fast_cpp.sh" "$SRC" "$out" >/dev/null
ln -sf "$target" "$out_dir/$stem"
echo "$out_dir/$stem"
