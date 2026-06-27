/**
 * AHC Solver for "Molecular Development" - Dual Stage SA
 * Stage 1: Center-of-Mass Approximation SA (Optimizes Grouping & Rough Timing)
 * Stage 2: Exact Topology SA (Optimizes Merge Order & Precise Timing with fixed members)
 * Final Step: Exact Simulation Validator (Computes strict score using double precision)
 */

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC target("avx2")

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

// --- Constants & Types ---
constexpr int N = 300;
constexpr int T_MAX = 1000;
constexpr int M = 10;
constexpr int K = 30;
constexpr int K_MERGES = K - 1;
constexpr float L = 100000.0f;
constexpr float HALF_L = L / 2.0f;
constexpr int I_L = 100000;

// --- Hyperparameters & Environment Variables ---
namespace hp {
#ifdef ONLINE_JUDGE
#define FROM_ENV(type, name, default_value) constexpr type name = default_value;
#else
template <class T>
T from_env(const char* name, T default_value) {
    const char* value = std::getenv(name);
    if (value == nullptr) {
        return default_value;
    }
    std::stringstream ss(value);
    T res;
    ss >> res;
    return res;
}
#define FROM_ENV(type, name, default_value) const type name = from_env<type>(#name, default_value);
#endif

FROM_ENV(double, TIME_LIMIT_TOTAL, 1.95);

FROM_ENV(double, TIME_RATIO_STAGE1, 0.6631967847480753);
FROM_ENV(double, S1_TEMP_BEGIN, 8728.750637585845);
FROM_ENV(double, S1_TEMP_END, 668.7546014714529);
FROM_ENV(double, S1_TEMP_LINEAR_EXP_RATIO, 0.9765332557569487);
FROM_ENV(double, S1_TEMP_OFFSET_RATIO, 0.12075237381719478);
FROM_ENV(double, S1_PROB_TIME_MODIFY, 31);
FROM_ENV(double, S1_PROB_SWAP_LEAF, 57);
FROM_ENV(double, S2_TEMP_BEGIN, 1737.7385773903384);
FROM_ENV(double, S2_TEMP_END, 1.6961227346812457);
FROM_ENV(double, S2_TEMP_LINEAR_EXP_RATIO, 0.35651042302360403);
FROM_ENV(double, S2_TEMP_OFFSET_RATIO, 0.24397287375919646);
FROM_ENV(double, S2_PROB_TIME_SHIFT, 66);

#undef FROM_ENV

void init() {
    // 必要に応じて初期化処理
}
}  // namespace hp

// 温度計算関数 (パラメータを引数で受け取るように汎用化)
inline double calc_temp(double time_ratio, double t_begin, double t_end, double t_linear_exp_ratio, double t_offset_ratio) {
    const double temp_offset = t_offset_ratio * t_end;
    const double temp_begin_without_offset = t_begin - temp_offset;
    const double temp_end_without_offset = t_end - temp_offset;

    // 線形減少成分
    const double temp_linear = temp_begin_without_offset + (temp_end_without_offset - temp_begin_without_offset) * time_ratio;

    // 指数減少成分
    const double temp_exp = temp_begin_without_offset * std::pow(temp_end_without_offset / temp_begin_without_offset, time_ratio);

    // ブレンド + オフセット
    const double temp = temp_linear * t_linear_exp_ratio + temp_exp * (1.0 - t_linear_exp_ratio) + temp_offset;
    return temp;
}


struct Point {
    int x, y, vx, vy;
};

// 事前計算用の座標テーブル [点ID][時刻][0=x, 1=y]
alignas(32) float pos_cache[N][T_MAX][2];

// 初期速度キャッシュ
int vel_cache[N][2];

struct MergeOp {
    int target_idx;
    int time;
};

struct Cluster {
    int core_idx;
    std::array<MergeOp, K_MERGES> merges;
};

struct Solution {
    std::array<Cluster, M> clusters;
};

// --- Utils ---

struct XorShift128 {
    unsigned int x = 123456789;
    unsigned int y = 362436069;
    unsigned int z = 521288629;
    unsigned int w = 88675123;

