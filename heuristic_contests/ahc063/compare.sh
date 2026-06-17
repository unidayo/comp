#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if (( $# < 2 )); then
    echo "usage: ./compare.sh BIN_A BIN_B [seed_from] [seed_to] [jobs]" >&2
    exit 2
fi

python3 "$SCRIPT_DIR/compare.py" "$@"
