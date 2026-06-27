
#ifdef ONLINE_JUDGE
#pragma GCC optimize "Ofast,omit-frame-pointer,inline,unroll-all-loops"
#pragma GCC target("avx2,bmi2")
#define SUBMIT
#else
//#define SUBMIT
#endif

#include <bits/stdc++.h>
#include <immintrin.h>

using namespace std;
using namespace chrono;

typedef long long ll;
typedef unsigned long long ull;
#define REP(i, n) for(int i=0; i<int(n); i++)
#define REPi(i, a, b) for(int i=int(a); i<int(b); i++)
#define MEMS(a,b) memset(a,b,sizeof(a))
#define mp make_pair
#define MOD(a, m) ((a % m + m) % m)
#define ALL(a) (a).begin(), (a).end()
#define RALL(a) (a).rbegin(), (a).rend()
template<class T>bool chmax(T &a, const T &b) { if (a<b) { a=b; return 1; } return 0; }
template<class T>bool chmin(T &a, const T &b) { if (b<a) { a=b; return 1; } return 0; }
constexpr ll MOD = 1e9+7;
constexpr ll INF = 1e14;

constexpr int SEED = 1000;

static mt19937 engine;

//#define FIX_LOOP

#ifdef AWS_CLOUD
#define AWS_TIME_SCALE 2.0
#else
#define AWS_TIME_SCALE 1.0
#endif

namespace Env {
    constexpr double time_limit = AWS_TIME_SCALE * 1.965;
    constexpr double start_temp = 1000;
    constexpr double end_temp = 10;
    constexpr bool minimization = false;
    constexpr int n_transition = 1;
    constexpr int beam_width = 10;
};

#ifdef SUBMIT
#define DEBUG(fmt, ...) ;
#define DUMP(fmt, ...) ;
#define ASSERT(expr, fmt_str, ...) ;
#else
#define DEBUG(fmt, ...) std::cerr << std::format(fmt __VA_OPT__(,) __VA_ARGS__)
#define DUMP(var) std::cerr << #var << " = " << (var) << std::endl;
#define ASSERT(expr, fmt_str, ...) \
    do { \
        if (!(expr)) { \
            std::cerr << "===================================================" << endl;  \
            std::cerr << std::format( \
                "Assertion failed: {}\nFile: {}:{}\nFunction: {}\nMessage: {}\n", \
                #expr, __FILE__, __LINE__, __func__, std::format(fmt_str __VA_OPT__(,) __VA_ARGS__) \
            ); \
            std::cerr << "===================================================" << endl;  \
            std::abort(); \
        } \
    } while (0)
#include "dbg_utils.h"
#endif // SUBMIT

////////////////////////////////////////////////////////////////////

using time_point_t = std::chrono::_V2::system_clock::time_point;

// D, R, U, L
constexpr int dx[] = {1, 0, -1, 0};
constexpr int dy[] = {0, 1, 0, -1};

////////////////////////////////////////////////////////////////////

/// xor128

struct Randxor {
    unsigned int x, y, z, w;

    Randxor() : x(123456789), y(362436069), z(521288629), w(88675123) {}

    unsigned int get(){
        unsigned int t;
        t = (x^(x<<11));
        x = y;
        y = z;
        z = w;
        return( w=(w^(w>>19))^(t^(t>>8)) );
    }
};

Randxor randxor;

/// return [0,1)
double rand01(){
    return 1.0 * randxor.get() / numeric_limits<unsigned int>::max();
}

/// return [left, right)
int rand_int(const int left, const int right){
    assert(right > left);
    return randxor.get() % (right - left) + left;
}

template<class T>
void shuffle(vector<T>& v) {
    int sz = v.size();
    for(int i = sz; i > 1; i--) {
        auto p = rand_int(0, i);
        swap(v[i-1], v[p]);
    }
}

template<class T>
inline T sample(const vector<T>& v) {
    ASSERT(v.size() > 0, "");
    return v[rand_int(0, v.size())];
}

////////////////////////////////////////////////////////////////////

struct Timer {
    std::chrono::_V2::system_clock::time_point sp;

    Timer() : sp(system_clock::now()) {}

    double get_time() const {
        const double t = duration_cast<microseconds>(system_clock::now() - sp).count() * 1e-6;
        return t;
    }
};

Timer timer;

////////////////////////////////////////////////////////////////////

template <std::floating_point T>
double get_linear_interpolate(T progress, double start_val, double end_val) {
    return start_val + (end_val - start_val) * progress;
}

template <std::floating_point T>
double get_exponential_interpolate(T progress, double start_val, double end_val) {
    return start_val * pow(end_val/start_val, progress);
}

// maintain bottom/top-K values. T must support less<T> for bottom-K and greater<T> for top-K.
template<class T, class Compare = less<T>>
struct SortedList {
    priority_queue<T, vector<T>, Compare> que;
    unsigned int size_limit;

    SortedList(unsigned int size) : size_limit(size) {}

    bool can_insert(T x) const {
        return que.size() < size_limit || Compare()(x, que.top());
    }

    void insert(T x) {
        if (!can_insert(x)) return;
        que.emplace(x);
        if (que.size() > size_limit) que.pop();
    }

    vector<T> get_list() {
        vector<T> ret;
        while (!que.empty()) {
            const auto x = que.top();
            que.pop();
            ret.emplace_back(x);
        }
        reverse(ALL(ret));
        // sorted by increasing order for bottom-K, while decreasing order for top-K
        return ret;
    }

    size_t size() const {
        return que.size();
    }
};

///////////////////////////////////////////////////////////////////

namespace input {
    constexpr int N = 10;
    constexpr int N2 = N * N;
    constexpr int M_MAX = 8;
    int M;
    constexpr int T = 100;
    int U;
    //array<array<int, N>, N> V;
    array<int, N2> V;
    vector<int> sxy;

    void read_input() {
        int _;
        cin >> _ >> M >> _ >> U;
        REP(i, N) {
            REP(j, N) {
                cin >> V[i * N + j];
            }
        }
        sxy.resize(M);
        REP(i, M) {
            int x, y;
            cin >> x >> y;
            sxy[i] = x * N + y;
        }
    }
};

int enc(int x, int y) {
    return x * input::N + y;
}

// return [row, col]
pair<int, int> dec(int v) {
    return {v / input::N, v % input::N};
}

int get_eval_score(double S0A) {
    return 1e5 * log2(1 + S0A);
}

#ifndef SUBMIT
#include "tester.cpp"
unique_ptr<TESTER::Tester> tester;
#endif

////////////////////////////////////////////////////////////////////

//using Bitboard = std::bitset<input::N2>;
using Bitboard = __uint128_t;

Bitboard mask_not_left_edge;
Bitboard mask_not_right_edge;
Bitboard mask_board;

// main関数などの初期化処理（build_adjs等と同じタイミング）で一度だけ呼ぶ
void init_bitboards() {
    for (int i = 0; i < input::N2; ++i) {
        if (i % input::N != 0) mask_not_left_edge |= (__uint128_t(1) << i);
        if (i % input::N != input::N - 1) mask_not_right_edge |= (__uint128_t(1) << i);
        mask_board |= (__uint128_t(1) << i);
    }
}

// 任意のビットボードに対して、上下左右に隣接するマスのビットボードを返す
inline Bitboard get_adjacent(const Bitboard& b) {
    return (((b << 1) & mask_not_left_edge) | 
           ((b >> 1) & mask_not_right_edge) | 
           (b << input::N) | 
           (b >> input::N)) & mask_board;
}

