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

// #define FLAG_PRINT_ANS_MIDDLE
// #define FLAG_PRINT_ACCEPTANCE_RATIO

// ---------------------------------------- template ----------------------------------------

// #define _GLIBCXX_DEBUG
// #include <bits/stdc++.h>
#include <algorithm>
#include <assert.h>
#include <bitset>
#include <chrono>
#include <complex>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include <unistd.h>
#include <x86intrin.h>

// #pragma GCC diagnostic ignored "-Wconversion"
// #include "Eigen/Dense"
// #pragma GCC diagnostic pop
// using Eigen::MatrixXd;
// using Eigen::VectorXd;

using namespace std;
// using std::cin;
// using std::cout;
using ll = long long;
using ull = unsigned long long;

#define bit(n, k) (((n) >> (k)) & 1) /*nのk bit目*/

#ifdef ONLINE_JUDGE
const double TIME_RATIO = 1.0;
#else
// const double TIME_RATIO = 0.01;
// const double TIME_RATIO = 0.1;
// const double TIME_RATIO = 0.5;
// const double TIME_RATIO = 1.3;
const double TIME_RATIO = 1.5;
// const double TIME_RATIO = 5.0;
#endif

constexpr int INF = 1000000000;
constexpr ll INFLL = 1ll << 40;
constexpr int TIME_LIMIT = 1950;
// constexpr int TIME_LIMIT = 1990;
// constexpr int TIME_LIMIT = 5990;

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
int get_time_from(int t) {
    int elapsed = get_time() - t;
    return elapsed;
}
int get_time_remaining() {
    int time_remaining = TIME_LIMIT - get_time();
    return time_remaining;
}
int get_time_remaining_until(int t) {
    int time_remaining = t - get_time();
    return time_remaining;
}
void print_time() {
    int elapsed = get_time();
    cerr << "[DATA] totaltime = " << elapsed << endl;
}

class HRandom {
public:
    HRandom() {
        random_mw = 1985;
        random_mz = 2012;
    }
    int intRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result >> 1;
    }
    unsigned int uintRandom_65535() {
        // random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        // random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        // return random_mw & 65535;
        const unsigned tmp = random_mw & 65535;
        random_mw = 18000 * tmp + (random_mw >> 16);
        return tmp;
    }
    unsigned int uintRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return result;
    }
    double dblRandom() {
        random_mz = 36969 * (random_mz & 65535) + (random_mz >> 16);
        random_mw = 18000 * (random_mw & 65535) + (random_mw >> 16);
        unsigned result = (random_mz << 16) + random_mw;
        return (result + 1.0) * 2.328306435454494e-10;
    }

private:
    unsigned random_mw;
    unsigned random_mz;
};

HRandom hrandom;
// uint32_t randxor() {
//     return hrandom.uintRandom();
// }
// void randxor_init() {}


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
void randxor_init() {}


double randdouble() {
    // return randxor() / double(UINT32_MAX);
    return hrandom.dblRandom();
}
bool randbool(const float& p = 0.5) {
    // return hrandom.uintRandom() < (p * UINT32_MAX);
    return hrandom.uintRandom_65535() < (p * 65535);
}

// TODO
template <float p>
bool randbool_const() {
    // return randxor() < (p * UINT32_MAX);
    // return hrandom.uintRandom() < (p * UINT32_MAX);
    return hrandom.uintRandom_65535() < (p * 65535);
}


// static uint32_t randxor2() {
//     static uint32_t x = 123456789;
//     static uint32_t y = 362436069;
//     static uint32_t z = 521288629;
//     static uint32_t w = 88675123;
//     uint32_t t;
//     t = x ^ (x << 11);
//     x = y;
//     y = z;
//     z = w;
//     return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
// }
// static uint32_t randxor() {
//     static uint32_t x = 123456789;
//     static uint32_t y = 362436069;
//     static uint32_t z = 521288629;
//     static uint32_t w = 88675123;
//     uint32_t t;
//     t = x ^ (x << 11);
//     y += randxor2();
//     x = y;
//     y = z;
//     z = w;
//     return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
// }


// const int RANDXOR_TABLE_SIZE = (1 << 20);
// uint32_t randxor_table[RANDXOR_TABLE_SIZE];
// static uint32_t randxor_gen() {
//     static uint32_t x = 123456789;
//     static uint32_t y = 362436069;
//     static uint32_t z = 521288629;
//     static uint32_t w = 88675123;
//     uint32_t t;
//     t = x ^ (x << 11);
//     x = y;
//     y = z;
//     z = w;
//     return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
// }
// void randxor_init() {
//     for (int i = 0; i < RANDXOR_TABLE_SIZE; i++) {
//         randxor_table[i] = randxor_gen();
//     }
// }
// uint32_t randxor() {
//     static int idx = 0;
//     if (idx == RANDXOR_TABLE_SIZE) {
//         idx = 0;
//     }
//     return randxor_table[idx++];
// }

// template <int n>
// inline int randrange_small() {
//     return randxor() % n;
//     // return (randxor() % (n * 100)) % n;
// }
// // array of function pointers
// int (*randrange_small_table[10])() = {randrange_small<0>, randrange_small<1>, randrange_small<2>, randrange_small<3>, randrange_small<4>, randrange_small<5>, randrange_small<6>, randrange_small<7>, randrange_small<8>, randrange_small<9>};


inline int randrange(const int n) {
    // int __attribute__((noinline)) randrange(int n) {
    assert(0 <= n);

    if (n == 2) {
        return randbool_const<0.5f>();
    }
    // if (n == 3) {
    //     if (randbool_const<1. / 3>()) {
    //         return 0;
    //     } else if (randbool_const<1. / 2>()) {
    //         return 1;
    //     } else
    //         return 2;
    // }
    // if (n == 4) {
    //     if (randbool_const<1. / 2>()) {
    //         if (randbool_const<1. / 2>()) {
    //             return 0;
    //         } else {
    //             return 1;
    //         }
    //     } else {
    //         if (randbool_const<1. / 2>()) {
    //             return 2;
    //         } else {
    //             return 3;
    //         }
    //     }
    // }
    if (n == 1) {
        return 0;
    }
    // if (n == 9) {
    //     if (randbool_const<1. / 9>()) {
    //         return 8;
    //     } else {
    //         return (uint64_t(hrandom.uintRandom()) * 8) >> 32;
    //         // return hrandom.uintRandom() & 7;
    //         // return hrandom.intRandom() & 7;
    //     }
    // }

    // cerr << n << endl;

    // if (n < 10) {
    //     return randrange_small_table[n]();
    // }

    // if (n == 3) {
    //     int ret = randxor() & 3;
    //     if (ret < 3)
    //         return ret;
    //     static int idx = 0;
    //     idx++;
    //     if (idx == 3) {
    //         idx = 0;
    //     }
    //     return idx;
    // }
    // return (uint64_t(randxor()) * n) >> 32;
    // return (uint64_t(hrandom.uintRandom()) * n) >> 32;
    return ((hrandom.uintRandom_65535()) * n) >> 16;

    // return randxor() % n;
    // return hrandom.intRandom() % n;
    // return ((randxor() % n) + randxor()) % n;
}

int randrange(const int& l, const int& r) {
    assert(l <= r);
    return randrange(r - l) + l;
}
// double randdouble() {
//     // return (double)randxor() / UINT32_MAX;
//     uint32_t x = randxor();
//     x >>= 9;
//     x |= 0x3F800000;
//     double y = bit_cast<float, uint32_t>(x);
//     return y - 1;
// }
// double randdouble() {
//     static double x = randxor() / UINT32_MAX;
//     const double dx = (sqrt(2) - 1) / 2;
//     x += dx;
//     if (x > 1)
//         x -= 1;
//     return x;
// }

template <typename T>
void print_data(const string& s, const T& t) {
    cerr << "[DATA] " << s << " = " << t << endl;
}
template <typename T>
void print_debug(const string& s, const T& t) {
#ifndef NDEBUG
    cerr << s << " = " << t << endl;
#endif
}

int rand_select_normalized(const vector<double>& prob) {
    assert(prob.size() >= 1);
    double p = randdouble();
    double p_acc = 0;
    for (int i = 0; i < (int)prob.size(); i++) {
        p_acc += prob[i];
        if (p < p_acc)
            return i;
    }
    return (int)prob.size() - 1;
}
int rand_select(vector<double> prob) {
    double prob_sum = accumulate(prob.begin(), prob.end(), 0.0);
    for (auto& p : prob) {
        p /= prob_sum;
    }
    return rand_select_normalized(prob);
}

const int LOG_TABLE_SIZE = 1 << 14;
double log_table[LOG_TABLE_SIZE];
void log_init() {
    for (int i = 0; i < LOG_TABLE_SIZE; i++) {
        log_table[i] = log(randdouble());
    }
}
inline double log_rand() {
    static int idx = 0;
    if (idx == LOG_TABLE_SIZE) {
        idx = 0;
    }
    return log_table[idx++];
}

// ---------------------------------------- variables ----------------------------------------

using Tcost = double;
// const Tcost WALL_COST = 7e9;
// const Tcost NOT_VISITED_COST = 1e25;
// const Tcost WALL_COST_BEGIN = 1e8;
// const Tcost WALL_COST_END = 1e10;
const Tcost WALL_COST_BEGIN = 1e20;
const Tcost WALL_COST_END = 1e20;
// const Tcost NOT_VISITED_COST_BEGIN = 1e3;
// const Tcost NOT_VISITED_COST_END = 1e6;
// const Tcost NOT_VISITED_COST_BEGIN = 1e10;
// const Tcost NOT_VISITED_COST_END = 1e10;
const Tcost NOT_VISITED_COST_BEGIN = 1e20;
const Tcost NOT_VISITED_COST_END = 1e20;
// const Tcost WALL_COST = WALL_COST_BEGIN;
// const Tcost NOT_VISITED_COST = NOT_VISITED_COST_BEGIN;
Tcost WALL_COST = WALL_COST_BEGIN;
Tcost NOT_VISITED_COST = NOT_VISITED_COST_BEGIN;

void update_cost_coef(double time_ratio) {
    WALL_COST = WALL_COST_BEGIN + (WALL_COST_END - WALL_COST_BEGIN) * time_ratio;
    NOT_VISITED_COST = NOT_VISITED_COST_BEGIN + (NOT_VISITED_COST_END - NOT_VISITED_COST_BEGIN) * time_ratio;
}

int N;
int NN;
// constexpr int N_MAX = 40;
constexpr int N_MAX = 40;
constexpr int NN_MAX = N_MAX * N_MAX;
bool H[N_MAX - 1][N_MAX];
bool V[N_MAX][N_MAX - 1];
int D[N_MAX][N_MAX];
int D_flatten[N_MAX * N_MAX];
// bool can_move_memo[N_MAX][N_MAX][4];
bool can_move_inside_memo[N_MAX][N_MAX][4];
bool can_move_withouthittingwall_memo[N_MAX][N_MAX][4];
int move_cost_memo[N_MAX][N_MAX][4];
bool can_move_inside_memo_flatten[N_MAX * N_MAX][4];
bool can_move_withouthittingwall_memo_flatten[N_MAX * N_MAX][4];
// bool can_move_inside_memo_point_flatten[N_MAX * N_MAX][N_MAX * N_MAX];
// bool can_move_withouthittingwall_memo_point_flatten[N_MAX * N_MAX][N_MAX * N_MAX];
int move_cost_memo_flatten[N_MAX * N_MAX][4];
int move_cost_memo_flatten2[NN_MAX][NN_MAX];

// vector<vector<vector<vector<int>>>> dist;
// vector<vector<int>> dist_flatten;
int dist[N_MAX][N_MAX][N_MAX][N_MAX];
int dist_flatten[NN_MAX][NN_MAX];
int dist_l1_flatten[NN_MAX][NN_MAX];

const int dx[4] = {0, 1, 0, -1};  // RDLU
const int dy[4] = {1, 0, -1, 0};  // RDLU

using Border = tuple<int, int, int, int>;  // x_min, x_max, y_min, y_max

const string DIR = "RDLU";
const int MAX_LEN = 10000;
const int MAX_LEN_PRACTICLE = 9000;
int idx2x[N_MAX * N_MAX];
int idx2y[N_MAX * N_MAX];
int xy2idx[N_MAX][N_MAX];

// ---------------------------------------- solver ----------------------------------------

constexpr int MYDEQUE_MAXSIZE = MAX_LEN + 100;
int increment_memo[MYDEQUE_MAXSIZE];
int decrement_memo[MYDEQUE_MAXSIZE];

// #define FLAG_CHECK_MYDEQUE
// #define FLAG_CHECK_MYDEQUE_PRINT
// #define FLAG_USE_DEQUE
template <typename T>
class MyDeque {
public:
    int max_size;
    int idx_front;
    int idx_end;
    vector<T> data;
    deque<T> que;
    // MyDeque() = default;

    MyDeque(int max_size = MYDEQUE_MAXSIZE) : max_size(max_size), idx_front(0), idx_end(0), data(max_size) {}
    MyDeque(const vector<T>& data, int max_size = MYDEQUE_MAXSIZE) : max_size(max_size), idx_front(0), idx_end(0), data(max_size) {
        assert((int)data.size() <= max_size);
        for (int i = 0; i < (int)data.size(); i++) {
            emplace_back(data[i]);
        }
        // for (int i = 0; i < (int)data.size(); i++) {
        //     que.push_back(data[i]);
        // }
    }
    void shift_forward() {
#ifdef FLAG_USE_DEQUE
        assert(size() >= 1);
        T x = que[0];
        que.pop_front();
        que.push_back(x);
        return;
#endif
        // swap(data[idx_front], data[idx_end]);
        data[idx_end] = data[idx_front];
        increment_inline(idx_front);
        increment_inline(idx_end);
    }
    void clear() {
        idx_end = idx_front;
    }
    int increment(const int& idx) {
        return increment_memo[idx];
        // return (idx == max_size - 1) ? 0 : idx + 1;
    }
    int decrement(const int& idx) {
        return decrement_memo[idx];
        // return (idx == 0) ? max_size - 1 : idx - 1;
    }
    void increment_inline(int& idx) {
        idx = increment_memo[idx];
        // idx = (idx == max_size - 1) ? 0 : idx + 1;
    }
    void decrement_inline(int& idx) {
        idx = decrement_memo[idx];
        // idx = (idx == 0) ? max_size - 1 : idx - 1;
    }
    void emplace_front(const T& x) {
#ifdef FLAG_USE_DEQUE
        que.push_front(x);
        return;
#endif
#ifdef FLAG_CHECK_MYDEQUE
        // cerr << "emplace_front" << endl;
        que.push_front(x);
#endif
        // return;
        decrement_inline(idx_front);
        data[idx_front] = x;
        assert(idx_front != idx_end);
#ifdef FLAG_CHECK_MYDEQUE
        if (idx_front == idx_end) {
            cerr << "idx_front = " << idx_front << endl;
            cerr << "idx_end = " << idx_end << endl;
            exit(0);
            increment_inline(idx_end);
        }
        if (front() != que.front()) {
            cerr << "failed in emplace_front" << endl;
            cerr << size() << " " << que.size() << endl;
            exit(0);
        }
        if (que.size() != size()) {
            cerr << "failed in emplace_front" << endl;
            cerr << size() << " " << que.size() << endl;
            exit(0);
        }
#endif
    }
    void emplace_back(const T& x) {
#ifdef FLAG_USE_DEQUE
        que.push_back(x);
        return;
#endif
#ifdef FLAG_CHECK_MYDEQUE
        // cerr << "emplace_back" << endl;
        que.emplace_back(x);
#endif
        // return;
        data[idx_end] = x;
        increment_inline(idx_end);
        assert(idx_front != idx_end);
#ifdef FLAG_CHECK_MYDEQUE
        if (idx_front == idx_end) {
            cerr << "idx_front = " << idx_front << endl;
            cerr << "idx_end = " << idx_end << endl;
            exit(0);
            decrement_inline(idx_front);
        }
        if (back() != que.back()) {
            cerr << "failed in emplace_back" << endl;
            cerr << size() << " " << que.size() << endl;
            exit(0);
        }
        if (que.size() != size()) {
            cerr << "failed in emplace_back" << endl;
            cerr << size() << " " << que.size() << endl;
            exit(0);
        }
#endif
    }
    int size() {
#ifdef FLAG_USE_DEQUE
        return que.size();
#endif
        // return que.size();
        int sz = (idx_end >= idx_front ? idx_end - idx_front : idx_end + max_size - idx_front);
#ifdef FLAG_CHECK_MYDEQUE
        // cerr << sz << " " << que.size() << endl;
        if (sz != (int)que.size()) {
            cerr << "failed in size" << endl;
            cerr << sz << " " << que.size() << endl;
            // exit(0);
        }
#endif
        // cerr << sz << " " << que.size() << endl;
        // if (sz != (int)que.size()) {
        //     exit(0);
        // }
        // assert(sz == (int)que.size());
        return sz;
    }
    T& operator[](int idx) {
#ifdef FLAG_USE_DEQUE
        return que[idx];
#endif
        assert(0 <= idx && idx < size());
        int idx_data = (idx_front + idx < max_size ? idx_front + idx : idx_front + idx - max_size);
#ifdef FLAG_CHECK_MYDEQUE
        if (data[idx_data] != que[idx]) {
            cerr << data[idx_data] << " " << que[idx] << endl;
            exit(0);
        }
#endif
        // return que[idx];
        // assert(data[idx_data] == que[idx]);
        return data[idx_data];
    }
    T& front() {
#ifdef FLAG_USE_DEQUE
        return que.front();
#endif
// return que.front();
#ifdef FLAG_CHECK_MYDEQUE
        if (size() == 0) {
            cerr << "size = " << size() << endl;
            exit(0);
        }
#endif
        return data[idx_front];
    }
    T& back() {
#ifdef FLAG_USE_DEQUE
        return que.back();
#endif
// return que.back();
#ifdef FLAG_CHECK_MYDEQUE
        if (size() == 0) {
            cerr << "size = " << size() << endl;
            exit(0);
        }
#endif
        return data[decrement(idx_end)];
    }
    void pop_front() {
#ifdef FLAG_USE_DEQUE
        que.pop_front();
        return;
#endif
        // assert(T(que.front()) == data[idx_front]);
        // return;
        increment_inline(idx_front);
#ifdef FLAG_CHECK_MYDEQUE
        que.pop_front();
        // cerr << "pop_front" << endl;
        if (size() == 0) {
            cerr << "size = " << size() << endl;
            exit(0);
        }
        if (front() != que.front()) {
            cerr << "failed in pop_front" << endl;
            cerr << front() << " " << que.front() << endl;
            exit(0);
        }
        if (que.size() != size()) {
            cerr << "failed in pop_front" << endl;
            cerr << size() << " " << que.size() << endl;
            exit(0);
        }
#endif
    }
    void pop_back() {
#ifdef FLAG_USE_DEQUE
        que.pop_back();
        return;
#endif
        // assert(T(que.back()) == data[decrement(idx_end)]);
        // return;
        decrement_inline(idx_end);
#ifdef FLAG_CHECK_MYDEQUE
        // cerr << "pop_back" << endl;
        que.pop_back();
        if (size() == 0) {
            cerr << "size = " << size() << endl;
            exit(0);
        }
        if (back() != que.back()) {
            cerr << "failed in pop_back" << endl;
            cerr << back() << " " << que.back() << endl;
            exit(0);
        }
        if (que.size() != size()) {
            cerr << "failed in pop_back" << endl;
            cerr << size() << " " << que.size() << endl;
            exit(0);
        }
#endif
    }
    bool empty() {
#ifdef FLAG_USE_DEQUE
        return que.empty();
#endif
// return que.empty();
#ifdef FLAG_CHECK_MYDEQUE
        if (size() == 0) {
            cerr << "size = " << size() << endl;
            exit(0);
        }
#endif
        return idx_front == idx_end;
    }
    vector<T> get_data() {
#ifdef FLAG_USE_DEQUE
        return vector<T>(que.begin(), que.end());
#endif

        // return vector(que.begin(), que.end());
        int sz = size();
        // cerr << "size = " << sz << endl;
        vector<T> res(sz);
        // static int cnt = 0;
        // cerr << cnt++ << endl;
        for (int i = 0; i < sz; i++) {
            res[i] = (*this)[i];
#ifdef FLAG_CHECK_MYDEQUE
            if (res[i] != que[i]) {
                cerr << "failed in get_data" << endl;
                cerr << res[i] << " " << que[i] << endl;
                exit(0);
            }
            // assert(res[i] == que[i]);
#endif
        }
        return res;
    }
};


