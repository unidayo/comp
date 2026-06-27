// コードかなり終わっててすみません...
// 全力でAIに書かせまくってたら手のつけようのない違法建築物になりました

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("Ofast")
#include <bits/stdc++.h>

using namespace std;
constexpr bool DEBUG = false;

// ==========================================
// 向き seed 高速化（同一セル4向きソースの warm-start 共有）
//   USE_DIR_SEED      : この最適化を使うか（false
//   で従来パスに完全フォールバック） SEED_FOR_EARLY    :
//   早期終了（ボール始点）グループにも seed を適用するか VERIFY_DIR_SEED   :
//   全域フル化して参照BFSと a 一致を assert（検証用・低速）
// ==========================================
constexpr bool USE_DIR_SEED = true;
constexpr bool SEED_FOR_EARLY = false;
constexpr bool VERIFY_DIR_SEED = false;
int g_log_T = 0;
int g_in_M = 0; // ボール数 in.M（SA から参照するためのグローバル）

// ==========================================
// SA Parameters
// ==========================================
struct SAParams {
    enum MoveType {
        CHANGE_ONE_LEN_PLUS,
        CHANGE_ONE_LEN_MINUS,
        CHANGE_ALL_LEN,
        FLIP_ONE_LR,
        FLIP_ALL_LR,
        DELETE_MID,
        INSERT_MID,
        CHANGE_PREFIX,
        CHANGE_SUFFIX,
        SWAP_MID,
        REVERSE_MID,
        RELOCATE_MID,
        UNIFORM_LEN,
        MOVE_LEN,
        MOVE_COUNT
    };

    string name = "SA";
    double time_limit = 1.95;
    double start_temp = 1000.0;
    double end_temp = 10.0;
    bool use_exp_decay = true;    // true: 指数減衰, false: べき乗減衰
    double power_decay_exp = 1.0; // べき乗減衰の指数 (1.0で線形)
    int time_check_interval = 0x7F;
    int stats_interval = 0; // 0: time_check_interval*10 を自動設定
    array<string, MOVE_COUNT> move_names = {
        "CHG_LEN1_PLUS", "CHG_LEN1_MINUS", "CHG_LENALL",  "FLIP1",   "FLIPALL",
        "DEL_MID",       "INS_MID",        "PREFIX",      "SUFFIX",  "SWAP_MID",
        "REV_MID",       "RELOC_MID",      "UNIFORM_LEN", "MOVE_LEN"};
    array<double, MOVE_COUNT> move_weights = {
        2.0, 2.0, 1.0, 3.0, 1.0, 2.0, 2.0, 1.0, 1.0, 2.0, 1.0, 2.0, 1.0, 2.0};
    bool allow_worse_moves = true; // false: 山登り
    bool maximize = false;
    // >0 のとき「独立モード」: restart_states の各候補を restart_iters
    // 反復ずつ、 候補内 progress(=cand_iter/restart_iters)
    // に基づく独立な温度スケジュールで
    // 焼きなまし、終わったら次の候補へ切り替える（候補を一周したら先頭へ戻る）。
    // 0 のときは従来の時間ベース温度 + interval/lockout 切替。
    // （i64 エイリアスは後方で定義されるためここでは int64_t を使う）
    int64_t restart_iters = 0;
};

struct Tuning {
    // 各フェーズの時間境界に掛ける共通倍率。
    // AHC_*_TIME* 環境変数を読んだ後、apply_time_boundary_scale()
    // で一度だけ反映する。
    static inline double time_boundary_scale = 1.0;

    // ---- Initial macro/order construction ----
    static constexpr double random_macro_len_times = 0.5;

    // ---- Broad macro search ----
    // コスト下界を目的関数にした軽量 SA を、多数の path/random macro
    // 初期解から走らせる。 候補プール上位を full-eval し、macro_sa の
    // restart_states として渡す。
    static inline double broad_phase_end = 0.37;    // broad SA 終了時刻[s]
    static inline int broad_per_macro_trials = 200; // 1初期解あたりのSA試行回数
    static inline int broad_random_count = 300;     // 追加するランダムマクロ数
    static inline int broad_pool_k = 400; // 保持数と full-eval 対象数は同一
    static inline double broad_start_temp = 0.33; // 下界スケールの開始温度
    static inline double broad_end_temp = 0.03;   // 下界スケールの終了温度
    static inline bool broad_use_exp_decay =
        true; // true: 指数減衰, false: べき乗減衰
    static inline double broad_power_decay_exp = 1.0; // 1.0 で線形
    static inline double broad_eval_deadline =
        0.53; // broad 候補 full-eval 打切り[s]
    static inline double broad_eval_hard_deadline =
        1.50; // feasible 未発見時の強制打切り[s]

    // broad 後に full-eval した候補を保持する上位件数。
    // random 初期解の上位数であり、macro_sa の restart_states
    // 件数上限でもある。
    static inline int random_top_k = 100;

    // ---- Macro SA ----
    static inline double sa_time_limit = 1.65; // macro SA 終了時刻[s]
    // optuna_local.db best10 average: score_mean=201.865625 seeds=900..995
    static inline double sa_start_temp = 0.3899248979;
    static inline double sa_end_temp = 0.03636157410;
    static inline bool use_exp_decay =
        false; // true: 指数減衰, false: べき乗減衰
    static inline double power_decay_exp = 2.4458469823; // 1.0 で線形
    static constexpr int sa_time_check_interval = 1;
    static constexpr bool sa_allow_worse_moves = true;

    static inline array<double, SAParams::MOVE_COUNT> sa_move_weights = {
        0.2843769186, 0.2763618701, 0.0000722485, 0.0195328304, 0.0000410629,
        0.0567593535, 0.1786244294, 0.0500250618, 0.0519462840, 0.0364477256,
        0.0000379286, 0.0135597185, 0.0066626727, 0.0255518955};
    static inline array<double, SAParams::MOVE_COUNT> broad_move_weights =
        sa_move_weights;

    static inline int macro_len_delta_plus = 1;
    static inline int macro_len_delta_minus = 2;
    static inline int macro_all_len_delta = 2;
    static inline int macro_move_len_max = 3;
    static inline int broad_len_delta_plus = macro_len_delta_plus;
    static inline int broad_len_delta_minus = macro_len_delta_minus;
    static inline int broad_all_len_delta = macro_all_len_delta;
    static inline int broad_move_len_max = macro_move_len_max;
    static inline int macro_top_k = 200;

    // macro_sa の restart 制御。
    // macro_restart_interval: この反復数ごとに次に良かった初期解へ切り替える。
    // macro_restart_lockout_mul: ベスト更新後 M*M*係数
    // 反復は切り替えを禁止する。 macro_sa_restart_iters:
    //   >0: 各候補をこの反復数ずつ独立した温度 schedule で焼く。
    //    0: 時間ベース温度 + interval/lockout 切替を使う。
    static inline int macro_restart_interval = 1000000;
    static inline double macro_restart_lockout_mul = 1.0;
    static inline int macro_sa_restart_iters = 1000;

    // ---- 下界スコア(bs)による早期枝刈り ----
    // macro_sa の近傍適用後、下界スコア bs（各ボール→担当籠の最短距離和）が
    // 受理許容幅に対して大きく悪化する場合、order 最適化(improve_order)を行わず
    // に異常に悪いスコアを返し、g_macro_cache にも載せて二度と受理しない。
    //   (threshold - current_score) * bs_prune_coef + bs_prune_offset
    //        < (new_bs - current.bs)  ならば打ち切り。
    static inline double bs_prune_coef = 0.5;   // 要チューニング
    static inline double bs_prune_offset = 2.0; // 要チューニング

    // ---- Macro preselect ----
    static inline double macro_preselect_deadline =
        1.75; // macro 事前選別打切り[s]

    // ---- Order SA ----
    static inline double order_sa_time_limit = 1.90; // order SA 終了時刻[s]
    static constexpr double order_hill_climb_span = 0.0025;
    static inline double order_hill_climb_deadline = 1.50; // 未使用
    static constexpr int order_no_improve_base = 100;
    static constexpr int order_no_improve_mul = 8;

    // optuna_order.db top10 median: median_score=188.120
    static inline double order_sa_start_temp = 1.1560420323;
    static inline double order_sa_end_temp = 0.1252299559;
    static inline bool order_sa_use_exp_decay =
        false; // true: 指数減衰, false: べき乗減衰
    static inline double order_sa_power_decay_exp = 1.3787796652; // 1.0 で線形

    static constexpr int ORDER_MOVE_COUNT = 5;
    // 0: swap
    // 1: reverse(2-opt)
    // 2: relocate(長さ1..M-1)
    // 3: adjacent-swap
    // 4: block-relocate+reverse(長さ1..M-1)
    // 環境変数 AHC_ORDER_W0..4 で上書き可能。
    static inline double order_sa_move_weights[ORDER_MOVE_COUNT] = {
        0.0480917786, 0.0194236724, 0.7538809874, 0.0106199489, 0.1211242306};

    // ---- Path annealing ----
    static inline double path_anneal_time_limit = 1.95; // 全体最終境界[s]
    static inline int path_slack = 0;            // 列挙する超過長(0=最短のみ)
    static inline int path_cap = 32;             // 区間あたり候補上限
    static inline int path_implicit_max_len = 5; // 暗黙init_macroの長さ上限
    static inline double path_temp0 = 0.5;
    static inline double path_temp1 = 0.0001;
    static inline bool path_use_exp_decay =
        true; // true: 指数減衰, false: べき乗減衰
    static inline double path_power_decay_exp = 1.0; // 1.0 で線形

    // ---- Presets ----
    // N/M の大きさ別に、主に optuna 対象の order/macro
    // 近傍パラメータを差し替える。
    struct Preset {
        // Broad macro search
        int broad_per_macro_trials = 200;
        double broad_start_temp = 0.33;
        double broad_end_temp = 0.03;
        bool broad_use_exp_decay = true;
        double broad_power_decay_exp = 1.0;

        // Order SA
        double order_sa_start_temp = 1.1560420323;
        double order_sa_end_temp = 0.1252299559;
        bool order_sa_use_exp_decay = false;
        double order_sa_power_decay_exp = 1.3787796652;
        array<double, ORDER_MOVE_COUNT> order_sa_move_weights = {
            0.0480917786, 0.0194236724, 0.7538809874, 0.0106199489,
            0.1211242306};

        // Macro SA
        double sa_start_temp = 2.2899248979;
        double sa_end_temp = 0.6636157410;
        bool use_exp_decay = false;
        double power_decay_exp = 2.4458469823;
        int macro_sa_restart_iters = 1000;
        array<double, SAParams::MOVE_COUNT> sa_move_weights = {
            0.2843769186, 0.2763618701, 0.0000722485, 0.0195328304,
            0.0000410629, 0.0567593535, 0.1786244294, 0.0500250618,
            0.0519462840, 0.0364477256, 0.0000379286, 0.0135597185,
            0.0066626727, 0.0255518955};
        array<double, SAParams::MOVE_COUNT> broad_move_weights =
            sa_move_weights;
        int macro_len_delta_plus = 1;
        int macro_len_delta_minus = 2;
        int macro_all_len_delta = 2;
        int macro_move_len_max = 3;
        int broad_len_delta_plus = macro_len_delta_plus;
        int broad_len_delta_minus = macro_len_delta_minus;
        int broad_all_len_delta = macro_all_len_delta;
        int broad_move_len_max = macro_move_len_max;

        // Phase time boundaries [s]。区間長中央値チューニング由来 (象限別)。
        // 既定は従来の global baseline (q11 はこの既定値を使う)。
        double broad_phase_end = 0.37;
        double broad_eval_deadline = 0.53;
        double sa_time_limit = 1.65;
        double macro_preselect_deadline = 1.75;
        double order_sa_time_limit = 1.90;
    };

    // presets[n_big][m_big], n_big: N > 15, m_big: M > 14
    // [0][0]: N <= 15 && M <= 14
    // [0][1]: N <= 15 && M > 14
    // [1][0]: N > 15  && M <= 14
    // [1][1]: N > 15  && M > 14
    static array<array<Preset, 2>, 2> presets;

    static void apply_preset(int N, int M) {
        const Preset &p = presets[N > 15 ? 1 : 0][M > 14 ? 1 : 0];
        broad_per_macro_trials = p.broad_per_macro_trials;
        broad_start_temp = p.broad_start_temp;
        broad_end_temp = p.broad_end_temp;
        broad_use_exp_decay = p.broad_use_exp_decay;
        broad_power_decay_exp = p.broad_power_decay_exp;
        order_sa_start_temp = p.order_sa_start_temp;
        order_sa_end_temp = p.order_sa_end_temp;
        order_sa_use_exp_decay = p.order_sa_use_exp_decay;
        order_sa_power_decay_exp = p.order_sa_power_decay_exp;
        for (int i = 0; i < ORDER_MOVE_COUNT; i++)
            order_sa_move_weights[i] = p.order_sa_move_weights[i];
        sa_start_temp = p.sa_start_temp;
        sa_end_temp = p.sa_end_temp;
        use_exp_decay = p.use_exp_decay;
        power_decay_exp = p.power_decay_exp;
        macro_sa_restart_iters = p.macro_sa_restart_iters;
        sa_move_weights = p.sa_move_weights;
        broad_move_weights = p.broad_move_weights;
        macro_len_delta_plus = p.macro_len_delta_plus;
        macro_len_delta_minus = p.macro_len_delta_minus;
        macro_all_len_delta = p.macro_all_len_delta;
        macro_move_len_max = p.macro_move_len_max;
        broad_len_delta_plus = p.broad_len_delta_plus;
        broad_len_delta_minus = p.broad_len_delta_minus;
        broad_all_len_delta = p.broad_all_len_delta;
        broad_move_len_max = p.broad_move_len_max;
        broad_phase_end = p.broad_phase_end;
        broad_eval_deadline = p.broad_eval_deadline;
        sa_time_limit = p.sa_time_limit;
        macro_preselect_deadline = p.macro_preselect_deadline;
        order_sa_time_limit = p.order_sa_time_limit;
    }

    static void apply_time_boundary_scale() {
        broad_phase_end *= time_boundary_scale;
        broad_eval_deadline *= time_boundary_scale;
        broad_eval_hard_deadline *= time_boundary_scale;
        order_hill_climb_deadline *= time_boundary_scale;
        sa_time_limit *= time_boundary_scale;
        macro_preselect_deadline *= time_boundary_scale;
        order_sa_time_limit *= time_boundary_scale;
        path_anneal_time_limit *= time_boundary_scale;
    }
};

inline array<array<Tuning::Preset, 2>, 2> Tuning::presets = [] {
    array<array<Tuning::Preset, 2>, 2> ps{};
    {
        auto &p = ps[0][0];
        // q00 broad+macro_sa optuna top tendency, seeds 200..296.
        p.broad_per_macro_trials = 2000;
        p.broad_start_temp = 1.05;
        p.broad_end_temp = 0.014;
        p.broad_use_exp_decay = false;
        p.broad_power_decay_exp = 1.26;
        p.sa_start_temp = 1.05;
        p.sa_end_temp = 0.62;
        p.use_exp_decay = true;
        p.power_decay_exp = 3.82;
        p.macro_sa_restart_iters = 3000;
    }
    {
        auto &p = ps[0][1];
        // q01 optuna top50 median: median_score=207.1718750000
        p.sa_move_weights = {0.2449079666, 0.2402487147, 0.0001847575,
                             0.0224808583, 0.0001543691, 0.0772833239,
                             0.1516480102, 0.0465013531, 0.0936589828,
                             0.0717009572, 0.0004925401, 0.0151326867,
                             0.0085978279, 0.0270076518};
        p.broad_move_weights = p.sa_move_weights;
        p.macro_len_delta_plus = 1;
        p.macro_len_delta_minus = 2;
        p.macro_all_len_delta = 3;
        p.macro_move_len_max = 2;
        p.broad_len_delta_plus = p.macro_len_delta_plus;
        p.broad_len_delta_minus = p.macro_len_delta_minus;
        p.broad_all_len_delta = p.macro_all_len_delta;
        p.broad_move_len_max = p.macro_move_len_max;

        // q01 broad+macro_sa optuna top tendency, seeds 200..296.
        p.broad_per_macro_trials = 200;
        p.broad_start_temp = 0.35;
        p.broad_end_temp = 0.065;
        p.broad_use_exp_decay = true;
        p.broad_power_decay_exp = 1.8;
        p.sa_start_temp = 0.84;
        p.sa_end_temp = 0.018;
        p.use_exp_decay = true;
        p.power_decay_exp = 0.9;
        p.macro_sa_restart_iters = 5000;
        // q01 phase times: 区間長中央値 (top5%) 由来。確度は薄め (要A/B)。
        p.broad_phase_end = 0.715;
        p.broad_eval_deadline = 0.846;
        p.sa_time_limit = 1.646;
        p.macro_preselect_deadline = 1.666;
        p.order_sa_time_limit = 1.686;
    }
    {
        auto &p = ps[1][0];
        // q10 broad+macro_sa optuna top tendency, seeds 200..296.
        p.broad_per_macro_trials = 200;
        p.broad_start_temp = 1.35;
        p.broad_end_temp = 0.079;
        p.broad_use_exp_decay = true;
        p.broad_power_decay_exp = 2.3;
        p.sa_start_temp = 1.74;
        p.sa_end_temp = 0.0008;
        p.use_exp_decay = false;
        p.power_decay_exp = 4.15;
        p.macro_sa_restart_iters = 1000;
        // q10 phase times: 区間長中央値 (top5%) 由来。最も確度が高い。
        p.broad_phase_end = 0.927;
        p.broad_eval_deadline = 0.947;
        p.sa_time_limit = 1.672;
        p.macro_preselect_deadline = 1.692;
        p.order_sa_time_limit = 1.712;
        // q10 温度: top5% 中央値由来 (importance sa系=0.6)。
        p.broad_start_temp = 0.848;
        p.sa_start_temp = 1.273;
        p.order_sa_end_temp = 0.083;
    }
    // {
    //     auto &p = ps[1][1];
    //     // q11 broad+macro_sa optuna top tendency, seeds 200..296.
    //     p.broad_per_macro_trials = 100;
    //     p.broad_start_temp = 0.024;
    //     p.broad_end_temp = 0.00063;
    //     p.broad_use_exp_decay = false;
    //     p.broad_power_decay_exp = 1.49;
    //     p.sa_start_temp = 0.32;
    //     p.sa_end_temp = 0.75;
    //     p.use_exp_decay = false;
    //     p.power_decay_exp = 3.05;
    //     p.macro_sa_restart_iters = 1000;
    // }
    return ps;
}();

#pragma region Library

// Macros & Constants
#define el '\n'
#define all(v) (v).begin(), (v).end()
using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
#define rep(i, n) for (i64 i = 0; i < (i64)(n); i++)
template <class T> using min_queue = priority_queue<T, vector<T>, greater<T>>;
template <class T> using max_queue = priority_queue<T>;

const double pi = 3.141592653589793238;
const i32 inf32 = 1073741823;
const i64 inf64 = 1LL << 60;
const string ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string abc = "abcdefghijklmnopqrstuvwxyz";
const int MOD = 998244353;
const array<int, 8> dx = {0, 0, -1, 1, -1, -1, 1, 1};
const array<int, 8> dy = {-1, 1, 0, 0, -1, 1, -1, 1};

// Printing
template <class T> void print_collection(ostream &out, T const &x);
template <class T, size_t... I>
void print_tuple(ostream &out, T const &a, index_sequence<I...>);
namespace std {
template <class... A> ostream &operator<<(ostream &out, tuple<A...> const &x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
}
template <class... A> ostream &operator<<(ostream &out, pair<A...> const &x) {
    print_tuple(out, x, index_sequence_for<A...>{});
    return out;
}
template <class A, size_t N>
ostream &operator<<(ostream &out, array<A, N> const &x) {
    print_collection(out, x);
    return out;
}
template <class A> ostream &operator<<(ostream &out, vector<A> const &x) {
    print_collection(out, x);
    return out;
}
template <class A> ostream &operator<<(ostream &out, deque<A> const &x) {
    print_collection(out, x);
    return out;
}
template <class A> ostream &operator<<(ostream &out, multiset<A> const &x) {
    print_collection(out, x);
    return out;
}
template <class A, class B>
ostream &operator<<(ostream &out, multimap<A, B> const &x) {
    print_collection(out, x);
    return out;
}
template <class A> ostream &operator<<(ostream &out, set<A> const &x) {
    print_collection(out, x);
    return out;
}
template <class A, class B>
ostream &operator<<(ostream &out, map<A, B> const &x) {
    print_collection(out, x);
    return out;
}
template <class A, class B>
ostream &operator<<(ostream &out, unordered_set<A> const &x) {
    print_collection(out, x);
    return out;
}
} // namespace std
template <class T, size_t... I>
void print_tuple(ostream &out, T const &a, index_sequence<I...>) {
    using swallow = int[];
    out << '(';
    (void)swallow{0, (void(out << (I == 0 ? "" : ", ") << get<I>(a)), 0)...};
    out << ')';
}
template <class T> void print_collection(ostream &out, T const &x) {
    int f = 0;
    out << '[';
    for (auto const &i : x) {
        out << (f++ ? "," : "");
        out << i;
    }
    out << "]";
}
// Random
struct RNG {
    uint64_t s[2];
    RNG(u64 seed) { reset(seed); }
    RNG() { reset(time(0)); }
    using result_type = u32;
    static constexpr u32 min() { return numeric_limits<u32>::min(); }
    static constexpr u32 max() { return numeric_limits<u32>::max(); }
    u32 operator()() { return randomInt32(); }
    static __attribute__((always_inline)) inline uint64_t rotl(const uint64_t x,
                                                               int k) {
        return (x << k) | (x >> (64 - k));
    }
    inline void reset(u64 seed) {
        struct splitmix64_state {
            u64 s;
            u64 splitmix64() {
                u64 result = (s += 0x9E3779B97f4A7C15);
                result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9;
                result = (result ^ (result >> 27)) * 0x94D049BB133111EB;
                return result ^ (result >> 31);
            }
        };
        splitmix64_state sm{seed};
        s[0] = sm.splitmix64();
        s[1] = sm.splitmix64();
    }
    uint64_t next() {
        const uint64_t s0 = s[0];
        uint64_t s1 = s[1];
        const uint64_t result = rotl(s0 * 5, 7) * 9;
        s1 ^= s0;
        s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
        s[1] = rotl(s1, 37);                   // c
        return result;
    }
    inline u32 randomInt32() { return next(); }
    inline u64 randomInt64() { return next(); }
    inline u32 random32(u32 r) { return (((u64)randomInt32()) * r) >> 32; }
    inline u64 random64(u64 r) { return randomInt64() % r; }
    inline u32 randomRange32(u32 l, u32 r) { return l + random32(r - l + 1); }
    inline u64 randomRange64(u64 l, u64 r) { return l + random64(r - l + 1); }
    inline double randomDouble() {
        return (double)randomInt32() / 4294967296.0;
    }
    inline double randomDoubleOpen01() {
        return (randomInt32() + 1.0) / 4294967297.0;
    }
    inline float randomFloat() { return (float)randomInt32() / 4294967296.0; }
    inline double randomRangeDouble(double l, double r) {
        return l + randomDouble() * (r - l);
    }
    inline double randomGaussian(double mean = 0.0, double stddev = 1.0) {
        double u1 = randomDoubleOpen01();
        double u2 = randomDouble();
        double z = sqrt(-2.0 * log(u1)) * cos(2.0 * 3.141592653589793238 * u2);
        return mean + stddev * z;
    }
    template <class T> void shuffle(vector<T> &v) {
        i32 sz = v.size();
        for (i32 i = sz; i > 1; i--) {
            i32 p = random32(i);
            swap(v[i - 1], v[p]);
        }
    }
    template <class T> void shuffle(T *fr, T *to) {
        i32 sz = distance(fr, to);
        for (int i = sz; i > 1; i--) {
            int p = random32(i);
            swap(fr[i - 1], fr[p]);
        }
    }
    template <class T> inline int sample_index(vector<T> const &v) {
        return random32(v.size());
    }
    template <class T> inline T sample(vector<T> const &v) {
        return v[sample_index(v)];
    }
} rng('M' + 'H' + time(0));

// Weighted Samplers

// 重みの動的更新が多い場合向き。sample/update共にO(log n)
class FenwickWeightedSampler {
  private:
    int n_ = 0;
    vector<double> bit_;
    vector<double> weight_;
    double total_ = 0.0;

    void add_internal(int idx0, double delta) {
        int i = idx0 + 1;
        while (i <= n_) {
            bit_[i] += delta;
            i += i & -i;
        }
    }

  public:
    // Complexity: O(n)
    void init(int n, double initial_weight = 0.0) {
        n_ = n;
        bit_.assign(n_ + 1, 0.0);
        weight_.assign(n_, initial_weight);
        total_ = 0.0;
        if (initial_weight != 0.0) {
            for (int i = 0; i < n_; i++) {
                add_internal(i, initial_weight);
            }
            total_ = initial_weight * n_;
        }
    }

    // Complexity: O(n log n)
    void build(const vector<double> &weights) {
        init((int)weights.size(), 0.0);
        for (int i = 0; i < n_; i++) {
            weight_[i] = weights[i];
            add_internal(i, weights[i]);
            total_ += weights[i];
        }
    }

    // Complexity: O(log n)
    void set_weight(int idx, double new_weight) {
        assert(0 <= idx && idx < n_);
        const double delta = new_weight - weight_[idx];
        weight_[idx] = new_weight;
        add_internal(idx, delta);
        total_ += delta;
    }

    // Complexity: O(log n)
    void add_weight(int idx, double delta) {
        assert(0 <= idx && idx < n_);
        weight_[idx] += delta;
        add_internal(idx, delta);
        total_ += delta;
    }

    // Complexity: O(log n)
    int sample() const {
        assert(n_ > 0);
        assert(total_ > 0.0);
        const double r = rng.randomDouble() * total_;

        int idx = 0;
        double prefix = 0.0;
        int step = 1;
        while ((step << 1) <= n_)
            step <<= 1;
        for (int k = step; k > 0; k >>= 1) {
            const int nxt = idx + k;
            if (nxt <= n_ && prefix + bit_[nxt] <= r) {
                idx = nxt;
                prefix += bit_[nxt];
            }
        }
        if (idx >= n_)
            idx = n_ - 1;
        return idx;
    }

    int size() const { return n_; }
    double total_weight() const { return total_; }
    double weight(int idx) const {
        assert(0 <= idx && idx < n_);
        return weight_[idx];
    }
};

// 重みが固定 or 更新頻度が低い場合向き。sampleはO(1)、更新時にO(n)のrebuild
class AliasWeightedSampler {
  private:
    int n_ = 0;
    vector<double> weight_;
    double total_ = 0.0;
    vector<double> prob_;
    vector<int> alias_;
    bool dirty_ = true;

    // Complexity: O(n)
    void rebuild_alias_table() {
        prob_.assign(n_, 0.0);
        alias_.assign(n_, 0);
        total_ = 0.0;
        for (double w : weight_)
            total_ += w;
        assert(total_ > 0.0);

        vector<double> scaled(n_);
        vector<int> small;
        vector<int> large;
        small.reserve(n_);
        large.reserve(n_);
        for (int i = 0; i < n_; i++) {
            scaled[i] = weight_[i] * n_ / total_;
            if (scaled[i] < 1.0) {
                small.push_back(i);
            } else {
                large.push_back(i);
            }
        }

        while (!small.empty() && !large.empty()) {
            const int s = small.back();
            small.pop_back();
            const int l = large.back();
            large.pop_back();
            prob_[s] = scaled[s];
            alias_[s] = l;
            scaled[l] -= (1.0 - scaled[s]);
            if (scaled[l] < 1.0) {
                small.push_back(l);
            } else {
                large.push_back(l);
            }
        }
        while (!large.empty()) {
            const int i = large.back();
            large.pop_back();
            prob_[i] = 1.0;
            alias_[i] = i;
        }
        while (!small.empty()) {
            const int i = small.back();
            small.pop_back();
            prob_[i] = 1.0;
            alias_[i] = i;
        }
        dirty_ = false;
    }

  public:
    // Complexity: O(n)
    void init(int n, double initial_weight = 0.0) {
        n_ = n;
        weight_.assign(n_, initial_weight);
        total_ = initial_weight * n_;
        prob_.clear();
        alias_.clear();
        dirty_ = true;
    }

    // Complexity: O(n)
    void build(const vector<double> &weights) {
        n_ = (int)weights.size();
        weight_ = weights;
        dirty_ = true;
        // Build lazily on sample() to allow batch updates before first use.
    }

    // Complexity: O(1), table becomes dirty
    void set_weight(int idx, double new_weight) {
        assert(0 <= idx && idx < n_);
        weight_[idx] = new_weight;
        dirty_ = true;
    }

    // Complexity: O(1), table becomes dirty
    void add_weight(int idx, double delta) {
        assert(0 <= idx && idx < n_);
        weight_[idx] += delta;
        dirty_ = true;
    }

    // Complexity: O(1) after table is ready, O(n) if rebuild is needed
    int sample() {
        assert(n_ > 0);
        if (dirty_)
            rebuild_alias_table();
        const int i = (int)rng.random32((uint32_t)n_);
        return (rng.randomDouble() < prob_[i]) ? i : alias_[i];
    }

    int size() const { return n_; }
    double total_weight() {
        if (dirty_)
            rebuild_alias_table();
        return total_;
    }
    double weight(int idx) const {
        assert(0 <= idx && idx < n_);
        return weight_[idx];
    }
};

// Timer
struct Timer {
    chrono::steady_clock::time_point t_begin;
    Timer() { t_begin = chrono::steady_clock::now(); }
    void reset() { t_begin = chrono::steady_clock::now(); }
    float elapsed() const {
        return chrono::duration<float>(chrono::steady_clock::now() - t_begin)
            .count();
    }
} timer;

// Util
template <class T> T &smin(T &x, T const &y) {
    x = min(x, y);
    return x;
}
template <class T> T &smax(T &x, T const &y) {
    x = max(x, y);
    return x;
}
template <typename T> int sgn(T val) {
    if (val < 0)
        return -1;
    if (val > 0)
        return 1;
    return 0;
}
static inline string int_to_string(int val, int digits = 0) {
    string s = to_string(val);
    reverse(begin(s), end(s));
    while ((int)s.size() < digits)
        s.push_back('0');
    reverse(begin(s), end(s));
    return s;
}
static inline void debug() { cerr << "\n"; }
template <class T, class... V> void debug(T const &t, V const &...v) {
    if constexpr (DEBUG) {
        cerr << t;
        if (sizeof...(v)) {
            cerr << ", ";
        }
        debug(v...);
    }
}
__attribute__((always_inline)) inline u64 bit(u64 x) { return 1ull << x; }
__attribute__((always_inline)) inline void setbit(u64 &a, u32 b,
                                                  u64 value = 1) {
    a = (a & ~bit(b)) | (value << b);
}
__attribute__((always_inline)) inline u64 getbit(u64 a, u32 b) {
    return (a >> b) & 1;
}
__attribute__((always_inline)) inline u64 lsb(u64 a) {
    return __builtin_ctzll(a);
}
__attribute__((always_inline)) inline int msb(uint64_t bb) {
    return __builtin_clzll(bb) ^ 63;
}
struct Init {
    Init() {
        ios::sync_with_stdio(0);
        cin.tie(0);
    }
} init;

// Lookup Tables
constexpr int TABLE_SIZE = 1024;
double log_table[TABLE_SIZE + 1];

inline void init_log_table() {
    for (int i = 0; i <= TABLE_SIZE; i++) {
        double x = (double)i / TABLE_SIZE;
        if (x == 0.0) {
            log_table[i] = -10.0; // log(0) = -inf を有限値で近似
        } else {
            log_table[i] = log(x);
        }
    }
}

// log(x) の線形補間テーブル引き (0 < x < 1)
inline double log_fast(double x) {
    if constexpr (DEBUG)
        assert(0.0 < x && x < 1.0);

    double index = x * TABLE_SIZE;
    int idx = (int)index;
    if (idx >= TABLE_SIZE)
        return log_table[TABLE_SIZE];

    double frac = index - idx;
    return log_table[idx] * (1.0 - frac) + log_table[idx + 1] * frac;
}

// Box-Muller法による正規分布の事前計算テーブル
constexpr int GAUSS_TABLE_SIZE = 131072;
double gauss_table[GAUSS_TABLE_SIZE];
inline void init_gauss_table() {
    for (int i = 0; i < GAUSS_TABLE_SIZE; i++) {
        double u1 = rng.randomDoubleOpen01();
        double u2 = rng.randomDouble();
        gauss_table[i] =
            sqrt(-2.0 * log(u1)) * cos(2.0 * 3.141592653589793238 * u2);
    }
}

inline double fast_gaussian(double mean, double stddev) {
    return mean + stddev * gauss_table[rng.random32(GAUSS_TABLE_SIZE)];
}

template <class Score>
inline bool is_better(Score lhs, Score rhs, bool maximize) {
    return maximize ? (lhs > rhs) : (lhs < rhs);
}

template <class Score>
inline bool is_not_worse(Score lhs, Score rhs, bool maximize) {
    return maximize ? (lhs >= rhs) : (lhs <= rhs);
}

// SA Engine
struct MoveStats {
    i64 try_cnt = 0;
    i64 evaluated_cnt = 0;
    i64 accept_cnt = 0;
    i64 improve_cnt = 0;
    i64 update_best_cnt = 0;
};

// MoveApplierインターフェース:
//   using state_t / score_t
//   optional<score_t> apply(state_t&, int move_type, f64 score_threshold)
//      — 遷移適用、評価可能なら次スコアを返す
//   void rollback(state_t&, int move_type)    — reject時のロールバック
//   void accept(state_t&, int move_type)      — accept時の後処理

