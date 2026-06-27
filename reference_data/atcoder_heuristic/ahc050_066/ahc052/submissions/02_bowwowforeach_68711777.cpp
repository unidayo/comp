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

constexpr int N = 30;
constexpr int NN = N * N;
constexpr int M = 10;
constexpr int K = 10;

DEF_CONTAINERS(N, N);
DEF_CONTAINERS(NN, NN);
DEF_CONTAINERS(M, M);
DEF_CONTAINERS(K, K);

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

    START_TUNING;
    PD(PaintRate, 5.16019353286149, 0.0, 100.0);
    END_TUNING;


    PD(StartTemp, 1000.0, 0.0, 0.0);
    PD(EndTemp, 0.0, 0.0, 0.0);
    PC(TempType, 0, 0, 1, 2);
    PD(TempPow, 1.5, 1.0, 2.0, 1.0);

    PI(InitialStateCount, 1, 1, 8, 1);

    PD(RollbackStartRate, 0, 0.0, 0.15, 0.0, 1.0);
    PI(RollbackCount, 100000, 100, 100000, 1);
    PD(RollbackNextMulti, 1.0, 0.9, 1.1, 0.1);
    PI(MinRollbackCount, 100, 100, 300, 1);

    PI(UpdateTmpMaskBit, 4, 0, 10, 0, 16);


} HP;




#undef PC
#undef PI
#undef PD


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

template <int W, int H>
struct GridSystemS {
   private:
    pint toPos_[W * H];

   public:
    constexpr GridSystemS() : toPos_{} {
        REP(y, H) {
            REP(x, W) {
                int id = x + W * y;
                toPos_[id] = pint{x, y};
            }
        }
    }

    inline constexpr int ToId(int x, int y) const {
        return x + W * y;
    }
    inline constexpr int ToId(const pint& p) const {
        return p.x + W * p.y;
    }
    inline constexpr const pint& ToPos(int id) const {
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
    inline bool IsIn(const pint& p) const {
        return !IsOut(p);
    }
    inline bool IsIn(int x, int y) const {
        return !IsOut(x, y);
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
};


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

template <int W, int H, int AROUND_COUNT>
struct AroundMapS {
    using CA = CCA<int, AROUND_COUNT>;
    CA table[W * H];

    constexpr AroundMapS(const array<pint, AROUND_COUNT>& aroundDirs) : table{} {
        REP(cellId, W * H) {
            pint p = { cellId % W, cellId / W };
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                if (x >= 0 && x < W &&
                    y >= 0 && y < H) {
                    table[cellId].push(x + y * W);
                }
            }
        }
    }

    inline constexpr const CA& operator ()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator [](int id) const {
        return table[id];
    }
};

template <int W, int H, int AROUND_COUNT>
struct DirMapS {
    using CA = CCA<int, AROUND_COUNT>;
    CA table[W * H];

    constexpr DirMapS(const array<pint, AROUND_COUNT>& aroundDirs) : table{} {
        REP(cellId, W * H) {
            pint p = { cellId % W, cellId / W };
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                int n = -1;
                if (x >= 0 && x < W &&
                    y >= 0 && y < H) {
                    n = x + y * W;
                }
                table[cellId].push(n);
            }
        }
    }

    inline constexpr const CA& operator ()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator [](int id) const {
        return table[id];
    }
};

template <int W, int H>
struct JointAroundMapS {
    using CA = CCA<int, 8>;
    CA table[W * H];

    constexpr JointAroundMapS() : table{} {
        REP(cellId, W * H) {
            pint p = { cellId % W, cellId / W };
            FOR(oy, -1, 2) {
                FOR(ox, -1, 2) {
                    if (oy == 0 && ox == 0) {
                        continue;
                    }
                    int x = p.a + ox;
                    int y = p.b + oy;
                    int n = -1;
                    if (x >= 0 && x < W &&
                        y >= 0 && y < H) {
                        n = x + y * W;
                    }
                    table[cellId].push(n);
                }
            }
        }
    }

    inline constexpr const CA& operator ()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator [](int id) const {
        return table[id];
    }
};


