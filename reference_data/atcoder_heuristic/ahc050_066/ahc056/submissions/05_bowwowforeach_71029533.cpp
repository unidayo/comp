#define FULL_LIB 1

#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0

#define TIME_LIMIT (1950)

#define NOT_SUBMIT 0
#define VALIDATION 0

#define IO_FILE 0
#define SELF_JUDGE 0

#define OUTPUT_INFO 0
#define OUTPUT_FINAL_INFO 0
#define OUTPUT_LOG 0
#define OUTPUT_VISUAL 0
#define OUTPUT_PERF 0
#define OUTPUT_COMMENT 0

#define FIX_RESULT 0
#define FIX_LOOP_COUNT 80000


#define TIME_LIMIT_US (TIME_LIMIT * 1000)

#define OUT_INFO OUTPUT_INFO
#define OUT_LOG OUTPUT_LOG
#define OUT_VIS OUTPUT_VISUAL
#define OUT_PERF OUTPUT_PERF

#ifdef __clang_version__
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif


#ifndef _MSC_VER
#pragma GCC target("avx512f,avx512bw,avx512dq,avx512vl,fma,prefer-vector-width=512")
#pragma GCC optimize("O3,unroll-loops,omit-frame-pointer")

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#define _USE_MATH_DEFINES
#ifdef __clang_version__

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <cfenv>
#include <cinttypes>
#include <cstdint>
#include <cwchar>
#include <cwctype>

#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <ratio>
#include <regex>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

#include <optional>

#else
#include <bits/stdc++.h>
#endif

using namespace std;


#define FOR(i, s, e) for (int i = int(s), i##_end = int(e); i < i##_end; ++i)
#define RFOR(i, s, e) for (int i = int(e) - 1, i##_s = int(s); i >= i##_s; --i)
#define REP(i, n) for (int i = 0, i##_size = int(n); i < i##_size; ++i)
#define RREP(i, n) for (int i = int(n) - 1; i >= 0; --i)


#define ALL(x) (x).begin(), (x).end()

template <class T, class U>
inline void chmin(T& a, const U& b) {
    if (b < a) {
        a = b;
    }
}
template <class T, class U>
inline void chmax(T& a, const U& b) {
    if (a < b) {
        a = b;
    }
}
template <class T, class U, class V>
inline void chclip(T& v, const U& lower, const V& upper) {
    if (v < lower) {
        v = lower;
    }
    else if (v > upper) {
        v = upper;
    }
}
template <class T>
inline constexpr T square(T v) {
    return v * v;
}

template <class T, int SIZE>
constexpr int len(const T (&)[SIZE]) {
    return SIZE;
}

#define cauto const auto

#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using f32 = float;
using f64 = double;

using Clock = chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

struct ChronoTimer {
   private:
    TimePoint startTime_;
    TimePoint endTime_;

   public:
    void Init() {
        startTime_ = Clock::now();
        endTime_ = startTime_;
    }

    void Start(int limit) {
        endTime_ = startTime_ + chrono::milliseconds(limit);
    }
    int ElapseTimeMs() const {
        return (int)chrono::duration_cast<chrono::milliseconds>(Clock::now() - startTime_).count();
    }

    bool IsOver() const {
        return Clock::now() >= endTime_;
    }

    inline void StartMs(int limit) {
        endTime_ = startTime_ + chrono::milliseconds(limit);
    }
    inline void StartUs(int limit) {
        endTime_ = startTime_ + chrono::microseconds(limit);
    }

    inline void Join() {
    }

    inline int ElapseTimeUs() const {
        return (int)chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - startTime_)
            .count();
    }

    void SetElapseTimeMs(int ms) {
        auto now = chrono::high_resolution_clock::now();
        auto limit = endTime_ - startTime_;
        startTime_ = now - chrono::milliseconds(ms);
        endTime_ = startTime_ + limit;
    }

    inline int LeftToUS(const TimePoint& limit) const {
        return (int)chrono::duration_cast<chrono::microseconds>(limit - chrono::high_resolution_clock::now()).count();
    }

    inline double NowRate() const {
        return (chrono::high_resolution_clock::now() - startTime_).count() / (double)(endTime_ - startTime_).count();
    }

    inline double CalcRate(const TimePoint& start) const {
        return (chrono::high_resolution_clock::now() - start).count() / (double)(endTime_ - start).count();
    }

    inline TimePoint Now() const {
        return chrono::high_resolution_clock::now();
    }
    inline TimePoint StartTime() const {
        return startTime_;
    }
    inline TimePoint EndTime() const {
        return endTime_;
    }

    TimePoint GetLimitTimePointUs(int limit) const {
        return startTime_ + chrono::microseconds(limit);
    }
};

TimePoint Now() {
    return chrono::high_resolution_clock::now();
}

ChronoTimer timer_;

template <class T>
void InstanceRun(int argc, const char* argv[]) {
    timer_.Init();
    T* m = new T;
    m->Run(argc, argv);
    quick_exit(0);
}

struct Main;

signed main(int argc, const char* argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(0);

    InstanceRun<Main>(argc, argv);
}


struct MemoryException {};


#define VABORT()
#define VASSERT(exp)






template <class A, class B>
struct pr {
    union {
        A a;
        A x;
        A first;
    };
    union {
        B b;
        B y;
        B second;
    };

    bool operator==(pr const& r) const {
        return a == r.a && b == r.b;
    }
    bool operator!=(pr const& r) const {
        return !((*this) == r);
    }
    bool operator<(pr const& r) const {
        if (a == r.a) {
            return b < r.b;
        }
        return a < r.a;
    }
    bool operator>(pr const& r) const {
        return r < (*this);
    }

    pr& operator+=(pr const& v) {
        a += v.a;
        b += v.b;
        return *this;
    }
    pr& operator-=(pr const& v) {
        a -= v.a;
        b -= v.b;
        return *this;
    }

    template <class C, class D>
    auto operator+(pr<C, D> const& v) const {
        return pr<decltype(a + v.a), decltype(b + v.b)>{a + v.a, b + v.b};
    }

    template <class C, class D>
    auto operator-(pr<C, D> const& v) const {
        return pr<decltype(a - v.a), decltype(b - v.b)>{a - v.a, b - v.b};
    }

    template <class C, class D>
    explicit operator pr<C, D>() const {
        return {C(a), D(b)};
    }

    template <class T>
    auto operator*(T const& v) const -> pr<decltype(x * v), decltype(y * v)> {
        return {x * v, y * v};
    }
    template <class T>
    auto operator/(T const& v) const -> pr<decltype(x / v), decltype(y / v)> {
        return {x / v, y / v};
    }

    template <class T>
    pr& operator*=(T const& v) {
        x *= v;
        y *= v;
        return *this;
    }
    template <class T>
    pr& operator/=(T const& v) {
        x /= v;
        y /= v;
        return *this;
    }

    pr operator-() const {
        return pr{-x, -y};
    }

    void flip() {
        swap(x, y);
    }

    friend istream& operator>>(istream& is, pr& p) {
        is >> p.a >> p.b;
        return is;
    }
    friend ostream& operator<<(ostream& os, pr const& p) {
        os << p.a << " " << p.b;
        return os;
    }

    template <size_t I>
    auto get() const {
        if constexpr (I == 0) {
            return x;
        }
        else if constexpr (I == 1) {
            return y;
        }
    }
};
using pint = pr<int, int>;
using pdouble = pr<double, double>;

static_assert(is_trivially_copyable<pint>::value, "not trivially_copyable");

template <class A, class B>
struct tuple_size<pr<A, B>> : integral_constant<size_t, 2> {};

template <class A, class B>
struct tuple_element<0, pr<A, B>> {
    using type = A;
};
template <class A, class B>
struct tuple_element<1, pr<A, B>> {
    using type = B;
};

pdouble ToDouble(const pint& p) {
    return pdouble{double(p.x), double(p.y)};
}
pint round(const pdouble& d) {
    return pint{(int)round(d.x), (int)round(d.y)};
}
inline double norm(const pdouble& d) {
    return sqrt((d.x * d.x) + (d.y * d.y));
}
inline double norm(const pint& d) {
    return sqrt((d.x * d.x) + (d.y * d.y));
}
inline int norm2(const pint& d) {
    return square(d.x) + square(d.y);
}
inline double norm2(const pdouble& d) {
    return square(d.x) + square(d.y);
}
inline pdouble normalized(const pdouble& d) {
    return d / norm(d);
}
inline double dot(const pdouble& a, const pdouble& b) {
    return a.x * b.x + a.y * b.y;
}
inline double cross(const pdouble& a, const pdouble& b) {
    return a.x * b.y - a.y * b.x;
}

template <class A, class B>
struct pr2 {
    A a;
    B b;
    template <size_t I>
    auto get() const {
        if constexpr (I == 0) {
            return a;
        }
        else if constexpr (I == 1) {
            return b;
        }
    }
};

#include <cstdint>


struct Xor64 {
    using result_type = uint32_t;
    static constexpr result_type min() {
        return 0;
    }
    static constexpr result_type max() {
        return UINT32_MAX;
    }

   private:
    Xor64(const Xor64& r) = delete;
    Xor64& operator=(const Xor64& r) = delete;

   public:
    uint64_t x;
    inline constexpr Xor64(uint64_t seed = 0) : x(88172645463325252ULL + seed) {
    }

    inline constexpr uint64_t Get64() {
        x ^= (x << 7);
        return x ^= (x >> 9);
    }

    inline constexpr result_type operator()() {
        return Get64() & 0xFFFFFFFF;
    }


    template <class T>
    inline constexpr T operator()(T r) {
        VASSERT(r <= 0xFFFFFFFF);
        return ((Get64() & 0xFFFFFFFF) * r) >> 32;
    }

    inline double GetDouble() {
        return Get64() / (double)UINT64_MAX;
    }
    inline bool GetProb(double E) {
        return GetDouble() <= E;
    }
};




template <class IT>
constexpr void Shuffle(IT&& begin, IT&& end, Xor64& rand) {
	int size = int(end - begin);
	if (size <= 1) {
		return;
	}
	REP(i, size - 1) {
		int j = i + rand(size - i);
		swap(*(begin + i), *(begin + j));
	}
}



template <class T, int CAP>
class CapArr {
   private:
    static_assert(is_trivially_copyable<T>::value);
    using SizeType = typename conditional<CAP <= 256, u8, u32>::type;

    T array_[CAP];
    SizeType size_ = 0;

   public:
    CapArr() {
        size_ = 0;
    }

    explicit CapArr(int size) {
        resize(size);
    }

    CapArr(int size, const T& e) {
        assign(size, e);
    }

    bool operator==(const CapArr<T, CAP>& r) const {
        if (size_ != r.size_) {
            return false;
        }
        REP(i, size_) {
            if (!(array_[i] == r.array_[i])) {
                return false;
            }
        }
        return true;
    }
    template <class U, int U_CAP>
    bool operator!=(const CapArr<U, U_CAP>& r) const {
        return !(*this == r);
    }

    bool MemEqual(const CapArr<T, CAP>& r) const {
        if (size_ != r.size_) {
            return false;
        }
        return memcmp(data(), r.data(), sizeof(T) * size_) == 0;
    }

    constexpr int capacity() const {
        return CAP;
    }

    int size() const {
        return size_;
    }
    bool empty() const {
        return size_ == 0;
    }
    bool exist() const {
        return size_ != 0;
    }

    void clear() {
        size_ = 0;
    }

    void resize(int size) {
        size_ = size;
    }

    void assign(int size, const T& e) {
        size_ = size;
        if constexpr (sizeof(T) == 1) {
            if constexpr (is_enum<T>::value) {
                memset(data(), typename underlying_type<T>::type(e), size);
            }
            else {
                memset(data(), *reinterpret_cast<const unsigned char*>(&e), size);
            }
        }
        else {
            for (int i = 0; i < size; ++i) {
                array_[i] = e;
            }
        }
    }

    void AssignIota(int size) {
        resize(size);
        iota(begin(), end(), 0);
    }
    void Iota(int size) {
        resize(size);
        iota(begin(), end(), 0);
    }

    void MemAssign(int size, int byte) {
        size_ = size;
        memset(data(), byte, sizeof(T) * size);
    }

    void MemCopy(const CapArr<T, CAP>& from) {
        size_ = from.size_;
        memcpy(data(), from.data(), sizeof(T) * from.size_);
    }

    template <int CAP2>
    void MemCopy(const CapArr<T, CAP2>& from) {
        static_assert(CAP2 <= CAP);
        size_ = from.size();
        memcpy(data(), from.data(), sizeof(T) * size_);
    }

    const T* data() const {
        return &array_[0];
    }
    T* data() {
        return &array_[0];
    }

    T& front() {
        return array_[0];
    }
    const T& front() const {
        return array_[0];
    }

    T& back() {
        return array_[size_ - 1];
    }
    const T& back() const {
        return array_[size_ - 1];
    }

    T& operator[](int index) {
        return array_[index];
    }

    const T& operator[](int index) const {
        return array_[index];
    }

    T* begin() {
        return &array_[0];
    }
    T* end() {
        return &array_[size_];
    }
    const T* begin() const {
        return &array_[0];
    }
    const T* end() const {
        return &array_[size_];
    }

    [[nodiscard]] T& push() {
        auto& ref = array_[size_];
        ++size_;
        return ref;
    }
    void push(const T& e) {
        array_[size_] = e;
        ++size_;
    }

    void pop() {
        --size_;
    }

    int find(const T& value) const {

        REP(i, size_) {
            if (array_[i] == value) {
                return i;
            }
        }
        return -1;
    }
    bool contains(const T& value) const {
        for (const auto& v : *this) {
            if (v == value) {
                return true;
            }
        }
        return false;
    }

    void insert(int index, const T& value) {
        insert(index, &value, 1);
    }

    void insert(int index, const T* mem, int size) {
        if (index == size_) {
            memcpy(data() + index, mem, sizeof(T) * size);
            size_ += size;
        }
        else {
            memmove(data() + index + size, data() + index, sizeof(T) * (size_ - index));
            memcpy(data() + index, mem, sizeof(T) * size);
            size_ += size;
        }
    }

    template <int RCAP>
    void append(const CapArr<T, RCAP>& r) {
        insert(size(), r.data(), r.size());
    }

    void remove(int index) {
        remove(index, index + 1);
    }

    void remove(int start, int end) {
        int size = end - start;
        memmove(data() + start, data() + end, sizeof(T) * (size_ - end));
        size_ -= size;
    }

    void RemoveSwap(int index) {
        array_[index] = array_[size_ - 1];
        --size_;
    }

    void RemoveWithKeeps(const CapArr<int, CAP>& idxs) {
        int to = 0;
        for (int idx : idxs) {
            array_[to] = array_[idx];
            ++to;
        }
        size_ = idxs.size();
    }

    void Remove(int start, int end) {
        memmove(data() + start, data() + end, sizeof(T) * (size_ - end));
        size_ -= end - start;
    }

    void RemoveInsert(int start, int end, const T* p, int size) {
        int newEnd = start + size;
        if (size_ - end > 0 && newEnd != end) {
            memmove(data() + newEnd, data() + end, sizeof(T) * (size_ - end));
        }

        memcpy(data() + start, p, sizeof(T) * size);

        size_ -= end - start;
        size_ += size;
    }

    void sort() {
        ::sort(begin(), end());
    }
    void rsort() {
        ::sort(begin(), end(), greater<T>());
    }
    template <class LESS>
    void sort(LESS&& less) {
        ::sort(begin(), end(), less);
    }
    void stable_sort() {
        ::stable_sort(begin(), end());
    }
    void stable_rsort() {
        ::stable_sort(begin(), end(), greater<T>());
    }
    template <class LESS>
    void stable_sort(LESS&& less) {
        ::stable_sort(begin(), end(), less);
    }

    void idx_sort(CapArr<int, CAP>& idxs) {
        idxs.Iota(size());
        idxs.sort([&](int a, int b) { return array_[a] < array_[b]; });
    }
    void idx_rsort(CapArr<int, CAP>& idxs) {
        idxs.Iota(size());
        idxs.sort([&](int a, int b) { return array_[b] < array_[a]; });
    }
    void idx_stable_sort(CapArr<int, CAP>& idxs) {
        idxs.Iota(size());
        idxs.stable_sort([&](int a, int b) { return array_[a] < array_[b]; });
    }
    void idx_stable_rsort(CapArr<int, CAP>& idxs) {
        idxs.Iota(size());
        idxs.stable_sort([&](int a, int b) { return array_[b] < array_[a]; });
    }

    template <class LESS>
    void nth_element(int n, LESS&& less) {
        ::nth_element(begin(), begin() + n, end(), less);
    }

    void reverse() {
        ::reverse(begin(), end());
    }

    template <class RAND>
    void shuffle(RAND&& r) {
        Shuffle(begin(), end(), r);
    }

    template <class RAND>
    const T& choice(RAND&& r) const {
        VASSERT(size_ > 0);
        return array_[r(size_)];
    }
};

template <class T, int CAPACITY>
struct CapacityQueue {
   private:
    array<T, CAPACITY> ar_ = {};
    int start_ = 0;
    int end_ = 0;

   public:
    inline void clear() {
        start_ = 0;
        end_ = 0;
    }

    inline void push(const T& v) {
        ar_[end_] = v;
        ++end_;
    }

    [[nodiscard]]
    inline T& push() {
        T& ptr = ar_[end_];
        ++end_;
        return ptr;
    }

    inline const T& get() const {
        return ar_[start_];
    }

    inline T pop() {
        return ar_[start_++];
    }

    inline bool empty() const {
        return start_ == end_;
    }

    inline bool exist() const {
        return start_ != end_;
    }

    inline int size() const {
        return end_ - start_;
    }

    inline int total_push_count() const {
        return end_;
    }

    const T& operator[](int i) const {
        return ar_[i];
    }
    int end_size() const {
        return end_;
    }
    int direct_start() const {
        return start_;
    }
    int direct_end() const {
        return end_;
    }

    inline auto begin() -> decltype(ar_.begin()) {
        return ar_.begin() + start_;
    }
    inline auto end() -> decltype(ar_.begin()) {
        return ar_.begin() + end_;
    }
    inline auto begin() const -> decltype(ar_.begin()) {
        return ar_.begin() + start_;
    }
    inline auto end() const -> decltype(ar_.begin()) {
        return ar_.begin() + end_;
    }

