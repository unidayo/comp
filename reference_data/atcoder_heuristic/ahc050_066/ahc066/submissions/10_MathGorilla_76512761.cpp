constexpr long long time_limit = (2000 - 15) * 1;
#pragma region
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
#include <atcoder/all>
#if PAHCER
#include <sys/resource.h>
#endif
using namespace std;
using namespace atcoder;

#define rep(i, begin, end) for (int i = begin; i < end; i++)
#define all(x) (x).begin(), (x).end()
using ll = long long;
using pi = pair<int, int>;
template <class T> using V = vector<T>;
template <class T> using VV = vector<vector<T>>;
template <class T> inline bool chmin(T &a, const T b) { if (a > b) { a = b; return true; } return false; }
template <class T> inline bool chmax(T &a, const T b) { if (a < b) { a = b; return true; } return false; }
template <class T> istream &operator>>(istream &is, vector<T> &v) { for (auto &e : v) is >> e; return is; }

struct fast_ios {
    fast_ios() {
        cin.tie(nullptr);
        ios::sync_with_stdio(false);
    }
} fast_ios_;

#if DEBUG
constexpr float machine_spec = 0.01;
#elif PROFILER
constexpr float machine_spec = 0.5;
#elif LOCAL
constexpr float machine_spec = 1.4;
#elif PAHCER
constexpr float machine_spec = 0.75 * 1.25;
#else
constexpr float machine_spec = 1;
#endif

struct Timer {
    static long long now_us() {
        #if PAHCER or LOCAL or DEBUG or PROFILER
        rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return (long long)usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec
             + (long long)usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec;
        #else
        return chrono::duration_cast<chrono::microseconds>(
            chrono::steady_clock::now().time_since_epoch()
        ).count();
        #endif
    }

    long long start = now_us();
    long long lap = now_us();
    int elapsed_ms() {
        long long elapsed = now_us() - start;
        #if DEBUG or LOCAL or PAHCER or PROFILER
        return elapsed * machine_spec / 1000;
        #else
        return elapsed / 1000;
        #endif
    }
    int lap_ms() {
        long long elapsed = now_us() - lap;
        #if DEBUG or LOCAL or PAHCER or PROFILER
        return elapsed * machine_spec / 1000;
        #else
        return elapsed / 1000;
        #endif
    }
    long long lap_us() {
        long long elapsed = now_us() - lap;
        #if DEBUG or LOCAL or PAHCER or PROFILER
        return elapsed * machine_spec;
        #else
        return elapsed;
        #endif
    }
    void mark_lap() { lap = now_us(); }
} timer;

long long build_all_pairs_shortest_path_call_count = 0;
long long build_all_pairs_shortest_path_total_us = 0;

namespace Log {
constexpr const char *INDENT = "    ";
constexpr const char *INDENT2 = "        ";

string macro_rle(const string &macro) {
    string res;
    rep(i, 0, (int)macro.size()) {
        int j = i + 1;
        while (j < (int)macro.size() && macro[j] == macro[i]) j++;
        if (!res.empty()) res.push_back(' ');
        res.push_back(macro[i]);
        if (j - i > 1) res += to_string(j - i);
        i = j - 1;
    }
    return res.empty() ? "-" : res;
}

void section(const string &name) {
    cerr << "\n[" << name << "]\n";
}

string percent(long long numerator, long long denominator) {
    ostringstream oss;
    double value = numerator == 0 || denominator == 0
        ? 0.0
        : ceil(1000.0 * numerator / denominator - 1e-12) / 10.0;
    oss << fixed << setprecision(1) << value << '%';
    return oss.str();
}
}  // namespace Log

class xrand {
    uint64_t x;
public:
    using result_type = uint32_t;
    static constexpr result_type min() { return numeric_limits<result_type>::min(); }
    static constexpr result_type max() { return numeric_limits<result_type>::max(); }
    xrand(uint64_t k = 998244353) : x(k) {}
    result_type operator()() {
        x ^= x << 9;
        return x ^= x >> 7;
    }
    result_type operator()(int m) { return int64_t((*this)()) * m >> 32; }
    result_type operator()(int l_include, int r_exclude) { return l_include + (*this)(r_exclude - l_include); }
    double rand() { return double((*this)()) / (int64_t(1) << 32); }
} rnd;

class LogRandTable {
    static constexpr int table_size = 1000;
    float log_rand_table[table_size];
public:
    LogRandTable() {
        rep(i, 0, table_size) log_rand_table[i] = log((i + 0.5f) / table_size);
    }
    float operator()() { return log_rand_table[rnd(table_size)]; }
} log_rand_table;

class AliasWeightTable {
    static constexpr int MAX_COST_SUM = 16 * 4 * 20 * 20;
    array<float, MAX_COST_SUM + 1> table;
public:
    AliasWeightTable() {
        rep(cost_sum, 0, MAX_COST_SUM + 1) {
            table[cost_sum] = pow(4.0f, -cost_sum / 16.0f);
        }
    }
    float operator[](int cost_sum) const {
        assert(0 <= cost_sum && cost_sum <= MAX_COST_SUM);
        return table[cost_sum];
    }
} alias_weight_table;

// Walker–Vose Alias method
// std::discrete_distribution より高速なサンプリング
struct AliasSampler {
    int n = 0;
    vector<uint64_t> entries;
    float sumW = 0.0f;
    vector<double> scaled;
    vector<int> work;

    static uint64_t make_entry(double probability, int alias) {
        probability = max(0.0, min(1.0, probability));
        uint32_t threshold = (uint32_t)(probability * numeric_limits<uint32_t>::max());
        return (uint64_t(threshold) << 32) | uint32_t(alias);
    }

    void build(const vector<float>& w) {
        n = (int)w.size();
        entries.resize(n);
        scaled.resize(n);
        work.resize(n);
        sumW = 0.0f;

        // 例外: N=0
        if (n == 0) throw runtime_error("empty weights");

        // 合計とゼロチェック
        double S = 0.0;
        for (float x : w) { 
            if (x < 0) throw runtime_error("negative weight");
            S += (double)x;
        }
        // 例外: 全ゼロ → 一様分布にフォールバック
        if (S == 0.0) {
            for (int i = 0; i < n; ++i) entries[i] = make_entry(1.0, i);
            sumW = 0.0f;
            return;
        }
        sumW = (float)S;

        const double scale = n / S;
        int small_size = 0;
        int large_begin = n;
        for (int i = 0; i < n; ++i) {
            scaled[i] = (double)w[i] * scale;
            if (scaled[i] < 1.0) work[small_size++] = i;
            else                 work[--large_begin] = i;
        }

        // 埋め合わせ
        while (small_size > 0 && large_begin < n) {
            const int s = work[--small_size];
            const int l = work[large_begin++];

            entries[s] = make_entry(scaled[s], l);

            scaled[l] -= 1.0 - scaled[s]; // 1の分だけ減らす
            if (scaled[l] < 1.0 - 1e-15) work[small_size++] = l;
            else                         work[--large_begin] = l;
        }

        // 端数処理（数値誤差で少しはみ出すことがある）
        while (large_begin < n) {
            const int l = work[large_begin++];
            entries[l] = make_entry(1.0, l);
        }
        while (small_size > 0) {
            const int s = work[--small_size];
            entries[s] = make_entry(1.0, s);
        }
    }

    template <class URBG>
    int sample(URBG& rng) const {
        uint64_t random_product = uint64_t(rng()) * uint32_t(n);
        int i = random_product >> 32;
        uint32_t coin = random_product;
        uint64_t entry = entries[i];
        uint32_t threshold = entry >> 32;
        int alias = uint32_t(entry);
        return coin <= threshold ? i : alias;
    }
};
#pragma endregion

constexpr int INF = 1 << 28;
constexpr unsigned short INF_DIST = numeric_limits<unsigned short>::max();
constexpr unsigned short INVALID_STATE = numeric_limits<unsigned short>::max();

// box -> ball 距離構築の早期打ち切りパラメータ
constexpr unsigned short BOX_TO_BALL_UNEXPLORED_DIST = 30;
constexpr bool ENABLE_BOX_TO_BALL_FIRST_DIST_CUTOFF = true;
constexpr int BOX_TO_BALL_FIRST_DIST_EXTRA = 10;
constexpr bool ENABLE_BOX_TO_BALL_PERCENT_CUTOFF = false;
constexpr int BOX_TO_BALL_TARGET_PERCENT = 45;

// innser-SAのパラメータ
constexpr int MAX_MACRO_LEN = 100;
// constexpr int INNER_SOLVER_TIME_MS = 3 * 3;
// constexpr int FINAL_SOLVER_TIME_MS = 300 * 3;
// constexpr int FINAL_SOLVER_SPLIT = 10;
constexpr float INNER_SA_TIME_MULTI = 0.5f;
constexpr int INNER_SOLVER_TIME_MS = 3 * INNER_SA_TIME_MULTI;
constexpr int FINAL_SOLVER_TIME_MS = 140;
constexpr int FINAL_SOLVER_SPLIT = 10;
constexpr double FINAL_SOLVER_SCORE_MARGIN = 1.02;
constexpr float INNER_SA_START_TEMP = 1.5f;
constexpr float FINAL_INNER_SA_START_TEMP_MULTI = 2.0f;
constexpr float INNER_SA_END_TEMP = 0.2f;
constexpr int INNER_CROSS_EXCHANGE_MAX_LEN = 4;
constexpr int INNER_CROSS_EXCHANGE_WEIGHT = 0;
constexpr bool ENABLE_EARLY_REJECT = true;
// constexpr int EARLY_REJECT_SOLVER_TIME_US = 500  * 3;
// constexpr int INITIAL_SEED_SOLVER_TIME_US = 500  * 3;
constexpr int EARLY_REJECT_SOLVER_TIME_US = 500 * INNER_SA_TIME_MULTI;
constexpr int INITIAL_SEED_SOLVER_TIME_US = 500 * INNER_SA_TIME_MULTI;
constexpr double EARLY_REJECT_SCORE_MARGIN = 1.02;
constexpr double FULL_EVAL_SCORE_MARGIN = 1.02;
// constexpr bool ENABLE_CACHE_REFRESH = true;
constexpr bool ENABLE_CACHE_REFRESH = true;
constexpr double CACHE_REFRESH_PROB = 0.1;
constexpr double CACHE_REFRESH_SCORE_MARGIN = 1.03;
constexpr bool ENABLE_ROUGH_CACHE = true;
constexpr double ROUGH_CACHE_REFRESH_PROB = 0.3;
constexpr double ROUGH_CACHE_REFRESH_SCORE_MARGIN = 1.03;


// 実験用パラメータ
constexpr bool ENABLE_INNER_SCORE_EXPERIMENT = false;
constexpr int EXPERIMENT_SAMPLE_MACROS = 30;
constexpr int EXPERIMENT_REPEAT = 30;
constexpr int EXPERIMENT_BUDGETS[] = {1, 3, 10, 30};
constexpr bool ENABLE_CHANGE_MACRO_EXPERIMENT = true;
constexpr int CHANGE_MACRO_EXPERIMENT_TIME_MS = 10;
constexpr float CHANGE_MACRO_EXPERIMENT_START_TEMP = 3.0f;
constexpr float CHANGE_MACRO_EXPERIMENT_END_TEMP = 0.1f;
constexpr int CHANGE_MACRO_EXPERIMENT_MAX_AFFIX_LEN = 6;
constexpr int TWO_MACRO_SOLVER_TIME_MS = 10;
constexpr int TWO_MACRO_SOLVER_RUNS = 10;
constexpr int TWO_MACRO_SOLVER_FINALISTS = 4;
constexpr int TWO_MACRO_SOLVER_FINAL_TIME_MS = 50;
constexpr float TWO_MACRO_SOLVER_START_TEMP = 1.0f;
constexpr float TWO_MACRO_SOLVER_END_TEMP = 0.15f;
constexpr int TWO_MACRO_SOLVER_MACRO_A_CANDIDATES = 6;
constexpr int TWO_MACRO_SOLVER_BATCH_GUARD_MS = 0;
constexpr int TWO_MACRO_OR_OPT_MAX_LEN = 8;
constexpr int TWO_MACRO_CROSS_EXCHANGE_MAX_LEN = 4;
constexpr float TWO_MACRO_FIRST_STAGE_KICK_STAGNATION_PROGRESS = 0.30f;
constexpr float TWO_MACRO_FINAL_STAGE_KICK_STAGNATION_PROGRESS = 0.02f;
constexpr int TWO_MACRO_KICK_MOVE_COUNT = 4;
constexpr int TWO_MACRO_KICK_MAX_TRIALS = 30;
constexpr int TWO_MACRO_NEIGHBOR_WEIGHTS[] = {
    1,  // change node dir
    1,  // swap
    4,  // move
    1,  // or-opt
    4,  // or-opt within macro
    1,  // change switch position
    1,  // move between macros while preserving other nodes' macro assignment
    1,  // cross-exchange within macro
};
const string CHANGE_MACRO_EXPERIMENT_INITIAL_PREFIX = "";
const string CHANGE_MACRO_EXPERIMENT_INITIAL_SUFFIX = "F";

// macro-searchのパラメータ
constexpr int multi = 1;
constexpr int TWO_MACRO_SOLVER_TOTAL_TIME_MS =
    TWO_MACRO_SOLVER_TIME_MS * TWO_MACRO_SOLVER_RUNS
    + TWO_MACRO_SOLVER_FINAL_TIME_MS * TWO_MACRO_SOLVER_FINALISTS
    + TWO_MACRO_SOLVER_BATCH_GUARD_MS;
constexpr int CHANGE_MACRO_EXPERIMENT_TOTAL_TIME_MS = CHANGE_MACRO_EXPERIMENT_TIME_MS * TWO_MACRO_SOLVER_MACRO_A_CANDIDATES;
constexpr int MACRO_SEARCH_TIME_MS = (time_limit - FINAL_SOLVER_TIME_MS - CHANGE_MACRO_EXPERIMENT_TOTAL_TIME_MS - TWO_MACRO_SOLVER_TOTAL_TIME_MS) * multi;
constexpr float MACRO_SA_START_TEMP = 1.5f;
constexpr float MACRO_SA_END_TEMP = 1.5f;
constexpr bool ENABLE_ADAPTIVE_MACRO_NEIGHBOR = false;
constexpr double MACRO_NEIGHBOR_PRIOR_TRIALS = 30.0;
constexpr double MACRO_NEIGHBOR_PRIOR_ACCEPT_RATE = 0.05;
constexpr int MACRO_NEIGHBOR_BASE_WEIGHTS[] = {
    2,  // insert
    2,  // remove
    0,  // single turn flip
    1,  // change tail turn
    1,  // insert block
    1,  // prepend F0 block
    1,  // resize block F-run
    2,  // jitter all block F-runs
    2,  // repeatedly jitter random block F-runs
    1,  // change pre-pre macro move
    1,  // toggle zero-gain pre-macro
    1,  // flip L/R turns in a block range
};
constexpr double MACRO_NEIGHBOR_MIN_ACCEPT_RATES[] = {
    0.01, 0.01, 0.00, 0.01, 0.01, 0.01, 0.01, 0.15, 0.15, 0.01, 0.01, 0.01,
};
constexpr double MACRO_NEIGHBOR_MAX_ACCEPT_RATES[] = {
    1.00, 1.00, 0.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 0.05, 1.00, 1.00,
};
const vector<pair<int, float>> MACRO_SEARCH_PHASES = {
    {10, 0.25f},
    {5, 0.25f},
    {3, 0.25f},
    {1, 0.25f},
};

// dirやactionの定数
constexpr int DIR_R = 0;
constexpr int DIR_D = 1;
constexpr int DIR_L = 2;
constexpr int DIR_U = 3;
const int di[4] = {0, 1, 0, -1};
const int dj[4] = {1, 0, -1, 0};
const char ACTION_CHARS[4] = {'F', 'R', 'L', 'P'};
const char MACRO_CHARS[8] = {'F', 'F', 'F', 'F', 'F', 'F', 'R', 'L'};
constexpr unsigned char DIR_PERMS[24][4] = {
    {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1},
    {0, 3, 1, 2}, {0, 3, 2, 1}, {1, 0, 2, 3}, {1, 0, 3, 2},
    {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0},
    {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 1, 0, 3}, {2, 1, 3, 0},
    {2, 3, 0, 1}, {2, 3, 1, 0}, {3, 0, 1, 2}, {3, 0, 2, 1},
    {3, 1, 0, 2}, {3, 1, 2, 0}, {3, 2, 0, 1}, {3, 2, 1, 0},
};



struct Input {
    int N, M, T;
    vector<string> v, h;
    vector<int> ball_cell, box_cell;
    float wall_ratio;

    int cell(int i, int j) const { return i * N + j; }

    bool has_internal_wall() const {
        for (const string &row : v) {
            if (row.find('1') != string::npos) return true;
        }
        for (const string &row : h) {
            if (row.find('1') != string::npos) return true;
        }
        return false;
    }

    void read() {
        cin >> N >> M >> T;
        v.resize(N);
        h.resize(N - 1);
        cin >> v >> h;
        ball_cell.assign(M + 1, 0);
        box_cell.assign(M + 1, 0);
        rep(k, 1, M + 1) {
            int b, c, d, e;
            cin >> b >> c >> d >> e;
            ball_cell[k] = cell(b, c);
            box_cell[k] = cell(d, e);
        }

        int wall_count = 0;
        for (const string &row : v) wall_count += count(all(row), '1');
        for (const string &row : h) wall_count += count(all(row), '1');
        wall_ratio = (double)wall_count / (N * (N - 1) * 2);
    }
};

struct SimResult {
    ll score;
    int correct;
    int expanded_basic;
};

SimResult compute_raw_score(const Input &in, const string &out) {
    if ((int)out.size() > in.T) {
        return {(ll)in.T * in.M + (int)out.size(), 0, 0};
    }

    vector<int> balls(in.N * in.N, -1);
    rep(k, 1, in.M + 1) balls[in.ball_cell[k]] = k;
    int pos = 0;
    int dir = DIR_R;
    int hand = -1;
    int expanded_basic = 0;
    string last_macro;
    string cur_macro;
    bool recording = false;

    auto can_move = [&](int cell, int d) {
        int i = cell / in.N;
        int j = cell % in.N;
        if (d == DIR_R) return j + 1 < in.N && in.v[i][j] == '0';
        if (d == DIR_L) return j - 1 >= 0 && in.v[i][j - 1] == '0';
        if (d == DIR_D) return i + 1 < in.N && in.h[i][j] == '0';
        return i - 1 >= 0 && in.h[i - 1][j] == '0';
    };

    auto apply_basic = [&](char c) {
        if (expanded_basic >= in.T) return false;
        if (c == 'F') {
            if (can_move(pos, dir)) pos += di[dir] * in.N + dj[dir];
        } else if (c == 'R') {
            dir = (dir + 1) & 3;
        } else if (c == 'L') {
            dir = (dir + 3) & 3;
        } else if (c == 'S') {
            swap(hand, balls[pos]);
        } else {
            assert(false);
        }
        expanded_basic++;
        return true;
    };

    for (char c : out) {
        if (expanded_basic >= in.T) break;
        if (c == 'F' || c == 'R' || c == 'L' || c == 'S') {
            if (!apply_basic(c)) break;
            if (recording) cur_macro.push_back(c);
        } else if (c == 'M') {
            if (recording) {
                last_macro = cur_macro;
                cur_macro.clear();
                recording = false;
            } else {
                cur_macro.clear();
                recording = true;
            }
        } else if (c == 'P') {
            string replay = last_macro;
            for (char b : replay) {
                if (!apply_basic(b)) break;
                if (recording) cur_macro.push_back(b);
            }
        } else {
            assert(false);
        }
    }

    int correct = 0;
    rep(k, 1, in.M + 1) {
        if (balls[in.box_cell[k]] == k) correct++;
    }
    ll score = (correct == in.M ? (ll)out.size() : (ll)in.T * (in.M - correct));
    return {score, correct, expanded_basic};
}

struct MacroSolver {
    const Input &in;
    // 現在評価しているマクロ文字列
    string macro;
    // pre_macro := 最終マクロ構築前に M pre_macro M で登録しておく部分文字列
    string pre_macro;
    // registration_macro := 初回 P 置換時に M ... M の中へ出力する、pre_macro を P で圧縮した body
    string registration_macro;
    // pre_pre_macro_move := pre_macro 登録前に初期位置をずらす操作。0/R/L のみ使う
    char pre_pre_macro_move = 0;
    // use_zero_gain_pre_macro := 手数得が 0 の pre_macro を初期位置ずらし目的で使うか
    bool use_zero_gain_pre_macro = false;
    // cells := 盤面セル数, states := (セル, 向き) の状態数
    int cells, states;
    // adj_basic[state, a] := state から基本操作 a(F/R/L) を1回行った後の状態
    vector<array<int, 3>> adj_basic;
    // graph_to[state, a] := state からボタン a(F/R/L/P) を1回押した後の状態
    vector<array<unsigned short, 4>> graph_to;
    // graph_to_packed[state] := graph_to[state][0..3] を 16bit ずつ詰めた BFS 用遷移
    vector<unsigned long long> graph_to_packed;
    // macro_to[state] := 現在のマクロを state から実行した後の状態
    vector<unsigned short> macro_to;
    // dist[source_row, dst_row] := source_states[source_row] から source_states[dst_row] への最短ボタン数
    vector<unsigned short> dist;
    // node_edge_cost[(prev.id, prev.dir_box, cur.id, cur.dir_ball, cur.dir_box)] := prev のかごから cur を運び終えるまでの局所コスト
    vector<int> node_edge_cost;
    // alias_after_to_id[cur_id] := cur_id の直前に置く id を、平均距離に基づいてサンプリング
    vector<AliasSampler> alias_after_to_id;
    // alias_before_from_id[prev_id] := prev_id の直後に置く id(1..M)を、平均距離に基づいてサンプリング
    vector<AliasSampler> alias_before_from_id;
    // ball_cell[id], box_cell[id] := id 番のボール位置・かご位置のセル番号。id=0 は番兵
    vector<int> ball_cell, box_cell;
    // source_states[row] := BFS 始点として距離を持つ状態
    vector<unsigned short> source_states;
    // source_index[state] := state が source_states の何行目か。BFS 始点でない場合は INF_DIST
    vector<unsigned short> source_index;
    // restore_parent[state] := path_buttons の復元用 BFS における state の親状態
    mutable vector<unsigned short> restore_parent;
    // restore_action[state] := path_buttons の復元用 BFS で親から state へ来るために押したボタン
    mutable vector<unsigned char> restore_action;
    // restore_q[i] := path_buttons の復元用 BFS キュー
    mutable vector<int> restore_q;
    // bfs_visit_tag[state] := 上位16bitが BFS 世代、下位16bitが発見 level
    vector<unsigned int> bfs_visit_tag;
    // bfs_origin_mask[state] := 最短距離で state へ到達できる同一セル内の開始向き bitset
    vector<unsigned char> bfs_origin_mask;
    // bfs_target_seen[state] := 現在の source cell から距離を保存する対象状態の世代
    vector<unsigned short> bfs_target_seen;
    // bfs_box_to_ball_target_seen[state] := 現在の source cell からの box -> ball 距離対象の世代
    vector<unsigned short> bfs_box_to_ball_target_seen;
    // bfs_exact_target_seen[state] := 早期打ち切り前に必ず距離を求める対象状態の世代
    vector<unsigned short> bfs_exact_target_seen;
    // bfs_q[i] := shortest path table 構築用 BFS キュー
    vector<unsigned short> bfs_q;
    // bfs_token := bfs_visit_tag の現在世代
    unsigned short bfs_token = 0;
    // bfs_target_token := bfs_target_seen の現在世代
    unsigned short bfs_target_token = 0;
    // mask_turn_extra[mask][src_dir] := mask 内の最寄り開始向きまで src_dir から回る追加コスト
    array<array<unsigned char, 4>, 16> mask_turn_extra{};
    // source_count := source_states の個数
    int source_count = 0;
    // source_cells := source_states に含まれるセルの重複なし一覧
    vector<int> source_cells;
    // source_cell_index[cell] := source_cells の index。source でないセルは -1
    vector<int> source_cell_index;
    // source_target_rows[cell_index] := その source cell 始点で保存する dst row 一覧
    vector<vector<unsigned short>> source_target_rows;
    // start_state := pre_macro 登録後の初期状態
    int start_state;
    // picked_before_pre_macro_id := (0,0) で pre_macro 前に拾い、最初にかごへ届けるボール
    int picked_before_pre_macro_id = 0;
    // score_base := raw_score に加えるマクロ登録固定コスト
    int score_base = 0;

    struct Node {
        int id;
        int dir_ball;
        int dir_box;
    };

    struct State {
        vector<Node> order;
        vector<int> pos;
        int raw_score = INF;
        int score = INF;
    };

    MacroSolver(const Input &in_, string macro_, char pre_pre_macro_move_ = 0,
                bool use_zero_gain_pre_macro_ = false)
        : in(in_),
          macro(std::move(macro_)),
          pre_pre_macro_move(pre_pre_macro_move_),
          use_zero_gain_pre_macro(use_zero_gain_pre_macro_) {
        cells = in.N * in.N;
        states = cells * 4;
        ball_cell = in.ball_cell;
        box_cell = in.box_cell;
        ball_cell[0] = box_cell[0] = 0;
        start_state = encode(0, DIR_R);
        build_mask_turn_extra();
        build_basic_graph();
        build_macro_registration();
        build_source_states();
        build_macro_graph();
        build_all_pairs_shortest_path();
        build_node_edge_cost_table();
        build_alias_samplers();
    }

    void set_macro(string macro_, char pre_pre_macro_move_ = 0, bool use_zero_gain_pre_macro_ = false) {
        macro = std::move(macro_);
        pre_pre_macro_move = pre_pre_macro_move_;
        use_zero_gain_pre_macro = use_zero_gain_pre_macro_;
        build_macro_registration();
        build_source_states();
        build_macro_graph();
        build_all_pairs_shortest_path();
        build_node_edge_cost_table();
        build_alias_samplers();
    }

    bool has_pre_pre_macro_move() const {
        return !pre_macro.empty() && (pre_pre_macro_move == 'R' || pre_pre_macro_move == 'L');
    }

    bool has_picked_before_pre_macro() const {
        return picked_before_pre_macro_id != 0;
    }

    int encode(int cell, int dir) const { return cell * 4 + dir; }

    bool can_move(int cell, int dir) const {
        int i = cell / in.N;
        int j = cell % in.N;
        if (dir == DIR_R) return j + 1 < in.N && in.v[i][j] == '0';
        if (dir == DIR_L) return j - 1 >= 0 && in.v[i][j - 1] == '0';
        if (dir == DIR_D) return i + 1 < in.N && in.h[i][j] == '0';
        return i - 1 >= 0 && in.h[i - 1][j] == '0';
    }

    int apply_basic_action(int state, int action) const {
        int cell = state / 4;
        int dir = state % 4;
        if (action == 0) {
            if (can_move(cell, dir)) cell += di[dir] * in.N + dj[dir];
        } else if (action == 1) {
            dir = (dir + 1) & 3;
        } else {
            dir = (dir + 3) & 3;
        }
        return encode(cell, dir);
    }

    void build_basic_graph() {
        adj_basic.assign(states, {});
        graph_to.assign(states, {});
        graph_to_packed.assign(states, 0);
        rep(s, 0, states) {
            rep(a, 0, 3) {
                adj_basic[s][a] = apply_basic_action(s, a);
                graph_to[s][a] = adj_basic[s][a];
            }
        }
    }

    void build_macro_graph() {
        if ((int)macro_to.size() != states) macro_to.resize(states);
        rep(s, 0, states) macro_to[s] = s;
        for (char c : macro) {
            int action = (c == 'F' ? 0 : (c == 'R' ? 1 : 2));
            rep(s, 0, states) macro_to[s] = adj_basic[macro_to[s]][action];
        }
        rep(s, 0, states) graph_to[s][3] = macro_to[s];
        rep(s, 0, states) {
            const auto &g = graph_to[s];
            graph_to_packed[s] = (unsigned long long)g[0]
                               | ((unsigned long long)g[1] << 16)
                               | ((unsigned long long)g[2] << 32)
                               | ((unsigned long long)g[3] << 48);
        }
    }