template <int W, int H>
struct RandAroundMapS {
    using CA = CCA<int, 4>;
    CapArr<CA, 24> table[W * H];

    constexpr RandAroundMapS() : table{} {
        REP(cellId, W * H) {
            pint p = { cellId % W, cellId / W };
            CA tmp{};
            for (const pint& a : Around4) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                if (x >= 0 && x < W &&
                    y >= 0 && y < H) {
                    tmp.push(x + y * W);
                }
            }

            CapArr<int, 4> idxs;
            REP(i, tmp.size()) {
                idxs.push(i);
            }
            do {
                auto& t = table[cellId].push();
                for (int i : idxs) {
                    t.push(tmp[i]);
                }
            } while (next_permutation(ALL(idxs)));
        }
    }

    template <class RAND>
    inline constexpr const CA& operator ()(int p, RAND& r) const {
        auto& tbl = table[p];
        return tbl[r(tbl.size())];
    }
};
GridSystemS<N, N> gs;
AroundMapS<N, N, 4> aroundMap(Around4);
DirMapS<N, N, 4> dirMap(Around4);


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

struct IOResult {
    KBox<MBox<Dir>> buttonsRobotsDir_;  
    dvec<u8> buttons_;

    void Init() {
        REP(ki, K) {
            REP(mi, M) {
                buttonsRobotsDir_[ki][mi] = Dir::N;
            }
        }
        buttons_.clear();
    }

    void Output(ostream& os) const {
        REP(ki, K) {
            REP(mi, M) {
                if (buttonsRobotsDir_[ki][mi] == Dir::N) {
                    os << "S" << " ";
                }
                else {
                    os << DirToChar(buttonsRobotsDir_[ki][mi]) << " ";
                }
            }
            os << endl;
        }
        REP(i, buttons_.size()) {
            os << (int)buttons_[i] << endl;
        }
    }
};

struct IOServer {

    MBox<int> start_;  
    NNBox<bool> isRightWall_;
    NNBox<bool> isDownWall_;

    bool CanMove(int from, int to) const {
        Dir dir = gs.CalcDir(from, to);
        if (dir == Dir::L) {
            return !isRightWall_[to];
        }
        else if (dir == Dir::R) {
            return !isRightWall_[from];
        }
        else if (dir == Dir::U) {
            return !isDownWall_[to];
        }
        else if (dir == Dir::D) {
            return !isDownWall_[from];
        }
        else {
            VABORT();
            return false;
        }
    }

    int CalcNextCell(int curCell, Dir dir) const {
        if (dir == Dir::N) {
            return curCell;
        }
        else {
            int nextCell = dirMap[curCell][(int)dir];
            if (nextCell >= 0 && CanMove(curCell, nextCell)) {
                return nextCell;
            }
        }
        return curCell;
    }

    int CalcScore(const IOResult& result) const {
        NNBox<bool> painted = {};
        MBox<int> curCells = start_;
        REP(mi, M) {
            painted[curCells[mi]] = true;
        }
        REP(i, result.buttons_.size()) {
            int ki = result.buttons_[i];
            REP(mi, M) {
                Dir dir = result.buttonsRobotsDir_[ki][mi];
                int nextCell = CalcNextCell(curCells[mi], dir);
                curCells[mi] = nextCell;
                painted[nextCell] = true;
            }
        }

        int score = 0;
        int paintCount = 0;
        REP(i, NN) {
            if (painted[i]) {
                ++paintCount;
            }
        }
        if (paintCount == NN) {
            return score = 3 * NN - result.buttons_.size();
        }
        else {
            return paintCount;
        }
    }

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        int dummy;
        is >> dummy;
        timer.Init();  
        is >> dummy >> dummy;

        int x, y;
        REP(i, M) {
            is >> y >> x;
            int id = gs.ToId(x, y);
            start_[i] = id;
        }

        isRightWall_.fill(false);
        isDownWall_.fill(false);