    const T& front() const {
        return ar_[start_];
    }
    const T& back() const {
        return ar_[end_ - 1];
    }

};

template <class T, int CAPACITY>
using CapQue = CapacityQueue<T, CAPACITY>;





template <int CAP>
struct CapSet {
   private:
    using Key = typename conditional<CAP <= 256, u8, u32>::type;
    CapArr<Key, CAP>
        elemens;  
    array<Key, CAP> indexs;  


   public:

    bool operator==(const CapSet<CAP>& r) const {
        if (elemens.size() != r.elemens.size()) {
            return false;
        }
        for (Key i : elemens) {
            if (!r.contains(i)) {
                return false;
            }
        }
        return true;
    }

    constexpr int capacity() {
        return CAP;
    }

    void fill(int n) {
        elemens.resize(n);
        iota(ALL(elemens), 0);
        iota(indexs.begin(), indexs.begin() + n, 0);
    }

    template <class Key2, int CAP2>
    void setfill(const CapArr<Key2, CAP2>& as) {
        elemens.resize(as.size());

        REP(ai, as.size()) {
            indexs[as[ai]] = ai;
            elemens[ai] = as[ai];
        }
    }

    void clear() {
        elemens.clear();
    }

    void set(Key i, bool exist) {
        if (exist) {
            fadd(i);
        }
        else {
            fremove(i);
        }
    }

    void add(Key i) {
        indexs[i] = elemens.size();
        elemens.push(i);
    }

    void fadd(Key i) {
        if (contains(i)) {
            return;
        }
        indexs[i] = elemens.size();
        elemens.push(i);
    }

    void remove(Key i) {
        Key removeIndex = indexs[i];
        Key lastIndex = elemens.size() - 1;

        if (removeIndex != lastIndex) {
            elemens[removeIndex] = elemens[lastIndex];
            indexs[elemens[lastIndex]] = removeIndex;
        }
        elemens.pop();
    }

    void fremove(Key i) {
        if (!contains(i)) {
            return;
        }
        Key removeIndex = indexs[i];
        Key lastIndex = elemens.size() - 1;

        if (removeIndex != lastIndex) {
            elemens[removeIndex] = elemens[lastIndex];
            indexs[elemens[lastIndex]] = removeIndex;
        }
        elemens.pop();
    }

    bool contains(Key i) const {
        return indexs[i] < (Key)elemens.size() && elemens[indexs[i]] == i;
    }

    int size() const {
        return elemens.size();
    }
    bool empty() const {
        return elemens.empty();
    }

    Key operator[](int index) const {
        return elemens[index];
    }

    auto begin() -> decltype(elemens.begin()) {
        return elemens.begin();
    }
    auto end() -> decltype(elemens.begin()) {
        return elemens.end();
    }
    auto begin() const -> decltype(elemens.begin()) {
        return elemens.begin();
    }
    auto end() const -> decltype(elemens.begin()) {
        return elemens.end();
    }

    void extend(const CapSet<CAP>& r) {
        for (int s : r) {
            add(s);
        }
    }
    void fextend(const CapSet<CAP>& r) {
        for (int s : r) {
            fadd(s);
        }
    }

    template <class RAND>
    const Key& choice(RAND&& r) const {
        return elemens.choice(r);
    }

    const CapArr<Key, CAP>& RefArray() const {
        return elemens;
    }
};


#include <bit>


template <class T, int CAP>
struct CapMap {
   private:
    using Key = typename conditional<CAP <= 256, u8, u32>::type;
    CapArr<pr2<Key, T>, CAP>
        elemens;  
    array<Key, CAP> indexs;  


   public:

    bool operator==(const CapMap<T, CAP>& r) const {
        if (elemens.size() != r.elemens.size()) {
            return false;
        }
        for (auto&& [k, v] : elemens) {
            if (!r.contains(k)) {
                return false;
            }
            if (v != r.get(k)) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const CapMap<T, CAP>& r) const {
        return !((*this) == r);
    }

    constexpr int capacity() {
        return CAP;
    }

    void clear() {
        elemens.clear();
    }
    void Clear() {
        clear();
    }

    void set(Key i, const T& value) {
        if (contains(i)) {
            auto& e = elemens[indexs[i]];
            e.b = value;
        }
        else {
            indexs[i] = elemens.size();
            auto& e = elemens.push();
            e.a = i;
            e.b = value;
        }
    }

    const T& get(Key i) const {
        return elemens[indexs[i]].b;
    }

    const T& get(Key i, const T& defaultValue) const {
        if (contains(i)) {
            return elemens[indexs[i]].b;
        }
        return defaultValue;
    }

    T& fref(Key i) {
        if (contains(i)) {
            return elemens[indexs[i]].b;
        }
        else {
            indexs[i] = elemens.size();
            auto& e = elemens.push();
            e.a = i;
            return e.b;
        }
    }

    T& ref(Key i) {
        return elemens[indexs[i]].b;
    }

    void AddValue(Key i, const T& addValue, const T& defaultValue) {
        if (contains(i)) {
            elemens[indexs[i]].b += addValue;
        }
        else {
            indexs[i] = elemens.size();
            auto& e = elemens.push();
            e.a = i;
            e.b = defaultValue + addValue;
        }
    }

    void remove(Key i) {
        Key removeIndex = indexs[i];
        Key lastIndex = elemens.size() - 1;

        if (removeIndex != lastIndex) {
            elemens[removeIndex] = elemens[lastIndex];
            indexs[elemens[lastIndex].a] = removeIndex;
        }
        elemens.pop();
    }

    bool contains(Key i) const {
        return indexs[i] < (Key)elemens.size() && elemens[indexs[i]].a == i;
    }

    int size() const {
        return elemens.size();
    }
    bool empty() const {
        return elemens.empty();
    }

    Key AtIndex(int index) const {
        return elemens[index].a;
    }

    auto begin() -> decltype(elemens.begin()) {
        return elemens.begin();
    }
    auto end() -> decltype(elemens.begin()) {
        return elemens.end();
    }
    auto begin() const -> decltype(elemens.begin()) {
        return elemens.begin();
    }
    auto end() const -> decltype(elemens.begin()) {
        return elemens.end();
    }
};


template <class T, int CAPACITY>
struct CapacityRingDeque {
private:
	array<T, CAPACITY> buf_;
	int head_ = 0;		
	int tail_ = 0;		
	int count_ = 0;

public:

	constexpr int capacity() const {
		return CAPACITY;
	}

	inline void clear() {
		head_ = 0;
		tail_ = 0;
		count_ = 0;
	}

	inline void push_back(const T& v) {
		VASSERT(count_ < CAPACITY);

		buf_[tail_] = v;
		++tail_;
		if (tail_ >= CAPACITY) {
			tail_ = 0;
		}
		++count_;
	}

	inline void push_front(const T& v) {
		VASSERT(count_ < CAPACITY);

		--head_;
		if (head_ < 0) {
			head_ = CAPACITY - 1;
		}
		buf_[head_] = v;
		++count_;
	}

	inline void pop_back() {
		VASSERT(count_ > 0);

		--tail_;
		if (tail_ < 0) {
			tail_ = CAPACITY - 1;
		}
		--count_;
	}

	inline void pop_front() {
		VASSERT(count_ > 0);

		++head_;
		if (head_ >= CAPACITY) {
			head_ = 0;
		}
		--count_;
	}

	inline int size() const {
		return count_;
	}

	inline bool empty() const {
		return count_ == 0;
	}

	inline const T& front() const {
		return buf_[head_];
	}
	inline const T& back() const {
		if (tail_ == 0) {
			return buf_[CAPACITY - 1];
		}
		return buf_[tail_ - 1];
	}

	void GetArr(CapArr<T, CAPACITY>& a) const {
		a.clear();
		int j = head_;
		REP(i, count_) {
			a.push(buf_[j]);
			++j;
			if (j >= CAPACITY) {
				j = 0;
			}
		}
	}
};

#define NO_SWAP 0

template <class T, int CAP, class CMP>
struct CapIntervalHeap {
    CMP cmp;
    CapArr<T, CAP> buf_;  

    bool empty() const {
        return buf_.empty();
    }
    int size() const {
        return buf_.size();
    }
    constexpr int capacity() const {
        return CAP;
    }

    void clear() {
        buf_.clear();
    }

    const T& GetMin() const {
        return buf_[0];
    }

    const T& GetMax() const {
        return buf_.size() < 2 ? buf_[0] : buf_[1];
    }

    void push(const T& v) {
        int c = buf_.size();
        buf_.push(v);

        if (c == 0) {
        }
        else if (c == 1) {
            if (cmp(buf_[1], buf_[0])) {
                swap(buf_[1], buf_[0]);
            }
        }
        else if (c & 1) {
            int cr = c;
            int cl = cr - 1;
            if (cmp(buf_[cr], buf_[cl])) {
                swap(buf_[cr], buf_[cl]);

                UpMin(cl);
            }
            else {
                UpMax(cr);
            }
        }
        else {
            int pl = (((c >> 1) - 1) & ~1);
            int pr = (pl | 1);

            if (cmp(buf_[c], buf_[pl])) {
                swap(buf_[c], buf_[pl]);
                UpMin(pl);
            }
            else if (cmp(buf_[pr], buf_[c])) {
                swap(buf_[pr], buf_[c]);
                UpMax(pr);
            }
            else {
            }
        }
    }

    void PopMin() {
        if (buf_.size() <= 1) {
            buf_.pop();
        }
        else {
            buf_[0] = buf_.back();
            buf_.pop();
            DownMin(0);
        }
    }

    void PopMax() {
        if (buf_.size() <= 2) {
            buf_.pop();
        }
        else {
            buf_[1] = buf_.back();
            buf_.pop();
            DownMax(1);
        }
    }

    T ReplaceMin(const T& v) {
        T ret = buf_[0];
        buf_[0] = v;

        if (buf_.size() >= 2) {
            if (cmp(buf_[1], buf_[0])) {
                swap(buf_[1], buf_[0]);
            }
            DownMin(0);
        }

        return ret;
    }




   private:
    void UpMin(int c) {
        T v = buf_[c];
        while (c > 1) {
            int p = (((c >> 1) - 1) & ~1);
            if (cmp(v, buf_[p])) {
                buf_[c] = buf_[p];
                c = p;
            }
            else {
                break;
            }
        }
        buf_[c] = v;
    }


    void UpMax(int c) {
        T v = buf_[c];
        while (c > 1) {
            int p = (((c >> 1) - 1) | 1);
            if (cmp(buf_[p], v)) {
                buf_[c] = buf_[p];
                c = p;
            }
            else {
                break;
            }
        }
        buf_[c] = v;
    }


    void DownMin(int p) {
        VASSERT((p & 1) == 0);
        int size = buf_.size();
        T v = buf_[p];
        for (int l = ((p << 1) + 2); l < size; l = ((p << 1) + 2)) {
            int r = l + 2;

            int c = (r < size && cmp(buf_[r], buf_[l])) ? r : l;

            if (cmp(buf_[c], v)) {
                buf_[p] = buf_[c];
                p = c;
            }
            else {
                break;
            }

            int pr = p | 1;
            if (pr >= size) {
                break;
            }
            if (cmp(buf_[pr], v)) {
                swap(buf_[pr], v);
            }
        }
        buf_[p] = v;
    }


    void DownMax(int p) {
        VASSERT((p & 1) == 1);
        int size = buf_.size();
        T v = buf_[p];

        for (int l = ((p << 1) + 1); l < size; l = ((p << 1) + 1)) {
            int r = l + 2;

            int c = (r < size && cmp(buf_[l], buf_[r])) ? r : l;

            if (cmp(v, buf_[c])) {
                buf_[p] = buf_[c];
                p = c;
            }
            else {
                break;
            }

            int pl = p ^ 1;
            if (cmp(v, buf_[pl])) {
                swap(v, buf_[pl]);
            }
        }
        buf_[p] = v;
    }

};

template<class T, class COMPARERE = less<T>>
struct PriorityQueue : public priority_queue<T, vector<T>, COMPARERE> {


	template <class D>
	void Cut(int size, D&& deleter) {
		if ((int)this->c.size() > size) {
			int orgSize = (int)this->c.size();
			for (int i = size; i < orgSize; ++i) {
				deleter(this->c[i]);
			}
			this->c.resize(size);
		}
	}

	vector<T>& Container() {
		return this->c;
	}

	void Cut(int size) {
		if ((int)this->c.size() > size) {
			this->c.resize(size);
		}
	}

	void Clear() {
		this->c.clear();
	}
};



template <int S>
struct CheckMapS {
   private:
    array<u32, S> checked_ = {};
    u32 mark_ = 1;

   public:
    void Clear() {
        ++mark_;

        if (mark_ == 0) {
            checked_ = {};
            ++mark_;
        }
    }
    bool IsChecked(int i) const {
        return checked_[i] == mark_;
    }
    void Check(int i) {
        checked_[i] = mark_;
    }
    void Reset(int i) {
        checked_[i] = mark_ - 1;
    }
    bool Enter(int i) {
        if (checked_[i] == mark_) {
            return false;
        }
        checked_[i] = mark_;
        return true;
    }

    bool operator[](int i) const {
        return checked_[i] == mark_;
    }

    bool operator==(const CheckMapS<S>& r) const {
        REP(i, S) {
            if (this->IsChecked(i) != r.IsChecked(i)) {
                return false;
            }
        }
        return true;
    }
};

template <class T, int S>
struct CheckMapDataS {
   private:
    array<T, S> data_;
    array<u32, S> checked_ = {};
    u32 mark_ = 1;

   public:
    void Clear() {
        ++mark_;

        if (mark_ == 0) {
            checked_ = {};
            ++mark_;
        }
    }

    bool IsChecked(int i) const {
        return checked_[i] == mark_;
    }
    void Check(int i) {
        checked_[i] = mark_;
    }

    void Set(int i, const T& value) {
        checked_[i] = mark_;
        data_[i] = value;
    }

