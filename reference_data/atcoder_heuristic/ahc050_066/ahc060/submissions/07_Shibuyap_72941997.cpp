#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <tuple>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <cstdint>
#include <chrono>
#include <numeric>

using namespace std;

// 乱数生成器
mt19937 rng(12345);

// 定数・グローバル変数
int N, M, K, T;
vector<vector<int>> adj;
vector<pair<int, int>> coords;

// BFSの深さ制限
static constexpr int MAX_DEPTH = 15;
static constexpr int MAX_BFS_NODES = 3000;

// suffixテーブル
static vector<vector<string>> suffix_tbl;

static void init_suffix_table()
{
  suffix_tbl.assign(MAX_DEPTH + 1, {});
  suffix_tbl[0].assign(1, "");
  for (int len = 1; len <= MAX_DEPTH; ++len) {
    int S = 1 << len;
    suffix_tbl[len].resize(S);
    for (int b = 0; b < S; ++b) {
      string s;
      s.resize(len);
      for (int i = 0; i < len; ++i) {
        s[i] = ((b >> i) & 1) ? 'R' : 'W';
      }
      suffix_tbl[len][b] = std::move(s);
    }
  }
}

// 状態管理
struct State
{
  int current_node;
  int prev_node;
  string current_ice;

  // 木の色管理: 0 for W, 1 for R
  vector<int> tree_colors;

  // 赤くするタイミング
  const vector<int>* activation_thresholds_ptr;

  // 各ショップの在庫状況
  vector<unordered_set<string>> shop_inventory;

  // 現在のターン数
  int turn;

  void reset(int /*k_val*/, const vector<int>* thresholds)
  {
    current_node = 0;
    prev_node = -1;
    current_ice.clear();
    fill(tree_colors.begin(), tree_colors.end(), 0);

    for (auto& s : shop_inventory) {
      s.clear();
    }
    turn = 0;
    activation_thresholds_ptr = thresholds;
  }
};

// BFS用の構造体
struct BfsState
{
  int u;
  int prev;       // ★ 追加: 逆流防止用に「直前ノード」を保持（親参照より速い）
  uint16_t bits;
  uint8_t len;
  int parent_idx; // q内のインデックス
  int turn;       // シミュレーション上の時刻（Action2の1ターン待ち込み）
  bool do_act2;   // このノード到着時にAction 2を行ったか
};

// 木の現在の色bitを取得
inline int get_tree_bit(int node_idx, const State& state)
{
  return state.tree_colors[node_idx];
}

