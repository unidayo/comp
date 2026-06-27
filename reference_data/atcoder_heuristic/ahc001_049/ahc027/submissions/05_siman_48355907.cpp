#define NDEBUG

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <climits>
#include <map>
#include <queue>
#include <set>
#include <cstring>
#include <cfloat>
#include <vector>
#include <chrono>
#include <bitset>

using namespace std;
typedef long long ll;
using namespace std::chrono;

double TIME_LIMIT = 1.95;
const int MAX_L = 100000;
const int MAX_N = 40;
const int GRID_SIZE = MAX_N + 2;
const int UP = 0;
const int RIGHT = 1;
const int DOWN = 2;
const int LEFT = 3;
const int DZ[4] = {-GRID_SIZE, 1, GRID_SIZE, -1};
const string DS[4] = {"U", "R", "D", "L"};
int CHECK_RANGE = 12;
double g_back_rate;
int SZ;

unsigned long long xor128() {
  static unsigned long long rx = 123456789, ry = 362436069, rz = 521288629, rw = 88675123;
  unsigned long long rt = (rx ^ (rx << 11));
  rx = ry;
  ry = rz;
  rz = rw;
  return (rw = (rw ^ (rw >> 19)) ^ (rt ^ (rt >> 8)));
}

const int DIR_PTN[24][4] = {
  {0, 1, 2, 3},
  {0, 1, 3, 2},
  {0, 2, 1, 3},
  {0, 2, 3, 1},
  {0, 3, 1, 2},
  {0, 3, 2, 1},
  {1, 0, 2, 3},
  {1, 0, 3, 2},
  {1, 2, 0, 3},
  {1, 2, 3, 0},
  {1, 3, 0, 2},
  {1, 3, 2, 0},
  {2, 0, 1, 3},
  {2, 0, 3, 1},
  {2, 1, 0, 3},
  {2, 1, 3, 0},
  {2, 3, 0, 1},
  {2, 3, 1, 0},
  {3, 0, 1, 2},
  {3, 0, 2, 1},
  {3, 1, 0, 2},
  {3, 1, 2, 0},
  {3, 2, 0, 1},
  {3, 2, 1, 0},
};

int calc_z(int y, int x) {
  return y * GRID_SIZE + x;
}

double get_time(high_resolution_clock::time_point &begin) {
  high_resolution_clock::time_point end = high_resolution_clock::now();
  return std::chrono::duration<double>(end - begin).count();
}

int W;
int N;
ll sum_D;
ll sum_OD;
bool g_exist_wall[GRID_SIZE * GRID_SIZE][4];
int g_step_counter[GRID_SIZE * GRID_SIZE];
int g_D[GRID_SIZE * GRID_SIZE];
int g_OD[GRID_SIZE * GRID_SIZE];
ll g_visited[GRID_SIZE * GRID_SIZE];
ll g_vid;
int g_last_visited_at[GRID_SIZE * GRID_SIZE];
int g_first_visited_at[GRID_SIZE * GRID_SIZE];
unsigned char g_last_visited_at_list[2][800][GRID_SIZE * GRID_SIZE];
bitset<GRID_SIZE * GRID_SIZE> g_visited_mask[2][100];
int g_distance[GRID_SIZE * GRID_SIZE][GRID_SIZE * GRID_SIZE];
vector <pair<int, int>> g_neighbors[GRID_SIZE * GRID_SIZE];
ll g_cost_memo[MAX_L + 10];
int g_wall_counter[GRID_SIZE * GRID_SIZE];

struct NodePointer {
  int parent;
  int dir;
  ll sum_d;
  ll value;
  ll penalty;
  ll h;

  NodePointer(int parent = -1, int dir = 0, ll sum_d = 0, ll value = 0) {
    this->parent = parent;
    this->dir = dir;
    this->sum_d = sum_d;
    this->value = value;
    this->h = 0;
  }

  bool operator>(const NodePointer &np) const {
    return sum_d + h > np.sum_d + np.h;
  }

  bool operator<(const NodePointer &np) const {
    return value < np.value;
  }
};

struct PathNode {
  int z;
  int dir;
  int dist;
  ll value;

  PathNode(int z = -1, int dir = -1, int dist = -1, ll value = 0) {
    this->z = z;
    this->dir = dir;
    this->dist = dist;
    this->value = value;
  }

