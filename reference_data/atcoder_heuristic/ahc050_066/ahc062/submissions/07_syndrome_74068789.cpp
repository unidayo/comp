// (◕ᴗ◕✿)

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
using namespace std;

// 高速な状態保持付き Xorshift 32ビット乱数ジェネレータ
uint32_t xor_x = 123456789, xor_y = 362436069, xor_z = 521288629, xor_w = 88675123;
inline uint32_t next_rand() {
    uint32_t t = xor_x ^ (xor_x << 11);
    xor_x = xor_y; xor_y = xor_z; xor_z = xor_w;
    return xor_w = (xor_w ^ (xor_w >> 19)) ^ (t ^ (t >> 8));
}

// Lemireの高速剰余演算: [0, range) の整数を返す
inline uint32_t rand_range(uint32_t range) {
    return (next_rand() * (uint64_t)range) >> 32;
}

struct Timer {
    public:
        Timer(int limit){
            start = chrono::high_resolution_clock::now();
            goal = start + chrono::milliseconds(limit);
        }
        inline int get_time(){return (chrono::high_resolution_clock::now() - start).count() / 1e6;}

    private:
        chrono::high_resolution_clock::time_point start;
        chrono::high_resolution_clock::time_point goal;  
};

// 制限時間を2秒に設定 (通信などのマージンを考慮して1980ms)
constexpr int TIME_LIMIT = 1980;
constexpr int N = 200;
constexpr int K = 6;
constexpr long long INF = 1e18;
constexpr int LOG_TABLE_SIZE = 65536;

struct Solver{
    vector<vector<long long>> original_A;
    vector<vector<long long>> A;
    vector<pair<int, int>> final_path;
    vector<pair<int, int>> best_path;
    long long best_score = -1;

    vector<vector<int>> perms;
    vector<vector<int>> swaps;
    int transition[720][13];
    int start_perm_idx;
    int goal_perm_idx;

    long long dp[N][720];
    int prev_id[N][720];

    double log_table[LOG_TABLE_SIZE];
    
    // 高速化用の隣接リスト
    int adj_deg[N * N];
    int adj_list[N * N][8];

    void input(){
        int n_input;
        cin >> n_input;
        original_A.assign(N, vector<long long>(N));
        for(int i = 0; i < N; ++i){
            for(int j = 0; j < N; ++j){
                cin >> original_A[i][j];
            }
        }
    }

    int perm_to_id(const vector<int>& p) {
        return lower_bound(perms.begin(), perms.end(), p) - perms.begin();
    }

    void init_dp() {
        vector<int> p = {0, 1, 2, 3, 4, 5};
        do {
            perms.push_back(p);
        } while (next_permutation(p.begin(), p.end()));

        for(int mask = 0; mask < (1 << 5); ++mask) {
            bool ok = true;
            for(int i = 0; i < 4; ++i) {
                if(((mask >> i) & 1) && ((mask >> (i + 1)) & 1)) {
                    ok = false;
                    break;
                }
            }
            if(ok) {
                vector<int> s = {0, 1, 2, 3, 4, 5};
                for(int i = 0; i < 5; ++i) {
                    if((mask >> i) & 1) swap(s[i], s[i+1]);
                }
                swaps.push_back(s);
            }
        }

        for(int i = 0; i < 720; ++i) {
            for(int j = 0; j < 13; ++j) {
                vector<int> p_new(6);
                for(int r = 0; r < 6; ++r) p_new[r] = perms[i][swaps[j][r]];
                transition[i][j] = perm_to_id(p_new);
            }
        }

        start_perm_idx = perm_to_id({1, 0, 3, 2, 5, 4}); 
        goal_perm_idx = perm_to_id({0, 1, 2, 3, 4, 5});
    }

    void init_sa_tables() {
        double n = 1.0 / (2.0 * LOG_TABLE_SIZE);
        for (int i = 0; i < LOG_TABLE_SIZE; ++i) {
            log_table[i] = log(((double)i / LOG_TABLE_SIZE) + n);
        }
        
        // グリッドグラフの隣接関係を事前計算し、SAループ内の境界チェックを完全排除
        const int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        const int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        for(int r = 0; r < N; ++r) {
            for(int c = 0; c < N; ++c) {
                int u = r * N + c;
                adj_deg[u] = 0;
                for(int i = 0; i < 8; ++i) {
                    int nr = r + dr[i], nc = c + dc[i];
                    if(nr >= 0 && nr < N && nc >= 0 && nc < N) {
                        adj_list[u][adj_deg[u]++] = nr * N + nc;
                    }
                }
            }
        }
    }

