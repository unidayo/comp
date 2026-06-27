#line 1 "main.cpp"
// 027.cpp final

#ifndef __clang__
#pragma GCC optimize("unroll-loops")
#pragma GCC optimize("O3,inline")            // Ofast = O3,fast-math,allow-store-data-races,no-protect-parens
#pragma GCC target("bmi,bmi2,lzcnt,popcnt")  // bit manipulation
#pragma GCC target("movbe")                  // byte swap
#pragma GCC target("aes,pclmul,rdrnd")       // encryption
#pragma GCC target("avx,avx2,f16c,fma,sse3,ssse3,sse4.1,sse4.2")  // SIMD
#endif

// #undef _GLIBCXX_DEBUG  // disable run-time bound checking, etc
#define NDEBUG

// #define _GLIBCXX_DEBUG

// #define DEBUG

#define TIME_LIMIT_INPUT 2945
// #define TIME_RATIO_INPUT 1.5
// #define TIME_LIMIT_INPUT 7 * 60 * 60 * 1000
// #define TIME_LIMIT_INPUT 600 * 1000
#define TIME_RATIO_INPUT 1
#define LOG_TABLE_SIZE_INPUT (1 << 20)
// #define PRINT_MAX_STACK_SIZE

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
#include <string>
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

using namespace std;
using std::cin;
using std::cout;
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
int select_from_hash(const uint& hash, const T& n) {
    // return int(ull(hash) * n >> 32);
    return hash % n;
}
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

private:
    uint random_mw_;
    uint random_mz_;

    inline void update_mw() { random_mw_ = 18000 * (random_mw_ & 65535) + (random_mw_ >> 16); }
    inline void update_mz() { random_mz_ = 36969 * (random_mz_ & 65535) + (random_mz_ >> 16); }
};
RandomGenerator rng;


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
    int top_;

public:
#ifdef PRINT_MAX_STACK_SIZE
    int max_size = 0;
#endif
    // FastStack() : top_(0) {}
    FastStack() : top_(0) { data_.resize(MaxSize); }
    FastStack(const vector<T>& vec) : top_(0) {
        assert(vec.size() <= MaxSize);
        copy(vec.begin(), vec.end(), data_);
        top_ = (int)vec.size();
    }
    template <class... Args>
    void emplace(Args&&... args) {
        assert(top_ < MaxSize);
        data_[top_++] = T(std::forward<Args>(args)...);
    }
    void push(const T& x) {
        check_size();
        data_[top_++] = x;
    }
    void push(T&& x) {
        check_size();
        data_[top_++] = x;
    }
    T* push_null() {
        check_size();
        return &data_[top_++];
    }
    void pop() { --top_; }
    T pop_val() { return data_[--top_]; }
    T* pop_pointer() { return &data_[--top_]; }
    int size() const { return top_; }
    void clear() { top_ = 0; }
    T& operator[](const int& i) { return data_[i]; }
    const T& operator[](const int& i) const { return data_[i]; }
    T* top_pointer() {
        assert(top_ > 0);
        return &data_[top_ - 1];
    }
    T* get_pointer(const int& i) { return &data_[i]; }
    T& top() {
        assert(top_ > 0);
        return data_[top_ - 1];
    }
    const T& top() const {
        assert(top_ > 0);
        return data_[top_ - 1];
    }
    vector<T>& data() { return data_; }
    bool empty() const { return top_ == 0; }
    vector<T> to_vector() const {
        print_warning("to_vector is slow");
        vector<T> res;
        res.reserve(top_);
        for (int i = 0; i < top_; i++) {
            res.emplace_back(data_[i]);
        }
        return res;
    }
    void resize(const int& n) {
        assert(n <= MaxSize);
        top_ = n;
    }

private:
    void check_size() {
        assert(print_if_false(top_ < MaxSize, "size overflow in FastStack"));
#ifdef PRINT_MAX_STACK_SIZE
        chmax(max_size, top_ + 1);
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
    int sample() { return que[rng.range_random(len)]; }
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

template <class T, class Candidate, class Score>
concept selector = requires(T& s, Candidate& c, const int& i) {
    { s.set_beam_width(i) } -> std::same_as<void>;
    { s.push(c) } -> std::same_as<bool>;
    { s.eval() } -> std::same_as<void>;
    { s[i] } -> std::same_as<Candidate&>;
    { s.size() } -> std::same_as<int>;
};

template <class T>
concept begin_turn = requires(T t) {
    { t() } -> std::same_as<void>;
};

template <class Candidate, class Hash, int MaxCandidates, class Direction = greater<decltype(Candidate::data)>,
    int MemoryRatio = 1>
class SelectorNagiss {
    // clang-format off
    // kaggle santa2023 での会話 (2023/01/07)
    // c-number 27 days ago ビームサーチでスコア上位n個を選ぶ代わりに今の実装にしているのは、多様性担保のためでしょうか？
    // nagiss 27 days ago 実装が楽で性能も良いと思っているので自分はよくそう書いてる感じです
    // c-number 27 days ago なるほど、大変参考になります（AHCでビーム書いたことがないので）
    // nagiss 27 days ago nagissサーチと呼んでいいですよ(?)
    // c-number 27 days ago このビムサで勝てたら宣伝させていただきます！
    // nagiss 27 days ago ハッシュ値みたいなのを使って似た状態が同じ場所に入るように変更しやすいのも良いところです
    // c-number 27 days ago あー、たしかにです
    // clang-format on

    using CandidateData = decltype(Candidate::data);

    static_assert(MemoryRatio >= 1);
    static_assert(is_same_v<Hash, u32>);

private:
    int beam_width_ = 0;
    FastStack<Candidate, MaxCandidates * MemoryRatio> candidates_;
    Direction dir_ = {};

public:
    SelectorNagiss() {}
    void set_beam_width(const int beam_width) {
        assert(0 < beam_width && beam_width <= MaxCandidates);
        beam_width_ = beam_width * MemoryRatio;
    }
    bool push(const Candidate& c, const Hash& hash) {
        assert(beam_width_ > 0);
        if (candidates_.size() < beam_width_) {
            candidates_.push(c);
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
    void clear() { candidates_.clear(); }
    int size() const {
        if constexpr (MemoryRatio == 1) {
            return candidates_.size();
        } else {
            return min((int)candidates_.size(), beam_width_ / MemoryRatio);
        }
    }
    bool empty() const { return candidates_.empty(); }
};

template <class State, totally_ordered Score, typename MoveId, totally_ordered CandidateData, typename Hash,
    template <class, class, int, class>
    class Selector,  // 上で定義しているselectorを使いたいけど方法がわからない 分かる方がいれば助けて欲しいです
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
    Score best_score = 0;
    int max_turn = -1;
    int beam_width = 1;
    bool flag_end = false;
    int turn = 0;

    BeamSearch() {}
    BeamSearch(DirectionCost dir) : dir_(dir) {}

    bool add_candidate(const MoveId& move_id, const CandidateData& data, const Hash& hash) {
        assert(print_if_false(enumerating_, "not enumerating now"));
        return candidates.push(Candidate{current_parent_, move_id, data}, hash);
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


// probabilities
template <class T>
T cdf(const T& x, const T& mu = 0, const T& sigma = 1) {
    return 0.5 * (1 + erf((x - mu) / (sigma * sqrt(T(2)))));
}
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
}

}  // namespace ethy
#line 29 "main.cpp"
// #include <atcoder/all>

#define COMPILE_SOLVE_ALL
#define CHECK_TIME
// #define HIGH_PERFORMANCE
// #define WA_WHEN_WRONG
// #define PRINT_DEBUG
// #define PRINT_HEATMAP

using namespace std;
using namespace ethy;

namespace sample {
void solve(int N, int M, double eps) {
    // Drill every square
    vector<pair<int, int>> has_oil;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << "q 1 " << i << " " << j << endl;
            string resp;
            cin >> resp;
            if (resp != "0") {
                has_oil.push_back({i, j});
            }
        }
    }

    // Print results
    cout << "a " << has_oil.size();
    for (auto& p : has_oil) {
        cout << " " << p.first << " " << p.second;
    }
    cout << endl;

    // Final response check
    string finalResp;
    cin >> finalResp;
    assert(finalResp == "1");
}
}  // namespace sample

// basic classes

template <int N, typename PointInternal>
class Point {
    static_assert(10 <= N && N <= 20, "N must be 10 <= N <= 20");
    static_assert(is_same_v<PointInternal, u8> || is_same_v<PointInternal, u16>, "PointInternal must be u8 or u16");

private:
    PointInternal idx_;

public:
    Point() : idx_(0) {}
    Point(const int x, const int y) : idx_(x * N + y) {
        assert(0 <= x && x < N);
        assert(0 <= y && y < N);
    }
    Point(const PointInternal idx) : idx_(idx) { assert(0 <= idx && idx < N * N); }
    int x() const { return idx_ / N; }  // TODO no use check with assert(false) because slow
    int y() const { return idx_ % N; }  //
    PointInternal raw() const { return idx_; }
    PointInternal idx() const { return idx_; }
    void operator+=(const Point& p) { idx_ += p.idx_; }
    void operator-=(const Point& p) { idx_ -= p.idx_; }
    Point operator+(const Point& p) const { return Point(idx_ + p.idx_); }
    Point operator-(const Point& p) const { return Point(idx_ - p.idx_); }
    void operator+=(const PointInternal d) { idx_ += d; }
    void operator-=(const PointInternal d) { idx_ -= d; }
    Point operator+(const PointInternal d) const { return Point(idx_ + d); }
    Point operator-(const PointInternal d) const { return Point(idx_ - d); }
    auto operator<=>(const Point& p) const = default;
    friend ostream& operator<<(ostream& os, const Point& p) {
        os << p.x() << " " << p.y();
        return os;
    }
};


// vairables

#ifdef HIGH_PERFORMANCE
constexpr int M_MODE2 = 22;
#else
constexpr int M_MODE2 = 12;
#endif

constexpr double eps = 1e-4;

constexpr double MIN_PROB_LOG = -10000;

constexpr int N_MAX = 20;
constexpr int M_MAX = 20;
constexpr int NN_MAX = 400;
int N_GLOBAL, M_GLOBAL;
int M;
int TILE_SUM = 0;
double Eps;
int Eps100;  // make temlate variable?
vector<vector<pii>> Polyominos;

vector<int> X_max;
vector<int> Y_max;

// class

template <int N, typename PointInternal>
class Solver {
    static_assert(10 <= N && N <= 20, "N must be 10 <= N <= 20");
    // static_assert(2 <= M && M <= 20, "M must be 2 <= M <= 20");

    using Point = ::Point<N, PointInternal>;
    using TOverlap = u8;
    using THashBoard = u16;
    using TQueryIndex = u16;
    using TPlaceIndex = PointInternal;

private:
    static constexpr int QUERIES_MAX = N * N * 2;
    static constexpr int NN = N * N;
    static constexpr int NNNN = NN * NN;
    vector<vector<Point>> Polyomions_;
    vector<vector<Point>> AvailablePlaces_;
    vector<array<PointInternal, NN>> AvailablePlaces_inv_;
    vector<int> NAvailablePlaces_;

    // important probabilities
    // static constexpr int offset_ProbKVSi = 50;  //
    // array<array<vector<double>, NN>, NN> ProbKVSi_with_offset_;  // P~(S_m|v,k)

    // array<array<int, NN>, NN> Si_size_;
    // array<array<int, NN>, NN> Si_offset_min_;
    // array<array<int, NN>, NN> Si_offset_max_;
    // vector<array<array<double, NN>, NN>> ProbSiKV_with_offset_;  // P~(S_m|v,k)
    // vector<array<array<double, NN>, NN>> ProbSiKV_with_offset_log_;

    array<int, NNNN> Si_size_;
    array<int, NNNN> Si_offset_min_;
    array<int, NNNN> Si_offset_max_;
    // vector<int> Si_size_;
    // vector<int> Si_offset_min_;
    // vector<int> Si_offset_max_;
    // vector<array<double, NNNN>> ProbSiKV_with_offset_;  // P~(S_m|v,k)
    vector<vector<double>> ProbKVSi_with_offset_;  // P~(S_m|v,k)
    // vector<array<double, NNNN>> ProbSiKV_with_offset_log_;
    vector<double> ProbSiKV_with_offset_log_;

