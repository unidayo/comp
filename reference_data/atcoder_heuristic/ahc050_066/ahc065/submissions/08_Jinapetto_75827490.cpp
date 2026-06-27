#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")
#include <bits/stdc++.h>

using namespace std;

const int INF = 1e9;
const int MAX_T = 100005;
constexpr double TIME_LIMIT = 1.85; // 実行時間制限 (秒)

using State = array<int16_t, 400>;

int belt_pos[20][40];
int b_idx[20][400];
int next_idx[40][2]; // [i][0: d=-1, 1: d=1]
int D[400];

State initial_board;
int16_t initial_min_box;

// --- 最適化: バージョン管理された dist 配列 ---
int dist_arr[MAX_T][400];
int dist_ver[MAX_T][400];
int current_ver = 0;

inline int get_dist(int t, int p) {
    return dist_ver[t][p] == current_ver ? dist_arr[t][p] : INF;
}

inline void set_dist(int t, int p, int d) {
    dist_arr[t][p] = d;
    dist_ver[t][p] = current_ver;
}

// 復元用配列
int16_t parent_p[MAX_T][400];
int8_t parent_m[MAX_T][400];
int8_t parent_d[MAX_T][400];
bool parent_is_ins[MAX_T][400];

// --- 最適化: ビットマスクによる valid 判定 ---
uint32_t valid_mask[MAX_T];
uint32_t p_to_m_mask[400];

// --- 最適化: アロケーションフリーのバケツキュー ---
struct StateNode {
    int t;
    int16_t p;
    int16_t d;
};

constexpr int BQ_SIZE = 1 << 22;
StateNode bq_buf[3][BQ_SIZE];
int bq_head[3], bq_tail[3];
int bq_active;
int bq_curr_dist;

inline void bq_clear() {
    for(int i = 0; i < 3; ++i) bq_head[i] = bq_tail[i] = 0;
    bq_active = 0;
    bq_curr_dist = 0;
}

inline bool bq_empty() {
    return bq_active == 0;
}

inline void bq_push(int d, int t, int16_t p) {
    int b = d % 3;
    bq_buf[b][bq_tail[b]++] = {t, p, (int16_t)d};
    bq_active++;
}

