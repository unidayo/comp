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
constexpr lint MOD = 47321342134473211;
constexpr lint B1 = 1532834020;
constexpr lint M1 = 2147482409;
constexpr lint B2 = 1388622299;
constexpr lint M2 = 2147478017;
void yes(bool expr) {cout << (expr ? "Yes" : "No") << "\n";}
template<class T>void chmax(T &a, const T &b) { if (a<b) a=b; }
template<class T>void chmin(T &a, const T &b) { if (b<a) a=b; }
constexpr int TIME_LIMIT = 1980;
int N, M, K, T;
vector<int> A, B, X, Y;
lint update_hash(lint cur, bool color) {
  if(!color) return (cur * 3 + 1);
  else return (cur * 3 + 2);
}
chrono ::system_clock::time_point start_time;

struct State {
  int id;
  int last_id;
  int node;
  lint hash;
  int cost;
  int last;
  int move = -1;
  bool color_change = false;
  State get_copy() const {
    State res;
    res.last_id = id;
    res.node = node;
    res.hash = hash;
    res.cost = cost;
    res.last = last;
    return res;
  }
  bool operator<(const State &rhs) const {
    if(cost != rhs.cost) return cost < rhs.cost;
    else if(node != rhs.node) return node < rhs.node;
    else return hash < rhs.hash;
  }
};


