import time

time_begin = time.perf_counter()

from typing import List, Union
from collections import deque
import heapq
import random
import math


INF = 10 ** 18
CANDIDATE = [(-1, -1), (-1, 0), (0, -1), (0, 1)]
# SWAP_COST = [100000000, 0]  # score ave: 90084
# SWAP_COST = [100000000, -10]  # score ave: 90100
# SWAP_COST = [100000000, -50]  # score ave: 90135
# SWAP_COST = [1000, -50]  # score ave: 90160
# SWAP_COST = [500, -50]  # score ave: 90233
# SWAP_COST = [300, -50]  # score ave: 90262
# SWAP_COST = [250, -50]  # score ave: 90252
# SWAP_COST = [200, -50]  # score ave: 90270
# SWAP_COST = [100, -50]  # score ave: 90195
# SWAP_COST = [200, -30]  # score ave: 90267
# SWAP_COST = [200, -70]  # score ave: 90276
# SWAP_COST = [500, -100]  # score ave: 90164
SWAP_COST_INIT = [200, -50]


def search_ball(value):
    for x in range(N):
        for y in range(x + 1):
            if balls[x][y] == value:
                return x, y


def swap_ball(x1, y1, x2, y2):
    balls[x1][y1], balls[x2][y2] = balls[x2][y2], balls[x1][y1]
    swap_list.append((x1, y1, x2, y2))


def search_path(target):
    """初期位置と、最も移動回数が少なくて済む移動先へのパスを探索して返す"""
    start_x, start_y = search_ball(target)
    x, y = start_x, start_y
    distances = [[INF] * (i + 1) for i in range(N)]
    before_direction: List[List[Union[None, tuple]]] = [[None] * (i + 1) for i in range(N)]
    que = [(0, x, y)]
    while que:
        dist, x, y = heapq.heappop(que)
        if x <= 0:
            break
        if dist > distances[x][y]:
            continue
        
        is_goal = True
        for dx, dy in CANDIDATE:
            nx = x + dx
            ny = y + dy
            if ny < 0 or ny > nx:
                continue
            n_ball = balls[nx][ny]
            if value > n_ball:
                continue
            if dx < 0:
                is_goal = False
            n_dist = dist + swap_cost[dx] + (BALL_NUM - n_ball)
            if n_dist < distances[nx][ny]:
                heapq.heappush(que, (n_dist, nx, ny))
                distances[nx][ny] = n_dist
                before_direction[nx][ny] = dx, dy
        if is_goal:
            break
    
    path = []
    while x != start_x or y != start_y:
        tmp = before_direction[x][y]
        path.append(tmp)
        dx, dy = tmp
        x -= dx
        y -= dy
    path.reverse()
    return start_x, start_y, path


N = 30
BALL_NUM = N * (N + 1) // 2
original_balls = []
for _ in range(N):
    line = list(map(int, input().split()))
    original_balls.append(line)

best_score = INF
best_answer = []
swap_cost = SWAP_COST_INIT
while time.perf_counter() - time_begin < 1.7:
    before_swap_cost = swap_cost.copy()
    swap_cost[0] += random.randrange(-50, 51)
    swap_cost[1] += random.randrange(-20, 21)
    
    balls = []
    for line in original_balls:
        balls.append(line.copy())
    
    swap_list = []
    for value in range(BALL_NUM):
        x, y, path = search_path(value)
        for dx, dy in path:
            nx = x + dx
            ny = y + dy
            swap_ball(x, y, nx, ny)
            x = nx
            y = ny
    if len(swap_list) < best_score:
        best_answer = swap_list
        best_score = len(swap_list)
    else:
        swap_cost = before_swap_cost

print(len(best_answer))
for swap in best_answer:
    print(*swap)