// 自身と隣接マスを合わせたビットボードを返す（膨張処理）
inline Bitboard dilate(const Bitboard& b) {
    return b | get_adjacent(b);
}

////////////////////////////////////////////////////////////////////

array<int, 4> di;
void build_di() {
    di[0] = input::N;
    di[1] = 1;
    di[2] = -input::N;
    di[3] = -1;
}

vector<int> adjs;
array<int, input::N2> adj_start;
array<int, input::N2> adj_count;
void build_adjs() {
    REP(v, input::N2) {
        adj_start[v] = adjs.size();
        const auto [x, y] = dec(v);
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < input::N && ny >= 0 && ny < input::N) {
                adjs.push_back(v + di[d]);
                ASSERT(v + di[d] >= 0, "v = {}, di[d] = {}, d = {}, nx = {}, ny = {}, x = {}, y = {}", v, di[d], d, nx, ny, x, y);
            }
        }
        adj_count[v] = adjs.size() - adj_start[v];
    }
}

pair<int, int> get_adj_info(int v) {
    return {adj_start[v], adj_count[v]};
}

////////////////////////////////////////////////////////////////////

using CellSel = bitset<input::N2>;

struct CellInfo {
    int level;
    int owner;
    // 0: 誰の領土でもない, 1: 自分の領土でレベル < U, 2: 他のプレイヤーの領土でレベル == 1, 3: 他のプレイヤーの領土でレベル >= 2, 4: 自分の領土でレベル == U
    array<uint8_t, input::M_MAX> cell_type;
    CellInfo() : level(0), owner(-1) {
        cell_type.fill(0);
    }

    void update_cell_state_type() {
        for (int p = 1; p < input::M; ++p) {
            if (owner == -1) {
                cell_type[p] = 0;
            } else if (owner == p) {
                cell_type[p] = (level < input::U) ? 1 : 4;
            } else {
                cell_type[p] = (level == 1) ? 2 : 3;
            }
        }
    }
};

struct Field {
    array<CellInfo, input::N2> cells;
    Field() {}
};

namespace FastCon9 {
vector<bool> con9;
vector<bool> connect9() {
    vector<bool> ok(1 << 9, false);
    for (int mask = 0; mask < (1 << 9); ++mask) {
        int k = 0;
        for (int i = 0; i < 4; ++i) {
            if ((mask >> (i * 2 + 1)) & 1) {
                ++k;
            }
        }
        vector<tuple<int, int, int>> triplets = {{0, 1, 3}, {1, 2, 5}, {3, 6, 7}, {5, 7, 8}};
        for (auto [a, b, c] : triplets) {
            if ((mask >> a & 1) && (mask >> b & 1) && (mask >> c & 1)) {
                --k;
            }
        }
        ok[mask] = (k == 1);
    }
    return ok;
}

struct FastMask9 {
    int shift_up;
    bool is_left_up;
    int shift_mid;
    bool is_left_mid;
    int shift_down;
    bool is_left_down;
    uint32_t final_mask;  // 盤面外のビットを落とす用
};

array<FastMask9, input::N2> fm9;
void init_fast_mask9() {
    for (int x = 0; x < input::N; ++x) {
        for (int y = 0; y < input::N; ++y) {
            int v = x * input::N + y;
            FastMask9& fm = fm9[v];

            // final_mask の初期化 (全9ビットを1にする)
            fm.final_mask = 0b111111111;

            // --- 上下端のマスク処理 ---
            if (x == 0) fm.final_mask &= ~0b000000111;             // 上の行(0,1,2 bit)をクリア
            if (x == input::N - 1) fm.final_mask &= ~0b111000000;  // 下の行(6,7,8 bit)をクリア

            // --- 左右端のマスク処理 ---
            if (y == 0) fm.final_mask &= ~0b001001001;             // 左の列(0,3,6 bit)をクリア
            if (y == input::N - 1) fm.final_mask &= ~0b100100100;  // 右の列(2,5,8 bit)をクリア

            // --- シフト量の計算 ---
            // 抽出元の起点は (行の先頭の y-1)。
            // それを抽出先のビット位置 (0, 3, 6) に合わせるための差分を計算。
            // 差分が正なら右シフト、負なら左シフトになる。

            // 1. 上の行 (ターゲット: 0ビット目)
            int src_up = (x - 1) * input::N + (y - 1);
            if (src_up >= 0) {
                fm.shift_up = src_up;
                fm.is_left_up = false;
            } else {
                fm.shift_up = -src_up;
                fm.is_left_up = true;
            }

            // 2. 真ん中の行 (ターゲット: 3ビット目)
            int src_mid = x * input::N + (y - 1);
            int diff_mid = src_mid - 3;
            if (diff_mid >= 0) {
                fm.shift_mid = diff_mid;
                fm.is_left_mid = false;
            } else {
                fm.shift_mid = -diff_mid;
                fm.is_left_mid = true;
            }

            // 3. 下の行 (ターゲット: 6ビット目)
            int src_down = (x + 1) * input::N + (y - 1);
            int diff_down = src_down - 6;
            if (diff_down >= 0) {
                fm.shift_down = diff_down;
                fm.is_left_down = false;
            } else {
                fm.shift_down = -diff_down;
                fm.is_left_down = true;
            }
        }
    }
}

// 盤面配列ではなく、プレイヤーの Bitboard を直接渡す
inline int check_can_delete_shift(Bitboard territory, int cell) {
    const FastMask9& fm = fm9[cell];

    // __uint128_t のビット演算
    Bitboard mask = 0;

    // 上の行を 0~2 ビット目に持ってくる
    if (fm.is_left_up)
        mask |= (territory << fm.shift_up) & 0b111;
    else
        mask |= (territory >> fm.shift_up) & 0b111;

    // 真ん中の行を 3~5 ビット目に持ってくる
    if (fm.is_left_mid)
        mask |= (territory << fm.shift_mid) & 0b111000;
    else
        mask |= (territory >> fm.shift_mid) & 0b111000;

    // 下の行を 6~8 ビット目に持ってくる
    if (fm.is_left_down)
        mask |= (territory << fm.shift_down) & 0b111000000;
    else
        mask |= (territory >> fm.shift_down) & 0b111000000;

    // 最後に盤面外にはみ出したゴミビットを綺麗に消す
    return static_cast<int>(mask) & fm.final_mask;
}

struct PextData {
    uint64_t mask_low;
    uint64_t mask_high;
    int shift_high;  // low側の有効ビット数 (PEXT結果の結合用)
};

PextData pext_data[input::N2];

// PEXTで圧縮されたビット列(最大9ビット=0~511)から直接結果を引くテーブル
// サイズ: 100 * 512 = 約 50 KB
uint8_t can_delete_pext[input::N2][512];

void build_pext_masks() {
    for (int x = 0; x < input::N; ++x) {
        for (int y = 0; y < input::N; ++y) {
            int cell = x * input::N + y;
            uint64_t m_low = 0;
            uint64_t m_high = 0;

            vector<int> valid_neighbors;

            // 元の get_mask9 と同じ順序（k = 0~8）で走査
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int nx = x + di;
                    int ny = y + dj;
                    if (nx >= 0 && nx < input::N && ny >= 0 && ny < input::N) {
                        int n_cell = nx * input::N + ny;
                        valid_neighbors.push_back(n_cell);

                        // 128bitビットボードの上位・下位にマスクをセット
                        if (n_cell < 64) {
                            m_low |= (1ULL << n_cell);
                        } else {
                            m_high |= (1ULL << (n_cell - 64));
                        }
                    } else {
                        // 盤面外はダミー値を記録
                        valid_neighbors.push_back(-1);
                    }
                }
            }

            pext_data[cell].mask_low = m_low;
            pext_data[cell].mask_high = m_high;
            pext_data[cell].shift_high = __builtin_popcountll(m_low);

            // --------------------------------------------------------
            // PEXTの出力結果から、直接 can_delete を引くテーブルの構築
            // --------------------------------------------------------
            int C = __builtin_popcountll(m_low) + __builtin_popcountll(m_high);  // 実際の隣接マス数

            // PEXTが出力しうる全パターン (2^C 通り) を網羅
            for (int p_mask = 0; p_mask < (1 << C); ++p_mask) {
                int mask9 = 0;
                int valid_idx = 0;  // PEXTで右詰めされたビットのインデックス

                // PEXTで圧縮された状態から、元の con9 用の 9bit マスクに復元する
                for (int k = 0; k < 9; ++k) {
                    if (valid_neighbors[k] != -1) {
                        if ((p_mask >> valid_idx) & 1) {
                            mask9 |= (1 << k);
                        }
                        valid_idx++;
                    }
                }

                // 復元した 9bit マスクで con9 を引き、PEXT用テーブルにキャッシュする
                can_delete_pext[cell][p_mask] = con9[mask9];
            }
        }
    }
}

