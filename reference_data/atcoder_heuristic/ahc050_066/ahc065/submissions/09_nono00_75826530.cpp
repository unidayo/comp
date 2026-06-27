#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

const int INF = 1e9;
int n = 20;
int goal_p;

//  ========================================================================
//  グローバル配列（メモリ確保ゼロ化で極限の速度を出す）
//  ========================================================================
int initial_board[400];
int initial_box_pos[400];

int M = 20;
int belts_1D_arr[20][40];
int c_ids[400][2];
int nxt_p_arr[20][2][400];

struct Op {
    int m, d;
};

const int MAX_STATES = 4000000;
const int MAX_K = 10005;

int dist_arr[MAX_STATES];
int parent_u[MAX_STATES];
int8_t parent_m[MAX_STATES];
int8_t parent_d[MAX_STATES];

bool protected_belt[MAX_K][20];
int visited_states[MAX_STATES];
int visited_cnt = 0;

int tmp_val[400];
int board[400];

//  XorShift 乱数
uint32_t xor_shift() {
    static uint32_t y = 2463534242;
    y = y ^ (y << 13);
    y = y ^ (y >> 17);
    return y ^ (y << 5);
}

//  ========================================================================
//  超高速 時空間0-1 BFS エンジン
//  ========================================================================
//  修正1: out_history を引数で受け取り、成功した時だけ歴史を更新する
vector<Op> solve_from(int start_X, const vector<Op>& base_ans, bool use_random, int best_known_size,
                      vector<vector<Op>>& out_history) {
    vector<Op> current_ans = base_ans;

    for (int X = start_X; X < 400; X++) {
        int K = current_ans.size();

        //  枝刈り：すでにベストスコアを超えている未来は探索しない
        if (use_random && K >= best_known_size) return {};

        int t_ready = -1;
        int curr_target = 0;

        for (int i = 0; i < 400; i++) board[i] = initial_board[i];

        if (board[goal_p] == 0) {
            board[goal_p] = -1;
            curr_target++;
        }

        //  VIPカウントの初期化
        int vip_on_belt[20] = {0};
        for (int p = 0; p < 400; p++) {
            if (board[p] != -1 && board[p] < X) {
                vip_on_belt[c_ids[p][0]]++;
                vip_on_belt[c_ids[p][1]]++;
            }
        }

        //  時刻表シミュレーション ＆ VIP差分更新
        for (int t = 0; t <= K; t++) {
            for (int m = 0; m < M; m++) {
                protected_belt[t][m] = (vip_on_belt[m] > 0);
            }
            if (t == K) break;

            if (curr_target == X && t_ready == -1) t_ready = t;

            int m = current_ans[t].m;
            int dir = current_ans[t].d;

            for (int i = 0; i < 40; i++) {
                int p = belts_1D_arr[m][i];
                int box = board[p];
                tmp_val[p] = box;
                if (box != -1 && box < X) {
                    int old_cross = (c_ids[p][0] == m) ? c_ids[p][1] : c_ids[p][0];
                    vip_on_belt[old_cross]--;
                }
            }
            for (int i = 0; i < 40; i++) {
                int p = belts_1D_arr[m][i];
                int n_p = nxt_p_arr[m][dir][p];
                int box = tmp_val[p];
                board[n_p] = box;
                if (box != -1 && box < X) {
                    int new_cross = (c_ids[n_p][0] == m) ? c_ids[n_p][1] : c_ids[n_p][0];
                    vip_on_belt[new_cross]++;
                }
            }

            if (board[goal_p] == curr_target) {
                //  修正2: 過去にVIPとして追跡されていた箱だけをカウントから引く
                if (curr_target < X) {
                    int cb0 = c_ids[goal_p][0];
                    int cb1 = c_ids[goal_p][1];
                    vip_on_belt[cb0]--;
                    vip_on_belt[cb1]--;
                }
                board[goal_p] = -1;
                curr_target++;
            }
        }
        if (curr_target == X && t_ready == -1) t_ready = K;

        //  0-1 BFS 探索
        if (curr_target <= X) {
            int init_X_p = -1;
            for (int i = 0; i < 400; i++)
                if (initial_board[i] == X) {
                    init_X_p = i;
                    break;
                }

            int start_u = 0 * 400 + init_X_p;
            dist_arr[start_u] = 0;
            visited_states[visited_cnt++] = start_u;

            vector<int> q0, q1;
            q0.reserve(4000);
            q1.reserve(4000);
            q0.push_back(start_u);

            int curr_cost = 0;
            int q0_idx = 0;
            int best_goal_u = -1;

            int r_m = use_random ? xor_shift() % 2 : 0;
            int r_d = use_random ? xor_shift() % 2 : 0;

            while (true) {
                if (q0_idx == q0.size()) {
                    if (q1.empty()) break;
                    q0.swap(q1);
                    q1.clear();
                    q0_idx = 0;
                    curr_cost++;
                }
                int u = q0[q0_idx++];
                int t = u / 400;
                int p = u % 400;

                if (p == goal_p && t >= t_ready) {
                    best_goal_u = u;
                    break;
                }

                if (t >= MAX_K - 5) continue;

                //  コスト0（タダ乗り）
                if (t < K) {
                    int m = current_ans[t].m;
                    int dir = current_ans[t].d;
                    int nxt_p = nxt_p_arr[m][dir][p];
                    int nxt_u = (t + 1) * 400 + nxt_p;

                    if (dist_arr[nxt_u] > curr_cost) {
                        if (dist_arr[nxt_u] == INF) visited_states[visited_cnt++] = nxt_u;
                        dist_arr[nxt_u] = curr_cost;
                        parent_u[nxt_u] = u;
                        parent_m[nxt_u] = -1;
                        q0.push_back(nxt_u);
                    }
                }

                //  コスト1（割り込み）
                for (int i = 0; i < 2; i++) {
                    int m = c_ids[p][(i + r_m) % 2];
                    if (!protected_belt[t][m]) {
                        for (int j = 0; j < 2; j++) {
                            int dir = (j + r_d) % 2;
                            int nxt_p = nxt_p_arr[m][dir][p];
                            int nxt_u = t * 400 + nxt_p;

                            if (dist_arr[nxt_u] > curr_cost + 1) {
                                if (dist_arr[nxt_u] == INF) visited_states[visited_cnt++] = nxt_u;
                                dist_arr[nxt_u] = curr_cost + 1;
                                parent_u[nxt_u] = u;
                                parent_m[nxt_u] = m;
                                parent_d[nxt_u] = dir;
                                q1.push_back(nxt_u);
                            }
                        }
                    }
                }
            }

            //  経路が見つからなかった場合のクラッシュ対策
            if (best_goal_u == -1) {
                for (int i = 0; i < visited_cnt; i++) dist_arr[visited_states[i]] = INF;
                visited_cnt = 0;
                return {};
            }

            //  経路復元
            vector<pair<int, Op>> inserts;
            int curr_u = best_goal_u;
            while (curr_u != start_u) {
                int pm = parent_m[curr_u];
                int pt = parent_u[curr_u] / 400;
                if (pm != -1) inserts.push_back({pt, {pm, parent_d[curr_u]}});
                curr_u = parent_u[curr_u];
            }
            reverse(inserts.begin(), inserts.end());

            vector<Op> new_ans;
            new_ans.reserve(K + inserts.size());
            int insert_idx = 0;
            for (int t = 0; t <= K; t++) {
                while (insert_idx < ssize(inserts) && inserts[insert_idx].first == t) {
                    new_ans.push_back(inserts[insert_idx].second);
                    insert_idx++;
                }
                if (t < K) new_ans.push_back(current_ans[t]);
            }
            current_ans = move(new_ans);

            for (int i = 0; i < visited_cnt; i++) dist_arr[visited_states[i]] = INF;
            visited_cnt = 0;
        }

        //  正しく構築できた部分だけをローカルの歴史に保存
        out_history[X] = current_ans;
    }
    return current_ans;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    auto time_start = chrono::high_resolution_clock::now();

    if (!(cin >> n)) return 0;
    goal_p = 0 * n + (n / 2);
    for (int i = 0; i < 400; i++) {
        cin >> initial_board[i];
        initial_box_pos[initial_board[i]] = i;
    }

    for (int i = 0; i < MAX_STATES; i++) dist_arr[i] = INF;

    //  ========================================================================
    //  トポロジーの構築（完全交差グリッド）
    //  ========================================================================
    cout << 20 << '\n';
    int m_idx = 0;

    for (int i = 0; i < n; i += 2) {
        int len = 0;
        for (int j = 0; j < n; j++) {
            int p = i * n + j;
            belts_1D_arr[m_idx][len++] = p;
            c_ids[p][0] = m_idx;
        }
        for (int j = n - 1; j >= 0; j--) {
            int p = (i + 1) * n + j;
            belts_1D_arr[m_idx][len++] = p;
            c_ids[p][0] = m_idx;
        }
        m_idx++;
    }

    for (int j = 0; j < n; j += 2) {
        int len = 0;
        for (int i = 0; i < n; i++) {
            int p = i * n + j;
            belts_1D_arr[m_idx][len++] = p;
            c_ids[p][1] = m_idx;
        }
        for (int i = n - 1; i >= 0; i--) {
            int p = i * n + (j + 1);
            belts_1D_arr[m_idx][len++] = p;
            c_ids[p][1] = m_idx;
        }
        m_idx++;
    }

    //  コンベアに乗っていないマスの行き先を「動かない」に初期化
    for (int m = 0; m < 20; m++) {
        for (int dir = 0; dir < 2; dir++) {
            for (int p = 0; p < 400; p++) {
                nxt_p_arr[m][dir][p] = p;
            }
        }
    }

    //  次の座標テーブルの計算
    for (int m = 0; m < 20; m++) {
        cout << 40;
        for (int i = 0; i < 40; i++) {
            int p = belts_1D_arr[m][i];
            cout << ' ' << (p / n) << ' ' << (p % n);
        }
        cout << '\n';

        for (int i = 0; i < 40; i++) {
            int p = belts_1D_arr[m][i];
            nxt_p_arr[m][1][p] = belts_1D_arr[m][(i + 1) % 40];
            nxt_p_arr[m][0][p] = belts_1D_arr[m][(i + 39) % 40];
        }
    }

    //  ========================================================================
    //  D&R ループの実行
    //  ========================================================================
    vector<vector<Op>> best_ans_history(400);
    vector<vector<Op>> temp_history(400);

    //  初回ベースラインの構築
    vector<Op> best_ans = solve_from(0, {}, false, INF, temp_history);
    int best_ans_size = best_ans.size();

    //  正しい歴史を保存
    for (int X = 0; X < 400; X++) best_ans_history[X] = temp_history[X];

    int iter = 0;
    while (true) {
        auto now = chrono::high_resolution_clock::now();
        if (chrono::duration<double>(now - time_start).count() > 1.85) break;

        //  破壊地点：100番〜379番の間でランダムに選ぶ
        int start_X = 100 + xor_shift() % 280;

        //  汚染されていない「真のベスト歴史」から再構築をスタート！
        vector<Op> base_ans = best_ans_history[start_X - 1];
        vector<Op> candidate = solve_from(start_X, base_ans, true, best_ans_size, temp_history);

        //  新しいルートが既存のベストを超えた場合のみ、歴史を更新する
        if (!candidate.empty() && candidate.size() < best_ans_size) {
            best_ans = move(candidate);
            best_ans_size = best_ans.size();
            for (int X = start_X; X < 400; X++) best_ans_history[X] = temp_history[X];
        }
        iter++;
    }

    //  --- 最終出力 ---
    cout << best_ans.size() << '\n';
    for (auto op: best_ans) {
        cout << op.m << ' ' << (op.d == 1 ? 1 : -1) << '\n';
    }

    return 0;
}
