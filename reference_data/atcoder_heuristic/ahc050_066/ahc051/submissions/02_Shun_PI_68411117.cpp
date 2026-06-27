#include <string>
#pragma GCC target("avx2,fma")
#pragma GCC optimize("O3")
#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;
using lint = long long int;
using P = pair<int, int>;
using PL = pair<lint, lint>;
#define FOR(i, begin, end) for(int i=(begin),i##_end_=(end);i<i##_end_;i++)
#define IFOR(i, begin, end) for(int i=(end)-1,i##_begin_=(begin);i>=i##_begin_;i--)
#define REP(i, n) FOR(i,0,n)
#define IREP(i, n) IFOR(i,0,n)
#define ALL(a)  (a).begin(),(a).end()

#define FMA256(a,b,c) _mm256_fmadd_ps((a),(b),(c))

constexpr int MOD = 1000000007;
vector<lint> RH_B = {1532834020, 1388622299};
vector<lint> RH_M = {2147482409, 2147478017};
constexpr int INF = 2147483647;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }

//提出時1950 ローカルテスト2350
constexpr int OVERALL_TIME_LIMIT = 1975;
constexpr bool OPTUNA = false; //optuna実行前に必ずtrueにすること！

//チューニング対象パラメータ
//最後にseed1001~2000でテストしてよかった方を選ぶ
//{'EDGE_LIMIT_BASE': 10735, 'EDGE_LIMIT_FACTOR': 142.64545614358457, 'EDGE_LIMIT_POWER': 0.6493467573239262, 'INIT_ORDER_FACTOR': 0.7599196856203103, 'INIT_ORDER_POWER': 2.892076387063876, 'INIT_DIST_FACTOR': 0.6393667332553615, 'INIT_X_LIMIT': 6520, 'START_TEMP': -1.969497167794708, 'END_TEMP': -4.725108663344491, 'START_TEMP_N': -0.13511583242915715, 'END_TEMP_N': -0.06444333678510625, 'START_TEMP_M_PER_N': -0.30164409308087614, 'END_TEMP_M_PER_N': 0.07605496315118075, 'START_TEMP_K_PER_N': 0.10867989513208369, 'END_TEMP_K_PER_N': -0.32193631072811124, 'START_GETA': 0.8225722823641455, 'END_GETA': 5.216315028529046, 'START_MODE_RND_THRESHOLD': 42.234259309056505, 'END_MODE_RND_THRESHOLD': 34.93664676287958, 'START_MODE_RND_THRESHOLD_N': 41.02429552161886, 'END_MODE_RND_THRESHOLD_N': 17.315425981994128, 'START_BTOL_THRESHOLD': 53.67677833898665, 'END_BTOL_THRESHOLD': 50.7819522718333, 'GLOBAL_WALL_MARGIN': 0, 'REBUILD_PROB_THRESHOLD': 0.001955963755208709, 'REBUILD_INTERVAL': 125}
//{'EDGE_LIMIT_BASE': 8857, 'EDGE_LIMIT_FACTOR': 109.33340402723896, 'EDGE_LIMIT_POWER': 0.6887829961554253, 'INIT_ORDER_FACTOR': 0.537995637116094, 'INIT_ORDER_POWER': 3.1521962014506357, 'INIT_DIST_FACTOR': 0.7752913185752602, 'INIT_X_LIMIT': 5782, 'START_TEMP': -2.035794306652827, 'END_TEMP': -4.812182387939177, 'START_TEMP_N': -0.39188576293537325, 'END_TEMP_N': -0.12699912233421315, 'START_TEMP_M_PER_N': -0.4487280141885729, 'END_TEMP_M_PER_N': -0.1314206888843446, 'START_TEMP_K_PER_N': 0.3681804921170876, 'END_TEMP_K_PER_N': 0.03560494359443095, 'START_GETA': 1.2948222481485299, 'END_GETA': 4.3787075768196555, 'START_MODE_RND_THRESHOLD': 49.904939890486155, 'END_MODE_RND_THRESHOLD': 27.262512580960383, 'START_MODE_RND_THRESHOLD_N': 1.710741563719452, 'END_MODE_RND_THRESHOLD_N': 32.237454001777884, 'START_BTOL_THRESHOLD': 63.847312894028555, 'END_BTOL_THRESHOLD': 68.30061180484685, 'GLOBAL_WALL_MARGIN': 1, 'REBUILD_PROB_THRESHOLD': 0.0015792754120998777, 'REBUILD_INTERVAL': 303}
int EDGE_LIMIT_BASE = 8857;
double EDGE_LIMIT_FACTOR = 109.33340402723896;
double EDGE_LIMIT_POWER = 0.6887829961554253;
double INIT_ORDER_FACTOR = 0.537995637116094;
double INIT_ORDER_POWER = 3.1521962014506357;
double INIT_DIST_FACTOR = 0.7752913185752602;
int INIT_X_LIMIT = 5782;
double START_TEMP = -2.035794306652827;
double END_TEMP = -4.812182387939177;
double START_TEMP_N = -0.39188576293537325;
double END_TEMP_N = -0.12699912233421315;
double START_TEMP_M_PER_N = -0.4487280141885729;
double END_TEMP_M_PER_N = -0.1314206888843446;
double START_TEMP_K_PER_N = 0.3681804921170876;
double END_TEMP_K_PER_N = 0.03560494359443095;
double START_GETA = 1.2948222481485299;
double END_GETA = 4.3787075768196555;
double START_MODE_RND_THRESHOLD = 49.904939890486155;
double END_MODE_RND_THRESHOLD = 27.262512580960383;
double START_MODE_RND_THRESHOLD_N = 1.710741563719452;
double END_MODE_RND_THRESHOLD_N = 32.237454001777884;
double START_BTOL_THRESHOLD = 63.847312894028555;
double END_BTOL_THRESHOLD = 68.30061180484685;
int GLOBAL_WALL_MARGIN = 0;
double REBUILD_PROB_THRESHOLD = 0.0015792754120998777;
int REBUILD_INTERVAL = 303;


int N, M, K, K_padded;
vector<P> pos;
double prob[3000];
alignas(32) float prob_f[3000];
mt19937 mt(0);
constexpr float EPS = 1e-9;
constexpr int NODE_MAX = 1024;

uint32_t xor32(void) {
  static uint32_t y = 2463534242;
  y = y ^ (y << 13); y = y ^ (y >> 17);
  return y = y ^ (y << 5);
}