template <class MoveApplier> class SA {
    using State = typename MoveApplier::state_t;
    using score_t = typename MoveApplier::score_t;

    const SAParams params;
    const int effective_stats_interval;
    double temp_table[TABLE_SIZE + 1];

    void init_temp_table() {
        for (int i = 0; i <= TABLE_SIZE; i++) {
            double progress = (double)i / TABLE_SIZE;
            if (params.use_exp_decay) {
                double ratio = params.end_temp / params.start_temp;
                temp_table[i] = params.start_temp * pow(ratio, progress);
            } else {
                double p = pow(progress, params.power_decay_exp);
                temp_table[i] = params.start_temp -
                                (params.start_temp - params.end_temp) * p;
            }
        }
    }

    double temp_decay(double progress) {
        double index = progress * TABLE_SIZE;
        int idx = (int)index;
        if (idx >= TABLE_SIZE)
            return temp_table[TABLE_SIZE];
        double frac = index - idx;
        return temp_table[idx] * (1.0 - frac) + temp_table[idx + 1] * frac;
    }

    inline void print_stats(i64 iter, f64 time, f64 temp, score_t score,
                            score_t best) {
        if constexpr (!DEBUG)
            return;
        (void)iter;
        (void)time;
        (void)temp;
        (void)score;
        (void)best;
        /*
        int n = (int)stats.size();
        cerr << "VISUALIZE: {";
        cerr << "\"sa_name\": \"" << params.name << "\", ";
        cerr << "\"iter\": " << iter << ", ";
        cerr << "\"time\": " << fixed << setprecision(3) << time << ", ";
        cerr << "\"temp\": " << fixed << setprecision(3) << temp << ", ";
        cerr << "\"score\": " << score << ", ";
        cerr << "\"best_score\": " << best << ", ";
        cerr << "\"moves\": [";
        for (int i = 0; i < n; i++) {
            if (i > 0)
                cerr << ", ";
            cerr << "{\"name\": \"" << params.move_names[i]
                 << "\", \"tried\": " << stats[i].try_cnt
                 << ", \"evaluated\": " << stats[i].evaluated_cnt
                 << ", \"accepted\": " << stats[i].accept_cnt
                 << ", \"improved\": " << stats[i].improve_cnt
                 << ", \"updated_best\": " << stats[i].update_best_cnt << "}";
        }
        cerr << "]}" << el;
        */
    }

  public:
    vector<MoveStats> stats;
    vector<State> top_states;

    SA(const SAParams &params = {})
        : params(params), effective_stats_interval(
                              params.stats_interval > 0
                                  ? params.stats_interval
                                  : 10 * (params.time_check_interval + 1)) {
        init_temp_table();
    }

    State run(const State &init_state, MoveApplier &applier,
              const vector<State> *restart_states = nullptr) {
        top_states.clear();
        AliasWeightedSampler move_selector;
        vector<double> move_weights(params.move_weights.begin(),
                                    params.move_weights.end());
        move_selector.build(move_weights);
        int num_moves = move_selector.size();
        stats.assign(num_moves, MoveStats{});

        State state = init_state;
        state.score = state.calc_score_full();
        auto better_state = [&](const State &a, const State &b) {
            if (a.score != b.score)
                return params.maximize ? a.score > b.score : a.score < b.score;
            if (a.last_E != b.last_E)
                return a.last_E < b.last_E;
            if (a.last_A != b.last_A)
                return a.last_A < b.last_A;
            return a.macro < b.macro;
        };
        auto remember_top_state = [&](const State &cand) {
            const int top_k = max(1, Tuning::macro_top_k);
            for (State &s : top_states) {
                if (s.macro == cand.macro) {
                    if (better_state(cand, s))
                        s = cand;
                    sort(top_states.begin(), top_states.end(), better_state);
                    return;
                }
            }
            if ((int)top_states.size() < top_k) {
                top_states.push_back(cand);
                sort(top_states.begin(), top_states.end(), better_state);
                return;
            }
            if (better_state(cand, top_states.back())) {
                top_states.back() = cand;
                sort(top_states.begin(), top_states.end(), better_state);
            }
        };
        remember_top_state(state);

        // SA: bestを独立管理 / 山登り: bestはstateへの参照（コピー不要）
        std::optional<State> best_sa;
        if (params.allow_worse_moves)
            best_sa.emplace(state);
        State &best = params.allow_worse_moves ? *best_sa : state;

        f64 current_temp = params.start_temp;
        f64 time_start = timer.elapsed();
        f64 time_now = time_start;
        i64 local_iter = 0;
        // 次に切り替えるランダム初期解のインデックス（先頭=ベストは init_state
        // として既に使用済みなので 1 から開始）。
        int restart_idx = 1;
        const int restart_interval = max(1, Tuning::macro_restart_interval);
        // 独立モード（params.restart_iters>0）用: 現在の候補内での反復数。
        const i64 restart_iters = params.restart_iters;
        i64 cand_iter = 0;
        // 現在のマクロ（直近の切り替え以降）のベストスコアと、その更新反復。
        // ベスト更新後 in.M*in.M 反復の間は切り替えを禁止する。
        score_t cur_best_score = state.score;
        i64 last_improve_iter = local_iter;
        const i64 restart_lockout =
            (i64)((double)g_in_M * g_in_M * Tuning::macro_restart_lockout_mul);
        struct BestLog {
            f64 time = 0.0;
            i64 iter = 0;
            score_t score = 0;
            int A = 0;
            int E = 0;
            string macro;
        };
        vector<BestLog> best_logs;

        while (true) {
            local_iter++;

            if (restart_iters > 0) {
                // ===== 独立モード =====
                // 各候補を restart_iters 反復ずつ、候補内 progress
                // に基づく独立な
                // 温度で焼きなまし、終わったら次の候補へ切り替える（一周したら
                // 先頭へ戻り、時間制限まで継続）。best_sa
                // は全候補で共有される。
                if (restart_states != nullptr && !restart_states->empty() &&
                    cand_iter >= restart_iters) {
                    if (restart_idx >= (int)restart_states->size())
                        restart_idx = 0;
                    state = (*restart_states)[restart_idx++];
                    state.score = state.calc_score_full();
                    cand_iter = 0;
                    remember_top_state(state);
                }
                if ((local_iter & params.time_check_interval) == 0) {
                    time_now = timer.elapsed();
                    if (time_now > params.time_limit)
                        break;
                }
                if (params.allow_worse_moves)
                    current_temp =
                        temp_decay((f64)cand_iter / (f64)restart_iters);
                cand_iter++;
            } else {
                // ===== 従来モード: 時間ベース温度 + interval/lockout 切替
                // =====
                // 一定反復ごとに次に良かったランダム初期解へ状態を切り替える。
                // best_sa（独立管理のベスト）は維持され、純粋な多様化となる。
                if (restart_states != nullptr && params.allow_worse_moves &&
                    local_iter % restart_interval == 0 &&
                    (local_iter - last_improve_iter) >= restart_lockout &&
                    restart_idx < (int)restart_states->size()) {
                    state = (*restart_states)[restart_idx++];
                    state.score = state.calc_score_full();
                    cur_best_score = state.score;
                    last_improve_iter = local_iter;
                    remember_top_state(state);
                }

                if ((local_iter & params.time_check_interval) == 0) {
                    time_now = timer.elapsed();
                    if (time_now > params.time_limit)
                        break;
                    if (params.allow_worse_moves) {
                        f64 progress = (time_now - time_start) /
                                       (params.time_limit - time_start);
                        current_temp = temp_decay(progress);
                    }
                    if constexpr (DEBUG) {
                        if (local_iter % effective_stats_interval == 0) {
                            print_stats(local_iter, time_now, current_temp,
                                        state.score, best.score);
                        }
                    }
                }
            }

            f64 delta_threshold;
            if (!params.allow_worse_moves) {
                delta_threshold = 0.0;
            } else {
                delta_threshold =
                    current_temp * log_fast(rng.randomDoubleOpen01());
            }
            f64 score_threshold = params.maximize
                                      ? (f64)state.score + delta_threshold
                                      : (f64)state.score - delta_threshold;

            int move_type = move_selector.sample();
            if constexpr (DEBUG)
                stats[move_type].try_cnt++;

            optional<score_t> next_score_opt =
                applier.apply(state, move_type, score_threshold);
            if (!next_score_opt)
                continue;
            score_t next_score = *next_score_opt;

            if constexpr (DEBUG)
                stats[move_type].evaluated_cnt++;

            bool accept =
                is_not_worse((f64)next_score, score_threshold, params.maximize);
            if constexpr (DEBUG) {
                f64 delta = (f64)(next_score - state.score);
                if (!params.maximize)
                    delta = -delta;
                if (delta > 0)
                    stats[move_type].improve_cnt++;
            }

            if (accept) {
                if constexpr (DEBUG)
                    stats[move_type].accept_cnt++;

                bool is_better_than_best =
                    is_better(next_score, best.score, params.maximize);
                // 現在のマクロ（直近の切り替え以降）のベスト更新を記録。
                if (is_better(next_score, cur_best_score, params.maximize)) {
                    cur_best_score = next_score;
                    last_improve_iter = local_iter;
                }
                state.score = next_score;
                applier.accept(state, move_type);
                remember_top_state(state);
                if (is_better_than_best) {
                    if (params.allow_worse_moves)
                        best = state;
                    if constexpr (DEBUG) {
                        stats[move_type].update_best_cnt++;
                        best_logs.push_back({timer.elapsed(), local_iter,
                                             best.score, best.last_A,
                                             best.last_E, best.macro});
                    }
                }
            } else {
                applier.rollback(state, move_type);
            }
        }
        if constexpr (DEBUG) {
            for (const BestLog &log : best_logs) {
                cerr << "[macro_sa_best] time=" << fixed << setprecision(3)
                     << log.time << " iter=" << log.iter
                     << " total_iter=" << local_iter << " score=" << log.score
                     << " A=" << log.A << " E=" << log.E << " T=" << g_log_T
                     << " macro=" << log.macro << el;
            }
            cerr << "[" << params.name << "_end] time=" << fixed
                 << setprecision(3) << timer.elapsed() << " iter=" << local_iter
                 << " total_iter=" << local_iter << " score=" << best.score
                 << " A=" << best.last_A << " E=" << best.last_E
                 << " T=" << g_log_T << " macro=" << best.macro << el;
        }
        return best;
    }
};
#pragma endregion

// ==========================================
// Profiling
// ==========================================
struct ProfileData {
    double time_macro_next = 0.0;
    double time_dijkstra = 0.0;
    double time_query = 0.0;
    double time_improve = 0.0;
    int64_t count_build = 0;
    int64_t count_improve = 0;
    int64_t improve_iters = 0;
    int64_t cache_hit = 0;
    int64_t cache_miss = 0;

    void print() const {
        double total =
            time_macro_next + time_dijkstra + time_query + time_improve;
        auto pct = [&](double t) {
            return total > 0 ? 100.0 * t / total : 0.0;
        };
        auto avg = [](double t, int64_t n) {
            return n > 0 ? t / (double)n : 0.0;
        };
        const int64_t cache_total = cache_hit + cache_miss;
        const double hit_rate =
            cache_total > 0 ? 100.0 * cache_hit / cache_total : 0.0;
        cerr << "\n========== PROFILE ==========\n";
        cerr << fixed << setprecision(4);
        cerr << "cache hit        : " << cache_hit << " / " << cache_total
             << " (" << fixed << setprecision(1) << hit_rate << "%)\n";
        cerr << fixed << setprecision(4);
        cerr << "build calls      : " << count_build << "\n";
        cerr << "  macro_next     : " << time_macro_next << "s ("
             << pct(time_macro_next)
             << "%, avg=" << avg(time_macro_next, count_build) * 1000.0
             << "ms/call)\n";
        cerr << "  dijkstra       : " << time_dijkstra << "s ("
             << pct(time_dijkstra)
             << "%, avg=" << avg(time_dijkstra, count_build) * 1000.0
             << "ms/call)\n";
        cerr << "  query (to_ball/box) : " << time_query << "s ("
             << pct(time_query)
             << "%, avg=" << avg(time_query, count_build) * 1000.0
             << "ms/call)\n";
        cerr << "improve_order calls: " << count_improve
             << " (iters=" << improve_iters << ")\n";
        cerr << "  improve_order  : " << time_improve << "s ("
             << pct(time_improve)
             << "%, avg=" << avg(time_improve, count_improve) * 1000.0
             << "ms/call, " << avg(time_improve, improve_iters) * 1000000.0
             << "us/iter)\n";
        cerr << "  total measured : " << total << "s\n";
        cerr << "=============================\n";
    }
} g_prof;

// ==========================================
// 問題ごとに編集する部分
// ==========================================

struct Input {
    int N = 0;
    int M = 0;
    int T = 0;
    vector<string> v;
    vector<string> h;
    vector<pair<int, int>> balls;
    vector<pair<int, int>> baskets;
    vector<int> ball_cells;
    vector<int> basket_cells;
    vector<array<int, 3>> base_next;
    // fwd_pow[k][sid] = sid から F(前進)を k 回適用した後の状態。
    // F の連続適用を 1 回のテーブル参照(O(S))に畳むための前計算。
    // N-1 回前進すれば必ず壁(=不動点)に到達するので、連長は N でクリップして
    // 厳密一致する（後続フェーズでも流用可能）。
    int fwd_max = 0; // = N（クリップ上限）
    vector<vector<int>> fwd_pow;

    int cell_id(int r, int c) const { return r * N + c; }
    int state_id(int r, int c, int dir) const {
        return (cell_id(r, c) << 2) | dir;
    }
    int state_cell(int sid) const { return sid >> 2; }
    int state_dir(int sid) const { return sid & 3; }
    int state_r(int sid) const { return state_cell(sid) / N; }
    int state_c(int sid) const { return state_cell(sid) % N; }

    bool can_move(int r, int c, int dir) const {
        if (dir == 0) {
            return c + 1 < N && v[r][c] == '0';
        }
        if (dir == 1) {
            return r + 1 < N && h[r][c] == '0';
        }
        if (dir == 2) {
            return c > 0 && v[r][c - 1] == '0';
        }
        return r > 0 && h[r - 1][c] == '0';
    }

    int forward_state(int sid) const {
        static constexpr array<int, 4> dr = {0, 1, 0, -1};
        static constexpr array<int, 4> dc = {1, 0, -1, 0};
        const int r = state_r(sid);
        const int c = state_c(sid);
        const int dir = state_dir(sid);
        if (!can_move(r, c, dir))
            return sid;
        return state_id(r + dr[dir], c + dc[dir], dir);
    }

    void input() {
        cin >> N >> M >> T;
        v.resize(N);
        h.resize(max(0, N - 1));
        for (int i = 0; i < N; i++)
            cin >> v[i];
        for (int i = 0; i < N - 1; i++)
            cin >> h[i];
        balls.resize(M);
        baskets.resize(M);
        ball_cells.resize(M);
        basket_cells.resize(M);
        for (int i = 0; i < M; i++) {
            int br, bc, gr, gc;
            cin >> br >> bc >> gr >> gc;
            balls[i] = {br, bc};
            baskets[i] = {gr, gc};
            ball_cells[i] = cell_id(br, bc);
            basket_cells[i] = cell_id(gr, gc);
        }
        base_next.assign(N * N * 4, {});
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                for (int dir = 0; dir < 4; dir++) {
                    const int sid = state_id(r, c, dir);
                    base_next[sid][0] = forward_state(sid);
                    base_next[sid][1] = state_id(r, c, (dir + 3) & 3);
                    base_next[sid][2] = state_id(r, c, (dir + 1) & 3);
                }
            }
        }
        // F 前進のべき乗テーブル（fwd_pow[k] = F を k 回適用）を前計算。
        const int S = N * N * 4;
        fwd_max = N;
        fwd_pow.assign(fwd_max + 1, vector<int>(S));
        for (int sid = 0; sid < S; sid++)
            fwd_pow[0][sid] = sid;
        for (int k = 1; k <= fwd_max; k++)
            for (int sid = 0; sid < S; sid++)
                fwd_pow[k][sid] = base_next[fwd_pow[k - 1][sid]][0];
    }
} in;

struct Cost {
    int a = inf32;
    int e = inf32;
};

inline bool operator<(const Cost &lhs, const Cost &rhs) {
    return lhs.a < rhs.a || (lhs.a == rhs.a && lhs.e < rhs.e);
}
inline bool operator==(const Cost &lhs, const Cost &rhs) {
    return lhs.a == rhs.a && lhs.e == rhs.e;
}
inline Cost add_cost(const Cost &lhs, const Cost &rhs) {
    if (lhs.a >= inf32 / 4 || rhs.a >= inf32 / 4)
        return {};
    return {lhs.a + rhs.a, lhs.e + rhs.e};
}

struct QueryDist {
    Cost cost;
    int end_sid = -1;
};

struct ParentStep {
    i16 prev = -1;
    char move = '?';
};

struct EvalResult {
    i64 score = inf64;
    int A = inf32;
    int E = inf32;
    vector<int> order;
    // このマクロの下界スコア（各ボール→担当籠の最短距離和）。
    // calc_score_full のキャッシュ復元時に bs を取り出すために保持する。
    i64 bs = inf64;
};

string compress_macro(const string &macro) {
    string res;
    const int n = (int)macro.size();
    for (int i = 0; i < n;) {
        if (macro[i] != 'L' && macro[i] != 'R') {
            res.push_back(macro[i]);
            i++;
            continue;
        }
        int rot = 0;
        int j = i;
        while (j < n && (macro[j] == 'L' || macro[j] == 'R')) {
            rot += (macro[j] == 'R') ? 1 : 3;
            j++;
        }
        rot &= 3;
        if (rot == 1) {
            res.push_back('R');
        } else if (rot == 2) {
            res.push_back('R');
            res.push_back('R');
        } else if (rot == 3) {
            res.push_back('L');
        }
        i = j;
    }
    return res;
}

// マクロの構造的表現。
// 文字列形は prefix + (F^len dir)* + suffix で、
//   - prefix: None / L / R （先頭の単一回転）
//   - mid:    {Fの長さ(>=1), 区切り回転 L/R} の列。
//             末尾(back)要素の dir は文字列化時に無視される。
//   - suffix: None / L / R / RR （末尾の回転）
// この規約により回転が連続して積み上がらず、圧縮操作は不要となる。
struct Macro {
    struct Mid {
        int len;  // >=1
        char dir; // 'L' or 'R'
    };
    char prefix = 0; // 0=None, 'L', 'R'
    vector<Mid> mid; // back の dir は to_string で無視される
    int suffix = 0;  // 0=None, 1=L, 2=R, 3=RR

    int length() const {
        int s = (prefix ? 1 : 0);
        for (const auto &m : mid)
            s += m.len;
        if (!mid.empty())
            s += (int)mid.size() - 1; // mid 間の区切り回転
        s += (suffix == 3) ? 2 : (suffix == 0 ? 0 : 1);
        return s;
    }

    string to_string() const {
        string s;
        s.reserve(length());
        if (prefix)
            s.push_back(prefix);
        const int k = (int)mid.size();
        for (int i = 0; i < k; i++) {
            s.append(mid[i].len, 'F');
            if (i + 1 < k)
                s.push_back(mid[i].dir); // back の LR は無視
        }
        if (suffix == 1)
            s.push_back('L');
        else if (suffix == 2)
            s.push_back('R');
        else if (suffix == 3) {
            s.push_back('R');
            s.push_back('R');
        }
        return s;
    }
};

inline char random_prefix_val() {
    const int r = rng.random32(3);
    return r == 0 ? 0 : (r == 1 ? 'L' : 'R');
}
inline int random_suffix_val() { return (int)rng.random32(4); }
inline char random_dir_val() { return rng.random32(2) ? 'R' : 'L'; }

Macro random_macro_struct() {
    const int lo = max(1, (int)(in.N * Tuning::random_macro_len_times));
    const int hi = in.N * 3;
    const int target_len = rng.randomRange32(lo, hi);
    const int fmin = 2;
    const int fmax = max(2, in.N);
    Macro m;
    m.prefix = random_prefix_val();
    m.suffix = random_suffix_val();
    // 長さ上限を守る範囲で mid を追加していく
    while (true) {
        Macro::Mid nm{(int)rng.randomRange32(fmin, fmax), random_dir_val()};
        m.mid.push_back(nm);
        if (m.length() > target_len) {
            m.mid.pop_back();
            break;
        }
    }
    return m;
}

// 任意の F/L/R 文字列を best-effort で Macro 構造へ変換する。
// canonical 形（prefix + (F^len 単一回転)* + suffix）はそのまま復元できる。
// 途中の U ターン(RR) など構造に収まらない部分は単一回転へ丸める(lossy)。
// 主にパスマクロ（最短経路文字列）やプール内のマクロ文字列を SA の初期解
// （macro_struct）に変換するために使う。
Macro macro_struct_from_string(const string &path) {
    const string s = compress_macro(path);
    const int n = (int)s.size();
    Macro m;
    int i = 0;
    auto read_turn_run = [&](int &i) -> int {
        int rot = 0;
        while (i < n && (s[i] == 'L' || s[i] == 'R')) {
            rot += (s[i] == 'R') ? 1 : 3;
            i++;
        }
        return rot & 3;
    };
    const int lead = read_turn_run(i);
    // F ラン列と、その直後の回転 rot(1..3) を集める。
    vector<pair<int, int>> runs; // (Fの長さ, 直後の回転 0..3)
    while (i < n && s[i] == 'F') {
        int len = 0;
        while (i < n && s[i] == 'F') {
            len++;
            i++;
        }
        const int rot = read_turn_run(i);
        runs.push_back({len, rot});
    }
    if (runs.empty()) {
        // F が無い: 回転だけ。lead を suffix として表現する。
        m.prefix = 0;
        m.suffix = (lead == 0) ? 0 : (lead == 1 ? 2 : (lead == 2 ? 3 : 1));
        return m;
    }
    // prefix: 単一回転のみ表現可能(RR は R へ丸める)。
    m.prefix = (lead == 1) ? 'R' : (lead == 3) ? 'L' : (lead == 2) ? 'R' : 0;
    const int k = (int)runs.size();
    for (int t = 0; t < k; t++) {
        if (t + 1 < k) {
            const int rot = runs[t].second;          // canonical では 1 or 3
            const char dir = (rot == 3) ? 'L' : 'R'; // RR(2) は R へ丸める
            m.mid.push_back({runs[t].first, dir});
        } else {
            // 最後の F ラン: back の dir は無視されるので何でも良い。
            m.mid.push_back({runs[t].first, 'R'});
            const int rot = runs[t].second;
            m.suffix = (rot == 0) ? 0 : (rot == 1 ? 2 : (rot == 2 ? 3 : 1));
        }
    }
    return m;
}

// マクロ生成にかかる追加コストの path 非依存ベースライン推定。
// MacroEvalContext::compute_macro_extra の path 非依存部分（best_extra）を
// 文字列のみから計算する軽量版。コスト下界の加算項として用いる。
inline int compute_macro_extra_baseline(const string &macro) {
    const int L = (int)macro.size();
    if (L == 0)
        return 0;
    int best_extra = L + 1; // x=0 ベースライン
    static thread_local vector<int> lcp_buf, dp_buf;
    const int st = L + 1;
    if ((int)lcp_buf.size() < st * st)
        lcp_buf.resize(st * st);
    int *__restrict lcp = lcp_buf.data();
    for (int j = 0; j <= L; j++)
        lcp[L * st + j] = 0;
    for (int i = 0; i <= L; i++)
        lcp[i * st + L] = 0;
    for (int i = L - 1; i >= 0; i--)
        for (int j = L - 1; j >= 0; j--)
            lcp[i * st + j] =
                (macro[i] == macro[j]) ? lcp[(i + 1) * st + (j + 1)] + 1 : 0;
    if ((int)dp_buf.size() < L + 1)
        dp_buf.resize(L + 1);
    int *__restrict dp = dp_buf.data();
    const int len_cap = min(Tuning::path_implicit_max_len, L / 2);
    for (int a = 0; a < L; a++) {
        for (int len = 1; a + len <= L && len <= len_cap; len++) {
            dp[L] = 0;
            for (int i = L - 1; i >= 0; i--) {
                int v = dp[i + 1] + 1;
                if (i + len <= L && lcp[i * st + a] >= len)
                    smin(v, dp[i + len] + 1);
                dp[i] = v;
            }
            smin(best_extra, len + dp[0] + 5);
        }
    }
    return best_extra;
}

struct MacroEvalContext {
    string macro;
    int macro_extra_baseline = 0; // x=0 のときの追加コスト = len(macro)+1
    // macro_extra[b] = ボール b が order の先頭(=最初のボール)のときの、
    // メインマクロ登録の追加コスト推定。初期地点→b の自然経路(復元経路)に
    // init_macro/main_macro が出現すれば登録コストが安くなることを反映する。
    vector<int> macro_extra;
    // compute_macro_extra 用 LCP バッファ（毎 build の再確保を避け使い回す）。
    // フラット配列で (L+1)*(L+1) を表現し、行ストライドは lcp_stride。
    vector<int> lcp_buf;
    int lcp_stride = 0;
    vector<int> macro_extra_dp_buf;
    // encode_len(macro, macro[a:a+len]) のボール非依存値を保持する作業領域。
    // enc_buf[a*enc_stride + len]（len は 1..len_cap）。
    vector<int> macro_extra_enc_buf;
    int macro_extra_enc_stride = 0;
    // 各ボールの part1_ops(初期地点→ボール の自然経路, 最初のP直前まで) に
    // macro[a:] が部分文字列として何文字一致するかの最大長 appear_len[a]。
    vector<int> macro_extra_appear_buf;
    // appear_len 計算の LCP2 ロー(2 行ローリング)。長さ L+1。
    vector<int> macro_extra_row0, macro_extra_row1;
    vector<array<int, 4>> next4;
    vector<int> sources;
    vector<int> sid_to_source;
    vector<vector<Cost>> dist;
    vector<vector<int>> dist_stamp;
    vector<vector<ParentStep>> parent;
    vector<vector<int>> parent_stamp;
    vector<int> bfs_buf;
    int build_gen = 0;
    bool sources_built = false;

    // 向き seed 用：同一セルの4向きソースをまとめたグループ。
    // dir_groups[g] = {dir0,dir1,dir2,dir3 の
    // source_idx}（全て有効=4向き揃い）。 group_is_early[g] =
    // そのセルが早期終了対象（ボール始点）か。
    vector<array<int, 4>> dir_groups;
    vector<char> group_is_early;
    vector<char>
        source_in_group; // source_idx -> グループ所属で通常ループ対象外か
    // VERIFY 用：参照フルBFSの作業領域。
    vector<Cost> verify_dist;
    vector<int> verify_stamp;
    vector<int> verify_q;

    // BFS早期終了（枝刈り）用（ソース固定なので初回のみ構築）。
    // 1:1対応のボール始点ソースは目的セル（対応かご）が1個だけなので、
    // そこへ到達した距離の層までで打ち切れる。start/かご始点は目的が
    // 全ボールセルに散るため枝刈りせず全域探索する。
    vector<char> need_all_balls; // src_idx -> 全ボール必要(start/かご始点)か
    vector<int>
        need_basket_cell; // src_idx -> ボール始点の対応かごセル(-1なら非該当)

    // ウェイポイント間遷移コストテーブル（evaluate_order高速化用）
    // wp_cells[i] = i番目ウェイポイントのcell_id
    // trans_cost[fi][ti][d1*4+d2] = wp[fi] を dir d1 で出発し wp[ti] へ dir d2
    // で到達するコスト
    vector<int> wp_cells;
    vector<int> cell_to_wp; // cell_id -> wp index (-1なら非ウェイポイント)
    int start_wp = 0;
    vector<int> ball_wp;
    vector<int> basket_wp;
    vector<vector<array<Cost, 16>>> trans_cost;
    mutable vector<vector<double>> wp_dirless_dist;
    mutable vector<AliasWeightedSampler> insert_prev_sampler;
    mutable vector<AliasWeightedSampler> insert_next_sampler;
    // ensure_order_sampling_tables 用の使い回しバッファ（毎build の一時 vector
    // 確保を避ける）。サイズは入力で固定。
    mutable vector<double> wprev_buf;
    mutable vector<double> wnext_buf;
    mutable bool order_sampling_tables_ready = false;

    MacroEvalContext() = default;
    explicit MacroEvalContext(const string &macro_) { build(macro_); }

    // メインマクロ登録の追加コスト（経路 A に加算）の上界を見積もる。
    // ベースライン（init_macro を使わず main_macro を直接登録）は macro_len+1
    // （M+macro+M = macro_len+2 だが、経路が既に初回 P を 1 と数えているため
    //  その P を消費する分 -1）。
    // さらに「初手で init_macro（= main_macro の部分文字列）を登録し、それを
    // 使って main_macro を録音する」場合の追加コスト上界を、全部分文字列に
    // ついて見積もり最小を採る:
    //   init_macro.size() + 4          初手 init_macro 登録の概算
    //   + encode_len(macro, init_macro) init_macro を P として使った録音長
    //   + 2                            録音を囲む M..M
    //   - 1                            初回 P を消費
    //   = len + encode_len + 5
    // 実際の後処理は経路前半も init_macro で圧縮しさらに縮むため、この値は
    // 真の追加コストの上界（安全側）になる。macro 長は高々 ~2N+α と短いので
    // LCP テーブル + DP の O(L^3) で十分。
    //
    // main_14: さらに「初期地点→最初のボール の自然経路 part1_ops に
    // init_macro/main_macro が部分文字列として出現する」場合を考慮する。
    // 出現すれば、その区間は経路上で元々実行されるので、init_macro 登録は
    // M..M で囲むだけ(+2)で済み、`init_macro.size()+4` の登録コストが不要に
    // なる(use_substring_macro_before_main_registration の実処理に対応)。
    //   in-path 部分文字列: term = encode_len + 3   (登録+2, 録音 M..M +2,
    //   初回P -1) main_macro 全体が出現: term = 2             (M..M で囲むだけ)
    // 「最初のボール」は order に依存するため、ボール別テーブル macro_extra[b]
    // を作る。encode_len はボール非依存なので一度だけ計算して enc[a][len] に
    // 保持し、ボール別ループでは出現判定(appear_len)と min 走査のみ行う。
    void compute_macro_extra() {
        macro_extra.assign(in.M, 0);
        if (macro.empty()) {
            macro_extra_baseline = 0;
            return;
        }
        const int L = (int)macro.size();
        int best_extra = L + 1; // x=0 ベースライン(path 非依存)

        // lcp[i][j] = macro[i:] と macro[j:] の最長共通接頭辞長。
        // init_macro = macro[a:a+len] が位置 i で一致 ⇔ lcp[i][a] >= len。
        // フラットバッファを使い回し、毎 build のヒープ再確保を避ける。
        lcp_stride = L + 1;
        if ((int)lcp_buf.size() < lcp_stride * lcp_stride)
            lcp_buf.resize(lcp_stride * lcp_stride);
        if ((int)macro_extra_dp_buf.size() < L + 1)
            macro_extra_dp_buf.resize(L + 1);
        int *__restrict lcp = lcp_buf.data();
        const int st = lcp_stride;
        // i==L / j==L の行・列は 0（接尾辞の番兵）。
        for (int j = 0; j <= L; j++)
            lcp[L * st + j] = 0;
        for (int i = 0; i <= L; i++)
            lcp[i * st + L] = 0;
        for (int i = L - 1; i >= 0; i--)
            for (int j = L - 1; j >= 0; j--)
                lcp[i * st + j] = (macro[i] == macro[j])
                                      ? lcp[(i + 1) * st + (j + 1)] + 1
                                      : 0;

        int *__restrict dp = macro_extra_dp_buf.data();
        // init_macro 長の上限は他箇所と統一して min(implicit_max_len, L/2)。
        const int len_cap = min(Tuning::path_implicit_max_len, L / 2);
        // enc[a*es+len] = encode_len(macro, macro[a:a+len])（ボール非依存）。
        macro_extra_enc_stride = len_cap + 1;
        const int es = macro_extra_enc_stride;
        if ((int)macro_extra_enc_buf.size() < L * es)
            macro_extra_enc_buf.resize(L * es);
        int *__restrict enc = macro_extra_enc_buf.data();
        for (int a = 0; a < L; a++) {
            for (int len = 1; a + len <= L && len <= len_cap; len++) {
                // encode_len: macro を、登録マクロ macro[a:a+len] を P で
                // 置換しながら表現する最小トークン数。
                dp[L] = 0;
                for (int i = L - 1; i >= 0; i--) {
                    int v = dp[i + 1] + 1;
                    if (i + len <= L && lcp[i * st + a] >= len)
                        smin(v, dp[i + len] + 1);
                    dp[i] = v;
                }
                const int e = dp[0];
                enc[a * es + len] = e;
                smin(best_extra, len + e + 5); // path 非依存ケース
            }
        }
        macro_extra_baseline = best_extra;

        // ---- ボール別: 初期地点→b の自然経路に macro 部分文字列が出現するか
        // ----
        const int start_src = sid_to_source[in.state_id(0, 0, 0)];
        if (start_src < 0) {
            for (int b = 0; b < in.M; b++)
                macro_extra[b] = best_extra;
            return;
        }
        if ((int)macro_extra_appear_buf.size() < L)
            macro_extra_appear_buf.resize(L);
        if ((int)macro_extra_row0.size() < L + 1)
            macro_extra_row0.resize(L + 1);
        if ((int)macro_extra_row1.size() < L + 1)
            macro_extra_row1.resize(L + 1);
        int *__restrict appear = macro_extra_appear_buf.data();
        const char *__restrict mac = macro.data();

        for (int b = 0; b < in.M; b++) {
            // start→ball[b] の a 最小経路を復元し、最初の 'P' 直前までを
            // part1_ops とする（実処理: 最初のマクロ使用より前の自然経路）。
            const QueryDist qd = best_to_cell(start_src, in.ball_cells[b]);
            if (qd.end_sid < 0) {
                macro_extra[b] = best_extra;
                continue;
            }
            const string raw = restore_path(start_src, qd.end_sid);
            int plen = 0;
            while (plen < (int)raw.size() && raw[plen] != 'P')
                plen++;
            const char *__restrict p1 = raw.data();

            // appear[a] = macro[a:] が part1(=p1[0:plen]) に部分文字列として
            // 一致する最大長。LCP2 を s について逆順に 2 行ローリングで計算:
            //   lcp2[s][a] = (p1[s]==macro[a]) ? lcp2[s+1][a+1]+1 : 0
            //   appear[a]  = max_s lcp2[s][a]
            for (int a = 0; a < L; a++)
                appear[a] = 0;
            int *__restrict nxt = macro_extra_row0.data();
            int *__restrict cur = macro_extra_row1.data();
            for (int a = 0; a <= L; a++)
                nxt[a] = 0;
            for (int s = plen - 1; s >= 0; s--) {
                cur[L] = 0;
                const char cs = p1[s];
                for (int a = L - 1; a >= 0; a--) {
                    const int v = (cs == mac[a]) ? nxt[a + 1] + 1 : 0;
                    cur[a] = v;
                    if (v > appear[a])
                        appear[a] = v;
                }
                std::swap(cur, nxt);
            }

            int extra_b = best_extra;
            // Case A: main_macro 全体が出現 → M..M で囲むだけ(+2)
            if (appear[0] >= L)
                smin(extra_b, 2);
            // 部分文字列ケース: in-path なら登録コストを省ける(enc+3)
            for (int a = 0; a < L; a++) {
                const int lmax = min(len_cap, L - a);
                const int ap = appear[a];
                const int upper = min(lmax, ap);
                for (int len = 1; len <= upper; len++)
                    smin(extra_b, enc[a * es + len] + 3);
            }
            macro_extra[b] = extra_b;
        }
    }

    // order の先頭ボール first_ball に対応する追加コストを返す。
    // first_ball<0(空 order など)や範囲外のときは path
    // 非依存ベースラインを使う。
    int macro_extra_at(int first_ball) const {
        if (first_ball < 0 || first_ball >= (int)macro_extra.size())
            return macro_extra_baseline;
        return macro_extra[first_ball];
    }

    Cost edge_cost(char op) const {
        if (op == 'P')
            return {1, (int)macro.size()};
        return {1, 1};
    }

    int simulate_macro(int sid) const {
        int cur = sid;
        for (char ch : macro) {
            if (ch == 'F') {
                cur = in.base_next[cur][0];
            } else if (ch == 'L') {
                cur = in.base_next[cur][1];
            } else {
                cur = in.base_next[cur][2];
            }
        }
        return cur;
    }

    void build_sources() {
        sid_to_source.assign(in.N * in.N * 4, -1);
        sources.clear();
        auto add_source = [&](int sid) {
            if (sid_to_source[sid] != -1)
                return;
            sid_to_source[sid] = (int)sources.size();
            sources.push_back(sid);
        };
        add_source(in.state_id(0, 0, 0));
        for (int i = 0; i < in.M; i++) {
            const auto &[br, bc] = in.balls[i];
            const auto &[gr, gc] = in.baskets[i];
            for (int dir = 0; dir < 4; dir++) {
                add_source(in.state_id(br, bc, dir));
                add_source(in.state_id(gr, gc, dir));
            }
        }

        // ウェイポイントテーブル構築（evaluate_order高速化）
        cell_to_wp.assign(in.N * in.N, -1);
        wp_cells.clear();
        auto add_wp = [&](int cell) {
            if (cell_to_wp[cell] == -1) {
                cell_to_wp[cell] = (int)wp_cells.size();
                wp_cells.push_back(cell);
            }
        };
        add_wp(in.cell_id(0, 0));
        for (int i = 0; i < in.M; i++) {
            add_wp(in.ball_cells[i]);
            add_wp(in.basket_cells[i]);
        }
        start_wp = cell_to_wp[in.cell_id(0, 0)];
        ball_wp.resize(in.M);
        basket_wp.resize(in.M);
        for (int i = 0; i < in.M; i++) {
            ball_wp[i] = cell_to_wp[in.ball_cells[i]];
            basket_wp[i] = cell_to_wp[in.basket_cells[i]];
        }
        const int W = (int)wp_cells.size();
        trans_cost.assign(W, vector<array<Cost, 16>>(W));

        // 早期終了（枝刈り）用テーブルの構築
        const int NC = in.N * in.N;
        vector<int> ball_idx_of_cell(NC, -1);
        vector<char> is_basket_cell(NC, 0);
        for (int i = 0; i < in.M; i++) {
            ball_idx_of_cell[in.ball_cells[i]] = i;
            is_basket_cell[in.basket_cells[i]] = 1;
        }
        const int start_cell = in.cell_id(0, 0);

        const int ns = (int)sources.size();
        need_all_balls.assign(ns, 0);
        need_basket_cell.assign(ns, -1);
        for (int s = 0; s < ns; s++) {
            const int cell = sources[s] >> 2;
            // 初期地点・かご始点は全ボールセルへの距離が必要（枝刈り対象外）
            need_all_balls[s] =
                ((cell == start_cell) || is_basket_cell[cell]) ? 1 : 0;
            // ボール始点は対応するかごセルのみが目的
            if (ball_idx_of_cell[cell] != -1)
                need_basket_cell[s] = in.basket_cells[ball_idx_of_cell[cell]];
        }

        // 向き seed 用グループ構築：4向き揃ったセル（=ボール/かごセル）のみ。
        // start セル(0,0)は dir0 のみのことが多く、4向き揃わなければグループ化
        // しない（通常ループで処理）。
        dir_groups.clear();
        group_is_early.clear();
        source_in_group.assign(ns, 0);
        {
            vector<char> cell_seen(in.N * in.N, 0);
            for (int s = 0; s < ns; s++) {
                const int cell = sources[s] >> 2;
                if (cell_seen[cell])
                    continue;
                cell_seen[cell] = 1;
                array<int, 4> grp{-1, -1, -1, -1};
                bool full = true;
                for (int dir = 0; dir < 4; dir++) {
                    const int sid = (cell << 2) | dir;
                    const int si = sid_to_source[sid];
                    if (si == -1) {
                        full = false;
                        break;
                    }
                    grp[dir] = si;
                }
                if (!full)
                    continue;
                // need_all_balls==0 のセル（=ボール始点で start
                // でない）が早期終了対象。
                const bool early = (need_all_balls[grp[0]] == 0);
                dir_groups.push_back(grp);
                group_is_early.push_back(early ? 1 : 0);
                for (int dir = 0; dir < 4; dir++)
                    source_in_group[grp[dir]] = 1;
            }
        }
    }

