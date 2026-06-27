#!/bin/bash

CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"
TEMPLATE="/home/wsl2ubuntu/comp/acc-config/cpp/main.cpp"
OJ="/home/wsl2ubuntu/comp/.venv/bin/oj"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ $# -lt 2 ]; then
    echo "Usage: $0 <contest_id> <label>" >&2
    echo "Example: $0 2080 c  -> creates contests/cf2080/c/cf2080_c.cpp" >&2
    exit 1
fi

CONTEST_ID="$1"
LABEL=$(echo "$2" | tr '[:upper:]' '[:lower:]')
UPPER_LABEL=$(echo "$LABEL" | tr '[:lower:]' '[:upper:]')
CONTEST_DIR="$CONTESTS_DIR/cf${CONTEST_ID}"
PROBLEM_DIR="$CONTEST_DIR/$LABEL"
CPP_FILE="$PROBLEM_DIR/cf${CONTEST_ID}_${LABEL}.cpp"

mkdir -p "$PROBLEM_DIR/test"
cp "$TEMPLATE" "$CPP_FILE"
echo "Created: $CPP_FILE"

URL="https://codeforces.com/contest/${CONTEST_ID}/problem/${UPPER_LABEL}"
"$OJ" dl "$URL" -d "$PROBLEM_DIR/test" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "Downloaded samples: $LABEL ($URL)"
else
    echo "Warning: could not download samples for $LABEL (login required?)" >&2
fi

"$SCRIPT_DIR/open_at_main.sh" "$CPP_FILE"