pair<int, vector<int>> solve(int COLOR_COST, int COLOR_COST_9000, int COLOR_COST_8000, int COLOR_COST_6000)
{

  vector<vector<int>> G(N);
  REP(i, M) {
    G[A[i]].emplace_back(B[i]);
    G[B[i]].emplace_back(A[i]);
  }
  //ステーションの隣接ノードは色を変えない
  vector<bool> is_station_adjacent(N, false);
  REP(i, K) {
    for(int to : G[i]) {
      is_station_adjacent[to] = true;
    }
  }
  int cur = 0;
  int last = -1;
  vector<bool> color(N);
  vector<unordered_set<lint>> hash_list(N);
  vector<int> last_visited(N, -1);
  vector<int> ans;
  vector<P> color_change_events;
  vector<vector<int>> bucket(100);
  while(ans.size() + color_change_events.size() < T) {
    double elapsed_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
    if(elapsed_time > TIME_LIMIT) break;
    //ショップからハッシュをキーにしてBFSを行い、最短ルートでスコアが得られるものを選ぶ
    //途中で色を変える必要がある場合は過去改変を行う（そのマスに最後に移動したターンを覚えておく）
    //色を変える操作に対してはコストを定義する

    vector<State> move_history;
    move_history.reserve(1000); // 適当に。あとで調整
    auto push_state = [&](State &s){
      s.id = (int)move_history.size();
      move_history.push_back(s);
    };

    auto calc_score = [&](const State &s, int candidate_count) {
      int base_cost = s.cost;

      REP(i, 100) bucket[i].clear();
      int bucket_ptr = 0;
      bucket[0].push_back(s.id);
      vector<State> candidates;
      vector<bool> done(K);
      bool first = true;
      while(bucket_ptr < (int)bucket.size()) {
        if(bucket[bucket_ptr].empty()) {
          bucket_ptr++;
          continue;
        }
        int id = bucket[bucket_ptr].back();
        bucket[bucket_ptr].pop_back();
        State s = move_history[id];
        //cerr << s.cost << " " << s.node << " " << s.hash << "\n";
        //cerr << "  node: " << s.node << ", cost: " << s.cost << ", hash: " << s.hash << "\n";
        if(s.node < K && s.cost != 0 && !first && !done[s.node]) {
          //ショップに到達
          if(hash_list[s.node].find(s.hash) == hash_list[s.node].end()) {
            done[s.node] = true;
            candidates.emplace_back(s);
            if((int)candidates.size() >= candidate_count) {
              return candidates;
            }
          }
        }
        first = false;
        //隣のノードに移動
        for(int to : G[s.node]) {
          if(s.last == to) continue; //直前のノードには戻らない
          //色を変えない場合
          State t = s.get_copy();
          t.move = to;
          t.color_change = false;
          if(s.node < K) t.hash = 0;
          if(to >= K) t.hash = update_hash(t.hash, color[to]);
          t.cost++;
          if(t.cost < bucket.size()) {
            t.last = s.node;
            t.node = to;
            push_state(t);
            bucket[t.cost-base_cost].push_back(t.id);
          }
          //色を変える場合
          if(last_visited[to] != -1 && !color[to] && to >= K && s.cost-base_cost < 5) {
            State u = s.get_copy();
            u.move = to;
            u.color_change = true;
            if(s.node < K) u.hash = 0;
            if(to >= K) u.hash = update_hash(u.hash, true);
            if(ans.size() > 9000) u.cost += COLOR_COST_9000;
            else if(ans.size() > 7500) u.cost += COLOR_COST_8000;
            else if(ans.size() > 6000) u.cost += COLOR_COST_6000;
            else u.cost += COLOR_COST; //色を変えるコスト
            if(is_station_adjacent[to]) u.cost += 1;
            if(u.cost < bucket.size()) {
              u.last = s.node;
              u.node = to;
              push_state(u);
              bucket[u.cost-base_cost].push_back(u.id);
            }
          }
        }
      }
      return candidates;
    };

    State init;
    init.last_id = -1;
    init.node = cur;
    init.hash = 0;
    init.cost = 0;
    init.last = last;
    init.move = -1;
    init.color_change = false;
    push_state(init);
    vector<State> candidates = calc_score(init, 4);
    //各候補について次の手の達成ターン数を考慮
    State best;
    double best_cost = 1e9;
    for(State &s : candidates) {
      int next_cost = calc_score(s, 1)[0].cost;
      double cost = s.cost * 0.5 + next_cost * 1.0;
      cost *= (10.0 + rand() % 500 / 1000.0); //少しランダム性を入れる
      if(cost < best_cost) {
        best_cost = cost;
        best = s;
      }
    }
    hash_list[best.node].insert(best.hash);

    //ルートの実行
    vector<int> id_list;
    int cur_id = best.id;
    cur = best.node;
    last = best.last;
    while(cur_id != -1) {
      id_list.emplace_back(cur_id);
      cur_id = move_history[cur_id].last_id;
    }
    reverse(ALL(id_list));
    REP(i, (int)id_list.size()) {
      State s = move_history[id_list[i]];
      if(s.move == -1) continue;
      ans.emplace_back(s.move);
      if(s.color_change) {
        color[s.move] = true;
        color_change_events.emplace_back(P(last_visited[s.move], s.move));
      }
      last_visited[s.move] = ans.size();
    }
    
  }
  sort(ALL(color_change_events));
  //color_changeをansに組み込む
  IREP(i, (int)color_change_events.size()) {
    P event = color_change_events[i];
    int insert_idx = event.first;
    ans.insert(ans.begin() + insert_idx, -1);
  }

  int score = 0;
  REP(i, K) score += (int)hash_list[i].size();
  return {score, ans};
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout.tie(0);
  cin >> N >> M >> K >> T;
  A.resize(M);
  B.resize(M);
  REP(i, M) cin >> A[i] >> B[i];
  X.resize(N);
  Y.resize(N);
  REP(i, N) cin >> X[i] >> Y[i];

  start_time = chrono::system_clock::now();
  int best_score = -1;
  vector<int> best_ans;
  while(true) {
    double elapsed_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start_time).count();
    if(elapsed_time > TIME_LIMIT) break;
    for(int COLOR_COST : {7}) for(int COLOR_COST_9000 : {2, 3}) for(int COLOR_COST_8000 : {3, 4}) for(int COLOR_COST_6000 : {4, 5}) {
      auto [score, ans] = solve(COLOR_COST, COLOR_COST_9000, COLOR_COST_8000, COLOR_COST_6000);
      if(score > best_score) {
        best_score = score;
        best_ans = ans;
      }
    }
  }

  REP(i, min(T, (int)best_ans.size())) {
    cout << best_ans[i] << "\n";
  }
  cerr << best_score << "\n";
  quick_exit(0);

}