    // ④ track_parent をテンプレート化してホットパスから分岐を除去
    template <bool track_parent> void run_bfs_impl(int src_idx) {
        int q_head = 0, q_tail = 0;
        const int src_sid = sources[src_idx];
        const int need_bk = (track_parent || need_all_balls[src_idx])
                                ? -1
                                : need_basket_cell[src_idx];
        const bool early = need_bk != -1;
        int stop_a = inf32;
        // src_idx はループ不変なので外側 vector の二重インダイレクションを
        // ホイストして生ポインタ経由でアクセスする
        Cost *__restrict dist_s = dist[src_idx].data();
        int *__restrict dstamp_s = dist_stamp[src_idx].data();
        ParentStep *__restrict par_s = parent[src_idx].data();
        int *__restrict pstamp_s = parent_stamp[src_idx].data();
        const int cur_gen = build_gen;
        dist_s[src_sid] = {0, 0};
        dstamp_s[src_sid] = cur_gen;
        if constexpr (track_parent) {
            par_s[src_sid] = {};
            pstamp_s[src_sid] = cur_gen;
        }
        bfs_buf[q_tail++] = src_sid;
        const int macro_e = (int)macro.size();
        static constexpr char ops[4] = {'F', 'L', 'R', 'P'};

        // ② unvisited は常に better なので Cost{} 生成・better 判定を削除
        // ③ F/L/R (e+=1) と P (e+=macro_e) を分割して三項演算子を除去
        while (q_head < q_tail) {
            const int sid = bfs_buf[q_head++];
            const Cost cur_dist = dist_s[sid];
            if (early && cur_dist.a >= stop_a)
                break;
            const auto &nexts = next4[sid];
            const int nd_a = cur_dist.a + 1;

            // F, L, R: nd_e = cur_dist.e + 1
            // main_13: a のみ最小化。e
            // は最初に到達した経路の値を記録するだけで、 同一 a 層内での e
            // 最小化（revisit 更新の else-if 分岐）は行わない。
            const int nd_e1 = cur_dist.e + 1;
            for (int k = 0; k < 3; k++) {
                static constexpr char ops3[3] = {'F', 'L', 'R'};
                const int to = nexts[k];
                if (dstamp_s[to] != cur_gen) {
                    dist_s[to] = {nd_a, nd_e1};
                    dstamp_s[to] = cur_gen;
                    if constexpr (track_parent) {
                        par_s[to] = {(i16)sid, ops3[k]};
                        pstamp_s[to] = cur_gen;
                    }
                    bfs_buf[q_tail++] = to;
                    if (early && stop_a == inf32 && (to >> 2) == need_bk)
                        stop_a = nd_a + 1;
                }
            }

            // P: nd_e = cur_dist.e + macro_e（マクロ登録済みのときのみ）
            if (macro_e > 0) {
                const int to = nexts[3];
                const int nd_ep = cur_dist.e + macro_e;
                if (dstamp_s[to] != cur_gen) {
                    dist_s[to] = {nd_a, nd_ep};
                    dstamp_s[to] = cur_gen;
                    if constexpr (track_parent) {
                        par_s[to] = {(i16)sid, 'P'};
                        pstamp_s[to] = cur_gen;
                    }
                    bfs_buf[q_tail++] = to;
                    if (early && stop_a == inf32 && (to >> 2) == need_bk)
                        stop_a = nd_a + 1;
                }
            }
        }
    }

    void run_bfs(int src_idx, bool track_parent) {
        if (track_parent)
            run_bfs_impl<true>(src_idx);
        else
            run_bfs_impl<false>(src_idx);
    }

    // 向き seed 付き BFS。
    //   seeds      : (兄弟 source_idx, 回転コスト rot)
    //   の列（最大3）。空でフルBFS。 allow_early:
    //   早期終了（need_basket_cell）を併用するか。
    // seed は table_{d'} + rot を ceiling として dist
    // に流し込み、真のソースのみ キュー投入。「nd_a < 既存a」のときのみ push
    // する label-correcting だが、 seed テーブルが全辺整合（厳密最短）なので
    // seed ノードを展開する必要はなく、 plain FIFO のまま厳密に最短 a
    // が求まる。
    template <bool track_parent, int NSEED>
    void run_bfs_seeded_impl(int src_idx, const array<pair<int, int>, 3> &seeds,
                             bool allow_early) {
        const int S = in.N * in.N * 4;
        const int src_sid = sources[src_idx];
        const int cur_gen = build_gen;
        Cost *__restrict dist_s = dist[src_idx].data();
        int *__restrict dstamp_s = dist_stamp[src_idx].data();
        ParentStep *__restrict par_s = parent[src_idx].data();
        int *__restrict pstamp_s = parent_stamp[src_idx].data();

        // --- seed 流し込み（ceiling） ---
        if constexpr (NSEED > 0) {
            const Cost *sd[3];
            const int *sst[3];
            int srot[3];
            for (int t = 0; t < NSEED; t++) {
                sd[t] = dist[seeds[t].first].data();
                sst[t] = dist_stamp[seeds[t].first].data();
                srot[t] = seeds[t].second;
            }
            for (int sid = 0; sid < S; sid++) {
                int ba = inf32, be = inf32;
                for (int t = 0; t < NSEED; t++) {
                    if (sst[t][sid] != cur_gen)
                        continue;
                    const int ca = sd[t][sid].a + srot[t];
                    const int ce = sd[t][sid].e + srot[t];
                    if (ca < ba || (ca == ba && ce < be)) {
                        ba = ca;
                        be = ce;
                    }
                }
                if (ba < inf32) {
                    dist_s[sid] = {ba, be};
                    dstamp_s[sid] = cur_gen;
                }
            }
        }

        // --- 早期終了の準備 ---
        const int need_bk =
            (allow_early && !track_parent && !need_all_balls[src_idx])
                ? need_basket_cell[src_idx]
                : -1;
        const bool early = need_bk != -1;
        int stop_a = inf32;
        if (early) {
            // seed 済みの need_bk セル各向きから stop_a の初期上界を取る。
            for (int dir = 0; dir < 4; dir++) {
                const int sid = (need_bk << 2) | dir;
                if (dstamp_s[sid] == cur_gen && dist_s[sid].a + 1 < stop_a)
                    stop_a = dist_s[sid].a + 1;
            }
        }

        // --- 真のソースを 0 で上書きしてキュー投入 ---
        int q_head = 0, q_tail = 0;
        dist_s[src_sid] = {0, 0};
        dstamp_s[src_sid] = cur_gen;
        if constexpr (track_parent) {
            par_s[src_sid] = {};
            pstamp_s[src_sid] = cur_gen;
        }
        bfs_buf[q_tail++] = src_sid;
        const int macro_e = (int)macro.size();

        while (q_head < q_tail) {
            const int sid = bfs_buf[q_head++];
            const Cost cur_dist = dist_s[sid];
            if (early && cur_dist.a >= stop_a)
                break;
            const auto &nexts = next4[sid];
            const int nd_a = cur_dist.a + 1;

            const int nd_e1 = cur_dist.e + 1;
            for (int k = 0; k < 3; k++) {
                static constexpr char ops3[3] = {'F', 'L', 'R'};
                const int to = nexts[k];
                const int cur_a =
                    (dstamp_s[to] == cur_gen) ? dist_s[to].a : inf32;
                if (nd_a < cur_a) {
                    dist_s[to] = {nd_a, nd_e1};
                    dstamp_s[to] = cur_gen;
                    if constexpr (track_parent) {
                        par_s[to] = {(i16)sid, ops3[k]};
                        pstamp_s[to] = cur_gen;
                    }
                    bfs_buf[q_tail++] = to;
                    if (early && (to >> 2) == need_bk && nd_a + 1 < stop_a)
                        stop_a = nd_a + 1;
                }
            }

            if (macro_e > 0) {
                const int to = nexts[3];
                const int nd_ep = cur_dist.e + macro_e;
                const int cur_a =
                    (dstamp_s[to] == cur_gen) ? dist_s[to].a : inf32;
                if (nd_a < cur_a) {
                    dist_s[to] = {nd_a, nd_ep};
                    dstamp_s[to] = cur_gen;
                    if constexpr (track_parent) {
                        par_s[to] = {(i16)sid, 'P'};
                        pstamp_s[to] = cur_gen;
                    }
                    bfs_buf[q_tail++] = to;
                    if (early && (to >> 2) == need_bk && nd_a + 1 < stop_a)
                        stop_a = nd_a + 1;
                }
            }
        }
    }

    // 非early グループ専用の高速 seeded BFS。
    // 全兄弟表が dense（全 S stamp 済み）であることを前提に、seed-scan の stamp
    // 分岐と BFS 内ループの stamp ロードを除去する。track_parent / early
    // は無し。
    template <int NSEED>
    void run_bfs_seeded_full(int src_idx,
                             const array<pair<int, int>, 3> &seeds) {
        const int S = in.N * in.N * 4;
        const int src_sid = sources[src_idx];
        const int cur_gen = build_gen;
        Cost *__restrict dist_s = dist[src_idx].data();
        int *__restrict dstamp_s = dist_stamp[src_idx].data();

        const Cost *__restrict s0 = dist[seeds[0].first].data();
        const int r0 = seeds[0].second;
        if constexpr (NSEED == 1) {
            for (int sid = 0; sid < S; sid++) {
                dist_s[sid] = {s0[sid].a + r0, s0[sid].e + r0};
                dstamp_s[sid] = cur_gen;
            }
        } else { // NSEED == 2
            const Cost *__restrict s1 = dist[seeds[1].first].data();
            const int r1 = seeds[1].second;
            for (int sid = 0; sid < S; sid++) {
                const int a0 = s0[sid].a + r0;
                const int a1 = s1[sid].a + r1;
                if (a0 <= a1)
                    dist_s[sid] = {a0, s0[sid].e + r0};
                else
                    dist_s[sid] = {a1, s1[sid].e + r1};
                dstamp_s[sid] = cur_gen;
            }
        }

        int q_head = 0, q_tail = 0;
        dist_s[src_sid] = {0, 0};
        bfs_buf[q_tail++] = src_sid;
        const int macro_e = (int)macro.size();
        while (q_head < q_tail) {
            const int sid = bfs_buf[q_head++];
            const Cost cur_dist = dist_s[sid];
            const auto &nexts = next4[sid];
            const int nd_a = cur_dist.a + 1;
            const int nd_e1 = cur_dist.e + 1;
            for (int k = 0; k < 3; k++) {
                const int to = nexts[k];
                if (nd_a < dist_s[to].a) {
                    dist_s[to] = {nd_a, nd_e1};
                    bfs_buf[q_tail++] = to;
                }
            }
            if (macro_e > 0) {
                const int to = nexts[3];
                if (nd_a < dist_s[to].a) {
                    dist_s[to] = {nd_a, cur_dist.e + macro_e};
                    bfs_buf[q_tail++] = to;
                }
            }
        }
    }

    // NSEED を実引数からディスパッチ。
    template <bool track_parent>
    void run_bfs_seeded(int src_idx, const array<pair<int, int>, 3> &seeds,
                        int nseed, bool allow_early) {
        switch (nseed) {
        case 0:
            run_bfs_seeded_impl<track_parent, 0>(src_idx, seeds, allow_early);
            break;
        case 1:
            run_bfs_seeded_impl<track_parent, 1>(src_idx, seeds, allow_early);
            break;
        case 2:
            run_bfs_seeded_impl<track_parent, 2>(src_idx, seeds, allow_early);
            break;
        default:
            run_bfs_seeded_impl<track_parent, 3>(src_idx, seeds, allow_early);
            break;
        }
    }

    static int rot_cost(int d1, int d2) {
        const int a = (d1 - d2) & 3;
        return min(a, 4 - a); // 0,1,2,1 -> 0,1,2
    }

    // 1グループ（同一セル4向き）を seed 共有で処理。
    // dir0 はフルBFS（seed 基底）。dir1..3 は確定済み兄弟からの min-seed。
    // start_src を含む dir0 のみ track_parent。
    void run_bfs_group(const array<int, 4> &grp, bool is_early, int start_src) {
        // VERIFY 時は全域フル化して参照と比較するため早期終了を切る。
        const bool early_dirs = is_early && SEED_FOR_EARLY && !VERIFY_DIR_SEED;

        // dir0：seed 基底のフルBFS。元の最速 impl をそのまま使う。
        // （かごセルは need_all_balls=1 で need_bk=-1 となり全域探索になる。
        //   ball セルを SEED_FOR_EARLY で混ぜる場合のみ、打ち切りを避けるため
        //   seeded(NSEED=0) 経由でフル化する。）
        {
            const int s0 = grp[0];
            if (s0 == start_src)
                run_bfs_impl<true>(s0);
            else if (!is_early)
                run_bfs_impl<false>(s0);
            else
                run_bfs_seeded<false>(s0, {}, 0, false);
        }

        for (int d = 1; d < 4; d++) {
            const int src_idx = grp[d];
            array<pair<int, int>, 3> seeds{};
            if (!is_early) {
                // 厳密フル dense 表前提の支配関係で最小 seed 集合を使い、
                // stamp 分岐を除去した高速パスを通す。
                if (d == 1) {
                    seeds[0] = {grp[0], 1};
                    run_bfs_seeded_full<1>(src_idx, seeds);
                } else if (d == 2) {
                    seeds[0] = {grp[1], 1};
                    run_bfs_seeded_full<1>(src_idx, seeds);
                } else { // d == 3
                    seeds[0] = {grp[2], 1};
                    seeds[1] = {grp[0], 1};
                    run_bfs_seeded_full<2>(src_idx, seeds);
                }
            } else {
                // 打ち切りで表が切り詰まり得るため、全兄弟から安全側で seed。
                int nseed = 0;
                for (int dp = 0; dp < d; dp++)
                    seeds[nseed++] = {grp[dp], rot_cost(d, dp)};
                // dir1..3 は start_src（dir0）になり得ないので
                // track_parent=false。
                run_bfs_seeded<false>(src_idx, seeds, nseed, early_dirs);
            }
        }
    }

    // 検証用：各ソースを参照フルBFS（seed無し・打ち切り無し）で再計算し、
    // 生成済み dist テーブルの a が一致するか assert する。
    void verify_dir_seed() {
        const int S = in.N * in.N * 4;
        const int macro_e = (int)macro.size();
        verify_dist.assign(S, Cost{});
        verify_stamp.assign(S, 0);
        verify_q.resize(S);
        int vgen = 0;
        for (int src_idx = 0; src_idx < (int)sources.size(); src_idx++) {
            vgen++;
            const int src_sid = sources[src_idx];
            int h = 0, t = 0;
            verify_dist[src_sid].a = 0;
            verify_stamp[src_sid] = vgen;
            verify_q[t++] = src_sid;
            while (h < t) {
                const int sid = verify_q[h++];
                const int na = verify_dist[sid].a + 1;
                const auto &nx = next4[sid];
                const int lim = (macro_e > 0) ? 4 : 3;
                for (int k = 0; k < lim; k++) {
                    const int to = nx[k];
                    if (verify_stamp[to] != vgen) {
                        verify_stamp[to] = vgen;
                        verify_dist[to].a = na;
                        verify_q[t++] = to;
                    }
                }
            }
            // 比較：到達可能集合と a の一致。
            for (int sid = 0; sid < S; sid++) {
                const bool ref_ok = (verify_stamp[sid] == vgen);
                const bool prod_ok = (dist_stamp[src_idx][sid] == build_gen);
                assert(ref_ok == prod_ok);
                if (ref_ok)
                    assert(dist[src_idx][sid].a == verify_dist[sid].a);
            }
        }
    }

    QueryDist best_to_cell(int src_idx, int cell) const {
        QueryDist best;
        for (int dir = 0; dir < 4; dir++) {
            const int sid = (cell << 2) | dir;
            if (dist_stamp[src_idx][sid] != build_gen)
                continue;
            const Cost &cur = dist[src_idx][sid];
            if (cur < best.cost) {
                best.cost = cur;
                best.end_sid = sid;
            }
        }
        return best;
    }

    double dirless_dist(int from_wp, int to_wp) const {
        ensure_order_sampling_tables();
        return wp_dirless_dist[from_wp][to_wp];
    }

    double inv_dirless_dist(int from_wp, int to_wp) const {
        return 1.0 / (1.0 + order_weight_dist(dirless_dist(from_wp, to_wp)));
    }

    double order_weight_dist(double dist) const { return dist * dist * dist; }

    void ensure_order_sampling_tables() const {
        if (order_sampling_tables_ready)
            return;
        const int W = (int)wp_cells.size();
        // 再確保を避け、外側・内側 vector の capacity を保持したまま 1e9
        // で埋める （W は入力で固定なので 2 回目以降は resize は no-op）。
        wp_dirless_dist.resize(W);
        for (auto &row : wp_dirless_dist) {
            row.resize(W);
            std::fill(row.begin(), row.end(), 1e9);
        }
        for (int fi = 0; fi < W; fi++) {
            for (int ti = 0; ti < W; ti++) {
                double sum = 0.0;
                int cnt = 0;
                for (const Cost &c : trans_cost[fi][ti]) {
                    if (c.a >= inf32 / 4)
                        continue;
                    sum += c.a;
                    cnt++;
                }
                if (cnt > 0)
                    wp_dirless_dist[fi][ti] = sum / cnt;
            }
        }

        auto inv_dist = [&](int from_wp, int to_wp) {
            return 1.0 /
                   (1.0 + order_weight_dist(wp_dirless_dist[from_wp][to_wp]));
        };
        insert_prev_sampler.resize(in.M);
        insert_next_sampler.resize(in.M);
        // build() は weights を内部にコピーするので、使い回しバッファを渡せる。
        wprev_buf.resize(in.M + 1);
        wnext_buf.resize(in.M);
        for (int b = 0; b < in.M; b++) {
            wprev_buf[0] = inv_dist(start_wp, ball_wp[b]); // ref=-1
            for (int prev = 0; prev < in.M; prev++) {
                wprev_buf[prev + 1] = inv_dist(basket_wp[prev], ball_wp[b]);
            }
            insert_prev_sampler[b].build(wprev_buf);
        }
        for (int x = 0; x < in.M; x++) {
            for (int nxt = 0; nxt < in.M; nxt++) {
                wnext_buf[nxt] = inv_dist(basket_wp[x], ball_wp[nxt]);
            }
            insert_next_sampler[x].build(wnext_buf);
        }
        order_sampling_tables_ready = true;
    }

    // build/build_lb の共通前段。build_gen を進め、macro 変換テーブル next4 と
    // ストレージを構築する。
    void prepare_transform(const string &macro_) {
        if constexpr (DEBUG)
            g_prof.count_build++;
        build_gen++;
        macro = macro_;
        order_sampling_tables_ready = false;
        const int S = in.N * in.N * 4;
        {
            chrono::steady_clock::time_point t0;
            if constexpr (DEBUG)
                t0 = chrono::steady_clock::now();
            if ((int)next4.size() < S) {
                next4.resize(S);
                for (int sid = 0; sid < S; sid++) {
                    next4[sid][0] = in.base_next[sid][0];
                    next4[sid][1] = in.base_next[sid][1];
                    next4[sid][2] = in.base_next[sid][2];
                }
            }
            for (int sid = 0; sid < S; sid++)
                next4[sid][3] = sid;
            // F の連続は in.fwd_pow で 1 回のテーブル参照に畳む（連長 N
            // でクリップ）。
            {
                const int n = (int)macro.size();
                int i = 0;
                while (i < n) {
                    if (macro[i] == 'F') {
                        int run = 0;
                        while (i < n && macro[i] == 'F') {
                            run++;
                            i++;
                        }
                        if (run > in.fwd_max)
                            run = in.fwd_max;
                        const vector<int> &fp = in.fwd_pow[run];
                        for (int sid = 0; sid < S; sid++)
                            next4[sid][3] = fp[next4[sid][3]];
                    } else {
                        const int op = (macro[i] == 'L') ? 1 : 2;
                        for (int sid = 0; sid < S; sid++)
                            next4[sid][3] = in.base_next[next4[sid][3]][op];
                        i++;
                    }
                }
            }
            // sources/dist/bfs_buf は初回のみ確保（sources は固定）
            if (!sources_built) {
                build_sources();
                sources_built = true;
                const int ns = (int)sources.size();
                dist.assign(ns, vector<Cost>(S));
                dist_stamp.assign(ns, vector<int>(S, 0));
                parent.assign(ns, vector<ParentStep>(S));
                parent_stamp.assign(ns, vector<int>(S, 0));
                bfs_buf.resize(S * 5);
            }
            if constexpr (DEBUG)
                g_prof.time_macro_next +=
                    chrono::duration<double>(chrono::steady_clock::now() - t0)
                        .count();
        }
    }

    // trans_cost テーブルと macro_extra を構築する（全 BFS 完了後に呼ぶ）。
    void build_trans_and_extra() {
        {
            // ウェイポイント間遷移コストテーブルを構築
            chrono::steady_clock::time_point t0;
            if constexpr (DEBUG)
                t0 = chrono::steady_clock::now();
            const int W = (int)wp_cells.size();
            for (int fi = 0; fi < W; fi++) {
                for (int ti = 0; ti < W; ti++) {
                    auto &tc = trans_cost[fi][ti];
                    tc.fill(Cost{});
                    for (int d1 = 0; d1 < 4; d1++) {
                        const int src = sid_to_source[(wp_cells[fi] << 2) | d1];
                        if (src == -1)
                            continue;
                        for (int d2 = 0; d2 < 4; d2++) {
                            const int to_sid = (wp_cells[ti] << 2) | d2;
                            if (dist_stamp[src][to_sid] != build_gen)
                                continue;
                            tc[d1 * 4 + d2] = dist[src][to_sid];
                        }
                    }
                }
            }
            if constexpr (DEBUG)
                g_prof.time_query +=
                    chrono::duration<double>(chrono::steady_clock::now() - t0)
                        .count();
        }
        compute_macro_extra();
    }

    // 下界スコア bs を求めるための軽量ビルド。
    // 各ボール i の dir0 ソース（向きは任意なので 0 固定）から、担当籠への
    // 最短距離を早期終了 BFS で求め、その総和を返す。trans_cost や残りの
    // ソース（dir1..3 ボール / 籠 / start）の BFS は行わない。
    // build_rest() を続けて呼べば、ここで計算済みの dir0 ボール BFS を
    // 再計算せずに完全な評価状態を構築できる（差分ビルド）。
    i64 build_lb(const string &macro_) {
        prepare_transform(macro_);
        i64 bs = 0;
        for (int i = 0; i < in.M; i++) {
            const int sid = (in.ball_cells[i] << 2) | 0; // 向き0固定
            const int src_idx = sid_to_source[sid];
            assert(src_idx != -1);
            run_bfs(src_idx, false); // 早期終了あり（need_basket_cell）
            const int bcell = in.basket_cells[i];
            int best = inf32;
            for (int d = 0; d < 4; d++) {
                const int to = (bcell << 2) | d;
                if (dist_stamp[src_idx][to] == build_gen)
                    smin(best, dist[src_idx][to].a);
            }
            assert(best < inf32);
            bs += best;
        }
        return bs;
    }

    // build_lb() 後に残りの BFS と trans_cost/macro_extra を構築し、完全な
    // 評価状態にする。early 群（純ボールセル）の dir0 は build_lb で計算済みの
    // ため再計算しない。track_parent=false 固定（ホットパス専用）。
    void build_rest() {
        {
            chrono::steady_clock::time_point t0;
            if constexpr (DEBUG)
                t0 = chrono::steady_clock::now();
            const int start_src = sid_to_source[in.state_id(0, 0, 0)];
            const bool use_grouped = USE_DIR_SEED;
            assert(use_grouped &&
                   "build_lb/build_rest は USE_DIR_SEED 前提で実装");
            for (int g = 0; g < (int)dir_groups.size(); g++) {
                const bool is_early = group_is_early[g];
                const bool seed_this =
                    !is_early || SEED_FOR_EARLY || VERIFY_DIR_SEED;
                if (seed_this) {
                    run_bfs_group(dir_groups[g], is_early, start_src);
                } else {
                    // dir0 は build_lb で計算済みのためスキップ。
                    for (int dir = 1; dir < 4; dir++) {
                        const int i = dir_groups[g][dir];
                        run_bfs(i, i == start_src);
                    }
                }
            }
            for (int i = 0; i < (int)sources.size(); i++) {
                if (source_in_group[i])
                    continue;
                run_bfs(i, i == start_src);
            }
            if (VERIFY_DIR_SEED && use_grouped)
                verify_dir_seed();
            if constexpr (DEBUG)
                g_prof.time_dijkstra +=
                    chrono::duration<double>(chrono::steady_clock::now() - t0)
                        .count();
        }
        build_trans_and_extra();
    }

    void build(const string &macro_, bool track_parent = false) {
        prepare_transform(macro_);
        {
            chrono::steady_clock::time_point t0;
            if constexpr (DEBUG)
                t0 = chrono::steady_clock::now();
            // main_14: track_parent=false（ホットループ）でも、初期地点ソース
            // だけは親を保持して「初期地点→最初のボール」の経路を復元可能にする。
            // 残りのソースは従来どおり <false>
            // の高速パスを通す（速度劣化なし）。
            const int start_src = sid_to_source[in.state_id(0, 0, 0)];
            // seeding は seed ノードに parent を張れないため、経路復元を伴う
            // track_parent ビルドでは使わず従来フルBFSにフォールバックする。
            const bool use_grouped = USE_DIR_SEED && !track_parent;
            if (use_grouped) {
                // グループ（同一セル4向き）は seed 共有で処理。
                // 早期終了グループ（ボール始点）は既定では seed せず通常処理。
                for (int g = 0; g < (int)dir_groups.size(); g++) {
                    const bool is_early = group_is_early[g];
                    const bool seed_this =
                        !is_early || SEED_FOR_EARLY || VERIFY_DIR_SEED;
                    if (seed_this) {
                        run_bfs_group(dir_groups[g], is_early, start_src);
                    } else {
                        for (int dir = 0; dir < 4; dir++) {
                            const int i = dir_groups[g][dir];
                            run_bfs(i, track_parent || i == start_src);
                        }
                    }
                }
                // グループに属さないソース（start セル単独など）。
                for (int i = 0; i < (int)sources.size(); i++) {
                    if (source_in_group[i])
                        continue;
                    run_bfs(i, track_parent || i == start_src);
                }
            } else {
                for (int i = 0; i < (int)sources.size(); i++) {
                    run_bfs(i, track_parent || i == start_src);
                }
            }
            if (VERIFY_DIR_SEED && use_grouped)
                verify_dir_seed();
            if constexpr (DEBUG)
                g_prof.time_dijkstra +=
                    chrono::duration<double>(chrono::steady_clock::now() - t0)
                        .count();
        }
        build_trans_and_extra();
    }

    string restore_path(int src_idx, int end_sid) const {
        string path;
        int cur = end_sid;
        while (cur != sources[src_idx]) {
            if (parent_stamp[src_idx][cur] != build_gen)
                break;
            const ParentStep &ps = parent[src_idx][cur];
            if (ps.prev < 0)
                break;
            path.push_back(ps.move);
            cur = ps.prev;
        }
        reverse(all(path));
        return path;
    }
};

// 1ステップのDP遷移: dp × trans_cost行 → dp_next
inline array<Cost, 4> transit_dp(const array<Cost, 4> &dp,
                                 const array<Cost, 16> &tc) {
    array<Cost, 4> res;
    res.fill(Cost{});
    for (int d1 = 0; d1 < 4; d1++) {
        if (dp[d1].a >= inf32 / 4)
            continue;
        const int base_a = dp[d1].a;
        const int base_e = dp[d1].e;
        const Cost *__restrict row = tc.data() + d1 * 4;
        for (int d2 = 0; d2 < 4; d2++) {
            const Cost &c = row[d2];
            if (c.a >= inf32 / 4)
                continue;
            const int na = base_a + c.a;
            const int ne = base_e + c.e;
            if (na < res[d2].a || (na == res[d2].a && ne < res[d2].e))
                res[d2] = {na, ne};
        }
    }
    return res;
}

// 最終DP状態からスコアのみ計算（order コピーを伴わない軽量版）
struct ScoreAE {
    i64 score;
    int A;
    int E;
};
// macro_extra は「最初のボール(order[0])」に対応する登録追加コスト。
// 呼び出し側が候補 order の先頭ボールから ctx.macro_extra_at() で求めて渡す。
inline ScoreAE dp_to_score(const array<Cost, 4> &fdp,
                           const MacroEvalContext &ctx, int macro_extra) {
    Cost best;
    for (int d = 0; d < 4; d++)
        if (fdp[d] < best)
            best = fdp[d];
    int A = best.a + 2 * in.M;
    int E = best.e + 2 * in.M;
    if (!ctx.macro.empty() && E > A) {
        // メインマクロ登録の追加コスト分を A に加算する。
        A += macro_extra;
    }
    ScoreAE res;
    res.A = A;
    res.E = E;
    if (A > in.T || E > in.T)
        res.score = (i64)(max(A, E) - in.T) * 1000 + A;
    else
        res.score = A;
    return res;
}

// 最終DP状態からEvalResultを計算（order をコピーするフル版）
inline EvalResult dp_to_result(const array<Cost, 4> &fdp,
                               const vector<int> &order,
                               const MacroEvalContext &ctx) {
    const int first_ball = order.empty() ? -1 : order[0];
    ScoreAE s = dp_to_score(fdp, ctx, ctx.macro_extra_at(first_ball));
    EvalResult res;
    res.order = order;
    res.A = s.A;
    res.E = s.E;
    res.score = s.score;
    return res;
}

// 近傍適用後の候補 order の先頭ボール(order[0])を、order を実際に
// 書き換えずに求めるヘルパー。先頭が変わるのは p==0 のときだけ。
// 区間反転 reverse(i,j): i==0 のとき先頭は order[j] になる。
inline int cand_first_ball_reverse(const vector<int> &order, int i, int j) {
    return (i == 0) ? order[j] : order[0];
}
// ブロック移動 relocate(i,L,ins,rev): [i,i+L) を取り除き、除去後配列の位置 ins
// に挿入(rev なら反転)。先頭は ins==0 ならブロック先頭、i==0 なら除去後の先頭。
inline int cand_first_ball_relocate(const vector<int> &order, int i, int L,
                                    int ins, bool rev) {
    if (ins == 0)
        return rev ? order[i + L - 1] : order[i];
    if (i == 0)
        return order[L];
    return order[0];
}

inline void apply_relocate_order(vector<int> &order, int i, int L, int ins,
                                 bool rev) {
    if (rev)
        reverse(order.begin() + i, order.begin() + i + L);
    if (ins < i) {
        rotate(order.begin() + ins, order.begin() + i, order.begin() + i + L);
    } else if (ins > i) {
        rotate(order.begin() + i, order.begin() + i + L,
               order.begin() + ins + L);
    }
}

EvalResult evaluate_order(const MacroEvalContext &ctx,
                          const vector<int> &order) {
    array<Cost, 4> dp;
    dp.fill(Cost{});
    dp[0] = {0, 0};
    int cur_wp = ctx.start_wp;
    for (int idx : order) {
        dp = transit_dp(dp, ctx.trans_cost[cur_wp][ctx.ball_wp[idx]]);
        cur_wp = ctx.ball_wp[idx];
        dp = transit_dp(dp, ctx.trans_cost[cur_wp][ctx.basket_wp[idx]]);
        cur_wp = ctx.basket_wp[idx];
    }
    return dp_to_result(dp, order, ctx);
}

struct OrderRelocateSampler {
    const MacroEvalContext &ctx;
    const vector<int> &order;
    int M;
    vector<int> pos;
    FenwickWeightedSampler left_sampler;
    FenwickWeightedSampler right_sampler; // index t means r=t+1

    OrderRelocateSampler(const MacroEvalContext &ctx_,
                         const vector<int> &order_)
        : ctx(ctx_), order(order_), M((int)order_.size()) {
        pos.resize(M);
        rebuild_all();
    }

    double left_weight(int l) const {
        const int from_wp =
            (l == 0) ? ctx.start_wp : ctx.basket_wp[order[l - 1]];
        return max(1.0, ctx.order_weight_dist(
                            ctx.dirless_dist(from_wp, ctx.ball_wp[order[l]])));
    }

    double right_weight(int r) const {
        if (r >= M)
            return 1.0;
        return max(1.0,
                   ctx.order_weight_dist(ctx.dirless_dist(
                       ctx.basket_wp[order[r - 1]], ctx.ball_wp[order[r]])));
    }

    void rebuild_all() {
        if (M == 0)
            return;
        for (int i = 0; i < M; i++)
            pos[order[i]] = i;
        vector<double> lw(M), rw(M);
        for (int i = 0; i < M; i++)
            lw[i] = left_weight(i);
        for (int i = 0; i < M; i++)
            rw[i] = right_weight(i + 1);
        left_sampler.build(lw);
        right_sampler.build(rw);
    }

    void refresh_pos_range(int lo, int hi) {
        lo = max(lo, 0);
        hi = min(hi, M);
        for (int i = lo; i < hi; i++)
            pos[order[i]] = i;
    }

    void refresh_boundary(int k) {
        if (k < 0 || k > M)
            return;
        if (k < M)
            left_sampler.set_weight(k, left_weight(k));
        if (k >= 1)
            right_sampler.set_weight(k - 1, right_weight(k));
    }

    void refresh_boundaries(int lo, int hi) {
        lo = max(lo, 0);
        hi = min(hi, M);
        for (int k = lo; k <= hi; k++)
            refresh_boundary(k);
    }

    void update_after_swap(int i, int j) {
        if (i == j)
            return;
        if (i > j)
            swap(i, j);
        pos[order[i]] = i;
        pos[order[j]] = j;
        refresh_boundary(i);
        refresh_boundary(i + 1);
        refresh_boundary(j);
        refresh_boundary(j + 1);
    }

    void update_after_reverse(int l, int r_exclusive) {
        if (r_exclusive - l <= 1)
            return;
        refresh_pos_range(l, r_exclusive);
        refresh_boundaries(l, r_exclusive);
    }

    void update_after_relocate(int old_l, int len, int ins) {
        const int lo = min(old_l, ins);
        const int hi = max(old_l + len, ins + len);
        refresh_pos_range(lo, hi);
        refresh_boundaries(lo, hi);
    }

    pair<int, int> sample_segment() {
        if (M <= 1)
            return {-1, -1};
        for (int tries = 0; tries < 16; tries++) {
            const int l = left_sampler.sample();
            const int r = right_sampler.sample() + 1;
            if (l < r && r - l < M)
                return {l, r};
        }
        const int L = 1 + rng.random32(M - 1);
        const int l = rng.random32(M - L + 1);
        return {l, l + L};
    }

    bool invalid_insert(int l, int r, int ins, bool do_reverse) const {
        const int L = r - l;
        if (ins < 0 || ins > M - L)
            return true;
        if (!do_reverse && ins == l)
            return true;
        if (do_reverse && L == 1 && ins == l)
            return true;
        return false;
    }

    int insertion_after_ref(int ref, int l, int r) const {
        if (ref < 0)
            return 0;
        const int p = pos[ref];
        if (l <= p && p < r)
            return -1;
        const int L = r - l;
        return (p < l) ? p + 1 : p - L + 1;
    }

