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

unsigned long xor128(void)
{
    static unsigned long x=123456789,y=362436069,z=541288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

#include <atcoder/all>
using namespace atcoder;
using mint = modint998244353;

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  int N, L;
  cin >> N >> L;
  vector<int> T(N);
  REP(i, N) cin >> T[i];
  REP(i, N) T[i] *= 2;

  auto start_time = chrono::system_clock::now();

  vector<int> a(N), b(N), c(N);
  vector<multiset<int>> rev(N);
  REP(i, N) {
    a[i] = (i+1)%N;
    b[i] = (i+1)%N;
    rev[(i+1)%N].insert(i);
    rev[(i+1)%N].insert(i);
  }
  REP(i, N) {
    c[a[i]] += T[i] / 2;
    c[b[i]] += T[i] / 2;
  }

  //2-opt,3-optでは、Tがなるべく近いようなものが選ばれた方が評価関数がなだらかになるので、事前に遷移候補を列挙する
  vector<tuple<int, int, int>> opt2;
  vector<tuple<int, int, int, int>> opt3;
  REP(i, N) FOR(j, i+1, N) opt2.emplace_back(abs(T[i]-T[j]), i, j);
  REP(i, N) FOR(j, i+1, N) FOR(k, j+1, N) opt3.emplace_back(abs(T[i]-T[j])+abs(T[j]-T[k])+abs(T[k]-T[i]), i, j, k);
  sort(ALL(opt2));
  sort(ALL(opt3));
  int CAND_FACTOR = 7;
  int OPT2_CANDIDATES = opt2.size() / CAND_FACTOR;
  int OPT3_CANDIDATES = opt3.size() / CAND_FACTOR;

  int TIME_LIMIT = 1980;
  int best_score = 0;
  REP(i, N) best_score += abs(c[i]-T[i]);
  int best_score_2 = INF;
  vector<int> best_a(N), best_b(N);
  int loop = 0;
  int best_turn = 0;
  float start_temp = 2.4, end_temp = 1.0;
  float temp = start_temp;
  float time;
  float temppow;

  while(true) {
    if(loop%50000 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      //cerr << loop << " " << best_score << endl;
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT;
      temppow = pow(0.1, temp);
    }
    loop++;

    int score = best_score;

    int mode = xor128() % 3;
    bool modei = xor128() % 2;
    bool modej = xor128() % 2;
    bool modek = xor128() % 2;
    int i, j, k;
    int next;
    if(mode == 0) {
      while(true) {
        i = xor128() % N;
        modei = xor128() % 2;
        if(modei) {
          if(c[b[i]] < T[b[i]]) continue;
        } else {
          if(c[a[i]] < T[a[i]]) continue;
        }
        next = xor128() % N;
        if(next == i) continue;
        if(modei) {
          if(next == b[i]) continue;
        } else {
          if(next == a[i]) continue;
        }
        if(modei) {
          if(c[b[next]] > T[b[next]]) continue;
        } else {
          if(c[a[next]] > T[a[next]]) continue;
        }
        break;
      }
    } else if(mode == 1) {
      int idx = xor128() % OPT2_CANDIDATES;
      i = get<1>(opt2[idx]);
      j = get<2>(opt2[idx]);
      if(!modei && a[i] == j) continue;
      if(modei && b[i] == j) continue;
      if(!modej && a[j] == i) continue;
      if(modej && b[j] == i) continue;
    } else {
      int idx = xor128() % OPT3_CANDIDATES;
      i = get<1>(opt3[idx]);
      j = get<2>(opt3[idx]);
      k = get<3>(opt3[idx]);
      if(!modei && a[i] == j) continue;
      if(modei && b[i] == j) continue;
      if(!modej && a[j] == k) continue;
      if(modej && b[j] == k) continue;
      if(!modek && a[k] == i) continue;
      if(modek && b[k] == i) continue;
    }

    int last;
    if(mode == 0) {
      last = (modei ? b[i] : a[i]);
      if(!modei) a[i] = next;
      else b[i] = next;
      score -= abs(c[last]-T[last]);
      c[last] -= T[i] / 2;
      score += abs(c[last]-T[last]);
      score -= abs(c[next]-T[next]);
      c[next] += T[i] / 2;
      score += abs(c[next]-T[next]);
    } else if(mode == 1) {
      //2-opt
      if(i == j) continue;
      if(!modei) {
        score -= abs(c[a[i]]-T[a[i]]);
        c[a[i]] -= T[i] / 2;
        score += abs(c[a[i]]-T[a[i]]);
      } else {
        score -= abs(c[b[i]]-T[b[i]]);
        c[b[i]] -= T[i] / 2;
        score += abs(c[b[i]]-T[b[i]]);
      }
      if(!modej) {
        score -= abs(c[a[j]]-T[a[j]]);
        c[a[j]] -= T[j] / 2;
        score += abs(c[a[j]]-T[a[j]]);
      } else {
        score -= abs(c[b[j]]-T[b[j]]);
        c[b[j]] -= T[j] / 2;
        score += abs(c[b[j]]-T[b[j]]);
      }
      if(!modei && !modej) swap(a[i], a[j]);
      else if(!modei && modej) swap(a[i], b[j]);
      else if(modei && !modej) swap(b[i], a[j]);
      else swap(b[i], b[j]);
      if(!modei) {
        score -= abs(c[a[i]]-T[a[i]]);
        c[a[i]] += T[i] / 2;
        score += abs(c[a[i]]-T[a[i]]);
      } else {
        score -= abs(c[b[i]]-T[b[i]]);
        c[b[i]] += T[i] / 2;
        score += abs(c[b[i]]-T[b[i]]);
      }
      if(!modej) {
        score -= abs(c[a[j]]-T[a[j]]);
        c[a[j]] += T[j] / 2;
        score += abs(c[a[j]]-T[a[j]]);
      } else {
        score -= abs(c[b[j]]-T[b[j]]);
        c[b[j]] += T[j] / 2;
        score += abs(c[b[j]]-T[b[j]]);
      }
    } else {
      //3-opt
      if(i == j || j == k || k == i) continue;
      if(!modei) {
        score -= abs(c[a[i]]-T[a[i]]);
        c[a[i]] -= T[i] / 2;
        score += abs(c[a[i]]-T[a[i]]);
      } else {
        score -= abs(c[b[i]]-T[b[i]]);
        c[b[i]] -= T[i] / 2;
        score += abs(c[b[i]]-T[b[i]]);
      }
      if(!modej) {
        score -= abs(c[a[j]]-T[a[j]]);
        c[a[j]] -= T[j] / 2;
        score += abs(c[a[j]]-T[a[j]]);
      } else {
        score -= abs(c[b[j]]-T[b[j]]);
        c[b[j]] -= T[j] / 2;
        score += abs(c[b[j]]-T[b[j]]);
      }
      if(!modek) {
        score -= abs(c[a[k]]-T[a[k]]);
        c[a[k]] -= T[k] / 2;
        score += abs(c[a[k]]-T[a[k]]);
      } else {
        score -= abs(c[b[k]]-T[b[k]]);
        c[b[k]] -= T[k] / 2;
        score += abs(c[b[k]]-T[b[k]]);
      }
      if(!modei && !modej && !modek) {
        int tmp = a[i];
        a[i] = a[j];
        a[j] = a[k];
        a[k] = tmp;
      } else if(!modei && !modej && modek) {
        int tmp = a[i];
        a[i] = a[j];
        a[j] = b[k];
        b[k] = tmp;
      } else if(!modei && modej && !modek) {
        int tmp = a[i];
        a[i] = b[j];
        b[j] = a[k];
        a[k] = tmp;
      } else if(!modei && modej && modek) {
        int tmp = a[i];
        a[i] = b[j];
        b[j] = b[k];
        b[k] = tmp;
      } else if(modei && !modej && !modek) {
        int tmp = b[i];
        b[i] = a[j];
        a[j] = a[k];
        a[k] = tmp;
      } else if(modei && !modej && modek) {
        int tmp = b[i];
        b[i] = a[j];
        a[j] = b[k];
        b[k] = tmp;
      } else if(modei && modej && !modek) {
        int tmp = b[i];
        b[i] = b[j];
        b[j] = a[k];
        a[k] = tmp;
      } else {
        int tmp = b[i];
        b[i] = b[j];
        b[j] = b[k];
        b[k] = tmp;
      }
      if(!modei) {
        score -= abs(c[a[i]]-T[a[i]]);
        c[a[i]] += T[i] / 2;
        score += abs(c[a[i]]-T[a[i]]);
      } else {
        score -= abs(c[b[i]]-T[b[i]]);
        c[b[i]] += T[i] / 2;
        score += abs(c[b[i]]-T[b[i]]);
      }
      if(!modej) {
        score -= abs(c[a[j]]-T[a[j]]);
        c[a[j]] += T[j] / 2;
        score += abs(c[a[j]]-T[a[j]]);
      } else {
        score -= abs(c[b[j]]-T[b[j]]);
        c[b[j]] += T[j] / 2;
        score += abs(c[b[j]]-T[b[j]]);
      }
      if(!modek) {
        score -= abs(c[a[k]]-T[a[k]]);
        c[a[k]] += T[k] / 2;
        score += abs(c[a[k]]-T[a[k]]);
      } else {
        score -= abs(c[b[k]]-T[b[k]]);
        c[b[k]] += T[k] / 2;
        score += abs(c[b[k]]-T[b[k]]);
      }

    }
    bool ok = true;
    if(a[i] == i || b[i] == i || a[i] == b[i]) ok = false;
    if(mode >= 1 && (a[j] == j || b[j] == j || a[j] == b[j])) ok = false;
    if(mode >= 2 && (a[k] == k || b[k] == k || a[k] == b[k])) ok = false;

    /*
    //接続性チェック
    vector<bool> used(N);
    deque<int> que;
    que.push_back(0);
    used[0] = true;
    while(!que.empty()) {
      int p = que.front();
      que.pop_front();
      if(a[p] != p && !used[a[p]]) {
        que.push_back(a[p]);
        used[a[p]] = true;
      }
      if(b[p] != p && !used[b[p]]) {
        que.push_back(b[p]);
        used[b[p]] = true;
      }
    }
    REP(i, N) if(!used[i]) ok = false;
    */

    float prob = exp(((float)best_score-score)*temppow);
    if(ok && (score <= best_score || prob*(float)INF > (xor128()%INF))) {
      best_score = score;
      if(score < best_score_2 && time > 1000) {
        if(ok) {
          best_score_2 = score;
          best_a = vector<int>(a);
          best_b = vector<int>(b);
        }
      }
    } else {
      if(mode == 0) {
        int next = (modei ? b[i] : a[i]);
        c[next] -= T[i] / 2;
        c[last] += T[i] / 2;
        if(!modei) a[i] = last;
        else b[i] = last;
      } else if(mode == 1) {
        if(!modei) {
          c[a[i]] -= T[i] / 2;
        } else {
          c[b[i]] -= T[i] / 2;
        }
        if(!modej) {
          c[a[j]] -= T[j] / 2;
        } else {
          c[b[j]] -= T[j] / 2;
        }
        if(!modei && !modej) swap(a[i], a[j]);
        else if(!modei && modej) swap(a[i], b[j]);
        else if(modei && !modej) swap(b[i], a[j]);
        else swap(b[i], b[j]);
        if(!modei) {
          c[a[i]] += T[i] / 2;
        } else {
          c[b[i]] += T[i] / 2;
        }
        if(!modej) {
          c[a[j]] += T[j] / 2;
        } else {
          c[b[j]] += T[j] / 2;
        }
      } else {
        if(!modei) {
          c[a[i]] -= T[i] / 2;
        } else {
          c[b[i]] -= T[i] / 2;
        }
        if(!modej) {
          c[a[j]] -= T[j] / 2;
        } else {
          c[b[j]] -= T[j] / 2;
        }
        if(!modek) {
          c[a[k]] -= T[k] / 2;
        } else {
          c[b[k]] -= T[k] / 2;
        }
        if(!modei && !modej && !modek) {
          int tmp = a[i];
          a[i] = a[k];
          a[k] = a[j];
          a[j] = tmp;
        } else if(!modei && !modej && modek) {
          int tmp = a[i];
          a[i] = b[k];
          b[k] = a[j];
          a[j] = tmp;
        } else if(!modei && modej && !modek) {
          int tmp = a[i];
          a[i] = a[k];
          a[k] = b[j];
          b[j] = tmp;
        } else if(!modei && modej && modek) {
          int tmp = a[i];
          a[i] = b[k];
          b[k] = b[j];
          b[j] = tmp;
        } else if(modei && !modej && !modek) {
          int tmp = b[i];
          b[i] = a[k];
          a[k] = a[j];
          a[j] = tmp;
        } else if(modei && !modej && modek) {
          int tmp = b[i];
          b[i] = b[k];
          b[k] = a[j];
          a[j] = tmp;
        } else if(modei && modej && !modek) {
          int tmp = b[i];
          b[i] = a[k];
          a[k] = b[j];
          b[j] = tmp;
        } else {
          int tmp = b[i];
          b[i] = b[k];
          b[k] = b[j];
          b[j] = tmp;
        }

        if(!modei) {
          c[a[i]] += T[i] / 2;
        } else {
          c[b[i]] += T[i] / 2;
        }
        if(!modej) {
          c[a[j]] += T[j] / 2;
        } else {
          c[b[j]] += T[j] / 2;
        }
        if(!modek) {
          c[a[k]] += T[k] / 2;
        } else {
          c[b[k]] += T[k] / 2;
        }
      }
    }
  }

  REP(i, N) cout << best_a[i] << " " << best_b[i] << "\n";

  //simulate
  a = best_a;
  b = best_b;
  c = vector<int>(N);
  vector<bool> mode(N);
  int p = 0;
  REP(i, L) {
    c[p]++;
    mode[p] = !mode[p];
    if(mode[p]) p = a[p];
    else p = b[p];
  }
  int score = 0;
  REP(i, N) score += abs(c[i]-T[i]/2);
  cerr << 1000000 - score << "\n";
}