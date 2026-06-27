#ifdef ONLINE_JUDGE
#pragma GCC optimize ("Ofast")
#pragma GCC optimize ("unroll-loops")
#ifndef NDEBUG
#define NDEBUG
#endif
constexpr int DEBUG = 0;
#else
constexpr int DEBUG = 1;
#endif

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <ext/random>
#include <atcoder/all>
#include <ext/pb_ds/assoc_container.hpp>

using namespace std;
using namespace atcoder;
using ll = long long;
using ull = unsigned long long;
using uint = unsigned int;
using i128 = __int128_t;
using ld = long double;
using pll = pair<ll,ll>;
using pii = pair<int,int>;
constexpr int inf = (1 << 30) - 1;
constexpr ll INF = (1LL << 62LL) - 1LL;
#define rep(i,n) for (int i = 0; i < (int)(n); ++i)
#define rrep(i,n) for (int i = (int)(n) - 1; i >= 0; --i)
#define Rep(i,t,n) for (int i = t; i < (int)(n); ++i)
#define All(a) a.begin(), a.end()
#define MYPQ(a,b,c) priority_queue<a, vector<a>, decltype(b)> c(b);
template<typename T>
bool chmax(T& a, const T& b) {if(a < b){a = b;return true;}else return false;}
template<typename T>
bool chmin(T& a, const T& b) {if(b < a){a = b;return true;}else return false;}

using rand32 = __gnu_cxx::sfmt607;
rand32 engine;
#define rand01 (1.0 * engine() / (UINT32_MAX + 1LL))
ofstream ofs, ofs2, ofs_result;

const int TIME_LIMIT = 1980;
int USE_TIME = 0;
const auto START_TIME = chrono::steady_clock::now();
auto NOW_TIME = START_TIME;
int use_time_ms(){
  NOW_TIME = chrono::steady_clock::now();
  const auto dur = NOW_TIME - START_TIME;
  return USE_TIME = (int)chrono::duration_cast<chrono::milliseconds>(dur).count();
}

void my_exit();