    int insertion_before_ref(int ref, int l, int r) const {
        const int p = pos[ref];
        if (l <= p && p < r)
            return -1;
        const int L = r - l;
        return (p < l) ? p : p - L;
    }

    int sample_insert(int l, int r, bool do_reverse) {
        const int first = do_reverse ? order[r - 1] : order[l];
        const int last = do_reverse ? order[l] : order[r - 1];
        for (int tries = 0; tries < 16; tries++) {
            int ins = -1;
            if (rng.random32(2) == 0) {
                const int ref_sample =
                    ctx.insert_prev_sampler[first].sample(); // 0 means start
                const int ref = ref_sample - 1;
                ins = insertion_after_ref(ref, l, r);
            } else {
                const int ref = ctx.insert_next_sampler[last].sample();
                ins = insertion_before_ref(ref, l, r);
            }
            if (!invalid_insert(l, r, ins, do_reverse))
                return ins;
        }
        const int L = r - l;
        for (int tries = 0; tries < 16; tries++) {
            const int ins = rng.random32(M - L + 1);
            if (!invalid_insert(l, r, ins, do_reverse))
                return ins;
        }
        return -1;
    }
};

EvalResult improve_order(const MacroEvalContext &ctx, vector<int> order,
                         double time_limit, [[maybe_unused]] bool verbose) {
    chrono::steady_clock::time_point t_imp0;
    if constexpr (DEBUG) {
        t_imp0 = chrono::steady_clock::now();
        g_prof.count_improve++;
    }
    const int M = in.M;

    // 増分DP用テーブル（再確保なし）
    vector<array<Cost, 4>> dp_saved(2 * M + 1);
    vector<int> wp(2 * M + 1);

    // dp_saved を pair p から末尾まで再構築し wp も更新（採用時のみ呼ぶ）
    auto rebuild = [&](int p) {
        for (int k = p; k < M; k++) {
            const int bwp = ctx.ball_wp[order[k]];
            const int xwp = ctx.basket_wp[order[k]];
            dp_saved[2 * k + 1] =
                transit_dp(dp_saved[2 * k], ctx.trans_cost[wp[2 * k]][bwp]);
            wp[2 * k + 1] = bwp;
            dp_saved[2 * k + 2] =
                transit_dp(dp_saved[2 * k + 1], ctx.trans_cost[bwp][xwp]);
            wp[2 * k + 2] = xwp;
        }
    };

    // pair p 以降をローカル変数だけで計算し最終dpを返す（中間保存なし）
    auto eval_from = [&](int p) -> array<Cost, 4> {
        array<Cost, 4> dp = dp_saved[2 * p];
        int cur_wp = wp[2 * p];
        for (int k = p; k < M; k++) {
            const int bwp = ctx.ball_wp[order[k]];
            const int xwp = ctx.basket_wp[order[k]];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        }
        return dp;
    };
    auto eval_reverse = [&](int i, int j) -> array<Cost, 4> {
        array<Cost, 4> dp = dp_saved[2 * i];
        int cur_wp = wp[2 * i];
        for (int k = j; k >= i; k--) {
            const int idx = order[k];
            const int bwp = ctx.ball_wp[idx];
            const int xwp = ctx.basket_wp[idx];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        }
        for (int k = j + 1; k < M; k++) {
            const int idx = order[k];
            const int bwp = ctx.ball_wp[idx];
            const int xwp = ctx.basket_wp[idx];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        }
        return dp;
    };
    auto eval_block_relocate = [&](int i, int L, int ins,
                                   bool do_reverse) -> array<Cost, 4> {
        const int p = min(i, ins);
        array<Cost, 4> dp = dp_saved[2 * p];
        int cur_wp = wp[2 * p];
        auto step = [&](int idx) {
            const int bwp = ctx.ball_wp[idx];
            const int xwp = ctx.basket_wp[idx];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        };
        auto step_block = [&]() {
            if (do_reverse) {
                for (int t = L - 1; t >= 0; t--)
                    step(order[i + t]);
            } else {
                for (int t = 0; t < L; t++)
                    step(order[i + t]);
            }
        };
        if (p == ins)
            step_block();
        for (int r = p; r < M - L; r++) {
            const int src = (r < i) ? r : r + L;
            step(order[src]);
            if (ins == r + 1)
                step_block();
        }
        return dp;
    };

    // 初期化
    dp_saved[0].fill(Cost{});
    dp_saved[0][0] = {0, 0};
    wp[0] = ctx.start_wp;
    rebuild(0);
    OrderRelocateSampler relocate_sampler(ctx, order);

    EvalResult best = dp_to_result(dp_saved[2 * M], order, ctx);
    int no_improve = 0, iter = 0;
    const int no_improve_limit = M * M;

    // 採用時のみ order をコピーして best を更新（候補評価ではコピーしない）
    auto commit = [&](const ScoreAE &c) {
        best.score = c.score;
        best.A = c.A;
        best.E = c.E;
        best.order = order;
    };

    // 近傍選択用サンプラー (SAと共通の重み、高速な AliasWeightedSampler を使用)
    AliasWeightedSampler move_selector;
    {
        vector<double> w(Tuning::ORDER_MOVE_COUNT);
        for (int t = 0; t < Tuning::ORDER_MOVE_COUNT; t++)
            w[t] = max(0.0, Tuning::order_sa_move_weights[t]);
        move_selector.build(w);
    }
    auto pick_move = [&]() -> int { return move_selector.sample(); };

    while (no_improve < no_improve_limit) {
        iter++;
        const int move_type = pick_move();
        switch (move_type) {
        case 0: {
            // 2点swap
            const int i = rng.random32(M);
            int j = rng.random32(M);
            if (i == j)
                continue;
            const int p = min(i, j);
            swap(order[i], order[j]);
            ScoreAE cand =
                dp_to_score(eval_from(p), ctx, ctx.macro_extra_at(order[0]));
            if (cand.score < best.score ||
                (cand.score == best.score && cand.E < best.E)) {
                commit(cand);
                rebuild(p);
                relocate_sampler.update_after_swap(i, j);
                no_improve = 0;
                if constexpr (DEBUG) {
                    if (verbose)
                        cerr << "[order] time=" << fixed << setprecision(3)
                             << timer.elapsed() << " iter=" << iter
                             << " score=" << best.score << " A=" << best.A
                             << " E=" << best.E << el;
                }
            } else {
                swap(order[i], order[j]);
                no_improve++;
            }
            break;
        }
        case 1: {
            // 区間反転
            int i = rng.random32(M), j = rng.random32(M);
            if (i == j)
                continue;
            if (i > j)
                swap(i, j);
            const int p = i;
            ScoreAE cand = dp_to_score(
                eval_reverse(i, j), ctx,
                ctx.macro_extra_at(cand_first_ball_reverse(order, i, j)));
            if (cand.score < best.score ||
                (cand.score == best.score && cand.E < best.E)) {
                reverse(order.begin() + i, order.begin() + j + 1);
                commit(cand);
                rebuild(p);
                relocate_sampler.update_after_reverse(i, j + 1);
                no_improve = 0;
                if constexpr (DEBUG) {
                    if (verbose)
                        cerr << "[order] time=" << fixed << setprecision(3)
                             << timer.elapsed() << " iter=" << iter
                             << " score=" << best.score << " A=" << best.A
                             << " E=" << best.E << el;
                }
            } else {
                no_improve++;
            }
            break;
        }
        case 2: {
            // ブロック削除挿入 (relocate, 長さ1..M-1)
            if (M <= 1)
                continue;
            auto [i, r] = relocate_sampler.sample_segment();
            if (i < 0)
                continue;
            const int L = r - i;
            const int ins = relocate_sampler.sample_insert(i, r, false);
            if (ins < 0)
                continue;
            const int p = min(i, ins);
            ScoreAE cand =
                dp_to_score(eval_block_relocate(i, L, ins, false), ctx,
                            ctx.macro_extra_at(cand_first_ball_relocate(
                                order, i, L, ins, false)));
            if (cand.score < best.score ||
                (cand.score == best.score && cand.E < best.E)) {
                apply_relocate_order(order, i, L, ins, false);
                commit(cand);
                rebuild(p);
                relocate_sampler.update_after_relocate(i, L, ins);
                no_improve = 0;
                if constexpr (DEBUG) {
                    if (verbose)
                        cerr << "[order] time=" << fixed << setprecision(3)
                             << timer.elapsed() << " iter=" << iter
                             << " score=" << best.score << " A=" << best.A
                             << " E=" << best.E << el;
                }
            } else {
                no_improve++;
            }
            break;
        }
        case 3: {
            // 隣接swap (j = i+1)
            if (M <= 1)
                continue;
            const int i = rng.random32(M - 1);
            const int p = i;
            swap(order[i], order[i + 1]);
            ScoreAE cand =
                dp_to_score(eval_from(p), ctx, ctx.macro_extra_at(order[0]));
            if (cand.score < best.score ||
                (cand.score == best.score && cand.E < best.E)) {
                commit(cand);
                rebuild(p);
                relocate_sampler.update_after_swap(i, i + 1);
                no_improve = 0;
                if constexpr (DEBUG) {
                    if (verbose)
                        cerr << "[order] time=" << fixed << setprecision(3)
                             << timer.elapsed() << " iter=" << iter
                             << " score=" << best.score << " A=" << best.A
                             << " E=" << best.E << el;
                }
            } else {
                swap(order[i], order[i + 1]);
                no_improve++;
            }
            break;
        }
        default: {
            // 4: ブロック移動+反転 (長さ1..M-1)
            const bool do_reverse = true;
            if (M <= 1)
                continue;
            auto [i, r] = relocate_sampler.sample_segment();
            if (i < 0)
                continue;
            const int L = r - i;
            const int ins = relocate_sampler.sample_insert(i, r, do_reverse);
            if (ins < 0)
                continue;
            const int p = min(i, ins);
            ScoreAE cand =
                dp_to_score(eval_block_relocate(i, L, ins, do_reverse), ctx,
                            ctx.macro_extra_at(cand_first_ball_relocate(
                                order, i, L, ins, do_reverse)));
            if (cand.score < best.score ||
                (cand.score == best.score && cand.E < best.E)) {
                apply_relocate_order(order, i, L, ins, true);
                commit(cand);
                rebuild(p);
                relocate_sampler.update_after_relocate(i, L, ins);
                no_improve = 0;
                if constexpr (DEBUG) {
                    if (verbose)
                        cerr << "[order] time=" << fixed << setprecision(3)
                             << timer.elapsed() << " iter=" << iter
                             << " score=" << best.score << " A=" << best.A
                             << " E=" << best.E << el;
                }
            } else {
                no_improve++;
            }
            break;
        }
        }
    }
    if constexpr (DEBUG) {
        g_prof.improve_iters += iter;
        g_prof.time_improve +=
            chrono::duration<double>(chrono::steady_clock::now() - t_imp0)
                .count();
    }
    return best;
}

EvalResult sa_improve_order(const MacroEvalContext &ctx,
                            const EvalResult &init_eval, double time_limit) {
    vector<int> order = init_eval.order;
    i64 cur_score = init_eval.score;
    EvalResult best = init_eval;
    const int M = in.M;

    // 増分DP用テーブル（再確保なし）
    vector<array<Cost, 4>> dp_saved(2 * M + 1);
    vector<int> wp(2 * M + 1);

    auto rebuild = [&](int p) {
        for (int k = p; k < M; k++) {
            const int bwp = ctx.ball_wp[order[k]];
            const int xwp = ctx.basket_wp[order[k]];
            dp_saved[2 * k + 1] =
                transit_dp(dp_saved[2 * k], ctx.trans_cost[wp[2 * k]][bwp]);
            wp[2 * k + 1] = bwp;
            dp_saved[2 * k + 2] =
                transit_dp(dp_saved[2 * k + 1], ctx.trans_cost[bwp][xwp]);
            wp[2 * k + 2] = xwp;
        }
    };
    auto eval_from = [&](int p) -> array<Cost, 4> {
        array<Cost, 4> dp = dp_saved[2 * p];
        int cur_wp = wp[2 * p];
        for (int k = p; k < M; k++) {
            const int bwp = ctx.ball_wp[order[k]];
            const int xwp = ctx.basket_wp[order[k]];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        }
        return dp;
    };
    auto eval_reverse = [&](int i, int j) -> array<Cost, 4> {
        array<Cost, 4> dp = dp_saved[2 * i];
        int cur_wp = wp[2 * i];
        for (int k = j; k >= i; k--) {
            const int idx = order[k];
            const int bwp = ctx.ball_wp[idx];
            const int xwp = ctx.basket_wp[idx];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        }
        for (int k = j + 1; k < M; k++) {
            const int idx = order[k];
            const int bwp = ctx.ball_wp[idx];
            const int xwp = ctx.basket_wp[idx];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        }
        return dp;
    };
    auto eval_block_relocate = [&](int i, int L, int ins,
                                   bool do_reverse) -> array<Cost, 4> {
        const int p = min(i, ins);
        array<Cost, 4> dp = dp_saved[2 * p];
        int cur_wp = wp[2 * p];
        auto step = [&](int idx) {
            const int bwp = ctx.ball_wp[idx];
            const int xwp = ctx.basket_wp[idx];
            dp = transit_dp(dp, ctx.trans_cost[cur_wp][bwp]);
            dp = transit_dp(dp, ctx.trans_cost[bwp][xwp]);
            cur_wp = xwp;
        };
        auto step_block = [&]() {
            if (do_reverse) {
                for (int t = L - 1; t >= 0; t--)
                    step(order[i + t]);
            } else {
                for (int t = 0; t < L; t++)
                    step(order[i + t]);
            }
        };
        if (p == ins)
            step_block();
        for (int r = p; r < M - L; r++) {
            const int src = (r < i) ? r : r + L;
            step(order[src]);
            if (ins == r + 1)
                step_block();
        }
        return dp;
    };
    // 候補評価はスコアのみ（order コピーを伴わない）。
    // extra = 候補 order の先頭ボールに対応するマクロ登録追加コスト。
    auto make_result = [&](const array<Cost, 4> &fdp, int extra) -> ScoreAE {
        return dp_to_score(fdp, ctx, extra);
    };
    int iter = 0;
    struct OrderSaBestLog {
        f64 time = 0.0;
        int iter = 0;
        i64 score = 0;
        int A = 0;
        int E = 0;
    };
    vector<OrderSaBestLog> order_sa_best_logs;
    // best 更新時のみ order をコピー
    auto commit = [&](const ScoreAE &c) {
        best.score = c.score;
        best.A = c.A;
        best.E = c.E;
        best.order = order;
        if constexpr (DEBUG) {
            order_sa_best_logs.push_back(
                {timer.elapsed(), iter, best.score, best.A, best.E});
        }
    };

    dp_saved[0].fill(Cost{});
    dp_saved[0][0] = {0, 0};
    wp[0] = ctx.start_wp;
    rebuild(0);
    OrderRelocateSampler relocate_sampler(ctx, order);

    const double time_start = timer.elapsed();
    const double span = max(1e-9, time_limit - time_start);
    double now = time_start;
    double temp = Tuning::order_sa_start_temp;

    // 近傍選択用サンプラー (高速な AliasWeightedSampler を使用)
    AliasWeightedSampler move_selector;
    {
        vector<double> w(Tuning::ORDER_MOVE_COUNT);
        for (int t = 0; t < Tuning::ORDER_MOVE_COUNT; t++)
            w[t] = max(0.0, Tuning::order_sa_move_weights[t]);
        move_selector.build(w);
    }
    auto pick_move = [&]() -> int { return move_selector.sample(); };

    while (true) {
        iter++;

        now = timer.elapsed();
        if (now > time_limit)
            break;
        double progress = min(1.0, (now - time_start) / span);
        if (Tuning::order_sa_use_exp_decay) {
            temp = Tuning::order_sa_start_temp *
                   pow(Tuning::order_sa_end_temp / Tuning::order_sa_start_temp,
                       progress);
        } else {
            double p = pow(progress, Tuning::order_sa_power_decay_exp);
            temp =
                Tuning::order_sa_start_temp -
                (Tuning::order_sa_start_temp - Tuning::order_sa_end_temp) * p;
        }
        const int move_type = pick_move();
        switch (move_type) {
        case 0: {
            const int i = rng.random32(M);
            int j = rng.random32(M);
            if (i == j)
                continue;
            const int p = min(i, j);
            swap(order[i], order[j]);
            ScoreAE cand =
                make_result(eval_from(p), ctx.macro_extra_at(order[0]));
            double delta = (double)(cand.score - cur_score);
            if (delta <= 0.0 || rng.randomDouble() < exp(-delta / temp)) {
                cur_score = cand.score;
                rebuild(p);
                relocate_sampler.update_after_swap(i, j);
                if (cand.score < best.score ||
                    (cand.score == best.score && cand.E < best.E))
                    commit(cand);
            } else {
                swap(order[i], order[j]);
            }
            break;
        }
        case 1: {
            int i = rng.random32(M), j = rng.random32(M);
            if (i == j)
                continue;
            if (i > j)
                swap(i, j);
            const int p = i;
            ScoreAE cand = make_result(
                eval_reverse(i, j),
                ctx.macro_extra_at(cand_first_ball_reverse(order, i, j)));
            double delta = (double)(cand.score - cur_score);
            if (delta <= 0.0 || rng.randomDouble() < exp(-delta / temp)) {
                reverse(order.begin() + i, order.begin() + j + 1);
                cur_score = cand.score;
                rebuild(p);
                relocate_sampler.update_after_reverse(i, j + 1);
                if (cand.score < best.score ||
                    (cand.score == best.score && cand.E < best.E))
                    commit(cand);
            }
            break;
        }
        case 2: {
            if (M <= 1)
                continue;
            auto [i, r] = relocate_sampler.sample_segment();
            if (i < 0)
                continue;
            const int L = r - i;
            const int ins = relocate_sampler.sample_insert(i, r, false);
            if (ins < 0)
                continue;
            const int p = min(i, ins);
            ScoreAE cand =
                make_result(eval_block_relocate(i, L, ins, false),
                            ctx.macro_extra_at(cand_first_ball_relocate(
                                order, i, L, ins, false)));
            double delta = (double)(cand.score - cur_score);
            if (delta <= 0.0 || rng.randomDouble() < exp(-delta / temp)) {
                apply_relocate_order(order, i, L, ins, false);
                cur_score = cand.score;
                rebuild(p);
                relocate_sampler.update_after_relocate(i, L, ins);
                if (cand.score < best.score ||
                    (cand.score == best.score && cand.E < best.E))
                    commit(cand);
            }
            break;
        }
        case 3: {
            // 隣接swap (j = i+1)
            if (M <= 1)
                continue;
            const int i = rng.random32(M - 1);
            const int p = i;
            swap(order[i], order[i + 1]);
            ScoreAE cand =
                make_result(eval_from(p), ctx.macro_extra_at(order[0]));
            double delta = (double)(cand.score - cur_score);
            if (delta <= 0.0 || rng.randomDouble() < exp(-delta / temp)) {
                cur_score = cand.score;
                rebuild(p);
                relocate_sampler.update_after_swap(i, i + 1);
                if (cand.score < best.score ||
                    (cand.score == best.score && cand.E < best.E))
                    commit(cand);
            } else {
                swap(order[i], order[i + 1]);
            }
            break;
        }
        default: {
            // 4: ブロック移動+反転 (長さ1..M-1)
            const bool do_reverse = true;
            if (M <= 1)
                continue;
            auto [i, r] = relocate_sampler.sample_segment();
            if (i < 0)
                continue;
            const int L = r - i;
            const int ins = relocate_sampler.sample_insert(i, r, do_reverse);
            if (ins < 0)
                continue;
            const int p = min(i, ins);
            ScoreAE cand =
                make_result(eval_block_relocate(i, L, ins, do_reverse),
                            ctx.macro_extra_at(cand_first_ball_relocate(
                                order, i, L, ins, do_reverse)));
            double delta = (double)(cand.score - cur_score);
            if (delta <= 0.0 || rng.randomDouble() < exp(-delta / temp)) {
                apply_relocate_order(order, i, L, ins, true);
                cur_score = cand.score;
                rebuild(p);
                relocate_sampler.update_after_relocate(i, L, ins);
                if (cand.score < best.score ||
                    (cand.score == best.score && cand.E < best.E))
                    commit(cand);
            }
            break;
        }
        }
    }
    if constexpr (DEBUG) {
        for (const OrderSaBestLog &log : order_sa_best_logs) {
            cerr << "[order_sa_best] time=" << fixed << setprecision(3)
                 << log.time << " iter=" << log.iter << " total_iter=" << iter
                 << " score=" << log.score << " A=" << log.A << " E=" << log.E
                 << " T=" << g_log_T << " macro=" << ctx.macro << el;
        }
        cerr << "[order_sa_end] time=" << fixed << setprecision(3)
             << timer.elapsed() << " iter=" << iter << " total_iter=" << iter
             << " score=" << best.score << " A=" << best.A << " E=" << best.E
             << " T=" << g_log_T << " macro=" << ctx.macro << el;
    }
    return best;
}

MacroEvalContext g_ctx;
unordered_map<string, EvalResult> g_macro_cache;

// マクロごとのコスト下界キャッシュ（マクロ文字列 -> 下界値）。
unordered_map<string, int> g_lb_cache;

// コスト下界を高速に求める軽量 BFS 構造。
// - start 地点(0,0,右)から任意のボール(向き任意)までの最短手数 A（早期終了）
// - 各ボール→対応かご の最短手数 A の総和（多始点・早期終了）
// - これらの総和 + マクロ生成推定コスト(compute_macro_extra_baseline) を返す。
// E は一切扱わない（手数 A の素朴な BFS）。バッファは使い回す。
struct MacroLB {
    int S = 0;
    vector<array<int, 4>> next4; // [0..2]=F/L/R, [3]=P(マクロ展開後)
    vector<int> dist;
    vector<int> stamp;
    int gen = 0;
    vector<int> q;
    vector<i16> par_prev;
    vector<char> par_move;
    vector<int> par_stamp;
    vector<char> is_ball_cell;
    int start_sid = 0;
    bool inited = false;

    void init() {
        if (inited)
            return;
        S = in.N * in.N * 4;
        next4.resize(S);
        dist.assign(S, 0);
        stamp.assign(S, 0);
        par_prev.assign(S, -1);
        par_move.assign(S, '?');
        par_stamp.assign(S, 0);
        q.resize(S);
        is_ball_cell.assign(in.N * in.N, 0);
        for (int k = 0; k < in.M; k++)
            is_ball_cell[in.ball_cells[k]] = 1;
        start_sid = in.state_id(0, 0, 0);
        inited = true;
    }

    // 列[0..2]（F/L/R 基本遷移）は不変なので初回のみ設定する。
    bool next4_base_ready = false;
    void ensure_next4_base() {
        if (next4_base_ready)
            return;
        for (int sid = 0; sid < S; sid++) {
            next4[sid][0] = in.base_next[sid][0];
            next4[sid][1] = in.base_next[sid][1];
            next4[sid][2] = in.base_next[sid][2];
        }
        next4_base_ready = true;
    }

    // next4[sid][3] = sid からマクロを展開実行した後の状態。
    // macro_sa と同様に、毎回更新するのはマクロ結果部（列[3]）だけ。
    // F の連続は in.fwd_pow による 1 回のテーブル参照（連長 N
    // でクリップ）で畳む。
    void build_next4(const string &macro) {
        ensure_next4_base();
        for (int sid = 0; sid < S; sid++)
            next4[sid][3] = sid;
        const int n = (int)macro.size();
        int i = 0;
        while (i < n) {
            const char ch = macro[i];
            if (ch == 'F') {
                int run = 0;
                while (i < n && macro[i] == 'F') {
                    run++;
                    i++;
                }
                if (run > in.fwd_max)
                    run =
                        in.fwd_max; // N-1 回で壁(不動点)に到達するため厳密一致
                const vector<int> &fp = in.fwd_pow[run];
                for (int sid = 0; sid < S; sid++)
                    next4[sid][3] = fp[next4[sid][3]];
            } else {
                const int op = (ch == 'L') ? 1 : 2;
                for (int sid = 0; sid < S; sid++)
                    next4[sid][3] = in.base_next[next4[sid][3]][op];
                i++;
            }
        }
    }

    // start から最初に到達するボールセルまでの A（早期終了）。到達不可なら
    // inf32。
    int bfs_start_to_ball(bool has_p) {
        gen++;
        int head = 0, tail = 0;
        dist[start_sid] = 0;
        stamp[start_sid] = gen;
        q[tail++] = start_sid;
        const int deg = has_p ? 4 : 3;
        while (head < tail) {
            const int sid = q[head++];
            if (is_ball_cell[sid >> 2])
                return dist[sid];
            const auto &nx = next4[sid];
            const int nd = dist[sid] + 1;
            for (int k = 0; k < deg; k++) {
                const int to = nx[k];
                if (stamp[to] != gen) {
                    stamp[to] = gen;
                    dist[to] = nd;
                    q[tail++] = to;
                }
            }
        }
        return inf32;
    }

    // ボールセル(4向き多始点)→かごセル の A（早期終了）。到達不可なら inf32。
    int bfs_ball_to_basket(int ball_cell, int basket_cell, bool has_p) {
        gen++;
        int head = 0, tail = 0;
        for (int d = 0; d < 4; d++) {
            const int sid = (ball_cell << 2) | d;
            dist[sid] = 0;
            stamp[sid] = gen;
            q[tail++] = sid;
        }
        const int deg = has_p ? 4 : 3;
        while (head < tail) {
            const int sid = q[head++];
            if ((sid >> 2) == basket_cell)
                return dist[sid];
            const auto &nx = next4[sid];
            const int nd = dist[sid] + 1;
            for (int k = 0; k < deg; k++) {
                const int to = nx[k];
                if (stamp[to] != gen) {
                    stamp[to] = gen;
                    dist[to] = nd;
                    q[tail++] = to;
                }
            }
        }
        return inf32;
    }

    // マクロのコスト下界。g_lb_cache でメモ化する。
    int cost_lower_bound(const string &macro) {
        auto it = g_lb_cache.find(macro);
        if (it != g_lb_cache.end())
            return it->second;
        build_next4(macro);
        const bool has_p = !macro.empty();
        i64 sum = 0;
        const int sa = bfs_start_to_ball(has_p);
        if (sa >= inf32) {
            g_lb_cache.emplace(macro, inf32);
            return inf32;
        }
        sum += sa;
        for (int k = 0; k < in.M; k++) {
            const int a =
                bfs_ball_to_basket(in.ball_cells[k], in.basket_cells[k], has_p);
            if (a >= inf32) {
                g_lb_cache.emplace(macro, inf32);
                return inf32;
            }
            sum += a;
        }
        sum += compute_macro_extra_baseline(macro);
        const int res = (sum >= inf32) ? inf32 : (int)sum;
        g_lb_cache.emplace(macro, res);
        return res;
    }

    // --- パスマクロ生成（マクロ無しの最短経路文字列）用 ---
    // 親付き全域 BFS（マクロ無し、F/L/R のみ）。
    void bfs_full_from(const vector<int> &srcs) {
        gen++;
        int head = 0, tail = 0;
        for (int s : srcs) {
            dist[s] = 0;
            stamp[s] = gen;
            par_prev[s] = -1;
            par_move[s] = '?';
            par_stamp[s] = gen;
            q[tail++] = s;
        }
        static constexpr char ops3[3] = {'F', 'L', 'R'};
        while (head < tail) {
            const int sid = q[head++];
            const auto &nx = next4[sid];
            const int nd = dist[sid] + 1;
            for (int k = 0; k < 3; k++) {
                const int to = nx[k];
                if (stamp[to] != gen) {
                    stamp[to] = gen;
                    dist[to] = nd;
                    par_prev[to] = (i16)sid;
                    par_move[to] = ops3[k];
                    par_stamp[to] = gen;
                    q[tail++] = to;
                }
            }
        }
    }
    string recover(int end_sid) const {
        string p;
        int cur = end_sid;
        while (par_stamp[cur] == gen && par_prev[cur] >= 0) {
            p.push_back(par_move[cur]);
            cur = par_prev[cur];
        }
        reverse(all(p));
        return p;
    }
    int best_dir_state(int cell) const {
        int best = inf32, bs = -1;
        for (int d = 0; d < 4; d++) {
            const int sid = (cell << 2) | d;
            if (stamp[sid] == gen && dist[sid] < best) {
                best = dist[sid];
                bs = sid;
            }
        }
        return bs;
    }

    // スタート→各ボール(M本) と 各ボール→対応かご(M本) の最短経路文字列(2M本)。
    // 空パスは除外する。
    vector<string> generate_path_macros() {
        vector<string> out;
        out.reserve(2 * in.M);
        build_next4(""); // マクロ無し（[0..2] のみ使用）
        // スタート→各ボール
        bfs_full_from({start_sid});
        for (int k = 0; k < in.M; k++) {
            const int es = best_dir_state(in.ball_cells[k]);
            if (es < 0)
                continue;
            string p = recover(es);
            if (!p.empty())
                out.push_back(std::move(p));
        }
        // 各ボール→対応かご
        for (int k = 0; k < in.M; k++) {
            const int bc = in.ball_cells[k];
            bfs_full_from(
                {(bc << 2) | 0, (bc << 2) | 1, (bc << 2) | 2, (bc << 2) | 3});
            const int es = best_dir_state(in.basket_cells[k]);
            if (es < 0)
                continue;
            string p = recover(es);
            if (!p.empty())
                out.push_back(std::move(p));
        }
        return out;
    }
};
MacroLB g_lb;

struct State {
    using score_t = int64_t;

    score_t score = -1;
    Macro macro_struct;
    string macro;
    vector<int> order;
    int last_A = inf32;
    int last_E = inf32;
    // bs: 現在受理済みマクロの下界スコア（各ボール→担当籠の最短距離和）。
    //     受理時のみ更新し、近傍適用後の早期枝刈り判定の基準値に使う。
    // last_bs: 直近 calc_score_full が評価した候補マクロの下界スコア。
    //     accept 時に bs へ反映する（rollback では bs を汚さない）。
    score_t bs = inf64;
    score_t last_bs = inf64;
    // last_order: 直近 calc_score_full が評価した候補マクロの山登り後 order。
    //     accept 時に order へ反映する。order
    //     は常に「現在受理済みマクロに対する 山登り後の状態」を保ち、次手の
    //     improve_order の warm-start に使う （rollback では order
    //     を汚さない）。
    vector<int> last_order;

    void init() {
        macro_struct = random_macro_struct();
        macro = macro_struct.to_string();
        order.resize(in.M);
        iota(all(order), 0);
        rng.shuffle(order);
        score = calc_score_full();
    }

    // 早期枝刈り時に返す「異常に悪いスコア」。実スコアより常に大きく、二度と
    // 受理されない値。delta 計算でのオーバーフローを避けるため inf64/2 とする。
    static constexpr score_t kPrunedScore = inf64 / 2;

    score_t calc_score_full(optional<score_t> opt_score_threshold = nullopt) {
        // macro_struct.to_string() の正規形では、F が 1 つでもあれば回転(L/R)は
        // 必ず F を挟むため連続せず compress は恒等変換になる。回転が連続して
        // 畳めるのは mid が空（= マクロに F
        // が無い）ときだけなので、その場合のみ compress
        // を呼び、ホットループでの無駄な string 確保・走査を避ける。
        if (macro.find('F') == string::npos)
            macro = compress_macro(macro);
        auto it = g_macro_cache.find(macro);
        if (it != g_macro_cache.end()) {
            if constexpr (DEBUG)
                g_prof.cache_hit++;
            const EvalResult &cached = it->second;
            last_order = cached.order;
            last_A = cached.A;
            last_E = cached.E;
            last_bs = cached.bs;
            // 閾値なし呼び出し（受理済み状態の評価）は order/bs
            // を直接確定する。
            if (!opt_score_threshold) {
                order = cached.order;
                bs = cached.bs;
            }
            return cached.score;
        }
        if constexpr (DEBUG)
            g_prof.cache_miss++;
        // 下界スコア bs は M 回の早期終了 BFS のみで求める軽量ビルド。
        // （残りのソースの BFS / trans_cost は枝刈りされなければ build_rest で
        //   差分構築する。build_lb で計算済みの dir0 ボール BFS
        //   は再計算しない。）
        const score_t new_bs = g_ctx.build_lb(macro);
        last_bs = new_bs;
        // 早期枝刈り: 近傍適用後の下界悪化が受理許容幅に対して大きすぎる場合、
        // order 最適化(improve_order)を行わず異常に悪いスコアを返し、
        // g_macro_cache にも載せて二度と受理されないようにする。
        if (opt_score_threshold) {
            const double tol = (double)*opt_score_threshold - (double)score;
            if (tol * Tuning::bs_prune_coef + Tuning::bs_prune_offset <
                (double)(new_bs - bs)) {
                EvalResult bad;
                bad.score = kPrunedScore;
                bad.A = inf32;
                bad.E = inf32;
                bad.order = order;
                bad.bs = new_bs;
                g_macro_cache[macro] = bad;
                last_A = bad.A;
                last_E = bad.E;
                last_order = order;
                return bad.score;
            }
        } else {
            // 閾値なし呼び出し（初期解/restart 等）は受理済み状態の評価なので
            // bs を直接更新する。
            bs = new_bs;
        }
        // 枝刈りされなかったので残りの BFS と trans_cost を差分構築する。
        g_ctx.build_rest();
        if (order.empty()) {
            order.resize(in.M);
            iota(all(order), 0);
        }
        const double now = timer.elapsed();
        // warm-start は現在受理済みマクロの山登り後 order（threshold 時は
        // order を上書きせず保持する）。候補の結果は last_order に入れ、
        // accept 時に order へ反映する。
        EvalResult best =
            improve_order(g_ctx, order,
                          min(Tuning::order_hill_climb_deadline,
                              now + Tuning::order_hill_climb_span),
                          false);
        last_order = best.order;
        last_A = best.A;
        last_E = best.E;
        best.bs = new_bs;
        g_macro_cache[macro] = best;
        // 閾値なし呼び出し（初期解/restart 等）は受理済み状態なので order を
        // 直接確定する。
        if (!opt_score_threshold)
            order = best.order;
        return best.score;
    }
};

// Macro 構造の近傍操作を mc に適用する。no-op/無効な手のときは false を返し、
// その場合 mc は変更しない（呼び出し側はロールバック不要）。
struct MacroMoveTuning {
    int len_delta_plus;
    int len_delta_minus;
    int all_len_delta;
    int move_len_max;
};

inline MacroMoveTuning macro_sa_move_tuning() {
    return {Tuning::macro_len_delta_plus, Tuning::macro_len_delta_minus,
            Tuning::macro_all_len_delta, Tuning::macro_move_len_max};
}

inline MacroMoveTuning broad_move_tuning() {
    return {Tuning::broad_len_delta_plus, Tuning::broad_len_delta_minus,
            Tuning::broad_all_len_delta, Tuning::broad_move_len_max};
}