inline StateNode bq_pop() {
    int b = bq_curr_dist % 3;
    while (bq_head[b] == bq_tail[b]) {
        bq_head[b] = bq_tail[b] = 0;
        bq_curr_dist++;
        b = bq_curr_dist % 3;
    }
    bq_active--;
    return bq_buf[b][bq_head[b]++];
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto start_time = chrono::steady_clock::now();
    mt19937 rng(4649373);

    int N;
    if (!(cin >> N)) return 0;
    
    for (int p = 0; p < 400; p++) cin >> initial_board[p];

    // --- 1. ベルトコンベアの設計 ---
    memset(b_idx, -1, sizeof(b_idx));
    for (int i = 0; i < 40; i++) {
        next_idx[i][0] = (i - 1 + 40) % 40;
        next_idx[i][1] = (i + 1) % 40;
    }

    for (int i = 0; i < 10; i++) {
        int m = i;
        int idx = 0;
        for(int x = 0; x < 20; x++) belt_pos[m][idx++] = (2*i) * 20 + x;
        for(int x = 19; x >= 0; x--) belt_pos[m][idx++] = (2*i + 1) * 20 + x;
    }
    for (int j = 0; j < 10; j++) {
        int m = 10 + j;
        int idx = 0;
        for(int y = 0; y < 20; y++) belt_pos[m][idx++] = y * 20 + (2*j);
        for(int y = 19; y >= 0; y--) belt_pos[m][idx++] = y * 20 + (2*j + 1);
    }

    for (int m = 0; m < 20; m++) {
        for (int i = 0; i < 40; i++) {
            b_idx[m][belt_pos[m][i]] = i;
        }
    }
    
    for(int p=0; p<400; p++) p_to_m_mask[p] = 0;
    for(int m=0; m<20; m++) {
        for(int i=0; i<40; i++) {
            p_to_m_mask[belt_pos[m][i]] |= (1U << m);
        }
    }

    // --- 2. BFSによる搬出口(0, 10)への最短距離計算 ---
    for (int i = 0; i < 400; i++) D[i] = INF;
    int goal_p = 10;
    D[goal_p] = 0;
    queue<int> q;
    q.push(goal_p);
    
    while(!q.empty()){
        int p = q.front(); q.pop();
        for (int m = 0; m < 20; m++) {
            int idx = b_idx[m][p];
            if(idx != -1) {
                for (int d_idx = 0; d_idx < 2; d_idx++) {
                    int np = belt_pos[m][next_idx[idx][d_idx]];
                    if (D[np] == INF) {
                        D[np] = D[p] + 1;
                        q.push(np);
                    }
                }
            }
        }
    }

    vector<pair<int, int>> best_ops;
    int best_score = -1;

    int loop_cnt = 0;
    bool timeout_flag = false;

    // --- 時間いっぱいループ ---
    while (true) {
        auto cur_time = chrono::steady_clock::now();
        double elapsed = chrono::duration<double>(cur_time - start_time).count();
        if (elapsed > TIME_LIMIT) break;

        vector<pair<int, int>> ops;
        ops.reserve(MAX_T);

        initial_min_box = 0;
        State temp_board = initial_board;

        if (temp_board[goal_p] == 0) {
            temp_board[goal_p] = -1;
            initial_min_box = 1;
        }

        // --- 箱 0 の搬出をランダム化 ---
        int k_start = 0;
        if (initial_min_box == 0) {
            int p0 = -1;
            for(int p = 0; p < 400; p++) {
                if (temp_board[p] == 0) { p0 = p; break; }
            }
            
            while(p0 != goal_p) {
                struct MoveCandidate { int m; int dir; int next_p; };
                vector<MoveCandidate> candidates;
                int current_d = D[p0];

                for(int m = 0; m < 20; m++) {
                    int idx = b_idx[m][p0];
                    if (idx != -1) {
                        for(int d_idx = 0; d_idx < 2; d_idx++) {
                            int np1 = belt_pos[m][next_idx[idx][d_idx]];
                            if (D[np1] < current_d) {
                                candidates.push_back({m, (d_idx == 0) ? -1 : 1, np1});
                            }
                            int idx_np1 = next_idx[idx][d_idx];
                            int np2 = belt_pos[m][next_idx[idx_np1][d_idx]];
                            if (D[np2] < current_d) {
                                candidates.push_back({m, (d_idx == 0) ? -1 : 1, np1});
                            }
                        }
                    }
                }

                if (candidates.empty() || ops.size() >= 100000) break;

                int sel = (loop_cnt == 0) ? 0 : (rng() % candidates.size());
                auto& choice = candidates[sel];
                ops.push_back({choice.m, choice.dir});
                
                int m = choice.m;
                int dir_idx = (choice.dir == -1) ? 0 : 1;
                int16_t tmp[40];
                for(int i = 0; i < 40; i++) tmp[i] = temp_board[belt_pos[m][i]];
                for(int i = 0; i < 40; i++) {
                    temp_board[belt_pos[m][next_idx[i][dir_idx]]] = tmp[i];
                }
                
                for(int p = 0; p < 400; p++) {
                    if (temp_board[p] == 0) { p0 = p; break; }
                }
            }

            if (temp_board[goal_p] == 0) {
                temp_board[goal_p] = -1;
                initial_min_box = 1;
            }
            k_start = 1;
        }

        // --- 箱 k 毎のループ ---
        for (int k = k_start; k < 400; ++k) {
            if (k % 5 == 0) {
                auto check_time = chrono::steady_clock::now();
                if (chrono::duration<double>(check_time - start_time).count() > TIME_LIMIT) {
                    timeout_flag = true;
                    break;
                }
            }

            int T_initial = ops.size();
            if (T_initial >= MAX_T - 50) break; 
            
            State current_board = initial_board;
            int16_t current_min_box = 0;
            if (current_board[goal_p] == 0) {
                current_board[goal_p] = -1;
                current_min_box = 1;
            }
            
            int pk_initial = -1;
            for(int p = 0; p < 400; p++) {
                if (current_board[p] == k) pk_initial = p;
            }
            if (pk_initial == -1) continue;

            auto calc_valid = [&](const State& b) {
                uint32_t mask = (1U << 20) - 1;
                for(int p = 0; p < 400; p++) {
                    if (b[p] >= 0 && b[p] < k) {
                        mask &= ~p_to_m_mask[p];
                    }
                }
                return mask;
            };
            
            valid_mask[0] = calc_valid(current_board);

            for(int t = 0; t < T_initial; t++) {
                int m = ops[t].first;
                int dir_idx = (ops[t].second == -1) ? 0 : 1;
                
                int16_t tmp[40];
                for(int i = 0; i < 40; i++) tmp[i] = current_board[belt_pos[m][i]];
                for(int i = 0; i < 40; i++) {
                    current_board[belt_pos[m][next_idx[i][dir_idx]]] = tmp[i];
                }
                if (current_board[goal_p] == current_min_box) {
                    current_board[goal_p] = -1;
                    current_min_box++;
                }
                valid_mask[t+1] = calc_valid(current_board);
            }

            // --- 高速化: 10回連続で動かせない区間があるか後ろから事前に走査 ---
            int min_valid_t = 0;
            int consecutive_zeros = 0;
            for (int t = T_initial; t >= 0; --t) {
                if (valid_mask[t] == 0) {
                    consecutive_zeros++;
                    if (consecutive_zeros >= 100) {
                        min_valid_t = t + 1; // これより過去 (t以下) へは探索を広げない
                        break;
                    }
                } else {
                    consecutive_zeros = 0;
                }
            }

            current_ver++; 
            bq_clear();
            
            set_dist(0, pk_initial, 0);
            bq_push(0, 0, pk_initial);

            while(!bq_empty()) {
                StateNode node = bq_pop();
                int t = node.t;
                int p = node.p;
                int d_curr = node.d;
                
                if (d_curr > get_dist(t, p)) continue; 
                if (t == T_initial) continue;
                
                int m_op = ops[t].first;
                int dir_idx = (ops[t].second == -1) ? 0 : 1;
                int np_flow = p;
                int idx = b_idx[m_op][p];
                if(idx != -1) np_flow = belt_pos[m_op][next_idx[idx][dir_idx]];
                
                if (d_curr < get_dist(t+1, np_flow)) {
                    set_dist(t+1, np_flow, d_curr);
                    parent_is_ins[t+1][np_flow] = false;
                    parent_p[t+1][np_flow] = p;
                    bq_push(d_curr, t+1, np_flow);
                }
                
                // 事前に計算した打ち切り境界点(min_valid_t)より過去への挿入遷移は行わない
                if (t < min_valid_t) continue;

                uint32_t v_mask = valid_mask[t];
                if (v_mask == 0) continue; // そもそもこのターン動かせるコンベアがない

                for(int m = 0; m < 20; m++) {
                    if (!(v_mask & (1U << m))) continue;
                    int idx_m = b_idx[m][p];
                    if (idx_m == -1) continue;
                    
                    for(int d_idx = 0; d_idx < 2; d_idx++) {
                        int np1 = belt_pos[m][next_idx[idx_m][d_idx]];
                        if (d_curr + 1 < get_dist(t, np1)) {
                            set_dist(t, np1, d_curr + 1);
                            parent_is_ins[t][np1] = true;
                            parent_p[t][np1] = p;
                            parent_m[t][np1] = m;
                            parent_d[t][np1] = d_idx;
                            bq_push(d_curr + 1, t, np1);
                        }
                        
                        int idx_np1 = next_idx[idx_m][d_idx];
                        int np2 = belt_pos[m][next_idx[idx_np1][d_idx]];
                        
                        if (d_curr + 2 < get_dist(t, np2)) {
                            if (get_dist(t, np1) > d_curr + 1) {
                                set_dist(t, np1, d_curr + 1);
                                parent_is_ins[t][np1] = true;
                                parent_p[t][np1] = p;
                                parent_m[t][np1] = m;
                                parent_d[t][np1] = d_idx;
                            }
                            set_dist(t, np2, d_curr + 2);
                            parent_is_ins[t][np2] = true;
                            parent_p[t][np2] = np1; 
                            parent_m[t][np2] = m;
                            parent_d[t][np2] = d_idx;
                            bq_push(d_curr + 2, t, np2);
                        }
                    }
                }
            }

            int best_p = -1;
            int min_total_cost = INF;
            for(int p = 0; p < 400; p++) {
                int final_dist = get_dist(T_initial, p);
                if (final_dist != INF && final_dist + D[p] <= min_total_cost) {
                    min_total_cost = final_dist + D[p];
                    best_p = p;
                }
            }

            if (best_p == -1) continue;

            vector<tuple<int, int, int>> insertions;
            int curr_t = T_initial;
            int curr_p = best_p;
            
            while(curr_t > 0 || curr_p != pk_initial) {
                bool is_ins = parent_is_ins[curr_t][curr_p];
                int pt = is_ins ? curr_t : curr_t - 1;
                int pp = parent_p[curr_t][curr_p];
                
                if (is_ins) {
                    int m = parent_m[curr_t][curr_p];
                    int d = (parent_d[curr_t][curr_p] == 0) ? -1 : 1;
                    insertions.push_back({curr_t, m, d});
                }
                curr_t = pt;
                curr_p = pp;
            }
            reverse(insertions.begin(), insertions.end()); 

            vector<pair<int, int>> new_ops;
            new_ops.reserve(T_initial + insertions.size() + D[best_p] + 10);
            int ins_idx = 0;
            
            for(int t = 0; t < T_initial; t++) {
                while(ins_idx < (int)insertions.size() && get<0>(insertions[ins_idx]) == t) {
                    new_ops.push_back({get<1>(insertions[ins_idx]), get<2>(insertions[ins_idx])});
                    ins_idx++;
                }
                new_ops.push_back(ops[t]);
            }
            
            while(ins_idx < (int)insertions.size() && get<0>(insertions[ins_idx]) == T_initial) {
                new_ops.push_back({get<1>(insertions[ins_idx]), get<2>(insertions[ins_idx])});
                ins_idx++;
            }
            
            int p = best_p;
            while(p != goal_p) {
                if (new_ops.size() >= 100000) break;
                int cd = D[p];
                bool found = false;
                for(int m = 0; m < 20; m++) {
                    int idx = b_idx[m][p];
                    if (idx != -1) {
                        for(int d_idx = 0; d_idx < 2; d_idx++) {
                            int np = belt_pos[m][next_idx[idx][d_idx]];
                            if (D[np] < cd) {
                                new_ops.push_back({m, (d_idx == 0) ? -1 : 1});
                                p = np;
                                found = true;
                                break;
                            }
                        }
                    }
                    if (found) break;
                }
            }
            
            ops = std::move(new_ops);
        }

        if (timeout_flag) break;

        if(ops.size() > 100000) ops.resize(100000);

        int current_score = 100000 - ops.size(); 
        if (best_ops.empty() || current_score > best_score) {
            best_score = current_score;
            best_ops = ops;
        }

        loop_cnt++;
    }

    if (best_ops.empty()) {
        best_ops.push_back({0, 1});
    }

    cout << 20 << "\n";
    for(int m = 0; m < 20; m++) {
        cout << 40;
        for(int i = 0; i < 40; i++) {
            int p = belt_pos[m][i];
            cout << " " << p / 20 << " " << p % 20;
        }
        cout << "\n";
    }
    cout << best_ops.size() << "\n";
    for(auto op : best_ops) {
        cout << op.first << " " << op.second << "\n";
    }

    return 0;
}