//  implemented by Gemini
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
#define rep(i, n) for (ll i = 0; i < (n); i++)

//  ==========================================
//  設定・高速化ユーティリティ
//  ==========================================
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

struct Timer {
    chrono::high_resolution_clock::time_point start;
    Timer() {
        reset();
    }
    void reset() {
        start = chrono::high_resolution_clock::now();
    }
    double get() {
        auto now = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::milliseconds>(now - start).count() / 1000.0;
    }
};

//  高速乱数 (XorShift)
mt19937_64 mt(random_device{}());
uint64_t xor64() {
    static uint64_t x = mt();
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return x;
}
inline double rand_double() {
    return (double)xor64() / ULLONG_MAX;
}
inline int rand_int(int mod) {
    return xor64() % mod;
}

//  ==========================================
//  問題データ (事前計算用テーブル含む)
//  ==========================================
int N, L, T;
ll K;
vector<ll> A;
vector<vector<ll>> C;

//  高速化: op から i, j への変換テーブル
int OP_TO_I[40];
int OP_TO_J[40];

//  ==========================================
//  状態管理
//  ==========================================
struct State {
    ll num_apple;
    ll B[4][10];
    ll P[4][10];

    State() {
        num_apple = K;
        //  memset等を使わずループ展開されやすいように初期化
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 10; j++) {
                B[i][j] = 1;
                P[i][j] = 0;
            }
        }
    }

    //  インライン化を強制
    inline ll get_cost(int i, int j) const {
        return C[i][j] * (P[i][j] + 1);
    }

    inline void execute_buy(int i, int j) {
        ll cost = get_cost(i, j);
        num_apple -= cost;
        P[i][j]++;
    }

    bool try_exe_greedy(int op) {
        if (op == -1) return true;
        //  テーブル参照に置き換え
        int i = OP_TO_I[op];
        int j = OP_TO_J[op];
        ll cost = get_cost(i, j);
        if (num_apple < cost) return false;
        num_apple -= cost;
        P[i][j]++;
        return true;
    }

    inline void progress() {
        //  Level 0
        for (int j = 0; j < 10; j++) {
            num_apple += A[j] * B[0][j] * P[0][j];
        }
        //  Level 1~3
        for (int i = 1; i < 4; i++) {
            for (int j = 0; j < 10; j++) {
                B[i - 1][j] += B[i][j] * P[i][j];
            }
        }
    }
};

//  ==========================================
//  候補リスト
//  ==========================================
vector<int> get_candidates_all(int hero_id) {
    vector<int> candidates;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < N; j++) {
            if (i >= 2 && j != hero_id) continue;
            candidates.push_back(i * N + j);
        }
    }
    return candidates;
}

vector<int> get_candidates_hero(int hero_id) {
    vector<int> candidates;
    for (int i = 0; i < L; i++) {
        candidates.push_back(i * N + hero_id);
    }
    return candidates;
}

//  ==========================================
//  貪欲法
//  ==========================================
vector<int> construct_greedy_ops_static(int hero_id) {
    vector<int> ops(T, -1);
    State s;
    vector<int> candidates = get_candidates_all(hero_id);
    candidates.push_back(-1);

    for (int t = 0; t < T; t++) {
        int best_op = -1;
        double best_eval = -1e18;
        int lookahead = min(100, T - 1 - t);

        for (int op: candidates) {
            State temp_s = s;
            if (!temp_s.try_exe_greedy(op)) {
                if (op != -1) continue;
            }
            temp_s.progress();
            for (int k = 0; k < lookahead; k++) temp_s.progress();

            double eval_val = (temp_s.num_apple < 0) ? -1.0 : (double)temp_s.num_apple;
            if (eval_val > best_eval) {
                best_eval = eval_val;
                best_op = op;
            }
        }
        ops[t] = best_op;
        s.try_exe_greedy(best_op);
        s.progress();
    }

    vector<int> orders;
    orders.reserve(T);  //  予約
    for (int op: ops)
        if (op != -1) orders.push_back(op);
    return orders;
}

//  ==========================================
//  SA Solver
//  ==========================================
struct SASolver {
    int hero_id;
    vector<int> current_orders;
    vector<int> best_orders;
    double current_score;
    double best_score;

    //  高速化: 事前確保済みキャッシュ
    vector<State> cache_states;
    vector<int> cache_times;

    vector<int> cand_hero;
    vector<int> cand_all;
    int size_hero;
    int size_all;

    int last_update_iter;
    double start_temp_local;

    SASolver() {}
    SASolver(int h): hero_id(h) {
        cand_hero = get_candidates_hero(hero_id);
        cand_all = get_candidates_all(hero_id);
        size_hero = cand_hero.size();
        size_all = cand_all.size();
        last_update_iter = 0;
        start_temp_local = 1000.0;

        //  高速化: 最大サイズで確保し、以降リサイズしない
        cache_states.resize(T + 5);
        cache_times.resize(T + 5);
    }

