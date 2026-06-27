#include <string>
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
vector<lint> RH_B = {1532834020, 1388622299};
vector<lint> RH_M = {2147482409, 2147478017};
constexpr int INF = 2147483647;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }

#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;
int EDGE_SIZE = 50;
int LENGTH_LIM_BASE = 300000;
int LENGTH_LIM_FACTOR = 300000;

vector<int> dx = {-1, 0, 1, 0};
vector<int> dy = {0, -1, 0, 1};
vector<int> dx8 = {-1, -1, 0, 1, 1, 1, 0, -1};
vector<int> dy8 = {0, -1, -1, -1, 0, 1, 1, 1};

unsigned long xor128(void)
{
    static unsigned long x=123456789,y=362436069,z=541288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  int N;
  cin >> N;
  vector<int> ax(N), ay(N), bx(N), by(N);
  REP(i, N) cin >> ax[i] >> ay[i];
  REP(i, N) cin >> bx[i] >> by[i];

  auto start_time = std::chrono::system_clock::now();

  //追加削除可能チェックの前計算
  vector<int> check(1<<8, 0);
  REP(bit, 1<<8) {
    vector<bool> check4(4, false);
    REP(d, 4) {
      if((bit >> (d*2)) & 1) check4[d] = true;
    }
    dsu uf(5);
    REP(d, 8) {
      if(d%2 == 0) continue;
      if((bit >> d) & 1) {
        int i1 = d/2;
        int i2 = (i1 + 1) % 4;
        if(check4[i1] && check4[i2]) uf.merge(i1, i2);
      }
    }
    set<int> leaders;
    REP(i, 4) if(check4[i]) leaders.insert(uf.leader(i));
    int comp1 = leaders.size();
    REP(i, 4) if(check4[i]) uf.merge(i, 4);
    set<int> leaders2;
    REP(i, 5) if(i == 4 || check4[i]) leaders2.insert(uf.leader(i));
    int comp2 = leaders2.size();
    if(comp1 != comp2) continue;

    //白マスについても同様に計算する
    REP(d, 4) check4[d] = !check4[d];
    uf = dsu(5);
    REP(d, 8) {
      if(d%2 == 0) continue;
      if(!((bit >> d) & 1)) {
        int i1 = d/2;
        int i2 = (i1 + 1) % 4;
        if(check4[i1] && check4[i2]) uf.merge(i1, i2);
      }
    }
    leaders.clear();
    REP(i, 4) if(check4[i]) leaders.insert(uf.leader(i));
    comp1 = leaders.size();
    REP(i, 4) if(check4[i]) uf.merge(i, 4);
    leaders2.clear();
    REP(i, 5) if(i == 4 || check4[i]) leaders2.insert(uf.leader(i));
    comp2 = leaders2.size();
    if(comp1 != comp2) continue;
    check[bit] = 1;
  }


  set<int> x_nopoint, y_nopoint;
  REP(i, 100001) {
    x_nopoint.insert(i);
    y_nopoint.insert(i);
  }
  REP(i, N) {
    x_nopoint.erase(ax[i]);
    y_nopoint.erase(ay[i]);
    x_nopoint.erase(bx[i]);
    y_nopoint.erase(by[i]);
  }
  //魚のいない座標で区切った時、各領域に何匹の魚がいるか
  //縦・横に集計する
  //（最後の要素は使わない）
  set<tuple<int, int, int>> x_edge, y_edge;
  for(int x : x_nopoint) {
    x_edge.insert({x, 0, 0});
  }
  for(int y : y_nopoint) {
    y_edge.insert({y, 0, 0});
  }
  REP(i, N) {
    auto it = x_edge.lower_bound({ax[i], 0, 0});
    if(it == x_edge.begin()) continue;
    --it;
    auto tup = *it;
    x_edge.erase(it);
    x_edge.insert(make_tuple(get<0>(tup), get<1>(tup)+1, get<2>(tup)+1));
  }
  REP(i, N) {
    auto it = x_edge.lower_bound({bx[i], 0, 0});
    if(it == x_edge.begin()) continue;
    --it;
    auto tup = *it;
    x_edge.erase(it);
    x_edge.insert(make_tuple(get<0>(tup), get<1>(tup)+1, get<2>(tup)-1));
  }
  REP(i, N) {
    auto it = y_edge.lower_bound({ay[i], 0, 0});
    if(it == y_edge.begin()) continue;
    --it;
    auto tup = *it;
    y_edge.erase(it);
    y_edge.insert(make_tuple(get<0>(tup), get<1>(tup)+1, get<2>(tup)+1));
  }
  REP(i, N) {
    auto it = y_edge.lower_bound({by[i], 0, 0});
    if(it == y_edge.begin()) continue;
    --it;
    auto tup = *it;
    y_edge.erase(it);
    y_edge.insert(make_tuple(get<0>(tup), get<1>(tup)+1, get<2>(tup)-1));
  }
  set<tuple<int, int, int>> x_edge2, y_edge2;
  for(auto p : x_edge) {
    if(get<1>(p) > 0) x_edge2.insert(p);
  }
  for(auto p : y_edge) {
    if(get<1>(p) > 0) y_edge2.insert(p);
  }
  x_edge = x_edge2;
  y_edge = y_edge2;
  vector<tuple<int, int, int>> x_edge_v(ALL(x_edge)), y_edge_v(ALL(y_edge));

  //サイズが小さい隣接領域をマージしていく
  set<P> sz_x, sz_y;
  REP(i, x_edge_v.size()-1) {
    sz_x.insert({get<1>(x_edge_v[i]) + get<1>(x_edge_v[i+1]), get<0>(x_edge_v[i])});
  }
  while(x_edge.size() > EDGE_SIZE) {
    int x1 = sz_x.begin()->second;
    auto it1 = x_edge.lower_bound({x1, 0, 0});
    auto it2 = x_edge.lower_bound({x1, 0, 0});
    it2++;
    int x2 = get<0>(*it2);
    int sum1 = get<1>(*it1) + get<1>(*it2);
    int sum2 = get<2>(*it1) + get<2>(*it2);
    tuple<int, int, int> next = {x1, sum1, sum2};
    auto it0 = x_edge.lower_bound({x1, 0, 0});
    if(it0 != x_edge.begin()) {
      it0--;
      P left = {get<1>(*it0) + sum1, get<0>(*it0)};
      sz_x.erase(sz_x.find({get<1>(*it0) + get<1>(*it1), get<0>(*it0)}));
      sz_x.insert(left);
    }
    auto it3 = x_edge.lower_bound({x2, 0, 0});
    it3++;
    if(it3 != x_edge.end()) {
      P right = {get<1>(*it3) + sum1, get<0>(*it1)};
      sz_x.erase(sz_x.find({get<1>(*it2) + get<1>(*it3), get<0>(*it2)}));
      sz_x.insert(right);
    }
    sz_x.erase(sz_x.find({sum1, x1}));
    x_edge.erase(it1);
    x_edge.erase(it2);
    x_edge.insert(next);
  }

  REP(i, y_edge_v.size()-1) {
    sz_y.insert({get<1>(y_edge_v[i]) + get<1>(y_edge_v[i+1]), get<0>(y_edge_v[i])});
  }
  while(y_edge.size() > EDGE_SIZE) {
    int y1 = sz_y.begin()->second;
    auto it1 = y_edge.lower_bound({y1, 0, 0});
    auto it2 = y_edge.lower_bound({y1, 0, 0});
    it2++;
    int y2 = get<0>(*it2);
    int sum1 = get<1>(*it1) + get<1>(*it2);
    int sum2 = get<2>(*it1) + get<2>(*it2);
    tuple<int, int, int> next = {y1, sum1, sum2};
    auto it0 = y_edge.lower_bound({y1, 0, 0});
    if(it0 != y_edge.begin()) {
      it0--;
      P left = {get<1>(*it0) + sum1, get<0>(*it0)};
      sz_y.erase(sz_y.find({get<1>(*it0) + get<1>(*it1), get<0>(*it0)}));
      sz_y.insert(left);
    }
    auto it3 = y_edge.lower_bound({y2, 0, 0});
    it3++;
    if(it3 != y_edge.end()) {
      P right = {get<1>(*it3) + sum1, get<0>(*it1)};
      sz_y.erase(sz_y.find({get<1>(*it2) + get<1>(*it3), get<0>(*it2)}));
      sz_y.insert(right);
    }
    sz_y.erase(sz_y.find({sum1, y1}));
    y_edge.erase(it1);
    y_edge.erase(it2);
    y_edge.insert(next);
  }

  int xmin = INF, xmax = -INF, ymin = INF, ymax = -INF;
  REP(i, N) {
    chmin(xmin, ax[i]);
    chmax(xmax, ax[i]);
    chmin(ymin, ay[i]);
    chmax(ymax, ay[i]);
    chmin(xmin, bx[i]);
    chmax(xmax, bx[i]);
    chmin(ymin, by[i]);
    chmax(ymax, by[i]);
  }

  x_edge_v = vector<tuple<int, int, int>>(ALL(x_edge));
  y_edge_v = vector<tuple<int, int, int>>(ALL(y_edge));
  //番兵を追加
  x_edge_v.push_back({xmax+1, 0, 0});
  x_edge_v.insert(x_edge_v.begin(), {xmin-1, 0, 0});
  y_edge_v.push_back({xmax+1, 0, 0});
  y_edge_v.insert(y_edge_v.begin(), {xmin-1, 0, 0});
  sort(ALL(x_edge_v));
  sort(ALL(y_edge_v));

  int V = x_edge_v.size();

  vector<vector<int>> grid_score(V, vector<int>(V, 0));
  REP(i, N) {
    auto xit = x_edge.lower_bound({ax[i], 0, 0});
    if(xit == x_edge.begin()) continue;
    //xit--;
    auto yit = y_edge.lower_bound({ay[i], 0, 0});
    if(yit == y_edge.begin()) continue;
    //yit--;
    int xidx = distance(x_edge.begin(), xit);
    int yidx = distance(y_edge.begin(), yit);
    grid_score[xidx][yidx]++;
  }
  REP(i, N) {
    auto xit = x_edge.lower_bound({bx[i], 0, 0});
    if(xit == x_edge.begin()) continue;
    //xit--;
    auto yit = y_edge.lower_bound({by[i], 0, 0});
    if(yit == y_edge.begin()) continue;
    //yit--;
    int xidx = distance(x_edge.begin(), xit);
    int yidx = distance(y_edge.begin(), yit);
    grid_score[xidx][yidx]--;
  }

  //100*100領域について多角形に含むか含まないかを決定する焼きなまし
  //中心の領域から始めて、連結している状態を維持しつつ領域を追加・削除する
  vector<int> grid_xlen(V, 0), grid_ylen(V, 0);
  FOR(i, 1, V-1) grid_xlen[i] = get<0>(x_edge_v[i+1]) - get<0>(x_edge_v[i]);
  FOR(i, 1, V-1) grid_ylen[i] = get<0>(y_edge_v[i+1]) - get<0>(y_edge_v[i]);

  vector<vector<bool>> grid(V, vector<bool>(V, false));
  int best_score = 0;
  int best_length = 0;
  int xl = 2, xr = V-2, yl = 2, yr = V-2;
  FOR(i, xl, xr+1) {
    FOR(j, yl, yr+1) {
      grid[i][j] = true;
      best_score += grid_score[i][j];
      if(i == xl || i == xr) best_length += grid_ylen[j];
      if(j == yl || j == yr) best_length += grid_xlen[i];
    }
  }
  double LENGTH_FACTOR_BASE = 0;
  double best_value = best_score - best_length * LENGTH_FACTOR_BASE;

  /*
  //領域テスト
  int score = 0;
  FOR(i, 1, V-1) {
    FOR(j, 1+abs(V/2-i), V-1-abs(V/2-i)) {
      grid[i][j] = true;
      score += grid_score[i][j];
    }
  }

  REP(i, V) {
    REP(j, V) {
      if(grid[i][j]) cerr << "#";
      else cerr << ".";
    }
    cerr << "\n";
  }
  */

  //多角形の周囲の長さを400000以下に抑えるため、周囲の長さを持ちつつ超えないように焼きなましをする

  int TIME_LIMIT = 1920;
  double tim = 0;
  int loop = 0;
  int last_time = 0;
  int loop_interval = 1000;
  int next_check = 0;
  double start_temp = 1.0, end_temp = -0.5;
  double temp = start_temp;
  double LENGTH_FACTOR = LENGTH_FACTOR_BASE;


  while(true) {
    if(loop == next_check) {
      auto now_time = std::chrono::system_clock::now();
      tim = std::chrono::duration_cast<std::chrono::milliseconds>(now_time-start_time).count();
      //cerr << "LOOP:" << loop << " SCORE:" << best_score << " VALUE:" << best_value << " LENGTH:" << best_length << " TIME:" << tim << endl;
      if(tim > TIME_LIMIT) break;
      if(tim - last_time < 10) loop_interval *= 2;
      else if(tim - last_time > 20 && loop_interval > 1) loop_interval /= 2;
      last_time = tim;
      next_check += loop_interval;
      temp = start_temp + (end_temp - start_temp) * tim / TIME_LIMIT;
      LENGTH_FACTOR = LENGTH_FACTOR_BASE * pow(0.01, tim / TIME_LIMIT);
      //if(loop > 0) break;
    }
    loop++;
    int length;
    int x, y;
    int LENGTH_LIM = max(400000.0, LENGTH_LIM_BASE + LENGTH_LIM_FACTOR * (1.0 - tim / TIME_LIMIT));
    if(best_length > LENGTH_LIM) best_value -= (best_length - LENGTH_LIM) * 10000;
    while(true) {
      length = best_length;
      x = xor128() % (V-2) + 1;
      y = xor128() % (V-2) + 1;
      //追加削除可能チェック
      //8方向を見て連結成分の数に変化が無ければOK
      int bit = 0;
      REP(d, 8) {
        int nx = x + dx8[d];
        int ny = y + dy8[d];
        if(grid[nx][ny]) bit += 1 << d;
      }
      if(!check[bit]) continue;

      //長さチェックをする
      int bit4 = 0;
      REP(d, 4) {
        bit4 += (bit >> (d*2) & 1) << d;
      }
      if(bit4 == 1 || bit4 == 4) {
        //x方向1マス
        length += 2*grid_xlen[x] * (grid[x][y] ? -1 : 1);
      } else if(bit4 == 2 || bit4 == 8) {
        //y方向1マス
        length += 2*grid_ylen[y] * (grid[x][y] ? -1 : 1);
      } else if(bit4 == 5 || bit4 == 10) {
        //x方向2マス or y方向2マスはあり得ない
        cerr << "ERROR" << endl;
      } else if(bit4 == 3 || bit4 == 6 || bit4 == 9 || bit4 == 12) {
        //x方向1マス、y方向1マス
        //変化なし
      } else if(bit4 == 7 || bit4 == 13) {
        //x方向2マス、y方向1マス
        length -= 2*grid_ylen[y] * (grid[x][y] ? -1 : 1);
      } else if(bit4 == 11 || bit4 == 14) {
        //x方向1マス、y方向2マス
        length -= 2*grid_xlen[x] * (grid[x][y] ? -1 : 1);
      } else {
        //全方向はあり得ない
        cerr << "ERROR" << endl;
      }
      if(length <= best_length || length <= LENGTH_LIM) break;
    }

    int score = best_score;
    if(grid[x][y]) score -= grid_score[x][y];
    else score += grid_score[x][y];
    double value = score - length * LENGTH_FACTOR;
    if(length > LENGTH_LIM) value -= (length - LENGTH_LIM) * 10000;
    double diff = value - best_value;

    double prob = exp(((double)diff)*pow(0.1, temp));
    if(diff >= 0 || prob*(double)INF > (xor128()%INF)) {
      best_score = score;
      best_value = value;
      best_length = length;
      grid[x][y] = !grid[x][y];
    }

  }


  //領域の出力
  //領域を構成する辺を全て出力する
  //最初に領域の端を見つけて、そこから順に辺をたどる
  int sx = -1;
  int sy = -1;
  int d = 0;
  REP(i, V-1) {
    REP(j, V) {
      if(!grid[i][j] && grid[i+1][j]) {
        sx = i+1;
        sy = j;
        d = 3;
        break;
      }
    }
    if(sx != -1) break;
  }

  int x = sx;
  int y = sy;
  vector<P> ans;
  ans.push_back({x, y});
  x += dx[d];
  y += dy[d];
  while(x != sx || y != sy) {
    bool t1, t2;
    if(d == 0) {
      t1 = grid[x-1][y-1];
      t2 = grid[x-1][y];
      if(t1 && t2) {
        if(grid[x][y]) d = 1;
        else d = 3;
        ans.push_back({x, y});
      } else if(!t1 && !t2) {
        if(grid[x][y]) d = 3;
        else d = 1;
        ans.push_back({x, y});
      }
    } else if(d == 2) {
      t1 = grid[x][y-1];
      t2 = grid[x][y];
      if(t1 && t2) {
        if(grid[x-1][y]) d = 1;
        else d = 3;
        ans.push_back({x, y});
      } else if(!t1 && !t2) {
        if(grid[x-1][y]) d = 3;
        else d = 1;
        ans.push_back({x, y});
      }
    } else if(d == 1) {
      t1 = grid[x-1][y-1];
      t2 = grid[x][y-1];
      if(t1 && t2) {
        if(grid[x][y]) d = 0;
        else d = 2;
        ans.push_back({x, y});
      } else if(!t1 && !t2) {
        if(grid[x][y]) d = 2;
        else d = 0;
        ans.push_back({x, y});
      }
    } else {
      t1 = grid[x-1][y];
      t2 = grid[x][y];
      if(t1 && t2) {
        if(grid[x][y-1]) d = 0;
        else d = 2;
        ans.push_back({x, y});
      } else if(!t1 && !t2) {
        if(grid[x][y-1]) d = 2;
        else d = 0;
        ans.push_back({x, y});
      }
    }
    x += dx[d];
    y += dy[d];
  }
  cout << ans.size() << "\n";
  for(auto p : ans) {
    cout << get<0>(x_edge_v[p.first]) << " " << get<0>(y_edge_v[p.second]) << "\n";
  }

  cerr << best_score + 1 << endl;

}