uint64_t s = 0x123456789abcdef0ULL; // 初期シード値
inline uint64_t xor64() {
  // xorshift64* (cheap & fast)
  uint64_t x = s;
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  s = x;
  return x * 2685821657736338717ULL;  // scramble
}

struct StopWatch {
  chrono::system_clock::time_point start_time;
  double elapsed_sum = 0.0;

  StopWatch() {}

  void start() {
    start_time = chrono::system_clock::now();
  }

  void stop() {
    auto end_time = chrono::system_clock::now();
    elapsed_sum += chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() / 1000.0;
  }
};

StopWatch stopwatch;

struct IndexSet {

  vector<int> que;
  vector<int> pos;

  IndexSet(int n) : pos(n, -1) {}

  void add(int v) {
    if(pos[v] != -1) return;
    pos[v] = que.size();
    que.push_back(v);
  }

  void remove(int v) {
    if(pos[v] == -1) return;
    int p = pos[v];
    int b = que.back();
    que[p] = b;
    que.pop_back();
    pos[b] = p;
    pos[v] = -1;
  }

  void change(int v) {
    int p = pos[v];
    if(p == -1) {
      add(v);
    } else {
      remove(v);
    }
  }

  bool contains(int v) const {
    return pos[v] != -1;
  }

  int size() const {
    return que.size();
  }

  void clear() {
    que.clear();
    fill(ALL(pos), -1);
  }
};