// DefaultMoveApplier（macro_sa）とブロード探索 SA の両方から使用する。
inline bool apply_macro_move(Macro &mc, int move_type,
                             const MacroMoveTuning &tuning) {
    switch (move_type) {
    case SAParams::CHANGE_ONE_LEN_PLUS: {
        // ランダムなインデックスのmidの長さを +x 変更
        if (mc.mid.empty())
            return false;
        const int idx = (int)rng.random32((u32)mc.mid.size());
        const int x = tuning.len_delta_plus;
        if (x <= 0)
            return false;
        const int delta = (int)rng.randomRange32(1, x);
        mc.mid[idx].len += delta;
        return true;
    }
    case SAParams::CHANGE_ONE_LEN_MINUS: {
        // ランダムなインデックスのmidの長さを -x 変更（下限1）
        if (mc.mid.empty())
            return false;
        const int idx = (int)rng.random32((u32)mc.mid.size());
        const int x = min(tuning.len_delta_minus, mc.mid[idx].len - 1);
        if (x <= 0)
            return false;
        const int delta = (int)rng.randomRange32(1, x);
        mc.mid[idx].len -= delta;
        return true;
    }
    case SAParams::CHANGE_ALL_LEN: {
        // 全てのmidの長さを ±y 変更（下限1）
        if (mc.mid.empty())
            return false;
        const int y = tuning.all_len_delta;
        for (auto &m : mc.mid) {
            const int delta = (int)rng.randomRange32(0, 2 * y) - y;
            m.len = max(1, m.len + delta);
        }
        return true;
    }
    case SAParams::FLIP_ONE_LR: {
        // ランダムなインデックスのmidのLRを反転
        if (mc.mid.empty())
            return false;
        const int idx = (int)rng.random32((u32)mc.mid.size());
        mc.mid[idx].dir = (mc.mid[idx].dir == 'L') ? 'R' : 'L';
        return true;
    }
    case SAParams::FLIP_ALL_LR: {
        // 全てのmidのLRをランダムに新規割り当て
        if (mc.mid.empty())
            return false;
        for (auto &m : mc.mid)
            m.dir = random_dir_val();
        return true;
    }
    case SAParams::DELETE_MID: {
        // ランダムなmidを削除
        if (mc.mid.empty())
            return false;
        const int idx = (int)rng.random32((u32)mc.mid.size());
        mc.mid.erase(mc.mid.begin() + idx);
        return true;
    }
    case SAParams::INSERT_MID: {
        // ランダムな位置にmidを追加
        const int fmin = 2;
        const int fmax = max(2, in.N / 2);
        Macro::Mid nm{(int)rng.randomRange32(fmin, fmax), random_dir_val()};
        const int pos = (int)rng.randomRange32(0, (u32)mc.mid.size());
        mc.mid.insert(mc.mid.begin() + pos, nm);
        return true;
    }
    case SAParams::CHANGE_PREFIX: {
        // プレフィックスをランダムに変更
        char np = random_prefix_val();
        while (np == mc.prefix)
            np = random_prefix_val();
        mc.prefix = np;
        return true;
    }
    case SAParams::CHANGE_SUFFIX: {
        // サフィックスをランダムに変更
        int ns = random_suffix_val();
        while (ns == mc.suffix)
            ns = random_suffix_val();
        mc.suffix = ns;
        return true;
    }
    case SAParams::SWAP_MID: {
        // ランダムな2つのmidを入れ替える
        const int k = (int)mc.mid.size();
        if (k < 2)
            return false;
        const int i = (int)rng.random32((u32)k);
        int j = (int)rng.random32((u32)k);
        if (i == j)
            return false;
        swap(mc.mid[i], mc.mid[j]);
        return true;
    }
    case SAParams::REVERSE_MID: {
        // ランダムな区間を反転する
        const int k = (int)mc.mid.size();
        if (k < 2)
            return false;
        int i = (int)rng.random32((u32)k);
        int j = (int)rng.random32((u32)k);
        if (i == j)
            return false;
        if (i > j)
            swap(i, j);
        reverse(mc.mid.begin() + i, mc.mid.begin() + j + 1);
        return true;
    }
    case SAParams::RELOCATE_MID: {
        // ランダムな1要素を別の位置へ移動する
        const int k = (int)mc.mid.size();
        if (k < 2)
            return false;
        const int i = (int)rng.random32((u32)k);
        const int ins = (int)rng.random32((u32)k);
        if (i == ins)
            return false;
        assert(0 <= ins && ins <= k - 1);
        Macro::Mid val = mc.mid[i];
        mc.mid.erase(mc.mid.begin() + i);
        mc.mid.insert(mc.mid.begin() + ins, val);
        return true;
    }
    case SAParams::UNIFORM_LEN: {
        // 全midの長さを共通値に揃える（ブートストラップ効率↑）
        if (mc.mid.empty())
            return false;
        int target;
        if (rng.random32(2)) {
            // 既存midのいずれかの長さに揃える
            target = mc.mid[rng.random32((u32)mc.mid.size())].len;
        } else {
            // [2, N/2] からランダムに選ぶ
            const int fmin = 2;
            const int fmax = max(2, in.N / 2);
            target = (int)rng.randomRange32(fmin, fmax);
        }
        for (auto &m : mc.mid)
            m.len = target;
        return true;
    }
    case SAParams::MOVE_LEN: {
        // 2つのmidを選び、片方からもう片方へ長さを移す
        // 渡す側が空にならない範囲で 1〜定数 の長さを移動
        const int k = (int)mc.mid.size();
        if (k < 2)
            return false;
        const int i = (int)rng.random32((u32)k); // 渡す側
        int j = (int)rng.random32((u32)k);       // 受け取る側
        if (i == j)
            return false;
        const int givable = mc.mid[i].len - 1; // 渡しても空にならない上限
        if (givable <= 0)
            return false;
        const int amount =
            (int)rng.randomRange32(1, min(tuning.move_len_max, givable));
        mc.mid[i].len -= amount;
        mc.mid[j].len += amount;
        return true;
    }
    default:
        return false;
    }
}

struct DefaultMoveApplier {
    using state_t = State;
    using score_t = state_t::score_t;
    const SAParams &params;
    explicit DefaultMoveApplier(const SAParams &params) : params(params) {
        saved_macro.reserve(in.T + 5);
    }
    string saved_macro; // rollback 用（orderはrollback不要、改善値を蓄積）
    Macro saved_struct;

    // macro_struct を文字列化して評価する
    optional<score_t> finalize(state_t &state, f64 score_threshold) {
        state.macro = state.macro_struct.to_string();
        if ((int)state.macro.size() > in.T)
            return nullopt;
        return state.calc_score_full((score_t)score_threshold);
    }

    optional<score_t> apply(state_t &state, int move_type,
                            f64 score_threshold) {
        saved_macro = state.macro; // reserve済みのためmallocなし
        saved_struct = state.macro_struct;
        if (!apply_macro_move(state.macro_struct, move_type,
                              macro_sa_move_tuning()))
            return nullopt;
        return finalize(state, score_threshold);
    }
    void rollback(state_t &state, int move_type) {
        state.macro = saved_macro;
        state.macro_struct = saved_struct;
        // bs / order は accept 時のみ更新しているため rollback での復元は不要。
        // order は現在受理済みマクロの山登り後状態のまま保たれる。
    }
    void accept(state_t &state, int move_type) {
        // 受理された候補の下界スコアと山登り後 order を現在状態へ確定する。
        state.bs = state.last_bs;
        state.order = state.last_order;
    }
};

// ============================================================
// マクロ端 (head/tail) への S/回転 焼き込みモデルの純粋ヘルパ群。
// improve_with_macro_both と、その一般化(区間候補つき DP) で共有する。
// 状態 s ∈ [0,64): bit0=hS, bit1-2=hR, bit3=tS, bit4-5=tR。
// head=(hS,hR), tail=(tS,tR)。回転は mod4、S パリティは mod2。
// ここにある関数はすべて副作用なし・盤面非依存(純粋)。
// ============================================================
namespace mbake {
// {S,L,R} 連の正味 (Sパリティ, 回転, 文字数)
inline array<int, 3> run_stats(const string &s) {
    int sp = 0, rn = 0;
    for (char c : s) {
        if (c == 'S')
            sp ^= 1;
        else if (c == 'R')
            rn = (rn + 1) & 3;
        else if (c == 'L')
            rn = (rn + 3) & 3;
    }
    return array<int, 3>{sp, rn, (int)s.size()};
}
inline bool is_slr(char c) { return c == 'S' || c == 'L' || c == 'R'; }
inline int corrlen_turn(int d) {
    d &= 3;
    return d == 0 ? 0 : (d == 2 ? 2 : 1);
}
inline int corrlen_s(int s, bool ball) { return (ball && s) ? 1 : 0; }
inline string turn_ops(int d) {
    d &= 3;
    if (d == 0)
        return "";
    if (d == 1)
        return "R";
    if (d == 2)
        return "RR";
    return "L";
}
// 状態 idx <-> (hS,hR,tS,tR)
inline int hS(int s) { return s & 1; }
inline int hR(int s) { return (s >> 1) & 3; }
inline int tS(int s) { return (s >> 3) & 1; }
inline int tR(int s) { return (s >> 4) & 3; }
// 1 個のマクロ端 (mS,mR) と元の連 (rS,rR)
// を同一セルで一致させる補正文字列。 (P の直前 or 直後に置く。順序: S →
// 回転。可換なので順序自由)
inline string indep_corr(int mS, int mR, int rS, int rR, bool ball) {
    const int eS = ball ? (rS ^ mS) : 0;
    const int eR = (rR - mR) & 3;
    return string(eS, 'S') + turn_ops(eR);
}
inline int indep_corr_len(int mS, int mR, int rS, int rR, bool ball) {
    const int eS = rS ^ mS;
    const int eR = (rR - mR) & 3;
    return corrlen_s(eS, ball) + corrlen_turn(eR);
}
inline int indep_gain(int mS, int mR, const array<int, 3> &run, bool ball) {
    return run[2] - indep_corr_len(mS, mR, run[0], run[1], ball);
}
// 同一セルに左Pの末尾(tS,tR)・元の連・右Pの先頭(hS,hR)。間に置く補正文字列。
inline string common_corr(int ts, int tr, int hs, int hr, int gS, int gR,
                          bool ball) {
    const int eS = ball ? (gS ^ ts ^ hs) : 0;
    const int eR = (gR - tr - hr) & 3;
    return string(eS, 'S') + turn_ops(eR);
}
inline int common_corr_len(int ts, int tr, int hs, int hr, int gS, int gR,
                           bool ball) {
    const int eS = gS ^ ts ^ hs;
    const int eR = (gR - tr - hr) & 3;
    return corrlen_s(eS, ball) + corrlen_turn(eR);
}
inline int common_gain(int ts, int tr, int hs, int hr, const array<int, 3> &run,
                       bool ball) {
    return run[2] - common_corr_len(ts, tr, hs, hr, run[0], run[1], ball);
}
// 片側の再録音追加文字列 (old(S,R) -> new(S,R))
inline string side_add(int oS, int oR, int nS, int nR) {
    return string(oS ^ nS, 'S') + turn_ops((nR - oR) & 3);
}
inline int side_add_len(int oS, int oR, int nS, int nR) {
    return (oS ^ nS) + corrlen_turn((nR - oR) & 3);
}
// bake 状態 a の登録マクロを b へ録り直す挿入列 / そのコスト(負=Aの増分)。
inline string block_str(int a, int b) {
    if (a == b)
        return "P";
    const string ha = side_add(hS(a), hR(a), hS(b), hR(b));
    const string ta = side_add(tS(a), tR(a), tS(b), tR(b));
    return "M" + ha + "P" + ta + "M";
}
inline int block_cost(int a, int b) {
    if (a == b)
        return 0;
    const int ha = side_add_len(hS(a), hR(a), hS(b), hR(b));
    const int ta = side_add_len(tS(a), tR(a), tS(b), tR(b));
    return -(ha + ta + 2); // "M" + ha + "P" + ta + "M" と "P" の差
}
} // namespace mbake

class Solver {
    State best;
    string answer;

    // パス列挙用の再利用バッファ（毎区間/毎呼び出しの再確保を避ける）。
    // seg_db_buf: 逆BFS距離。スタンプ方式で「未訪問」を表すため inf32
    // 埋めは不要。
    //   stamp[u] == seg_db_gen のときだけ db[u] が有効（その回で訪問済み）。
    // seg_rev_buf: 逆隣接リスト（inner vector の capacity を保持して再利用）。
    vector<int> seg_db_buf;
    vector<int> seg_db_stamp;
    int seg_db_gen = 0;
    vector<int> seg_bfs_q;
    vector<vector<int>> seg_rev_buf;

    // 暫定スコア計算の高速化キャッシュ。best.macro 固定の間（パスSA中）は
    // マクロのみに依存する量を一度だけ計算して使い回す。
    struct ProvMacroCache {
        string macro = "\x01"; // 未初期化を表す番兵（空マクロと区別）
        int implicit_max_len = -1;
        int max_frun = 0;
        vector<string> impl_cands; // 暗黙init_macro候補（部分文字列）
        unordered_map<string, int> enc_macro_size; // init_macro -> enc(macro)長
    } prov_cache;

    // enumerate_main_macro_substrings のメモ。引数(=best.macro)が固定の間は
    // 全部分文字列リストが不変なので再生成を避ける。const 参照で返すことで
    // 呼び出し側のコピーも消す（返り値のライフタイムはこのキャッシュに従う）。
    mutable string emms_key_ = "\x01"; // 未初期化番兵（空文字列と区別）
    mutable vector<string> emms_val_;

    void ensure_prov_cache(int implicit_max_len) {
        implicit_max_len = min(implicit_max_len, (int)best.macro.size() / 2);
        if (prov_cache.macro == best.macro &&
            prov_cache.implicit_max_len == implicit_max_len)
            return;
        prov_cache.macro = best.macro;
        prov_cache.implicit_max_len = implicit_max_len;
        prov_cache.max_frun =
            min(max_consecutive_f_in_macro(best.macro), implicit_max_len);
        prov_cache.impl_cands.clear();
        prov_cache.enc_macro_size.clear();
        if (best.macro.empty())
            return;
        for (int len = 2; len <= prov_cache.max_frun; len++) {
            string im(len, 'F');
            prov_cache.enc_macro_size[im] =
                (int)encode_with_registered_macro(best.macro, im).size();
        }
        for (const string &im : enumerate_main_macro_substrings(best.macro)) {
            if ((int)im.size() < 2 || (int)im.size() > implicit_max_len)
                continue;
            prov_cache.impl_cands.push_back(im);
            if (!prov_cache.enc_macro_size.count(im))
                prov_cache.enc_macro_size[im] =
                    (int)encode_with_registered_macro(best.macro, im).size();
        }
    }

    struct RestoredMacroCandidate {
        State state;
        string restored_answer;
        string raw_path_ops;
        i64 provisional_score = inf64;
        i64 init_best_score = inf64;
        int both_all_gain = 0;
    };

    int expanded_basic_count(const string &ops, int cap = inf32) const {
        string last_macro, recording_macro;
        bool recording = false;
        int e = 0;
        for (char ch : ops) {
            if (ch == 'M') {
                if (recording) {
                    last_macro = recording_macro;
                    recording_macro.clear();
                    recording = false;
                } else {
                    recording_macro.clear();
                    recording = true;
                }
            } else if (ch == 'P') {
                e += (int)last_macro.size();
                if (recording)
                    recording_macro += last_macro;
            } else {
                e++;
                if (recording)
                    recording_macro.push_back(ch);
            }
            if (e > cap)
                return e;
        }
        return e;
    }

    i64 output_score_for_limit(const string &ops) const {
        const int A = (int)ops.size();
        const int E = expanded_basic_count(ops, in.T);
        if (A > in.T || E > in.T)
            return inf64 / 4 + (i64)max(A, E);
        return A;
    }

    string registered_macro_from_answer(const string &ops) const {
        string last_macro, recording_macro;
        bool recording = false;
        for (char ch : ops) {
            if (ch == 'M') {
                if (recording) {
                    last_macro = recording_macro;
                    recording_macro.clear();
                    recording = false;
                } else {
                    recording_macro.clear();
                    recording = true;
                }
            } else if (recording) {
                if (ch == 'P') {
                    recording_macro += last_macro;
                } else {
                    recording_macro.push_back(ch);
                }
            }
        }
        return last_macro;
    }

    void log_answer_stage(const string &tag, const string &ops) const {
        if constexpr (DEBUG) {
            const i64 score = output_score_for_limit(ops);
            const int A = (int)ops.size();
            const int E = expanded_basic_count(ops, in.T);
            cerr << "[" << tag << "] score=" << score << " A=" << A
                 << " E=" << E << " T=" << in.T
                 << " macro=" << registered_macro_from_answer(ops) << el;
            cerr << "[score_log] stage=" << tag << " score=" << score
                 << " A=" << A << " E=" << E << " T=" << in.T << el;
        }
    }

    string build_plain_answer() {
        g_ctx.build("", true);
        const auto &ctx = g_ctx;
        const int n_steps = (int)best.order.size() * 2;
        vector<int> wp_cells;
        wp_cells.reserve(n_steps + 1);
        wp_cells.push_back(in.cell_id(0, 0));
        for (int idx : best.order) {
            wp_cells.push_back(in.ball_cells[idx]);
            wp_cells.push_back(in.basket_cells[idx]);
        }

        vector<array<Cost, 4>> dp(n_steps + 1);
        vector<array<int8_t, 4>> par(n_steps + 1);
        dp[0].fill(Cost{});
        dp[0][0] = {0, 0};
        par[0].fill(-1);
        for (int step = 0; step < n_steps; step++) {
            dp[step + 1].fill(Cost{});
            par[step + 1].fill(-1);
            const int cc = wp_cells[step];
            const int nc = wp_cells[step + 1];
            for (int d1 = 0; d1 < 4; d1++) {
                if (dp[step][d1].a >= inf32 / 4)
                    continue;
                const int src = ctx.sid_to_source[(cc << 2) | d1];
                if (src == -1)
                    continue;
                for (int d2 = 0; d2 < 4; d2++) {
                    const int to_sid = (nc << 2) | d2;
                    if (ctx.dist_stamp[src][to_sid] != ctx.build_gen)
                        continue;
                    const Cost nd =
                        add_cost(dp[step][d1], ctx.dist[src][to_sid]);
                    if (nd < dp[step + 1][d2]) {
                        dp[step + 1][d2] = nd;
                        par[step + 1][d2] = (int8_t)d1;
                    }
                }
            }
        }

        int best_d = 0;
        for (int d = 1; d < 4; d++) {
            if (dp[n_steps][d] < dp[n_steps][best_d])
                best_d = d;
        }
        vector<int> chosen(n_steps + 1);
        chosen[n_steps] = best_d;
        for (int step = n_steps; step > 0; step--)
            chosen[step - 1] = par[step][chosen[step]];

        string ops;
        for (int step = 0; step < n_steps; step++) {
            const int src =
                ctx.sid_to_source[(wp_cells[step] << 2) | chosen[step]];
            const int end_sid = (wp_cells[step + 1] << 2) | chosen[step + 1];
            ops += ctx.restore_path(src, end_sid);
            ops.push_back('S');
        }
        if ((int)ops.size() > in.T)
            ops.resize(in.T);
        return ops;
    }

    string materialize_first_macro_use(const string &ops) const {
        if (best.macro.empty())
            return ops;
        string res;
        res.reserve(ops.size() + best.macro.size() + 2);
        bool created = false;
        for (char ch : ops) {
            if (ch == 'P' && !created) {
                res.push_back('M');
                res += best.macro;
                res.push_back('M');
                created = true;
            } else {
                res.push_back(ch);
            }
        }
        return res;
    }

    string use_substring_macro_before_main_registration(
        const string &part1_ops, const string &main_macro,
        const string &part2_ops) const {
        string best_ans = part1_ops + "M" + main_macro + "M" + part2_ops;
        i64 best_sc = output_score_for_limit(best_ans);
        int best_a = (int)best_ans.size();

        // 「途中でメインマクロを作る」: 最初のマクロ使用(part2 の先頭の
        // P)までの 自然な復元経路 part1_ops
        // の中に、メインマクロと完全一致する連続区間が あれば、その区間を M..M
        // で囲むだけで登録できる。区間内の基本操作は元々
        // 経路上で実行されるので追加コストは M 2 個分の +2 のみ（最初に使う予定
        // だった P はそのまま残す）。F 連続に限らず回転を含むマクロでも有効で、
        // メインマクロ登録 +len+1 の baseline より良い。
        {
            const int L = (int)main_macro.size();
            for (int pos = 0; pos + L <= (int)part1_ops.size(); pos++) {
                if (part1_ops.compare(pos, L, main_macro) != 0)
                    continue;
                string cand;
                cand.reserve(part1_ops.size() + part2_ops.size() + 3);
                cand += part1_ops.substr(0, pos);
                cand.push_back('M');
                cand += main_macro;
                cand.push_back('M');
                cand += part1_ops.substr(pos + L);
                cand.push_back('P'); // first_p で使う予定だった P を復活
                cand += part2_ops;
                const i64 sc = output_score_for_limit(cand);
                const int a = (int)cand.size();
                if (sc < best_sc || (sc == best_sc && a < best_a)) {
                    best_sc = sc;
                    best_a = a;
                    best_ans = cand;
                }
                break; // どの一致位置でも +2 で同コストなので最初の一致のみ
            }
        }

        for (const string &init_macro :
             enumerate_main_macro_substrings(main_macro)) {
            if (init_macro.size() == main_macro.size())
                continue;
            const int pos = (int)part1_ops.find(init_macro);
            if (pos == (int)string::npos)
                continue;

            string part1;
            part1.reserve(part1_ops.size() + 2);
            part1 += part1_ops.substr(0, pos);
            part1.push_back('M');
            part1 += init_macro;
            part1.push_back('M');
            part1 += encode_with_registered_macro(
                part1_ops.substr(pos + (int)init_macro.size()), init_macro);

            const string recording =
                encode_with_registered_macro(main_macro, init_macro);
            string cand = part1 + "M" + recording + "M" + part2_ops;
            const i64 sc = output_score_for_limit(cand);
            const int a = (int)cand.size();
            if (sc < best_sc || (sc == best_sc && a < best_a)) {
                best_sc = sc;
                best_a = a;
                best_ans = cand;
            }
        }
        return best_ans;
    }

    struct InitialSimResult {
        int sid = -1;
        int e = 0;
        string last_macro;
    };

    InitialSimResult simulate_initial_ops(const string &ops) const {
        string last_macro, recording_macro;
        bool recording = false;
        InitialSimResult res;
        res.sid = in.state_id(0, 0, 0);
        auto apply_basic = [&](char ch) {
            if (ch == 'F')
                res.sid = in.base_next[res.sid][0];
            else if (ch == 'L')
                res.sid = in.base_next[res.sid][1];
            else if (ch == 'R')
                res.sid = in.base_next[res.sid][2];
            res.e++;
        };
        for (char ch : ops) {
            if (ch == 'M') {
                if (recording) {
                    last_macro = recording_macro;
                    recording_macro.clear();
                    recording = false;
                } else {
                    recording_macro.clear();
                    recording = true;
                }
            } else if (ch == 'P') {
                for (char b : last_macro)
                    apply_basic(b);
                if (recording)
                    recording_macro += last_macro;
            } else {
                apply_basic(ch);
                if (recording)
                    recording_macro.push_back(ch);
            }
        }
        res.last_macro = last_macro;
        return res;
    }

    // 復元経路の最終正規化。F/P/M で区切られた {L,R,S} の連の中では回転(L/R)と
    // swap(S) は可換なので、回転を S の前にまとめる(SL/SR ではなく LS/RS
    // の順)。 90 度回転は最小表現(R=+1, L=+3)で一意。180
    // 度回転(RR=LL)は、強制的に 決まる 90 度回転の LS/RS の出現数を比べ、LS
    // が多ければ LL、そうでなければ RR に揃える(RRS/SRR ⇔ LLS)。S
    // を含まない連は触らない。
    static string normalize_rotation_swap(const string &ops) {
        const int n = (int)ops.size();
        auto is_rs = [](char c) { return c == 'L' || c == 'R' || c == 'S'; };
        // 180 度回転の向きを、強制的に決まる 90 度回転(rot==1→RS, rot==3→LS)の
        // 出現数で決める。
        int ls_fixed = 0, rs_fixed = 0;
        for (int i = 0; i < n;) {
            if (!is_rs(ops[i])) {
                i++;
                continue;
            }
            int j = i, rot = 0, s_count = 0;
            for (; j < n && is_rs(ops[j]); j++) {
                if (ops[j] == 'R')
                    rot = (rot + 1) & 3;
                else if (ops[j] == 'L')
                    rot = (rot + 3) & 3;
                else
                    s_count++;
            }
            if (s_count > 0) {
                if (rot == 1)
                    rs_fixed++;
                else if (rot == 3)
                    ls_fixed++;
            }
            i = j;
        }
        const bool use_ll = ls_fixed > rs_fixed;

        string res;
        res.reserve(n);
        for (int i = 0; i < n;) {
            if (!is_rs(ops[i])) {
                res.push_back(ops[i]);
                i++;
                continue;
            }
            int j = i, rot = 0, s_count = 0;
            for (; j < n && is_rs(ops[j]); j++) {
                if (ops[j] == 'R')
                    rot = (rot + 1) & 3;
                else if (ops[j] == 'L')
                    rot = (rot + 3) & 3;
                else
                    s_count++;
            }
            if (s_count == 0) {
                // S を含まない回転連はそのまま。
                res.append(ops, i, j - i);
            } else {
                if (rot == 1)
                    res.push_back('R');
                else if (rot == 3)
                    res.push_back('L');
                else if (rot == 2)
                    res.append(use_ll ? "LL" : "RR");
                res.append(s_count, 'S');
            }
            i = j;
        }
        return res;
    }

    // 呼び出し側で g_ctx.build(best.macro, true) 済みであること(マクロのみに
    // 依存し開始位置に依らないため、開始 sid ごとに作り直す必要はない)。
    string build_path_from_start_state(int start_sid,
                                       const vector<int> &order) {
        const auto &ctx = g_ctx;
        const int n = (int)order.size();
        const int n_steps = n * 2;

        // スタート状態(sources に無い任意状態)から最初のボールセルまでを
        // 1 回の BFS で探索する。dp[1] 用の 4 方向コストと、最初の区間の経路
        // 復元用の親を同時に得る(従来はコスト用と復元用で 2 回 Dijkstra して
        // いた)。ゴールはセル単位なので既存 run_bfs と同様、初めて到達した a 層
        // の +1 以降は枝刈りする(スタート↔最初のボールは 1:1)。
        const int S = in.N * in.N * 4;
        vector<Cost> sdist(S);
        vector<ParentStep> spar(S);
        array<Cost, 4> first_ball_cost;
        first_ball_cost.fill(Cost{});
        const int first_cell = (n_steps > 0) ? in.ball_cells[order[0]] : -1;
        if (n_steps > 0) {
            vector<int> buf;
            buf.reserve(S);
            sdist[start_sid] = {0, 0};
            buf.push_back(start_sid);
            const int macro_e = (int)ctx.macro.size();
            int stop_a = inf32, head = 0;
            static constexpr char ops3[3] = {'F', 'L', 'R'};
            while (head < (int)buf.size()) {
                const int sid = buf[head++];
                const Cost cur = sdist[sid];
                if (cur.a >= stop_a)
                    break;
                const int na = cur.a + 1, ne1 = cur.e + 1;
                for (int k = 0; k < 3; k++) {
                    const int to = ctx.next4[sid][k];
                    if (sdist[to].a >= inf32 / 4) {
                        sdist[to] = {na, ne1};
                        spar[to] = {(i16)sid, ops3[k]};
                        buf.push_back(to);
                        if (stop_a == inf32 && (to >> 2) == first_cell)
                            stop_a = na + 1;
                    } else if (na == sdist[to].a && ne1 < sdist[to].e) {
                        sdist[to].e = ne1;
                        spar[to] = {(i16)sid, ops3[k]};
                    }
                }
                if (macro_e > 0) {
                    const int to = ctx.next4[sid][3];
                    const int nep = cur.e + macro_e;
                    if (sdist[to].a >= inf32 / 4) {
                        sdist[to] = {na, nep};
                        spar[to] = {(i16)sid, 'P'};
                        buf.push_back(to);
                        if (stop_a == inf32 && (to >> 2) == first_cell)
                            stop_a = na + 1;
                    } else if (na == sdist[to].a && nep < sdist[to].e) {
                        sdist[to].e = nep;
                        spar[to] = {(i16)sid, 'P'};
                    }
                }
            }
            for (int d = 0; d < 4; d++)
                first_ball_cost[d] = sdist[(first_cell << 2) | d];
        }

        vector<int> wp_cells;
        wp_cells.reserve(n_steps);
        for (int idx : order) {
            wp_cells.push_back(in.ball_cells[idx]);
            wp_cells.push_back(in.basket_cells[idx]);
        }

        vector<array<Cost, 4>> dp(n_steps + 1);
        vector<array<int8_t, 4>> par(n_steps + 1);
        dp[0].fill(Cost{});
        par[0].fill(-1);
        if (n_steps > 0) {
            dp[1] = first_ball_cost;
            par[1].fill(-2);
        }
        for (int step = 1; step < n_steps; step++) {
            dp[step + 1].fill(Cost{});
            par[step + 1].fill(-1);
            const int cc = wp_cells[step - 1];
            const int nc = wp_cells[step];
            for (int d1 = 0; d1 < 4; d1++) {
                if (dp[step][d1].a >= inf32 / 4)
                    continue;
                const int src = ctx.sid_to_source[(cc << 2) | d1];
                if (src == -1)
                    continue;
                for (int d2 = 0; d2 < 4; d2++) {
                    const int to_sid = (nc << 2) | d2;
                    if (ctx.dist_stamp[src][to_sid] != ctx.build_gen)
                        continue;
                    const Cost nd =
                        add_cost(dp[step][d1], ctx.dist[src][to_sid]);
                    if (nd < dp[step + 1][d2]) {
                        dp[step + 1][d2] = nd;
                        par[step + 1][d2] = (int8_t)d1;
                    }
                }
            }
        }

        int best_d = 0;
        for (int d = 1; d < 4; d++) {
            if (dp[n_steps][d] < dp[n_steps][best_d])
                best_d = d;
        }
        vector<int> chosen(n_steps + 1);
        chosen[n_steps] = best_d;
        for (int step = n_steps; step > 1; step--)
            chosen[step - 1] = par[step][chosen[step]];

        string ops;
        if (n_steps > 0) {
            const int first_end = (wp_cells[0] << 2) | chosen[1];
            // 上の BFS で記録した親を辿って復元(2 回目の探索は不要)
            string fp;
            int cur = first_end;
            while (cur != start_sid && spar[cur].prev >= 0) {
                fp.push_back(spar[cur].move);
                cur = spar[cur].prev;
            }
            reverse(fp.begin(), fp.end());
            ops += fp;
            ops.push_back('S');
        }
        for (int step = 1; step < n_steps; step++) {
            const int src =
                ctx.sid_to_source[(wp_cells[step - 1] << 2) | chosen[step]];
            const int end_sid = (wp_cells[step] << 2) | chosen[step + 1];
            ops += ctx.restore_path(src, end_sid);
            ops.push_back('S');
        }
        return normalize_rotation_swap(ops);
    }

    static string encode_with_registered_macro(const string &ops,
                                               const string &macro) {
        if (macro.empty())
            return ops;
        const int n = (int)ops.size();
        const int L = (int)macro.size();
        vector<int> dp(n + 1, inf32);
        vector<char> take_p(n + 1, 0);
        dp[n] = 0;
        for (int i = n - 1; i >= 0; i--) {
            dp[i] = dp[i + 1] + 1;
            if (i + L <= n && ops.compare(i, L, macro) == 0 &&
                dp[i + L] + 1 < dp[i]) {
                dp[i] = dp[i + L] + 1;
                take_p[i] = 1;
            }
        }

        string res;
        res.reserve(dp[0]);
        for (int i = 0; i < n;) {
            if (take_p[i]) {
                res.push_back('P');
                i += L;
            } else {
                res.push_back(ops[i++]);
            }
        }
        return res;
    }

    string build_initial_macro_answer_with_path(const string &init_ops,
                                                const string &init_macro,
                                                const string &path_ops) {
        const int first_p = (int)path_ops.find('P');
        if (first_p == (int)string::npos) {
            return init_ops +
                   encode_with_registered_macro(path_ops, init_macro);
        }

        const string recording =
            encode_with_registered_macro(best.macro, init_macro);
        string ans = init_ops;
        ans += encode_with_registered_macro(path_ops.substr(0, first_p),
                                            init_macro);
        ans.push_back('M');
        ans += recording;
        ans.push_back('M');
        ans += path_ops.substr(first_p + 1);
        return ans;
    }

    // 返り値は内部メモへの const 参照。呼び出し側は、返り値（または
    // それを走査する range-for）が生きている間に別マクロで本関数を再呼び出し
    // しないこと（メモが上書きされ参照が無効化される）。現状の呼び出し箇所は
    // いずれもループ本体内で本関数を呼ばないので安全。
    const vector<string> &
    enumerate_main_macro_substrings(const string &main_macro) const {
        if (emms_key_ == main_macro)
            return emms_val_;
        vector<string> res;
        unordered_set<string> seen;
        for (int l = 0; l < (int)main_macro.size(); l++) {
            for (int r = l + 1; r <= (int)main_macro.size(); r++) {
                string s = main_macro.substr(l, r - l);
                if (seen.insert(s).second)
                    res.push_back(std::move(s));
            }
        }
        sort(res.begin(), res.end(), [](const string &a, const string &b) {
            if (a.size() != b.size())
                return a.size() < b.size();
            return a < b;
        });
        emms_key_ = main_macro;
        emms_val_ = std::move(res);
        return emms_val_;
    }

    int simulate_basic_ops_from(int sid, const string &ops) const {
        int cur = sid;
        for (char ch : ops) {
            if (ch == 'F')
                cur = in.base_next[cur][0];
            else if (ch == 'L')
                cur = in.base_next[cur][1];
            else if (ch == 'R')
                cur = in.base_next[cur][2];
        }
        return cur;
    }

    vector<pair<int, string>>
    enumerate_init_macro_start_paths(int max_dist) const {
        const int S = in.N * in.N * 4;
        const int start = in.state_id(0, 0, 0);
        vector<int> dist(S, inf32);
        vector<ParentStep> par(S);
        vector<int> buf;
        buf.reserve(S);
        dist[start] = 0;
        buf.push_back(start);
        int head = 0;
        static constexpr char ops3[3] = {'F', 'L', 'R'};
        while (head < (int)buf.size()) {
            const int sid = buf[head++];
            if (dist[sid] >= max_dist)
                continue;
            for (int k = 0; k < 3; k++) {
                const int to = in.base_next[sid][k];
                if (dist[to] <= dist[sid] + 1)
                    continue;
                dist[to] = dist[sid] + 1;
                par[to] = {(i16)sid, ops3[k]};
                buf.push_back(to);
            }
        }

        vector<pair<int, string>> starts;
        starts.reserve(buf.size());
        for (int sid : buf) {
            if (dist[sid] > max_dist)
                continue;
            string path;
            int cur = sid;
            while (cur != start) {
                path.push_back(par[cur].move);
                cur = par[cur].prev;
            }
            reverse(path.begin(), path.end());
            starts.push_back({sid, path});
        }
        sort(starts.begin(), starts.end(), [](const auto &a, const auto &b) {
            if (a.second.size() != b.second.size())
                return a.second.size() < b.second.size();
            if (a.first != b.first)
                return a.first < b.first;
            return a.second < b.second;
        });
        return starts;
    }

    string improve_without_explicit_initial_macro(
        const string &base_ans, const string &raw_path_ops,
        const vector<string> &init_macros) const {
        const int first_p = (int)raw_path_ops.find('P');
        if (first_p == (int)string::npos)
            return base_ans;

        const string part1_ops = raw_path_ops.substr(0, first_p);
        const string part2_ops = raw_path_ops.substr(first_p + 1);
        string best_ans = base_ans;
        i64 best_sc = output_score_for_limit(best_ans);
        int best_a = (int)best_ans.size();

        for (const string &init_macro : init_macros) {
            const int L = (int)init_macro.size();
            for (int pos = 0; pos + L <= (int)part1_ops.size(); pos++) {
                if (part1_ops.compare(pos, L, init_macro) != 0)
                    continue;

                string cand;
                cand.reserve(part1_ops.size() + part2_ops.size() +
                             best.macro.size() + init_macro.size() + 8);
                cand += part1_ops.substr(0, pos);
                cand.push_back('M');
                cand += init_macro;
                cand.push_back('M');
                cand += encode_with_registered_macro(part1_ops.substr(pos + L),
                                                     init_macro);
                cand.push_back('M');
                cand += encode_with_registered_macro(best.macro, init_macro);
                cand.push_back('M');
                cand += part2_ops;

                const i64 sc = output_score_for_limit(cand);
                const int a = (int)cand.size();
                if (sc < best_sc || (sc == best_sc && a < best_a)) {
                    best_sc = sc;
                    best_a = a;
                    best_ans = std::move(cand);
                }
                break;
            }
        }
        return best_ans;
    }

