#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

#define REP(i, n) for (int i = 0; i < (int)(n); ++ (i))
#define REP3(i, m, n) for (int i = (m); (i) < (int)(n); ++ (i))

uint64_t rand64() {
    static uint64_t x = 88172645463325252ULL;
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
}

double rand_p() { return rand64() * (1.0 / UINT64_MAX); }

void shuffle(std::vector<int> &array) {
    for (int i = array.size() - 1; i >= 1; i--) {
        int j = rand64() % (i + 1);
        std::swap(array[i], array[j]);
    }
}

using namespace std::chrono;
using namespace std;
struct Timer {
    system_clock::time_point start;

    Timer() { reset(); }

    void reset() { start = system_clock::now(); }

    double sec_elapsed() const {
        return duration_cast<milliseconds>(system_clock::now() - start).count() / 1000.0;
    }
};

constexpr int N = 20;
constexpr int M = 40;
int P[M][2];
int NUMBER[N * N];

void input() {
    int tmp;
    cin >> tmp >> tmp;
    REP(i, M) { cin >> P[i][0] >> P[i][1]; }
    REP(i, N) REP(j, N) { NUMBER[i * N + j] = 0; }
    REP(i, M) { NUMBER[P[i][0] * N + P[i][1]] = i; }
}

int di[] = {-1, 1, 0, 0};
int dj[] = {0, 0, -1, 1};
string dir[] = {"U", "D", "L", "R"};

namespace util {
    inline bool in_board(int i, int j) { return 0 <= i && i < N && 0 <= j && j < N; }
}

struct State {
    std::array<bool, N * N> toPlaceBlock;  // true のときにブロックをおくべき
    std::vector<int> toPlaceBlockList; // ブロックをおくべき位置のリスト

    std::vector<int> order;

    // 以下はシミュレーションにて変更される
    std::array<int, N * N> board;  // 0 のときにブロックは存在しない。1のときに存在してるだけ、2のときに1度は利用された
    std::vector<int> set;
    std::vector<int> use;
};

void initialize(State &state) {
    state.toPlaceBlock.fill(false);
    state.toPlaceBlockList.clear();
    state.board.fill(false);
    state.order = {0, 1, 2, 3};
    state.set.clear();
    state.use.clear();
}

namespace neighbor {
    // ブロックをランダムに1つ削除する
    void delete_block(State &state) {
        if (state.toPlaceBlockList.size() == 0) return; // ブロックが存在しないなら近傍を適用しない

        int index = rand64() % state.toPlaceBlockList.size();
        state.toPlaceBlock[state.toPlaceBlockList[index]] = false;
        state.toPlaceBlockList.erase(state.toPlaceBlockList.begin() + index);
    }

    // ブロックをランダムに1つ追加する
    void add_block(State &state) {
        while (true) {
            int pos = rand64() % (N * N);
            if (NUMBER[pos] != 0) continue;
            if (state.toPlaceBlock[pos]) continue;

            state.toPlaceBlock[pos] = true;
            state.toPlaceBlockList.push_back(pos);
            break;
        }
    }

    // ブロックの位置をランダムに変更する
    void change_block(State &state) {
        delete_block(state);
        add_block(state);
    }

    // BFSの順番をシャッフルする
    void shuffle_order(State &state) {
        shuffle(state.order);
    }
        
    void modify(State &state) {
        auto rand = rand_p();
        if (rand < 0.05) shuffle_order(state);
        else change_block(state);
    }

    std::vector<int> toPlaceBlockListOld;
    std::vector<int> shuffle_orderOld;
    void snapshot(State &state) {
        toPlaceBlockListOld = state.toPlaceBlockList;
        shuffle_orderOld = state.order;
    }

    void rollback(State &state) {
        REP(i, state.toPlaceBlockList.size()) {
            state.toPlaceBlock[state.toPlaceBlockList[i]] = false;
        }
        REP(i, toPlaceBlockListOld.size()) {
            state.toPlaceBlock[toPlaceBlockListOld[i]] = true;
        }
        state.toPlaceBlockList = toPlaceBlockListOld;
        state.order = shuffle_orderOld;
    }
}

