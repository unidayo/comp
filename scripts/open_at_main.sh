#!/bin/bash

set -u

if [ $# -eq 0 ]; then
    echo "Usage: $0 <file>" >&2
    exit 1
fi

FILE="$1"

if ! command -v code >/dev/null 2>&1; then
    exit 0
fi

LINE=$(awk '
    /^[[:space:]]*(int|signed|void)[[:space:]]+main[[:space:]]*\(/ { print NR; found = 1; exit }
    /^[[:space:]]*def[[:space:]]+main[[:space:]]*\(/ { print NR; found = 1; exit }
    END { if (!found) print 1 }
' "$FILE")

code -g "$FILE:$LINE:1" || code "$FILE" || true