int sign(int x) {
  return (x > 0) - (x < 0);
}
int orientation(const P &a, const P &b, const P &c) {
  int cross = (b.first - a.first) * (c.second - a.second) - (b.second - a.second) * (c.first - a.first);
  return sign(cross);
}
bool segments_intersect(const P &p1, const P &p2, const P &q1, const P &q2) {
  if(p1 == q1 || p1 == q2 || p2 == q1 || p2 == q2) {
    return false; // 同じ点は交差しない
  }
  if (max(p1.first, p2.first) < min(q1.first, q2.first) ||
      max(q1.first, q2.first) < min(p1.first, p2.first) ||
      max(p1.second, p2.second) < min(q1.second, q2.second) ||
      max(q1.second, q2.second) < min(p1.second, p2.second)) {
      return false;
  }
  int o1 = orientation(p1, p2, q1);
  int o2 = orientation(p1, p2, q2);
  int o3 = orientation(q1, q2, p1);
  int o4 = orientation(q1, q2, p2);
  return (o1 * o2 <= 0) && (o3 * o4 <= 0);
}
double euclidean_distance(const P &a, const P &b) {
  return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

template <class RandomIt>
inline void fast_shuffle(RandomIt first, RandomIt last) {
  ptrdiff_t n = last - first;
  if (n <= 1) return;
  for (ptrdiff_t i = n - 1 - xor32() % 2; i > 0; i -= 2) {
    uint32_t j = xor32() % (i + 1);
    swap(first[i], first[(ptrdiff_t)j]);
  }
}


struct EdgeManager {
  vector<vector<int>> edge_candidates;
  vector<vector<bool>> valid_edge_mat;
  vector<P> edges;
  vector<int> edge_map;
  vector<vector<int>> edge_confliction_list;

  vector<int> edge_confliction_state;
  vector<tuple<int, int, int, int>> global_wall_list;
  vector<IndexSet> rnbr;

  void init() {
    int EDGE_LIMIT = EDGE_LIMIT_BASE / pow(M/EDGE_LIMIT_FACTOR, EDGE_LIMIT_POWER) / EDGE_LIMIT_FACTOR;
    //N~N+Mの頂点から、距離の近い順にEDGE_LIMIT個の頂点を選ぶ
    edge_candidates.resize(N + M + 1);
    valid_edge_mat.resize(N + M + 1, vector<bool>(N + M + 1, false));
    FOR(i, N, N+M+1) {
      vector<pair<double, int>> candidates;
      REP(j, N+M) {
        if (i == j) continue; // 自分自身は除外
        double dist = euclidean_distance(pos[i], pos[j]);
        candidates.emplace_back(dist, j);
      }
      sort(ALL(candidates));
      REP(j, min(EDGE_LIMIT, (int)candidates.size())) {
        if(!valid_edge_mat[i][candidates[j].second]) {
          edge_candidates[i].push_back(candidates[j].second);
          valid_edge_mat[i][candidates[j].second] = true;
        }
        if(!valid_edge_mat[candidates[j].second][i] && i != N+M && candidates[j].second >= N) {
          edge_candidates[candidates[j].second].push_back(i);
          valid_edge_mat[candidates[j].second][i] = true;
        }
      }
    }

    //全ての辺を列挙（重複を除く）
    edges = vector<P>();
    REP(i, N+M+1) FOR(j, i+1, N+M+1) {
      if(valid_edge_mat[i][j] || valid_edge_mat[j][i]) {
        edges.emplace_back(i, j);
      }
    }
    int E = edges.size();
    edge_map = vector<int>(NODE_MAX * NODE_MAX, -1);
    REP(i, E) {
      edge_map[edges[i].first * NODE_MAX + edges[i].second] = i;
      edge_map[edges[i].second * NODE_MAX + edges[i].first] = i;
    }

    //各辺について、他の辺との交差関係を記録
    edge_confliction_list.resize(E);
    REP(i, E) {
      FOR(j, i + 1, E) {
        if (segments_intersect(pos[edges[i].first], pos[edges[i].second], pos[edges[j].first], pos[edges[j].second])) {
          edge_confliction_list[i].push_back(j);
          edge_confliction_list[j].push_back(i);
        }
      }
    }

    edge_confliction_state = vector<int>(E);
    rnbr = vector<IndexSet>(N + M + 1, IndexSet(N + M + 1));
  }

  void clear() {
    edge_confliction_state = vector<int>(edges.size());
  }

  //つなぐ先と、その時破壊する辺ID
  vector<P> get_valid_candidates(int v, bool allow_remove_edge = true) {
    vector<P> valid_candidates;
    for (int u : edge_candidates[v]) {
      int edge_index = edge_map[v * NODE_MAX + u];
      if(edge_confliction_state[edge_index] == 0) {
        valid_candidates.emplace_back(u, -1);
      }
    }
    return valid_candidates;
  }

  P get_random_valid_candidate(int v, bool allow_remove_edge = true) {
    vector<P> valid_candidates = get_valid_candidates(v, allow_remove_edge);
    if(valid_candidates.empty()) return P(-1, -1);
    return valid_candidates[xor32() % valid_candidates.size()];
  }

  P get_random_valid_candidate_fast(int v) {
    fast_shuffle(edge_candidates[v].begin(), edge_candidates[v].end());
    for (int u : edge_candidates[v]) {
      int edge_index = edge_map[v * NODE_MAX + u];
      if(edge_confliction_state[edge_index] == 0) {
        return {u, -1};
      }
    }
    return P(-1, -1);
  }

  void add_edges(int u, int v) {
    int edge_index = edge_map[u * NODE_MAX + v];
    for(int conflicted_edge : edge_confliction_list[edge_index]) {
      edge_confliction_state[conflicted_edge]++;
    }
  }
  void remove_edges(int u, int v) {
    int edge_index = edge_map[u * NODE_MAX + v];
    for(int conflicted_edge : edge_confliction_list[edge_index]) {
      edge_confliction_state[conflicted_edge]--;
    }
  }

  void set_global_wall(int x1, int y1, int x2, int y2) {
    //仮想的な壁を追加する
    global_wall_list.emplace_back(x1, y1, x2, y2);
    //global_wall_listの各辺に対して、edge_confliction_stateを更新する
    REP(i, edges.size()) {
      P edge = edges[i];
      if(segments_intersect({x1, y1}, {x2, y2}, pos[edge.first], pos[edge.second])) {
        edge_confliction_state[i]++;
      }
    }
  }

  void reset_global_wall() {
    for(auto &wall : global_wall_list) {
      int x1 = get<0>(wall);
      int y1 = get<1>(wall);
      int x2 = get<2>(wall);
      int y2 = get<3>(wall);
      //global_wall_listの各辺に対して、edge_confliction_stateを更新する
      REP(i, edges.size()) {
        P edge = edges[i];
        if(segments_intersect({x1, y1}, {x2, y2}, pos[edge.first], pos[edge.second])) {
          edge_confliction_state[i]--;
        }
      }
    }
    global_wall_list.clear();
  }
};

vector<int> visited_check_loop;
int visited_check_loop_cnt = 0;
//check_loopをメモ化する
vector<vector<int>> memo_check_loop;
int memo_check_loop_cnt = 1;// 辺が変わったらインクリメント
template <typename T>
bool check_loop(int u, int v, const T &outs) {
  //頂点uから頂点vまでのパスが存在するかを確認する
  if(u < N) return false;
  if(v == N+M) return false;
  if(u == v) return true;
  if(memo_check_loop[u][v] == memo_check_loop_cnt) return true;
  else if(memo_check_loop[u][v] == -memo_check_loop_cnt) return false;
  visited_check_loop_cnt++;
  queue<int> que;
  que.push(u);
  visited_check_loop[u] = visited_check_loop_cnt;
  while(!que.empty()) {
    int cur = que.front();
    que.pop();
    if(cur == v || memo_check_loop[cur][v] == memo_check_loop_cnt) {
      memo_check_loop[u][v] = memo_check_loop_cnt;
      return true;
    }
    int out1 = outs[cur].first;
    int out2 = outs[cur].second;
    if(out1 != -1 && visited_check_loop[out1] != visited_check_loop_cnt) {
      visited_check_loop[out1] = visited_check_loop_cnt;
      que.push(out1);
    }
    if(out2 != -1 && visited_check_loop[out2] != visited_check_loop_cnt) {
      visited_check_loop[out2] = visited_check_loop_cnt;
      que.push(out2);
    }
  }
  memo_check_loop[u][v] = -memo_check_loop_cnt;
  return false;
}


bool search(int v, vector<bool> &visited, vector<P> &outs, int order, EdgeManager &edge_manager) {
  visited[v] = true;
  vector<P> valid_candidates = edge_manager.get_valid_candidates(v, false);
  vector<pair<double, int>> candidates;
  memo_check_loop_cnt++;
  for(P p : valid_candidates) {
    int i = p.first;
    if(check_loop(i, v, outs)) continue; // ループができる場合はスキップ
    double dist = euclidean_distance(pos[v], pos[i]);
    if(i < N) {
      dist *= 1.0 - pow((double)order / M, INIT_ORDER_POWER) * INIT_ORDER_FACTOR;
    }
    if(outs[v].first != -1) {
      if(outs[v].first == i) continue;
      dist -= euclidean_distance(pos[outs[v].first], pos[i]) * INIT_DIST_FACTOR;
    }
    candidates.emplace_back(dist, i);
  }
  if(candidates.empty()) {
    return false;
  }
  sort(ALL(candidates));

  int cnt = 0;
  for(auto &candidate : candidates) {
    int u = candidate.second;

    // 接続可能ならば接続
    if(outs[v].first == -1) {
      outs[v].first = u; // vからuへの接続を記録
    } else if(outs[v].second == -1) {
      outs[v].second = u; // vからuへの2つ目の接続を記録
    }
    edge_manager.add_edges(v, u);
    edge_manager.rnbr[u].add(v);
    return true;
  }
  if(outs[v].first != -1) {
    return true;
  }
  return false;
}

void add_global_wall(EdgeManager &edge_manager, int X_LIMIT=INIT_X_LIMIT) {
  //スタートからゴールまでを遠回りさせるために仮想的な壁を配置する
  //x=0において[(0,0),(0,10000)]からスタートし、x座標の昇順にゴール座標で区間を分割する
  //最終的に最も広い区間に対して線分を引き仮想的な壁を構築する
  //EdgeManagerにおいて仮想的な壁と交差する場所全てに仮想IDを設定し通行不能にする
  vector<P> goal_list(N);
  REP(i, N) {
    goal_list[i] = {pos[i].first, pos[i].second};
  }
  sort(ALL(goal_list));

  //上下の壁を構築
  //up_y、down_yの初期値を探索する
  //最終的なdown_y - up_yが最も大きいものを採用
  int best_width = 0;
  vector<tuple<int, int, int, int>> best_up_wall, best_down_wall;
  for(int _up_y = -1000*GLOBAL_WALL_MARGIN; _up_y <= 1000*GLOBAL_WALL_MARGIN; _up_y += 1000) {
    for(int _down_y = 10000-1000*GLOBAL_WALL_MARGIN; _down_y <= 10000+1000*GLOBAL_WALL_MARGIN; _down_y += 1000) {
      vector<tuple<int, int, int, int>> up_wall, down_wall;
      int up_x = 0, up_y = _up_y;
      int down_x = 0, down_y = _down_y;
      REP(i, goal_list.size()) {
        P wall = goal_list[i];
        int x = wall.first;
        int y = wall.second;
        if(x > X_LIMIT) break; // X_LIMITを超える場合は無視
        if(y < (up_y + down_y) / 2) {
          int next_x = x;
          int next_y = y + 1;
          if(y < up_y) next_y = up_y;
          up_wall.emplace_back(up_x, up_y, next_x, next_y);
          up_x = next_x;
          up_y = next_y;
        } else {
          int next_x = x;
          int next_y = y - 1;
          if(y > down_y) next_y = down_y;
          down_wall.emplace_back(down_x, down_y, next_x, next_y);
          down_x = next_x;
          down_y = next_y;
        }
      }
      up_wall.emplace_back(up_x, up_y, X_LIMIT, up_y);
      down_wall.emplace_back(down_x, down_y, X_LIMIT, down_y);

      chmax(up_y, 0);
      chmin(down_y, 10000);
      int width = down_y - up_y;
      if(width > best_width) {
        best_width = width;
        best_up_wall = up_wall;
        best_down_wall = down_wall;
      }
      //cerr << "up_y: " << _up_y << ", down_y: " << _down_y << ", width: " << width << endl;
    }
  }

  //仮想的な壁をEdgeManagerに登録
  for(auto &wall : best_up_wall) {
    int x1 = get<0>(wall);
    int y1 = get<1>(wall);
    int x2 = get<2>(wall);
    int y2 = get<3>(wall);
    edge_manager.set_global_wall(x1, y1, x2, y2);
  }
  for(auto &wall : best_down_wall) {
    int x1 = get<0>(wall);
    int y1 = get<1>(wall);
    int x2 = get<2>(wall);
    int y2 = get<3>(wall);
    edge_manager.set_global_wall(x1, y1, x2, y2);
  }

}


vector<P> build_outs(EdgeManager &edge_manager) {
  vector<P> outs(N+M+1, {-1, -1});

  //BFSで木を構築する
  //接続先は距離の小さい順に探索し、まだ訪れていない経由点 or ゴールでありかつ交差が無ければ接続する
  vector<bool> visited(N+M+1, false);
  vector<bool> queued(N+M+1, false);
  queued[N+M] = true;
  deque<int> queue;
  queue.push_back(N+M);
  int order = 0;

  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();
    REP(i, 2) {
      bool ret = search(v, visited, outs, order, edge_manager);
      if(!ret || v == N + M) break;
    }
    order++;

    if(outs[v].first != -1 && !queued[outs[v].first] && outs[v].first >= N) {
      queued[outs[v].first] = true;
      queue.push_back(outs[v].first);
    }
    if(outs[v].second != -1 && !queued[outs[v].second] && outs[v].second >= N) {
      queued[outs[v].second] = true;
      queue.push_back(outs[v].second);
    }
  }
  //残った頂点にも適当に辺を引く
  FOR(i, N, N+M) if(!queued[i]) {
    REP(j, 2) {
      search(i, visited, outs, order, edge_manager);
    }
    order++;
  }

  return outs;
}

