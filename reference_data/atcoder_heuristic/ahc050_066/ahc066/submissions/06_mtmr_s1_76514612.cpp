/*
 * AHC - Macro Controller
 *
 * 解法概略
 * --------
 * 基本方針は、「どの順序でボールを拾うか」と「登録するマクロ本体」を焼きなましで最適化する。
 * ボールを拾ったら対応するかごまで運ぶことは固定し、順序と移動経路だけを最適化する。
 *
 * 探索:
 *   ボールを処理する順序を焼きなましする。
 *   マクロ本体は複数系統を独立に持ち、低確率でどれか1系統だけ変更する。
 *   順序近傍の評価では、各系統のマクロで同じ順序を評価し、そのうち最良の値を使う。
 *   固定された順序に対して、各ボール処理後の向きを DP で最小化する。
 *   順序近傍では prefix DP を保持し、変わった位置以降だけ再評価する。
 *   近傍は swap/reverse/insert に加え、短い連続区間を別位置へ動かす block insert も使う。
 *   block insert は、地域的にまとまったボール列をまとめて移動する Or-opt 風の操作。
 *   swap は一部、壁あり距離で見た長い接続の前後を優先して選び、無駄な移動を壊しやすくする。
 *   これにより探索内容はほぼそのまま、同じ時間で回せる近傍数を増やす。
 *
 *   マクロ本体の変更は全体評価が重いので、変更候補をそのまま全順序で評価しない。
 *   まず現在の復元経路から「移動だけ」の代表区間を少数抽出し、
 *   その区間だけを対象に短い内側焼きなましを行ってマクロ候補を作る。
 *   その候補だけを従来通り全体DPで評価し、外側焼きなましの受理判定にかける。
 *   つまり、サンプル区間評価は候補生成を速くするための近似であり、
 *   最終的な採択基準は必ず全体評価にする。
 *
 * 各遷移:
 *   直前のかご、または初期位置から、次のボール k を処理する。
 *   現在位置 -> ボール位置 -> かご位置 の最短ボタン列を、向き込みで評価する。
 *   S で拾い、S で置く。
 *
 * マクロ付き最短経路:
 *   状態を (位置, 向き) とし、F/R/L/P の辺で層BFSする。
 *   P は1ボタンでマクロ本体を展開実行し、展開後の基本操作数は phys として別に持つ。
 *   全辺のボタン数は +1 なので、btn は BFS の深さで確定し、同じ深さでは phys を最小化する。
 *
 * マクロ:
 *   初期候補は、自然な移動区間の F/R/L 最短路プールから部分列を複数切り出して作る。
 *   それらを同じ経路プール由来のサンプルで軽量評価し、上位だけを full 初期評価へ回す。
 *   slot 初期化では、可能なら異なる区間由来の候補を選び、最初から系統を分ける。
 *   焼きなまし中は F/R/L からなるマクロ本体へ変異させる。
 *   長すぎる連続回転は同じ向きになる短い表現へ正規化する。
 *   変異時は、現在の復元経路から移動区間をサンプルし、
 *   その区間だけを軽量評価する内側SAで候補マクロを数本作る。
 *   最終的な採択は、必ず全体DP評価で判定する。
 *
 *   探索上は P を最初から使えるものとして経路を作る。
 *   ただし実際の出力では、最初に P を使う場所を M <macro body> M に置き換え、
 *   その場で同じ動きをしながら登録する。以後の P は通常の再生として使う。
 *
 *   さらに、マクロ登録そのものを短くするために、小さい F 連打マクロを先に登録して、
 *   <macro body> の中の F 連打を P で圧縮する候補も試す。
 *   すでに最初の P より前に同じ F 連打が出ているなら、その部分を M F...F M に
 *   置き換えるだけなので実際の移動は変わらない。
 *   それとは別に、冒頭で F 連打を登録して実際に進んだ状態から経路を作り直す候補や、
 *   冒頭で右を向いてから F 連打を登録・実行し、その向きで進んだ状態から
 *   経路を作り直す候補も比較する。
 *   さらに F 連打だけについては、左上で上向きに空撃ちして元状態へ戻す安全な候補も試す。
 *   評価中の登録コストは、初手区間 (開始位置 -> 最初のボール -> 最初のかご) に限って
 *   最初の P より前に自然に現れる F 連打を再利用できる場合はそのぶんだけ見込む。
 *   それ以外の区間については先行 run に期待せず、必ず実現できる登録方法を基準にする。
 *
 *   あるマクロ本体に対して、状態 (位置, 向き) 間の最短ボタン列を層BFSで求める。
 *   辺は F/R/L/P で、P はマクロを一括実行する 1 ボタンとして扱う。
 *
 * 出力:
 *   1本の焼きなましで最良順序と最良マクロ本体を選び、DP 復元した P 付きボタン列の
 *   最初の P を M <macro body> M に差し替えて出力する。
 *   探索中は、マクロ本体が異なるSA評価上位候補と best 更新履歴を保持する。
 *   最後の後処理は最終best 1件だけでなく、まず評価ランキング上位、その後に履歴候補の順で実行する。
 *   これにより、SA評価では少し劣っていても、登録圧縮や S 付きマクロ化で逆転する候補を拾う。
 *   最終復元では通常登録、F 連打 / R + F 連打の圧縮登録、冒頭で進んでから作り直す登録を比較し、
 *   一番短い実出力を採用する。
 *   仕上げとして、登録マクロの先頭、末尾、または両端に S を付ける候補も試す。
 *   先頭S候補では、元経路の S の後に回転だけを挟んで P する箇所を P 先頭の S に吸収する。
 *   末尾S候補では、元経路の P の後に回転だけを挟んで S する箇所を P 末尾の S に吸収する。
 *   両端S候補では、S + body + S を登録し、P ごとに先頭S/末尾Sの盤面作用をまとめて補償する。
 *   特に PP や P[LR]*P では、前の P 末尾Sと次の P 先頭Sが同じマスで連続するため、
 *   2回の S として相殺できる。この境界相殺は preS/postS を別々に適用するだけでは拾えない。
 *   単独 P を使う箇所は、追加した S が盤面を変える場合だけ反対側に S を追加して相殺する。
 *   正しい箱の上にあるボールも盤面上に残っているものとして扱い、そこで P する場合は
 *   相殺用の S を追加する。短くなる候補だけを採用する。
 *   さらに、途中から S 付きマクロへ切り替える候補も比較する。
 *   通常マクロから先頭S/末尾S/両端Sマクロへ切り替える場合は、
 *   その場の P を実行しながら M S P M / M P S M / M S P S M で新マクロを登録する。
 *   必要なら後続で同じ形を使って通常マクロへ戻し、切り替え区間だけ S 付きマクロを使う。
 *   stage1/2 後に選ばれた最良候補1件だけ、まず順序全体を小区間に分けて
 *   固定マクロの exact DP で一通り並べ替える。
 *   その後は時間が残る限り、ランダムに選んだ小区間を同じ exact DP で繰り返し最適化する。
 *   時間切れ時は、その時点までに得られた改善だけを反映する。
 *
 *   最終的に、
 *      1. 完了数が多い
 *      2. 出力ボタン長が短い
 *      3. primitive 長が短い
 *   ものを採用する。
 *
 * デバッグ:
 *   cerr に選ばれたマクロ本体、完了数、primitive 長、出力長、反復数、時間などを出す。
 */

#include <bits/stdc++.h>
using namespace std;

// main() より前に計測を始め、入力読み込み時間も制限時間に含める。
static const auto PROGRAM_START_TIME = chrono::steady_clock::now();

// 問題制約より T <= 2*N^2*M <= 32000。
// 距離・DPコストは 16bit に収め、MoveTree のメモリ帯域を抑える。
static const int INF = numeric_limits<uint16_t>::max();
static const long long ENERGY_UNIT = 100000000LL;
static const int MAX_M = 40;
static const int MAX_SNUM = 20 * 20 * 4;
static const int BEST_HISTORY_COUNT = 20;
static const int BEST_RANKING_COUNT = 10;
static const int SA_TURN_LIMIT_MARGIN = 10;
static const int START_POS = 0;
static const int START_DIR = 1;
using StateId = uint16_t;
static constexpr StateId INVALID_STATE = numeric_limits<StateId>::max();

// ---- Hyper parameters ----------------------------------------------------
static const double SA_TOTAL_TIME_LIMIT_SEC = 1.905;
static const double SA_POSTPROCESS_TIME_LIMIT_SEC = 1.935;
static const double BLOCK_EXACT_DP_DEADLINE_SEC = 1.965;

static const int SA_RANDOM_START_MIN = 4;
static const int SA_RANDOM_START_MAX = 16;

static const double SA_START_TEMP_SCALE = 0.036856774903026544;
static const double SA_END_TEMP = 0.4212420699673085;
static const double SA_UPHILL_ACCEPT_TEMP_LIMIT = 60.0;

// order 近傍の比率。ここは累積値ではなく、そのまま各操作の出現割合。
static const int SA_WEIGHTED_SWAP_MOVE_PERCENT = 30; // 長い接続付近の2点を優先して入れ替える
static const int SA_SWAP_MOVE_PERCENT = 10;       // ランダムな2点を入れ替える
static const int SA_ADJACENT_SWAP_MOVE_PERCENT = 10; // 隣接2点を入れ替える
static const int SA_REVERSE_MOVE_PERCENT = 10;    // 区間を反転する
static const int SA_INSERT_MOVE_PERCENT = 30;     // 1点を別位置へ挿入する
static const int SA_BLOCK_INSERT_MOVE_PERCENT = 10; // 短い連続区間を別位置へ挿入する
static const int SA_BLOCK_INSERT_MAX_LEN = 3;     // block insert の最大区間長
static_assert(
    SA_WEIGHTED_SWAP_MOVE_PERCENT + SA_SWAP_MOVE_PERCENT +
    SA_ADJACENT_SWAP_MOVE_PERCENT +
    SA_REVERSE_MOVE_PERCENT +
    SA_INSERT_MOVE_PERCENT + SA_BLOCK_INSERT_MOVE_PERCENT == 100
);
static const int SA_TIME_CHECK_MASK = 255;
static const double SA_PROGRESS_REPORT_INTERVAL_SEC = 0.20;

// マクロ焼きなまし関連。近傍比率は合計100にする。
static const int SA_MACRO_MUTATE_INTERVAL_ITERS = 1000; // 前回 macro try からこの iter 数ごとに試す
static const int MACRO_SLOT_COUNT = 2;             // 独立に焼きなますマクロ系統数
static const int SA_MACRO_MAX_LEN = 50;            // 焼きなますマクロ本体の最大長
static const int SA_INITIAL_PATH_MACRO_CANDIDATES = 100;  // 経路プールから切り出して軽量評価する初期候補数
static const int SA_INITIAL_PATH_MACRO_MIN_LEN = 8;       // 区間由来初期マクロの切り出し最短長
static const int SA_INITIAL_PATH_MACRO_MAX_LEN = 40;      // 区間由来初期マクロの切り出し最長長
static const int SA_INITIAL_PATH_SAMPLE_COUNT = 12;       // 初期候補の軽量評価に使う経路プールサンプル数
static_assert(SA_INITIAL_PATH_MACRO_CANDIDATES >= 1);
static_assert(SA_INITIAL_PATH_MACRO_MIN_LEN >= 1);
static_assert(SA_INITIAL_PATH_MACRO_MIN_LEN <= SA_INITIAL_PATH_MACRO_MAX_LEN);
static_assert(SA_INITIAL_PATH_SAMPLE_COUNT >= 1);
static const int SA_MACRO_REPLACE_PERCENT = 1;    // マクロ近傍: 1文字置換
static const int SA_MACRO_EDGE_INSERT_PERCENT = 17; // マクロ近傍: 先頭/末尾への1文字追加
static const int SA_MACRO_MIDDLE_INSERT_PERCENT = 45; // マクロ近傍: 内部への1文字挿入
static const int SA_MACRO_DELETE_PERCENT = 17;     // マクロ近傍: 1文字削除
static const int SA_MACRO_REVERSE_PERCENT = 10;    // マクロ近傍: 部分反転
static const int SA_MACRO_PATH_SLICE_PERCENT = 10;  // マクロ近傍: 事前計算区間から同じ長さを切り出す
static const int SA_MACRO_INNER_SHRINK_LIMIT = 2;  // 内側SA中に開始長から縮めてよい文字数
static const int SA_MACRO_INNER_GROW_LIMIT = 2;    // 内側SA中に開始長から伸ばしてよい文字数
static_assert(
    SA_MACRO_REPLACE_PERCENT + SA_MACRO_EDGE_INSERT_PERCENT +
    SA_MACRO_MIDDLE_INSERT_PERCENT +
    SA_MACRO_DELETE_PERCENT + SA_MACRO_REVERSE_PERCENT +
    SA_MACRO_PATH_SLICE_PERCENT == 100
);
static_assert(SA_MACRO_INNER_SHRINK_LIMIT >= 0);
static_assert(SA_MACRO_INNER_GROW_LIMIT >= 0);

// マクロ変異を全体評価する前に、代表的な移動区間だけで短い内側SAを回す。
// ここでは候補を選ぶだけで、採択は従来通り全体評価の energy で決める。
static const int SA_MACRO_SAMPLE_COUNT = 12;          // サンプルする移動区間数
static const int SA_MACRO_SAMPLE_INNER_STEPS = 100;    // 1回のマクロ変異で試す軽量近傍数
static const int SA_MACRO_POOL_EVAL_COUNT = 2;         // サンプル上位から全体評価へ回す本数
static const int SA_MACRO_SAMPLE_BTN_WEIGHT = 1000;   // サンプル評価: ボタン数を強く見る
static const int SA_MACRO_SAMPLE_PHYS_WEIGHT = 1;     // 同点付近では primitive 長も見る
static const double SA_MACRO_SAMPLE_START_TEMP = 3000.0;
static const double SA_MACRO_SAMPLE_END_TEMP = 200.0;

// 登録圧縮では、短い直進マクロ候補をコード内の小さな固定集合として試す。
// 候補を増やすと最終復元の比較回数が増えるので、まずは軽い範囲に留める。
static const bool ENABLE_S_PREFIX_MACRO_FINISH = true;
static const bool ENABLE_S_POSTFIX_MACRO_FINISH = true;
static const bool ENABLE_BLOCK_EXACT_DP_FINISH = true;
static const int BLOCK_EXACT_DP_TARGETS = 12;         // 仕上げで exact DP する1区間の最大ターゲット数
static constexpr array<int, 5> F_RUN_COMPRESS_RUN_LENGTHS = {2, 3, 4, 5, 6}; // F-run 圧縮で試す runLen 候補

int N, M, T;
vector<string> vwall, hwall;
vector<int> ballPos, basketPos;

int SNUM;
vector<array<StateId, 3>> primitiveNextState;
vector<vector<uint16_t>> cellDist;
array<int, 4> startStateByDir;
vector<array<int, 4>> ballStateByDir, basketStateByDir;

// (マス, 向き) を1つの状態番号へ詰める。
inline int sid(int pos, int dir) {
    return pos * 4 + dir;
}

// dir: 0=U, 1=R, 2=D, 3=L
// 1マス直進できるなら移動後のマス、壁か盤外なら -1 を返す。
int move_pos(int pos, int dir) {
    int i = pos / N;
    int j = pos % N;

    if (dir == 0) {
        if (i == 0) return -1;
        if (hwall[i - 1][j] == '1') return -1;
        return (i - 1) * N + j;
    }
    if (dir == 1) {
        if (j == N - 1) return -1;
        if (vwall[i][j] == '1') return -1;
        return i * N + (j + 1);
    }
    if (dir == 2) {
        if (i == N - 1) return -1;
        if (hwall[i][j] == '1') return -1;
        return (i + 1) * N + j;
    }
    if (dir == 3) {
        if (j == 0) return -1;
        if (vwall[i][j - 1] == '1') return -1;
        return i * N + (j - 1);
    }

    return -1;
}

// 向きは無視し、壁だけを考慮したマス間距離を全始点から BFS しておく。
// order 近傍で「遠い接続」を軽く見積もるための代理指標。
void build_cell_distance_cache() {
    int cells = N * N;
    cellDist.assign(cells, vector<uint16_t>(cells, (uint16_t)INF));

    queue<int> que;
    for (int src = 0; src < cells; src++) {
        vector<uint16_t>& dist = cellDist[src];
        dist[src] = 0;
        que.push(src);

        while (!que.empty()) {
            int pos = que.front();
            que.pop();

            for (int dir = 0; dir < 4; dir++) {
                int np = move_pos(pos, dir);
                if (np == -1 || dist[np] != INF) continue;
                dist[np] = (uint16_t)(dist[pos] + 1);
                que.push(np);
            }
        }
    }
}

// 盤面固定で使い回せる状態遷移を前計算する。
// マクロ経路BFSの内側で座標計算や壁判定を繰り返さないためのキャッシュ。
void build_static_state_caches() {
    build_cell_distance_cache();

    primitiveNextState.resize(SNUM);

    for (int pos = 0; pos < N * N; pos++) {
        for (int dir = 0; dir < 4; dir++) {
            int state = sid(pos, dir);

            int np = move_pos(pos, dir);
            primitiveNextState[state][0] = (StateId)((np == -1) ? state : sid(np, dir)); // F
            primitiveNextState[state][1] = (StateId)sid(pos, (dir + 1) & 3);             // R
            primitiveNextState[state][2] = (StateId)sid(pos, (dir + 3) & 3);             // L
        }
    }

    for (int dir = 0; dir < 4; dir++) {
        startStateByDir[dir] = sid(START_POS, dir);
    }

    ballStateByDir.assign(M, {});
    basketStateByDir.assign(M, {});
    for (int k = 0; k < M; k++) {
        for (int dir = 0; dir < 4; dir++) {
            ballStateByDir[k][dir] = sid(ballPos[k], dir);
            basketStateByDir[k][dir] = sid(basketPos[k], dir);
        }
    }
}

struct CostState {
    uint32_t val;

    static constexpr uint32_t INF_VAL = 0xFFFFFFFFu;

    static constexpr uint32_t clamp_component(int x) {
        return (uint32_t)((x > INF) ? INF : x);
    }

    static constexpr uint32_t pack(int btn_, int phys_) {
        return (clamp_component(btn_) << 16) | clamp_component(phys_);
    }

    CostState() : val(INF_VAL) {}
    CostState(int btn_, int phys_) : val(pack(btn_, phys_)) {}

    int btn() const { return (int)(val >> 16); }
    int phys() const { return (int)(val & 0xFFFFu); }
};

static_assert(sizeof(CostState) == 4);

inline int cost_btn(uint32_t val) { return (int)(val >> 16); }
inline int cost_phys(uint32_t val) { return (int)(val & 0xFFFFu); }

// ボタン数優先、同点なら展開後の基本操作数優先で距離を更新する。
bool relax_cost(CostState& dst, int btn, int phys) {
    if (btn >= INF) return false;
    uint32_t nval = CostState::pack(btn, phys);
    if (nval < dst.val) {
        dst.val = nval;
        return true;
    }
    return false;
}

array<CostState, 4> empty_dir_costs() {
    array<CostState, 4> costs;
    for (auto& s : costs) s = CostState{};
    return costs;
}

int best_dir(const array<CostState, 4>& costs) {
    int best = -1;
    for (int d = 0; d < 4; d++) {
        if (costs[d].btn() >= INF) continue;
        if (best == -1 || costs[d].val < costs[best].val) {
            best = d;
        }
    }
    return best;
}

// SA中は後処理で数手増える余地を残しておく。
int safe_turn_limit() {
    return max(0, T - SA_TURN_LIMIT_MARGIN);
}

// 後処理で実際に採用する候補は、問題の上限Tそのもので判定する。
int output_turn_limit() {
    return T;
}

struct MoveTree {
    // 探索中に使う距離だけの木。大量に作るので復元情報は持たない。
    array<uint32_t, MAX_SNUM> dist;
};

struct PathTree {
    // 最終出力のボタン列復元だけで使う木。
    array<uint32_t, MAX_SNUM> dist;
    array<StateId, MAX_SNUM> prevState;
    array<char, MAX_SNUM> prevAction;
};

// マクロを使わない単発ボタン F/R/L の遷移。
int apply_button_state(int state, char action) {
    if (!primitiveNextState.empty()) {
        if (action == 'F') return primitiveNextState[state][0];
        if (action == 'R') return primitiveNextState[state][1];
        if (action == 'L') return primitiveNextState[state][2];
        return state;
    }

    int pos = state / 4;
    int dir = state % 4;

    if (action == 'F') {
        int np = move_pos(pos, dir);
        return (np == -1) ? state : sid(np, dir);
    }

    if (action == 'R') {
        return sid(pos, (dir + 1) & 3);
    }

    if (action == 'L') {
        return sid(pos, (dir + 3) & 3);
    }

    return state;
}

struct SolveResult {
    // 最終的に出力する解と、ログに出す最低限の統計。
    string macroBody;
    int completed = 0;
    int primitiveLen = 0;
    int outputLen = 0;
    string ans;
};

struct XorShift {
    uint64_t x;

    explicit XorShift(uint64_t seed) : x(seed ? seed : 88172645463325252ULL) {}

    uint64_t next64() {
        x ^= x << 7;
        x ^= x >> 9;
        return x;
    }

    int nextInt(int n) {
        return (int)(next64() % (uint64_t)n);
    }

    double nextDouble() {
        return (next64() >> 11) * (1.0 / 9007199254740992.0);
    }
};

double elapsed_sec(chrono::steady_clock::time_point start) {
    return chrono::duration<double>(chrono::steady_clock::now() - start).count();
}

// 入力から決定的な乱数 seed を作る。同じ入力なら同じ探索になる。
uint64_t make_seed() {
    uint64_t h = 1469598103934665603ULL;

    auto mix = [&](uint64_t v) {
        h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    };

    mix((uint64_t)N);
    mix((uint64_t)M);
    mix((uint64_t)T);
    for (int k = 0; k < M; k++) {
        mix((uint64_t)ballPos[k] * 1000003ULL + (uint64_t)basketPos[k]);
    }

    return h;
}

vector<string> macroBodies;
unordered_map<string, int> macroBodyIds;
vector<vector<StateId>> macroBodyNextCache;
vector<vector<unique_ptr<MoveTree>>> macroBodyMoveTreeCache;
vector<vector<unique_ptr<PathTree>>> macroBodyPathTreeCache;

// BFS の frontier は毎回同じサイズ帯で使うので、作業配列を使い回す。
// MoveTree/PathTree の dist はキャッシュ本体に残すが、current/next の再確保は避ける。
array<StateId, MAX_SNUM> bfsCurrentScratch;
array<StateId, MAX_SNUM> bfsNextScratch;

