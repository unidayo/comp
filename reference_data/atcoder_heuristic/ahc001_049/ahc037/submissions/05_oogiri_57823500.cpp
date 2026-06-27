#include <bits/stdc++.h>
using namespace std;
using ll = long long;
#define rep(i, s, n) for (int i = (s); i < (int)(n); i++)
#define all(v) v.begin(), v.end()

#define MODE 1 // 0ならローカル

/* ---------- 焼きなましゾーン ---------- */
// 定数
constexpr int END_TIME = 1985; // 処理を打ち切る時間
const auto START_TIME = chrono::system_clock::now(); // 開始時刻
constexpr double start_temp = 110, end_temp = 0; // 最大点数変化、最小点数変化

// XorShift128
unsigned int randxor() {
    static unsigned int x=123456789, y=362436069, z=521288629, w=88675123;
    unsigned int t;
    t=(x^(x<<11)); x=y; y=z; z=w;
    return(w=(w^(w>>19))^(t^(t>>8)));
}

// 経過時間取得関数
int getTime() {
    auto end_time = chrono::system_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(end_time-START_TIME).count();
}

// 温度関数
double get_temp() {
  return start_temp + (end_temp-start_temp) * getTime() / END_TIME; // 一定に温度を下げたいとき用
  //return start_temp*pow(end_temp/(double)start_temp, getTime()/(double)END_TIME); // 山登り結構いけるとき用
}

// 遷移確率
double prob(double new_score, double pre_score) {
  double temp = get_temp();
  return exp((new_score-pre_score)/temp);
}
/* ---------- 焼きなましゾーンここまで ---------- */

int culcCost(int x1, int y1, int x2, int y2) {
  return x2-x1+y2-y1;
}

int main() {
  int N;
  cin >> N;
  int L = 0;
  vector<pair<int,int>> AB(N+1);
  AB[0] = {0, 0};
  rep(i, 1, N+1) {
    int A, B;
    cin >> A >> B;
    AB[i] = {A, B};
    L = max({L, A, B});
  }
  ll cost = 0;

  vector<vector<int>> ans;
  vector<pair<int,int>> allV(N+1);
  rep(i, 0, N+1) allV[i] = AB[i];
  rep(_, 0, 40) {
  // 根付き木を構築
  vector<int> par(allV.size());
  vector<int> costs(allV.size());
  par[0] = -1;
  rep(i, 1, allV.size()) {
    int mini = 2000000000, minInd = 0;
    rep(j, 0, allV.size()) {
      if (i == j) continue;
      if (allV[j].first > allV[i].first || allV[j].second > allV[i].second) continue;
      int tmp = culcCost(allV[j].first, allV[j].second, allV[i].first, allV[i].second);
      if (tmp < mini) {
        mini = tmp;
        minInd = j;
      }
    }
    par[i] = minInd;
    costs[i] = mini;
  }
  // 追加すべき頂点を決定
  vector<pair<ll,pair<int,int>>> newV;
  rep(i, 1, allV.size()) {
    rep(j, i+1, allV.size()) {
      if ((allV[i].first > allV[j].first)^(allV[i].second > allV[j].second)) {
        pair<int,int> loc = {min(allV[i].first, allV[j].first), min(allV[i].second, allV[j].second)};
        // 少なくとも両方の親となることを確認allV
        int c1 = culcCost(loc.first, loc.second, allV[i].first, allV[i].second);
        int c2 = culcCost(loc.first, loc.second, allV[j].first, allV[j].second);
        if (c1 > costs[i] || c2 > costs[j]) continue;
        // コストの削減量を調べる
        ll dCost = 0;
        int mini = 2000000000;
        rep(k, 0, allV.size()) {
          if (allV[k].first > loc.first || allV[k].second > loc.second) {
            if (allV[k].first < loc.first || allV[k].second < loc.second) continue;
            int c = culcCost(loc.first, loc.second, allV[k].first, allV[k].second);
            dCost += min(0, c-costs[k]);
          } else {
            int c = culcCost(allV[k].first, allV[k].second, loc.first, loc.second);
            mini = min(mini, c);
          }
        }
        dCost += mini;
        if (dCost < 0) newV.push_back({dCost, loc});
      }
    }
  }
  sort(all(newV));
  rep(i, 0, N*4) {
    if (i == newV.size()) break;
    allV.push_back(newV[i].second);
  }
  // 不要な頂点を削除
  vector<int> usedCnt(allV.size());
  vector<int> par2(allV.size());
  par2[0] = -1;
  rep(i, 1, allV.size()) {
    int mini = 2000000000, minInd = 0;
    rep(j, 0, allV.size()) {
      if (i == j) continue;
      if (allV[j].first > allV[i].first || allV[j].second > allV[i].second) continue;
      int tmp = culcCost(allV[j].first, allV[j].second, allV[i].first, allV[i].second);
      if (tmp < mini) {
        mini = tmp;
        minInd = j;
      }
    }
    usedCnt[minInd]++;
    par2[i] = minInd;
  }
  stack<int> toDel;
  vector<bool> isDel(allV.size());
  rep(i, N+1, allV.size()) {
    if (usedCnt[i] == 0) toDel.push(i);
  }
  while (!toDel.empty()) {
    int tmp = toDel.top(); toDel.pop();
    isDel[tmp] = 1;
    usedCnt[par2[tmp]]--;
    if (par2[tmp] > N && usedCnt[par2[tmp]] == 0) toDel.push(par2[tmp]);
  }
  rep(i, N+1, allV.size()) {
    if (usedCnt[i] == 1) isDel[i] = 1;
  }
  vector<pair<int,int>> memo = allV;
  allV.clear();
  rep(i, 0, memo.size()) {
    if (isDel[i]) continue;
    allV.push_back(memo[i]);
  }
  }
  // 解を構築
  rep(i, 1, allV.size()) {
    int mini = 2000000000, minInd = 0;
    rep(j, 0, allV.size()) {
      if (i == j) continue;
      if (allV[j].first > allV[i].first || allV[j].second > allV[i].second) continue;
      int tmp = culcCost(allV[j].first, allV[j].second, allV[i].first, allV[i].second);
      if (tmp < mini) {
        mini = tmp;
        minInd = j;
      }
    }
    ans.push_back({allV[minInd].first, allV[minInd].second, allV[i].first, allV[i].second});
  }

  sort(all(ans));
  cout << ans.size() << endl;
  rep(i, 0, ans.size()) {
    rep(j, 0, 4) cout << ans[i][j] << " ";
    cout << endl;
    cost += culcCost(ans[i][0], ans[i][1], ans[i][2], ans[i][3]);
  }

#if MODE == 0
  ofstream f;
  f.open("./analyze/costs.csv", ios_base::app);
  f << (int)round(N*(double)L/(1.+cost)*1000000) << endl;
#endif
}
