#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>

using namespace std;

const int N = 200;
long long original_A[N][N];
long long A[N][N];

// --- 超高速乱数生成 ---
inline unsigned int xorshift() {
    static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned int t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

// 焼きなまし用の対数テーブル（exp関数の代わりに使用して超高速化）
double log_table[65536];
void init_log_table() {
    for (int i = 0; i < 65536; ++i) {
        log_table[i] = log((i + 0.5) / 65536.0);
    }
}

// 1次元座標での8方向隣接チェック（インライン展開で爆速化）
inline bool is_adj_1D(int id1, int id2) {
    int dr = abs(id1 / 200 - id2 / 200);
    int dc = abs(id1 % 200 - id2 % 200);
    return dr <= 1 && dc <= 1;
}

// --- 盤面の座標系変換 ---
void load_A_mode(int mode) {
    for(int r=0; r<N; ++r) {
        for(int c=0; c<N; ++c) {
            int nr = r, nc = c;
            if (mode & 1) nr = N - 1 - nr;
            if (mode & 2) nc = N - 1 - nc;
            if (mode & 4) swap(nr, nc);
            A[r][c] = original_A[nr][nc];
        }
    }
}

inline pair<int, int> get_original_coord(int r, int c, int mode) {
    int nr = r, nc = c;
    if (mode & 1) nr = N - 1 - nr;
    if (mode & 2) nc = N - 1 - nc;
    if (mode & 4) swap(nr, nc);
    return {nr, nc};
}

// --- DP用Uターンの固定ジオメトリ ---
vector<pair<int,int>> get_bottom_uturn_cells(int b, int c) {
    int C = 4 * b;
    if (c == 3) return {{196,C+3}, {196,C+4}};
    if (c == 2) return {{196,C+2}, {197,C+2}, {197,C+3}, {197,C+4}, {197,C+5}, {196,C+5}};
    if (c == 1) return {{196,C+1}, {197,C+1}, {198,C+1}, {198,C+2}, {198,C+3}, {198,C+4}, {198,C+5}, {198,C+6}, {197,C+6}, {196,C+6}};
    if (c == 0) return {{196,C+0}, {197,C+0}, {198,C+0}, {199,C+0}, {199,C+1}, {199,C+2}, {199,C+3}, {199,C+4}, {199,C+5}, {199,C+6}, {199,C+7}, {198,C+7}, {197,C+7}, {196,C+7}};
    return {};
}

vector<pair<int,int>> get_top_uturn_cells(int b, int c) {
    int C = 4 * b;
    if (c == 3) return {{3,C+3}, {3,C+4}};
    if (c == 2) return {{3,C+2}, {2,C+2}, {2,C+3}, {2,C+4}, {2,C+5}, {3,C+5}};
    if (c == 1) return {{3,C+1}, {2,C+1}, {1,C+1}, {1,C+2}, {1,C+3}, {1,C+4}, {1,C+5}, {1,C+6}, {2,C+6}, {3,C+6}};
    if (c == 0) return {{3,C+0}, {2,C+0}, {1,C+0}, {0,C+0}, {0,C+1}, {0,C+2}, {0,C+3}, {0,C+4}, {0,C+5}, {0,C+6}, {0,C+7}, {1,C+7}, {2,C+7}, {3,C+7}};
    return {};
}

// --- グローバルDP ---
vector<pair<int, int>> build_dp_path() {
    vector<vector<int>> perms;
    vector<int> p = {0, 1, 2, 3};
    do { perms.push_back(p); } while (next_permutation(p.begin(), p.end()));

    vector<vector<int>> valid_trans(24);
    for (int i = 0; i < 24; ++i) {
        for (int j = 0; j < 24; ++j) {
            bool ok = true;
            for (int k = 0; k < 4; ++k) if (abs(perms[i][k] - perms[j][k]) > 1) ok = false;
            if (ok) valid_trans[i].push_back(j);
        }
    }

    auto get_perm_idx = [&](const vector<int>& v) {
        for(int i=0; i<24; ++i) if(perms[i] == v) return i;
        return 0;
    };

    int TOTAL_STEPS = 50 * 191 + 49;
    vector<vector<long long>> dp(TOTAL_STEPS + 1, vector<long long>(24, -1e18));
    vector<vector<int>> parent(TOTAL_STEPS + 1, vector<int>(24, -1));

    long long start_score = 0;
    for(int i = 0; i < 4; ++i) {
        for(int r = 0; r <= 4; ++r) {
            double temp_s = (double)r;
            long long temp_w[4] = { (long long)round(temp_s), (long long)round(19999.0 - temp_s), (long long)round(20000.0 + temp_s), (long long)round(39999.0 - temp_s) };
            start_score += temp_w[i] * A[r][i];
        }
    }
    dp[0][0] = start_score;

    int step = 0;
    for (int b = 0; b < 50; ++b) {
        int C = 4 * b;
        for (int k = 0; k < 191; ++k) {
            int r_to = (b % 2 == 0) ? (5 + k) : (194 - k);
            double s = 5.0 + (double)step * 9995.0 / TOTAL_STEPS;
            long long w[4] = { (long long)round(s), (long long)round(19999.0 - s), (long long)round(20000.0 + s), (long long)round(39999.0 - s) };

            for (int curr_p = 0; curr_p < 24; ++curr_p) {
                if (dp[step][curr_p] < 0) continue;
                for (int next_p : valid_trans[curr_p]) {
                    long long cost = 0;
                    for (int th = 0; th < 4; ++th) cost += w[th] * A[r_to][C + perms[next_p][th]];
                    if (dp[step][curr_p] + cost > dp[step+1][next_p]) {
                        dp[step+1][next_p] = dp[step][curr_p] + cost;
                        parent[step+1][next_p] = curr_p;
                    }
                }
            }
            step++;
        }
        
        if (b < 49) {
            double s = 5.0 + (double)step * 9995.0 / TOTAL_STEPS;
            long long w[4] = { (long long)round(s), (long long)round(19999.0 - s), (long long)round(20000.0 + s), (long long)round(39999.0 - s) };

            for (int curr_p = 0; curr_p < 24; ++curr_p) {
                if (dp[step][curr_p] < 0) continue;
                vector<int> np_vec(4);
                for(int th=0; th<4; ++th) np_vec[th] = 3 - perms[curr_p][th];
                int next_p = get_perm_idx(np_vec);
                
                long long cost = 0;
                for(int th=0; th<4; ++th) {
                    auto cells = (b % 2 == 0) ? get_bottom_uturn_cells(b, perms[curr_p][th]) : get_top_uturn_cells(b, perms[curr_p][th]);
                    for(auto cell : cells) cost += w[th] * A[cell.first][cell.second];
                    int r_next_start = (b % 2 == 0) ? 195 : 4;
                    cost += w[th] * A[r_next_start][C + 4 + np_vec[th]];
                }
                
                if (dp[step][curr_p] + cost > dp[step+1][next_p]) {
                    dp[step+1][next_p] = dp[step][curr_p] + cost;
                    parent[step+1][next_p] = curr_p;
                }
            }
            step++;
        }
    }

    long long best_val = -1e18;
    int best_final_p = -1;
    for (int p = 0; p < 24; ++p) {
        if (dp[step][p] < 0) continue;
        long long cost = 0;
        double s = 10000.0;
        long long w[4] = { (long long)s, (long long)(19999.0 - s), (long long)(20000.0 + s), (long long)(39999.0 - s) };
        for(int th=0; th<4; ++th) for(int r=3; r>=0; --r) cost += w[th] * A[r][196 + perms[p][th]];
        dp[step][p] += cost;
        
        if (abs(perms[p][0] - perms[p][1]) <= 1 && abs(perms[p][2] - perms[p][3]) <= 1) {
            if (dp[step][p] > best_val) { best_val = dp[step][p]; best_final_p = p; }
        }
    }

    vector<int> best_perms(TOTAL_STEPS + 1);
    int curr_p = best_final_p;
    for (int i = TOTAL_STEPS; i >= 0; --i) {
        best_perms[i] = curr_p;
        if(i > 0) curr_p = parent[i][curr_p];
    }

    vector<pair<int,int>> P[4];
    for(int th=0; th<4; ++th) for(int r=0; r<=4; ++r) P[th].push_back({r, th});

    int curr_step = 0;
    for (int b = 0; b < 50; ++b) {
        int C = 4 * b;
        for (int k = 0; k < 191; ++k) {
            int r_to = (b % 2 == 0) ? (5 + k) : (194 - k);
            int p = best_perms[curr_step + 1];
            for(int th=0; th<4; ++th) P[th].push_back({r_to, C + perms[p][th]});
            curr_step++;
        }
        if (b < 49) {
            int p = best_perms[curr_step];
            int np = best_perms[curr_step + 1];
            for(int th=0; th<4; ++th) {
                auto cells = (b % 2 == 0) ? get_bottom_uturn_cells(b, perms[p][th]) : get_top_uturn_cells(b, perms[p][th]);
                for(auto cell : cells) P[th].push_back(cell);
                int r_next = (b % 2 == 0) ? 195 : 4;
                P[th].push_back({r_next, C + 4 + perms[np][th]});
            }
            curr_step++;
        }
    }

    int final_p = best_perms[curr_step];
    for(int th=0; th<4; ++th) {
        int col = 196 + perms[final_p][th];
        for(int r=3; r>=0; --r) P[th].push_back({r, col});
    }

    vector<pair<int,int>> final_path = P[0];
    reverse(P[1].begin(), P[1].end()); final_path.insert(final_path.end(), P[1].begin(), P[1].end());
    final_path.insert(final_path.end(), P[2].begin(), P[2].end());
    reverse(P[3].begin(), P[3].end()); final_path.insert(final_path.end(), P[3].begin(), P[3].end());

    return final_path;
}

// スコア計算
long long calc_score(const vector<pair<int, int>>& path) {
    long long V = 0;
    for (int k = 0; k < N * N; ++k) V += (long long)k * A[path[k].first][path[k].second];
    return V;
}

// --- 超高速 焼きなまし法 ---
void simulated_annealing(vector<pair<int, int>>& path, double time_limit_for_this_run, auto run_start_time) {
    // 1次元配列化 (キャッシュ最適化)
    int path1D[40000];
    int pos_in_path[40000];
    long long A_1D[40000];

    for (int k = 0; k < 40000; ++k) {
        int id = path[k].first * 200 + path[k].second;
        path1D[k] = id;
        pos_in_path[id] = k;
    }
    for (int r = 0; r < 200; ++r) {
        for (int c = 0; c < 200; ++c) A_1D[r * 200 + c] = A[r][c];
    }

    // ★ここで温度を調整できます
    double start_temp = 1500000.0; 
    double end_temp = 10.0;
    double current_temp = start_temp;

    int iter_count = 0;
    int accepted_count = 0;

    long long current_score = calc_score(path);
    long long best_score = current_score;
    int best_path1D[40000];
    copy(path1D, path1D + 40000, best_path1D);

    const int d_row[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int d_col[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    while (true) {
        // 時刻チェックの頻度を下げて高速化 (8191回に1度)
        if ((iter_count & 8191) == 0) { 
            auto now = chrono::high_resolution_clock::now();
            double elapsed = chrono::duration<double>(now - run_start_time).count();
            if (elapsed > time_limit_for_this_run) break;
            
            double progress = elapsed / time_limit_for_this_run;
            current_temp = start_temp * pow(end_temp / start_temp, progress);
        }
        iter_count++;

        // ビット演算で乱数分岐（モジュロ演算より高速）
        // 10: 1点スワップ, 残り: スマート2-opt
        int op_type = ((xorshift() & 15) < 10) ? 0 : 1; 

        if (op_type == 0) {
            // --- 1. スワップ近傍 ---
            int i = xorshift() % 40000;
            int id_i = path1D[i];
            int r = id_i / 200, c = id_i % 200;
            int dr = (xorshift() % 7) - 3, dc = (xorshift() % 7) - 3;
            int nr = r + dr, nc = c + dc;
            
            if (nr < 0 || nr >= 200 || nc < 0 || nc >= 200) continue;
            int id_j = nr * 200 + nc;
            int j = pos_in_path[id_j];
            if (i == j) continue;
            if (i > j) swap(i, j);

            int pi = path1D[i], pj = path1D[j];
            bool ok = true;
            if (j - i == 1) {
                if (i > 0 && !is_adj_1D(path1D[i-1], pj)) ok = false;
                if (!is_adj_1D(pj, pi)) ok = false;
                if (j < 39999 && !is_adj_1D(pi, path1D[j+1])) ok = false;
            } else {
                if (i > 0 && !is_adj_1D(path1D[i-1], pj)) ok = false;
                if (i < 39999 && !is_adj_1D(pj, path1D[i+1])) ok = false;
                if (j > 0 && !is_adj_1D(path1D[j-1], pi)) ok = false;
                if (j < 39999 && !is_adj_1D(pi, path1D[j+1])) ok = false;
            }

            if (ok) {
                long long delta = (long long)(j - i) * (A_1D[pi] - A_1D[pj]);
                // 超高速 対数テーブルによるSA判定
                long long threshold = (delta >= 0) ? 0 : (long long)(current_temp * log_table[xorshift() & 65535]);
                if (delta >= threshold) {
                    swap(path1D[i], path1D[j]);
                    pos_in_path[pj] = i;
                    pos_in_path[pi] = j;
                    current_score += delta;
                    accepted_count++;
                    if (current_score > best_score) { 
                        best_score = current_score; 
                        copy(path1D, path1D + 40000, best_path1D);
                    }
                }
            }
        } 
        else {
            // --- 2. スマート2-opt ---
            int i = 1 + (xorshift() % 39997);
            int p_prev = path1D[i-1];
            
            int dir = xorshift() % 8;
            int nr = (p_prev / 200) + d_row[dir];
            int nc = (p_prev % 200) + d_col[dir];
            
            if (nr >= 0 && nr < 200 && nc >= 0 && nc < 200) {
                int id_j = nr * 200 + nc;
                int j = pos_in_path[id_j];
                
                if (j > i && j - i <= 1000 && j < 39999) {
                    if (is_adj_1D(path1D[i], path1D[j+1])) {
                        long long delta = 0;
                        int* p_front = &path1D[i];
                        int* p_back = &path1D[j];
                        int dist = j - i;
                        
                        // ポインタ演算による超高速差分計算ループ
                        while (p_front < p_back) {
                            delta += (long long)dist * (A_1D[*p_front] - A_1D[*p_back]);
                            p_front++;
                            p_back--;
                            dist -= 2;
                        }

                        // 超高速 対数テーブルによるSA判定
                        long long threshold = (delta >= 0) ? 0 : (long long)(current_temp * log_table[xorshift() & 65535]);
                        if (delta >= threshold) {
                            reverse(path1D + i, path1D + j + 1);
                            for (int k = i; k <= j; ++k) pos_in_path[path1D[k]] = k;
                            current_score += delta;
                            accepted_count++;
                            if (current_score > best_score) { 
                                best_score = current_score; 
                                copy(path1D, path1D + 40000, best_path1D);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 1次元配列から元の形式に復元
    for (int k = 0; k < 40000; ++k) {
        path[k] = {best_path1D[k] / 200, best_path1D[k] % 200};
    }
    cerr << "SA Iterations: " << iter_count << " | Accepted: " << accepted_count << "\n";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    init_log_table(); // SA用テーブルの初期化

    auto total_start_time = chrono::high_resolution_clock::now();

    int n_input;
    if (!(cin >> n_input)) return 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cin >> original_A[i][j];
        }
    }

    struct Candidate {
        int mode;
        vector<pair<int, int>> path;
        long long score;
    };
    vector<Candidate> candidates;

    // 全8パターンのDPを実行
    for (int mode = 0; mode < 8; ++mode) {
        load_A_mode(mode);
        vector<pair<int, int>> dp_path = build_dp_path();
        candidates.push_back({mode, dp_path, calc_score(dp_path)});
    }

    // スコア降順ソート
    sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        return a.score > b.score;
    });

    // ★ここで上位n個の初期解に対するマルチスタートを設定できます
    int num_sa_runs = 2; // 例: 上位2個の解を焼きなます
    num_sa_runs = min(num_sa_runs, (int)candidates.size());

    long long absolute_best_V = -1;
    vector<pair<int, int>> absolute_best_path;
    int absolute_best_mode = -1;

    // 上位 n 個に対して時間を分割して焼きなまし法を実行
    for (int i = 0; i < num_sa_runs; ++i) {
        int mode = candidates[i].mode;
        vector<pair<int, int>> path = candidates[i].path;
        load_A_mode(mode);

        auto now = chrono::high_resolution_clock::now();
        double elapsed_so_far = chrono::duration<double>(now - total_start_time).count();
        // 最大2.90秒から経過時間を引き、残りの候補数で均等に割る
        double time_for_this_run = (2.97 - elapsed_so_far) / (num_sa_runs - i);

        if (time_for_this_run > 0.05) { // 少なくとも0.05秒は残っている場合のみ実行
            cerr << "--- Running SA for Mode " << mode << " (" << time_for_this_run << " sec) ---\n";
            simulated_annealing(path, time_for_this_run, chrono::high_resolution_clock::now());
        }

        long long final_V = calc_score(path);
        if (final_V > absolute_best_V) {
            absolute_best_V = final_V;
            absolute_best_path = path;
            absolute_best_mode = mode;
        }
    }

    long long final_score = round((double)absolute_best_V / (N * N));

    cerr << "=================================\n";
    cerr << "Absolutely Best Mode: " << absolute_best_mode << "\n";
    cerr << "Final Score: " << final_score << "\n";
    cerr << "=================================\n";

    // オリジナル座標系で出力
    for (const auto& p : absolute_best_path) {
        auto orig = get_original_coord(p.first, p.second, absolute_best_mode);
        cout << orig.first << " " << orig.second << "\n";
    }

    return 0;
}