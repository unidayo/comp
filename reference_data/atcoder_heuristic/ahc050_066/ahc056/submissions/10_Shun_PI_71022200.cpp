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

#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;

constexpr int TIME_LIMIT = 1975;
bool OPTUNA = false;

//OPTUNAパラメータ

// {'MERGE_COST': 0.12628990279184057, 'Q_UPDATE_COST': 0.8171435155063608, 'COLOR_COST': 0.39208799735656696, 'CHANGE_DIR_COST': 0.10890071365699855, 'AVAILABLE_MARGIN_FACTOR': 0.27091420318321946, 'PROB_MAT_FACTOR': 0.2445722089948466, 'BEAM_WIDTH_SORT_FACTOR': 1.8764896092572187, 'C_FACTOR_INIT': 0.5620395830303605, 'C_FACTOR_MULT': 0.5670320698362717, 'C_FACTOR_POW': 0.586525159029183, 'C_STEP': 4}
double MERGE_COST = 0.12628990279184057;
double Q_UPDATE_COST = 0.8171435155063608;
double COLOR_COST = 0.39208799735656696;
double CHANGE_DIR_COST = 0.10890071365699855;
double AVAILABLE_MARGIN_FACTOR = 0.27091420318321946;
double PROB_MAT_FACTOR = 0.2445722089948466;
double BEAM_WIDTH_SORT_FACTOR = 1.8764896092572187;
double C_FACTOR_INIT = 0.5620395830303605;
double C_FACTOR_MULT = 0.5670320698362717;
double C_FACTOR_POW = 0.586525159029183;
int C_STEP = 4;


vector<int> dx = {-1, 0, 1, 0, 0};
vector<int> dy = {0, -1, 0, 1, 0};
vector<char> dc = {'U', 'L', 'D', 'R', 'S'};
mt19937 mt(0);

int N, K, T;
vector<string> V, H;
vector<int> X, Y;
vector<int> available_turn;
int available_turn_sum = 0;

struct DsuWithBit {
  int C;
  dsu d;
  vector<lint> bit;
  vector<int> color, owner;
  DsuWithBit() {}
  DsuWithBit(int C){
    this->C = C;
    d = dsu(C*64);
  }
  void add_q(int q) {
    REP(k, C) {
      bit.emplace_back(1LL << q);
      color.emplace_back(k);
      owner.emplace_back(k);
    }
    merge(0, q*C);
  }
  void merge(int x, int y) {
    bit[leader(x)] |= bit[leader(y)];
    bit[leader(y)] |= bit[leader(x)];
    d.merge(x, y);
  }
  int leader(int x) {
    return d.leader(x);
  }
  bool can_merge(int x, int y) {
    if((bit[leader(x)] & bit[leader(y)]) != 0) return false;
    int qx = x / C;
    int ox = x % C;
    int qy = y / C;
    int oy = y % C;
    int cx = color[qx*C+ox];
    int cy = color[qy*C+oy];
    if(cx == cy) return true;
    if(cx == 0 || cy == 0) return false;
    //ケンペ鎖によりマージ可能かどうかを判定したい
    deque<pair<int, bool>> que;
    lint visited = bit[leader(x)];
    lint tmp = visited;
    while(tmp) {
      int b = __builtin_ctzll(tmp); // 最下位1ビットの位置
      tmp &= tmp - 1;
      que.emplace_back(b, false);
    }
    while(!que.empty()) {
      auto [q, mode] = que.front(); que.pop_front();
      int c = (mode ? cx : cy);
      lint new_bits = bit[leader(q*C+owner[q*C+c])] & (~visited);
      while(new_bits) {
        int b = __builtin_ctzll(new_bits); // 最下位1ビットの位置
        new_bits &= new_bits - 1;
        que.emplace_back(b, !mode);
        visited |= 1LL << b;
      }
    }
    if((visited & bit[leader(y)]) != 0) {
      return false;
    }
    //mergeできるように色を反転する
    while(visited) {
      int b = __builtin_ctzll(visited); // 最下位1ビットの位置
      visited &= visited - 1;
      int ox = owner[b*C+cx];
      int oy = owner[b*C+cy];
      swap(color[b*C+ox], color[b*C+oy]);
      swap(owner[b*C+cx], owner[b*C+cy]);
    }
    return true;
  }
  bool same(int x, int y) {
    return d.same(x, y);
  }
};

