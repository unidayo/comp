#!/bin/bash

CONTESTS_DIR="/home/wsl2ubuntu/comp/contests"

if [ $# -eq 0 ]; then
  echo "エラー: 引数が必要です" >&2
  exit 1
fi

CONTEST_ID="$1"

cd "$CONTESTS_DIR" || exit 1

# 問題1つだけ取得
acc new "$CONTEST_ID" -c none
cd "$CONTEST_ID"

acc add -c inquire

TASK_ID=$(ls -d */ | sed 's|/||' | tail -n 1)
cd "$TASK_ID" || exit 1

CPP_FILE=$(ls -t *.cpp | head -n 1)

code "$CPP_FILE"