inline bool check_can_delete_pext(Bitboard territory, int cell) {
    const auto& pd = pext_data[cell];

    // 128bit を上位・下位の 64bit に分割
    uint64_t t_low = territory & 0xFFFFFFFFFFFFFFFFULL;
    uint64_t t_high = territory >> 64;

    // _pext_u64 命令で、周囲の必要なビットだけを右詰めに抽出 (それぞれ1クロック)
    uint64_t ext_low = _pext_u64(t_low, pd.mask_low);
    uint64_t ext_high = _pext_u64(t_high, pd.mask_high);

    // 下位で抽出されたビット数分だけ上位をシフトし、結合する
    int pext_result = ext_low | (ext_high << pd.shift_high);

    // 事前計算したテーブルから直接結果を返す
    return can_delete_pext[cell][pext_result];
}

inline void init_mask9() {
#ifdef AWS_CLOUD
    init_fast_mask9();
#else
    con9 = connect9();
    build_pext_masks();
#endif
}

inline bool check_can_delete(Bitboard territory, int cell) {
#ifdef AWS_CLOUD
    return check_can_delete_shift(territory, cell);
#else
    return check_can_delete_pext(territory, cell);
#endif
}

};  // namespace FastCon9

struct GameState {
    int turn;
    Field field;
    vector<int> pos;    // player positions
    vector<int> scores;
    vector<Bitboard> player_territory; // player_territory[i] := プレイヤーiの領土を表すビットボード
    vector<Bitboard> player_reachable_territory; // player_reachable_territory[i] := プレイヤーiの到達可能な領土を表すビットボード
    vector<bool> need_update_reachable_territory;

    GameState() : turn(0) {
        pos.resize(input::M);
        scores.resize(input::M, 0);
        player_territory.resize(input::M);
        player_reachable_territory.resize(input::M);
        need_update_reachable_territory.resize(input::M, true);
        REP(i, input::M) {
            pos[i] = input::sxy[i];
            field.cells[pos[i]].owner = i;
            field.cells[pos[i]].level = 1;
            field.cells[pos[i]].update_cell_state_type();
            scores[i] += input::V[pos[i]];
            player_territory[i] |= (__uint128_t(1) << pos[i]);
            update_reachable_territory(i);
        }
    }

    Bitboard get_reachable_territory(int player_id) const {
        Bitboard current = 0;
        //current.set(pos[player_id]); // 現在地からスタート
        current |= (__uint128_t(1) << pos[player_id]);
        const Bitboard allowed = player_territory[player_id]; // 通って良いのは自分の領土のみ
        
        while (true) {
            Bitboard next = dilate(current) & allowed;
            if (next == current) break; // 変化しなくなったら完了
            current = next;
        }
        return current;
    }

    void update_reachable_territory(int player_id) {
        if (!need_update_reachable_territory[player_id]) return;
        player_reachable_territory[player_id] = get_reachable_territory(player_id);
        need_update_reachable_territory[player_id] = false;
    }

    int update_valid_moves(int player_id, array<int, input::N2>& valid_moves) const {
        // 1. 到達可能領土を計算
        const Bitboard& reachable = player_reachable_territory[player_id];
        
        // 2. 移動候補（到達可能領土 + その隣接マス）を計算
        Bitboard candidates = dilate(reachable);
        
        // 3. 敵の現在位置を除外
        Bitboard enemy_pos = 0;
        for (int p = 0; p < input::M; ++p) {
            if (p != player_id) enemy_pos |= (__uint128_t(1) << pos[p]);
        }
        candidates &= ~enemy_pos;

        uint64_t low = candidates & 0xFFFFFFFFFFFFFFFFULL;
        uint64_t high = (candidates >> 64);

        int cnt = 0;
        // __builtin_ctzllで最下位ビットの位置を取得してループ
        while (low) {
            int bit_pos = __builtin_ctzll(low);
            valid_moves[cnt++] = bit_pos;
            low &= low - 1; // 最下位ビットを消す
        }
        while (high) {
            int bit_pos = __builtin_ctzll(high);
            valid_moves[cnt++] = bit_pos + 64;
            high &= high - 1; // 最下位ビットを消す
        }

        return cnt;
    }

    void query(int txy) {
        const auto [x, y] = dec(txy);
        cout << x << " " << y << endl;
        cout.flush();
#ifndef SUBMIT
        tester->next_turn(x, y);
#endif
    }

    vector<int> get_player_moves() const {
        vector<int> moves(input::M);
        // Read tx_p, ty_p for all M players
        REP(i, input::M) {
            [[maybe_unused]] int tx, ty;
#ifndef SUBMIT
            tx = tester->state.selected[i].x;
            ty = tester->state.selected[i].y;
#else
            cin >> tx >> ty;
#endif
            moves[i] = enc(tx, ty);
        }
        return moves;
    }

    // need to call get_player_moves() before calling this function
    void update_state_from_input() {
        // Read ex_p, ey_p for all M players and update pos
        REP(i, input::M) {
            int ex, ey;
#ifndef SUBMIT
            ex = tester->state.pos[i].x;
            ey = tester->state.pos[i].y;
#else
            cin >> ex >> ey;
#endif
            pos[i] = enc(ex, ey);
        }

        // Read O_ij for all N x N cells and update field.cells[k].owner
        REP(x, input::N) {
            REP(y, input::N) {
                int owner;
#ifndef SUBMIT
                owner = tester->state.owner[x][y];
#else
                cin >> owner;
#endif
                field.cells[enc(x, y)].owner = owner;
                if (owner >= 0) {
                    for (auto& territory: player_territory) {
                        territory &= ~(__uint128_t(1) << enc(x, y));
                    }
                    player_territory[owner] |= (__uint128_t(1) << enc(x, y));
                }
            }
        }

        REP(i, input::M) {
            need_update_reachable_territory[i] = true;  // TODO?
            update_reachable_territory(i);
        }

        // Read L_ij for all N x N cells and update field.cells[k].level
        REP(x, input::N) {
            REP(y, input::N) {
                int level;
#ifndef SUBMIT
                level = tester->state.level[x][y];
#else
                cin >> level;
#endif
                field.cells[enc(x, y)].level = level;
            }
        }

        REP(i, input::N2) {
            field.cells[i].update_cell_state_type();
        }

        update_scores();
        turn += 1;
    }