        char c;
        REP(y, N) {
            REP(x, N - 1) {
                is >> c;
                if (c == '1') {
                    isRightWall_[gs.ToId(x, y)] = true;
                }
            }
        }
        REP(y, N - 1) {
            REP(x, N) {
                is >> c;
                if (c == '1') {
                    isDownWall_[gs.ToId(x, y)] = true;
                }
            }
        }
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


constexpr double ToRad(double deg) {
	return deg * M_PI / 180.0;
}
constexpr double ToDeg(double rad) {
	return rad * 180.0 / M_PI;
}

double sin_deg(double deg) {
	return sin(ToRad(deg));
}
double cos_deg(double deg) {
	return cos(ToRad(deg));
}
double tan_deg(double deg) {
	return tan(ToRad(deg));
}

MBox<NNBox<int>> misStartDists_;

dvec<dvec<int>> misStartCells_;

void Ready() {
    REP(mi, M) {
        NNQue<int> que;
        auto& dists = misStartDists_[mi];
        dists.fill(INT_MAX);
        {
            int start = server.start_[mi];
            que.push(start);
            dists[start] = 0;
        }
        while (!que.empty()) {
            int p = que.pop();
            for (int n : aroundMap[p]) {
                if (!server.CanMove(p, n)) {
                    continue;
                }
                int d = dists[p] + 1;
                if (d < dists[n]) {
                    que.push(n);
                    dists[n] = d;
                }
            }
        }
    }

    misStartCells_.resizeR(M);

    REP(cell, NN) {
        dvec<int> mis(M);
        iota(ALL(mis), 0);
        sort(ALL(mis), [&](int a, int b) { return misStartDists_[a][cell] < misStartDists_[b][cell]; });
        REP(i, 3) {
            int mi = mis[i];
            misStartCells_[mi].push(cell);
        }
    }
}

struct Solver {
    void Run() {
        Ready();

        struct Cand {
            int vertMove;
            int sideMove;
            int moveTurn;
        };
        static dvec<Cand> cands;
        for (int vertMove = 5; vertMove < 12; vertMove += 1) {
            for (int sideMove = 8; sideMove < 20; sideMove += 2) {
                int turn = vertMove * sideMove * 2 - 1;
                if (turn * 10 < NN) {
                    continue;
                }
                auto& c = cands.pushR();
                c.vertMove = vertMove;
                c.sideMove = sideMove;
                c.moveTurn = turn;
            }
        }
        sort(ALL(cands), [](const Cand& a, const Cand& b) { return a.moveTurn < b.moveTurn; });

        IOResult bestResult;
        int bestScore = 0;
        Cand bestCand;

        IOResult tmpResult;

        for (cauto& c : cands) {

            if (!Greedy(c.vertMove, c.sideMove, true, 0, tmpResult)) {
                continue;
            }
            int score = server.CalcScore(tmpResult);
            if (score > bestScore) {
                bestScore = score;
                bestResult = tmpResult;
                bestCand = c;

                if (bestScore > NN) {
                    break;
                }
            }
        }

        int bestSkipCount = 0;
        while (!timer_.IsOver()) {
            int vertMove = bestCand.vertMove - 1 + rand_(2);
            int sideMove = bestCand.sideMove - 1 + rand_(2);
            int skipCount = 0;  

            int turn = vertMove * sideMove * 2 - 1 - skipCount;
            if (turn * 10 < NN) {
                continue;
            }

            if (!Greedy(vertMove, sideMove, false, skipCount, tmpResult)) {
                continue;
            }
            int score = server.CalcScore(tmpResult);
            if (score > bestScore) {
                bestScore = score;
                bestResult = tmpResult;
                bestCand.vertMove = vertMove;
                bestCand.sideMove = sideMove;
                bestSkipCount = skipCount;
            }
        }


        server.Output(bestResult);
    }