struct State {
  P *outs, *outs_tmp;
  int *idx, *idx_tmp;
  float *dp, *dp_tmp;
  float *type_prob, *type_prob_tmp;
  vector<IndexSet> rnbr;

  size_t outs_size, idx_size, dp_size, type_prob_size;

  State(vector<P> &outs_init, vector<int> &idx_init) {
    dp = new float[(N + M) * N];
    dp_tmp = new float[(N + M) * N];
    outs = new P[(N + M + 1)];
    outs_tmp = new P[(N + M + 1)];
    idx = new int[(N + M)];
    idx_tmp = new int[(N + M)];
    type_prob = new float[(N + M) * N];
    type_prob_tmp = new float[(N + M) * N];
    fill_n(dp, (N + M) * N, 0.0f);
    fill_n(dp_tmp, (N + M) * N, 0.0f);
    fill_n(type_prob, (N + M) * N, 1.0f);
    fill_n(type_prob_tmp, (N + M) * N, 1.0f);
    memcpy(outs, outs_init.data(), sizeof(P) * (N + M + 1));
    memcpy(idx, idx_init.data(), sizeof(int) * (N + M));

    outs_size = sizeof(P) * (N + M + 1);
    idx_size = sizeof(int) * (N + M);
    dp_size = sizeof(float) * (N + M) * N;
    type_prob_size = sizeof(float) * (N + M) * N;
  }
  void init() {

    // 小さなデータは通常のmemcpy
    memcpy(outs_tmp, outs, outs_size);
    memcpy(idx_tmp, idx, idx_size);

    // 大きなデータはSIMD最適化
    fast_memcpy_float(dp_tmp, dp, (N + M) * N);
    fast_memcpy_float(type_prob_tmp, type_prob, (N + M) * N);
  }

  void reset() {
    // ポインタswapで瞬時切り替え（既に最適）
    swap(outs, outs_tmp);
    swap(idx, idx_tmp);
    swap(dp, dp_tmp);
    swap(type_prob, type_prob_tmp);
  }

  State get_copy() {
    vector<P> outs_init(N + M + 1);
    vector<int> idx_init(N + M);
    memcpy(outs_init.data(), outs_tmp, sizeof(P) * (N + M + 1));
    memcpy(idx_init.data(), idx_tmp, sizeof(int) * (N + M));
    State ret(outs_init, idx_init);
    REP(i, N+M) REP(j, N) {
      ret.dp[i*N+j] = dp[i*N+j];
      ret.type_prob[i*N+j] = type_prob[i*N+j];
    }
    return ret;
  }

private:
  // AVX2を使った高速float配列コピー（8並列）
  void fast_memcpy_float(float* dst, const float* src, size_t count) {
    size_t i = 0;
    const size_t simd_count = (count / 8) * 8; // 8要素ずつ処理

    // AVX2で8つのfloatを同時コピー
    for(; i < simd_count; i += 8) {
      __m256 data = _mm256_loadu_ps(&src[i]);
      _mm256_storeu_ps(&dst[i], data);
    }

    // 残りをスカラーでコピー
    for(; i < count; i++) {
      dst[i] = src[i];
    }
  }

};