    void initialize() {
        current_orders = construct_greedy_ops_static(hero_id);
        current_orders.reserve(T + 50);  //  余分に確保して再割り当て防止
        current_score = simulate(current_orders, 0, true);
        best_orders = current_orders;
        best_score = current_score;
    }

    //  シミュレーション（インライン化推奨）
    inline double simulate(const vector<int>& orders, int start_idx, bool update_cache) {
        State s;
        int current_t = 0;

        //  キャッシュ復帰
        if (start_idx > 0) {
            //  start_idx-1 がキャッシュ境界内かチェックは省略（ロジック上保証する）
            s = cache_states[start_idx - 1];
            current_t = cache_times[start_idx - 1];
        } else {
            start_idx = 0;
        }

        //  update_cache時のresize削除。コンストラクタで確保済み。
        //  単にインデックスアクセスするだけ。

        int n_orders = orders.size();
        int order_idx = start_idx;

        //  メインループ
        for (int t = current_t; t < T; t++) {
            int executed_op = -1;

            if (order_idx < n_orders) {
                int op = orders[order_idx];
                //  テーブル参照
                int i = OP_TO_I[op];
                int j = OP_TO_J[op];

                ll cost = s.get_cost(i, j);
                if (s.num_apple >= cost) {
                    s.execute_buy(i, j);
                    executed_op = op;
                    if (update_cache) cache_states[order_idx] = s;
                    order_idx++;
                }
            }
            s.progress();

            if (update_cache && executed_op != -1) {
                //  order_idx は既にインクリメントされているので -1
                cache_states[order_idx - 1] = s;
                cache_times[order_idx - 1] = t + 1;
            }

            //  オーバーフロー即死チェック
            if (s.num_apple < 0) return 0.0;
        }
        if (s.num_apple <= 0) return 0.0;
        return 100000.0 * log2((double)s.num_apple);
    }

    //  ステップ実行
    void step(int iter, double time_ratio) {
        if (iter - last_update_iter > 10000) {
            current_orders = best_orders;
            current_score = simulate(current_orders, 0, true);
            start_temp_local = 1000.0;
            last_update_iter = iter;
        }

        double end_temp = 50.0;
        double temp = start_temp_local + (end_temp - start_temp_local) * time_ratio;

        int mode = rand_int(5);

        //  高速化: Change/Swap はコピーしない (Undo方式)
        //  Insert/Erase/Rotate は構造が変わるのでコピーする (Backup方式)

        bool is_structure_change = (mode == 0 || mode == 1 || mode == 4);

        //  構造変更系用のバックアップ
        //  vectorのコピーコストはあるが、頻度とサイズ(N~100)を考えると許容範囲
        //  Change/Swapのコピーを無くすことが重要
        static vector<int> backup;
        if (is_structure_change) {
            backup = current_orders;
        }

        bool possible = true;
        int start_idx = 0;

        //  Undo用変数 (Change/Swap用)
        int undo_idx1 = -1, undo_idx2 = -1;
        int undo_val1 = -1;  //  Change用

        if (mode == 0) {  //  Insert
            if (current_orders.size() < T) {
                int pos = rand_int(current_orders.size() + 1);
                int val = (rand_double() < 0.05) ? cand_all[rand_int(size_all)] : cand_hero[rand_int(size_hero)];
                current_orders.insert(current_orders.begin() + pos, val);
                start_idx = pos;
            } else
                possible = false;
        } else if (mode == 1) {  //  Erase
            if (!current_orders.empty()) {
                int pos = rand_int(current_orders.size());
                current_orders.erase(current_orders.begin() + pos);
                start_idx = pos;
            } else
                possible = false;
        } else if (mode == 2) {  //  Change (Copyなし)
            if (!current_orders.empty()) {
                int pos = rand_int(current_orders.size());
                int val = (rand_double() < 0.05) ? cand_all[rand_int(size_all)] : cand_hero[rand_int(size_hero)];

                undo_idx1 = pos;
                undo_val1 = current_orders[pos];  //  元の値を保存

                current_orders[pos] = val;
                start_idx = pos;
            } else
                possible = false;
        } else if (mode == 3) {  //  Swap (Copyなし)
            if (current_orders.size() >= 2) {
                int p1 = rand_int(current_orders.size());
                int p2 = rand_int(current_orders.size());
                if (p1 == p2)
                    possible = false;
                else {
                    swap(current_orders[p1], current_orders[p2]);
                    undo_idx1 = p1;
                    undo_idx2 = p2;
                    start_idx = min(p1, p2);
                }
            } else
                possible = false;
        } else {  //  Rotate
            if (current_orders.size() >= 3) {
                int L = rand_int(current_orders.size() - 1);
                int max_len = current_orders.size() - L;
                if (max_len < 2) {
                    possible = false;
                } else {
                    int len = 2 + rand_int(max_len - 1);
                    int R = L + len;
                    int M = L + 1 + rand_int(len - 1);
                    rotate(current_orders.begin() + L, current_orders.begin() + M, current_orders.begin() + R);
                    start_idx = L;
                }
            } else
                possible = false;
        }

        if (!possible) return;

        double new_score = simulate(current_orders, start_idx, false);
        double diff = new_score - current_score;

        if (diff >= 0 || exp(diff / temp) > rand_double()) {
            current_score = new_score;
            simulate(current_orders, start_idx, true);  //  キャッシュ更新
            if (new_score > best_score) {
                best_score = new_score;
                best_orders = current_orders;
                last_update_iter = iter;
            }
        } else {
            //  Revert (Undo or Restore Backup)
            if (is_structure_change) {
                current_orders = backup;
            } else {
                //  高速Undo
                if (mode == 2) {  //  Change Undo
                    current_orders[undo_idx1] = undo_val1;
                } else if (mode == 3) {  //  Swap Undo
                    swap(current_orders[undo_idx1], current_orders[undo_idx2]);
                }
            }
        }
    }

