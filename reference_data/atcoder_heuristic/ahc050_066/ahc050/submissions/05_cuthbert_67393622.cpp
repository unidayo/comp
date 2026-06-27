#line 1 "main.cpp"
// =============================================================================
//  Skating Rink Robot – Hybrid Greedy + Hill‑Climb Solver (with early discard)
// =============================================================================
//  * Adds an early‑exit rule: if the cumulative crash probability in the first
//    `prefix_len` steps exceeds `DISCARD_THRESHOLD` (default 1 %), we abandon the
//    evaluation immediately and return.
//  * The threshold is exposed as a tunable constant.
// -----------------------------------------------------------------------------

#ifndef __clang__
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3,inline")
#pragma GCC target("bmi,bmi2,lzcnt,popcnt")
#pragma GCC target("movbe")
#pragma GCC target("aes,pclmul,rdrnd")
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")
#endif

#define PRINT_LOG

#define TIME_LIMIT_INPUT 1985
#define TIME_RATIO_INPUT 1.5

#line 2 "library/include.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
// #include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#line 2 "library/variable.cpp"

#ifdef ONLINE_JUDGE
constexpr double TIME_RATIO = 1.0;
#else
#ifdef TIME_RATIO_INPUT
constexpr double TIME_RATIO = TIME_RATIO_INPUT;
#else
constexpr double TIME_RATIO = 1.0;
#endif
#endif

constexpr int INF = 1e9;
constexpr long long INFLL = 1ll << 60;
#ifdef TIME_LIMIT_INPUT
constexpr int TIME_LIMIT = TIME_LIMIT_INPUT;
#else
constexpr int TIME_LIMIT = 1900;  // 問題固有
#endif
#line 4 "library/util.cpp"

#line 6 "library/util.cpp"

using namespace std;
using std::cin;
using std::cout;
using uint = unsigned int;
using i8 = signed char;
using u8 = unsigned char;
using i16 = signed short;
using u16 = unsigned short;
using i32 = signed int;
using u32 = unsigned int;
using i64 = signed long long;
using u64 = unsigned long long;
using ll = long long;
using ull = unsigned long long;
using pii = pair<int, int>;


namespace ethy {
// print

template <typename T>
inline void print_data(const string& s, const T& t) {
#ifndef ONLINE_JUDGE
    cerr << "[DATA] " << s << " = " << t << endl;
#endif
}
template <typename T>
inline void print_debug(const string& s, const T& t) {
#ifndef ONLINE_JUDGE
    cerr << s << " = " << t << endl;
#endif
}
template <typename T>
inline void print_debug(const T& t) {
#ifndef ONLINE_JUDGE
    cerr << t << endl;
#endif
}
void print_warning(const string& s) {
#ifndef ONLINE_JUDGE
    cerr << "[WARNING] " << s << endl;
#endif
}
bool print_if_false(const bool assertion, const char* message) {
    if (!assertion) {
        cout << message << endl;
    }
    return assertion;
}


// basic
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
void fill_array(A (&array)[N], const T& val) {
    std::fill((T*)array, (T*)(array + N), val);
}
template <typename T>
int select_from_hash_u32_unsafe(const uint& hash, const T& n) {
    return int(ull(hash) * n >> 32);
}
template <typename T>
int select_from_hash_u32_safe(const uint& hash, const T& n) {
    return hash % n;
}
struct SelectFromHashU32Unsafe{
    template <typename T>
    int operator()(const uint& hash, const T& n){return int(ull(hash) * n >> 32);}
};
struct SelectFromHashU32Safe{
    template <typename T>
    int operator()(const uint& hash, const T& n){return hash % n;}
};
template <typename T, typename U>
void inc_cyclic(T& x, const U& n) {
    if (++x == n)
        x = 0;
}
template <int N, class T>
void inc_cyclic(T& x) {
    if (++x == N)
        x = 0;
}
template <typename T, typename U>
void dec_cyclic(T& x, const U& n) {
    if (--x == -1)
        x = n - 1;
}
template <int N, class T>
void dec_cyclic(T& x) {
    if (--x == -1)
        x = N - 1;
}

// time
auto time_start = chrono::system_clock::now();
int get_time() {
    int elapsed
        = (int)(TIME_RATIO
                * double(
                    chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - time_start).count()));
    return elapsed;
}
int get_time_remaining() {
    int time_remaining = TIME_LIMIT - get_time();
    return time_remaining;
}
double get_time_remaining_ratio() { return (double)get_time_remaining() / TIME_LIMIT; }
void print_time() {
    int elapsed = get_time();
    cerr << "[DATA] totaltime = " << elapsed << endl;
}

// random
class RandomGeneratorBase {
public:
    virtual int int_random() = 0;
    virtual uint uint_random() = 0;
    inline double double_random() { return (uint_random() + 1.0) * 2.328306435454494e-10; }
    inline int range_random_safe(const int& r) { return uint_random() % r; }
    inline int range_random_safe(const int& l, const int& r) { return l + range_random_safe(r - l); }
    inline int range_random(const int& r) { return (ull(uint_random()) * r) >> 32; }
    inline int range_random(const int& l, const int& r) { return l + range_random(r - l); }
    template <auto R>
    inline int range_random() {
        return (ull(uint_random()) * R) >> 32;
    }
    template <auto L, auto R>
    inline int range_random() {
        return L + range_random(R - L);
    }
    template <class T>
    inline bool bool_random(const T& p) {
        return uint_random() < (p * 4294967296);
    }
    template <auto P>
    inline bool bool_random() {
        return uint_random() < (P * 4294967296);
    }
};
class RandomGenerator {
public:
    RandomGenerator() {
        random_mw_ = 1999;
        random_mz_ = 2024;
    }
    void reset() {
        random_mw_ = 1999;
        random_mz_ = 2024;
    }
    inline int int_random() {
        update_mw();
        update_mz();
        unsigned result = (random_mz_ << 16) + random_mw_;
        return result >> 1;
    }
    inline uint uint_random_65535() {
        update_mw();
        return random_mw_ & 65535;
    }
    inline uint uint_random_65535_unsafe() {
        const unsigned tmp = random_mw_ & 65535;
        random_mw_ = 18000 * tmp + (random_mw_ >> 16);
        return tmp;
    }
    inline uint uint_random() {
        update_mw();
        update_mz();
        unsigned result = (random_mz_ << 16) + random_mw_;
        return result;
    }
    inline double double_random() {
        // 0 <= result < 1
        unsigned result = uint_random();
        return (result + 1.0) * 2.328306435454494e-10;
    }
    inline int range_random_safe(const int& r) {
        assert(r > 0);
        return uint_random() % r;
    }
    inline int range_random_safe(const int& l, const int& r) {
        assert(l < r);
        return l + range_random_safe(r - l);
    }
    inline int range_random(const int& r) {
        assert(r > 0);
        return (ull(uint_random()) * r) >> 32;
    }
    inline int range_random(const int& l, const int& r) {
        assert(l < r);
        return l + range_random(r - l);
    }
    inline int range_random_65535(const int& r) {
        assert(r > 0);
        assert(r < 65536 / 2);
        return (uint_random_65535() * r) >> 16;
    }
    inline int range_random_65535(const int& l, const int& r) {
        assert(l < r);
        return l + range_random_65535(r - l);
    }
    inline int range_random_65535_unsafe(const int& r) {
        assert(r > 0);
        assert(r < 65536 / 2);
        return (uint_random_65535_unsafe() * r) >> 16;
    }
    inline int range_random_65535_unsafe(const int& l, const int& r) {
        assert(l < r);
        return l + range_random_65535_unsafe(r - l);
    }
    template <auto R>
    inline int range_random() {
        assert(R > 0);
        return (ull(uint_random()) * R) >> 32;
    }
    template <auto L, auto R>
    inline int range_random() {
        assert(L < R);
        return L + range_random(R - L);
    }
    template <auto R>
    inline int range_random_65535() {
        assert(R > 0);
        assert(R < 65536 / 2);
        return (uint_random_65535() * R) >> 16;
    }
    template <auto L, auto R>
    inline int range_random_65535() {
        assert(L < R);
        return L + range_random_65535(R - L);
    }
    template <auto R>
    inline int range_random_65535_unsafe() {
        assert(R > 0);
        assert(R < 65536 / 2);
        return (uint_random_65535_unsafe() * R) >> 16;
    }
    template <auto L, auto R>
    inline int range_random_65535_unsafe() {
        assert(L < R);
        return L + range_random_65535_unsafe(R - L);
    }
    template <class T>
    inline bool bool_random(const T& p) {
        assert(p >= 1. / 65536);
        return uint_random_65535() < (p * 65536);
    }
    template <class T>
    inline bool bool_random_unsafe(const T& p) {
        assert(p >= 1. / 65536);
        return uint_random_65535_unsafe() < (p * 65536);
    }
    template <class T>
    inline bool bool_random_safe(const T& p) {
        return uint_random() < (p * 4294967296);
    }
    template <auto P>
    inline bool bool_random() {
        assert(P >= 1. / 65536);
        return uint_random_65535() < (P * 65536);
    }
    template <auto P>
    inline bool bool_random_unsafe() {
        assert(P >= 1. / 65536);
        return uint_random_65535_unsafe() < (P * 65536);
    }
    template <auto P>
    inline bool bool_random_safe() {
        return uint_random() < (P * 4294967296);
    }
    inline uint get_mw() const { return random_mw_; }
    inline uint get_mz() const { return random_mz_; }

private:
    uint random_mw_;
    uint random_mz_;

