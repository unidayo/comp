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
vector<int> dy = {0, 1, 0, -1};
constexpr int TIME_LIMIT = 1940;
constexpr int CUT_RAND_BASE_START = 4;
constexpr int CUT_RAND_BASE_END = 64;
constexpr double START_TEMP = 0.8;
constexpr double END_TEMP = -0.5;
int n, m;
#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;
int c[52*52];
int nbrcnt[101*101];

int check_can_del(int x, int y) {
  bool ok = true;
  bool check[4], check_diag[4];
  fill_n(check, 4, false);
  fill_n(check_diag, 4, false);
  int check_cnt = 0;
  int check_diag_cnt = 0;
  REP(d, 4) {
    int nx = x + dx[d];
    int ny = y + dy[d];
    if(nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
    if(c[x*n+y] == c[nx*n+ny]) {
      check[d] = true;
      check_cnt++;
    }
  }
  REP(d, 4) {
    int d2 = (d + 1) % 4;
    if(check[d] && check[d2]) {
      int nx = x + dx[d] + dx[d2];
      int ny = y + dy[d] + dy[d2];
      if(nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
      if(c[x*n+y] != c[nx*n+ny]) {
        ok = false;
        break;
      }
      check_diag[d] = true;
      check_diag_cnt++;
    }
  }
  if(check_diag_cnt >= 3) return -2;
  if(check_diag_cnt == 2 && (check_diag[0] && check_diag[2] || check_diag[1] && check_diag[3])) return -2;
  REP(d, 2) {
    if(check[d] && check[(d+2)%4]) {
      if(check_diag[d] && check_diag[(d+1)%4]) continue;
      if(check_diag[d+2] && check_diag[(d+3)%4]) continue;
      ok = false;
      break;
    }
  }
  if(ok) return -1;
  else if(check_cnt == 2) {
    REP(d, 4) if(check[d] && check[(d+1)%4]) return d;
    return -2;
  } else return -2;
}

pair<lint, double> change_color(int x, int y, int nc, lint score, double value) {
  REP(dd, 4) {
    int nnx = x + dx[dd];
    int nny = y + dy[dd];
    if(nnx < 0 || nnx >= n || nny < 0 || nny >= n) continue;
    if(c[x*n+y] == c[nnx*n+nny]) {
      continue;
    }
    int mic = min(c[x*n+y], c[nnx*n+nny]);
    int mac = max(c[x*n+y], c[nnx*n+nny]);
    if(nbrcnt[mic*m+mac] == -1) {
      continue;
    }
    nbrcnt[mic*m+mac]--;
    if(nbrcnt[mic*m+mac] == 0) {
      value -= 1e9;
    }
  }

  if(c[x*n+y] == 0) {
    score--;
    value--;
  }
  assert(c[x*n+y] != nc);
  c[x*n+y] = nc;
  if(c[x*n+y] == 0) {
    score++;
    value++;
  }
  REP(dd, 4) {
    int nnx = x + dx[dd];
    int nny = y + dy[dd];
    if(nnx < 0 || nnx >= n || nny < 0 || nny >= n) continue;
    if(c[x*n+y] == c[nnx*n+nny]) {
      continue;
    }
    int mic = min(c[x*n+y], c[nnx*n+nny]);
    int mac = max(c[x*n+y], c[nnx*n+nny]);
    if(nbrcnt[mic*m+mac] == -1) {
      value -= 1e9;
      continue;
    }
    if(nbrcnt[mic*m+mac] == 0) {
      value += 1e9;
    }
    nbrcnt[mic*m+mac]++;
  }
  return make_pair(score, value);
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> n >> m;
  fill_n(c, 52*52, 0);
  REP(i, n) REP(j, n) cin >> c[(i+1)*(n+2)+(j+1)];
  n += 2;
  m++;
  auto start_time = std::chrono::system_clock::now();
  mt19937 mt(0);

  fill_n(nbrcnt, 101*101, 0);
  REP(i, n) REP(j, n){
    REP(d, 4) {
      int ni = i + dx[d];
      int nj = j + dy[d];
      if(ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
      if(c[i*n+j] == c[ni*n+nj]) continue;
      int mic = min(c[i*n+j], c[ni*n+nj]);
      int mac = max(c[i*n+j], c[ni*n+nj]);
      nbrcnt[mic*m+mac]++;
    }
  }
  REP(i, m) REP(j, m) nbrcnt[i*m+j] /= 2;
  REP(i, m) REP(j, m) if(nbrcnt[i*m+j] == 0) nbrcnt[i*m+j] = -1;

  lint best_score = 0;
  double time = 0;
  int loop = 0;
  int last_time = 0;
  int loop_interval = 1000;
  int next_check = 0;
  double start_temp = START_TEMP, end_temp = END_TEMP;
  double temp = start_temp;

  while(true) {
    if(loop == next_check) {
      auto now_time = std::chrono::system_clock::now();
      time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time-start_time).count();
      cerr << "LOOP:" << loop << " SCORE:" << best_score << " TIME:" << time << endl;
      if(time > TIME_LIMIT) break;
      if(time - last_time < 25) loop_interval *= 2;
      else if(time - last_time > 50 && loop_interval > 1) loop_interval /= 2;
      last_time = time;
      next_check += loop_interval;
      temp = start_temp + (end_temp - start_temp) * time / TIME_LIMIT;
    }
    loop++;

    int cut_rand_base = CUT_RAND_BASE_START + (CUT_RAND_BASE_END - CUT_RAND_BASE_START) * time / TIME_LIMIT;
    int mode = mt() % cut_rand_base;
    if(mode == 0) {
      //直線カット操作
      //色0から色0まで直線を引く
      //その上をカットして、小さい方の連結領域をスライドする
      //nbrcntを数えなおして、問題なければ採用
      //TODO: カットするときに1個ずらして切れるようにするとなんか多様性が生まれそう
      int sx, sy, d;
      int c_tmp[52*52];
      copy_n(c, 52*52, c_tmp);
      int zd = mt() % 4;
      if(d%2 == zd%2) zd = -1;
      vector<int> cut_x, cut_y;
      while(true) {
        sx = mt() % n;
        sy = mt() % n;
        d = mt() % 4;
        if(c[sx*n+sy] != 0) continue;
        bool ok = false;
        while(true) {
          int d2 = (d + 1) % 4;
          int d3 = (d + 3) % 4;
          int nx2 = sx + dx[d2];
          int ny2 = sy + dy[d2];
          int nx3 = sx + dx[d3];
          int ny3 = sy + dy[d3];
          int cnt = 0;
          if(!(nx2 < 0 || nx2 >= n || ny2 < 0 || ny2 >= n) && c[nx2*n+ny2] != 0) cnt++;
          if(!(nx3 < 0 || nx3 >= n || ny3 < 0 || ny3 >= n) && c[nx3*n+ny3] != 0) cnt++;
          if(!ok) {
            if(cnt == 2) ok = true;
          } else {
            if(cnt < 2) break;
          }
          if(ok) {
            int nnx = sx;
            int nny = sy;
            if(zd != -1) {
              nnx = sx + dx[zd];
              nny = sy + dy[zd];
              if(nnx < 0 || nnx >= n || nny < 0 || nny >= n) break;
            }
            if(c_tmp[nnx*n+nny] != 0) {
              c_tmp[nnx*n+nny] = 0;
              cut_x.push_back(nnx);
              cut_y.push_back(nny);
            }
          }

          int nx = sx + dx[d];
          int ny = sy + dy[d];
          if(nx < 0 || nx >= n || ny < 0 || ny >= n) break;
          if(c[nx*n+ny] == 0) break;
          sx = nx;
          sy = ny;
        }
        if(!ok) continue;
        break;
      }

      //連結領域が３つ以上の場合は不適
      //2つの場合は、小さい方の連結領域をスライドする
      dsu uf(n*n);
      REP(i, n) REP(j, n) if(c_tmp[i*n+j] != 0) {
        REP(d, 2) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if(ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
          if(c_tmp[ni*n+nj] == 0) continue;
          uf.merge(i*n+j, ni*n+nj);
        }
      }
      int cnt = 0;
      vector<int> leaders;
      REP(i, n) REP(j, n) if(c_tmp[i*n+j] != 0) {
        if(uf.leader(i*n+j) == i*n+j) {
          cnt++;
          leaders.push_back(i*n+j);
        }
      }
      int max_leader = leaders[0];
      FOR(i, 1, leaders.size()) if(uf.size(max_leader) < uf.size(leaders[i])) max_leader = leaders[i];
      //スライド方向を決める
      //カットした座標から見て両方向に別のグループがあればわかる
      int sd = -1;
      REP(k, cut_x.size()) {
        int x = cut_x[k];
        int y = cut_y[k];
        int d2 = (d + 1) % 4;
        int d3 = (d + 3) % 4;
        int nx2 = x + dx[d2];
        int ny2 = y + dy[d2];
        int nx3 = x + dx[d3];
        int ny3 = y + dy[d3];
        if(nx2 < 0 || nx2 >= n || ny2 < 0 || ny2 >= n) continue;
        if(nx3 < 0 || nx3 >= n || ny3 < 0 || ny3 >= n) continue;
        if(c_tmp[nx2*n+ny2] == 0 || c_tmp[nx3*n+ny3] == 0) continue;
        if(uf.leader(nx2*n+ny2) == uf.leader(nx3*n+ny3)) continue;
        if(uf.leader(nx2*n+ny2) != max_leader) sd = d3;
        else sd = d2;
        break;
      }
      if(sd == -1) continue;

      int c_aft[52*52];
      fill_n(c_aft, 52*52, 0);
      REP(i, n) REP(j, n) if(c_tmp[i*n+j] != 0) {
        if(uf.leader(i*n+j) != max_leader) {
          int ni = i + dx[sd];
          int nj = j + dy[sd];
          if(ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
          c_aft[ni*n+nj] = c_tmp[i*n+j];
        } else {
          c_aft[i*n+j] = c_tmp[i*n+j];
        }
      }
      //nbrcntを数えなおす
      int nbrcnt_tmp[101*101];
      fill_n(nbrcnt_tmp, 101*101, 0);
      REP(i, n) REP(j, n){
        REP(d, 2) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if(ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
          if(c_aft[i*n+j] == c_aft[ni*n+nj]) continue;
          int mic = min(c_aft[i*n+j], c_aft[ni*n+nj]);
          int mac = max(c_aft[i*n+j], c_aft[ni*n+nj]);
          nbrcnt_tmp[mic*m+mac]++;
        }
      }
      bool ok = true;
      REP(i, m) REP(j, m) if(nbrcnt[i*m+j] == -1) {
        if(nbrcnt_tmp[i*m+j] > 0) ok = false;
        nbrcnt_tmp[i*m+j] = -1;
      }
      REP(i, m) REP(j, m) if((nbrcnt[i*m+j] == 0) !=  (nbrcnt_tmp[i*m+j] == 0)) {
        ok = false;
      }
      if(!ok) continue;

      //再度連結チェック
      dsu uf_aft(n*n), uf_color(n*n);
      REP(i, n) REP(j, n) if(c_aft[i*n+j] != 0) {
        REP(d, 2) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if(ni < 0 || ni >= n || nj < 0 || nj >= n) continue;
          if(c_aft[ni*n+nj] == 0) continue;
          uf_aft.merge(i*n+j, ni*n+nj);
          if(c_aft[i*n+j] == c_aft[ni*n+nj]) uf_color.merge(i*n+j, ni*n+nj);
        }
      }
      cnt = 0;
      vector<int> color_cnts(m, 0);
      REP(i, n) REP(j, n) if(c_aft[i*n+j] != 0) {
        if(uf_aft.leader(i*n+j) == i*n+j) {
          cnt++;
        }
        if(uf_color.leader(i*n+j) == i*n+j) {
          color_cnts[c_aft[i*n+j]]++;
        }
      }
      if(cnt != 1) continue;
      ok = true;
      FOR(i, 1, m) if(color_cnts[i] != 1) ok = false;
      if(!ok) continue;

      //採用
      copy_n(c_aft, 52*52, c);
      copy_n(nbrcnt_tmp, 101*101, nbrcnt);
      best_score = 0;
      FOR(i, 1, n-1) FOR(j, 1, n-1) if(c[i*n+j] == 0) best_score++;
      continue;
    }



    //ランダムにマスと方向を選び、選んだ方向のマスの色で置き換える
    int x, y, d, nx, ny;
    vector<int> dix, diy;
    while(true) {
      x = mt() % (n-2) + 1;
      y = mt() % (n-2) + 1;
      d = mt() % 4;
      nx = x + dx[d];
      ny = y + dy[d];
      dix = vector<int>();
      diy = vector<int>();
      if(c[x*n+y] == c[nx*n+ny]) continue;
      int ret = check_can_del(x, y);
      int lastx = x;
      int lasty = y;
      while(ret >= 0) {
        int dix_tmp = lastx + dx[ret] + dx[(ret+1)%4];
        int diy_tmp = lasty + dy[ret] + dy[(ret+1)%4];
        if(c[lastx*n+lasty] == c[dix_tmp*n+diy_tmp]) continue;
        ret = check_can_del(dix_tmp, diy_tmp);
        dix.push_back(dix_tmp);
        diy.push_back(diy_tmp);
        lastx = dix_tmp;
        lasty = diy_tmp;
      }
      if(ret == -1) break;
    }
    lint score = best_score;
    double value = 0;
    int lastc = c[x*n+y];
    auto ret = change_color(x, y, c[nx*n+ny], score, value);
    score = ret.first;
    value = ret.second;

    vector<int> lastdic;
    int lastdic_tmp = lastc;
    REP(k, dix.size()) {
      int dix_tmp = dix[k];
      int diy_tmp = diy[k];
      lastdic.push_back(c[dix_tmp*n+diy_tmp]);
      auto ret = change_color(dix_tmp, diy_tmp, lastdic_tmp, score, value);
      lastdic_tmp = lastdic[k];
      score = ret.first;
      value = ret.second;
    }

    double prob = exp(((double)value)*pow(0.1, temp));
    if(value >= 0 || prob > (mt()%INF)/(double)INF) {
      best_score = score;
    } else {
      IREP(k, dix.size()) {
        int dix_tmp = dix[k];
        int diy_tmp = diy[k];
        change_color(dix_tmp, diy_tmp, lastdic[k], score, value);
      }
      change_color(x, y, lastc, score, value);
    }
  }

  REP(i, n-2) {
    REP(j, n-2) {
      cout << c[(i+1)*n+j+1] << " ";
    }
    cout << endl;
  }

  cerr << best_score << endl;
}