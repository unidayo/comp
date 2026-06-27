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
constexpr int INF = 1e9;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }
vector<int> dx = {-1, -1, 0, 0, 1, 1};
vector<int> dy = {-1, 0, -1, 1, 0, 1};

double calc_cost(int hand, int cost) {
  return hand + cost / 18.0;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  int N = 30, M = 465;
  vector<vector<int>> sb(N, vector<int>(N, -1));
  REP(i, N) REP(j, i+1) cin >> sb[i][j];

  auto start_time = chrono::system_clock::now();

  //距離行列の計算
  vector<int> itox(M), itoy(M);
  vector<vector<int>> xytoi(N, vector<int>(N));
  vector<vector<double>> bestb(N, vector<double>(N));
  int ptr = 0;
  REP(i, N) REP(j, i+1) {
    itox[ptr] = i;
    itoy[ptr] = j;
    bestb[i][j] = ptr;
    xytoi[i][j] = ptr;
    ptr++;
  }

  int best_score = 1e9;
  vector<tuple<int, int, int, int>> best_ans;
  vector<vector<tuple<int, int, int, int>>> best_ans_per(M);

  double time = 0;
  int loop = 0;

  while(true) {
    auto end_time = chrono::system_clock::now();
    time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    if(time > 1980) break;
    loop++;

    vector<vector<int>> b(sb);
  
    vector<tuple<int, int, int, int>> ans;
    vector<vector<tuple<int, int, int, int>>> ans_per(M);

    vector<vector<int>> state(N, vector<int>(N, 0)); //0:目的地にならない 1:目的地 2:確定済み
    int start_idx = 0;
    if(loop > 1 && time > 900) {
      //途中までは最善手と同じにする部分破壊
      start_idx = rand() % (min(M-1, (int)((M-1) * time / 1900 + 1)));
      REP(i, start_idx) {
        for(auto [x, y, nx, ny] : best_ans_per[i]) {
          ans.push_back(make_tuple(x, y, nx, ny));
          ans_per[i].push_back(make_tuple(x, y, nx, ny));
          swap(b[x][y], b[nx][ny]);
        }
      }
    }
    //stateの設定をする
    REP(i, N) REP(j, i+1) if(b[i][j] < start_idx) state[i][j] = 2;
    REP(i, N) REP(j, i+1) if(state[i][j] != 2) {
      bool ok = true;
      REP(dd, 2) {
        int nnx = i - 1;
        int nny = j - dd;
        if(nnx < 0 || nnx >= N || nny < 0 || nny >= N) continue;
        if(b[nnx][nny] == -1) continue;
        if(state[nnx][nny] != 2) ok = false;
      }
      if(ok) state[i][j] = 1;
    }

    FOR(zz, start_idx, M) {
      int i = zz;

      int x = 0;
      int y = 0;
      REP(j, N) REP(k, N) if(b[j][k] == i) {
        x = j;
        y = k;
        break;
      }

      //コストを考慮したダイクストラ法を行う
      vector<vector<double>> dist(N, vector<double>(N, INF));
      vector<vector<int>> prev_x(N, vector<int>(N, -1));
      vector<vector<int>> prev_y(N, vector<int>(N, -1));
      vector<vector<int>> prev_d(N, vector<int>(N, -1));
      dist[x][y] = 0;
      deque<tuple<double, int, int>> st;
      st.push_back(make_tuple(0, x, y));
      while(!st.empty()) {
        auto [d, x, y] = st.front();
        st.pop_front();
        if(dist[x][y] != d) continue;
        REP(j, 2) {
          int nx = x + dx[j];
          int ny = y + dy[j];
          if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
          if(state[nx][ny] == 2) continue;
          if(b[nx][ny] == -1) continue;
          double cost = abs(bestb[x][y] - b[nx][ny]) - abs(bestb[nx][ny] - b[nx][ny]);
          double val = dist[x][y] + calc_cost(1, cost) + (rand() % 1000 / 8000.0 * (rand() % 2 == 0 ? 1 : -1));
          if(dist[nx][ny] > val) {
            dist[nx][ny] = val;
            prev_x[nx][ny] = x;
            prev_y[nx][ny] = y;
            prev_d[nx][ny] = j;
            st.push_back(make_tuple(dist[nx][ny], nx, ny));
          }
        }
      }

      //最適な候補マスを決定（乱数も含める）
      double min_cost = INF;
      int min_x = -1;
      int min_y = -1;
      REP(j, N) REP(k, j+1) if(state[j][k] == 1) {
        if(dist[j][k] == INF) continue;
        double value = dist[j][k];
        if(value < min_cost) {
          min_cost = value;
          min_x = j;
          min_y = k;
        }
      }


      //経路復元
      vector<tuple<int, int, int, int, int>> route;
      int cur_x = min_x;
      int cur_y = min_y;
      while(true) {
        int nx = prev_x[cur_x][cur_y];
        int ny = prev_y[cur_x][cur_y];
        if(nx == -1) break;
        route.push_back(make_tuple(cur_x, cur_y, nx, ny, prev_d[cur_x][cur_y]));
        cur_x = nx;
        cur_y = ny;
      }

      //操作確定
      reverse(ALL(route));
      REP(k, route.size()) {
        auto [nx, ny, x, y, d] = route[k];
        //swap先がswapすると損になる石である場合、そいつ自身をswapすることで改善できるか考える
        //この処理を再起的にできないか？
        int cost = abs(bestb[x][y] - b[nx][ny]) - abs(bestb[nx][ny] - b[nx][ny]);
        if(k < route.size() - 1) {
          //次の手のswap先でない方をみて、その石とswapすることでcostが改善できるかどうかを考える
          double cur_cost = calc_cost(1, cost);
          int nd = get<4>(route[k+1]);
          nd = (nd + 1) % 2;
          int nnx = nx + dx[nd];
          int nny = ny + dy[nd];
          bool ok = true;
          if(nnx < 0 || nnx >= N || nny < 0 || nny >= N) ok = false;
          if(b[nnx][nny] == -1) ok = false;
          if(state[nnx][nny] == 2) ok = false;
          if(ok) {
            double next_cost = abs(bestb[nnx][nny] - b[nx][ny]) - abs(bestb[nx][ny] - b[nx][ny]);
            next_cost += abs(bestb[x][y] - b[nnx][nny]) - abs(bestb[nnx][nny] - b[nnx][nny]);
            next_cost = calc_cost(2, next_cost);
            if(cur_cost > next_cost) {
              ans.push_back(make_tuple(nx, ny, nnx, nny));
              ans_per[i].push_back(make_tuple(nx, ny, nnx, nny));
              swap(b[nx][ny], b[nnx][nny]);
            }
          }

        }
        ans.push_back(make_tuple(x, y, nx, ny));
        ans_per[i].push_back(make_tuple(x, y, nx, ny));
        swap(b[x][y], b[nx][ny]);
      }

      //新たな候補マスの追加
      state[min_x][min_y] = 2;
      REP(d, 2) {
        int nx = min_x + 1;
        int ny = min_y + d;
        if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
        if(b[nx][ny] == -1) continue;
        if(state[nx][ny] == 0) {
          bool ok = true;
          REP(dd, 2) {
            int nnx = nx - 1;
            int nny = ny - dd;
            if(nnx < 0 || nnx >= N || nny < 0 || nny >= N) continue;
            if(b[nnx][nny] == -1) continue;
            if(state[nnx][nny] != 2) ok = false;
          }
          if(ok) state[nx][ny] = 1;
        }
      }
    }

    bool ok = true;
    REP(j, N-1) REP(k, j+1) REP(d, 2) {
      int nx = j + 1;
      int ny = k + d;
      if(b[j][k] > b[nx][ny]) ok = false;
    }

    // cerr << loop << " " << start_idx << " " << ans.size() << " " << best_score << endl;
    if(ok && best_score > ans.size()) {
      best_score = ans.size();
      best_ans = ans;
      best_ans_per = ans_per;
      bestb = vector<vector<double>>(N, vector<double>(N));
      REP(i, N) REP(j, N) bestb[i][j] = b[i][j];
      REP(z, 2) {
        REP(i, N) REP(j, N) {
          //周りのボールの値を含めて平均値にする
          int factor = 2;
          double sum = bestb[i][j] * factor;
          double cnt = factor;
          REP(d, 6) {
            int nx = i + dx[d];
            int ny = j + dy[d];
            if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if(b[nx][ny] == -1) continue;
            sum += bestb[nx][ny];
            cnt++;
          }
          b[i][j] = sum / cnt;
        }
        REP(i, N) REP(j, N) bestb[i][j] = b[i][j];
      }
    }
  }

  cout << best_ans.size() << "\n";
  REP(i, best_ans.size()) cout << get<0>(best_ans[i]) << " " << get<1>(best_ans[i]) << " " << get<2>(best_ans[i]) << " " << get<3>(best_ans[i]) << "\n";

  cerr << 100000 - 5 * best_score << endl;
}