    inline void update_mw() { random_mw_ = 18000 * (random_mw_ & 65535) + (random_mw_ >> 16); }
    inline void update_mz() { random_mz_ = 36969 * (random_mz_ & 65535) + (random_mz_ >> 16); }
};
class RandomGeneratorMwc128xxa32 : public RandomGeneratorBase {
public:
    const u32 MULT = 3487286589;

    inline static u32 x2_ = 12345;
    inline static u32 x3_ = 0xcafef00d;
    inline static u64 c_x1_ = u64(0xd15ea5e5) << 32 | 23456;

    uint uint_random() override {
        u64 x = (u64)(x3_)*MULT;
        u32 result = (x3_ ^ x2_) + ((u32)(c_x1_) ^ (u32)(x >> 32));
        x3_ = x2_;
        x2_ = (u32)(c_x1_);
        c_x1_ = x + (c_x1_ >> 32);
        return result;
    }
    int int_random() override { return uint_random(); }
};
RandomGenerator rng;
RandomGeneratorMwc128xxa32 rng2;
#ifdef RAND_TABLE_SIZE_INPUT
constexpr int RAND_TABLE_SIZE = RAND_TABLE_SIZE_INPUT;
uint rand_table_uint[RAND_TABLE_SIZE];
void init_rand_table_uint() {
    for (int i = 0; i < RAND_TABLE_SIZE; i++) {
        rand_table_uint[i] = rng.uint_random();
    }
}
inline uint rand_from_table_uint() {
    static int idx = 0;
    if (idx == RAND_TABLE_SIZE)
        idx = 0;
    return rand_table_uint[idx++];
}
#endif
#ifdef RAND_TABLE_SIZE_INPUT_65535
constexpr int RAND_TABLE_SIZE_65535 = RAND_TABLE_SIZE_INPUT_65535;
uint rand_table_uint_65535[RAND_TABLE_SIZE_65535];
void init_rand_table_uint_65535() {
    for (int i = 0; i < RAND_TABLE_SIZE_65535; i++) {
        rand_table_uint_65535[i] = rng.uint_random() & 65535;
    }
}
inline uint rand_from_table_uint_65535() {
    static int idx = 0;
    if (idx == RAND_TABLE_SIZE_65535)
        idx = 0;
    return rand_table_uint_65535[idx++];
}
#endif


// PointTemplate
constexpr int dx[] = {1, 0, -1, 0, 1, -1, -1, 1, 0};
constexpr int dy[] = {0, 1, 0, -1, 1, 1, -1, -1, 0};
template <int N, int NEdge, typename PointInternal>
class PointTemplate {
public:
    static constexpr int M = N + NEdge;
    using TInternal = PointInternal;

private:
    PointInternal idx_;
    static constexpr int MM = M * M;
    static constexpr int NM = N * M;
    inline static bool check_inside_[MM];
    // inline static PointInternal xy2idx_[M][M];
    inline static pair<int, int> idx2xy_[MM];


public:
    static constexpr PointInternal D[9] = {PointInternal(M), PointInternal(1), PointInternal(-M), PointInternal(-1),
        PointInternal(M + 1), PointInternal(-M + 1), PointInternal(-M - 1), PointInternal(M - 1), PointInternal(0)};

    PointTemplate() : idx_(0) {}
    // PointTemplate(const int& x, const int& y) : idx_(xy2idx_[x][y]) {}
    PointTemplate(const PointInternal& idx) : idx_(idx) {}
    PointTemplate(const int& x, const int& y) : idx_(x * M + y) {}
    bool is_inside() const { return 0 <= idx_ && idx_ < NM && check_inside_[idx_]; }
    PointTemplate adj(const int& dir) const { return PointTemplate(idx_ + D[dir]); }
    template <int DIR>
    PointTemplate adj() const {
        return PointTemplate(idx_ + D[DIR]);
    }
    bool can_move_inside(const int& dir) const {
        const PointTemplate nx = adj(dir);
        return nx.is_inside();
    }
    PointInternal raw() const { return idx_; }
    int x() const { return idx2xy_[idx_].first; }
    int y() const { return idx2xy_[idx_].second; }
    void adj_update(const int& dir) { idx_ += D[dir]; }
    template <int DIR>
    void adj_update() {
        idx_ += D[DIR];
    }
    PointTemplate& operator+=(const int& dir) {
        idx_ += D[dir];
        return *this;
    }
    PointTemplate operator+(const int& dir) const { return PointTemplate(idx_ + D[dir]); }
    PointTemplate& operator-=(const int& dir) {
        idx_ -= D[dir];
        return *this;
    }
    PointTemplate operator-(const int& dir) const { return PointTemplate(idx_ - D[dir]); }
    PointTemplate& operator+=(const PointTemplate& p) {
        idx_ += p.idx_;
        return *this;
    }
    PointTemplate operator+(const PointTemplate& p) const { return PointTemplate(idx_ + p.idx_); }
    PointTemplate& operator-=(const PointTemplate& p) {
        idx_ -= p.idx_;
        return *this;
    }
    PointTemplate operator-(const PointTemplate& p) const { return PointTemplate(idx_ - p.idx_); }
    bool operator==(const PointTemplate& p) const { return idx_ == p.idx_; }
    bool operator!=(const PointTemplate& p) const { return idx_ != p.idx_; }
    auto operator<=>(const PointTemplate& p) const { return idx_ <=> p.idx_; }
    int get_dir(const PointTemplate& p) const {
        const PointInternal d = p.idx_ - idx_;
        for (int i = 0; i < 8; i++) {
            if (D[i] == d) {
                return i;
            }
        }
        return -1;
    }
    static bool init() {
        static bool flag = false;
        if (flag) {
            return false;
        }
        flag = true;
        // idx2xy_
        for (int x = 0; x < M; x++) {
            for (int y = 0; y < M; y++) {
                idx2xy_[x * M + y] = {x, y};
            }
        }
        // check_inside_
        for (int x = 0; x < M; x++) {
            for (int y = 0; y < M; y++) {
                check_inside_[x * M + y] = x < N && y < N;
            }
        }
        return true;
    }
    friend ostream& operator<<(ostream& os, const PointTemplate& p) {
        os << "(" << p.x() << ", " << p.y() << ")";
        return os;
    }
};