    vector<double> ProbSiKV_entropy_;

    int convert_kv(const int& k, const int& v) const { return k * NN + v; }

    // past queries
    // exclude answer queries
    vector<pair<vector<Point>, TOverlap>> Queries_;
    vector<int> Queries_size_;

    vector<vector<vector<pair<TQueryIndex, TOverlap>>>> MemoDOverlaps_;  // ポリオミノを動かした時の差分計算用
                                                                         // [m][place_index][_] -> {q, dv}
    vector<vector<vector<TOverlap>>> MemoDOverlaps2_;  // ポリオミノを動かした時の差分計算用
                                                       // [m][place_index][q] -> dv

    vector<double> log_table_int_;


    // for sampling
    class Sampler {
        using Solver = ::Solver<N, PointInternal>;

    private:
        int idx_nxt_;
        array<TOverlap, Solver::QUERIES_MAX> overlaps_queries_bef_;  // 初期値 -1

    public:
        // array<TOverlap, NN> overlaps; // deprecated
        // array<array<THashBoard, M_MAX + 1>, NN> hash_bases;
        vector<array<THashBoard, NN>> hash_bases_places;  // [m][place_index] -> hash
        vector<TPlaceIndex> places_indecies;
        Solver* solver;

        vector<TOverlap> overlaps_queries;
        double prob_log;
        THashBoard hash_places;

        // for sampling
        static constexpr int MAX_SAMPLE_MEMO_POWER = 10;
        static constexpr int MAX_SAMPLE_MEMO = 1 << MAX_SAMPLE_MEMO_POWER;
        vector<array<TPlaceIndex, MAX_SAMPLE_MEMO>>
            sample_memos;  // ここから一様にサンプリングすることで偏りのあるサンプリングを実現
        // array<int, N_MAX> sample_memos_sizee
        array<array<int, M_MAX>, NN> sample_memos_cnt;


        Sampler() : places_indecies(M) {}
        void sample(int n_sample) {}

        // array<TOverlap, NN> get_overlaps() const {
        //     array<TOverlap, NN> overlaps;
        //     fill(overlaps.begin(), overlaps.end(), TOverlap(0));
        //     for (int m = 0; m < M; m++) {
        //         const auto q = solver->AvailablePlaces_[m][places_indecies[m]];
        //         for (const auto& p : solver->Polyomions_[m]) {
        //             overlaps[q.raw() + p.raw()]++;
        //         }
        //     }
        //     return overlaps;
        // }

        array<TOverlap, NN> get_overlaps() const { return get_overlaps(places_indecies); }
        array<TOverlap, NN> get_overlaps(const vector<TPlaceIndex>& places_indecies) const {
            array<TOverlap, NN> overlaps;
            fill(overlaps.begin(), overlaps.end(), TOverlap(0));
            for (int m = 0; m < M; m++) {
                const auto q = solver->AvailablePlaces_[m][places_indecies[m]];
                for (const auto& p : solver->Polyomions_[m]) {
                    overlaps[q.raw() + p.raw()]++;
                }
            }
            return overlaps;
        }

        array<TOverlap, NN> get_overlaps_bool(const vector<TPlaceIndex>& places_indecies) const {
            array<TOverlap, NN> overlaps;
            fill(overlaps.begin(), overlaps.end(), TOverlap(0));
            for (int m = 0; m < M; m++) {
                const auto q = solver->AvailablePlaces_[m][places_indecies[m]];
                for (const auto& p : solver->Polyomions_[m]) {
                    overlaps[q.raw() + p.raw()] = 1;
                }
            }
            return overlaps;
        }