    int apply_sequence(int state, const string &ops) const {
        for (char c : ops) {
            int action = (c == 'F' ? 0 : (c == 'R' ? 1 : 2));
            state = adj_basic[state][action];
        }
        return state;
    }

    int count_non_overlapping_occurrences(const string &s, const string &pat) const {
        if (pat.empty() || s.size() < pat.size()) return 0;
        int cnt = 0;
        for (int i = 0; i + (int)pat.size() <= (int)s.size();) {
            if (s.compare(i, pat.size(), pat) == 0) {
                cnt++;
                i += pat.size();
            } else {
                i++;
            }
        }
        return cnt;
    }

    string compress_registration_macro(const string &pat) const {
        if (pat.empty()) return macro;
        string res;
        for (int i = 0; i < (int)macro.size();) {
            if (i + (int)pat.size() <= (int)macro.size() && macro.compare(i, pat.size(), pat) == 0) {
                res.push_back('P');
                i += pat.size();
            } else {
                res.push_back(macro[i]);
                i++;
            }
        }
        return res;
    }

    void add_pre_macro_candidate(vector<string> &candidates, const string &pat) const {
        if (pat.size() < 2 || pat.size() > macro.size()) return;
        candidates.push_back(pat);
    }

    void build_macro_registration() {
        pre_macro.clear();
        registration_macro = macro;

        int max_f_run = 0;
        int cur_f_run = 0;
        for (char c : macro) {
            if (c == 'F') {
                chmax(max_f_run, ++cur_f_run);
            } else {
                cur_f_run = 0;
            }
        }

        vector<string> candidates;
        rep(len, 2, max_f_run + 1) add_pre_macro_candidate(candidates, string(len, 'F'));
        rep(len, 1, max_f_run + 1) {
            string fs(len, 'F');
            add_pre_macro_candidate(candidates, "R" + fs);
            add_pre_macro_candidate(candidates, "L" + fs);
            add_pre_macro_candidate(candidates, fs + "R");
            add_pre_macro_candidate(candidates, fs + "L");
        }

        int best_gain = 0;
        string best_pre;
        string best_registration = macro;
        string zero_gain_pre;
        string zero_gain_registration;
        for (const string &cand : candidates) {
            int occurrences = count_non_overlapping_occurrences(macro, cand);
            int gain = ((int)cand.size() - 1) * occurrences - ((int)cand.size() + 2);
            if (gain > best_gain) {
                best_gain = gain;
                best_pre = cand;
                best_registration = compress_registration_macro(cand);
            } else if (gain == 0 && zero_gain_pre.empty()) {
                zero_gain_pre = cand;
                zero_gain_registration = compress_registration_macro(cand);
            }
        }
        if (best_gain == 0 && use_zero_gain_pre_macro && !zero_gain_pre.empty()) {
            best_pre = std::move(zero_gain_pre);
            best_registration = std::move(zero_gain_registration);
        }

        pre_macro = std::move(best_pre);
        registration_macro = std::move(best_registration);
        picked_before_pre_macro_id = 0;
        if (!pre_macro.empty()) {
            rep(id, 1, in.M + 1) if (in.ball_cell[id] == 0) {
                picked_before_pre_macro_id = id;
                break;
            }
        }
        if (macro.empty()) {
            score_base = 0;
        } else if (pre_macro.empty()) {
            score_base = (int)macro.size() + 1;
        } else {
            int pre_pre_cost = has_pre_pre_macro_move() ? 1 : 0;
            score_base = pre_pre_cost + (int)pre_macro.size() + 2 + (int)registration_macro.size() + 1;
        }
        if (has_picked_before_pre_macro()) score_base++;
        start_state = encode(0, DIR_R);
        if (has_pre_pre_macro_move()) {
            string move(1, pre_pre_macro_move);
            start_state = apply_sequence(start_state, move);
        }
        start_state = apply_sequence(start_state, pre_macro);
        ball_cell = in.ball_cell;
        box_cell = in.box_cell;
        ball_cell[0] = start_state / 4;
        box_cell[0] = has_picked_before_pre_macro()
            ? box_cell[picked_before_pre_macro_id]
            : start_state / 4;
    }

    void add_source_state(int state) {
        if (source_index[state] != INF_DIST) return;
        assert(state < INF_DIST);
        assert(source_states.size() < INF_DIST);
        source_index[state] = source_states.size();
        source_states.push_back(state);
    }

    void build_source_states() {
        source_states.clear();
        source_index.assign(states, INF_DIST);
        rep(id, 0, in.M + 1) rep(dir, 0, 4) add_source_state(encode(box_cell[id], dir));
        rep(id, 1, in.M + 1) rep(dir, 0, 4) add_source_state(encode(ball_cell[id], dir));
        if (has_picked_before_pre_macro()) {
            rep(dir, 0, 4) add_source_state(encode(start_state / 4, dir));
        }
        source_count = source_states.size();

        source_cells.clear();
        source_cell_index.assign(cells, -1);
        auto add_source_cell = [&](int cell) {
            if (source_cell_index[cell] != -1) return source_cell_index[cell];
            int idx = source_cells.size();
            source_cell_index[cell] = idx;
            source_cells.push_back(cell);
            return idx;
        };
        rep(row, 0, (int)source_states.size()) add_source_cell(source_states[row] / 4);

        source_target_rows.assign(source_cells.size(), {});
        rep(id, 0, in.M + 1) {
            int idx = source_cell_index[box_cell[id]];
            assert(idx != -1);
            rep(ball_id, 1, in.M + 1) rep(dir, 0, 4) {
                unsigned short row = source_index[encode(ball_cell[ball_id], dir)];
                assert(row != INF_DIST);
                source_target_rows[idx].push_back(row);
            }
        }
        rep(id, 1, in.M + 1) {
            int idx = source_cell_index[ball_cell[id]];
            assert(idx != -1);
            rep(dir, 0, 4) {
                unsigned short row = source_index[encode(box_cell[id], dir)];
                assert(row != INF_DIST);
                source_target_rows[idx].push_back(row);
            }
        }
        if (has_picked_before_pre_macro()) {
            int idx = source_cell_index[start_state / 4];
            assert(idx != -1);
            rep(dir, 0, 4) {
                unsigned short row = source_index[encode(box_cell[picked_before_pre_macro_id], dir)];
                assert(row != INF_DIST);
                source_target_rows[idx].push_back(row);
            }
        }
        for (auto &rows : source_target_rows) {
            sort(all(rows));
            rows.erase(unique(all(rows)), rows.end());
        }

    }

    int source_row(int src) const {
        int row = source_index[src];
        assert(row != INF_DIST);
        return row;
    }

    int table_index(int row, int dst_row) const {
        return row * source_count + dst_row;
    }

    int distance_from(int src, int dst) const {
        unsigned short d = dist[table_index(source_row(src), source_row(dst))];
        return d == INF_DIST ? INF : d;
    }

    int turn_distance(int from_dir, int to_dir) const {
        int diff = abs(from_dir - to_dir);
        return min(diff, 4 - diff);
    }

    void build_mask_turn_extra() {
        rep(mask, 0, 16) rep(src_dir, 0, 4) {
            int extra = 2;
            rep(origin_dir, 0, 4) if ((mask >> origin_dir) & 1) {
                chmin(extra, turn_distance(src_dir, origin_dir));
            }
            mask_turn_extra[mask][src_dir] = extra;
        }
    }

    bool is_target_row(const vector<unsigned short> &targets, unsigned short row) const {
        return binary_search(all(targets), row);
    }

    bool should_store_distance_entry(int src_row, int dst_row) const {
        int src_cell = source_states[src_row] / 4;
        int cell_idx = source_cell_index[src_cell];
        assert(cell_idx != -1);
        return is_target_row(source_target_rows[cell_idx], dst_row);
    }

    void fill_dist_rows_from_mask(int base_row, unsigned short dst_row, unsigned short base_dist, unsigned char mask) {
        rep(src_dir, 0, 4) {
            int extra = mask_turn_extra[mask][src_dir];
            unsigned short d = (base_dist >= INF_DIST - extra ? INF_DIST : (unsigned short)(base_dist + extra));
            dist[table_index(base_row + src_dir, dst_row)] = d;
        }
    }

    void build_all_pairs_shortest_path() {
        long long profile_start_us = Timer::now_us();
        source_count = source_states.size();
        int table_size = source_count * source_count;
        if ((int)dist.size() != table_size) {
            dist.resize(table_size);
        }
        if ((int)bfs_visit_tag.size() != states) {
            bfs_visit_tag.assign(states, 0);
            bfs_target_seen.assign(states, 0);
            bfs_box_to_ball_target_seen.assign(states, 0);
            bfs_exact_target_seen.assign(states, 0);
            bfs_origin_mask.resize(states);
            bfs_q.resize(states);
            bfs_token = 0;
            bfs_target_token = 0;
        }

        unsigned int *visit_tag = bfs_visit_tag.data();
        unsigned short *target_seen = bfs_target_seen.data();
        unsigned short *box_to_ball_target_seen = bfs_box_to_ball_target_seen.data();
        unsigned short *exact_target_seen = bfs_exact_target_seen.data();
        unsigned char *origin_mask = bfs_origin_mask.data();
        unsigned short *q = bfs_q.data();
        const unsigned short *src_idx = source_index.data();
        const unsigned long long *to = graph_to_packed.data();

        for (int src_cell : source_cells) {
            int cell_idx = source_cell_index[src_cell];
            int base_row = source_index[encode(src_cell, 0)];
            assert(base_row != INF_DIST);
            const vector<unsigned short> &target_rows = source_target_rows[cell_idx];
            int target_count = target_rows.size();
            int head = 0, tail = 0;
            int reached_sources = 0;
            int box_to_ball_target_count = 0;
            int reached_box_to_ball_targets = 0;
            int exact_target_count = 0;
            int reached_exact_targets = 0;
            int first_box_to_ball_dist = -1;
            bfs_target_token++;
            if (bfs_target_token == INF_DIST) {
                fill(all(bfs_target_seen), 0);
                fill(all(bfs_box_to_ball_target_seen), 0);
                fill(all(bfs_exact_target_seen), 0);
                target_seen = bfs_target_seen.data();
                box_to_ball_target_seen = bfs_box_to_ball_target_seen.data();
                exact_target_seen = bfs_exact_target_seen.data();
                bfs_target_token = 1;
            }
            for (unsigned short row : target_rows) {
                int target_state = source_states[row];
                target_seen[target_state] = bfs_target_token;
                rep(src_dir, 0, 4) {
                    dist[table_index(base_row + src_dir, row)] = BOX_TO_BALL_UNEXPLORED_DIST;
                }
            }
            bool is_box_source = false;
            rep(id, 0, in.M + 1) if (box_cell[id] == src_cell) {
                is_box_source = true;
                break;
            }
            if (is_box_source) {
                rep(id, 1, in.M + 1) rep(dir, 0, 4) {
                    int target_state = encode(ball_cell[id], dir);
                    if (box_to_ball_target_seen[target_state] != bfs_target_token) {
                        box_to_ball_target_seen[target_state] = bfs_target_token;
                        box_to_ball_target_count++;
                    }
                }
            }
            if (!is_box_source) {
                for (unsigned short row : target_rows) {
                    exact_target_seen[source_states[row]] = bfs_target_token;
                }
                exact_target_count = target_count;
            } else {
                rep(id, 1, in.M + 1) if (ball_cell[id] == src_cell) rep(dir, 0, 4) {
                    int target_state = encode(box_cell[id], dir);
                    if (exact_target_seen[target_state] != bfs_target_token) {
                        exact_target_seen[target_state] = bfs_target_token;
                        exact_target_count++;
                    }
                }
                if (has_picked_before_pre_macro() && start_state / 4 == src_cell) {
                    rep(dir, 0, 4) {
                        int target_state = encode(box_cell[picked_before_pre_macro_id], dir);
                        if (exact_target_seen[target_state] != bfs_target_token) {
                            exact_target_seen[target_state] = bfs_target_token;
                            exact_target_count++;
                        }
                    }
                }
            }
            bfs_token++;
            if (bfs_token == INF_DIST) {
                fill(all(bfs_visit_tag), 0);
                visit_tag = bfs_visit_tag.data();
                bfs_token = 1;
            }
            unsigned int bfs_base_tag = (unsigned int)bfs_token << 16;
            rep(dir, 0, 4) {
                int src = encode(src_cell, dir);
                visit_tag[src] = bfs_base_tag;
                origin_mask[src] = 1u << dir;
                q[tail++] = src;
            }
            int level_tail = tail;
            unsigned short cur_dist = 0;
            int percent_target_count =
                (box_to_ball_target_count * BOX_TO_BALL_TARGET_PERCENT + 99) / 100;
            while (head < tail && reached_sources < target_count) {
                bool exact_targets_reached = reached_exact_targets == exact_target_count;
                bool first_dist_cutoff =
                    ENABLE_BOX_TO_BALL_FIRST_DIST_CUTOFF
                    && first_box_to_ball_dist != -1
                    && cur_dist >= first_box_to_ball_dist + BOX_TO_BALL_FIRST_DIST_EXTRA;
                // if(first_dist_cutoff) cerr << "First dist cutoff: " << cur_dist << " >= " << first_box_to_ball_dist << " + " << BOX_TO_BALL_FIRST_DIST_EXTRA << endl;
                
                // bool percent_cutoff =
                //     ENABLE_BOX_TO_BALL_PERCENT_CUTOFF
                //     && reached_box_to_ball_targets >= percent_target_count;
                // if (is_box_source && exact_targets_reached && (first_dist_cutoff || percent_cutoff)) break;
                if (is_box_source && exact_targets_reached && first_dist_cutoff) break;

                unsigned int next_level_tag = bfs_base_tag | (unsigned int)(cur_dist + 1);
                int current_level_tail = level_tail;
                while (head < current_level_tail && reached_sources < target_count) {
                    int s = q[head++];
                    unsigned char cur_mask = origin_mask[s];
                    if (target_seen[s] == bfs_target_token) {
                        unsigned short dst_row = src_idx[s];
                        fill_dist_rows_from_mask(base_row, dst_row, cur_dist, cur_mask);
                        reached_sources++;
                        if (box_to_ball_target_seen[s] == bfs_target_token) {
                            reached_box_to_ball_targets++;
                            if (first_box_to_ball_dist == -1) first_box_to_ball_dist = cur_dist;
                        }
                        if (exact_target_seen[s] == bfs_target_token) reached_exact_targets++;
                    }
                    unsigned long long packed_to = to[s];
                    int ns = packed_to & 0xffffu;
                    unsigned int tag = visit_tag[ns];
                    if ((tag & 0xffff0000u) != bfs_base_tag) {
                        visit_tag[ns] = next_level_tag;
                        origin_mask[ns] = cur_mask;
                        q[tail++] = ns;
                    } else if (tag == next_level_tag) {
                        origin_mask[ns] |= cur_mask;
                    }
                    ns = (packed_to >> 16) & 0xffffu;
                    tag = visit_tag[ns];
                    if ((tag & 0xffff0000u) != bfs_base_tag) {
                        visit_tag[ns] = next_level_tag;
                        origin_mask[ns] = cur_mask;
                        q[tail++] = ns;
                    } else if (tag == next_level_tag) {
                        origin_mask[ns] |= cur_mask;
                    }
                    ns = (packed_to >> 32) & 0xffffu;
                    tag = visit_tag[ns];
                    if ((tag & 0xffff0000u) != bfs_base_tag) {
                        visit_tag[ns] = next_level_tag;
                        origin_mask[ns] = cur_mask;
                        q[tail++] = ns;
                    } else if (tag == next_level_tag) {
                        origin_mask[ns] |= cur_mask;
                    }
                    ns = packed_to >> 48;
                    tag = visit_tag[ns];
                    if ((tag & 0xffff0000u) != bfs_base_tag) {
                        visit_tag[ns] = next_level_tag;
                        origin_mask[ns] = cur_mask;
                        q[tail++] = ns;
                    } else if (tag == next_level_tag) {
                        origin_mask[ns] |= cur_mask;
                    }
                }
                level_tail = tail;
                cur_dist++;
            }
            assert(reached_exact_targets == exact_target_count);
        }
        build_all_pairs_shortest_path_call_count++;
        build_all_pairs_shortest_path_total_us += Timer::now_us() - profile_start_us;
    }

    vector<unsigned short> build_exact_all_pairs_shortest_path() const {
        vector<unsigned short> exact(source_count * source_count, INF_DIST);
        vector<unsigned short> seen(states, 0);
        vector<unsigned short> q(states);
        unsigned short token = 0;
        const unsigned short *src_idx = source_index.data();
        const unsigned long long *to = graph_to_packed.data();

        rep(row, 0, source_count) {
            int src = source_states[row];
            unsigned short *dist_row = exact.data() + row * source_count;
            int head = 0, tail = 0;
            int level_tail = 1;
            int reached_sources = 1;
            token++;
            if (token == INF_DIST) {
                fill(all(seen), 0);
                token = 1;
            }
            seen[src] = token;
            dist_row[row] = 0;
            q[tail++] = src;
            unsigned short next_dist = 1;
            while (head < tail && reached_sources < source_count) {
                int s = q[head++];
                unsigned long long packed_to = to[s];
                int ns = packed_to & 0xffffu;
                if (seen[ns] != token) {
                    seen[ns] = token;
                    unsigned short dst_row = src_idx[ns];
                    if (dst_row != INF_DIST) {
                        dist_row[dst_row] = next_dist;
                        reached_sources++;
                    }
                    q[tail++] = ns;
                }
                ns = (packed_to >> 16) & 0xffffu;
                if (seen[ns] != token) {
                    seen[ns] = token;
                    unsigned short dst_row = src_idx[ns];
                    if (dst_row != INF_DIST) {
                        dist_row[dst_row] = next_dist;
                        reached_sources++;
                    }
                    q[tail++] = ns;
                }
                ns = (packed_to >> 32) & 0xffffu;
                if (seen[ns] != token) {
                    seen[ns] = token;
                    unsigned short dst_row = src_idx[ns];
                    if (dst_row != INF_DIST) {
                        dist_row[dst_row] = next_dist;
                        reached_sources++;
                    }
                    q[tail++] = ns;
                }
                ns = packed_to >> 48;
                if (seen[ns] != token) {
                    seen[ns] = token;
                    unsigned short dst_row = src_idx[ns];
                    if (dst_row != INF_DIST) {
                        dist_row[dst_row] = next_dist;
                        reached_sources++;
                    }
                    q[tail++] = ns;
                }
                if (head == level_tail) {
                    level_tail = tail;
                    next_dist++;
                }
            }
        }
        return exact;
    }

    void log_exact_distance_diff(const vector<unsigned short> &exact) const {
        int checked_entries = 0;
        int diff_entries = 0;
        int checked_rows = 0;
        int diff_rows = 0;
        rep(row, 0, source_count) {
            bool row_checked = false;
            bool row_diff = false;
            rep(dst_row, 0, source_count) {
                if (!should_store_distance_entry(row, dst_row)) continue;
                row_checked = true;
                checked_entries++;
                int idx = table_index(row, dst_row);
                if (dist[idx] != exact[idx]) {
                    diff_entries++;
                    row_diff = true;
                }
            }
            checked_rows += row_checked;
            diff_rows += row_diff;
        }
        cerr << "dist exact diff: rows=" << diff_rows << "/" << checked_rows
             << ", entries=" << diff_entries << "/" << checked_entries
             << ", macro=\"" << macro << "\"" << endl;
    }

    void use_exact_distance_table() {
        vector<unsigned short> exact = build_exact_all_pairs_shortest_path();
        // log_exact_distance_diff(exact);
        dist = std::move(exact);
        build_node_edge_cost_table();
        build_alias_samplers();
    }

    int state_of_ball(const Node &node) const { return encode(ball_cell[node.id], node.dir_ball); }
    int state_of_box(const Node &node) const { return encode(box_cell[node.id], node.dir_box); }

    int node_edge_index(int prev_id, int prev_dir_box, int cur_id, int cur_dir_ball, int cur_dir_box) const {
        int m = in.M + 1;
        return ((((prev_id * 4 + prev_dir_box) * m + cur_id) * 4 + cur_dir_ball) * 4 + cur_dir_box);
    }

    void build_node_edge_cost_table() {
        int m = in.M + 1;
        int total = m * 4 * m * 4 * 4;
        if ((int)node_edge_cost.size() != total) node_edge_cost.resize(total);

        vector<unsigned short> ball_row(m * 4), box_row(m * 4);
        rep(id, 0, m) rep(dir, 0, 4) {
            ball_row[id * 4 + dir] = source_index[encode(ball_cell[id], dir)];
            box_row[id * 4 + dir] = source_index[encode(box_cell[id], dir)];
            assert(ball_row[id * 4 + dir] != INF_DIST);
            assert(box_row[id * 4 + dir] != INF_DIST);
        }

        vector<int> carry_cost(m * 4 * 4, INF);
        const unsigned short *dist_data = dist.data();
        rep(cur_id, 0, m) rep(cur_dir_ball, 0, 4) {
            unsigned short b_row = ball_row[cur_id * 4 + cur_dir_ball];
            const unsigned short *dist_from_b = dist_data + b_row * source_count;
            int carry_base = (cur_id * 4 + cur_dir_ball) * 4;
            rep(cur_dir_box, 0, 4) {
                unsigned short d = dist_from_b[box_row[cur_id * 4 + cur_dir_box]];
                carry_cost[carry_base + cur_dir_box] = (d == INF_DIST ? INF : (int)d + 1);
            }
        }

        int *out = node_edge_cost.data();
        rep(prev_id, 0, m) rep(prev_dir_box, 0, 4) {
            unsigned short a_row = box_row[prev_id * 4 + prev_dir_box];
            const unsigned short *dist_from_a = dist_data + a_row * source_count;
            rep(cur_id, 0, m) rep(cur_dir_ball, 0, 4) {
                unsigned short d_ab = dist_from_a[ball_row[cur_id * 4 + cur_dir_ball]];
                int reach_ball_cost = (d_ab == INF_DIST ? INF : (int)d_ab + 1);
                int carry_base = (cur_id * 4 + cur_dir_ball) * 4;
                rep(cur_dir_box, 0, 4) {
                    int carry = carry_cost[carry_base + cur_dir_box];
                    *out++ = (reach_ball_cost >= INF || carry >= INF ? INF : reach_ball_cost + carry);
                }
            }
        }
        assert(out == node_edge_cost.data() + total);
    }

    int edge_cost_between_nodes(const Node &prev, const Node &cur) const {
        return node_edge_cost[node_edge_index(prev.id, prev.dir_box, cur.id, cur.dir_ball, cur.dir_box)];
    }

    int macro_registration_extra_cost() const {
        return score_from_raw(0);
    }

    vector<int> ball_to_box_button_distances() const {
        vector<int> res(in.M + 1, INF);
        rep(id, 1, in.M + 1) {
            int best = INF;
            rep(dir_ball, 0, 4) rep(dir_box, 0, 4) {
                int b = encode(ball_cell[id], dir_ball);
                int c = encode(box_cell[id], dir_box);
                chmin(best, distance_from(b, c));
            }
            res[id] = best;
        }
        return res;
    }

    int box_to_ball_cost_sum(int from_id, int to_id) const {
        int sum = 0;
        rep(from_dir, 0, 4) {
            unsigned short from_row = source_index[encode(box_cell[from_id], from_dir)];
            assert(from_row != INF_DIST);
            const unsigned short *dist_from = dist.data() + from_row * source_count;
            rep(to_dir, 0, 4) {
                unsigned short to_row = source_index[encode(ball_cell[to_id], to_dir)];
                assert(to_row != INF_DIST);
                unsigned short d = dist_from[to_row];
                if (d == INF_DIST) return INF;
                sum += (int)d + 1;
            }
        }
        return sum;
    }

    float alias_weight_from_cost_sum(int cost_sum) const {
        if (cost_sum >= INF) return 0.0f;
        return alias_weight_table[cost_sum];
    }

    void build_alias_samplers() {
        int m = in.M + 1;
        alias_after_to_id.assign(m, AliasSampler());
        alias_before_from_id.assign(m, AliasSampler());

        vector<float> weights;
        rep(to_id, 0, m) {
            weights.assign(m, 0.0f);
            rep(from_id, 0, m) {
                if (to_id == 0) weights[from_id] = 0.0f;
                else weights[from_id] = alias_weight_from_cost_sum(box_to_ball_cost_sum(from_id, to_id));
            }
            alias_after_to_id[to_id].build(weights);
        }

        rep(from_id, 0, m) {
            weights.assign(in.M, 0.0f);
            rep(to_id, 1, m) {
                weights[to_id - 1] = alias_weight_from_cost_sum(box_to_ball_cost_sum(from_id, to_id));
            }
            alias_before_from_id[from_id].build(weights);
        }
    }

    int score_from_raw(int raw_score) const {
        if (raw_score >= INF) return INF;
        return raw_score + score_base;
    }

    int initial_delivery_cost(int dir_box) const {
        if (!has_picked_before_pre_macro()) return 0;
        int move_cost = distance_from(start_state, encode(box_cell[0], dir_box));
        return move_cost >= INF ? INF : move_cost + 1;
    }

    void rebuild_positions(State &state) const {
        state.pos.assign(in.M + 1, -1);
        rep(i, 0, (int)state.order.size()) state.pos[state.order[i].id] = i;
    }

    void evaluate_state(State &state) const {
        if ((int)state.pos.size() != in.M + 1) rebuild_positions(state);
        state.raw_score = initial_delivery_cost(state.order[0].dir_box);
        if (state.raw_score >= INF) {
            state.score = INF;
            return;
        }
        rep(i, 1, (int)state.order.size()) {
            const Node &prev = state.order[i - 1];
            const Node &cur = state.order[i];
            int cost = edge_cost_between_nodes(prev, cur);
            if (cost >= INF) {
                state.raw_score = INF;
                state.score = INF;
                return;
            }
            state.raw_score += cost;
        }
        state.score = score_from_raw(state.raw_score);
    }

    bool optimize_all_dirs_dp(State &state) const {
        int node_count = (int)state.order.size() - 1;
        if (node_count <= 0) {
            if (!has_picked_before_pre_macro()) return false;
            int old_dir = state.order[0].dir_box;
            int best_raw_score = INF;
            rep(dir_box, 0, 4) if (chmin(best_raw_score, initial_delivery_cost(dir_box))) {
                state.order[0].dir_ball = state.order[0].dir_box = dir_box;
            }
            state.raw_score = best_raw_score;
            state.score = score_from_raw(best_raw_score);
            return state.order[0].dir_box != old_dir;
        }
        int old_score = state.score;
        int stage_count = node_count * 2;
        vector<array<int, 4>> dp(stage_count);
        vector<array<unsigned char, 4>> parent_dir(stage_count);
        for (auto &row : dp) row.fill(INF);

        int start_dir = state.order[0].dir_box;
        rep(i, 1, node_count + 1) {
            const Node &prev = state.order[i - 1];
            const Node &cur = state.order[i];
            int ball_stage = (i - 1) * 2;
            int box_stage = ball_stage + 1;

            rep(dir_ball, 0, 4) {
                int best = INF;
                unsigned char best_prev_dir = 0;
                rep(prev_dir_box, 0, 4) {
                    int prev_cost = i == 1
                        ? (has_picked_before_pre_macro()
                            ? initial_delivery_cost(prev_dir_box)
                            : (prev_dir_box == start_dir ? 0 : INF))
                        : dp[ball_stage - 1][prev_dir_box];
                    if (prev_cost >= INF) continue;
                    int move_cost = distance_from(
                        encode(box_cell[prev.id], prev_dir_box),
                        encode(ball_cell[cur.id], dir_ball));
                    if (move_cost >= INF) continue;
                    int cost = prev_cost + move_cost + 1;
                    if (chmin(best, cost)) best_prev_dir = prev_dir_box;
                }
                dp[ball_stage][dir_ball] = best;
                parent_dir[ball_stage][dir_ball] = best_prev_dir;
            }

            rep(dir_box, 0, 4) {
                int best = INF;
                unsigned char best_dir_ball = 0;
                rep(dir_ball, 0, 4) {
                    int prev_cost = dp[ball_stage][dir_ball];
                    if (prev_cost >= INF) continue;
                    int move_cost = distance_from(
                        encode(ball_cell[cur.id], dir_ball),
                        encode(box_cell[cur.id], dir_box));
                    if (move_cost >= INF) continue;
                    int cost = prev_cost + move_cost + 1;
                    if (chmin(best, cost)) best_dir_ball = dir_ball;
                }
                dp[box_stage][dir_box] = best;
                parent_dir[box_stage][dir_box] = best_dir_ball;
            }
        }

        int best_raw_score = INF;
        int dir = 0;
        rep(dir_box, 0, 4) {
            if (chmin(best_raw_score, dp[stage_count - 1][dir_box])) dir = dir_box;
        }
        if (best_raw_score >= INF) return false;

        bool changed = false;
        for (int i = node_count; i >= 1; i--) {
            int ball_stage = (i - 1) * 2;
            int box_stage = ball_stage + 1;
            changed |= state.order[i].dir_box != dir;
            state.order[i].dir_box = dir;
            dir = parent_dir[box_stage][dir];
            changed |= state.order[i].dir_ball != dir;
            state.order[i].dir_ball = dir;
            dir = parent_dir[ball_stage][dir];
        }
        if (has_picked_before_pre_macro()) {
            changed |= state.order[0].dir_box != dir;
            state.order[0].dir_ball = state.order[0].dir_box = dir;
        }
        state.raw_score = best_raw_score;
        state.score = score_from_raw(best_raw_score);
        assert(state.score <= old_score);
        return changed;
    }