// log memo
#ifdef LOG_TABLE_SIZE_INPUT
constexpr int LOG_TABLE_SIZE = LOG_TABLE_SIZE_INPUT;
double log_table[LOG_TABLE_SIZE];
double log_table_stable[LOG_TABLE_SIZE];
void init_log_table() {
    // for (int i = 0; i < LOG_TABLE_SIZE; i++) {
    //     log_table[i] = log(rng.double_random());
    // }
    for (int i = 0; i < LOG_TABLE_SIZE; i++) {
        double x = ((double)i + 0.5) / LOG_TABLE_SIZE;
        log_table[i] = log(x);
        log_table_stable[i] = log_table[i];
    }
    // shuffle
    for (int i = 0; i < LOG_TABLE_SIZE; i++) {
        int j = rng.range_random_safe(LOG_TABLE_SIZE);
        swap(log_table[i], log_table[j]);
    }
}
inline double log_random() {
    static int idx = 0;
    if (idx == LOG_TABLE_SIZE)
        idx = 0;
    return log_table[idx++];
}
#endif

namespace container {
// FastDeque
// random access: O(1)
// push/pop front/back: O(1)
// rotate forward/backward: O(1)
template <typename T, int MaxSize>
class FastDeque {
    T data_[MaxSize];
    int head_;
    int tail_;

public:
    FastDeque() : head_(0), tail_(0) {}
    FastDeque(const vector<T>& vec) : head_(0), tail_(0) {
        assert(vec.size() < MaxSize);
        copy(vec.begin(), vec.end(), data_);
        tail_ = (int)vec.size();
    }
    template <class... Args>
    void emplace_back(Args&&... args) {
        data_[tail_] = T(std::forward<Args>(args)...);
        increment(tail_);
        assert(head_ != tail_);
    }
    void push_back(const T& x) {
        data_[tail_] = x;
        increment(tail_);
        assert(head_ != tail_);
    }
    void push_back(T&& x) {
        data_[tail_] = x;
        increment(tail_);
        assert(head_ != tail_);
    }
    template <class... Args>
    void emplace_front(Args&&... args) {
        decrement(head_);
        data_[head_] = T(std::forward<Args>(args)...);
        assert(head_ != tail_);
    }
    void push_front(const T& x) {
        decrement(head_);
        data_[head_] = x;
        assert(head_ != tail_);
    }
    void push_front(T&& x) {
        decrement(head_);
        data_[head_] = x;
        assert(head_ != tail_);
    }
    void pop_back() {
        assert(head_ != tail_);
        decrement(tail_);
    }
    void pop_front() {
        assert(head_ != tail_);
        increment(head_);
    }
    T pop_back_val() {
        assert(head_ != tail_);
        decrement(tail_);
        return data_[tail_];
    }
    T pop_front_val() {
        assert(head_ != tail_);
        T res = move(data_[head_]);
        increment(head_);
        return res;
    }
    int size() const {
        int sz = (tail_ >= head_ ? tail_ - head_ : MaxSize - head_ + tail_);
        return sz;
    }
    void clear() { head_ = tail_ = 0; }
    T& operator[](const int& i) { return data_[head_ + i < MaxSize ? head_ + i : head_ + i - MaxSize]; }
    const T& operator[](const int& i) const { return data_[head_ + i < MaxSize ? head_ + i : head_ + i - MaxSize]; }
    T& front() { return data_[head_]; }
    const T& front() const { return data_[head_]; }
    T& back() { return data_[tail_ == 0 ? MaxSize - 1 : tail_ - 1]; }
    const T& back() const { return data_[tail_ == 0 ? MaxSize - 1 : tail_ - 1]; }
    void rotate_forward() {
        if (head_ == tail_)
            return;
        data_[tail_] = move(data_[head_]);
        increment(head_);
        increment(tail_);
    }
    void rotate_backward() {
        if (head_ == tail_)
            return;
        decrement(head_);
        decrement(tail_);
        data_[head_] = move(data_[tail_]);
    }
    bool empty() const { return head_ == tail_; }
    vector<T> to_vector() const {
        print_warning("to_vector is slow");
        int sz = size();
        vector<T> res;
        res.reserve(sz);
        for (int i = head_; i != tail_; increment(i)) {
            res.emplace_back(data_[i]);
        }
        return res;
    }

private:
    inline void increment(int& x) const {
        if (++x == MaxSize)
            x = 0;
    }
    inline void decrement(int& x) const {
        if (--x == -1)
            x = MaxSize - 1;
    }
};

// FastStack
// random access: O(1)
// push/pop: O(1)
template <typename T, int MaxSize>
class FastStack {
private:
    // T data_[MaxSize];
    vector<T> data_;
    int top_minus1_;

public:
#ifdef PRINT_MAX_STACK_SIZE
    int max_size = 0;
#endif
    using value_type = T;
    using size_type = int;
    using reference = T&;
    using const_reference = const T&;
    using iterator = typename vector<T>::iterator;
    using const_iterator = typename vector<T>::const_iterator;
    // FastStack() : top_(0) {}
    FastStack() : top_minus1_(-1) { data_.resize(MaxSize); }
    FastStack(const vector<T>& vec) : top_minus1_(-1) {
        assert(vec.size() <= MaxSize);
        copy(vec.begin(), vec.end(), data_);
        top_minus1_ = (int)vec.size() - 1;
    }
    template <class... Args>
    void emplace(Args&&... args) {
        assert(top_minus1_ < MaxSize);
        data_[++top_minus1_] = T(std::forward<Args>(args)...);
    }
    void push(const T& x) {
        check_size();
        data_[++top_minus1_] = x;
    }
    void push(T&& x) {
        check_size();
        data_[++top_minus1_] = x;
    }
    int push_and_return_size(const T& x) {
        check_size();
        data_[++top_minus1_] = x;
        return top_minus1_ + 1;
    }
    int push_and_return_size(T&& x) {
        check_size();
        data_[++top_minus1_] = x;
        return top_minus1_ + 1;
    }
    void push_back(const T& x) {
        check_size();
        data_[++top_minus1_] = x;
    }
    void push_back(T&& x) {
        check_size();
        data_[++top_minus1_] = x;
    }
    T* push_null() {
        check_size();
        return &data_[++top_minus1_];
    }
    void pop() { --top_minus1_; }
    void pop_back() { --top_minus1_; }
    T pop_val() { return data_[top_minus1_--]; }
    T* pop_pointer() { return &data_[top_minus1_--]; }
    int size() const { return top_minus1_ + 1; }
    void clear() { top_minus1_ = -1; }
    T& operator[](const int& i) { return data_[i]; }
    const T& operator[](const int& i) const { return data_[i]; }
    T* top_pointer() {
        assert(top_minus1_ >= 0);
        return &data_[top_minus1_];
    }
    T* get_pointer(const int& i) { return &data_[i]; }
    T& top() {
        assert(top_minus1_ >= 0);
        return data_[top_minus1_];
    }
    const T& top() const {
        assert(top_minus1_ >= 0);
        return data_[top_minus1_];
    }
    vector<T>& data() { return data_; }
    bool empty() const { return top_minus1_ == -1; }
    vector<T> to_vector() const {
        print_warning("to_vector is slow");
        vector<T> res;
        res.reserve(top_minus1_ + 1);
        for (int i = 0; i < top_minus1_ + 1; i++) {
            res.emplace_back(data_[i]);
        }
        return res;
    }
    void resize(const int& n) {
        assert(n <= MaxSize);
        top_minus1_ = n - 1;
    }
    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }
    iterator begin() { return data_.begin(); }
    const_iterator begin() const { return data_.begin(); }
    iterator end() { return data_.begin() + top_minus1_ + 1; }
    const_iterator end() const { return data_.begin() + top_minus1_ + 1; }

private:
    void check_size() {
        assert(print_if_false(top_minus1_ + 1 < MaxSize, "size overflow in FastStack"));
#ifdef PRINT_MAX_STACK_SIZE
        chmax(max_size, top_minus1_ + 2);
#endif
    }
};

