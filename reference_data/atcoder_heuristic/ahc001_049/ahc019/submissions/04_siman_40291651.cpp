#include <cassert>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <climits>
#include <map>
#include <queue>
#include <set>
#include <cstring>
#include <vector>

using namespace std;
using namespace std::chrono;
typedef long long ll;

const ll CYCLE_PER_SEC = 2700000000;
double TIME_LIMIT = 5.8;

unsigned long long xor128() {
  static unsigned long long rx = 123456789, ry = 362436069, rz = 521288629, rw = 88675123;
  unsigned long long rt = (rx ^ (rx << 11));
  rx = ry;
  ry = rz;
  rz = rw;
  return (rw = (rw ^ (rw >> 19)) ^ (rt ^ (rt >> 8)));
}

unsigned long long int get_cycle() {
  unsigned int low, high;
  __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
  return ((unsigned long long int) low) | ((unsigned long long int) high << 32);
}

double get_time(unsigned long long int begin_cycle) {
  return (double) (get_cycle() - begin_cycle) / CYCLE_PER_SEC;
}

double get_time(high_resolution_clock::time_point &begin) {
  high_resolution_clock::time_point end = high_resolution_clock::now();
  ll count = duration_cast<microseconds>(end - begin).count();

  return count / 1000000.0;
}

const ll BASE_SCORE = 1000000000;
const int MAX_D = 14;
const int GRID_SIZE = MAX_D + 2;
const int CUBE_SIZE = GRID_SIZE * GRID_SIZE * GRID_SIZE;
const int NOT_FOUND = -1;
const int WALL = -1;
const int EMPTY = 0;
const int XZ_MASK = 3855;
const int YZ_MASK = 255;

const int FRONT = -GRID_SIZE;
const int BACK = GRID_SIZE;
const int LEFT = -1;
const int RIGHT = 1;
const int UP = -1 * GRID_SIZE * GRID_SIZE;
const int DOWN = GRID_SIZE * GRID_SIZE;

const int ROT_NUM = 24;

const int DT_ORIGIN[6] = {FRONT, RIGHT, BACK, LEFT, UP, DOWN};
const int DT[2][ROT_NUM][6] = {
  {
    {FRONT, RIGHT, BACK, LEFT, UP,    DOWN},
    {FRONT, UP,   BACK, DOWN,  LEFT, RIGHT},
    {FRONT, LEFT, BACK, RIGHT, DOWN, UP},
    {FRONT, DOWN,  BACK, UP,   RIGHT, LEFT},

    {RIGHT, BACK, LEFT, FRONT, UP,   DOWN},
    {RIGHT, UP,    LEFT, DOWN, FRONT, BACK},
    {RIGHT, FRONT, LEFT, BACK, DOWN,  UP},
    {RIGHT, DOWN, LEFT, UP,    BACK, FRONT},

    {BACK, UP, FRONT, DOWN, RIGHT, LEFT},
    {BACK, LEFT, FRONT, RIGHT, UP,   DOWN},
    {BACK, RIGHT, FRONT, LEFT, DOWN,  UP},
    {BACK, DOWN,  FRONT, UP,   LEFT, RIGHT},

    {LEFT, UP, RIGHT, DOWN, BACK,  FRONT},
    {LEFT, FRONT, RIGHT, BACK, UP,   DOWN},
    {LEFT, DOWN, RIGHT, UP, FRONT, BACK},
    {LEFT, BACK, RIGHT, FRONT, DOWN, UP},

    {UP, LEFT, DOWN, RIGHT, FRONT, BACK},
    {UP, RIGHT, DOWN, LEFT,  BACK, FRONT},
    {UP, BACK,  DOWN, FRONT, LEFT, RIGHT},
    {UP, FRONT, DOWN, BACK, RIGHT, LEFT},

    {DOWN, LEFT,  UP, RIGHT, BACK,  FRONT},
    {DOWN, RIGHT, UP, LEFT, FRONT, BACK},
    {DOWN, FRONT, UP, BACK,  LEFT, RIGHT},
    {DOWN, BACK, UP, FRONT, RIGHT, LEFT},
  },
  {
    {FRONT, UP,    BACK, DOWN, RIGHT, LEFT},
    {FRONT, LEFT, BACK, RIGHT, UP,   DOWN},
    {FRONT, DOWN, BACK, UP,    LEFT, RIGHT},
    {FRONT, RIGHT, BACK, LEFT, DOWN,  UP},

    {RIGHT, UP,   LEFT, DOWN,  BACK, FRONT},
    {RIGHT, FRONT, LEFT, BACK, UP,    DOWN},
    {RIGHT, DOWN,  LEFT, UP,   FRONT, BACK},
    {RIGHT, BACK, LEFT, FRONT, DOWN, UP},

    {BACK, UP, FRONT, DOWN, LEFT,  RIGHT},
    {BACK, LEFT, FRONT, RIGHT, DOWN, UP},
    {BACK, DOWN,  FRONT, UP,   RIGHT, LEFT},
    {BACK, RIGHT, FRONT, LEFT, UP,   DOWN},

    {LEFT, UP, RIGHT, DOWN, FRONT, BACK},
    {LEFT, FRONT, RIGHT, BACK, DOWN, UP},
    {LEFT, DOWN, RIGHT, UP, BACK,  FRONT},
    {LEFT, BACK, RIGHT, FRONT, UP,   DOWN},

    {UP, BACK, DOWN, FRONT, RIGHT, LEFT},
    {UP, LEFT,  DOWN, RIGHT, BACK, FRONT},
    {UP, FRONT, DOWN, BACK,  LEFT, RIGHT},
    {UP, RIGHT, DOWN, LEFT, FRONT, BACK},

    {DOWN, FRONT, UP, BACK,  RIGHT, LEFT},
    {DOWN, RIGHT, UP, LEFT, BACK,  FRONT},
    {DOWN, BACK,  UP, FRONT, LEFT, RIGHT},
    {DOWN, LEFT, UP, RIGHT, FRONT, BACK},
  },
};

