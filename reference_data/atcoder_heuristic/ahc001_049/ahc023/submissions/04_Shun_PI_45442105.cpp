#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
using lint = long long int;
using P = pair<int, int>;
using PL = pair<lint, lint>;
#define FOR(i, begin, end) for(int i=(begin),i##_end_=(end);i<i##_end_;i++)
#define IFOR(i, begin, end) for(int i=(end)-1,i##_begin_=(begin);i>=i##_begin_;i--)
#define REP(i, n) FOR(i,0,n)
#define IREP(i, n) IFOR(i,0,n)
#define ALL(a)  (a).begin(),(a).end()
constexpr int MOD = 1000000007;
constexpr lint B1 = 1532834020;
constexpr lint M1 = 2147482409;
constexpr lint B2 = 1388622299;
constexpr lint M2 = 2147478017;
constexpr int INF = 2147483647;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }

constexpr bool IS_SYUUKEI = false;
constexpr double TIME_LIMIT = 1910;
constexpr int LOOP_CHECK_INTERVAL = 200000;
constexpr int CHECK_CROP_INTERVAL = 2000;
constexpr int SCORE_BASE = 25;
constexpr int TUIKA_VALUE_BASE = 11;
constexpr double TUIKA_VALUE_POW = 0.3;

/*
やること
・高速化
・焼きなましの評価関数設計
  ・空き区間のスコアを適切に設計したい
  ・例えば通路マスに仮に作物が入らなかったとしても、2ターン以上の空きを作ったら一定のスコアを与えておく方が通路を消しやすい？
*/

// https://algo-logic.info/bridge-lowlink/
struct Edge {
    int to;
};
using Graph = vector<vector<Edge>>;

/* Lowlink: グラフの関節点・橋を列挙する構造体
    作成: O(E+V)
    関節点の集合: vector<int> aps
    橋の集合: vector<P> bridges
*/
struct LowLink {
  
    const Graph &G;
    vector<int> used, ord, low;
    vector<int> aps;  // articulation points
    vector<P> bridges;
    LowLink(const Graph &G_) : G(G_) {
        used.assign(G.size(), 0);
        ord.assign(G.size(), 0);
        low.assign(G.size(), 0);
        int k = 0;
        for (int i = 0; i < (int)G.size(); i++) {
            if (!used[i]) k = dfs(i, k, -1);
        }
    }
    int dfs(int id, int k, int par) { // id:探索中の頂点, k:dfsで何番目に探索するか, par:idの親
        used[id] = true;
        ord[id] = k++;
        low[id] = ord[id];
        bool is_aps = false;
        int count = 0; // 子の数
        for (auto &e : G[id]) {
            if (!used[e.to]) {
                count++;
                k = dfs(e.to, k, id);
                low[id] = min(low[id], low[e.to]);
                if (par != -1 && ord[id] <= low[e.to]) is_aps = true; // 条件2を満たすので関節点
                if (ord[id] < low[e.to]) bridges.emplace_back(min(id, e.to), max(id, e.to));
            } else if (e.to != par) { // eが後退辺の時
                low[id] = min(low[id], ord[e.to]);
            }
        }
        if (par == -1 && count >= 2) is_aps = true; // 条件1を満たすので関節点
        if (is_aps) aps.push_back(id);
        return k;
    }
};

//ビームサーチ用状態
struct State {
  double score;
  int board[20*20];
  lint SDPtr[200];
  int crop_queue[20*20];
  int crop_queue_timer[20*20];
  bool operator < (const State &other) const { return score < other.score; }
  State get_copy() {
    State ret;
    ret.score = score;
    memcpy(ret.board, board, sizeof(board));
    memcpy(ret.SDPtr, SDPtr, sizeof(SDPtr));
    memcpy(ret.crop_queue, crop_queue, sizeof(crop_queue));
    memcpy(ret.crop_queue_timer, crop_queue_timer, sizeof(crop_queue_timer));
    return ret;
  };
};

vector<int> dx = {-1, 0, 1, 0};
vector<int> dy = {0, 1, 0, -1};

int T, H, W, i0, K;
double L;
vector<string> h, w;
vector<int> S, D;
vector<vector<vector<int>>> SD;
int SDSize[10000];
vector<vector<bool>> is_neighbor;

struct SDPtr {
  int ptr[10000];
  SDPtr() {
    fill_n(ptr, 10000, 0);
  }
  int get_SDrest(int s, int d) {
    int idx = (s-1)*100 + d-1;
    if(idx < 0 || idx >= 10000) return 0;
    int ret = SDSize[idx] - ptr[idx];
    return ret;
  };
  int get_cropidx(int s, int d) {
    int idx = (s-1)*100 + d-1;
    if(ptr[idx] == SDSize[idx]) return -1;
    return SD[s][d][ptr[idx]];
  };
  void incr_SDptr(int s, int d) {
    int idx = (s-1)*100 + d-1;
    assert(ptr[idx] < SDSize[idx]);
    ptr[idx]++;
  };
  void decr_SDptr(int s, int d) {
    int idx = (s-1)*100 + d-1;
    assert(ptr[idx] >= 0);
    ptr[idx]--;
  };
};

bool can_move(int x, int y, int d) {
  int nx = x + dx[d];
  int ny = y + dy[d];
  if(nx < 0 || nx >= H || ny < 0 || ny >= W) return false;
  if (d == 0) {
    return h[x-1][y] == '0';
  } else if (d == 1) {
    return w[x][y] == '0';
  } else if (d == 2) {
    return h[x][y] == '0';
  } else {
    return w[x][y-1] == '0';
  }
}

double GROUP_C = 0.9763962344406639;
double GROUP_POW = 0.5880848035021756;
double GROUP_A = 0.601597248712105;
double GROUP_L = 0.1514157242379738;
double TUURO_BASE = 0.4051054524728503;
double get_estscore(int gsize) {
  double est_work_rate = GROUP_C - pow((double)gsize / 400.0, GROUP_POW) * GROUP_A;
  est_work_rate += (L-1.0) * GROUP_L;
  chmin(est_work_rate, 1.0);
  chmax(est_work_rate, 0.0);
  double estscore = est_work_rate * gsize;
  return 1000000 * estscore / H / W;
};

double get_estscore_tuuro(int subtree_size) {
  double estscore = pow(TUURO_BASE, subtree_size);
  return 1000000 * estscore / H / W;
}

void calc_group_aps_sub(unordered_set<int> &gpos, bool *aps) {
  //特定のグループについて関節点の列挙をする
  int gsize = gpos.size();
  vector<int> gpos_list(ALL(gpos));
  Graph g(gsize);
  REP(i, gsize) REP(j, gsize) {
    if(!is_neighbor[gpos_list[i]][gpos_list[j]]) continue;
    g[i].push_back({j});
  }
  LowLink lowlink(g);
  for(int i : gpos_list) aps[i] = false;
  for(int i : lowlink.aps) aps[gpos_list[i]] = true;
};

void calc_group_aps_sub_2(int t, unordered_set<int> &gpos, bool *aps) {
  //特定のグループについて関節点の列挙をする
  int gsize = gpos.size();
  vector<int> gpos_list(ALL(gpos));
  Graph g(gsize);
  REP(i, gsize) REP(j, gsize) {
    if(!is_neighbor[gpos_list[i]][gpos_list[j]]) continue;
    g[i].push_back({j});
  }
  LowLink lowlink(g);
  for(int i : gpos_list) aps[t*20*20+i] = false;
  for(int i : lowlink.aps) aps[t*20*20+gpos_list[i]] = true;
};