vector<int> in_degree;
int queued_cnt = 0;
vector<int> queued;
alignas(32) float prob_sum_f[320]; // 2*K_paddedまで対応

vector<int> select_cnt;
vector<int> type_map;

double calc_score(State &state, EdgeManager &edge_manager, vector<int> &start_idx) {

  int start = state.outs[N+M].first;
  deque<int> queue;

  //与えられた始点よりも親のノードのみを更新する
  queued_cnt++;
  for(int x : start_idx) {
    queue.push_back(x);
    queued[x] = queued_cnt;
  }

  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();
    REP(i, N) state.dp[v*N+i] = 0.0f; //初期化
    int s = edge_manager.rnbr[v].que.size();
    REP(i, s) {
      int u = edge_manager.rnbr[v].que[i];
      if(u == N+M) continue;
      in_degree[u]++;
      if(queued[u] != queued_cnt) {
        queue.push_back(u);
        queued[u] = queued_cnt;
      }
    }
  }

  for(int x : start_idx) {
    if(in_degree[x] == 0) queue.push_back(x);
  }

  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();

    //分別機の決定
    if(v >= N) {
      bool ok = true;
      int out1 = state.outs[v].first;
      int out2 = state.outs[v].second;
      if(out1 != -1 && state.idx[out1] == -1) out1 = -1;
      if(out2 != -1 && state.idx[out2] == -1) out2 = -1;
      if(out1 == -1 && out2 == -1) ok = false;

      if(ok) {
        if(out1 != -1 && out2 != -1) {
          assert(out1 != out2);

          // AVX2最適化版の内積計算（8並列）
          fill_n(prob_sum_f, K_padded, 0.0f);
          float v1_sum = 0.0f;
          float v2_sum = 0.0f;
          int dp1_idx = out1 * N;
          int dp2_idx = out2 * N;
          int type_prob_idx = v * N;
          REP(i, N) {
            const float dp1 = state.dp[dp1_idx+i];
            const float dp2 = state.dp[dp2_idx+i];
            const float tp  = state.type_prob[type_prob_idx+i];
            const float s   = (dp1 + dp2) * tp;
            if (s < EPS) continue;

            v1_sum += dp1 * tp;
            v2_sum += dp2 * tp;

            __m256 d_vec = _mm256_set1_ps((dp1 - dp2) * tp);
            const float* __restrict p_row = &prob_f[i*K_padded];
            float* __restrict sum = prob_sum_f;

            for (int k = 0; k < K_padded; k += 8) {
              __m256 p = _mm256_load_ps(p_row + k);
              __m256 cur = _mm256_load_ps(sum + k);
              cur = FMA256(d_vec, p, cur);       // ← fmadd
              _mm256_store_ps(sum + k, cur);
            }
          }

          float max_prob = prob_sum_f[0];
          int max_type = 0;
          float min_prob = prob_sum_f[0];
          int min_type = 0;
          FOR(k, 1, K) {
            float p = prob_sum_f[k];
            if(p > max_prob) {
              max_prob = p;
              max_type = k;
            }
            if(p < min_prob) {
              min_prob = p;
              min_type = k;
            }
          }

          const float cand1 = v2_sum + max_prob;
          const float cand2 = v1_sum - min_prob;
          int best_type = -1;
          bool swap_out = false;
          if(cand1 > cand2) {
            best_type = max_type;
          } else {
            swap_out = true;
            best_type = min_type;
          }
          select_cnt[best_type]++;

          state.idx[v] = best_type;
          if(swap_out) {
            state.outs[v].first = out2;
            state.outs[v].second = out1;
            out1 = state.outs[v].first;
            out2 = state.outs[v].second;
          }
          if(state.idx[v] != -1) {
            REP(i, N) {
              float dp1 = state.dp[out1*N+i];
              float dp2 = state.dp[out2*N+i];
              state.dp[v*N+i] += (dp1 - dp2) * (float)prob[i*K+state.idx[v]] + dp2;
            }
          }
        } else {
          //なんでもOK
          if(state.idx[v] == -1) state.idx[v] = xor32() % K;
          REP(i, N) {
            if(out1 != -1) state.dp[v*N+i] += state.dp[out1*N+i];
            else if(out2 != -1) state.dp[v*N+i] += state.dp[out2*N+i];
          }
        }
      }
    } else {
      //v < Nのとき
      state.dp[v*N+state.idx[v]] = 1.0f;
    }

    //伝搬
    for(int u : edge_manager.rnbr[v].que) {
      if(u == N+M) continue;
      in_degree[u]--;
      assert(in_degree[u] >= 0);
      if(in_degree[u] == 0) {
        queue.push_back(u);
      }
    }
  }

  if(state.idx[start] == -1) return 1.0;

  double score = 0.0;
  REP(i, N) score += (double)state.dp[start*N+i];

  return 1.0 - score / N;
}



//厳密に求めない方がスコアがいい？
vector<int> degree;
void calc_type_prob(State &state, double geta) {

  deque<int> queue;
  int start = state.outs[N+M].first;

  //type_probの計算（高速なスコア差分計算のため）
  FOR(i, N, N+M) REP(j, N) state.type_prob[i*N+j] = (float)geta;
  degree.assign(N + M, 0);
  queue.push_back(start);
  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();
    if(state.idx[v] == -1) continue; // 無効化された頂点は無視
    int out1 = state.outs[v].first;
    int out2 = state.outs[v].second;
    if(out1 != -1) {
      degree[out1]++;
      if(degree[out1] == 1) {
        queue.push_back(out1);
      }
    }
    if(out2 != -1) {
      degree[out2]++;
      if(degree[out2] == 1) {
        queue.push_back(out2);
      }
    }
  }
  queue.push_back(start);
  REP(i, N) state.type_prob[start*N+i] = 1.0f; // 始点の確率は1.0
  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();
    if(state.idx[v] == -1) continue; // 無効化された頂点は無視
    if(v < N) continue;
    if(v >= N) {
      int out1 = state.outs[v].first;
      int out2 = state.outs[v].second;
      if(out1 != -1 && out2 != -1) {
        REP(i, N) {
          int idx = v*N+i;
          float p = state.type_prob[idx] * (float)prob[i*K+state.idx[v]];
          state.type_prob[out1*N+i] += p;
          state.type_prob[out2*N+i] += state.type_prob[idx] - p;
        }
        degree[out1]--;
        if(degree[out1] == 0) {
          queue.push_back(out1);
        }
        degree[out2]--;
        if(degree[out2] == 0) {
          queue.push_back(out2);
        }
        continue;
      }
      if(out1 != -1) {
        REP(i, N) {
          state.type_prob[out1*N+i] += state.type_prob[v*N+i] * (float)prob[i*K+state.idx[v]];
        }
        degree[out1]--;
        if(degree[out1] == 0) {
          queue.push_back(out1);
        }
      }
      if(out2 != -1) {
        REP(i, N) {
          state.type_prob[out2*N+i] += state.type_prob[v*N+i] * (1.0f - (float)prob[i*K+state.idx[v]]);
        }
        degree[out2]--;
        if(degree[out2] == 0) {
          queue.push_back(out2);
        }
      }
    }
  }
}