    void Reset(int i) {
        checked_[i] = mark_ - 1;
    }
    const T& Get(int i) const {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
    T& Ref(int i) {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
    const T& Ref(int i) const {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
    T& operator[](int i) {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
    const T& operator[](int i) const {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }

    T GetIf(int i, const T& defaultValue) const {
        if (checked_[i] == mark_) {
            return data_[i];
        }
        return defaultValue;
    }
};


template <class T>
T* mem_alloc(int size) {
#ifdef _MSC_VER
    return (T*)_aligned_malloc(size * sizeof(T), alignof(T));
#else
    return (T*)std::aligned_alloc(alignof(T), size * sizeof(T));
#endif
}

template <class T>
void mem_free(T* ptr) {
#ifdef _MSC_VER
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

namespace dvec_trivial {

template <class T>
struct dvec {
   private:
    static_assert(std::is_trivially_destructible_v<T>);
    static_assert(std::is_trivially_copyable_v<T>);

    static constexpr int DefaultMallocSize = 1;

   public:
    using dvec_tag = void;  

   private:
    T* ptr_;
    int size_;  
    int cap_;   

   public:
    ~dvec() {
        if (ptr_) {
            mem_free(ptr_);
        }
    }

    dvec() : ptr_(nullptr), size_(0), cap_(0) {
    }

    explicit dvec(int size) : ptr_(mem_alloc<T>(size)), size_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new (&ptr_[i]) T();
        }
    }

    explicit dvec(const std::vector<T>& vec)
        : ptr_(mem_alloc<T>((int)vec.size())), size_((int)vec.size()), cap_((int)vec.size()) {
        for (int i = 0; i < size_; ++i) {
            new (&ptr_[i]) T(vec[i]);
        }
    }

    dvec(int size, const T& v) : ptr_(mem_alloc<T>(size)), size_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new (&ptr_[i]) T(v);
        }
    }

    dvec(const dvec& r) {
        if (r.ptr_) {
            ptr_ = mem_alloc<T>(r.size_);
            size_ = r.size_;
            cap_ = r.size_;
            memcpy(ptr_, r.ptr_, sizeof(T) * size_);
        }
        else {
            ptr_ = nullptr;
            size_ = 0;
            cap_ = 0;
        }
    }

    dvec(dvec&& r) noexcept : ptr_(r.ptr_), size_(r.size_), cap_(r.cap_) {
        r.ptr_ = nullptr;
        r.size_ = 0;
        r.cap_ = 0;
    }

    void operator=(const dvec& r) {
        if (r.ptr_) {
            if (r.size_ > cap_) {
                mem_free(ptr_);
                ptr_ = mem_alloc<T>(r.size_);
                cap_ = r.size_;
            }
            size_ = r.size_;
            memcpy(ptr_, r.ptr_, sizeof(T) * size_);
        }
        else {
            size_ = 0;
        }
    }

    void operator=(dvec&& r) noexcept {
        std::swap(ptr_, r.ptr_);
        std::swap(size_, r.size_);
        std::swap(cap_, r.cap_);
    }

    bool operator==(const dvec& r) const {
        if (size_ != r.size_) {
            return false;
        }
        for (int i = 0; i < size_; ++i) {
            if (ptr_[i] != r.ptr_[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const dvec& r) const {
        return !(*this == r);
    }

    bool empty() const {
        return size_ == 0;
    }
    inline void clear() {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (int i = 0; i < size_; ++i) {
                ptr_[i].~T();
            }
        }
        size_ = 0;
    }
    inline int size() const {
        return size_;
    }
    inline int capacity() const {
        return cap_;
    }
    inline T& operator[](int i) {
        VASSERT(ptr_);
        VASSERT(i >= 0 && i <= size_);
        return ptr_[i];
    }
    inline const T& operator[](int i) const {
        VASSERT(ptr_);
        VASSERT(i >= 0 && i <= size_);
        return ptr_[i];
    }
    inline T* data() {
        return ptr_;
    }
    inline const T* data() const {
        return ptr_;
    }

    T* begin() {
        return ptr_;
    }
    const T* begin() const {
        return ptr_;
    }
    T* end() {
        return ptr_ + size_;
    }
    const T* end() const {
        return ptr_ + size_;
    }

    T& front() {
        VASSERT(size_ > 0);
        return *ptr_;
    }
    const T& front() const {
        VASSERT(size_ > 0);
        return *ptr_;
    }
    T& back() {
        VASSERT(size_ > 0);
        return ptr_[size_ - 1];
    }
    const T& back() const {
        VASSERT(size_ > 0);
        return ptr_[size_ - 1];
    }


    void reserve(int size) {
        VASSERT(size >= 0);
        if (size > cap_) {
            int newCap = max(DefaultMallocSize, max(size, cap_ * 2));
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                memcpy(newPtr, ptr_, sizeof(T) * size_);
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;
        }
    }

    void resizeR(int size) {
        VASSERT(size >= 0);
        reserve(size);
        size_ = size;
    }
    void resize(int size) {
        VASSERT(size >= 0);
        reserve(size);
        size_ = size;
    }
    void resize(int size, const T& v) {
        VASSERT(size >= 0);
        reserve(size);
        for (int i = size_; i < size; ++i) {
            ptr_[i] = v;
        }
        size_ = size;
    }

    void assign(int size, const T& v) {
        VASSERT(size >= 0);
        if (size > cap_) {
            mem_free(ptr_);
            ptr_ = mem_alloc<T>(size);
            cap_ = size;
        }
        for (int i = 0; i < size; ++i) {
            ptr_[i] = v;
        }
        size_ = size;
    }

    [[nodiscard]] T& pushR() {
        if (size_ == cap_) {
            reserve(size_ + 1);
        }
        return ptr_[size_++];
    }

    void push(const T& v) {
        pushR() = v;
    }

    void pushEmpty() {
        if (size_ == cap_) {
            reserve(size_ + 1);
        }
        ++size_;
    }

    void pop() {
        VASSERT(size_ > 0);
        --size_;
    }

    void remove(int idx) {
        VASSERT(idx >= 0 && idx < size_);
        if (idx + 1 < size_) {
            memmove(ptr_ + idx, ptr_ + idx + 1, sizeof(T) * (size_ - 1 - idx));
        }
        --size_;
    }

    void removeSwap(int idx) {
        VASSERT(idx >= 0 && idx < size_);
        ptr_[idx] = ptr_[size_ - 1];
        --size_;
    }

    void swapInsert(int idx, const T& v) {
        VASSERT(idx >= 0 && idx <= size_);
        if (size_ == cap_) {
            reserve(size_ + 1);
        }
        ptr_[size_] = ptr_[idx];
        ptr_[idx] = v;
        ++size_;
    }

    [[nodiscard]] T& insertR(int idx) {
        VASSERT(idx >= 0 && idx <= size_);

        if (size_ == cap_) {
            int newCap = std::max(DefaultMallocSize, cap_ * 2);
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                memcpy(newPtr, ptr_, sizeof(T) * idx);
                if (idx < size_) {
                    memcpy(newPtr + idx + 1, ptr_ + idx, sizeof(T) * (size_ - idx));
                }
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;
        }
        else {
            if (idx < size_) {
                memmove(ptr_ + idx + 1, ptr_ + idx, sizeof(T) * (size_ - idx));
            }
        }
        ++size_;
        return ptr_[idx];
    }

    void resizeArea(int start, int end, int areaSize) {
        VASSERT(start >= 0 && end >= 0);
        VASSERT(start <= end);
        VASSERT(areaSize >= 0);

        int newSize = size_ - (end - start) + areaSize;

        if (newSize > cap_) {
            int newCap = std::max(DefaultMallocSize, cap_ * 2);
            if (newSize > newCap) {
                newCap = newSize * 3 / 2;
            }
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                memcpy(newPtr, ptr_, sizeof(T) * start);
                if (end < size_) {
                    memcpy(newPtr + start + areaSize, ptr_ + end, sizeof(T) * (size_ - end));
                }
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;
        }
        else {
            if (end < size_) {
                memmove(ptr_ + start + areaSize, ptr_ + end, sizeof(T) * (size_ - end));
            }
        }
        size_ = newSize;
    }

    void RemoveInsert(int start, int end, const T* p, int size) {
        int newEnd = start + size;
        resizeArea(start, end, size);
        memcpy(data() + start, p, sizeof(T) * size);
    }


    void extend(const dvec<T>& r) {
        RemoveInsert(size_, size_, r.ptr_, r.size_);
    }

    int find(const T& v) const {
        for (int i = 0; i < size_; ++i) {
            if (ptr_[i] == v) {
                return i;
            }
        }
        return -1;
    }
    bool contains(const T& v) const {
        for (int i = 0; i < size_; ++i) {
            if (ptr_[i] == v) {
                return true;
            }
        }
        return false;
    }

    template <class RAND>
    const T& choice(RAND&& r) const {
        VASSERT(size_ > 0);
        return ptr_[r(size_)];
    }
};
};  

namespace dvec_non_trivial {

template <class T>
struct dvec {
   private:

    static constexpr int DefaultMallocSize = 1;

   public:
    using dvec_tag = void;  

   private:
    T* ptr_;
    int size_;     
    int consted_;  
    int cap_;      

   public:
    ~dvec() {
        if (ptr_) {
            for (int i = 0; i < consted_; ++i) {
                ptr_[i].~T();
            }
            mem_free(ptr_);
        }
    }

    dvec() : ptr_(nullptr), size_(0), consted_(0), cap_(0) {
    }

    explicit dvec(int size) : ptr_(mem_alloc<T>(size)), size_(size), consted_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new (&ptr_[i]) T();
        }
    }

    explicit dvec(const std::vector<T>& vec) {
        int size = (int)vec.size();
        ptr_ = mem_alloc<T>(size);
        size_ = size;
        consted_ = size;
        cap_ = size;
        ;
        for (int i = 0; i < size; ++i) {
            new (&ptr_[i]) T(vec[i]);
        }
    }

    dvec(int size, const T& v) : ptr_(mem_alloc<T>(size)), size_(size), consted_(size), cap_(size) {
        for (int i = 0; i < size; ++i) {
            new (&ptr_[i]) T(v);
        }
    }

    dvec(const dvec& r) {
        if (r.ptr_) {
            ptr_ = mem_alloc<T>(r.size_);
            size_ = r.size_;
            consted_ = size_;  
            cap_ = r.size_;

            for (int i = 0; i < size_; ++i) {
                new (&ptr_[i]) T(r.ptr_[i]);
            }
        }
        else {
            ptr_ = nullptr;
            size_ = 0;
            consted_ = 0;
            cap_ = 0;
        }
    }

    dvec(dvec&& r) noexcept : ptr_(r.ptr_), size_(r.size_), consted_(r.consted_), cap_(r.cap_) {
        r.ptr_ = nullptr;
        r.size_ = 0;
        r.consted_ = 0;
        r.cap_ = 0;
    }

    void operator=(const dvec& r) {
        if (r.size_ <= cap_) {
            for (int i = 0; i < r.size_; ++i) {
                if (i < consted_) {
                    ptr_[i] = r.ptr_[i];
                }
                else {
                    new (&ptr_[i]) T(r.ptr_[i]);
                }
            }
            size_ = r.size_;
            chmax(consted_, size_);
        }
        else {
            for (int i = 0; i < consted_; ++i) {
                ptr_[i].~T();
            }

            mem_free(ptr_);
            ptr_ = mem_alloc<T>(r.cap_);

            for (int i = 0; i < r.size_; ++i) {
                new (&ptr_[i]) T(r.ptr_[i]);
            }

            size_ = r.size_;
            consted_ = size_;
            cap_ = r.cap_;
        }
    }

    void operator=(dvec&& r) noexcept {
        std::swap(ptr_, r.ptr_);
        std::swap(size_, r.size_);
        std::swap(consted_, r.consted_);
        std::swap(cap_, r.cap_);
    }

    bool operator==(const dvec& r) const {
        if (size_ != r.size_) {
            return false;
        }
        for (int i = 0; i < size_; ++i) {
            if (ptr_[i] != r.ptr_[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const dvec& r) const {
        return !(*this == r);
    }

    bool empty() const {
        return size_ == 0;
    }
    inline void clear() {
        size_ = 0;
    }
    inline int size() const {
        return size_;
    }
    inline int constructed() const {
        return consted_;
    }
    inline int capacity() const {
        return cap_;
    }
    inline T& operator[](int i) {
        VASSERT(ptr_);
        VASSERT(i >= 0 && i <= size_);
        return ptr_[i];
    }
    inline const T& operator[](int i) const {
        VASSERT(ptr_);
        VASSERT(i >= 0 && i <= size_);
        return ptr_[i];
    }
    inline T* data() {
        return ptr_;
    }
    inline const T* data() const {
        return ptr_;
    }

    T* begin() {
        return ptr_;
    }
    const T* begin() const {
        return ptr_;
    }
    T* end() {
        return ptr_ + size_;
    }
    const T* end() const {
        return ptr_ + size_;
    }

    T& front() {
        VASSERT(size_ > 0);
        return *ptr_;
    }
    const T& front() const {
        VASSERT(size_ > 0);
        return *ptr_;
    }
    T& back() {
        VASSERT(size_ > 0);
        return ptr_[size_ - 1];
    }
    const T& back() const {
        VASSERT(size_ > 0);
        return ptr_[size_ - 1];
    }

    void reserve(int size) {
        VASSERT(size >= 0);
        if (size > cap_) {
            int newCap = max(DefaultMallocSize, max(size, cap_ * 2));
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                for (int i = 0; i < consted_; ++i) {
                    new (&newPtr[i]) T(std::move(ptr_[i]));
                }
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;
        }
    }

    void resizeR(int size) {
        VASSERT(size >= 0);
        if (size <= cap_) {
            if (size > consted_) {
                for (int i = consted_; i < size; ++i) {
                    new (&ptr_[i]) T();
                }
                consted_ = size;
            }
            size_ = size;
        }
        else {
            int newCap = max(DefaultMallocSize, max(size, cap_ * 2));
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                for (int i = 0; i < consted_; ++i) {
                    new (&newPtr[i]) T(std::move(ptr_[i]));
                }
                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;

            VASSERT(size > consted_);
            for (int i = consted_; i < size; ++i) {
                new (&ptr_[i]) T();
            }
            consted_ = size;
            size_ = size;
        }
    }
    void resize(int size) {
        resizeR(size);
    }

    void assign(int size, const T& v) {
        VASSERT(size >= 0);
        if (size <= cap_) {
            for (int i = 0; i < size; ++i) {
                if (i < consted_) {
                    ptr_[i] = v;
                }
                else {
                    new (&ptr_[i]) T(v);
                }
            }
            size_ = size;
            chmax(consted_, size_);
        }
        else {
            for (int i = 0; i < consted_; ++i) {
                ptr_[i].~T();
            }

            int newCap = max(DefaultMallocSize, max(size, cap_ * 2));
            mem_free(ptr_);
            ptr_ = mem_alloc<T>(newCap);

            for (int i = 0; i < size; ++i) {
                new (&ptr_[i]) T(v);
            }
            size_ = size;
            consted_ = size_;
            cap_ = newCap;
        }
    }

    [[nodiscard]] T& pushR() {
        if (size_ < consted_) {
            ++size_;
            return ptr_[size_ - 1];
        }
        else if (size_ == cap_) {
            reserve(size_ + 1);
        }
        new (&ptr_[size_]) T();
        ++size_;
        VASSERT(consted_ < size_);
        consted_ = size_;

        return ptr_[size_ - 1];
    }
    [[nodiscard]] T& push() {
        return pushR();
    }
    void push(const T& v) {
        pushR() = v;
    }

    void pushEmpty() {
        if (size_ < consted_) {
            ++size_;
        }
        else if (size_ == cap_) {
            reserve(size_ + 1);
        }
        new (&ptr_[size_]) T();
        ++size_;
        VASSERT(consted_ < size_);
        consted_ = size_;
    }

    void pop() {
        --size_;
    }

    void remove(int idx) {
        VASSERT(idx >= 0 && idx < size_);
        if (idx + 1 < size_) {
            T tmp = std::move(ptr_[idx]);
            for (int i = idx + 1; i < size_; ++i) {
                ptr_[i - 1] = std::move(ptr_[i]);
            }
            ptr_[size_ - 1] = std::move(tmp);
        }
        --size_;
    }

    [[nodiscard]] T& insertR(int idx) {
        VASSERT(idx >= 0 && idx <= size_);

        if (size_ == cap_) {
            int newCap = std::max(DefaultMallocSize, cap_ * 2);
            T* newPtr = mem_alloc<T>(newCap);

            if (ptr_) {
                for (int i = 0; i < idx; ++i) {
                    new (&newPtr[i]) T(std::move(ptr_[i]));
                }
                if (idx < consted_) {
                    for (int i = idx; i < consted_; ++i) {
                        new (&newPtr[i + 1]) T(std::move(ptr_[i]));
                    }
                }
                ++consted_;

                new (&newPtr[idx]) T();

                mem_free(ptr_);
            }

            ptr_ = newPtr;
            cap_ = newCap;
        }
        else {
            if (idx < size_) {
                if (size_ < consted_) {
                    T tmp = std::move(ptr_[size_]);
                    RFOR(i, idx, size_) {
                        ptr_[i + 1] = std::move(ptr_[i]);
                    }
                    ptr_[idx] = std::move(tmp);
                }
                else {
                    VASSERT(size_ == consted_);

                    RFOR(i, idx, size_) {
                        if (i + 1 == size_) {
                            new (&ptr_[i + 1]) T(std::move(ptr_[i]));
                        }
                        else {
                            ptr_[i + 1] = std::move(ptr_[i]);
                        }
                    }
                    new (&ptr_[idx]) T();
                    ++consted_;
                }
            }
            else {
                if (size_ < consted_) {
                }
                else {
                    new (&ptr_[idx]) T();
                    ++consted_;
                }
            }
        }
        ++size_;
        return ptr_[idx];
    }

    template <class RAND>
    const T& choice(RAND&& r) const {
        VASSERT(size_ > 0);
        return ptr_[r(size_)];
    }
};
};  

template <class T>
using dvec = conditional_t<is_trivial_v<T>, dvec_trivial::dvec<T>, dvec_non_trivial::dvec<T>>;



template <class T, class CMP = std::less<T>>
struct dheap {
    CMP cmp;
    dvec<T> buf_;

    void reserve(int size) {
        buf_.reserve(size);
    }

    int capacity() const {
        return buf_.capacity();
    }

    void clear() {
        buf_.clear();
    }
    bool empty() const {
        return buf_.empty();
    }
    bool exist() const {
        return !buf_.empty();
    }
    int size() const {
        return buf_.size();
    }

    const T& top() {
        return buf_.front();
    }

    void push(const T& v) {
        buf_.pushR() = v;
        Up(buf_.size() - 1);
    }

    T pop() {
        T ret = buf_.front();
        if (buf_.size() >= 2) {
            swap(buf_[0], buf_.back());
            buf_.pop();
            Down(0);
        }
        else {
            buf_.pop();
        }
        return ret;
    }

    T popPush(const T& v) {
        T ret = buf_.front();
        buf_[0] = v;
        if (buf_.size() >= 2) {
            Down(0);
        }
        return ret;
    }


    T& BeginUpdateTop() {
        return buf_.front();
    }
    void EndUpdateTop() {
        if (buf_.size() >= 2) {
            Down(0);
        }
    }

   private:
    void Up(int c) {
        T v = buf_[c];
        while (c > 0) {
            int p = ((c - 1) >> 1);
            if (cmp(buf_[p], v)) {
                buf_[c] = buf_[p];
                c = p;
            }
            else {
                break;
            }
        }
        buf_[c] = v;
    }

    void Down(int p) {
        int size = buf_.size();
        T v = buf_[p];
        while (true) {
            int l = (p << 1) + 1;
            int r = l + 1;

            int c;
            if (r < size) {
                c = cmp(buf_[r], buf_[l]) ? l : r;
            }
            else if (l < size) {
                c = l;
            }
            else {
                break;
            }

            if (cmp(v, buf_[c])) {
                buf_[p] = buf_[c];
                p = c;
            }
            else {
                break;
            }
        }
        buf_[p] = v;
    }
};


template <class K, class V>
struct dhmap {
    uint32_t m_capacity = 0;
    K* m_keys = nullptr;
    V* m_values = nullptr;
    uint8_t* m_states = nullptr;  
    uint32_t m_mask = 0;

    uint32_t m_count = 0;

    ~dhmap() {
        free(m_keys);
        free(m_values);
        free(m_states);
    }

    dhmap() {
    }

    dhmap(const dhmap&) = delete;
    void operator=(const dhmap&) = delete;

    dhmap(dhmap&& r) noexcept
        : m_capacity(r.m_capacity),
          m_keys(r.m_keys),
          m_values(r.m_values),
          m_states(r.m_states),
          m_mask(r.m_mask),
          m_count(r.m_count)
    {
        r.m_capacity = 0;
        r.m_keys = nullptr;
        r.m_values = nullptr;
        r.m_states = nullptr;
        r.m_mask = 0;
        r.m_count = 0;
    }

    void init(uint32_t capacity) {
        if (capacity <= m_capacity) {
            clear();
            return;
        }

        free(m_keys);
        free(m_values);
        free(m_states);

        m_capacity = 1ULL << bit_width(capacity);
        m_mask = m_capacity - 1;
        m_keys = (K*)malloc(m_capacity * sizeof(K));
        m_values = (V*)malloc(m_capacity * sizeof(V));
        m_states = (uint8_t*)calloc(m_capacity, sizeof(uint8_t));
        m_count = 0;
    }

    void clear() {
        memset(m_states, 0, m_capacity * sizeof(uint8_t));
        m_count = 0;
    }

    void set(K key, const V& v) {
        VASSERT(m_count <
                m_capacity);  

        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i]) {
                if (key == m_keys[i]) {
                    m_values[i] = v;
                    break;
                }
            }
            else {
                m_states[i] = 1;
                m_keys[i] = key;
                m_values[i] = v;
                ++m_count;
                break;
            }
            (++i) &= m_mask;
        }
    }

    V* try_get(K key) {
        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i]) {
                if (key == m_keys[i]) {
                    return &m_values[i];
                }
            }
            else {
                break;
            }
            (++i) &= m_mask;
        }
        return nullptr;
    }


    bool contains(K key) const {
        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i]) {
                if (key == m_keys[i]) {
                    return true;
                }
            }
            else {
                break;
            }
            (++i) &= m_mask;
        }
        return false;
    }


    int size() const {
        return m_count;
    }

    int capacity() const {
        return (int)m_capacity;
    }
    bool Direct(int i, K& k, V& v) const {
        if (m_states[i]) {
            k = m_keys[i];
            v = m_values[i];
            return true;
        }
        return false;
    }

    double GetUseRate() const {
        return m_count / (double)m_capacity;
    }
};