    bool Greedy(int vertMove, int sideMove, bool safety, int skipCount, IOResult& result) {
        result.Init();

        static dvec<array<Dir, 3>> jigDirPatterns;
        if (jigDirPatterns.empty()) {
            REP(firstDi, 4) {
                Dir firstDir = (Dir)firstDi;
                array<Dir, 2> vertDirs = {RotateLeft(firstDir), RotateRight(firstDir)};
                REP(vertDi, 2) {
                    auto& p = jigDirPatterns.pushR();
                    p[0] = (Dir)firstDi;
                    p[1] = vertDirs[vertDi];
                    p[2] = Back(firstDir);
                }
            }
        }

        static dvec<u8> jigButtons;
        jigButtons.clear();
        REP(i, vertMove) {
            REP(j, sideMove) {
                if (i == 0 && j < skipCount) {
                    continue;
                }
                jigButtons.push(0);
            }
            jigButtons.push(1);
            REP(j, sideMove) {
                jigButtons.push(2);
            }
            if (i + 1 < vertMove) {
                jigButtons.push(1);
            }
        }
        NNBox<int> refCounts = {};
        int totalPaint = 0;

        static dvec<dvec<dvec<dvec<int>>>> misStartsCells(M);  
        REP(mi, M) {
            misStartsCells[mi].resizeR(NN);

            REP(start, NN) {
                misStartsCells[mi][start].resizeR(jigDirPatterns.size());

                REP(ptn, jigDirPatterns.size()) {
                    static NNCheckMap used;
                    used.Clear();

                    auto& cells = misStartsCells[mi][start][ptn];
                    cells.clear();

                    int cell = start;
                    cells.push(cell);
                    used.Check(cell);

                    REP(i, jigButtons.size()) {
                        int ki = jigButtons[i];
                        VASSERT(ki >= 0 && ki < 3);
                        Dir dir = jigDirPatterns[ptn][ki];
                        int next = dirMap[cell][(int)dir];
                        if (next >= 0 && server.CanMove(cell, next)) {
                            cell = next;
                            if (used.Enter(cell)) {
                                cells.push(cell);
                            }
                        }
                    }
                }
            }
        }

        auto PaintAdd = [&](int start, int mi, int ptn) {
            cauto& cells = misStartsCells[mi][start][ptn];
            for (int cell : cells) {
                if (refCounts[cell] == 0) {
                    ++totalPaint;
                }
                ++refCounts[cell];
            }



        };

        auto PaintRemove = [&](int start, int mi, int ptn) {
            cauto& cells = misStartsCells[mi][start][ptn];
            for (int cell : cells) {
                --refCounts[cell];
                if (refCounts[cell] == 0) {
                    --totalPaint;
                }
            }


        };

        MBox<int> starts = server.start_;
        KBox<int> misPtn;
        REP(mi, M) {
            misPtn[mi] = rand_(jigDirPatterns.size());
        }

        double paintRate = HP.PaintRate;
        if (safety) {
            paintRate = 1000;
        }

        REP(mi, M) {
            Dir firstDir = (Dir)rand_(4);
            array<Dir, 2> vertDirs = {RotateLeft(firstDir), RotateRight(firstDir)};
            Dir vertDir = vertDirs[rand_(2)];
            Dir secondDir = Back(firstDir);

            PaintAdd(starts[mi], mi, misPtn[mi]);
        }

        while (true) {
            if (timer_.IsOver()) {
                return false;
            }

            bool updated = false;
            REP(mi, M) {
                int oldStart = starts[mi];
                int oldTotalPoint = totalPaint;
                int oldPtn = misPtn[mi];

                double bestScore = -1e100;
                int bestTotalPaint = totalPaint;
                int bestStart = starts[mi];
                int bestPtn = misPtn[mi];

                PaintRemove(starts[mi], mi, misPtn[mi]);

                for (int start : misStartCells_[mi]) {
                    REP(ptn, jigDirPatterns.size()) {
                        PaintAdd(start, mi, ptn);

                        int dist = misStartDists_[mi][start];
                        double score = totalPaint * paintRate - dist;
                        if (score > bestScore) {
                            bestScore = score;
                            bestTotalPaint = totalPaint;
                            bestStart = start;
                            bestPtn = ptn;
                        }

                        PaintRemove(start, mi, ptn);
                    }
                }

                if (bestStart != oldStart || bestPtn != oldPtn) {
                    updated = true;

                    starts[mi] = bestStart;
                    misPtn[mi] = bestPtn;
                }

                PaintAdd(starts[mi], mi, misPtn[mi]);
                VASSERT(totalPaint == bestTotalPaint);
            }
            if (!updated) {
                break;
            }
        }

        if (totalPaint != NN) {
            return false;
        }


        MBox<NNBox<int>> misDists;
        REP(mi, M) {
            NNQue<int> que;
            auto& dists = misDists[mi];
            dists.fill(INT_MAX);
            {
                int start = starts[mi];
                que.push(start);
                dists[start] = 0;
            }
            while (!que.empty()) {
                int p = que.pop();
                for (int n : aroundMap[p]) {
                    if (!server.CanMove(p, n)) {
                        continue;
                    }
                    int d = dists[p] + 1;
                    if (d < dists[n]) {
                        que.push(n);
                        dists[n] = d;
                    }
                }
            }
        }

        KBox<MBox<Dir>> buttonsRobotsDir;
        REP(ki, K) {
            REP(mi, M) {
                buttonsRobotsDir[ki][mi] = Dir::N;
            }
        }
        REP(mi, M) {
            cauto& dirs = jigDirPatterns[misPtn[mi]];
            REP(ki, 3) {
                buttonsRobotsDir[ki][mi] = dirs[ki];
            }
        }

        int fixButtons = 3;
        MBox<int> curCells = server.start_;  
        dvec<int> readyButtons;
        MBox<array<int, 5>> misUseDirs = {};  
        REP(mi, M) {
            REP(ki, fixButtons) {
                ++misUseDirs[mi][(int)buttonsRobotsDir[ki][mi]];
            }
        }

        while (true) {
            if (timer_.IsOver()) {
                return false;
            }

            int farDist = 0;
            REP(mi, M) {
                int dist = misDists[mi][curCells[mi]];
                farDist = max(farDist, dist);
            }
            if (farDist == 0) {
                break;
            }

            int useKi = -1;
            int bestTotalDist = INT_MAX;
            REP(ki, fixButtons) {
                bool ng = false;
                int totalDist = 0;
                REP(mi, M) {
                    int curCell = curCells[mi];
                    Dir dir = buttonsRobotsDir[ki][mi];
                    int nextCell = server.CalcNextCell(curCell, dir);

                    int curDist = misDists[mi][curCell];
                    int nextDist = misDists[mi][nextCell];
                    if (nextDist >= farDist) {
                        ng = true;
                        break;
                    }

                    totalDist += nextDist;
                }

                if (ng) {
                    continue;
                }

                if (totalDist < bestTotalDist) {
                    bestTotalDist = totalDist;
                    useKi = ki;
                }
            }

            if (useKi < 0) {
                if (fixButtons >= K) {
                    return false;
                    break;
                }

                int ki = fixButtons;
                REP(mi, M) {
                    int curCell = curCells[mi];
                    int bestDi = 4;
                    int bestDist = misDists[mi][curCell];
                    int bestUseCount = misUseDirs[mi][bestDi];
                    REP(di, 4) {
                        int nextCell = dirMap[curCell][di];
                        if (nextCell >= 0 && server.CanMove(curCell, nextCell)) {
                            int nextDist = misDists[mi][nextCell];
                            if (nextDist < bestDist) {
                                bestDist = nextDist;
                                bestDi = di;
                                bestUseCount = misUseDirs[mi][bestDi];
                            }
                            else if (nextDist == bestDist) {
                                int useDirs = misUseDirs[mi][di];
                                if (useDirs < bestUseCount) {
                                    bestUseCount = useDirs;
                                    bestDi = di;
                                }
                            }
                        }
                    }
                    buttonsRobotsDir[ki][mi] = (Dir)bestDi;
                    ++misUseDirs[mi][bestDi];
                }
                useKi = fixButtons;
                ++fixButtons;
            }

            readyButtons.push(useKi);

            int ki = useKi;
            REP(mi, M) {
                int curCell = curCells[mi];
                Dir dir = buttonsRobotsDir[ki][mi];
                int nextCell = server.CalcNextCell(curCell, dir);
                curCells[mi] = nextCell;
            }
        }

        result.buttons_.clear();
        REP(i, readyButtons.size()) {
            result.buttons_.push(readyButtons[i]);
        }
        REP(i, jigButtons.size()) {
            result.buttons_.push(jigButtons[i]);
        }
        result.buttonsRobotsDir_ = buttonsRobotsDir;
        return true;
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