void cleanse_state(State &state, EdgeManager &edge_manager, bool is_submit = false) {
  //解の補正
  //スタートから到達可能　かつ　ゴールから逆に見て到達可能な頂点以外を全て無効化する
  vector<vector<int>> rnbr(N+M);
  FOR(i, N, N+M) {
    if(state.outs[i].first != -1) rnbr[state.outs[i].first].push_back(i);
    if(state.outs[i].second != -1) rnbr[state.outs[i].second].push_back(i);
  }
  vector<bool> visited_1(N+M, false);
  int start = state.outs[N+M].first;
  deque<int> queue;
  queue.push_back(start);
  visited_1[start] = true;
  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();
    if(state.outs[v].first != -1 && !visited_1[state.outs[v].first]) {
      visited_1[state.outs[v].first] = true;
      queue.push_back(state.outs[v].first);
    }
    if(state.outs[v].second != -1 && !visited_1[state.outs[v].second]) {
      visited_1[state.outs[v].second] = true;
      queue.push_back(state.outs[v].second);
    }
  }

  vector<bool> visited_2(N+M, false);
  REP(i, N) {
    queue.push_back(i);
    visited_2[i] = true;
  }

  while(!queue.empty()) {
    int v = queue.front();
    queue.pop_front();
    for(int u : rnbr[v]) {
      if(!visited_2[u]) {
        visited_2[u] = true;
        queue.push_back(u);
      }
    }
  }

  FOR(i, N, N+M) {
    if(!visited_1[i] || !visited_2[i]) {
      state.idx[i] = -1; // 分別機のidxを-1にする
    }
  }

  //-1を指し示している頂点を無効化
  deque<int> invalid_queue;
  FOR(i, N, N+M) {
    if(state.idx[i] == -1) {
      invalid_queue.push_back(i);
    }
  }
  vector<bool> out1_ng(N+M, false);
  vector<bool> out2_ng(N+M, false);
  REP(i, N+M) {
    if(state.outs[i].first == -1) out1_ng[i] = true;
    if(state.outs[i].second == -1) out2_ng[i] = true;
  }
  while(!invalid_queue.empty()) {
    int v = invalid_queue.front();
    invalid_queue.pop_front();
    assert(v != start);
    for(int u : rnbr[v]) {
      if(state.outs[u].first == v) {
        out1_ng[u] = true;
        if(out2_ng[u]) {
          state.idx[u] = -1; // idxも無効化
          invalid_queue.push_back(u);
        }
      } else if(state.outs[u].second == v) {
        out2_ng[u] = true;
        if(out1_ng[u]) {
          state.idx[u] = -1; // idxも無効化
          invalid_queue.push_back(u);
        }
      }
    }
  }

  if(!is_submit) return;

  REP(i, N+M) {
    if(out1_ng[i]) state.outs[i].first = -1;
    if(out2_ng[i]) state.outs[i].second = -1;
  }

  //片方だけ設定されている頂点の対応
  FOR(i, N, N+M) {
    if(state.outs[i].first != -1 && state.outs[i].second == -1) {
      state.outs[i].second = state.outs[i].first;
    } else if(state.outs[i].first == -1 && state.outs[i].second != -1) {
      state.outs[i].first = state.outs[i].second;
    }
  }
}


void rebuild_prob(State &state) {

  //現時点で使われていない分別機種を除外する
  int type_cnt_sum = 0;
  REP(i, K) type_cnt_sum += select_cnt[i];
  if(type_cnt_sum == 0) return;

  vector<bool> used(K, false);
  REP(i, N+M) {
    if(state.idx[i] != -1) {
      used[state.idx[i]] = true; // 使用されている分別機種を記録
    }
  }

  double TYPE_CNT_THRESHOLD = REBUILD_PROB_THRESHOLD * type_cnt_sum;
  int new_K = 0;
  REP(i, K) {
    if(select_cnt[i] >= TYPE_CNT_THRESHOLD || used[i]) new_K++;
  }
  if(new_K <= 5) return;
  double* new_prob = new double[new_K * N];

  int ptr = 0;
  vector<int> new_type_map(K, -1);
  vector<bool> mapped(N+M+1, false);
  REP(i, K) {
    if(select_cnt[i] >= TYPE_CNT_THRESHOLD || used[i]) {
      new_type_map[ptr] = type_map[i];
      //stateの更新
      REP(j, N+M) if(state.idx[j] == i && !mapped[j]) {
        state.idx[j] = ptr; // 新しいidxに更新
        state.idx_tmp[j] = ptr; // tmpも更新
      }
      //確率をnew_probにコピー
      REP(j, N) {
        new_prob[j * new_K + ptr] = prob[j * K + i];
      }
      ptr++;
    }
  }

  memcpy(prob, new_prob, sizeof(double) * new_K * N);
  type_map = new_type_map; // 新しいtype_mapを設定
  cerr << "Rebuild prob: K = " << K << " -> " << new_K << "\n";
  K = new_K;
  K_padded = (K + 7) / 8 * 8; // Kを8の倍数にパディング

  REP(i, K_padded) {
    REP(j, N) {
      if(i < K) {
        prob_f[j * K_padded + i] = new_prob[j * K + i];
      } else {
        prob_f[j * K_padded + i] = 0.0f; // パディング部分は0にする
      }
    }
  }
  select_cnt = vector<int>(K, 0);
}



