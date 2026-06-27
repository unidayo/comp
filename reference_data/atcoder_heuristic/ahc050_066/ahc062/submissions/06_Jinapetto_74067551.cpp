#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3")
#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using vi = vector<int>;
using vl = vector<long long>;
using vs = vector<string>;
using vc = vector<char>;
using vb = vector<bool>;
using vvi = vector<vector<int>>;
using vvvi = vector<vector<vector<int>>>;
using vvvvi = vector<vector<vector<vector<int>>>>;
using vvl = vector<vector<long long>>;
using vvvl = vector<vector<vector<long long>>>;
using vvvvl = vector<vector<vector<vector<long long>>>>;
using vvc = vector<vector<char>>;
using vvb = vector<vector<bool>>;
using vvvb = vector<vector<vector<bool>>>;
using vvvvb = vector<vector<vector<vector<bool>>>>;

#define rep(i, n) for (int i = 0; i < (int)(n); i++)
#define ALL(obj) (obj).begin(),(obj).end()

struct Timer{
    chrono::steady_clock::time_point st;
    Timer(){
        st = chrono::steady_clock::now();
    }
    inline double get(){
        return chrono::duration<double>(chrono::steady_clock::now() - st).count();
    }
} tim;

struct XorShift{
    using result_type = uint32_t;
    uint64_t x = 2003082720030827ull;
    inline uint32_t operator()(){
        x^=x<<7;
        x^=x>>9;
        return (uint32_t)x; 
    }
    inline uint32_t i(uint32_t m){
        return m ? (uint32_t)((uint64_t)(*this)()*m >> 32) : (*this)();
    }
    inline double d(){
        return (double)((*this)() & 0xFFFFFF) / 16777216.0;
    }
    static constexpr uint32_t min() { return 0; }
    static constexpr uint32_t max() { return UINT32_MAX; }
} rnd;

inline bool ac(double start_time, double cur_time, double end_time, double score_dist, double start_temp, double end_temp){
    double temp = start_temp + (end_temp - start_temp) * (cur_time - start_time) / (end_time - start_time);
    if(score_dist >= 0.0) return true;
    if(score_dist <= -8.0 * temp) return false;
    return rnd.d() < exp(score_dist/temp);
}

inline int chebyshev(pair<int, int> p1, pair<int, int> p2) {
    return max(abs(p1.first - p2.first), abs(p1.second - p2.second));
}

const ll LINF = (ll)1e18;

