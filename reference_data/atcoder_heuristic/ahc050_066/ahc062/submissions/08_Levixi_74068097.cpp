#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;

int N;
vector<vector<int>> A_orig;
vector<vector<int>> perms;

// チェビシェフ距離
inline int chebyshev_dist(pair<int, int> p1, pair<int, int> p2) {
    return max(abs(p1.first - p2.first), abs(p1.second - p2.second));
}

// 2点スワップの合法判定
bool check_swap_valid(int i, int j, const vector<pair<int, int>>& current_ans) {
    if (i > j) swap(i, j);
    int n2 = (int)current_ans.size();
    pair<int, int> pi = current_ans[j]; // i番目に移動させる座標
    pair<int, int> pj = current_ans[i]; // j番目に移動させる座標
    
    // i番目の位置での接続チェック
    if (i > 0 && chebyshev_dist(current_ans[i - 1], pi) > 1) return false;
    pair<int, int> next_i = (i + 1 == j) ? pj : current_ans[i + 1];
    if (chebyshev_dist(pi, next_i) > 1) return false;
    
    // j番目の位置での接続チェック
    pair<int, int> prev_j = (j - 1 == i) ? pi : current_ans[j - 1];
    if (chebyshev_dist(prev_j, pj) > 1) return false;
    if (j < n2 - 1 && chebyshev_dist(pj, current_ans[j + 1]) > 1) return false;
    
    return true;
}

// 座標変換 (8パターン)
pair<int, int> transform_coord(int r, int c, int n, int type) {
    switch (type) {
        case 0: return {r, c};
        case 1: return {c, n - 1 - r};
        case 2: return {n - 1 - r, n - 1 - c};
        case 3: return {n - 1 - c, r};
        case 4: return {r, n - 1 - c};
        case 5: return {n - 1 - r, c};
        case 6: return {c, r};
        case 7: return {n - 1 - c, n - 1 - r};
        default: return {r, c};
    }
}

// --- 角ブロックの前計算 ---
struct CornerRoute {
    int u, v;
    vector<vector<pair<int, int>>> paths;
};
vector<CornerRoute> corner_transitions[4][4];
int req_len[4] = {7, 5, 3, 1};

void dfs(int path_idx, int len, int r, int c, int in_pts[4][2], int out_pts[4][2],
         int used, int in_used, int out_used, vector<pair<int,int>> cur_path[4], 
         vector<CornerRoute>& routes) {
    if (len == req_len[path_idx]) {
        int w_out = -1;
        for (int w = 0; w < 4; ++w) {
            if (!(out_used & (1 << w)) && out_pts[w][0] == r && out_pts[w][1] == c) {
                w_out = w; break;
            }
        }
        if (w_out == -1) return;
        int n_out_used = out_used | (1 << w_out);

        if (path_idx == 3) {
            CornerRoute cr;
            cr.paths.resize(4);
            for (int i = 0; i < 4; ++i) cr.paths[i] = cur_path[i];
            
            vector<int> u_vec(4), v_vec(4);
            for (int i = 0; i < 4; ++i) {
                for (int w = 0; w < 4; ++w) {
                    if (in_pts[w][0] == cr.paths[i][0].first && in_pts[w][1] == cr.paths[i][0].second) u_vec[i] = w;
                    if (out_pts[w][0] == cr.paths[i].back().first && out_pts[w][1] == cr.paths[i].back().second) v_vec[i] = w;
                }
            }
            cr.u = -1; cr.v = -1;
            for (int p = 0; p < 24; ++p) {
                if (perms[p] == u_vec) cr.u = p;
                if (perms[p] == v_vec) cr.v = p;
            }
            if (cr.u != -1 && cr.v != -1) routes.push_back(cr);
        } else {
            for (int w = 0; w < 4; ++w) {
                if (!(in_used & (1 << w))) {
                    int nr = in_pts[w][0], nc = in_pts[w][1];
                    if (!(used & (1 << (nr * 4 + nc)))) {
                        int n_in_used = in_used | (1 << w);
                        int n_used = used | (1 << (nr * 4 + nc));
                        cur_path[path_idx + 1].push_back({nr, nc});
                        dfs(path_idx + 1, 1, nr, nc, in_pts, out_pts, n_used, n_in_used, n_out_used, cur_path, routes);
                        cur_path[path_idx + 1].pop_back();
                    }
                }
            }
        }
        return;
    }

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (nr < 0 || nr >= 4 || nc < 0 || nc >= 4) continue;
            if (used & (1 << (nr * 4 + nc))) continue;
            int n_used = used | (1 << (nr * 4 + nc));
            cur_path[path_idx].push_back({nr, nc});
            dfs(path_idx, len + 1, nr, nc, in_pts, out_pts, n_used, in_used, out_used, cur_path, routes);
            cur_path[path_idx].pop_back();
        }
    }
}