State do_sa(vector<P> &outs, EdgeManager &edge_manager, int TIME_LIMIT) {

  auto start_time = chrono::system_clock::now();

  vector<int> idx(N+M);
  REP(i, N) idx[i] = i;

  State state(outs, idx);
  vector<int> start_idx_init;
  REP(i, N) start_idx_init.push_back(i);

  double best_score = calc_score(state, edge_manager, start_idx_init);
  calc_type_prob(state, 0.1);

  double best_score_2 = best_score;
  State best_state_2 = state;

  int loop = 0;
  double start_temp = START_TEMP, end_temp = END_TEMP;
  double start_geta = START_GETA, end_geta = END_GETA;
  double start_mode_rnd_threshold = START_MODE_RND_THRESHOLD, end_mode_rnd_threshold = END_MODE_RND_THRESHOLD;
  double mode_rnd_threshold, btol_threshold;
  //N, M/N, K/Nの値に応じて補正を入れる
  double N_factor = (N - 12.5) / 7.5;
  double M_factor = ((double)M / N - 30.0) / 20.0;
  double K_factor = ((double)K / N - 2.5) / 1.5;
  start_temp += N_factor * START_TEMP_N;
  end_temp += N_factor * END_TEMP_N;
  start_temp += M_factor * START_TEMP_M_PER_N;
  end_temp += M_factor * END_TEMP_M_PER_N;
  start_temp += K_factor * START_TEMP_K_PER_N;
  end_temp += K_factor * END_TEMP_K_PER_N;
  start_mode_rnd_threshold += N_factor * START_MODE_RND_THRESHOLD_N;
  end_mode_rnd_threshold += N_factor * END_MODE_RND_THRESHOLD_N;

  double temp = start_temp;
  double time;
  bool need_init = true;
  int erased_global_wall = 0;
  vector<int> start_idx;

  int scored = 0;
  int accept = 0;

  memo_check_loop_cnt++;

  while(true) {
    if(loop%500 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      //cerr << "Loop: " << loop << ", Time: " << time << "ms, Best Score: " << (int)round(1e9 * best_score) << "\n";
      if(time > TIME_LIMIT * 0.05 * erased_global_wall && erased_global_wall < 10) {
        edge_manager.reset_global_wall();
        erased_global_wall++;
        if(erased_global_wall < 10) add_global_wall(edge_manager, INIT_X_LIMIT - INIT_X_LIMIT / 10 * erased_global_wall);
      }
      if(loop%(500*REBUILD_INTERVAL) == 0 && loop > 0 && time < TIME_LIMIT * 0.8) rebuild_prob(state);
      mode_rnd_threshold = start_mode_rnd_threshold + (end_mode_rnd_threshold - start_mode_rnd_threshold) * time / (double)TIME_LIMIT;
      btol_threshold = START_BTOL_THRESHOLD + (END_BTOL_THRESHOLD - START_BTOL_THRESHOLD) * time / (double)TIME_LIMIT;
    }
    loop++;
    if(need_init) {
      state.init();
      start_idx.clear();
    }
    need_init = false;

    int mode_rnd = xor32() % 1024;
    int mode = 0;

    if(mode_rnd < mode_rnd_threshold) mode = 0;
    else mode = 2;
    int a, b, m, l, b2 = -1, l2 = -1;
    bool btol = false;

    if(mode == 0) {
      //N未満をランダムにswap
      int r = xor32() % (N*N);
      a = r / N;
      b = r % N;
      if(a == b) continue;
      swap(state.idx[a], state.idx[b]);
      start_idx = {a, b};
    } else {
      //分別機から出る出口をランダムに繋ぎ変える
      a = xor32() % (M+1) + N;
      //if(state.idx[a] == -1) continue;
      P cand = edge_manager.get_random_valid_candidate_fast(a);
      b = cand.first;
      if(state.outs[a].first == b) continue;
      if(state.outs[a].second == b) continue;
      if(check_loop(b, a, state.outs)) continue; // ループチェック

      m = xor32() % 2;
      if(a == N+M) m = 0;
      start_idx = {a};
      if(a == N+M) start_idx = {};

      if(m == 0) {
        l = state.outs[a].first;
        state.outs[a].first = b;
      } else {
        l = state.outs[a].second;
        state.outs[a].second = b;
      }
      if(l != -1) edge_manager.rnbr[l].remove(a);
      if(b != -1) edge_manager.rnbr[b].add(a);

      //a->lからa->bに変更した際、確率でb->lの線も引く
      btol = xor32() % 100 < btol_threshold;
      if(l != -1 && b >= N && edge_manager.valid_edge_mat[b][l] && btol && state.outs[b].first != l && state.outs[b].second != l) {
        //線分交差チェック
        int edge_index = edge_manager.edge_map[b * NODE_MAX + l];
        if(edge_manager.edge_confliction_state[edge_index] != 0) {
          btol = false;
        }
        if(check_loop(l, b, state.outs)) btol = false;
        if(btol) {
          int m2 = xor32() % 2;
          if(m2 == 0) {
            l2 = state.outs[b].first;
            state.outs[b].first = l;
          } else {
            l2 = state.outs[b].second;
            state.outs[b].second = l;
          }
          if(l2 != -1) edge_manager.rnbr[l2].remove(b);
          edge_manager.rnbr[l].add(b);
          start_idx.push_back(b);
        }
      } else btol = false;
    }

    need_init = true;

    double score = calc_score(state, edge_manager, start_idx);
    scored++;
    double diff = best_score - score;

    //double diff = calc_score_diff_fast(state, start_idx);
    temp = start_temp + (end_temp - start_temp) * time / (double)TIME_LIMIT;
    double prob = exp(diff*pow(0.1, temp));
    if(diff >= 0 || prob*(double)INF > (xor32()%INF)) {
      best_score = score;
      accept++;
      //cleanse_state(state);
      if(time > 0.8 * TIME_LIMIT && score < best_score_2) {
        best_score_2 = score;
        best_state_2 = state.get_copy();
      }

      double geta = start_geta + (end_geta - start_geta) * time / (double)TIME_LIMIT;
      geta = pow(0.1, geta);
      calc_type_prob(state, geta);
      if(mode == 2) {
        if(l != -1) edge_manager.remove_edges(a, l);
        if(b != -1) edge_manager.add_edges(a, b);
        if(btol) {
          if(l2 != -1 && l2 != a) edge_manager.remove_edges(b, l2);
          if(l != -1) edge_manager.add_edges(b, l);
        }
        memo_check_loop_cnt++;
      }
    } else {
      //rnbrを逆操作で戻す
      if(mode == 2) {
        if(l != -1) edge_manager.rnbr[l].add(a);
        if(b != -1) edge_manager.rnbr[b].remove(a);
        if(btol) {
          if(l2 != -1) edge_manager.rnbr[l2].add(b);
          edge_manager.rnbr[l].remove(b);
        }
      }
      state.reset();
    }
  }

  cerr << "Total loops: " << loop << ", Time: " << time << "ms, Best Score: " << (int)round(1e9 * best_score) << "\n";
  cerr << "SA Scored: " << scored << ", Rate: " << (double)scored / loop << "\n";
  cerr << "SA Accepted: " << accept << ", Rate: " << (double)accept / loop << "\n";

  return best_state_2;
}