    // AVX ??
    void update_scores() {
        scores.assign(input::M, 0);
        for (int i = 0; i < input::N2; ++i) {
            int owner = field.cells[i].owner;
            if (owner >= 0) {
                scores[owner] += input::V[i] * field.cells[i].level;
            }
        }
    }
};


////////////////////////////////////////////////////////////////////

struct EstimateParams {
    double wa, wb, wc, wd, eps;
    double weight; 

    EstimateParams() : weight(1.0) {
        wa = (0.3 + 1.0) / 2;
        wb = (0.3 + 1.0) / 2;
        wc = (0.3 + 1.0) / 2;
        wd = (0.3 + 1.0) / 2;
        eps = (0.1 + 0.5) / 2;
    }

    void reset_param_zero() {
        wa = 0.0;
        wb = 0.0;
        wc = 0.0;
        wd = 0.0;
        eps = 0.0;
    }

    void init_random() {
        wa = rand01() * 0.7 + 0.3; // 0.3 ~ 1.0
        wb = rand01() * 0.7 + 0.3;
        wc = rand01() * 0.7 + 0.3;
        wd = rand01() * 0.7 + 0.3;
        eps = rand01() * 0.4 + 0.1; // 0.1 ~ 0.5
        weight = 1.0;
    }

    void load_params([[maybe_unused]] int ai_idx, [[maybe_unused]] double noise) {
#ifndef SUBMIT
        wa = tester->input.wa[ai_idx];
        wb = tester->input.wb[ai_idx];
        wc = tester->input.wc[ai_idx];
        wd = tester->input.wd[ai_idx];
        eps = tester->input.eps[ai_idx];

        const double w = noise;
        wa += rand01() * 2 * w - w;
        wb += rand01() * 2 * w - w;
        wc += rand01() * 2 * w - w;
        wd += rand01() * 2 * w - w;
        eps += rand01() * 2 * w - w;

        wa = max(0.3, min(1.0, wa));
        wb = max(0.3, min(1.0, wb));
        wc = max(0.3, min(1.0, wc));
        wd = max(0.3, min(1.0, wd));
        eps = max(0.1, min(0.5, eps));

        //DEBUG("AI{} params: wa={:.3f}, wb={:.3f}, wc={:.3f}, wd={:.3f}, eps={:.3f}\n", ai_idx, wa, wb, wc, wd, eps);
#endif
    }

    void print_params([[maybe_unused]] int ai_idx) const {
#ifndef SUBMIT
        const double wba = wb / wa;
        const double wca = wc / wa;
        const double wda = wd / wa;

        double wba_diff = wba - tester->input.wb[ai_idx] / tester->input.wa[ai_idx];
        double wca_diff = wca - tester->input.wc[ai_idx] / tester->input.wa[ai_idx];
        double wda_diff = wda - tester->input.wd[ai_idx] / tester->input.wa[ai_idx];
        double eps_diff = eps - tester->input.eps[ai_idx];
        DEBUG("AI{} params val: wba={:.3f}, wca={:.3f}, wda={:.3f}, eps={:.3f}, diff: wba={:.3f}, wca={:.3f}, wda={:.3f}, eps={:.3f}\n", ai_idx, wba, wca, wda, eps, wba_diff, wca_diff, wda_diff, eps_diff);
#endif
    }
};

struct AIScoreCache {
    // scores[cell_idx][state_type]
    // state_type:
    // 0: 誰の領土でもない (wa)
    // 1: 自分の領土でレベル < U (wb)
    // 2: 他のプレイヤーの領土でレベル == 1 (wc)
    // 3: 他のプレイヤーの領土でレベル >= 2 (wd)
    array<array<double, 5>, input::N2> scores;

    void init(const EstimateParams& p) {
        for (int i = 0; i < input::N2; ++i) {
            double v = input::V[i]; 
            
            scores[i][0] = v * p.wa;
            scores[i][1] = v * p.wb;
            scores[i][2] = v * p.wc;
            scores[i][3] = v * p.wd;
            scores[i][4] = 0;
        }
    }
};

// for particle filter
void update_reachable_cells_score(int ai_id, int num_reachable_cells, const array<int, input::N2>& reachable_cells, const EstimateParams& param, const GameState& game, array<double, input::N2>& reachable_cells_score) {
    const int player_id = ai_id + 1; // AIのプレイヤーID (0は人間プレイヤー)
    const double w[5] = {param.wa, param.wb, param.wc, param.wd, 0.0};
    for (int i = 0; i < num_reachable_cells; ++i) {
        int cell_idx = reachable_cells[i];
        const CellInfo& cell = game.field.cells[cell_idx];
        reachable_cells_score[i] = input::V[cell_idx] * w[cell.cell_type[player_id]];
    }
}

// for particle filter
// return num of best_moves, and fill best_moves_idx with the indices of the best moves
int get_best_moves(int num_reachable_cells, const array<double, input::N2>& reachable_cells_score, array<int, input::N2>& best_cells_idx) {
    double max_score = -1;
    for (int i = 0; i < num_reachable_cells; ++i) {
        chmax(max_score, reachable_cells_score[i]);
    }

    int best_cells_count = 0;
    const double tolerance = 1e-9 * max(abs(max_score), 1.0);
    for (int i = 0; i < num_reachable_cells; ++i) {
        if (reachable_cells_score[i] > max_score - tolerance) {
            best_cells_idx[best_cells_count++] = i;
        }
    }
    //return vector<int>(best_cells_idx.begin(), best_cells_idx.begin() + best_cells_count);
    return best_cells_count;
}
// return size is same as num_reachable_cells, and values are probabilities corresponding to reachable_cells
void update_reachable_cells_prob(int num_reachable_cells, int num_best_moves, const array<int, input::N2>& best_moves_idx, const EstimateParams& param, array<double, input::N2>& cells_probs) {
    for (int i = 0; i < num_reachable_cells; ++i) {
        cells_probs[i] = param.eps / num_reachable_cells;
    }

    for (int i = 0; i < num_best_moves; ++i) {
        cells_probs[best_moves_idx[i]] += (1.0 - param.eps) / num_best_moves;
    }
}

double compute_likelihood(int ai_id, int observed_move, int num_valid_moves, const array<int, input::N2>& valid_moves, const EstimateParams& param, const GameState& game) {
    array<double, input::N2> valid_moves_score;
    update_reachable_cells_score(ai_id, num_valid_moves, valid_moves, param, game, valid_moves_score);
    array<int, input::N2> best_moves_idx;
    const int num_best_moves = get_best_moves(num_valid_moves, valid_moves_score, best_moves_idx);
    array<double, input::N2> probs;
    update_reachable_cells_prob(num_valid_moves, num_best_moves, best_moves_idx, param, probs);
    REP(i, num_valid_moves) {
        if (valid_moves[i] == observed_move) {
            return probs[i];
        }
    }
    ASSERT(false, "Observed move {} is not in valid moves for AI{}. valid_moves = {}", observed_move, ai_id, valid_moves);
    return 0.0;
}

