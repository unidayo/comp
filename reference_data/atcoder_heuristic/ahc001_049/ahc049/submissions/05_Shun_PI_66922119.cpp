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
int N;
vector<int> w, d;
vector<float> wd; // wの0.3乗とdの0.7乗を掛けたもの
constexpr int TIME_LIMIT = 1985; // ミリ秒
mt19937 mt(0);

unsigned long xor128(void)
{
    static unsigned long x=123456789,y=362436069,z=541288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

int calc_dist(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

//ある順序で荷物を上に積んで運び出す際に、それが可能か・可能ならかかる手数を返す
int calc_path_score(vector<int> &path) {
  int score = 0;
  int wsum = 0;
  int distsum = 0;
  int x = 0, y = 0; // 現在の位置
  IREP(i, path.size()) {
    int idx = path[i];
    if(d[idx] <= wsum) return -1;
    int dist = calc_dist(x, y, idx/N, idx%N);
    score += dist;
    distsum += dist;
    wsum += w[idx] * distsum;
    x = idx / N;
    y = idx % N;
  }
  int back_dist = calc_dist(x, y, 0, 0);
  score += back_dist;
  return score;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N;
  w.resize(N*N);
  d.resize(N*N);
  REP(i, N) REP(j, N) cin >> w[i*N + j];
  REP(i, N) REP(j, N) cin >> d[i*N + j];
  REP(i, N*N) wd.push_back(pow(w[i], 0.4) * pow(d[i], 0.6));

  auto start_time = chrono::system_clock::now();

  //複数のパスを作り、その順に運んでいく
  //「ある荷物の下にその荷物を置けるか？」を用いて、どの荷物列の下にその荷物を置くかを決めていく貪欲を考えると、ビームサーチになりそう
  //決める順序はdの昇順？(0,0からの距離も重要そう)
  vector<P> order(N*N-1);
  FOR(i, 1, N*N) order[i-1] = P(wd[i], i);
  sort(ALL(order));
  vector<vector<int>> paths;
  vector<int> wsum, dist_sum;
  REP(i, N*N-1) {
    int idx = order[i].second;
    assert(idx > 0); // idxは1から始まる
    //末端からの距離が最も近く、かつ挿入可能なところに貪欲に入れる
    int min_dist = calc_dist(0, 0, idx/N, idx%N);
    int min_idx = -1;
    REP(j, paths.size()) {
      int dist = calc_dist(idx/N, idx%N, paths[j].back()/N, paths[j].back()%N);
      if(dist < min_dist && d[idx] > wsum[j]) {
        min_dist = dist;
        min_idx = j;
      }
    }
    if(min_idx == -1) {
      //新しいパスを作る
      paths.push_back({idx});
      wsum.push_back(w[idx] * calc_dist(0, 0, idx/N, idx%N));
      dist_sum.push_back(calc_dist(0, 0, idx/N, idx%N));
    } else {
      //既存のパスに追加
      wsum[min_idx] += w[idx] * (dist_sum[min_idx] + calc_dist(paths[min_idx].back()/N, paths[min_idx].back()%N, idx/N, idx%N));
      dist_sum[min_idx] += calc_dist(paths[min_idx].back()/N, paths[min_idx].back()%N, idx/N, idx%N);
      paths[min_idx].push_back(idx);
    }
  }
  REP(i, paths.size()) reverse(ALL(paths[i]));
  cerr << "Initial paths: " << paths.size() << "\n";

  vector<int> idx_to_path(N*N);
  REP(i, paths.size()) {
    for(int idx : paths[i]) {
      idx_to_path[idx] = i;
    }
  }
  vector<bool> used_path(paths.size(), true);

  int best_score = 0;
  vector<int> path_score(paths.size());
  REP(i, paths.size()) {
    path_score[i] = calc_path_score(paths[i]);
    best_score += path_score[i];
  }
  int loop = 0;
  float start_temp = 0.3, end_temp = -0.7;
  float temp = start_temp;
  float time;

  while(true) {
    if(loop%10000 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      //cerr << loop << " " << best_score << "\n";
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT;
      REP(i, paths.size()) if(paths[i].empty()) used_path[i] = false;
    }
    loop++;

    //あるインデックスを選び、別のパスに移動
    int mode = 0;
    if(xor128()%100 < 25) mode = 1;
    else if(xor128()%100 < 80) mode = 2;
    int idx, new_path_idx;
    vector<int> old_path, new_path;
    int score = best_score;
    int score1, score2;
    int idx1, idx2;
    if(mode == 0) {
      idx = xor128() % (N*N-1) + 1;
      int last_path_pos = -1;
      REP(i, paths[idx_to_path[idx]].size()) {
        if(paths[idx_to_path[idx]][i] == idx) {
          last_path_pos = i;
          break;
        }
      }
      assert(last_path_pos != -1);
      while(true) {
        new_path_idx = xor128() % (paths.size());
        if(!used_path[new_path_idx]) continue;
        if(new_path_idx == idx_to_path[idx]) continue;
        break;
      }

      old_path = vector<int>(paths[idx_to_path[idx]]);
      new_path = vector<int>(paths[new_path_idx]);

      //移動
      paths[idx_to_path[idx]].erase(paths[idx_to_path[idx]].begin() + last_path_pos);
      //d順ソート
      REP(i, paths[new_path_idx].size()+1) {
        if(i == paths[new_path_idx].size() || wd[paths[new_path_idx][i]] <= wd[idx]) {
          paths[new_path_idx].insert(paths[new_path_idx].begin() + i, idx);
          break;
        }
      }

      //スコア計算
      score1 = calc_path_score(paths[idx_to_path[idx]]);
      score2 = calc_path_score(paths[new_path_idx]);
      score -= path_score[idx_to_path[idx]];
      score -= path_score[new_path_idx];
      score += score1;
      score += score2;
      if(score1 == -1 || score2 == -1) {
        score = 1e9;
      }
    } else if(mode == 1) {
      while(true) {
        idx1 = xor128() % (N*N-1) + 1;
        idx2 = xor128() % (N*N-1) + 1;
        if(idx_to_path[idx1] == idx_to_path[idx2]) continue;
        if((float)wd[idx1] / wd[idx2] > 1.5 || (float)wd[idx2] / wd[idx1] > 1.5) continue; //荷物の重さが近いと交換しない
        break;
      }
      int pos1 = -1, pos2 = -1;
      REP(i, paths[idx_to_path[idx1]].size()) {
        if(paths[idx_to_path[idx1]][i] == idx1) {
          pos1 = i;
          break;
        }
      }
      REP(i, paths[idx_to_path[idx2]].size()) {
        if(paths[idx_to_path[idx2]][i] == idx2) {
          pos2 = i;
          break;
        }
      }
      assert(pos1 != -1 && pos2 != -1);

      old_path = vector<int>(paths[idx_to_path[idx1]]);
      new_path = vector<int>(paths[idx_to_path[idx2]]);
      swap(paths[idx_to_path[idx1]][pos1], paths[idx_to_path[idx2]][pos2]);
      //d順ソート
      while(pos1 > 0 && wd[paths[idx_to_path[idx1]][pos1-1]] <= wd[paths[idx_to_path[idx1]][pos1]]) {
        swap(paths[idx_to_path[idx1]][pos1], paths[idx_to_path[idx1]][pos1-1]);
        pos1--;
      }
      while(pos2 > 0 && wd[paths[idx_to_path[idx2]][pos2-1]] <= wd[paths[idx_to_path[idx2]][pos2]]) {
        swap(paths[idx_to_path[idx2]][pos2], paths[idx_to_path[idx2]][pos2-1]);
        pos2--;
      }

      score1 = calc_path_score(paths[idx_to_path[idx1]]);
      score2 = calc_path_score(paths[idx_to_path[idx2]]);
      score -= path_score[idx_to_path[idx1]];
      score -= path_score[idx_to_path[idx2]];
      score += score1;
      score += score2;
      if(score1 == -1 || score2 == -1) {
        score = 1e9;
      }
    } else if(mode == 2) {
      //2-opt
      //2つのパスを選び、区間同士を入れ替える
      int l1, l2;
      while(true) {
        idx1 = xor128() % paths.size();
        idx2 = xor128() % paths.size();
        if(idx1 == idx2) continue;
        if(!used_path[idx1] || !used_path[idx2]) continue;
        if(paths[idx1].size() < 2 || paths[idx2].size() < 2) continue; //パスが短すぎると入れ替えられない
        l1 = xor128() % (paths[idx1].size() - 1) + 1;
        //末尾l1, l2番目のdを比較する
        int d1 = wd[paths[idx1][paths[idx1].size() - l1]];
        l2 = -1;
        REP(i, paths[idx2].size()) {
          int d2 = wd[paths[idx2][i]];
          if(d2 < d1) {
            l2 = paths[idx2].size() - i; //l2はidx2の末尾からの距離
            break;
          }
        }
        if(l2 == -1) continue; //l2が決まらなかったらスキップ
        int d2 = wd[paths[idx2][paths[idx2].size() - l2]];
        if(d1 > wd[paths[idx2][paths[idx2].size() - l2 - 1]]) continue;
        if(d2 > wd[paths[idx1][paths[idx1].size() - l1 - 1]]) continue;
        break;
      }

      old_path = vector<int>(paths[idx1]);
      new_path = vector<int>(paths[idx2]);
      vector<int> tmp1, tmp2;
      tmp1 = vector<int>(paths[idx1].begin() + paths[idx1].size() - l1, paths[idx1].end());
      tmp2 = vector<int>(paths[idx2].begin() + paths[idx2].size() - l2, paths[idx2].end());
      REP(i, l1) paths[idx1].pop_back();
      REP(i, l2) paths[idx2].pop_back();
      REP(i, l1) paths[idx2].push_back(tmp1[i]);
      REP(i, l2) paths[idx1].push_back(tmp2[i]);
      score1 = calc_path_score(paths[idx1]);
      score2 = calc_path_score(paths[idx2]);
      score -= path_score[idx1];
      score -= path_score[idx2];
      score += score1;
      score += score2;
      if(score1 == -1 || score2 == -1) {
        score = 1e9;
      }
    } else {
      //あるパスの隣接交換
      while(true) {
        idx = xor128() % paths.size();
        if(!used_path[idx] || paths[idx].size() < 2) continue;
        break;
      }
      idx1 = xor128() % (paths[idx].size()-1);
      idx2 = idx1 + 1;
      old_path = vector<int>(paths[idx]);
      swap(paths[idx][idx1], paths[idx][idx2]);
      score1 = calc_path_score(paths[idx]);
      score -= path_score[idx];
      score += score1;
      if(score1 == -1) {
        score = 1e9;
      }
    }

    int diff = best_score - score;
    float prob = exp(diff*pow(0.1, temp));
    if(diff >= 0 || prob*(float)INF > (xor128()%INF)) {
      best_score = score;
      if(mode == 0) {
        path_score[idx_to_path[idx]] = score1;
        path_score[new_path_idx] = score2;
        idx_to_path[idx] = new_path_idx;
      } else if(mode == 1) {
        path_score[idx_to_path[idx1]] = score1;
        path_score[idx_to_path[idx2]] = score2;
        swap(idx_to_path[idx1], idx_to_path[idx2]);
      } else if(mode == 2) {
        path_score[idx1] = score1;
        path_score[idx2] = score2;
        REP(i, paths[idx1].size()) {
          idx_to_path[paths[idx1][i]] = idx1;
        }
        REP(i, paths[idx2].size()) {
          idx_to_path[paths[idx2][i]] = idx2;
        }
      } else {
        path_score[idx] = score1;
      }
    } else {
      //元に戻す
      if(mode == 0) {
        paths[idx_to_path[idx]] = old_path;
        paths[new_path_idx] = new_path;
      } else if(mode == 1) {
        paths[idx_to_path[idx1]] = old_path;
        paths[idx_to_path[idx2]] = new_path;
      } else if(mode == 2) {
        paths[idx1] = old_path;
        paths[idx2] = new_path;
      } else {
        paths[idx] = old_path;
      }
    }

  }

  //出力の作成
  int x = 0, y = 0; // 現在の位置
  int score = 16400;
  REP(i, paths.size()) {
    if(paths[i].empty()) continue;
    paths[i].push_back(0);
    REP(j, paths[i].size()) {
      int nx = paths[i][j] / N;
      int ny = paths[i][j] % N;
      while(x < nx) {
        cout << "D" << "\n";
        x++;
        score--;
      }
      while(x > nx) {
        cout << "U" << "\n";
        x--;
        score--;
      }
      while(y < ny) {
        cout << "R" << "\n";
        y++;
        score--;
      }
      while(y > ny) {
        cout << "L" << "\n";
        y--;
        score--;
      }
      if(j != paths[i].size() - 1) {
        cout << "1" << "\n"; // 荷物を積む
      }
    }
  }

  cerr << score << "\n";
}