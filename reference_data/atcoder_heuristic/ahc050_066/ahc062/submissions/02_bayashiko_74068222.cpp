#pragma GCC optimize("Ofast,unroll-loops,fast-math")

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstring>

using namespace std;

const int N = 200;
const int M = N * N;

// Larger blocks = fewer lazy updates for suffix add (79 vs 160)
constexpr int g_block_shift = 9;
constexpr int g_block_size = 1 << g_block_shift;
constexpr int g_block_count = (M + g_block_size - 1) >> g_block_shift;

uint16_t A_val[M];
uint16_t p[M];
uint16_t pos[M];

uint32_t sum_A[M];
long long sum_iA[M];
long long lazy_iA[g_block_count + 1] __attribute__((aligned(64)));

// OPT: Pack x,y into single uint16_t to halve cache lines in is_adj
// Low byte = x, high byte = y
uint16_t xy_of[M];

int vis_stamp[M];
int current_stamp = 1;

const int8_t dir_x[] = {-1, 0, 1, -1, 1, -1, 0, 1};
const int8_t dir_y[] = {-1, -1, -1, 0, 0, 1, 1, 1};

uint16_t adj_list[M][8];
uint8_t adj_deg[M];

// OPT: Branchless is_adj with packed coordinates
inline bool is_adj(int u, int v) {
    uint16_t a = xy_of[u], b = xy_of[v];
    unsigned dx = (unsigned)((int)(a & 0xFF) - (int)(b & 0xFF) + 1);
    unsigned dy = (unsigned)((int)(a >> 8) - (int)(b >> 8) + 1);
    return (dx <= 2u) & (dy <= 2u);
}

// Inline is_adj check with precomputed x,y of one side
inline bool is_adj_xy(int ax, int ay, int v) {
    uint16_t b = xy_of[v];
    unsigned dx = (unsigned)(ax - (int)(b & 0xFF) + 1);
    unsigned dy = (unsigned)(ay - (int)(b >> 8) + 1);
    return (dx <= 2u) & (dy <= 2u);
}

static uint32_t rng_x = 123456789, rng_y = 362436069, rng_z = 521288629, rng_w = 88675123;

inline uint32_t xor128() {
    uint32_t t = rng_x ^ (rng_x << 11);
    rng_x = rng_y; rng_y = rng_z; rng_z = rng_w;
    return rng_w = (rng_w ^ (rng_w >> 19)) ^ (t ^ (t >> 8));
}

inline uint32_t next_rand(uint32_t range) {
    return ((uint64_t)xor128() * range) >> 32;
}

double get_time() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - start).count();
}

constexpr double g_accept_mult = 18.071866295048764;
constexpr int g_p_normal = 39;
constexpr int g_p_end = 46;
constexpr int g_p_start = 53;
constexpr int g_time_check_mask = 8191;

// OPT: Fast exp (Schraudolph 1999) — ~5x faster than std::exp
inline double fast_exp(double x) {
    union { double d; int64_t i; } u;
    u.i = (int64_t)(6497320848556798.0 * x + 4606794787188903936.0);
    return u.d;
}

inline bool accept(long long delta, double temp) {
    if (delta >= 0) return true;
    double nd = (double)delta;
    if (nd < -g_accept_mult * temp) return false;
    return fast_exp(nd / temp) > (double)xor128() * 2.3283064365386963e-10;
}

inline long long get_sum_A(int i) { return sum_A[i]; }
inline long long get_sum_iA(int i) { return sum_iA[i] + lazy_iA[i >> g_block_shift]; }

inline void add_lazy_iA(int L, int R, long long delta) {
    if (L > R) return;
    int bL = L >> g_block_shift;
    int bR = R >> g_block_shift;
    if (bL == bR) {
        for (int i = L; i <= R; ++i) sum_iA[i] += delta;
    } else {
        int end_bL = ((bL + 1) << g_block_shift) - 1;
        for (int i = L; i <= end_bL; ++i) sum_iA[i] += delta;
        for (int b = bL + 1; b < bR; ++b) lazy_iA[b] += delta;
        int start_bR = bR << g_block_shift;
        for (int i = start_bR; i <= R; ++i) sum_iA[i] += delta;
    }
}

