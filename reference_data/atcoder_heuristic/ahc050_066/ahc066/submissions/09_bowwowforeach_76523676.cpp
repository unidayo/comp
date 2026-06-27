#define FULL_LIB 1
#define TIME_LIMIT (1940)
#define TIME_LIMIT_US (TIME_LIMIT * 1000)

#define BUILD_MODE_MAIN 0        
#define BUILD_MODE_EVAL 1        
#define BUILD_MODE_VALID_EVAL 2  
#define BUILD_MODE_PERF 3        
#define BUILD_MODE_OPTUNE 4      
#define BUILD_MODE_UNIT_TEST 5   
#define BUILD_MODE_SUBMIT 6      
#define BUILD_MODE_CODETEST 7    


#define CFG_MODE_MAIN 0

#define CFG_MODE_EVAL 0

#define CFG_MODE_PERF 0

#define CFG_MODE_OPTUNE 0

#define CFG_MODE_UNIT_TEST 0

#define CFG_IO_FILE 0

#define CFG_VALID 0

#define CFG_OUT_PERF 0

#define CFG_OUT_MEM 0

#define CFG_OUT_VIS 0

#define CFG_FIX_RESULT 0



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

template <class T>
void InstanceRun(int argc, const char* argv[]) {
    T* m = new T;
    m->Run(argc, argv);
    quick_exit(0);
}

struct Main;

signed main(int argc, const char* argv[]) {
    std::cin.tie(0);
    std::ios::sync_with_stdio(0);

#ifdef _MSC_VER
    {
        THREAD_POWER_THROTTLING_STATE t;
        std::memset(&t, 0, sizeof(t));
        t.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
        t.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
        t.StateMask = 0;
        SetThreadInformation(GetCurrentThread(), ThreadPowerThrottling, &t, sizeof(t));
    }
#endif
    InstanceRun<Main>(argc, argv);
}

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


#define cauto const auto

#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;


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

inline pdouble ToDouble(const pint& p) {
    return pdouble{double(p.x), double(p.y)};
}
inline pint round(const pdouble& d) {
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

using Clock = chrono::steady_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

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
        return (int)chrono::duration_cast<chrono::microseconds>(Clock::now() - startTime_).count();
    }

    void SetElapseTimeMs(int ms) {
        auto now = Clock::now();
        auto limit = endTime_ - startTime_;
        startTime_ = now - chrono::milliseconds(ms);
        endTime_ = startTime_ + limit;
    }

    inline int LeftToUS(const TimePoint& limit) const {
        return (int)chrono::duration_cast<chrono::microseconds>(limit - Clock::now()).count();
    }

    inline double NowRate() const {
        return (Clock::now() - startTime_).count() / (double)(endTime_ - startTime_).count();
    }

    inline double CalcRate(const TimePoint& start) const {
        return (Clock::now() - start).count() / (double)(endTime_ - start).count();
    }

    inline TimePoint Now() const {
        return Clock::now();
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
    return Clock::now();
}
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

    void SetDeviceRandom() {
        random_device rd;
        uint64_t s = (uint64_t(rd()) << 32) ^ uint64_t(rd());
        if (s == 0) {
            s = 0x123456789abcdef;
        }
        x = s;
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

    inline double GetDoubleRange(double l, double r) {
        return l + GetDouble() * (r - l);
    }
};
using Rng = Xor64;







template <class T, int CAP>
class CapArr {
   private:
    static_assert(is_trivially_copyable<T>::value);
    using SizeType = typename conditional<CAP <= 255, u8, u32>::type;

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
    void push_back(const T& e) {
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

    [[nodiscard]] T& insertR(int index) {
        if (index == size_) {
            size_ += 1;
        }
        else {
            memmove(data() + index + 1, data() + index, sizeof(T) * (size_ - index));
            size_ += 1;
        }
        return array_[index];
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


#include <cstddef>
#include <iterator>

template <class T, int CAPACITY>
struct CapacityRingDeque {
   private:
    array<T, CAPACITY> buf_;
    int head_ = 0;  
    int tail_ = 0;  
    int count_ = 0;

   public:
    bool operator==(const CapacityRingDeque<T, CAPACITY>& r) const {
        if (count_ != r.count_) {
            return false;
        }

        int j = head_;
        int rj = r.head_;

        REP(i, count_) {
            if (!(buf_[j] == r.buf_[rj])) {
                return false;
            }
            ++j;
            if (j >= CAPACITY) {
                j = 0;
            }
            ++rj;
            if (rj >= CAPACITY) {
                rj = 0;
            }
        }
        return true;
    }

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
        VASSERT(count_ > 0);
        return buf_[head_];
    }
    inline const T& back() const {
        VASSERT(count_ > 0);
        if (tail_ == 0) {
            return buf_[CAPACITY - 1];
        }
        return buf_[tail_ - 1];
    }
    inline T& front() {
        VASSERT(count_ > 0);
        return buf_[head_];
    }
    inline T& back() {
        VASSERT(count_ > 0);
        if (tail_ == 0) {
            return buf_[CAPACITY - 1];
        }
        return buf_[tail_ - 1];
    }

    inline const T& operator[](int index) const {
        VASSERT(index >= 0);
        VASSERT(index < count_);

        int realIndex = head_ + index;
        if (realIndex >= CAPACITY) {
            realIndex -= CAPACITY;
        }
        return buf_[realIndex];
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

    void GetRawAccess(const array<T, CAPACITY>*& buf, int& head, int& tail, int& count) const {
        buf = &buf_;
        head = head_;
        tail = tail_;
        count = count_;
    }

    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        CapacityRingDeque* deque = nullptr;
        int index = 0;  

        reference operator*() const {
            int realIndex = deque->head_ + index;
            if (realIndex >= CAPACITY) {
                realIndex -= CAPACITY;
            }
            return deque->buf_[realIndex];
        }

        pointer operator->() const {
            return &(**this);
        }

        iterator& operator++() {
            ++index;
            return *this;
        }

        iterator operator++(int) {
            iterator old = *this;
            ++(*this);
            return old;
        }

        bool operator==(const iterator& r) const {
            return index == r.index && deque == r.deque;
        }

        bool operator!=(const iterator& r) const {
            return !(*this == r);
        }
    };

    struct const_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const CapacityRingDeque* deque = nullptr;
        int index = 0;  

        reference operator*() const {
            int realIndex = deque->head_ + index;
            if (realIndex >= CAPACITY) {
                realIndex -= CAPACITY;
            }
            return deque->buf_[realIndex];
        }

        pointer operator->() const {
            return &(**this);
        }

        const_iterator& operator++() {
            ++index;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator old = *this;
            ++(*this);
            return old;
        }

        bool operator==(const const_iterator& r) const {
            return index == r.index && deque == r.deque;
        }

        bool operator!=(const const_iterator& r) const {
            return !(*this == r);
        }
    };

    iterator begin() {
        return iterator{this, 0};
    }

    iterator end() {
        return iterator{this, count_};
    }

    const_iterator begin() const {
        return const_iterator{this, 0};
    }

    const_iterator end() const {
        return const_iterator{this, count_};
    }

    const_iterator cbegin() const {
        return const_iterator{this, 0};
    }

    const_iterator cend() const {
        return const_iterator{this, count_};
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
    void reset() {
        Clear();
    }

    bool IsChecked(int i) const {
        return checked_[i] == mark_;
    }
    bool test(int i) const {
        return IsChecked(i);
    }

    void Check(int i) {
        checked_[i] = mark_;
    }
    void set(int i) {
        Check(i);
    }

    void Reset(int i) {
        checked_[i] = mark_ - 1;
    }
    void reset(int i) {
        Reset(i);
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
struct CheckMapDataD {
   private:
    vector<T> data_;
    vector<u32> checked_;
    u32 mark_ = 1;

   public:
    void Init(int size) {
        SetSize(size);
        Clear();
    }

    void SetSize(int size) {
        checked_.resize(size, mark_);
        data_.resize(size);
    }

    void Clear() {
        ++mark_;

        if (mark_ == 0) {
            checked_.assign(checked_.size(), 0);
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
    const T& operator[](int i) const {
        VASSERT(checked_[i] == mark_);
        return data_[i];
    }
    T& operator[](int i) {
        VASSERT(checked_[i] == mark_);
        return data_[i];
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

    void push_back(const T& v) {
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
    void resize(int size, const T& v) {
        int srcSize = size_;
        resizeR(size);
        for (int i = srcSize; i < size; ++i) {
            ptr_[i] = v;
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

    void removeSwap(int idx) {
        VASSERT(idx >= 0 && idx < size_);
        if (idx + 1 < size_) {
            swap(ptr_[idx], ptr_[size_ - 1]);
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
    uint64_t* m_states = nullptr;  
    uint32_t m_mask = 0;

    uint32_t m_count = 0;

    dhmap(const dhmap&) = delete;
    void operator=(const dhmap&) = delete;
    dhmap() {
    }

    ~dhmap() {
        free(m_keys);
        free(m_values);
        free(m_states);
    }

    inline uint32_t NextIndex(uint32_t i) const {
        return (i + 1) & m_mask;
    }

    inline uint32_t HomeIndex(K key) const {
        return key & m_mask;
    }

    inline uint32_t ProbeDistance(uint32_t index, K key) const {
        return (index - HomeIndex(key)) & m_mask;
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
        m_states = (uint64_t*)calloc((m_capacity + 63) / 64, sizeof(uint64_t));
        m_count = 0;
    }

    void clear() {
        if (m_capacity) {
            memset(m_states, 0, (m_capacity + 63) / 64 * sizeof(uint64_t));
        }
        m_count = 0;
    }

    bool Find(K key, u32& index) const {
        VASSERT(m_count < m_capacity);

        uint32_t i = HomeIndex(key);
        for (;;) {
            if ((m_states[i >> 6] & (1ULL << (i & 63))) == 0) {
                index = i;
                return false;
            }
            if (m_keys[i] == key) {
                index = i;
                return true;
            }
            i = NextIndex(i);
        }
    }

    void InsertWithIndex(u32 index, K key, const V& v) {
        VASSERT((m_states[index >> 6] & (1ULL << (index & 63))) == 0);
        m_states[index >> 6] |= 1ULL << (index & 63);
        m_keys[index] = key;
        m_values[index] = v;
        ++m_count;
    }

    void UpdateWithIndex(u32 index, const V& v) {
        VASSERT(m_states[index >> 6] & (1ULL << (index & 63)));
        m_values[index] = v;
    }

    V& GetWithIndex(u32 index) {
        VASSERT(m_states[index >> 6] & (1ULL << (index & 63)));
        return m_values[index];
    }
    const V& GetWithIndex(u32 index) const {
        VASSERT(m_states[index >> 6] & (1ULL << (index & 63)));
        return m_values[index];
    }

    void RemoveWithIndex(u32 index) {
        VASSERT(m_states[index >> 6] & (1ULL << (index & 63)));

        uint32_t i = index;
        uint32_t j = NextIndex(i);
        while (m_states[j >> 6] & (1ULL << (j & 63))) {
            if (ProbeDistance(j, m_keys[j]) > ProbeDistance(i, m_keys[j])) {
                m_keys[i] = m_keys[j];
                m_values[i] = m_values[j];
                m_states[i >> 6] |= 1ULL << (i & 63);
                i = j;
            }
            j = NextIndex(j);
        }

        m_states[i >> 6] &= ~(1ULL << (i & 63));
        --m_count;
    }

    void set(K key, const V& v) {
        u32 index;
        if (Find(key, index)) {
            UpdateWithIndex(index, v);
        }
        else {
            InsertWithIndex(index, key, v);
        }
    }

    V* try_get(K key) {
        u32 index;
        if (Find(key, index)) {
            return &m_values[index];
        }
        else {
            return nullptr;
        }
    }

    void remove(K key) {
        u32 index;
        if (!Find(key, index)) {
            VABORT();
        }
        RemoveWithIndex(index);
    }

    bool contains(K key) const {
        u32 index;
        if (Find(key, index)) {
            return true;
        }
        else {
            return false;
        }
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





inline uint64_t RotateLeft(uint64_t bit, uint64_t shift) {
    return (bit << shift) | (bit >> (64ULL - shift));
}
inline uint64_t RotateRight(uint64_t bit, uint64_t shift) {
    return (bit >> shift) | (bit << (64ULL - shift));
}






#include <immintrin.h>


template <int SIZE>
struct BitArr {
   private:
    friend class Iterator;
    static constexpr int ARR_SIZE = (SIZE + 63) / 64;
    u64 arr_[ARR_SIZE] = {};

   public:
    bool operator==(const BitArr<SIZE>& r) const noexcept {
        return memcmp(arr_, r.arr_, sizeof(arr_)) == 0;
    }

    void GetRaw(array<u64, ARR_SIZE>& dst) const noexcept {
        memcpy(dst.data(), arr_, sizeof(arr_));
    }
    void SetRaw(const array<u64, ARR_SIZE>& src) noexcept {
        memcpy(arr_, src.data(), sizeof(arr_));
    }

    void clear() noexcept {
        reset();
    }
    void reset() noexcept {
        memset(arr_, 0, sizeof(arr_));
    }
    void set() noexcept {
        memset(arr_, 0xFF, sizeof(arr_));
    }

    void set(int i) noexcept {
        arr_[i >> 6] |= 1ULL << (i & 63);
    }

    void reset(int i) noexcept {
        arr_[i >> 6] &= ~(1ULL << (i & 63));
    }

    void flip(int i) noexcept {
        if (get(i)) {
            reset(i);
        }
        else {
            set(i);
        }
    }

    bool test(int i) const noexcept {
        return get(i);
    }
    bool get(int i) const noexcept {
        return arr_[i >> 6] & (1ULL << (i & 63));
    }
    bool operator[](int i) const noexcept {
        return get(i);
    }

    BitArr<SIZE> operator|(const BitArr<SIZE>& r) const noexcept {
        auto dst = *this;
        dst |= r;
        return dst;
    }
    BitArr<SIZE> operator&(const BitArr<SIZE>& r) const noexcept {
        auto dst = *this;
        dst &= r;
        return dst;
    }
    BitArr<SIZE> operator^(const BitArr<SIZE>& r) const noexcept {
        auto dst = *this;
        dst ^= r;
        return dst;
    }

    void operator|=(const BitArr<SIZE>& r) noexcept {
        REP(i, ARR_SIZE) {
            arr_[i] |= r.arr_[i];
        }
    }
    void OrAssign2(const BitArr<SIZE>& lhs, const BitArr<SIZE>& rhs) noexcept {
        REP(i, ARR_SIZE) {
            arr_[i] |= lhs.arr_[i] | rhs.arr_[i];
        }
    }
    void operator&=(const BitArr<SIZE>& r) noexcept {
        REP(i, ARR_SIZE) {
            arr_[i] &= r.arr_[i];
        }
    }
    void operator^=(const BitArr<SIZE>& r) noexcept {
        REP(i, ARR_SIZE) {
            arr_[i] ^= r.arr_[i];
        }
    }

    BitArr<SIZE> operator~() const noexcept {
        BitArr<SIZE> dst;
        REP(i, ARR_SIZE) {
            dst.arr_[i] = ~arr_[i];
        }
        return dst;
    }

    BitArr<SIZE> operator>>(int k) const noexcept {
        auto dst = *this;
        dst >>= k;
        return dst;
    }
    BitArr<SIZE> operator<<(int k) const noexcept {
        auto dst = *this;
        dst <<= k;
        return dst;
    }

    void operator>>=(int k) noexcept {
        if (k == 0) {
            return;
        }
        REP(i, ARR_SIZE - 1) {
            arr_[i] = (arr_[i] >> k) | (arr_[i + 1] << (64 - k));
        }
        {
            arr_[ARR_SIZE - 1] = (arr_[ARR_SIZE - 1] >> k);
        }
    }

    void operator<<=(int k) noexcept {
        if (k == 0) {
            return;
        }

        RFOR(i, 1, ARR_SIZE) {
            arr_[i] = (arr_[i] << k) | (arr_[i - 1] >> (64 - k));
        }
        {
            arr_[0] = (arr_[0] << k);
        }

        if constexpr (SIZE % 64) {
            arr_[ARR_SIZE - 1] &= (1ULL << (SIZE % 64)) - 1;
        }
    }

    void RightShiftOr(const BitArr<SIZE>& r, int k) noexcept {
        if (k == 0) {
            return;
        }

        REP(i, ARR_SIZE - 1) {
            arr_[i] |= (r.arr_[i] >> k) | (r.arr_[i + 1] << (64 - k));
        }
        {
            arr_[ARR_SIZE - 1] |= (r.arr_[ARR_SIZE - 1] >> k);
        }
    }

    void LeftShiftOrNoMask(const BitArr<SIZE>& r, int k) noexcept {
        if (k == 0) {
            return;
        }

        RFOR(i, 1, ARR_SIZE) {
            arr_[i] |= (r.arr_[i] << k) | (r.arr_[i - 1] >> (64 - k));
        }
        {
            arr_[0] |= (r.arr_[0] << k);
        }

    }

    void MaskedRightShiftOr(const BitArr<SIZE>& r, const BitArr<SIZE>& mask, int k) noexcept {
        if (k == 0) {
            REP(i, ARR_SIZE) {
                arr_[i] |= r.arr_[i] & mask.arr_[i];
            }
            return;
        }
        VASSERT(k < 64);
        REP(i, ARR_SIZE - 1) {
            arr_[i] |= ((r.arr_[i] & mask.arr_[i]) >> k) | ((r.arr_[i + 1] & mask.arr_[i + 1]) << (64 - k));
        }
        {
            arr_[ARR_SIZE - 1] |= (r.arr_[ARR_SIZE - 1] & mask.arr_[ARR_SIZE - 1]) >> k;
        }
    }

    template <int K>
    void MaskedRightShiftOr(const BitArr<SIZE>& r, const BitArr<SIZE>& mask) noexcept {
        static_assert(0 <= K && K < 64);
        if constexpr (K == 0) {
            REP(i, ARR_SIZE) {
                arr_[i] |= r.arr_[i] & mask.arr_[i];
            }
        }
        else {
            REP(i, ARR_SIZE - 1) {
                arr_[i] |= ((r.arr_[i] & mask.arr_[i]) >> K) | ((r.arr_[i + 1] & mask.arr_[i + 1]) << (64 - K));
            }
            {
                arr_[ARR_SIZE - 1] |= (r.arr_[ARR_SIZE - 1] & mask.arr_[ARR_SIZE - 1]) >> K;
            }
        }
    }

    void MaskedLeftShiftOr(const BitArr<SIZE>& r, const BitArr<SIZE>& mask, int k) noexcept {
        if (k == 0) {
            REP(i, ARR_SIZE) {
                arr_[i] |= r.arr_[i] & mask.arr_[i];
            }
            return;
        }
        VASSERT(k < 64);
        RFOR(i, 1, ARR_SIZE) {
            arr_[i] |= ((r.arr_[i] & mask.arr_[i]) << k) | ((r.arr_[i - 1] & mask.arr_[i - 1]) >> (64 - k));
        }
        {
            arr_[0] |= (r.arr_[0] & mask.arr_[0]) << k;
        }
        if constexpr (SIZE % 64) {
            arr_[ARR_SIZE - 1] &= (1ULL << (SIZE % 64)) - 1;
        }
    }

    template <int K>
    void MaskedLeftShiftOr(const BitArr<SIZE>& r, const BitArr<SIZE>& mask) noexcept {
        static_assert(0 <= K && K < 64);
        if constexpr (K == 0) {
            REP(i, ARR_SIZE) {
                arr_[i] |= r.arr_[i] & mask.arr_[i];
            }
        }
        else {
            RFOR(i, 1, ARR_SIZE) {
                arr_[i] |= ((r.arr_[i] & mask.arr_[i]) << K) | ((r.arr_[i - 1] & mask.arr_[i - 1]) >> (64 - K));
            }
            {
                arr_[0] |= (r.arr_[0] & mask.arr_[0]) << K;
            }
            if constexpr (SIZE % 64) {
                arr_[ARR_SIZE - 1] &= (1ULL << (SIZE % 64)) - 1;
            }
        }
    }

    template <int W>
    void LURDShiftOrNoMask(const BitArr<SIZE>& r) noexcept {
        if constexpr (ARR_SIZE >= 1) {
            arr_[0] |= (r.arr_[0] >> 1) | (r.arr_[0] >> W) | (r.arr_[0] << 1) | (r.arr_[0] << W);
            if constexpr (ARR_SIZE >= 2) {
                arr_[0] |= (r.arr_[1] << 63) | (r.arr_[1] << (64 - W));
            }
        }
        if constexpr (ARR_SIZE >= 2) {
            arr_[1] |= (r.arr_[1] >> 1) | (r.arr_[1] >> W) | (r.arr_[1] << 1) | (r.arr_[1] << W) | (r.arr_[0] >> 63) |
                       (r.arr_[0] >> (64 - W));
            if constexpr (ARR_SIZE >= 3) {
                arr_[1] |= (r.arr_[2] << 63) | (r.arr_[2] << (64 - W));
            }
        }
        if constexpr (ARR_SIZE >= 3) {
            arr_[2] |= (r.arr_[2] >> 1) | (r.arr_[2] >> W) | (r.arr_[2] << 1) | (r.arr_[2] << W) | (r.arr_[1] >> 63) |
                       (r.arr_[1] >> (64 - W));
            if constexpr (ARR_SIZE >= 4) {
                arr_[2] |= (r.arr_[3] << 63) | (r.arr_[3] << (64 - W));
            }
        }
        if constexpr (ARR_SIZE >= 4) {
            arr_[3] |= (r.arr_[3] >> 1) | (r.arr_[3] >> W) | (r.arr_[3] << 1) | (r.arr_[3] << W) | (r.arr_[2] >> 63) |
                       (r.arr_[2] >> (64 - W));
        }
        static_assert(ARR_SIZE <= 4, "ARR_SIZE must be less than or equal to 4");
    }

    void NotAnd(const BitArr<SIZE>& r) noexcept {
        REP(i, ARR_SIZE) {
            arr_[i] &= ~r.arr_[i];
        }
    }

    bool FilterNewAndMark(BitArr<SIZE>& visited) noexcept {
        bool any = false;
        REP(i, ARR_SIZE) {
            u64 b = arr_[i] & ~visited.arr_[i];
            arr_[i] = b;
            visited.arr_[i] |= b;
            any |= b != 0;
        }
        return any;
    }

    bool any() const noexcept {
        for (cauto& b : arr_) {
            if (b) {
                return true;
            }
        }
        return false;
    }

    bool none() const noexcept {
        for (cauto& b : arr_) {
            if (b) {
                return false;
            }
        }
        return true;
    }

    bool HasCommon(const BitArr<SIZE>& r) const noexcept {
        REP(i, ARR_SIZE) {
            if (arr_[i] & r.arr_[i]) {
                return true;
            }
        }
        return false;
    }

    int CountCommon(const BitArr<SIZE>& r) const noexcept {
        int c = 0;
        REP(i, ARR_SIZE) {
            c += ::popcount(arr_[i] & r.arr_[i]);
        }
        return c;
    }

    int popcount() const noexcept {
        int c = 0;
        REP(i, ARR_SIZE) {
            c += ::popcount(arr_[i]);
        }
        return c;
    }
    int count() const noexcept {
        int c = 0;
        REP(i, ARR_SIZE) {
            c += ::popcount(arr_[i]);
        }
        return c;
    }

    int select(int k) const noexcept {
        VASSERT(k >= 0);
        int c = 0;
        REP(i, ARR_SIZE) {
            int bc = std::popcount(arr_[i]);
            if (k < c + bc) {
                u64 p = _pdep_u64(1ULL << (k - c), arr_[i]);
                return (i << 6) | std::countr_zero(p);
            }
            c += bc;
        }
        return -1;
    }

    u64 pext(const BitArr<SIZE>& mask) const noexcept {
        u64 res = 0;
        int idx = 0;
        REP(i, ARR_SIZE) {
            u64 m = mask.arr_[i];
            u64 v = arr_[i];
            res |= _pext_u64(v, m) << idx;
            idx += std::popcount(m);
        }
        VASSERT(idx <= 64);
        return res;
    }

    template <class Func>
    void forEachSetBit(Func&& func) const {
        REP(ai, ARR_SIZE) {
            u64 bits = arr_[ai];
            while (bits != 0) {
                int bi = countr_zero(bits);
                func((ai << 6) | bi);
                bits &= bits - 1;
            }
        }
    }

    struct Iterator {
        const u64* ptr_;
        u64 bits_;
        int ai_;
        int idx_;  

        Iterator() : idx_(SIZE) {
        }

        Iterator(const BitArr* p) {
            ptr_ = p->arr_;
            ai_ = 0;

            while (ai_ < ARR_SIZE && *ptr_ == 0) {
                ++ptr_;
                ++ai_;
            }
            if (ai_ < ARR_SIZE) {
                bits_ = *ptr_;
                int bi = countr_zero(bits_);
                idx_ = (ai_ << 6) | bi;
                bits_ &= bits_ - 1;
            }
            else {
                idx_ = SIZE;
            }
        }

        int operator*() const {
            return idx_;
        }

        Iterator& operator++() {
            VASSERT(idx_ < SIZE);

            if (bits_ == 0) {
                ++ptr_;
                ++ai_;
                while (ai_ < ARR_SIZE && *ptr_ == 0) {
                    ++ptr_;
                    ++ai_;
                }
                if (ai_ < ARR_SIZE) {
                    bits_ = *ptr_;
                }
            }
            if (bits_ != 0) {
                int bi = countr_zero(bits_);
                idx_ = (ai_ << 6) | bi;
                bits_ &= bits_ - 1;
            }
            else {
                idx_ = SIZE;
            }
            return *this;
        }


        bool operator!=(const Iterator& it) const {
            return idx_ != it.idx_;
        }
        bool operator==(const Iterator& it) const {
            return idx_ == it.idx_;
        }
    };

    Iterator begin() const {
        return Iterator(this);
    }
    Iterator end() const {
        return Iterator();
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

constexpr int N_min = 10;
constexpr int N_max = 20;
constexpr int NN_max = N_max * N_max;
constexpr int M_min = N_min / 2;
constexpr int M_max = N_max * 2;
constexpr int T_min = 1;
constexpr int T_max = 2 * N_max * N_max * M_max;

DEF_CONTAINERS(NN, NN_max);
DEF_CONTAINERS(M, M_max);

int N;
int NN;
int M;
int T;
NNArr<i8> initialBallGrid_;
MArr<int> initialBallCells_;
NNArr<i8> goalGrid_;
MArr<int> ballToGoalCell_;


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


int ToCellDirIdx(int cell, int di) {
    return (cell << 2) | di;
}

void FromCellDirIdx(int idx, int& cell, int& di) {
    cell = idx >> 2;
    di = idx & 3;
}

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

inline Dir RotateR(Dir d) {
    constexpr Dir nexts[4] = {
        Dir::U,
        Dir::R,
        Dir::D,
        Dir::L,
    };
    return nexts[(int8_t)d];
}
inline Dir RotateL(Dir d) {
    constexpr Dir nexts[4] = {
        Dir::D,
        Dir::L,
        Dir::U,
        Dir::R,
    };
    return nexts[(int8_t)d];
}

bool IsHorizontal(Dir dir) {
    return dir == Dir::L || dir == Dir::R;
}
bool IsVertical(Dir dir) {
    return dir == Dir::U || dir == Dir::D;
}
inline Dir Back(Dir d) {
    return Dir(u8(d) ^ 2);
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
   private:
    NNArr<bool> hasRightWall_;
    NNArr<bool> hasBottomWall_;

    vector<pint> toPos_;
    vector<CCA<int, 4>> around_;  
    vector<array<int, 4>> dirs_;  
    vector<array<int, 3>> nextCellDirIdx_;  
    vector<array<int, 3>> nextMacroCellDirIdx_;  

   public:
    void InitInput(istream& is) {
        hasRightWall_.assign(NN, false);
        hasBottomWall_.assign(NN, false);

        char c;
        REP(y, N) {
            REP(x, N - 1) {
                is >> c;
                if (c == '1') {
                    int id = x + N * y;
                    hasRightWall_[id] = true;
                }
            }
        }
        REP(y, N - 1) {
            REP(x, N) {
                is >> c;
                if (c == '1') {
                    int id = x + N * y;
                    hasBottomWall_[id] = true;
                }
            }
        }

        auto CanMove = [&](int from, int to, int di) {
            if (di == 0) {
                return !hasRightWall_[to];
            }
            else if (di == 1) {
                return !hasBottomWall_[to];
            }
            else if (di == 2) {
                return !hasRightWall_[from];
            }
            else {
                return !hasBottomWall_[from];
            }
        };

        toPos_.resize(NN);
        around_.resize(NN);
        dirs_.resize(NN);
        REP(y, N) {
            REP(x, N) {
                int id = x + N * y;
                toPos_[id] = pint{x, y};

                REP(di, 4) {
                    const pint& a = Around4[di];
                    int x2 = x + a.a;
                    int y2 = y + a.b;
                    int nc = x2 + y2 * N;
                    if (x2 >= 0 && x2 < N && y2 >= 0 && y2 < N && CanMove(id, nc, di)) {
                        around_[id].push(nc);
                        dirs_[id][di] = nc;
                    }
                    else {
                        dirs_[id][di] = -1;
                    }
                }
            }
        }

        nextCellDirIdx_.resize(NN * 4);
        nextMacroCellDirIdx_.resize(NN * 4);
        REP(cell, NN) {
            REP(di, 4) {
                int idx = ToCellDirIdx(cell, di);
                int forwardCell = dirs_[cell][di];
                int forwardIdx = forwardCell < 0 ? -1 : ToCellDirIdx(forwardCell, di);
                int leftIdx = ToCellDirIdx(cell, (di + 3) & 3);
                int rightIdx = ToCellDirIdx(cell, (di + 1) & 3);
                nextCellDirIdx_[idx][0] = forwardIdx;
                nextCellDirIdx_[idx][1] = leftIdx;
                nextCellDirIdx_[idx][2] = rightIdx;
                nextMacroCellDirIdx_[idx][0] = forwardIdx < 0 ? idx : forwardIdx;
                nextMacroCellDirIdx_[idx][1] = leftIdx;
                nextMacroCellDirIdx_[idx][2] = rightIdx;
            }
        }
    }

    inline int ToId(int x, int y) const {
        VASSERT(x >= 0 && x < N);
        VASSERT(y >= 0 && y < N);
        return x + N * y;
    }
    inline int ToId(const pint& p) const {
        VASSERT(p.x >= 0 && p.x < N);
        VASSERT(p.y >= 0 && p.y < N);
        return p.x + N * p.y;
    }
    inline const pint& ToPos(int id) const {
        return toPos_[id];
    }


    inline const CCA<int, 4>& around(int id) const {
        return around_[id];
    }
    inline const array<int, 4>& dirs(int id) const {
        return dirs_[id];
    }
    inline const array<int, 3>& nextCellDirIdx(int idx) const {
        return nextCellDirIdx_[idx];
    }
    inline const array<int, 3>& nextMacroCellDirIdx(int idx) const {
        return nextMacroCellDirIdx_[idx];
    }

    bool HasRightWall(int id) const {
        return hasRightWall_[id];
    }
    bool HasBottomWall(int id) const {
        return hasBottomWall_[id];
    }
};
GridSystem gs;


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




struct {


    PI(R_TransMacroChange, 0, 0, 10, 0);
    PI(R_TransMacroLRFlip, 0, 0, 10, 0);
    PI(R_TransMacroRotate, 0, 0, 10, 0);
    PI(R_TransMacroSwap, 0, 0, 10, 0);
    PI(R_TransBallNearInsert, 0, 0, 10, 0);

    PI(R_ChoiceFowardRate, 21, 10, 16, 0);
    PI(R_ChoiceRotateRate, 1, 1, 5, 0);

    START_TUNING;
    PI(R_TransRangeShrink, 261, 220, 280, 0);
    PI(R_TransRangeExpand, 109, 80, 120, 0);
    PI(R_TransMacroAddSide, 147, 120, 160, 0);
    PI(R_TransMacroRemoveSide, 43, 30, 50, 0);
    PI(R_TransMacroAddRand, 48, 30, 60, 0);
    PI(R_TransMacroRemoveRand, 44, 30, 55, 0);
    PI(R_TransMacroFowardExpand, 35, 20, 50, 0);
    PI(R_TransMacroFowardShrink, 24, 15, 30, 0);
    PI(R_TransBallInsert, 752, 700, 800, 0);
    PI(R_TransBallSwap, 192, 160, 220, 0);
    PI(R_TransBallNearSwap, 18, 10, 30, 0);
    PI(S_TransMacroSRemove, 45, 0, 50, 0);
    PI(S_TransMacroSMove, 3, 0, 10, 0);
    END_TUNING;

    PD(R_EvalStep, 0.0013827749163174954, 0.000, 0.002, 0.0);
    PD(R_EvalMaxIntervalTurn, 0.009960759903315486, 0.004, 0.014, 0.0);
    PD(R_EvalIntervalVariance, 0.0980433765761252, 0.04, 0.14, 0.0);
    PD(R_PenaltyTime, 1.0, 0.5, 1.0, 0.0001, 1.0);

    PD(Type0_StartTemp, 0.935013450398287, 0.5, 1.5, 0.0);
    PD(Type0_EndTemp, 0.7236855224801009, 0.4, 0.9, 0.0);
    PI(InitialStateCount, 26, 16, 32, 1);
    PD(FilterStartTime, 0.033046021148126145, 0.0, 0.1, 0.0, 1.0);
    PD(FilterEndTime, 0.5499484094548396, 0.3, 0.7, 0.0, 1.0);

    PD(RollbackStartRate, 0.24385189225789028, 0.0, 0.2, 0.0, 1.0);
    PI(RollbackCount, 47530, 100, 1000000, 1);
    PD(RollbackNextMulti, 1.4328045269949838, 0.9, 1.1, 0.1);
    PI(MinRollbackCount, 16105, 100, 10000, 1);


    PC(TempType, 0, 0, 1, 2);

    IF_START(TempType == 0);
    IF_END;

    IF_START(TempType == 1);
    PD(Type1_StartTemp, 1.4, 0.0, 10.0, 0.0);
    PD(Type1_EndTemp, 0.4, 0.0, 1.0, 0.0);
    IF_END;

    IF_START(TempType == 2);
    PD(Type2_StartTemp, 1.4, 0.0, 10.0, 0.0);
    PD(Type2_EndTemp, 0.5, 0.0, 1.0, 0.0);
    PD(Type2_TempPow, 1.5, 1.0, 3.0, 1.0);
    IF_END;

    PI(UpdateTmpMaskBit, 4, 0, 10, 0, 16);



} HP;




#undef PC
#undef PI
#undef PD



struct Result {
    dvec<char> commands_;
};

struct IOServer {
   private:

   public:
    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        is >> N;
        timer.Init();  
        is >> M;
        is >> T;

        NN = N * N;
        gs.InitInput(is);

        initialBallGrid_.assign(NN, -1);
        initialBallCells_.clear();
        goalGrid_.assign(NN, -1);
        ballToGoalCell_.resize(M);

        int b, c, d, e;
        REP(i, M) {
            is >> b >> c >> d >> e;
            int ball = gs.ToId(c, b);
            int goal = gs.ToId(e, d);

            initialBallGrid_[ball] = i;
            initialBallCells_.push(ball);
            goalGrid_[goal] = i;
            ballToGoalCell_[i] = goal;
        }
    }

    void Output(const Result& result) {
        ostream& os = cout;
        REP(i, result.commands_.size()) {
            os << result.commands_[i] << '\n';
        }
        os.flush();
    }

    void Finalize() {
    }
};
IOServer server;


struct EasyState {
    int cell_;              
    u8 di_;                 
    i8 ball_;               
    int turns_;             
    int steps_;             
    MArr<bool> completed_;  

    void Init() {
        cell_ = 0;
        di_ = 2;
        ball_ = -1;
        turns_ = 0;
        steps_ = 0;
        completed_.assign(M, false);
    }


    void CompleteBall(int ballId) {
        completed_[ballId] = true;
    }
};

struct FindSegPathResult {
    int endIdx_ = -1;
    int turn_ = INT_MAX;
    int step_ = INT_MAX;
    bool usedMacro_ = false;
    dvec<char> commands_;

    void Clear() {
        endIdx_ = -1;
        turn_ = INT_MAX;
        step_ = INT_MAX;
        usedMacro_ = false;
        commands_.clear();
    }
};

struct FindSegPathStat {
    int endIdx_ = -1;
    int turn_ = INT_MAX;
    int step_ = INT_MAX;
    bool found_ = false;
    bool usedMacro_ = false;
};

struct FindSegPathFrontierOption {
    int endIdx_ = -1;
    int turn_ = INT_MAX;
    int step_ = INT_MAX;
    bool usedMacro_ = false;
};

struct FindSegPathFrontierResult {
    using OptionList = CapArr<FindSegPathFrontierOption, NN_max * 4>;

    OptionList options_;
    const OptionList* optionsRef_ = &options_;

    void Clear() {
        options_.clear();
        optionsRef_ = &options_;
    }

    bool Found() const {
        return !GetOptions().empty();
    }

    const OptionList& GetOptions() const {
        return *optionsRef_;
    }
};

struct FindSegPathUniversalFrontierCacheEntry {
    bool found_ = false;
    FindSegPathFrontierResult::OptionList options_;
};

struct FindSegPathUniversalCache {
    dhmap<u64, FindSegPathStat> statCache_;
    dhmap<u64, FindSegPathUniversalFrontierCacheEntry> frontierCache_;
};

constexpr int FindSegPathCompactStrideNMax = 11;  
constexpr int FindSegPathMaxPublicIdxBits = 11;
constexpr int FindSegPathMaxPublicIdxStride = 1 << FindSegPathMaxPublicIdxBits;
constexpr int FindSegPathStateMax = (NN_max + 1) * FindSegPathMaxPublicIdxStride;
constexpr int FindSegPathMacroPlanCacheCapacity = 64;
constexpr int FindSegPathMacroPlanBlockCapacity = 2;
constexpr int FindSegPathMacroMax = 64;
constexpr int FindSegPathUniversalStatCacheCapacityHint = 1000000;
constexpr int FindSegPathUniversalFrontierCacheCapacityHint = 200000;
using FindSegPathMacro = CapArr<char, FindSegPathMacroMax>;

template <class MACRO>
bool AssignFindSegPathMacro(FindSegPathMacro& dst, const MACRO& src) {
    int n = (int)src.size();
    if (n > FindSegPathMacroMax) {
        return false;
    }
    dst.resize(n);
    REP(i, n) {
        dst[i] = src[i];
    }
    return true;
}

template <class MACRO>
bool SameFindSegPathMacro(const FindSegPathMacro& lhs, const MACRO& rhs) {
    if ((int)lhs.size() != (int)rhs.size()) {
        return false;
    }
    REP(i, lhs.size()) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

const NNArr<unsigned char>& GetFindSegPathFixedOther(int targetBall) {
    static int preparedNN = -1;
    static int preparedM = -1;
    static array<NNArr<unsigned char>, M_max> fixedOtherByTarget;

    if (preparedNN != NN || preparedM != M) {
        REP(target, M) {
            auto& fixedOther = fixedOtherByTarget[target];
            fixedOther.assign(NN, 0);
            REP(ball, M) {
                if (ball == target) {
                    continue;
                }
                fixedOther[initialBallCells_[ball]] = 1;
                fixedOther[ballToGoalCell_[ball]] = 1;
            }
        }
        preparedNN = NN;
        preparedM = M;
    }

    return fixedOtherByTarget[targetBall];
}

inline u64 FindSegPathCacheToken(u64 x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

inline void MixFindSegPathCacheKey(u64& h, int kind, u64 value) {
    h ^= FindSegPathCacheToken(((u64)kind << 48) ^ value);
}

template <class MACRO>
u64 CalcFindSegPathUniversalCacheKey(int startCell, int startDi, int targetBall, const MACRO& macro) {
    u64 cacheKey = 0;
    MixFindSegPathCacheKey(cacheKey, 0, startCell);
    MixFindSegPathCacheKey(cacheKey, 1, startDi);
    MixFindSegPathCacheKey(cacheKey, 2, targetBall);
    MixFindSegPathCacheKey(cacheKey, 3, macro.size());
    REP(i, macro.size()) {
        MixFindSegPathCacheKey(cacheKey, 4 + i, (u64)(unsigned char)macro[i]);
    }
    return cacheKey;
}

inline u64 CalcFindSegPathUniversalCacheKeyByMacroHash(int startCell, int startDi, int targetBall, int macroSize,
                                                       u64 macroHash) {
    u64 cacheKey = 0;
    MixFindSegPathCacheKey(cacheKey, 0, startCell);
    MixFindSegPathCacheKey(cacheKey, 1, startDi);
    MixFindSegPathCacheKey(cacheKey, 2, targetBall);
    MixFindSegPathCacheKey(cacheKey, 3, macroSize);
    MixFindSegPathCacheKey(cacheKey, 4, macroHash);
    return cacheKey;
}

template <int PublicIdxBits, class MACRO>
bool FindSegPathImpl(int startCell, int startDi, int targetBall, const MArr<bool>& ballsCompleted, const MACRO& macro,
                     FindSegPathResult& result, bool needCommands, int targetEndIdx,
                     FindSegPathFrontierResult* frontier, int targetUsedMacro) {
    static_assert(0 < PublicIdxBits && PublicIdxBits <= FindSegPathMaxPublicIdxBits);

    struct Node {
        u16 turn_ = 0xffff;
        u16 step_ = 0xffff;
        int fromState_ = -1;
        char command_ = 0;
        bool usedMacro_ = false;
    };
    result.Clear();
    if (frontier) {
        frontier->Clear();
    }

    VASSERT(0 <= startCell && startCell < NN);
    VASSERT(0 <= startDi && startDi < 4);
    VASSERT(0 <= targetBall && targetBall < M);
    VASSERT((int)ballsCompleted.size() == M);
    for (char command : macro) {
        VASSERT(command == 'F' || command == 'L' || command == 'R' || command == 'S');
    }

    if (ballsCompleted[targetBall]) {
        result.endIdx_ = ToCellDirIdx(startCell, startDi);
        result.turn_ = 0;
        result.step_ = 0;
        result.usedMacro_ = false;
        if (frontier) {
            frontier->options_.push(FindSegPathFrontierOption{result.endIdx_, 0, 0, false});
        }
        result.commands_.clear();
        return true;
    }

    int noneCell = NN;
    int posCount = NN + 1;
    int publicStateCount = NN * 4;
    int denseStateCount = publicStateCount * posCount;
    constexpr int publicIdxBits = PublicIdxBits;
    constexpr int publicIdxMask = (1 << publicIdxBits) - 1;
    constexpr int publicIdxStride = 1 << publicIdxBits;
    int stateCapacity = posCount * publicIdxStride;
    VASSERT(publicStateCount <= publicIdxStride);
    VASSERT(publicIdxStride <= FindSegPathMaxPublicIdxStride);
    VASSERT(stateCapacity <= FindSegPathStateMax);
    int goalCell = ballToGoalCell_[targetBall];
    const NNArr<unsigned char>& fixedOther = GetFindSegPathFixedOther(targetBall);

    static int transitionTableNN = -1;
    static array<int, NN_max * 4> publicCell;
    static array<int, NN_max * 4> nextPublicF;
    static array<int, NN_max * 4> nextPublicMacroF;
    static array<int, NN_max * 4> nextPublicL;
    static array<int, NN_max * 4> nextPublicR;
    if (transitionTableNN != NN) {
        REP(cell, NN) {
            REP(di, 4) {
                int publicIdx = ToCellDirIdx(cell, di);
                publicCell[publicIdx] = cell;
                int nextCell = gs.dirs(cell)[di];
                nextPublicF[publicIdx] = nextCell < 0 ? -1 : ToCellDirIdx(nextCell, di);
                nextPublicMacroF[publicIdx] = nextCell < 0 ? publicIdx : ToCellDirIdx(nextCell, di);
                nextPublicL[publicIdx] = ToCellDirIdx(cell, (di + 3) & 3);
                nextPublicR[publicIdx] = ToCellDirIdx(cell, (di + 1) & 3);
            }
        }
        transitionTableNN = NN;
    }

    auto ToStatePublic = [&](int publicIdx, int targetPos) {
        VASSERT(0 <= publicIdx && publicIdx < publicStateCount);
        VASSERT(0 <= targetPos && targetPos < posCount);
        return (targetPos << publicIdxBits) | publicIdx;
    };
    auto ToState = [&](int cell, int di, int targetPos) {
        VASSERT(0 <= cell && cell < NN);
        VASSERT(0 <= di && di < 4);
        return ToStatePublic(ToCellDirIdx(cell, di), targetPos);
    };
    auto CellContent = [&](int cell, int targetPos, int heldOtherOrigin) {
        if (targetPos == cell) {
            return 1;  
        }
        if (heldOtherOrigin == cell) {
            return 0;  
        }
        return fixedOther[cell] ? 2 : 0;
    };

    auto ApplyS = [&](int publicIdx, int& targetPos, int& heldOtherOrigin, bool inMacro) {
        int cell = publicCell[publicIdx];
        int content = CellContent(cell, targetPos, heldOtherOrigin);
        if (!inMacro) {
            if (targetPos == noneCell) {
                if (cell != goalCell || content != 0) {
                    return false;
                }
                targetPos = cell;
                return true;
            }
            if (heldOtherOrigin != noneCell) {
                if (cell != heldOtherOrigin) {
                    return false;
                }
                if (content == 1) {
                    targetPos = noneCell;
                }
                heldOtherOrigin = noneCell;
                return true;
            }
            if (content == 1) {
                targetPos = noneCell;
                return true;
            }
            return false;
        }

        if (targetPos == noneCell) {
            VASSERT(heldOtherOrigin == noneCell);
            if (content == 0) {
                targetPos = cell;
            }
            else if (content == 2) {
                targetPos = cell;
                heldOtherOrigin = cell;
            }
            else {
                VABORT();
            }
        }
        else if (heldOtherOrigin != noneCell) {
            if (cell != heldOtherOrigin) {
                return false;
            }
            if (content == 1) {
                targetPos = noneCell;
                heldOtherOrigin = noneCell;
            }
            else if (content == 0) {
                heldOtherOrigin = noneCell;
            }
            else {
                return false;
            }
        }
        else {
            if (content == 1) {
                targetPos = noneCell;
            }
            else if (content == 2) {
                heldOtherOrigin = cell;
            }
        }
        return true;
    };

    struct MacroPlan {
        int nn_ = -1;
        int lastUsed_ = 0;
        FindSegPathMacro macro_;
        int preBlock_ = -1;
        int postBlock_ = -1;
        bool hasS_ = false;
        CapArr<array<int, NN_max * 4>, FindSegPathMacroPlanBlockCapacity> blockNexts_;
    };

    static vector<MacroPlan> macroPlanCache;
    static int macroPlanTick = 0;
    ++macroPlanTick;

    MacroPlan* macroPlan = nullptr;
    for (auto& entry : macroPlanCache) {
        if (entry.nn_ == NN && SameFindSegPathMacro(entry.macro_, macro)) {
            macroPlan = &entry;
            break;
        }
    }

    if (!macroPlan) {
        if ((int)macroPlanCache.size() < FindSegPathMacroPlanCacheCapacity) {
            macroPlanCache.push_back(MacroPlan{});
            macroPlan = &macroPlanCache.back();
        }
        else {
            macroPlan = &macroPlanCache[0];
            for (auto& entry : macroPlanCache) {
                if (entry.lastUsed_ < macroPlan->lastUsed_) {
                    macroPlan = &entry;
                }
            }
        }

        macroPlan->nn_ = NN;
        bool assignedMacro = AssignFindSegPathMacro(macroPlan->macro_, macro);
        if (!assignedMacro) {
            result.Clear();
            return false;
        }
        macroPlan->preBlock_ = -1;
        macroPlan->postBlock_ = -1;
        macroPlan->hasS_ = false;
        macroPlan->blockNexts_.clear();

        auto AddMacroBlock = [&](int l, int r) {
            if (l >= r) {
                return -1;
            }
            int blockIdx = macroPlan->blockNexts_.size();
            macroPlan->blockNexts_.resize(blockIdx + 1);
            auto& nexts = macroPlan->blockNexts_[blockIdx];
            REP(publicIdx, publicStateCount) {
                int p = publicIdx;
                FOR(j, l, r) {
                    char command = macroPlan->macro_[j];
                    if (command == 'F') {
                        p = nextPublicMacroF[p];
                    }
                    else if (command == 'L') {
                        p = nextPublicL[p];
                    }
                    else {
                        VASSERT(command == 'R');
                        p = nextPublicR[p];
                    }
                }
                nexts[publicIdx] = p;
            }
            return blockIdx;
        };

        int macroSCount = 0;
        int macroSPos = -1;
        REP(i, macroPlan->macro_.size()) {
            if (macroPlan->macro_[i] == 'S') {
                ++macroSCount;
                VASSERT(macroSCount <= 1);
                macroSPos = i;
            }
        }

        if (macroSPos < 0) {
            macroPlan->preBlock_ = AddMacroBlock(0, macroPlan->macro_.size());
        }
        else {
            macroPlan->hasS_ = true;
            macroPlan->preBlock_ = AddMacroBlock(0, macroSPos);
            macroPlan->postBlock_ = AddMacroBlock(macroSPos + 1, macroPlan->macro_.size());
        }
    }
    macroPlan->lastUsed_ = macroPlanTick;

    auto ApplyMacroPublic = [&](int publicIdx, int targetPos, int& nextState) {
        if (macroPlan->preBlock_ >= 0) {
            publicIdx = macroPlan->blockNexts_[macroPlan->preBlock_][publicIdx];
        }
        if (macroPlan->hasS_) {
            int heldOtherOrigin = noneCell;
            if (!ApplyS(publicIdx, targetPos, heldOtherOrigin, true) || heldOtherOrigin != noneCell) {
                return false;
            }
        }
        if (macroPlan->postBlock_ >= 0) {
            publicIdx = macroPlan->blockNexts_[macroPlan->postBlock_][publicIdx];
        }
        nextState = (targetPos << publicIdxBits) | publicIdx;
        return true;
    };
    const int macroSize = (int)macroPlan->macro_.size();

    if (!macroPlan->hasS_) {
        constexpr int PhaseInitial = 0;
        constexpr int PhaseHeld = 1;
        constexpr int PhaseGoal = 2;
        int fastStateCapacity = 3 * publicIdxStride;
        int reserveNodeCount = min(fastStateCapacity, max(4096, NN * 4 * 16));

        auto ToFastState = [&](int publicIdx, int phase) {
            VASSERT(0 <= publicIdx && publicIdx < publicStateCount);
            VASSERT(0 <= phase && phase < 3);
            return (phase << publicIdxBits) | publicIdx;
        };

        int initialCell = initialBallCells_[targetBall];
        int initialPhase = initialCell == goalCell ? PhaseGoal : PhaseInitial;
        int initialState = ToFastState(ToCellDirIdx(startCell, startDi), initialPhase);

        if (!needCommands && frontier == nullptr && targetEndIdx < 0 && targetUsedMacro < 0) {
            struct FastStatNode {
                u16 turn_ = 0xffff;
                u16 step_ = 0xffff;
                bool usedMacro_ = false;
            };

            static vector<FastStatNode> statNodeData;
            static vector<u32> statNodeChecked;
            static u32 statNodeMark = 1;
            if ((int)statNodeData.size() < fastStateCapacity) {
                statNodeData.resize(fastStateCapacity);
            }
            if ((int)statNodeChecked.size() < fastStateCapacity) {
                statNodeChecked.resize(fastStateCapacity, 0);
            }
            ++statNodeMark;
            if (statNodeMark == 0) {
                fill(statNodeChecked.begin(), statNodeChecked.end(), 0);
                ++statNodeMark;
            }
            const u32 currentStatNodeMark = statNodeMark;

            static dvec<int> statCurNodes;
            static dvec<int> statNextNodes;
            statCurNodes.clear();
            statNextNodes.clear();
            statCurNodes.reserve(reserveNodeCount);
            statNextNodes.reserve(reserveNodeCount);

            auto PushStat = [&](dvec<int>& layerNodes, int state, int turn, int step, bool usedMacro) {
                if (step > T) {
                    return;
                }
                u32& checked = statNodeChecked[state];
                if (checked == currentStatNodeMark) {
                    FastStatNode& nextNode = statNodeData[state];
                    VASSERT(nextNode.turn_ <= turn);
                    if (nextNode.turn_ != turn || nextNode.step_ <= step) {
                        return;
                    }
                    nextNode.step_ = step;
                    nextNode.usedMacro_ = usedMacro;
                }
                else {
                    checked = currentStatNodeMark;
                    FastStatNode& nextNode = statNodeData[state];
                    nextNode.turn_ = turn;
                    nextNode.step_ = step;
                    nextNode.usedMacro_ = usedMacro;
                    layerNodes.push(state);
                }
            };

            statNodeChecked[initialState] = currentStatNodeMark;
            FastStatNode& initialNode = statNodeData[initialState];
            initialNode.turn_ = 0;
            initialNode.step_ = 0;
            initialNode.usedMacro_ = false;
            statCurNodes.push(initialState);

            for (int turn = 0; !statCurNodes.empty(); ++turn) {
                int bestState = -1;
                int bestStep = INT_MAX;
                int bestPublicIdx = -1;
                REP(i, statCurNodes.size()) {
                    int state = statCurNodes[i];
                    const FastStatNode& node = statNodeData[state];
                    VASSERT(node.turn_ == turn);
                    int phase = state >> publicIdxBits;
                    if (phase != PhaseGoal) {
                        continue;
                    }
                    if (node.step_ < bestStep) {
                        bestState = state;
                        bestStep = node.step_;
                        bestPublicIdx = state & publicIdxMask;
                    }
                }
                if (bestState >= 0) {
                    result.endIdx_ = bestPublicIdx;
                    result.turn_ = turn;
                    result.step_ = bestStep;
                    result.usedMacro_ = statNodeData[bestState].usedMacro_;
                    result.commands_.clear();
                    return true;
                }
                if (turn >= T) {
                    break;
                }

                statNextNodes.clear();
                int nextTurn = turn + 1;
                REP(i, statCurNodes.size()) {
                    int state = statCurNodes[i];
                    const FastStatNode& node = statNodeData[state];
                    VASSERT(node.turn_ == turn);
                    if (node.step_ >= T) {
                        continue;
                    }

                    int publicIdx = state & publicIdxMask;
                    int phase = state >> publicIdxBits;
                    int stateBase = phase << publicIdxBits;
                    int nextStep = node.step_ + 1;
                    int nextF = nextPublicF[publicIdx];
                    if (nextF >= 0) {
                        PushStat(statNextNodes, stateBase | nextF, nextTurn, nextStep, node.usedMacro_);
                    }
                    PushStat(statNextNodes, stateBase | nextPublicL[publicIdx], nextTurn, nextStep, node.usedMacro_);
                    PushStat(statNextNodes, stateBase | nextPublicR[publicIdx], nextTurn, nextStep, node.usedMacro_);

                    int cell = publicCell[publicIdx];
                    int nextPhase = -1;
                    if (phase == PhaseInitial) {
                        if (cell == initialCell) {
                            nextPhase = PhaseHeld;
                        }
                    }
                    else if (phase == PhaseHeld) {
                        if (cell == goalCell && !fixedOther[cell]) {
                            nextPhase = PhaseGoal;
                        }
                    }
                    else {
                        VASSERT(phase == PhaseGoal);
                        if (cell == goalCell) {
                            nextPhase = PhaseHeld;
                        }
                    }
                    if (nextPhase >= 0) {
                        PushStat(statNextNodes, ToFastState(publicIdx, nextPhase), nextTurn, nextStep, node.usedMacro_);
                    }

                    int macroStep = node.step_ + macroSize;
                    if (macroSize > 0 && macroStep <= T) {
                        int macroPublicIdx = publicIdx;
                        if (macroPlan->preBlock_ >= 0) {
                            macroPublicIdx = macroPlan->blockNexts_[macroPlan->preBlock_][macroPublicIdx];
                        }
                        PushStat(statNextNodes, stateBase | macroPublicIdx, nextTurn, macroStep, true);
                    }
                }
                swap(statCurNodes, statNextNodes);
            }

            return false;
        }

        static vector<Node> fastNodeData;
        static vector<u32> fastNodeChecked;
        static u32 fastNodeMark = 1;
        if ((int)fastNodeData.size() < fastStateCapacity) {
            fastNodeData.resize(fastStateCapacity);
        }
        if ((int)fastNodeChecked.size() < fastStateCapacity) {
            fastNodeChecked.resize(fastStateCapacity, 0);
        }
        ++fastNodeMark;
        if (fastNodeMark == 0) {
            fill(fastNodeChecked.begin(), fastNodeChecked.end(), 0);
            ++fastNodeMark;
        }
        const u32 currentFastNodeMark = fastNodeMark;

        static dvec<int> fastCurNodes;
        static dvec<int> fastNextNodes;
        fastCurNodes.clear();
        fastNextNodes.clear();
        fastCurNodes.reserve(reserveNodeCount);
        fastNextNodes.reserve(reserveNodeCount);

        auto PushFast = [&](dvec<int>& layerNodes, int state, int turn, int step, int fromState, char command,
                            bool usedMacro) {
            if (step > T) {
                return;
            }
            u32& checked = fastNodeChecked[state];
            if (checked == currentFastNodeMark) {
                Node& nextNode = fastNodeData[state];
                VASSERT(nextNode.turn_ <= turn);
                if (nextNode.turn_ != turn || nextNode.step_ <= step) {
                    return;
                }
                nextNode.step_ = step;
                if (needCommands) {
                    nextNode.fromState_ = fromState;
                    nextNode.command_ = command;
                }
                nextNode.usedMacro_ = usedMacro;
            }
            else {
                checked = currentFastNodeMark;
                Node& nextNode = fastNodeData[state];
                nextNode.turn_ = turn;
                nextNode.step_ = step;
                if (needCommands) {
                    nextNode.fromState_ = fromState;
                    nextNode.command_ = command;
                }
                nextNode.usedMacro_ = usedMacro;
                layerNodes.push(state);
            }
        };

        auto BuildFastNodeCommands = [&](int state, dvec<char>& commands) {
            commands.clear();
            static dvec<char> reversed;
            reversed.clear();

            while (state >= 0) {
                const Node& node = fastNodeData[state];
                if (node.fromState_ < 0) {
                    break;
                }
                reversed.push(node.command_);
                state = node.fromState_;
            }
            for (int i = (int)reversed.size() - 1; i >= 0; --i) {
                commands.push(reversed[i]);
            }
        };

        fastNodeChecked[initialState] = currentFastNodeMark;
        Node& initialNode = fastNodeData[initialState];
        initialNode.turn_ = 0;
        initialNode.step_ = 0;
        if (needCommands) {
            initialNode.fromState_ = -1;
            initialNode.command_ = '*';
        }
        initialNode.usedMacro_ = false;
        fastCurNodes.push(initialState);

        for (int turn = 0; !fastCurNodes.empty(); ++turn) {
            int bestState = -1;
            int bestStep = INT_MAX;
            int bestPublicIdx = -1;
            REP(i, fastCurNodes.size()) {
                int state = fastCurNodes[i];
                const Node& node = fastNodeData[state];
                VASSERT(node.turn_ == turn);

                int publicIdx = state & publicIdxMask;
                int phase = state >> publicIdxBits;
                if (phase != PhaseGoal) {
                    continue;
                }
                if (targetEndIdx >= 0 && publicIdx != targetEndIdx) {
                    continue;
                }
                if (targetUsedMacro >= 0 && (node.usedMacro_ ? 1 : 0) != targetUsedMacro) {
                    continue;
                }
                if (frontier) {
                    frontier->options_.push(
                        FindSegPathFrontierOption{publicIdx, node.turn_, node.step_, node.usedMacro_});
                }
                if (node.step_ < bestStep) {
                    bestState = state;
                    bestStep = node.step_;
                    bestPublicIdx = publicIdx;
                }
            }
            if (bestState >= 0) {
                result.endIdx_ = bestPublicIdx;
                result.turn_ = turn;
                result.step_ = bestStep;
                result.usedMacro_ = fastNodeData[bestState].usedMacro_;
                if (needCommands) {
                    static dvec<char> commands;
                    BuildFastNodeCommands(bestState, commands);
                    result.commands_ = commands;
                }
                else {
                    result.commands_.clear();
                }
                return true;
            }
            if (turn >= T) {
                break;
            }

            fastNextNodes.clear();
            int nextTurn = turn + 1;
            REP(i, fastCurNodes.size()) {
                int state = fastCurNodes[i];
                const Node& node = fastNodeData[state];
                VASSERT(node.turn_ == turn);
                if (node.step_ >= T) {
                    continue;
                }

                int publicIdx = state & publicIdxMask;
                int phase = state >> publicIdxBits;
                int stateBase = phase << publicIdxBits;
                int nextStep = node.step_ + 1;

                int nextF = nextPublicF[publicIdx];
                if (nextF >= 0) {
                    PushFast(fastNextNodes, stateBase | nextF, nextTurn, nextStep, state, 'F', node.usedMacro_);
                }
                PushFast(fastNextNodes, stateBase | nextPublicL[publicIdx], nextTurn, nextStep, state, 'L',
                         node.usedMacro_);
                PushFast(fastNextNodes, stateBase | nextPublicR[publicIdx], nextTurn, nextStep, state, 'R',
                         node.usedMacro_);

                int cell = publicCell[publicIdx];
                int nextPhase = -1;
                if (phase == PhaseInitial) {
                    if (cell == initialCell) {
                        nextPhase = PhaseHeld;
                    }
                }
                else if (phase == PhaseHeld) {
                    if (cell == goalCell && !fixedOther[cell]) {
                        nextPhase = PhaseGoal;
                    }
                }
                else {
                    VASSERT(phase == PhaseGoal);
                    if (cell == goalCell) {
                        nextPhase = PhaseHeld;
                    }
                }
                if (nextPhase >= 0) {
                    PushFast(fastNextNodes, ToFastState(publicIdx, nextPhase), nextTurn, nextStep, state, 'S',
                             node.usedMacro_);
                }

                int macroStep = node.step_ + macroSize;
                if (macroSize > 0 && macroStep <= T) {
                    int macroPublicIdx = publicIdx;
                    if (macroPlan->preBlock_ >= 0) {
                        macroPublicIdx = macroPlan->blockNexts_[macroPlan->preBlock_][macroPublicIdx];
                    }
                    PushFast(fastNextNodes, stateBase | macroPublicIdx, nextTurn, macroStep, state, 'P', true);
                }
            }
            swap(fastCurNodes, fastNextNodes);
        }

        return false;
    }

    int reserveNodeCount = min(denseStateCount, max(65536, NN * 4 * 128));
    static vector<Node> nodeData;
    static vector<u32> nodeChecked;
    static u32 nodeMark = 1;
    if ((int)nodeData.size() < stateCapacity) {
        nodeData.resize(stateCapacity);
    }
    if ((int)nodeChecked.size() < stateCapacity) {
        nodeChecked.resize(stateCapacity, 0);
    }
    ++nodeMark;
    if (nodeMark == 0) {
        fill(nodeChecked.begin(), nodeChecked.end(), 0);
        ++nodeMark;
    }
    const u32 currentNodeMark = nodeMark;

    static dvec<int> curNodes;
    static dvec<int> nextNodes;
    curNodes.clear();
    nextNodes.clear();
    curNodes.reserve(reserveNodeCount);
    nextNodes.reserve(reserveNodeCount);
    auto PushNext = [&](dvec<int>& layerNodes, int state, int turn, int step, int fromState, char command,
                        bool usedMacro) {
        if (step > T) {
            return;
        }
        u32& checked = nodeChecked[state];
        if (checked == currentNodeMark) {
            Node& nextNode = nodeData[state];
            VASSERT(nextNode.turn_ <= turn);
            if (nextNode.turn_ != turn || nextNode.step_ <= step) {
                return;
            }
            nextNode.step_ = step;
            if (needCommands) {
                nextNode.fromState_ = fromState;
                nextNode.command_ = command;
            }
            nextNode.usedMacro_ = usedMacro;
        }
        else {
            checked = currentNodeMark;
            Node& nextNode = nodeData[state];
            nextNode.turn_ = turn;
            nextNode.step_ = step;
            if (needCommands) {
                nextNode.fromState_ = fromState;
                nextNode.command_ = command;
            }
            nextNode.usedMacro_ = usedMacro;
            layerNodes.push(state);
        }
    };

    auto BuildNodeCommands = [&](int state, dvec<char>& commands) {
        commands.clear();
        static dvec<char> reversed;
        reversed.clear();

        while (state >= 0) {
            const Node& node = nodeData[state];
            if (node.fromState_ < 0) {
                break;
            }
            reversed.push(node.command_);
            state = node.fromState_;
        }
        for (int i = (int)reversed.size() - 1; i >= 0; --i) {
            commands.push(reversed[i]);
        }
    };

    int initialState = ToState(startCell, startDi, initialBallCells_[targetBall]);
    nodeChecked[initialState] = currentNodeMark;
    Node& initialNode = nodeData[initialState];
    initialNode.turn_ = 0;
    initialNode.step_ = 0;
    if (needCommands) {
        initialNode.fromState_ = -1;
        initialNode.command_ = '*';
    }
    initialNode.usedMacro_ = false;
    curNodes.push(initialState);

    for (int turn = 0; !curNodes.empty(); ++turn) {
        int bestState = -1;
        int bestStep = INT_MAX;
        int bestPublicIdx = -1;
        REP(i, curNodes.size()) {
            int state = curNodes[i];
            const Node& node = nodeData[state];
            VASSERT(node.turn_ == turn);

            int publicIdx;
            int targetPos;
            publicIdx = state & publicIdxMask;
            targetPos = state >> publicIdxBits;
            if (targetPos != goalCell) {
                continue;
            }
            if (targetEndIdx >= 0 && publicIdx != targetEndIdx) {
                continue;
            }
            if (targetUsedMacro >= 0 && (node.usedMacro_ ? 1 : 0) != targetUsedMacro) {
                continue;
            }
            if (frontier) {
                frontier->options_.push(FindSegPathFrontierOption{publicIdx, node.turn_, node.step_, node.usedMacro_});
            }
            if (node.step_ < bestStep) {
                bestState = state;
                bestStep = node.step_;
                bestPublicIdx = publicIdx;
            }
        }
        if (bestState >= 0) {
            result.endIdx_ = bestPublicIdx;
            result.turn_ = turn;
            result.step_ = bestStep;
            result.usedMacro_ = nodeData[bestState].usedMacro_;
            if (needCommands) {
                static dvec<char> commands;
                BuildNodeCommands(bestState, commands);
                result.commands_ = commands;
            }
            else {
                result.commands_.clear();
            }
            return true;
        }
        if (turn >= T) {
            break;
        }

        nextNodes.clear();
        int nextTurn = turn + 1;
        REP(i, curNodes.size()) {
            int state = curNodes[i];
            const Node& node = nodeData[state];
            VASSERT(node.turn_ == turn);
            if (node.step_ >= T) {
                continue;
            }

            int publicIdx;
            int targetPos;
            publicIdx = state & publicIdxMask;
            targetPos = state >> publicIdxBits;
            int stateBase = targetPos << publicIdxBits;
            int nextStep = node.step_ + 1;

            int nextF = nextPublicF[publicIdx];
            if (nextF >= 0) {
                PushNext(nextNodes, stateBase | nextF, nextTurn, nextStep, state, 'F', node.usedMacro_);
            }
            PushNext(nextNodes, stateBase | nextPublicL[publicIdx], nextTurn, nextStep, state, 'L', node.usedMacro_);
            PushNext(nextNodes, stateBase | nextPublicR[publicIdx], nextTurn, nextStep, state, 'R', node.usedMacro_);

            int nextTargetPos = targetPos;
            int nextHeldOtherOrigin = noneCell;
            if (ApplyS(publicIdx, nextTargetPos, nextHeldOtherOrigin, false)) {
                VASSERT(nextHeldOtherOrigin == noneCell);
                PushNext(nextNodes, (nextTargetPos << publicIdxBits) | publicIdx, nextTurn, nextStep, state, 'S',
                         node.usedMacro_);
            }

            int macroState;
            int macroStep = node.step_ + macroSize;
            if (macroSize > 0 && macroStep <= T && ApplyMacroPublic(publicIdx, targetPos, macroState)) {
                PushNext(nextNodes, macroState, nextTurn, macroStep, state, 'P', true);
            }
        }
        swap(curNodes, nextNodes);
    }

    return false;
}

template <class MACRO>
bool FindSegPath(int startCell, int startDi, int targetBall, const MArr<bool>& ballsCompleted, const MACRO& macro,
                 FindSegPathResult& result, bool needCommands = true, int targetEndIdx = -1,
                 FindSegPathFrontierResult* frontier = nullptr, int targetUsedMacro = -1) {
    int publicStateCount = NN * 4;
    if (publicStateCount <= (1 << 9)) {
        return FindSegPathImpl<9>(startCell, startDi, targetBall, ballsCompleted, macro, result, needCommands,
                                  targetEndIdx, frontier, targetUsedMacro);
    }
    if (publicStateCount <= (1 << 10)) {
        return FindSegPathImpl<10>(startCell, startDi, targetBall, ballsCompleted, macro, result, needCommands,
                                   targetEndIdx, frontier, targetUsedMacro);
    }
    return FindSegPathImpl<FindSegPathMaxPublicIdxBits>(startCell, startDi, targetBall, ballsCompleted, macro, result,
                                                        needCommands, targetEndIdx, frontier, targetUsedMacro);
}

template <class MACRO>
bool FindSegPathUniversalWithCacheKey(FindSegPathUniversalCache& cache, int startCell, int startDi, int targetBall,
                                      const MArr<bool>& ballsCompleted, const MACRO& macro, FindSegPathResult& result,
                                      bool needCommands, u64 cacheKey) {
    result.Clear();
    VASSERT(0 <= startCell && startCell < NN);
    VASSERT(0 <= startDi && startDi < 4);
    VASSERT(0 <= targetBall && targetBall < M);
    VASSERT((int)ballsCompleted.size() == M);

    if (ballsCompleted[targetBall]) {
        result.endIdx_ = ToCellDirIdx(startCell, startDi);
        result.turn_ = 0;
        result.step_ = 0;
        result.usedMacro_ = false;
        return true;
    }

    auto& statCache = cache.statCache_;
    VASSERT(statCache.capacity() > 0);

    if (FindSegPathStat* stat = statCache.try_get(cacheKey)) {
        if (!needCommands || !stat->found_) {
            result.endIdx_ = stat->endIdx_;
            result.turn_ = stat->turn_;
            result.step_ = stat->step_;
            result.usedMacro_ = stat->usedMacro_;
            result.commands_.clear();
            return stat->found_;
        }
    }

    bool found = FindSegPath(startCell, startDi, targetBall, ballsCompleted, macro, result, needCommands);

    FindSegPathStat stat;
    stat.found_ = found;
    stat.endIdx_ = result.endIdx_;
    stat.turn_ = result.turn_;
    stat.step_ = result.step_;
    stat.usedMacro_ = result.usedMacro_;
    if (statCache.size() + 1 < statCache.capacity()) {
        statCache.set(cacheKey, stat);
    }
    return found;
}

template <class MACRO>
bool FindSegPathUniversalWithCache(FindSegPathUniversalCache& cache, int startCell, int startDi, int targetBall,
                                   const MArr<bool>& ballsCompleted, const MACRO& macro, FindSegPathResult& result,
                                   bool needCommands = false) {
    u64 cacheKey = CalcFindSegPathUniversalCacheKey(startCell, startDi, targetBall, macro);
    return FindSegPathUniversalWithCacheKey(cache, startCell, startDi, targetBall, ballsCompleted, macro, result,
                                            needCommands, cacheKey);
}

template <class MACRO>
bool FindSegPathUniversalWithCacheMacroHash(FindSegPathUniversalCache& cache, int startCell, int startDi,
                                            int targetBall, const MArr<bool>& ballsCompleted, const MACRO& macro,
                                            int macroSize, u64 macroHash, FindSegPathResult& result,
                                            bool needCommands = false) {
    u64 cacheKey = CalcFindSegPathUniversalCacheKeyByMacroHash(startCell, startDi, targetBall, macroSize, macroHash);
    return FindSegPathUniversalWithCacheKey(cache, startCell, startDi, targetBall, ballsCompleted, macro, result,
                                            needCommands, cacheKey);
}

inline bool TryGetFindSegPathUniversalStatCache(FindSegPathUniversalCache& cache, u64 cacheKey,
                                                FindSegPathResult& result, bool& foundPath) {
    auto& statCache = cache.statCache_;
    VASSERT(statCache.capacity() > 0);
    if (FindSegPathStat* stat = statCache.try_get(cacheKey)) {
        result.Clear();
        result.endIdx_ = stat->endIdx_;
        result.turn_ = stat->turn_;
        result.step_ = stat->step_;
        result.usedMacro_ = stat->usedMacro_;
        result.commands_.clear();
        foundPath = stat->found_;
        return true;
    }
    return false;
}

template <class MACRO>
bool FindSegPathUniversalFrontierWithCache(FindSegPathUniversalCache& cache, int startCell, int startDi, int targetBall,
                                           const MArr<bool>& ballsCompleted, const MACRO& macro,
                                           FindSegPathFrontierResult& result) {
    result.Clear();
    VASSERT(0 <= startCell && startCell < NN);
    VASSERT(0 <= startDi && startDi < 4);
    VASSERT(0 <= targetBall && targetBall < M);
    VASSERT((int)ballsCompleted.size() == M);

    if (ballsCompleted[targetBall]) {
        result.options_.push(FindSegPathFrontierOption{ToCellDirIdx(startCell, startDi), 0, 0, false});
        return true;
    }

    u64 cacheKey = CalcFindSegPathUniversalCacheKey(startCell, startDi, targetBall, macro);

    auto& frontierCache = cache.frontierCache_;
    VASSERT(frontierCache.capacity() > 0);

    if (FindSegPathUniversalFrontierCacheEntry* entry = frontierCache.try_get(cacheKey)) {
        result.optionsRef_ = &entry->options_;
        return entry->found_;
    }

    FindSegPathResult bestResult;
    bool found = FindSegPath(startCell, startDi, targetBall, ballsCompleted, macro, bestResult, false, -1, &result);

    FindSegPathUniversalFrontierCacheEntry entry;
    entry.found_ = found;
    entry.options_ = result.options_;
    if (frontierCache.size() + 1 < frontierCache.capacity()) {
        frontierCache.set(cacheKey, entry);
    }
    return found;
}

template <class MACRO>
bool FindSegPathUniversalToEnd(int startCell, int startDi, int targetBall, const MArr<bool>& ballsCompleted,
                               const MACRO& macro, int targetEndIdx, FindSegPathResult& result,
                               int targetUsedMacro = -1) {
    bool found = FindSegPath(startCell, startDi, targetBall, ballsCompleted, macro, result, true, targetEndIdx, nullptr,
                             targetUsedMacro);
    return found && result.endIdx_ == targetEndIdx &&
           (targetUsedMacro < 0 || (result.usedMacro_ ? 1 : 0) == targetUsedMacro);
}

constexpr int MacroCacheMacroMax = 64;

struct MacroCacheMacro {
    array<char, MacroCacheMacroMax> data_;
    array<u8, MacroCacheMacroMax> ops_;
    u16 size_ = 0;

    int size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void clear() {
        size_ = 0;
    }

    char& operator[](int i) {
        return data_[i];
    }

    const char& operator[](int i) const {
        return data_[i];
    }

    char* begin() {
        return data_.data();
    }

    char* end() {
        return data_.data() + size_;
    }

    const char* begin() const {
        return data_.data();
    }

    const char* end() const {
        return data_.data() + size_;
    }

    u8 op(int i) const {
        return ops_[i];
    }

    u8 ToOp(char c) const {
        if (c == 'F') {
            return 0;
        }
        if (c == 'L') {
            return 1;
        }
        if (c == 'R') {
            return 2;
        }
        return 3;
    }

    bool Same(const string& macro) const {
        if (size_ != macro.size()) {
            return false;
        }
        REP(i, size_) {
            if (data_[i] != macro[i]) {
                return false;
            }
        }
        return true;
    }

    template <int CAP>
    bool Same(const CapArr<char, CAP>& macro) const {
        if (size_ != macro.size()) {
            return false;
        }
        REP(i, size_) {
            if (data_[i] != macro[i]) {
                return false;
            }
        }
        return true;
    }

    void Assign(const string& macro) {
        int len = (int)macro.size();
        if (len > MacroCacheMacroMax) {
            VABORT();
            len = MacroCacheMacroMax;
        }
        size_ = (u16)len;
        REP(i, len) {
            data_[i] = macro[i];
            ops_[i] = ToOp(macro[i]);
        }
    }

    template <int CAP>
    void Assign(const CapArr<char, CAP>& macro) {
        int len = macro.size();
        if (len > MacroCacheMacroMax) {
            VABORT();
            len = MacroCacheMacroMax;
        }
        size_ = (u16)len;
        REP(i, len) {
            data_[i] = macro[i];
            ops_[i] = ToOp(macro[i]);
        }
    }
};

struct MacroCache {
    MacroCacheMacro macro_;
    mutable CapArr<i16, NN_max * 4> cache_;

    void InitNoOp() {
        macro_.clear();
        cache_.assign(NN * 4, -1);
    }

    void Init(const string& macro) {
        if (!macro_.Same(macro) || cache_.empty()) {
            macro_.Assign(macro);
            cache_.assign(NN * 4, -1);
        }
    }

    template <int CAP>
    void Init(const CapArr<char, CAP>& macro) {
        if (macro_.Same(macro) && !cache_.empty()) {
            return;
        }

        macro_.Assign(macro);
        cache_.assign(NN * 4, -1);
    }

    int CalcAppliedIdxUncached(int workCell, int workDi) const {
        return CalcAppliedIdxUncached(ToCellDirIdx(workCell, workDi));
    }

    int CalcAppliedIdxUncached(int idx) const {
        REP(i, macro_.size()) {
            u8 op = macro_.op(i);
            VASSERT(op < 3);
            idx = gs.nextMacroCellDirIdx(idx)[op];
        }
        return idx;
    }

    int CalcAppliedIdx(int idx) const {
        int cacheIdx = (idx & 3) * NN + (idx >> 2);
        int cached = cache_[cacheIdx];
        if (cached >= 0) {
            return cached;
        }

        int nextIdx = CalcAppliedIdxUncached(idx);
        cache_[cacheIdx] = (i16)nextIdx;
        return nextIdx;
    }
};

struct FindPathResult {
    CapArr<int, NN_max * 4> dists_;          
    CapArr<int, NN_max * 4> steps_;          
    CapArr<int, NN_max * 4> froms_;          
    CapArr<char, NN_max * 4> fromCommands_;  

    int GetDist(int idx) const {
        return dists_[idx];
    }
    int GetStep(int idx) const {
        return steps_[idx];
    }

    bool IsUsedMacro(int idx) const {
        while (true) {
            int fromIdx = froms_[idx];
            if (fromIdx < 0) {
                break;
            }
            char c = fromCommands_[idx];
            if (c == 'P') {
                return true;
            }
            idx = fromIdx;
        }
        return false;
    }

    void BuildCommand(int idx, dvec<char>& commands) const {
        commands.clear();

        while (true) {
            int fromIdx = froms_[idx];
            if (fromIdx < 0) {
                break;
            }
            char c = fromCommands_[idx];
            commands.push(c);
            idx = fromIdx;
        }

        reverse(ALL(commands));
    }

    void BuildRandComman(int idx, dvec<char>& commands) {
        commands.clear();



        while (true) {
            int fromIdx = froms_[idx];
            if (fromIdx < 0) {
                break;
            }
            char c = fromCommands_[idx];
            commands.push(c);
            idx = fromIdx;
        }

        reverse(ALL(commands));
    }
};

void FindPath(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache,
              FindPathResult& result, bool firstFindEnd = true) {
    VASSERT(!endIdxs.empty());

    static CheckMapS<NN_max * 4> targets;
    targets.Clear();
    int targetCount = 0;
    for (int idx : endIdxs) {
        if (!targets[idx]) {
            targets.Check(idx);
            ++targetCount;
        }
    }
    int completCount = 0;

    int macroStep = macroCache.macro_.size();

    static array<CapArr<int, NN_max * 4>, 2> ques;  
    static CheckMapS<NN_max * 4> pushed;            

    auto& dists = result.dists_;                
    auto& steps = result.steps_;                
    auto& froms = result.froms_;                
    auto& fromCommands = result.fromCommands_;  

    {
        auto& que = ques[0];
        que.clear();
        pushed.Clear();

        dists.assign(NN * 4, INT_MAX);
        steps.resize(NN * 4);         
        froms.resize(NN * 4);         
        fromCommands.resize(NN * 4);  

        for (int idx : startIdxs) {
            dists[idx] = 0;
            steps[idx] = 0;
            froms[idx] = -1;
            fromCommands[idx] = '*';

            if (pushed.Enter(idx)) {
                que.push(idx);
            }
        }
    }

    auto Push = [&](CapArr<int, NN_max * 4>& nextQue, CheckMapS<NN_max * 4>& pushed, int nextCell, int nextDi,
                    int nextDist, int nextStep, int curIdx, char command) {
        int nextIdx = ToCellDirIdx(nextCell, nextDi);

        if (nextDist < dists[nextIdx] || (nextDist == dists[nextIdx] && nextStep < steps[nextIdx])) {
            dists[nextIdx] = nextDist;
            steps[nextIdx] = nextStep;
            froms[nextIdx] = curIdx;
            fromCommands[nextIdx] = command;

            if (pushed.Enter(nextIdx)) {
                nextQue.push(nextIdx);
            }
        }
    };

    int curQueIdx = 0;

    while (!ques[curQueIdx].empty()) {
        auto& curQue = ques[curQueIdx];
        auto& nextQue = ques[curQueIdx ^ 1];
        nextQue.clear();
        pushed.Clear();

        for (int curIdx : curQue) {
            if (targets[curIdx]) {
                targets.reset(curIdx);
                ++completCount;
                if (completCount == targetCount) {
                    break;
                }
            }

            int curCell;
            int curDi;
            FromCellDirIdx(curIdx, curCell, curDi);

            {
                int nextCell = gs.dirs(curCell)[curDi];
                if (nextCell >= 0) {
                    Push(nextQue, pushed, nextCell, curDi, dists[curIdx] + 1, steps[curIdx] + 1, curIdx, 'F');
                }
            }

            {
                Push(nextQue, pushed, curCell, (curDi + 3) & 3, dists[curIdx] + 1, steps[curIdx] + 1, curIdx, 'L');
            }

            {
                Push(nextQue, pushed, curCell, (curDi + 1) & 3, dists[curIdx] + 1, steps[curIdx] + 1, curIdx, 'R');
            }

            {
                int nextIdx = macroCache.CalcAppliedIdx(curIdx);
                int nextCell;
                int nextDi;
                FromCellDirIdx(nextIdx, nextCell, nextDi);

                Push(nextQue, pushed, nextCell, nextDi, dists[curIdx] + 1, steps[curIdx] + macroStep, curIdx, 'P');
            }
        }
        if (firstFindEnd) {
            if (completCount > 0) {
                break;
            }
        }
        if (completCount == targetCount) {
            break;
        }

        curQueIdx ^= 1;
    }
}

enum class FindPathMacroSMode : u8 {
    Pickup,
    Put,
};

constexpr int FindPathMacroSStateMax = NN_max * 4 * (NN_max + 1);

struct FindPathMacroSResult {
    CapArr<int, NN_max * 4> dists_;                    
    CapArr<int, NN_max * 4> steps_;                    
    CapArr<int, NN_max * 4> resultStates_;             
    CapArr<int, FindPathMacroSStateMax> stateDists_;   
    CapArr<int, FindPathMacroSStateMax> stateSteps_;   
    CapArr<int, FindPathMacroSStateMax> stateFroms_;   
    CapArr<char, FindPathMacroSStateMax> stateCommands_;  

    int GetDist(int idx) const {
        return dists_[idx];
    }

    int GetStep(int idx) const {
        return steps_[idx];
    }

    bool IsUsedMacro(int idx) const {
        int state = resultStates_[idx];
        if (state < 0) {
            return false;
        }

        while (true) {
            int fromState = stateFroms_[state];
            if (fromState < 0) {
                break;
            }
            if (stateCommands_[state] == 'P') {
                return true;
            }
            state = fromState;
        }
        return false;
    }

    void BuildCommand(int idx, dvec<char>& commands) const {
        commands.clear();

        int state = resultStates_[idx];
        VASSERT(state >= 0);
        while (true) {
            int fromState = stateFroms_[state];
            if (fromState < 0) {
                break;
            }
            commands.push(stateCommands_[state]);
            state = fromState;
        }

        reverse(ALL(commands));
    }
};

void FindPathWithMacroS(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache,
                        const NNArr<i8>& ballGrid, FindPathMacroSMode mode, FindPathMacroSResult& result,
                        bool firstFindEnd = true) {
    VASSERT(!endIdxs.empty());

    int noneCell = NN;
    int cellStateCount = NN + 1;
    int stateCount = NN * 4 * cellStateCount;
    VASSERT(stateCount <= FindPathMacroSStateMax);

    auto ToState = [&](int cell, int di, int tempCell) {
        return ToCellDirIdx(cell, di) * cellStateCount + tempCell;
    };
    auto FromState = [&](int state, int& cell, int& di, int& tempCell) {
        tempCell = state % cellStateCount;
        int publicIdx = state / cellStateCount;
        FromCellDirIdx(publicIdx, cell, di);
    };
    auto ToPublicIdx = [&](int state) {
        return state / cellStateCount;
    };

    static CheckMapS<NN_max * 4> targets;
    static CheckMapS<NN_max * 4> foundTargets;
    targets.Clear();
    foundTargets.Clear();
    int targetCount = 0;
    for (int idx : endIdxs) {
        if (!targets[idx]) {
            targets.Check(idx);
            ++targetCount;
        }
    }
    int completeCount = 0;

    auto& dists = result.dists_;
    auto& steps = result.steps_;
    auto& resultStates = result.resultStates_;
    auto& stateDists = result.stateDists_;
    auto& stateSteps = result.stateSteps_;
    auto& stateFroms = result.stateFroms_;
    auto& stateCommands = result.stateCommands_;

    dists.assign(NN * 4, INT_MAX);
    steps.resize(NN * 4);
    resultStates.assign(NN * 4, -1);
    stateDists.assign(stateCount, INT_MAX);
    stateSteps.resize(stateCount);
    stateFroms.resize(stateCount);
    stateCommands.resize(stateCount);

    auto IsTerminalState = [&](int tempCell) {
        return tempCell == noneCell;
    };

    auto UpdatePublicResult = [&](int state) {
        int cell;
        int di;
        int tempCell;
        FromState(state, cell, di, tempCell);
        if (!IsTerminalState(tempCell)) {
            return;
        }

        int publicIdx = ToCellDirIdx(cell, di);
        if (!targets[publicIdx]) {
            return;
        }

        int dist = stateDists[state];
        int step = stateSteps[state];
        if (dist < dists[publicIdx] || (dist == dists[publicIdx] && step < steps[publicIdx])) {
            dists[publicIdx] = dist;
            steps[publicIdx] = step;
            resultStates[publicIdx] = state;
        }
        if (foundTargets.Enter(publicIdx)) {
            ++completeCount;
        }
    };

    static array<CapArr<int, FindPathMacroSStateMax>, 2> ques;
    static CheckMapS<FindPathMacroSStateMax> pushed;

    {
        auto& que = ques[0];
        que.clear();
        pushed.Clear();

        for (int idx : startIdxs) {
            int cell;
            int di;
            FromCellDirIdx(idx, cell, di);
            int state = ToState(cell, di, noneCell);
            stateDists[state] = 0;
            stateSteps[state] = 0;
            stateFroms[state] = -1;
            stateCommands[state] = '*';
            UpdatePublicResult(state);

            if (pushed.Enter(state)) {
                que.push(state);
            }
        }
    }

    auto PushState = [&](CapArr<int, FindPathMacroSStateMax>& nextQue, CheckMapS<FindPathMacroSStateMax>& pushed,
                         int nextState, int nextDist, int nextStep, int curState, char command) {
        if (nextDist < stateDists[nextState] ||
            (nextDist == stateDists[nextState] && nextStep < stateSteps[nextState])) {
            stateDists[nextState] = nextDist;
            stateSteps[nextState] = nextStep;
            stateFroms[nextState] = curState;
            stateCommands[nextState] = command;
            UpdatePublicResult(nextState);

            if (pushed.Enter(nextState)) {
                nextQue.push(nextState);
            }
        }
    };

    auto ApplyCommand = [&](int state, char command, bool inMacro, int& nextState) {
        int cell;
        int di;
        int tempCell;
        FromState(state, cell, di, tempCell);

        if (command == 'F') {
            int nextCell = gs.dirs(cell)[di];
            if (nextCell < 0) {
                if (!inMacro) {
                    return false;
                }
                nextCell = cell;
            }
            nextState = ToState(nextCell, di, tempCell);
            return true;
        }
        if (command == 'L') {
            nextState = ToState(cell, (di + 3) & 3, tempCell);
            return true;
        }
        if (command == 'R') {
            nextState = ToState(cell, (di + 1) & 3, tempCell);
            return true;
        }
        if (command == 'S') {
            if (mode == FindPathMacroSMode::Pickup) {
                if (tempCell == noneCell) {
                    int ball = ballGrid[cell];
                    nextState = ToState(cell, di, ball >= 0 ? cell : noneCell);
                    return true;
                }
                if (cell != tempCell) {
                    return false;
                }
                nextState = ToState(cell, di, noneCell);
                return true;
            }

            if (tempCell == noneCell) {
                if (ballGrid[cell] >= 0) {
                    return false;
                }
                nextState = ToState(cell, di, cell);
                return true;
            }
            if (cell == tempCell) {
                nextState = ToState(cell, di, noneCell);
                return true;
            }
            if (ballGrid[cell] >= 0) {
                return false;
            }
            nextState = state;
            return true;
        }

        VABORT();
        return false;
    };

    auto ApplyMacro = [&](int state, int& nextState) {
        int workState = state;
        for (char command : macroCache.macro_) {
            int tmpState;
            if (!ApplyCommand(workState, command, true, tmpState)) {
                return false;
            }
            workState = tmpState;
        }
        nextState = workState;
        return true;
    };

    int macroStep = macroCache.macro_.size();
    int curQueIdx = 0;

    while (!ques[curQueIdx].empty()) {
        auto& curQue = ques[curQueIdx];
        auto& nextQue = ques[curQueIdx ^ 1];
        nextQue.clear();
        pushed.Clear();

        for (int curState : curQue) {
            for (char command : {'F', 'L', 'R', 'S'}) {
                int nextState;
                if (ApplyCommand(curState, command, false, nextState)) {
                    PushState(nextQue, pushed, nextState, stateDists[curState] + 1, stateSteps[curState] + 1,
                              curState, command);
                }
            }

            int macroState;
            if (ApplyMacro(curState, macroState)) {
                PushState(nextQue, pushed, macroState, stateDists[curState] + 1, stateSteps[curState] + macroStep,
                          curState, 'P');
            }
        }

        if (firstFindEnd && completeCount > 0) {
            break;
        }
        if (completeCount == targetCount) {
            break;
        }

        curQueIdx ^= 1;
    }
}

struct FindBallToGoalPathWithMacroSResult {
    struct Node {
        u64 key_ = 0;
        int dist_ = INT_MAX;
        int step_ = INT_MAX;
        int from_ = -1;
        char command_ = 0;
    };

    dvec<Node> nodes_;
    int bestNode_ = -1;
    int dist_ = INT_MAX;
    int step_ = INT_MAX;
    int endIdx_ = -1;

    bool Found() const {
        return bestNode_ >= 0;
    }

    int GetDist() const {
        return dist_;
    }

    int GetStep() const {
        return step_;
    }

    bool IsUsedMacro() const {
        int nodeIdx = bestNode_;
        while (nodeIdx >= 0) {
            cauto& node = nodes_[nodeIdx];
            if (node.command_ == 'P') {
                return true;
            }
            nodeIdx = node.from_;
        }
        return false;
    }

    void BuildCommand(dvec<char>& commands) const {
        commands.clear();
        VASSERT(bestNode_ >= 0);

        int nodeIdx = bestNode_;
        while (nodeIdx >= 0) {
            cauto& node = nodes_[nodeIdx];
            if (node.from_ < 0) {
                break;
            }
            commands.push(node.command_);
            nodeIdx = node.from_;
        }
        reverse(ALL(commands));
    }
};

bool FindBallToGoalPathWithMacroS(const dvec<int>& startIdxs, int targetBall, const MacroCache& macroCache,
                                  const NNArr<i8>& ballGrid, FindBallToGoalPathWithMacroSResult& result,
                                  int goalCell = -1) {
    VASSERT(!startIdxs.empty());
    VASSERT(0 <= targetBall && targetBall < M);
    if (goalCell < 0) {
        goalCell = ballToGoalCell_[targetBall];
    }

    int noneCell = NN;
    int posCount = NN + 1;
    int targetStartCell = -1;
    REP(cell, NN) {
        if (ballGrid[cell] == targetBall) {
            targetStartCell = cell;
            break;
        }
    }
    if (targetStartCell < 0) {
        return false;
    }

    int reserveNodeCount = min(1 << 20, max(65536, NN * 4 * 256));
    result.nodes_.clear();
    result.nodes_.reserve(reserveNodeCount);
    result.bestNode_ = -1;
    result.dist_ = INT_MAX;
    result.step_ = INT_MAX;
    result.endIdx_ = -1;

    auto ToKey = [&](int cell, int di, int targetPos, int heldOtherOrigin) {
        return (((u64)ToCellDirIdx(cell, di) * posCount + targetPos) * posCount + heldOtherOrigin);
    };
    auto FromKey = [&](u64 key, int& cell, int& di, int& targetPos, int& heldOtherOrigin) {
        heldOtherOrigin = (int)(key % posCount);
        key /= posCount;
        targetPos = (int)(key % posCount);
        key /= posCount;
        FromCellDirIdx((int)key, cell, di);
    };

    auto CellContent = [&](int cell, int targetPos, int heldOtherOrigin) {
        if (targetPos == cell) {
            return 1;  
        }
        int ball = ballGrid[cell];
        if (ball >= 0 && ball != targetBall && cell != heldOtherOrigin) {
            return 2;  
        }
        return 0;  
    };

    auto IsBetter = [](int dist, int step, int curDist, int curStep) {
        return dist < curDist || (dist == curDist && step < curStep);
    };

    auto UpdateBest = [&](int nodeIdx) {
        cauto& node = result.nodes_[nodeIdx];
        int cell;
        int di;
        int targetPos;
        int heldOtherOrigin;
        FromKey(node.key_, cell, di, targetPos, heldOtherOrigin);
        if (targetPos != goalCell || heldOtherOrigin != noneCell) {
            return;
        }

        if (IsBetter(node.dist_, node.step_, result.dist_, result.step_)) {
            result.bestNode_ = nodeIdx;
            result.dist_ = node.dist_;
            result.step_ = node.step_;
            result.endIdx_ = ToCellDirIdx(cell, di);
        }
    };

    unordered_map<u64, int> nodeIdxs;
    nodeIdxs.reserve(reserveNodeCount);

    auto GetNodeIdx = [&](u64 key) {
        auto it = nodeIdxs.find(key);
        if (it != nodeIdxs.end()) {
            return it->second;
        }

        int nodeIdx = result.nodes_.size();
        FindBallToGoalPathWithMacroSResult::Node node;
        node.key_ = key;
        result.nodes_.push(node);
        nodeIdxs.emplace(key, nodeIdx);
        return nodeIdx;
    };

    array<dvec<int>, 2> ques;
    auto PushNode = [&](dvec<int>& nextQue, u64 key, int dist, int step, int from, char command) {
        int nodeIdx = GetNodeIdx(key);
        auto& node = result.nodes_[nodeIdx];
        if (!IsBetter(dist, step, node.dist_, node.step_)) {
            return;
        }

        node.dist_ = dist;
        node.step_ = step;
        node.from_ = from;
        node.command_ = command;
        UpdateBest(nodeIdx);
        nextQue.push(nodeIdx);
    };

    auto ApplyCommand = [&](u64 key, char command, bool inMacro, u64& nextKey) {
        int cell;
        int di;
        int targetPos;
        int heldOtherOrigin;
        FromKey(key, cell, di, targetPos, heldOtherOrigin);

        if (command == 'F') {
            int nextCell = gs.dirs(cell)[di];
            if (nextCell < 0) {
                if (!inMacro) {
                    return false;
                }
                nextCell = cell;
            }
            nextKey = ToKey(nextCell, di, targetPos, heldOtherOrigin);
            return true;
        }
        if (command == 'L') {
            nextKey = ToKey(cell, (di + 3) & 3, targetPos, heldOtherOrigin);
            return true;
        }
        if (command == 'R') {
            nextKey = ToKey(cell, (di + 1) & 3, targetPos, heldOtherOrigin);
            return true;
        }
        if (command == 'S') {
            int content = CellContent(cell, targetPos, heldOtherOrigin);

            if (targetPos == noneCell) {
                VASSERT(heldOtherOrigin == noneCell);
                if (content == 0) {
                    targetPos = cell;
                }
                else if (content == 2) {
                    targetPos = cell;
                    heldOtherOrigin = cell;
                }
                else {
                    VABORT();
                }
            }
            else if (heldOtherOrigin != noneCell) {
                if (cell != heldOtherOrigin) {
                    return false;
                }
                if (content == 1) {
                    targetPos = noneCell;
                    heldOtherOrigin = noneCell;
                }
                else if (content == 0) {
                    heldOtherOrigin = noneCell;
                }
                else {
                    return false;
                }
            }
            else {
                if (content == 1) {
                    targetPos = noneCell;
                }
                else if (content == 2) {
                    heldOtherOrigin = cell;
                }
            }

            nextKey = ToKey(cell, di, targetPos, heldOtherOrigin);
            return true;
        }

        VABORT();
        return false;
    };

    auto ApplyMacro = [&](u64 key, u64& nextKey) {
        u64 workKey = key;
        for (char command : macroCache.macro_) {
            u64 tmpKey;
            if (!ApplyCommand(workKey, command, true, tmpKey)) {
                return false;
            }
            workKey = tmpKey;
        }
        nextKey = workKey;
        return true;
    };

    ques[0].clear();
    ques[1].clear();
    for (int idx : startIdxs) {
        int cell;
        int di;
        FromCellDirIdx(idx, cell, di);
        u64 key = ToKey(cell, di, targetStartCell, noneCell);
        int nodeIdx = GetNodeIdx(key);
        auto& node = result.nodes_[nodeIdx];
        if (IsBetter(0, 0, node.dist_, node.step_)) {
            node.dist_ = 0;
            node.step_ = 0;
            node.from_ = -1;
            node.command_ = '*';
            UpdateBest(nodeIdx);
            ques[0].push(nodeIdx);
        }
    }

    if (result.Found()) {
        return true;
    }

    int macroStep = macroCache.macro_.size();
    int curQueIdx = 0;
    int curDist = 0;

    while (!ques[curQueIdx].empty()) {
        auto& curQue = ques[curQueIdx];
        auto& nextQue = ques[curQueIdx ^ 1];
        nextQue.clear();

        for (int nodeIdx : curQue) {
            cauto& node = result.nodes_[nodeIdx];
            if (node.dist_ != curDist) {
                continue;
            }

            for (char command : {'F', 'L', 'R', 'S'}) {
                u64 nextKey;
                if (ApplyCommand(node.key_, command, false, nextKey)) {
                    PushNode(nextQue, nextKey, node.dist_ + 1, node.step_ + 1, nodeIdx, command);
                }
            }

            u64 macroKey;
            if (ApplyMacro(node.key_, macroKey)) {
                PushNode(nextQue, macroKey, node.dist_ + 1, node.step_ + macroStep, nodeIdx, 'P');
            }
        }

        if (result.Found()) {
            break;
        }

        curQueIdx ^= 1;
        ++curDist;
    }

    return result.Found();
}


namespace bit_path_detail {

using CellBits = BitArr<NN_max>;

void SetIdx(array<CellBits, 4>& bits, int idx) {
    bits[idx & 3].set(idx >> 2);
}

bool HasCommon(const array<CellBits, 4>& lhs, const array<CellBits, 4>& rhs) {
    REP(di, 4) {
        if (lhs[di].HasCommon(rhs[di])) {
            return true;
        }
    }
    return false;
}

int FindReachedIdx(const array<CellBits, 4>& cur, const dvec<int>& endIdxs) {
    for (int idx : endIdxs) {
        int cell;
        int di;
        FromCellDirIdx(idx, cell, di);
        if (cur[di][cell]) {
            return idx;
        }
    }
    return -1;
}

int FindReachedTargetCellIdx(const array<CellBits, 4>& cur, int targetCell) {
    REP(di, 4) {
        if (cur[di][targetCell]) {
            return (targetCell << 2) | di;
        }
    }
    return -1;
}

int MarkReachedTargets(const array<CellBits, 4>& cur, array<CellBits, 4>& targets, CapArr<int, NN_max * 4>& turns,
                       int turn) {
    int reachedCount = 0;
    REP(di, 4) {
        CellBits reached = cur[di] & targets[di];
        reached.forEachSetBit([&](int cell) {
            int idx = ToCellDirIdx(cell, di);
            turns[idx] = turn;
            targets[di].reset(cell);
            ++reachedCount;
        });
    }
    return reachedCount;
}

void BuildMoveMasks(array<CellBits, 4>& moveMasks) {
    REP(di, 4) {
        moveMasks[di].clear();
    }

    REP(cell, NN) {
        REP(di, 4) {
            if (gs.dirs(cell)[di] >= 0) {
                moveMasks[di].set(cell);
            }
        }
    }
}

const array<CellBits, 4>& GetMoveMasks() {
    static array<CellBits, 4> moveMasks = {};
    static bool initialized = false;
    if (!initialized) {
        BuildMoveMasks(moveMasks);
        initialized = true;
    }
    return moveMasks;
}

void AddForwardMoves(const array<CellBits, 4>& cur, const array<CellBits, 4>& moveMasks, array<CellBits, 4>& next) {
    next[0].MaskedRightShiftOr<1>(cur[0], moveMasks[0]);
    next[1].MaskedRightShiftOr(cur[1], moveMasks[1], N);
    next[2].MaskedLeftShiftOr<1>(cur[2], moveMasks[2]);
    next[3].MaskedLeftShiftOr(cur[3], moveMasks[3], N);
}

void AddRotateMoves(const array<CellBits, 4>& cur, array<CellBits, 4>& next) {
    next[0].OrAssign2(cur[1], cur[3]);
    next[1].OrAssign2(cur[0], cur[2]);
    next[2].OrAssign2(cur[1], cur[3]);
    next[3].OrAssign2(cur[0], cur[2]);
}

template <int DI>
void AddMacroMovesDir(const CellBits& cur, const MacroCache& macroCache, i16* cache, array<CellBits, 4>& next) {
    i16* cacheDi = cache + DI * NN;
    cur.forEachSetBit([&](int cell) {
        int nextIdx = cacheDi[cell];
        if (nextIdx < 0) {
            nextIdx = macroCache.CalcAppliedIdxUncached(cell, DI);
            cacheDi[cell] = (i16)nextIdx;
        }
        next[nextIdx & 3].set(nextIdx >> 2);
    });
}

void AddMacroMoves(const array<CellBits, 4>& cur, const MacroCache& macroCache, array<CellBits, 4>& next) {
    i16* cache = macroCache.cache_.data();
    AddMacroMovesDir<0>(cur[0], macroCache, cache, next);
    AddMacroMovesDir<1>(cur[1], macroCache, cache, next);
    AddMacroMovesDir<2>(cur[2], macroCache, cache, next);
    AddMacroMovesDir<3>(cur[3], macroCache, cache, next);
}

template <int TN>
using CellBitsN = BitArr<TN * TN>;

template <int TN>
void SetIdxN(array<CellBitsN<TN>, 4>& bits, int idx) {
    bits[idx & 3].set(idx >> 2);
}

template <int TN>
bool HasCommonN(const array<CellBitsN<TN>, 4>& lhs, const array<CellBitsN<TN>, 4>& rhs) {
    if (lhs[0].HasCommon(rhs[0])) {
        return true;
    }
    if (lhs[1].HasCommon(rhs[1])) {
        return true;
    }
    if (lhs[2].HasCommon(rhs[2])) {
        return true;
    }
    return lhs[3].HasCommon(rhs[3]);
}

template <int TN>
int FindReachedIdxN(const array<CellBitsN<TN>, 4>& cur, const dvec<int>& endIdxs) {
    for (int idx : endIdxs) {
        int cell = idx >> 2;
        int di = idx & 3;
        if (cur[di][cell]) {
            return idx;
        }
    }
    return -1;
}

template <int TN>
int FindReachedTargetCellIdxN(const array<CellBitsN<TN>, 4>& cur, int targetCell) {
    if (cur[0][targetCell]) {
        return (targetCell << 2) | 0;
    }
    if (cur[1][targetCell]) {
        return (targetCell << 2) | 1;
    }
    if (cur[2][targetCell]) {
        return (targetCell << 2) | 2;
    }
    if (cur[3][targetCell]) {
        return (targetCell << 2) | 3;
    }
    return -1;
}

template <int TN>
int MarkReachedTargetsN(const array<CellBitsN<TN>, 4>& cur, array<CellBitsN<TN>, 4>& targets,
                        CapArr<int, NN_max * 4>& turns, int turn) {
    int reachedCount = 0;
    REP(di, 4) {
        CellBitsN<TN> reached = cur[di] & targets[di];
        reached.forEachSetBit([&](int cell) {
            int idx = (cell << 2) | di;
            turns[idx] = turn;
            targets[di].reset(cell);
            ++reachedCount;
        });
    }
    return reachedCount;
}

template <int TN>
void BuildMoveMasksN(array<CellBitsN<TN>, 4>& moveMasks) {
    REP(di, 4) {
        moveMasks[di].clear();
    }

    REP(cell, TN * TN) {
        REP(di, 4) {
            if (gs.dirs(cell)[di] >= 0) {
                moveMasks[di].set(cell);
            }
        }
    }
}

template <int TN>
const array<CellBitsN<TN>, 4>& GetMoveMasksN() {
    static array<CellBitsN<TN>, 4> moveMasks = {};
    static bool initialized = false;
    if (!initialized) {
        BuildMoveMasksN<TN>(moveMasks);
        initialized = true;
    }
    return moveMasks;
}

template <int TN>
void AddForwardMovesN(const array<CellBitsN<TN>, 4>& cur, const array<CellBitsN<TN>, 4>& moveMasks,
                      array<CellBitsN<TN>, 4>& next) {
    next[0].template MaskedRightShiftOr<1>(cur[0], moveMasks[0]);
    next[1].template MaskedRightShiftOr<TN>(cur[1], moveMasks[1]);
    next[2].template MaskedLeftShiftOr<1>(cur[2], moveMasks[2]);
    next[3].template MaskedLeftShiftOr<TN>(cur[3], moveMasks[3]);
}

template <int TN>
void AddRotateMovesN(const array<CellBitsN<TN>, 4>& cur, array<CellBitsN<TN>, 4>& next) {
    next[0].OrAssign2(cur[1], cur[3]);
    next[1].OrAssign2(cur[0], cur[2]);
    next[2].OrAssign2(cur[1], cur[3]);
    next[3].OrAssign2(cur[0], cur[2]);
}

template <int TN, int DI>
void AddMacroMovesDirN(const CellBitsN<TN>& cur, const MacroCache& macroCache, i16* cache,
                       array<CellBitsN<TN>, 4>& next) {
    i16* cacheDi = cache + DI * (TN * TN);
    cur.forEachSetBit([&](int cell) {
        int nextIdx = cacheDi[cell];
        if (nextIdx < 0) {
            nextIdx = macroCache.CalcAppliedIdxUncached(cell, DI);
            cacheDi[cell] = (i16)nextIdx;
        }
        next[nextIdx & 3].set(nextIdx >> 2);
    });
}

template <int TN>
void AddMacroMovesN(const array<CellBitsN<TN>, 4>& cur, const MacroCache& macroCache, array<CellBitsN<TN>, 4>& next) {
    i16* cache = macroCache.cache_.data();
    AddMacroMovesDirN<TN, 0>(cur[0], macroCache, cache, next);
    AddMacroMovesDirN<TN, 1>(cur[1], macroCache, cache, next);
    AddMacroMovesDirN<TN, 2>(cur[2], macroCache, cache, next);
    AddMacroMovesDirN<TN, 3>(cur[3], macroCache, cache, next);
}

template <int TN>
int BitCalcTurnTargetCellT(int startIdx, int targetCell, const MacroCache& macroCache, int& reachedIdx) {
    reachedIdx = -1;

    array<CellBitsN<TN>, 4> cur = {};
    array<CellBitsN<TN>, 4> visited = {};
    cauto& moveMasks = GetMoveMasksN<TN>();

    SetIdxN<TN>(cur, startIdx);
    SetIdxN<TN>(visited, startIdx);

    int turn = 0;
    while (true) {
        reachedIdx = FindReachedTargetCellIdxN<TN>(cur, targetCell);
        if (reachedIdx >= 0) {
            return turn;
        }

        array<CellBitsN<TN>, 4> next = {};
        AddForwardMovesN<TN>(cur, moveMasks, next);
        AddRotateMovesN<TN>(cur, next);
        AddMacroMovesN<TN>(cur, macroCache, next);

        bool any = false;
        REP(di, 4) {
            any |= next[di].FilterNewAndMark(visited[di]);
        }

        if (!any) {
            return INT_MAX;
        }

        cur = next;
        ++turn;
    }
}

template <int TN>
int BitCalcTurnT(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache, int& reachedIdx) {
    VASSERT(!startIdxs.empty());
    VASSERT(!endIdxs.empty());
    reachedIdx = -1;

    array<CellBitsN<TN>, 4> cur = {};
    array<CellBitsN<TN>, 4> visited = {};
    array<CellBitsN<TN>, 4> targets = {};
    cauto& moveMasks = GetMoveMasksN<TN>();

    for (int idx : startIdxs) {
        SetIdxN<TN>(cur, idx);
        SetIdxN<TN>(visited, idx);
    }
    for (int idx : endIdxs) {
        SetIdxN<TN>(targets, idx);
    }

    int turn = 0;
    while (true) {
        if (HasCommonN<TN>(cur, targets)) {
            reachedIdx = FindReachedIdxN<TN>(cur, endIdxs);
            VASSERT(reachedIdx >= 0);
            return turn;
        }

        array<CellBitsN<TN>, 4> next = {};
        AddForwardMovesN<TN>(cur, moveMasks, next);
        AddRotateMovesN<TN>(cur, next);
        AddMacroMovesN<TN>(cur, macroCache, next);

        bool any = false;
        REP(di, 4) {
            any |= next[di].FilterNewAndMark(visited[di]);
        }

        if (!any) {
            return INT_MAX;
        }

        cur = next;
        ++turn;
    }
}

template <int TN>
int BitCalcAllTurnsT(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache,
                     CapArr<int, NN_max * 4>& turnByIdx) {
    VASSERT(!startIdxs.empty());
    VASSERT(!endIdxs.empty());

    turnByIdx.assign(TN * TN * 4, INT_MAX);

    array<CellBitsN<TN>, 4> cur = {};
    array<CellBitsN<TN>, 4> visited = {};
    array<CellBitsN<TN>, 4> targets = {};
    cauto& moveMasks = GetMoveMasksN<TN>();

    for (int idx : startIdxs) {
        SetIdxN<TN>(cur, idx);
        SetIdxN<TN>(visited, idx);
    }
    for (int idx : endIdxs) {
        SetIdxN<TN>(targets, idx);
    }

    int targetCount = 0;
    REP(di, 4) {
        targetCount += targets[di].count();
    }

    int completeCount = 0;
    int turn = 0;
    while (true) {
        completeCount += MarkReachedTargetsN<TN>(cur, targets, turnByIdx, turn);
        if (completeCount == targetCount) {
            return turn;
        }

        array<CellBitsN<TN>, 4> next = {};
        AddForwardMovesN<TN>(cur, moveMasks, next);
        AddRotateMovesN<TN>(cur, next);
        AddMacroMovesN<TN>(cur, macroCache, next);

        bool any = false;
        REP(di, 4) {
            any |= next[di].FilterNewAndMark(visited[di]);
        }

        if (!any) {
            return INT_MAX;
        }

        cur = next;
        ++turn;
    }
}

bool PushPathState(FindPathResult& result, array<CellBits, 4>& next, int curIdx, int nextIdx, int nextDist,
                   int nextStep, char command) {
    auto& dists = result.dists_;
    auto& steps = result.steps_;
    auto& froms = result.froms_;
    auto& fromCommands = result.fromCommands_;

    if (nextDist < dists[nextIdx] || (nextDist == dists[nextIdx] && nextStep < steps[nextIdx])) {
        dists[nextIdx] = nextDist;
        steps[nextIdx] = nextStep;
        froms[nextIdx] = curIdx;
        fromCommands[nextIdx] = command;
        SetIdx(next, nextIdx);
        return true;
    }
    return false;
}

}  

int BitCalcTurn(int startIdx, int targetCell, const MacroCache& macroCache, int& reachedIdx) {
    using namespace bit_path_detail;

    switch (N) {
        case 10:
            return BitCalcTurnTargetCellT<10>(startIdx, targetCell, macroCache, reachedIdx);
        case 11:
            return BitCalcTurnTargetCellT<11>(startIdx, targetCell, macroCache, reachedIdx);
        case 12:
            return BitCalcTurnTargetCellT<12>(startIdx, targetCell, macroCache, reachedIdx);
        case 13:
            return BitCalcTurnTargetCellT<13>(startIdx, targetCell, macroCache, reachedIdx);
        case 14:
            return BitCalcTurnTargetCellT<14>(startIdx, targetCell, macroCache, reachedIdx);
        case 15:
            return BitCalcTurnTargetCellT<15>(startIdx, targetCell, macroCache, reachedIdx);
        case 16:
            return BitCalcTurnTargetCellT<16>(startIdx, targetCell, macroCache, reachedIdx);
        case 17:
            return BitCalcTurnTargetCellT<17>(startIdx, targetCell, macroCache, reachedIdx);
        case 18:
            return BitCalcTurnTargetCellT<18>(startIdx, targetCell, macroCache, reachedIdx);
        case 19:
            return BitCalcTurnTargetCellT<19>(startIdx, targetCell, macroCache, reachedIdx);
        case 20:
            return BitCalcTurnTargetCellT<20>(startIdx, targetCell, macroCache, reachedIdx);
    }
    VABORT();
    return INT_MAX;
}

int BitCalcTurn(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache, int& reachedIdx) {
    using namespace bit_path_detail;

    switch (N) {
        case 10:
            return BitCalcTurnT<10>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 11:
            return BitCalcTurnT<11>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 12:
            return BitCalcTurnT<12>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 13:
            return BitCalcTurnT<13>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 14:
            return BitCalcTurnT<14>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 15:
            return BitCalcTurnT<15>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 16:
            return BitCalcTurnT<16>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 17:
            return BitCalcTurnT<17>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 18:
            return BitCalcTurnT<18>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 19:
            return BitCalcTurnT<19>(startIdxs, endIdxs, macroCache, reachedIdx);
        case 20:
            return BitCalcTurnT<20>(startIdxs, endIdxs, macroCache, reachedIdx);
    }
    VABORT();
    return INT_MAX;
}

int BitCalcAllTurns(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache,
                    CapArr<int, NN_max * 4>& turnByIdx) {
    using namespace bit_path_detail;

    switch (N) {
        case 10:
            return BitCalcAllTurnsT<10>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 11:
            return BitCalcAllTurnsT<11>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 12:
            return BitCalcAllTurnsT<12>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 13:
            return BitCalcAllTurnsT<13>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 14:
            return BitCalcAllTurnsT<14>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 15:
            return BitCalcAllTurnsT<15>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 16:
            return BitCalcAllTurnsT<16>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 17:
            return BitCalcAllTurnsT<17>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 18:
            return BitCalcAllTurnsT<18>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 19:
            return BitCalcAllTurnsT<19>(startIdxs, endIdxs, macroCache, turnByIdx);
        case 20:
            return BitCalcAllTurnsT<20>(startIdxs, endIdxs, macroCache, turnByIdx);
    }
    VABORT();
    return INT_MAX;
}

int BitCalcAllTurns(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache,
                    dvec<int>& endTurns) {
    CapArr<int, NN_max * 4> turnByIdx;
    int result = BitCalcAllTurns(startIdxs, endIdxs, macroCache, turnByIdx);

    endTurns.resize(endIdxs.size());
    REP(i, endIdxs.size()) {
        endTurns[i] = turnByIdx[endIdxs[i]];
    }
    return result;
}

void BitFindPath(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& macroCache,
                 FindPathResult& result) {
    using namespace bit_path_detail;

    VASSERT(!startIdxs.empty());
    VASSERT(!endIdxs.empty());

    auto& dists = result.dists_;
    auto& steps = result.steps_;
    auto& froms = result.froms_;
    auto& fromCommands = result.fromCommands_;

    dists.assign(NN * 4, INT_MAX);
    steps.resize(NN * 4);
    froms.resize(NN * 4);
    fromCommands.resize(NN * 4);

    array<CellBits, 4> cur = {};
    array<CellBits, 4> targets = {};

    for (int idx : startIdxs) {
        dists[idx] = 0;
        steps[idx] = 0;
        froms[idx] = -1;
        fromCommands[idx] = '*';
        SetIdx(cur, idx);
    }
    for (int idx : endIdxs) {
        SetIdx(targets, idx);
    }

    int turn = 0;
    int macroStep = macroCache.macro_.size();
    while (true) {
        if (HasCommon(cur, targets)) {
            return;
        }

        array<CellBits, 4> next = {};
        bool any = false;

        REP(curDi, 4) {
            cur[curDi].forEachSetBit([&](int curCell) {
                int curIdx = ToCellDirIdx(curCell, curDi);
                int nextDist = turn + 1;
                int curStep = steps[curIdx];

                int forwardCell = gs.dirs(curCell)[curDi];
                if (forwardCell >= 0) {
                    int nextIdx = ToCellDirIdx(forwardCell, curDi);
                    any |= PushPathState(result, next, curIdx, nextIdx, nextDist, curStep + 1, 'F');
                }

                {
                    int nextIdx = ToCellDirIdx(curCell, (curDi + 3) & 3);
                    any |= PushPathState(result, next, curIdx, nextIdx, nextDist, curStep + 1, 'L');
                }

                {
                    int nextIdx = ToCellDirIdx(curCell, (curDi + 1) & 3);
                    any |= PushPathState(result, next, curIdx, nextIdx, nextDist, curStep + 1, 'R');
                }

                {
                    int nextIdx = macroCache.CalcAppliedIdx(curIdx);
                    any |= PushPathState(result, next, curIdx, nextIdx, nextDist, curStep + macroStep, 'P');
                }
            });
        }

        if (!any) {
            return;
        }

        cur = next;
        ++turn;
    }
}

namespace greedy2b {

using Greedy2BEndDirs = CapArr<u8, M_max * 2>;

bool FindPathStepOnly(int startIdx, int targetIdx, const MacroCache& macroCache, int& targetTurn, int& targetStep) {
    static array<CapArr<int, NN_max * 4>, 2> ques;
    static CheckMapS<NN_max * 4> pushed;
    static CapArr<int, NN_max * 4> dists;
    static CapArr<int, NN_max * 4> steps;

    targetTurn = INT_MAX;
    targetStep = INT_MAX;

    dists.assign(NN * 4, INT_MAX);
    steps.assign(NN * 4, INT_MAX);
    ques[0].clear();
    ques[1].clear();
    pushed.Clear();

    dists[startIdx] = 0;
    steps[startIdx] = 0;
    if (pushed.Enter(startIdx)) {
        ques[0].push(startIdx);
    }

    int macroStep = macroCache.macro_.size();
    auto Push = [&](CapArr<int, NN_max * 4>& nextQue, CheckMapS<NN_max * 4>& nextPushed, int nextIdx,
                    int nextTurn, int nextStep) {
        if (nextTurn < dists[nextIdx] || (nextTurn == dists[nextIdx] && nextStep < steps[nextIdx])) {
            dists[nextIdx] = nextTurn;
            steps[nextIdx] = nextStep;
            if (nextPushed.Enter(nextIdx)) {
                nextQue.push(nextIdx);
            }
        }
    };

    int curQueIdx = 0;
    while (!ques[curQueIdx].empty()) {
        auto& curQue = ques[curQueIdx];
        auto& nextQue = ques[curQueIdx ^ 1];
        nextQue.clear();
        pushed.Clear();

        for (int curIdx : curQue) {
            if (curIdx == targetIdx) {
                targetTurn = dists[curIdx];
                targetStep = steps[curIdx];
                return true;
            }

            int curCell;
            int curDi;
            FromCellDirIdx(curIdx, curCell, curDi);

            int nextTurn = dists[curIdx] + 1;
            int nextStep = steps[curIdx] + 1;

            int forwardCell = gs.dirs(curCell)[curDi];
            if (forwardCell >= 0) {
                Push(nextQue, pushed, ToCellDirIdx(forwardCell, curDi), nextTurn, nextStep);
            }
            Push(nextQue, pushed, ToCellDirIdx(curCell, (curDi + 3) & 3), nextTurn, nextStep);
            Push(nextQue, pushed, ToCellDirIdx(curCell, (curDi + 1) & 3), nextTurn, nextStep);
            Push(nextQue, pushed, macroCache.CalcAppliedIdx(curIdx), nextTurn, steps[curIdx] + macroStep);
        }

        curQueIdx ^= 1;
    }

    return false;
}

bool CalcTotalTurnWithOrder(const MacroCache& macroCache, int limitTurn, int& totalTurn, const MArr<u8>& order,
                            Greedy2BEndDirs* endDirs = nullptr) {
    int curCell = 0;
    int curDi = 2;
    totalTurn = 1 + macroCache.macro_.size();  

    static dvec<int> startIdxs;
    static dvec<int> endIdxs;

    if (endDirs) {
        endDirs->clear();
    }

    REP(loop, M) {
        {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            {
                int mi = order[loop];
                int cell = initialBallCells_[mi];
                REP(di, 4) {
                    endIdxs.push(ToCellDirIdx(cell, di));
                }
            }

            int bestIdx;
            int reachTurn = BitCalcTurn(startIdxs, endIdxs, macroCache, bestIdx);

            VASSERT(bestIdx >= 0);

            totalTurn += reachTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
            if (endDirs) {
                endDirs->push((u8)curDi);
            }
        }

        int ball = initialBallGrid_[curCell];
        ++totalTurn;
        if (totalTurn >= limitTurn) {
            return false;
        }

        {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            int goalCell = ballToGoalCell_[ball];
            REP(di, 4) {
                endIdxs.push(ToCellDirIdx(goalCell, di));
            }

            int bestIdx;
            int reachTurn = BitCalcTurn(startIdxs, endIdxs, macroCache, bestIdx);

            totalTurn += reachTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
            if (endDirs) {
                endDirs->push((u8)curDi);
            }
        }

        ++totalTurn;
        if (totalTurn >= limitTurn) {
            return false;
        }
    }

    return true;
}

bool CalcTotalTurnWithOrder(const string& macro, int limitTurn, int& totalTurn, const MArr<u8>& order,
                            Greedy2BEndDirs* endDirs = nullptr) {
    MacroCache macroCache;
    macroCache.Init(macro);
    return CalcTotalTurnWithOrder(macroCache, limitTurn, totalTurn, order, endDirs);
}

bool CalcTotalTurn(const MacroCache& macroCache, const EasyState& es, int limitTurn, int& totalTurn,
                   MArr<u8>* order = nullptr, Greedy2BEndDirs* endDirs = nullptr) {
    MArr<bool> ballCompleted = es.completed_;
    int curCell = es.cell_;
    int curDi = es.di_;
    int curBall = es.ball_;
    totalTurn = es.turns_ + 1 + macroCache.macro_.size();  

    static dvec<int> startIdxs;
    static dvec<int> endIdxs;

    if (order) {
        order->clear();
    }
    if (endDirs) {
        endDirs->clear();
    }

    int completeCount = 0;
    REP(mi, M) {
        if (ballCompleted[mi]) {
            ++completeCount;
        }
    }

    while (completeCount < M) {
        if (curBall < 0) {

            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            REP(mi, M) {
                if (!ballCompleted[mi]) {
                    int cell = initialBallCells_[mi];
                    REP(di, 4) {
                        endIdxs.push(ToCellDirIdx(cell, di));
                    }
                }
            }

            int bestIdx;
            int reachTurn = BitCalcTurn(startIdxs, endIdxs, macroCache, bestIdx);

            VASSERT(bestIdx >= 0);

            totalTurn += reachTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
            if (endDirs) {
                endDirs->push((u8)curDi);
            }

            curBall = initialBallGrid_[curCell];

            if (order) {
                order->push(curBall);
            }

            ++totalTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }
        }
        else {

            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            int goalCell = ballToGoalCell_[curBall];
            REP(di, 4) {
                endIdxs.push(ToCellDirIdx(goalCell, di));
            }

            int bestIdx;
            int reachTurn = BitCalcTurn(startIdxs, endIdxs, macroCache, bestIdx);

            totalTurn += reachTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
            if (endDirs) {
                endDirs->push((u8)curDi);
            }

            ballCompleted[curBall] = true;
            curBall = -1;
            ++completeCount;

            ++totalTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }
        }
    }

    return true;
}

bool CalcTotalTurn(const MacroCache& macroCache, int limitTurn, int& totalTurn, MArr<u8>* order = nullptr,
                   Greedy2BEndDirs* endDirs = nullptr) {
    EasyState es;
    es.Init();
    return CalcTotalTurn(macroCache, es, limitTurn, totalTurn, order, endDirs);
}

bool CalcTotalTurn(const string& macro, int limitTurn, int& totalTurn, MArr<u8>* order = nullptr,
                   Greedy2BEndDirs* endDirs = nullptr) {
    MacroCache macroCache;
    macroCache.Init(macro);
    return CalcTotalTurn(macroCache, limitTurn, totalTurn, order, endDirs);
}

bool CalcTotalTurnHard(const MacroCache& macroCache, int limitTurn, int& totalTurn, MArr<u8>* order = nullptr) {
    MArr<bool> ballCompleted(M, false);
    int curCell = 0;
    int curDi = 2;
    totalTurn = 1 + macroCache.macro_.size();  

    if (order) {
        order->clear();
    }

    static dvec<int> startIdxs;
    static dvec<int> endIdxs;
    static CapArr<int, NN_max * 4> turnByIdx;

    static MArr<MArr<double>> goalToBallMat;
    {
        goalToBallMat.resize(M);
        REP(mi, M) {
            goalToBallMat[mi].resize(M);
        }

        REP(from, M) {
            startIdxs.clear();
            int goalCell = ballToGoalCell_[from];
            REP(di, 4) {
                startIdxs.push(ToCellDirIdx(goalCell, di));
            }

            endIdxs.clear();
            REP(to, M) {
                if (from == to) {
                    continue;
                }
                int ballCell = initialBallCells_[to];
                REP(di, 4) {
                    endIdxs.push(ToCellDirIdx(ballCell, di));
                }
            }

            BitCalcAllTurns(startIdxs, endIdxs, macroCache, turnByIdx);

            REP(to, M) {
                if (from == to) {
                    continue;
                }
                int ballCell = initialBallCells_[to];
                double total = 0;
                REP(di, 4) {
                    int idx = ToCellDirIdx(ballCell, di);
                    total += turnByIdx[idx];
                }

                goalToBallMat[from][to] = total / 4.0;
            }
        }
    }

    REP(loop, M) {
        {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            REP(mi, M) {
                if (!ballCompleted[mi]) {
                    int cell = initialBallCells_[mi];
                    REP(di, 4) {
                        endIdxs.push(ToCellDirIdx(cell, di));
                    }
                }
            }

            BitCalcAllTurns(startIdxs, endIdxs, macroCache, turnByIdx);

            int bestMi = -1;
            int bestIdx = -1;
            int bestTurn = INT_MAX;
            double bestScore = -1e100;
            REP(mi, M) {
                if (!ballCompleted[mi]) {
                    int ballCell = initialBallCells_[mi];
                    int getTurn = INT_MAX;
                    int getIdx = -1;
                    REP(di, 4) {
                        int idx = ToCellDirIdx(ballCell, di);
                        int turn = turnByIdx[idx];
                        if (turn < getTurn) {
                            getTurn = turn;
                            getIdx = idx;
                        }
                    }

                    double nextBallTurn = 1e100;
                    if (loop + 1 == M) {
                        nextBallTurn = 0;
                    }
                    else {
                        REP(nextMi, M) {
                            if (nextMi == mi) {
                                continue;
                            }
                            if (ballCompleted[nextMi]) {
                                continue;
                            }
                            chmin(nextBallTurn, goalToBallMat[mi][nextMi]);
                        }
                    }

                    double score = -getTurn - nextBallTurn;
                    if (score > bestScore) {
                        bestScore = score;
                        bestMi = mi;
                        bestIdx = getIdx;
                        bestTurn = getTurn;
                    }
                }
            }

            VASSERT(bestIdx >= 0);

            totalTurn += bestTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
        }

        int ball = initialBallGrid_[curCell];
        ++totalTurn;
        if (totalTurn >= limitTurn) {
            return false;
        }

        {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            int goalCell = ballToGoalCell_[ball];
            REP(di, 4) {
                endIdxs.push(ToCellDirIdx(goalCell, di));
            }

            int bestIdx;
            int reachTurn = BitCalcTurn(startIdxs, endIdxs, macroCache, bestIdx);

            totalTurn += reachTurn;
            if (totalTurn >= limitTurn) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
        }

        ++totalTurn;
        if (totalTurn >= limitTurn) {
            return false;
        }

        ballCompleted[ball] = true;
        if (order) {
            order->push(ball);
        }
    }

    return true;
}

bool CalcTotalTurnHard(const string& macro, int limitTurn, int& totalTurn, MArr<u8>* order = nullptr) {
    MacroCache macroCache;
    macroCache.Init(macro);
    return CalcTotalTurnHard(macroCache, limitTurn, totalTurn, order);
}

bool SolveWithOrder(const MacroCache& macroCache, int limitTurn, const MArr<u8>& order, int& totalTurn, int& totalStep,
                    dvec<char>* result) {
    int curCell = 0;
    int curDi = 2;
    static dvec<char> tmpResult;
    if (result) {
        tmpResult.clear();
    }
    totalTurn = 1 + macroCache.macro_.size();  
    totalStep = 0;

    static FindPathResult pathResult;
    static dvec<char> commands;
    static dvec<int> startIdxs;
    static dvec<int> endIdxs;

    for (int targetMi : order) {
        {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            {
                endIdxs.clear();
                int cell = initialBallCells_[targetMi];
                REP(di, 4) {
                    endIdxs.push(ToCellDirIdx(cell, di));
                }
            }

            BitFindPath(startIdxs, endIdxs, macroCache, pathResult);

            int bestIdx = -1;
            double bestScore = -1e100;
            for (int idx : endIdxs) {
                int dist = pathResult.GetDist(idx);
                if (dist == INT_MAX) {
                    continue;
                }
                int step = pathResult.GetStep(idx);
                double score = -dist * 10000 - step;
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = idx;
                    break;
                }
            }

            VASSERT(bestIdx >= 0);

            if (result) {
                pathResult.BuildCommand(bestIdx, commands);
                tmpResult.extend(commands);
            }
            totalTurn += pathResult.GetDist(bestIdx);
            totalStep += pathResult.GetStep(bestIdx);
            if (totalTurn >= limitTurn || totalStep > T) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
        }

        int ball = initialBallGrid_[curCell];
        if (result) {
            tmpResult.push('S');
        }
        ++totalTurn;
        ++totalStep;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            endIdxs.clear();
            int goalCell = ballToGoalCell_[ball];
            REP(di, 4) {
                endIdxs.push(ToCellDirIdx(goalCell, di));
            }

            BitFindPath(startIdxs, endIdxs, macroCache, pathResult);

            int bestIdx = -1;
            double bestScore = -1e100;
            for (int idx : endIdxs) {
                int dist = pathResult.GetDist(idx);
                if (dist == INT_MAX) {
                    continue;
                }
                int step = pathResult.GetStep(idx);
                double score = -dist * 10000 - step;
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = idx;
                    break;
                }
            }

            VASSERT(bestIdx >= 0);
            if (result) {
                pathResult.BuildCommand(bestIdx, commands);
                tmpResult.extend(commands);
            }
            totalTurn += pathResult.GetDist(bestIdx);
            totalStep += pathResult.GetStep(bestIdx);
            if (totalTurn >= limitTurn || totalStep > T) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
        }

        if (result) {
            tmpResult.push('S');
        }
        ++totalTurn;
        ++totalStep;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }
    }

    if (result) {
        result->clear();

        bool firstP = true;
        for (char c : tmpResult) {
            if (firstP && c == 'P') {
                result->push('M');
                for (char m : macroCache.macro_) {
                    result->push(m);
                }
                result->push('M');
                firstP = false;
            }
            else {
                result->push(c);
            }
        }
    }

    return true;
}

bool SolveWithOrder(const MacroCache& macroCache, int limitTurn, const MArr<u8>& order,
                    const Greedy2BEndDirs& endDirs, int& totalTurn, int& totalStep, dvec<char>* result) {
    if ((int)order.size() != M || (int)endDirs.size() != M * 2) {
        return false;
    }

    int curCell = 0;
    int curDi = 2;
    static dvec<char> tmpResult;
    if (result) {
        tmpResult.clear();
    }
    totalTurn = 1 + macroCache.macro_.size();  
    totalStep = 0;

    static FindPathResult pathResult;
    static dvec<char> commands;
    static dvec<int> startIdxs;
    static dvec<int> endIdxs;

    auto MoveToFixedEnd = [&](int targetCell, int endDir) -> bool {
        int startIdx = ToCellDirIdx(curCell, curDi);
        int targetIdx = ToCellDirIdx(targetCell, endDir);
        int turn;
        int step;

        if (result) {
            startIdxs.clear();
            startIdxs.push(startIdx);
            endIdxs.clear();
            endIdxs.push(targetIdx);

            BitFindPath(startIdxs, endIdxs, macroCache, pathResult);
            turn = pathResult.GetDist(targetIdx);
            if (turn == INT_MAX) {
                return false;
            }
            step = pathResult.GetStep(targetIdx);
            pathResult.BuildCommand(targetIdx, commands);
            tmpResult.extend(commands);
        }
        else {
            if (!FindPathStepOnly(startIdx, targetIdx, macroCache, turn, step)) {
                return false;
            }
        }

        totalTurn += turn;
        totalStep += step;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        curCell = targetCell;
        curDi = endDir;
        return true;
    };

    REP(i, order.size()) {
        int targetMi = order[i];
        if (!MoveToFixedEnd(initialBallCells_[targetMi], endDirs[i * 2])) {
            return false;
        }

        int ball = initialBallGrid_[curCell];
        if (ball != targetMi) {
            return false;
        }
        if (result) {
            tmpResult.push('S');
        }
        ++totalTurn;
        ++totalStep;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        if (!MoveToFixedEnd(ballToGoalCell_[ball], endDirs[i * 2 + 1])) {
            return false;
        }

        if (result) {
            tmpResult.push('S');
        }
        ++totalTurn;
        ++totalStep;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }
    }

    if (result) {
        result->clear();

        bool firstP = true;
        for (char c : tmpResult) {
            if (firstP && c == 'P') {
                result->push('M');
                for (char m : macroCache.macro_) {
                    result->push(m);
                }
                result->push('M');
                firstP = false;
            }
            else {
                result->push(c);
            }
        }
    }

    return true;
}

bool SolveWithOrder(const string& macro, int limitTurn, const MArr<u8>& order, int& totalTurn, int& totalStep,
                    dvec<char>* result) {
    MacroCache macroCache;
    macroCache.Init(macro);
    return SolveWithOrder(macroCache, limitTurn, order, totalTurn, totalStep, result);
}

bool SolveWithOrder(const string& macro, int limitTurn, const MArr<u8>& order, const Greedy2BEndDirs& endDirs,
                    int& totalTurn, int& totalStep, dvec<char>* result) {
    MacroCache macroCache;
    macroCache.Init(macro);
    return SolveWithOrder(macroCache, limitTurn, order, endDirs, totalTurn, totalStep, result);
}

bool Solve(const MacroCache& macroCache, int limitTurn, dvec<char>* result, MArr<u8>* order = nullptr) {
    MArr<bool> ballCompleted(M, false);
    int curCell = 0;
    int curDi = 2;
    static dvec<char> tmpResult;
    if (result) {
        tmpResult.clear();
    }
    int totalTurn = 1 + macroCache.macro_.size();  
    int totalStep = 0;

    static FindPathResult pathResult;
    static dvec<char> commands;
    if (order) {
        order->clear();
    }

    REP(loop, M) {
        {
            dvec<int> startIdxs;
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            dvec<int> endIdxs;
            REP(mi, M) {
                if (!ballCompleted[mi]) {
                    int cell = initialBallCells_[mi];
                    REP(di, 4) {
                        endIdxs.push(ToCellDirIdx(cell, di));
                    }
                }
            }

            BitFindPath(startIdxs, endIdxs, macroCache, pathResult);

            int bestIdx = -1;
            double bestScore = -1e100;
            for (int idx : endIdxs) {
                int dist = pathResult.GetDist(idx);
                if (dist == INT_MAX) {
                    continue;
                }
                int step = pathResult.GetStep(idx);
                double score = -dist * 10000 - step;
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = idx;
                    break;
                }
            }

            VASSERT(bestIdx >= 0);

            if (result) {
                pathResult.BuildCommand(bestIdx, commands);
                tmpResult.extend(commands);
            }
            totalTurn += pathResult.GetDist(bestIdx);
            totalStep += pathResult.GetStep(bestIdx);
            if (totalTurn >= limitTurn || totalStep > T) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
        }

        int ball = initialBallGrid_[curCell];
        if (result) {
            tmpResult.push('S');
        }
        ++totalTurn;
        ++totalStep;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        {
            dvec<int> startIdxs;
            startIdxs.push(ToCellDirIdx(curCell, curDi));

            dvec<int> endIdxs;
            int goalCell = ballToGoalCell_[ball];
            REP(di, 4) {
                endIdxs.push(ToCellDirIdx(goalCell, di));
            }

            BitFindPath(startIdxs, endIdxs, macroCache, pathResult);

            int bestIdx = -1;
            double bestScore = -1e100;
            for (int idx : endIdxs) {
                int dist = pathResult.GetDist(idx);
                if (dist == INT_MAX) {
                    continue;
                }
                int step = pathResult.GetStep(idx);
                double score = -dist * 10000 - step;
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = idx;
                    break;
                }
            }

            VASSERT(bestIdx >= 0);
            if (result) {
                pathResult.BuildCommand(bestIdx, commands);
                tmpResult.extend(commands);
            }
            totalTurn += pathResult.GetDist(bestIdx);
            totalStep += pathResult.GetStep(bestIdx);
            if (totalTurn >= limitTurn || totalStep > T) {
                return false;
            }

            FromCellDirIdx(bestIdx, curCell, curDi);
        }

        if (result) {
            tmpResult.push('S');
        }
        ++totalTurn;
        ++totalStep;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        ballCompleted[ball] = true;

        if (order) {
            order->push(ball);
        }
    }

    if (result) {
        result->clear();

        bool firstP = true;
        for (char c : tmpResult) {
            if (firstP && c == 'P') {
                result->push('M');
                for (char m : macroCache.macro_) {
                    result->push(m);
                }
                result->push('M');
                firstP = false;
            }
            else {
                result->push(c);
            }
        }
    }

    return true;
}

bool Solve(const string& macro, int limitTurn, Result& result) {
    MacroCache macroCache;
    macroCache.Init(macro);
    return Solve(macroCache, limitTurn, &result.commands_);
}

void Solve(Result& result, string& resultMacro) {
    vector<string> macros;
    macros.reserve(16384);

    auto AddMacro = [&](const string& macro) {
        if (find(macros.begin(), macros.end(), macro) == macros.end()) {
            macros.push_back(macro);
        }
    };

    {
        array<string, 3> curves = {"L", "R", "LL"};

        auto dfs = [&](auto&& dfs, string& macro, int depth) -> void {
            if (macro.size() > 40 || depth >= 3) {
                return;
            }

            int firstSize = macro.size();
            FOR(fc, 1, 9) {
                macro.push_back('F');
                AddMacro(macro);

                for (cauto& curve : curves) {
                    macro += curve;
                    AddMacro(macro);

                    dfs(dfs, macro, depth + 1);

                    macro.resize(macro.size() - curve.size());
                }
            }
            macro.resize(firstSize);
        };
        string macro;
        dfs(dfs, macro, 0);
    }

    int bestTurn = INT_MAX;

    for (const string& macro : macros) {
        int turn;
        if (CalcTotalTurnHard(macro, bestTurn, turn)) {
            if (turn < bestTurn) {
                bestTurn = turn;
                resultMacro = macro;
            }
        }
    }

    bool ret = Solve(resultMacro, INT_MAX, result);

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


enum class SAResult : u8 {
    Ok,    
    Ng,    
    Fail,  
};

class SAConfig {
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

    bool CheckAccept(double new_eval_score, Rng& rng) {
        if (new_eval_score >= eval_score) {
            return true;
        }
        else {
            if (new_eval_score >= GetAcceptScore(rng)) {
                return true;
            }
        }
        return false;
    }

    double GetAcceptScore(Rng& rng) {
        if (!accept_score) {
            if (!accept_value) {
                double rand_value = rng.GetDouble();
                accept_value = temp * log(rand_value);
            }
            accept_score = eval_score + accept_value.value();
        }
        return accept_score.value();
    }
};
template <class State>
struct EasySA {
   private:
    struct StateData {
        State bestState_;  
        f64 eval_score;    

        i32 no_best_try_count;   
        i32 rollback_try_count;  

        void Init(const State& state, int rollback_count) {
            bestState_ = state;
            eval_score = state.evalScore_;
            no_best_try_count = 0;
            rollback_try_count = rollback_count;
        }
    };

   private:
    dvec<StateData> datas_;

   public:
    int updateTmpMask_ = 127;

    int8_t tempType_ = 0;
    double tempPow_ = 2.0;

    double rollbackStartTimeRate_ = 1.0;  
    int rollbackCount_ = INT_MAX;         
    double rollbackMul_ = 1.0;            
    int minRollbackCount_ = 1;            

    double filterStartTime_ = 0.2;  
    double filterEndTime_ = 1.0;    

   public:
    void Run(ChronoTimer& timer, dvec<State>& states, f64 start_temp, f64 end_temp, auto&& trans) {
        datas_.clear();
        for (cauto& state : states) {
            datas_.pushR().Init(state, rollbackCount_);
        }
        dvec<int> sis(states.size());
        iota(ALL(sis), 0);


        auto startTime = timer.Now();
        auto endTime = timer.EndTime();
        double timeRate = 0.0;
        double temp = 0.0;
        int loop_count = 0;
        int enable_count = 0;
        double div_time = 1.0 / duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        SAChecker checker;
        while (true) {
            {
                if ((loop_count & updateTmpMask_) == 0) {
                    auto now = timer.Now();
                    if (now >= endTime) {
                        break;
                    }

                    timeRate = duration_cast<std::chrono::microseconds>(now - startTime).count() * div_time;
                    temp = GetTemp(start_temp, end_temp, timeRate, tempType_, tempPow_);
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
                SAResult result = trans(state, checker, timeRate, ti);
                ++data.no_best_try_count;

                if (result == SAResult::Ok) {
                    double eval_score = state.evalScore_;
                    data.eval_score = eval_score;

                    auto raw_score = state.rawScore_;
                    if (raw_score > data.bestState_.rawScore_) {
                        data.bestState_ = state;
                        data.no_best_try_count = 0;
                    }
                }
                if (timeRate >= rollbackStartTimeRate_) {
                    if (data.no_best_try_count >= data.rollback_try_count) {
                        state = data.bestState_;
                        data.eval_score = state.evalScore_;
                        data.no_best_try_count = 0;

                        data.rollback_try_count = max(int(data.rollback_try_count * rollbackMul_), minRollbackCount_);

                    }
                }
            }

            if (sis.size() > 1) {
                int target_count = states.size();
                if (timeRate >= filterEndTime_) {
                    target_count = 1;
                }
                else if (timeRate >= filterStartTime_) {
                    double r = 1.0 - (timeRate - filterStartTime_) /
                                         (filterEndTime_ - filterStartTime_);  
                    target_count = 1 + (int)floor(states.size() * r);
                }
                if (sis.size() > target_count) {
                    sort(ALL(sis),
                         [&](int a, int b) { return datas_[a].bestState_.rawScore_ > datas_[b].bestState_.rawScore_; });
                    sis.resizeR(target_count);

                }
            }

        }


    }

    double GetTemp(double startTemp, double endTemp, double timeRate, int tempType, double tempPow) const {
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

    const State& GetBest() const {
        int best = 0;
        FOR(i, 1, datas_.size()) {
            if (datas_[i].bestState_.rawScore_ > datas_[best].bestState_.rawScore_) {
                best = i;
            }
        }
        return datas_[best].bestState_;
    }

};

struct RunMacroPart {
    u8 f_;
    char turn_;  

    int Len() const {
        return f_ + (turn_ != 0 ? 1 : 0);
    }
    bool HasF() const {
        return f_ > 0;
    }
    bool HasTurn() const {
        return turn_ != 0;
    }
    bool IsLR() const {
        return turn_ == 'L' || turn_ == 'R';
    }
};

template <int CAP>
struct RunMacro {
    static_assert(CAP <= 255);
    static constexpr int RunCapacity = CAP + 1;

    CapArr<RunMacroPart, RunCapacity> runs_;
    CapArr<u8, CAP + 2> starts_;        
    CapArr<u8, RunCapacity> fRuns_;     
    CapArr<u8, RunCapacity> turnRuns_;  
    CapArr<u8, RunCapacity> lrRuns_;    
    u8 len_ = 0;
    u8 fCount_ = 0;
    u8 turnCount_ = 0;

    void Clear() {
        runs_.clear();
        starts_.clear();
        starts_.push(0);
        fRuns_.clear();
        turnRuns_.clear();
        lrRuns_.clear();
        len_ = 0;
        fCount_ = 0;
        turnCount_ = 0;
    }

    template <class MACRO>
    bool BuildFromImpl(const MACRO& baseMacro) {
        Clear();
        int n = (int)baseMacro.size();
        if (n > CAP) {
            return false;
        }

        int i = 0;
        while (i < n) {
            int f = 0;
            while (i < n && baseMacro[i] == 'F') {
                ++f;
                ++i;
            }

            char turn = 0;
            if (i < n) {
                turn = baseMacro[i];
                ++i;
            }

            auto& run = runs_.push();
            run.f_ = (u8)f;
            run.turn_ = turn;
        }

        RebuildIndexes();
        return true;
    }

    bool BuildFrom(const string& baseMacro) {
        return BuildFromImpl(baseMacro);
    }

    template <int MCAP>
    bool BuildFrom(const CapArr<char, MCAP>& baseMacro) {
        return BuildFromImpl(baseMacro);
    }

    void BuildString(string& dst) const {
        dst.clear();
        dst.reserve(len_);
        for (cauto& run : runs_) {
            dst.append(run.f_, 'F');
            if (run.turn_ != 0) {
                dst.push_back(run.turn_);
            }
        }
    }

    string ToString() const {
        string result;
        BuildString(result);
        return result;
    }

    int size() const {
        return len_;
    }
    bool empty() const {
        return len_ == 0;
    }
    int RunCount() const {
        return runs_.size();
    }
    bool HasF() const {
        return !fRuns_.empty();
    }
    bool HasTurn() const {
        return !turnRuns_.empty();
    }
    bool HasLR() const {
        return !lrRuns_.empty();
    }

    int RunStart(int runIdx) const {
        VASSERT(0 <= runIdx && runIdx < runs_.size());
        return starts_[runIdx];
    }
    int RunEnd(int runIdx) const {
        VASSERT(0 <= runIdx && runIdx < runs_.size());
        return starts_[runIdx + 1];
    }
    int FStart(int runIdx) const {
        return RunStart(runIdx);
    }
    int FEnd(int runIdx) const {
        VASSERT(0 <= runIdx && runIdx < runs_.size());
        return starts_[runIdx] + runs_[runIdx].f_;
    }
    int TurnPos(int runIdx) const {
        VASSERT(0 <= runIdx && runIdx < runs_.size());
        if (runs_[runIdx].turn_ == 0) {
            return -1;
        }
        return starts_[runIdx] + runs_[runIdx].f_;
    }

    int FindRunByPos(int pos) const {
        VASSERT(0 <= pos && pos < len_);
        REP(i, runs_.size()) {
            if (pos < starts_[i + 1]) {
                return i;
            }
        }
        VABORT();
        return -1;
    }

    char At(int pos) const {
        int runIdx = FindRunByPos(pos);
        cauto& run = runs_[runIdx];
        int offset = pos - starts_[runIdx];
        if (offset < run.f_) {
            return 'F';
        }
        return run.turn_;
    }

    bool IsValid() const {
        if (starts_.size() != runs_.size() + 1) {
            return false;
        }

        int pos = 0;
        int fCount = 0;
        int turnCount = 0;
        REP(i, runs_.size()) {
            if (starts_[i] != pos) {
                return false;
            }
            cauto& run = runs_[i];
            if (run.turn_ == 'F') {
                return false;
            }
            pos += run.Len();
            fCount += run.f_;
            if (run.turn_ != 0) {
                ++turnCount;
            }
        }

        return starts_.back() == pos && len_ == pos && fCount_ == fCount && turnCount_ == turnCount;
    }

    void RebuildIndexes() {
        starts_.clear();
        fRuns_.clear();
        turnRuns_.clear();
        lrRuns_.clear();

        int pos = 0;
        int fCount = 0;
        int turnCount = 0;
        REP(i, runs_.size()) {
            starts_.push((u8)pos);
            cauto& run = runs_[i];
            if (run.f_ > 0) {
                fRuns_.push((u8)i);
            }
            if (run.turn_ != 0) {
                turnRuns_.push((u8)i);
                ++turnCount;
                if (run.IsLR()) {
                    lrRuns_.push((u8)i);
                }
            }
            pos += run.Len();
            fCount += run.f_;
        }
        starts_.push((u8)pos);

        len_ = (u8)pos;
        fCount_ = (u8)fCount;
        turnCount_ = (u8)turnCount;
    }
};

vector<string> MacroTable = {
    "FFFF",
    "",

    "FFFFRF",
    "FFLFFFF",
    "FFFFFFLF",
    "FFFFFFRF",
    "FFFFFLFL",
    "FLFFFRFR",
    "FFFFLFFFL",
    "FFFFLFFLL",
    "FFLFFFFLL",
    "FFLFFFRFR",
    "FRFFFFFLL",
    "LFFFFFFLL",
    "LFFFRFFFR",
    "FFFFFFFLFF",
    "FFFFFFFRFF",
    "FFFFFFRFFF",
    "FFFFLFFFFF",
    "FFFFLFFFFL",
    "FFFFRFFFLL",
    "FFFRFFFFFR",
    "FFFRFFFFLL",
    "FFLFFFFFLL",
    "FFLFFRFFFL",
    "FLFFFFFFLL",
    "LFFFFRFFFL",
    "RFFFFFFRFR",
    "FFFFFFFFFRF",
    "FFFFFFFRFFR",
    "FFFFFFLFFLL",
    "FFFFFFRFFRR",
    "FFFFFLFFFLL",
    "FFFFFLFFFRR",
    "FFFFFLFFLFR",
    "FFFFFRFFFLL",
    "FFFFFRFFLFL",
    "FFFFLFFFFFF",
    "FFFFLFFFFRR",
    "FFFFLFFFRFR",
    "FFFRFFFFFLL",
    "FFFRFFFFFRR",
    "FFLFFFFFFRR",
    "FFRFFFFFFFF",
    "FFRFFFFFFRR",
    "FLFFFFFFFFL",
    "FRFFFFFFFLL",
    "FRFFFLFFFRF",
    "FRFLFFFFFFL",
    "LFLFFFFRFFF",
    "RFFLFFRFFFF",
    "FFFFFFLFFRFR",
    "FFFFFRFFFLFL",
    "FFFFLFFFRFFR",
    "FFFFRFFFFRRF",
    "FFFFRFFFLFLF",
    "FFFRFFFFFFLL",
    "FFLFFFFFFFFL",
    "FFLFFFFFFFRR",
    "FRFFFFFFFLFL",
    "RFFFFFFFRFRF",
    "RFFFFRFFFFFR",
    "FFFFFFFLFFFRR",
    "FFFFLFFFFFFLL",
    "FFFFRFFFLFFFL",
    "FFFLFFFFFFFFL",
    "FFFLFFFFFFRFR",
    "FFFLFFFFFRFRF",
    "FFRFFFFLFFFFL",
    "FRFRFFFLFFFRR",
    "LFFFFFFFLFFFL",
    "LFFFFRFFLFFLF",
    "RFFFFFFFFFFFR",
    "FFFFFFFFFFFFFR",
    "FFFFFFFFFLFFRR",
    "FFFFFFFRFFFLFL",
    "FFFFFFLFFFRFFR",
    "FFFFFLFFFFFRFR",
    "FFFFFLFFFRFFRF",
    "FFFFFRFFFFFFRR",
    "FFFFFRFFFFFRRF",
    "FFFFLFFFFFFFLF",
    "FFFFLFFFFFFFLL",
    "FFFFLFFFFFRFRF",
    "FFFFRFFFFFFLFL",
    "FFFFRFFFFFLFFL",
    "FFFLFFFFFFFFLL",
    "FFFLFFFFFFFFRR",
    "FLFFFFFFFFRFFR",
    "FLFFFFFFLFFFFL",
    "LFFFLFFFFFFFFL",
    "RRFFLFFFFFRFRF",
    "FFFFFFFFFFRFFLL",
    "FFFFFFFFLFFFRFR",
    "FFFFFFFLFFFFLLF",
    "FFFFFFFLFFFFRRF",
    "FFFFFFFRFFFFLLF",
    "FFFFFFLFFFFFRFR",
    "FFFFFFRFFFFFFLL",
    "FFFFFFRFFFFFRRF",
    "FFFFFLFFFFFFFLL",
    "FFFFFRFFFFLFRRR",
    "FFFFFRLFFFFRFRF",
    "FFFFRFFFFFFFLFL",
    "FFFFRFFFFLFFLFL",
    "FFFLFFFFFFFFFRR",
    "FFFRFFFFLFFFLFL",
    "FFFRFFFFLFFRFRR",
    "FFRFFFFFFFFFLFL",
    "FFRFRFRFFFFFFLF",
    "FLFFFFFFFFFRFFR",
    "FLFFFFFRFFFFLFL",
    "FLFFRFLFFFFRFFR",
    "FRFFFFFFFFLFFLL",
    "FRFFFFFFLFFFFLL",
    "FRFFFFFFLFFFRFR",
    "LFFFFLFFFFFRFLL",
    "LFRFFFFLFFFFFLL",
    "RFFFFFFFLFFFFRF",
    "RFRFFFFFFFLFFLL",
    "RRFFFFFRFFFLFLF",
    "FFFFFFFFFFRFFFLL",
    "FFFFFFFFFLFFFRRF",
    "FFFFFFFFRFFFFLFL",
    "FFFFFFFLFFFFFRFR",
    "FFFFFFFRFFFFFLFL",
    "FFFFFFRFFFFFFLFL",
    "FFFFLFFFFRFFFRFR",
    "FFFFRFFFFFFFFFLL",
    "FFFRFFFFLFLFRFFR",
    "FFLFFFFFFRFFFLFL",
    "FFLFFFFFFRFRFFRR",
    "FFLFFFFFLFFFFFFL",
    "FFLFFFFRFFFFLFFL",
    "FFRFFFFLFFFFFRFR",
    "FFRFFFLFFFRFFFFR",
    "FLFFFFFLFFRFFFLL",
    "FRFFFRFFFFFLFLFL",
    "LFFLFFFRFFFFLFFL",
    "LLFFLFFFFRFFFFFR",
    "RFFFFRFFLFFFFFLL",
    "RFFFLFFFFFFRFFFF",
    "RFFFRFFFFFFLFFLL",
    "RFLFFFFFFRFFLFRR",
    "RRFLFFFFFFFFRFFR",
    "FFFFFFFFFRFFFFLFL",
    "FFFFFFFRFFFFFFLFL",
    "FFFFFFLFFFFFFFRFR",
    "FFFFFFRFFFFFFFFLL",
    "FFFFFLFFFFFFFFRFR",
    "FFFFFRFFFFFFFFFRF",
    "FFFFFRFFFFFFFFFRR",
    "FFFFFRFFFFFFFFLFL",
    "FFFFFRFFFFFFFLFLF",
    "FFFFRFFFFFFFFFLFL",
    "FFFLFFFFFRFFLFFRR",
    "FFFLFRFFFFFLFFFLL",
    "FFFRFFFFFFFFFFRRF",
    "FFRFLFLFFFRFFFRFF",
    "FLFFFFFFRFFFFRRFF",
    "FLFFFFRFFLFFFFFRR",
    "FRFFFFFFFFLFFFRFR",
    "FRFFFLFFRFFFFFFRR",
    "LFFFFLFFFFFRFFLLF",
    "LFFFRFFFFFFLFFFRR",
    "LLFFFLFFFFFRFFFFR",
    "LLFFRFFFFFFFLFFLF",
    "LLFFRFFFFLFFFFFFL",
    "RFFFFFLFFFFFFRFRF",
    "RFFFFFLFFFRFFRFFR",
    "RFFFLFFFFFFFFFRFR",
    "RRFFFFFFFFFFLFFLL",
    "RRFFFFFLFFFFFRFRF",
    "RRFFFLFFFFFFFRFFR",
    "RRFFLFFFFFFFRFFFR",
    "FFFFFFFFFFFRFFFFFF",
    "FFFFFFFFFFLFFFFFLL",
    "FFFFFFFFFRFFFFLFLF",
    "FFFFFFFFLFFFFFFRFR",
    "FFFFFFFFLFFFRFFFRF",
    "FFFFFFFRFFFFFFFFRR",
    "FFFFFFLFFFFFFFFLFL",
    "FFFFFFLFFFFFFFFRFR",
    "FFFFFFLFFFFFFFRFFR",
    "FFFFFFRFFFFFFFLFLF",
    "FFFFFFRFFFFFFLFFLF",
    "FFFFFFRFFFFFLFFFLL",
    "FFFFFRFFFFFFFFRFFR",
    "FFFFLFRFRFFFFFFLFL",
    "FFFFRFFFFFLFFFLFFL",
    "FFFFRFFFFLFFFFRFFF",
    "FFFLFFFFFFFFFFRFRF",
    "FFFLFFFFFRFLFFFFLL",
    "FFFRFFFFFFFFFFRFFR",
    "FFFRFFFFLFFRFFFFLL",
    "FFFRFFFLFFFFFFRFRR",
    "FFLFFFFLFFFRFFFLFL",
    "FFLFRFFFLFFFFFFFRR",
    "FFRFFFFLFFFFFLLFFF",
    "FFRFLFRFFFFFLFFFLL",
    "FLFFFFFLFFFRFFFLFL",
    "FRFFFLFFFFFFFRFFRF",
    "FRFFFLFFLFFFFRFFRF",
    "LFFFFFFFLFFFFFRFRR",
    "LFFFFFFFRFFFFLFFFL",
    "LFFFFFFRFFFRFFLFFL",
    "LFFFFFLFFFFFFFRFRR",
    "LFFFFFLFFRFFFFFRRF",
    "LFFFFFRFFFFLFFLFFL",
    "RFFFLFFFFFFFFFRFFR",
    "RFFFLFFFFFRFFFFFRR",
    "FFFFFFFFFRFFFFFLFLF",
    "FFFFFFFFRFFFFFFFLFL",
    "FFFFFFFFRFFFFFFLLFF",
    "FFFFFFLFFFFFFFRFRFR",
    "FFFFLFFFFFFFFFRFFRF",
    "FFFFLFRFFFRFFFFLFFL",
    "FFFFRFFFFFFFFFLFFFL",
    "FFFFRFFFFLFFFFFFRRR",
    "FFFLFFFFFFRFFFFLFFL",
    "FFRFFFFFFFFFLFFFFLF",
    "FFRFFFFFFFRFFFFFFFR",
    "FLFFFFFFFFFLFFFFRFR",
    "FRFFFLFRFFLFFFFRFRF",
    "LFFFFFRFFFFFFLFFFLF",
    "LLFFRFFFFFFFFLFFLFF",
    "RFFFFFFFRFFFLFFFLFL",
    "RRFFFLFFFFRFFFFRFFF",
    "FFFFFFFFRFFFFFFFFLFL",
    "FFFFFFFRFFFFFFFLFLFF",
    "FFFFFFLFFFFFFFFFRFRF",
    "FFFFFRFFFFFFFFFFFFLL",
    "FFFLFFFFFFRFFFFFLFFL",
    "FFFRFFFRFFFFLFFFFFRR",
    "FFLFFFFFFFRFFFFFLFLF",
    "FFLFFFFRFFFFFFFLFFFL",
    "FFLFFFRFFFLFFFFFFRFR",
    "FFRFFFFFLFFFFFFFFRRL",
    "FFRFFFLFFFRFFFFFFFLL",
    "FLFFFFFLFFFFFRFFFLFL",
    "FLFFFFRFFFFLFFFFFFLL",
    "FRFFFLFFFFFRFFFFFFRR",
    "LFFFFFFRFFFFFFLFFLFF",
    "LFFFFFLFFFFFFFFRFRFF",
    "LFFFRFFFFFFFFFFLFFLF",
    "LFFRFFFFLFFFFFFFFFLL",
    "RFFFFFFFFRFFFFFFLFLL",
    "RFFFFLFFRFFFLFFFFRFR",
    "RFRLFFFRFFFFFLFFFFLL",
    "RRFFFFFFLFFFFFFRFFFR",
    "RRFFFFFRFFFFLFFFFLFF",
    "FFFFFFFFLFFFFFFFFRFRR",
    "FFFFFFFRFFFFFFLFFLLFR",
    "FFFFFFFRFFFLFFRFFFLFL",
    "FFFFFFRFFFFFFFFFFFFRR",
    "FFFFFFRFFFFFFFFFFFLFL",
    "FFFLFFFFFFRFFFLFFFFLF",
    "FFRFFFFFFFLFFFFRFFFFR",
    "FRFFFFFFFFFFLFFFLFFLL",
    "FRFFFFFRFFFFLFFFFFLFL",
    "LFFFFRFFFFFFLFFFRFFRR",
    "LFFFRFFFFFFLFFFFFFFLL",
    "LFFRFFFFFLFFFFFFFFLLF",
    "LLFFFFFRFFFFFFLFFFFLF",
    "RFFFFFFLFFFFFRFFFFFFR",
    "RFFLFFFLFFFFFFFFRFFFR",
    "RRFFFFFFRFFFFFFLFFFLF",
    "FFFFFFFFFFFFRFFFFFLFLF",
    "FFFFFFFFFLFFFFFFFFFRFR",
    "FFFFFLFFFFRFFRFFLFFFFL",
    "FFFRFFFFFFFFFLFFFFRFRR",
    "FFLFFFFFFFFFFRFFFFFFRR",
    "FFLFFFFFFFFLFFFFRFFRFR",
    "FFLFFFFRFFFFLFFFFFLFFL",
    "FLFFFFFFRFFFFFLFFFFLFL",
    "FLFFFFFFRFFFFFLFFFLLFR",
    "FRFFFFFFFFFLFFFFFFFRFR",
    "LFFFFFFFFFFFFFFFRFFFRR",
    "LFFFFFRFFFFFFLFFFFFLFF",
    "RFFFFFFFFRFFFFFLFFLFFL",
    "RFRFFFFFLFFFFFFRFFRFFF",
    "RRFFLFFFFFFRFFFFFFFRFR",
    "FFFFFFFFFLFFFFFFLLFFFFR",
    "FFFFFFFFFRFFFFLFRFFFLFL",
    "FFFFFFFFLFFFFFRFFFFFRFR",
    "FFFFFLFFFFFFRFFFFFFRFFR",
    "FFFLFFFFFFFFFRFFLFFFRRF",
    "FFFRFFLFFFFFFFFRFFFFLFL",
    "FFLFFFFFFFFFFFFFFRFFFRF",
    "FFLFFFFFFFFFFRFFFFLLFLF",
    "FFLFFFLFFFFFFFFFRFFFFLL",
    "FFRFFFFFFFLFFFFFFFRFFRF",
    "FLFFFFFFFFFRFFFFFFLFFLF",
    "FLFFFFFRFFFFFLFFFFFLFFL",
    "FRFFFFFFFFFFFFLFFFFFRFR",
    "FRFFFFFFFRFFFFFFLFFFLFL",
    "FRFFFFLFFFFFFFRFFFLFLFL",
    "FRRFFFLFFFFFFFFRFFFFFRF",
    "LFFFFFFFFLFFFFFRFFFFLFL",
    "LFFFFFRFFFFFFFLFFFFLFFL",
    "LFFFFRFFFFLFFFFFFFFFFRR",
    "RFFFFFFFFLFFFFFFFRFFRFF",
    "RFFFFFFFFLFFFFFRFFFFFRR",
    "RFFFFFFLFFFFFFRFFFFRFFR",
    "RFFFRFFFRFFFFFFFLFFFLFF",
    "RFFLFFFFFFFFFFFFFRFFFRF",
    "RFFLFFFFFFFFFFFFRFFFRFF",
    "RFRFFFFFRFFFFFLFFFFFLFL",
    "FFFFFFFFLFFFFFFFFFFRFFRF",
    "FFFFFFFRFFFFFFFFFFFFLFLF",
    "FFFFFFRFRFFFFFFFFFLFFLFF",
    "FFFFFRFFFFFFFFFFFFLFFFLF",
    "FFFFRFFFFFFFFFFFLFFFFFRR",
    "FFLFFFFFRFFFLFFFRFFFLFFL",
    "FRFFFFFFFFFFFFLFFFFFRFFR",
    "FRFFFFLFFFFFRFFFFFFFFRFR",
    "FRRFFFFFFFRFFFFFFLFFLFFL",
    "LFFFFFFFRFFFFFFLFFFRFFLL",
    "RFFFFFFFFFFFLFFFFFFRFRFF",
    "RFFFRFFFRFFFFFFFFLFFFFLF",
    "RFFLFFFFFFFFFFFFRFFFFFRR",
    "RFFRFFFLFFFFRFFFFLFFFLFL",
    "RRFFFFFFFFLFFFFFFFFRFRFF",
    "RRFFFFFFFFLFFFFFRFFFFRFF",
    "FFFFFFFFFFFRFFFFFFFFFFLFL",
    "FFFFFFFFFLFFFFFFFFFFFFRFR",
    "FFFFFFFLFFFFFFFFFFFFFFRFR",
    "FFFLFFFFFFFRFFFFFFFFFLFLF",
    "FFFLFFFFFLFFFFFRFFFFFRFRF",
    "FLFFFFFFFFFFFLFFFFFRFFRFR",
    "FLFFFFFFFRFFFFLFFFFFLFFLF",
    "FRFFFFFFFFFFRFFFFFFLFFLFL",
    "LFFFFFFFFFFFRFFFFFFFFFLFL",
    "LFFFFFFFFFFFRFFFFFLFLFFFL",
    "RFFFFFFFRFFFFFFFLFLFFRFFR",
    "RFFFFFFRFFFFFFFLFFFLFFRFR",
    "FFFFLFFFFFFFFFFFFFFRFFFFRF",
    "FLFFFFFFRFFFFFFLFFFFFFLFFL",
    "LFFFFFFFFFFFFRFFFFLFFFFFLF",
    "LFFFFLFFFRFFRFFFFFLFFFLFLF",
    "LLFFLFFFFFFFFFRFFFLFFFLLFF",
    "RFFFFFFLFFFFFFRFFFFFFFFFRR",
    "RFFFFFFLFFFFFFRFFFFFFRFFFR",
    "RFFFFLFFFLFFFFFFFFFFRFFFRF",
    "RFLFFFFFFFFFFFFRFFFFFFRFFR",
    "FFFFFFRFFFFRFFFFLFFFFFFLFFL",
    "FFFFFLFFFFFFFFFLFFFFRFFRFFR",
    "FFLFFFLFFFFRFFRFFFFFFFFLLFF",
    "FRFFFFFFFFFFLFFFFFFFFFFFRFR",
    "LFFFFFFFFFRFFRFFFFFFFFLFFFL",
    "LLFFFRFFFFFFFFFFFFLFFFFFLFF",
    "FFFFFFFLFFFFRFFRFFFFFFFFFLFL",
    "FFFFFFRFFFLFFLFFFFFFRFFFRRFL",
    "FFFFFLFFFFFFRFRFFFFLFFFFFFFL",
    "FFFLFFFFRFFRFFFFFFFFFFLFFFLF",
    "LFFFFFLFFFFFFRFFFFFFRFFRFRFR",
    "FFFFFRFFFFFFFFFFFFFFLFFFFFLFF",
    "FFFFLFFFFLFFFFFFFFFFRFFFFRFRF",
    "FFFFRFFFLFFLFFFFFFFFFFFFRFFFR",
    "FFLFFFFFFFFFFFFFFRFFFFFFFFLLF",
    "FRFFRFFFFFFFFFRFFFFFFFLFFLFFF",
    "FFFFFFFFRFFFFLFLFFFFFFFFFFRFFR",
    "FFFFFFFFRFFFRFFFFFFLFFFFFFLFLF",
    "FFFFFFFRFFFRFFFFFFFFFLFFFLFFLF",
    "FFFLFFFFFFFFFRFFFFFFFFFLFFFFLF",
    "FFLFFFFFFFFFLFFFFFFFFRFFFFRFFR",
    "FLFFFFFFFFFFFFRFFFFLFFFFFFLFFL",
    "FLFFFFFFFFFFRFRFFFFFFFFFFFLFFL",
    "FLFFFFFFFFRFFFFFFFFLFFFFFLFLFR",
    "FLFFFFRFFFLFFFLFFFFFFRFFFFFFLL",
    "LFFFRFFFFFLFFFFFFFFFFFFRFFFRRF",
    "RFFFFFFFFFFFFFRFFFFFFLFFFFLFFL",
    "RFFFFFFFLFFFFFRFFFFFFFFFFRRFFL",
    "FFFLFFFFFRFFFFFFFLFFFFFFFRFRFFR",
    "FRFFFFFFFFFLFFFFFFFFFRFFFRFFFFR",
    "LFFFLFFFFFFFFRFFFFFFFFFFLFFFFLF",
    "RFFFFFLFFFFFFRFRFFFFFLFFFFFFFLL",
    "FFFFFFFFLFFFFFFLFFFFFFRFFFFRFFFFR",
    "FFFFFFLFFFFFFRFFFRFFFFFFFFFFLFLFF",
    "FFFFFFRFFFFFLFLFFFFFFFFFFFFRFFFFR",
    "FFFFLFFFFFFFFFRFFRFFFFFFFFFFFLFFL",
    "FFFFLFFFFFFRFFFFFRFFFFFLFFFFFLFFL",
    "FFFRFFFFFFFFFRFFFFFFLFFFFFFFLFFFL",
    "FRFFFFFFFFLFFFFFLFFFFFFFFRFFFRFFF",
    "FFFFFFFFLFFFFFFFRFFFRFFFFFFFFFFLFL",
    "LFFFFRFFFRFFFFFFFFLFFFFFFFFLFFFLFF",
    "FFLFFFLFFFFFFFFRFFFFRFFFFFFFLFFFFLF",
    "RFFFFFFFFLFFFFFFRFFFFFFFFFLFFFRFFRF",
    "RFFFFLFFFFFFFFFFFRFRFFFFFFFFFFFFLFL",
    "FFFLFFFFFFFFFFFRFFFRFFFFFFFFFFFFLFFL",
    "FRFFFFLFFFFFFFFFLFFFFFFFFRFFFFFFFFRR",
    "RFFFFFLFFFFFFFFFFFFRFRFFFFFFFFFFFLFL",
    "FFFFFFFLFFFFFFFRFRFFFFFFFFFFFFFFLFLFF",
    "FFLFFFFFFFRFFFFFFFFRFFFFLFFFFFFFLFLFR",
    "FFFFFLFFFFFFFFFFFFRFRFFFFFFFFFFFFLFFFL",
    "FFFLFFFFFFFFFRFFRFFFFFFFFFFFFFFFFFFLFL",
    "FFRFFFRFFFFFFLFFFFFFFLFFFFFFFFFFRFFFFR",
    "LFLFFFFFFFFFFFFRFFFFFFFFLFFFFFFFFLFFFF",
    "FFFFFFLFFFFFFFFFFFFFRFRFFFFFFFFFFFFFLFL",
    "FFFFFLFFFFFFFFFFFRFFRFFFFFFFFFFFFFFLFLF",
    "RFFFFFFLFFFFFFFFFFRFRFFFFFFFFFFFFFFLFLF",
    "FFFFFFFFFRFFFFFFLFLFFFFFFFFFFFRFFFFFFRFF",
    "FFFLFFFFFFFFFFFFFRFFRFFFFFFFFFFFFFFLFFLF",
    "FFFRFFFRFFFFFLFFFRFFFFFFFFLFFFFFFFLFFFLF",
    "RFFFFFRFFFRFFFFFFFFFFFLFFFFFLFFFFFFRFRFF",
    "FFFFFFLFFFFFFFFFLFFFFFFFFFFFFRFFFRFFFFFFF",
    "LFFFFFRFFFFFFFFFFFFFFFFFFLFLFFFFFFFFRFRFF",
    "RFFFFFFFFFFFFFLFFLFFFFFFFFFFFFFFFFFRFFFRF",
    "FFFFLFFFFFFFFFFFFFFFFFFFFFRFFRFFFFFFFLFFFL",
    "FFFFLFFFFFFFFFFFFFRFFFRFFFFFFFFFFFFLFFFFLFF",
    "FFFFFFFFFFFFFFFFFFLFFFFFFFFFLFFFFFFFFFFFRFFRF",
    "FFFFLFFFFFFFFRFFFFFRFFFFFFFFFFLFFFFFFLFFFFFFL",
    "FLFFFFLFFFFFFFFFFFFFFFFFFFFRFFRFFFFFFFFFFLFFL",
    "FFLFFFFFFFFFFFFFFFRFFRFFFFFFFFFFFFFFFFFFFFLFFFL",
    "LFFFFFFFFFFFFFFFFFRFFFFFFFFRFFFFFFFFFLFFFFLFFFL",
    "RFFFFFFRFFFRFFFFFFFFFFFFRFFFFFFLFFFFFFFFFFFFLFF",
    "FFFFFFFFFFLFFFFFFLFFFFFFFFFFRFFFFFFFFFFRFFFFFFRR",
    "RFFFFFFFFFFFFFFFFRFFFFFFFRFFFFFFFFFFFFFFFFLFFFFLFFFFFF",
    "FFFLFFFFFFFFFFFFFFFFFFFFRFFRFFFFFFFFFFFFFFFFFFFFFFFFFLFFFFFLFFF",
    "FFFFFFFLFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFRFRFFFFFFFFFFFFFFFFLFFLF",
};


namespace sa4r {

constexpr int SA4MacroMax = 64;
using SA4Macro = CapArr<char, SA4MacroMax>;
static_assert(SA4MacroMax <= numeric_limits<i8>::max());

struct MacroRange {
    i8 l_;
    i8 r_;

    bool operator==(const MacroRange& r) const {
        return l_ == r.l_ && r_ == r.r_;
    }
    bool operator!=(const MacroRange& r) const {
        return l_ != r.l_ || r_ != r.r_;
    }
};
static_assert(sizeof(MacroRange) == 2);

template <class SRC>
bool AssignSA4Macro(SA4Macro& dst, const SRC& src) {
    int n = (int)src.size();
    if (n > SA4MacroMax) {
        return false;
    }
    dst.resize(n);
    REP(i, n) {
        dst[i] = src[i];
    }
    return true;
}

SA4Macro MakeSA4Macro(const char* src) {
    SA4Macro macro;
    for (int i = 0; src[i] != 0; ++i) {
        VASSERT(i < SA4MacroMax);
        macro.push(src[i]);
    }
    return macro;
}

template <int CAP>
bool BuildSA4MacroRange(const SA4Macro& baseMacro, const MacroRange& range, CapArr<char, CAP>& macro) {
    if (range.l_ < 0 || range.l_ > range.r_ || range.r_ > (int)baseMacro.size()) {
        return false;
    }
    int len = range.r_ - range.l_;
    if (len > macro.capacity()) {
        return false;
    }

    macro.resize(len);
    REP(i, len) {
        macro[i] = baseMacro[range.l_ + i];
    }
    return true;
}

template <int LCAP, int RCAP>
int CompareSA4Macro(const CapArr<char, LCAP>& lhs, const CapArr<char, RCAP>& rhs) {
    int len = min(lhs.size(), rhs.size());
    REP(i, len) {
        if (lhs[i] != rhs[i]) {
            return lhs[i] < rhs[i] ? -1 : 1;
        }
    }
    if (lhs.size() == rhs.size()) {
        return 0;
    }
    return lhs.size() < rhs.size() ? -1 : 1;
}

bool IsValidRanges(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& ranges) {
    int n = (int)baseMacro.size();
    if (n > SA4MacroMax) {
        return false;
    }
    for (cauto& range : ranges) {
        if (range.l_ < 0 || range.l_ > range.r_ || range.r_ > n) {
            return false;
        }
    }
    FOR(i, 1, ranges.size()) {
        if (ranges[i].l_ > ranges[i - 1].l_ || ranges[i].r_ < ranges[i - 1].r_) {
            return false;
        }
    }
    return true;
}

void NormalizeRangesToBase(const SA4Macro& baseMacro, CapArr<MacroRange, M_max * 2>& ranges) {
    int n = (int)baseMacro.size();
    for (auto& range : ranges) {
        if (range.l_ < 0) {
            range.l_ = 0;
        }
        if (range.r_ < 0) {
            range.r_ = 0;
        }
        if (range.l_ > n) {
            range.l_ = n;
        }
        if (range.r_ > n) {
            range.r_ = n;
        }
        if (range.l_ > range.r_) {
            range.l_ = range.r_;
        }
    }
    FOR(i, 1, ranges.size()) {
        if (ranges[i].l_ > ranges[i - 1].l_) {
            ranges[i].l_ = ranges[i - 1].l_;
        }
        if (ranges[i].r_ < ranges[i - 1].r_) {
            ranges[i].r_ = ranges[i - 1].r_;
        }
    }
    VASSERT(IsValidRanges(baseMacro, ranges));
}

void InsertBaseMacroCommand(SA4Macro& baseMacro, CapArr<MacroRange, M_max * 2>& ranges, int pos, char command) {
    VASSERT((int)baseMacro.size() < SA4MacroMax);
    VASSERT(0 <= pos && pos <= (int)baseMacro.size());
    baseMacro.insert(pos, command);

    REP(k, ranges.size()) {
        auto& range = ranges[k];
        if (pos < range.l_) {
            ++range.l_;
            ++range.r_;
        }
        else if (range.l_ <= pos && pos <= range.r_) {
            ++range.r_;
        }
    }
    NormalizeRangesToBase(baseMacro, ranges);
}

bool EraseBaseMacroCommand(SA4Macro& baseMacro, CapArr<MacroRange, M_max * 2>& ranges, int pos) {
    if ((int)baseMacro.size() <= 1) {
        return false;
    }
    VASSERT(0 <= pos && pos < (int)baseMacro.size());
    baseMacro.remove(pos);

    REP(k, ranges.size()) {
        auto& range = ranges[k];
        if (pos < range.l_) {
            --range.l_;
            --range.r_;
        }
        else if (range.l_ <= pos && pos < range.r_) {
            --range.r_;
        }
    }
    NormalizeRangesToBase(baseMacro, ranges);
    return true;
}

bool IsValidOrder(const MArr<u8>& order) {
    if ((int)order.size() != M) {
        return false;
    }

    MArr<bool> used(M, false);
    for (u8 ball : order) {
        if ((int)ball >= M || used[ball]) {
            return false;
        }
        used[ball] = true;
    }
    return true;
}

constexpr int SA4IntervalMax = M_max * 2;
constexpr int SA4RangeStateMax = SA4IntervalMax + 1;
constexpr int SA4InvalidPrev = 255;
constexpr int SA4DPInf = INT_MAX / 4;
constexpr u16 SA4PathStatInf = numeric_limits<u16>::max();
using SA4MacroKey = u64;
using SA4RunMacro = RunMacro<SA4MacroMax>;

struct SA4Cost {
    int turn_;
    int step_;
};

struct SA4PathOption {
    int turn_;
    int step_;
    u8 helperIdx_;
    bool valid_;
};

struct SA4IntervalPathOptions {
    array<array<SA4PathOption, 4>, 4> basic_;
    array<array<SA4PathOption, 4>, 4> macro_;
    array<array<SA4PathOption, 4>, 4> builder_;

    void Clear() {
        for (int fromDir = 0; fromDir < 4; ++fromDir) {
            for (int toDir = 0; toDir < 4; ++toDir) {
                basic_[fromDir][toDir].valid_ = false;
                macro_[fromDir][toDir].valid_ = false;
                builder_[fromDir][toDir].valid_ = false;
            }
        }
    }
};

struct SA4IntervalPathStat {
    u16 turn_;
    u16 step_;
    u16 savedTurn_;
    u8 endDir_;
    u8 usedMacro_;
};

struct SA4TurnOnlyPathStat {
    u16 turn_;
    u16 savedTurn_;
    u8 endDir_;
    u8 usedMacro_;
};
static_assert(sizeof(SA4IntervalPathStat) == 8);
static_assert(sizeof(SA4TurnOnlyPathStat) == 6);

u16 ToSA4PathStatValue(int value) {
    VASSERT(0 <= value && value < (int)SA4PathStatInf);
    if (value < 0) {
        return 0;
    }
    if (value >= (int)SA4PathStatInf) {
        return SA4PathStatInf - 1;
    }
    return (u16)value;
}

constexpr int SA4PathCacheCapacityHint = 400000;
constexpr int SA4TurnOnlyPathCacheCapacityHint = 4000000;
constexpr int SA4TurnOnlyPathHotCacheBits = 16;
constexpr int SA4TurnOnlyPathHotCacheSize = 1 << SA4TurnOnlyPathHotCacheBits;
constexpr int SA4TurnOnlyPathStartCellMax = M_max * 2 + 1;
constexpr int SA4TurnOnlyPathTargetCellMax = M_max * 2;
constexpr int SA4TurnOnlyPathPairMax = SA4TurnOnlyPathStartCellMax * 4 * SA4TurnOnlyPathTargetCellMax;
constexpr u16 SA4TurnOnlyPathInvalidPair = numeric_limits<u16>::max();
static_assert(SA4TurnOnlyPathPairMax < SA4TurnOnlyPathHotCacheSize);
constexpr int SA4MacroCacheCapacity = (1 << 17) - 1;

struct SA4TurnOnlyPathHotCacheEntry {
    u64 key_;
    SA4TurnOnlyPathStat stat_;
    bool valid_;
};

struct SA4TurnOnlyPathHotCache {
    array<SA4TurnOnlyPathHotCacheEntry, SA4TurnOnlyPathHotCacheSize> entries_;

    void Clear() {
        for (auto& entry : entries_) {
            entry.valid_ = false;
        }
    }

    int Index(u32 hotIndex) const {
        return (int)(hotIndex & (SA4TurnOnlyPathHotCacheSize - 1));
    }

    bool Find(u64 key, u32 hotIndex, SA4TurnOnlyPathStat& stat) const {
        cauto& entry = entries_[Index(hotIndex)];
        if (!entry.valid_ || entry.key_ != key) {
            return false;
        }
        stat = entry.stat_;
        return true;
    }

    void Set(u64 key, u32 hotIndex, const SA4TurnOnlyPathStat& stat) {
        auto& entry = entries_[Index(hotIndex)];
        entry.key_ = key;
        entry.stat_ = stat;
        entry.valid_ = true;
    }
};

dhmap<u64, SA4IntervalPathStat> sa4PathCache_;
dhmap<u64, SA4TurnOnlyPathStat> sa4TurnOnlyPathCache_;
SA4TurnOnlyPathHotCache sa4TurnOnlyPathHotCache_;
dhmap<u64, MacroCache> sa4MacroCache_;
array<array<int, NN_max * 4>, NN_max * 4> sa4NoMacroTurns_;

void InitSA4NoMacroTurns() {
    static array<CapArr<int, NN_max * 4>, 2> ques;
    static CheckMapS<NN_max * 4> pushed;
    static CapArr<int, NN_max * 4> dists;

    REP(startIdx, NN * 4) {
        auto& turns = sa4NoMacroTurns_[startIdx];
        REP(idx, NN * 4) {
            turns[idx] = INT_MAX;
        }

        dists.assign(NN * 4, INT_MAX);
        ques[0].clear();
        ques[1].clear();
        pushed.Clear();

        dists[startIdx] = 0;
        if (pushed.Enter(startIdx)) {
            ques[0].push(startIdx);
        }

        int curQueIdx = 0;
        while (!ques[curQueIdx].empty()) {
            auto& curQue = ques[curQueIdx];
            auto& nextQue = ques[curQueIdx ^ 1];
            nextQue.clear();
            pushed.Clear();

            for (int curIdx : curQue) {
                int curCell;
                int curDir;
                FromCellDirIdx(curIdx, curCell, curDir);
                if (turns[curIdx] == INT_MAX) {
                    turns[curIdx] = dists[curIdx];
                }

                int nextDist = dists[curIdx] + 1;
                auto PushIdx = [&](int nextIdx) {
                    if (nextDist < dists[nextIdx]) {
                        dists[nextIdx] = nextDist;
                        if (pushed.Enter(nextIdx)) {
                            nextQue.push(nextIdx);
                        }
                    }
                };

                int forwardCell = gs.dirs(curCell)[curDir];
                if (forwardCell >= 0) {
                    PushIdx(ToCellDirIdx(forwardCell, curDir));
                }
                PushIdx(ToCellDirIdx(curCell, (curDir + 3) & 3));
                PushIdx(ToCellDirIdx(curCell, (curDir + 1) & 3));
            }

            curQueIdx ^= 1;
        }
    }
}

int GetSA4NoMacroTurn(int startCell, int fromDir, int targetCell, int targetDir) {
    return sa4NoMacroTurns_[ToCellDirIdx(startCell, fromDir)][ToCellDirIdx(targetCell, targetDir)];
}

SA4TurnOnlyPathStat MakeSA4TurnOnlyPathStat(int startCell, int fromDir, int targetCell, int targetDir, int turn) {
    int noMacroTurn = GetSA4NoMacroTurn(startCell, fromDir, targetCell, targetDir);
    u16 savedTurn = 0;
    u8 usedMacro = 0;
    if (noMacroTurn == INT_MAX) {
        usedMacro = 1;
    }
    else if (noMacroTurn > turn) {
        savedTurn = ToSA4PathStatValue(noMacroTurn - turn);
        usedMacro = 1;
    }
    return SA4TurnOnlyPathStat{ToSA4PathStatValue(turn), savedTurn, (u8)targetDir, usedMacro};
}

struct SA4MacroHasher {
    array<array<u64, 4>, SA4MacroMax> charHash_;
    array<u64, SA4MacroMax + 1> lenHash_;
    array<i8, 256> charToKind_;
    array<array<u64, SA4MacroMax + 1>, SA4MacroMax> baseMacroRangeXor_;
    SA4Macro preparedBaseMacro_;
    bool prepared_ = false;

    void Init(Rng& rng) {
        REP(i, SA4MacroMax) {
            REP(kind, 4) {
                charHash_[i][kind] = rng.Get64();
            }
        }
        REP(len, SA4MacroMax + 1) {
            lenHash_[len] = rng.Get64();
        }

        charToKind_.fill(-1);
        charToKind_['F'] = 0;
        charToKind_['L'] = 1;
        charToKind_['R'] = 2;
        charToKind_['S'] = 3;

        preparedBaseMacro_.clear();
        prepared_ = false;
    }

    void PrepareBaseMacro(const SA4Macro& baseMacro) {
        if (prepared_ && preparedBaseMacro_ == baseMacro) {
            return;
        }

        preparedBaseMacro_ = baseMacro;
        int n = (int)baseMacro.size();
        VASSERT(n <= SA4MacroMax);

        REP(start, n) {
            auto& rangeXor = baseMacroRangeXor_[start];
            rangeXor[0] = 0;
            u64 h = 0;
            int maxLen = min(SA4MacroMax, n - start);
            REP(i, maxLen) {
                int kind = charToKind_[(unsigned char)baseMacro[start + i]];
                h ^= charHash_[i][kind];
                rangeXor[i + 1] = h;
            }
        }
        prepared_ = true;
    }

    void CalcPreparedKey(const MacroRange& range, SA4MacroKey& key) {
        VASSERT(prepared_);
        VASSERT(0 <= range.l_ && range.l_ <= range.r_ && range.r_ <= (int)preparedBaseMacro_.size());
        int len = range.r_ - range.l_;
        VASSERT(len <= SA4MacroMax);
        if (len == 0) {
            key = lenHash_[len];
            return;
        }
        VASSERT(range.l_ < SA4MacroMax);
        key = lenHash_[len] ^ baseMacroRangeXor_[range.l_][range.r_ - range.l_];
    }

    void CalcKey(const SA4Macro& baseMacro, const MacroRange& range, SA4MacroKey& key) {
        PrepareBaseMacro(baseMacro);
        CalcPreparedKey(range, key);
    }
};
SA4MacroHasher sa4MacroHasher_;

struct SA4PathCacheHasher {
    array<u8, NN_max> startCellId_;
    array<u8, NN_max> targetCellId_;
    array<u64, SA4TurnOnlyPathPairMax> pairHash_;
    int targetCellCount_;

    void Init(Rng& rng) {
        startCellId_.fill(255);
        targetCellId_.fill(255);

        int startCellCount = 0;
        targetCellCount_ = 0;
        auto AddStartCell = [&](int cell) {
            VASSERT(0 <= cell && cell < NN);
            if (startCellId_[cell] == 255) {
                VASSERT(startCellCount < SA4TurnOnlyPathStartCellMax);
                startCellId_[cell] = (u8)startCellCount++;
            }
        };
        auto AddTargetCell = [&](int cell) {
            VASSERT(0 <= cell && cell < NN);
            if (targetCellId_[cell] == 255) {
                VASSERT(targetCellCount_ < SA4TurnOnlyPathTargetCellMax);
                targetCellId_[cell] = (u8)targetCellCount_++;
            }
        };

        AddStartCell(0);
        REP(ball, M) {
            AddStartCell(initialBallCells_[ball]);
            AddStartCell(ballToGoalCell_[ball]);
            AddTargetCell(initialBallCells_[ball]);
            AddTargetCell(ballToGoalCell_[ball]);
        }

        REP(i, SA4TurnOnlyPathPairMax) {
            pairHash_[i] = rng.Get64();
        }
    }

    u16 CalcPairId(int startCell, int fromDir, int targetCell) const {
        VASSERT(0 <= startCell && startCell < NN);
        VASSERT(0 <= targetCell && targetCell < NN);
        VASSERT(0 <= fromDir && fromDir < 4);

        int startId = startCellId_[startCell];
        int targetId = targetCellId_[targetCell];
        VASSERT(startId != 255 && targetId != 255);
        int pairId = ((startId << 2) | fromDir) * targetCellCount_ + targetId;
        VASSERT(0 <= pairId && pairId < SA4TurnOnlyPathPairMax);
        return (u16)pairId;
    }

    u64 CalcKey(u16 pairId, const SA4MacroKey& macroKey) const {
        VASSERT(pairId != SA4TurnOnlyPathInvalidPair);
        return pairHash_[pairId] ^ macroKey;
    }

    u32 CalcHotIndex(u16 pairId, const SA4MacroKey& macroKey) const {
        VASSERT(pairId != SA4TurnOnlyPathInvalidPair);
        return ((u32)macroKey ^ (u32)(macroKey >> 32) ^ pairId) & (SA4TurnOnlyPathHotCacheSize - 1);
    }

    void Calc(int startCell, int fromDir, int targetCell, const SA4MacroKey& macroKey, u64& key, u32& hotIndex) const {
        u16 pairId = CalcPairId(startCell, fromDir, targetCell);
        VASSERT(pairId != SA4TurnOnlyPathInvalidPair);
        key = CalcKey(pairId, macroKey);
        hotIndex = CalcHotIndex(pairId, macroKey);
    }
};
SA4PathCacheHasher sa4PathCachHasser_;

bool BuildSA4Macro(const SA4Macro& baseMacro, const MacroRange& range, SA4Macro& macro) {
    return BuildSA4MacroRange(baseMacro, range, macro);
}

bool IsValidSA4MacroRange(const SA4Macro& baseMacro, const MacroRange& range) {
    if (range.l_ < 0 || range.l_ > range.r_ || range.r_ > (int)baseMacro.size()) {
        return false;
    }
    return range.r_ - range.l_ <= SA4MacroMax;
}

MacroCache& GetCachedSA4MacroCache(const SA4Macro& baseMacro, const MacroRange& range, const SA4MacroKey& macroKey) {
    static MacroCache fallbackMacroCache;

    VASSERT(IsValidSA4MacroRange(baseMacro, range));

    u32 cacheIndex;
    if (sa4MacroCache_.Find(macroKey, cacheIndex)) {
        return sa4MacroCache_.GetWithIndex(cacheIndex);
    }

    SA4Macro macro;
    bool built = BuildSA4Macro(baseMacro, range, macro);
    VASSERT(built);

    MacroCache macroCache;
    if (built) {
        macroCache.Init(macro);
    }
    if (sa4MacroCache_.size() + 1 >= sa4MacroCache_.capacity()) {
        fallbackMacroCache = macroCache;
        return fallbackMacroCache;
    }
    sa4MacroCache_.InsertWithIndex(cacheIndex, macroKey, macroCache);
    return sa4MacroCache_.GetWithIndex(cacheIndex);
}

bool FindCachedBestPathTurnOnly(int startCell, int fromDir, int targetCell, const SA4Macro& baseMacro,
                                const MacroRange& range, const SA4MacroKey& macroKey, SA4TurnOnlyPathStat& bestStat);
bool FindCachedBestPathTurnOnlyByKey(int startCell, int fromDir, int targetCell, const SA4Macro& baseMacro,
                                     const MacroRange& range, const SA4MacroKey& macroKey, u64 key, u32 hotIndex,
                                     SA4TurnOnlyPathStat& bestStat);

bool FindPathStepOnly(int startIdx, int targetIdx, const MacroCache& macroCache, int& targetTurn, int& targetStep) {
    constexpr u8 InfTurn = 255;
    static array<CapArr<int, NN_max * 4>, 2> ques;
    static CheckMapS<NN_max * 4> pushed;
    static CapArr<u8, NN_max * 4> dists;
    static CapArr<u8, NN_max * 4> macroCounts;

    targetTurn = INT_MAX;
    targetStep = INT_MAX;

    dists.assign(NN * 4, InfTurn);
    macroCounts.resize(NN * 4);

    auto& startQue = ques[0];
    startQue.clear();
    ques[1].clear();
    pushed.Clear();

    dists[startIdx] = 0;
    macroCounts[startIdx] = 0;
    if (pushed.Enter(startIdx)) {
        startQue.push(startIdx);
    }

    int macroStep = macroCache.macro_.size();
    auto Push = [&](CapArr<int, NN_max * 4>& nextQue, CheckMapS<NN_max * 4>& pushed, int nextIdx, int nextDist,
                    int nextMacroCount) {
        if (nextDist >= InfTurn) {
            return;
        }
        if (nextDist < dists[nextIdx] || (nextDist == dists[nextIdx] && nextMacroCount < macroCounts[nextIdx])) {
            dists[nextIdx] = (u8)nextDist;
            macroCounts[nextIdx] = (u8)nextMacroCount;
            if (pushed.Enter(nextIdx)) {
                nextQue.push(nextIdx);
            }
        }
    };

    int curQueIdx = 0;
    while (!ques[curQueIdx].empty()) {
        auto& curQue = ques[curQueIdx];
        auto& nextQue = ques[curQueIdx ^ 1];
        nextQue.clear();
        pushed.Clear();

        for (int curIdx : curQue) {
            if (curIdx == targetIdx) {
                targetTurn = dists[curIdx];
                targetStep = targetTurn + macroCounts[curIdx] * (macroStep - 1);
                return true;
            }

            int nextDist = dists[curIdx] + 1;
            int nextMacroCount = macroCounts[curIdx];

            cauto& nextIdxs = gs.nextCellDirIdx(curIdx);
            if (nextIdxs[0] >= 0) {
                Push(nextQue, pushed, nextIdxs[0], nextDist, nextMacroCount);
            }
            Push(nextQue, pushed, nextIdxs[1], nextDist, nextMacroCount);
            Push(nextQue, pushed, nextIdxs[2], nextDist, nextMacroCount);
            Push(nextQue, pushed, macroCache.CalcAppliedIdx(curIdx), nextDist, nextMacroCount + 1);
        }

        curQueIdx ^= 1;
    }

    return false;
}

bool FindCachedBestPathStat(int startCell, int fromDir, int targetCell, const SA4Macro& baseMacro,
                            const MacroRange& range, const SA4MacroKey& macroKey, SA4IntervalPathStat& bestStat) {
    VASSERT(IsValidSA4MacroRange(baseMacro, range));

    u64 key;
    u32 hotIndex;
    sa4PathCachHasser_.Calc(startCell, fromDir, targetCell, macroKey, key, hotIndex);
    u32 cacheIndex;
    if (sa4PathCache_.Find(key, cacheIndex)) {
        SA4IntervalPathStat& cachedStat = sa4PathCache_.GetWithIndex(cacheIndex);
        if (cachedStat.turn_ == SA4PathStatInf) {
            sa4TurnOnlyPathHotCache_.Set(key, hotIndex, SA4TurnOnlyPathStat{SA4PathStatInf, 0, 0, 0});
            return false;
        }
        sa4TurnOnlyPathHotCache_.Set(
            key, hotIndex,
            SA4TurnOnlyPathStat{cachedStat.turn_, cachedStat.savedTurn_, cachedStat.endDir_, cachedStat.usedMacro_});
        bestStat = cachedStat;
        return true;
    }

    SA4TurnOnlyPathStat turnOnlyStat;
    if (!FindCachedBestPathTurnOnlyByKey(startCell, fromDir, targetCell, baseMacro, range, macroKey, key, hotIndex,
                                         turnOnlyStat)) {
        return false;
    }

    if (!turnOnlyStat.usedMacro_) {
        SA4IntervalPathStat cachedStat;
        cachedStat.turn_ = turnOnlyStat.turn_;
        cachedStat.step_ = turnOnlyStat.turn_;
        cachedStat.savedTurn_ = turnOnlyStat.savedTurn_;
        cachedStat.endDir_ = turnOnlyStat.endDir_;
        cachedStat.usedMacro_ = turnOnlyStat.usedMacro_;
        sa4PathCache_.InsertWithIndex(cacheIndex, key, cachedStat);
        bestStat = cachedStat;
        return true;
    }

    MacroCache& macroCache = GetCachedSA4MacroCache(baseMacro, range, macroKey);
    int startIdx = ToCellDirIdx(startCell, fromDir);
    int targetIdx = ToCellDirIdx(targetCell, turnOnlyStat.endDir_);

    SA4IntervalPathStat cachedStat;
    int turn;
    int step;
    if (FindPathStepOnly(startIdx, targetIdx, macroCache, turn, step)) {
        VASSERT(turn == turnOnlyStat.turn_);
        cachedStat.turn_ = turnOnlyStat.turn_;
        cachedStat.step_ = ToSA4PathStatValue(step);
        cachedStat.savedTurn_ = turnOnlyStat.savedTurn_;
        cachedStat.endDir_ = turnOnlyStat.endDir_;
        cachedStat.usedMacro_ = turnOnlyStat.usedMacro_;
    }
    else {
        cachedStat = SA4IntervalPathStat{SA4PathStatInf, 0, 0, 0, 0};
    }

    sa4PathCache_.InsertWithIndex(cacheIndex, key, cachedStat);

    if (cachedStat.turn_ == SA4PathStatInf) {
        return false;
    }
    bestStat = cachedStat;
    return true;
}

bool FindCachedBestPathStatWithTurnOnly(int startCell, int fromDir, int targetCell, const SA4Macro& baseMacro,
                                        const MacroRange& range, const SA4MacroKey& macroKey,
                                        const SA4TurnOnlyPathStat& turnOnlyStat, SA4IntervalPathStat& bestStat) {
    VASSERT(IsValidSA4MacroRange(baseMacro, range));

    u64 key;
    u32 hotIndex;
    sa4PathCachHasser_.Calc(startCell, fromDir, targetCell, macroKey, key, hotIndex);

    u32 cacheIndex;
    if (sa4PathCache_.Find(key, cacheIndex)) {
        SA4IntervalPathStat& cachedStat = sa4PathCache_.GetWithIndex(cacheIndex);
        if (cachedStat.turn_ == SA4PathStatInf) {
            sa4TurnOnlyPathHotCache_.Set(key, hotIndex, SA4TurnOnlyPathStat{SA4PathStatInf, 0, 0, 0});
            return false;
        }
        sa4TurnOnlyPathHotCache_.Set(key, hotIndex, turnOnlyStat);
        bestStat = cachedStat;
        return true;
    }

    if (!turnOnlyStat.usedMacro_) {
        SA4IntervalPathStat cachedStat;
        cachedStat.turn_ = turnOnlyStat.turn_;
        cachedStat.step_ = turnOnlyStat.turn_;
        cachedStat.savedTurn_ = turnOnlyStat.savedTurn_;
        cachedStat.endDir_ = turnOnlyStat.endDir_;
        cachedStat.usedMacro_ = turnOnlyStat.usedMacro_;
        sa4PathCache_.InsertWithIndex(cacheIndex, key, cachedStat);
        bestStat = cachedStat;
        return true;
    }

    MacroCache& macroCache = GetCachedSA4MacroCache(baseMacro, range, macroKey);
    int startIdx = ToCellDirIdx(startCell, fromDir);
    int targetIdx = ToCellDirIdx(targetCell, turnOnlyStat.endDir_);

    SA4IntervalPathStat cachedStat;
    int turn;
    int step;
    if (FindPathStepOnly(startIdx, targetIdx, macroCache, turn, step)) {
        VASSERT(turn == turnOnlyStat.turn_);
        cachedStat.turn_ = turnOnlyStat.turn_;
        cachedStat.step_ = ToSA4PathStatValue(step);
        cachedStat.savedTurn_ = turnOnlyStat.savedTurn_;
        cachedStat.endDir_ = turnOnlyStat.endDir_;
        cachedStat.usedMacro_ = turnOnlyStat.usedMacro_;
    }
    else {
        cachedStat = SA4IntervalPathStat{SA4PathStatInf, 0, 0, 0, 0};
    }

    sa4PathCache_.InsertWithIndex(cacheIndex, key, cachedStat);
    sa4TurnOnlyPathHotCache_.Set(
        key, hotIndex,
        cachedStat.turn_ == SA4PathStatInf ? SA4TurnOnlyPathStat{SA4PathStatInf, 0, 0, 0} : turnOnlyStat);

    if (cachedStat.turn_ == SA4PathStatInf) {
        return false;
    }
    bestStat = cachedStat;
    return true;
}

bool FindBestPathTurnOnly(int startCell, int fromDir, int targetCell, const MacroCache& macroCache,
                          SA4TurnOnlyPathStat& bestStat) {
    int reachedIdx;
    int turn = BitCalcTurn(ToCellDirIdx(startCell, fromDir), targetCell, macroCache, reachedIdx);
    if (turn == INT_MAX) {
        return false;
    }

    int reachedCell = reachedIdx >> 2;
    int reachedDir = reachedIdx & 3;
    VASSERT(reachedCell == targetCell);

    bestStat = MakeSA4TurnOnlyPathStat(startCell, fromDir, targetCell, reachedDir, turn);
    return true;
}

bool FindCachedBestPathTurnOnlyByKey(int startCell, int fromDir, int targetCell, const SA4Macro& baseMacro,
                                     const MacroRange& range, const SA4MacroKey& macroKey, u64 key, u32 hotIndex,
                                     SA4TurnOnlyPathStat& bestStat) {
    VASSERT(IsValidSA4MacroRange(baseMacro, range));

    if (sa4TurnOnlyPathHotCache_.Find(key, hotIndex, bestStat)) {
        return bestStat.turn_ != SA4PathStatInf;
    }

    u32 cacheIndex;
    if (sa4TurnOnlyPathCache_.Find(key, cacheIndex)) {
        SA4TurnOnlyPathStat& cachedStat = sa4TurnOnlyPathCache_.GetWithIndex(cacheIndex);
        sa4TurnOnlyPathHotCache_.Set(key, hotIndex, cachedStat);
        if (cachedStat.turn_ == SA4PathStatInf) {
            return false;
        }
        bestStat = cachedStat;
        return true;
    }

    MacroCache& macroCache = GetCachedSA4MacroCache(baseMacro, range, macroKey);

    SA4TurnOnlyPathStat cachedStat;
    bool found = FindBestPathTurnOnly(startCell, fromDir, targetCell, macroCache, cachedStat);
    if (!found) {
        cachedStat = SA4TurnOnlyPathStat{SA4PathStatInf, 0, 0, 0};
    }
    sa4TurnOnlyPathCache_.InsertWithIndex(cacheIndex, key, cachedStat);
    sa4TurnOnlyPathHotCache_.Set(key, hotIndex, cachedStat);

    if (!found) {
        return false;
    }
    bestStat = cachedStat;
    return true;
}

bool FindCachedBestPathTurnOnly(int startCell, int fromDir, int targetCell, const SA4Macro& baseMacro,
                                const MacroRange& range, const SA4MacroKey& macroKey, SA4TurnOnlyPathStat& bestStat) {
    VASSERT(IsValidSA4MacroRange(baseMacro, range));

    u64 key;
    u32 hotIndex;
    sa4PathCachHasser_.Calc(startCell, fromDir, targetCell, macroKey, key, hotIndex);
    return FindCachedBestPathTurnOnlyByKey(startCell, fromDir, targetCell, baseMacro, range, macroKey, key, hotIndex,
                                           bestStat);
}

constexpr u8 SA4ActionBasic = 0;
constexpr u8 SA4ActionMacro = 1;
constexpr u8 SA4ActionBuilder = 2;
constexpr int SA4BuilderStateMax = NN_max * 4 * 2;
constexpr int SA4BuilderMaxHelperLen = 8;
constexpr int SA4BuilderMaxHelpers = 12;
constexpr int SA4BuilderHelperCandidateMax = SA4MacroMax * SA4BuilderMaxHelperLen;
using SA4BuilderHelperMacro = CapArr<char, SA4BuilderMaxHelperLen>;

struct SA4BuilderHelperCandidate {
    int saving_;
    SA4BuilderHelperMacro macro_;
};

bool IsBetterCost(int turn, int step, const SA4Cost& current) {
    if (current.turn_ >= SA4DPInf) {
        return true;
    }
    if (turn != current.turn_) {
        return turn < current.turn_;
    }
    return step < current.step_;
}

bool IsBetterPathOption(int turn, int step, const SA4PathOption& current) {
    if (!current.valid_) {
        return true;
    }
    if (turn != current.turn_) {
        return turn < current.turn_;
    }
    return step < current.step_;
}

bool SameMacroRangeContent(const SA4Macro& baseMacro, int start, const MacroRange& range) {
    int len = range.r_ - range.l_;
    if (len <= 0 || start < 0 || start + len > (int)baseMacro.size()) {
        return false;
    }
    REP(i, len) {
        if (baseMacro[start + i] != baseMacro[range.l_ + i]) {
            return false;
        }
    }
    return true;
}

template <int CAP>
bool SameMacroContent(const SA4Macro& baseMacro, int start, const CapArr<char, CAP>& helperMacro) {
    int len = helperMacro.size();
    if (len <= 0 || start < 0 || start + len > (int)baseMacro.size()) {
        return false;
    }
    REP(i, len) {
        if (baseMacro[start + i] != helperMacro[i]) {
            return false;
        }
    }
    return true;
}

int CalcMacroRecordingLen(const SA4Macro& baseMacro, const MacroRange& targetRange, const MacroRange* prevRange) {
    VASSERT(0 <= targetRange.l_ && targetRange.l_ <= targetRange.r_ && targetRange.r_ <= (int)baseMacro.size());

    int targetLen = targetRange.r_ - targetRange.l_;
    int prevLen = prevRange == nullptr ? 0 : prevRange->r_ - prevRange->l_;
    if (prevLen <= 0) {
        return targetLen;
    }

    int len = 0;
    for (int i = targetRange.l_; i < targetRange.r_;) {
        if (i + prevLen <= targetRange.r_ && SameMacroRangeContent(baseMacro, i, *prevRange)) {
            ++len;
            i += prevLen;
        }
        else {
            ++len;
            ++i;
        }
    }
    return len;
}

template <int CAP>
int CalcMacroRecordingLen(const SA4Macro& baseMacro, const MacroRange& targetRange,
                          const CapArr<char, CAP>& helperMacro) {
    VASSERT(0 <= targetRange.l_ && targetRange.l_ <= targetRange.r_ && targetRange.r_ <= (int)baseMacro.size());

    int helperLen = helperMacro.size();
    int len = 0;
    for (int i = targetRange.l_; i < targetRange.r_;) {
        if (helperLen > 0 && i + helperLen <= targetRange.r_ && SameMacroContent(baseMacro, i, helperMacro)) {
            ++len;
            i += helperLen;
        }
        else {
            ++len;
            ++i;
        }
    }
    return len;
}

void AppendMacroRecordingCommands(const SA4Macro& baseMacro, const MacroRange& targetRange, const MacroRange* prevRange,
                                  dvec<char>& result) {
    VASSERT(0 <= targetRange.l_ && targetRange.l_ <= targetRange.r_ && targetRange.r_ <= (int)baseMacro.size());

    int prevLen = prevRange == nullptr ? 0 : prevRange->r_ - prevRange->l_;
    for (int i = targetRange.l_; i < targetRange.r_;) {
        if (prevLen > 0 && i + prevLen <= targetRange.r_ && SameMacroRangeContent(baseMacro, i, *prevRange)) {
            result.push('P');
            i += prevLen;
        }
        else {
            result.push(baseMacro[i]);
            ++i;
        }
    }
}

template <int CAP>
void AppendMacroRecordingCommands(const SA4Macro& baseMacro, const MacroRange& targetRange,
                                  const CapArr<char, CAP>& helperMacro, dvec<char>& result) {
    VASSERT(0 <= targetRange.l_ && targetRange.l_ <= targetRange.r_ && targetRange.r_ <= (int)baseMacro.size());

    int helperLen = helperMacro.size();
    for (int i = targetRange.l_; i < targetRange.r_;) {
        if (helperLen > 0 && i + helperLen <= targetRange.r_ && SameMacroContent(baseMacro, i, helperMacro)) {
            result.push('P');
            i += helperLen;
        }
        else {
            result.push(baseMacro[i]);
            ++i;
        }
    }
}

template <int CAP>
int CalcBuilderMacroBuildTurn(const SA4Macro& baseMacro, const MacroRange& targetRange,
                              const CapArr<char, CAP>& helperMacro) {
    return (int)helperMacro.size() + 2 + CalcMacroRecordingLen(baseMacro, targetRange, helperMacro) + 2;
}

template <int CAP>
int CalcBuilderMacroBuildStep(const MacroRange& targetRange, const CapArr<char, CAP>& helperMacro) {
    return (int)helperMacro.size() + targetRange.r_ - targetRange.l_;
}

template <int CAP>
void AppendBuilderMacroBuildCommands(const SA4Macro& baseMacro, const MacroRange& targetRange,
                                     const CapArr<char, CAP>& helperMacro, dvec<char>& result) {
    result.push('M');
    for (char c : helperMacro) {
        result.push(c);
    }
    result.push('M');
    result.push('M');
    AppendMacroRecordingCommands(baseMacro, targetRange, helperMacro, result);
    result.push('M');
}

void BuildBuilderHelperCandidates(const SA4Macro& baseMacro, const MacroRange& targetRange,
                                  array<SA4BuilderHelperMacro, SA4BuilderMaxHelpers>& helpers, int& helperCount) {
    helperCount = 0;
    int targetLen = targetRange.r_ - targetRange.l_;
    if (targetLen <= 0) {
        return;
    }

    CapArr<SA4BuilderHelperCandidate, SA4BuilderHelperCandidateMax> cands;
    int maxHelperLen = min(SA4BuilderMaxHelperLen, targetLen);
    FOR(len, 2, maxHelperLen + 1) {
        for (int start = targetRange.l_; start + len <= targetRange.r_; ++start) {
            SA4BuilderHelperMacro helperMacro;
            bool builtHelper = BuildSA4MacroRange(baseMacro, MacroRange{(i8)start, (i8)(start + len)}, helperMacro);
            VASSERT(builtHelper);
            int recordingLen = CalcMacroRecordingLen(baseMacro, targetRange, helperMacro);
            int savingAgainstLiteralAfterHelper = targetLen + 2 - (recordingLen + 4);
            if (savingAgainstLiteralAfterHelper <= 0) {
                continue;
            }

            bool exists = false;
            for (cauto& cand : cands) {
                if (cand.macro_ == helperMacro) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                auto& cand = cands.push();
                cand.saving_ = savingAgainstLiteralAfterHelper;
                cand.macro_ = helperMacro;
            }
        }
    }

    cands.sort([](cauto& lhs, cauto& rhs) {
        if (lhs.saving_ != rhs.saving_) {
            return lhs.saving_ > rhs.saving_;
        }
        if (lhs.macro_.size() != rhs.macro_.size()) {
            return lhs.macro_.size() > rhs.macro_.size();
        }
        return CompareSA4Macro(lhs.macro_, rhs.macro_) < 0;
    });

    for (cauto& cand : cands) {
        if (helperCount >= SA4BuilderMaxHelpers) {
            break;
        }
        helpers[helperCount++] = cand.macro_;
    }
}

int CalcMacroUpdateExtra(const SA4Macro& baseMacro, const MacroRange& prevRange, const MacroRange& targetRange) {
    if (prevRange == targetRange) {
        return 0;
    }

    const MacroRange* prevRangePtr = prevRange.l_ < 0 ? nullptr : &prevRange;
    return CalcMacroRecordingLen(baseMacro, targetRange, prevRangePtr) + 1;
}

int CalcMacroUpdateExtra(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange, int rangeState,
                         int intervalIdx, int noneRangeState) {
    cauto& interval = intervalsRange[intervalIdx];
    if (rangeState == noneRangeState) {
        return CalcMacroRecordingLen(baseMacro, interval, nullptr) + 1;
    }
    if (intervalsRange[rangeState] == interval) {
        return 0;
    }

    cauto& prevRange = intervalsRange[rangeState];
    int extra = CalcMacroRecordingLen(baseMacro, interval, &prevRange) + 1;
    VASSERT(extra >= 0);
    return extra;
}

bool AppendPathCommandsWithMacroUpdate(const SA4Macro& baseMacro, const MacroRange& prevRange,
                                       const MacroRange& targetRange, const dvec<char>& pathCommands,
                                       dvec<char>& result) {
    if (prevRange == targetRange) {
        result.extend(pathCommands);
        return true;
    }

    const MacroRange* prevRangePtr = prevRange.l_ < 0 ? nullptr : &prevRange;
    bool replaced = false;
    for (char c : pathCommands) {
        if (!replaced && c == 'P') {
            replaced = true;
            result.push('M');
            AppendMacroRecordingCommands(baseMacro, targetRange, prevRangePtr, result);
            result.push('M');
        }
        else {
            result.push(c);
        }
    }
    return replaced;
}

bool AppendPathCommandsWithMacroUpdate(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                                       int prevRangeState, int intervalIdx, int noneRangeState,
                                       const dvec<char>& pathCommands, dvec<char>& result) {
    cauto& interval = intervalsRange[intervalIdx];
    MacroRange prevRange =
        prevRangeState == noneRangeState ? MacroRange{(i8)-1, (i8)-1} : intervalsRange[prevRangeState];
    return AppendPathCommandsWithMacroUpdate(baseMacro, prevRange, interval, pathCommands, result);
}

void InitNoOpMacroCache(MacroCache& macroCache) {
    macroCache.InitNoOp();
}

bool InitSA4MacroCacheFromRange(MacroCache& macroCache, const SA4Macro& baseMacro, const MacroRange& range) {
    SA4Macro macro;
    bool built = BuildSA4Macro(baseMacro, range, macro);
    if (!built) {
        return false;
    }
    macroCache.Init(macro);
    return true;
}

int ToBuilderPathState(int phase, int publicIdx) {
    return phase * (NN * 4) + publicIdx;
}

void FromBuilderPathState(int state, int& phase, int& publicIdx) {
    int publicStateCount = NN * 4;
    phase = state / publicStateCount;
    publicIdx = state - phase * publicStateCount;
}

struct SA4BuilderPathResult {
    CapArr<int, SA4BuilderStateMax> dists_;
    CapArr<int, SA4BuilderStateMax> steps_;
    CapArr<int, SA4BuilderStateMax> froms_;
    CapArr<char, SA4BuilderStateMax> fromCommands_;

    int GetDist(int publicIdx) const {
        return dists_[ToBuilderPathState(1, publicIdx)];
    }

    int GetStep(int publicIdx) const {
        return steps_[ToBuilderPathState(1, publicIdx)];
    }

    void BuildCommand(int publicIdx, const SA4Macro& baseMacro, const MacroRange& targetRange,
                      const SA4BuilderHelperMacro& helperMacro, dvec<char>& commands) const {
        dvec<char> tokens;
        int state = ToBuilderPathState(1, publicIdx);
        while (true) {
            int fromState = froms_[state];
            if (fromState < 0) {
                break;
            }
            tokens.push(fromCommands_[state]);
            state = fromState;
        }

        reverse(ALL(tokens));
        commands.clear();
        for (char c : tokens) {
            if (c == 'B') {
                AppendBuilderMacroBuildCommands(baseMacro, targetRange, helperMacro, commands);
            }
            else {
                commands.push(c);
            }
        }
    }
};

void FindPathWithBuilder(const dvec<int>& startIdxs, const dvec<int>& endIdxs, const MacroCache& targetMacroCache,
                         const MacroCache& helperMacroCache, int builderTurn, int builderStep,
                         SA4BuilderPathResult& result, bool firstFindEnd = true) {
    VASSERT(!endIdxs.empty());

    static CheckMapS<SA4BuilderStateMax> targets;
    targets.Clear();
    int targetCount = 0;
    for (int idx : endIdxs) {
        int targetState = ToBuilderPathState(1, idx);
        if (!targets[targetState]) {
            targets.Check(targetState);
            ++targetCount;
        }
    }
    int completeCount = 0;

    int publicStateCount = NN * 4;
    int stateCount = publicStateCount * 2;
    auto& dists = result.dists_;
    auto& steps = result.steps_;
    auto& froms = result.froms_;
    auto& fromCommands = result.fromCommands_;

    dists.assign(stateCount, INT_MAX);
    steps.resize(stateCount);
    froms.resize(stateCount);
    fromCommands.resize(stateCount);

    using Node = tuple<int, int, int>;
    static priority_queue<Node, vector<Node>, greater<Node>> que;
    while (!que.empty()) {
        que.pop();
    }

    for (int idx : startIdxs) {
        int state = ToBuilderPathState(0, idx);
        dists[state] = 0;
        steps[state] = 0;
        froms[state] = -1;
        fromCommands[state] = '*';
        que.push({0, 0, state});
    }

    auto Push = [&](int nextState, int nextDist, int nextStep, int curState, char command) {
        if (nextDist < dists[nextState] || (nextDist == dists[nextState] && nextStep < steps[nextState])) {
            dists[nextState] = nextDist;
            steps[nextState] = nextStep;
            froms[nextState] = curState;
            fromCommands[nextState] = command;
            que.push({nextDist, nextStep, nextState});
        }
    };

    int targetMacroStep = targetMacroCache.macro_.size();
    while (!que.empty()) {
        auto [curDist, curStep, curState] = que.top();
        que.pop();
        if (curDist != dists[curState] || curStep != steps[curState]) {
            continue;
        }

        if (targets[curState]) {
            targets.reset(curState);
            ++completeCount;
            if (firstFindEnd || completeCount == targetCount) {
                break;
            }
        }

        int phase = curState >= publicStateCount ? 1 : 0;
        int phaseOffset = phase * publicStateCount;
        int publicIdx = curState - phaseOffset;

        cauto& nextIdxs = gs.nextCellDirIdx(publicIdx);
        if (nextIdxs[0] >= 0) {
            Push(phaseOffset + nextIdxs[0], curDist + 1, curStep + 1, curState, 'F');
        }
        Push(phaseOffset + nextIdxs[1], curDist + 1, curStep + 1, curState, 'L');
        Push(phaseOffset + nextIdxs[2], curDist + 1, curStep + 1, curState, 'R');

        if (phase == 0) {
            int afterHelper = helperMacroCache.CalcAppliedIdx(publicIdx);
            int afterTarget = targetMacroCache.CalcAppliedIdx(afterHelper);
            Push(publicStateCount + afterTarget, curDist + builderTurn, curStep + builderStep, curState, 'B');
        }
        else {
            int afterTarget = targetMacroCache.CalcAppliedIdx(publicIdx);
            Push(publicStateCount + afterTarget, curDist + 1, curStep + targetMacroStep, curState, 'P');
        }
    }
}

bool SolveFixedOrderDP(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                       const MArr<u8>& order, int limitTurn, int& totalTurn, int& totalStep, dvec<char>* result) {
    VASSERT(IsValidOrder(order));
    VASSERT(IsValidRanges(baseMacro, intervalsRange));

    constexpr int DirN = 4;
    int intervalCount = M * 2;
    int noneRangeState = intervalCount;
    int rangeStateCount = intervalCount + 1;

    static array<int, SA4IntervalMax> targetCells;
    REP(i, M) {
        int ball = order[i];
        targetCells[i * 2] = initialBallCells_[ball];
        targetCells[i * 2 + 1] = ballToGoalCell_[ball];
    }

    static array<SA4IntervalPathOptions, SA4IntervalMax> pathOptions;
    static dvec<int> startIdxs;
    static dvec<int> endIdxs;
    static FindPathResult basicPathResult;
    static FindPathResult macroPathResult;
    static SA4BuilderPathResult builderPathResult;
    static MacroCache basicMacroCache;
    static MacroCache macroCache;
    static MacroCache builderHelperCache;
    static array<array<SA4BuilderHelperMacro, SA4BuilderMaxHelpers>, SA4IntervalMax> builderHelpers;
    static array<int, SA4IntervalMax> builderHelperCounts;
    InitNoOpMacroCache(basicMacroCache);

    REP(intervalIdx, intervalCount) {
        pathOptions[intervalIdx].Clear();
        builderHelperCounts[intervalIdx] = -1;

        int startCell = intervalIdx == 0 ? 0 : targetCells[intervalIdx - 1];
        int targetCell = targetCells[intervalIdx];
        if (!InitSA4MacroCacheFromRange(macroCache, baseMacro, intervalsRange[intervalIdx])) {
            return false;
        }

        endIdxs.clear();
        REP(toDir, DirN) {
            endIdxs.push(ToCellDirIdx(targetCell, toDir));
        }

        REP(fromDir, DirN) {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(startCell, fromDir));

            FindPath(startIdxs, endIdxs, basicMacroCache, basicPathResult, false);
            REP(toDir, DirN) {
                int idx = ToCellDirIdx(targetCell, toDir);
                int turn = basicPathResult.GetDist(idx);
                if (turn == INT_MAX) {
                    continue;
                }
                auto& option = pathOptions[intervalIdx].basic_[fromDir][toDir];
                option.valid_ = true;
                option.turn_ = turn;
                option.step_ = basicPathResult.GetStep(idx);
            }

            FindPath(startIdxs, endIdxs, macroCache, macroPathResult, false);
            REP(toDir, DirN) {
                int idx = ToCellDirIdx(targetCell, toDir);
                int turn = macroPathResult.GetDist(idx);
                if (turn == INT_MAX || !macroPathResult.IsUsedMacro(idx)) {
                    continue;
                }
                auto& option = pathOptions[intervalIdx].macro_[fromDir][toDir];
                option.valid_ = true;
                option.turn_ = turn;
                option.step_ = macroPathResult.GetStep(idx);
            }
        }
    }

    auto BuildBuilderOptions = [&](int intervalIdx) {
        if (builderHelperCounts[intervalIdx] >= 0) {
            return;
        }

        BuildBuilderHelperCandidates(baseMacro, intervalsRange[intervalIdx], builderHelpers[intervalIdx],
                                     builderHelperCounts[intervalIdx]);
        if (builderHelperCounts[intervalIdx] == 0) {
            return;
        }

        int startCell = intervalIdx == 0 ? 0 : targetCells[intervalIdx - 1];
        int targetCell = targetCells[intervalIdx];
        if (!InitSA4MacroCacheFromRange(macroCache, baseMacro, intervalsRange[intervalIdx])) {
            builderHelperCounts[intervalIdx] = 0;
            return;
        }

        endIdxs.clear();
        REP(toDir, DirN) {
            endIdxs.push(ToCellDirIdx(targetCell, toDir));
        }

        REP(fromDir, DirN) {
            startIdxs.clear();
            startIdxs.push(ToCellDirIdx(startCell, fromDir));

            REP(helperIdx, builderHelperCounts[intervalIdx]) {
                cauto& helperMacro = builderHelpers[intervalIdx][helperIdx];
                builderHelperCache.Init(helperMacro);
                int builderTurn = CalcBuilderMacroBuildTurn(baseMacro, intervalsRange[intervalIdx], helperMacro);
                int builderStep = CalcBuilderMacroBuildStep(intervalsRange[intervalIdx], helperMacro);
                FindPathWithBuilder(startIdxs, endIdxs, macroCache, builderHelperCache, builderTurn, builderStep,
                                    builderPathResult, false);

                REP(toDir, DirN) {
                    int idx = ToCellDirIdx(targetCell, toDir);
                    int turn = builderPathResult.GetDist(idx);
                    if (turn == INT_MAX) {
                        continue;
                    }
                    int step = builderPathResult.GetStep(idx);
                    auto& option = pathOptions[intervalIdx].builder_[fromDir][toDir];
                    if (IsBetterPathOption(turn, step, option)) {
                        option.valid_ = true;
                        option.turn_ = turn;
                        option.step_ = step;
                        option.helperIdx_ = (u8)helperIdx;
                    }
                }
            }
        }
    };

    static SA4Cost dp[2][SA4RangeStateMax][DirN];
    static u8 prevRange[SA4IntervalMax + 1][SA4RangeStateMax][DirN];
    static u8 prevDir[SA4IntervalMax + 1][SA4RangeStateMax][DirN];
    static u8 prevAction[SA4IntervalMax + 1][SA4RangeStateMax][DirN];
    static u8 prevBuilderHelper[SA4IntervalMax + 1][SA4RangeStateMax][DirN];
    static array<int, SA4IntervalMax + 1> routeRange;
    static array<int, SA4IntervalMax + 1> routeDir;
    static array<u8, SA4IntervalMax> routeAction;
    static array<u8, SA4IntervalMax> routeBuilderHelper;

    auto ClearLayer = [&](int layer) {
        REP(rangeState, rangeStateCount) {
            REP(dir, DirN) {
                dp[layer][rangeState][dir].turn_ = SA4DPInf;
            }
        }
    };

    auto RunDP = [&](bool useBuilder, bool restoreRoute) -> bool {
        ClearLayer(0);
        ClearLayer(1);
        dp[0][noneRangeState][2] = SA4Cost{0, 0};

        REP(intervalIdx, intervalCount) {
            int curLayer = intervalIdx & 1;
            int nextLayer = curLayer ^ 1;
            ClearLayer(nextLayer);

            REP(rangeState, rangeStateCount) {
                int updateExtra = 0;
                bool calculatedUpdateExtra = false;
                REP(fromDir, DirN) {
                    cauto& cur = dp[curLayer][rangeState][fromDir];
                    if (cur.turn_ >= SA4DPInf) {
                        continue;
                    }

                    auto Relax = [&](int nextRangeState, int toDir, u8 action, int addTurn, int addStep, u8 helperIdx) {
                        int nextTurn = cur.turn_ + addTurn + 1;
                        int nextStep = cur.step_ + addStep + 1;
                        if (nextTurn >= limitTurn || nextStep > T) {
                            return;
                        }

                        auto& dst = dp[nextLayer][nextRangeState][toDir];
                        if (!IsBetterCost(nextTurn, nextStep, dst)) {
                            return;
                        }

                        dst.turn_ = nextTurn;
                        dst.step_ = nextStep;
                        prevRange[intervalIdx + 1][nextRangeState][toDir] = (u8)rangeState;
                        prevDir[intervalIdx + 1][nextRangeState][toDir] = (u8)fromDir;
                        prevAction[intervalIdx + 1][nextRangeState][toDir] = action;
                        prevBuilderHelper[intervalIdx + 1][nextRangeState][toDir] = helperIdx;
                    };

                    REP(toDir, DirN) {
                        cauto& basicOption = pathOptions[intervalIdx].basic_[fromDir][toDir];
                        if (basicOption.valid_) {
                            Relax(rangeState, toDir, SA4ActionBasic, basicOption.turn_, basicOption.step_,
                                  SA4InvalidPrev);
                        }

                        cauto& macroOption = pathOptions[intervalIdx].macro_[fromDir][toDir];
                        if (macroOption.valid_) {
                            if (!calculatedUpdateExtra) {
                                updateExtra = CalcMacroUpdateExtra(baseMacro, intervalsRange, rangeState, intervalIdx,
                                                                   noneRangeState);
                                calculatedUpdateExtra = true;
                            }
                            Relax(intervalIdx, toDir, SA4ActionMacro, macroOption.turn_ + updateExtra,
                                  macroOption.step_, SA4InvalidPrev);
                        }

                        cauto& builderOption = pathOptions[intervalIdx].builder_[fromDir][toDir];
                        if (useBuilder && rangeState == noneRangeState && builderOption.valid_) {
                            Relax(intervalIdx, toDir, SA4ActionBuilder, builderOption.turn_, builderOption.step_,
                                  builderOption.helperIdx_);
                        }
                    }
                }
            }
        }

        int bestRangeState = -1;
        int bestDir = -1;
        int lastLayer = intervalCount & 1;
        SA4Cost bestCost;
        bestCost.turn_ = SA4DPInf;
        REP(rangeState, rangeStateCount) {
            REP(dir, DirN) {
                cauto& cost = dp[lastLayer][rangeState][dir];
                if (cost.turn_ >= SA4DPInf) {
                    continue;
                }
                if (IsBetterCost(cost.turn_, cost.step_, bestCost)) {
                    bestCost = cost;
                    bestRangeState = rangeState;
                    bestDir = dir;
                }
            }
        }

        if (bestRangeState < 0) {
            return false;
        }

        totalTurn = bestCost.turn_;
        totalStep = bestCost.step_;
        if (!restoreRoute) {
            return true;
        }

        routeRange[intervalCount] = bestRangeState;
        routeDir[intervalCount] = bestDir;

        for (int intervalIdx = intervalCount; intervalIdx >= 1; --intervalIdx) {
            int rangeState = routeRange[intervalIdx];
            int dir = routeDir[intervalIdx];
            u8 pr = prevRange[intervalIdx][rangeState][dir];
            u8 pd = prevDir[intervalIdx][rangeState][dir];
            if (pr == SA4InvalidPrev || pd == SA4InvalidPrev) {
                return false;
            }
            routeRange[intervalIdx - 1] = pr;
            routeDir[intervalIdx - 1] = pd;
            routeAction[intervalIdx - 1] = prevAction[intervalIdx][rangeState][dir];
            routeBuilderHelper[intervalIdx - 1] = prevBuilderHelper[intervalIdx][rangeState][dir];
        }

        return routeRange[0] == noneRangeState && routeDir[0] == 2;
    };

    if (!RunDP(false, true)) {
        return false;
    }

    int firstMacroInterval = -1;
    REP(intervalIdx, intervalCount) {
        if (routeAction[intervalIdx] == SA4ActionMacro) {
            firstMacroInterval = intervalIdx;
            break;
        }
    }
    if (firstMacroInterval < 0) {
        firstMacroInterval = intervalCount - 1;
    }

    REP(intervalIdx, firstMacroInterval + 1) {
        BuildBuilderOptions(intervalIdx);
    }

    if (!RunDP(true, true)) {
        return false;
    }
    if (!result) {
        return true;
    }

    dvec<char>* output = result;
    static dvec<char> builtCommands;
    builtCommands.clear();
    result = &builtCommands;

    static dvec<char> tmpCommand;
    REP(intervalIdx, intervalCount) {
        int startCell = intervalIdx == 0 ? 0 : targetCells[intervalIdx - 1];
        int targetCell = targetCells[intervalIdx];
        int fromDir = routeDir[intervalIdx];
        int toDir = routeDir[intervalIdx + 1];
        u8 action = routeAction[intervalIdx];

        startIdxs.clear();
        startIdxs.push(ToCellDirIdx(startCell, fromDir));

        endIdxs.clear();
        int targetIdx = ToCellDirIdx(targetCell, toDir);
        endIdxs.push(targetIdx);

        if (action == SA4ActionMacro) {
            if (!InitSA4MacroCacheFromRange(macroCache, baseMacro, intervalsRange[intervalIdx])) {
                return false;
            }
            FindPath(startIdxs, endIdxs, macroCache, macroPathResult, true);
            if (macroPathResult.GetDist(targetIdx) == INT_MAX || !macroPathResult.IsUsedMacro(targetIdx)) {
                return false;
            }
            macroPathResult.BuildCommand(targetIdx, tmpCommand);
            if (!AppendPathCommandsWithMacroUpdate(baseMacro, intervalsRange, routeRange[intervalIdx], intervalIdx,
                                                   noneRangeState, tmpCommand, *result)) {
                return false;
            }
        }
        else if (action == SA4ActionBuilder) {
            u8 helperIdx = routeBuilderHelper[intervalIdx];
            if (helperIdx >= builderHelperCounts[intervalIdx]) {
                return false;
            }

            if (!InitSA4MacroCacheFromRange(macroCache, baseMacro, intervalsRange[intervalIdx])) {
                return false;
            }
            cauto& helperMacro = builderHelpers[intervalIdx][helperIdx];
            builderHelperCache.Init(helperMacro);
            int builderTurn = CalcBuilderMacroBuildTurn(baseMacro, intervalsRange[intervalIdx], helperMacro);
            int builderStep = CalcBuilderMacroBuildStep(intervalsRange[intervalIdx], helperMacro);
            FindPathWithBuilder(startIdxs, endIdxs, macroCache, builderHelperCache, builderTurn, builderStep,
                                builderPathResult, true);
            if (builderPathResult.GetDist(targetIdx) == INT_MAX) {
                return false;
            }
            builderPathResult.BuildCommand(targetIdx, baseMacro, intervalsRange[intervalIdx], helperMacro, tmpCommand);
            result->extend(tmpCommand);
        }
        else {
            FindPath(startIdxs, endIdxs, basicMacroCache, basicPathResult, true);
            if (basicPathResult.GetDist(targetIdx) == INT_MAX || basicPathResult.IsUsedMacro(targetIdx)) {
                return false;
            }
            basicPathResult.BuildCommand(targetIdx, tmpCommand);
            result->extend(tmpCommand);
        }
        result->push('S');
    }

    if ((int)result->size() != totalTurn) {
        return false;
    }
    *output = *result;
    return true;
}

bool BuildFixedOrderFromPathStats(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                                  const MArr<u8>& order, const CapArr<SA4IntervalPathStat, M_max * 2>& pathStats,
                                  int expectedTurn, int expectedStep, dvec<char>& result) {
    VASSERT(IsValidOrder(order));
    VASSERT(IsValidRanges(baseMacro, intervalsRange));

    static dvec<int> startIdxs;
    static dvec<int> endIdxs;
    static FindPathResult basicPathResult;
    static FindPathResult macroPathResult;
    static MacroCache basicMacroCache;
    static MacroCache macroCache;
    static dvec<char> tmpCommand;
    InitNoOpMacroCache(basicMacroCache);

    result.clear();

    int totalTurn = 0;
    int totalStep = 0;
    int lastTargetCell = 0;
    int fromDir = 2;
    MacroRange curRange = {-1, -1};

    REP(intervalIdx, M * 2) {
        int ball = order[intervalIdx >> 1];
        int targetCell = (intervalIdx & 1) ? ballToGoalCell_[ball] : initialBallCells_[ball];
        cauto& stat = pathStats[intervalIdx];
        int targetIdx = ToCellDirIdx(targetCell, stat.endDir_);

        startIdxs.clear();
        startIdxs.push(ToCellDirIdx(lastTargetCell, fromDir));
        endIdxs.clear();
        endIdxs.push(targetIdx);

        if (stat.usedMacro_) {
            if (!InitSA4MacroCacheFromRange(macroCache, baseMacro, intervalsRange[intervalIdx])) {
                return false;
            }
            FindPath(startIdxs, endIdxs, macroCache, macroPathResult, true);
            if (macroPathResult.GetDist(targetIdx) != stat.turn_ || macroPathResult.GetStep(targetIdx) != stat.step_ ||
                !macroPathResult.IsUsedMacro(targetIdx)) {
                return false;
            }

            macroPathResult.BuildCommand(targetIdx, tmpCommand);
            if (!AppendPathCommandsWithMacroUpdate(baseMacro, curRange, intervalsRange[intervalIdx], tmpCommand,
                                                   result)) {
                return false;
            }
            if (curRange != intervalsRange[intervalIdx]) {
                totalTurn += CalcMacroUpdateExtra(baseMacro, curRange, intervalsRange[intervalIdx]);
                curRange = intervalsRange[intervalIdx];
            }
        }
        else {
            FindPath(startIdxs, endIdxs, basicMacroCache, basicPathResult, true);
            if (basicPathResult.GetDist(targetIdx) != stat.turn_ || basicPathResult.GetStep(targetIdx) != stat.step_) {
                return false;
            }
            basicPathResult.BuildCommand(targetIdx, tmpCommand);
            result.extend(tmpCommand);
        }

        result.push('S');
        totalTurn += stat.turn_ + 1;
        totalStep += stat.step_ + 1;
        lastTargetCell = targetCell;
        fromDir = stat.endDir_;
    }

    return totalTurn == expectedTurn && totalStep == expectedStep && expectedStep <= T &&
           (int)result.size() == expectedTurn;
}

void BuildSA4IntervalMacroKeys(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                               int intervalCount, CapArr<SA4MacroKey, M_max * 2>& macroKeys) {
    sa4MacroHasher_.PrepareBaseMacro(baseMacro);
    macroKeys.resize(intervalCount);
    REP(intervalIdx, intervalCount) {
        sa4MacroHasher_.CalcPreparedKey(intervalsRange[intervalIdx], macroKeys[intervalIdx]);
    }
}

int CalcTurnOnlyIntervalStepLowerBound(const MacroRange& interval, const SA4TurnOnlyPathStat& stat) {
    int intervalStepLowerBound = stat.turn_;
    int macroLen = interval.r_ - interval.l_;
    if (macroLen > 1 && stat.savedTurn_ > 0) {
        int minMacroUseCount = (stat.savedTurn_ + macroLen - 2) / (macroLen - 1);
        intervalStepLowerBound += minMacroUseCount * (macroLen - 1);
    }
    return intervalStepLowerBound;
}

bool EvaluateFixedOrderTurnOnlyWithCache(
    const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange, const MArr<u8>& order,
    int limitTurn, int& totalTurn, CapArr<SA4TurnOnlyPathStat, M_max * 2>& turnOnlyPathStats,
    CapArr<SA4MacroKey, M_max * 2>& macroKeys, const SA4Macro& prevBaseMacro,
    const CapArr<MacroRange, M_max * 2>& prevIntervalsRange, const MArr<u8>& prevOrder,
    const CapArr<SA4IntervalPathStat, M_max * 2>& prevPathStats, const CapArr<SA4MacroKey, M_max * 2>& prevMacroKeys,
    const CapArr<int, SA4IntervalMax + 1>& prevPrefixTurns,
    const CapArr<MacroRange, SA4IntervalMax + 1>& prevPrefixRanges, int dirtyStart) {
    VASSERT(IsValidOrder(order));
    VASSERT(IsValidRanges(baseMacro, intervalsRange));

    int intervalCount = M * 2;
    turnOnlyPathStats.resize(intervalCount);
    macroKeys.resize(intervalCount);
    VASSERT((int)prevIntervalsRange.size() == intervalCount);
    VASSERT((int)prevOrder.size() == M);
    VASSERT((int)prevPathStats.size() == intervalCount);
    VASSERT((int)prevMacroKeys.size() == intervalCount);
    VASSERT((int)prevPrefixTurns.size() == intervalCount + 1);
    VASSERT((int)prevPrefixRanges.size() == intervalCount + 1);
    VASSERT(0 <= dirtyStart && dirtyStart <= intervalCount);

    bool sameBase = &prevBaseMacro == &baseMacro || prevBaseMacro == baseMacro;
    bool sameOrder = &prevOrder == &order || prevOrder == order;

    bool preparedBaseMacro = false;
    auto CalcMacroKey = [&](const MacroRange& interval, SA4MacroKey& macroKey) {
        if (!preparedBaseMacro) {
            sa4MacroHasher_.PrepareBaseMacro(baseMacro);
            preparedBaseMacro = true;
        }
        sa4MacroHasher_.CalcPreparedKey(interval, macroKey);
    };

    auto GetTargetCell = [](const MArr<u8>& fixedOrder, int intervalIdx) {
        int ball = fixedOrder[intervalIdx >> 1];
        return (intervalIdx & 1) ? ballToGoalCell_[ball] : initialBallCells_[ball];
    };

    REP(intervalIdx, dirtyStart) {
        cauto& prevStat = prevPathStats[intervalIdx];
        turnOnlyPathStats[intervalIdx] =
            SA4TurnOnlyPathStat{prevStat.turn_, prevStat.savedTurn_, prevStat.endDir_, prevStat.usedMacro_};
        macroKeys[intervalIdx] = prevMacroKeys[intervalIdx];
    }

    totalTurn = prevPrefixTurns[dirtyStart];
    MacroRange curRange = prevPrefixRanges[dirtyStart];
    int lastTargetCell = dirtyStart == 0 ? 0 : GetTargetCell(order, dirtyStart - 1);
    int lastPrevTargetCell = dirtyStart == 0 ? 0 : GetTargetCell(prevOrder, dirtyStart - 1);
    u8 lastEndDir = dirtyStart == 0 ? 2 : prevPathStats[dirtyStart - 1].endDir_;
    u8 lastPrevEndDir = lastEndDir;
    if (totalTurn >= limitTurn) {
        return false;
    }

    FOR(intervalIdx, dirtyStart, intervalCount) {
        int startCell = lastTargetCell;
        int targetCell = GetTargetCell(order, intervalIdx);
        int prevTargetCell = sameOrder ? targetCell : GetTargetCell(prevOrder, intervalIdx);
        int fromDir = lastEndDir;

        cauto& interval = intervalsRange[intervalIdx];
        cauto& prevInterval = prevIntervalsRange[intervalIdx];
        cauto& prevMacroKey = prevMacroKeys[intervalIdx];
        auto& macroKey = macroKeys[intervalIdx];
        if (sameBase && interval == prevInterval) {
            macroKey = prevMacroKey;
        }
        else {
            CalcMacroKey(interval, macroKey);
        }

        bool reused = false;
        int prevFromDir = lastPrevEndDir;
        auto& stat = turnOnlyPathStats[intervalIdx];

        if (sameOrder) {
            if (fromDir == prevFromDir && macroKey == prevMacroKey) {
                cauto& prevStat = prevPathStats[intervalIdx];
                stat = SA4TurnOnlyPathStat{prevStat.turn_, prevStat.savedTurn_, prevStat.endDir_, prevStat.usedMacro_};
                reused = true;
            }
        }
        else {
            int prevStartCell = lastPrevTargetCell;
            if (startCell == prevStartCell && fromDir == prevFromDir && targetCell == prevTargetCell &&
                macroKey == prevMacroKey) {
                cauto& prevStat = prevPathStats[intervalIdx];
                stat = SA4TurnOnlyPathStat{prevStat.turn_, prevStat.savedTurn_, prevStat.endDir_, prevStat.usedMacro_};
                reused = true;
            }
        }

        if (!reused) {
            if (!FindCachedBestPathTurnOnly(startCell, fromDir, targetCell, baseMacro, interval, macroKey, stat)) {
                return false;
            }
        }

        int intervalTurn = stat.turn_;
        if (stat.usedMacro_ && curRange != intervalsRange[intervalIdx]) {
            intervalTurn += CalcMacroUpdateExtra(baseMacro, curRange, intervalsRange[intervalIdx]);
            curRange = intervalsRange[intervalIdx];
        }

        totalTurn += intervalTurn + 1;
        if (totalTurn >= limitTurn) {
            return false;
        }

        lastEndDir = stat.endDir_;
        lastTargetCell = targetCell;
        lastPrevTargetCell = prevTargetCell;
        lastPrevEndDir = prevPathStats[intervalIdx].endDir_;
    }

    return true;
}

bool EvaluateFixedOrderWithCache(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                                 const MArr<u8>& order, int limitTurn, int& totalTurn, int& totalStep,
                                 CapArr<SA4IntervalPathStat, M_max * 2>& pathStats,
                                 CapArr<SA4MacroKey, M_max * 2>& macroKeys, MacroRange& usedRange,
                                 const SA4Macro* prevBaseMacro = nullptr,
                                 const CapArr<MacroRange, M_max * 2>* prevIntervalsRange = nullptr,
                                 const MArr<u8>* prevOrder = nullptr,
                                 const CapArr<SA4IntervalPathStat, M_max * 2>* prevPathStats = nullptr,
                                 const CapArr<SA4MacroKey, M_max * 2>* prevMacroKeys = nullptr) {
    VASSERT(IsValidOrder(order));
    VASSERT(IsValidRanges(baseMacro, intervalsRange));

    int intervalCount = M * 2;
    pathStats.resize(intervalCount);
    macroKeys.resize(intervalCount);

    bool canReusePrev = prevBaseMacro && prevIntervalsRange && prevOrder && prevPathStats && prevMacroKeys &&
                        (int)prevIntervalsRange->size() == intervalCount && (int)prevOrder->size() == M &&
                        (int)prevPathStats->size() == intervalCount && (int)prevMacroKeys->size() == intervalCount;
    bool sameBaseAndOrder = canReusePrev && *prevBaseMacro == baseMacro && *prevOrder == order;
    bool preparedBaseMacro = false;
    auto CalcMacroKey = [&](const MacroRange& interval, SA4MacroKey& macroKey) {
        if (!preparedBaseMacro) {
            sa4MacroHasher_.PrepareBaseMacro(baseMacro);
            preparedBaseMacro = true;
        }
        sa4MacroHasher_.CalcPreparedKey(interval, macroKey);
    };

    auto GetTargetCell = [](const MArr<u8>& fixedOrder, int intervalIdx) {
        int ball = fixedOrder[intervalIdx >> 1];
        return (intervalIdx & 1) ? ballToGoalCell_[ball] : initialBallCells_[ball];
    };

    totalTurn = 0;
    totalStep = 0;
    MacroRange curRange = {-1, -1};
    int lastTargetCell = 0;
    int lastPrevTargetCell = 0;

    REP(intervalIdx, intervalCount) {
        int startCell = intervalIdx == 0 ? 0 : lastTargetCell;
        int targetCell = GetTargetCell(order, intervalIdx);
        int prevTargetCell = canReusePrev && !sameBaseAndOrder ? GetTargetCell(*prevOrder, intervalIdx) : 0;
        int fromDir = intervalIdx == 0 ? 2 : pathStats[intervalIdx - 1].endDir_;
        cauto& interval = intervalsRange[intervalIdx];
        auto& macroKey = macroKeys[intervalIdx];
        if (sameBaseAndOrder && interval == (*prevIntervalsRange)[intervalIdx]) {
            macroKey = (*prevMacroKeys)[intervalIdx];
        }
        else {
            CalcMacroKey(interval, macroKey);
        }

        bool reused = false;
        if (canReusePrev) {
            int prevFromDir = intervalIdx == 0 ? 2 : (*prevPathStats)[intervalIdx - 1].endDir_;

            if (sameBaseAndOrder) {
                cauto& prevInterval = (*prevIntervalsRange)[intervalIdx];
                if (fromDir == prevFromDir && (interval == prevInterval || macroKey == (*prevMacroKeys)[intervalIdx])) {
                    pathStats[intervalIdx] = (*prevPathStats)[intervalIdx];
                    reused = true;
                }
            }
            else {
                int prevStartCell = intervalIdx == 0 ? 0 : lastPrevTargetCell;
                if (startCell == prevStartCell && fromDir == prevFromDir && targetCell == prevTargetCell &&
                    macroKey == (*prevMacroKeys)[intervalIdx]) {
                    pathStats[intervalIdx] = (*prevPathStats)[intervalIdx];
                    reused = true;
                }
            }
        }

        if (!reused) {
            auto& stat = pathStats[intervalIdx];
            if (!FindCachedBestPathStat(startCell, fromDir, targetCell, baseMacro, interval, macroKey, stat)) {
                return false;
            }
        }

        cauto& stat = pathStats[intervalIdx];
        int intervalTurn = stat.turn_;
        if (stat.usedMacro_ && curRange != intervalsRange[intervalIdx]) {
            intervalTurn += CalcMacroUpdateExtra(baseMacro, curRange, intervalsRange[intervalIdx]);
            curRange = intervalsRange[intervalIdx];
        }

        totalTurn += intervalTurn + 1;
        totalStep += stat.step_ + 1;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        lastTargetCell = targetCell;
        lastPrevTargetCell = prevTargetCell;
    }

    usedRange = curRange;

    return true;
}

bool EvaluateFixedOrderWithCache(
    const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange, const MArr<u8>& order,
    int limitTurn, int& totalTurn, int& totalStep, CapArr<SA4IntervalPathStat, M_max * 2>& pathStats,
    CapArr<SA4MacroKey, M_max * 2>& macroKeys, MacroRange& usedRange, const SA4Macro& prevBaseMacro,
    const CapArr<MacroRange, M_max * 2>& prevIntervalsRange, const MArr<u8>& prevOrder,
    const CapArr<SA4IntervalPathStat, M_max * 2>& prevPathStats, const CapArr<SA4MacroKey, M_max * 2>& prevMacroKeys,
    const CapArr<SA4TurnOnlyPathStat, M_max * 2>& turnOnlyPathStats,
    const CapArr<SA4MacroKey, M_max * 2>& turnOnlyMacroKeys, const CapArr<int, SA4IntervalMax + 1>& prevPrefixTurns,
    const CapArr<int, SA4IntervalMax + 1>& prevPrefixSteps,
    const CapArr<MacroRange, SA4IntervalMax + 1>& prevPrefixRanges, int dirtyStart,
    CapArr<int, SA4IntervalMax + 1>& prefixTurns, CapArr<int, SA4IntervalMax + 1>& prefixSteps,
    CapArr<MacroRange, SA4IntervalMax + 1>& prefixRanges) {
    VASSERT(IsValidOrder(order));
    VASSERT(IsValidRanges(baseMacro, intervalsRange));

    int intervalCount = M * 2;
    pathStats.resize(intervalCount);
    macroKeys = turnOnlyMacroKeys;
    VASSERT((int)prevIntervalsRange.size() == intervalCount);
    VASSERT((int)prevOrder.size() == M);
    VASSERT((int)prevPathStats.size() == intervalCount);
    VASSERT((int)prevMacroKeys.size() == intervalCount);
    VASSERT((int)turnOnlyPathStats.size() == intervalCount);
    VASSERT((int)turnOnlyMacroKeys.size() == intervalCount);
    VASSERT((int)prevPrefixTurns.size() == intervalCount + 1);
    VASSERT((int)prevPrefixSteps.size() == intervalCount + 1);
    VASSERT((int)prevPrefixRanges.size() == intervalCount + 1);
    VASSERT(0 <= dirtyStart && dirtyStart <= intervalCount);
    prefixTurns.resize(intervalCount + 1);
    prefixSteps.resize(intervalCount + 1);
    prefixRanges.resize(intervalCount + 1);

    bool sameOrder = &prevOrder == &order || prevOrder == order;

    auto GetTargetCell = [](const MArr<u8>& fixedOrder, int intervalIdx) {
        int ball = fixedOrder[intervalIdx >> 1];
        return (intervalIdx & 1) ? ballToGoalCell_[ball] : initialBallCells_[ball];
    };

    REP(intervalIdx, dirtyStart) {
        pathStats[intervalIdx] = prevPathStats[intervalIdx];
    }
    REP(i, dirtyStart + 1) {
        prefixTurns[i] = prevPrefixTurns[i];
        prefixSteps[i] = prevPrefixSteps[i];
        prefixRanges[i] = prevPrefixRanges[i];
    }

    totalTurn = prevPrefixTurns[dirtyStart];
    totalStep = prevPrefixSteps[dirtyStart];
    MacroRange curRange = prevPrefixRanges[dirtyStart];
    int lastTargetCell = dirtyStart == 0 ? 0 : GetTargetCell(order, dirtyStart - 1);
    int lastPrevTargetCell = dirtyStart == 0 ? 0 : GetTargetCell(prevOrder, dirtyStart - 1);
    u8 lastEndDir = dirtyStart == 0 ? 2 : prevPathStats[dirtyStart - 1].endDir_;
    u8 lastPrevEndDir = lastEndDir;
    if (totalTurn >= limitTurn || totalStep > T) {
        return false;
    }

    FOR(intervalIdx, dirtyStart, intervalCount) {
        int startCell = lastTargetCell;
        int targetCell = GetTargetCell(order, intervalIdx);
        int prevTargetCell = sameOrder ? targetCell : GetTargetCell(prevOrder, intervalIdx);
        int fromDir = lastEndDir;

        cauto& interval = intervalsRange[intervalIdx];
        cauto& macroKey = macroKeys[intervalIdx];
        cauto& prevMacroKey = prevMacroKeys[intervalIdx];

        bool reused = false;
        int prevFromDir = lastPrevEndDir;
        if (sameOrder) {
            if (fromDir == prevFromDir && macroKey == prevMacroKey) {
                pathStats[intervalIdx] = prevPathStats[intervalIdx];
                reused = true;
            }
        }
        else {
            int prevStartCell = lastPrevTargetCell;
            if (startCell == prevStartCell && fromDir == prevFromDir && targetCell == prevTargetCell &&
                macroKey == prevMacroKey) {
                pathStats[intervalIdx] = prevPathStats[intervalIdx];
                reused = true;
            }
        }

        if (!reused) {
            auto& stat = pathStats[intervalIdx];
            cauto& turnOnlyStat = turnOnlyPathStats[intervalIdx];
            if (!FindCachedBestPathStatWithTurnOnly(startCell, fromDir, targetCell, baseMacro, interval, macroKey,
                                                    turnOnlyStat, stat)) {
                return false;
            }
        }

        cauto& stat = pathStats[intervalIdx];
        int intervalTurn = stat.turn_;
        if (stat.usedMacro_ && curRange != interval) {
            intervalTurn += CalcMacroUpdateExtra(baseMacro, curRange, interval);
            curRange = interval;
        }

        totalTurn += intervalTurn + 1;
        totalStep += stat.step_ + 1;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        lastEndDir = stat.endDir_;
        lastTargetCell = targetCell;
        lastPrevTargetCell = prevTargetCell;
        lastPrevEndDir = prevPathStats[intervalIdx].endDir_;
        prefixTurns[intervalIdx + 1] = totalTurn;
        prefixSteps[intervalIdx + 1] = totalStep;
        prefixRanges[intervalIdx + 1] = curRange;
    }

    usedRange = curRange;
    return true;
}

template <class PATH_STATS>
double CalcEvalPenalty(int totalStep, const PATH_STATS& pathStats) {
    double p = 0;

    p += totalStep * HP.R_EvalStep;

    int maxTurn = 0;
    double intervalTurnVariance = 0.0;
    {
        double intervalTurnSum = 0;
        double intervalTurnSum2 = 0;

        REP(i, pathStats.size()) {
            int turn = pathStats[i].turn_;
            chmax(maxTurn, turn);
            intervalTurnSum += turn;
            intervalTurnSum2 += (double)turn * turn;
        }

        double div = 1.0 / (double)(2 * M);
        double mean = intervalTurnSum * div;
        intervalTurnVariance = intervalTurnSum2 * div - square(mean);
    }

    p += maxTurn * HP.R_EvalMaxIntervalTurn;

    p += intervalTurnVariance * HP.R_EvalIntervalVariance;

    return p;
}

double Eval(int totalTurn, int totalStep, const SA4Macro& baseMacro, const MArr<u8>& order,
            const CapArr<SA4IntervalPathStat, M_max * 2>& intervalPathStats, double timeRate) {
    double e = -totalTurn;
    if (timeRate < HP.R_PenaltyTime) {
        e -= CalcEvalPenalty(totalStep, intervalPathStats) * (HP.R_PenaltyTime - timeRate) / HP.R_PenaltyTime;
    }
    return e;
}

int CalcTurnOnlyStepLowerBound(const CapArr<MacroRange, M_max * 2>& intervalsRange, const MArr<u8>& order,
                               const CapArr<SA4TurnOnlyPathStat, M_max * 2>& turnOnlyPathStats) {
    VASSERT((int)intervalsRange.size() == (int)turnOnlyPathStats.size());
    int stepLowerBound = 0;
    int curCell = 0;
    int curDir = 2;
    REP(intervalIdx, turnOnlyPathStats.size()) {
        int ball = order[intervalIdx >> 1];
        int targetCell = (intervalIdx & 1) ? ballToGoalCell_[ball] : initialBallCells_[ball];
        cauto& stat = turnOnlyPathStats[intervalIdx];

        stepLowerBound += CalcTurnOnlyIntervalStepLowerBound(intervalsRange[intervalIdx], stat) + 1;
        curCell = targetCell;
        curDir = stat.endDir_;
    }
    return stepLowerBound;
}

double EvalTurnOnlyUpperBound(int totalTurn, int stepLowerBound,
                              const CapArr<SA4TurnOnlyPathStat, M_max * 2>& turnOnlyPathStats, double timeRate) {
    double e = -totalTurn;
    if (timeRate < HP.R_PenaltyTime) {
        e -= CalcEvalPenalty(stepLowerBound, turnOnlyPathStats) * (HP.R_PenaltyTime - timeRate) / HP.R_PenaltyTime;
    }
    return e;
}

struct SAState {
    int rawScore_;
    double evalScore_;
    SA4Macro baseMacro_;
    SA4RunMacro runMacro_;
    CapArr<MacroRange, M_max * 2> intervalsRange_;  
    CapArr<SA4IntervalPathStat, M_max * 2> intervalPathStats_;
    CapArr<SA4MacroKey, M_max * 2> intervalMacroKeys_;
    CapArr<int, SA4IntervalMax + 1> prefixTurns_;
    CapArr<int, SA4IntervalMax + 1> prefixSteps_;
    CapArr<MacroRange, SA4IntervalMax + 1> prefixRanges_;
    MArr<u8> order_;
    int totalTurn_;
    int totalStep_;

    void SetScore(int totalTurn, int totalStep, double evalScore) {
        totalTurn_ = totalTurn;
        totalStep_ = totalStep;
        rawScore_ = -totalTurn_;
        evalScore_ = evalScore;
    }

    void RebuildPrefixes() {
        int intervalCount = M * 2;
        VASSERT((int)intervalsRange_.size() == intervalCount);
        VASSERT((int)intervalPathStats_.size() == intervalCount);
        prefixTurns_.resize(intervalCount + 1);
        prefixSteps_.resize(intervalCount + 1);
        prefixRanges_.resize(intervalCount + 1);

        int totalTurn = 0;
        int totalStep = 0;
        MacroRange curRange = {-1, -1};
        prefixTurns_[0] = totalTurn;
        prefixSteps_[0] = totalStep;
        prefixRanges_[0] = curRange;

        REP(intervalIdx, intervalCount) {
            cauto& stat = intervalPathStats_[intervalIdx];
            cauto& interval = intervalsRange_[intervalIdx];
            int intervalTurn = stat.turn_;
            if (stat.usedMacro_ && curRange != interval) {
                intervalTurn += CalcMacroUpdateExtra(baseMacro_, curRange, interval);
                curRange = interval;
            }
            totalTurn += intervalTurn + 1;
            totalStep += stat.step_ + 1;
            prefixTurns_[intervalIdx + 1] = totalTurn;
            prefixSteps_[intervalIdx + 1] = totalStep;
            prefixRanges_[intervalIdx + 1] = curRange;
        }

        VASSERT(totalTurn == totalTurn_);
        VASSERT(totalStep == totalStep_);
    }

    template <class MACRO>
    void Init(const MACRO& macro, const MArr<u8>& order) {
        VASSERT((int)macro.size() <= SA4MacroMax);
        bool assigned = AssignSA4Macro(baseMacro_, macro);
        VASSERT(assigned);
        bool builtRunMacro = runMacro_.BuildFrom(baseMacro_);
        VASSERT(builtRunMacro);
        intervalsRange_.assign(M * 2, MacroRange{(i8)0, (i8)macro.size()});

        VASSERT(IsValidOrder(order));
        order_ = order;
        MacroRange usedRange;
        bool ret = EvaluateFixedOrderWithCache(baseMacro_, intervalsRange_, order_, INT_MAX, totalTurn_, totalStep_,
                                               intervalPathStats_, intervalMacroKeys_, usedRange);
        if (!ret) {
            VABORT();
            return;
        }

        double evalScore = Eval(totalTurn_, totalStep_, baseMacro_, order_, intervalPathStats_, 0);

        SetScore(totalTurn_, totalStep_, evalScore);
        RebuildPrefixes();
    }
};

void Solve(ChronoTimer& timer, Result& result) {
    Rng rng;
    sa4MacroHasher_.Init(rng);
    sa4PathCachHasser_.Init(rng);
    sa4PathCache_.init(SA4PathCacheCapacityHint);
    sa4TurnOnlyPathCache_.init(SA4TurnOnlyPathCacheCapacityHint);
    sa4TurnOnlyPathHotCache_.Clear();
    sa4MacroCache_.init(SA4MacroCacheCapacity);
    InitSA4NoMacroTurns();

    auto FlipLR = [](char c) -> char {
        if (c == 'L') {
            return 'R';
        }
        if (c == 'R') {
            return 'L';
        }
        return c;
    };

    auto FindRandomF = [&](const SA4RunMacro& runMacro) -> int {
        if (!runMacro.HasF()) {
            return -1;
        }
        int runIdx = runMacro.fRuns_[rng(runMacro.fRuns_.size())];
        cauto& run = runMacro.runs_[runIdx];
        return runMacro.FStart(runIdx) + rng(run.f_);
    };

    auto FindRandomTurn = [&](const SA4RunMacro& runMacro) -> int {
        if (!runMacro.HasLR()) {
            return -1;
        }
        int runIdx = runMacro.lrRuns_[rng(runMacro.lrRuns_.size())];
        return runMacro.TurnPos(runIdx);
    };

    static RandomTable transTbl;
    {
        transTbl.push(0, HP.R_TransRangeShrink);
        transTbl.push(1, HP.R_TransRangeExpand);

        transTbl.push(2, HP.R_TransMacroChange);        
        transTbl.push(3, HP.R_TransMacroAddSide);       
        transTbl.push(4, HP.R_TransMacroRemoveSide);    
        transTbl.push(14, HP.R_TransMacroAddRand);      
        transTbl.push(15, HP.R_TransMacroRemoveRand);   
        transTbl.push(13, HP.R_TransMacroRotate);       
        transTbl.push(12, HP.R_TransMacroSwap);         
        transTbl.push(6, HP.R_TransMacroLRFlip);        
        transTbl.push(7, HP.R_TransMacroFowardExpand);  
        transTbl.push(8, HP.R_TransMacroFowardShrink);  

        transTbl.push(5, HP.R_TransBallInsert);       
        transTbl.push(16, HP.R_TransBallSwap);        
        transTbl.push(10, HP.R_TransBallNearInsert);  
        transTbl.push(11, HP.R_TransBallNearSwap);    
    }

    static RandomTable baseCands;
    baseCands.push('F', HP.R_ChoiceFowardRate);
    baseCands.push('L', HP.R_ChoiceRotateRate);
    baseCands.push('R', HP.R_ChoiceRotateRate);

    CapArr<SA4IntervalPathStat, M_max * 2> tmpNewPathStats;
    CapArr<SA4MacroKey, M_max * 2> tmpNewMacroKeys;
    CapArr<SA4TurnOnlyPathStat, M_max * 2> tmpTurnOnlyPathStats;
    CapArr<SA4MacroKey, M_max * 2> tmpTurnOnlyMacroKeys;
    CapArr<int, SA4IntervalMax + 1> tmpNewPrefixTurns;
    CapArr<int, SA4IntervalMax + 1> tmpNewPrefixSteps;
    CapArr<MacroRange, SA4IntervalMax + 1> tmpNewPrefixRanges;
    SA4Macro tmpCutBaseMacro;
    CapArr<MacroRange, M_max * 2> tmpCutIntervalsRange;
    CapArr<SA4MacroKey, M_max * 2> tmpCutMacroKeys;
    CapArr<MacroRange, SA4IntervalMax + 1> tmpCutPrefixRanges;

    auto Trans = [&](SAState& state, SAChecker& checker, double timeRate, int& transIdx) -> SAResult {
        auto TryUpdateScore = [&](SAState& state, const SA4Macro& newBaseMacro,
                                  const CapArr<MacroRange, M_max * 2>& newIntervalsRange, const MArr<u8>& newOrder,
                                  int dirtyStart) -> SAResult {
            if ((int)newBaseMacro.size() > SA4MacroMax) {
                return SAResult::Fail;
            }
            int intervalCount = M * 2;
            dirtyStart = clamp(dirtyStart, 0, intervalCount);
            double acceptScore = checker.GetAcceptScore(rng);
            int limitTurn = int(-acceptScore) + 1;

            int newTotalTurn;
            int newTotalStep;
            auto& newPathStats = tmpNewPathStats;
            auto& newMacroKeys = tmpNewMacroKeys;
            auto& turnOnlyPathStats = tmpTurnOnlyPathStats;
            auto& turnOnlyMacroKeys = tmpTurnOnlyMacroKeys;
            auto& newPrefixTurns = tmpNewPrefixTurns;
            auto& newPrefixSteps = tmpNewPrefixSteps;
            auto& newPrefixRanges = tmpNewPrefixRanges;
            MacroRange newUsedRange;
            int turnOnlyTotalTurn;

            if (!EvaluateFixedOrderTurnOnlyWithCache(
                    newBaseMacro, newIntervalsRange, newOrder, limitTurn, turnOnlyTotalTurn, turnOnlyPathStats,
                    turnOnlyMacroKeys, state.baseMacro_, state.intervalsRange_, state.order_, state.intervalPathStats_,
                    state.intervalMacroKeys_, state.prefixTurns_, state.prefixRanges_, dirtyStart)) {
                return SAResult::Ng;
            }


            {
                int stepLowerBound = CalcTurnOnlyStepLowerBound(newIntervalsRange, newOrder, turnOnlyPathStats);
                double turnOnlyEvalUpperBound =
                    EvalTurnOnlyUpperBound(turnOnlyTotalTurn, stepLowerBound, turnOnlyPathStats, timeRate);
                if (turnOnlyEvalUpperBound < acceptScore) {
                    return SAResult::Ng;
                }
            }

            bool ret = EvaluateFixedOrderWithCache(
                newBaseMacro, newIntervalsRange, newOrder, limitTurn, newTotalTurn, newTotalStep, newPathStats,
                newMacroKeys, newUsedRange, state.baseMacro_, state.intervalsRange_, state.order_,
                state.intervalPathStats_, state.intervalMacroKeys_, turnOnlyPathStats, turnOnlyMacroKeys,
                state.prefixTurns_, state.prefixSteps_, state.prefixRanges_, dirtyStart, newPrefixTurns, newPrefixSteps,
                newPrefixRanges);
            if (!ret) {
                return SAResult::Ng;
            }
            VASSERT(newTotalTurn == turnOnlyTotalTurn);

            double newEvalScore = Eval(newTotalTurn, newTotalStep, newBaseMacro, newOrder, newPathStats, timeRate);
            if (!checker.CheckAccept(newEvalScore, rng)) {
                return SAResult::Ng;
            }

            const SA4Macro* commitBaseMacro = &newBaseMacro;
            const CapArr<MacroRange, M_max * 2>* commitIntervalsRange = &newIntervalsRange;
            const CapArr<SA4MacroKey, M_max * 2>* commitMacroKeys = &newMacroKeys;
            const CapArr<MacroRange, SA4IntervalMax + 1>* commitPrefixRanges = &newPrefixRanges;
            auto& cutBaseMacro = tmpCutBaseMacro;
            auto& cutIntervalsRange = tmpCutIntervalsRange;
            auto& cutMacroKeys = tmpCutMacroKeys;
            auto& cutPrefixRanges = tmpCutPrefixRanges;

            if (newUsedRange.l_ >= 0 && newUsedRange.l_ < newUsedRange.r_) {
                int cutL = newUsedRange.l_;
                int cutR = newUsedRange.r_;
                cutIntervalsRange = newIntervalsRange;

                REP(i, cutIntervalsRange.size()) {
                    cauto& oldRange = newIntervalsRange[i];
                    int l = max((int)oldRange.l_, cutL) - cutL;
                    int r = min((int)oldRange.r_, cutR) - cutL;
                    cutIntervalsRange[i] = MacroRange{(i8)l, (i8)r};
                }

                bool builtCutMacro = BuildSA4MacroRange(newBaseMacro, MacroRange{(i8)cutL, (i8)cutR}, cutBaseMacro);
                VASSERT(builtCutMacro);
                VASSERT(IsValidRanges(cutBaseMacro, cutIntervalsRange));
                BuildSA4IntervalMacroKeys(cutBaseMacro, cutIntervalsRange, M * 2, cutMacroKeys);
                cutPrefixRanges.resize(M * 2 + 1);
                REP(i, newPrefixRanges.size()) {
                    cauto& range = newPrefixRanges[i];
                    if (range.l_ < 0) {
                        cutPrefixRanges[i] = MacroRange{(i8)-1, (i8)-1};
                    }
                    else {
                        int l = max((int)range.l_, cutL) - cutL;
                        int r = min((int)range.r_, cutR) - cutL;
                        cutPrefixRanges[i] = MacroRange{(i8)l, (i8)r};
                    }
                }
                commitBaseMacro = &cutBaseMacro;
                commitIntervalsRange = &cutIntervalsRange;
                commitMacroKeys = &cutMacroKeys;
                commitPrefixRanges = &cutPrefixRanges;
            }

            state.SetScore(newTotalTurn, newTotalStep, newEvalScore);
            state.baseMacro_ = *commitBaseMacro;
            bool builtRunMacro = state.runMacro_.BuildFrom(state.baseMacro_);
            VASSERT(builtRunMacro);
            state.intervalsRange_ = *commitIntervalsRange;
            state.intervalPathStats_ = newPathStats;
            state.intervalMacroKeys_ = *commitMacroKeys;
            state.order_ = newOrder;
            state.prefixTurns_ = newPrefixTurns;
            state.prefixSteps_ = newPrefixSteps;
            state.prefixRanges_ = *commitPrefixRanges;
            VASSERT(state.prefixTurns_.back() == state.totalTurn_);
            VASSERT(state.prefixSteps_.back() == state.totalStep_);
            return SAResult::Ok;
        };

        auto DirtyStartFromOrderIndex = [](int orderIdx) { return clamp(orderIdx * 2, 0, M * 2); };
        auto FirstDifferentRange = [&](const CapArr<MacroRange, M_max * 2>& ranges) {
            REP(intervalIdx, M * 2) {
                if (ranges[intervalIdx] != state.intervalsRange_[intervalIdx]) {
                    return intervalIdx;
                }
            }
            return M * 2;
        };
        auto DirtyStartFromMacroPos = [&](const CapArr<MacroRange, M_max * 2>& ranges, int pos) {
            REP(intervalIdx, M * 2) {
                cauto& range = ranges[intervalIdx];
                if (range.l_ <= pos && pos < range.r_) {
                    return intervalIdx;
                }
            }
            return M * 2;
        };

        transIdx = transTbl(rng);

        if (transIdx == 0) {
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int intervalIdx = rng(M * 2);

            auto CanApplyRangeMove = [&](int c) -> bool {
                cauto& interval = newIntervalsRange[intervalIdx];
                if (c == 1) {
                    return interval.l_ < interval.r_;
                }
                if (c == 2) {
                    return interval.l_ < interval.r_;
                }
                VABORT();
                return false;
            };

            auto ApplyRangeMove = [&](CapArr<MacroRange, M_max * 2>& ranges, int c) {
                auto& interval = ranges[intervalIdx];
                if (c == 1) {
                    ++interval.l_;
                    int newL = interval.l_;
                    RREP(i, intervalIdx) {
                        if (ranges[i].l_ < newL) {
                            ranges[i].l_ = newL;
                            if (ranges[i].r_ < newL) {
                                ranges[i].r_ = newL;
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
                else if (c == 2) {
                    --interval.r_;
                    int newR = interval.r_;
                    RREP(i, intervalIdx) {
                        if (ranges[i].r_ > newR) {
                            ranges[i].r_ = newR;
                            if (ranges[i].l_ > newR) {
                                ranges[i].l_ = newR;
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
                else {
                    VABORT();
                }
                VASSERT(IsValidRanges(state.baseMacro_, ranges));
            };

            CapArr<int, 2> cands;
            for (char c : {1, 2}) {
                if (CanApplyRangeMove(c)) {
                    cands.push(c);
                }
            }
            if (cands.empty()) {
                return SAResult::Fail;
            }

            int c = cands.choice(rng);
            ApplyRangeMove(newIntervalsRange, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, state.baseMacro_, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 1) {
            int baseMacroSize = (int)state.baseMacro_.size();
            CapArr<int, M_max * 2> candIntervals;
            REP(i, M * 2) {
                cauto& range = state.intervalsRange_[i];
                if (range.l_ > 0 || range.r_ < baseMacroSize) {
                    candIntervals.push(i);
                }
            }
            if (candIntervals.empty()) {
                return SAResult::Fail;
            }

            int intervalIdx = candIntervals.choice(rng);
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            auto CanApplyRangeMove = [&](int c) -> bool {
                cauto& interval = newIntervalsRange[intervalIdx];
                if (c == 0) {
                    return interval.l_ > 0;
                }
                if (c == 3) {
                    return interval.r_ < (int)state.baseMacro_.size();
                }
                VABORT();
                return false;
            };

            auto ApplyRangeMove = [&](CapArr<MacroRange, M_max * 2>& ranges, int c) {
                auto& interval = ranges[intervalIdx];
                if (c == 0) {
                    --interval.l_;
                    int newL = interval.l_;
                    FOR(i, intervalIdx + 1, ranges.size()) {
                        if (ranges[i].l_ > newL) {
                            ranges[i].l_ = newL;
                        }
                        else {
                            break;
                        }
                    }
                }
                else if (c == 3) {
                    ++interval.r_;
                    int newR = interval.r_;
                    FOR(i, intervalIdx + 1, ranges.size()) {
                        if (ranges[i].r_ < newR) {
                            ranges[i].r_ = newR;
                        }
                        else {
                            break;
                        }
                    }
                }
                else {
                    VABORT();
                }
                VASSERT(IsValidRanges(state.baseMacro_, ranges));
            };

            CapArr<int, 2> cands;
            for (char c : {0, 3}) {
                if (CanApplyRangeMove(c)) {
                    cands.push(c);
                }
            }
            if (cands.empty()) {
                return SAResult::Fail;
            }

            int c = cands.choice(rng);
            ApplyRangeMove(newIntervalsRange, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, state.baseMacro_, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 2) {
            SA4Macro newMacro = state.baseMacro_;

            int j = rng(newMacro.size());
            static dvec<char> cands;
            cands.clear();
            for (char c : baseCands.table_) {
                if (newMacro[j] != c) {
                    cands.push(c);
                }
            }
            newMacro[j] = cands.choice(rng);

            int dirtyStart = DirtyStartFromMacroPos(state.intervalsRange_, j);
            SAResult result = TryUpdateScore(state, newMacro, state.intervalsRange_, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 3) {
            if ((int)state.baseMacro_.size() >= SA4MacroMax) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(2) ? 0 : newMacro.size();
            char c = baseCands(rng);
            InsertBaseMacroCommand(newMacro, newIntervalsRange, j, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 4) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(2) ? 0 : (newMacro.size() - 1);
            if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 14) {
            if ((int)state.baseMacro_.size() >= SA4MacroMax) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(newMacro.size() + 1);
            char c = baseCands(rng);
            InsertBaseMacroCommand(newMacro, newIntervalsRange, j, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 15) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(newMacro.size());
            if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 13) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            if (rng(2)) {
                char c = newMacro[0];
                if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, 0)) {
                    return SAResult::Fail;
                }
                InsertBaseMacroCommand(newMacro, newIntervalsRange, newMacro.size(), c);
            }
            else {
                char c = newMacro.back();
                if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, newMacro.size() - 1)) {
                    return SAResult::Fail;
                }
                InsertBaseMacroCommand(newMacro, newIntervalsRange, 0, c);
            }

            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, 0);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 12) {
            if (state.baseMacro_.size() < 2) {
                return SAResult::Fail;
            }
            SA4Macro newMacro = state.baseMacro_;

            int a = rng(newMacro.size());
            char baseChar = newMacro[a];

            CapArr<int, SA4MacroMax> cands;
            REP(i, newMacro.size()) {
                if (newMacro[i] != baseChar) {
                    cands.push(i);
                }
            }
            if (cands.empty()) {
                return SAResult::Fail;
            }

            int b = cands.choice(rng);

            VASSERT(newMacro[a] != newMacro[b]);
            swap(newMacro[a], newMacro[b]);

            int dirtyStart =
                min(DirtyStartFromMacroPos(state.intervalsRange_, a), DirtyStartFromMacroPos(state.intervalsRange_, b));
            SAResult result = TryUpdateScore(state, newMacro, state.intervalsRange_, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 6) {
            SA4Macro newMacro = state.baseMacro_;
            int j = FindRandomTurn(state.runMacro_);
            if (j < 0) {
                return SAResult::Fail;
            }
            newMacro[j] = FlipLR(newMacro[j]);

            int dirtyStart = DirtyStartFromMacroPos(state.intervalsRange_, j);
            SAResult result = TryUpdateScore(state, newMacro, state.intervalsRange_, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 7) {
            if ((int)state.baseMacro_.size() >= SA4MacroMax) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;
            int j = FindRandomF(state.runMacro_);
            if (j < 0) {
                return SAResult::Fail;
            }
            if (rng(2) == 0) {
                ++j;
            }
            InsertBaseMacroCommand(newMacro, newIntervalsRange, j, 'F');

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 8) {
            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;
            int j = FindRandomF(state.runMacro_);
            if (j < 0 || !EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 5) {
            MArr<u8> newOrder = state.order_;
            int a = rng(M);
            int b = rng(M - 1);
            if (b >= a) {
                ++b;
            }

            u8 ball = newOrder[a];
            newOrder.remove(a);
            newOrder.insert(b, ball);

            int dirtyStart = DirtyStartFromOrderIndex(min(a, b));
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 16) {
            if (M <= 1) {
                return SAResult::Fail;
            }
            MArr<u8> newOrder = state.order_;
            int a = rng(M);
            int b = rng(M - 1);
            if (b >= a) {
                ++b;
            }
            swap(newOrder[a], newOrder[b]);

            int dirtyStart = DirtyStartFromOrderIndex(min(a, b));
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 10) {
            if (M <= 1) {
                return SAResult::Fail;
            }
            MArr<u8> newOrder = state.order_;
            int a = rng(M);
            int maxDist = min(4, M - 1);
            int dist = 1 + rng(maxDist);
            if (rng(2) == 0) {
                dist = -dist;
            }
            int b = a + dist;
            if (b < 0 || b >= M) {
                b = a - dist;
            }
            if (b < 0 || b >= M || b == a) {
                return SAResult::Fail;
            }

            u8 ball = newOrder[a];
            newOrder.remove(a);
            newOrder.insert(b, ball);

            int dirtyStart = DirtyStartFromOrderIndex(min(a, b));
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        else if (transIdx == 11) {
            if (M <= 1) {
                return SAResult::Fail;
            }
            MArr<u8> newOrder = state.order_;
            int a = rng(M - 1);
            swap(newOrder[a], newOrder[a + 1]);

            int dirtyStart = DirtyStartFromOrderIndex(a);
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }

        VABORT();
        return SAResult::Fail;
    };

    static EasySA<SAState> sa;
    sa.updateTmpMask_ = (1 << HP.UpdateTmpMaskBit) - 1;
    sa.tempType_ = HP.TempType;
    sa.tempPow_ = HP.Type2_TempPow;
    sa.rollbackStartTimeRate_ = HP.RollbackStartRate;
    sa.rollbackCount_ = HP.RollbackCount;
    sa.rollbackMul_ = HP.RollbackNextMulti;
    sa.minRollbackCount_ = HP.MinRollbackCount;
    sa.filterStartTime_ = HP.FilterStartTime;
    sa.filterEndTime_ = HP.FilterEndTime;

    double startTemp;
    double endTemp;
    if (HP.TempType == 0) {
        startTemp = HP.Type0_StartTemp;
        endTemp = HP.Type0_EndTemp;
    }
    else if (HP.TempType == 1) {
        startTemp = HP.Type1_StartTemp;
        endTemp = HP.Type1_EndTemp;
    }
    else {
        startTemp = HP.Type2_StartTemp;
        endTemp = HP.Type2_EndTemp;
    }

    static dvec<SAState> states(HP.InitialStateCount);
    {
        struct TurnCand {
            SA4Macro macro_;
            MArr<u8> order_;
            greedy2b::Greedy2BEndDirs endDirs_;
            int turn_;
        };

        int totalTurn;
        int totalStep;
        MArr<u8> order;
        greedy2b::Greedy2BEndDirs endDirs;
        MacroCache macroCache;
        static vector<TurnCand> turnCands;
        turnCands.clear();
        turnCands.reserve(MacroTable.size());

        for (cauto& macroSrc : MacroTable) {
            SA4Macro macro;
            if (!AssignSA4Macro(macro, macroSrc)) {
                continue;
            }
            macroCache.Init(macro);
            if (greedy2b::CalcTotalTurn(macroCache, INT_MAX, totalTurn, &order, &endDirs)) {
                auto& c = turnCands.emplace_back();
                c.macro_ = macro;
                c.order_ = order;
                c.endDirs_ = endDirs;
                c.turn_ = totalTurn;
            }
        }

        stable_sort(ALL(turnCands), [](cauto& lhs, cauto& rhs) { return lhs.turn_ < rhs.turn_; });

        int stateCount = 0;
        REP(ci, turnCands.size()) {
            cauto& turnCand = turnCands[ci];
            macroCache.Init(turnCand.macro_);
            int limitTurn = turnCand.turn_ == INT_MAX ? INT_MAX : turnCand.turn_ + 1;
            if (greedy2b::SolveWithOrder(macroCache, limitTurn, turnCand.order_, turnCand.endDirs_, totalTurn,
                                         totalStep, nullptr)) {
                VASSERT(totalTurn == turnCand.turn_);
                states[stateCount].Init(turnCand.macro_, turnCand.order_);
                ++stateCount;
            }

            if (stateCount >= HP.InitialStateCount) {
                break;
            }
        }

        VASSERT(stateCount > 0);
        if (stateCount == 0) {
            VABORT();
        }
        int validStateCount = stateCount;
        FOR(i, stateCount, states.size()) {
            states[i] = states[i % validStateCount];
        }

    }

    sa.Run(timer, states, startTemp, endTemp, Trans);

    cauto& best = sa.GetBest();
    int totalTurn = best.totalTurn_;
    int totalStep = best.totalStep_;
    int dpTurn = 0;
    int dpStep = 0;
    static dvec<char> dpCommands;
    bool dpOk =
        SolveFixedOrderDP(best.baseMacro_, best.intervalsRange_, best.order_, INT_MAX, dpTurn, dpStep, &dpCommands);

    static dvec<char> fixedStatsCommands;
    bool fixedStatsOk =
        BuildFixedOrderFromPathStats(best.baseMacro_, best.intervalsRange_, best.order_, best.intervalPathStats_,
                                     best.totalTurn_, best.totalStep_, fixedStatsCommands);

    if (dpOk) {
        result.commands_ = dpCommands;
        totalTurn = dpTurn;
        totalStep = dpStep;
    }
    if (fixedStatsOk && (!dpOk || (int)fixedStatsCommands.size() < (int)result.commands_.size())) {
        result.commands_ = fixedStatsCommands;
        totalTurn = best.totalTurn_;
        totalStep = best.totalStep_;
    }
    if (!dpOk && !fixedStatsOk) {
        VABORT();
    }


}

}  

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
struct SizeFilter2 {
    int width_;
    SizeFilterSegTree seg_;
    dvec<double> scores_;
    dvec<T> datas_;

    void Reserve(int capacity) {
        scores_.reserve(capacity);
        datas_.reserve(capacity);
        seg_.buf_.reserve(capacity);
    }

    void Init(int width) {
        width_ = width;
        scores_.clear();
        datas_.clear();
        scores_.reserve(width);
        datas_.reserve(width);
        seg_.Init(width);
    }

    void Clear() {
        scores_.clear();
        datas_.clear();
        seg_.Init(width_);
    }

    bool CanPush(double evalScore) const {
        if (scores_.size() == width_) {
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
        if (scores_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return nullptr;
            }

            int index = minData.index;
            scores_[index] = evalScore;
            seg_.Set(evalScore, index);
            return &datas_[index];
        }
        else {
            int index = scores_.size();
            scores_.push(evalScore);
            auto& ret = datas_.pushR();

            if (scores_.size() == width_) {
                REP(index, scores_.size()) {
                    seg_.InitSet(scores_[index], index);
                }
                seg_.InitUpdate(width_);
            }
            return &ret;
        }
        return nullptr;
    }

    T* TryPush(double evalScore, bool& reused) {
        reused = false;
        if (scores_.size() == width_) {
            cauto& minData = seg_.GetMin();
            if (evalScore <= minData.evalScore) {
                return nullptr;
            }

            int index = minData.index;
            scores_[index] = evalScore;
            seg_.Set(evalScore, index);
            reused = true;
            return &datas_[index];
        }
        else {
            int index = scores_.size();
            scores_.push(evalScore);
            auto& ret = datas_.pushR();

            if (scores_.size() == width_) {
                REP(index, scores_.size()) {
                    seg_.InitSet(scores_[index], index);
                }
                seg_.InitUpdate(width_);
            }
            return &ret;
        }
        return nullptr;
    }
};


namespace sa4s {

constexpr int SA4MacroMax = 64;
using SA4Macro = CapArr<char, SA4MacroMax>;

struct MacroRange {
    int l_;
    int r_;

    bool operator==(const MacroRange& r) const {
        return l_ == r.l_ && r_ == r.r_;
    }
    bool operator!=(const MacroRange& r) const {
        return l_ != r.l_ || r_ != r.r_;
    }
};

template <class SRC>
bool AssignSA4Macro(SA4Macro& dst, const SRC& src) {
    int n = (int)src.size();
    if (n > SA4MacroMax) {
        return false;
    }
    dst.resize(n);
    REP(i, n) {
        dst[i] = src[i];
    }
    return true;
}

SA4Macro MakeSA4Macro(const char* src) {
    SA4Macro macro;
    for (int i = 0; src[i] != 0; ++i) {
        VASSERT(i < SA4MacroMax);
        macro.push(src[i]);
    }
    return macro;
}

FindSegPathUniversalCache& GetSA4SFindSegPathCache() {
    static FindSegPathUniversalCache cache;
    return cache;
}

void InitSA4SFindSegPathCache() {
    auto& cache = GetSA4SFindSegPathCache();
    cache.statCache_.init(FindSegPathUniversalStatCacheCapacityHint);

}

template <int CAP>
bool BuildSA4MacroRange(const SA4Macro& baseMacro, const MacroRange& range, CapArr<char, CAP>& macro) {
    if (range.l_ < 0 || range.l_ > range.r_ || range.r_ > (int)baseMacro.size()) {
        return false;
    }
    int len = range.r_ - range.l_;
    if (len > macro.capacity()) {
        return false;
    }

    macro.resize(len);
    REP(i, len) {
        macro[i] = baseMacro[range.l_ + i];
    }
    return true;
}

bool IsValidRanges(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& ranges) {
    int n = (int)baseMacro.size();
    for (cauto& range : ranges) {
        if (range.l_ < 0 || range.l_ > range.r_ || range.r_ > n) {
            return false;
        }
    }
    FOR(i, 1, ranges.size()) {
        if (ranges[i].l_ > ranges[i - 1].l_ || ranges[i].r_ < ranges[i - 1].r_) {
            return false;
        }
    }
    return true;
}

void NormalizeRangesToBase(const SA4Macro& baseMacro, CapArr<MacroRange, M_max * 2>& ranges) {
    int n = (int)baseMacro.size();
    for (auto& range : ranges) {
        if (range.l_ < 0) {
            range.l_ = 0;
        }
        if (range.r_ < 0) {
            range.r_ = 0;
        }
        if (range.l_ > n) {
            range.l_ = n;
        }
        if (range.r_ > n) {
            range.r_ = n;
        }
        if (range.l_ > range.r_) {
            range.l_ = range.r_;
        }
    }
    FOR(i, 1, ranges.size()) {
        if (ranges[i].l_ > ranges[i - 1].l_) {
            ranges[i].l_ = ranges[i - 1].l_;
        }
        if (ranges[i].r_ < ranges[i - 1].r_) {
            ranges[i].r_ = ranges[i - 1].r_;
        }
    }
    VASSERT(IsValidRanges(baseMacro, ranges));
}

void InsertBaseMacroCommand(SA4Macro& baseMacro, CapArr<MacroRange, M_max * 2>& ranges, int pos, char command) {
    VASSERT(0 <= pos && pos <= (int)baseMacro.size());
    baseMacro.insert(pos, command);

    REP(k, ranges.size()) {
        auto& range = ranges[k];
        if (pos < range.l_) {
            ++range.l_;
            ++range.r_;
        }
        else if (range.l_ <= pos && pos <= range.r_) {
            ++range.r_;
        }
    }
    NormalizeRangesToBase(baseMacro, ranges);
}

bool EraseBaseMacroCommand(SA4Macro& baseMacro, CapArr<MacroRange, M_max * 2>& ranges, int pos) {
    if ((int)baseMacro.size() <= 1) {
        return false;
    }
    VASSERT(0 <= pos && pos < (int)baseMacro.size());
    baseMacro.remove(pos);

    REP(k, ranges.size()) {
        auto& range = ranges[k];
        if (pos < range.l_) {
            --range.l_;
            --range.r_;
        }
        else if (range.l_ <= pos && pos < range.r_) {
            --range.r_;
        }
    }
    NormalizeRangesToBase(baseMacro, ranges);
    return true;
}

bool IsValidOrder(const MArr<u8>& order) {
    if ((int)order.size() != M) {
        return false;
    }

    MArr<bool> used(M, false);
    for (u8 ball : order) {
        if ((int)ball >= M || used[ball]) {
            return false;
        }
        used[ball] = true;
    }
    return true;
}

template <class MACRO>
int CountMacroS(const MACRO& macro) {
    int count = 0;
    for (char c : macro) {
        if (c == 'S') {
            ++count;
        }
    }
    return count;
}

constexpr int SA4InvalidPrev = 255;
constexpr int SA4DPInf = INT_MAX / 4;
constexpr int SA4SegmentDPBeamWidth = 8192;

struct SA4MacroKey {
    u8 len_ = 0;
    u64 hash_ = 0;

    bool operator==(const SA4MacroKey& other) const {
        return len_ == other.len_ && hash_ == other.hash_;
    }
    bool operator!=(const SA4MacroKey& other) const {
        return !(*this == other);
    }
};

struct SA4Cost {
    int turn_ = SA4DPInf;
    int step_ = SA4DPInf;
};

struct SA4IntervalPathStat {
    int turn_ = SA4DPInf;
    int step_ = SA4DPInf;
    u8 endDir_ = 0;
    u8 usedMacro_ = 0;
    int endIdx_ = -1;
};

u64 SA4SplitMix64(u64 x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

u64 SA4HashToken(int place, int kind) {
    u64 x = 0x6a09e667f3bcc909ULL;
    x ^= (u64)(place + 1) * 0x9e3779b97f4a7c15ULL;
    x ^= (u64)(kind + 1) * 0xbf58476d1ce4e5b9ULL;
    return SA4SplitMix64(x);
}

int SA4MacroCommandKind(char c) {
    if (c == 'F') {
        return 0;
    }
    if (c == 'L') {
        return 1;
    }
    if (c == 'R') {
        return 2;
    }
    VASSERT(c == 'S');
    return 3;
}

struct SA4MacroHasher {
    array<array<u64, 4>, SA4MacroMax> charHash_;
    array<u64, SA4MacroMax + 1> lenHash_;
    array<array<u64, SA4MacroMax + 1>, SA4MacroMax + 1> baseMacroRangeXor_;
    SA4Macro preparedBaseMacro_;
    bool prepared_ = false;

    SA4MacroHasher() {
        REP(i, SA4MacroMax) {
            REP(kind, 4) {
                charHash_[i][kind] = SA4HashToken(100 + i, kind);
            }
        }
        REP(len, SA4MacroMax + 1) {
            lenHash_[len] = SA4HashToken(200, len);
        }
    }

    void PrepareBaseMacro(const SA4Macro& baseMacro) {
        if (prepared_ && preparedBaseMacro_ == baseMacro) {
            return;
        }
        prepared_ = true;
        preparedBaseMacro_ = baseMacro;
        int n = (int)baseMacro.size();
        VASSERT(n <= SA4MacroMax);
        REP(start, n + 1) {
            baseMacroRangeXor_[start][0] = 0;
            int maxLen = min(SA4MacroMax, n - start);
            u64 h = 0;
            REP(i, maxLen) {
                h ^= charHash_[i][SA4MacroCommandKind(baseMacro[start + i])];
                baseMacroRangeXor_[start][i + 1] = h;
            }
        }
    }

    void CalcPreparedKey(const MacroRange& range, SA4MacroKey& key) {
        VASSERT(prepared_);
        VASSERT(0 <= range.l_ && range.l_ <= range.r_ && range.r_ <= (int)preparedBaseMacro_.size());
        int len = range.r_ - range.l_;
        VASSERT(len <= SA4MacroMax);
        key.len_ = (u8)len;
        key.hash_ = lenHash_[len] ^ baseMacroRangeXor_[range.l_][len];
    }

    void CalcKey(const SA4Macro& baseMacro, const MacroRange& range, SA4MacroKey& key) {
        PrepareBaseMacro(baseMacro);
        CalcPreparedKey(range, key);
    }
};

SA4MacroHasher& GetSA4MacroHasher() {
    static SA4MacroHasher hasher;
    return hasher;
}

bool IsBetterCost(int turn, int step, const SA4Cost& current) {
    if (turn != current.turn_) {
        return turn < current.turn_;
    }
    return step < current.step_;
}

bool SameMacroRangeContent(const SA4Macro& baseMacro, int start, const MacroRange& range) {
    int len = range.r_ - range.l_;
    if (len <= 0 || start < 0 || start + len > (int)baseMacro.size()) {
        return false;
    }
    REP(i, len) {
        if (baseMacro[start + i] != baseMacro[range.l_ + i]) {
            return false;
        }
    }
    return true;
}

int CalcMacroRecordingLen(const SA4Macro& baseMacro, const MacroRange& targetRange, const MacroRange* prevRange) {
    VASSERT(0 <= targetRange.l_ && targetRange.l_ <= targetRange.r_ && targetRange.r_ <= (int)baseMacro.size());

    int prevLen = prevRange == nullptr ? 0 : prevRange->r_ - prevRange->l_;
    int len = 0;
    for (int i = targetRange.l_; i < targetRange.r_;) {
        if (prevLen > 0 && i + prevLen <= targetRange.r_ && SameMacroRangeContent(baseMacro, i, *prevRange)) {
            ++len;
            i += prevLen;
        }
        else {
            ++len;
            ++i;
        }
    }
    return len;
}

void AppendMacroRecordingCommands(const SA4Macro& baseMacro, const MacroRange& targetRange, const MacroRange* prevRange,
                                  dvec<char>& result) {
    VASSERT(0 <= targetRange.l_ && targetRange.l_ <= targetRange.r_ && targetRange.r_ <= (int)baseMacro.size());

    int prevLen = prevRange == nullptr ? 0 : prevRange->r_ - prevRange->l_;
    for (int i = targetRange.l_; i < targetRange.r_;) {
        if (prevLen > 0 && i + prevLen <= targetRange.r_ && SameMacroRangeContent(baseMacro, i, *prevRange)) {
            result.push('P');
            i += prevLen;
        }
        else {
            result.push(baseMacro[i]);
            ++i;
        }
    }
}

int CalcMacroUpdateExtra(const SA4Macro& baseMacro, const MacroRange& prevRange, const MacroRange& targetRange) {
    if (prevRange == targetRange) {
        return 0;
    }

    const MacroRange* prevRangePtr = prevRange.l_ < 0 ? nullptr : &prevRange;
    return CalcMacroRecordingLen(baseMacro, targetRange, prevRangePtr) + 1;
}

int CalcMacroUpdateExtra(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange, int rangeState,
                         int intervalIdx, int noneRangeState) {
    cauto& interval = intervalsRange[intervalIdx];
    if (rangeState == noneRangeState) {
        return CalcMacroRecordingLen(baseMacro, interval, nullptr) + 1;
    }
    if (intervalsRange[rangeState] == interval) {
        return 0;
    }

    cauto& prevRange = intervalsRange[rangeState];
    int extra = CalcMacroRecordingLen(baseMacro, interval, &prevRange) + 1;
    VASSERT(extra >= 0);
    return extra;
}

bool AppendPathCommandsWithMacroUpdate(const SA4Macro& baseMacro, const MacroRange& prevRange,
                                       const MacroRange& targetRange, const dvec<char>& pathCommands,
                                       dvec<char>& result) {
    if (prevRange == targetRange) {
        result.extend(pathCommands);
        return true;
    }

    const MacroRange* prevRangePtr = prevRange.l_ < 0 ? nullptr : &prevRange;
    bool replaced = false;
    for (char c : pathCommands) {
        if (!replaced && c == 'P') {
            replaced = true;
            result.push('M');
            AppendMacroRecordingCommands(baseMacro, targetRange, prevRangePtr, result);
            result.push('M');
        }
        else {
            result.push(c);
        }
    }
    return replaced;
}

bool AppendPathCommandsWithMacroUpdate(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                                       int prevRangeState, int intervalIdx, int noneRangeState,
                                       const dvec<char>& pathCommands, dvec<char>& result) {
    cauto& interval = intervalsRange[intervalIdx];
    MacroRange prevRange = prevRangeState == noneRangeState ? MacroRange{-1, -1} : intervalsRange[prevRangeState];
    return AppendPathCommandsWithMacroUpdate(baseMacro, prevRange, interval, pathCommands, result);
}

void BuildSA4IntervalMacroKeys(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                               int intervalCount, CapArr<SA4MacroKey, M_max * 2>& macroKeys) {
    macroKeys.resize(intervalCount);
    auto& hasher = GetSA4MacroHasher();
    hasher.PrepareBaseMacro(baseMacro);
    REP(intervalIdx, intervalCount) {
        hasher.CalcPreparedKey(intervalsRange[intervalIdx], macroKeys[intervalIdx]);
    }
}

bool EvaluateFixedOrderSegmentsWithCache(
    const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange, const MArr<u8>& order,
    int limitTurn, int& totalTurn, int& totalStep, CapArr<SA4IntervalPathStat, M_max * 2>& pathStats,
    CapArr<SA4MacroKey, M_max * 2>& macroKeys, MacroRange& usedRange, const SA4Macro* prevBaseMacro = nullptr,
    const CapArr<MacroRange, M_max * 2>* prevIntervalsRange = nullptr, const MArr<u8>* prevOrder = nullptr,
    const CapArr<SA4IntervalPathStat, M_max * 2>* prevPathStats = nullptr,
    const CapArr<SA4MacroKey, M_max * 2>* prevMacroKeys = nullptr,
    const CapArr<int, M_max + 1>* prevPrefixTurns = nullptr, const CapArr<int, M_max + 1>* prevPrefixSteps = nullptr,
    const CapArr<MacroRange, M_max + 1>* prevPrefixRanges = nullptr, int dirtyStart = 0,
    CapArr<int, M_max + 1>* prefixTurns = nullptr, CapArr<int, M_max + 1>* prefixSteps = nullptr,
    CapArr<MacroRange, M_max + 1>* prefixRanges = nullptr) {
    if (!IsValidOrder(order)) {
        return false;
    }
    VASSERT(IsValidRanges(baseMacro, intervalsRange));
    if ((int)intervalsRange.size() != M) {
        return false;
    }

    pathStats.resize(M);
    macroKeys.resize(M);
    bool canReusePrev = prevBaseMacro && prevIntervalsRange && prevOrder && prevPathStats && prevMacroKeys &&
                        (int)prevIntervalsRange->size() == M && (int)prevOrder->size() == M &&
                        (int)prevPathStats->size() == M && (int)prevMacroKeys->size() == M;
    bool canUseDirtyPrefix = canReusePrev && prevPrefixTurns && prevPrefixSteps && prevPrefixRanges &&
                             (int)prevPrefixTurns->size() == M + 1 && (int)prevPrefixSteps->size() == M + 1 &&
                             (int)prevPrefixRanges->size() == M + 1;
    bool sameBaseAndOrder = canReusePrev && *prevBaseMacro == baseMacro && *prevOrder == order;
    dirtyStart = canUseDirtyPrefix ? clamp(dirtyStart, 0, M) : 0;
    if (prefixTurns) {
        prefixTurns->resize(M + 1);
    }
    if (prefixSteps) {
        prefixSteps->resize(M + 1);
    }
    if (prefixRanges) {
        prefixRanges->resize(M + 1);
    }

    static MArr<bool> ballCompleted;
    ballCompleted.assign(M, false);
    int curCell = 0;
    int curDir = 2;
    totalTurn = 0;
    totalStep = 0;
    MacroRange curRange = {-1, -1};
    if (canUseDirtyPrefix && dirtyStart > 0) {
        REP(segIdx, dirtyStart) {
            pathStats[segIdx] = (*prevPathStats)[segIdx];
            macroKeys[segIdx] = (*prevMacroKeys)[segIdx];
            ballCompleted[order[segIdx]] = true;
        }
        REP(i, dirtyStart + 1) {
            if (prefixTurns) {
                (*prefixTurns)[i] = (*prevPrefixTurns)[i];
            }
            if (prefixSteps) {
                (*prefixSteps)[i] = (*prevPrefixSteps)[i];
            }
            if (prefixRanges) {
                (*prefixRanges)[i] = (*prevPrefixRanges)[i];
            }
        }
        totalTurn = (*prevPrefixTurns)[dirtyStart];
        totalStep = (*prevPrefixSteps)[dirtyStart];
        curRange = (*prevPrefixRanges)[dirtyStart];
        int prevEndIdx = (*prevPathStats)[dirtyStart - 1].endIdx_;
        VASSERT(prevEndIdx >= 0);
        FromCellDirIdx(prevEndIdx, curCell, curDir);
    }
    else {
        if (prefixTurns) {
            (*prefixTurns)[0] = totalTurn;
        }
        if (prefixSteps) {
            (*prefixSteps)[0] = totalStep;
        }
        if (prefixRanges) {
            (*prefixRanges)[0] = curRange;
        }
    }
    usedRange = curRange;
    SA4Macro targetMacro;
    FindSegPathResult pathResult;
    auto& pathSegCache = GetSA4SFindSegPathCache();
    auto& macroHasher = GetSA4MacroHasher();
    macroHasher.PrepareBaseMacro(baseMacro);

    static array<int, M_max> prevStartCells;
    static array<int, M_max> prevStartDirs;
    static array<int, M_max> prevSegByBall;
    if (canReusePrev) {
        REP(i, M) {
            prevSegByBall[i] = -1;
        }
        int prevCell = 0;
        int prevDir = 2;
        REP(prevIdx, M) {
            prevStartCells[prevIdx] = prevCell;
            prevStartDirs[prevIdx] = prevDir;
            prevSegByBall[(*prevOrder)[prevIdx]] = prevIdx;
            int prevEndIdx = (*prevPathStats)[prevIdx].endIdx_;
            if (prevEndIdx >= 0) {
                FromCellDirIdx(prevEndIdx, prevCell, prevDir);
            }
        }
    }

    if (totalTurn >= limitTurn || totalStep > T) {
        return false;
    }

    FOR(segIdx, dirtyStart, M) {
        int targetBall = order[segIdx];
        cauto& interval = intervalsRange[segIdx];
        auto& macroKey = macroKeys[segIdx];
        if (sameBaseAndOrder && interval == (*prevIntervalsRange)[segIdx]) {
            macroKey = (*prevMacroKeys)[segIdx];
        }
        else {
            macroHasher.CalcPreparedKey(interval, macroKey);
        }

        bool reused = false;
        if (canReusePrev) {
            auto TryReusePrevStat = [&](int prevIdx) {
                if (prevIdx < 0) {
                    return false;
                }
                cauto& prevStat = (*prevPathStats)[prevIdx];
                if (prevStat.endIdx_ < 0) {
                    return false;
                }
                if ((*prevOrder)[prevIdx] != targetBall) {
                    return false;
                }
                if (prevStartCells[prevIdx] != curCell || prevStartDirs[prevIdx] != curDir) {
                    return false;
                }
                if (macroKey != (*prevMacroKeys)[prevIdx]) {
                    return false;
                }
                pathStats[segIdx] = prevStat;
                return true;
            };
            reused = TryReusePrevStat(segIdx);
            if (!reused) {
                reused = TryReusePrevStat(prevSegByBall[targetBall]);
            }
        }

        if (reused) {
            cauto& stat = pathStats[segIdx];
            pathResult.endIdx_ = stat.endIdx_;
            pathResult.turn_ = stat.turn_;
            pathResult.step_ = stat.step_;
            pathResult.usedMacro_ = stat.usedMacro_ != 0;
        }
        else {
            u64 pathCacheKey = CalcFindSegPathUniversalCacheKeyByMacroHash(curCell, curDir, targetBall,
                                                                           (int)macroKey.len_, macroKey.hash_);
            bool foundPath = false;
            if (!TryGetFindSegPathUniversalStatCache(pathSegCache, pathCacheKey, pathResult, foundPath)) {
                bool builtMacro = BuildSA4MacroRange(baseMacro, interval, targetMacro);
                VASSERT(builtMacro);
                if (!FindSegPathUniversalWithCacheKey(pathSegCache, curCell, curDir, targetBall, ballCompleted,
                                                      targetMacro, pathResult, false, pathCacheKey)) {
                    return false;
                }
            }
            else if (!foundPath) {
                return false;
            }
        }

        int segmentTurn = pathResult.turn_;
        if (pathResult.usedMacro_ && curRange != interval) {
            segmentTurn += CalcMacroUpdateExtra(baseMacro, curRange, interval);
            curRange = interval;
        }

        totalTurn += segmentTurn;
        totalStep += pathResult.step_;
        if (totalTurn >= limitTurn || totalStep > T) {
            return false;
        }

        int endCell;
        int endDir;
        FromCellDirIdx(pathResult.endIdx_, endCell, endDir);
        auto& stat = pathStats[segIdx];
        stat.turn_ = pathResult.turn_;
        stat.step_ = pathResult.step_;
        stat.endDir_ = (u8)endDir;
        stat.usedMacro_ = pathResult.usedMacro_ ? 1 : 0;
        stat.endIdx_ = pathResult.endIdx_;

        curCell = endCell;
        curDir = endDir;
        ballCompleted[targetBall] = true;
        if (prefixTurns) {
            (*prefixTurns)[segIdx + 1] = totalTurn;
        }
        if (prefixSteps) {
            (*prefixSteps)[segIdx + 1] = totalStep;
        }
        if (prefixRanges) {
            (*prefixRanges)[segIdx + 1] = curRange;
        }
    }

    usedRange = curRange;
    return true;
}

bool SolveFixedOrderSegments(const SA4Macro& baseMacro, const CapArr<MacroRange, M_max * 2>& intervalsRange,
                             const MArr<u8>& order, int limitTurn, int& totalTurn, int& totalStep, dvec<char>* result) {
    if (!IsValidOrder(order)) {
        return false;
    }
    VASSERT(IsValidRanges(baseMacro, intervalsRange));
    if ((int)intervalsRange.size() != M) {
        return false;
    }

    if (result) {
        result->clear();
    }

    int segCount = M;
    int publicCount = NN * 4;
    static array<MacroRange, M_max> uniqueRanges;
    static array<u8, M_max> intervalRangeState;
    int uniqueRangeCount = 0;
    REP(segIdx, segCount) {
        cauto& interval = intervalsRange[segIdx];
        int rangeState = -1;
        REP(i, uniqueRangeCount) {
            if (uniqueRanges[i] == interval) {
                rangeState = i;
                break;
            }
        }
        if (rangeState < 0) {
            rangeState = uniqueRangeCount;
            uniqueRanges[uniqueRangeCount++] = interval;
        }
        intervalRangeState[segIdx] = (u8)rangeState;
    }
    int noneRangeState = uniqueRangeCount;
    int rangeStateCount = uniqueRangeCount + 1;
    int stateCount = rangeStateCount * publicCount;
    int startPublic = ToCellDirIdx(0, 2);

    auto StateIdx = [&](int rangeState, int publicIdx) {
        VASSERT(0 <= rangeState && rangeState < rangeStateCount);
        VASSERT(0 <= publicIdx && publicIdx < publicCount);
        return rangeState * publicCount + publicIdx;
    };
    auto StateRange = [&](int stateIdx) {
        VASSERT(0 <= stateIdx && stateIdx < stateCount);
        return stateIdx / publicCount;
    };
    auto StatePublic = [&](int stateIdx) {
        VASSERT(0 <= stateIdx && stateIdx < stateCount);
        return stateIdx % publicCount;
    };

    static array<SA4Macro, M_max> segmentMacros;
    REP(segIdx, segCount) {
        cauto& interval = intervalsRange[segIdx];
        bool builtMacro = BuildSA4MacroRange(baseMacro, interval, segmentMacros[segIdx]);
        VASSERT(builtMacro);
        if (!builtMacro) {
            return false;
        }
    }
    static vector<int> rangeUpdateExtra;
    rangeUpdateExtra.assign(rangeStateCount * segCount, 0);
    REP(rangeState, rangeStateCount) {
        REP(segIdx, segCount) {
            int targetRangeState = intervalRangeState[segIdx];
            if (rangeState == targetRangeState) {
                continue;
            }
            const MacroRange* prevRangePtr = rangeState == noneRangeState ? nullptr : &uniqueRanges[rangeState];
            int extra = CalcMacroRecordingLen(baseMacro, intervalsRange[segIdx], prevRangePtr) + 1;
            VASSERT(extra >= 0);
            rangeUpdateExtra[rangeState * segCount + segIdx] = extra;
        }
    }
    auto AppendPathCommandsWithCompressedRange = [&](int prevRangeState, int intervalIdx,
                                                     const dvec<char>& pathCommands, dvec<char>& out) {
        MacroRange prevRange = prevRangeState == noneRangeState ? MacroRange{-1, -1} : uniqueRanges[prevRangeState];
        return AppendPathCommandsWithMacroUpdate(baseMacro, prevRange, intervalsRange[intervalIdx], pathCommands, out);
    };
    static dvec<char> baselineCommands;
    int baselineTurn = SA4DPInf;
    int baselineStep = SA4DPInf;
    bool hasBaseline = false;

    auto GreedyRestore = [&]() -> bool {
        if (result) {
            result->clear();
        }

        MArr<bool> ballCompleted(M, false);
        int curCell = 0;
        int curDir = 2;
        totalTurn = 0;
        totalStep = 0;
        MacroRange curRange = {-1, -1};
        static FindSegPathResult pathResult;
        auto& pathSegCache = GetSA4SFindSegPathCache();

        REP(segIdx, segCount) {
            int targetBall = order[segIdx];
            cauto& interval = intervalsRange[segIdx];

            bool foundPath = FindSegPathUniversalWithCache(pathSegCache, curCell, curDir, targetBall, ballCompleted,
                                                           segmentMacros[segIdx], pathResult, result != nullptr);
            if (!foundPath) {
                return false;
            }

            int segmentTurn = pathResult.turn_;
            if (pathResult.usedMacro_ && curRange != interval) {
                segmentTurn += CalcMacroUpdateExtra(baseMacro, curRange, interval);
            }

            totalTurn += segmentTurn;
            totalStep += pathResult.step_;
            if (totalTurn >= limitTurn || totalStep > T) {
                return false;
            }

            if (result) {
                if (pathResult.usedMacro_ && curRange != interval) {
                    if (!AppendPathCommandsWithMacroUpdate(baseMacro, curRange, interval, pathResult.commands_,
                                                           *result)) {
                        return false;
                    }
                }
                else {
                    result->extend(pathResult.commands_);
                }
            }

            if (pathResult.usedMacro_ && curRange != interval) {
                curRange = interval;
            }

            FromCellDirIdx(pathResult.endIdx_, curCell, curDir);
            ballCompleted[targetBall] = true;
        }

        return !result || (int)result->size() == totalTurn;
    };
    auto UseBaseline = [&]() -> bool {
        if (!hasBaseline) {
            return GreedyRestore();
        }
        totalTurn = baselineTurn;
        totalStep = baselineStep;
        if (result) {
            *result = baselineCommands;
        }
        return true;
    };
    if (result) {
        bool greedyOk = GreedyRestore();
        if (!greedyOk) {
            return false;
        }
        baselineTurn = totalTurn;
        baselineStep = totalStep;
        baselineCommands = *result;
        hasBaseline = true;
    }

    return hasBaseline;

    static vector<SA4Cost> curCost;
    static vector<SA4Cost> nextCost;
    static vector<int> activeCur;
    static vector<int> activeNext;
    static vector<unsigned char> inNext;
    static vector<u8> prevRange;
    static vector<int> prevPublic;
    static vector<unsigned char> prevUsedMacro;
    static vector<int> publicList;
    static vector<unsigned char> publicQueued;

    curCost.assign(stateCount, SA4Cost{});
    nextCost.assign(stateCount, SA4Cost{});
    activeCur.clear();
    activeNext.clear();
    inNext.assign(stateCount, 0);
    prevRange.assign((segCount + 1) * stateCount, SA4InvalidPrev);
    prevPublic.assign((segCount + 1) * stateCount, -1);
    prevUsedMacro.assign((segCount + 1) * stateCount, 0);
    publicQueued.assign(publicCount, 0);

    int startState = StateIdx(noneRangeState, startPublic);
    curCost[startState] = SA4Cost{0, 0};
    activeCur.push_back(startState);

    MArr<bool> ballCompleted(M, false);
    static FindSegPathFrontierResult frontier;
    auto& pathSegCache = GetSA4SFindSegPathCache();
    REP(segIdx, segCount) {
        int targetBall = order[segIdx];
        for (int stateIdx : activeNext) {
            nextCost[stateIdx] = SA4Cost{};
        }
        for (int stateIdx : activeCur) {
            inNext[stateIdx] = 0;
        }
        activeNext.clear();
        publicList.clear();

        for (int stateIdx : activeCur) {
            int publicIdx = StatePublic(stateIdx);
            if (!publicQueued[publicIdx]) {
                publicQueued[publicIdx] = 1;
                publicList.push_back(publicIdx);
            }
        }

        for (int publicIdx : publicList) {
            int startCell;
            int startDir;
            FromCellDirIdx(publicIdx, startCell, startDir);

            bool foundFrontier = FindSegPathUniversalFrontierWithCache(pathSegCache, startCell, startDir, targetBall,
                                                                       ballCompleted, segmentMacros[segIdx], frontier);
            if (!foundFrontier) {
                continue;
            }

            cauto& frontierOptions = frontier.GetOptions();
            REP(rangeState, rangeStateCount) {
                int stateIdx = StateIdx(rangeState, publicIdx);
                cauto& cur = curCost[stateIdx];
                if (cur.turn_ >= SA4DPInf) {
                    continue;
                }
                REP(optionIdx, frontierOptions.size()) {
                    cauto& option = frontierOptions[optionIdx];
                    int nextRangeState = rangeState;
                    int addTurn = option.turn_;
                    if (option.usedMacro_) {
                        addTurn += rangeUpdateExtra[rangeState * segCount + segIdx];
                        nextRangeState = intervalRangeState[segIdx];
                    }
                    int nextTurn = cur.turn_ + addTurn;
                    int nextStep = cur.step_ + option.step_;
                    if (nextTurn >= limitTurn || nextStep > T) {
                        continue;
                    }

                    int nextState = StateIdx(nextRangeState, option.endIdx_);
                    auto& dst = nextCost[nextState];
                    if (!IsBetterCost(nextTurn, nextStep, dst)) {
                        continue;
                    }

                    dst.turn_ = nextTurn;
                    dst.step_ = nextStep;
                    int prevIdx = (segIdx + 1) * stateCount + nextState;
                    prevRange[prevIdx] = (u8)rangeState;
                    prevPublic[prevIdx] = publicIdx;
                    prevUsedMacro[prevIdx] = option.usedMacro_ ? 1 : 0;
                    if (!inNext[nextState]) {
                        inNext[nextState] = 1;
                        activeNext.push_back(nextState);
                    }
                }
            }
        }
        for (int publicIdx : publicList) {
            publicQueued[publicIdx] = 0;
        }

        if (activeNext.empty()) {
            return UseBaseline();
        }
        if ((int)activeNext.size() > SA4SegmentDPBeamWidth) {
            nth_element(activeNext.begin(), activeNext.begin() + SA4SegmentDPBeamWidth, activeNext.end(),
                        [&](int a, int b) {
                            cauto& ca = nextCost[a];
                            cauto& cb = nextCost[b];
                            return ca.turn_ < cb.turn_ || (ca.turn_ == cb.turn_ && ca.step_ < cb.step_);
                        });
            FOR(i, SA4SegmentDPBeamWidth, activeNext.size()) {
                int stateIdx = activeNext[i];
                nextCost[stateIdx] = SA4Cost{};
                inNext[stateIdx] = 0;
            }
            activeNext.resize(SA4SegmentDPBeamWidth);
        }

        swap(activeCur, activeNext);
        swap(curCost, nextCost);
        ballCompleted[targetBall] = true;
    }

    int bestState = -1;
    SA4Cost bestCost;
    for (int stateIdx : activeCur) {
        cauto& cost = curCost[stateIdx];
        if (IsBetterCost(cost.turn_, cost.step_, bestCost)) {
            bestCost = cost;
            bestState = stateIdx;
        }
    }
    if (bestState < 0) {
        return UseBaseline();
    }

    if (hasBaseline && !IsBetterCost(bestCost.turn_, bestCost.step_, SA4Cost{baselineTurn, baselineStep})) {
        return UseBaseline();
    }

    totalTurn = bestCost.turn_;
    totalStep = bestCost.step_;
    if (!result) {
        return true;
    }

    static array<int, M_max + 1> routeRange;
    static array<int, M_max + 1> routePublic;
    static array<unsigned char, M_max> routeUsedMacro;
    routeRange[segCount] = StateRange(bestState);
    routePublic[segCount] = StatePublic(bestState);

    for (int segIdx = segCount; segIdx >= 1; --segIdx) {
        int stateIdx = StateIdx(routeRange[segIdx], routePublic[segIdx]);
        int prevIdx = segIdx * stateCount + stateIdx;
        int pr = prevRange[prevIdx];
        int pp = prevPublic[prevIdx];
        if (pr == SA4InvalidPrev || pp < 0) {
            return UseBaseline();
        }
        routeRange[segIdx - 1] = pr;
        routePublic[segIdx - 1] = pp;
        routeUsedMacro[segIdx - 1] = prevUsedMacro[prevIdx];
    }
    if (routeRange[0] != noneRangeState || routePublic[0] != startPublic) {
        return UseBaseline();
    }

    static dvec<char> dpCommands;
    dpCommands.clear();
    ballCompleted.assign(M, false);
    static FindSegPathResult pathResult;
    REP(segIdx, segCount) {
        int targetBall = order[segIdx];
        int startCell;
        int startDir;
        FromCellDirIdx(routePublic[segIdx], startCell, startDir);
        int targetUsedMacro = routeUsedMacro[segIdx] ? 1 : 0;

        bool foundPath =
            FindSegPathUniversalToEnd(startCell, startDir, targetBall, ballCompleted, segmentMacros[segIdx],
                                      routePublic[segIdx + 1], pathResult, targetUsedMacro);
        if (!foundPath) {
            return UseBaseline();
        }

        if (pathResult.usedMacro_) {
            if (!AppendPathCommandsWithCompressedRange(routeRange[segIdx], segIdx, pathResult.commands_, dpCommands)) {
                return UseBaseline();
            }
        }
        else {
            dpCommands.extend(pathResult.commands_);
        }
        ballCompleted[targetBall] = true;
    }

    if ((int)dpCommands.size() != totalTurn) {
        return UseBaseline();
    }
    *result = dpCommands;
    return true;
}

template <class PATH_STATS>
double CalcEvalPenalty(int totalStep, const PATH_STATS& pathStats) {
    double p = 0;

    p += totalStep * HP.R_EvalStep;

    int maxTurn = 0;
    double intervalTurnVariance = 0.0;
    {
        double intervalTurnSum = 0;
        double intervalTurnSum2 = 0;

        REP(i, pathStats.size()) {
            int turn = pathStats[i].turn_;
            chmax(maxTurn, turn);
            intervalTurnSum += turn;
            intervalTurnSum2 += (double)turn * turn;
        }

        VASSERT(pathStats.size() > 0);
        double div = 1.0 / (double)pathStats.size();
        double mean = intervalTurnSum * div;
        intervalTurnVariance = intervalTurnSum2 * div - square(mean);
    }

    p += maxTurn * HP.R_EvalMaxIntervalTurn;

    p += intervalTurnVariance * HP.R_EvalIntervalVariance;

    return p;
}

template <class PATH_STATS>
double Eval(int totalTurn, int totalStep, const PATH_STATS& pathStats, double timeRate) {
    double e = -totalTurn;
    if (timeRate < HP.R_PenaltyTime) {
        e -= CalcEvalPenalty(totalStep, pathStats) * (HP.R_PenaltyTime - timeRate) / HP.R_PenaltyTime;
    }
    return e;
}

struct SAState {
    int rawScore_;
    double evalScore_;
    SA4Macro baseMacro_;
    CapArr<MacroRange, M_max * 2> intervalsRange_;  
    CapArr<SA4IntervalPathStat, M_max * 2> intervalPathStats_;
    CapArr<SA4MacroKey, M_max * 2> intervalMacroKeys_;
    CapArr<int, M_max + 1> prefixTurns_;
    CapArr<int, M_max + 1> prefixSteps_;
    CapArr<MacroRange, M_max + 1> prefixRanges_;
    MArr<u8> order_;
    int totalTurn_;
    int totalStep_;

    void SetScore(int totalTurn, int totalStep, double evalScore) {
        totalTurn_ = totalTurn;
        totalStep_ = totalStep;
        rawScore_ = -totalTurn_;
        evalScore_ = evalScore;
    }

    void RebuildPrefixes() {
        VASSERT((int)intervalsRange_.size() == M);
        VASSERT((int)intervalPathStats_.size() == M);
        prefixTurns_.resize(M + 1);
        prefixSteps_.resize(M + 1);
        prefixRanges_.resize(M + 1);

        int totalTurn = 0;
        int totalStep = 0;
        MacroRange curRange = {-1, -1};
        prefixTurns_[0] = totalTurn;
        prefixSteps_[0] = totalStep;
        prefixRanges_[0] = curRange;

        REP(segIdx, M) {
            cauto& stat = intervalPathStats_[segIdx];
            cauto& interval = intervalsRange_[segIdx];
            int segmentTurn = stat.turn_;
            if (stat.usedMacro_ && curRange != interval) {
                segmentTurn += CalcMacroUpdateExtra(baseMacro_, curRange, interval);
                curRange = interval;
            }
            totalTurn += segmentTurn;
            totalStep += stat.step_;
            prefixTurns_[segIdx + 1] = totalTurn;
            prefixSteps_[segIdx + 1] = totalStep;
            prefixRanges_[segIdx + 1] = curRange;
        }

        VASSERT(totalTurn == totalTurn_);
        VASSERT(totalStep == totalStep_);
    }

    template <class MACRO>
    bool Init(const MACRO& macro, const MArr<u8>& order) {
        if (CountMacroS(macro) > 1) {
            return false;
        }
        if (!AssignSA4Macro(baseMacro_, macro)) {
            return false;
        }
        intervalsRange_.assign(M, MacroRange{0, (int)macro.size()});

        if (!IsValidOrder(order)) {
            return false;
        }
        order_ = order;
        MacroRange usedRange;
        bool ret = EvaluateFixedOrderSegmentsWithCache(baseMacro_, intervalsRange_, order_, INT_MAX, totalTurn_,
                                                       totalStep_, intervalPathStats_, intervalMacroKeys_, usedRange);
        if (!ret) {
            return false;
        }

        double evalScore = Eval(totalTurn_, totalStep_, intervalPathStats_, 0);

        SetScore(totalTurn_, totalStep_, evalScore);
        RebuildPrefixes();
        return true;
    }
};

void Solve(ChronoTimer& timer, Result& result) {
    Rng rng;
    InitSA4SFindSegPathCache();

    static RandomTable baseCands;
    baseCands.push('F', HP.R_ChoiceFowardRate);
    baseCands.push('L', HP.R_ChoiceRotateRate);
    baseCands.push('R', HP.R_ChoiceRotateRate);

    auto FlipLR = [](char c) -> char {
        if (c == 'L') {
            return 'R';
        }
        if (c == 'R') {
            return 'L';
        }
        return c;
    };

    auto FindRandomF = [&](const SA4Macro& macro) -> int {
        int n = (int)macro.size();
        if (n == 0) {
            return -1;
        }
        int first = rng(n);
        REP(i, n) {
            int j = (first + i) % n;
            if (macro[j] == 'F') {
                return j;
            }
        }
        return -1;
    };

    auto FindRandomTurn = [&](const SA4Macro& macro) -> int {
        int n = (int)macro.size();
        if (n == 0) {
            return -1;
        }
        int first = rng(n);
        REP(i, n) {
            int j = (first + i) % n;
            if (macro[j] == 'L' || macro[j] == 'R') {
                return j;
            }
        }
        return -1;
    };

    auto FindMacroS = [](const SA4Macro& macro) -> int {
        REP(i, macro.size()) {
            if (macro[i] == 'S') {
                return i;
            }
        }
        return -1;
    };

    static RandomTable transTbl;
    {
        transTbl.push(0, HP.R_TransRangeShrink);
        transTbl.push(1, HP.R_TransRangeExpand);

        transTbl.push(2, HP.R_TransMacroChange);        
        transTbl.push(3, HP.R_TransMacroAddSide);       
        transTbl.push(4, HP.R_TransMacroRemoveSide);    
        transTbl.push(14, HP.R_TransMacroAddRand);      
        transTbl.push(15, HP.R_TransMacroRemoveRand);   
        transTbl.push(13, HP.R_TransMacroRotate);       
        transTbl.push(12, HP.R_TransMacroSwap);         
        transTbl.push(6, HP.R_TransMacroLRFlip);        
        transTbl.push(7, HP.R_TransMacroFowardExpand);  
        transTbl.push(8, HP.R_TransMacroFowardShrink);  

        transTbl.push(18, HP.S_TransMacroSRemove);  
        transTbl.push(19, HP.S_TransMacroSMove);    

        transTbl.push(5, HP.R_TransBallInsert);       
        transTbl.push(16, HP.R_TransBallSwap);        
        transTbl.push(10, HP.R_TransBallNearInsert);  
        transTbl.push(11, HP.R_TransBallNearSwap);    
    }

    CapArr<SA4IntervalPathStat, M_max * 2> tmpNewPathStats;
    CapArr<SA4MacroKey, M_max * 2> tmpNewMacroKeys;
    CapArr<int, M_max + 1> tmpNewPrefixTurns;
    CapArr<int, M_max + 1> tmpNewPrefixSteps;
    CapArr<MacroRange, M_max + 1> tmpNewPrefixRanges;
    SA4Macro tmpCutBaseMacro;
    CapArr<MacroRange, M_max * 2> tmpCutIntervalsRange;
    CapArr<SA4MacroKey, M_max * 2> tmpCutMacroKeys;
    CapArr<MacroRange, M_max + 1> tmpCutPrefixRanges;


    auto Trans = [&](SAState& state, SAChecker& checker, double timeRate, int& transIdx) -> SAResult {
        auto TryUpdateScore = [&](SAState& state, SA4Macro& newBaseMacro,
                                  CapArr<MacroRange, M_max * 2>& newIntervalsRange, const MArr<u8>& newOrder,
                                  int dirtyStart) -> SAResult {
            if (CountMacroS(newBaseMacro) > 1) {
                return SAResult::Fail;
            }
            dirtyStart = clamp(dirtyStart, 0, M);


            double acceptScore = checker.GetAcceptScore(rng);
            int limitTurn = int(-acceptScore) + 1;

            int newTotalTurn;
            int newTotalStep;
            auto& newPathStats = tmpNewPathStats;
            auto& newMacroKeys = tmpNewMacroKeys;
            auto& newPrefixTurns = tmpNewPrefixTurns;
            auto& newPrefixSteps = tmpNewPrefixSteps;
            auto& newPrefixRanges = tmpNewPrefixRanges;
            MacroRange newUsedRange;
            bool ret = EvaluateFixedOrderSegmentsWithCache(
                newBaseMacro, newIntervalsRange, newOrder, limitTurn, newTotalTurn, newTotalStep, newPathStats,
                newMacroKeys, newUsedRange, &state.baseMacro_, &state.intervalsRange_, &state.order_,
                &state.intervalPathStats_, &state.intervalMacroKeys_, &state.prefixTurns_, &state.prefixSteps_,
                &state.prefixRanges_, dirtyStart, &newPrefixTurns, &newPrefixSteps, &newPrefixRanges);
            if (!ret) {
                return SAResult::Ng;
            }

            double newEvalScore = Eval(newTotalTurn, newTotalStep, newPathStats, timeRate);
            if (!checker.CheckAccept(newEvalScore, rng)) {
                return SAResult::Ng;
            }

            const CapArr<MacroRange, M_max + 1>* commitPrefixRanges = &newPrefixRanges;

            if (newUsedRange.l_ >= 0 && newUsedRange.l_ < newUsedRange.r_) {
                int cutL = newUsedRange.l_;
                int cutR = newUsedRange.r_;
                int cutLen = cutR - cutL;
                auto& cutIntervalsRange = tmpCutIntervalsRange;
                cutIntervalsRange = newIntervalsRange;

                REP(i, cutIntervalsRange.size()) {
                    cauto& oldRange = newIntervalsRange[i];
                    int l = max(oldRange.l_, cutL) - cutL;
                    int r = min(oldRange.r_, cutR) - cutL;
                    cutIntervalsRange[i] = MacroRange{l, r};
                }

                auto& cutBaseMacro = tmpCutBaseMacro;
                bool builtCutMacro = BuildSA4MacroRange(newBaseMacro, MacroRange{cutL, cutR}, cutBaseMacro);
                VASSERT(builtCutMacro);
                VASSERT(IsValidRanges(cutBaseMacro, cutIntervalsRange));
                auto& cutMacroKeys = tmpCutMacroKeys;
                BuildSA4IntervalMacroKeys(cutBaseMacro, cutIntervalsRange, cutIntervalsRange.size(), cutMacroKeys);
                auto& cutPrefixRanges = tmpCutPrefixRanges;
                cutPrefixRanges.resize(M + 1);
                REP(i, newPrefixRanges.size()) {
                    cauto& range = newPrefixRanges[i];
                    if (range.l_ < 0) {
                        cutPrefixRanges[i] = MacroRange{-1, -1};
                    }
                    else {
                        int l = max(range.l_, cutL) - cutL;
                        int r = min(range.r_, cutR) - cutL;
                        cutPrefixRanges[i] = MacroRange{l, r};
                    }
                }
                newBaseMacro = cutBaseMacro;
                newIntervalsRange = cutIntervalsRange;
                newMacroKeys = cutMacroKeys;
                newUsedRange = MacroRange{0, cutLen};
                commitPrefixRanges = &cutPrefixRanges;
            }

            state.SetScore(newTotalTurn, newTotalStep, newEvalScore);
            state.baseMacro_ = newBaseMacro;
            state.intervalsRange_ = newIntervalsRange;
            state.intervalPathStats_ = newPathStats;
            state.intervalMacroKeys_ = newMacroKeys;
            state.order_ = newOrder;
            state.prefixTurns_ = newPrefixTurns;
            state.prefixSteps_ = newPrefixSteps;
            state.prefixRanges_ = *commitPrefixRanges;
            VASSERT(state.prefixTurns_.back() == state.totalTurn_);
            VASSERT(state.prefixSteps_.back() == state.totalStep_);
            return SAResult::Ok;
        };

        auto DirtyStartFromOrderIndex = [](int orderIdx) { return clamp(orderIdx, 0, M); };
        auto FirstDifferentRange = [&](const CapArr<MacroRange, M_max * 2>& ranges) {
            REP(segIdx, M) {
                if (ranges[segIdx] != state.intervalsRange_[segIdx]) {
                    return segIdx;
                }
            }
            return M;
        };
        auto DirtyStartFromMacroPos = [&](const CapArr<MacroRange, M_max * 2>& ranges, int pos) {
            REP(segIdx, M) {
                cauto& range = ranges[segIdx];
                if (range.l_ <= pos && pos < range.r_) {
                    return segIdx;
                }
            }
            return M;
        };

        transIdx = transTbl(rng);

        if (transIdx == 0) {
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int intervalIdx = rng(newIntervalsRange.size());

            auto CanApplyRangeMove = [&](int c) -> bool {
                cauto& interval = newIntervalsRange[intervalIdx];
                if (c == 1) {
                    return interval.l_ < interval.r_;
                }
                if (c == 2) {
                    return interval.l_ < interval.r_;
                }
                VABORT();
                return false;
            };

            auto ApplyRangeMove = [&](CapArr<MacroRange, M_max * 2>& ranges, int c) {
                auto& interval = ranges[intervalIdx];
                if (c == 1) {
                    ++interval.l_;
                    int newL = interval.l_;
                    RREP(i, intervalIdx) {
                        if (ranges[i].l_ < newL) {
                            ranges[i].l_ = newL;
                            if (ranges[i].r_ < newL) {
                                ranges[i].r_ = newL;
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
                if (c == 2) {
                    --interval.r_;
                    int newR = interval.r_;
                    RREP(i, intervalIdx) {
                        if (ranges[i].r_ > newR) {
                            ranges[i].r_ = newR;
                            if (ranges[i].l_ > newR) {
                                ranges[i].l_ = newR;
                            }
                        }
                        else {
                            break;
                        }
                    }
                }
                VASSERT(IsValidRanges(state.baseMacro_, ranges));
            };

            CapArr<int, 2> cands;
            FOR(c, 1, 3) {
                if (CanApplyRangeMove(c)) {
                    cands.push(c);
                }
            }
            if (cands.empty()) {
                return SAResult::Fail;
            }

            int c = cands.choice(rng);
            ApplyRangeMove(newIntervalsRange, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, state.baseMacro_, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 1) {
            int baseMacroSize = (int)state.baseMacro_.size();
            CapArr<int, M_max * 2> candIntervals;
            REP(i, state.intervalsRange_.size()) {
                cauto& range = state.intervalsRange_[i];
                if (range.l_ > 0 || range.r_ < baseMacroSize) {
                    candIntervals.push(i);
                }
            }
            if (candIntervals.empty()) {
                return SAResult::Fail;
            }

            int intervalIdx = candIntervals.choice(rng);
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            auto CanApplyRangeMove = [&](int c) -> bool {
                cauto& interval = newIntervalsRange[intervalIdx];
                if (c == 0) {
                    return interval.l_ > 0;
                }
                if (c == 3) {
                    return interval.r_ < baseMacroSize;
                }
                VABORT();
                return false;
            };

            auto ApplyRangeMove = [&](CapArr<MacroRange, M_max * 2>& ranges, int c) {
                auto& interval = ranges[intervalIdx];
                if (c == 0) {
                    --interval.l_;
                    int newL = interval.l_;
                    FOR(i, intervalIdx + 1, ranges.size()) {
                        if (ranges[i].l_ > newL) {
                            ranges[i].l_ = newL;
                        }
                        else {
                            break;
                        }
                    }
                }
                else if (c == 3) {
                    ++interval.r_;
                    int newR = interval.r_;
                    FOR(i, intervalIdx + 1, ranges.size()) {
                        if (ranges[i].r_ < newR) {
                            ranges[i].r_ = newR;
                        }
                        else {
                            break;
                        }
                    }
                }
                else {
                    VABORT();
                }
                VASSERT(IsValidRanges(state.baseMacro_, ranges));
            };

            CapArr<int, 2> cands;
            for (int c : {0, 3}) {
                if (CanApplyRangeMove(c)) {
                    cands.push(c);
                }
            }
            if (cands.empty()) {
                return SAResult::Fail;
            }

            int c = cands.choice(rng);
            ApplyRangeMove(newIntervalsRange, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, state.baseMacro_, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 2) {
            SA4Macro newMacro = state.baseMacro_;

            int j = rng(newMacro.size());
            static dvec<char> cands;
            cands.clear();
            for (char c : baseCands.table_) {
                if (newMacro[j] != c) {
                    cands.push(c);
                }
            }
            newMacro[j] = cands.choice(rng);

            int dirtyStart = DirtyStartFromMacroPos(state.intervalsRange_, j);
            SAResult result = TryUpdateScore(state, newMacro, state.intervalsRange_, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 3) {
            if ((int)state.baseMacro_.size() >= SA4MacroMax) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(2) ? 0 : newMacro.size();
            char c = baseCands(rng);
            InsertBaseMacroCommand(newMacro, newIntervalsRange, j, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 4) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(2) ? 0 : (newMacro.size() - 1);
            if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 14) {
            if ((int)state.baseMacro_.size() >= SA4MacroMax) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(newMacro.size() + 1);
            char c = baseCands(rng);
            InsertBaseMacroCommand(newMacro, newIntervalsRange, j, c);

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 15) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            int j = rng(newMacro.size());
            if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 13) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;

            if (rng(2)) {
                char c = newMacro[0];
                if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, 0)) {
                    return SAResult::Fail;
                }
                InsertBaseMacroCommand(newMacro, newIntervalsRange, newMacro.size(), c);
            }
            else {
                char c = newMacro.back();
                if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, newMacro.size() - 1)) {
                    return SAResult::Fail;
                }
                InsertBaseMacroCommand(newMacro, newIntervalsRange, 0, c);
            }

            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, 0);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 12) {
            if (state.baseMacro_.size() < 2) {
                return SAResult::Fail;
            }
            SA4Macro newMacro = state.baseMacro_;

            int a = rng(newMacro.size());
            char baseChar = newMacro[a];

            CapArr<int, SA4MacroMax> cands;
            REP(i, newMacro.size()) {
                if (newMacro[i] != baseChar) {
                    cands.push(i);
                }
            }
            if (cands.empty()) {
                return SAResult::Fail;
            }

            int b = cands.choice(rng);
            swap(newMacro[a], newMacro[b]);

            int dirtyStart =
                min(DirtyStartFromMacroPos(state.intervalsRange_, a), DirtyStartFromMacroPos(state.intervalsRange_, b));
            SAResult result = TryUpdateScore(state, newMacro, state.intervalsRange_, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 5) {
            MArr<u8> newOrder = state.order_;
            int a = rng(M);
            int b = rng(M - 1);
            if (b >= a) {
                ++b;
            }

            u8 ball = newOrder[a];
            newOrder.remove(a);
            newOrder.insert(b, ball);

            int dirtyStart = DirtyStartFromOrderIndex(min(a, b));
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 16) {
            if (M <= 1) {
                return SAResult::Fail;
            }
            MArr<u8> newOrder = state.order_;
            int a = rng(M);
            int b = rng(M - 1);
            if (b >= a) {
                ++b;
            }
            swap(newOrder[a], newOrder[b]);

            int dirtyStart = DirtyStartFromOrderIndex(min(a, b));
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 10) {
            if (M <= 1) {
                return SAResult::Fail;
            }
            MArr<u8> newOrder = state.order_;
            int a = rng(M);
            int maxDist = min(4, M - 1);
            int dist = 1 + rng(maxDist);
            if (rng(2) == 0) {
                dist = -dist;
            }
            int b = a + dist;
            if (b < 0 || b >= M) {
                b = a - dist;
            }
            if (b < 0 || b >= M || b == a) {
                return SAResult::Fail;
            }

            u8 ball = newOrder[a];
            newOrder.remove(a);
            newOrder.insert(b, ball);

            int dirtyStart = DirtyStartFromOrderIndex(min(a, b));
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 11) {
            if (M <= 1) {
                return SAResult::Fail;
            }
            MArr<u8> newOrder = state.order_;
            int a = rng(M - 1);
            swap(newOrder[a], newOrder[a + 1]);

            int dirtyStart = DirtyStartFromOrderIndex(a);
            SAResult result = TryUpdateScore(state, state.baseMacro_, state.intervalsRange_, newOrder, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 6) {
            SA4Macro newMacro = state.baseMacro_;
            int j = FindRandomTurn(newMacro);
            if (j < 0) {
                return SAResult::Fail;
            }
            newMacro[j] = FlipLR(newMacro[j]);

            int dirtyStart = DirtyStartFromMacroPos(state.intervalsRange_, j);
            SAResult result = TryUpdateScore(state, newMacro, state.intervalsRange_, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 7) {
            if ((int)state.baseMacro_.size() >= SA4MacroMax) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;
            int j = FindRandomF(newMacro);
            if (j < 0) {
                return SAResult::Fail;
            }
            if (rng(2) == 0) {
                ++j;
            }
            InsertBaseMacroCommand(newMacro, newIntervalsRange, j, 'F');

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 8) {
            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;
            int j = FindRandomF(newMacro);
            if (j < 0 || !EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 18) {
            if ((int)state.baseMacro_.size() <= 1) {
                return SAResult::Fail;
            }

            int j = FindMacroS(state.baseMacro_);
            if (j < 0) {
                return SAResult::Fail;
            }

            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;
            if (!EraseBaseMacroCommand(newMacro, newIntervalsRange, j)) {
                return SAResult::Fail;
            }

            int dirtyStart = FirstDifferentRange(newIntervalsRange);
            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        else if (transIdx == 19) {
            int sPos = FindMacroS(state.baseMacro_);
            SA4Macro newMacro = state.baseMacro_;
            CapArr<MacroRange, M_max * 2> newIntervalsRange = state.intervalsRange_;
            int dirtyStart;

            if (sPos < 0) {
                if ((int)newMacro.size() >= SA4MacroMax) {
                    return SAResult::Fail;
                }
                int j = rng(newMacro.size() + 1);
                InsertBaseMacroCommand(newMacro, newIntervalsRange, j, 'S');
                dirtyStart = FirstDifferentRange(newIntervalsRange);
            }
            else {
                if (newMacro.size() <= 1) {
                    return SAResult::Fail;
                }
                int j = rng(newMacro.size() - 1);
                if (j >= sPos) {
                    ++j;
                }
                swap(newMacro[sPos], newMacro[j]);
                dirtyStart =
                    min(DirtyStartFromMacroPos(newIntervalsRange, sPos), DirtyStartFromMacroPos(newIntervalsRange, j));
            }

            SAResult result = TryUpdateScore(state, newMacro, newIntervalsRange, state.order_, dirtyStart);
            if (result != SAResult::Ok) {
                return result;
            }
            return SAResult::Ok;
        }
        VABORT();
        return SAResult::Fail;
    };

    static EasySA<SAState> sa;
    sa.updateTmpMask_ = (1 << HP.UpdateTmpMaskBit) - 1;
    sa.tempType_ = HP.TempType;
    sa.tempPow_ = HP.Type2_TempPow;
    sa.rollbackStartTimeRate_ = HP.RollbackStartRate;
    sa.rollbackCount_ = HP.RollbackCount;
    sa.rollbackMul_ = HP.RollbackNextMulti;
    sa.minRollbackCount_ = HP.MinRollbackCount;
    sa.filterStartTime_ = HP.FilterStartTime;
    sa.filterEndTime_ = HP.FilterEndTime;

    double startTemp;
    double endTemp;
    if (HP.TempType == 0) {
        startTemp = HP.Type0_StartTemp;
        endTemp = HP.Type0_EndTemp;
    }
    else if (HP.TempType == 1) {
        startTemp = HP.Type1_StartTemp;
        endTemp = HP.Type1_EndTemp;
    }
    else {
        startTemp = HP.Type2_StartTemp;
        endTemp = HP.Type2_EndTemp;
    }

    auto BuildInitialOrder = [](const SA4Macro& macro, MArr<u8>& order) -> bool {
        int totalTurn;
        int totalStep;
        MacroCache macroCache;
        macroCache.Init(macro);
        return greedy2b::CalcTotalTurn(macroCache, INT_MAX, totalTurn, &order) &&
               greedy2b::SolveWithOrder(macroCache, INT_MAX, order, totalTurn, totalStep, nullptr);
    };
    auto BuildSequentialOrder = [](MArr<u8>& order) {
        order.clear();
        REP(i, M) {
            order.push((u8)i);
        }
    };
    auto EvaluateInitialSegments = [&](const SA4Macro& macro, const MArr<u8>& order, int& totalTurn, int& totalStep,
                                       CapArr<SA4IntervalPathStat, M_max * 2>& pathStats) -> bool {
        if (CountMacroS(macro) > 1) {
            return false;
        }
        if (!IsValidOrder(order)) {
            return false;
        }
        CapArr<MacroRange, M_max * 2> ranges;
        ranges.assign(M, MacroRange{0, (int)macro.size()});
        CapArr<SA4MacroKey, M_max * 2> macroKeys;
        MacroRange usedRange;
        return EvaluateFixedOrderSegmentsWithCache(macro, ranges, order, INT_MAX, totalTurn, totalStep, pathStats,
                                                   macroKeys, usedRange);
    };

    static dvec<SAState> states(HP.InitialStateCount);
    {
        struct Cand {
            SA4Macro macro_;
            MArr<u8> order_;
        };
        static SizeFilter2<Cand> filter;
        filter.Init(HP.InitialStateCount);

        int limitTurn = INT_MAX;
        constexpr int LimitMargin = 30;

        int totalTurn;
        int totalStep;
        int segmentTurn;
        int segmentStep;
        MArr<u8> order;
        MArr<u8> fallbackOrder;
        CapArr<SA4IntervalPathStat, M_max * 2> initialPathStats;
        SA4Macro fallbackMacro = MakeSA4Macro("FFFF");
        if (!BuildInitialOrder(fallbackMacro, fallbackOrder)) {
            BuildSequentialOrder(fallbackOrder);
        }
        MacroCache macroCache;
        for (cauto& macroSrc : MacroTable) {
            SA4Macro macro;
            if (!AssignSA4Macro(macro, macroSrc)) {
                continue;
            }
            macroCache.Init(macro);
            if (greedy2b::CalcTotalTurn(macroCache, limitTurn, totalTurn, &order)) {
                double tmpEvalScore = -totalTurn;
                if (filter.CanPush(tmpEvalScore)) {
                    if (greedy2b::SolveWithOrder(macroCache, limitTurn, order, totalTurn, totalStep, nullptr) &&
                        EvaluateInitialSegments(macro, order, segmentTurn, segmentStep, initialPathStats)) {
                        double evalScore = Eval(segmentTurn, segmentStep, initialPathStats, 0);
                        if (Cand* ptr = filter.TryPush(evalScore)) {
                            auto& c = *ptr;
                            c.macro_ = macro;
                            c.order_ = order;

                            if (segmentTurn + LimitMargin < limitTurn) {
                                limitTurn = segmentTurn + LimitMargin;
                            }
                        }
                    }
                }
            }
        }

        int idx = 0;
        for (cauto& c : filter.datas_) {
            if (idx >= (int)states.size()) {
                break;
            }
            if (states[idx].Init(c.macro_, c.order_)) {
                ++idx;
            }
        }
        if (idx == 0 && states[0].Init(fallbackMacro, fallbackOrder)) {
            idx = 1;
        }
        if (idx == 0) {
            BuildSequentialOrder(fallbackOrder);
            if (states[0].Init(fallbackMacro, fallbackOrder)) {
                idx = 1;
            }
        }
        if (idx == 0) {
            VABORT();
            return;
        }
        FOR(i, idx, states.size()) {
            states[i] = states[i % idx];
        }
    }

    sa.Run(timer, states, startTemp, endTemp, Trans);

    cauto& best = sa.GetBest();
    int totalTurn;
    int totalStep;
    if (!SolveFixedOrderSegments(best.baseMacro_, best.intervalsRange_, best.order_, INT_MAX, totalTurn, totalStep,
                                 &result.commands_)) {
        VABORT();
    }


}

}  

struct Solver {
    void Run(ChronoTimer& timer) {
        Result result;

        if (N <= FindSegPathCompactStrideNMax) {
            sa4s::Solve(timer, result);
        }
        else {
            sa4r::Solve(timer, result);
        }


        server.Output(result);

    }

    int CountMacroRuns(const Result& result) const {
        int count = 0;
        for (char command : result.commands_) {
            if (command == 'P') {
                ++count;
            }
        }
        return count;
    }

    int CountBasicCommand(const Result& result) const {
        int count = 0;
        bool recoding = false;
        int macroCount = 0;
        int recodingCount = 0;
        for (char command : result.commands_) {
            if (command == 'F' || command == 'L' || command == 'R' || command == 'S') {
                ++count;
                if (recoding) {
                    ++recodingCount;
                }
            }
            else if (command == 'M') {
                if (recoding) {
                    macroCount = recodingCount;
                    recodingCount = 0;
                }
                else {
                    recodingCount = 0;
                }
                recoding = !recoding;
            }
            else if (command == 'P') {
                count += macroCount;
                if (recoding) {
                    recodingCount += macroCount;
                }
            }
        }
        return count;
    }
};

struct Main {
    void Run(int argc, const char* argv[]) {
        VASSERT(argc == 1);
        ChronoTimer timer;
        server.InitInput(timer);

        timer.StartMs(TIME_LIMIT);

        static Solver solver;
        solver.Run(timer);

        server.Finalize();
    }
};