double E_POWER[15][4] = {
  {1.0, 1.0,  1.0,  1.0},
  {1.0, 1.0,  1.0,  1.0},
  {1.0, 1.0,  1.0,  1.0},
  {1.0, 1.0,  1.0,  1.0},
  {1.0, 1.0,  1.0,  1.0},
  {1.0, 0.78, 3.23, 5.71},
  {1.0, 0.78, 3.23, 5.71},
  {1.0, 0.58, 3.89, 4.25},
  {1.0, 1.95, 2.70, 3.35},
  {1.0, 1.60, 2.50, 3.25},
  {1.0, 0.05, 2.25, 2.75},
  {1.0, 1.55, 2.35, 2.85},
  {1.0, 1.56, 2.83, 3.07},
  {1.0, 1.28, 2.94, 3.39},
  {1.0, 1.92, 2.28, 2.63},
};

int D;
int F[2][GRID_SIZE][GRID_SIZE];
int R[2][GRID_SIZE][GRID_SIZE];

ll g_blocks[2][CUBE_SIZE];
ll g_blocks_origin[2][CUBE_SIZE];
int g_block_counter[2][2][GRID_SIZE][GRID_SIZE];
vector<int> g_empty_space_list[2];
vector<int> g_empty_spaces[2];
double g_panel_bonus_origin[4 * CUBE_SIZE];
double g_panel_bonus[4 * CUBE_SIZE];
ll g_visited[GRID_SIZE * GRID_SIZE * GRID_SIZE];
ll g_visited_space[4 * CUBE_SIZE];
ll g_vid;
ll g_removable[2][CUBE_SIZE];
ll g_rid;
map <ll, ll> g_checked;
ll g_cid;

int calc_t(int x, int y, int z) {
  return (x << 8) + (y << 4) + z;
}

struct Parameters {
  int x;
  int y;

  Parameters() {
    this->x = 27;
    this->y = 85;
  }
};

Parameters params;

struct Block {
  int id;
  int size;

  Block(int id = -1, int size = -1) {
    this->id = id;
    this->size = size;
  }

  bool operator>(const Block &b) const {
    return size > b.size;
  }
};

struct Point {
  int x;
  int y;
  int z;

  Point(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }
};

struct Command {
  int block_id;
  int t0;
  int rot0;
  int t1;
  int rot1;
  int rot_type;

  Command(int block_id = -1, int t0 = -1, int rot0 = -1, int t1 = -1, int rot1 = -1, int rot_type = -1) {
    this->block_id = block_id;
    this->t0 = t0;
    this->rot0 = rot0;
    this->t1 = t1;
    this->rot1 = rot1;
    this->rot_type = rot_type;
  }
};

ll build_key(ll block_id, ll t0, ll rot0, ll t1, ll rot1, ll rot_type) {
  assert(t1 < 4096);
  assert(block_id >= 1);
  return (block_id << 35) + (t0 << 23) + (rot0 << 18) + (t1 << 6) + (rot1 << 1) + rot_type;
}

int get_block_id(ll key) {
  return key >> 35;
}

Command decode_key(ll key) {
  int block_id = key / (1LL << 35);
  key %= (1LL << 35);
  int t0 = key / (1LL << 23);
  key %= (1LL << 23);
  int rot0 = key / (1LL << 18);
  key %= (1LL << 18);
  int t1 = key / (1LL << 6);
  key %= (1LL << 6);
  int rot1 = key / (1L << 1);
  key %= (1LL << 1);
  int rot_type = key;

  return Command(block_id, t0, rot0, t1, rot1, rot_type);
}