// サンプル区間だけで候補マクロを評価する inner SA 用の作業領域。
// 候補ごと・サンプルごとの再確保と dist 全初期化を避ける。
vector<StateId> sampleMacroNextScratch;
vector<uint32_t> sampleDistScratch;
array<StateId, MAX_SNUM> sampleCurrentScratch;
array<StateId, MAX_SNUM> sampleNextScratch;

// マクロ本体の正規化。
// F で区切られた L/R の連続部分を、同じ向きになる 0-2 手へ縮める。
// LR/RL のように正味回転が0の列は消える。
string normalize_macro_body(const string& body) {
    string normalized;
    string turnRun;
    normalized.reserve(body.size());
    turnRun.reserve(body.size());

    auto flush_turn_run = [&]() {
        if (turnRun.empty()) return;

        int signedRot = 0;
        for (char ch : turnRun) {
            signedRot += (ch == 'R') ? 1 : -1;
        }

        int rot = signedRot % 4;
        if (rot < 0) rot += 4;

        if (rot == 1) {
            normalized.push_back('R');
        } else if (rot == 2) {
            normalized += (signedRot < 0) ? "LL" : "RR";
        } else if (rot == 3) {
            normalized.push_back('L');
        }

        turnRun.clear();
    };

    for (char ch : body) {
        if (ch == 'L' || ch == 'R') {
            turnRun.push_back(ch);
        } else {
            flush_turn_run();
            normalized.push_back(ch);
        }
    }
    flush_turn_run();

    return normalized;
}

// 正規化済みのマクロ本体を登録し、内部IDを返す。
// 同じ文字列なら同じIDを返し、グローバルな移動キャッシュを共有する。
int register_macro_body(const string& normalizedBody) {
    auto it = macroBodyIds.find(normalizedBody);
    if (it != macroBodyIds.end()) return it->second;

    int id = (int)macroBodies.size();
    macroBodies.push_back(normalizedBody);
    macroBodyIds.emplace(macroBodies.back(), id);
    macroBodyNextCache.emplace_back();
    macroBodyMoveTreeCache.emplace_back(SNUM);
    macroBodyPathTreeCache.emplace_back(SNUM);
    return id;
}

// ある状態からマクロ本体を展開して実行した後の状態を求める。
int apply_macro_body_state(int state, const string& body) {
    for (char ch : body) {
        if (ch == 'F') {
            state = primitiveNextState[state][0];
        } else if (ch == 'R') {
            state = primitiveNextState[state][1];
        } else if (ch == 'L') {
            state = primitiveNextState[state][2];
        }
    }

    return state;
}

// macroNext は「各 state に body を適用した終点」を全 state 分まとめて作る。
// state ごとに body をなぞる代わりに、body を 1 文字ずつ全 state へ合成する。
void build_macro_body_next_vector(const string& body, vector<StateId>& next) {
    next.resize(SNUM);
    for (int state = 0; state < SNUM; state++) {
        next[state] = (StateId)state;
    }

    for (char ch : body) {
        if (ch == 'F') {
            for (int state = 0; state < SNUM; state++) {
                next[state] = primitiveNextState[next[state]][0];
            }
        } else if (ch == 'R') {
            for (int state = 0; state < SNUM; state++) {
                next[state] = primitiveNextState[next[state]][1];
            }
        } else {
            for (int state = 0; state < SNUM; state++) {
                next[state] = primitiveNextState[next[state]][2];
            }
        }
    }
}

// 各状態で P を押したときの遷移先を、マクロIDごとに遅延計算する。
const vector<StateId>& get_macro_body_next(int macroId) {
    vector<StateId>& next = macroBodyNextCache[macroId];
    if (!next.empty()) return next;

    const string& body = macroBodies[macroId];
    build_macro_body_next_vector(body, next);
    return next;
}

// 固定マクロに対する層BFS本体。
// 評価用は距離だけ、復元用は prevState/prevAction も埋める。
template <bool TrackPath>
void run_macro_body_bfs_impl(
    int macroId,
    int startState,
    uint32_t* dist,
    StateId* prevState,
    char* prevAction
) {
    const vector<StateId>& pNext = get_macro_body_next(macroId);
    const int macroPhys = (int)macroBodies[macroId].size();

    fill_n(dist, SNUM, CostState::INF_VAL);
    if constexpr (TrackPath) {
        fill_n(prevState, SNUM, INVALID_STATE);
        fill_n(prevAction, SNUM, 0);
    }

    auto* current = &bfsCurrentScratch;
    auto* next = &bfsNextScratch;
    int currentCount = 1;
    int layerBtn = 0;

    dist[startState] = CostState::pack(0, 0);
    (*current)[0] = startState;

    while (currentCount > 0) {
        int nextCount = 0;
        int nextBtn = layerBtn + 1;
        uint32_t nextBtnHi = ((uint32_t)nextBtn) << 16;

        for (int idx = 0; idx < currentCount; idx++) {
            StateId cur = (*current)[idx];
            int curPhys = cost_phys(dist[cur]);
            {
                StateId ns = pNext[cur];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + macroPhys);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if constexpr (TrackPath) {
                            prevState[ns] = cur;
                            prevAction[ns] = 'P';
                        }
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
            {
                StateId ns = primitiveNextState[cur][0];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + 1);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if constexpr (TrackPath) {
                            prevState[ns] = cur;
                            prevAction[ns] = 'F';
                        }
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
            {
                StateId ns = primitiveNextState[cur][1];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + 1);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if constexpr (TrackPath) {
                            prevState[ns] = cur;
                            prevAction[ns] = 'R';
                        }
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
            {
                StateId ns = primitiveNextState[cur][2];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + 1);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if constexpr (TrackPath) {
                            prevState[ns] = cur;
                            prevAction[ns] = 'L';
                        }
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
        }

        swap(current, next);
        currentCount = nextCount;
        layerBtn = nextBtn;
    }
}

void run_macro_body_bfs_nopath(
    int macroId,
    int startState,
    array<uint32_t, MAX_SNUM>& dist
) {
    run_macro_body_bfs_impl<false>(macroId, startState, dist.data(), nullptr, nullptr);
}

void run_macro_body_bfs_with_path(
    int macroId,
    int startState,
    array<uint32_t, MAX_SNUM>& dist,
    array<StateId, MAX_SNUM>& prevState,
    array<char, MAX_SNUM>& prevAction
) {
    run_macro_body_bfs_impl<true>(macroId, startState, dist.data(), prevState.data(), prevAction.data());
}

// 固定マクロに対して、始点から全状態への最短コストだけを層BFSで求める。
// 辺は F/R/L/P で、どれも btn は +1。よって priority_queue は不要。
// 同じ btn で到達できる場合だけ、展開後の基本操作数 phys が小さい方へ更新する。
const MoveTree& get_macro_body_move_tree(int macroId, int startState) {
    unique_ptr<MoveTree>& ptr = macroBodyMoveTreeCache[macroId][startState];
    if (ptr) return *ptr;

    ptr = make_unique<MoveTree>();
    run_macro_body_bfs_nopath(macroId, startState, ptr->dist);

    return *ptr;
}

// 最終出力用に、復元情報付きの木を必要になった始点だけ作る。
const PathTree& get_macro_body_path_tree(int macroId, int startState) {
    unique_ptr<PathTree>& ptr = macroBodyPathTreeCache[macroId][startState];
    if (ptr) return *ptr;

    ptr = make_unique<PathTree>();
    run_macro_body_bfs_with_path(
        macroId,
        startState,
        ptr->dist,
        ptr->prevState,
        ptr->prevAction
    );

    return *ptr;
}

// get_macro_body_path_tree() の復元情報から、start -> target のボタン列だけを作る。
string build_macro_body_move_buttons(int macroId, int startState, int targetState) {
    const PathTree& tree = get_macro_body_path_tree(macroId, startState);

    if (cost_btn(tree.dist[targetState]) >= INF) return "";

    vector<char> rev;
    int cur = targetState;
    while (cur != startState) {
        char action = tree.prevAction[cur];
        if (!action) break;
        rev.push_back(action);
        cur = tree.prevState[cur];
    }

    reverse(rev.begin(), rev.end());
    return string(rev.begin(), rev.end());
}

// 最初の P を、その場で同じ動きをしながら登録する M body M に置き換える。
string replace_first_p_with_body_registration(const string& buttons, const string& registerBody) {
    size_t pos = buttons.find('P');
    if (pos == string::npos) return buttons;

    string out;
    out.reserve(buttons.size() + registerBody.size() + 1);
    out.append(buttons, 0, pos);
    out.push_back('M');
    out += registerBody;
    out.push_back('M');
    out.append(buttons, pos + 1, string::npos);
    return out;
}

string replace_first_p_with_registration(const string& buttons, const string& body) {
    return replace_first_p_with_body_registration(buttons, body);
}

// 出力列を実際のマクロ登録規則で展開したときの基本操作数を数える。
// M 中の P は、その時点で登録済みのマクロを実行し、その展開結果が新しいマクロにも記録される。
int expanded_primitive_length(const string& buttons, int limit = INF) {
    string macro;
    string recordingBody;
    bool recording = false;
    int total = 0;

    auto add_len = [&](int len) -> bool {
        if (len > limit - total) {
            total = limit + 1;
            return false;
        }
        total += len;
        return true;
    };

    for (char ch : buttons) {
        if (ch == 'M') {
            if (recording) {
                macro = recordingBody;
                recordingBody.clear();
                recording = false;
            } else {
                recordingBody.clear();
                recording = true;
            }
            continue;
        }

        if (ch == 'P') {
            if (!add_len((int)macro.size())) return total;
            if (recording) {
                recordingBody += macro;
                if ((int)recordingBody.size() > limit) {
                    recordingBody.resize(limit + 1);
                }
            }
            continue;
        }

        if (ch == 'F' || ch == 'R' || ch == 'L' || ch == 'S') {
            if (!add_len(1)) return total;
            if (recording) recordingBody.push_back(ch);
        }
    }

    return total;
}

// 事前にどこかの生 F^k を MF^kM に置き換えて小マクロ F^k を登録し、
// 本命マクロ登録時の body 内 F^k を P で圧縮する。
// 先に実行される F^k を使うので移動は変わらず、最終的に登録される本命マクロも元の body と同じ。
string f_run_macro_body(int len) {
    return string(len, 'F');
}

string encode_body_with_f_run_macro(const string& body, int runLen) {
    string encoded;
    encoded.reserve(body.size());

    for (int i = 0; i < (int)body.size(); i++) {
        bool match = true;
        if (i + runLen > (int)body.size()) {
            match = false;
        } else {
            for (int j = 0; j < runLen; j++) {
                if (body[i + j] != 'F') {
                    match = false;
                    break;
                }
            }
        }

        if (match) {
            encoded.push_back('P');
            i += runLen - 1;
        } else {
            encoded.push_back(body[i]);
        }
    }

    return encoded;
}

struct RegistrationCompressionLog {
    bool used = false;
    int sourceKind = 0;  // 0=none, 1=path, 2=airshot, 3=moved-forward, 4=moved-turn-then-forward
    int normalLen = 0;
    int compressedLen = 0;
    int saved = 0;
    int runLen = 0;
    int replacedRuns = 0;
    int firstRunPos = -1;
};

const char* compression_mode_name(const RegistrationCompressionLog& log) {
    if (!log.used) return "none";
    if (log.sourceKind == 1) return "path-f-run";
    if (log.sourceKind == 2) return "airshot-f-run";
    if (log.sourceKind == 3) return "moved-forward-run";
    if (log.sourceKind == 4) return "moved-turn-then-forward-run";
    return "unknown";
}

struct EdgeSMacroLog {
    bool tried = false;
    bool used = false;
    int normalLen = 0;    // 後処理全体の元長
    int inputLen = 0;     // pre/post S を試した直前の長さ
    int candidateLen = 0;
    int saved = 0;
    int totalSaved = 0;
    int absorbedS = 0;
    int guardedP = 0;
    int freeP = 0;
};

struct EdgeSSwitchLog {
    bool tried = false;
    bool used = false;
    bool toPrefix = false;
    int mode = 0;
    int normalLen = 0;
    int inputLen = 0;
    int candidateLen = 0;
    int saved = 0;
    int totalSaved = 0;
    int totalUses = 0;
    int switchUse = 0;
    int switchBackUse = 0;
    int noBackBestLen = INF;
    int switchBackSaved = 0;
    int absorbedS = 0;
    int guardedP = 0;
    int freeP = 0;
};

struct EdgeSMacroUseInfo {
    int pPos = -1;
    int prefixAbsorbPos = -1;
    int postfixAbsorbPos = -1;
    bool prefixGuard = false;
    bool postfixGuard = false;
};

enum EdgeSMode {
    EDGE_S_PREFIX = 0,
    EDGE_S_POSTFIX = 1,
    EDGE_S_BOTH = 2,
};

const char* edge_s_mode_name(int mode) {
    if (mode == EDGE_S_PREFIX) return "prefix";
    if (mode == EDGE_S_POSTFIX) return "postfix";
    if (mode == EDGE_S_BOTH) return "both";
    return "unknown";
}

const char* edge_s_mode_name(bool toPrefix) {
    return edge_s_mode_name(toPrefix ? EDGE_S_PREFIX : EDGE_S_POSTFIX);
}

int next_non_turn_pos(const string& raw, int pos);

int count_compressed_f_runs(const string& body, int runLen) {
    int count = 0;
    for (int i = 0; i + runLen <= (int)body.size(); i++) {
        bool match = true;
        for (int j = 0; j < runLen; j++) {
            if (body[i + j] != 'F') {
                match = false;
                break;
            }
        }
        if (match) {
            count++;
            i += runLen - 1;
        }
    }
    return count;
}

string try_replace_first_p_with_f_run_compression(
    const string& buttons,
    const string& body,
    int runLen,
    const string& normal,
    RegistrationCompressionLog* candidateLog
) {
    size_t firstP = buttons.find('P');
    if (firstP == string::npos) return normal;

    string runBody = f_run_macro_body(runLen);
    size_t runPos = buttons.find(runBody);
    bool useExistingRun = (runPos != string::npos && runPos + runLen - 1 < firstP);

    string encodedBody = encode_body_with_f_run_macro(body, runLen);
    const int normalExtra = (int)body.size() + 1;
    const int compressedExtra =
        (int)encodedBody.size() + (useExistingRun ? 3 : runLen + 5);
    if (compressedExtra >= normalExtra) return normal;

    string out;
    out.reserve(buttons.size() + encodedBody.size() + runLen + 8);

    if (useExistingRun) {
        out.append(buttons, 0, runPos);
        out.push_back('M');
        out += runBody;
        out.push_back('M');
        out.append(buttons, runPos + runLen, firstP - (runPos + runLen));
    } else {
        // 自然に出る F^k がなくても、冒頭で壁へ空撃ちして登録する方が出力長で得なら使う。
        // ここでは元経路を壊さないことを優先し、左上で上向きに撃ってから向きを戻す。
        out.push_back('L');
        out.push_back('M');
        out += runBody;
        out.push_back('M');
        out.push_back('R');
        out.append(buttons, 0, firstP);
    }
    out.push_back('M');
    out += encodedBody;
    out.push_back('M');
    out.append(buttons, firstP + 1, string::npos);

    if (out.size() < normal.size() && candidateLog != nullptr) {
        candidateLog->used = true;
        candidateLog->sourceKind = useExistingRun ? 1 : 2;
        candidateLog->normalLen = (int)normal.size();
        candidateLog->compressedLen = (int)out.size();
        candidateLog->saved = (int)normal.size() - (int)out.size();
        candidateLog->runLen = runLen;
        candidateLog->replacedRuns = count_compressed_f_runs(body, runLen);
        candidateLog->firstRunPos = useExistingRun ? (int)runPos : -1;
    }

    return out;
}

struct RegistrationEstimate {
    int btn = 0;
    int phys = 0;
};

struct PrefixRunInfo {
    int maxRun = 0;
    bool hitP = false;
};

// 評価用の登録コスト見積もり。
// 通常登録は、経路上の最初の P を M body M に置換するだけなので、
// その P が元々実行していた body の primitive は経路側で既に数えられている。
// 一方、F^k 圧縮のために冒頭で空撃ち登録する場合は、その空撃ち分の primitive が追加で必要。
RegistrationEstimate estimated_registration_extra(const string& body, int availableRunLen) {
    RegistrationEstimate best;
    best.btn = (int)body.size() + 1;
    best.phys = 0;
    if (body.find('P') != string::npos) return best;

    for (int runLen : F_RUN_COMPRESS_RUN_LENGTHS) {
        string encodedBody = encode_body_with_f_run_macro(body, runLen);
        if (availableRunLen >= runLen) {
            int reusedExtra = (int)encodedBody.size() + 3;
            if (reusedExtra < best.btn ||
                (reusedExtra == best.btn && best.phys > 0)) {
                best.btn = reusedExtra;
                best.phys = 0;
            }
        }
        int compressedExtra = (int)encodedBody.size() + runLen + 5;
        int compressedPhysExtra = runLen + 2; // L, F^k, R を空撃ちする
        if (compressedExtra < best.btn ||
            (compressedExtra == best.btn && compressedPhysExtra < best.phys)) {
            best.btn = compressedExtra;
            best.phys = compressedPhysExtra;
        }
    }
    return best;
}

RegistrationEstimate estimated_registration_extra(const string& body) {
    return estimated_registration_extra(body, 0);
}

PrefixRunInfo analyze_path_prefix_until_first_p(
    const PathTree& tree,
    int startState,
    int targetState
) {
    PrefixRunInfo info;
    if (cost_btn(tree.dist[targetState]) >= INF) return info;

    vector<char> rev;
    int cur = targetState;
    while (cur != startState) {
        char action = tree.prevAction[cur];
        if (!action) break;
        rev.push_back(action);
        cur = tree.prevState[cur];
    }

    int run = 0;
    for (auto it = rev.rbegin(); it != rev.rend(); ++it) {
        char action = *it;
        if (action == 'P') {
            info.hitP = true;
            break;
        }
        if (action == 'F') {
            run++;
            info.maxRun = max(info.maxRun, run);
        } else {
            run = 0;
        }
    }

    return info;
}

// 焼きなましでマクロ本体を壊すときに使う1文字を選ぶ。
char random_macro_char(XorShift& rng) {
    static const char cs[3] = {'F', 'R', 'L'};
    return cs[rng.nextInt(3)];
}

string shortest_primitive_buttons(int startState, int targetState) {
    if (startState == targetState) return "";

    vector<int> dist(SNUM, -1);
    vector<int> prev(SNUM, -1);
    vector<char> action(SNUM, 0);
    vector<int> current, next;
    current.reserve(SNUM);
    next.reserve(SNUM);

    dist[startState] = 0;
    current.push_back(startState);

    auto relax = [&](int cur, int ns, char ch) {
        if (dist[ns] != -1) return;
        dist[ns] = dist[cur] + 1;
        prev[ns] = cur;
        action[ns] = ch;
        next.push_back(ns);
    };

    while (!current.empty() && dist[targetState] == -1) {
        next.clear();
        for (int cur : current) {
            relax(cur, primitiveNextState[cur][0], 'F');
            relax(cur, primitiveNextState[cur][1], 'R');
            relax(cur, primitiveNextState[cur][2], 'L');
        }
        current.swap(next);
    }

    if (dist[targetState] == -1) return "";

    vector<char> rev;
    int cur = targetState;
    while (cur != startState) {
        rev.push_back(action[cur]);
        cur = prev[cur];
        if (cur < 0) return "";
    }

    reverse(rev.begin(), rev.end());
    return string(rev.begin(), rev.end());
}

struct InitialPathMacroCandidate {
    string body;
    int segmentId = -1;
};

struct InitialPathSource {
    // 初期候補用に集めた、自然な移動区間の F/R/L 最短路。
    // startState -> targetState をこの path で移動できる。
    string path;
    int segmentId = -1;
    int startState = 0;
    int targetState = 0;
};

struct MacroSample {
    // 現在の経路や初期経路プールで使う「移動だけ」の代表区間。
    // startState -> targetState を、候補マクロでどれだけ短くできるかを軽量評価する。
    int startState = 0;
    int targetState = 0;
    int weight = 1;
};

vector<InitialPathSource> build_initial_path_pool() {
    vector<pair<int, int>> segments;
    segments.reserve(max(1, 2 * M));

    int curPos = START_POS;
    for (int k = 0; k < M; k++) {
        segments.push_back({curPos, ballPos[k]});
        segments.push_back({ballPos[k], basketPos[k]});
        curPos = basketPos[k];
    }

    vector<InitialPathSource> pool;
    pool.reserve(max(1, (int)segments.size() * 16));

    for (int segmentId = 0; segmentId < (int)segments.size(); segmentId++) {
        int fromPos = segments[segmentId].first;
        int toPos = segments[segmentId].second;

        for (int sd = 0; sd < 4; sd++) {
            if (segmentId == 0 && sd != START_DIR) continue;
            for (int td = 0; td < 4; td++) {
                int startState = sid(fromPos, sd);
                int targetState = sid(toPos, td);
                string path = shortest_primitive_buttons(startState, targetState);
                if (path.empty()) continue;

                pool.push_back(InitialPathSource{
                    std::move(path),
                    segmentId,
                    startState,
                    targetState
                });
            }
        }
    }

    return pool;
}

InitialPathMacroCandidate make_initial_path_macro_body(
    const vector<InitialPathSource>& pool,
    XorShift& rng
) {
    if (pool.empty()) return {};

    const InitialPathSource& source = pool[rng.nextInt((int)pool.size())];
    if (source.path.empty()) return {};

    int maxLen = min(SA_INITIAL_PATH_MACRO_MAX_LEN, (int)source.path.size());
    int minLen = min(SA_INITIAL_PATH_MACRO_MIN_LEN, maxLen);
    int len = minLen + rng.nextInt(maxLen - minLen + 1);
    int start = rng.nextInt((int)source.path.size() - len + 1);

    InitialPathMacroCandidate ret;
    ret.body = normalize_macro_body(source.path.substr(start, len));
    ret.segmentId = source.segmentId;
    return ret;
}

vector<MacroSample> make_initial_path_macro_samples(
    const vector<InitialPathSource>& pool,
    XorShift& rng
) {
    vector<MacroSample> samples;
    if (pool.empty()) return samples;

    vector<int> ids(pool.size());
    iota(ids.begin(), ids.end(), 0);

    int count = min(SA_INITIAL_PATH_SAMPLE_COUNT, (int)ids.size());
    for (int i = 0; i < count; i++) {
        int j = i + rng.nextInt((int)ids.size() - i);
        swap(ids[i], ids[j]);
        const InitialPathSource& source = pool[ids[i]];
        samples.push_back(MacroSample{source.startState, source.targetState, 1});
    }

    return samples;
}

