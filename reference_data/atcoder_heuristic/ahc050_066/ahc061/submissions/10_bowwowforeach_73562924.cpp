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

#define USE_ORACLE_AI_PARAM 0

#define CHECK_PARAM_ACCURACY 1

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

template <class T>
void InstanceRun(int argc, const char* argv[]) {
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

constexpr int N = 10;  
constexpr int NN = N * N;
constexpr int M_min = 2;  
constexpr int M_max = 8;  
constexpr int T = 100;
constexpr int U_min = 1;  
constexpr int U_max = 5;  

constexpr double W_min = 0.3;
constexpr double W_max = 1.0;
constexpr double Eps_min = 0.1;
constexpr double Eps_max = 0.5;

DEF_CONTAINERS(N, N);
DEF_CONTAINERS(NN, NN);
DEF_CONTAINERS(T, T);

DEF_CONTAINERS(M, M_max);
DEF_CONTAINERS(U, U_max);

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


    PD(EvalState4_Scale, 1.0, 0.95, 1.05);

    PI(SingleAIParamEstimator_ParticleCount, 768, 8, 768, 1);
    PD(SingleAIParamEstimator_ResampleEssRate, 0.55, 0.01, 0.99);
    PD(SingleAIParamEstimator_MinLikelihood, 1e-15, 1e-20, 1e-6, 0.0);
    PC(SingleAIParamEstimator_ResampleMoveMode, 2, 1, 2);  
    IF_START(SingleAIParamEstimator_ResampleMoveMode == 1);
    PD(SingleAIParamEstimator_JitterRatio, 0.05, 0.0, 0.1);
    PD(SingleAIParamEstimator_JitterEps, 0.02, 0.0, 0.4);
    IF_END;
    IF_START(SingleAIParamEstimator_ResampleMoveMode == 2);
    PI(SingleAIParamEstimator_McmcMoveIters, 2, 0, 10, 1);
    PI(SingleAIParamEstimator_McmcHistoryWindow, 100, 1, 100, 1, 100);
    PD(SingleAIParamEstimator_McmcStepRatio, 0.05, 0.0, 0.1);
    PD(SingleAIParamEstimator_McmcStepEps, 0.02, 0.0, 0.5);
    IF_END;


    PI(Decider5B_RawScoreTurn, 0, 0, 100, 1, 100);

    PD(Decider_PwAlpha, 2.2116396821714708, 0.0, 1.5, 1e-06);
    PD(Decider_PwCoeff, 3.9678872751783913, 0.0, 8.0, 1e-06);
    PI(Decider_PwRootInit, 1, 1, 16, 1);   
    PI(Decider_PwInnerInit, 1, 1, 16, 1);  

    PD(Decider_PwValueBonus, -0.0011818718302107488, -0.05, 0.05);
    PD(Decider_PriorScoreCompress, 10426.903725596783, 100.0, 50000.0, 1e-06);
    PI(Decider_SearchHorizon, 18, 14, 24, 1);
    PI(Decider_RolloutTopK, 4, 3, 11, 1);
    PD(Decider_RolloutTemp, 1360.8460749519265, 1200.0, 2200.0, 0.0);
    PI(Decider_NodeActionBaseRoot, 13, 12, 22, 1);
    PI(Decider_NodeActionBaseInner, 15, 4, 14, 1);
    PD(Decider_PuctExplore, 0.2205651180310576, 0.1, 0.3);               
    PD(Decider_PuctPriorTemp, 5206.91190987037, 4000.0, 6000.0, 1e-06);  

    START_TUNING;
    PD(EvalAction3_U1Get, 854.9897695631801, 0, 1000);
    END_TUNING;
    PD(EvalAction3_M2Get, 520.6368282812841, 0.0, 1000.0);


    PD(EvalAction3_ScoreRatioMin, 1.6750489474956183, 0.1, 4.0, 1e-06);
    PD(EvalAction3_ScoreRatioMax, 6.223266577285193, 1.0, 10.0, 1e-06);
    PD(EvalAction3_GainWeightEarly, 1.3528427691937353, 0.0, 3.0);
    PD(EvalAction3_GainWeightLate, 1.2987594343975404, 0.0, 3.0);
    PD(EvalAction3_DenyWeightEarly, 2.2492815790342506, 0.0, 3.0);
    PD(EvalAction3_DenyWeightLate, 1.8346721082192639, 0.0, 3.0);
    PD(EvalAction3_OtherDenyRate, 0.20528938148236534, 0.0, 1.0);
    PD(EvalAction3_DefendWeightEarly, -2.691922994512465, -3.0, 0.0);
    PD(EvalAction3_DefendWeightLate, 9.579898876300529, 2.0, 10.0);
    PD(EvalAction3_FrontierWeightEarly, 0.35519064672998085, 0.0, 0.5);
    PD(EvalAction3_FrontierWeightLate, 0.42057132591407237, 0.0, 0.5);
    PD(EvalAction3_DefendLevel1Boost, 7.516954696304367, 1.0, 7.0, 1e-06);
    PD(EvalAction3_CongestionPenaltyRate, 1.192888855273822, 0.0, 3.0);
    PD(EvalAction3_CapPenaltyRate, 16.191055835585953, 2.0, 12.0);
    PD(EvalAction3_TieBreakRate, 7.294163831267526e-05, -0.1, 0.1);
    PD(EvalAction3_EnemyWaThreatRate, 3.3114350607711924, 0.0, 5.0);
    PD(EvalAction3_EnemyWbThreatRate, 1.2669427140226182, 0.0, 2.0);
    PD(EvalAction3_EnemyWcThreatRate, -1.6079134076805546, -2.0, 0.0);
    PD(EvalAction3_EnemyWdThreatRate, 0.31334681761791217, -2.0, 4.0);





    PD(Decider5C_PuctPriorTemp, 942.7338508376101, 0.0, 2000.0, 1e-06);
    PI(Decider5C_SearchHorizon, 14, 8, 24, 1);
    PD(Decider5C_PriorScoreCompress, 15226.483749423824, 10000.0, 20000.0, 1e-06);
    PI(Decider5C_RolloutTopK, 11, 1, 16, 1);
    PD(Decider5C_RolloutTemp, 2587.5348713139574, 800.0, 5000.0, 0.0);
    PI(Decider5C_NodeActionBaseRoot, 8, 1, 16, 1);
    PI(Decider5C_RootUniformRounds, 60, 40, 80, 1);
    PI(Decider5C_RootFocusTopK, 14, 4, 24, 1);
    PI(Decider5C_RootMaxStageScenarios, 92, 40, 128, 1);

    PI(Decider5C_RawScoreTurn, 47, 20, 80, 1, 100);
    PD(EvalState5_CaseScoreScale, 121325.11723100065, 50000.0, 150000.0);
    PD(EvalState5_DiffRate, 0.009298421487410828, 0.0, 0.1);
    PD(EvalState5_HorizonGainRateEarly, 1.9026543705730643, 1.0, 3.0);
    PD(EvalState5_HorizonGainRateLate, -1.0636727440337634, -2.0, 0);
    PD(EvalState5_WeakRiskRateEarly, 1.1076267873909482, 0.0, 2.0);
    PD(EvalState5_WeakRiskRateLate, 1.0348041452373453, 0.0, 2.0);
    PD(EvalState5_NeutralContestRiskRateEarly, 0.04388526965610695, 0.0, 0.5);  
    PD(EvalState5_NeutralContestRiskRateLate, -0.14459368801324313, -0.5, 0.5);
    PD(EvalState5_ContactRiskRateEarly, 0.44042541771929766, 0.0, 1.0);
    PD(EvalState5_ContactRiskRateLate, 0.25147598612197125, 0.0, 0.8);
    PD(EvalState5_IslandPenaltyRateEarly, -0.3123549413496246, -1.0, 0.5);
    PD(EvalState5_IslandPenaltyRateLate, 0.2543221176578774, 0.0, 1.0);

    PD(EvalState5_GrowStockRateEarly, 0.0, 0.0, 1.5);
    PD(EvalState5_GrowStockRateLate, 0.0, 0.0, 1.5);
    PD(EvalState5_NeutralSafeStockRateEarly, 0.0, 0.0, 2.0);
    PD(EvalState5_NeutralSafeStockRateLate, 0.0, 0.0, 2.0);
    PD(EvalState5_FinishStockRateEarly, 0.0, 0.0, 2.5);
    PD(EvalState5_FinishStockRateLate, 0.0, 0.0, 2.5);
    PD(EvalState5_ChipStockRateEarly, 0.0, 0.0, 1.0);
    PD(EvalState5_ChipStockRateLate, 0.0, 0.0, 1.0);

    PD(EvalState5_BestNeutralSafeRate, 0.0, 0.0, 2.0);
    PD(EvalState5_BestGrowRate, 0.0, 0.0, 2.0);
    PD(EvalState5_BestFinishRate, 0.0, 0.0, 3.0);
    PD(EvalState5_BestChipRate, 0.0, 0.0, 2.0);
    PI(EvalState5_GainTurnCapEarly, 0, 0, 60, 1);
    PI(EvalState5_GainTurnCapLate, 0, 0, 40, 1);

    PD(EvalState5_ThreatBase, 0.0, 0.0, 2.0);
    PD(EvalState5_ThreatWaRate, 0.0, -1.0, 2.5);
    PD(EvalState5_ThreatWbRate, 0.0, -1.0, 2.5);
    PD(EvalState5_ThreatWcRate, 0.0, -1.0, 3.0);
    PD(EvalState5_ThreatWdRate, 0.0, -1.0, 2.5);
    PD(EvalState5_ThreatMin, 0.0, 0.0, 1.5);
    PD(EvalState5_ThreatMax, 0.0, 0.0, 5.0);

    PD(EvalState5_StableRateEarly, 0.0, 0.0, 0.5);
    PD(EvalState5_StableRateLate, 0.0, 0.0, 0.5);
    PD(EvalState5_MovableValueRateEarly, 0.0, 0.0, 0.05);
    PD(EvalState5_MovableValueRateLate, 0.0, 0.0, 0.05);


    PD(EvalState4_GrowStockRate, 0.01829488364661022, 0.0, 0.08);
    PD(EvalState4_WeakRiskRate, -0.07928718082658412, -0.13, -0.05);
    PD(EvalState4_StableBonusRate, 0.27912977963652064, 0.0, 0.2);
    PD(EvalState4_NeutralThreatRateEarly, -0.19721453594646127, -0.2, -0.15);
    PD(EvalState4_NeutralThreatRateLate, -0.036685829336561976, 0.0, 0.2);
    PD(EvalState4_ContactPenaltyRate, 0.11308723332471697, 0.0, 0.07);
    PD(EvalState4_EnemyNearRate, -0.2516334139648044, -0.25, -0.15);
    PD(EvalState4_EnemyFarRate, 0.2821925637309507, 0.2, 0.3);
    PD(EvalState4_IslandPenaltyRate, 0.023326312157454607, -0.1, 0.0);
    PD(EvalState4_EarlyLowValueSafeRate, -0.03644518384950622, -0.05, 0.0);
    PD(EvalState4_LateHighValueSafeRate, -0.018264792146805, 0.0, 0.05);
    PD(EvalState4_EnemyWaThreatRate, 1.4055052280729683, 1.0, 2.0);
    PD(EvalState4_EnemyWbThreatRate, -1.6668617960511174, -2.8, -1.5);
    PD(EvalState4_EnemyWcThreatRate, -1.9705219656577204, -3.5, -2.5);
    PD(EvalState4_EnemyWdThreatRate, -0.13263070595149745, -1.2, -0.4);



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

template <int W, int H, class CellType = int>
struct GridSystemS {
   private:
    pint toPos_[W * H];
    CCA<CellType, 4> around_[W * H];  
    array<CellType, 4> dirs_[W * H];  

   public:
    constexpr GridSystemS() : toPos_{}, around_{}, dirs_{} {
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
            }
        }
    }

    inline constexpr CellType ToId(int x, int y) const {
        return x + W * y;
    }
    inline constexpr CellType ToId(const pint& p) const {
        return p.x + W * p.y;
    }
    inline constexpr const pint& ToPos(CellType id) const {
        return toPos_[id];
    }

    inline int CalcL1Dist(const pint& a, const pint& b) const {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }
    inline int CalcL1Dist(CellType a, CellType b) const {
        return CalcL1Dist(ToPos(a), ToPos(b));
    }

    inline int CalcL2Dist2(const pint& a, const pint& b) const {
        return square(a.x - b.x) + square(a.y - b.y);
    }
    inline int CalcL2Dist2(CellType a, CellType b) const {
        return CalcL2Dist2(ToPos(a), ToPos(b));
    }

    inline double CalcL2Dist(const pint& a, const pint& b) const {
        return sqrt(CalcL2Dist2(a, b));
    }
    inline double CalcL2Dist(CellType a, CellType b) const {
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

    inline CellType RotateRight90(CellType id) const {
        pint p = ToPos(id);
        return ToId(W - 1 - p.y, p.x);
    }

    inline Dir CalcDir(CellType from, CellType to) const {
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

    inline Dir CalcDirM(CellType from, CellType to) const {
        return ::CalcDirM(ToPos(from), ToPos(to));
    }

    inline constexpr const CCA<CellType, 4>& around(CellType id) const {
        return around_[id];
    }
    inline constexpr const array<CellType, 4>& dirs(CellType id) const {
        return dirs_[id];
    }
};
constexpr GridSystemS<N, N, u8> gs;



struct GameConst {
    int M;
    int U;
    NNBox<int> Vs;

#if RESTORE_MODE || TARGET_AUGMENT || TARGET_SELFPLAY || TARGET_PREDICT
    void AssertSame(const GameConst& r) const {
        VASSERT(M == r.M);
        VASSERT(U == r.U);
        VASSERT(Vs == r.Vs);
    }
#endif
};

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <random>
#include <unordered_set>
#include <utility>
#include <vector>

struct GenInput {
    int N = 10;
    int M = 0;
    int T = 100;
    int U = 0;
    std::vector<std::vector<int>> V;
    std::vector<std::pair<int, int>> xy;
    std::vector<double> wa;
    std::vector<double> wb;
    std::vector<double> wc;
    std::vector<double> wd;
    std::vector<double> eps;
    std::vector<std::vector<double>> r;  
};

inline GenInput Generate(uint64_t seed, std::optional<int> M_opt = std::nullopt,
                         std::optional<int> U_opt = std::nullopt) {
    struct Rng {
        std::mt19937_64 eng;
        explicit Rng(uint64_t s) : eng(s) {
        }

        int IntInclusive(int lo, int hi) {
            std::uniform_int_distribution<int> dist(lo, hi);
            return dist(eng);
        }
        int IntExclusive(int lo, int hi) {
            std::uniform_int_distribution<int> dist(lo, hi - 1);
            return dist(eng);
        }
        double RealExclusive(double lo, double hi) {
            const double hiOpen = std::nextafter(hi, lo);
            std::uniform_real_distribution<double> dist(lo, hiOpen);
            return dist(eng);
        }
    };

    Rng rng(seed);

    constexpr int kN = 10;
    constexpr int kT = 100;
    int M = rng.IntInclusive(2, 8);
    int U = rng.IntInclusive(1, 5);
    int K = rng.IntInclusive(0, 2);
    double a = rng.RealExclusive(0.0, 3.0);

    if (seed == 0) {
        M = 4;
        U = 3;
        K = 0;
        a = 1.0;
    }

    if (M_opt.has_value()) M = *M_opt;
    if (U_opt.has_value()) U = *U_opt;

    std::vector<std::vector<double>> V(kN, std::vector<double>(kN, 0.0));
    for (int i = 0; i < kN; ++i) {
        for (int j = 0; j < kN; ++j) {
            V[i][j] = std::pow(rng.RealExclusive(0.5, 1.0), a);
        }
    }

    for (int kk = 0; kk < K; ++kk) {
        const int x = rng.IntExclusive(0, kN);
        const int y = rng.IntExclusive(0, kN);
        const double b = rng.RealExclusive(1.0, 4.0);
        const int m = rng.IntInclusive(0, 4);
        const double R = rng.RealExclusive(1.0, 5.0);

        for (int i = 0; i < kN; ++i) {
            for (int j = 0; j < kN; ++j) {
                const double dx = static_cast<double>(i - x);
                const double dy = static_cast<double>(j - y);
                if (m == 0) {
                    V[i][j] += b * std::exp(-(dx * dx + dy * dy) / (2.0 * R * R));
                }
                else if (m == 1) {
                    V[i][j] += b / (1.0 + std::sqrt(dx * dx + dy * dy) / R);
                }
                else if (m == 2) {
                    if (dx * dx + dy * dy <= R * R) {
                        V[i][j] += b / 4.0;
                    }
                }
                else if (m == 3) {
                    V[i][j] += b / (1.0 + (std::abs(dx) + std::abs(dy)) / R);
                }
                else {
                    if (std::abs(dx) + std::abs(dy) <= R) {
                        V[i][j] += b / 4.0;
                    }
                }
            }
        }
    }

    double total = 0.0;
    for (int i = 0; i < kN; ++i) {
        for (int j = 0; j < kN; ++j) {
            total += V[i][j];
        }
    }
    const int target_sum = 1000 * kN * kN;

    std::vector<std::vector<int>> V_int(kN, std::vector<int>(kN, 0));
    for (int i = 0; i < kN; ++i) {
        for (int j = 0; j < kN; ++j) {
            V_int[i][j] = (int)std::ceil(V[i][j] * (double)target_sum / total);
        }
    }

    long long current_sum = 0;
    for (int i = 0; i < kN; ++i) {
        for (int j = 0; j < kN; ++j) {
            current_sum += V_int[i][j];
        }
    }
    long long remaining = current_sum - target_sum;
    while (remaining > 0) {
        const int i = rng.IntExclusive(0, kN);
        const int j = rng.IntExclusive(0, kN);
        if (V_int[i][j] > 1) {
            --V_int[i][j];
            --remaining;
        }
    }

    std::vector<std::pair<int, int>> xy(M, {0, 0});
    std::unordered_set<int> used;
    for (int i = 0; i < M; ++i) {
        while (true) {
            const int x = rng.IntExclusive(0, kN);
            const int y = rng.IntExclusive(0, kN);
            const int id = x * kN + y;
            if (used.insert(id).second) {
                xy[i] = {x, y};
                break;
            }
        }
    }

    const int ai_count = std::max(0, M - 1);
    std::vector<double> wa(ai_count, 0.0);
    std::vector<double> wb(ai_count, 0.0);
    std::vector<double> wc(ai_count, 0.0);
    std::vector<double> wd(ai_count, 0.0);
    std::vector<double> eps(ai_count, 0.0);
    for (int i = 0; i < ai_count; ++i) {
        wa[i] = rng.RealExclusive(0.3, 1.0);
        wb[i] = rng.RealExclusive(0.3, 1.0);
        wc[i] = rng.RealExclusive(0.3, 1.0);
        wd[i] = rng.RealExclusive(0.3, 1.0);
        eps[i] = rng.RealExclusive(0.1, 0.5);
    }

    std::vector<std::vector<double>> r(ai_count, std::vector<double>(2 * kT, 0.0));
    for (int i = 0; i < ai_count; ++i) {
        for (int j = 0; j < 2 * kT; ++j) {
            r[i][j] = rng.RealExclusive(0.0, 1.0);
        }
    }

    GenInput out;
    out.N = kN;
    out.M = M;
    out.T = kT;
    out.U = U;
    out.V = std::move(V_int);
    out.xy = std::move(xy);
    out.wa = std::move(wa);
    out.wb = std::move(wb);
    out.wc = std::move(wc);
    out.wd = std::move(wd);
    out.eps = std::move(eps);
    out.r = std::move(r);
    return out;
}

struct AIParam {
    double wa;
    double wb;
    double wc;
    double wd;
    double eps;

    bool operator==(const AIParam& r) const {
        return wa == r.wa && wb == r.wb && wc == r.wc && wd == r.wd && eps == r.eps;
    }
};

AIParam NormalizeParam(const AIParam& src) {
    AIParam out = src;
    out.wa = max(0.0, out.wa);
    out.wb = max(0.0, out.wb);
    out.wc = max(0.0, out.wc);
    out.wd = max(0.0, out.wd);
    const double sum = out.wa + out.wb + out.wc + out.wd;
    if (sum <= 1e-12) {
        out.wa = 1.0;
        out.wb = 1.0;
        out.wc = 1.0;
        out.wd = 1.0;
    }
    out.eps = clamp(out.eps, 0.0, 1.0);
    return out;
}

AIParam NormalizeParamSum1(const AIParam& src) {
    AIParam out = src;
    out.wa = max(0.0, out.wa);
    out.wb = max(0.0, out.wb);
    out.wc = max(0.0, out.wc);
    out.wd = max(0.0, out.wd);
    const double sum = out.wa + out.wb + out.wc + out.wd;
    if (sum <= 1e-12) {
        out.wa = 0.25;
        out.wb = 0.25;
        out.wc = 0.25;
        out.wd = 0.25;
    }
    else {
        out.wa /= sum;
        out.wb /= sum;
        out.wc /= sum;
        out.wd /= sum;
    }

    out.eps = clamp(out.eps, 0.0, 1.0);
    return out;
}




inline uint64_t RotateLeft(uint64_t bit, uint64_t shift) {
    return (bit << shift) | (bit >> (64ULL - shift));
}
inline uint64_t RotateRight(uint64_t bit, uint64_t shift) {
    return (bit >> shift) | (bit << (64ULL - shift));
}








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

    void clear() noexcept {
        reset();
    }
    void reset() noexcept {
        memset(arr_, 0, sizeof(arr_));
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


void CalcReachableMovable(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, int player,
                          bitset<NN>& reachable, NNSet& movableSet) {
    static thread_local NNQue<int> que;
    static thread_local NNCheck check;
    que.clear();
    check.Clear();

    reachable.reset();
    movableSet.clear();

    int start = poss[player];
    que.push(start);
    check.Check(start);
    reachable.set(start);
    movableSet.add(start);

    while (!que.empty()) {
        const int p = que.pop();
        for (int n : gs.around(p)) {
            if (check.Enter(n)) {
                movableSet.fadd(n);
                if (owners[n] == player) {
                    reachable.set(n);
                    que.push(n);
                }
            }
        }
    }

    REP(other, gc.M) {
        if (other != player) {
            movableSet.fremove(poss[other]);
        }
    }
}

void CalcReachableMovable(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, int player,
                          bitset<NN>& reachable, NNSet& movableSet, BitArr<NN>& excludeMovable) {
    static thread_local NNQue<u8> que;
    static thread_local NNCheck check;
    que.clear();
    check.Clear();

    reachable.reset();
    movableSet.clear();

    auto start = poss[player];
    que.push(start);
    check.Check(start);
    reachable.set(start);
    movableSet.add(start);

    while (!que.empty()) {
        const auto p = que.pop();
        for (auto n : gs.around(p)) {
            if (check.Enter(n)) {
                movableSet.fadd(n);
                if (owners[n] == player) {
                    reachable.set(n);
                    que.push(n);
                }
            }
        }
    }

    excludeMovable.clear();
    REP(other, gc.M) {
        if (other != player) {
            if (movableSet.contains(poss[other])) {
                movableSet.remove(poss[other]);
                excludeMovable.set(poss[other]);
            }
        }
    }
}

void CalcPlayersScore2(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, MArr<int>& scores) {
    scores.assign(gc.M, 0);
    REP(i, NN) {
        int owner = owners[i];
        if (owner >= 0) {
            scores[owner] += gc.Vs[i] * levels[i];
        }
    }
}

int CalcTargetM(const GameConst& gc, const MArr<int>& scores) {
    int targetM = 1;
    FOR(m, 2, gc.M) {
        if (scores[m] > scores[targetM]) {
            targetM = m;
        }
    }
    return targetM;
}

void CalcOrderdMs(const GameConst& gc, const MArr<int>& scores, MArr<int>& orderdMs) {
    orderdMs.clear();
    FOR(m, 1, gc.M) {
        orderdMs.push(m);
    }
    sort(ALL(orderdMs), [&](int m1, int m2) { return scores[m1] > scores[m2]; });
}

void CalcMsOrder(const GameConst& gc, const MArr<int>& scores, MArr<int>& msOrder) {
    MArr<int> orderdMs;
    CalcOrderdMs(gc, scores, orderdMs);
    msOrder.resize(gc.M);
    msOrder[0] = -1;
    REP(i, orderdMs.size()) {
        msOrder[orderdMs[i]] = i;
    }
}

void CalcOrders(const GameConst& gc, const MArr<int>& scores, MArr<int>& orderdMs, MArr<int>& msOrder) {
    CalcOrderdMs(gc, scores, orderdMs);
    msOrder.resize(gc.M);
    msOrder[0] = -1;
    REP(i, orderdMs.size()) {
        msOrder[orderdMs[i]] = i;
    }
}

double CalcCaseScore(const GameConst& gc, const MArr<int>& scores) {
    int myScore = scores[0];
    int maxOpponentScore = 0;
    FOR(m, 1, gc.M) {
        if (scores[m] > maxOpponentScore) {
            maxOpponentScore = scores[m];
        }
    }
    return 100000 * log2(1.0 + (myScore / (double)maxOpponentScore));
}

double CalcValueScore(const GameConst& gc, const MArr<int>& scores) {
    int myScore = scores[0];
    int maxOpponentScore = 0;
    FOR(m, 1, gc.M) {
        if (scores[m] > maxOpponentScore) {
            maxOpponentScore = scores[m];
        }
    }
    return (myScore - maxOpponentScore) / (double)(myScore + maxOpponentScore);
}
double CalcValueScore2(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels) {
    MArr<int> scores;
    CalcPlayersScore2(gc, owners, levels, scores);
    return CalcValueScore(gc, scores);
}

void CalcMovable(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, int player, NNArr<u8>& movable) {
    static thread_local NNQue<int> que;
    static thread_local NNCheck check;
    que.clear();
    check.Clear();

    movable.clear();

    int start = poss[player];
    que.push(start);
    check.Check(start);
    movable.push(start);

    while (!que.empty()) {
        const int p = que.pop();
        for (int n : gs.around(p)) {
            if (check.Enter(n)) {
                bool hasOtherPlayer = false;
                REP(other, gc.M) {
                    if (other != player && poss[other] == n) {
                        hasOtherPlayer = true;
                        break;
                    }
                }
                if (!hasOtherPlayer) {
                    movable.push(n);
                }

                if (owners[n] == player) {
                    que.push(n);
                }
            }
        }
    }
}

void CalcPlayerArea(const NNBox<s8>& owners, const MArr<s8>& poss, int player, NNArr<u8>& cells) {
    cells.clear();

    NNQue<int> que;
    NNCheck check;
    {
        int start = poss[player];
        que.push(start);
        check.Check(start);

        cells.push(start);
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.around(p)) {
            if (check.Enter(n)) {
                if (owners[n] == player) {
                    que.push(n);
                    cells.push(n);
                }
            }
        }
    }
}

constexpr u8 InfDist = 255;
void CalcDist(const NNArr<u8>& starts, NNBox<u8>& dists) {
    NNQue<int> que;
    dists.fill(InfDist);
    for (int start : starts) {
        que.push(start);
        dists[start] = 0;
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.around(p)) {
            int d = dists[p] + 1;
            if (d < dists[n]) {
                que.push(n);
                dists[n] = d;
            }
        }
    }
}