inline void recalc_sums(int L, int R) {
    uint32_t cur_sumA = (L > 0) ? sum_A[L - 1] : 0;
    long long cur_sumiA = (L > 0) ? (sum_iA[L - 1] + lazy_iA[(L - 1) >> g_block_shift]) : 0;
    int i = L;
    while (i <= R) {
        int block_idx = i >> g_block_shift;
        int end_i = min(R, ((block_idx + 1) << g_block_shift) - 1);
        long long lazy_val = lazy_iA[block_idx];
        for (; i <= end_i; ++i) {
            uint32_t val = A_val[p[i]];
            cur_sumA += val;                 sum_A[i] = cur_sumA;
            cur_sumiA += (long long)i * val; sum_iA[i] = cur_sumiA - lazy_val;
        }
    }
}

// OPT: Precompute p[a+1] coords to avoid repeated xy_of lookups
void try_2opt_normal(double temp) {
    int a = next_rand(M - 2);
    int u = p[a];
    int u_next = p[a + 1];
    uint16_t xy_next = xy_of[u_next];
    int x_next = xy_next & 0xFF;
    int y_next = xy_next >> 8;

    int valid_b[8]; int num_b = 0;
    int deg = adj_deg[u];
    for (int i = 0; i < deg; ++i) {
        int b = pos[adj_list[u][i]];
        if (b > a + 1 && b < M - 1) {
            if (is_adj_xy(x_next, y_next, p[b + 1])) valid_b[num_b++] = b;
        }
    }
    if (num_b == 0) return;
    int b = valid_b[next_rand(num_b)];

    long long delta = (long long)(b + a + 1) * (get_sum_A(b) - get_sum_A(a)) - 2 * (get_sum_iA(b) - get_sum_iA(a));

    if (accept(delta, temp)) {
        std::reverse(p + a + 1, p + b + 1);
        for (int i = a + 1; i <= b; ++i) pos[p[i]] = i;
        recalc_sums(a + 1, b);
        add_lazy_iA(b + 1, M - 1, delta);
    }
}

void try_2opt_end(double temp) {
    int u = p[M - 1];
    int valid_a[8]; int num_a = 0;

    int deg = adj_deg[u];
    for (int i = 0; i < deg; ++i) {
        int a = pos[adj_list[u][i]];
        if (a < M - 2) valid_a[num_a++] = a;
    }
    if (num_a == 0) return;
    int a = valid_a[next_rand(num_a)];

    long long delta = (long long)(M - 1 + a + 1) * (get_sum_A(M - 1) - get_sum_A(a)) - 2 * (get_sum_iA(M - 1) - get_sum_iA(a));

    if (accept(delta, temp)) {
        std::reverse(p + a + 1, p + M);
        for (int i = a + 1; i < M; ++i) pos[p[i]] = i;
        recalc_sums(a + 1, M - 1);
    }
}

void try_2opt_start(double temp) {
    int u = p[0];
    int valid_b[8]; int num_b = 0;

    int deg = adj_deg[u];
    for (int i = 0; i < deg; ++i) {
        int b_cand = pos[adj_list[u][i]] - 1;
        if (b_cand > 0 && b_cand < M - 1) valid_b[num_b++] = b_cand;
    }
    if (num_b == 0) return;
    int b = valid_b[next_rand(num_b)];

    long long delta = (long long)b * get_sum_A(b) - 2 * get_sum_iA(b);

    if (accept(delta, temp)) {
        std::reverse(p, p + b + 1);
        for (int i = 0; i <= b; ++i) pos[p[i]] = i;
        recalc_sums(0, b);
        add_lazy_iA(b + 1, M - 1, delta);
    }
}