// =========================================================
// 50x50 の多様な初期パスを生成する関数 (16パターン)
// =========================================================
vector<vector<pair<int, int>>> generate_paths50(int W) {
    vector<vector<pair<int, int>>> paths;

    for (int corner = 0; corner < 4; corner++) {
        for (int clock = 0; clock < 2; clock++) {
            vector<pair<int, int>> path;
            int t = 0, b = W - 1, l = 0, ri = W - 1;
            while(t <= b && l <= ri) {
                if (clock) {
                    for(int j = l; j <= ri; j++) path.push_back({t, j}); t++;
                    for(int i = t; i <= b; i++) path.push_back({i, ri}); ri--;
                    if(t <= b) { for(int j = ri; j >= l; j--) path.push_back({b, j}); b--; }
                    if(l <= ri) { for(int i = b; i >= t; i--) path.push_back({i, l}); l++; }
                } else {
                    for(int i = t; i <= b; i++) path.push_back({i, l}); l++;
                    for(int j = l; j <= ri; j++) path.push_back({b, j}); b--;
                    if(l <= ri) { for(int i = b; i >= t; i--) path.push_back({i, ri}); ri--; }
                    if(t <= b) { for(int j = ri; j >= l; j--) path.push_back({t, j}); t++; }
                }
            }
            for (auto& p : path) {
                if (corner & 2) p.first = W - 1 - p.first;
                if (corner & 1) p.second = W - 1 - p.second;
            }
            paths.push_back(path);
        }
    }

    for (int corner = 0; corner < 4; corner++) {
        for (int horiz = 0; horiz < 2; horiz++) {
            vector<pair<int, int>> path;
            int r_start = (corner & 2) ? W - 1 : 0;
            int c_start = (corner & 1) ? W - 1 : 0;
            int r_step = (corner & 2) ? -1 : 1;
            int c_step = (corner & 1) ? -1 : 1;

            if (horiz) {
                for (int i = 0; i < W; i++) {
                    int r = r_start + i * r_step;
                    for (int j = 0; j < W; j++) {
                        int c = (i % 2 == 0) ? (c_start + j * c_step) : (c_start + (W - 1 - j) * c_step);
                        path.push_back({r, c});
                    }
                }
            } else {
                for (int j = 0; j < W; j++) {
                    int c = c_start + j * c_step;
                    for (int i = 0; i < W; i++) {
                        int r = (j % 2 == 0) ? (r_start + i * r_step) : (r_start + (W - 1 - i) * r_step);
                        path.push_back({r, c});
                    }
                }
            }
            paths.push_back(path);
        }
    }
    return paths;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    if (!(cin >> N)) return 0;

    vvi A(N, vi(N));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) cin >> A[i][j];
    }

    int dr[] = {0, 1, 0, -1};
    int dc[] = {1, 0, -1, 0};

    int W50 = N / 4;
    auto paths50 = generate_paths50(W50);

    // --- DP事前計算: 順列と、有効な遷移元のリスト ---
    vector<vector<int>> perms;
    vector<int> p_base = {0, 1, 2, 3};
    do { perms.push_back(p_base); } while (next_permutation(p_base.begin(), p_base.end()));

    vector<vector<int>> valid_prev_p(24);
    for(int p = 0; p < 24; p++) {
        for(int prev_p = 0; prev_p < 24; prev_p++) {
            bool ok = true;
            for(int j = 0; j < 4; j++) {
                int m = perms[p][j];
                int prev_j = -1;
                for(int x = 0; x < 4; x++) if(perms[prev_p][x] == m) prev_j = x;
                // まっすぐな管の中なので、物理的なレーン移動は隣接(±1)まで
                if(abs(j - prev_j) > 1) { ok = false; break; }
            }
            if(ok) valid_prev_p[p].push_back(prev_p);
        }
    }

    vector<pair<int, int>> best_initial_path;
    long long best_initial_score = -LINF;

    for (const auto& path50 : paths50) {
        if (tim.get() > 1.0) break;

        vector<vector<vector<bool>>> edge50(W50, vector<vector<bool>>(W50, vector<bool>(4, false)));
        for(int i = 0; i < (int)path50.size() - 1; i++){
            int r1 = path50[i].first, c1 = path50[i].second;
            int r2 = path50[i+1].first, c2 = path50[i+1].second;
            if(r2 == r1 && c2 == c1 + 1) { edge50[r1][c1][0] = true; edge50[r2][c2][2] = true; }
            if(r2 == r1 + 1 && c2 == c1) { edge50[r1][c1][1] = true; edge50[r2][c2][3] = true; }
            if(r2 == r1 && c2 == c1 - 1) { edge50[r1][c1][2] = true; edge50[r2][c2][0] = true; }
            if(r2 == r1 - 1 && c2 == c1) { edge50[r1][c1][3] = true; edge50[r2][c2][1] = true; }
        }

        int W100 = N / 2;
        auto can_move50 = [&](int r, int c, int d) -> bool {
            int nr = r + dr[d], nc = c + dc[d];
            if(nr < 0 || nr >= W100 || nc < 0 || nc >= W100) return false;
            int mr1 = r / 2, mc1 = c / 2, mr2 = nr / 2, mc2 = nc / 2;
            if(mr1 == mr2 && mc1 == mc2) return true;
            return edge50[mr1][mc1][d];
        };

        int dir50 = 0;
        if (path50[1].second == path50[0].second + 1) dir50 = 0;
        else if (path50[1].first == path50[0].first + 1) dir50 = 1;
        else if (path50[1].second == path50[0].second - 1) dir50 = 2;
        else if (path50[1].first == path50[0].first - 1) dir50 = 3;

        int r100 = 2 * path50[0].first + ((dir50 == 2 || dir50 == 3) ? 1 : 0);
        int c100 = 2 * path50[0].second + ((dir50 == 1 || dir50 == 2) ? 1 : 0);
        int dir100 = dir50;

        vector<pair<int, int>> path100;
        vector<vector<bool>> vis100(W100, vector<bool>(W100, false));
        path100.push_back({r100, c100});
        vis100[r100][c100] = true;

        bool valid_path100 = true;
        for(int step = 1; step < W100 * W100; step++) {
            bool moved = false;
            for(int i = 0; i < 4; i++) {
                int d = (dir100 + 1 + 3 * i) % 4; 
                if(can_move50(r100, c100, d)) {
                    int nr = r100 + dr[d], nc = c100 + dc[d];
                    if(!vis100[nr][nc]) { r100 = nr; c100 = nc; dir100 = d; moved = true; break; }
                }
            }
            if(!moved) { valid_path100 = false; break; } 
            path100.push_back({r100, c100});
            vis100[r100][c100] = true;
        }
        if(!valid_path100) continue;

        vector<vector<vector<bool>>> edge100(W100, vector<vector<bool>>(W100, vector<bool>(4, false)));
        for(int i = 0; i < (int)path100.size() - 1; i++){
            int r1 = path100[i].first, c1 = path100[i].second;
            int r2 = path100[i+1].first, c2 = path100[i+1].second;
            if(r2 == r1 && c2 == c1 + 1) { edge100[r1][c1][0] = true; edge100[r2][c2][2] = true; }
            if(r2 == r1 + 1 && c2 == c1) { edge100[r1][c1][1] = true; edge100[r2][c2][3] = true; }
            if(r2 == r1 && c2 == c1 - 1) { edge100[r1][c1][2] = true; edge100[r2][c2][0] = true; }
            if(r2 == r1 - 1 && c2 == c1) { edge100[r1][c1][3] = true; edge100[r2][c2][1] = true; }
        }

        auto can_move100 = [&](int r, int c, int d) -> bool {
            int nr = r + dr[d], nc = c + dc[d];
            if(nr < 0 || nr >= N || nc < 0 || nc >= N) return false;
            int mr1 = r / 2, mc1 = c / 2, mr2 = nr / 2, mc2 = nc / 2;
            if(mr1 == mr2 && mc1 == mc2) return true;
            return edge100[mr1][mc1][d];
        };

        int dir100_start = 0;
        if (path100[1].second == path100[0].second + 1) dir100_start = 0;
        else if (path100[1].first == path100[0].first + 1) dir100_start = 1;
        else if (path100[1].second == path100[0].second - 1) dir100_start = 2;
        else if (path100[1].first == path100[0].first - 1) dir100_start = 3;

        int R = 2 * path100[0].first + ((dir100_start == 2 || dir100_start == 3) ? 1 : 0);
        int C = 2 * path100[0].second + ((dir100_start == 1 || dir100_start == 2) ? 1 : 0);
        int cur_dir = dir100_start;

        vector<pair<int, int>> final_path;
        final_path.reserve(N * N);
        vector<vector<bool>> vis200(N, vector<bool>(N, false));
        final_path.push_back({R, C});
        vis200[R][C] = true;

        bool valid_final_path = true;
        for(int step = 1; step < N * N; step++) {
            bool moved = false;
            for(int i = 0; i < 4; i++) {
                int d = (cur_dir + 1 + 3 * i) % 4; 
                if(can_move100(R, C, d)) {
                    int nr = R + dr[d], nc = C + dc[d];
                    if(!vis200[nr][nc]) { R = nr; C = nc; cur_dir = d; moved = true; break; }
                }
            }
            if(!moved) { valid_final_path = false; break; } 
            final_path.push_back({R, C});
            vis200[R][C] = true;
        }
        if(!valid_final_path) continue;

        vector<int> path_dir(N * N, -1);
        for(int t = 0; t < N * N - 1; t++) {
            auto curr = final_path[t], nxt = final_path[t+1];
            if (nxt.second > curr.second) path_dir[t] = 0;
            else if (nxt.first > curr.first) path_dir[t] = 1;
            else if (nxt.second < curr.second) path_dir[t] = 2;
            else if (nxt.first < curr.first) path_dir[t] = 3;
        }
        path_dir[N * N - 1] = path_dir[N * N - 2];

        vector<vector<int>> dir(N, vector<int>(N, -1));
        vector<vector<int>> prev_dir(N, vector<int>(N, -1));
        for(int t = 0; t < N * N; t++) {
            dir[final_path[t].first][final_path[t].second] = path_dir[t];
            prev_dir[final_path[t].first][final_path[t].second] = (t > 0) ? path_dir[t-1] : path_dir[0];
        }

        vector<vector<bool>> is_corner(N, vector<bool>(N, false));
        for(int r = 0; r < N; r++) {
            for(int c = 0; c < N; c++) {
                if (dir[r][c] % 2 != prev_dir[r][c] % 2) is_corner[r][c] = true;
            }
        }

        vector<vector<vector<pair<int, int>>>> tubes;
        for (int r = 0; r <= N - 4; r += 4) {
            int c = 0;
            while (c < N) {
                bool valid = true;
                for (int i = 0; i < 4; i++) if (dir[r+i][c] % 2 != 0 || is_corner[r+i][c]) valid = false;
                if (valid) {
                    int start_c = c;
                    while (c < N) {
                        bool v = true;
                        for (int i = 0; i < 4; i++) if (dir[r+i][c] % 2 != 0 || is_corner[r+i][c]) v = false;
                        if (!v) break;
                        c++;
                    }
                    int end_c = c - 1;
                    if (end_c - start_c + 1 >= 2) {
                        vector<vector<pair<int, int>>> tube;
                        for (int cc = start_c; cc <= end_c; cc++) {
                            vector<pair<int, int>> sec;
                            for (int i = 0; i < 4; i++) sec.push_back({r+i, cc});
                            tube.push_back(sec);
                        }
                        tubes.push_back(tube);
                    }
                } else c++;
            }
        }
        for (int c = 0; c <= N - 4; c += 4) {
            int r = 0;
            while (r < N) {
                bool valid = true;
                for (int i = 0; i < 4; i++) if (dir[r][c+i] % 2 != 1 || is_corner[r][c+i]) valid = false;
                if (valid) {
                    int start_r = r;
                    while (r < N) {
                        bool v = true;
                        for (int i = 0; i < 4; i++) if (dir[r][c+i] % 2 != 1 || is_corner[r][c+i]) v = false;
                        if (!v) break;
                        r++;
                    }
                    int end_r = r - 1;
                    if (end_r - start_r + 1 >= 2) {
                        vector<vector<pair<int, int>>> tube;
                        for (int rr = start_r; rr <= end_r; rr++) {
                            vector<pair<int, int>> sec;
                            for (int i = 0; i < 4; i++) sec.push_back({rr, c+i});
                            tube.push_back(sec);
                        }
                        tubes.push_back(tube);
                    }
                } else r++;
            }
        }

        vector<pair<int, int>> current_new_path = final_path;

        // =========================================================
        // 超高速化 DP エンジン
        // =========================================================
        for (auto& tube : tubes) {
            int K = tube.size();
            
            // 1. チューブの境界ボックス (Bounding Box) を取得
            int min_r = N, max_r = -1, min_c = N, max_c = -1;
            for(int k=0; k<K; k++) {
                for(int j=0; j<4; j++) {
                    min_r = min(min_r, tube[k][j].first);
                    max_r = max(max_r, tube[k][j].first);
                    min_c = min(min_c, tube[k][j].second);
                    max_c = max(max_c, tube[k][j].second);
                }
            }
            
            // チューブ内判定 (mapの代わりの O(1) 判定)
            auto is_in_tube = [&](pair<int, int> p) {
                return p.first >= min_r && p.first <= max_r && p.second >= min_c && p.second <= max_c;
            };

            // 進行方向判定
            bool is_horiz = (tube[0][0].second != tube[1][0].second);
            auto get_k = [&](pair<int, int> p) {
                return is_horiz ? (p.second - min_c) : (p.first - min_r);
            };

            // 2. ストランド (Time) を高速抽出
            vector<vector<int>> strands;
            vector<int> current_strand;
            for (int t = 0; t < N * N; t++) {
                if (is_in_tube(final_path[t])) {
                    current_strand.push_back(t);
                } else if (!current_strand.empty()) {
                    strands.push_back(current_strand);
                    current_strand.clear();
                }
            }
            if (!current_strand.empty()) strands.push_back(current_strand);

            bool valid_strands = (strands.size() == 4);
            for(auto& s : strands) if(s.size() != K) valid_strands = false;
            if (!valid_strands) continue;

            vector<vector<int>> Time(4, vector<int>(K, -1));
            for (int m = 0; m < 4; m++) {
                for (int t : strands[m]) {
                    Time[m][get_k(final_path[t])] = t;
                }
            }

            // 3. 状態(p_idx)と位置(k)に対するスコア・境界チェックの事前計算
            vector<vector<long long>> score_P_k(24, vector<long long>(K, -LINF));
            for(int k=0; k<K; k++) {
                for(int p_idx=0; p_idx<24; p_idx++) {
                    auto& P = perms[p_idx];
                    bool valid_bounds = true;
                    long long score = 0;
                    
                    for(int j=0; j<4; j++) {
                        int T = Time[P[j]][k];
                        pair<int, int> cell = tube[k][j];
                        score += (long long)T * A[cell.first][cell.second];
                        
                        if (T - 1 >= 0) {
                            pair<int, int> prev_pos = final_path[T - 1];
                            if (!is_in_tube(prev_pos) && chebyshev(cell, prev_pos) > 1) { valid_bounds = false; break; }
                        }
                        if (T + 1 < N * N) {
                            pair<int, int> next_pos = final_path[T + 1];
                            if (!is_in_tube(next_pos) && chebyshev(cell, next_pos) > 1) { valid_bounds = false; break; }
                        }
                    }
                    if(valid_bounds) score_P_k[p_idx][k] = score;
                }
            }

            // 4. 洗練された DP 最内ループ
            vector<vector<long long>> dp(K, vector<long long>(24, -LINF));
            vector<vector<int>> parent(K, vector<int>(24, -1));

            for (int p_idx = 0; p_idx < 24; p_idx++) dp[0][p_idx] = score_P_k[p_idx][0];

            for (int k = 1; k < K; k++) {
                for (int p_idx = 0; p_idx < 24; p_idx++) {
                    long long score = score_P_k[p_idx][k];
                    if (score == -LINF) continue; 

                    long long best_prev = -LINF;
                    int best_prev_p = -1;

                    for (int prev_p : valid_prev_p[p_idx]) {
                        if (dp[k-1][prev_p] > best_prev) {
                            best_prev = dp[k-1][prev_p];
                            best_prev_p = prev_p;
                        }
                    }

                    if (best_prev != -LINF) {
                        dp[k][p_idx] = best_prev + score;
                        parent[k][p_idx] = best_prev_p;
                    }
                }
            }

            // DP 復元
            long long best_val = -LINF;
            int best_p = -1;
            for (int p = 0; p < 24; p++) {
                if (dp[K-1][p] > best_val) { best_val = dp[K-1][p]; best_p = p; }
            }

            if (best_p != -1) {
                int curr_p = best_p;
                for (int k = K - 1; k >= 0; k--) {
                    auto& P = perms[curr_p];
                    for (int j = 0; j < 4; j++) current_new_path[ Time[P[j]][k] ] = tube[k][j];
                    curr_p = parent[k][curr_p];
                }
            }
        }

        long long current_score = 0;
        for (int t = 0; t < N * N; t++) {
            current_score += (long long)t * A[current_new_path[t].first][current_new_path[t].second];
        }

        if (current_score > best_initial_score) {
            best_initial_score = current_score;
            best_initial_path = current_new_path;
        }
    }

    if (best_initial_path.empty()) {
        for(int i=0; i<N; i++) for(int j=0; j<N; j++) best_initial_path.push_back({i, j});
    }

    vector<pair<int, int>> new_path = best_initial_path;

    // =========================================================
    // 局所探索 (焼きなまし法: Simulated Annealing)
    // =========================================================
    vector<vector<int>> time_at(N, vector<int>(N, -1));
    for (int t = 0; t < N * N; t++) {
        time_at[new_path[t].first][new_path[t].second] = t;
    }

    int dr8[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dc8[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    double start_time = tim.get();
    double cur_time = start_time;
    double end_time = 2.95;
    
    double start_temp = 8e7;
    double end_temp = 100.0; 

    int loop_count = 0;
    
    while (true) {
        if ((loop_count & 1023) == 0) {
            cur_time = tim.get();
            if(end_time < cur_time) break;
        }
        loop_count++;

        int t1 = rnd.i(N * N);
        int r = new_path[t1].first;
        int c = new_path[t1].second;

        int dir = rnd.i(8);
        int nr = r + dr8[dir];
        int nc = c + dc8[dir];

        if (nr < 0 || nr >= N || nc < 0 || nc >= N) continue;

        int t2 = time_at[nr][nc];
        if (t1 == t2) continue;

        int swap_t1 = t1;
        int swap_t2 = t2;
        if (swap_t1 > swap_t2) swap(swap_t1, swap_t2);

        pair<int, int> p1 = new_path[swap_t1];
        pair<int, int> p2 = new_path[swap_t2];

        bool can_swap = true;
        if (swap_t2 == swap_t1 + 1) {
            if (swap_t1 > 0 && chebyshev(new_path[swap_t1 - 1], p2) > 1) can_swap = false;
            if (swap_t2 < N * N - 1 && chebyshev(p1, new_path[swap_t2 + 1]) > 1) can_swap = false;
        } else {
            if (swap_t1 > 0 && chebyshev(new_path[swap_t1 - 1], p2) > 1) can_swap = false;
            if (swap_t1 < N * N - 1 && chebyshev(p2, new_path[swap_t1 + 1]) > 1) can_swap = false;
            if (swap_t2 > 0 && chebyshev(new_path[swap_t2 - 1], p1) > 1) can_swap = false;
            if (swap_t2 < N * N - 1 && chebyshev(p1, new_path[swap_t2 + 1]) > 1) can_swap = false;
        }

        if (!can_swap) continue;

        long long delta = (long long)(swap_t2 - swap_t1) * (A[p1.first][p1.second] - A[p2.first][p2.second]);

        if(ac(start_time, cur_time, end_time, delta, start_temp, end_temp)){
            swap(new_path[swap_t1], new_path[swap_t2]);
            time_at[p2.first][p2.second] = swap_t1;
            time_at[p1.first][p1.second] = swap_t2;
        }
    }

    // =========================================================
    // 5. 出力
    // =========================================================
    for (int t = 0; t < N * N; t++) cout << new_path[t].first << " " << new_path[t].second << "\n";

    return 0;
}