string make_path_slice_macro_body(
    const vector<InitialPathSource>& pool,
    int len,
    XorShift& rng
) {
    if (pool.empty() || len <= 0) return "";

    for (int tries = 0; tries < 16; tries++) {
        const InitialPathSource& source = pool[rng.nextInt((int)pool.size())];
        if ((int)source.path.size() < len) continue;

        int start = rng.nextInt((int)source.path.size() - len + 1);
        string body = normalize_macro_body(source.path.substr(start, len));
        if (!body.empty()) return body;
    }

    return "";
}

// マクロ本体の近傍を1つ作る。置換/挿入/削除/反転の後、必ず正規化する。
// minLen/maxLen は長さを動かす近傍だけに効く。置換/反転は長さを変えない。
string mutate_macro_body(
    const string& body,
    XorShift& rng,
    int maxLen = SA_MACRO_MAX_LEN,
    int minLen = 1
) {
    string cand = body;
    minLen = max(1, min(minLen, maxLen));
    int moveType = rng.nextInt(100);

    if (cand.empty()) {
        cand.push_back(random_macro_char(rng));
        return normalize_macro_body(cand);
    }

    const int replaceLimit = SA_MACRO_REPLACE_PERCENT;
    const int edgeInsertLimit = replaceLimit + SA_MACRO_EDGE_INSERT_PERCENT;
    const int middleInsertLimit = edgeInsertLimit + SA_MACRO_MIDDLE_INSERT_PERCENT;
    const int deleteLimit = middleInsertLimit + SA_MACRO_DELETE_PERCENT;
    const int reverseLimit = deleteLimit + SA_MACRO_REVERSE_PERCENT;

    if (moveType < replaceLimit) {
        int i = rng.nextInt((int)cand.size());
        char ch = random_macro_char(rng);
        if (cand.size() > 1) {
            int guard = 0;
            while (ch == cand[i] && guard++ < 8) ch = random_macro_char(rng);
        }
        cand[i] = ch;
    } else if (moveType < edgeInsertLimit && (int)cand.size() < maxLen) {
        int i = (rng.nextInt(2) == 0) ? 0 : (int)cand.size();
        cand.insert(cand.begin() + i, random_macro_char(rng));
    } else if (moveType < middleInsertLimit && (int)cand.size() < maxLen) {
        if ((int)cand.size() <= 1) {
            int i = rng.nextInt((int)cand.size() + 1);
            cand.insert(cand.begin() + i, random_macro_char(rng));
        } else {
            int i = 1 + rng.nextInt((int)cand.size() - 1);
            cand.insert(cand.begin() + i, random_macro_char(rng));
        }
    } else if (moveType < deleteLimit && (int)cand.size() > minLen) {
        int i = rng.nextInt((int)cand.size());
        cand.erase(cand.begin() + i);
    } else if (moveType < reverseLimit && (int)cand.size() >= 2) {
        int l = rng.nextInt((int)cand.size());
        int r = rng.nextInt((int)cand.size());
        if (l > r) swap(l, r);
        reverse(cand.begin() + l, cand.begin() + r + 1);
    }

    return normalize_macro_body(cand);
}

struct TransOpt {
    // 「直前位置 prev からボール k を処理し、向き startDir -> endDir へ行く」1遷移の候補。
    uint16_t addBtn;
    uint16_t addPhys;
    uint8_t endDir;
    uint8_t midDir;

    TransOpt() = default;
    TransOpt(int endDir_, int addBtn_, int addPhys_, int midDir_)
        : addBtn((uint16_t)min(addBtn_, INF)),
          addPhys((uint16_t)min(addPhys_, INF)),
          endDir((uint8_t)endDir_),
          midDir((uint8_t)midDir_) {}
};

static_assert(sizeof(TransOpt) <= 8);

struct TransList {
    // 終了向きは最大4通りなので、vector を使わず固定配列で持つ。
    int count;
    array<TransOpt, 4> items;

    TransList() : count(0) {}

    void push(const TransOpt& opt) {
        items[count++] = opt;
    }
};

struct TransitionCache {
    // 1つのマクロ本体に対する prev/k/startDir の遷移表。
    // 全構築は重いので、必要になった遷移だけ get() で遅延構築する。
    int macroId = -1;
    int registrationExtraBtn = 0;
    int registrationExtraPhys = 0;
    string macroBody;
    mutable vector<TransList> opts;
    mutable vector<uint8_t> built;
    mutable vector<TransList> initialOpts;
    mutable vector<uint8_t> initialBuilt;
    mutable vector<const MoveTree*> moveTreeByState;

    int index(int prev, int k, int startDir) const {
        return (prev * M + k) * 4 + startDir;
    }

    const MoveTree& get_tree(int state) const {
        const MoveTree*& ptr = moveTreeByState[state];
        if (ptr == nullptr) {
            ptr = &get_macro_body_move_tree(macroId, state);
        }
        return *ptr;
    }

    void build_from_state(int fromState, int k, TransList& out) const {
        out.count = 0;

        const MoveTree& fromTree = get_tree(fromState);

        array<CostState, 4> toBall;
        array<const MoveTree*, 4> fromBallTrees;
        for (int bd = 0; bd < 4; bd++) {
            toBall[bd].val = fromTree.dist[ballStateByDir[k][bd]];
            fromBallTrees[bd] = &get_tree(ballStateByDir[k][bd]);
        }

        for (int cd = 0; cd < 4; cd++) {
            CostState bestCost;
            int bestBd = 0;
            int basketState = basketStateByDir[k][cd];

            for (int bd = 0; bd < 4; bd++) {
                const CostState p1 = toBall[bd];
                if (p1.btn() >= INF) continue;

                const MoveTree& ballTree = *fromBallTrees[bd];
                CostState p2;
                p2.val = ballTree.dist[basketState];
                if (p2.btn() >= INF) continue;

                int addBtn = p1.btn() + 1 + p2.btn() + 1;
                int addPhys = p1.phys() + 1 + p2.phys() + 1;

                CostState candCost(addBtn, addPhys);
                if (candCost.val < bestCost.val) {
                    bestCost = candCost;
                    bestBd = bd;
                }
            }

            if (bestCost.btn() < INF) {
                out.push(TransOpt{
                    cd,
                    bestCost.btn(),
                    bestCost.phys(),
                    bestBd
                });
            }
        }
    }

    void build_initial_one(int k, TransList& out) const {
        out.count = 0;

        PathTree fromStartTree;
        run_macro_body_bfs_with_path(
            macroId,
            sid(START_POS, START_DIR),
            fromStartTree.dist,
            fromStartTree.prevState,
            fromStartTree.prevAction
        );

        array<PathTree, 4> fromBallTrees;
        array<uint8_t, 4> ballBuilt{};
        array<CostState, 4> toBall;
        array<PrefixRunInfo, 4> firstInfos;
        for (int bd = 0; bd < 4; bd++) {
            int ballState = ballStateByDir[k][bd];
            toBall[bd].val = fromStartTree.dist[ballState];
            if (toBall[bd].btn() >= INF) continue;
            firstInfos[bd] = analyze_path_prefix_until_first_p(
                fromStartTree,
                sid(START_POS, START_DIR),
                ballState
            );
        }

        for (int cd = 0; cd < 4; cd++) {
            CostState bestCost;
            int bestBd = 0;
            int basketState = basketStateByDir[k][cd];

            for (int bd = 0; bd < 4; bd++) {
                const CostState p1 = toBall[bd];
                if (p1.btn() >= INF) continue;

                if (!ballBuilt[bd]) {
                    ballBuilt[bd] = 1;
                    run_macro_body_bfs_with_path(
                        macroId,
                        ballStateByDir[k][bd],
                        fromBallTrees[bd].dist,
                        fromBallTrees[bd].prevState,
                        fromBallTrees[bd].prevAction
                    );
                }

                const PathTree& ballTree = fromBallTrees[bd];
                CostState p2;
                p2.val = ballTree.dist[basketState];
                if (p2.btn() >= INF) continue;

                int maxRun = firstInfos[bd].maxRun;
                if (!firstInfos[bd].hitP) {
                    PrefixRunInfo secondInfo = analyze_path_prefix_until_first_p(
                        ballTree,
                        ballStateByDir[k][bd],
                        basketState
                    );
                    maxRun = max(maxRun, secondInfo.maxRun);
                }

                RegistrationEstimate reg = estimated_registration_extra(macroBody, maxRun);
                int addBtn = reg.btn + p1.btn() + 1 + p2.btn() + 1;
                int addPhys = reg.phys + p1.phys() + 1 + p2.phys() + 1;

                CostState candCost(addBtn, addPhys);
                if (candCost.val < bestCost.val) {
                    bestCost = candCost;
                    bestBd = bd;
                }
            }

            if (bestCost.btn() < INF) {
                out.push(TransOpt{
                    cd,
                    bestCost.btn(),
                    bestCost.phys(),
                    bestBd
                });
            }
        }
    }

    void build_one(int prev, int k, int startDir, TransList& out) const {
        int fromState = (prev == M) ? startStateByDir[startDir] : basketStateByDir[prev][startDir];
        build_from_state(fromState, k, out);
    }

    const TransList& get(int prev, int k, int startDir) const {
        int idx = index(prev, k, startDir);
        if (!built[idx]) {
            build_one(prev, k, startDir, opts[idx]);
            built[idx] = 1;
        }
        return opts[idx];
    }

    const TransList& get_initial(int k) const {
        if (!initialBuilt[k]) {
            build_initial_one(k, initialOpts[k]);
            initialBuilt[k] = 1;
        }
        return initialOpts[k];
    }

    void prebuild_for_order(const array<int, MAX_M>& order, int steps) const {
        steps = max(0, min(steps, M));
        if (steps <= 0) return;

        get_initial(order[0]);
        for (int i = 1; i < steps; i++) {
            int prev = order[i - 1];
            int k = order[i];
            for (int sd = 0; sd < 4; sd++) {
                get(prev, k, sd);
            }
        }
    }
};

// 評価DPの初期状態。
// 登録差分は初手候補ごとに変わるので、0手目では持たず、最初の遷移に含める。
array<CostState, 4> initial_dir_costs(const TransitionCache& cache) {
    array<CostState, 4> dp = empty_dir_costs();
    (void)cache;
    dp[START_DIR] = CostState{0, 0};
    return dp;
}

const TransList& get_order_transition_list(
    const TransitionCache& cache,
    int prev,
    int k,
    int startDir
) {
    if (prev == M) return cache.get_initial(k);
    return cache.get(prev, k, startDir);
}

array<CostState, 4> advance_order_dp(
    const TransitionCache& cache,
    const array<CostState, 4>& dp,
    int prev,
    int k
) {
    array<CostState, 4> ndp = empty_dir_costs();

    for (int sd = 0; sd < 4; sd++) {
        if (dp[sd].btn() >= INF) continue;

        if (prev == M && sd != START_DIR) continue;

            const TransList& list = get_order_transition_list(cache, prev, k, sd);
        for (int oi = 0; oi < list.count; oi++) {
            const TransOpt& opt = list.items[oi];
            int nPhys = dp[sd].phys() + opt.addPhys;
            if (nPhys > safe_turn_limit()) continue;

            int nBtn = dp[sd].btn() + opt.addBtn;
            if (nBtn > safe_turn_limit()) continue;

            relax_cost(ndp[opt.endDir], nBtn, nPhys);
        }
    }

    return ndp;
}

// マクロ本体 rawBody を登録する。遷移表は必要になったものだけ遅延構築する。
// prev=M は初期位置を表し、それ以外は「直前に置いたかご」の index。
TransitionCache build_transition_cache(const string& rawBody) {
    string body = normalize_macro_body(rawBody);

    TransitionCache cache;
    cache.macroId = register_macro_body(body);
    RegistrationEstimate estimate = estimated_registration_extra(body);
    cache.registrationExtraBtn = estimate.btn;
    cache.registrationExtraPhys = estimate.phys;
    cache.macroBody = body;
    cache.opts.resize((M + 1) * M * 4);
    cache.built.assign((M + 1) * M * 4, 0);
    cache.initialOpts.resize(M);
    cache.initialBuilt.assign(M, 0);
    cache.moveTreeByState.assign(SNUM, nullptr);

    return cache;
}

struct OrderEval {
    // 固定されたボール順序を、あるマクロで先頭から何個処理できるか評価した結果。
    int completed = 0;
    int btn = 0;
    int phys = 0;
    long long energy = (1LL << 60);
};

struct OrderState {
    // 現在の順序と、その順序を先頭から処理した prefix DP。
    // prefixDp[i] は「先頭 i 個を処理した後、各向きで終わる最小コスト」。
    array<int, MAX_M> order{};
    array<array<CostState, 4>, MAX_M + 1> prefixDp{};
    OrderEval eval;
};

// 順序の先頭 from 個までは同じ、という前提で from 以降だけ prefix DP を作り直す。
// energyCutoff を指定した場合、最終的にその値以下になれない候補は途中で打ち切る。
// 未処理数ペナルティは後で減り得るので、単調に増える btn だけを下界として使う。
bool rebuild_order_state_from(
    const TransitionCache& cache,
    OrderState& state,
    int from,
    long long energyCutoff = (1LL << 60)
) {
    const int n = M;

    from = max(0, min(from, n));

    if (from == 0) {
        state.prefixDp[0] = initial_dir_costs(cache);
    }

    OrderEval ret;
    ret.energy = (long long)M * ENERGY_UNIT;

    auto too_expensive_even_if_all_completed = [&](int btn, int completed) {
        // 未計算の各ボールも、完了するには最低でも拾うSと置くSの2ボタンが必要。
        // 移動は0の可能性があるので、ここでは 2 * 残り個数 だけを安全な下界にする。
        int remaining = n - completed;
        return (long long)btn + 2LL * remaining > energyCutoff;
    };

    for (int i = 1; i <= from; i++) {
        int d = best_dir(state.prefixDp[i]);
        if (d == -1) {
            state.eval = ret;
            for (int j = i; j < n; j++) {
                state.prefixDp[j + 1] = empty_dir_costs();
            }
            return ret.energy <= energyCutoff;
        }

        ret.completed = i;
        ret.btn = state.prefixDp[i][d].btn();
        ret.phys = state.prefixDp[i][d].phys();
        ret.energy = (long long)(M - ret.completed) * ENERGY_UNIT + ret.btn;
        if (too_expensive_even_if_all_completed(ret.btn, ret.completed)) {
            state.eval = ret;
            return false;
        }
    }

    if (from == 0) {
        int d = best_dir(state.prefixDp[0]);
        if (d != -1 && too_expensive_even_if_all_completed(state.prefixDp[0][d].btn(), 0)) {
            state.eval = ret;
            return false;
        }
    }

    for (int i = from; i < n; i++) {
        int prev = (i == 0) ? M : state.order[i - 1];
        int k = state.order[i];
        state.prefixDp[i + 1] = advance_order_dp(cache, state.prefixDp[i], prev, k);

        int d = best_dir(state.prefixDp[i + 1]);
        if (d == -1) {
            for (int j = i + 1; j < n; j++) {
                state.prefixDp[j + 1] = empty_dir_costs();
            }
            break;
        }

        ret.completed = i + 1;
        ret.btn = state.prefixDp[i + 1][d].btn();
        ret.phys = state.prefixDp[i + 1][d].phys();
        ret.energy = (long long)(M - ret.completed) * ENERGY_UNIT + ret.btn;
        if (too_expensive_even_if_all_completed(ret.btn, ret.completed)) {
            state.eval = ret;
            return false;
        }
    }

    state.eval = ret;
    return ret.energy <= energyCutoff;
}

OrderState make_order_state(const TransitionCache& cache, const vector<int>& order) {
    OrderState state;
    for (int i = 0; i < M; i++) state.order[i] = order[i];
    rebuild_order_state_from(cache, state, 0);
    return state;
}

vector<int> order_state_to_vector(const OrderState& state) {
    vector<int> order(M);
    for (int i = 0; i < M; i++) order[i] = state.order[i];
    return order;
}

// 解の優劣。まず完了数、次に出力ボタン数、最後に展開後の基本操作数。
bool better_eval(const OrderEval& a, const OrderEval& b) {
    if (a.completed != b.completed) return a.completed > b.completed;
    if (a.btn != b.btn) return a.btn < b.btn;
    return a.phys < b.phys;
}

struct BestSnapshot {
    vector<int> order;
    int cache = 0;
    OrderEval eval;
};

struct BlockExactDpLog {
    int k = 0;
    int blocks = 0;
    int improvedBlocks = 0;
    int deterministicImprovedBlocks = 0;
    int randomBlocks = 0;
    int randomImprovedBlocks = 0;
    int deterministicAfter = INF;
    int randomAfter = INF;
    bool timeout = false;
    double time = 0.0;
};

struct BuiltSnapshot {
    BestSnapshot snapshot;
    SolveResult result;
    OrderEval eval;
    int cache = 0;
    RegistrationCompressionLog compressionLog;
    EdgeSMacroLog sPrefixLog;
    EdgeSMacroLog sBothLog;
    EdgeSMacroLog sPostfixLog;
    EdgeSSwitchLog sSwitchLog;
    bool blockExactDpTried = false;
    bool blockExactDpUsed = false;
    int blockExactDpBefore = INF;
    int blockExactDpAfter = INF;
    BlockExactDpLog blockExactDpLog;
};

CostState evaluate_fixed_tail_from(
    const TransitionCache& cache,
    const vector<int>& order,
    int fromPos,
    int completed,
    int prevTarget,
    int startDir
) {
    array<CostState, 4> cur{};
    cur[startDir] = CostState(0, 0);

    int prev = prevTarget;
    for (int pos = fromPos; pos < completed; pos++) {
        int target = order[pos];
        array<CostState, 4> nxt{};
        for (int sd = 0; sd < 4; sd++) {
            if (cur[sd].btn() >= INF) continue;
            const TransList& list = cache.get(prev, target, sd);
            for (int oi = 0; oi < list.count; oi++) {
                const TransOpt& opt = list.items[oi];
                relax_cost(
                    nxt[opt.endDir],
                    cur[sd].btn() + opt.addBtn,
                    cur[sd].phys() + opt.addPhys
                );
            }
        }
        cur = nxt;
        prev = target;
    }

    CostState best;
    for (int dir = 0; dir < 4; dir++) {
        relax_cost(best, cur[dir].btn(), cur[dir].phys());
    }
    return best;
}

bool improve_order_block_exact_dp(
    const TransitionCache& cache,
    const BestSnapshot& base,
    int blockStart,
    int blockLen,
    BestSnapshot& improved,
    chrono::steady_clock::time_point deadline,
    BlockExactDpLog* log
) {
    if (!ENABLE_BLOCK_EXACT_DP_FINISH) return false;
    auto startTime = chrono::steady_clock::now();

    auto finish = [&](bool ok) {
        if (log != nullptr) {
            log->time = chrono::duration<double>(
                chrono::steady_clock::now() - startTime
            ).count();
        }
        return ok;
    };

    const int completed = base.eval.completed;
    if (blockStart < 0 || blockStart >= completed) return finish(false);
    const int kNum = min(blockLen, completed - blockStart);
    if (log != nullptr) log->k = max(log->k, kNum);
    if (kNum <= 1) return finish(false);

    OrderState baseState = make_order_state(cache, base.order);
    int prefixDir = best_dir(baseState.prefixDp[blockStart]);
    if (prefixDir == -1) return finish(false);

    vector<int> block(kNum);
    for (int i = 0; i < kNum; i++) {
        block[i] = base.order[blockStart + i];
    }

    const int maskCount = 1 << kNum;
    const int totalStates = maskCount * kNum * 4;
    vector<CostState> dp(totalStates);
    vector<int8_t> parentLast(totalStates, -1);
    vector<int8_t> parentDir(totalStates, -1);

    auto idx_of = [&](int mask, int last, int dir) {
        return ((mask * kNum + last) * 4 + dir);
    };

    auto relax_state = [&](int mask, int last, int dir, int btn, int phys, int prevLast, int prevDir) {
        if (btn > safe_turn_limit() || phys > safe_turn_limit()) return;
        int idx = idx_of(mask, last, dir);
        if (relax_cost(dp[idx], btn, phys)) {
            parentLast[idx] = (int8_t)prevLast;
            parentDir[idx] = (int8_t)prevDir;
        }
    };

    int fixedPrev = (blockStart == 0) ? M : base.order[blockStart - 1];
    for (int ni = 0; ni < kNum; ni++) {
        int target = block[ni];
        int mask = 1 << ni;

        for (int sd = 0; sd < 4; sd++) {
            if (baseState.prefixDp[blockStart][sd].btn() >= INF) continue;

            const TransList& list = cache.get(fixedPrev, target, sd);
            for (int oi = 0; oi < list.count; oi++) {
                const TransOpt& opt = list.items[oi];
                int nBtn = baseState.prefixDp[blockStart][sd].btn() + opt.addBtn;
                int nPhys = baseState.prefixDp[blockStart][sd].phys() + opt.addPhys;
                relax_state(mask, ni, opt.endDir, nBtn, nPhys, -1, sd);
            }
        }
    }

    for (int mask = 1; mask < maskCount; mask++) {
        if ((mask & 31) == 0 && chrono::steady_clock::now() >= deadline) {
            if (log != nullptr) log->timeout = true;
            return finish(false);
        }

        for (int last = 0; last < kNum; last++) {
            if ((mask & (1 << last)) == 0) continue;
            int prevTarget = block[last];

            for (int sd = 0; sd < 4; sd++) {
                const CostState cur = dp[idx_of(mask, last, sd)];
                if (cur.btn() >= INF) continue;

                int rest = ((maskCount - 1) ^ mask);
                while (rest) {
                    int bit = rest & -rest;
                    int ni = __builtin_ctz((unsigned)bit);
                    rest -= bit;

                    int target = block[ni];
                    const TransList& list = cache.get(prevTarget, target, sd);
                    int nMask = mask | bit;
                    for (int oi = 0; oi < list.count; oi++) {
                        const TransOpt& opt = list.items[oi];
                        int nBtn = cur.btn() + opt.addBtn;
                        int nPhys = cur.phys() + opt.addPhys;
                        relax_state(nMask, ni, opt.endDir, nBtn, nPhys, last, sd);
                    }
                }
            }
        }
    }

    const int fullMask = maskCount - 1;
    int bestLast = -1;
    int bestDir = -1;
    CostState bestTotal;
    const int nextPos = blockStart + kNum;
    for (int last = 0; last < kNum; last++) {
        for (int dir = 0; dir < 4; dir++) {
            const CostState c = dp[idx_of(fullMask, last, dir)];
            if (c.btn() >= INF) continue;

            CostState tail = evaluate_fixed_tail_from(
                cache,
                base.order,
                nextPos,
                completed,
                block[last],
                dir
            );
            if (tail.btn() >= INF) continue;
            CostState total(c.btn() + tail.btn(), c.phys() + tail.phys());

            if (bestLast == -1 || total.val < bestTotal.val) {
                bestLast = last;
                bestDir = dir;
                bestTotal = total;
            }
        }
    }
    if (bestLast == -1) return finish(false);

    vector<int> newBlock(kNum);
    int mask = fullMask;
    int last = bestLast;
    int dir = bestDir;
    for (int pos = kNum - 1; pos >= 0; pos--) {
        int idx = idx_of(mask, last, dir);
        newBlock[pos] = block[last];
        int prevLast = parentLast[idx];
        int prevDir = parentDir[idx];
        mask ^= (1 << last);
        last = prevLast;
        dir = prevDir;
    }

    vector<int> newOrder = base.order;
    for (int i = 0; i < kNum; i++) {
        newOrder[blockStart + i] = newBlock[i];
    }

    OrderState newState = make_order_state(cache, newOrder);
    if (!better_eval(newState.eval, base.eval)) return finish(false);

    improved = BestSnapshot{newOrder, base.cache, newState.eval};
    return finish(true);
}