// using Lint = list<int>;
// using VLint = vector<Lint>;
// using VVLint = vector<VLint>;

using Dint = MyDeque<int>;
// using Dint = deque<int>;
using VDint = vector<Dint>;
using VVDint = vector<VDint>;


// int n_copy_sum = 0;
template <typename T>
void copy_vector(const vector<T>& src, vector<T>& dst, int& n) {
    n = (int)src.size();
    // n_copy_sum += n;
    assert(n <= (int)dst.size());
    // cerr << sizeof(T) << " " << sizeof(int) << " " << sizeof(ll) << " " << sizeof(short) << endl;
    memcpy(dst.data(), src.data(), sizeof(T) * n);
    // copy(src.begin(), src.end(), dst.begin());
    // for (int i = 0; i < n; i++) {
    //     dst[i] = src[i];
    // }
}

class SimulatedAnnealingConfig {
public:
    int max_iter;
    int max_time;
    int max_n_double;
    double temp_begin;
    double temp_end;
    bool flag_true_prob;
    SimulatedAnnealingConfig() {}
    SimulatedAnnealingConfig(
        int max_iter,
        int max_time,
        int max_n_double,
        double temp_begin,
        double temp_end,
        bool flag_true_prob) : max_iter(max_iter),
                               max_time(max_time),
                               max_n_double(max_n_double),
                               temp_begin(temp_begin),
                               temp_end(temp_end),
                               flag_true_prob(flag_true_prob) {}
};

class UnionFind {
public:
    vector<int> data;
    UnionFind(int size) : data(size, -1) {}
    bool unite(int x, int y) {
        x = root(x);
        y = root(y);
        if (x != y) {
            if (data[y] < data[x])
                swap(x, y);
            data[x] += data[y];
            data[y] = x;
        }
        return x != y;
    }
    bool same(int x, int y) { return root(x) == root(y); }
    int root(int x) { return data[x] < 0 ? x : data[x] = root(data[x]); }
    int size(int x) { return -data[root(x)]; }
};

ll calc_sum_tri_naive(const int& x) {
    assert(0 <= x);
    if (x < -1) {
        cerr << "x = " << x << endl;
    }
    assert(-1 <= x);
    return (ll)x * (x + 1) / 2;
}
ll calc_sum_tri_memo[10010];
ll calc_sum_tri(const int& x) {
    assert(0 <= x);
    return calc_sum_tri_memo[x];
}


namespace PointOriginal {
class Point {
private:
    int _x, _y;

public:
    Point() : _x(0), _y(0) {}
    Point(int x, int y) : _x(x), _y(y) {}
    bool can_move_inside_naive(int dir) {
        int nx = _x + dx[dir];
        int ny = _y + dy[dir];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N)
            return false;
        return true;
    }
    bool can_move_withouthittingwall_naive(int dir) {
        int nx = _x + dx[dir];
        int ny = _y + dy[dir];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N)
            return false;
        if (dir == 0) {
            return !V[_x][_y];
        } else if (dir == 1) {
            return !H[_x][_y];
        } else if (dir == 2) {
            return !V[nx][ny];
        } else if (dir == 3) {
            return !H[nx][ny];
        }
        assert(false);
    }
    bool can_move_inside(int dir) {
        return can_move_inside_memo[_x][_y][dir];
    }
    bool can_move_withouthittingwall(int dir) {
        return can_move_withouthittingwall_memo[_x][_y][dir];
    }
    int cost_move_without_hitting_wall(int dir) {
        return move_cost_memo[_x][_y][dir];
    }
    bool can_move_inside(Point p) {
        assert((p - *this).norm() == 1);
        int dir = (*this).get_dir(p);
        return can_move_inside(dir);
    }
    bool can_move_withouthittingwall(Point p) {
        assert((p - *this).norm() == 1);
        int dir = (*this).get_dir(p);
        return can_move_withouthittingwall(dir);
    }
    int cost_move_without_hitting_wall(Point p) {
        assert((p - *this).norm() == 1);
        int dir = (*this).get_dir(p);
        return cost_move_without_hitting_wall(dir);
    }
    int raw() const { return _x * N + _y; }
    int x() const { return _x; }
    int y() const { return _y; }
    Point move(int dir) {
        int nx = _x + dx[dir];
        int ny = _y + dy[dir];
        return Point(nx, ny);
    }
    void move_inplace(int dir) {
        _x += dx[dir];
        _y += dy[dir];
    }
    void operator+=(const Point& other) {
        _x += other._x;
        _y += other._y;
    }
    void operator-=(const Point& other) {
        _x -= other._x;
        _y -= other._y;
    }
    Point operator+(const Point& other) const {
        return Point(_x + other._x, _y + other._y);
    }
    Point operator-(const Point& other) const {
        return Point(_x - other._x, _y - other._y);
    }
    int norm() const {
        return abs(_x) + abs(_y);
    }
    bool operator==(const Point& other) const {
        return _x == other._x && _y == other._y;
    }
    int get_dir(const Point& p) const;
    // bool operator!=(const Point& other) const {
    //     return _x != other._x || _y != other._y;
    // }
    // bool operator<(const Point& other) const {
    //     return _x < other._x || (_x == other._x && _y < other._y);
    // }

    // print
    friend ostream& operator<<(ostream& os, const Point& p) {
        os << "(" << p._x << ", " << p._y << ")";
        return os;
    }
};
const Point dpoint[4] = {Point(0, 1), Point(1, 0), Point(0, -1), Point(-1, 0)};
}  // namespace PointOriginal


int xy2idx_func(int x, int y) {
    return x * N + y;
}

using PointInternal = short;
PointInternal dpoint_raw[4] = {};
class Point {
private:
    // int _x, _y;
    PointInternal idx;

public:
    Point() : idx(0) {}
    Point(const int& x, const int& y) : idx(xy2idx[x][y]) {}
    Point(const PointInternal& idx) : idx(idx) {}
    // Point(const int& idx) : idx(idx) {}
    bool can_move_inside_naive(int dir) {
        int _x = idx2x[idx];
        int _y = idx2y[idx];
        int nx = _x + dx[dir];
        int ny = _y + dy[dir];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N)
            return false;
        return true;
    }
    bool can_move_withouthittingwall_naive(int dir) {
        int _x = idx2x[idx];
        int _y = idx2y[idx];
        int nx = _x + dx[dir];
        int ny = _y + dy[dir];
        if (nx < 0 || nx >= N || ny < 0 || ny >= N)
            return false;
        if (dir == 0) {
            return !V[_x][_y];
        } else if (dir == 1) {
            return !H[_x][_y];
        } else if (dir == 2) {
            return !V[nx][ny];
        } else if (dir == 3) {
            return !H[nx][ny];
        }
        assert(false);
    }
    bool can_move_inside(const int& dir) const {
        return can_move_inside_memo_flatten[idx][dir];
    }
    bool can_move_withouthittingwall(const int& dir) const {
        return can_move_withouthittingwall_memo_flatten[idx][dir];
    }
    int cost_move_without_hitting_wall(const int& dir) const {
        return move_cost_memo_flatten[idx][dir];
    }
    bool can_move_inside(const Point& p) const {
        // assert((p - *this).norm() == 1);
        int dir = (*this).get_dir(p);
        return can_move_inside(dir);
    }
    bool can_move_withouthittingwall(const Point& p) const {
        // assert((p - *this).norm() == 1);
        int dir = (*this).get_dir(p);
        return can_move_withouthittingwall(dir);
    }
    // int cost_move_without_hitting_wall(const Point& p) const {
    //     // assert((p - *this).norm() == 1);
    //     int dir = (*this).get_dir(p);
    //     return cost_move_without_hitting_wall(dir);
    // }
    int cost_move_without_hitting_wall(const Point& p) const {
        return move_cost_memo_flatten2[idx][p.idx];
    }
    PointInternal raw() const { return idx; }
    int x() const { return idx2x[idx]; }
    int y() const { return idx2y[idx]; }
    Point move(const int& dir) const {
        // int nx = _x + dx[dir];
        // int ny = _y + dy[dir];
        // return Point(nx, ny);
        return Point(idx + dpoint_raw[dir]);
    }
    void move_inplace(const int& dir) {
        // _x += dx[dir];
        // _y += dy[dir];
        idx += dpoint_raw[dir];
    }
    void operator+=(const Point& other) {
        // _x += other._x;
        // _y += other._y;
        idx += other.idx;
    }
    void operator-=(const Point& other) {
        // _x -= other._x;
        // _y -= other._y;
        idx -= other.idx;
    }
    Point operator+(const Point& other) const {
        // return Point(_x + other._x, _y + other._y);
        return Point(idx + other.idx);
    }
    Point operator-(const Point& other) const {
        // return Point(_x - other._x, _y - other._y);
        return Point(idx - other.idx);
    }
    bool operator!=(const Point& other) const {
        return idx != other.idx;
    }

    int norm() const {
        // return abs(_x) + abs(_y);
        assert(false);
    }
    bool operator==(const Point& other) const {
        // return _x == other._x && _y == other._y;
        return idx == other.idx;
    }
    int get_dir(const Point& p) const;
    int get_dir_naive(const Point& p) const;
    // bool operator!=(const Point& other) const {
    //     return _x != other._x || _y != other._y;
    // }
    // bool operator<(const Point& other) const {
    //     return _x < other._x || (_x == other._x && _y < other._y);
    // }

    // print
    friend ostream& operator<<(ostream& os, const Point& p) {
        os << "(" << p.x() << ", " << p.y() << ")";
        return os;
    }
};
// const Point dpoint[4] = {Point(0, 1), Point(1, 0), Point(0, -1), Point(-1, 0)};
Point dpoint[4] = {};
using Path = vector<Point>;
using Paths = vector<Path>;
// using Dath = deque<Point>;
using Dath = MyDeque<Point>;
// using Lath = list<Point>;

void concat_paths(Path& path1, const Path& path2) {
    // TODO p の選び方

    int D_max = -1;
    vector<int> seen(NN);
    for (const auto& p : path1) {
        seen[p.raw()] = 1;
    }
    for (const auto& p : path2) {
        if (seen[p.raw()] == 1 && chmax(D_max, D_flatten[p.raw()]))
            seen[p.raw()] = 2;
    }
    Point p;
    int idx1 = -1, idx2 = -1;
    for (int i = 0; i < (int)path1.size(); i++) {
        if (seen[path1[i].raw()] == 2 && D_flatten[path1[i].raw()] == D_max) {
            p = path1[i];
            idx1 = i;
            break;
        }
    }
    assert(idx1 != -1);
    for (int i = 0; i < (int)path2.size(); i++) {
        if (path2[i] == p && D_flatten[path2[i].raw()] == D_max) {
            idx2 = i;
            break;
        }
    }
    assert(idx2 != -1);
    // path1.insert(path1.begin() + idx1, path2.begin() + idx2, path2.end());
    // path1.insert(path1.begin() + idx1, path2.begin(), path2.begin() + idx2);
    // cerr << idx1 << " " << idx2 << endl;
    path1.insert(path1.begin() + idx1, path2.begin(), path2.begin() + idx2);
    path1.insert(path1.begin() + idx1, path2.begin() + idx2, path2.end());


    // Point p = path1[0];
    // int idx = -1;
    // for (int i = 0; i < (int)path2.size(); i++) {
    //     if (path2[i] == p) {
    //         idx = i;
    //         break;
    //     }
    // }
    // assert(idx != -1);
    // path1.insert(path1.begin(), path2.begin(), path2.begin() + idx);
    // path1.insert(path1.end(), path2.begin() + idx, path2.end());
}

void print_path_as_ans(const Path& path) {
#ifndef NDEBUG
    // assert(check_path(path));
    // for (int i = 0; i < (int)path.size(); i++) {
    //     cerr << path[i] << " ";
    // }
    // cerr << endl;
#endif
    int idx_begin = -1;
    int len = (int)path.size();
    for (int i = 0; i < len; ++i) {
        Point p = path[i];
        if (idx_begin == -1 && p.x() == 0 && p.y() == 0) {
            idx_begin = i;
            break;
        }
    }
    assert(idx_begin != -1);
    for (int i = 0; i < len; ++i) {
        int idx1 = (idx_begin + i) % len;
        int idx2 = (idx_begin + i + 1) % len;
        int dir = path[idx1].get_dir(path[idx2]);
        cout << DIR[dir];
    }
    cout << endl;
}

void print_path(const Path& path) {
    int len = (int)path.size();
    for (int i = 0; i < len; ++i) {
        Point p = path[i];
        cerr << p.x() << " " << p.y() << endl;
    }
}

// int Point::get_dir(const Point& p) const {
//     PointInternal tmp = p.idx - idx;
//     for (int dir = 0; dir < 4; ++dir) {
//         if (tmp == dpoint_raw[dir])
//             return dir;
//     }
//     cerr << *this << " " << p << endl;
//     assert(false);
// }
vector<vector<int>> get_dir_memo;
int Point::get_dir_naive(const Point& p) const {
    PointInternal tmp = p.idx - idx;
    for (int dir = 0; dir < 4; ++dir) {
        if (tmp == dpoint_raw[dir])
            return dir;
    }
    cerr << *this << " " << p << endl;
    assert(false);
}
int Point::get_dir(const Point& p) const {
    assert(get_dir_memo[idx][p.idx] != -1);
    return get_dir_memo[idx][p.idx];
}


bool check_point_in_shortest_path(const Point& p_begin, const Point& p_mid, const Point& p_end) {
    return dist_flatten[p_begin.raw()][p_mid.raw()] + dist_flatten[p_end.raw()][p_mid.raw()] == dist_flatten[p_begin.raw()][p_end.raw()];
}

// TODO
// paths
// vector<vector<Point>> next_point_memo;
// vector<vector<vector<Point>>> shortest_path_next_point_memo;
// vector<vector<int>> shortest_path_next_point_idx_memo;
Point next_point_memo[NN_MAX][4];
int next_point_memo_size[NN_MAX];
Point shortest_path_next_point_memo[NN_MAX][NN_MAX][4];
int shortest_path_next_point_memo_size[NN_MAX][NN_MAX];
// Point shortest_path_next_point_idx_memo[NN_MAX][NN_MAX];
// int shortest_path_next_point_idx_memo_size[NN_MAX][NN_MAX];


// int __attribute__((noinline)) randrange(int n) {
inline Point get_random_shortest_path_next_point(const Point& p_begin, const Point& p_end) {
    // Point __attribute__((noinline)) get_random_shortest_path_next_point(const Point& p_begin, const Point& p_end) {
    // const int d = shortest_path_next_point_memo[p_end.raw()][p_begin.raw()].size();
    // assert(d >= 1);
    // return shortest_path_next_point_memo[p_end.raw()][p_begin.raw()][randrange(d)];
    // const int d = shortest_path_next_point_memo[p_end.raw()][p_begin.raw()].size();
    // return shortest_path_next_point_memo[p_end.raw()][p_begin.raw()][randrange(d)];
    return shortest_path_next_point_memo[p_end.raw()][p_begin.raw()][randrange(shortest_path_next_point_memo_size[p_end.raw()][p_begin.raw()])];

    // return shortest_path_next_point_memo[p_end.raw()][p_begin.raw()][0];

    // int d = shortest_path_next_point_memo[p_end.raw()][p_begin.raw()].size();
    // return shortest_path_next_point_memo[p_end.raw()][p_begin.raw()][d - 1];

    // if (shortest_path_next_point_idx_memo[p_end.raw()][p_begin.raw()] == shortest_path_next_point_memo[p_end.raw()][p_begin.raw()].size()) {
    //     shortest_path_next_point_idx_memo[p_end.raw()][p_begin.raw()] = 0;
    // }
    // return shortest_path_next_point_memo[p_end.raw()][p_begin.raw()][shortest_path_next_point_idx_memo[p_end.raw()][p_begin.raw()]++];
}
inline Point get_random_next_point(const Point& p_begin) {
    // int sz = next_point_memo[p_begin.raw()].size();
    // return next_point_memo[p_begin.raw()][randrange(sz)];
    int sz = next_point_memo_size[p_begin.raw()];
    return next_point_memo[p_begin.raw()][randrange(sz)];
}
Path calc_random_path(Point p_begin, const Point& p_end, const double& prob_rand);
Path calc_shortest_path(Point p_begin, const Point& p_end);

void twoopt_greedy(Path& path) {
    for (int i = 0; i < (int)path.size(); i++) {
        for (int j = i + 2; j < (int)path.size(); j++) {
            if (dist_flatten[path[i].raw()][path[j].raw()] + dist_flatten[path[i + 1].raw()][path[j + 1].raw()] < dist_flatten[path[i].raw()][path[i + 1].raw()] + dist_flatten[path[j].raw()][path[j + 1].raw()]) {
                reverse(path.begin() + i + 1, path.begin() + j + 1);
            }
        }
    }
}

