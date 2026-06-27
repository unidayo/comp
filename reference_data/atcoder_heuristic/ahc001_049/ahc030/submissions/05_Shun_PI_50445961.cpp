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

constexpr bool OPTUNA = false;

//{'MAX_PROB': 0.8179848589808724, 'YAKINAMASHI_LOOP_CNT': 175, 'STATE_REMAIN_CNT': 7824, 'STATE_REMAIN_CNT_M4': 614, 'STATE_REMAIN_CNT_M5': 225, 'STATE_REMAIN_CNT_M6': 205, 'STATE_REMAIN_CNT_M8': 25, 'STATE_REMAIN_CNT_M10': 66, 'LOOP_CNT_FACTOR': 58034, 'LOOP_CNT_FACTOR_M4': 50729, 'LOOP_CNT_FACTOR_M5': 12877, 'LOOP_CNT_FACTOR_M6': 49782, 'LOOP_CNT_FACTOR_M8': 23518, 'LOOP_CNT_FACTOR_M10': 30936, 'WEIGHT_POWER': 1.6340798421709375, 'TEMP': 0.45496141400002077, 'STATE_SET_SIZE': 2125, 'USE_STATE_FOR_QUERY_MAX': 677, 'QUERY_CNT_FACTOR': 0.09199199020208003, 'SWAP_THRESHOLD': 53, 'SWAP_SLIDE_THRESHOLD': 57, 'VAR_FACTOR': 0.652411310836095, 'SEEN_STATE_STEP_PROB': 10}
double MAX_PROB = 0.8179848589808724;
int YAKINAMASHI_LOOP_CNT = 175;
int STATE_REMAIN_CNT = 7824;
int STATE_REMAIN_CNT_M4 = 614;
int STATE_REMAIN_CNT_M5 = 225;
int STATE_REMAIN_CNT_M6 = 205;
int STATE_REMAIN_CNT_M8 = 25;
int STATE_REMAIN_CNT_M10 = 66;
int LOOP_CNT_FACTOR = 58034;
int LOOP_CNT_FACTOR_M4 = 50729;
int LOOP_CNT_FACTOR_M5 = 12877;
int LOOP_CNT_FACTOR_M6 = 49782;
int LOOP_CNT_FACTOR_M8 = 23518;
int LOOP_CNT_FACTOR_M10 = 30936;
double WEIGHT_POWER = 1.6340798421709375;
double TEMP = 0.45496141400002077;
int STATE_SET_SIZE = 2125;
int USE_STATE_FOR_QUERY_MAX = 677;
double QUERY_CNT_FACTOR = 0.09199199020208003;
int SWAP_THRESHOLD = 53;
int SWAP_SLIDE_THRESHOLD = 57;
double VAR_FACTOR = 0.652411310836095;
int SEEN_STATE_STEP_PROB = 10;




vector<int> dx = {1, 0, -1, 0};
vector<int> dy = {0, 1, 0, -1};

mt19937 mt(0);

int N, M;
double eps;
vector<vector<int>> poly;
vector<vector<lint>> poly_bit;
vector<int> poly_size_x, poly_size_y;
chrono::system_clock::time_point start_time;

double x_to_v(double x, int k) {
  return (x - k * eps) / (1 - 2 * eps);
}

double get_norm(double x, double mu, double sigma) {
  double ret = exp(-(x-mu)*(x-mu)/(2*sigma*sigma)) / (sigma * sqrt(2*3.1415926535));
  if(ret < 1e-10) ret = 1e-10;
  return ret;
}
double get_norm_segment(double a, double b, double mu, double sigma) {
  if(a > b) swap(a, b);
  double ret = erf((b-mu)/(sigma*sqrt(2))) - erf((a-mu)/(sigma*sqrt(2)));
  if(ret < 1e-10) ret = 1e-10;
  return ret;
}

struct State {
  double prob;
  vector<int> sx, sy;
  vector<lint> board;
  lint hash;
  State(vector<int> sx, vector<int> sy, vector<lint> board) : sx(sx), sy(sy), board(board) {
    hash = 0;
    REP(i, N) {
      hash = (hash * B1 + board[i]) % M1;
    }
  }
  bool operator<(const State &s) const {
    return prob > s.prob;
  }
};

struct Query {
  int ret, size;
  vector<lint> mask;
};

vector<State> state_list;
unordered_set<lint> hash_set;