        void display() const { display(places_indecies); }
        void display(const vector<TPlaceIndex>& places_indecies) const {
#ifndef ONLINE_JUDGE
#ifdef PRINT_DEBUG
            const auto overlaps = get_overlaps(places_indecies);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    cerr << ((int)overlaps[i * N + j] ? to_string((int)overlaps[i * N + j]) : ".");
                }
                cerr << endl;
            }
            cerr << endl;
#endif
#endif
        }

        void add_last_query() {
            assert((int)overlaps_queries.size() + 1 == (int)solver->Queries_.size());
            overlaps_queries.emplace_back(0);
            for (int m = 0; m < M; m++) {
                if (solver->MemoDOverlaps_[m][places_indecies[m]].empty())
                    continue;
                const auto [q, val] = solver->MemoDOverlaps_[m][places_indecies[m]].back();
                assert(0 <= q && q < (int)overlaps_queries.size());
                if (q == (int)overlaps_queries.size() - 1) {
                    overlaps_queries[q] += val;
                }
            }

#ifdef PRINT_DEBUG
            cerr << "overlaps_queries: " << (int)overlaps_queries.back() << endl;
#endif

            // update prob_log
            {
                assert(overlaps_queries.size() == solver->Queries_.size());
                const auto& [points, s] = solver->Queries_.back();
                const int k = (int)points.size();
                const int v = overlaps_queries.back();
                prob_log += solver->get_ProbKVSi_log(k, v, s);
            }
        }

        void init_overlaps_queries() {
            overlaps_queries.resize(solver->Queries_.size());
            fill(overlaps_queries.begin(), overlaps_queries.end(), TOverlap(0));
            for (int m = 0; m < M; m++) {
                if (solver->MemoDOverlaps_[m][places_indecies[m]].empty())
                    continue;
                for (const auto& [q, val] : solver->MemoDOverlaps_[m][places_indecies[m]]) {
                    overlaps_queries[q] += val;
                }
            }
        }

        void init_sample_memos() {
            sample_memos.resize(N);
            reset_sample_memos();
        }

        void reset_sample_memos() {
            for (int n = 0; n < NN; n++) {
                fill(sample_memos_cnt[n].begin(), sample_memos_cnt[n].end(), 0);
            }
            for (int m = 0; m < M; m++) {
                const int n = solver->NAvailablePlaces_[m];
                int tmp = 0;
                // fill(sample_memos_cnt[m].begin(), sample_memos_cnt[m].end(), 0);
                for (int i = 0; i < MAX_SAMPLE_MEMO; i++) {
                    // sample_memos_cnt[m][tmp]++;
                    sample_memos_cnt[tmp][m]++;
                    sample_memos[m][i] = tmp++;
                    if (tmp == n)
                        tmp = 0;
                }
            }
        }

        double get_alpha(const int& m, const TPlaceIndex& place_index_bef, const TPlaceIndex& place_index_aft) {
            // return 0;
            // const double a = log(sample_memos_cnt[m][place_index_bef]);
            // const double b = log(sample_memos_cnt[m][place_index_aft]);
            // return log(a) - log(b);
            // return log(1. * sample_memos_cnt[m][place_index_bef] / (sample_memos_cnt[m][place_index_aft]));
            // cerr << (int)solver->NAvailablePlaces_[m] << endl;
            // cerr << (int)place_index_bef << " " << (int)place_index_aft << endl;
            // cerr << sample_memos_cnt[m][place_index_bef] << " " << sample_memos_cnt[m][place_index_aft] << endl;
            return solver->log_table_int_[sample_memos_cnt[place_index_bef][m]]
                   - solver->log_table_int_[sample_memos_cnt[place_index_aft][m]];
        }


        // void add_sample(const int& m, const TPlaceIndex& place_index) {
        //     const int n = sample_memos_size[m];
        //     if (n < MAX_SAMPLE_MEMO) {
        //         sample_memos[m][n] = place_index;
        //         sample_memos_size[m]++;
        //     }
        // }

        TPlaceIndex get_random_place_index_biased(const int& m) {
            // const int n = sample_memos_size[m];
            // assert(n >= (int)solver->NAvailablePlaces_[m]);
            // const int i = rng.range_random(n);
            // const int i = rng.range_random_(MAX_SAMPLE_MEMO);
            // return sample_memos[m][i];
            // assert(MAX_SAMPLE_MEMO == (1 << 16));
            // return sample_memos[m][rng.uint_random_65535_unsafe()];
            return sample_memos[m][rng.uint_random_65535_unsafe() >> (16 - MAX_SAMPLE_MEMO_POWER)];
        }

        void add_sample_memo(const int& m, const TPlaceIndex& place_index) {
            // const int n = sample_memos_size[m];
            // if (n < MAX_SAMPLE_MEMO) {
            //     sample_memos[m][n] = place_index;
            //     sample_memos_size[m]++;
            // } else {
            //     const int i = rng.range_random(MAX_SAMPLE_MEMO);
            //     sample_memos[m][i] = place_index;
            // }

            // return;

            const int i = rng.uint_random_65535_unsafe() >> (16 - MAX_SAMPLE_MEMO_POWER);
            if (sample_memos_cnt[sample_memos[m][i]][m] <= 2) {
                return;
            }
            // cerr << "add " << (int)sample_memos[m][i] << " -> " << (int)place_index << endl;
            // cerr << sample_memos_cnt[m][sample_memos[m][i]] << " " << sample_memos_cnt[m][place_index] << endl;
            sample_memos_cnt[sample_memos[m][i]][m]--;
            sample_memos_cnt[place_index][m]++;
            sample_memos[m][i] = place_index;
        }

        double get_prob_log_naive() {
            array<TOverlap, NN> overlaps = get_overlaps();
            double prob_log = 0;
            for (int q = 0; q < (int)solver->Queries_.size(); q++) {
                const auto& [points, s] = solver->Queries_[q];
                const int k = (int)points.size();
                // const int v = overlaps[q];
                TOverlap v = 0;
                for (const auto& p : points) {
                    v += overlaps[p.raw()];
                }
                prob_log += solver->get_ProbKVSi_log(k, v, s);
            }
            return prob_log;
        }

        double get_prob_log() const { return get_prob_log(places_indecies); }
        double get_prob_log(const vector<TPlaceIndex>& places_indecies) const {
            // double get_prob_log() const {
            // log(P(B|queries))
            // calculate overlaps_queries
            const int n_queries = solver->Queries_.size();
            vector<TOverlap> overlaps_queries(n_queries);
            for (int m = 0; m < M; m++) {
                const auto& place_index = places_indecies[m];
                for (const auto& [q, val] : solver->MemoDOverlaps_[m][place_index]) {
                    overlaps_queries[q] += val;
                    // cerr << "m: " << (int)m << " q: " << (int)q << " val: " << (int)val << endl;
                }
            }

            // calculate prob_log
            double prob_log = 0;
            for (int q = 0; q < n_queries; q++) {
                const auto& [points, s] = solver->Queries_[q];
                const int k = (int)points.size();
                const int v = overlaps_queries[q];
                prob_log += solver->get_ProbKVSi_log(k, v, s);
            }
            return prob_log;
        }
        double get_prob_log_last(const vector<TPlaceIndex>& places_indecies) const {
            // caulcate constribution of last query
            // calculate overlaps_queries
            const int n_queries = solver->Queries_.size();
            const int q = n_queries - 1;
            assert(n_queries >= 1);
            TOverlap overlap_query = 0;
            for (int m = 0; m < M; m++) {
                const auto& place_index = places_indecies[m];
                overlap_query += solver->MemoDOverlaps2_[m][place_index][q];
            }

            // calculate prob_log
            const auto& [points, s] = solver->Queries_[q];
            const int k = (int)points.size();
            const int v = overlap_query;
            return solver->get_ProbKVSi_log(k, v, s);
        }

        void update_prob_log() {
            // log(P(B|queries))
            assert(overlaps_queries.size() == solver->Queries_.size());
            init_overlaps_queries();
            prob_log = 0;
            for (int q = 0; q < (int)overlaps_queries.size(); q++) {
                const auto& [points, s] = solver->Queries_[q];
                const int k = (int)points.size();
                const int v = overlaps_queries[q];
                prob_log += solver->get_ProbKVSi_log(k, v, s);
            }
            // cerr << prob_log << " " << get_prob_log_naive() << endl;
            // assert(prob_log < -1000 || abs(prob_log - get_prob_log_naive()) < 1e-6);
        }


        bool step2() {
            // change 2 places
            const auto m1 = idx_nxt_;
            int m2 = rng.range_random_65535_unsafe(M - 1);
            if (m2 >= m1)
                m2++;

            // select place
            // const TPlaceIndex place_index_new1 = solver->get_random_place_index(m1);
            // const TPlaceIndex place_index_new2 = solver->get_random_place_index(m2);
            const TPlaceIndex place_index_new1 = get_random_place_index_biased(m1);
            const TPlaceIndex place_index_new2 = get_random_place_index_biased(m2);

            // simple implementation
            auto places_indecies_new = places_indecies;
            places_indecies_new[m1] = place_index_new1;
            places_indecies_new[m2] = place_index_new2;
            const double prob_log_new = get_prob_log(places_indecies_new);
            const double dprob_log = prob_log_new - prob_log;
            const double alpha1 = get_alpha(m1, places_indecies[m1], place_index_new1);
            const double alpha2 = get_alpha(m2, places_indecies[m2], place_index_new2);
            // const bool flag_update = (dprob_log >= 0.) || (dprob_log > log_random());
            const bool flag_update
                = (dprob_log + alpha1 + alpha2 >= 0.) || (dprob_log + alpha1 + alpha2 > log_random());
            if (flag_update) {
                hash_places ^= hash_bases_places[m1][places_indecies[m1]] ^ hash_bases_places[m1][place_index_new1]
                               ^ hash_bases_places[m2][places_indecies[m2]] ^ hash_bases_places[m2][place_index_new2];
                places_indecies = places_indecies_new;
                prob_log = prob_log_new;
                init_overlaps_queries();
                add_sample_memo(m1, place_index_new1);
                add_sample_memo(m2, place_index_new2);
            }
            return flag_update;
        }

        bool step2_ver2() {
            // change 2 places
            const auto m1 = idx_nxt_;
            int m2 = rng.range_random_65535_unsafe(M - 1);
            if (m2 >= m1)
                m2++;

            // select place
            // const TPlaceIndex place_index_new1 = solver->get_random_place_index(m1);
            // const TPlaceIndex place_index_new2 = solver->get_random_place_index(m2);
            const TPlaceIndex place_index_new1 = get_random_place_index_biased(m1);
            const TPlaceIndex place_index_new2 = get_random_place_index_biased(m2);

            // simple implementation
            auto places_indecies_new = places_indecies;
            places_indecies_new[m1] = place_index_new1;
            places_indecies_new[m2] = place_index_new2;
            // const double prob_log_new = get_prob_log(places_indecies_new);
            // const double dprob_log = prob_log_new - prob_log;

            double dprob_log = 0;
            for (int q = 0; q < (int)solver->Queries_.size(); q++) {
                const auto& [points, s] = solver->Queries_[q];
                // const int k = solver->Queries_size_[q];
                const int k = (int)points.size();
                const auto dv = solver->MemoDOverlaps2_[m1][place_index_new1][q]
                                - solver->MemoDOverlaps2_[m1][places_indecies[m1]][q]
                                + solver->MemoDOverlaps2_[m2][place_index_new2][q]
                                - solver->MemoDOverlaps2_[m2][places_indecies[m2]][q];
                if (dv == 0)
                    continue;
                const TOverlap v_bef = overlaps_queries[q];
                dprob_log += solver->get_ProbKVSi_log(k, v_bef + dv, s) - solver->get_ProbKVSi_log(k, v_bef, s);
            }

            const double alpha1 = get_alpha(m1, places_indecies[m1], place_index_new1);
            const double alpha2 = get_alpha(m2, places_indecies[m2], place_index_new2);
            // const bool flag_update = (dprob_log >= 0.) || (dprob_log > log_random());
            const bool flag_update
                = (dprob_log + alpha1 + alpha2 >= 0.) || (dprob_log + alpha1 + alpha2 > log_random());
            if (flag_update) {
                hash_places ^= hash_bases_places[m1][places_indecies[m1]] ^ hash_bases_places[m1][place_index_new1]
                               ^ hash_bases_places[m2][places_indecies[m2]] ^ hash_bases_places[m2][place_index_new2];
                // places_indecies = places_indecies_new;
                for (int q = 0; q < (int)solver->Queries_.size(); q++) {
                    overlaps_queries[q] += solver->MemoDOverlaps2_[m1][place_index_new1][q]
                                           - solver->MemoDOverlaps2_[m1][places_indecies[m1]][q]
                                           + solver->MemoDOverlaps2_[m2][place_index_new2][q]
                                           - solver->MemoDOverlaps2_[m2][places_indecies[m2]][q];
                }
                prob_log += dprob_log;
                places_indecies[m1] = place_index_new1;
                places_indecies[m2] = place_index_new2;
                // init_overlaps_queries();
                add_sample_memo(m1, place_index_new1);
                add_sample_memo(m2, place_index_new2);
            }
            return flag_update;
        }

        bool step_ver2() {
            inc_cyclic(idx_nxt_, M);

            // if (rng.bool_random_unsafe<0.05f>()) {
            //     return step2_ver2();
            // }

            const TPlaceIndex place_index_new = get_random_place_index_biased(idx_nxt_);
            if (places_indecies[idx_nxt_] == place_index_new) {
                return false;
            }

            const int n_queries = (int)solver->Queries_.size();

            double dprob_log = 0;
            for (int q = 0; q < n_queries; q++) {
                const auto& [points, s] = solver->Queries_[q];
                const int k = solver->Queries_size_[q];
                // const int k = (int)points.size();
                const auto dv = solver->MemoDOverlaps2_[idx_nxt_][place_index_new][q]
                                - solver->MemoDOverlaps2_[idx_nxt_][places_indecies[idx_nxt_]][q];
                if (dv == 0)
                    continue;
                const TOverlap v_bef = overlaps_queries[q];
                // dprob_log += solver->get_ProbKVSi_log(k, v_bef + dv, s) - solver->get_ProbKVSi_log(k, v_bef, s);
                const auto kv_bef = solver->convert_kv(k, v_bef);
                dprob_log += solver->get_ProbKVSi_log(kv_bef + dv, s) - solver->get_ProbKVSi_log(kv_bef, s);
            }

            const double alpha = get_alpha(idx_nxt_, places_indecies[idx_nxt_], place_index_new);
            bool flag_update = (dprob_log + alpha >= 0.) || (dprob_log + alpha > log_random());

            if (flag_update) {
                hash_places ^= hash_bases_places[idx_nxt_][places_indecies[idx_nxt_]]
                               ^ hash_bases_places[idx_nxt_][place_index_new];
                for (int q = 0; q < n_queries; q++) {
                    overlaps_queries[q] += solver->MemoDOverlaps2_[idx_nxt_][place_index_new][q]
                                           - solver->MemoDOverlaps2_[idx_nxt_][places_indecies[idx_nxt_]][q];
                }
                prob_log += dprob_log;
                places_indecies[idx_nxt_] = place_index_new;
                add_sample_memo(idx_nxt_, place_index_new);
            } else {
            }

            return flag_update;
        }

        bool step(double temp = 1.) {
            // TODO hash を用いた遷移制限

            // if (M < M_MODE2)
            return step_ver2();

            inc_cyclic(idx_nxt_, M);

            // if (rng.bool_random<0.99f>())
            //     return step2();

            // select place
            // const TPlaceIndex place_index_new = solver->get_random_place_index(idx_nxt_);
            const TPlaceIndex place_index_new = get_random_place_index_biased(idx_nxt_);
            if (places_indecies[idx_nxt_] == place_index_new) {
                return false;
            }

            // {
            //     static int cnt = 0, cnt_total = 0;
            //     cnt_total++;
            //     if (solver->Queries_.size() >= solver->MemoDOverlaps_[idx_nxt_][places_indecies[idx_nxt_]].size()
            //                                        + solver->MemoDOverlaps_[idx_nxt_][place_index_new].size()) {
            //         cnt++;
            //     }
            //     if (cnt_total % 50000 == 0) {
            //         cerr << "cnt: " << cnt << " " << cnt_total << " " << (double)cnt / cnt_total << endl;
            //     }
            // }

            // set of queries for updateing overlaps
            // must take sum of sets
            static int iter = 0;
            iter++;
            static array<int, Solver::QUERIES_MAX> indexset_seen;
            static array<TQueryIndex, Solver::QUERIES_MAX> indexset_arr;
            static array<TOverlap, Solver::QUERIES_MAX> overlaps_queries_new;
            int indexset_size = 0;

            // erase queries
            for (const auto& [q, val] : solver->MemoDOverlaps_[idx_nxt_][places_indecies[idx_nxt_]]) {
                indexset_seen[q] = iter;
                indexset_arr[indexset_size++] = q;
                overlaps_queries_new[q] = overlaps_queries[q] - val;
            }

            // add queries
            for (const auto& [q, val] : solver->MemoDOverlaps_[idx_nxt_][place_index_new]) {
                if (indexset_seen[q] != iter) {
                    indexset_arr[indexset_size++] = q;
                    overlaps_queries_new[q] = overlaps_queries[q] + val;
                } else {
                    overlaps_queries_new[q] += val;
                }
            }

            // update query
            // TODO 情報の持ち方を変えて定数倍高速化でそう <- done?
            double dprob_log = 0;
            for (int i = 0; i < indexset_size; i++) {
                const auto q = indexset_arr[i];
                const TOverlap v_bef = overlaps_queries[q];
                const TOverlap v = overlaps_queries_new[q];
                if (v == v_bef)
                    continue;
                const auto& [points, s] = solver->Queries_[q];
                // const int k = (int)points.size();
                const int k = solver->Queries_size_[q];
                dprob_log += solver->get_ProbKVSi_log(k, v, s) - solver->get_ProbKVSi_log(k, v_bef, s);
                // if (dprob_log < MIN_PROB_LOG)
                //     break;
            }

            const double alpha = get_alpha(idx_nxt_, places_indecies[idx_nxt_], place_index_new);
            bool flag_update = (dprob_log + alpha >= 0.) || (dprob_log + alpha > log_random());

            // bool flag_update
            //     = (dprob_log < MIN_PROB_LOG ? false : (dprob_log >= 0.) || (dprob_log > log_random() * temp));
            // bool flag_update = dprob_log >= 0;

            // const bool flag_update = dprob_log >= 0;
            // flag_update |= rng.bool_random_unsafe<0.02f>();

            if (flag_update) {
                hash_places ^= hash_bases_places[idx_nxt_][places_indecies[idx_nxt_]]
                               ^ hash_bases_places[idx_nxt_][place_index_new];
                places_indecies[idx_nxt_] = place_index_new;
                prob_log += dprob_log;
                for (int i = 0; i < indexset_size; i++) {
                    const auto q = indexset_arr[i];
                    overlaps_queries[q] = overlaps_queries_new[q];
                }
                add_sample_memo(idx_nxt_, place_index_new);
            } else {
            }

            return flag_update;
        }

        void init() {
            idx_nxt_ = rng.range_random_safe(M);
            fill(overlaps_queries_bef_.begin(), overlaps_queries_bef_.end(), TOverlap(-1));
            hash_bases_places.resize(M);
            for (int m = 0; m < M; m++) {
                for (int i = 0; i < NN; i++) {
                    hash_bases_places[m][i] = rng.uint_random();
                }
            }
            init_sample_memos();
            init_places();
            // init_overlaps_queries(); // called inside update_prob_log
        }

        THashBoard get_hash(const vector<TPlaceIndex>& places_indecies) {
            hash_places = 0;
            for (int m = 0; m < M; m++) {
                hash_places ^= hash_bases_places[m][places_indecies[m]];
            }
            return hash_places;
        }

        void init_hash_prob() {
            hash_places = 0;
            for (int m = 0; m < M; m++) {
                hash_places ^= hash_bases_places[m][places_indecies[m]];
            }
            update_prob_log();
        }

        void init_places() {
            hash_places = 0;
            for (int m = 0; m < M; m++) {
                places_indecies[m] = solver->get_random_place_index(m);
                // places_indecies[m] = get_random_place_index_biased(m);
                hash_places ^= hash_bases_places[m][places_indecies[m]];
            }
            update_prob_log();
        }

        // void init_overlaps() {
        //     fill(overlaps.begin(), overlaps.end(), TOverlap(0));
        //     for (int m = 0; m < M; m++) {
        //         const auto q = solver->AvailablePlaces_[m][places_indies_[m]];
        //         for (const auto& p : solver->Polyomions_[m]) {
        //             // overlaps[places[m].raw() + p.raw()]++;
        //             overlaps[q.raw() + p.raw()]++;
        //         }
        //     }
        // }

        // THashBoard get_hash_board() {
        //     assert(false);
        //     THashBoard hash = 0;
        //     for (int i = 0; i < NN; i++) {
        //         hash ^= hash_bases[i][overlaps[i]];
        //     }
        //     return hash;
        // }
    };
    const int SAMPLERS_ = 1;  // 何並列でMCMCするか

    vector<Sampler> samplers_;

