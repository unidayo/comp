#define FULL_LIB 1

#define CODETEST 0
#define OPTUNE 0
#define PERFORMANCE 0
#define EVAL 0
#define UNIT_TEST 0

#define TIME_LIMIT (1900)

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




template <class T, int CAP>
struct CapMap {
private:
	using Key = typename conditional<CAP <= 256, u8, u32>::type;
	CapArr<pr2<Key, T>, CAP> elemens;		
	array<Key, CAP> indexs;		


public:

	bool operator == (const CapMap<T, CAP>& r) const {
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
	bool operator != (const CapMap<T, CAP>& r) const {
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




struct CheckMapD {
   private:
    vector<u32> checked_;
    u32 mark_ = 1;

   public:
    void Init(int size) {
        checked_.resize(size, mark_);
        Clear();
    }

    void SetSize(int size) {
        checked_.resize(size, mark_);
    }
    void Clear() {
        ++mark_;

        if (mark_ == 0) {
            checked_.assign(checked_.size(), 0);
            ++mark_;
        }
    }
    inline bool IsChecked(int i) const {
        return checked_[i] == mark_;
    }

    bool Enter(int i) {
        if (checked_[i] == mark_) {
            return false;
        }
        checked_[i] = mark_;
        return true;
    }

    inline bool operator[](int i) const {
        return checked_[i] == mark_;
    }

    inline void Check(int i) {
        checked_[i] = mark_;
    }

    inline void Reset(int i) {
        checked_[i] = mark_ - 1;
    }
};

template <class K>
struct CapHashSet {
    uint32_t m_capacity;
    K* m_keys;
    CheckMapD m_states;
    uint32_t m_mask;
    uint32_t m_count = 0;


    ~CapHashSet() {
        free(m_keys);
    }

    CapHashSet() {
    }

    CapHashSet(const CapHashSet&) = delete;
    void operator=(const CapHashSet&) = delete;

    CapHashSet(CapHashSet&& r) noexcept
        : m_capacity(r.m_capacity),
          m_keys(r.m_keys),
          m_states(r.m_states),
          m_mask(r.m_mask),
          m_count(r.m_count)
    {
        r.m_capacity = 0;
        r.m_keys = nullptr;
        r.m_states.Clear();
        r.m_mask = 0;
        r.m_count = 0;
    }

    void init(uint32_t capacity) {
        VASSERT(capacity > 0);
        m_capacity = 1ULL << (32 - countl_zero(u32(capacity - 1)));
        VASSERT(m_capacity >= capacity);
        m_mask = m_capacity - 1;
        m_keys = (K*)malloc(m_capacity * sizeof(K));
        VASSERT(m_keys);
        m_states.Init(m_capacity);
        m_count = 0;
    }

    void clear() {
        m_states.Clear();
        m_count = 0;
    }

    inline void set(K key) {
        uint32_t i = key & m_mask;
        for (;;) {
            if (m_states[(int)i]) {
                if (key == m_keys[i]) {
                    break;
                }
            }
            else {
                m_states.Check((int)i);
                m_keys[i] = key;
                ++m_count;
                break;
            }
            (++i) &= m_mask;
        }
    }
    inline void insert(K key) {
        set(key);
    }

    inline bool contains(K key) const {
        uint32_t i = key & m_mask;
        for (;;) {
            if (m_states[(int)i]) {
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

    bool enter(K key) {
        uint32_t i = key & m_mask;
        for (;;) {
            if (m_states[i]) {
                if (key == m_keys[i]) {
                    return false;
                }
            }
            else {
                m_states.Check((int)i);
                m_keys[i] = key;
                ++m_count;
                return true;
            }
            (++i) &= m_mask;
        }
    }

    int size() const {
        return m_count;
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
using dvec = std::conditional_t<std::is_trivial_v<T>, dvec_trivial::dvec<T>, dvec_non_trivial::dvec<T>>;



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
    using name##CheckMap = CheckMapS<size>;

constexpr int N_min = 20;
constexpr int N_max = 40;
constexpr int NN_min = N_min * N_min;
constexpr int NN_max = N_max * N_max;

DEF_CONTAINERS(N, N_max);
DEF_CONTAINERS(NN, NN_max);

int N;
int NN;
int K;  

#define PARAM_CATEGORY(NAME, VALUE, ...) int NAME = VALUE;
#define PARAM_INT(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, ...) int NAME = VALUE;
#define PARAM_DOUBLE(NAME, VALUE, LOWER_VALUE, UPPER_VALUE, ...) double NAME = VALUE;


#define PARAM_LOWER(v)
#define PARAM_UPPER(v)
#define START_TUNING
#define END_TUNING

#define PARAM_GROUP(NAME)
#define PARAM_GROUP_END


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



    PI(MainChangeRemoveSizeMin, 2, 1, 4, 1);
    PI(MainChangeRemoveSizeMax, 9, 4, 12, 1);
    PD(MainChangeNewMaxSize, 3, 2, 4, 1);


    START_TUNING;
    PD(MainPathLength, 0.6135322735210452, 0.3, 0.9);
    PD(MainSplitPathLength, 0.7655433081564618, 0.5, 1.0);
    PD(MainScoreRate, 0.9414064093809085, 0.5, 1.5);
    PD(BranchScoreRate, 0.8049027159966715, 0.5, 1.5);
    PD(MainScoreRate_NN, -0.14958129436644427, -0.1, 0.1);
    PD(BranchScoreRate_NN, 0.13261294203701154, -0.1, 0.1);

    END_TUNING;
    PD(SafeScore, 2.306033504282567, 1.5, 3.0);
    PD(ExitPenalty, 9.818679095809664, 7.0, 12.0);

    PD(BranchBaseScore, 5.031883677945023, 3.0, 7.0);
    PD(BranchRateExp, 0.4413968504795551, 0.3, 0.55);
    PD(BranchSafeBonus, 2.2540379815708746, 1.7, 2.7);
    PD(BranchCurveExp, 0.6492380791876408, 0.4, 0.8);


    PD(MainScoreRate_Kr, 0.0, -0.1, 0.1);
    PD(BranchScoreRate_Kr, 0.0, -0.1, 0.1);

    PI(TransMainChange, 39, 50, 150, 0);
    PI(TransEdgeCahange, 5, 0, 10, 0);
    PI(TransRandomBranchChange, 0, 0, 10, 0);
    PI(TransBadBranchChange, 10, 0, 10, 0);


    PD(ExitLength, -1.8336484142875682, -1.0, 1.0);
    PD(BranchLength, 0.1506641986007809, -1.0, 1.0);

    PD(StartTemp, 4.89596463237755, 0.0, 10.0, 0.0);
    PD(EndTemp, 0.6419210234243955, 0.0, 1.0, 0.0);
    PC(TempType, 0, 0, 1, 2);
    PD(TempPow, 1.9346661561475385, 1.0, 2.0, 1.0);

    PI(InitialStateCount, 1, 1, 8, 1);

    PD(RollbackStartRate, 0, 0.0, 0.15, 0.0, 1.0);
    PI(RollbackCount, 100000, 100, 100000, 1);
    PD(RollbackNextMulti, 1.0, 0.9, 1.1, 0.1);
    PI(MinRollbackCount, 100, 100, 300, 1);

    PI(UpdateTmpMaskBit, 4, 0, 10, 0, 16);


} HP;


#define DEF_DOUBLE_PARAM(name)                                                  \
    static const double name = BlendDoubleParam(HP.name, -1e10, 1e10,           \
                                                {                               \
                                                    ipNN.GetRate(HP.name##_NN), \
                                                    ipKr.GetRate(HP.name##_Kr), \
                                                });
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
constexpr array<pint, 8> Around8 = {pint{-1, 0}, pint{-1, -1}, pint{0, -1}, pint{1, -1},
                                    pint{1, 0},  pint{1, 1},   pint{0, 1},  pint{-1, 1}};

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
        'L', 'U', 'R', 'D', 'N', '*',
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
    vector<CCA<int, 4>> tblr_;     
    vector<CCA<int, 8>> around8_;  

   public:
    void Init(int w, int h) {
        W = w;
        H = h;
        WH = W * H;

        toPos_.resize(WH);
        around_.resize(WH);
        dirs_.resize(WH);
        tblr_.resize(WH);
        around8_.resize(WH);
        REP(y, H) {
            REP(x, W) {
                int id = x + W * y;
                toPos_[id] = pint{x, y};

                REP(di, 4) {
                    const pint& a = Around4[di];
                    int x2 = x + a.a;
                    int y2 = y + a.b;
                    if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
                        int nc = x2 + y2 * W;
                        around_[id].push(nc);
                        dirs_[id][di] = nc;
                    }
                    else {
                        dirs_[id][di] = -1;
                    }
                }

                for (int di : {1, 3, 0, 2}) {
                    const pint& a = Around4[di];
                    int x2 = x + a.a;
                    int y2 = y + a.b;
                    if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
                        int nc = x2 + y2 * W;
                        tblr_[id].push(nc);
                    }
                }

                REP(di, 8) {
                    const pint& a = Around8[di];
                    int x2 = x + a.a;
                    int y2 = y + a.b;
                    if (x2 >= 0 && x2 < W && y2 >= 0 && y2 < H) {
                        int nc = x2 + y2 * W;
                        around8_[id].push(nc);
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

    int CalcMovedCell(int start, const initializer_list<Dir>& moveDirs) {
        if (start < 0) {
            return -1;
        }
        int p = start;
        for (Dir d : moveDirs) {
            int n = dirs_[p][(int)d];
            if (n < 0) {
                return -1;
            }
            p = n;
        }
        return p;
    }

    inline const CCA<int, 4>& around(int id) const {
        return around_[id];
    }
    inline const array<int, 4>& dirs(int id) const {
        return dirs_[id];
    }
    inline const CCA<int, 4>& tblr(int id) const {
        return tblr_[id];
    }
    inline const CCA<int, 8>& around8(int id) const {
        return around8_[id];
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

double logsumexp(double log_a, double log_b) {
    if (log_a > log_b) {
        return log_a + log1p(exp(log_b - log_a));
    }
    else {
        return log_b + log1p(exp(log_a - log_b));
    }
}

bool IsReachable(const NNArr<bool>& isTree, int start, int goal) {
    if (start == goal) {
        return true;
    }

    NNArr<bool> visited;
    visited.assign(NN, false);

    NNQue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int p = q.pop();
        for (cauto& n : gs.around(p)) {
            if (isTree[n]) {
                continue;
            }
            if (visited[n]) {
                continue;
            }
            if (n == goal) {
                return true;
            }
            visited[n] = true;
            q.push(n);
        }
    }
    return false;
}

bool CalcPath(const NNArr<bool>& isTree, int start, int goal, dvec<int>& path) {
    path.clear();
    if (start == goal) {
        path.push(start);
        return true;
    }

    NNArr<bool> visited;
    visited.assign(NN, false);
    NNArr<int> froms;
    froms.assign(NN, -1);

    NNQue<int> q;
    q.push(goal);
    visited[goal] = true;
    froms[goal] = -1;

    while (!q.empty()) {
        int p = q.pop();
        for (cauto& n : gs.around(p)) {
            if (isTree[n]) {
                continue;
            }
            if (visited[n]) {
                continue;
            }
            visited[n] = true;
            froms[n] = p;
            q.push(n);
            if (n == start) {
                q.clear();
                break;
            }
        }
    }
    if (froms[start] >= 0) {
        for (int p = start; p >= 0; p = froms[p]) {
            path.push(p);
        }
        return true;
    }
    return false;
}

int CalcReachCount(const NNArr<bool>& isTree, int start) {
    NNArr<bool> visited;
    visited.assign(NN, false);

    NNQue<int> q;
    q.push(start);
    visited[start] = true;

    int count = 1;

    while (!q.empty()) {
        int p = q.pop();
        for (cauto& n : gs.around(p)) {
            if (isTree[n]) {
                continue;
            }
            if (visited[n]) {
                continue;
            }
            visited[n] = true;
            ++count;
            q.push(n);
        }
    }
    return count;
}

void MakeDistMap(const NNArr<bool>& isTree, int start, NNArr<int>& dist) {
    dist.assign(NN, INT_MAX);
    dist[start] = 0;

    NNQue<int> q;
    q.push(start);

    while (!q.empty()) {
        int p = q.pop();
        for (cauto& n : gs.around(p)) {
            if (isTree[n]) {
                continue;
            }
            if (dist[p] + 1 < dist[n]) {
                dist[n] = dist[p] + 1;
                q.push(n);
            }
        }
    }
}

void MakeReachMap(const NNArr<bool>& isTree, int start, NNArr<bool>& reach) {
    reach.assign(NN, false);
    reach[start] = true;

    NNQue<int> q;
    q.push(start);

    while (!q.empty()) {
        int p = q.pop();
        for (cauto& n : gs.around(p)) {
            if (isTree[n]) {
                continue;
            }
            if (!reach[n]) {
                reach[n] = true;
                q.push(n);
            }
        }
    }
}

void MakeTempMap(const NNArr<bool>& isTree, const NNArr<bool>& isSeen, NNArr<bool>& tmpIsTree) {
    tmpIsTree.assign(NN, false);
    REP(i, NN) {
        if (isSeen[i]) {
            tmpIsTree[i] = isTree[i];
        }
        else {
            tmpIsTree[i] = false;
        }
    }
}



InputParam<int> ipNN(NN_min, NN_max);
InputParam<double> ipKr(0, 1.0 / 6.0);

struct IOServer {

    int turn_;
    int target_;          
    int pos_;             
    NNArr<bool> isTree_;  
    NNArr<bool> isSeen_;  

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        is >> N;
        timer.Init();  

        turn_ = 0;

        NN = N * N;
        gs.Init(N, N);

        int ty, tx;
        is >> ty >> tx;
        target_ = gs.ToId(tx, ty);

        isTree_.assign(NN, false);
        isSeen_.assign(NN, false);

        K = 0;
        char c;
        REP(i, NN) {
            is >> c;
            if (c == 'T') {
                isTree_[i] = true;
                K++;
            }
        }

        ipNN.SetValue(NN);
        ipKr.SetValue(K / (double)NN);

        pos_ = -1;

        turn_ = -1;
        Input();
        turn_ = 0;
    }

    void Input() {
        if (turn_ == 0) {
            return;
        }
        istream& is = cin;
        int py, px;
        is >> py >> px;
        pos_ = gs.ToId(px, py);

        int s;
        is >> s;
        int y, x;
        REP(i, s) {
            is >> y >> x;
            int id = gs.ToId(x, y);
            isSeen_[id] = true;
        }

    }

    bool IsEnd() const {
        return pos_ == target_;
    }

    void Output(const dvec<int>& result) {
        ostream& os = cout;
        os << result.size();
        REP(i, result.size()) {
            isTree_[result[i]] = true;
            cauto& p = gs.ToPos(result[i]);
            os << " " << p.y << " " << p.x;
        }
        os << endl;

        ++turn_;

    }

    void OutputSkip() {
        ostream& os = cout;
        os << -1 << endl;

    }

    void Finalize() {

    }
};
IOServer server;

Xor64 rand_;


bool FindOutline(const NNArr<bool>& isTree, int startCell, Dir startWallDir, dvec<int>& outline) {
    outline.clear();

    Dir dir = RotateLeft(startWallDir);  
    int p = startCell;
    outline.push(p);
    while (outline.size() == 1 || p != startCell) {
        if (outline.size() >= NN) {
            return false;
        }
        Dir nextDir = RotateRight(dir);
        bool moved = false;
        REP(i, 4) {
            int n = gs.dirs(p)[(int)nextDir];
            if (n >= 0 && !isTree[n]) {
                outline.push(n);
                p = n;
                dir = nextDir;
                moved = true;
                break;
            }
            nextDir = RotateLeft(nextDir);
        }
        if (!moved) {
            return false;
        }
    }
    return true;
}

bool FindOutlineEx(const NNArr<bool>& isTree, const NNArr<bool>& firstZone, int startCell, Dir startWallDir,
                   dvec<int>& outline) {
    outline.clear();

    static NNCheckMap canMove;
    canMove.Clear();

    Dir dir = RotateLeft(startWallDir);  
    int p = startCell;
    outline.push(p);
    bool inFirstZone = true;
    while (outline.size() == 1 || p != startCell) {
        if (outline.size() >= NN) {
            return false;
        }
        Dir nextDir = RotateRight(dir);
        bool moved = false;
        REP(i, 4) {
            int n = gs.dirs(p)[(int)nextDir];
            if (n >= 0 && (!isTree[n] || (inFirstZone && firstZone[n]) || canMove[n])) {
                outline.push(n);
                p = n;
                dir = nextDir;
                moved = true;
                if (isTree[n]) {
                    canMove.Check(n);
                }
                if (inFirstZone && !firstZone[n]) {
                    inFirstZone = false;
                }
                break;
            }
            nextDir = RotateLeft(nextDir);
        }
        if (!moved) {
            return false;
        }
    }

    if (inFirstZone) {
        return false;
    }

    return true;
}

void FindDivPath(const NNArr<bool>& isWall, int startCell, int goalCell, dvec<int>& path) {
    path.clear();

    if (startCell == goalCell) {
        if (!isWall[startCell]) path.push(startCell);
        return;
    }
    if (startCell < 0 || goalCell < 0) return;
    if (isWall[startCell] || isWall[goalCell]) return;

    struct Node {
        int cell;
        int g;     
        int f;     
        int perp;  
    };
    struct Cmp {
        bool operator()(const Node& a, const Node& b) const {
            if (a.f != b.f) return a.f > b.f;              
            if (a.g != b.g) return a.g < b.g;              
            if (a.perp != b.perp) return a.perp > b.perp;  
            return a.cell > b.cell;                        
        }
    };

    auto manhattan = [&](int a, int b) -> int {
        pint pa = gs.ToPos(a);
        pint pb = gs.ToPos(b);
        return std::abs(pa.x - pb.x) + std::abs(pa.y - pb.y);
    };

    const pint S = gs.ToPos(startCell);
    const pint G = gs.ToPos(goalCell);
    const int Dx = G.x - S.x;
    const int Dy = G.y - S.y;
    auto perp_numer = [&](int c) -> int {
        pint p = gs.ToPos(c);
        long long vx = p.x - S.x;
        long long vy = p.y - S.y;
        long long num = vx * (long long)Dy - vy * (long long)Dx;
        if (num < 0) num = -num;
        return (int)num;
    };

    const int INF = int(1e9);
    NNArr<int> gScore(NN, INF);
    NNArr<int> cameFrom(NN, -1);
    NNArr<char> closed(NN, 0);
    std::priority_queue<Node, std::vector<Node>, Cmp> open;

    gScore[startCell] = 0;
    open.push(Node{startCell, 0, manhattan(startCell, goalCell), perp_numer(startCell)});

    bool found = false;

    while (!open.empty()) {
        Node cur = open.top();
        open.pop();
        if (closed[cur.cell]) continue;
        closed[cur.cell] = 1;

        if (cur.cell == goalCell) {
            found = true;
            break;
        }

        for (int ni : gs.around(cur.cell)) {
            if (ni < 0) continue;      
            if (isWall[ni]) continue;  
            if (closed[ni]) continue;

            int tentative_g = cur.g + 1;  
            if (tentative_g < gScore[ni]) {
                gScore[ni] = tentative_g;
                cameFrom[ni] = cur.cell;
                int h = manhattan(ni, goalCell);
                int f = tentative_g + h;
                int per = perp_numer(ni);
                open.push(Node{ni, tentative_g, f, per});
            }
        }
    }

    if (!found && cameFrom[goalCell] == -1) {
        path.clear();
        return;
    }

    path.clear();
    for (int p = goalCell; p != -1; p = cameFrom[p]) {
        path.push(p);
        if (p == startCell) break;
    }
    std::reverse(path.begin(), path.end());
}

bool CalcPathSideTrees(const NNArr<bool>& isTree, int startCell, const dvec<int>& path, dvec<int>& sideWalls,
                       array<int, 2>& sidesSize, array<int, 2>& sidesHole) {
    sideWalls.clear();

    NNArr<s8> sideMap(NN, -1);       
    NNArr<int> sideTurnMap(NN, -1);  
    REP(pi, path.size()) {
        if (pi - 1 >= 0) {
            int prev = path[pi - 1];
            int cur = path[pi];
            Dir dir = gs.CalcDir(prev, cur);
            {
                int n = gs.dirs(cur)[(int)RotateLeft(dir)];
                if (n >= 0 && !isTree[n]) {
                    sideMap[n] = 0;  
                    chmax(sideTurnMap[n], pi);
                }
            }
            {
                int n = gs.dirs(cur)[(int)RotateRight(dir)];
                if (n >= 0 && !isTree[n]) {
                    sideMap[n] = 1;  
                    chmax(sideTurnMap[n], pi);
                }
            }
        }
        if (pi + 1 < path.size()) {
            int next = path[pi + 1];
            int cur = path[pi];
            Dir dir = gs.CalcDir(cur, next);
            {
                int n = gs.dirs(cur)[(int)RotateLeft(dir)];
                if (n >= 0 && !isTree[n]) {
                    sideMap[n] = 0;  
                    chmax(sideTurnMap[n], pi);
                }
            }
            {
                int n = gs.dirs(cur)[(int)RotateRight(dir)];
                if (n >= 0 && !isTree[n]) {
                    sideMap[n] = 1;  
                    chmax(sideTurnMap[n], pi);
                }
            }
        }
    }

    REP(pi, path.size()) {
        sideMap[path[pi]] = -2;
    }

    for (int n : gs.around(path.back())) {
        if (!isTree[n] && sideMap[n] == -1) {
            sideMap[n] = 2;
        }
    }

    static NNQue<int> que;
    static NNCheckMap check;

    {
        que.clear();
        check.Clear();
        array<int, 2> sidesCount = {0, 0};
        REP(i, NN) {
            if (sideMap[i] >= 0 && sideMap[i] <= 1) {
                sidesCount[sideMap[i]]++;
            }
            if (sideMap[i] == 0) {
                que.push(i);
                check.Check(i);
            }
        }
        if (sidesCount[0] == 0 || sidesCount[1] == 0) {
            return false;
        }

        while (!que.empty()) {
            int p = que.pop();
            for (int n : gs.around(p)) {
                if (isTree[n]) {
                    continue;
                }
                if (sideMap[n] == -2 || sideMap[n] == 2) {
                    continue;
                }
                if (check.Enter(n)) {
                    if (sideMap[n] == 1) {
                        return false;
                    }
                    que.push(n);
                }
            }
        }
    }

    array<int, 2> sidesBestHole = {-1, -1};
    array<int, 2> sidesBestSize = {-1, -1};
    array<int, 2> sidesBestTurn = {-1, -1};
    REP(i, NN) {
        if (sideMap[i] < 0 || sideMap[i] == 2) {
            continue;
        }

        if (gs.CalcL1Dist(i, path.back()) > 1) {
            continue;
        }

        que.clear();
        check.Clear();
        {
            int start = i;
            que.push(start);
            check.Check(start);
        }
        while (!que.empty()) {
            int p = que.pop();
            for (int n : gs.around(p)) {
                if (isTree[n]) {
                    continue;
                }
                if (sideMap[n] != -1) {
                    continue;
                }
                if (check.Enter(n)) {
                    que.push(n);
                }
            }
        }

        int side = sideMap[i];
        int size = que.total_push_count();
        int turn = sideTurnMap[i];
        if (size > sidesBestSize[side] || (size == sidesBestSize[side] && turn > sidesBestTurn[side])) {
            sidesBestSize[side] = size;
            sidesBestHole[side] = i;
            sidesBestTurn[side] = turn;
        }
    }
    if (sidesBestSize[0] < 0 || sidesBestSize[1] < 0) {
        return false;
    }
    VASSERT(sidesBestSize[0] >= 0 || sidesBestSize[1] >= 0);

    REP(i, NN) {
        if (sideMap[i] >= 0) {
            if (i == sidesBestHole[0] || i == sidesBestHole[1]) {
                continue;
            }
            sideWalls.push(i);
        }
    }
    sidesSize = sidesBestSize;
    sidesHole = sidesBestHole;
    return true;
}

bool FindDivLine(const NNArr<bool>& isTree, int startCell, Dir startWallDir, dvec<int>& path, dvec<int>& sideWalls,
                 array<int, 2>& sidesSize, array<int, 2>& sidesHole) {
    dvec<int> outline;
    if (!FindOutline(isTree, startCell, startWallDir, outline)) {
        return false;
    }

    double bestScore = -1e100;
    dvec<int> bestPath;
    dvec<int> bestSideWalls;
    array<int, 2> bestSidesSize;
    array<int, 2> bestSidesHole;
    dvec<int> tmpPath;
    dvec<int> tmpSideWalls;
    array<int, 2> tmpSidesSize;
    array<int, 2> tmpSidesHole;
    REP(i, outline.size()) {
        int goalCell = outline[i];
        if (goalCell == startCell) {
            continue;
        }
        FindDivPath(isTree, startCell, goalCell, tmpPath);

        if (!CalcPathSideTrees(isTree, startCell, tmpPath, tmpSideWalls, tmpSidesSize, tmpSidesHole)) {
            continue;
        }

        double score = 0;
        score += min(tmpSidesSize[0], tmpSidesSize[1]) * 1000;
        score += (tmpSidesSize[0] + tmpSidesSize[1]) * 1;
        if (score > bestScore) {
            bestScore = score;
            bestPath = tmpPath;
            bestSideWalls = tmpSideWalls;
            bestSidesSize = tmpSidesSize;
            bestSidesHole = tmpSidesHole;
        }
    }

    if (bestPath.empty()) {
        return false;
    }

    path = bestPath;
    sideWalls = bestSideWalls;
    sidesSize = bestSidesSize;
    sidesHole = bestSidesHole;
    return true;
}
bool FindDivLine(const NNArr<bool>& isTree, int startCell, Dir startWallDir, dvec<int>& path) {
    dvec<int> outline;
    if (!FindOutline(isTree, startCell, startWallDir, outline)) {
        return false;
    }

    double bestScore = -1e100;
    dvec<int> bestPath;
    dvec<int> bestSideWalls;
    array<int, 2> bestSidesSize;
    array<int, 2> bestSidesHole;
    dvec<int> tmpPath;
    dvec<int> tmpSideWalls;
    array<int, 2> tmpSidesSize;
    array<int, 2> tmpSidesHole;
    REP(i, outline.size()) {
        int goalCell = outline[i];
        if (goalCell == startCell) {
            continue;
        }
        FindDivPath(isTree, startCell, goalCell, tmpPath);

        if (!CalcPathSideTrees(isTree, startCell, tmpPath, tmpSideWalls, tmpSidesSize, tmpSidesHole)) {
            continue;
        }

        double score = 0;
        score += min(tmpSidesSize[0], tmpSidesSize[1]) * 1000;
        score += (tmpSidesSize[0] + tmpSidesSize[1]) * 1;
        if (score > bestScore) {
            bestScore = score;
            bestPath = tmpPath;
            bestSideWalls = tmpSideWalls;
            bestSidesSize = tmpSidesSize;
            bestSidesHole = tmpSidesHole;
        }
    }

    if (bestPath.empty()) {
        return false;
    }

    path = bestPath;
    return true;
}


void CleanPath(dvec<int>& outline, bool closed) {
    {

        auto Detect = [&](int& idxA, int& idxB) -> bool {
            NNArr<int> lastVisit(NN, -1);
            REP(i, outline.size()) {
                int cell = outline[i];
                for (int n : gs.around(cell)) {
                    if (lastVisit[n] == i - 1) {
                        continue;
                    }
                    if (closed) {
                        if (lastVisit[n] == 0) {
                            continue;
                        }
                    }
                    if (lastVisit[n] >= 0) {
                        idxA = lastVisit[n];
                        idxB = i;
                        return true;
                    }
                }
                lastVisit[cell] = i;
            }
            return false;
        };

        auto Cut = [&](int idxA, int idxB) {
            VASSERT(idxA < idxB);
            if (closed) {
                int sizeAB = idxB - idxA;
                int sizeBA = outline.size() - sizeAB;
                if (sizeAB <= sizeBA) {
                    dvec<int> newOutline;
                    REP(i, idxA + 1) {
                        newOutline.push(outline[i]);
                    }
                    FOR(i, idxB, outline.size()) {
                        newOutline.push(outline[i]);
                    }
                    outline = newOutline;
                }
                else {
                    dvec<int> newOutline;
                    FOR(i, idxA, idxB + 1) {
                        newOutline.push(outline[i]);
                    }
                    outline = newOutline;
                }
            }
            else {
                int sizeAB = idxB - idxA;

                dvec<int> newOutline;
                REP(i, idxA + 1) {
                    newOutline.push(outline[i]);
                }
                FOR(i, idxB, outline.size()) {
                    newOutline.push(outline[i]);
                }
                outline = newOutline;
            }
        };

        while (true) {
            int idxA = -1, idxB = -1;
            if (Detect(idxA, idxB)) {
                Cut(idxA, idxB);
            }
            else {
                break;
            }
        }
    }
}

bool FindDiv2Line(const NNArr<bool>& isTree, int baseStartCell, Dir baseWallDir, int subStartCell, Dir subWallDir,
                  dvec<int>& basePath, dvec<int>& subPath) {
    auto tmpTree = isTree;
    tmpTree[subStartCell] = true;

    dvec<int> sideWalls;
    array<int, 2> sidesSize;
    array<int, 2> sidesHole;
    if (!FindDivLine(tmpTree, baseStartCell, baseWallDir, basePath, sideWalls, sidesSize, sidesHole)) {
        return false;
    }

    tmpTree = isTree;
    for (int cell : sideWalls) {
        tmpTree[cell] = true;
    }
    for (int cell : sidesHole) {
        tmpTree[cell] = true;
    }

    dvec<int> outline;
    if (!FindOutline(tmpTree, subStartCell, subWallDir, outline)) {
        return false;
    }

    subPath = outline;
    subPath.resize(subPath.size() * 3 / 4);
    CleanPath(subPath, false);

    return true;
}




enum class SAResult : u8 {
    Ok,    
    Ng,    
    Fail,  
};

template <class Pack, class RawScoreType>
struct StateData {
    Pack best_pack;               
    RawScoreType best_raw_score;  
    f64 eval_score;               

    s32 no_best_try_count;   
    s32 rollback_try_count;  

    template <class State>
    void Init(const State& state, int rollback_count) {
        state.Save(best_pack);
        best_raw_score = state.RawScore();
        eval_score = state.EvalScore();
        no_best_try_count = 0;
        rollback_try_count = rollback_count;
    }
};

class Config {
   public:
    int update_tmp_mask = 127;

    int8_t temp_type = 0;
    double temp_pow = 2.0;

    double rollback_start_time_rate = 1.0;  
    int rollback_count = INT_MAX;           
    double rollback_mul = 1.0;              
    int min_rollback_count = 1;             

    double filter_start_time = 0.2;  
    double filter_end_time = 1.0;    
};

class SAChecker {
   public:
    double temp = 0.0;
    double eval_score = 0.0;
    optional<double> accept_value;
    optional<double> accept_score;

    bool CheckAccept(double new_eval_score) {
        if (new_eval_score >= eval_score) {
            return true;
        }
        else {
            if (new_eval_score >= GetAcceptScore()) {
                return true;
            }
        }
        return false;
    }

    double GetAcceptScore() {
        if (!accept_score) {
            if (!accept_value) {
                double rand_value = rand_.GetDouble();
                accept_value = temp * log(rand_value);
            }
            accept_score = eval_score + accept_value.value();
        }
        return accept_score.value();
    }
};

template <class Pack, class RawScoreType>
struct SA {
   private:
    dvec<StateData<Pack, RawScoreType>> datas_;

   public:
    template <class State>
    void Run(dvec<State>& states, int time_limit_us, f64 start_temp, f64 end_temp, const Config& config) {
        datas_.clear();
        for (cauto& state : states) {
            datas_.pushR().Init(state, config.rollback_count);
        }
        dvec<int> sis(states.size());
        iota(ALL(sis), 0);


        auto start_time = min(time_limit_us, timer_.ElapseTimeUs());
        double timeRate = 0.0;
        double temp = 0.0;
        int loop_count = 0;
        int enable_count = 0;
        double div_time = 1.0 / double(time_limit_us - start_time);
        SAChecker checker;
        while (true) {
            {
                if ((loop_count & config.update_tmp_mask) == 0) {
                    auto now = timer_.ElapseTimeUs();
                    if (now >= time_limit_us) {
                        break;
                    }

                    timeRate = (now - start_time) * div_time;
                    temp = GetTemp(start_temp, end_temp, timeRate, config.temp_type, config.temp_pow);
                }
            }
            ++loop_count;

            checker.temp = temp;
            checker.accept_value.reset();

            for (int si : sis) {
                auto& state = states[si];
                auto& data = datas_[si];
                checker.eval_score = data.eval_score;
                checker.accept_score.reset();

                int ti = 0;
                SAResult result = state.Update(checker, timeRate, ti);
                ++data.no_best_try_count;

                if (result == SAResult::Ok) {
                    double eval_score = state.EvalScore();
                    data.eval_score = eval_score;

                    auto raw_score = state.RawScore();
                    if (raw_score > data.best_raw_score) {
                        data.best_raw_score = raw_score;
                        state.Save(data.best_pack);
                        data.no_best_try_count = 0;
                    }


                }
                if (timeRate >= config.rollback_start_time_rate) {
                    if (data.no_best_try_count >= data.rollback_try_count) {
                        state.Load(data.best_pack);
                        data.eval_score = state.EvalScore();
                        data.no_best_try_count = 0;

                        data.rollback_try_count =
                            max(int(data.rollback_try_count * config.rollback_mul), config.min_rollback_count);

                    }
                }
            }

            if (sis.size() > 1) {
                int target_count = states.size();
                if (timeRate >= config.filter_end_time) {
                    target_count = 1;
                }
                else if (timeRate >= config.filter_start_time) {
                    double r =
                        1.0 - (timeRate - config.filter_start_time) /
                                  (config.filter_end_time - config.filter_start_time);  
                    target_count = 1 + (int)floor(states.size() * r);
                }
                if (sis.size() > target_count) {
                    sort(ALL(sis), [&](int a, int b) { return datas_[a].best_raw_score > datas_[b].best_raw_score; });
                    sis.resizeR(target_count);

                }
            }

        }


    }

    double GetTemp(double startTemp, double endTemp, double timeRate, int tempType, double tempPow) {
        if (tempType == 0) {
            return startTemp + (endTemp - startTemp) * timeRate;
        }
        else if (tempType == 1) {
            return startTemp * pow(endTemp / startTemp, timeRate);
        }
        else {
            return startTemp * pow(endTemp / startTemp, pow(timeRate, tempPow));
        }
    }

    const StateData<Pack, RawScoreType>& GetBest() {
        int best = 0;
        FOR(i, 1, datas_.size()) {
            if (datas_[i].best_raw_score > datas_[best].best_raw_score) {
                best = i;
            }
        }
        return datas_[best];
    }

};


struct RandomTable {
    vector<int> table_;

    bool IsEmpty() const {
        return table_.empty();
    }

    void clear() {
        table_.clear();
    }

    void push(int value, int count) {
        table_.reserve(table_.size() + count);
        REP(i, count) {
            table_.emplace_back(value);
        }
    }


    int operator()(Xor64& r) {
        return table_[r(table_.size())];
    }
};


namespace sap {

struct GuardPattern {
    dvec<int> wallCells_;   
    dvec<int> checkCells_;  
    int enterCell_;         
    int innerCell_;         
    int blockCell_;         
    int deadCellCount_;     

    dvec<int> outline_;  

};

dvec<GuardPattern> targetGuardPatterns_;

int initialCell_;
NNArr<bool> aroundTarget_;

bool useGuardPattern_;


int CalcDeadCellCount(const NNArr<bool>& isWall, const NNCheckMap& visible, int enterCell, int innerCell) {
    NNQue<int> que;
    NNArr<int> dists;
    NNArr<int> froms;
    static NNCheckMap danger;
    danger.Clear();
    dists.assign(NN, INT_MAX);
    froms.assign(NN, -1);

    danger.Check(innerCell);

    int deadCellCount = 0;

    {
        int start = enterCell;
        que.push(start);
        dists[start] = 0;
        froms[start] = -1;
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.tblr(p)) {
            if (visible[n] && isWall[n]) {
                continue;
            }
            int d = dists[p] + 1;
            if (d < dists[n]) {
                que.push(n);
                dists[n] = d;
                froms[n] = p;

                if (danger[p]) {
                    danger.Check(n);
                    ++deadCellCount;
                }
            }
        }
    }

    VASSERT(deadCellCount >= 1);
    return deadCellCount;
}

bool IsNeedCheckWall(const dvec<int>& walls, int enterCell, int innerCell, int blockCell) {
    return true;

    static NNCheckMap visible;
    NNArr<bool> isWall = server.isTree_;
    visible.Clear();

    for (int wallCell : walls) {
        if (wallCell == blockCell) {
        }
        else {
            visible.Check(wallCell);
        }
        isWall[wallCell] = true;
    }

    int noVisibleDeadCount = CalcDeadCellCount(isWall, visible, enterCell, innerCell);

    visible.Check(blockCell);
    int visibleDeadCount = CalcDeadCellCount(isWall, visible, enterCell, innerCell);

    return visibleDeadCount < noVisibleDeadCount;
}


void Ready() {
    cauto& isTree = server.isTree_;

    REP(openDir, 4) {
        int openCell = gs.dirs(server.target_)[(int)openDir];
        if (openCell < 0 || isTree[openCell]) {
            continue;
        }

        int leftDir = (int)RotateLeft(Dir(openDir));
        int rightDir = (int)RotateRight(Dir(openDir));

        dvec<int> wallCells;
        int checkCell = -1;

        int p = server.target_;
        REP(i, N) {
            int n = gs.dirs(p)[openDir];
            if (n < 0) {
                break;
            }
            if (isTree[n]) {
                break;
            }

            int leftCell = gs.dirs(n)[leftDir];
            int rightCell = gs.dirs(n)[rightDir];
            bool leftIsOpen = (leftCell >= 0 && !isTree[leftCell]);
            bool rightIsOpen = (rightCell >= 0 && !isTree[rightCell]);

            if (leftIsOpen || rightIsOpen) {
                int nn = gs.dirs(n)[openDir];
                if (nn >= 0) {
                    wallCells.push(nn);
                }

                for (int nn : gs.around(server.target_)) {
                    if (nn != openCell) {
                        wallCells.push(nn);
                    }
                }

                if (leftIsOpen && rightIsOpen) {
                    {
                        auto& pattern = targetGuardPatterns_.push();
                        pattern.wallCells_ = wallCells;
                        pattern.wallCells_.push(leftCell);
                        pattern.enterCell_ = rightCell;
                        pattern.innerCell_ = n;
                        pattern.blockCell_ = nn;
                    }
                    {
                        auto& pattern = targetGuardPatterns_.push();
                        pattern.wallCells_ = wallCells;
                        pattern.wallCells_.push(rightCell);
                        pattern.enterCell_ = leftCell;
                        pattern.innerCell_ = n;
                        pattern.blockCell_ = nn;
                    }
                }
                else if (leftIsOpen) {
                    auto& pattern = targetGuardPatterns_.push();
                    pattern.wallCells_ = wallCells;
                    if (rightCell >= 0) {
                        VASSERT(isTree[rightCell]);
                        pattern.wallCells_.push(rightCell);
                    }
                    pattern.enterCell_ = leftCell;
                    pattern.innerCell_ = n;
                    pattern.blockCell_ = nn;

                    if (rightCell < 0) {
                        pattern.wallCells_.push(gs.dirs(leftCell)[leftDir]);

                        if (!IsNeedCheckWall(pattern.wallCells_, pattern.enterCell_, pattern.innerCell_,
                                             pattern.blockCell_)) {
                            pattern.wallCells_.pop();
                        }
                    }
                }
                else {
                    auto& pattern = targetGuardPatterns_.push();
                    pattern.wallCells_ = wallCells;
                    pattern.enterCell_ = rightCell;
                    if (leftCell >= 0) {
                        VASSERT(isTree[leftCell]);
                        pattern.wallCells_.push(leftCell);
                    }
                    pattern.innerCell_ = n;
                    pattern.blockCell_ = nn;

                    if (leftCell < 0) {
                        pattern.wallCells_.push(gs.dirs(rightCell)[rightDir]);

                        if (!IsNeedCheckWall(pattern.wallCells_, pattern.enterCell_, pattern.innerCell_,
                                             pattern.blockCell_)) {
                            pattern.wallCells_.pop();
                        }
                    }
                }
                break;
            }
            else {
                if (leftCell >= 0) {
                    VASSERT(isTree[leftCell]);
                    wallCells.push(leftCell);
                }
                if (rightCell >= 0) {
                    VASSERT(isTree[rightCell]);
                    wallCells.push(rightCell);
                }
            }
            p = n;
        }
    }

    dvec<int> enables;
    REP(i, targetGuardPatterns_.size()) {
        auto& pattern = targetGuardPatterns_[i];
        auto isTree = server.isTree_;
        for (int cell : pattern.wallCells_) {
            isTree[cell] = true;
        }
        if (IsReachable(isTree, server.pos_, server.target_)) {
            enables.push(i);
        }
    }
    dvec<GuardPattern> enabledPatterns;
    for (int i : enables) {
        enabledPatterns.push(targetGuardPatterns_[i]);
    }
    targetGuardPatterns_ = move(enabledPatterns);

    initialCell_ = server.pos_;
    VASSERT(initialCell_ == gs.ToId(N / 2, 0));

    aroundTarget_.assign(NN, false);
    for (int n : gs.around(server.target_)) {
        aroundTarget_[n] = true;
    }

    REP(pt, targetGuardPatterns_.size()) {
        auto& pattern = targetGuardPatterns_[pt];
        auto isTree = server.isTree_;
        for (int cell : pattern.wallCells_) {
            isTree[cell] = true;
        }

        dvec<int> outline;
        bool closed = false;
        {
            auto Rotate = [&](Dir d, int times) -> Dir { return Dir(((int)d + times) & 0b11); };

            isTree[pattern.innerCell_] = true;  

            REP(rot, 2) {
                Dir dir = gs.CalcDir(pattern.enterCell_, pattern.innerCell_);
                int backRot = rot == 0 ? 3 : 1;
                int stepDir = rot == 0 ? 1 : 3;
                dir = Rotate(dir, stepDir);

                int p = pattern.enterCell_;
                static dvec<int> path;
                path.clear();
                path.push(p);
                while (true) {
                    Dir nextDir = Rotate(dir, backRot);
                    bool finish = false;
                    bool moved = false;
                    REP(i, 4) {
                        int n = gs.dirs(p)[(int)nextDir];
                        if (n < 0) {
                            finish = true;
                            break;
                        }
                        if (!isTree[n]) {
                            path.push(n);
                            p = n;
                            dir = nextDir;
                            moved = true;
                            break;
                        }
                        nextDir = Rotate(nextDir, stepDir);
                    }
                    if (finish) {
                        break;
                    }
                    VASSERT(moved);
                    if (p == pattern.enterCell_) {
                        closed = true;
                        path.pop();
                        break;
                    }
                }

                if (closed) {
                    outline = path;
                    break;
                }

                if (rot == 1) {
                    reverse(ALL(outline));
                    FOR(i, 1, path.size()) {
                        outline.push(path[i]);
                    }
                }
                else {
                    outline = path;
                }
            }

            isTree[pattern.innerCell_] = false;  
        }

        {

            auto Detect = [&](int& idxA, int& idxB) -> bool {
                NNArr<int> lastVisit(NN, -1);
                REP(i, outline.size()) {
                    int cell = outline[i];
                    for (int n : gs.around(cell)) {
                        if (lastVisit[n] == i - 1) {
                            continue;
                        }
                        if (closed) {
                            if (lastVisit[n] == 0) {
                                continue;
                            }
                        }
                        if (lastVisit[n] >= 0) {
                            idxA = lastVisit[n];
                            idxB = i;
                            return true;
                        }
                    }
                    lastVisit[cell] = i;
                }
                return false;
            };

            auto Cut = [&](int idxA, int idxB) {
                VASSERT(idxA < idxB);
                if (closed) {
                    int sizeAB = idxB - idxA;
                    int sizeBA = outline.size() - sizeAB;
                    if (sizeAB <= sizeBA) {
                        dvec<int> newOutline;
                        REP(i, idxA + 1) {
                            newOutline.push(outline[i]);
                        }
                        FOR(i, idxB, outline.size()) {
                            newOutline.push(outline[i]);
                        }
                        outline = newOutline;
                    }
                    else {
                        dvec<int> newOutline;
                        FOR(i, idxA, idxB + 1) {
                            newOutline.push(outline[i]);
                        }
                        outline = newOutline;
                    }
                }
                else {
                    int sizeAB = idxB - idxA;

                    dvec<int> newOutline;
                    REP(i, idxA + 1) {
                        newOutline.push(outline[i]);
                    }
                    FOR(i, idxB, outline.size()) {
                        newOutline.push(outline[i]);
                    }
                    outline = newOutline;
                }
            };

            while (true) {
                int idxA = -1, idxB = -1;
                if (Detect(idxA, idxB)) {
                    Cut(idxA, idxB);
                }
                else {
                    break;
                }
            }

        }

        pattern.outline_ = outline;


        int tryIndex = 0;
        while (true) {
            NNCheckMap visible;
            for (int start : pattern.outline_) {
                visible.Check(start);

                REP(di, 4) {
                    int p = start;
                    REP(i, N) {
                        int n = gs.dirs(p)[di];
                        if (n < 0) {
                            break;
                        }
                        visible.Check(n);
                        if (isTree[n]) {
                            break;
                        }
                        p = n;
                    }
                }
            }

            NNCheckMap inner;
            int innerCount = 0;
            {
                NNQue<int> que;
                auto& check = inner;
                {
                    int start = server.target_;
                    que.push(start);
                    check.Check(start);
                    ++innerCount;
                }
                while (!que.empty()) {
                    int p = que.pop();
                    for (int n : gs.around(p)) {
                        if (visible[n]) {
                            continue;
                        }
                        if (check.Enter(n)) {
                            que.push(n);
                            ++innerCount;
                        }
                    }
                }
            }

            bool updatedWall = false;
            NNSet checkWalls;
            REP(i, NN) {
                if (inner[i]) {
                    for (int n : gs.around(i)) {
                        if (n == pattern.innerCell_) {
                            continue;
                        }
                        if (n == pattern.enterCell_) {
                            continue;
                        }

                        if (visible[n]) {
                            checkWalls.fadd(n);

                            if (!isTree[n]) {
                                isTree[n] = true;
                                pattern.wallCells_.push(n);
                                updatedWall = true;
                            }
                        }
                    }
                }
            }

            if (updatedWall) {
                continue;
            }

            if (pattern.blockCell_ >= 0) {
                if (IsNeedCheckWall(pattern.wallCells_, pattern.enterCell_, pattern.innerCell_, pattern.blockCell_)) {
                    checkWalls.fadd(pattern.blockCell_);
                }
            }


            pattern.deadCellCount_ = innerCount;
            VASSERT(pattern.deadCellCount_ >= 1);
            for (int w : checkWalls) {
                pattern.checkCells_.push(w);
            }

            break;
        }
    }



}

struct CountMap {
    NNArr<u8> mainCount_;  
    NNArr<u8> sumCount_;   

    void Init() {
        mainCount_.assign(NN, 0);
        sumCount_.assign(NN, 0);
    }

    void AddMainCount(int cell) {
        for (int n : gs.around(cell)) {
            VASSERT(mainCount_[n] < 255);
            VASSERT(sumCount_[n] < 255);
            ++mainCount_[n];
            ++sumCount_[n];
        }
    }
    void AddBranchCount(int cell) {
        for (int n : gs.around(cell)) {
            VASSERT(sumCount_[n] < 255);
            ++sumCount_[n];
        }
    }

    void RemoveMainCount(int cell) {
        for (int n : gs.around(cell)) {
            VASSERT(mainCount_[n] > 0);
            VASSERT(sumCount_[n] > 0);
            --mainCount_[n];
            --sumCount_[n];
        }
    }
    void RemoveBranchCount(int cell) {
        for (int n : gs.around(cell)) {
            VASSERT(sumCount_[n] > 0);
            --sumCount_[n];
        }
    }

    int GetMainCount(int cell) const {
        return mainCount_[cell];
    }
    int GetSumCount(int cell) const {
        return sumCount_[cell];
    }
};

enum class BranchType : u8 {
    Normal = 0,  
    Enter = 1,   
    Exit = 2,    
};

struct Branch {
    bool enable_;      
    BranchType type_;  
    dvec<int> path_;   

    int curveCount_;  

    int guardPatternIdx_;  
};

void MakeWallMap(const NNArr<bool>& isTree, CountMap countMap, const NNArr<bool>& isMoved, NNArr<bool>& isWall) {

    NNQue<int> que;
    NNCheckMap check;
    REP(start, NN) {
        if (isMoved[start]) {
            que.push(start);
            check.Check(start);
        }
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.around(p)) {
            if (isTree[n]) {
                continue;
            }
            if (countMap.GetSumCount(n) > 1) {
                continue;
            }
            if (check.Enter(n)) {
                countMap.AddBranchCount(n);
                que.push(n);
            }
        }
    }

    isWall = isTree;
    REP(i, NN) {
        if (!check.IsChecked(i)) {
            isWall[i] = true;
        }
    }
}


double Eval(const NNArr<bool>& newIsTree, const CountMap& countMap, const dvec<int>& mainPath,
            const dvec<dvec<int>>& branchMap, const dvec<Branch>& branchs, double timeRate) {

    NNArr<bool> isWall;
    {

        NNArr<bool> isMoved(NN, false);
        for (int p : mainPath) {
            isMoved[p] = true;
        }
        REP(bi, branchs.size()) {
            cauto& branch = branchs[bi];
            if (branch.enable_) {
                FOR(i, 1, branch.path_.size()) {
                    int p = branch.path_[i];
                    isMoved[p] = true;
                }
            }
        }
        MakeWallMap(newIsTree, countMap, isMoved, isWall);


    }

    int mainSplitIndex = -1;
    {
        REP(i, mainPath.size()) {
            int cell = mainPath[i];
            if (initialCell_ == cell) {
                mainSplitIndex = i;
                break;
            }
        }
        if (mainSplitIndex < 0) {
            int mainStartCell = -1;
            REP(bi, branchs.size()) {
                cauto& branch = branchs[bi];
                if (branch.enable_ && branch.path_.back() == initialCell_) {
                    mainStartCell = branch.path_.front();
                    break;
                }
            }
            VASSERT(mainStartCell >= 0);

            REP(i, mainPath.size()) {
                int cell = mainPath[i];
                if (cell == mainStartCell) {
                    mainSplitIndex = i;
                    break;
                }
            }
        }
        VASSERT(mainSplitIndex >= 0);
    }

    double mainScore = 0;
    mainScore += mainPath.size() * HP.MainPathLength;

    {
        int A = mainSplitIndex + 1;
        int B = mainPath.size() - mainSplitIndex;
        mainScore += HP.MainSplitPathLength * 2 * A * B / (double)((A + B));
    }


    NNArr<double> rateMap(NN, -1.0);
    REP(mi, mainPath.size()) {
        int p = mainPath[mi];
        if (mainPath.size() <= 1) {
            rateMap[p] = 0;
        }
        else {
            double r = mi / (double)(mainPath.size() - 1);
            r = abs((r - 0.5) * 2.0);
            rateMap[p] = r;
        }
    }



    double branchScore = 0;
    for (int mainDir : {-1, 1}) {
        static dvec<int> splitMainPath;
        static NNCheckMap isSplitMain;
        splitMainPath.clear();
        isSplitMain.Clear();
        if (mainDir == -1) {
            RREP(i, mainSplitIndex + 1) {
                splitMainPath.push(mainPath[i]);
                isSplitMain.Check(mainPath[i]);
            }
        }
        else {
            FOR(i, mainSplitIndex, mainPath.size()) {
                splitMainPath.push(mainPath[i]);
                isSplitMain.Check(mainPath[i]);
            }
        }



        NNArr<int> visibleTurnMap(NN, INT_MAX);
        NNArr<int> moveTurnMap(NN, INT_MAX);
        REP(pi, splitMainPath.size()) {
            moveTurnMap[splitMainPath[pi]] = pi;
            chmin(visibleTurnMap[splitMainPath[pi]], pi);

            REP(di, 4) {
                int p = splitMainPath[pi];
                REP(i, N) {
                    int n = gs.dirs(p)[di];
                    if (n < 0) {
                        continue;
                    }
                    chmin(visibleTurnMap[n], pi);

                    if (isWall[n]) {
                        break;
                    }
                    p = n;
                }
            }
        }

        auto CalcExitScore = [&](const Branch& branch) -> double {
            double score = 0;
            cauto& path = branch.path_;

            int patternIdx = branch.guardPatternIdx_;
            cauto& pattern = targetGuardPatterns_[patternIdx];

            bool isSafe = true;
            static NNCheckMap branchVisible;
            branchVisible.Clear();
            REP(pi, path.size()) {
                REP(di, 4) {
                    int p = path[pi];
                    REP(i, N) {
                        int n = gs.dirs(p)[di];
                        if (n < 0) {
                            break;
                        }
                        branchVisible.Check(n);
                        if (isWall[n]) {
                            break;
                        }
                        p = n;
                    }
                }
                if (path[pi] == pattern.enterCell_) {
                    break;
                }
            }

            int enterTurn = moveTurnMap[path[0]];
            VASSERT(enterTurn < INT_MAX);
            int deadCellCount = pattern.deadCellCount_;

            if (visibleTurnMap[path.back()] != INT_MAX) {
                isSafe = false;
            }
            else {
                if (useGuardPattern_) {
                    for (int cell : pattern.checkCells_) {
                        if (branchVisible[cell]) {
                            continue;
                        }
                        if (visibleTurnMap[cell] > enterTurn) {
                            isSafe = false;
                            break;
                        }
                    }
                }
                else {
                    REP(i, NN) {
                        if (visibleTurnMap[i] <= enterTurn) {
                            branchVisible.Check(i);
                        }
                    }
                    deadCellCount = CalcDeadCellCount(isWall, branchVisible, pattern.enterCell_, pattern.innerCell_);

                }
            }


            if (isSafe) {
                VASSERT(deadCellCount >= 1);
                score += NN * HP.SafeScore / (double)deadCellCount;
            }
            else {
                score -= NN * HP.ExitPenalty;
            }

            score += path.size() * HP.ExitLength;

            return score;
        };

        auto CalcBranchScore = [&](const Branch& branch) -> double {
            double score = 0;
            cauto& path = branch.path_;

            bool isSafe = true;
            {
                int curvePrevIndex = -1;
                Dir finalDir = gs.CalcDir(path[path.size() - 2], path.back());  
                RREP(i, path.size() - 2) {
                    if (gs.CalcDir(path[i], path[i + 1]) != finalDir) {
                        curvePrevIndex = i;
                        break;
                    }
                }
                VASSERT(curvePrevIndex >= 0);
                int curvePrevCell = path[curvePrevIndex];

                Dir enterDir = gs.CalcDir(path[curvePrevIndex], path[curvePrevIndex + 1]);
                Dir curveDir = finalDir;


                CapArr<int, 3> checkCells;
                checkCells.push(gs.CalcMovedCell(path[path.size() - 2], {Back(curveDir)}));
                checkCells.push(gs.CalcMovedCell(path.back(), {curveDir}));
                checkCells.push(gs.CalcMovedCell(path.back(), {enterDir}));

                static NNCheckMap branchVisible;
                branchVisible.Clear();
                REP(pi, path.size()) {
                    REP(di, 4) {
                        int p = path[pi];
                        REP(i, N) {
                            int n = gs.dirs(p)[di];
                            if (n < 0) {
                                break;
                            }
                            branchVisible.Check(n);
                            if (isWall[n]) {
                                break;
                            }
                            p = n;
                        }
                    }
                    if (path[pi] == curvePrevCell) {
                        break;
                    }
                }

                int enterTurn = moveTurnMap[path[0]];
                VASSERT(enterTurn < INT_MAX);

                if (visibleTurnMap[path.back()] != INT_MAX) {
                    isSafe = false;
                }
                else {
                    REP(i, checkCells.size()) {
                        int cell = checkCells[i];
                        if (cell < 0) {
                            continue;
                        }
                        if (visibleTurnMap[cell] > enterTurn) {
                            if (branchVisible[cell]) {
                                continue;
                            }
                            isSafe = false;
                            break;
                        }
                    }
                }
            }

            double rate = rateMap[path.front()];
            VASSERT(rate >= 0);

            double baseScore = HP.BranchBaseScore;
            if (isSafe) {
                baseScore *= HP.BranchSafeBonus;
            }
            score += baseScore * pow(rate, HP.BranchRateExp) * pow(branch.curveCount_, HP.BranchCurveExp);
            score += path.size() * HP.BranchLength;

            return score;
        };

        REP(bi, branchs.size()) {
            cauto& branch = branchs[bi];
            if (branch.enable_) {
                if (branch.type_ == BranchType::Enter) {
                    continue;
                }

                int startCell = branch.path_.front();
                if (!isSplitMain[startCell]) {
                    continue;
                }
                if (startCell == splitMainPath.front()) {
                    if (mainDir > 0) {
                        continue;
                    }
                }

                if (branch.type_ == BranchType::Exit) {
                    branchScore += CalcExitScore(branch);
                }
                else {
                    branchScore += CalcBranchScore(branch);
                }
            }
        }
    }


    DEF_DOUBLE_PARAM(MainScoreRate);
    DEF_DOUBLE_PARAM(BranchScoreRate);

    double score = mainScore * MainScoreRate + branchScore * BranchScoreRate;

    return score;
}


void CalcAroundPath(const NNArr<bool>& isTree, const dvec<int>& mainPath, int maxCut, const dvec<Branch>& branches,
                    dvec<int>& bestPath) {
    NNArr<int> countMap(NN, 0);
    REP(i, mainPath.size()) {
        for (int n : gs.around(mainPath[i])) {
            ++countMap[n];
        }
    }
    for (cauto& branch : branches) {
        if (branch.enable_) {
            FOR(i, 1, branch.path_.size()) {
                for (int n : gs.around(branch.path_[i])) {
                    ++countMap[n];
                }
            }
        }
    }

    bestPath = mainPath;

    constexpr int MaxLoop = 10000;
    int loopIndex = 0;

    dvec<int> path = mainPath;
    auto dfs = [&](auto&& dfs, int p, Dir dir) -> void {
        ++loopIndex;
        bool moved = false;
        array<int, 4> nds = {0, 1, 2, 3};
        Shuffle(ALL(nds), rand_);

        for (int nextDir : nds) {
            int n = gs.dirs(p)[(int)nextDir];
            if (n >= 0 && !isTree[n] && countMap[n] < 2) {
                path.push(n);
                for (int nn : gs.around(n)) {
                    ++countMap[nn];
                }
                dfs(dfs, n, Dir::L);
                if (loopIndex >= MaxLoop) {
                    return;
                }
                path.pop();
                for (int nn : gs.around(n)) {
                    --countMap[nn];
                }
                moved = true;
            }
        }
        if (!moved) {
            if (path.size() > bestPath.size()) {
                bestPath = path;
            }
            return;
        }
    };
    REP(i, maxCut + 1) {
        dfs(dfs, path.back(), gs.CalcDir(path[path.size() - 2], path.back()));
        if (i < maxCut) {
            for (int n : gs.around(path.back())) {
                --countMap[n];
            }
            path.pop();
        }
    }
}

void AppendInitialPath(const NNArr<bool>& srcTree, dvec<int>& mainPath, CountMap& countMap,
                       const dvec<Branch>& branches) {
    auto Close = [&](bool openFront, bool openBack, NNArr<bool>& isTree) {
        isTree = srcTree;

        int start = openFront ? 1 : 0;
        int end = openBack ? mainPath.size() - 1 : mainPath.size();

        FOR(i, start, end) {
            int cell = mainPath[i];
            for (int n : gs.around(cell)) {
                isTree[n] = true;
            }
        }
        for (cauto& branch : branches) {
            if (branch.enable_) {
                FOR(i, 1, branch.path_.size()) {
                    int cell = branch.path_[i];
                    for (int n : gs.around(cell)) {
                        isTree[n] = true;
                    }
                }
            }
        }
        if (openFront) {
            isTree[mainPath[0]] = false;
        }
        if (openBack) {
            isTree[mainPath.back()] = false;
        }
    };

    auto MakeFirstZone = [&](NNArr<bool>& firstZone) {
        firstZone.assign(NN, false);
        for (int path : mainPath) {
            for (int n : gs.around(path)) {
                firstZone[n] = true;
            }
        }
        for (int path : mainPath) {
            firstZone[path] = false;
        }

        for (cauto& branch : branches) {
            if (branch.enable_) {
                FOR(i, 1, branch.path_.size()) {
                    int cell = branch.path_[i];
                    for (int n : gs.around(cell)) {
                        firstZone[n] = false;
                    }
                }
            }
        }
    };

    auto ReverseOutline = [](dvec<int>& path) {
        VASSERT(path.front() == path.back());
        reverse(ALL(path));
    };

    auto CleanOutline = [&](dvec<int>& path) {
        auto backup = path;
        int srcCell = path.front();
        CleanPath(path, true);

        int missingSize = 0;
        REP(i, backup.size()) {
            if (path.front() == backup[i]) {
                break;
            }
            ++missingSize;
        }
        if (missingSize > 0) {
            backup.resize(missingSize);
            backup.extend(path);
            swap(path, backup);
        }



        static NNArr<int> indexMap;
        indexMap.assign(NN, -1);
        REP(i, path.size()) {
            int cell = path[i];

            for (int n : gs.around(cell)) {
                if (n < 0) {
                    continue;
                }
                if (indexMap[n] >= 0 && indexMap[n] != i - 1) {
                    path.resize(i);
                    return;
                }
            }

            indexMap[cell] = i;
        }
    };

    auto AppendFront = [&](const dvec<int>& path) {
        dvec<int> newPath;
        RFOR(i, 1, path.size()) {
            newPath.push(path[i]);
            countMap.AddMainCount(path[i]);
        }
        REP(i, mainPath.size()) {
            newPath.push(mainPath[i]);
        }
        mainPath = newPath;
    };
    auto AppendBack = [&](const dvec<int>& path) {
        FOR(i, 1, path.size()) {
            mainPath.push(path[i]);
            countMap.AddMainCount(path[i]);
        }
    };


    NNArr<bool> isTree;
    Close(false, true, isTree);

    int startCell = mainPath.back();
    Dir startWallDir = gs.CalcDir(mainPath.back(), mainPath[mainPath.size() - 2]);
    dvec<int> path;
    if (FindDivLine(isTree, startCell, startWallDir, path)) {
        AppendBack(path);


        {
            reverse(mainPath.begin(), mainPath.end());
            int maxCut = 0;

            CalcAroundPath(srcTree, mainPath, maxCut, branches, path);
            mainPath = path;

            reverse(mainPath.begin(), mainPath.end());


        }

        {
            int maxCut = 0;
            if (branches.size() > 0) {
                cauto& branch = branches[0];
                if (branch.enable_) {
                    int branchStart = branch.path_.front();
                    RREP(i, mainPath.size()) {
                        if (mainPath[i] == branchStart) {
                            maxCut = mainPath.size() - i - 1;
                            break;
                        }
                    }
                }
            }

            CalcAroundPath(srcTree, mainPath, maxCut, branches, path);
            mainPath = path;
        }


    }

    countMap.Init();
    REP(i, mainPath.size()) {
        countMap.AddMainCount(mainPath[i]);
    }
    for (cauto& branch : branches) {
        if (branch.enable_) {
            FOR(i, 1, branch.path_.size()) {
                countMap.AddBranchCount(branch.path_[i]);
            }
        }
    }

    return;
}

struct Pack {
    double evalScore_;
    dvec<int> mainPath_;
    CountMap countMap_;
    dvec<dvec<int>> branchMap_;  
    dvec<Branch> branches_;
    NNArr<bool> isTree_;  
};

struct State {
    double evalScore_;

    dvec<int> mainPath_;
    CountMap countMap_;          
    dvec<dvec<int>> branchMap_;  
    dvec<Branch> branches_;
    NNArr<bool> isTree_;  

    void Save(Pack& pack) const {
        pack.evalScore_ = evalScore_;
        pack.mainPath_ = mainPath_;
        pack.countMap_ = countMap_;
        pack.branchMap_ = branchMap_;
        pack.branches_ = branches_;
        pack.isTree_ = isTree_;
    }

    void Load(const Pack& pack) {
        evalScore_ = pack.evalScore_;
        mainPath_ = pack.mainPath_;
        countMap_ = pack.countMap_;
        branchMap_ = pack.branchMap_;
        branches_ = pack.branches_;
        isTree_ = pack.isTree_;
    }

    void Init() {
        mainPath_.clear();
        countMap_.Init();
        branchMap_.resize(NN);
        for (auto& bs : branchMap_) {
            bs.clear();
        }
        branches_.clear();

        isTree_ = server.isTree_;
        useGuardPattern_ = false;

        dvec<int> ptn(targetGuardPatterns_.size());
        iota(ALL(ptn), 0);
        Shuffle(ALL(ptn), rand_);
        sort(ALL(ptn), [](int l, int r) {
            return targetGuardPatterns_[l].deadCellCount_ < targetGuardPatterns_[r].deadCellCount_;
        });

        double bestEvalScore = -1e100;
        dvec<int> bestMainPath;
        CountMap bestCountMap;          
        dvec<dvec<int>> bestBranchMap;  
        dvec<Branch> bestBranches;
        NNArr<bool> bestIsTree;  

        bool found = false;
        for (int pt : ptn) {
            cauto& pattern = targetGuardPatterns_[pt];

            REP(rev, 2) {
                mainPath_.clear();
                countMap_.Init();
                branchMap_.resize(NN);
                for (auto& bs : branchMap_) {
                    bs.clear();
                }
                branches_.clear();

                dvec<int> outline = pattern.outline_;
                if (rev == 1) {
                    reverse(ALL(outline));
                }

                auto tmpTree = server.isTree_;
                for (int cell : pattern.wallCells_) {
                    tmpTree[cell] = true;
                }

                NNSet requireVisibles;
                for (int cell : pattern.checkCells_) {
                    requireVisibles.fadd(cell);
                }

                int connectIndex = -1;
                int connectCell = -1;
                REP(i, outline.size()) {
                    int cell = outline[i];
                    REP(di, 4) {
                        int p = cell;
                        REP(j, N) {
                            int n = gs.dirs(p)[di];
                            if (n < 0) {
                                break;
                            }
                            requireVisibles.fremove(n);
                            if (tmpTree[n]) {
                                break;
                            }
                            p = n;
                        }
                    }
                    if (requireVisibles.empty()) {
                        connectIndex = i;
                        connectCell = cell;
                        break;
                    }
                }
                if (connectIndex < 0) {
                    continue;
                }

                for (int cell : pattern.checkCells_) {
                    requireVisibles.fadd(cell);
                }
                int startIndex = -1;
                RREP(i, connectIndex + 1) {
                    int cell = outline[i];
                    REP(di, 4) {
                        int p = cell;
                        REP(j, N) {
                            int n = gs.dirs(p)[di];
                            if (n < 0) {
                                break;
                            }
                            requireVisibles.fremove(n);
                            if (tmpTree[n]) {
                                break;
                            }
                            p = n;
                        }
                    }
                    if (requireVisibles.empty()) {
                        startIndex = i;
                        break;
                    }
                }
                VASSERT(startIndex >= 0);




                {
                    static dvec<int> enterPath;
                    {
                        auto tmpTree2 = tmpTree;
                        FOR(i, startIndex + 1, connectIndex + 1) {
                            for (int cell : gs.around(outline[i])) {
                                tmpTree2[cell] = true;
                            }
                        }
                        FOR(i, startIndex + 1, connectIndex + 1) {
                            tmpTree2[outline[i]] = false;
                        }
                        if (!CalcPath(tmpTree2, server.target_, outline[startIndex], enterPath)) {
                            continue;
                        }
                    }

                    auto tmpTree2 = tmpTree;
                    FOR(i, startIndex, connectIndex) {
                        for (int cell : gs.around(outline[i])) {
                            tmpTree2[cell] = true;
                        }
                    }
                    for (int cell : enterPath) {
                        if (cell == connectCell) {
                            continue;
                        }
                        for (int n : gs.around(cell)) {
                            tmpTree2[n] = true;
                        }
                    }

                    FOR(i, startIndex, connectIndex + 1) {
                        tmpTree2[outline[i]] = false;
                    }
                    for (int cell : enterPath) {
                        if (cell == connectCell) {
                            continue;
                        }
                        tmpTree2[cell] = false;
                    }

                    if (!CalcPath(tmpTree2, initialCell_, connectCell, mainPath_)) {
                        continue;
                    }
                    RFOR(i, startIndex, connectIndex) {
                        mainPath_.push(outline[i]);
                    }
                }

                for (int cell : mainPath_) {
                    countMap_.AddMainCount(cell);
                }

                dvec<int> tmpPath;
                {
                    static NNCheckMap isMain;
                    static NNCheckMap isFixed;
                    isMain.Clear();
                    isFixed.Clear();
                    for (int cell : mainPath_) {
                        isMain.Check(cell);
                    }

                    REP(bi, branches_.size()) {
                        cauto& branch = branches_[bi];
                        if (branch.enable_) {
                            if (branch.path_.back() == initialCell_) {
                                FOR(i, 1, branch.path_.size()) {
                                    int cell = branch.path_[i];
                                    for (int n : gs.around(cell)) {
                                        if (!isMain[n]) {
                                            isFixed.Check(n);
                                        }
                                    }
                                }
                            }
                        }
                    }

                    NNQue<int> que;
                    NNCheckMap check;
                    NNArr<int> froms;
                    froms.assign(NN, -1);
                    int mainCell = -1;
                    {
                        int start = server.target_;
                        que.push(start);
                        check.Check(start);
                        froms[start] = -1;
                    }
                    while (!que.empty()) {
                        int p = que.pop();
                        for (int n : gs.around(p)) {
                            if (tmpTree[n] || isFixed[n]) {
                                continue;
                            }
                            if (!isMain[n] && countMap_.GetMainCount(n) >= 2) {
                                continue;
                            }

                            if (check.Enter(n)) {
                                que.push(n);
                                froms[n] = p;

                                if (isMain[n]) {
                                    que.clear();
                                    mainCell = n;
                                    break;
                                }
                            }
                        }
                    }
                    if (mainCell < 0) {
                        continue;
                    }

                    tmpPath.clear();
                    for (int p = mainCell; p >= 0; p = froms[p]) {
                        tmpPath.push(p);
                    }
                }





                branchMap_[tmpPath.front()].push(branches_.size());
                auto& branch = branches_.push();
                branch.enable_ = true;
                branch.type_ = BranchType::Exit;
                branch.path_ = tmpPath;
                branch.curveCount_ = -1;
                branch.guardPatternIdx_ = pt;

                FOR(i, 1, branch.path_.size()) {
                    int cell = branch.path_[i];
                    countMap_.AddBranchCount(cell);
                }



                isTree_ = server.isTree_;
                for (int cell : pattern.wallCells_) {
                    isTree_[cell] = true;
                }

                AppendInitialPath(isTree_, mainPath_, countMap_, branches_);

                useGuardPattern_ = true;
                evalScore_ = Eval(isTree_, countMap_, mainPath_, branchMap_, branches_, 0);
                Validate();
                found = true;

                if (evalScore_ > bestEvalScore) {
                    bestEvalScore = evalScore_;
                    bestMainPath = mainPath_;
                    bestCountMap = countMap_;
                    bestBranchMap = branchMap_;
                    bestBranches = branches_;
                    bestIsTree = isTree_;
                }
            }
        }
        if (found) {
            evalScore_ = bestEvalScore;
            mainPath_ = bestMainPath;
            countMap_ = bestCountMap;
            branchMap_ = bestBranchMap;
            branches_ = bestBranches;
            isTree_ = bestIsTree;
            useGuardPattern_ = true;
        }

        if (!useGuardPattern_) {
            found = false;
            REP(tryIndex, 100) {
                mainPath_.clear();
                countMap_.Init();
                branchMap_.resize(NN);
                for (auto& bs : branchMap_) {
                    bs.clear();
                }
                branches_.clear();

                auto tmpTree = server.isTree_;

                for (int cell : gs.around(server.target_)) {
                    tmpTree[cell] = true;
                }

                NNArr<bool> reachMap;
                MakeReachMap(tmpTree, server.pos_, reachMap);
                dvec<int> cells;
                REP(i, NN) {
                    if (reachMap[i]) {
                        cells.push(i);
                    }
                }
                int a = rand_(cells.size());
                int b = rand_(cells.size() - 1);
                if (b >= a) {
                    b++;
                }

                int startCell = cells[a];
                int endCell = cells[b];

                NNQue<int> que;
                NNArr<u8> dists;
                NNArr<int> froms;
                dists.assign(NN, 255);
                froms.assign(NN, -1);
                {
                    int start = startCell;
                    que.push(start);
                    dists[start] = 0;
                    froms[start] = -1;
                }
                while (!que.empty()) {
                    int p = que.pop();
                    for (int n : gs.around(p)) {
                        if (tmpTree[n]) {
                            continue;
                        }
                        int d = dists[p] + 1;
                        if (d < dists[n]) {
                            que.push(n);
                            dists[n] = d;
                            froms[n] = p;
                        }
                    }
                }
                VASSERT(froms[endCell] >= 0);

                int cur = endCell;
                while (cur >= 0) {
                    mainPath_.push(cur);
                    cur = froms[cur];
                }

                for (int cell : mainPath_) {
                    countMap_.AddMainCount(cell);
                }

                dvec<int> removedBis;
                NNArr<bool> tmpIsTree;
                bool ret = CalcEnterPath(tmpTree, mainPath_, removedBis, tmpIsTree);
                if (!ret) {
                    continue;
                }
                isTree_ = tmpIsTree;

                evalScore_ = Eval(isTree_, countMap_, mainPath_, branchMap_, branches_, 0);
                found = true;
                break;
            }
            VASSERT(found);
        }
    }

    double RawScore() const {
        return evalScore_;
    }

    double EvalScore() const {
        return evalScore_;
    }

    void RemoveBranch(int bi, bool removeMap) {
        auto& branch = branches_[bi];
        VASSERT(branch.enable_);

        FOR(i, 1, branch.path_.size()) {
            countMap_.RemoveBranchCount(branch.path_[i]);
        }
        branch.enable_ = false;

        if (removeMap) {
            int idx = branchMap_[branch.path_.front()].find(bi);
            VASSERT(idx >= 0);
            branchMap_[branch.path_.front()].remove(idx);
        }
    }

    void RestoreBranch(int bi, bool restoreMap) {
        auto& branch = branches_[bi];
        VASSERT(!branch.enable_);

        FOR(i, 1, branch.path_.size()) {
            countMap_.AddBranchCount(branch.path_[i]);
        }
        branch.enable_ = true;

        if (restoreMap) {
            branchMap_[branch.path_.front()].push(bi);
        }
    }

    void CompactBranches() {
        int bc = 0;
        REP(i, branches_.size()) {
            if (branches_[i].enable_) {
                int cell = branches_[i].path_.front();
                int idx = branchMap_[cell].find(i);
                VASSERT(idx >= 0);
                branchMap_[cell][idx] = bc;
                if (bc != i) {
                    swap(branches_[bc], branches_[i]);
                }
                ++bc;
            }
        }
        branches_.resize(bc);
    }

    void RemoveMainPath(int startMi, int endMi, dvec<int>& removedBis) {
        FOR(mi, startMi + 1, endMi) {
            countMap_.RemoveMainCount(mainPath_[mi]);

            for (int bi : branchMap_[mainPath_[mi]]) {
                removedBis.push(bi);
                RemoveBranch(bi, false);
            }
            branchMap_[mainPath_[mi]].clear();
        }
    }

    void RestoreMainPath(int startMi, int endMi, const dvec<int>& removedBis) {
        FOR(mi, startMi + 1, endMi) {
            countMap_.AddMainCount(mainPath_[mi]);
        }
        for (int bi : removedBis) {
            RestoreBranch(bi, true);
        }
    }

    bool CalcNewPathOld(const NNArr<bool>& isTree, int startMi, int endMi, int maxSize, dvec<int>& newPath,
                        dvec<int>& removedBis) {
        newPath.clear();

        int startP = startMi >= 0 ? mainPath_[startMi] : -1;
        int endP = endMi < mainPath_.size() ? mainPath_[endMi] : -1;
        if (startP < 0 && endP < 0) {
            return false;
        }

        if (startP < 0 || endP < 0) {
            int edgeStartCell = startP >= 0 ? startP : endP;
            int srcSize = endMi - startMi - 1;
            int minSize = srcSize / 2;
            int maxSize = srcSize * 2;
            int targetSize = minSize + rand_(maxSize - minSize + 1);

            auto dfs = [&](auto&& dfs, int p) -> bool {
                if (newPath.size() >= targetSize) {
                    return true;
                }

                auto nexts = gs.around(p);
                Shuffle(ALL(nexts), rand_);
                for (int n : nexts) {
                    if (isTree[n] || aroundTarget_[n]) {
                        continue;
                    }

                    if (countMap_.GetMainCount(n) != 1) {
                        continue;
                    }

                    countMap_.AddMainCount(n);
                    newPath.push(n);

                    if (dfs(dfs, n)) {
                        return true;
                    }

                    newPath.pop();
                    countMap_.RemoveMainCount(n);
                }
                return false;
            };
            if (!dfs(dfs, edgeStartCell)) {
                return false;
            }
            if (startP < 0) {
                reverse(ALL(newPath));
            }
        }
        else {
            NNArr<bool> isAroundEnd(NN, false);
            for (int n : gs.around(endP)) {
                isAroundEnd[n] = true;
            }

            auto dfs = [&](auto&& dfs, int p) -> bool {
                if (isAroundEnd[p]) {
                    return true;
                }

                auto nexts = gs.around(p);
                Shuffle(ALL(nexts), rand_);
                for (int n : nexts) {
                    if (isTree[n] || aroundTarget_[n]) {
                        continue;
                    }

                    if (isAroundEnd[n]) {
                        if (countMap_.GetMainCount(n) != 2) {
                            continue;
                        }
                    }
                    else {
                        if (countMap_.GetMainCount(n) != 1) {
                            continue;
                        }
                    }

                    int minSize = newPath.size() + 1 + gs.CalcL1Dist(n, endP);
                    if (minSize > maxSize) {
                        continue;
                    }

                    countMap_.AddMainCount(n);
                    newPath.push(n);

                    if (dfs(dfs, n)) {
                        return true;
                    }

                    newPath.pop();
                    countMap_.RemoveMainCount(n);
                }
                return false;
            };
            if (!dfs(dfs, mainPath_[startMi])) {
                return false;
            }
        }

        REP(bi, branches_.size()) {
            auto& branch = branches_[bi];
            if (branch.enable_) {
                bool invalid = false;
                FOR(i, 1, branch.path_.size() - 1) {
                    int cell = branch.path_[i];
                    if (countMap_.GetSumCount(cell) != 2) {
                        invalid = true;
                        break;
                    }
                }
                int cell = branch.path_.back();
                if (countMap_.GetSumCount(cell) != 1) {
                    invalid = true;
                }

                if (invalid) {
                    RemoveBranch(bi, true);
                    removedBis.push(bi);
                }
            }
        }

        return true;
    }

    bool CalcNewPath(const NNArr<bool>& isTree, int startMi, int endMi, int maxSize, dvec<int>& newPath,
                     dvec<int>& newMainPath, dvec<int>& removedBis) {
        if (!CalcNewPathOld(isTree, startMi, endMi, maxSize, newPath, removedBis)) {
            return false;
        }

        newMainPath.clear();
        {
            REP(pi, startMi + 1) {
                newMainPath.push(mainPath_[pi]);
            }
            newMainPath.extend(newPath);
            FOR(pi, endMi, mainPath_.size()) {
                newMainPath.push(mainPath_[pi]);
            }
        }

        return true;
    }

    bool CalcEnterPath(const NNArr<bool>& isTree, const dvec<int>& mainPath, dvec<int>& removedBis,
                       NNArr<bool>& newTree) {

        newTree = isTree;

        static NNCheckMap isMain;
        isMain.Clear();
        for (int cell : mainPath) {
            isMain.Check(cell);
        }
        bool connectedInitialCell = false;
        bool connectedTargetCell = false;
        REP(bi, branches_.size()) {
            auto& branch = branches_[bi];
            if (branch.enable_) {
                if (branch.type_ == BranchType::Enter || branch.type_ == BranchType::Exit) {
                    RemoveBranch(bi, true);
                    removedBis.push(bi);
                }
            }
        }

        if (isMain[initialCell_]) {
            connectedInitialCell = true;
        }


        static NNCheckMap aroundEnterPath;
        aroundEnterPath.Clear();

        int oldBranchCount = branches_.size();

        if (!connectedInitialCell) {
            if (!isMain[initialCell_] && countMap_.GetMainCount(initialCell_) >= 2) {
                return false;
            }

            NNQue<int> que;
            NNArr<int> dists;
            NNArr<int> froms;
            dists.assign(NN, INT_MAX);
            froms.assign(NN, -1);
            {
                int start = initialCell_;
                que.push(start);
                dists[start] = 0;
                froms[start] = -1;
            }
            int connectCell = -1;
            while (!que.empty()) {
                int p = que.pop();
                for (int n : gs.around(p)) {
                    if (isTree[n]) {
                        continue;
                    }
                    if (!isMain[n] && countMap_.GetMainCount(n) >= 2) {
                        continue;
                    }

                    int d = dists[p] + 1;
                    if (d < dists[n]) {
                        que.push(n);
                        dists[n] = d;
                        froms[n] = p;

                        if (isMain[n]) {
                            que.clear();
                            connectCell = n;
                            break;
                        }
                    }
                }
            }

            if (connectCell < 0) {
                return false;
            }

            static dvec<int> path;
            path.clear();
            int p = connectCell;
            while (p != -1) {
                path.push(p);
                p = froms[p];
            }

            int newBi = branches_.size();
            branchMap_[connectCell].push(newBi);
            auto& newBranch = branches_.push();
            newBranch.enable_ = true;
            newBranch.type_ = BranchType::Enter;
            newBranch.curveCount_ = -1;
            newBranch.guardPatternIdx_ = -1;
            swap(newBranch.path_, path);

            FOR(i, 1, newBranch.path_.size()) {
                countMap_.AddBranchCount(newBranch.path_[i]);
            }

            FOR(i, 1, newBranch.path_.size()) {
                int cell = newBranch.path_[i];
                for (int n : gs.around(cell)) {
                    aroundEnterPath.Check(n);
                }
            }
        }

        if (!connectedTargetCell && useGuardPattern_) {
            int enterBi = -1;
            static NNCheckMap isFixed;
            isFixed.Clear();

            REP(bi, branches_.size()) {
                cauto& branch = branches_[bi];
                if (branch.enable_) {
                    if (branch.path_.back() == initialCell_) {
                        enterBi = bi;
                        FOR(i, 1, branch.path_.size()) {
                            int cell = branch.path_[i];
                            for (int n : gs.around(cell)) {
                                if (!isMain[n]) {
                                    isFixed.Check(n);
                                }
                            }
                        }
                    }
                }
            }

            static dvec<int> ptns;
            ptns.resize(targetGuardPatterns_.size());
            iota(ALL(ptns), 0);
            Shuffle(ALL(ptns), rand_);

            bool found = false;
            for (int ptnIdx : ptns) {
                cauto& pattern = targetGuardPatterns_[ptnIdx];

                bool canWall = true;
                for (int cell : pattern.wallCells_) {
                    if (isMain[cell]) {
                        canWall = false;
                        break;
                    }
                }
                if (!canWall) {
                    continue;
                }

                auto tmpTree = server.isTree_;
                for (int cell : pattern.wallCells_) {
                    tmpTree[cell] = true;
                }

                {
                    NNQue<int> que;
                    NNCheckMap check;
                    NNArr<int> froms;
                    froms.assign(NN, -1);
                    int mainCell = -1;
                    {
                        int start = server.target_;
                        que.push(start);
                        check.Check(start);
                        froms[start] = -1;
                    }
                    while (!que.empty()) {
                        int p = que.pop();
                        for (int n : gs.around(p)) {
                            if (tmpTree[n] || isFixed[n]) {
                                continue;
                            }
                            if (!isMain[n] && countMap_.GetMainCount(n) >= 2) {
                                continue;
                            }

                            if (check.Enter(n)) {
                                que.push(n);
                                froms[n] = p;

                                if (isMain[n]) {
                                    que.clear();
                                    mainCell = n;
                                    break;
                                }
                            }
                        }
                    }
                    if (mainCell < 0) {
                        continue;
                    }

                    static dvec<int> path;
                    path.clear();
                    int p = mainCell;
                    while (p != -1) {
                        path.push(p);
                        p = froms[p];
                    }

                    int newBi = branches_.size();
                    branchMap_[path.front()].push(newBi);
                    auto& newBranch = branches_.push();
                    newBranch.enable_ = true;
                    newBranch.type_ = BranchType::Exit;
                    newBranch.curveCount_ = -1;
                    newBranch.guardPatternIdx_ = ptnIdx;
                    swap(newBranch.path_, path);

                    FOR(i, 1, newBranch.path_.size()) {
                        countMap_.AddBranchCount(newBranch.path_[i]);
                    }

                    FOR(i, 1, newBranch.path_.size()) {
                        int cell = newBranch.path_[i];
                        for (int n : gs.around(cell)) {
                            aroundEnterPath.Check(n);
                        }
                    }

                    newTree = tmpTree;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }

        REP(bi, oldBranchCount) {
            auto& branch = branches_[bi];
            if (branch.enable_) {
                bool deleted = false;
                FOR(i, 1, branch.path_.size()) {
                    int cell = branch.path_[i];
                    if (aroundEnterPath[cell]) {
                        RemoveBranch(bi, true);
                        removedBis.push(bi);
                        deleted = true;
                        break;
                    }
                }
                if (!deleted) {

                    FOR(i, 1, branch.path_.size()) {
                        int cell = branch.path_[i];
                        if (newTree[cell]) {
                            RemoveBranch(bi, true);
                            removedBis.push(bi);
                            break;
                        }
                    }
                }
            }
        }
        return true;
    }

    bool CalcNewBranch(const NNArr<bool>& isTree, int startCell) {
        static dvec<int> path;
        path.clear();


        constexpr int targetCurveCount = 3;
        int resultCurveCount = -1;
        auto dfs = [&](auto&& dfs, int p, Dir lastDir, int curveCount) -> bool {
            if (lastDir == Dir::Invalid) {
                for (int n : gs.around(p)) {
                    if (isTree[n]) {
                        continue;
                    }
                    if (countMap_.GetSumCount(n) != 1) {
                        continue;
                    }

                    countMap_.AddBranchCount(n);
                    path.push(n);

                    if (dfs(dfs, n, gs.CalcDir(p, n), curveCount)) {
                        return true;
                    }

                    path.pop();
                    countMap_.RemoveBranchCount(n);
                }
            }
            else {
                bool existCurve = false;
                array<Dir, 2> sideDirs = {RotateLeft(lastDir), RotateRight(lastDir)};
                for (Dir dir : sideDirs) {
                    int n = gs.dirs(p)[(int)dir];
                    if (n < 0 || isTree[n]) {
                        continue;
                    }
                    if (countMap_.GetSumCount(n) != 1) {
                        continue;
                    }

                    countMap_.AddBranchCount(n);
                    path.push(n);

                    if (curveCount + 1 >= targetCurveCount) {
                        resultCurveCount = curveCount + 1;
                        return true;
                    }

                    if (dfs(dfs, n, dir, curveCount + 1)) {
                        return true;
                    }

                    path.pop();
                    countMap_.RemoveBranchCount(n);
                    existCurve = true;
                }

                if (existCurve) {
                    for (Dir dir : sideDirs) {
                        int n = gs.dirs(p)[(int)dir];
                        if (n < 0 || isTree[n]) {
                            continue;
                        }
                        if (countMap_.GetSumCount(n) != 1) {
                            continue;
                        }

                        countMap_.AddBranchCount(n);
                        path.push(n);

                        resultCurveCount = curveCount + 1;
                        return true;
                    }
                    return false;
                }

                int n = gs.dirs(p)[(int)lastDir];
                if (n < 0 || isTree[n]) {
                    return false;
                }
                if (countMap_.GetSumCount(n) != 1) {
                    return false;
                }

                countMap_.AddBranchCount(n);
                path.push(n);

                if (dfs(dfs, n, lastDir, curveCount)) {
                    return true;
                }

                path.pop();
                countMap_.RemoveBranchCount(n);
            }
            return false;
        };

        path.push(startCell);
        if (dfs(dfs, startCell, Dir::Invalid, 0)) {
            branchMap_[startCell].push(branches_.size());

            auto& newBranch = branches_.push();
            newBranch.enable_ = true;
            newBranch.type_ = BranchType::Normal;
            newBranch.curveCount_ = resultCurveCount;
            newBranch.guardPatternIdx_ = -1;
            swap(newBranch.path_, path);
            return true;
        }
        return false;
    }

    SAResult Update(SAChecker& checker, double timeRate, int& transIdx) {

        static RandomTable tbl = [] {
            RandomTable tbl;
            tbl.push(0, 100);
            tbl.push(1, 10);
            tbl.push(2, 0);
            tbl.push(3, 10);
            return tbl;
        }();
        transIdx = tbl(rand_);

        if (transIdx == 0) {
            static const int SizeMin = HP.MainChangeRemoveSizeMin;
            static const int SizeMax = max(HP.MainChangeRemoveSizeMin, HP.MainChangeRemoveSizeMax);

            int size = SizeMin + rand_(SizeMax - SizeMin + 1);
            chmin(size, mainPath_.size());

            int startMi = rand_(mainPath_.size() - size + 2) - 1;
            int endMi = startMi + size;
            VASSERT(startMi >= -1);
            VASSERT(endMi <= mainPath_.size());

            int maxSize = int(size * HP.MainChangeNewMaxSize);

            int oldBranchCount = branches_.size();

            static dvec<int> removedBis;
            removedBis.clear();
            RemoveMainPath(startMi, endMi, removedBis);

            static dvec<int> newPath;
            static dvec<int> newMainPath;
            if (!CalcNewPath(isTree_, startMi, endMi, maxSize, newPath, newMainPath, removedBis)) {
                RestoreMainPath(startMi, endMi, removedBis);
                Validate();
                return SAResult::Fail;
            }

            NNArr<bool> newTree;
            if (!CalcEnterPath(isTree_, newMainPath, removedBis, newTree)) {
                for (int p : newPath) {
                    countMap_.RemoveMainCount(p);
                }

                FOR(br, oldBranchCount, branches_.size()) {
                    RemoveBranch(br, true);
                }
                branches_.resize(oldBranchCount);

                RestoreMainPath(startMi, endMi, removedBis);

                Validate();
                return SAResult::Fail;
            }

            REP(pi, newPath.size()) {
                int p = newPath[pi];
                if (CalcNewBranch(newTree, p)) {
                }
            }

            double newEvalScore = Eval(newTree, countMap_, newMainPath, branchMap_, branches_, timeRate);

            if (checker.CheckAccept(newEvalScore)) {
                evalScore_ = newEvalScore;

                swap(mainPath_, newMainPath);

                CompactBranches();

                isTree_ = newTree;

                Validate();
                return SAResult::Ok;
            }
            else {
                for (int p : newPath) {
                    countMap_.RemoveMainCount(p);
                }

                FOR(br, oldBranchCount, branches_.size()) {
                    RemoveBranch(br, true);
                }
                branches_.resize(oldBranchCount);

                RestoreMainPath(startMi, endMi, removedBis);

                Validate();
                return SAResult::Ng;
            }
        }

        else if (transIdx == 1) {
            int oldBranchCount = branches_.size();
            static dvec<int> removedBis;
            removedBis.clear();

            static NNArr<int> mainPathIndexMap;
            mainPathIndexMap.assign(NN, -1);
            REP(mi, mainPath_.size()) {
                mainPathIndexMap[mainPath_[mi]] = mi;
            }

            struct Connect {
                int edgeIdx;
                int connectMi;
                int newEdgeMi;
                int appendCell;
            };
            static dvec<Connect> connects;
            connects.clear();
            REP(edgeIdx, 2) {
                int edgeCell = (edgeIdx == 0 ? mainPath_.front() : mainPath_.back());
                REP(di, 4) {
                    int n = gs.dirs(edgeCell)[di];
                    if (n < 0) {
                        continue;
                    }
                    if (server.isTree_[n]) {
                        continue;
                    }
                    if (mainPathIndexMap[n] >= 0) {
                        continue;
                    }
                    array<Dir, 2> sideDirs = {RotateLeft((Dir)di), RotateRight((Dir)di)};
                    array<int, 2> sideCells = {gs.dirs(n)[(int)sideDirs[0]], gs.dirs(n)[(int)sideDirs[1]]};
                    bool sideOk = true;
                    for (int ceel : sideCells) {
                        if (ceel >= 0 && mainPathIndexMap[ceel] >= 0) {
                            sideOk = false;
                            break;
                        }
                    }
                    if (!sideOk) {
                        continue;
                    }

                    int nn = gs.dirs(n)[di];
                    if (nn < 0) {
                        continue;
                    }

                    if (mainPathIndexMap[nn] >= 0) {
                        int connectMi = mainPathIndexMap[nn];

                        if (edgeIdx == 0) {
                            int newEdgeMi = connectMi - 1;
                            auto& c = connects.pushR();
                            c.edgeIdx = edgeIdx;
                            c.connectMi = connectMi;
                            c.newEdgeMi = newEdgeMi;
                            c.appendCell = gs.dirs(edgeCell)[di];
                        }
                        else {
                            int newEdgeMi = connectMi + 1;
                            auto& c = connects.pushR();
                            c.edgeIdx = edgeIdx;
                            c.connectMi = connectMi;
                            c.newEdgeMi = newEdgeMi;
                            c.appendCell = gs.dirs(edgeCell)[di];
                        }
                    }
                }
            }

            if (connects.empty()) {
                return SAResult::Fail;
            }

            cauto& connect = connects.choice(rand_);

            static dvec<int> newPath;
            static dvec<int> newMainPath;
            static dvec<int> removedMainCells;
            static dvec<int> addedMainCells;
            newPath.clear();
            newMainPath.clear();
            removedMainCells.clear();
            addedMainCells.clear();
            if (connect.edgeIdx == 0) {
                removedMainCells.push(mainPath_[connect.connectMi - 1]);
                addedMainCells.push(connect.appendCell);

                RREP(i, connect.connectMi - 1) {
                    newPath.push(mainPath_[i]);
                }
                newPath.push(connect.appendCell);

                newMainPath = newPath;
                FOR(i, connect.connectMi, mainPath_.size()) {
                    newMainPath.push(mainPath_[i]);
                }
            }
            else {
                removedMainCells.push(mainPath_[connect.connectMi + 1]);
                addedMainCells.push(connect.appendCell);

                newPath.push(connect.appendCell);
                RFOR(i, connect.connectMi + 2, mainPath_.size()) {
                    newPath.push(mainPath_[i]);
                }

                REP(i, connect.connectMi + 1) {
                    newMainPath.push(mainPath_[i]);
                }
                REP(i, newPath.size()) {
                    newMainPath.push(newPath[i]);
                }
            }
            VASSERT(newMainPath.size() == mainPath_.size());

            for (int p : removedMainCells) {
                countMap_.RemoveMainCount(p);
            }
            for (int p : addedMainCells) {
                countMap_.AddMainCount(p);
            }

            {
                for (int removedCell : removedMainCells) {
                    for (int bi : branchMap_[removedCell]) {
                        cauto& branch = branches_[bi];
                        if (branch.enable_) {
                            RemoveBranch(bi, true);
                            removedBis.push(bi);
                        }
                    }
                }
            }
            REP(bi, branches_.size()) {
                auto& branch = branches_[bi];
                if (branch.enable_) {
                    bool invalid = false;
                    FOR(i, 1, branch.path_.size()) {
                        int cell = branch.path_[i];
                        if (countMap_.GetMainCount(cell) >= 2) {
                            invalid = true;
                            break;
                        }
                    }
                    if (invalid) {
                        RemoveBranch(bi, true);
                        removedBis.push(bi);
                    }
                }
            }

            NNArr<bool> newTree;
            if (!CalcEnterPath(isTree_, newMainPath, removedBis, newTree)) {
                for (int p : addedMainCells) {
                    countMap_.RemoveMainCount(p);
                }
                for (int p : removedMainCells) {
                    countMap_.AddMainCount(p);
                }

                FOR(br, oldBranchCount, branches_.size()) {
                    RemoveBranch(br, true);
                }
                branches_.resize(oldBranchCount);

                for (int bi : removedBis) {
                    RestoreBranch(bi, true);
                }

                Validate();
                return SAResult::Fail;
            }

            REP(pi, addedMainCells.size()) {
                int p = addedMainCells[pi];
                if (CalcNewBranch(newTree, p)) {
                }
            }

            double newEvalScore = Eval(newTree, countMap_, newMainPath, branchMap_, branches_, timeRate);

            if (checker.CheckAccept(newEvalScore)) {
                evalScore_ = newEvalScore;

                swap(mainPath_, newMainPath);

                CompactBranches();

                isTree_ = newTree;

                Validate();
                return SAResult::Ok;
            }
            else {
                for (int p : addedMainCells) {
                    countMap_.RemoveMainCount(p);
                }
                for (int p : removedMainCells) {
                    countMap_.AddMainCount(p);
                }

                FOR(br, oldBranchCount, branches_.size()) {
                    RemoveBranch(br, true);
                }
                branches_.resize(oldBranchCount);

                for (int bi : removedBis) {
                    RestoreBranch(bi, true);
                }

                Validate();
                return SAResult::Ng;
            }
        }

        else if (transIdx == 2) {




            int oldBranchCount = branches_.size();

            static dvec<int> removedBis;
            removedBis.clear();

            static dvec<int> bis;
            bis.clear();
            REP(bi, branches_.size()) {
                cauto& branch = branches_[bi];
                if (branch.enable_) {
                    if (branch.type_ == BranchType::Enter || branch.type_ == BranchType::Exit) {
                        continue;
                    }
                    bis.push(bi);
                }
            }
            if (bis.empty()) {
                return SAResult::Fail;
            }

            int removeCount = 1 + rand_(min(5, bis.size()));
            Shuffle(ALL(bis), rand_);
            REP(i, removeCount) {
                int br = bis[i];
                RemoveBranch(br, true);
                removedBis.push(br);
            }

            static dvec<int> shufflePath;
            shufflePath = mainPath_;
            Shuffle(ALL(shufflePath), rand_);

            for (int p : shufflePath) {
                if (CalcNewBranch(isTree_, p)) {
                }
            }

            double newEvalScore = Eval(isTree_, countMap_, mainPath_, branchMap_, branches_, timeRate);

            if (checker.CheckAccept(newEvalScore)) {
                evalScore_ = newEvalScore;

                CompactBranches();
                Validate();
                return SAResult::Ok;
            }
            else {
                FOR(br, oldBranchCount, branches_.size()) {
                    RemoveBranch(br, true);
                }
                branches_.resize(oldBranchCount);
                for (int bi : removedBis) {
                    RestoreBranch(bi, true);
                }
                Validate();
                return SAResult::Ng;
            }
        }

        else if (transIdx == 3) {
            int oldBranchCount = branches_.size();

            static dvec<int> removedBis;
            removedBis.clear();

            cauto& mainPath = mainPath_;
            cauto& branches = branches_;

            NNArr<bool> isWall;
            {
                NNArr<bool> isMoved(NN, false);
                for (int p : mainPath_) {
                    isMoved[p] = true;
                }
                REP(bi, branches_.size()) {
                    cauto& branch = branches_[bi];
                    if (branch.enable_) {
                        FOR(i, 1, branch.path_.size()) {
                            int p = branch.path_[i];
                            isMoved[p] = true;
                        }
                    }
                }
                MakeWallMap(isTree_, countMap_, isMoved, isWall);
            }

            int mainSplitIndex = -1;
            {
                REP(i, mainPath_.size()) {
                    int cell = mainPath_[i];
                    if (initialCell_ == cell) {
                        mainSplitIndex = i;
                        break;
                    }
                }
                if (mainSplitIndex < 0) {
                    int mainStartCell = -1;
                    REP(bi, branches_.size()) {
                        cauto& branch = branches_[bi];
                        if (branch.enable_ && branch.path_.back() == initialCell_) {
                            mainStartCell = branch.path_.front();
                            break;
                        }
                    }
                    VASSERT(mainStartCell >= 0);

                    REP(i, mainPath_.size()) {
                        int cell = mainPath_[i];
                        if (cell == mainStartCell) {
                            mainSplitIndex = i;
                            break;
                        }
                    }
                }
                VASSERT(mainSplitIndex >= 0);
            }

            double branchScore = 0;
            for (int mainDir : {-1, 1}) {
                static dvec<int> splitMainPath;
                static NNCheckMap isSplitMain;
                splitMainPath.clear();
                isSplitMain.Clear();
                if (mainDir == -1) {
                    RREP(i, mainSplitIndex + 1) {
                        splitMainPath.push(mainPath[i]);
                        isSplitMain.Check(mainPath[i]);
                    }
                }
                else {
                    FOR(i, mainSplitIndex, mainPath.size()) {
                        splitMainPath.push(mainPath[i]);
                        isSplitMain.Check(mainPath[i]);
                    }
                }

                NNArr<int> visibleTurnMap(NN, INT_MAX);
                NNArr<int> moveTurnMap(NN, INT_MAX);
                REP(pi, splitMainPath.size()) {
                    moveTurnMap[splitMainPath[pi]] = pi;
                    chmin(visibleTurnMap[splitMainPath[pi]], pi);

                    REP(di, 4) {
                        int p = splitMainPath[pi];
                        REP(i, N) {
                            int n = gs.dirs(p)[di];
                            if (n < 0) {
                                continue;
                            }
                            chmin(visibleTurnMap[n], pi);

                            if (isWall[n]) {
                                break;
                            }
                            p = n;
                        }
                    }
                }

                auto IsSaveBranch = [&](const Branch& branch) -> double {
                    cauto& path = branch.path_;

                    bool isSafe = true;
                    {
                        int curvePrevIndex = -1;
                        Dir finalDir = gs.CalcDir(path[path.size() - 2], path.back());  
                        RREP(i, path.size() - 2) {
                            if (gs.CalcDir(path[i], path[i + 1]) != finalDir) {
                                curvePrevIndex = i;
                                break;
                            }
                        }
                        VASSERT(curvePrevIndex >= 0);
                        int curvePrevCell = path[curvePrevIndex];

                        Dir enterDir = gs.CalcDir(path[curvePrevIndex], path[curvePrevIndex + 1]);
                        Dir curveDir = finalDir;


                        CapArr<int, 3> checkCells;
                        checkCells.push(gs.CalcMovedCell(path[path.size() - 2], {Back(curveDir)}));
                        checkCells.push(gs.CalcMovedCell(path.back(), {curveDir}));
                        checkCells.push(gs.CalcMovedCell(path.back(), {enterDir}));

                        static NNCheckMap branchVisible;
                        branchVisible.Clear();
                        REP(pi, path.size()) {
                            REP(di, 4) {
                                int p = path[pi];
                                REP(i, N) {
                                    int n = gs.dirs(p)[di];
                                    if (n < 0) {
                                        break;
                                    }
                                    branchVisible.Check(n);
                                    if (isWall[n]) {
                                        break;
                                    }
                                    p = n;
                                }
                            }
                            if (path[pi] == curvePrevCell) {
                                break;
                            }
                        }

                        int enterTurn = moveTurnMap[path[0]];
                        VASSERT(enterTurn < INT_MAX);

                        if (visibleTurnMap[path.back()] != INT_MAX) {
                            isSafe = false;
                        }
                        else {
                            REP(i, checkCells.size()) {
                                int cell = checkCells[i];
                                if (cell < 0) {
                                    continue;
                                }
                                if (visibleTurnMap[cell] > enterTurn) {
                                    if (branchVisible[cell]) {
                                        continue;
                                    }
                                    isSafe = false;
                                    break;
                                }
                            }
                        }
                    }
                    return isSafe;
                };

                REP(bi, branches.size()) {
                    cauto& branch = branches[bi];
                    if (branch.enable_) {
                        if (branch.type_ == BranchType::Enter || branch.type_ == BranchType::Exit) {
                            continue;
                        }

                        int startCell = branch.path_.front();
                        if (!isSplitMain[startCell]) {
                            continue;
                        }
                        if (startCell == splitMainPath.front()) {
                            if (mainDir > 0) {
                                continue;
                            }
                        }

                        bool isSafe = IsSaveBranch(branch);
                        if (!isSafe) {
                            RemoveBranch(bi, true);
                            removedBis.push(bi);
                        }
                    }
                }
            }

            static dvec<int> shufflePath;
            shufflePath = mainPath_;
            Shuffle(ALL(shufflePath), rand_);

            for (int p : shufflePath) {
                if (CalcNewBranch(isTree_, p)) {
                }
            }

            double newEvalScore = Eval(isTree_, countMap_, mainPath_, branchMap_, branches_, timeRate);

            if (checker.CheckAccept(newEvalScore)) {
                evalScore_ = newEvalScore;

                CompactBranches();
                Validate();
                return SAResult::Ok;
            }
            else {
                FOR(br, oldBranchCount, branches_.size()) {
                    RemoveBranch(br, true);
                }
                branches_.resize(oldBranchCount);
                for (int bi : removedBis) {
                    RestoreBranch(bi, true);
                }
                Validate();
                return SAResult::Ng;
            }
        }

        VABORT();
        return SAResult::Fail;
    }

    void Validate() {
        return;

        REP(i, mainPath_.size()) {
            int p = mainPath_[i];
            VASSERT(!isTree_[p]);
        }

        FOR(i, 1, mainPath_.size()) {
            int p = mainPath_[i - 1];
            int n = mainPath_[i];
            bool found = false;
            for (int nn : gs.around(p)) {
                if (nn == n) {
                    found = true;
                    break;
                }
            }
            VASSERT(found);
        }

        NNArr<bool> onMainPath(NN, false);
        for (int cell : mainPath_) {
            onMainPath[cell] = true;
        }

        NNArr<u8> checkMainCountMap;
        NNArr<u8> checkSumCountMap;
        checkMainCountMap.assign(NN, 0);
        checkSumCountMap.assign(NN, 0);
        for (int cell : mainPath_) {
            for (int n : gs.around(cell)) {
                checkMainCountMap[n]++;
                checkSumCountMap[n]++;
            }
        }

        REP(bi, branches_.size()) {
            cauto& branch = branches_[bi];
            VASSERT(branch.enable_);
            int startCell = branch.path_.front();
            int idx = branchMap_[startCell].find(bi);
            VASSERT(idx >= 0);

            if (branch.type_ == BranchType::Enter) {
                VASSERT(branch.path_.back() == initialCell_);
                VASSERT(branch.curveCount_ == -1);
                VASSERT(branch.guardPatternIdx_ == -1);
            }
            else if (branch.type_ == BranchType::Exit) {
                VASSERT(branch.path_.back() == server.target_);
                VASSERT(branch.curveCount_ == -1);
                VASSERT(branch.guardPatternIdx_ >= 0);
            }
            else {
                VASSERT(branch.type_ == BranchType::Normal);
                VASSERT(branch.path_.back() != initialCell_);
                VASSERT(branch.path_.back() != server.target_);
                VASSERT(branch.curveCount_ >= 1);
                VASSERT(branch.guardPatternIdx_ == -1);
            }

            FOR(i, 1, branch.path_.size()) {
                int p = branch.path_[i - 1];
                int n = branch.path_[i];
                bool found = false;
                for (int nn : gs.around(p)) {
                    if (nn == n) {
                        found = true;
                        break;
                    }
                }
                VASSERT(found);
            }

            FOR(i, 1, branch.path_.size()) {
                for (int n : gs.around(branch.path_[i])) {
                    checkSumCountMap[n]++;
                }
            }
            for (int cell : branch.path_) {
                VASSERT(!isTree_[cell]);
            }
        }

        REP(i, NN) {
            VASSERT(checkMainCountMap[i] == countMap_.mainCount_[i]);
            VASSERT(checkSumCountMap[i] == countMap_.sumCount_[i]);
        }
    }
};
static constexpr int StateSize = sizeof(State);

void Solve(int timeLimitUs, dvec<int>& pattern) {
    {
        Ready();
    }

    dvec<State> states(HP.InitialStateCount);
    for (auto& state : states) {
        state.Init();
    }


    SA<Pack, decltype(states[0].RawScore())> sa;
    Config config;

    config.update_tmp_mask = (1 << HP.UpdateTmpMaskBit) - 1;

    config.temp_type = HP.TempType;
    config.temp_pow = HP.TempPow;

    config.rollback_start_time_rate = HP.RollbackStartRate;
    config.rollback_count = HP.RollbackCount;
    config.rollback_mul = HP.RollbackNextMulti;
    config.min_rollback_count = HP.MinRollbackCount;

    config.filter_start_time = 0.2;
    config.filter_end_time = 1.0;

    if (!useGuardPattern_) {
        timeLimitUs = int(timeLimitUs * 0.8);
    }
    sa.Run<State>(states, timeLimitUs, HP.StartTemp, HP.EndTemp, config);

    cauto& best = sa.GetBest();
    cauto& bestPack = best.best_pack;
    cauto& path = bestPack.mainPath_;

    NNArr<bool> moved(NN, false);
    for (int cell : path) {
        moved[cell] = true;
    }
    for (cauto& branch : bestPack.branches_) {
        for (int cell : branch.path_) {
            moved[cell] = true;
        }
    }
    NNArr<bool> isWall;
    MakeWallMap(bestPack.isTree_, bestPack.countMap_, moved, isWall);

    if (!useGuardPattern_) {
        NNArr<bool> reachable;
        MakeReachMap(isWall, initialCell_, reachable);

        isWall[server.target_] = false;

        REP(openDir, 4) {
            int openCell = gs.dirs(server.target_)[(int)openDir];
            if (openCell < 0 || server.isTree_[openCell]) {
                continue;
            }
            auto tmpTree = server.isTree_;
            for (int n : gs.around(server.target_)) {
                if (n != openCell) {
                    tmpTree[n] = true;
                }
            }
            dvec<int> path;
            if (CalcPath(tmpTree, openCell, initialCell_, path)) {
                for (int p : path) {
                    if (reachable[p]) {
                        break;
                    }
                    isWall[p] = false;
                }
            }
        }
    }

    REP(i, NN) {
        if (!server.isTree_[i] && isWall[i]) {
            pattern.push(i);
        }
    }



}
}  

struct Solver {
    void Run() {
        dvec<int> pattern;

        sap::Solve(TIME_LIMIT_US, pattern);

        if (sap::useGuardPattern_) {
            server.Output(pattern);
            server.Input();
            server.OutputSkip();
        }
        else {
            FixPatternRun(pattern);
        }
    }

    void FixPatternRun(const dvec<int>& pattern) {
        dvec<int> result;
        REP(turn, 100000) {
            server.Input();
            if (server.IsEnd()) {
                break;
            }
            result.clear();

            if (turn == 0) {
                result = pattern;
            }

            else {
                if (timer_.IsOver()) {
                    auto tmpTree = server.isTree_;
                    for (int n : gs.around(server.target_)) {
                        if (!tmpTree[n] && !server.isSeen_[n]) {
                            auto isTree = tmpTree;
                            isTree[n] = true;
                            if (IsReachable(isTree, server.pos_, server.target_)) {
                                result.push(n);
                                tmpTree[n] = true;
                            }
                        }
                    }
                    server.Output(result);
                    server.Input();
                    break;
                }
                else {
                    bool requireCheck = false;
                    {
                        cauto& p = gs.ToPos(server.pos_);
                        for (int n : gs.around(server.target_)) {
                            cauto& np = gs.ToPos(n);
                            if (p.x == np.x || p.y == np.y) {
                                requireCheck = true;
                                break;
                            }
                        }
                    }
                    if (requireCheck) {
                        static NNCheckMap visible;
                        visible.Clear();

                        REP(di, 4) {
                            int p = server.pos_;
                            REP(i, N) {
                                int n = gs.dirs(p)[di];
                                if (n < 0) {
                                    break;
                                }
                                visible.Check(n);
                                if (server.isTree_[n]) {
                                    break;
                                }
                                if (n == server.target_) {
                                    break;
                                }
                                p = n;
                            }
                        }

                        auto tmpTree = server.isTree_;
                        for (int n : gs.around(server.target_)) {
                            if (!tmpTree[n] && visible[n] && !server.isSeen_[n]) {
                                auto isTree = tmpTree;
                                isTree[n] = true;
                                if (IsReachable(isTree, server.pos_, server.target_)) {
                                    result.push(n);
                                    tmpTree[n] = true;
                                }
                            }
                        }
                    }
                }
            }

            server.Output(result);
        }
        if (!server.IsEnd()) {
            server.OutputSkip();
        }
    }

    void FixPattern() {
        dvec<int> result;
        vector<vector<int>> pattern = {
            {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
            {0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1},
            {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
            {0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
            {1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1},
            {1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0},
            {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
            {0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0},
            {1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
            {0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0},
            {0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1},
            {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
            {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0},
            {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},

        };

        REP(turn, 10000) {
            server.Input();
            if (server.IsEnd()) {
                break;
            }
            result.clear();

            if (turn == 0) {
                REP(i, NN) {
                    pint p = gs.ToPos(i);
                    if (pattern[p.y][p.x] != 0 && server.isTree_[i] == false) {
                        result.push(i);
                    }
                }
            }
            server.Output(result);
        }
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