bool improve_order_blocks_exact_dp(
    const TransitionCache& cache,
    const BestSnapshot& base,
    BestSnapshot& improved,
    BestSnapshot* deterministicImproved,
    XorShift& rng,
    chrono::steady_clock::time_point deadline,
    BlockExactDpLog* log
) {
    if (!ENABLE_BLOCK_EXACT_DP_FINISH) return false;
    auto startTime = chrono::steady_clock::now();

    auto finish = [&](bool ok) {
        if (log != nullptr) {
            log->time = chrono::duration<double>(
                chrono::steady_clock::now() - startTime
            ).count();
        }
        return ok;
    };

    BestSnapshot cur = base;
    int improvedBlocks = 0;
    int deterministicImprovedBlocks = 0;
    const int completed = base.eval.completed;
    if (log != nullptr) log->k = BLOCK_EXACT_DP_TARGETS;

    for (int blockStart = 0; blockStart < completed; blockStart += BLOCK_EXACT_DP_TARGETS) {
        if (chrono::steady_clock::now() >= deadline) {
            if (log != nullptr) log->timeout = true;
            break;
        }

        if (log != nullptr) log->blocks++;
        BestSnapshot blockImproved;
        BlockExactDpLog blockLog;
        bool ok = improve_order_block_exact_dp(
            cache,
            cur,
            blockStart,
            BLOCK_EXACT_DP_TARGETS,
            blockImproved,
            deadline,
            &blockLog
        );
        if (blockLog.timeout) {
            if (log != nullptr) log->timeout = true;
            break;
        }
        if (!ok) continue;

        cur = std::move(blockImproved);
        improvedBlocks++;
        deterministicImprovedBlocks++;
    }

    if (deterministicImproved != nullptr) {
        *deterministicImproved = cur;
    }

    while (chrono::steady_clock::now() < deadline) {
        const int curCompleted = cur.eval.completed;
        const int maxLen = min(BLOCK_EXACT_DP_TARGETS, curCompleted);
        if (maxLen <= 1) break;

        int blockLen = 2 + rng.nextInt(maxLen - 1);
        int blockStart = rng.nextInt(curCompleted - blockLen + 1);

        if (log != nullptr) log->randomBlocks++;
        BestSnapshot blockImproved;
        BlockExactDpLog blockLog;
        bool ok = improve_order_block_exact_dp(
            cache,
            cur,
            blockStart,
            blockLen,
            blockImproved,
            deadline,
            &blockLog
        );
        if (blockLog.timeout) {
            if (log != nullptr) log->timeout = true;
            break;
        }
        if (!ok) continue;

        cur = std::move(blockImproved);
        improvedBlocks++;
        if (log != nullptr) log->randomImprovedBlocks++;
    }

    if (log != nullptr) {
        log->improvedBlocks = improvedBlocks;
        log->deterministicImprovedBlocks = deterministicImprovedBlocks;
    }
    improved = std::move(cur);
    return finish(improvedBlocks > 0);
}

// 初期解候補用の Fisher-Yates shuffle。
void shuffle_order(vector<int>& order, XorShift& rng) {
    for (int i = (int)order.size() - 1; i > 0; i--) {
        int j = rng.nextInt(i + 1);
        swap(order[i], order[j]);
    }
}

// 壁ありのマス間距離。万一到達不能なら大きめの値として扱う。
int cell_distance(int from, int to) {
    if (!cellDist.empty()) {
        uint16_t d = cellDist[from][to];
        if (d != INF) return d;
    }

    int fi = from / N;
    int fj = from % N;
    int ti = to / N;
    int tj = to % N;
    return abs(fi - ti) + abs(fj - tj) + N * N;
}

// order[pos] を処理し始めるまでの「移動の接続」距離。
// pos=0 は初期位置 -> 最初のボール、それ以外は直前の箱 -> 次のボール。
int order_cut_distance(const array<int, MAX_M>& order, int pos) {
    if (pos < 0 || pos >= M) return 0;

    int from = (pos == 0) ? START_POS : basketPos[order[pos - 1]];
    int to = ballPos[order[pos]];
    return cell_distance(from, to);
}

// swap で触る位置は、その前後の接続が長いほど選ばれやすくする。
// 厳密な差分利得ではなく、悪そうな場所を優先して壊すための軽い重み。
int order_swap_position_weight(const array<int, MAX_M>& order, int pos) {
    int d = order_cut_distance(order, pos);
    if (pos + 1 < M) d += order_cut_distance(order, pos + 1);
    return 1 + d * d;
}

int pick_weighted_swap_position(const array<int, MAX_M>& order, XorShift& rng, int banned = -1) {
    array<int, MAX_M> weight{};
    long long total = 0;

    for (int i = 0; i < M; i++) {
        if (i == banned) continue;
        weight[i] = order_swap_position_weight(order, i);
        total += weight[i];
    }

    if (total <= 0) {
        if (banned == -1) return rng.nextInt(M);
        int x = rng.nextInt(M - 1);
        if (x >= banned) x++;
        return x;
    }

    long long r = (long long)(rng.next64() % (uint64_t)total);
    for (int i = 0; i < M; i++) {
        if (i == banned) continue;
        if (r < weight[i]) return i;
        r -= weight[i];
    }
    for (int i = 0; i < M; i++) {
        if (i != banned) return i;
    }
    return 0;
}

// SAで使うボール順序の近傍。
// weighted swap / random swap / adjacent swap / reverse / single insert に加え、
// 短い連続区間を動かす block insert も使う。
// changedFrom には、prefix DP を作り直すべき最初の位置を返す。
bool mutate_order_array(array<int, MAX_M>& order, XorShift& rng, int& changedFrom) {
    int n = M;
    if (n < 2) return false;

    int moveType = rng.nextInt(100);
    const int weightedSwapLimit = SA_WEIGHTED_SWAP_MOVE_PERCENT;
    const int swapLimit = weightedSwapLimit + SA_SWAP_MOVE_PERCENT;
    const int adjacentSwapLimit = swapLimit + SA_ADJACENT_SWAP_MOVE_PERCENT;
    const int reverseLimit = adjacentSwapLimit + SA_REVERSE_MOVE_PERCENT;
    const int insertLimit = reverseLimit + SA_INSERT_MOVE_PERCENT;

    if (moveType < weightedSwapLimit) {
        int a = pick_weighted_swap_position(order, rng);
        int b = pick_weighted_swap_position(order, rng, a);
        swap(order[a], order[b]);
        changedFrom = min(a, b);
        return true;
    }

    if (moveType < swapLimit) {
        int a = rng.nextInt(n);
        int b = rng.nextInt(n);
        if (a == b) return false;
        swap(order[a], order[b]);
        changedFrom = min(a, b);
        return true;
    }

    if (moveType < adjacentSwapLimit) {
        int i = rng.nextInt(n - 1);
        swap(order[i], order[i + 1]);
        changedFrom = i;
        return true;
    }

    if (moveType < reverseLimit) {
        int l = rng.nextInt(n);
        int r = rng.nextInt(n);
        if (l > r) swap(l, r);
        if (l == r) return false;
        reverse(order.begin() + l, order.begin() + r + 1);
        changedFrom = l;
        return true;
    }

    if (moveType >= insertLimit) {
        int maxLen = min(SA_BLOCK_INSERT_MAX_LEN, n - 1);
        if (maxLen < 2) return false;

        int len = 2 + rng.nextInt(maxLen - 1);
        int l = rng.nextInt(n - len + 1);
        int r = l + len;
        int to = rng.nextInt(n - len + 1);
        if (to == l) return false;

        array<int, MAX_M> old = order;
        int idx = 0;
        if (to < l) {
            for (int i = 0; i < to; i++) order[idx++] = old[i];
            for (int i = l; i < r; i++) order[idx++] = old[i];
            for (int i = to; i < l; i++) order[idx++] = old[i];
            for (int i = r; i < n; i++) order[idx++] = old[i];
        } else {
            for (int i = 0; i < l; i++) order[idx++] = old[i];
            for (int i = r; i < to + len; i++) order[idx++] = old[i];
            for (int i = l; i < r; i++) order[idx++] = old[i];
            for (int i = to + len; i < n; i++) order[idx++] = old[i];
        }

        changedFrom = min(l, to);
        return true;
    }

    int from = rng.nextInt(n);
    int to = rng.nextInt(n);
    if (from == to) return false;

    int x = order[from];
    if (from < to) {
        for (int i = from; i < to; i++) order[i] = order[i + 1];
        order[to] = x;
    } else {
        for (int i = from; i > to; i--) order[i] = order[i - 1];
        order[to] = x;
    }
    changedFrom = min(from, to);
    return true;
}

// 現在位置から見て一番安い未処理ボールを貪欲に選び、SAの初期順序にする。
vector<int> make_greedy_order(const TransitionCache& cache) {
    vector<int> order;
    vector<char> used(M, 0);
    order.reserve(M);

    array<CostState, 4> dp = initial_dir_costs(cache);

    for (int step = 0; step < M; step++) {
        int prev = order.empty() ? M : order.back();
        int bestK = -1;
        array<CostState, 4> bestDp = empty_dir_costs();
        CostState bestCost;

        for (int k = 0; k < M; k++) {
            if (used[k]) continue;

            array<CostState, 4> ndp = advance_order_dp(cache, dp, prev, k);
            int candDir = best_dir(ndp);
            if (candDir == -1) continue;

            if (ndp[candDir].val < bestCost.val) {
                bestK = k;
                bestDp = ndp;
                bestCost = ndp[candDir];
            }
        }

        if (bestK == -1) {
            // ここから先はこの貪欲DPでは到達不能。SA用の順列としては残りを任意順で埋める。
            for (int k = 0; k < M; k++) {
                if (!used[k]) order.push_back(k);
            }
            break;
        }

        dp = bestDp;
        used[bestK] = 1;
        order.push_back(bestK);
    }

    return order;
}

// 評価で選ばれた順序とマクロに対して、登録前の仮想 P を含む経路を復元する。
// registrationExtraBtn は「最初の P を登録列に置き換える差分」として、DP上だけ先に足す。
string build_raw_buttons_for_order(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    int initialState,
    int initialExtraBtn,
    int initialExtraPhys,
    int registrationExtraBtn
) {
    steps = min(steps, (int)order.size());
    if (steps <= 0) return "";

    vector<array<CostState, 4>> dp(steps + 1);
    for (auto& row : dp) {
        for (auto& s : row) s = CostState{};
    }

    struct PrevChoice {
        int prevDir = -1;
        int midDir = -1;
    };

    vector<array<PrevChoice, 4>> pre(steps + 1);
    int initialDir = initialState % 4;
    dp[0][initialDir] = CostState{
        initialExtraBtn + registrationExtraBtn,
        initialExtraPhys
    };

    for (int i = 0; i < steps; i++) {
        int prev = (i == 0) ? M : order[i - 1];
        int k = order[i];

        for (int sd = 0; sd < 4; sd++) {
            if (dp[i][sd].btn() >= INF) continue;

            TransList firstList;
            const TransList* listPtr = nullptr;
            if (i == 0) {
                if (initialState == sid(START_POS, START_DIR) &&
                    initialExtraBtn == 0 &&
                    initialExtraPhys == 0 &&
                    registrationExtraBtn == 0 &&
                    sd == START_DIR) {
                    listPtr = &cache.get_initial(k);
                } else {
                    cache.build_from_state(initialState, k, firstList);
                    listPtr = &firstList;
                }
            } else {
                listPtr = &cache.get(prev, k, sd);
            }
            const TransList& list = *listPtr;
            for (int oi = 0; oi < list.count; oi++) {
                const TransOpt& opt = list.items[oi];
                int nPhys = dp[i][sd].phys() + opt.addPhys;
                if (nPhys > safe_turn_limit()) continue;

                int nBtn = dp[i][sd].btn() + opt.addBtn;
                if (nBtn > safe_turn_limit()) continue;

                if (relax_cost(dp[i + 1][opt.endDir], nBtn, nPhys)) {
                    pre[i + 1][opt.endDir] = PrevChoice{sd, opt.midDir};
                }
            }
        }
    }

    int endDir = best_dir(dp[steps]);

    if (endDir == -1) return "";

    vector<int> midDirs(steps);
    vector<int> endDirs(steps);

    int curDirState = endDir;
    for (int i = steps; i >= 1; i--) {
        const PrevChoice choice = pre[i][curDirState];
        endDirs[i - 1] = curDirState;
        midDirs[i - 1] = choice.midDir;

        curDirState = choice.prevDir;
        if (curDirState < 0) curDirState = 1;
    }

    string out;
    int curPos = initialState / 4;
    int curDir = initialState % 4;

    for (int i = 0; i < steps; i++) {
        int k = order[i];
        int bp = ballPos[k];
        int cp = basketPos[k];
        int bd = midDirs[i];
        int cd = endDirs[i];

        out += build_macro_body_move_buttons(cache.macroId, sid(curPos, curDir), sid(bp, bd));
        out.push_back('S');

        curPos = bp;
        curDir = bd;

        out += build_macro_body_move_buttons(cache.macroId, sid(curPos, curDir), sid(cp, cd));
        out.push_back('S');

        curPos = cp;
        curDir = cd;
    }

    return out;
}

vector<MacroSample> collect_macro_samples(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    XorShift& rng
) {
    steps = min(steps, (int)order.size());
    if (steps <= 0) return {};

    vector<array<CostState, 4>> dp(steps + 1);
    for (auto& row : dp) {
        for (auto& s : row) s = CostState{};
    }

    struct PrevChoice {
        int prevDir = -1;
        int midDir = -1;
    };

    vector<array<PrevChoice, 4>> pre(steps + 1);
    dp[0] = initial_dir_costs(cache);

    for (int i = 0; i < steps; i++) {
        int prev = (i == 0) ? M : order[i - 1];
        int k = order[i];

        for (int sd = 0; sd < 4; sd++) {
            if (dp[i][sd].btn() >= INF) continue;

            if (prev == M && sd != START_DIR) continue;

            const TransList& list = get_order_transition_list(cache, prev, k, sd);
            for (int oi = 0; oi < list.count; oi++) {
                const TransOpt& opt = list.items[oi];
                int nPhys = dp[i][sd].phys() + opt.addPhys;
                if (nPhys > safe_turn_limit()) continue;

                int nBtn = dp[i][sd].btn() + opt.addBtn;
                if (nBtn > safe_turn_limit()) continue;

                if (relax_cost(dp[i + 1][opt.endDir], nBtn, nPhys)) {
                    pre[i + 1][opt.endDir] = PrevChoice{sd, opt.midDir};
                }
            }
        }
    }

    int endDir = best_dir(dp[steps]);
    if (endDir == -1) return {};

    vector<int> midDirs(steps);
    vector<int> endDirs(steps);

    int curDirState = endDir;
    for (int i = steps; i >= 1; i--) {
        const PrevChoice choice = pre[i][curDirState];
        if (choice.prevDir < 0 || choice.midDir < 0) return {};
        endDirs[i - 1] = curDirState;
        midDirs[i - 1] = choice.midDir;
        curDirState = choice.prevDir;
    }

    vector<MacroSample> samples;
    samples.reserve(steps * 2);

    int curPos = START_POS;
    int curDir = START_DIR;

    for (int i = 0; i < steps; i++) {
        int k = order[i];
        int bp = ballPos[k];
        int cp = basketPos[k];
        int bd = midDirs[i];
        int cd = endDirs[i];

        int s1 = sid(curPos, curDir);
        int t1 = sid(bp, bd);
        if (s1 != t1) samples.push_back(MacroSample{s1, t1, 1});

        int s2 = sid(bp, bd);
        int t2 = sid(cp, cd);
        if (s2 != t2) samples.push_back(MacroSample{s2, t2, 1});

        curPos = cp;
        curDir = cd;
    }

    if ((int)samples.size() > SA_MACRO_SAMPLE_COUNT) {
        for (int i = 0; i < SA_MACRO_SAMPLE_COUNT; i++) {
            int j = i + rng.nextInt((int)samples.size() - i);
            swap(samples[i], samples[j]);
        }
        samples.resize(SA_MACRO_SAMPLE_COUNT);
    }

    return samples;
}

// サンプル評価専用の no-path 層BFS。
// frontier ごとに btn は共通なので、次層の btn をループ外でまとめて扱う。
void run_sample_macro_bfs(
    const vector<StateId>& macroNext,
    int macroPhys,
    int startState,
    int targetState,
    vector<uint32_t>& dist
) {
    if ((int)dist.size() != SNUM) dist.resize(SNUM);

    fill(dist.begin(), dist.end(), CostState::INF_VAL);
    auto* current = &sampleCurrentScratch;
    auto* next = &sampleNextScratch;
    int currentCount = 1;

    dist[startState] = CostState::pack(0, 0);
    (*current)[0] = (StateId)startState;

    const uint32_t infVal = CostState::INF_VAL;
    int layerBtn = 0;

    while (currentCount > 0 && dist[targetState] == infVal) {
        int nextCount = 0;
        int nextBtn = layerBtn + 1;
        uint32_t nextBtnHi = ((uint32_t)nextBtn) << 16;

        for (int idx = 0; idx < currentCount; idx++) {
            StateId cur = (*current)[idx];
            int curPhys = cost_phys(dist[cur]);
            {
                StateId ns = macroNext[cur];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + macroPhys);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
            {
                StateId ns = primitiveNextState[cur][0];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + 1);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
            {
                StateId ns = primitiveNextState[cur][1];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + 1);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
            {
                StateId ns = primitiveNextState[cur][2];
                if (ns != cur) {
                    uint32_t nval = nextBtnHi | CostState::clamp_component(curPhys + 1);
                    uint32_t oldVal = dist[ns];
                    if (nval < oldVal) {
                        dist[ns] = nval;
                        if ((oldVal >> 16) > (uint32_t)nextBtn) {
                            (*next)[nextCount++] = ns;
                        }
                    }
                }
            }
        }

        swap(current, next);
        currentCount = nextCount;
        layerBtn = nextBtn;
    }
}

// 呼び出し元は normalize 済みマクロ本体を渡す前提。
long long score_macro_body_on_samples(const string& body, const vector<MacroSample>& samples) {
    if (samples.empty()) return 0;

    vector<StateId>& macroNext = sampleMacroNextScratch;
    build_macro_body_next_vector(body, macroNext);

    vector<uint32_t>& dist = sampleDistScratch;

    long long score = 0;
    const int macroPhys = (int)body.size();

    for (const MacroSample& sample : samples) {
        run_sample_macro_bfs(
            macroNext,
            macroPhys,
            sample.startState,
            sample.targetState,
            dist
        );

        uint32_t c = dist[sample.targetState];
        if (cost_btn(c) >= INF) {
            score += sample.weight * 1LL * INF * SA_MACRO_SAMPLE_BTN_WEIGHT;
        } else {
            score += sample.weight * 1LL *
                (cost_btn(c) * SA_MACRO_SAMPLE_BTN_WEIGHT +
                 cost_phys(c) * SA_MACRO_SAMPLE_PHYS_WEIGHT);
        }
    }

    return score;
}

vector<string> improve_macro_body_pool_on_samples(
    const string& currentBody,
    const vector<MacroSample>& samples,
    XorShift& rng,
    const vector<InitialPathSource>& pathPool
) {
    const int innerMinLen = max(
        1,
        (int)currentBody.size() - SA_MACRO_INNER_SHRINK_LIMIT
    );
    const int innerMaxLen = min(
        SA_MACRO_MAX_LEN,
        (int)currentBody.size() + SA_MACRO_INNER_GROW_LIMIT
    );

    if (samples.empty() || SA_MACRO_SAMPLE_INNER_STEPS <= 0) {
        return {mutate_macro_body(currentBody, rng, innerMaxLen, innerMinLen)};
    }

    vector<pair<long long, string>> pool;
    unordered_map<string, long long> sampleScoreCache;
    sampleScoreCache.reserve(SA_MACRO_SAMPLE_INNER_STEPS * 2 + 8);

    auto cached_sample_score = [&](const string& body) -> long long {
        auto it = sampleScoreCache.find(body);
        if (it != sampleScoreCache.end()) return it->second;

        long long score = score_macro_body_on_samples(body, samples);
        sampleScoreCache.emplace(body, score);
        return score;
    };

    auto add_pool = [&](const string& body, long long score) {
        if (body == currentBody) return;
        for (const auto& item : pool) {
            if (item.second == body) return;
        }
        pool.push_back({score, body});
        sort(pool.begin(), pool.end(), [](const auto& a, const auto& b) {
            if (a.first != b.first) return a.first < b.first;
            return a.second.size() < b.second.size();
        });
        if ((int)pool.size() > SA_MACRO_POOL_EVAL_COUNT) {
            pool.resize(SA_MACRO_POOL_EVAL_COUNT);
        }
    };

    string cur = currentBody;
    long long curScore = cached_sample_score(cur);

    for (int step = 0; step < SA_MACRO_SAMPLE_INNER_STEPS; step++) {
        double rate = (SA_MACRO_SAMPLE_INNER_STEPS <= 1)
            ? 1.0
            : (double)step / (double)(SA_MACRO_SAMPLE_INNER_STEPS - 1);
        double temp = SA_MACRO_SAMPLE_START_TEMP *
            pow(SA_MACRO_SAMPLE_END_TEMP / SA_MACRO_SAMPLE_START_TEMP, rate);

        string cand;
        if (!pathPool.empty() &&
            rng.nextInt(100) < SA_MACRO_PATH_SLICE_PERCENT) {
            cand = make_path_slice_macro_body(pathPool, (int)cur.size(), rng);
        }
        if (cand.empty()) {
            cand = mutate_macro_body(cur, rng, innerMaxLen, innerMinLen);
        }
        long long candScore = cached_sample_score(cand);
        add_pool(cand, candScore);

        long long diff = candScore - curScore;

        bool accept = false;
        if (diff <= 0) {
            accept = true;
        } else if ((double)diff < 60.0 * temp) {
            accept = exp(-(double)diff / temp) > rng.nextDouble();
        }

        if (accept) {
            cur = std::move(cand);
            curScore = candScore;
        }
    }

    if (pool.empty()) {
        return {mutate_macro_body(currentBody, rng, innerMaxLen, innerMinLen)};
    }

    vector<string> ret;
    ret.reserve(pool.size());
    for (auto& item : pool) {
        ret.push_back(std::move(item.second));
    }
    return ret;
}