public:
    Solver() {
#ifdef PRINT_DEBUG
        cerr << " construct Solver" << endl;
#endif
        assert(N == N_GLOBAL);
        assert(M == M_GLOBAL);
    }

    double do_query_random() {
        // int k = rng.range_random_safe(3, 10);
        // int k = rng.range_random_safe(NN - 10, NN);
        // int k = rng.range_random_safe(40, 50);

        vector<Point> mask;

        int k = NN * 0.5;
        for (int i = 0; i < k; i++) {
            mask.emplace_back(rng.range_random_safe(NN));
        }
        sort(mask.begin(), mask.end());
        mask.erase(unique(mask.begin(), mask.end()), mask.end());
        k = (int)mask.size();

        // int k = NN * 0.35;
        // mask.resize(NN);
        // iota(mask.begin(), mask.end(), 0);
        // for (int i = 0; i < NN - k; i++) {
        //     int idx = rng.range_random_safe(NN - i);
        //     swap(mask[idx], mask[NN - 1 - i]);
        // }
        // mask.resize(k);

        ask(mask);
        double cost = 1. / sqrt(k);
        return cost;
    }

    double do_query_random2() {
        int dx = 5;
        int dy = 5;
        int x = rng.range_random_safe(N - dx + 1);
        int y = rng.range_random_safe(N - dy + 1);
        vector<Point> mask;
        for (int xx = 0; xx < dx; xx++) {
            for (int yy = 0; yy < dy; yy++) {
                if (rng.bool_random_safe(0.5))
                    mask.emplace_back(x + xx, y + yy);
            }
        }
        if (mask.size() == 0)
            mask.emplace_back(x, y);
        sort(mask.begin(), mask.end());
        mask.erase(unique(mask.begin(), mask.end()), mask.end());
        int k = (int)mask.size();
        ask(mask);
        double cost = 1. / sqrt(k);
        return cost;
    }

    // double do_query_hillclimbing_ver1(const vector<array<TOverlap, NN>>& memo_overlaps) {
    double do_query_hillclimbing_ver1() {
        // if (rng.bool_random_unsafe<0.5>())
        return do_query_random();
        // return do_query_random2();

        /*
        int n_memo = (int)memo_overlaps.size();

        if (n_memo == 0) {
            assert(false);
            return do_query_random();
        }

        vector<double> prob_logs(n_memo);
        vector<double> probs(n_memo);
        for (int i = 0; i < n_memo; i++) {
            copy(memo_overlaps[i].begin(), memo_overlaps[i].end(), samplers_[0].overlaps.begin());
            prob_logs[i] = samplers_[0].get_prob_log();
        }
        const double prob_log_max = *max_element(prob_logs.begin(), prob_logs.end());
        for (int i = 0; i < n_memo; i++) {
            probs[i] = exp(prob_logs[i] - prob_log_max);
        }
        double prob_sum = accumulate(probs.begin(), probs.end(), 0.);
        for (int i = 0; i < n_memo; i++) {
            probs[i] /= prob_sum;
        }

        // prob cutoff
        const double prob_cutoff = 1. / (n_memo * 2);
        prob_sum = 0;
        for (int i = 0; i < n_memo; i++) {
            if (probs[i] < prob_cutoff) {
                probs[i] = -100;
            } else
                prob_sum += probs[i];
        }

        for (int i = 0; i < n_memo; i++) {
            if (probs[i] > 0)
                probs[i] /= prob_sum;
            if (probs[i] > 0.01)
                cerr << "probs[" << i << "] = " << probs[i] << endl;
        }


        auto calc_score = [&](const vector<Point>& mask) {
            double term1 = 0;
            double term2 = 0;

            int k = (int)mask.size();

            vector<double> p_v(TILE_SUM + 1);
            for (int i = 0; i < n_memo; i++) {
                if (probs[i] < 0)
                    continue;
                const auto& overlaps = memo_overlaps[i];
                int v = 0;
                for (const auto& p : mask) {
                    v += overlaps[p.raw()];
                }
                p_v[v] += probs[i];

                vector<double> p_s(NN);
                for (int s = 0; s < NN; s++) {
                    p_s[s] = get_ProbKVSi(k, v, s);
                }
                // normalize(p_s);
                term1 += probs[i] * calc_entropy(p_s);
            }
            vector<double> p_s(NN);
            for (int v = 0; v <= TILE_SUM; v++) {
                for (int s = 0; s < NN; s++) {
                    p_s[s] += p_v[v] * get_ProbKVSi(k, v, s);
                }
            }
            // cerr << accumulate(p_s.begin(), p_s.end(), 0.) << endl;
            // normalize(p_s);
            term2 += calc_entropy(p_s);

            const double ret = (-term1 + term2) * sqrt(k);
            // const double ret = (-term1 + term2);

            // cerr << "ret = " << ret << endl;

            if (ret < -1e-7) {
                cerr << "ret = " << ret << endl;
                assert(false);
            }

            // return 1.;
            return ret;
        };

        // return do_query_random();

        vector<Point> mask;
        double best_score = -1;

        if (1) {
            int k = rng.range_random_safe(2, 5);
            for (int i = 0; i < k; i++) {
                mask.emplace_back(rng.range_random_safe(NN));
            }
            sort(mask.begin(), mask.end());
            mask.erase(unique(mask.begin(), mask.end()), mask.end());
            k = (int)mask.size();

            // for (int i = 0; i < NN; i++) {
            //     if (rng.bool_random_safe(1. / 2))
            //         mask.emplace_back(i);
            // }
        }

        const int max_size_mask = 10;

        if (1)
            for (int i = 0; i < 1000; i++) {
                PointInternal idx = rng.range_random_safe(NN);
                bool flag_include = false;
                for (const auto& p : mask) {
                    if (p.raw() == idx) {
                        flag_include = true;
                        break;
                    }
                }
                if (flag_include) {
                    if (mask.size() == 1)
                        continue;
                    if (mask.size() <= 3)
                        continue;
                    mask.erase(remove(mask.begin(), mask.end(), Point(idx)), mask.end());
                    double score = calc_score(mask);
                    if (chmax(best_score, score)) {
                        cerr << "best_score: " << best_score << endl;
                    } else {
                        mask.emplace_back(idx);
                    }
                } else {
                    if (mask.size() == NN - 1)
                        continue;
                    if (rng.bool_random_safe(0.5)) {
                        // 追加
                        if (mask.size() >= max_size_mask)
                            continue;
                        mask.emplace_back(idx);
                        double score = calc_score(mask);
                        if (chmax(best_score, score)) {
                            // cerr << "best_score: " << best_score << endl;
                        } else {
                            mask.pop_back();
                        }
                    } else {
                        // 変更
                        if (mask.size() == 0)
                            continue;
                        int i = rng.range_random_safe(mask.size());
                        const auto idx_bef = mask[i];
                        mask[i] = Point(idx);
                        double score = calc_score(mask);
                        if (chmax(best_score, score)) {
                            // cerr << "best_score: " << best_score << endl;
                        } else {
                            mask[i] = idx_bef;
                        }
                    }
                }
            }

        // if (mask.size() <= 100)
        samplers_[0].init_overlaps();
        // return do_query_random();


        // print mask
        cerr << "mask size = " << mask.size() << endl;
        for (auto p : mask) {
            cerr << p.x() << " " << p.y() << "  ";
        }
        cerr << endl;

        // return do_query_random();
        ask(mask);
        return 1. / sqrt(mask.size());
        */
    }

    void solve() {
#ifdef PRINT_DEBUG
        cerr << " start Solver.solve()" << endl;
#endif
        init();
        double cost = 0;
        // vector<array<TOverlap, NN>> memo_overlaps;
        // unordered_set<THashBoard> memo_hash_boards;

        using P = tuple<vector<TPlaceIndex>, THashBoard, double>;
        const int n_max_remember_max = 2000;
        const int n_max_remember = [&]() {
            if (N == 10)
                return 2000;
            else if (N == 11)
                return 1500;
            else
                return 1000;
        }();
        int n_candidates = 10;
        int n_moves = 5000;
        if (M == 2) {
            n_candidates = 200;
            n_moves = 500;
        } else if (M == 3) {
            n_candidates = 200;
            n_moves = 4000;
        } else if (M == 4) {
            n_candidates = 100;
            n_moves = 2000;
        } else if (M == 5) {
            n_candidates = 200;
            n_moves = 1000;
        } else if (M == 6) {
            n_candidates = 100;
            n_moves = 1000;
        } else if (M == 7) {
            n_candidates = 100;
            n_moves = 1000;
        } else if (M == 8) {
            n_candidates = 40;
            n_moves = 2000;
        } else if (M == 9) {
            n_candidates = 40;
            n_moves = 2000;
        } else if (M == 10) {
            n_candidates = 10;
            n_moves = 4000;
        } else if (M == 11) {
            n_candidates = 20;
            n_moves = 2000;
        } else if (M == 12) {
            n_candidates = 10;
            n_moves = 1000;
        } else if (M == 13) {
            n_candidates = 10;
            n_moves = 2000;
        } else if (M == 14) {
            n_candidates = 20;
            n_moves = 2000;
        } else if (M == 15) {
            n_candidates = 5;
            n_moves = 4000;
        } else if (M == 16) {
            n_candidates = 5;
            n_moves = 4000;
        } else if (M == 17) {
            n_candidates = 5;
            n_moves = 4000;
        } else if (M == 18) {
            n_candidates = 10;
            n_moves = 1000;
        } else if (M == 19) {
            n_candidates = 5;
            n_moves = 1000;
        } else if (M == 20) {
            n_candidates = 5;
            n_moves = 1000;
        } else {
            assert(false);
        }

#ifdef HIGH_PERFORMANCE
        n_candidates = 500;
        n_moves = 10000;
#endif

        // n_candidates = 5;
        // n_moves = 4000;

        if (M >= M_MODE2) {
            n_candidates *= 2;
        }


        if (1) {
            int c = 0;
            if (N == 10)
                c = 8;
            // else if (N == 14)
            //     c = 7;
            // else if (N == 16)
            //     c = 5;
            // else if (N >= 18)
            //     c = 9;

            if (c == 5) {
                double ratio = pow((M / 10.), 1.3);
                n_candidates *= ratio;
                n_moves *= ratio;
            } else if (c == 7) {
                double ratio = pow((M / 15.), -0.5);
                n_candidates *= ratio;
                n_moves *= ratio;
            } else if (c == 8) {
                double ratio = pow((M / 10.), 0.5);
                n_candidates = n_candidates / ratio;
                n_moves = n_moves / ratio;
            } else if (c == 9) {
                double ratio = pow(M / 20., 1);
                n_candidates = n_candidates / ratio;
            }
        }

        const int n_candidates_original = n_candidates;
        const int n_moves_original = n_moves;

        ObjectPool<P, n_max_remember_max> pool_places;
        // priority_queue<pair<double, int>> que;
        // using PQue = priority_queue<pair<double, int>>;
        // using PQue = priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>;
        using PQue = MyPriorityQueue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>;
        PQue que;
        static_assert(is_same_v<THashBoard, u16>);

        vector<THashBoard> memo_wrong_hash;

        bool flag_last_update = false;

        // samplers_[0].init_sample_memos();

        // remember all possible places if M == 2
        // if (M == 2) {
        //     n_max_remember = 0;
        //     n_candidates = 0;
        //     n_moves = 0;
        //     for (int i0 = 0; i0 < (int)AvailablePlaces_[0].size(); i0++) {
        //         for (int i1 = 0; i1 < (int)AvailablePlaces_[1].size(); i1++) {
        //             n_max_remember++;
        //             vector<TPlaceIndex> places_indecies = {TPlaceIndex(i0), TPlaceIndex(i1)};
        //             const auto hash = samplers_[0].get_hash(places_indecies);
        //             const double prob_log = samplers_[0].get_prob_log(places_indecies);
        //             que.emplace(prob_log, pool_places.emplace(places_indecies, hash, prob_log));
        //         }
        //     }
        // }


        for (int n_query = 0; n_query < QUERIES_MAX; n_query++) {
#ifdef WA_WHEN_WRONG
            if (get_time_remaining() < 0 && n_query == QUERIES_MAX - 1) {
                cout << "wrong answer :(" << endl;
            }
#endif

            // dynamic
            {
                // ver 1
                if (get_time_remaining_ratio() < 0.3) {
                    n_candidates = (n_candidates_original + 4) / 5;
                } else {
                    n_candidates = n_candidates_original * 1.5;
                }
                // ver 2
                // if (get_time_remaining_ratio() < 0.3) {
                //     n_candidates = (n_candidates_original + 4) / 5;
                // } else {
                //     n_candidates = n_candidates_original * 3;
                // }
                // ver 3
                // if (get_time_remaining_ratio() < 0.5) {
                //     n_candidates = n_candidates_original;
                // } else {
                //     n_candidates = n_candidates_original * 3;
                // }
            }

            // samplers_[0].reset_sample_memos();

            print_data("n_query", n_query);
            print_time();
#ifdef PRINT_DEBUG
            cerr << "cost: " << cost << endl;
#endif
#ifdef CHECK_TIME
            if (get_time_remaining() < 0) {
                // if (M != 3) {
                // TODO emergency mode


                // no more push to que
                static vector<int> que_seen_idx(que.size());
                static int n_seen = 0;

                if (n_seen == (int)que.size()) {
                    // if (que.empty()) {
                    // if (1) {
                    ask({0});
                } else {
                    int idx_pool = -1;

                    // idx_pool = que.top().second;
                    // que.pop();

                    // take bottom in que

                    int idx_best = -1;
                    double prob_log_max = MIN_PROB_LOG;

                    for (int i = 0; i < que.size(); i++) {
                        if (que_seen_idx[i])
                            continue;
                        auto [prob_log, idx_pool_] = que.c[i];
                        if (chmax(prob_log_max, prob_log)) {
                            idx_pool = idx_pool_;
                            idx_best = i;
                        }
                    }

                    if (idx_best == -1) {
                        ask({0});
                        continue;
                    }

                    que_seen_idx[idx_best] = 1;
                    n_seen++;

                    // if (1) {
                    //     PQue que_new;
                    //     while (que.size()) {
                    //         auto [prob_log, idx_pool_] = que.top();
                    //         que.pop();
                    //         if (que.size() == 0)
                    //             idx_pool = idx_pool_;
                    //         else
                    //             que_new.emplace(prob_log, idx_pool_);
                    //     }
                    //     que = move(que_new);
                    // }
                    const auto [places, hash, prob_log] = pool_places[idx_pool];
                    if (answer(get_points(places)))
                        return;
                    cost += 1;
                    // pool_places.pop(idx_pool);
                }
                continue;
            }
#endif

            if (M >= M_MODE2 && n_query < int(NN) - 1) {
                cost += do_query_random();
                continue;
            }
            // if (n_query < int(NN) - 1) {
            //     n_candidates = 1;
            // }


            if (M >= 5) {
                samplers_[0].reset_sample_memos();
                for (int i = 0; i < que.size(); i++) {
                    auto [prob_log, idx_pool] = que.c[i];
                    for (int m = 0; m < M; m++) {
                        const auto& [places, hash, prob_log] = pool_places[idx_pool];
                        samplers_[0].add_sample_memo(m, places[m]);
                    }
                }
            }

#ifdef PRINT_DEBUG
            for (int m = 0; m < M; m++) {
                // print sample_memo_cnt
                for (int i = 0; i < NN; i++) {
                    cerr << samplers_[0].sample_memos_cnt[i][m] << " ";
                }
                cerr << endl;
            }
#endif


            // 確率が高い配置を覚える
            vector<double> prob_logs_hash_indexed(1 << 16, -100000);
            for (const auto& hash : memo_wrong_hash) {
                prob_logs_hash_indexed[hash] = 10;
            }
            // if (flag_last_update) {
            if (true) {
                PQue que_new;
                // while (!que.empty()) {
                //     auto [prob_log_bef, idx_pool] = que.top();
                for (int i = 0; i < que.size(); i++) {
                    auto [prob_log_bef, idx_pool] = que.c[i];
                    // que.pop();

                    // cerr << (int)que.size() << " " << pool_places.size() << " " <<
                    // pool_places.garbage_index_.size()
                    //      << endl;
                    auto& [places, hash, prob_log] = pool_places[idx_pool];
                    if (prob_logs_hash_indexed[hash] > 1) {
                        pool_places.pop(idx_pool);
                        continue;
                    }
                    // prob_log = samplers_[0].get_prob_log(places);
                    // que_new.emplace(prob_log, idx_pool);
                    // chmax(prob_logs_hash_indexed[hash], prob_log);
                    if (1) {
                        // const double prob_log_true = samplers_[0].get_prob_log(places);
                        // const double prob_log_new = prob_log_bef + samplers_[0].get_prob_log_last(places);
                        if (flag_last_update)
                            prob_log = prob_log_bef + samplers_[0].get_prob_log_last(places);
                        // prob_log = prob_log_true;
                        que_new.emplace(prob_log, idx_pool);
                        chmax(prob_logs_hash_indexed[hash], prob_log);
                        // if (abs(prob_log_true - prob_log) > 1e-9) {
                        //     cerr << "prob_log: " << prob_log << " prob_log_true: " << prob_log_true
                        //          << " prob_log_bef: " << prob_log_bef
                        //          << " prob_log_last: " << samplers_[0].get_prob_log_last(places) << endl;
                        //     assert(false);
                        // }
                    }
                }
                que = move(que_new);
            }

            // cerr << "prob_log: " << samplers_[0].get_prob_log() << endl;
            // cerr << "prob_log_correct: " << samplers_[0].get_prob_log_naive() << endl;
            // if (M == 2 || Queries_.size() % 5 == 0)


            if (0) {
                // 近傍探索
                const auto pool_places_bef = pool_places;
                pool_places.clear();
                PQue que_new = que;
                while (!que.empty()) {
                    // cerr << (int)que.size() << " " << pool_places.size() << " " << pool_places.garbage_index_.size()
                    //      << endl;
                    auto [prob_log_bef, idx_pool] = que.top();
                    que.pop();
                    // cerr << (int)que.size() << " " << pool_places.size() << " " <<
                    // pool_places.garbage_index_.size()
                    //      << endl;
                    auto [places, hash, prob_log] = pool_places_bef[idx_pool];
                    for (int m1 = 0; m1 < M; m1++) {
                        // for (int m2 = m1 + 1; m2 < M; m2++) {
                        for (int i1 = 0; i1 < NAvailablePlaces_[m1]; i1++) {
                            // for (int i2 = 0; i2 < NAvailablePlaces_[m2]; i2++) {
                            auto places_new = places;
                            places_new[m1] = TPlaceIndex(i1);
                            // places_new[m2] = TPlaceIndex(i2);
                            const auto hash_new = samplers_[0].get_hash(places_new);
                            const auto prob_log_new = samplers_[0].get_prob_log(places_new);
                            if (prob_logs_hash_indexed[hash_new] + eps < prob_log_new) {
                                prob_logs_hash_indexed[hash_new] = prob_log_new;
                                if (que_new.size() < n_max_remember || prob_log_new > que_new.top().first + eps) {
                                    if (que_new.size() == n_max_remember) {
                                        const auto [prob_log, idx_pool] = que_new.top();
                                        que_new.pop();
                                        pool_places.pop(idx_pool);
                                    }
                                    que_new.emplace(
                                        prob_log_new, pool_places.emplace(places_new, hash_new, prob_log_new));
                                }
                            }
                            // }
                        }
                        // }
                    }
                }
                que = move(que_new);
            }

            if (M <= 3 && n_query == 0) {
                // if (0) {
                for (int i = 0; i < n_max_remember; i++) {
                    samplers_[0].init_places();
                    const double& prob_log = samplers_[0].prob_log;
                    const auto& hash = samplers_[0].hash_places;
                    if (prob_logs_hash_indexed[hash] + eps < prob_log) {
                        prob_logs_hash_indexed[hash] = prob_log;
                        if (que.size() < n_max_remember || prob_log > que.top().first + eps) {
                            que.emplace(prob_log, pool_places.emplace(samplers_[0].places_indecies, hash, prob_log));
                        }
                    }
                }
            } else {
                // const bool flag_emergency = (M >= 10);
                // const bool flag_emergency = false;


                int cnt_total = n_candidates * n_moves;
                int cnt_update = 0;
                for (int i = 0; i < n_candidates; i++) {
                    int cnt = 0;
                    // if (que.size() >= n_max_remember / 2 && rng.bool_random_unsafe<0.1f>()) {
                    //     auto [prob_log, idx_pool] = que.c[rng.range_random_65535_unsafe(que.size())];
                    //     auto& [places, hash, prob_log_] = pool_places[idx_pool];
                    //     samplers_[0].places_indecies = places;
                    //     samplers_[0].init_hash_prob();
                    // } else {
                    //     samplers_[0].init_places();
                    // }
                    samplers_[0].init_places();


                    // int last_update = -1;
                    // const double temp_begin = 2;
                    // const double temp_end = 1.;
                    for (int j = 0; j < n_moves; j++) {
                        // if (flag_emergency && j - last_update >= M * 50)
                        //     break;

                        // double temp = temp_begin + (temp_end - temp_begin) * j / n_moves;
                        bool flag_update = samplers_[0].step();
                        if (flag_update) {
                            // last_update = j;
                            cnt_update++;
                            const double& prob_log = samplers_[0].prob_log;
                            const auto& hash = samplers_[0].hash_places;
                            if (prob_logs_hash_indexed[hash] + eps < prob_log) {
                                prob_logs_hash_indexed[hash] = prob_log;
                                if (que.size() < n_max_remember || prob_log > que.top().first + eps) {
                                    cnt++;
                                    if (que.size() == n_max_remember) {
                                        const auto [prob_log, idx_pool] = que.top();
                                        que.pop();
                                        pool_places.pop(idx_pool);
                                    }
                                    que.emplace(
                                        prob_log, pool_places.emplace(samplers_[0].places_indecies, hash, prob_log));
                                }
                            }
                        }
                    }
                    // if (n_query == 0 && que.size() == n_max_remember)
                    //     break;
                    if (get_time_remaining() < 0)
                        break;
                    if (n_query == QUERIES_MAX - 1) {
                        i = 0;
                    }
                    // cerr << cnt << "/" << n_moves << endl;
                    // if (cnt == 0)
                    //     break;
                }
#ifdef PRINT_DEBUG
                cerr << "update ratio : " << (double)cnt_update / cnt_total << endl;
#endif
            }

            // cerr << "prob_log: " << samplers_[0].prob_log << endl;
            // cerr << "prob_log_correct: " << samplers_[0].get_prob_log_naive() << endl;

            // display best
            // if (1) {
            //     const auto [prob_log, idx_pool] = que.top();
            //     const auto& [places, hash, prob_log_] = pool_places[idx_pool];
            //     cerr << "prob_log: " << prob_log << endl;
            //     samplers_[0].display(places);
            // }

            // answer?
            bool flag_answer = false;

            int idx_pool_ans = -1;
            vector<pair<double, int>> V_prob_log_idx_pool = que.c;
            // vector<pair<double, int>> V_prob_log_idx_pool;
            // while (!que.empty()) {
            //     V_prob_log_idx_pool.emplace_back(que.top());
            //     que.pop();
            // }
            sort(V_prob_log_idx_pool.begin(), V_prob_log_idx_pool.end(), greater<>());
// display best
#ifdef PRINT_DEBUG
            {
                const auto [prob_log, idx_pool] = V_prob_log_idx_pool[0];
                const auto& [places, hash, prob_log_] = pool_places[idx_pool];
                cerr << "prob_log: " << prob_log << endl;
                samplers_[0].display(places);
            }
            for (int i = 0; i < (int)V_prob_log_idx_pool.size(); i++) {
                const auto [prob_log, idx_pool] = V_prob_log_idx_pool[i];
                const auto& [places, hash, prob_log_] = pool_places[idx_pool];
                for (int j = 0; j < M; j++) {
                    const auto idx = places[j];
                    const auto p = AvailablePlaces_[j][idx];
                    cerr << p << "  ";
                    // cerr << (int)p.raw() << " ";
                }
                cerr << endl;
                if (i >= 10)
                    break;
                // cerr << (int)places.size() << endl;
            }
            for (int i = 0; i < (int)V_prob_log_idx_pool.size(); i++) {
                cerr << V_prob_log_idx_pool[i].first << " ";
                if (i >= 10)
                    break;
            }
            cerr << endl;
#endif
            if (0) {
                // best prob
                vector<TPlaceIndex> places{78, 109, 32, 30, 116, 132, 72, 77};
                const auto prob_log = samplers_[0].get_prob_log(places);
                cerr << "best prob_log: " << prob_log << endl;
            }
            if (n_query == QUERIES_MAX - 1) {
                flag_answer = true;
                const auto [prob_log, idx_pool] = V_prob_log_idx_pool[0];
                idx_pool_ans = idx_pool;
            } else if (V_prob_log_idx_pool.size() >= 2) {
                // TODO 確率が高い順に overlap を見て、同じなら merge して比較
                // priority_queue to vector
                // const double min_dprob_log = 2.0;  // exp(5)~150
                const double min_dprob_log = 2.2;  // exp(5)~150

                auto f = [&](int idx) {
                    auto [prob_log, idx_pool] = V_prob_log_idx_pool[idx];
                    const auto& [places, hash, prob_log_] = pool_places[idx_pool];
                    return samplers_[0].get_overlaps_bool(places);
                };

                const auto overlaps_bool_best = f(0);

                for (int i = 1; i < (int)V_prob_log_idx_pool.size(); i++) {
                    auto overlaps_bool_tmp = f(i);
                    if (overlaps_bool_best == overlaps_bool_tmp)
                        continue;
                    if (V_prob_log_idx_pool[0].first - V_prob_log_idx_pool[i].first > min_dprob_log) {
                        flag_answer = true;
                        const auto& [prob_log, idx_pool] = V_prob_log_idx_pool[0];
                        idx_pool_ans = idx_pool;
                    }
                    break;
                }

                // if (V_prob_log_idx_pool[0].first - V_prob_log_idx_pool[1].first > min_dprob_log) {
                //     flag_answer = true;
                //     const auto& [prob_log, idx_pool] = V_prob_log_idx_pool[0];
                //     idx_pool_ans = idx_pool;
                // }
            }
            que = PQue(V_prob_log_idx_pool.begin(), V_prob_log_idx_pool.end());

            // hillclimbingver2
            auto do_query_hillclimbing_ver2 = [&]() {
                // return do_query_random();

                {
                    auto V_copy = V_prob_log_idx_pool;
                    sort(V_copy.begin(), V_copy.end(), greater<>());
                    assert(V_copy == V_prob_log_idx_pool);
                }

                constexpr int n_memo_max = 1000;
                int n_memo = 0;
                vector<array<TOverlap, NN>> memo_overlaps;
                vector<double> probs;
                const double prob_log_cutoff = 20;
                double prob_log_max = 0;
                for (const auto& [prob_log, idx_pool] : V_prob_log_idx_pool) {
                    if (n_memo >= 1 && prob_log_max > prob_log + prob_log_cutoff)
                        break;
                    const auto& [places, hash, prob_log_] = pool_places[idx_pool];
                    memo_overlaps.emplace_back(samplers_[0].get_overlaps(places));
                    if (n_memo == 0) {
                        prob_log_max = prob_log;
                        // prob_logs.emplace_back(0);
                        probs.emplace_back(1.);
                    } else {
                        // prob_logs.emplace_back(prob_log - prob_log_max);
                        probs.emplace_back(exp(prob_log - prob_log_max));
                    }
                    n_memo++;
                    if (n_memo == n_memo_max)
                        break;
                }
                const double prob_sum = accumulate(probs.begin(), probs.end(), 0.);
                for (auto& prob : probs) {
                    prob /= prob_sum;
                }

#ifdef PRINT_DEBUG
                cerr << "n_memos: " << n_memo << endl;
#endif


                // const int k_max = 10;
                const int k_max = NN - 1;

                // 関係する盤面を前計算
                vector memo_place_to_board(NN, vector<pair<int, int>>());
                for (int n = 0; n < n_memo; n++) {
                    for (int i = 0; i < NN; i++) {
                        if (memo_overlaps[n][i] > 0) {
                            memo_place_to_board[i].emplace_back(n, memo_overlaps[n][i]);
                        }
                    }
                }

                array<Point, NN> mask;
                int k = 0;  // size
                for (int i = 0; i < NN; i++) {
                    mask[i] = Point(i);
                }

                // shuffle
                for (int i = NN - 1; i >= 1; i--) {
                    swap(mask[i], mask[rng.range_random_65535_unsafe(i + 1)]);
                }
                // k = rng.range_random(NN / 4, NN / 4 * 3);
                // k = rng.range_random(2, 5);
                // k = NN / 2;
                // k = NN * 0.4;
                k = NN * 0.25;
                chmin(k, k_max);

                // 各盤面のvを持っておく
                vector<int> memo_v(n_memo, 0);
                for (int n = 0; n < n_memo; n++) {
                    const auto& overlaps = memo_overlaps[n];
                    for (int i = 0; i < k; i++) {
                        const auto& p = mask[i];
                        memo_v[n] += overlaps[p.raw()];
                    }
                    assert(memo_v[n] < NN);
                }

                // double term1 = 0;
                // double term2 = 0;
                double score_raw = 0;
                vector<double> p_v(NN);
                vector<double> p_s(NN);

                auto calc_ksqrt_with_offset = [&](const int& k) {
                    constexpr double k_offset = 1.;
                    return sqrt(k + k_offset);
                };

                auto calc_term1 = [&]() {
                    double term1 = 0;
                    const int kNN = k * NN;
                    for (int n = 0; n < n_memo; n++) {
                        const double prob = probs[n];
                        assert(prob >= 0);
                        const auto v = memo_v[n];
                        // term1 += prob * get_ProbKVSi_entropy(k, v);
                        term1 += prob * ProbSiKV_entropy_[kNN + v];
                    }
                    return term1;
                };

                auto calc_term2 = [&]() {
                    fill(p_v.begin(), p_v.end(), 0);
                    fill(p_s.begin(), p_s.end(), 0);
                    double term2 = 0;
                    for (int n = 0; n < n_memo; n++) {
                        const double prob = probs[n];
                        assert(prob >= 0);
                        const auto v = memo_v[n];
                        p_v[v] += prob;
                    }
                    for (int v = 0; v < NN; v++) {
                        if (p_v[v] == 0)
                            continue;
                        const auto kv = convert_kv(k, v);
                        const int s_size = Si_size_[kv];
                        int s = Si_offset_min_[kv];
                        for (int s_offseted = 0; s_offseted < s_size; s_offseted++) {
                            p_s[s++] += p_v[v] * get_ProbKVSi_raw(kv, s_offseted);
                        }
                    }
                    term2 = calc_entropy(p_s);
                    return term2;
                };

                score_raw = -calc_term1() + calc_term2();

                auto calc_dscore_add = [&](const int& idx1, const double& log_rand_cutoff) {
                    if (k == NN - 1)
                        return false;
                    if (k >= k_max)
                        return false;
                    assert(k <= idx1 && idx1 < NN);
                    const auto a = mask[idx1];
                    // double dterm1 = 0;
                    // double dterm2 = 0;
                    int k_new = k + 1;

                    // vector<double> p_v(TILE_SUM + 1);

                    // 関係するboardのみ考慮
                    for (auto& [n, dv] : memo_place_to_board[a.raw()]) {
                        double prob = probs[n];
                        assert(prob >= 0);
                        memo_v[n] += dv;
                    }
                    k++;
                    const double score_raw_new = -calc_term1() + calc_term2();
                    k--;
                    const double dscore
                        = score_raw_new * calc_ksqrt_with_offset(k_new) - score_raw * calc_ksqrt_with_offset(k);
                    if (dscore >= 0 || dscore > log_rand_cutoff) {
                        score_raw = score_raw_new;
                        swap(mask[k], mask[idx1]);
                        k = k_new;
                        return true;
                    } else {
                        // rollback
                        for (auto& [n, dv] : memo_place_to_board[a.raw()]) {
                            memo_v[n] -= dv;
                        }
                        return false;
                    }
                };
                auto calc_dscore_remove = [&](const int& idx2, const double& log_rand_cutoff) {
                    if (k == 1)
                        return false;
                    assert(0 <= idx2 && idx2 < k);
                    const auto b = mask[idx2];
                    int k_new = k - 1;

                    // 関係するboardのみ考慮
                    for (auto& [n, dv] : memo_place_to_board[b.raw()]) {
                        double prob = probs[n];
                        assert(prob >= 0);
                        memo_v[n] -= dv;
                    }
                    k--;
                    const double score_raw_new = -calc_term1() + calc_term2();
                    k++;
                    const double dscore
                        = score_raw_new * calc_ksqrt_with_offset(k_new) - score_raw * calc_ksqrt_with_offset(k);
                    if (dscore >= 0 || dscore > log_rand_cutoff) {
                        score_raw = score_raw_new;
                        swap(mask[k - 1], mask[idx2]);
                        k = k_new;
                        return true;
                    } else {
                        // rollback
                        for (auto& [n, dv] : memo_place_to_board[b.raw()]) {
                            memo_v[n] += dv;
                        }
                        return false;
                    }
                };
                auto calc_dscore_change = [&](const int& idx1, const int& idx2, const double& log_rand_cutoff) {
                    // no k change!
                    // add a
                    // remove b
                    if (k == 1)
                        return false;
                    if (k == NN - 1)
                        return false;
                    if (k >= k_max)
                        return false;
                    assert(k <= idx1 && idx1 < NN);
                    assert(0 <= idx2 && idx2 < k);
                    const auto a = mask[idx1];
                    const auto b = mask[idx2];

                    // double dscore_raw = 0;
                    for (auto& [n, dv] : memo_place_to_board[a.raw()]) {
                        double prob = probs[n];
                        assert(prob >= 0);
                        memo_v[n] += dv;
                    }
                    for (auto& [n, dv] : memo_place_to_board[b.raw()]) {
                        double prob = probs[n];
                        assert(prob >= 0);
                        memo_v[n] -= dv;
                    }
                    const double score_raw_new = -calc_term1() + calc_term2();
                    const double dscore
                        = score_raw_new * calc_ksqrt_with_offset(k) - score_raw * calc_ksqrt_with_offset(k);
                    if (dscore >= 0 || dscore > log_rand_cutoff) {
                        score_raw = score_raw_new;
                        swap(mask[idx1], mask[idx2]);
                        return true;
                    } else {
                        // rollback
                        for (auto& [n, dv] : memo_place_to_board[a.raw()]) {
                            memo_v[n] -= dv;
                        }
                        for (auto& [n, dv] : memo_place_to_board[b.raw()]) {
                            memo_v[n] += dv;
                        }
                        return false;
                    }
                };

                // const double temp_begin = 0.1;
                // const double temp_end = 0.0001;
                // const int total_iter = 10000;
                double temp_begin = 0;
                double temp_end = 0;
                int total_iter = 1000;
                if (M == 2) {
                    temp_begin = 1e-3;
                    temp_end = 1e-4;
                    total_iter = 20000;
                } else if (M == 3) {
                    temp_begin = 1e-3;
                    temp_end = 1e-4;
                    total_iter = 10000;
                } else if (M == 4) {
                    temp_begin = 1e-3;
                    temp_end = 1e-4;
                    total_iter = 5000;
                } else if (M == 5) {
                    total_iter = 3000;
                } else if (M <= 8) {
                    total_iter = 2000;
                }
                int k_best = k;
                double score_best = score_raw * calc_ksqrt_with_offset(k);
                array<Point, NN> mask_best = mask;

                int cnt_no_update = 0;

                for (int iter = 0; iter < total_iter; iter++) {
                    double temp = temp_begin + (temp_end - temp_begin) * iter / total_iter;
                    double log_rand_cutoff = temp * log_random();
                    bool flag_update = false;
                    if (M <= 16 && (rng.uint_random_65535_unsafe() % 2 == 1)) {
                        // if (rng.uint_random_65535_unsafe() % 2 == 1) {
                        // if (0) {
                        int idx1 = rng.range_random_safe(k, NN);
                        int idx2 = rng.range_random_safe(k);
                        flag_update = calc_dscore_change(idx1, idx2, log_rand_cutoff);
                    } else {
                        int idx = rng.range_random_safe(NN);
                        if (idx < k) {
                            if (k <= 3)
                                continue;
                            flag_update = calc_dscore_remove(idx, log_rand_cutoff);
                        } else {
                            flag_update = calc_dscore_add(idx, log_rand_cutoff);
                        }
                    }
                    // } else if (rng.bool_random<0.5f>()) {
                    //     int idx1 = rng.range_random_65535_unsafe(k, NN);
                    //     flag_update = calc_dscore_add(idx1, log_rand_cutoff);
                    // } else {
                    //     int idx2 = rng.range_random_65535_unsafe(k);
                    //     flag_update = calc_dscore_remove(idx2, log_rand_cutoff);
                    // }
                    if (flag_update) {
                        if (chmax(score_best, score_raw * calc_ksqrt_with_offset(k))) {
                            mask_best = mask;
                            k_best = k;
                        }
                        cnt_no_update = 0;
                    } else {
                        cnt_no_update++;
                        if (cnt_no_update > NN * 1.4) {
                            break;
                        }
                    }
                }

                vector<Point> mask_ans(k_best);
                for (int i = 0; i < k_best; i++) {
                    mask_ans[i] = mask_best[i];
                }
#ifdef PRINT_DEBUG
                cerr << "mask size: " << k_best << endl;
                for (int i = 0; i < k_best; i++) {
                    cerr << mask_best[i] << "  ";
                }
                cerr << endl;
#endif
                ask(mask_ans);
                return 1. / sqrt(k);
            };

#ifdef PRINT_HEATMAP
            // probability heatmap
            // if (n_query % 10 == 0) {
            if (1) {
                array<double, NN> heatmap;
                fill(heatmap.begin(), heatmap.end(), 0);
                for (auto& [prob_log, idx_pool] : V_prob_log_idx_pool) {
                    const auto& [places, hash, prob_log_] = pool_places[idx_pool];
                    for (int i = 0; i < M; i++) {
                        const auto idx = places[i];
                        const auto p = AvailablePlaces_[i][idx];
                        for (auto& q : Polyomions_[i]) {
                            heatmap[q.raw() + p.raw()] += exp(prob_log_);
                        }
                    }
                }
                const double max_heat = *max_element(heatmap.begin(), heatmap.end());
                for (int i = 0; i < NN; i++) {
                    double heat = heatmap[i] / max_heat;
                    const Point p(i);
                    string s;
                    {
                        stringstream ss;
                        ss << hex << std::setw(2) << setfill('0') << clamp((int)(heat * 255), 0, 255);
                        // to color code
                        s = ss.str();
                        s = "#" + s + s + s;
                    }

                    cout << "#c " << p.x() << " " << p.y() << " " << s << endl;
                }
            }
#endif

            if (flag_answer) {
                flag_last_update = false;
                const auto [places, hash, prob_log] = pool_places[idx_pool_ans];
// cerr << "prob_log: " << prob_log << endl;
// cerr << "prob_log: " << samplers_[0].get_prob_log(places) << endl;
#ifdef PRINT_DEBUG
                cerr << "answer index" << endl;
                for (auto& x : places) {
                    cerr << (int)x << " ";
                }
                cerr << endl;
#endif
                bool flag_correct = answer(get_points(places));
                if (flag_correct) {
                    return;
                } else {
                    memo_wrong_hash.emplace_back(hash);
#ifdef PRINT_DEBUG
                    cerr << "wrong" << endl;
                    cerr << "hash: " << (int)hash << endl;
                    cerr << "hash: " << samplers_[0].get_hash(places) << endl;
#endif
                    samplers_[0].display(places);
                    cost += 1;
                }
            } else {
                flag_last_update = true;
                // cost += do_query_random();
                cost += do_query_hillclimbing_ver2();
                samplers_[0].add_last_query();
                // if (n_query != QUERIES_MAX - 1) {
                //     cost += do_query_hillclimbing_ver2();
                //     samplers_[0].add_last_query();
                // }
            }
        }


#ifdef PRINT_DEBUG
        cerr << " end Solver.solve()" << endl;
#endif
    }

    Point get_random_place(const int m) const {
        return AvailablePlaces_[m][rng.range_random_safe(AvailablePlaces_[m].size())];
    }

    int get_random_place_index(const int m) const { return rng.range_random_65535_unsafe(NAvailablePlaces_[m]); }