void CalcDistMyArea(const NNBox<s8>& owners, int player, const NNArr<u8>& starts, NNBox<u8>& dists) {
    NNQue<int> que;
    dists.fill(InfDist);
    for (int start : starts) {
        que.push(start);
        dists[start] = 0;
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.around(p)) {
            if (owners[n] == player || owners[n] < 0) {
                int d = dists[p] + 1;
                if (d < dists[n]) {
                    que.push(n);
                    dists[n] = d;
                }
            }
        }
    }
}

void CalcSafeArea(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, NNBox<bool>& isSafe) {
    isSafe.fill(true);
    NNArr<u8> cells;
    FOR(m, 1, gc.M) {
        CalcPlayerArea(owners, poss, m, cells);
        for (int cell : cells) {
            isSafe[cell] = false;
        }
    }
}

void CalcMovableSafeArea(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss,
                         NNBox<bool>& isMovableSafe) {
    NNBox<bool> isSafe;
    CalcSafeArea(gc, owners, poss, isSafe);

    NNQue<u8> que;
    static thread_local NNCheck check;
    check.Clear();
    isMovableSafe.fill(false);
    {
        int start = poss[0];
        que.push(start);
        check.Check(start);
        isMovableSafe[start] = true;
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.around(p)) {
            if (check.Enter(n)) {
                if (isSafe[n]) {
                    isMovableSafe[n] = true;
                    que.push(n);
                }
            }
        }
    }
}


void CalcMovableSafeAreaTurnReverse(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                                    const MArr<s8>& poss, const NNArr<u8>& starts, NNBox<u8>& costs) {

    NNBox<bool> isSafe;
    CalcSafeArea(gc, owners, poss, isSafe);

    struct Node {
        s8 p;
        u8 cost;
        bool operator<(const Node& n) const {
            return cost > n.cost;
        }
    };

    static thread_local PriorityQueue<Node> que;
    costs.fill(InfDist);
    que.Clear();

    for (int start : starts) {
        VASSERT(isSafe[start]);
        VASSERT(owners[start] != 0);  
        costs[start] = 0;
        que.push(Node{(s8)start, 0});
    }

    while (!que.empty()) {
        Node node = que.top();
        que.pop();

        if (costs[node.p] != node.cost) {
            continue;
        }

        for (int n : gs.around(node.p)) {
            if (!isSafe[n]) {
                continue;
            }

            u8 cost = node.cost;
            if (owners[node.p] >= 0 && owners[node.p] != 0) {
                cost += levels[node.p];
            }
            else {
                cost += 1;
            }

            if (cost < costs[n]) {
                costs[n] = cost;
                que.push(Node{(s8)n, cost});

            }
        }
    }
}

void CalcGettableMovable(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, int player,
                         const NNArr<u8>& srcMovable, NNArr<u8>& movable) {
    movable.clear();
    for (int cell : srcMovable) {
        if (levels[cell] < gc.U || owners[cell] != player) {
            movable.push(cell);
        }
    }

    if (movable.empty()) {
        movable = srcMovable;
    }
}
void CalcGettableMovable(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                         int player, NNArr<u8>& movable) {
    NNArr<u8> tmpMovable;
    CalcMovable(gc, owners, poss, player, tmpMovable);
    CalcGettableMovable(gc, owners, levels, player, tmpMovable, movable);
}

int SampleByProbOnMovable(const NNArr<u8>& movable, const NNBox<double>& probs, Xor64& rand) {
    double total = 0.0;
    for (int cell : movable) {
        const double p = probs[cell];
        if (p > 0.0) {
            total += p;
        }
    }

    const double r = rand.GetDouble() * total;
    double acc = 0.0;
    for (auto cell : movable) {
        const double p = probs[cell];
        if (p > 0.0) {
            acc += p;
            if (r <= acc) {
                return cell;
            }
        }
    }
    return movable.back();
}

double EvalAICellScore(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, int player, int cell,
                       const AIParam& prm) {
    const auto owner = owners[cell];
    const auto level = levels[cell];
    const auto value = gc.Vs[cell];
    if (owner < 0) {
        return value * prm.wa;
    }
    if (owner == player) {
        return (level < gc.U) ? (value * prm.wb) : 0.0;
    }
    return (level == 1) ? (value * prm.wc) : (value * prm.wd);
}

int SampleByEstimatedModelWithTwoRandoms(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                                         int player, const NNArr<u8>& movable, const AIParam& prm, double r1,
                                         double r2) {
    VASSERT(!movable.empty());

    const double eps = clamp(prm.eps, 0.0, 1.0);
    const double rr1 = clamp(r1, 0.0, 1.0 - 1e-15);
    const double rr2 = clamp(r2, 0.0, 1.0 - 1e-15);

    if (rr1 < eps) {
        const int idx = min((int)(rr2 * (double)movable.size()), (int)movable.size() - 1);
        return movable[idx];
    }

    array<double, NN> scores{};
    double maxScore = -1e100;
    REP(i, (int)movable.size()) {
        const int cell = movable[i];
        const double score = EvalAICellScore(gc, owners, levels, player, cell, prm);
        scores[i] = score;
        maxScore = max(maxScore, score);
    }
    const double tol = 1e-9 * max(1.0, fabs(maxScore));
    const double threshold = maxScore - tol;

    int bestCount = 0;
    REP(i, (int)movable.size()) {
        if (scores[i] >= threshold) {
            bestCount++;
        }
    }

    if (bestCount <= 0) {
        const int idx = min((int)(rr2 * (double)movable.size()), (int)movable.size() - 1);
        return movable[idx];
    }

    const int pickOrder = min((int)(rr2 * (double)bestCount), bestCount - 1);
    int seen = 0;
    REP(i, (int)movable.size()) {
        if (scores[i] < threshold) {
            continue;
        }
        if (seen == pickOrder) {
            return movable[i];
        }
        seen++;
    }

    return movable.back();
}

int SampleByEstimatedModel(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, int player,
                           const NNArr<u8>& movable, const AIParam& prm, Xor64& rand) {
    const double r1 = rand.GetDouble();
    const double r2 = rand.GetDouble();
    return SampleByEstimatedModelWithTwoRandoms(gc, owners, levels, player, movable, prm, r1, r2);
}

void CalcAIMoveProb(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, int player,
                    const NNArr<u8>& movable, const AIParam& prm, NNBox<double>& outProb) {
    outProb.fill(0.0);

    NNArr<double> scores;
    scores.resize(movable.size());
    double maxScore = -1e100;
    REP(i, movable.size()) {
        const int cell = movable[i];
        double score = EvalAICellScore(gc, owners, levels, player, cell, prm);
        scores[i] = score;
        maxScore = max(maxScore, score);
    }

    const double tol = 1e-9 * max(1.0, fabs(maxScore));
    int bestCount = 0;
    REP(i, movable.size()) {
        if (scores[i] >= maxScore - tol) {
            bestCount++;
        }
    }
    if (bestCount <= 0) {
        return;
    }

    const double eps = clamp(prm.eps, 0.0, 1.0);
    const double randomPart = eps / (double)movable.size();
    const double greedyPart = (1.0 - eps) / (double)bestCount;

    REP(i, movable.size()) {
        const int cell = movable[i];
        outProb[cell] += randomPart;
        if (scores[i] >= maxScore - tol) {
            outProb[cell] += greedyPart;
        }
    }
}

void CalcExpectScoreMap(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                        const MArr<int>& scores, const MArr<NNSet>& movableSets,
                        const MArr<NNBox<double>>& playersProbs, int targetM, NNBox<double>& outValue) {
    NNBox<double> otherProbs;
    {
        otherProbs.fill(1.0);
        FOR(m, 1, gc.M) {
            if (m == targetM) {
                continue;
            }
            for (auto cell : movableSets[m]) {
                otherProbs[cell] *= (1.0 - playersProbs[m][cell]);
            }
        }
        REP(i, NN) {
            otherProbs[i] = 1.0 - otherProbs[i];
        }
    }

    NNBox<double> otherSingleProbs;
    {
        REP(i, NN) {
            double p0 = 1.0;  
            double p1 = 0.0;  
            FOR(m, 1, gc.M) {
                if (m == targetM) {
                    continue;
                }
                double p = playersProbs[m][i];
                double np1 = p1 * (1.0 - p) + p0 * p;
                double np0 = p0 * (1.0 - p);
                p0 = np0;
                p1 = np1;
            }
            otherSingleProbs[i] = p1;
        }
    }

    auto CalcScoreDelta = [&](int myCell, int opCell, double otherProb, int myPlayer, int opPlayer,
                              double& myScoreDelta, double& opScoreDelta) {
        myScoreDelta = 0;
        opScoreDelta = 0;

        double okProb;
        if (owners[myCell] == myPlayer) {
            okProb = 1.0;
        }
        else {
            if (myCell == opCell) {
                okProb = 0.0;
            }
            else {
                okProb = 1.0 - otherProb;
            }
        }

        if (okProb > 0) {
            if (owners[myCell] == myPlayer) {
                if (levels[myCell] < gc.U) {
                    myScoreDelta += gc.Vs[myCell] * okProb;  
                }
                else {
                }
            }
            else if (owners[myCell] < 0) {
                myScoreDelta += gc.Vs[myCell] * okProb;
            }
            else {
                if (levels[myCell] == 1) {
                    myScoreDelta += gc.Vs[myCell] * okProb;
                }
                else {
                }

                if (owners[myCell] == opPlayer) {
                    opScoreDelta -= gc.Vs[myCell] * okProb;
                }
            }
        }

    };

    for (auto myCell : movableSets[0]) {

        double myScore = scores[0];
        double opScore = scores[targetM];

        for (auto opCell : movableSets[targetM]) {
            double opProb = playersProbs[targetM][opCell];

            {
                double myScoreDelta;
                double opScoreDelta;
                CalcScoreDelta(myCell, opCell, otherProbs[myCell], 0, targetM, myScoreDelta, opScoreDelta);
                myScore += myScoreDelta * opProb;
                opScore += opScoreDelta * opProb;
            }

            {
                double myScoreDelta;
                double opScoreDelta;
                CalcScoreDelta(opCell, myCell, otherProbs[opCell], targetM, 0, opScoreDelta, myScoreDelta);
                myScore += myScoreDelta * opProb;
                opScore += opScoreDelta * opProb;
            }

            REP(otherCell, NN) {
                if (otherCell == myCell || otherCell == opCell) {
                }
                else {
                    double singleEventProb = otherSingleProbs[otherCell] * opProb;

                    if (owners[otherCell] == 0) {
                        myScore -= gc.Vs[otherCell] * singleEventProb;
                    }
                    else if (owners[otherCell] == targetM) {
                        opScore -= gc.Vs[otherCell] * singleEventProb;
                    }
                }
            }
        }

        VASSERT(opScore > 0);  
        outValue[myCell] = 100000 * log2(1.0 + (myScore / (double)opScore));
    }
}

void CalcExpectScoreMapFast(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                            const MArr<int>& scores, const MArr<NNSet>& movableSets,
                            const MArr<NNBox<double>>& playersProbs, int targetM, NNBox<double>& outValue) {
    NNBox<double> otherProbs;
    {
        otherProbs.fill(1.0);
        FOR(m, 1, gc.M) {
            if (m == targetM) {
                continue;
            }
            for (auto cell : movableSets[m]) {
                otherProbs[cell] *= (1.0 - playersProbs[m][cell]);
            }
        }
        REP(i, NN) {
            otherProbs[i] = 1.0 - otherProbs[i];
        }
    }

    NNBox<double> otherSingleProbs;
    {
        REP(i, NN) {
            double p0 = 1.0;  
            double p1 = 0.0;  
            FOR(m, 1, gc.M) {
                if (m == targetM) {
                    continue;
                }
                double p = playersProbs[m][i];
                double np1 = p1 * (1.0 - p) + p0 * p;
                double np0 = p0 * (1.0 - p);
                p0 = np0;
                p1 = np1;
            }
            otherSingleProbs[i] = p1;
        }
    }

    NNBox<double> myMoveNoCollisionMyScoreDelta{};  
    NNBox<double> myMoveNoCollisionOpScoreDelta{};  
    NNBox<double> opMoveNoCollisionMyScoreDelta{};  
    NNBox<double> opMoveNoCollisionOpScoreDelta{};  

    NNBox<double> otherMoveMyScoreDelta{};
    NNBox<double> otherMoveOpScoreDelta{};
    double otherMovemyScoreDeltaSum = 0.0;
    double otherMoveOpScoreDeltaSum = 0.0;

    REP(cell, NN) {
        const int owner = owners[cell];
        const int level = levels[cell];
        const double v = gc.Vs[cell];
        const double okProb = 1.0 - otherProbs[cell];

        if (owner == 0) {
            if (level < gc.U) {
                myMoveNoCollisionMyScoreDelta[cell] = v;
            }
        }
        else if (owner < 0) {
            myMoveNoCollisionMyScoreDelta[cell] = v * okProb;
        }
        else {
            if (level == 1) {
                myMoveNoCollisionMyScoreDelta[cell] = v * okProb;
            }
            if (owner == targetM) {
                myMoveNoCollisionOpScoreDelta[cell] = -v * okProb;
            }
        }

        if (owner == targetM) {
            if (level < gc.U) {
                opMoveNoCollisionOpScoreDelta[cell] = v;
            }
        }
        else if (owner < 0) {
            opMoveNoCollisionOpScoreDelta[cell] = v * okProb;
        }
        else {
            if (level == 1) {
                opMoveNoCollisionOpScoreDelta[cell] = v * okProb;
            }
            if (owner == 0) {
                opMoveNoCollisionMyScoreDelta[cell] = -v * okProb;
            }
        }

        const double singleProb = otherSingleProbs[cell];
        if (owner == 0) {
            const double delta = -v * singleProb;
            otherMoveMyScoreDelta[cell] = delta;
            otherMovemyScoreDeltaSum += delta;
        }
        else if (owner == targetM) {
            const double delta = -v * singleProb;
            otherMoveOpScoreDelta[cell] = delta;
            otherMoveOpScoreDeltaSum += delta;
        }
    }

    NNBox<double> targetOpProbs{};
    double targetOpProbSum = 0.0;
    double weightedOtherMoveMyByOp = 0.0;
    double weightedOtherMoveOpByOp = 0.0;
    double opMoveNoCollisionMyScoreDeltaSum = 0.0;
    double opMoveNoCollisionOpScoreDeltaSum = 0.0;
    for (auto opCell : movableSets[targetM]) {
        const double opProb = playersProbs[targetM][opCell];
        targetOpProbs[opCell] += opProb;
        targetOpProbSum += opProb;

        weightedOtherMoveMyByOp += otherMoveMyScoreDelta[opCell] * opProb;
        weightedOtherMoveOpByOp += otherMoveOpScoreDelta[opCell] * opProb;
        opMoveNoCollisionMyScoreDeltaSum += opMoveNoCollisionMyScoreDelta[opCell] * opProb;
        opMoveNoCollisionOpScoreDeltaSum += opMoveNoCollisionOpScoreDelta[opCell] * opProb;
    }

    const double otherMoveMyScoreBase = otherMovemyScoreDeltaSum * targetOpProbSum - weightedOtherMoveMyByOp;
    const double otherMoveOpScoreBase = otherMoveOpScoreDeltaSum * targetOpProbSum - weightedOtherMoveOpByOp;

    for (auto myCell : movableSets[0]) {

        double myScore = scores[0];
        double opScore = scores[targetM];

        const double opProbAtMyCell = targetOpProbs[myCell];
        const double opProbExceptMyCell = targetOpProbSum - opProbAtMyCell;

        myScore += opMoveNoCollisionMyScoreDeltaSum;
        opScore += opMoveNoCollisionOpScoreDeltaSum;

        if (owners[myCell] != targetM && opProbAtMyCell > 0) {
            myScore -= opMoveNoCollisionMyScoreDelta[myCell] * opProbAtMyCell;
            opScore -= opMoveNoCollisionOpScoreDelta[myCell] * opProbAtMyCell;
        }

        myScore += otherMoveMyScoreBase - otherMoveMyScoreDelta[myCell] * opProbExceptMyCell;
        opScore += otherMoveOpScoreBase - otherMoveOpScoreDelta[myCell] * opProbExceptMyCell;

        const double myMoveProb = (owners[myCell] == 0) ? targetOpProbSum : opProbExceptMyCell;
        myScore += myMoveNoCollisionMyScoreDelta[myCell] * myMoveProb;
        opScore += myMoveNoCollisionOpScoreDelta[myCell] * myMoveProb;

        VASSERT(opScore > 0);  
        outValue[myCell] = 100000 * log2(1.0 + (myScore / (double)opScore));
    }
}

namespace {
struct GuardEdge {
    int to;
    int rev;
    long long cap;
};

constexpr long long GuardINF = (1LL << 60);
constexpr long long GuardBaseCost = 10000;  
constexpr long long GuardInsideCost = 1;    
constexpr int GuardNodesPerCell = 2;
constexpr int GuardNodeCount = GuardNodesPerCell * NN + 2;
constexpr int GuardSrc = GuardNodesPerCell * NN;
constexpr int GuardSink = GuardSrc + 1;

void CollectEnemyPieceCells(const GameConst& gc, const MArr<s8>& poss, NNArr<u8>& outCells) {
    outCells.clear();
    array<u8, NN> used{};
    FOR(m, 1, gc.M) {
        const int cell = poss[m];
        if (0 <= cell && cell < NN && !used[cell]) {
            used[cell] = 1;
            outCells.push((u8)cell);
        }
    }
}

void AddInnerCell(array<u8, NN>& isRequiredInner, int x, int y) {
    if (0 <= x && x < N && 0 <= y && y < N) {
        isRequiredInner[y * N + x] = 1;
    }
}

void AddBresenhamLine(array<u8, NN>& isRequiredInner, int fromCell, int toCell) {
    int x0 = fromCell % N;
    int y0 = fromCell / N;
    const int x1 = toCell % N;
    const int y1 = toCell / N;

    const int dx = abs(x1 - x0);
    const int sx = (x0 < x1) ? 1 : -1;
    const int dy = -abs(y1 - y0);
    const int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    AddInnerCell(isRequiredInner, x0, y0);
    while (!(x0 == x1 && y0 == y1)) {
        const int prevX = x0;
        const int prevY = y0;

        const int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }

        if (x0 != prevX && y0 != prevY) {
            const int dA = abs(x0 - x1) + abs(prevY - y1);
            const int dB = abs(prevX - x1) + abs(y0 - y1);
            if (dA <= dB) {
                AddInnerCell(isRequiredInner, x0, prevY);
            }
            else {
                AddInnerCell(isRequiredInner, prevX, y0);
            }
        }

        AddInnerCell(isRequiredInner, x0, y0);
    }
}