////////////////////////////////////////////////////////////////////

struct TurnAnalysis {
    // 1. ジオメトリ計算結果 (全員共通)
    // プレイヤーごとの到達可能マスとその数
    array<int, input::M_MAX> num_valid_moves;
    array<array<int, input::N2>, input::M_MAX> valid_moves;

    // 2. 推定平均パラメータに基づく評価 (AIのみ)
    // successive_halving のルートノード展開などで使用
    array<int, input::M_MAX> num_best_moves;
    array<array<int, input::N2>, input::M_MAX> best_moves_idx;

    // 初期化・計算関数
    void compute(const GameState& game, const vector<AIScoreCache>& caches) {
        // 自分(0) と AI(1~M-1) の有効な移動先を計算
        for (int i = 0; i < input::M; ++i) {
            num_valid_moves[i] = game.update_valid_moves(i, valid_moves[i]);
        }

        // AI(1~M-1) について、平均パラメータを用いたスコアと最善手を計算
        for (int i = 1; i < input::M; ++i) {
            // 最善手の抽出
            calc_best_moves_from_cache(i, caches[i - 1], game);
        }
    }

    void calc_best_moves_from_cache(int player_id, const AIScoreCache& cache, const GameState& game) {
        double max_score = -1.0;
        int count = 0;

        for (int i = 0; i < num_valid_moves[player_id]; ++i) {
            const int& cell_idx = valid_moves[player_id][i];
            const auto& cell_type = game.field.cells[cell_idx].cell_type[player_id];
            const double score = cache.scores[cell_idx][cell_type];

            // 2. 最大値の判定と更新
            // 浮動小数点の誤差吸収用
            double tolerance = 1e-9 * std::max(std::abs(max_score), 1.0);

            if (score > max_score + tolerance) {
                // 明確に最大値が更新された場合、リストをリセット
                max_score = score;
                best_moves_idx[player_id][0] = i;
                count = 1;
            } else if (score >= max_score - tolerance) {
                // 最大値と同等（許容誤差内）の場合、リストに追加
                best_moves_idx[player_id][count++] = i;
                // 念のため max_score を厳密な値に更新しておく
                if (score > max_score) max_score = score;
            }
        }

        num_best_moves[player_id] = count;
    }
};

////////////////////////////////////////////////////////////////////

struct ActionHistory {
    int observed_move;
    int num_valid_moves;
    array<int, input::N2> valid_moves;
    array<uint8_t, input::N2> cell_types;
};

struct AIEstimator {
    int ai_id;
    int num_particles;
    vector<EstimateParams> particles;
    vector<ActionHistory> history;

    AIEstimator(int id, int n) : ai_id(id), num_particles(n) {
        particles.resize(num_particles);
        for (auto& p : particles) p.init_random();
        for (auto& p : particles) p.weight = 1.0 / num_particles;
    }

    EstimateParams sample_params() {
        return sample(particles);   // assume weight is normalized
    }

    void update(int observed_move, int n_valid_moves, const array<int, input::N2>& valid_moves, const GameState& game_before_move) {
        ActionHistory h;
        h.observed_move = observed_move;
        h.num_valid_moves = n_valid_moves;
        h.valid_moves = valid_moves;
        
        // 合法手マスについてだけ、そのターンの cell_type を記録しておく
        const int player_id = ai_id + 1;
        for (int i = 0; i < n_valid_moves; ++i) {
            int cell_idx = valid_moves[i];
            h.cell_types[cell_idx] = game_before_move.field.cells[cell_idx].cell_type[player_id];
        }
        history.push_back(h);

        double weight_sum = 0.0;
        
        // 1. 各パーティクルの尤度を計算して重みを更新
        for (auto& p : particles) {
            double likelihood = compute_likelihood(ai_id, observed_move, n_valid_moves, valid_moves, p, game_before_move);
            p.weight *= likelihood;
            weight_sum += p.weight;
        }

        // 2. 正規化
        for (auto& p : particles) {
            p.weight /= weight_sum;
        }

        resample(game_before_move.turn);
        //normalize_weights();
    }

    void resample(int turn) {
        vector<EstimateParams> next_particles;
        next_particles.reserve(num_particles);

        vector<double> weights(num_particles);
        double w_sum = 0.0;
        for (int i = 0; i < num_particles; ++i) {
            weights[i] = particles[i].weight;
            w_sum += weights[i];
        }

        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        const double noise_std = get_linear_interpolate(1.0 * turn / input::T, 0.05, 0.01);
        std::normal_distribution<double> gaussian(0.0, noise_std);

        for (int i = 0; i < num_particles; ++i) {
            int idx = dist(engine);
            EstimateParams p = particles[idx];  // 元のパーティクル

            // 提案分布: ノイズを加えた候補 p_prime を作成
            EstimateParams p_prime = p;
            p_prime.wa = max(0.3, min(1.0, p_prime.wa + gaussian(engine)));
            p_prime.wb = max(0.3, min(1.0, p_prime.wb + gaussian(engine)));
            p_prime.wc = max(0.3, min(1.0, p_prime.wc + gaussian(engine)));
            p_prime.wd = max(0.3, min(1.0, p_prime.wd + gaussian(engine)));
            p_prime.eps = max(0.1, min(0.5, p_prime.eps + gaussian(engine)));

            // 履歴全体の対数尤度を比較
            double log_L_p = compute_history_log_likelihood(p);
            double log_L_p_prime = compute_history_log_likelihood(p_prime);

            // Accept / Reject 判定
            double diff = log_L_p_prime - log_L_p;

            if (diff >= 0.0 || rand01() < std::exp(diff)) {
                // Accept: 候補 p_prime を採用
                next_particles.push_back(p_prime);
            } else {
                // Reject: 元の p を維持
                next_particles.push_back(p);
            }
        }

        // 4. すべてのパーティクルの重みを平坦にリセットする
        for (auto& p : next_particles) {
            p.weight = 1.0 / num_particles;
        }

        particles = std::move(next_particles);
    }

    double compute_history_log_likelihood(const EstimateParams& p) const {
        double log_L = 0.0;
        const double w[5] = {p.wa, p.wb, p.wc, p.wd, 0.0};

        for (const auto& h : history) {
            if (h.num_valid_moves == 0) continue;

            // 1. まず「実際に観測された手」のスコアを計算する
            const double observed_score = input::V[h.observed_move] * w[h.cell_types[h.observed_move]];

            // Vもwも非負なのでスコアは常に非負。std::abs は不要。
            const double tolerance = 1e-9 * std::max(observed_score, 1.0);

            int best_moves_count = 0;
            bool is_best = true;

            // 2. 短絡評価（Short-circuit）による1パス計算
            for (int i = 0; i < h.num_valid_moves; ++i) {
                const int cell = h.valid_moves[i];
                const double s = input::V[cell] * w[h.cell_types[cell]];

                if (s > observed_score + tolerance) {
                    // 観測された手よりも「明らかに良い手」が1つでも見つかったら、
                    // 観測された手は最善手ではないことが確定するため、ループを即座に抜ける。
                    is_best = false;
                    break;
                }

                // 観測された手と同等のスコアを持つ手をカウント
                if (s >= observed_score - tolerance) {
                    best_moves_count++;
                }
            }

            // 3. 確率の計算
            double prob = p.eps / h.num_valid_moves;
            if (is_best) {
                prob += (1.0 - p.eps) / best_moves_count;
            }

            // 4. 対数尤度の加算
            log_L += std::log(prob);
        }

        return log_L;
    }