int main(int argc, char *argv[])
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> M >> eps;
  REP(i, M) {
    int d;
    cin >> d;
    vector<int> poly_tmp;
    int max_x = 0, max_y = 0;
    REP(j, d) {
      int x, y;
      cin >> x >> y;
      poly_tmp.push_back(x*N+y);
      chmax(max_x, x);
      chmax(max_y, y);
    }
    poly.push_back(poly_tmp);
    poly_size_x.push_back(max_x + 1);
    poly_size_y.push_back(max_y + 1);
  }

  if(OPTUNA) {
    if(argc == 0) {
      cerr << "Usage: ./a.out <input_file> <output_file>" << endl;
      return 1;
    } else {
      MAX_PROB = atof(argv[1]);
      YAKINAMASHI_LOOP_CNT = atoi(argv[2]);
      STATE_REMAIN_CNT = atoi(argv[3]);
      STATE_REMAIN_CNT_M4 = atoi(argv[4]);
      STATE_REMAIN_CNT_M5 = atoi(argv[5]);
      STATE_REMAIN_CNT_M6 = atoi(argv[6]);
      STATE_REMAIN_CNT_M8 = atoi(argv[7]);
      STATE_REMAIN_CNT_M10 = atoi(argv[8]);
      LOOP_CNT_FACTOR = atoi(argv[9]);
      LOOP_CNT_FACTOR_M4 = atoi(argv[10]);
      LOOP_CNT_FACTOR_M5 = atoi(argv[11]);
      LOOP_CNT_FACTOR_M6 = atoi(argv[12]);
      LOOP_CNT_FACTOR_M8 = atoi(argv[13]);
      LOOP_CNT_FACTOR_M10 = atoi(argv[14]);
      WEIGHT_POWER = atof(argv[15]);
      TEMP = atof(argv[16]);
      STATE_SET_SIZE = atoi(argv[17]);
      USE_STATE_FOR_QUERY_MAX = atoi(argv[18]);
      QUERY_CNT_FACTOR = atof(argv[19]);
      SWAP_THRESHOLD = atoi(argv[20]);
      SWAP_SLIDE_THRESHOLD = atoi(argv[21]);
      VAR_FACTOR = atof(argv[22]);
      SEEN_STATE_STEP_PROB = atoi(argv[23]);
    }
  }

  start_time = chrono::system_clock::now();

  int Vsum = 0;
  REP(i, M) Vsum += poly[i].size();

  //行毎のビットでpolyを持つ
  poly_bit = vector<vector<lint>>(M, vector<lint>(N, 0));
  REP(i, M) for(int p : poly[i]) poly_bit[i][p/N] += 1LL << (3*(p%N));

  //図形をスライドさせたときのオン・オフになるマスを列挙する
  vector<vector<vector<P>>> slide_off(M, vector<vector<P>>(4));
  vector<vector<vector<P>>> slide_on(M, vector<vector<P>>(4));
  REP(i, M) REP(d, 4) {
    set<P> pos_bef;
    for(int p : poly[i]) pos_bef.insert({p/N, p%N});
    set<P> pos_aft;
    for(int p : poly[i]) {
      int x = p/N, y = p%N;
      int nx = x + dx[d], ny = y + dy[d];
      pos_aft.insert({nx, ny});
    }
    for(P p : pos_bef) if(pos_aft.count(p) == 0) slide_off[i][d].push_back(p);
    for(P p : pos_aft) if(pos_bef.count(p) == 0) slide_on[i][d].push_back(p);
  }

  //図形のスワップ時のオン・オフとなるマスを列挙する
  vector<vector<vector<P>>> swap_off(M, vector<vector<P>>(M));
  vector<vector<vector<P>>> swap_on(M, vector<vector<P>>(M));
  REP(i, M) REP(j, M) {
    set<P> pos_bef;
    for(int p : poly[i]) pos_bef.insert({p/N, p%N});
    set<P> pos_aft;
    for(int p : poly[j]) {
      int x = p/N, y = p%N;
      x += (poly_size_x[i] - poly_size_x[j]) / 2;
      y += (poly_size_y[i] - poly_size_y[j]) / 2;
      pos_aft.insert({x, y});
    }
    for(P p : pos_bef) if(pos_aft.count(p) == 0) swap_off[i][j].push_back(p);
    for(P p : pos_aft) if(pos_bef.count(p) == 0) swap_on[i][j].push_back(p);
  }

  //図形のスワップ・スライド時のオン・オフになるマスを列挙する
  vector<vector<vector<vector<P>>>> swap_slide_off(M, vector<vector<vector<P>>>(M, vector<vector<P>>(4)));
  vector<vector<vector<vector<P>>>> swap_slide_on(M, vector<vector<vector<P>>>(M, vector<vector<P>>(4)));
  REP(i, M) REP(j, M) REP(d, 4) {
    set<P> pos_bef;
    for(int p : poly[i]) pos_bef.insert({p/N, p%N});
    set<P> pos_aft;
    for(int p : poly[j]) {
      int x = p/N, y = p%N;
      x += (poly_size_x[i] - poly_size_x[j]) / 2;
      y += (poly_size_y[i] - poly_size_y[j]) / 2;
      int nx = x + dx[d], ny = y + dy[d];
      pos_aft.insert({nx, ny});
    }
    for(P p : pos_bef) if(pos_aft.count(p) == 0) swap_slide_off[i][j][d].push_back(p);
    for(P p : pos_aft) if(pos_bef.count(p) == 0) swap_slide_on[i][j][d].push_back(p);
  }


  //p(x|v)の前計算
  //各k, v毎にxの得られる確率を求める
  //正規分布の値が分かれば求まる
  vector<vector<vector<double>>> p_x_v(N*N, vector<vector<double>>(Vsum+1, vector<double>(Vsum+1, 0))); //k, v, x
  REP(v, Vsum+1) REP(x, Vsum+1) p_x_v[1][v][x] = (v == x ? 1 : 0);
  FOR(k, 2, N*N) REP(v, Vsum+1) REP(x, Vsum+1) {
    double mu = (k - v) * eps + v * (1 - eps);
    double sigma = sqrt(k * eps * (1 - eps));
    double l = x - 0.5;
    if(x == 0) l = -1e18;
    double r = x + 0.5;
    if(x == Vsum) r = 1e18;
    p_x_v[k][v][x] = get_norm_segment(l, r, mu, sigma);
  }

  if(M == 2) {
    /*****************************************/
    //M=2の場合は全パターン列挙＋ベイズ推定
    /*****************************************/

    REP(sx1, N-poly_size_x[0]+1) REP(sy1, N-poly_size_y[0]+1) {
      REP(sx2, N-poly_size_x[1]+1) REP(sy2, N-poly_size_y[1]+1) {
        vector<int> sx = {sx1, sx2}, sy = {sy1, sy2};
        vector<lint> board(N);
        for(int p : poly[0]) board[sx1+p/N] += 1LL << (3*(sy1+p%N));
        for(int p : poly[1]) board[sx2+p/N] += 1LL << (3*(sy2+p%N));
        State state(sx, sy, board);
        if(hash_set.count(state.hash) == 0) {
          state_list.push_back(state);
          hash_set.insert(state.hash);
        }
      }
    }

    REP(i, state_list.size()) state_list[i].prob = 1.0 / state_list.size();

    cerr << "state_list.size(): " << state_list.size() << endl;

    int turn = 0;

    while(turn < 2*N*N) {

      turn++;

      sort(ALL(state_list));

      auto time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > 2930) {
        break;
      }

      //ある状態の確率がx%を超えたらそれを出力する
      double max_prob = state_list[0].prob;
      //cerr << turn << " " << max_prob << endl;
      cout << "# max_prob:" << max_prob << endl;

      //debug
      /*
      REP(k, 5) {
        cerr << "##" << " " << k << " " << state_list[k].prob << " ";
        REP(i, 2) cerr << state_list[k].sx[i] << " " << state_list[k].sy[i] << " ";
        cerr << endl;
      }
      */

      int USE_STATE_FOR_QUERY = 0;
      double prob_sum_for_query = 0;
      REP(i, state_list.size()) {
        prob_sum_for_query += state_list[i].prob;
        if((i >= 9 && prob_sum_for_query > 0.9) || i == USE_STATE_FOR_QUERY_MAX) {
          USE_STATE_FOR_QUERY = i+1;
          break;
        }
      }

      //cerr << "prob_sum_for_query: " << prob_sum_for_query << endl;
      bool give_up = false;

      //重み付き分散値ができるだけ大きくなるような点をクエリに貪欲に追加していく
      //TODO: M=2では余裕があるので焼きなましでクエリを作りたい
      vector<vector<bool>> query(N, vector<bool>(N, false));
      vector<int> state_cnt(USE_STATE_FOR_QUERY);
      int query_cnt = 0;
      double max_value = -1e9;
      while(query_cnt < N*N - 1) {
        auto time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
        if(time > 2930) {
          give_up = true;
          break;
        }
        int target_x = -1, target_y = -1;
        REP(i, N) REP(j, N) if(!query[i][j]) {
          vector<int> state_cnt_tmp(state_cnt);
          REP(k, USE_STATE_FOR_QUERY) state_cnt_tmp[k] += state_list[k].board[i] >> (3*j) & 7;
          double mean = 0;
          REP(k, USE_STATE_FOR_QUERY) mean += state_cnt_tmp[k] * state_list[k].prob;
          mean /= prob_sum_for_query;
          double var = 0;
          REP(k, USE_STATE_FOR_QUERY) var += (state_cnt_tmp[k] - mean) * (state_cnt_tmp[k] - mean) * state_list[k].prob;
          double value = (VAR_FACTOR * var - ((query_cnt+1) * eps * (1 - eps))) * sqrt(query_cnt+1);
          if(value > max_value) {
            max_value = value;
            target_x = i;
            target_y = j;
          }
        }
        //cerr << query_cnt << " " << max_value << endl;
        if(target_x == -1) break;
        query[target_x][target_y] = true;
        query_cnt++;
        REP(k, USE_STATE_FOR_QUERY) state_cnt[k] += state_list[k].board[target_x] >> (3*target_y) & 7;
      }

      if(max_prob > MAX_PROB || query_cnt == 0 || give_up) {
        vector<lint> ans_board = state_list[0].board;
        int cnt = 0;
        REP(i, N) REP(j, N) if((ans_board[i]>>(3*j)&7) > 0) cnt++;
        cout << "a " << cnt;
        REP(i, N) REP(j, N) if((ans_board[i]>>(3*j)&7) > 0) cout << " " << i << " " << j;
        cout << endl;
        cout.flush();

        int ret;
        cin >> ret;
        if(ret == 1) return 0;
        else {
          //その状態を0％にした上で続行
          state_list[0].prob = 0;
          double prob_sum = 0;
          REP(i, state_list.size()) prob_sum += state_list[i].prob;
          REP(i, state_list.size()) state_list[i].prob /= prob_sum;
          continue;
        }
      }

      //クエリを出力
      cout << "q " << query_cnt;
      REP(i, N) REP(j, N) if(query[i][j]) cout << " " << i << " " << j;
      cout << endl;
      cout.flush();

      //クエリ結果を受け取る
      int ret;
      cin >> ret;
      chmax(ret, 0);
      chmin(ret, Vsum);

      //確率をベイズ更新
      //p(v|x) ~ p(x|v) * p(v)を使って更新する
      state_cnt = vector<int>(state_list.size());
      REP(i, N) REP(j, N) if(query[i][j]) REP(k, state_list.size()) state_cnt[k] += state_list[k].board[i] >> (3*j) & 7;
      double prob_sum = 0;
      REP(i, state_list.size()) {
        state_list[i].prob *= p_x_v[query_cnt][state_cnt[i]][ret];
        prob_sum += state_list[i].prob;
      }
      REP(i, state_list.size()) state_list[i].prob /= prob_sum;
    }

  } else {
    /*****************************************/
    //Mが中程度の場合は範囲クエリ＋焼きなましサンプリングベイズ推定
    /*****************************************/

    int turn = 0;
    vector<Query> query_list;
    vector<vector<vector<int>>> pos_to_query(N, vector<vector<int>>(N)); //各マスから対応するクエリIDを持つ
    while(turn < 2*N*N) {

      auto time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > 2930) {
        //TLE緊急回避モード
        //安定解に切り替える
        vector<vector<int>> v(N, vector<int>(N, -1));

        int cur_Vsum = 0;
        REP(i, N) REP(j, N) if(v[i][j] > 0) cur_Vsum += v[i][j];
        deque<P> que;
        REP(i, N) REP(j, N) if(v[i][j] > 0) {
          REP(k, 4) {
            int nx = i + dx[k], ny = j + dy[k];
            if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if(v[nx][ny] == -1) que.push_back(P(nx, ny));
          }
        }
        while(true) {
          if(cur_Vsum == Vsum) break;
          int target_x = -1, target_y = -1;
          while(!que.empty()) {
            P p = que.front();
            que.pop_front();
            int x = p.first, y = p.second;
            if(v[x][y] == -1) {
              target_x = x;
              target_y = y;
              break;
            }
          }
          if(target_x == -1) {
            //最も確定マスから遠い未確定マスを選ぶ
            vector<vector<int>> dist(N, vector<int>(N, INF));
            deque<P> que_bfs;
            REP(i, N) REP(j, N) {
              if(v[i][j] != -1) {
                dist[i][j] = 0;
                que_bfs.push_front(P(i, j));
              } else if(i == 0 || i == N-1 || j == 0 || j == N-1) {
                dist[i][j] = 1;
                que_bfs.push_back(P(i, j));
              }
            }
            while(!que_bfs.empty()) {
              P p = que_bfs.front();
              que_bfs.pop_front();
              int x = p.first, y = p.second;
              REP(k, 4) {
                int nx = x + dx[k], ny = y + dy[k];
                if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
                if(dist[nx][ny] > dist[x][y] + 1) {
                  dist[nx][ny] = dist[x][y] + 1;
                  que_bfs.push_back(P(nx, ny));
                }
              }
            }
            int max_dist = 0;
            REP(i, N) REP(j, N) if(dist[i][j] > max_dist) {
              max_dist = dist[i][j];
              target_x = i;
              target_y = j;
            }
          }

          if(turn == 2*N*N) break;
          turn++;

          cout << "q 1 " << target_x << " " << target_y << endl;
          cout.flush();
          cin >> v[target_x][target_y];

          //クエリ結果が1以上なら周囲のマスをキューに入れる
          if(v[target_x][target_y] > 0) {
            cur_Vsum += v[target_x][target_y];
            REP(k, 4) {
              int nx = target_x + dx[k], ny = target_y + dy[k];
              if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
              if(v[nx][ny] == -1) que.push_back(P(nx, ny));
            }
          }
        }

        if(turn == 2*N*N) break;
        turn++;

        int cnt = 0;
        REP(i, N) REP(j, N) if(v[i][j] > 0) cnt++;

        cout << "a " << cnt;
        REP(i, N) REP(j, N) if(v[i][j] > 0) cout << " " << i << " " << j;
        cout << endl;
        cout.flush();

        int ret;
        cin >> ret;
        if(ret == 1) return 0;
        else cerr << "error" << endl;
        return 0;
      }

      turn++;

      if(turn == 1) {
        //1ターン目はランダムサンプリングを行う
        REP(i, STATE_SET_SIZE) {
          vector<int> sx(M), sy(M);
          REP(i, M) {
            sx[i] = mt() % (N - poly_size_x[i] + 1);
            sy[i] = mt() % (N - poly_size_y[i] + 1);
          }
          vector<lint> board(N);
          REP(i, M) REP(j, poly_bit[i].size()) board[sx[i]+j] += poly_bit[i][j] << (3*sy[i]);
          State state(sx, sy, board);
          state.prob = 1.0 / STATE_SET_SIZE;
          if(hash_set.count(state.hash) == 0) {
            state_list.push_back(state);
            hash_set.insert(state.hash);
          }
        }
      } else sort(ALL(state_list));

      //ある状態の確率がx%を超えたらそれを出力する
      double max_prob = state_list[0].prob;
      //cerr << turn << " " << max_prob << " " << state_list.size() << " " << time << endl;
      cout << "# max_prob:" << max_prob << " " << state_list.size() << " " << time << endl;

      //終了判定？
      //手持ちの状態のうち確率最大値が閾値を上回ること
      if(max_prob > MAX_PROB) {
        vector<lint> ans_board = state_list[0].board;
        int cnt = 0;
        REP(i, N) REP(j, N) if((ans_board[i]>>(3*j)&7) > 0) cnt++;
        cout << "a " << cnt;
        REP(i, N) REP(j, N) if((ans_board[i]>>(3*j)&7) > 0) cout << " " << i << " " << j;
        cout << endl;
        cout.flush();

        int ret;
        cin >> ret;
        if(ret == 1) return 0;
        else {
          //その状態を削除して続行
          //tried_hash.insert(state_list[0].hash);
          state_list[0].prob = 0;
          double prob_sum = 0;
          REP(i, state_list.size()) prob_sum += state_list[i].prob;
          REP(i, state_list.size()) state_list[i].prob /= prob_sum;
          continue;
        }
      }
      //手持ちの状態から焼きなましを行って状態の水増しを行う
      //遷移は図形のスライド、スワップ

      if(turn >= 2) {
        //2ターン目以降は手持ちの状態をベースに図形スライドを行い、確率の低いものを排除していく
        //この実装だと1スライドしかできないので逐次的にサンプルするようにしたい
        vector<State> next_state_list;
        if(M >= 4) STATE_REMAIN_CNT = STATE_REMAIN_CNT_M4;
        if(M >= 5) STATE_REMAIN_CNT = STATE_REMAIN_CNT_M5;
        if(M >= 6) STATE_REMAIN_CNT = STATE_REMAIN_CNT_M6;
        if(M >= 8) STATE_REMAIN_CNT = STATE_REMAIN_CNT_M8;
        if(M >= 10) STATE_REMAIN_CNT = STATE_REMAIN_CNT_M10;
        chmin(STATE_REMAIN_CNT, (int)state_list.size());
        REP(k, STATE_REMAIN_CNT) {
          //確率の計算
          double prob = 1;
          for(auto q : query_list) {
            int cnt = 0;
            REP(i, N) REP(shift, 3) cnt += __builtin_popcountll((state_list[k].board[i] & (q.mask[i]<<shift))) * (1<<shift);
            prob *= p_x_v[q.size][cnt][q.ret];
          }
          State next_state = State(state_list[k].sx, state_list[k].sy, state_list[k].board);
          next_state.prob = prob;
          next_state_list.push_back(next_state);
        }

        chmin(YAKINAMASHI_LOOP_CNT, (int)state_list.size());
        chmin(YAKINAMASHI_LOOP_CNT, STATE_REMAIN_CNT);
        vector<double> weight(YAKINAMASHI_LOOP_CNT, 0);
        REP(i, YAKINAMASHI_LOOP_CNT) weight[i] = pow(state_list[i].prob, WEIGHT_POWER);

        REP(k, YAKINAMASHI_LOOP_CNT) {
          auto time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
          if(time > 2930) {
            break;
          }
          //焼きなましをしながら状態生成
          int LOOP_CNT = weight[k] * LOOP_CNT_FACTOR;
          if(M >= 4) LOOP_CNT = weight[k] * LOOP_CNT_FACTOR_M4;
          if(M >= 5) LOOP_CNT = weight[k] * LOOP_CNT_FACTOR_M5;
          if(M >= 6) LOOP_CNT = weight[k] * LOOP_CNT_FACTOR_M6;
          if(M >= 8) LOOP_CNT = weight[k] * LOOP_CNT_FACTOR_M8;
          if(M >= 10) LOOP_CNT = weight[k] * LOOP_CNT_FACTOR_M10;
          chmax(LOOP_CNT, 1);
          State s = next_state_list[k];
          double best_prob = s.prob;
          vector<int> best_sx(s.sx), best_sy(s.sy);

          //各クエリの結果を持っておくことで差分計算を目指す
          //各マス→どのクエリに対応するか の紐づけにより可能に
          vector<int> best_query_cnt(query_list.size(), 0);
          REP(i, query_list.size()) {
            REP(j, N) REP(shift, 3) best_query_cnt[i] += __builtin_popcountll(s.board[j] & (query_list[i].mask[j]<<shift)) * (1<<shift);
          }

          REP(loop, LOOP_CNT) {
            vector<int> sx(best_sx), sy(best_sy);
            vector<int> query_cnt(best_query_cnt);
            int do_slide, do_swap;
            bool mode = mt() % 100 < SWAP_THRESHOLD;
            if(mode) {
              int i = mt() % M;
              int j = mt() % 4;
              sx[i] += dx[j];
              sy[i] += dy[j];
              if(sx[i] < 0 || sx[i] > N - poly_size_x[i] || sy[i] < 0 || sy[i] > N - poly_size_y[i]) continue;
              for(P p : slide_off[i][j]) {
                int x = best_sx[i] + p.first;
                int y = best_sy[i] + p.second;
                for(int qid : pos_to_query[x][y]) query_cnt[qid]--;
              }
              for(P p : slide_on[i][j]) {
                int x = best_sx[i] + p.first;
                int y = best_sy[i] + p.second;
                for(int qid : pos_to_query[x][y]) query_cnt[qid]++;
              }
            } else {
              int i, j;
              while(true) {
                i = mt() % M;
                j = mt() % M;
                if(i == j) continue;
                break;
              }
              swap(sx[i], sx[j]);
              swap(sy[i], sy[j]);
              sx[i] += (poly_size_x[j] - poly_size_x[i]) / 2;
              sy[i] += (poly_size_y[j] - poly_size_y[i]) / 2;
              sx[j] += (poly_size_x[i] - poly_size_x[j]) / 2;
              sy[j] += (poly_size_y[i] - poly_size_y[j]) / 2;

              bool do_slide_i = mt() % 100 < SWAP_SLIDE_THRESHOLD;
              int di = mt() % 4;
              if(do_slide_i) {
                sx[i] += dx[di];
                sy[i] += dy[di];
              }
              bool do_slide_j = mt() % 100 < SWAP_SLIDE_THRESHOLD;
              int dj = mt() % 4;
              if(do_slide_j) {
                sx[j] += dx[dj];
                sy[j] += dy[dj];
              }

              if(sx[i] < 0 || sx[i] > N - poly_size_x[i] || sy[i] < 0 || sy[i] > N - poly_size_y[i]) continue;
              if(sx[j] < 0 || sx[j] > N - poly_size_x[j] || sy[j] < 0 || sy[j] > N - poly_size_y[j]) continue;
              if(!do_slide_j) {
                for(P p : swap_off[i][j]) {
                  int x = best_sx[i] + p.first;
                  int y = best_sy[i] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]--;
                }
                for(P p : swap_on[i][j]) {
                  int x = best_sx[i] + p.first;
                  int y = best_sy[i] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]++;
                }
              } else {
                for(P p : swap_slide_off[i][j][dj]) {
                  int x = best_sx[i] + p.first;
                  int y = best_sy[i] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]--;
                }
                for(P p : swap_slide_on[i][j][dj]) {
                  int x = best_sx[i] + p.first;
                  int y = best_sy[i] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]++;
                }
              }
              if(!do_slide_i) {
                for(P p : swap_off[j][i]) {
                  int x = best_sx[j] + p.first;
                  int y = best_sy[j] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]--;
                }
                for(P p : swap_on[j][i]) {
                  int x = best_sx[j] + p.first;
                  int y = best_sy[j] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]++;
                }
              } else {
                for(P p : swap_slide_off[j][i][di]) {
                  int x = best_sx[j] + p.first;
                  int y = best_sy[j] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]--;
                }
                for(P p : swap_slide_on[j][i][di]) {
                  int x = best_sx[j] + p.first;
                  int y = best_sy[j] + p.second;
                  for(int qid : pos_to_query[x][y]) query_cnt[qid]++;
                }
              }
            }

            //確率の計算
            double prob = 1;
            REP(i, query_list.size()) {
              prob *= p_x_v[query_list[i].size][query_cnt[i]][query_list[i].ret];
            }

            double prob_ratio = prob / (1e-256 + best_prob);
            if(prob_ratio >= 1 || mt()%INF/(double)INF < pow(prob_ratio, TEMP)) {
              vector<lint> board(N);
              REP(i, M) REP(j, poly_bit[i].size()) board[sx[i]+j] += (poly_bit[i][j] << (3*sy[i]));
              State next_state = State(sx, sy, board);
              next_state.prob = prob;
              bool do_step = false;
              if(hash_set.count(next_state.hash) == 0) {
                next_state_list.push_back(next_state);
                hash_set.insert(next_state.hash);
                do_step = true;
              } else {
                //みたことある状態への遷移を抑えることで新しい状態のサンプリングを強めたい
                if(mt() % 100 < SEEN_STATE_STEP_PROB) do_step = true;
              }
              if(do_step) {
                best_prob = prob;
                best_sx = sx;
                best_sy = sy;
                best_query_cnt = query_cnt;
              }
            }
          }
        }

        sort(ALL(next_state_list));
        while(next_state_list.size() > STATE_SET_SIZE) next_state_list.pop_back();
        double prob_sum = 0;
        REP(i, next_state_list.size()) prob_sum += next_state_list[i].prob;
        REP(i, next_state_list.size()) next_state_list[i].prob /= prob_sum;
        state_list = next_state_list;
      }

      int USE_STATE_FOR_QUERY = 0;
      double prob_sum_for_query = 0;
      REP(i, state_list.size()) {
        prob_sum_for_query += state_list[i].prob;
        if((i >= 9 && prob_sum_for_query > 0.8) || i == USE_STATE_FOR_QUERY_MAX || i == state_list.size() - 1){
          USE_STATE_FOR_QUERY = i+1;
          break;
        }
      }

      bool give_up = false;

      //重み付き分散値ができるだけ大きくなるような点をクエリに貪欲に追加していく
      vector<vector<bool>> query(N, vector<bool>(N, false));
      vector<int> state_cnt(USE_STATE_FOR_QUERY);
      int query_cnt = 0;
      double max_value = -1e9;
      while(query_cnt < N*N - 1) {
        auto time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
        if(time > 2930) {
          give_up = true;
          break;
        }
        int target_x = -1, target_y = -1;
        if(query_cnt < 3) max_value = -1e9;
        REP(i, N) REP(j, N) if(!query[i][j]) {
          vector<int> state_cnt_tmp(state_cnt);
          REP(k, USE_STATE_FOR_QUERY) state_cnt_tmp[k] += state_list[k].board[i] >> (3*j) & 7;
          double mean = 0;
          REP(k, USE_STATE_FOR_QUERY) mean += state_cnt_tmp[k] * state_list[k].prob;
          mean /= prob_sum_for_query;
          double var = 0;
          REP(k, USE_STATE_FOR_QUERY) var += (state_cnt_tmp[k] - mean) * (state_cnt_tmp[k] - mean) * state_list[k].prob;
          double value = (var - ((query_cnt+1) * eps * (1 - eps))) * pow(query_cnt+1, QUERY_CNT_FACTOR);
          if(value > max_value) {
            max_value = value;
            target_x = i;
            target_y = j;
          }
        }
        //cerr << query_cnt << " " << max_value << endl;
        if(target_x == -1) break;
        query[target_x][target_y] = true;
        query_cnt++;
        REP(k, USE_STATE_FOR_QUERY) state_cnt[k] += state_list[k].board[target_x] >> (3*target_y) & 7;
      }

      if(turn == 1) {
        //初手は左右分割クエリで固定
        query = vector<vector<bool>>(N, vector<bool>(N, false));
        query_cnt = 0;
        REP(i, N) REP(j, N) if(j < N/2) {
          query[i][j] = true;
          query_cnt++;
        }

      }

      //終了判定？
      //手持ちの状態のうち確率最大値が閾値を上回ること
      if(query_cnt == 0 || give_up) {
        vector<lint> ans_board = state_list[0].board;
        int cnt = 0;
        REP(i, N) REP(j, N) if((ans_board[i]>>(3*j)&7) > 0) cnt++;
        cout << "a " << cnt;
        REP(i, N) REP(j, N) if((ans_board[i]>>(3*j)&7) > 0) cout << " " << i << " " << j;
        cout << endl;
        cout.flush();

        int ret;
        cin >> ret;
        if(ret == 1) return 0;
        else {
          //その状態を削除して続行
          state_list[0].prob = 0;
          double prob_sum = 0;
          REP(i, state_list.size()) prob_sum += state_list[i].prob;
          REP(i, state_list.size()) state_list[i].prob /= prob_sum;
          continue;
        }
      }

      //クエリを出力
      cout << "q " << query_cnt;
      REP(i, N) REP(j, N) if(query[i][j]) cout << " " << i << " " << j;
      cout << endl;
      cout.flush();

      //クエリ結果を受け取る
      int ret;
      cin >> ret;
      chmax(ret, 0);
      chmin(ret, Vsum);

      //確率をベイズ更新
      //p(v|x) ~ p(x|v) * p(v)を使って更新する
      state_cnt = vector<int>(state_list.size());
      REP(i, N) REP(j, N) if(query[i][j]) REP(k, state_list.size()) state_cnt[k] += state_list[k].board[i] >> (3*j) & 7;
      double prob_sum = 0;
      REP(i, state_list.size()) {
        state_list[i].prob *= p_x_v[query_cnt][state_cnt[i]][ret];
        prob_sum += state_list[i].prob;
      }
      REP(i, state_list.size()) state_list[i].prob /= prob_sum;

      //クエリを記録
      Query q;
      q.ret = ret;
      q.size = 0;
      q.mask = vector<lint>(N, 0);
      REP(i, N) REP(j, N) if(query[i][j]) {
        q.mask[i] += 1LL << (3*j);
        q.size++;
        pos_to_query[i][j].push_back(query_list.size());
      }
      query_list.push_back(q);
    }

  }
}