void ConnectCellsByMSTWithBresenham(const NNArr<u8>& seedCells, array<u8, NN>& isRequiredInner) {
    const int seedCount = (int)seedCells.size();
    if (seedCount <= 1) {
        return;
    }

    vector<int> bestCost(seedCount, numeric_limits<int>::max());
    vector<int> parent(seedCount, -1);
    vector<u8> used(seedCount, 0);
    bestCost[0] = 0;

    REP(step, seedCount) {
        int v = -1;
        REP(i, seedCount) {
            if (used[i]) {
                continue;
            }
            if (v < 0 || bestCost[i] < bestCost[v]) {
                v = i;
            }
        }
        if (v < 0) {
            break;
        }

        used[v] = 1;
        if (parent[v] >= 0) {
            AddBresenhamLine(isRequiredInner, (int)seedCells[v], (int)seedCells[parent[v]]);
        }

        const int vx = (int)seedCells[v] % N;
        const int vy = (int)seedCells[v] / N;
        REP(to, seedCount) {
            if (used[to]) {
                continue;
            }
            const int tx = (int)seedCells[to] % N;
            const int ty = (int)seedCells[to] / N;
            const int ddx = vx - tx;
            const int ddy = vy - ty;
            const int lineCost = abs(ddx) + abs(ddy);
            if (lineCost < bestCost[to]) {
                bestCost[to] = lineCost;
                parent[to] = v;
            }
        }
    }
}

void BuildRequiredInnerByEnemyArea(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss,
                                   array<u8, NN>& isRequiredInner) {
    isRequiredInner.fill(0);

    NNArr<u8> areaCells;
    array<u8, NN> areaAdded{};

    FOR(m, 1, gc.M) {
        if (poss[m] < 0 || poss[m] >= NN) {
            continue;
        }
        NNArr<u8> enemyArea;
        CalcPlayerArea(owners, poss, m, enemyArea);

        for (int cell : enemyArea) {
            isRequiredInner[cell] = 1;
            if (!areaAdded[cell]) {
                areaAdded[cell] = 1;
                areaCells.push((u8)cell);
            }
        }
    }

    ConnectCellsByMSTWithBresenham(areaCells, isRequiredInner);
}

void BuildMyReachableOwnOrNeutral(const NNBox<s8>& owners, const MArr<s8>& poss, array<u8, NN>& canUseCell) {
    canUseCell.fill(0);
    if (poss.empty()) {
        return;
    }

    const int start = (int)poss[0];
    if (start < 0 || start >= NN) {
        return;
    }

    queue<int> que;
    que.push(start);
    canUseCell[start] = 1;

    while (!que.empty()) {
        const int cell = que.front();
        que.pop();
        for (int nc : gs.around(cell)) {
            if (canUseCell[nc]) {
                continue;
            }
            if (owners[nc] == 0 || owners[nc] < 0) {
                canUseCell[nc] = 1;
                que.push(nc);
            }
        }
    }
}

void SolveGuardByRequiredInner(const NNBox<s8>& dummy, const MArr<s8>& poss, const array<u8, NN>& isRequiredInner,
                               NNArr<u8>& guardCells, NNArr<u8>* insideCells = nullptr,
                               NNArr<u8>* outsideCells = nullptr, const array<bool, NN>* canUseCell = nullptr) {
    guardCells.clear();
    if (insideCells) {
        insideCells->clear();
    }
    if (outsideCells) {
        outsideCells->clear();
    }

    array<u8, NN> blocked{};
    REP(cell, NN) {
        const bool notReachable = (canUseCell != nullptr && !(*canUseCell)[cell]);
        blocked[cell] = (isRequiredInner[cell] || notReachable) ? 1 : 0;
    }

    bool hasInner = false;
    REP(cell, NN) {
        if (blocked[cell]) {
            hasInner = true;
            break;
        }
    }
    if (!hasInner) {
        return;
    }

    vector<vector<GuardEdge>> graph(GuardNodeCount);
    auto AddEdge = [&](int from, int to, long long cap) {
        GuardEdge fwd{to, (int)graph[to].size(), cap};
        GuardEdge rev{from, (int)graph[from].size(), 0};
        graph[from].push_back(fwd);
        graph[to].push_back(rev);
    };

    (void)poss;
    REP(cell, NN) {
        const int in = cell * 2;
        const int out = in + 1;
        if (blocked[cell]) {
            AddEdge(in, out, GuardINF);
        }
        else {
            AddEdge(in, out, GuardBaseCost);
        }
    }

    REP(cell, NN) {
        const int out = cell * 2 + 1;
        for (int nc : gs.around(cell)) {
            AddEdge(out, nc * 2, GuardINF);
        }
    }

    REP(cell, NN) {
        if (isRequiredInner[cell]) {
            AddEdge(GuardSrc, cell * 2, GuardINF);
        }
    }

    REP(cell, NN) {
        if (!blocked[cell]) {
            AddEdge(cell * 2 + 1, GuardSink, GuardInsideCost);
        }
    }

    REP(cell, NN) {
        const int x = cell % N;
        const int y = cell / N;
        if ((x == 0 || x == N - 1 || y == 0 || y == N - 1) && !blocked[cell]) {
            AddEdge(cell * 2 + 1, GuardSink, GuardINF);
        }
    }

    vector<int> dinicLevel(GuardNodeCount), iter(GuardNodeCount);
    auto BfsLevel = [&]() -> bool {
        fill(dinicLevel.begin(), dinicLevel.end(), -1);
        queue<int> que;
        dinicLevel[GuardSrc] = 0;
        que.push(GuardSrc);
        while (!que.empty()) {
            const int v = que.front();
            que.pop();
            for (const auto& e : graph[v]) {
                if (e.cap > 0 && dinicLevel[e.to] < 0) {
                    dinicLevel[e.to] = dinicLevel[v] + 1;
                    que.push(e.to);
                }
            }
        }
        return dinicLevel[GuardSink] >= 0;
    };

    function<long long(int, long long)> DfsFlow = [&](int v, long long f) -> long long {
        if (v == GuardSink) {
            return f;
        }
        for (int& i = iter[v]; i < (int)graph[v].size(); ++i) {
            auto& e = graph[v][i];
            if (e.cap <= 0 || dinicLevel[e.to] != dinicLevel[v] + 1) {
                continue;
            }
            const long long pushed = DfsFlow(e.to, min(f, e.cap));
            if (pushed <= 0) {
                continue;
            }
            e.cap -= pushed;
            graph[e.to][e.rev].cap += pushed;
            return pushed;
        }
        return 0;
    };

    while (BfsLevel()) {
        fill(iter.begin(), iter.end(), 0);
        while (true) {
            const long long f = DfsFlow(GuardSrc, GuardINF);
            if (f <= 0) {
                break;
            }
        }
    }

    vector<u8> reachable(GuardNodeCount, 0);
    queue<int> que;
    que.push(GuardSrc);
    reachable[GuardSrc] = 1;
    while (!que.empty()) {
        const int v = que.front();
        que.pop();
        for (const auto& e : graph[v]) {
            if (e.cap > 0 && !reachable[e.to]) {
                reachable[e.to] = 1;
                que.push(e.to);
            }
        }
    }

    REP(cell, NN) {
        const int in = cell * 2;
        const int out = in + 1;
        const bool isGuard = !blocked[cell] && reachable[in] && !reachable[out];
        if (isGuard) {
            guardCells.push((u8)cell);
        }
        else if (insideCells && reachable[in]) {
            insideCells->push((u8)cell);
        }
        else if (outsideCells) {
            outsideCells->push((u8)cell);
        }
    }
}
}  

void CalcGuardCells(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, NNArr<u8>& guardCells,
                    NNArr<u8>& insideCells, NNArr<u8>& outsideCells) {
    if (gc.M <= 1) {
        guardCells.clear();
        insideCells.clear();
        outsideCells.clear();
        return;
    }

    array<u8, NN> isRequiredInner{};
    NNArr<u8> enemyPieceCells;
    CollectEnemyPieceCells(gc, poss, enemyPieceCells);
    for (int cell : enemyPieceCells) {
        isRequiredInner[cell] = 1;
    }
    ConnectCellsByMSTWithBresenham(enemyPieceCells, isRequiredInner);
    SolveGuardByRequiredInner(owners, poss, isRequiredInner, guardCells, &insideCells, &outsideCells);
}

void CalcGuardCells(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, NNArr<u8>& guardCells) {
    if (gc.M <= 1) {
        guardCells.clear();
        return;
    }

    array<u8, NN> isRequiredInner{};
    NNArr<u8> enemyPieceCells;
    CollectEnemyPieceCells(gc, poss, enemyPieceCells);
    for (int cell : enemyPieceCells) {
        isRequiredInner[cell] = 1;
    }
    ConnectCellsByMSTWithBresenham(enemyPieceCells, isRequiredInner);
    SolveGuardByRequiredInner(owners, poss, isRequiredInner, guardCells, nullptr, nullptr);
}

void CalcGuardCellsByEnemyArea(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss,
                               NNArr<u8>& guardCells, NNArr<u8>& insideCells, NNArr<u8>& outsideCells) {
    if (gc.M <= 1) {
        guardCells.clear();
        insideCells.clear();
        outsideCells.clear();
        return;
    }

    array<u8, NN> isRequiredInner{};
    BuildRequiredInnerByEnemyArea(gc, owners, poss, isRequiredInner);

    SolveGuardByRequiredInner(owners, poss, isRequiredInner, guardCells, &insideCells, &outsideCells);
}

void CalcGuardCellsByEnemyArea(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss,
                               NNArr<u8>& guardCells) {
    if (gc.M <= 1) {
        guardCells.clear();
        return;
    }

    array<u8, NN> isRequiredInner{};
    BuildRequiredInnerByEnemyArea(gc, owners, poss, isRequiredInner);

    SolveGuardByRequiredInner(owners, poss, isRequiredInner, guardCells, nullptr, nullptr);
}

void CalcGuardCellsByEnemyAreaReachable(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss,
                                        NNArr<u8>& guardCells, NNArr<u8>& insideCells, NNArr<u8>& outsideCells) {
    if (gc.M <= 1) {
        guardCells.clear();
        insideCells.clear();
        outsideCells.clear();
        return;
    }

    array<u8, NN> isRequiredInner{};
    BuildRequiredInnerByEnemyArea(gc, owners, poss, isRequiredInner);

    array<bool, NN> canUseCell{};
    CalcMovableSafeArea(gc, owners, poss, canUseCell);

    SolveGuardByRequiredInner(owners, poss, isRequiredInner, guardCells, &insideCells, &outsideCells, &canUseCell);
}

void CalcGuardCellsByEnemyAreaReachable(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss,
                                        NNArr<u8>& guardCells) {
    if (gc.M <= 1) {
        guardCells.clear();
        return;
    }

    array<u8, NN> isRequiredInner{};
    BuildRequiredInnerByEnemyArea(gc, owners, poss, isRequiredInner);

    array<bool, NN> canUseCell{};
    CalcMovableSafeArea(gc, owners, poss, canUseCell);

    SolveGuardByRequiredInner(owners, poss, isRequiredInner, guardCells, nullptr, nullptr, &canUseCell);
}

struct Game {
    u8 turn_;
    NNBox<s8> owners_;  
    NNBox<s8> levels_;  
    MArr<s8> poss_;     

#if RESTORE_MODE || TARGET_AUGMENT || TARGET_SELFPLAY || TARGET_PREDICT
    void AssertSame(const Game& r) const {
        VASSERT(turn_ == r.turn_);
        VASSERT(owners_ == r.owners_);
        VASSERT(levels_ == r.levels_);
        VASSERT(poss_ == r.poss_);
    }
#endif


    void Tick(const GameConst& gc, const MArr<AIParam>& aiParams, u8 command, MArr<s8>& playersTarget, Xor64& rand) {
        MArr<s8> moves(gc.M);
        moves[0] = command;

        NNArr<u8> movable;
        FOR(m, 1, gc.M) {
            ::CalcMovable(gc, owners_, poss_, m, movable);
            moves[m] = SampleByEstimatedModel(gc, owners_, levels_, m, movable, aiParams[m], rand);
        }

        playersTarget.resize(gc.M);
        REP(m, gc.M) {
            playersTarget[m] = moves[m];
        }


        array<int, NN> moveCounts{};
        REP(p, gc.M) {
            moveCounts[moves[p]]++;
        }

        array<u8, M_max> collected{};

        REP(p, gc.M) {
            const int cell = moves[p];
            if (moveCounts[cell] >= 2) {
                const int owner = (int)owners_[cell];
                if (owner != p) {
                    collected[p] = 1;
                }
            }
        }

        REP(p, gc.M) {
            if (collected[p]) continue;

            const int cell = moves[p];
            const int owner = (int)owners_[cell];

            if (owner < 0) {
                owners_[cell] = (s8)p;
                levels_[cell] = 1;
            }
            else if (owner == p) {
                if (levels_[cell] < gc.U) {
                    levels_[cell]++;
                }
            }
            else {
                levels_[cell]--;
                if (levels_[cell] <= 0) {
                    owners_[cell] = (s8)p;
                    levels_[cell] = 1;
                }
                else {
                    collected[p] = 1;
                }
            }
        }

        REP(p, gc.M) {
            if (collected[p]) {
            }
            else {
                poss_[p] = (s8)moves[p];
            }
        }

        ++turn_;
    }

    void CalcMovable(const GameConst& gc, int player, NNArr<u8>& movable) const {
        ::CalcMovable(gc, owners_, poss_, player, movable);
    }

    bool IsGameOver() const {
        if (turn_ >= T) {
            return true;  
        }
        return false;
    }

    void GetScores(const GameConst& gc, MArr<int>& scores) const {
        scores.assign(gc.M, 0);
        REP(i, NN) {
            const int owner = (int)owners_[i];
            if (owner >= 0) {
                scores[owner] += gc.Vs[i] * levels_[i];
            }
        }
    }

    void TurnEnd() {
        ++turn_;
    }

    bool IsEnd() const {
        return IsGameOver();
    }
};
constexpr int GameSize = sizeof(Game);

#if TARGET_SELFPLAY || TARGET_BATTLE || TARGET_PREDICT || TARGET_TUNING
void MakeInit(GameConst& gc, MArr<AIParam>& aiParams, Game& game, Xor64& rand, std::optional<int> M_opt = std::nullopt,
              std::optional<int> U_opt = std::nullopt) {
    game.turn_ = 0;
    auto seed = rand.Get64();
    auto input = Generate(seed, M_opt, U_opt);

    gc.M = input.M;
    gc.U = input.U;
    REP(i, NN) {
        int y = i / N;
        int x = i % N;
        gc.Vs[i] = input.V[y][x];
    }
    game.owners_.fill(-1);
    game.levels_.fill(0);

    VASSERT(input.xy.size() == gc.M);

    game.poss_.clear();
    REP(i, gc.M) {
        auto [y, x] = input.xy[i];
        int id = y * N + x;
        game.owners_[id] = i;
        game.levels_[id] = 1;
        game.poss_.push_back((s8)id);
    }

    aiParams.resize(gc.M);
    REP(i, gc.M - 1) {
        aiParams[i + 1].wa = input.wa[i];
        aiParams[i + 1].wb = input.wb[i];
        aiParams[i + 1].wc = input.wc[i];
        aiParams[i + 1].wd = input.wd[i];
        aiParams[i + 1].eps = input.eps[i];
    }
}
#endif

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

struct IOServer {

    GameConst gc_;
    Game game_;

    bool HasOracleParams() const {
        return false;
    }

    bool TryGetOracleParam(int player, AIParam& out) const {
        VABORT();
        (void)player;
        (void)out;
        return false;
    }

    void InitInput(ChronoTimer& timer) {
        istream& is = cin;

        int dummyT;
        is >> dummyT;
        timer.Init();  
        is >> gc_.M >> dummyT >> gc_.U;

        game_.turn_ = 0;
        game_.owners_.fill(-1);
        game_.levels_.fill(0);

        REP(i, NN) {
            is >> gc_.Vs[i];
        }

        game_.poss_.resize(gc_.M);
        REP(i, gc_.M) {
            int row, col;
            is >> row >> col;
            game_.poss_[i] = (s8)gs.ToId(col, row);
        }

        REP(i, gc_.M) {
            int pos = game_.poss_[i];
            game_.owners_[pos] = (s8)i;
            game_.levels_[pos] = 1;
        }

    }

    void Output(int cell, MArr<s8>& playersTarget) {
        playersTarget.resize(gc_.M);

        {
            ostream& os = cout;
            const pint& p = gs.ToPos(cell);
            const int row = p.y;
            const int col = p.x;
            os << row << " " << col << endl;
        }

        istream& is = cin;

        REP(m, gc_.M) {
            int row, col;
            is >> row >> col;
            playersTarget[m] = (s8)gs.ToId(col, row);
        }

        REP(m, gc_.M) {
            int row, col;
            is >> row >> col;
            game_.poss_[m] = (s8)gs.ToId(col, row);
        }

        REP(i, NN) {
            int owner;
            is >> owner;
            game_.owners_[i] = (s8)owner;
        }

        REP(i, NN) {
            int level;
            is >> level;
            game_.levels_[i] = (s8)level;
        }
        ++game_.turn_;
    }

    void Finalize() {
    }

};
IOServer server;

template <int CAP>
struct LowLinkS {
    CapArr<bool, CAP> jointMap_;  
    CapArr<int, CAP> joints_;     

    template <class AROUND_MAP>
    void Init(int size, const AROUND_MAP& aroundMap, const CapArr<bool, CAP>& movable, int start, bool extraStart) {
        CapArr<int, CAP> ord_;
        CapArr<int, CAP> low_;
        ord_.assign(size, -1);
        low_.assign(size, -1);
        jointMap_.assign(size, false);
        joints_.clear();

        int num = 0;
        auto dfs = [&](auto&& dfs, int i, int parent) -> void {
            ord_[i] = num++;
            low_[i] = ord_[i];

            int childCount = 0;
            bool isJoint = false;

            for (int next : aroundMap[i]) {
                if (!movable[next]) {
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
                joints_.push(i);
                jointMap_[i] = true;
            }
        };
        if (movable[start]) {
            dfs(dfs, start, -1);

            if (extraStart) {
                if (!jointMap_[start]) {
                    for (int n : aroundMap[start]) {
                        if (movable[n]) {
                            joints_.push(start);
                            jointMap_[start] = true;
                            break;
                        }
                    }
                }
            }
        }
    }
};


template <int W, int H, int AROUND_COUNT>
struct AroundMapS {
    using CA = CCA<int, AROUND_COUNT>;
    CA table[W * H];

    constexpr AroundMapS(const array<pint, AROUND_COUNT>& aroundDirs) : table{} {
        REP(cellId, W * H) {
            pint p = {cellId % W, cellId / W};
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                if (x >= 0 && x < W && y >= 0 && y < H) {
                    table[cellId].push(x + y * W);
                }
            }
        }
    }

    inline constexpr const CA& operator()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator[](int id) const {
        return table[id];
    }
};

template <int W, int H, int AROUND_COUNT>
struct DirMapS {
    using CA = CCA<int, AROUND_COUNT>;
    CA table[W * H];

    constexpr DirMapS(const array<pint, AROUND_COUNT>& aroundDirs) : table{} {
        REP(cellId, W * H) {
            pint p = {cellId % W, cellId / W};
            for (const pint& a : aroundDirs) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                int n = -1;
                if (x >= 0 && x < W && y >= 0 && y < H) {
                    n = x + y * W;
                }
                table[cellId].push(n);
            }
        }
    }

    inline constexpr const CA& operator()(int id) const {
        return table[id];
    }
    inline constexpr const CA& operator[](int id) const {
        return table[id];
    }
};

template <int W, int H>
struct RandAroundMapS {
    using CA = CCA<int, 4>;
    CapArr<CA, 24> table[W * H];