    inline unsigned int next() {
        unsigned int t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
    inline int next_int(int range) {
        return next() % range;
    }
    inline float next_float() {
        return next() * (1.0f / 4294967295.0f);
    }
} rng;

// 初期化：全時刻の座標を事前計算する
void precompute_positions(const vector<Point>& points) {
    for (int i = 0; i < N; ++i) {
        int x0 = points[i].x;
        int y0 = points[i].y;
        int vx = points[i].vx;
        int vy = points[i].vy;

        for (int t = 0; t < T_MAX; ++t) {
            long long px = (long long)x0 + (long long)vx * t;
            long long py = (long long)y0 + (long long)vy * t;

            px %= I_L;
            if (px < 0)
                px += I_L;
            py %= I_L;
            if (py < 0)
                py += I_L;

            pos_cache[i][t][0] = (float)px;
            pos_cache[i][t][1] = (float)py;
        }
    }
}

// ソート状態を維持しながら time を更新する
inline int set_time_sorted(Cluster& cl, int idx, int new_t) {
    cl.merges[idx].time = new_t;
    // Bubble Left
    while (idx > 0 && cl.merges[idx].time < cl.merges[idx - 1].time) {
        std::swap(cl.merges[idx], cl.merges[idx - 1]);
        idx--;
    }
    // Bubble Right
    while (idx < K_MERGES - 1 && cl.merges[idx].time > cl.merges[idx + 1].time) {
        std::swap(cl.merges[idx], cl.merges[idx + 1]);
        idx++;
    }
    return idx;
}

// --- Stage 1 Logic (Approximation: Center of Mass) ---

inline long long eval_cluster_approx(const Cluster& cl, const vector<Point>& points) {
    float cx = points[cl.core_idx].x;
    float cy = points[cl.core_idx].y;
    int sum_vx = points[cl.core_idx].vx;
    int sum_vy = points[cl.core_idx].vy;
    int current_mass = 1;
    int current_time = 0;
    long long cost_sum = 0;

    const MergeOp* op_ptr = &cl.merges[0];
    const MergeOp* end_ptr = &cl.merges[K_MERGES];

    for (; op_ptr != end_ptr; ++op_ptr) {
        int t = op_ptr->time;
        int target_idx = op_ptr->target_idx;
        int dt = t - current_time;

        if (dt > 0) {
            float inv_mass = 1.0f / current_mass;
            cx += (sum_vx * inv_mass) * dt;
            cy += (sum_vy * inv_mass) * dt;

            // Fast wrap
            if (cx >= L)
                cx -= L;
            else if (cx < 0)
                cx += L;
            if (cx >= L)
                cx -= L;
            else if (cx < 0)
                cx += L;
            if (cy >= L)
                cy -= L;
            else if (cy < 0)
                cy += L;
            if (cy >= L)
                cy -= L;
            else if (cy < 0)
                cy += L;
        }

        float tx = pos_cache[target_idx][t][0];
        float ty = pos_cache[target_idx][t][1];

        float dx = std::abs(cx - tx);
        float dy = std::abs(cy - ty);
        if (dx > HALF_L)
            dx = L - dx;
        if (dy > HALF_L)
            dy = L - dy;

        cost_sum += (long long)(std::sqrt(dx * dx + dy * dy) + 0.5f);

        sum_vx += vel_cache[target_idx][0];
        sum_vy += vel_cache[target_idx][1];
        current_mass++;
        current_time = t;
    }
    return cost_sum;
}

// --- Stage 2 Logic (Exact: Relative Coordinates) ---

// 相対座標構造体
struct RelPoint {
    float rx, ry;  // Coreからの相対位置 (-L/2 ~ L/2 に正規化して持つ)
};

// 厳密評価関数：クラスター内の任意の点との最小距離を計算
inline long long eval_cluster_exact(const Cluster& cl, const vector<Point>& points) {
    // Coreの初期状態
    float core_x = points[cl.core_idx].x;
    float core_y = points[cl.core_idx].y;

    // Coreの相対位置は常に(0,0)だが、計算用にリストに入れる
    static vector<RelPoint> active_rel;
    active_rel.clear();
    active_rel.reserve(K);
    active_rel.push_back({0.0f, 0.0f});

    float cvx = points[cl.core_idx].vx;
    float cvy = points[cl.core_idx].vy;
    int current_mass = 1;
    int current_time = 0;
    long long total_cost = 0;

    for (const auto& op : cl.merges) {
        int t = op.time;
        int target_idx = op.target_idx;
        int dt = t - current_time;

        // Coreの絶対位置だけ更新する
        if (dt > 0) {
            float inv_mass = 1.0f / current_mass;
            core_x += (cvx * inv_mass) * dt;
            core_y += (cvy * inv_mass) * dt;

            // Normalize Core
            if (core_x >= L)
                core_x -= L;
            else if (core_x < 0)
                core_x += L;
            if (core_x >= L)
                core_x -= L;
            else if (core_x < 0)
                core_x += L;
            if (core_y >= L)
                core_y -= L;
            else if (core_y < 0)
                core_y += L;
            if (core_y >= L)
                core_y -= L;
            else if (core_y < 0)
                core_y += L;
        }

        // ターゲットの絶対位置
        float tx = pos_cache[target_idx][t][0];
        float ty = pos_cache[target_idx][t][1];

        // ターゲットのCoreに対する相対位置を計算
        float rel_tx = tx - core_x;
        float rel_ty = ty - core_y;

        // 相対位置もトーラス上で最短になるように正規化 (-L/2 ~ L/2)
        if (rel_tx > HALF_L)
            rel_tx -= L;
        else if (rel_tx < -HALF_L)
            rel_tx += L;
        if (rel_ty > HALF_L)
            rel_ty -= L;
        else if (rel_ty < -HALF_L)
            rel_ty += L;

        // クラスター内の既存点との最短距離を探す
        float min_dist_sq = 1e18f;

        for (const auto& p : active_rel) {
            float dx = std::abs(p.rx - rel_tx);
            float dy = std::abs(p.ry - rel_ty);
            if (dx > HALF_L)
                dx = L - dx;
            if (dy > HALF_L)
                dy = L - dy;

            float d2 = dx * dx + dy * dy;
            if (d2 < min_dist_sq) {
                min_dist_sq = d2;
            }
        }

        total_cost += (long long)(std::sqrt(min_dist_sq) + 0.5f);

        active_rel.push_back({rel_tx, rel_ty});

        cvx += vel_cache[target_idx][0];
        cvy += vel_cache[target_idx][1];
        current_mass++;
        current_time = t;
    }

    return total_cost;
}

// 最終出力生成用
void generate_output_ops(const Cluster& cl, const vector<Point>& points,
    vector<tuple<int, int, int>>& out_ops) {
    float core_x = points[cl.core_idx].x;
    float core_y = points[cl.core_idx].y;

    struct RelPointWithID {
        int id;
        float rx, ry;
    };
    vector<RelPointWithID> active_rel;
    active_rel.reserve(K);
    active_rel.push_back({cl.core_idx, 0.0f, 0.0f});

    float cvx = points[cl.core_idx].vx;
    float cvy = points[cl.core_idx].vy;
    int current_mass = 1;
    int current_time = 0;

    for (const auto& op : cl.merges) {
        int t = op.time;
        int target_idx = op.target_idx;
        int dt = t - current_time;

        if (dt > 0) {
            float inv_mass = 1.0f / current_mass;
            core_x += (cvx * inv_mass) * dt;
            core_y += (cvy * inv_mass) * dt;
            if (core_x >= L)
                core_x -= L;
            else if (core_x < 0)
                core_x += L;
            if (core_x >= L)
                core_x -= L;
            else if (core_x < 0)
                core_x += L;
            if (core_y >= L)
                core_y -= L;
            else if (core_y < 0)
                core_y += L;
            if (core_y >= L)
                core_y -= L;
            else if (core_y < 0)
                core_y += L;
        }

        float tx = pos_cache[target_idx][t][0];
        float ty = pos_cache[target_idx][t][1];

        float rel_tx = tx - core_x;
        float rel_ty = ty - core_y;
        if (rel_tx > HALF_L)
            rel_tx -= L;
        else if (rel_tx < -HALF_L)
            rel_tx += L;
        if (rel_ty > HALF_L)
            rel_ty -= L;
        else if (rel_ty < -HALF_L)
            rel_ty += L;

        float min_dist_sq = 1e18f;
        int best_parent = -1;

        for (const auto& p : active_rel) {
            float dx = std::abs(p.rx - rel_tx);
            float dy = std::abs(p.ry - rel_ty);
            if (dx > HALF_L)
                dx = L - dx;
            if (dy > HALF_L)
                dy = L - dy;

            float d2 = dx * dx + dy * dy;
            if (d2 < min_dist_sq) {
                min_dist_sq = d2;
                best_parent = p.id;
            }
        }

        out_ops.emplace_back(t, best_parent, target_idx);
        active_rel.push_back({target_idx, rel_tx, rel_ty});

        cvx += vel_cache[target_idx][0];
        cvy += vel_cache[target_idx][1];
        current_mass++;
        current_time = t;
    }
}

// -----------------------------------------------------
// 厳密シミュレーター
// 問題文の定義に従い、double精度で最初からシミュレーションを行い、
// 正確なコストとスコアを計算する関数
// -----------------------------------------------------
long long compute_strict_score(int N_val, int T_val, int M_val, double L_val,
    const vector<Point>& initial_points,
    const vector<tuple<int, int, int>>& ops) {
    // 初期化
    vector<double> x(N_val), y(N_val), vx(N_val), vy(N_val);
    vector<int> component(N_val);
    vector<vector<int>> members(N_val);

    for (int i = 0; i < N_val; ++i) {
        x[i] = initial_points[i].x;
        y[i] = initial_points[i].y;
        vx[i] = initial_points[i].vx;
        vy[i] = initial_points[i].vy;
        component[i] = i;
        members[i].push_back(i);
    }

    // 操作を時刻ごとに整理
    vector<vector<pair<int, int>>> schedule(T_val);
    for (const auto& op : ops) {
        schedule[get<0>(op)].push_back({get<1>(op), get<2>(op)});
    }

    double total_cost = 0;

    // 時間発展シミュレーション
    for (int t = 0; t < T_val; ++t) {
        // 1. 結合フェーズ
        for (const auto& p : schedule[t]) {
            int i = p.first;
            int j = p.second;

            // 既に同じ成分ならスキップ（通常は発生しないはず）
            if (component[i] == component[j])
                continue;

            // 距離計算
            double dx = std::abs(x[i] - x[j]);
            double dy = std::abs(y[i] - y[j]);
            double dist_x = std::min(dx, L_val - dx);
            double dist_y = std::min(dy, L_val - dy);

            // round(sqrt(...))
            double dist = std::sqrt(dist_x * dist_x + dist_y * dist_y);
            long long cost = std::llround(dist);
            total_cost += cost;

            // 結合処理
            int root_i = component[i];
            int root_j = component[j];

            // 運動量保存則による新速度
            double size_i = (double)members[root_i].size();
            double size_j = (double)members[root_j].size();
            double new_vx = (size_i * vx[members[root_i][0]] + size_j * vx[members[root_j][0]]) / (size_i + size_j);
            double new_vy = (size_i * vy[members[root_i][0]] + size_j * vy[members[root_j][0]]) / (size_i + size_j);

            // 成分のマージ (小さい方を大きい方にマージするなどしても良いが、ここではjをiに入れる)
            for (int member_idx : members[root_j]) {
                component[member_idx] = root_i;
                members[root_i].push_back(member_idx);
            }
            members[root_j].clear();  // jは空になる

            // 新しい速度を成分全員に適用
            for (int member_idx : members[root_i]) {
                vx[member_idx] = new_vx;
                vy[member_idx] = new_vy;
            }
        }

        // 2. 移動フェーズ
        for (int i = 0; i < N_val; ++i) {
            x[i] += vx[i];
            y[i] += vy[i];

            // トーラス処理 (doubleなのでfmodまたはifループ)
            // L_val = 100000.0, vx ~ 100.0 なので if で十分
            if (x[i] >= L_val)
                x[i] -= L_val;
            else if (x[i] < 0.0)
                x[i] += L_val;
            // 念のためwhile (稀に大きな値になる場合の安全策)
            if (x[i] >= L_val)
                x[i] = std::fmod(x[i], L_val);
            if (x[i] < 0.0)
                x[i] = std::fmod(x[i], L_val) + L_val;

            if (y[i] >= L_val)
                y[i] -= L_val;
            else if (y[i] < 0.0)
                y[i] += L_val;
            if (y[i] >= L_val)
                y[i] = std::fmod(y[i], L_val);
            if (y[i] < 0.0)
                y[i] = std::fmod(y[i], L_val) + L_val;
        }
    }

    // スコア計算
    // D_sumが小さいほど良い -> 1e6 * log2( CONST / (D + 1) )
    double score_val = 1000000.0 * std::log2((L_val * (N_val - M_val)) / (total_cost + 1.0));
    return std::llround(score_val);
}

// --- Main ---

int main() {
    cin.tie(nullptr);
    ios::sync_with_stdio(false);

    // Hyperparameters init
    hp::init();

    auto start_clock = chrono::steady_clock::now();

    // Time allocation from Hyperparameters
    const double time_limit_total = hp::TIME_LIMIT_TOTAL;
    const double time_limit_stage1 = time_limit_total * hp::TIME_RATIO_STAGE1;

    int n_in, t_in, m_in, k_in;
    long long l_in;
    if (!(cin >> n_in >> t_in >> m_in >> k_in >> l_in))
        return 0;

    vector<Point> points(N);
    for (int i = 0; i < N; ++i) {
        cin >> points[i].x >> points[i].y >> points[i].vx >> points[i].vy;
        vel_cache[i][0] = points[i].vx;
        vel_cache[i][1] = points[i].vy;
    }

    precompute_positions(points);

    // Initial Solution Construction (Distance based)
    Solution sol;
    vector<pair<int, float>> sort_helper(N);
    for (int i = 0; i < N; ++i) {
        float px = pos_cache[i][500][0];
        float py = pos_cache[i][500][1];
        sort_helper[i] = {i, px + py * 0.0001f};
    }
    sort(sort_helper.begin(), sort_helper.end(), [](auto& a, auto& b) {
        return a.second < b.second;
    });

    vector<int> p_indices(N);
    for (int i = 0; i < N; ++i)
        p_indices[i] = sort_helper[i].first;

    for (int m = 0; m < M; ++m) {
        sol.clusters[m].core_idx = p_indices[m * K];
        for (int k = 0; k < K_MERGES; ++k) {
            sol.clusters[m].merges[k] = {p_indices[m * K + k + 1], 0};
        }
    }

    // Init timings roughly (spread out)
    for (int m = 0; m < M; ++m) {
        for (int k = 0; k < K_MERGES; ++k) {
            sol.clusters[m].merges[k].time = (k + 1) * (T_MAX / K);
        }
    }

    // ==========================================
    // STAGE 1: Approximate SA (Center of Mass)
    // ==========================================

    vector<long long> cluster_costs(M);
    for (int m = 0; m < M; ++m) {
        cluster_costs[m] = eval_cluster_approx(sol.clusters[m], points);
    }

    // Stage 1 Probabilities Thresholds
    const int S1_THRESH_1 = hp::S1_PROB_TIME_MODIFY;
    const int S1_THRESH_2 = S1_THRESH_1 + hp::S1_PROB_SWAP_LEAF;
    
    // 時間計測を一定イテレーションおきにするための変数
    double progress_s1 = 0.0;
    float temp_s1 = (float)hp::S1_TEMP_BEGIN;

    {
        int iter_count = 0;

        while (true) {
            iter_count++;
            
            // 1024イテレーションごとに時間チェックと温度更新
            if ((iter_count & 1023) == 0) {
                auto now = chrono::steady_clock::now();
                double elapsed = chrono::duration<double>(now - start_clock).count();
                if (elapsed > time_limit_stage1)
                    break;
                
                progress_s1 = elapsed / time_limit_stage1;
                if (progress_s1 > 1.0)
                    progress_s1 = 1.0;

                // Use Hyperparameters for Temperature
                temp_s1 = (float)calc_temp(progress_s1,
                    hp::S1_TEMP_BEGIN,
                    hp::S1_TEMP_END,
                    hp::S1_TEMP_LINEAR_EXP_RATIO,
                    hp::S1_TEMP_OFFSET_RATIO);
            }

            // Temperature is updated only when time is measured
            float temp = temp_s1; 

            int type = rng.next_int(100);

            if (type < S1_THRESH_1) {  // Time Modify
                int c_idx = rng.next_int(M);
                int op_idx = rng.next_int(K_MERGES);
                int old_t = sol.clusters[c_idx].merges[op_idx].time;

                int new_t = old_t + (rng.next_int(101) - 50);
                if (new_t < 0)
                    new_t = 0;
                if (new_t >= T_MAX)
                    new_t = T_MAX - 1;

                if (new_t == old_t)
                    continue;

                int new_op_idx = set_time_sorted(sol.clusters[c_idx], op_idx, new_t);
                long long new_cost = eval_cluster_approx(sol.clusters[c_idx], points);
                long long diff = new_cost - cluster_costs[c_idx];

                if (diff <= 0 || rng.next_float() < std::exp(-diff / temp)) {
                    cluster_costs[c_idx] = new_cost;
                } else {
                    set_time_sorted(sol.clusters[c_idx], new_op_idx, old_t);  // Revert
                }

            } else if (type < S1_THRESH_2) {  // Swap Leaf
                int c1 = rng.next_int(M);
                int c2 = rng.next_int(M);
                if (c1 == c2)
                    continue;

                int op1 = rng.next_int(K_MERGES);
                int op2 = rng.next_int(K_MERGES);

                int p1 = sol.clusters[c1].merges[op1].target_idx;
                int p2 = sol.clusters[c2].merges[op2].target_idx;

                sol.clusters[c1].merges[op1].target_idx = p2;
                sol.clusters[c2].merges[op2].target_idx = p1;

                long long nc1 = eval_cluster_approx(sol.clusters[c1], points);
                long long nc2 = eval_cluster_approx(sol.clusters[c2], points);
                long long diff = (nc1 + nc2) - (cluster_costs[c1] + cluster_costs[c2]);

                if (diff <= 0 || rng.next_float() < std::exp(-diff / temp)) {
                    cluster_costs[c1] = nc1;
                    cluster_costs[c2] = nc2;
                } else {
                    sol.clusters[c1].merges[op1].target_idx = p1;
                    sol.clusters[c2].merges[op2].target_idx = p2;
                }

            } else {  // Swap Core (Remaining Prob)
                int c_idx = rng.next_int(M);
                int op_idx = rng.next_int(K_MERGES);
                int old_core = sol.clusters[c_idx].core_idx;
                int old_leaf = sol.clusters[c_idx].merges[op_idx].target_idx;

                sol.clusters[c_idx].core_idx = old_leaf;
                sol.clusters[c_idx].merges[op_idx].target_idx = old_core;

                long long new_cost = eval_cluster_approx(sol.clusters[c_idx], points);
                long long diff = new_cost - cluster_costs[c_idx];

                if (diff <= 0 || rng.next_float() < std::exp(-diff / temp)) {
                    cluster_costs[c_idx] = new_cost;
                } else {
                    sol.clusters[c_idx].core_idx = old_core;
                    sol.clusters[c_idx].merges[op_idx].target_idx = old_leaf;
                }
            }
        }
    }

    // ==========================================
    // STAGE 2: Exact Topology SA
    // ==========================================

    for (int m = 0; m < M; ++m) {
        cluster_costs[m] = eval_cluster_exact(sol.clusters[m], points);
    }

    // Stage 2 Probabilities Thresholds
    const int S2_THRESH_1 = hp::S2_PROB_TIME_SHIFT;
    // Remainder is ORDER_SWAP
    
    // Stage 2の開始時刻を記録
    auto now_s2_start = chrono::steady_clock::now();
    double start_elapsed = chrono::duration<double>(now_s2_start - start_clock).count();
    
    // 時間計測を一定イテレーションおきにするための変数
    double progress_s2 = 0.0;
    float temp_s2 = (float)hp::S2_TEMP_BEGIN;

    {
        int iter_count = 0;

        while (true) {
            iter_count++;
            
            // 256イテレーションごとに時間チェックと温度更新
            if ((iter_count & 255) == 0) {
                auto now = chrono::steady_clock::now();
                double elapsed = chrono::duration<double>(now - start_clock).count();
                if (elapsed > time_limit_total)
                    break;
                
                progress_s2 = (elapsed - start_elapsed) / (time_limit_total - start_elapsed);
                if (progress_s2 < 0.0)
                    progress_s2 = 0.0;
                if (progress_s2 > 1.0)
                    progress_s2 = 1.0;

                // Use Hyperparameters for Temperature
                temp_s2 = (float)calc_temp(progress_s2,
                    hp::S2_TEMP_BEGIN,
                    hp::S2_TEMP_END,
                    hp::S2_TEMP_LINEAR_EXP_RATIO,
                    hp::S2_TEMP_OFFSET_RATIO);
            }
            
            // Temperature is updated only when time is measured
            float temp = temp_s2;

            int c_idx = rng.next_int(M);
            int type = rng.next_int(100);

            if (type < S2_THRESH_1) {  // Time Shift
                int op_idx = rng.next_int(K_MERGES);
                int old_t = sol.clusters[c_idx].merges[op_idx].time;

                int new_t = old_t + (rng.next_int(61) - 30);
                if (new_t < 0)
                    new_t = 0;
                if (new_t >= T_MAX)
                    new_t = T_MAX - 1;

                if (new_t == old_t)
                    continue;

                int new_op_idx = set_time_sorted(sol.clusters[c_idx], op_idx, new_t);
                long long new_cost = eval_cluster_exact(sol.clusters[c_idx], points);
                long long diff = new_cost - cluster_costs[c_idx];

                if (diff <= 0 || rng.next_float() < std::exp(-diff / temp)) {
                    cluster_costs[c_idx] = new_cost;
                } else {
                    set_time_sorted(sol.clusters[c_idx], new_op_idx, old_t);  // Revert
                }

            } else {  // Order Swap (Remaining Prob)
                int op1 = rng.next_int(K_MERGES);
                int op2 = rng.next_int(K_MERGES);
                if (op1 == op2)
                    continue;

                int t1 = sol.clusters[c_idx].merges[op1].target_idx;
                int t2 = sol.clusters[c_idx].merges[op2].target_idx;

                sol.clusters[c_idx].merges[op1].target_idx = t2;
                sol.clusters[c_idx].merges[op2].target_idx = t1;

                long long new_cost = eval_cluster_exact(sol.clusters[c_idx], points);
                long long diff = new_cost - cluster_costs[c_idx];

                if (diff <= 0 || rng.next_float() < std::exp(-diff / temp)) {
                    cluster_costs[c_idx] = new_cost;
                } else {
                    sol.clusters[c_idx].merges[op1].target_idx = t1;
                    sol.clusters[c_idx].merges[op2].target_idx = t2;
                }
            }
        }
    }

    // --- Final Output Generation ---
    long long final_cost_approx = 0;
    vector<tuple<int, int, int>> output_list;
    output_list.reserve(N - M);

    for (int m = 0; m < M; ++m) {
        generate_output_ops(sol.clusters[m], points, output_list);
        final_cost_approx += cluster_costs[m];  // This cost is from eval_cluster_exact (float based)
    }

    sort(output_list.begin(), output_list.end(), [](const auto& a, const auto& b) {
        return get<0>(a) < get<0>(b);
    });

    // 1. Calculate and output the low precision score
    double raw_score_low = 0;
    if (final_cost_approx > 0) {
        raw_score_low = 1e6 * std::log2((double)(L * (N - M)) / (double)(final_cost_approx + 1));
    }
    long long score_low = std::llround(raw_score_low);
    if (final_cost_approx == 0)
        score_low = 1000000000;  // avoid div 0 theoretically

    cerr << "[DATA] score_low_precision = " << score_low << endl;

    // 2. Calculate Strict Score using Double Precision Simulation
    //long long strict_score = compute_strict_score(N, T_MAX, M, (double)L, points, output_list);
    //cerr << "[DATA] score = " << strict_score << endl;

    for (const auto& op : output_list) {
        cout << get<0>(op) << " " << get<1>(op) << " " << get<2>(op) << "\n";
    }

    return 0;
}