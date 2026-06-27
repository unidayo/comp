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

vector<int> dx = {-1, 0, 1, 0};
vector<int> dy = {0, -1, 0, 1};
vector<char>dc = {'U', 'L', 'D', 'R'};

int N, M;
vector<P> p;

unsigned long xor128(void)
{
    static unsigned long x=123456789,y=362436069,z=541288629,w=88675123;
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

struct IndexSet {

  vector<int> que;
  vector<int> pos;

  IndexSet(int n) : pos(n, -1) {}

  void add(int v) {
    if(pos[v] != -1) return;
    pos[v] = que.size();
    que.push_back(v);
  }

  void remove(int v) {
    if(pos[v] == -1) return;
    int p = pos[v];
    int b = que.back();
    que[p] = b;
    que.pop_back();
    pos[b] = p;
    pos[v] = -1;
  }

  void change(int v) {
    int p = pos[v];
    if(p == -1) {
      add(v);
    } else {
      remove(v);
    }
  }

  bool contains(int v) const {
    return pos[v] != -1;
  }

  int size() const {
    return que.size();
  }
};

int calc_dist(int a, int b) {
  return abs(a/N - b/N) + abs(a%N - b%N);
}

vector<pair<char, char>> calc_score(vector<vector<int>> &state, vector<vector<int>> &turn_pos) {
  vector<pair<char, char>> ans;
  //ある行・列に存在する壁の位置をソートして記憶
  vector<int> up_stop(N*N), down_stop(N*N), left_stop(N*N), right_stop(N*N);
  REP(i, N) REP(j, N) {
    up_stop[i*N+j] = 0;
    down_stop[i*N+j] = N-1;
    left_stop[i*N+j] = 0;
    right_stop[i*N+j] = N-1;
  }
  //BFS（全ターンで使いまわせるようにする）
  deque<int> que;
  vector<int> dist(N*N, INF);
  vector<int> prev(N*N, -1);
  vector<int> prev_move(N*N, -1);
  vector<bool> is_wall(N*N, false);
  int ptr = 0;
  vector<int> dist_ptr(N*N, -1);
  int x = p[0].first;
  int y = p[0].second;
  REP(i, M-1) {
    vector<int> destinations(state[i]);
    destinations.push_back(p[i+1].first*N+p[i+1].second);

    REP(z, destinations.size()) {
      int dest_pos = destinations[z];
      bool is_wall_turn = z < destinations.size()-1;

      //壁の設置をBFSにより行う
      que.clear();
      que.push_back(x*N+y);
      dist[x*N+y] = 0;
      ptr++;
      dist_ptr[x*N+y] = ptr;
      int next_x = -1, next_y = -1;
      while(!que.empty()) {
        int pos = que.front();
        que.pop_front();
        if(is_wall_turn && calc_dist(pos, dest_pos) == 1) {
          //壁の設置ができるマスに到達した
          next_x = pos/N;
          next_y = pos%N;
          break;
        }
        if(!is_wall_turn && pos == dest_pos) {
          //目的地に到達した
          next_x = pos/N;
          next_y = pos%N;
          break;
        }
        //移動
        REP(j, 4) {
          int nx = pos/N + dx[j];
          int ny = pos%N + dy[j];
          if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
          int npos = nx*N+ny;
          if(is_wall[npos]) continue;
          if(dist_ptr[npos] == ptr) continue;
          dist[npos] = dist[pos] + 1;
          prev[npos] = pos;
          dist_ptr[npos] = ptr;
          prev_move[npos] = j;
          que.push_back(npos);
        }
        //滑走
        REP(j, 4) {
          int nx = pos/N;
          int ny = pos%N;
          if(j == 0) nx = up_stop[pos];
          else if(j == 1) ny = left_stop[pos];
          else if(j == 2) nx = down_stop[pos];
          else if(j == 3) ny = right_stop[pos];
          int npos = nx*N+ny;
          if(is_wall[npos]) continue;
          if(dist_ptr[npos] == ptr) continue;
          dist[npos] = dist[pos] + 1;
          prev[npos] = pos;
          dist_ptr[npos] = ptr;
          prev_move[npos] = j + 4;
          que.push_back(npos);
        }
      }
      if(next_x == -1) {
        return vector<pair<char, char>>();
      }

      //経路復元
      vector<pair<char, char>> ans_tmp;
      int tmppos = next_x*N+next_y;
      while(true) {
        turn_pos[i].push_back(tmppos);
        if(tmppos == x*N+y) break;
        int pos = prev[tmppos];
        if(prev_move[tmppos] < 4) {
          ans_tmp.push_back({'M', dc[prev_move[tmppos]]});
        } else {
          ans_tmp.push_back({'S', dc[prev_move[tmppos]-4]});
        }
        tmppos = pos;
      }
      reverse(ALL(ans_tmp));

      //壁の設置処理
      if(is_wall_turn) {
        //壁の設置
        //nextから見た壁の設置位置を求める
        int wall_dir = -1;
        REP(d, 4) {
          int nx = next_x + dx[d];
          int ny = next_y + dy[d];
          if(nx*N+ny == dest_pos) {
            wall_dir = d;
            break;
          }
        }
        ans_tmp.push_back({'A', dc[wall_dir]});
        is_wall[dest_pos] = !is_wall[dest_pos];
        //壁の上下左右の更新
        REP(xx, dest_pos/N) chmin(down_stop[xx*N+dest_pos%N], dest_pos/N-1);
        FOR(xx, dest_pos/N+1, N) chmax(up_stop[xx*N+dest_pos%N], dest_pos/N+1);
        REP(yy, dest_pos%N) chmin(right_stop[dest_pos/N*N+yy], dest_pos%N-1);
        FOR(yy, dest_pos%N+1, N) chmax(left_stop[dest_pos/N*N+yy], dest_pos%N+1);
      }

      for(auto p : ans_tmp) {
        ans.push_back(p);
      }

      x = next_x;
      y = next_y;

    }


  }

  return ans;
}


struct State {
  int x, y;
  int next_dest_idx;
  IndexSet is_wall;
};

vector<IndexSet> build_turn_pos_set(vector<vector<int>> &turn_pos) {
  vector<IndexSet> turn_pos_set(M-1, IndexSet(N*N));
  REP(i, M-1) {
    for(int pos : turn_pos[i]) {
      int x = pos/N;
      int y = pos%N;
      REP(d, 4) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
        int npos = nx*N+ny;
        turn_pos_set[i].add(npos);
      }
    }
  }
  return turn_pos_set;
}