    constexpr RandAroundMapS() : table{} {
        REP(cellId, W * H) {
            pint p = {cellId % W, cellId / W};
            CA tmp{};
            for (const pint& a : Around4) {
                int x = p.a + a.a;
                int y = p.b + a.b;
                if (x >= 0 && x < W && y >= 0 && y < H) {
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
    inline constexpr const CA& operator()(int p, RAND& r) const {
        auto& tbl = table[p];
        return tbl[r(tbl.size())];
    }
};




struct JointChecker {
    array<pint, 8> offsets_;    
    array<bool, 256> isJoint_;  



    void Init() {
        offsets_ = {pint{-1, -1}, pint{0, -1}, pint{1, -1}, pint{-1, 0},
                    pint{1, 0},   pint{-1, 1}, pint{0, 1},  pint{1, 1}};

        isJoint_.fill(false);

        constexpr AroundMapS<3, 3, 4> aroundMap(Around4);

        CapArr<bool, 9> grid;
        REP(bit, 1 << 8) {
            grid.assign(9, false);
            grid[4] = true;
            REP(p, 8) {
                cauto& offset = offsets_[p];
                int cell = (offset.x + 1) + (offset.y + 1) * 3;
                if ((bit >> p) & 1) {
                    grid[cell] = true;
                }
            }

            if (grid[4]) {
                LowLinkS<9> lowLink;
                lowLink.Init(9, aroundMap, grid, 4, false);

                if (lowLink.jointMap_[4]) {
                    isJoint_[bit] = true;
                }
                else {
                    int count = 0;
                    for (int n : aroundMap[4]) {
                        if (grid[n]) {
                            ++count;
                        }
                    }
                }

            }
        }
    }
};


struct State {
    NNBox<s8> owners_;
    NNBox<s8> levels_;
    MArr<s8> poss_;
    MArr<int> scores_;

    MArr<bitset<NN>> reachables_;       
    MArr<BitArr<NN>> excludeMovables_;  
    MArr<NNSet> movableSets_;           

    void Init(const GameConst& gc, const Game& game) {
        owners_ = game.owners_;
        levels_ = game.levels_;
        poss_ = game.poss_;
        CalcPlayersScore2(gc, owners_, levels_, scores_);

        reachables_.resize(gc.M);
        excludeMovables_.resize(gc.M);
        movableSets_.resize(gc.M);
        REP(m, gc.M) {
            CalcReachableMovable(gc, owners_, poss_, m, reachables_[m], movableSets_[m], excludeMovables_[m]);
        }
    }
};
constexpr int StateSize = sizeof(State);


void CalcPlayersArea(const GameConst& gc, const State& state, MArr<int>& areas) {
    areas.assign(gc.M, 0);
    REP(i, NN) {
        int owner = state.owners_[i];
        if (owner >= 0) {
            areas[owner]++;
        }
    }
}

void CalcPlayerArea(const State& state, int player, NNArr<u8>& cells) {
    CalcPlayerArea(state.owners_, state.poss_, player, cells);
}


void CalcPlayerDistFreeOnly(const State& state, int player, NNBox<u8>& dists) {
    NNQue<s8> que;
    dists.fill(InfDist);
    REP(i, NN) {
        if (state.owners_[i] == player) {
            que.push(i);
            dists[i] = 0;
        }
    }
    while (!que.empty()) {
        int p = que.pop();
        for (int n : gs.around(p)) {
            if (state.owners_[n] >= 0) {
                continue;
            }
            auto d = dists[p] + 1;
            if (d < dists[n]) {
                que.push(n);
                dists[n] = d;
            }
        }
    }
}

void CalcGetTurnMap(const State& state, int player, NNBox<u8>& costs) {

    struct Node {
        s8 p;
        u8 cost;
        bool operator<(const Node& n) const {
            return cost > n.cost;
        }
    };

    static thread_local PriorityQueue<Node> que;
    costs.fill(InfDist);
    que.Clear();

    for (int start : state.movableSets_[player]) {
        if (state.owners_[start] == player) {
            costs[start] = 0;
            que.push(Node{(s8)start, 0});
        }
        else {
            costs[start] = 1;
            que.push(Node{(s8)start, 1});
        }
    }

    while (!que.empty()) {
        Node node = que.top();
        que.pop();

        if (costs[node.p] != node.cost) {
            continue;
        }

        for (int n : gs.around(node.p)) {
            u8 cost = node.cost;
            if (state.owners_[n] == player) {
            }
            else if (state.owners_[n] > 0) {
                cost += state.levels_[n];
            }
            else {
                cost += 1;
            }

            if (cost < costs[n]) {
                costs[n] = cost;
                que.push(Node{(s8)n, cost});
            }
        }
    }
}


enum class MoveType : u8 {
    Neutral,       
    Grow,          
    FinishAttack,  
    ChipAttack,    
};

void CalcAIGreedyMove(const GameConst& gc, const State& state, int player, const NNArr<u8>& movableCells,
                      array<NNArr<s8>, 4>& moveTypesCands) {
    array<int, 4> moveTypesValue;
    REP(i, 4) {
        moveTypesValue[i] = -1;
        moveTypesCands[i].clear();
    }

    auto Add = [&](MoveType type, int cell) {
        int typeId = (int)type;
        int value = gc.Vs[cell];
        if (value > moveTypesValue[typeId]) {
            moveTypesValue[typeId] = value;
            moveTypesCands[typeId].clear();
            moveTypesCands[typeId].push(cell);
        }
        else if (value == moveTypesValue[typeId]) {
            moveTypesCands[typeId].push(cell);
        }
    };

    for (int cell : movableCells) {
        if (state.owners_[cell] < 0) {
            Add(MoveType::Neutral, cell);
        }
        else if (state.owners_[cell] == player) {
            if (state.levels_[cell] < gc.U) {
                Add(MoveType::Grow, cell);
            }
            else {
            }
        }
        else {
            if (state.levels_[cell] == 1) {
                Add(MoveType::FinishAttack, cell);
            }
            else {
                Add(MoveType::ChipAttack, cell);
            }
        }
    }
}


double EvalAICellScore(const GameConst& gc, const State& state, int player, int cell, const AIParam& prm) {
    return EvalAICellScore(gc, state.owners_, state.levels_, player, cell, prm);
}

void CalcAIGreedyMove(const GameConst& gc, const State& state, int player, const AIParam& prm, NNArr<u8>& greedyCells) {
    greedyCells.clear();

    cauto& movableCells = state.movableSets_[player];

    NNArr<double> scores;
    scores.resize(movableCells.size());
    double maxScore = -1e100;
    REP(i, movableCells.size()) {
        const int cell = movableCells[i];
        double score = EvalAICellScore(gc, state, player, cell, prm);
        scores[i] = score;
        maxScore = max(maxScore, score);
    }

    const double tol = 1e-9 * max(1.0, fabs(maxScore));
    int bestCount = 0;
    REP(i, movableCells.size()) {
        if (scores[i] >= maxScore - tol) {
            bestCount++;
        }
    }
    if (bestCount <= 0) {
        return;
    }

    REP(i, movableCells.size()) {
        const int cell = movableCells[i];
        if (scores[i] >= maxScore - tol) {
            greedyCells.push(cell);
        }
    }
}

void CalcAIMoveProb(const GameConst& gc, const State& state, int player, const AIParam& prm, NNBox<double>& outProb) {
    CalcAIMoveProb(gc, state.owners_, state.levels_, player, state.movableSets_[player].RefArray(), prm, outProb);
}

int SampleByProbOnMovable(const State& state, int player, const NNBox<double>& probs, Xor64& rand) {
    return SampleByProbOnMovable(state.movableSets_[player].RefArray(), probs, rand);
}

int SampleByEstimatedModel(const GameConst& gc, const State& state, int player, const AIParam& prm, Xor64& rand) {
    return SampleByEstimatedModel(gc, state.owners_, state.levels_, player, state.movableSets_[player].RefArray(), prm,
                                  rand);
}

void ApplyMovesOneTurnImpl(const GameConst& gc, State& next, const MArr<s8>& moves) {
    REP(m, gc.M) {
        for (auto cell : next.excludeMovables_[m]) {
            next.movableSets_[m].fadd(cell);
        }
    }

    array<u8, M_max> conflicted{};
    REP(p, gc.M) {
        FOR(other, p + 1, gc.M) {
            if (moves[p] == moves[other]) {
                conflicted[p] = 1;
                conflicted[other] = 1;
            }
        }
    }

    array<u8, M_max> collected{};

    REP(p, gc.M) {
        if (!conflicted[p]) {
            continue;
        }
        const int cell = (int)moves[p];
        const int owner = (int)next.owners_[cell];
        if (owner != p) {
            collected[p] = 1;
        }
    }

    array<bool, M_max> gains{};
    array<bool, M_max> losts{};
    array<u8, M_max> lostCounts{};
    array<s8, M_max> lostCells{};
    lostCells.fill(-1);

    REP(p, gc.M) {
        if (collected[p]) continue;

        const int cell = moves[p];
        const int owner = (int)next.owners_[cell];

        if (owner < 0) {
            next.owners_[cell] = (s8)p;
            next.levels_[cell] = 1;
            next.scores_[p] += gc.Vs[cell];
            gains[p] = 1;
        }
        else if (owner == p) {
            if (next.levels_[cell] < gc.U) {
                next.levels_[cell]++;
                next.scores_[p] += gc.Vs[cell];
            }
        }
        else {
            next.levels_[cell]--;
            if (next.levels_[cell] <= 0) {
                next.scores_[owner] -= gc.Vs[cell];
                next.owners_[cell] = (s8)p;
                next.levels_[cell] = 1;
                next.scores_[p] += gc.Vs[cell];
                gains[p] = 1;
                losts[owner] = 1;
                lostCounts[owner]++;
                if (lostCounts[owner] == 1) {
                    lostCells[owner] = (s8)cell;
                }
            }
            else {
                next.scores_[owner] -= gc.Vs[cell];
                collected[p] = 1;
            }
        }
    }

    REP(p, gc.M) {
        if (!collected[p]) {
            next.poss_[p] = (s8)moves[p];
        }
    }

    static JointChecker jc = [] {
        JointChecker jc;
        jc.Init();
        return jc;
    }();
    auto CanSkipRecalcAfterSingleLoss = [&](int player, int lostCell) -> bool {
        if (lostCell < 0 || lostCell >= NN) {
            return false;
        }
        if (lostCell == (int)next.poss_[player]) {
            return false;
        }
        if (!next.reachables_[player].test(lostCell)) {
            return true;
        }

        const int cx = lostCell % N;
        const int cy = lostCell / N;
        int bit = 0;
        REP(i, 8) {
            const int nx = cx + jc.offsets_[i].x;
            const int ny = cy + jc.offsets_[i].y;
            if ((unsigned)nx >= (unsigned)N || (unsigned)ny >= (unsigned)N) {
                continue;
            }
            const int n = nx + ny * N;
            if (next.reachables_[player].test(n)) {
                bit |= (1 << i);
            }
        }
        return !jc.isJoint_[bit];
    };
    auto UpdateLossLocally = [&](int player, int lostCell) {
        if (next.reachables_[player].test(lostCell)) {
            next.reachables_[player].reset(lostCell);
        }

        array<int, 5> affectedCells{};
        int affectedCount = 0;
        affectedCells[affectedCount++] = lostCell;
        for (int n : gs.around(lostCell)) {
            affectedCells[affectedCount++] = n;
        }

        REP(i, affectedCount) {
            const int cell = affectedCells[i];
            bool shouldContain = false;
            if (next.reachables_[player].test(cell)) {
                shouldContain = true;
            }
            else {
                for (int n : gs.around(cell)) {
                    if (next.reachables_[player].test(n)) {
                        shouldContain = true;
                        break;
                    }
                }
            }

            if (shouldContain) {
                next.movableSets_[player].fadd(cell);
            }
            else {
                next.movableSets_[player].fremove(cell);
            }
        }
    };

    REP(p, gc.M) {
        if (losts[p]) {
            bool usedLocalUpdate = false;
            if (lostCounts[p] == 1 && !gains[p]) {
                const int lostCell = (int)lostCells[p];
                if (CanSkipRecalcAfterSingleLoss(p, lostCell)) {
                    UpdateLossLocally(p, lostCell);
                    usedLocalUpdate = true;
                }
            }

            if (!usedLocalUpdate) {
                CalcReachableMovable(gc, next.owners_, next.poss_, p, next.reachables_[p], next.movableSets_[p],
                                     next.excludeMovables_[p]);
                continue;
            }
        }

        if (gains[p]) {
            const int newPos = (int)next.poss_[p];

            bool needRecalc = false;
            for (int n : gs.around(newPos)) {
                if (next.owners_[n] == p && !next.movableSets_[p].contains(n)) {
                    needRecalc = true;
                    break;
                }
            }
            if (needRecalc) {
                CalcReachableMovable(gc, next.owners_, next.poss_, p, next.reachables_[p], next.movableSets_[p],
                                     next.excludeMovables_[p]);
                continue;
            }

            next.reachables_[p].set(newPos);
            for (int n : gs.around(newPos)) {
                next.movableSets_[p].fadd(n);
            }
        }

        next.excludeMovables_[p].clear();
        REP(other, gc.M) {
            if (other == p) {
                continue;
            }
            if (next.movableSets_[p].contains(next.poss_[other])) {
                next.movableSets_[p].remove(next.poss_[other]);
                next.excludeMovables_[p].set(next.poss_[other]);
            }
        }
    }

}

void ApplyMovesOneTurn(const GameConst& gc, const State& state, const MArr<s8>& moves, State& next) {
    next = state;
    ApplyMovesOneTurnImpl(gc, next, moves);
}

void ApplyMovesOneTurn(const GameConst& gc, State& state, const MArr<s8>& moves) {
    ApplyMovesOneTurnImpl(gc, state, moves);
}



struct SingleAIParamEstimator {
    enum class ResampleMoveMode : u8 {
        None = 0,    
        Jitter = 1,  
        MH = 2,      
    };

    struct Config {
        int particleCount = 384;
        double resampleEssRate = 0.55;
        double minLikelihood = 1e-15;
        ResampleMoveMode resampleMoveMode = ResampleMoveMode::Jitter;
        double jitterRatio = 0.05;
        double jitterEps = 0.02;
        int mcmcMoveIters = 2;
        int mcmcHistoryWindow = T;
        double mcmcStepRatio = 0.05;
        double mcmcStepEps = 0.02;
    };

    struct Particle {
        double wbOverWa = 1.0;
        double wcOverWa = 1.0;
        double wdOverWa = 1.0;
        double eps = 0.3;
        double w = 0.0;
    };

    struct ObservationMeta {
        int candidateCount = 0;
        int observedClass = -1;  
        int observedValue = -1;
        bool hasPositiveClass = false;  
        array<int, 4> classTopValue{};
        array<u8, 4> classExists{};
        array<array<int, NN>, 4> classValuesDesc{};  
        array<int, 4> classValueCount{};             
    };

    struct Observation {
        int observedCell = -1;
        int observedIdx = -1;
        int stayCell = -1;  
        ObservationMeta meta{};
    };

    static constexpr double kRatioMin = 0.3 / 1.0;
    static constexpr double kRatioMax = 1.0 / 0.3;
    static constexpr double kEpsMin = 0.1;
    static constexpr double kEpsMax = 0.5;
    static constexpr double kJudgeTieTolerance = 1e-9;

    int player_ = -1;
    Config cfg_;
    vector<Particle> particles_;
    vector<Particle> resampleBuffer_;
    vector<Observation> observations_;
    mutable vector<double> workScores_;
    mutable vector<int> workBestIdx_;
    mutable vector<int> workCandidates_;

    SingleAIParamEstimator() = default;

    SingleAIParamEstimator(int player, Xor64& rand) {
        Init(player, rand);
    }

    void Init(int player, Xor64& rand) {
        player_ = player;
        cfg_ = BuildConfigFromHyperParam();
        if (cfg_.particleCount < 8) {
            cfg_.particleCount = 8;
        }
        if (cfg_.mcmcMoveIters < 0) {
            cfg_.mcmcMoveIters = 0;
        }
        if (cfg_.mcmcHistoryWindow <= 0) {
            cfg_.mcmcHistoryWindow = T;
        }
        particles_.resize(cfg_.particleCount);
        resampleBuffer_.resize(cfg_.particleCount);
        Reset(rand);
    }

    int Player() const {
        return player_;
    }

    void Reset(Xor64& rand) {
        if (particles_.empty()) {
            return;
        }
        observations_.clear();
        observations_.reserve(T);
        workScores_.clear();
        workBestIdx_.clear();
        workCandidates_.clear();
        workScores_.reserve(NN);
        workBestIdx_.reserve(NN);
        workCandidates_.reserve(NN);

        const int n = (int)particles_.size();
        const double uw = 1.0 / (double)n;
        for (auto& p : particles_) {
            const double wa = rand.GetDoubleRange(0.3, 1.0);
            const double wb = rand.GetDoubleRange(0.3, 1.0);
            const double wc = rand.GetDoubleRange(0.3, 1.0);
            const double wd = rand.GetDoubleRange(0.3, 1.0);

            p.wbOverWa = clamp(wb / wa, kRatioMin, kRatioMax);
            p.wcOverWa = clamp(wc / wa, kRatioMin, kRatioMax);
            p.wdOverWa = clamp(wd / wa, kRatioMin, kRatioMax);
            p.eps = rand.GetDoubleRange(kEpsMin, kEpsMax);
            p.w = uw;
        }
    }

    double Update(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                  int observedCell, Xor64& rand) {
        auto& candidates = workCandidates_;
        candidates.clear();
        BuildCandidates(gc, owners, poss, candidates);
        const int observedIdx = FindCandidateIndex(candidates, observedCell);
        if (candidates.empty()) {
            ObservationMeta meta;
            BuildObservationMeta(gc, owners, levels, candidates, observedIdx, meta);
            AppendObservation(observedCell, observedIdx, (int)poss[player_], std::move(meta));
            return ((int)poss[player_] == observedCell) ? 1.0 : cfg_.minLikelihood;
        }

        ObservationMeta meta;
        BuildObservationMeta(gc, owners, levels, candidates, observedIdx, meta);

        double predictive = 0.0;
        double sumW = 0.0;
        for (auto& pt : particles_) {
            const double prob = CalcMoveProbGivenMeta(meta, observedIdx, pt);
            predictive += pt.w * prob;
            pt.w *= max(prob, cfg_.minLikelihood);
            sumW += pt.w;
        }

        if (!(sumW > 0.0) || !isfinite(sumW)) {
            const double uw = 1.0 / (double)particles_.size();
            for (auto& pt : particles_) {
                pt.w = uw;
            }
        }
        else {
            for (auto& pt : particles_) {
                pt.w /= sumW;
            }
        }

        AppendObservation(observedCell, observedIdx, (int)poss[player_], std::move(meta));

        const double ess = CalcESS();
        if (ess < cfg_.resampleEssRate * (double)particles_.size()) {
            ResampleAndMove(gc, rand);
        }

        return predictive;
    }

    void CalcCellProbs(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                       NNBox<double>& outProb) const {
        outProb.fill(0.0);

        auto& candidates = workCandidates_;
        candidates.clear();
        BuildCandidates(gc, owners, poss, candidates);
        if (candidates.empty()) {
            int stay = (int)poss[player_];
            if (0 <= stay && stay < NN) {
                outProb[stay] = 1.0;
            }
            return;
        }

        auto& scores = workScores_;
        auto& bestIdx = workBestIdx_;
        bestIdx.reserve(candidates.size());
        const int candCount = (int)candidates.size();
        double uniformMass = 0.0;

        for (const auto& pt : particles_) {
            CalcScoresAndBest(gc, owners, levels, candidates, pt, scores, bestIdx);
            const int bestCount = (int)bestIdx.size();
            if (bestCount <= 0) {
                continue;
            }

            const double eps = clamp(pt.eps, kEpsMin, kEpsMax);
            uniformMass += pt.w * (eps / (double)candCount);
            const double greedyPart = pt.w * ((1.0 - eps) / (double)bestCount);
            for (int idx : bestIdx) {
                const int cell = candidates[idx];
                outProb[cell] += greedyPart;
            }
        }
        for (int cell : candidates) {
            outProb[cell] += uniformMass;
        }

        double sum = 0.0;
        for (int cell : candidates) {
            sum += outProb[cell];
        }
        if (sum > 0.0) {
            for (int cell : candidates) {
                outProb[cell] /= sum;
            }
        }
    }

    AIParam MeanParam() const {
        AIParam out{};
        out.wa = 1.0;
        out.wb = 1.0;
        out.wc = 1.0;
        out.wd = 1.0;
        out.eps = 0.3;
        if (particles_.empty()) {
            return out;
        }

        double wb = 0.0;
        double wc = 0.0;
        double wd = 0.0;
        double eps = 0.0;
        for (const auto& pt : particles_) {
            wb += pt.w * pt.wbOverWa;
            wc += pt.w * pt.wcOverWa;
            wd += pt.w * pt.wdOverWa;
            eps += pt.w * pt.eps;
        }
        out.wb = wb;
        out.wc = wc;
        out.wd = wd;
        out.eps = eps;
        return out;
    }

    AIParam MAPParam() const {
        AIParam out{};
        out.wa = 1.0;
        out.wb = 1.0;
        out.wc = 1.0;
        out.wd = 1.0;
        out.eps = 0.3;
        if (particles_.empty()) {
            return out;
        }

        int best = 0;
        for (int i = 1; i < (int)particles_.size(); ++i) {
            if (particles_[i].w > particles_[best].w) {
                best = i;
            }
        }
        out.wb = particles_[best].wbOverWa;
        out.wc = particles_[best].wcOverWa;
        out.wd = particles_[best].wdOverWa;
        out.eps = particles_[best].eps;
        return out;
    }

    AIParam SampleParamFromPosterior(Xor64& rand) const {
        AIParam out{};
        out.wa = 1.0;
        out.wb = 1.0;
        out.wc = 1.0;
        out.wd = 1.0;
        out.eps = 0.3;
        if (particles_.empty()) {
            return out;
        }

        double sumW = 0.0;
        for (const auto& pt : particles_) {
            if (pt.w > 0.0 && isfinite(pt.w)) {
                sumW += pt.w;
            }
        }
        if (!(sumW > 0.0) || !isfinite(sumW)) {
            const int idx = rand((int)particles_.size());
            out.wb = particles_[idx].wbOverWa;
            out.wc = particles_[idx].wcOverWa;
            out.wd = particles_[idx].wdOverWa;
            out.eps = particles_[idx].eps;
            return out;
        }

        const double r = rand.GetDouble() * sumW;
        double acc = 0.0;
        int picked = (int)particles_.size() - 1;
        for (int i = 0; i < (int)particles_.size(); ++i) {
            const double w = (particles_[i].w > 0.0 && isfinite(particles_[i].w)) ? particles_[i].w : 0.0;
            acc += w;
            if (r <= acc) {
                picked = i;
                break;
            }
        }
        out.wb = particles_[picked].wbOverWa;
        out.wc = particles_[picked].wcOverWa;
        out.wd = particles_[picked].wdOverWa;
        out.eps = particles_[picked].eps;
        return out;
    }

    double CalcESS() const {
        double inv = 0.0;
        for (const auto& pt : particles_) {
            inv += pt.w * pt.w;
        }
        if (!(inv > 0.0)) {
            return 0.0;
        }
        return 1.0 / inv;
    }

   private:
    static ResampleMoveMode ParseResampleMoveMode(int mode) {
        if (mode == 0) {
            return ResampleMoveMode::None;
        }
        if (mode == 2) {
            return ResampleMoveMode::MH;
        }
        return ResampleMoveMode::Jitter;
    }

    static Config BuildConfigFromHyperParam() {
        Config cfg;
        cfg.particleCount = (int)HP.SingleAIParamEstimator_ParticleCount;
        cfg.resampleEssRate = (double)HP.SingleAIParamEstimator_ResampleEssRate;
        cfg.minLikelihood = (double)HP.SingleAIParamEstimator_MinLikelihood;
        cfg.resampleMoveMode = ParseResampleMoveMode((int)HP.SingleAIParamEstimator_ResampleMoveMode);
        cfg.jitterRatio = (double)HP.SingleAIParamEstimator_JitterRatio;
        cfg.jitterEps = (double)HP.SingleAIParamEstimator_JitterEps;
        cfg.mcmcMoveIters = (int)HP.SingleAIParamEstimator_McmcMoveIters;
        cfg.mcmcHistoryWindow = (int)HP.SingleAIParamEstimator_McmcHistoryWindow;
        cfg.mcmcStepRatio = (double)HP.SingleAIParamEstimator_McmcStepRatio;
        cfg.mcmcStepEps = (double)HP.SingleAIParamEstimator_McmcStepEps;
        return cfg;
    }

    void BuildCandidates(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, vector<int>& out) const {
        out.clear();

        array<u8, NN> occupied{};
        REP(p, gc.M) {
            if (p == player_) {
                continue;
            }
            const int pos = (int)poss[p];
            if (0 <= pos && pos < NN) {
                occupied[pos] = 1;
            }
        }

        array<u8, NN> visited{};
        array<int, NN> que{};
        int head = 0;
        int tail = 0;

        const int start = (int)poss[player_];
        if (start < 0 || start >= NN) {
            return;
        }
        que[tail++] = start;
        visited[start] = 1;

        while (head < tail) {
            const int cell = que[head++];

            if (!occupied[cell]) {
                out.push_back(cell);
            }

            if ((int)owners[cell] == player_) {
                for (int nc : gs.around(cell)) {
                    if (!visited[nc]) {
                        visited[nc] = 1;
                        que[tail++] = nc;
                    }
                }
            }
        }
    }

    int FindCandidateIndex(const vector<int>& candidates, int observedCell) const {
        for (int i = 0; i < (int)candidates.size(); ++i) {
            if (candidates[i] == observedCell) {
                return i;
            }
        }
        return -1;
    }

    int ClassifyCell(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, int cell) const {
        const int owner = (int)owners[cell];
        if (owner == -1) {
            return 0;
        }
        if (owner == player_) {
            return ((int)levels[cell] < gc.U) ? 1 : 4;
        }
        return ((int)levels[cell] == 1) ? 2 : 3;
    }

    void BuildObservationMeta(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                              const vector<int>& candidates, int observedIdx, ObservationMeta& out) const {
        out.candidateCount = (int)candidates.size();
        out.observedClass = -1;
        out.observedValue = -1;
        out.hasPositiveClass = false;
        out.classTopValue.fill(0);
        out.classExists.fill(0);
        out.classValueCount.fill(0);

        for (int i = 0; i < (int)candidates.size(); ++i) {
            const int cell = candidates[i];
            const int cls = ClassifyCell(gc, owners, levels, cell);
            const int value = (int)gc.Vs[cell];

            if (cls != 4) {
                out.hasPositiveClass = true;
                const int idx = out.classValueCount[cls];
                if (idx < NN) {
                    out.classValuesDesc[cls][idx] = value;
                    out.classValueCount[cls] = idx + 1;
                }
            }

            if (i == observedIdx) {
                out.observedClass = cls;
                out.observedValue = value;
            }
        }

        REP(cls, 4) {
            const int n = out.classValueCount[cls];
            if (n <= 0) {
                continue;
            }
            auto b = out.classValuesDesc[cls].begin();
            sort(b, b + n, greater<int>());
            out.classExists[cls] = 1;
            out.classTopValue[cls] = out.classValuesDesc[cls][0];
        }

        if (observedIdx < 0 || out.candidateCount <= 0) {
            return;
        }
    }

    void AppendObservation(int observedCell, int observedIdx, int stayCell, ObservationMeta&& meta) {
        Observation obs;
        obs.observedCell = observedCell;
        obs.observedIdx = observedIdx;
        obs.stayCell = stayCell;
        obs.meta = std::move(meta);
        observations_.push_back(std::move(obs));
    }

    double CalcCellScore(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, int cell,
                         const Particle& pt) const {
        const int owner = (int)owners[cell];
        const int level = (int)levels[cell];
        const double value = (double)gc.Vs[cell];

        if (owner == -1) {
            return value * 1.0;
        }
        if (owner == player_) {
            if (level < gc.U) {
                return value * pt.wbOverWa;
            }
            return 0.0;
        }
        if (level == 1) {
            return value * pt.wcOverWa;
        }
        return value * pt.wdOverWa;
    }

    void CalcScoresAndBest(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                           const vector<int>& candidates, const Particle& pt, vector<double>& scores,
                           vector<int>& bestIdx) const {
        const int n = (int)candidates.size();
        scores.resize(n);
        bestIdx.clear();

        double maxScore = -1e100;
        for (int i = 0; i < n; ++i) {
            const double sc = CalcCellScore(gc, owners, levels, candidates[i], pt);
            scores[i] = sc;
            if (sc > maxScore) {
                maxScore = sc;
            }
        }

        const double tol = kJudgeTieTolerance * max(1.0, fabs(maxScore));
        for (int i = 0; i < n; ++i) {
            if (scores[i] >= maxScore - tol) {
                bestIdx.push_back(i);
            }
        }
    }

    double CalcMoveProbGivenParticle(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                                     const vector<int>& candidates, int observedIdx, const Particle& pt,
                                     vector<double>& scores, vector<int>& bestIdx) const {
        if (observedIdx < 0) {
            return cfg_.minLikelihood;
        }

        CalcScoresAndBest(gc, owners, levels, candidates, pt, scores, bestIdx);

        bool obsIsBest = false;
        for (int idx : bestIdx) {
            if (idx == observedIdx) {
                obsIsBest = true;
                break;
            }
        }

        const double eps = clamp(pt.eps, kEpsMin, kEpsMax);
        const double randomPart = eps / (double)candidates.size();
        const double greedyPart = obsIsBest ? ((1.0 - eps) / (double)bestIdx.size()) : 0.0;
        return max(randomPart + greedyPart, cfg_.minLikelihood);
    }

    int CountClassBestByThreshold(const ObservationMeta& meta, int cls, double coef, double thresholdScore) const {
        const int n = meta.classValueCount[cls];
        if (n <= 0) {
            return 0;
        }
        const double valueThreshold = thresholdScore / coef;
        int count = 0;
        REP(i, n) {
            if ((double)meta.classValuesDesc[cls][i] >= valueThreshold) {
                count++;
            }
            else {
                break;
            }
        }
        return count;
    }

    double CalcMoveProbGivenMeta(const ObservationMeta& meta, int observedIdx, const Particle& pt) const {
        if (observedIdx < 0 || meta.candidateCount <= 0) {
            return cfg_.minLikelihood;
        }

        const double eps = clamp(pt.eps, kEpsMin, kEpsMax);
        const double randomPart = eps / (double)meta.candidateCount;

        if (!meta.hasPositiveClass) {
            const double greedyPart = (1.0 - eps) / (double)meta.candidateCount;
            return max(randomPart + greedyPart, cfg_.minLikelihood);
        }
        if (meta.observedClass < 0 || meta.observedClass >= 4) {
            return max(randomPart, cfg_.minLikelihood);
        }

        array<double, 4> classScore{};
        double maxScore = -1e100;
        REP(c, 4) {
            if (!meta.classExists[c]) {
                classScore[c] = -1e100;
                continue;
            }
            const double coef = (c == 0) ? 1.0 : ((c == 1) ? pt.wbOverWa : ((c == 2) ? pt.wcOverWa : pt.wdOverWa));
            classScore[c] = (double)meta.classTopValue[c] * coef;
            if (classScore[c] > maxScore) {
                maxScore = classScore[c];
            }
        }

        const double tol = kJudgeTieTolerance * max(1.0, fabs(maxScore));
        const double thresholdScore = maxScore - tol;
        const double obsCoef =
            (meta.observedClass == 0)
                ? 1.0
                : ((meta.observedClass == 1) ? pt.wbOverWa : ((meta.observedClass == 2) ? pt.wcOverWa : pt.wdOverWa));
        const double obsScore = (double)meta.observedValue * obsCoef;
        if (obsScore < thresholdScore) {
            return max(randomPart, cfg_.minLikelihood);
        }

        int bestCount = 0;
        REP(c, 4) {
            if (!meta.classExists[c]) {
                continue;
            }
            if (classScore[c] >= thresholdScore) {
                const double coef = (c == 0) ? 1.0 : ((c == 1) ? pt.wbOverWa : ((c == 2) ? pt.wcOverWa : pt.wdOverWa));
                bestCount += CountClassBestByThreshold(meta, c, coef, thresholdScore);
            }
        }
        if (bestCount <= 0) {
            return max(randomPart, cfg_.minLikelihood);
        }
        const double greedyPart = (1.0 - eps) / (double)bestCount;
        return max(randomPart + greedyPart, cfg_.minLikelihood);
    }

    double CalcMoveProbGivenObservation(const Observation& obs, const Particle& pt) const {
        if (obs.meta.candidateCount <= 0) {
            return (obs.stayCell == obs.observedCell) ? 1.0 : cfg_.minLikelihood;
        }
        return CalcMoveProbGivenMeta(obs.meta, obs.observedIdx, pt);
    }

    double CalcLogPosteriorFromHistory(const Particle& pt) const {
        if (observations_.empty()) {
            return 0.0;
        }
        const int histSize = (int)observations_.size();
        const int begin = max(0, histSize - cfg_.mcmcHistoryWindow);
        double logPost = 0.0;
        for (int i = begin; i < histSize; ++i) {
            const double prob = CalcMoveProbGivenObservation(observations_[i], pt);
            logPost += log(max(prob, cfg_.minLikelihood));
        }
        return logPost;
    }

    double ProposeReflect(double cur, double step, double lo, double hi, Xor64& rand) {
        if (!(step > 0.0)) {
            return clamp(cur, lo, hi);
        }
        double x = cur + rand.GetDoubleRange(-step, step);
        while (x < lo || x > hi) {
            if (x < lo) {
                x = lo + (lo - x);
            }
            if (x > hi) {
                x = hi - (x - hi);
            }
        }
        return x;
    }

    void JitterParticle(Particle& pt, Xor64& rand) {
        pt.wbOverWa =
            clamp(pt.wbOverWa + rand.GetDoubleRange(-cfg_.jitterRatio, cfg_.jitterRatio), kRatioMin, kRatioMax);
        pt.wcOverWa =
            clamp(pt.wcOverWa + rand.GetDoubleRange(-cfg_.jitterRatio, cfg_.jitterRatio), kRatioMin, kRatioMax);
        pt.wdOverWa =
            clamp(pt.wdOverWa + rand.GetDoubleRange(-cfg_.jitterRatio, cfg_.jitterRatio), kRatioMin, kRatioMax);
        pt.eps = clamp(pt.eps + rand.GetDoubleRange(-cfg_.jitterEps, cfg_.jitterEps), kEpsMin, kEpsMax);
    }

    int CalcAdaptiveMcmcMoveIters() const {
        return cfg_.mcmcMoveIters;
    }

    void MoveParticleByMH(const GameConst& gc, Particle& pt, int moveIters, Xor64& rand) {
        if (moveIters <= 0 || observations_.empty()) {
            return;
        }
        (void)gc;
        double currentLogPost = CalcLogPosteriorFromHistory(pt);
        REP(iter, moveIters) {
            Particle proposal = pt;
            proposal.wbOverWa = ProposeReflect(proposal.wbOverWa, cfg_.mcmcStepRatio, kRatioMin, kRatioMax, rand);
            proposal.wcOverWa = ProposeReflect(proposal.wcOverWa, cfg_.mcmcStepRatio, kRatioMin, kRatioMax, rand);
            proposal.wdOverWa = ProposeReflect(proposal.wdOverWa, cfg_.mcmcStepRatio, kRatioMin, kRatioMax, rand);
            proposal.eps = ProposeReflect(proposal.eps, cfg_.mcmcStepEps, kEpsMin, kEpsMax, rand);

            const double proposalLogPost = CalcLogPosteriorFromHistory(proposal);
            const double logAlpha = proposalLogPost - currentLogPost;
            const double u = max(rand.GetDouble(), 1e-300);
            if (logAlpha >= 0.0 || log(u) < logAlpha) {
                pt = proposal;
                currentLogPost = proposalLogPost;
            }
        }
    }

    void ResampleAndMove(const GameConst& gc, Xor64& rand) {
        if (particles_.empty()) {
            return;
        }
        const int adaptiveMcmcMoveIters = CalcAdaptiveMcmcMoveIters();
        const int n = (int)particles_.size();
        if ((int)resampleBuffer_.size() != n) {
            resampleBuffer_.resize(n);
        }
        REP(i, n) {
            resampleBuffer_[i] = particles_[i];
        }
        auto& old = resampleBuffer_;

        const double u0 = rand.GetDouble() / (double)n;
        int j = 0;
        double acc = old[0].w;

        for (int i = 0; i < n; ++i) {
            const double u = u0 + (double)i / (double)n;
            while (u > acc && j + 1 < n) {
                ++j;
                acc += old[j].w;
            }
            Particle np = old[j];

            if (cfg_.resampleMoveMode == ResampleMoveMode::Jitter) {
                JitterParticle(np, rand);
            }
            else if (cfg_.resampleMoveMode == ResampleMoveMode::MH) {
                MoveParticleByMH(gc, np, adaptiveMcmcMoveIters, rand);
            }
            np.w = 1.0 / (double)n;

            particles_[i] = np;
        }
    }
};

struct EnemyAIParamEstimatorManager {
    static SingleAIParamEstimator::ResampleMoveMode ParseResampleMoveMode(int mode) {
        if (mode == 0) {
            return SingleAIParamEstimator::ResampleMoveMode::None;
        }
        if (mode == 2) {
            return SingleAIParamEstimator::ResampleMoveMode::MH;
        }
        return SingleAIParamEstimator::ResampleMoveMode::Jitter;
    }

    static const char* ResampleMoveModeName(SingleAIParamEstimator::ResampleMoveMode mode) {
        if (mode == SingleAIParamEstimator::ResampleMoveMode::None) {
            return "none";
        }
        if (mode == SingleAIParamEstimator::ResampleMoveMode::MH) {
            return "mh";
        }
        return "jitter";
    }

    array<SingleAIParamEstimator, M_max> aiEstimators_{};  
    array<NNBox<double>, M_max> aiMoveProbs_{};            
    array<NNBox<double>, M_max> aiTrueMoveProbs_{};        
    array<double, M_max> aiPredictive_{};                  
    array<AIParam, M_max> oracleParams_{};                 
    array<bool, M_max> hasOracleParam_{};                  
    array<AIParam, M_max> aiTrueParamsNorm_{};             
    array<double, M_max> paramWL1Now_{};                   
    array<double, M_max> paramWRmseNow_{};                 
    array<double, M_max> paramEpsAbsNow_{};                
    array<double, M_max> paramMoveTVNow_{};                

    void Init(const GameConst& gc, Xor64& rand) {
        const auto resampleMode = ParseResampleMoveMode((int)HP.SingleAIParamEstimator_ResampleMoveMode);

        aiPredictive_.fill(0.0);
        paramWL1Now_.fill(0.0);
        paramWRmseNow_.fill(0.0);
        paramEpsAbsNow_.fill(0.0);
        paramMoveTVNow_.fill(0.0);
        REP(m, M_max) {
            aiMoveProbs_[m].fill(0.0);
            aiTrueMoveProbs_[m].fill(0.0);
            aiTrueParamsNorm_[m] = {};
        }
        FOR(m, 1, gc.M) {
            aiEstimators_[m].Init(m, rand);
        }
    }

    void ClearOracleParams() {
        hasOracleParam_.fill(false);
        oracleParams_.fill({});
    }

    void SetOracleParam(int player, const AIParam& prm) {
        VASSERT(0 <= player && player < M_max);
        oracleParams_[player] = prm;
        hasOracleParam_[player] = true;
    }

    void SetOracleParams(const MArr<AIParam>& params) {
        ClearOracleParams();
        const int n = min((int)params.size(), M_max);
        REP(player, n) {
            oracleParams_[player] = params[player];
            hasOracleParam_[player] = true;
        }
    }

    bool HasOracleParams() const {
        REP(player, M_max) {
            if (hasOracleParam_[player]) {
                return true;
            }
        }
        return false;
    }

    bool TryGetOracleParam(int player, AIParam& out) const {
        if (player < 0 || player >= M_max) {
            return false;
        }
        if (!hasOracleParam_[player]) {
            return false;
        }
        out = oracleParams_[player];
        return true;
    }

    void RefreshMoveProbs(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss) {
        REP(m, gc.M) {
            aiMoveProbs_[m].fill(0.0);
        }
        FOR(m, 1, gc.M) {
            aiEstimators_[m].CalcCellProbs(gc, owners, levels, poss, aiMoveProbs_[m]);
        }
        UpdateParamAccuracyMetrics(gc, owners, levels, poss);
    }

    void Update(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                const MArr<s8>& playersTarget, Xor64& rand) {
        if ((int)playersTarget.size() != gc.M) {
            return;
        }
        FOR(m, 1, gc.M) {
            aiPredictive_[m] = aiEstimators_[m].Update(gc, owners, levels, poss, playersTarget[m], rand);
        }
    }

    void GetEstimatedParam(int player, AIParam& out) const {
        out = aiEstimators_[player].MeanParam();
    }

    void GetSampledEstimatedParam(int player, AIParam& out, Xor64& rand) const {
        out = aiEstimators_[player].SampleParamFromPosterior(rand);
    }

    const NNBox<double>& MoveProb(int player) const {
        VASSERT(0 <= player && player < M_max);
        return aiMoveProbs_[player];
    }

    void PrintParamAccuracySummary(const GameConst& gc) const {
        if (!HasOracleParams()) {
            cerr << "param acc: oracle unavailable." << endl;
            return;
        }

        int totalPlayers = 0;
        double totalWL1 = 0.0;
        double totalWRmse = 0.0;
        double totalEpsAbs = 0.0;
        double totalMoveTV = 0.0;

        FOR(m, 1, gc.M) {
            if (!hasOracleParam_[m]) {
                continue;
            }


            totalPlayers += 1;
            totalWL1 += paramWL1Now_[m];
            totalWRmse += paramWRmseNow_[m];
            totalEpsAbs += paramEpsAbsNow_[m];
            totalMoveTV += paramMoveTVNow_[m];
        }

        if (totalPlayers <= 0) {
            cerr << "param acc: no samples." << endl;
            return;
        }

        cerr << "moveTV: " << (totalMoveTV / (double)totalPlayers) << endl;
    }


   private:
    void CalcMovableCells(const GameConst& gc, const NNBox<s8>& owners, const MArr<s8>& poss, int player,
                          NNArr<int>& movableCells) const {
        movableCells.clear();
        if (player <= 0 || player >= gc.M) {
            return;
        }

        array<u8, NN> occupied{};
        REP(m, gc.M) {
            const int pos = (int)poss[m];
            if (0 <= pos && pos < NN) {
                occupied[pos] = 1;
            }
        }

        NNQue<int> que;
        NNCheck check;
        const int start = (int)poss[player];
        if (start < 0 || start >= NN) {
            return;
        }
        que.push(start);
        check.Check(start);
        movableCells.push(start);

        while (!que.empty()) {
            const int cell = que.pop();
            for (int nc : gs.around(cell)) {
                if (!check.Enter(nc)) continue;
                if (!occupied[nc]) {
                    movableCells.push(nc);
                }
                if (owners[nc] == player) {
                    que.push(nc);
                }
            }
        }
    }

    AIParam NormalizeParamForDisplay(const AIParam& src) const {
        AIParam out = src;
        out.wa = max(0.0, out.wa);
        out.wb = max(0.0, out.wb);
        out.wc = max(0.0, out.wc);
        out.wd = max(0.0, out.wd);
        const double sum = out.wa + out.wb + out.wc + out.wd;
        if (sum <= 1e-12) {
            out.wa = 0.25;
            out.wb = 0.25;
            out.wc = 0.25;
            out.wd = 0.25;
        }
        else {
            out.wa /= sum;
            out.wb /= sum;
            out.wc /= sum;
            out.wd /= sum;
        }
        return out;
    }

    void UpdateParamAccuracyMetrics(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                                    const MArr<s8>& poss) {
        FOR(m, 1, gc.M) {
            if (!hasOracleParam_[m]) {
                aiTrueParamsNorm_[m] = {};
                aiTrueMoveProbs_[m].fill(0.0);
                paramWL1Now_[m] = 0.0;
                paramWRmseNow_[m] = 0.0;
                paramEpsAbsNow_[m] = 0.0;
                paramMoveTVNow_[m] = 0.0;
                continue;
            }

            AIParam trueParam{};
            if (!TryGetOracleParam(m, trueParam)) {
                aiTrueParamsNorm_[m] = {};
                aiTrueMoveProbs_[m].fill(0.0);
                paramWL1Now_[m] = 0.0;
                paramWRmseNow_[m] = 0.0;
                paramEpsAbsNow_[m] = 0.0;
                paramMoveTVNow_[m] = 0.0;
                continue;
            }

            const AIParam estParam = NormalizeParamForDisplay(aiEstimators_[m].MeanParam());
            aiTrueParamsNorm_[m] = NormalizeParamForDisplay(trueParam);
            CalcAIMoveProbByParam(gc, owners, levels, poss, m, trueParam, aiTrueMoveProbs_[m]);

            const double dwa = estParam.wa - aiTrueParamsNorm_[m].wa;
            const double dwb = estParam.wb - aiTrueParamsNorm_[m].wb;
            const double dwc = estParam.wc - aiTrueParamsNorm_[m].wc;
            const double dwd = estParam.wd - aiTrueParamsNorm_[m].wd;
            paramWL1Now_[m] = abs(dwa) + abs(dwb) + abs(dwc) + abs(dwd);
            paramWRmseNow_[m] = sqrt((dwa * dwa + dwb * dwb + dwc * dwc + dwd * dwd) / 4.0);
            paramEpsAbsNow_[m] = abs(estParam.eps - aiTrueParamsNorm_[m].eps);
            paramMoveTVNow_[m] = CalcMoveProbTV(aiMoveProbs_[m], aiTrueMoveProbs_[m]);
        }
    }

   public:
    void CalcMoveProbs(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels, const MArr<s8>& poss,
                       MArr<NNBox<double>>& enemyMoveProbs) const {
        enemyMoveProbs.resize(gc.M);
        FOR(m, 1, gc.M) {
            aiEstimators_[m].CalcCellProbs(gc, owners, levels, poss, enemyMoveProbs[m]);
        }
    }

   private:
    void CalcAIMoveProbByParam(const GameConst& gc, const NNBox<s8>& owners, const NNBox<s8>& levels,
                               const MArr<s8>& poss, int player, const AIParam& prm, NNBox<double>& outProb) const {
        outProb.fill(0.0);
        if (player <= 0 || player >= gc.M) {
            return;
        }

        NNArr<int> movableCells;
        CalcMovableCells(gc, owners, poss, player, movableCells);
        if (movableCells.empty()) {
            return;
        }

        array<double, NN> scores{};
        const int n = (int)movableCells.size();
        double maxScore = -1e100;
        REP(i, n) {
            const int cell = movableCells[i];
            const int owner = owners[cell];
            const int level = levels[cell];
            const double value = (double)gc.Vs[cell];

            double score = 0.0;
            if (owner == -1) {
                score = value * prm.wa;
            }
            else if (owner == player) {
                score = (level < gc.U) ? (value * prm.wb) : 0.0;
            }
            else {
                score = (level == 1) ? (value * prm.wc) : (value * prm.wd);
            }
            scores[i] = score;
            maxScore = max(maxScore, score);
        }

        const double tol = 1e-9 * max(1.0, fabs(maxScore));
        const double threshold = maxScore - tol;
        int bestCount = 0;
        REP(i, n) {
            if (scores[i] >= threshold) {
                bestCount++;
            }
        }
        if (bestCount <= 0) {
            return;
        }

        const double eps = clamp(prm.eps, 0.0, 1.0);
        const double randomPart = eps / (double)n;
        const double greedyPart = (1.0 - eps) / (double)bestCount;

        REP(i, n) {
            const int cell = movableCells[i];
            outProb[cell] += randomPart;
            if (scores[i] >= threshold) {
                outProb[cell] += greedyPart;
            }
        }
    }

    double CalcMoveProbTV(const NNBox<double>& estProb, const NNBox<double>& trueProb) const {
        double l1 = 0.0;
        REP(i, NN) {
            l1 += abs(estProb[i] - trueProb[i]);
        }
        return 0.5 * l1;
    }
};

double CalcAnyAIMoveProb(const GameConst& gc, const EnemyAIParamEstimatorManager& enemyAiEstimator, int cell) {
    double freeProb = 1.0;
    FOR(m, 1, gc.M) {
        double p = enemyAiEstimator.aiMoveProbs_[m][cell];
        freeProb *= (1.0 - p);
    }
    return 1.0 - freeProb;
}

double CalcAnyAIMoveProb(const GameConst& gc, const MArr<NNBox<double>>& enemyMoveProbs, int cell) {
    double freeProb = 1.0;
    FOR(m, 1, gc.M) {
        double p = enemyMoveProbs[m][cell];
        freeProb *= (1.0 - p);
    }
    return 1.0 - freeProb;
}

void BuildModelParams(const GameConst& gc, const EnemyAIParamEstimatorManager& enemyAiEstimator,
                      MArr<AIParam>& aiParams) {
    aiParams.resize(gc.M);
    FOR(m, 1, gc.M) {
        AIParam prm{};
        enemyAiEstimator.GetEstimatedParam(m, prm);
        aiParams[m] = NormalizeParam(prm);
    }
}

void BuildModelParamsSum1(const GameConst& gc, const EnemyAIParamEstimatorManager& enemyAiEstimator,
                          MArr<AIParam>& aiParams) {
    aiParams.resize(gc.M);
    FOR(m, 1, gc.M) {
        AIParam prm{};
        enemyAiEstimator.GetEstimatedParam(m, prm);
        aiParams[m] = NormalizeParamSum1(prm);
    }
}
void BuildModelParamsSampled(const GameConst& gc, const EnemyAIParamEstimatorManager& enemyAiEstimator,
                             MArr<AIParam>& aiParams, Xor64& rand) {
    aiParams.resize(gc.M);
    FOR(m, 1, gc.M) {
        AIParam prm{};
        enemyAiEstimator.GetSampledEstimatedParam(m, prm, rand);
        aiParams[m] = NormalizeParam(prm);
    }
}


struct IAgent {
    virtual ~IAgent() {
    }

    virtual void Calc(ChronoTimer& timer, const GameConst& gc, const Game& game,
                      const EnemyAIParamEstimatorManager& enemyAiEstimator, NNBox<float>& probs, u8& commands,
                      Xor64& rand_) = 0;

    virtual void SetNoise(const NNBox<float>& probNoizse) = 0;

    virtual void Finalize() = 0;
};

#define USE_ZERO 0


template <class T>
struct MPArray {
   private:
    T* ptr_;
    int size_;
    friend struct MemoryPool;

   public:
    void Init() {
        ptr_ = nullptr;
        size_ = 0;
    }
    bool IsNull() const {
        return ptr_ == nullptr;
    }
    inline void clear() {
        ptr_ = nullptr;
        size_ = 0;
    }
    inline int size() const {
        VASSERT(ptr_);
        return size_;
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

    void fill(const T& v) {
        REP(i, size_) {
            ptr_[i] = v;
        }
    }
};

template <class T>
struct MPCapArray {
   private:
    T* ptr_;
    int size_;
    int capacity_;
    friend struct MemoryPool;

   public:
    void init() {
        ptr_ = nullptr;
        size_ = 0;
        capacity_ = 0;
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
    inline int capacity() const {
        return capacity_;
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

    void assign(int size, const T& v) {
        VASSERT(size <= capacity_);
        size_ = size;
        REP(i, size_) {
            ptr_[i] = v;
        }
    }
    void push_back(const T& v) {
        VASSERT(size_ < capacity_);
        ptr_[size_] = v;
        ++size_;
    }
};

template <class T>
struct MPArray2 {
   private:
    T* ptr_;
    int width_;
    int height_;
    friend struct MemoryPool;

   public:
    void Init() {
        ptr_ = nullptr;
        width_ = 0;
        height_ = 0;
    }
    bool IsNull() const {
        return ptr_ == nullptr;
    }
    inline int width() const {
        return width_;
    }
    inline int height() const {
        return height_;
    }
    inline T& at(int x, int y) {
        return ptr_[y * width_ + x];
    }
    inline const T& at(int x, int y) const {
        return ptr_[y * width_ + x];
    }
    inline T& operator()(int x, int y) {
        return ptr_[y * width_ + x];
    }
    inline const T& operator()(int x, int y) const {
        return ptr_[y * width_ + x];
    }
    inline int capacity() const {
        return width_ * height_;
    }
    inline T* data() {
        return ptr_;
    }

    void fill(const T& v) {
        REP(i, width_ * height_) {
            ptr_[i] = v;
        }
    }
};


struct MemoryPool {
    u8* buffer_;
    size_t capacity_;
    void* cur_;
    size_t space_;

    ~MemoryPool() {
        if (buffer_) {
            free(buffer_);
        }
    }

    MemoryPool() {
        buffer_ = nullptr;
        capacity_ = 0;
        cur_ = nullptr;
        space_ = 0;
    }

    inline void Init(size_t capacity) {
        if (buffer_) {
            free(buffer_);
        }
        buffer_ = (uint8_t*)malloc(capacity);
        capacity_ = capacity;
        cur_ = buffer_;
        space_ = capacity_;
    }

    inline void Clear() {
        cur_ = buffer_;
        space_ = capacity_;
    }

   public:
    inline void* Malloc(size_t alignment, size_t size) {
        void* alignedPtr = align(alignment, size, cur_, space_);
        if (!alignedPtr) {
            throw MemoryException();
        }


        reinterpret_cast<u8*&>(cur_) += size;
        space_ -= size;
        return alignedPtr;
    }

   public:
    template <class T>
    inline T* Malloc() {
        return (T*)Malloc(alignof(T), sizeof(T));
    }

    template <class T>
    inline MPArray<T> MallocArray(int count) {
        MPArray<T> ar;
        ar.ptr_ = (T*)Malloc(alignof(T), count * sizeof(T));
        ar.size_ = count;
        return ar;
    }

    template <class T>
    inline MPCapArray<T> MallocCapArray(int capacity) {
        MPCapArray<T> ar;
        ar.ptr_ = (T*)Malloc(alignof(T), capacity * sizeof(T));
        ar.size_ = 0;
        ar.capacity_ = capacity;
        return ar;
    }

    template <class T>
    inline MPCapArray<T> ReallocCapArray(const MPCapArray<T>& src, int newCapacity) {
        VASSERT(src.capacity() < newCapacity);
        MPCapArray<T> ar;
        ar.ptr_ = (T*)Malloc(alignof(T), newCapacity * sizeof(T));
        ar.size_ = src.size_;
        ar.capacity_ = newCapacity;
        if (ar.size_ > 0) {
            memcpy(ar.ptr_, src.ptr_, src.size_ * sizeof(T));
        }
        return ar;
    }

    template <class T>
    inline MPArray2<T> MallocArray2(int width, int height) {
        MPArray2<T> ar;
        ar.ptr_ = (T*)Malloc(alignof(T), width * height * sizeof(T));
        ar.width_ = width;
        ar.height_ = height;
        return ar;
    }

    s32 ToHandle(const void* ptr) const {
        return s32(((u8*)ptr) - buffer_);
    }
    template <class T>
    T* FromHandle(s32 handle) const {
        return (T*)&buffer_[handle];
    }

    inline double GetMemoryRate() const {
        return (capacity_ - space_) / (double)capacity_;
    }
};

#include <array>

template <class TValue, int CAPACITY>
class FixedTopK {
   public:
    static_assert(CAPACITY > 0, "CAPACITY must be positive");

    void Init(int limit) {
        limit_ = clamp(limit, 0, CAPACITY);
        size_ = 0;
    }

    int Size() const {
        return size_;
    }

    int Limit() const {
        return limit_;
    }

    template <class BETTER_FN>
    bool TryPush(const TValue& value, BETTER_FN&& betterFn) {
        if (limit_ <= 0) {
            return false;
        }
        if (size_ == limit_ && !betterFn(value, values_[size_ - 1])) {
            return false;
        }

        int insertPos = size_;
        if (size_ < limit_) {
            size_++;
        }
        else {
            insertPos = limit_ - 1;
        }

        while (insertPos > 0 && betterFn(value, values_[insertPos - 1])) {
            values_[insertPos] = values_[insertPos - 1];
            insertPos--;
        }
        values_[insertPos] = value;
        return true;
    }

    const TValue& operator[](int idx) const {
        return values_[idx];
    }

   private:
    std::array<TValue, CAPACITY> values_{};
    int size_ = 0;
    int limit_ = 0;
};


struct SafeMap {
    MArr<double> enemyTargetScores_;

    struct Cell {
        bool inited_;
        bool safeLevel1_;
        bool safeLevel2_;

        int GetSafeLevel() const {
            VASSERT(inited_);
            if (safeLevel1_) {
                return 1;
            }
            else if (safeLevel2_) {
                return 2;
            }
            else {
                return 0;
            }
        }

        void Clear() {
            inited_ = false;
            safeLevel1_ = false;
            safeLevel2_ = false;
        }

        void SetDanger() {
            inited_ = true;
            safeLevel1_ = false;
            safeLevel2_ = false;
        }

        void Set(bool safeLevel1, bool safeLevel2) {
            inited_ = true;
            safeLevel1_ = safeLevel1;
            safeLevel2_ = safeLevel2;
        }
    };
    mutable NNBox<Cell> cells_;

    void Init(const GameConst& gc, const State& state, const MArr<AIParam>& aiParams) {
        enemyTargetScores_.assign(gc.M, 0.0);
        cells_.fill({false, false, false});

        cauto& owners = state.owners_;
        cauto& levels = state.levels_;
        cauto& poss = state.poss_;

        static constexpr double kJudgeTieTolerance = 1e-9;
        FOR(m, 1, gc.M) {
            cauto& prm = aiParams[m];
            double bestScore = -1e100;  

            constexpr int SkipCount = 0;  

            struct Cand {
                int cell;
                double score;
            };
            FixedTopK<Cand, SkipCount + 1> topK;
            topK.Init(SkipCount + 1);
            for (auto cell : state.movableSets_[m]) {
                double score = EvalAICellScore(gc, owners, levels, m, cell, prm);
                topK.TryPush({cell, score}, [](const Cand& a, const Cand& b) { return a.score > b.score; });
            }

            int moveCount = 0;
            REP(i, topK.Size()) {
                cauto& cand = topK[i];
                if (owners[cand.cell] == m) {
                    int upCount = gc.U - levels[cand.cell];
                    if (upCount < 0) {
                        upCount = 1;
                    }
                    if (moveCount + upCount <= SkipCount && i + 1 < topK.Size()) {
                        moveCount += upCount;
                        continue;
                    }
                }
                bestScore = cand.score;
                break;
            }
            VASSERT(bestScore > -1e100);

            enemyTargetScores_[m] = bestScore - bestScore * kJudgeTieTolerance;
        }

        FOR(m, 1, gc.M) {
            auto cell = poss[m];
            cells_[cell] = {true, false, false};
        }
    }

    void UpdateCell(const GameConst& gc, const State& state, const MArr<AIParam>& aiParams, int cell) const {
        if (cells_[cell].inited_) {
            return;
        }

        auto IsMovable = [&](int cell, int m) -> bool { return state.movableSets_[m].contains(cell); };

        auto IsTargetedByEnemy = [&](int cell, int enemyM) -> bool {
            VASSERT(enemyM > 0);
            cauto& prm = aiParams[enemyM];
            double score = gc.Vs[cell] * aiParams[enemyM].wa;
            return score >= enemyTargetScores_[enemyM];
        };
        auto IsGuardedByEnemy = [&](int cell, int enemyM) -> bool {
            VASSERT(enemyM > 0);
            cauto& prm = aiParams[enemyM];
            double score = gc.Vs[cell] * aiParams[enemyM].wb;
            return score >= enemyTargetScores_[enemyM];
        };
        auto IsAttackedByEnemyLevel1 = [&](int cell, int enemyM) -> bool {
            VASSERT(enemyM > 0);
            double score = gc.Vs[cell] * aiParams[enemyM].wc;
            return score >= enemyTargetScores_[enemyM];
        };
        auto IsAttackedByEnemyLevel2 = [&](int cell, int enemyM) -> bool {
            VASSERT(enemyM > 0);
            VASSERT(gc.U >= 2);
            double score = gc.Vs[cell] * aiParams[enemyM].wd;
            return score >= enemyTargetScores_[enemyM];
        };

        cauto& owners = state.owners_;
        cauto& levels = state.levels_;
        cauto& poss = state.poss_;

        if (owners[cell] > 0) {
            if (IsMovable(cell, owners[cell]) && IsGuardedByEnemy(cell, owners[cell])) {
                cells_[cell].SetDanger();
                return;
            }

            MArr<bool> ownersChecked(gc.M, false);
            ownersChecked[owners[cell]] = true;
            bool attackedByLevel1 = false;
            bool attackedByLevel2 = false;
            for (auto n : gs.around(cell)) {
                auto owner = owners[n];
                if (owner > 0 && !ownersChecked[owner]) {
                    ownersChecked[owner] = true;

                    if (IsMovable(cell, owner)) {
                        if (IsAttackedByEnemyLevel1(cell, owner)) {
                            attackedByLevel1 = true;
                        }
                        if (levels[cell] >= 2 && IsAttackedByEnemyLevel2(cell, owner)) {
                            attackedByLevel2 = true;
                        }
                    }
                }
            }
            if (attackedByLevel1 || attackedByLevel2) {
                cells_[cell].SetDanger();
                return;
            }
        }

        if (owners[cell] < 0) {
            MArr<bool> ownersChecked(gc.M, false);
            for (auto n : gs.around(cell)) {
                auto owner = owners[n];
                if (owner > 0 && !ownersChecked[owner]) {
                    ownersChecked[owner] = true;

                    if (IsMovable(cell, owner)) {
                        if (IsTargetedByEnemy(cell, owner)) {
                            cells_[cell].SetDanger();
                            return;
                        }
                    }
                }
            }
        }

        bool attackedByLevel1 = false;
        bool attackedByLevel2 = false;
        {
            MArr<bool> ownersChecked(gc.M, false);
            for (auto n : gs.around(cell)) {
                auto owner = owners[n];
                if (owner > 0 && !ownersChecked[owner]) {
                    ownersChecked[owner] = true;

                    if (IsMovable(cell, owner)) {
                        if (IsAttackedByEnemyLevel1(cell, owner)) {
                            attackedByLevel1 = true;
                        }
                        if (gc.U >= 2 && IsAttackedByEnemyLevel2(cell, owner)) {
                            attackedByLevel2 = true;
                        }
                    }
                }
            }
        }
        if (gc.U == 1) {
            attackedByLevel2 = true;
        }

        cells_[cell].Set(!attackedByLevel1, !attackedByLevel2);
    }


    bool IsSafeLevel1(const GameConst& gc, const State& state, const MArr<AIParam>& aiParams, int cell) const {
        UpdateCell(gc, state, aiParams, cell);
        return cells_[cell].safeLevel1_;
    }
    bool IsSafeLevel2(const GameConst& gc, const State& state, const MArr<AIParam>& aiParams, int cell) const {
        UpdateCell(gc, state, aiParams, cell);
        return cells_[cell].safeLevel2_;
    }

    int GetSafeLevel(const GameConst& gc, const State& state, const MArr<AIParam>& aiParams, int cell) const {
        UpdateCell(gc, state, aiParams, cell);
        return cells_[cell].GetSafeLevel();
    }
};

void EvalAction3B(const GameConst& gc, int turn, const State& state, const MArr<NNBox<double>>& playersProbs,
                  const MArr<AIParam>& aiParams, const NNArr<u8>& movableCells, NNArr<double>& evals) {
    cauto& scores = state.scores_;
    int targetM = CalcTargetM(gc, scores);

    array<double, M_max> enemyThreatScales;
    enemyThreatScales[0] = 1;
    FOR(m, 1, gc.M) {
        const AIParam& prm = aiParams[m];
        const double threat = 1.0 + prm.wa * HP.EvalAction3_EnemyWaThreatRate +
                              prm.wb * HP.EvalAction3_EnemyWbThreatRate + prm.wc * HP.EvalAction3_EnemyWcThreatRate +
                              prm.wd * HP.EvalAction3_EnemyWdThreatRate;
        enemyThreatScales[m] = clamp(threat, 0.05, 3.0);
    }

    const double myScore = scores[0];
    const double targetScore = scores[targetM];
    const double scoreRatio =
        clamp(myScore / targetScore, (double)HP.EvalAction3_ScoreRatioMin, (double)HP.EvalAction3_ScoreRatioMax);

    const double phase = turn / (double)(T - 1);
    const double gainWeight =
        (HP.EvalAction3_GainWeightEarly + (HP.EvalAction3_GainWeightLate - HP.EvalAction3_GainWeightEarly) * phase);
    const double denyWeight =
        (HP.EvalAction3_DenyWeightEarly + (HP.EvalAction3_DenyWeightLate - HP.EvalAction3_DenyWeightEarly) * phase);
    const double otherDenyWeight = denyWeight * HP.EvalAction3_OtherDenyRate;
    const double defendWeight = (HP.EvalAction3_DefendWeightEarly +
                                 (HP.EvalAction3_DefendWeightLate - HP.EvalAction3_DefendWeightEarly) * phase);
    const double frontierWeight = (HP.EvalAction3_FrontierWeightEarly +
                                   (HP.EvalAction3_FrontierWeightLate - HP.EvalAction3_FrontierWeightEarly) * phase);

    auto CalcEnemyNoProb = [&](int cell) -> double {
        double p0 = 1.0;  
        FOR(m, 1, gc.M) {
            p0 *= (1.0 - playersProbs[m][cell]);
        }
        return p0;
    };
    auto CalcFrontierBase = [&](int cell) -> int {
        int frontier = 0;
        for (auto n : gs.around(cell)) {
            if (state.owners_[n] != 0) {
                frontier += gc.Vs[n];
            }
        }
        return frontier;
    };

    const auto myPos = state.poss_[0];

    evals.resize(movableCells.size());
    REP(i, movableCells.size()) {
        const int cell = movableCells[i];
        const auto owner = state.owners_[cell];
        const auto level = state.levels_[cell];
        const auto v = gc.Vs[cell];

        if (owner == 0) {
            const bool canGrow = (level < gc.U);

            double defendGain = 0.0;
            if (cell != myPos) {
                const double anyEnemyThreat = 1.0 - CalcEnemyNoProb(cell);
                defendGain = anyEnemyThreat * v;
                if (level == 1) {
                    defendGain *= HP.EvalAction3_DefendLevel1Boost;
                }
            }

            const double frontier = CalcFrontierBase(cell) * frontierWeight;  
            const double capPenalty = canGrow ? 0.0 : (v * HP.EvalAction3_CapPenaltyRate);
            const double myGain = canGrow ? v : 0.0;

            double eval = 0.0;
            eval += gainWeight * myGain;
            eval += defendWeight * defendGain;
            eval += frontier;
            eval -= capPenalty;
            eval += v * HP.EvalAction3_TieBreakRate;
            evals[i] = eval;
            continue;
        }

        const double pNoEnemy = CalcEnemyNoProb(cell);
        const double successProb = pNoEnemy;
        const double collideProb = 1.0 - pNoEnemy;
        const double congestionPenalty = collideProb * v * HP.EvalAction3_CongestionPenaltyRate;

        if (owner < 0) {
            const double myGain = v * successProb;
            const double frontier = CalcFrontierBase(cell) * successProb * frontierWeight;

            double eval = 0.0;
            eval += gainWeight * myGain;
            eval += frontier;
            eval -= congestionPenalty;
            eval += v * HP.EvalAction3_TieBreakRate;
            evals[i] = eval;
            continue;
        }

        const bool isFinishAttack = (level == 1);
        const double myGain = isFinishAttack ? (v * successProb) : 0.0;
        double enemyLoss = v * successProb * enemyThreatScales[owner];  
        const double targetLoss = (owner == targetM) ? enemyLoss : 0.0;
        const double otherLoss = (owner == targetM) ? 0.0 : enemyLoss;
        const double frontier = isFinishAttack ? (CalcFrontierBase(cell) * successProb * frontierWeight) : 0.0;

        double eval = 0.0;
        eval += gainWeight * myGain;
        eval += scoreRatio * denyWeight * targetLoss;
        eval += scoreRatio * otherDenyWeight * otherLoss;
        eval += frontier;
        eval -= congestionPenalty;
        eval += v * HP.EvalAction3_TieBreakRate;

        evals[i] = eval;
    }
}



#define EVALSTATE4_VALUE_FORMULA 0



double EvalState4Impl(const GameConst& gc, int turn, const State& state, const MArr<AIParam>& aiParams,
                      bool useBestEnemyOnly, int rawScoreTurn) {
    cauto& playersScore = state.scores_;
    const int fixedTargetM = CalcTargetM(gc, playersScore);
    auto IsEvalPlayer = [&](int m) -> bool {
        if (!useBestEnemyOnly) {
            return true;
        }
        return m == 0 || m == fixedTargetM;
    };

    const int rawTurn = clamp(rawScoreTurn, 0, T);
    if (turn >= rawTurn) {
        const double myScore = playersScore[0];
        const double targetScore = playersScore[fixedTargetM];
#if EVALSTATE4_VALUE_FORMULA == 0
        const double denom = max(1e-9, myScore + targetScore);
        const double valueScore = (myScore - targetScore) / denom;
#elif EVALSTATE4_VALUE_FORMULA == 1
        const double valueScore = log2(1.0 + myScore / targetScore) - 1.0;
#else
#error "invalid EVALSTATE4_VALUE_FORMULA"
#endif
        return valueScore * HP.EvalState4_Scale;
    }

    const double phase = clamp((double)turn / (double)max(1, T - 1), 0.0, 1.0);
    const double earlyPhase = 1.0 - phase;

    double boardAvgValue = 0.0;
    REP(cell, NN) {
        boardAvgValue += (double)gc.Vs[cell];
    }
    boardAvgValue /= (double)max(1, NN);

    array<double, M_max> curScores{};
    array<double, M_max> growStocks{};                
    array<double, M_max> weakRisks{};                 
    array<double, M_max> islandPenalties{};           
    array<double, M_max> stableBonuses{};             
    array<double, M_max> earlyLowValueSafeBonuses{};  
    array<double, M_max> lateHighValueSafeBonuses{};  
    array<double, M_max> neutralThreatPenalties{};    
    array<double, M_max> enemyContactPenalties{};     
    array<double, M_max> enemyNearBonuses{};          
    array<double, M_max> enemyFarBonuses{};           
    array<double, M_max> expectScores{};

    REP(cell, NN) {
        const int owner = (int)state.owners_[cell];
        if (owner < 0 || !IsEvalPlayer(owner)) {
            continue;
        }
        const int level = (int)state.levels_[cell];
        const double v = (double)gc.Vs[cell];
        const double score = v * (double)level;

        curScores[owner] += score;
        if (level < gc.U) {
            growStocks[owner] += v * (double)(gc.U - level);
        }
        if (level > 1) {
            stableBonuses[owner] += v * (double)(level - 1);
        }
        if (!state.reachables_[owner].test(cell)) {
            islandPenalties[owner] += score;
        }

        for (int n : gs.around(cell)) {
            const int no = (int)state.owners_[n];
            if (no >= 0 && no != owner && (!useBestEnemyOnly || IsEvalPlayer(no))) {
                enemyContactPenalties[owner] += 0.5 * (v + (double)gc.Vs[n]);
            }
        }
    }

    const int oppCount = useBestEnemyOnly ? 1 : max(1, gc.M - 1);
    const double invOppCount = 1.0 / (double)oppCount;
    array<double, M_max> enemyThreatScales{};
    enemyThreatScales.fill(1.0);
    FOR(m, 1, gc.M) {
        if (!IsEvalPlayer(m)) {
            continue;
        }
        if (m >= (int)aiParams.size()) {
            continue;
        }
        const AIParam& prm = aiParams[m];
        const double wa = max(0.0, prm.wa);
        const double wb = max(0.0, prm.wb);
        const double wc = max(0.0, prm.wc);
        const double wd = max(0.0, prm.wd);
        const double sumW = wa + wb + wc + wd;
        if (sumW <= 1e-12) {
            continue;
        }

        const double waRate = wa / sumW;
        const double wbRate = wb / sumW;
        const double wcRate = wc / sumW;
        const double wdRate = wd / sumW;
        const double threat = 1.0 + waRate * HP.EvalState4_EnemyWaThreatRate +
                              wbRate * HP.EvalState4_EnemyWbThreatRate + wcRate * HP.EvalState4_EnemyWcThreatRate +
                              wdRate * HP.EvalState4_EnemyWdThreatRate;
        enemyThreatScales[m] = clamp(threat, 0.05, 3.0);
    }

    array<double, NN> moverThreatSums{};
    FOR(m, 0, gc.M) {
        if (!IsEvalPlayer(m)) {
            continue;
        }
        const double threat = enemyThreatScales[m];
        for (int cell : state.movableSets_[m]) {
            moverThreatSums[cell] += threat;
        }
    }
    FOR(attacker, 0, gc.M) {
        if (!IsEvalPlayer(attacker)) {
            continue;
        }
        const double attackerThreat = enemyThreatScales[attacker];
        for (int cell : state.movableSets_[attacker]) {
            const int owner = (int)state.owners_[cell];
            if (owner < 0 || owner == attacker || !IsEvalPlayer(owner)) {
                continue;
            }
            if ((int)state.levels_[cell] != 1) {
                continue;
            }
            weakRisks[owner] += (double)gc.Vs[cell] * attackerThreat * invOppCount;
        }
    }
    FOR(m, 0, gc.M) {
        if (!IsEvalPlayer(m)) {
            continue;
        }
        const double selfThreat = enemyThreatScales[m];
        for (int cell : state.movableSets_[m]) {
            if ((int)state.owners_[cell] >= 0) {
                continue;
            }
            const double v = (double)gc.Vs[cell];
            const double otherMoverThreat = max(0.0, moverThreatSums[cell] - selfThreat);
            const double threatRate = clamp(otherMoverThreat * invOppCount, 0.0, 1.0);
            const double safeRate = 1.0 - threatRate;
            const double lowValue = max(0.0, boardAvgValue - v);
            const double highValue = max(0.0, v - boardAvgValue);

            earlyLowValueSafeBonuses[m] += lowValue * safeRate;
            lateHighValueSafeBonuses[m] += highValue * safeRate;
            neutralThreatPenalties[m] += v * threatRate;
        }
    }

    const double invMaxGridDist = 1.0 / (double)max(1, 2 * (N - 1));
    FOR(m, 0, gc.M) {
        if (!IsEvalPlayer(m)) {
            continue;
        }
        NNArr<u8> enemyCells;
        REP(cell, NN) {
            const int owner = (int)state.owners_[cell];
            if (owner >= 0 && owner != m && (!useBestEnemyOnly || IsEvalPlayer(owner))) {
                enemyCells.push(cell);
            }
        }
        NNBox<u8> distToEnemy;
        if (enemyCells.empty()) {
            distToEnemy.fill(0);
        }
        else {
            CalcDist(enemyCells, distToEnemy);
        }
        REP(cell, NN) {
            if ((int)state.owners_[cell] != m) {
                continue;
            }
            const double score = (double)gc.Vs[cell] * (double)state.levels_[cell];
            const double d = (double)distToEnemy[cell];
            enemyNearBonuses[m] += score / (1.0 + d);
            enemyFarBonuses[m] += score * d * invMaxGridDist;
        }
    }

    FOR(m, 0, gc.M) {
        if (!IsEvalPlayer(m)) {
            continue;
        }
        const double neutralThreatRate =
            earlyPhase * HP.EvalState4_NeutralThreatRateEarly + phase * HP.EvalState4_NeutralThreatRateLate;
        double expect = curScores[m];
        expect += growStocks[m] * HP.EvalState4_GrowStockRate;
        expect -= weakRisks[m] * HP.EvalState4_WeakRiskRate;
        expect -= islandPenalties[m] * HP.EvalState4_IslandPenaltyRate;
        expect += stableBonuses[m] * HP.EvalState4_StableBonusRate;
        expect += earlyPhase * earlyLowValueSafeBonuses[m] * HP.EvalState4_EarlyLowValueSafeRate;
        expect += phase * lateHighValueSafeBonuses[m] * HP.EvalState4_LateHighValueSafeRate;
        expect -= neutralThreatPenalties[m] * neutralThreatRate;
        expect -= enemyContactPenalties[m] * HP.EvalState4_ContactPenaltyRate;
        expect += enemyNearBonuses[m] * HP.EvalState4_EnemyNearRate;
        expect += enemyFarBonuses[m] * HP.EvalState4_EnemyFarRate;
        expectScores[m] = max(1.0, expect);
    }


    int targetM = fixedTargetM;
    if (!useBestEnemyOnly) {
        FOR(m, 2, gc.M) {
            if (expectScores[m] > expectScores[targetM]) {
                targetM = m;
            }
        }
    }

    const double myScore = expectScores[0];
    const double targetScore = max(1e-9, expectScores[targetM]);
#if EVALSTATE4_VALUE_FORMULA == 0
    const double denom = max(1e-9, myScore + targetScore);
    const double valueScore = (myScore - targetScore) / denom;
#elif EVALSTATE4_VALUE_FORMULA == 1
    const double valueScore = log2(1.0 + myScore / targetScore) - 1.0;
#else
#error "invalid EVALSTATE4_VALUE_FORMULA"
#endif
    return valueScore * HP.EvalState4_Scale;
}

double EvalState4(const GameConst& gc, int turn, const State& state, const MArr<AIParam>& aiParams, int rawScoreTurn) {
    return EvalState4Impl(gc, turn, state, aiParams, false, rawScoreTurn);
}

double EvalState4B(const GameConst& gc, int turn, const State& state, const MArr<AIParam>& aiParams, int rawScoreTurn) {
    return EvalState4Impl(gc, turn, state, aiParams, true, rawScoreTurn);
}

double EvalState5(const GameConst& gc, int turn, const State& state, const MArr<AIParam>& aiParams, int rawScoreTurn) {
    auto ToCaseObjective = [&](double myScore, double targetScore) -> double {
        myScore = max(1.0, myScore);
        targetScore = max(1e-9, targetScore);
        return (myScore - targetScore) / (myScore + targetScore);
    };

    cauto& playersScore = state.scores_;
    const int fixedTargetM = CalcTargetM(gc, playersScore);

    const int rawTurn = clamp(rawScoreTurn, 0, T);
    if (turn >= rawTurn) {
        const double myScore = (double)playersScore[0];
        const double targetScore = (double)playersScore[fixedTargetM];
        return ToCaseObjective(myScore, targetScore);
    }

    const double remainTurns = (double)max(0, T - turn);
    const double remainRate = clamp(remainTurns / (double)max(1, T), 0.0, 1.0);
    const double phase = 1.0 - remainRate;
    auto BlendEarlyLate = [&](double early, double late) -> double { return early * remainRate + late * phase; };

    array<double, M_max> curScores{};
    array<double, M_max> growStocks{};            
    array<double, M_max> stableStocks{};          
    array<double, M_max> islandPenalties{};       
    array<double, M_max> frontierContactRisks{};  
    array<double, M_max> weakRisks{};             
    array<double, M_max> neutralSafeStocks{};     
    array<double, M_max> neutralContestStocks{};  
    array<double, M_max> finishStocks{};          
    array<double, M_max> chipStocks{};            
    array<double, M_max> bestGrow{};
    array<double, M_max> bestNeutralSafe{};
    array<double, M_max> bestFinish{};
    array<double, M_max> bestChip{};
    array<double, M_max> movableValues{};  
    array<double, M_max> expectScores{};

    REP(cell, NN) {
        const int owner = (int)state.owners_[cell];
        if (owner < 0) {
            continue;
        }

        const int level = (int)state.levels_[cell];
        const double v = (double)gc.Vs[cell];
        const double score = v * (double)level;
        curScores[owner] += score;

        if (level < gc.U) {
            growStocks[owner] += v * (double)(gc.U - level);
        }
        if (level > 1) {
            stableStocks[owner] += v * (double)(level - 1);
        }
        if (!state.reachables_[owner].test(cell)) {
            islandPenalties[owner] += score;
        }

        for (int n : gs.around(cell)) {
            const int no = (int)state.owners_[n];
            if (no >= 0 && no != owner) {
                frontierContactRisks[owner] += 0.5 * (v + (double)gc.Vs[n]);
            }
        }
    }
    FOR(m, 0, gc.M) {
        for (int cell : state.movableSets_[m]) {
            movableValues[m] += (double)gc.Vs[cell];
        }
    }

    array<double, M_max> threatScales{};
    threatScales.fill(1.0);
    const double threatMin = min(HP.EvalState5_ThreatMin, HP.EvalState5_ThreatMax);
    const double threatMax = max(HP.EvalState5_ThreatMin, HP.EvalState5_ThreatMax);
    FOR(m, 1, gc.M) {
        if (m >= (int)aiParams.size()) {
            continue;
        }
        const AIParam& prm = aiParams[m];
        const double wa = max(0.0, prm.wa);
        const double wb = max(0.0, prm.wb);
        const double wc = max(0.0, prm.wc);
        const double wd = max(0.0, prm.wd);
        const double sumW = wa + wb + wc + wd;
        if (sumW <= 1e-12) {
            continue;
        }
        const double expandRate = wa / sumW;
        const double growRate = wb / sumW;
        const double finishRate = wc / sumW;
        const double chipRate = wd / sumW;
        const double threat = HP.EvalState5_ThreatBase + HP.EvalState5_ThreatWaRate * expandRate +
                              HP.EvalState5_ThreatWbRate * growRate + HP.EvalState5_ThreatWcRate * finishRate +
                              HP.EvalState5_ThreatWdRate * chipRate;
        threatScales[m] = clamp(threat, threatMin, threatMax);
    }

    array<double, NN> moverThreatSums{};
    FOR(m, 0, gc.M) {
        const double threat = threatScales[m];
        for (int cell : state.movableSets_[m]) {
            moverThreatSums[cell] += threat;
        }
    }

    const int oppCount = max(1, gc.M - 1);
    const double invOppCount = 1.0 / (double)oppCount;
    FOR(attacker, 0, gc.M) {
        const double selfThreat = threatScales[attacker];
        for (int cell : state.movableSets_[attacker]) {
            const int owner = (int)state.owners_[cell];
            const int level = (int)state.levels_[cell];
            const double v = (double)gc.Vs[cell];

            if (owner < 0) {
                const double otherThreat = max(0.0, moverThreatSums[cell] - selfThreat);
                const double contestRate = otherThreat / (selfThreat + otherThreat + 1e-9);
                const double safeRate = 1.0 - contestRate;
                neutralSafeStocks[attacker] += v * safeRate;
                neutralContestStocks[attacker] += v * contestRate;
                bestNeutralSafe[attacker] = max(bestNeutralSafe[attacker], v * safeRate);
                continue;
            }

            if (owner == attacker) {
                if (level < gc.U) {
                    bestGrow[attacker] = max(bestGrow[attacker], v);
                }
                continue;
            }

            if (level == 1) {
                finishStocks[attacker] += v;
                bestFinish[attacker] = max(bestFinish[attacker], v);
                weakRisks[owner] += v * selfThreat * invOppCount;
            }
            else {
                chipStocks[attacker] += v;
                bestChip[attacker] = max(bestChip[attacker], v);
            }
        }
    }

    FOR(m, 0, gc.M) {
        const double stockCap =
            growStocks[m] * BlendEarlyLate(HP.EvalState5_GrowStockRateEarly, HP.EvalState5_GrowStockRateLate) +
            neutralSafeStocks[m] *
                BlendEarlyLate(HP.EvalState5_NeutralSafeStockRateEarly, HP.EvalState5_NeutralSafeStockRateLate) +
            finishStocks[m] * BlendEarlyLate(HP.EvalState5_FinishStockRateEarly, HP.EvalState5_FinishStockRateLate) +
            chipStocks[m] * BlendEarlyLate(HP.EvalState5_ChipStockRateEarly, HP.EvalState5_ChipStockRateLate);
        const double perTurnGain =
            max(bestNeutralSafe[m] * HP.EvalState5_BestNeutralSafeRate,
                max(bestGrow[m] * HP.EvalState5_BestGrowRate,
                    max(bestFinish[m] * HP.EvalState5_BestFinishRate, bestChip[m] * HP.EvalState5_BestChipRate)));
        const double gainTurnCap =
            max(1.0, BlendEarlyLate((double)HP.EvalState5_GainTurnCapEarly, (double)HP.EvalState5_GainTurnCapLate));
        const double gainTurns = min(remainTurns, gainTurnCap);
        const double horizonGain = min(stockCap, perTurnGain * gainTurns);

        double expect = curScores[m];
        expect += horizonGain * BlendEarlyLate(HP.EvalState5_HorizonGainRateEarly, HP.EvalState5_HorizonGainRateLate);
        expect += stableStocks[m] * BlendEarlyLate(HP.EvalState5_StableRateEarly, HP.EvalState5_StableRateLate);
        expect +=
            movableValues[m] * BlendEarlyLate(HP.EvalState5_MovableValueRateEarly, HP.EvalState5_MovableValueRateLate);
        expect -= weakRisks[m] * BlendEarlyLate(HP.EvalState5_WeakRiskRateEarly, HP.EvalState5_WeakRiskRateLate);
        expect -= neutralContestStocks[m] *
                  BlendEarlyLate(HP.EvalState5_NeutralContestRiskRateEarly, HP.EvalState5_NeutralContestRiskRateLate);
        expect -= frontierContactRisks[m] *
                  BlendEarlyLate(HP.EvalState5_ContactRiskRateEarly, HP.EvalState5_ContactRiskRateLate);
        expect -= islandPenalties[m] *
                  BlendEarlyLate(HP.EvalState5_IslandPenaltyRateEarly, HP.EvalState5_IslandPenaltyRateLate);
        expectScores[m] = max(1.0, expect);
    }

    int targetM = 1;
    FOR(m, 2, gc.M) {
        if (expectScores[m] > expectScores[targetM]) {
            targetM = m;
        }
    }

    return ToCaseObjective(expectScores[0], expectScores[targetM]);
}

#define USE_GETTABLE_MOVABLE 0

#define REUSE_NODE 0

struct MoveTargetDecider5B {
    static constexpr int kMaxActionsPerNode = 28;
    static constexpr int kPwVisitTableMax = 8192;
    static constexpr int kZobristValueStates = 256;
    struct AIScoreCache {
        MArr<NNBox<double>> neutralScores;
        MArr<NNBox<double>> growScores;
        MArr<NNBox<double>> finishAttackScores;
        MArr<NNBox<double>> chipAttackScores;
    };

    struct ActionStat {
        int cell = -1;
        int visits = 0;
        double valueSum = 0.0;
        double prior = 0.0;  
        int branchCount = 0;
    };

    struct Node {
        State state;
        uint64_t hash = 0;
        int visits = 0;
        double valueSum = 0.0;
        bool initialized = false;
        int activeActionCount = 0;                       
        int maxActionCount = 0;                          
        CapArr<ActionStat, kMaxActionsPerNode> actions;  
        MArr<NNBox<double>> cachedPlayersProbs;
    };

    long long perfTotalIterations_ = 0;
    long long perfTotalUsedUs_ = 0;
    long long perfTotalNodes_ = 0;
    int perfTurns_ = 0;
    MemoryPool pool_;
    dhmap<u64, int> hashToNodeHandle_;  
    bool zobristInitialized_ = false;
    NNBox<array<u64, kZobristValueStates>> zobristOwnerKeys_;
    NNBox<array<u64, kZobristValueStates>> zobristLevelKeys_;
    array<array<u64, kZobristValueStates>, M_max> zobristPosKeys_;
    array<u64, T + 1> zobristTurnKeys_{};

    int simCount_ = -1;
    void SetSimCount(int simCount) {
        simCount_ = simCount;
    }

    void Init() {
        pool_.Init(500 * 1024 * 1024);
        hashToNodeHandle_.init(1024 * 100);
        InitZobristTables();
    }

    int DecideTargetAndProb(ChronoTimer& timer, const GameConst& gc, const State& state,
                            const EnemyAIParamEstimatorManager& enemyAiEstimator, int turnNow, NNBox<float>& probs,
                            Xor64& rand) {
        hashToNodeHandle_.clear();
        pool_.Clear();


        const int horizon = CalcSearchHorizon(turnNow);
        const u64 rootHash = HashState(gc, state, turnNow);

        Node* root = nullptr;
        {
            if (root == nullptr) {
                root = BeginSearchTree(state, rootHash);
            }
        }

        MArr<AIParam> aiParams;
        BuildModelParamsSum1(gc, enemyAiEstimator, aiParams);
        AIScoreCache aiScoreCache;
        BuildAIScoreCache(gc, aiParams, aiScoreCache);

        const int startUs = timer.ElapseTimeUs();

        int iterations = 0;
        while (true) {

            Simulate(gc, root, horizon, 0, turnNow, enemyAiEstimator, aiParams, aiScoreCache, rand);
            iterations++;

            if (simCount_ > 0) {
                if (iterations >= simCount_) {
                    break;
                }
            }
            else {
                if (timer.IsOver()) {
                    break;
                }
            }
        }
        const int usedUs = max(0, timer.ElapseTimeUs() - startUs);
        const int nodeCount = 0;
        const int rootActionCount = root->initialized ? root->activeActionCount : 0;

        int bestCell = root->actions[0].cell;
        int bestVisits = -1;
        double bestTieScore = -1e100;
        int totalVisits = 0;
        for (const ActionStat& action : root->actions) {
            const int visits = action.visits;
            const double mean = (visits > 0) ? (action.valueSum / (double)visits) : -1e100;
            const double tieScore = (visits > 0) ? mean : action.prior;
            if (visits > bestVisits || (visits == bestVisits && tieScore > bestTieScore + 1e-9)) {
                bestVisits = visits;
                bestTieScore = tieScore;
                bestCell = action.cell;
            }
            totalVisits += visits;
        }
        probs.fill(0.0f);
        for (const ActionStat& action : root->actions) {
            const int visits = action.visits;
            const float prob = (totalVisits > 0) ? ((float)visits / (float)totalVisits) : 0.0f;
            probs[action.cell] = prob;
        }
        LogSearchPerf(turnNow, usedUs, horizon, iterations, nodeCount, rootActionCount, bestVisits, true, root);

        return bestCell;
    }

   private:

    Node* BeginSearchTree(const State& rootState, uint64_t rootHash) {
        Node* node = pool_.Malloc<Node>();
        ResetNode(*node, rootState, rootHash);
        return node;
    }

    void ResetNode(Node& node, const State& state, uint64_t hash) {
        node.state = state;
        node.hash = hash;
        node.visits = 0;
        node.valueSum = 0.0;
        node.initialized = false;
        node.activeActionCount = 0;
        node.maxActionCount = 0;
        node.actions.clear();
        node.cachedPlayersProbs.clear();
    }

    void LogSearchPerf(int turnNow, int usedUs, int horizon, int iterations, int nodeCount, int rootActionCount,
                       int bestVisits, bool usedMcts, const Node* rootNode) {
        perfTotalIterations_ += iterations;
        perfTotalUsedUs_ += usedUs;
        perfTotalNodes_ += nodeCount;
        perfTurns_++;

        const double iterPerMs = (usedUs > 0) ? ((double)iterations * 1000.0 / (double)usedUs) : 0.0;


    }

    int CalcSearchHorizon(int turnNow) const {
        return max(1, min((int)HP.Decider_SearchHorizon, T - turnNow));
    }

    int CalcNodeActionLimit(int depth, int turnNow, int candCount) const {
        int limit = (depth == 0) ? (int)HP.Decider_NodeActionBaseRoot : (int)HP.Decider_NodeActionBaseInner;
        return min(limit, candCount);

    }

    int CalcInitialActiveActionCount(int depth, int maxActionCount) const {
        const int base = (depth == 0) ? HP.Decider_PwRootInit : HP.Decider_PwInnerInit;
        return clamp(base, 1, maxActionCount);
    }

    void UpdateActionSetByProgressiveWidening(Node& node) const {
        if (node.maxActionCount <= 0) {
            node.activeActionCount = 0;
            return;
        }

        const int n = max(1, node.visits);
        const double pwAlpha = (double)HP.Decider_PwAlpha;
        const double pwCoeff = (double)HP.Decider_PwCoeff;
        static thread_local array<int, kPwVisitTableMax + 1> pwTable{};
        static thread_local bool pwTableInited = false;
        static thread_local double lastAlpha = -1.0;
        static thread_local double lastCoeff = -1.0;
        if (!pwTableInited || fabs(lastAlpha - pwAlpha) > 1e-12 || fabs(lastCoeff - pwCoeff) > 1e-12) {
            REP(i, kPwVisitTableMax + 1) {
                const int visits = max(1, i);
                pwTable[i] = 1 + (int)floor(pwCoeff * pow((double)visits, pwAlpha));
            }
            lastAlpha = pwAlpha;
            lastCoeff = pwCoeff;
            pwTableInited = true;
        }

        int target = 0;
        if (n <= kPwVisitTableMax) {
            target = pwTable[n];
        }
        else {
            target = 1 + (int)floor(pwCoeff * pow((double)n, pwAlpha));
        }
        target = max(target, node.activeActionCount);
        target = min(target, node.maxActionCount);
        node.activeActionCount = max(1, target);
    }

    double ConvertEvalForPW(const GameConst& gc, int cell, double rawEval) const {
        return rawEval + (double)gc.Vs[cell] * HP.Decider_PwValueBonus;
    }

    double ConvertEvalForPrior(double rawEval) const {
        const double scale = (double)HP.Decider_PriorScoreCompress;
        return tanh(rawEval / scale) * scale;
    }

    double ConvertQForPuct(double rawQ) const {
        return rawQ;
    }

    void BuildAIScoreCache(const GameConst& gc, const MArr<AIParam>& aiParams, AIScoreCache& cache) const {
        cache.neutralScores.resize(gc.M);
        cache.growScores.resize(gc.M);
        cache.finishAttackScores.resize(gc.M);
        cache.chipAttackScores.resize(gc.M);

        FOR(m, 1, gc.M) {
            const AIParam& prm = aiParams[m];
            REP(cell, NN) {
                const double v = (double)gc.Vs[cell];
                cache.neutralScores[m][cell] = v * prm.wa;
                cache.growScores[m][cell] = v * prm.wb;
                cache.finishAttackScores[m][cell] = v * prm.wc;
                cache.chipAttackScores[m][cell] = v * prm.wd;
            }
        }
    }

    void CalcAIMoveProbByCache(const GameConst& gc, const State& state, int player, const AIParam& prm,
                               const AIScoreCache& cache, NNBox<double>& outProb) const {
        outProb.fill(0.0);
        cauto& movable = state.movableSets_[player].RefArray();
        const int movableCount = movable.size();
        VASSERT(movableCount > 0);

        array<double, NN> scores;
        double maxScore = -1e100;
        REP(i, movableCount) {
            const auto cell = movable[i];
            const auto owner = state.owners_[cell];
            const auto level = state.levels_[cell];

            double score = 0.0;
            if (owner < 0) {
                score = cache.neutralScores[player][cell];
            }
            else if (owner == player) {
                score = (level < gc.U) ? cache.growScores[player][cell] : 0.0;
            }
            else {
                score = (level == 1) ? cache.finishAttackScores[player][cell] : cache.chipAttackScores[player][cell];
            }

            scores[i] = score;
            maxScore = max(maxScore, score);
        }

        const double threshold = maxScore - 1e-9 * max(1.0, maxScore);
        int bestCount = 0;
        REP(i, movableCount) {
            if (scores[i] >= threshold) {
                ++bestCount;
            }
        }
        VASSERT(bestCount > 0);

        const double eps = clamp(prm.eps, 0.0, 1.0);
        const double randomPart = eps / (double)movableCount;
        const double greedyPart = (1.0 - eps) / (double)bestCount;

        REP(i, movableCount) {
            const auto cell = movable[i];
            outProb[cell] = randomPart;
            if (scores[i] >= threshold) {
                outProb[cell] = randomPart + greedyPart;
            }
        }
    }

    void InitNodeActions(const GameConst& gc, const EnemyAIParamEstimatorManager& enemyAiEstimator,
                         const MArr<AIParam>& aiParams, const AIScoreCache& aiScoreCache, Node& node, int depth,
                         int turnNow) const {
        node.initialized = true;
        node.activeActionCount = 0;
        node.maxActionCount = 0;

        cauto& movable = node.state.movableSets_[0].RefArray();

        BuildPlayersProbMaps(gc, node.state, depth, enemyAiEstimator, aiParams, aiScoreCache, node.cachedPlayersProbs);

        struct ScoredCell {
            double pwScore = 0.0;
            double priorScore = 0.0;
            int cell = -1;
        };
        NNArr<ScoredCell> scoredCells;
        NNArr<double> evals;
        EvalAction(gc, depth, turnNow, node.state, aiParams, node.cachedPlayersProbs, movable, evals);

        for (int i = 0; i < (int)movable.size(); i++) {
            const int cell = movable[i];
            const double rawEval = evals[i];
            ScoredCell scored{};
            scored.cell = cell;
            scored.pwScore = ConvertEvalForPW(gc, cell, rawEval);
            scored.priorScore = ConvertEvalForPrior(rawEval);
            scoredCells.push(scored);
        }

        sort(scoredCells.begin(), scoredCells.end(), [](const ScoredCell& a, const ScoredCell& b) {
            if (fabs(a.pwScore - b.pwScore) > 1e-9) {
                return a.pwScore > b.pwScore;
            }
            return a.cell < b.cell;
        });

        const int limit = min(kMaxActionsPerNode, CalcNodeActionLimit(depth, turnNow, scoredCells.size()));
        node.maxActionCount = limit;
        node.actions.clear();

        NNArr<double> priorProb;
        priorProb.assign(limit, 0.0);
        if (limit > 0) {
            const double bestScore = scoredCells[0].priorScore;
            double sumWeight = 0.0;
            REP(i, limit) {
                const double z = clamp((scoredCells[i].priorScore - bestScore) / HP.Decider_PuctPriorTemp, -20.0, 20.0);
                const double w = exp(z);
                priorProb[i] = w;
                sumWeight += w;
            }
            if (sumWeight > 1e-12) {
                REP(i, limit) {
                    priorProb[i] /= sumWeight;
                }
            }
            else {
                const double uniform = 1.0 / (double)limit;
                REP(i, limit) {
                    priorProb[i] = uniform;
                }
            }
        }

        REP(i, limit) {
            ActionStat action{};
            action.cell = scoredCells[i].cell;
            action.prior = priorProb[i];
            node.actions.push(action);
        }

        node.activeActionCount = CalcInitialActiveActionCount(depth, node.maxActionCount);
    }

    int SelectActionByPuct(const Node& node) const {
        const int actionCount = min(node.activeActionCount, node.actions.size());
        VASSERT(actionCount > 0);

        int bestUnvisitedIdx = -1;
        double bestUnvisitedPrior = -1e100;
        REP(i, actionCount) {
            const ActionStat& action = node.actions[i];
            if (action.visits > 0) {
                continue;
            }
            if (action.prior > bestUnvisitedPrior + 1e-12) {
                bestUnvisitedPrior = action.prior;
                bestUnvisitedIdx = i;
            }
            else if (fabs(action.prior - bestUnvisitedPrior) <= 1e-12 && bestUnvisitedIdx >= 0) {
                if (action.cell < node.actions[bestUnvisitedIdx].cell) {
                    bestUnvisitedIdx = i;
                }
            }
        }
        if (bestUnvisitedIdx >= 0) {
            return bestUnvisitedIdx;
        }

        const double sqrtVisits = sqrt((double)max(1, node.visits));
        int bestIdx = 0;
        double bestScore = -1e100;

        REP(i, actionCount) {
            const ActionStat& action = node.actions[i];
            const double qRaw = (action.visits > 0) ? (action.valueSum / (double)action.visits) : 0.0;
            const double q = ConvertQForPuct(qRaw);
            const double u = HP.Decider_PuctExplore * action.prior * sqrtVisits / (1.0 + (double)action.visits);
            const double puct = q + u;
            if (puct > bestScore + 1e-12) {
                bestScore = puct;
                bestIdx = i;
            }
            else if (fabs(puct - bestScore) <= 1e-12) {
                if (action.prior > node.actions[bestIdx].prior + 1e-12) {
                    bestIdx = i;
                }
            }
        }
        return bestIdx;
    }

    int SelectAction(const Node& node) const {
        return SelectActionByPuct(node);
    }

    void ApplySampledTurn(const GameConst& gc, const State& state, int depth, int myMove,
                          const EnemyAIParamEstimatorManager& enemyAiEstimator, const MArr<AIParam>& aiParams,
                          const MArr<NNBox<double>>& playersProbs, State& nextState, Xor64& rand,
                          MArr<s8>* sampledMovesOut = nullptr) const {
        MArr<s8> moves(gc.M);
        moves[0] = myMove;

        FOR(m, 1, gc.M) {
            int enemyMove = SampleByProbOnMovable(state, m, playersProbs[m], rand);
            moves[m] = enemyMove;
        }

        if (sampledMovesOut != nullptr) {
            *sampledMovesOut = moves;
        }
        ApplyMovesOneTurn(gc, state, moves, nextState);
    }

    void ApplySampledTurnInplace(const GameConst& gc, State& state, int depth, int myMove,
                                 const EnemyAIParamEstimatorManager& enemyAiEstimator, const MArr<AIParam>& aiParams,
                                 const MArr<NNBox<double>>& playersProbs, Xor64& rand) const {
        MArr<s8> moves(gc.M);
        moves[0] = myMove;

        FOR(m, 1, gc.M) {
            int enemyMove = SampleByProbOnMovable(state, m, playersProbs[m], rand);
            moves[m] = enemyMove;
        }

        ApplyMovesOneTurn(gc, state, moves);
    }

    static uint64_t NextSplitMix64(uint64_t& x) {
        x += 0x9e3779b97f4a7c15ull;
        uint64_t z = x;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
        return z ^ (z >> 31);
    }

    void InitZobristTables() {
        if (zobristInitialized_) {
            return;
        }

        uint64_t seed = 0x726573756c745f6bull;
        REP(i, NN) {
            REP(v, kZobristValueStates) {
                zobristOwnerKeys_[i][v] = NextSplitMix64(seed);
                zobristLevelKeys_[i][v] = NextSplitMix64(seed);
            }
        }
        REP(p, M_max) {
            REP(v, kZobristValueStates) {
                zobristPosKeys_[p][v] = NextSplitMix64(seed);
            }
        }
        REP(t, T + 1) {
            zobristTurnKeys_[t] = NextSplitMix64(seed);
        }
        zobristInitialized_ = true;
    }

    uint64_t GetTurnHashKey(int turn) const {
        if (0 <= turn && turn <= T) {
            return zobristTurnKeys_[turn];
        }
        uint64_t turnSeed = (uint64_t)(int64_t)turn ^ 0xd1b54a32d192ed03ull;
        return NextSplitMix64(turnSeed);
    }

    uint64_t HashState(const GameConst& gc, const State& state, int turn) {
        InitZobristTables();

        uint64_t h = 0;
        h ^= GetTurnHashKey(turn);
        REP(i, NN) {
            const uint8_t ownerKey = (uint8_t)state.owners_[i];
            const uint8_t levelKey = (uint8_t)state.levels_[i];
            h ^= zobristOwnerKeys_[i][ownerKey];
            h ^= zobristLevelKeys_[i][levelKey];
        }
        REP(p, min(gc.M, M_max)) {
            const uint8_t posKey = (uint8_t)state.poss_[p];
            h ^= zobristPosKeys_[p][posKey];
        }
        return h;
    }

    uint64_t CalcHashByDelta(const GameConst& gc, const State& prevState, const State& nextState,
                             const MArr<s8>& sampledMoves, uint64_t prevHash, int prevTurn) const {
        uint64_t h = prevHash;
        const int nextTurn = prevTurn + 1;
        h ^= GetTurnHashKey(prevTurn);
        h ^= GetTurnHashKey(nextTurn);

        REP(p, min(gc.M, M_max)) {
            const uint8_t prevPos = (uint8_t)prevState.poss_[p];
            const uint8_t nextPos = (uint8_t)nextState.poss_[p];
            if (prevPos != nextPos) {
                h ^= zobristPosKeys_[p][prevPos];
                h ^= zobristPosKeys_[p][nextPos];
            }
        }

        array<uint8_t, NN> usedCells{};
        REP(p, min(gc.M, sampledMoves.size())) {
            const int cell = (int)sampledMoves[p];
            if ((unsigned)cell >= (unsigned)NN || usedCells[cell]) {
                continue;
            }
            usedCells[cell] = 1;

            const uint8_t prevOwner = (uint8_t)prevState.owners_[cell];
            const uint8_t nextOwner = (uint8_t)nextState.owners_[cell];
            if (prevOwner != nextOwner) {
                h ^= zobristOwnerKeys_[cell][prevOwner];
                h ^= zobristOwnerKeys_[cell][nextOwner];
            }

            const uint8_t prevLevel = (uint8_t)prevState.levels_[cell];
            const uint8_t nextLevel = (uint8_t)nextState.levels_[cell];
            if (prevLevel != nextLevel) {
                h ^= zobristLevelKeys_[cell][prevLevel];
                h ^= zobristLevelKeys_[cell][nextLevel];
            }
        }
        return h;
    }

    void MakeProbMap(const GameConst& gc, const State& state, int depth,
                     const EnemyAIParamEstimatorManager& enemyAiEstimator, const MArr<AIParam>& aiParams,
                     const AIScoreCache& aiScoreCache, int m, NNBox<double>& probs) const {
        if (depth == 0) {
            probs = enemyAiEstimator.MoveProb(m);
        }
        else {
            CalcAIMoveProbByCache(gc, state, m, aiParams[m], aiScoreCache, probs);
        }
    }

    void BuildPlayersProbMaps(const GameConst& gc, const State& state, int depth,
                              const EnemyAIParamEstimatorManager& enemyAiEstimator, const MArr<AIParam>& aiParams,
                              const AIScoreCache& aiScoreCache, MArr<NNBox<double>>& playersProbs) const {
        playersProbs.resize(gc.M);
        FOR(m, 1, gc.M) {
            MakeProbMap(gc, state, depth, enemyAiEstimator, aiParams, aiScoreCache, m, playersProbs[m]);
        }
    }

    int SelectRolloutMove(const NNArr<u8>& movableCells, const NNArr<double>& evals, Xor64& rand) const {
        VASSERT(!movableCells.empty());
        const int count = (int)movableCells.size();
        if (count == 1) {
            return movableCells[0];
        }

        const int topK = min((int)HP.Decider_RolloutTopK, count);
        const double temp = HP.Decider_RolloutTemp;

        array<u8, NN> topIdxs;
        array<double, NN> topScores;
        int topSize = 0;
        REP(i, count) {
            const double score = evals[i];
            if (topSize < topK) {
                int pos = topSize;
                while (pos > 0 && score > topScores[pos - 1]) {
                    topScores[pos] = topScores[pos - 1];
                    topIdxs[pos] = topIdxs[pos - 1];
                    pos--;
                }
                topScores[pos] = score;
                topIdxs[pos] = (u8)i;
                topSize++;
                continue;
            }

            if (score <= topScores[topK - 1]) {
                continue;
            }

            int pos = topK - 1;
            while (pos > 0 && score > topScores[pos - 1]) {
                topScores[pos] = topScores[pos - 1];
                topIdxs[pos] = topIdxs[pos - 1];
                pos--;
            }
            topScores[pos] = score;
            topIdxs[pos] = (u8)i;
        }

        VASSERT(topSize == topK);
        const double topBest = topScores[0];

        array<double, NN> weights;
        double sumWeight = 0.0;
        REP(i, topK) {
            const double z = clamp((topScores[i] - topBest) / temp, -20.0, 20.0);
            const double w = exp(z);
            weights[i] = w;
            sumWeight += w;
        }
        if (sumWeight <= 1e-12) {
            return movableCells[topIdxs[0]];
        }

        const double r = rand.GetDouble() * sumWeight;
        double acc = 0.0;
        REP(i, topK) {
            acc += weights[i];
            if (r <= acc) {
                return movableCells[topIdxs[i]];
            }
        }
        return movableCells[topIdxs[topK - 1]];
    }

    void EvalAction(const GameConst& gc, int depth, int turnNow, const State& state, const MArr<AIParam>& aiParams,
                    const MArr<NNBox<double>>& playersProbs, const NNArr<u8>& movable, NNArr<double>& evals) const {
        EvalAction3B(gc, turnNow + depth, state, playersProbs, aiParams, movable, evals);
    }

    double EvalState(const GameConst& gc, int turnNow, const State& state, const MArr<AIParam>& aiParams) const {
        return ::EvalState4(gc, turnNow, state, aiParams, HP.Decider5B_RawScoreTurn);
    }

    double Rollout(const GameConst& gc, State cur, int depth, int turnNow, int horizon,
                   const EnemyAIParamEstimatorManager& enemyAiEstimator, const MArr<AIParam>& aiParams,
                   const AIScoreCache& aiScoreCache, Xor64& rand) const {
        MArr<NNBox<double>> playersProbs;

        while (depth < horizon) {
            cauto& movable = cur.movableSets_[0].RefArray();

            BuildPlayersProbMaps(gc, cur, depth, enemyAiEstimator, aiParams, aiScoreCache, playersProbs);

            NNArr<double> evals;
            EvalAction(gc, depth, turnNow, cur, aiParams, playersProbs, movable, evals);
            const int myMove = SelectRolloutMove(movable, evals, rand);

            ApplySampledTurnInplace(gc, cur, depth, myMove, enemyAiEstimator, aiParams, playersProbs, rand);
            depth++;
        }
        return EvalState(gc, turnNow + depth, cur, aiParams);
    }

    double Simulate(const GameConst& gc, Node* nodePtr, int horizon, int depth, int turnNow,
                    const EnemyAIParamEstimatorManager& enemyAiEstimator, const MArr<AIParam>& aiParams,
                    const AIScoreCache& aiScoreCache, Xor64& rand) {
        Node& node = *nodePtr;

        if (depth >= horizon) {
            const double value = EvalState(gc, turnNow + depth, node.state, aiParams);
            node.visits++;
            node.valueSum += value;
            return value;
        }

        if (!node.initialized) {
            InitNodeActions(gc, enemyAiEstimator, aiParams, aiScoreCache, node, depth, turnNow);
        }
        VASSERT(!node.actions.empty());

        UpdateActionSetByProgressiveWidening(node);
        VASSERT(node.activeActionCount > 0);

        const int actionIdx = SelectAction(node);
        ActionStat& action = node.actions[actionIdx];

        State nextState;
        MArr<s8> sampledMoves;
        ApplySampledTurn(gc, node.state, depth, action.cell, enemyAiEstimator, aiParams, node.cachedPlayersProbs,
                         nextState, rand, &sampledMoves);

        const int curTurn = turnNow + depth;
        const uint64_t key = CalcHashByDelta(gc, node.state, nextState, sampledMoves, node.hash, curTurn);
        double value = 0.0;

        if (int* handlePtr = hashToNodeHandle_.try_get(key)) {
            int handle = *handlePtr;
            Node* nextNode = pool_.FromHandle<Node>(handle);

            value = Simulate(gc, nextNode, horizon, depth + 1, turnNow, enemyAiEstimator, aiParams, aiScoreCache, rand);
        }
        else {

            Node* nextNode = pool_.Malloc<Node>();
            int nextHandle = pool_.ToHandle(nextNode);
            ResetNode(*nextNode, nextState, key);
            hashToNodeHandle_.set(key, nextHandle);

            action.branchCount++;
            value = Rollout(gc, nextState, depth + 1, turnNow, horizon, enemyAiEstimator, aiParams, aiScoreCache, rand);

            nextNode->visits = 1;
            nextNode->valueSum = value;
        }

        node.visits++;
        node.valueSum += value;
        action.visits++;
        action.valueSum += value;
        return value;
    }
};

struct AgentDecider5 : public IAgent {
    MoveTargetDecider5B moveTargetDecider_{};

    AgentDecider5() {
        moveTargetDecider_.Init();
    }

    virtual void Calc(ChronoTimer& timer, const GameConst& gc, const Game& game,
                      const EnemyAIParamEstimatorManager& enemyAiEstimator, NNBox<float>& probs, u8& commands,
                      Xor64& rand_) {
        State state;
        state.Init(gc, game);
        commands = moveTargetDecider_.DecideTargetAndProb(timer, gc, state, enemyAiEstimator, game.turn_, probs, rand_);
    }

    void SetSimCount(int simCount) {
        moveTargetDecider_.SetSimCount(simCount);
    }

    virtual void SetNoise(const NNBox<float>& probNoizse) {
    }

    void Finalize() {
    }
};


struct AgentWrapper {
    Xor64 rand_;
    unique_ptr<IAgent> agent_;

    void Init() {
        auto agent = make_unique<AgentDecider5>();
        agent_ = move(agent);
    }

    int Run(ChronoTimer& timer, const GameConst& gc, const Game& game,
            const EnemyAIParamEstimatorManager& enemyAiEstimator, int turnNow, Xor64& rand) {
        NNBox<float> probs;
        u8 command;
        agent_->Calc(timer, gc, game, enemyAiEstimator, probs, command, rand_);
        return command;
    }

    void Finalize() {
        agent_->Finalize();
    }
};

struct Solver {
    EnemyAIParamEstimatorManager enemyAiEstimator_{};
    AgentWrapper agentWrapper_;

    void Run(ChronoTimer& timer, const GameConst& gc, Xor64& rand) {
        agentWrapper_.Init();

        Greedy(timer, gc, rand);
    }

    void Greedy(ChronoTimer& timer, const GameConst& gc, Xor64& rand) {
        auto& game = server.game_;

        enemyAiEstimator_.Init(gc, rand);
        FOR(m, 1, gc.M) {
            AIParam prm;
            if (server.TryGetOracleParam(m, prm)) {
                enemyAiEstimator_.SetOracleParam(m, prm);
            }
        }
        enemyAiEstimator_.RefreshMoveProbs(gc, game.owners_, game.levels_, game.poss_);

        ChronoTimer turnTimer;

        REP(t, T) {
            double timeLeft = TIME_LIMIT_US - timer.ElapseTimeUs();
            int turnLeft = T - t;
            int curTime = (int)(timeLeft / turnLeft);
            turnTimer.Init();
            turnTimer.StartUs(curTime);

            int cell = agentWrapper_.Run(turnTimer, gc, server.game_, enemyAiEstimator_, t, rand);
            const auto beforeGame = game;

            MArr<s8> playersTarget;
            server.Output(cell, playersTarget);
            enemyAiEstimator_.Update(gc, beforeGame.owners_, beforeGame.levels_, beforeGame.poss_, playersTarget, rand);

            enemyAiEstimator_.RefreshMoveProbs(gc, game.owners_, game.levels_, game.poss_);
        }


        agentWrapper_.Finalize();
    }
};

struct Main {
    void Run(int argc, const char* argv[]) {
        VASSERT(argc == 1);
        ChronoTimer timer;
        server.InitInput(timer);

        timer.StartMs(TIME_LIMIT);

        Xor64 rand;
        static thread_local Solver solver;
        solver.Run(timer, server.gc_, rand);

        server.Finalize();

    }
};