// ObjectPool
// reuse memory for objects
template <typename T, int MaxSize>
class ObjectPool {
    T data_[MaxSize];
    int sz_;  // maximum used size

public:
    FastStack<int, MaxSize> garbage_index_;
    ObjectPool() : sz_(0) {}
    int push(const T& x) {
        if (garbage_index_.empty()) {
            assert(sz_ < MaxSize);
            data_[sz_] = x;
            return sz_++;
        } else {
            int idx = garbage_index_.pop_val();
            data_[idx] = x;
            return idx;
        }
    }
    template <class... Args>
    int emplace(Args&&... args) {
        if (garbage_index_.empty()) {
            assert(sz_ < MaxSize);
            data_[sz_] = T(std::forward<Args>(args)...);
            return sz_++;
        } else {
            int idx = garbage_index_.pop_val();
            data_[idx] = T(std::forward<Args>(args)...);
            return idx;
        }
    }
    void pop(const int& idx) { garbage_index_.push(idx); }
    T pop_val(const int& idx) {
        garbage_index_.push(idx);
        return data_[idx];
    }
    T& operator[](const int& i) { return data_[i]; }
    const T& operator[](const int& i) const { return data_[i]; }
    int size() const { return sz_; }
    void clear() {
        sz_ = 0;
        garbage_index_.clear();
    }
};
/*
template <typename T, int MaxSize>
class ObjectPoolPointer {
    T data_[MaxSize];
    int sz_;  // maximum used size
    FastStack<T*, MaxSize> garbage_pointer_;

public:
    ObjectPoolPointer() : sz_(0) {}
    T* push(const T& x) {
        if (garbage_pointer_.empty()) {
            assert(sz_ < MaxSize);
            data_[sz_] = x;
            return &data_[sz_++];
        } else {
            return garbage_pointer_.pop_val() = &x;
        }
    }
    void pop(const T* p) {
        garbage_pointer_.push(p);
    }
    T* pop_val(const T* p) {
        garbage_pointer_.push(p);
        return p;
    }
    // T& operator[](const int& i) {
    //     return data_[i];
    // }
    int size() const {
        return sz_;
    }
};
*/

#ifdef USE_FAST_VECTOR
// vector を継承
template <typename T, class Allocator = allocator<T>>
class FastVector : public vector<T, Allocator> {
public:
    void clear() {
        // clear without calling destructor
        // this->_M_erase_at_end(this->_M_impl._M_start);
        // if (auto __n = this->_M_impl._M_finish - this->_M_impl._M_start) {
        //     // std::_Destroy(__pos, this->_M_impl._M_finish, _M_get_Tp_allocator());
        //     this->_M_impl._M_finish = this->_M_impl._M_start;
        //     // _GLIBCXX_ASAN_ANNOTATE_SHRINK(__n);
        // }
        this->_M_impl._M_finish = this->_M_impl._M_start;
    }
};
#else
template <typename T, class Allocator = allocator<T>>
using FastVector = vector<T, Allocator>;
#endif

// https://topcoder-tomerun.hatenablog.jp/entry/2021/06/12/134643
template <typename T = int>
struct IndexSet {
    static_assert(is_integral_v<T>);
    vector<T> pos;
    vector<T> que;
    int len = 0;
    int sz = 0;
    IndexSet() {}
    IndexSet(const int n) { init(n); }
    void init(const int n) {
        pos.resize(n);
        que.resize(n);
        len = 0;
        sz = n;
        iota(pos.begin(), pos.end(), 0);
        iota(que.begin(), que.end(), 0);
    }
    template <typename U>
    void add(const U& v) {
        const auto p = pos[v];
        swap(que[p], que[len++]);
        swap(pos[v], pos[que[p]]);
    }
    template <typename U>
    bool add_safe(const U& v) {
        if (!contains(v)) {
            add(v);
            return true;
        } else {
            return false;
        }
    }
    template <typename U>
    void remove(const U& v) {
        const auto p = pos[v];
        assert(p < len);
        swap(que[p], que[--len]);
        swap(pos[v], pos[que[p]]);
    }
    template <typename U>
    void remove_safe(const U& v) {
        if (contains(v)) {
            remove(v);
        }
    }
    void clear() { len = 0; }
    template <typename U>
    bool contains(const U& v) const {
        return pos[v] < len;
    }
    int size() const { return len; }
    T sample() { return que[rng.range_random(len)]; }
    bool empty() const { return len == 0; }
    void fill() { len = sz; }
    const T& operator[](const int& i) const { return que[i]; }
    T& operator[](const int& i) { return que[i]; }
};

// priority_queue for accessing vector
template <typename T, typename Container = std::vector<T>, typename Compare = std::less<typename Container::value_type>>
class MyPriorityQueue : public std::priority_queue<T, Container, Compare> {
public:
    using std::priority_queue<T, Container, Compare>::c;
    using std::priority_queue<T, Container, Compare>::priority_queue;
    Container to_vector() { return c; }
};
}  // namespace container
using namespace container;

// check if c++ version is newer than c++20
// #if __cplusplus > 202002L
namespace beam_search {
// https://atcoder.jp/contests/rcl-contest-2020-qual/submissions/49895043

// (state) -> score
template <class T, class State, class Score>
concept get_score = requires(T t, State& s) {
    requires std::totally_ordered<Score>;
    { t(s) } -> std::same_as<Score>;
};

// (state, move) -> void
template <class T, class State, class MoveId>
concept apply_move = requires(T t, State& s, const MoveId& m) {
    { t(s, m) } -> std::same_as<void>;
};

// (state) -> void
template <class T, class State, class MoveId>
concept undo_move = requires(T t, State& s, const MoveId& m) {
    { t(s, m) } -> std::same_as<void>;
};

// (state) -> void
template <class T, class State>
concept enumerate_candidates = requires(T t, State& s) {
    { t(s) } -> std::same_as<void>;
};

// // (turn) -> void
// template <class T>
// concept update_turn = requires(T t, int turn) {
//     { t(turn) } -> std::same_as<void>;
// };

template <class T>
concept begin_turn = requires(T t) {
    { t() } -> std::same_as<void>;
};

// fast?
template <class Candidate, class Hash, int MaxCandidates, class Direction = greater<decltype(Candidate::data)>,
    int MemoryRatio = 1, class SelectFromHash = SelectFromHashU32Unsafe>
class SelectorBucket {
    using CandidateData = decltype(Candidate::data);