// ブロックをおいて新しいパスを返す
std::vector<int> simulate_move_and_place(int this_i, int this_j, std::vector<int> &path, State &state) {
    static std::array<int, N * N> to_visit_count{};

    // to_visit_count の更新
    // path に沿って移動する。滑走を考慮する
    {
        int tmp_i = this_i;
        int tmp_j = this_j;
        REP(k, path.size()) {
            if (path[k] < 4) {
                // 通常移動
                tmp_i += di[path[k]];
                tmp_j += dj[path[k]];
                to_visit_count[tmp_i * N + tmp_j] += 1;
            } else {
                // 滑走移動
                int d = path[k] - 4;
                while (true) {
                    int ti = tmp_i + di[d];
                    int tj = tmp_j + dj[d];
                    if (!util::in_board(ti, tj)) break;
                    if (state.board[ti * N + tj] > 0) {
                        if (state.board[ti * N + tj] == 1)
                        {
                            state.board[ti * N + tj] = 2;
                            state.use.push_back(ti * N + tj);
                        }
                        break; // ブロック/外
                    }
                    tmp_i = ti; tmp_j = tj;
                    to_visit_count[tmp_i * N + tmp_j] += 1;
                }
            }
        }
    }

    std::vector<int> result;

    // path に沿って移動する。滑走を考慮する
    REP(k, path.size()) {
        // 通常の移動の時
        if (path[k] < 4) {
            this_i += di[path[k]];
            this_j += dj[path[k]];
            to_visit_count[this_i * N + this_j] -= 1;
        } else {
            // 滑走の時
            int d = path[k] - 4;
            while (true) {
                int ti = this_i + di[d];
                int tj = this_j + dj[d];
                if (!util::in_board(ti, tj) || state.board[ti * N + tj]) break; // ブロック/外
                this_i = ti; this_j = tj;
                to_visit_count[this_i * N + this_j] -= 1;
            }
        }

        result.push_back(path[k]);

        // 4近傍を見て、toPlaceBlock = true ならブロックをおく
        REP(kk, 4)
        {
            int neighbor_i = this_i + di[kk];
            int neighbor_j = this_j + dj[kk];
            if (!util::in_board(neighbor_i, neighbor_j) || state.board[neighbor_i * N + neighbor_j]) break; // ブロック/外
            if (to_visit_count[neighbor_i * N + neighbor_j] != 0) continue; // もう一度訪れる可能性がある

            if (state.toPlaceBlock[neighbor_i * N + neighbor_j]) {
                state.board[neighbor_i * N + neighbor_j] = true;
                result.push_back(8 + kk);
                state.set.emplace_back(neighbor_i * N + neighbor_j);
            }
        }
    }
    return result;
};

// 現在地点(this_i, this_j) から (next_i, next_j) に到達するための最短手数を求め、その移動方法を返す
// 到達不可能な時には {false, {}} を返す
std::pair<bool, std::vector<int>> solve(int this_i, int this_j, int next_i, int next_j, State &state) {
    constexpr int SZ  = N * N;
    constexpr int INF = 1e9;

    auto unpack = [](int v){ return std::pair<int,int>{ v / N, v % N }; };

    static int  dist[SZ];
    static int  prev_v[SZ];
    static char prev_code[SZ];   // 0..7 (上記エンコード)
    std::fill(dist, dist + SZ, INF);

    int s = this_i * N + this_j;
    int g = next_i * N + next_j;

    std::deque<int> que;
    dist[s] = 0;
    que.push_back(s);

    /* ---------- BFS 本体 ---------- */
    while (!que.empty()) {
        int v = que.front(); que.pop_front();
        if (v == g) break;

        auto [i, j] = unpack(v);

        /* 4 方向について (M と S の 2 種エッジ) */
        REP(d, 4) {
            int ni = i + di[state.order[d]];
            int nj = j + dj[state.order[d]];

            /* --- 1 マス移動 (M) --- */
            if (util::in_board(ni, nj) && !state.board[ni * N + nj]) {
                int nv = ni * N + nj;
                if (dist[nv] > dist[v] + 1) {
                    dist[nv]      = dist[v] + 1;
                    prev_v[nv]    = v;
                    prev_code[nv] = state.order[d];          // 0–3
                    if (nv == g) goto reconstruct;
                    que.push_back(nv);
                }
            }

            /* --- 滑走 (S) --- */
            int si = i, sj = j;
            while (true) {
                int ti = si + di[state.order[d]];
                int tj = sj + dj[state.order[d]];
                if (!util::in_board(ti, tj) || state.board[ti * N + tj]) break; // ブロック/外
                si = ti; sj = tj;
            }
            if (si != i || sj != j) {           // 少なくとも 1 マス進んだ
                int nv = si * N + sj;
                if (dist[nv] > dist[v] + 1) {
                    dist[nv]      = dist[v] + 1;
                    prev_v[nv]    = v;
                    prev_code[nv] = state.order[d] + 4;      // 4–7 (滑走)
                    if (nv == g) goto reconstruct;
                    que.push_back(nv);
                }
            }
        }
    }

    return {false, {}}; // 到達不能

reconstruct:
    std::vector<int> path;
    for (int v = g; v != s; v = prev_v[v]) path.push_back(prev_code[v]);
    std::reverse(path.begin(), path.end());
    return {true, std::move(path)};
}