    inline long long get_step(int logical_id, int c, int b) {
        bool is_even_b = (b % 2 == 0);
        bool is_outward = (is_even_b && (logical_id == 0 || logical_id == 3 || logical_id == 4)) || 
                          (!is_even_b && (logical_id == 1 || logical_id == 2 || logical_id == 5));
        
        int global_pair_idx = b * 3 + (logical_id / 2);
        bool l_to_r = (global_pair_idx % 2 == 0);
        int cell_offset_in_track = l_to_r ? c : (N - 1 - c);
        
        if (is_outward) {
            int order_in_b = 0;
            if (is_even_b) {
                if (logical_id == 0) order_in_b = 0;
                else if (logical_id == 3) order_in_b = 1;
                else if (logical_id == 4) order_in_b = 2;
            } else {
                if (logical_id == 1) order_in_b = 0;
                else if (logical_id == 2) order_in_b = 1;
                else if (logical_id == 5) order_in_b = 2;
            }
            return (long long)(b * 3 + order_in_b) * N + cell_offset_in_track;
        } else {
            int order_in_b = 0;
            if (is_even_b) {
                if (logical_id == 5) order_in_b = 0;
                else if (logical_id == 2) order_in_b = 1;
                else if (logical_id == 1) order_in_b = 2;
            } else {
                if (logical_id == 4) order_in_b = 0;
                else if (logical_id == 3) order_in_b = 1;
                else if (logical_id == 0) order_in_b = 2;
            }
            long long return_base_step = 20200LL;
            return return_base_step + (long long)((32 - b) * 3 + order_in_b) * N + cell_offset_in_track;
        }
    }

    void build_path() {
        final_path.clear();
        int num_blocks = N / K;
        vector<pair<int, int>> global_outward;
        vector<pair<int, int>> global_return;

        for(int b = 0; b < num_blocks; ++b) {
            int r_base = b * K;

            for(int c = 0; c < N; ++c) {
                for(int i = 0; i < 720; ++i) dp[c][i] = -INF;
            }

            long long initial_cost = 0;
            for(int r = 0; r < K; ++r) {
                int logical_id = perms[start_perm_idx][r];
                initial_cost += get_step(logical_id, N - 1, b) * A[r_base + r][N - 1];
            }
            dp[N - 1][start_perm_idx] = initial_cost;

            for(int c = N - 2; c >= 0; --c) {
                long long step_vals[6];
                for(int logical_id = 0; logical_id < K; ++logical_id) {
                    step_vals[logical_id] = get_step(logical_id, c, b);
                }
                
                long long cost_table[720];
                for (int next_id = 0; next_id < 720; ++next_id) {
                    long long cost = 0;
                    for(int r = 0; r < K; ++r) {
                        cost += step_vals[perms[next_id][r]] * A[r_base + r][c];
                    }
                    cost_table[next_id] = cost;
                }

                for(int id = 0; id < 720; ++id) {
                    if(dp[c + 1][id] <= -INF / 2) continue;
                    long long current_dp = dp[c + 1][id];
                    
                    for(int f = 0; f < 13; ++f) {
                        int next_id = transition[id][f];
                        long long new_cost = current_dp + cost_table[next_id];
                        
                        if(dp[c][next_id] < new_cost) {
                            dp[c][next_id] = new_cost;
                            prev_id[c][next_id] = id;
                        }
                    }
                }
            }

            vector<int> best_ids(N);
            int curr_id = goal_perm_idx;
            best_ids[0] = curr_id;
            for(int c = 0; c < N - 1; ++c) {
                curr_id = prev_id[c][curr_id];
                best_ids[c + 1] = curr_id;
            }

            vector<vector<pair<int, int>>> block_paths(K);
            for(int c = 0; c < N; ++c) {
                for(int r = 0; r < K; ++r) {
                    int logical_id = perms[best_ids[c]][r];
                    block_paths[logical_id].push_back({r_base + r, c});
                }
            }

            for(int logical_id = 0; logical_id < K; ++logical_id) {
                int global_pair_idx = b * 3 + (logical_id / 2);
                bool l_to_r = (global_pair_idx % 2 == 0);
                
                vector<pair<int, int>> p = block_paths[logical_id];
                if (!l_to_r) reverse(p.begin(), p.end());

                if ((b % 2 == 0 && (logical_id == 0 || logical_id == 3 || logical_id == 4)) || (b % 2 && (logical_id == 1 || logical_id == 2 || logical_id == 5))) {
                    global_outward.insert(global_outward.end(), p.begin(), p.end());
                } else {
                    global_return.insert(global_return.begin(), p.begin(), p.end());
                }
            }
        }

        vector<pair<int, int>> u_turn_path;
        for(int c = N - 1; c >= 1; c -= 2){
            u_turn_path.push_back({198, c});
            u_turn_path.push_back({199, c});
            u_turn_path.push_back({199, c - 1});
            u_turn_path.push_back({198, c - 1});
        }
        
        final_path.insert(final_path.end(), global_outward.begin(), global_outward.end());
        final_path.insert(final_path.end(), u_turn_path.begin(), u_turn_path.end());
        final_path.insert(final_path.end(), global_return.begin(), global_return.end());
    }

