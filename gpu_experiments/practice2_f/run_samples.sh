#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

run_case() {
    local name="$1"
    local input="$2"
    local expected="$3"
    local actual
    actual="$(./practice2_f_gpu <<< "$input")"
    if [ "$actual" != "$expected" ]; then
        echo "NG: $name" >&2
        echo "expected: $expected" >&2
        echo "actual:   $actual" >&2
        exit 1
    fi
    echo "OK: $name"
}

run_case "sample-1" "4 5
1 2 3 4
5 6 7 8 9" "5 16 34 60 70 70 59 36"

run_case "sample-2" "1 1
10000000
10000000" "871938225"