// 行動決定ロジック (Path finding)
void solve_bfs_path(State& state, vector<int>& move_queue)
{
  move_queue.clear();

  const int base_turn = state.turn;
  const auto& thr = *state.activation_thresholds_ptr;
  const string& base_ice = state.current_ice;

  // BFS Queue (メモリ再利用)
  static vector<BfsState> q;
  q.clear();
  // なるべく再確保を避ける（ノード上限+α）
  const int TARGET_RESERVE = MAX_BFS_NODES + 1024;
  if ((int)q.capacity() < TARGET_RESERVE) q.reserve(TARGET_RESERVE);

  // 初手候補の列挙
  static vector<int> valid_moves;
  valid_moves.clear();
  if (valid_moves.capacity() < adj[state.current_node].size()) valid_moves.reserve(adj[state.current_node].size());

  // 先頭展開
  for (int v : adj[state.current_node]) {
    if (v == state.prev_node) continue;
    valid_moves.push_back(v);

    const int arrival_time = base_turn + 1;
    bool do_act2 = false;
    int harvested_bit = 0;

    if (v >= K) {
      int current_color = state.tree_colors[v];
      if (current_color == 0) { // 白
        if (arrival_time >= thr[v]) do_act2 = true;
        harvested_bit = 0; // 収穫は白
      } else {
        harvested_bit = 1; // 既に赤
      }
    }

    uint16_t bits = 0;
    uint8_t len = 0;
    if (v >= K) {
      bits = (uint16_t)harvested_bit;
      len = 1;
    }

    const int finish_turn = arrival_time + (do_act2 ? 1 : 0);
    q.push_back({ v, state.current_node, bits, len, -1, finish_turn, do_act2 });
  }

  struct Candidate {
    int q_idx;
    int shop_idx;
    int dist;
  };

  static vector<Candidate> candidates;
  candidates.clear();

  int search_limit_dist = MAX_DEPTH;
  const int EXTRA_SEARCH = 0;

  // final_str を再利用（capacityを育てて再確保を減らす）
  static string final_str;
  final_str.clear();
  if (final_str.capacity() < base_ice.size() + (size_t)MAX_DEPTH) {
    final_str.reserve(base_ice.size() + (size_t)MAX_DEPTH);
  }

  size_t qi = 0;
  while (qi < q.size() && qi < (size_t)MAX_BFS_NODES) {
    const int current_q_idx = (int)qi;
    const BfsState bs = q[qi++]; // コピー（push_backでvectorが動く可能性があるため参照は避ける）

    const int dist_from_start = bs.turn - base_turn;

    if (dist_from_start > search_limit_dist) break;
    if (dist_from_start > MAX_DEPTH) break;

    // ショップ到達
    if (bs.u < K) {
      const string& suf = suffix_tbl[bs.len][bs.bits];

      final_str.assign(base_ice);
      final_str.append(suf);

      auto& inv = state.shop_inventory[bs.u];
      if (inv.find(final_str) == inv.end()) {
        candidates.push_back({ current_q_idx, bs.u, dist_from_start });
        if (search_limit_dist == MAX_DEPTH) {
          search_limit_dist = dist_from_start + EXTRA_SEARCH;
        }
      }
      continue;
    }

    // 次の移動
    for (int next_v : adj[bs.u]) {
      if (next_v == bs.prev) continue; // ★ ここが高速化ポイント（親参照をやめる）

      const int arrival_time = bs.turn + 1;
      bool do_act2 = false;
      int harvested_bit = 0;

      if (next_v >= K) {
        int current_color = state.tree_colors[next_v];
        if (current_color == 0) {
          if (arrival_time >= thr[next_v]) do_act2 = true;
          harvested_bit = 0;
        } else {
          harvested_bit = 1;
        }
      }

      uint16_t nbits = bs.bits;
      uint8_t nlen = bs.len;
      if (next_v >= K) {
        if (nlen < MAX_DEPTH) {
          nbits = (uint16_t)(nbits | (uint16_t)(harvested_bit << nlen));
          ++nlen;
        }
      }

      const int finish_turn = arrival_time + (do_act2 ? 1 : 0);
      q.push_back({ next_v, bs.u, nbits, nlen, current_q_idx, finish_turn, do_act2 });
    }
  }

  // ベストな移動を選択
  int best_q_idx = -1;
  double best_score = -1e18;

  for (const auto& cand : candidates) {
    const int q_idx = cand.q_idx;
    const int shop_idx = cand.shop_idx;
    const int dist = cand.dist;
    const size_t inv_size = state.shop_inventory[shop_idx].size();

    const double score = -(dist * 1000.0) - (double)inv_size;
    if (score > best_score) {
      best_score = score;
      best_q_idx = q_idx;
    }
  }

  if (best_q_idx != -1) {
    // 経路復元（move_queueはスタックとして使用：末尾が次の行動）
    int curr = best_q_idx;
    while (curr != -1) {
      const BfsState& bs = q[curr];

      // 実行順: Move(u) -> Action(-1)
      // push順: Action(-1) -> Move(u)
      if (bs.do_act2) move_queue.push_back(-1);
      move_queue.push_back(bs.u);

      curr = bs.parent_idx;
    }
    return;
  }

  // 見つからなかった場合
  if (!valid_moves.empty()) {
    move_queue.push_back(valid_moves[rng() % valid_moves.size()]);
  } else {
    move_queue.push_back(adj[state.current_node][0]);
  }
}

// シミュレーション実行関数
pair<int, vector<int>> run_simulation(State& state, const vector<int>& thresholds)
{
  state.reset(K, &thresholds);

  vector<int> history;
  history.reserve(T);

  // ★ move_queue を static にして「毎回の確保」を消す（挙動は同じ）
  static vector<int> move_queue;
  move_queue.clear();
  if ((int)move_queue.capacity() < MAX_DEPTH * 2 + 8) move_queue.reserve(MAX_DEPTH * 2 + 8);

  while (state.turn < T) {
    int action = -2;

    // 1. 強制Action 2判定 (閾値到達による割り込み)
    bool forced_action2 = false;
    if (state.current_node >= K) {
      if (state.tree_colors[state.current_node] == 0) {
        int threshold = (*state.activation_thresholds_ptr)[state.current_node];
        if (state.turn >= threshold) {
          action = -1;
          forced_action2 = true;
          move_queue.clear();
        }
      }
    }

    // 2. 移動計画
    if (!forced_action2) {
      if (move_queue.empty()) {
        solve_bfs_path(state, move_queue);
      }

      if (!move_queue.empty()) {
        action = move_queue.back();
        move_queue.pop_back();
      } else {
        // 万が一のフォールバック
        action = adj[state.current_node][0];
      }
    }

    history.push_back(action);

    if (action == -1) {
      // Action 2
      state.tree_colors[state.current_node] = 1;
    }
    else {
      // Action 1
      int next_node = action;
      if (next_node < K) {
        state.shop_inventory[next_node].insert(state.current_ice);
        state.current_ice.clear();
        move_queue.clear(); // ショップについたら計画終了
      }
      else {
        state.current_ice.push_back(state.tree_colors[next_node] ? 'R' : 'W');
      }
      state.prev_node = state.current_node;
      state.current_node = next_node;
    }
    state.turn++;
  }

  int score = 0;
  for (const auto& inv : state.shop_inventory) {
    score += (int)inv.size();
  }
  return { score, history };
}