private:
    int ask(const vector<Point>& points) {
        cout << "q " << points.size();
        for (const auto& p : points) {
            cout << " " << p.x() << " " << p.y();
        }
        cout << endl;
        int s;
        cin >> s;
        assert(s == clamp(s, 0, NN - 1));
        s = clamp(s, 0, NN - 1);

        // add to past queries
        Queries_.emplace_back(points, s);
        Queries_size_.emplace_back(points.size());


        // update MemoDOverlaps_
        {
            int q = (int)Queries_.size() - 1;
            array<TOverlap, NN> mask2d;
            fill(mask2d.begin(), mask2d.end(), TOverlap(0));
            for (auto& p : points) {
                mask2d[p.raw()] = (TOverlap)1;
            }
            for (int m = 0; m < M; m++) {
                for (int idx_place = 0; idx_place < (int)AvailablePlaces_[m].size(); idx_place++) {
                    const auto& p = AvailablePlaces_[m][idx_place];
                    int val = 0;
                    for (const auto& q : Polyomions_[m]) {
                        val += mask2d[p.raw() + q.raw()];
                    }
                    if (val != 0) {
                        auto& v = MemoDOverlaps_[m][idx_place];
                        v.emplace_back(q, val);
                        // cerr << "m: " << (int)m << " q: " << (int)q << " val: " << (int)val << endl;
                    }
                    {
                        auto& v = MemoDOverlaps2_[m][idx_place];
                        v.emplace_back(val);
                    }
                }
            }
        }

        return s;
    }

    vector<Point> get_points(const vector<TPlaceIndex>& places) const {
        vector<Point> ret;
        for (int m = 0; m < M; m++) {
            const auto& p = Polyomions_[m];
            const auto& q = AvailablePlaces_[m][places[m]];
            for (const auto& r : p) {
                ret.emplace_back(q.x() + r.x(), q.y() + r.y());
            }
        }
        sort(ret.begin(), ret.end());
        ret.erase(unique(ret.begin(), ret.end()), ret.end());
        return ret;
    }

    bool answer(const vector<Point>& points) {
        cout << "a " << points.size();
        for (const auto& p : points) {
            cout << " " << p.x() << " " << p.y();
        }
        cout << endl;
        int s;
        cin >> s;
        return bool(s);
    }

    void init() {
        print_debug(" start Solver.init()");

        log_table_int_.resize(1 << 16);
        for (int i = 1; i < (1 << 16); i++) {
            log_table_int_[i] = log(i);
        }

        Polyomions_.resize(M);
        for (int i = 0; i < M; i++) {
            for (const auto& [x, y] : Polyominos[i]) {
                Polyomions_[i].emplace_back(x, y);
            }
        }
        AvailablePlaces_.resize(M);
        AvailablePlaces_inv_.resize(M);
        NAvailablePlaces_.resize(M);
        for (int m = 0; m < M; m++) {
            fill(AvailablePlaces_inv_[m].begin(), AvailablePlaces_inv_[m].end(), PointInternal(-1));
            for (int x = 0; x < N - X_max[m]; x++) {
                for (int y = 0; y < N - Y_max[m]; y++) {
                    const auto p = Point(x, y);
                    AvailablePlaces_[m].emplace_back(p);
                    AvailablePlaces_inv_[m][p.raw()] = (TPlaceIndex)AvailablePlaces_[m].size() - 1;
                }
            }
            NAvailablePlaces_[m] = (int)AvailablePlaces_[m].size();
            // cerr << m << " " << Shifts_[m].size() << endl;
            // for (auto& p : Shifts_[m]) {
            //     cerr << p.x() << " " << p.y() << endl;
            // }
        }

        // probabilities
        // ProbSiKV_with_offset_log_.resize(2 * offset_ProbKVSi + 1);
        ProbSiKV_entropy_.resize(NNNN);
        // for (int k = 0; k < NN; k++) {
        //     fill(Si_size_[k].begin(), Si_size_[k].end(), 0);
        //     fill(Si_offset_min_[k].begin(), Si_offset_min_[k].end(), 0);
        //     fill(Si_offset_max_[k].begin(), Si_offset_max_[k].end(), 0);
        // }

        fill(Si_size_.begin(), Si_size_.end(), 0);
        fill(Si_offset_min_.begin(), Si_offset_min_.end(), 0);
        fill(Si_offset_max_.begin(), Si_offset_max_.end(), 0);
        // Si_size_.resize(NNNN, 0);
        // Si_offset_min_.resize(NNNN, 0);
        // Si_offset_max_.resize(NNNN, 0);

        int s_size_max = 0;


        const int n_sigma = 7;
        const int n_offset = 10;
        for (int k = 1; k < NN; k++) {
            for (int v = 0; v < NN; v++) {
                const double mu = (k - v) * Eps + v * (1 - Eps);
                const double var = k * Eps * (1 - Eps);
                const double sigma = sqrt(var);

                const int s_min = max(0, int(mu - n_sigma * sigma - n_offset));
                const int s_max = min(NN, int(mu + n_sigma * sigma + n_offset + 1));
                const int s_size = s_max - s_min;

                chmax(s_size_max, s_size);
            }
        }
        // ProbSiKV_with_offset_.resize(s_size_max);
        ProbSiKV_with_offset_log_.resize(s_size_max * NNNN);
        ProbKVSi_with_offset_.resize(NNNN);

        for (int k = 0; k < NN; k++) {
            for (int v = 0; v < NN; v++) {
                const int kv = convert_kv(k, v);
                if (k == 0) {
                    // auto& prob = ProbSiKV_with_offset_[0][kv];
                    ProbKVSi_with_offset_[kv].resize(1);
                    auto& prob = ProbKVSi_with_offset_[kv][0];
                    prob = 1.;
                    auto& prob_log = ProbSiKV_with_offset_log_[kv];
                    prob_log = 0.;
                    Si_size_[kv] = 1;
                    Si_offset_min_[kv] = 0;
                    Si_offset_max_[kv] = 1;
                    auto& entropy = ProbSiKV_entropy_[kv];
                    entropy = 0;
                    continue;
                }

                // ProbKVSi_with_offset_[k][v].resize(2 * offset_ProbKVSi + 1);
                const double mu = (k - v) * Eps + v * (1 - Eps);
                const double var = k * Eps * (1 - Eps);
                const double sigma = sqrt(var);

                const int s_min = max(0, int(mu - n_sigma * sigma - n_offset));
                const int s_max = min(NN, int(mu + n_sigma * sigma + n_offset + 1));
                const int s_size = s_max - s_min;
                Si_offset_min_[kv] = s_min;
                Si_offset_max_[kv] = s_max;
                Si_size_[kv] = s_size;
                ProbKVSi_with_offset_[kv].resize(s_size);
                // assert(s_size <= (int)ProbSiKV_with_offset_.size());
                // if (s_size > (int)ProbSiKV_with_offset_.size()) {
                //     ProbSiKV_with_offset_.resize(s_size);
                //     ProbSiKV_with_offset_log_.resize(s_size);
                // }

                double prob_sum = 0;
                double cdf_bef = 0;

                for (int s_offseted = 0; s_offseted < s_size; s_offseted++) {
                    const int s = s_min + s_offseted;
                    // auto& prob = ProbSiKV_with_offset_[s_offseted][kv];
                    auto& prob = ProbKVSi_with_offset_[kv][s_offseted];
                    double cdf_now;
                    assert(0 <= s && s < NN);
                    if (s == s_max - 1) {
                        cdf_now = 1;
                        // cdf_now = cdf(s + 0.5, mu, sigma);
                    } else {
                        cdf_now = cdf(s + 0.5, mu, sigma);
                    }
                    prob = cdf_now - cdf_bef;
                    cdf_bef = cdf_now;
                    prob_sum += prob;
                    // cerr << k << " " << v << " " << s << " " << prob << endl;
                }

#ifdef PRINT_DEBUG
                if (abs(prob_sum - 1) > 1e-6) {
                    cerr << k << " " << v << " " << prob_sum << endl;
                }
#endif
                assert(abs(prob_sum - 1) < 1e-6);

                auto& entropy = ProbSiKV_entropy_[kv];
                entropy = 0;
                int s_tmp = kv;
                for (int s_offseted = 0; s_offseted < s_size; s_offseted++) {
                    // auto& prob = ProbSiKV_with_offset_[s_offseted][kv];
                    auto prob = ProbKVSi_with_offset_[kv][s_offseted];
                    prob /= prob_sum;
                    auto& prob_log = ProbSiKV_with_offset_log_[s_tmp];
                    prob_log = log(prob);
                    chmax(prob_log, MIN_PROB_LOG);
                    entropy += -prob * prob_log;
                    s_tmp += NNNN;
                }
                // cerr << k << " " << v << " " << entropy << endl;


                /*
                for (int s_offseted = -offset_ProbKVSi; s_offseted <= offset_ProbKVSi; s_offseted++) {
                    const int s = v + s_offseted;
                    auto& prob = ProbKVSi_with_offset_[k][v][s_offseted + offset_ProbKVSi];
                    double cdf_now;
                    if (s < 0) {
                        continue;
                    } else if (s == 0) {
                        cdf_now = cdf(0.5, mu, sigma);
                    } else {
                        cdf_now = cdf(s + 0.5, mu, sigma);
                    }
                    prob = cdf_now - cdf_bef;
                    cdf_bef = cdf_now;
                    // assert(prob == get_ProbKVSi(k, v, s));
                    prob_sum += prob;
                }
                // assert(abs(prob_sum - 1) < 1e-6);
                for (int s_offseted = -offset_ProbKVSi; s_offseted <= offset_ProbKVSi; s_offseted++) {
                    auto& prob = ProbKVSi_with_offset_[k][v][s_offseted + offset_ProbKVSi];
                    prob /= prob_sum;
                    // auto& prob_log = ProbKVSi_with_offset_log_[k][v][s_offseted + offset_ProbKVSi];
                    auto& prob_log = ProbSiKV_with_offset_log_[s_offseted + offset_ProbKVSi][k][v];
                    prob_log = log(prob);
                    chmax(prob_log, MIN_PROB_LOG);
                }
                ProbKVSi_with_offset_entropy_[k][v] = calc_entropy(ProbKVSi_with_offset_[k][v]);
                */
            }
        }

        // cerr << "ProbKVSi(1, 1, 1) = " << get_ProbKVSi(1, 1, 1) << endl;

        // ポリオミノを動かした時の重複度の変化をメモする用
        // 値の追加はクエリ時に行う
        MemoDOverlaps_.resize(M);
        for (int m = 0; m < M; m++) {
            auto& v = MemoDOverlaps_[m];
            v.resize((int)AvailablePlaces_[m].size());
        }

        MemoDOverlaps2_.resize(M);
        for (int m = 0; m < M; m++) {
            auto& v = MemoDOverlaps2_[m];
            v.resize((int)AvailablePlaces_[m].size());
        }


        // samplers
        samplers_.resize(SAMPLERS_);
        for (int i = 0; i < SAMPLERS_; i++) {
            samplers_[i].solver = this;
            samplers_[i].init();
        }

        print_debug(" end Solver.init()");
    }

    // double get_ProbKVSi(const int& k, const int& v, const int& s) const {
    //     assert(0 <= k && k < NN);
    //     assert(0 <= v && v < NN);
    //     const int s_offseted = s - Si_offset_min_[k][v];
    //     if (0 <= s_offseted && s_offseted < Si_size_[k][v]) {
    //         return ProbSiKV_with_offset_[s_offseted][k][v];
    //     } else {
    //         return 0;
    //     }
    // }
    inline double get_ProbKVSi_log(const int& k, const int& v, const int& s) const {
        assert(0 <= k && k < NN);
        assert(0 <= v && v < NN);
        assert(s >= 0);
        const auto kv = convert_kv(k, v);
        const int s_offseted = s - Si_offset_min_[kv];
        if (0 <= s_offseted && s_offseted < Si_size_[kv]) {
            // return ProbSiKV_with_offset_log_[s_offseted][kv];
            return ProbSiKV_with_offset_log_[s_offseted * NNNN + kv];
        } else {
            return MIN_PROB_LOG;
        }
    }
    inline double get_ProbKVSi_log(const int& kv, const int& s) const {
        assert(s >= 0);
        const int s_offseted = s - Si_offset_min_[kv];
        if (0 <= s_offseted && s_offseted < Si_size_[kv]) {
            // return ProbSiKV_with_offset_log_[s_offseted][kv];
            return ProbSiKV_with_offset_log_[s_offseted * NNNN + kv];
        } else {
            return MIN_PROB_LOG;
        }
    }
    // double get_ProbKVSi_raw(const int& k, const int& v, const int& s_raw) const {
    //     assert(0 <= k && k < NN);
    //     assert(0 <= v && v < NN);
    //     assert(0 <= s_raw && s_raw < Si_size_[convert_kv(k, v)]);
    //     return ProbSiKV_with_offset_[s_raw][convert_kv(k, v)];
    // }
    inline double get_ProbKVSi_raw(const int& kv, const int& s_raw) const {
        assert(0 <= k && k < NN);
        assert(0 <= v && v < NN);
        assert(0 <= s_raw && s_raw < Si_size_[kv]);
        // return ProbSiKV_with_offset_[s_raw][kv];
        return ProbKVSi_with_offset_[kv][s_raw];
    }
    inline double get_ProbKVSi_log_raw(const int& k, const int& v, const int& s_raw) const {
        assert(0 <= s_raw && s_raw < Si_size_[convert_kv(k, v)]);
        // return ProbSiKV_with_offset_log_[s_raw][convert_kv(k, v)];
        return ProbSiKV_with_offset_log_[s_raw * NNNN + convert_kv(k, v)];
    }
    double get_ProbKVSi_entropy(const int& k, const int& v) const {
        //  return ProbKVSi_with_offset_entropy_[k][v];
        return ProbSiKV_entropy_[convert_kv(k, v)];
    }
};