    vector<pair<int, int>> get_final_log() {
        vector<pair<int, int>> logs;
        State s;
        int n_orders = best_orders.size();
        int order_idx = 0;

        for (int t = 0; t < T; t++) {
            int executed_op = -1;
            if (order_idx < n_orders) {
                int op = best_orders[order_idx];
                int i = OP_TO_I[op];
                int j = OP_TO_J[op];
                ll cost = s.get_cost(i, j);
                if (s.num_apple >= cost) {
                    s.execute_buy(i, j);
                    executed_op = op;
                    order_idx++;
                }
            }
            logs.push_back({t, executed_op});
            s.progress();
        }
        return logs;
    }
};

//  ==========================================
//  メイン
//  ==========================================
int main() {
    cin.tie(0)->sync_with_stdio(0);
    Timer timer;

    cin >> N >> L >> T >> K;
    A.resize(N);
    for (int i = 0; i < N; i++) cin >> A[i];
    C.resize(L, vector<ll>(N));
    for (int i = 0; i < L; i++) rep(j, N) cin >> C[i][j];

    //  高速化: テーブル初期化
    rep(i, L) rep(j, N) {
        int op = i * N + j;
        OP_TO_I[op] = i;
        OP_TO_J[op] = j;
    }

    //  Phase 1: 多点スタート
    vector<SASolver> solvers;
    solvers.reserve(N);  //  予約
    rep(h, N) {
        solvers.emplace_back(h);
    }

    for (auto& s: solvers) {
        s.initialize();
    }

    //  Phase 2: SA Loop
    const double TIME_LIMIT = 1.95;
    int iter = 0;

    bool culled_5 = false;
    bool culled_2 = false;
    bool culled_1 = false;

    while (true) {
        iter++;

        //  時間チェック頻度を調整 (256回に1回)
        if ((iter & 255) == 0) {
            double now = timer.get();
            if (now > TIME_LIMIT) break;

            int target_size = solvers.size();
            if (!culled_5 && now > 0.3) {
                target_size = 5;
                culled_5 = true;
            } else if (!culled_2 && now > 0.7) {
                target_size = 2;
                culled_2 = true;
            } else if (!culled_1 && now > 1.2) {
                target_size = 1;
                culled_1 = true;
            }

            if (target_size < (int)solvers.size()) {
                sort(solvers.begin(), solvers.end(),
                     [](const SASolver& a, const SASolver& b) { return a.best_score > b.best_score; });
                solvers.resize(target_size);
            }
        }

        double time_ratio = timer.get() / TIME_LIMIT;

        for (auto& s: solvers) {
            s.step(iter, time_ratio);
        }
    }

    double best_global_score = -1.0;
    int best_idx = 0;
    rep(i, solvers.size()) {
        if (solvers[i].best_score > best_global_score) {
            best_global_score = solvers[i].best_score;
            best_idx = i;
        }
    }

    auto final_logs = solvers[best_idx].get_final_log();
    vector<string> output_lines(T, "-1");
    for (auto& log: final_logs) {
        if (log.first >= T) break;
        if (log.second != -1) {
            output_lines[log.first] = to_string(log.second / N) + " " + to_string(log.second % N);
        }
    }
    for (int t = 0; t < T; t++) cout << output_lines[t] << "\n";

    return 0;
}