struct Rule {
  int c, q, A, S, D;
  // 比較演算子: a, b が同じなら「同一」とみなす
  bool operator<(const Rule& other) const {
      if (q != other.q) return q < other.q;
      return c < other.c;
  }
  void output_rule() {
    cout << c << " " << q << " " << A << " " << S << " " << dc[D] << "\n";
  }
};

bool can_move(int x, int y, int d) {
  if(d == 0) {
    if(x == 0) return false;
    return H[x-1][y] != '1';
  } else if(d == 1) {
    if(y == 0) return false;
    return V[x][y-1] != '1';
  } else if(d == 2) {
    if(x == N-1) return false;
    return H[x][y] != '1';
  } else {
    if(y == N-1) return false;
    return V[x][y] != '1';
  }
}

vector<vector<int>> dist_mat;
vector<vector<double>> prob_mat;
void build_dist_mat() {
  //全マス間の距離を求める
  dist_mat = vector<vector<int>>(N*N, vector<int>(N*N, INF));
  REP(sx, N) REP(sy, N) {
    deque<P> que;
    que.emplace_back(sx, sy);
    dist_mat[sx*N+sy][sx*N+sy] = 0;
    while(!que.empty()) {
      auto [cx, cy] = que.front(); que.pop_front();
      REP(d, 4) {
        if(!can_move(cx, cy, d)) continue;
        int nx = cx + dx[d];
        int ny = cy + dy[d];
        if(dist_mat[sx*N+sy][nx*N+ny] == INF) {
          dist_mat[sx*N+sy][nx*N+ny] = dist_mat[sx*N+sy][cx*N+cy] + 1;
          que.emplace_back(nx, ny);
        }
      }
    }
  }

  //通貨確率を求める
  prob_mat = vector<vector<double>>(N, vector<double>(N, 0.0));
  REP(k, K-1) {
    int sx = X[k];
    int sy = Y[k];
    int gx = X[k+1];
    int gy = Y[k+1];
    vector<vector<double>> prob(N, vector<double>(N, 0.0));
    prob[sx][sy] = 1.0;
    deque<P> que;
    que.emplace_back(sx, sy);
    while(!que.empty()) {
      auto [cx, cy] = que.front(); que.pop_front();
      int cd = dist_mat[cx*N+cy][gx*N+gy];
      int move_count = 0;
      REP(d, 4) {
        if(!can_move(cx, cy, d)) continue;
        int nx = cx + dx[d];
        int ny = cy + dy[d];
        if(dist_mat[nx*N+ny][gx*N+gy] < cd) {
          move_count++;
        }
      }
      REP(d, 4) {
        if(!can_move(cx, cy, d)) continue;
        int nx = cx + dx[d];
        int ny = cy + dy[d];
        if(dist_mat[nx*N+ny][gx*N+gy] < cd) {
          double p = prob[cx][cy] / move_count;
          if(prob[nx][ny] == 0.0) {
            que.emplace_back(nx, ny);
          }
          prob[nx][ny] += p;
        }
      }
    }
    REP(x, N) REP(y, N) {
      prob_mat[x][y] += prob[x][y];
    }
  }

  double prob_sum = 0.0;
  REP(x, N) REP(y, N) {
    prob_sum += prob_mat[x][y];
  }
  REP(x, N) REP(y, N) {
    prob_mat[x][y] /= (prob_sum / (N*N));
    prob_mat[x][y] = pow(prob_mat[x][y], 0.5);
  }
}