    string improve_with_initial_macro_candidates(const string &base_ans,
                                                 const vector<int> &order,
                                                 const string &raw_path_ops) {
        if (best.macro.empty())
            return base_ans;
        string best_ans = base_ans;
        i64 best_sc = output_score_for_limit(best_ans);
        int best_a = (int)best_ans.size();
        int best_e = expanded_basic_count(best_ans, in.T);
        string best_init_macro;
        string best_init_ops;

        // 部分文字列の長さ上限。これが無いと L が大きいときに O(L^2) 個の
        // init_macro 候補が全て走り、事後最適化が肥大化する。上限は min(10, L/2)。
        const int init_macro_len_cap = min(10, (int)best.macro.size() / 2);
        vector<string> init_macros;
        for (const string &im : enumerate_main_macro_substrings(best.macro))
            if ((int)im.size() <= init_macro_len_cap)
                init_macros.push_back(im);

        {
            string cand = improve_without_explicit_initial_macro(
                best_ans, raw_path_ops, init_macros);
            const i64 sc = output_score_for_limit(cand);
            const int a = (int)cand.size();
            if (sc < best_sc || (sc == best_sc && a < best_a)) {
                best_sc = sc;
                best_a = a;
                best_e = expanded_basic_count(cand, in.T);
                best_ans = std::move(cand);
                best_init_ops = "(implicit)";
            }
        }

        static constexpr int INIT_MACRO_START_DIST = 3;
        const auto start_paths =
            enumerate_init_macro_start_paths(INIT_MACRO_START_DIST);
        vector<string> petit_by_sid(in.N * in.N * 4);
        for (const auto &[sid, petit] : start_paths)
            petit_by_sid[sid] = petit;

        vector<vector<pair<int, string>>> by_after(in.N * in.N * 4);
        for (const auto &[start_sid, petit] : start_paths) {
            (void)start_sid;
            const int sid_after_petit =
                simulate_basic_ops_from(in.state_id(0, 0, 0), petit);
            for (const string &init_macro : init_macros) {
                const int after =
                    simulate_basic_ops_from(sid_after_petit, init_macro);
                by_after[after].push_back({sid_after_petit, init_macro});
            }
        }

        unordered_map<int, string> path_cache;
        for (int after = 0; after < (int)by_after.size(); after++) {
            if (by_after[after].empty())
                continue;
            auto it_path = path_cache.find(after);
            if (it_path == path_cache.end()) {
                it_path =
                    path_cache
                        .emplace(after,
                                 build_path_from_start_state(after, order))
                        .first;
            }
            for (const auto &[before, init_macro] : by_after[after]) {
                const string &petit = petit_by_sid[before];
                string init_ops;
                init_ops.reserve(petit.size() + init_macro.size() + 2);
                init_ops += petit;
                init_ops.push_back('M');
                init_ops += init_macro;
                init_ops.push_back('M');

                string cand = build_initial_macro_answer_with_path(
                    init_ops, init_macro, it_path->second);
                const i64 sc = output_score_for_limit(cand);
                const int a = (int)cand.size();
                if (sc < best_sc || (sc == best_sc && a < best_a)) {
                    best_sc = sc;
                    best_a = a;
                    best_e = expanded_basic_count(cand, in.T);
                    best_init_macro = init_macro;
                    best_init_ops = init_ops;
                    best_ans = std::move(cand);
                }
            }
        }
        if constexpr (DEBUG) {
            if (!best_init_macro.empty() || !best_init_ops.empty()) {
                cerr << "[init_macro] len=" << best_init_macro.size()
                     << " A=" << best_a << " E=" << best_e
                     << " init=" << best_init_ops << el;
            }
        }
        return best_ans;
    }

    string improve_with_tail_macro(const string &base_ans) const {
        const int n = (int)base_ans.size();
        string rev = base_ans;
        reverse(rev.begin(), rev.end());
        const int q = (int)rev.find('P');
        if (q == (int)string::npos)
            return base_ans;

        struct Candidate {
            int gain = 0;
            int l = -1;
            int r = -1;
            vector<int> match_starts;
        };
        Candidate best_cand;

        for (int l = max(0, q - 3); l <= q; l++) {
            for (int r = q; r <= min(n - 1, q + 3); r++) {
                const int len = r - l + 1;
                bool has_m = false;
                for (int k = l; k <= r; k++) {
                    if (rev[k] == 'M') {
                        has_m = true;
                        break;
                    }
                }
                if (has_m)
                    continue;

                // 小窓 [l,r] (アンカー P = 元の最後の P = rev の q を含む) を 1
                // ブロックとし、奥(元の前方)へ同じブロックを探して集める。
                //  - 窓と窓の間の隙間は P 以外(F/L/R/S)で埋まっていてよい。
                //  - ただし隙間に P があってはいけない: 隙間で初めて踏んだ P
                //  は次窓の
                //    アンカーのはずで、そこで完全一致する窓が作れなければ停止する
                //    (= 巻き込まれ P を残さない)。
                //  - M
                //  を踏んでも停止。これにより登録〜各置換の間に再録音が無く、
                //    再生マクロが一意 = 畳み込みは構成的に V 不変。
                // アンカー P を q に揃えるため、窓内のアンカー位置 off
                // を用いる。
                const int off = q - l;
                vector<int> starts;
                int last_end = r;
                int i = r + 1;
                while (i < n) {
                    if (rev[i] == 'M')
                        break; // M 停止
                    if (rev[i] != 'P') {
                        i++; // 隙間の埋め草 (P/M 以外) は許容
                        continue;
                    }
                    // 隙間後に初めて踏んだ P。これを次窓のアンカーとみなす。
                    const int s = i - off;
                    if (s <= last_end || s + len > n)
                        break; // 想定位置にアンカーが無い = 巻き込まれ P → 停止
                    bool blk_m = false;
                    for (int k = s; k < s + len; k++) {
                        if (rev[k] == 'M') {
                            blk_m = true;
                            break;
                        }
                    }
                    if (blk_m)
                        break;
                    if (rev.compare(l, len, rev, s, len) != 0)
                        break; // 完全一致しない P = 巻き込まれ P → 停止
                    starts.push_back(s);
                    last_end = s + len - 1;
                    i = s + len; // 受理した窓の内部 P を飛ばす
                }
                if (starts.empty())
                    continue;

                const int gain = (len - 1) * (int)starts.size() - 2;
                if (gain <= 0)
                    continue;
                if (gain > best_cand.gain ||
                    (gain == best_cand.gain &&
                     starts.back() > best_cand.match_starts.back())) {
                    best_cand = {gain, l, r, starts};
                }
            }
        }
        if (best_cand.gain <= 0) {
            if constexpr (DEBUG) {
                cerr << "[tail_macro] gain=0 applied=0" << el;
            }
            return base_ans;
        }

        struct Interval {
            int lo = 0;
            int hi = 0;
            int type = 0; // 0: replace by P, 1: wrap by M..M
        };
        auto to_original_interval = [&](int rev_l, int len, int type) {
            return Interval{n - (rev_l + len), n - 1 - rev_l, type};
        };

        const int len = best_cand.r - best_cand.l + 1;
        vector<Interval> intervals;
        intervals.reserve(best_cand.match_starts.size() + 1);
        intervals.push_back(to_original_interval(best_cand.l, len, 0));
        for (int i = 0; i + 1 < (int)best_cand.match_starts.size(); i++) {
            intervals.push_back(
                to_original_interval(best_cand.match_starts[i], len, 0));
        }
        intervals.push_back(
            to_original_interval(best_cand.match_starts.back(), len, 1));
        sort(intervals.begin(), intervals.end(),
             [](const Interval &a, const Interval &b) {
                 return tie(a.lo, a.hi, a.type) < tie(b.lo, b.hi, b.type);
             });
        for (int i = 1; i < (int)intervals.size(); i++) {
            if (intervals[i].lo <= intervals[i - 1].hi)
                return base_ans;
        }

        string cand;
        cand.reserve(base_ans.size() - best_cand.gain + 8);
        int pos = 0;
        for (const Interval &iv : intervals) {
            cand += base_ans.substr(pos, iv.lo - pos);
            if (iv.type == 1) {
                cand.push_back('M');
                cand += base_ans.substr(iv.lo, iv.hi - iv.lo + 1);
                cand.push_back('M');
            } else {
                cand.push_back('P');
            }
            pos = iv.hi + 1;
        }
        cand += base_ans.substr(pos);

        // 隣接タイル化により畳み込みは構成的に V
        // 不変なはず。来ないことを期待して assert で守る
        // (壊れたら設計の誤りなので即落として顕在化させる)。
        assert(simulate_final_config(cand) == simulate_final_config(base_ans));

        const i64 base_sc = output_score_for_limit(base_ans);
        const i64 cand_sc = output_score_for_limit(cand);
        if (cand_sc < base_sc ||
            (cand_sc == base_sc && cand.size() < base_ans.size())) {
            if constexpr (DEBUG) {
                cerr << "[tail_macro] gain=" << best_cand.gain << " applied=1"
                     << " rev_l=" << best_cand.l << " rev_r=" << best_cand.r
                     << " cnt=" << best_cand.match_starts.size()
                     << " A=" << cand.size()
                     << " E=" << expanded_basic_count(cand, in.T) << el;
            }
            return cand;
        }
        if constexpr (DEBUG) {
            cerr << "[tail_macro] gain=" << best_cand.gain << " applied=0"
                 << " base_sc=" << base_sc << " cand_sc=" << cand_sc
                 << " base_A=" << base_ans.size() << " cand_A=" << cand.size()
                 << " cand_E=" << expanded_basic_count(cand, in.T) << el;
        }
        return base_ans;
    }

    // 既存マクロを末尾 F で延長して "PF" 群を畳む最適化。
    //
    // アイデア: 直前に登録済みのマクロを X とすると、操作列中の "PF" は「X
    // を再生して F する」を意味する。ある "PF" を MPFM で括ると、新しいマクロ
    // XF (X の展開 + F) が登録される。以降の "PF" は "P" 1 文字で同じ動作を
    // 再現でき、各 1 文字短縮できる。
    //
    // 状態シミュレーションで各手 (MPLRSF) 直後の位置・向きを記録し、後ろから
    // 走査する。
    //  - P の次が F: 変換可能 (PF->P)。これを +1 と数える。登録点をここにした
    //    場合の短縮量 = (自分以降の変換可能 PF 個数) - 3 を評価する
    //    (登録点は MPFM で +2、それ以外の変換可能 PF は -1 ずつ)。
    //  - P の直後の F が無効 (壁で動かない=同じ場所/向きに留まる)、もしくは
    //    以降に操作が無い: +0。マクロを XF にしても V
    //    は不変なので無害。何もせず 走査を続ける。
    //  - それ以外の P / M: 打ち切る。
    // tail 畳み込みとは別系統で、最終最適化で並列に試して良い方を採る。
    string improve_with_pf_extend(const string &base_ans) const {
        const int n = (int)base_ans.size();
        if (n == 0)
            return base_ans;

        // 各手 (MPLRSF) 実行直後の位置・向きを記録する。
        vector<int> post_r(n), post_c(n), post_dir(n);
        {
            int pr = 0, pc = 0, pdir = 0;
            string last_macro, rec;
            bool recording = false;
            static constexpr int dr[4] = {0, 1, 0, -1};
            static constexpr int dc[4] = {1, 0, -1, 0};
            auto do_basic = [&](char ch) {
                if (ch == 'F') {
                    if (in.can_move(pr, pc, pdir)) {
                        pr += dr[pdir];
                        pc += dc[pdir];
                    }
                } else if (ch == 'R') {
                    pdir = (pdir + 1) & 3;
                } else if (ch == 'L') {
                    pdir = (pdir + 3) & 3;
                }
                // S は位置・向きに影響しない
            };
            for (int i = 0; i < n; i++) {
                char ch = base_ans[i];
                if (ch == 'M') {
                    if (recording) {
                        last_macro = rec;
                        rec.clear();
                        recording = false;
                    } else {
                        rec.clear();
                        recording = true;
                    }
                } else if (ch == 'P') {
                    for (char mc : last_macro) {
                        do_basic(mc);
                        if (recording)
                            rec.push_back(mc);
                    }
                } else {
                    do_basic(ch);
                    if (recording)
                        rec.push_back(ch);
                }
                post_r[i] = pr;
                post_c[i] = pc;
                post_dir[i] = pdir;
            }
        }

        // 後ろから走査して登録点 reg_i と変換対象を決める。
        // cum = 自分より後方 (排他) の変換可能 PF
        // 個数。登録点をここにした場合の 短縮量 gain = cum - 2 (登録点 PF ->
        // MPFM で +2、後方 PF -> P で -cum)。cum
        // は後方へ進むほど単調増加するので最も後方の変換可能 PF が最大。
        int reg_i = -1; // 登録点 (MPFM にする P の位置)
        int best_gain = 0;
        int cum = 0;
        vector<int> conv_ps; // 変換可能 PF の P インデックス (降順)
        for (int i = n - 1; i >= 0; i--) {
            const char ch = base_ans[i];
            if (ch == 'M')
                break;
            if (ch != 'P')
                continue;
            const bool next_is_f = (i + 1 < n && base_ans[i + 1] == 'F');
            if (next_is_f) {
                const int gain = cum - 2; // ここを登録点にした場合の短縮量
                if (gain > best_gain) {
                    best_gain = gain;
                    reg_i = i;
                }
                cum++;
                conv_ps.push_back(i);
            } else {
                // 無害判定: 直後の F が無効 (同じ場所/向きに留まる) か末尾
                const bool f_noop =
                    !in.can_move(post_r[i], post_c[i], post_dir[i]);
                const bool at_end = (i + 1 >= n);
                if (f_noop || at_end) {
                    // +0: 何もしない (走査継続)
                } else {
                    break; // 打ち切り
                }
            }
        }

        if (best_gain <= 0 || reg_i < 0)
            return base_ans;

        // reg_i は MPFM、reg_i より後方の変換可能 PF は PF->P。
        // conv_ps は降順なので、reg_i 以降がすべて対象 (reg_i が最も後方の
        // 変換可能 PF)。
        vector<char> act(n, 0); // 0:通常 1:登録P 2:変換P 3:Fスキップ
        int conv_cnt = 0;
        for (int p : conv_ps) {
            if (p < reg_i)
                continue;
            assert(p + 1 < n && base_ans[p + 1] == 'F');
            act[p] = (p == reg_i) ? 1 : 2;
            act[p + 1] = 3;
            conv_cnt++;
        }

        string cand;
        cand.reserve(n + 2);
        for (int i = 0; i < n; i++) {
            if (act[i] == 1) {
                cand += "MPFM";
            } else if (act[i] == 2) {
                cand.push_back('P');
            } else if (act[i] == 3) {
                // F は削除済み
            } else {
                cand.push_back(base_ans[i]);
            }
        }

        // 構成的に最終ボール配置 (V) は不変のはず。来ないことを期待して
        // assert。
        assert(simulate_final_config(cand) == simulate_final_config(base_ans));

        const i64 base_sc = output_score_for_limit(base_ans);
        const i64 cand_sc = output_score_for_limit(cand);
        if (cand_sc < base_sc ||
            (cand_sc == base_sc && cand.size() < base_ans.size())) {
            if constexpr (DEBUG) {
                cerr << "[pf_extend] gain=" << best_gain << " applied=1"
                     << " reg_i=" << reg_i << " conv=" << conv_cnt
                     << " A=" << cand.size()
                     << " E=" << expanded_basic_count(cand, in.T) << el;
            }
            return cand;
        }
        if constexpr (DEBUG) {
            cerr << "[pf_extend] gain=" << best_gain << " applied=0"
                 << " base_sc=" << base_sc << " cand_sc=" << cand_sc
                 << " base_A=" << base_ans.size() << " cand_A=" << cand.size()
                 << el;
        }
        return base_ans;
    }

    // improve_with_pf_extend の "FP" (F が前) 版。
    //
    // 直前マクロ X に対し "FP" は「F してから X を再生」を意味する。"FP" を
    // MFPM で括ると新しいマクロ FX (F + X の展開) が登録され、以降の "FP" は
    // "P" 1 文字で同じ動作を再現できる。延長 F は再生の「先頭」に来るため、
    // 無害判定は P 実行「前」の状態で F が無効 (壁で動かない) かどうかで行う
    // (PF と違い、末尾でも先頭 F が動けば X の実行位置が変わり V が壊れるので
    // 末尾の特例は無い)。
    //  - P の前 (直前の文字) が F: 変換可能 (FP->P)。+1。
    //  - P 実行前の状態から F が無効 (同じ場所/向きに留まる): +0 (無害)。
    //  - それ以外の P / M: 打ち切る。
    string improve_with_fp_extend(const string &base_ans) const {
        const int n = (int)base_ans.size();
        if (n == 0)
            return base_ans;

        // 各手 (MPLRSF) 実行直後の位置・向きを記録する。
        vector<int> post_r(n), post_c(n), post_dir(n);
        {
            int pr = 0, pc = 0, pdir = 0;
            string last_macro, rec;
            bool recording = false;
            static constexpr int dr[4] = {0, 1, 0, -1};
            static constexpr int dc[4] = {1, 0, -1, 0};
            auto do_basic = [&](char ch) {
                if (ch == 'F') {
                    if (in.can_move(pr, pc, pdir)) {
                        pr += dr[pdir];
                        pc += dc[pdir];
                    }
                } else if (ch == 'R') {
                    pdir = (pdir + 1) & 3;
                } else if (ch == 'L') {
                    pdir = (pdir + 3) & 3;
                }
                // S は位置・向きに影響しない
            };
            for (int i = 0; i < n; i++) {
                char ch = base_ans[i];
                if (ch == 'M') {
                    if (recording) {
                        last_macro = rec;
                        rec.clear();
                        recording = false;
                    } else {
                        rec.clear();
                        recording = true;
                    }
                } else if (ch == 'P') {
                    for (char mc : last_macro) {
                        do_basic(mc);
                        if (recording)
                            rec.push_back(mc);
                    }
                } else {
                    do_basic(ch);
                    if (recording)
                        rec.push_back(ch);
                }
                post_r[i] = pr;
                post_c[i] = pc;
                post_dir[i] = pdir;
            }
        }
        // P 実行前の状態 = 直前の手の直後の状態 (先頭は初期状態 (0,0) 右向き)。
        auto pre_state = [&](int i) {
            if (i == 0)
                return array<int, 3>{0, 0, 0};
            return array<int, 3>{post_r[i - 1], post_c[i - 1], post_dir[i - 1]};
        };

        // 後ろから走査して登録点 reg_i と変換対象を決める。
        int reg_i = -1; // 登録点 (MFPM にする P の位置)
        int best_gain = 0;
        int cum = 0;         // 自分より後方 (排他) の変換可能 FP 個数
        vector<int> conv_ps; // 変換可能 FP の P インデックス (降順)
        for (int i = n - 1; i >= 0; i--) {
            const char ch = base_ans[i];
            if (ch == 'M')
                break;
            if (ch != 'P')
                continue;
            const bool prev_is_f = (i - 1 >= 0 && base_ans[i - 1] == 'F');
            if (prev_is_f) {
                const int gain = cum - 2; // ここを登録点にした場合の短縮量
                if (gain > best_gain) {
                    best_gain = gain;
                    reg_i = i;
                }
                cum++;
                conv_ps.push_back(i);
            } else {
                // 無害判定: P 実行前の状態で先頭 F が無効
                // (同じ場所/向きに留まる)
                const auto st = pre_state(i);
                const bool f_noop = !in.can_move(st[0], st[1], st[2]);
                if (f_noop) {
                    // +0: 何もしない (走査継続)
                } else {
                    break; // 打ち切り
                }
            }
        }

        if (best_gain <= 0 || reg_i < 0)
            return base_ans;

        // reg_i は MFPM、reg_i より後方の変換可能 FP は FP->P。
        // F は P の直前 (p-1) にあるので、変換時は p-1 を削除する。
        vector<char> act(n, 0); // 0:通常 1:登録P 2:変換P 3:Fスキップ
        int conv_cnt = 0;
        for (int p : conv_ps) {
            if (p < reg_i)
                continue;
            assert(p - 1 >= 0 && base_ans[p - 1] == 'F');
            act[p] = (p == reg_i) ? 1 : 2;
            act[p - 1] = 3;
            conv_cnt++;
        }

        string cand;
        cand.reserve(n + 2);
        for (int i = 0; i < n; i++) {
            if (act[i] == 1) {
                cand += "MFPM"; // 直前の F (act==3) を取り込んで再構成
            } else if (act[i] == 2) {
                cand.push_back('P');
            } else if (act[i] == 3) {
                // F は削除済み (登録点なら MFPM 内に取り込み済み)
            } else {
                cand.push_back(base_ans[i]);
            }
        }

        // 構成的に最終ボール配置 (V) は不変のはず。来ないことを期待して
        // assert。
        assert(simulate_final_config(cand) == simulate_final_config(base_ans));

        const i64 base_sc = output_score_for_limit(base_ans);
        const i64 cand_sc = output_score_for_limit(cand);
        if (cand_sc < base_sc ||
            (cand_sc == base_sc && cand.size() < base_ans.size())) {
            if constexpr (DEBUG) {
                cerr << "[fp_extend] gain=" << best_gain << " applied=1"
                     << " reg_i=" << reg_i << " conv=" << conv_cnt
                     << " A=" << cand.size()
                     << " E=" << expanded_basic_count(cand, in.T) << el;
            }
            return cand;
        }
        if constexpr (DEBUG) {
            cerr << "[fp_extend] gain=" << best_gain << " applied=0"
                 << " base_sc=" << base_sc << " cand_sc=" << cand_sc
                 << " base_A=" << base_ans.size() << " cand_A=" << cand.size()
                 << el;
        }
        return base_ans;
    }

    // base_ans を最後まで実行したときの最終ボール配置 (ball_at, held) を返す。
    // V 一致判定 (前後Sの付け外しがボール配置を変えていないか) に用いる。
    pair<vector<int>, int> simulate_final_config(const string &ops) const {
        vector<int> ball_at(in.N * in.N, -1);
        for (int k = 0; k < in.M; k++)
            ball_at[in.ball_cells[k]] = k;
        int held = -1, pr = 0, pc = 0, pdir = 0;
        string last_macro, rec;
        bool recording = false;
        static constexpr int dr[4] = {0, 1, 0, -1};
        static constexpr int dc[4] = {1, 0, -1, 0};
        auto do_basic = [&](char ch) {
            if (ch == 'F') {
                if (in.can_move(pr, pc, pdir)) {
                    pr += dr[pdir];
                    pc += dc[pdir];
                }
            } else if (ch == 'R') {
                pdir = (pdir + 1) & 3;
            } else if (ch == 'L') {
                pdir = (pdir + 3) & 3;
            } else if (ch == 'S') {
                std::swap(held, ball_at[pr * in.N + pc]);
            }
        };
        for (char ch : ops) {
            if (ch == 'M') {
                if (recording) {
                    last_macro = rec;
                    rec.clear();
                    recording = false;
                } else {
                    rec.clear();
                    recording = true;
                }
            } else if (ch == 'P') {
                for (char mc : last_macro) {
                    do_basic(mc);
                    if (recording)
                        rec.push_back(mc);
                }
            } else {
                do_basic(ch);
                if (recording)
                    rec.push_back(ch);
            }
        }
        return {ball_at, held};
    }

    // 各 P が使うマクロの「先頭」「末尾」に S(swap) と回転(L/R) の両方を仕込む
    // DP。 1 回の再録音 M<頭ops>P<末尾ops>M で S と回転をまとめて変更できる (M
    // の固定費を共有)。
    //
    //  状態 = (頭の (S, 回転), 末尾の (S, 回転))。S∈{0,1}(自己逆元),
    //  回転∈{0,1,2,3}(mod4: R=+1,RR=+2,L=+3)。1 側 2*4=8, 両側 8*8=64 状態。
    //  - 頭の S は開始セル A_p で swap、頭の回転は本体全体を回す。
    //  - 末尾の S は終了セル B_p で swap、末尾の回転は本体後の向きを変える。
    //  - S と回転は可換(swap は向きに、回転は ball に干渉しない)。各 P の前後の
    //    元の {S,L,R} 連を「正味の (S パリティ, 回転)」とみなし、頭/末尾状態で
    //    吸収しきれない分だけ P の直前/直後に補正(S 0/1 個 + 回転 corrlen
    //    個)を置く。
    //    回転側は本体経路を回すが、補正で「元と同じ正味」に戻すので軌跡は不変。
    //  - 直接隣接する P 同士(間に F が無い)は B_{p}=A_{p+1}
    //  の同一セルで、末尾と
    //    次の先頭がまとめて偶奇/回転調整される。
    //  正しさは全シミュレートの assert(ボール配置一致)で守る。
    //
    //  use_s / use_lr で機能を制限し 3 変種を作れる:
    //   - (S, LR) = (true, false): 前後S のみ (旧 s_macro_both 相当)
    //   - (false, true)          : 前後LR のみ
    //   - (true, true)           : 全盛り
    //  機能を切った側はマクロに仕込まず、補正で元の (S パリティ/回転)
    //  を最小文字数で 保持する。E が T ぎりぎりのケースで E
    //  の増え方が異なる変種を選べる。
    string improve_with_macro_both(const string &base_ans, bool use_s,
                                   bool use_lr,
                                   int *applied_gain = nullptr) const {
        if (applied_gain)
            *applied_gain = 0;
#ifdef MB_HARNESS
        {
            static const char *cap = getenv("AHC_MB_CAPTURE");
            if (cap) {
                static std::ofstream ofs(cap, std::ios::app);
                ofs << base_ans << '\t' << (use_s ? 1 : 0) << '\t'
                    << (use_lr ? 1 : 0) << '\n';
            }
        }
#endif
        const char *vtag = use_s ? (use_lr ? "all" : "S") : "LR";
        const string macro = registered_macro_from_answer(base_ans);
        if (macro.empty()) {
            if constexpr (DEBUG)
                cerr << "[macro_both:" << vtag
                     << "] gain=0 applied=0 reason=no_macro" << el;
            return base_ans;
        }

        int reg_end = -1;
        int reg_open = -1; // 最後に登録されるマクロ構築の開き M の位置
        {
            bool recording = false;
            int cur_open = -1;
            for (int i = 0; i < (int)base_ans.size(); i++) {
                if (base_ans[i] == 'M') {
                    if (recording) {
                        recording = false;
                        reg_end = i;
                        reg_open = cur_open;
                    } else {
                        recording = true;
                        cur_open = i;
                    }
                }
            }
        }
        if (reg_end < 0) {
            if constexpr (DEBUG)
                cerr << "[macro_both:" << vtag
                     << "] gain=0 applied=0 reason=no_registration" << el;
            return base_ans;
        }

        const string head = base_ans.substr(0, reg_end + 1);
        const string tail = base_ans.substr(reg_end + 1);
        const int n = (int)tail.size();

        vector<int> p_idx;
        for (int i = 0; i < n; i++)
            if (tail[i] == 'P')
                p_idx.push_back(i);
        const int K = (int)p_idx.size();
        if (K == 0) {
            if constexpr (DEBUG)
                cerr << "[macro_both:" << vtag
                     << "] gain=0 applied=0 reason=no_tail_p" << el;
            return base_ans;
        }

        // base_ans を実行し、各 tail-P の A_p / B_p のボール存在 (S no-op
        // 判定用)。
        vector<char> ballA(K, 0), ballB(K, 0);
        // 登録 M..M
        // の開始セル(reg_open)・終了セル(reg_end)でのボール/把持有無。 ここに
        // ha0/ta0 の S を仕込むと登録実行時に 1 回 swap が起きるが、
        // ボールも把持も無ければ no-op なので recon 側 S を省ける。
        bool ball_regX = false, ball_regY = false;
        {
            vector<int> ball_at(in.N * in.N, -1);
            for (int k = 0; k < in.M; k++)
                ball_at[in.ball_cells[k]] = k;
            int held = -1, pr = 0, pc = 0, pdir = 0;
            string last_macro, rec;
            bool recording = false;
            static constexpr int dr[4] = {0, 1, 0, -1};
            static constexpr int dc[4] = {1, 0, -1, 0};
            auto do_basic = [&](char ch) {
                if (ch == 'F') {
                    if (in.can_move(pr, pc, pdir)) {
                        pr += dr[pdir];
                        pc += dc[pdir];
                    }
                } else if (ch == 'R') {
                    pdir = (pdir + 1) & 3;
                } else if (ch == 'L') {
                    pdir = (pdir + 3) & 3;
                } else if (ch == 'S') {
                    std::swap(held, ball_at[pr * in.N + pc]);
                }
            };
            int kp = 0;
            for (int i = 0; i < (int)base_ans.size(); i++) {
                const char ch = base_ans[i];
                if (ch == 'M') {
                    if (recording) {
                        if (i == reg_end)
                            ball_regY =
                                (held != -1 || ball_at[pr * in.N + pc] != -1);
                        last_macro = rec;
                        rec.clear();
                        recording = false;
                    } else {
                        if (i == reg_open)
                            ball_regX =
                                (held != -1 || ball_at[pr * in.N + pc] != -1);
                        rec.clear();
                        recording = true;
                    }
                    continue;
                }
                if (ch == 'P') {
                    const bool tail_p = (i > reg_end && kp < K);
                    if (tail_p)
                        ballA[kp] =
                            (held != -1 || ball_at[pr * in.N + pc] != -1) ? 1
                                                                          : 0;
                    for (char mc : last_macro) {
                        do_basic(mc);
                        if (recording)
                            rec.push_back(mc);
                    }
                    if (tail_p) {
                        ballB[kp] =
                            (held != -1 || ball_at[pr * in.N + pc] != -1) ? 1
                                                                          : 0;
                        kp++;
                    }
                    continue;
                }
                do_basic(ch);
                if (recording)
                    rec.push_back(ch);
            }
            if (kp != K) {
                if constexpr (DEBUG)
                    cerr << "[macro_both:" << vtag
                         << "] gain=0 applied=0 reason=simulate_mismatch K="
                         << K << " seen=" << kp << el;
                return base_ans;
            }
        }

        // 純粋ヘルパ(run_stats/is_slr/corrlen_*/turn_ops/hS..tR/indep_*/
        // common_*/side_add*/block_*) は namespace mbake へ抽出済み。
        using namespace mbake;
        // 機能制限: 切った側はマクロに仕込まない (S/回転 が 0 の状態のみ許可)
        auto valid = [&](int s) {
            if (!use_s && (hS(s) || tS(s)))
                return false;
            if (!use_lr && (hR(s) || tR(s)))
                return false;
            return true;
        };

        // gap (P_t と P_{t+1} の間) の分類とデータ
        struct Gap {
            bool nof;           // 間に F が無い (同一セル) か
            array<int, 3> run;  // nof のときの連 / sep のとき未使用
            array<int, 3> post; // sep: P_t 末尾側の連
            array<int, 3> pre;  // sep: P_{t+1} 先頭側の連
            string middle;      // sep: 連の間 (F 等) を逐語コピー
        };
        auto make_gap = [&](int t) {
            const int gl = p_idx[t] + 1, gr = p_idx[t + 1];
            const string g = tail.substr(gl, gr - gl);
            Gap gp;
            if (g.find('F') == string::npos) {
                gp.nof = true;
                gp.run = run_stats(g);
            } else {
                gp.nof = false;
                int a = 0;
                while (a < (int)g.size() && is_slr(g[a]))
                    a++;
                int b = (int)g.size();
                while (b > 0 && is_slr(g[b - 1]))
                    b--;
                gp.post = run_stats(g.substr(0, a));
                gp.pre = run_stats(g.substr(b));
                gp.middle = g.substr(a, b - a);
            }
            return gp;
        };
        vector<Gap> gaps(max(0, K - 1));
        for (int t = 0; t + 1 < K; t++)
            gaps[t] = make_gap(t);

        auto gap_gain = [&](int t, int a, int b) -> int {
            const Gap &g = gaps[t];
            if (g.nof)
                return common_gain(tS(a), tR(a), hS(b), hR(b), g.run, ballB[t]);
            return indep_gain(tS(a), tR(a), g.post, ballB[t]) +
                   indep_gain(hS(b), hR(b), g.pre, ballA[t + 1]);
        };

        const int NEG = numeric_limits<int>::min() / 4;
        const int ST = 64;
        struct MacroBothCandidate {
            string ops;
            int gain = NEG;
            int start = 0;
            int A = inf32;
            int E = inf32;
            i64 score = inf64;
        };

        // 高速化用: 片側 bake x∈[0,8) (=(S,R)) を y へ録り直すコスト表。
        // head/tail でデコードが同一(bit0=S, bit1-2=R)なので 1 表を共有する。
        array<array<int, 8>, 8> SAL{};
        for (int x = 0; x < 8; x++)
            for (int y = 0; y < 8; y++)
                SAL[x][y] = mbake::side_add_len(x & 1, (x >> 1) & 3, y & 1,
                                                (y >> 1) & 3);

        auto build_candidate = [&](int p_start) -> MacroBothCandidate {
            MacroBothCandidate res;
            res.start = p_start;
            if (p_start < 0 || p_start >= K)
                return res;

            // 前置き(P_start の前)の pre 連 と 末尾(P_{K-1} の後)の post 連。
            int pre0_s = p_idx[p_start];
            while (pre0_s > 0 && is_slr(tail[pre0_s - 1]))
                pre0_s--;
            const array<int, 3> pre0 =
                run_stats(tail.substr(pre0_s, p_idx[p_start] - pre0_s));
            const int last_idx = p_idx[K - 1];
            int sufe = last_idx + 1;
            while (sufe < n && is_slr(tail[sufe]))
                sufe++;
            const array<int, 3> suf =
                run_stats(tail.substr(last_idx + 1, sufe - (last_idx + 1)));

            const int L = K - p_start;
            // dp[layer][s]: layer 番目の P を bake 状態 s で使うときの最大
            // gain。 par は持たず、復元時に後ろ向き smallest-a
            // スキャンで再現する (前向きは factored で値のみ高速計算、tie-break
            // は元と一致)。
            vector<array<int, 64>> dp(L);
            for (int t = 0; t < L; t++)
                for (int s = 0; s < ST; s++)
                    dp[t][s] = NEG;
            // head の最後のマクロ構築 (既存の M...M) に初期状態 s0 を仕込む。
            // M は既存なので追加コストは「中身 ha0/ta0」と「帳尻合わせ
            // recon_head/recon_tail」のみ。常にフル相殺の recon を出すため
            // コストは 2*(中身文字数)。p_start>0 のときは p_start
            // より前の verbatim tail-P が影響を受けるため bake 不可。
            const bool allow_bake = (p_start == 0) && (reg_open >= 0);
            // tail 側の遅延評価: 登録終了セル Y と最初の P が同一セル
            // (間に F 無し ⇔ pre0_s==0) なら、recon_tail の回転を最初の補正へ
            // 畳み込める。これで登録 bake の tail 回転コストが 2x→1x になり、
            // 最初の補正回転も (pre0回転 - hR(st0) - tr0) に合成され短くなる。
            // S は held 整合のため recon_tail 側に残し、回転のみ畳む。
            const bool defer_tail = allow_bake && (pre0_s == 0);
            // head 側 recon 回転の畳み込み: 登録開き M 直前(セル X)の nav 末尾
            // SLR 連へ recon_head の回転 -hr0 を合成する。trail は同一セル上
            // (F 無し)なので S 群を前出し+純回転を (trailR - hr0)
            // に書換えできる。 S は held 整合のため別途(ball
            // ゲート)残し、回転のみ畳む。
            int nav_trail_s = (reg_open >= 0) ? reg_open : 0;
            while (nav_trail_s > 0 && is_slr(base_ans[nav_trail_s - 1]))
                nav_trail_s--;
            int trailR = 0, trail_s_verb = 0, trail_lr_verb = 0;
            for (int i = nav_trail_s; i < reg_open; i++) {
                const char c = base_ans[i];
                if (c == 'R') {
                    trailR = (trailR + 1) & 3;
                    trail_lr_verb++;
                } else if (c == 'L') {
                    trailR = (trailR + 3) & 3;
                    trail_lr_verb++;
                } else if (c == 'S') {
                    trail_s_verb++;
                }
            }
            auto head_bake_cost = [&](int s0, bool defer) -> int {
                // head 回転: recon を nav 末尾回転へ畳み込む。s0=0
                // 基準(verbatim trail=trail_lr_verb)に対する増分 = 書換後 nav
                // 回転 corrlen_turn(trailR-hr0) + ha0 回転 corrlen_turn(hr0)
                // - 元の trail 回転 trail_lr_verb。trail 空なら 2x に退化。
                // tail 回転は defer 時のみ 1x(ta0 のみ、recon は畳み込み)。
                // S は登録セルにボール/把持が無ければ ha0/ta0 の S が登録実行で
                // no-op になり recon 側 S が不要 → 追加コスト 1。ボール有りで
                // 2。
                const int head_s = hS(s0) * (ball_regX ? 2 : 1);
                const int tail_s = tS(s0) * (ball_regY ? 2 : 1);
                const int head_rot = corrlen_turn((trailR - hR(s0)) & 3) +
                                     corrlen_turn(hR(s0)) - trail_lr_verb;
                const int tail_rot = (defer ? 1 : 2) * corrlen_turn(tR(s0));
                return -(head_s + head_rot + tail_s + tail_rot);
            };
            // 最初の P (pre0 連) の gain。defer 時は s0 の tr0
            // を回転補正へ畳む。
            auto entry_pre0_gain = [&](int s, int s0) -> int {
                if (defer_tail) {
                    const int sS = corrlen_s(pre0[0] ^ hS(s), ballA[p_start]);
                    const int sR = corrlen_turn((pre0[1] - hR(s) - tR(s0)) & 3);
                    return pre0[2] - (sS + sR);
                }
                return indep_gain(hS(s), hR(s), pre0, ballA[p_start]);
            };
            // s0(head に焼く初期状態)と s(=st[0]) を同時に最適化して dp[0]
            // を作る。 defer 時は s0 の tr0 が pre0
            // 補正に効くため両者を分離できない。
            array<int, 64> best_entry_arg;
            for (int s = 0; s < ST; s++)
                best_entry_arg[s] = 0;
            const int s0_hi = allow_bake ? ST : 1; // bake 不可なら s0=0 のみ
            for (int s0 = 0; s0 < s0_hi; s0++) {
                if (allow_bake && !valid(s0))
                    continue;
                const int hb = (s0 == 0) ? 0 : head_bake_cost(s0, defer_tail);
                for (int s = 0; s < ST; s++) {
                    if (!valid(s))
                        continue;
                    const int v =
                        hb + block_cost(s0, s) + entry_pre0_gain(s, s0);
                    if (v > dp[0][s]) {
                        dp[0][s] = v;
                        best_entry_arg[s] = s0;
                    }
                }
            }
            // 遷移 dp[dt+1][b] = max_a dp[dt][a] + block_cost(a,b)
            //                        + gap_gain(t,a,b)
            // を head(8)×tail(8) 分離で O(64·8) に。状態 s = ha + 8*ta
            // (ha=s&7=(hS,hR), ta=(s>>3)&7=(tS,tR))。
            // block_cost の a≠b 部分は -(SAL[ha][hb]+SAL[ta][tb]+2) と分離、
            // gap_gain は sep で GT(ta)+GH(hb)、nof で CG(ta,hb)。
            // a==b は別枝 (block_cost=0)。neq 側に a==b を -2 付きで含めても
            // val_eq(+0) が必ず上回るので無害。
            const int NEG_HALF = NEG / 2;
            for (int t = p_start; t + 1 < K; t++) {
                const int dt = t - p_start;
                const auto &cur = dp[dt];
                auto &nxt = dp[dt + 1];
                const Gap &g = gaps[t];
                array<int, 8> GT{}, GH{};
                array<array<int, 8>, 8> CG{};
                if (g.nof) {
                    for (int ta = 0; ta < 8; ta++)
                        for (int hb = 0; hb < 8; hb++)
                            CG[ta][hb] =
                                common_gain(ta & 1, (ta >> 1) & 3, hb & 1,
                                            (hb >> 1) & 3, g.run, ballB[t]);
                } else {
                    for (int ta = 0; ta < 8; ta++)
                        GT[ta] =
                            indep_gain(ta & 1, (ta >> 1) & 3, g.post, ballB[t]);
                    for (int hb = 0; hb < 8; hb++)
                        GH[hb] = indep_gain(hb & 1, (hb >> 1) & 3, g.pre,
                                            ballA[t + 1]);
                }
                // M2[hb][ta] = max_ha ( cur[ha+8*ta] - SAL[ha][hb] )
                array<array<int, 8>, 8> M2{};
                for (int hb = 0; hb < 8; hb++)
                    for (int ta = 0; ta < 8; ta++) {
                        int best = NEG;
                        for (int ha = 0; ha < 8; ha++) {
                            const int v = cur[ha + 8 * ta] - SAL[ha][hb];
                            if (v > best)
                                best = v;
                        }
                        M2[hb][ta] = best;
                    }
                for (int hb = 0; hb < 8; hb++) {
                    for (int tb = 0; tb < 8; tb++) {
                        const int b = hb + 8 * tb;
                        if (!valid(b)) {
                            nxt[b] = NEG;
                            continue;
                        }
                        int inner = NEG;
                        if (g.nof) {
                            for (int ta = 0; ta < 8; ta++) {
                                const int v =
                                    M2[hb][ta] + CG[ta][hb] - SAL[ta][tb];
                                if (v > inner)
                                    inner = v;
                            }
                        } else {
                            for (int ta = 0; ta < 8; ta++) {
                                const int v = M2[hb][ta] + GT[ta] - SAL[ta][tb];
                                if (v > inner)
                                    inner = v;
                            }
                        }
                        int neq = (g.nof ? inner : GH[hb] + inner) - 2;
                        int val_eq = NEG;
                        if (cur[b] > NEG)
                            val_eq =
                                cur[b] + (g.nof ? CG[tb][hb] : GT[tb] + GH[hb]);
                        const int best = max(neq, val_eq);
                        nxt[b] = (best < NEG_HALF) ? NEG : best;
                    }
                }
            }
            int best_gain = NEG, best_s = 0;
            for (int s = 0; s < ST; s++) {
                if (dp[L - 1][s] <= NEG)
                    continue;
                const int g =
                    dp[L - 1][s] + indep_gain(tS(s), tR(s), suf, ballB[K - 1]);
                if (g > best_gain) {
                    best_gain = g;
                    best_s = s;
                }
            }
            res.gain = best_gain;
            if (best_gain <= 0)
                return res;

            vector<int> st(L);
            st[L - 1] = best_s;
            // 後ろ向き復元: layer t の状態 b を達成した最小の a を探す。
            // 前向きが strict-max(smallest-a) なので、値一致の最小 a が par
            // に一致。
            for (int layer = L - 1; layer > 0; layer--) {
                const int b = st[layer];
                const int gt = p_start + layer - 1;
                int found = -1;
                for (int a = 0; a < ST; a++) {
                    if (dp[layer - 1][a] <= NEG)
                        continue;
                    if (dp[layer - 1][a] + block_cost(a, b) +
                            gap_gain(gt, a, b) ==
                        dp[layer][b]) {
                        found = a;
                        break;
                    }
                }
                st[layer - 1] = found;
            }

            // head に仕込む初期状態 s0 と、それに伴う head/tail 改変文字列。
            const int s0 = best_entry_arg[st[0]];
            const int hs0 = hS(s0), hr0 = hR(s0), ts0 = tS(s0), tr0 = tR(s0);
            const string ha0 = string(hs0, 'S') + turn_ops(hr0);
            const string ta0 = string(ts0, 'S') + turn_ops(tr0);
            // body 録音軌跡を保つため開き M 直前で ha0 を相殺、
            // tail 先頭で ta0 を相殺する (各 S は同一セルで偶数回 → V 不変)。
            // 登録セルにボール/把持が無ければ ha0/ta0 の S は登録実行で no-op
            // なので recon 側 S を省く (head_bake_cost と整合)。
            // recon_head の回転は nav 末尾 SLR 連へ畳み込む(下の nav_mod)。
            // S(held 整合)のみ recon として残す。tail も S は ball ゲート。
            const string recon_tail =
                turn_ops((4 - tr0) & 3) + string(ball_regY ? ts0 : 0, 'S');
            string head_mod;
            if (s0 == 0) {
                head_mod = head;
            } else {
                // nav 末尾 SLR 連を「S 群(verbatim count) + 畳込回転
                // (trailR - hr0)」へ書換え、recon_head 回転を吸収する。
                const string nav_mod = base_ans.substr(0, nav_trail_s) +
                                       string(trail_s_verb, 'S') +
                                       turn_ops((trailR - hr0) & 3);
                const string recon_head_s = string(ball_regX ? hs0 : 0, 'S');
                head_mod =
                    nav_mod + recon_head_s + "M" + ha0 +
                    base_ans.substr(reg_open + 1, reg_end - reg_open - 1) +
                    ta0 + "M";
            }

            // 再構築。p_start より前の tail はそのまま残し、suffix だけマクロ端
            // DP で更新する。
            string nt;
            nt.reserve(n + 16);
            if (defer_tail) {
                // pre0_s==0 なので tail.substr(0,pre0_s) は空。recon_tail の
                // 回転は最初の補正へ畳み込み、S(held 整合)のみ残す。
                nt += string(ball_regY ? ts0 : 0, 'S');
                const int eS = ballA[p_start] ? (pre0[0] ^ hS(st[0])) : 0;
                const int eR = (pre0[1] - hR(st[0]) - tr0) & 3;
                nt += string(eS, 'S') + turn_ops(eR);
            } else {
                nt += recon_tail;
                nt += tail.substr(0, pre0_s);
                nt += indep_corr(hS(st[0]), hR(st[0]), pre0[0], pre0[1],
                                 ballA[p_start]);
            }
            nt += block_str(s0, st[0]);
            for (int t = p_start; t + 1 < K; t++) {
                const Gap &g = gaps[t];
                const int a = st[t - p_start], b = st[t + 1 - p_start];
                if (g.nof) {
                    nt += common_corr(tS(a), tR(a), hS(b), hR(b), g.run[0],
                                      g.run[1], ballB[t]);
                } else {
                    nt += indep_corr(tS(a), tR(a), g.post[0], g.post[1],
                                     ballB[t]);
                    nt += g.middle;
                    nt += indep_corr(hS(b), hR(b), g.pre[0], g.pre[1],
                                     ballA[t + 1]);
                }
                nt += block_str(a, b);
            }
            nt += indep_corr(tS(st[L - 1]), tR(st[L - 1]), suf[0], suf[1],
                             ballB[K - 1]);
            nt += tail.substr(sufe);

            res.ops = head_mod + nt;
            res.A = (int)res.ops.size();
            res.E = expanded_basic_count(res.ops, in.T);
            res.score = output_score_for_limit(res.ops);
            return res;
        };

        MacroBothCandidate cand = build_candidate(0);
        if (cand.gain <= 0) {
            if constexpr (DEBUG)
                cerr << "[macro_both:" << vtag << "] gain=0 applied=0 K=" << K
                     << el;
            return base_ans;
        }

        // 軌跡保存により構成的に V 不変なはず。来ないことを期待して assert
        // で守る。
        // assert(simulate_final_config(cand.ops) ==
        // simulate_final_config(base_ans));

        const i64 base_sc = output_score_for_limit(base_ans);
        auto better_than_base = [&](const MacroBothCandidate &x) {
            return x.score < base_sc ||
                   (x.score == base_sc && x.ops.size() < base_ans.size());
        };
        if (!better_than_base(cand) && cand.score >= inf64 / 8 &&
            cand.gain > 0 && cand.A <= in.T && cand.E > in.T) {
            if constexpr (DEBUG)
                cerr << "[macro_both:" << vtag << "] bs_try K=" << K
                     << " full_gain=" << cand.gain << " full_A=" << cand.A
                     << " full_E=" << cand.E << " T=" << in.T << el;
            int lo = 0, hi = K;
            optional<MacroBothCandidate> found;
            while (hi - lo > 1) {
                const int mid = (lo + hi) / 2;
                MacroBothCandidate m = build_candidate(mid);
                if (m.A > in.T || m.E > in.T) {
                    lo = mid; // 早すぎて展開長が重いので、右へ寄せる。
                } else if (m.gain <= 0) {
                    hi = mid; // 短すぎて利得がないので、左へ寄せる。
                } else {
                    found = std::move(m);
                    hi = mid; // 制約内で利得あり。より早い開始位置も試す。
                }
            }
            if (found.has_value()) {
                MacroBothCandidate bs = std::move(*found);
                if constexpr (DEBUG)
                    cerr << "[macro_both:" << vtag
                         << "] bs_found start=" << bs.start
                         << " gain=" << bs.gain << " A=" << bs.A
                         << " E=" << bs.E << " better=" << better_than_base(bs)
                         << el;
                if (better_than_base(bs))
                    cand = std::move(bs);
            } else if constexpr (DEBUG) {
                cerr << "[macro_both:" << vtag << "] bs_not_found K=" << K
                     << el;
            }
        }
        if (better_than_base(cand)) {
#ifdef MB_VCHECK
            assert(simulate_final_config(cand.ops) ==
                   simulate_final_config(base_ans));
#endif
            if constexpr (DEBUG)
                cerr << "[macro_both:" << vtag << "] gain=" << cand.gain
                     << " applied=1 K=" << K << " start=" << cand.start
                     << " A=" << cand.A << " E=" << cand.E << el;
            if (applied_gain)
                *applied_gain = cand.gain;
            return cand.ops;
        }
        if constexpr (DEBUG)
            cerr << "[macro_both:" << vtag << "] gain=" << cand.gain
                 << " applied=0 base_A=" << base_ans.size()
                 << " cand_A=" << cand.A << " cand_E=" << cand.E << el;
        return base_ans;
    }