    static_assert(MemoryRatio >= 1);
    static_assert(is_same_v<Hash, u32>);

private:
    int beam_width_ = 0;
    FastStack<Candidate, MaxCandidates * MemoryRatio> candidates_;
    Direction dir_ = {};
    SelectFromHash select_from_hash = {};
    bool flag_full_ = false;

public:
    SelectorBucket() {}
    void set_beam_width(const int beam_width) {
        assert(0 < beam_width && beam_width <= MaxCandidates);
        beam_width_ = beam_width * MemoryRatio;
    }
    bool push(const Candidate& c, const Hash& hash) {
        assert(beam_width_ > 0);
        // if (candidates_.size() < beam_width_ && idx >= candidates_.size()) {
        // if (candidates_.size() < beam_width_) {
        if (!flag_full_) {
            candidates_.push(c);
            flag_full_ = (candidates_.size() == beam_width_);
            return true;
        } else {
            const int idx = select_from_hash(hash, beam_width_);
            if (dir_(c.data, candidates_[idx].data)) {
                candidates_[idx] = c;
                return true;
            }
            return false;
        }
    }
    // 高速化
    template <class MoveId>
    bool push(const int& parent, const MoveId& move_id, const CandidateData& data, const Hash& hash) {
        assert(beam_width_ > 0);
        if (!flag_full_) {
            flag_full_ = candidates_.push_and_return_size(Candidate{parent, move_id, data}) == beam_width_;
            return true;
        } else {
            const auto idx = select_from_hash(hash, beam_width_);
            auto& candidate = candidates_[idx];
            if (dir_(data, candidate.data)) {
                candidate = Candidate{parent, move_id, data};
                return true;
            }
            return false;
        }
    }
    void eval() {
        if constexpr (MemoryRatio != 1) {
            if (candidates_.size() > beam_width_ / MemoryRatio) {
                nth_element(candidates_.data().begin(), candidates_.data().begin() + beam_width_ / MemoryRatio,
                    candidates_.data().begin() + candidates_.size(),
                    [&](const Candidate& a, const Candidate& b) { return dir_(a.data, b.data); });
                candidates_.resize(beam_width_);
            }
        }
    }
    Candidate& operator[](const int& i) { return candidates_[i]; }
    const Candidate& operator[](const int& i) const { return candidates_[i]; }
    void clear() {
        candidates_.clear();
        flag_full_ = false;
    }
    int size() const {
        if constexpr (MemoryRatio == 1) {
            return candidates_.size();
        } else {
            return min((int)candidates_.size(), beam_width_ / MemoryRatio);
        }
    }
    bool empty() const { return candidates_.empty(); }
};

// 普通のビームサーチ
// 重複削除
// 上位N個
template <class Candidate, class Hash, int MaxCandidates, class Direction = greater<decltype(Candidate::data)>>
class SelectorGreedy {
    using CandidateData = decltype(Candidate::data);

private:
    int beam_width_ = 0;
    unordered_map<Hash, Candidate> candidates_mp_;
    vector<Candidate> candidates_;
    Direction dir_ = {};

public:
    SelectorGreedy() {}
    void set_beam_width(const int beam_width){
        beam_width_ = beam_width;
    }
    bool push(const Candidate& c, const Hash& hash){
        assert(beam_width_ > 0);
        /* candidates_[hash] = c; */
        if(candidates_mp_.count(hash) == 0 || dir_(c.data, candidates_mp_[hash].data)){
            candidates_mp_[hash] = c;
            return true;
        }
        return false;
    }
    template <class MoveId>
    bool push(const int& parent, const MoveId& move_id, const CandidateData& data, const Hash& hash) {
        assert(beam_width_ > 0);
        if (candidates_mp_.count(hash) == 0 || dir_(data, candidates_mp_[hash].data)) {
            candidates_mp_[hash] = Candidate{parent, move_id, data};
            return true;
        }
        return false;
    }
    void eval(){
        candidates_.clear();
        candidates_.reserve(candidates_mp_.size());
        auto it = candidates_mp_.begin();
        while(it != candidates_mp_.end()){
            candidates_.emplace_back(it->second);
            it++;
        }
        // candidates_mp_.clear();
        if((int)candidates_.size() > beam_width_){
            nth_element(candidates_.begin(), candidates_.begin()+beam_width_, candidates_.end(),
                [&](const Candidate& c1, const Candidate& c2){return dir_(c1.data, c2.data);});
            candidates_.resize(beam_width_);
        }
    }
    Candidate& operator[](const int& i) { return candidates_[i]; }
    const Candidate& operator[](const int& i) const { return candidates_[i]; }
    void clear() { 
        candidates_mp_.clear();
        candidates_.clear();
    }
    int size() const {
        return min(beam_width_, (int)candidates_mp_.size());
    }
    bool empty() const { return candidates_mp_.empty(); }
};

template <class T, class Candidate, class Hash>
concept selector = requires(T& s, Candidate& c, const Hash& h, const int& i){
    { s.set_beam_width(i) } -> std::same_as<void>;
    { s.push(c, h) } -> std::same_as<bool>;
    { s.eval() } -> std::same_as<void>;
    { s[i] } -> std::same_as<Candidate&>;
    { s.clear() } -> std::same_as<void>;
    { s.size() } -> std::same_as<int>;
};

template <class State, totally_ordered Score, typename MoveId, totally_ordered CandidateData, typename Hash,
    template <class, class, int, class> class Selector,
    class DirectionScore = greater<Score>, class DirectionCost = greater<CandidateData>, int MaxBeamWidth = 1,
    int MaxCandidates = 100, int MaxNodes = 100>
class BeamSearch {
private:
    struct TreeNode {
        TreeNode* parent;
        TreeNode* child;
        TreeNode* prev;
        TreeNode* next;
        MoveId move_id;
        int index;
    };
    struct Candidate {
        int parent;
        MoveId move_id;
        CandidateData data;
    };

    DirectionScore dir_;
    int current_parent_ = 0;
    bool enumerating_ = false;
    bool filtering_ = false;

    template <apply_move<State, MoveId> ApplyMove, undo_move<State, MoveId> UndoMove, class OnLeaf>
    void dfs_tree(TreeNode* root, State& state, ApplyMove apply_move, UndoMove undo_move, OnLeaf on_leaf) {
        TreeNode* current_node = root;
        while (current_node) {
            // go down to leaf
            while (current_node->child) {
                current_node = current_node->child;
                apply_move(state, current_node->move_id);
            }
            // visit leaf nodes
            on_leaf(current_node);
            while (current_node->next) {
                undo_move(state, current_node->move_id);
                current_node = current_node->next;
                apply_move(state, current_node->move_id);
                on_leaf(current_node);
            }
            while (true) {
                if (current_node == root)
                    return;
                // go up to parent where sibling exists
                undo_move(state, current_node->move_id);
                current_node = current_node->parent;
                if (current_node->next) {
                    undo_move(state, current_node->move_id);
                    current_node = current_node->next;
                    apply_move(state, current_node->move_id);
                    break;
                }
            }
        }
    }

public:
    Selector<Candidate, Hash, MaxCandidates, DirectionCost> candidates;
    static_assert(selector<decltype(candidates), Candidate, Hash>);