int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  
  cin >> N >> M;
  p.resize(M);
  REP(i, M) {
    cin >> p[i].first >> p[i].second;
  }

  //やりたいこと
  //有望な壁の位置を事前に決めておきたい

  vector<int> pos_idx(N*N, -1);
  REP(i, M) pos_idx[p[i].first*N+p[i].second] = i;

  auto start_time = chrono::system_clock::now();
  vector<vector<int>> state(M-1); //各ターンでどこにどの順番で壁を設置するか
  int wall_count = 0;
  vector<bool> wall(N*N, 0); //壁の設置位置を記憶
  vector<vector<int>> turn_pos(M-1); //各ターンでどこにどの順番で壁を設置するか
  int best_score = calc_score(state, turn_pos).size();
  vector<IndexSet> turn_pos_set = build_turn_pos_set(turn_pos);
  int TIME_LIMIT = 1975;
  int loop = 0;
  float start_temp = -0.2, end_temp = -1.0;
  float temp = start_temp;
  float time;
  int best_score_2 = best_score;
  vector<vector<int>> best_state = state;

  vector<int> turn_weight(M-1, 0);
  REP(i, M-1) turn_weight[i] = pow(M-2-i, 2.0);
  vector<int> turn_pool;
  REP(i, M-1) REP(j, turn_weight[i]) {
    turn_pool.push_back(i);
  }

  while(true) {
    if(loop%100 == 0) {
      time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
      if(time > TIME_LIMIT) break;
      //cerr << loop << " " << best_score << endl;
      temp = start_temp + (end_temp - start_temp) * time / (float)TIME_LIMIT;
    }
    loop++;

    //あるターンの壁設置動作の追加or削除をし、BFSでスコア計算する
    vector<vector<int>> last_state(state);
    int mode_rnd = xor128() % 1000;
    int mode = 0;
    if(mode_rnd < 200) mode = 0;
    else if(mode_rnd < 400) mode = 1;
    else if(mode_rnd < 600) mode = 2;
    else if(mode_rnd < 800) mode = 3;
    else if(mode_rnd < 950) mode = 4;
    else mode = 5;
    if((mode == 1 || mode == 2 || mode == 3) && wall_count == 0) mode = 0;
    int add_pos = -1;
    int remove_pos = -1;
    if(mode == 0) {
      //add
      int turn = turn_pool[xor128() % turn_pool.size()];
      int insert_idx = xor128() % (1 + state[turn].size());
      add_pos = turn_pos_set[turn].que[xor128() % turn_pos_set[turn].size()];
      //壁の設置範囲を限定する
      //現在の最短ルート周辺のみ置けるとする
      if(wall[add_pos]) continue;
      state[turn].insert(state[turn].begin() + insert_idx, add_pos);
    } else if(mode == 1) {
      //remove
      int turn;
      while(true) {
        turn = turn_pool[xor128() % turn_pool.size()];
        if(state[turn].size() == 0) continue;
        break;
      }
      int remove_idx = xor128() % state[turn].size();
      remove_pos = state[turn][remove_idx];
      assert(wall[remove_pos]);
      state[turn].erase(state[turn].begin() + remove_idx);
    } else if(mode == 2) {
      //remove and add
      int turn;
      while(true) {
        turn = turn_pool[xor128() % turn_pool.size()];
        if(state[turn].size() == 0) continue;
        break;
      }
      int remove_idx = xor128() % state[turn].size();
      remove_pos = state[turn][remove_idx];
      assert(wall[remove_pos]);
      add_pos = turn_pos_set[turn].que[xor128() % turn_pos_set[turn].size()];
      //壁の設置範囲を限定する
      //現在の最短ルート周辺のみ置けるとする
      if(wall[add_pos]) continue;
      state[turn].erase(state[turn].begin() + remove_idx);
      int insert_idx = xor128() % (1 + state[turn].size());
      state[turn].insert(state[turn].begin() + insert_idx, add_pos);
    } else if(mode == 3) {
      //move
      int turn;
      while(true) {
        turn = turn_pool[xor128() % turn_pool.size()];
        if(state[turn].size() == 0) continue;
        break;
      }
      int remove_idx = xor128() % state[turn].size();
      remove_pos = state[turn][remove_idx];
      assert(wall[remove_pos]);
      add_pos = remove_pos;
      int x = add_pos/N;
      int y = add_pos%N;
      while(true) {
        int d = xor128() % 4;
        int nx = x + dx[d];
        int ny = y + dy[d];
        if(nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
        add_pos = nx*N+ny;
        break;
      }
      if(wall[add_pos]) continue;
      state[turn][remove_idx] = add_pos;
    } else if(mode == 4) {
      //change
      int turn = turn_pool[xor128() % turn_pool.size()];
      if(state[turn].size() == 0) continue;
      int change_idx = xor128() % state[turn].size();
      int change_pos = state[turn][change_idx];
      assert(wall[change_pos]);
      state[turn].erase(state[turn].begin() + change_idx);
      int next_turn = turn_pool[xor128() % turn_pool.size()];
      int insert_idx = xor128() % (1 + state[next_turn].size());
      state[next_turn].insert(state[next_turn].begin() + insert_idx, change_pos);
    } else {
      //swap
      int turn = turn_pool[xor128() % turn_pool.size()];
      if(state[turn].size() <= 1) continue;
      int swap_idx_1 = xor128() % state[turn].size();
      int swap_idx_2 = xor128() % state[turn].size();
      if(swap_idx_1 == swap_idx_2) continue;
      swap(state[turn][swap_idx_1], state[turn][swap_idx_2]);
    }

    //全ターン貪欲BFSによるスコア計算
    vector<vector<int>> turn_pos(M-1);
    int score = calc_score(state, turn_pos).size();
    if(score == 0) {
      state = last_state;
      continue;
    }
    int diff = best_score - score;
    double prob = exp(diff*pow(0.1, temp));
    if(diff >= 0 || prob*(double)INF > (xor128()%INF)) {
      best_score = score;
      if(add_pos != -1) {
        assert(!wall[add_pos]);
        wall[add_pos] = true;
        wall_count++;
      }
      if(remove_pos != -1) {
        assert(wall[remove_pos]);
        wall[remove_pos] = false;
        wall_count--;
      }
      turn_pos_set = build_turn_pos_set(turn_pos);
      if(score < best_score_2) {
        best_score_2 = score;
        best_state = vector<vector<int>>(state);
      }
    } else {
      state = last_state;
    }

  }

  REP(i, M-1) cerr << state[i].size() << " ";
  cerr << "\n";

  vector<pair<char, char>> ans = calc_score(best_state, turn_pos);
  for(pair<char, char> p : ans) {
    cout << p.first << " " << p.second << "\n";
  }
  cerr << 1640 - ans.size() << "\n";
}