Path tsp_greedy(Path points_input) {
    sort(points_input.begin(), points_input.end(), [](const Point& p1, const Point& p2) {
        return D_flatten[p1.raw()] > D_flatten[p2.raw()];
    });
    Path path;
    for (auto& p : points_input) {
        if (path.size() == 0) {
            path.emplace_back(p);
            continue;
        }
        int idx_insert = (int)path.size();
        int ddist_min = dist_flatten[path[0].raw()][p.raw()] + dist_flatten[path.back().raw()][p.raw()] - dist_flatten[path[0].raw()][path.back().raw()];
        for (int i = 0; i < (int)path.size() - 1; i++) {
            int ddist = dist_flatten[path[i].raw()][p.raw()] + dist_flatten[path[(i + 1) % path.size()].raw()][p.raw()] - dist_flatten[path[i].raw()][path[i + 1].raw()];
            if (chmin(ddist_min, ddist)) {
                idx_insert = i + 1;
            }
        }
        path.insert(path.begin() + idx_insert, p);
    }
    // twoopt_greedy(path);
    return path;
}

Path complement_path(const Path& path_input) {
    Path path;
    Point p_bef = path_input.back();
    for (const auto& p_nxt : path_input) {
        Path path_insert = calc_shortest_path(p_bef, p_nxt);
        // Path path_insert = calc_random_path(p_bef, p_nxt, 0.2);
        path.insert(path.end(), path_insert.begin(), path_insert.end());
        p_bef = p_nxt;
    }
    return path;
}

// const int Nmax_memo_short_path = 10;
// const int Nmax_memo_short_path = 4;
// constexpr int Nmax_memo_short_path = 4;
constexpr int Nmax_memo_short_path = 3;
vector<vector<vector<Paths>>> memo_short_path(
    Nmax_memo_short_path * 2 + 1,
    vector(Nmax_memo_short_path * 2 + 1,
        vector<Paths>(Nmax_memo_short_path + 1)));
vector<vector<vector<int>>> memo_short_path_size(
    Nmax_memo_short_path * 2 + 1,
    vector(Nmax_memo_short_path * 2 + 1,
        vector<int>(Nmax_memo_short_path + 1)));
// Paths memo_short_path[Nmax_memo_short_path * 2 + 1][Nmax_memo_short_path * 2 + 1][Nmax_memo_short_path + 1];
vector<vector<vector<vector<Border>>>> memo_short_path_border(
    Nmax_memo_short_path * 2 + 1,
    vector(Nmax_memo_short_path * 2 + 1,
        vector<vector<Border>>(Nmax_memo_short_path + 1)));
// Paths memo_short_path[NN_MAX][NN_MAX][Nmax_memo_short_path + 1];
// int memo_short_path_size[NN_MAX][NN_MAX][Nmax_memo_short_path + 1];
// vector<Border> memo_short_path_border[NN_MAX][NN_MAX][Nmax_memo_short_path + 1];


// vector<vector<vector<Paths>>> memo_short_path_with_length(
//     NN_MAX, vector(NN_MAX, vector<Paths>(Nmax_memo_short_path + 1)));
// Paths memo_short_path_with_length[NN_MAX][NN_MAX][Nmax_memo_short_path + 1];

void dfs_short_path(pair<int, int> p, Path& path, Border border) {
    int px = p.first;
    int py = p.second;

    // if (((int)path.size() >= 3) && path.back() == path[path.size() - 3]) {
    //     cerr << 0 << " ";
    //     for (auto& p : path) {
    //         cerr << p.raw() << " ";
    //     }
    //     cerr << endl;
    //     return;
    // }

    int px_with_offset = px + Nmax_memo_short_path;
    int py_with_offset = py + Nmax_memo_short_path;
    int len = (int)path.size();
    memo_short_path[px_with_offset][py_with_offset][len].emplace_back(path);
    memo_short_path_size[px_with_offset][py_with_offset][len]++;
    memo_short_path_border[px_with_offset][py_with_offset][len].emplace_back(border);

    // if (!path.empty() && path.back() == Point(0))
    //     return;

    if (len < Nmax_memo_short_path) {
        for (int dir = 0; dir < 4; ++dir) {
            auto [x_min, x_max, y_min, y_max] = border;
            int px_new = px + dx[dir];
            int py_new = py + dy[dir];
            chmin(x_min, px_new);
            chmax(x_max, px_new);
            chmin(y_min, py_new);
            chmax(y_max, py_new);
            path.emplace_back(Point(xy2idx_func(px_new, py_new)));
            dfs_short_path(make_pair(px_new, py_new), path, Border(x_min, x_max, y_min, y_max));
            path.pop_back();
        }
    }
}
// Path get_random_path(Point p_begin, Point p_end, Path& path, int& path_len, int max_length = Nmax_memo_short_path) {
void get_random_path(Point p_begin, Point p_end, Path& path, int& path_len, int max_length = Nmax_memo_short_path) {
    // Path get_random_path(Point p_begin, Point p_end, int max_length, Path path_tmp = {}) {
    int dpx = p_end.x() - p_begin.x(), dpy = p_end.y() - p_begin.y();
    bool flag_even = (dpx + dpy) % 2 == 0;
    int x_begin = p_begin.x(), y_begin = p_begin.y();
    int len, n_cand;
    while (true) {
        if (flag_even) {
            len = randrange(0, (max_length / 2) + 1) * 2;
            n_cand = memo_short_path_size[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len];
        } else {
            len = randrange(0, max_length / 2) * 2 + 1;
            n_cand = memo_short_path_size[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len];
        }
        if (n_cand == 0)
            continue;
        int idx = randrange(n_cand);
        Border& border = memo_short_path_border[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
        auto [x_min, x_max, y_min, y_max] = border;
        if (x_begin + x_min < 0 || x_begin + x_max >= N || y_begin + y_min < 0 || y_begin + y_max >= N)
            continue;
        // path = memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
        // copy_vector(memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx], path, path_len);

        path_len = memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx].size();
        for (int i = 0; i < path_len; i++) {
            path[i] = p_begin + memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx][i];
        }

        break;
    }
}
void get_random_path_with_length(const Point& p_begin, const Point& p_end, Path& path, const int& path_len) {
    // Path get_random_path(Point p_begin, Point p_end, int max_length, Path path_tmp = {}) {
    int dpx = p_end.x() - p_begin.x(), dpy = p_end.y() - p_begin.y();
    // bool flag_even = (dpx + dpy) % 2 == 0;
    int x_begin = p_begin.x(), y_begin = p_begin.y();
    int n_cand = memo_short_path_size[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][path_len];
#ifndef NDEBUG
    if (n_cand == 0) {
        cerr << p_begin << " " << p_end << " " << path_len << endl;
    }
#endif
    assert(n_cand != 0);
    while (true) {
        int idx = randrange(n_cand);

        // if (path_len == 3 && dist_l1_flatten[p_begin.raw()][p_end.raw()] == 3) {
        //     // Border& border = memo_short_path_border[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][path_len][idx];
        //     // auto [x_min, x_max, y_min, y_max] = border;
        //     // if (x_begin + x_min < 0 || x_begin + x_max >= N || y_begin + y_min < 0 || y_begin + y_max >= N) {
        //     //     cerr << p_begin << " " << p_end << " " << path_len << " " << dist_l1_flatten[p_begin.raw()][p_end.raw()] << endl;
        //     //     exit(0);
        //     // }
        // } else {
        Border& border = memo_short_path_border[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][path_len][idx];
        auto [x_min, x_max, y_min, y_max] = border;
        if (x_begin + x_min < 0 || x_begin + x_max >= N || y_begin + y_min < 0 || y_begin + y_max >= N)
            continue;
        // path = memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
        // copy_vector(memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx], path, path_len);
        // }
        for (int i = 0; i < path_len - 1; i++) {
            path[i] = p_begin + memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][path_len][idx][i];
        }

        // static int cnt_total = 0;
        // static int cnt_wall = 0;
        // bool flag = false;
        // Point p_tmp = p_begin;
        // for (int i = 0; i < path_len; i++) {
        //     if (!p_tmp.can_move_withouthittingwall(path[i])) {
        //         flag = true;
        //         break;
        //     }
        //     p_tmp = path[i];
        // }
        // cnt_total++;
        // if (flag)
        //     cnt_wall++;
        // if (cnt_total % 1000 == 0) {
        //     cerr << cnt_total << " " << 1. * cnt_wall / cnt_total << endl;
        // }

        break;
    }
}
bool get_random_path_with_length_ver2(Point p_begin, Point p_end, Path& path, int& path_len) {
    // Path get_random_path(Point p_begin, Point p_end, int max_length, Path path_tmp = {}) {
    int dpx = p_end.x() - p_begin.x(), dpy = p_end.y() - p_begin.y();
    // bool flag_even = (dpx + dpy) % 2 == 0;
    int x_begin = p_begin.x(), y_begin = p_begin.y();
    int len = path_len;
    int n_cand = memo_short_path_size[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len];
#ifndef NDEBUG
    if (n_cand == 0) {
        cerr << p_begin << " " << p_end << " " << path_len << endl;
    }
#endif
    assert(n_cand != 0);
    while (true) {
        int idx = randrange(n_cand);
        Border& border = memo_short_path_border[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
        auto [x_min, x_max, y_min, y_max] = border;
        if (x_begin + x_min < 0 || x_begin + x_max >= N || y_begin + y_min < 0 || y_begin + y_max >= N)
            continue;
        // path = memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
        // copy_vector(memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx], path, path_len);

        if (path_len > 0) {
            Point p = p_begin;
            for (int i = 0; i < path_len - 1; i++) {
                path[i] = p_begin + memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx][i];
                if (!p.can_move_withouthittingwall(path[i])) {
                    return false;
                }
                p = path[i];
            }
            path[path_len - 1] = p_begin + memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx][path_len - 1];
            if (!p.can_move_withouthittingwall(path[path_len - 1])) {
                return false;
            }
        }
        return true;

        // static int cnt_total = 0;
        // static int cnt_wall = 0;
        // bool flag = false;
        // Point p_tmp = p_begin;
        // for (int i = 0; i < path_len; i++) {
        //     if (!p_tmp.can_move_withouthittingwall(path[i])) {
        //         flag = true;
        //         break;
        //     }
        //     p_tmp = path[i];
        // }
        // cnt_total++;
        // if (flag)
        //     cnt_wall++;
        // if (cnt_total % 1000 == 0) {
        //     cerr << cnt_total << " " << 1. * cnt_wall / cnt_total << endl;
        // }

        break;
    }
}
void init_memo_short_path() {
    // TODO border用のメモ化
    Path path;
    dfs_short_path({0, 0}, path, Border(0, 0, 0, 0));
    // for (int x = 0; x < Nmax_memo_short_path * 2 + 1; x++) {
    //     for (int y = 0; y < Nmax_memo_short_path * 2 + 1; y++) {
    //         int Sum = 0;
    //         for (int i = 0; i <= Nmax_memo_short_path; i++) {
    //             Sum += memo_short_path[x][y][i].size();
    //         }
    //         cerr << x - Nmax_memo_short_path << " " << y - Nmax_memo_short_path << " " << Sum << endl;
    //     }
    // }

    // return;

    // for (int p_begin_idx = 0; p_begin_idx < NN; p_begin_idx++) {
    //     Point p_begin = Point(p_begin_idx);
    //     int x_begin = p_begin.x(), y_begin = p_begin.y();
    //     for (int dpx = -Nmax_memo_short_path; dpx <= Nmax_memo_short_path; dpx++) {
    //         for (int dpy = -Nmax_memo_short_path; dpy <= Nmax_memo_short_path; dpy++) {
    //             int x_end = x_begin + dpx, y_end = y_begin + dpy;
    //             if (x_end < 0 || x_end >= N || y_end < 0 || y_end >= N)
    //                 continue;
    //             Point p_end = Point(x_end, y_end);
    //             for (int len = 0; len <= Nmax_memo_short_path; len++) {
    //                 int n_cand = memo_short_path_size[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len];
    //                 for (int idx = 0; idx < n_cand; idx++) {
    //                     Border& border = memo_short_path_border[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
    //                     auto [x_min, x_max, y_min, y_max] = border;
    //                     if (x_begin + x_min < 0 || x_begin + x_max >= N || y_begin + y_min < 0 || y_begin + y_max >= N)
    //                         continue;
    //                     Path path = memo_short_path[dpx + Nmax_memo_short_path][dpy + Nmax_memo_short_path][len][idx];
    //                     bool flag = true;
    //                     Point p = p_begin;
    //                     // cerr << p_begin << " " << p_end << " " << n_cand << endl;
    //                     // continue;
    //                     // cerr << p_begin << " ";
    //                     for (int i = 0; i < len; i++) {
    //                         path[i] += p_begin;
    //                         // cerr << path[i] << " ";
    //                         if (!p.can_move_withouthittingwall(path[i])) {
    //                             flag = false;
    //                             break;
    //                         }
    //                         p = path[i];
    //                     }
    //                     // cerr << endl;
    //                     if (flag) {
    //                         memo_short_path_with_length[p_begin_idx][p_end.raw()][len].emplace_back(path);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}

class Cost {
    // private:
public:
    ll _numer;
    int _denom;
    int _wall_cnt;
    int _not_visited_cnt;

public:
    Cost() : _numer(0), _denom(0), _wall_cnt(0), _not_visited_cnt(0) {}
    Cost(ll numer, int denom, int wall_cnt, int not_visited_cnt) : _numer(numer), _denom(denom), _wall_cnt(wall_cnt), _not_visited_cnt(not_visited_cnt) {}
    Tcost value() const { return (Tcost)_numer / _denom; }
    int value_int() const { return (int)(_numer / _denom); }
    bool operator<(const Cost& other) const {
        if (_not_visited_cnt != other._not_visited_cnt)
            return _not_visited_cnt < other._not_visited_cnt;
        if (_wall_cnt != other._wall_cnt)
            return _wall_cnt < other._wall_cnt;
        return _numer * other._denom < other._numer * _denom;
    }
    void print() const {
        cerr << "cost = " << value_int() << " (" << _numer << "/" << _denom << ")" << endl;
        cerr << "wall_cnt = " << _wall_cnt << endl;
        cerr << "not_visited_cnt = " << _not_visited_cnt << endl;
    }
    Tcost value_soft() const {
        Tcost ret = value();
        // ret += _wall_cnt * WALL_COST / _denom;
        // ret += _not_visited_cnt * NOT_VISITED_COST;
        return ret;
    }
    bool operator==(const Cost& other) const {
        return _numer == other._numer && _denom == other._denom && _wall_cnt == other._wall_cnt && _not_visited_cnt == other._not_visited_cnt;
    }
};

bool check_path(const vector<Point>& path) {
    int len = (int)path.size();
    vector<vector<int>> last_visit(N, vector<int>(N, 0));
    vector<vector<int>> not_visited(N, vector<int>(N, 1));
    int cnt_visited = 0;
    for (int i = 0; i < len; ++i) {
        Point p = path[i];
        int j = (i == len - 1 ? 0 : i + 1);
        Point q = path[j];
        if (dist_flatten[p.raw()][q.raw()] != 1) {
            cerr << "invalid path : abs(p - q) != 1" << endl;
            return false;
        }
        cnt_visited += not_visited[p.x()][p.y()];
        not_visited[p.x()][p.y()] = 0;
    }
    if (cnt_visited != N * N) {
        cerr << "invalid path : cnt_visited != N * N" << endl;
        return false;
    }
    return true;
}

Cost calc_path_cost(const vector<Point>& path, bool flag_naive = false) {
    Cost cost;
    int len = (int)path.size();
    vector<vector<int>> last_visit(N, vector<int>(N, INF));
    for (int i = 0; i < len; ++i) {
        int j = (i == len - 1 ? 0 : i + 1);
        Point p = path[i];
        Point q = path[j];
        int x = p.x();
        int y = p.y();
        last_visit[x][y] = i - len;
        // cerr << x << " " << y << " " << q.x() << " " << q.y() << endl;
        // cost._wall_cnt += p.cost_move_without_hitting_wall(q);
        // if (!p.can_move(q)) {
        //     cerr << x << " " << y << " " << q.x() << " " << q.y() << endl;
        // }
    }
    for (int i = 0; i < len; i++) {
        Point p = path[i];
        int x = p.x();
        int y = p.y();
        cost._numer += calc_sum_tri(i - last_visit[x][y] - 1) * D[x][y];
        last_visit[x][y] = i;
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            cost._not_visited_cnt += (last_visit[x][y] == INF);
        }
    }

    if (flag_naive) {
        cost._numer = 0;
        int memo[N_MAX][N_MAX] = {};
        auto f = [&](int x, int y) {
            for (int xx = 0; xx < N; xx++) {
                for (int yy = 0; yy < N; yy++) {
                    if ((xx == x) && (yy == y))
                        continue;
                    memo[xx][yy] += D[xx][yy];
                }
            }
            return;
        };
        for (int i = 0; i < len; ++i) {
            Point p = path[i];
            int x = p.x();
            int y = p.y();
            f(x, y);
            memo[x][y] = 0;
        }
        for (int i = 0; i < len; i++) {
            Point p = path[i];
            int x = p.x();
            int y = p.y();
            f(x, y);
            memo[x][y] = 0;
            for (int xx = 0; xx < N; xx++) {
                for (int yy = 0; yy < N; yy++) {
                    cost._numer += memo[xx][yy];
                }
            }
        }
    }

    cost._denom = len;


    return cost;
}


// VVDint convert_path2durations(const Path& path) {
//     int len = (int)path.size();
//     vector<vector<int>> last_visit(N, vector<int>(N, INF));
//     VVDint durations(N, VDint(N));
//     for (int i = 0; i < len; i++) {
//         Point p = path[i];
//         int x = p.x();
//         int y = p.y();
//         last_visit[x][y] = i - len;
//     }
//     for (int i = 0; i < len; i++) {
//         Point p = path[i];
//         int x = p.x();
//         int y = p.y();
//         durations[x][y].emplace_back(i - last_visit[x][y]);
//         last_visit[x][y] = i;
//     }
//     // rotate durations
//     for (int x = 0; x < N; x++) {
//         for (int y = 0; y < N; y++) {
//             auto& dur = durations[x][y];
//             int l = (int)dur.front();
//             dur.pop_front();
//             dur.emplace_back(l);
//         }
//     }
//     return durations;
// }


VDint convert_path2durations_flatten(const Path& path) {
    int len = (int)path.size();
    vector<int> last_visit(NN, INF);
    VDint durations(NN);
    for (int i = 0; i < len; i++) {
        const Point& p = path[i];
        last_visit[p.raw()] = i - len;
    }
    for (int i = 0; i < len; i++) {
        const Point& p = path[i];
        durations[p.raw()].emplace_back(i - last_visit[p.raw()]);
        last_visit[p.raw()] = i;
    }
    // rotate durations
    for (int idx = 0; idx < NN; idx++) {
        auto& dur = durations[idx];
        int l = (int)dur.front();
        dur.pop_front();
        dur.emplace_back(l);
    }
    return durations;
}


