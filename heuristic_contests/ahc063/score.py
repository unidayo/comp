#!/usr/bin/env python3
# AHC063 スコア計算機 + シミュレーター
# 標準入力: 「入力\n---\n出力」形式
# 使い方: echo "$input\n---\n$output" | python3 score.py

import sys

lines = sys.stdin.read().splitlines()
sep = lines.index("---")
in_lines = lines[:sep]
out_lines = [l.strip() for l in lines[sep + 1:] if l.strip()]

# 入力パース
N, M, C = map(int, in_lines[0].split())
d = list(map(int, in_lines[1].split()))
f = [list(map(int, in_lines[2 + i].split())) for i in range(N)]

# ====== 色メカニクス ======
# - 移動（食事なし）: 位置 push_front + pop_back、色列は変化なし
# - 食事: 位置 push_front（尻尾残す）、食べた色を色列 push_back
# - 噛みちぎり: インデックス h で発生したら h+1 以降を餌に戻し、色列も切る

DIR = {'U': (-1, 0), 'D': (1, 0), 'L': (0, -1), 'R': (0, 1)}

snake_pos = [(4, 0), (3, 0), (2, 0), (1, 0), (0, 0)]  # head=index0
snake_col = [1, 1, 1, 1, 1]

T = 0
for line in out_lines:
    d_char = line[0]
    if d_char not in DIR:
        print(f"Invalid direction at turn {T}: {line}", file=sys.stderr)
        sys.exit(1)

    dr, dc = DIR[d_char]
    hi, hj = snake_pos[0]
    ni, nj = hi + dr, hj + dc

    if not (0 <= ni < N and 0 <= nj < N):
        print(f"Out of bounds at turn {T}: ({ni},{nj})", file=sys.stderr)
        sys.exit(1)
    if len(snake_pos) >= 2 and (ni, nj) == snake_pos[1]:
        print(f"U-turn at turn {T}", file=sys.stderr)
        sys.exit(1)

    if f[ni][nj] > 0:
        # 食事: 頭を追加、尻尾はそのまま残す、食べた色を末尾に追加
        food_color = f[ni][nj]
        f[ni][nj] = 0
        snake_pos = [(ni, nj)] + snake_pos
        snake_col = snake_col + [food_color]
        # 食事と噛みちぎりは同ターンに発生しない
    else:
        # 移動: 頭を追加、尻尾を除去、色列は変化なし
        snake_pos = [(ni, nj)] + snake_pos[:-1]

        # 噛みちぎり判定: 新しい頭が胴体（インデックス1~k-2）に重なるか
        k = len(snake_pos)
        head = snake_pos[0]
        for h in range(1, k - 1):
            if snake_pos[h] == head:
                for p in range(h + 1, k):
                    pi, pj = snake_pos[p]
                    f[pi][pj] = snake_col[p]
                snake_pos = snake_pos[:h + 1]
                snake_col = snake_col[:h + 1]
                break

    T += 1

# スコア計算
k = len(snake_pos)
E = sum(1 for p in range(k) if snake_col[p] != d[p])
score = T + 10000 * (E + 2 * (M - k))

print(score)
print(f"T={T}  k={k}  M={M}  E={E}  M-k={M-k}  penalty={10000*(E+2*(M-k))}", file=sys.stderr)