string replace_first_p_with_compressed_registration(
    const string& buttons,
    const string& body,
    RegistrationCompressionLog* log = nullptr
) {
    string normal = replace_first_p_with_registration(buttons, body);
    if (log != nullptr) {
        *log = RegistrationCompressionLog{};
        log->normalLen = (int)normal.size();
        log->compressedLen = (int)normal.size();
    }

    // body 内に P があると、圧縮用 P と区別できないのでこの後処理は使わない。
    if (body.find('P') != string::npos) return normal;

    string best = normal;
    RegistrationCompressionLog bestLog;

    for (int runLen : F_RUN_COMPRESS_RUN_LENGTHS) {
        RegistrationCompressionLog candLog;
        string cand = try_replace_first_p_with_f_run_compression(
            buttons,
            body,
            runLen,
            normal,
            &candLog
        );

        if (candLog.used && cand.size() < best.size()) {
            best = std::move(cand);
            bestLog = candLog;
        }
    }

    if (bestLog.used && log != nullptr) {
        *log = bestLog;
    }

    return best;
}

struct EdgeMacroBoardSim {
    array<uint8_t, 400> ballsAt{};
    uint8_t heldBall = 0;
    int state = 0;

    EdgeMacroBoardSim() : heldBall(0), state(sid(START_POS, START_DIR)) {
        ballsAt.fill(0);
        for (int k = 0; k < M; k++) {
            ballsAt[ballPos[k]]++;
        }
    }

    bool s_would_change_board() const {
        int pos = state >> 2;
        return heldBall || ballsAt[pos] != 0;
    }

    void press_s() {
        int pos = state >> 2;
        if (heldBall) {
            ballsAt[pos]++;
            heldBall = 0;
        } else if (ballsAt[pos] != 0) {
            ballsAt[pos]--;
            heldBall = 1;
        }
    }
};

void append_turns_and_apply(const string& raw, int l, int r, string& out, int& state) {
    for (int i = l; i < r; i++) {
        out.push_back(raw[i]);
        state = apply_button_state(state, raw[i]);
    }
}

void append_raw_segment_and_apply(
    const string& raw,
    int l,
    int r,
    string& out,
    EdgeMacroBoardSim& sim
) {
    for (int i = l; i < r; i++) {
        char ch = raw[i];
        out.push_back(ch);
        if (ch == 'S') {
            sim.press_s();
        } else if (ch == 'F' || ch == 'R' || ch == 'L') {
            sim.state = apply_button_state(sim.state, ch);
        }
    }
}

vector<EdgeSMacroUseInfo> analyze_edge_s_macro_uses(
    const string& raw,
    const string& movementBody
) {
    vector<EdgeSMacroUseInfo> uses(1);
    EdgeMacroBoardSim sim;

    for (int i = 0; i < (int)raw.size(); i++) {
        char ch = raw[i];
        if (ch == 'S') {
            sim.press_s();
            continue;
        }
        if (ch == 'F' || ch == 'R' || ch == 'L') {
            sim.state = apply_button_state(sim.state, ch);
            continue;
        }
        if (ch != 'P') continue;

        EdgeSMacroUseInfo info;
        info.pPos = i;

        int l = i - 1;
        while (l >= 0 && (raw[l] == 'L' || raw[l] == 'R')) l--;
        if (l >= 0 && raw[l] == 'S') {
            info.prefixAbsorbPos = l;
        }
        info.prefixGuard = sim.s_would_change_board();

        int afterBodyState = apply_macro_body_state(sim.state, movementBody);
        int r = i + 1;
        while (r < (int)raw.size() && (raw[r] == 'L' || raw[r] == 'R')) r++;
        if (r < (int)raw.size() && raw[r] == 'S') {
            info.postfixAbsorbPos = r;
        }

        sim.state = afterBodyState;
        info.postfixGuard = sim.s_would_change_board();
        sim.state = afterBodyState;

        uses.push_back(info);
    }

    return uses;
}

struct EdgeSSwitchChoice {
    bool valid = false;
    bool toPrefix = false;
    int mode = EDGE_S_PREFIX;
    int switchUse = 0;
    int switchBackUse = 0;
    int noBackBestLen = INF;
    int switchBackSaved = 0;
    int totalUses = 0;
    int candidateLen = INF;
};

EdgeSSwitchChoice find_best_edge_s_switch_choice(
    const string& raw,
    const vector<EdgeSMacroUseInfo>& uses,
    int inputLen
) {
    EdgeSSwitchChoice best;
    best.totalUses = max(0, (int)uses.size() - 1);
    if (best.totalUses < 2) return best;

    vector<int> prefixDelta(best.totalUses + 1, 0);
    vector<int> postfixDelta(best.totalUses + 1, 0);
    vector<int> bothPrefixDelta(best.totalUses + 1, 0);
    vector<int> bothPostfixDelta(best.totalUses + 1, 0);
    vector<int> bothBoundaryDelta(best.totalUses + 1, 0);
    vector<int> prefixSuffix(best.totalUses + 3, 0);
    vector<int> postfixSuffix(best.totalUses + 3, 0);
    vector<int> bothSuffix(best.totalUses + 3, 0);
    for (int i = best.totalUses; i >= 1; i--) {
        prefixDelta[i] = (uses[i].prefixAbsorbPos != -1) ? -1 : (uses[i].prefixGuard ? 1 : 0);
        postfixDelta[i] = (uses[i].postfixAbsorbPos != -1) ? -1 : (uses[i].postfixGuard ? 1 : 0);
        bothPrefixDelta[i] = prefixDelta[i];
        bothPostfixDelta[i] = postfixDelta[i];
        prefixSuffix[i] = prefixDelta[i] + prefixSuffix[i + 1];
        postfixSuffix[i] = postfixDelta[i] + postfixSuffix[i + 1];
    }
    for (int i = best.totalUses - 1; i >= 1; i--) {
        int nextPos = next_non_turn_pos(raw, uses[i].pPos + 1);
        if (nextPos == uses[i + 1].pPos) {
            bothBoundaryDelta[i] = -bothPostfixDelta[i] - bothPrefixDelta[i + 1];
        } else if (uses[i].postfixAbsorbPos != -1 &&
                   uses[i].postfixAbsorbPos == uses[i + 1].prefixAbsorbPos) {
            int desiredPrefix = uses[i + 1].prefixGuard ? 1 : 0;
            bothBoundaryDelta[i] = desiredPrefix - bothPrefixDelta[i + 1];
        }
    }
    for (int i = best.totalUses; i >= 1; i--) {
        bothSuffix[i] = bothPrefixDelta[i] + bothPostfixDelta[i] + bothBoundaryDelta[i] + bothSuffix[i + 1];
    }

    auto consider = [&](int mode, int switchUse, int switchBackUse, int switchBackSaved, int candidateLen) {
        if (!best.valid || candidateLen < best.candidateLen ||
            (candidateLen == best.candidateLen &&
             (switchUse < best.switchUse ||
              (switchUse == best.switchUse && mode < best.mode)))) {
            best.valid = true;
            best.mode = mode;
            best.toPrefix = (mode == EDGE_S_PREFIX);
            best.switchUse = switchUse;
            best.switchBackUse = switchBackUse;
            best.switchBackSaved = switchBackSaved;
            best.candidateLen = candidateLen;
        }
    };

    int noBackBestLen = INF;
    for (int i = 2; i <= best.totalUses; i++) {
        int prefixNoBackLen = inputLen + 3 + prefixSuffix[i];
        int postfixNoBackLen = inputLen + 3 + postfixSuffix[i];
        int bothNoBackLen = inputLen + 4 + bothSuffix[i];
        noBackBestLen = min(noBackBestLen, min(prefixNoBackLen, min(postfixNoBackLen, bothNoBackLen)));
        consider(EDGE_S_PREFIX, i, 0, 0, prefixNoBackLen);
        consider(EDGE_S_POSTFIX, i, 0, 0, postfixNoBackLen);
        consider(EDGE_S_BOTH, i, 0, 0, bothNoBackLen);
    }

    vector<int> prefixPrefix(best.totalUses + 1, 0);
    vector<int> postfixPrefix(best.totalUses + 1, 0);
    vector<int> bothPrefix(best.totalUses + 1, 0);
    for (int i = 1; i <= best.totalUses; i++) {
        prefixPrefix[i] = prefixPrefix[i - 1] + prefixDelta[i];
        postfixPrefix[i] = postfixPrefix[i - 1] + postfixDelta[i];
        bothPrefix[i] = bothPrefix[i - 1] + bothPrefixDelta[i] + bothPostfixDelta[i] + bothBoundaryDelta[i];
    }

    for (int l = 2; l <= best.totalUses - 1; l++) {
        for (int r = l; r <= best.totalUses - 1; r++) {
            int prefixSum = prefixPrefix[r] - prefixPrefix[l - 1];
            int postfixSum = postfixPrefix[r] - postfixPrefix[l - 1];
            int bothSum = bothPrefix[r] - bothPrefix[l - 1];
            if (r < best.totalUses) bothSum -= bothBoundaryDelta[r];
            int prefixWithBackLen = inputLen + 6 + prefixSum;
            int postfixWithBackLen = inputLen + 6 + postfixSum;
            int bothWithBackLen = inputLen + 8 + bothSum;
            consider(EDGE_S_PREFIX, l, r + 1, noBackBestLen - prefixWithBackLen, prefixWithBackLen);
            consider(EDGE_S_POSTFIX, l, r + 1, noBackBestLen - postfixWithBackLen, postfixWithBackLen);
            consider(EDGE_S_BOTH, l, r + 1, noBackBestLen - bothWithBackLen, bothWithBackLen);
        }
    }

    best.noBackBestLen = noBackBestLen;

    return best;
}

string build_s_switch_macro_candidate(
    const string& raw,
    const string& registerBody,
    const string& movementBody,
    const vector<EdgeSMacroUseInfo>& uses,
    const EdgeSSwitchChoice& choice,
    EdgeSSwitchLog* log
) {
    if (log != nullptr) {
        *log = EdgeSSwitchLog{};
        log->tried = true;
        log->toPrefix = choice.toPrefix;
        log->mode = choice.mode;
        log->totalUses = choice.totalUses;
        log->switchUse = choice.switchUse;
        log->switchBackUse = choice.switchBackUse;
        log->noBackBestLen = choice.noBackBestLen;
        log->switchBackSaved = choice.switchBackSaved;
    }
    if (!choice.valid) return "";

    string out;
    out.reserve(raw.size() + registerBody.size() + 16);

    EdgeMacroBoardSim sim;
    bool registered = false;
    int prevPos = 0;
    int absorbedS = 0;
    int guardedP = 0;
    int freeP = 0;

    auto emit_normal_use = [&](bool switchBackNow) {
        if (!registered) {
            out.push_back('M');
            out += registerBody;
            out.push_back('M');
            registered = true;
        } else if (switchBackNow) {
            out += choice.toPrefix ? "MSPM" : "MPSM";
        } else {
            out.push_back('P');
        }
        sim.state = apply_macro_body_state(sim.state, movementBody);
    };

    if (choice.mode == EDGE_S_BOTH) {
        bool pendingPostfix = false;

        auto settle_pending_postfix = [&]() {
            if (!pendingPostfix) return;
            if (sim.s_would_change_board()) {
                sim.press_s();
                out.push_back('S');
                sim.press_s();
                guardedP++;
            } else {
                sim.press_s();
                freeP++;
            }
            pendingPostfix = false;
        };

        auto append_both_segment = [&](int l, int r, int absorbPPos, bool& prefixAbsorbed) {
            for (int i = l; i < r; i++) {
                char ch = raw[i];
                if (ch == 'S') {
                    if (pendingPostfix) {
                        sim.press_s();
                        pendingPostfix = false;
                        absorbedS++;
                        continue;
                    }
                    int j = next_non_turn_pos(raw, i + 1);
                    if (absorbPPos != -1 && j == absorbPPos) {
                        sim.press_s();
                        prefixAbsorbed = true;
                        absorbedS++;
                        continue;
                    }
                    out.push_back('S');
                    sim.press_s();
                    continue;
                }
                if (ch == 'F') {
                    settle_pending_postfix();
                }
                out.push_back(ch);
                if (ch == 'F' || ch == 'R' || ch == 'L') {
                    sim.state = apply_button_state(sim.state, ch);
                }
            }
        };

        auto emit_both_use = [&](bool switchNow, bool prefixAbsorbed) {
            if (pendingPostfix) {
                pendingPostfix = false;
            } else if (prefixAbsorbed) {
                // 元 S の効果は append_both_segment() で反映済み。
            } else if (sim.s_would_change_board()) {
                out.push_back('S');
                sim.press_s();
                sim.press_s();
                guardedP++;
            } else {
                sim.press_s();
                freeP++;
            }

            if (switchNow) {
                out += "MSPSM";
            } else {
                out.push_back('P');
            }
            sim.state = apply_macro_body_state(sim.state, movementBody);
            pendingPostfix = true;
        };

        for (int use = 1; use <= choice.totalUses; use++) {
            const EdgeSMacroUseInfo& info = uses[use];
            bool switchNow = (use == choice.switchUse);
            bool switchBackNow = (choice.switchBackUse != 0 && use == choice.switchBackUse);
            bool switched = (use > choice.switchUse) &&
                (choice.switchBackUse == 0 || use < choice.switchBackUse);

            if (!switched && !switchNow) {
                if (switchBackNow) {
                    bool dummyPrefixAbsorbed = false;
                    append_both_segment(prevPos, info.pPos, -1, dummyPrefixAbsorbed);
                    settle_pending_postfix();
                    out += "MSPSM";
                    sim.state = apply_macro_body_state(sim.state, movementBody);
                    pendingPostfix = false;
                    prevPos = info.pPos + 1;
                } else {
                    append_raw_segment_and_apply(raw, prevPos, info.pPos, out, sim);
                    emit_normal_use(false);
                    prevPos = info.pPos + 1;
                }
                continue;
            }

            bool prefixAbsorbed = false;
            append_both_segment(prevPos, info.pPos, info.pPos, prefixAbsorbed);
            emit_both_use(switchNow, prefixAbsorbed);
            prevPos = info.pPos + 1;
        }

        if (choice.switchBackUse == 0) {
            bool dummyPrefixAbsorbed = false;
            append_both_segment(prevPos, (int)raw.size(), -1, dummyPrefixAbsorbed);
            settle_pending_postfix();
        } else {
            append_raw_segment_and_apply(raw, prevPos, (int)raw.size(), out, sim);
        }

        if (!registered) return "";

        if (log != nullptr) {
            log->candidateLen = (int)out.size();
            log->absorbedS = absorbedS;
            log->guardedP = guardedP;
            log->freeP = freeP;
        }

        return out;
    }

    for (int use = 1; use <= choice.totalUses; use++) {
        const EdgeSMacroUseInfo& info = uses[use];
        bool switchNow = (use == choice.switchUse);
        bool switchBackNow = (choice.switchBackUse != 0 && use == choice.switchBackUse);
        bool switched = (use > choice.switchUse) &&
            (choice.switchBackUse == 0 || use < choice.switchBackUse);

        if (!switched && !switchNow) {
            append_raw_segment_and_apply(raw, prevPos, info.pPos, out, sim);
            emit_normal_use(switchBackNow);
            prevPos = info.pPos + 1;
            continue;
        }

        if (choice.toPrefix) {
            if (info.prefixAbsorbPos != -1) {
                append_raw_segment_and_apply(raw, prevPos, info.prefixAbsorbPos, out, sim);
                sim.press_s();
                append_turns_and_apply(raw, info.prefixAbsorbPos + 1, info.pPos, out, sim.state);
                if (switchNow) {
                    out += "MSPM";
                } else {
                    out.push_back('P');
                }
                sim.state = apply_macro_body_state(sim.state, movementBody);
                absorbedS++;
            } else {
                append_raw_segment_and_apply(raw, prevPos, info.pPos, out, sim);
                if (info.prefixGuard) {
                    out.push_back('S');
                    guardedP++;
                } else {
                    freeP++;
                }
                if (switchNow) {
                    out += "MSPM";
                } else {
                    out.push_back('P');
                }
                sim.state = apply_macro_body_state(sim.state, movementBody);
            }
            prevPos = info.pPos + 1;
            continue;
        }

        append_raw_segment_and_apply(raw, prevPos, info.pPos, out, sim);
        int afterBodyState = apply_macro_body_state(sim.state, movementBody);
        if (switchNow) {
            out += "MPSM";
        } else {
            out.push_back('P');
        }

        if (info.postfixAbsorbPos != -1) {
            sim.state = afterBodyState;
            sim.press_s();
            append_turns_and_apply(raw, info.pPos + 1, info.postfixAbsorbPos, out, sim.state);
            absorbedS++;
            prevPos = info.postfixAbsorbPos + 1;
        } else {
            sim.state = afterBodyState;
            if (info.postfixGuard) {
                sim.press_s();
                out.push_back('S');
                sim.press_s();
                guardedP++;
            } else {
                sim.press_s();
                freeP++;
            }
            prevPos = info.pPos + 1;
        }
    }

    append_raw_segment_and_apply(raw, prevPos, (int)raw.size(), out, sim);

    if (!registered) return "";

    if (log != nullptr) {
        log->candidateLen = (int)out.size();
        log->absorbedS = absorbedS;
        log->guardedP = guardedP;
        log->freeP = freeP;
    }

    return out;
}

// 仕上げ用: マクロ本体を S + body として登録する候補を作る。
// 元の S の後に L/R だけを挟んで P する箇所は、P の先頭 S に吸収する。
// 一方、元経路で単独 P を使っていた箇所は、先頭 S が盤面を変える場合だけ "S P" にする。
// 同じ場所で S を2回打つので、保持中なら置いて拾い直し、非保持なら拾って置き直してから body が始まる。
// 非保持かつそのマスにボールがないなら、先頭 S は何も起こさないので P のままでよい。
string build_s_prefixed_macro_candidate(
    const string& raw,
    const string& registerBody,
    const string& movementBody,
    EdgeSMacroLog* log
) {
    if (log != nullptr) {
        *log = EdgeSMacroLog{};
        log->tried = true;
    }
    if (!ENABLE_S_PREFIX_MACRO_FINISH) return "";
    if (raw.find('P') == string::npos) return "";

    string prefixedBody;
    prefixedBody.reserve(registerBody.size() + 1);
    prefixedBody.push_back('S');
    prefixedBody += registerBody;

    string out;
    out.reserve(raw.size() + prefixedBody.size() + 16);

    bool registered = false;
    EdgeMacroBoardSim sim;
    int absorbedS = 0;
    int guardedP = 0;
    int freeP = 0;

    auto emit_macro_use = [&]() {
        if (!registered) {
            out.push_back('M');
            out += prefixedBody;
            out.push_back('M');
            registered = true;
        } else {
            out.push_back('P');
        }
    };

    for (int i = 0; i < (int)raw.size(); i++) {
        char ch = raw[i];

        if (ch == 'S') {
            int j = i + 1;
            while (j < (int)raw.size() && (raw[j] == 'L' || raw[j] == 'R')) {
                j++;
            }
            if (j < (int)raw.size() && raw[j] == 'P') {
                // 移動を挟まない S は、回転後の P 先頭 S に吸収できる。
                // 例: SP -> P, SLP -> LP, SRP -> RP, SRRP -> RRP, SLLP -> LLP。
                sim.press_s();
                append_turns_and_apply(raw, i + 1, j, out, sim.state);
                emit_macro_use();
                sim.state = apply_macro_body_state(sim.state, movementBody);
                absorbedS++;
                i = j;
                continue;
            }

            out.push_back(ch);
            sim.press_s();
            continue;
        }

        if (ch == 'P') {
            if (sim.s_would_change_board()) {
                // 先に S を追加しておけば、P 内の先頭 S と合わせて同じ場所で2回 S を打つことになり、
                // body 開始時点の保持状態と盤面が元経路と揃う。
                // 箱上の正しいボールの上で P する場合も、先頭 S だけだと拾って持ち去るので、
                // 直前に S を追加してから P する必要がある。
                out.push_back('S');
                guardedP++;
            } else {
                freeP++;
            }
            emit_macro_use();
            sim.state = apply_macro_body_state(sim.state, movementBody);
            continue;
        }

        out.push_back(ch);
        if (ch == 'F' || ch == 'R' || ch == 'L') {
            sim.state = apply_button_state(sim.state, ch);
        }
    }

    if (!registered) return "";

    if (log != nullptr) {
        log->candidateLen = (int)out.size();
        log->absorbedS = absorbedS;
        log->guardedP = guardedP;
        log->freeP = freeP;
    }

    return out;
}

string build_s_prefixed_macro_candidate(
    const string& raw,
    const string& body,
    EdgeSMacroLog* log
) {
    return build_s_prefixed_macro_candidate(raw, body, body, log);
}