    int max_consecutive_f_in_macro(const string &macro) const {
        int best_run = 0, cur = 0;
        for (char ch : macro) {
            if (ch == 'F') {
                cur++;
                smax(best_run, cur);
            } else {
                cur = 0;
            }
        }
        return best_run;
    }

    string restore_answer_for_current_best(string *raw_path_ops = nullptr) {
        g_ctx.build(best.macro, true);
        string ops =
            build_path_from_start_state(in.state_id(0, 0, 0), best.order);
        if (raw_path_ops)
            *raw_path_ops = ops;

        string best_ans;
        if (best.macro.empty()) {
            if ((int)ops.size() > in.T)
                ops.resize(in.T);
            best_ans = ops;
        } else {
            const int first_p = (int)ops.find('P');
            if (first_p == (int)string::npos) {
                best_ans = ops;
            } else {
                best_ans = use_substring_macro_before_main_registration(
                    ops.substr(0, first_p), best.macro,
                    ops.substr(first_p + 1));
            }
            if (output_score_for_limit(best_ans) >= inf64 / 8)
                best_ans = build_plain_answer();
        }
        return best_ans;
    }

    i64 compute_provisional_macro_score(RestoredMacroCandidate &cand) {
        const State saved = best;
        best = cand.state;
        if (cand.restored_answer.empty())
            cand.restored_answer =
                restore_answer_for_current_best(&cand.raw_path_ops);
        g_ctx.build(best.macro, true);

        i64 best_init_score = output_score_for_limit(cand.restored_answer);
        string best_init_answer = cand.restored_answer;
        // 登録マクロ本体は FFF...（F が len 個）のみ。録音(M...M)の外側に
        // L / R を置き、上の壁にぶつけて右に戻ることで初期位置 (0,0,dir0) を
        // 維持する冗長操作とする。"L"+FFF+"R" は開始状態に戻る（上端で F は
        // no-op）ため、after は常に開始 sid と一致し、生パスは候補によらず
        // cand.raw_path_ops を再利用できる
        const string &path_ops = cand.raw_path_ops;
        const int first_p = (int)path_ops.find('P');
        // 各 len の答えは構築せず、build_initial_macro_answer_with_path が
        // 生成する文字列長を encode 長から直接見積もる:
        //   init_ops(= L M FFF M R) = 4 + len
        //   + encode(先頭〜初P, FFF)
        //   + M encode(best.macro, FFF) M = 2 + ...
        //   + 後半(初Pを消費した残り)
        // = R + macro + len + 5 - (短縮できる長さ)。
        auto estimate_init_macro_a = [&](int len) -> i64 {
            const string init_macro(len, 'F');
            if (first_p == (int)string::npos) {
                return (i64)(4 + len) +
                       (i64)encode_with_registered_macro(path_ops, init_macro)
                           .size();
            }
            const int enc_prefix = (int)encode_with_registered_macro(
                                       path_ops.substr(0, first_p), init_macro)
                                       .size();
            const int enc_macro =
                (int)encode_with_registered_macro(best.macro, init_macro)
                    .size();
            const int suffix = (int)path_ops.size() - first_p - 1;
            return (i64)(4 + len) + enc_prefix + 2 + enc_macro + suffix;
        };
        // 暗黙登録パターン: 初めての P までの道中(part1)に best.macro の
        // 部分文字列が自然に現れるなら、その場を M <sub> M で囲むだけで登録
        // できる(壁ぶつけの L M FFF M R より安い)。回転を含む任意の部分文字列で
        // 有効で、improve_without_explicit_initial_macro と同じ仕組み。
        // best.macro のユニークな部分文字列を全探索し、長さを直接見積もって
        // 上記の壁ぶつけ推定との min を取る。
        //   = part1[0:pos] + M sub M + encode(part1[pos+len:],sub)
        //     + M encode(best.macro,sub) M + part2
        const string part1_ops = (first_p == (int)string::npos)
                                     ? string()
                                     : path_ops.substr(0, first_p);
        const string part2_ops = (first_p == (int)string::npos)
                                     ? string()
                                     : path_ops.substr(first_p + 1);
        auto estimate_implicit_a = [&](const string &init_macro,
                                       int pos) -> i64 {
            const int len = (int)init_macro.size();
            const int enc_tail = (int)encode_with_registered_macro(
                                     part1_ops.substr(pos + len), init_macro)
                                     .size();
            const int enc_macro =
                (int)encode_with_registered_macro(best.macro, init_macro)
                    .size();
            return (i64)pos + len + enc_tail + enc_macro +
                   (int)part2_ops.size() + 4;
        };

        const int implicit_max_len = min(max(1, Tuning::path_implicit_max_len),
                                         (int)best.macro.size() / 2);
        const int x =
            min(max_consecutive_f_in_macro(best.macro), implicit_max_len);
        int best_len = -1;             // 壁ぶつけ登録の len（-1 は不使用）
        bool best_is_implicit = false; // false: 壁ぶつけ登録, true: 暗黙登録
        string best_impl_macro;
        int best_impl_pos = -1;
        // 壁ぶつけ登録（FFF のみ。L/R が初期位置維持の冗長操作になるため）。
        for (int len = 2; len <= x; len++) {
            const i64 sc_exp = estimate_init_macro_a(len);
            if (sc_exp < best_init_score) {
                best_init_score = sc_exp;
                best_len = len;
                best_is_implicit = false;
            }
        }
        // 暗黙登録（best.macro のユニーク部分文字列を全探索）。
        if (first_p != (int)string::npos) {
            for (const string &init_macro :
                 enumerate_main_macro_substrings(best.macro)) {
                if ((int)init_macro.size() < 2)
                    continue; // 長さ1 は P と同コストで利得なし。
                if ((int)init_macro.size() > implicit_max_len)
                    continue;
                const int pos = (int)part1_ops.find(init_macro);
                if (pos == (int)string::npos)
                    continue;
                const i64 sc_imp = estimate_implicit_a(init_macro, pos);
                if (sc_imp < best_init_score) {
                    best_init_score = sc_imp;
                    best_is_implicit = true;
                    best_impl_macro = init_macro;
                    best_impl_pos = pos;
                }
            }
        }
        if (best_is_implicit || best_len != -1) {
            // 勝者のみ実構築して both_gain 計算に渡す。
            if (best_is_implicit) {
                const string &init_macro = best_impl_macro;
                const int len = (int)init_macro.size();
                string cand;
                cand.reserve(path_ops.size() + best.macro.size() + len + 8);
                cand += part1_ops.substr(0, best_impl_pos);
                cand.push_back('M');
                cand += init_macro;
                cand.push_back('M');
                cand += encode_with_registered_macro(
                    part1_ops.substr(best_impl_pos + len), init_macro);
                cand.push_back('M');
                cand += encode_with_registered_macro(best.macro, init_macro);
                cand.push_back('M');
                cand += part2_ops;
                best_init_answer = std::move(cand);
            } else {
                const string init_macro(best_len, 'F');
                assert(simulate_basic_ops_from(in.state_id(0, 0, 0),
                                               "L" + string(best_len, 'F') +
                                                   "R") ==
                       in.state_id(0, 0, 0));
                string init_ops;
                init_ops.reserve(best_len + 4);
                init_ops.push_back('L');
                init_ops.push_back('M');
                init_ops += init_macro;
                init_ops.push_back('M');
                init_ops.push_back('R');
                best_init_answer = build_initial_macro_answer_with_path(
                    init_ops, init_macro, path_ops);
            }
            assert((i64)best_init_answer.size() == best_init_score);
        }

        int both_gain = 0;
        (void)improve_with_macro_both(best_init_answer, true, true, &both_gain);
        cand.init_best_score = best_init_score;
        cand.both_all_gain = both_gain;
        cand.provisional_score = best_init_score - both_gain;
        if (timer.elapsed() <= Tuning::macro_preselect_deadline) {
            auto cands = build_path_segment_candidates(
                cand.raw_path_ops, Tuning::path_slack, Tuning::path_cap, false);
            vector<int> greedy_sel = build_greedy_path_selection(cands);
            const i64 greedy_score =
                provisional_score_from_segments(cands, greedy_sel);
            if (greedy_score < cand.provisional_score) {
                cand.raw_path_ops = build_raw_from_segments(cands, greedy_sel);
                cand.restored_answer.clear();
                cand.init_best_score = greedy_score;
                cand.both_all_gain = 0;
                cand.provisional_score = greedy_score;
            }
        }
        best = saved;
        return cand.provisional_score;
    }

    // ============================================================
    // 実験: 区間ごとのパス列挙 + パス選択焼きなまし
    // ============================================================
    // raw_path_ops（F/L/R/P/S 列、P=best.macro 再生）を渡すと、
    //   - 明示 init_macro（壁ぶつけ LMFF..MR）
    //   - 暗黙 init_macro（P 到達までの道中に現れる部分文字列）
    //   - init_macro なし
    // の最良 best_init_score を求め、init_macro なし版に both_all を適用した
    // gain を引いた暫定スコア上界を返す。compute_provisional_macro_score の
    // raw を外から渡せる簡易版（評価器として使い回す）。
    i64 provisional_score_from_raw(const string &raw_in) {
        const string &path_ops = raw_in;

        // init_macro なし版（基準復元解）
        string restored_answer;
        if (best.macro.empty()) {
            string ops = path_ops;
            if ((int)ops.size() > in.T)
                ops.resize(in.T);
            restored_answer = ops;
        } else {
            const int first_p = (int)path_ops.find('P');
            if (first_p == (int)string::npos) {
                restored_answer = path_ops;
            } else {
                restored_answer = use_substring_macro_before_main_registration(
                    path_ops.substr(0, first_p), best.macro,
                    path_ops.substr(first_p + 1));
            }
            if (output_score_for_limit(restored_answer) >= inf64 / 8)
                restored_answer = build_plain_answer();
        }

        i64 best_init_score = output_score_for_limit(restored_answer);

        if (!best.macro.empty()) {
            // best.macro 固定の間だけ有効なキャッシュ（部分文字列列挙・
            // enc(macro,init_macro)長・最長F連長）を用意して使い回す。
            ensure_prov_cache(Tuning::path_implicit_max_len);
            const string &p = path_ops;
            const int first_p = (int)p.find('P');
            // enc_macro は raw 非依存なのでキャッシュ参照（init_macro -> 長さ）
            auto enc_macro_of = [&](const string &im) -> int {
                auto it = prov_cache.enc_macro_size.find(im);
                return it != prov_cache.enc_macro_size.end()
                           ? it->second
                           : (int)encode_with_registered_macro(best.macro, im)
                                 .size();
            };
            auto estimate_init_macro_a = [&](int len) -> i64 {
                const string init_macro(len, 'F');
                if (first_p == (int)string::npos) {
                    return (i64)(4 + len) +
                           (i64)encode_with_registered_macro(p, init_macro)
                               .size();
                }
                const int enc_prefix = (int)encode_with_registered_macro(
                                           p.substr(0, first_p), init_macro)
                                           .size();
                const int suffix = (int)p.size() - first_p - 1;
                return (i64)(4 + len) + enc_prefix + 2 +
                       enc_macro_of(init_macro) + suffix;
            };
            const string part1_ops = (first_p == (int)string::npos)
                                         ? string()
                                         : p.substr(0, first_p);
            const string part2_ops = (first_p == (int)string::npos)
                                         ? string()
                                         : p.substr(first_p + 1);
            auto estimate_implicit_a = [&](const string &init_macro,
                                           int pos) -> i64 {
                const int len = (int)init_macro.size();
                const int enc_tail =
                    (int)encode_with_registered_macro(
                        part1_ops.substr(pos + len), init_macro)
                        .size();
                return (i64)pos + len + enc_tail + enc_macro_of(init_macro) +
                       (int)part2_ops.size() + 4;
            };
            for (int len = 2; len <= prov_cache.max_frun; len++)
                smin(best_init_score, estimate_init_macro_a(len));
            if (first_p != (int)string::npos) {
                for (const string &im : prov_cache.impl_cands) {
                    const int pos = (int)part1_ops.find(im);
                    if (pos == (int)string::npos)
                        continue;
                    smin(best_init_score, estimate_implicit_a(im, pos));
                }
            }
        }

        // both_all は init_macro なし版に適用（gain は上界として安全）
        int both_gain = 0;
        (void)improve_with_macro_both(restored_answer, true, true, &both_gain);
        return best_init_score - both_gain;
    }

    int selected_path_size(const vector<vector<string>> &cands,
                           const vector<int> &sel) const {
        int n = (int)cands.size();
        int sz = n;
        for (int s = 0; s < n; s++)
            sz += (int)cands[s][sel[s]].size();
        return sz;
    }

    string build_raw_from_segments(const vector<vector<string>> &cands,
                                   const vector<int> &sel) const {
        const int n = (int)cands.size();
        string r;
        r.reserve(selected_path_size(cands, sel));
        for (int s = 0; s < n; s++) {
            r += cands[s][sel[s]];
            r.push_back('S');
        }
        return r;
    }

    void split_segments_at_first_p(const vector<vector<string>> &cands,
                                   const vector<int> &sel, string &part1,
                                   string &part2, bool &found_p) const {
        const int n = (int)cands.size();
        const int reserve_sz = selected_path_size(cands, sel);
        part1.clear();
        part2.clear();
        part1.reserve(reserve_sz);
        part2.reserve(reserve_sz);
        found_p = false;
        for (int s = 0; s < n; s++) {
            const string &seg = cands[s][sel[s]];
            if (!found_p) {
                const size_t pos = seg.find('P');
                if (pos == string::npos) {
                    part1 += seg;
                    part1.push_back('S');
                } else {
                    part1.append(seg, 0, pos);
                    part2.append(seg, pos + 1, string::npos);
                    part2.push_back('S');
                    found_p = true;
                }
            } else {
                part2 += seg;
                part2.push_back('S');
            }
        }
    }

    i64 provisional_score_from_segments(const vector<vector<string>> &cands,
                                        const vector<int> &sel) {
        string part1_ops, part2_ops;
        bool found_p = false;
        split_segments_at_first_p(cands, sel, part1_ops, part2_ops, found_p);

        string restored_answer;
        if (best.macro.empty()) {
            restored_answer = build_raw_from_segments(cands, sel);
            if ((int)restored_answer.size() > in.T)
                restored_answer.resize(in.T);
        } else if (!found_p) {
            restored_answer = part1_ops;
        } else {
            restored_answer = use_substring_macro_before_main_registration(
                part1_ops, best.macro, part2_ops);
        }
        if (!best.macro.empty() &&
            output_score_for_limit(restored_answer) >= inf64 / 8)
            restored_answer = build_plain_answer();

        i64 best_init_score = output_score_for_limit(restored_answer);

        if (!best.macro.empty()) {
            ensure_prov_cache(Tuning::path_implicit_max_len);
            auto enc_macro_of = [&](const string &im) -> int {
                auto it = prov_cache.enc_macro_size.find(im);
                return it != prov_cache.enc_macro_size.end()
                           ? it->second
                           : (int)encode_with_registered_macro(best.macro, im)
                                 .size();
            };
            auto estimate_init_macro_a = [&](int len) -> i64 {
                const string init_macro(len, 'F');
                if (!found_p) {
                    return (i64)(4 + len) + (i64)encode_with_registered_macro(
                                                part1_ops, init_macro)
                                                .size();
                }
                const int enc_prefix =
                    (int)encode_with_registered_macro(part1_ops, init_macro)
                        .size();
                return (i64)(4 + len) + enc_prefix + 2 +
                       enc_macro_of(init_macro) + (int)part2_ops.size();
            };
            auto estimate_implicit_a = [&](const string &init_macro,
                                           int pos) -> i64 {
                const int len = (int)init_macro.size();
                const int enc_tail =
                    (int)encode_with_registered_macro(
                        part1_ops.substr(pos + len), init_macro)
                        .size();
                return (i64)pos + len + enc_tail + enc_macro_of(init_macro) +
                       (int)part2_ops.size() + 4;
            };
            for (int len = 2; len <= prov_cache.max_frun; len++)
                smin(best_init_score, estimate_init_macro_a(len));
            if (found_p) {
                for (const string &im : prov_cache.impl_cands) {
                    const int pos = (int)part1_ops.find(im);
                    if (pos == (int)string::npos)
                        continue;
                    smin(best_init_score, estimate_implicit_a(im, pos));
                }
            }
        }

        int both_gain = 0;
        (void)improve_with_macro_both(restored_answer, true, true, &both_gain);
        return best_init_score - both_gain;
    }

    // raw_path_ops から最終出力文字列を構築（build_answer と同等の後処理）。
    string finalize_answer_from_raw(const string &raw_in) {
        g_ctx.build(best.macro, true);
        string ops = normalize_rotation_swap(raw_in);
        string best_ans;
        if (best.macro.empty()) {
            if ((int)ops.size() > in.T)
                ops.resize(in.T);
            best_ans = ops;
        } else {
            const int first_p = (int)ops.find('P');
            if (first_p == (int)string::npos) {
                best_ans = ops;
            } else {
                best_ans = use_substring_macro_before_main_registration(
                    ops.substr(0, first_p), best.macro,
                    ops.substr(first_p + 1));
            }
            if (output_score_for_limit(best_ans) >= inf64 / 8)
                best_ans = build_plain_answer();
        }
        string ans =
            improve_with_initial_macro_candidates(best_ans, best.order, ops);
        {
            auto pick_better = [&](const string &a, const string &b) {
                const i64 sa = output_score_for_limit(a);
                const i64 sb = output_score_for_limit(b);
                if (sa != sb)
                    return sa < sb ? a : b;
                return a.size() <= b.size() ? a : b;
            };
            string tail_or_s = improve_with_tail_macro(ans);

            const string md = improve_with_macro_both(ans, 1, 1);
            const string serial = improve_with_tail_macro(md);
            tail_or_s = pick_better(tail_or_s, serial);
            // tail と並列に PF / FP 延長畳み込みを試して良い方を採る。
            tail_or_s = pick_better(tail_or_s, improve_with_pf_extend(ans));
            tail_or_s = pick_better(tail_or_s, improve_with_fp_extend(ans));
            ans = tail_or_s;
        }
        return ans;
    }

    // 区間 sid_a -> sid_b の、最短長 d または d+1 のパスを列挙（cap 個まで）。
    // 先頭 candidate には default_path（既存復元の最短パス）を必ず入れる。
    vector<string> enumerate_segment_paths(int sid_a, int sid_b,
                                           const string &default_path, int cap,
                                           int max_slack,
                                           const vector<vector<int>> &rev) {
        const int S = in.N * in.N * 4;
        const bool has_macro = !best.macro.empty();
        const int n_ops = has_macro ? 4 : 3;
        static constexpr char OPS[4] = {'F', 'L', 'R', 'P'};

        // 逆 BFS で db[u] = u から b への最短手数。
        // スタンプ方式: stamp[u]==gen のときだけ db[u] が有効（=訪問済み）。
        // gen を毎回 ++ するので inf32 埋めもリセットも不要。
        if ((int)seg_db_buf.size() < S) {
            seg_db_buf.resize(S);
            seg_db_stamp.assign(S, 0);
        }
        const int gen = ++seg_db_gen;
        int *__restrict db = seg_db_buf.data();
        int *__restrict stamp = seg_db_stamp.data();
        auto visited = [&](int u) { return stamp[u] == gen; };
        vector<int> &q = seg_bfs_q;
        q.clear();
        {
            db[sid_b] = 0;
            stamp[sid_b] = gen;
            q.push_back(sid_b);
            int head = 0;
            while (head < (int)q.size()) {
                const int u = q[head++];
                const int nd = db[u] + 1;
                for (int w : rev[u]) {
                    // BFS は距離非減少順に訪問するので未訪問判定で最短が確定。
                    if (!visited(w)) {
                        db[w] = nd;
                        stamp[w] = gen;
                        q.push_back(w);
                    }
                }
            }
        }

        vector<string> out;
        unordered_set<string> seen;
        out.push_back(default_path);
        seen.insert(default_path);

        if (visited(sid_a)) {
            string cur;
            // slack: これまでに費やした「db を縮めなかった」ステップ数の累計。
            // 最短 d に対して長さ d + slack のパスを許す（max_slack=1
            // が既定）。std::function を避け self-recursion
            // で呼び出しを軽くする。
            auto dfs = [&](auto &self, int u, int slack) -> void {
                if ((int)out.size() >= cap)
                    return;
                if (u == sid_b) {
                    if (seen.insert(cur).second)
                        out.push_back(cur);
                    return;
                }
                for (int k = 0; k < n_ops; k++) {
                    const int v = g_ctx.next4[u][k];
                    if (!visited(v))
                        continue;
                    const int prog = db[u] - db[v]; // -1, 0, 1
                    const int ns = slack + (1 - prog);
                    if (ns > max_slack)
                        continue;
                    cur.push_back(OPS[k]);
                    self(self, v, ns);
                    cur.pop_back();
                    if ((int)out.size() >= cap)
                        return;
                }
            };
            dfs(dfs, sid_a, 0);
        }
        return out;
    }

    vector<vector<string>>
    build_path_segment_candidates(const string &default_raw, int max_slack,
                                  int cap, [[maybe_unused]] bool verbose) {
        const auto &ctx = g_ctx;
        const int M = (int)best.order.size();
        const int nseg = 2 * M;

        vector<int> cells(nseg + 1);
        cells[0] = in.cell_id(0, 0);
        for (int k = 0; k < M; k++) {
            cells[2 * k + 1] = in.ball_cells[best.order[k]];
            cells[2 * k + 2] = in.basket_cells[best.order[k]];
        }

        vector<int> seg_a(nseg), seg_b(nseg);
        vector<string> default_path(nseg);
        {
            int si = 0;
            string cur_path;
            for (char ch : default_raw) {
                if (ch == 'S') {
                    assert(si < nseg);
                    default_path[si++] = cur_path;
                    cur_path.clear();
                } else {
                    cur_path.push_back(ch);
                }
            }
            assert(si == nseg && cur_path.empty());
        }
        int sid = in.state_id(0, 0, 0);
        for (int s = 0; s < nseg; s++) {
            seg_a[s] = sid;
            for (char ch : default_path[s]) {
                if (ch == 'F')
                    sid = in.base_next[sid][0];
                else if (ch == 'L')
                    sid = in.base_next[sid][1];
                else if (ch == 'R')
                    sid = in.base_next[sid][2];
                else if (ch == 'P')
                    sid = ctx.next4[sid][3];
            }
            seg_b[s] = sid;
            assert((seg_a[s] >> 2) == cells[s]);
            assert((seg_b[s] >> 2) == cells[s + 1]);
        }

        const int S = in.N * in.N * 4;
        const int n_ops = best.macro.empty() ? 3 : 4;
        // 逆隣接リストはメンバに保持し、inner vector の capacity を残したまま
        // clear→再構築して再確保を避ける。
        if ((int)seg_rev_buf.size() < S)
            seg_rev_buf.resize(S);
        for (int u = 0; u < S; u++)
            seg_rev_buf[u].clear();
        for (int u = 0; u < S; u++)
            for (int k = 0; k < n_ops; k++)
                seg_rev_buf[ctx.next4[u][k]].push_back(u);
        const vector<vector<int>> &rev = seg_rev_buf;

        vector<vector<string>> cands(nseg);
        for (int s = 0; s < nseg; s++)
            cands[s] = enumerate_segment_paths(
                seg_a[s], seg_b[s], default_path[s], cap, max_slack, rev);

        if constexpr (DEBUG) {
            if (verbose) {
                cerr << "[path_exp] nseg=" << nseg << " cap=" << cap
                     << " max_slack=" << max_slack << " cand_counts=[";
                i64 total_comb_log = 0;
                for (int s = 0; s < nseg; s++) {
                    cerr << cands[s].size() << (s + 1 < nseg ? "," : "");
                    total_comb_log += (i64)cands[s].size();
                }
                cerr << "] sum=" << total_comb_log << el;
            }
        }
        return cands;
    }

