#!/bin/bash
set -euo pipefail

CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

if [ $# -eq 0 ]; then
  echo "エラー: 引数が必要です" >&2
  exit 1
fi

CONTEST_ID="$1"

cd "$CONTESTS_DIR" || exit 1

if [ ! -d "$CONTEST_ID" ]; then
  # 問題1つだけ取得するため、コンテストだけ作ってタスクは後で選ぶ
  acc new "$CONTEST_ID" -c none
fi

cd "$CONTEST_ID"

TMP_PREFIX="__new_problem_tmp_$$_$(date +%Y%m%d%H%M%S)"
acc add -c inquire -f -t "${TMP_PREFIX}_{tasklabel}" < /dev/tty

mapfile -t TMP_DIRS < <(find . -maxdepth 1 -type d -name "${TMP_PREFIX}_*" | sort)
if [ "${#TMP_DIRS[@]}" -eq 0 ]; then
  echo "エラー: 作成された問題ディレクトリが見つかりません" >&2
  exit 1
fi

OPEN_FILE=""
for TMP_DIR in "${TMP_DIRS[@]}"; do
  TMP_NAME="$(basename "$TMP_DIR")"
  TASK_ID="${TMP_NAME#${TMP_PREFIX}_}"
  DEST_DIR="$TASK_ID"

  if [ -e "$DEST_DIR" ]; then
    index=2
    while [ -e "${TASK_ID}_${index}" ]; do
      index=$((index + 1))
    done
    DEST_DIR="${TASK_ID}_${index}"
  fi

  mv "$TMP_DIR" "$DEST_DIR"

  python3 - "$TMP_NAME" "$DEST_DIR" <<'PY'
import json
import sys
from pathlib import Path

tmp_name = sys.argv[1]
dest_dir = sys.argv[2]
path = Path("contest.acc.json")

data = json.loads(path.read_text())
updated = False
for task in reversed(data.get("tasks", [])):
    directory = task.get("directory")
    if isinstance(directory, dict) and directory.get("path") == tmp_name:
        directory["path"] = dest_dir
        updated = True
        break

if not updated:
    raise SystemExit(f"contest.acc.json に {tmp_name} の項目が見つかりません")

path.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n")
PY

  CPP_FILE=$(find "$DEST_DIR" -maxdepth 1 -type f -name '*.cpp' -printf '%f\n' | sort | head -n 1)
  if [ -z "$CPP_FILE" ]; then
    echo "警告: $DEST_DIR に .cpp が見つかりません" >&2
    continue
  fi

  echo "Created: $CONTESTS_DIR/$CONTEST_ID/$DEST_DIR/$CPP_FILE"
  if [ -z "$OPEN_FILE" ]; then
    OPEN_FILE="$CONTESTS_DIR/$CONTEST_ID/$DEST_DIR/$CPP_FILE"
  fi
done

if [ -n "$OPEN_FILE" ]; then
  "$SCRIPT_DIR/open_at_main.sh" "$OPEN_FILE"
fi