class FastPath {
    // O(1) rotate forward 1
    // O(1) delete front
    // O(1) insert front
    // TODO insert back for high performance
    // private:
public:
    Dath dath;
    VDint durations;
    ll current_durationxD_sum;
    ll Dsum;
    vector<int> memo_nextidx;
    int current_offset_duration;
    int current_offset_nextidx;
    Cost cost;

    int replace_path_length;
    Path replace_path_bef;
    Cost cost_bef;

    int n_pop_max;
    int n_pop_bef;

public:
    FastPath() = default;
    FastPath(const Path& path) {
        // dath = Dath(path.begin(), path.end());
        dath = Dath(path);
        durations = convert_path2durations_flatten(path);
        current_durationxD_sum = 0;
        for (int idx = 0; idx < NN; idx++) {
            assert(!durations[idx].empty());
            current_durationxD_sum += durations[idx].back() * D_flatten[idx];
        }
        // memo_offset_duration.resize(N, vector<int>(N, 0));
        memo_nextidx.resize(NN, -1);
        Dsum = 0;
        for (int i = 0; i < (int)path.size(); i++) {
            Point p = path[i];
            int idx = p.raw();
            if (memo_nextidx[idx] == -1)
                memo_nextidx[idx] = i;
        }
        for (int x = 0; x < N; x++) {
            for (int y = 0; y < N; y++) {
                Dsum += D[x][y];
            }
        }
        current_offset_duration = 0;
        current_offset_nextidx = 0;
        cost = calc_path_cost(path);

        replace_path_length = -1;

        n_pop_max = 10000;
        n_pop_bef = 0;

        replace_path_bef.resize(n_pop_max);

        // for (auto& d : durations[0][0]) {
        //     cerr << d << " ";
        // }
        // cerr << endl;

        // ll tmp = 0;
        // for (int x = 0; x < N; x++) {
        //     for (int y = 0; y < N; y++) {
        //         for (int& d : durations[x][y]) {
        //             tmp += D[x][y] * calc_sum_tri(d - 1);
        //         }
        //     }
        // }
        // cerr << tmp << " " << cost._numer << endl;
    }
    void print_state() {
        cerr << "current_offset_duration = " << current_offset_duration << endl;
        cerr << "current_offset_nextidx = " << current_offset_nextidx << endl;
        cerr << "current_durationxD_sum = " << current_durationxD_sum << endl;
        cerr << "Dsum = " << Dsum << endl;
        cerr << "cost = " << cost.value() << endl;
        cerr << "cost_bef = " << cost_bef.value() << endl;
        cerr << "n_pop_bef = " << n_pop_bef << endl;
    }
    void shift_forward() {
        // TODO : fix
        const Point p = dath.front();
        // int x = p.x();
        // int y = p.y();
        // dath.pop_front();
        // dath.emplace_back(p);
        dath.shift_forward();
        auto& dur = durations[p.raw()];

        const int c = dur.back();
        const int c_true = c + current_offset_duration;
        dur.back() += current_offset_duration;
        current_durationxD_sum -= c_true * D_flatten[p.raw()];

        const int d_true = dur.front();
        const int d = d_true - current_offset_duration;
        dur.pop_front();
        dur.emplace_back(d);
        // dur.shift_forward();
        // dur.back() -= current_offset_duration;
        current_durationxD_sum += d_true * D_flatten[p.raw()];

        current_offset_nextidx++;
        memo_nextidx[p.raw()] = d_true - current_offset_duration + current_offset_nextidx - 1;
    }
    void emplace_front(const Point& p) {
        // int x = p.x(), y = p.y();
        dath.emplace_front(p);
        auto& dur = durations[p.raw()];

        if (dur.empty()) {
            current_offset_duration++;

            const int d = (int)dath.size();
            cost._numer += current_durationxD_sum + calc_sum_tri(d - 1) * D_flatten[p.raw()];
            current_durationxD_sum += d * D_flatten[p.raw()] + Dsum;
            Dsum += D_flatten[p.raw()];

            dur.emplace_front((int)dath.size() - current_offset_duration);
            cost._not_visited_cnt--;
        } else {
            current_offset_duration++;
            const int d = memo_nextidx[p.raw()] + current_offset_duration - current_offset_nextidx;
            cost._numer += (calc_sum_tri(d - 1)
                               + calc_sum_tri(dur.back() - d + current_offset_duration - 1)
                               - calc_sum_tri(dur.back() + current_offset_duration - 1))
                               * D_flatten[p.raw()]
                           + current_durationxD_sum;
            current_durationxD_sum -= d * D_flatten[p.raw()] - Dsum;
            dur.back() -= d;

            dur.emplace_front(d);
        }

        memo_nextidx[p.raw()] = -current_offset_duration + current_offset_nextidx;
        cost._denom++;
    }
    int next_idx(const Point& p) {
        return memo_nextidx[p.raw()] + current_offset_duration - current_offset_nextidx;
    }
    int next_idx() {
        return next_idx(dath.back());
    }
    Point pop_front() {
        Point p = dath.front();
        // int x = p.x(), y = p.y();
        dath.pop_front();

        auto& dur = durations[p.raw()];
        const int d = dur.front();
        dur.pop_front();

        if (dur.empty()) {
            const int d = (int)dath.size() + 1;
            cost._numer -= calc_sum_tri(d - 1) * D_flatten[p.raw()];
            Dsum -= D_flatten[p.raw()];
            current_durationxD_sum -= d * D_flatten[p.raw()];
            cost._numer -= current_durationxD_sum - Dsum;
            current_durationxD_sum -= Dsum;
            cost._not_visited_cnt++;
        } else {
            memo_nextidx[p.raw()] = d - current_offset_duration + current_offset_nextidx;
            current_durationxD_sum += d * D_flatten[p.raw()] - Dsum;
            cost._numer += (-calc_sum_tri(dur.back() + current_offset_duration - 1)
                               - calc_sum_tri(d - 1)
                               + calc_sum_tri(dur.back() + d + current_offset_duration - 1))
                               * D_flatten[p.raw()]
                           - current_durationxD_sum;
            dur.back() += d;
        }

        cost._denom--;
        current_offset_duration--;
        return p;
    }
    void emplace_front_without_updating_cost(const Point& p) {
        dath.emplace_front(p);
        auto& dur = durations[p.raw()];

        if (dur.empty()) {
            current_offset_duration++;

            current_durationxD_sum += dath.size() * D_flatten[p.raw()] + Dsum;
            Dsum += D_flatten[p.raw()];

            const int d = (int)dath.size() - current_offset_duration;
            dur.emplace_front(d);
        } else {
            current_offset_duration++;
            const int d = memo_nextidx[p.raw()] + current_offset_duration - current_offset_nextidx;
            current_durationxD_sum += Dsum - d * D_flatten[p.raw()];
            dur.back() -= d;
            dur.emplace_front(d);
        }
        memo_nextidx[p.raw()] = -current_offset_duration + current_offset_nextidx;
    }
    Point pop_front_without_updating_cost() {
        const Point p = dath.front();
        dath.pop_front();

        auto& dur = durations[p.raw()];
        const int d = dur.front();
        dur.pop_front();

        if (dur.empty()) {
            const int d = (int)dath.size() + 1;
            Dsum -= D_flatten[p.raw()];
            current_durationxD_sum -= d * D_flatten[p.raw()] + Dsum;
        } else {
            dur.back() += d;
            memo_nextidx[p.raw()] = d - current_offset_duration + current_offset_nextidx;
            current_durationxD_sum += d * D_flatten[p.raw()] - Dsum;
        }
        current_offset_duration--;
        return p;
    }
    void pop_front(int n) {
        // Point p1 = dath.back();
        for (int i = 0; i < n; i++) {
            // Point p2 = dath.front();
            // assert(p1.can_move_inside(p2));
            // cerr << n << " " << p1.x() << " " << p1.y() << " " << p2.x() << " " << p2.y() << endl;
            // cost._wall_cnt -= p1.cost_move_without_hitting_wall(p2); // because paths do not cross wall
            // p1 = p2; swap(p1, p2);
            pop_front();
        }
        // const Point p2 = dath.front();
        // cost._wall_cnt -= p1.cost_move_without_hitting_wall(p2);
    }
    void emplace_front(const Path& path, const int& path_len) {
        Point p1;
        Point p2 = dath.front();
        for (int i = path_len - 1; i >= 0; i--) {
            p1 = path[i];
            cost._wall_cnt += p1.cost_move_without_hitting_wall(p2);
            emplace_front(p1);
            // p2 = p1;
            swap(p1, p2);
        }
        p1 = dath.back();
        cost._wall_cnt += p1.cost_move_without_hitting_wall(p2);
    }
    void pop_front_without_updating_cost(int n) {
        for (int i = 0; i < n; i++) {
            pop_front_without_updating_cost();
        }
    }
    void emplace_front_without_updating_cost(const Path& path, int n = -1) {
        if (n == -1)
            n = path.size();
        for (int i = n - 1; i >= 0; i--) {
            emplace_front_without_updating_cost(path[i]);
        }
    }
    Cost replace_path_with_shortest_path_simple(const int& len_change) {
        assert(len_change >= 1);
        n_pop_bef = len_change;
        static Point p_begin;
        p_begin = dath[len_change];
        static Point p_end;
        p_end = dath.back();
        if (p_begin == p_end) {
            p_begin = dath[++n_pop_bef];
        }
        // cerr << p_begin << " " << p_end << endl;
        replace_path_length = dist_flatten[p_begin.raw()][p_end.raw()] - 1;

        for (int i = 0; i < n_pop_bef; i++) {
            replace_path_bef[i] = dath[i];
        }
        cost_bef = cost;
        pop_front(n_pop_bef);

        // cerr << p_begin << endl;

        if (replace_path_length > 0) {
            p_begin = get_random_shortest_path_next_point(p_begin, p_end);
            for (int i = 0; i < replace_path_length - 1; i++) {
                // cerr << p_begin << endl;
                emplace_front(p_begin);
                p_begin = get_random_shortest_path_next_point(p_begin, p_end);
            }
            emplace_front(p_begin);
        }
        // cerr << p_begin << " " << p_end << endl;

        // cerr << dath.back();
        // for (int i = 0; i < replace_path_length + 1; i++) {
        //     cerr << " " << dath[i];
        // }
        // cerr << endl;

        return cost;
    }
    Cost replace_path(int n, const Path& path, const int& path_len) {
        // dath[0...n) -> path
        replace_path_length = path_len;
        n_pop_bef = n;
        for (int i = 0; i < n_pop_bef; i++) {
            replace_path_bef[i] = dath[i];
        }
        cost_bef = cost;
        pop_front(n);
        emplace_front(path, path_len);
        return cost;
    }
    // ll get_dnumer(const Point& p) {
    //     // TODO 高速化
    //     // emplace_front しないでもコストを計算できるようにする
    //     // ついでに疑似 emplace_front も実装
    //     emplace_front(Point(dath.back()));
    //     Cost cost_bef = cost;
    //     pop_front();
    //     emplace_front(p);
    //     Cost cost_new = cost;
    //     pop_front();
    //     return cost_new._numer - cost_bef._numer;
    //     // return cost_bef._numer - cost_new._numer;
    // }
    Cost replace_path_with_best_shortest_path(int n) {
        assert(dist_flatten[dath.back().raw()][dath[dath.size() - 2].raw()] == 1);
        // TODO
        // cerr << "size = " << dath.size() << endl;
        n_pop_bef = n;
        // if (n_pop_bef == 0) {
        //     cerr << "---------------------------------" << endl;
        // }
        // cerr << "n_pop_bef = " << n_pop_bef << endl;
        for (int i = 0; i < n_pop_bef; i++) {
            replace_path_bef[i] = dath[i];
        }
        cost_bef = cost;
        // cerr << dath.back() << " ";
        // for (int i = 0; i <= n_pop_bef; i++) {
        //     cerr << dath[i] << " ";
        // }
        // cerr << endl;
        pop_front(n);
        Cost cost_memo = cost;
        // print_state();

        // 探索
        static vector<ll> memo_numer(NN);
        static vector<Point> memo_best_bef(NN);
        static Path path(NN);
        static int iter = 1;
        static vector<int> seen(NN);
        iter++;
        int path_len = 0;
        Point p_begin = dath[0];
        Point p_end = dath.back();
        assert(p_begin != p_end);
        queue<Point> que;
        que.emplace(p_begin);
        memo_numer[p_begin.raw()] = 0;
        int dist_bef = dist_flatten[p_begin.raw()][p_end.raw()];
        int dist_init = dist_bef;
        int dist_now;
        int n_emplace_front = 0;

        // int size_init = (int)dath.size();

        auto get_dnumer = [&](const Point& p) {
            auto& dur = durations[p.raw()];
            // cerr << p << " dur size = " << dur.size() << endl;
            if (dur.empty()) {
                return -2 * INFLL;
            } else {
                // TODO バグってないか確認
                // int d_total = dur.back() + current_offset_duration + dist_bef;
                // int d2 = memo_nextidx[p.raw()] + current_offset_duration - current_offset_nextidx + 1;
                // int d1 = d_total - d2;
                int d2 = memo_nextidx[p.raw()] + current_offset_duration - current_offset_nextidx;
                int d1 = dur.back() - d2 + current_offset_duration;
                int d_total = d1 + d2;
                // cerr << d1 << " " << d2 << " " << d_total << endl;
                // cerr << p << " " << dath[dath.size() - d1] << " " << dath[d2] << endl;

                assert(p == dath[dath.size() - d1]);
                assert(p == dath[d2]);
                assert(d1 + d2 == d_total);

                // cerr << d1 << " " << d2 << " " << d_total << endl;

                // d_total += dist_init - 1;
                d2 += n_emplace_front + 1;
                // d1 = d_total - d2;
                // d1 += dist_init - n_emplace_front - 1;
                d1 += dist_init - n_emplace_front - 2;
                d_total = d1 + d2;

                // cerr << p << " " << d1 << " " << d2 << " " << d_total << endl;

                ll dnumer = (calc_sum_tri(d1 - 1) + calc_sum_tri(d2 - 1) - calc_sum_tri(d_total - 1)) * D_flatten[p.raw()];
                return dnumer;
            }
        };

        // cerr << "p_begin = " << p_begin << endl;
        // cerr << "p_end = " << p_end << endl;
        while (!que.empty()) {
            Point p_now = que.front();
            // cerr << "p_now = " << p_now << endl;
            que.pop();
            int idx = p_now.raw();
            if (p_now == p_end) {
                break;
            }
            dist_now = dist_flatten[idx][p_end.raw()];
            if (dist_now != dist_bef) {
                n_emplace_front++;
                // emplace_front_without_updating_cost(p_now);  // dummy
                // emplace_front(p_begin);  // dummy
                dist_bef = dist_now;
            }
            // for (const auto& p_nxt : shortest_path_next_point_memo[p_end.raw()][p_now.raw()]) {
            for (int i = 0; i < shortest_path_next_point_memo_size[p_end.raw()][p_now.raw()]; i++) {
                Point p_nxt = shortest_path_next_point_memo[p_end.raw()][p_now.raw()][i];
                // cerr << "p_nxt = " << p_nxt << endl;
                ll dcost = 0;
                if (p_nxt != p_end)
                    dcost = get_dnumer(p_nxt);
                ll cost_nxt = memo_numer[idx] + dcost;
                if (seen[p_nxt.raw()] != iter) {
                    seen[p_nxt.raw()] = iter;
                    memo_numer[p_nxt.raw()] = cost_nxt;
                    memo_best_bef[p_nxt.raw()] = p_now;
                    que.emplace(p_nxt);
                } else {
                    if (memo_numer[p_nxt.raw()] > cost_nxt) {
                        memo_numer[p_nxt.raw()] = cost_nxt;
                        memo_best_bef[p_nxt.raw()] = p_now;
                    }
                }
            }
        }
        // cerr << n_emplace_front - dist_init + 1 << endl;
        assert(n_emplace_front == dist_init - 1);
        // cerr << memo_numer[p_end.raw()] << endl;
        // pop_front_without_updating_cost(n_emplace_front);
        // pop_front(n_emplace_front);
        Point p_now = p_end;
        // cerr << "start path" << endl;
        // cerr << p_now << endl;
        p_now = memo_best_bef[p_now.raw()];
        while (p_now != p_begin) {
            // cerr << path_len << " " << p_begin << " " << p_end << " " << p_now << endl;
            path[path_len++] = p_now;
            // if (p_now == memo_best_bef[p_now.raw()])
            //     exit(0);
            p_now = memo_best_bef[p_now.raw()];
        }
        // cerr << p_now << endl;
        // exit(0);
        // cerr << "print path" << endl;
        // for (int i = 0; i < path_len; i++) {
        //     cerr << i << " " << path[i] << endl;
        // }

        // assert(size_init == (int)dath.size());


        // print_state();
        // cost = cost_memo;
        replace_path_length = path_len;
        emplace_front(path, path_len);


        // if (!((n_pop_bef == 0) || (dist_flatten[dath.back().raw()][dath[0].raw()] == 1))) {
        //     cerr << "n_pop_bef = " << n_pop_bef << endl;
        //     cerr << "dath.back() = " << dath.back() << endl;
        //     cerr << "dath[n_pop_bef] = " << dath[n_pop_bef] << endl;
        //     cerr << "dist_flatten[dath.back().raw()][dath[0].raw()] = " << dist_flatten[dath.back().raw()][dath[n_pop_bef].raw()] << endl;
        //     assert(false);
        // }

        if (true) {
            int cnt = 0;
            for (int i = 0; i < size(); i++) {
                int j = i + 1;
                if (j == size())
                    j = 0;
                if (dist_flatten[dath[i].raw()][dath[j].raw()] != 1)
                    cnt++;
            }
            if (cnt >= 1) {
                cost.print();
                cerr << "size = " << size() << endl;
                cerr << dath.back() << " ";
                for (int i = 0; i <= path_len; i++) {
                    cerr << dath[i] << " ";
                }
                cerr << endl;
                for (int i = 0; i < size(); i++) {
                    int j = i + 1;
                    if (j == size())
                        j = 0;
                    if (dist_flatten[dath[i].raw()][dath[j].raw()] != 1)
                        cerr << i << " " << j << " " << dath[i] << " " << dath[j] << " " << dist_flatten[dath[i].raw()][dath[j].raw()] << endl;
                }
                assert(false);
            }
        }

        // cerr << "total path begin" << endl;
        // cerr << dath.back() << endl;
        // for (int i = 0; i < path_len + 4; i++) {
        //     cerr << dath[i] << endl;
        // }
        // cerr << "total path end" << endl;

        return cost;
    }
    Cost replace_path_with_shortest_path(int n) {
        // not so fast...
        // dath[0...n) -> shortest path
        // cerr << n << endl;
        const Point p_end = dath.back();
        Point p_begin = dath[n - 1];
        cost_bef = cost;
        if (p_begin == p_end) {
            replace_path_length = 0;
            n_pop_bef = n;
            for (int i = 0; i < n_pop_bef; i++) {
                replace_path_bef[i] = dath[i];
            }
            pop_front(n);
            cost._wall_cnt += p_begin.cost_move_without_hitting_wall(dath[0]);
        } else {
            n_pop_bef = n - 1;
            for (int i = 0; i < n_pop_bef; i++) {
                replace_path_bef[i] = dath[i];
            }
            pop_front(n_pop_bef);
            assert(Point(dath[0]) == p_begin);
            replace_path_length = dist_flatten[p_begin.raw()][p_end.raw()] - 1;
            Point p_nxt = get_random_shortest_path_next_point(p_begin, p_end);
            while (p_nxt != p_end) {
                cost._wall_cnt += p_begin.cost_move_without_hitting_wall(p_nxt);
                emplace_front(p_nxt);
                p_begin = p_nxt;
                p_nxt = get_random_shortest_path_next_point(p_begin, p_end);
            }
            cost._wall_cnt += p_begin.cost_move_without_hitting_wall(p_nxt);
        }
        return cost;
    }
    Cost replace_path_undo() {
        // undo replace_path
        // TODO do not calculate cost
        pop_front_without_updating_cost(replace_path_length);
        // cerr << "n_pop_bef = " << n_pop_bef << endl;
        // cerr << dath.back() << " " << dath[0] << endl;
        assert((n_pop_bef == 0) || (dist_flatten[dath.back().raw()][replace_path_bef[0].raw()] == 1));
        emplace_front_without_updating_cost(replace_path_bef, n_pop_bef);
        replace_path_length = 0;
        n_pop_bef = 0;
        cost = cost_bef;
        // cerr << dath.back() << " " << dath[0] << endl;
        assert(dist_flatten[dath.back().raw()][dath[0].raw()] == 1);
        return cost;
    }
    void reset_undo() {
        cost_bef = cost;
        replace_path_length = 0;
        n_pop_bef = 0;
    }
    void shift_forward(int n) {
        for (int i = 0; i < n; i++) {
            shift_forward();
        }
    }
    bool check_cost(bool flag_print = false) {
        Cost cost_true = calc_path_cost(get_path());
        if (flag_print) {
            cost.print();
            cost_true.print();
        }
        if (cost != cost_true) {
            cerr << size() << endl;
            cost.print();
            cost_true.print();
        }
        return cost == cost_true;
    }
    Path get_path() {
        // return Path(dath.begin(), dath.end());
        return dath.get_data();
    }
    int size() {
        return (int)dath.size();
    }
};