void calc_group_aps(int *group, bool *aps) {
  //全グループについて関節点の列挙をする
  //間接点の列挙
  Graph g(H * W);
  REP(i, H) REP(j, W) {
    if(group[i*20+j] == -1) continue;
    REP(d, 4) {
      if(!can_move(i, j, d)) continue;
      int ni = i + dx[d];
      int nj = j + dy[d];
      if(group[ni*20+nj] != group[i*20+j]) continue;
      g[i * W + j].push_back({ni * W + nj});
    }
  }
  LowLink lowlink(g);
  fill_n(aps, 20*20, false);
  for(int i : lowlink.aps) aps[i] = true;
};

int dfs_for_empty_cell(int x, int y, int t, int *board, int *edge, int *group, vector<vector<int>> &SDptr, vector<tuple<int, int, int>> &ret, vector<vector<tuple<int, int, int>>> &pos_list) {
  int min_D = 100;
  assert(board[x*20+y] != -1);
  if(board[x*20+y] != -2) chmin(min_D, D[board[x*20+y]]);
  REP(dir, 4) if(can_move(x, y, dir)) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if(edge[x*20*4+y*4+dir] == 1) {
      //child
      chmin(min_D, dfs_for_empty_cell(nx, ny, t, board, edge, group, SDptr, ret, pos_list));
    } else if(edge[x*20*4+y*4+dir] == -1) {
      //group
      if(board[nx*20+ny] == -1) {
        int gidx = group[nx*20+ny];
        for(auto tup : pos_list[gidx]) {
          int px = get<1>(tup);
          int py = get<2>(tup);
          if(board[px*20+py] < 0) continue;
          int crop_idx = board[px*20+py];
          chmin(min_D, D[crop_idx]);
          break;
        }
      }
      else chmin(min_D, D[board[nx*20+ny]]);
    }
  }
  //作物の割り当てを探索
  if(board[x*20+y] == -2) {
    bool ok = false;
    IFOR(didx, t+1, min_D+1) FOR(sidx, t, didx) {
      if(ok) break;
      if(SD[sidx][didx].size() == SDptr[sidx][didx]) continue;
      int crop_idx = SD[sidx][didx][SDptr[sidx][didx]];
      SDptr[sidx][didx]++;
      ret.push_back({x, y, crop_idx});
      chmin(min_D, didx);
      ok = true;
    }
  }

  return min_D;
};

void dfs_for_empty_cell_init(int x, int y, int *board, int *edge, vector<vector<int>> &SDptr, vector<tuple<int, int, int>> &ret) {
  assert(board[x*20+y] != -1);
  //Dの昇順・Sの降順に貪欲に作物を決める
  if(board[x*20+y] == -2) {
    bool ok = false;
    FOR(didx, 1, 101) IFOR(sidx, 1, didx) {
      if(ok) break;
      if(SD[sidx][didx].size() == SDptr[sidx][didx]) continue;
      int crop_idx = SD[sidx][didx][SDptr[sidx][didx]];
      SDptr[sidx][didx]++;
      ret.push_back({x, y, crop_idx});
      ok = true;
    }
  }
  REP(dir, 4) if(can_move(x, y, dir)) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if(edge[x*20*4+y*4+dir] == 1) {
      //child
      dfs_for_empty_cell_init(nx, ny, board, edge, SDptr, ret);
    }
  }
};

bool dfs_for_empty_cell_check_group(int x, int y, int t, int *board, int *edge, int *group, vector<vector<bool>> &prohibit_group) {
  assert(board[x*20+y] != -1);
  bool ret = false;
  REP(dir, 4) if(can_move(x, y, dir)) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if(edge[x*20*4+y*4+dir] == 1) {
      //child
      if(dfs_for_empty_cell_check_group(nx, ny, t, board, edge, group, prohibit_group)) ret = true;
    } else if(edge[x*20*4+y*4+dir] == -1) {
      //group
      int gidx = group[nx*20+ny];
      if(gidx == -1) continue;
      if(prohibit_group[gidx][t]) ret = true;
      if(prohibit_group[gidx][t-1]) ret = true;
    }
  }
  return ret;
};

void dfs_for_empty_cell_set_prohibit(int x, int y, int t, int *board, int *edge, int *group, vector<vector<bool>> &prohibit_group, int len) {
  assert(board[x*20+y] != -1);
  bool ret = false;
  REP(dir, 4) if(can_move(x, y, dir)) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if(edge[x*20*4+y*4+dir] == 1) {
      //child
      dfs_for_empty_cell_set_prohibit(nx, ny, t, board, edge, group, prohibit_group, len);
    } else if(edge[x*20*4+y*4+dir] == -1) {
      //group
      int gidx = group[nx*20+ny];
      if(gidx == -1) continue;
      FOR(i, t, t+len-1) prohibit_group[gidx][i] = true;
    }
  }
};

//通路マスの部分木の大きさの計算（稼働率計算用）
int dfs_for_get_subtree_size(int x, int y, int *edge, vector<vector<int>> &subtree_size) {
  int ret = 1;
  REP(dir, 4) if(edge[x*20*4+y*4+dir] == 1) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    ret += dfs_for_get_subtree_size(nx, ny, edge, subtree_size);
  }
  subtree_size[x][y] = ret;
  return ret;
};

void dfs_node_check(int x, int y, int t, bool *node, bool *vst) {
  assert(node[t*20*20+x*20+y]);
  if(vst[x*20+y]) return;
  vst[x*20+y] = true;
  REP(dir, 4) if(can_move(x, y, dir)) {
    int nx = x + dx[dir];
    int ny = y + dy[dir];
    if(node[t*20*20+nx*20+ny]) dfs_node_check(nx, ny, t, node, vst);
  }
}

bool check_prohibit_group(int g, int S, int D, vector<vector<bool>> &prohibit_group, vector<vector<bool>> &prohibit_SD, int t) {
  if(D-S+1 <= 3 && t >= 2) return false;
  if(prohibit_group[g][D]) return false;
  if(prohibit_SD[S][D]) return false;
  return true;
};

bool check_can_add(int x, int y, int t, bool *node) {
  if(node[t*20*20+x*20+y]) return false;
  if(x == i0 && y == 0) return true;
  //接続チェック
  bool ok = false;
  REP(d, 4) {
    if(!can_move(x, y, d)) continue;
    int nx = x + dx[d];
    int ny = y + dy[d];
    if(node[t*20*20+nx*20+ny]) {
      ok = true;
      break;
    }
  }
  return ok;
};
bool check_can_del(int x, int y, int t, bool *node) {
  if(!node[t*20*20+x*20+y]) return false;
  if(x == i0 && y == 0) {
    bool ok = true;
    REP(d, 4) {
      if(can_move(x, y, d)) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if(node[t*20*20+nx*20+ny]) {
          ok = false;
          break;
        }
      }
    }
    return ok;
  }
  //関節点チェック
  //TODO: 自身を中心としたk*kの領域で自身からBFSして連結成分を調べる
  //次に自身への遷移を禁止した上でBFSして、連結成分が増えたら関節点
  bool ok = true;
  bool check[4], check_diag[4];
  fill_n(check, 4, false);
  fill_n(check_diag, 4, false);
  int check_cnt = 0;
  REP(d, 4) {
    if(can_move(x, y, d)) {
      int nx = x + dx[d];
      int ny = y + dy[d];
      if(node[t*20*20+nx*20+ny]) check[d] = true;
    }
  }
  REP(d, 4) {
    int d2 = (d + 1) % 4;
    if(check[d] && check[d2]) {
      int nx = x + dx[d] + dx[d2];
      int ny = y + dy[d] + dy[d2];
      if(!can_move(nx, ny, (d+2)%4) || !can_move(nx, ny, (d2+2)%4) || !node[t*20*20+nx*20+ny]) {
        ok = false;
        break;
      }
      check_diag[d] = true;
      check_cnt++;
    }
  }
  if(!ok) return false;
  if(check_cnt >= 3) return false;
  if(check_cnt == 2 && (check_diag[0] && check_diag[2] || check_diag[1] && check_diag[3])) return false;
  REP(d, 2) {
    if(check[d] && check[(d+2)%4]) {
      if(check_diag[d] && check_diag[(d+1)%4]) continue;
      if(check_diag[d+2] && check_diag[(d+3)%4]) continue;
      ok = false;
      break;
    }
  }
  return ok;
};