    Score best_score = 0;
    int max_turn = -1;
    int beam_width = 1;
    bool flag_end = false;
    int turn = 0;

    BeamSearch() {}
    BeamSearch(DirectionCost dir) : dir_(dir) {}

    bool add_candidate(const MoveId& move_id, const CandidateData& data, const Hash& hash) {
        assert(print_if_false(enumerating_, "not enumerating now"));
        // return candidates.push(Candidate{current_parent_, move_id, data}, hash); // slow
        return candidates.push(current_parent_, move_id, data, hash);  // fast
    }

    template <get_score<State, Score> GetScore, apply_move<State, MoveId> ApplyMove, undo_move<State, MoveId> UndoMove,
        enumerate_candidates<State> EnumerateCandidates, begin_turn BeginTurn>
    vector<MoveId> run_tree(const State& inital_state, GetScore get_score, ApplyMove apply_move, UndoMove undo_move,
        EnumerateCandidates enumerate_candidates, BeginTurn begin_turn) {
        using Node = TreeNode;
        FastStack<Node, MaxNodes> nodes;
        FastStack<Node*, MaxNodes> nodes_unused;
        auto pick = [&]() {
            if (nodes_unused.empty()) {
                Node* res = nodes.push_null();
                res->index = nodes.size() - 1;
                return res;
            } else {
                return nodes_unused.pop_val();
            }
        };
        auto pool = [&](Node* node) { nodes_unused.push(node); };
        FastStack<Node*, MaxNodes> current_leaves;  // TODO
        turn = 0;

        // set initial state
        State state = inital_state;
        Node* root = pick();
        *root = {nullptr, nullptr, nullptr, nullptr, {}, 0};

        while (true) {
#ifdef DEBUG
            cerr << "turn = " << turn << endl;
#endif
            begin_turn();

            // update "root"
            while (root->child && !root->child->next) {
                root = root->child;
                apply_move(state, root->move_id);
            }

            // update candidates
            candidates.clear();
            if (max_turn == -1 || turn < max_turn) {
                // enumerate candidates
                enumerating_ = true;
                current_leaves.clear();
                dfs_tree(root, state, apply_move, undo_move, [&](Node* leaf) {
                    current_leaves.push(leaf);
                    current_parent_ = leaf->index;
                    enumerate_candidates(state);
                });
                enumerating_ = false;
            }

            // check if finished
            if (flag_end || candidates.empty()) {
                // pick the best state
                Node* best_leaf = nullptr;
                dfs_tree(root, state, apply_move, undo_move, [&](Node* leaf) {
                    Score score = get_score(state);
                    if (best_leaf == nullptr || dir_(score, best_score)) {
                        best_score = score;
                        best_leaf = leaf;
                    }
                });
                assert(print_if_false(best_leaf, "no states at the end"));

                print_debug("best score in beam search", best_score);

                // restore moves
                vector<MoveId> res;
                Node* node = best_leaf;
                while (node->parent) {
                    res.push_back(node->move_id);
                    node = node->parent;
                }
                reverse(res.begin(), res.end());

#ifdef PRINT_MAX_STACK_SIZE
                cerr << "print max stack size" << endl;
                cerr << "nodes.max_size = " << nodes.max_size << endl;
#endif

                return res;
            }

            candidates.eval();

            // compute next states
            const int n_candidates = candidates.size();
            for (int i = 0; i < n_candidates; i++) {
                const auto& cand = candidates[i];
                Node* parent = nodes.get_pointer(cand.parent);
                Node* node = pick();
                *node = {parent, nullptr, nullptr, parent->child, cand.move_id, node->index};
                if (node->next) {
                    node->next->prev = node;
                }
                parent->child = node;
            }

            // pool unused nodes
            const int n_current_leaves = current_leaves.size();
            for (int i = 0; i < n_current_leaves; i++) {
                Node* node = current_leaves[i];
                while (!node->child) {
                    pool(node);
                    const auto prev = node->prev;
                    const auto next = node->next;
                    if (prev) {
                        if (next) {
                            prev->next = next;
                            next->prev = prev;
                        } else {
                            prev->next = nullptr;
                        }
                    } else {
                        auto parent = node->parent;
                        if (next) {
                            next->prev = nullptr;
                            parent->child = next;
                        } else {
                            parent->child = nullptr;
                            node = parent;
                            continue;
                        }
                    }
                    break;
                }
            }
            turn++;
        }
    }
};

template <class BeamSearch>
class BeamWidthManager {
    BeamSearch* bs_;
    int start_time_;
    int time_limit_;
    int turn_limit_;
    int min_beam_width_;
    int max_beam_width_;
    int current_beam_width_;
    int sum_beam_width_;


public:
    BeamWidthManager(BeamSearch& bs) : sum_beam_width_(0) { bs_ = &bs; }

    void init(const int min_beam_width, const int max_beam_width, const int time_limit) {
        start_time_ = get_time();
        time_limit_ = time_limit;
        turn_limit_ = bs_->max_turn;
        min_beam_width_ = min_beam_width;
        max_beam_width_ = max_beam_width;
    }

    void update(bool flag_udpate_beam_width = false) {
        current_beam_width_ = bs_->candidates.size();
        sum_beam_width_ += current_beam_width_;
        if (turn_limit_ >= 1) {
            int time_elapsed = get_time() - start_time_;
            int turn_elapsed = bs_->turn;
            int time_remaining = time_limit_ - get_time();
            int turn_remaining = turn_limit_ - turn_elapsed;
            if (flag_udpate_beam_width) {
                // 呼び出し側で管理

                // https://qiita.com/rhoo/items/2f647e32f6ff2c6ee056#%E7%AC%AC2%E5%9B%9E-rco%E6%97%A5%E6%9C%AC%E6%A9%8B%E3%83%8F%E3%83%BC%E3%83%95%E3%83%9E%E3%83%A9%E3%82%BD%E3%83%B3-%E4%BA%88%E9%81%B8-a-%E3%82%B2%E3%83%BC%E3%83%A0%E5%AE%9F%E6%B3%81%E8%80%85x%E3%81%AE%E6%8C%91%E6%88%A6
                // int a = turn_limit_ - bs_->turn;
                // int n = bs_->turn;
                // int r = time_limit_ - get_time();
                // int e = time_elapsed;
                // double coef = sqrt(1. * n * r / (a * e));
                // bs_->candidates.set_beam_width(
                //     clamp(int(current_beam_width_ * coef), min_beam_width_, max_beam_width_));

                int new_beam_width = int(1. * sum_beam_width_ * time_remaining / time_elapsed / turn_remaining);
                bs_->candidates.set_beam_width(clamp(new_beam_width, min_beam_width_, max_beam_width_));
            }
        } else {
            cerr << "turn limit = " << turn_limit_ << endl;
            cerr << "turn_limit is not set or invalid or not implemented" << endl;
        }
    }
};
}  // namespace beam_search
// #endif