class Solver_sa_fast_1 {
public:
    FastPath fastpath;
    Cost cost;
    Cost cost_best;
    Path path_best;

    Solver_sa_fast_1(const vector<Point>& path) : cost(), cost_best() {
        fastpath = FastPath(path);
        cost = fastpath.cost;
        cost_best = cost;
        path_best = path;
    }
    void reload_best_path() {
        fastpath = FastPath(path_best);
        cost = fastpath.cost;
        cost_best = cost;
    }
    void flip_path() {
        Path path = fastpath.get_path();
        reverse(path.begin(), path.end());
        fastpath = FastPath(path);
    }
    void double_path(int n_double) {
        Path path = fastpath.get_path();
        for (int i = 1; i < n_double; i++) {
            if (fastpath.size() + path.size() < MAX_LEN_PRACTICLE)
                fastpath.emplace_front(path, path.size());
        }
        cost = fastpath.cost;
    }
    void simulated_annealing_reheat(SimulatedAnnealingConfig config) {
        // setup config
        const int max_iter = config.max_iter;
        int max_time = config.max_time;
        int max_n_double = config.max_n_double;
        double temp_begin = config.temp_begin;
        double temp_end = config.temp_end;
        const bool flag_true_prob = config.flag_true_prob;

        int iter = 0;
        int iter_hit_wall_total = 0;
        int iter_total = 0;
        int iter_accept = 0;
        int time_begin = get_time();
        int time_end = time_begin + max_time;
        int total_time = time_end - time_begin;
        int time_batch = total_time / (max_n_double + 1);

        int neighbor_bef = 0;
        vector<int> neighbor_accept_cnt(20);

        for (int n_double = 0; n_double <= max_n_double; ++n_double) {

            if (n_double != 0 && fastpath.size() < MAX_LEN_PRACTICLE / 2) {
                Path path = fastpath.get_path();
                // int cnt = 0;
                while (fastpath.size() + path.size() < MAX_LEN_PRACTICLE) {
                    fastpath.emplace_front(path, path.size());
                    // cnt++;
                    // if (cnt == 3)
                    break;
                }
                // int n = path.size();
                // for (int i = n - 1; i >= 0; i--) {
                //     fastpath.emplace_front(path[i]);
                // }
                cost = fastpath.cost;
            }
            iter = 0;
            cost = fastpath.cost;

            int time_begin = get_time();
            int time_now = time_begin;
            int counter = 0;
            int time_remaining = time_batch;
            double temp = temp_begin;

            int iter_batch_total = (1 << 13);
            int iter_batch_cnt = 0;
            // ll len_sum_batch = 0;
            double len_decay_rate = 0.9999;
            double len_decay_batch = (int)fastpath.size();

            // #ifdef FLAG_PRINT_ACCEPTANCE_RATIO
            int iter_accept_batch = 0;
            // #endif

            static bool flag_reached_top = false;
            flag_reached_top = false;

            double time_ratio = 0;

            temp = temp_begin;
            // int iter_accept_temp = 0;
            // int iter_update_temp = 0;
            // int iter_accept_temp_max = 2000;
            // int iter_update_temp_max = 5000;
            // double temp_decay_rate = 0.99;

            while (max_iter == INF || iter < max_iter) {
                if (++counter == iter_batch_total) {
                    time_now = get_time();
                    time_remaining = time_batch - (time_now - time_begin);
                    if (time_remaining <= 0)
                        break;
                        // if (max_iter == INF) {
                        //     temp = temp_begin + (temp_end - temp_begin) * (time_batch - time_remaining) / time_batch;
                        // } else {
                        //     temp = temp_begin + (temp_end - temp_begin) * iter / max_iter;
                        // }


                        // temp *= 0.999;
                        // iter_accept_batch = 0;
#ifdef FLAG_PRINT_ACCEPTANCE_RATIO
                    cerr << "acceptance ratio = " << 1. * iter_accept_batch / iter_batch_total << " " << iter_accept_batch << " " << iter_batch_total << endl;
                    iter_accept_batch = 0;
#endif
                    counter = 0;
                    time_ratio = 1. * (time_batch - time_remaining) / time_batch;
                }

                // if ((iter_update_temp == iter_update_temp_max) || (iter_accept_temp == iter_accept_temp_max)) {
                //     // cerr << iter_update_temp << " " << iter_accept_temp << endl;

                //     temp *= temp_decay_rate;
                //     iter_accept_temp = 0;
                //     iter_update_temp = 0;
                // }

                iter_batch_cnt++;
                // len_sum_batch += fastpath.size();
                len_decay_batch = len_decay_rate * len_decay_batch + (1 - len_decay_rate) * fastpath.size();
                if (iter_batch_cnt == iter_batch_total) {
                    // lower temperature or not
                    // int len_avg_batch = len_sum_batch / iter_batch_total;
                    int len_avg_batch = len_decay_batch;
                    // if (len_avg_batch > 9000)
                    //     temp *= 0.990;
                    // else if (len_avg_batch < 4000)
                    //     temp *= 1.01;
                    // else
                    //     temp *= 0.990;
                    // temp *= 1 + clamp((3000 - len_avg_batch), -1000, 1000) * 0.000001;

                    // int len_begin = 2000;
                    // int len_end = 1000;
                    // int target_len = len_begin + (len_end - len_begin) * (time_batch - time_remaining) / time_batch;
                    // temp *= 1 + clamp((target_len - len_avg_batch), -1000, 1000) * 0.000001;

                    // if (fastpath.size() > 6000) {
                    //     // temp_begin = temp;
                    //     flag_reached_top = true;
                    // }

                    // if (flag_reached_top) {
                    //     temp *= 0.995;
                    // } else {
                    //     temp *= 1.005;
                    // }

                    // temp = 1000;
                    // double temp_calc_from_time = temp_begin + (temp_end - temp_begin) * (time_batch - time_remaining) / time_batch;
                    // chmin(temp, temp_calc_from_time);
                    temp = temp_begin + (temp_end - temp_begin) * (time_batch - time_remaining) / time_batch;

                    // len_sum_batch = 0;


                    // double iter_accept_ratio = 1. * iter_accept_batch / iter_batch_total;
                    // cerr << iter << " " << iter_accept_ratio << endl;

                    // int n_neighbor_type = 10;
                    // vector<double> V_accept_ratio(n_neighbor_type);
                    // for (int i = 0; i < n_neighbor_type; i++) {
                    //     double accept_ratio = 1. * neighbor_accept_cnt[i] / iter_batch_total;
                    //     V_accept_ratio[i] = accept_ratio;
                    //     // cerr << accept_ratio << " ";
                    // }
                    // cerr << endl;
                    // double accept_ratio_sum = accumulate(V_accept_ratio.begin(), V_accept_ratio.end(), 0.);
                    // for (int i = 0; i < n_neighbor_type; i++) {
                    //     cerr << V_accept_ratio[i] / accept_ratio_sum << " ";
                    // }
                    // cerr << endl;

                    // fill(neighbor_accept_cnt.begin(), neighbor_accept_cnt.end(), 0);
                    iter_batch_cnt = 0;
                    // iter_accept_batch = 0;
                }


                // if (cost._not_visited_cnt > 0) {
                //     cerr << "not visited " << iter << " " << cost._not_visited_cnt << endl;
                // }


                // if (iter >= 10000)
                //     break;
                int neighbor_type = get_random_neighbor();
                if (neighbor_type == -1)
                    continue;

                // iter_update_temp++;


                if (fastpath.size() > MAX_LEN) {
                    // assert(false);
                    // temp_begin = 2e1;
                    // TODO
                    // check when making neighbor
                    undo();
                    continue;
                }
                if (cost._wall_cnt > 0 || cost._not_visited_cnt > 0) {
                    // TODO
                    // do not hit wall when making neighbor
                    iter_hit_wall_total++;
                    undo();
                    continue;
                }

                // if (check_path(fastpath.get_path())) {
                //     fastpath.cost.print();
                //     for (int i = 0; i < fastpath.size(); i++) {
                //         int j = i + 1;
                //         if (j == fastpath.size())
                //             j = 0;
                //         if (dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] != 1) {
                //             cerr << i << " " << j << " " << fastpath.dath[i] << " " << fastpath.dath[j] << " " << dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] << endl;
                //         }
                //     }
                //     // assert(false);
                // }
                // cerr << iter << " " << fastpath.size() << endl;
                // assert(fastpath.check_cost(false));

                // update_cost_coef(time_ratio);

                Tcost cost_soft_original = fastpath.cost_bef.value_soft();
                Tcost cost_soft_new = fastpath.cost.value_soft();
                // if (fastpath.cost._not_visited_cnt > 0) {
                //     undo();
                //     continue;
                // }
                Tcost cost_soft_diff = cost_soft_new - cost_soft_original;
                // chmin(temp, 50.);
                double prob = 0;
                // if (iter == 10000)
                //     break;
                if (iter % 100000 == 0) {
                    // fastpath.cost.print();
                    // print_path_as_ans(fastpath.get_path());
                    // cost_best.print();

#ifdef FLAG_PRINT_ANS_MIDDLE
                    cerr << iter << " " << cost.value_int() << " " << cost_best.value_int() << " " << WALL_COST << " " << NOT_VISITED_COST << " " << fastpath.size() << " " << cost_soft_original << " " << cost_soft_new << " " << cost_soft_diff << " " << temp << endl;
                    // if (cost_new._wall_cnt == 0 && cost_new._not_visited_cnt == 0)
                    print_path_as_ans(fastpath.get_path());
#endif
                }

                if (cost_soft_diff < 0) {
                    prob = 1;
                } else {
                    if (flag_true_prob) {
                        // prob = randbool(exp(-cost_soft_diff / temp));
                        prob = log_rand() * temp < -cost_soft_diff;
                        // if (temp < -1 && prob)
                        //     cerr << temp << " " << prob << endl;
                    } else {
                        // prob = exp(-cost_soft_diff / temp);
                        // 等価
                        prob = ((cost_soft_diff / temp) < 745);
                        // cerr << bool(prob) << " " << (-cost_soft_diff / temp) << endl;
                    }
                    // prob = randbool(exp(-cost_soft_diff / temp));
                    // cerr << prob << endl;
                }

                // if (temp < 0 && prob) {
                //     cerr << cost_soft_diff << " " << temp << endl;
                // }

                // if (cost_soft_diff < temp / 2)
                //     prob = true;

                // prob = 1;
                if (prob) {
                    // if (cost_new < cost) {
                    // cost = cost_new;
                    // path = path_new;
                    iter_accept++;
                    iter_accept_batch++;
                    // iter_accept_temp++;
                    if (neighbor_type >= 0)
                        neighbor_accept_cnt[neighbor_type]++;
                    // fastpath.reset_undo();
                } else {
                    undo();
                }
                if (fastpath.cost < cost_best) {
                    cost_best = fastpath.cost;
                    // if (time_ratio > 0.9)
                    path_best = fastpath.get_path();
                }
                iter++;
            }

            iter_total += iter;
            temp_begin = 1e0;
        }
        cerr << " total iter = " << iter_total << endl;
        cerr << "accept ratio = " << 1. * iter_accept / iter_total << endl;
        cerr << "wall hit ratio = " << 1. * iter_hit_wall_total / iter_total << endl;
        cost_best.print();
    }
    void hill_climbing(int max_iter = INF, int max_n_double = 6) {
        // *this = Solver_sa_fast_1(path_best);
        int iter = 0;
        int n_double = 0;
        int total_time = get_time_remaining();
        while (iter < max_iter) {
            // cerr << iter << endl;
            if (max_iter == INF) {
                if (1. * total_time / (max_n_double + 1) * (n_double + 1) < total_time - get_time_remaining()) {
                    if (fastpath.size() < MAX_LEN_PRACTICLE / 2) {
                        Path path = fastpath.get_path();
                        int n = path.size();
                        for (int i = n - 1; i >= 0; i--) {
                            fastpath.emplace_front(path[i]);
                        }
                        cost = fastpath.cost;
                    }
                    n_double++;
                }
            }
            // if (get_time_remaining() < 0)
            //     break;
            Cost cost_bef = cost;
            get_random_neighbor();

            // TODO
            // fastpath.check_cost();
            assert(fastpath.check_cost());

            if (cost_bef < cost) {
                undo();
            }
            // cost.print();
            iter++;
        }
        cerr << "total iter = " << iter << endl;
        cost.print();
        if (cost < cost_best) {
            cost_best = cost;
            path_best = fastpath.get_path();
        }
    }
    void hill_climing_dp() {
        int iter = 0;
        while (get_time_remaining() >= 0) {
            if (fastpath.size() < MAX_LEN_PRACTICLE / 2) {
                double_path(2);
            }
            int n_shift = randrange(1, 10);
            fastpath.shift_forward(n_shift);
            Cost cost_bef = cost;
            Point p_begin = fastpath.dath.back();
            // int d = min(N * 3, (int)fastpath.size() / 4);
            int d = (int)fastpath.size() / 4;
            // int len_change = randrange(d / 2, d);
            int len_change = randrange(2, d);
            // int len_change = randrange(1, 20);
            Point p_end = fastpath.dath[len_change];
            // cerr << p_begin << " " << p_end << " " << len_change << endl;
            if (p_begin == p_end)
                continue;
            iter++;
            cost = fastpath.replace_path_with_best_shortest_path(len_change);
            assert(fastpath.check_cost());
            // cerr << "not visited = " << cost._not_visited_cnt << " " << cost_bef._not_visited_cnt << endl;
            if (len_change + 1 == dist_flatten[p_begin.raw()][p_end.raw()]) {
                cerr << "len same" << endl;
                assert(!(cost_bef < cost));
            }
            if (cost_bef < cost) {
                // cerr << "undo" << endl;
                fastpath.replace_path_undo();
                assert(cost_bef == fastpath.cost);
                cost = cost_bef;
            }
        }
        cerr << "total iter = " << iter << endl;
        if (cost < cost_best) {
            cost_best = cost;
            path_best = fastpath.get_path();
        }
    }
    int get_random_neighbor() {
        assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[fastpath.dath.size() - 2].raw()] == 1);
        assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[0].raw()] == 1);
        // static Path path_insert(Nmax_memo_short_path + 1);
        static Path path_insert(100 + 1);
        // static Path path_insert(NN);
        int path_insert_len;
        // int n_shift = randrange(1, Nmax_memo_short_path);
        // int n_shift = randrange(1, 5);

        // int n_shift = 1;
        fastpath.reset_undo();
        fastpath.shift_forward();
        // fastpath.reset_undo();
        assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[fastpath.dath.size() - 2].raw()] == 1);
        int len_change;
        Point p_begin, p_end;
        p_begin = fastpath.dath.back();

        int neighbor_type;

        // if (fastpath.durations[p_begin.raw()].size() >= 2 && randbool_const<0.2>())
        //     neighbor_type = 7;
        // else if (randbool_const<0.995>())
        // if (randbool_const<0.993f>())
        //     neighbor_type = 0;
        // // else if (randbool<0.5>())
        // else if (false)
        //     neighbor_type = 1;
        // else if (true)
        //     neighbor_type = 2;
        // else
        //     neighbor_type = 3;


        // if (randbool_const<0.03f>())
        //     neighbor_type = 7;
        // else if (randbool_const<0.03f>())
        //     neighbor_type = 8;

        if (randbool_const<0.993f * 0.94f>())
            neighbor_type = 0;
        else if (randbool_const<0.45f>())
            neighbor_type = 7;
        else if (randbool_const<0.817f>())
            neighbor_type = 8;
        else if (true)
            neighbor_type = 2;


        // if (randbool(0.001))
        //     neighbor_type = 2;

        // {
        //     if (fastpath.durations[p_begin.raw()].size() >= 2) {
        //         int idx = fastpath.next_idx();
        //         if (idx <= N && fastpath.durations[p_begin.raw()].size() >= 2 && randbool<0.2>())
        //             // if (idx <= N)
        //             neighbor_type = 4;
        //     }
        // }


        // {
        //     if (dist_flatten[p_begin.raw()][fastpath.dath[2].raw()] == 1
        //         && randbool(0.00)) {
        //         neighbor_type = 6;
        //     }
        // }


        // DEBUG
        // if (neighbor_type >= 2)
        //     neighbor_type = 0;
        // // neighbor_type = 1;
        // if (randbool(0.5))
        //     neighbor_type = 1;
        // neighbor_type = 0;

        // cerr << "start neighbor type = " << neighbor_type << endl;

        // if (neighbor_type >= 1)
        // neighbor_type = 0;

        // if (randbool<0.01>())
        //     neighbor_type = 4;

        if (neighbor_type == 0) {
            // if (randbool(0.99)) {  // best
            // if (randbool(1.)) {
            //    if (randbool(0.)) {

            len_change = randrange(1, Nmax_memo_short_path + 1);
            // len_change = rand_select({0, 1, 1.5, 1.5, 1, 0});
            // len_change = randrange(4) + 1;

            // double p = randdouble();
            // if(p < 0.5)
            //     len_change = 0;

            int len_change_original = len_change;
            // len_change = randrange(0, Nmax_memo_short_path + 3);
            // int max_length_path_insert = min(
            //     Nmax_memo_short_path,
            //     MAX_LEN - (int)fastpath.size() + len_change - 1);
            int max_length_path_insert = Nmax_memo_short_path;  // TODO 高速化?
            // if (len_change > 0) {
            if (true) {
                p_end = fastpath.dath[len_change - 1];
                // TODO dist_flatten -> dist_without_floating_wall に変更
                int d = dist_flatten[p_begin.raw()][p_end.raw()];
                // if (d > max_length_path_insert || d > Nmax_memo_short_path) {
                //     assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[0].raw()] == 1);
                //     cerr << "d = " << d << endl;
                //     return -1;
                // }
                // path_insert_len = randrange(-1, 2) * 2 + len_change;
                // if (path_insert_len < 0)
                //     path_insert_len += 2;
                // if (path_insert_len > Nmax_memo_short_path)
                //     path_insert_len -= 2;
                // if (path_insert_len < dist_flatten[p_begin.raw()][p_end.raw()])
                //     return;
                // chmin(path_insert_len, dist_flatten[p_begin.raw()][p_end.raw()]);
                // cerr << p_begin << " " << p_end << " " << path_insert_len << " " << dist_flatten[p_begin.raw()][p_end.raw()] << endl;

                // original
                // get_random_path(p_begin, p_end, path_insert, path_insert_len, max_length_path_insert);

                // 直線を排除
                if (len_change == 2 && d == 2 && p_end.raw() + p_begin.raw() == fastpath.dath[0].raw() * 2) {
                    len_change = 1;
                    d = 1;
                    p_end = fastpath.dath[0];
                }

                // Try
                path_insert_len = len_change;
                assert(len_change == path_insert_len);
                if ((len_change >= 2) && (len_change - d >= 2) && randbool_const<0.6f>()) {
                    // if ((len_change >= 2) && (len_change - d >= 2) && randbool<1.0>()) {
                    path_insert_len -= 2;
                    // } else if (len_change <= Nmax_memo_short_path - 2 && randbool_const<0.8>()) {
                } else if (len_change <= Nmax_memo_short_path - 2) {
                    path_insert_len += 2;
                }
                // cerr << len_change << " " << path_insert_len << endl;
                // if (len_change == path_insert_len) {
                //     // cerr << len_change << " " << d << endl;
                //     return -1;
                // }

                // cerr << len_change << " " << d << " " << path_insert_len << endl;


                if (len_change == 2 && path_insert_len == 2 && d == 2) {
                    path_insert[0] = Point(p_begin.raw() + p_end.raw() - fastpath.dath[0].raw());
                } else {
                    get_random_path_with_length(
                        p_begin, p_end, path_insert, path_insert_len);
                }

                // get_random_path_with_length(
                //     p_begin, p_end, path_insert, path_insert_len);

                // bool flag = get_random_path_with_length_ver2(
                //     p_begin, p_end, path_insert, path_insert_len);
                // if (!flag)
                //     return -2;

                if (path_insert_len == 3 && d == 3 && path_insert[0] == fastpath.dath[0] && path_insert[1] == fastpath.dath[1]) {
                    if (p_begin.raw() + path_insert[1].raw() == path_insert[0].raw() * 2) {
                        path_insert[1] = Point(path_insert[0].raw() + p_end.raw() - path_insert[1].raw());
                    } else {
                        path_insert[0] = Point(p_begin.raw() + path_insert[1].raw() - path_insert[0].raw());
                    }
                    // static int cnt = 0;
                    // cnt++;
                    // if (cnt % 10000 == 0)
                    //     cerr << "cnt = " << cnt << endl;
                }

                if (0) {
                    static int cnt = 0;
                    bool flag = false;
                    if (p_begin == p_end && path_insert_len == 2)
                        flag = true;
                    else {
                        for (int i = 0; i < len_change; i++) {
                            if (fastpath.dath[i] != path_insert[i]) {
                                flag = false;
                                break;
                            }
                        }
                    }
                    if (flag) {
                        cnt++;
                        if (cnt % 10000 == 0)
                            cerr << "cnt = " << cnt << endl;
                    }
                }


                if (0) {
                    static int cnt_same = 0;
                    bool flag = true;
                    if (len_change != path_insert_len)
                        flag = false;
                    else {
                        for (int i = 0; i < len_change; i++) {
                            if (fastpath.dath[i] != path_insert[i]) {
                                flag = false;
                                break;
                            }
                        }
                    }
                    if (flag) {
                        cnt_same++;
                        if (cnt_same % 10000 == 0)
                            cerr << "cnt_same = " << cnt_same << endl;
                    }
                }


                // Point p = p_begin;
                // for (int i = 0; i < path_insert_len; i++) {
                //     if (!p.can_move_withouthittingwall(path_insert[i]))
                //         return -2;
                //     p = path_insert[i];
                // }

                if (path_insert_len > 0) {
                    len_change--;
                    path_insert_len--;
                }


                if (0) {
                    static int cnt = 0;
                    bool flag = true;
                    if (len_change != path_insert_len)
                        flag = false;
                    else {
                        for (int i = 0; i < len_change; i++) {
                            if (fastpath.dath[i] != path_insert[i]) {
                                flag = false;
                                break;
                            }
                        }
                    }
                    if (flag) {
                        cnt++;
                        // cerr << fastpath.dath.back() << " ";
                        // for (int i = 0; i <= len_change; i++) {
                        //     cerr << fastpath.dath[i] << " ";
                        // }
                        // cerr << endl;
                        if (cnt % 10000 == 0)
                            cerr << "cnt = " << cnt << endl;
                    }
                }

                cost = fastpath.replace_path(len_change, path_insert, path_insert_len);

                // if (cost._wall_cnt > 0) {
                //     fastpath.cost_bef.print();
                //     cost.print();
                //     exit(0);
                // }

                // const Path path_insert_ver2 = get_random_path_with_length_ver2(p_begin, p_end, path_insert_len);
                // const Path& path = memo_short_path_with_length[p_begin.raw()][p_end.raw()][path_insert_len][randrange(memo_short_path_with_length[p_begin.raw()][p_end.raw()][path_insert_len].size())];
                // get_random_path_with_length_ver2(
                //     p_begin, p_end, path_insert, path_insert_len);
                // if (path_insert_len > 0) {
                //     len_change--;
                //     path_insert_len--;
                // }
                // cost = fastpath.replace_path(len_change, path_insert, path_insert_len);

            } else {
                cerr << "deprecated" << endl;
                assert(false);
                exit(0);
                p_end
                    = p_begin;
                // return -1;
                if (max_length_path_insert < 1) {
                    assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[0].raw()] == 1);
                    return -1;
                }
                get_random_path(p_begin, p_end, path_insert, path_insert_len, max_length_path_insert);
                // path_insert_len = 2;
                // get_random_path_with_length(
                //     p_begin, p_end, path_insert, path_insert_len);
                cost = fastpath.replace_path(len_change, path_insert, path_insert_len);
            }

            assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[fastpath.dath.size() - 2].raw()] == 1);
            if (fastpath.cost._wall_cnt > 0) {
                undo();
                return -1;
            }

            // cerr << len_change_original << " " << len_change << " " << path_insert_len << " " << fastpath.size() << endl;

            if (false) {
                int cnt = 0;
                for (int i = 0; i < fastpath.size(); i++) {
                    int j = i + 1;
                    if (j == fastpath.size())
                        j = 0;
                    if (dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] != 1)
                        cnt++;
                }
                if (cnt >= 1) {
                    fastpath.cost.print();
                    cerr << "size = " << fastpath.size() << endl;
                    cerr << fastpath.dath.back() << " ";
                    for (int i = 0; i <= path_insert_len + 10; i++) {
                        cerr << fastpath.dath[i] << " ";
                    }
                    cerr << endl;
                    for (int i = 0; i < fastpath.size(); i++) {
                        int j = i + 1;
                        if (j == fastpath.size())
                            j = 0;
                        if (dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] != 1)
                            cerr << i << " " << j << " " << fastpath.dath[i] << " " << fastpath.dath[j] << " " << dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] << endl;
                    }
                    assert(false);
                }
            }
            assert(fastpath.cost == calc_path_cost(fastpath.get_path()));

            assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[0].raw()] == 1);
            return len_change_original;
        } else if (neighbor_type == 1) {
            int d = N * 3;
            // int d = 30;
            len_change = randrange(2, d);
            Point p_end = fastpath.dath[len_change];
            if (p_begin == p_end)
                return -1;
            Cost cost_bef = fastpath.cost;
            cost = fastpath.replace_path_with_best_shortest_path(len_change);


            // if (len_change + 1 == dist_flatten[p_begin.raw()][p_end.raw()]) {
            //     // cerr << "len same" << endl;
            //     // assert((cost == cost_bef) || (cost < cost_bef));
            //     if (cost_bef < cost) {
            //         cost_bef.print();
            //         cost.print();
            //         cerr << "neighbor type " << neighbor_type << " broken" << endl;
            //         // exit(0);
            //         assert(false);
            //     }
            // }

            if (false) {
                assert(fastpath.cost == calc_path_cost(fastpath.get_path()));
                // fastpath.undo();
                // assert(cost_bef == fastpath.cost);
            }

            // assert(check_path(fastpath.get_path()));

            // cost.print();
            assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[fastpath.dath.size() - 2].raw()] == 1);

            if (false) {
                int cnt = 0;
                for (int i = 0; i < fastpath.size(); i++) {
                    int j = i + 1;
                    if (j == fastpath.size())
                        j = 0;
                    if (dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] != 1)
                        cnt++;
                }
                if (cnt >= 1) {
                    fastpath.cost.print();
                    cerr << len_change << endl;
                    cerr << "size = " << fastpath.size() << endl;
                    cerr << fastpath.dath.back() << " ";
                    for (int i = 0; i <= path_insert_len + 10; i++) {
                        cerr << fastpath.dath[i] << " ";
                    }
                    cerr << endl;
                    for (int i = 0; i < fastpath.size(); i++) {
                        int j = i + 1;
                        if (j == fastpath.size())
                            j = 0;
                        if (dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] != 1)
                            cerr << i << " " << j << " " << fastpath.dath[i] << " " << fastpath.dath[j] << " " << dist_flatten[fastpath.dath[i].raw()][fastpath.dath[j].raw()] << endl;
                    }
                    assert(false);
                }
            }


        } else if (neighbor_type == 2) {
            len_change = randrange(1, min((int)fastpath.size() / 2, 200));

            // OLD
            if (0) {
                p_end = fastpath.dath[len_change - 1];
                Path path_insert;  // TODO
                path_insert = calc_shortest_path(p_begin, p_end);
                // cerr << p_begin << " " << p_end << endl;
                // for (int i = 0; i < (int)path_insert.size(); i++) {
                //     cerr << path_insert[i] << " ";
                // }
                // cerr << endl;
                // if (path_insert.size() > 0 && Point(path_insert.back()) != p_end)
                //     exit(0);
                // assert(path_insert.back() == p_end);
                // if (path_insert.size() > 0) {
                //     // assert(p_end == Point(path_insert.back()));
                //     len_change--;
                //     path_insert.pop_back();
                // }
                // cerr << path_insert.size() << endl;
                // if (path_insert.size() > 0)
                //     cerr << p_begin << " " << path_insert[0] << " " << path_insert.back() << " " << p_end << " " << len_change << " " << path_insert.size() << endl;
                cost = fastpath.replace_path(len_change, path_insert, path_insert.size());
            }

            // NEW
            if (1)
                cost = fastpath.replace_path_with_shortest_path_simple(len_change);

            // cerr << fastpath.dath.back() << " ";
            // for (int i = 0; i <= path_insert.size(); i++) {
            //     cerr << fastpath.dath[i] << " ";
            // }
            // cerr << endl;

            // assert(check_path(fastpath.get_path()));

            // cost = fastpath.replace_path_with_shortest_path(len_change);
        } else if (neighbor_type == 3) {
            // static Path path_insert(NN);
            // int path_insert_len;

            int len_change_max = fastpath.size() / 3;
            // chmin(len_change_max, int(fastpath.size() / 2));
            len_change = randrange(1, len_change_max);
            Point point_mid = Point(randrange(NN));
            Point point_end = fastpath.dath[len_change];
            // cerr << p_begin << " " << point_mid << " " << point_end << " " << len_change << endl;
            Path path1 = calc_shortest_path(p_begin, point_mid);
            Path path2 = calc_shortest_path(point_mid, point_end);
            // concat
            path1.insert(path1.end(), path2.begin(), path2.end());
            path_insert_len = (int)path1.size();
            if (path1.size() == 0)
                len_change += 1;
            else
                path_insert_len--;
            // cerr << path1.size() << endl;
            // cerr << fastpath.dath.back() << " ";
            // for (const auto& p : path1) {
            //     cerr << p << " ";
            // }
            // cerr << endl;
            // cerr << fastpath.dath[len_change] << endl;
            // for (int i = 0; i < (int)path1.size() - 1; i++) {
            //     path1[i].get_dir(path1[i + 1]);
            // }
            cost = fastpath.replace_path(len_change, path1, path_insert_len);
        } else if (neighbor_type == 4) {
            // reverse
            if (fastpath.durations[p_begin.raw()].size() == 1)
                return -1;
            // int n_max = N;
            int idx = fastpath.next_idx();
            Point p_end = fastpath.dath[idx];
            // cerr << idx << " " << p_begin << " " << p_end << endl;
            assert(p_begin == p_end);
            // if (idx > n_max)
            //     return;
            // Path path_insert = Path(fastpath.dath.begin(), fastpath.dath.begin() + idx);
            Path path_insert(idx);
            for (int i = 0; i < idx; i++) {
                path_insert[i] = fastpath.dath[i];
            }
            cost = fastpath.replace_path(idx, path_insert, path_insert.size());
        } else if (neighbor_type == 5) {
            // return -1;
            // TODO 反転も
            do {
                len_change = randrange(1, 20 + 1);
                p_end = fastpath.dath[len_change];
            } while (p_begin == p_end);
            Path path_insert = calc_random_path(p_begin, p_end, 0.5);
            int path_insert_len = path_insert.size();
            fastpath.replace_path(len_change, path_insert, path_insert_len);
        } else if (neighbor_type == 6) {
            len_change = 2;
            path_insert_len = 0;
            fastpath.replace_path(len_change, path_insert, path_insert_len);
        } else if (neighbor_type == 7) {
            // 次の連鎖と一部交換 ← 交換はコストが高いのでそのまま代入に変更
            if (fastpath.durations[p_begin.raw()].size() <= 1)
                return -2;
            const int len_change = randrange(1, 50);
            const Point p_end = fastpath.dath[len_change];
            if (p_begin == p_end)
                return -2;
            if (fastpath.durations[p_end.raw()].size() <= 1)
                return -2;
            const int p_begin_next_idx = fastpath.next_idx();
            int p_end_next_idx = fastpath.next_idx(p_end);
            // cerr << p_begin << " " << p_end << " " << p_begin_next_idx << " " << fastpath.dath[p_begin_next_idx] << endl;
            for (int i = 0; i < (int)fastpath.durations[p_end.raw()].size() - 1; i++) {
                // cerr << p_end_next_idx << " " << fastpath.dath[p_end_next_idx] << endl;
                if (p_begin_next_idx < p_end_next_idx)
                    break;
                p_end_next_idx += fastpath.durations[p_end.raw()][i];
            }
            if (p_begin_next_idx + 50 < p_end_next_idx || p_end_next_idx >= fastpath.size() || p_begin_next_idx > p_end_next_idx)
                return -2;
            // cerr << -1 << " " << len_change << " " << p_begin_next_idx << " " << p_end_next_idx << endl;

            // if (fastpath.dath[p_end_next_idx] != p_end) {
            //     cerr << "wrong" << endl;
            //     cerr << p_end << " " << p_end_next_idx << " " << fastpath.dath[p_end_next_idx] << endl;
            // }

            // return -2;

            // Path path_insert(p_end_next_idx - p_begin_next_idx - 1);
            for (int i = 0; i < p_end_next_idx - p_begin_next_idx - 1; i++) {
                path_insert[i] = fastpath.dath[p_begin_next_idx + i + 1];
            }
            // cerr << "before : " << p_begin;
            // for (int i = 0; i < len_change + 1; i++) {
            //     cerr << " " << fastpath.dath[i];
            // }
            // cerr << endl;
            cost = fastpath.replace_path(len_change, path_insert, p_end_next_idx - p_begin_next_idx - 1);
            // cerr << "after : " << p_begin;
            // for (int i = 0; i < path_insert.size() + 1; i++) {
            //     cerr << " " << fastpath.dath[i];
            // }
            // cerr << endl;
        } else if (neighbor_type == 8) {
            // p_begin->p_end->...->p_end->p_begin があったときに前者を後者で置き換える
            if (fastpath.durations[p_begin.raw()].size() <= 1)
                return -2;
            const int len_change = randrange(1, 50);
            const Point p_end = fastpath.dath[len_change];
            if (p_begin == p_end)
                return -2;
            if (fastpath.durations[p_end.raw()].size() <= 1)
                return -2;
            int p_end_next_idx = fastpath.next_idx(p_end) + fastpath.durations[p_end.raw()][0];
            int p_begin_next_idx = fastpath.next_idx();
            // cerr << p_begin << " " << p_end << " " << p_begin_next_idx << " " << fastpath.dath[p_begin_next_idx] << endl;
            for (int i = 0; i < (int)fastpath.durations[p_begin.raw()].size() - 1; i++) {
                // cerr << p_end_next_idx << " " << fastpath.dath[p_end_next_idx] << endl;
                if (p_end_next_idx < p_begin_next_idx)
                    break;
                p_begin_next_idx += fastpath.durations[p_begin.raw()][i];
            }

            if (p_end_next_idx < len_change || p_end_next_idx + 50 < p_begin_next_idx || p_end_next_idx >= fastpath.size())
                return -2;
            // cerr << -1 << " " << len_change << " " << p_begin_next_idx << " " << p_end_next_idx << endl;
            // cerr << p_begin << " " << p_end << " " << fastpath.dath[p_begin_next_idx] << " " << fastpath.dath[p_end_next_idx] << endl;

            // for (int i = p_end_next_idx; i <= p_begin_next_idx; i++) {
            //     cerr << fastpath.dath[i] << " ";
            // }
            // cerr << endl;


            // if (fastpath.dath[p_end_next_idx] != p_end) {
            //     cerr << "wrong" << endl;
            //     cerr << p_end << " " << p_end_next_idx << " " << fastpath.dath[p_end_next_idx] << endl;
            // }

            // return -2;

            // Path path_insert(p_begin_next_idx - p_end_next_idx - 1);
            // cerr << p_begin << " ";
            for (int i = 0; i < p_begin_next_idx - p_end_next_idx - 1; i++) {
                path_insert[i] = fastpath.dath[p_begin_next_idx - i - 1];
                // cerr << path_insert[i] << " ";
            }
            // cerr << p_end << endl;
            // cerr << endl;
            // cerr << "before : " << p_begin;
            // for (int i = 0; i < len_change + 1; i++) {
            //     cerr << " " << fastpath.dath[i];
            // }
            // cerr << endl;
            cost = fastpath.replace_path(len_change, path_insert, p_begin_next_idx - p_end_next_idx - 1);
            // cerr << "after : " << p_begin;
            // for (int i = 0; i < path_insert.size() + 1; i++) {
            //     cerr << " " << fastpath.dath[i];
            // }
            // cerr << endl;

        } else {
            assert(false);
        }
        // cerr << "neighbor type = " << neighbor_type << endl;
        assert(dist_flatten[fastpath.dath.back().raw()][fastpath.dath[0].raw()] == 1);
        cost = fastpath.cost;
        return -2;
    }
    void undo() {
        // assert(fastpath.cost == calc_path_cost(fastpath.get_path()));
        cost = fastpath.replace_path_undo();
        // assert(fastpath.cost == calc_path_cost(fastpath.get_path()));
    }
    Path get_path_best() {
        return path_best;
    }
};