template <int N>
void init() {
    print_debug(" begin init()");
    // for (int i = 1; i < (1 << 16); i++) {
    //     cerr << i << " " << log(i) << endl;
    //     log_table_int[i] = log(i);
    // }

    X_max.resize(M);
    Y_max.resize(M);
    for (int i = 0; i < M; i++) {
        for (const auto& [x, y] : Polyominos[i]) {
            chmax(X_max[i], x);
            chmax(Y_max[i], y);
        }
    }
    print_debug(" end init()");
}


template <int N, typename PointInternal>
void solve() {
#ifdef PRINT_DEBUG
    cerr << " start solve" << endl;
    cerr << "N: " << N << ", M: " << M << endl;
#endif
    init<N>();
    Solver<N, PointInternal> solver;
    solver.solve();
}

void solve0000() {
    // assert(N_GLOBAL == 15);
    // assert(M_GLOBAL == 2);
    solve<15, u8>();
    // solve<16, u8>();
    // solve<17, u16>();
}

#ifdef COMPILE_SOLVE_ALL
// clang-format off
void solve() {
    switch (N_GLOBAL) {
#define CASE(N, P) case (N): solve<N, P>(); break;
// u8 N=10~16
CASE(10, u8) CASE(11, u8) CASE(12, u8) CASE(13, u8) CASE(14, u8)
CASE(15, u8) CASE(16, u8)
// u16 N=17~20
CASE(17, u16) CASE(18, u16) CASE(19, u16) CASE(20, u16)
        default : cerr << "input not supported" << endl;
        cerr << "N: " << N_GLOBAL << endl;
        assert(false);
    }
#undef FOR_EACH_N
#undef CASE
}
// clang-format on
#endif