    // 正規化
    void normalize_weights() {
        double weight_sum = 0.0;
        for (const auto& p : particles) {
            weight_sum += p.weight;
        }
        for (auto& p : particles) {
            p.weight /= weight_sum;
        }
    }

    // シミュレーションで使うための代表値（重み付き平均など）を取得する関数
    EstimateParams get_mean_params() const { 
        EstimateParams mean;
        mean.reset_param_zero();
        for (const auto& p : particles) {
            mean.wa += p.wa * p.weight;
            mean.wb += p.wb * p.weight;
            mean.wc += p.wc * p.weight;
            mean.wd += p.wd * p.weight;
            mean.eps += p.eps * p.weight;
        }
        return mean;
    }
};

////////////////////////////////////////////////////////////////////

struct Sim {
    GameState state;
    const vector<EstimateParams>& params;

    Sim(const GameState& initial_state, const vector<EstimateParams>& initial_params) : state(initial_state), params(initial_params) {}

    int decide_ai_move(int ai_idx, bool deterministic, int num_reachable_cells, const array<int, input::N2>& reachable_cells, int num_best_moves, const array<int, input::N2>& best_moves_idx) {
        if (deterministic || rand01() >= params[ai_idx].eps) {
            ASSERT(num_best_moves > 0, "No best moves for AI{}! This should not happen. num_reachable_cells = {}", ai_idx, num_reachable_cells);
            const int selected_idx = best_moves_idx[rand_int(0, num_best_moves)];
            return reachable_cells[selected_idx];
        } else {
            //return sample(reachable_cells);
            ASSERT(num_reachable_cells > 0, "No reachable cells for AI{}! This should not happen.", ai_idx);
            return reachable_cells[rand_int(0, num_reachable_cells)];
        }
    }

    void simulate_turn(int txy, const TurnAnalysis& analysis) {
        array<int, input::M_MAX> moves;

        moves[0] = txy; 
        for (int i = 1; i < input::M; ++i) {
            const bool deterministic = false;
            moves[i] = decide_ai_move(i - 1, deterministic, analysis.num_valid_moves[i], analysis.valid_moves[i],
                                         analysis.num_best_moves[i], analysis.best_moves_idx[i]);
        }

        // ステップ2: 駒のカウント
        static vector<int> move_counts(input::N2, 0);
        for (int i = 0; i < input::M; ++i) {
            move_counts[moves[i]]++;
        }

        // ステップ3: 衝突
        bitset<input::M_MAX> collected = 0;
        for (int i = 0; i < input::M; ++i) {
            if (move_counts[moves[i]] >= 2) {
                if (i != state.field.cells[moves[i]].owner) collected.set(i);
            }
        }

        // ステップ4: 行動処理
        for (int i = 0; i < input::M; ++i) {
            if (collected[i]) continue;
            const int owner = state.field.cells[moves[i]].owner;
            const int v = input::V[moves[i]];

            if (owner == -1) {
                state.field.cells[moves[i]].owner = i;
                state.field.cells[moves[i]].level = 1;
                state.player_territory[i] |= (__uint128_t(1) << moves[i]);
                state.player_reachable_territory[i] |= (__uint128_t(1) << moves[i]);
                if (state.player_territory[i] != state.player_reachable_territory[i]) {
                    state.need_update_reachable_territory[i] = true;    // 分断状態から復帰した場合
                }
                state.scores[i] += v;
            } else if (owner == i) {
                if (state.field.cells[moves[i]].level < input::U) {
                    state.field.cells[moves[i]].level++;
                    state.scores[i] += v;
                }
            } else {
                state.field.cells[moves[i]].level--;
                state.scores[owner] -= v;
                if (state.field.cells[moves[i]].level == 0) {
                    state.field.cells[moves[i]].owner = i;
                    state.field.cells[moves[i]].level = 1;
                    state.player_territory[owner] &= ~(__uint128_t(1) << moves[i]);
                    state.need_update_reachable_territory[owner] = true;
                    state.player_territory[i] |= (__uint128_t(1) << moves[i]);
                    state.player_reachable_territory[i] |= (__uint128_t(1) << moves[i]);
                    if (state.player_territory[i] != state.player_reachable_territory[i]) {
                        state.need_update_reachable_territory[i] = true;    // 分断状態から復帰した場合
                    }
                    state.scores[i] += v;
                } else {
                    collected[i] = true;
                }
            }

            state.field.cells[moves[i]].update_cell_state_type();
        }

        // ステップ5: 回収
        for (int i = 0; i < input::M; ++i) {
            if (collected[i]) continue;
            else state.pos[i] = moves[i];
        }

        REP(i, input::M) {
            state.update_reachable_territory(i);
        }

        // reset move_counts for next turn
        for (int i = 0; i < input::M; ++i) {
            move_counts[moves[i]] = 0;
        }

        state.turn += 1;
    }
};

////////////////////////////////////////////////////////////////////

int get_manhattan_distance(int cell1, int cell2) {
    auto [x1, y1] = dec(cell1);
    auto [x2, y2] = dec(cell2);
    return abs(x1 - x2) + abs(y1 - y2);
}

void print_reachable_cells(const vector<int>& cells) {
    vector<pair<int, int>> coords(cells.size());
    REP(i, cells.size()) {
        coords[i] = dec(cells[i]);
    }
    DEBUG("reachable_cells: {}\n", coords);
}