    vector<int>
    build_greedy_path_selection(const vector<vector<string>> &cands) const {
        const int n = (int)cands.size();
        vector<int> greedy_sel(n, 0);
        for (int s = 0; s < n; s++) {
            int best_idx = 0;
            int best_bonus = -1;
            int best_len = inf32;
            for (int idx = 0; idx < (int)cands[s].size(); idx++) {
                const string &p = cands[s][idx];
                const int bonus = (!p.empty() && p.back() == 'P' ? 2 : 0) +
                                  (!p.empty() && p.front() == 'P' ? 1 : 0);
                const int len = (int)p.size();
                if (bonus > best_bonus ||
                    (bonus == best_bonus && len < best_len)) {
                    best_bonus = bonus;
                    best_len = len;
                    best_idx = idx;
                }
            }
            greedy_sel[s] = best_idx;
        }
        return greedy_sel;
    }

    // best(macro+order) を固定し、区間パス選択を焼きなまして
    // (最良暫定スコア, その raw_path_ops) を返す。time_limit は絶対時刻[s]。
    // no_improve_limit>0 のとき、その回数連続で非改善なら早期終了（選別用に
    // 軽量化できる。0 で無効）。
    pair<i64, string> optimize_paths_for_best(double time_limit, int max_slack,
                                              int cap,
                                              [[maybe_unused]] bool verbose,
                                              int no_improve_limit = 0) {
        auto get_i = [](const char *name, int def) {
            const char *vv = getenv(name);
            return vv ? stoi(vv) : def;
        };

        [[maybe_unused]] const double t_enter = timer.elapsed();
        g_ctx.build(best.macro, true);
        if constexpr (DEBUG) {
            if (verbose)
                cerr << "[path_prep] enter=" << fixed << setprecision(4)
                     << t_enter << " build_done=" << timer.elapsed()
                     << " time_limit=" << time_limit << el;
        }
        const auto &ctx = g_ctx;
        const int M = (int)best.order.size();
        const int nseg = 2 * M;
        if (nseg == 0) {
            string r =
                build_path_from_start_state(in.state_id(0, 0, 0), best.order);
            return {provisional_score_from_raw(r), r};
        }

        const string default_raw =
            build_path_from_start_state(in.state_id(0, 0, 0), best.order);
        vector<vector<string>> cands =
            build_path_segment_candidates(default_raw, max_slack, cap, verbose);

        struct VectorHash {
            size_t operator()(const vector<int> &v) const {
                u64 h = 0x9e3779b97f4a7c15ULL;
                for (int x : v) {
                    h ^= (u64)x + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
                }
                return (size_t)h;
            }
        };
        unordered_map<vector<int>, i64, VectorHash> score_cache;
        score_cache.reserve(4096);

        auto eval_sel = [&](const vector<int> &v) -> i64 {
            auto it = score_cache.find(v);
            if (it != score_cache.end())
                return it->second;
            const i64 sc = provisional_score_from_segments(cands, v);
            score_cache.emplace(v, sc);
            return sc;
        };

        vector<int> default_sel(nseg, 0); // sel=0 で default パス
        i64 default_prov = eval_sel(default_sel);
        vector<int> greedy_sel = build_greedy_path_selection(cands);
        i64 greedy_prov = eval_sel(greedy_sel);
        vector<int> sel =
            (greedy_prov < default_prov) ? greedy_sel : default_sel;
        i64 cur = min(default_prov, greedy_prov);
        [[maybe_unused]] const i64 init_prov0 =
            cur; // SA 開始時（=選別+orderSA確定の既定パス）
        vector<int> best_sel = sel;
        i64 best_prov = cur;

        // 候補が 2 個以上ある区間のみ近傍対象
        vector<int> movable;
        vector<vector<int>> next_indices(nseg);
        for (int s = 0; s < nseg; s++)
            if ((int)cands[s].size() >= 2) {
                movable.push_back(s);
                const int cnt = (int)cands[s].size();
                next_indices[s].resize(cnt * (cnt - 1));
                int ptr = 0;
                for (int old = 0; old < cnt; old++) {
                    for (int idx = 0; idx < cnt; idx++) {
                        if (idx != old)
                            next_indices[s][ptr++] = idx;
                    }
                }
            }
        if constexpr (DEBUG) {
            if (verbose)
                cerr << "[path_exp] init_prov=" << cur
                     << " default_prov=" << default_prov
                     << " greedy_prov=" << greedy_prov
                     << " movable_segs=" << movable.size() << "/" << nseg
                     << " setup_done=" << fixed << setprecision(4)
                     << timer.elapsed()
                     << " window_left=" << (time_limit - timer.elapsed()) << el;
        }

        if (!movable.empty()) {
            // 区間パス選択の焼きなまし。改善/同値は常に採用し、悪化は
            // Tuning::path_temp0 -> path_temp1 の温度で確率受理する。
            i64 iter = 0, no_improve = 0;
            [[maybe_unused]] i64 accept = 0;
            [[maybe_unused]] const i64 print_every =
                DEBUG ? get_i("AHC_PATH_PRINT_EVERY", 2000) : 0;
            const double time_start = timer.elapsed();
            const double span = max(1e-9, time_limit - time_start);
            double now = time_start;
            double temp = Tuning::path_temp0;
            while (true) {
                now = timer.elapsed();
                if (now >= time_limit)
                    break;
                if (no_improve_limit > 0 && no_improve >= no_improve_limit)
                    break;
                iter++;
                const double progress = min(1.0, (now - time_start) / span);
                if (Tuning::path_use_exp_decay && Tuning::path_temp0 > 0.0 &&
                    Tuning::path_temp1 > 0.0) {
                    temp =
                        Tuning::path_temp0 *
                        pow(Tuning::path_temp1 / Tuning::path_temp0, progress);
                } else {
                    const double p =
                        pow(progress, Tuning::path_power_decay_exp);
                    temp = Tuning::path_temp0 +
                           (Tuning::path_temp1 - Tuning::path_temp0) * p;
                }
                temp = max(1e-9, temp);

                const int s = movable[rng.random32(movable.size())];
                const int old_idx = sel[s];
                const int cnt = (int)cands[s].size();
                const int new_idx = next_indices[s][old_idx * (cnt - 1) +
                                                    (int)rng.random32(cnt - 1)];
                sel[s] = new_idx;

                const i64 cand_score = eval_sel(sel);
                const i64 delta = cand_score - cur;
                if (delta <= 0 ||
                    rng.randomDouble() < exp(-(double)delta / temp)) {
                    cur = cand_score;
                    if constexpr (DEBUG)
                        accept++;
                    if (cur < best_prov) {
                        best_prov = cur;
                        best_sel = sel;
                        no_improve = 0;
                    } else {
                        no_improve++;
                    }
                } else {
                    sel[s] = old_idx;
                    no_improve++;
                }

                if constexpr (DEBUG) {
                    if (verbose && iter % print_every == 0) {
                        cerr << "[path_exp] iter=" << iter << " time=" << fixed
                             << setprecision(3) << timer.elapsed()
                             << " cur=" << cur << " best=" << best_prov
                             << " temp=" << temp << " accept=" << accept << el;
                    }
                }
            }
            if constexpr (DEBUG) {
                if (verbose)
                    cerr << "[path_exp] SA_end iter=" << iter
                         << " accept=" << accept << " best_prov=" << best_prov
                         << " time=" << fixed << setprecision(3)
                         << timer.elapsed() << el;
            }
        }
        if constexpr (DEBUG) {
            if (verbose)
                cerr << "[path_sa_effect] before=" << init_prov0
                     << " after=" << best_prov
                     << " gain=" << (init_prov0 - best_prov) << " nseg=" << nseg
                     << " T=" << in.T << el;
        }
        return {best_prov, build_raw_from_segments(cands, best_sel)};
    }

    void build_answer() {
        const float build_answer_t0 = timer.elapsed();
        g_ctx.build(best.macro, true);
        EvalResult final_eval = evaluate_order(g_ctx, best.order);

        string ops =
            build_path_from_start_state(in.state_id(0, 0, 0), best.order);

        string best_ans;
        if (best.macro.empty()) {
            if ((int)ops.size() > in.T)
                ops.resize(in.T);
            best_ans = ops;
        } else {
            const int first_p = (int)ops.find('P');
            if (first_p == (int)string::npos) {
                best_ans = ops;
            } else {
                best_ans = use_substring_macro_before_main_registration(
                    ops.substr(0, first_p), best.macro,
                    ops.substr(first_p + 1));
            }
            if (output_score_for_limit(best_ans) >= inf64 / 8)
                best_ans = build_plain_answer();
        }

        log_answer_stage("final_opt_base", best_ans);
        answer =
            improve_with_initial_macro_candidates(best_ans, best.order, ops);
        log_answer_stage("final_opt_initial_macro", answer);
        {
            const string before_final_opt = answer;
            auto pick_better = [&](const string &x, const string &y) {
                const i64 sx = output_score_for_limit(x);
                const i64 sy = output_score_for_limit(y);
                if (sx != sy)
                    return sx < sy ? x : y;
                return x.size() <= y.size() ? x : y;
            };
            // マクロ端 DP の 3 変種 (前後S / 前後LR / 全盛り)。E
            // の増え方が異なるので T ぎりぎりのケースに備えて全部試し、それぞれ
            // tail 畳み込みと直列にする。 tail の後方スキャンは M
            // で止まり再生マクロが一意なので安全に合成できる。
            string tail_or_s = improve_with_tail_macro(before_final_opt);
            log_answer_stage("final_opt_tail_macro", tail_or_s);
            const array<array<bool, 2>, 3> variants = {
                {{true, false}, {false, true}, {true, true}}}; // S / LR / all
            const string md = improve_with_macro_both(before_final_opt, 1, 1);
            const string serial = improve_with_tail_macro(md);
            tail_or_s = pick_better(tail_or_s, serial);
            // tail と並列に PF / FP 延長畳み込みを試して良い方を採る。
            tail_or_s = pick_better(tail_or_s,
                                    improve_with_pf_extend(before_final_opt));
            tail_or_s = pick_better(tail_or_s,
                                    improve_with_fp_extend(before_final_opt));
            answer = tail_or_s;
            log_answer_stage("final_opt_macro_both_pick", answer);
        }
        if constexpr (DEBUG) {
            const float build_answer_ms =
                (timer.elapsed() - build_answer_t0) * 1000.0f;
            const i64 out_score = output_score_for_limit(answer);
            const int out_A = (int)answer.size();
            const int out_E = expanded_basic_count(answer, in.T);
            cerr << "[final] score=" << final_eval.score
                 << " A=" << final_eval.A << " E=" << final_eval.E
                 << " T=" << in.T << " out_score=" << out_score
                 << " out_A=" << out_A << " out_E=" << out_E
                 << " macro=" << best.macro << el;
            cerr << "[score_log] stage=final score=" << out_score
                 << " A=" << out_A << " E=" << out_E << " T=" << in.T << el;
            cerr << "[build_answer] " << build_answer_ms << " ms" << el;
        }
    }

  public:
    Solver() {}
#ifdef MB_HARNESS
    // capture したコーパス (各行: base_ans \t use_s \t use_lr) を replay し、
    // improve_with_macro_both の出力を 1 行ずつ stdout に吐く。リファクタ前後の
    // バイト一致検証用。
    void mb_replay(const char *path) {
        std::ifstream ifs(path);
        std::string line;
        while (std::getline(ifs, line)) {
            if (line.empty())
                continue;
            const size_t t1 = line.rfind('\t');
            const size_t t0 = line.rfind('\t', t1 - 1);
            const std::string base_ans = line.substr(0, t0);
            const int us = std::stoi(line.substr(t0 + 1, t1 - t0 - 1));
            const int ul = std::stoi(line.substr(t1 + 1));
            std::cout << improve_with_macro_both(base_ans, (bool)us, (bool)ul)
                      << '\n';
        }
    }
#endif
    void solve() {
        g_lb.init();

        // ===== ブロード探索フェーズ =====
        // コスト下界を目的関数とした軽量 SA
        // で、パスマクロ/ランダムマクロを初期解
        // として広くマクロ空間を探索し、評価した各マクロを候補プールへ集める。
        // プールはコスト下界が小さい上位 broad_pool_k 個を保持する。
        const int pool_k = max(1, Tuning::broad_pool_k);

        // 初期解マクロ列: パスマクロ(2M本) +
        // ランダムマクロ。下界昇順で処理する。
        // 各 init_macro は「1 スロット」であり、焼きなまし後はそのスロットを
        // 改善後のマクロで置換する（途中状態を別候補として溜め込まない）。
        vector<pair<int, string>> init_macros; // (lb, macro)
        int path_macro_total = 0, path_macro_ok = 0;
        int rand_macro_ok = 0;
        {
            vector<string> paths = g_lb.generate_path_macros();
            path_macro_total = (int)paths.size();
            for (string &p : paths) {
                if ((int)p.size() > in.T)
                    continue;
                const int lb = g_lb.cost_lower_bound(p);
                if (lb < inf32) {
                    init_macros.push_back({lb, p});
                    path_macro_ok++;
                }
            }
            for (int r = 0; r < Tuning::broad_random_count; r++) {
                Macro mc = random_macro_struct();
                string s = mc.to_string();
                if ((int)s.size() > in.T)
                    continue;
                const int lb = g_lb.cost_lower_bound(s);
                if (lb < inf32) {
                    init_macros.push_back({lb, s});
                    rand_macro_ok++;
                }
            }
            sort(
                init_macros.begin(), init_macros.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });
        }
        if constexpr (DEBUG) {
            const int best_init_lb =
                init_macros.empty() ? -1 : init_macros.front().first;
            cerr << "[broad_init] path_macros=" << path_macro_ok << "/"
                 << path_macro_total << " rand_macros=" << rand_macro_ok << "/"
                 << Tuning::broad_random_count
                 << " init_macros=" << init_macros.size()
                 << " best_lb=" << best_init_lb << " time=" << fixed
                 << setprecision(3) << timer.elapsed() << el;
        }

        // 近傍選択サンプラー（macro_sa と同じ重み）。
        AliasWeightedSampler move_selector;
        {
            vector<double> mw(Tuning::broad_move_weights.begin(),
                              Tuning::broad_move_weights.end());
            move_selector.build(mw);
        }
        const int per_trials = max(1, Tuning::broad_per_macro_trials);
        const double bt0 = Tuning::broad_start_temp;
        const double bt1 = Tuning::broad_end_temp;
        const double broad_end = Tuning::broad_phase_end;
        const bool broad_exp_decay = Tuning::broad_use_exp_decay;
        const double broad_power_exp = Tuning::broad_power_decay_exp;

        // 候補集合: init_macro と 1:1 のスロット。初期値は init_macro
        // そのもの。
        // 焼きなましで改善されたスロットは、そのベスト状態で置換する。
        // （未処理スロットは元の init_macro のまま候補に残る。）
        vector<pair<int, string>> cand = init_macros; // (lb, macro)

        i64 broad_iter = 0;
        i64 broad_accept = 0;    // SA 受理回数（ログ用）
        int broad_processed = 0; // 焼きなまし着手したスロット数（ログ用）
        int broad_improved = 0;  // 焼きなましで改善したスロット数（ログ用）
        bool broad_timeout = false;
        for (int si = 0; si < (int)init_macros.size(); si++) {
            if (timer.elapsed() >= broad_end) {
                broad_timeout = true;
                break;
            }
            const auto &im = init_macros[si];
            Macro mc = macro_struct_from_string(im.second);
            string cur_s = mc.to_string();
            if ((int)cur_s.size() > in.T)
                continue;
            int cur_lb = g_lb.cost_lower_bound(cur_s);
            if (cur_lb >= inf32)
                continue;
            broad_processed++;
            // このスロットの焼きなまし中ベスト（元の init_macro
            // も候補に含める）。
            int slot_best_lb = im.first;
            string slot_best_s = im.second;
            if (cur_lb < slot_best_lb) {
                slot_best_lb = cur_lb;
                slot_best_s = cur_s;
            }
            for (int t = 0; t < per_trials; t++) {
                broad_iter++;
                if (timer.elapsed() >= broad_end) {
                    broad_timeout = true;
                    break;
                }
                const int mt = move_selector.sample();
                Macro saved = mc;
                if (!apply_macro_move(mc, mt, broad_move_tuning()))
                    continue;
                string ns = mc.to_string();
                if ((int)ns.size() > in.T) {
                    mc = saved;
                    continue;
                }
                const int nlb = g_lb.cost_lower_bound(ns);
                if (nlb >= inf32) {
                    mc = saved;
                    continue;
                }
                if (nlb < slot_best_lb) {
                    slot_best_lb = nlb;
                    slot_best_s = ns;
                }
                // SA 受理判定（コスト下界の最小化）。
                const double progress = (double)t / per_trials;
                double temp = bt0;
                if (broad_exp_decay && bt0 > 0.0 && bt1 > 0.0) {
                    temp = bt0 * pow(bt1 / bt0, progress);
                } else {
                    const double p = pow(progress, broad_power_exp);
                    temp = bt0 + (bt1 - bt0) * p;
                }
                const double thr =
                    (double)cur_lb - temp * log_fast(rng.randomDoubleOpen01());
                if ((double)nlb <= thr) {
                    cur_s.swap(ns);
                    cur_lb = nlb;
                    broad_accept++;
                } else {
                    mc = saved;
                }
            }
            // 改善後の状態でスロットを置換する。
            if (slot_best_lb < cand[si].first) {
                broad_improved++;
                cand[si] = {slot_best_lb, slot_best_s};
            }
        }

        // スロット集合を重複除去してプールを作る（full-eval の入力）。
        vector<pair<int, string>> pool; // (lb, macro)
        {
            unordered_set<string> seen;
            seen.reserve(cand.size() * 2);
            pool.reserve(cand.size());
            for (auto &c : cand)
                if (c.first < inf32 && seen.insert(c.second).second)
                    pool.push_back(c);
        }
        if constexpr (DEBUG) {
            int pool_best_lb = inf32;
            for (const auto &pr : pool)
                smin(pool_best_lb, pr.first);
            cerr << "[broad_sa] iter=" << broad_iter
                 << " accept=" << broad_accept
                 << " processed=" << broad_processed << "/"
                 << init_macros.size() << " improved=" << broad_improved
                 << " pool=" << pool.size() << " pool_best_lb=" << pool_best_lb
                 << " timeout=" << (broad_timeout ? 1 : 0) << " time=" << fixed
                 << setprecision(3) << timer.elapsed()
                 << " (broad_end=" << broad_end << ")" << el;
        }

        // ===== 候補プール上位を full-eval して macro_sa の初期解にする =====
        // （macro_sa は restart_states として random_top を参照するので、従来と
        //  同じ変数名で best 先頭の vector<State> を構築する。）
        const int random_top_k = max(1, Tuning::random_top_k);
        auto random_better = [](const State &a, const State &b) {
            if (a.score != b.score)
                return a.score < b.score;
            if (a.last_E != b.last_E)
                return a.last_E < b.last_E;
            if (a.last_A != b.last_A)
                return a.last_A < b.last_A;
            return a.macro < b.macro;
        };
        sort(pool.begin(), pool.end(), [](const auto &a, const auto &b) {
            if (a.first != b.first)
                return a.first < b.first;
            return a.second < b.second;
        });
        vector<State> random_top;
        random_top.reserve(random_top_k + 1);
        // ログ用: E<=T 候補数 / 初回発見時刻 / eval_deadline 超過で延長したか /
        // 最小 E / 打ち切り理由。
        int legal_e_count = 0;
        double first_legal_time = -1.0;
        bool eval_extended = false;
        int min_eval_E = inf32;
        int best_eval_idx = -1;      // evaluated list index, 0-based
        int best_eval_pool_idx = -1; // sorted pool index, 0-based
        const char *broad_eval_stop = "all";
        {
            const int eval_k = min((int)pool.size(), pool_k);
            const double eval_deadline = Tuning::broad_eval_deadline;
            const double hard_deadline = Tuning::broad_eval_hard_deadline;
            // E<=T の候補が 1 つでも見つかったか。見つかるまでは eval_deadline
            // を 超えても評価を続け、最悪 hard_deadline で打ち切る。
            bool have_legal_e = false;
            for (int i = 0; i < eval_k; i++) {
                // 先頭(下界最良)は必ず評価する。以降は時間制限で打ち切るが、
                // E<=T 候補が未発見なら eval_deadline を無視して続行する。
                if (i > 0) {
                    const double now = timer.elapsed();
                    if (now >= hard_deadline) {
                        broad_eval_stop = "hard_deadline";
                        break;
                    }
                    if (have_legal_e && now >= eval_deadline) {
                        broad_eval_stop = "eval_deadline";
                        break;
                    }
                    // E<=T 未発見のまま eval_deadline を超えて延長中。
                    if (!have_legal_e && now >= eval_deadline)
                        eval_extended = true;
                }
                State st;
                st.macro_struct = macro_struct_from_string(pool[i].second);
                st.macro = st.macro_struct.to_string();
                st.score = st.calc_score_full();
                if (best_eval_idx < 0 ||
                    random_better(st, random_top[best_eval_idx])) {
                    best_eval_idx = (int)random_top.size();
                    best_eval_pool_idx = i;
                }
                smin(min_eval_E, st.last_E);
                if (st.last_E <= in.T) {
                    if (!have_legal_e)
                        first_legal_time = timer.elapsed();
                    have_legal_e = true;
                    legal_e_count++;
                }
                random_top.push_back(std::move(st));
            }
        }
        if constexpr (DEBUG) {
            cerr << "[broad_eval] evaluated=" << random_top.size()
                 << " legal_e=" << legal_e_count << " min_E=" << min_eval_E
                 << " T=" << in.T << " extended=" << (eval_extended ? 1 : 0)
                 << " best_eval_idx=" << best_eval_idx << " best_eval_pos="
                 << (best_eval_idx < 0 ? -1 : best_eval_idx + 1)
                 << " best_pool_idx=" << best_eval_pool_idx << " best_pool_pos="
                 << (best_eval_pool_idx < 0 ? -1 : best_eval_pool_idx + 1)
                 << " first_legal_t=" << fixed << setprecision(3)
                 << first_legal_time << " stop=" << broad_eval_stop
                 << " time=" << timer.elapsed() << el;
            if (legal_e_count == 0)
                cerr << "[broad_eval][WARN] no E<=T candidate found in broad "
                        "phase (min_E="
                     << min_eval_E << " > T=" << in.T << ")" << el;
        }
        {
            // フォールバック: プールが空ならランダム初期解を1件確保する。
            if (random_top.empty()) {
                State st;
                st.init();
                random_top.push_back(std::move(st));
            }
            sort(random_top.begin(), random_top.end(), random_better);
            if ((int)random_top.size() > random_top_k)
                random_top.resize(random_top_k);
        }
        assert(!random_top.empty());
        best = random_top.front();
        if constexpr (DEBUG) {
            cerr << "[broad_end] time=" << fixed << setprecision(3)
                 << timer.elapsed() << " broad_iter=" << broad_iter
                 << " init_macros=" << init_macros.size()
                 << " pool=" << pool.size()
                 << " evaluated=" << random_top.size()
                 << " score=" << best.score << " A=" << best.last_A
                 << " E=" << best.last_E << " T=" << in.T
                 << " macro=" << best.macro << el;
        }

        SAParams sap{
            .name = "macro_sa",
            .time_limit = Tuning::sa_time_limit,
            .start_temp = Tuning::sa_start_temp,
            .end_temp = Tuning::sa_end_temp,
            .use_exp_decay = Tuning::use_exp_decay,
            .power_decay_exp = Tuning::power_decay_exp,
            .time_check_interval = Tuning::sa_time_check_interval,
            .stats_interval = 0,
            .move_names = {"CHG_LEN1_PLUS", "CHG_LEN1_MINUS", "CHG_LENALL",
                           "FLIP1", "FLIPALL", "DEL_MID", "INS_MID", "PREFIX",
                           "SUFFIX", "SWAP_MID", "REV_MID", "RELOC_MID",
                           "UNIFORM_LEN", "MOVE_LEN"},
            .move_weights = Tuning::sa_move_weights,
            .allow_worse_moves = Tuning::sa_allow_worse_moves,
            .maximize = false,
            .restart_iters = Tuning::macro_sa_restart_iters,
        };
        DefaultMoveApplier applier(sap);
        SA<DefaultMoveApplier> sa(sap);
        best = sa.run(best, applier, &random_top);
        if constexpr (DEBUG)
            cerr << "[macro_sa_phase_end] score=" << best.score
                 << " A=" << best.last_A << " E=" << best.last_E
                 << " T=" << in.T << " macro=" << best.macro << el;

        const int path_cap = Tuning::path_cap;
        const int path_slack = Tuning::path_slack;

        // top-K 選別フェーズ（従来どおり provisional で 1 つ選ぶ）
        vector<State> top_states = sa.top_states;
        {
            bool has_best_macro = false;
            for (const State &cand : top_states) {
                if (cand.macro == best.macro) {
                    has_best_macro = true;
                    break;
                }
            }
            if (!has_best_macro)
                top_states.push_back(best);
        }
        {
            // マクロ SA スコアの良い順(最小化)に並べ、良いものから評価する。
            sort(top_states.begin(), top_states.end(),
                 [](const State &a, const State &b) {
                     if (a.score != b.score)
                         return a.score < b.score;
                     if (a.last_E != b.last_E)
                         return a.last_E < b.last_E;
                     if (a.last_A != b.last_A)
                         return a.last_A < b.last_A;
                     return a.macro < b.macro;
                 });

            vector<RestoredMacroCandidate> candidates;
            candidates.reserve(top_states.size());
            for (const State &st : top_states) {
                // 定数で決めた経過時刻で打ち切る。少なくとも先頭(最良マクロ)
                // は必ず評価する。
                if (!candidates.empty() &&
                    timer.elapsed() > Tuning::macro_preselect_deadline)
                    break;
                RestoredMacroCandidate cand;
                cand.state = st;
                cand.provisional_score = compute_provisional_macro_score(cand);
                candidates.push_back(std::move(cand));
            }
            assert(!candidates.empty());

            int selected_idx = 0;
            for (int i = 1; i < (int)candidates.size(); i++) {
                const auto &a = candidates[i];
                const auto &b = candidates[selected_idx];
                const bool better =
                    a.provisional_score < b.provisional_score ||
                    (a.provisional_score == b.provisional_score &&
                     (a.state.score < b.state.score ||
                      (a.state.score == b.state.score &&
                       a.restored_answer.size() < b.restored_answer.size())));
                if (better)
                    selected_idx = i;
            }
            best = candidates[selected_idx].state;
            if constexpr (DEBUG) {
                const auto &sel = candidates[selected_idx];
                cerr << "[macro_preselect] top_k=" << candidates.size()
                     << " provisional=" << sel.provisional_score
                     << " init_score=" << sel.init_best_score
                     << " both_gain=" << sel.both_all_gain
                     << " restored_A=" << sel.restored_answer.size()
                     << " macro_score=" << best.score
                     << " macro_A=" << best.last_A << " macro_E=" << best.last_E
                     << " T=" << in.T << " macro=" << best.macro << el;
            }
        }

        // order SA フェーズ
        {
            g_ctx.build(best.macro, true);
            EvalResult order_init = evaluate_order(g_ctx, best.order);
            EvalResult order_final = sa_improve_order(
                g_ctx, order_init, Tuning::order_sa_time_limit);
            best.order = order_final.order;
            best.score = order_final.score;
            best.last_A = order_final.A;
            best.last_E = order_final.E;
        }
        if constexpr (DEBUG)
            cerr << "[order_sa_phase_end] score=" << best.score
                 << " A=" << best.last_A << " E=" << best.last_E
                 << " T=" << in.T << " macro=" << best.macro << el;
        // パス焼きなましフェーズ: order SA で確定した (macro, order) のもとで
        // 区間パス選択を焼きなまし、確定パスに build_answer
        // 相当の最終改善をかけ て出力する（order SA
        // とこのフェーズの境界が新設の時間区切り）。
        {
            auto [prov, raw] = optimize_paths_for_best(
                Tuning::path_anneal_time_limit, path_slack, path_cap, true);
            answer = finalize_answer_from_raw(raw);
            if constexpr (DEBUG)
                cerr << "[path_anneal_final] best_prov=" << prov
                     << " out_A=" << answer.size()
                     << " out_score=" << output_score_for_limit(answer)
                     << " T=" << in.T << el;
        }
    }
    void print() {
        for (char ch : answer) {
            cout << ch << el;
        }
    }
};

int main() {
    init_log_table();
    // init_gauss_table();
    in.input();
    g_log_T = in.T;
    g_in_M = in.M;
    Tuning::apply_preset(in.N, in.M);
    // 環境変数からチューニングパラメータを読み込む
    {
        auto get_d = [](const char *name, double def) {
            const char *v = getenv(name);
            return v ? stod(v) : def;
        };
        auto get_i = [](const char *name, int def) {
            const char *v = getenv(name);
            return v ? stoi(v) : def;
        };
        Tuning::time_boundary_scale =
            get_d("AHC_TIME_BOUNDARY_SCALE", Tuning::time_boundary_scale);

        // Broad macro search
        Tuning::broad_phase_end =
            get_d("AHC_BROAD_PHASE_END", Tuning::broad_phase_end);
        Tuning::broad_per_macro_trials =
            max(1, get_i("AHC_BROAD_PER_MACRO_TRIALS",
                         Tuning::broad_per_macro_trials));
        Tuning::broad_random_count =
            max(0, get_i("AHC_BROAD_RANDOM_COUNT", Tuning::broad_random_count));
        Tuning::broad_pool_k =
            max(1, get_i("AHC_BROAD_POOL_K", Tuning::broad_pool_k));
        Tuning::broad_start_temp =
            get_d("AHC_BROAD_START_TEMP", Tuning::broad_start_temp);
        Tuning::broad_end_temp =
            get_d("AHC_BROAD_END_TEMP", Tuning::broad_end_temp);
        Tuning::broad_use_exp_decay =
            (bool)get_i("AHC_BROAD_USE_EXP_DECAY", Tuning::broad_use_exp_decay);
        Tuning::broad_power_decay_exp =
            get_d("AHC_BROAD_POWER_DECAY_EXP", Tuning::broad_power_decay_exp);
        Tuning::broad_eval_deadline =
            get_d("AHC_BROAD_EVAL_DEADLINE", Tuning::broad_eval_deadline);
        Tuning::broad_eval_hard_deadline = get_d(
            "AHC_BROAD_EVAL_HARD_DEADLINE", Tuning::broad_eval_hard_deadline);
        Tuning::random_top_k =
            max(1, get_i("AHC_RANDOM_TOP_K", Tuning::random_top_k));

        // Macro SA
        Tuning::sa_time_limit =
            get_d("AHC_SA_TIME_LIMIT", Tuning::sa_time_limit);
        Tuning::sa_start_temp =
            get_d("AHC_SA_START_TEMP", Tuning::sa_start_temp);
        Tuning::sa_end_temp = get_d("AHC_SA_END_TEMP", Tuning::sa_end_temp);
        Tuning::use_exp_decay =
            (bool)get_i("AHC_USE_EXP_DECAY", Tuning::use_exp_decay);
        Tuning::power_decay_exp =
            get_d("AHC_POWER_DECAY_EXP", Tuning::power_decay_exp);
        for (int i = 0; i < (int)SAParams::MOVE_COUNT; i++) {
            string name = "AHC_W" + to_string(i);
            const char *v = getenv(name.c_str());
            if (v)
                Tuning::sa_move_weights[i] = stod(v);
            string broad_name = "AHC_BROAD_W" + to_string(i);
            const char *bv = getenv(broad_name.c_str());
            if (bv)
                Tuning::broad_move_weights[i] = stod(bv);
        }
        const int macro_len_delta = get_i("AHC_MACRO_LEN_DELTA", 1);
        Tuning::macro_len_delta_plus =
            get_i("AHC_MACRO_LEN_DELTA_PLUS", macro_len_delta);
        Tuning::macro_len_delta_minus =
            get_i("AHC_MACRO_LEN_DELTA_MINUS", macro_len_delta);
        Tuning::macro_all_len_delta =
            get_i("AHC_MACRO_ALL_LEN_DELTA", Tuning::macro_all_len_delta);
        Tuning::macro_move_len_max =
            get_i("AHC_MACRO_MOVE_LEN_MAX", Tuning::macro_move_len_max);
        const int broad_len_delta =
            get_i("AHC_BROAD_LEN_DELTA", Tuning::macro_len_delta_plus);
        Tuning::broad_len_delta_plus =
            get_i("AHC_BROAD_LEN_DELTA_PLUS", broad_len_delta);
        Tuning::broad_len_delta_minus =
            get_i("AHC_BROAD_LEN_DELTA_MINUS", broad_len_delta);
        Tuning::broad_all_len_delta =
            get_i("AHC_BROAD_ALL_LEN_DELTA", Tuning::broad_all_len_delta);
        Tuning::broad_move_len_max =
            get_i("AHC_BROAD_MOVE_LEN_MAX", Tuning::broad_move_len_max);
        Tuning::macro_top_k =
            max(1, get_i("AHC_MACRO_TOP_K", Tuning::macro_top_k));
        Tuning::macro_restart_interval =
            max(1, get_i("AHC_MACRO_RESTART_INTERVAL",
                         Tuning::macro_restart_interval));
        Tuning::macro_restart_lockout_mul = get_d(
            "AHC_MACRO_RESTART_LOCKOUT_MUL", Tuning::macro_restart_lockout_mul);
        Tuning::macro_sa_restart_iters =
            max(0, get_i("AHC_MACRO_SA_RESTART_ITERS",
                         Tuning::macro_sa_restart_iters));

        // 下界スコア(bs)による早期枝刈り
        Tuning::bs_prune_coef =
            get_d("AHC_BS_PRUNE_COEF", Tuning::bs_prune_coef);
        Tuning::bs_prune_offset =
            get_d("AHC_BS_PRUNE_OFFSET", Tuning::bs_prune_offset);

        // Macro preselect
        Tuning::macro_preselect_deadline = get_d(
            "AHC_MACRO_PRESELECT_DEADLINE", Tuning::macro_preselect_deadline);

        // Order SA
        Tuning::order_sa_time_limit =
            get_d("AHC_ORDER_SA_TIME_LIMIT", Tuning::order_sa_time_limit);
        Tuning::order_sa_start_temp =
            get_d("AHC_ORDER_SA_START_TEMP", Tuning::order_sa_start_temp);
        Tuning::order_sa_end_temp =
            get_d("AHC_ORDER_SA_END_TEMP", Tuning::order_sa_end_temp);
        Tuning::order_sa_use_exp_decay = (bool)get_i(
            "AHC_ORDER_SA_USE_EXP_DECAY", Tuning::order_sa_use_exp_decay);
        Tuning::order_sa_power_decay_exp = get_d(
            "AHC_ORDER_SA_POWER_DECAY_EXP", Tuning::order_sa_power_decay_exp);
        for (int i = 0; i < Tuning::ORDER_MOVE_COUNT; i++) {
            string name = "AHC_ORDER_W" + to_string(i);
            const char *v = getenv(name.c_str());
            if (v)
                Tuning::order_sa_move_weights[i] = stod(v);
        }

        // Path annealing
        Tuning::path_anneal_time_limit =
            get_d("AHC_PATH_TL", Tuning::path_anneal_time_limit);
        Tuning::path_slack =
            max(0, get_i("AHC_PATH_SLACK", Tuning::path_slack));
        Tuning::path_cap = max(1, get_i("AHC_PATH_K", Tuning::path_cap));
        Tuning::path_implicit_max_len =
            get_i("AHC_PATH_IMPLICIT_MAX_LEN", Tuning::path_implicit_max_len);
        Tuning::path_temp0 = get_d("AHC_PATH_TEMP0", Tuning::path_temp0);
        Tuning::path_temp1 = get_d("AHC_PATH_TEMP1", Tuning::path_temp1);
        Tuning::path_use_exp_decay =
            (bool)get_i("AHC_PATH_USE_EXP_DECAY", Tuning::path_use_exp_decay);
        Tuning::path_power_decay_exp =
            get_d("AHC_PATH_POWER_DECAY_EXP", Tuning::path_power_decay_exp);
        Tuning::apply_time_boundary_scale();
    }
#ifdef MB_HARNESS
    if (const char *rp = getenv("AHC_MB_REPLAY")) {
        Solver s;
        s.mb_replay(rp);
        cout.flush();
        _Exit(0);
    }
#endif
    Solver solver;
    solver.solve();
    solver.print();
    if constexpr (DEBUG) {
        g_prof.print();
        cerr.flush();
    }
    cout.flush();
    _Exit(0);
}
