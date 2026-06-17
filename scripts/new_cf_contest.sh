#!/bin/bash

CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"
TEMPLATE="/home/wsl2ubuntu/comp/acc-config/cpp/main.cpp"
OJ="/home/wsl2ubuntu/comp/.venv/bin/oj"

if [ $# -lt 2 ]; then
    echo "Usage: $0 <contest_id> <num_problems>" >&2
    echo "Example: $0 2080 6  -> creates a, b, c, d, e, f" >&2
    exit 1
fi

CONTEST_ID="$1"
NUM_PROBLEMS="$2"
CONTEST_DIR="$CONTESTS_DIR/cf${CONTEST_ID}"
LABELS=(a b c d e f g h i j)

if [ ! -d "$CONTESTS_DIR" ]; then
    echo "Error: contests directory not found: $CONTESTS_DIR" >&2
    exit 1
fi

if [ ! -f "$TEMPLATE" ]; then
    echo "Error: template not found: $TEMPLATE" >&2
    exit 1
fi

mkdir -p "$CONTEST_DIR"
echo "Created: $CONTEST_DIR"

FIRST_FILE=""
PIDS=()

for ((i = 0; i < NUM_PROBLEMS; i++)); do
    LABEL="${LABELS[$i]}"
    UPPER_LABEL=$(echo "$LABEL" | tr '[:lower:]' '[:upper:]')
    PROBLEM_DIR="$CONTEST_DIR/$LABEL"
    CPP_FILE="$PROBLEM_DIR/cf${CONTEST_ID}_${LABEL}.cpp"

    mkdir -p "$PROBLEM_DIR/test"

    # Copy template directly (no ACL expansion to stay within CF's 65535 char limit)
    cp "$TEMPLATE" "$CPP_FILE"
    echo "Created: $CPP_FILE"

    # Download test cases in background
    URL="https://codeforces.com/contest/${CONTEST_ID}/problem/${UPPER_LABEL}"
    (
        cd "$PROBLEM_DIR" || exit
        "$OJ" dl "$URL" -d test 2>/dev/null
        if [ $? -eq 0 ]; then
            echo "Downloaded samples: $LABEL ($URL)"
        else
            echo "Warning: could not download samples for $LABEL (login required?)" >&2
        fi
    ) &
    PIDS+=($!)

    if [ -z "$FIRST_FILE" ]; then
        FIRST_FILE="$CPP_FILE"
    fi
done

# Wait for all downloads
for pid in "${PIDS[@]}"; do
    wait "$pid"
done

echo "Done. Opening $FIRST_FILE ..."
code "$FIRST_FILE"