// 仕上げ用: マクロ本体を body + S として登録する候補を作る。
// 元の P の後に L/R だけを挟んで S する箇所は、P 末尾の S に吸収する。
// 単独 P を使う箇所は、マクロ末尾の S が盤面を変える場合だけ直後に S を追加する。
string build_s_postfixed_macro_candidate(
    const string& raw,
    const string& registerBody,
    const string& movementBody,
    EdgeSMacroLog* log
) {
    if (log != nullptr) {
        *log = EdgeSMacroLog{};
        log->tried = true;
    }
    if (!ENABLE_S_POSTFIX_MACRO_FINISH) return "";
    if (raw.find('P') == string::npos) return "";

    string postfixedBody;
    postfixedBody.reserve(registerBody.size() + 1);
    postfixedBody += registerBody;
    postfixedBody.push_back('S');

    string out;
    out.reserve(raw.size() + postfixedBody.size() + 16);

    bool registered = false;
    EdgeMacroBoardSim sim;
    int absorbedS = 0;
    int guardedP = 0;
    int freeP = 0;

    auto emit_macro_use = [&]() {
        if (!registered) {
            out.push_back('M');
            out += postfixedBody;
            out.push_back('M');
            registered = true;
        } else {
            out.push_back('P');
        }
    };

    for (int i = 0; i < (int)raw.size(); i++) {
        char ch = raw[i];

        if (ch == 'P') {
            int afterBodyState = apply_macro_body_state(sim.state, movementBody);

            int j = i + 1;
            while (j < (int)raw.size() && (raw[j] == 'L' || raw[j] == 'R')) {
                j++;
            }

            if (j < (int)raw.size() && raw[j] == 'S') {
                // 移動を挟まない S は、P 末尾の S に吸収できる。
                // 例: PS -> P, PLS -> PL, PRS -> PR, PRRS -> PRR, PLLS -> PLL。
                emit_macro_use();
                sim.state = afterBodyState;
                sim.press_s();
                append_turns_and_apply(raw, i + 1, j, out, sim.state);
                absorbedS++;
                i = j;
                continue;
            }

            sim.state = afterBodyState;
            bool needsGuard = sim.s_would_change_board();
            emit_macro_use();
            if (needsGuard) {
                // P 内の末尾 S で盤面が変わる場合は、直後に S を追加して同じ場所で2回 S を打つ。
                // これで次の操作に入る時点の保持状態と盤面が元経路と揃う。
                sim.press_s();
                out.push_back('S');
                sim.press_s();
                guardedP++;
            } else {
                // 空マスで何も持っていないなら、末尾 S は何もしない。
                sim.press_s();
                freeP++;
            }
            continue;
        }

        out.push_back(ch);
        if (ch == 'S') {
            sim.press_s();
        } else if (ch == 'F' || ch == 'R' || ch == 'L') {
            sim.state = apply_button_state(sim.state, ch);
        }
    }

    if (!registered) return "";

    if (log != nullptr) {
        log->candidateLen = (int)out.size();
        log->absorbedS = absorbedS;
        log->guardedP = guardedP;
        log->freeP = freeP;
    }

    return out;
}

string build_s_postfixed_macro_candidate(
    const string& raw,
    const string& body,
    EdgeSMacroLog* log
) {
    return build_s_postfixed_macro_candidate(raw, body, body, log);
}

int next_non_turn_pos(const string& raw, int pos) {
    while (pos < (int)raw.size() && (raw[pos] == 'L' || raw[pos] == 'R')) pos++;
    return pos;
}

// 仕上げ用: マクロ本体を S + body + S として登録する候補を作る。
// PP / P[LR]*P の境界では、前 P の末尾 S と次 P の先頭 S が同じマスで相殺する。
// preS/postS の二段適用ではこの相殺を見落とすため、末尾 S を次の移動まで保留して処理する。
string build_s_both_raw_candidate(
    const string& raw,
    const string& movementBody,
    EdgeSMacroLog* log
) {
    if (log != nullptr) {
        *log = EdgeSMacroLog{};
        log->tried = true;
    }
    if (!ENABLE_S_PREFIX_MACRO_FINISH || !ENABLE_S_POSTFIX_MACRO_FINISH) return "";
    if (raw.find('P') == string::npos) return "";

    string out;
    out.reserve(raw.size() + 16);
    EdgeMacroBoardSim sim;
    bool pendingPostfix = false;
    int prefixAbsorbPPos = -1;
    int absorbedS = 0;
    int guardedP = 0;
    int freeP = 0;

    auto settle_pending_postfix = [&]() {
        if (!pendingPostfix) return;
        if (sim.s_would_change_board()) {
            sim.press_s();
            out.push_back('S');
            sim.press_s();
            guardedP++;
        } else {
            sim.press_s();
            freeP++;
        }
        pendingPostfix = false;
    };

    for (int i = 0; i < (int)raw.size(); i++) {
        char ch = raw[i];

        if (ch == 'S') {
            if (pendingPostfix) {
                // P 末尾の S がこの元 S を実行する。間の回転は位置を変えない。
                sim.press_s();
                pendingPostfix = false;
                absorbedS++;
                continue;
            }

            int j = next_non_turn_pos(raw, i + 1);
            if (j < (int)raw.size() && raw[j] == 'P') {
                // 次 P の先頭 S がこの元 S を実行する。
                sim.press_s();
                prefixAbsorbPPos = j;
                absorbedS++;
                continue;
            }

            out.push_back('S');
            sim.press_s();
            continue;
        }

        if (ch == 'P') {
            if (pendingPostfix) {
                // PP または P[LR]*P。両端の自動 S は同じマスで打たれるので相殺する。
                pendingPostfix = false;
            } else if (prefixAbsorbPPos == i) {
                // 元 S の効果は読み取り時点で sim に反映済み。
                prefixAbsorbPPos = -1;
            } else if (sim.s_would_change_board()) {
                out.push_back('S');
                sim.press_s();
                sim.press_s();
                guardedP++;
            } else {
                sim.press_s();
                freeP++;
            }

            out.push_back('P');
            sim.state = apply_macro_body_state(sim.state, movementBody);
            pendingPostfix = true;
            continue;
        }

        if (ch == 'F') {
            settle_pending_postfix();
        }
        out.push_back(ch);
        if (ch == 'F' || ch == 'R' || ch == 'L') {
            sim.state = apply_button_state(sim.state, ch);
        }
    }

    settle_pending_postfix();

    if (log != nullptr) {
        log->candidateLen = (int)out.size();
        log->absorbedS = absorbedS;
        log->guardedP = guardedP;
        log->freeP = freeP;
    }

    return out;
}

string build_s_both_macro_candidate(
    const string& raw,
    const string& registerBody,
    const string& movementBody,
    EdgeSMacroLog* log
) {
    string bothBody;
    bothBody.reserve(registerBody.size() + 2);
    bothBody.push_back('S');
    bothBody += registerBody;
    bothBody.push_back('S');

    EdgeSMacroLog tmp;
    string rawCand = build_s_both_raw_candidate(raw, movementBody, &tmp);
    if (rawCand.empty()) {
        if (log != nullptr) *log = tmp;
        return "";
    }

    string out = replace_first_p_with_body_registration(rawCand, bothBody);
    if (log != nullptr) {
        *log = tmp;
        log->candidateLen = (int)out.size();
    }
    return out;
}

string build_s_both_macro_candidate(
    const string& raw,
    const string& body,
    EdgeSMacroLog* log
) {
    return build_s_both_macro_candidate(raw, body, body, log);
}

string build_moved_f_run_prefix(int runLen) {
    string runBody = f_run_macro_body(runLen);
    string out;
    out.reserve(runLen + 2);
    out.push_back('M');
    out += runBody;
    out.push_back('M');
    return out;
}

string build_moved_turn_then_f_run_prefix(int runLen) {
    string out;
    out.reserve(runLen + 3);
    out.push_back('R');
    out.push_back('M');
    out += f_run_macro_body(runLen);
    out.push_back('M');
    return out;
}

string build_raw_after_moved_f_run_registration(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    int runLen,
    int registrationExtra
) {
    string runBody = f_run_macro_body(runLen);

    int initialState = apply_macro_body_state(sid(START_POS, START_DIR), runBody);
    int prefixBtn = runLen + 2;      // M F^k M
    int prefixPhys = runLen;        // 登録中も実際に F^k を実行する

    return build_raw_buttons_for_order(
        cache,
        order,
        steps,
        initialState,
        prefixBtn,
        prefixPhys,
        registrationExtra
    );
}

string build_raw_after_moved_turn_then_f_run_registration(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    int runLen,
    int registrationExtra
) {
    int initialState = apply_button_state(sid(START_POS, START_DIR), 'R');
    initialState = apply_macro_body_state(initialState, f_run_macro_body(runLen));
    int prefixBtn = runLen + 3;     // R, M F^k M
    int prefixPhys = runLen + 1;    // R + F^k を実行する

    return build_raw_buttons_for_order(
        cache,
        order,
        steps,
        initialState,
        prefixBtn,
        prefixPhys,
        registrationExtra
    );
}

// 自然な F^k が先に出ない場合でも、冒頭で F^k を登録し、そのぶん進んだ状態から
// 同じ順序の経路を作り直す候補を試す。文字列置換だけで済ませるより経路整合性が高い。
string build_buttons_with_moved_f_run_registration(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    int runLen,
    RegistrationCompressionLog* log
) {
    string encodedBody = encode_body_with_f_run_macro(cache.macroBody, runLen);
    int registrationExtra = (int)encodedBody.size() + 1;

    string raw = build_raw_after_moved_f_run_registration(
        cache,
        order,
        steps,
        runLen,
        registrationExtra
    );
    if (raw.empty() || raw.find('P') == string::npos) return "";

    string out;
    out.reserve(runLen + 2 + raw.size() + encodedBody.size() + 1);
    out += build_moved_f_run_prefix(runLen);
    out += replace_first_p_with_body_registration(raw, encodedBody);

    if (log != nullptr) {
        log->used = true;
        log->sourceKind = 3;
        log->compressedLen = (int)out.size();
        log->runLen = runLen;
        log->replacedRuns = count_compressed_f_runs(cache.macroBody, runLen);
        log->firstRunPos = -1;
    }

    return out;
}

string build_buttons_with_moved_turn_then_f_run_registration(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    int runLen,
    RegistrationCompressionLog* log
) {
    string encodedBody = encode_body_with_f_run_macro(cache.macroBody, runLen);
    int registrationExtra = (int)encodedBody.size() + 1;

    string raw = build_raw_after_moved_turn_then_f_run_registration(
        cache,
        order,
        steps,
        runLen,
        registrationExtra
    );
    if (raw.empty() || raw.find('P') == string::npos) return "";

    string out;
    out.reserve(runLen + 3 + raw.size() + encodedBody.size() + 1);
    out += build_moved_turn_then_f_run_prefix(runLen);
    out += replace_first_p_with_body_registration(raw, encodedBody);

    if (log != nullptr) {
        log->used = true;
        log->sourceKind = 4;
        log->compressedLen = (int)out.size();
        log->runLen = runLen;
        log->replacedRuns = count_compressed_f_runs(cache.macroBody, runLen);
        log->firstRunPos = -1;
    }

    return out;
}

string build_raw_with_existing_f_run_registration(const string& raw, int runLen) {
    size_t firstP = raw.find('P');
    if (firstP == string::npos) return "";

    string runBody = f_run_macro_body(runLen);
    size_t runPos = raw.find(runBody);
    if (runPos == string::npos || runPos + runLen - 1 >= firstP) return "";

    string out;
    out.reserve(raw.size() + 2);
    out.append(raw, 0, runPos);
    out.push_back('M');
    out += runBody;
    out.push_back('M');
    out.append(raw, runPos + runLen, string::npos);
    return out;
}

string build_raw_with_airshot_f_run_registration(const string& raw, int runLen) {
    string runBody = f_run_macro_body(runLen);

    string out;
    out.reserve(raw.size() + runLen + 4);
    out.push_back('L');
    out.push_back('M');
    out += runBody;
    out.push_back('M');
    out.push_back('R');
    out += raw;
    return out;
}

bool output_fits_limits(const string& buttons) {
    return (int)buttons.size() <= output_turn_limit() &&
        expanded_primitive_length(buttons, output_turn_limit()) <= output_turn_limit();
}

void try_edge_s_after_f_run_registration(
    const string& preparedRaw,
    const string& encodedBody,
    const string& movementBody,
    int normalLen,
    string& best,
    RegistrationCompressionLog& bestLog,
    const RegistrationCompressionLog& compressionBaseLog,
    EdgeSMacroLog& bestSPrefixLog,
    EdgeSMacroLog& bestSBothLog,
    EdgeSMacroLog& bestSPostfixLog,
    EdgeSSwitchLog& bestSwitchLog
) {
    if (preparedRaw.empty()) return;

    string compressedOnly = replace_first_p_with_body_registration(preparedRaw, encodedBody);
    int inputLen = (int)compressedOnly.size();

    EdgeSMacroLog prefixLog;
    string prefixCand = build_s_prefixed_macro_candidate(
        preparedRaw,
        encodedBody,
        movementBody,
        &prefixLog
    );
    prefixLog.normalLen = normalLen;
    prefixLog.inputLen = inputLen;
    if (!prefixCand.empty() &&
        prefixCand.size() < best.size() &&
        output_fits_limits(prefixCand)) {
        prefixLog.used = true;
        prefixLog.saved = prefixLog.inputLen - (int)prefixCand.size();
        prefixLog.totalSaved = normalLen - (int)prefixCand.size();
        best = std::move(prefixCand);
        bestLog = compressionBaseLog;
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = inputLen;
        bestLog.saved = normalLen - inputLen;
        bestSPrefixLog = prefixLog;
        bestSBothLog.used = false;
        bestSPostfixLog.used = false;
        bestSwitchLog.used = false;
    }

    EdgeSMacroLog bothLog;
    string bothCand = build_s_both_macro_candidate(
        preparedRaw,
        encodedBody,
        movementBody,
        &bothLog
    );
    bothLog.normalLen = normalLen;
    bothLog.inputLen = inputLen;
    if (!bothCand.empty() &&
        bothCand.size() < best.size() &&
        output_fits_limits(bothCand)) {
        bothLog.used = true;
        bothLog.saved = bothLog.inputLen - (int)bothCand.size();
        bothLog.totalSaved = normalLen - (int)bothCand.size();
        best = std::move(bothCand);
        bestLog = compressionBaseLog;
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = inputLen;
        bestLog.saved = normalLen - inputLen;
        bestSPrefixLog.used = false;
        bestSBothLog = bothLog;
        bestSPostfixLog.used = false;
        bestSwitchLog.used = false;
    }

    EdgeSMacroLog postfixLog;
    string postfixCand = build_s_postfixed_macro_candidate(
        preparedRaw,
        encodedBody,
        movementBody,
        &postfixLog
    );
    postfixLog.normalLen = normalLen;
    postfixLog.inputLen = inputLen;
    if (!postfixCand.empty() &&
        postfixCand.size() < best.size() &&
        output_fits_limits(postfixCand)) {
        postfixLog.used = true;
        postfixLog.saved = postfixLog.inputLen - (int)postfixCand.size();
        postfixLog.totalSaved = normalLen - (int)postfixCand.size();
        best = std::move(postfixCand);
        bestLog = compressionBaseLog;
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = inputLen;
        bestLog.saved = normalLen - inputLen;
        bestSPrefixLog.used = false;
        bestSBothLog.used = false;
        bestSPostfixLog = postfixLog;
        bestSwitchLog.used = false;
    }
}

void try_edge_s_switch_after_registration(
    const string& preparedRaw,
    const string& registerBody,
    const string& movementBody,
    int normalLen,
    string& best,
    RegistrationCompressionLog& bestLog,
    const RegistrationCompressionLog& compressionBaseLog,
    EdgeSMacroLog& bestSPrefixLog,
    EdgeSMacroLog& bestSBothLog,
    EdgeSMacroLog& bestSPostfixLog,
    EdgeSSwitchLog& bestSwitchLog
) {
    if (preparedRaw.empty()) return;

    string compressedOnly = replace_first_p_with_body_registration(preparedRaw, registerBody);
    int inputLen = (int)compressedOnly.size();
    vector<EdgeSMacroUseInfo> uses = analyze_edge_s_macro_uses(preparedRaw, movementBody);
    EdgeSSwitchChoice choice = find_best_edge_s_switch_choice(preparedRaw, uses, inputLen);

    EdgeSSwitchLog switchLog;
    switchLog.tried = true;
    switchLog.toPrefix = choice.toPrefix;
    switchLog.mode = choice.mode;
    switchLog.totalUses = choice.totalUses;
    switchLog.switchUse = choice.switchUse;
    switchLog.normalLen = normalLen;
    switchLog.inputLen = inputLen;

    if (!choice.valid) {
        if (!bestSwitchLog.tried) bestSwitchLog = switchLog;
        return;
    }

    string cand = build_s_switch_macro_candidate(
        preparedRaw,
        registerBody,
        movementBody,
        uses,
        choice,
        &switchLog
    );
    if (cand.empty()) {
        if (!bestSwitchLog.tried) bestSwitchLog = switchLog;
        return;
    }

    switchLog.normalLen = normalLen;
    switchLog.inputLen = inputLen;
    switchLog.saved = inputLen - (int)cand.size();
    switchLog.totalSaved = normalLen - (int)cand.size();
    if (!bestSwitchLog.tried || switchLog.candidateLen < bestSwitchLog.candidateLen) {
        bestSwitchLog = switchLog;
    }

    if (cand.size() < best.size() && output_fits_limits(cand)) {
        switchLog.used = true;
        best = std::move(cand);
        bestLog = compressionBaseLog;
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = inputLen;
        bestLog.saved = normalLen - inputLen;
        bestSPrefixLog.used = false;
        bestSBothLog.used = false;
        bestSPostfixLog.used = false;
        bestSwitchLog = switchLog;
    }
}