    int eval(const State &state) const {
        State copy = state;
        evaluate_state(copy);
        return copy.score;
    }

    Node best_node_from(int from_state, int id) const {
        Node best{id, 0, 0};
        int best_cost = INF;
        rep(db, 0, 4) rep(dx, 0, 4) {
            Node cur{id, db, dx};
            int b = state_of_ball(cur);
            int c = state_of_box(cur);
            int cost = distance_from(from_state, b) + distance_from(b, c);
            if (chmin(best_cost, cost)) best = cur;
        }
        return best;
    }

    State initial_state() const {
        vector<int> unused;
        rep(i, 1, in.M + 1) if (i != picked_before_pre_macro_id) unused.push_back(i);
        State state;
        int start_dir = start_state % 4;
        if (has_picked_before_pre_macro()) {
            int best_cost = INF;
            rep(dir_box, 0, 4) if (chmin(best_cost, initial_delivery_cost(dir_box))) start_dir = dir_box;
        }
        state.order.push_back({0, start_dir, start_dir});
        int cur_state = encode(box_cell[0], start_dir);
        while (!unused.empty()) {
            int best_pos = 0;
            Node best_node = best_node_from(cur_state, unused[0]);
            int best_cost = INF;
            rep(pos, 0, (int)unused.size()) {
                Node node = best_node_from(cur_state, unused[pos]);
                int cost = distance_from(cur_state, state_of_ball(node))
                         + distance_from(state_of_ball(node), state_of_box(node));
                if (chmin(best_cost, cost)) {
                    best_pos = pos;
                    best_node = node;
                }
            }
            state.order.push_back(best_node);
            cur_state = state_of_box(best_node);
            unused.erase(unused.begin() + best_pos);
        }
        rebuild_positions(state);
        evaluate_state(state);
        return state;
    }

    struct SA {
        struct StaticData {
            const MacroSolver *solver = nullptr;
            bool strong_neighborhood = false;
        };
        StaticData data;

        SA(const MacroSolver *solver) {
            data.solver = solver;
        }

        struct Transition {
            int types[2000];
            int type_size = 0;

            long long try_cnt[100] = {};
            long long accept_cnt[100] = {};
            const int TransitionTypeSize = 7;
            static constexpr int NORMAL_RELOCATE_CANDIDATES = 1;
            static constexpr int STRONG_RELOCATE_CANDIDATES = 3;
            static constexpr int CHANGE_DIR_OPTIMIZE_RATE = 10;

            Transition() {
                rep(i, 0, 20) types[type_size++] = 0;  // dir
                rep(i, 0, 5) types[type_size++] = 1;  // swap
                rep(i, 0, 20) types[type_size++] = 2;  // move
                rep(i, 0, 50) types[type_size++] = 3;  // or-opt
                // rep(i, 0, 1) types[type_size++] = 4;  // greedy move
                // rep(i, 0, 1) types[type_size++] = 5;  // optimize dirs
                rep(i, 0, INNER_CROSS_EXCHANGE_WEIGHT) types[type_size++] = 6;  // cross-exchange
            }

            bool execute(State &state, StaticData &data, float current_temp) {
                int type = types[rnd(type_size)];
                bool res = false;
                switch (type) {
                    case 0: res = change_dirs(state, data, current_temp); break;
                    case 1: res = swap_order(state, data, current_temp); break;
                    case 2: res = move_order(state, data, current_temp); break;
                    case 3: res = oropt_order(state, data, current_temp); break;
                    case 4: res = greedy_move_order(state, data, current_temp); break;
                    case 5: res = optimize_dirs(state, data, current_temp); break;
                    case 6: res = cross_exchange_order(state, data, current_temp); break;
                    default: assert(false);
                }
                try_cnt[type]++;
                if (res) accept_cnt[type]++;
                return res;
            }

            bool should_accept_transition(float score_diff, float current_temp) {
                return score_diff <= 0 || log_rand_table() * current_temp < -score_diff;
            }

            struct EdgePositions {
                int v[32];
                int sz = 0;

                void add(int i, int n) {
                    if (i <= 0 || i >= n) return;
                    rep(k, 0, sz) if (v[k] == i) return;
                    v[sz++] = i;
                }

                void add_around_node(int i, int n) {
                    add(i, n);
                    add(i + 1, n);
                }
            };

            EdgePositions edge_positions_around_nodes(initializer_list<int> node_positions, int n) {
                EdgePositions edge_positions;
                for (int i : node_positions) edge_positions.add_around_node(i, n);
                return edge_positions;
            }

            EdgePositions edge_positions_around_nodes(const vector<int> &node_positions, int n) {
                EdgePositions edge_positions;
                for (int i : node_positions) edge_positions.add_around_node(i, n);
                return edge_positions;
            }

            void evaluate_delta(State &next, const State &state, StaticData &data,
                                const EdgePositions &old_edges, const EdgePositions &new_edges) {
                if (state.raw_score >= INF) {
                    data.solver->evaluate_state(next);
                    return;
                }

                int raw_score = state.raw_score;
                rep(k, 0, old_edges.sz) {
                    int i = old_edges.v[k];
                    const Node &prev = state.order[i - 1];
                    const Node &cur = state.order[i];
                    int cost = data.solver->edge_cost_between_nodes(prev, cur);
                    if (cost >= INF) {
                        data.solver->evaluate_state(next);
                        return;
                    }
                    raw_score -= cost;
                }
                rep(k, 0, new_edges.sz) {
                    int i = new_edges.v[k];
                    const Node &prev = next.order[i - 1];
                    const Node &cur = next.order[i];
                    int cost = data.solver->edge_cost_between_nodes(prev, cur);
                    if (cost >= INF) {
                        next.raw_score = INF;
                        next.score = INF;
                        return;
                    }
                    raw_score += cost;
                }
                next.raw_score = raw_score;
                next.score = data.solver->score_from_raw(raw_score);
            }

            int raw_score_without_old_edges(const State &state, StaticData &data,
                                            const EdgePositions &old_edges) {
                if (state.raw_score >= INF) return INF;
                int raw_score = state.raw_score;
                rep(k, 0, old_edges.sz) {
                    int i = old_edges.v[k];
                    const Node &prev = state.order[i - 1];
                    const Node &cur = state.order[i];
                    int cost = data.solver->edge_cost_between_nodes(prev, cur);
                    if (cost >= INF) return INF;
                    raw_score -= cost;
                }
                return raw_score;
            }

            void evaluate_delta_after_mutation(State &state, StaticData &data,
                                               int raw_score_without_old,
                                               const EdgePositions &new_edges) {
                if (raw_score_without_old >= INF) {
                    data.solver->evaluate_state(state);
                    return;
                }
                int raw_score = raw_score_without_old;
                rep(k, 0, new_edges.sz) {
                    int i = new_edges.v[k];
                    const Node &prev = state.order[i - 1];
                    const Node &cur = state.order[i];
                    int cost = data.solver->edge_cost_between_nodes(prev, cur);
                    if (cost >= INF) {
                        state.raw_score = INF;
                        state.score = INF;
                        return;
                    }
                    raw_score += cost;
                }
                state.raw_score = raw_score;
                state.score = data.solver->score_from_raw(raw_score);
            }

            const Node &node_after_segment_move(const State &state, int l, int len, int j, int pos) {
                if (j < l) {
                    if (pos < j) return state.order[pos];
                    if (pos < j + len) return state.order[l + pos - j];
                    if (pos < l + len) return state.order[pos - len];
                    return state.order[pos];
                } else {
                    if (pos < l) return state.order[pos];
                    if (pos < j) return state.order[pos + len];
                    if (pos < j + len) return state.order[l + pos - j];
                    return state.order[pos];
                }
            }

            int raw_score_after_segment_move(const State &state, StaticData &data,
                                             int l, int len, int j,
                                             const EdgePositions &old_edges,
                                             const EdgePositions &new_edges) {
                int raw_score = raw_score_without_old_edges(state, data, old_edges);
                if (raw_score >= INF) return INF;
                rep(k, 0, new_edges.sz) {
                    int i = new_edges.v[k];
                    const Node &prev = node_after_segment_move(state, l, len, j, i - 1);
                    const Node &cur = node_after_segment_move(state, l, len, j, i);
                    int cost = data.solver->edge_cost_between_nodes(prev, cur);
                    if (cost >= INF) return INF;
                    raw_score += cost;
                }
                return raw_score;
            }

            void apply_segment_move(State &state, int l, int len, int j) {
                array<Node, 4> seg;
                rep(k, 0, len) seg[k] = state.order[l + k];
                if (j < l) {
                    for (int p = l - 1; p >= j; p--) state.order[p + len] = state.order[p];
                } else {
                    rep(p, l, j) state.order[p] = state.order[p + len];
                }
                rep(k, 0, len) state.order[j + k] = seg[k];
                int from = min(l, j);
                int to = max(l + len, j + len);
                rep(p, from, to) state.pos[state.order[p].id] = p;
            }

            void apply_cross_exchange(State &state, int l, int len1, int middle_len, int len2) {
                int r1 = l + len1;
                int r2 = r1 + middle_len + len2;
                rotate(state.order.begin() + l, state.order.begin() + r1, state.order.begin() + r2);
                rotate(state.order.begin() + l,
                       state.order.begin() + l + middle_len,
                       state.order.begin() + l + middle_len + len2);
                rep(i, l, r2) state.pos[state.order[i].id] = i;
            }

            int sample_insert_position_by_alias(const State &state, StaticData &data, int l, int len) {
                int n = state.order.size();
                int reduced_n = n - len;
                bool sample_after = rnd(2) == 0;
                rep(_, 0, 16) {
                    if (sample_after) {
                        int sampled_id = data.solver->alias_after_to_id[state.order[l].id].sample(rnd);
                        int pos = state.pos[sampled_id];
                        if (pos < 0 || (l <= pos && pos < l + len)) continue;
                        int reduced_pos = pos < l ? pos : pos - len;
                        int j = reduced_pos + 1;
                        if (j == l || j < 1 || j > reduced_n) continue;
                        return j;
                    } else {
                        int sampled_id = data.solver->alias_before_from_id[state.order[l + len - 1].id].sample(rnd) + 1;
                        int pos = state.pos[sampled_id];
                        if (pos < 0 || (l <= pos && pos < l + len)) continue;
                        int j = pos < l ? pos : pos - len;
                        if (j == l || j < 1 || j >= reduced_n) continue;
                        return j;
                    }
                }
                return -1;
            }

            int sample_swap_position_by_alias(const State &state, StaticData &data, int i) {
                int n = state.order.size();
                bool sample_after = rnd(2) == 0;
                int current_id = state.order[i].id;
                rep(_, 0, 16) {
                    if (sample_after) {
                        int sampled_id = data.solver->alias_after_to_id[current_id].sample(rnd);
                        if (sampled_id == current_id) continue;
                        int pos = state.pos[sampled_id];
                        int j = pos + 1;
                        if (pos < 0 || j <= 0 || j >= n || j == i) continue;
                        return j;
                    } else {
                        int sampled_id = data.solver->alias_before_from_id[current_id].sample(rnd) + 1;
                        if (sampled_id == current_id) continue;
                        int j = state.pos[sampled_id];
                        if (j <= 0 || j >= n || j == i) continue;
                        return j;
                    }
                }
                return -1;
            }

            bool try_apply(State &state, State &next, StaticData &data, float current_temp,
                           const EdgePositions &old_edges, const EdgePositions &new_edges) {
                evaluate_delta(next, state, data, old_edges, new_edges);
                int score_diff = next.score - state.score;
                if (!should_accept_transition(score_diff, current_temp)) return false;
                state = std::move(next);
                return true;
            }

            int edge_cost(const State &state, StaticData &data, int i) {
                const Node &prev = state.order[i - 1];
                const Node &cur = state.order[i];
                return data.solver->edge_cost_between_nodes(prev, cur);
            }

            int local_cost_around(const State &state, StaticData &data, int i) {
                int n = state.order.size();
                int cost = edge_cost(state, data, i);
                if (cost >= INF) return INF;
                if (i + 1 < n) {
                    int next_cost = edge_cost(state, data, i + 1);
                    if (next_cost >= INF) return INF;
                    cost += next_cost;
                }
                return cost;
            }

            void optimize_dir_ball(State &state, StaticData &data, int i, int &best_cost) {
                int best_dir_ball = state.order[i].dir_ball;
                const unsigned char *dirs = DIR_PERMS[rnd(24)];
                rep(k, 0, 4) {
                    int dir_ball = dirs[k];
                    state.order[i].dir_ball = dir_ball;
                    int cost = local_cost_around(state, data, i);
                    if (cost <= best_cost) {
                        best_cost = cost;
                        best_dir_ball = dir_ball;
                    }
                }
                state.order[i].dir_ball = best_dir_ball;
            }

            void optimize_dir_box(State &state, StaticData &data, int i, int &best_cost) {
                int best_dir_box = state.order[i].dir_box;
                const unsigned char *dirs = DIR_PERMS[rnd(24)];
                rep(k, 0, 4) {
                    int dir_box = dirs[k];
                    state.order[i].dir_box = dir_box;
                    int cost = local_cost_around(state, data, i);
                    if (cost <= best_cost) {
                        best_cost = cost;
                        best_dir_box = dir_box;
                    }
                }
                state.order[i].dir_box = best_dir_box;
            }

            bool optimize_node_dirs(State &state, StaticData &data, int i) {
                int n = state.order.size();
                if (i <= 0 || i >= n) return false;
                Node original = state.order[i];
                int best_cost = local_cost_around(state, data, i);
                if (rnd(2) == 0) {
                    optimize_dir_ball(state, data, i, best_cost);
                    optimize_dir_box(state, data, i, best_cost);
                } else {
                    optimize_dir_box(state, data, i, best_cost);
                    optimize_dir_ball(state, data, i, best_cost);
                }
                return state.order[i].dir_ball != original.dir_ball || state.order[i].dir_box != original.dir_box;
            }

            bool change_dirs(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                int i = rnd(1, n);
                EdgePositions edges = edge_positions_around_nodes({i}, n);
                int old_raw_score = state.raw_score;
                int old_score = state.score;
                int raw_score_without_old = raw_score_without_old_edges(state, data, edges);
                Node original = state.order[i];
                if (rnd(100) < CHANGE_DIR_OPTIMIZE_RATE) {
                    if (!optimize_node_dirs(state, data, i)) {
                        state.order[i] = original;
                        return false;
                    }
                } else {
                    int type = rnd(3);
                    if (type == 0 || type == 2) state.order[i].dir_ball = rnd(4);
                    if (type == 1 || type == 2) state.order[i].dir_box = rnd(4);
                }
                evaluate_delta_after_mutation(state, data, raw_score_without_old, edges);
                int score_diff = state.score - old_score;
                if (!should_accept_transition(score_diff, current_temp)) {
                    state.order[i] = original;
                    state.raw_score = old_raw_score;
                    state.score = old_score;
                    return false;
                }
                return true;
            }

            bool swap_order(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                int i = rnd(1, n);
                int j = sample_swap_position_by_alias(state, data, i);
                if (j == -1) return false;
                if (i == j) return false;
                EdgePositions edges = edge_positions_around_nodes({i, j}, n);
                int old_raw_score = state.raw_score;
                int old_score = state.score;
                int raw_score_without_old = raw_score_without_old_edges(state, data, edges);
                swap(state.order[i], state.order[j]);
                evaluate_delta_after_mutation(state, data, raw_score_without_old, edges);
                int score_diff = state.score - old_score;
                if (!should_accept_transition(score_diff, current_temp)) {
                    swap(state.order[i], state.order[j]);
                    state.raw_score = old_raw_score;
                    state.score = old_score;
                    return false;
                }
                state.pos[state.order[i].id] = i;
                state.pos[state.order[j].id] = j;
                return true;
            }

            bool move_order(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                int i = rnd(1, n);
                int best_score = INF;
                int best_raw_score = INF;
                int best_j = -1;
                bool found = false;
                int candidates = data.strong_neighborhood ? STRONG_RELOCATE_CANDIDATES : NORMAL_RELOCATE_CANDIDATES;
                rep(_, 0, candidates) {
                    int j = sample_insert_position_by_alias(state, data, i, 1);
                    if (j == -1) continue;
                    if (i == j) continue;
                    EdgePositions old_edges;
                    EdgePositions new_edges;
                    if (j < i) {
                        old_edges.add(j, n);
                        old_edges.add(i, n);
                        old_edges.add(i + 1, n);
                        new_edges.add(j, n);
                        new_edges.add(j + 1, n);
                        new_edges.add(i + 1, n);
                    } else {
                        old_edges.add(i, n);
                        old_edges.add(i + 1, n);
                        old_edges.add(j + 1, n);
                        new_edges.add(i, n);
                        new_edges.add(j, n);
                        new_edges.add(j + 1, n);
                    }
                    int raw_score = raw_score_after_segment_move(state, data, i, 1, j, old_edges, new_edges);
                    int score = raw_score >= INF ? INF : data.solver->score_from_raw(raw_score);
                    if (chmin(best_score, score)) {
                        best_raw_score = raw_score;
                        best_j = j;
                        found = true;
                    }
                }
                if (!found) return false;
                int score_diff = best_score - state.score;
                if (!should_accept_transition(score_diff, current_temp)) return false;
                apply_segment_move(state, i, 1, best_j);
                state.raw_score = best_raw_score;
                state.score = best_score;
                return true;
            }

            bool greedy_move_order(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                int i = rnd(1, n);
                int best_score = INF;
                int best_raw_score = INF;
                int best_j = -1;
                rep(j, 1, n) {
                    if (i == j) continue;
                    EdgePositions old_edges;
                    EdgePositions new_edges;
                    if (j < i) {
                        old_edges.add(j, n);
                        old_edges.add(i, n);
                        old_edges.add(i + 1, n);
                        new_edges.add(j, n);
                        new_edges.add(j + 1, n);
                        new_edges.add(i + 1, n);
                    } else {
                        old_edges.add(i, n);
                        old_edges.add(i + 1, n);
                        old_edges.add(j + 1, n);
                        new_edges.add(i, n);
                        new_edges.add(j, n);
                        new_edges.add(j + 1, n);
                    }
                    int raw_score = raw_score_after_segment_move(state, data, i, 1, j, old_edges, new_edges);
                    int score = raw_score >= INF ? INF : data.solver->score_from_raw(raw_score);
                    if (chmin(best_score, score)) {
                        best_raw_score = raw_score;
                        best_j = j;
                    }
                }
                if (best_j == -1) return false;
                int score_diff = best_score - state.score;
                if (!should_accept_transition(score_diff, current_temp)) return false;
                apply_segment_move(state, i, 1, best_j);
                state.raw_score = best_raw_score;
                state.score = best_score;
                return true;
            }

            bool optimize_single_dir(State &state, StaticData &data, int encoded_dir) {
                int n = state.order.size();
                int i = encoded_dir / 2 + 1;
                bool is_box_dir = encoded_dir % 2;
                if (i <= 0 || i >= n) return false;
                EdgePositions edges = edge_positions_around_nodes({i}, n);
                int raw_score_without_old = raw_score_without_old_edges(state, data, edges);
                Node original_node = state.order[i];
                int original_score = state.score;
                Node best_node = state.order[i];
                int best_raw_score = state.raw_score;
                int best_score = state.score;
                const unsigned char *dirs = DIR_PERMS[rnd(24)];
                rep(k, 0, 4) {
                    if (is_box_dir) state.order[i].dir_box = dirs[k];
                    else state.order[i].dir_ball = dirs[k];
                    evaluate_delta_after_mutation(state, data, raw_score_without_old, edges);
                    if (state.score <= best_score) {
                        best_node = state.order[i];
                        best_raw_score = state.raw_score;
                        best_score = state.score;
                    }
                }
                bool changed = best_node.dir_ball != original_node.dir_ball
                            || best_node.dir_box != original_node.dir_box
                            || best_score != original_score;
                state.order[i] = best_node;
                state.raw_score = best_raw_score;
                state.score = best_score;
                return changed;
            }

            bool optimize_dirs(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                int dir_count = (n - 1) * 2;
                if (dir_count <= 0) return false;
                vector<int> dirs(dir_count);
                rep(i, 0, dir_count) dirs[i] = i;
                for (int i = dir_count - 1; i > 0; i--) swap(dirs[i], dirs[rnd(i + 1)]);
                State original = state;
                bool changed = false;
                rep(k, 0, dir_count) changed |= optimize_single_dir(state, data, dirs[k]);
                if (!changed) return false;
                int score_diff = state.score - original.score;
                if (!should_accept_transition(score_diff, current_temp)) {
                    state = std::move(original);
                    return false;
                }
                return true;
            }

            bool oropt_order(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                if (n <= 3) return false;
                int len = rnd(2, min(4, n));
                int l = rnd(1, n - len + 1);
                int j = sample_insert_position_by_alias(state, data, l, len);
                if (j == -1) return false;
                if (j == l) return false;
                EdgePositions old_edges;
                EdgePositions new_edges;
                int r = l + len;
                if (j < l) {
                    old_edges.add(j, n);
                    old_edges.add(l, n);
                    old_edges.add(r, n);
                    new_edges.add(j, n);
                    new_edges.add(j + len, n);
                    new_edges.add(r, n);
                } else {
                    old_edges.add(l, n);
                    old_edges.add(r, n);
                    old_edges.add(j + len, n);
                    new_edges.add(l, n);
                    new_edges.add(j, n);
                    new_edges.add(j + len, n);
                }
                int raw_score = raw_score_after_segment_move(state, data, l, len, j, old_edges, new_edges);
                int score = raw_score >= INF ? INF : data.solver->score_from_raw(raw_score);
                int score_diff = score - state.score;
                if (!should_accept_transition(score_diff, current_temp)) return false;
                apply_segment_move(state, l, len, j);
                state.raw_score = raw_score;
                state.score = score;
                return true;
            }

            bool cross_exchange_order(State &state, StaticData &data, float current_temp) {
                int n = state.order.size();
                int node_count = n - 1;
                if (node_count < 2) return false;

                int len1 = rnd(1, min(INNER_CROSS_EXCHANGE_MAX_LEN, node_count - 1) + 1);
                int l1 = rnd(1, n - len1 + 1);
                int remaining = node_count - len1;
                int len2 = rnd(1, min(INNER_CROSS_EXCHANGE_MAX_LEN, remaining) + 1);
                int left_count = max(0, l1 - 1 - len2 + 1);
                int right_count = max(0, n - (l1 + len1) - len2 + 1);
                if (left_count + right_count == 0) return false;
                int side_offset = rnd(left_count + right_count);
                int l2 = side_offset < left_count
                    ? 1 + side_offset
                    : l1 + len1 + side_offset - left_count;
                if (l2 < l1) {
                    swap(l1, l2);
                    swap(len1, len2);
                }

                int r1 = l1 + len1;
                int r2 = l2 + len2;
                int middle_len = l2 - r1;
                EdgePositions old_edges;
                old_edges.add(l1, n);
                old_edges.add(r1, n);
                old_edges.add(l2, n);
                old_edges.add(r2, n);
                EdgePositions new_edges;
                new_edges.add(l1, n);
                new_edges.add(l1 + len2, n);
                new_edges.add(l1 + len2 + middle_len, n);
                new_edges.add(r2, n);

                int old_raw_score = state.raw_score;
                int old_score = state.score;
                int raw_score_without_old = raw_score_without_old_edges(state, data, old_edges);
                apply_cross_exchange(state, l1, len1, middle_len, len2);
                evaluate_delta_after_mutation(state, data, raw_score_without_old, new_edges);
                int score_diff = state.score - old_score;
                if (!should_accept_transition(score_diff, current_temp)) {
                    apply_cross_exchange(state, l1, len2, middle_len, len1);
                    state.raw_score = old_raw_score;
                    state.score = old_score;
                    return false;
                }
                return true;
            }
        } transition;

        struct Temp {
            float start_temp = INNER_SA_START_TEMP;
            float end_temp = INNER_SA_END_TEMP;

            float get_temp(float progress) {
                return pow(start_temp, 1.0f - progress) * pow(end_temp, progress);
                // return start_temp + (end_temp - start_temp) * progress;
            }
        } temp;

        State run_us(int time_limit_us, State state, bool verbose = true, float start_temp_multi = 1.0f) {
            timer.mark_lap();
            data.strong_neighborhood = time_limit_us >= FINAL_SOLVER_TIME_MS * 1000;
            temp.start_temp = INNER_SA_START_TEMP * start_temp_multi;

            State best_state = state;
            float current_temp = temp.start_temp;
            int loop_cnt = 0;
            int accept_cnt = 0;
            time_limit_us = max(1, time_limit_us);
            long long elapsed_us = timer.lap_us();
            while (elapsed_us < time_limit_us) {
                float progress = (float)elapsed_us / time_limit_us;
                current_temp = temp.get_temp(min(1.0f, progress));
                rep(_, 0, 256) {
                    loop_cnt++;
                    bool accept = transition.execute(state, data, current_temp);
                    accept_cnt += accept;

                    if (accept && state.score < best_state.score) {
                        // cerr << "best update: macro=\"" << data.solver->macro << "\", loop_cnt=" << loop_cnt
                        //      << ", temp=" << current_temp << ", score=" << state.score << endl;
                        best_state = state;
                    }
                }
                elapsed_us = timer.lap_us();
            }
            data.solver->optimize_all_dirs_dp(best_state);
            if (verbose) {
                cerr << "anneal done: macro=\"" << data.solver->macro << "\", loop_cnt=" << loop_cnt
                     << ", accept_rate=" << (loop_cnt == 0 ? 0.0 : (double)accept_cnt / loop_cnt)
                     << ", best=" << best_state.score << endl;
            }
            return best_state;
        }

        State run(int time_limit_ms, State state, bool verbose = true, float start_temp_multi = 1.0f) {
            return run_us(time_limit_ms * 1000, std::move(state), verbose, start_temp_multi);
        }
    };

    State anneal(int budget_ms, bool verbose = true, float start_temp_multi = 1.0f) const {
        SA sa(this);
        return sa.run(budget_ms, initial_state(), verbose, start_temp_multi);
    }

    State anneal_us(int budget_us, bool verbose = true, float start_temp_multi = 1.0f) const {
        SA sa(this);
        return sa.run_us(budget_us, initial_state(), verbose, start_temp_multi);
    }

    string path_buttons(int src, int dst, int &expanded_basic, bool &macro_registered) const {
        string res;
        if ((int)restore_parent.size() != states) {
            restore_parent.resize(states);
            restore_action.resize(states);
            restore_q.resize(states);
        }
        fill(all(restore_parent), INVALID_STATE);
        fill(all(restore_action), 255);

        int head = 0, tail = 0;
        restore_parent[src] = src;
        restore_q[tail++] = src;
        while (head < tail && restore_parent[dst] == INVALID_STATE) {
            int s = restore_q[head++];
            rep(a, 0, 4) {
                int ns = graph_to[s][a];
                if (restore_parent[ns] != INVALID_STATE) continue;
                restore_parent[ns] = s;
                restore_action[ns] = a;
                restore_q[tail++] = ns;
                if (ns == dst) break;
            }
        }

        vector<int> actions;
        int cur = dst;
        while (cur != src && restore_parent[cur] != INVALID_STATE) {
            int a = restore_action[cur];
            if (a < 0 || a >= 4) break;
            actions.push_back(a);
            cur = restore_parent[cur];
        }
        reverse(all(actions));

        cur = src;
        for (int a : actions) {
            if (a == 3 && !macro_registered) {
                res.push_back('M');
                res += registration_macro;
                res.push_back('M');
                macro_registered = true;
            } else {
                res.push_back(ACTION_CHARS[a]);
            }
            expanded_basic += (a == 3 ? (int)macro.size() : 1);
            cur = graph_to[cur][a];
        }
        return res;
    }