void precompute_corners() {
    for (int d_in = 0; d_in < 4; ++d_in) {
        for (int d_out = 0; d_out < 4; ++d_out) {
            if (d_in == d_out || abs(d_in - d_out) == 2) continue;
            int in_pts[4][2], out_pts[4][2];
            for (int w = 0; w < 4; ++w) {
                if (d_in == 0) { in_pts[w][0] = w; in_pts[w][1] = 0; }
                else if (d_in == 1) { in_pts[w][0] = 0; in_pts[w][1] = w; }
                else if (d_in == 2) { in_pts[w][0] = w; in_pts[w][1] = 3; }
                else if (d_in == 3) { in_pts[w][0] = 3; in_pts[w][1] = w; }

                if (d_out == 0) { out_pts[w][0] = w; out_pts[w][1] = 3; }
                else if (d_out == 1) { out_pts[w][0] = 3; out_pts[w][1] = w; }
                else if (d_out == 2) { out_pts[w][0] = w; out_pts[w][1] = 0; }
                else if (d_out == 3) { out_pts[w][0] = 0; out_pts[w][1] = w; }
            }
            vector<pair<int,int>> cur_path[4];
            for (int w = 0; w < 4; ++w) {
                int r = in_pts[w][0], c = in_pts[w][1];
                cur_path[0].push_back({r, c});
                dfs(0, 1, r, c, in_pts, out_pts, (1 << (r * 4 + c)), (1 << w), 0, cur_path, corner_transitions[d_in][d_out]);
                cur_path[0].pop_back();
            }
        }
    }
}

void add_block(int R, int C, int in_dir, int out_dir, 
               vector<pair<int,int>>& L0, vector<pair<int,int>>& L1, 
               vector<pair<int,int>>& L2, vector<pair<int,int>>& L3) {
    int r0 = R * 4, c0 = C * 4;
    auto add = [&](vector<pair<int,int>>& L, const vector<pair<int,int>>& pts) {
        for (auto p : pts) L.push_back({r0 + p.first, c0 + p.second});
    };
    if (in_dir == 0 && out_dir == 0) {
        add(L0, {{0,0},{0,1},{0,2},{0,3}}); add(L1, {{1,0},{1,1},{1,2},{1,3}});
        add(L2, {{2,0},{2,1},{2,2},{2,3}}); add(L3, {{3,0},{3,1},{3,2},{3,3}});
    } else if (in_dir == 1 && out_dir == 1) {
        add(L0, {{0,3},{1,3},{2,3},{3,3}}); add(L1, {{0,2},{1,2},{2,2},{3,2}});
        add(L2, {{0,1},{1,1},{2,1},{3,1}}); add(L3, {{0,0},{1,0},{2,0},{3,0}});
    } else if (in_dir == 2 && out_dir == 2) {
        add(L0, {{3,3},{3,2},{3,1},{3,0}}); add(L1, {{2,3},{2,2},{2,1},{2,0}});
        add(L2, {{1,3},{1,2},{1,1},{1,0}}); add(L3, {{0,3},{0,2},{0,1},{0,0}});
    } else if (in_dir == 3 && out_dir == 3) {
        add(L0, {{3,0},{2,0},{1,0},{0,0}}); add(L1, {{3,1},{2,1},{1,1},{0,1}});
        add(L2, {{3,2},{2,2},{1,2},{0,2}}); add(L3, {{3,3},{2,3},{1,3},{0,3}});
    } else if (in_dir == 0 && out_dir == 1) { 
        add(L0, {{0,0},{0,1},{0,2},{0,3},{1,3},{2,3},{3,3}});
        add(L1, {{1,0},{1,1},{1,2},{2,2},{3,2}}); add(L2, {{2,0},{2,1},{3,1}}); add(L3, {{3,0}});
    } else if (in_dir == 1 && out_dir == 2) { 
        add(L0, {{0,3},{1,3},{2,3},{3,3},{3,2},{3,1},{3,0}});
        add(L1, {{0,2},{1,2},{2,2},{2,1},{2,0}}); add(L2, {{0,1},{1,1},{1,0}}); add(L3, {{0,0}});
    } else if (in_dir == 2 && out_dir == 3) { 
        add(L0, {{3,3},{3,2},{3,1},{3,0},{2,0},{1,0},{0,0}});
        add(L1, {{2,3},{2,2},{2,1},{1,1},{0,1}}); add(L2, {{1,3},{1,2},{0,2}}); add(L3, {{0,3}});
    } else if (in_dir == 3 && out_dir == 0) { 
        add(L0, {{3,0},{2,0},{1,0},{0,0},{0,1},{0,2},{0,3}});
        add(L1, {{3,1},{2,1},{1,1},{1,2},{1,3}}); add(L2, {{3,2},{2,2},{2,3}}); add(L3, {{3,3}});
    }
}