template <class K, class V>
struct dhmapR {
    uint32_t m_capacity = 0;
    K* m_keys = nullptr;
    V* m_values = nullptr;
    uint8_t* m_states = nullptr;  
    uint32_t m_mask = 0;

    uint32_t m_count = 0;

    dhmapR(const dhmapR&) = delete;
    void operator=(const dhmapR&) = delete;
    dhmapR() {
    }

    ~dhmapR() {
        free(m_keys);
        free(m_values);
        free(m_states);
    }

    void init(uint32_t capacity) {
        if (capacity <= m_capacity) {
            clear();
            return;
        }

        free(m_keys);
        free(m_values);
        free(m_states);

        m_capacity = 1ULL << bit_width(capacity);
        m_mask = m_capacity - 1;
        m_keys = (K*)malloc(m_capacity * sizeof(K));
        m_values = (V*)malloc(m_capacity * sizeof(V));
        m_states = (uint8_t*)calloc(m_capacity, sizeof(uint8_t));
        m_count = 0;
    }

    void clear() {
        memset(m_states, 0, m_capacity * sizeof(uint8_t));
        m_count = 0;
    }

    inline void set(K key, const V& v) {
        VASSERT(m_count <
                m_capacity);  

        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i] == 1) {
                if (key == m_keys[i]) {
                    m_values[i] = v;
                    break;
                }
            }
            else {
                m_states[i] = 1;
                m_keys[i] = key;
                m_values[i] = v;
                ++m_count;
                break;
            }
            (++i) &= m_mask;
        }
    }
    inline V* try_get(K key) {
        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i] == 0) {
                break;
            }
            else if (m_states[i] == 1) {
                if (key == m_keys[i]) {
                    return &m_values[i];
                }
            }
            (++i) &= m_mask;
        }
        return nullptr;
    }

    void remove(K key) {
        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i] == 1) {
                if (key == m_keys[i]) {
                    m_states[i] = 2;
                    --m_count;
                    return;
                }
            }
            (++i) &= m_mask;
        }
        VABORT();
    }

    bool contains(K key) const {
        uint32_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i] == 0) {
                break;
            }
            else if (m_states[i] == 1) {
                if (key == m_keys[i]) {
                    return true;
                }
            }
            (++i) &= m_mask;
        }
        return false;
    }


    int size() const {
        return m_count;
    }

    int capacity() const {
        return (int)m_capacity;
    }


    inline double GetUseRate() const {
        return m_count / (double)m_capacity;
    }
};


#define DEF_CONTAINERS(name, size)                     \
    template <class TYPE>                              \
    using name##Box = array<TYPE, size>;               \
    template <class TYPE>                              \
    using name##Arr = CapArr<TYPE, size>;              \
    template <class TYPE>                              \
    using name##Que = CapQue<TYPE, size>;              \
    using name##Set = CapSet<size>;                    \
    template <class TYPE>                              \
    using name##Map = CapMap<TYPE, size>;              \
    template <class TYPE>                              \
    using name##Deque = CapacityRingDeque<TYPE, size>; \
    using name##Check = CheckMapS<size>;

constexpr int N_max = 20;
constexpr int NN_max = N_max * N_max;
DEF_CONTAINERS(N, N_max);
DEF_CONTAINERS(NN, NN_max);

int N;
int NN;
int K;
int T;

#define PARAM_CATEGORY(NAME, VALUE, ...) int NAME = VALUE;
#define PARAM_INT(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, ...) int NAME = VALUE;
#define PARAM_DOUBLE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, ...) double NAME = VALUE;


#define PARAM_LOWER(v)
#define PARAM_UPPER(v)
#define START_TUNING
#define END_TUNING

#define PARAM_GROUP(NAME)
#define PARAM_GROUP_END

#define IF_START(cond)
#define IF_END


template <class T>
struct InputParam {
    T minValue_;
    T maxValue_;
    T value_;

    InputParam(T minValue, T maxValue) {
        minValue_ = minValue;
        maxValue_ = maxValue;
    }

    void SetValue(T value) {
        value_ = value;
    }

    double GetRate(double strong) const {
        double r = (value_ - minValue_) / (double)(maxValue_ - minValue_) * 2.0 - 1.0;  
        return r * strong;
    }
};

static double BlendDoubleParam(double baseValue, double minValue, double maxValue, initializer_list<double> rates) {
    double totalRate = 1;
    for (double rate : rates) {
        totalRate += rate;
    }

    double value = baseValue * totalRate;

    chmax(value, minValue);
    chmin(value, maxValue);

    return value;
}

static int BlendIntParam(double baseValue, int minValue, int maxValue, initializer_list<double> rates) {
    double totalRate = 1;
    for (double rate : rates) {
        totalRate += rate;
    }

    int value = (int)round(baseValue * totalRate);

    chmax(value, minValue);
    chmin(value, maxValue);

    return value;
}
#define PC PARAM_CATEGORY
#define PI PARAM_INT
#define PD PARAM_DOUBLE




constexpr
struct {

    PD(CurveCostSigma, 0.8658448114356756, 0.6, 0.8, 0.0);
    PD(BackCostSigma, 1.015055978659987, 0.9, 1.3, 0.0);
    PD(PastCurveCostSigma, 0.05514745359150296, 0.0, 0.05, 0.0);
    PD(FutureLineCostSigma, 1.3725199710975373, 1.25, 1.45, 0.0);

    PD(FutureSideStampColorScoreSigma, 0.3610564190270871, 0.2, 0.5, 0.0);
    PD(NextStampColorScoreSigma, 0.5886802676467332, 0.7, 0.83, 0.0);
    PD(SameStateScoreSigma, 0.9330177859335396, 0.9, 1.0, 0.0);
    PD(SameColorScoreSigma, 0.23371019109514315, 0.1, 0.2, 0.0);
    PD(ColorCountScoreSigma, 0.7509566605728574, 0.5, 0.7, 0.0);
    PD(StateCountScoreSigma, 1.4124092749895625, 1.2, 1.4, 0.0);
    PD(RuleCountScoreSigma, 0.26558418031973885, 0.1, 0.2, 0.0);
    PD(BalanceStateStepSigma, 1.346101653554972, 1.2, 1.4, 0.0);
    PD(ReuseScoreSigma, 1.404273177528121, 0.9, 1.1, 0.0);
    PD(UpdateNextColorScoreSigma, 0.00020582875640249432, 0.0, 0.005, 0.0);
    PD(AddScoreSigma, 0.04959800110231119, 0.1, 0.3, 0.0);
    PD(NewRuleScoreSigma, 0.09621890106754433, 0.0, 0.2, 0.0);
    PD(FutureScoreRateSigma, 0.6613475258718077, 0.8, 1.0, 0.0);
    PD(FutureFreeColorCountScoreSigma, 0.3524694288869308, 0.1, 0.5, 0.0);
    PD(FreeColorRuleScoreSigma, 0.848748545172213, 1.0, 1.3, 0.0);

    PI(AssignLoop, 2, 1, 4, 1);

    PD(RemakeParamBaseScale, 0.3232416496457002, 0.2, 0.4, 0.0);
    PD(AssignParamSigma, 0.21512790922393632, 0.1, 0.3, 0.0);

    PI(FirstRemakeCount, 5, 4, 8, 0);

    PD(CurveCost, 1.5819016042438356, 1.1, 1.4, 0.0);
    PD(BackCost, 1.4468115585112182, 1.3, 1.6, 0.0);
    PD(PastCurveCost, 1.3361288999611685, 1.0, 1.3, 0.0);
    PD(FutureLineCost, 1.4919551450874626, 1.2, 1.7, 0.0);

    PD(FutureSideStampColorScore, -0.0016951088710930582, -0.02, 0.0);
    PD(NextStampColorScore, -0.0001637941960756866, -0.0005, 0.0);
    PD(SameStateScore, 0.008571225119750057, 0.0, 0.02);
    PD(SameColorScore, 0.006515742180742845, 0.0, 0.01);
    PD(ColorCountScore, 0.7015240579191451, 0.5, 0.9);
    PD(StateCountScore, 4.341104053524752, 4.0, 4.5);
    PD(RuleCountScore, 0.09800088422245203, 0.0, 0.1);
    PD(BalanceStateStep, 1.31010990795681, 1.2, 1.4);
    PD(ReuseScore, 0.7014621783265278, 0.9, 1.1);
    PD(UpdateNextColorScore, 0.7997229486806241, 0.7, 1.0);
    PD(AddScore, 0.2840057868543338, 0.1, 0.5);
    PD(NewRuleScore, -0.4158141793541563, -0.5, -0.3);
    PD(FutureScoreRate, 21.11314283595058, 20.0, 25.0);
    PD(FutureFreeColorCountScore, 0.14040722839137385, 0.0, 0.3);
    PD(FreeColorRuleScore, -0.0006738914012691043, -0.01, 0.0);  

    START_TUNING;
    PD(WrongDirTime, 0.4, 0.0, 1.0, 0.0, 1.0);
    END_TUNING;



} HP;


#undef PC
#undef PI
#undef PD



template <class T, int CAP>
struct CCA {
   private:
    T ar[CAP];  
    int s;

   public:
    void init() {
        s = 0;
    }
    inline constexpr void push(const T& v) {
        ar[s++] = v;
    }
    inline constexpr void pop() {
        --s;
    }
    inline constexpr const T* begin() const {
        return &ar[0];
    }
    inline constexpr const T* end() const {
        return &ar[s];
    }
    inline constexpr T* begin() {
        return &ar[0];
    }
    inline constexpr T* end() {
        return &ar[s];
    }

    inline constexpr const T& operator()(int i) const {
        VASSERT(i >= 0 && i < CAP);
        return ar[i];
    }
    inline constexpr const T& operator[](int i) const {
        VASSERT(i >= 0 && i < CAP);
        return ar[i];
    }
    inline constexpr int size() const {
        return s;
    }

    T choice(Xor64& r) const {
        return ar[r(s)];
    }
};

#define USE_AROUND 1
#define USE_DISTMAT 1

enum class Dir : u8 {
    L = 0,
    U,
    R,
    D,
    N,
    Invalid,
};

constexpr array<Dir, 4> Dir4 = {
    Dir::L,
    Dir::U,
    Dir::R,
    Dir::D,
};

constexpr array<pint, 4> Around4 = {pint{-1, 0}, pint{0, -1}, pint{1, 0}, pint{0, 1}};
inline Dir RotateRight(Dir d) {
    constexpr Dir nexts[4] = {
        Dir::U,
        Dir::R,
        Dir::D,
        Dir::L,
    };
    return nexts[(int8_t)d];
}
inline Dir RotateLeft(Dir d) {
    constexpr Dir nexts[4] = {
        Dir::D,
        Dir::L,
        Dir::U,
        Dir::R,
    };
    return nexts[(int8_t)d];
}

inline Dir Back(Dir d) {
    return Dir(s8(d) ^ 2);
}

bool IsHorizontal(Dir dir) {
    return dir == Dir::L || dir == Dir::R;
}
bool IsVertical(Dir dir) {
    return dir == Dir::U || dir == Dir::D;
}

inline Dir CalcDirM(const pint& from, const pint& to) {
    if (from.x > to.x) {
        return Dir::L;
    }
    else if (from.y > to.y) {
        return Dir::U;
    }
    else if (from.x < to.x) {
        return Dir::R;
    }
    else if (from.y < to.y) {
        return Dir::D;
    }
    else {
        return Dir::N;
    }
}

inline char DirToChar(Dir dir) {
    VASSERT((int)dir >= 0 && (int)dir < 5);
    static const char chars[6] = {
        'L', 'U', 'R', 'D', 'S', '*',
    };
    return chars[(int)dir];
}

struct GridSystem {
    int W;
    int H;
    int WH;  

   private:
    vector<pint> toPos_;

    vector<CCA<int, 4>> around_;  
    vector<array<int, 4>> dirs_;  
    vector<vector<int>> distMat_;  

   public:
    void Init(int w, int h, const dvec<bool>& isRightWall, const dvec<bool>& isDownWall) {
        W = w;
        H = h;
        WH = W * H;

        toPos_.resize(WH);
        around_.resize(WH);
        dirs_.resize(WH);
        REP(y, H) {
            REP(x, W) {
                int id = x + W * y;
                toPos_[id] = pint{x, y};

                REP(di, 4) {
                    const pint& a = Around4[di];
                    int x2 = x + a.a;
                    int y2 = y + a.b;
                    if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
                        bool canMove = true;
                        if (di == 0) {
                            if (isRightWall[y * W + (x - 1)]) {
                                canMove = false;
                            }
                        }
                        else if (di == 1) {
                            if (isDownWall[(y - 1) * W + x]) {
                                canMove = false;
                            }
                        }
                        else if (di == 2) {
                            if (isRightWall[y * W + x]) {
                                canMove = false;
                            }
                        }
                        else if (di == 3) {
                            if (isDownWall[y * W + x]) {
                                canMove = false;
                            }
                        }

                        if (canMove) {
                            int nc = x2 + y2 * W;
                            around_[id].push(nc);
                            dirs_[id][di] = nc;
                        }
                        else {
                            dirs_[id][di] = -1;
                        }
                    }
                    else {
                        dirs_[id][di] = -1;
                    }
                }
            }
        }

        NNQue<int> que;
        distMat_.resize(WH);
        REP(start, WH) {
            auto& dists = distMat_[start];
            dists.assign(NN, INT_MAX);
            que.clear();
            {
                que.push(start);
                dists[start] = 0;
            }
            while (!que.empty()) {
                int p = que.pop();
                for (int n : around(p)) {
                    int d = dists[p] + 1;
                    if (d < dists[n]) {
                        que.push(n);
                        dists[n] = d;
                    }
                }
            }
        }
    }

    inline int ToId(int x, int y) const {
        VASSERT(x >= 0 && x < W);
        VASSERT(y >= 0 && y < H);
        return x + W * y;
    }
    inline int ToId(const pint& p) const {
        VASSERT(p.x >= 0 && p.x < W);
        VASSERT(p.y >= 0 && p.y < H);
        return p.x + W * p.y;
    }
    inline const pint& ToPos(int id) const {
        return toPos_[id];
    }

    int GetDist(int from, int to) const {
        return distMat_[from][to];
    }

    inline int CalcL1Dist(const pint& a, const pint& b) const {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }
    inline int CalcL1Dist(int a, int b) const {
        return CalcL1Dist(ToPos(a), ToPos(b));
    }

    inline int CalcL2Dist2(const pint& a, const pint& b) const {
        return square(a.x - b.x) + square(a.y - b.y);
    }
    inline int CalcL2Dist2(int a, int b) const {
        return CalcL2Dist2(ToPos(a), ToPos(b));
    }

    inline double CalcL2Dist(const pint& a, const pint& b) const {
        return sqrt(CalcL2Dist2(a, b));
    }
    inline double CalcL2Dist(int a, int b) const {
        return CalcL2Dist(ToPos(a), ToPos(b));
    }

    inline bool IsOut(int x, int y) const {
        if (x < 0 || x >= W || y < 0 || y >= H) {
            return true;
        }
        return false;
    }
    inline bool IsOut(const pint& p) const {
        return IsOut(p.x, p.y);
    }
    inline bool IsIn(int x, int y) const {
        return !IsOut(x, y);
    }
    inline bool IsIn(const pint& p) const {
        return !IsOut(p.x, p.y);
    }

    inline bool IsBorder(int x, int y) const {
        if (IsOut(x, y)) {
            return false;
        }
        if (x == 0 || x == W - 1 || y == 0 || y == H - 1) {
            return true;
        }
        return false;
    }
    inline bool IsBorder(const pint& p) const {
        return IsBorder(p.x, p.y);
    }
    inline bool IsBorder(int cell) const {
        return IsBorder(ToPos(cell));
    }

    inline int RotateRight90(int id) const {
        pint p = ToPos(id);
        return ToId(W - 1 - p.y, p.x);
    }

    inline Dir CalcDir(int from, int to) const {
        if (from - 1 == to) {
            return Dir::L;
        }
        else if (from - W == to) {
            return Dir::U;
        }
        else if (from + 1 == to) {
            return Dir::R;
        }
        else if (from + W == to) {
            return Dir::D;
        }
        else if (from == to) {
            return Dir::N;
        }
        else {
            VABORT();
        }
        return Dir::Invalid;
    }

    inline Dir CalcDirM(int from, int to) const {
        return ::CalcDirM(ToPos(from), ToPos(to));
    }

    inline const CCA<int, 4>& around(int id) const {
        return around_[id];
    }
    inline const array<int, 4>& dirs(int id) const {
        return dirs_[id];
    }
};
GridSystem gs;




template <class T>
bool SetMax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}
template <class T>
bool SetMin(T& a, const T& b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}




struct Rule {
    int color_;
    int state_;
    int nextColor_;
    int nextState_;
    Dir moveDir_;

    bool operator==(const Rule& other) const {
        return color_ == other.color_ && state_ == other.state_ && nextColor_ == other.nextColor_ &&
               nextState_ == other.nextState_ && moveDir_ == other.moveDir_;
    }

    void Set(int color, int state, int nextColor, int nextState, Dir moveDir) {
        color_ = color;
        state_ = state;
        nextColor_ = nextColor;
        nextState_ = nextState;
        moveDir_ = moveDir;
    }
};

struct IOResult {
    int C_;
    int Q_;
    dvec<int> initialColors_;
    dvec<Rule> rules_;

    void Init() {
        C_ = -1;
        Q_ = -1;
        initialColors_.assign(NN, 0);
        rules_.clear();
    }

    void Output(ostream& os) const {
        os << C_ << " " << Q_ << " " << rules_.size() << endl;
        REP(y, N) {
            REP(x, N) {
                os << max(0, initialColors_[x + y * N]);
                if (x + 1 < N) {
                    os << " ";
                }
            }
            os << endl;
        }
        REP(ri, rules_.size()) {
            const Rule& r = rules_[ri];
            os << r.color_ << " " << r.state_ << " " << max(0, r.nextColor_) << " " << r.nextState_ << " "
               << DirToChar(r.moveDir_) << endl;
        }
    }
};

struct IOServer {

    dvec<bool> isRightWall_;
    dvec<bool> isDownWall_;
    dvec<int> targets_;

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        is >> N;
        timer.Init();  
        is >> K >> T;

