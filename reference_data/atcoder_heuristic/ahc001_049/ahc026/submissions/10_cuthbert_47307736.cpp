#pragma GCC optimize("unroll-loops")

#pragma GCC optimize("O3,inline")  // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens


#pragma GCC target("bmi,bmi2,lzcnt,popcnt")                       // bit manipulation
#pragma GCC target("movbe")                                       // byte swap
#pragma GCC target("aes,pclmul,rdrnd")                            // encryption
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")  // SIMD

#undef _GLIBCXX_DEBUG  // disable run-time bound checking, etc
#define NDEBUG

#define FLAG_PRINT_SCORE
#define FLAG_PRINT_SCORE_END

// ---------------------------------------- template ----------------------------------------

#define _GLIBCXX_DEBUG
// #include <bits/stdc++.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

#include <unistd.h>
#include <x86intrin.h>

// #pragma GCC diagnostic ignored "-Wconversion"
// #include "Eigen/Dense"
// #pragma GCC diagnostic pop
// using Eigen::MatrixXd;
// using Eigen::VectorXd;

using namespace std;
using std::cin;
using std::cout;
using ll = long long;
using ull = unsigned long long;
using pi = pair<int, int>;

#define bit(n, k) (((n) >> (k)) & 1) /*nのk bit目*/

#ifdef ONLINE_JUDGE
const double TIME_RATIO = 1.0;
#else
const double TIME_RATIO = 0.1;
// const double TIME_RATIO = 1.0;
// const double TIME_RATIO = 1.5;
#endif

constexpr int INF = 1000000000;
constexpr ll INFLL = 1ll << 60;
constexpr int TIME_LIMIT = 1950;

template <class T>
bool chmin(T& a, const T& b) {
    if (a > b) {
        a = b;
        return 1;
    } else
        return 0;
}
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return 1;
    } else
        return 0;
}

template <typename A, size_t N, typename T>
void Fill(A (&array)[N], const T& val) {
    std::fill((T*)array, (T*)(array + N), val);
}

auto start = chrono::system_clock::now();
int get_time() {
    int elapsed = (int)(TIME_RATIO * double(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start).count()));
    return elapsed;
}
int get_time_remaining() {
    int time_remaining = TIME_LIMIT - get_time();
    return time_remaining;
}
void print_time() {
    int elapsed = get_time();
    cerr << "[DATA] totaltime = " << elapsed << endl;
}

static uint32_t randxor() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

int randrange(int n) {
    assert(0 <= n);
    // return (randxor() / 123) % n;
    return randxor() % n;
}

int randrange(int l, int r) {
    assert(l <= r);
    return randrange(r - l) + l;
}

double randdouble() {
    // (0, 1])
    return (double)randxor() / UINT32_MAX;
    // return (double)randxor() / UINT32_MAX + std::numeric_limits<double>::min();
}

bool randbool(double p = 0.5) {
    return randdouble() < p;
}


// ---------------------------------------- variables ----------------------------------------
const int N = 200;
const int M = 10;
int B[10][20];