struct Step {
    int type; // 0: 直進, 1: 角
    int r_coord[4], c_coord[4];
    long long idx_i[4]; 
    long long idx_list[4][7];
    int in_dir, out_dir, block_r, block_c;
};

// 特定の盤面状態で経路を解く
pair<long long, vector<pair<int, int>>> solve_once(const vector<vector<int>>& A) {
    int M = N / 4;
    vector<pair<int, int>> P;
    int r = 0, c = 0;
    for (int k = 0; k < M / 2; ++k) {
        for (; c < M - 1 - k; ++c) P.push_back({r, c});
        for (; r < M - 1 - k; ++r) P.push_back({r, c});
        for (; c > k; --c) P.push_back({r, c});
        for (; r > k + 1; --r) P.push_back({r, c});
        P.push_back({r, c});
        c++;
    }

    int num_blocks = P.size();
    vector<int> in_dir(num_blocks), out_dir(num_blocks);
    for (int i = 0; i < num_blocks; ++i) {
        if (i < num_blocks - 1) {
            int dr = P[i+1].first - P[i].first, dc = P[i+1].second - P[i].second;
            if (dc == 1) out_dir[i] = 0;
            else if (dr == 1) out_dir[i] = 1;
            else if (dc == -1) out_dir[i] = 2;
            else if (dr == -1) out_dir[i] = 3;
        }
        if (i > 0) in_dir[i] = out_dir[i-1];
    }
    in_dir[0] = 0; out_dir[num_blocks - 1] = in_dir[num_blocks - 1];

    vector<pair<int, int>> L0, L1, L2, L3;
    for (int i = 0; i < num_blocks; ++i) {
        add_block(P[i].first, P[i].second, in_dir[i], out_dir[i], L0, L1, L2, L3);
    }
    reverse(L1.begin(), L1.end());
    reverse(L3.begin(), L3.end());

    vector<pair<int, int>> res_ans;
    res_ans.insert(res_ans.end(), L0.begin(), L0.end());
    res_ans.insert(res_ans.end(), L1.begin(), L1.end());
    res_ans.insert(res_ans.end(), L2.begin(), L2.end());
    res_ans.insert(res_ans.end(), L3.begin(), L3.end());

    vector<vector<int>> local_pos_grid(N, vector<int>(N));
    for (int i = 0; i < (int)res_ans.size(); ++i) local_pos_grid[res_ans[i].first][res_ans[i].second] = i;

    vector<Step> dp_steps;
    for (int b = 0; b < num_blocks; ++b) {
        int dir = in_dir[b], out = out_dir[b], R = P[b].first, C = P[b].second;
        if (dir == out) {
            vector<int> p_base = (dir == 0 || dir == 3) ? vector<int>{0, 1, 2, 3} : vector<int>{3, 2, 1, 0};
            for (int dt = 0; dt < 4; ++dt) {
                Step s; s.type = 0;
                for (int w = 0; w < 4; ++w) {
                    if (dir == 0) { s.r_coord[w] = R*4 + w; s.c_coord[w] = C*4 + dt; }
                    else if (dir == 1) { s.r_coord[w] = R*4 + dt; s.c_coord[w] = C*4 + w; }
                    else if (dir == 2) { s.r_coord[w] = R*4 + w; s.c_coord[w] = C*4 + 3 - dt; }
                    else if (dir == 3) { s.r_coord[w] = R*4 + 3 - dt; s.c_coord[w] = C*4 + w; }
                }
                for (int i = 0; i < 4; ++i) {
                    int orig_w = p_base[i];
                    s.idx_i[i] = local_pos_grid[s.r_coord[orig_w]][s.c_coord[orig_w]];
                }
                dp_steps.push_back(s);
            }
        } else {
            Step s; s.type = 1; s.in_dir = dir; s.out_dir = out; s.block_r = R; s.block_c = C;
            vector<pair<int,int>> dum[4];
            add_block(R, C, dir, out, dum[0], dum[1], dum[2], dum[3]);
            for (int i = 0; i < 4; ++i) {
                for (int k = 0; k < req_len[i]; ++k) {
                    s.idx_list[i][k] = local_pos_grid[dum[i][k].first][dum[i][k].second];
                }
            }
            dp_steps.push_back(s);
        }
    }

    int T_steps = (int)dp_steps.size();
    const long long INF = 1e18;
    vector<vector<long long>> dp(T_steps, vector<long long>(24, -INF));
    vector<vector<int>> prev_id(T_steps, vector<int>(24, -1));
    vector<vector<int>> prev_route(T_steps, vector<int>(24, -1));

    int start_perm = -1;
    vector<int> p_base0 = (in_dir[0] == 0 || in_dir[0] == 3) ? vector<int>{0,1,2,3} : vector<int>{3,2,1,0};
    for (int i = 0; i < 24; ++i) if (perms[i] == p_base0) start_perm = i;

    long long score0 = 0;
    for (int i = 0; i < 4; ++i) {
        int w = perms[start_perm][i];
        score0 += (long long)dp_steps[0].idx_i[i] * A[dp_steps[0].r_coord[w]][dp_steps[0].c_coord[w]];
    }
    dp[0][start_perm] = score0;

    vector<vector<bool>> can_trans(24, vector<bool>(24, false));
    for (int u = 0; u < 24; ++u) {
        for (int v = 0; v < 24; ++v) {
            bool ok = true;
            for (int i = 0; i < 4; ++i) if (abs(perms[u][i] - perms[v][i]) > 1) ok = false;
            can_trans[u][v] = ok;
        }
    }

    for (int t = 1; t < T_steps; ++t) {
        if (dp_steps[t].type == 0) {
            for (int u = 0; u < 24; ++u) {
                long long score_u = 0;
                for (int i = 0; i < 4; ++i) {
                    int w = perms[u][i];
                    score_u += (long long)dp_steps[t].idx_i[i] * A[dp_steps[t].r_coord[w]][dp_steps[t].c_coord[w]];
                }
                for (int v = 0; v < 24; ++v) {
                    if (dp[t-1][v] == -INF || !can_trans[v][u]) continue;
                    if (dp[t-1][v] + score_u > dp[t][u]) {
                        dp[t][u] = dp[t-1][v] + score_u;
                        prev_id[t][u] = v;
                    }
                }
            }
        } else {
            int d_in = dp_steps[t].in_dir, d_out = dp_steps[t].out_dir;
            for (int r_idx = 0; r_idx < (int)corner_transitions[d_in][d_out].size(); ++r_idx) {
                const auto& route = corner_transitions[d_in][d_out][r_idx];
                int u = route.u, v = route.v;
                if (dp[t-1][u] == -INF) continue;
                
                long long score_add = 0;
                for (int i = 0; i < 4; ++i) {
                    for (int k = 0; k < req_len[i]; ++k) {
                        int row = dp_steps[t].block_r * 4 + route.paths[i][k].first;
                        int col = dp_steps[t].block_c * 4 + route.paths[i][k].second;
                        score_add += (long long)dp_steps[t].idx_list[i][k] * A[row][col];
                    }
                }
                if (dp[t-1][u] + score_add > dp[t][v]) {
                    dp[t][v] = dp[t-1][u] + score_add;
                    prev_id[t][v] = u;
                    prev_route[t][v] = r_idx;
                }
            }
        }
    }

    int end_perm = -1;
    vector<int> p_base_end = (in_dir[num_blocks - 1] == 0 || in_dir[num_blocks - 1] == 3) ? vector<int>{0,1,2,3} : vector<int>{3,2,1,0};
    for (int i = 0; i < 24; ++i) if (perms[i] == p_base_end) end_perm = i;

    long long final_score = dp[T_steps-1][end_perm];
    int u_curr = end_perm;
    for (int t = T_steps - 1; t >= 0; --t) {
        if (dp_steps[t].type == 0) {
            for (int i = 0; i < 4; ++i) {
                int w = perms[u_curr][i];
                res_ans[dp_steps[t].idx_i[i]] = {dp_steps[t].r_coord[w], dp_steps[t].c_coord[w]};
            }
            u_curr = prev_id[t][u_curr];
        } else {
            int r_idx = prev_route[t][u_curr];
            int prev_u = prev_id[t][u_curr];
            const auto& route = corner_transitions[dp_steps[t].in_dir][dp_steps[t].out_dir][r_idx];
            for (int i = 0; i < 4; ++i) {
                for (int k = 0; k < req_len[i]; ++k) {
                    int row = dp_steps[t].block_r * 4 + route.paths[i][k].first;
                    int col = dp_steps[t].block_c * 4 + route.paths[i][k].second;
                    res_ans[dp_steps[t].idx_list[i][k]] = {row, col};
                }
            }
            u_curr = prev_u;
        }
    }
    return {final_score, res_ans};
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    if (!(cin >> N)) return 0;

    auto start_time = chrono::high_resolution_clock::now();
    auto get_time = [&]() {
        return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_time).count();
    };

    A_orig.assign(N, vector<int>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) cin >> A_orig[i][j];
    }

    vector<int> p = {0, 1, 2, 3};
    do { perms.push_back(p); } while (next_permutation(p.begin(), p.end()));
    precompute_corners();

    long long max_score = -1;
    vector<pair<int, int>> best_ans;

    // 8パターンの回転・反転を試す
    for (int type = 0; type < 8; ++type) {
        vector<vector<int>> A_trans(N, vector<int>(N));
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                pair<int, int> orig = transform_coord(i, j, N, type);
                A_trans[i][j] = A_orig[orig.first][orig.second];
            }
        }

        auto result = solve_once(A_trans);
        
        vector<pair<int, int>> ans_restored;
        for (auto p_coord : result.second) {
            ans_restored.push_back(transform_coord(p_coord.first, p_coord.second, N, type));
        }

        long long current_score = 0;
        for (int k = 0; k < (int)ans_restored.size(); ++k) {
            current_score += (long long)k * A_orig[ans_restored[k].first][ans_restored[k].second];
        }

        if (current_score > max_score) {
            max_score = current_score;
            best_ans = ans_restored;
        }
    }

    // --- 山登り法での仕上げ ---
    vector<vector<int>> pos_grid(N, vector<int>(N));
    for (int i = 0; i < (int)best_ans.size(); ++i) {
        pos_grid[best_ans[i].first][best_ans[i].second] = i;
    }

    bool improved = true;
    while (improved && get_time() < 2850) {
        improved = false;
        for (int i = 0; i < N * N; ++i) {
            if (get_time() >= 2850) break;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = best_ans[i].first + dr, nc = best_ans[i].second + dc;
                    if (nr < 0 || nr >= N || nc < 0 || nc >= N) continue;
                    
                    int j = pos_grid[nr][nc];
                    // i < j かつ A[i] > A[j] ならば入れ替えるとスコア改善
                    if (i < j && A_orig[best_ans[i].first][best_ans[i].second] > A_orig[best_ans[j].first][best_ans[j].second]) {
                        if (check_swap_valid(i, j, best_ans)) {
                            pos_grid[best_ans[i].first][best_ans[i].second] = j;
                            pos_grid[best_ans[j].first][best_ans[j].second] = i;
                            swap(best_ans[i], best_ans[j]);
                            improved = true;
                        }
                    }
                }
            }
        }
    }

    for (auto p_ans : best_ans) {
        cout << p_ans.first << " " << p_ans.second << "\n";
    }

    return 0;
}