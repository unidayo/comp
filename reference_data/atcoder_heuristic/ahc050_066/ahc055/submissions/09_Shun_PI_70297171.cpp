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

constexpr int TIME_LIMIT = 1940;
constexpr int A_THRESHOLD = 10;

mt19937_64 mt(0);
int N;
vector<int> H_init, C;
vector<vector<int>> A;

#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;


int vst_id = 0;
vector<int> vst;
bool check_loop(int u, int target, vector<vector<int>> &nbr) {
  if(u == target) return true;
  vst[u] = vst_id;
  for(auto v : nbr[u]) if(v != -1 && vst[v] != vst_id) {
    if(check_loop(v, target, nbr)) return true;
  }
  return false;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N;
  H_init.resize(N);
  C.resize(N);
  A.resize(N, vector<int>(N));
  REP(i, N) cin >> H_init[i];
  REP(i, N) cin >> C[i];
  REP(i, N) REP(j, N) cin >> A[i][j];
  int hard_sum = 0;
  REP(i, N) hard_sum += H_init[i];
  vst.resize(N, -1);

  vector<vector<int>> weapon_search_idx(N);
  vector<vector<int>> weapon_search_idx_below(N);
  REP(i, N) REP(j, N) {
    if(A[i][j] > A_THRESHOLD && i != j) {
      weapon_search_idx[i].push_back(j);
    } else if(i != j) {
      weapon_search_idx_below[i].push_back(j);
    }
    sort(ALL(weapon_search_idx[i]), [&](int x, int y) {
      return A[i][x] > A[i][y];
    });
    sort(ALL(weapon_search_idx_below[i]), [&](int x, int y) {
      return A[i][x] > A[i][y];
    });
  }

  //とりあえず貪欲
  //とった武器を一番有効に使える場所で予約してしまう戦法
  auto start_time = chrono::system_clock::now();
  vector<vector<int>> weapon_idx_list(N);
  vector<bool> opened(N, false);
  vector<int> H(H_init);
  int turn_sum = 0;
  vector<int> p(N);
  REP(turn, N) {
    int min_turn = INF;
    int bidx = -1;
    REP(i, N) {
      if(opened[i]) continue;
      int total_turn = max(0, H[i]) + weapon_idx_list[i].size();
      if(total_turn < min_turn) {
        min_turn = total_turn;
        bidx = i;
      }
    }

    opened[bidx] = true;
    p[turn] = bidx;
    turn_sum += min_turn;

    //武器を最も有効に使える場所に割り振る
    REP(k, C[bidx]) {
      int max_effect = 0;
      int target_idx = -1;
      REP(i, N) {
        if(opened[i]) continue;
        int damage = min(H[i], A[bidx][i]);
        if(damage > max_effect) {
          max_effect = damage;
          target_idx = i;
        }
      }
      if(target_idx == -1) break;
      weapon_idx_list[target_idx].push_back(bidx);
      H[target_idx] -= A[bidx][target_idx];
    }
  }
  cerr << "Initial greedy turns: " << turn_sum << "\n";

  //有向グラフ（DAG）を焼く
  //各頂点からはC[i]本の辺が出る
  //閉路ができていなければ必ずそれを満たす順序が構築できる！
  vector<vector<int>> nbr(N);
  REP(i, N) {
    for(int j : weapon_idx_list[i]) {
      nbr[j].push_back(i);
    }
  }
  REP(i, N) {
    assert(nbr[i].size() <= C[i]);
    while(nbr[i].size() < C[i]) {
      nbr[i].push_back(-1);
    }
  }

  //順序を焼く
  int best_score = 0;
  REP(i, N) {
    best_score += max(0, H[i]);
    for(auto v : nbr[i]) {
      if(v != -1) best_score++;
    }
  }
  int loop = 0;
  float start_temp = 1.0, end_temp = 0.0;
  float temp = start_temp;
  float time;
  int best_score_2 = best_score;
  vector<vector<int>> best_nbr = nbr;

  while(true) {
    if(loop%10000 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      cerr << loop << " " << best_score << "\n";
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT;
    }
    loop++;

    int score = best_score;

    //TODO: 2-optを実装する
    //Aが大きいものほど選択されやすくする

    //ランダムに辺を選び接続しなおすか、削除する
    //この時閉路ができてはいけない
    int i = mt() % N;
    int j = mt() % nbr[i].size();
    int old_target = nbr[i][j];
    if(weapon_search_idx.size() == 0) continue;
    if(weapon_search_idx[i].size() <= 1 && nbr[i][j] != -1) continue;
    if(nbr[i][j] != -1) {
      //削除
      score -= max(0, H[nbr[i][j]]);
      score--; //武器を使用しない分
      H[nbr[i][j]] += A[i][nbr[i][j]];
      score += max(0, H[nbr[i][j]]);
      nbr[i][j] = -1;
    }

    //新たに繋ぎなおし、閉路チェックをDFSで行う
    int new_target = old_target;
    while(new_target == old_target) {
      int rnd_idx = mt() % weapon_search_idx[i].size();
      new_target = weapon_search_idx[i][rnd_idx];
    }
    nbr[i][j] = new_target;
    //閉路チェック
    vst_id++;
    if(check_loop(new_target, i, nbr)) score += 1e9;
    score -= max(0, H[nbr[i][j]]);
    H[nbr[i][j]] -= A[i][nbr[i][j]];
    score += max(0, H[nbr[i][j]]);
    score++; //武器を使用する分

    int diff = best_score - score;
    float prob = exp(diff*pow(0.1, temp));
    if(diff >= 0 || prob*(float)INF > (mt()%INF)) {
      best_score = score;
      if(best_score < best_score_2) {
        best_score_2 = best_score;
        best_nbr = vector<vector<int>>(nbr);
      }
    } else {
      if(nbr[i][j] != -1) {
        H[nbr[i][j]] += A[i][nbr[i][j]];
      }
      nbr[i][j] = old_target;
      if(nbr[i][j] != -1) {
        H[nbr[i][j]] -= A[i][nbr[i][j]];
      }
    }
  }

  //DAGを求める
  scc_graph g(N);
  REP(i, N) {
    for(auto v : best_nbr[i]) {
      if(v != -1) g.add_edge(i, v);
    }
  }
  vector<vector<int>> dag = g.scc();
  assert(dag.size() == N);

  weapon_idx_list = vector<vector<int>>(N);
  REP(i, N) {
    for(auto v : best_nbr[i]) {
      if(v != -1) {
        weapon_idx_list[v].push_back(i);
      }
    }
  }

  vector<P> best_ans;
  H = vector<int>(H_init);
  REP(turn, N) {
    int bidx = dag[turn][0];
    for(auto widx : weapon_idx_list[bidx]) {
      if(H[bidx] <= 0) break;
      best_ans.emplace_back(widx, bidx);
      H[bidx] -= A[widx][bidx];
    }
    REP(_, max(0, H[bidx])) {
      best_ans.emplace_back(-1, bidx);
    }
  }

  for(auto p : best_ans) {
    cout << p.first << " " << p.second << "\n";
  }

  cerr << "time: " << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count() << " ms\n";
  cerr << hard_sum - best_score_2 + 1 << endl;

  quick_exit(0);
}