// ランダムな閾値設定を生成するヘルパー関数（★ 参照渡しで再確保削減）
static void generate_random_thresholds(vector<int>& thresholds)
{
  thresholds.assign(N, 0);

  static vector<int> tree_indices;
  const int num_trees = N - K;
  if ((int)tree_indices.size() != num_trees) tree_indices.resize(num_trees);

  // K..N-1 を詰める
  for (int i = 0; i < num_trees; ++i) tree_indices[i] = K + i;

  shuffle(tree_indices.begin(), tree_indices.end(), rng);

  const int n_start_red = (int)(num_trees * 0.15);
  const int n_end_white = (int)(num_trees * 0.15);

  for (int i = 0; i < num_trees; ++i) {
    int u = tree_indices[i];
    if (i < n_start_red) {
      thresholds[u] = 0;
    }
    else if (i < num_trees - n_end_white) {
      thresholds[u] = rng() % (T + 1);
    }
    else {
      thresholds[u] = T + 1;
    }
  }
}

int main()
{
  auto start_time = chrono::steady_clock::now();

  ios_base::sync_with_stdio(false);
  cin.tie(nullptr);

  init_suffix_table();

  if (!(cin >> N >> M >> K >> T)) return 0;
  adj.assign(N, {});
  for (int i = 0; i < M; ++i) {
    int u, v;
    cin >> u >> v;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }
  coords.resize(N);
  for (int i = 0; i < N; ++i) {
    cin >> coords[i].first >> coords[i].second;
  }

  State state_obj;
  state_obj.tree_colors.resize(N);
  state_obj.shop_inventory.resize(K);
  for (int i = 0; i < K; ++i) {
    state_obj.shop_inventory[i].reserve(1024);
    state_obj.shop_inventory[i].max_load_factor(0.7f); // ★ 探索/挿入の平均を軽くする
  }
  state_obj.current_ice.reserve(256);

  // --- Multi-Start Phase ---
  vector<int> current_thresholds;
  generate_random_thresholds(current_thresholds);

  int current_score = 0;
  vector<int> best_history;
  int best_score = -1;

  long long initial_loops = 0;
  while (true) {
    auto now = chrono::steady_clock::now();
    if (chrono::duration_cast<chrono::milliseconds>(now - start_time).count() > 400) break;

    vector<int> cand_thresholds;
    generate_random_thresholds(cand_thresholds);

    auto res = run_simulation(state_obj, cand_thresholds);

    if (res.first > best_score) {
      best_score = res.first;
      best_history = res.second;
      current_thresholds = std::move(cand_thresholds);
      current_score = best_score;
    }
    initial_loops++;
  }

  // --- Hill Climbing Phase ---
  const long long TIME_LIMIT_MS = 1950;
  long long loops = 0;
  long long accepts = 0;
  long long last_update_loop = 0;

  while (true) {
    auto now = chrono::steady_clock::now();
    if (chrono::duration_cast<chrono::milliseconds>(now - start_time).count() > TIME_LIMIT_MS) break;

    loops++;

    int change_idx = K + (rng() % (N - K));
    int original_val = current_thresholds[change_idx];

    bool is_kick = false;
    vector<pair<int, int>> kick_backup;
    if (loops - last_update_loop > 1 && (rng() % 2 == 0)) {
      is_kick = true;
      kick_backup.reserve(3);
      for (int k = 0; k < 3; ++k) {
        int idx = K + (rng() % (N - K));
        kick_backup.push_back({ idx, current_thresholds[idx] });
        current_thresholds[idx] = rng() % (T + 2);
      }
    }
    else {
      int r = rng() % 100;
      int new_val;
      if (r < 1) new_val = 0;
      else if (r < 2) new_val = T + 1;
      else new_val = rng() % (T + 1);

      if (new_val == original_val) {
        new_val = (original_val + 100) % (T + 2);
      }
      current_thresholds[change_idx] = new_val;
    }

    auto next_res = run_simulation(state_obj, current_thresholds);
    int next_score = next_res.first;

    if (next_score >= current_score) {
      current_score = next_score;
      if (next_score > best_score) {
        best_score = next_score;
        best_history = next_res.second;
        last_update_loop = loops;
      }
      accepts++;
    }
    else {
      if (is_kick) {
        for (auto& p : kick_backup) {
          current_thresholds[p.first] = p.second;
        }
      }
      else {
        current_thresholds[change_idx] = original_val;
      }
    }
  }

  for (int action : best_history) {
    cout << action << "\n";
  }

  cerr << "InitLoops: " << initial_loops << " HCLoops: " << loops
       << " BestScore: " << best_score
       << " Accepts: " << accepts << endl;

  return 0;
}

/*
【強い枝刈り（挙動が変わり得るのでデフォルトでは入れていません）】

1) BFSの重複状態カット（visited）
   Key = (u, prev, bits, len, turn) の完全一致を落とす。
   ただし「MAX_BFS_NODESで打ち切る」挙動が変わり、結果が変動し得る。

2) 先に見つかった候補 dist を上限に、展開push自体を抑制
   例えば dist_from_start + 1 > search_limit_dist のときは子をpushしない、等。
   これも探索順序/打ち切り条件に絡み、結果が変わり得る。
*/
