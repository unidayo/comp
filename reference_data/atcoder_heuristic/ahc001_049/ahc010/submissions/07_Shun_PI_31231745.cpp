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
constexpr bool DEBUG = false;
constexpr int TIME_LIMIT = 1850;
constexpr int LIMIT_CNT = 5000;
constexpr int RETURN_TURN = 900;
constexpr int LOOP_CHECK = 300;

vector<int> dx = {0, -1, 0, 1};
vector<int> dy = {-1, 0, 1, 0};
vector<vector<int>> to = {
    {1, 0, -1, -1},
    {3, -1, -1, 0},
    {-1, -1, 3, 2},
    {-1, 2, 1, -1},
    {1, 0, 3, 2},
    {3, 2, 1, 0},
    {2, -1, 0, -1},
    {-1, 3, -1, 1}
};
vector<string> t, s, bestt;
vector<vector<int>> vst, bestvst;
bool conn_d(int i, int j) {
  if(i < 0 || j < 0 || i >= 30 || j >= 30) return false;
  if(i == 29) return false;
  if(t[i][j] == '1' || t[i][j] == '2' || t[i][j] == '4' || t[i][j] == '5' || t[i][j] == '7') {
    if(t[i+1][j] == '0' || t[i+1][j] == '3' || t[i+1][j] == '4' || t[i+1][j] == '5' || t[i+1][j] == '7') return true;
  }
  return false;
};
bool conn_r(int i, int j) {
  if(i < 0 || j < 0 || i >= 30 || j >= 30) return false;
  if(j == 29) return false;
  if(t[i][j] == '2' || t[i][j] == '3' || t[i][j] == '4' || t[i][j] == '5' || t[i][j] == '6') {
    if(t[i][j+1] == '0' || t[i][j+1] == '1' || t[i][j+1] == '4' || t[i][j+1] == '5' || t[i][j+1] == '6') return true;
  }
  return false;
};
string get_ans() {
  string ans = "";
  REP(i, 30) REP(j, 30) {
    ans += (char)('0' + (t[i][j] - s[i][j] + 4) % 4);
  }
  return ans;
}
int calc_score() {
  vector<int> l;
  vector<vector<vector<bool>>> vst(30, vector<vector<bool>>(30, vector<bool>(4)));
  REP(x, 30) REP(y, 30) REP(k, 4) if(!vst[x][y][k]) {
    int i = x;
    int j = y;
    int d = k;
    int length = 0;
    while(true) {
      vst[i][j][d] = true;
      int d2 = to[t[i][j] - '0'][d];
      if (d2 == -1) {
        length = 0;
        break;
      }
      vst[i][j][d2] = true;
      i += dx[d2];
      j += dy[d2];
      if (i < 0 || i >= 30 || j < 0 || j >= 30) {
        length = 0;
        break;
      }
      d = (d2 + 2) % 4;
      length += 1;
      if (i == x && j == y && d == k) break;
    }
    l.push_back(length);
  }
  sort(ALL(l));
  if(l.size() <= 1) return 0;
  else return l[l.size()-1] * l[l.size()-2];
}
unordered_map<char, char> next_char;

int si, sj, sd;
int i, j, d;
int length;
int cnt;
bool dfs() {
  //cerr << length << " " << abs(si - i) + abs(sj - j) << endl;
  //cerr << get_ans() << endl;
  if (length > 0 && i == si && j == sj && d == sd) {
    return true;
  }
  cnt++;
  if(cnt >= LIMIT_CNT) return false;
  vector<char> valid_t;
  REP(z, 4) {
    if(vst[i][j] == 1 && z != 0) continue;
    if(t[i][j] >= '4' && z >= 2) continue;
    char m = t[i][j];
    REP(k, z) m = next_char[m];
    int d2 = to[m - '0'][d];
    if(d2 != -1) valid_t.push_back(m);
  }
  vst[i][j]++;
  //TODO: valid_tの優先方向を選ぶ
  if(valid_t.size() == 2) {
    int d21 = to[valid_t[0] - '0'][d];
    int i1 = i + dx[d21];
    int j1 = j + dy[d21];
    int d1 = abs(si - i1) + abs(sj - j1);
    int d22 = to[valid_t[1] - '0'][d];
    int i2 = i + dx[d22];
    int j2 = j + dy[d22];
    int d2 = abs(si - i2) + abs(sj - j2);
    if(length <= RETURN_TURN) {
      if(d1 < d2) swap(valid_t[0], valid_t[1]);
    } else {
      if(d1 > d2) swap(valid_t[0], valid_t[1]);
    }
    if(d1 == d2 && rand() % 2 == 0) swap(valid_t[0], valid_t[1]);
  }

  for(char vt : valid_t) {
    char tmp = t[i][j];
    t[i][j] = vt;
    int d2 = to[vt - '0'][d];
    i += dx[d2];
    j += dy[d2];
    if (i >= 0 && i < 30 && j >= 0 && j < 30 && vst[i][j] <= 1) {
      int tmpd = d;
      d = (d2 + 2) % 4;
      length++;
      if(dfs()) return true;
      length--;
      d = tmpd;
    }
    i -= dx[d2];
    j -= dy[d2];
    t[i][j] = tmp;
  }
  vst[i][j]--;

  return false;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  t = vector<string>(30);
  REP(i, 30) cin >> t[i];
  s = vector<string>(t);

  next_char['0'] = '1';
  next_char['1'] = '2';
  next_char['2'] = '3';
  next_char['3'] = '0';
  next_char['4'] = '5';
  next_char['5'] = '4';
  next_char['6'] = '7';
  next_char['7'] = '6';

  auto start_time = std::chrono::system_clock::now();
  double time = 0;

  int loop = 0;
  int best_score = 0;
  vector<string> bestt(t);
  vector<vector<int>> bestvst(30, vector<int>(30));
  while(true) {
    if(loop%LOOP_CHECK == 0) {
      auto now_time = std::chrono::system_clock::now();
      time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time-start_time).count();
      if(DEBUG) cout << "LOOP:" << loop << " SCORE:" << best_score << " TIME:" << time << endl;
      if(time > TIME_LIMIT) break;
    }
    loop++;

    si = rand() % 30;
    sj = rand() % 30;
    sd = rand() % 4;
    i = si;
    j = sj;
    d = sd;
    length = 0;
    cnt = 0;
    t = vector<string>(s);
    vst = vector<vector<int>>(30, vector<int>(30));
    bool ret = dfs();
    if(ret && length * length / 4 > best_score) {
      int besti = 0;
      int bestj = 0;
      int best_tmp_score = 0;
      REP(i, 30) {
        REP(j, 30) {
          if(vst[i][j] == 2 && (t[i][j] == '4' || t[i][j] == '5')) {
            t[i][j] = next_char[t[i][j]];
            int ret = calc_score();
            t[i][j] = next_char[t[i][j]];
            if(ret > best_tmp_score) { 
              best_tmp_score = ret;
              besti = i;
              bestj = j;
            }
          }
        }
      }
      t[besti][bestj] = next_char[t[besti][bestj]];
      if(best_tmp_score > best_score) {
        best_score = best_tmp_score;
        bestt = t;
        bestvst = vst;
      }
    }
  }
  t = bestt;

  cerr << best_score << endl;
  cout << get_ans() << endl;
}