Path calc_shortest_path(Point p_begin, const Point& p_end) {
    // (p_begin, p_end]
    Path path;
    // int x_begin = p_begin.x(), y_begin = p_begin.y();
    // int x_end = p_end.x(), y_end = p_end.y();
    while (p_begin != p_end) {
        // cerr << dist_flatten[p_begin.raw()][p_end.raw()] << " " << p_begin << " " << p_end << " "
        //      << shortest_path_next_point_memo[p_begin.raw()][p_end.raw()].size() << endl;
        p_begin = get_random_shortest_path_next_point(p_begin, p_end);
        path.emplace_back(p_begin);
    }
    return path;
}
Path calc_random_path(Point p_begin, const Point& p_end, const double& prob_rand) {
    Path path;
    while (p_begin != p_end) {
        if (randbool(prob_rand)) {
            p_begin = get_random_next_point(p_begin);
        } else {
            p_begin = get_random_shortest_path_next_point(p_begin, p_end);
        }
        // cerr << p_begin << endl;
        path.emplace_back(p_begin);
    }
    return path;
}

// ---------------------------------------- initial solution ----------------------------------------


namespace initial_solution {
vector<vector<int>> visited;
void dfs(Point p_now, Path& path, int& not_visited_cnt) {
    int x = p_now.x(), y = p_now.y();
    visited[x][y] = 1;
    not_visited_cnt--;
    path.push_back(p_now);
    for (int dir = 0; dir < 4; dir++) {
        if (!p_now.can_move_withouthittingwall(dir))
            continue;
        Point p_nxt = p_now.move(dir);
        int x_nxt = p_nxt.x(), y_nxt = p_nxt.y();
        if (visited[x_nxt][y_nxt])
            continue;
        dfs(p_nxt, path, not_visited_cnt);
        if (not_visited_cnt == 0)
            return;
        path.push_back(p_now);
    }
}
Path solve(int x = 0, int y = 0) {
    visited.resize(0);
    visited.resize(N, vector<int>(N, 0));
    Path path;
    int not_visited_cnt = N * N;
    dfs(Point(x, y), path, not_visited_cnt);
    Path path_remain = calc_shortest_path(path.back(), *path.begin());
    if (path_remain.size() >= 2) {
        copy(path_remain.begin(), path_remain.end() - 1, back_inserter(path));
    }
    cerr << (int)path_remain.size() << endl;
    return path;
}
Path solve4() {
    bool flag = false;
    Path path;
    Cost cost;
    vector<Point> points_begin{{0, 0}, {0, N - 1}, {N - 1, 0}, {N - 1, N - 1}};
    // vector<Point> points_begin{{N - 1, 0}};
    for (auto&& p : points_begin) {
        int x = p.x(), y = p.y();
        Path path_new = solve(x, y);
        // for (auto&& p : path_new) {
        //     cerr << p.x() << " " << p.y() << endl;
        // }

        Cost cost_new = calc_path_cost(path_new);
        if (!flag || cost_new < cost) {
            flag = true;
            path = path_new;
            cost = cost_new;
        }
    }
    cerr << "finish initial solution" << endl;
    return path;
}
}  // namespace initial_solution


