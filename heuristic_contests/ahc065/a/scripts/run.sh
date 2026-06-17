#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
  echo "usage: $0 INPUT [OUTPUT]" >&2
  exit 2
fi

cd "$(dirname "$0")/.."
input="$1"
output="${2:-/dev/stdout}"

./bin/main < "$input" > "$output"

