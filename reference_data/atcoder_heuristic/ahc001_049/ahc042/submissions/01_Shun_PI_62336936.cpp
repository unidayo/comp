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

vector<int> dx = {-1, 0, 1, 0};
vector<int> dy = {0, -1, 0, 1};
vector<char> dc = {'U', 'L', 'D', 'R'};
mt19937 mt(0);

unsigned long xor128(void)
{
    static unsigned long x=123456789,y=362436069,z=541288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

int N;
constexpr int TIME_LIMIT = 1980;

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N;
  vector<string> C(N);
  REP(i, N) cin >> C[i];

  vector<vector<int>> init_board(N, vector<int>(N, -1));
  REP(i, N) REP(j, N) {
    if(C[i][j] == 'x') init_board[i][j] = 0;
    else if(C[i][j] == 'o') init_board[i][j] = 1;
  }

  auto start_time = chrono::system_clock::now();
  
  //焼きなましをする
  //操作列を焼く
  //近傍は挿入、削除、swap
  //各操作はそれで搬出できる鬼がいなくなるまで行うものとする
  //評価関数は生スコア（操作列は大きくなりすぎないようにする）

  vector<tuple<int, int, int>> ans;
  vector<int> best_shift;
  int loop = 0;
  int best_turn = 0;
  int best_donecnt = 0;
  float best_score = 0;
  float start_temp = 0.1, end_temp = -0.4;
  float start_donecnt_factor = 5, end_donecnt_factor = 12;
  float start_penalty = 10, end_penalty = 1000;
  float donecnt_factor = start_donecnt_factor;
  float temp = start_temp;
  float penalty_factor = start_penalty;
  float time;
  float temppow = pow(0.1, temp);

  int best_turn2 = INF;
  vector<tuple<int, int, int>> ans2;
  vector<int> best_shift2;

  while(true) {
    if(loop%3000 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      //cerr << loop << " " << best_donecnt << " " << best_turn << " " << best_score << " " << ans.size() << endl;
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT;
      temppow = pow(0.1, temp);
      donecnt_factor = start_donecnt_factor + (end_donecnt_factor - start_donecnt_factor) * time / (float)TIME_LIMIT;
      penalty_factor = start_penalty + (end_penalty - start_penalty) * time / (float)TIME_LIMIT;
    }
    loop++;

    int mode_rnd = xor128() % 100;
    int mode = 0;
    if(mode_rnd < 15) mode = 0;
    else if(mode_rnd < 30) mode = 1;
    else if(mode_rnd < 80) mode = 2;
    else if(mode_rnd < 90) mode = 3;
    else mode = 4;
    if(ans.size() <= 20) mode = 0;
    vector<tuple<int, int, int>> last_ans(ans);
    if(mode == 0) {
      //挿入
      int idx = xor128() % (ans.size() + 1);
      int d = xor128() % 4;
      int x = xor128() % N;
      int lim = ((xor128() % 5 == 0) ? -1 : 1);
      ans.insert(ans.begin() + idx, {d, x, lim});
    } else if(mode == 1) {
      //削除
      int idx = xor128() % ans.size();
      if(xor128()%2 == 0) idx = ans.size()-1; //末尾削除
      ans.erase(ans.begin() + idx);
    } else if(mode == 2) {
      //swap
      int i, j;
      while(true) {
        i = xor128() % ans.size();
        j = xor128() % ans.size();
        if(i != j && abs(i-j) < 10) break;
      }
      swap(ans[i], ans[j]);
    } else if(mode == 3) {
      //変更
      int idx = xor128() % ans.size();
      int d = xor128() % 4;
      int x = xor128() % N;
      int lim = ((xor128() % 5 == 0) ? -1 : 1);
      ans[idx] = {d, x, lim};
    } else {
      //操作の方向を逆にする
      int idx = xor128() % ans.size();
      get<0>(ans[idx]) = (get<0>(ans[idx]) + 2) % 4;
    } 

    //スコアの計算
    //操作列を手前から見て、その操作で搬出できる鬼を全て搬出する
    //何も搬出できない場合は1手だけ動かす（このとき福が搬出されたら大きなペナルティ）
    vector<vector<int>> board(init_board);
    int turn = 0;
    int donecnt = 0;
    int penalty = 0;
    vector<int> shift;
    for(auto p: ans) {
      int d = get<0>(p);
      int x = get<1>(p);
      int lim = get<2>(p);
      if(lim == -1) lim = N;
      int cnt = 1;
      if(d == 0) {
        REP(i, N) {
          if(i >= lim) break;
          if(board[i][x] == 0) {
            cnt = i+1;
            donecnt++;
          } else if(board[i][x] == 1) {
            if(i == 0) penalty++;
            break;
          }
        }
      } else if(d == 1) {
        REP(i, N) {
          if(i >= lim) break;
          if(board[x][i] == 0) {
            cnt = i+1;
            donecnt++;
          } else if(board[x][i] == 1) {
            if(i == 0) penalty++;
            break;
          }
        }
      } else if(d == 2) {
        IREP(i, N) {
          if(N-1-i >= lim) break;
          if(board[i][x] == 0) {
            cnt = N-i;
            donecnt++;
          } else if(board[i][x] == 1) {
            if(i == N-1) penalty++;
            break;
          }
        }
      } else {
        IREP(i, N) {
          if(N-1-i >= lim) break;
          if(board[x][i] == 0) {
            cnt = N-i;
            donecnt++;
          } else if(board[x][i] == 1) {
            if(i == N-1) penalty++;
            break;
          }
        } 
      }
      turn += cnt;
      shift.push_back(cnt);
      //シフト操作の実施
      if(d == 0) {
        REP(i, N) {
          if(i < N-cnt) board[i][x] = board[i+cnt][x];
          else board[i][x] = -1;
        }
      } else if(d == 1) {
        REP(i, N) {
          if(i < N-cnt) board[x][i] = board[x][i+cnt];
          else board[x][i] = -1;
        }
      } else if(d == 2) {
        IREP(i, N) {
          if(i >= cnt) board[i][x] = board[i-cnt][x];
          else board[i][x] = -1;
        }
      } else {
        IREP(i, N) {
          if(i >= cnt) board[x][i] = board[x][i-cnt];
          else board[x][i] = -1;
        }
      }
    }

    float score = donecnt * donecnt_factor - turn - penalty * penalty_factor;
    float prob = exp(((float)score-best_score)*temppow);
    if(score >= best_score || prob*(float)INF > (xor128()%INF)) {
      best_score = score;
      best_shift = shift;
      best_turn = turn;
      best_donecnt = donecnt;
      if(donecnt == 2*N && turn < best_turn2 && penalty == 0) {
        ans2 = ans;
        best_shift2 = shift;
        best_turn2 = turn;
      }
    } else {
      ans = last_ans;
    }
  }

  REP(i, ans2.size()) {
    int shift = best_shift2[i];
    auto p = ans2[i];
    REP(z, shift) cout << dc[get<0>(p)] << " " << get<1>(p) << endl;
  }

  if(best_donecnt == 2*N) cerr << 3200 - best_turn2 << endl;
  else cerr << 0 << endl;
}