#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 || $# -gt 2 ]]; then
    echo "Usage: $0 SOURCE.cpp [OUTPUT]" >&2
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

if [[ ! -f "$SRC" || "${SRC##*.}" != "cpp" ]]; then
    echo "Source must be an existing .cpp file: $SRC" >&2
    exit 2
fi

src_dir="$(dirname "$SRC")"
parent="$(basename "$src_dir")"
grandparent="$(basename "$(dirname "$src_dir")")"
stem="$(basename "$SRC" .cpp)"
target="${grandparent}_${parent}_${stem}"

CHECK_FLAGS=()
if [[ "${COMP_FAST_ENABLE_CHECKS:-1}" != "0" ]]; then
    CHECK_FLAGS=(
        -D_GLIBCXX_DEBUG
        -fsanitize=undefined,address
        -fno-sanitize=leak
        -fno-omit-frame-pointer
    )
fi

BUILD_DIR="${COMP_FAST_BUILD_DIR:-$ROOT/build-fast}"
effective_pch_flags="${CHECK_FLAGS[*]} ${PCH_CXXFLAGS:-}"
pch_key_source="${COMP_FAST_PCH_KEY:-default}|$effective_pch_flags"
pch_key="${COMP_FAST_PCH_KEY:-default}_$(printf '%s' "$pch_key_source" | sha1sum | cut -c 1-12)"
PCH_DIR="$BUILD_DIR/pch/$pch_key"
PCH_HEADER="$PCH_DIR/comp_fast_pch.hpp"
PCH_GCH="$PCH_HEADER.gch"
OUT="${2:-$BUILD_DIR/$target}"

mkdir -p "$BUILD_DIR" "$PCH_DIR"
mkdir -p "$(dirname "$OUT")"

tmp_header="$(mktemp "$PCH_DIR/comp_fast_pch.XXXXXX.hpp")"
cat >"$tmp_header" <<'EOF'
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <charconv>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numbers>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <atcoder/all>
#include <ankerl/unordered_dense.h>
EOF

if [[ ! -f "$PCH_HEADER" ]] || ! cmp -s "$tmp_header" "$PCH_HEADER"; then
    mv "$tmp_header" "$PCH_HEADER"
    rm -f "$PCH_GCH"
else
    rm -f "$tmp_header"
fi

CXX="${CXX:-/opt/gcc-15.2/bin/g++}"
COMMON_FLAGS=(
    -std=c++23
    -O0
    -pipe
    -fdiagnostics-color=always
    -DLOCAL
    -DATCODER
    -I"$ROOT"
    -isystem
    /usr/lib/ac-library
)

build_key="$(
    {
        printf 'cxx=%s\n' "$CXX"
        printf 'common_flags=%s\n' "${COMMON_FLAGS[*]}"
        printf 'check_flags=%s\n' "${CHECK_FLAGS[*]}"
        printf 'pch_cxxflags=%s\n' "${PCH_CXXFLAGS:-}"
        printf 'cxxflags=%s\n' "${CXXFLAGS:-}"
        sha1sum "$SRC"
        sha1sum "$PCH_HEADER"
    } | sha1sum | cut -d ' ' -f 1
)"
META="$OUT.buildmeta"
if [[ -x "$OUT" && -f "$META" ]] && [[ "$(cat "$META")" == "$build_key" ]]; then
    echo "$OUT"
    exit 0
fi

if [[ ! -f "$PCH_GCH" || "$PCH_HEADER" -nt "$PCH_GCH" ]]; then
    echo "Building fast C++ PCH..." >&2
    "$CXX" "${COMMON_FLAGS[@]}" "${CHECK_FLAGS[@]}" ${PCH_CXXFLAGS:-} -x c++-header "$PCH_HEADER" -o "$PCH_GCH"
fi

"$CXX" "${COMMON_FLAGS[@]}" "${CHECK_FLAGS[@]}" ${PCH_CXXFLAGS:-} ${CXXFLAGS:-} -include "$PCH_HEADER" "$SRC" -o "$OUT"
printf '%s\n' "$build_key" >"$META"
echo "$OUT"
