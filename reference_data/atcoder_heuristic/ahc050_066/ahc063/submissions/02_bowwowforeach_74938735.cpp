#define FULL_LIB 1
#define TIME_LIMIT (1950)
#define TIME_LIMIT_US (TIME_LIMIT * 1000)

#define BUILD_MODE_MAIN 0        
#define BUILD_MODE_EVAL 1        
#define BUILD_MODE_VALID_EVAL 2  
#define BUILD_MODE_PERF 3        
#define BUILD_MODE_OPTUNE 4      
#define BUILD_MODE_UNIT_TEST 5   
#define BUILD_MODE_SUBMIT 6      
#define BUILD_MODE_CODETEST 7    
#define BUILD_MODE_LLM 8         


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
        VASSERT(m_count < m_capacity);

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
        VASSERT(m_count < m_capacity);

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

constexpr array<pint, 4> Around4 = {pint{-1, 0}, pint{0, -1}, pint{1, 0}, pint{0, 1}};

struct GridSystem {
    int W;
    int H;
    int WH;  

   private:
    vector<pint> toPos_;
    vector<CCA<int, 4>> around_;  
    vector<array<int, 4>> dirs_;  
    vector<Dir> calcDirTable_;    

   public:
    void Init(int w, int h) {
        W = w;
        H = h;
        WH = W * H;

        toPos_.resize(WH);
        around_.resize(WH);
        dirs_.resize(WH);
        calcDirTable_.assign((size_t)WH * WH, Dir::Invalid);
        REP(id, WH) {
            calcDirTable_[id * WH + id] = Dir::N;
        }
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
                        calcDirTable_[id * WH + nc] = Dir4[di];
                    }
                    else {
                        dirs_[id][di] = -1;
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
        VASSERT(from >= 0 && from < WH);
        VASSERT(to >= 0 && to < WH);
        Dir dir = calcDirTable_[from * WH + to];
        VASSERT(dir != Dir::Invalid);
        return dir;
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

            while (*ptr_ == 0 && ai_ < ARR_SIZE) {
                ++ptr_;
                ++ai_;
            }
            if (ai_ < ARR_SIZE) {
                bits_ = *ptr_;
                int bi = countr_zero(bits_);
                idx_ = (ai_ << 6) | bi;
                bits_ &= ~(1ULL << bi);
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
                while (*ptr_ == 0 && ai_ < ARR_SIZE) {
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
                bits_ &= ~(1ULL << bi);
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
    using name##Check = CheckMapS<size>;               \
    using name##Bit = BitArr<size>;

constexpr int N_min = 8;
constexpr int N_max = 16;
constexpr int NN_max = N_max * N_max;

constexpr int M_min = 16;
constexpr int M_max = 192;

constexpr int C_min = 3;
constexpr int C_max = 7;

DEF_CONTAINERS(N, N_max);
DEF_CONTAINERS(NN, NN_max);
DEF_CONTAINERS(M, M_max);
DEF_CONTAINERS(C, C_max + 1);  

int N;
int NN;
int M;
int C;
MArr<u8> targetBodyColors_;  
NNArr<u8> startGridColors_;  
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


    START_TUNING;





    PD(EvalCurveCountSameRate, 0.3090764832519882, -0.5, 0.5, -1.0, 1.0);
    PD(EvalFoodSpreadSameRate, 0.3140729602836378, -0.5, 0.5, -1.0, 1.0);
    PD(EvalHasAroundNextSameRate, 0.18461639906139232, -0.5, 0.5, -1.0, 1.0);
    PD(EvalCurveCount, 0.4207747445332068, 0.2, 0.8, 0.0, 5.0);
    PD(EvalFoodSpread, 0.8641793273964486, 0.5, 1.5, 0.0, 5.0);
    PD(EvalHasAroundNext, 3.559553550671569, 2.0, 5.0, 0.0, 5.0);
    PI(MaxSameCountDown, 10, 15, 25, 0, 30);
    PC(FirstEncodeLevel, 0, 0, 1, 2);
    END_TUNING;









} HP;


constexpr array<array<double, 5>, 9> HPArray_EvalCurveCountSameRate = {{
    {{-0.659228, 0.911537, 0.876179, -0.348371, -0.005226}},
    {{-0.192314, -0.262141, -0.482757, -0.140267, 0.760182}},
    {{-0.172247, -0.197999, -0.790800, 0.055670, 0.999874}},
    {{0.889049, 0.752271, 0.324516, 0.364717, -0.990953}},
    {{0.806839, -0.717792, 0.905443, 0.811751, -0.214578}},
    {{0.737011, 0.551605, 0.950268, -0.211182, 0.651290}},
    {{0.785492, 0.919468, 0.849028, 0.881220, 0.189227}},
    {{0.043159, -0.117442, 0.898957, 0.166022, 0.173787}},
    {{-0.871992, -0.278662, -0.620123, -0.959026, -0.604939}},
}};
constexpr array<array<double, 5>, 9> HPArray_EvalFoodSpreadSameRate = {{
    {{0.824821, 0.666516, 0.950483, 0.664164, -0.555850}},
    {{0.849339, 0.889996, 0.781135, -0.160066, 0.989355}},
    {{0.765217, 0.702802, 0.908622, 0.464224, 0.931314}},
    {{0.926016, 0.785597, 0.339920, 0.070040, 0.303891}},
    {{0.929209, -0.939455, 0.913124, 0.857491, 0.088967}},
    {{0.905475, 0.457230, 0.139492, 0.082086, 0.406024}},
    {{0.786586, 0.981925, 0.471410, 0.525364, 0.046727}},
    {{0.894804, 0.964722, 0.068062, 0.372992, -0.074126}},
    {{-0.338144, 0.996094, 0.813630, 0.180391, 0.654151}},
}};
constexpr array<array<double, 5>, 9> HPArray_EvalHasAroundNextSameRate = {{
    {{0.950545, 0.480261, -0.983622, -0.335893, 0.428666}},
    {{0.030482, -0.652330, 0.926160, -0.335514, 0.108076}},
    {{0.124404, -0.818245, 0.126208, -0.383785, 0.078196}},
    {{-0.051860, -0.808596, 0.998527, 0.519852, 0.027371}},
    {{0.548808, 0.941293, -0.028329, 0.214244, 0.378288}},
    {{-0.414525, 0.202915, 0.020109, 0.048109, 0.401380}},
    {{0.747426, 0.830174, -0.407246, 0.692763, 0.044352}},
    {{0.705853, -0.047061, 0.119896, 0.995877, 0.415746}},
    {{-0.386311, 0.071576, 0.250170, 0.151356, -0.509284}},
}};
constexpr array<array<double, 5>, 9> HPArray_EvalCurveCount = {{
    {{2.359963, 0.792820, 1.456441, 3.795264, 2.680895}},
    {{0.886680, 1.937557, 1.771823, 0.425168, 4.097338}},
    {{1.678721, 2.589160, 1.077603, 4.435323, 4.842671}},
    {{1.444453, 1.856393, 2.080712, 2.556652, 4.691646}},
    {{2.840805, 0.650164, 4.790979, 3.119185, 1.256838}},
    {{3.667645, 4.776785, 4.731173, 0.528011, 2.678912}},
    {{2.607472, 2.348993, 4.868193, 3.590743, 1.862405}},
    {{2.676433, 4.082145, 4.357498, 2.346422, 3.163800}},
    {{0.004142, 1.889694, 0.090245, 1.585400, 3.421949}},
}};
constexpr array<array<double, 5>, 9> HPArray_EvalFoodSpread = {{
    {{2.834188, 3.555163, 4.953287, 4.705955, 2.593692}},
    {{4.255733, 4.629732, 3.980480, 0.532472, 4.330217}},
    {{4.417450, 4.537493, 3.693102, 3.894954, 2.497679}},
    {{4.549871, 4.426130, 4.706906, 3.741909, 2.653257}},
    {{4.181770, 4.569634, 4.773169, 4.987539, 1.417271}},
    {{4.678922, 4.484218, 3.695378, 0.428233, 3.132540}},
    {{4.997947, 4.840431, 4.736272, 3.440989, 0.586657}},
    {{3.653280, 4.832342, 4.838718, 3.111054, 2.614684}},
    {{2.717225, 1.552155, 0.964898, 3.919436, 1.648128}},
}};
constexpr array<array<double, 5>, 9> HPArray_EvalHasAroundNext = {{
    {{2.337708, 1.210780, 1.239759, 2.565932, 2.369879}},
    {{2.997259, 4.703589, 2.369953, 2.853974, 2.818910}},
    {{0.923891, 4.753375, 2.693072, 4.745140, 4.190997}},
    {{2.062553, 3.827637, 3.652119, 3.420851, 4.068756}},
    {{3.989635, 3.712027, 2.963100, 3.483334, 3.696049}},
    {{3.178811, 4.352953, 4.834870, 3.703492, 3.550783}},
    {{1.287988, 3.181042, 1.747977, 4.129652, 4.998983}},
    {{2.138799, 3.540521, 4.269857, 2.281588, 4.461320}},
    {{1.824023, 4.594789, 3.379273, 4.831993, 4.849399}},
}};
constexpr array<array<int, 5>, 9> HPArray_MaxSameCountDown = {{
    {{15, 6, 3, 18, 17}},
    {{25, 13, 9, 16, 6}},
    {{3, 17, 4, 5, 25}},
    {{6, 6, 7, 2, 11}},
    {{14, 7, 4, 16, 12}},
    {{26, 12, 11, 20, 13}},
    {{16, 5, 3, 6, 29}},
    {{8, 10, 8, 13, 10}},
    {{11, 19, 22, 16, 18}},
}};
constexpr array<array<int, 5>, 9> HPArray_FirstEncodeLevel = {{
    {{2, 0, 2, 0, 2}},
    {{0, 0, 1, 1, 1}},
    {{0, 1, 1, 1, 1}},
    {{1, 2, 1, 1, 1}},
    {{2, 1, 2, 1, 1}},
    {{2, 1, 1, 1, 1}},
    {{1, 1, 1, 1, 1}},
    {{2, 1, 1, 1, 1}},
    {{0, 1, 0, 0, 0}},
}};

void UpdateArrayParam() {

    constexpr array<int, 10> N_ranges = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
    constexpr array<double, 6> Mr_ranges = {0.25, 0.35, 0.45, 0.55, 0.65, 0.75};

    double Mr = M / (double)NN;

    int ni = N_ranges.size() - 1;
    REP(i, N_ranges.size() - 1) {
        if (N < N_ranges[i + 1]) {
            ni = i;
            break;
        }
    }

    int mri = Mr_ranges.size() - 2;
    REP(i, Mr_ranges.size() - 1) {
        if (Mr < Mr_ranges[i + 1]) {
            mri = i;
            break;
        }
    }


    HP.EvalCurveCount = HPArray_EvalCurveCount[ni][mri];
    HP.EvalCurveCountSameRate = HPArray_EvalCurveCountSameRate[ni][mri];
    HP.EvalFoodSpread = HPArray_EvalFoodSpread[ni][mri];
    HP.EvalFoodSpreadSameRate = HPArray_EvalFoodSpreadSameRate[ni][mri];
    HP.EvalHasAroundNext = HPArray_EvalHasAroundNext[ni][mri];
    HP.EvalHasAroundNextSameRate = HPArray_EvalHasAroundNextSameRate[ni][mri];
    HP.MaxSameCountDown = HPArray_MaxSameCountDown[ni][mri];
    HP.FirstEncodeLevel = HPArray_FirstEncodeLevel[ni][mri];
}


#undef PC
#undef PI
#undef PD


struct Sim {
    MDeque<u8> bodyCells_;  
    MArr<u8> bodyColors_;   
    NNArr<u8> gridColors_;  
    NNBit bodyExist_;       
    u8 sameCount_;          
    u8 diffCount_;          

    void Init(const NNArr<u8>& gridColors) {
        bodyCells_.clear();
        bodyColors_.clear();
        bodyExist_.reset();
        REP(i, 5) {
            int cell = gs.ToId(0, i);
            bodyCells_.push_front(cell);
            bodyColors_.push(1);
            if (i > 1) {
                bodyExist_.set(cell);
            }
        }
        gridColors_ = gridColors;
        sameCount_ = 5;
        diffCount_ = 0;
    }

    void Clear() {
        bodyCells_.clear();
        bodyColors_.clear();
        gridColors_.clear();
        bodyExist_.reset();
        sameCount_ = 0;
        diffCount_ = 0;
    }
    bool IsValid() const {
        return bodyCells_.size() != 0;
    }

    void MoveTo(u8 newHead, const MArr<u8>& targetBodyColors = targetBodyColors_) {
        bodyCells_.push_front(newHead);

        if (gridColors_[newHead] != 0) {
            const u8 eatColor = gridColors_[newHead];
            const int appendIdx = bodyColors_.size();

            if (eatColor != targetBodyColors_[appendIdx]) {
                ++diffCount_;
            }
            if (appendIdx < targetBodyColors.size()) {
                if (sameCount_ == appendIdx && eatColor == targetBodyColors[sameCount_]) {
                    ++sameCount_;
                }
            }
            bodyColors_.push_back(eatColor);
            gridColors_[newHead] = 0;
            bodyExist_.set(newHead);
        }
        else {

            bodyCells_.pop_back();

            if (bodyExist_.test(newHead)) {
                int colI = -1;
                FOR(i, 1, bodyCells_.size()) {
                    if (bodyCells_[i] == newHead) {
                        colI = i;
                        break;
                    }
                }
                VASSERT(colI != -1);
                RFOR(i, colI + 1, bodyCells_.size()) {
                    if (bodyColors_[i] != targetBodyColors_[i]) {
                        --diffCount_;
                    }
                    int cell = bodyCells_[i];
                    bodyExist_.reset(cell);
                    gridColors_[cell] = bodyColors_[i];
                    bodyCells_.pop_back();
                }
                bodyColors_.resize(colI + 1);
                bodyExist_.reset(bodyCells_[bodyCells_.size() - 2]);
                bodyExist_.set(newHead);

                sameCount_ = min(sameCount_, (u8)(bodyCells_.size()));
            }
            else {
                bodyExist_.reset(bodyCells_[bodyCells_.size() - 2]);
                bodyExist_.set(newHead);
            }
        }


    }

    void Move(Dir dir, const MArr<u8>& targetBodyColors = targetBodyColors_) {
        int head = bodyCells_.front();
        int newHead = gs.dirs(head)[(int)dir];
        VASSERT(newHead != -1);
        VASSERT(newHead != bodyCells_[1]);  
        MoveTo(newHead, targetBodyColors);
    }

    void UpdateCount(const MArr<u8>& targetBodyColors) {
        int sameCount = 0;
        bool stillSame = true;
        REP(i, min(bodyColors_.size(), targetBodyColors.size())) {
            if (bodyColors_[i] == targetBodyColors[i]) {
                if (stillSame) {
                    ++sameCount;
                }
            }
            else {
                stillSame = false;
            }
        }
        sameCount_ = sameCount;
    }


    int CalcRawScore(int turn) const {
        return turn + 10000 * (diffCount_ + 2 * (M - bodyColors_.size()));
    }

    struct SimUndo {
        enum class Kind : u8 {
            Eat,
            MoveOnly,
            Bite,
        };

        Kind kind_;
        u8 newHead_;
        u8 oldTailCell_;
        u8 eatenColor_;

        u8 prevSameCount_;
        u8 prevDiffCount_;
        NNBit prevBodyExist_;

        MArr<u8> poppedCells_;
        MArr<u8> poppedColors_;
    };

    void MoveTo(u8 newHead, SimUndo& undo, const MArr<u8>& targetBodyColors = targetBodyColors_) {
        undo.kind_ = SimUndo::Kind::MoveOnly;
        undo.newHead_ = newHead;
        undo.oldTailCell_ = bodyCells_.back();
        undo.prevSameCount_ = sameCount_;
        undo.prevDiffCount_ = diffCount_;
        undo.prevBodyExist_ = bodyExist_;
        undo.poppedCells_.clear();
        undo.poppedColors_.clear();

        bodyCells_.push_front(newHead);

        if (gridColors_[newHead] != 0) {
            undo.kind_ = SimUndo::Kind::Eat;
            undo.eatenColor_ = gridColors_[newHead];

            const u8 eatColor = gridColors_[newHead];
            const int appendIdx = bodyColors_.size();

            if (eatColor != targetBodyColors_[appendIdx]) ++diffCount_;
            if (appendIdx < targetBodyColors.size() && sameCount_ == appendIdx &&
                eatColor == targetBodyColors[sameCount_]) {
                ++sameCount_;
            }

            bodyColors_.push_back(eatColor);
            gridColors_[newHead] = 0;
            bodyExist_.set(newHead);
            return;
        }

        bodyCells_.pop_back();

        if (bodyExist_.test(newHead)) {
            undo.kind_ = SimUndo::Kind::Bite;

            int colI = -1;
            FOR(i, 1, bodyCells_.size()) {
                if (bodyCells_[i] == newHead) {
                    colI = i;
                    break;
                }
            }

            RFOR(i, colI + 1, bodyCells_.size()) {
                int cell = bodyCells_[i];
                undo.poppedCells_.push_back(cell);
                undo.poppedColors_.push_back(bodyColors_[i]);

                if (bodyColors_[i] != targetBodyColors_[i]) --diffCount_;
                bodyExist_.reset(cell);
                gridColors_[cell] = bodyColors_[i];
                bodyCells_.pop_back();
            }

            bodyColors_.resize(colI + 1);
            bodyExist_.reset(bodyCells_[bodyCells_.size() - 2]);
            bodyExist_.set(newHead);
            sameCount_ = min(sameCount_, (u8)bodyCells_.size());
        }
        else {
            bodyExist_.reset(bodyCells_[bodyCells_.size() - 2]);
            bodyExist_.set(newHead);
        }
    }

    void Revert(const SimUndo& undo) {
        sameCount_ = undo.prevSameCount_;
        diffCount_ = undo.prevDiffCount_;
        bodyExist_ = undo.prevBodyExist_;

        switch (undo.kind_) {
            case SimUndo::Kind::Eat:
                gridColors_[undo.newHead_] = undo.eatenColor_;
                bodyCells_.pop_front();
                bodyColors_.pop();
                break;

            case SimUndo::Kind::MoveOnly:
                bodyCells_.pop_front();
                bodyCells_.push_back(undo.oldTailCell_);
                break;

            case SimUndo::Kind::Bite:
                for (int i = (int)undo.poppedCells_.size() - 1; i >= 0; --i) {
                    gridColors_[undo.poppedCells_[i]] = 0;
                    bodyCells_.push_back(undo.poppedCells_[i]);
                    bodyColors_.push_back(undo.poppedColors_[i]);
                }
                bodyCells_.push_back(undo.oldTailCell_);
                bodyCells_.pop_front();
                break;
        }
    }
};
constexpr int SimSize = sizeof(Sim);

double GetFoodPosCost(u8 cell) {
    static NNArr<double> costs = [] {
        NNArr<double> costs(NN);
        REP(cell, NN) {
            costs[cell] = gs.CalcL1Dist(0, cell);
        }
        return costs;
    }();
    return costs[cell];
}

double GetCenterDists(u8 cell) {
    static NNArr<double> cellDists = [] {
        NNArr<double> dists(NN);
        double center = (N - 1) / 2.0;
        REP(cell, NN) {
            cauto& p = gs.ToPos(cell);
            double dist = square(p.x - center) + square(p.y - center);
            dists[cell] = dist;
        }
        return dists;
    }();
    return cellDists[cell];
}

double GetPosCost(u8 cell) {
    static NNArr<double> costs = [] {
        NNArr<double> costs(NN);
        REP(cell, NN) {
            cauto& p = gs.ToPos(cell);
            int cost = 10000;
            chmin(cost, p.x);
            chmin(cost, p.y);
            chmin(cost, gs.W - 1 - p.x);
            chmin(cost, gs.H - 1 - p.y);
            costs[cell] = cost;
        }
        return costs;
    }();
    return costs[cell];
}


int GetCellNorm2(u8 cell) {
    static NNArr<int> cellNorm2 = [] {
        NNArr<int> cellNorm2(NN);
        REP(cell, NN) {
            cauto& p = gs.ToPos(cell);
            cellNorm2[cell] = p.x * p.x + p.y * p.y;
        }
        return cellNorm2;
    }();
    return cellNorm2[cell];
}

struct VarianceSet {
    i16 sumX_;
    i16 sumY_;
    int sumNorm2_;

    void Init(int cell) {
        cauto& p = gs.ToPos(cell);
        sumX_ = p.x;
        sumY_ = p.y;
        sumNorm2_ = GetCellNorm2(cell);
    }
    void Set(const VarianceSet& prevSet, int cell) {
        cauto& p = gs.ToPos(cell);
        sumX_ = prevSet.sumX_ + p.x;
        sumY_ = prevSet.sumY_ + p.y;
        sumNorm2_ = prevSet.sumNorm2_ + GetCellNorm2(cell);
    }
};

struct OnlineVariance2D {
    i16 sumX;
    i16 sumY;
    int sumNorm2;

    void init() {
        sumX = 0;
        sumY = 0;
        sumNorm2 = 0;
    }

    void add(int cell) {
        cauto& p = gs.ToPos(cell);
        sumX += p.x;
        sumY += p.y;
        sumNorm2 += GetCellNorm2(cell);
    }

    void AddRange(const VarianceSet* beforeBegin, const VarianceSet* endInclusive) {
        if (beforeBegin == endInclusive) {
            return;
        }
        sumX += endInclusive->sumX_ - beforeBegin->sumX_;
        sumY += endInclusive->sumY_ - beforeBegin->sumY_;
        sumNorm2 += endInclusive->sumNorm2_ - beforeBegin->sumNorm2_;
    }

    void remove(int cell) {
        cauto& p = gs.ToPos(cell);
        sumX -= p.x;
        sumY -= p.y;
        sumNorm2 -= GetCellNorm2(cell);
    }

    double variance(int n) const {
        if (n == 0) {
            return 0.0;
        }
        return (n * sumNorm2 - sumX * sumX - sumY * sumY) / double(n * n);
    }
};

struct Hasher {
    NNBox<CBox<u64>> gridHash_;  
    MBox<CBox<u64>> bodyColorHash_;           
    NNBox<u64> easyBodyCellHash_;             
    array<u64, M_max + 1> bodyLengthHash_;    
    array<u64, 4> headDirHash_;               
    array<u64, M_max + 1> curveCountHash_;    
    array<u64, M_max + 1> sameCountHash_;     
    array<u64, M_max + 1> maxSameCountHash_;  

    Hasher() {
        Rng rng;

        REP(cell, NN) {
            REP(color, C + 1) {
                gridHash_[cell][color] = rng.Get64();
            }
        }


        REP(bodyIdx, M) {
            REP(color, C + 1) {
                bodyColorHash_[bodyIdx][color] = rng.Get64();
            }
        }

        REP(cell, NN) {
            easyBodyCellHash_[cell] = rng.Get64();
        }

        REP(length, M + 1) {
            bodyLengthHash_[length] = rng.Get64();
        }
        headDirHash_.fill(0);
        REP(dir, 4) {
            headDirHash_[dir] = rng.Get64();
        }
        REP(curveCount, M + 1) {
            curveCountHash_[curveCount] = rng.Get64();
        }
        REP(sameCount, M + 1) {
            sameCountHash_[sameCount] = rng.Get64();
        }
        REP(maxSameCount, M + 1) {
            maxSameCountHash_[maxSameCount] = rng.Get64();
        }
    }



};

#define USE_BODY_EXIST 0
#define USE_BODY_VARIANCE 0
#define USE_FOOD_CENTER_DIST 0
#define USE_HEAD_POS_COST 0
#define USE_BODY_BLOCK_COST 0  
#define USE_POWER_BIT 0
#define USE_MAX_SAME_COUNT 0
#define USE_CUT_CONTINUE 0         
#define USE_BODY_COLOR_HASH 1      
#define USE_GRID_COLOR_HASH 1      
#define USE_EASY_BODY_CELL_HASH 0  
#define USE_CURVE_DIR_COUNT 0
#define USE_FOOD_POS_COST 0

constexpr int SimExCellBuf = 1600;

template <bool UseColorHash>
struct SimExColorHashState {};

template <>
struct SimExColorHashState<true> {
    u64 bodyColorHash_;  
    u64 gridColorHash_;  
};

template <bool UseColorHash>
struct SimExColorHashDeltaF {};

template <>
struct SimExColorHashDeltaF<true> {
    u64 bodyColorHash_;  
    u64 gridColorHash_;  
};

template <bool UseColorHash>
struct SimExColorHashDeltaB {};

template <>
struct SimExColorHashDeltaB<true> {
    u64 prevBodyColorHash_;  
    u64 prevGridColorHash_;  
};

template <bool UseColorHash>
struct SimExT : SimExColorHashState<UseColorHash> {
    array<u8, SimExCellBuf> cells_;                  
    array<Dir, SimExCellBuf> dirs_;                  
    array<u16, SimExCellBuf> curvePrefixes_;         
    array<VarianceSet, SimExCellBuf> varianceSets_;  
    MArr<u8> bodyColors_;            
    NNArr<u8> cellsColor_;           
    NNArr<i16> cellsTurn_;           
    int head_;                       
    int tail_;                       
    int turn_;                       
    u8 sameCount_;                   
    u8 diffCount_;                   
    u8 curveCount_;                  
    OnlineVariance2D foodVariance_;  

    struct DeltaF : SimExColorHashDeltaF<UseColorHash> {
        u8 mode_;                        
        u8 cell_;                        
        Dir dir_;                        
        u8 sameCount_;                   
        u8 diffCount_;                   
        u8 curveCount_;                  
        OnlineVariance2D foodVariance_;  
    };

    struct DeltaB : SimExColorHashDeltaB<UseColorHash> {
        i16 prevCellTurn_;  
        i16 prevTail_;      
        u8 mode_;           
        u8 prevSameCount_;
        u8 prevDiffCount_;
        u8 prevCurveCount_;
        OnlineVariance2D prevFoodVariance_;
    };

    void Init(const Hasher& hasher) {
        bodyColors_.clear();
        cellsTurn_.assign(NN, -10000);
        cellsColor_ = startGridColors_;

        REP(y, 5) {
            int cell = gs.ToId(0, y);
            cells_[y] = cell;
            dirs_[y] = Dir::D;
            bodyColors_.push_back(1);
            cellsTurn_[cell] = y;
            if (y == 0) {
                varianceSets_[y].Init(cell);
                curvePrefixes_[y] = 0;
            }
            else {
                cauto& prevSet = varianceSets_[y - 1];
                varianceSets_[y].Set(prevSet, cell);
                curvePrefixes_[y] = curvePrefixes_[y - 1] + (y >= 2 && dirs_[y - 1] != dirs_[y] ? 1 : 0);
            }
        }
        head_ = 4;
        tail_ = 0;
        turn_ = 5;
        sameCount_ = 5;
        diffCount_ = 0;
        curveCount_ = 0;
        foodVariance_.init();
        REP(i, NN) {
            if (cellsColor_[i] != 0) {
                foodVariance_.add(i);
            }
        }
        if constexpr (UseColorHash) {
            this->bodyColorHash_ = 0;
            REP(i, bodyColors_.size()) {
                int color = bodyColors_[i];
                this->bodyColorHash_ ^= hasher.bodyColorHash_[i][color];
            }
        }
        if constexpr (UseColorHash) {
            this->gridColorHash_ = 0;
            REP(i, NN) {
                int color = cellsColor_[i];
                if (color != 0) {
                    this->gridColorHash_ ^= hasher.gridHash_[i][color];
                }
            }
        }
    }

    bool Move(const Hasher& hasher, u8 newCell, Dir newDir, int maxDown, DeltaF& deltaF, DeltaB& deltaB) {

        const int newCellTurn = cellsTurn_[newCell];

        bool isCut = newCellTurn >= tail_ + 2;
        u8 newBodyCount;  
        u8 newSameCount;  
        if (isCut) {
            int newTail = newCellTurn;

            newBodyCount = head_ + 2 - newTail;
            newSameCount = sameCount_;
            if (newBodyCount < sameCount_) {
                newSameCount = newBodyCount;
            }
            u8 downCount = sameCount_ - newSameCount;
            if (downCount > maxDown) {
                return false;
            }
        }

        deltaB.prevCellTurn_ = newCellTurn;
        deltaB.prevTail_ = tail_;
        deltaB.prevSameCount_ = sameCount_;
        deltaB.prevDiffCount_ = diffCount_;
        deltaB.prevCurveCount_ = curveCount_;
        deltaB.prevFoodVariance_ = foodVariance_;
        if constexpr (UseColorHash) {
            deltaB.prevBodyColorHash_ = this->bodyColorHash_;
            deltaB.prevGridColorHash_ = this->gridColorHash_;
        }

        if (isCut) {
            deltaB.mode_ = 2;
            deltaF.mode_ = 2;

            int newTail = newCellTurn;


            u8* cellsPtr = &cells_[tail_ + 1];
            u8* bodyColorsPtr = &bodyColors_[head_ - tail_];
            u8* targetBodyColorsPtr = &targetBodyColors_[head_ - tail_];
            const CBox<u64>* bodyColorHashPtr = &hasher.bodyColorHash_[head_ - tail_];
            FOR(t, tail_, newTail - 1) {
                u8 cell = *cellsPtr++;
                u8 color = *bodyColorsPtr--;
                u8 targetColor = *targetBodyColorsPtr--;
                if (color != targetColor) {
                    --diffCount_;
                }
                cellsColor_[cell] = color;
                if constexpr (UseColorHash) {
                    this->bodyColorHash_ ^= (*bodyColorHashPtr)[color];
                    --bodyColorHashPtr;
                    this->gridColorHash_ ^= hasher.gridHash_[cell][color];
                }
            }
            foodVariance_.AddRange(&varianceSets_[tail_], &varianceSets_[newTail - 1]);
            curveCount_ -= curvePrefixes_[newTail + 1] - curvePrefixes_[tail_ + 1];

            bodyColors_.resize(newBodyCount);
            sameCount_ = newSameCount;
            tail_ = newTail;
        }
        else if (cellsColor_[newCell] != 0) {
            deltaB.mode_ = 1;
            deltaF.mode_ = 1;

            u8 color = cellsColor_[newCell];
            bool appendSameCount = false;
            if (color == targetBodyColors_[head_ - tail_ + 1]) {
                if (sameCount_ == head_ - tail_ + 1) {
                    ++sameCount_;
                    appendSameCount = true;
                }
            }
            else {
                ++diffCount_;
            }

            bodyColors_.push(color);
            cellsColor_[newCell] = 0;
            foodVariance_.remove(newCell);
            if constexpr (UseColorHash) {
                this->bodyColorHash_ ^= hasher.bodyColorHash_[bodyColors_.size() - 1][color];
                this->gridColorHash_ ^= hasher.gridHash_[newCell][color];
            }
        }
        else {
            deltaB.mode_ = 0;
            deltaF.mode_ = 0;

            if (dirs_[tail_ + 1] != dirs_[tail_ + 2]) {
                --curveCount_;
            }
            ++tail_;
        }

        ++head_;
        cells_[head_] = newCell;
        dirs_[head_] = newDir;
        varianceSets_[head_].Set(varianceSets_[head_ - 1], newCell);
        curvePrefixes_[head_] = curvePrefixes_[head_ - 1] + (dirs_[head_ - 1] != newDir ? 1 : 0);
        cellsTurn_[newCell] = turn_;

        if (newDir != dirs_[head_ - 1]) {
            ++curveCount_;
        }

        ++turn_;

        deltaF.cell_ = newCell;
        deltaF.dir_ = newDir;
        deltaF.sameCount_ = sameCount_;
        deltaF.diffCount_ = diffCount_;
        deltaF.curveCount_ = curveCount_;
        deltaF.foodVariance_ = foodVariance_;
        if constexpr (UseColorHash) {
            deltaF.bodyColorHash_ = this->bodyColorHash_;
            deltaF.gridColorHash_ = this->gridColorHash_;
        }

        return true;
    }

    void Apply(const DeltaF& deltaF) {
        u8 newCell = deltaF.cell_;

        if (deltaF.mode_ == 2) {
            int newTail = cellsTurn_[newCell];
            u8* cellsPtr = &cells_[tail_ + 1];
            u8* bodyColorsPtr = &bodyColors_[head_ - tail_];
            FOR(t, tail_, newTail - 1) {
                u8 cell = *cellsPtr++;
                cellsColor_[cell] = *bodyColorsPtr--;
            }
            bodyColors_.resize(head_ + 2 - newTail);
            tail_ = newTail;
        }
        else if (deltaF.mode_ == 1) {
            bodyColors_.push(cellsColor_[newCell]);
            cellsColor_[newCell] = 0;
        }
        else {
            ++tail_;
        }

        ++head_;
        cells_[head_] = newCell;
        dirs_[head_] = deltaF.dir_;
        varianceSets_[head_].Set(varianceSets_[head_ - 1], newCell);
        curvePrefixes_[head_] = curvePrefixes_[head_ - 1] + (dirs_[head_ - 1] != deltaF.dir_ ? 1 : 0);
        cellsTurn_[newCell] = turn_;

        ++turn_;

        sameCount_ = deltaF.sameCount_;
        diffCount_ = deltaF.diffCount_;
        curveCount_ = deltaF.curveCount_;
        foodVariance_ = deltaF.foodVariance_;
        if constexpr (UseColorHash) {
            this->bodyColorHash_ = deltaF.bodyColorHash_;
            this->gridColorHash_ = deltaF.gridColorHash_;
        }

    }

    void Revert(const DeltaB& deltaB) {
        auto newCell = cells_[head_];

        --turn_;
        cellsTurn_[newCell] = deltaB.prevCellTurn_;
        --head_;

        if (deltaB.mode_ == 0) {
            --tail_;
        }
        else if (deltaB.mode_ == 1) {
            u8 color = bodyColors_.back();

            cellsColor_[newCell] = color;
            bodyColors_.pop();
        }
        else {
            int newTail = tail_;
            tail_ = deltaB.prevTail_;
            bodyColors_.resize(head_ + 1 - tail_);

            u8* cellsPtr = &cells_[tail_ + 1];
            u8* bodyColorsPtr = &bodyColors_[head_ - tail_];
            FOR(t, tail_, newTail - 1) {
                u8 cell = *cellsPtr++;
                *bodyColorsPtr-- = cellsColor_[cell];
                cellsColor_[cell] = 0;

            }
        }

        sameCount_ = deltaB.prevSameCount_;
        diffCount_ = deltaB.prevDiffCount_;
        curveCount_ = deltaB.prevCurveCount_;
        foodVariance_ = deltaB.prevFoodVariance_;
        if constexpr (UseColorHash) {
            this->bodyColorHash_ = deltaB.prevBodyColorHash_;
            this->gridColorHash_ = deltaB.prevGridColorHash_;
        }

    }

    void Validate(Hasher& hasher) const {
        int sameCount = 0;
        int diffCount = 0;
        bool diffed = false;
        NNBit bodyExist;
        FOR(t, tail_ + 2, head_ + 1) {
            bodyExist.set(cells_[t]);
        }
        RFOR(t, tail_, head_ + 1) {
            u8 color = bodyColors_[head_ - t];
            if (color == targetBodyColors_[head_ - t]) {
                if (!diffed) {
                    ++sameCount;
                }
            }
            else {
                ++diffCount;
                diffed = true;
            }
        }
        VASSERT(sameCount == sameCount_);
        VASSERT(diffCount == diffCount_);

        int curveCount = 0;
        array<u8, 2> curveDirsCount = {0, 0};
        Dir prevDir = gs.CalcDir(cells_[tail_], cells_[tail_ + 1]);
        FOR(t, tail_ + 1, head_) {
            Dir newDir = gs.CalcDir(cells_[t], cells_[t + 1]);
            if (prevDir != newDir) {
                ++curveCount;
            }
            prevDir = newDir;
        }
        VASSERT(curveCount == curveCount_);

        FOR(t, 1, head_ + 1) {
            Dir dir = gs.CalcDir(cells_[t - 1], cells_[t]);
            VASSERT(dir == dirs_[t]);
        }

        OnlineVariance2D foodVariance;
        foodVariance.init();
        double foodCenterDists = 0.0;
        REP(i, NN) {
            if (cellsColor_[i] != 0) {
                foodVariance.add(i);
                foodCenterDists += GetCenterDists(i);
            }
        }
        int n = M - (head_ + 1 - tail_);
        VASSERT(fabs(foodVariance.variance(n) - foodVariance_.variance(n)) < 1e-6);

        if constexpr (UseColorHash) {
            u64 bodyColorHash = 0;
            REP(i, bodyColors_.size()) {
                int color = bodyColors_[i];
                bodyColorHash ^= hasher.bodyColorHash_[i][color];
            }
            VASSERT(bodyColorHash == this->bodyColorHash_);
        }
        if constexpr (UseColorHash) {
            u64 gridColorHash = 0;
            REP(i, NN) {
                int color = cellsColor_[i];
                if (color != 0) {
                    gridColorHash ^= hasher.gridHash_[i][color];
                }
            }
            VASSERT(gridColorHash == this->gridColorHash_);
        }
    }

    int GetBodyLength() const {
        return head_ - tail_ + 1;
    }

    u8 GetHeadCell() const {
        return cells_[head_];
    }

    u8 GetBeforeHeadCell() const {
        return cells_[head_ - 1];
    }

    u8 GetBodyCell(int i) const {
        return cells_[head_ - i];
    }

    u8 GetTailCell() const {
        return cells_[tail_];
    }

    Dir GetHeadDir() const {
        return gs.CalcDir(cells_[head_ - 1], cells_[head_]);
    }

    int CalcRawScore() const {
        return (turn_ - 5) + 10000 * (diffCount_ + 2 * (M - bodyColors_.size()));
    }

    void GetBodyCells(MDeque<u8>& bodyCells) const {
        bodyCells.clear();
        RFOR(i, tail_, head_ + 1) {
            bodyCells.push_back(cells_[i]);
        }
    }

    bool IsCollisionBody(u8 cell) const {
        return cellsTurn_[cell] >= head_ + 1 - sameCount_ + 2;
    }

};
using SimEx = SimExT<true>;
using SimExNoColorHash = SimExT<false>;

constexpr int SimExSize = sizeof(SimEx);
constexpr int SimExDeltaFSize = sizeof(SimEx::DeltaF);
constexpr int SimExDeltaBSize = sizeof(SimEx::DeltaB);
constexpr int SimExNoColorHashSize = sizeof(SimExNoColorHash);
constexpr int SimExNoColorHashDeltaFSize = sizeof(SimExNoColorHash::DeltaF);
constexpr int SimExNoColorHashDeltaBSize = sizeof(SimExNoColorHash::DeltaB);

static_assert(SimExNoColorHashDeltaFSize < SimExDeltaFSize);
static_assert(SimExNoColorHashDeltaBSize < SimExDeltaBSize);


struct IOServer {
   private:
   public:
    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        is >> N;
        timer.Init();  
        NN = N * N;
        is >> M >> C;

        targetBodyColors_.resize(M);
        startGridColors_.resize(NN);

        int v;
        REP(i, M) {
            is >> v;
            targetBodyColors_[i] = v;
        }
        REP(i, NN) {
            is >> v;
            startGridColors_[i] = v;
        }
        gs.Init(N, N);
    }


    void Output(const dvec<Dir>& result) {
        ostream& os = cout;
        REP(i, result.size()) {
            os << DirToChar(result[i]) << '\n';
        }
        os.flush();
    }

    void Finalize() {
    }
};
IOServer server;


struct PathFinder {
    NNQue<u8> que_;
    NNArr<u8> dists_;
    NNArr<int> froms_;

    void UpdatePath(const MDeque<u8>& bodyCells, int requireBodySize, const NNArr<bool>& fixed) {
        que_.clear();
        dists_.assign(NN, 255);
        froms_.resize(NN);

        REP(i, requireBodySize - 2) {
            int cell = bodyCells[i];
            dists_[cell] = 0;
            froms_[cell] = -1;
        }

        {
            auto startCell = bodyCells.front();
            auto prevCell = bodyCells[1];

            que_.push(startCell);
            dists_[startCell] = 0;
            froms_[startCell] = -1;

            dists_[prevCell] = 0;
            froms_[prevCell] = -1;
        }

        while (!que_.empty()) {
            auto p = que_.pop();
            auto nd = dists_[p] + 1;
            for (auto n : gs.around(p)) {
                if (fixed[n]) {
                    continue;
                }

                if (nd < dists_[n]) {
                    dists_[n] = nd;
                    froms_[n] = p;
                    que_.push(n);
                }
            }
        }
    }

    void UpdatePathToFood(const NNArr<u8>& gridColors, const MDeque<u8>& bodyCells, int requireBodySize,
                          const NNArr<bool>& fixed) {
        que_.clear();
        dists_.assign(NN, 255);
        froms_.resize(NN);

        REP(i, requireBodySize - 2) {
            int cell = bodyCells[i];
            dists_[cell] = 0;
            froms_[cell] = -1;
        }

        {
            auto startCell = bodyCells.front();
            auto prevCell = bodyCells[1];

            que_.push(startCell);
            dists_[startCell] = 0;
            froms_[startCell] = -1;

            dists_[prevCell] = 0;
            froms_[prevCell] = -1;
        }

        while (!que_.empty()) {
            auto p = que_.pop();
            auto nd = dists_[p] + 1;
            for (auto n : gs.around(p)) {
                if (fixed[n]) {
                    continue;
                }

                if (nd < dists_[n]) {
                    dists_[n] = nd;
                    froms_[n] = p;

                    if (gridColors[n] == 0) {
                        que_.push(n);
                    }
                }
            }
        }
    }

    u8 GetDist(u8 cell) const {
        return dists_[cell];
    }

    bool GetPath(NNArr<u8>& path, u8 targetCell) {
        if (dists_[targetCell] == 255 || froms_[targetCell] == -1) {
            return false;
        }

        path.clear();
        int cell = targetCell;
        while (cell != -1) {
            path.push(cell);
            cell = froms_[cell];
        }
        reverse(path.begin(), path.end());
        return true;
    }
};

bool CalcCutPath(const MDeque<u8>& bodyCells, int cutTurn, int cutBodyIdx, const NNArr<bool>& fixed, NNArr<u8>& path) {
    VASSERT(cutTurn > 0);

    NNArr<u8> dists;
    dists.assign(NN, 255);

    REP(i, bodyCells.size() - 2) {
        dists[bodyCells[i]] = bodyCells.size() - 2 - i;
    }

    int cutDist = bodyCells.size() - 2 + cutTurn;

    path.clear();

    auto dfs = [&](auto&& dfs, u8 p) -> bool {
        auto nd = dists[p] + 1;
        for (auto n : gs.around(p)) {
            if (fixed[n]) {
                continue;
            }

            if (nd == cutDist) {
                if (dists[n] < 255) {
                    if (dists[n] == dists[p] - 1) {
                        continue;
                    }

                    int diff = cutDist - dists[n] + 1;
                    if (diff <= cutBodyIdx) {
                        path.push(n);
                        return true;
                    }
                }
            }
            else if (nd < dists[n]) {
                auto backupDist = dists[n];
                dists[n] = nd;
                path.push(n);
                if (dfs(dfs, n)) {
                    return true;
                }
                path.pop();
                dists[n] = backupDist;
            }
        }
        return false;
    };
    path.push(bodyCells.front());
    return dfs(dfs, bodyCells.front());
}


bool FindPathToTargetColor(u8 startCell, u8 prevCell, u8 targetColor, const NNArr<bool>& fixed,
                           const NNArr<u8>& gridColors, NNArr<u8>& path) {
    const int stateCount = NN * NN;
    static vector<int> fromStates;
    static vector<char> visited;
    fromStates.assign(stateCount, -1);
    visited.assign(stateCount, false);

    queue<int> que;
    auto EncodeState = [](int prev, int cur) -> int { return prev * NN + cur; };
    auto DecodePrev = [](int state) -> int { return state / NN; };
    auto DecodeCur = [](int state) -> int { return state % NN; };

    int endState = -1;
    int startState = EncodeState(prevCell, startCell);
    que.push(startState);
    visited[startState] = true;

    while (!que.empty()) {
        int state = que.front();
        que.pop();

        int prev = DecodePrev(state);
        int cur = DecodeCur(state);
        for (int next : gs.around(cur)) {
            if (next == prev) {
                continue;
            }
            if (fixed[next]) {
                continue;
            }

            int nextState = EncodeState(cur, next);
            if (visited[nextState]) {
                continue;
            }

            visited[nextState] = true;
            fromStates[nextState] = state;
            que.push(nextState);

            if (gridColors[next] == targetColor) {
                endState = nextState;
                break;
            }
        }
        if (endState != -1) {
            break;
        }
    }
    if (endState == -1) {
        return false;
    }

    path.clear();
    {
        int state = endState;
        while (state != -1) {
            path.push(DecodeCur(state));
            state = fromStates[state];
        }
        reverse(path.begin(), path.end());
    }
    return true;
}

bool FindPathToCell(const NNDeque<u8>& bodyCells, u8 targetCell, const NNArr<bool>& fixed, NNArr<u8>& path) {
    if (bodyCells.size() < 2) {
        return false;
    }

    u8 startCell = bodyCells.front();
    u8 prevCell = bodyCells[1];

    NNArr<u8> movableTurns(NN, 0);
    {
        int turn = 1;
        RREP(i, bodyCells.size() - 2) {
            int cell = bodyCells[i];
            movableTurns[cell] = turn;
            ++turn;
        }
    }

    const int stateCount = NN * NN;
    auto EncodeState = [](int prev, int cur) -> int { return prev * NN + cur; };
    auto DecodePrev = [](int state) -> int { return state / NN; };
    auto DecodeCur = [](int state) -> int { return state % NN; };

    int maxMovableTurn = 0;
    REP(cell, NN) {
        chmax(maxMovableTurn, (int)movableTurns[cell]);
    }
    const int maxSearchTurn = min(255, maxMovableTurn + 4 * NN);

    vector<vector<int>> states(maxSearchTurn + 1);
    vector<vector<int>> parentIndices(maxSearchTurn + 1);
    vector<int> nextStateIndex(stateCount, -1);
    vector<int> touchedStates;

    int startState = EncodeState(prevCell, startCell);
    states[0].push_back(startState);
    parentIndices[0].push_back(-1);

    int endTurn = -1;
    int endIndex = -1;

    REP(turn, maxSearchTurn) {
        auto& curStates = states[turn];
        auto& nextStates = states[turn + 1];
        auto& nextParents = parentIndices[turn + 1];
        nextStates.clear();
        nextParents.clear();

        REP(parentIndex, curStates.size()) {
            int state = curStates[parentIndex];
            int prev = DecodePrev(state);
            int cur = DecodeCur(state);
            for (int next : gs.around(cur)) {
                if (next == prev) {
                    continue;
                }
                if (fixed[next]) {
                    continue;
                }

                if (turn < movableTurns[next]) {
                    continue;
                }

                int nextState = EncodeState(cur, next);
                if (nextStateIndex[nextState] != -1) {
                    continue;
                }

                nextStateIndex[nextState] = nextStates.size();
                touchedStates.push_back(nextState);
                nextStates.push_back(nextState);
                nextParents.push_back(parentIndex);

                if (next == targetCell) {
                    endTurn = turn + 1;
                    endIndex = nextStateIndex[nextState];
                    break;
                }
            }
            if (endTurn != -1) {
                break;
            }
        }
        for (int state : touchedStates) {
            nextStateIndex[state] = -1;
        }
        touchedStates.clear();

        if (endTurn != -1) {
            break;
        }
    }
    if (endTurn == -1) {
        return false;
    }

    path.clear();
    {
        int turn = endTurn;
        int index = endIndex;
        while (turn >= 0) {
            int state = states[turn][index];
            path.push(DecodeCur(state));
            index = parentIndices[turn][index];
            --turn;
        }
        reverse(path.begin(), path.end());
    }
    return true;
}

bool FindBitePath(const NNDeque<u8>& bodyCells, int biteTurn, int bodyIndex, const NNArr<bool>& fixed,
                  NNArr<u8>& path) {
    if (bodyCells.size() < 2) {
        return false;
    }
    if (biteTurn < 3) {
        return false;
    }
    if (bodyIndex < 0 || bodyIndex >= bodyCells.size()) {
        return false;
    }

    NNArr<u8> movableTurns(NN, 0);
    {
        int turn = 1;
        RREP(i, max(0, bodyIndex - 2)) {
            int cell = bodyCells[i];
            movableTurns[cell] = turn;
            ++turn;
        }
    }

    static NNCheck moved;
    moved.Clear();

    const int prefixTurn = biteTurn - 3;
    const int stateCount = NN * NN;
    auto EncodeState = [](int prev, int cur) -> int { return prev * NN + cur; };

    auto RotateBite = [&](int turn, u8 startCell, u8 startPrevCell) -> bool {
        Dir front = gs.CalcDir(startPrevCell, startCell);
        for (int rotateDir : {-1, 1}) {
            u8 cell = startCell;
            u8 prevCell = startPrevCell;
            Dir curDir = front;
            CapArr<u8, 3> rotatePath;
            REP(r, 3) {
                curDir = (Dir)(((int)curDir + rotateDir + 4) % 4);
                int nextCell = gs.dirs(cell)[(int)curDir];
                if (nextCell == -1) {
                    break;
                }
                if (nextCell == prevCell) {
                    break;
                }
                if (fixed[nextCell]) {
                    break;
                }
                if (turn + r < movableTurns[nextCell]) {
                    break;
                }
                rotatePath.push(nextCell);

                if (r < 2) {
                    if (moved[nextCell]) {
                        break;
                    }
                }
                else {
                    for (u8 c : rotatePath) {
                        path.push(c);
                    }
                    return true;
                }
                prevCell = cell;
                cell = nextCell;
            }
        }
        return false;
    };

    auto CanRotateWithoutMoved = [&](int turn, u8 startCell, u8 startPrevCell) -> bool {
        Dir front = gs.CalcDir(startPrevCell, startCell);
        for (int rotateDir : {-1, 1}) {
            u8 cell = startCell;
            u8 prevCell = startPrevCell;
            Dir curDir = front;
            bool ok = true;
            REP(r, 3) {
                curDir = (Dir)(((int)curDir + rotateDir + 4) % 4);
                int nextCell = gs.dirs(cell)[(int)curDir];
                if (nextCell == -1) {
                    ok = false;
                    break;
                }
                if (nextCell == prevCell) {
                    ok = false;
                    break;
                }
                if (fixed[nextCell]) {
                    ok = false;
                    break;
                }
                if (turn + r < movableTurns[nextCell]) {
                    ok = false;
                    break;
                }
                prevCell = cell;
                cell = nextCell;
            }
            if (ok) {
                return true;
            }
        }
        return false;
    };

    vector<vector<char>> canReach(prefixTurn + 1, vector<char>(stateCount, false));
    REP(prev, NN) {
        REP(cur, NN) {
            if (prev == cur) {
                continue;
            }
            if (gs.CalcL1Dist(prev, cur) != 1) {
                continue;
            }
            int state = EncodeState(prev, cur);
            canReach[prefixTurn][state] = CanRotateWithoutMoved(prefixTurn, (u8)cur, (u8)prev);
        }
    }
    for (int turn = prefixTurn - 1; turn >= 0; --turn) {
        REP(prev, NN) {
            REP(cur, NN) {
                if (prev == cur) {
                    continue;
                }
                if (gs.CalcL1Dist(prev, cur) != 1) {
                    continue;
                }
                int state = EncodeState(prev, cur);
                for (int next : gs.around(cur)) {
                    if (next == prev) {
                        continue;
                    }
                    if (fixed[next]) {
                        continue;
                    }
                    if (turn < movableTurns[next]) {
                        continue;
                    }
                    int nextState = EncodeState(cur, next);
                    if (canReach[turn + 1][nextState]) {
                        canReach[turn][state] = true;
                        break;
                    }
                }
            }
        }
    }

    auto dfs = [&](auto&& dfs, int turn, u8 cell, u8 prevCell) -> bool {
        int state = EncodeState(prevCell, cell);
        if (!canReach[turn][state]) {
            return false;
        }

        if (turn >= prefixTurn) {
            return RotateBite(turn, cell, prevCell);
        }

        REP(di, 4) {
            int nextCell = gs.dirs(cell)[di];
            if (nextCell == -1) {
                continue;
            }
            if (nextCell == prevCell) {
                continue;
            }
            if (fixed[nextCell]) {
                continue;
            }
            if (turn < movableTurns[nextCell]) {
                continue;
            }
            if (moved[nextCell]) {
                continue;
            }
            int nextState = EncodeState(cell, nextCell);
            if (!canReach[turn + 1][nextState]) {
                continue;
            }

            moved.set(nextCell);
            path.push(nextCell);
            if (dfs(dfs, turn + 1, nextCell, cell)) {
                return true;
            }
            path.pop();
            moved.reset(nextCell);
        }
        return false;
    };

    path.clear();
    path.push(bodyCells.front());
    if (!dfs(dfs, 0, bodyCells.front(), bodyCells[1])) {
        path.clear();
        return false;
    }
    return true;
}

struct BitOutGuard {
    NNBit leftGuard_;
    NNBit rightGuard_;
    NNBit bottomGuard_;

    BitOutGuard() {
        leftGuard_.set();
        rightGuard_.set();
        bottomGuard_.set();

        REP(i, N) {
            leftGuard_.reset(gs.ToId(0, i));
            rightGuard_.reset(gs.ToId(N - 1, i));
            bottomGuard_.reset(gs.ToId(i, N - 1));
        }
    }
};

struct BitPathFinder {
    array<NNBit, 256> turnsBit_;
    array<NNBit, 256> expandableTurnsBit_;  
    NNArr<u8> dists_;                       
    int maxTurn_;

    void Update(const MDeque<u8>& body, int requireBodySize, int maxTurn) {
        VASSERT(requireBodySize >= 5);
        static BitOutGuard outGuard;

        dists_.assign(NN, 255);

        NNBit movable;
        movable.set();
        REP(i, requireBodySize - 2) {
            movable.reset(body[i]);
        }

        {
            auto& cur = turnsBit_[0];
            cur.reset();
            cur.set(body.front());
            dists_[body.front()] = 0;
        }

        NNBit reachable;
        reachable = turnsBit_[0];

        maxTurn_ = min(maxTurn + 1, 256);
        REP(t, min(maxTurn, 255)) {
            cauto& cur = turnsBit_[t];
            expandableTurnsBit_[t] = cur;
            auto& next = turnsBit_[t + 1];
            next.reset();
            next |= (cur & outGuard.rightGuard_) << 1;
            next |= (cur & outGuard.leftGuard_) >> 1;
            next |= (cur & outGuard.bottomGuard_) << N;
            next |= cur >> N;
            next &= movable;

            next &= ~reachable;

            if (next.none()) {
                maxTurn_ = t + 1;
                break;
            }
            reachable |= next;

            for (auto i : next) {
                dists_[i] = t + 1;
            }


            if (requireBodySize - 3 - t >= 0) {
                movable.set(body[requireBodySize - 3 - t]);
            }
        }
    }
    void Update(const SimEx& sim, int requireBodySize, int maxTurn) {
        VASSERT(requireBodySize >= 5);
        static BitOutGuard outGuard;

        dists_.assign(NN, 255);

        NNBit movable;
        movable.set();
        REP(i, requireBodySize - 2) {
            movable.reset(sim.GetBodyCell(i));
        }

        {
            auto& cur = turnsBit_[0];
            cur.reset();
            cur.set(sim.GetHeadCell());
            dists_[sim.GetHeadCell()] = 0;
        }

        NNBit reachable;
        reachable = turnsBit_[0];

        maxTurn_ = min(maxTurn + 1, 256);
        REP(t, min(maxTurn, 255)) {
            cauto& cur = turnsBit_[t];
            expandableTurnsBit_[t] = cur;
            auto& next = turnsBit_[t + 1];
            next.reset();
            next |= (cur & outGuard.rightGuard_) << 1;
            next |= (cur & outGuard.leftGuard_) >> 1;
            next |= (cur & outGuard.bottomGuard_) << N;
            next |= cur >> N;
            next &= movable;

            next &= ~reachable;

            if (next.none()) {
                maxTurn_ = t + 1;
                break;
            }
            reachable |= next;

            for (auto i : next) {
                dists_[i] = t + 1;
            }


            if (requireBodySize - 3 - t >= 0) {
                movable.set(sim.GetBodyCell(requireBodySize - 3 - t));
            }
        }
    }

    void CalcReachable(const SimEx& sim, int requireBodySize, int maxTurn, NNBit& reachable) {
        VASSERT(requireBodySize >= 5);
        static BitOutGuard outGuard;


        NNBit movable;
        movable.set();
        REP(i, requireBodySize - 2) {
            movable.reset(sim.GetBodyCell(i));
        }

        {
            auto& cur = turnsBit_[0];
            cur.reset();
            cur.set(sim.GetHeadCell());
        }

        reachable = turnsBit_[0];

        maxTurn_ = min(maxTurn + 1, 256);
        REP(t, min(maxTurn, 255)) {
            cauto& cur = turnsBit_[t];
            expandableTurnsBit_[t] = cur;
            auto& next = turnsBit_[t + 1];
            next.reset();
            next |= (cur & outGuard.rightGuard_) << 1;
            next |= (cur & outGuard.leftGuard_) >> 1;
            next |= (cur & outGuard.bottomGuard_) << N;
            next |= cur >> N;
            next &= movable;

            next &= ~reachable;

            if (next.none()) {
                maxTurn_ = t + 1;
                break;
            }
            reachable |= next;



            if (requireBodySize - 3 - t >= 0) {
                movable.set(sim.GetBodyCell(requireBodySize - 3 - t));
            }
        }
    }

    void UpdateWithFix(const MDeque<u8>& body, int requireBodySize, int maxTurn, const NNBit& fixed) {
        VASSERT(requireBodySize >= 5);
        static BitOutGuard outGuard;

        dists_.assign(NN, 255);

        NNBit movable;
        movable = ~fixed;
        REP(i, requireBodySize - 2) {
            movable.reset(body[i]);
        }

        {
            auto& cur = turnsBit_[0];
            cur.reset();
            cur.set(body.front());
            dists_[body.front()] = 0;
        }

        NNBit reachable;
        reachable = turnsBit_[0];

        maxTurn_ = min(maxTurn + 1, 256);
        REP(t, min(maxTurn, 255)) {
            cauto& cur = turnsBit_[t];
            expandableTurnsBit_[t] = cur;
            auto& next = turnsBit_[t + 1];
            next.reset();
            next |= (cur & outGuard.rightGuard_) << 1;
            next |= (cur & outGuard.leftGuard_) >> 1;
            next |= (cur & outGuard.bottomGuard_) << N;
            next |= cur >> N;
            next &= movable;

            next &= ~reachable;

            if (next.none()) {
                maxTurn_ = t + 1;
                break;
            }
            reachable |= next;

            for (auto i : next) {
                dists_[i] = t + 1;
            }


            if (requireBodySize - 3 - t >= 0) {
                movable.set(body[requireBodySize - 3 - t]);
            }
        }
    }

    void UpdateToFood(const NNArr<u8>& gridColors, const MDeque<u8>& body, int requireBodySize, int maxTurn) {
        VASSERT(requireBodySize >= 5);
        static BitOutGuard outGuard;

        dists_.assign(NN, 255);

        NNBit outable;
        outable.set();
        REP(i, NN) {
            if (gridColors[i] != 0) {
                outable.reset(i);
            }
        }

        NNBit movable;
        movable.set();
        REP(i, requireBodySize - 2) {
            movable.reset(body[i]);
        }

        {
            auto& cur = turnsBit_[0];
            cur.reset();
            cur.set(body.front());
            dists_[body.front()] = 0;
        }

        NNBit reachable;
        reachable = turnsBit_[0];

        maxTurn_ = min(maxTurn + 1, 256);
        REP(t, min(maxTurn, 255)) {
            auto cur = turnsBit_[t];
            cur &= outable;
            expandableTurnsBit_[t] = cur;
            auto& next = turnsBit_[t + 1];

            next.reset();
            next |= (cur & outGuard.rightGuard_) << 1;
            next |= (cur & outGuard.leftGuard_) >> 1;
            next |= (cur & outGuard.bottomGuard_) << N;
            next |= cur >> N;
            next &= movable;

            next &= ~reachable;

            if (next.none()) {
                maxTurn_ = t + 1;
                break;
            }
            reachable |= next;

            for (auto i : next) {
                dists_[i] = t + 1;
            }


            if (requireBodySize - 3 - t >= 0) {
                movable.set(body[requireBodySize - 3 - t]);
            }
        }
    }

    void UpdateToFood(const SimEx& sim, int requireBodySize, int maxTurn) {
        VASSERT(requireBodySize >= 5);
        static BitOutGuard outGuard;

        dists_.assign(NN, 255);

        NNBit outable;
        outable.set();
        REP(i, NN) {
            if (sim.cellsColor_[i] != 0) {
                outable.reset(i);
            }
        }

        NNBit movable;
        movable.set();
        REP(i, requireBodySize - 2) {
            movable.reset(sim.GetBodyCell(i));
        }

        {
            auto& cur = turnsBit_[0];
            cur.reset();
            cur.set(sim.GetHeadCell());
            dists_[sim.GetHeadCell()] = 0;
        }

        NNBit reachable;
        reachable = turnsBit_[0];

        maxTurn_ = min(maxTurn + 1, 256);
        REP(t, min(maxTurn, 255)) {
            auto cur = turnsBit_[t];
            cur &= outable;
            expandableTurnsBit_[t] = cur;
            auto& next = turnsBit_[t + 1];

            next.reset();
            next |= (cur & outGuard.rightGuard_) << 1;
            next |= (cur & outGuard.leftGuard_) >> 1;
            next |= (cur & outGuard.bottomGuard_) << N;
            next |= cur >> N;
            next &= movable;

            next &= ~reachable;

            if (next.none()) {
                maxTurn_ = t + 1;
                break;
            }
            reachable |= next;

            for (auto i : next) {
                dists_[i] = t + 1;
            }


            if (requireBodySize - 3 - t >= 0) {
                movable.set(sim.GetBodyCell(requireBodySize - 3 - t));
            }
        }
    }

    u8 GetDist(int to) const {
        return dists_[to];
    }

    bool GetPath(int to, NNArr<u8>& path) const {
        VASSERT(to >= 0 && to < NN);

        path.clear();

        u8 dist = GetDist(to);
        if (dist == 255) {
            return false;
        }

        int cur = to;
        RREP(t, dist) {
            path.push_back(cur);

            const auto& prev = expandableTurnsBit_[t];
            bool found = false;
            for (auto p : gs.around(cur)) {
                if (prev.test(p)) {
                    cur = p;
                    found = true;
                    break;
                }
            }
            if (!found) {
                path.clear();
                return false;
            }
        }
        path.push_back(cur);
        reverse(path.begin(), path.end());
        return true;
    }

    bool GetPrev(int to, u8& prevCell) const {
        VASSERT(to >= 0 && to < NN);

        prevCell = 255;

        u8 dist = GetDist(to);
        if (dist == 255) {
            return false;
        }

        int cur = to;
        RREP(t, dist) {
            const auto& prev = expandableTurnsBit_[t];
            bool found = false;
            for (auto p : gs.around(cur)) {
                if (prev.test(p)) {
                    cur = p;
                    found = true;
                    break;
                }
            }
            if (!found) {
                prevCell = 255;
                return false;
            }
        }
        prevCell = cur;

        return true;
    }
};

void MakeTargetGrid(NNArr<u8>& targetGridColors, NNArr<u8>& finalCellOrder) {
    targetGridColors.assign(NN, 0);
    finalCellOrder.clear();

    int requireCount = M - 5;
    NNArr<bool> used(NN, false);
    int count = 0;

    int side = 0;
    FOR(i, 1, 16) {
        if (i * i >= M - 5) {
            side = i;
            break;
        }
    }
    if (side > N - 2) {
        array<NNArr<u8>, 32> ltCands;
        REP(y, N - 2) {
            REP(x, N - 2) {
                int cell = gs.ToId(x, y);
                int dist = gs.CalcL1Dist(cell, 0);
                ltCands[dist].push_back(cell);
            }
        }
        array<NNArr<u8>, 32> rbCands;
        REP(ry, N - 2) {
            REP(rx, N - 2) {
                int cell = gs.ToId(N - 1 - rx, N - 1 - ry);
                int dist = gs.CalcL1Dist(cell, NN - 1);
                rbCands[dist].push_back(cell);
            }
        }


        int count = 0;
        REP(dist, 32) {
            count += ltCands[dist].size();
        }
        VASSERT(ltCands[2 * (N - 3)].size() == 1);  

        if (count >= requireCount) {
            VABORT();
        }
        else {
            int ltDistCount = 2 * (N - 3) + 1;
            int rbDistCount = 0;
            count += rbCands[rbDistCount].size();
            ++rbDistCount;

            REP(loop, 100) {
                if (count >= requireCount) {
                    break;
                }
                if (ltDistCount <= N - 2) {
                    break;
                }

                --ltDistCount;
                count -= ltCands[ltDistCount].size();
                count += rbCands[rbDistCount].size();
                ++rbDistCount;
            }

            int ti = 5;
            NNArr<u8> cands;

            {
                NNArr<u8> cands;
                NNArr<bool> used(NN, false);
                REP(dist, rbDistCount) {
                    for (int cell : rbCands[dist]) {
                        cands.push_back(cell);
                        used[cell] = true;
                        if (cands.size() >= requireCount) {
                            break;
                        }
                    }
                    if (cands.size() >= requireCount) {
                        break;
                    }
                }

                if (N == 8 && M == 48) {
                    cands.push_back(15);
                    used[15] = true;
                }

                REP(rx, N) {
                    if (rx % 2 == 0) {
                        RREP(ry, N) {
                            int cell = gs.ToId(N - 1 - rx, N - 1 - ry);
                            if (used[cell]) {
                                targetGridColors[cell] = targetBodyColors_[ti++];
                                finalCellOrder.push_back(cell);
                            }
                        }
                    }
                    else {
                        REP(ry, N) {
                            int cell = gs.ToId(N - 1 - rx, N - 1 - ry);
                            if (used[cell]) {
                                targetGridColors[cell] = targetBodyColors_[ti++];
                                finalCellOrder.push_back(cell);
                            }
                        }
                    }
                }
            }

            {
                NNArr<bool> used(NN, false);
                REP(dist, ltDistCount) {
                    for (int cell : ltCands[dist]) {
                        cands.push_back(cell);
                        used[cell] = true;
                        if (cands.size() >= requireCount) {
                            break;
                        }
                    }
                    if (cands.size() >= requireCount) {
                        break;
                    }
                }

                REP(x, N) {
                    if (x % 2 == 0) {
                        RREP(y, N) {
                            int cell = gs.ToId(x, y);
                            if (used[cell]) {
                                targetGridColors[cell] = targetBodyColors_[ti++];
                                finalCellOrder.push_back(cell);
                            }
                        }
                    }
                    else {
                        REP(y, N) {
                            int cell = gs.ToId(x, y);
                            if (used[cell]) {
                                targetGridColors[cell] = targetBodyColors_[ti++];
                                finalCellOrder.push_back(cell);
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        REP(y, side) {
            REP(x, side) {
                int cell = gs.ToId(x, y);
                used[cell] = true;
                ++count;
                if (count >= requireCount) {
                    break;
                }
            }
            if (count >= requireCount) {
                break;
            }
        }
    }

    int height = (requireCount + side - 1) / side;
    int ti = 5;
    bool rev = true;
    RREP(y, height) {
        if (rev) {
            RREP(x, N) {
                int cell = gs.ToId(x, y);
                if (used[cell]) {
                    targetGridColors[cell] = targetBodyColors_[ti++];
                    finalCellOrder.push_back(cell);
                }
            }
        }
        else {
            REP(x, N) {
                int cell = gs.ToId(x, y);
                if (used[cell]) {
                    targetGridColors[cell] = targetBodyColors_[ti++];
                    finalCellOrder.push_back(cell);
                }
            }
        }
        rev = !rev;
    }
}

void SolveRule(const NNArr<u8>& targetGridColors, const NNArr<u8>& finalCellOrder, dvec<Dir>& result, int& rawScore,
               bool& completed) {
    result.clear();
    rawScore = INT_MAX;
    completed = false;

    PathFinder finder;
    Sim sim;
    sim.Init(startGridColors_);

    auto ApplyMove = [&](const NNArr<u8>& path) {
        REP(i, path.size() - 1) {
            int from = path[i];
            int to = path[i + 1];
            Dir d = gs.CalcDir(from, to);
            result.push(d);
            sim.Move(d);
        }
    };

    Rng rng;

    auto TryFailMove = [&](const NNArr<bool>& fixed) -> bool {
        array<Dir, 4> dirs = {Dir::D, Dir::L, Dir::R, Dir::U};
        shuffle(ALL(dirs), rng);
        for (Dir dir : dirs) {
            int head = sim.bodyCells_.front();
            int newHead = gs.dirs(head)[(int)dir];
            if (newHead == -1) {
                continue;
            }
            if (newHead == sim.bodyCells_[1]) {
                continue;
            }
            if (fixed[newHead]) {
                continue;
            }
            NNArr<u8> path;
            path.push(head);
            path.push(newHead);
            ApplyMove(path);
            return true;
        }
        return false;
    };

    struct TargetCand {
        CapArr<u8, 16> cells;
    };
    static dvec<TargetCand> targetCands;

    REP(turn, 10000) {
        NNArr<bool> fixed(NN, false);  
        targetCands.clear();

        {
            int maxX = N - 2;
            REP(y, N) {
                REP(x, maxX) {
                    int cell = gs.ToId(x, y);
                    if (targetGridColors[cell] == 0) {
                        maxX = x;
                        break;
                    }
                    if (sim.gridColors_[cell] == targetGridColors[cell]) {
                        fixed[cell] = true;
                    }
                    else {
                        auto& cand = targetCands.push();
                        cand.cells.clear();
                        cand.cells.push(cell);

                        FOR(x2, x + 1, maxX) {
                            int cell2 = gs.ToId(x2, y);
                            if (targetGridColors[cell2] == 0) {
                                break;
                            }

                            if (x2 + 1 - x > 4) {
                                break;
                            }

                            auto& cand = targetCands.push();
                            cand.cells.clear();
                            RFOR(x3, x, x2 + 1) {
                                int cell3 = gs.ToId(x3, y);
                                cand.cells.push(cell3);
                            }
                        }

                        maxX = x;
                        break;
                    }
                }
            }
        }

        {
            int minX = 2;
            RREP(y, N) {
                int lineFixCount = 0;
                RFOR(x, minX, N) {
                    int cell = gs.ToId(x, y);
                    if (targetGridColors[cell] == 0) {
                        minX = x + 1;
                        break;
                    }
                    if (sim.gridColors_[cell] == targetGridColors[cell]) {
                        fixed[cell] = true;
                        ++lineFixCount;
                    }
                    else {
                        auto& cand = targetCands.push();
                        cand.cells.clear();
                        cand.cells.push(cell);
                        minX = x + 1;
                        break;
                    }
                }
            }
        }

        if (targetCands.empty()) {
            bool fail = false;
            NNArr<bool> finalFixed = fixed;
            REP(i, finalCellOrder.size()) {
                u8 targetCell = finalCellOrder[i];
                finalFixed[targetCell] = false;

                if (sim.gridColors_[targetCell] == 0) {
                    VABORT();
                    continue;
                }

                NNArr<u8> path;
                finder.UpdatePath(sim.bodyCells_, sim.bodyCells_.size(), finalFixed);
                if (!finder.GetPath(path, targetCell)) {
                    if (TryFailMove(finalFixed)) {
                    }
                    fail = true;
                    break;
                }
                ApplyMove(path);
            }
            if (fail) {
                continue;
            }
            break;
        }

        finder.UpdatePathToFood(sim.gridColors_, sim.bodyCells_, sim.bodyCells_.size(), fixed);

        TargetCand bestTargetCand;
        int bestGotCellCount = 0;    
        int bestTargetBodyIdx = -1;  
        {
            double bestScore = -1e100;
            for (cauto& cand : targetCands) {
                double score = 0;
                score += cand.cells.size() * 100;

                int targetBodyIdx = -1;
                int targetGotCellCount = 0;
                FOR(i, 5, sim.bodyCells_.size()) {
                    int gotCellCount = 0;
                    REP(j, cand.cells.size()) {
                        if (i + j >= sim.bodyCells_.size()) {
                            break;
                        }
                        if (sim.bodyColors_[i + j] != targetGridColors[cand.cells[j]]) {
                            break;
                        }
                        ++gotCellCount;
                    }
                    if (gotCellCount == cand.cells.size()) {
                        targetBodyIdx = i;
                        targetGotCellCount = gotCellCount;

                        break;
                    }
                }
                if (targetBodyIdx != -1) {
                }
                else {
                    FOR(i, max(5, sim.bodyCells_.size() - cand.cells.size() + 1), sim.bodyCells_.size()) {
                        bool match = true;
                        REP(j, sim.bodyCells_.size() - i) {
                            if (sim.bodyColors_[i + j] != targetGridColors[cand.cells[j]]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) {
                            int gotCellCount = sim.bodyCells_.size() - i;
                            auto targetColor = targetGridColors[cand.cells[gotCellCount]];
                            bool hasNext = false;
                            REP(cell, NN) {
                                if (sim.gridColors_[cell] == 0) {
                                    continue;
                                }
                                if (sim.gridColors_[cell] == targetColor) {
                                    u8 dist = finder.GetDist(cell);
                                    if (dist < 255) {
                                        hasNext = true;
                                        break;
                                    }
                                }
                            }
                            if (!hasNext) {
                                match = false;
                            }
                        }
                        if (match) {
                            targetBodyIdx = i;
                            targetGotCellCount = sim.bodyCells_.size() - i;
                            break;
                        }
                    }
                }


                if (score > bestScore) {
                    bestScore = score;
                    bestTargetCand = cand;
                    bestGotCellCount = targetGotCellCount;
                    bestTargetBodyIdx = targetBodyIdx;
                }
            }
        }


        if (bestGotCellCount != bestTargetCand.cells.size()) {
            auto targetCell = bestTargetCand.cells[bestGotCellCount];
            auto targetColor = targetGridColors[targetCell];

            u8 bestDist = 255;
            int bestTargetCell = -1;
            REP(cell, NN) {
                if (sim.gridColors_[cell] == 0) {
                    continue;
                }
                if (sim.gridColors_[cell] == targetColor) {
                    u8 dist = finder.GetDist(cell);
                    if (dist < bestDist) {
                        bestDist = dist;
                        bestTargetCell = cell;
                    }
                }
            }

            if (bestTargetCell == -1) {
                finder.UpdatePath(sim.bodyCells_, 5, fixed);

                REP(cell, NN) {
                    if (sim.gridColors_[cell] == 0) {
                        continue;
                    }
                    if (sim.gridColors_[cell] == targetColor) {
                        u8 dist = finder.GetDist(cell);
                        if (dist < bestDist) {
                            bestDist = dist;
                            bestTargetCell = cell;
                        }
                    }
                }
            }

            NNArr<u8> path;
            if (bestTargetCell == -1 || !finder.GetPath(path, bestTargetCell)) {
                if (TryFailMove(fixed)) {
                    continue;
                }
                break;
            }
            ApplyMove(path);
        }
        else {
            auto targetBodyIdx = bestTargetBodyIdx;

            {
                bool moved = false;
                REP(i, sim.bodyCells_.size() - bestTargetCand.cells.size() + 1) {
                    bool match = true;
                    REP(j, bestTargetCand.cells.size()) {
                        if (i + j >= sim.bodyCells_.size()) {
                            break;
                        }
                        if (sim.bodyCells_[i + j] != bestTargetCand.cells[j]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        int cutTurn = targetBodyIdx - i;
                        if (cutTurn > 0) {
                            NNArr<u8> path;
                            if (CalcCutPath(sim.bodyCells_, cutTurn, targetBodyIdx, fixed, path)) {
                                ApplyMove(path);
                                moved = true;
                                break;
                            }
                        }
                    }
                }
                if (moved) {
                    continue;
                }
            }

            {
                int matchSize = 0;
                RFOR(useTargetSize, 1, bestTargetCand.cells.size()) {
                    bool match = true;
                    REP(i, useTargetSize) {
                        int ti = bestTargetCand.cells.size() - useTargetSize + i;
                        if (sim.bodyCells_[i] != bestTargetCand.cells[ti]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        matchSize = useTargetSize;
                    }
                }
                if (matchSize > 0) {
                    auto targetCell = bestTargetCand.cells[bestTargetCand.cells.size() - 1 - matchSize];

                    if (targetCell != sim.bodyCells_[1]) {
                        NNArr<u8> path;
                        path.push(sim.bodyCells_.front());
                        path.push(targetCell);
                        ApplyMove(path);
                        continue;
                    }
                }
            }

            {
                auto targetCell = bestTargetCand.cells.back();


                auto tmpFixed = fixed;
                REP(i, bestTargetCand.cells.size() - 1) {
                    tmpFixed[bestTargetCand.cells[i]] = true;
                }

                NNArr<u8> path;
                finder.UpdatePath(sim.bodyCells_, targetBodyIdx + bestTargetCand.cells.size(), tmpFixed);
                if (finder.GetPath(path, targetCell)) {
                    ApplyMove(path);
                    continue;
                }
            }

            if (TryFailMove(fixed)) {
            }
        }
    }

    rawScore = sim.CalcRawScore(result.size());
    completed = sim.sameCount_ == M;
}

void SolveRule2(const dvec<Dir>& guildResult, dvec<Dir>& result, int& rawScore, bool& completed) {
    result.clear();
    rawScore = INT_MAX;
    completed = false;

    Rng rng;

    auto ApplyMove = [&](Sim& sim, const NNArr<u8>& path, dvec<Dir>* result) {
        REP(i, path.size() - 1) {
            int from = path[i];
            int to = path[i + 1];
            Dir d = gs.CalcDir(from, to);
            if (result) {
                result->push(d);
            }
            sim.Move(d);
        }
    };

    auto CheckBiteAndTake = [&](const Sim& sim, NNArr<u8>& resultPath) -> bool {
        BitPathFinder finder;
        finder.Update(sim.bodyCells_, sim.sameCount_, 255);

        NNArr<u8> bodyTurn;
        bodyTurn.assign(NN, 0);
        RREP(i, sim.bodyCells_.size() - 2) {
            int t = sim.bodyCells_.size() - 2 - i;
            bodyTurn[sim.bodyCells_[i]] = t;
        }

        double bestScore = -1e100;
        NNArr<u8> bestPath;
        REP(to, NN) {
            u8 dist = finder.GetDist(to);
            if (dist == 255) {
                continue;
            }

            int prevCell;
            if (dist == 0) {
                prevCell = sim.bodyCells_[1];
            }
            else {
                NNArr<u8> path;
                bool ret = finder.GetPath(to, path);
                VASSERT(ret);
                prevCell = path[path.size() - 2];
            }

            for (auto n : gs.around(to)) {
                if (n == prevCell) {
                    continue;
                }

                if (bodyTurn[n] > 0) {
                    int reachTurn = sim.bodyCells_.size() - 2 + dist + 1;
                    int bodyLeft = reachTurn - bodyTurn[n] + 1;

                    NNArr<u8> path;
                    if (bodyLeft < sim.sameCount_) {
                        int overCut = sim.sameCount_ - bodyLeft;

                        bool ret = finder.GetPath(to, path);
                        VASSERT(ret);
                        path.push(n);

                        FOR(i, bodyLeft, sim.sameCount_) {
                            int cell = sim.bodyCells_[i - dist - 1];
                            path.push(cell);
                        }
                    }
                    else if (bodyLeft > sim.sameCount_) {
                        continue;
                    }
                    else {
                        bool ret = finder.GetPath(to, path);
                        VASSERT(ret);
                        path.push(n);
                    }

                    int totalTurn = dist + path.size() - 1;
                    Sim nextSim = sim;
                    ApplyMove(nextSim, path, nullptr);

                    u8 nextColor = targetBodyColors_[nextSim.sameCount_];

                    BitPathFinder finder2;
                    finder2.UpdateToFood(nextSim.gridColors_, nextSim.bodyCells_, nextSim.sameCount_, 255);

                    u8 bestDist = 255;
                    u8 bestCell = 0;
                    REP(i, NN) {
                        if (nextSim.gridColors_[i] == nextColor) {
                            u8 dist = finder2.GetDist(i);
                            if (dist < bestDist) {
                                bestDist = dist;
                                bestCell = i;
                            }
                        }
                    }
                    if (bestDist < 255) {
                        totalTurn += bestDist;
                        if (totalTurn < 255) {
                            double score = -totalTurn;
                            if (score > bestScore) {
                                bestScore = score;

                                NNArr<u8> path2;
                                bool ret = finder2.GetPath(bestCell, path2);
                                VASSERT(ret);
                                bestPath = path;
                                FOR(i, 1, path2.size()) {
                                    bestPath.push(path2[i]);
                                }
                            }
                        }
                    }
                }
            }
        }
        resultPath = bestPath;
        return resultPath.size() > 0;
    };



    Sim sim;
    sim.Init(startGridColors_);

    auto TryFailMove = [&]() -> bool {
        array<Dir, 4> dirs = {Dir::D, Dir::L, Dir::R, Dir::U};
        shuffle(ALL(dirs), rng);
        for (Dir dir : dirs) {
            int head = sim.bodyCells_.front();
            int newHead = gs.dirs(head)[(int)dir];
            if (newHead == -1) {
                continue;
            }
            if (newHead == sim.bodyCells_[1]) {
                continue;
            }

            NNArr<u8> path;
            path.push(head);
            path.push(newHead);
            ApplyMove(sim, path, &result);
            return true;
        }
        return false;
    };

    {
        NNArr<u8> path;
        u8 cell = sim.bodyCells_.front();
        REP(i, guildResult.size()) {
            Dir dir = guildResult[i];
            int newCell = gs.dirs(cell)[(int)dir];
            path.clear();
            path.push(cell);
            path.push(newCell);
            ApplyMove(sim, path, &result);
            cell = newCell;
        }
    }

    BitPathFinder finder;
    REP(loop, 1000) {
        if (sim.sameCount_ == M) {
            break;
        }

        u8 nextColor = targetBodyColors_[sim.sameCount_];

        if (sim.bodyCells_.size() == sim.sameCount_) {
            {
                NNBit visited;
                NNArr<u8> bestPath;
                NNArr<u8> path;
                auto dfs = [&](auto&& dfs, int cell, int sc) -> void {
                    visited.set(cell);
                    path.push(cell);

                    if (path.size() > bestPath.size()) {
                        bestPath = path;
                    }

                    if (sc < M) {
                        for (int n : gs.around(cell)) {
                            if (visited.test(n) || targetBodyColors_[sc] != sim.gridColors_[n]) {
                                continue;
                            }
                            dfs(dfs, n, sc + 1);
                        }
                    }

                    visited.reset(cell);
                    path.pop();
                };
                dfs(dfs, sim.bodyCells_.front(), sim.sameCount_);
                if (bestPath.size() > 1) {
                    ApplyMove(sim, bestPath, &result);
                    continue;
                }
            }
            finder.UpdateToFood(sim.gridColors_, sim.bodyCells_, sim.sameCount_, 255);

            u8 bestDist = 255;
            u8 bestCell = 0;
            REP(i, NN) {
                if (sim.gridColors_[i] == nextColor) {
                    u8 dist = finder.GetDist(i);
                    if (dist < bestDist) {
                        bestDist = dist;
                        bestCell = i;
                    }
                }
            }
            if (bestDist < 255) {
                NNArr<u8> path;
                bool ret = finder.GetPath(bestCell, path);
                VASSERT(ret);
                ApplyMove(sim, path, &result);
                continue;
            }
        }

        NNArr<u8> path;
        if (CheckBiteAndTake(sim, path)) {
            ApplyMove(sim, path, &result);
            continue;
        }

        {
            finder.Update(sim.bodyCells_, sim.sameCount_, 255);

            u8 bestDist = 255;
            NNArr<u8> bestPath;
            REP(i, NN) {
                if (sim.gridColors_[i] == nextColor) {
                    {
                        u8 dist = finder.GetDist(i);
                        if (dist < 255) {
                            Sim nextSim = sim;

                            NNArr<u8> path;
                            bool ret = finder.GetPath(i, path);
                            VASSERT(ret);
                            ApplyMove(nextSim, path, nullptr);

                            NNArr<u8> path2;
                            if (CheckBiteAndTake(nextSim, path2)) {
                                int totalDist = dist;
                                if (totalDist < bestDist) {
                                    bestDist = totalDist;
                                    bestPath = path;
                                }
                            }
                        }
                    }

                    for (auto n : gs.around(i)) {
                        u8 dist = finder.GetDist(n);
                        if (dist < 255) {
                            Sim nextSim = sim;

                            NNArr<u8> path;
                            bool ret = finder.GetPath(n, path);
                            VASSERT(ret);
                            ApplyMove(nextSim, path, nullptr);

                            NNArr<u8> path2;
                            if (CheckBiteAndTake(nextSim, path2)) {
                                int totalDist = dist + path.size() - 1;
                                if (totalDist < bestDist) {
                                    bestDist = totalDist;
                                    bestPath = path;
                                    FOR(i, 1, path2.size()) {
                                        bestPath.push(path2[i]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (bestDist < 255) {
                ApplyMove(sim, bestPath, &result);
                continue;
            }
        }

        TryFailMove();
    }

    rawScore = sim.CalcRawScore(result.size());
    completed = sim.sameCount_ == M;
}

template <class K, class V>
struct DynHashMapForBeam {
    uint32_t m_capacity = 0;
    K* m_keys = nullptr;
    V* m_values = nullptr;
    uint8_t* m_states = nullptr;  
    uint32_t m_mask = 0;

    uint32_t m_count = 0;

    DynHashMapForBeam(const DynHashMapForBeam&) = delete;
    void operator=(const DynHashMapForBeam&) = delete;
    DynHashMapForBeam() {
    }

    ~DynHashMapForBeam() {
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
        m_states = (uint8_t*)calloc(m_capacity, sizeof(uint8_t));
        m_count = 0;
    }

    void clear() {
        if (m_capacity) {
            memset(m_states, 0, m_capacity * sizeof(uint8_t));
        }
        m_count = 0;
    }

    void set(K key, const V& v) {
        VASSERT(m_count < m_capacity);

        uint32_t i = HomeIndex(key);
        for (;;) {
            if (m_states[i] == 0) {
                m_states[i] = 1;
                m_keys[i] = key;
                m_values[i] = v;
                ++m_count;
                return;
            }
            if (m_keys[i] == key) {
                m_values[i] = v;
                return;
            }
            i = NextIndex(i);
        }
    }

    V* try_get(K key) {
        uint32_t i = HomeIndex(key);
        for (;;) {
            if (m_states[i] == 0) {
                return nullptr;
            }
            if (m_keys[i] == key) {
                return &m_values[i];
            }
            i = NextIndex(i);
        }
    }

    void remove(K key) {
        uint32_t hole = HomeIndex(key);
        for (;;) {
            if (m_states[hole] == 0) {
                VABORT();
            }
            if (m_keys[hole] == key) {
                break;
            }
            hole = NextIndex(hole);
        }

        uint32_t i = hole;
        uint32_t j = NextIndex(i);
        while (m_states[j]) {
            if (ProbeDistance(j, m_keys[j]) > ProbeDistance(i, m_keys[j])) {
                m_keys[i] = m_keys[j];
                m_values[i] = m_values[j];
                m_states[i] = 1;
                i = j;
            }
            j = NextIndex(j);
        }

        m_states[i] = 0;
        --m_count;
    }

    bool contains(K key) const {
        uint32_t i = HomeIndex(key);
        for (;;) {
            if (m_states[i] == 0) {
                return false;
            }
            if (m_keys[i] == key) {
                return true;
            }
            i = NextIndex(i);
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
    DynHashMapForBeam<u64, int> hashMap_;

    void Reserve(int capacity) {
        nodePool_.reserve(capacity);
        seg_.buf_.reserve(capacity);
        hashMap_.init(capacity * 2);
    }

    void Init(int width) {
        width_ = width;
        nodePool_.clear();
        nodePool_.reserve(width);
        seg_.Init(width);
        hashMap_.init(width * 2);
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
};

#include <algorithm>
#include <chrono>
#include <deque>

struct BeamWidthManager {
    int minWidth_ = 100;           
    int widthCap_ = 40000;         
    double timeRateWeight_ = 1.0;  

   public:

    int CalcWidth(const Duration& workTime, int widthCount, const Duration& timeLeft, int turnLeft, double timeRate) {
        double widthPerTime = widthCount / (double)workTime.count();
        double executableWidth = widthPerTime * (double)timeLeft.count();
        executableWidth *= (timeRateWeight_ * (1.0 - timeRate) + 1.0 * timeRate);
        double turnWidth = executableWidth / turnLeft;

        return clamp((int)turnWidth, minWidth_, widthCap_);
    }
};

#define EULER_RESTORE_CHECK 0


struct EasyEulerBSWorkData {
    int totalWorkTurn_ = 0;
    int totalWidthCount_ = 0;
    Duration resumeWorkTime_ = Duration::zero();
};

template <class State, class Delta, class RawScoreType>
struct EasyEulerBS {
    int tourCap_ = 100000;  
    int maxTurn_ = 100;            
    int measureTurn_ = 5;          
    int measureWidth_ = 1000;      
    int minWidth_ = 100;           
    int widthCap_ = 40000;         
    double timeRateWeight_ = 1.0;  

   private:
    struct Edge {
        bool forward_;
        bool enable_;
        u32 id_;  
        Delta delta_;
    };

    dvec<Edge> curTour_;     
    dvec<Edge> nextTour_;    
    dvec<int> fowardStack_;  

    BeamFilter<int> filter_;
    BeamWidthManager widthManager_;

    int maxWidth_;     
    int maxTourSize_;  
    bool isComplete_;  

    int totalWorkTurn_;        
    int totalWidthCount_;      
    Duration resumeWorkTime_;  

   public:
    void SetWorkData(const EasyEulerBSWorkData& workData) {
        totalWorkTurn_ = workData.totalWorkTurn_;
        totalWidthCount_ = workData.totalWidthCount_;
        resumeWorkTime_ = workData.resumeWorkTime_;
    }

    EasyEulerBSWorkData GetWorkData() const {
        EasyEulerBSWorkData workData;
        workData.totalWorkTurn_ = totalWorkTurn_;
        workData.totalWidthCount_ = totalWidthCount_;
        workData.resumeWorkTime_ = resumeWorkTime_;
        return workData;
    }

    void Ready(bool reuseWorkData) {
        int forwardCap = tourCap_ / 2;

        curTour_.clear();
        curTour_.reserve(tourCap_);
        nextTour_.clear();
        nextTour_.reserve(tourCap_);
        fowardStack_.clear();
        fowardStack_.reserve(forwardCap);

        filter_.Reserve(widthCap_);

        widthManager_.minWidth_ = minWidth_;              
        widthManager_.widthCap_ = widthCap_;              
        widthManager_.timeRateWeight_ = timeRateWeight_;  

        if (!reuseWorkData) {
            totalWorkTurn_ = 0;
            totalWidthCount_ = 0;
            resumeWorkTime_ = Duration::zero();
        }

        maxWidth_ = 0;
        maxTourSize_ = 0;
        isComplete_ = false;
    }

    void Run(int logIdx, ChronoTimer& timer, State& state, auto&& makeNext, auto&& apply, auto&& revert) {
        int depth = 0;
        int nextDepth = 0;
        u32 edgeId = 0;
        int startTurn = 0;


        auto AddEdge = [&](int turn) {
            makeNext(turn, state,
                     [&](const double& evalScore, const u64& hash, const Delta& deltaF, const Delta& deltaB) {
                         if (int* nextEi = filter_.TryPush(evalScore, hash)) {
                             FOR(i, nextDepth, depth) {
                                 nextTour_.push(curTour_[fowardStack_[i]]);
                             }
                             nextDepth = depth;

                             *nextEi = nextTour_.size();

                             auto& f = nextTour_.pushR();
                             f.forward_ = true;
                             f.enable_ = false;
                             f.delta_ = deltaF;
                             f.id_ = edgeId;

                             auto& b = nextTour_.pushR();
                             b.forward_ = false;
                             b.delta_ = deltaB;
                             b.id_ = edgeId;

                             ++edgeId;
                         }
                     });
        };


        int turn = 0;
        auto startTime = timer.Now();
        auto endTime = timer.EndTime();

        while (true) {
            auto nowTime = timer.Now();
            auto timeLeft = endTime - nowTime;
            if (timeLeft.count() <= 0) {
                break;
            }
            int width;
            if (turn + totalWorkTurn_ < measureTurn_) {
                width = measureWidth_;
            }
            else {
                auto workTime = nowTime - startTime + resumeWorkTime_;
                int turnLeft = maxTurn_ - turn;
                double timeRate = workTime.count() / (double)(endTime - startTime).count();
                width = widthManager_.CalcWidth(workTime, totalWidthCount_, timeLeft, turnLeft, timeRate);
            }

            filter_.Init(width);

            nextTour_.clear();
            fowardStack_.clear();
            nextDepth = depth;

            int expandCount = 0;
            int appliedCount = 0;

            const int targetDepth = turn - startTurn;
            if (curTour_.empty()) {
                AddEdge(turn);
                ++totalWidthCount_;
                ++expandCount;
            }
            else {
                const auto* curTourData = curTour_.data();
                for (int ei = 0, curTourSize = curTour_.size(); ei < curTourSize; ++ei) {
                    cauto& e = curTourData[ei];
                    if (e.forward_) {
                        fowardStack_.pushR() = ei;
                        ++depth;

                        if (depth == targetDepth && e.enable_) {
                            const int forwardSize = fowardStack_.size();
                            for (int i = appliedCount; i < forwardSize; ++i) {
                                cauto& applyEdge = curTourData[fowardStack_[i]];
                                apply(state, applyEdge.delta_);
                            }
                            appliedCount = forwardSize;
                            ++totalWidthCount_;
                            ++expandCount;
                            AddEdge(turn);
                        }
                    }
                    else {
                        if (fowardStack_.size() == appliedCount) {
                            revert(state, e.delta_);
                            --appliedCount;
                        }
                        fowardStack_.pop();

                        --depth;
                        if (nextDepth > depth) {
                            nextTour_.pushR() = e;
                            --nextDepth;
                        }
                    }
                }
            }

            ++turn;

            if (isComplete_ || filter_.nodePool_.size() == 0) {
                break;
            }

            for (cauto& node : filter_.nodePool_) {
                int idx = node.data_;
                nextTour_[idx].enable_ = true;
            }

            int skipCount = 0;
            int tourSize = nextTour_.size();
            REP(i, tourSize / 2) {
                if (nextTour_[i].id_ == nextTour_[tourSize - 1 - i].id_) {
                    ++skipCount;
                }
                else {
                    break;
                }
            }
            if (skipCount) {
                curTour_.clear();
                FOR(i, skipCount, tourSize - skipCount) {
                    curTour_.push(nextTour_[i]);
                }

                REP(i, skipCount) {
                    apply(state, nextTour_[i].delta_);
                }
                startTurn += skipCount;
            }
            else {
                swap(curTour_, nextTour_);
            }

            chmax(maxWidth_, filter_.nodePool_.size());
            chmax(maxTourSize_, curTour_.size());
        }



        totalWorkTurn_ += turn;
        resumeWorkTime_ += timer.Now() - startTime;
    }

    void SetComplete() {
        isComplete_ = true;
    }
};


namespace solver_euler {
template <bool UseColorHash>
struct BeamStateT {
    using SimType = SimExT<UseColorHash>;
    SimType sim_;

    void Init(const Hasher& hasher) {
        sim_.Init(hasher);
    }

    void Apply(const typename SimType::DeltaF& deltaF) {
        sim_.Apply(deltaF);
    }

    void Revert(const typename SimType::DeltaB& deltaB) {
        sim_.Revert(deltaB);
    }

};

template <bool UseColorHash>
struct BeamDeltaT {
    union {
        typename SimExT<UseColorHash>::DeltaF deltaF_;
        typename SimExT<UseColorHash>::DeltaB deltaB_;
    };
};

struct EvalTable {
    array<double, M_max + 1> sameRateTbl_;
    array<array<double, M_max + 1>, M_max + 1> curveCoefTbl_;  
    array<array<double, M_max + 1>, M_max + 1> foodCoefTbl_;   
    array<double, M_max + 1> aroundBonusTbl_;

    EvalTable() {

        REP(sameCount, M + 1) {
            double sameRate = sameCount / (double)M;
            sameRateTbl_[sameCount] = sameRate;
            aroundBonusTbl_[sameCount] = HP.EvalHasAroundNext * (1 + sameRate * HP.EvalHasAroundNextSameRate);

            foodCoefTbl_[sameCount][0] = 0.0;
            FOR(n, 1, M + 1) {
                foodCoefTbl_[sameCount][n] = HP.EvalFoodSpread * (1 + sameRate * HP.EvalFoodSpreadSameRate) * 2.0 /
                                             (double(n * n) * square(N - 1.0));
            }
        }

        FOR(bodyLength, 5, M + 1) {
            FOR(sameCount, 5, bodyLength + 1) {
                double sameRate = sameRateTbl_[sameCount];
                curveCoefTbl_[bodyLength][sameCount] =
                    1.0 / (double)(bodyLength - 2) * HP.EvalCurveCount * (1 + sameRate * HP.EvalCurveCountSameRate);
            }
        }
    }
};


template <class SimType>
double Eval(const solver_euler::EvalTable& evalTable, const SimType& sim) {
    double evalScore = 0;
    const int sameCount = sim.sameCount_;
    const int bodyLength = sim.GetBodyLength();
    const u8 headCell = sim.GetHeadCell();


    evalScore += sameCount;

    {
        evalScore -= sim.curveCount_ * evalTable.curveCoefTbl_[bodyLength][sameCount];
    }

    {

        int n = M - bodyLength;

        if (n > 0) {
            cauto& v = sim.foodVariance_;
            evalScore -= (n * v.sumNorm2 - v.sumX * v.sumX - v.sumY * v.sumY) * evalTable.foodCoefTbl_[sameCount][n];
        }

    }

    if (sameCount == bodyLength) {
        int nextColor = targetBodyColors_[sameCount];

        bool hasAround = false;
        for (int n : gs.around(headCell)) {
            if (sim.cellsColor_[n] == nextColor) {
                hasAround = true;
                break;
            }
        }
        if (hasAround) {
            evalScore += evalTable.aroundBonusTbl_[sameCount];
        }
    }


















    return evalScore;
}

template <bool UseColorHash>
void RunBeamSearch(ChronoTimer& timer, const Hasher& hasher, solver_euler::EvalTable& evalTable, int logIdx,
                   int maxTurn, bool reuseWorkData, int& resultRawScore, int& resultCompleteTurn,
                   bool& resultIsCompleted, CapArr<u8, SimExCellBuf>& resultPath,
                   const EasyEulerBSWorkData* srcWorkData = nullptr, EasyEulerBSWorkData* dstWorkData = nullptr) {
    using BeamState = BeamStateT<UseColorHash>;
    using BeamDelta = BeamDeltaT<UseColorHash>;
    using SimType = typename BeamState::SimType;

    static EasyEulerBS<BeamState, BeamDelta, i64> bs;

    BeamDelta deltaF;
    BeamDelta deltaB;

    auto Apply = [&](BeamState& state, const BeamDelta& delta) { state.Apply(delta.deltaF_); };
    auto Revert = [&](BeamState& state, const BeamDelta& delta) { state.Revert(delta.deltaB_); };

    auto Encode = [&](const SimType& sim, int dir) -> u64 {
        u64 hash = 0;
        int len = sim.GetBodyLength();
        u8 headCell = sim.GetHeadCell();

        if constexpr (UseColorHash) {
            hash ^= hasher.bodyLengthHash_[len];
            hash ^= hasher.easyBodyCellHash_[headCell];
            hash ^= hasher.headDirHash_[dir];
            hash ^= hasher.curveCountHash_[sim.curveCount_];
            hash ^= hasher.sameCountHash_[sim.sameCount_];
            hash ^= sim.gridColorHash_;
            hash ^= sim.bodyColorHash_;
        }
        else {
            hash ^= hasher.bodyLengthHash_[len];
            hash ^= hasher.easyBodyCellHash_[headCell];
            hash ^= hasher.headDirHash_[dir];
            if (HP.FirstEncodeLevel >= 1) {
                hash ^= hasher.curveCountHash_[sim.curveCount_];
            }
            if (HP.FirstEncodeLevel >= 2) {
                hash ^= hasher.sameCountHash_[sim.sameCount_];
            }
        }
        return hash;
    };

    auto UpdateResult = [&](const SimType& sim) {
        auto rawScore = sim.CalcRawScore();
        if (rawScore < resultRawScore) {
            resultRawScore = rawScore;
            resultPath.resize(sim.turn_ - 4);
            memcpy(resultPath.data(), sim.cells_.data() + 4, sim.turn_ - 4);
            if (sim.sameCount_ == M) {
                resultCompleteTurn = sim.turn_ - 5;
                resultIsCompleted = true;
                bs.SetComplete();
            }
        }
    };

    auto MakeNext = [&](int turn, BeamState& state, auto&& registerNode) {
        auto& sim = state.sim_;

        auto headCell = sim.GetHeadCell();
        cauto& dirs = gs.dirs(headCell);
        auto ngCell = sim.GetBeforeHeadCell();
        REP(newDir, 4) {
            int newHead = dirs[newDir];
            if (newHead == -1) {
                continue;
            }
            if (newHead == ngCell) {
                continue;
            }

            if (!sim.Move(hasher, newHead, Dir(newDir), HP.MaxSameCountDown, deltaF.deltaF_, deltaB.deltaB_)) {
                continue;
            }

            UpdateResult(sim);

            if (sim.sameCount_ < M) {
                double evalScore = solver_euler::Eval(evalTable, sim);
                u64 hash = Encode(sim, newDir);
                registerNode(evalScore, hash, deltaF, deltaB);
            }

            sim.Revert(deltaB.deltaB_);
        }
    };

    BeamState rootState;
    rootState.Init(hasher);

    bs.tourCap_ = 2000000;     
    bs.measureTurn_ = 10;      
    bs.measureWidth_ = 1500;   
    bs.minWidth_ = 100;        
    bs.widthCap_ = 65535;      
    bs.timeRateWeight_ = 1.0;  
    bs.maxTurn_ = maxTurn;

    if (srcWorkData != nullptr) {
        bs.SetWorkData(*srcWorkData);
    }
    bs.Ready(reuseWorkData);
    bs.Run(logIdx, timer, rootState, MakeNext, Apply, Revert);
    if (dstWorkData != nullptr) {
        *dstWorkData = bs.GetWorkData();
    }
}
};  

void SolverEuler(ChronoTimer& timer, int preBestTurn, dvec<Dir>& result, int& bestRawScore, bool& bestIsCompleted) {
    result.clear();
    bestRawScore = INT_MAX;
    bestIsCompleted = false;

    static Hasher hasher;
    static solver_euler::EvalTable evalTable;

    int resultRawScore = INT_MAX;  
    int resultCompleteTurn = INT_MAX;
    bool resultIsCompleted = false;
    CapArr<u8, SimExCellBuf> resultPath;
    auto InitResult = [&]() {
        resultRawScore = INT_MAX;
        resultCompleteTurn = INT_MAX;
        resultIsCompleted = false;
        resultPath.clear();
    };
    int firstMaxTurn = 1500;  
    firstMaxTurn = int(0.0114 * M * M + 2.8549 * M - 17.477);
    firstMaxTurn += 400 * M / 150.0;  
    firstMaxTurn += 100;              
    chmin(firstMaxTurn, 1500);
    chmin(firstMaxTurn, preBestTurn - 1);

    EasyEulerBSWorkData beamWorkData;
    solver_euler::RunBeamSearch<false>(timer, hasher, evalTable, 0, firstMaxTurn, false, resultRawScore,
                                       resultCompleteTurn, resultIsCompleted, resultPath, nullptr, &beamWorkData);


    bestRawScore = resultRawScore;
    CapArr<u8, SimExCellBuf> bestPath = resultPath;
    bestIsCompleted = resultIsCompleted;

    {
        InitResult();

        int secondMaxTurn = bestPath.size() - 1 - 1;  
        solver_euler::RunBeamSearch<true>(timer, hasher, evalTable, 1, secondMaxTurn, true, resultRawScore,
                                          resultCompleteTurn, resultIsCompleted, resultPath, &beamWorkData, nullptr);


        if (resultRawScore < bestRawScore) {
            bestRawScore = resultRawScore;
            bestPath = resultPath;
            bestIsCompleted = resultIsCompleted;
        }
    }

    result.resize(bestPath.size() - 1);
    u8 cell = gs.ToId(0, 4);
    REP(i, bestPath.size() - 1) {
        u8 newCell = bestPath[i + 1];
        result[i] = gs.CalcDir(cell, newCell);
        cell = newCell;
    }


}

struct Solver {
    void Run(ChronoTimer& timer) {
        int bestRawScore = INT_MAX;
        bool bestIsCompleted = false;
        static dvec<Dir> bestResult;

        static dvec<Dir> tmpResult;
        int tmpRawScore;
        bool tmpIsCompleted;

        int bestTurn = 2000;
        if (bestIsCompleted) {
            bestTurn = bestResult.size();
        }

        SolverEuler(timer, bestTurn, tmpResult, tmpRawScore, tmpIsCompleted);

        if (tmpRawScore < bestRawScore) {
            bestRawScore = tmpRawScore;
            bestResult = tmpResult;
            bestIsCompleted = tmpIsCompleted;
        }

        if (!bestIsCompleted) {
            {
                SolveRule2(bestResult, tmpResult, tmpRawScore, tmpIsCompleted);

                if (tmpRawScore < bestRawScore) {
                    bestRawScore = tmpRawScore;
                    bestResult = tmpResult;
                    bestIsCompleted = tmpIsCompleted;
                }
            }

            {
                NNArr<u8> finalCellOrder;
                NNArr<u8> targetGridColors;
                MakeTargetGrid(targetGridColors, finalCellOrder);
                SolveRule(targetGridColors, finalCellOrder, tmpResult, tmpRawScore, tmpIsCompleted);
                if (tmpRawScore < bestRawScore) {
                    bestRawScore = tmpRawScore;
                    bestResult = tmpResult;
                    bestIsCompleted = tmpIsCompleted;
                }
            }

        }


        server.Output(bestResult);

    }
};

struct Main {
    void Run(int argc, const char* argv[]) {
        VASSERT(argc == 1);
        ChronoTimer timer;
        server.InitInput(timer);

        UpdateArrayParam();

        timer.StartMs(TIME_LIMIT);

        static Solver solver;
        solver.Run(timer);

        server.Finalize();

    }
};