int main(int argc, char *argv[])
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> T >> H >> W >> i0;
  h.resize(H-1);
  REP(i, H-1) cin >> h[i];
  w.resize(H);
  REP(i, H) cin >> w[i];
  cin >> K;
  S.resize(K);
  D.resize(K);
  REP(i, K) cin >> S[i] >> D[i];
  auto start_time = chrono::system_clock::now();

  double START_TEMP = 1.7709236566177948, END_TEMP = 0.8293304015791998;
  if(argc > 1) {
    START_TEMP = atof(argv[1]);
    END_TEMP = atof(argv[2]);
  }

  REP(i, K) L += D[i] - S[i] + 1;
  L /= H * W * T;

  SD = vector<vector<vector<int>>>(T+1, vector<vector<int>>(T+1));
  REP(i, K) SD[S[i]][D[i]].push_back(i);
  REP(i, 100) REP(j, 100) SDSize[i*100+j] = SD[i+1][j+1].size();

  is_neighbor = vector<vector<bool>>(H*W, vector<bool>(H*W));
  REP(a, H*W) {
    int ax = a / W;
    int ay = a % W;
    REP(dir, 4) {
      int bx = ax + dx[dir];
      int by = ay + dy[dir];
      if(!can_move(ax, ay, dir)) continue;
      is_neighbor[a][bx*W+by] = true;
    }
  }


  //稼働率推定
  int Lbin = max(0, min(9, (int)floor((L - 1.0) * 10)));
  cerr << Lbin << "\n";
  vector<vector<int>> work_bunsi(401, vector<int>(101));
  vector<vector<int>> work_bunbo(401, vector<int>(101));
  vector<vector<int>> work_bunsi_tuuro(16, vector<int>(101));
  vector<vector<int>> work_bunbo_tuuro(16, vector<int>(101));

  mt19937 mt(0);


  /*** 1. 最適な通路マスの配置を焼きなます ***/

  int best_board[20*20];
  fill_n(best_board, 20*20, -1);
  int best_board_sub[20*20];
  fill_n(best_board_sub, 20*20, -1);
  best_board_sub[i0*20+0] = -2; //根ノード
  int best_edge[20*20*4];
  fill_n(best_edge, 20*20*4, -1);
  int best_edge_sub[20*20*4];
  fill_n(best_edge_sub, 20*20*4, -1);
  int best_group[20*20];
  fill_n(best_group, 20*20, -1);
  int best_dist[20*20];
  fill_n(best_dist, 20*20, -1);
  int largest_group_x = i0;
  int largest_group_y = 0;

  double best_estscore = 0;
  double best_estscore_sub = 0;
  double start_temp = 3.6;
  double end_temp = 3.4;
  double temp;
  int LOOP_CNT = 1000;

  REP(z, LOOP_CNT) {
    // cerr << z << " " << best_estscore_sub << " " << best_estscore << "\n";
    if(z%100 == 0) temp = start_temp + (end_temp - start_temp) * z / LOOP_CNT;

    //始点から適当にDFSして、その道は空きマスにする
    //空きマスに隣接する全てのマスから多点BFSしてマスをグループ化する
    int edge[20*20*4];
    memcpy(edge, best_edge_sub, sizeof(edge));
    int board[20*20];
    memcpy(board, best_board_sub, sizeof(board));

    if(mt() % 3 == 0 || z == 0) {
      //追加

      //始点の決定
      int sx = largest_group_x;
      int sy = largest_group_y;
      if(mt() % 3 == 0) {
        while(true) {
          sx = mt() % H;
          sy = mt() % W;
          if(board[sx*20+sy] == -2) break;
        }
      } else {
        //始点の隣の空きマスから始めたい
        int sd = mt() % 4;
        REP(k, 4) {
          int dir = (sd + k) % 4;
          int nx = sx + dx[dir];
          int ny = sy + dy[dir];
          if(can_move(sx, sy, dir) && board[nx*20+ny] == -2) {
            sx = nx;
            sy = ny;
            break;
          }
        }
      }
      //始点からBFS
      int dist_tmp[20*20];
      fill_n(dist_tmp, 20*20, INF);
      int prevd[20*20];
      fill_n(prevd, 20*20, -1);
      queue<int> q;
      q.push(sx*20+sy);
      dist_tmp[sx*20+sy] = 0;
      while(!q.empty()){
        int i = q.front() / 20;
        int j = q.front() % 20;
        q.pop();
        int sd = (prevd[i*20+j] + 1) % 4;
        REP(dd, 4){
          int k = (sd + dd) % 4;
          int ni = i + dx[k];
          int nj = j + dy[k];
          if(can_move(i, j, k) && dist_tmp[ni*20+nj] == INF && board[ni*20+nj] != -2){
            dist_tmp[ni*20+nj] = dist_tmp[i*20+j] + 1;
            prevd[ni*20+nj] = (k+2)%4;
            q.push(ni*20+nj);
          }
        }
      }
      //終点を決める
      int tx, ty;
      while(true) {
        tx = mt() % H;
        ty = mt() % W;
        if(dist_tmp[tx*20+ty] != INF) break;
      }
      //選んだ終点から始点までのパスを空きマスにする
      while(true) {
        board[tx*20+ty] = -2;
        if(sx == tx && sy == ty) break;
        int dir = prevd[tx*20+ty];
        int px = tx + dx[dir];
        int py = ty + dy[dir];
        edge[tx*20*4+ty*4+dir] = 0;
        tx = px;
        ty = py;
        edge[tx*20*4+ty*4+(dir+2)%4] = 1;
      }

    } else {
      //削除
      int remove_cnt = 1 + mt() % 5;
      REP(r, remove_cnt) {
        int x, y;
        REP(zzz, 1000) {
          if(zzz == 999) {
            x = -1;
            break;
          }
          x = mt() % H;
          y = mt() % W;
          if(board[x*20+y] != -2) continue;
          if(x == i0 && y == 0) continue;
          //葉ノードの判定
          bool is_leaf = true;
          REP(d, 4) if(edge[x*20*4+y*4+d] == 1) is_leaf = false;
          if(is_leaf) break;
        }

        if(x != -1) {
          board[x*20+y] = -1;
          REP(d, 4) {
            if(edge[x*20*4+y*4+d] == 0) {
              int nx = x + dx[d];
              int ny = y + dy[d];
              edge[x*20*4+y*4+d] = -1;
              edge[nx*20*4+ny*4+(d+2)%4] = -1;
            }
          }
        }
      }
    }

    int board_tmp[20*20];
    memcpy(board_tmp, board, sizeof(board_tmp));

    //グループ領域を作る
    //グループの大きさでダイクストラっぽくするとある程度均等に分けられる
    int dist[20*20];
    fill_n(dist, 20*20, INF);
    set<tuple<int, int, int>> q2;
    int group[20*20];
    fill_n(group, 20*20, -1);
    int gcnt = 0;
    REP(i, H) REP(j, W) if(board[i*20+j] == -1) {
      REP(d, 4) {
        if(can_move(i, j, d)) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if(board[ni*20+nj] != -2) continue;
          q2.insert({1, i, j});
          dist[i*20+j] = 1;
          group[i*20+j] = gcnt;
          gcnt++;
          break;
        }
      }
    }
    vector<int> gsize(gcnt, 1);
    while(!q2.empty()) {
      auto tup = *q2.begin();
      q2.erase(q2.begin());
      int i = get<1>(tup);
      int j = get<2>(tup);
      int cnt = 0;
      REP(k, 4) {
        if(can_move(i, j, k)) {
          int ni = i + dx[k];
          int nj = j + dy[k];
          if(dist[ni*20+nj] != INF) continue;
          if(board[ni*20+nj] == -2) continue;
          cnt++;
        }
      }
      int nd = gsize[group[i*20+j]] + cnt;
      REP(k, 4) {
        if(can_move(i, j, k)) {
          int ni = i + dx[k];
          int nj = j + dy[k];
          if(dist[ni*20+nj] != INF) continue;
          if(board[ni*20+nj] == -2) continue;
          dist[ni*20+nj] = nd;
          group[ni*20+nj] = group[i*20+j];
          gsize[group[i*20+j]]++;
          q2.insert({nd, ni, nj});
        }
      }
    }

    int largest_group = 0;
    int last_largest_group_x = largest_group_x;
    int last_largest_group_y = largest_group_y;
    REP(i, gcnt) if(gsize[i] > gsize[largest_group] || (gsize[i] == gsize[largest_group] && mt() % 2 == 0)) largest_group = i;
    REP(i, H) REP(j, W) if(group[i*20+j] == largest_group && dist[i*20+j] == 1) {
      largest_group_x = i;
      largest_group_y = j;
    }

    double estscore = 0;
    REP(i, gcnt) estscore += get_estscore(gsize[i]);
    vector<vector<int>> subtree_size(H, vector<int>(W));
    dfs_for_get_subtree_size(i0, 0, edge, subtree_size);
    REP(i, H) REP(j, W) if(subtree_size[i][j] >= 1) estscore += get_estscore_tuuro(subtree_size[i][j]);

    double prob = exp(((double)estscore-best_estscore_sub)*pow(0.1, temp));
    if(estscore >= best_estscore_sub || prob > (mt()%INF)/(double)INF) {
      best_estscore_sub = estscore;
      memcpy(best_board_sub, board_tmp, sizeof(best_board_sub));
      memcpy(best_edge_sub, edge, sizeof(best_edge_sub));

      if(estscore >= best_estscore) {
        best_estscore = estscore;
        memcpy(best_board, board_tmp, sizeof(best_board));
        memcpy(best_edge, edge, sizeof(best_edge));
        memcpy(best_group, group, sizeof(best_group));
        memcpy(best_dist, dist, sizeof(best_dist));
      }
    } else {
      largest_group_x = last_largest_group_x;
      largest_group_y = last_largest_group_y;
    }
  }


  /*** 2. 各グループの大きさを焼きなましで最適化 ***/

  int gcnt = 0;
  REP(i, H) REP(j, W) chmax(gcnt, best_group[i*20+j] + 1);
  vector<int> gsize(gcnt);
  REP(i, H) REP(j, W) if(best_group[i*20+j] != -1) gsize[best_group[i*20+j]]++;
  bool aps[20*20];
  calc_group_aps(best_group, aps);
  //関節点の差分計算のため、各グループの座標をunordered_setで保持
  vector<unordered_set<int>> gpos(gcnt);
  REP(i, H) REP(j, W) if(best_group[i*20+j] != -1) gpos[best_group[i*20+j]].insert(i*20+j);

  start_temp = 1.5;
  end_temp = 0.0;
  LOOP_CNT = 10000;

  REP(z, LOOP_CNT) {
    // cerr << z << " " << best_estscore_sub << " " << best_estscore << "\n";
    if(z%100 == 0) temp = start_temp + (end_temp - start_temp) * z / LOOP_CNT;
    int x, y, dir, nx, ny;
    while(true) {
      x = mt() % H;
      y = mt() % W;
      if(best_group[x*20+y] == -1) continue;
      if(aps[x*20+y]) continue;
      if(best_dist[x*20+y] == 1) continue;
      dir = mt() % 4;
      if(!can_move(x, y, dir)) continue;
      nx = x + dx[dir];
      ny = y + dy[dir];
      if(best_group[nx*20+ny] == -1) continue;
      if(best_group[nx*20+ny] == best_group[x*20+y]) continue;
      break;
    }
    int prev_group = best_group[x*20+y];
    int next_group = best_group[nx*20+ny];
    double estscore = best_estscore;
    estscore -= get_estscore(gsize[prev_group]);
    estscore -= get_estscore(gsize[next_group]);
    best_group[x*20+y] = next_group;
    gsize[prev_group]--;
    gsize[next_group]++;
    estscore += get_estscore(gsize[prev_group]);
    estscore += get_estscore(gsize[next_group]);

    double prob = exp(((double)estscore-best_estscore)*pow(0.1, temp));
    if(estscore >= best_estscore || prob > (mt()%INF)/(double)INF) {
      best_estscore = estscore;
      gpos[prev_group].erase(x*20+y);
      gpos[next_group].insert(x*20+y);
      calc_group_aps_sub(gpos[prev_group], aps);
      calc_group_aps_sub(gpos[next_group], aps);
    } else {
      best_group[x*20+y] = prev_group;
      gsize[prev_group]++;
      gsize[next_group]--;
    }
  }


  int group[20*20];
  memcpy(group, best_group, sizeof(group));
  int dist[20*20];
  memcpy(dist, best_dist, sizeof(dist));
  //distの再計算
  REP(i, H) REP(j, W) if(dist[i*20+j] != 1) dist[i*20+j] = INF;
  REP(i, H) REP(j, H) if(dist[i*20+j] == 1) {
    //各グループ内部でBFSする
    queue<int> q;
    q.push(i*20+j);
    while(!q.empty()){
      int i = q.front() / 20;
      int j = q.front() % 20;
      q.pop();
      REP(k, 4){
        int ni = i + dx[k];
        int nj = j + dy[k];
        if(can_move(i, j, k) && dist[ni*20+nj] == INF && group[ni*20+nj] == group[i*20+j]){
          dist[ni*20+nj] = dist[i*20+j] + 1;
          q.push(ni*20+nj);
        }
      }
    }
  }

  //グループサイズの大きい順にインデックスを降り直す
  vector<P> gsize_tmp(gcnt);
  REP(i, gcnt) gsize_tmp[i].first = gsize[i];
  REP(i, gcnt) gsize_tmp[i].second = i;
  sort(ALL(gsize_tmp), greater<P>());
  vector<int> gidx(gcnt);
  REP(i, gcnt) gidx[gsize_tmp[i].second] = i;
  REP(i, H) REP(j, W) if(group[i*20+j] != -1) group[i*20+j] = gidx[group[i*20+j]];

  vector<vector<tuple<int, int, int>>> pos_list(gcnt);
  REP(i, H) REP(j, W) if(group[i*20+j] != -1) pos_list[group[i*20+j]].push_back({dist[i*20+j], i, j});
  REP(i, gcnt) sort(ALL(pos_list[i]));
  gsize = vector<int>(gcnt);
  REP(i, gcnt) gsize[i] = pos_list[i].size();

  //稼働率推定のため、通路マスの部分木の大きさを計算
  vector<vector<int>> subtree_size_tuuro(H, vector<int>(W));
  dfs_for_get_subtree_size(i0, 0, best_edge, subtree_size_tuuro);

  //部分木サイズが一定以上小さいマスに接続するグループは、D禁止日を設定したい
  //なんか長さ4以下は使えないようにしたらそれだけでよくなった

  //アルゴリズム
  //各ターンについて、長さ2~5当たりの作物の数を調べる
  //どこまでの長さについて調べるかはLの値に応じて閾値を決めたい
  //その数と同一の部分木サイズの通路マスについて、その部分木に属するグループ全てにおいてその日（とその前の日？）禁止日でないことが確認出来たら、それらのグループに禁止日を設定
  //禁止日を設定した場合は、該当するSDの組をグループで使用禁止とする
  //あるグループで禁止日を設定したら、別のグループでは設定しない（tを飛ばす？）
  double time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
  cerr << time << "\n";
  vector<vector<bool>> prohibit_group(gcnt, vector<bool>(T+1, false));
  vector<vector<bool>> prohibit_SD(T+1, vector<bool>(T+1, false));
  int t = 5;
  while(t <= T) {
    int MAX_LEN_LIM = 2;
    //if(L >= 1.6) MAX_LEN_LIM = 5;
    IFOR(max_len, 2, MAX_LEN_LIM+1) { //作物の考慮する最大の長さ
      if(t+max_len-1 > T) break;
      int max_subtree = 0;
      int max_i = -1, max_j = -1;
      int crop_cnt = 0;
      FOR(S, t, t+max_len) crop_cnt += SD[S][t+max_len-1].size();
      //同一サイズの部分木の存在チェックをする
      REP(i, H) REP(j, W) if(group[i*20+j] == -1 && crop_cnt == subtree_size_tuuro[i][j]) {
        if(!dfs_for_empty_cell_check_group(i, j, t, best_board, best_edge, group, prohibit_group)) {
          chmax(max_subtree, subtree_size_tuuro[i][j]);
          max_i = i;
          max_j = j;
        }
      }
      if(max_subtree > 0) {
        dfs_for_empty_cell_set_prohibit(max_i, max_j, t, best_board, best_edge, group, prohibit_group, max_len);
        FOR(S, t, t+max_len) prohibit_SD[S][t+max_len-1] = true;
        t += max_len-2;
        break;
      }
    }
    t++;
  }

  /*** 3. 作物計画を乱択貪欲(TODO: ビームサーチ)で割り当て ***/
  //とりあえずchokudaiサーチを書く

  int loop = 0;
  int best_score = 0;
  vector<tuple<int, int, int>> best_ans(K, {-1, -1, -1});
  while(true) {
    double time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
    cerr << loop << " " << best_score << " " << time << "\n";
    if(loop == 1) break;
    loop++;

    vector<tuple<int, int, int>> ans(K, {-1, -1, -1});
    int board[20*20] = {-1};
    memcpy(board, best_board, sizeof(board));

    int cptr = 0;
    int score = 0;
    vector<int> gcrop(gcnt);
    set<int> nextg;
    REP(i, gcnt) nextg.insert(i);
    vector<vector<P>> Dpos(T+1);
    vector<vector<int>> SDptr(T+1, vector<int>(T+1));

    vector<tuple<double, int, int>> search_params;
    REP(p, 100) REP(ds, p+1) search_params.push_back({p, ds, p - ds});
    sort(ALL(search_params));
    int SEARCH_PARAM_SIZE = 50;
    int RAND_PARAM = 1;
    int DLATE_PROHIBIT_TURN = 10;
    search_params = vector<tuple<double, int, int>>(search_params.begin(), search_params.begin() + SEARCH_PARAM_SIZE);
    //収穫後すぐに植える作物のキュー
    vector<vector<int>> crop_queue(H, vector<int>(W, -1));
    vector<vector<int>> crop_queue_timer(H, vector<int>(W, -1));

    FOR(t, 1, T+1) {

      //初手
      //初手はDの昇順、Sの降順に貪欲に通路マスに敷き詰める
      if(t == 1) {
        vector<tuple<int, int, int>> empty_crop_init;
        dfs_for_empty_cell_init(i0, 0, board, best_edge, SDptr, empty_crop_init);
        for(auto tup : empty_crop_init) {
          int x = get<0>(tup);
          int y = get<1>(tup);
          int crop_idx = get<2>(tup);
          board[x*20+y] = crop_idx;
          Dpos[D[crop_idx]].push_back({x, y});
          ans[crop_idx] = {x, y, t};
        }
      }

      //植える
      //キューがある作物を植える
      REP(i, H) REP(j, W) if(crop_queue_timer[i][j] == t) {
        assert(board[i*20+j] == -1);
        int crop_idx = crop_queue[i][j];
        board[i*20+j] = crop_idx;
        gcrop[group[i*20+j]]++;
        nextg.erase(group[i*20+j]);
        Dpos[D[crop_idx]].push_back({i, j});
        ans[crop_idx] = {i, j, t};
        crop_queue[i][j] = -1;
        crop_queue_timer[i][j] = -1;
      }
      for(int g : nextg) {
        //目標とするDを全探索し、最もペナルティ値の低い植え方を選ぶ
        //たぶんビームサーチできる
        //二段畑戦法の実装をする
        //A=B+Cとする
        //探索先は、D=AかつS=t~t+xを基準に、D=BかつS=t~t+x、D=CかつS=t~t+xを探す
        //Dの変動は基本許容できなさそう
        //TODO: Dを早める方法を考える
        //1段階目で早めるのは禁止だが、2段階目のBかCどちらかで早めることはできる
        //BとC両方で早める場合、どちらかでソートした時にどちらかがソートされていない状態は許されない？
        //Sが遅い作物を予約するのは基本良くないので何らかのペナルティを入れたい
        //Lが小さい場合は特に影響しそう
        double best_penalty = INF;
        tuple<int, int, int> best_target_D = {-1, -1, -1};
        vector<int> target_D_search_list;
        IFOR(tmp, 2, 15) target_D_search_list.push_back(tmp);
        FOR(tmp, 15, 31) target_D_search_list.push_back(tmp);
        if(pos_list[g].size() <= 2) FOR(tmp, 31, 101) target_D_search_list.push_back(tmp);
        for(int target_D : target_D_search_list) {
          int count = 0;
          double penalty = 0;
          for(auto tup : search_params) {
            if(count == pos_list[g].size()) break;
            double p = get<0>(tup);
            if(penalty + (pos_list[g].size() - count) * p / (pos_list[g].size() * target_D) > best_penalty) break;
            int sidx = t + get<1>(tup);
            int didx = t + target_D - get<2>(tup);
            if(didx < t || didx > T) continue;
            if(sidx < t || sidx > T) continue;
            if(!check_prohibit_group(g, sidx, didx, prohibit_group, prohibit_SD, t)) continue;
            if(t <= DLATE_PROHIBIT_TURN && get<2>(tup) > 0) continue;
            int c = SD[sidx][didx].size() - SDptr[sidx][didx];
            if(c == 0) continue;
            chmin(c, (int)pos_list[g].size() - count);
            count += c;
            penalty += (double)c * p / (pos_list[g].size() * target_D);
          }
          penalty += (double)(pos_list[g].size() - count) / pos_list[g].size();
          penalty *= (1000 + mt() % RAND_PARAM) / 1000.0;
          penalty += (100 - target_D) * 1e-4;
          if(penalty < best_penalty) {
            best_penalty = penalty;
            best_target_D = {target_D, -1, -1};
          }
        }

        //二段畑戦法の探索
        int SLATE_LIM = 16;
        int AMAX = 30;
        IFOR(A, 4, AMAX) {
          int didx_A = t + A - 1;
          if(didx_A > T) continue;
          FOR(B, 2, A-1) {
            int didx_B = t + B - 1;
            int didx_C = t + A - 1;

            //1段目の個数を全探索すると良さそう？
            FOR(nA, 1, pos_list[g].size()) {
              int nBC = pos_list[g].size() - nA;
              double penalty = 0;
              int countA = 0;
              REP(Slate, min(B, SLATE_LIM)) {
                int sidx_A = t + Slate;
                if(sidx_A + 1 == didx_A) break;
                if(!check_prohibit_group(g, sidx_A, didx_A, prohibit_group, prohibit_SD, t)) continue;
                int cA = SD[sidx_A][didx_A].size() - SDptr[sidx_A][didx_A];
                chmin(cA, nA - countA);
                countA += cA;
                penalty += (double)cA * Slate / (pos_list[g].size() * A);
                if(countA == nA) break;
              }
              if(penalty > best_penalty) continue;
              if(t <= 10 && countA < nA) continue;
              int countB = 0;
              REP(Slate, SLATE_LIM) {
                int sidx_B = t + Slate;
                if(sidx_B + 1 == didx_B) break;
                if(!check_prohibit_group(g, sidx_B, didx_B, prohibit_group, prohibit_SD, t)) continue;
                int cB = SD[sidx_B][didx_B].size() - SDptr[sidx_B][didx_B];
                chmin(cB, nBC - countB);
                countB += cB;
                penalty += (double)cB * Slate / (pos_list[g].size() * A);
                if(countB == nBC) break;
              }
              if(penalty > best_penalty) continue;
              if(t <= 10 && countB < nBC) continue;
              int countC = 0;
              REP(Slate, SLATE_LIM) {
                int sidx_C = t + B + Slate;
                if(sidx_C + 1 == didx_C) break;
                if(!check_prohibit_group(g, sidx_C, didx_C, prohibit_group, prohibit_SD, t)) continue;
                int cC = SD[sidx_C][didx_C].size() - SDptr[sidx_C][didx_C];
                chmin(cC, nBC - countC);
                countC += cC;
                penalty += (double)cC * Slate / (pos_list[g].size() * A);
                if(countC == nBC) break;
              }
              if(t <= 10 && countC < nBC) continue;

              penalty += (double)(nA - countA) / pos_list[g].size();
              penalty += (double)(nBC - countB) / pos_list[g].size();
              penalty += (double)(nBC - countC) / pos_list[g].size();
              penalty *= (1000 + mt() % RAND_PARAM) / 1000.0;
              penalty += (100 - A) * 1e-4;
              //Lが1に近い場合、Sが遅い作物を前もって予約するのは良くない
              penalty *= (600 + B * nBC) / 600.0;
              if(penalty < best_penalty) {
                best_penalty = penalty;
                best_target_D = {A, B, nA};
              }
            }
          }
        }

        if(get<0>(best_target_D) == -1) continue;
        //実際に植える作物のidxを取得
        if(get<1>(best_target_D) == -1) {
          vector<P> c2;
          for(auto tup : search_params) {
            double p = get<0>(tup);
            int sidx = t + get<1>(tup);
            int didx = t + get<0>(best_target_D) - get<2>(tup);
            if(didx < t || didx > T) continue;
            if(sidx < t || sidx > T) continue;
            if(t <= DLATE_PROHIBIT_TURN && get<2>(tup) > 0) continue;
            while(c2.size() < pos_list[g].size() && SDptr[sidx][didx] < SD[sidx][didx].size()) {
              int crop_idx = SD[sidx][didx][SDptr[sidx][didx]];
              c2.push_back({D[crop_idx], crop_idx});
              SDptr[sidx][didx]++;
            }
          }
          sort(ALL(c2));

          int ptr = 0;
          for(auto tup : pos_list[g]) {
            int x = get<1>(tup);
            int y = get<2>(tup);
            if(ptr < c2.size()) {
              int crop_idx = c2[ptr].second;
              board[x*20+y] = crop_idx;
              gcrop[g]++;
              Dpos[D[crop_idx]].push_back({x, y});
              ans[crop_idx] = {x, y, t};
              ptr++;
            }
          }
        } else {
          int A = get<0>(best_target_D);
          int B = get<1>(best_target_D);
          int nA = get<2>(best_target_D);
          int nBC = pos_list[g].size() - nA;
          int didx_A = t + A - 1;
          int didx_B = t + B - 1;
          int didx_C = t + A - 1;
          int cnt = 0;
          for(auto tup : pos_list[g]) {
            int x = get<1>(tup);
            int y = get<2>(tup);
            if(cnt < nBC) {
              int crop_idx = -1, sidx_B;
              REP(Slate, SLATE_LIM) {
                sidx_B = t + Slate;
                if(sidx_B + 1 == didx_B) break;
                if(SDptr[sidx_B][didx_B] == SD[sidx_B][didx_B].size()) continue;
                crop_idx = SD[sidx_B][didx_B][SDptr[sidx_B][didx_B]];
                SDptr[sidx_B][didx_B]++;
                break;
              }
              if(crop_idx != -1) {
                board[x*20+y] = crop_idx;
                gcrop[g]++;
                Dpos[D[crop_idx]].push_back({x, y});
                ans[crop_idx] = {x, y, t};
              }

              int crop_queue_idx = -1, sidx_C;
              REP(Slate, SLATE_LIM) {
                sidx_C = t + B + Slate;
                if(sidx_C + 1 == didx_C) break;
                if(SDptr[sidx_C][didx_C] == SD[sidx_C][didx_C].size()) continue;
                crop_queue_idx = SD[sidx_C][didx_C][SDptr[sidx_C][didx_C]];
                SDptr[sidx_C][didx_C]++;
                break;
              }
              if(crop_queue_idx != -1) {
                crop_queue[x][y] = crop_queue_idx;
                crop_queue_timer[x][y] = t + B;
              }

            } else {
              int crop_idx = -1, sidx_A;
              REP(Slate, min(B, SLATE_LIM)) {
                sidx_A = t + Slate;
                if(sidx_A + 1 == didx_A) break;
                if(SDptr[sidx_A][didx_A] == SD[sidx_A][didx_A].size()) continue;
                crop_idx = SD[sidx_A][didx_A][SDptr[sidx_A][didx_A]];
                SDptr[sidx_A][didx_A]++;
                break;
              }
              if(crop_idx != -1) {
                board[x*20+y] = crop_idx;
                gcrop[g]++;
                Dpos[D[crop_idx]].push_back({x, y});
                ans[crop_idx] = {x, y, t};
              }
            }
            cnt++;
          }
        }
      }

      //空きマスで植えられそうな作物があれば植える
      vector<tuple<int, int, int>> empty_crop;
      dfs_for_empty_cell(i0, 0, t, board, best_edge, group, SDptr, empty_crop, pos_list);
      for(auto tup : empty_crop) {
        int x = get<0>(tup);
        int y = get<1>(tup);
        int crop_idx = get<2>(tup);
        board[x*20+y] = crop_idx;
        Dpos[D[crop_idx]].push_back({x, y});
        ans[crop_idx] = {x, y, t};
      }

      //稼働率集計
      if(IS_SYUUKEI) {
        REP(i, H) REP(j, W) {
          if(group[i*20+j] != -1) {
            if(board[i*20+j] >= 0 && S[board[i*20+j]] <= t) work_bunsi[gsize[group[i*20+j]]][t]++;
            work_bunbo[gsize[group[i*20+j]]][t]++;
          } else if(subtree_size_tuuro[i][j] < 16){
            if(board[i*20+j] >= 0 && S[board[i*20+j]] <= t) work_bunsi_tuuro[subtree_size_tuuro[i][j]][t]++;
            work_bunbo_tuuro[subtree_size_tuuro[i][j]][t]++;
          }
        }
      }

      //収穫
      nextg.clear();
      for(P p : Dpos[t]) {
        int i = p.first;
        int j = p.second;
        score += D[board[i*20+j]] - S[board[i*20+j]] + 1;
        if(group[i*20+j] != -1) {
          gcrop[group[i*20+j]]--;
          if(gcrop[group[i*20+j]] == 0) nextg.insert(group[i*20+j]);
        }
        if(group[i*20+j] != -1) board[i*20+j] = -1;
        else board[i*20+j] = -2;
      }
    }
    score = 25 * score;

    if(score > best_score) {
      best_score = score;
      best_ans = ans;
    }
  }

  /*** 4. D=1~99に関する領域を変えることでさらに改善 ***/
  //ノード情報、各マスの作物計画（区間）情報、割り当て済み作物を管理するポインタ の3つを持って焼く
  bool node[101*20*20];
  fill_n(node, 101*20*20, false);
  vector<set<tuple<int, int, int>>> order(20*20); //作付け計画、(l, r, S) ただしl<=S r==D, S==-1の時作物無し
  vector<set<tuple<int, int, int>>> best_order(20*20);
  SDPtr sdptr;

  //初期解の割り当て
  //従来方法で作った解をここの初期解とする？
  vector<set<int>> cut(H*W);
  REP(i, H) REP(j, W) FOR(t, 1, 101) {
    node[t*20*20+i*20+j] = true;
    cut[i*20+j].insert(t);
  }
  REP(i, K) {
    int x = get<0>(best_ans[i]);
    int y = get<1>(best_ans[i]);
    int t = get<2>(best_ans[i]);
    if(x == -1) continue;
    FOR(tt, t, D[i]) {
      node[tt*20*20+x*20+y] = false;
      cut[x*20+y].erase(tt);
    }
  }
  //根から接続できていないノードがあれば消す（終盤の空きマス対策）
  FOR(t, 1, 101) {
    bool vst[20*20];
    fill_n(vst, 20*20, false);
    if(node[t*20*20+i0*20]) dfs_node_check(i0, 0, t, node, vst);
    REP(i, H) REP(j, W) if(node[t*20*20+i*20+j] && !vst[i*20+j]) {
      node[t*20*20+i*20+j] = false;
      cut[i*20+j].erase(t);
    }
  }
  REP(i, H) REP(j, W) {
    int last = 1;
    for(int t : cut[i*20+j]) {
      order[i*20+j].insert({last, t, -1});
      last = t+1;
    }
  }
  REP(i, K) {
    int x = get<0>(best_ans[i]);
    int y = get<1>(best_ans[i]);
    int t = get<2>(best_ans[i]);
    if(x == -1) continue;
    order[x*20+y].insert({t, D[i], S[i]});
    assert(count(ALL(order[x*20+y]), make_tuple(t, D[i], -1)));
    order[x*20+y].erase({t, D[i], -1});
    sdptr.incr_SDptr(S[i], D[i]);
  }
  best_score = 0;
  int best_value = 0; //空きマスにも点数を付けたい
  REP(i, H) REP(j, W) {
    for(auto tup : order[i*20+j]) {
      int l = get<0>(tup);
      int r = get<1>(tup);
      int S = get<2>(tup);
      if(S != -1) {
        best_value += SCORE_BASE * (r - S + 1);
        best_score += SCORE_BASE * (r - S + 1);
      }
    }
  }

  bool can_add[101*20*20];
  fill_n(can_add, 101*20*20, false);
  FOR(t, 1, 101) REP(i, H) REP(j, W) can_add[t*20*20+i*20+j] = check_can_add(i, j, t, node);
  bool can_del[101*20*20];
  fill_n(can_del, 101*20*20, false);
  FOR(t, 1, 101) REP(i, H) REP(j, W) can_del[t*20*20+i*20+j] = check_can_del(i, j, t, node);

  vector<int> random_992020_list(10000000);
  REP(i, 10000000) random_992020_list[i] = mt() % (99*20*20);
  int random_992020_idx = 0;
  vector<int> random_list(1000000);
  REP(i, 1000000) random_list[i] = mt() % INF;
  int random_idx = 0;

  start_temp = START_TEMP;
  end_temp = END_TEMP;
  loop = 0;
  int SLIM = 100;
  int TUIKA_VALUE = TUIKA_VALUE_BASE;
  double tmp_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
  float temp_pow;

  while(true) {
    if(loop%LOOP_CHECK_INTERVAL == 0) {
      double time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      //cerr << loop << " " << best_score << " " << best_value << " " << time << "\n";
      if(time > TIME_LIMIT) break;
      temp = start_temp + (end_temp - start_temp) * pow((time-tmp_time) / (TIME_LIMIT-tmp_time), 1.5);
      temp_pow = pow(0.1, temp);
      TUIKA_VALUE = (TUIKA_VALUE_BASE+1) * pow((TIME_LIMIT-time) / (TIME_LIMIT-tmp_time), TUIKA_VALUE_POW);
    }
    loop++;

    int mode = loop % 2;
    if(loop%CHECK_CROP_INTERVAL == 0) mode = 2;
    int t, x, y, pos;
    lint score = best_score;
    lint value = best_value;
    tuple<int, int, int> prev_tup_1 = {-1, -1, -1}, prev_tup_2 = {-1, -1, -1}, next_tup_1 = {-1, -1, -1}, next_tup_2 = {-1, -1, -1};

    if(mode == 2) {
      //全区間について、より良い作物が余ってないかどうかチェックする
      REP(pos, 400) {
        vector<tuple<int, int, int>> del;
        for(auto tup : order[pos]) {
          int l = get<0>(tup);
          int r = get<1>(tup);
          int S = get<2>(tup);
          FOR(s, l, r) {
            if(s == S) break;
            if(sdptr.get_SDrest(s, r) == 0) continue;
            sdptr.incr_SDptr(s, r);
            del.push_back(tup);
            if(S != -1) {
              sdptr.decr_SDptr(S, r);
              score -= SCORE_BASE * (r - S + 1);
              value -= SCORE_BASE * (r - S + 1);
            }
            order[pos].insert(make_tuple(l, r, s));
            score += SCORE_BASE * (r - s + 1);
            value += SCORE_BASE * (r - s + 1);
            break;
          }
        }
        for(auto tup : del) order[pos].erase(tup);
      }
    } else if(mode == 0) {
      //追加
      REP(zz, 100) {
        if(zz == 99) {
          t = -1;
          break;
        }
        int r = random_992020_list[random_992020_idx];
        random_992020_idx++;
        if(random_992020_idx == 10000000) random_992020_idx = 0;
        if(!can_add[r+400]) continue;
        t = r / (400) + 1;
        x = (r % (400)) / 20;
        y = r % 20;
        break;
      }
      if(t == -1) continue;
      pos = x*20+y;

      //点数の更新
      prev_tup_1 = *prev(order[pos].lower_bound(make_tuple(t+1, -1, -1)));
      int prev_l = get<0>(prev_tup_1);
      int prev_r = get<1>(prev_tup_1);
      int prev_S = get<2>(prev_tup_1);
      if(prev_S != -1) {
        sdptr.decr_SDptr(prev_S, prev_r);
        score -= SCORE_BASE * (prev_r - prev_S + 1);
        value -= SCORE_BASE * (prev_r - prev_S + 1);
      }

      bool ok = false;
      FOR(s, prev_l, prev_l+SLIM) {
        if(s >= t) break;
        if(sdptr.get_SDrest(s, t) == 0) continue;
        sdptr.incr_SDptr(s, t);
        next_tup_1 = make_tuple(prev_l, t, s);
        score += SCORE_BASE * (t - s + 1);
        value += SCORE_BASE * (t - s + 1);
        ok = true;
        break;
      }
      if(!ok) {
        next_tup_1 = make_tuple(prev_l, t, -1);
      }

      ok = false;
      FOR(s, t+1, prev_r+SLIM) {
        if(s >= prev_r) break;
        if(sdptr.get_SDrest(s, prev_r) == 0) continue;
        sdptr.incr_SDptr(s, prev_r);
        next_tup_2 = make_tuple(t+1, prev_r, s);
        score += SCORE_BASE * (prev_r - s + 1);
        value += SCORE_BASE * (prev_r - s + 1);
        ok = true;
        break;
      }
      if(!ok) {
        next_tup_2 = make_tuple(t+1, prev_r, -1);
      }

      //辺の数に比例した点数
      REP(d, 4) if(can_move(x, y, d)) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if(!node[t*20*20+nx*20+ny]) continue;
        value += TUIKA_VALUE;
      }

    } else {
      //削除
      REP(zz, 100) {
        if(zz == 99) {
          t = -1;
          break;
        }
        int r = random_992020_list[random_992020_idx];
        random_992020_idx++;
        if(random_992020_idx == 10000000) random_992020_idx = 0;
        if(!can_del[r+400]) continue;
        t = r / (400) + 1;
        x = (r % (400)) / 20;
        y = r % 20;
        break;
      }
      if(t == -1) continue;
      pos = x*20+y;

      //点数の更新
      auto ptr = order[pos].lower_bound(make_tuple(t+1, -1, -1));
      prev_tup_1 = *prev(ptr);
      int prev_l_1 = get<0>(prev_tup_1);
      int prev_r_1 = get<1>(prev_tup_1);
      int prev_S_1 = get<2>(prev_tup_1);
      if(prev_S_1 != -1) {
        sdptr.decr_SDptr(prev_S_1, prev_r_1);
        score -= SCORE_BASE * (prev_r_1 - prev_S_1 + 1);
        value -= SCORE_BASE * (prev_r_1 - prev_S_1 + 1);
      }

      prev_tup_2 = *ptr;
      int prev_l_2 = get<0>(prev_tup_2);
      int prev_r_2 = get<1>(prev_tup_2);
      int prev_S_2 = get<2>(prev_tup_2);
      if(prev_S_2 != -1) {
        sdptr.decr_SDptr(prev_S_2, prev_r_2);
        score -= SCORE_BASE * (prev_r_2 - prev_S_2 + 1);
        value -= SCORE_BASE * (prev_r_2 - prev_S_2 + 1);
      }

      assert(prev_r_1 + 1 == prev_l_2);

      bool ok = false;
      FOR(s, prev_l_1, prev_l_1+SLIM) {
        if(s >= prev_r_2) break;
        if(sdptr.get_SDrest(s, prev_r_2) == 0) continue;
        sdptr.incr_SDptr(s, prev_r_2);
        next_tup_1 = make_tuple(prev_l_1, prev_r_2, s);
        score += SCORE_BASE * (prev_r_2 - s + 1);
        value += SCORE_BASE * (prev_r_2 - s + 1);
        ok = true;
        break;
      }
      if(!ok) {
        next_tup_1 = make_tuple(prev_l_1, prev_r_2, -1);
      }

      //辺の数に比例した点数
      REP(d, 4) if(can_move(x, y, d)) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if(!node[t*20*20+nx*20+ny]) continue;
        value -= TUIKA_VALUE;
      }
    }

    float prob = exp(((float)value-best_value)*temp_pow);
    int r = random_list[random_idx];
    random_idx++;
    if(random_idx == 1000000) random_idx = 0;
    if(mode == 2 || value >= best_value || prob*INF > r) {
      best_value = value;
      best_score = score;
      if(mode != 2) {
        if(mode == 0) {
          node[t*20*20+x*20+y] = true;
          order[pos].erase(prev_tup_1);
          order[pos].insert(next_tup_1);
          order[pos].insert(next_tup_2);
        } else {
          node[t*20*20+x*20+y] = false;
          order[pos].erase(prev_tup_1);
          order[pos].erase(prev_tup_2);
          order[pos].insert(next_tup_1);
        }
        FOR(dx, -1, 2) FOR(dy, -1, 2) {
          int nx = x + dx;
          int ny = y + dy;
          if(nx < 0 || nx >= H || ny < 0 || ny >= W) continue;
          if(abs(dx)+abs(dy) <= 1) can_add[t*20*20+nx*20+ny] = check_can_add(nx, ny, t, node);
          can_del[t*20*20+nx*20+ny] = check_can_del(nx, ny, t, node);
        }
      }
    } else {
      if(mode == 0) {
        if(get<2>(next_tup_1) != -1) sdptr.decr_SDptr(get<2>(next_tup_1), get<1>(next_tup_1));
        if(get<2>(next_tup_2) != -1) sdptr.decr_SDptr(get<2>(next_tup_2), get<1>(next_tup_2));
        if(get<2>(prev_tup_1) != -1) sdptr.incr_SDptr(get<2>(prev_tup_1), get<1>(prev_tup_1));
      } else {
        if(get<2>(next_tup_1) != -1) sdptr.decr_SDptr(get<2>(next_tup_1), get<1>(next_tup_1));
        if(get<2>(prev_tup_1) != -1) sdptr.incr_SDptr(get<2>(prev_tup_1), get<1>(prev_tup_1));
        if(get<2>(prev_tup_2) != -1) sdptr.incr_SDptr(get<2>(prev_tup_2), get<1>(prev_tup_2));
      }
    }

  }

  //作物の最終割り当てを決める
  //Sの遅れの昇順に全ての区間を走査
  lint best_score_2 = 0;
  SDPtr sdptr3;
  vector<set<tuple<int, int, int>>> order_2(20*20);
  REP(slate, 101) {
    REP(i, H) REP(j, W) {
      vector<tuple<int, int, int>> del;
      for(auto tup : order[i*20+j]) {
        int l = get<0>(tup);
        int r = get<1>(tup);
        int S = l+slate;
        if(slate == 100) {
          order_2[i*20+j].insert(make_tuple(l, r, -1));
          continue;
        }
        if(S >= r) continue;
        if(sdptr3.get_SDrest(S, r) == 0) continue;
        sdptr3.incr_SDptr(S, r);
        order_2[i*20+j].insert(make_tuple(l, r, S));
        del.push_back(tup);
        best_score_2 += SCORE_BASE * (r - S + 1);
      }
      for(auto tup : del) order[i*20+j].erase(tup);
    }
  }

  vector<tuple<int, int, int, int>> ans;
  SDPtr sdptr2;
  REP(i, H) REP(j, W) {
    for(auto tup : order_2[i*20+j]) {
      int l = get<0>(tup);
      int r = get<1>(tup);
      int s = get<2>(tup);
      if(s == -1) continue;
      int crop_idx = sdptr2.get_cropidx(s, r);
      sdptr2.incr_SDptr(s, r);
      ans.push_back({crop_idx, i, j, l});
    }
  }
  int M = ans.size();
  cout << M << "\n";
  REP(i, M) cout << get<0>(ans[i])+1 << " " << get<1>(ans[i]) << " " << get<2>(ans[i]) << " " << get<3>(ans[i]) << "\n";

  cerr << best_score_2 << "\n";
}