// 評価で選ばれた順序とマクロに対して、実際に出力する M/P 付きボタン列を復元する。
string build_buttons_for_order(
    const TransitionCache& cache,
    const vector<int>& order,
    int steps,
    RegistrationCompressionLog* compressionLog = nullptr,
    EdgeSMacroLog* sPrefixLog = nullptr,
    EdgeSMacroLog* sBothLog = nullptr,
    EdgeSMacroLog* sPostfixLog = nullptr,
    EdgeSSwitchLog* sSwitchLog = nullptr
) {
    string raw = build_raw_buttons_for_order(
        cache,
        order,
        steps,
        sid(START_POS, START_DIR),
        0,
        0,
        0
    );
    if (raw.empty()) return "";

    RegistrationCompressionLog bestLog;
    string normal = replace_first_p_with_registration(raw, cache.macroBody);
    const int normalLen = (int)normal.size();
    string best = replace_first_p_with_compressed_registration(raw, cache.macroBody, &bestLog);
    if ((int)best.size() > output_turn_limit() ||
        expanded_primitive_length(best, output_turn_limit()) > output_turn_limit()) {
        best = normal;
        bestLog = RegistrationCompressionLog{};
    }
    if (!bestLog.used) {
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = (int)best.size();
    }

    EdgeSMacroLog candSPrefixLog;
    EdgeSMacroLog candSBothLog;
    EdgeSMacroLog candSPostfixLog;
    EdgeSSwitchLog candSSwitchLog;

    if (cache.macroBody.find('P') == string::npos) {
        for (int runLen : F_RUN_COMPRESS_RUN_LENGTHS) {
            RegistrationCompressionLog candLog;
            candLog.normalLen = normalLen;
            string cand = build_buttons_with_moved_f_run_registration(
                cache,
                order,
                steps,
                runLen,
                &candLog
            );

            if (!cand.empty() &&
                (int)cand.size() <= output_turn_limit() &&
                expanded_primitive_length(cand, output_turn_limit()) <= output_turn_limit() &&
                cand.size() < best.size()) {
                candLog.normalLen = normalLen;
                candLog.saved = normalLen - (int)cand.size();
                best = std::move(cand);
                bestLog = candLog;
            }

            RegistrationCompressionLog candRfLog;
            candRfLog.normalLen = normalLen;
            string candRf = build_buttons_with_moved_turn_then_f_run_registration(
                cache,
                order,
                steps,
                runLen,
                &candRfLog
            );

            if (!candRf.empty() &&
                (int)candRf.size() <= output_turn_limit() &&
                expanded_primitive_length(candRf, output_turn_limit()) <= output_turn_limit() &&
                candRf.size() < best.size()) {
                candRfLog.normalLen = normalLen;
                candRfLog.saved = normalLen - (int)candRf.size();
                best = std::move(candRf);
                bestLog = candRfLog;
            }
        }
    }

    const int edgeMacroBaseLen = normalLen;

    // F^k 小マクロで本命マクロを圧縮登録したうえで、さらに本命マクロの前後に S を付ける候補。
    // encodedBody 内の P は小マクロとして記録中に展開され、実際に登録される本命マクロは
    // S + cache.macroBody または cache.macroBody + S と同じ動きになる。
    if (cache.macroBody.find('P') == string::npos) {
        for (int runLen : F_RUN_COMPRESS_RUN_LENGTHS) {
            string encodedBody = encode_body_with_f_run_macro(cache.macroBody, runLen);
            if (encodedBody == cache.macroBody) continue;

            RegistrationCompressionLog comboLog;
            comboLog.used = true;
            comboLog.sourceKind = 0;
            comboLog.runLen = runLen;
            comboLog.replacedRuns = count_compressed_f_runs(cache.macroBody, runLen);
            comboLog.firstRunPos = -1;

            string existingRaw = build_raw_with_existing_f_run_registration(raw, runLen);
            if (!existingRaw.empty()) {
                size_t runPos = raw.find(f_run_macro_body(runLen));
                comboLog.sourceKind = 1;
                comboLog.firstRunPos = (int)runPos;
                try_edge_s_after_f_run_registration(
                    existingRaw,
                    encodedBody,
                    cache.macroBody,
                    edgeMacroBaseLen,
                    best,
                    bestLog,
                    comboLog,
                    candSPrefixLog,
                    candSBothLog,
                    candSPostfixLog,
                    candSSwitchLog
                );
                try_edge_s_switch_after_registration(
                    existingRaw,
                    encodedBody,
                    cache.macroBody,
                    edgeMacroBaseLen,
                    best,
                    bestLog,
                    comboLog,
                    candSPrefixLog,
                    candSBothLog,
                    candSPostfixLog,
                    candSSwitchLog
                );
            }

            comboLog.firstRunPos = -1;
            comboLog.sourceKind = 2;
            string airshotRaw = build_raw_with_airshot_f_run_registration(raw, runLen);
            try_edge_s_after_f_run_registration(
                airshotRaw,
                encodedBody,
                cache.macroBody,
                edgeMacroBaseLen,
                best,
                bestLog,
                comboLog,
                candSPrefixLog,
                candSBothLog,
                candSPostfixLog,
                candSSwitchLog
            );
            try_edge_s_switch_after_registration(
                airshotRaw,
                encodedBody,
                cache.macroBody,
                edgeMacroBaseLen,
                best,
                bestLog,
                comboLog,
                candSPrefixLog,
                candSBothLog,
                candSPostfixLog,
                candSSwitchLog
            );

            int movedRegistrationExtra = (int)encodedBody.size() + 2;
            string movedRaw = build_raw_after_moved_f_run_registration(
                cache,
                order,
                steps,
                runLen,
                movedRegistrationExtra
            );
            if (!movedRaw.empty()) {
                string movedPreparedRaw = build_moved_f_run_prefix(runLen) + movedRaw;
                comboLog.sourceKind = 3;
                comboLog.firstRunPos = -1;
                try_edge_s_after_f_run_registration(
                    movedPreparedRaw,
                    encodedBody,
                    cache.macroBody,
                    edgeMacroBaseLen,
                    best,
                    bestLog,
                    comboLog,
                    candSPrefixLog,
                    candSBothLog,
                    candSPostfixLog,
                    candSSwitchLog
                );
                try_edge_s_switch_after_registration(
                    movedPreparedRaw,
                    encodedBody,
                    cache.macroBody,
                    edgeMacroBaseLen,
                    best,
                    bestLog,
                    comboLog,
                    candSPrefixLog,
                    candSBothLog,
                    candSPostfixLog,
                    candSSwitchLog
                );
            }

            int movedTurnRegistrationExtra = (int)encodedBody.size() + 2;
            string movedTurnRaw = build_raw_after_moved_turn_then_f_run_registration(
                cache,
                order,
                steps,
                runLen,
                movedTurnRegistrationExtra
            );
            if (!movedTurnRaw.empty()) {
                string movedTurnPreparedRaw = build_moved_turn_then_f_run_prefix(runLen) + movedTurnRaw;
                comboLog.sourceKind = 4;
                comboLog.firstRunPos = -1;
                try_edge_s_after_f_run_registration(
                    movedTurnPreparedRaw,
                    encodedBody,
                    cache.macroBody,
                    edgeMacroBaseLen,
                    best,
                    bestLog,
                    comboLog,
                    candSPrefixLog,
                    candSBothLog,
                    candSPostfixLog,
                    candSSwitchLog
                );
                try_edge_s_switch_after_registration(
                    movedTurnPreparedRaw,
                    encodedBody,
                    cache.macroBody,
                    edgeMacroBaseLen,
                    best,
                    bestLog,
                    comboLog,
                    candSPrefixLog,
                    candSBothLog,
                    candSPostfixLog,
                    candSSwitchLog
                );
                comboLog.sourceKind = 0;
            }
        }
    }

    EdgeSMacroLog plainSPrefixLog;
    string sPrefixCand = build_s_prefixed_macro_candidate(raw, cache.macroBody, &plainSPrefixLog);
    plainSPrefixLog.normalLen = edgeMacroBaseLen;
    plainSPrefixLog.inputLen = edgeMacroBaseLen;
    if (!sPrefixCand.empty() &&
        (int)sPrefixCand.size() <= output_turn_limit() &&
        expanded_primitive_length(sPrefixCand, output_turn_limit()) <= output_turn_limit() &&
        sPrefixCand.size() < best.size()) {
        plainSPrefixLog.used = true;
        plainSPrefixLog.saved = edgeMacroBaseLen - (int)sPrefixCand.size();
        plainSPrefixLog.totalSaved = plainSPrefixLog.saved;
        candSPrefixLog = plainSPrefixLog;
        candSBothLog.used = false;
        candSPostfixLog.used = false;
        candSSwitchLog.used = false;
        best = std::move(sPrefixCand);
        bestLog = RegistrationCompressionLog{};
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = (int)best.size();
    } else if (!candSPrefixLog.tried) {
        candSPrefixLog = plainSPrefixLog;
    }

    EdgeSMacroLog plainSBothLog;
    string sBothCand = build_s_both_macro_candidate(raw, cache.macroBody, &plainSBothLog);
    plainSBothLog.normalLen = edgeMacroBaseLen;
    plainSBothLog.inputLen = edgeMacroBaseLen;
    if (!sBothCand.empty() &&
        (int)sBothCand.size() <= output_turn_limit() &&
        expanded_primitive_length(sBothCand, output_turn_limit()) <= output_turn_limit() &&
        sBothCand.size() < best.size()) {
        candSPrefixLog.used = false;
        plainSBothLog.used = true;
        plainSBothLog.saved = edgeMacroBaseLen - (int)sBothCand.size();
        plainSBothLog.totalSaved = plainSBothLog.saved;
        candSBothLog = plainSBothLog;
        candSPostfixLog.used = false;
        candSSwitchLog.used = false;
        best = std::move(sBothCand);
        bestLog = RegistrationCompressionLog{};
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = (int)best.size();
    } else if (!candSBothLog.tried) {
        candSBothLog = plainSBothLog;
    }

    EdgeSMacroLog plainSPostfixLog;
    string sPostfixCand = build_s_postfixed_macro_candidate(raw, cache.macroBody, &plainSPostfixLog);
    plainSPostfixLog.normalLen = edgeMacroBaseLen;
    plainSPostfixLog.inputLen = edgeMacroBaseLen;
    if (!sPostfixCand.empty() &&
        (int)sPostfixCand.size() <= output_turn_limit() &&
        expanded_primitive_length(sPostfixCand, output_turn_limit()) <= output_turn_limit() &&
        sPostfixCand.size() < best.size()) {
        candSPrefixLog.used = false;
        candSBothLog.used = false;
        plainSPostfixLog.used = true;
        plainSPostfixLog.saved = edgeMacroBaseLen - (int)sPostfixCand.size();
        plainSPostfixLog.totalSaved = plainSPostfixLog.saved;
        candSPostfixLog = plainSPostfixLog;
        candSSwitchLog.used = false;
        best = std::move(sPostfixCand);
        bestLog = RegistrationCompressionLog{};
        bestLog.normalLen = normalLen;
        bestLog.compressedLen = (int)best.size();
    } else if (!candSPostfixLog.tried) {
        candSPostfixLog = plainSPostfixLog;
    }

    try_edge_s_switch_after_registration(
        raw,
        cache.macroBody,
        cache.macroBody,
        edgeMacroBaseLen,
        best,
        bestLog,
        RegistrationCompressionLog{},
        candSPrefixLog,
        candSBothLog,
        candSPostfixLog,
        candSSwitchLog
    );

    if (compressionLog != nullptr) {
        *compressionLog = bestLog;
    }
    if (sPrefixLog != nullptr) {
        *sPrefixLog = candSPrefixLog;
    }
    if (sBothLog != nullptr) {
        *sBothLog = candSBothLog;
    }
    if (sPostfixLog != nullptr) {
        *sPostfixLog = candSPostfixLog;
    }
    if (sSwitchLog != nullptr) {
        *sSwitchLog = candSSwitchLog;
    }

    return best;
}