        NN = N * N;

        isRightWall_.assign(NN, false);
        isDownWall_.assign(NN, false);
        char c;
        REP(y, N) {
            REP(x, N - 1) {
                is >> c;
                if (c == '1') {
                    isRightWall_[y * N + x] = true;
                }
            }
        }
        REP(y, N - 1) {
            REP(x, N) {
                is >> c;
                if (c == '1') {
                    isDownWall_[y * N + x] = true;
                }
            }
        }

        targets_.resize(K);
        int y, x;
        REP(i, K) {
            is >> y >> x;
            targets_[i] = x + y * N;
        }

        gs.Init(N, N, isRightWall_, isDownWall_);
    }

    void Input() {
        istream& is = cin;
    }

    void Output(IOResult& result) {
        ostream& os = cout;
        result.Output(os);
    }

    void Finalize() {
    }
};
IOServer server;


Xor64 rand_;


void Validation(const IOResult& result, const dvec<int>* allPath = nullptr) {
    auto colors = result.initialColors_;
    REP(i, NN) {
        chmax(colors[i], 0);
    }
    int lastPos = -1;
    int pos = server.targets_[0];
    if (allPath) {
        VASSERT(pos == (*allPath)[0]);
    }
    int pi = 0;

    int state = 0;
    int targetTi = 1;
    while (targetTi < server.targets_.size() && pos == server.targets_[targetTi]) {
        ++targetTi;
    }
    int maxState = 0;
    int maxColor = 0;
    REP(turn, T) {
        if (targetTi >= server.targets_.size()) {
            break;
        }
        lastPos = pos;
        bool hit = false;
        bool wait = false;
        int color = colors[pos];
        REP(ri, result.rules_.size()) {
            auto& rule = result.rules_[ri];
            if (rule.color_ == color && rule.state_ == state) {
                chmax(maxColor, rule.color_);
                chmax(maxState, rule.nextState_);


                colors[pos] = max(0, rule.nextColor_);
                int nextState = rule.nextState_;
                cauto dir = rule.moveDir_;
                if (dir == Dir::N) {
                    wait = true;
                }
                else {
                    int nextPos = gs.dirs(pos)[(int)dir];
                    if (nextPos >= 0) {
                        pos = nextPos;
                    }
                }
                state = nextState;
                hit = true;
                break;
            }
        }
        VASSERT(hit);
        while (targetTi < server.targets_.size() && pos == server.targets_[targetTi]) {
            ++targetTi;
        }

        if (!wait) {
            ++pi;
            if (allPath) {
                VASSERT(pos == (*allPath)[pi]);
            }
        }
    }

    VASSERT(targetTi == server.targets_.size());

    VASSERT(maxState + 1 == result.Q_);
    VASSERT(maxColor + 1 == result.C_);
}



struct RuleMatrix {
    constexpr static int MatSize = 64;

    dvec<Rule> rules_;
    int C_;
    int Q_;
    double divCQ_;     
    double divNormC_;  
    double divNormQ_;  

    array<int, MatSize> colorsUsedCount_;
    array<int, MatSize> statesUsedCount_;

   private:
    array<array<int, MatSize>, MatSize> mat_;  

   public:
    void Init() {
        for (auto& v : mat_) {
            v.fill(-1);
        }
        rules_.clear();
        C_ = 0;
        Q_ = 0;

        rules_.reserve(2048);
        colorsUsedCount_.fill(0);
        statesUsedCount_.fill(0);
    }

    void AddRule(int color, int state, int nextColor, int nextState, Dir moveDir, int limitC, int limitQ) {
        VASSERT(color < MatSize);
        VASSERT(state < MatSize);
        VASSERT(color >= 0);
        VASSERT(state >= 0);
        VASSERT(nextState >= 0);

        VASSERT(mat_[color][state] == -1);  

        mat_[color][state] = rules_.size();
        rules_.pushR().Set(color, state, nextColor, nextState, moveDir);

        ++colorsUsedCount_[color];
        ++statesUsedCount_[state];

        bool updated = false;
        if (color + 1 > C_) {
            C_ = color + 1;
            updated = true;
        }
        if (state + 1 > Q_) {
            Q_ = state + 1;
            updated = true;
        }
        if (updated) {
            divCQ_ = 1.0 / double(C_ * Q_);
            int mC = max(C_, limitC);
            int mQ = max(Q_, limitQ);
            divNormQ_ = 1.0 / (double)((mQ * (mQ + 1) * 0.5) * mC);
            divNormC_ = 1.0 / (double)((mC * (mC + 1) * 0.5) * mQ);
        }
    }

    void PopRule() {
        VASSERT(!rules_.empty());
        auto& r = rules_.back();

        --colorsUsedCount_[r.color_];
        --statesUsedCount_[r.state_];

        mat_[r.color_][r.state_] = -1;
        rules_.pop();
    }
    void SetCQ(int C, int Q, int limitC, int limitQ) {
        bool updated = false;
        if (C != C_) {
            C_ = C;
            updated = true;
        }
        if (Q != Q_) {
            Q_ = Q;
            updated = true;
        }
        if (updated) {
            divCQ_ = 1.0 / double(C_ * Q_);
            int mC = max(C_, limitC);
            int mQ = max(Q_, limitQ);
            divNormQ_ = 1.0 / (double)((mQ * (mQ + 1) * 0.5) * mC);
            divNormC_ = 1.0 / (double)((mC * (mC + 1) * 0.5) * mQ);
        }
    }

    bool HasRule(int color, int state) const {
        VASSERT(color < MatSize);
        VASSERT(state < MatSize);
        return mat_[color][state] >= 0;
    }

    int GetRuleIdx(int color, int state) const {
        VASSERT(color < MatSize);
        VASSERT(state < MatSize);
        int idx = mat_[color][state];
        return idx;
    }

    Rule& GetRule(int color, int state) {
        VASSERT(color < MatSize);
        VASSERT(state < MatSize);
        int idx = mat_[color][state];
        return rules_[idx];
    }
    const Rule& GetRule(int color, int state) const {
        VASSERT(color < MatSize);
        VASSERT(state < MatSize);
        int idx = mat_[color][state];
        return rules_[idx];
    }

    void SwapStartState(int startState) {
        if (startState != 0) {
            for (auto& r : rules_) {
                if (r.state_ == 0) {
                    r.state_ = startState;
                }
                else if (r.state_ == startState) {
                    r.state_ = 0;
                }

                if (r.nextState_ == 0) {
                    r.nextState_ = startState;
                }
                else if (r.nextState_ == startState) {
                    r.nextState_ = 0;
                }
            }

        }
    }

    int GetScore() const {
        return C_ + Q_;
    }
};

void CalcTargetsPathAll2(dvec<int>& path) {
    constexpr double INF = 1e300;
    constexpr double EPS = 1e-12;  

    auto feq = [&](double a, double b) {
        double s = std::max({1.0, std::fabs(a), std::fabs(b)});
        return std::fabs(a - b) <= EPS * s;
    };
    auto betterThan = [&](double a_val, int a_steps, double b_val, int b_steps) {
        if (!feq(a_val, b_val)) return a_val < b_val;  
        return a_steps < b_steps;                      
    };

    struct Cost {
        double val;  
        int steps;   
        bool operator<(const Cost& o) const {
            if (!std::isnan(val) && !std::isnan(o.val)) {
                if (std::fabs(val - o.val) > EPS) return val > o.val;
                return steps > o.steps;
            }
            return std::isnan(val) && !std::isnan(o.val);
        }
    };

    cauto& waypoints = server.targets_;
    int M = waypoints.size();

    auto idx = [&](int cell, int dir, int i) { return cell + NN * dir + NN * 4 * i; };
    auto idxb = [&](int id, int& cell, int& dir, int& i) {
        i = id / (NN * 4);
        id -= i * (NN * 4);
        dir = id / NN;
        cell = id % NN;
    };
    const int NSTATE = NN * 4 * (M + 1);

    vector<Cost> dist(NSTATE, Cost{INF, INT_MAX / 4});
    vector<int> par(NSTATE);

    std::priority_queue<std::pair<Cost, int>> pq;
    for (int dir = 0; dir < 4; ++dir) {
        int id = idx(waypoints[0], dir, 1);
        dist[id] = Cost{0.0, 0};
        par[id] = -1;
        pq.push({dist[id], id});
    }

    auto is_waypoint = [&](int i, int cell) { return (i < M && waypoints[i] == cell); };

    int goal_id = -1;

    while (!pq.empty()) {
        auto [c, u] = pq.top();
        pq.pop();
        if (!(feq(c.val, dist[u].val) && c.steps == dist[u].steps)) continue;

        int cell, dir, i;
        idxb(u, cell, dir, i);

        int ni = i;
        if (is_waypoint(i, cell)) {
            ni = i + 1;
            if (ni == M) {
                goal_id = u;
                break;
            }  
        }

        for (int ndir = 0; ndir < 4; ++ndir) {
            int ncell = gs.dirs(cell)[ndir];
            if (ncell < 0) continue;

            double add_cost = 0;
            if (par[u] != -1) {
                if (IsHorizontal(Dir(dir)) != IsHorizontal(Dir(ndir))) {
                    if (RotateRight(Dir(dir)) == Dir(ndir)) {
                        add_cost += 1.0;
                    }
                    else {
                        add_cost += 1.0;
                    }
                }
                else if (dir != ndir) {
                    add_cost += 1.5;
                }
            }
            add_cost += 1;


            Cost nc{c.val + add_cost, c.steps + 1};
            int v = idx(ncell, ndir, ni);
            if (betterThan(nc.val, nc.steps, dist[v].val, dist[v].steps)) {
                dist[v] = nc;
                par[v] = idx(cell, dir, i);
                pq.push({nc, v});
            }
        }
    }

    VASSERT(goal_id != -1);

    auto unpack = [&](int id) {
        int cell, dir, i;
        idxb(id, cell, dir, i);
        return std::tuple<int, int, int>(cell, dir, i);
    };
    path.clear();
    for (int u = goal_id; u != -1;) {
        auto [cell, dir, i] = unpack(u);
        path.push(cell);
        auto p = par[u];
        if (p == -1) break;
        u = p;
    }
    std::reverse(path.begin(), path.end());
}

void CalcTargetsPathAll2Fast(dvec<int>& path) {
    struct Cost {
        int cost;
        int steps;
        bool operator<(const Cost& o) const {
            if (cost != o.cost) return cost < o.cost;
            return steps < o.steps;
        }
    };

    cauto& waypoints = server.targets_;
    int M = waypoints.size();

    auto idx = [&](int cell, int dir, int i) { return cell + NN * dir + NN * 4 * i; };
    auto idxb = [&](int id, int& cell, int& dir, int& i) {
        i = id / (NN * 4);
        id -= i * (NN * 4);
        dir = id / NN;
        cell = id % NN;
    };
    auto unpack = [&](int id) {
        int cell, dir, i;
        idxb(id, cell, dir, i);
        return std::tuple<int, int, int>(cell, dir, i);
    };
    const int NSTATE = NN * 4 * (M + 1);

    auto is_waypoint = [&](int i, int cell) { return (i < M && waypoints[i] == cell); };

    constexpr int SCALE = 2;
    constexpr int STEP_COST = 1 * SCALE;
    constexpr int CURVE_COST = 1 * SCALE;
    constexpr int BACK_COST = 3;  

    constexpr int Q_COUNT_BIT = 4;
    constexpr int QueCount = (1 << Q_COUNT_BIT);
    constexpr int QueMask = QueCount - 1;

    static dvec<Cost> dist;
    static dvec<int> par;
    dist.assign(NSTATE, Cost{INT_MAX / 4, INT_MAX / 4});
    par.assign(NSTATE, -1);

    static array<CapacityRingDeque<pr<Cost, int>, NN_max * 4>, QueCount> ques;
    for (auto& q : ques) {
        q.clear();
    }

    auto pushInit = [&](int dir) {
        int id = idx(waypoints[0], dir, 1);
        dist[id] = Cost{0, 0};
        par[id] = -1;
        ques[0].push_back(pr<Cost, int>{dist[id], id});
    };

    for (int dir = 0; dir < 4; ++dir) {
        pushInit(dir);
    }

    int goal_id = -1;
    int emptyQueCount = 0;
    int curCost = 0;

    while (emptyQueCount < QueCount) {
        auto& que = ques[curCost & QueMask];
        if (que.empty()) {
            ++emptyQueCount;
        }
        else {
            emptyQueCount = 0;
            while (!que.empty()) {
                auto [c, u] = que.front();
                que.pop_front();

                if (!(c.cost == dist[u].cost && c.steps == dist[u].steps)) continue;

                int cell, dir, i;
                idxb(u, cell, dir, i);

                int ni = i;
                if (is_waypoint(i, cell)) {
                    ni = i + 1;
                    if (ni == M) {
                        goal_id = u;
                        break;
                    }
                }

                cauto& dirs = gs.dirs(cell);
                for (int ndir = 0; ndir < 4; ++ndir) {
                    int ncell = dirs[ndir];
                    if (ncell < 0) continue;

                    int addCost = STEP_COST;
                    if (par[u] != -1 && dir != ndir) {
                        if (Back(Dir(dir)) == Dir(ndir)) {
                            addCost += BACK_COST;
                        }
                        else {
                            addCost += CURVE_COST;
                        }
                    }

                    VASSERT(addCost < QueCount);  
                    chmin(addCost, QueCount - 1);

                    Cost nc{c.cost + addCost, c.steps + 1};
                    int v = idx(ncell, ndir, ni);
                    if (nc < dist[v]) {
                        auto& que = ques[nc.cost & QueMask];
                        if (que.size() < que.capacity()) {
                            dist[v] = nc;
                            par[v] = u;
                            que.push_back(pr<Cost, int>{nc, v});
                        }
                    }
                }
            }

            if (goal_id != -1) break;
            VASSERT(que.empty());
        }
        ++curCost;
    }

    VASSERT(goal_id != -1);

    path.clear();
    for (int u = goal_id; u != -1;) {
        auto [cell, dir, i] = unpack(u);
        path.push(cell);
        int p = par[u];
        if (p == -1) break;
        u = p;
    }
    std::reverse(path.begin(), path.end());
}

double normal(double mean, double stddev) {
    double u1 = rand_.GetDouble();
    if (u1 <= 1e-12) u1 = 1e-12;
    double u2 = rand_.GetDouble();
    double r = std::sqrt(-2.0 * std::log(u1));
    double th = 2.0 * M_PI * u2;
    return mean + stddev * r * std::cos(th);
}

#define USE_LIMIT_LENGTH 1



struct RemakeParam {
    double curveCost_;
    double backCost_;
    double pastCurveCost_;
    double futureLineCost_;
};

inline RemakeParam MakeRemakeParamAroundBase(const RemakeParam& base, const RemakeParam& sigma,
                                             double runScale  
) {
    RemakeParam p = base;

    auto mul_jitter = [&](double value, double s) {
        double z = normal(0.0, s * runScale);  
        double factor = std::exp(z);           
        return value * factor;
    };

    p.curveCost_ = mul_jitter(base.curveCost_, sigma.curveCost_);
    p.backCost_ = mul_jitter(base.backCost_, sigma.backCost_);
    p.pastCurveCost_ = mul_jitter(base.pastCurveCost_, sigma.pastCurveCost_);
    p.futureLineCost_ = mul_jitter(base.futureLineCost_, sigma.futureLineCost_);

    chmax(p.curveCost_, 0.0);
    chmax(p.backCost_, 0.0);
    chmax(p.pastCurveCost_, 0.0);
    chmax(p.futureLineCost_, 0.0);

    return p;
}

bool FindPath2(const dvec<bool>& pastCurved, const dvec<int>& futureLineCount, const dvec<int>& waypoints,
               int beforeCell, int limitStep, dvec<int>& path, const RemakeParam& param) {
    struct Cost {
        int cost;
        int steps;  
        bool operator==(const Cost& o) const {
            return cost == o.cost && steps == o.steps;
        }
        bool operator<(const Cost& o) const {
            if (cost != o.cost) return cost < o.cost;
            return steps < o.steps;
        }
    };

    int M = waypoints.size();

    auto idx = [&](int cell, int dir, int i) { return cell + NN * dir + NN * 4 * i; };
    auto idxb = [&](int id, int& cell, int& dir, int& i) {
        int D0 = NN * 4;
        int q0 = id / D0;
        int r0 = id % D0;  

        i = q0;
        dir = r0 / NN;
        cell = r0 % NN;  
    };
    auto GetCell = [&](int id) { return id % NN; };
    const int NSTATE = NN * 4 * (M + 1);

    auto is_waypoint = [&](int i, int cell) { return (i < M && waypoints[i] == cell); };

    constexpr int Q_COUNT_BIT = 10;
    constexpr int QueCount = (1 << Q_COUNT_BIT);
    constexpr int QueMask = QueCount - 1;

    static dvec<Cost> dist;
    static dvec<int> par;
    dist.assign(NSTATE, Cost{INT_MAX, INT_MAX});
    par.resize(NSTATE);

    static array<CapacityRingDeque<pr<Cost, int>, NN_max * 4>, QueCount> ques;  
    for (auto& q : ques) {
        q.clear();
    }

    if (beforeCell < 0) {
        for (int dir = 0; dir < 4; ++dir) {
            int id = idx(waypoints[0], dir, 1);
            dist[id] = Cost{0, 0};
            par[id] = -1;
            ques[0].push_back(pr<Cost, int>{dist[id], id});
        }
    }
    else {
        Dir inDir = gs.CalcDir(beforeCell, waypoints[0]);
        int dir = static_cast<int>(inDir);
        int id = idx(waypoints[0], dir, 1);
        dist[id] = Cost{0, 0};
        par[id] = -1;
        ques[0].push_back(pr<Cost, int>{dist[id], id});
    }

    int goal_id = -1;
    int emptyQueCount = 0;
    int curCost = 0;

    constexpr int Scale = 5;                                                  
    const int backCostInt = int(param.curveCost_ * param.backCost_ * Scale);  
    const int curveCostInt = int(param.curveCost_ * Scale);
    const int pastCurveCostInt = int(param.pastCurveCost_ * Scale);

    while (emptyQueCount < QueCount) {
        auto& que = ques[curCost & QueMask];
        if (que.empty()) {
            ++emptyQueCount;
        }
        else {
            emptyQueCount = 0;
            while (!que.empty()) {
                auto [c, u] = que.front();
                que.pop_front();

                if (!(c.cost == dist[u].cost && c.steps == dist[u].steps)) {
                    continue;
                }

                int cell, dir, i;
                idxb(u, cell, dir, i);

                int ni = i;
                if (is_waypoint(i, cell)) {
                    ni = i + 1;
                    if (ni == M) {
                        goal_id = u;
                        break;
                    }  
                }

                cauto& dirs = gs.dirs(cell);
                REP(ndir, 4) {
                    int ncell = dirs[ndir];
                    if (ncell < 0) {
                        continue;
                    }

                    if (ni == 1) {
                        int stepLeft = gs.GetDist(ncell, waypoints[1]);
                        int minStep = c.steps + 1 + stepLeft;
                        if (minStep > limitStep) {
                            continue;
                        }
                    }

                    bool isCurve = false;
                    if (dir != ndir) {
                        isCurve = true;
                    }

                    int addCost = 0;

                    if (isCurve) {
                        if (Back(Dir(dir)) == Dir(ndir)) {
                            addCost += backCostInt;
                        }
                        else {
                            addCost += curveCostInt;
                        }

                        if (!pastCurved[cell]) {
                            addCost += int(futureLineCount[cell] * param.futureLineCost_ * Scale);
                        }

                    }
                    else {
                        if (pastCurved[cell]) {
                            addCost += pastCurveCostInt;
                        }
                    }

                    chmin(addCost, QueCount - 1);
                    VASSERT(addCost >= 0);  

                    Cost nc{c.cost + addCost, c.steps + 1};
                    int v = idx(ncell, ndir, ni);
                    if (nc < dist[v]) {
                        auto& que = ques[nc.cost & QueMask];
                        {
                            if (que.size() < que.capacity()) {
                                dist[v] = nc;
                                par[v] = idx(cell, dir, i);
                                que.push_back(pr<Cost, int>{nc, v});
                            }
                        }
                    }
                }
            }

            if (goal_id != -1) {
                break;
            }
            VASSERT(que.empty());
        }
        ++curCost;
    }

    if (goal_id < 0) {
        return false;
    }

    VASSERT(goal_id != -1);

    path.clear();
    for (int u = goal_id; u != -1;) {
        int cell = GetCell(u);
        path.push(cell);
        int p = par[u];
        if (p == -1) break;
        u = p;
    }
    reverse(path.begin(), path.end());
    return true;
}

