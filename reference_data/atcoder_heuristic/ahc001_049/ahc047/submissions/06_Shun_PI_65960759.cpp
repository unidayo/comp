#include <string>
#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;
using lint = long long int;
using PA = pair<int, int>;
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
int N, M, L;
vector<pair<string, int>> SP;
vector<double> base;
mt19937 mt(chrono::steady_clock::now().time_since_epoch().count());

#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;

constexpr int TOP_STRING = 4;
constexpr int TIME_LIMIT = 500;
constexpr int TIME_LIMIT_2 = 1940;
constexpr int ANS_POOL = 100;

unsigned long xor128(void)
{
    static unsigned long x=123456789,y=362436069,z=541288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

float calc_value(const vector<vector<int>> &state)
{
  vector<vector<double>> cnt(M, vector<double>(M));
  REP(k, TOP_STRING) {
    REP(i, SP[k].first.size() - 1) {
      int a = state[k][i];
      int b = state[k][i + 1];
      cnt[a][b] += base[SP[k].first.size()];
    }
  }
  float entropy = 0;
  REP(i, M) {
    double sum = 0;
    REP(j, M) sum += cnt[i][j];
    if (sum == 0) continue;
    REP(j, M) {
      if (cnt[i][j] == 0 || cnt[i][j] == sum) continue;
      float p = (float)cnt[i][j] / sum;
      entropy += cnt[i][j] * (1 - p);
    }
  }
  return entropy;
}

vector<vector<int>> calc_weight(const vector<vector<int>> &state)
{
  vector<vector<int>> ans(M, vector<int>(M));
  vector<vector<double>> cnt(M, vector<double>(M));
  REP(k, TOP_STRING) {
    REP(i, SP[k].first.size() - 1) {
      int a = state[k][i];
      int b = state[k][i + 1];
      cnt[a][b] += base[SP[k].first.size()];
    }
  }
  REP(i, M) {
    double sum = 0;
    REP(j, M) sum += cnt[i][j];
    int rest = 100;
    REP(j, M) {
      if(sum == 0) break;
      ans[i][j] = cnt[i][j] * ANS_POOL / sum; // paramater
      rest -= ans[i][j];
    }
    //残りは各状態のスタート状態に割り振る
    bool all = rest == 100;
    while(rest > 0) {
      REP(j, M) if(all || ans[i][j] > 0) {
        ans[i][j] += 1;
        rest--;
        if(rest == 0) break;
      }
    }
  }

  return ans;
}

//転置行列を求める
vector<vector<float>> trans_mat(vector<vector<float>> a) {
  vector<vector<float>> ret(a[0].size(), vector<float>(a.size()));
  REP(i, a.size()) REP(j, a[0].size()) ret[j][i] = a[i][j];
  return ret;
}

//逆行列を求める
bool inv_mat(vector<vector<float>> a, vector<vector<float>> &inv) {
  assert(a.size() == a[0].size() && inv.size() == inv[0].size());
  int n = a.size();
  REP(i, n) REP(j, n) inv[i][j] = (i == j ? 1.0 : 0.0);
  REP(i, n) {
    float ma = 0.0;
    int pivot;
    FOR(j, i, n) {
      if(a[j][i] > ma) {
        ma = a[j][i];
        pivot = j;
      }
    }
    if(ma == 0.0) return false;
    if(pivot != i) {
      REP(j, n) {
        swap(a[i][j], a[pivot][j]);
        swap(inv[i][j], inv[pivot][j]);
      }
    }
    float tmp = 1.0 / a[i][i];
    REP(j, n) {
      a[i][j] *= tmp;
      inv[i][j] *= tmp;
    }
    REP(j, n) {
      if(i != j) {
        float tmp2 = a[j][i];
        REP(k, n) {
          a[j][k] -= a[i][k] * tmp2;
          inv[j][k] -= inv[i][k] * tmp2;
        }
      }
    }
  }
  return true;
}

vector<vector<float>> mxm(vector<vector<float>> a, vector<vector<float>> b) {
  vector<vector<float>> ret(a.size(), vector<float>(a.size()));
  REP(i, a.size()) REP(j, a.size()) REP(k, a[0].size()) ret[i][j] += a[i][k]*b[k][j];
  return ret;
}
vector<float> mxv(vector<vector<float>> a, vector<float> b) {
  vector<float> ret(a.size());
  REP(i, a.size()) REP(j, b.size()) ret[i] += a[i][j]*b[j];
  return ret;
}

float calc_weight_score(const vector<vector<int>> &weight, bool prune = true)
{
  //定常状態における各状態にいる確率を求める
  vector<vector<float>> mat(M, vector<float>(M));
  REP(i, M) REP(j, M) mat[i][j] = (float)weight[i][j] / 100.0;
  //定常分布を連立方程式を解いて求める
  vector<vector<float>> A(M, vector<float>(M));
  REP(i, M) REP(j, M) A[i][j] = mat[j][i];
  REP(i, M) A[i][i] -= 1.0;
  REP(i, M) A[M-1][i] = 1.0;
  vector<vector<float>> inv(M, vector<float>(M));
  if(!inv_mat(A, inv)) {
    cerr << "inv_mat failed" << endl;
    return 0;
  }
  vector<float> b(M);
  b[M-1] = 1.0;
  vector<float> pi = mxv(inv, b);
  float pisum = 0;
  REP(i, M) pisum += pi[i];
  REP(i, M) pi[i] /= pisum;

  float score = 0;
  REP(k, N) {
    int l = SP[k].first.size();
    //各ビットを適宜枝刈りすることで高速化できる
    vector<pair<float, int>> probs(2);
    REP(i, 2) {
      int s = i + (SP[k].first[0]-'a') * 2;
      probs[i] = {pi[s], s};
    }
    REP(t, l-1) {
      vector<pair<float, int>> next_probs(2);
      REP(i, 2) next_probs[i] = {0, i + (SP[k].first[t+1]-'a') * 2};
      for(auto pa : probs) {
        float p = pa.first;
        int s = pa.second;
        REP(i, 2) {
          int n = i + (SP[k].first[t+1]-'a') * 2;
          float next_p = p * mat[s][n];
          //if (prune && next_p < 1e-10) continue;
          next_probs[i].first += next_p;
        }
      }
      probs = next_probs;
    }
    float psum = 0;
    for(auto pa : probs) psum += pa.first;
    if(psum == 0) continue;
    score += (1.0 - pow(1.0-psum, L)) * SP[k].second;
    //if(psum > 0) cout << psum << " " << score << endl;
  }
  return score;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> M >> L;
  SP.resize(N);
  REP(i, N) cin >> SP[i].first >> SP[i].second;
  sort(ALL(SP), [](const auto &a, const auto &b) {
    return a.second > b.second;
  });
  base = vector<double>(M+1);
  FOR(i, 1, M+1) base[i] = 1.0 / pow(L, 1.0/i);
  FOR(i, 1, M+1) cerr << base[i] << " ";
  cerr << "\n";

  auto start_time = chrono::system_clock::now();

  vector<vector<int>> move_cnt(6, vector<int>(6));
  REP(i, 6) {
    REP(j, SP[i].first.size() - 1) {
      int a = SP[i].first[j] - 'a';
      int b = SP[i].first[j + 1] - 'a';
      move_cnt[a][b]++;
    }
  }
  REP(i, 6) {
    REP(j, 6) cerr << move_cnt[i][j] << " ";
    cerr << "\n";
  }

  //a~fを2個ずつ持つ
  //各文字について、2個のうちどちらを割り当てるかを決める
  //そうすると各状態の遷移先として必要な状態の数が決まる
  //この数がなるべく小さい（＝均等）と良さそう
  vector<vector<int>> state(N, vector<int>(M));
  REP(i, N) REP(j, SP[i].first.size()) {
    state[i][j] = (SP[i].first[j] - 'a') * 2 + xor128() % 2;
  }
  //エントロピーがなるべく小さくなるような状態にする

  float best_score = calc_value(state);
  int loop = 0;
  float start_temp = 2.5, end_temp = 1.0;
  float temp = start_temp;
  float time;

  while(true) {
    if(loop%3000 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      cerr << loop << " " << best_score << "\n";
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT;
    }
    loop++;

    //ランダムに文字を選び、その文字の状態を変化する
    int str_idx = mt() % TOP_STRING;
    int swap_idx_1 = mt() % SP[str_idx].first.size();
    state[str_idx][swap_idx_1] = state[str_idx][swap_idx_1] / 2 * 2 + (state[str_idx][swap_idx_1] + 1) % 2;
    vector<vector<int>> weight = calc_weight(state);
    float score = calc_weight_score(weight);
    float diff = score - best_score;
    float prob = exp(diff*pow(0.1, temp));
    if(diff >= 0 || prob*(float)INF > (xor128()%INF)) {
      best_score = score;
    } else {
      state[str_idx][swap_idx_1] = state[str_idx][swap_idx_1] / 2 * 2 + (state[str_idx][swap_idx_1] + 1) % 2;
    }
  }

  //初期解を元に、重みを焼きなます
  //評価は、まず固有ベクトルを求めて定常状態における各状態にいる確率を求める
  //次に、その状態から各文字列が成立する確率を求めてスコアを計算する
  vector<vector<int>> weight = calc_weight(state);
  best_score = calc_weight_score(weight);
  loop = 0;
  start_temp = 0.5;
  end_temp = 0.0;
  temp = start_temp;
  double best_score_2 = best_score;
  vector<vector<int>> best_weight(weight);

  while(true) {
    if(loop%3000 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT_2) break;
      cerr << loop << " " << best_score << "\n";
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT_2;
      //kick
      //ある行をランダムに初期化
      if(loop%3000 == 0) {
        if(xor128() % 8 == 0 && best_score_2 > best_score) {
          best_score = best_score_2;
          weight = vector<vector<int>>(best_weight);
        }
        int row = xor128() % M;
        REP(i, M) weight[row][i] = 0;
        int rest = 100;
        while(rest > 0) {
          int n = xor128() % M;
          int a = row / 2;
          int b = n / 2;
          if(move_cnt[a][b] == 0) continue;
          weight[row][n] += 1;
          rest--;
        }
        best_score = calc_weight_score(weight);
      }
    }
    loop++;

    //重みの変化を遷移とし、スコアの期待値を評価値とする
    int row = xor128() % M;
    int col1, col2, val;
    while(true) {
      col1 = xor128() % M;
      col2 = xor128() % M;
      val = xor128() % 5 + 1;
      if(col1 != col2 && weight[row][col1] >= val) break;
    }
    weight[row][col1] -= val;
    weight[row][col2] += val;
    float score = calc_weight_score(weight);
    float diff = score - best_score;
    float prob = exp(diff*pow(0.1, temp));
    if(diff >= 0 || prob*(float)INF > (xor128()%INF)) {
      best_score = score;
      if (best_score > best_score_2) {
        //連結チェック
        dsu uf(M);
        REP(i, M) {
          REP(j, M) {
            if (weight[i][j] > 0) uf.merge(i, j);
          }
        }
        bool ok = uf.size(0) == M;
        if (!ok) {
          cerr << "not connected" << endl;
          continue;
        }
        best_weight = weight;
        best_score_2 = best_score;
      }
    } else {
      weight[row][col1] += val;
      weight[row][col2] -= val;
    }
  }


  REP(i, M) {
    cout << (char)('a' + i/2) << " ";
    REP(j, M) {
      cout << best_weight[i][j] << " ";
    }
    cout << "\n";
  }
  best_score = calc_weight_score(best_weight, false);
  cerr << (int)best_score << "\n";
}