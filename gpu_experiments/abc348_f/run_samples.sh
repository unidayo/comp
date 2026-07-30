#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

run_case() {
    local name="$1"
    local input="$2"
    local expected="$3"
    local actual
    actual="$(./abc348_f_gpu <<< "$input")"
    if [ "$actual" != "$expected" ]; then
        echo "NG: $name" >&2
        echo "expected: $expected" >&2
        echo "actual:   $actual" >&2
        exit 1
    fi
    echo "OK: $name"
}

run_case "sample-1" "3 3
1 2 3
1 3 4
2 3 4" "1"

run_case "sample-2" "6 5
8 27 27 10 24
27 8 2 4 5
15 27 26 17 24
27 27 27 27 27
27 7 22 11 27
19 27 27 27 27" "5"