// メイン探索。ボール順序とマクロ本体を同じSAの中で動かす。
SolveResult solve_annealing_order_and_macro(chrono::steady_clock::time_point programStart) {
    XorShift rng(make_seed() ^ 0x51d00d1234567890ULL);

    vector<TransitionCache> caches;
    caches.reserve(128);
    unordered_set<string> initialMacroBodies;
    vector<int> initialMacroSegmentId;
    vector<string> cacheOrigin;

    auto add_initial_macro = [&](const string& rawBody, int segmentId = -1, const string& origin = "fallback") {
        string body = normalize_macro_body(rawBody);
        if (body.empty()) return;
        if (!initialMacroBodies.insert(body).second) return;
        caches.push_back(build_transition_cache(body));
        initialMacroSegmentId.push_back(segmentId);
        cacheOrigin.push_back(origin);
    };

    auto origin_of_cache = [&](int ci) -> string {
        if (ci < 0 || ci >= (int)cacheOrigin.size()) return "unknown";
        return cacheOrigin[ci];
    };

    auto segment_of_cache = [&](int ci) -> int {
        if (ci < 0 || ci >= (int)initialMacroSegmentId.size()) return -1;
        return initialMacroSegmentId[ci];
    };

    // 既存の自然な移動区間から経路プールを作り、候補マクロを複数切り出す。
    // 全候補を重い full 評価へ回さず、まず同じプール由来のサンプルで軽く順位付けする。
    vector<InitialPathSource> initialPathPool = build_initial_path_pool();
    vector<MacroSample> initialSamples = make_initial_path_macro_samples(initialPathPool, rng);
    vector<pair<long long, InitialPathMacroCandidate>> initialCandidates;
    unordered_set<string> initialCandidateBodies;
    initialCandidates.reserve(SA_INITIAL_PATH_MACRO_CANDIDATES);

    auto add_initial_candidate = [&](InitialPathMacroCandidate cand) {
        if (cand.body.empty()) return;
        if (!initialCandidateBodies.insert(cand.body).second) return;

        long long score = initialSamples.empty()
            ? (long long)cand.body.size()
            : score_macro_body_on_samples(cand.body, initialSamples);

        initialCandidates.push_back({score, std::move(cand)});
        sort(initialCandidates.begin(), initialCandidates.end(), [](const auto& a, const auto& b) {
            if (a.first != b.first) return a.first < b.first;
            return a.second.body.size() < b.second.body.size();
        });
    };

    const int pathMacroRetryLimit = max(1, SA_INITIAL_PATH_MACRO_CANDIDATES * 20);
    for (int tries = 0;
         tries < pathMacroRetryLimit &&
         (int)initialCandidateBodies.size() < SA_INITIAL_PATH_MACRO_CANDIDATES;
         tries++) {
        add_initial_candidate(make_initial_path_macro_body(initialPathPool, rng));
    }

    vector<int> identity(M);
    iota(identity.begin(), identity.end(), 0);

    array<OrderState, MACRO_SLOT_COUNT> curStates;
    array<int, MACRO_SLOT_COUNT> curCaches{};
    vector<OrderState> initialBestState(caches.size());
    vector<char> hasInitialBest(caches.size(), 0);

    vector<int> bestOrder = identity;
    int bestCache = 0;
    OrderEval bestEval;
    vector<BestSnapshot> bestHistory;
    vector<BestSnapshot> bestRanking;

    auto remember_best_snapshot = [&]() {
        bestHistory.push_back(BestSnapshot{bestOrder, bestCache, bestEval});
        if ((int)bestHistory.size() > BEST_HISTORY_COUNT) {
            bestHistory.erase(bestHistory.begin());
        }
    };

    auto sort_best_ranking = [&]() {
        sort(bestRanking.begin(), bestRanking.end(), [](const BestSnapshot& a, const BestSnapshot& b) {
            return better_eval(a.eval, b.eval);
        });
        if ((int)bestRanking.size() > BEST_RANKING_COUNT) {
            bestRanking.resize(BEST_RANKING_COUNT);
        }
    };

    auto consider_rank_snapshot = [&](const vector<int>& order, int cache, const OrderEval& eval) {
        const string& body = caches[cache].macroBody;
        for (BestSnapshot& snapshot : bestRanking) {
            if (caches[snapshot.cache].macroBody != body) continue;
            if (better_eval(eval, snapshot.eval)) {
                snapshot = BestSnapshot{order, cache, eval};
                sort_best_ranking();
            }
            return;
        }

        if ((int)bestRanking.size() < BEST_RANKING_COUNT ||
            better_eval(eval, bestRanking.back().eval)) {
            bestRanking.push_back(BestSnapshot{order, cache, eval});
            sort_best_ranking();
        }
    };

    auto would_enter_ranking = [&](const string& body, const OrderEval& eval) {
        for (const BestSnapshot& snapshot : bestRanking) {
            if (caches[snapshot.cache].macroBody == body) {
                return better_eval(eval, snapshot.eval);
            }
        }
        return (int)bestRanking.size() < BEST_RANKING_COUNT ||
            better_eval(eval, bestRanking.back().eval);
    };

    auto try_start_order = [&](int ci, const vector<int>& ord) {
        if (ci >= (int)initialBestState.size()) {
            initialBestState.resize(ci + 1);
            hasInitialBest.resize(ci + 1, 0);
        }

        OrderState state = make_order_state(caches[ci], ord);
        consider_rank_snapshot(order_state_to_vector(state), ci, state.eval);
        if (!hasInitialBest[ci] || better_eval(state.eval, initialBestState[ci].eval)) {
            hasInitialBest[ci] = 1;
            initialBestState[ci] = state;
        }
        if (bestEval.energy == (1LL << 60) || better_eval(state.eval, bestEval)) {
            bestOrder = order_state_to_vector(state);
            bestCache = ci;
            bestEval = state.eval;
            remember_best_snapshot();
        }
    };

    vector<vector<int>> randomStartOrders;
    int starts = min(SA_RANDOM_START_MAX, max(SA_RANDOM_START_MIN, M));
    randomStartOrders.reserve(starts);
    for (int s = 0; s < starts; s++) {
        vector<int> ord = identity;
        shuffle_order(ord, rng);
        randomStartOrders.push_back(std::move(ord));
    }

    auto evaluate_initial_cache = [&](int ci) {
        try_start_order(ci, identity);
        vector<int> greedy = make_greedy_order(caches[ci]);
        if ((int)greedy.size() == M) {
            try_start_order(ci, greedy);
        }

        for (const vector<int>& ord : randomStartOrders) {
            try_start_order(ci, ord);
        }

        return hasInitialBest[ci] && initialBestState[ci].eval.completed == M;
    };

    // サンプルランキング上位から順に本評価する。
    // T-10 以内で全完了できる初期候補が出たら、以後の重い本評価は省略する。
    bool hasCompleteInitial = false;
    for (const auto& item : initialCandidates) {
        int before = (int)caches.size();
        add_initial_macro(item.second.body, item.second.segmentId, "path");
        if ((int)caches.size() == before) continue;

        int ci = before;
        if (evaluate_initial_cache(ci)) {
            hasCompleteInitial = true;
            break;
        }
    }

    if (caches.empty()) {
        // すべての自然区間が空だった場合の実行不能回避。ランダム初期マクロは使わない。
        add_initial_macro("F", -1, "fallback");
        evaluate_initial_cache(0);
    } else if (!hasCompleteInitial) {
        // サンプル上位で全完了できない場合も、評価済み候補の最良 prefix から開始する。
        // 追加の候補を闇雲に増やすより、以降のSAに時間を残す。
    }

    vector<int> rankedInitialCaches;
    for (int ci = 0; ci < (int)caches.size(); ci++) {
        if (!hasInitialBest[ci]) continue;
        rankedInitialCaches.push_back(ci);
    }
    sort(rankedInitialCaches.begin(), rankedInitialCaches.end(), [&](int a, int b) {
        return better_eval(initialBestState[a].eval, initialBestState[b].eval);
    });

    vector<char> usedInitialCache(caches.size(), 0);
    unordered_set<int> usedInitialSegments;

    auto pick_initial_cache_for_slot = [&](int slot) {
        if (rankedInitialCaches.empty()) return 0;

        // まず区間由来で、かつ別 segment の候補を優先する。
        for (int ci : rankedInitialCaches) {
            if (usedInitialCache[ci]) continue;
            int seg = segment_of_cache(ci);
            if (seg < 0 || usedInitialSegments.count(seg)) continue;
            return ci;
        }

        // 区間由来が足りなければ、未使用候補から評価順に埋める。
        for (int ci : rankedInitialCaches) {
            if (!usedInitialCache[ci]) return ci;
        }

        return rankedInitialCaches[min(slot, (int)rankedInitialCaches.size() - 1)];
    };

    for (int s = 0; s < MACRO_SLOT_COUNT; s++) {
        int ci = pick_initial_cache_for_slot(s);
        if (ci >= 0 && ci < (int)usedInitialCache.size()) {
            usedInitialCache[ci] = 1;
            int seg = segment_of_cache(ci);
            if (seg >= 0) usedInitialSegments.insert(seg);
        }
        curCaches[s] = ci;
        curStates[s] = initialBestState[ci];
        //caches[ci].prebuild_for_order(curStates[s].order, M);
    }

    cerr << "[anneal-init slots]";
    for (int s = 0; s < MACRO_SLOT_COUNT; s++) {
        int ci = curCaches[s];
        cerr << " slot" << s
             << " cache=" << ci
             << " origin=" << origin_of_cache(ci)
             << " seg=" << segment_of_cache(ci)
             << " macro=" << caches[ci].macroBody
             << " len=" << caches[ci].macroBody.size();
    }
    cerr << "\n";

    auto best_current_slot = [&]() {
        int best = 0;
        for (int s = 1; s < MACRO_SLOT_COUNT; s++) {
            if (better_eval(curStates[s].eval, curStates[best].eval)) best = s;
        }
        return best;
    };

    int curBestSlot = best_current_slot();
    long long curEnergy = curStates[curBestSlot].eval.energy;

    cerr << "[anneal-init macro] "
         << "slots=" << MACRO_SLOT_COUNT
         << " completed=" << bestEval.completed << "/" << M
         << " btn=" << bestEval.btn
         << " phys=" << bestEval.phys
         << " macro=" << caches[bestCache].macroBody
         << " len=" << caches[bestCache].macroBody.size()
         << "\n";

    const double startTemp = (double)max(1, bestEval.btn) * SA_START_TEMP_SCALE;
    const double endTemp = SA_END_TEMP;

    long long iter = 0;
    int macroTries = 0;
    int macroAccepted = 0;
    double progress = 0.0;
    double temp = startTemp;
    double lastReportTime = 0.0;
    double lastImproveTime = 0.0;
    long long accepted = 0;
    long long uphillAccepted = 0;
    long long improved = 0;
    long long sampleMacroMoves = 0;
    long long lastReportIter = 0;
    long long lastReportAccepted = 0;
    long long lastReportUphill = 0;
    long long lastReportImproved = 0;
    long long lastMacroTryIter = 0;

    auto print_progress = [&](double elapsed, const char* tag) {
        long long spanIter = max(1LL, iter - lastReportIter);
        long long spanAccepted = accepted - lastReportAccepted;
        long long spanUphill = uphillAccepted - lastReportUphill;
        long long spanImproved = improved - lastReportImproved;

        cerr << "[anneal-progress macro " << tag << "] "
             << "t=" << fixed << setprecision(3) << elapsed
             << " iter=" << iter
             << " temp=" << setprecision(3) << temp
             << " best=" << bestEval.completed << "/" << M
             << " btn=" << bestEval.btn
             << " phys=" << bestEval.phys
             << " macro=" << caches[bestCache].macroBody
             << " len=" << caches[bestCache].macroBody.size()
             << " curSlot=" << curBestSlot
             << " cur=" << curStates[curBestSlot].eval.completed << "/" << M
             << " btn=" << curStates[curBestSlot].eval.btn
             << " phys=" << curStates[curBestSlot].eval.phys
             << " macro=" << caches[curCaches[curBestSlot]].macroBody
             << " curLen=" << caches[curCaches[curBestSlot]].macroBody.size()
             << " acc=" << spanAccepted << "/" << spanIter
             << " uphill=" << spanUphill
             << " improve=" << spanImproved
             << " macroTry=" << macroTries
             << " macroAcc=" << macroAccepted
             << " sampleMacro=" << sampleMacroMoves
             << " lastImprove=" << setprecision(3) << lastImproveTime
             << "\n";

        lastReportIter = iter;
        lastReportAccepted = accepted;
        lastReportUphill = uphillAccepted;
        lastReportImproved = improved;
        lastReportTime = elapsed;
    };

    while (M >= 2) {
        if ((iter & SA_TIME_CHECK_MASK) == 0) {
            double elapsed = elapsed_sec(programStart);
            if (elapsed >= SA_TOTAL_TIME_LIMIT_SEC) break;
            progress = elapsed / max(0.001, SA_TOTAL_TIME_LIMIT_SEC);
            temp = startTemp * pow(endTemp / startTemp, progress);

            if (elapsed - lastReportTime >= SA_PROGRESS_REPORT_INTERVAL_SEC) {
                print_progress(elapsed, "tick");
            }
        }

        iter++;

        bool macroMove = (iter - lastMacroTryIter >= SA_MACRO_MUTATE_INTERVAL_ITERS);

        if (macroMove) {
            lastMacroTryIter = iter;
            macroTries++;
            int slot = rng.nextInt(MACRO_SLOT_COUNT);
            vector<MacroSample> samples = collect_macro_samples(
                caches[curCaches[slot]],
                order_state_to_vector(curStates[slot]),
                curStates[slot].eval.completed,
                rng
            );
            vector<string> bodies = improve_macro_body_pool_on_samples(
                caches[curCaches[slot]].macroBody,
                samples,
                rng,
                initialPathPool
            );
            if (!samples.empty()) sampleMacroMoves++;

            bool foundMacroCandidate = false;
            bool timedOutDuringMacroEval = false;
            OrderEval bestCandEval;
            OrderState bestCandState;
            TransitionCache bestCandCacheObj;

            for (const string& body : bodies) {
                TransitionCache tmpCache = build_transition_cache(body);

                // マクロ本体の変更は遷移表の再構築が重い。
                // 候補を1本作るたび、マスク周期を待たずに時間を見る。
                {
                    double elapsed = elapsed_sec(programStart);
                    if (elapsed >= SA_TOTAL_TIME_LIMIT_SEC) {
                        timedOutDuringMacroEval = true;
                        break;
                    }
                    progress = elapsed / max(0.001, SA_TOTAL_TIME_LIMIT_SEC);
                    temp = startTemp * pow(endTemp / startTemp, progress);
                }

                long long acceptWindow = (long long)(SA_UPHILL_ACCEPT_TEMP_LIMIT * temp);
                long long slotEnergy = curStates[slot].eval.energy;
                long long acceptEnergyLimit = slotEnergy + max(0LL, acceptWindow);

                OrderState tmpState;
                tmpState.order = curStates[slot].order;
                bool possiblyAccepted = rebuild_order_state_from(
                    tmpCache,
                    tmpState,
                    0,
                    acceptEnergyLimit
                );
                if (!possiblyAccepted) continue;

                OrderEval tmpEval = tmpState.eval;

                if (!foundMacroCandidate || better_eval(tmpEval, bestCandEval)) {
                    foundMacroCandidate = true;
                    bestCandEval = tmpEval;
                    bestCandState = std::move(tmpState);
                    bestCandCacheObj = std::move(tmpCache);
                }
            }

            if (!foundMacroCandidate) {
                if (timedOutDuringMacroEval) break;
                continue;
            }

            int storedCandCache = -1;
            auto store_candidate_cache = [&]() {
                if (storedCandCache == -1) {
                    storedCandCache = (int)caches.size();
                    caches.push_back(std::move(bestCandCacheObj));
                    initialMacroSegmentId.push_back(-1);
                    cacheOrigin.push_back("anneal");
                }
                return storedCandCache;
            };

            if (would_enter_ranking(bestCandCacheObj.macroBody, bestCandEval)) {
                int rankCache = store_candidate_cache();
                consider_rank_snapshot(order_state_to_vector(bestCandState), rankCache, bestCandEval);
            }

            long long diff = bestCandEval.energy - curStates[slot].eval.energy;
            bool accept = false;
            if (diff <= 0) {
                accept = true;
            } else if ((double)diff < SA_UPHILL_ACCEPT_TEMP_LIMIT * temp) {
                accept = exp(-(double)diff / temp) > rng.nextDouble();
            }

            if (better_eval(bestCandEval, bestEval)) {
                bestOrder = order_state_to_vector(bestCandState);
                bestCache = store_candidate_cache();
                bestEval = bestCandEval;
                remember_best_snapshot();
                improved++;
                lastImproveTime = elapsed_sec(programStart);
            }

            if (accept) {
                accepted++;
                if (diff > 0) uphillAccepted++;
                macroAccepted++;
                curStates[slot] = std::move(bestCandState);
                curCaches[slot] = store_candidate_cache();
                caches[curCaches[slot]].prebuild_for_order(curStates[slot].order, M);
                curBestSlot = best_current_slot();
                curEnergy = curStates[curBestSlot].eval.energy;
            }
        } else {
            array<int, MAX_M> candOrder = curStates[curBestSlot].order;
            int changedFrom = 0;
            if (!mutate_order_array(candOrder, rng, changedFrom)) continue;

            array<OrderState, MACRO_SLOT_COUNT> candStates;
            int candBestSlot = 0;
            for (int s = 0; s < MACRO_SLOT_COUNT; s++) {
                candStates[s].order = candOrder;
                copy(
                    curStates[s].prefixDp.begin(),
                    curStates[s].prefixDp.begin() + changedFrom + 1,
                    candStates[s].prefixDp.begin()
                );
                rebuild_order_state_from(caches[curCaches[s]], candStates[s], changedFrom);
                if (s == 0 || better_eval(candStates[s].eval, candStates[candBestSlot].eval)) {
                    candBestSlot = s;
                }
            }

            OrderEval candEval = candStates[candBestSlot].eval;
            long long diff = candEval.energy - curEnergy;
            bool accept = false;

            if (diff <= 0) {
                accept = true;
            } else if ((double)diff < SA_UPHILL_ACCEPT_TEMP_LIMIT * temp) {
                accept = exp(-(double)diff / temp) > rng.nextDouble();
            }

            if (better_eval(candEval, bestEval)) {
                bestOrder = order_state_to_vector(candStates[candBestSlot]);
                bestCache = curCaches[candBestSlot];
                bestEval = candEval;
                remember_best_snapshot();
                improved++;
                lastImproveTime = elapsed_sec(programStart);
            }

            consider_rank_snapshot(
                order_state_to_vector(candStates[candBestSlot]),
                curCaches[candBestSlot],
                candEval
            );

            if (accept) {
                accepted++;
                if (diff > 0) uphillAccepted++;
                curStates = std::move(candStates);
                curBestSlot = candBestSlot;
                curEnergy = candEval.energy;
            }
        }
    }

    {
        double elapsed = elapsed_sec(programStart);
        print_progress(elapsed, "final");
    }

    if (bestHistory.empty()) {
        remember_best_snapshot();
    }

    auto build_snapshot = [&](const BestSnapshot& snapshot) {
        BuiltSnapshot built;
        built.snapshot = snapshot;
        built.eval = snapshot.eval;
        built.cache = snapshot.cache;

        const TransitionCache& cache = caches[snapshot.cache];
        string ans = build_buttons_for_order(
            cache,
            snapshot.order,
            snapshot.eval.completed,
            &built.compressionLog,
            &built.sPrefixLog,
            &built.sBothLog,
            &built.sPostfixLog,
            &built.sSwitchLog
        );
        if ((int)ans.size() > T) {
            ans.resize(T);
        }
        if (ans.empty()) {
            built.result.completed = -1;
            built.result.outputLen = INF;
            return built;
        }

        if (built.sSwitchLog.used) {
            if (built.sSwitchLog.mode == EDGE_S_PREFIX) {
                built.result.macroBody = "S" + cache.macroBody;
            } else if (built.sSwitchLog.mode == EDGE_S_POSTFIX) {
                built.result.macroBody = cache.macroBody + "S";
            } else {
                built.result.macroBody = "S" + cache.macroBody + "S";
            }
        } else if (built.sPrefixLog.used) {
            built.result.macroBody = "S" + cache.macroBody;
        } else if (built.sBothLog.used) {
            built.result.macroBody = "S" + cache.macroBody + "S";
        } else if (built.sPostfixLog.used) {
            built.result.macroBody = cache.macroBody + "S";
        } else {
            built.result.macroBody = cache.macroBody;
        }
        built.result.completed = snapshot.eval.completed;
        built.result.primitiveLen = snapshot.eval.phys;
        built.result.outputLen = (int)ans.size();
        built.result.ans = std::move(ans);
        built.blockExactDpBefore = built.result.outputLen;
        built.blockExactDpAfter = built.result.outputLen;

        return built;
    };

    auto better_built = [&](const BuiltSnapshot& a, const BuiltSnapshot& b) {
        if (a.result.completed != b.result.completed) {
            return a.result.completed > b.result.completed;
        }
        if (a.result.outputLen != b.result.outputLen) {
            return a.result.outputLen < b.result.outputLen;
        }
        return a.result.primitiveLen < b.result.primitiveLen;
    };

    BuiltSnapshot chosenBuilt;
    bool hasChosenBuilt = false;
    int postprocessTried = 0;
    int postprocessRankingTried = 0;
    int postprocessHistoryTried = 0;
    bool postprocessTimedOut = false;

    auto try_postprocess_snapshot = [&](const BestSnapshot& snapshot, int& counter) {
        if (elapsed_sec(programStart) >= SA_POSTPROCESS_TIME_LIMIT_SEC) {
            postprocessTimedOut = true;
            return false;
        }

        BuiltSnapshot built = build_snapshot(snapshot);
        postprocessTried++;
        counter++;
        if (built.result.completed < 0) return true;
        if (!hasChosenBuilt || better_built(built, chosenBuilt)) {
            chosenBuilt = std::move(built);
            hasChosenBuilt = true;
        }
        return true;
    };

    // まずSA評価ランキング上位を試す。マクロ本体が重複しないように保持しているので、
    // 後処理で逆転しやすい多様な候補を先に見る。
    for (const BestSnapshot& snapshot : bestRanking) {
        if (!try_postprocess_snapshot(snapshot, postprocessRankingTried)) break;
    }

    // 残り時間があれば、直近のbest更新履歴を新しい順に試す。
    if (!postprocessTimedOut) {
        for (int hi = (int)bestHistory.size() - 1; hi >= 0; hi--) {
            if (!try_postprocess_snapshot(bestHistory[hi], postprocessHistoryTried)) break;
        }
    }

    if (!hasChosenBuilt) {
        BuiltSnapshot built = build_snapshot(BestSnapshot{bestOrder, bestCache, bestEval});
        chosenBuilt = std::move(built);
        hasChosenBuilt = true;
    }

    // stage3 は、stage1/2 まで終えた最良候補1件だけに対して行う。
    // まず順序全体を block exact DP で一通り走査し、その後は時間が残る限り
    // ランダム区間の block exact DP を繰り返す。改善はその場で順次反映する。
    if (ENABLE_BLOCK_EXACT_DP_FINISH &&
        hasChosenBuilt &&
        chosenBuilt.result.completed >= 0 &&
        chosenBuilt.cache >= 0 &&
        chosenBuilt.cache < (int)caches.size() &&
        elapsed_sec(programStart) < BLOCK_EXACT_DP_DEADLINE_SEC) {
        chosenBuilt.blockExactDpTried = true;
        chosenBuilt.blockExactDpBefore = chosenBuilt.result.outputLen;
        chosenBuilt.blockExactDpAfter = chosenBuilt.result.outputLen;

        BestSnapshot blockImproved;
        BestSnapshot deterministicImproved = chosenBuilt.snapshot;
        BlockExactDpLog blockLog;
        blockLog.deterministicAfter = chosenBuilt.result.outputLen;
        blockLog.randomAfter = chosenBuilt.result.outputLen;
        auto blockDeadline = programStart +
            chrono::duration_cast<chrono::steady_clock::duration>(
                chrono::duration<double>(BLOCK_EXACT_DP_DEADLINE_SEC)
            );
        if (improve_order_blocks_exact_dp(
                caches[chosenBuilt.cache],
                chosenBuilt.snapshot,
                blockImproved,
                &deterministicImproved,
                rng,
                blockDeadline,
                &blockLog
            )) {
            if (blockLog.deterministicImprovedBlocks > 0) {
                BuiltSnapshot detBuilt = build_snapshot(deterministicImproved);
                if (detBuilt.result.completed >= 0) {
                    blockLog.deterministicAfter = detBuilt.result.outputLen;
                }
            }

            BuiltSnapshot blockBuilt = build_snapshot(blockImproved);
            if (blockBuilt.result.completed >= 0) {
                blockLog.randomAfter = blockBuilt.result.outputLen;
            } else {
                blockLog.randomAfter = blockLog.deterministicAfter;
            }
            if (blockBuilt.result.completed >= 0 &&
                better_built(blockBuilt, chosenBuilt)) {
                RegistrationCompressionLog compressionLog = chosenBuilt.compressionLog;
                EdgeSMacroLog sPrefixLog = chosenBuilt.sPrefixLog;
                EdgeSMacroLog sBothLog = chosenBuilt.sBothLog;
                EdgeSMacroLog sPostfixLog = chosenBuilt.sPostfixLog;
                EdgeSSwitchLog sSwitchLog = chosenBuilt.sSwitchLog;

                blockBuilt.compressionLog = compressionLog;
                blockBuilt.sPrefixLog = sPrefixLog;
                blockBuilt.sBothLog = sBothLog;
                blockBuilt.sPostfixLog = sPostfixLog;
                blockBuilt.sSwitchLog = sSwitchLog;
                blockBuilt.blockExactDpTried = true;
                blockBuilt.blockExactDpUsed = true;
                blockBuilt.blockExactDpBefore = chosenBuilt.result.outputLen;
                blockBuilt.blockExactDpAfter = blockBuilt.result.outputLen;
                blockBuilt.blockExactDpLog = blockLog;

                chosenBuilt = std::move(blockBuilt);
            } else {
                chosenBuilt.blockExactDpLog = blockLog;
            }
        } else {
            if (blockLog.deterministicImprovedBlocks > 0) {
                BuiltSnapshot detBuilt = build_snapshot(deterministicImproved);
                if (detBuilt.result.completed >= 0) {
                    blockLog.deterministicAfter = detBuilt.result.outputLen;
                }
            }
            blockLog.randomAfter = blockLog.deterministicAfter;
            chosenBuilt.blockExactDpLog = blockLog;
        }
    }

    double elapsed = elapsed_sec(programStart);

    SolveResult ret = std::move(chosenBuilt.result);
    bestEval = chosenBuilt.eval;
    bestCache = chosenBuilt.cache;
    RegistrationCompressionLog compressionLog = chosenBuilt.compressionLog;
    EdgeSMacroLog sPrefixLog = chosenBuilt.sPrefixLog;
    EdgeSMacroLog sBothLog = chosenBuilt.sBothLog;
    EdgeSMacroLog sPostfixLog = chosenBuilt.sPostfixLog;
    EdgeSSwitchLog sSwitchLog = chosenBuilt.sSwitchLog;

    cerr << "[postprocess-history] "
         << "tried=" << postprocessTried
         << " rankingTried=" << postprocessRankingTried
         << " historyTried=" << postprocessHistoryTried
         << " timeout=" << (postprocessTimedOut ? 1 : 0)
         << " chosenCache=" << bestCache
         << " chosenEvalBtn=" << bestEval.btn
         << " chosenOutput=" << ret.outputLen
         << "\n";

    if (compressionLog.used) {
        cerr << "[postprocess-stage1-compress] "
             << "mode=" << compression_mode_name(compressionLog)
             << " normal=" << compressionLog.normalLen
             << " result=" << compressionLog.compressedLen
             << " saved=" << compressionLog.saved
             << " runLen=" << compressionLog.runLen
             << " replacedRuns=" << compressionLog.replacedRuns
             << " firstRunPos=" << compressionLog.firstRunPos
             << "\n";
    } else {
        cerr << "[postprocess-stage1-compress] "
             << "mode=none"
             << " normal=" << compressionLog.normalLen
             << "\n";
    }

    if (sPrefixLog.tried) {
        cerr << "[postprocess-stage2-edge-s] "
             << "mode=prefix"
             << " used=" << (sPrefixLog.used ? 1 : 0)
             << " input=" << sPrefixLog.inputLen
             << " result=" << sPrefixLog.candidateLen
             << " edgeSaved=" << sPrefixLog.saved
             << " totalSaved=" << sPrefixLog.totalSaved
             << " normal=" << sPrefixLog.normalLen
             << " absorbedS=" << sPrefixLog.absorbedS
             << " guardedP=" << sPrefixLog.guardedP
             << " freeP=" << sPrefixLog.freeP
             << "\n";
    }
    if (sBothLog.tried) {
        cerr << "[postprocess-stage2-edge-s] "
             << "mode=both"
             << " used=" << (sBothLog.used ? 1 : 0)
             << " input=" << sBothLog.inputLen
             << " result=" << sBothLog.candidateLen
             << " edgeSaved=" << sBothLog.saved
             << " totalSaved=" << sBothLog.totalSaved
             << " normal=" << sBothLog.normalLen
             << " absorbedS=" << sBothLog.absorbedS
             << " guardedP=" << sBothLog.guardedP
             << " freeP=" << sBothLog.freeP
             << "\n";
    }
    if (sPostfixLog.tried) {
        cerr << "[postprocess-stage2-edge-s] "
             << "mode=postfix"
             << " used=" << (sPostfixLog.used ? 1 : 0)
             << " input=" << sPostfixLog.inputLen
             << " result=" << sPostfixLog.candidateLen
             << " edgeSaved=" << sPostfixLog.saved
             << " totalSaved=" << sPostfixLog.totalSaved
             << " normal=" << sPostfixLog.normalLen
             << " absorbedS=" << sPostfixLog.absorbedS
             << " guardedP=" << sPostfixLog.guardedP
             << " freeP=" << sPostfixLog.freeP
             << "\n";
    }
    if (sSwitchLog.tried) {
        cerr << "[postprocess-stage2-edge-s-switch] "
             << "mode=" << edge_s_mode_name(sSwitchLog.mode)
             << " used=" << (sSwitchLog.used ? 1 : 0)
             << " input=" << sSwitchLog.inputLen
             << " result=" << sSwitchLog.candidateLen
             << " edgeSaved=" << sSwitchLog.saved
             << " totalSaved=" << sSwitchLog.totalSaved
             << " normal=" << sSwitchLog.normalLen
             << " switchUse=" << sSwitchLog.switchUse
             << " switchBackUse=" << sSwitchLog.switchBackUse
             << " noBackBest=" << sSwitchLog.noBackBestLen
             << " switchBackSaved=" << sSwitchLog.switchBackSaved
             << " totalUses=" << sSwitchLog.totalUses
             << " absorbedS=" << sSwitchLog.absorbedS
             << " guardedP=" << sSwitchLog.guardedP
             << " freeP=" << sSwitchLog.freeP
             << "\n";
    }

    const char* finalEdgeSMode = "none";
    int finalEdgeSInput = compressionLog.compressedLen;
    int finalEdgeSResult = ret.outputLen;
    int finalEdgeSSaved = 0;
    int finalSwitchUse = 0;
    int finalSwitchBackUse = 0;
    int finalNoBackBest = INF;
    int finalSwitchBackSaved = 0;
    int finalTotalUses = 0;
    if (sSwitchLog.used) {
        if (sSwitchLog.mode == EDGE_S_PREFIX) {
            finalEdgeSMode = "switch-prefix";
        } else if (sSwitchLog.mode == EDGE_S_POSTFIX) {
            finalEdgeSMode = "switch-postfix";
        } else {
            finalEdgeSMode = "switch-both";
        }
        finalEdgeSInput = sSwitchLog.inputLen;
        finalEdgeSResult = sSwitchLog.candidateLen;
        finalEdgeSSaved = sSwitchLog.totalSaved;
        finalSwitchUse = sSwitchLog.switchUse;
        finalSwitchBackUse = sSwitchLog.switchBackUse;
        finalNoBackBest = sSwitchLog.noBackBestLen;
        finalSwitchBackSaved = sSwitchLog.switchBackSaved;
        finalTotalUses = sSwitchLog.totalUses;
    } else if (sPrefixLog.used) {
        finalEdgeSMode = "prefix";
        finalEdgeSInput = sPrefixLog.inputLen;
        finalEdgeSResult = sPrefixLog.candidateLen;
        finalEdgeSSaved = sPrefixLog.totalSaved;
    } else if (sBothLog.used) {
        finalEdgeSMode = "both";
        finalEdgeSInput = sBothLog.inputLen;
        finalEdgeSResult = sBothLog.candidateLen;
        finalEdgeSSaved = sBothLog.totalSaved;
    } else if (sPostfixLog.used) {
        finalEdgeSMode = "postfix";
        finalEdgeSInput = sPostfixLog.inputLen;
        finalEdgeSResult = sPostfixLog.candidateLen;
        finalEdgeSSaved = sPostfixLog.totalSaved;
    }
    cerr << "[postprocess-stage2-edge-s-final] "
         << "mode=" << finalEdgeSMode
         << " input=" << finalEdgeSInput
         << " result=" << finalEdgeSResult
         << " finalOutput=" << ret.outputLen
         << " totalSaved=" << finalEdgeSSaved
         << " switchUse=" << finalSwitchUse
         << " switchBackUse=" << finalSwitchBackUse
         << " noBackBest=" << finalNoBackBest
         << " switchBackSaved=" << finalSwitchBackSaved
         << " totalUses=" << finalTotalUses
         << "\n";

    cerr << "[postprocess-stage3-block-dp] "
         << "tried=" << (chosenBuilt.blockExactDpTried ? 1 : 0)
         << " used=" << (chosenBuilt.blockExactDpUsed ? 1 : 0)
         << " k=" << chosenBuilt.blockExactDpLog.k
         << " blocks=" << chosenBuilt.blockExactDpLog.blocks
         << " improvedBlocks=" << chosenBuilt.blockExactDpLog.improvedBlocks
         << " randomBlocks=" << chosenBuilt.blockExactDpLog.randomBlocks
         << " randomImproved=" << chosenBuilt.blockExactDpLog.randomImprovedBlocks
         << " timeout=" << (chosenBuilt.blockExactDpLog.timeout ? 1 : 0)
         << " time=" << fixed << setprecision(4) << chosenBuilt.blockExactDpLog.time
         << " before=" << chosenBuilt.blockExactDpBefore
         << " after=" << chosenBuilt.blockExactDpAfter
         << " saved=" << (chosenBuilt.blockExactDpBefore - chosenBuilt.blockExactDpAfter)
         << "\n";

    cerr << "[postprocess-stage3-block-dp-det] "
         << "blocks=" << chosenBuilt.blockExactDpLog.blocks
         << " improved=" << chosenBuilt.blockExactDpLog.deterministicImprovedBlocks
         << " before=" << chosenBuilt.blockExactDpBefore
         << " after=" << chosenBuilt.blockExactDpLog.deterministicAfter
         << " saved=" << max(0, chosenBuilt.blockExactDpBefore - chosenBuilt.blockExactDpLog.deterministicAfter)
         << "\n";

    cerr << "[postprocess-stage3-block-dp-random] "
         << "blocks=" << chosenBuilt.blockExactDpLog.randomBlocks
         << " improved=" << chosenBuilt.blockExactDpLog.randomImprovedBlocks
         << " before=" << chosenBuilt.blockExactDpLog.deterministicAfter
         << " after=" << chosenBuilt.blockExactDpLog.randomAfter
         << " saved=" << max(0, chosenBuilt.blockExactDpLog.deterministicAfter - chosenBuilt.blockExactDpLog.randomAfter)
         << "\n";

    cerr << "[anneal-macro] "
         << "macro=" << ret.macroBody
         << " len=" << ret.macroBody.size()
         << " completed=" << ret.completed << "/" << M
         << " primitive=" << ret.primitiveLen
         << " output=" << ret.outputLen
         << " evalBtn=" << bestEval.btn
         << " evalPhys=" << bestEval.phys
         << " iterations=" << iter
         << " caches=" << caches.size()
         << " macroTry=" << macroTries
         << " macroAcc=" << macroAccepted
         << " time=" << fixed << setprecision(4) << elapsed
         << "\n";

    return ret;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto totalStart = PROGRAM_START_TIME;

    cin >> N >> M >> T;

    vwall.resize(N);
    for (int i = 0; i < N; i++) {
        cin >> vwall[i];
    }

    hwall.resize(N - 1);
    for (int i = 0; i < N - 1; i++) {
        cin >> hwall[i];
    }

    ballPos.resize(M);
    basketPos.resize(M);

    for (int k = 0; k < M; k++) {
        int b, c, d, e;
        cin >> b >> c >> d >> e;
        ballPos[k] = b * N + c;
        basketPos[k] = d * N + e;
    }

    SNUM = N * N * 4;
    assert(SNUM <= MAX_SNUM);
    build_static_state_caches();
    macroBodies.reserve(256);
    macroBodyIds.reserve(256);
    macroBodyNextCache.reserve(256);
    macroBodyMoveTreeCache.reserve(256);
    macroBodyPathTreeCache.reserve(256);

    double prepTime = elapsed_sec(totalStart);

    cerr << "[prep] "
         << "N=" << N
         << " M=" << M
         << " T=" << T
         << " SNUM=" << SNUM
         << " time=" << fixed << setprecision(4) << prepTime
         << "\n";

    // 1本の焼きなましで、ボール順序とマクロ本体の両方を動かす。
    SolveResult best = solve_annealing_order_and_macro(totalStart);

    double totalTime = elapsed_sec(totalStart);

    cerr << "[chosen] "
         << "macro=" << best.macroBody
         << " len=" << best.macroBody.size()
         << " completed=" << best.completed << "/" << M
         << " primitive=" << best.primitiveLen
         << " output=" << best.outputLen
         << " totalTime=" << fixed << setprecision(4) << totalTime
         << "\n";

    for (char ch : best.ans) {
        cout << ch << '\n';
    }

    // 巨大な経路キャッシュのグローバルデストラクタを待つと、計測外で時間を食うことがある。
    // 出力だけ明示的に flush して、以後の解放処理は飛ばして終了する。
    cout.flush();
    _Exit(0);
}
