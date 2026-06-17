#!/bin/bash
# AHC063 テスト実行スクリプト
# 使い方:
#   ./run.sh              # seed=1 で1ケース実行
#   ./run.sh 42           # seed=42 で実行
#   ./run.sh 1 10         # seed=1~10 で10ケース実行してスコア集計
#   ./run.sh 1 100 8      # seed=1~100 を8並列で実行

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
COMP_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$COMP_DIR/build-debug"
RELEASE_DIR="$COMP_DIR/build-release"
SCORE_PY="$SCRIPT_DIR/score.py"

# ビルド
echo "[build]"
ninja -C "$BUILD_DIR" ahc063_gen_gen 2>&1 | grep -v "^ninja" | grep -v "^$" || true
ninja -C "$RELEASE_DIR" ahc063_a_ahc063_a 2>&1 | grep -v "^ninja" | grep -v "^$" || true

SEED_FROM=${1:-1}
SEED_TO=${2:-$SEED_FROM}
JOBS=${3:-1}

run_one() {
    local seed=$1
    local tmpout="/tmp/ahc_out_$$_${seed}"
    local input=$("$BUILD_DIR/ahc063_gen_gen" "$seed")
    local stderr_out
    stderr_out=$(echo "$input" | timeout 2.5 "$RELEASE_DIR/ahc063_a_ahc063_a" 2>&1 1>"$tmpout")
    local output=$(cat "$tmpout")
    rm -f "$tmpout"
    local score=$(python3 "$SCORE_PY" <<< "${input}"$'\n'"---"$'\n'"${output}" 2>/dev/null || echo "?")
    local T=$(echo "$output" | wc -l)
    [ -n "$stderr_out" ] && echo "  $stderr_out" >&2
    echo "seed=$seed  score=$score  T=$T"
}

if (( JOBS > 1 )) && (( SEED_TO - SEED_FROM >= 1 )); then
    export -f run_one
    export BUILD_DIR RELEASE_DIR SCORE_PY
    RESULTS=$(seq "$SEED_FROM" "$SEED_TO" | xargs -P "$JOBS" -I{} bash -c 'run_one {}' | sort -t= -k2 -n)
    echo "$RESULTS"

    total_score=0; count=0
    while IFS= read -r line; do
        score=$(echo "$line" | grep -oP 'score=\K[0-9]+')
        if [[ -n "$score" ]]; then
            total_score=$((total_score + score))
            count=$((count + 1))
        fi
    done <<< "$RESULTS"
    if (( count > 1 )); then
        echo "---"
        echo "avg score: $((total_score / count))  (${count} cases)"
    fi
else
    total_score=0; count=0
    for seed in $(seq "$SEED_FROM" "$SEED_TO"); do
        line=$(run_one "$seed")
        echo "$line"
        score=$(echo "$line" | grep -oP 'score=\K[0-9]+')
        if [[ -n "$score" && "$score" =~ ^[0-9]+$ ]]; then
            total_score=$((total_score + score))
            count=$((count + 1))
        fi
    done
    if (( count > 1 )); then
        echo "---"
        echo "avg score: $((total_score / count))  (${count} cases)"
    fi
fi