struct StateLog;
vector<StateLog> states_logs;
struct StateLog {
  int id, last;
  int C;
  int dir, last_dir;
  int bq, nq;
  int bc, nc;
};
struct State {
  int id, last = -1;
  int C; //色の上限

  //行動ログ
  int dir = -1;
  int last_dir = -1;
  int bq = -1;
  int nq = -1;
  int bc = -1;
  int nc = -1;

  int x, y; //座標
  int turn; //移動したターン数
  int available_margin; //目的地までの最短距離に対する残りの移動余裕量
  int k; //次の目的地のインデックス

  vector<int> q; //内部状態（各移動方向毎）
  int next_q; //次に使う内部状態
  vector<int> next_color;
  double cost_base; //コスト計算の素点
  //undo用
  int available_margin_tmp;
  double cost_base_tmp;

  vector<int> color;
  vector<vector<P>> color_map; //color_mapは前の移動方向・次の移動方向に対して（前の色→後の色）を持つ

  //仮の色管理
  DsuWithBit color_dsu;

  double cost; //評価値
  bool operator < (const State &other) const { return cost < other.cost; }

  State get_copy() {
    State new_state;
    new_state.last = id;
    new_state.bq = nq;
    new_state.last_dir = dir;
    new_state.C = C;
    new_state.x = x;
    new_state.y = y;
    new_state.turn = turn;

    new_state.available_margin = available_margin;
    new_state.cost_base = cost_base;

    new_state.k = k;
    new_state.q = q;
    new_state.next_q = next_q;
    new_state.next_color = next_color;
    new_state.color = color;
    new_state.color_map = color_map;
    new_state.color_dsu = color_dsu;
    return new_state;
  }

  void init(int _C, int minimum_required_turn) {
    C = _C;
    x = X[K-1];
    y = Y[K-1];
    turn = 0;
    available_margin = available_turn[K-2] - dist_mat[x*N+y][X[K-2]*N+Y[K-2]];
    k = K - 2;
    q = vector<int>(4, 0);
    REP(i, 4) q[i] = i;
    next_q = 4;
    next_color = vector<int>(4, 1);
    cost_base = 0;
    color = vector<int>(N*N);
    color_map = vector<vector<P>>(16);
    REP(i, 4) reset_color_map(i, q[i]);
    color_dsu = DsuWithBit(C);
    REP(i, 4) color_dsu.add_q(i);
  }
  void debug() {
    cerr << "State id: " << id << ", last: " << last << ", pos: (" << x << ", " << y << "), dir: " << dir << ", last_dir: " << last_dir << ", bq: " << bq << ", nq: " << nq << ", bc: " << bc << ", nc: " << nc << ", turn: " << turn << ", available_margin: " << available_margin << ", k: " << k << ", next_q: " << next_q << ", cost_base: " << cost_base << ", cost: " << cost << "\n";
    cerr << "color:\n";
    REP(i, N) {
      REP(j, N) {
        cerr << get_color(i, j) << " ";
      }
      cerr << "\n";
    }
  }
  void push() {
    id = states_logs.size();
    StateLog state_log;
    state_log.id = id;
    state_log.last = last;
    state_log.C = C;
    state_log.dir = dir;
    state_log.last_dir = last_dir;
    state_log.bq = bq;
    state_log.nq = nq;
    state_log.bc = bc;
    state_log.nc = nc;
    states_logs.emplace_back(state_log);
  }
  int calc_score() {
    return C + next_q;
  }
  void calc_cost() {
    cost = cost_base;
    cost -= AVAILABLE_MARGIN_FACTOR * available_margin; //最短距離に対する残りの移動余裕量
    cost += 0.000001 * (mt() % 100);
  }
  bool is_end() {
    return k == -1;
  }