// probabilities
template <class T>
T cdf(const T& x, const T& mu = 0, const T& sigma = 1) {
    return 0.5 * (1 + erf((x - mu) / (sigma * sqrt(T(2)))));
}
#ifdef LOG_TABLE_SIZE_INPUT
template <class T>
T calc_entropy(const vector<T>& p) {
    T res = 0;
    for (const auto& x : p) {
        if (x >= 0.5 / LOG_TABLE_SIZE) {
            res -= x * log_table_stable[int(x * LOG_TABLE_SIZE)];
        }
        // if (x > 1e-9)
        //     res -= x * log(x);
    }
    return res;
}
#endif
template <typename T>
void normalize(vector<T>& p) {
    T sum = accumulate(p.begin(), p.end(), T(0));
    for (auto& x : p) {
        x /= sum;
    }
}


void init_library() {
#ifdef LOG_TABLE_SIZE_INPUT
    init_log_table();
#endif
#ifdef RAND_TABLE_SIZE_INPUT
    init_rand_table_uint();
#endif
#ifdef RAND_TABLE_SIZE_INPUT_65535
    init_rand_table_uint_65535();
#endif
}

}  // namespace ethy
#line 26 "main.cpp"

using namespace std;
using namespace ethy;

// -----------------------------------------------------------------------------
//  Problem constants & helper aliases
// -----------------------------------------------------------------------------
constexpr int N = 40;    // board size is fixed
int M;                   // number of initial rocks
vector<string> S(N);     // input board
uint8_t is_walls[N][N];  // 1 = rock, 0 = free

using pii = pair<int, int>;

// Early‑discard parameter ------------------------------------------------------
//   If, during evaluation of the first `prefix_len` moves, the probability that
//   the robot has already been crushed exceeds this threshold, the routine
//   returns immediately (giving up on this candidate).
//   Tune freely (e.g., 0.005, 0.02 …).
constexpr double DISCARD_THRESHOLD = 1.00;

// Small epsilon for float comparisons
constexpr double EPS = 1e-12;

// -----------------------------------------------------------------------------
//  Utility
// -----------------------------------------------------------------------------
int calc_dist(const pii& a, const pii& b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

// -----------------------------------------------------------------------------
//  Core: expectation evaluation + greedy suffix generator
// -----------------------------------------------------------------------------
/**
 * @brief   Evaluate expected score for a hybrid solution.
 *
 * @param   prefix_list     – first `prefix_len` moves (may be empty when 0).
 * @param   prefix_len      – length of the prefix to replay exactly.
 * @param   full_len        – total number of moves (N² − M).
 * @param   cutoff          – conventional optimistic‑upper‑bound cutoff
 *                            (negative to disable).
 * @param   discard_thr     – early‑discard threshold for cumulative crash prob.
 * @return  {expected_score, full_solution_list}. If the evaluation was aborted
 *          by a cutoff/threshold, second element is empty.
 */
pair<double, vector<pii>> calculate_hybrid_score_and_get_list(
    const vector<pii>& prefix_list,
    int prefix_len,
    int full_len,
    double cutoff = -1.0,
    double discard_thr = DISCARD_THRESHOLD) {

    // ------------------------- initialisation --------------------------------
    uint8_t current_is_walls[N][N];
    memcpy(current_is_walls, is_walls, sizeof(is_walls));

    float prob[N][N] = {};
    if (full_len <= 1)
        return {0.0, {}};

    const float initial_prob = 1.0f / static_cast<float>(full_len);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (!is_walls[i][j])
                prob[i][j] = initial_prob;

    // Pre‑compute continuous segments of empty cells along rows and columns.
    array<vector<pii>, N> row_segments, col_segments;
    for (int i = 0; i < N; ++i) {
        int st = -1;
        for (int j = 0; j < N; ++j) {
            if (!current_is_walls[i][j]) {
                if (st == -1)
                    st = j;
            } else {
                if (st != -1) {
                    row_segments[i].push_back({st, j - 1});
                    st = -1;
                }
            }
        }
        if (st != -1)
            row_segments[i].push_back({st, N - 1});
    }
    for (int j = 0; j < N; ++j) {
        int st = -1;
        for (int i = 0; i < N; ++i) {
            if (!current_is_walls[i][j]) {
                if (st == -1)
                    st = i;
            } else {
                if (st != -1) {
                    col_segments[j].push_back({st, i - 1});
                    st = -1;
                }
            }
        }
        if (st != -1)
            col_segments[j].push_back({st, N - 1});
    }

    double expected_score = 0.0;
    double prob_survive_tot = 1.0;  // P(robot still alive before current step)

    pii last_pos{-1, -1};
    vector<pii> full_list;
    full_list.reserve(full_len);

    // ---------------------------- Phase 1 -------------------------------------
    float next_prob[N][N] = {};
    // double next_prob_sum = 0;
    double last_updated_iter[N][N] = {};
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            next_prob[i][j] = 0.0;
            last_updated_iter[i][j] = -1;
        }
    }

    uint64_t next_active_cells_rows[N] = {};

    auto add_next_prob = [&](int iter, int r, int c, double p) {
        assert(r >= 0 && r < N && c >= 0 && c < N);
        if (last_updated_iter[r][c] != iter) {
            next_prob[r][c] = 0.0;
            last_updated_iter[r][c] = iter;
            next_active_cells_rows[r] |= (1LL << c);
        }
        next_prob[r][c] += p;
        // next_prob_sum += p;
    };


    for (int k = 0; k < prefix_len; ++k) {
        const auto& p = prefix_list[k];
        full_list.push_back(p);
        const int pr = p.first, pc = p.second;

        // ① move distribution
        memset(next_prob, 0, sizeof(next_prob));
        for (int r = 0; r < N; ++r) {
            for (const auto& seg : row_segments[r]) {
                float s = 0.f;
                for (int c = seg.first; c <= seg.second; ++c)
                    s += prob[r][c];
                if (s > EPS) {
                    next_prob[r][seg.first] += 0.25f * s;
                    next_prob[r][seg.second] += 0.25f * s;
                }
            }
        }
        for (int c = 0; c < N; ++c) {
            for (const auto& seg : col_segments[c]) {
                float s = 0.f;
                for (int r = seg.first; r <= seg.second; ++r)
                    s += prob[r][c];
                if (s > EPS) {
                    next_prob[seg.first][c] += 0.25f * s;
                    next_prob[seg.second][c] += 0.25f * s;
                }
            }
        }

        // ② crash & survive probability
        const float prob_crash = next_prob[pr][pc];
        prob_survive_tot -= prob_crash;

        expected_score += prob_survive_tot;

        // ③ update board & segments
        current_is_walls[pr][pc] = 1;
        last_pos = p;
        // row‑segments split
        for (auto it = row_segments[pr].begin(); it != row_segments[pr].end();) {
            if (it->first <= pc && pc <= it->second) {
                const int s = it->first, e = it->second;
                it = row_segments[pr].erase(it);
                if (s < pc)
                    row_segments[pr].emplace_back(s, pc - 1);
                if (pc < e)
                    row_segments[pr].emplace_back(pc + 1, e);
                break;
            } else
                ++it;
        }
        // col‑segments split
        for (auto it = col_segments[pc].begin(); it != col_segments[pc].end();) {
            if (it->first <= pr && pr <= it->second) {
                const int s = it->first, e = it->second;
                it = col_segments[pc].erase(it);
                if (s < pr)
                    col_segments[pc].emplace_back(s, pr - 1);
                if (pr < e)
                    col_segments[pc].emplace_back(pr + 1, e);
                break;
            } else
                ++it;
        }

        // ④ renormalise prob
        for (int r = 0; r < N; ++r)
            for (int c = 0; c < N; ++c)
                prob[r][c] = next_prob[r][c];
        prob[pr][pc] = 0.f;
    }

    // ---------------------------- Phase 2 -------------------------------------
    for (int k = prefix_len; k < full_len; ++k) {
        // (1) compute next_prob similarly
        memset(next_prob, 0, sizeof(next_prob));
        for (int r = 0; r < N; ++r) {
            for (const auto& seg : row_segments[r]) {
                float s = 0.f;
                for (int c = seg.first; c <= seg.second; ++c)
                    s += prob[r][c];
                if (s > EPS) {
                    next_prob[r][seg.first] += 0.25f * s;
                    next_prob[r][seg.second] += 0.25f * s;
                }
            }
        }
        for (int c = 0; c < N; ++c) {
            for (const auto& seg : col_segments[c]) {
                float s = 0.f;
                for (int r = seg.first; r <= seg.second; ++r)
                    s += prob[r][c];
                if (s > EPS) {
                    next_prob[seg.first][c] += 0.25f * s;
                    next_prob[seg.second][c] += 0.25f * s;
                }
            }
        }

        // (2) pick best position by 3‑tier heuristic
        float min_prob = 1e9f;
        for (int r = 0; r < N; ++r)
            for (int c = 0; c < N; ++c)
                if (!current_is_walls[r][c])
                    min_prob = min(min_prob, next_prob[r][c]);

        vector<pii> prob_candidates;
        for (int r = 0; r < N; ++r)
            for (int c = 0; c < N; ++c)
                if (!current_is_walls[r][c] && fabs(next_prob[r][c] - min_prob) < EPS)
                    prob_candidates.push_back({r, c});

        pii best_pos;
        if (prob_candidates.size() == 1) {
            best_pos = prob_candidates[0];
        } else {
            float min_traffic = 1e9f;
            vector<pair<float, pii>> traffic_scores;
            for (const auto& cand : prob_candidates) {
                const int r = cand.first, c = cand.second;
                float h_sum = 0.f, v_sum = 0.f;
                for (const auto& seg : row_segments[r])
                    if (c >= seg.first && c <= seg.second)
                        for (int cc = seg.first; cc <= seg.second; ++cc)
                            h_sum += prob[r][cc];
                for (const auto& seg : col_segments[c])
                    if (r >= seg.first && r <= seg.second)
                        for (int rr = seg.first; rr <= seg.second; ++rr)
                            v_sum += prob[rr][c];
                const float score = h_sum + v_sum;
                min_traffic = min(min_traffic, score);
                traffic_scores.push_back({score, cand});
            }
            vector<pii> traffic_candidates;
            for (const auto& sc : traffic_scores)
                if (fabs(sc.first - min_traffic) < EPS)
                    traffic_candidates.push_back(sc.second);
            if (traffic_candidates.size() == 1) {
                best_pos = traffic_candidates[0];
            } else {
                int max_dist = -1;
                best_pos = traffic_candidates[0];
                if (last_pos.first != -1)
                    for (const auto& cand : traffic_candidates) {
                        const int d = calc_dist(cand, last_pos);
                        if (d > max_dist) {
                            max_dist = d;
                            best_pos = cand;
                        }
                    }
            }
        }

        const int pr = best_pos.first, pc = best_pos.second;
        full_list.push_back(best_pos);
        if (k == full_len - 1)
            break;  // last move adds no score

        const float prob_crash = next_prob[pr][pc];
        prob_survive_tot -= prob_crash;
        expected_score += prob_survive_tot;

        // update board / segments / distribution (same as Phase‑1)
        current_is_walls[pr][pc] = 1;
        last_pos = best_pos;
        // row split
        for (auto it = row_segments[pr].begin(); it != row_segments[pr].end();) {
            if (it->first <= pc && pc <= it->second) {
                const int s = it->first, e = it->second;
                it = row_segments[pr].erase(it);
                if (s < pc)
                    row_segments[pr].push_back({s, pc - 1});
                if (pc < e)
                    row_segments[pr].push_back({pc + 1, e});
                break;
            } else
                ++it;
        }
        // column split
        for (auto it = col_segments[pc].begin(); it != col_segments[pc].end();) {
            if (it->first <= pr && pr <= it->second) {
                const int s = it->first, e = it->second;
                it = col_segments[pc].erase(it);
                if (s < pr)
                    col_segments[pc].push_back({s, pr - 1});
                if (pr < e)
                    col_segments[pc].push_back({pr + 1, e});
                break;
            } else
                ++it;
        }
        for (int r = 0; r < N; ++r)
            for (int c = 0; c < N; ++c)
                prob[r][c] = next_prob[r][c];
        prob[pr][pc] = 0.f;
    }

    return {expected_score, full_list};
}