int main(int argc, char *argv[])
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> M >> K;
  cerr << "N: " << N << ", M: " << M << ", K: " << K << "\n";
  pos.resize(N+M);
  REP(i, N+M) cin >> pos[i].first >> pos[i].second;
  pos.emplace_back(0, 5000);

  fill_n(prob, K*N, 0.0);
  REP(i, K) REP(j, N) cin >> prob[j*K+i];
  auto start_time = chrono::system_clock::now();

  //std::this_thread::sleep_for(std::chrono::milliseconds(500));

  if(OPTUNA) {
    if(argc == 0) {
      cerr << "Usage: ./a.out <input_file> <output_file>" << endl;
      return 1;
    } else {
      EDGE_LIMIT_BASE = atoi(argv[1]);
      EDGE_LIMIT_FACTOR = atof(argv[2]);
      EDGE_LIMIT_POWER = atof(argv[3]);
      INIT_ORDER_FACTOR = atof(argv[4]);
      INIT_ORDER_POWER = atof(argv[5]);
      INIT_DIST_FACTOR = atof(argv[6]);
      INIT_X_LIMIT = atoi(argv[7]);
      START_TEMP = atof(argv[8]);
      END_TEMP = atof(argv[9]);
      START_TEMP_N = atof(argv[10]);
      END_TEMP_N = atof(argv[11]);
      START_TEMP_M_PER_N = atof(argv[12]);
      END_TEMP_M_PER_N = atof(argv[13]);
      START_TEMP_K_PER_N = atof(argv[14]);
      END_TEMP_K_PER_N = atof(argv[15]);
      START_GETA = atof(argv[16]);
      END_GETA = atof(argv[17]);
      START_MODE_RND_THRESHOLD = atof(argv[18]);
      END_MODE_RND_THRESHOLD = atof(argv[19]);
      START_MODE_RND_THRESHOLD_N = atof(argv[20]);
      END_MODE_RND_THRESHOLD_N = atof(argv[21]);
      START_BTOL_THRESHOLD = atof(argv[22]);
      END_BTOL_THRESHOLD = atof(argv[23]);
      GLOBAL_WALL_MARGIN = atoi(argv[24]);
      REBUILD_PROB_THRESHOLD = atof(argv[25]);
      REBUILD_INTERVAL = atoi(argv[26]);
    }
  }

  // Kを8の倍数にパディング（AVX2用）
  K_padded = ((K + 7) / 8) * 8;
  // float配列にコピー（SIMD用、パディングあり）
  fill_n(prob_f, K_padded*N, 0.0f);
  REP(j, N) {
    REP(i, K) {
      prob_f[j*K_padded+i] = (float)prob[j*K+i];
    }
  }

  vector<int> start_idx_init;
  REP(i, N) start_idx_init.push_back(i);

  visited_check_loop.resize(N+M+1);
  in_degree.resize(N+M+1, 0);
  degree.resize(N+M+1, 0);
  queued.resize(N+M+1, false);
  memo_check_loop.resize(N+M+1, vector<int>(N+M+1, 0));
  select_cnt.resize(K, 0);
  type_map.resize(K, 0);
  REP(i, K) type_map[i] = i;

  EdgeManager edge_manager;
  edge_manager.init();
  cerr << "EdgeManager initialized in " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count() << "ms\n";

  //マクロなルートを形作るため仮想的な壁を立てる（改善が必要）
  add_global_wall(edge_manager);

  //グラフの構築
  vector<P> best_outs = build_outs(edge_manager);
  cerr << "Graph built in " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count() << "ms\n";

  //構築に失敗した場合のフォールバックをする
  vector<int> idx(N+M+1);
  REP(i, N) idx[i] = i;
  State init_state(best_outs, idx);
  double init_score = calc_score(init_state, edge_manager, start_idx_init);
  cerr << "Init score: " << init_score << "\n";
  if(init_score >= 0.99 || std::isnan(init_score) || std::isinf(init_score)) {
    cerr << "Init score is too high.\n";
    edge_manager.reset_global_wall();
    REP(i, N+M+1) edge_manager.rnbr[i].clear();
    best_outs = build_outs(edge_manager);
  }

  //グラフの形とゴールの種類を焼く（分配の種類は貪欲に決める）
  double sa_time = OVERALL_TIME_LIMIT - chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
  State best_state = do_sa(best_outs, edge_manager, sa_time);
  cerr << "Simulated Annealing completed in " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count() << "ms\n";

  //スコアの計算
  REP(i, N+M+1) edge_manager.rnbr[i].clear();
  REP(i, N+M+1) {
    if(best_state.outs[i].first != -1) edge_manager.rnbr[best_state.outs[i].first].add(i);
    if(best_state.outs[i].second != -1) edge_manager.rnbr[best_state.outs[i].second].add(i);
  }
  double best_score = calc_score(best_state, edge_manager, start_idx_init);

  cleanse_state(best_state, edge_manager, true);

  //解の出力
  REP(i, N) cout << best_state.idx[i] << (i!=N-1 ? " " : "");
  cout << "\n";
  //始点
  cout << best_state.outs[N+M].first << "\n";
  FOR(i, N, N+M) {
    if(best_state.idx[i] == -1) cout << -1 <<"\n";
    else cout << type_map[best_state.idx[i]] << " " << best_state.outs[i].first << " " << best_state.outs[i].second << "\n";
  }

  cerr << "StopWatch: " << (int)stopwatch.elapsed_sum << "us\n";
  cerr << (int)round(1e9 * best_score) << "\n";

  if(!OPTUNA) quick_exit(0);
}