  int get_color(int x, int y) {
    return color[x*N+y] ;
  }
  void set_color(int x, int y, int c) {
    color[x*N+y] = c;
  }
  P get_color_map(int d, int nd, int c) {
    //与えられたd, nd, cに対して適用可能なルールがあるかどうかを調べる
    P ret = {-1, -1};
    for(auto p : color_map[d*4+nd]) {
      if(p.first == c) {
        ret = {p.second, -1};
        break;
      }
      if(color_dsu.same(p.first, c)) {
        ret = {p.second, -1};
        break;
      }
      //p.firstとcがマージできるか？
      if(color_dsu.can_merge(p.first, c)) {
        ret = {p.second, p.first};
      }
    }
    return ret;
  }
  void set_color_map(int d, int nd, int c, int nc) {
    color_map[d*4+nd].emplace_back(c, nc);
  }
  void reset_color_map(int d, int q) {
    REP(i, 4) {
      color_map[d*4+i].clear();
      color_map[i*4+d].clear();
    }
    set_color_map(d, d, 0, 0);
    next_color[d] = q*C + 1;
  }

  //先に行動可能判定および評価値計算だけやる
  bool check_move(int _d, bool force=false) {
    available_margin_tmp = available_margin;
    cost_base_tmp = cost_base;

    int d = (_d + 2) % 4;
    if(turn > 3 && (d+2)%4 == dir) return false; //目的地到達直後以外Uターン禁止
    int bctmp = get_color(x, y);
    int cur_dist_to_goal = dist_mat[x*N+y][X[k]*N+Y[k]];
    int nx = x - dx[d];
    int ny = y - dy[d];
    if(!can_move(x, y, _d) && !force) return false;
    int new_dist_to_goal = dist_mat[nx*N+ny][X[k]*N+Y[k]];
    if(new_dist_to_goal > cur_dist_to_goal) {
      available_margin -= 2;
    } else if(new_dist_to_goal == cur_dist_to_goal) {
      available_margin -= 1;
    }
    if(available_margin < 0 && !force) return false;

    if(dir != -1) {
      P cmp = get_color_map(d, dir, bctmp);
      int cm = cmp.first;
      int bcm = cmp.second;
      if(cm != -1) {
        if(bcm != -1) cost_base += MERGE_COST;
      } else if(next_color[d]%C == 0) {
        cost_base += Q_UPDATE_COST;
      } else {
        cost_base += COLOR_COST;
      }
      if(cm > 0 && bctmp == 0) cost_base += PROB_MAT_FACTOR * prob_mat[x][y]; //TODO: 色0を1以上にするマスについて、なるべく使用頻度の低いマスの方が良い
      if(dir != d) cost_base += CHANGE_DIR_COST;
    }

    calc_cost();
    return true;
  }

  void do_move(int _d) {
    int d = (_d + 2) % 4;
    dir = d;
    bc = get_color(x, y);
    int nx = x - dx[d];
    int ny = y - dy[d];
    turn++;

    if(last_dir != -1) {
      P cm_p = get_color_map(d, last_dir, bc);
      int cm = cm_p.first;
      int bcm = cm_p.second;
      if(cm != -1) {
        set_color(x, y, cm);
        if(bcm != -1) {
          color_dsu.merge(bc, bcm);
        }
      } else if(next_color[d]%C == 0) {
        q[d] = next_q;
        next_q++;
        color_dsu.add_q(q[d]);
        reset_color_map(d, q[d]);
        set_color(x, y, next_color[d]);
        next_color[d]++;
      } else {
        set_color(x, y, next_color[d]);
        set_color_map(d, last_dir, bc, next_color[d]);
        next_color[d]++;
      }
    }

    nq = q[d];
    nc = get_color(x, y);
    x = nx;
    y = ny;

    if(x == X[k] && y == Y[k]) {
      k--;
      if(k >= 0) available_margin = available_turn[k] - dist_mat[x*N+y][X[k]*N+Y[k]];
      turn = 0;
    }
    calc_cost();
  }