// -----------------------------------------------------------------------------
//  Solver (greedy + hill‑climb)
// -----------------------------------------------------------------------------
void solve() {
    const int empty_count = N * N - M;
    if (empty_count == 0)
        return;  // nothing to do

    // --- 1. Greedy initial solution -----------------------------------------
    auto init_res = calculate_hybrid_score_and_get_list({}, 0, empty_count, -1.0, DISCARD_THRESHOLD);
    double best_score = init_res.first;
    vector<pii> best_list = init_res.second;

    // --- 2. Hill‑climb -------------------------------------------------------
    const int time_limit = TIME_LIMIT_INPUT;

    // exclude last 380 moves from swapping – they matter less & rebuild is cheap
    const int prefix_len = max(0, empty_count - 450);
    vector<pii> current_prefix(best_list.begin(), best_list.begin() + prefix_len);

    int iter = 0;
    while (get_time() < time_limit && prefix_len >= 2) {
        int i, j;
        int dist;
        do {
            if (rng.range_random(100) < 50) {

                i = rng.range_random(prefix_len - 1);
                j = rng.range_random(prefix_len - 1);
            } else {
                i = rng.range_random(prefix_len / 2 - 1);
                j = rng.range_random(prefix_len / 2, prefix_len - 1);
            }
        } while (i == j || abs(i - j) < 300);

        swap(current_prefix[i], current_prefix[j]);

        auto res = calculate_hybrid_score_and_get_list(current_prefix,
            prefix_len,
            empty_count,
            best_score,
            DISCARD_THRESHOLD);
        const double new_score = res.first;
        if (!res.second.empty() && new_score > best_score + 1e-9) {
            best_score = new_score;
            best_list = std::move(res.second);
        } else {
            swap(current_prefix[i], current_prefix[j]);  // revert
        }
        ++iter;
    }

#ifndef ONLINE_JUDGE
    print_data("hill_climb_iterations", iter);
    int final_score_print = static_cast<int>(round(best_score * 1e6 / max(1, empty_count - 1)));
    print_data("score", final_score_print);
#endif

    // --- 3. Output -----------------------------------------------------------
    for (const auto& p : best_list)
        cout << p.first << " " << p.second << '\n';
}

// -----------------------------------------------------------------------------
//  I/O & init
// -----------------------------------------------------------------------------
void init_main() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            is_walls[i][j] = (S[i][j] == '#');
}

void read_input() {
    int dummy;
    cin >> dummy >> M;
    for (int i = 0; i < N; ++i)
        cin >> S[i];
}

int main() {
    init_library();
    print_time();
    read_input();
    init_main();
    solve();
    print_time();
    return 0;
}