    pair<string, int> restore(const State &state) const {
        string out;
        int expanded_basic = pre_macro.size() + (has_pre_pre_macro_move() ? 1 : 0)
                           + (has_picked_before_pre_macro() ? 1 : 0);
        if (has_picked_before_pre_macro()) {
            out.push_back('S');
        }
        if (has_pre_pre_macro_move()) {
            out.push_back(pre_pre_macro_move);
        }
        if (!pre_macro.empty()) {
            out.push_back('M');
            out += pre_macro;
            out.push_back('M');
        }
        bool macro_registered = macro.empty();
        if (has_picked_before_pre_macro()) {
            out += path_buttons(start_state, state_of_box(state.order[0]), expanded_basic, macro_registered);
            out.push_back('S');
            expanded_basic++;
        }
        rep(i, 1, (int)state.order.size()) {
            const Node &prev = state.order[i - 1];
            const Node &cur = state.order[i];
            out += path_buttons(state_of_box(prev), state_of_ball(cur), expanded_basic, macro_registered);
            out.push_back('S');
            expanded_basic++;
            out += path_buttons(state_of_ball(cur), state_of_box(cur), expanded_basic, macro_registered);
            out.push_back('S');
            expanded_basic++;
        }
        return {out, expanded_basic};
    }
};

struct MacroSearch {
    const Input &in;
    MacroSolver solver;
    const bool enable_zigzag_normalize;

    static constexpr int MACRO_NEIGHBOR_TYPE_SIZE = 12;

    struct NeighborStats {
        long long trial = 0;
        long long early_reject = 0;
        long long full_eval = 0;
        long long accept = 0;
    };

    struct MacroBlock {
        unsigned char f = 0;
        unsigned char turn = 0;  // 0: none, 1: R, 2: RR, 3: L
    };

    struct State {
        string macro;
        char pre_pre_macro_move = 0;
        bool use_zero_gain_pre_macro = false;
        vector<MacroBlock> blocks;
        string output;
        ll score = (1LL << 60);
        int buttons = INF;
        int expanded_basic = INF;
        int correct = 0;
        bool full_evaluated = false;
        int initial_seed_id = -1;
    };

    struct EvaluatedMacro {
        string macro;
        char pre_pre_macro_move = 0;
        bool use_zero_gain_pre_macro = false;
        ll score;
        int buttons;
        int expanded_basic;
        int correct;
    };

    struct RoughCacheEntry {
        ll score = (1LL << 60);
        State state;
        bool has_state = false;
    };

    struct InitialSeedCandidate {
        string macro;
        string solver_macro;
        char pre_pre_macro_move = 0;
        bool use_zero_gain_pre_macro = false;
        MacroSolver::State inner_state;
        int initial_seed_id = -1;
    };

    vector<EvaluatedMacro> history;
    unordered_map<string, State> cache;
    unordered_map<string, RoughCacheEntry> rough_cache;
    long long early_check_cnt = 0;
    long long early_reject_cnt = 0;
    long long cache_hit_cnt = 0;
    long long cache_refresh_cnt = 0;
    long long cache_refresh_update_cnt = 0;
    long long rough_cache_hit_cnt = 0;
    long long rough_cache_refresh_cnt = 0;
    long long rough_cache_update_cnt = 0;
    ll best_score_seen = (1LL << 60);
    array<NeighborStats, MACRO_NEIGHBOR_TYPE_SIZE> neighbor_stats;
    int active_neighbor_type = -1;
    vector<State> initial_seed_states;
    bool best_update_log_started = false;
    ll best_update_log_score = (1LL << 60);

    MacroSearch(const Input &in_)
        : in(in_), solver(in_, ""), enable_zigzag_normalize(!in_.has_internal_wall()) {}

    static const char *neighbor_name(int type) {
        static const char *names[MACRO_NEIGHBOR_TYPE_SIZE] = {
            "insert",
            "remove",
            "single_turn_flip",
            "change_tail_turn",
            "insert_block",
            "prepend_zero_f_block",
            "resize_block_f",
            "jitter_all_block_f",
            "jitter_random_blocks_f",
            "change_pre_pre_macro",
            "toggle_zero_gain_pre_macro",
            "flip_block_turn_range",
        };
        assert(0 <= type && type < MACRO_NEIGHBOR_TYPE_SIZE);
        return names[type];
    }

    char random_macro_char() const {
        return MACRO_CHARS[rnd(sizeof(MACRO_CHARS))];
    }

    string random_macro_piece(int len) const {
        string res;
        rep(i, 0, len) res.push_back(random_macro_char());
        return res;
    }

    char random_pre_pre_macro_move() const {
        int x = rnd(10);
        if (x < 6) return 0;
        if (x < 9) return 'R';
        return 'L';
    }

    static const char *pre_pre_macro_move_name(char move) {
        if (move == 'R') return "R";
        if (move == 'L') return "L";
        return "none";
    }

    static const char *bool_name(bool value) {
        return value ? "true" : "false";
    }

    string cache_key(const string &macro, char pre_pre_macro_move) const {
        string key = macro;
        key.push_back('\n');
        key.push_back(pre_pre_macro_move == 0 ? 'N' : pre_pre_macro_move);
        return key;
    }

    int count_non_overlapping_occurrences(const string &s, const string &pat) const {
        if (pat.empty() || s.size() < pat.size()) return 0;
        int cnt = 0;
        for (int i = 0; i + (int)pat.size() <= (int)s.size();) {
            if (s.compare(i, pat.size(), pat) == 0) {
                cnt++;
                i += pat.size();
            } else {
                i++;
            }
        }
        return cnt;
    }

    void update_pre_macro_gain_info(const string &macro, const string &pat, int &best_gain, bool &has_zero_gain) const {
        if (pat.size() < 2 || pat.size() > macro.size()) return;
        int occurrences = count_non_overlapping_occurrences(macro, pat);
        int gain = ((int)pat.size() - 1) * occurrences - ((int)pat.size() + 2);
        chmax(best_gain, gain);
        if (gain == 0) has_zero_gain = true;
    }

    bool zero_gain_pre_macro_flag_relevant(const string &macro) const {
        int max_f_run = 0;
        int cur_f_run = 0;
        for (char c : macro) {
            if (c == 'F') {
                chmax(max_f_run, ++cur_f_run);
            } else {
                cur_f_run = 0;
            }
        }

        int best_gain = -INF;
        bool has_zero_gain = false;
        rep(len, 2, max_f_run + 1) {
            update_pre_macro_gain_info(macro, string(len, 'F'), best_gain, has_zero_gain);
        }
        rep(len, 1, max_f_run + 1) {
            string fs(len, 'F');
            update_pre_macro_gain_info(macro, "R" + fs, best_gain, has_zero_gain);
            update_pre_macro_gain_info(macro, "L" + fs, best_gain, has_zero_gain);
            update_pre_macro_gain_info(macro, fs + "R", best_gain, has_zero_gain);
            update_pre_macro_gain_info(macro, fs + "L", best_gain, has_zero_gain);
        }
        return best_gain <= 0 && has_zero_gain;
    }

    bool canonical_use_zero_gain_pre_macro(const string &macro, bool use_zero_gain_pre_macro) const {
        return use_zero_gain_pre_macro && zero_gain_pre_macro_flag_relevant(macro);
    }

    string cache_key(const string &macro, char pre_pre_macro_move, bool use_zero_gain_pre_macro) const {
        string key = cache_key(macro, pre_pre_macro_move);
        key.push_back('\n');
        key.push_back(canonical_use_zero_gain_pre_macro(macro, use_zero_gain_pre_macro) ? 'Z' : 'N');
        return key;
    }

    string normalize_macro(string macro) const {
        string res;
        int turn = 0;
        auto flush_turn = [&]() {
            turn = (turn % 4 + 4) % 4;
            if (turn == 1) {
                res.push_back('R');
            } else if (turn == 2) {
                res.push_back('R');
                res.push_back('R');
            } else if (turn == 3) {
                res.push_back('L');
            }
            turn = 0;
        };
        for (char c : macro) {
            if (c != 'F' && c != 'R' && c != 'L') continue;
            if (c == 'R') {
                turn++;
            } else if (c == 'L') {
                turn--;
            } else {
                flush_turn();
                res.push_back(c);
            }
        }
        flush_turn();
        if ((int)res.size() > MAX_MACRO_LEN) res.resize(MAX_MACRO_LEN);
        return res;
    }

    string normalize_macro_for_downstream(string macro) const {
        if (!enable_zigzag_normalize) return macro;
        macro = normalize_macro(std::move(macro));

        while (true) {
            vector<int> replacement(macro.size(), -1);
            array<int, 4> first_run;
            array<bool, 4> used_dir{};
            first_run.fill(-1);
            int dir = DIR_R;

            for (int i = 0; i < (int)macro.size();) {
                if (macro[i] == 'R') {
                    dir = (dir + 1) & 3;
                    i++;
                    continue;
                }
                if (macro[i] == 'L') {
                    dir = (dir + 3) & 3;
                    i++;
                    continue;
                }

                int j = i;
                while (j < (int)macro.size() && macro[j] == 'F') j++;
                if (used_dir[(dir + 2) & 3]) {
                    first_run.fill(-1);
                    used_dir.fill(false);
                }
                if (first_run[dir] == -1) {
                    first_run[dir] = i;
                    replacement[i] = j - i;
                } else {
                    replacement[first_run[dir]] += j - i;
                    replacement[i] = 0;
                }
                used_dir[dir] = true;
                i = j;
            }

            string next;
            next.reserve(macro.size());
            for (int i = 0; i < (int)macro.size();) {
                if (macro[i] != 'F') {
                    next.push_back(macro[i++]);
                    continue;
                }
                int j = i;
                while (j < (int)macro.size() && macro[j] == 'F') j++;
                next.append(replacement[i], 'F');
                i = j;
            }
            next = normalize_macro(std::move(next));
            if (next == macro) return macro;
            macro = std::move(next);
        }
    }

    vector<MacroBlock> macro_to_blocks(string macro) const {
        vector<MacroBlock> blocks;
        for (int i = 0; i < (int)macro.size();) {
            int f = 0;
            while (i < (int)macro.size() && macro[i] == 'F') {
                f++;
                i++;
            }
            int turn = 0;
            while (i < (int)macro.size() && (macro[i] == 'R' || macro[i] == 'L')) {
                turn += macro[i] == 'R' ? 1 : -1;
                i++;
            }
            turn = (turn % 4 + 4) % 4;
            if (f > 0 || turn != 0) blocks.push_back(MacroBlock{(unsigned char)f, (unsigned char)turn});
        }
        return blocks;
    }

    int turn_len(int turn) const {
        return turn == 0 ? 0 : (turn == 2 ? 2 : 1);
    }

    int blocks_length(const vector<MacroBlock> &blocks) const {
        int len = 0;
        for (const MacroBlock &b : blocks) len += b.f + turn_len(b.turn);
        return len;
    }

    void append_turn(string &macro, int turn) const {
        if (turn == 1) {
            macro.push_back('R');
        } else if (turn == 2) {
            macro.push_back('R');
            macro.push_back('R');
        } else if (turn == 3) {
            macro.push_back('L');
        }
    }

    string blocks_to_macro(const vector<MacroBlock> &blocks) const {
        string macro;
        macro.reserve(MAX_MACRO_LEN + 4);
        for (const MacroBlock &b : blocks) {
            macro.append(b.f, 'F');
            append_turn(macro, b.turn);
        }
        return normalize_macro(std::move(macro));
    }

    bool random_positive_f_block(int max_len, MacroBlock &block) const {
        int turns[4];
        int turn_size = 0;
        rep(turn, 0, 4) {
            if (1 + turn_len(turn) <= max_len) turns[turn_size++] = turn;
        }
        if (turn_size == 0) return false;
        int turn = turns[rnd(turn_size)];
        int max_f = min(5, max_len - turn_len(turn));
        if (max_f <= 0) return false;
        int f = rnd(1, max_f + 1);
        block = MacroBlock{(unsigned char)f, (unsigned char)turn};
        return true;
    }

    State empty_macro_state() const {
        State state;
        state.score = (1LL << 60);
        return state;
    }

    void clamp_macro(string &macro) const {
        if ((int)macro.size() > MAX_MACRO_LEN) macro.resize(MAX_MACRO_LEN);
    }

    State make_state(const string &macro, char pre_pre_macro_move, bool use_zero_gain_pre_macro,
                     string out, const SimResult &sim, bool full_evaluated = true) const {
        State state;
        state.macro = macro;
        state.pre_pre_macro_move = pre_pre_macro_move;
        state.use_zero_gain_pre_macro = use_zero_gain_pre_macro;
        state.blocks = macro_to_blocks(macro);
        state.output = std::move(out);
        state.score = sim.score;
        state.buttons = state.output.size();
        state.expanded_basic = sim.expanded_basic;
        state.correct = sim.correct;
        state.full_evaluated = full_evaluated;
        return state;
    }

    State with_state_macro(State state, const string &macro, bool use_zero_gain_pre_macro) const {
        state.macro = macro;
        state.use_zero_gain_pre_macro = use_zero_gain_pre_macro;
        state.blocks = macro_to_blocks(macro);
        return state;
    }

    void observe_score(ll score) {
        chmin(best_score_seen, score);
    }

    State evaluate_full(const string &macro, const string &solver_macro,
                        char pre_pre_macro_move, bool use_zero_gain_pre_macro) {
        if (0 <= active_neighbor_type && active_neighbor_type < MACRO_NEIGHBOR_TYPE_SIZE) {
            neighbor_stats[active_neighbor_type].full_eval++;
        }
        auto inner_state = solver.anneal(INNER_SOLVER_TIME_MS, false);
        auto [out, expanded] = solver.restore(inner_state);
        auto sim = compute_raw_score(in, out);

        State state = make_state(macro, pre_pre_macro_move, use_zero_gain_pre_macro, std::move(out), sim);
        history.push_back({solver_macro, state.pre_pre_macro_move, state.use_zero_gain_pre_macro,
                           state.score, state.buttons, state.expanded_basic, state.correct});
        string key = cache_key(solver_macro, state.pre_pre_macro_move, state.use_zero_gain_pre_macro);
        auto [it, inserted] = cache.emplace(key, state);
        if (!inserted && state.score < it->second.score) {
            it->second = state;
            cache_refresh_update_cnt++;
        }
        observe_score(it->second.score);
        return state;
    }