int get_next_move_greedy(const GameState& game,
                         const TurnAnalysis& analysis,
                         const vector<EstimateParams>& params) {

    const double epsilon = get_linear_interpolate(game.turn / (double)input::T, 0.25, 0.0);
    if (rand01() < epsilon) {
        int random_cell = analysis.valid_moves[0][rand_int(0, analysis.num_valid_moves[0])];
        //DEBUG("Randomly selected cell: {}\n", dec(random_cell));
        return random_cell;
    }

    int target_cell = -1;
    double max_score = -1;

    int best_ai_score = *max_element(game.scores.begin() + 1, game.scores.end());

    array<double, input::N2> ai_not_select_probs;
    ai_not_select_probs.fill(1.0);

    for (int ai_idx = 0; ai_idx < input::M - 1; ++ai_idx) {
        int player_id = ai_idx + 1;
        int num_moves = analysis.num_valid_moves[player_id];
        int num_best = analysis.num_best_moves[player_id];

        if (num_moves == 0) continue;

        double prob_base = params[ai_idx].eps / num_moves;
        double prob_best = prob_base + (1.0 - params[ai_idx].eps) / num_best;

        // 1. まず全ての有効手に対して、ランダム行動分の非選択確率を掛ける
        for (int i = 0; i < num_moves; ++i) {
            int cell = analysis.valid_moves[player_id][i];
            ai_not_select_probs[cell] *= (1.0 - prob_base);
        }

        // 2. 最善手に対してのみ、確率の差分を補正する
        // (1 - prob_best) にしたいので、掛けすぎてしまった (1 - prob_base) で割り戻す
        double correction = (1.0 - prob_best) / (1.0 - prob_base);
        for (int i = 0; i < num_best; ++i) {
            int cell_idx = analysis.best_moves_idx[player_id][i];
            int cell = analysis.valid_moves[player_id][cell_idx];
            ai_not_select_probs[cell] *= correction;
        }
    }

    REP(i, analysis.num_valid_moves[0]) {
        const int& cell = analysis.valid_moves[0][i];
        auto next_player_score = game.scores[0];
        double next_best_ai_score = best_ai_score;
        if (game.field.cells[cell].owner == 0) {
            // topのAIほど大きくする？
            const double ai_select_bonus = get_linear_interpolate(ai_not_select_probs[cell], 8.0, 0.6);

            const bool can_delete = FastCon9::check_can_delete_pext(game.player_territory[game.field.cells[cell].owner], cell); 
            const double delete_score = can_delete ? 1.00 : 2.5;
            const double delete_bonus = ((game.field.cells[cell].level == 1) ? delete_score : 1.0);

            next_player_score += delete_bonus * ai_select_bonus * input::V[cell] * (game.field.cells[cell].level < input::U);
        } else if (game.field.cells[cell].owner == -1) {
            const double expand_bonus = get_linear_interpolate(game.turn / (double)input::T, 2.0, 1.0);
            next_player_score += expand_bonus * input::V[cell] * ai_not_select_probs[cell];
        } else {
            const bool can_delete = FastCon9::check_can_delete_pext(game.player_territory[game.field.cells[cell].owner], cell); 
            const double delete_bonus = can_delete ? 1.00 : 1.25;
            next_player_score += delete_bonus * ai_not_select_probs[cell] * input::V[cell] * sqrt(1.0 / game.field.cells[cell].level);
            next_best_ai_score = -1.0;
            for (int i = 1; i < input::M; ++i) {
                double expected_ai_score = game.scores[i];
                if (i == game.field.cells[cell].owner) {
                    expected_ai_score -= ai_not_select_probs[cell] * input::V[cell]; // 攻撃対象のAIだけスコアを引く
                }
                chmax(next_best_ai_score, expected_ai_score);
            }
        }
        const double score = next_player_score / next_best_ai_score;
        //DEBUG("cell : {}, score: {:.3f}\n", dec(cell), score);

        if (chmax(max_score, score)) {
            target_cell = cell;
        }
    }
    return target_cell;
}

struct MoveStat {
    int move;
    double mean;
    double var;
    int count;

    static const int K = 10;
    SortedList<double, less<double>> bottom_scores;
    double bottom_mean;

    MoveStat() : move(-1), mean(0), var(0), count(0), bottom_scores(K), bottom_mean(0) {}

    // Welford's algorithm
    void add(double x) {
        count++;
        double delta = x - mean;
        mean += delta / count;
        var += delta * (x - mean);
        if (bottom_scores.can_insert(x)) bottom_scores.insert(x);
    }

    double get_mean() const {
        return mean;
    }

    double get_variance() const {
        return count > 1 ? var / (count - 1) : 0.0;
    }

    void update_bottom_mean() {
        const auto vec = bottom_scores.get_list();
        if (vec.empty()) return;
        double sum = 0.0;
        for (const auto score : vec) {
            sum += score;
            bottom_scores.insert(score);    // restore
        }
        int n = vec.size();
        bottom_mean = sum / n;
    }

    double get_bottom_mean() const {
        return bottom_mean;
    }
};

int n_turn_simulation;  // for debug

int get_next_move_successive_halving(
    const GameState& current_game, 
    const TurnAnalysis& analysis,
    double end_time,
    vector<AIEstimator>& estimators
) {
    int num_valid_moves = analysis.num_valid_moves[0];
    const auto& valid_moves = analysis.valid_moves[0];

    array<int, input::N2> filtered_moves;
    int num_filtered_moves = 0;
    for (int i = 0; i < num_valid_moves; ++i) {
        int cell = valid_moves[i];
        // レベルMAXの自領土など、探索不要な手を除く処理
        if (current_game.field.cells[cell].owner == 0 && current_game.field.cells[cell].level == input::U) continue;
        filtered_moves[num_filtered_moves++] = cell;
    }
    // 全て除外されてしまった場合は元に戻す
    if (num_filtered_moves == 0) {
        for (int i = 0; i < num_valid_moves; ++i) filtered_moves[i] = valid_moves[i];
        num_filtered_moves = num_valid_moves;
    }

    // 候補が1つしかない場合は探索せずに即座に返す
    if (num_filtered_moves == 1) return filtered_moves[0];

    vector<MoveStat> active_moves(num_filtered_moves);
    for (int i = 0; i < num_filtered_moves; ++i) {
        active_moves[i].move = filtered_moves[i];
    }

    const int init_turn = current_game.turn;
    const int N_TURN = get_linear_interpolate(1.0 * init_turn / input::T, 23, 10);

    double start_time = timer.get_time();
    double total_allocated_time = end_time - start_time;

    // ラウンド数の計算 (要素数が1になるまで半分にしていく回数)
    const int K = num_filtered_moves;
    int total_rounds = 0;
    int temp_k = K;
    while (temp_k > 1) {
        total_rounds++;
        temp_k = (temp_k + 1) / 2;
    }

    TurnAnalysis step_analysis;
    vector<AIScoreCache> score_caches(input::M - 1);
    vector<EstimateParams> params(input::M - 1);
    //REP(i, input::M - 1) {
    //    estimators[i].setup_dist();
    //}

    // Successive Halving のメインループ
    for (int round = 1; round <= total_rounds; ++round) {
        int num_active = active_moves.size();
        if (num_active == 1) break;

        // このラウンドの終了時刻を計算（全ラウンドに均等に時間を割り当て）
        double round_time_budget = total_allocated_time / total_rounds;
        double round_end_time = start_time + round_time_budget * round;

        int iteration = 0;
        // 時間が許す限り、残っている全候補に「均等に」シミュレーションを割り振る
        while (true) {
#ifdef FIX_LOOP
            if (iteration >= (1 << (1 + round))) break;
#else
            if (timer.get_time() >= round_end_time) break;
#endif

            // fix rand
            Randxor iteration_rand;
            iteration_rand.x = randxor.get();
            iteration_rand.y = randxor.get();
            iteration_rand.z = randxor.get();
            iteration_rand.w = randxor.get();

            for (int i = 0; i < input::M - 1; ++i) {
                params[i] = estimators[i].sample_params();

                //const double noise = 0.0;
                //params[i].load_params(i, noise);   // cheating for debug

                score_caches[i].init(params[i]);
            }

            TurnAnalysis root_analysis = analysis; 
            for (int i = 1; i < input::M; ++i) {
                root_analysis.calc_best_moves_from_cache(i, score_caches[i - 1], current_game);
            }

            const auto randxor_backup = randxor;
            for (int i = 0; i < num_active; ++i) {
                randxor = iteration_rand;

                int move = active_moves[i].move;

                // --- 1回のプレイアウト処理 ---
                Sim sim(current_game, params);

                sim.simulate_turn(move, root_analysis);

                while (sim.state.turn < input::T && sim.state.turn < init_turn + N_TURN) {
                    step_analysis.compute(sim.state, score_caches);
                    int greedy_move = get_next_move_greedy(sim.state, step_analysis, params);
                    sim.simulate_turn(greedy_move, step_analysis);
                }

                double s0 = sim.state.scores[0];
                double sA = 0;
                for (int p = 1; p < input::M; ++p) {
                    chmax(sA, (double)sim.state.scores[p]);
                }

                double eval_score = s0 / sA;

                // --------------------------------------------------------

                active_moves[i].add(eval_score);

                n_turn_simulation += 1;
            }

            iteration += 1;
            randxor = randxor_backup; // restore
        }

        auto get_eval = [](const MoveStat& m) {
            //return m.get_mean();
            return m.get_mean() + 0.05 * m.get_bottom_mean();
        };

        for (auto& m : active_moves) {
            m.update_bottom_mean();
        }

        // ラウンド終了後、平均スコアの降順にソートして下位半分を切り捨てる
        sort(active_moves.begin(), active_moves.end(), [&](const MoveStat& a, const MoveStat& b) {
            return get_eval(a) > get_eval(b);
        });

        // デバッグ用: ラウンドごとの絞り込み状況を確認できます
        DEBUG("--- Round {} ---\n", round);
        for(auto& m : active_moves) {
            DEBUG("Move: {}, Visits: {}, Avg: {:.3f}, Std: {:.3f}, BotAvg: {:.3f}, Eval: {:.3f}\n", dec(m.move), m.count, m.get_mean(), sqrt(m.get_variance()), m.get_bottom_mean(), get_eval(m));
        }

        int next_k = (num_active + 1) / 2; // 切り上げで上位半分を残す
        active_moves.resize(next_k);
    }

    // DEBUG("--- Successive Halving Selected Move: {}, Avg Eval: {:.3f} ---\n", dec(active_moves[0].move), active_moves[0].get_avg());
    return active_moves[0].move;
}