void read_input() {
    cin >> N_GLOBAL >> M_GLOBAL;
    cin >> Eps;
    M = M_GLOBAL;
    Eps100 = int(Eps * 100 + 0.5);
    Polyominos.resize(M_GLOBAL);
    for (int i = 0; i < M_GLOBAL; i++) {
        int d;
        cin >> d;
        TILE_SUM += d;
        Polyominos[i].resize(d);
        for (int j = 0; j < d; j++) {
            cin >> Polyominos[i][j].first >> Polyominos[i][j].second;
        }
    }
    print_data("N", N_GLOBAL);
    print_data("M", M_GLOBAL);
    print_data("eps100", Eps100);
}

void test() {
    const int N = 128;
    vector V(N, vector(N, vector<u32>(N)));
    vector Vp(N, vector(N, vector<pair<u16, u8>>(N)));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                u32 x = rng.range_random_65535(200);
                u32 y = rng.range_random_65535(200);
                u32 z = (x << 16) | y;
                V[i][j][k] = z;
                Vp[i][j][k] = {x, y};
            }
        }
    }

    print_time();
    u32 mask1 = 0;
    u32 mask2 = 0;
    const int M = 1 << 30;
    rng.reset();
    for (int _ = 0; _ < M; _++) {
        const auto i = rng.range_random_65535_unsafe(N);
        const auto j = rng.range_random_65535_unsafe(N);
        const auto k = rng.range_random_65535_unsafe(N);
        const auto& z = V[i][j][k];
        mask1 ^= z >> 16;
        mask2 ^= z & ((1 << 16) - 1);
    }
    cerr << mask1 << " " << mask2 << endl;

    print_time();
    rng.reset();
    mask1 = mask2 = 0;
    for (int _ = 0; _ < M; _++) {
        const auto i = rng.range_random_65535_unsafe(N);
        const auto j = rng.range_random_65535_unsafe(N);
        const auto k = rng.range_random_65535_unsafe(N);
        const auto& [x, y] = Vp[i][j][k];
        mask1 ^= x;
        mask2 ^= y;
    }
    cerr << mask1 << " " << mask2 << endl;
    print_time();

    exit(0);
}

int main() {
    // test();

    init_library();
    print_time();
    read_input();

    // if (M >= 7) {
    //     sample::solve(N_GLOBAL, M_GLOBAL, Eps);
    //     return 0;
    // }

#ifdef COMPILE_SOLVE_ALL
    solve();
#else
    solve0000();
#endif

    print_time();
    return 0;
}

// メモ
/*
 * 観測値を[0, NN) に丸め込んでいる
 * 多分正しい
 */