void FindShortRandPath(int startCell, int endCell, dvec<int>& path) {
    NNQue<int> que;
    NNArr<int> dists;
    NNArr<int> froms;
    dists.assign(NN, INT_MAX);
    froms.assign(NN, -1);
    {
        int start = startCell;
        que.push(start);
        dists[start] = 0;
        froms[start] = -1;
    }
    CCA<int, 4> arounds;
    while (!que.empty()) {
        int p = que.pop();

        arounds = gs.around(p);
        Shuffle(ALL(arounds), rand_);
        for (int n : arounds) {
            int d = dists[p] + 1;
            if (d < dists[n]) {
                que.push(n);
                dists[n] = d;
                froms[n] = p;
            }
        }
    }

    path.clear();
    int p = endCell;
    while (p != -1) {
        path.push(p);
        p = froms[p];
    }
    reverse(ALL(path));
}

bool IsCurve(const dvec<int>& path, int pi) {
    VASSERT(pi > 0 && pi + 1 < path.size());
    int prevCell = path[pi - 1];
    int cell = path[pi];
    int nextCell = path[pi + 1];
    Dir inDir = gs.CalcDir(prevCell, cell);
    Dir outDir = gs.CalcDir(cell, nextCell);
    return inDir != outDir;
}

struct PathRemaker {
    dvec<int> allPath_;
    dvec<int> targetPis_;
    dvec<dvec<bool>> cellsCurveRecord_;  

    void Init(const dvec<int>& allPath) {

        allPath_ = allPath;
        targetPis_.clear();
        cellsCurveRecord_.resize(NN);
        for (auto& v : cellsCurveRecord_) {
            v.clear();
        }

        targetPis_.push(0);  

        int ti = 1;
        REP(pi, allPath.size()) {
            int cell = allPath[pi];
            if (cell == server.targets_[ti]) {
                targetPis_.push(pi);
                ti++;
            }

            if (pi > 0 && pi + 1 < allPath.size()) {
                cellsCurveRecord_[cell].push(IsCurve(allPath, pi));
            }
        }
    }

    void Remake(const RemakeParam& param, int iter) {
        auto& allPath = allPath_;

        static dvec<bool> pastCurved2;
        static dvec<int> futureLineCount2;
        static dvec<dvec<bool>> newCellsCurveRecord;
        static dvec<int> newTargetPis;

        static dvec<int> newPath;
        static dvec<int> path;
        static dvec<int> waypoints;

        REP(i, iter) {
            newPath.clear();

            int endPi = allPath.size() - 1;
            newPath.push(allPath[endPi]);

            int pathLength = allPath.size();

            pastCurved2.assign(NN, false);

            newCellsCurveRecord.resize(NN);
            for (auto& v : newCellsCurveRecord) {
                v.clear();
            }
            newTargetPis.clear();
            newTargetPis.push(0);

            futureLineCount2.assign(NN, 0);
            REP(i, NN) {
                cauto& record = cellsCurveRecord_[i];
                RREP(ri, record.size()) {
                    if (record[ri]) {
                        break;
                    }
                    else {
                        futureLineCount2[i]++;
                    }
                }
            }

            RREP(ti, server.targets_.size() - 1) {
                int target = server.targets_[ti];
                int startPi = targetPis_[ti];

                int startPi2 = -1;
                if (ti - 1 >= 0) {
                    startPi2 = targetPis_[ti - 1];
                }

                FOR(pi, max(startPi, 1), endPi) {
                    int cell = allPath[pi];
                    bool isCurve = cellsCurveRecord_[cell].back();

                    cellsCurveRecord_[cell].pop();

                    if (isCurve) {
                        cauto& record = cellsCurveRecord_[cell];
                        int count = 0;
                        RREP(ri, record.size()) {
                            if (record[ri]) {
                                break;
                            }
                            else {
                                ++count;
                            }
                        }
                        futureLineCount2[cell] = count;
                    }
                    else {
                        --futureLineCount2[cell];
                    }
                }

                int lastNewPathSize = newPath.size();

                waypoints.clear();
                waypoints.push(allPath[endPi]);
                waypoints.push(allPath[startPi]);
                if (startPi2 >= 0) {
                    waypoints.push(allPath[startPi2]);
                }
                int beforeCell = (endPi + 1 < allPath.size()) ? allPath[endPi + 1] : -1;

                int limitStep = T + 1 - (pathLength - (endPi - startPi));
                if (!FindPath2(pastCurved2, futureLineCount2, waypoints, beforeCell, limitStep, path, param)) {
                    RFOR(pi, startPi, endPi) {
                        newPath.push(allPath[pi]);
                    }
                }
                else {
                    if (startPi2 >= 0) {
                        REP(pi, path.size()) {
                            if (path[pi] == allPath[startPi]) {
                                path.resize(pi + 1);
                                break;
                            }
                        }
                    }

                    int newLength = pathLength - (endPi - startPi) + (path.size() - 1);
                    if (newLength - 1 > T) {
                        VABORT();
                        RFOR(pi, startPi, endPi) {
                            newPath.push(allPath[pi]);
                        }
                    }
                    else {
                        FOR(pi, 1, path.size()) {
                            newPath.push(path[pi]);
                        }
                        pathLength = newLength;
                    }
                }

                newTargetPis.push(newPath.size() - 1);

                FOR(pi, lastNewPathSize - 1, newPath.size() - 1) {
                    if (pi > 0) {
                        bool isCurve = IsCurve(newPath, pi);
                        int cell = newPath[pi];
                        if (isCurve) {
                            pastCurved2[cell] = true;
                        }

                        newCellsCurveRecord[cell].push(isCurve);
                    }
                }

                endPi = startPi;
            }

            VASSERT(newPath.size() == pathLength);
            VASSERT(newPath.size() - 1 <= T);

            reverse(ALL(newPath));
            allPath = newPath;

            VASSERT(newTargetPis.size() == server.targets_.size());
            REP(i, K) {
                targetPis_[i] = newPath.size() - 1 - newTargetPis[K - 1 - i];
            }

            REP(i, NN) {
                reverse(ALL(newCellsCurveRecord[i]));
            }
            swap(cellsCurveRecord_, newCellsCurveRecord);
        }
    }

    void Shuffle() {
        auto& allPath = allPath_;

        static dvec<dvec<bool>> newCellsCurveRecord;
        static dvec<int> newTargetPis;

        static dvec<int> newPath;
        static dvec<int> path;

        newPath.clear();

        int endPi = allPath.size() - 1;
        newPath.push(allPath[endPi]);

        newCellsCurveRecord.resize(NN);
        for (auto& v : newCellsCurveRecord) {
            v.clear();
        }
        newTargetPis.clear();
        newTargetPis.push(0);

        RREP(ti, server.targets_.size() - 1) {
            int target = server.targets_[ti];
            int startPi = targetPis_[ti];

            int lastNewPathSize = newPath.size();

            FindShortRandPath(allPath[endPi], allPath[startPi], path);
            FOR(pi, 1, path.size()) {
                newPath.push(path[pi]);
            }

            newTargetPis.push(newPath.size() - 1);

            FOR(pi, lastNewPathSize - 1, newPath.size() - 1) {
                if (pi > 0) {
                    bool isCurve = IsCurve(newPath, pi);
                    int cell = newPath[pi];
                    newCellsCurveRecord[cell].push(isCurve);
                }
            }

            endPi = startPi;
        }

        VASSERT(newPath.size() <= allPath.size());

        reverse(ALL(newPath));
        allPath = newPath;

        VASSERT(newTargetPis.size() == server.targets_.size());
        REP(i, K) {
            targetPis_[i] = newPath.size() - 1 - newTargetPis[K - 1 - i];
        }

        REP(i, NN) {
            reverse(ALL(newCellsCurveRecord[i]));
        }
        swap(cellsCurveRecord_, newCellsCurveRecord);
    }
};

template <class T>
struct JagArray3 {
    dvec<dvec<dvec<T>>> data_;

    void Init(int reserveA, int reserveB, int reserveC) {
        data_.resize(reserveA);
        for (auto& v1 : data_) {
            v1.resize(reserveB);
            for (auto& v2 : v1) {
                v2.resize(reserveC);
            }
        }
    }

    int SizeA() const {
        return data_.size();
    }
    int SizeB(int a) const {
        VASSERT(a >= 0);
        VASSERT(a < data_.size());
        return data_[a].size();
    }
    int SizeC(int a, int b) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(a < data_.size());
        VASSERT(b < data_[a].size());
        return data_[a][b].size();
    }

    bool Has(int a, int b, int c) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        if (a >= data_.size()) {
            return false;
        }
        if (b >= data_[a].size()) {
            return false;
        }
        if (c >= data_[a][b].size()) {
            return false;
        }
        return true;
    }

    T& SetAt(int a, int b, int c) {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        if (a >= data_.size()) {
            data_.resize(a + 1);
        }
        if (b >= data_[a].size()) {
            data_[a].resize(b + 1);
        }
        if (c >= data_[a][b].size()) {
            data_[a][b].resize(c + 1);
        }
        return data_[a][b][c];
    }

    const T& At(int a, int b, int c) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        VASSERT(Has(a, b, c));
        return data_[a][b][c];
    }
};

template <class T>
struct JagArray4 {
    dvec<dvec<dvec<dvec<T>>>> data_;

    void Init() {
        for (auto& v1 : data_) {
            for (auto& v2 : v1) {
                for (auto& v3 : v2) {
                    v3.clear();
                }
                v2.clear();
            }
            v1.clear();
        }
        data_.clear();
    }

    int SizeA() const {
        return data_.size();
    }
    int SizeB(int a) const {
        VASSERT(a >= 0);
        VASSERT(a < data_.size());
        return data_[a].size();
    }
    int SizeC(int a, int b) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(a < data_.size());
        VASSERT(b < data_[a].size());
        return data_[a][b].size();
    }
    int SizeD(int a, int b, int c) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        VASSERT(a < data_.size());
        VASSERT(b < data_[a].size());
        VASSERT(c < data_[a][b].size());
        return data_[a][b][c].size();
    }

    bool Has(int a, int b, int c, int d) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        VASSERT(d >= 0);
        if (a >= data_.size()) {
            return false;
        }
        if (b >= data_[a].size()) {
            return false;
        }
        if (c >= data_[a][b].size()) {
            return false;
        }
        if (d >= data_[a][b][c].size()) {
            return false;
        }
        return true;
    }

    T& SetAt(int a, int b, int c, int d) {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        VASSERT(d >= 0);
        if (a >= data_.size()) {
            data_.resize(a + 1);
        }
        if (b >= data_[a].size()) {
            data_[a].resize(b + 1);
        }
        if (c >= data_[a][b].size()) {
            data_[a][b].resize(c + 1);
        }
        if (d >= data_[a][b][c].size()) {
            data_[a][b][c].resize(d + 1);
        }
        return data_[a][b][c][d];
    }
    const T& At(int a, int b, int c, int d) const {
        VASSERT(a >= 0);
        VASSERT(b >= 0);
        VASSERT(c >= 0);
        VASSERT(d >= 0);
        VASSERT(Has(a, b, c, d));
        return data_[a][b][c][d];
    }
};

struct Stamp {
    Dir inDir_;         
    Dir outDir_;        
    int inColor_;       
    int outColor_;      
    int inState_;       
    int outState_;      
    pint prevStamp_;    
    pint nextStamp_;    
    bool inStateFree_;  

    void Set(Dir inDir, Dir outDir) {
        inDir_ = inDir;
        outDir_ = outDir;
        inColor_ = -1;
        outColor_ = -1;
        inState_ = -1;
        outState_ = -1;
        prevStamp_ = pint{-1, -1};
        nextStamp_ = pint{-1, -1};
        inStateFree_ = false;
    }
};

void CalcStamps(const dvec<int>& allPath, dvec<dvec<Stamp>>& cellsStamps, dvec<pint>& stampOrder) {
    stampOrder.clear();
    static dvec<bool> curved;
    curved.assign(NN, false);  
    static dvec<bool> visited;
    visited.assign(NN, false);  
    pint nextStamp = pint{-1, -1};

    cellsStamps.resize(NN);
    for (auto& stamps : cellsStamps) {
        stamps.clear();
    }

    int space = 0;
    RFOR(i, 0, allPath.size() - 1) {
        int cell = allPath[i];
        int prevCell;
        Dir prevDir;
        if (i - 1 >= 0) {
            prevCell = allPath[i - 1];
            prevDir = gs.CalcDir(prevCell, cell);
        }
        else {
            prevCell = -1;
            prevDir = Dir::N;
        }
        ++space;

        int nextCell = allPath[i + 1];
        Dir nextDir = gs.CalcDir(cell, nextCell);

        if (curved[cell]) {
            auto& s = cellsStamps[cell].pushR();
            s.Set(prevDir, nextDir);
            s.nextStamp_ = nextStamp;

            if (nextStamp.a >= 0) {
                cellsStamps[nextStamp.a][nextStamp.b].prevStamp_ = pint{cell, (int)(cellsStamps[cell].size() - 1)};

                if (space == 1) {
                    cellsStamps[nextStamp.a][nextStamp.b].inStateFree_ = true;
                }
            }
            nextStamp = pint{cell, (int)(cellsStamps[cell].size() - 1)};
            stampOrder.push(nextStamp);
            space = 0;
        }
        else {
            if (prevDir != nextDir) {
                curved[cell] = true;

                auto& s = cellsStamps[cell].pushR();
                s.Set(prevDir, nextDir);
                s.nextStamp_ = nextStamp;
                if (visited[cell]) {
                    s.outColor_ = 0;
                }

                if (nextStamp.a >= 0) {
                    cellsStamps[nextStamp.a][nextStamp.b].prevStamp_ = pint{cell, (int)(cellsStamps[cell].size() - 1)};

                    if (space == 1) {
                        cellsStamps[nextStamp.a][nextStamp.b].inStateFree_ = true;
                    }
                }
                nextStamp = pint{cell, (int)(cellsStamps[cell].size() - 1)};
                stampOrder.push(nextStamp);
                space = 0;
            }
        }
        visited[cell] = true;
    }


    auto DispOrderStamps = [&]() {
        REP(i, stampOrder.size()) {
            auto [cell, stampIndex] = stampOrder[i];
            cerr << "Cell:" << cell;
            cauto& stamp = cellsStamps[cell][stampIndex];
            cerr << "  " << DirToChar(stamp.inDir_) << DirToChar(stamp.outDir_) << " " << stamp.inColor_ << ":"
                 << stamp.inState_ << " -> " << stamp.outColor_ << ":" << stamp.outState_ << endl;
        }
    };
}

struct StateDirTable {
   private:
    constexpr static int MatSize = 64;
    array<Dir, MatSize> stateToDir_;
    array<CapArr<int, MatSize>, 4> dirsStates_;

   public:
    void Init() {
        stateToDir_.fill(Dir::N);
        REP(i, 4) {
            dirsStates_[i].clear();
        }
    }

    bool CanUse(int state, Dir dir) const {
        VASSERT(state < MatSize);
        return stateToDir_[state] == dir || stateToDir_[state] == Dir::N || dir == Dir::N;
    }

    void Register(int state, Dir dir) {
        if (dir == Dir::N) {
            return;
        }
        VASSERT(state < MatSize);
        VASSERT(stateToDir_[state] == Dir::N || stateToDir_[state] == dir);

        if (stateToDir_[state] == Dir::N) {
            dirsStates_[(int)dir].push(state);
        }

        stateToDir_[state] = dir;
    }

    void RevertRegister(int state, Dir dir) {
        VASSERT(state < MatSize);

        if (dir == Dir::N) {
            Dir curDir = stateToDir_[state];
            if (curDir != Dir::N) {
                auto& vec = dirsStates_[(int)stateToDir_[state]];
                VASSERT(vec.back() == state);
                vec.pop();
            }
        }
        stateToDir_[state] = dir;
    }

    Dir GetDir(int state) const {
        VASSERT(state < MatSize);
        return stateToDir_[state];
    }


