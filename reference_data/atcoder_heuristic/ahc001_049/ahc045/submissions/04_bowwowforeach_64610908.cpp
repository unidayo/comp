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
#pragma GCC target ("avx2")
#pragma GCC optimize "O3,omit-frame-pointer,inline"
#pragma GCC optimize ("unroll-loops")

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

    T array_[CAP];
    int size_ = 0;

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
            int newCap = size;
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
            reserve(std::max(DefaultMallocSize, cap_ * 2));
        }
        return ptr_[size_++];
    }

    void push(const T& v) {
        pushR() = v;
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
            ptr_ = mem_alloc<T>(r.size_);

            for (int i = 0; i < r.size_; ++i) {
                new (&ptr_[i]) T(r.ptr_[i]);
            }

            size_ = r.size_;
            consted_ = size_;
            cap_ = size_;
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
            int newCap = size;
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
            int newCap = size;
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

            mem_free(ptr_);
            ptr_ = mem_alloc<T>(size);

            for (int i = 0; i < size; ++i) {
                new (&ptr_[i]) T(v);
            }
            size_ = size;
            consted_ = size_;
            cap_ = size_;
        }
    }


    [[nodiscard]] T& pushR() {
        if (size_ < consted_) {
            ++size_;
            return ptr_[size_ - 1];
        }
        else if (size_ == cap_) {
            reserve(std::max(DefaultMallocSize, cap_ * 2));
        }
        new (&ptr_[size_]) T();
        ++size_;
        VASSERT(consted_ < size_);
        consted_ = size_;

        return ptr_[size_ - 1];
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
    size_t m_capacity = 0;
    K* m_keys = nullptr;
    V* m_values = nullptr;
    uint8_t* m_states = nullptr;  
    size_t m_mask = 0;

    int m_count = 0;

    dhmap(const dhmap&) = delete;
    void operator=(const dhmap&) = delete;
    dhmap() {
    }

    ~dhmap() {
        free(m_keys);
        free(m_values);
        free(m_states);
    }

    void init(size_t capacity) {
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
        VASSERT((size_t)m_count <
                m_capacity);  

        size_t i = key & m_mask;
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
        size_t i = key & m_mask;
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
        size_t i = key & m_mask;
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
        size_t i = key & m_mask;
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
    bool Direct(int i, K& k, V& v) const {
        if (m_states[i]) {
            k = m_keys[i];
            v = m_values[i];
            return true;
        }
        return false;
    }

    inline double GetUseRate() const {
        return m_count / (double)m_capacity;
    }
};


template <class K>
struct dhset {
    size_t m_capacity = 0;
    K* m_keys = nullptr;
    uint8_t* m_states = nullptr;  
    size_t m_mask = 0;

    int m_count = 0;

    dhset(const dhset&) = delete;
    void operator=(const dhset&) = delete;
    dhset() {
    }

    ~dhset() {
        free(m_keys);
        free(m_states);
    }

    void init(size_t capacity) {
        if (capacity <= m_capacity) {
            clear();
            return;
        }

        free(m_keys);
        free(m_states);

        m_capacity = 1ULL << bit_width(capacity);
        m_mask = m_capacity - 1;
        m_keys = (K*)malloc(m_capacity * sizeof(K));
        m_states = (uint8_t*)calloc(m_capacity, sizeof(uint8_t));
        m_count = 0;
    }

    void clear() {
        memset(m_states, 0, m_capacity * sizeof(uint8_t));
        m_count = 0;
    }

    inline void set(K key) {
        VASSERT((size_t)m_count <
                m_capacity);  

        size_t i = key & m_mask;
        REP(loop, m_capacity) {
            if (m_states[i] == 1) {
                if (key == m_keys[i]) {
                    break;
                }
            }
            else {
                m_states[i] = 1;
                m_keys[i] = key;
                ++m_count;
                break;
            }
            (++i) &= m_mask;
        }
    }
    void remove(K key) {
        size_t i = key & m_mask;
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
        size_t i = key & m_mask;
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


#define DEF_CONTAINERS(name, size)        \
    template <class TYPE>                 \
    using name##Box = array<TYPE, size>;  \
    template <class TYPE>                 \
    using name##Arr = CapArr<TYPE, size>; \
    template <class TYPE>                 \
    using name##Que = CapQue<TYPE, size>; \
    using name##Set = CapSet<size>;       \
    template <class TYPE>                 \
    using name##Map = CapMap<TYPE, size>; \
    template <class TYPE>                 \
    using name##Deque = CapacityRingDeque<TYPE, size>;

constexpr int N = 800;
constexpr int M_max = 400;
constexpr int Q = 400;
constexpr int L_min = 3;
constexpr int L_max = 15;
constexpr int W_min = 500;
constexpr int W_max = 2500;


constexpr int PointMax = 10000;

DEF_CONTAINERS(N, N);
DEF_CONTAINERS(M, M_max);
DEF_CONTAINERS(Q, Q);
DEF_CONTAINERS(L, L_max);

int M;
int L;
int W;


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


    PD(EstimatePointTimeWeight, 0.7, 0.0, 2.0);
    PD(FirstGroupTimeWeight, 0.2, 0.0, 0.4, 0);
    PD(UpdateGroupTimeWeight, 0.1, 0.0, 0.2, 0);


    PI(EP_SampleQueCount, 2, 1, 4, 1);       
    PD(EP_WeightScaleScale, 1, 0.99, 1.01);  
    PI(EP_FirstSample, 20, 15, 20, 1);

    PD(EP_WeightScale, 2.73333524761478, 2.5, 3.0, 1.0);
    PD(EP_OneLoopQueryRate, 0.24718847850845124, 0.1, 0.5, 0.0);
    PI(EP_VertexIter, 53, 30, 90, 1);
    PD(EP_TimeRateStart, 0.11417503867623818, 0.0, 0.5, 0.0, 1.0);
    PD(EP_TimeRateEnd, 0.49104599512868985, 0.5, 1.0, 0.0, 1.0);
    PD(EP_HCBestScoreRate, 0.7393018788245713, 0.5, 1.5, 0.0);



    PD(EP_FirstEdgeDistRate, 30.83173254855196, 0.0, 200.0);
    PD(EP_FirstVertexUseRate, 22653.492696027617, 15000.0, 25000.0);
    PD(EP_FirstEdgeUseRate, 10407.44981394889, 5000.0, 10000.0);
    PD(EP_VarRate, 1.064442325990824, 0.5, 1.0);
    PD(EP_VertexUseRate, 666.3078108775096, 0.0, 1000.0);
    PD(EP_EdgeUseRate, 396.1512710207579, 0.0, 1600.0);
    PD(EP_EdgeConstraintScore, 18076.896399949143, 15000.0, 20000.0);
    PD(EP_NoUseEdgeScore, 1489.9235351100638, 1000.0, 9000.0);
    PD(EP_AllUseEdgeScore, 195.67156211278055, 0.0, 1000.0);

    START_TUNING;

    END_TUNING;




    PD(GB_SizeDiff, 1.0369096856168052, 0.0, 2.0);
    PD(GB_CutSumRate, 1.1927413171956918, 0.0, 2.0);
    PD(GB_TargetDiff, 1.6154141636555022, 0.0, 2.0);






    PD(G_StartTemp, 42.18589904030368, 0.0, 50.0, 0.0);
    PD(G_EndTemp, 13.96080264818707, 0.0, 30.0, 0.0);
    PI(G_InitialStateCount, 8, 1, 8, 1);
    PI(G_RollbackCount, 58316, 50000, 100000, 1);
    PD(G_RollbackNextMulti, 0.916705959936248, 0.8, 1.1, 0.1);
    PI(G_MinRollbackCount, 157, 1, 300, 1);
    PC(UseCompSort, 1, 0, 1);

    PI(G_UpdateTmpMaskBit, 4, 0, 10, 0, 16);
    PD(G_RollbackStartRate, 0, 0.0, 0.15, 0.0, 1.0);

    PC(G_TempType, 0, 0, 1, 2);
    PD(G_TempPow, 2, 1.0, 3.0, 1.0);



} HP;


#undef PC
#undef PI
#undef PD







struct IOResult {
    MArr<NArr<int>> groupsVertexs_;
    MArr<NArr<pint>> groupsEdges_;
};
struct Rect {
    int l, t, r, b;  

    pint GetCenter() const {
        return pint{(l + r) / 2, (t + b) / 2};
    }
    int GetSize() const {
        return (r - l + 1) * (b - t + 1);
    }
    pint GetRandomPoint(Xor64& rand_) const {
        return pint{l + rand_(r - l + 1), t + rand_(b - t + 1)};
    }

    void Clip(pint& p) const {
        if (p.x < l) {
            p.x = l;
        }
        else if (p.x > r) {
            p.x = r;
        }
        if (p.y < t) {
            p.y = t;
        }
        else if (p.y > b) {
            p.y = b;
        }
    }
};


template <int SIZE>
struct UnionFindS {
   private:
    CapArr<int, SIZE> nodes;  
    int groupCount;

   public:
    inline int RootIndex(int i) {
        if (nodes[i] < 0) {
            return i;
        }
        return nodes[i] = RootIndex(nodes[i]);
    }

   public:
    void Init(int n) {
        nodes.assign(n, -1);
        groupCount = n;
    }

    void Join(int a, int b) {
        int rootA = RootIndex(a);
        int rootB = RootIndex(b);
        if (rootA == rootB) {
            return;
        }

        if (nodes[rootA] > nodes[rootB]) {
            swap(rootA, rootB);
        }

        nodes[rootA] += nodes[rootB];
        nodes[rootB] = rootA;
        --groupCount;
    }

    bool IsReachable(int a, int b) {
        int rootA = RootIndex(a);
        int rootB = RootIndex(b);
        return rootA == rootB;
    }

    int GetRoot(int i) {
        return RootIndex(i);
    }

    bool IsRoot(int i) const {
        return nodes[i] < 0;
    }

    int Count(int a) const {
        return -nodes[const_cast<UnionFindS&>(*this).RootIndex(a)];
    }

    int GetGroupCount() const {
        return groupCount;
    }

    void GetGroups(dvec<dvec<int>>& groups) {
        groups.resizeR(groupCount);
        for (auto& g : groups) {
            g.clear();
        }

        CapArr<int, SIZE> nodesGroupIndex(nodes.size(), -1);
        int nextIndex = 0;
        REP(i, nodes.size()) {
            int root = RootIndex(i);
            if (nodesGroupIndex[root] < 0) {
                nodesGroupIndex[root] = nextIndex++;
            }
            groups[nodesGroupIndex[root]].push(i);
        }
    }
};


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

int CalcEuclideanDistance(int x1, int y1, int x2, int y2) {
    return int(sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}
int CalcEuclideanDistance(const pint& p1, const pint& p2) {
    return CalcEuclideanDistance(p1.x, p1.y, p2.x, p2.y);
}

int CalcMinDistance(const Rect& rect1, const Rect& rect2) {
    int dx = max(0, max(rect1.l - rect2.r, rect2.l - rect1.r));
    int dy = max(0, max(rect1.t - rect2.b, rect2.t - rect1.b));

    return CalcEuclideanDistance(dx, dy, 0, 0);
}

int CalcMaxDistance(const Rect& rect1, const Rect& rect2) {
    int max_dist = 0;

    for (int x1 : {rect1.l, rect1.r}) {
        for (int y1 : {rect1.t, rect1.b}) {
            for (int x2 : {rect2.l, rect2.r}) {
                for (int y2 : {rect2.t, rect2.b}) {
                    max_dist = max(max_dist, CalcEuclideanDistance(x1, y1, x2, y2));
                }
            }
        }
    }
    return max_dist;
}

void DoQuery(LArr<int> query, const NBox<NBox<int>>& dist, LArr<pint>& edges) {
    query.sort();
    edges.clear();

    vector<tuple<int, int, int>> edges_candidate;
    REP(i, query.size()) {
        FOR(j, i + 1, query.size()) {
            edges_candidate.push_back({dist[query[i]][query[j]], i, j});
        }
    }

    sort(edges_candidate.begin(), edges_candidate.end());

    UnionFindS<N> uf;
    uf.Init(query.size());
    for (auto& [_, i, j] : edges_candidate) {
        if (uf.IsReachable(i, j)) {
            continue;
        }
        uf.Join(i, j);
        edges.push(pint{query[i], query[j]});
    }

    sort(edges.begin(), edges.end());
}

s64 ComputeScoreDetails(const MArr<int>& Gs, const NBox<NBox<int>>& dist, const IOResult& result) {
    typedef vector<vector<int>> Groups;
    typedef vector<vector<pair<int, int>>> Edges;

    vector<int> city_group(N, -1);
    vector<int> city_id(N, -1);
    int line_ptr = 0;
    s64 score = 0;
    Groups groups(M);
    Edges edges(M);

    cauto& groupsVertexs = result.groupsVertexs_;
    cauto& groupsEdges = result.groupsEdges_;

    for (int g = 0; g < M; ++g) {
        vector<int> group;
        for (int k = 0; k < Gs[g]; ++k) {
            int i = groupsVertexs[g][k];
            VASSERT(city_group[i] == -1);

            city_group[i] = g;
            city_id[i] = k;
            group.push_back(i);
        }
        groups[g] = std::move(group);

        UnionFindS<N> dsu;
        dsu.Init(Gs[g]);
        for (int k = 0; k < Gs[g] - 1; ++k) {
            int i = groupsEdges[g][k].a;
            int j = groupsEdges[g][k].b;

            VASSERT(city_group[i] == g && city_group[j] == g);

            int id_i = city_id[i];
            int id_j = city_id[j];
            VASSERT(!dsu.IsReachable(id_i, id_j));

            dsu.Join(id_i, id_j);
            score += dist[i][j];
            edges[g].emplace_back(i, j);
        }
    }

    return score;
}

struct DistRange {
    int min_;
    int max_;

    static DistRange Make(const Rect& a, const Rect& b) {
        DistRange ret;
        ret.min_ = CalcMinDistance(a, b);
        ret.max_ = CalcMaxDistance(a, b);
        return ret;
    }

    int GetCenter() const {
        return (min_ + max_) / 2;
    }
};

struct EdgeNo {
    NBox<NBox<int>> niToEi_;  
    dvec<pint> eiToNi_;       
    NBox<dvec<int>> nearArounds_;
    dvec<int> nearEis_;
    NBox<NBox<bool>> isNear_;

    void Init(const NBox<Rect>& rects) {
        REP(i, N) {
            FOR(j, i + 1, N) {
                niToEi_[i][j] = niToEi_[j][i] = eiToNi_.size();
                auto& ni = eiToNi_.pushR();
                ni.a = i;
                ni.b = j;
            }
        }

        dvec<DistRange> distRanges(eiToNi_.size());
        REP(ei, distRanges.size()) {
            auto [a, b] = eiToNi_[ei];
            distRanges[ei] = DistRange::Make(rects[a], rects[b]);
        }

        for (auto& n : isNear_) {
            n.fill(false);
        }

        UnionFindS<N> uf;
        uf.Init(N);

        int loop = 0;
        while (uf.GetGroupCount() > 1) {
            NBox<int> nearMax;
            nearMax.fill(INT_MAX);
            REP(ei, distRanges.size()) {
                auto [a, b] = eiToNi_[ei];
                if (uf.IsReachable(a, b)) {
                    continue;
                }
                chmin(nearMax[uf.GetRoot(a)], distRanges[ei].max_);
                chmin(nearMax[uf.GetRoot(b)], distRanges[ei].max_);
            }

            auto tmpUf = uf;

            REP(ei, distRanges.size()) {
                auto [a, b] = eiToNi_[ei];
                if (tmpUf.IsReachable(a, b)) {
                    continue;
                }

                if (distRanges[ei].min_ > nearMax[tmpUf.GetRoot(a)] &&
                    distRanges[ei].min_ > nearMax[tmpUf.GetRoot(b)]) {
                    continue;
                }
                nearArounds_[a].push(b);
                nearArounds_[b].push(a);
                nearEis_.push(ei);
                isNear_[a][b] = true;
                isNear_[b][a] = true;
                uf.Join(a, b);
            }

            ++loop;
            if (loop > 10000) {
                cerr << "**failed**" << endl;
                break;
            }
        }
    }

    int EdgeCount() const {
        return eiToNi_.size();
    }

    int ToEi(int ni, int nj) const {
        return niToEi_[ni][nj];
    }

    const pint& ToNi(int ei) const {
        return eiToNi_[ei];
    }

    bool IsNear(int ei) const {
        auto [a, b] = eiToNi_[ei];
        return isNear_[a][b];
    }
};
EdgeNo edgeNo_;

struct IOServer {
    NBox<Rect> rects_;
    dvec<int> Gs_;  


    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        int dummy;
        is >> dummy;
        timer.Init();  

        is >> M >> dummy >> L >> W;

        Gs_.resizeR(M);
        REP(i, M) {
            int g;
            is >> g;
            Gs_[i] = g;
        }

        NBox<pint> tmpCenters;
        REP(i, N) {
            int l, t, r, b;
            is >> l >> r >> t >> b;
            rects_[i].l = l;
            rects_[i].r = r;
            rects_[i].t = t;
            rects_[i].b = b;
            tmpCenters[i] = rects_[i].GetCenter();
        }

        static NBox<NBox<int>> distMat;
        REP(i, N) {
            FOR(j, i + 1, N) {
                int d = CalcEuclideanDistance(tmpCenters[i].x, tmpCenters[i].y, tmpCenters[j].x, tmpCenters[j].y);
                distMat[i][j] = distMat[j][i] = d;
            }
        }

        edgeNo_.Init(rects_);


    }

    void Input() {
        istream& is = cin;
    }

    void Query(const LArr<int>& query, LArr<pint>& edges) {
        ostream& os = cout;

        os << "? " << query.size();
        REP(i, query.size()) {
            os << " " << query[i];
        }
        os << endl;

        istream& is = cin;
        edges.resize(query.size() - 1);
        REP(i, query.size() - 1) {
            is >> edges[i].x >> edges[i].y;
        }
    }

    void Output(const IOResult& result) {
        ostream& os = cout;
        cauto& groupsVertexs = result.groupsVertexs_;
        cauto& groupsEdges = result.groupsEdges_;

        os << "!" << endl;
        REP(i, M) {
            VASSERT(Gs_[i] == groupsVertexs[i].size());
            VASSERT(Gs_[i] == groupsEdges[i].size() + 1);
            cauto& vertexs = groupsVertexs[i];
            cauto& edges = groupsEdges[i];

            REP(j, vertexs.size()) {
                if (j != 0) {
                    os << " ";
                }
                os << vertexs[j];
            }
            os << endl;

            REP(j, edges.size()) {
                os << edges[j].x << " " << edges[j].y << endl;
            }
        }
    }

    void Finalize() {
    }
};
IOServer server;


Xor64 rand_;


class OrderGraph {
   public:
    int n;
    dvec<dvec<int>> adj;  
    dvec<int> indegree;   

    void Init(int size) {
        n = size;
        adj.resizeR(size);
        for (auto& a : adj) {
            a.clear();
        }
        indegree.assign(size, 0);
    }

    void add(int a, int b) {
        adj[a].push(b);
        indegree[b]++;
    }

    int compare(int a, int b) const {
        static CheckMapD visited;
        visited.Init(n);
        if (dfs(a, b, visited)) {
            return -1;  
        }
        visited.Clear();
        if (dfs(b, a, visited)) {
            return 1;  
        }
        return 0;  
    }

   private:
    bool dfs(int a, int b, CheckMapD& visited) const {
        if (a == b) {
            return true;
        }

        visited.Check(a);
        for (int next : adj[a]) {
            if (!visited[next]) {
                if (dfs(next, b, visited)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool hasCycle() const {
        static queue<int> q;
        static dvec<int> in_deg;
        in_deg = indegree;
        int count = 0;

        for (int i = 0; i < n; i++) {
            if (in_deg[i] == 0) q.push(i);
        }

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            count++;
            for (int next : adj[node]) {
                if (--in_deg[next] == 0) {
                    q.push(next);
                }
            }
        }

        return count != n;  
    }

   public:
    void CalcLargeList(dvec<dvec<int>>& largers) const {
        static queue<int> q;
        static dvec<int> in_deg;
        static dvec<int> sorted;
        sorted.clear();
        sorted.reserve(n);
        in_deg = indegree;
        int count = 0;

        for (int i = 0; i < n; i++) {
            if (in_deg[i] == 0) {
                q.push(i);
            }
        }

        while (!q.empty()) {
            int node = q.front();
            q.pop();
            sorted.push(node);

            for (int next : adj[node]) {
                if (--in_deg[next] == 0) {
                    q.push(next);
                }
                VASSERT(in_deg[next] >= 0);
            }
        }
        VASSERT(sorted.size() == n);


        largers.resizeR(n);
        static CheckMapD checked;
        RREP(ii, n) {
            int a = sorted[ii];
            largers[a].clear();

            checked.Init(n);
            for (int b : adj[a]) {
                if (checked.Enter(b)) {
                    largers[a].push(b);
                }
                for (int c : largers[b]) {
                    if (checked.Enter(c)) {
                        largers[a].push(c);
                    }
                }
            }

        }
    }
};












struct QueryAnalyzer {
    OrderGraph graph_;
    dvec<pint> constraints_;
    dvec<dvec<int>> vertexsConstraints_;  
    dvec<u64> edgeHash_;
    dhset<u64> hasRelEdges_;     
    dvec<int> edgesCheckCount_;  
    dvec<int> edgesUseCount_;    



    u64 GetDoubleEdge(int ei1, int ei2) const {
        return edgeHash_[ei1] ^ edgeHash_[ei2];
    }

    bool HasConstraintEdges(int ei1, int ei2) const {
        return hasRelEdges_.contains(GetDoubleEdge(ei1, ei2));
    }

    void Init() {
        int edgeCount = edgeNo_.EdgeCount();
        graph_.Init(edgeCount);
        constraints_.clear();
        vertexsConstraints_.resizeR(N);
        for (auto& a : vertexsConstraints_) {
            a.clear();
        }
        REP(ei, edgeCount) {
            edgeHash_.push(rand_.Get64());
        }
        hasRelEdges_.init(edgeCount);

        edgesCheckCount_.assign(edgeCount, 0);
        edgesUseCount_.assign(edgeCount, 0);

    }

    void Add(const LArr<int>& query, const LArr<pint>& edges) {

        REP(i, query.size()) {
            FOR(j, i + 1, query.size()) {
                int ei = edgeNo_.ToEi(query[i], query[j]);
                edgesCheckCount_[ei]++;
            }
        }

        LArr<LArr<bool>> queryEdgeMat(query.size());
        for (auto& e : queryEdgeMat) {
            e.assign(query.size(), false);
        }
        for (auto& e : edges) {
            int a = query.find(e.a);
            int b = query.find(e.b);
            queryEdgeMat[a][b] = queryEdgeMat[b][a] = true;
            edgesUseCount_[edgeNo_.ToEi(e.a, e.b)]++;
        }

        REP(qi, query.size()) {
            LQue<int> que;
            LArr<bool> checked(query.size(), false);
            LArr<int> froms(query.size(), -1);
            checked[qi] = true;
            que.push(qi);

            while (!que.empty()) {
                int p = que.pop();

                REP(n, query.size()) {
                    if (p == n) {
                        continue;
                    }
                    if (checked[n]) {
                        continue;
                    }
                    if (queryEdgeMat[p][n]) {
                        froms[n] = p;
                        checked[n] = true;
                        que.push(n);
                    }
                }
            }

            FOR(qj, qi + 1, query.size()) {
                if (!queryEdgeMat[qi][qj]) {
                    LArr<int> path;
                    {
                        int p = qj;
                        while (p != qi) {
                            path.push(p);
                            p = froms[p];
                        }
                        path.push(qi);
                    }

                    int noLinkNi = query[qi];
                    int noLinkNj = query[qj];
                    int noLinkEi = edgeNo_.ToEi(noLinkNi, noLinkNj);
                    if (noLinkEi < 0) {
                        continue;
                    }

                    REP(i, path.size() - 1) {
                        int ni = query[path[i]];
                        int nj = query[path[i + 1]];
                        int ei = edgeNo_.ToEi(ni, nj);
                        if (ei < 0) {
                            continue;
                        }

                        if (HasConstraintEdges(ei, noLinkEi)) {
                            continue;
                        }

                        graph_.add(ei, noLinkEi);  
                        int newCi = constraints_.size();
                        constraints_.pushR() = pint{ei, noLinkEi};

                        CapArr<int, 4> vs;
                        for (int v : {noLinkNi, noLinkNj, ni, nj}) {
                            if (vs.find(v) < 0) {
                                vs.push(v);
                            }
                        }
                        for (int v : vs) {
                            vertexsConstraints_[v].push(newCi);
                        }
                        hasRelEdges_.set(GetDoubleEdge(ei, noLinkEi));
                    }
                }
            }
        }
    }

    void AddDirect(int a, int b) {
        graph_.add(a, b);
    }

    bool IsNouseEdge(int ei) const {
        return edgesUseCount_[ei] < edgesCheckCount_[ei];
    }

    bool IsAllUseEdge(int ei) const {
        return edgesCheckCount_[ei] > 0 && edgesUseCount_[ei] == edgesCheckCount_[ei];
    }

    int Compare(int a, int b) const {
        return graph_.compare(a, b);
    }



};

double CalcMCTWithSortedEis(const dvec<double>& edgesDist, const dvec<int>& srcSortedEis, dvec<int>& dstEis) {
    dstEis.clear();

    UnionFindS<N> uf;
    uf.Init(N);

    double cost = 0;
    for (int ei : srcSortedEis) {
        auto& ni = edgeNo_.ToNi(ei);
        if (uf.IsReachable(ni.a, ni.b)) {
            continue;
        }
        uf.Join(ni.a, ni.b);
        cost += edgesDist[ei];
        dstEis.push(ei);
    }
    return cost;
}

double CalcMCTWithEis(const dvec<double>& edgesDist, dvec<int>& srcEis, dvec<int>& dstEis) {
    sort(ALL(srcEis), [&](int a, int b) { return edgesDist[a] < edgesDist[b]; });
    return CalcMCTWithSortedEis(edgesDist, srcEis, dstEis);
}

double CalcMCTWithVertexs(const dvec<double>& edgesDist, const dvec<int>& vertexs, dvec<int>& dstEis) {
    dstEis.clear();

    static dvec<int> srcEis;
    srcEis.clear();
    REP(i, vertexs.size()) {
        FOR(j, i + 1, vertexs.size()) {
            int ei = edgeNo_.ToEi(vertexs[i], vertexs[j]);
            if (ei >= 0) {
                srcEis.push(ei);
            }
        }
    }
    return CalcMCTWithEis(edgesDist, srcEis, dstEis);
}

double CalcMCTWithEdgeMatrixVertexs(const dvec<double>& edgesDist, const NBox<NBox<bool>>& isEdge,
                                    const dvec<int>& vertexs, dvec<int>& dstEis) {
    dstEis.clear();

    static dvec<int> srcEis;
    srcEis.clear();
    REP(i, vertexs.size()) {
        FOR(j, i + 1, vertexs.size()) {
            if (!isEdge[vertexs[i]][vertexs[j]]) {
                continue;
            }
            int ei = edgeNo_.ToEi(vertexs[i], vertexs[j]);
            if (ei >= 0) {
                srcEis.push(ei);
            }
        }
    }
    return CalcMCTWithEis(edgesDist, srcEis, dstEis);
}


double CalcMCTWithQA(QueryAnalyzer& qa, const dvec<double>& edgesDist, const dvec<int>& vertexs, dvec<int>& eis) {
    eis.clear();

    double cost = 0;
    static dvec<int> tmpEis;
    tmpEis.clear();
    REP(i, vertexs.size()) {
        FOR(j, i + 1, vertexs.size()) {
            int ei = edgeNo_.ToEi(vertexs[i], vertexs[j]);
            tmpEis.push(ei);
        }
    }

    sort(ALL(tmpEis), [&](int a, int b) {
        return edgesDist[a] < edgesDist[b];
    });

    UnionFindS<N> uf;
    uf.Init(N);
    for (int ei : tmpEis) {
        auto& ni = edgeNo_.ToNi(ei);
        if (uf.IsReachable(ni.a, ni.b)) {
            continue;
        }
        uf.Join(ni.a, ni.b);
        cost += edgesDist[ei];
        eis.push(ei);
    }
    return cost;
}

struct SamplingQue {
    double weightSum_;
    NBox<pdouble> pointSums_;
    dvec<double> distSums_;
    int sampleCount_;

    bool HasSample() const {
        return sampleCount_ > 0;
    }

    void Init() {
        weightSum_ = 0;
        pointSums_.fill(pdouble{});
        distSums_.assign(edgeNo_.EdgeCount(), 0.0);
        sampleCount_ = 0;
    }

    void AddSample(const NBox<pint>& points, double weight) {
        REP(ni, N) {
            pointSums_[ni].x += points[ni].x * weight;
            pointSums_[ni].y += points[ni].y * weight;
        }
        for (int ei : edgeNo_.nearEis_) {
            cauto& ni = edgeNo_.ToNi(ei);
            cauto& p = points[ni.a];
            cauto& q = points[ni.b];
            int d = CalcEuclideanDistance(p, q);
            distSums_[ei] += d * weight;
        }
        weightSum_ += weight;

        ++sampleCount_;
    }


    void GetPoints(NBox<pint>& points) const {
        REP(ni, N) {
            points[ni].x = (int)round(pointSums_[ni].x / weightSum_);
            points[ni].y = (int)round(pointSums_[ni].y / weightSum_);
        }
    }


    void AddPoints(NBox<pdouble>& points) const {
        REP(ni, N) {
            points[ni].x += pointSums_[ni].x;
            points[ni].y += pointSums_[ni].y;
        }
    }

    void AddDists(dvec<double>& dists) const {
        for (int ei : edgeNo_.nearEis_) {
            dists[ei] += distSums_[ei];
        }
    }



};

void Sampling(int iterCount, SamplingQue& que, double weight) {
    REP(iter, iterCount) {
        NBox<pint> points;
        REP(ni, N) {
            cauto& rect = server.rects_[ni];
            int x = rect.l + rand_(rect.r - rect.l + 1);
            int y = rect.t + rand_(rect.b - rect.t + 1);
            points[ni].x = x;
            points[ni].y = y;
        }
        que.AddSample(points, weight);
    }
}

void SamplingDist(int iterCount, dvec<double>& edgesDist) {
    edgesDist.assign(edgeNo_.EdgeCount(), 0.0);

    REP(iter, iterCount) {
        NBox<pint> points;
        REP(ni, N) {
            cauto& rect = server.rects_[ni];
            int x = rect.l + rand_(rect.r - rect.l + 1);
            int y = rect.t + rand_(rect.b - rect.t + 1);
            points[ni].x = x;
            points[ni].y = y;
        }

        REP(ei, edgeNo_.EdgeCount()) {
            cauto& ni = edgeNo_.ToNi(ei);
            cauto& p = points[ni.a];
            cauto& q = points[ni.b];
            int d = CalcEuclideanDistance(p, q);
            edgesDist[ei] += d;
        }
    }
    REP(ei, edgeNo_.EdgeCount()) {
        edgesDist[ei] /= iterCount;
    }
}

void SamplingDistMCT(int iterCount, dvec<double>& edgesDist, dvec<int>& edgesUseCount) {
    edgesDist.assign(edgeNo_.EdgeCount(), 0.0);
    edgesUseCount.assign(edgeNo_.EdgeCount(), 0);

    static dvec<int> srcEis;
    srcEis.resizeR(edgeNo_.EdgeCount());
    iota(ALL(srcEis), 0);

    REP(iter, iterCount) {
        NBox<pint> points;
        REP(ni, N) {
            cauto& rect = server.rects_[ni];
            int x = rect.l + rand_(rect.r - rect.l + 1);
            int y = rect.t + rand_(rect.b - rect.t + 1);
            points[ni].x = x;
            points[ni].y = y;
        }

        static dvec<double> singleEdgesDist;
        singleEdgesDist.resizeR(edgeNo_.EdgeCount());
        REP(ei, edgeNo_.EdgeCount()) {
            cauto& ni = edgeNo_.ToNi(ei);
            cauto& p = points[ni.a];
            cauto& q = points[ni.b];
            int d = CalcEuclideanDistance(p, q);
            singleEdgesDist[ei] = d;
            edgesDist[ei] += d;
        }

        static dvec<int> singleEis;
        double cost = CalcMCTWithEis(singleEdgesDist, srcEis, singleEis);
        for (int ei : singleEis) {
            ++edgesUseCount[ei];
        }
    }
    REP(ei, edgeNo_.EdgeCount()) {
        edgesDist[ei] /= iterCount;
    }
}



enum class SAResult : u8 {
    Ok,    
    Ng,    
    Fail,  
};

template <class Pack>
struct StateData {
    Pack best_pack;      
    s64 best_raw_score;  
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

template <class Pack>
struct SA {
   private:
    dvec<StateData<Pack>> datas_;

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
                    else {
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

    const StateData<Pack>& GetBest() {
        int best = 0;
        FOR(i, 1, datas_.size()) {
            if (datas_[i].best_raw_score > datas_[best].best_raw_score) {
                best = i;
            }
        }
        return datas_[best];
    }

};

namespace point_hc {

struct State {
    NBox<pint> points_;
    s64 evalScore_;

    void Init(const QueryAnalyzer& qa, const NBox<pint>& points, bool resumePoint) {
        if (resumePoint) {
            REP(i, N) {
                points_[i] = points[i];
            }
        }
        else {
            REP(i, N) {
                points_[i] = server.rects_[i].GetCenter();
            }
        }
        evalScore_ = CalcScore(qa);
    }

    s64 CalcScore(const QueryAnalyzer& qa) {
        s64 score = 0;
        REP(i, qa.constraints_.size()) {
            auto& c = qa.constraints_[i];
            auto& e1 = edgeNo_.ToNi(c.a);
            auto& e2 = edgeNo_.ToNi(c.b);

            int d1 = norm2(points_[e1.a] - points_[e1.b]);
            int d2 = norm2(points_[e2.a] - points_[e2.b]);

            if (d1 > d2) {
                score -= square(int(sqrt(d1)) - int(sqrt(d2)));
            }
        }
        return score;
    }

    bool Update(const QueryAnalyzer& qa, int iterCount, int timeLimitUs) {


        REP(i, N) {
            if (timeLimitUs >= 0) {
                if (timer_.ElapseTimeUs() > timeLimitUs) {
                    return false;
                }
            }

            if (qa.vertexsConstraints_[i].empty()) {
                points_[i] = server.rects_[i].GetRandomPoint(rand_);
                continue;
            }

            s64 baseScore = evalScore_;
            for (int ci : qa.vertexsConstraints_[i]) {
                cauto& c = qa.constraints_[ci];
                auto& e1 = edgeNo_.ToNi(c.a);
                auto& e2 = edgeNo_.ToNi(c.b);

                int d1 = norm2(points_[e1.a] - points_[e1.b]);
                int d2 = norm2(points_[e2.a] - points_[e2.b]);

                if (d1 > d2) {
                    baseScore += square(int(sqrt(d1)) - int(sqrt(d2)));
                }
            }

            pint bestPoint = points_[i];
            s64 bestScore = s64(evalScore_ * HP.EP_HCBestScoreRate);
            REP(iter, iterCount) {
                points_[i] = server.rects_[i].GetRandomPoint(rand_);

                s64 newScore = baseScore;
                for (int ci : qa.vertexsConstraints_[i]) {
                    cauto& c = qa.constraints_[ci];
                    auto& e1 = edgeNo_.ToNi(c.a);
                    auto& e2 = edgeNo_.ToNi(c.b);

                    int d1 = norm2(points_[e1.a] - points_[e1.b]);
                    int d2 = norm2(points_[e2.a] - points_[e2.b]);

                    if (d1 > d2) {
                        newScore -= square(int(sqrt(d1)) - int(sqrt(d2)));
                        if (newScore < bestScore) {
                            break;
                        }
                    }
                }
                if (newScore >= bestScore) {
                    bestScore = newScore;
                    bestPoint = points_[i];

                }
                if (newScore == baseScore) {
                    break;
                }
            }
            VASSERT(bestPoint.x >= 0);
            points_[i] = bestPoint;
            evalScore_ = bestScore;
        }
        return true;
    }
};  
static constexpr int StateSize = sizeof(State);

void SamplingTime(int timeLimitUs, const QueryAnalyzer& qa, SamplingQue& que, int vertexIterCount, double weight) {

    State state;
    if (que.HasSample()) {
        NBox<pint> points;
        que.GetPoints(points);
        state.Init(qa, points, true);
    }
    else {
        NBox<pint> points;
        state.Init(qa, points, false);
    }


    int updateTimeLimit = -1;
    while (true) {
        if (!state.Update(qa, vertexIterCount, updateTimeLimit)) {
            break;
        }
        updateTimeLimit = timeLimitUs;

        cauto& points = state.points_;
        que.AddSample(points, weight);

        if (timer_.ElapseTimeUs() > timeLimitUs) {
            break;
        }
    }
}
}  



#define LEAK_CHECK 0  

template <class OBJECT>
struct ObjectPool {
    static_assert(is_trivially_copyable_v<OBJECT>);

    OBJECT* pool_ = nullptr;       
    OBJECT** reusable_ = nullptr;  
    int capacity_ = 0;             
    int usedTotalCount_ = 0;       
    int usedPoolCount_ = 0;        
    int reusableCount_ = 0;        

    int maxTotalCount_ = 0;


    ~ObjectPool() {
        if (pool_) {
            mem_free(pool_);
        }
        if (reusable_) {
            mem_free(reusable_);
        }
    }


    void Init(int capacity) {
        if (capacity != capacity_) {
            if (pool_) {
                mem_free(pool_);
            }
            if (reusable_) {
                mem_free(reusable_);
            }
            pool_ = mem_alloc<OBJECT>(capacity);
            reusable_ = mem_alloc<OBJECT*>(capacity);
            capacity_ = capacity;

            VASSERT(pool_);
            VASSERT(reusable_);
        }
        usedTotalCount_ = 0;
        usedPoolCount_ = 0;
        reusableCount_ = 0;
    }

    void Clear() {
        usedTotalCount_ = 0;
        usedPoolCount_ = 0;
        reusableCount_ = 0;
    }

    OBJECT* New() {
        if (reusableCount_) {
            ++usedTotalCount_;
            --reusableCount_;
            chmax(maxTotalCount_, usedTotalCount_);
            return reusable_[reusableCount_];
        }

        if (usedPoolCount_ >= capacity_) {
        }
        ++usedTotalCount_;
        chmax(maxTotalCount_, usedTotalCount_);
        return &pool_[usedPoolCount_++];
    }

    inline void Delete(OBJECT* obj) {
        reusable_[reusableCount_] = obj;
        ++reusableCount_;
        --usedTotalCount_;
    }

    inline void Delete(OBJECT** start, int count) {
        memcpy(&reusable_[reusableCount_], start, sizeof(OBJECT*) * count);
        reusableCount_ += count;
        usedTotalCount_ -= count;

    }

    inline int GetUsedCount() const {
        return usedTotalCount_;
    }

    inline string Usage() const {
        stringstream ss;
        ss << usedTotalCount_ << " (" << (int)floor(usedTotalCount_ * 100 / (double)capacity_ + 0.5) << " %)";
        return ss.str();
    }

    inline int UsedRate() const {
        return (int)round(usedTotalCount_ * 100 / (double)capacity_);
    }

    inline int GetCapacity() const {
        return capacity_;
    }

    inline int GetMaxUseCount() const {
        return maxTotalCount_;
    }

    inline int GetMaxUsedRate() const {
        return (int)round((double)GetMaxUseCount() * 100 / (double)capacity_);
    }
    inline string MaxUsage() const {
        stringstream ss;
        ss << maxTotalCount_ << " / " << capacity_ << " (" << (int)floor(maxTotalCount_ * 100 / (double)capacity_ + 0.5)
           << " %)";
        return ss.str();
    }
    double GetMaxMemoryRate() const {
        return maxTotalCount_ / (double)capacity_;
    }
    double GetMemoryRate() const {
        return usedTotalCount_ / (double)capacity_;
    }
};

struct SizeFilterSegTree {
    struct Data {
        double evalScore;
        int index;
    };
    dvec<Data> buf_;
    int depth_;
    int size_;

    void Init(int size) {
        depth_ = bit_width((u32)size);
        size_ = 1 << depth_;
        buf_.assign(size_ * 2, Data{numeric_limits<double>::max(), 1});
    }

    void InitSet(double evalScore, int index) {
        buf_[size_ + index].evalScore = evalScore;
        buf_[size_ + index].index = index;
    }
    void InitUpdate(int size) {
        RFOR(k, 1, ((size_ + size - 1) >> 1) + 1) {
            buf_[k] = SizeFilterSegTree::Min(buf_[k << 1], buf_[(k << 1) ^ 1]);
        }
    }
    void Set(double evalScore, int index) {
        int p = index + size_;
        buf_[p].evalScore = evalScore;
        REP(i, depth_) {
            int s = p >> i;
            int k = s >> 1;
            buf_[k] = SizeFilterSegTree::Min(buf_[s], buf_[s ^ 1]);
        }
    }
    const Data& GetMin() const {
        return buf_[1];
    }
    static const Data& Min(const Data& a, const Data& b) {
        if (a.evalScore < b.evalScore) {
            return a;
        }
        else {
            return b;
        }
    }
};




template <class T>
struct BeamFilter {
    struct Node {
        double evalScore_;
        u64 hash_;
        T data_;
    };

    int width_;
    SizeFilterSegTree seg_;
    dvec<Node> nodePool_;
    dhmap<u64, int> hashMap_;

    void Reserve(int capacity) {
        nodePool_.reserve(capacity);
        seg_.buf_.reserve(capacity);
        hashMap_.init(capacity * 4);
    }

    void Init(int width) {
        width_ = width;
        nodePool_.clear();
        nodePool_.reserve(width);
        seg_.Init(width);
        hashMap_.init(width * 4);
    }

    bool CanPush(double evalScore) const {
        if (nodePool_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return false;
            }
        }
        return true;
    }

    T* TryPush(double evalScore, u64 hash) {
        if (nodePool_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return nullptr;
            }
            if (int* indexPtr = hashMap_.try_get(hash)) {
                int index = *indexPtr;
                if (evalScore > nodePool_[index].evalScore_) {
                    auto& node = nodePool_[index];
                    node.evalScore_ = evalScore;
                    seg_.Set(evalScore, index);
                    return &node.data_;
                }
            }
            else {
                int index = minData.index;
                auto& node = nodePool_[index];
                hashMap_.remove(node.hash_);
                node.evalScore_ = evalScore;
                node.hash_ = hash;
                hashMap_.set(hash, index);
                seg_.Set(evalScore, index);
                return &node.data_;
            }
        }
        else {
            if (int* indexPtr = hashMap_.try_get(hash)) {
                int index = *indexPtr;
                if (evalScore > nodePool_[index].evalScore_) {
                    auto& node = nodePool_[index];
                    node.evalScore_ = evalScore;
                    return &node.data_;
                }
            }
            else {
                int index = nodePool_.size();
                auto& node = nodePool_.pushR();
                node.evalScore_ = evalScore;
                node.hash_ = hash;
                hashMap_.set(hash, index);

                if (nodePool_.size() == width_) {
                    REP(index, nodePool_.size()) {
                        cauto& node = nodePool_[index];
                        seg_.InitSet(node.evalScore_, index);
                    }
                    seg_.InitUpdate(width_);
                }
                return &node.data_;
            }
        }
        return nullptr;
    }

    T* TryPush(double evalScore, u64 hash, bool& reused) {
        reused = false;

        if (nodePool_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return nullptr;
            }
            if (int* indexPtr = hashMap_.try_get(hash)) {
                int index = *indexPtr;
                if (evalScore > nodePool_[index].evalScore_) {
                    auto& node = nodePool_[index];
                    node.evalScore_ = evalScore;
                    seg_.Set(evalScore, index);
                    reused = true;
                    return &node.data_;
                }
            }
            else {
                int index = minData.index;
                auto& node = nodePool_[index];
                hashMap_.remove(node.hash_);
                node.evalScore_ = evalScore;
                node.hash_ = hash;
                hashMap_.set(hash, index);
                seg_.Set(evalScore, index);
                reused = true;
                return &node.data_;
            }
        }
        else {
            if (int* indexPtr = hashMap_.try_get(hash)) {
                int index = *indexPtr;
                if (evalScore > nodePool_[index].evalScore_) {
                    auto& node = nodePool_[index];
                    node.evalScore_ = evalScore;
                    reused = true;
                    return &node.data_;
                }
            }
            else {
                int index = nodePool_.size();
                auto& node = nodePool_.pushR();
                node.evalScore_ = evalScore;
                node.hash_ = hash;
                hashMap_.set(hash, index);

                if (nodePool_.size() == width_) {
                    REP(index, nodePool_.size()) {
                        cauto& node = nodePool_[index];
                        seg_.InitSet(node.evalScore_, index);
                    }
                    seg_.InitUpdate(width_);
                }
                return &node.data_;
            }
        }
        return nullptr;
    }
};

struct LowLink {
    int n_;
    CapArr<bool, N> jointMap_;  
    CapArr<int, N> ord_;
    CapArr<int, N> low_;

    void Init(int n) {
        n_ = n;
        jointMap_.assign(n, false);
        ord_.assign(n, -1);
        low_.assign(n, -1);
    }

    void PartialInit(const dvec<int>& vs) {
        for (int v : vs) {
            jointMap_[v] = false;
            ord_[v] = -1;
            low_[v] = -1;
        }
    }

    void Calc(const dvec<dvec<int>>& aroundMap, const NBox<int>& nsGroup, int start) {
        int num = 0;
        auto dfs = [&](auto&& dfs, int i, int parent) -> void {
            ord_[i] = num++;
            low_[i] = ord_[i];

            int childCount = 0;
            bool isJoint = false;

            for (int next : aroundMap[i]) {
                if (nsGroup[i] != nsGroup[next]) {
                    continue;
                }
                if (ord_[next] < 0) {
                    dfs(dfs, next, i);
                    chmin(low_[i], low_[next]);
                    ++childCount;

                    if (ord_[i] <= low_[next]) {
                        isJoint = true;
                    }
                }
                else if (next != parent) {
                    chmin(low_[i], ord_[next]);
                }
            }
            if (i == start) {
                isJoint = childCount >= 2;
            }
            if (isJoint) {
                jointMap_[i] = true;
            }
        };

        dfs(dfs, start, -1);
    }

    bool IsJoint(int i) const {
        return jointMap_[i];
    }
};
#pragma warning(push)
#pragma warning(disable : 5055)
#include <boost/polygon/voronoi.hpp>
#include <boost/polygon/segment_data.hpp>

#pragma warning(pop)

using namespace boost::polygon;


namespace boost {
namespace polygon {
template <>
struct geometry_concept<pint> {
    typedef point_concept type;
};

template <>
struct point_traits<pint> {
    typedef int coordinate_type;
    static inline coordinate_type get(const pint& p, orientation_2d orient) {
        return (orient == HORIZONTAL) ? p.x : p.y;
    }
};
}  
}  

void CalcDelaunayTriangle(const NBox<pint>& points, dvec<int>& edges) {
    boost::polygon::voronoi_diagram<double> vd;
    construct_voronoi(points.begin(), points.end(), &vd);

    static CheckMapD edgeCheck;
    edgeCheck.Init(edgeNo_.EdgeCount());
    static CheckMapD vertexCheck;
    vertexCheck.Init(N);

    edges.clear();
    for (auto it = vd.edges().begin(); it != vd.edges().end(); ++it) {
        if (!it->is_primary()) {
            continue;
        }

        const voronoi_diagram<double>::cell_type* cell1 = it->cell();
        const voronoi_diagram<double>::cell_type* cell2 = it->twin()->cell();

        if (!cell1 || !cell2) continue;

        int site1 = (int)cell1->source_index();
        int site2 = (int)cell2->source_index();

        int ei = edgeNo_.ToEi(site1, site2);
        if (edgeCheck.Enter(ei)) {
            vertexCheck.Check(site1);
            vertexCheck.Check(site2);
            edges.push(ei);
        }
    }

    REP(i, N) {
        if (!vertexCheck.IsChecked(i)) {
            int bestNi = -1;
            int bestD = INT_MAX;
            REP(j, N) {
                if (i == j) {
                    continue;
                }
                int d = CalcEuclideanDistance(points[i], points[j]);
                if (d < bestD) {
                    bestD = d;
                    bestNi = j;
                }
            }
            VASSERT(bestNi >= 0);
            int ei = edgeNo_.ToEi(i, bestNi);
            if (edgeCheck.Enter(ei)) {
                edges.push(ei);
            }
        }
    }
}

struct GroupDelaunay {
    dvec<int> sortedBaseEis_;      
    NBox<NBox<bool>> isBaseEdge_;  
    dvec<dvec<int>> aroundMap_;    

    void Init(const NBox<pint>& points, const dvec<double>& edgesDist) {
        CalcDelaunayTriangle(points, sortedBaseEis_);

        REP(i, N) {
            isBaseEdge_[i].fill(false);
        }
        aroundMap_.resizeR(N);
        REP(i, N) {
            aroundMap_[i].clear();
        }
        for (int ei : sortedBaseEis_) {
            auto [a, b] = edgeNo_.ToNi(ei);
            isBaseEdge_[a][b] = isBaseEdge_[b][a] = true;
            aroundMap_[a].push(b);
            aroundMap_[b].push(a);
        }

        REP(i, N - 1) {
            double bestD = 1e100;
            int bestJ = -1;
            FOR(j, i + 1, N) {
                double d = edgesDist[edgeNo_.ToEi(i, j)];
                if (d < bestD) {
                    bestD = d;
                    bestJ = j;
                }
            }
            VASSERT(bestJ >= 0);
            int j = bestJ;
            if (isBaseEdge_[i][j]) {
                continue;
            }

            isBaseEdge_[i][j] = isBaseEdge_[j][i] = true;
            aroundMap_[i].push(j);
            aroundMap_[j].push(i);
            sortedBaseEis_.push(edgeNo_.ToEi(i, j));
        }
        sort(ALL(sortedBaseEis_), [&](int ei1, int ei2) { return edgesDist[ei1] < edgesDist[ei2]; });

        REP(i, N) {
            sort(ALL(aroundMap_[i]),
                 [&](int a, int b) { return edgesDist[edgeNo_.ToEi(i, a)] < edgesDist[edgeNo_.ToEi(i, b)]; });
        }
    }
};

void FitOrderGroups(dvec<dvec<int>>& groupsVertexs) {
    dvec<dvec<int>> tmpGroupsVertexs(M);

    dvec<int> nowGis(M);
    dvec<int> targetGis(M);
    iota(ALL(nowGis), 0);
    iota(ALL(targetGis), 0);
    sort(ALL(nowGis), [&](int a, int b) { return groupsVertexs[a].size() > groupsVertexs[b].size(); });
    sort(ALL(targetGis), [&](int a, int b) { return server.Gs_[a] > server.Gs_[b]; });

    REP(i, M) {
        int nowMi = nowGis[i];
        int targetMi = targetGis[i];
        tmpGroupsVertexs[targetMi] = move(groupsVertexs[nowMi]);
    }
    groupsVertexs = move(tmpGroupsVertexs);
}

void FitGroupSizeDelauney(const NBox<pint>& points, const dvec<double>& edgesDist, const dvec<dvec<int>>& aroundMap,
                          const dvec<int>& sortedBaseEis, const NBox<NBox<bool>>& isBaseEdge,
                          dvec<dvec<int>>& groupsVertexs) {
    NBox<int> nsGroup;
    REP(mi, M) {
        for (int ni : groupsVertexs[mi]) {
            nsGroup[ni] = mi;
        }
    }


    static priority_queue<pair<double, int>> que;
    static dvec<double> dist;
    static dvec<int> prev;
    static LowLink lowlink;
    lowlink.Init(N);
    MSet updatedMis;
    updatedMis.fill(M);

    while (true) {
        dist.assign(N, 1e100);
        prev.assign(N, -1);

        for (int mi : updatedMis) {
            lowlink.PartialInit(groupsVertexs[mi]);
            lowlink.Calc(aroundMap, nsGroup, groupsVertexs[mi][0]);
        }

        REP(ni, N) {
            int mi = nsGroup[ni];
            if (groupsVertexs[mi].size() > server.Gs_[mi]) {
                if (!lowlink.jointMap_[ni]) {
                    dist[ni] = 0;
                    que.push({0, ni});
                }
            }
        }

        while (!que.empty()) {
            auto [d, ni] = que.top();
            que.pop();
            d = -d;
            if (dist[ni] < d) {
                continue;
            }
            int mi = nsGroup[ni];

            for (int nj : aroundMap[ni]) {
                int mj = nsGroup[nj];
                double cost = 0;
                if (mi != mj) {
                    if (lowlink.jointMap_[ni]) {
                        continue;
                    }
                    cost = edgesDist[edgeNo_.ToEi(ni, nj)];
                }
                double nextCost = d + cost;

                if (dist[nj] > nextCost) {
                    dist[nj] = nextCost;
                    prev[nj] = ni;
                    que.push({-dist[nj], nj});
                }
            }
        }

        int bestNi = -1;
        double bestD = 1e100;
        REP(mi, M) {
            if (groupsVertexs[mi].size() < server.Gs_[mi]) {
                for (int ni : groupsVertexs[mi]) {
                    if (dist[ni] < bestD) {
                        bestNi = ni;
                        bestD = dist[ni];
                    }
                }
            }
        }
        if (bestNi < 0) {
            break;
        }

        static dvec<int> path;
        path.clear();
        for (int ni = bestNi; ni >= 0; ni = prev[ni]) {
            path.push(ni);
        }
        VASSERT(path.size() >= 2);

        updatedMis.clear();
        RREP(i, path.size() - 1) {
            int ni = path[i];
            int nj = path[i + 1];
            int mi = nsGroup[ni];
            int mj = nsGroup[nj];
            if (mi != mj) {
                if (lowlink.jointMap_[nj]) {
                    break;
                }

                nsGroup[nj] = mi;
                int idx = groupsVertexs[mj].find(nj);
                groupsVertexs[mj].remove(idx);
                groupsVertexs[mi].push(nj);
                updatedMis.fadd(mi);
                updatedMis.fadd(mj);
            }
        }
    }

}

bool IsValidSize(const dvec<dvec<int>>& groupsVertexs) {
    REP(mi, M) {
        if (groupsVertexs[mi].size() != server.Gs_[mi]) {
            return false;
        }
    }
    return true;
}

namespace group_cs {

int mstRoot_;                   
NBox<int> mstParents_;          
NBox<dvec<int>> mstAroundMap_;  

NBox<u64> hashTbl_;  

void MakeTree(QueryAnalyzer& qa, const dvec<double>& edgesDist, const dvec<int>& sortedAllEis) {
    dvec<int> eis;
    CalcMCTWithSortedEis(edgesDist, sortedAllEis, eis);
    mstParents_.fill(-1);

    for (int ei : eis) {
        auto [a, b] = edgeNo_.ToNi(ei);
        mstAroundMap_[a].push(b);
        mstAroundMap_[b].push(a);
    }

    NBox<bool> visited;
    visited.fill(false);
    auto dfs = [&](auto&& dfs, int v) -> void {
        for (int nv : mstAroundMap_[v]) {
            if (visited[nv]) {
                continue;
            }
            visited[nv] = true;
            mstParents_[nv] = v;
            dfs(dfs, nv);
        }
    };
    mstRoot_ = 0;
    visited[mstRoot_] = true;
    dfs(dfs, mstRoot_);
}

void CalcPartial(const dvec<double>& edgesDist, int v, NBox<s16>& partialSize, NBox<double>& partialCost) {
    auto dfs = [&](auto&& dfs, int v) -> void {
        partialSize[v] = 1;  
        partialCost[v] = 0;

        for (int nv : mstAroundMap_[v]) {
            if (mstParents_[nv] != v) {
                continue;
            }
            dfs(dfs, nv);
            partialSize[v] += partialSize[nv];
            partialCost[v] += partialCost[nv];
            partialCost[v] += edgesDist[edgeNo_.ToEi(v, nv)];
        }
    };
    dfs(dfs, v);
}

void CalcPartial(const dvec<double>& edgesDist, const NBox<s16>& partialRoot, int v, NBox<s16>& partialSize,
                 NBox<double>& partialCost) {
    auto dfs = [&](auto&& dfs, int v) -> void {
        partialSize[v] = 1;  
        partialCost[v] = 0;

        for (int nv : mstAroundMap_[v]) {
            if (mstParents_[nv] != v) {
                continue;
            }
            if (partialRoot[nv] != partialRoot[v]) {
                continue;
            }
            dfs(dfs, nv);
            partialSize[v] += partialSize[nv];
            partialCost[v] += partialCost[nv];
            partialCost[v] += edgesDist[edgeNo_.ToEi(v, nv)];
        }
    };
    dfs(dfs, v);
}

void FillPartialRoot(const dvec<double>& edgesDist, NBox<s16>& partialRoot, int v, int targetRoot, int newRoot) {
    auto dfs = [&](auto&& dfs, int v) -> void {
        partialRoot[v] = newRoot;

        for (int nv : mstAroundMap_[v]) {
            if (mstParents_[nv] != v) {
                continue;
            }
            if (partialRoot[nv] == targetRoot) {
                dfs(dfs, nv);
            }
        }
    };
    dfs(dfs, v);
}

struct BeamState {
    NBox<double> partialCost_;     
    NBox<s16> partialSize_;        
    NBox<s16> partialRoot_;        
    bitset<N> isPartial_;          
    bitset<400> groupsCompleted_;  
    int allTargetSize_;            
    int targetRoot_;               

    void InitRoot(const dvec<double>& edgesDist) {
        isPartial_.reset();
        isPartial_[mstRoot_] = true;
        CalcPartial(edgesDist, mstRoot_, partialSize_, partialCost_);
        partialRoot_.fill(mstRoot_);

        groupsCompleted_.reset();
        allTargetSize_ = N;
        targetRoot_ = mstRoot_;
    }

    void Cut(const dvec<double>& edgesDist, int v) {
        isPartial_[v] = true;
        int parent = mstParents_[v];
        VASSERT(parent >= 0);
        int parentRoot = partialRoot_[parent];

        FillPartialRoot(edgesDist, partialRoot_, v, parentRoot, v);

        CalcPartial(edgesDist, partialRoot_, parentRoot, partialSize_, partialCost_);
    }

    void GetResult(dvec<dvec<int>>& groupsVertexs) const {
        NBox<int> rootsGroup;
        {
            int mi = 0;
            REP(i, N) {
                if (i == partialRoot_[i]) {
                    rootsGroup[i] = mi;
                    ++mi;
                }
            }
            VASSERT(mi == M);
        }

        groupsVertexs.resizeR(M);
        for (auto& v : groupsVertexs) {
            v.clear();
        }
        REP(i, N) {
            int mi = rootsGroup[partialRoot_[i]];
            groupsVertexs[mi].push(i);
        }
    }
};
constexpr int BeamStateSize = sizeof(BeamState);

struct BeamAction {
    int bestV_;
    int bestNextTargetRoot_;
    int bestTi_;
};

struct BeamNode {
    int ref_;  

    BeamNode* srcNode_;  
    BeamAction action_;  
    BeamState state_;    
    bool expanded_;      
    double sizeDiff_;    
    double cutSum_;      
    double evalScore_;
    u64 hash_;

    void InitRoot(const dvec<double>& edgesDist) {
        srcNode_ = nullptr;
        action_.bestV_ = -1;
        action_.bestNextTargetRoot_ = -1;
        action_.bestTi_ = -1;
        state_.InitRoot(edgesDist);
        expanded_ = true;
        sizeDiff_ = 0;
        cutSum_ = 0;
        evalScore_ = 0;
        hash_ = 0;
    }

    void InitFrom(BeamNode* srcNode, int ti, int v, int targetRoot, double sizeDiff, double cutSum, double evalScore,
                  u64 hash) {
        srcNode_ = srcNode;
        action_.bestV_ = v;
        action_.bestNextTargetRoot_ = targetRoot;
        action_.bestTi_ = ti;
        state_;
        expanded_ = false;
        sizeDiff_ = sizeDiff;
        cutSum_ = cutSum;
        evalScore_ = evalScore;
        hash_ = hash;
    }

    void Expand(const dvec<double>& edgesDist) {
        VASSERT(!expanded_);
        state_ = srcNode_->state_;
        state_.Cut(edgesDist, action_.bestV_);
        state_.targetRoot_ = action_.bestNextTargetRoot_;
        state_.groupsCompleted_[action_.bestTi_] = true;
        state_.allTargetSize_ -= server.Gs_[action_.bestTi_];
        expanded_ = true;
    }
};
constexpr int BeamNodeSize = sizeof(BeamNode);

struct BeamComp {
    bool operator()(const BeamNode* lhs, const BeamNode* rhs) const {
        return lhs->evalScore_ < rhs->evalScore_;
    }
};

void DeleteNode(ObjectPool<BeamNode>& pool, BeamNode* node) {
    --node->ref_;
    VASSERT(node->ref_ >= 0);
    if (node->ref_ == 0) {
        BeamNode* parent = node->srcNode_;
        pool.Delete(node);

        if (parent) {
            DeleteNode(pool, parent);
        }
    }
};

void RunBeam(QueryAnalyzer& qa, const NBox<pint>& points, const dvec<double>& edgesDist, const dvec<int>& sortedAllEis,
             const GroupDelaunay& delaunay, dvec<dvec<dvec<int>>>& multiGroupsVertexs, int timeLimitUs) {
    MakeTree(qa, edgesDist, sortedAllEis);

    int maxTurn = M;  

    REP(i, N) {
        hashTbl_[i] = rand_.Get64();
    }

    static ObjectPool<BeamNode> pool_;
    pool_.Init(50000);

    constexpr int BufSize = 64;
    static dvec<CapIntervalHeap<BeamNode*, BufSize, BeamComp>> turnsHeap;
    turnsHeap.resizeR(maxTurn);

    static MBox<dhmap<u64, double>> turnsHashMap;
    {
        int cap = 4000000 / (maxTurn);
        FOR(t, 1, maxTurn) {
            turnsHashMap[t].init(cap);
        }
    }

    static BeamFilter<dvec<dvec<int>>> tops;
    tops.Init(HP.G_InitialStateCount);

    auto TimeOver = [&]() {
        if (!tops.nodePool_.empty()) {
            if (timer_.ElapseTimeUs() > timeLimitUs) {
                return true;
            }
        }
        return false;
    };

    auto TryPush = [&](int turn, double evalScore, u64 hash) -> BeamNode* {
        VASSERT(turn < maxTurn);

        auto& h = turnsHeap[turn];
        if (h.size() < BufSize) {
        }
        else {
            if (h.GetMin()->evalScore_ < evalScore) {
            }
            else {
                return nullptr;
            }
        }

        auto& hm = turnsHashMap[turn];
        if (double* cachedEvalScore = hm.try_get(hash)) {
            if (*cachedEvalScore >= evalScore) {
                return nullptr;
            }
            else {
                *cachedEvalScore = evalScore;
            }
        }
        else {
            hm.set(hash, evalScore);
        }

        if (h.size() < BufSize) {
            BeamNode* node = pool_.New();
            node->ref_ = 1;
            node->evalScore_ = evalScore;  
            h.push(node);
            return node;
        }
        else {
            BeamNode* node = pool_.New();
            node->ref_ = 1;
            node->evalScore_ = evalScore;  

            BeamNode* old = h.ReplaceMin(node);
            DeleteNode(pool_, old);
            return node;
        }
    };

    {
        BeamNode* node = pool_.New();
        node->ref_ = 1;
        node->InitRoot(edgesDist);
        turnsHeap[0].push(node);
    }

    auto MakeNext = [&](int turn, BeamNode* node) {
        cauto& state = node->state_;

        if (turn + 1 == maxTurn) {
            double eval = node->evalScore_;
            u64 hash = node->hash_;
            if (tops.CanPush(eval) && !TimeOver()) {
                dvec<dvec<int>> tmpGroupsVertexs;
                state.GetResult(tmpGroupsVertexs);
                FitOrderGroups(tmpGroupsVertexs);

                FitGroupSizeDelauney(points, edgesDist, delaunay.aroundMap_, delaunay.sortedBaseEis_,
                                     delaunay.isBaseEdge_, tmpGroupsVertexs);

                if (IsValidSize(tmpGroupsVertexs)) {
                    if (dvec<dvec<int>>* ptr = tops.TryPush(eval, hash)) {
                        *ptr = move(tmpGroupsVertexs);
                    }
                }
            }
            return;
        }

        int requireCutAfterTargetSize = M - turn - 1;

        auto Push = [&](int leftSize, int ti, int targetSize, int completeSize, double cutCost, double completeCost,
                        int cutV, int nextTargetRoot) {
            if (leftSize >= requireCutAfterTargetSize) {
                double sizeDiff = square(targetSize - completeSize);
                double cutSum = node->cutSum_ + cutCost;

                int allTargetSize = state.allTargetSize_ - targetSize;
                double targetDiff = square(allTargetSize - leftSize);

                double eval = 0;
                eval -= sizeDiff * HP.GB_SizeDiff;
                eval += cutSum * 0.001 * HP.GB_CutSumRate;
                eval -= targetDiff * 0.1 * HP.GB_TargetDiff;

                u64 hash = node->hash_ ^ hashTbl_[cutV];

                if (BeamNode* next = TryPush(turn + 1, eval, hash)) {
                    next->InitFrom(node, ti, cutV, nextTargetRoot, sizeDiff, cutSum, eval, hash);
                    ++node->ref_;
                }
            }
        };

        REP(v, N) {
            if (state.isPartial_[v]) {
                continue;
            }

            if (state.partialRoot_[v] != state.targetRoot_) {
                continue;
            }

            int childSize = state.partialSize_[v];
            double childCost = state.partialCost_[v];

            int partialRoot = state.partialRoot_[v];
            int parentSize = state.partialSize_[partialRoot] - childSize;
            double parentCost = state.partialCost_[partialRoot] - childCost;

            double cutCost = edgesDist[edgeNo_.ToEi(v, mstParents_[v])];
            parentCost -= cutCost;

            REP(ti, M) {
                if (state.groupsCompleted_[ti]) {
                    continue;
                }

                int targetSize = server.Gs_[ti];

                Push(parentSize, ti, targetSize, childSize, cutCost, childCost, v, partialRoot);
                Push(childSize, ti, targetSize, parentSize, cutCost, parentCost, v, v);
            }
        }
    };

    int width = 0;
    int startTurn = 0;
    int endTurn = maxTurn;
    while (true) {
        FOR(turn, startTurn, endTurn) {
            if (turnsHeap[turn].empty()) {
                if (turn == startTurn) {
                    ++startTurn;
                }
                continue;
            }
            if (TimeOver()) {
                break;
            }

            BeamNode* node = turnsHeap[turn].GetMax();
            turnsHeap[turn].PopMax();

            if (!node->expanded_) {
                node->Expand(edgesDist);

                if (node->srcNode_) {
                    DeleteNode(pool_, node->srcNode_);
                    node->srcNode_ = nullptr;
                }
            }

            MakeNext(turn, node);

            DeleteNode(pool_, node);
        }
        ++width;

        if (TimeOver()) {
            break;
        }
        if (startTurn >= endTurn) {
            break;
        }
    }

    multiGroupsVertexs.clear();
    for (cauto& n : tops.nodePool_) {
        multiGroupsVertexs.pushR() = move(n.data_);
    }
}

}  

namespace group_prim {

struct Edge {
    int ei_;
    int vi_;
    double dist_;

    bool operator<(const Edge& rhs) const {
        return dist_ > rhs.dist_;
    }
};

bool Run(QueryAnalyzer& qa, const NBox<pint>& points, const dvec<double>& edgesDist, const GroupDelaunay& delaunay,
         dvec<dvec<int>>& groupsVertexs, int timeLimitUs) {
    groupsVertexs.clear();

    cauto& aroundMap = delaunay.aroundMap_;

    auto gs = server.Gs_;
    sort(ALL(gs), [](int a, int b) { return a > b; });

    static PriorityQueue<Edge> que;

    NBox<bool> used = {};
    REP(loopCount, M) {
        int targetSize = gs[loopCount];

        int bestStart = -1;
        static dvec<int> bestVs;
        bestVs.clear();
        double bestCost = 1e100;
        REP(start, N) {
            if (used[start]) {
                continue;
            }

            static dvec<int> vs;
            double cost = 0;
            if (targetSize == 1) {
                vs.clear();
                vs.push(start);
            }
            else {
                que.Clear();
                NBox<bool> visited = {};
                {
                    for (int n : aroundMap[start]) {
                        if (used[n] || visited[n]) {
                            continue;
                        }
                        double dist = edgesDist[edgeNo_.ToEi(start, n)];
                        que.push(Edge{edgeNo_.ToEi(start, n), n, dist});
                    }
                }

                vs.clear();
                vs.push(start);
                visited[start] = true;

                while (!que.empty()) {
                    auto node = que.top();
                    que.pop();
                    int v = node.vi_;
                    if (visited[v]) {
                        continue;
                    }

                    vs.push(v);
                    visited[v] = true;
                    cost += node.dist_;

                    if (vs.size() >= targetSize) {
                        break;
                    }

                    for (int n : aroundMap[v]) {
                        if (used[n] || visited[n]) {
                            continue;
                        }
                        double dist = edgesDist[edgeNo_.ToEi(v, n)];
                        que.push(Edge{edgeNo_.ToEi(v, n), n, dist});
                    }
                }
            }

            if (vs.size() == targetSize) {
                if (cost < bestCost) {
                    bestCost = cost;
                    bestStart = start;
                    bestVs = vs;
                }
            }
            else {
                if (vs.size() > bestVs.size()) {
                    bestStart = start;
                    bestVs = vs;
                }
            }
        }

        groupsVertexs.pushR() = bestVs;
        for (int v : bestVs) {
            VASSERT(!used[v]);
            used[v] = true;
        }
    }

    NBox<int> nsGroup;
    nsGroup.fill(-1);
    REP(g, groupsVertexs.size()) {
        for (int v : groupsVertexs[g]) {
            nsGroup[v] = g;
        }
    }

    {
        que.Clear();
        NBox<bool> visited = {};  

        REP(start, N) {
            if (used[start]) {
                for (int n : aroundMap[start]) {
                    if (used[n] || visited[n]) {
                        continue;
                    }
                    double dist = edgesDist[edgeNo_.ToEi(start, n)];
                    que.push(Edge{edgeNo_.ToEi(start, n), n, dist});
                }
            }
        }

        while (!que.empty()) {
            auto node = que.top();
            que.pop();
            int v = node.vi_;

            if (visited[v]) {
                continue;
            }

            auto [a, b] = edgeNo_.ToNi(node.ei_);
            int groupV = (a == v) ? b : a;
            int mi = nsGroup[groupV];
            VASSERT(mi >= 0);
            groupsVertexs[mi].push(v);
            nsGroup[v] = mi;
            visited[v] = true;

            for (int n : aroundMap[v]) {
                if (used[n] || visited[n]) {
                    continue;
                }
                double dist = edgesDist[edgeNo_.ToEi(v, n)];
                que.push(Edge{edgeNo_.ToEi(v, n), n, dist});
            }
        }
    }

    FitOrderGroups(groupsVertexs);

    FitGroupSizeDelauney(points, edgesDist, delaunay.aroundMap_, delaunay.sortedBaseEis_, delaunay.isBaseEdge_,
                         groupsVertexs);

    if (!IsValidSize(groupsVertexs)) {
        return false;
    }
    return true;
}

}  

template <class T>
struct SizeFilter {
    struct Node {
        double evalScore_;
        T data_;
    };

    int width_;
    SizeFilterSegTree seg_;
    dvec<Node> nodePool_;

    void Reserve(int capacity) {
        nodePool_.reserve(capacity);
        seg_.buf_.reserve(capacity);
    }

    void Init(int width) {
        width_ = width;
        nodePool_.clear();
        nodePool_.reserve(width);
        seg_.Init(width);
    }


    bool CanPush(double evalScore) const {
        if (nodePool_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return false;
            }
            return true;
        }
        else {
            return true;
        }
    }

    T* TryPush(double evalScore) {
        if (nodePool_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return nullptr;
            }

            int index = minData.index;
            auto& node = nodePool_[index];
            node.evalScore_ = evalScore;
            seg_.Set(evalScore, index);
            return &node.data_;
        }
        else {
            int index = nodePool_.size();
            auto& node = nodePool_.pushR();
            node.evalScore_ = evalScore;

            if (nodePool_.size() == width_) {
                REP(index, nodePool_.size()) {
                    cauto& node = nodePool_[index];
                    seg_.InitSet(node.evalScore_, index);
                }
                seg_.InitUpdate(width_);
            }
            return &node.data_;
        }
        return nullptr;
    }

    T* TryPush(double evalScore, bool& reused) {
        reused = false;
        if (nodePool_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return nullptr;
            }

            int index = minData.index;
            auto& node = nodePool_[index];
            node.evalScore_ = evalScore;
            seg_.Set(evalScore, index);
            reused = true;
            return &node.data_;
        }
        else {
            int index = nodePool_.size();
            auto& node = nodePool_.pushR();
            node.evalScore_ = evalScore;

            if (nodePool_.size() == width_) {
                REP(index, nodePool_.size()) {
                    cauto& node = nodePool_[index];
                    seg_.InitSet(node.evalScore_, index);
                }
                seg_.InitUpdate(width_);
            }
            return &node.data_;
        }
        return nullptr;
    }
};


namespace group_sa {

NBox<pint> points_;
dvec<double> edgesDist_;
dvec<int> sortedBaseEis_;      
NBox<NBox<bool>> isBaseEdge_;  
dvec<dvec<int>> aroundMap_;    

struct Pack {
    NBox<int> nsGroup_;  
    dvec<dvec<int>> groupsVertexs_;
    dvec<double> groupsCost_;
    double totalCost_;
};

struct State {
    NBox<int> nsGroup_;  
    dvec<dvec<int>> groupsVertexs_;
    dvec<double> groupsCost_;
    double totalCost_;

    void Init(const dvec<dvec<int>>& groupsVertexs) {
        VASSERT(groupsVertexs.size() == M);
        totalCost_ = 0;
        groupsVertexs_ = groupsVertexs;
        groupsCost_.resizeR(M);
        REP(mi, groupsVertexs_.size()) {
            for (int v : groupsVertexs_[mi]) {
                nsGroup_[v] = mi;
            }

            dvec<int> eis;
            double cost = CalcMCTWithEdgeMatrixVertexs(edgesDist_, isBaseEdge_, groupsVertexs_[mi], eis);
            groupsCost_[mi] = cost;
            totalCost_ += cost;


        }
    }

    s64 RawScore() const {
        return -s64(totalCost_);
    }
    double EvalScore() const {
        return -totalCost_;
    }

    SAResult Update(SAChecker& checker, double timeRate, int& ti) {
        int mA_v1 = rand_(N);
        int mA = nsGroup_[mA_v1];
        int mB_v1 = -1;
        for (int v : aroundMap_[mA_v1]) {
            if (nsGroup_[v] != mA) {
                mB_v1 = v;
                break;
            }
        }
        if (mB_v1 < 0) {
            return SAResult::Fail;
        }
        int mB = nsGroup_[mB_v1];

        if (groupsVertexs_[mA].size() == 1 && groupsVertexs_[mB].size() == 1) {
            return SAResult::Fail;
        }

        int mA_v2 = -1;
        int mB_v2 = -1;
        if (groupsVertexs_[mA].size() == 1 || groupsVertexs_[mB].size() == 1) {
            if (groupsVertexs_[mA].size() == 1) {
                mA_v2 = mA_v1;
                mB_v2 = groupsVertexs_[mB][rand_(groupsVertexs_[mB].size())];
            }
            else {
                mA_v2 = groupsVertexs_[mA][rand_(groupsVertexs_[mA].size())];
                mB_v2 = mB_v1;
            }
        }
        else {
            int startEi = rand_(sortedBaseEis_.size());
            REP(i, sortedBaseEis_.size()) {
                int ei = (startEi + i) % sortedBaseEis_.size();
                auto [a, b] = edgeNo_.ToNi(sortedBaseEis_[ei]);
                if (a == mA_v1 || a == mB_v1 || b == mA_v1 || b == mB_v1) {
                    continue;
                }
                if (mA == nsGroup_[a] && mB == nsGroup_[b]) {
                    mA_v2 = a;
                    mB_v2 = b;
                    break;
                }
                else if (mA == nsGroup_[b] && mB == nsGroup_[a]) {
                    mA_v2 = b;
                    mB_v2 = a;
                    break;
                }
            }
            if (mA_v2 < 0) {
                return SAResult::Fail;
            }
        }

        static dvec<int> vertexs;
        vertexs.clear();
        for (int v : groupsVertexs_[mA]) {
            if (v != mA_v1) {
                vertexs.push(v);
            }
        }
        vertexs.push(mB_v2);
        static dvec<int> eis;
        double mA_cost = CalcMCTWithEdgeMatrixVertexs(edgesDist_, isBaseEdge_, vertexs, eis);
        if (eis.size() != vertexs.size() - 1) {
            return SAResult::Fail;
        }

        vertexs.clear();
        for (int v : groupsVertexs_[mB]) {
            if (v != mB_v2) {
                vertexs.push(v);
            }
        }
        vertexs.push(mA_v1);
        double mB_cost = CalcMCTWithEdgeMatrixVertexs(edgesDist_, isBaseEdge_, vertexs, eis);
        if (eis.size() != vertexs.size() - 1) {
            return SAResult::Fail;
        }

        double newTotalCost = totalCost_ - groupsCost_[mA] - groupsCost_[mB] + mA_cost + mB_cost;

        if (checker.CheckAccept(-newTotalCost)) {
            nsGroup_[mA_v1] = mB;
            nsGroup_[mB_v2] = mA;
            groupsVertexs_[mA].remove(groupsVertexs_[mA].find(mA_v1));
            groupsVertexs_[mA].push(mB_v2);
            groupsVertexs_[mB].remove(groupsVertexs_[mB].find(mB_v2));
            groupsVertexs_[mB].push(mA_v1);
            groupsCost_[mA] = mA_cost;
            groupsCost_[mB] = mB_cost;
            totalCost_ = newTotalCost;

            return SAResult::Ok;
        }
        else {
            return SAResult::Ng;
        }
    }

    void Save(Pack& pack) const {
        pack.nsGroup_ = nsGroup_;
        pack.groupsVertexs_ = groupsVertexs_;
        pack.groupsCost_ = groupsCost_;
        pack.totalCost_ = totalCost_;
    }

    void Load(const Pack& pack) {
        nsGroup_ = pack.nsGroup_;
        groupsVertexs_ = pack.groupsVertexs_;
        groupsCost_ = pack.groupsCost_;
        totalCost_ = pack.totalCost_;
    }
};
static constexpr int StateSize = sizeof(State);

void Run(QueryAnalyzer& qa, const NBox<pint>& points, const dvec<double>& edgesDist, const GroupDelaunay& delaunay,
         const dvec<dvec<dvec<int>>>& multiGroupsVertexs, dvec<dvec<int>>& groupsVertexs, int timeLmitUs) {
    VASSERT(M > 1);

    {
        points_ = points;
        edgesDist_ = edgesDist;
        sortedBaseEis_ = delaunay.sortedBaseEis_;
        isBaseEdge_ = delaunay.isBaseEdge_;
        aroundMap_ = delaunay.aroundMap_;
    }

    dvec<State> states(HP.G_InitialStateCount);
    REP(i, states.size()) {
        states[i].Init(multiGroupsVertexs[i % multiGroupsVertexs.size()]);
    }


    SA<Pack> sa;
    Config config;

    config.update_tmp_mask = (1 << HP.G_UpdateTmpMaskBit) - 1;

    config.temp_type = HP.G_TempType;
    config.temp_pow = HP.G_TempPow;

    config.rollback_start_time_rate = HP.G_RollbackStartRate;
    config.rollback_count = HP.G_RollbackCount;
    config.rollback_mul = HP.G_RollbackNextMulti;
    config.min_rollback_count = HP.G_MinRollbackCount;

    if (M >= 2) {
        sa.Run<State>(states, timeLmitUs, HP.G_StartTemp, HP.G_EndTemp, config);
        cauto& best = sa.GetBest();
        groupsVertexs.resizeR(M);
        for (auto& vs : groupsVertexs) {
            vs.clear();
        }
        REP(i, N) {
            int gi = best.best_pack.nsGroup_[i];
            groupsVertexs[gi].push(i);
        }
    }
    else {
        groupsVertexs.resizeR(M);
        groupsVertexs[0].resizeR(N);
        iota(ALL(groupsVertexs[0]), 0);
    }

}

}  

void EstimatePointTriangleUseQueTime(int timeLimitUs, QueryAnalyzer& qa, int queryCount, NBox<pint>& points,
                                     dvec<double>& edgesDist) {
    const int SampleCount = HP.EP_SampleQueCount;
    double sampleWeight = 1;
    double weightScale = HP.EP_WeightScale;

    static dvec<SamplingQue> sampleQues(SampleCount);
    REP(si, SampleCount) {
        sampleQues[si].Init();
        Sampling(HP.EP_FirstSample / SampleCount, sampleQues[si], sampleWeight);
    }
    sampleWeight *= weightScale;
    weightScale *= HP.EP_WeightScaleScale;

    auto GetMeanPoints = [&](NBox<pint>& points) {
        double weightSum = 0;
        NBox<pdouble> pointSums;
        pointSums.fill(pdouble{});
        REP(si, SampleCount) {
            sampleQues[si].AddPoints(pointSums);
            weightSum += sampleQues[si].weightSum_;
        }
        REP(i, N) {
            points[i].x = (int)round(pointSums[i].x / weightSum);
            points[i].y = (int)round(pointSums[i].y / weightSum);
        }
    };

    auto GetMeanDist = [&](dvec<double>& edgesDist) {
        double weightSum = 0;
        edgesDist.assign(edgeNo_.EdgeCount(), 0.0);
        REP(si, SampleCount) {
            sampleQues[si].AddDists(edgesDist);
            weightSum += sampleQues[si].weightSum_;
        }
        for (int ei : edgeNo_.nearEis_) {
            edgesDist[ei] /= weightSum;
        }
    };
    GetMeanDist(edgesDist);

    QArr<int> queryCountTbl;
    {
        int queryLeft = queryCount;
        int oneLoopQueryCount = max(1, int(N / (double)L * HP.EP_OneLoopQueryRate));
        int saCount = (queryLeft + oneLoopQueryCount - 1) / oneLoopQueryCount;

        REP(i, saCount) {
            int qc = min(oneLoopQueryCount, queryLeft);
            queryCountTbl.push(qc);
            queryLeft -= qc;
        }
    }


    struct Node {
        int ei;  
        int vi;  
        double eval;
        bool operator<(const Node& rhs) const {
            return eval < rhs.eval;
        }
    };
    static PriorityQueue<Node> que;
    que.Clear();
    NBox<bool> used;
    NBox<double> evals;
    LArr<int> query;

    NBox<int> vertexUseCount = {};
    static dvec<int> edgeUseCount;
    edgeUseCount.assign(edgeNo_.EdgeCount(), 0);

    int saCount = queryCountTbl.size();
    int maxStep = saCount * SampleCount;
    int curStep = 0;

    REP(loop, saCount) {
        int qc = queryCountTbl[loop];
        VASSERT(qc > 0);

        REP(qi, qc) {
            int startEi = -1;
            {
                int start = 0;
                REP(i, N) {
                    if (vertexUseCount[i] < vertexUseCount[start]) {
                        start = i;
                    }
                }

                int bestEi = -1;
                double bestEval = -1e100;
                for (int j : edgeNo_.nearArounds_[start]) {
                    int ei = edgeNo_.ToEi(start, j);
                    double eval = 0;
                    eval -= edgesDist[edgeNo_.ToEi(start, j)] * HP.EP_FirstEdgeDistRate;
                    eval -= vertexUseCount[j] * HP.EP_FirstVertexUseRate;
                    eval -= edgeUseCount[ei] * HP.EP_FirstEdgeUseRate;

                    if (eval > bestEval) {
                        bestEval = eval;
                        bestEi = ei;
                    }
                }
                VASSERT(bestEi >= 0);
                startEi = bestEi;
            }
            que.Clear();
            used.fill(false);
            evals.fill(-1e100);  
            query.clear();

            auto AddEdge = [&](int ei) {
                auto [a, b] = edgeNo_.ToNi(ei);
                REP(i, N) {
                    if (i == a || i == b) {
                        continue;
                    }
                    if (used[i]) {
                        continue;
                    }

                    int ei1 = edgeNo_.ToEi(a, i);
                    int ei2 = edgeNo_.ToEi(b, i);
                    int ei3 = edgeNo_.ToEi(a, b);

                    VASSERT(edgeNo_.IsNear(ei3));
                    if (!edgeNo_.IsNear(ei1) || !edgeNo_.IsNear(ei2)) {
                        continue;
                    }

                    double d1 = edgesDist[ei1];
                    double d2 = edgesDist[ei2];
                    double d3 = edgesDist[ei3];
                    double eval = 0;

                    {
                        double mean = (d1 + d2 + d3) / 3.0;
                        double var = (square(d1 - mean) + square(d2 - mean) + square(d3 - mean)) / 3.0;
                        eval -= var * HP.EP_VarRate;
                    }


                    eval -= vertexUseCount[i] * HP.EP_VertexUseRate;

                    eval -= edgeUseCount[ei1] * HP.EP_EdgeUseRate;
                    eval -= edgeUseCount[ei2] * HP.EP_EdgeUseRate;

                    int hasConstraintCount = 0;
                    if (qa.HasConstraintEdges(ei1, ei2)) {
                        ++hasConstraintCount;
                    }
                    if (qa.HasConstraintEdges(ei1, ei3)) {
                        ++hasConstraintCount;
                    }
                    if (qa.HasConstraintEdges(ei2, ei3)) {
                        ++hasConstraintCount;
                    }
                    eval -= hasConstraintCount * HP.EP_EdgeConstraintScore;



                    if (qa.IsNouseEdge(ei1)) {
                        eval -= HP.EP_NoUseEdgeScore;
                    }
                    if (qa.IsNouseEdge(ei2)) {
                        eval -= HP.EP_NoUseEdgeScore;
                    }

                    if (qa.IsAllUseEdge(ei1)) {
                        eval -= HP.EP_AllUseEdgeScore;
                    }
                    if (qa.IsAllUseEdge(ei2)) {
                        eval -= HP.EP_AllUseEdgeScore;
                    }

                    if (eval > evals[i]) {
                        evals[i] = eval;
                        que.push(Node{ei, i, eval});
                    }
                }
            };
            auto AddVertex = [&](int ei, int vi) {
                auto [a, b] = edgeNo_.ToNi(ei);
                AddEdge(edgeNo_.ToEi(a, vi));
                AddEdge(edgeNo_.ToEi(b, vi));
            };

            {
                auto [a, b] = edgeNo_.ToNi(startEi);
                query.push(a);
                query.push(b);
                used[a] = true;
                used[b] = true;
                vertexUseCount[a]++;
                vertexUseCount[b]++;
                edgeUseCount[startEi]++;
            }

            AddEdge(startEi);
            while (!que.empty()) {
                auto [ei, vi, eval] = que.top();
                que.pop();
                if (used[vi]) {
                    continue;
                }

                used[vi] = true;
                query.push(vi);
                vertexUseCount[vi]++;

                {
                    auto [a, b] = edgeNo_.ToNi(ei);
                    for (int addEi : {edgeNo_.ToEi(a, vi), edgeNo_.ToEi(b, vi)}) {
                        ++edgeUseCount[addEi];

                    }
                }

                if (query.size() >= L) {
                    break;
                }
                AddVertex(ei, vi);
            }

            LArr<pint> response;
            server.Query(query, response);
            qa.Add(query, response);

        }

        double saRate = loop / (double)(saCount - 1);
        double timeRate = HP.EP_TimeRateStart + HP.EP_TimeRateEnd * saRate;

        REP(si, SampleCount) {
            int now = timer_.ElapseTimeUs();
            int timeLeft = timeLimitUs - now;
            int stepLeft = maxStep - curStep;
            int saTime = int(timeLeft / stepLeft * timeRate);
            chmax(saTime, 1);

            point_hc::SamplingTime(now + saTime, qa, sampleQues[si], HP.EP_VertexIter, sampleWeight);
            ++curStep;
        }
        GetMeanDist(edgesDist);
        sampleWeight *= weightScale;
        weightScale *= HP.EP_WeightScaleScale;
    }
    VASSERT(curStep == maxStep);

    GetMeanPoints(points);

    REP(ei, edgeNo_.EdgeCount()) {
        if (!edgeNo_.IsNear(ei)) {
            VASSERT(edgeUseCount[ei] == 0);
            auto [a, b] = edgeNo_.ToNi(ei);
            edgesDist[ei] = CalcEuclideanDistance(points[a], points[b]);
        }
        else {
        }
    }

}

struct Solver {
    void Run() {
        static QueryAnalyzer qa;
        qa.Init();

        double timeWeightSum = HP.EstimatePointTimeWeight + HP.FirstGroupTimeWeight + HP.UpdateGroupTimeWeight;

        int estimatePointTimeLimit = int(TIME_LIMIT_US * HP.EstimatePointTimeWeight / timeWeightSum);
        if (M == 1) {
            estimatePointTimeLimit = int(TIME_LIMIT_US * 0.95);  
        }

        NBox<pint> points;
        static dvec<double> edgesDist;
        EstimatePoint(estimatePointTimeLimit, qa, Q, points, edgesDist);

        dvec<int> sortedAllEis(edgeNo_.EdgeCount());
        iota(ALL(sortedAllEis), 0);
        SortEdges(qa, edgesDist, sortedAllEis);

        dvec<dvec<int>> groupsVertexs;
        if (M == 1) {
            groupsVertexs.resizeR(1);
            groupsVertexs[0].resizeR(N);
            iota(ALL(groupsVertexs[0]), 0);
        }
        else {
            GroupDelaunay groupDelaunay;
            groupDelaunay.Init(points, edgesDist);

            dvec<dvec<dvec<int>>> multiGroupsVertexs;
            DivideGroupFirst(
                int(TIME_LIMIT_US * (HP.EstimatePointTimeWeight + HP.FirstGroupTimeWeight) / timeWeightSum), qa, points,
                edgesDist, sortedAllEis, groupDelaunay, multiGroupsVertexs);
            DivideGroupUpdate(TIME_LIMIT_US, qa, points, edgesDist, groupDelaunay, multiGroupsVertexs, groupsVertexs);
        }

        static IOResult result;
        MakeResult(qa, edgesDist, sortedAllEis, groupsVertexs, result);

        server.Output(result);

    }

    void EstimatePoint(int timeLimitUs, QueryAnalyzer& qa, int queryCount, NBox<pint>& points,
                       dvec<double>& edgesDist) {
        EstimatePointTriangleUseQueTime(timeLimitUs, qa, queryCount, points, edgesDist);

    }

    void SortEdges(QueryAnalyzer& qa, const dvec<double>& edgesDist, dvec<int>& eis) {
        if (HP.UseCompSort) {
            static dvec<dvec<int>> largers;
            qa.graph_.CalcLargeList(largers);
            stable_sort(ALL(eis), [&](int a, int b) {
                if (largers[a].find(b) >= 0) {
                    return true;
                }
                if (largers[b].find(a) >= 0) {
                    return false;
                }
                return edgesDist[a] < edgesDist[b];
            });
        }
        else {
            sort(ALL(eis), [&](int a, int b) { return edgesDist[a] < edgesDist[b]; });
        }


    }

    void DivideGroupFirst(int timeLimitUs, QueryAnalyzer& qa, const NBox<pint>& points, const dvec<double>& edgesDist,
                          const dvec<int>& sortedAllEis, const GroupDelaunay& delaunay,
                          dvec<dvec<dvec<int>>>& multiGroupsVertexs) {
        timer_.StartUs(timeLimitUs);
        if (M < 250) {
            group_cs::RunBeam(qa, points, edgesDist, sortedAllEis, delaunay, multiGroupsVertexs, timeLimitUs);
        }
        else {
            multiGroupsVertexs.resizeR(1);
            if (!group_prim::Run(qa, points, edgesDist, delaunay, multiGroupsVertexs[0], timeLimitUs)) {
                group_cs::RunBeam(qa, points, edgesDist, sortedAllEis, delaunay, multiGroupsVertexs, timeLimitUs);
            }
        }
    }

    void DivideGroupUpdate(int timeLimitUs, QueryAnalyzer& qa, const NBox<pint>& points, const dvec<double>& edgesDist,
                           const GroupDelaunay& delaunay, const dvec<dvec<dvec<int>>>& multiGroupsVertexs,
                           dvec<dvec<int>>& groupsVertexs) {
        timer_.StartUs(timeLimitUs);
        group_sa::Run(qa, points, edgesDist, delaunay, multiGroupsVertexs, groupsVertexs, timeLimitUs);
    }

    void MakeResult(QueryAnalyzer& qa, const dvec<double>& edgesDist, const dvec<int>& sortedAllEis,
                    const dvec<dvec<int>>& groupsVertexs, IOResult& result) {
        result.groupsVertexs_.resize(M);
        result.groupsEdges_.resize(M);
        REP(mi, M) {
            result.groupsVertexs_[mi].clear();
            result.groupsEdges_[mi].clear();

            for (int v : groupsVertexs[mi]) {
                result.groupsVertexs_[mi].push(v);
            }
        }

        NBox<int> nsGroup;
        REP(i, M) {
            for (int ni : groupsVertexs[i]) {
                nsGroup[ni] = i;
            }
        }

        UnionFindS<N> uf;
        uf.Init(N);
        for (int ei : sortedAllEis) {
            auto [a, b] = edgeNo_.ToNi(ei);
            if (nsGroup[a] != nsGroup[b]) {
                continue;
            }
            if (uf.IsReachable(a, b)) {
                continue;
            }
            uf.Join(a, b);
            result.groupsEdges_[nsGroup[a]].push(pint{a, b});
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