  void undo_move() {
    available_margin = available_margin_tmp;
    cost_base = cost_base_tmp;
  }

};


tuple<int, int, set<Rule>, vector<vector<int>>> get_output(State state) {
  int C = state.C;
  int Q = state.next_q;
  int final_q = state.nq; //final_qが0になるようにqはシフトする
  cerr << "Final C: " << C << ", Final Q: " << Q << "\n";

  //仮の色→真の色のマッピング
  vector<int> color_map = state.color_dsu.color;

  set<Rule> ans;
  vector<vector<int>> color(N, vector<int>(N));
  REP(i, N) REP(j, N) {
    color[i][j] = color_map[state.get_color(i, j)];
  }
  int turn = 0;
  StateLog s = states_logs[state.id];
  while(true) {
    if(s.last_dir == -1) break;
    //cerr << turn << " " << s.nc << " " << (s.nq - final_q + Q) % Q << " " << s.bc << " " << (s.bq - final_q + Q) % Q << " " << s.last_dir << "\n";
    Rule rule = {color_map[s.nc], (s.nq - final_q + Q) % Q, color_map[s.bc], (s.bq - final_q + Q) % Q, s.last_dir};
    ans.insert(rule);
    assert(s.last != -1);
    s = states_logs[s.last];
    turn++;
  }

  return {C, Q, ans, color};
}

chrono::system_clock::time_point start_time;
State do_beam(int C, int minimum_required_turn, bool fix_width_one=false) {
  vector<State> beam_next_k;
  State init;
  init.init(C, minimum_required_turn);
  init.push();
  beam_next_k.push_back(init);
  State best_state;
  int BEAM_WIDTH = fix_width_one ? 1 : 100;
  int use_states_sum = 0;
  int turn_time_sum = 0;
  vector<int> d_list = {0, 1, 2, 3};
  int turn_overall = 0;

  IREP(k, K-1) {
    vector<State> beam = beam_next_k;
    sort(ALL(beam));
    if(beam.size() > BEAM_WIDTH) {
      beam.erase(beam.begin() + BEAM_WIDTH, beam.end());
    }
    vector<State> next_beam;;
    beam_next_k = vector<State>();
    REP(t, available_turn[k]) {
      if(beam.empty()) break;
      //cerr << k << " " << t << ", beam size: " << beam.size() << ", BEAM_WIDTH: " << BEAM_WIDTH << " " << ", best_cost: " << beam[0].cost << "\n";
      auto turn_start_time = chrono::system_clock::now();
      bool sorted = false;
      next_beam = vector<State>();
      for(State &s : beam) {
        if(best_state.last != -1 && s.calc_score() >= best_state.calc_score()) {
          continue;
        }
        shuffle(ALL(d_list), mt);
        for(int d : d_list) {
          if(!s.check_move(d) || (sorted && next_beam[BEAM_WIDTH-1].cost <= s.cost)) {
            s.undo_move();
            continue;
          }
          State next_state = s.get_copy();
          next_state.do_move(d);
          next_state.push();
          if(next_state.k == k) {
            next_beam.push_back(next_state);
            //next_state.debug();
            if(next_beam.size() >= BEAM_WIDTH*BEAM_WIDTH_SORT_FACTOR) {
              sort(ALL(next_beam));
              while(next_beam.size() > BEAM_WIDTH) {
                next_beam.pop_back();
              }
              sorted = true;
            }
          } else {
            beam_next_k.push_back(next_state);
            if(next_state.is_end()) {
              State final_state = next_state.get_copy();
              final_state.do_move(final_state.last_dir);
              final_state.push();
              if(best_state.last == -1 || final_state.calc_score() < best_state.calc_score()) {
                best_state = final_state;
              }
            }
          }
          s.undo_move();
        }
      }
      sort(ALL(next_beam));
      if(next_beam.size() > BEAM_WIDTH) {
        next_beam.erase(next_beam.begin() + BEAM_WIDTH, next_beam.end());
      }

      if(!fix_width_one) {
        //ビーム幅の調整
        //次のターンのビーム幅を求める
        auto turn_end_time = chrono::system_clock::now();
        double turn_time = chrono::duration_cast<chrono::microseconds>(turn_end_time-turn_start_time).count();
        double rest_time = TIME_LIMIT*1000 - chrono::duration_cast<chrono::microseconds>(turn_end_time-start_time).count();
        use_states_sum += beam.size();
        turn_time_sum += turn_time;
        double time_per_state = (double)turn_time_sum / use_states_sum;
        double next_turn_time = rest_time / max(1, available_turn_sum - turn_overall - 1);
        BEAM_WIDTH = next_turn_time / time_per_state;
        chmax(BEAM_WIDTH, 1);
        turn_overall++;
      }

      beam = next_beam;
    }
  }
  return best_state;
}