void try_block_move(double temp) {
    int a = next_rand(M - 2);
    int u = p[a];
    int valid_b[8]; int num_b = 0;

    int deg = adj_deg[u];
    for (int i = 0; i < deg; ++i) {
        int b_cand = pos[adj_list[u][i]] - 1;
        if (b_cand >= a + 1 && b_cand < M - 1) valid_b[num_b++] = b_cand;
    }
    if (num_b == 0) return;
    int b = valid_b[next_rand(num_b)];

    int u2 = p[a + 1];
    int pb = p[b];
    uint16_t xy_pb = xy_of[pb];
    int x_pb = xy_pb & 0xFF;
    int y_pb = xy_pb >> 8;

    int valid_c[8]; int num_c = 0;
    deg = adj_deg[u2];
    for (int i = 0; i < deg; ++i) {
        int c_cand = pos[adj_list[u2][i]];
        if (c_cand < M - 1 && (c_cand < a || c_cand > b)) {
            if (is_adj_xy(x_pb, y_pb, p[c_cand + 1])) valid_c[num_c++] = c_cand;
        }
    }
    if (num_c == 0) return;
    int c = valid_c[next_rand(num_c)];

    long long delta;
    if (c < a) delta = (long long)(c - a) * (get_sum_A(b) - get_sum_A(a)) + (long long)(b - a) * (get_sum_A(a) - get_sum_A(c));
    else       delta = -(long long)(b - a) * (get_sum_A(c) - get_sum_A(b)) + (long long)(c - b) * (get_sum_A(b) - get_sum_A(a));

    if (accept(delta, temp)) {
        if (c < a) {
            std::rotate(p + c + 1, p + a + 1, p + b + 1);
            for (int i = c + 1; i <= b; ++i) pos[p[i]] = i;
            recalc_sums(c + 1, b);
            add_lazy_iA(b + 1, M - 1, delta);
        } else {
            std::rotate(p + a + 1, p + b + 1, p + c + 1);
            for (int i = a + 1; i <= c; ++i) pos[p[i]] = i;
            recalc_sums(a + 1, c);
            add_lazy_iA(c + 1, M - 1, delta);
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int dummy_N;
    if (!(cin >> dummy_N)) return 0;
    for (int i = 0; i < M; ++i) cin >> A_val[i];

    for (int i = 0; i < M; ++i) {
        int cx = i % N, cy = i / N, deg = 0;
        xy_of[i] = (uint16_t)((cy << 8) | cx);
        for (int dir = 0; dir < 8; ++dir) {
            int nx = cx + dir_x[dir], ny = cy + dir_y[dir];
            if (nx >= 0 && nx < N && ny >= 0 && ny < N) adj_list[i][deg++] = ny * N + nx;
        }
        adj_deg[i] = deg;
    }

    memset(lazy_iA, 0, sizeof(lazy_iA));

    bool built = false;
    for (int attempts = 0; attempts < 10; ++attempts) {
        ++current_stamp;
        if (current_stamp == 0) { fill(vis_stamp, vis_stamp + M, 0); current_stamp = 1; }
        int start_node = next_rand(M);
        p[0] = start_node; vis_stamp[start_node] = current_stamp;

        bool ok = true;
        for (int i = 1; i < M; ++i) {
            int best_v = -1, min_deg = 9, min_A = 1000000000;
            int deg = adj_deg[p[i - 1]];
            for (int j = 0; j < deg; ++j) {
                int v = adj_list[p[i - 1]][j];
                if (vis_stamp[v] != current_stamp) {
                    int d2_cnt = 0, d2_deg = adj_deg[v];
                    for (int k = 0; k < d2_deg; ++k) d2_cnt += (vis_stamp[adj_list[v][k]] != current_stamp);
                    if (d2_cnt < min_deg || (d2_cnt == min_deg && A_val[v] < min_A)) {
                        min_deg = d2_cnt; min_A = A_val[v]; best_v = v;
                    }
                }
            }
            if (best_v == -1) { ok = false; break; }
            p[i] = best_v; vis_stamp[best_v] = current_stamp;
        }
        if (ok) { built = true; break; }
    }

    if (!built) {
        int idx = 0;
        for (int y = 0; y < N; ++y) for (int x = 0; x < N; ++x) p[idx++] = y * N + (y % 2 == 0 ? x : N - 1 - x);
    }
    for (int i = 0; i < M; ++i) pos[p[i]] = i;

    uint32_t cur_sumA = 0;
    long long cur_sumiA = 0;
    for (int i = 0; i < M; ++i) {
        uint32_t val = A_val[p[i]];
        cur_sumA += val;                 sum_A[i] = cur_sumA;
        cur_sumiA += (long long)i * val; sum_iA[i] = cur_sumiA;
    }

    double time_limit = 2.985;
    double T0 = 244665375.55216876;
    double T1 = 1959913.2583149767;
    double log_T0 = log(T0);
    double log_ratio = log(T1 / T0);
    double inv_tl = 1.0 / time_limit;
    double temp = T0;

    int iter = 0;
    while (true) {
        if ((iter & g_time_check_mask) == 0) {
            double ct = get_time();
            if (ct > time_limit) break;
            temp = exp(log_T0 + log_ratio * ct * inv_tl);
        }

        int r = next_rand(100);
        if (r < g_p_normal) try_2opt_normal(temp);
        else if (r < g_p_end) try_2opt_end(temp);
        else if (r < g_p_start) try_2opt_start(temp);
        else try_block_move(temp);

        iter++;
    }

    for (int i = 0; i < M; ++i) cout << (p[i] / N) << " " << (p[i] % N) << "\n";
    return 0;
}
