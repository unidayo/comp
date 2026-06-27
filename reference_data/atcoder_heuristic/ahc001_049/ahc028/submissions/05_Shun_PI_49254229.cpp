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

int N, M, si, sj;
int dp2[60750000];
int len_prefix[200*200];

constexpr int C1 = 15*15*6*15*15;
constexpr int C2 = 6*15*15;
constexpr int C3 = 15*15;

unsigned long xor128(){
  static unsigned long x=123456789, y=362436069, z=521288629, w=88675123;
  unsigned long t;
  t=(x^(x<<11));
  x=y; y=z; z=w;
  return w=(w^(w>>19))^(t^(t>>8));
}

int calc_score(vector<int> &p, vector<int> &endpos, vector<vector<vector<vector<int>>>> &dp) {
  int score = 0;
  int x = si;
  int y = sj;
  REP(i, p.size()) {
    int l = 0;
    if(i > 0) l = len_prefix[p[i-1]*M+p[i]];
    score += dp[p[i]][endpos[i]][l][x*N+y];
    x = endpos[i]/N;
    y = endpos[i]%N;
  }
  return score;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> M >> si >> sj;
  vector<string> A(N);
  REP(i, N) cin >> A[i];
  vector<string> t(M);
  REP(i, M) cin >> t[i];

  auto start_time = std::chrono::system_clock::now();

  //何マス被るか
  fill_n(len_prefix, 200*200, 0);
  REP(i, M) {
    REP(j, M) {
      FOR(l, 1, 5) {
        if (t[i].substr(5-l, l) == t[j].substr(0, l)) {
          len_prefix[i*M+j] = l;
        }
      }
    }
  }

  //各文字の位置
  vector<vector<int>> pos(26);
  REP(i, N) {
    REP(j, N) {
      pos[A[i][j]-'A'].push_back(i*N+j);
    }
  }

  //各単語について、スタート地点とゴール地点が決まった時に最適なルートを求める
  //スタート地点は全マス＋文字被りを考慮した分
  //ゴール地点は単語の最後の文字全て
  //N*N*5頂点に対するダイクストラ法
  //単語ID、終わり文字位置(N*Nだけど実際は1/26)、達成済み文字数、地点（N*N）に対してDPをする（ルートは経路復元）
  vector<vector<vector<vector<int>>>> dp(M, vector<vector<vector<int>>>(N*N, vector<vector<int>>(6, vector<int>(N*N, INF))));
  vector<vector<vector<vector<int>>>> nextpos(M, vector<vector<vector<int>>>(N*N, vector<vector<int>>(6, vector<int>(N*N, -1))));
  REP(i, M) REP(j, N*N) {
    if(A[j/N][j%N] != t[i].back()) continue;
    dp[i][j][5][j] = 0;
    IREP(l, 5) {
      REP(x, N*N) if(dp[i][j][l+1][x] != INF) {
        if(l > 0) {
          for(int y : pos[t[i][l-1]-'A']) {
            int d = abs(x/N-y/N) + abs(x%N-y%N) + 1;
            if (dp[i][j][l+1][x] + d < dp[i][j][l][y]) {
              dp[i][j][l][y] = dp[i][j][l+1][x] + d;
              nextpos[i][j][l][y] = x;
            }
          }
        } else {
          REP(y, N*N) {
            int d = abs(x/N-y/N) + abs(x%N-y%N) + 1;
            if (dp[i][j][l+1][x] + d < dp[i][j][l][y]) {
              dp[i][j][l][y] = dp[i][j][l+1][x] + d;
              nextpos[i][j][l][y] = x;
            }
          }
        }
      }
    }
  }

  //TODO: dpを1次元配列にする（スコア計算の高速化）
  fill_n(dp2, M*N*N*6*N*N, INF);
  REP(i, M) REP(j, N*N) REP(l, 6) REP(x, N*N) dp2[i*C1+j*C2+l*C3+x] = dp[i][j][l][x];

  //単語列と各単語の終了位置を焼きなまし
  //終了位置はその単語の終了文字である必要がある

  //いい感じの初期解を考えたい！
  //残っている単語のうち被りが最も大きい単語を選ぶ
  vector<int> p;
  set<int> rest;
  REP(i, M) rest.insert(i);
  REP(i, M) {
    if(t[i][0] == A[si][sj]) {
      p.push_back(i);
      rest.erase(i);
      break;
    }
  }
  while(!rest.empty()) {
    int last = p.back();
    int best = -1;
    int best_score = -1;
    for(int i : rest) {
      int score = len_prefix[last*M+i];
      if(score > best_score) {
        best_score = score;
        best = i;
      }
    }
    p.push_back(best);
    rest.erase(best);
  }

  vector<int> endpos;
  REP(i, M) endpos.push_back(pos[t[p[i]].back()-'A'][0]);
  int best_score = calc_score(p, endpos, dp);
  int best_score_2 = best_score;
  vector<int> best_p(p);
  vector<int> best_endpos(endpos);

  cerr << "init score: " << 10000 - best_score << endl;

  //焼きなまし
  double time = 0;
  int loop = 0;
  int last_time = 0;
  int loop_interval = 1;
  int next_check = 1;
  float start_temp = 1.5, end_temp = 0.2;
  float TEMP_FACTOR = 1.1;
  float temp = start_temp;
  int TIME_LIMIT = 1900;
  //mt19937 mt(0);
  while(true) {
    if(loop == next_check) {
      auto now_time = std::chrono::system_clock::now();
      time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time-start_time).count();
      //cerr << "LOOP:" << loop << " SCORE:" << best_score << " TIME:" << time << endl;
      if(time > TIME_LIMIT) break;
      if(time - last_time < 25) loop_interval *= 2;
      else if(time - last_time > 50 && loop_interval > 1) loop_interval /= 2;
      last_time = time;
      next_check += loop_interval;
      temp = start_temp + (end_temp - start_temp) * pow(time / TIME_LIMIT, TEMP_FACTOR);
    }
    loop++;

    //終了位置変更操作または順序変更（どうやって？）をする
    int MODE_BASE = 100;
    int mode = xor128() % MODE_BASE;
    int x, y;
    int l, r;
    int score = best_score;
    bool do_end_change = mode <= 30;
    if(do_end_change) {
      //終了位置変更
      while(true) {
        x = xor128() % M;
        if(pos[t[p[x]].back()-'A'].size() == 1) continue;
        y = xor128() % pos[t[p[x]].back()-'A'].size();
        if(pos[t[p[x]].back()-'A'][y] == endpos[x]) continue;
        break;
      }
      int next_endpos = pos[t[p[x]].back()-'A'][y];

      //スコアの更新
      score -= dp2[p[x]*C1+endpos[x]*C2+(x > 0 ? len_prefix[p[x-1]*M+p[x]] : 0)*C3+(x > 0 ? endpos[x-1] : si*N+sj)];
      if(x < M-1) score -= dp2[p[x+1]*C1+endpos[x+1]*C2+len_prefix[p[x]*M+p[x+1]]*C3+endpos[x]];
      score += dp2[p[x]*C1+next_endpos*C2+(x > 0 ? len_prefix[p[x-1]*M+p[x]] : 0)*C3+(x > 0 ? endpos[x-1] : si*N+sj)];
      if(x < M-1) score += dp2[p[x+1]*C1+endpos[x+1]*C2+len_prefix[p[x]*M+p[x+1]]*C3+next_endpos];
    } else {
      //単語の挿入
      //ある区間を選択してそれを別の場所に挿入する
      while(true) {
        l = xor128() % (M+1);
        r = xor128() % (M+1);
        y = xor128() % (M+1);
        int mode2 = xor128() % 2;
        if(l > r) swap(l, r);
        if(mode2 == 0) {
          if(y > r) swap(y, r);
          if(y > l) swap(y, l);
          if(y == l) continue;
        } else {
          if(y < l) swap(y, l);
          if(y < r) swap(y, r);
          if(y == r) continue;
        }
        if(l == r) continue;
        r--;
        break;
      }

      //スコアの更新
      score -= dp2[p[l]*C1+endpos[l]*C2+(l > 0 ? len_prefix[p[l-1]*M+p[l]] : 0)*C3+(l > 0 ? endpos[l-1] : si*N+sj)];
      if(r < M-1) {
        score -= dp2[p[r+1]*C1+endpos[r+1]*C2+len_prefix[p[r]*M+p[r+1]]*C3+endpos[r]];
        score += dp2[p[r+1]*C1+endpos[r+1]*C2+(l > 0 ? len_prefix[p[l-1]*M+p[r+1]] : 0)*C3+(l > 0 ? endpos[l-1] : si*N+sj)];
      }
      score += dp2[p[l]*C1+endpos[l]*C2+(y > 0 ? len_prefix[p[y-1]*M+p[l]] : 0)*C3+(y > 0 ? endpos[y-1] : si*N+sj)];
      if(y < M) {
        score -= dp2[p[y]*C1+endpos[y]*C2+(y > 0 ? len_prefix[p[y-1]*M+p[y]] : 0)*C3+(y > 0 ? endpos[y-1] : si*N+sj)];
        score += dp2[p[y]*C1+endpos[y]*C2+len_prefix[p[r]*M+p[y]]*C3+endpos[r]];
      }

    }
    

    float prob = exp(((float)best_score-score)/temp);
    if(score <= best_score || prob > (xor128()%INF)/(float)INF) {
      best_score = score;
      if(!do_end_change) {
        //挿入操作を行う
        vector<int> new_p;
        REP(i, M+1) {
          if(i == y) FOR(j, l, r+1) new_p.push_back(p[j]);
          if(i == M) break;
          if(l <= i && i <= r) continue;
          else new_p.push_back(p[i]);
        }
        p = new_p;
        vector<int> new_endpos;
        REP(i, M+1) {
          if(i == y) FOR(j, l, r+1) new_endpos.push_back(endpos[j]);
          if(i == M) break;
          if(l <= i && i <= r) continue;
          else new_endpos.push_back(endpos[i]);
        }
        endpos = new_endpos;
      } else {
        endpos[x] = pos[t[p[x]].back()-'A'][y];
      }

      if(best_score < best_score_2 && time > 1600) {
        best_score_2 = best_score;
        best_p = vector<int>(p);
        best_endpos = vector<int>(endpos);
      }
    }

  }


  //結果の出力
  vector<P> ans;
  int x = si;
  int y = sj;
  REP(i, M) {
    int l = 0;
    if(i > 0) l = len_prefix[best_p[i-1]*M+best_p[i]];
    while(true) {
      int d = nextpos[best_p[i]][best_endpos[i]][l][x*N+y];
      if(d == -1) break;
      x = d/N;
      y = d%N;
      l++;
      ans.emplace_back(x, y);
    }
  }
  for(auto p : ans) {
    cout << p.first << " " << p.second << "\n";
  }

  cerr << "loop: " << loop << endl;
  cerr << 10000 - best_score_2 << endl;
}