std::vector<int> annealing(State cur_state, const Timer &timer, const double time_limit) {
    // 温度設定
    const double start_temp = 1e3;
    const double end_temp = 1e0;

    double inv_temp = 1.0 / start_temp;

    int iter_count = 0;
    int cur_score = -100000000;

    std::vector<int> best_answer;
    int best_score = -100000000;

    double start_time = timer.sec_elapsed();
    double now = start_time;

    while (now < time_limit) {
        {
            now = timer.sec_elapsed();
            const double time_ratio = (now - start_time) / (time_limit - start_time);
            const double temp = start_temp + (end_temp - start_temp) * time_ratio;  // 線形
            //const double temp = start_temp * std::pow(end_temp / start_temp, time_ratio);  // pow 	
            inv_temp = 1.0 / temp;
        }

        neighbor::snapshot(cur_state);
        cur_state.board.fill(false);
        cur_state.set.clear();
        cur_state.use.clear();
        neighbor::modify(cur_state);
        
        std::vector<int> answer;
        bool can_move = true;
        REP(k, M - 1) {
            int this_i = P[k][0];
            int this_j = P[k][1];
            int next_i = P[k + 1][0];
            int next_j = P[k + 1][1];

            // 1. u から v までの最短経路を求める
            auto [can, path] = solve(this_i, this_j, next_i, next_j, cur_state);
            if (!can) {
                can_move = false;
                break;
            }
            auto result = simulate_move_and_place(this_i, this_j, path, cur_state);
            answer.insert(answer.end(), result.begin(), result.end());
        }

        if (!can_move) {
            neighbor::rollback(cur_state);
            continue;
        }

        int new_score = -(answer.size() + cur_state.use.size() - cur_state.set.size()) * 1000;

        double prob = std::exp((new_score - cur_score) * inv_temp);

        if (prob > rand_p()) {
            cur_score = new_score;
        } else {
            neighbor::rollback(cur_state);
        }
        if (cur_score > best_score) {
            best_score = cur_score;
            best_answer = answer;
        }
        iter_count += 1;
    }
    std::cerr << iter_count << std::endl;
    return best_answer;
}

void shrink_unused_blocks(std::vector<int>& codes)
{
    /* ---------- 1 回目：使用フラグを付ける ---------------------------- */
    bool board[400]{};          // ブロック有無
    bool used [400]{};          // 壁が停止に使われたか
    int  ci = P[0][0], cj = P[0][1];

    for (int cd : codes) {
        if (cd < 4) {                       // M
            ci += di[cd];  cj += dj[cd];
        }
        else if (cd < 8) {                  // S
            int d = cd - 4;
            int ni = ci, nj = cj;
            while (true) {
                int ti = ni + di[d], tj = nj + dj[d];
                if (!util::in_board(ti,tj) || board[ti*N+tj]) break;
                ni = ti;  nj = tj;
            }
            // 次セルに壁ならその壁に衝突
            int stop_i = ni + di[d], stop_j = nj + dj[d];
            if (util::in_board(stop_i, stop_j) && board[stop_i*N+stop_j])
                used[stop_i*N+stop_j] = true;
            ci = ni; cj = nj;
        }
        else {                              // A (8..11)
            int d = cd & 3;
            int bi = ci + di[d], bj = cj + dj[d];
            int pos = bi * N + bj;
            board[pos] = !board[pos];       // toggle
        }
    }

    /* ---------- 2 回目：不要な A を落とす ---------------------------- */
    std::vector<int> filtered;
    std::fill(std::begin(board), std::end(board), false);
    ci = P[0][0]; cj = P[0][1];

    for (int cd : codes) {
        if (cd < 4) {                       // M
            ci += di[cd];  cj += dj[cd];
            filtered.push_back(cd);
        }
        else if (cd < 8) {                  // S
            int d = cd - 4;
            while (true) {
                int ti = ci + di[d], tj = cj + dj[d];
                if (!util::in_board(ti,tj) || board[ti*N+tj]) break;
                ci = ti; cj = tj;
            }
            filtered.push_back(cd);
        }
        else {                              // A
            int d = cd & 3;
            int bi = ci + di[d], bj = cj + dj[d];
            int pos = bi * N + bj;

            if (!board[pos]) {                  // 壁を置く
                if (used[pos]) {                // 使われる壁だけ出力
                    board[pos] = true;
                    filtered.push_back(cd);
                }
            } else {                            // 壁を外す
                if (used[pos]) {                // 使われた壁なら外しも必要
                    board[pos] = false;
                    filtered.push_back(cd);
                }
            }
        }
    }
    codes.swap(filtered);                       // 置き換え
}

int main() {
    Timer timer;
    input();

    State state;
    initialize(state);

    // ランダムにおく
    REP(k, 60) {
        neighbor::add_block(state);
    }

    auto best_answer = annealing(state, timer, 1.95);
    shrink_unused_blocks(best_answer);

    // 答えを出力、利用しないブロックは出力しない
    REP(i, best_answer.size()) {
        if (best_answer[i] < 4) cout << "M ";
        else if (best_answer[i] < 8) cout << "S ";
        else if (best_answer[i] < 12) cout << "A ";
        cout << dir[best_answer[i] % 4] << endl;
    }
}