// ---------------------------------------- solver ----------------------------------------
class Solver {
    using Color = int;

public:
    int score;
    int board[M][N];
    int pile_sz[M];
    int idx2pile[N];
    int idx2height[N];
    int idx_nxt;
    vector<pair<int, int>> operations;
    Solver() {
        Fill(board, -1);
        Fill(pile_sz, N / M);
        score = 0;
        idx_nxt = 0;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N / M; j++) {
                board[i][j] = B[i][j];
                idx2pile[B[i][j]] = i;
                idx2height[B[i][j]] = j;
            }
        }
    }

    // copy
    Solver(const Solver& solver) {
        score = solver.score;
        idx_nxt = solver.idx_nxt;
        copy(solver.board[0], solver.board[M], board[0]);
        copy(solver.pile_sz, solver.pile_sz + M, pile_sz);
        copy(solver.idx2pile, solver.idx2pile + N, idx2pile);
        copy(solver.idx2height, solver.idx2height + N, idx2height);
    }

    pair<int, int> step() {
        // simple greedy
        assert(idx_nxt != N);
        int idx_nxt_pile = idx2pile[idx_nxt];
        int idx_nxt_height = idx2height[idx_nxt];
        if (idx_nxt_height == pile_sz[idx_nxt_pile] - 1) {
            // box on top of pile
            return {idx_nxt, -1};
        }
        // move box on pile
        int idxmove = board[idx_nxt_pile][idx_nxt_height + 1];
        // search for pile with maximum minimum number
        bool seen[M] = {};
        int cnt = 0;
        for (int idx = idx_nxt; idx < N; idx++) {
            int pile = idx2pile[idx];
            if (seen[pile])
                continue;
            seen[pile] = true;
            cnt++;
            if (cnt == M - 1) {
                // seen almost all piles
                break;
            }
        }
        // seen[pile] == false
        int pile = int(min_element(seen, seen + M) - seen);
        return {idxmove, pile};
    }

    pair<int, int> step_greedy_3() {
        // 少し改良したgreedy
        // 山を移す時に、ペナルティをつける
        assert(idx_nxt != N);
        int idx_nxt_pile = idx2pile[idx_nxt];
        int idx_nxt_height = idx2height[idx_nxt];
        if (idx_nxt_height == pile_sz[idx_nxt_pile] - 1) {
            // box on top of pile
            return {idx_nxt, -1};
        }
        // move box on pile
        int idxmove = board[idx_nxt_pile][idx_nxt_height + 1];
        // search for pile with maximum minimum number
        bool seen[M] = {};
        int cnt = 0;
        int idx;
        int idx_min_on_top_of_idx_nxt = -1;
        for (idx = idx_nxt; idx < N; idx++) {
            int pile = idx2pile[idx];
            if (idx != idx_nxt && idx_min_on_top_of_idx_nxt == -1 && pile == idx_nxt_pile && idx2height[idx] > idx_nxt_height)
                idx_min_on_top_of_idx_nxt = idx;
            if (seen[pile])
                continue;
            seen[pile] = true;
            cnt++;
            if (cnt == M - 1) {
                // seen almost all piles
                break;
            }
        }
        // seen[pile] == false
        int pile = int(min_element(seen, seen + M) - seen);

        if (idx_min_on_top_of_idx_nxt != -1) {
            int idx_min_in_pile_after = N;
            if (pile_sz[pile] != 0)
                idx_min_in_pile_after = *min_element(board[pile], board[pile] + pile_sz[pile]);
            if (idx_min_in_pile_after > idx_min_on_top_of_idx_nxt
                && idx2height[idx_min_on_top_of_idx_nxt] + 1 != pile_sz[idx_nxt_pile]) {
                idxmove = board[idx_nxt_pile][idx2height[idx_min_on_top_of_idx_nxt] + 1];
                // cerr << idx_min_in_pile_after << " " << idx_min_on_top_of_idx_nxt << " " << pile << " " << idx_nxt_pile << endl;
            }
        }
        return {idxmove, pile};
    }

    pair<int, int> step_greedy_5() {
        // 少し改良したgreedy
        // 山を移す時に、ペナルティをつける
        assert(idx_nxt != N);
        int idx_nxt_pile = idx2pile[idx_nxt];
        int idx_nxt_height = idx2height[idx_nxt];
        if (idx_nxt_height == pile_sz[idx_nxt_pile] - 1) {
            // box on top of pile
            return {idx_nxt, -1};
        }
        // move box on pile
        int idxmove = board[idx_nxt_pile][idx_nxt_height + 1];
        // search for pile with maximum minimum number
        bool seen[M] = {};
        int cnt = 0;
        int idx;
        // int pile = -1;
        // for (int i = 0; i < M; i++) {
        //     if (pile_sz[i] == 0) {
        //         pile = i;
        //         break;
        //     }
        // }

        int idx_min_on_top_of_idx_nxt = N;
        int idx_max_on_top_of_idx_nxt = -1;
        for (int i = idx_nxt_height + 1; i < pile_sz[idx_nxt_pile]; i++) {
            chmin(idx_min_on_top_of_idx_nxt, board[idx_nxt_pile][i]);
            chmax(idx_max_on_top_of_idx_nxt, board[idx_nxt_pile][i]);
        }

        for (idx = idx_nxt; idx < idx_max_on_top_of_idx_nxt; idx++) {
            int pile = idx2pile[idx];
            if (seen[pile])
                continue;
            seen[pile] = true;
            cnt++;
            if (cnt == M - 1) {
                // seen almost all piles
                break;
            }
        }
        // seen[pile] == false
        int pile = -1;
        pile = int(min_element(seen, seen + M) - seen);

        bool flag_1 = false;
        if (cnt != M - 1 && idx_max_on_top_of_idx_nxt != -1) {
            for (int idx = idx_max_on_top_of_idx_nxt + 1; idx < N; idx++) {
                int pile_tmp = idx2pile[idx];
                if (!seen[pile_tmp]) {
                    pile = pile_tmp;
                    flag_1 = true;
                    break;
                }
            }
        }

        // if (flag_1) {
        if (false) {
            idxmove = idx_min_on_top_of_idx_nxt;
        } else if (idx_min_on_top_of_idx_nxt != N) {
            // NEW
            int idx_min_in_pile_after = N;
            if (pile_sz[pile] != 0)
                idx_min_in_pile_after = *min_element(board[pile], board[pile] + pile_sz[pile]);
            if (idx_min_in_pile_after > idx_min_on_top_of_idx_nxt
                && idx2height[idx_min_on_top_of_idx_nxt] + 1 != pile_sz[idx_nxt_pile]) {
                idxmove = board[idx_nxt_pile][idx2height[idx_min_on_top_of_idx_nxt] + 1];
                // cerr << idx_min_in_pile_after << " " << idx_min_on_top_of_idx_nxt << " " << pile << " " << idx_nxt_pile << endl;
            }
        }
        return {idxmove, pile};
    }


    pair<int, int> step_greedy_4() {
        // 少し改良したgreedy
        // 山を移す時に、ペナルティをつける
        assert(idx_nxt != N);
        int idx_nxt_pile = idx2pile[idx_nxt];
        int idx_nxt_height = idx2height[idx_nxt];
        if (idx_nxt_height == pile_sz[idx_nxt_pile] - 1) {
            // box on top of pile
            return {idx_nxt, -1};
        }
        // move box on pile
        int idxmove = board[idx_nxt_pile][idx_nxt_height + 1];
        // search for pile with maximum minimum number
        bool seen[M] = {};
        int cnt = 0;
        int idx;
        int pile = -1;
        for (int i = 0; i < M; i++) {
            if (pile_sz[i] == 0) {
                pile = i;
                break;
            }
        }

        if (pile == -1) {
            for (idx = idx_nxt; idx < N; idx++) {
                int pile = idx2pile[idx];
                // if (idx != idx_nxt && idx_min_on_top_of_idx_nxt == -1 && pile == idx_nxt_pile && idx2height[idx] > idx_nxt_height)
                //     idx_min_on_top_of_idx_nxt = idx;
                if (seen[pile])
                    continue;
                seen[pile] = true;
                cnt++;
                if (cnt == M - 1) {
                    // seen almost all piles
                    break;
                }
            }
            // seen[pile] == false
            pile = int(min_element(seen, seen + M) - seen);
        }

        int idx_min_on_top_of_idx_nxt = N;
        for (int i = idx_nxt_height + 1; i < pile_sz[idx_nxt_pile]; i++) {
            chmin(idx_min_on_top_of_idx_nxt, board[idx_nxt_pile][i]);
        }

        if (idx_min_on_top_of_idx_nxt != N) {
            // NEW
            int idx_min_in_pile_after = N;
            if (pile_sz[pile] != 0)
                idx_min_in_pile_after = *min_element(board[pile], board[pile] + pile_sz[pile]);
            if (idx_min_in_pile_after > idx_min_on_top_of_idx_nxt
                && idx2height[idx_min_on_top_of_idx_nxt] + 1 != pile_sz[idx_nxt_pile]) {
                idxmove = board[idx_nxt_pile][idx2height[idx_min_on_top_of_idx_nxt] + 1];
                // cerr << idx_min_in_pile_after << " " << idx_min_on_top_of_idx_nxt << " " << pile << " " << idx_nxt_pile << endl;
            }
        }
        return {idxmove, pile};
    }


    int update(const pi& p) {
        const auto [idx, pile_nxt] = p;
        assert(idx < N);
        if (pile_nxt == -1) {
            // move
            int pile = idx2pile[idx];
            assert(idx_nxt == idx);
            assert(idx2height[idx] + 1 == pile_sz[idx2pile[idx]]);
            idx2height[idx] = -1;
            board[pile][pile_sz[pile] - 1] = -1;
            pile_sz[pile]--;
            idx2pile[idx] = -1;
            idx2height[idx] = -1;
            idx_nxt++;
            return 0;
        } else {
            // move
            int height_original = idx2height[idx];
            int pile_original = idx2pile[idx];
            assert(pile_nxt != pile_original);
            int n_box_move = pile_sz[pile_original] - height_original;
            int cost = n_box_move + 1;
            // update idx2pile, idx2height
            for (int i = height_original; i < pile_sz[pile_original]; i++) {
                idx2pile[board[pile_original][i]] = pile_nxt;
                idx2height[board[pile_original][i]] = pile_sz[pile_nxt] + i - height_original;
            }
            // update board
            copy(board[pile_original] + height_original, board[pile_original] + pile_sz[pile_original], board[pile_nxt] + pile_sz[pile_nxt]);
            // update pile_sz
            pile_sz[pile_nxt] += n_box_move;
            pile_sz[pile_original] -= n_box_move;
            // update score
            score += cost;
            return cost;
        }
    }
    int update_rev(const pair<int, int>& op_rev) {
        // reverse of update;
        const auto [idx, pile_original] = op_rev;
        assert((idx == idx_nxt - 1) || (idx == idx_nxt));
        if (idx == idx_nxt - 1) {
            // add idx to top of pile
            // board
            board[pile_original][pile_sz[pile_original]] = idx;
            // pile_sz
            pile_sz[pile_original]++;
            // idx2pile, idx2height
            idx2pile[idx] = pile_original;
            idx2height[idx] = pile_sz[pile_original];
            // idx_nxt
            idx_nxt--;
            return 0;
        } else {
            pair<int, int> op = make_pair(idx, pile_original);
            int dscore = -update(op);
            score += dscore;
            return dscore;
        }
    }

    bool check_end() {
        return idx_nxt == N;
    }

    int playout_greedy(int idx_max = N, int use_step = 1) {
        Solver solver = *this;  // copy
        int s = solver.solve_greedy(false, idx_max, use_step);
        return s;
    }
    int playout_greedy(const pi& op, int idx_max = N, int use_step = 1) {
        Solver solver = *this;  // copy
        solver.update(op);      // update
        int s = solver.solve_greedy(false, idx_max, use_step);
        return s;
    }


    int solve_greedy(bool remember_opeartions = false, int idx_max = N, int use_step = 1) {
        if (remember_opeartions)
            operations.reserve(N * 2);
        chmin(idx_max, N);
        while (idx_nxt != idx_max) {
            pair<int, int> p;

            if (use_step == 1)
                p = step();
            else if (use_step == 3)
                p = step_greedy_3();
            else if (use_step == 4)
                p = step_greedy_4();
            else if (use_step == 5)
                p = step_greedy_5();
            else
                assert(false);
            if (remember_opeartions)
                operations.emplace_back(p);
            update(p);
        }
        return score;
    }

    // templated playout
    // template <typename F>
    // int playout_greedy(F step_func, int idx_max = N) {
    //     Solver solver = *this;  // copy
    //     int s = solver.solve_greedy(step_func, false, idx_max);
    //     return s;
    // }
    template <typename F>
    int playout_greedy(const pi& op, F step_func, int idx_max = N) {
        Solver solver = *this;  // copy
        solver.update(op);      // update
        int s = solver.solve_greedy(step_func, false, idx_max);
        return s;
    }

    // // solve greedy with templated function (step or step_greedy_3)
    // template <typename F>
    // int solve_greedy(F step_func, bool remember_opeartions = false, int idx_max = N) {
    //     if (remember_opeartions)
    //         operations.reserve(N * 2);
    //     chmin(idx_max, N);
    //     while (idx_nxt != idx_max) {
    //         auto p = step_func(*this);
    //         if (remember_opeartions)
    //             operations.emplace_back(p);
    //         update(p);
    //     }
    //     return score;
    // }


    // try several greedy
    int solve_0514() {
        // playout with greedy
        operations.reserve(N * 2);
        while (idx_nxt != N) {
            // auto op_greedy = step();
            auto op_greedy = step_greedy_3();
            if (op_greedy.second == -1) {
                // carry out
                operations.emplace_back(op_greedy);
                update(op_greedy);
                continue;
            }

            vector<int> V_use_step{5};

            int idx_max = N;
            // int idx_max = idx_nxt + 70;

            // move
            auto op_best = op_greedy;
            int score_best = playout_greedy(op_best);
            if (true)
                for (int use_step : V_use_step) {
                    pair<int, int> op;
                    if (use_step == 5) {
                        op = step_greedy_5();
                    } else
                        assert(false);
                    int score = playout_greedy(op, idx_max, use_step);
                    if (chmin(score_best, score)) {
                        op_best = op;
                    }
                }

            // playout all
            int pile_nxt = idx2pile[idx_nxt];
            int height_nxt = idx2height[idx_nxt];
            for (int height = height_nxt + 1; height < pile_sz[pile_nxt]; height++) {
                int idxmove = board[pile_nxt][height];
                for (int pile = 0; pile < M; pile++) {
                    if (pile == pile_nxt)
                        continue;
                    auto op = make_pair(idxmove, pile);

                    for (int use_step : V_use_step) {
                        int score = playout_greedy(op, idx_max, use_step);
                        if (chmin(score_best, score)) {
                            op_best = op;
                        }
                    }
                }
            }
            // pile_nxt 以外の移動
            // if (get_time_remaining() >= 100) {
            //     int N_try = 1000;
            //     while (N_try--) {
            //         int pile = randrange(M - 1);
            //         if (pile == pile_nxt)
            //             pile++;
            //         if (pile_sz[pile] == 0)
            //             continue;
            //         int height = randrange(pile_sz[pile]);
            //         int idxmove = board[pile][height];
            //         int pile_nxt_move = randrange(M - 2);
            //         while (pile_nxt_move == pile || pile_nxt_move == pile_nxt)
            //             pile_nxt_move++;
            //         auto op = make_pair(idxmove, pile_nxt_move);
            //         // cerr << N_try << " " << pile << " " << pile_sz[pile] << " " << height << endl;
            //         int score = playout_greedy(op);
            //         // cerr << score << endl;
            //         // cerr << idxmove << " " << height << endl;
            //         if (chmin(score_best, score)) {
            //             op_best = op;
            //         }
            //     }
            // }

            // pile_nxt 以外の全ての移動
            if (get_time_remaining() >= 50) {
                for (int idxmove = idx_nxt + 1; idxmove < N; idxmove++) {
                    int pile_original = idx2pile[idxmove];
                    bool flag_move_to_0 = false;
                    if (pile_original == pile_nxt)
                        continue;
                    for (int pile_nxt_move = 0; pile_nxt_move < M; pile_nxt_move++) {
                        if (pile_nxt_move == pile_original)
                            continue;
                        if (pile_sz[pile_nxt_move] == 0) {
                            if (flag_move_to_0)
                                continue;
                            flag_move_to_0 = true;
                        }
                        auto op = make_pair(idxmove, pile_nxt_move);
                        for (int use_step : V_use_step) {
                            int score = playout_greedy(op, idx_max, use_step);
                            if (chmin(score_best, score)) {
                                op_best = op;
                            }
                        }
                    }
                }
            }

            operations.emplace_back(op_best);
            update(op_best);
        }
        return score;
    }

    int solve_0353() {
        // playout with greedy
        operations.reserve(N * 2);
        while (idx_nxt != N) {
            auto op_greedy = step();
            if (op_greedy.second == -1) {
                // carry out
                operations.emplace_back(op_greedy);
                update(op_greedy);
                continue;
            }

            // int idx_max = N;
            int idx_max = idx_nxt + 70;

            // move
            auto op_best = op_greedy;
            int score_best = playout_greedy(op_best);

            // playout all
            int pile_nxt = idx2pile[idx_nxt];
            int height_nxt = idx2height[idx_nxt];
            for (int height = height_nxt + 1; height < pile_sz[pile_nxt]; height++) {
                int idxmove = board[pile_nxt][height];
                for (int pile = 0; pile < M; pile++) {
                    if (pile == pile_nxt)
                        continue;
                    auto op = make_pair(idxmove, pile);
                    int score = playout_greedy(op, idx_max);
                    if (chmin(score_best, score)) {
                        op_best = op;
                    }
                }
            }
            // pile_nxt 以外の移動
            // if (get_time_remaining() >= 100) {
            //     int N_try = 1000;
            //     while (N_try--) {
            //         int pile = randrange(M - 1);
            //         if (pile == pile_nxt)
            //             pile++;
            //         if (pile_sz[pile] == 0)
            //             continue;
            //         int height = randrange(pile_sz[pile]);
            //         int idxmove = board[pile][height];
            //         int pile_nxt_move = randrange(M - 2);
            //         while (pile_nxt_move == pile || pile_nxt_move == pile_nxt)
            //             pile_nxt_move++;
            //         auto op = make_pair(idxmove, pile_nxt_move);
            //         // cerr << N_try << " " << pile << " " << pile_sz[pile] << " " << height << endl;
            //         int score = playout_greedy(op);
            //         // cerr << score << endl;
            //         // cerr << idxmove << " " << height << endl;
            //         if (chmin(score_best, score)) {
            //             op_best = op;
            //         }
            //     }
            // }

            // pile_nxt 以外の全ての移動
            if (get_time_remaining() >= 100) {
                for (int idxmove = idx_nxt + 1; idxmove < N; idxmove++) {
                    int pile_original = idx2pile[idxmove];
                    bool flag_move_to_0 = false;
                    if (pile_original == pile_nxt)
                        continue;
                    for (int pile_nxt_move = 0; pile_nxt_move < M; pile_nxt_move++) {
                        if (pile_nxt_move == pile_original)
                            continue;
                        if (pile_sz[pile_nxt_move] == 0) {
                            if (flag_move_to_0)
                                continue;
                            flag_move_to_0 = true;
                        }
                        auto op = make_pair(idxmove, pile_nxt_move);
                        int score = playout_greedy(op, idx_max);
                        if (chmin(score_best, score)) {
                            op_best = op;
                        }
                    }
                }
            }

            operations.emplace_back(op_best);
            update(op_best);
        }
        return score;
    }

    void print_ans() {
        cerr << "[DATA] score_raw = " << score << endl;
        cerr << "[DATA] score = " << 10000 - score << endl;
        for (auto [idx, pile] : operations) {
            cout << idx + 1 << " " << pile + 1 << endl;
        }
    }
};

// ---------------------------------------- main ----------------------------------------

void read_input() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N / M; j++) {
            cin >> B[i][j];
            B[i][j]--;
        }
    }
}

void solve1() {
    cerr << "start solve1" << endl;
    Solver solver;
    cerr << "greedy score = " << solver.playout_greedy() << endl;
    // solver.solve_greedy(true);
    // solver.solve_greedy([&] { return solver.step_greedy_3(); }, true); // does not work anymore
    // solver.solve_0353();
    // solver.simulated_annealing();
    // solver.solve_0514(
    //     [&](Solver& solver) {
    //         vector<pair<int, int>> V{
    //             solver.step(),
    //             solver.step_greedy_3(),
    //         };
    //         return V;
    //     });
    solver.solve_0514();
    // solver.solve_greedy(true, N, 5);
    solver.print_ans();    
    cerr << "end solve1" << endl;
}

int main() {
    read_input();
    // print_time();
    solve1();

    print_time();
}