    void sa(Timer& timer) {
        int n2 = N * N;
        
        vector<int> path(n2);
        vector<long long> A_flat(n2);
        for (int i = 0; i < n2; ++i) {
            int r = best_path[i].first;
            int c = best_path[i].second;
            int idx = r * N + c;
            path[i] = idx;
            A_flat[idx] = original_A[r][c];
        }
        
        vector<int> pos(n2);
        for (int i = 0; i < n2; ++i) pos[path[i]] = i;

        long long current_score = 0;
        for (int i = 0; i < n2; ++i) current_score += (long long)i * A_flat[path[i]];
        long long global_best_score = current_score;
        vector<int> global_best_P = path;

        double T0 = 8e6; 
        double T1 = 1e2; 
        double temp_diff = T0 - T1; 
        int iter = 0;

        // O(1) 隣接判定関数 (定数200での除算・剰余はコンパイラにより高速化される)
        auto is_adj = [](int u, int v) {
            int r1 = u / 200, c1 = u % 200;
            int r2 = v / 200, c2 = v % 200;
            return abs(r1 - r2) <= 1 && abs(c1 - c2) <= 1;
        };

        double temp = T0;
        const long long* A_ptr = A_flat.data();
        int* path_ptr = path.data();
        
        while (true) {
            // タイマーチェックの頻度を下げて計測オーバーヘッドを減少
            if ((iter & 1023) == 0) {
                int elapsed = timer.get_time();
                if (elapsed >= TIME_LIMIT) break;
                double rate = (double)elapsed / TIME_LIMIT;
                temp = T0 - rate * temp_diff; 
            }
            iter++;
            
            int type = rand_range(3);

            if (type == 0) {
                // 【2-opt】
                int i = rand_range(n2 - 1);
                int u = path_ptr[i];
                int deg = adj_deg[u];
                int start_d = rand_range(deg);
                
                int a = -1, b = -1;
                for(int d = 0; d < deg; ++d) {
                    int v = adj_list[u][start_d];
                    start_d++; if (start_d == deg) start_d = 0;

                    int j = pos[v];
                    if (i == j || i + 1 == j || i == j + 1) continue;
                    
                    int c_a = i < j ? i + 1 : j + 1;
                    int c_b = i < j ? j : i;
                    if (c_b + 1 == n2 || is_adj(path_ptr[c_a], path_ptr[c_b + 1])) {
                        a = c_a; b = c_b;
                        if (a < 20000 && b > 20000) continue;
                        break;
                    }
                }
                if (a == -1 || (a < 20000 && b > 20000)) continue;

                // [高速化] ループの反転対称性を利用し、足し算の回数を半分にカット
                long long diff = 0;
                int len = b - a + 1;
                long long cur_weight = len - 1;
                int half = len >> 1;
                
                for (int k = 0; k < half; ++k) {
                    diff += cur_weight * (A_ptr[path_ptr[a + k]] - A_ptr[path_ptr[b - k]]);
                    cur_weight -= 2;
                }

                double threshold = temp * log_table[next_rand() & 65535];
                if ((double)diff >= threshold) {
                    current_score += diff;
                    reverse(path.begin() + a, path.begin() + b + 1);
                    for (int k = a; k <= b; ++k) pos[path_ptr[k]] = k;
                    if (current_score > global_best_score) {
                        global_best_score = current_score;
                        global_best_P = path;
                    }
                }
            } else {
                // 【3-opt】
                int i = rand_range(n2 - 3) + 1;
                int j = -1, k = -1;
                
                int u1 = path_ptr[i-1];
                int deg1 = adj_deg[u1];
                int start_d1 = rand_range(deg1);
                
                for(int d1 = 0; d1 < deg1; ++d1) {
                    int v1 = adj_list[u1][start_d1];
                    start_d1++; if (start_d1 == deg1) start_d1 = 0;
                    
                    int c_j = pos[v1] - 1;
                    if (c_j >= i) {
                        int u2 = path_ptr[i];
                        int deg2 = adj_deg[u2];
                        int start_d2 = rand_range(deg2);
                        
                        for(int d2 = 0; d2 < deg2; ++d2) {
                            int v2 = adj_list[u2][start_d2];
                            start_d2++; if (start_d2 == deg2) start_d2 = 0;
                            
                            int c_k = pos[v2];
                            if (c_k > c_j) {
                                if (c_k + 1 == n2 || is_adj(path_ptr[c_j], path_ptr[c_k + 1])) {
                                    j = c_j; k = c_k;
                                    break;
                                }
                            }
                        }
                        if (j != -1) break;
                    }
                }
                if (j == -1) continue;

                long long L1 = j - i + 1;
                long long L2 = k - j;
                
                // [高速化] 数式変形により掛け算をループ外に出し、純粋な加算ループにしてSIMD最適化を誘発
                long long sum1 = 0;
                #pragma GCC ivdep
                for (int x = i; x <= j; ++x) sum1 += A_ptr[path_ptr[x]];
                
                long long sum2 = 0;
                #pragma GCC ivdep
                for (int x = j + 1; x <= k; ++x) sum2 += A_ptr[path_ptr[x]];
                
                long long diff = L2 * sum1 - L1 * sum2;

                double threshold = temp * log_table[next_rand() & 65535];
                if ((double)diff >= threshold) {
                    current_score += diff;
                    rotate(path.begin() + i, path.begin() + j + 1, path.begin() + k + 1);
                    for (int x = i; x <= k; ++x) pos[path_ptr[x]] = x;
                    if (current_score > global_best_score) {
                        global_best_score = current_score;
                        global_best_P = path;
                    }
                }
            }
        }
        
        for (int i = 0; i < n2; ++i) {
            best_path[i] = {global_best_P[i] / N, global_best_P[i] % N};
        }
        best_score = global_best_score;
        cerr << "SA Iterations: " << iter << "\n";
    }

    void run(){
        Timer timer(TIME_LIMIT);
        input();
        init_dp();
        init_sa_tables();

        // 貪欲法による初期解構築は1方向のみ（元盤面）で実行して時間を節約
        A = original_A;
        build_path();

        long long score = 0;
        vector<pair<int, int>> current_orig_path(N * N);
        for(int step = 0; step < N * N; ++step){
            auto [nr, nc] = final_path[step];
            current_orig_path[step] = {nr, nc};
            score += (long long)step * original_A[nr][nc];
        }

        best_score = score;
        best_path = current_orig_path;

        // 残りの時間をすべて焼きなましに投資（ループ最適化により莫大なイテレーションが回ります）
        sa(timer);

        double final_score_print = (double)best_score / (N * N);
        cerr << "Expected Score: " << round(final_score_print) << "\n";
        cerr << "Execution Time: " << timer.get_time() << " ms\n";

        for(int k = 0; k < N * N; ++k){
            cout << best_path[k].first << " " << best_path[k].second << "\n";
        }
    }
};

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    // SA用の巨大配列を安全に確保するためヒープを使用
    Solver* solver = new Solver();
    solver->run();
    delete solver;
    return 0;
}