    void SwapStartState(int startState) {
        if (startState != 0) {
            swap(stateToDir_[0], stateToDir_[startState]);
        }
    }

    const CapArr<int, MatSize>& GetStatesForDir(Dir dir) const {
        VASSERT(dir != Dir::N);
        return dirsStates_[(int)dir];
    }
};

struct SolverState {
    RuleMatrix ruleMat_;
    StateDirTable stateDirTbl_;
    dvec<dvec<Stamp>> cellsStamps_;

    constexpr static int MatSize = 64;
    array<array<array<array<array<s8, 2>, MatSize>, MatSize>, 4>, 4> countMat_;
    dvec<int> freeColorRis_;  

    array<array<array<array<s8, 2>, MatSize>, 4>, 4> countMat2_;

    array<int, 4> inDirsUsedRuleCount_;

    int colorCountScore_;
    int stateCountScore_;

    void Init() {
        ruleMat_.Init();
        stateDirTbl_.Init();

        cellsStamps_.clear();

        colorCountScore_ = 0;
        stateCountScore_ = 0;

        for (auto& mat2 : countMat_) {
            for (auto& mat3 : mat2) {
                for (auto& mat4 : mat3) {
                    for (auto& mat5 : mat4) {
                        mat5.fill(0);
                    }
                }
            }
        }
        for (auto& mat2 : countMat2_) {
            for (auto& mat3 : mat2) {
                for (auto& mat4 : mat3) {
                    mat4.fill(0);
                }
            }
        }
        freeColorRis_.clear();
        freeColorRis_.reserve(NN);

        inDirsUsedRuleCount_.fill(0);
    }

    struct Backup {
        bool addRule_;

        int updatedRuleColor_;
        int updatedRuleState_;
        int countMatIndex_;  

        int C_;
        int Q_;

        int cell_;
        int stampIndex_;
        int inColor_;
        int inState_;

        Dir inStateToDir_;
    };

    void PushRule(int cell, int stampIndex, int inColor, int inState, int limitC, int limitQ) {
        Backup backup;
        PushRule(cell, stampIndex, inColor, inState, limitC, limitQ, backup);
    }

    void PushRule(int cell, int stampIndex, int inColor, int inState, int limitC, int limitQ, Backup& backup) {
        VASSERT(inColor >= 0);
        VASSERT(inState >= 0);

        auto& stamp = cellsStamps_[cell][stampIndex];
        Dir moveDir = stamp.outDir_;
        int outColor = stamp.outColor_;
        int outState = stamp.outState_;

        backup.inStateToDir_ = stateDirTbl_.GetDir(inState);
        if (stateDirTbl_.CanUse(inState, stamp.inDir_)) {
            stateDirTbl_.Register(inState, stamp.inDir_);
        }

        backup.updatedRuleColor_ = -1;
        backup.updatedRuleState_ = -1;
        backup.countMatIndex_ = -1;
        backup.C_ = ruleMat_.C_;
        backup.Q_ = ruleMat_.Q_;

        if (ruleMat_.HasRule(inColor, inState)) {
            backup.addRule_ = false;

            auto& rule = ruleMat_.GetRule(inColor, inState);

            VASSERT(rule.moveDir_ == moveDir);

            if (rule.nextColor_ < 0 && outColor >= 0) {
                {
                    int candRuleInDir = (int)stateDirTbl_.GetDir(inState);
                    VASSERT(candRuleInDir != (int)Dir::N);

                    int ri = ruleMat_.GetRuleIdx(inColor, inState);
                    int idx = freeColorRis_.find(ri);
                    VASSERT(idx >= 0);
                    freeColorRis_.removeSwap(idx);
                    backup.countMatIndex_ = idx;

                    --countMat_[(int)stamp.outDir_][candRuleInDir][outState][0][1];
                    VASSERT(countMat2_[(int)stamp.outDir_][candRuleInDir][0][1] >= 0);
                    REP(c, ruleMat_.C_) {
                        --countMat_[(int)stamp.outDir_][candRuleInDir][outState][c + 1][1];
                        VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][outState][c + 1][1] >= 0);
                        --countMat2_[(int)stamp.outDir_][candRuleInDir][c][1];
                        VASSERT(countMat2_[(int)stamp.outDir_][candRuleInDir][c][1] >= 0);
                    }
                    ++countMat_[(int)stamp.outDir_][candRuleInDir][outState][0][0];
                    ++countMat_[(int)stamp.outDir_][candRuleInDir][outState][outColor + 1][0];
                    ++countMat2_[(int)stamp.outDir_][candRuleInDir][outColor][0];
                }

                rule.nextColor_ = outColor;

                backup.updatedRuleColor_ = inColor;
                backup.updatedRuleState_ = inState;
            }
            else {
                VASSERT(outColor < 0 || rule.nextColor_ == outColor);
            }

            VASSERT(rule.nextState_ == outState);
        }
        else {
            backup.addRule_ = true;

            int newRi = ruleMat_.rules_.size();

            ruleMat_.AddRule(inColor, inState, outColor, outState, moveDir, limitC, limitQ);
            stateCountScore_ += ruleMat_.statesUsedCount_[inState];
            colorCountScore_ += ruleMat_.colorsUsedCount_[inColor];

            if (ruleMat_.C_ > backup.C_) {
                for (int ri : freeColorRis_) {
                    auto& r = ruleMat_.rules_[ri];
                    VASSERT(r.nextColor_ < 0);  
                    int ruleInDir = (int)stateDirTbl_.GetDir(r.state_);
                    VASSERT(ruleInDir != (int)Dir::N);
                    FOR(c, backup.C_, ruleMat_.C_) {
                        ++countMat_[(int)r.moveDir_][ruleInDir][r.nextState_][c + 1][1];
                        ++countMat2_[(int)r.moveDir_][ruleInDir][c][1];
                    }
                }
            }

            int candRuleInDir = (int)stateDirTbl_.GetDir(inState);
            if (candRuleInDir != (int)Dir::N) {
                if (outColor < 0) {
                    ++countMat_[(int)stamp.outDir_][candRuleInDir][outState][0][1];
                    REP(c, ruleMat_.C_) {
                        ++countMat_[(int)stamp.outDir_][candRuleInDir][outState][c + 1][1];
                        ++countMat2_[(int)stamp.outDir_][candRuleInDir][c][1];
                    }
                    freeColorRis_.push(newRi);
                }
                else {
                    ++countMat_[(int)stamp.outDir_][candRuleInDir][outState][0][0];
                    ++countMat_[(int)stamp.outDir_][candRuleInDir][outState][outColor + 1][0];
                    ++countMat2_[(int)stamp.outDir_][candRuleInDir][outColor][0];
                }

                ++inDirsUsedRuleCount_[candRuleInDir];
            }
        }

        backup.cell_ = cell;
        backup.stampIndex_ = stampIndex;
        backup.inColor_ = stamp.inColor_;
        backup.inState_ = stamp.inState_;

        stamp.inColor_ = inColor;
        stamp.inState_ = inState;

        {
            auto& stamps = cellsStamps_[cell];
            if (stampIndex + 1 < stamps.size()) {
                auto& nextStamp = stamps[stampIndex + 1];
                VASSERT(nextStamp.inColor_ == -1);
                nextStamp.outColor_ = stamp.inColor_;
            }
        }
        {
            if (stamp.prevStamp_.a >= 0) {
                auto& prevStamp = cellsStamps_[stamp.prevStamp_.a][stamp.prevStamp_.b];
                VASSERT(prevStamp.outState_ == -1);
                prevStamp.outState_ = stamp.inState_;
            }
        }
    }

    void PopRule(const Backup& backup, int limitC, int limitQ) {
        auto& stamp = cellsStamps_[backup.cell_][backup.stampIndex_];

        if (!backup.addRule_) {
            if (backup.updatedRuleColor_ >= 0) {
                {
                    int candRuleInDir = (int)stateDirTbl_.GetDir(stamp.inState_);
                    VASSERT(candRuleInDir != (int)Dir::N);
                    int ri = ruleMat_.GetRuleIdx(backup.updatedRuleColor_, backup.updatedRuleState_);
                    freeColorRis_.swapInsert(backup.countMatIndex_, ri);

                    --countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][0];
                    --countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][stamp.outColor_ + 1][0];
                    --countMat2_[(int)stamp.outDir_][candRuleInDir][stamp.outColor_][0];
                    VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][0] >= 0);
                    VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][stamp.outColor_ + 1][0] >= 0);
                    VASSERT(countMat2_[(int)stamp.outDir_][candRuleInDir][stamp.outColor_][0] >= 0);

                    ++countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][1];
                    REP(c, ruleMat_.C_) {
                        ++countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][c + 1][1];
                        ++countMat2_[(int)stamp.outDir_][candRuleInDir][c][1];
                    }
                }

                auto& rule = ruleMat_.GetRule(backup.updatedRuleColor_, backup.updatedRuleState_);
                VASSERT(rule.nextColor_ >= 0);
                rule.nextColor_ = -1;
            }
        }
        else {
            int candRuleInDir = (int)stateDirTbl_.GetDir(stamp.inState_);
            if (candRuleInDir != (int)Dir::N) {
                --inDirsUsedRuleCount_[candRuleInDir];

                if (stamp.outColor_ < 0) {
                    --countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][1];
                    VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][1] >= 0);
                    REP(c, ruleMat_.C_) {
                        --countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][c + 1][1];
                        --countMat2_[(int)stamp.outDir_][candRuleInDir][c][1];
                        VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][c + 1][1] >= 0);
                        VASSERT(countMat2_[(int)stamp.outDir_][candRuleInDir][c][1] >= 0);
                    }
                    freeColorRis_.pop();
                }
                else {
                    --countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][0];
                    --countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][stamp.outColor_ + 1][0];
                    --countMat2_[(int)stamp.outDir_][candRuleInDir][stamp.outColor_][0];
                    VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][0][0] >= 0);
                    VASSERT(countMat_[(int)stamp.outDir_][candRuleInDir][stamp.outState_][stamp.outColor_ + 1][0] >= 0);
                    VASSERT(countMat2_[(int)stamp.outDir_][candRuleInDir][stamp.outColor_][0] >= 0);
                }
            }

            if (ruleMat_.C_ > backup.C_) {
                for (int ri : freeColorRis_) {
                    auto& r = ruleMat_.rules_[ri];
                    VASSERT(r.nextColor_ < 0);  
                    int ruleInDir = (int)stateDirTbl_.GetDir(r.state_);
                    VASSERT(ruleInDir != (int)Dir::N);
                    FOR(c, backup.C_, ruleMat_.C_) {
                        --countMat_[(int)r.moveDir_][ruleInDir][r.nextState_][c + 1][1];
                        --countMat2_[(int)r.moveDir_][ruleInDir][c][1];
                        VASSERT(countMat_[(int)r.moveDir_][ruleInDir][r.nextState_][c + 1][1] >= 0);
                        VASSERT(countMat2_[(int)r.moveDir_][ruleInDir][c][1] >= 0);
                    }
                }
            }

            stateCountScore_ -= ruleMat_.statesUsedCount_[stamp.inState_];
            colorCountScore_ -= ruleMat_.colorsUsedCount_[stamp.inColor_];

            ruleMat_.PopRule();
            ruleMat_.SetCQ(backup.C_, backup.Q_, limitC, limitQ);
        }

        if (stateDirTbl_.CanUse(stamp.inState_, stamp.inDir_)) {
            stateDirTbl_.RevertRegister(stamp.inState_, backup.inStateToDir_);
        }

        stamp.inColor_ = backup.inColor_;
        stamp.inState_ = backup.inState_;

        {
            auto& stamps = cellsStamps_[backup.cell_];
            if (backup.stampIndex_ + 1 < stamps.size()) {
                auto& nextStamp = stamps[backup.stampIndex_ + 1];
                VASSERT(nextStamp.outColor_ >= 0);
                nextStamp.outColor_ = -1;
            }
        }
        {
            if (stamp.prevStamp_.a >= 0) {
                auto& prevStamp = cellsStamps_[stamp.prevStamp_.a][stamp.prevStamp_.b];
                VASSERT(prevStamp.outState_ >= 0);
                prevStamp.outState_ = -1;
            }
        }
    }

    bool CanAddRule(Dir inDir) const {
        VASSERT(inDir != Dir::N);
        int stateCount = stateDirTbl_.GetStatesForDir(inDir).size();
        int useCount = inDirsUsedRuleCount_[(int)inDir];
        return useCount < stateCount * (ruleMat_.C_ - 1);
    }

};

struct AssignParam {
    double reuseScore_;
    double updateNextColorScore_;  
    double addScore_;
    double newRuleScore_;
    double futureSideStampColorScore_;
    double nextStampColorScore_;
    double futureScoreRate_;
    double sameStateScore_;
    double sameColorScore_;
    double colorCountScore_;
    double stateCountScore_;
    double ruleCountScore_;
    double balanceStateStep_;

    double futureFreeColorCountScore_;
    double freeColorRuleScore_;

    double reuseUpdateNextColorScore_;  
    double reuseAddScore_;              
};

void PreCalcAssignParam(AssignParam& p) {
    p.reuseUpdateNextColorScore_ = p.reuseScore_ * p.updateNextColorScore_;
    p.reuseAddScore_ = p.reuseScore_ * p.addScore_;
}

inline AssignParam MakeAssignParamAroundBase(const AssignParam& base, const AssignParam& sigma,
                                             double runScale  
) {
    AssignParam p = base;

    auto mul_jitter = [&](double value, double s) {
        double z = normal(0, s * runScale);  
        double factor = std::exp(z);         
        return value * factor;
    };

    p.reuseScore_ = mul_jitter(base.reuseScore_, sigma.reuseScore_);
    p.updateNextColorScore_ = mul_jitter(base.updateNextColorScore_, sigma.updateNextColorScore_);
    p.addScore_ = mul_jitter(base.addScore_, sigma.addScore_);
    p.newRuleScore_ = mul_jitter(base.newRuleScore_, sigma.newRuleScore_);
    p.futureSideStampColorScore_ = mul_jitter(base.futureSideStampColorScore_, sigma.futureSideStampColorScore_);
    p.nextStampColorScore_ = mul_jitter(base.nextStampColorScore_, sigma.nextStampColorScore_);
    p.futureScoreRate_ = mul_jitter(base.futureScoreRate_, sigma.futureScoreRate_);
    p.sameStateScore_ = mul_jitter(base.sameStateScore_, sigma.sameStateScore_);
    p.sameColorScore_ = mul_jitter(base.sameColorScore_, sigma.sameColorScore_);
    p.colorCountScore_ = mul_jitter(base.colorCountScore_, sigma.colorCountScore_);
    p.stateCountScore_ = mul_jitter(base.stateCountScore_, sigma.stateCountScore_);
    p.ruleCountScore_ = mul_jitter(base.ruleCountScore_, sigma.ruleCountScore_);
    p.balanceStateStep_ = mul_jitter(base.balanceStateStep_, sigma.balanceStateStep_);
    p.futureFreeColorCountScore_ = mul_jitter(base.futureFreeColorCountScore_, sigma.futureFreeColorCountScore_);
    p.freeColorRuleScore_ = mul_jitter(base.freeColorRuleScore_, sigma.freeColorRuleScore_);

    chmax(p.reuseScore_, 0.0);
    chmax(p.updateNextColorScore_, 0.0);
    chmax(p.addScore_, 0.0);
    chmin(p.newRuleScore_, 0.0);
    chmax(p.futureScoreRate_, 0.0);
    chmax(p.sameStateScore_, 0.0);
    chmax(p.sameColorScore_, 0.0);
    chmax(p.colorCountScore_, 0.0);
    chmax(p.stateCountScore_, 0.0);
    chmax(p.ruleCountScore_, 0.0);
    chmax(p.balanceStateStep_, 0.0);

    PreCalcAssignParam(p);
    return p;
}

double CalcSideStampScore(const SolverState& solverState, int cell, int stampIndex, const AssignParam& param) {
    const dvec<dvec<Stamp>>& cellsStamps = solverState.cellsStamps_;
    cauto& countMat = solverState.countMat_;
    cauto& stamp = cellsStamps[cell][stampIndex];
    Dir inDir = stamp.inDir_;
    Dir outDir = stamp.outDir_;
    int nextColor = stamp.outColor_;
    int nextState = stamp.outState_;

    if (inDir == Dir::N) {
        return 0;
    }

    cauto& count = countMat[(int)outDir][(int)inDir][nextState][nextColor + 1];
    if (count[0] > 0) {
        return param.reuseScore_;
    }

    if (count[1] > 0) {
        return param.reuseUpdateNextColorScore_;
    }

    if (solverState.CanAddRule(inDir)) {
        return param.reuseAddScore_;
    }

    return param.newRuleScore_;
}

double CalcFutureStampScore(const SolverState& solverState, int cell, int stampIndex, const AssignParam& param) {
    const StateDirTable& stateDirTbl = solverState.stateDirTbl_;
    const dvec<dvec<Stamp>>& cellsStamps = solverState.cellsStamps_;

    cauto& stamp = cellsStamps[cell][stampIndex];
    Dir inDir = stamp.inDir_;
    Dir outDir = stamp.outDir_;
    int nextColor = stamp.outColor_;
    VASSERT(stamp.outState_ < 0);

    if (inDir == Dir::N) {
        return 0;
    }

    cauto& cnadNextState = stateDirTbl.GetStatesForDir(outDir);
    if (cnadNextState.empty()) {
        return 0;
    }

    const RuleMatrix& ruleMat = solverState.ruleMat_;

    cauto& countMat2 = solverState.countMat2_;
    cauto& count = countMat2[(int)outDir][(int)inDir][nextColor];

    return (count[0] + count[1] * param.futureFreeColorCountScore_) * ruleMat.divCQ_ * param.futureScoreRate_;


}