int main(int argc, char* argv[])
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> K >> T;
  V.resize(N);
  REP(i, N) cin >> V[i];
  H.resize(N-1);
  REP(i, N-1) cin >> H[i];
  X.resize(K);
  Y.resize(K);
  REP(i, K) cin >> X[i] >> Y[i];
  start_time = chrono::system_clock::now();

  if(OPTUNA) {
    if(argc == 0) {
      cerr << "Usage: ./a.out <input_file> <output_file>" << endl;
      return 1;
    } else {
      MERGE_COST = atof(argv[1]);
      Q_UPDATE_COST = atof(argv[2]);
      COLOR_COST = atof(argv[3]);
      CHANGE_DIR_COST = atof(argv[4]);
      AVAILABLE_MARGIN_FACTOR = atof(argv[5]);
      PROB_MAT_FACTOR = atof(argv[6]);
      BEAM_WIDTH_SORT_FACTOR = atof(argv[7]);
      C_FACTOR_INIT = atof(argv[8]);
      C_FACTOR_MULT = atof(argv[9]);
      C_FACTOR_POW = atof(argv[10]);
      C_STEP = atoi(argv[11]);
    }
  }

  states_logs.reserve(1000000);

  build_dist_mat();
  int minimum_required_turn = 0;
  REP(k, K-1) minimum_required_turn += dist_mat[X[k]*N+Y[k]][X[k+1]*N+Y[k+1]];

  //各目的地間で消費可能なターン数を求める
  available_turn = vector<int>(K-1);
  REP(k, K-1) {
    available_turn[k] = dist_mat[X[k]*N+Y[k]][X[k+1]*N+Y[k+1]] * T / minimum_required_turn;
    available_turn_sum += available_turn[k];
  }

  int C = sqrt(minimum_required_turn) * C_FACTOR_INIT;
  chmin(C, 63);
  cerr << "minimum_required_turn: " << minimum_required_turn << ", C: " << C << "\n";

  REP(k, C_STEP) {
    State beam_state_try = do_beam(C, minimum_required_turn, true);
    if(beam_state_try.last != -1) {
      int used_area = beam_state_try.C * beam_state_try.next_q;
      int next_C = pow(used_area, C_FACTOR_POW) * C_FACTOR_MULT;
      chmin(next_C, 63);
      C = next_C;
      cerr << "Refine C: " << C << "\n";
    }
  }
  State best_state = do_beam(C, minimum_required_turn, false);
  if(best_state.last == -1) {
    cerr << "No solution found\n";
    quick_exit(0);
  }

  auto [best_C, best_Q, best_ans, best_color] = get_output(best_state);
  cout << best_C << " " << best_Q << " " << best_ans.size() << "\n";
  REP(i, N) {
    REP(j, N) {
      cout << best_color[i][j] << " ";
    }
    cout << "\n";
  }
  for(Rule rule : best_ans) rule.output_rule();

  cerr << best_C + best_Q << "\n";

  quick_exit(0);
}