    State evaluate(string macro, char pre_pre_macro_move = 0, bool use_zero_gain_pre_macro = false) {
        macro = normalize_macro(std::move(macro));
        if (macro.empty()) return empty_macro_state();
        string solver_macro = normalize_macro_for_downstream(macro);
        use_zero_gain_pre_macro = canonical_use_zero_gain_pre_macro(solver_macro, use_zero_gain_pre_macro);
        string key = cache_key(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        if (auto it = cache.find(key); it != cache.end()) {
            observe_score(it->second.score);
            return with_state_macro(it->second, macro, use_zero_gain_pre_macro);
        }

        solver.set_macro(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        return evaluate_full(macro, solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
    }

    InitialSeedCandidate evaluate_initial_seed(string macro, char pre_pre_macro_move = 0,
                                               bool use_zero_gain_pre_macro = false) {
        macro = normalize_macro(std::move(macro));
        assert(!macro.empty());
        string solver_macro = normalize_macro_for_downstream(macro);
        use_zero_gain_pre_macro = canonical_use_zero_gain_pre_macro(solver_macro, use_zero_gain_pre_macro);
        solver.set_macro(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        auto inner_state = solver.anneal_us(INITIAL_SEED_SOLVER_TIME_US, false);
        return {std::move(macro), std::move(solver_macro), pre_pre_macro_move,
                use_zero_gain_pre_macro, std::move(inner_state)};
    }

    State restore_initial_seed(const InitialSeedCandidate &candidate) {
        solver.set_macro(candidate.solver_macro, candidate.pre_pre_macro_move, candidate.use_zero_gain_pre_macro);
        auto [out, expanded] = solver.restore(candidate.inner_state);
        auto sim = compute_raw_score(in, out);
        State state = make_state(candidate.macro, candidate.pre_pre_macro_move,
                                 candidate.use_zero_gain_pre_macro, std::move(out), sim, false);
        state.expanded_basic = expanded;
        state.initial_seed_id = candidate.initial_seed_id;
        return state;
    }

    bool should_refresh_cache_hit(const State &cached) const {
        if (!ENABLE_CACHE_REFRESH) return false;
        if ((double)cached.score >= (double)best_score_seen * CACHE_REFRESH_SCORE_MARGIN) return false;
        return rnd.rand() < CACHE_REFRESH_PROB;
    }

    bool should_refresh_rough_cache_hit(const RoughCacheEntry &cached) const {
        if (!ENABLE_ROUGH_CACHE) return false;
        if (best_score_seen < (1LL << 59)
            && (double)cached.score > (double)best_score_seen * ROUGH_CACHE_REFRESH_SCORE_MARGIN) {
            return false;
        }
        return rnd.rand() < ROUGH_CACHE_REFRESH_PROB;
    }

    State run_rough_eval(const string &macro, char pre_pre_macro_move, bool use_zero_gain_pre_macro) {
        early_check_cnt++;
        auto rough_inner_state = solver.anneal_us(EARLY_REJECT_SOLVER_TIME_US, false);
        auto [out, expanded] = solver.restore(rough_inner_state);
        auto sim = compute_raw_score(in, out);
        return make_state(macro, pre_pre_macro_move, use_zero_gain_pre_macro, std::move(out), sim, false);
    }

    void store_rough_cache_score(const string &macro, char pre_pre_macro_move,
                                 bool use_zero_gain_pre_macro, ll score) {
        if (!ENABLE_ROUGH_CACHE) return;
        string solver_macro = normalize_macro_for_downstream(macro);
        auto [it, inserted] = rough_cache.emplace(
            cache_key(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro), RoughCacheEntry{});
        if (inserted || score < it->second.score) {
            it->second.score = score;
            if (!inserted) rough_cache_update_cnt++;
        }
    }

    void store_rough_cache(const State &rough) {
        if (!ENABLE_ROUGH_CACHE) return;
        string solver_macro = normalize_macro_for_downstream(rough.macro);
        auto [it, inserted] = rough_cache.emplace(
            cache_key(solver_macro, rough.pre_pre_macro_move, rough.use_zero_gain_pre_macro), RoughCacheEntry{});
        if (inserted || !it->second.has_state || rough.score < it->second.score) {
            it->second.score = rough.score;
            it->second.state = rough;
            it->second.has_state = true;
            if (!inserted) rough_cache_update_cnt++;
        }
    }

    RoughCacheEntry make_rough_cache_entry(const State &rough) const {
        RoughCacheEntry entry;
        entry.score = rough.score;
        entry.state = rough;
        entry.has_state = true;
        return entry;
    }

    bool refresh_rough_cache_entry(const string &macro, char pre_pre_macro_move,
                                   bool use_zero_gain_pre_macro, RoughCacheEntry &entry) {
        rough_cache_refresh_cnt++;
        State refreshed = run_rough_eval(macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        store_rough_cache(refreshed);
        if (!entry.has_state || refreshed.score < entry.score) {
            entry = make_rough_cache_entry(refreshed);
            return true;
        }
        return false;
    }

    bool should_early_reject(double accept_score_limit) {
        early_check_cnt++;
        auto rough_state = solver.anneal_us(EARLY_REJECT_SOLVER_TIME_US, false);
        if ((double)rough_state.score > accept_score_limit * EARLY_REJECT_SCORE_MARGIN) {
            early_reject_cnt++;
            if (0 <= active_neighbor_type && active_neighbor_type < MACRO_NEIGHBOR_TYPE_SIZE) {
                neighbor_stats[active_neighbor_type].early_reject++;
            }
            return true;
        }
        auto [out, expanded] = solver.restore(rough_state);
        auto sim = compute_raw_score(in, out);
        if ((double)sim.score > accept_score_limit * EARLY_REJECT_SCORE_MARGIN) {
            early_reject_cnt++;
            if (0 <= active_neighbor_type && active_neighbor_type < MACRO_NEIGHBOR_TYPE_SIZE) {
                neighbor_stats[active_neighbor_type].early_reject++;
            }
            return true;
        }
        return false;
    }

    void record_early_reject() {
        early_reject_cnt++;
        if (0 <= active_neighbor_type && active_neighbor_type < MACRO_NEIGHBOR_TYPE_SIZE) {
            neighbor_stats[active_neighbor_type].early_reject++;
        }
    }

    bool should_full_eval_from_rough(const State &rough) const {
        if (best_score_seen >= (1LL << 59)) return true;
        return (double)rough.score <= (double)best_score_seen * FULL_EVAL_SCORE_MARGIN;
    }

    bool evaluate_for_transition(string macro, char pre_pre_macro_move, bool use_zero_gain_pre_macro,
                                 double accept_score_limit, State &state) {
        macro = normalize_macro(std::move(macro));
        if (macro.empty()) return false;
        string solver_macro = normalize_macro_for_downstream(macro);
        use_zero_gain_pre_macro = canonical_use_zero_gain_pre_macro(solver_macro, use_zero_gain_pre_macro);
        string key = cache_key(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        if (auto it = cache.find(key); it != cache.end()) {
            cache_hit_cnt++;
            observe_score(it->second.score);
            if (should_refresh_cache_hit(it->second)) {
                cache_refresh_cnt++;
                solver.set_macro(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
                evaluate_full(macro, solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
                it = cache.find(key);
            }
            state = with_state_macro(it->second, macro, use_zero_gain_pre_macro);
            return true;
        }

        solver.set_macro(solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        if (!ENABLE_EARLY_REJECT) {
            state = evaluate_full(macro, solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
            return true;
        }

        double reject_limit = accept_score_limit * EARLY_REJECT_SCORE_MARGIN;
        if (ENABLE_ROUGH_CACHE) {
            if (auto it = rough_cache.find(key); it != rough_cache.end()) {
                rough_cache_hit_cnt++;
                RoughCacheEntry rough = it->second;
                if (should_refresh_rough_cache_hit(rough)) {
                    refresh_rough_cache_entry(macro, pre_pre_macro_move, use_zero_gain_pre_macro, rough);
                }
                if ((double)rough.score > reject_limit) {
                    record_early_reject();
                    return false;
                }
                if (!rough.has_state) {
                    refresh_rough_cache_entry(macro, pre_pre_macro_move, use_zero_gain_pre_macro, rough);
                    if ((double)rough.score > reject_limit) {
                        record_early_reject();
                        return false;
                    }
                }
                if (should_full_eval_from_rough(rough.state)) {
                    state = evaluate_full(macro, solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
                } else {
                    state = with_state_macro(std::move(rough.state), macro, use_zero_gain_pre_macro);
                }
                return true;
            }
        }

        early_check_cnt++;
        auto rough_inner_state = solver.anneal_us(EARLY_REJECT_SOLVER_TIME_US, false);
        if ((double)rough_inner_state.score > reject_limit) {
            store_rough_cache_score(macro, pre_pre_macro_move, use_zero_gain_pre_macro, rough_inner_state.score);
            record_early_reject();
            return false;
        }

        auto [out, expanded] = solver.restore(rough_inner_state);
        auto sim = compute_raw_score(in, out);
        State rough = make_state(macro, pre_pre_macro_move, use_zero_gain_pre_macro, std::move(out), sim, false);
        store_rough_cache(rough);
        if ((double)rough.score > reject_limit) {
            record_early_reject();
            return false;
        }

        if (should_full_eval_from_rough(rough)) {
            state = evaluate_full(macro, solver_macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        } else {
            state = std::move(rough);
        }
        return true;
    }

    struct SA {
        struct StaticData {
            MacroSearch *search = nullptr;
        };
        StaticData data;

        SA(MacroSearch *search) {
            static_assert((int)size(MACRO_NEIGHBOR_BASE_WEIGHTS) == MACRO_NEIGHBOR_TYPE_SIZE);
            static_assert((int)size(MACRO_NEIGHBOR_MIN_ACCEPT_RATES) == MACRO_NEIGHBOR_TYPE_SIZE);
            static_assert((int)size(MACRO_NEIGHBOR_MAX_ACCEPT_RATES) == MACRO_NEIGHBOR_TYPE_SIZE);
            data.search = search;
        }

        struct Transition {
            const int TransitionTypeSize = MACRO_NEIGHBOR_TYPE_SIZE;

            double adaptive_weight(int type, const StaticData &data) const {
                if (MACRO_NEIGHBOR_BASE_WEIGHTS[type] == 0) return 0.0;
                if (!ENABLE_ADAPTIVE_MACRO_NEIGHBOR) return MACRO_NEIGHBOR_BASE_WEIGHTS[type];
                const auto &stats = data.search->neighbor_stats[type];
                double accept_rate =
                    (stats.accept + MACRO_NEIGHBOR_PRIOR_TRIALS * MACRO_NEIGHBOR_PRIOR_ACCEPT_RATE)
                    / (stats.trial + MACRO_NEIGHBOR_PRIOR_TRIALS);
                accept_rate = clamp(accept_rate,
                                    MACRO_NEIGHBOR_MIN_ACCEPT_RATES[type],
                                    MACRO_NEIGHBOR_MAX_ACCEPT_RATES[type]);
                return MACRO_NEIGHBOR_BASE_WEIGHTS[type] * accept_rate;
            }

            int select_type(const StaticData &data) const {
                double weight_sum = 0.0;
                rep(type, 0, TransitionTypeSize) weight_sum += adaptive_weight(type, data);
                double x = rnd.rand() * weight_sum;
                rep(type, 0, TransitionTypeSize) {
                    x -= adaptive_weight(type, data);
                    if (x < 0.0) return type;
                }
                return TransitionTypeSize - 1;
            }

            bool execute(State &state, StaticData &data, float current_temp) {
                int type = select_type(data);
                data.search->active_neighbor_type = type;
                bool res = false;
                switch (type) {
                    case 0: res = insert_chars(state, data, current_temp); break;
                    case 1: res = remove_chars(state, data, current_temp); break;
                    case 2: res = single_turn_flip_chars(state, data, current_temp); break;
                    case 3: res = change_tail_turn_chars(state, data, current_temp); break;
                    case 4: res = insert_block(state, data, current_temp); break;
                    case 5: res = prepend_zero_f_block(state, data, current_temp); break;
                    case 6: res = resize_block_f(state, data, current_temp); break;
                    case 7: res = jitter_all_block_f(state, data, current_temp); break;
                    case 8: res = jitter_random_blocks_f(state, data, current_temp); break;
                    case 9: res = change_pre_pre_macro_move(state, data, current_temp); break;
                    case 10: res = toggle_zero_gain_pre_macro(state, data, current_temp); break;
                    case 11: res = flip_block_turn_range(state, data, current_temp); break;
                    default: assert(false);
                }
                data.search->active_neighbor_type = -1;
                data.search->neighbor_stats[type].trial++;
                if (res) data.search->neighbor_stats[type].accept++;
                return res;
            }

            bool should_accept_transition(ll score_diff, float current_temp) {
                return score_diff <= 0 || log_rand_table() * current_temp < -score_diff;
            }

            bool try_apply(State &state, const string &macro, char pre_pre_macro_move,
                           bool use_zero_gain_pre_macro,
                           StaticData &data, float current_temp) {
                string normalized = data.search->normalize_macro(macro);
                string solver_macro = data.search->normalize_macro_for_downstream(normalized);
                use_zero_gain_pre_macro =
                    data.search->canonical_use_zero_gain_pre_macro(solver_macro, use_zero_gain_pre_macro);
                if (normalized == state.macro
                    && pre_pre_macro_move == state.pre_pre_macro_move
                    && use_zero_gain_pre_macro == state.use_zero_gain_pre_macro) {
                    return false;
                }

                State next;
                if (ENABLE_EARLY_REJECT) {
                    double accept_allowance = -log_rand_table() * current_temp;
                    double accept_score_limit = (double)state.score + accept_allowance;
                    if (!data.search->evaluate_for_transition(normalized, pre_pre_macro_move, use_zero_gain_pre_macro,
                                                              accept_score_limit, next)) {
                        return false;
                    }
                    if (next.score > state.score && (double)(next.score - state.score) >= accept_allowance) return false;
                } else {
                    next = data.search->evaluate(normalized, pre_pre_macro_move, use_zero_gain_pre_macro);
                    ll score_diff = next.score - state.score;
                    if (!should_accept_transition(score_diff, current_temp)) return false;
                }
                state = std::move(next);
                return true;
            }

            bool try_apply(State &state, const string &macro, char pre_pre_macro_move,
                           StaticData &data, float current_temp) {
                return try_apply(state, macro, pre_pre_macro_move, state.use_zero_gain_pre_macro, data, current_temp);
            }

            bool try_apply(State &state, const string &macro, StaticData &data, float current_temp) {
                return try_apply(state, macro, state.pre_pre_macro_move, state.use_zero_gain_pre_macro, data, current_temp);
            }

            bool insert_chars(State &state, StaticData &data, float current_temp) {
                string macro = state.macro;
                if ((int)macro.size() >= MAX_MACRO_LEN) return false;
                int cnt = min((int)rnd(1, 4), MAX_MACRO_LEN - (int)macro.size());
                int pos = rnd((int)macro.size() + 1);
                macro.insert(pos, data.search->random_macro_piece(cnt));
                return try_apply(state, macro, data, current_temp);
            }

            bool remove_chars(State &state, StaticData &data, float current_temp) {
                string macro = state.macro;
                if (macro.empty()) return false;
                int cnt = min((int)rnd(1, 4), (int)macro.size());
                int pos = rnd((int)macro.size() - cnt + 1);
                macro.erase(pos, cnt);
                return try_apply(state, macro, data, current_temp);
            }

            bool single_turn_flip_chars(State &state, StaticData &data, float current_temp) {
                string macro = state.macro;
                vector<int> positions;
                rep(i, 0, (int)macro.size()) {
                    if (macro[i] == 'L' || macro[i] == 'R') positions.push_back(i);
                }
                if (positions.empty()) return false;
                int pos = positions[rnd((int)positions.size())];
                macro[pos] = (macro[pos] == 'L' ? 'R' : 'L');
                return try_apply(state, macro, data, current_temp);
            }

            bool change_tail_turn_chars(State &state, StaticData &data, float current_temp) {
                string macro = state.macro;
                int base_len = macro.size();
                while (base_len > 0 && (macro[base_len - 1] == 'L' || macro[base_len - 1] == 'R')) {
                    base_len--;
                }
                string base = macro.substr(0, base_len);
                string current_tail = macro.substr(base_len);

                static const string tails[4] = {"", "R", "L", "RR"};
                vector<string> candidates;
                for (const string &tail : tails) {
                    if (tail == current_tail) continue;
                    if ((int)base.size() + (int)tail.size() > MAX_MACRO_LEN) continue;
                    candidates.push_back(tail);
                }
                if (candidates.empty()) return false;

                macro = base + candidates[rnd((int)candidates.size())];
                return try_apply(state, macro, data, current_temp);
            }

            bool try_apply_blocks(State &state, const vector<MacroBlock> &blocks, StaticData &data, float current_temp) {
                return try_apply(state, data.search->blocks_to_macro(blocks), data, current_temp);
            }

            bool insert_block(State &state, StaticData &data, float current_temp) {
                vector<MacroBlock> blocks = state.blocks;
                int rem = MAX_MACRO_LEN - data.search->blocks_length(blocks);
                MacroBlock block;
                if (!data.search->random_positive_f_block(rem, block)) return false;
                int pos = rnd((int)blocks.size() + 1);
                blocks.insert(blocks.begin() + pos, block);
                return try_apply_blocks(state, blocks, data, current_temp);
            }

            bool prepend_zero_f_block(State &state, StaticData &data, float current_temp) {
                vector<MacroBlock> blocks = state.blocks;
                int rem = MAX_MACRO_LEN - data.search->blocks_length(blocks);
                int turns[3] = {1, 2, 3};
                vector<int> candidates;
                rep(i, 0, 3) {
                    if (data.search->turn_len(turns[i]) <= rem) candidates.push_back(turns[i]);
                }
                if (candidates.empty()) return false;
                MacroBlock block{0, (unsigned char)candidates[rnd((int)candidates.size())]};
                blocks.insert(blocks.begin(), block);
                return try_apply_blocks(state, blocks, data, current_temp);
            }

            bool resize_block_f(State &state, StaticData &data, float current_temp) {
                vector<MacroBlock> blocks = state.blocks;
                if (blocks.empty()) return false;
                int idx = rnd((int)blocks.size());
                int cur_len = data.search->blocks_length(blocks);
                int min_f = idx == 0 ? 0 : 1;
                int max_f = (int)blocks[idx].f + (MAX_MACRO_LEN - cur_len);
                int low = max(min_f, (int)blocks[idx].f - 5);
                int high = min(max_f, (int)blocks[idx].f + 5);
                if (low > high) return false;
                int nf = rnd(low, high + 1);
                if (nf == blocks[idx].f && low < high) nf = (nf == high ? nf - 1 : nf + 1);
                blocks[idx].f = (unsigned char)nf;
                return try_apply_blocks(state, blocks, data, current_temp);
            }

            bool jitter_all_block_f(State &state, StaticData &data, float current_temp) {
                vector<MacroBlock> blocks = state.blocks;
                if (blocks.empty()) return false;
                bool changed = false;
                rep(i, 0, (int)blocks.size()) {
                    int delta = rnd(3) - 1;
                    int nf = (int)blocks[i].f + delta;
                    int min_f = i == 0 ? 0 : 1;
                    nf = max(min_f, nf);
                    if (nf != blocks[i].f) changed = true;
                    blocks[i].f = (unsigned char)nf;
                }
                if (!changed) return false;
                if (data.search->blocks_length(blocks) > MAX_MACRO_LEN) return false;
                return try_apply_blocks(state, blocks, data, current_temp);
            }

            bool jitter_random_blocks_f(State &state, StaticData &data, float current_temp) {
                vector<MacroBlock> blocks = state.blocks;
                int block_count = blocks.size();
                if (block_count == 0) return false;
                int count = rnd(block_count / 2, block_count);
                bool changed = false;
                rep(_, 0, count) {
                    int idx = rnd(block_count);
                    int delta = rnd(2) * 2 - 1;
                    int min_f = idx == 0 ? 0 : 1;
                    int nf = max(min_f, (int)blocks[idx].f + delta);
                    if (nf != blocks[idx].f) changed = true;
                    blocks[idx].f = (unsigned char)nf;
                }
                if (!changed) return false;
                if (data.search->blocks_length(blocks) > MAX_MACRO_LEN) return false;
                return try_apply_blocks(state, blocks, data, current_temp);
            }

            bool flip_block_turn_range(State &state, StaticData &data, float current_temp) {
                vector<MacroBlock> blocks = state.blocks;
                if (blocks.empty()) return false;
                // int l = rnd((int)blocks.size());
                // int r = rnd(l + 1, (int)blocks.size() + 1);
                int l = 0;
                int r = (int)blocks.size();
                bool changed = false;
                rep(i, l, r) {
                    if (blocks[i].turn == 1) {
                        blocks[i].turn = 3;
                        changed = true;
                    } else if (blocks[i].turn == 3) {
                        blocks[i].turn = 1;
                        changed = true;
                    }
                }
                if (!changed) return false;
                return try_apply_blocks(state, blocks, data, current_temp);
            }

            bool change_pre_pre_macro_move(State &state, StaticData &data, float current_temp) {
                rep(_, 0, 4) {
                    char move = data.search->random_pre_pre_macro_move();
                    if (move == state.pre_pre_macro_move) continue;
                    return try_apply(state, state.macro, move, data, current_temp);
                }
                return false;
            }

            bool toggle_zero_gain_pre_macro(State &state, StaticData &data, float current_temp) {
                string solver_macro = data.search->normalize_macro_for_downstream(state.macro);
                if (!data.search->zero_gain_pre_macro_flag_relevant(solver_macro)) return false;
                return try_apply(state, state.macro, state.pre_pre_macro_move,
                                 !state.use_zero_gain_pre_macro, data, current_temp);
            }

        } transition;

        struct Temp {
            float start_temp = MACRO_SA_START_TEMP;
            float end_temp = MACRO_SA_END_TEMP;

            float get_temp(float progress) {
                // return pow(start_temp, 1.0f - progress) * pow(end_temp, progress);
                return start_temp + (end_temp - start_temp) * progress;
            }
        } temp;

        State run(int time_limit, State state) {
            int start_ms = timer.elapsed_ms();
            State best_state = state;
            data.search->observe_score(best_state.score);
            float current_temp = temp.start_temp;
            int loop_cnt = 0;
            while (timer.elapsed_ms() - start_ms < time_limit) {
                float progress = (float)(timer.elapsed_ms() - start_ms) / max(1, time_limit);
                current_temp = temp.get_temp(min(1.0f, progress));
                loop_cnt++;
                bool accept = transition.execute(state, data, current_temp);

                if (accept && state.full_evaluated && state.score < best_state.score) {
                    if (state.score < data.search->best_update_log_score) {
                        if (!data.search->best_update_log_started) {
                            cerr << Log::INDENT
                                 << left << setw(12) << "loop_cnt"
                                 << right << setw(8) << "score"
                                 << setw(12) << "operations"
                                 << "  " << left << setw(28) << "macro"
                                 << setw(10) << "pre_pre"
                                 << "zero_pre\n";
                            data.search->best_update_log_started = true;
                        }
                        cerr << Log::INDENT
                             << left << setw(12) << loop_cnt
                             << right << setw(8) << state.score
                             << setw(12) << state.expanded_basic
                             << "  " << left << setw(28) << Log::macro_rle(state.macro)
                             << setw(10) << MacroSearch::pre_pre_macro_move_name(state.pre_pre_macro_move)
                             << MacroSearch::bool_name(state.use_zero_gain_pre_macro) << '\n';
                        data.search->best_update_log_score = state.score;
                    }
                    best_state = state;
                }
            }
            return best_state;
        }
    };

    vector<string> initial_seed_macros() const {
        return {
            "FFFFRFFFFFFFFLFL",
            "FFFRFFFFFFLFL",
            "FFFFFFLFFFRFR",
            "FFFLFFFFFFRFR",
            "FFFFLFFFRFR",
            "FFFRFFFFFLFL",
            "FFFFFRFFFFFLFL",
            "FFFFRFFFFFFFFFLFL",
            "FFFFFFLFFFFFFRFR",
            "FFFLFFFFFFFRFR",
            "FFFFLFFFFRFR",
            "FFFFFLFFFRFR",
            "FFFLFFFFRFR",
            "FFFFRFFFFLFL",
            "FFFFFFFFRFFFFLFL",
            "FFFFFFFLFFFFFFRFR",
            "FFFFFFFRFFFFFFLFL",
            "FFRFFFFFFLFL",
            "FFFLFFFFFRFR",
            "FFFFFLFFFFFRFR",
            "RFFFFFFFFRFFFFLFFLFL",
            "FFFFFFFFLFFFFRFR",
            "FFFFRFFFFFLFFL",
            "FFFRFFFFFFFFLFL",
            "FFFFRFFFFLFFL",
            "FFFFFFFFRFFFFFLFL",
            "FFLFFFFRFR",
            "FFRFFFFFFFFLFL",
            "FFFRFFFLFL",
            "FFFFFFFRFFFFFLFL",
            "FFFLFFFFFFFFFRFRFFFFFFFFFFFFLFFFL",
            "FFFRFFFFLFL",
            "LFFFFFFFLFFFFRFFRFR",
            "RFFFFFFRFFFFFFLFFFLFL",
            "LFFFFFFFFLFFFFRFRFR",
            "FFFFFFLFFFFFRFFR",
            "FFFLFFFFFFRR",
            "FFFFFRFFFFFFFFFLFL",
            "FFFRFFFFFFRR",
            "FFRFFFFLFL",
            "FFFFFFRFFFFFLFFL",
            "FFFRFLFFFRFFFRR",
            "FFFLFFFRFFFRFFFFFFLFL",
            "LFFFRFFFLFFFFFFRR",
            "FFFLFFFFFFFFRFR",
            "FFFFFRFFFFFFRR",
            "FFFFFFLFFFFFFFRFR",
            "FFLFFFFFFFRFR",
            "FFFFFLFFRFR",
            "FFFRFFFFFFFFFRR",
            "FFFLFFFFFFFRR",
            "FFFFRFFFFFFLFFL",
            "FFFFRFFFFFFFFLFFL",
            "FFFRFFFFLFFL",
            "FFFRFFFLFLFFFFFFFFFRFR",
            "FFFRFLFFRFFRR",
            "FRFFFFFRR",
            "FFFRFFFFFFLFLFFFFFFFFFFRFR",
            "FFFFRFFFFRR",
            "FFRFFFFFRR",
            "FFFFRFFFFFFFFFFLFL",
            "RFFFFFLFFFFFRFFFFFRFFR",
            "FFFFRFFFFFFFFFLFLFFFFFFFFFFFFRFFR",
            "FFFLFFFFFFFRFFR",
            "FFFFFLFFFFFFFFFFRFR",
            "FFFFLFFFRR",
            "FFFFFRFFFRR",
            "FFFRFFFFFRR",
            "FFFRFFFFFFFRR",
            "FRFFFFFLFL",
            "FFFFRFFRR",
            "FFFFRFFFRR",
            "FFFLFFFFFFFFFRR",
            "FLFFFFFRFFR",
            "FRFFFLFFFRFFFFFFRR",
            "RFLFFFRFLFFFRFFFRR",
            "FFLFFFFFFFRR",
            "FFFFFFLFFFFFFFFRFR",
            "FFFFFLFFFFFFRFFR",
            "FFFFRFFFFFFFFRR",
            "RFFLFFFFRFFFFFFFRR",
            "FFFFFLFFFFFLFFFFFRFFFFFRFFFR",
            "FFFFFRFFFFRR",
            "RFFFFLFFFFRFFFFFFFFRFR",
            "FFFFFLFRFRFFFFFLFL",
            "FFFLFFFFFFFFFFRFFRFFFFFFFFFFFFLFFL",
            "FLFFFFFRFR",
            "FFFFLFFFRFFR",
            "FFFLFFFFRR",
            "FFFFFFFFFRFFFFLFL",
            "FFFFRFFFFFFLFFRFFRR",
            "FFFFFFRFFFFFLFLFFFFFFFFFFRFR",
            "FFFFFLFFRR",
            "FFFFFRFFRR",
            "RFFFLFFFFFFRFFFFRFR",
            "FFFFFRFLFLFFFFFFRFR",
            "FFFFFLFFFFLFFFFRFFFFRFFR",
            "LFRFFFFFFFFLFFFFRR",
            "RFFFFFFFFFRFFFFFFFLFLFL",
            "FFFFFFRFFFFFFRFFFFFFLFLFFFRFFFLFL",
            "FFFFFRFFFFFFFFRR",
            "RFFFFFFFFFFFFRFFFFLFLFL",
            "FFFRFFFFLFFRFFFRR",
            "FFFFFFFFFRFFFLFL",
            "FFLFFFFFFRR",
            "FFLFFFFRR",
            "FFFFRFFFFFFFRR",
            "FFFRFFLFFFRFFFFRR",
            "FFFFFFRFFFFFRR",
            "FFFFRFFFFLFFFFL",
            "RFFFFFRFFFLFFRR",
            "FFFFFRFFFFFLFFFL",
            "FFFFLFFFRFRFFFFFFFFFFFFLFFLFFFFFFFFFFFFRFR",
            "FLFFFFRFR",
            "FFFFFLFFFRFRFFFFFFFFLFL",
            "FFFRFFFFFFFFRR",
            "RFFFFRFLFFRFR",
            "FFLFFFFFRR",
            "RFFFFFFFRFFR",
            "RFFFFRFLFFFRFFFR",
            "RFFFLFFFRFFFFRR",
            "RRFLFFFFFFRFFFR",
            "FFFRFFFFFFFFFLFLFFFFFFFFFFFFFRFFFR",
            "FFFLFFFFFFFFFFFRFRFFFFFFFFFFFFLFL",
            "LFFFFFFFLFFFL",
            "FLFFFFFFFFRFFR",
            "FFFLFFFFFRFFLFFRR",
            "LFRFFFFFFLFFFRR",
            "RFFFFRFFFFLFFRFR",
            "FFFFLFFFFFRFFLFFRR",
            "FFFLFFFFFRR",
            "LFFFFFFFRFFFLFL",
            "FRFFFRFFFLFFFRR",
            "LFFFFRFFFFRFFFFFFFFFFFFFFFFLFLFFFFFFFFFFFFRR",
            "FLFFRFFFLFFFFRR",
            "RFFFFFFFRFR",
            "LFFFFFFFFFFLFFFFFRFRR",
            "FFLFFFRFR",
            "FLFFFFFFRR",
            "FFFFFFRFFFFFFFRR",
            "RFFFFFFLFFFFFFRFFFR",
            "FFFFFFRFFFFFFFFFFFFLFLFFFFFFFFFFFFFFRFR",
            "FFFFRFFFFFLFFRFRR",
            "FFFFLFFFFFFFRR",
            "FRFFFLFFFRFFFRR",
            "LFFFFFFLFFFFRFLFL",
            "FFFLFFFFFRFRFFFFFFFFFLFFFL",
            "FFFFRFFFFFFFFFFFFFFFFLFFLFFFFFFFFFFFFFFFFRFFR",
            "FFLFFFFFFFFFRFFFRFFFFFFFFFFFLFFFL",
            "FFFFFFLFFFFRFRF",
            "FRFFFFFFFFLFFL",
            "FFRFLFFRFFFRR",
            "FFRFFFFLFFFRFFFRR",
            "LFFFFFFRFFFFFLFFFFFLFL",
            "FFRFFFFLFFFFLFFFFFFFFFFFFRFRFFFFFFFFFFFFLFL",
            "FFRFFFLFL",
            "RFFFFFFFFFFRFFFLFRFR",
            "FFFFRFFFFFFFFFLFLFFFFFFFFFFFFFRFFR",
            "FFFFFFLFFFFFFLFFFFFFRFFFFFFRFFFFR",
            "LFFFRFFFFFFLFFFFRR",
            "FFLFFFFFFFFFFRFFR",
            "FFFLFFFFFFFFRR",
            "RFFFFFFLFFFFFRFFFRFFR",
            "FFFFFFLFFRR",
            "FFLFFFFFFFFFRFRFFFFFFFFFFFFLFL",
            "FLFFRFLFFFFRFFR",
            "FFLFFFFFFFFFFFFRFRFFFFFFFFFFFFLFFFFL",
            "FFRFFFLFFFFRFFLFL",
            "RFFFFFRFR",
            "FFFRFFLFFRFRR",
            "FFRFFLFFLFFFFFFFRFFR",
            "FFFLFFFFRFFFLFRR",
            "LFFFFLFFFFRFFRR",
            "FFFLFFFRFFFLFFFFLFL",
            "LFFFRFFFLFFFFFFLFL",
            "FRFFFFLFFFFFFFRFR",
            "FRFFFFFFFLFL",
            "RFFFRFFFLFFFFFFRFR",
            "FFRFFFFRR",
            "LFFFFRFFFFR",
            "FFFFRFFLFFL",
            "LFFFFFFRFFFFFFLFFLFFFL",
            "FFRFFFFLFFLFFFFFFFFRFRFFFFFFFFFFFFLFL",
            "LFFRFFFFRFFFFFFFFFFFFLFLFFFFFFFFFFFFFRFFR",
            "RFFFLFFRFFFRR",
            "LFRFFLFFFFFFRR",
            "FFFFLFFFFFFFFRFRF",
            "FFRFFFFFFFFFFFLFL",
            "RFFLFRFFFRR",
            "RFFRFFFLFFFFRFR",
            "FFLFFFRFLFFFRR",
            "RFFFFFFFFLFFFFFRFR",
            "FFFRFFFFFFLFFFLFL",
            "RFFFFFFLFFFRFFFFFFRR",
            "FFFRFFFFFRFFFLFFFLFFFL",
            "RFFFFFFLFFFFFFRFFRFFR",
            "RFFFFFFFFFLFFFRFR",
            "FFFRFFFFLFFFL",
            "FFFFFRFFFFFFFFFFFFLFLFFFFFFFFFFFFRFFR",
            "FFFFFRFFFFFFFRR",
            "FFFFLFFFFRFRFFFFFFFFFLFLF",
            "FFFFFFFFRFFFLFFL",
            "LFFFRFFFRFFFFFFLFFLFFFFFFFFFRFR",
            "FFFFLFRFFRFFFFFLFL",
            "FRFFFFFFFFRR",
            "RFFFFFFFFFRFFFFFFLFFFRFR",
            "FFFFFLFFRFFR",
            "RFFFFFFFFFLFFFFRFR",
            "FRFFFFFFLFFLFFFFFFFFFRFFFR",
            "FFLFFFFFFFFFFRFR",
            "LFFFFFLFFFL",
            "RFFFFFFLFFFFFFRFFFFRFFFR",
            "FFRFFFFLFLFFFFFFFFFFRFFR",
            "LFFFLFFFFRFFFRR",
            "FFLFFFFFFRFFLFFFFFRR",
            "FRFFFFFFLFFFRFRFR",
            "FFFLFFFFFRFLFFFFRR",
            "FLFFFFRFFLFFFRFFFRR",
            "FFFLFFFFFFFLFFFRFFFRFR",
            "FRFFFFFFFFFLFFFRFR",
            "FLFFFFFRFRFFFFFFFFFFFFFFFFFFFFLFFLFFFFFFFFFFRFLFFFFFRR",
            "LFFFFFFRR",
            "FFFFFFFFFRFFFFFFFFLFLF",
            "FFLFFFFFRFFFFLFL",
            "FFFFFFFFFRFFFFLFLFFFFFFFFRFRF",
            "FRFFFLFFFL",
            "FRFFFFFFFFFLFL",
            "FFFFRFFFFFLFFLFFFFFFFFRFFFFR",
            "FFFFFFFFFLFFFFFFLFFFFFFRFFFFFFFRFFFR",
            "FFFFLFFFFRFFFFLFFLFL",
            "FFFRFFFRR",
            "LFFLFFFRFFFFLFFL",
            "FFFFLFFFFFFFFFFFFFFFFRFFFRFFFFFFFFFFFFLFL",
            "FFFFRFFFFFFFFFFFFFFFFLFFFFLFFFFFFFFFFFFFFFFRFR",
            "FLFFFFFFRFFFFLFFFFLFFL",
            "RRFLFFFFRFR",
            "FLFFFFRFFLFFFFRFFFFRR",
            "FFFFLFFFFRFFLFFFFLFL",
            "LFFFFFFFFLFFL",
            "FFFFFLFFFFFRFRFFFFFFFFFFFFFFFFFFFFFFFFFLFLFFFFFFFFFFFFFFFRFFR",
            "LFFFRFFFFFFLFFFLFFFRFFFFFFRFR",
            "RFFFLFFFLFFFRFRFFFFFFFLFL",
            "FRFFFFFFLFFFFFFRFFFFR",
            "LFFRFFFFFLFFFFFFFRR",
            "LFRFFLFFFFFFRFR",
            "FFFFRFFFFFFLFFFLFFFRFFRFR",
            "FFFFFFLFFFRFFRFFFFFFFFFLFL",
            "FFFLFFFLFRFFFRFR",
            "FFFRFFLFFFFFRFFFFRR",
            "FFFFLFFFFFFFRFLFFFRR",
            "FFFFFFRFFFFFRFFFFLFFFFLFFFFL",
            "FFFFRFFLFFFLFFFFFRFFR",
            "FLFFFFRR",
            "LFFFFFFRFR",
            "FFFLFFFFFFFRFFFRFFFFFFLFL",
            "LFFFFFFFLFFL",
            "FFFRFFFFLFFLFFFFFFFRFFFFR",
            "FFFFRFFFFFRFFFFFFFFLFFFFLFFFFFFFFRFR",
            "RFFFFLFFFL",
            "FFRFFFFFRFFFFFFLFFFFLFL",
            "LFFFFFFRFFRFFFFFFFFFFFFFFFFFFLFLFFFFFFFFFFFFFRFFR",
            "RFFRFFLFFFRFFR",
            "FFLFFRFFFRFFFFFFLFL",
            "FFLFFFFFRFFFFFFRR",
            "FLFFFFFFFFRFFFFFLFFL",
            "FRFFFLFFFLFFFFFFFRFR",
            "RFFFFLFFFFFFFRFR",
            "FFFLFFFFFFFFFFFFRFR",
            "FFFFLFFFFFFFFFRFFFRFR",
            "RFFFRFFFFFR",
            "FFFLFFFFFFFFFRFLFFFRR",
            "RFFFFRFFFLFFFFFFRR",
            "RFFFFFFLFFFFFFRFFFRFFFFR",
            "FFFFFFLFFFFFFRR",
            "FFFFLFFFFRFRF",
            "FFFFRFFFFLFFFRFLFL",
            "FFFFLFFFFRFFRFFFFFFFFFFLFFFL",
            "FFFLFLFFRFFRFFFLFFFFRR",
            "FFLFFRFFFFLFRR",
            "FFFFFLFFFFLFFFFFFRFFFFFRFFFFR",
            "FFFFFLFFFFLFRFFRFFR",
            "FFLFFFFFFFFFFFFFFFRFFRFFFFFFFFFFLFFFRFFFFFLFL",
            "FFFFFFFLFFFFFFRFFFRFFFR",
            "LFFRFFFFFLFFFFFFFLFL",
            "FFLFFRFRFFFFFFFFFLFL",
            "FFFFRFFFFFLFFFFFLFL",
            "FFFFLF",
            "FRFLFFFRFRR",
            "RFFFFFFFLFFFRFFFR",
            "FFFFRFFFFLFFFFLF",
            "FFFFFFLFFFRFRF",
            "RFFRFFFFLFFFRFFR",
            "FFFFRFFFLFFFRFR",
            "FLFFFFFRFFFLFFFFFFLFL",
            "FFFFFFLFFFFFFRFFFFFFRFR",
            "FFRFFFFFFFFFFLFFLFFFFFFFFFFRFLFFFFFRFR",
            "RRFFFRFFFRFFFFFFFFFFFFFFFLFLFFFFFFFFFFFFFRFFR",
            "FFLFFFFFRFFRFFFFFFFFFLFFFL",
            "LFFFFFFRFLFFFLFFFFFRFRFR",
            "FFFFFFFRFFFFFFFRFFFLFFFLFFL",
            "FLFFFFFFRFFFFFFLFFL",
            "FLFFFFFFRFFFLFRFFFLFL",
            "RFLFFFFRFFFFFFFFLFLFFFFFFFFFFRFFFFR",
            "FFFFRFFFFFFLFLF",
            "FFFRFFFFFLFFFRFRR",
            "RFFFLFFFLFFFFFFFFFRFFFFR",
            "LFFFFRFFFRFFFFFFFFFFFFLFFLFFFFFFFFFRR",
            "FFFLFFFRFFFFRR",
            "LFFFFLFFFRFFRFR",
            "FFFFFFFFFFFFRFFFFRFFFFFFFFFFFFLFFFFLFFFFFRFRR",
            "FFFFFFFFFRFFFFRFFFFFFFFLFFLFFFFFFL",
            "LFFRFFFLFFFFFFFFFRFR",
            "FFFFRF",
            "FFFFFFFFFLFFFL",
            "LFFFFFRFFFLFFFL",
            "FFFFFFFFRFFFFFFFFRFFFFFFLFFFLFFFFL",
            "FFFFFFLFFFFRFRFFFFFFFFFFFFFFLFFL",
            "FFFFLFFFFFFFFFFFFRFFFFRFFFFFFFFFFFFFFLFFL",
            "FFRFFFLFFFRFFFFFFFFFRR",
            "FFFFRFFFFFFFFLFFFFLF",
            "FFFFFFLFFFLFL",
            "RFFFFFRR",
            "FFFFFFFFFFFLFFFFFFFLFFFFFFFFFFRFFFFFRFFFFFLFFFFFRR",
            "FLFFFRFFFFFFFLFFFRFFFRR",
            "LFFFFFFFFLFFFL",
            "FFFRFFFFFFFFFFFRR",
            "FFFRFFFFFFLFFFLFFFFFFFFFFFFFRFFRFFFFFFFFFFFFRR",
            "LFFLFFFFL",
            "FFFFFFFFFFFFRFFRFFFFFFFFFFFFFLFFFL",
            "FFFLFFFFFFFLFL",
            "FFRFFFLFFFFFRFR",
            "FRFFFFFLFFFLFFFFFFFFFFRFRFFFFFFFFFFFFFFRR",
            "LFFFRFFFFFFFLFFFFFFFLFL",
            "RFFRFFFFFFFFLFFLFFFFFFFFFFFFFFFFRFFRFFFFFFFFLFL",
            "FFRFFFFFFFLFFFLFFFFRFR",
            "FRFFFFLFFRFLFFRFFRFR",
            "FFFLFFFFFFFFFRFRFFFFFFFFFFFFFLFLFFFFFFFFFFFFRFFFR",
            "RFFFFLFFRFFFFRR",
            "FFFFRFFFFLFFFFRFFFFFFFFRR",
            "FLFFFRFFRFFFFFFFFFLFL",
            "RFLFFLFFFFRFFR",
            "FFFFRFFFFLFFFFRFRFR",
            "FFLFFFFRFFFLFFFFFFFRR",
            "RFFFFFFFFFFFFRFRFFFFFFFFFFFFFFFFFFLFFLFFFFFFFFFFFFRFFRFR",
            "FFFFFFR",
            "LFFFFFFFFFLFL",
            "RFFFLFFFFFFFRFFR",
            "FFFFFFFFFLFFFFFFFRFFFFRFFR",
            "RFFFFFFLFFLFFFFFFFFFFFRFFRFFFFFFFFFFRR",
            "FFFFLFFFFFFRFRF",
            "FFFFRFFFFFFFFLFFFFLFFFFFFFFFFFRFR",
            "FFFFRFFFFFFFFLFFFFFFFFRR",
            "FFLFFFFFRFFFLFFRR",
            "FFRFFFFFFFFFFFLFLFFFFFFFFFFFFRFFFR",
            "FFRFFFFFFFFFRR",
            "RFFFLFFFFFFRFFFFRF",
            "FFFFLFFFFFFFFRFFFFLFFLFL",
            "FRFFFFFFFFLFFFRFR",
            "RFFFFFFLFFFFRFR",
            "FFFFRFFFFFFFLFLFFFFFFFFFLFRFFRFFFR",
            "FFFRFFFFFFLFFFFFFLFFL",
            "LFFFFFRFFFLFFFLFFFL",
            "LFFFRFLFFFRR",
            "RFFLFFFFFFRFFFLFL",
            "FFFFRFFFFFFRFFFFLFFFFLFFFFFRFFFFFRR",
            "LFFFRFFFRFFFFFFLFFFL",
            "FFFFFFFFFFFFRFRFFFFFFFFFFFFLFFFFL",
            "FFFFFRFFFFFLFFFLFL",
            "FFFFLFFFFRFFFFLFFFFLFFFFFFFFRFRFFLFFFFFRFFFFR",
            "FRFFFFFRFLFFRFFLFFRR",
            "FRFFFFFLFFFFFFLFFFFFFFFFFFFFFRFFRFFFFFFFFFFFFFFFFRR",
            "FFFFLFFFFFFLFFFRFRFFR",
            "FFFFRFFFFFFFFFFFFLFFFFLFFFFFFFFFFFFFFFFFFFFRFR",
            "RFFFFFFFFFFFFRFFFFFFFLFLFFL",
            "LFRFFRFFFFFFFFRR",
            "FFFLFFFRFFFLFFFFFFFFFLFL",
            "RFFRFRFFFFFFFLFFFFL",
            "LFFFRFFFFFFLFFFFFFFLFFFRFFFRFFR",
            "FLFFFFFFRFFFFFFFFFFFLFLFFFFFFFFFFRFFFFFRFR",
            "FRFFFFFFFFFFFFLFFLFFFFFFFFFFFFRFR",
            "FRFFFFLFFFFFFRFFFR",
            "FFFFFR",
            "FFFFFLF",
            "RFFFLFFFFFFFFFRFFR",
            "FFRFFFL",
            "FRFFFFFFFFLFFFFFRRF",
            "RFFFFFFFFLFFFFFFFFRFFFRFFFR",
            "RRFFRFFFFLFFFFFFL",
            "FFFFFL",
            "FFFFLFFFFFFFFFRFFFFFFRFR",
            "FLFFFF",
            "FFFFLFFFFRFFFRFR",
            "FRFFLFFLFFFFRFRFFFFFFFFLFL",
            "FFFFRFFFFFLFLFFFFFFFFFFRFFFFRF",
            "RFFFFFFFFFLFL",
            "FFLFFFF",
            "FFRFFFFLFFLFFFFFRFR",
            "LFFFFLFFFFFFFFRFRFR",
            "RFFFFLFFRF",
            "FFFFFFFRFFFFFLFFLF",
            "FFRFFFFFFFFFFRR",
            "FFRFFFFFLFFFFFFRFFFFFRFR",
            "RFFFFFFFFLFFFFRFRFFR",
            "RFFFFFFFLFLFFFFFFFFFFFFRFFRFFFFFFFFFFFFRR",
            "RFFFFFFFFFRFFFFFFLFLFFFL",
            "FFFFFRFFFFFFLFFLFFFFFFFRFR",
            "FFFFLFFFFFFFFRFFFFRR",
            "FRFFFFLFFFFFFFFRFFFFR",
            "LFFFFLFFFFFFFFFFFFRFRFFFFFFFFFFFFFLFLFFFFFFRFR",
            "FFFFFFFLFFFL",
            "FFFFFLFFFFFFFFFLFL",
            "RFFFFFFFFFFFFLFFLFFFFFFFFFFFFFFFFFFFFRFFRFFFFFFFFRR",
            "RFFFFFFFRFFFFFFLFFFFRRF",
            "LFFFFRFFRFFFFFFLFFFFFFRR",
            "FFFRFFFRFFFFFFLFFFLFL",
            "FFFFFFRFFFRRF",
            "FFFRFFFFFFFFFLFLFFFFFFFFFFRFFRF",
            "FLFFFFFFFFFRFRFFFFFFFFFFFFLFFFFFRFRR",
            "RFFFFFFFFFFFFFLFFLFFFFFFFFFFFFFFFFFRFFFR",
            "FLFFFRFFLFFFFRFFFFRR",
            "FFFFFFFFLFFFFFRFFFFFRFR",
            "FFFFRFFFFFFFFFFFFLFLFFFFRR",
            "LFFFFFFFFRFFFFFFLFFLFFL",
            "FLFFFFFFFFRFFFFFFFFFLFFFFLF",
            "FFFFRFFFFFFFFFFFFFFFFFFFFLFL",
            "FRFFFF",
            "RFFFFFFLFFFLFFFFFFFFRFFFR",
            "FFFRFFFFFFFFFFFFFFFFFFLFFFLFFFFFFFFFFFRFFR",
            "FFFFRFLFFFFLFFFFFFFFFFFFFFFFFRFFRFFFFFFFFFFFFFFFFFLFFL",
            "FLFFRFFR",
            "LFFFLFFFFL",
            "FFFRFFFFFFFLFFFFLFFL",
            "FFFFL",
            "LFFFFFFFFFFFFFFFFLFFLFFFFFFFFFFFFFFFFFRFFFRFFFFFFFFLFL",
            "LFFFFFFRFFFLFFLFFFFFFFFFRFFFFFFRFFR",
            "FFLFFFFRFFRFFFFFFFFFLFFFFFLFL",
            "RFLFFFFFFLFFFRFFFR",
            "FFFFFLFFFFFFFFRFFFFRF",
            "FFFFFFFFLFFFFFFFRFFRFFR",
            "FFFFRFFFRRF",
            "LFFFFFFRFFL",
            "RFFFFFFFFFFFFLFFFFFFRFRFFFR",
            "FFFRFFFFFFFFFFFFFFFFFFLFFLFFFFFFFFFFFFFFFFFFRFFFFFR",
            "FLFFFFFFRFFFRFFFFFFLFFLF",
            "FFFFLFFFFFRFRF",
            "FFFLFFFFFFFFFLFFL",
            "FFFFFFRFFFFFFFFLFFRFRFFFFFFFFLFFLFFFFFL",
            "LFFFFRFFFRFFFFFFFFFFFFLFFLFFFFFFFFFFFRFFFFR",
            "RFFFFFFFFFFFFFRFFFFFFRFFFFFFFFFFFFFFFFLFFLFFFFFRR",
            "FFRFFFFFFLFFLFFRR",
            "FFFRFFFFLFLFFFFFFFFRFFFRF",
            "FFFRFFFLFRFFFRFR",
            "LFLFFFFFFLFFFFFFLFFFFFFFFFFFFFRFFRFFFFFFFFFFFFFRR",
            "RRFFFRFFLFL",
            "FFFFFFFFFRFFFRFFFFFFFFFLFFFLFFL",
            "FFFFFFFRFF",
            "FFFFFFFFFFRFFFFRFR",
            "FFFFRFFFFRFFFFFFFFLFFFFLFFFFFL",
            "FFFRFFFRFFFFLFFFFFRR",
            "FRFFFFLFFFFFFFFFRFFFFFLFFFFRR",
            "FFFFFRFFFFRFFFLFFFFFLFFFL",
            "LFFFFFLFFFFFFRFRFFR",
            "FFFFFFLFFFFFFFRFFFRF",
            "FFFFFRFFFFFFFRRF",
            "FLFFFFRFFFFFFFFFLFFFFLFFFFFRFFFFRFFFFR",
            "FFRFFFFFLFFFRFFFFR",
            "LFRFRFFFFFFFFFLFFLFFFFFFFFFFRFFFR",
            "FRFFFFFFFFLFFFFRFRFFFFFFLFFFFRR",
            "RFFFFLFFFLFFFFFFRFFFR",
            "LFLFFFRFFFFFRR",
            "FRFFLFFFFFFFFRFRFFFFFFFFFFFLFFL",
            "FFFRFFFFFFFFLFFFLFFFRFFFR",
            "FFFFFFFLFFFFFFFFFFFFFFRFFRFFFFFFFLFLF",
            "FFFFFFFRFFRRF",
            "FRFFFFFFLFFFFFFFRR",
            "FFLFFFFLFFFRFFFFRR",
            "RFRFFFLFF",
            "FFFFFFFFLFFFFFFFRFFFRFFFFFFFFFFLFL",
            "LFFLFFLFFFFFFFFRFFR",
            "RFRFFFFFFFRFFFFFFFLFFFLFFF",
            "RFFFFFFLFFLF",
            "FFFRFFFFFFFFFLFFLFFRFFFRR",
            "RFFFLFFFFLFFFFFFFFFFFRFFFFFFRFFFFFFFFFFFFFFFFFRFR",
            "FFFFFFFLFFFFLFFFFFFRFFFFFFFFFFRFFR",
            "FFFFFFFFLFFFFFFFFRFFRFFFFFFFFFFFFLFLFFFFFFFFRR",
            "FFFFRFFFLFFFRFLFFFL",
            "FLFFFFFFFFFFRR",
            "RFFFFFFLFFLFFFFFFFFFFFFFFFRFFRFFFFFRR",
            "FFLFFFFFRFFFFFRFFFFFFRR",
            "RFFFFFFLFFFFLF",
            "FRFFFFFFFFRFFFFFFFFLFFLFFFFRR",
            "FFFLFFFFFFFFFRFFFRFFFR",
            "LFFFFFFFFFRFRFFFFFFFFFLFFFL",
            "RFFFFLFFFFFFFFFRFFFFFLFFFRR",
            "RFFFFLFFFFFL",
            "FRFFFFF",
            "RFFFRFFFFFFLFLFFFFFFFRFFFRF",
            "FRFLFFR",
            "FLFFFFFFFFFFFRFRFFFFFFFFFFFFFFFLFLFFFFFFFFFFFFRFR",
            "RFRFFLFFRR",
            "RFFFFLFFFFFLFFFFFFFFRFFFFFFRFFFR",
            "FFFRFFFFRFFFLFFFFFRFR",
            "FFFFFFFFLFFFFFLFFFRFFFFFRFR",
            "FFFRFFFLFFFFFFFFFRFFR",
            "RFFFLFFFFFRFFFLFFFFLFL",
            "LFFFFFFFRFFFFFFFLFFFFLFF",
            "FFFFFFFFFFFFFRFFFFRFFFFFFFFFFFFFFFFFLFFLFFFF",
            "FFFFFFLFFFFFFFFFFFLFFFFFFRFRFFFFFFFFFFLFFFFFRFFFFRFR",
            "FLFFFFFFFFRFFRFR",
            "FRFFF",
            "FLFFFFFFFRFFFLFFFFLFFFL",
            "FLFFFRFFFFRFFFFRFFFLFFFFLFFL",
            "FRFFRFLFFLFFFFRFFFFRR",
            "RFLFFLFFFFFFFFRFFFLFL",
            "FFFFRFRFFFFFFFLFFFFLFFL",
            "FFLFFRFFFFFFLFFFFRFRFFRF",
            "RFFFFFFFFFFLFRFRFFFFFFFFFFLFLFFFFFFLFFFRR",
            "FFFLFFFFFRFFFFFFFFRR",
            "FFFRFFFFFFFFFLFLFFFFFFRFFFRFFFFFFFLFFL",
            "FFRFFFFFFFLFLFFFFFFFFFFFRR",
            "LFFFFLFFFLFFFFFFLFFRFFLFFFFFFFFFFFFRFRFFFFFFFFFFFFFRR",
            "LFFFFRFRFFFFFFFFFRFFFFLFLFFFFFFFFRFFFR",
            "FFFFFFFFFLFFFFFRFRFFFLFFFFFFFL",
            "FFFFFFFFLFFFFRRFFFFFFFRR",
            "FLFLFFFFLFFFFFFFFFFRFRFFFFFFFFFFFFLFFFFRR",
            "LFFFFLFFFFLFFFFRFFFFRFFFFR",

        };
    }

    State run(int search_time_ms) {
        int start_ms = timer.elapsed_ms();
        vector<string> seeds = initial_seed_macros();

        initial_seed_states.clear();
        unordered_set<string> used_seeds;
        vector<InitialSeedCandidate> initial_seed_candidates;
        rep(seed_id, 0, (int)seeds.size()) {
            const string &macro = seeds[seed_id];
            string normalized = normalize_macro(macro);
            if (normalized.empty() || !used_seeds.insert(normalized).second) continue;
            InitialSeedCandidate candidate = evaluate_initial_seed(normalized);
            candidate.initial_seed_id = seed_id;
            initial_seed_candidates.push_back(std::move(candidate));
        }
        sort(all(initial_seed_candidates), [](const InitialSeedCandidate &a, const InitialSeedCandidate &b) {
            return a.inner_state.score < b.inner_state.score;
        });
        int required_initial_seeds = MACRO_SEARCH_PHASES.empty() ? 0 : MACRO_SEARCH_PHASES.front().first;
        initial_seed_states.reserve(required_initial_seeds);
        for (const InitialSeedCandidate &candidate : initial_seed_candidates) {
            State state = restore_initial_seed(candidate);
            if (state.expanded_basic > in.T) continue;
            initial_seed_states.push_back(std::move(state));
            if ((int)initial_seed_states.size() >= required_initial_seeds) break;
        }
        sort(all(initial_seed_states), [](const State &a, const State &b) {
            if (a.score != b.score) return a.score < b.score;
            if (a.buttons != b.buttons) return a.buttons < b.buttons;
            return a.expanded_basic < b.expanded_basic;
        });
        State best = initial_seed_states.empty()
            ? empty_macro_state()
            : evaluate(initial_seed_states[0].macro,
                       initial_seed_states[0].pre_pre_macro_move,
                       initial_seed_states[0].use_zero_gain_pre_macro);
        Log::section("initial seed top10");
        rep(rank, 0, min(10, (int)initial_seed_states.size())) {
            const State &seed_state = initial_seed_states[rank];
            cerr << Log::INDENT << (rank + 1) << ". " << Log::macro_rle(seed_state.macro) << '\n';
        }

        Log::section("MacroSearch");
        best_update_log_score = best.score;
        SA sa(this);
        vector<State> candidates = initial_seed_states;
        State global_best = best;
        auto state_less = [](const State &a, const State &b) {
            if (a.score != b.score) return a.score < b.score;
            if (a.buttons != b.buttons) return a.buttons < b.buttons;
            return a.expanded_basic < b.expanded_basic;
        };
        auto select_top_unique = [&](vector<State> states, int limit) {
            sort(all(states), state_less);
            vector<State> selected;
            selected.reserve(min(limit, (int)states.size()));
            unordered_set<string> used;
            for (const State &state : states) {
                string key = cache_key(state.macro, state.pre_pre_macro_move, state.use_zero_gain_pre_macro);
                if (!used.insert(key).second) continue;
                selected.push_back(state);
                if ((int)selected.size() >= limit) break;
            }
            return selected;
        };

        rep(phase_idx, 0, (int)MACRO_SEARCH_PHASES.size()) {
            int keep_count = MACRO_SEARCH_PHASES[phase_idx].first;
            float phase_ratio = MACRO_SEARCH_PHASES[phase_idx].second;
            vector<State> selected = select_top_unique(candidates, keep_count);
            if (selected.empty()) break;

            int elapsed_ms = timer.elapsed_ms() - start_ms;
            int remaining_total_ms = max(0, search_time_ms - elapsed_ms);
            int requested_phase_ms = max(0, (int)round((double)search_time_ms * phase_ratio));
            int phase_budget_ms = min(remaining_total_ms, requested_phase_ms);
            if (phase_budget_ms <= 0) break;
            int phase_end_ms = timer.elapsed_ms() + phase_budget_ms;
            vector<State> phase_results;
            phase_results.reserve(selected.size());
            rep(i, 0, (int)selected.size()) {
                State start_state = evaluate(selected[i].macro,
                                             selected[i].pre_pre_macro_move,
                                             selected[i].use_zero_gain_pre_macro);
                int starts_left = (int)selected.size() - i;
                int run_budget_ms = max(0, (phase_end_ms - timer.elapsed_ms()) / max(1, starts_left));
                State result = sa.run(run_budget_ms, start_state);
                if (state_less(result, global_best)) global_best = result;
                phase_results.push_back(std::move(result));
            }
            candidates = std::move(phase_results);
            sort(all(candidates), state_less);
        }

        cerr << '\n' << Log::INDENT << "trials:\n"
             << Log::INDENT2 << "early_reject=" << early_reject_cnt << '/' << early_check_cnt << '\n'
             << Log::INDENT2 << "exact_cache: hit=" << cache_hit_cnt
             << ", refresh=" << cache_refresh_cnt
             << ", update=" << cache_refresh_update_cnt << '\n'
             << Log::INDENT2 << "rough_cache: hit=" << rough_cache_hit_cnt
             << ", refresh=" << rough_cache_refresh_cnt
             << ", update=" << rough_cache_update_cnt << '\n';

        cerr << '\n' << Log::INDENT << "[neighbor stats]\n";
        cerr << Log::INDENT2 << left << setw(28) << "type"
             << right << setw(12) << "trial"
             << setw(14) << "early_reject"
             << setw(12) << "full_eval"
             << setw(10) << "accept"
             << setw(10) << "acc_rate" << endl;
        rep(type, 0, MacroSearch::MACRO_NEIGHBOR_TYPE_SIZE) {
            const auto &stats = neighbor_stats[type];
            if (stats.trial == 0) continue;
            cerr << Log::INDENT2 << left << setw(28) << MacroSearch::neighbor_name(type)
                 << right << setw(12) << stats.trial
                 << setw(14) << stats.early_reject
                 << setw(12) << stats.full_eval
                 << setw(10) << stats.accept
                 << setw(10) << Log::percent(stats.accept, stats.trial) << '\n';
        }
        return global_best;
    }
};

void run_inner_score_experiment(const Input &in, const vector<MacroSearch::EvaluatedMacro> &history) {
    if (history.empty()) return;

    vector<MacroSearch::EvaluatedMacro> samples;
    int sample_size = min(EXPERIMENT_SAMPLE_MACROS, (int)history.size());
    samples.reserve(sample_size);
    if (sample_size == 1) {
        samples.push_back(history[0]);
    } else {
        rep(i, 0, sample_size) {
            int idx = (long long)i * ((int)history.size() - 1) / (sample_size - 1);
            samples.push_back(history[idx]);
        }
    }

    cerr << "inner score experiment start: samples=" << samples.size()
         << ", repeat=" << EXPERIMENT_REPEAT << endl;
    ios::fmtflags cerr_flags = cerr.flags();
    streamsize cerr_precision = cerr.precision();
    cerr << fixed << setprecision(2);
    for (const auto &sample : samples) {
        MacroSolver solver(in, sample.macro, sample.pre_pre_macro_move, sample.use_zero_gain_pre_macro);
        cerr << "experiment macro=\"" << sample.macro << "\""
             << ", pre_pre=" << MacroSearch::pre_pre_macro_move_name(sample.pre_pre_macro_move)
             << ", zero_pre=" << MacroSearch::bool_name(sample.use_zero_gain_pre_macro);
        for (int budget_ms : EXPERIMENT_BUDGETS) {
            vector<double> scores;
            scores.reserve(EXPERIMENT_REPEAT);
            rep(_, 0, EXPERIMENT_REPEAT) {
                auto state = solver.anneal(budget_ms, false);
                auto [out, expanded] = solver.restore(state);
                auto sim = compute_raw_score(in, out);
                scores.push_back((double)sim.score);
            }

            double sum = accumulate(all(scores), 0.0);
            double ave = sum / scores.size();
            double var = 0.0;
            for (double score : scores) var += (score - ave) * (score - ave);
            double stddev = sqrt(var / scores.size());
            cerr << ", " << budget_ms << "ms ave=" << ave << " std=" << stddev;
        }
        cerr << endl;
    }
    cerr.flags(cerr_flags);
    cerr.precision(cerr_precision);
    cerr << "inner score experiment done: elapsed_time=" << timer.elapsed_ms() << "ms" << endl;
}

struct ChangeMacroExperiment {
    const Input &in;
    const MacroSearch &search;

    struct MacroEval {
        vector<int> dist;
        ll sum = (1LL << 60);
        int generation_cost = INF;
    };

    struct State {
        string macro_a;
        char pre_pre_macro_move_a = 0;
        bool use_zero_gain_pre_macro_a = false;
        string prefix_b;
        string suffix_b;
        string macro_b;
        ll score = (1LL << 60);
    };

    unordered_map<string, MacroEval> eval_cache;
    vector<State> candidates;

    ChangeMacroExperiment(const Input &in_, const MacroSearch &search_) : in(in_), search(search_) {}

    const MacroEval &evaluate_macro(string macro, char pre_pre_macro_move = 0,
                                    bool use_zero_gain_pre_macro = false) {
        string key = search.cache_key(macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        auto it = eval_cache.find(key);
        if (it != eval_cache.end()) return it->second;

        MacroSolver solver(in, macro, pre_pre_macro_move, use_zero_gain_pre_macro);
        MacroEval eval;
        eval.dist = solver.ball_to_box_button_distances();
        eval.generation_cost = solver.macro_registration_extra_cost();
        eval.sum = 0;
        rep(id, 1, in.M + 1) {
            if (eval.dist[id] >= INF) {
                eval.sum = (1LL << 60);
                break;
            }
            eval.sum += eval.dist[id];
        }
        auto [inserted_it, inserted] = eval_cache.emplace(std::move(key), std::move(eval));
        return inserted_it->second;
    }

    string build_macro_b(const State &state) const {
        return state.prefix_b + state.macro_a + state.suffix_b;
    }

    int macro_b_generation_cost_from_a(const State &state) const {
        if (state.prefix_b.empty() && state.suffix_b.empty()) return 0;
        return (int)state.prefix_b.size() + (int)state.suffix_b.size() + 2;
    }

    ll evaluate_pair(const State &state) {
        evaluate_macro(state.macro_a, state.pre_pre_macro_move_a, state.use_zero_gain_pre_macro_a);
        evaluate_macro(state.macro_b);
        const MacroEval &a = eval_cache.find(search.cache_key(
            state.macro_a, state.pre_pre_macro_move_a, state.use_zero_gain_pre_macro_a))->second;
        const MacroEval &b = eval_cache.find(search.cache_key(state.macro_b, 0, false))->second;
        ll score = (ll)a.generation_cost + macro_b_generation_cost_from_a(state);
        rep(id, 1, in.M + 1) {
            int d = min(a.dist[id], b.dist[id]);
            if (d >= INF) return (1LL << 60);
            score += d;
        }
        return score;
    }

    void refresh_state(State &state) {
        if ((int)state.prefix_b.size() + (int)state.suffix_b.size() > CHANGE_MACRO_EXPERIMENT_MAX_AFFIX_LEN) {
            while ((int)state.prefix_b.size() + (int)state.suffix_b.size() > CHANGE_MACRO_EXPERIMENT_MAX_AFFIX_LEN) {
                if (!state.suffix_b.empty()) state.suffix_b.pop_back();
                else state.prefix_b.pop_back();
            }
        }
        state.macro_b = build_macro_b(state);
        state.score = evaluate_pair(state);
    }

    string random_macro_piece(int len) const {
        string res;
        rep(i, 0, len) res.push_back(search.random_macro_char());
        return res;
    }

    string &select_affix(State &state) const {
        return rnd(2) == 0 ? state.prefix_b : state.suffix_b;
    }

    bool mutate_affixes(State &state) {
        string old_prefix = state.prefix_b;
        string old_suffix = state.suffix_b;
        int total_len = (int)state.prefix_b.size() + (int)state.suffix_b.size();
        int type = rnd(6);
        if (type <= 2) {
            if (total_len >= CHANGE_MACRO_EXPERIMENT_MAX_AFFIX_LEN) return false;
            string &s = select_affix(state);
            char c = search.random_macro_char();
            if (&s == &state.prefix_b) {
                s.insert(s.begin(), c);
            } else {
                s.push_back(c);
            }
        } else if (type <= 4) {
            string &s = select_affix(state);
            if (s.empty()) return false;
            if (&s == &state.prefix_b) {
                s.erase(s.begin());
            } else {
                s.pop_back();
            }
        } else {
            string &s = select_affix(state);
            if (s.empty()) return false;
            int pos = rnd((int)s.size());
            s[pos] = search.random_macro_char();
        }
        if (state.prefix_b == old_prefix && state.suffix_b == old_suffix) return false;
        refresh_state(state);
        return true;
    }

    bool accept(ll diff, float temp) const {
        return diff <= 0 || log_rand_table() * temp < -diff;
    }

    void add_candidate(const State &state) {
        if (state.macro_b == state.macro_a) return;
        for (State &candidate : candidates) {
            if (candidate.macro_b == state.macro_b) {
                if (state.score < candidate.score) candidate = state;
                return;
            }
        }
        candidates.push_back(state);
    }

    vector<State> top_candidates(int limit) {
        sort(all(candidates), [](const State &a, const State &b) {
            if (a.score != b.score) return a.score < b.score;
            if (a.prefix_b.size() + a.suffix_b.size() != b.prefix_b.size() + b.suffix_b.size()) {
                return a.prefix_b.size() + a.suffix_b.size() < b.prefix_b.size() + b.suffix_b.size();
            }
            return a.macro_b < b.macro_b;
        });
        if ((int)candidates.size() > limit) candidates.resize(limit);
        return candidates;
    }

    void run(const string &opt_macro, char pre_pre_macro_move, bool use_zero_gain_pre_macro) {
        State state;
        state.macro_a = opt_macro;
        state.pre_pre_macro_move_a = pre_pre_macro_move;
        state.use_zero_gain_pre_macro_a = use_zero_gain_pre_macro;
        state.prefix_b = CHANGE_MACRO_EXPERIMENT_INITIAL_PREFIX;
        state.suffix_b = CHANGE_MACRO_EXPERIMENT_INITIAL_SUFFIX;
        refresh_state(state);
        add_candidate(state);

        int start_ms = timer.elapsed_ms();
        int loop_cnt = 0;
        while (timer.elapsed_ms() - start_ms < CHANGE_MACRO_EXPERIMENT_TIME_MS) {
            float progress = (float)(timer.elapsed_ms() - start_ms) / max(1, CHANGE_MACRO_EXPERIMENT_TIME_MS);
            float temp = CHANGE_MACRO_EXPERIMENT_START_TEMP
                       + (CHANGE_MACRO_EXPERIMENT_END_TEMP - CHANGE_MACRO_EXPERIMENT_START_TEMP) * min(1.0f, progress);
            State next = state;
            loop_cnt++;
            if (!mutate_affixes(next)) continue;
            add_candidate(next);
            ll diff = next.score - state.score;
            if (!accept(diff, temp)) continue;
            state = std::move(next);
        }

        cerr << Log::INDENT << "loop_cnt=" << loop_cnt << '\n';
    }
};

vector<ChangeMacroExperiment::State> run_change_macro_experiment(
    const Input &in, const MacroSearch &search, const MacroSearch::EvaluatedMacro &macro_a) {
    if (macro_a.macro.empty()) return {};
    ChangeMacroExperiment experiment(in, search);
    experiment.run(macro_a.macro, macro_a.pre_pre_macro_move, macro_a.use_zero_gain_pre_macro);
    auto candidates = experiment.top_candidates(TWO_MACRO_SOLVER_RUNS);
    return candidates;
}

struct TwoMacroSolver {
    const Input &in;
    MacroSolver solver_a;
    MacroSolver solver_b;
    string prefix_b;
    string suffix_b;
    int macro_b_from_a_cost = 0;
    vector<uint32_t> use_solver_a_threshold;

    using Node = MacroSolver::Node;

    struct State {
        vector<Node> order;
        vector<int> pos;
        int switch_pos = 1;
        int score = INF;
    };

    TwoMacroSolver(const Input &in_, const string &macro_a, char pre_pre_macro_move_a,
                   bool use_zero_gain_pre_macro_a, const string &prefix_b_, const string &suffix_b_)
        : in(in_),
          solver_a(in_, macro_a, pre_pre_macro_move_a, use_zero_gain_pre_macro_a),
          solver_b(in_, prefix_b_ + macro_a + suffix_b_),
          prefix_b(prefix_b_),
          suffix_b(suffix_b_) {
        macro_b_from_a_cost = prefix_b.empty() && suffix_b.empty()
            ? 0
            : (int)prefix_b.size() + (int)suffix_b.size() + 2;
        solver_a.use_exact_distance_table();
        solver_b.use_exact_distance_table();
        build_use_solver_a_prob();
    }

    void build_use_solver_a_prob() {
        vector<int> cost_a = solver_a.ball_to_box_button_distances();
        vector<int> cost_b = solver_b.ball_to_box_button_distances();
        use_solver_a_threshold.assign(in.M + 1, 1U << 31);
        rep(id, 1, in.M + 1) {
            int cost_diff = max(-30, min(30, cost_a[id] - cost_b[id]));
            float probability = 1.0f / (1.0f + exp2((float)cost_diff));
            
            use_solver_a_threshold[id] = probability >= 1.0f
                ? numeric_limits<uint32_t>::max()
                : (uint32_t)(probability * numeric_limits<uint32_t>::max());
        }
    }

    inline bool sample_use_solver_a(int id) const {
        return rnd() <= use_solver_a_threshold[id];
    }

    int state_of_ball(const Node &node) const {
        return solver_a.encode(solver_a.ball_cell[node.id], node.dir_ball);
    }

    int state_of_box(const Node &node) const {
        return solver_a.encode(solver_a.box_cell[node.id], node.dir_box);
    }

    int edge_cost_at(int i, const Node &prev, const Node &cur, int switch_pos) const {
        if (i < switch_pos) return solver_a.edge_cost_between_nodes(prev, cur);
        if (i > switch_pos) return solver_b.edge_cost_between_nodes(prev, cur);

        int reach = solver_a.distance_from(solver_a.state_of_box(prev), solver_a.state_of_ball(cur));
        int carry = solver_b.distance_from(solver_b.state_of_ball(cur), solver_b.state_of_box(cur));
        if (reach >= INF || carry >= INF) return INF;
        return reach + 1 + carry + macro_b_from_a_cost + 1;
    }

    struct EdgePositions {
        int v[32];
        int sz = 0;

        inline void add(int i, int n) {
            if (i <= 0 || i >= n) return;
            rep(k, 0, sz) if (v[k] == i) return;
            v[sz++] = i;
        }

        inline void add_around_node(int i, int n) {
            add(i, n);
            add(i + 1, n);
        }

    };

    inline int edge_regime(int edge_pos, int switch_pos) const {
        return edge_pos < switch_pos ? 0 : edge_pos == switch_pos ? 1 : 2;
    }

    void add_shifted_edges_with_changed_regime(
        EdgePositions &old_edges, EdgePositions &new_edges,
        int old_l, int old_r, int shift, int n, int switch_pos) const {
        if (old_l >= old_r || shift == 0) return;
        int check_l = max(old_l, min(switch_pos, switch_pos - shift) - 1);
        int check_r = min(old_r, max(switch_pos, switch_pos - shift) + 2);
        rep(i, check_l, check_r) {
            int new_i = i + shift;
            if (edge_regime(i, switch_pos) == edge_regime(new_i, switch_pos)) continue;
            old_edges.add(i, n);
            new_edges.add(new_i, n);
        }
    }

    void segment_move_edge_positions(
        int l, int len, int j, int n, int switch_pos,
        EdgePositions &old_edges, EdgePositions &new_edges) const {
        int r = l + len;
        if (j < l) {
            old_edges.add(j, n);
            old_edges.add(l, n);
            old_edges.add(r, n);
            new_edges.add(j, n);
            new_edges.add(j + len, n);
            new_edges.add(r, n);

            add_shifted_edges_with_changed_regime(
                old_edges, new_edges, l + 1, r, j - l, n, switch_pos);
            add_shifted_edges_with_changed_regime(
                old_edges, new_edges, j + 1, l, len, n, switch_pos);
        } else {
            old_edges.add(l, n);
            old_edges.add(r, n);
            old_edges.add(j + len, n);
            new_edges.add(l, n);
            new_edges.add(j, n);
            new_edges.add(j + len, n);

            add_shifted_edges_with_changed_regime(
                old_edges, new_edges, l + 1, r, j - l, n, switch_pos);
            add_shifted_edges_with_changed_regime(
                old_edges, new_edges, r + 1, j + len, -len, n, switch_pos);
        }
    }

    inline void rebuild_positions(State &state) const {
        state.pos.assign(in.M + 1, -1);
        rep(i, 0, (int)state.order.size()) state.pos[state.order[i].id] = i;
    }

    inline void evaluate_state(State &state) const {
        if ((int)state.pos.size() != in.M + 1) rebuild_positions(state);
        state.score = solver_a.initial_delivery_cost(state.order[0].dir_box);
        if (state.score >= INF) return;
        rep(i, 1, (int)state.order.size()) {
            int cost = edge_cost_at(i, state.order[i - 1], state.order[i], state.switch_pos);
            if (cost >= INF) {
                state.score = INF;
                return;
            }
            state.score += cost;
        }
        state.score += solver_a.macro_registration_extra_cost();
    }

    bool optimize_all_dirs_dp(State &state) const {
        int node_count = (int)state.order.size() - 1;
        if (node_count <= 0) return false;
        int old_score = state.score;
        int stage_count = node_count * 2;
        vector<array<int, 4>> dp(stage_count);
        vector<array<unsigned char, 4>> parent_dir(stage_count);
        for (auto &row : dp) row.fill(INF);

        int start_dir = state.order[0].dir_box;
        rep(i, 1, node_count + 1) {
            const Node &prev = state.order[i - 1];
            const Node &cur = state.order[i];
            int ball_stage = (i - 1) * 2;
            int box_stage = ball_stage + 1;
            const MacroSolver &reach_solver = i <= state.switch_pos ? solver_a : solver_b;
            const MacroSolver &carry_solver = i < state.switch_pos ? solver_a : solver_b;

            rep(dir_ball, 0, 4) {
                int best = INF;
                unsigned char best_prev_dir = 0;
                rep(prev_dir_box, 0, 4) {
                    int prev_cost = i == 1
                        ? (solver_a.has_picked_before_pre_macro()
                            ? solver_a.initial_delivery_cost(prev_dir_box)
                            : (prev_dir_box == start_dir ? 0 : INF))
                        : dp[ball_stage - 1][prev_dir_box];
                    if (prev_cost >= INF) continue;
                    int move_cost = reach_solver.distance_from(
                        reach_solver.encode(reach_solver.box_cell[prev.id], prev_dir_box),
                        reach_solver.encode(reach_solver.ball_cell[cur.id], dir_ball));
                    if (move_cost >= INF) continue;
                    int cost = prev_cost + move_cost + 1;
                    if (chmin(best, cost)) best_prev_dir = prev_dir_box;
                }
                dp[ball_stage][dir_ball] = best;
                parent_dir[ball_stage][dir_ball] = best_prev_dir;
            }

            rep(dir_box, 0, 4) {
                int best = INF;
                unsigned char best_dir_ball = 0;
                rep(dir_ball, 0, 4) {
                    int prev_cost = dp[ball_stage][dir_ball];
                    if (prev_cost >= INF) continue;
                    int move_cost = carry_solver.distance_from(
                        carry_solver.encode(carry_solver.ball_cell[cur.id], dir_ball),
                        carry_solver.encode(carry_solver.box_cell[cur.id], dir_box));
                    if (move_cost >= INF) continue;
                    int cost = prev_cost + move_cost + 1;
                    if (i == state.switch_pos) cost += macro_b_from_a_cost;
                    if (chmin(best, cost)) best_dir_ball = dir_ball;
                }
                dp[box_stage][dir_box] = best;
                parent_dir[box_stage][dir_box] = best_dir_ball;
            }
        }

        int best_path_score = INF;
        int dir = 0;
        rep(dir_box, 0, 4) {
            if (chmin(best_path_score, dp[stage_count - 1][dir_box])) dir = dir_box;
        }
        if (best_path_score >= INF) return false;

        bool changed = false;
        for (int i = node_count; i >= 1; i--) {
            int ball_stage = (i - 1) * 2;
            int box_stage = ball_stage + 1;
            changed |= state.order[i].dir_box != dir;
            state.order[i].dir_box = dir;
            dir = parent_dir[box_stage][dir];
            changed |= state.order[i].dir_ball != dir;
            state.order[i].dir_ball = dir;
            dir = parent_dir[ball_stage][dir];
        }
        if (solver_a.has_picked_before_pre_macro()) {
            changed |= state.order[0].dir_box != dir;
            state.order[0].dir_ball = state.order[0].dir_box = dir;
        }
        state.score = best_path_score + solver_a.macro_registration_extra_cost();
        assert(state.score <= old_score);
        return changed;
    }

    int computed_score(const State &state) const {
        State copy = state;
        evaluate_state(copy);
        return copy.score;
    }

    bool validate_delta_score(const State &state, const char *tag) const {
#ifdef DEBUG_TWO_MACRO_DELTA
        int actual = computed_score(state);
        if (actual != state.score) {
            cerr << "two macro delta mismatch: tag=" << tag
                 << ", delta_score=" << state.score
                 << ", actual_score=" << actual
                 << ", switch_pos=" << state.switch_pos << endl;
            return false;
        }
#endif
        return true;
    }

    inline int edge_cost(const State &state, int i) const {
        return edge_cost_at(i, state.order[i - 1], state.order[i], state.switch_pos);
    }

    inline int score_without_edges(const State &state, const EdgePositions &edges) const {
        if (state.score >= INF) return INF;
        int score = state.score;
        rep(k, 0, edges.sz) {
            int cost = edge_cost(state, edges.v[k]);
            if (cost >= INF) return INF;
            score -= cost;
        }
        return score;
    }

    inline void evaluate_delta_after_mutation(State &state, int score_without_old,
                                              const EdgePositions &new_edges) const {
        if (score_without_old >= INF) {
            evaluate_state(state);
            return;
        }
        int score = score_without_old;
        rep(k, 0, new_edges.sz) {
            int cost = edge_cost(state, new_edges.v[k]);
            if (cost >= INF) {
                state.score = INF;
                return;
            }
            score += cost;
        }
        state.score = score;
    }

    const Node &node_after_segment_move(
        const State &state, int l, int len, int j, int pos) const {
        if (j < l) {
            if (pos < j) return state.order[pos];
            if (pos < j + len) return state.order[l + pos - j];
            if (pos < l + len) return state.order[pos - len];
            return state.order[pos];
        }
        if (pos < l) return state.order[pos];
        if (pos < j) return state.order[pos + len];
        if (pos < j + len) return state.order[l + pos - j];
        return state.order[pos];
    }

    int score_after_segment_move(
        const State &state, int l, int len, int j,
        const EdgePositions &old_edges, const EdgePositions &new_edges) const {
        int score = score_without_edges(state, old_edges);
        if (score >= INF) {
            score = solver_a.macro_registration_extra_cost()
                  + solver_a.initial_delivery_cost(state.order[0].dir_box);
            rep(i, 1, (int)state.order.size()) {
                const Node &prev = node_after_segment_move(state, l, len, j, i - 1);
                const Node &cur = node_after_segment_move(state, l, len, j, i);
                int cost = edge_cost_at(i, prev, cur, state.switch_pos);
                if (cost >= INF) return INF;
                score += cost;
            }
            return score;
        }
        rep(k, 0, new_edges.sz) {
            int i = new_edges.v[k];
            const Node &prev = node_after_segment_move(state, l, len, j, i - 1);
            const Node &cur = node_after_segment_move(state, l, len, j, i);
            int cost = edge_cost_at(i, prev, cur, state.switch_pos);
            if (cost >= INF) return INF;
            score += cost;
        }
        return score;
    }

    inline void apply_move(State &state, int i, int j) const {
        Node node = state.order[i];
        if (i < j) {
            rep(k, i, j) state.order[k] = state.order[k + 1];
        } else {
            for (int k = i; k > j; k--) state.order[k] = state.order[k - 1];
        }
        state.order[j] = node;
        int from = min(i, j);
        int to = max(i, j);
        rep(k, from, to + 1) state.pos[state.order[k].id] = k;
    }

    inline void apply_segment_move(State &state, int l, int len, int j) const {
        array<Node, TWO_MACRO_OR_OPT_MAX_LEN> segment;
        rep(k, 0, len) segment[k] = state.order[l + k];
        if (j < l) {
            for (int p = l - 1; p >= j; p--) state.order[p + len] = state.order[p];
        } else {
            rep(p, l, j) state.order[p] = state.order[p + len];
        }
        rep(k, 0, len) state.order[j + k] = segment[k];
        int from = min(l, j);
        int to = max(l + len, j + len);
        rep(p, from, to) state.pos[state.order[p].id] = p;
    }

    Node best_node_from_a(int from_state, int id) const {
        return solver_a.best_node_from(from_state, id);
    }

    State initial_state() const {
        State state;
        auto base = solver_a.initial_state();
        state.order = base.order;
        int edge_count = (int)state.order.size() - 1;
        state.switch_pos = max(1, min(edge_count, edge_count / 2 + 1));
        rebuild_positions(state);
        evaluate_state(state);
        return state;
    }

    inline bool accept(int diff, float temp) const {
        return diff <= 0 || log_rand_table() * temp < -diff;
    }

    inline bool edge_uses_solver_a(int edge_pos, int switch_pos) const {
        return edge_pos <= switch_pos;
    }

    int sample_swap_position_by_alias(const State &state, int i) const {
        int n = state.order.size();
        int current_id = state.order[i].id;
        bool use_a = sample_use_solver_a(current_id);
        const MacroSolver &solver = use_a ? solver_a : solver_b;
        bool sample_after = rnd(2) == 0;
        const AliasSampler &sampler = sample_after
            ? solver.alias_after_to_id[current_id]
            : solver.alias_before_from_id[current_id];
        int sampled_id_offset = sample_after ? 0 : 1;
        int position_to_j_offset = sample_after ? 1 : -1;
        int j_to_edge_offset = sample_after ? 0 : 1;
        rep(_, 0, 4) {
            int sampled_id = sampler.sample(rnd) + sampled_id_offset;
            if (sampled_id == current_id) continue;
            int pos = state.pos[sampled_id];
            if (pos < 0) continue;
            int j = pos + position_to_j_offset;
            int edge_pos = j + j_to_edge_offset;
            if (j <= 0 || j >= n || j == i) continue;
            if (edge_uses_solver_a(edge_pos, state.switch_pos) != use_a) continue;
            return j;
        }
        return -1;
    }

    int sample_move_position_by_alias(const State &state, int i) const {
        int n = state.order.size();
        int current_id = state.order[i].id;
        bool use_a = sample_use_solver_a(current_id);
        const MacroSolver &solver = use_a ? solver_a : solver_b;
        bool sample_after = rnd(2) == 0;
        const AliasSampler &sampler = sample_after
            ? solver.alias_after_to_id[current_id]
            : solver.alias_before_from_id[current_id];
        int sampled_id_offset = sample_after ? 0 : 1;
        int insert_offset = sample_after ? 1 : 0;
        int edge_offset = sample_after ? 0 : 1;
        rep(_, 0, 4) {
            int sampled_id = sampler.sample(rnd) + sampled_id_offset;
            if (sampled_id == current_id) continue;
            int pos = state.pos[sampled_id];
            if (pos < 0) continue;
            int reduced_pos = pos < i ? pos : pos - 1;
            int j = reduced_pos + insert_offset;
            int edge_pos = j + edge_offset;
            if (j <= 0 || j >= n || j == i) continue;
            if (edge_uses_solver_a(edge_pos, state.switch_pos) != use_a) continue;
            return j;
        }
        return -1;
    }

    int sample_segment_insert_position_by_alias(const State &state, int l, int len) const {
        int n = state.order.size();
        int reduced_n = n - len;
        bool sample_after = rnd(2) == 0;
        int endpoint_id = sample_after ? state.order[l].id : state.order[l + len - 1].id;
        bool use_a = sample_use_solver_a(endpoint_id);
        const MacroSolver &solver = use_a ? solver_a : solver_b;
        const AliasSampler &sampler = sample_after
            ? solver.alias_after_to_id[endpoint_id]
            : solver.alias_before_from_id[endpoint_id];
        int sampled_id_offset = sample_after ? 0 : 1;
        int insert_offset = sample_after ? 1 : 0;
        int edge_offset = sample_after ? 0 : len;
        int max_j = sample_after ? reduced_n : reduced_n - 1;
        rep(_, 0, 4) {
            int sampled_id = sampler.sample(rnd) + sampled_id_offset;
            int pos = state.pos[sampled_id];
            if (pos < 0 || (l <= pos && pos < l + len)) continue;
            int reduced_pos = pos < l ? pos : pos - len;
            int j = reduced_pos + insert_offset;
            int edge_pos = j + edge_offset;
            if (j == l || j < 1 || j > max_j) continue;
            if (edge_uses_solver_a(edge_pos, state.switch_pos) != use_a) continue;
            return j;
        }
        return -1;
    }

    int sample_segment_insert_position_in_range_by_alias(
        const State &state, int l, int len, int range_l, int range_r,
        const MacroSolver &solver) const {
        bool sample_after = rnd(2) == 0;
        int endpoint_id = sample_after ? state.order[l].id : state.order[l + len - 1].id;
        const AliasSampler &sampler = sample_after
            ? solver.alias_after_to_id[endpoint_id]
            : solver.alias_before_from_id[endpoint_id];
        int sampled_id_offset = sample_after ? 0 : 1;
        int insert_offset = sample_after ? 1 : 0;
        rep(_, 0, 4) {
            int sampled_id = sampler.sample(rnd) + sampled_id_offset;
            int pos = state.pos[sampled_id];
            if (pos < range_l || pos >= range_r || (l <= pos && pos < l + len)) continue;
            int reduced_pos = pos < l ? pos : pos - len;
            int j = reduced_pos + insert_offset;
            if (j == l || j < range_l || j + len > range_r) continue;
            return j;
        }
        return -1;
    }

    int sample_transfer_insert_position(
        const State &state, int i, bool from_a, int new_switch_pos) const {
        int n = state.order.size();
        const MacroSolver &solver = from_a ? solver_b : solver_a;
        int current_id = state.order[i].id;
        bool sample_after = rnd(2) == 0;
        const AliasSampler &sampler = sample_after
            ? solver.alias_after_to_id[current_id]
            : solver.alias_before_from_id[current_id];
        int sampled_id_offset = sample_after ? 0 : 1;
        int insert_offset = sample_after ? 1 : 0;
        rep(_, 0, 8) {
            int sampled_id = sampler.sample(rnd) + sampled_id_offset;
            int pos = state.pos[sampled_id];
            if (pos <= 0 || pos == i) continue;
            bool target_is_a = pos < state.switch_pos;
            if (target_is_a == from_a) continue;
            int reduced_pos = pos < i ? pos : pos - 1;
            int j = reduced_pos + insert_offset;
            if (j <= 0 || j >= n || j == i) continue;
            bool moved_node_is_a = j < new_switch_pos;
            if (moved_node_is_a == from_a) continue;
            return j;
        }
        return -1;
    }

    bool change_node_dir(State &state, float temp) const {
        int n = state.order.size();
        int i = rnd(1, n);
        EdgePositions edges;
        edges.add_around_node(i, n);
        int old_score = state.score;
        int score_without_old = score_without_edges(state, edges);
        Node original = state.order[i];
        int type = rnd(3);
        if (type == 0 || type == 2) state.order[i].dir_ball = rnd(4);
        if (type == 1 || type == 2) state.order[i].dir_box = rnd(4);
        evaluate_delta_after_mutation(state, score_without_old, edges);
        int diff = state.score - old_score;
        if (!accept(diff, temp)) {
            state.order[i] = original;
            state.score = old_score;
            return false;
        }
        validate_delta_score(state, "dir");
        return true;
    }

    bool swap_order(State &state, float temp) const {
        int n = state.order.size();
        int i = rnd(1, n);
        int j = sample_swap_position_by_alias(state, i);
        if (j == -1) return false;
        EdgePositions edges;
        edges.add_around_node(i, n);
        edges.add_around_node(j, n);
        int old_score = state.score;
        int score_without_old = score_without_edges(state, edges);
        swap(state.order[i], state.order[j]);
        evaluate_delta_after_mutation(state, score_without_old, edges);
        int diff = state.score - old_score;
        if (!accept(diff, temp)) {
            swap(state.order[i], state.order[j]);
            state.score = old_score;
            return false;
        }
        state.pos[state.order[i].id] = i;
        state.pos[state.order[j].id] = j;
        validate_delta_score(state, "swap");
        return true;
    }

    bool move_order(State &state, float temp, bool force_accept = false) const {
        int n = state.order.size();
        int i = rnd(1, n);
        int j = sample_move_position_by_alias(state, i);
        if (j == -1) return false;
        EdgePositions old_edges;
        EdgePositions new_edges;
        segment_move_edge_positions(i, 1, j, n, state.switch_pos, old_edges, new_edges);
        int old_score = state.score;
        int new_score = score_after_segment_move(state, i, 1, j, old_edges, new_edges);
        int diff = new_score - old_score;
        if (!force_accept && !accept(diff, temp)) return false;
        apply_move(state, i, j);
        state.score = new_score;
        validate_delta_score(state, force_accept ? "kick-move" : "move");
        return true;
    }

    bool oropt_order(State &state, float temp) const {
        int n = state.order.size();
        int max_len = min(TWO_MACRO_OR_OPT_MAX_LEN, n - 2);
        if (max_len < 2) return false;
        int len = rnd(2, max_len + 1);
        int l = rnd(1, n - len + 1);
        int j = sample_segment_insert_position_by_alias(state, l, len);
        if (j == -1) return false;
        EdgePositions old_edges;
        EdgePositions new_edges;
        segment_move_edge_positions(l, len, j, n, state.switch_pos, old_edges, new_edges);
        int old_score = state.score;
        int new_score = score_after_segment_move(state, l, len, j, old_edges, new_edges);
        int diff = new_score - old_score;
        if (!accept(diff, temp)) return false;
        apply_segment_move(state, l, len, j);
        state.score = new_score;
        validate_delta_score(state, "or-opt");
        return true;
    }

    bool oropt_order_within_macro(State &state, float temp) const {
        int n = state.order.size();
        bool use_a = rnd(2) == 0;
        int range_l = use_a ? 1 : state.switch_pos;
        int range_r = use_a ? state.switch_pos : n;
        int max_len = min(TWO_MACRO_OR_OPT_MAX_LEN, range_r - range_l - 1);
        if (max_len < 2) return false;
        int len = rnd(2, max_len + 1);
        int l = rnd(range_l, range_r - len + 1);
        const MacroSolver &solver = use_a ? solver_a : solver_b;
        int j = sample_segment_insert_position_in_range_by_alias(
            state, l, len, range_l, range_r, solver);
        if (j == -1) return false;
        EdgePositions old_edges;
        EdgePositions new_edges;
        segment_move_edge_positions(l, len, j, n, state.switch_pos, old_edges, new_edges);
        int old_score = state.score;
        int new_score = score_after_segment_move(state, l, len, j, old_edges, new_edges);
        int diff = new_score - old_score;
        if (!accept(diff, temp)) return false;
        apply_segment_move(state, l, len, j);
        state.score = new_score;
        validate_delta_score(state, "within-macro-or-opt");
        return true;
    }

    bool change_switch_pos(State &state, float temp) const {
        int n = state.order.size();
        int edge_count = n - 1;
        if (edge_count <= 1) return false;
        int old_switch_pos = state.switch_pos;
        int new_switch_pos;
        if (rnd(2) == 0) {
            new_switch_pos = old_switch_pos + rnd(3) - 1;
        } else {
            new_switch_pos = rnd(1, edge_count + 1);
        }
        new_switch_pos = max(1, min(edge_count, new_switch_pos));
        if (new_switch_pos == old_switch_pos) return false;

        int old_score = state.score;
        if (old_score >= INF) {
            state.switch_pos = new_switch_pos;
            evaluate_state(state);
        } else {
            int score = old_score;
            int l = min(old_switch_pos, new_switch_pos);
            int r = max(old_switch_pos, new_switch_pos);
            rep(i, l, r + 1) {
                int cost = edge_cost_at(i, state.order[i - 1], state.order[i], old_switch_pos);
                if (cost >= INF) {
                    score = INF;
                    break;
                }
                score -= cost;
            }
            if (score < INF) {
                rep(i, l, r + 1) {
                    int cost = edge_cost_at(i, state.order[i - 1], state.order[i], new_switch_pos);
                    if (cost >= INF) {
                        score = INF;
                        break;
                    }
                    score += cost;
                }
            }
            state.switch_pos = new_switch_pos;
            state.score = score;
        }
        int diff = state.score - old_score;
        if (!accept(diff, temp)) {
            state.switch_pos = old_switch_pos;
            state.score = old_score;
            return false;
        }
        validate_delta_score(state, "switch");
        return true;
    }

    bool transfer_move_order(State &state, float temp) const {
        int n = state.order.size();
        bool from_a = rnd(2) == 0;
        int source_l = from_a ? 1 : state.switch_pos;
        int source_r = from_a ? state.switch_pos : n;
        if (source_l >= source_r) return false;

        int i = rnd(source_l, source_r);
        int new_switch_pos = state.switch_pos + (from_a ? -1 : 1);
        if (new_switch_pos < 1 || new_switch_pos >= (int)state.order.size()) return false;
        int j = sample_transfer_insert_position(state, i, from_a, new_switch_pos);
        if (j == -1) return false;

        State original = state;
        apply_move(state, i, j);
        state.switch_pos = new_switch_pos;
        evaluate_state(state);
        int diff = state.score - original.score;
        if (!accept(diff, temp)) {
            state = std::move(original);
            return false;
        }
        validate_delta_score(state, "transfer-move");
        return true;
    }

    bool cross_exchange_within_macro(State &state, float temp) const {
        int n = state.order.size();
        bool use_a = rnd(2) == 0;
        int range_l = use_a ? 1 : state.switch_pos;
        int range_r = use_a ? state.switch_pos : n;
        int range_len = range_r - range_l;
        if (range_len < 2) return false;

        int len1 = rnd(1, min(TWO_MACRO_CROSS_EXCHANGE_MAX_LEN, range_len - 1) + 1);
        int l1 = rnd(range_l, range_r - len1 + 1);
        int remaining = range_len - len1;
        int len2 = rnd(1, min(TWO_MACRO_CROSS_EXCHANGE_MAX_LEN, remaining) + 1);
        int left_count = max(0, l1 - range_l - len2 + 1);
        int right_count = max(0, range_r - (l1 + len1) - len2 + 1);
        if (left_count + right_count == 0) return false;
        int l2;
        int side_offset = rnd(left_count + right_count);
        if (side_offset < left_count) {
            l2 = range_l + side_offset;
        } else {
            l2 = l1 + len1 + side_offset - left_count;
        }
        if (l2 < l1) {
            swap(l1, l2);
            swap(len1, len2);
        }
        int r1 = l1 + len1;
        int r2 = l2 + len2;
        if (r1 > l2) return false;

        State original = state;
        rotate(state.order.begin() + l1, state.order.begin() + r1, state.order.begin() + r2);
        int middle_len = l2 - r1;
        rotate(state.order.begin() + l1,
               state.order.begin() + l1 + middle_len,
               state.order.begin() + l1 + middle_len + len2);
        rebuild_positions(state);
        evaluate_state(state);
        int diff = state.score - original.score;
        if (!accept(diff, temp)) {
            state = std::move(original);
            return false;
        }
        validate_delta_score(state, "within-macro-cross-exchange");
        return true;
    }

    bool mutate(State &state, float temp) const {
        constexpr int neighbor_count = sizeof(TWO_MACRO_NEIGHBOR_WEIGHTS)
                                     / sizeof(TWO_MACRO_NEIGHBOR_WEIGHTS[0]);
        int total_weight = 0;
        rep(type, 0, neighbor_count) total_weight += TWO_MACRO_NEIGHBOR_WEIGHTS[type];
        int selected = rnd(total_weight);
        int type = 0;
        while (selected >= TWO_MACRO_NEIGHBOR_WEIGHTS[type]) {
            selected -= TWO_MACRO_NEIGHBOR_WEIGHTS[type++];
        }
        switch (type) {
            case 0: return change_node_dir(state, temp);
            case 1: return swap_order(state, temp);
            case 2: return move_order(state, temp);
            case 3: return oropt_order(state, temp);
            case 4: return oropt_order_within_macro(state, temp);
            case 5: return change_switch_pos(state, temp);
            case 6: return transfer_move_order(state, temp);
            case 7: return cross_exchange_within_macro(state, temp);
            default: assert(false);
        }
        return false;
    }

    State anneal(State state, int budget_ms, float kick_stagnation_progress) const {
        State best = state;
        timer.mark_lap();
        long long budget_us = max(1, budget_ms) * 1000LL;
        int loop_cnt = 0;
        int accept_cnt = 0;
        int kick_cnt = 0;
        float last_best_progress = 0.0f;
        int local_best_score = state.score;
        float last_local_best_progress = 0.0f;
        while (true) {
            long long elapsed_us = timer.lap_us();
            if (elapsed_us >= budget_us) break;
            float progress = (float)elapsed_us / budget_us;

            if (progress - last_local_best_progress >= kick_stagnation_progress) {
                int moved = 0;
                rep(_, 0, TWO_MACRO_KICK_MAX_TRIALS) {
                    if (!move_order(state, 0.0f, true)) continue;
                    moved++;
                    if (moved >= TWO_MACRO_KICK_MOVE_COUNT) break;
                }
                kick_cnt++;
                local_best_score = state.score;
                last_local_best_progress = progress;
            }

            // 指数
            float temp = TWO_MACRO_SOLVER_START_TEMP
                       * pow(TWO_MACRO_SOLVER_END_TEMP / TWO_MACRO_SOLVER_START_TEMP,
                             min(1.0f, progress));
            // 線形
            // float temp = TWO_MACRO_SOLVER_START_TEMP + (TWO_MACRO_SOLVER_END_TEMP - TWO_MACRO_SOLVER_START_TEMP) * min(1.0f, progress);
            rep(_, 0, 512){
                loop_cnt++;
                if (!mutate(state, temp)) continue;
                accept_cnt++;
                if (state.score < local_best_score) {
                    local_best_score = state.score;
                    last_local_best_progress = min(1.0f, progress);
                }
                if (state.score < best.score){
                    best = state;
                    last_best_progress = min(1.0f, progress);
                }
            }
        }
        optimize_all_dirs_dp(best);
        cerr << Log::INDENT2 << "done: loop_cnt=" << loop_cnt
             << ", accept_rate=" << Log::percent(accept_cnt, loop_cnt)
             << ", last_best_progress=" << Log::percent(
                    (long long)ceil(last_best_progress * 1000.0f), 1000)
             << ", kick_cnt=" << kick_cnt
             << ", score=" << best.score
             << '\n';
        return best;
    }

    State anneal(int budget_ms) const {
        return anneal(initial_state(), budget_ms, TWO_MACRO_FIRST_STAGE_KICK_STAGNATION_PROGRESS);
    }

    string path_buttons_b(int src, int dst, int &expanded_basic,
                          bool &macro_b_registered, bool &macro_a_registered) const {
        string res;
        if ((int)solver_b.restore_parent.size() != solver_b.states) {
            solver_b.restore_parent.resize(solver_b.states);
            solver_b.restore_action.resize(solver_b.states);
            solver_b.restore_q.resize(solver_b.states);
        }
        fill(all(solver_b.restore_parent), INVALID_STATE);
        fill(all(solver_b.restore_action), 255);

        int head = 0, tail = 0;
        solver_b.restore_parent[src] = src;
        solver_b.restore_q[tail++] = src;
        while (head < tail && solver_b.restore_parent[dst] == INVALID_STATE) {
            int s = solver_b.restore_q[head++];
            rep(a, 0, 4) {
                int ns = solver_b.graph_to[s][a];
                if (solver_b.restore_parent[ns] != INVALID_STATE) continue;
                solver_b.restore_parent[ns] = s;
                solver_b.restore_action[ns] = a;
                solver_b.restore_q[tail++] = ns;
                if (ns == dst) break;
            }
        }

        vector<int> actions;
        int cur = dst;
        while (cur != src && solver_b.restore_parent[cur] != INVALID_STATE) {
            int a = solver_b.restore_action[cur];
            if (a < 0 || a >= 4) break;
            actions.push_back(a);
            cur = solver_b.restore_parent[cur];
        }
        reverse(all(actions));

        for (int a : actions) {
            if (a == 3 && !macro_b_registered) {
                if (macro_b_from_a_cost > 0 && macro_a_registered) {
                    res.push_back('M');
                    res += prefix_b;
                    res.push_back('P');
                    res += suffix_b;
                    res.push_back('M');
                } else {
                    res.push_back('M');
                    res += solver_b.macro;
                    res.push_back('M');
                }
                macro_b_registered = true;
            } else {
                res.push_back(ACTION_CHARS[a]);
            }
            expanded_basic += (a == 3 ? (int)solver_b.macro.size() : 1);
            if (a == 3 && !macro_a_registered && macro_b_from_a_cost > 0) {
                // Direct macroB registration does not make macroA available as P.
            }
        }
        return res;
    }

    pair<string, int> restore(const State &state) const {
        string out;
        int expanded_basic = solver_a.pre_macro.size() + (solver_a.has_pre_pre_macro_move() ? 1 : 0)
                           + (solver_a.has_picked_before_pre_macro() ? 1 : 0);
        if (solver_a.has_picked_before_pre_macro()) {
            out.push_back('S');
        }
        if (solver_a.has_pre_pre_macro_move()) {
            out.push_back(solver_a.pre_pre_macro_move);
        }
        if (!solver_a.pre_macro.empty()) {
            out.push_back('M');
            out += solver_a.pre_macro;
            out.push_back('M');
        }
        bool macro_a_registered = false;
        bool macro_b_registered = false;
        if (solver_a.has_picked_before_pre_macro()) {
            out += solver_a.path_buttons(solver_a.start_state, solver_a.state_of_box(state.order[0]),
                                         expanded_basic, macro_a_registered);
            out.push_back('S');
            expanded_basic++;
        }
        rep(i, 1, (int)state.order.size()) {
            const Node &prev = state.order[i - 1];
            const Node &cur = state.order[i];
            if (i < state.switch_pos) {
                out += solver_a.path_buttons(solver_a.state_of_box(prev), solver_a.state_of_ball(cur),
                                             expanded_basic, macro_a_registered);
                out.push_back('S');
                expanded_basic++;
                out += solver_a.path_buttons(solver_a.state_of_ball(cur), solver_a.state_of_box(cur),
                                             expanded_basic, macro_a_registered);
                out.push_back('S');
                expanded_basic++;
            } else if (i == state.switch_pos) {
                out += solver_a.path_buttons(solver_a.state_of_box(prev), solver_a.state_of_ball(cur),
                                             expanded_basic, macro_a_registered);
                out.push_back('S');
                expanded_basic++;
                if (macro_b_from_a_cost == 0) macro_b_registered = macro_a_registered;
                out += path_buttons_b(solver_b.state_of_ball(cur), solver_b.state_of_box(cur),
                                      expanded_basic, macro_b_registered, macro_a_registered);
                out.push_back('S');
                expanded_basic++;
            } else {
                out += path_buttons_b(solver_b.state_of_box(prev), solver_b.state_of_ball(cur),
                                      expanded_basic, macro_b_registered, macro_a_registered);
                out.push_back('S');
                expanded_basic++;
                out += path_buttons_b(solver_b.state_of_ball(cur), solver_b.state_of_box(cur),
                                      expanded_basic, macro_b_registered, macro_a_registered);
                out.push_back('S');
                expanded_basic++;
            }
        }
        return {out, expanded_basic};
    }
};

int main(int argc, char *argv[]) {
#if DEBUG || PROFILER
    freopen("tools/in/0000.txt", "r", stdin);
#endif
    Input in;
    in.read();
    // if(in.wall_ratio < 0.01 || in.wall_ratio > 0.05){
    //     exit(-1);
    // }

    int macro_search_time_ms = MACRO_SEARCH_TIME_MS;
    if (argc >= 2) macro_search_time_ms = atoi(argv[1]);
    bool enable_change_macro_experiment = ENABLE_CHANGE_MACRO_EXPERIMENT;
    rep(arg_idx, 2, argc) {
        string arg = argv[arg_idx];
        if (arg == "--change-macro-experiment") enable_change_macro_experiment = true;
    }

    MacroSearch search(in);
    auto best = search.run(macro_search_time_ms);

    vector<MacroSearch::EvaluatedMacro> final_candidates = search.history;
    final_candidates.push_back({best.macro, best.pre_pre_macro_move, best.use_zero_gain_pre_macro,
                                best.score, best.buttons, best.expanded_basic, best.correct});
    for (auto &candidate : final_candidates) {
        candidate.macro = search.normalize_macro_for_downstream(std::move(candidate.macro));
        candidate.use_zero_gain_pre_macro =
            search.canonical_use_zero_gain_pre_macro(candidate.macro, candidate.use_zero_gain_pre_macro);
    }
    ll final_candidate_best_score = best.score;
    for (const auto &candidate : final_candidates) chmin(final_candidate_best_score, candidate.score);
    sort(all(final_candidates), [](const MacroSearch::EvaluatedMacro &a, const MacroSearch::EvaluatedMacro &b) {
        if (a.score != b.score) return a.score < b.score;
        if (a.buttons != b.buttons) return a.buttons < b.buttons;
        return a.expanded_basic < b.expanded_basic;
    });
    vector<MacroSearch::EvaluatedMacro> best_macros;
    unordered_set<string> used_final_macros;
    double final_candidate_score_limit = (double)final_candidate_best_score * FINAL_SOLVER_SCORE_MARGIN;
    for (const auto &candidate : final_candidates) {
        if ((double)candidate.score > final_candidate_score_limit) break;
        string key = search.cache_key(candidate.macro, candidate.pre_pre_macro_move, candidate.use_zero_gain_pre_macro);
        if (used_final_macros.insert(key).second) {
            best_macros.push_back(candidate);
        }
    }
    if (best_macros.empty()) {
        string final_macro = search.normalize_macro_for_downstream(best.macro);
        bool final_use_zero_gain_pre_macro =
            search.canonical_use_zero_gain_pre_macro(final_macro, best.use_zero_gain_pre_macro);
        best_macros.push_back({std::move(final_macro), best.pre_pre_macro_move, final_use_zero_gain_pre_macro,
                               best.score, best.buttons, best.expanded_basic, best.correct});
    }
    vector<MacroSearch::EvaluatedMacro> final_macros;
    final_macros.reserve(FINAL_SOLVER_SPLIT);
    rep(i, 0, FINAL_SOLVER_SPLIT) {
        final_macros.push_back(best_macros[i % (int)best_macros.size()]);
    }
    vector<MacroSearch::EvaluatedMacro> two_macro_a_pool = final_candidates;

    int after_final_reserved_ms = 0;
    if (enable_change_macro_experiment) {
        after_final_reserved_ms += CHANGE_MACRO_EXPERIMENT_TOTAL_TIME_MS;
        after_final_reserved_ms += TWO_MACRO_SOLVER_TOTAL_TIME_MS;
    }
    int final_deadline_ms = max(0, (int)time_limit - after_final_reserved_ms);
    Log::section("FinalSolver");
    cerr << Log::INDENT
         << left << setw(12) << "trial"
         << right << setw(8) << "score"
         << setw(12) << "operations"
         << "  " << left << setw(28) << "macro"
         << setw(10) << "pre_pre"
         << "zero_pre\n";
    auto log_final_row = [&](const string &label) {
        cerr << Log::INDENT
             << left << setw(12) << label
             << right << setw(8) << best.score
             << setw(12) << best.expanded_basic
             << "  " << left << setw(28) << Log::macro_rle(best.macro)
             << setw(10) << MacroSearch::pre_pre_macro_move_name(best.pre_pre_macro_move)
             << MacroSearch::bool_name(best.use_zero_gain_pre_macro) << '\n';
    };
    int final_best_trial = 0;
    unordered_map<string, unique_ptr<MacroSolver>> final_solver_cache;
    rep(i, 0, (int)final_macros.size()) {
        int remaining_ms = max(0, final_deadline_ms - timer.elapsed_ms());
        int runs_left = (int)final_macros.size() - i;
        if (remaining_ms <= 0) {
            break;
        }
        int slot_ms = max(1, remaining_ms / runs_left);
        int slot_deadline_ms = timer.elapsed_ms() + slot_ms;
        const auto &final_candidate = final_macros[i];
        const string &final_macro = final_candidate.macro;
        char final_pre_pre_macro_move = final_candidate.pre_pre_macro_move;
        bool final_use_zero_gain_pre_macro = final_candidate.use_zero_gain_pre_macro;
        string final_key = search.cache_key(final_macro, final_pre_pre_macro_move, final_use_zero_gain_pre_macro);
        auto it = final_solver_cache.find(final_key);
        if (it == final_solver_cache.end()) {
            auto solver = make_unique<MacroSolver>(in, final_macro, final_pre_pre_macro_move, final_use_zero_gain_pre_macro);
            solver->use_exact_distance_table();
            it = final_solver_cache.emplace(final_key, std::move(solver)).first;
        }
        MacroSolver &final_solver = *it->second;
        int budget_ms = max(0, slot_deadline_ms - timer.elapsed_ms());
        if (budget_ms <= 0) continue;
        auto final_state = final_solver.anneal(budget_ms, false, FINAL_INNER_SA_START_TEMP_MULTI);
        auto [out, expanded] = final_solver.restore(final_state);
        auto sim = compute_raw_score(in, out);
        two_macro_a_pool.push_back({final_macro, final_pre_pre_macro_move, final_use_zero_gain_pre_macro,
                                    sim.score, (int)out.size(), sim.expanded_basic, sim.correct});
        if (sim.score < best.score) {
            best.macro = final_macro;
            best.pre_pre_macro_move = final_pre_pre_macro_move;
            best.use_zero_gain_pre_macro = final_use_zero_gain_pre_macro;
            best.output = std::move(out);
            best.score = sim.score;
            best.buttons = best.output.size();
            best.expanded_basic = sim.expanded_basic;
            best.correct = sim.correct;
            final_best_trial = i + 1;
            log_final_row(to_string(i + 1) + "/" + to_string(FINAL_SOLVER_SPLIT));
        }
    }
    cerr << '\n' << Log::INDENT << "Best\n";
    log_final_row(final_best_trial == 0
        ? "initial"
        : to_string(final_best_trial) + "/" + to_string(FINAL_SOLVER_SPLIT));
    ll final_solver_best_score = best.score;
    bool best_is_two_macro = false;
    TwoMacroSolver::State best_two_macro_state;

    if (ENABLE_INNER_SCORE_EXPERIMENT) {
        run_inner_score_experiment(in, search.history);
    }

    if (enable_change_macro_experiment) {
        string downstream_best_macro = search.normalize_macro_for_downstream(best.macro);
        bool downstream_best_use_zero_gain_pre_macro =
            search.canonical_use_zero_gain_pre_macro(downstream_best_macro, best.use_zero_gain_pre_macro);
        two_macro_a_pool.push_back({std::move(downstream_best_macro), best.pre_pre_macro_move,
                                    downstream_best_use_zero_gain_pre_macro,
                                    best.score, best.buttons, best.expanded_basic, best.correct});
        sort(all(two_macro_a_pool), [](const MacroSearch::EvaluatedMacro &a, const MacroSearch::EvaluatedMacro &b) {
            if (a.score != b.score) return a.score < b.score;
            if (a.buttons != b.buttons) return a.buttons < b.buttons;
            return a.expanded_basic < b.expanded_basic;
        });
        vector<MacroSearch::EvaluatedMacro> two_macro_a_candidates;
        two_macro_a_candidates.reserve(TWO_MACRO_SOLVER_MACRO_A_CANDIDATES);
        unordered_set<string> used_macro_a;
        for (const auto &candidate : two_macro_a_pool) {
            string key = search.cache_key(
                candidate.macro, candidate.pre_pre_macro_move, candidate.use_zero_gain_pre_macro);
            if (!used_macro_a.insert(key).second) continue;
            two_macro_a_candidates.push_back(candidate);
            if ((int)two_macro_a_candidates.size() >= TWO_MACRO_SOLVER_MACRO_A_CANDIDATES) break;
        }

        Log::section("change macro experiment");
        vector<ChangeMacroExperiment::State> macro_pairs;
        for (const auto &macro_a_candidate : two_macro_a_candidates) {
            auto candidates = run_change_macro_experiment(in, search, macro_a_candidate);
            macro_pairs.insert(macro_pairs.end(), all(candidates));
        }
        sort(all(macro_pairs), [](const ChangeMacroExperiment::State &a, const ChangeMacroExperiment::State &b) {
            if (a.score != b.score) return a.score < b.score;
            if (a.prefix_b.size() + a.suffix_b.size() != b.prefix_b.size() + b.suffix_b.size()) {
                return a.prefix_b.size() + a.suffix_b.size() < b.prefix_b.size() + b.suffix_b.size();
            }
            if (a.macro_a != b.macro_a) return a.macro_a < b.macro_a;
            return a.macro_b < b.macro_b;
        });
        vector<ChangeMacroExperiment::State> selected_macro_pairs;
        selected_macro_pairs.reserve(TWO_MACRO_SOLVER_RUNS);
        unordered_set<string> used_macro_pair;
        for (const auto &macro_pair : macro_pairs) {
            string key = search.cache_key(macro_pair.macro_a, macro_pair.pre_pre_macro_move_a,
                                          macro_pair.use_zero_gain_pre_macro_a);
            key.push_back('\n');
            key += macro_pair.macro_b;
            if (!used_macro_pair.insert(key).second) continue;
            selected_macro_pairs.push_back(macro_pair);
            if ((int)selected_macro_pairs.size() >= TWO_MACRO_SOLVER_RUNS) break;
        }
        macro_pairs = std::move(selected_macro_pairs);

        if (!macro_pairs.empty()) {
            Log::section("two macro solver");
            array<ll, TWO_MACRO_SOLVER_RUNS> two_scores;
            two_scores.fill(INF);
            int two_macro_runs = min(TWO_MACRO_SOLVER_RUNS, (int)macro_pairs.size());
            vector<TwoMacroSolver::State> two_states(two_macro_runs);
            vector<string> two_outputs(two_macro_runs);
            vector<int> two_expanded_basic(two_macro_runs, 0);
            vector<int> two_correct(two_macro_runs, 0);
            vector<int> completed_runs;
            completed_runs.reserve(two_macro_runs);

            cerr << Log::INDENT << "first stage\n";
            rep(run_idx, 0, two_macro_runs) {
                int remaining_ms = max(0, (int)time_limit - TWO_MACRO_SOLVER_BATCH_GUARD_MS - timer.elapsed_ms());
                int runs_left = two_macro_runs - run_idx;
                if (remaining_ms <= 0) break;
                int slot_ms = min(TWO_MACRO_SOLVER_TIME_MS, max(1, remaining_ms / runs_left));
                int slot_deadline_ms = timer.elapsed_ms() + slot_ms;
                const auto &macro_pair = macro_pairs[run_idx];
                cerr << Log::INDENT << '[' << (run_idx + 1) << '/' << two_macro_runs << "] "
                     << Log::macro_rle(macro_pair.prefix_b) << " | "
                     << Log::macro_rle(macro_pair.macro_a) << " | "
                     << Log::macro_rle(macro_pair.suffix_b)
                     << ", pre_pre=" << MacroSearch::pre_pre_macro_move_name(macro_pair.pre_pre_macro_move_a)
                     << ", zero_pre=" << MacroSearch::bool_name(macro_pair.use_zero_gain_pre_macro_a) << '\n';
                TwoMacroSolver two_solver(in, macro_pair.macro_a, macro_pair.pre_pre_macro_move_a,
                                          macro_pair.use_zero_gain_pre_macro_a,
                                          macro_pair.prefix_b, macro_pair.suffix_b);
                int budget_ms = max(0, slot_deadline_ms - timer.elapsed_ms());
                if (budget_ms <= 0) break;
                two_states[run_idx] = two_solver.anneal(budget_ms);
                auto [two_out, two_expanded] = two_solver.restore(two_states[run_idx]);
                auto two_sim = compute_raw_score(in, two_out);
                two_scores[run_idx] = two_sim.score;
                two_outputs[run_idx] = std::move(two_out);
                two_expanded_basic[run_idx] = two_sim.expanded_basic;
                two_correct[run_idx] = two_sim.correct;
                completed_runs.push_back(run_idx);
            }

            sort(all(completed_runs), [&](int a, int b) {
                return two_scores[a] < two_scores[b];
            });
            int finalist_count = min(TWO_MACRO_SOLVER_FINALISTS, (int)completed_runs.size());
            cerr << Log::INDENT << "final stage\n";
            rep(finalist_idx, 0, finalist_count) {
                int run_idx = completed_runs[finalist_idx];
                int remaining_ms = max(0, (int)time_limit - TWO_MACRO_SOLVER_BATCH_GUARD_MS - timer.elapsed_ms());
                int runs_left = finalist_count - finalist_idx;
                if (remaining_ms <= 0) break;
                int slot_ms = min(TWO_MACRO_SOLVER_FINAL_TIME_MS, max(1, remaining_ms / runs_left));
                int slot_deadline_ms = timer.elapsed_ms() + slot_ms;
                const auto &macro_pair = macro_pairs[run_idx];
                cerr << Log::INDENT << '[' << (finalist_idx + 1) << '/' << finalist_count
                     << "] trial=" << (run_idx + 1) << ' '
                     << Log::macro_rle(macro_pair.prefix_b) << " | "
                     << Log::macro_rle(macro_pair.macro_a) << " | "
                     << Log::macro_rle(macro_pair.suffix_b) << '\n';
                TwoMacroSolver two_solver(in, macro_pair.macro_a, macro_pair.pre_pre_macro_move_a,
                                          macro_pair.use_zero_gain_pre_macro_a,
                                          macro_pair.prefix_b, macro_pair.suffix_b);
                int budget_ms = max(0, slot_deadline_ms - timer.elapsed_ms());
                if (budget_ms <= 0) break;
                two_states[run_idx] = two_solver.anneal(
                    std::move(two_states[run_idx]), budget_ms,
                    TWO_MACRO_FINAL_STAGE_KICK_STAGNATION_PROGRESS);
                auto [two_out, two_expanded] = two_solver.restore(two_states[run_idx]);
                auto two_sim = compute_raw_score(in, two_out);
                two_scores[run_idx] = two_sim.score;
                two_outputs[run_idx] = std::move(two_out);
                two_expanded_basic[run_idx] = two_sim.expanded_basic;
                two_correct[run_idx] = two_sim.correct;
            }

            ll best_two_score = INF;
            int best_two_run_idx = -1;
            for (int run_idx : completed_runs) {
                if (two_scores[run_idx] < best_two_score) {
                    best_two_score = two_scores[run_idx];
                    best_two_run_idx = run_idx;
                }
            }
            cerr << '\n' << Log::INDENT << "candidate scores:\n"
                 << Log::INDENT2
                 << left << setw(10) << "trial"
                 << right << setw(10) << "score"
                 << "  " << left << setw(28) << "MacroA"
                 << setw(16) << "prefixB"
                 << "suffixB\n";
            rep(run_idx, 0, two_macro_runs) {
                cerr << Log::INDENT2
                     << left << setw(10) << (run_idx + 1)
                     << right << setw(10) << (two_scores[run_idx] >= INF ? -1 : two_scores[run_idx])
                     << "  " << left << setw(28) << Log::macro_rle(macro_pairs[run_idx].macro_a)
                     << setw(16) << Log::macro_rle(macro_pairs[run_idx].prefix_b)
                     << Log::macro_rle(macro_pairs[run_idx].suffix_b);
                if (run_idx == best_two_run_idx) cerr << "  <- best";
                cerr << '\n';
            }

            if (best_two_run_idx != -1 && best_two_score < best.score) {
                const auto &best_macro_pair = macro_pairs[best_two_run_idx];
                best.macro = best_macro_pair.macro_a;
                best.pre_pre_macro_move = best_macro_pair.pre_pre_macro_move_a;
                best.use_zero_gain_pre_macro = best_macro_pair.use_zero_gain_pre_macro_a;
                best.output = std::move(two_outputs[best_two_run_idx]);
                best.score = best_two_score;
                best.buttons = best.output.size();
                best.expanded_basic = two_expanded_basic[best_two_run_idx];
                best.correct = two_correct[best_two_run_idx];
                best_is_two_macro = true;
                best_two_macro_state = std::move(two_states[best_two_run_idx]);
            }
            double improvement = final_solver_best_score == 0
                ? 0.0
                : 100.0 * (final_solver_best_score - best.score) / final_solver_best_score;
            cerr << '\n' << Log::INDENT << final_solver_best_score << " -> " << best.score
                 << " (+" << fixed << setprecision(1) << improvement << "% improvement)"
                 << defaultfloat << setprecision(6) << '\n';
        }
    }
    cerr << "elapsed_time=" << timer.elapsed_ms() << "ms\n";

    string best_output = best.output;
    if ((int)best_output.size() > in.T) best_output.resize(in.T);
    for (char c : best_output) cout << c << '\n';
    Log::section("Summary");
    cerr << Log::INDENT << "build_all_pairs_shortest_path profile: calls="
         << build_all_pairs_shortest_path_call_count
         << ", total_us=" << build_all_pairs_shortest_path_total_us
         << ", avg_us="
         << fixed << setprecision(1)
         << (build_all_pairs_shortest_path_call_count == 0
                 ? 0.0
                 : (double)build_all_pairs_shortest_path_total_us
                       / build_all_pairs_shortest_path_call_count)
         << defaultfloat << setprecision(6) << '\n';
    cerr << Log::INDENT << "elapsed_time=" << timer.elapsed_ms() << "ms\n";
    cerr << Log::INDENT << "Best Score = " << best.score << '\n';
    // if (best_is_two_macro) {
    //     cerr << Log::INDENT << "MacroA ID order\n";
    //     rep(i, 1, best_two_macro_state.switch_pos) {
    //         cerr << Log::INDENT2 << i << ". " << best_two_macro_state.order[i].id << '\n';
    //     }
    //     cerr << Log::INDENT << "MacroB ID order\n";
    //     rep(i, best_two_macro_state.switch_pos, (int)best_two_macro_state.order.size()) {
    //         cerr << Log::INDENT2 << i << ". " << best_two_macro_state.order[i].id << '\n';
    //     }
    // }
}