double CalcSideStampColorScore(const dvec<dvec<Stamp>>& cellsStamps, int cell, int stampIndex,
                               const AssignParam& param) {
    cauto& stamp = cellsStamps[cell][stampIndex];

    VASSERT(stamp.outColor_ >= 0);


    double score = 0;
    if (stamp.inDir_ == stamp.outDir_) {
        if (stamp.prevStamp_.a >= 0) {
            cauto& prevStamp = cellsStamps[stamp.prevStamp_.a][stamp.prevStamp_.b];
            if (prevStamp.outColor_ >= 0 && prevStamp.outColor_ == stamp.outColor_) {
                score += param.futureSideStampColorScore_;
            }
        }
        if (stamp.nextStamp_.a >= 0) {
            cauto& nextStamp = cellsStamps[stamp.nextStamp_.a][stamp.nextStamp_.b];
            if (nextStamp.outColor_ >= 0 && nextStamp.outColor_ == stamp.outColor_) {
                score += param.futureSideStampColorScore_;
            }
        }
    }

    return score;
}


bool FindRule2(SolverState& solverState, int cell, int stampIndex, int& color, int& state, int limitC, int limitQ,
               const AssignParam& param, bool useWrongDir) {
    color = -1;
    state = -1;
    double bestScore = -1e100;

    const RuleMatrix& ruleMat = solverState.ruleMat_;
    cauto& stateDirTbl = solverState.stateDirTbl_;
    const dvec<dvec<Stamp>>& cellsStamps = solverState.cellsStamps_;
    cauto& stamp = cellsStamps[cell][stampIndex];
    Dir inDir = stamp.inDir_;
    Dir outDir = stamp.outDir_;
    int nextColor = stamp.outColor_;
    int nextState = stamp.outState_;

    VASSERT(inDir != Dir::N);

    SolverState::Backup backup;

#define USE_SKIP 0

    auto Eval = [&](int c, int q, double& score) {
        score = 0;

        bool reusable = false;
        bool updateNextColor = false;
        int ri = ruleMat.GetRuleIdx(c, q);
        if (ri >= 0) {
            auto& r = ruleMat.rules_[ri];
            if (r.moveDir_ == outDir && r.nextState_ == nextState &&
                (r.nextColor_ == nextColor || r.nextColor_ < 0 || nextColor < 0)) {
                reusable = true;
                if (r.nextColor_ < 0 && nextColor >= 0) {
                    updateNextColor = true;
                }
            }
            else {
                return false;
            }
        }
        else {
        }


        if (!reusable || updateNextColor) {
            if (!stateDirTbl.CanUse(q, inDir)) {
                return false;
            }
        }

        if (reusable) {
            if (updateNextColor) {
                score += param.reuseUpdateNextColorScore_;
            }
            else {
                score += param.reuseScore_;
            }
        }
        else {
            score += param.reuseAddScore_;
        }


        if (q == nextState) {
            score += param.sameStateScore_;
        }
        if (c == nextColor) {
            score += param.sameColorScore_;
        }

        if (stamp.nextStamp_.a >= 0) {
            cauto& nextStamp = solverState.cellsStamps_[stamp.nextStamp_.a][stamp.nextStamp_.b];
            VASSERT(nextStamp.inColor_ >= 0);
            if (nextStamp.inColor_ == c) {
                score += param.nextStampColorScore_;
            }
        }

        solverState.PushRule(cell, stampIndex, c, q, limitC, limitQ, backup);

        if (stamp.prevStamp_.a >= 0) {
            score += CalcSideStampScore(solverState, stamp.prevStamp_.a, stamp.prevStamp_.b, param);
        }

        if (stampIndex + 1 < solverState.cellsStamps_[cell].size()) {
            score += CalcFutureStampScore(solverState, cell, stampIndex + 1, param);
            score += CalcSideStampColorScore(solverState.cellsStamps_, cell, stampIndex + 1, param);
        }


        score -= solverState.colorCountScore_ * ruleMat.divNormC_ * param.colorCountScore_;
        score -= solverState.stateCountScore_ * ruleMat.divNormQ_ * param.stateCountScore_;

        score -= ruleMat.rules_.size() * param.ruleCountScore_;

        score += solverState.freeColorRis_.size() * param.freeColorRuleScore_;

        solverState.PopRule(backup, limitC, limitQ);

        return true;
    };

    if (ruleMat.Q_ == 0) {
        if (stateDirTbl.CanUse(0, inDir)) {
            state = 0;
        }
        else {
            state = 1;
            VASSERT(stateDirTbl.CanUse(1, inDir));
        }
        color = 1;
        return false;
    }

    if (!useWrongDir) {
        for (auto q : stateDirTbl.GetStatesForDir(inDir)) {
            FOR(c, 1, ruleMat.C_) {
                double score = 0;
                if (Eval(c, q, score)) {
                    if (score > bestScore) {
                        bestScore = score;
                        color = c;
                        state = q;
                    }
                }
            }
        }
    }
    else {
        if (stamp.inStateFree_) {
            REP(q, ruleMat.Q_) {
                FOR(c, 1, ruleMat.C_) {
                    double score = 0;
                    if (Eval(c, q, score)) {
                        if (score > bestScore) {
                            bestScore = score;
                            color = c;
                            state = q;
                        }
                    }
                }
            }
        }
        else {
            REP(q, ruleMat.Q_) {
                if (!stateDirTbl.CanUse(q, inDir)) {
                    continue;
                }
                FOR(c, 1, ruleMat.C_) {
                    double score = 0;
                    if (Eval(c, q, score)) {
                        if (score > bestScore) {
                            bestScore = score;
                            color = c;
                            state = q;
                        }
                    }
                }
            }

        }
    }

    if (limitC >= 0 && limitQ >= 0) {
        if (ruleMat.Q_ + 1 < limitQ) {
            int newState = ruleMat.Q_;
            if (newState < limitQ) {
                FOR(c, 1, ruleMat.C_) {
                    double score = 0;
                    if (Eval(c, newState, score)) {
                        if (score > bestScore) {
                            bestScore = score;
                            color = c;
                            state = newState;
                        }
                    }
                }
            }
        }

        if (ruleMat.C_ + 1 < limitC) {
            int newColor = max(1, ruleMat.C_);

            bool added = false;
            REP(q, ruleMat.Q_) {
                if (!stateDirTbl.CanUse(q, inDir)) {
                    continue;
                }
                double score = 0;
                if (Eval(newColor, q, score)) {
                    if (score > bestScore) {
                        bestScore = score;
                        color = newColor;
                        state = q;
                    }
                }
                added = true;
            }
            if (!added) {
                int newState = ruleMat.Q_;
                if (newState < limitQ) {
                    double score = 0;
                    if (Eval(newColor, newState, score)) {
                        if (score > bestScore) {
                            bestScore = score;
                            color = newColor;
                            state = newState;
                        }
                    }
                }
            }
        }
    }

    if (color >= 0) {
        return true;
    }


    if (ruleMat.Q_ + param.balanceStateStep_ < ruleMat.C_) {
        state = ruleMat.Q_;

        color = -1;
        int q = state;
        double bestScore = -1e100;
        FOR(c, 1, ruleMat.C_) {
            double score = 0;
            if (Eval(c, q, score)) {
                if (score > bestScore) {
                    bestScore = score;
                    color = c;
                }
            }
        }
        VASSERT(color >= 0);
    }
    else {
        color = max(1, ruleMat.C_);

        state = -1;
        double bestScore = -1e100;
        int c = color;
        REP(q, ruleMat.Q_) {
            if (!stateDirTbl.CanUse(q, inDir)) {
                continue;
            }
            double score = 0;
            if (Eval(c, q, score)) {
                if (score > bestScore) {
                    bestScore = score;
                    state = q;
                }
            }
        }

        if (state < 0) {
            state = ruleMat.Q_;
        }
    }
    return false;
}

bool FindFinalRule(SolverState& solverState, int cell, int stampIndex, int& color, int& state, int limitC, int limitQ,
                   const AssignParam& param) {
    cauto& ruleMat = solverState.ruleMat_;
    cauto& stateDirTbl = solverState.stateDirTbl_;

    cauto& stamp = solverState.cellsStamps_[cell][stampIndex];
    Dir inDir = stamp.inDir_;
    Dir outDir = stamp.outDir_;
    int nextColor = stamp.outColor_;
    int nextState = stamp.outState_;


    color = -1;
    state = -1;

    REP(q, ruleMat.Q_) {
        VASSERT(stateDirTbl.CanUse(q, inDir));
        FOR(c, 1, ruleMat.C_) {
            if (ruleMat.HasRule(c, q)) {
                auto& r = ruleMat.GetRule(c, q);
                if (r.moveDir_ == outDir && r.nextState_ == nextState &&
                    (r.nextColor_ == nextColor || r.nextColor_ < 0 || nextColor < 0)) {
                    color = c;
                    state = q;
                    return true;
                }
            }
            else {
                VASSERT(q < ruleMat.Q_);
                color = c;
                state = q;
                return true;
            }
        }
    }


    int bestScore = INT_MAX;
    {
        int newState = ruleMat.Q_;

        int score = max(ruleMat.Q_, newState + 1) + ruleMat.C_;
        state = newState;
        color = 1;  
        bestScore = score;
    }

    {
        int newColor = max(1, ruleMat.C_);

        int score = ruleMat.Q_ + max(ruleMat.C_, newColor + 1);
        if (score < bestScore) {
            state = 1;  
            color = newColor;
        }
    }
    VASSERT(color >= 0);
    VASSERT(state >= 0);

    return false;
}

void RegisterLastStampDir(SolverState& solverState, const dvec<pint>& stampOrder) {
    auto& cellsStamps = solverState.cellsStamps_;
    auto& stateDirTbl = solverState.stateDirTbl_;

    int cell = stampOrder.front().a;
    int si = stampOrder.front().b;
    auto& stamp = cellsStamps[cell][si];

    VASSERT(stamp.outState_ < 0);
    stamp.outState_ = 0;
    stateDirTbl.Register(0, stamp.outDir_);
}

void FinalizeAssign(SolverState& solverState, const dvec<pint>& stampOrder) {
    auto& cellsStamps = solverState.cellsStamps_;
    auto& ruleMat = solverState.ruleMat_;
    auto& stateDirTbl = solverState.stateDirTbl_;

    REP(i, ruleMat.Q_) {
        ruleMat.AddRule(0, i, 0, i, stateDirTbl.GetDir(i), 1, 1);
    }

    int nextState = 0;
    auto [cell, si] = stampOrder.back();
    nextState = cellsStamps[cell][si].inState_;
    ruleMat.SwapStartState(nextState);
    stateDirTbl.SwapStartState(nextState);
}

bool Assign(SolverState& solverState, const dvec<pint>& stampOrder, int limitC, int limitQ, bool limitEnd,
            const AssignParam& param, bool useWrongDir) {

    auto& cellsStamps = solverState.cellsStamps_;
    auto& ruleMat = solverState.ruleMat_;
    auto& stateDirTbl = solverState.stateDirTbl_;

    RegisterLastStampDir(solverState, stampOrder);

    REP(i, stampOrder.size()) {
        int cell = stampOrder[i].a;
        int si = stampOrder[i].b;

        auto& stamp = cellsStamps[cell][si];


        int inColor = -1;
        int inState = -1;
        if (i + 1 < stampOrder.size()) {
            FindRule2(solverState, cell, si, inColor, inState, limitC, limitQ, param, useWrongDir);
        }
        else {
            FindFinalRule(solverState, cell, si, inColor, inState, limitC, limitQ, param);
        }
        solverState.PushRule(cell, si, inColor, inState, limitC, limitQ);

        if (limitEnd) {
            if (limitC >= 0 && ruleMat.C_ > limitC) {
                return false;
            }
            if (limitQ >= 0 && ruleMat.Q_ > limitQ) {
                return false;
            }
        }
    }


    FinalizeAssign(solverState, stampOrder);



    return true;
}


void Greedy5(IOResult& result) {
    auto IsOver = [&]() {
        return timer_.IsOver();
    };

    RemakeParam remakeParamBase;
    remakeParamBase.curveCost_ = HP.CurveCost;
    remakeParamBase.backCost_ = HP.BackCost;
    remakeParamBase.pastCurveCost_ = HP.PastCurveCost;
    remakeParamBase.futureLineCost_ = HP.FutureLineCost;

    RemakeParam remakeParamSigma;
    remakeParamSigma.curveCost_ = HP.CurveCostSigma;
    remakeParamSigma.backCost_ = HP.BackCostSigma;
    remakeParamSigma.pastCurveCost_ = HP.PastCurveCostSigma;
    remakeParamSigma.futureLineCost_ = HP.FutureLineCostSigma;

    AssignParam assignParamBase;
    assignParamBase.reuseScore_ = HP.ReuseScore;
    assignParamBase.updateNextColorScore_ = HP.UpdateNextColorScore;
    assignParamBase.addScore_ = HP.AddScore;
    assignParamBase.newRuleScore_ = HP.NewRuleScore;
    assignParamBase.futureSideStampColorScore_ = HP.FutureSideStampColorScore;
    assignParamBase.nextStampColorScore_ = HP.NextStampColorScore;
    assignParamBase.futureScoreRate_ = HP.FutureScoreRate;
    assignParamBase.sameStateScore_ = HP.SameStateScore;
    assignParamBase.sameColorScore_ = HP.SameColorScore;
    assignParamBase.colorCountScore_ = HP.ColorCountScore;
    assignParamBase.stateCountScore_ = HP.StateCountScore;
    assignParamBase.ruleCountScore_ = HP.RuleCountScore;
    assignParamBase.balanceStateStep_ = HP.BalanceStateStep;
    assignParamBase.futureFreeColorCountScore_ = HP.FutureFreeColorCountScore;
    assignParamBase.freeColorRuleScore_ = HP.FreeColorRuleScore;

    AssignParam assignParamSigma;
    assignParamSigma.reuseScore_ = HP.ReuseScoreSigma;
    assignParamSigma.updateNextColorScore_ = HP.UpdateNextColorScoreSigma;
    assignParamSigma.addScore_ = HP.AddScoreSigma;
    assignParamSigma.newRuleScore_ = HP.NewRuleScoreSigma;
    assignParamSigma.futureSideStampColorScore_ = HP.FutureSideStampColorScoreSigma;
    assignParamSigma.nextStampColorScore_ = HP.NextStampColorScoreSigma;
    assignParamSigma.futureScoreRate_ = HP.FutureScoreRateSigma;
    assignParamSigma.sameStateScore_ = HP.SameStateScoreSigma;
    assignParamSigma.sameColorScore_ = HP.SameColorScoreSigma;
    assignParamSigma.colorCountScore_ = HP.ColorCountScoreSigma;
    assignParamSigma.stateCountScore_ = HP.StateCountScoreSigma;
    assignParamSigma.ruleCountScore_ = HP.RuleCountScoreSigma;
    assignParamSigma.balanceStateStep_ = HP.BalanceStateStepSigma;
    assignParamSigma.futureFreeColorCountScore_ = HP.FutureFreeColorCountScoreSigma;
    assignParamSigma.freeColorRuleScore_ = HP.FreeColorRuleScoreSigma;

    PreCalcAssignParam(assignParamBase);

    PathRemaker remaker;

    dvec<int> firstAllPath;
    int shortestPathLen;
    {
        CalcTargetsPathAll2Fast(firstAllPath);

        shortestPathLen = firstAllPath.size() - 1;


        remaker.Init(firstAllPath);
    }

    remaker.Remake(remakeParamBase, HP.FirstRemakeCount);


    static SolverState solverState;
    solverState.Init();

    static dvec<pint> stampOrder;
    static dvec<dvec<Stamp>> baseCellsStamps;
    {
        CalcStamps(remaker.allPath_, baseCellsStamps, stampOrder);

    }

    double y = sqrt(0.1814 * shortestPathLen + 28.326);
    int firstLimitCQ = (int)(y * 0.8);

    {

        solverState.cellsStamps_ = baseCellsStamps;
        Assign(solverState, stampOrder, firstLimitCQ, firstLimitCQ, false, assignParamBase, false);

    }

    int appendRemakeCount = 0;
    int appendAssignCount = 0;
    static SolverState secondSolverState;
    bool bestIsWrongDir = false;

    while (!IsOver()) {


        RemakeParam remakeParam = MakeRemakeParamAroundBase(remakeParamBase, remakeParamSigma, HP.RemakeParamBaseScale);
        remaker.Remake(remakeParam, 1);
        ++appendRemakeCount;
        if (IsOver()) {
            break;
        }

        CalcStamps(remaker.allPath_, baseCellsStamps, stampOrder);
        if (IsOver()) {
            break;
        }

        REP(assignLoop, HP.AssignLoop) {
            AssignParam assignParam = MakeAssignParamAroundBase(assignParamBase, assignParamSigma, HP.AssignParamSigma);

            int curScore = solverState.ruleMat_.GetScore();
            int curQ = solverState.ruleMat_.Q_;
            int curC = solverState.ruleMat_.C_;
            static dvec<pint> limitCQs;
            limitCQs.clear();
            limitCQs.push(pint{curC - 1, curQ});
            limitCQs.push(pint{curC, curQ - 1});
            for (auto [limitC, limitQ] : limitCQs) {
                secondSolverState.Init();
                secondSolverState.cellsStamps_ = baseCellsStamps;

                bool useWrongDir = timer_.ElapseTimeMs() > TIME_LIMIT * HP.WrongDirTime;
                if (Assign(secondSolverState, stampOrder, limitC, limitQ, true, assignParam, useWrongDir)) {
                    if (secondSolverState.ruleMat_.GetScore() < solverState.ruleMat_.GetScore()) {
                        solverState = secondSolverState;
                        bestIsWrongDir = useWrongDir;

                        break;
                        ++appendAssignCount;
                    }
                }

                ++appendAssignCount;
                if (IsOver()) {
                    break;
                }
            }
            if (IsOver()) {
                break;
            }
        }
        if (IsOver()) {
            break;
        }
    }



    cauto& ruleMat = solverState.ruleMat_;
    cauto& cellsStamps = solverState.cellsStamps_;
    result.Init();
    result.rules_ = ruleMat.rules_;
    result.C_ = ruleMat.C_;
    result.Q_ = ruleMat.Q_;
    REP(i, NN) {
        cauto& cellStamps = cellsStamps[i];
        if (cellStamps.size()) {
            result.initialColors_[i] = cellStamps.back().inColor_;
        }
    }



}

struct Solver {
    void Run() {
        IOResult result;
        result.Init();
        Greedy5(result);
        server.Output(result);

    }
};

struct Main {
    void Run(int argc, const char* argv[]) {
        VASSERT(argc == 1);
        server.InitInput(timer_);

        timer_.StartMs(TIME_LIMIT);

        static Solver solver;
        solver.Run();

        server.Finalize();

    }
};