void solve(const double end_time) {
    GameState game;

    const int num_particles = 150;
    vector<AIEstimator> ai_estimators;
    for (int i = 0; i < input::M - 1; ++i) {
        ai_estimators.emplace_back(i, num_particles);
    }

#ifndef SUBMIT
    vector<int> n_greedy_moves(input::M-1, 0);
    vector<int> n_expected_moves(input::M-1, 0);
#endif

    TurnAnalysis analysis;

    for (int t = 0; t < input::T; t++) {
        DEBUG("--- Turn {} ---\n", t);
        vector<EstimateParams> params(input::M-1); // AIの推定パラメータ
        for (int i = 0; i < input::M - 1; ++i) {
            params[i] = ai_estimators[i].get_mean_params(); // TODO: delete
        }

        vector<AIScoreCache> score_caches(input::M - 1);
        for (int i = 0; i < input::M - 1; ++i) {
            score_caches[i].init(params[i]);
        }

        analysis.compute(game, score_caches);

        const double time_limit = end_time / input::T * (t + 1);
        const int target_cell = get_next_move_successive_halving(game, analysis, time_limit, ai_estimators);

        // greedy
        //vector<vector<int>> reachable_cells_all(input::M);
        //vector<vector<double>> reachable_cells_score_all(input::M);
        //vector<vector<int>> best_moves_idx_all(input::M);
        //reachable_cells_all[0] = game.get_valid_moves(0);
        //REP(ai_idx, input::M - 1) {
        //    reachable_cells_all[ai_idx + 1] = game.get_valid_moves(ai_idx + 1);
        //    reachable_cells_score_all[ai_idx + 1] = get_reachable_cells_score(ai_idx, reachable_cells_all[ai_idx + 1], params[ai_idx], game);
        //    best_moves_idx_all[ai_idx + 1] = get_best_moves(reachable_cells_score_all[ai_idx + 1]);
        //}
        //const int target_cell = get_next_move_greedy(game, reachable_cells_all, reachable_cells_score_all, best_moves_idx_all, params);

        // Issue the query to move to the selected cell
        game.query(target_cell);
        const auto moves = game.get_player_moves();

        for (int ai_idx = 0; ai_idx < input::M - 1; ++ai_idx) {
            const int player_id = ai_idx + 1;
            const int observed_move = moves[player_id];
            ai_estimators[ai_idx].update(observed_move, analysis.num_valid_moves[player_id], analysis.valid_moves[player_id], game);

#ifndef SUBMIT
            // check
            array<int, input::N2> reachable_cells;
            const auto num_reachable_cells = game.update_valid_moves(ai_idx + 1, reachable_cells);
            EstimateParams tester_params;
            tester_params.load_params(ai_idx, 0.0);
            array<double, input::N2> reachable_cells_score;
            update_reachable_cells_score(ai_idx, num_reachable_cells, reachable_cells, tester_params, game, reachable_cells_score);
            const int best_idx = max_element(reachable_cells_score.begin(), reachable_cells_score.begin() + num_reachable_cells) - reachable_cells_score.begin();
            const int expected_move = reachable_cells[best_idx];
            if (expected_move == observed_move) {
                n_greedy_moves[ai_idx]++;
                array<double, input::N2> reachable_cells_score_trained_param;
                update_reachable_cells_score(ai_idx, num_reachable_cells, reachable_cells, params[ai_idx], game, reachable_cells_score_trained_param);
                const int expected_best_idx = max_element(reachable_cells_score_trained_param.begin(), reachable_cells_score_trained_param.begin() + num_reachable_cells) - reachable_cells_score_trained_param.begin();
                if (expected_best_idx == best_idx) {
                    n_expected_moves[ai_idx]++;
                    DEBUG("Turn {}: AI{} expected move matches tester's expected move. expected_best_move={}\n", t, ai_idx, dec(reachable_cells[expected_best_idx]));
                } else {
                    DEBUG("Turn {}: AI{} expected move differs from tester's expected move. expected_best_move={}, observed_move={}\n", t, ai_idx, dec(reachable_cells[expected_best_idx]), dec(observed_move));
                }
            }
#endif
        }

        game.update_state_from_input();
        REP(i, input::M) {
            game.update_reachable_territory(i);
        }
    }

    REP(i, input::M-1) {
        DEBUG("AI{} final estimated params:\n", i);
        // weightの大きい順に上位K粒子のパラメータを表示
        const int K = 3;
        vector<pair<double, EstimateParams>> weighted_particles;
        for (const auto& p : ai_estimators[i].particles) {
            weighted_particles.emplace_back(p.weight, p);
        }
        sort(weighted_particles.rbegin(), weighted_particles.rend(), [](const auto& a, const auto& b) {
            return a.first < b.first; // weightの降順
        });
        for (int k = 0; k < K && k < (int)weighted_particles.size(); ++k) {
            DEBUG("  Particle {}: weight = {:.3f}\n", k, weighted_particles[k].first);
            weighted_particles[k].second.print_params(i);
        }
    }

#ifndef SUBMIT
    REP(i, input::M-1) {
        DEBUG("AI{} expected move accuracy: {}/{} = {:.3f}%\n", i, n_expected_moves[i], n_greedy_moves[i], 100.0 * n_expected_moves[i] / n_greedy_moves[i]);
    }
#endif
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    engine = mt19937(SEED);

    //read_args(argc, argv);

    input::read_input();
#ifndef SUBMIT
    TESTER::prepare_input();
    tester = make_unique<TESTER::Tester>();
#endif

    init_bitboards();
    build_di();
    build_adjs();
    FastCon9::init_mask9();

    solve(Env::time_limit);

#ifndef SUBMIT
    DEBUG("Internal Tester Score = {}\n", TESTER::compute_score(tester->input, tester->state));
#endif

    DEBUG("n_turn_simulation: {}\n", n_turn_simulation);

    const double time = timer.get_time();
    DEBUG("time: {:.3f} [s]\n", time);
    //assert(time < 2.0);
    return 0;
}