  bool operator>(const PathNode &pn) const {
    if (dist == pn.dist) {
      return value > pn.value;
    } else {
      return dist > pn.dist;
    }
  }
};

struct Node {
  int parent;
  int t;
  int z;
  int dir;
  int idx;
  int remain_cnt;
  int dup_step_count;
  ll sum_d;
  ll value;
  ll penalty;

  Node(int parent = -1, int t = -1, int z = -1, int dir = -1, ll sum_d = -1, ll value = 0) {
    this->parent = parent;
    this->t = t;
    this->z = z;
    this->dir = dir;
    this->sum_d = sum_d;
    this->value = value;
  }
};

const int MAX_NODE_NUM = 300000;
Node g_nodes[MAX_NODE_NUM];

class RecurringCleaningRoute {
public:
  void load_data() {
    memset(g_exist_wall, false, sizeof(g_exist_wall));
    memset(g_visited, 0, sizeof(g_visited));
    memset(g_D, -1, sizeof(g_D));
    memset(g_wall_counter, 0, sizeof(g_wall_counter));
    g_vid = 0;
    SZ = calc_z(0, 0);

    for (ll l = 0; l <= MAX_L; ++l) {
      g_cost_memo[l] = l * (l + 1) / 2;
    }

    cin >> N;
    fprintf(stderr, "N = %d\n", N);

    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        if (y == 0) {
          g_exist_wall[z][UP] = true;
        }
        if (y == N - 1) {
          g_exist_wall[z][DOWN] = true;
        }
        if (x == 0) {
          g_exist_wall[z][LEFT] = true;
        }
        if (x == N - 1) {
          g_exist_wall[z][RIGHT] = true;
        }
      }
    }

    for (int y = 0; y < N - 1; ++y) {
      string row;
      cin >> row;

      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        int nz = calc_z(y + 1, x);
        if (row[x] == '0') continue;

        g_exist_wall[z][DOWN] = true;
        g_exist_wall[nz][UP] = true;
      }
    }

    for (int y = 0; y < N; ++y) {
      string col;
      cin >> col;

      for (int x = 0; x < N - 1; ++x) {
        int z = calc_z(y, x);
        int nz = calc_z(y, x + 1);
        if (col[x] == '0') continue;

        g_exist_wall[z][RIGHT] = true;
        g_exist_wall[nz][LEFT] = true;
      }
    }
  }

  void setup_wall_count_and_neighbors() {
    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);

        for (int dir = 0; dir < 4; ++dir) {
          if (g_exist_wall[z][dir]) {
            g_wall_counter[z]++;
            continue;
          }
          int nz = z + DZ[dir];
          g_neighbors[z].push_back(make_pair(nz, dir));
        }
      }
    }
  }

  void setup_sum_d() {
    sum_D = 0;

    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        cin >> g_D[z];
        sum_D += g_D[z];
      }
    }

    sum_OD = sum_D;
    memcpy(g_OD, g_D, sizeof(g_D));
  }

  void setup_w_count() {
    W = 0;
    for (int y = 0; y < N - 1; ++y) {
      bool hit_wall = false;

      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        if (g_exist_wall[z][DOWN]) {
          if (not hit_wall) {
            ++W;
          }
          hit_wall = true;
        } else {
          hit_wall = false;
        }
      }
    }
    for (int x = 0; x < N - 1; ++x) {
      bool hit_wall = false;

      for (int y = 0; y < N; ++y) {
        int z = calc_z(y, x);
        if (g_exist_wall[z][RIGHT]) {
          if (not hit_wall) {
            ++W;
          }
          hit_wall = true;
        } else {
          hit_wall = false;
        }
      }
    }

    fprintf(stderr, "W = %d\n", W);
  }

  void setup_distances() {
    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int cz = calc_z(y, x);

        queue <pair<int, int>> que;
        que.push(make_pair(cz, 0));
        ++g_vid;

        while (not que.empty()) {
          pair<int, int> p = que.front();
          que.pop();
          int z = p.first;
          int dist = p.second;

          if (g_visited[z] == g_vid) continue;
          g_visited[z] = g_vid;
          g_distance[cz][z] = dist;

          for (auto [nz, dir] : g_neighbors[z]) {
            if (g_visited[nz] == g_vid) continue;

            que.push(make_pair(nz, dist + 1));
          }
        }
      }
    }
  }

  void dfs(int z, int dir_ptn, vector<short> &commands) {
    g_visited[z] = g_vid;

    for (int i = 0; i < 4; ++i) {
      int dir = DIR_PTN[dir_ptn][i];
      if (g_exist_wall[z][dir]) continue;
      int nz = z + DZ[dir];
      if (g_visited[nz] == g_vid) continue;

      commands.push_back(nz);
      dfs(nz, dir_ptn, commands);
      commands.push_back(z);
    }
  }

  vector<short> build_first_commands(int dir_ptn) {
    vector<short> commands;
    commands.push_back(SZ);
    ++g_vid;
    dfs(SZ, dir_ptn, commands);

    return commands;
  }

  void show_grid() {
    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        fprintf(stderr, "%2d ", g_step_counter[z]);
      }
      fprintf(stderr, "\n");
    }
  }

  vector<short> bfs_shape(vector<short> &commands) {
    g_visited[SZ] = g_vid;
    int last_unvisited_z = SZ;
    bool visited[GRID_SIZE * GRID_SIZE];
    memset(visited, false, sizeof(visited));
    vector<short> res;
    res.push_back(SZ);

    for (int i = 0; i < commands.size(); ++i) {
      int cz = commands[i];

      if (not visited[cz]) {
        vector<short> new_commands = find_path(last_unvisited_z, cz);
        res.insert(res.end(), new_commands.begin(), new_commands.end());
        last_unvisited_z = cz;
      }
      visited[cz] = true;
    }

    return res;
  }

  vector<short> find_path(int from_z, int to_z) {
    queue <pair<int, int>> que;
    que.push(make_pair(from_z, -1));
    int history[GRID_SIZE * GRID_SIZE];
    memset(history, -1, sizeof(history));

    ++g_vid;
    while (not que.empty()) {
      pair<int, int> p = que.front();
      que.pop();
      int z = p.first;
      int dir = p.second;

      if (g_visited[z] == g_vid) continue;
      g_visited[z] = g_vid;
      history[z] = dir;

      if (z == to_z) {
        vector<short> commands;
        int cur_z = z;
        while (cur_z != from_z) {
          int dir = history[cur_z];
          commands.push_back(cur_z);
          cur_z += DZ[(dir + 2) % 4];
        }
        assert(cur_z == from_z);
        reverse(commands.begin(), commands.end());
        return commands;
      }

      for (auto [nz, dir] : g_neighbors[z]) {
        if (g_visited[nz] == g_vid) continue;

        que.push(make_pair(nz, dir));
      }
    }

    fprintf(stderr, "from_z = %d, to_z = %d\n", from_z, to_z);
    assert(false);
  }

  vector<short> reshape_commands(vector<short> &commands) {
    vector<short> new_commands;

    ++g_vid;
    int remain_cell_cnt = N * N - 1;
    g_visited[SZ] = g_vid;

    for (int i = 0; i < commands.size() && remain_cell_cnt > 0; ++i) {
      int cz = commands[i];

      if (g_visited[cz] != g_vid) {
        --remain_cell_cnt;
      }

      g_visited[cz] = g_vid;
      new_commands.push_back(cz);
    }

    return new_commands;
  }

  Node build_root_node_mini(vector<short> &cur_commands, int offset, int beam_depth) {
    memset(g_last_visited_at, -1, sizeof(g_last_visited_at));

    Node node;
    node.t = 0;
    node.z = calc_z(0, 0);
    node.sum_d = 0;
    node.value = 0;
    ++g_vid;

    sum_D = sum_OD;

    for (int t = 0; t < offset; ++t) {
      int cz = cur_commands[t];

      g_last_visited_at[cz] = t + 1;
    }

    int L = cur_commands.size();
    for (int t = offset + beam_depth; t < min(offset + beam_depth + 3 * CHECK_RANGE, L); ++t) {
      int cz = cur_commands[t];

      g_last_visited_at[cz] = t + 1;
    }

    node.t = offset;
    node.penalty = sum_D;
    return node;
  }

  Node build_root_node(vector<short> &first_commands) {
    memset(g_last_visited_at, 0, sizeof(g_last_visited_at));
    memset(g_last_visited_at_list[0][0], 0, sizeof(g_last_visited_at_list[0][0]));

    Node node;
    node.t = 0;
    node.z = calc_z(0, 0);
    node.sum_d = 0;
    node.value = 0;
    node.remain_cnt = N * N - 1;
    node.dup_step_count = 0;
    ++g_vid;

    g_visited[SZ] = g_vid;
    for (int t = 0; t < first_commands.size(); ++t) {
      int cz = first_commands[t];
      if (g_visited[cz] != g_vid) {
        --node.remain_cnt;
      }
      g_visited[cz] = g_vid;
    }

    sum_D = 0;
    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        if (g_visited[z] != g_vid) {
          g_D[z] = 25 + g_OD[z];
        } else {
          g_D[z] = g_OD[z];
        }
        sum_D += g_D[z];
      }
    }

    for (int t = 0; t < first_commands.size(); ++t) {
      int cz = first_commands[t];

      node.sum_d += sum_D;
      int last_at = g_last_visited_at[cz];
      node.t += 1;
      node.sum_d -= (node.t - last_at) * g_D[cz];
      g_last_visited_at[cz] = node.t;
      node.z = cz;
    }

    node.penalty = sum_D;
    return node;
  }

  vector<short> mini_beam(int len, int ext, vector<short> &cur_commands, int from_z, int to_z, int offset,
                          int *step_counter, bool skip_check = false, bool dup_step_check = true) {
    int beam_depth = len + ext;
    Node root = build_root_node_mini(cur_commands, offset, len + 1);
    root.z = from_z;
    assert(root.z == from_z);
    root.penalty = INT_MAX;
    int node_num = 0;
    g_nodes[node_num] = root;
    queue<int> que;
    que.push(0);
    int best_nid = 0;
    g_visited_mask[0][0] = bitset<GRID_SIZE * GRID_SIZE>();
    g_visited_mask[0][0].set(from_z);
    if (offset - 2 >= 0) {
      int bz = cur_commands[offset - 2];
      g_visited_mask[0][0].set(bz);
    }
    double best_value = DBL_MAX;
    int beam_width = 3;
    assert(offset + beam_depth < cur_commands.size());

    for (int depth = 0; depth < beam_depth; ++depth) {
      vector <NodePointer> np_list(beam_width * 4);
      int idx = 0;
      int key = depth % 2;
      assert(offset + depth < cur_commands.size());
      int bz = cur_commands[offset + depth];
      int cand_num = 0;

      while (not que.empty()) {
        int node_id = que.front();
        que.pop();
        Node &node = g_nodes[node_id];
        node.idx = idx;

        for (auto [nz, dir] : g_neighbors[node.z]) {
          int dist = g_distance[nz][to_z];
          if (beam_depth - depth - 1 < dist) continue;
          if (dup_step_check) {
            if (g_wall_counter[nz] <= 2 && g_visited_mask[key][idx][nz]) continue;
            if (g_last_visited_at[nz] != -1 && abs(node.t - g_last_visited_at[nz]) < CHECK_RANGE) continue;
          }
          int last_at = g_last_visited_at[nz];
          ll nsum_d = node.sum_d + sum_OD;
          nsum_d -= (abs(node.t - last_at) + 1) * g_OD[nz];
          ll value = nsum_d + xor128() % 200000;

          NodePointer np(node_id, dir, nsum_d, value);
          np.penalty = node.penalty;
          if (nz != bz) {
            np.penalty = 0;
          }
          np_list[cand_num] = np;
          ++cand_num;
        }

        idx++;
      }

      if (cand_num > beam_width) {
        np_list.resize(cand_num);
        partial_sort(np_list.begin(), np_list.begin() + beam_width, np_list.end());
      }

      ++g_vid;
      for (int i = 0; i < beam_width && i < cand_num; ++i) {
        NodePointer &np = np_list[i];
        Node &parent = g_nodes[np.parent];
        int nz = parent.z + DZ[np.dir];
        g_visited[nz] = g_vid;
        int n_key = (key + 1) % 2;

        Node next(np.parent, parent.t + 1, nz, np.dir, np.sum_d, np.value);
        next.penalty = np.penalty;
        g_visited_mask[n_key][i] = g_visited_mask[key][parent.idx];
        g_visited_mask[n_key][i].set(nz);

        ++node_num;
        assert(node_num < MAX_NODE_NUM);
        g_nodes[node_num] = next;
        que.push(node_num);

        if (next.z == to_z && next.penalty == 0) {
          double ave = next.sum_d;

          if (best_value > ave) {
            bool ok = true;

            if (not skip_check) {
              for (int y = 0; y < N && ok; ++y) {
                for (int x = 0; x < N && ok; ++x) {
                  int z = calc_z(y, x);
                  if (step_counter[z] != 0) continue;
                  if (not g_visited_mask[n_key][i][z]) {
                    ok = false;
                  }
                }
              }
            }

            if (ok) {
              best_value = ave;
              best_nid = node_num;
            }
          }
        }
      }
    }

    vector<short> commands;
    int cur_nid = best_nid;
    while (cur_nid != 0) {
      Node &node = g_nodes[cur_nid];
      commands.push_back(node.z);
      cur_nid = node.parent;
    }
    if (commands.size() > 0) {
      assert(commands[0] == to_z);
      reverse(commands.begin(), commands.end());
      commands.pop_back();
    }

    return commands;
  }

  vector<short> beam(int beam_depth, vector<short> &first_commands, bool is_last = false) {
    Node root = build_root_node(first_commands);
    int node_num = 0;
    g_nodes[node_num] = root;
    queue<int> que;
    que.push(0);
    int best_nid = 0;
    int offset = first_commands.size();
    memset(g_last_visited_at_list[0][0], 0, sizeof(g_last_visited_at_list[0][0]));

    for (int depth = 0; depth < beam_depth; ++depth) {
      priority_queue <NodePointer, vector<NodePointer>, greater<NodePointer>> pque;
      vector <ll> min_values(GRID_SIZE * GRID_SIZE, LLONG_MAX);
      int idx = 0;
      int key = depth % 2;

      while (not que.empty()) {
        int node_id = que.front();
        que.pop();
        Node &node = g_nodes[node_id];
        node.idx = idx;

        for (auto [nz, dir] : g_neighbors[node.z]) {
          if (g_wall_counter[node.z] <= 1 && (dir ^ 2) == node.dir) continue;
          ll penalty = node.penalty;
          int remain_cnt = node.remain_cnt;
          int dup_step_count = node.dup_step_count + 1;
          if (depth >= beam_depth * g_back_rate && g_wall_counter[node.z] <= 1 &&
              g_last_visited_at_list[key][idx][nz] != 0)
            continue;
          int last_at = offset + g_last_visited_at_list[key][idx][nz];
          if (g_last_visited_at_list[key][idx][nz] == 0) {
            last_at = g_last_visited_at[nz];
          }
          ll nsum_d = node.sum_d + penalty;
          if (g_last_visited_at_list[key][idx][nz] == 0 && g_last_visited_at[nz] == 0) {
            --remain_cnt;
            dup_step_count = 0;
            penalty += (g_OD[nz] - g_D[nz]);
            nsum_d -= (node.t - last_at + 1) * g_D[nz];
          } else {
            nsum_d -= (node.t - last_at + 1) * g_OD[nz];
          }
          ll value = node.value + nsum_d;
          ll h = nsum_d + remain_cnt * sum_OD;
          if (dup_step_count > 3 * N) {
            h *= 2;
          }

          if (min_values[nz] > h) {
            NodePointer np(node_id, dir, nsum_d, value);
            min_values[nz] = h;
            np.penalty = penalty;
            np.h = h;
            pque.push(np);
          }
        }

        idx++;
      }

      double best_value = DBL_MAX;
      int beam_width = 800;
      if (not is_last) {
        beam_width = N * N / 4;
      }
      ++g_vid;
      for (int i = 0; i < beam_width && not pque.empty(); ++i) {
        NodePointer np = pque.top();
        pque.pop();
        Node &parent = g_nodes[np.parent];
        int nz = parent.z + DZ[np.dir];
        if (g_visited[nz] == g_vid) {
          --i;
          continue;
        }
        g_visited[nz] = g_vid;
        int n_key = (key + 1) % 2;

        Node next(np.parent, parent.t + 1, nz, np.dir, np.sum_d, np.value);
        next.penalty = np.penalty;
        next.remain_cnt = parent.remain_cnt;
        next.dup_step_count = parent.dup_step_count + 1;
        memcpy(g_last_visited_at_list[n_key][i], g_last_visited_at_list[key][parent.idx],
               sizeof(g_last_visited_at_list[key][parent.idx]));
        g_last_visited_at_list[n_key][i][nz] = (depth + 1) % 255 + 1;
        if (nz != calc_z(0, 0) && g_last_visited_at[nz] == 0 && g_last_visited_at_list[key][parent.idx][nz] == 0) {
          next.dup_step_count = 0;
          next.remain_cnt--;
        }

        ++node_num;
        assert(node_num < MAX_NODE_NUM);
        g_nodes[node_num] = next;
        que.push(node_num);

        if (next.z == calc_z(0, 0) || not is_last || (not is_last && next.remain_cnt == 0)) {
          double ave = (double) next.value / (double) next.t;
          if (not is_last) {
            ave = np.h + next.value / next.t;
          }

          if (best_value > ave) {
            best_value = ave;
            best_nid = node_num;
            if (is_last || next.remain_cnt == 0) {
              depth += 10000;
              break;
            }
          }
        }
      }
    }

    vector<short> commands;
    int cur_nid = best_nid;
    while (cur_nid != 0) {
      Node &node = g_nodes[cur_nid];
      commands.push_back(node.z);
      cur_nid = node.parent;
    }
    reverse(commands.begin(), commands.end());

    return commands;
  }

  ll calc_score(vector<short> &commands) {
    memset(g_last_visited_at, -1, sizeof(g_last_visited_at));

    ll L = commands.size() - 1;
    g_first_visited_at[SZ] = 0;
    ll value = 0;
    for (int t = 1; t <= L; ++t) {
      int cz = commands[t];

      if (g_last_visited_at[cz] == -1) {
        g_first_visited_at[cz] = t;
      } else {
        int iter = t - g_last_visited_at[cz] - 1;
        int d = g_D[cz];
        value += g_cost_memo[iter] * d;
      }
      g_last_visited_at[cz] = t;
    }

    for (int y = 0; y < N; ++y) {
      for (int x = 0; x < N; ++x) {
        int z = calc_z(y, x);
        int ft = g_first_visited_at[z];
        int lt = g_last_visited_at[z];
        int iter = L - lt + ft - 1;
        int d = g_D[z];
        value += g_cost_memo[iter] * d;
      }
    }

    return value / L;
  }

  vector<short> search_all_clean_path(int beam_depth, double time_limit = 0.8) {
    high_resolution_clock::time_point begin = high_resolution_clock::now();
    vector<short> commands;
    commands.push_back(SZ);

    bool all_clean = false;
    ll best_score = LLONG_MAX;
    vector<short> best_commands = commands;
    while (get_time(begin) < time_limit && commands.size() < MAX_L - 10000) {
      vector<short> new_commands = beam(beam_depth, commands);
      if (new_commands.size() > 0) {
        commands.insert(commands.end(), new_commands.begin(), new_commands.end());

        if (not all_clean) {
          int remain_cnt = count_remain_cells(commands);
          int branch_th = 3 * N;
          if (N <= 24) {
            branch_th = N / 4;
          } else if (N <= 28) {
            branch_th = N / 2;
          } else if (N <= 32) {
            branch_th = N;
          } else if (N <= 35) {
            branch_th = 2 * N;
          }

          if (remain_cnt <= branch_th) {
            while (remain_cnt > 0) {
              commands = build_branch_path(commands);
              remain_cnt = count_remain_cells(commands);
            }
            assert(remain_cnt == 0);
          }
          all_clean |= (remain_cnt == 0);
        }
        if (all_clean) {
          vector<short> last_commands = beam(350, commands, true);

          if (last_commands.size() > 0) {
            vector<short> res = commands;
            res.insert(res.end(), last_commands.begin(), last_commands.end());

            ll score = calc_score(res);
            if (best_score > score) {
              best_score = score;
              best_commands = res;
              break;
            } else {
              break;
            }
          }
        }
      }
    }

    if (all_clean) {
      return best_commands;
    } else {
      return vector<short>();
    }
  }

  vector<short> build_branch_path(vector<short> &cur_commands) {
    vector<short> new_commands;
    ++g_vid;

    for (int i = 0; i < cur_commands.size(); ++i) {
      int cz = cur_commands[i];
      g_visited[cz] = g_vid;
    }

    for (int i = 0; i < cur_commands.size(); ++i) {
      int cz = cur_commands[i];
      new_commands.push_back(cz);

      for (auto [nz, dir] : g_neighbors[cz]) {
        if (g_visited[nz] == g_vid) continue;

        new_commands.push_back(nz);
        new_commands.push_back(cz);
        g_visited[nz] = g_vid;
      }
    }

    return new_commands;
  }

  vector<short> safety_clean_path_search(high_resolution_clock::time_point &begin, double time_limit = TIME_LIMIT) {
    vector<short> first_commands;
    int min_value = INT_MAX;
    for (int ptn = 0; ptn < 24; ++ptn) {
      vector<short> commands = build_first_commands(ptn);
      vector<short> f_commands = reshape_commands(commands);
      vector<short> res = bfs_shape(f_commands);

      if (min_value > res.size()) {
        min_value = res.size();
        first_commands = res;
      }
    }
    vector<short> commands = first_commands;

    ll best_score = LLONG_MAX;
    vector<short> best_commands = commands;
    int fail_cnt = 0;
    int loop_cnt = 0;
    while (get_time(begin) < time_limit && commands.size() < MAX_L - 10000) {
      vector<short> new_commands = beam(350, commands);
      if (new_commands.size() > 0) {
        commands.insert(commands.end(), new_commands.begin(), new_commands.end());
        vector<short> last_commands = beam(350, commands, true);

        if (last_commands.size() > 0) {
          vector<short> res = commands;
          res.insert(res.end(), last_commands.begin(), last_commands.end());

          ll score = calc_score(res);
          if (best_score > score) {
            best_score = score;
            best_commands = res;
            fprintf(stderr, "loop_cnt = %d, score = %lld\n", loop_cnt, score);
            fail_cnt = 0;
          } else {
            ++fail_cnt;
            if (fail_cnt >= 1) break;
          }
        }
      }
      ++loop_cnt;
    }

    return best_commands;
  }

  int count_remain_cells(vector<short> &commands) {
    int remain_cnt = N * N - 1;
    ++g_vid;
    g_visited[SZ] = g_vid;

    for (int i = 0; i < commands.size() && remain_cnt > 0; ++i) {
      int cz = commands[i];
      if (g_visited[cz] != g_vid) {
        --remain_cnt;
      }

      g_visited[cz] = g_vid;
    }

    return remain_cnt;
  }

  vector<short> sa(vector<short> &original_commands, high_resolution_clock::time_point &begin) {
    fprintf(stderr, "start sa: %lf\n", get_time(begin));
    vector<short> cur_commands = original_commands;
    vector<short> best_commands = original_commands;

    ll cur_score = calc_score(cur_commands);
    ll best_score = cur_score;
    int L = cur_commands.size();
    int step_counter[GRID_SIZE * GRID_SIZE];
    memset(step_counter, 0, sizeof(step_counter));
    step_counter[SZ] = 1;
    for (int i = 0; i < L - 1; ++i) {
      int cz = cur_commands[i];
      step_counter[cz]++;
    }

    double total_diff = 0.0;
    double temp = 0.1;
    ll R = 100000000;
    int try_count = 0;
    double cur_time = 0.0;
    CHECK_RANGE = N;

    while (cur_time < TIME_LIMIT) {
      if (try_count % 10 == 0) {
        cur_time = get_time(begin);
      }
      int len = 2 + 1 * (xor128() % 7);
      int ext = 2 + 2 * (xor128() % 3);
      double remain_time = (TIME_LIMIT - cur_time) / TIME_LIMIT;
      int start_idx = xor128() % (L - len - 2 - ext) + 1;
      int end_idx = start_idx + len;

      assert(end_idx < cur_commands.size());
      assert(start_idx >= 1);
      int from_z = cur_commands[start_idx - 1];
      int to_z = cur_commands[end_idx];

      bool skip_check = true;
      for (int idx = start_idx; idx < end_idx; ++idx) {
        int z = cur_commands[idx];
        step_counter[z]--;
        if (step_counter[z] == 0) {
          skip_check = false;
        }
      }

      assert(end_idx < cur_commands.size());
      vector<short> new_commands = mini_beam(len, ext, cur_commands, from_z, to_z, start_idx, step_counter,
                                             skip_check);
      if (new_commands.empty()) {
        new_commands = mini_beam(len, ext, cur_commands, from_z, to_z, start_idx, step_counter, skip_check, false);
      }
      vector<short> before_commands = cur_commands;

      if (new_commands.size() > 0) {
        cur_commands.erase(cur_commands.begin() + start_idx, cur_commands.begin() + end_idx);
        cur_commands.insert(cur_commands.begin() + start_idx, new_commands.begin(), new_commands.end());

        ll score = calc_score(cur_commands);
        double diff_score = cur_score - score;
        total_diff += fabs(diff_score);

        if (diff_score > 0 ||
            (abs(diff_score) < best_score * 0.005 && xor128() % R < R * exp(diff_score / (temp * sqrt(remain_time))))) {
          cur_score = score;
          L = cur_commands.size();

          memset(step_counter, 0, sizeof(step_counter));
          for (int i = 0; i < L; ++i) {
            int cz = cur_commands[i];
            step_counter[cz]++;
          }

          if (best_score > score) {
            best_score = score;
            best_commands = cur_commands;
          }
        } else {
          cur_commands = before_commands;
          for (int idx = start_idx; idx < end_idx; ++idx) {
            int z = cur_commands[idx];
            step_counter[z]++;
          }
        }
      } else {
        for (int idx = start_idx; idx < end_idx; ++idx) {
          int z = cur_commands[idx];
          step_counter[z]++;
        }
      }

      ++try_count;
      double average_diff = total_diff / try_count;
      temp = 0.375 * remain_time * average_diff;
    }

    fprintf(stderr, "try_count: %d\n", try_count);
    return best_commands;
  }

  void run() {
    high_resolution_clock::time_point begin = high_resolution_clock::now();
    load_data();
    setup_wall_count_and_neighbors();
    setup_sum_d();
    setup_w_count();
    setup_distances();

    vector<short> best_commands;
    ll best_score = LLONG_MAX;
    int fail_cnt = 0;
    int retry_cnt = 0;
    int build_path_cnt = 0;
    map<int, bool> checked_params;
    while (get_time(begin) < 0.5) {
      int beam_depth = N * (4 + (xor128() % 5));
      if (W <= N * 0.25) {
        g_back_rate = 0.0 + 0.5 * (xor128() % 2);
      } else {
        g_back_rate = 0.3 + 0.1 * (xor128() % 7);
      }
      int key = beam_depth * 100 + g_back_rate * 10;
      if (checked_params[key]) {
        ++retry_cnt;
        if (retry_cnt >= 100) break;
        continue;
      }
      retry_cnt = 0;
      checked_params[key] = true;
      vector<short> commands = search_all_clean_path(beam_depth, 0.8);
      if (commands.empty()) continue;

      ll score = calc_score(commands);
      ++build_path_cnt;

      if (best_score > score) {
        best_score = score;
        best_commands = commands;
        fprintf(stderr, "best_score: %lld\n", best_score);
        if (N >= 32) break;
      } else {
        ++fail_cnt;
        if (fail_cnt >= 2) break;
      }
    }

    fprintf(stderr, "build_path_cnt: %d\n", build_path_cnt);

    if (best_commands.empty()) {
      best_commands = safety_clean_path_search(begin, 1.5);
    }

    best_commands = sa(best_commands, begin);

    fprintf(stderr, "finished_at: %lf, score = %lld, L = %d\n", get_time(begin), calc_score(best_commands),
            (int) best_commands.size());
    vector<char> dirs = commands2dirs(best_commands);
    string output = "";
    for (int i = 0; i < dirs.size(); ++i) {
      int dir = dirs[i];
      output.append(DS[dir]);
    }
    cout << output << endl;
  }

  vector<char> commands2dirs(vector<short> &commands) {
    vector<char> dirs;

    for (int i = 0; i < commands.size() - 1; ++i) {
      int cz = commands[i];
      int cy = cz / GRID_SIZE;
      int cx = cz % GRID_SIZE;
      int nz = commands[i + 1];
      int ny = nz / GRID_SIZE;
      int nx = nz % GRID_SIZE;

      if (cy == ny) {
        if (cx < nx) {
          dirs.push_back(RIGHT);
        } else {
          dirs.push_back(LEFT);
        }
      } else {
        if (cy < ny) {
          dirs.push_back(DOWN);
        } else {
          dirs.push_back(UP);
        }
      }
    }

    return dirs;
  }
};

int main() {
  RecurringCleaningRoute rcr;
  rcr.run();

  return 0;
}