namespace initial_solution2 {
Path solve() {
    Path path;
    Path path_root;
    // vector<int> D_thresholds = {100, 50, 25, 0};
    // vector<int> D_thresholds = {50, 50, 25, 25, 0, 0};
    // vector<int> D_thresholds = {50, 50, 25, 25, 0, 0};
    // vector<int> D_thresholds = {100, 0};

    int n_d = 3;
    vector<int> D_thresholds;
    // int D_max = *max_element(D_flatten, D_flatten + NN);
    int D_max = accumulate(D_flatten, D_flatten + NN, 0ll) / NN;
    for (int i = 0; i < n_d; i++) {
        D_thresholds.push_back((D_max / 2) * (n_d - 1 - i) / (n_d - 1));
    }

    for (const auto& D_threshold : D_thresholds) {
        Path points_use;
        for (int i = 0; i < NN; i++) {
            if (D_flatten[i] >= D_threshold)
                points_use.emplace_back(Point(i));
        }
        if (points_use.size() <= 1)
            continue;
        // path.insert(path.end(), path.begin(), path.end());
        Path path_tsp = tsp_greedy(points_use);
        if (path.size() == 0) {
            path_root = path_tsp;
            path = path_tsp;
        } else {
            // Path path_tmp = path_root;
            // concat_paths(path_tmp, path_tsp);
            // concat_paths(path, path_tmp);
            concat_paths(path, path_tsp);
        }
    }
    path = complement_path(path);
    return path;
}
}  // namespace initial_solution2


namespace initial_solution3 {
Path solve() {
    // int n_d = 4;
    // vector<int> D_thresholds = {100, 50, 25, 0};
    // vector<int> D_thresholds = {50, 50, 25, 25, 0, 0};
    // vector<int> D_thresholds = {50, 50, 25, 25, 0, 0};
    // vector<int> D_thresholds = {100, 0};

    int n_d = 3;
    vector<int> D_thresholds;
    // int D_max = (*max_element(D_flatten, D_flatten + NN)) * 0.8;
    int D_max = accumulate(D_flatten, D_flatten + NN, 0ll) / NN;
    // D_max = 100;
    for (int i = 0; i < n_d; i++) {
        if (i == n_d - 1)
            D_thresholds.emplace_back(0);
        else
            D_thresholds.emplace_back(D_max);
        D_max /= 2;
    }

    sort(D_thresholds.begin(), D_thresholds.end(), greater<int>());

    vector<vector<Point>> points_use(n_d);
    for (int i = 0; i < NN; i++) {
        int d = D_flatten[i];
        for (int j = 0; j < n_d; j++) {
            if (d >= D_thresholds[j]) {
                points_use[j].emplace_back(Point(i));
            }
        }
    }
    for (int i = 0; i < n_d; i++) {
        // sort
        sort(points_use[i].begin(), points_use[i].end(), [&](const Point& p1, const Point& p2) {
            return D_flatten[p1.raw()] > D_flatten[p2.raw()];
        });
    }

    vector<Path> paths(1);

    for (int i = 0; i < n_d; i++) {
        if (i == 0) {
            paths[0] = points_use[0];
        } else {
            int n = paths.size();
            // for (int k = 0; k < 10; k++) {
            for (int j = 0; j < n; j++) {
                paths.emplace_back(paths[j]);
            }
            // }
            n = paths.size();

            for (auto point : points_use[i]) {
                int path_idx = -1;
                int dist_min = INF, size_min = INF;
                int n = paths.size();
                for (int j = 0; j < n; j++) {
                    int dist_tmp = INF;
                    for (auto point_tmp : paths[j]) {
                        chmin(dist_tmp, dist_flatten[point.raw()][point_tmp.raw()]);
                    }
                    if (dist_tmp < dist_min) {
                        dist_min = dist_tmp;
                        size_min = paths[j].size();
                        path_idx = j;
                    } else if (dist_tmp == dist_min) {
                        if (paths[j].size() < size_min) {
                            size_min = paths[j].size();
                            path_idx = j;
                        }
                    }
                }
                paths[path_idx].emplace_back(point);
            }
        }
    }

    for (auto& path : paths) {
        path = tsp_greedy(path);
    }
    for (int i = 1; i < paths.size(); i++) {
        concat_paths(paths[0], paths[i]);
    }
    Path path = paths[0];
    path = complement_path(path);
    return path;
}
}  // namespace initial_solution3


// ---------------------------------------- sample ----------------------------------------
namespace sample {
const int MAX_N = 50;
vector<string> h(MAX_N - 1), v(MAX_N);
vector<vector<int>> d(MAX_N, vector<int>(MAX_N, 0));
vector<vector<bool>> visited(MAX_N, vector<bool>(MAX_N, false));

pair<int, int> DIJ[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
string DIR = "RDLU";

void dfs(int i, int j, string& ans) {
    visited[i][j] = true;
    for (int dir = 0; dir < 4; ++dir) {
        int di = DIJ[dir].first;
        int dj = DIJ[dir].second;
        int i2 = i + di;
        int j2 = j + dj;
        if (0 <= i2 && i2 < N && 0 <= j2 && j2 < N && !visited[i2][j2]) {
            if ((di == 0 && v[i][min(j, j2)] == '0') || (dj == 0 && h[min(i, i2)][j] == '0')) {
                // cout << DIR[dir];
                ans += DIR[dir];
                dfs(i2, j2, ans);
                // cout << DIR[(dir + 2) % 4];
                ans += DIR[(dir + 2) % 4];
            }
        }
    }
}

int calc_score(string ans) {
    int memo[N_MAX][N_MAX] = {};
    int x = 0, y = 0;
    int len = (int)ans.size();
    auto f = [&](int x, int y) {
        for (int xx = 0; xx < N; xx++) {
            for (int yy = 0; yy < N; yy++) {
                if ((xx == x) && (yy == y))
                    continue;
                memo[xx][yy] += d[xx][yy];
            }
        }
        return;
    };
    for (int i = 0; i < len; ++i) {
        char c = ans[i];
        if (c == 'R') {
            y++;
        } else if (c == 'D') {
            x++;
        } else if (c == 'L') {
            y--;
        } else if (c == 'U') {
            x--;
        }
        f(x, y);
        memo[x][y] = 0;
    }
    assert((x == 0) && (y == 0));
    ll ret = 0;
    for (int i = 0; i < len; ++i) {
        char c = ans[i];
        if (c == 'R') {
            y++;
        } else if (c == 'D') {
            x++;
        } else if (c == 'L') {
            y--;
        } else if (c == 'U') {
            x--;
        }
        f(x, y);
        memo[x][y] = 0;

        for (int xx = 0; xx < N; xx++) {
            for (int yy = 0; yy < N; yy++) {
                ret += memo[xx][yy];
            }
        }
    }
    return ret / len;
}

vector<Point> solve(bool flag_print = false) {
    for (int i = 0; i < N - 1; ++i) {
        for (int j = 0; j < N; j++) {
            h[i] += '0' + H[i][j];
        }
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N - 1; j++) {
            v[i] += '0' + V[i][j];
        }
    }
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            d[i][j] = D[i][j];
        }
    }
    string ans;
    dfs(0, 0, ans);

    if (flag_print) {
        cout << ans << endl;
        int score = calc_score(ans);
        print_data("score", score);
    }

    int len = (int)ans.size();
    vector<Point> path(len);
    Point p(0, 0);
    for (int i = 0; i < len; i++) {
        path[i] = p;
        if (ans[i] == 'R') {
            p += dpoint[0];
        } else if (ans[i] == 'D') {
            p += dpoint[1];
        } else if (ans[i] == 'L') {
            p += dpoint[2];
        } else if (ans[i] == 'U') {
            p += dpoint[3];
        }
    }
    return path;
}
}  // namespace sample

// ---------------------------------------- main ----------------------------------------

void init_dist() {
    Fill(dist, INF);
    // dist.resize(N, vector(N, vector(N, vector(N, INF))));
    // dist_flatten.resize(NN, vector(NN, INF));

    MyDeque<Point> que;
    for (int x1 = 0; x1 < N; x1++) {
        for (int y1 = 0; y1 < N; y1++) {
            // bfs search
            Point p_begin(x1, y1);
            // queue<Point> que;
            // que.emplace(p_begin);
            que.clear();
            que.emplace_back(p_begin);
            dist[x1][y1][x1][y1] = 0;
            while (!que.empty()) {
                auto p_now = que.front();
                // que.pop();
                que.pop_front();
                int x_now = p_now.x();
                int y_now = p_now.y();
                for (int dir = 0; dir < 4; dir++) {
                    if (!can_move_withouthittingwall_memo[x_now][y_now][dir])
                        continue;
                    Point p_nxt = p_now.move(dir);
                    int x_nxt = p_nxt.x();
                    int y_nxt = p_nxt.y();
                    if (dist[x1][y1][x_nxt][y_nxt] != INF)
                        continue;
                    dist[x1][y1][x_nxt][y_nxt] = dist[x1][y1][x_now][y_now] + 1;
                    // que.emplace(p_nxt);
                    que.emplace_back(p_nxt);
                }
            }
        }
    }
    for (int x1 = 0; x1 < N; x1++) {
        for (int y1 = 0; y1 < N; y1++) {
            const int idx1 = Point(x1, y1).raw();
            for (int x2 = 0; x2 < N; x2++) {
                for (int y2 = 0; y2 < N; y2++) {
                    const int idx2 = Point(x2, y2).raw();
                    dist_flatten[idx1][idx2] = dist[x1][y1][x2][y2];
                    // dist_l1_flatten[idx1][idx2] = abs(x1 - x2) + abs(y1 - y2);
                }
            }
            // cerr << dist[x1][y1][0][0] << " ";
        }
        // cerr << endl;
    }
    // exit(0);

    // shortest_path_next_point_memo.resize(NN, vector<vector<Point>>(NN));
    // shortest_path_next_point_idx_memo.resize(NN, vector<int>(NN));
    for (int x1 = 0; x1 < N; x1++) {
        for (int y1 = 0; y1 < N; y1++) {
            for (int x2 = 0; x2 < N; x2++) {
                for (int y2 = 0; y2 < N; y2++) {
                    Point p1 = Point(x1, y1);
                    Point p2 = Point(x2, y2);
                    for (int dir = 0; dir < 4; dir++) {
                        if (!p2.can_move_withouthittingwall(dir))
                            continue;
                        if (dist_flatten[p1.raw()][p2.move(dir).raw()] == dist_flatten[p1.raw()][p2.raw()] - 1) {
                            // shortest_path_next_point_memo[p1.raw()][p2.raw()].emplace_back(p2.move(dir));
                            shortest_path_next_point_memo[p1.raw()][p2.raw()][shortest_path_next_point_memo_size[p1.raw()][p2.raw()]++] = p2.move(dir);
                        }
                    }
                    // if (p1 != p2)
                    //     assert(!shortest_path_next_point_memo[p1.raw()][p2.raw()].empty());
                }
            }
        }
    }
    // next_point_memo.resize(NN);
    for (int x1 = 0; x1 < N; x1++) {
        for (int y1 = 0; y1 < N; y1++) {
            Point p1 = Point(x1, y1);
            for (int dir = 0; dir < 4; dir++) {
                if (!p1.can_move_withouthittingwall(dir))
                    continue;
                Point p2 = p1.move(dir);
                // next_point_memo[p1.raw()].emplace_back(p2);
                next_point_memo[p1.raw()][next_point_memo_size[p1.raw()]++] = p2;
            }
        }
    }


    // for (int x = 0; x < N; x++) {
    //     for (int y = 0; y < N; y++) {
    //         int d = dist[0][0][x][y];
    //         cerr << d << " ";
    //     }
    //     cerr << endl;
    // }
}

