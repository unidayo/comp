#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <random>
using namespace std;

class UnionFind2d{
  int n;
  vector<int> data;
  int num_connected_components_;
  int get_idx(pair<int,int> x){
    auto [a,b] = x;
    return a*n + b;
  }
  pair<int,int> get_val(int x){
    return pair(x/n,x%n);
  }
public:
  UnionFind2d(int n) : n(n), data(n*n, -1), num_connected_components_(n*n) {}
  bool unite(pair<int,int> x, pair<int,int> y) noexcept {
    int x_idx = find(get_idx(x));
    int y_idx = find(get_idx(y));
    if(x_idx != y_idx){
      --num_connected_components_;
      if(data[y_idx] < data[x_idx]) std::swap(x_idx,y_idx);
      data[x_idx] += data[y_idx];
      data[y_idx] = x_idx;
    }
    return x_idx != y_idx;
  }
  bool same(pair<int,int> x, pair<int,int> y) noexcept {
    return find(x) == find(y);
  }
  bool same(int x, int y) noexcept {
    return find(x) == find(y);
  }
  int find(pair<int,int> x){
    return find(get_idx(x));
  }
  int find(int x_idx) noexcept {
    if(data[x_idx] < 0) return x_idx;
    return data[x_idx] = find(data[x_idx]);
  }
  int size(int x) noexcept {
    return -data[find(x)];
  }
  int size(pair<int,int> x) noexcept {
    return size(find(x));
  }
  int num_connected_components() const noexcept {
    return num_connected_components_;
  }
};

template<int n>
struct Box {
  array<array<int,n>,n> data;
  Box(){
    for(int i = 0; i < n; ++i)
      fill(data[i].begin(),data[i].end(),0);
  }
  pair<int,int> put(int pos, int type){
    for(int i = 0; i < n; ++i){
      for(int j = 0; j < n; ++j){
	if(data[i][j]) continue;
	--pos;
	if(!pos){
	  data[i][j] = type;
	  return pair(i,j);
	}
      }
    }
    // assert(false);
    return pair(-1,-1);
  }
  void transpose(){
    for(int i = 0; i < n; ++i){
      for(int j = 0; j < i; ++j){
	swap(data[i][j],data[j][i]);
      }
    }
  }
  void move_L(){
    for(auto& d: data){
      int j = -1;
      for(int i = 0; i < n; ++i){
	if(d[i] && j < 0) continue;
	if(d[i]){
	  swap(d[i],d[j]);
	  while(j <= i and d[j])
	    ++j;
	}else if(j < 0){
	  j = i;
	}
      }
    }
  }
  void move_R(){
    for(auto& d: data){
      int j = -1;
      for(int i = n-1; i >= 0; --i){
	if(d[i] && j < 0) continue;
	if(d[i]){
	  swap(d[i],d[j]);
	  while(j >= i and d[j])
	    --j;
	}else if(j < 0){
	  j = i;
	}
      }
    }
  }
  void move_F(){
    transpose();
    move_L();
    transpose();
  }
  void move_B(){
    transpose();
    move_R();
    transpose();
  }
  void move(char c){
    if(c == 'F') move_F();
    if(c == 'B') move_B();
    if(c == 'L') move_L();
    if(c == 'R') move_R();
  }
  void dump(){
    cerr << "===========\n";
    for(auto d: data){
      for(auto e: d)
	cerr << e;
      cerr << '\n';
    }
    cerr << "===========\n";
  }
};

template<int n>
double score(const Box<n>& box){
  UnionFind2d uf(n);
  vector<int> cnt(3);
  for(int i = 0; i < n; ++i){
    for(int j = 0; j < n; ++j){
      if(!box.data[i][j]) continue;
      ++cnt[box.data[i][j]-1];
      if(i+1 < n and box.data[i][j] == box.data[i+1][j])
	uf.unite(pair(i,j),pair(i+1,j));
      if(j+1 < n and box.data[i][j] == box.data[i][j+1])
	uf.unite(pair(i,j),pair(i,j+1));
    }
  }
  double ans = 0;
  for(int i = 0; i < n*n; ++i){
    if(uf.find(i) != i) continue;
    int sz = uf.size(i);
    ans += sz*sz;
  }
  ans *= 1e6;
  ans /= cnt[0]*cnt[0] + cnt[1]*cnt[1] + cnt[2]*cnt[2];
  return ans;
}

int rand(int ub){// [0,ub)
  static random_device seed_gen;
  static mt19937 engine(seed_gen());
  return engine()%ub;
}

template<int n>
double greedy_simulate(Box<n> b, int t, const vector<int>& F, const char target){
  for(int i = t; i < n*n; ++i){
    int pos = rand(n*n-i);
    b.put(pos,F[i]);
    char dir;
    if(F[i+1] == target) dir = 'F';
    else if(F[i] == target) dir = 'B';
    else if(F[i+1] == (target+1)%3+1) dir = 'L';
    else dir = 'R';
    b.move(dir);
  }
  return score(b);
}

template<int n>
double estimate_value(Box<n> b, int t, const vector<int>& F, const char target){
  double e = 0;
  const int test_num = 800;
  for(int i = 0; i < test_num; ++i){
    e += greedy_simulate(b,t,F,target);
  }
  return e/test_num;
}

int main(){
  const int n = 10;
  using box = Box<n>;
  
  vector<int> F(n*n);
  for(int i = 0; i < n*n; ++i)
    cin >> F[i];
  
  vector<int> cnt(3);
  for(auto f: F)
    ++cnt[f-1];

  int target = max_element(cnt.begin(),cnt.end()) - cnt.begin() + 1;

  box b;
  const string FBLR = "FBLR";

  int initial = n*n/2;
  for(int i = 0; i < initial; ++i){
    int t;
    cin >> t;
    auto [x,y] = b.put(t,F[i]);
    char dir;
    if(F[i+1] == target) dir = 'F';
    else if(F[i] == target) dir = 'B';
    else if(F[i+1] == (target+1)%3+1) dir = 'L';
    else dir = 'R';
    b.move(dir);
    cout << dir << endl;
  }
  
  for(int i = initial; i < n*n; ++i){
    int t;
    cin >> t;
    auto [x,y] = b.put(t,F[i]);
    char dir = 'L';
    double s = 0;
    for(auto c: FBLR){
      box b_ = b;
      b_.move(c);
      double s_ = estimate_value(b_,i+1,F,target);
      if(s_ > s){
	dir = c;
	s = s_;
      }
    }

    b.move(dir);
    cout << dir << endl;
  }
  b.dump();
  cerr << score(b) << '\n';
}