Point calc_point(int t) {
  int x = t >> 8;
  int y = t >> 4 & 15;
  int z = t & 15;

  return Point(x, y, z);
}

class SilhouetteBlockPuzzleCreation {
public:
  void setup_parameters() {
    params.x = stoi(getenv("MM_X_VALUE"));
    params.y = stoi(getenv("MM_Y_VALUE"));
  }

  void load_data() {
    memset(F, 0, sizeof(F));
    memset(R, 0, sizeof(R));
    memset(g_blocks, WALL, sizeof(g_blocks));
    cin >> D;

    fprintf(stderr, "D = %d\n", D);

    for (int i = 0; i < 2; ++i) {
      for (int y = 1; y <= D; ++y) {
        string row;
        cin >> row;

        for (int x = 1; x <= D; ++x) {
          F[i][y][x] = row[x - 1] - '0';
        }
      }
      for (int y = 1; y <= D; ++y) {
        string row;
        cin >> row;

        for (int x = 1; x <= D; ++x) {
          R[i][y][x] = row[x - 1] - '0';
        }
      }
    }
  }

  void setup() {
    memset(g_removable, -1, sizeof(g_removable));
    memset(g_panel_bonus_origin, 0, sizeof(g_panel_bonus_origin));
    memset(g_panel_bonus, 0, sizeof(g_panel_bonus));
    memset(g_visited_space, 0, sizeof(g_visited_space));
    memset(g_visited, -1, sizeof(g_visited));
    memset(g_block_counter, 0, sizeof(g_block_counter));
    g_vid = 0;
    g_cid = 0;
    g_rid = 0;
    const int DZ[4] = {-1, 0, 1, 0};
    const int DX[4] = {0, 1, 0, -1};
    const int DY[4] = {0, 1, 0, -1};
    double P_BONUS[5] = {1.0, 1.0, 1.0, 1.0, 1.0};
    P_BONUS[1] = pow(D, E_POWER[D][1]);
    P_BONUS[2] = pow(D, E_POWER[D][2]);
    P_BONUS[3] = pow(D, E_POWER[D][3]);
    int valid_space_counter[2][2][GRID_SIZE][GRID_SIZE];
    memset(valid_space_counter, 0, sizeof(valid_space_counter));
    for (int type = 0; type < 2; ++type) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          for (int z = 1; z <= D; ++z) {
            int v1 = F[type][z][x];
            int v2 = R[type][z][y];

            if (v1 == 1 && v2 == 1) {
              valid_space_counter[type][0][z][x]++;
              valid_space_counter[type][1][z][y]++;
            }
          }
        }
      }
    }

    for (int type = 0; type < 2; ++type) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          for (int z = 1; z <= D; ++z) {
            int t = calc_t(x, y, z);
            int v1 = F[type][z][x];
            int v2 = R[type][z][y];

            int ec0 = 0;
            int ec1 = 0;
            for (int dir = 0; dir < 4; ++dir) {
              int nz = z + DZ[dir];
              int nx = x + DX[dir];
              int ny = y + DY[dir];

              if (F[type][nz][nx] == 0) {
                ec0 += 1;
              }
              if (R[type][nz][ny] == 0) {
                ec1 += 1;
              }
            }

            if (v1 == 1 && v2 == 1) {
              g_blocks[type][t] = EMPTY;
              g_panel_bonus[2 * type * CUBE_SIZE + (t & XZ_MASK)] =
                P_BONUS[ec0] * pow(D - valid_space_counter[type][0][z][x] + 1, 2);
              g_panel_bonus[2 * type * CUBE_SIZE + CUBE_SIZE + (t & YZ_MASK)] =
                P_BONUS[ec1] * pow(D - valid_space_counter[type][1][z][y] + 1, 2);
              int push_cnt = max(1.0, pow(ec0 * ec1, 2));

              g_empty_space_list[type].push_back(t);
              for (int c = 0; c < push_cnt; ++c) {
                g_empty_spaces[type].push_back(t);
              }
            }
          }
        }
      }
    }

    memcpy(g_panel_bonus_origin, g_panel_bonus, sizeof(g_panel_bonus));
    memcpy(g_blocks_origin, g_blocks, sizeof(g_blocks));
  }

  ll select_random_block_id(int type) {
    int x, y, z, t;
    int limit = 10000;

    do {
      x = xor128() % D + 1;
      y = xor128() % D + 1;
      z = xor128() % D + 1;
      t = calc_t(x, y, z);
      --limit;
    } while (g_blocks[type][t] == EMPTY && limit > 0);

    if (g_blocks[type][t] != EMPTY) {
      return g_blocks[type][t];
    } else {
      return NOT_FOUND;
    }
  }

  int select_random_point(int type) {
    int x, y, z, t;
    int limit = D * D;
    int L = g_empty_spaces[type].size();

    do {
      t = g_empty_spaces[type][xor128() % L];
      Point p = calc_point(t);
      x = p.x;
      y = p.y;
      z = p.z;
      --limit;
    } while ((
               (g_block_counter[type][0][z][x] > 0 || g_block_counter[type][1][z][y] > 0) ||
               g_blocks[type][t] != EMPTY) &&
             limit > 0);

    if (limit > 0) return t;
    limit = D * D;

    do {
      t = g_empty_spaces[type][xor128() % L];
      Point p = calc_point(t);
      x = p.x;
      y = p.y;
      z = p.z;
      --limit;
    } while ((
               (g_block_counter[type][0][z][x] > 0 && g_block_counter[type][1][z][y] > 0) ||
               g_blocks[type][t] != EMPTY) &&
             limit > 0);
    if (limit > 0) return t;

    int min_value = INT_MAX;
    int best_t = NOT_FOUND;

    for (int t : g_empty_space_list[type]) {
      if (g_blocks[type][t] != EMPTY) continue;
      int x = t >> 8;
      int y = t >> 4 & 15;
      int z = t & 15;
      int value = 27 * (g_block_counter[type][0][z][x] + g_block_counter[type][1][z][y]);

      for (int dir = 0; dir < 6; ++dir) {
        int nt = t + DT_ORIGIN[dir];
        if (g_blocks[type][nt] != EMPTY) value += 85;
      }

      if (min_value > value) {
        min_value = value;
        best_t = t;
      }
    }

    return best_t;
  }

  void update_removable_blocks() {
    ++g_rid;

    for (int type = 0; type < 2; ++type) {
      for (int t : g_empty_space_list[type]) {
        if (g_blocks[type][t] <= EMPTY) continue;
        int block_id = get_block_id(g_blocks[type][t]);

        int dirs[6] = {NOT_FOUND, NOT_FOUND, NOT_FOUND, NOT_FOUND, NOT_FOUND, NOT_FOUND};
        int same_cnt = 0;
        for (int dir = 0; dir < 6; ++dir) {
          int nt = t + DT_ORIGIN[dir];
          if (get_block_id(g_blocks[type][nt]) == block_id) {
            dirs[same_cnt] = DT_ORIGIN[dir];
            ++same_cnt;
          }
        }

        if (same_cnt == 1) {
          g_removable[type][t] = g_rid;
        } else if (2 <= same_cnt && same_cnt <= 4) {
          int connect_cnt = 0;
          for (int i = 0; i < same_cnt; ++i) {
            for (int j = i + 1; j < same_cnt; ++j) {
              int nt = t + dirs[i] + dirs[j];
              if (nt != t && get_block_id(g_blocks[type][nt]) == block_id) ++connect_cnt;
            }
          }

          if (connect_cnt >= same_cnt - 1) g_removable[type][t] = g_rid;
        }
      }
    }
  }

  int search_best_blocks(high_resolution_clock::time_point &begin) {
    int block_id = 1;
    ll min_score = LLONG_MAX;
    ll best_blocks[2][GRID_SIZE * GRID_SIZE * GRID_SIZE];
    double cur_time = get_time(begin);
    ll loop_cnt = 0;
    int rot_type = 0;
    ll cur_score = 0;
    memcpy(g_panel_bonus, g_panel_bonus_origin, sizeof(g_panel_bonus_origin));

    while (cur_time < TIME_LIMIT) {
      cur_time = get_time(begin);
      int t0 = select_random_point(0);
      int t1 = select_random_point(1);

      if (t0 == NOT_FOUND || t1 == NOT_FOUND) {
        for (int j = 0; j < 2; ++j) {
          ll target_id = select_random_block_id(0);
          if (target_id == NOT_FOUND) break;
          int size = erase_block_all(get_block_id(target_id));
          cur_score -= BASE_SCORE / size;
        }
        rot_type = (rot_type + 1) % 2;
        continue;
      }

      int rot0 = 0;
      int best_rot = 0;
      double best_value = 0;
      int best_t0 = NOT_FOUND;
      int best_t1 = NOT_FOUND;
      ++g_cid;

      if (block_id >= 2) {
        update_removable_blocks();
      }

      int retry_cnt = (D >= 8) ? 13 : 2;
      if (D == 8) retry_cnt = 8;
      if (D == 7) retry_cnt = 4;
      for (int l = 0; l < retry_cnt; ++l) {
        for (int rot = 0; rot < ROT_NUM; ++rot) {
          ++g_vid;
          double value = fill_block_light(t0, rot0, t1, rot, rot_type, DT[rot_type][rot0], DT[rot_type][rot]);

          if (best_value < value) {
            best_value = value;
            best_rot = rot;
            best_t0 = t0;
            best_t1 = t1;
          }
        }

        if (g_empty_space_list[0].size() > g_empty_space_list[1].size()) {
          g_checked[t0] = g_cid;
          int limit = 2 * D;
          do {
            t0 = select_random_point(0);
            --limit;
          } while (g_checked[t0] == g_cid && limit > 0);
          if (t0 == NOT_FOUND || g_checked[t0] == g_cid) break;
        } else {
          g_checked[t1] = g_cid;
          int limit = 2 * D;
          do {
            t1 = select_random_point(1);
            --limit;
          } while (g_checked[t1] == g_cid && limit > 0);
          if (t1 == NOT_FOUND || g_checked[t1] == g_cid) break;
        }
      }

      assert(best_t1 != NOT_FOUND);
      int size = fill_block(block_id, best_t0, rot0, best_t1, best_rot, rot_type);
      cur_score += BASE_SCORE / size;
      if (block_id >= 2) {
        cur_score = hill_crime();
      }
      ++block_id;
      ++loop_cnt;

      bool ok = is_completed();
      if (ok || cur_score > min_score) {
        if (ok && min_score > cur_score) {
          min_score = cur_score;
          memcpy(best_blocks, g_blocks, sizeof(g_blocks));
        }

        block_id = 1;
        cur_score = 0;
        rot_type = (rot_type + 1) % 2;
        memset(g_block_counter, 0, sizeof(g_block_counter));
        memcpy(g_blocks, g_blocks_origin, sizeof(g_blocks_origin));
        memcpy(g_panel_bonus, g_panel_bonus_origin, sizeof(g_panel_bonus_origin));
      }
    }

    memcpy(g_blocks, best_blocks, sizeof(best_blocks));
    reassign_block_id();

    fprintf(stderr, "loop_cnt: %lld, min_score: %lld\n", loop_cnt, min_score);
    return count_block_num();
  }

  bool is_completed() {
    for (int type = 0; type < 2; ++type) {
      for (int t : g_empty_space_list[type]) {
        Point p = calc_point(t);
        int x = p.x;
        int y = p.y;
        int z = p.z;
        int v1 = F[type][z][x];
        int v2 = R[type][z][y];

        if (v1 == 1 && v2 == 1) {
          if (g_block_counter[type][0][z][x] == 0 || g_block_counter[type][1][z][y] == 0) return false;
        }
      }
    }

    return true;
  }

  int count_block_num() {
    int cnt = 0;
    ++g_cid;

    for (int type = 0; type < 2; ++type) {
      for (int t : g_empty_space_list[type]) {
        ll block_id = g_blocks[type][t];

        if (block_id != EMPTY && g_checked[block_id] != g_cid) {
          g_checked[block_id] = g_cid;
          ++cnt;
        }
      }
    }

    return cnt;
  }

  ll calc_score_full_fast(map<ll, int> &size_memo, int nt0, int nt3, int id0, int id1, int id2, ll cur_score) {
    ll score = 0;
    ++g_vid;
    ++g_cid;
    int t_list[2] = {nt0, nt3};
    int id_list[2] = {id1, id2};

    assert(id0 >= 1 && size_memo[id0] > 0);
    score += BASE_SCORE / (size_memo[id0] + 1);

    for (auto [block_id, size] : size_memo) {
      if (block_id == id0 || block_id == id1 || block_id == id2) continue;
      if (block_id <= 0) continue;
      if (size <= 0) continue;

      score += BASE_SCORE / size;
    }

    for (int i = 0; i < 2; ++i) {
      int t = t_list[i];
      int id = id_list[i];
      if (t == EMPTY) continue;
      if (id == EMPTY) continue;

      for (int dir = 0; dir < 6; ++dir) {
        int nt = t + DT_ORIGIN[dir];
        if (g_visited[nt] == g_vid) continue;
        if (g_blocks[0][nt] == WALL) continue;
        if (g_blocks[0][nt] == EMPTY) continue;

        ll key = g_blocks[0][nt];
        int block_id = get_block_id(key);
        if (block_id != id) continue;
        if (g_checked[block_id] == g_cid) return LLONG_MAX;
        g_checked[block_id] = g_cid;

        int size = calc_block_size(nt, block_id);
        score += BASE_SCORE / size;
        if (cur_score <= score) break;
      }
    }

    return score;
  }

  ll calc_score_full() {
    ll score = 0;
    ++g_vid;

    for (int t : g_empty_space_list[0]) {
      if (g_visited[t] == g_vid) continue;
      ll key = g_blocks[0][t];
      int block_id = get_block_id(key);

      if (key != EMPTY) {
        int size = calc_block_size(t, block_id);
        score += BASE_SCORE / size;
      }
    }

    return score;
  }

  void reassign_block_id() {
    fprintf(stderr, "reassign_block_id =>\n");
    int new_block_id = 1;
    ++g_cid;
    ++g_vid;
    g_checked[WALL] = g_cid;

    for (int x = 1; x <= D; ++x) {
      for (int y = 1; y <= D; ++y) {
        for (int z = 1; z <= D; ++z) {
          int rt = calc_t(x, y, z);
          if (g_visited[rt] == g_vid) continue;
          if (g_blocks[0][rt] == EMPTY) continue;
          if (g_blocks[0][rt] == WALL) continue;

          queue<int> que;
          que.push(rt);
          while (not que.empty()) {
            int t = que.front();
            que.pop();
            if (g_visited[t] == g_vid) continue;
            g_visited[t] = g_vid;

            ll key = g_blocks[0][t];
            Command cmd = decode_key(key);
            g_blocks[0][cmd.t0] = new_block_id;
            g_blocks[1][cmd.t1] = new_block_id;

            for (int dir = 0; dir < 6; ++dir) {
              int nt = t + DT_ORIGIN[dir];
              if (g_blocks[0][nt] == WALL) continue;
              if (g_visited[nt] == g_vid) continue;
              if (get_block_id(g_blocks[0][nt]) != cmd.block_id) continue;

              que.push(nt);
            }
          }

          ++new_block_id;
        }
      }
    }

    for (int type = 0; type < 2; ++type) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          for (int z = 1; z <= D; ++z) {
            int t = calc_t(x, y, z);
            if (g_blocks[type][t] == -1) {
              g_blocks[type][t] = 0;
            }
          }
        }
      }
    }
  }

  int calc_block_size(int t, int block_id) {
    assert(g_visited[t] != g_vid);
    g_visited[t] = g_vid;
    int size = 1;

    for (int dir = 0; dir < 6; ++dir) {
      int nt = t + DT_ORIGIN[dir];
      if (get_block_id(g_blocks[0][nt]) != block_id) continue;
      if (g_visited[nt] == g_vid) continue;

      size += calc_block_size(nt, block_id);
    }

    return size;
  }

  int fill_block(int block_id, int t0, int rot0, int t1, int rot1, int rot_type) {
    assert(g_blocks[0][t0] == EMPTY);
    assert(g_blocks[1][t1] == EMPTY);

    Point p0 = calc_point(t0);
    Point p1 = calc_point(t1);

    ll key = build_key(block_id, t0, rot0, t1, rot1, rot_type);
    int cnt = 1;
    g_blocks[0][t0] = key;
    g_blocks[1][t1] = key;
    g_block_counter[0][0][p0.z][p0.x]++;
    g_block_counter[0][1][p0.z][p0.y]++;
    g_block_counter[1][0][p1.z][p1.x]++;
    g_block_counter[1][1][p1.z][p1.y]++;
    g_panel_bonus[t0 & XZ_MASK] = 0;
    g_panel_bonus[CUBE_SIZE + (t0 & YZ_MASK)] = 0;
    g_panel_bonus[2 * CUBE_SIZE + (t1 & XZ_MASK)] = 0;
    g_panel_bonus[3 * CUBE_SIZE + (t1 & YZ_MASK)] = 0;

    for (int dir = 0; dir < 6; ++dir) {
      int nt0 = t0 + DT[rot_type][rot0][dir];
      int nt1 = t1 + DT[rot_type][rot1][dir];
      if (g_blocks[0][nt0] != EMPTY) continue;
      if (g_blocks[1][nt1] != EMPTY) continue;

      cnt += fill_block(block_id, nt0, rot0, nt1, rot1, rot_type);
    }

    return cnt;
  }

  double fill_block_light(int t0, int rot0, int t1, int rot1, int rot_type, const int *DT0, const int *DT1) {
    int xz0 = t0 & XZ_MASK;
    int yz0 = CUBE_SIZE + (t0 & YZ_MASK);
    int xz1 = 2 * CUBE_SIZE + (t1 & XZ_MASK);
    int yz1 = 3 * CUBE_SIZE + (t1 & YZ_MASK);

    double value = 1;
    if (g_visited_space[xz0] != g_vid) {
      g_visited_space[xz0] = g_vid;
      value += g_panel_bonus[xz0];
    }
    if (g_visited_space[yz0] != g_vid) {
      g_visited_space[yz0] = g_vid;
      value += g_panel_bonus[yz0];
    }
    if (g_visited_space[xz1] != g_vid) {
      g_visited_space[xz1] = g_vid;
      value += g_panel_bonus[xz1];
    }
    if (g_visited_space[yz1] != g_vid) {
      g_visited_space[yz1] = g_vid;
      value += g_panel_bonus[yz1];
    }
    g_visited[t0] = g_vid;

    for (int dir = 0; dir < 6; ++dir) {
      int nt0 = t0 + DT0[dir];
      int nt1 = t1 + DT1[dir];
      if (g_blocks[0][nt0] != EMPTY && g_removable[0][nt0] != g_rid) continue;
      if (g_blocks[1][nt1] != EMPTY && g_removable[1][nt1] != g_rid) continue;
      if (g_visited[nt0] == g_vid) continue;

      value += fill_block_light(nt0, rot0, nt1, rot1, rot_type, DT0, DT1);
    }

    return value;
  }

  int erase_block_all(int target_block_id) {
    int cnt = 0;

    for (int type = 0; type < 2; ++type) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          for (int z = 1; z <= D; ++z) {
            int t = calc_t(x, y, z);
            ll block_id = get_block_id(g_blocks[type][t]);

            if (block_id == target_block_id) {
              ++cnt;
              g_blocks[type][t] = EMPTY;
              g_block_counter[type][0][z][x]--;
              g_block_counter[type][1][z][y]--;
              if (g_block_counter[type][0][z][x] == 0) {
                g_panel_bonus[2 * type * CUBE_SIZE + (t & XZ_MASK)] = g_panel_bonus_origin[2 * type * CUBE_SIZE +
                                                                                           (t & XZ_MASK)];
              }
              if (g_block_counter[type][1][z][y] == 0) {
                g_panel_bonus[2 * type * CUBE_SIZE + CUBE_SIZE + (t & YZ_MASK)] = g_panel_bonus_origin[
                  2 * type * CUBE_SIZE + CUBE_SIZE + (t & YZ_MASK)];
              }
              assert(g_block_counter[type][0][z][x] >= 0);
              assert(g_block_counter[type][0][z][y] >= 0);
            }
          }
        }
      }
    }

    return cnt / 2;
  }

  void run() {
    high_resolution_clock::time_point begin = high_resolution_clock::now();
    load_data();
    setup();

    search_best_blocks(begin);
    int N = count_block_num();
    cout << N << endl;
    fprintf(stderr, "N = %d\n", N);

    for (int type = 0; type < 2; ++type) {
      for (int x = 1; x <= D; ++x) {
        for (int y = 1; y <= D; ++y) {
          for (int z = 1; z <= D; ++z) {
            int t = calc_t(x, y, z);
            cout << g_blocks[type][t] << " ";
          }
        }
      }
      cout << endl;
    }

    fprintf(stderr, "finished_time: %.4f\n", get_time(begin));
  }

  ll hill_crime() {
    priority_queue <Block, vector<Block>, greater<Block>> pque;
    map<ll, int> size_memo;
    ll cur_score = calc_score_full();
    ++g_cid;

    for (int t : g_empty_space_list[0]) {
      ll key = g_blocks[0][t];
      if (key == EMPTY) continue;

      int block_id = get_block_id(key);
      size_memo[block_id]++;
    }

    for (auto &[block_id, size] : size_memo) {
      pque.push(Block(block_id, size));
    }

    int updated_cnt = 0;
    priority_queue <Block, vector<Block>, greater<Block>> stock;

    while (not pque.empty()) {
      Block b = pque.top();
      pque.pop();
      bool updated = false;

      for (int t0 : g_empty_space_list[0]) {
        ll key = g_blocks[0][t0];
        if (key == EMPTY) continue;
        if (key == WALL) continue;
        Command cmd = decode_key(key);
        if (cmd.block_id != b.id) continue;

        int t1 = cmd.t1;
        assert(g_blocks[0][t0] == g_blocks[1][t1]);
        int rot_type = cmd.rot_type;
        int rot0 = cmd.rot0;
        int rot1 = cmd.rot1;

        for (int dir = 0; dir < 6; ++dir) {
          int nt0 = t0 + DT[rot_type][rot0][dir];
          int nt1 = t1 + DT[rot_type][rot1][dir];
          ll n0_key = g_blocks[0][nt0];
          ll n1_key = g_blocks[1][nt1];
          if (n0_key == WALL) continue;
          if (n1_key == WALL) continue;
          if (get_block_id(n0_key) == cmd.block_id) continue;
          if (n0_key != EMPTY && size_memo[cmd.block_id] + 1 >= size_memo[get_block_id(n0_key)]) continue;
          if (n1_key != EMPTY && size_memo[cmd.block_id] + 1 >= size_memo[get_block_id(n1_key)]) continue;

          Command n0_cmd = decode_key(n0_key);
          Command n1_cmd = decode_key(n1_key);
          // nt0 ブロック0 の進行方向座標
          // nt1 ブロック1 の進行方向座標
          // nt2 ブロック0 の進行方向の座標にブロックがある場合の対となるブロック 1の座標 (nt0 と nt2 の key は同じになる)
          // nt3 ブロック1 の進行方向の座標にブロックがある場合の対となるブロック 0の座標 (nt1 と nt3 の key は同じになる)
          int nt2 = n0_cmd.t1;
          int nt3 = n1_cmd.t0;
          ll n3_key = g_blocks[0][nt3];

          int nx0 = nt0 >> 8;
          int ny0 = nt0 >> 4 & 15;
          int nz0 = nt0 & 15;
          int nx1 = nt1 >> 8;
          int ny1 = nt1 >> 4 & 15;
          int nz1 = nt1 & 15;
          int nx2 = nt2 >> 8;
          int ny2 = nt2 >> 4 & 15;
          int nz2 = nt2 & 15;
          int nx3 = nt3 >> 8;
          int ny3 = nt3 >> 4 & 15;
          int nz3 = nt3 & 15;

          ll temp0 = g_blocks[0][nt0];
          ll temp1 = g_blocks[1][nt2];
          ll temp2 = g_blocks[1][nt1];
          ll temp3 = g_blocks[0][nt3];

          // 事前にブロックを削除する
          g_blocks[0][nt0] = EMPTY;
          g_blocks[1][nt2] = EMPTY;
          g_blocks[1][nt1] = EMPTY;
          g_blocks[0][nt3] = EMPTY;

          bool ok = true;
          // 進行方向に対してブロックを設置する
          ll new_key = build_key(cmd.block_id, nt0, rot0, nt1, rot1, rot_type);
          g_blocks[0][nt0] = new_key;
          g_blocks[1][nt1] = new_key;
          if (nt1 != nt2) {
            if (temp0 == EMPTY) {
              g_block_counter[0][0][nz0][nx0]++;
              g_block_counter[0][1][nz0][ny0]++;
            }
            if (temp2 == EMPTY) {
              g_block_counter[1][0][nz1][nx1]++;
              g_block_counter[1][1][nz1][ny1]++;
            }
            if (temp0 != EMPTY) {
              g_block_counter[1][0][nz2][nx2]--;
              g_block_counter[1][1][nz2][ny2]--;
              if (F[1][nz2][nx2] == 1 && g_block_counter[1][0][nz2][nx2] == 0) ok = false;
              if (R[1][nz2][ny2] == 1 && g_block_counter[1][1][nz2][ny2] == 0) ok = false;
            }
            if (temp2 != EMPTY) {
              g_block_counter[0][0][nz3][nx3]--;
              g_block_counter[0][1][nz3][ny3]--;
              if (F[0][nz3][nx3] == 1 && g_block_counter[0][0][nz3][nx3] == 0) ok = false;
              if (R[0][nz3][ny3] == 1 && g_block_counter[0][1][nz3][ny3] == 0) ok = false;
            }
          }

          ll score = LLONG_MAX;
          if (ok) {
            score = calc_score_full_fast(size_memo, nt0, nt3, cmd.block_id, get_block_id(n0_key),
                                         get_block_id(n3_key), cur_score);
          }

          if (ok && cur_score > score) {
            ++updated_cnt;
            b.size++;
            updated = true;
            cur_score = score;

            size_memo[cmd.block_id]++;
            size_memo[get_block_id(n0_key)]--;
            if (nt1 != nt2) {
              size_memo[get_block_id(n3_key)]--;
            }
          } else {
            if (nt1 != nt2) {
              if (temp0 == EMPTY) {
                g_block_counter[0][0][nz0][nx0]--;
                g_block_counter[0][1][nz0][ny0]--;
              } else {
                g_block_counter[1][0][nz2][nx2]++;
                g_block_counter[1][1][nz2][ny2]++;
              }
              if (temp2 == EMPTY) {
                g_block_counter[1][0][nz1][nx1]--;
                g_block_counter[1][1][nz1][ny1]--;
              } else {
                g_block_counter[0][0][nz3][nx3]++;
                g_block_counter[0][1][nz3][ny3]++;
              }
            }

            g_blocks[0][nt0] = temp0;
            g_blocks[1][nt2] = temp1;
            g_blocks[1][nt1] = temp2;
            g_blocks[0][nt3] = temp3;
          }
        }
      }

      if (updated) {
        pque.push(b);
      } else {
        stock.push(b);
      }

      if (pque.empty() && updated_cnt > 0) {
        updated_cnt = 0;
        pque = stock;
        stock = priority_queue < Block, vector < Block >, greater < Block >> ();
      }
    }

    return cur_score;
  }

  void check_new_block_id(int type, int t, int block_id, int new_block_id) {
    assert(get_block_id(g_blocks[type][t]) == block_id);
    g_visited[t] = g_vid;
    g_blocks[type][t] = new_block_id;

    for (int dir = 0; dir < 6; ++dir) {
      int nt = t + DT_ORIGIN[dir];
      if (get_block_id(g_blocks[type][nt]) != block_id) continue;

      check_new_block_id(type, nt, block_id, new_block_id);
    }
  }
};

int main() {
  SilhouetteBlockPuzzleCreation sbpc;
  sbpc.run();

  return 0;
}