namespace init_move_cost {
using PointOriginal::Point;
bool on_border(int x, int y) {
    return x == 0 || x == N || y == 0 || y == N;
}
bool on_border(Point p) {
    return on_border(p.x(), p.y());
}
void init_move_cost() {
    using Wall = pair<Point, Point>;
    vector<Wall> walls;
    for (int x = 0; x < N_MAX - 1; x++) {
        for (int y = 0; y < N_MAX; y++) {
            if (H[x][y]) {
                Point p = Point(x + 1, y);
                Point q = Point(x + 1, y + 1);
                walls.emplace_back(p, q);
            }
        }
    }
    for (int x = 0; x < N_MAX; x++) {
        for (int y = 0; y < N_MAX - 1; y++) {
            if (V[x][y]) {
                Point p = Point(x, y + 1);
                Point q = Point(x + 1, y + 1);
                walls.emplace_back(p, q);
            }
        }
    }
    vector<vector<vector<int>>> connects(N_MAX + 1, vector<vector<int>>(N_MAX + 1));
    int n = (int)walls.size();
    for (int i = 0; i < n; i++) {
        auto [p, q] = walls[i];
        for (auto pp : {p, q}) {
            int x = pp.x(), y = pp.y();
            connects[x][y].emplace_back(i);
        }
    }

    // for (auto& w : walls) {
    //     cerr << w.first << " " << w.second << endl;
    // }

    UnionFind uf(n + 1);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> que;
    vector<vector<int>> seen_grid_cnt(N_MAX + 1, vector<int>(N_MAX + 1, 0));
    vector<int> seen(n, 0);
    vector<int> cost_memo(n, INF);
    for (int i = 0; i < n; i++) {
        Wall wall = walls[i];
        bool flag = false;
        bool flag_on_border = false;
        for (auto p : {wall.first, wall.second}) {
            if (on_border(p))
                flag_on_border = true;
            if (!on_border(p) && connects[p.x()][p.y()].size() == 1) {
                flag = true;
                break;
            }
        }
        if (flag) {
            cost_memo[i] = 1;
            que.emplace(1, i);
            // for (auto p : {wall.first, wall.second}) {
            //     seen_grid_cnt[p.x()][p.y()]++;
            // }
        }
        if (flag_on_border) {
            uf.unite(i, n);
        }
    }

    for (int x = 0; x <= N; x++) {
        for (int y = 0; y <= N; y++) {
            for (int m = 1; m < (int)connects[x][y].size(); m++) {
                int idx1 = connects[x][y][m - 1];
                int idx2 = connects[x][y][m];
                uf.unite(idx1, idx2);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (uf.same(i, n)) {
            seen[i] = 1;
            cost_memo[i] = 1000000;
        }
    }

    while (!que.empty()) {
        auto [cost, idx] = que.top();
        que.pop();
        if (seen[idx])
            continue;
        seen[idx] = 1;
        Wall wall = walls[idx];
        // cerr << wall.first << " " << wall.second << " " << cost << endl;
        for (auto p : {wall.first, wall.second}) {
            int x = p.x(), y = p.y();
            seen_grid_cnt[x][y]++;
            if (on_border(p))
                continue;
            if (seen_grid_cnt[x][y] + 1 != (int)connects[x][y].size())
                continue;
            int nxt_idx = -1;
            int nxt_cost = 1;
            for (auto idx2 : connects[x][y]) {
                if (!seen[idx2]) {
                    assert(nxt_idx == -1);
                    nxt_idx = idx2;
                } else {
                    nxt_cost += cost_memo[idx2];
                }
            }
            assert(nxt_idx != -1);
            if (chmin(cost_memo[nxt_idx], nxt_cost)) {
                que.emplace(nxt_cost, nxt_idx);
            }
        }
    }

    for (int i = 0; i < n; i++) {
        Wall wall = walls[i];
        auto [p, q] = wall;
        int x = p.x(), y = p.y();
        if (p.x() == q.x()) {
            move_cost_memo[x - 1][y][1] = cost_memo[i];
            move_cost_memo[x][y][3] = cost_memo[i];
            assert(!can_move_withouthittingwall_memo[x - 1][y][1]);
            assert(!can_move_withouthittingwall_memo[x][y][3]);
        } else {
            move_cost_memo[x][y - 1][0] = cost_memo[i];
            move_cost_memo[x][y][2] = cost_memo[i];
            assert(!can_move_withouthittingwall_memo[x][y - 1][0]);
            assert(!can_move_withouthittingwall_memo[x][y][2]);
        }
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int dir = 0; dir < 4; dir++) {
                move_cost_memo_flatten[xy2idx[x][y]][dir] = move_cost_memo[x][y][dir];
                int nx = x + dx[dir], ny = y + dy[dir];
                if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                    continue;
                move_cost_memo_flatten2[xy2idx[x][y]][xy2idx[nx][ny]] = move_cost_memo[x][y][dir];
            }
        }
    }
}
}  // namespace init_move_cost

void init() {
    for (int i = 0; i < MYDEQUE_MAXSIZE; i++) {
        increment_memo[i] = (i == MYDEQUE_MAXSIZE - 1 ? 0 : i + 1);
        decrement_memo[i] = (i == 0 ? MYDEQUE_MAXSIZE - 1 : i - 1);
    }

    randxor_init();
    log_init();
    NN = N * N;

    for (int i = 0; i < 10010; i++) {
        calc_sum_tri_memo[i] = calc_sum_tri_naive(i);
    }

    for (int dir = 0; dir < 4; dir++) {
        int x = dx[dir], y = dy[dir];
        dpoint[dir] = xy2idx_func(x, y);
        dpoint_raw[dir] = xy2idx_func(x, y);
    }

    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            int idx = xy2idx_func(x, y);
            idx2x[idx] = x;
            idx2y[idx] = y;
            xy2idx[x][y] = idx;
            D_flatten[idx] = D[x][y];
        }
    }


    get_dir_memo.resize(NN, vector<int>(NN, -1));
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int dir = 0; dir < 4; dir++) {
                int nx = x + dx[dir], ny = y + dy[dir];
                if (nx < 0 || nx >= N || ny < 0 || ny >= N)
                    continue;
                Point p = Point(x, y);
                Point q = Point(nx, ny);
                // cerr << p.raw() << " " << q.raw() << endl;
                get_dir_memo[p.raw()][q.raw()] = dir;
            }
        }
    }

    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            for (int dir = 0; dir < 4; dir++) {
                // can_move_memo[x][y][dir] = Point(x, y).can_move_naive(dir);
                can_move_inside_memo[x][y][dir] = Point(x, y).can_move_inside_naive(dir);
                can_move_withouthittingwall_memo[x][y][dir] = Point(x, y).can_move_withouthittingwall_naive(dir);
                int idx = xy2idx[x][y];
                can_move_inside_memo_flatten[idx][dir] = can_move_inside_memo[x][y][dir];
                can_move_withouthittingwall_memo_flatten[idx][dir] = can_move_withouthittingwall_memo[x][y][dir];
            }
        }
    }

    print_time();
    init_memo_short_path();
    print_time();
    init_dist();
    init_move_cost::init_move_cost();
}


void read_input(istream& in = cin) {
    cerr << "read input" << endl;
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    in >> N;
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N; j++) {
            char c;
            in >> c;
            H[i][j] = c == '1';
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 1; j++) {
            char c;
            in >> c;
            V[i][j] = c == '1';
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            in >> D[i][j];

            // D[i][j] /= 10;
            // double alpha = 2;
            // D[i][j] = (int)pow(D[i][j], alpha);

            // if (D[i][j] < 20)
            // D[i][j] = 0;
            // D[i][j] /= 2;
        }
    }
}

bool read_from_file(int argc, char* argv[]) {
    if (argc == 2) {
        string filename = argv[1];
        ifstream ifs(filename);
        if (ifs.is_open()) {
            cerr << "read from file" << endl;
            read_input(ifs);
            return true;
        }
    }
    return false;
}

void test() {
    vector<Point> path = sample::solve(false);
    for (int i = 0; i < 0; i++) {
        int len = (int)path.size();
        for (int i = 0; i < len; i++) {
            path.emplace_back(path[i]);
        }
    }
    assert(check_path(path));

    FastPath fast_path(path);
    Cost cost_fast_path, cost_path;
    auto del = [&]() {
        fast_path.pop_front();
        path = fast_path.get_path();
        cost_fast_path = fast_path.cost;
        cost_path = calc_path_cost(path);
    };
    auto add = [&](int x = -1, int y = -1) {
        if (x == -1)
            x = randrange(N);
        if (y == -1)
            y = randrange(N);
        Point p(x, y);
        fast_path.emplace_front(p);
        path = fast_path.get_path();
        cost_fast_path = fast_path.cost;
        cost_path = calc_path_cost(path);
    };
    auto shift_forward = [&]() {
        fast_path.shift_forward();
        path = fast_path.get_path();
        cost_fast_path = fast_path.cost;
        cost_path = calc_path_cost(path);
    };
    auto print = [&]() {
        cost_fast_path.print();
        cost_path.print();
    };

    // Cost cost_path = calc_path_cost(path, true);
    // cost_path.print();

    // path.pop_front();
    // path.pop_front();

    int n = 10000000;
    // n = (int)path.size() * 3;
    int n_rep = 1;
    // n = 5;

    for (int i = 0; i < n_rep; i++) {
        for (int j = 0; j < n; j++) {
            cerr << "j = " << j << endl;
            int d = randrange(10);
            cerr << d << endl;
            if (d == 0) {
                int x = randrange(N);
                int y = randrange(N);
                add(x, y);
                // add(0, 0);
            } else if (d == 1) {
                if (cost_fast_path._not_visited_cnt > 0)
                    continue;
                // continue;
                if (fast_path.dath.size() > 10)
                    del();
                else
                    continue;
            } else {
                shift_forward();
            }
            print();
            assert(cost_fast_path == cost_path);

            cerr << fast_path.current_durationxD_sum << " " << fast_path.Dsum << " " << fast_path.current_offset_duration << " " << fast_path.current_offset_nextidx << endl;
        }
    }
    return;

    n = 10;
    for (int j = 0; j < n_rep; j++) {
        // cerr << "rotate" << endl;
        // for (int i = 0; i < n; i++) {
        //     cerr << i << endl;
        //     shift_forward();
        //     // print();
        //     // assert(cost_fast_path == cost_path);
        // }
        vector<Point> points_remove;
        cerr << "del" << endl;
        for (int i = 0; i < n; i++) {
            // cerr << i << endl;
            Point p = fast_path.dath.front();
            points_remove.emplace_back(p);
            del();
            print();
            assert(cost_fast_path == cost_path);
        }
        cerr << "rotate" << endl;
        for (int i = 0; i < n; i++) {
            // cerr << i << endl;
            shift_forward();
            print();
            assert(cost_fast_path == cost_path);
        }
        cerr << "add" << endl;
        for (int i = 0; i < n; i++) {
            // cerr << i << endl;
            Point p = points_remove[i];
            int x = p.x(), y = p.y();
            add(x, y);
            print();
            assert(cost_fast_path == cost_path);
        }
        print();
        assert(cost_fast_path == cost_path);
    }
}

void test_speed_fastpath() {
    vector<Point> path = sample::solve(false);
    for (int i = 0; i < 0; i++) {
        int len = (int)path.size();
        for (int i = 0; i < len; i++) {
            path.emplace_back(path[i]);
        }
    }
    assert(check_path(path));

    FastPath fast_path(path);

    cerr << "start recording" << endl;
    int begin = get_time();
    int n = 1000000000;
    int b = 10;
    for (int i = 0; i < n / b; i++) {
        int d = randrange(3);
        if (d == 0) {
            int x = randrange(N);
            int y = randrange(N);
            fast_path.emplace_front(Point(x, y));
        } else if (d == 1) {
            if (fast_path.dath.size() > 10)
                fast_path.pop_front();
            else
                continue;
        } else
            fast_path.shift_forward();
        // for (int j = 0; j < b; j++) {
        //     fast_path.shift_forward();
        // }
        // for (int j = 0; j < b; j++) {
        //     // int x = randrange(N);
        //     // int y = randrange(N);
        //     int x = 0, y = 0;
        //     fast_path.emplace_front({x, y});
        // }
        // for (int j = 0; j < b; j++) {
        //     fast_path.pop_front();
        // }
    }
    int end = get_time();
    cerr << "end recording" << endl;
    int d_time = end - begin;
    cerr << "time = " << d_time / 1000. << endl;
    double fps = 1. * n / d_time * 1000;
    cerr << "fps = " << fps << endl;
}

void test_randbool() {
    return;
    int n = 1e9;
    int cnt = 0;
    int time_begin = get_time();
    for (int i = 0; i < n; i++) {
        if (randbool(0.5))
            cnt++;
    }
    int time_end = get_time();
    cerr << cnt << endl;
    cerr << "time = " << double(time_end - time_begin) / 1000. << endl;
    cnt = 0;
    time_begin = get_time();
    for (int i = 0; i < n; i++) {
        if (randbool_const<0.5f>())
            cnt++;
    }
    time_end = get_time();
    cerr << cnt << endl;
    cerr << "time = " << double(time_end - time_begin) / 1000. << endl;
}

void test_deque() {
    return;
    int n = 1e3;
    int max_size = 2;
    // deque speed
    int time_start = get_time();
    // deque<int> dq;
    MyDeque<int> dq(max_size);
    for (int i = 0; i < n; i++) {
        if ((int)dq.size() == max_size)
            dq.pop_back();
        dq.emplace_front(i);
    }
    int s = 0;
    for (int i = 0; i < max_size; i++) {
        s += dq[i];
    }
    auto tmp = dq.get_data();
    cerr << accumulate(tmp.begin(), tmp.end(), 0) << endl;
    int time_end = get_time();
    cerr << s << endl;
    cerr << "deque time = " << double(time_end - time_start) / 1000. << endl;

    // vector speed
    vector<int> V(max_size);
    int idx_insert = 0;
    int idx_pop = 0;
    int sz = 0;
    time_start = get_time();
    for (int i = 0; i < n; i++) {
        V[idx_insert] = i;
        idx_insert = (idx_insert == max_size - 1) ? 0 : idx_insert + 1;
        if (idx_insert == idx_pop)
            idx_pop = (idx_pop == 0 ? max_size - 1 : idx_pop - 1);
    }
    s = 0;
    for (int i = 0; i < max_size; i++) {
        s += V[i];
    }
    time_end = get_time();
    cerr << s << endl;
    cerr << "vector time = " << double(time_end - time_start) / 1000. << endl;
}

void test_exp() {
    double l = 0;
    double r = 1e12;
    while (abs(l - r) > 1e-8) {
        double mid = (l + r) / 2;
        if (bool(exp(-mid)) != 0)
            l = mid;
        else
            r = mid;
    }
    cerr << l << " " << r << " " << bool(exp(-l)) << " " << bool(exp(-r)) << endl;
}


void solve2() {
    Path path;
    // path = sample::solve(true);
    // return;
    // Path path = initial_solution::solve(N / 2, N / 2);
    path = initial_solution3::solve();
    // Path path = initial_solution::solve(0, 0);
    if (1) {
        // cerr << path2[0] << " " << path2.back() << endl;
        // cerr << path[0] << " " << path.back() << endl;
        int len = (int)path.size();
        // int len = (int)path2.size();
        for (int n = 0; n < 0; n++) {  // originally 1
            for (int i = 0; i < len; i++) {
                path.emplace_back(path[i]);
                // path.emplace_back(path2[i]);
            }
        }
    }
    {
        // print_path_as_ans(path);
        // Cost cost = calc_path_cost(path, true);
        // print_data("score", cost.value_int());
        // print_time();
        // return;
    }

    // assert(check_path(path));
    // Cost cost = calc_path_cost(path, true);
    // cost.print();
    // print_debug("score_tmp", cost.value_int());

    Cost cost;

    Solver_sa_fast_1 solver(path);


    // update_cost_coef(1);
    // solver.hill_climbing(INF);
    // update_cost_coef(0.001);
    // solver.simulated_annealing_reheat(100000, 0);

    // SimulatedAnnealingConfig config1(INF, get_time_remaining(), 1, 1e2, 1e-7);
    // // SimulatedAnnealingConfig config1(INF, get_time_remaining(), 1, 1e1, 1e-7);
    // solver.simulated_annealing_reheat(config1);

    // double temp_mid = 1.0;
    // SimulatedAnnealingConfig config1(INF, get_time_remaining() * 0.5, 0, 1e2, 1e-7);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining(), 0, 1, 1e-7);
    // solver.simulated_annealing_reheat(config1);


    // double temp_mid = 1.0;
    // SimulatedAnnealingConfig config1;
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() * 0.1, 0, 3e2, 1e-7);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() * 0.5, 0, 1e2, 1e-7);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 2, 0, 1, 1e-7);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining(), 0, 0.1, 1e-7);
    // solver.simulated_annealing_reheat(config1);

    // SimulatedAnnealingConfig config1;
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 10, 0, 1e2, 1e1);
    // solver.simulated_annealing_reheat(config1);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 2, 0, 1e1, 1);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining(), 0, 1, 1e-7);
    // solver.simulated_annealing_reheat(config1);


    // // if(prob) のときの最適?
    // SimulatedAnnealingConfig config1;
    // double temp_mid = 10;
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 10, 0, 1e2, temp_mid, false);
    // solver.simulated_annealing_reheat(config1);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 2, 0, temp_mid, 1e-7, false);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining(), 0, 3, 1e-7, false);
    // solver.simulated_annealing_reheat(config1);


    // SimulatedAnnealingConfig config1;
    // double temp_initial = 1e4;
    // double temp_mid = 1e2;
    // double temp_final = 1e0;
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 3, 0, temp_initial, temp_mid, true);
    // solver.simulated_annealing_reheat(config1);
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining() / 2, 0, temp_mid, temp_final, true);
    // solver.simulated_annealing_reheat(config1);
    // solver.reload_best_path();
    // solver.double_path(2);
    // config1 = SimulatedAnnealingConfig(INF, get_time_remaining(), 0, temp_mid, temp_final, true);
    // solver.simulated_annealing_reheat(config1);

    // return;

    // best
    SimulatedAnnealingConfig config1;
    double temp_initial = 6e3;
    double temp_mid = 5e2;
    double temp_final = 1e0;
    // double temp_mid = 5e-13;
    // double temp_final = 3e-13;
    solver.double_path(2);
    config1 = SimulatedAnnealingConfig(INF, get_time_remaining() * 0.5, 0, temp_initial, temp_mid, true);
    solver.simulated_annealing_reheat(config1);
    int n_total = 1;
    for (int i = 0; i < n_total; i++) {
        // if (i != 0)
        solver.reload_best_path();
        double temp_begin = temp_mid + (temp_final - temp_mid) * i / n_total;
        double temp_end = temp_mid + (temp_final - temp_mid) * (i + 1) / n_total;
        solver.double_path(2);
        int t = get_time_remaining() * (i + 1) / n_total;
        // config1 = SimulatedAnnealingConfig(INF, t, 0, temp_mid, temp_final, true);
        config1 = SimulatedAnnealingConfig(INF, t, 0, temp_begin, temp_end, true);
        solver.simulated_annealing_reheat(config1);
    }

    path = solver.get_path_best();
    cost = solver.cost_best;

    cost.print();
    assert(cost == calc_path_cost(path, true));

#ifndef ONLINE_JUDGE
    print_data("score", cost.value_int());
    print_data("N", N);
    print_data("D_ave", accumulate(D_flatten, D_flatten + NN, 0) / NN);
    int wall_cnt = 0;
    for (int x = 0; x < N_MAX - 1; x++) {
        for (int y = 0; y < N_MAX; y++) {
            wall_cnt += H[x][y];
        }
    }
    for (int x = 0; x < N_MAX; x++) {
        for (int y = 0; y < N_MAX - 1; y++) {
            wall_cnt += V[x][y];
        }
    }
    print_data("wall_ratio", 1. * wall_cnt / NN);
#endif
    print_path_as_ans(path);
    print_time();
    // cerr << "n_copy sum = " << n_copy_sum << endl;
}

int main(int argc, char* argv[]) {
    print_time();
    // test_exp();
    // test_deque();
    // test_randbool();

    if (!(read_from_file(argc, argv)))
        read_input();
    print_time();
    init();
    print_time();
    // return 0;

    // test();
    // test_speed_astpath();
    // return 0;

    solve2();
    return 0;
}


/*
TODO
探索を効率化
壁の重みをなめらかに
近傍探索時に壁を考慮して枝刈り, raise acceptance ratio
壁に当たらない近傍を作る
最短経路だとdpで最適経路が求まる
reverse 近傍？

近傍
線の平行移動

高速化
Pointをunsigned short
vector copy
dequeを循環vectorにする クラスを作る
*/

/*
 * 次のコードのどこがおかしいでしょうか？

                if (cost_soft_diff < 0) {
                    prob = 1;
                } else {
                    prob = exp(-cost_soft_diff / temp);
                }

                if (prob) {
*/