namespace _global{
int N;
int M;
vector<vector<int>>B;

struct Ans{
  vector<pii> a;
  vector<vector<int>> b;
  vector<int> sorted;
  int score = 0, now = 1, last_b;
  void init(){
    sorted.assign(M,0);
  }
  void print(){
    for(auto &[v,i]: a){
      cout << v << " " << i << "\n";
    }
    cout << flush;
  }
  void f1(int v, int i, int idx = 0){
    assert(now <= v && v <= N);
    assert(1 <= i && i <= M);
    a.emplace_back(v,i);
    Rep(j,idx,M){
      if(j+1 == i)continue;
      rep(k,b[j].size()){
        if(b[j][k] != v)continue;
        Rep(l,k,b[j].size())b[i-1].push_back(b[j][l]);
        b[j].resize(k);
        return;
      }
    }
    assert(0);
  }
  bool f2(){
    rep(i,M){
      if(b[i].size() && b[i].back() == now){
        a.emplace_back(now, 0);
        now++;
        b[i].pop_back();
        last_b = i;
        chmin(sorted[i], (int)b[i].size());
        return true;
      }
    }
    return false;
  }
  void search_now(){
    rep(i,M){
      for(auto &p: b[i]){
        if(p == now){
          last_b = i;
          return;
        }
      }
    }
    assert(0);
  }
  void ue_huriwake(){
    while(b[last_b].back() != now){
      int t = b[last_b].back();
      int id = -1, mx = 0;
      rep(i,M){
        if(i == last_b)continue;
        if(b[i].empty()){
          if(mx == 0)id = i;
          continue;
        }
        if(b[i].back() > t)continue;
        if(chmax(mx, b[i].back())){
          id = i;
        }
      }
      if(id != -1){
        int l = b[last_b].size()-1;
        while(l > 0 && b[last_b][l] > b[last_b][l-1] && (b[id].empty() || b[last_b][l-1] > b[id].back()))l--;
        f1(b[last_b][l], id+1, last_b);
        /*
        b[id].push_back(t);
        b[last_b].pop_back();
        a.emplace_back(t,id+1);
        */
        continue;
      }
      int mn = N+1;
      rep(i,M){
        if(i == last_b)continue;
        if(chmin(mn, b[i].back()))id = i;
      }
      if(sorted[id] == (int)b[id].size() && b[id].size() && b[id].back() > t)sorted[id]++;
      int l = b[last_b].size()-1;
      while(l > 0 && b[last_b][l] > b[last_b][l-1] && b[last_b][l-1] > now)l--;

      f1(b[last_b][l], id+1, last_b);
    }
  }
  bool is_sort(int idx){
    if(sorted[idx] == 0)return false;
    /*
    int num = N+1;
    rep(i,b[idx].size()){
      if(chmin(num, b[idx][i]))continue;
      return false;
    }
    return true;
    */
    assert((int)b[idx].size() >= sorted[idx]);
    if((int)b[idx].size() == sorted[idx])return true;
    return false;
  }
  void sita_huriwake(){
    while(b[last_b].size()){
      int x = last_b;
      if(f2()){last_b = x; continue;}
      if(is_sort(last_b))return;
      int t = b[last_b].back();
      int id = -1, mx = 0;
      rep(i,M){
        if(i == last_b)continue;
        if(b[i].empty()){
          if(mx == 0)id = i;
          continue;
        }
        if(b[i].back() > t)continue;
        if(chmax(mx, b[i].back())){
          id = i;
        }
      }
      if(id != -1){
        int l = b[last_b].size()-1;
        while(l > 0 && b[last_b][l] > b[last_b][l-1] && (b[id].empty() || b[last_b][l-1] > b[id].back()))l--;
        f1(b[last_b][l], id+1, last_b);
        /*
        b[id].push_back(t);
        b[last_b].pop_back();
        a.emplace_back(t,id+1);
        */
        continue;
      }
      int mn = N+1;
      rep(i,M){
        if(i == last_b)continue;
        if(chmin(mn, b[i].back()))id = i;
      }
      if(sorted[id] == (int)b[id].size() && b[id].size() && b[id].back() > t)sorted[id]++;
      int l = b[last_b].size()-1;
      while(l > 0 && b[last_b][l] > b[last_b][l-1] && b[last_b][l-1] > now)l--;

      f1(b[last_b][l], id+1, last_b);
    }
  }
  void greedy(){
    int mx1 = N+1;
    if(b[last_b].size())mx1 = b[last_b].back();
    bool is_first = true;
    while(1){
      int x = last_b;
      if(f2()){last_b = x; continue;}
      int id = -1, mx = 0, l = -1;
      rep(i,M){
        if(i == last_b)continue;
        if(b[i].size() <= 1)continue;
        if(sorted[i] >= (int)b[i].size())continue;
        if(b[i].back() < mx1 && b[i].back() > mx){
          int lt = b[i].size()-1;
          while(lt > 0 && b[i][lt] < b[i][lt-1] && b[i][lt-1] < mx1)lt--;
          if(lt == 0)continue;
          id = i, mx = b[i].back(), l = lt;
        }
      }
      if(id == -1){
        sorted[last_b] = b[last_b].size();
        return;
      }
      if(is_first){
        if(a.size()){
          auto p = a.back();
          if(p.first == b[id].back()){
            a.pop_back();
            b[last_b].push_back(b[id].back());
            b[id].pop_back();
            continue;
          }
        }
      }
      f1(b[id][l], last_b+1, id);
      mx1 = mx;
      is_first = false;
    }
  }
  void solve(){
    if(f2())return;
    search_now();
    ue_huriwake();
    assert(f2());
    sita_huriwake();
    //assert(b[last_b].empty());
    greedy();
  }
};
Ans ans;
};
using namespace _global;

void input(){
  cin >> N >> M;
  B.resize(M);
  rep(i,M){
    B[i].assign(N/M, 0);
    rep(j,N/M){
      cin >> B[i][j];
    }
  }
}

void init(){
  ans.b = B;
  ans.init();
}

void solve(){
  while(ans.now <= N){
    ans.solve();
  }
}

void output(){
  ans.print();
}

void my_exit(){
  output();
  cout << flush;
  exit(0);
}

void Main1(){
  input();
  init();
  solve();
  output();
}

int main([[maybe_unused]]int argc, [[maybe_unused]]char *argv[]){
  cin.tie(nullptr);
  ios_base::sync_with_stdio(false);
  Main1();
  cerr << use_time_ms() << "ms\n";
  return 0;
}